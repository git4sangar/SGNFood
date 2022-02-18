/*
 * HttpClient.cpp
 *
 *  Created on: 16-Feb-2020
 *      Author: tstone10
 */

#include <pthread.h>
#include <queue>
#include <string.h>
#include <curl/curl.h>
#include "HttpClient.h"
#include <sstream>
#include <unistd.h>
#include <curl/curl.h>

#include "Constants.h"

std::queue<HttpReqPkt *> HttpClient::reqQ;
pthread_mutex_t HttpClient::mtxgQ   = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t HttpClient::mtxgCond = PTHREAD_COND_INITIALIZER;

void HttpClient::postReqFormData(std::string strUrl, std::map<std::string, std::string> formData, int64_t iChatId, unsigned int iOrderNo) {
	HttpReqPkt *pRqPkt	= new HttpReqPkt();

	pRqPkt->setClient(getSharedPtr());
	pRqPkt->setChatId(iChatId);
	pRqPkt->setReqType(HTTP_REQ_TYPE_POST);
	pRqPkt->setUrl(strUrl);
	pRqPkt->setOrderNo(iOrderNo);
	pRqPkt->setFormData(formData);

	pRqPkt->addHeader(std::string("Content-Type: application/x-www-form-urlencoded"));

	pushToQ(pRqPkt);
}

void HttpClient :: pushToQ(HttpReqPkt *pReqPkt) {
	if(pReqPkt) {
		pthread_mutex_lock(&mtxgQ);
		reqQ.push(pReqPkt);
		pthread_cond_signal(&mtxgCond);
		pthread_mutex_unlock(&mtxgQ);
	}
}

HttpReqPkt *HttpClient :: readFromQ() {
	HttpReqPkt *pReqPkt;
    pthread_mutex_lock(&mtxgQ);
    if(reqQ.empty()) {
    	pthread_cond_wait(&mtxgCond, &mtxgQ);
    }
    pReqPkt	= reqQ.front();
    reqQ.pop();
    pthread_mutex_unlock(&mtxgQ);
    return pReqPkt;
}

void *HttpClient :: run(void *pFilePtr) {
	HttpReqPkt *pReqPkt	= NULL;
    long respCode = 0;
    FILE *fp;
    CURLcode res = CURLE_OK;
    struct curl_slist* curl_hdrs;
    char pPostPayLoad[MAX_BUFF_SIZE];
    std::shared_ptr<HttpClient> pThis;

    curl_mime *form = NULL;
	curl_mimepart *field = NULL;
	std::map<std::string, std::string> formData;
	std::map<std::string, std::string>::iterator itrFormData;

    fp  = reinterpret_cast<FILE *>(pFilePtr);
    fprintf(fp, "HttpClient: Curl thread started\n"); fflush(fp);

    std::string strCAFile		= std::string(BOT_ROOT_PATH) + std::string(CA_CERT_FILE);
    while(1) {
    	form = NULL; field = NULL; curl_hdrs	= NULL; formData.clear();
    	pReqPkt	= HttpClient::readFromQ();
    	pThis   = pReqPkt->getClient();
    	fprintf(fp, "HttpClient: Got HTTP req: %s\n", pReqPkt->getUrl().c_str()); fflush(fp);

        CURL *curl  = curl_easy_init();
        if(curl && pThis->pListener) {
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 0L);
            curl_easy_setopt(curl, CURLOPT_URL, pReqPkt->getUrl().c_str());

            //  Set the header
            std::vector<std::string> headers	= pReqPkt->getHeaders();
            for(std::string header : headers) {
            	curl_hdrs = curl_slist_append(curl_hdrs, header.c_str());
            }
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_hdrs);

            //  For verifying server
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
            curl_easy_setopt(curl, CURLOPT_CAINFO, strCAFile.c_str());

            switch(pReqPkt->getReqType()) {
				case HTTP_REQ_TYPE_POST:
				    curl_easy_setopt(curl, CURLOPT_POST, 1L);
					curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_function);
					curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&pThis->strResp);

					formData    = pReqPkt->getFormData();
					std::stringstream ss; std::string prefix;
					for(prefix.clear(), itrFormData = formData.begin(); itrFormData != formData.end(); itrFormData++, prefix = "&") {
                        //fprintf(fp, "%s : %s\n", itrFormData->first.c_str(), itrFormData->second.c_str()); fflush(fp);
                        ss << prefix << itrFormData->first << "=";

                        strcpy(pPostPayLoad, itrFormData->second.c_str());
                        ss <<  curl_easy_escape(curl, pPostPayLoad, 0);
					}
					strcpy(pPostPayLoad, ss.str().c_str());
					curl_easy_setopt(curl, CURLOPT_POSTFIELDS, pPostPayLoad);
					break;
            }

            res = curl_easy_perform(curl);

            CURLcode infoResp = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &respCode);
            if(CURLE_OK == res && CURLE_OK == infoResp && respCode >= 200 && respCode <= 299) {
                fprintf(fp, "HttpClient: ChatId: %d, Response Data: %s, Response code %ld\n",
                        pReqPkt->getOrderNo(), pThis->strResp.c_str(), respCode); fflush(fp);
                pThis->pListener->onDownloadSuccess(pReqPkt->getChatId(), pReqPkt->getOrderNo(), pThis->strResp, fp);
            } else {
				fprintf(fp, "HttpClient: Error: ChatId %d, Res %d, infoResp %d, respCode %ld, response %s\n",
				        pReqPkt->getOrderNo(), res, infoResp, respCode, pThis->strResp.c_str()); fflush(fp);
                pThis->pListener->onDownloadFailure(pReqPkt->getChatId(), pReqPkt->getOrderNo(), fp);
            }
        }
        if(curl_hdrs) {curl_slist_free_all(curl_hdrs); curl_hdrs = NULL; }
        if(curl) curl_easy_cleanup(curl);
        delete pReqPkt;
    }
    return NULL;
}

