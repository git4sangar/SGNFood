/*
 * HttpClient.h
 *
 *  Created on: 16-Feb-2020
 *      Author: tstone10
 */

#ifndef HTTPCLIENT_H_
#define HTTPCLIENT_H_

#include <pthread.h>
#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <memory>
#include <utility>
#include <curl/curl.h>

#define	HTTP_REQ_TYPE_DWLD	(1)
#define	HTTP_REQ_TYPE_UPLD	(2)
#define HTTP_REQ_TYPE_GET	(3)
#define HTTP_REQ_TYPE_POST	(4)

#define MAX_BUFF_SIZE       (16 * ONE_KB)

class HttpResponse {
public:
    HttpResponse(){};
    virtual ~HttpResponse(){};
    virtual void onDownloadSuccess(int64_t iChatId, unsigned int iOrderNo, std::string strResp, FILE *fp) = 0;
    virtual void onDownloadFailure(int64_t iChatId, unsigned int iOrderNo, FILE *fp) = 0;
};

class HttpReqPkt;
class HttpClient : public std::enable_shared_from_this<HttpClient> {
    std::shared_ptr<HttpResponse> pListener;
    FILE *fp;
    std::string strResp;

public:
    HttpClient(FILE *pFP) : fp(pFP) {}
    virtual ~HttpClient() {}
    void postReqFormData(std::string strUrl, std::map<std::string, std::string> formData, int64_t iChatId, unsigned int iOrderNo);

    void subscribeListener(std::shared_ptr<HttpResponse> pObj) { pListener = pObj; }
    FILE *getFilePtr() { return fp; }
    std::shared_ptr<HttpClient> getSharedPtr() { return shared_from_this();}

    static size_t write_function(char *ptr, size_t size, size_t nmemb, void *userdata);
    static void *run(void *pThis);
    static HttpReqPkt *readFromQ();
    void pushToQ(HttpReqPkt *pReqPkt);

    static std::queue<HttpReqPkt *> reqQ;
    static pthread_mutex_t mtxgQ;
    static pthread_cond_t mtxgCond;
};

class HttpReqPkt {
	std::vector<std::string> headers;
	std::string strUrl, strData;
	unsigned int reqType, iOrderNo;
    int64_t iChatId;
	std::shared_ptr<HttpClient> pHttpClient;
	std::map<std::string, std::string> mapFormData;

public:
	HttpReqPkt() : reqType {HTTP_REQ_TYPE_POST}, iOrderNo{0}, iChatId{0} {}
	virtual ~HttpReqPkt() {}

	std::vector<std::string> getHeaders() { return headers; }
	std::string getUrl() { return strUrl; }
	unsigned int getReqType() { return reqType; }
	unsigned int getOrderNo() { return iOrderNo; }
	int64_t getChatId() { return iChatId; }
	std::string getUserData() { return strData; }
	std::shared_ptr<HttpClient> getClient() { return pHttpClient; }
	std::map<std::string, std::string> getFormData() { return mapFormData; }

	void addHeader(std::string strHdr) { headers.push_back(strHdr); }
	void setUrl(std::string url) { strUrl = url; }
	void setReqType(unsigned int iReqType) { reqType = iReqType; }
	void setOrderNo(unsigned int iNo)   { iOrderNo = iNo; }
	void setChatId(int64_t iId)    { iChatId = iId; }
	void setUserData(std::string strCnt) { strData = strCnt; }
	void setClient(std::shared_ptr<HttpClient> pClient) { pHttpClient = pClient; }
	void setFormData(std::map<std::string, std::string> frmData) { mapFormData = frmData; }
};

/*
 * void uploadLogs(int cmdNo, std::string fileName, std::string strData);
 * void smartMeterUpdate(int cmdNo, std::string strUrl);
 * void jabberClientUpdate(int cmdNo, std::string strUrl);
 * void watchDogUpdate(int cmdNo, std::string strUrl);
 * */

#endif /* HTTPCLIENT_H_ */