size_t HttpClient :: write_function(char *ptr, size_t size, size_t nmemb, void *userdata) {
    std::string *pStrRsp    = reinterpret_cast<std::string *>(userdata);
    int iTotal = (size*nmemb);

    //  Notification app sends packets of 0 length. So skip those.
    if(0 < iTotal) {
        char *pBuffer   = new char[1+iTotal];
        memcpy(pBuffer, ptr, iTotal);
        pBuffer[iTotal] = '\0';

        std::stringstream ss;
        ss << *pStrRsp << pBuffer;
        *pStrRsp = ss.str();

        delete[] pBuffer;
    }
    return iTotal;
}


/*
 * void HttpClient::uploadLogs(int cmdNo, std::string fileName, std::string strLogData) {
    if(strLogData.empty() || 0 > cmdNo) {
        info_log << "HttpClient: Log content empty" << std::endl;
        return;
    }

    HttpReqPkt *pRqPkt  = new HttpReqPkt();
    pRqPkt->setReqType(HTTP_REQ_TYPE_UPLD);
    pRqPkt->setUrl(Config::getInstance()->getLogUploadURL());
    std::string strFile = std::string(TECHNO_SPURS_ROOT_PATH) + fileName +
                            Utils::getYYYYMMDD_HHMMSS();
    pRqPkt->setTgtFile(strFile);
    pRqPkt->setCmdNo(cmdNo);
    pRqPkt->setCmd("upload_logs");

    //  Prepare the headers
    std::string strUniqID   = Config::getInstance()->getRPiUniqId();
    pRqPkt->addHeader(std::string("Content-Type: multipart/form-data"));
    pRqPkt->addHeader(std::string("module:") + std::string(MODULE_NAME));
    pRqPkt->addHeader(std::string("panel:") + strUniqID);

    pRqPkt->setUserData(strLogData);

    info_log << "HttpClient: Making log upload request" << std::endl;
    pushToQ(pRqPkt);
}

void HttpClient::smartMeterUpdate(int cmdNo, std::string strUrl) {
    if(strUrl.empty() || 0 > cmdNo) {
        fprintf(fp, "HttpClient: Smart meter update URL empty\n"); fflush(fp);
        return;
    }
    HttpReqPkt *pRqPkt  = genericUpdate(strUrl, std::string(TECHNO_SPURS_APP_FOLDER), "smart_meter_update", cmdNo);
    fprintf(fp, "HttpClient: Making Smart meter update request with CmdNo: %d\n", cmdNo); fflush(fp);
    pushToQ(pRqPkt);
}

void HttpClient::jabberClientUpdate(int cmdNo, std::string strUrl) {
    if(strUrl.empty() || 0 > cmdNo) {
        info_log << "HttpClient: Jabber client update URL empty" << std::endl;
        return;
    }
    HttpReqPkt *pRqPkt  = genericUpdate(strUrl, std::string(TECHNO_SPURS_JABBER_FOLDER), "jabber_client_update", cmdNo);
    info_log << "HttpClient: Making Jabber client update request with CmdNo: " << cmdNo << std::endl;
    pushToQ(pRqPkt);
}

HttpReqPkt *HttpClient::genericUpdate(std::string strUrl, std::string strFolder, std::string strCmd, int cmdNo) {
    HttpReqPkt *pRqPkt  = new HttpReqPkt();
    pRqPkt->setReqType(HTTP_REQ_TYPE_DWLD);
    pRqPkt->setUrl(strUrl);
    pRqPkt->setTgtFile(strFolder);
    pRqPkt->setCmdNo(cmdNo);
    pRqPkt->setCmd(strCmd);
    pRqPkt->addHeader(std::string("Content-Type: application/zip"));
    return pRqPkt;
}

void HttpClient::watchDogUpdate(int cmdNo, std::string strUrl) {
    if(strUrl.empty() || 0 > cmdNo) {
        info_log << "HttpClient: Watchdog update update URL empty" << std::endl;
        return;
    }
    HttpReqPkt *pRqPkt  = genericUpdate(strUrl, std::string(TECHNO_SPURS_WDOG_FOLDER), "watchdog_update", cmdNo);
    info_log << "HttpClient: Making Watchdog update request with CmdNo: " << cmdNo << std::endl;
    pushToQ(pRqPkt);
}

                //  We don't need the file anymore. So deleting the same
                if(HTTP_REQ_TYPE_UPLD == pReqPkt->getReqType()) {
                    unlink(file_name.c_str());
                    std::stringstream ss;
                    ss << "{ \"from\" : \"" << strUnqId << "\" ,\"command\" : \""
                            << pReqPkt->getCmd() << "\", \"success\" : true, \"command_no\" : "
                            << pReqPkt->getCmdNo() << " }" << std::endl;
                    pJbrCli->sendMsgTo(ss.str(), cPanelJid);
                }
                /*case HTTP_REQ_TYPE_DWLD:
                    file_name   = std::string(TECHNO_SPURS_ROOT_PATH) + std::string(TECHNO_SPURS_DOWNLOAD_FILE);
                    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_function);
                    write_data  = fopen(file_name.c_str(), "wb");
                    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)write_data);
                    break;

                case HTTP_REQ_TYPE_UPLD:
                    file_name   = pReqPkt->getTgtFile();
                    write_data  = fopen(file_name.c_str(), "w");
                    if(write_data) {
                        fwrite(pReqPkt->getUserData().c_str(), 1, pReqPkt->getUserData().length(), write_data);
                        fclose(write_data);
                        write_data = NULL;
                    }
                    form    = curl_mime_init(curl);
                    field   = curl_mime_addpart(form);
                    curl_mime_name(field, "file");
                    curl_                mime_filedata(field, file_name.c_str());
                    curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
                    break;
    size_t HttpClient :: write_function(char *ptr, size_t size, size_t nmemb, void *userdata) {
        size_t ret  = 0;
        FILE *fp    = reinterpret_cast<FILE *>(userdata);

        size_t iTotal = (size * nmemb);
        if(0 < iTotal) {
            ret = fwrite(ptr, 1, iTotal, fp);
        }
        return ret;
    }

*/
