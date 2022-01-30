/*sgn
 * Daemon.cpp
 *
 *  Created on: 02-Jul-2020
 *      Author: tstone10
 */


#include <OrderMgmt.h>
#include <iostream>
#include <unistd.h>     // fork()
#include <stdlib.h>     // exit()
#include <stdio.h>
#include <map>
#include <tuple>
#include <thread>
#include <pthread.h>

//  socket
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <tgbot/tgbot.h>
#include "fifo_map.h"

#include "BaseButton.h"
#include "FAQs.h"
#include "Version.h"
#include "vigenere/encrypt.h"
#include "DBInterface.h"
#include "ProductList.h"
#include "SingleOrder.h"
#include "EditMenu.h"
#include "Checkout.h"
#include "OrderMgmt.h"
#include "ViewCart.h"
#include "PriceChange.h"
#include "WalletMgmt.h"
#include "SGNAdmin.h"
#include "SGNParser.h"
#include "SpecialList.h"
#include "MyAddress.h"
#include "Constants.h"
#include "QuickMenu.h"
#include "HttpClient.h"

#define MYPORT (60000)
#define UPDATE_ID_FILE "update_id.txt"

std::map<unsigned int, UserContext> m_Context;
std::vector<unsigned int> adminChatIds;
nlohmann::fifo_map<std::string, std::string> descToCode;
pthread_mutex_t mtx_01;
std::vector< std::tuple<unsigned int, unsigned int> > inTm;
std::map<unsigned int, unsigned int> inMsgTmStamps;
std::string BotVersion::STR_MSG_DEFF_RELEASE;
//bool isAgent;

void initGlobals(FILE *fp) {
    descToCode.clear();
    descToCode[BREAKFAST]       = "TF-";
    descToCode[BISIBELEBATH]    = "TF-";
    descToCode[SAMBAR]          = "SB-";
    descToCode[MORKUZHAMBU]     = "SB-";
    descToCode[VATHAKUZHAMBU]   = "SB-";
    descToCode[RASAM]           = "RS-";
    descToCode[CURRY]           = "CR-";
    descToCode[KOOTU]           = "KT-";
    descToCode[RICE]            = "RC-";
    descToCode[LUNCH]           = "LN-";
    descToCode[SWEET]           = "SW-";
    descToCode[PICKLE]          = "PK-";
    descToCode[ULUNDU_VADAI]    = "Ulundu Vadai(2)";
    descToCode[COCONUT_POLI]    = "Coconut Poli(3)";
    descToCode[DHAL_POLI]       = "Dhall Poli(3)";

    curl_global_init(CURL_GLOBAL_ALL);

    pthread_t tCurThread;
    pthread_create(&tCurThread, NULL, &HttpClient::run, (void *)fp);
    pthread_detach(tCurThread);
}

void BaseButton::cleanup(TgBot::Message::Ptr pMsg, std::map<std::string, std::shared_ptr<BaseButton>>& lstBaseBtns, FILE *fp) {
    std::map<std::string, std::shared_ptr<BaseButton>>::const_iterator itrBtn;
    if(lstBaseBtns.end() != (itrBtn = lstBaseBtns.find(std::to_string(pMsg->chat->id)))) {
        lstBaseBtns.erase(itrBtn);

        std::map<unsigned int, UserContext>::const_iterator itrCntxt;
        if(m_Context.end() != (itrCntxt = m_Context.find(pMsg->chat->id)))
            m_Context.erase(itrCntxt);
    }
}

void BaseButton::createKBBtn(std::string strName, std::vector<TgBot::KeyboardButton::Ptr> &row,
                             std::map<std::string, std::shared_ptr<BaseButton>>& lstBaseBtns, std::shared_ptr<BaseButton> pBtn) {
    if(strName.empty()) return;

    TgBot::KeyboardButton::Ptr btn = std::make_shared<TgBot::KeyboardButton>();
    btn->text   = strName;
    row.push_back(btn);

    if(pBtn && lstBaseBtns.end() == lstBaseBtns.find(strName))
        lstBaseBtns[strName] = pBtn;
}

bool isMobileNoPresent(std::string strAddress) {
    int iDigits = 0;

    for(char &c : strAddress) {
        if(std::isspace(c)) continue;
        if(std::isdigit(c)) iDigits++;
        else iDigits = 0;

        if(MAX_MOBILE_DIGITS <= iDigits) {
            break;
        }
    }   
    return MAX_MOBILE_DIGITS <= iDigits;
}

std::string getMobileNo(std::string strAddress) {
    int iDigits = 0;
    bool isDone = false;
    std::string strMobile;

    for(char &c : strAddress) {
        if(std::isspace(c)) continue;

        if(std::isdigit(c)) { iDigits++; strMobile += c; }
        else if(isDone) break;
        else { iDigits = 0; strMobile.clear(); }

        if(MAX_MOBILE_DIGITS <= iDigits) {
            isDone = true;
        }
    }   
    return strMobile.substr(strMobile.length() - MAX_MOBILE_DIGITS);
}

void petWatchDog(FILE *fp) {
   int sockfd = 0;
   struct hostent *he;
   sockfd = socket(AF_INET, SOCK_DGRAM, 0);
   struct sockaddr_in their_addr;

   he=gethostbyname("localhost");
   their_addr.sin_family = AF_INET;      /* host byte order */
   their_addr.sin_port = htons(MYPORT);  /* short, network byte order */
   their_addr.sin_addr = *((struct in_addr *)he->h_addr);
   bzero(&(their_addr.sin_zero), 8);     /* zero the rest of the struct */
   sendto(sockfd, BIN_FILE_PATH, strlen(BIN_FILE_PATH)+1, 0,
             (struct sockaddr *)&their_addr, sizeof(struct sockaddr));
   close(sockfd);
   //fprintf(fp, "BaseBot %ld: Petting WatchDog\n", time(0)); fflush(fp);
}

void plsWaitThread(std::shared_ptr<TgBot::Bot> pBot, FILE *fp) {
    time_t curTm;
    std::vector<unsigned int> chatIds;
    std::vector< std::tuple<unsigned int, unsigned int> >::iterator itrWait;

    while(1) {
        chatIds.clear();
        pthread_mutex_lock(&mtx_01);
        for(itrWait = inTm.begin(); inTm.end() != itrWait;) {
            std::get<1>(*itrWait)   = std::get<1>(*itrWait) - 1;
            if(std::get<1>(*itrWait) <= time(0)) {
                chatIds.push_back(std::get<0>(*itrWait));
                inTm.erase(itrWait);
            } else itrWait++;
        }
        for(auto &chatId : chatIds)
            pBot->getApi().sendMessage(chatId, "<b>processing...</b>", false, 0, nullptr, "HTML");

        pthread_mutex_unlock(&mtx_01);
        sleep(1);
    }
}

void sendNotifyThread(std::shared_ptr<TgBot::Bot> pBot, DBInterface::Ptr hDB, FILE *fp) {
    std::vector<Notifs::Ptr> notifs;
    std::vector<Notifs::Ptr>::iterator itr;
    unsigned int iLoop = 0, iChatId = 0;
	//std::shared_ptr<TgBot::Bot> pBot = std::make_shared<TgBot::Bot>("1351042610:AAFJriXJPfpsZs--xaKVKp7kjVf7n7tQr7Q");	// ManiMama Bot

    while(1) {
        //  Check if any in database
        notifs = hDB->getNotifications(fp);

        if(!notifs.empty()) {
            fprintf(fp, "Got %ld notification request.\n", notifs.size()); fflush(fp);
            for(iLoop = 1, itr = notifs.begin(); notifs.end() != itr; itr++, iLoop++) {
                iChatId = (MAX_USERS > (*itr)->m_ChatId) ? adminChatIds[1] : (*itr)->m_ChatId;
                try {
                    pBot->getApi().sendMessage(iChatId, (*itr)->m_Msg, false, 0, nullptr, "HTML");
                    if(!(iLoop%10)) pBot->getApi().sendMessage(adminChatIds[1], "Pls wait sending notifications.", false, 0, nullptr, "HTML");
                } catch(std::exception &e) {
                    std::string strExcept = e.what();
                    if(std::string::npos != strExcept.find("blocked") || std::string::npos != strExcept.find("not found")) hDB->deactivate(iChatId, fp);
                    fprintf(fp, "Exception : %s, while sending notification to user.\n", strExcept.c_str()); fflush(fp);
                }
                if(itr != notifs.end())hDB->removeNotif((*itr)->m_NotifId, fp);
            }
            if(10 < iLoop) pBot->getApi().sendMessage(adminChatIds[1], "Sent all notifications.", false, 0, nullptr, "HTML");

            notifs.clear();
            fprintf(fp, "Sent all notifications.\n"); fflush(fp);
        }
        sleep(2);
    }
}

std::map<std::string, std::shared_ptr<BaseButton>>::const_iterator isEditMenu(std::map<std::string, std::shared_ptr<BaseButton>>& listBaseBtns, std::string strCmd) {
    std::map<std::string, std::shared_ptr<BaseButton>>::const_iterator retItr;
    retItr  = listBaseBtns.end();

    if( ('+' == strCmd[0] || '-' == strCmd[0]) && (std::string::npos == strCmd.substr(1).find_first_not_of("0123456789")) ) {
        retItr = listBaseBtns.find(STR_BTN_EDIT_MENU);
    }
    return retItr;
}

std::map<std::string, std::shared_ptr<BaseButton>>::const_iterator isSingleOrder(std::map<std::string, std::shared_ptr<BaseButton>>& listBaseBtns, std::string strCmd) {
    std::map<std::string, std::shared_ptr<BaseButton>>::const_iterator retItr;
    retItr  = listBaseBtns.end();
    if(std::string::npos    == strCmd.find_first_not_of("0123456789") ||
        std::string::npos   != strCmd.find(STR_BTN_CNFM_ORDER) ||
        std::string::npos   != strCmd.find(STR_BTN_CNCL_ORDER) ||
        std::string::npos   != strCmd.find(SRT_BTN_DLVR_ORDER)
        ) {
        retItr = listBaseBtns.find(STR_BTN_SINGLE_ORDER);
    }
    return retItr;
}

std::int32_t getUpdateId() {
    std::string root_path   = std::string(BOT_ROOT_PATH);
    std::string log_path    = std::string(BOT_LOG_PATH);
    std::string update_file	= root_path + log_path + UPDATE_ID_FILE;

    std::string strUpdateId("0");
    std::ifstream myfile(update_file);
	if(myfile.is_open()) {
	    myfile >> strUpdateId;
		myfile.close();
	}
    return std::stoi(strUpdateId);    
}

void setUpdateId(std::int32_t updateId) {
	static std::int32_t prev = 0;
    std::string root_path   = std::string(BOT_ROOT_PATH);
    std::string log_path    = std::string(BOT_LOG_PATH);
    std::string update_file	= root_path + log_path + UPDATE_ID_FILE;

	if(prev != updateId) {
		prev = updateId;
	    std::ofstream myfile(update_file, std::ios::trunc);
	    myfile << std::to_string(updateId);
		myfile.flush();
	    myfile.close();
	}
}

void BotMainLoop(FILE *fp) {
    std::string root_path   = std::string(BOT_ROOT_PATH);
    std::string db_path     = std::string(BOT_DB_PATH);
    std::string db_file     = std::string(BOT_DB_FILE);
    time_t startSec = time(NULL);

    fprintf(fp, "Main: %ld: Starting MainLoop\n", time(0)); fflush(fp);
    initGlobals(fp);

    //  Admin Chat Ids
    adminChatIds.push_back(550919816);      // Myself
#ifdef AURA
    adminChatIds.push_back(303802126);      // Shalini
#endif
#ifdef MANI_MAMA
    adminChatIds.push_back(1352652258);     // Santosh 1
    adminChatIds.push_back(1476972743);     // Santosh 2
#endif

    std::shared_ptr<TgBot::Bot> pBot = std::make_shared<TgBot::Bot>(BOT_TOKEN);
    std::thread recv_thread(&plsWaitThread, pBot, fp);

    DBInterface::Ptr hDB       = std::make_shared<DBInterface>(root_path + db_path + db_file, fp);
    std::map<std::string, std::shared_ptr<BaseButton>> listBaseBtns;

    std::thread notf_thread(&sendNotifyThread, pBot, hDB, fp);

    listBaseBtns["version"]             = std::make_shared<BotVersion>(hDB);
    listBaseBtns["Version"]             = listBaseBtns["version"];

    listBaseBtns[STR_BTN_PRODUCT_LIST]  = std::make_shared<ProductList>(hDB);
    listBaseBtns[STR_BTN_MAINMENU]      = listBaseBtns[STR_BTN_PRODUCT_LIST];
    listBaseBtns["/start"]              = listBaseBtns[STR_BTN_MAINMENU];
    listBaseBtns["start"]               = listBaseBtns[STR_BTN_MAINMENU];
    listBaseBtns[STR_BTN_EMPTY_CART]    = listBaseBtns[STR_BTN_MAINMENU];
    listBaseBtns[STR_BTN_FAQ]           = std::make_shared<FAQs>(hDB);

    listBaseBtns[STR_BTN_FUND_ME]       = std::make_shared<SpecialList>(hDB);
    listBaseBtns[STR_BTN_VIEW_CART]     = std::make_shared<ViewCart>(hDB);
    listBaseBtns[STR_BTN_REMOVE]        = listBaseBtns[STR_BTN_VIEW_CART];

    listBaseBtns[STR_BTN_CHECKOUT]      = listBaseBtns[STR_BTN_MAINMENU];
    listBaseBtns[STR_BTN_TOP_UP]        = std::make_shared<Checkout>(hDB);

    listBaseBtns[STR_BTN_SHPG_ADDRESS]  = std::make_shared<MyAddress>(hDB);
    listBaseBtns[STR_BTN_CHANGE_ADDRESS]= listBaseBtns[STR_BTN_SHPG_ADDRESS];

    listBaseBtns[STR_BTN_ORDERS]        = std::make_shared<OrderMgmt>(hDB);
    listBaseBtns[STR_BTN_NEW_ORDERS]    = listBaseBtns[STR_BTN_ORDERS];
    listBaseBtns[STR_BTN_CNF_ORDERS]    = listBaseBtns[STR_BTN_ORDERS];
    listBaseBtns[STR_BTN_YOUR_ORDERS]   = listBaseBtns[STR_BTN_ORDERS];
    listBaseBtns[STR_BTN_DLVRD_ORDERS]  = listBaseBtns[STR_BTN_ORDERS];
    listBaseBtns[STR_BTN_CNCLD_ORDERS]  = listBaseBtns[STR_BTN_ORDERS];

    listBaseBtns[STR_BTN_MENU_MGMT]     = std::make_shared<PriceChange>(hDB);
    listBaseBtns[STR_BTN_EDIT_PRICE]    = listBaseBtns[STR_BTN_MENU_MGMT];
    listBaseBtns[STR_BTN_ITEM_LIST]     = listBaseBtns[STR_BTN_MENU_MGMT];
    listBaseBtns[STR_BTN_ADD_PROD]      = listBaseBtns[STR_BTN_MENU_MGMT];

    listBaseBtns[STR_BTN_EDIT_MENU]     = std::make_shared<EditMenu>(hDB);
    listBaseBtns[STR_BTN_SHOW_MENU]     = listBaseBtns[STR_BTN_EDIT_MENU];
    listBaseBtns[STR_BTN_DONE_MENU]     = listBaseBtns[STR_BTN_EDIT_MENU];

    listBaseBtns[STR_BTN_SINGLE_ORDER]  = std::make_shared<SingleOrder>(hDB);
    listBaseBtns[STR_BTN_CNFM_ORDER]    = listBaseBtns[STR_BTN_SINGLE_ORDER];
    listBaseBtns[STR_BTN_CNCL_ORDER]    = listBaseBtns[STR_BTN_SINGLE_ORDER];

    listBaseBtns[STR_BTN_NEW_TOPUPs]    = std::make_shared<WalletMgmt>(hDB);
    listBaseBtns[STR_BTN_CNF_TOPUPs]    = listBaseBtns[STR_BTN_NEW_TOPUPs];
    listBaseBtns[STR_BTN_CNCLD_TOPUPs]  = listBaseBtns[STR_BTN_NEW_TOPUPs];

    listBaseBtns[STR_BTN_QUICK_MENU]    = std::make_shared<QuickMenu>(hDB);
    listBaseBtns[STR_BTN_ADMIN_PG]      = std::make_shared<SGNAdmin>(hDB);
    listBaseBtns[STR_BTN_SEND_MSG]      = listBaseBtns[STR_BTN_ADMIN_PG];
    listBaseBtns[STR_BTN_PARSER]        = std::make_shared<SGNParser>(hDB);

    pBot->getEvents().onAnyMessage( [pBot, &listBaseBtns, fp, &startSec](TgBot::Message::Ptr pMsg) {
        petWatchDog(fp);
        static bool isSkipOver = false;

        fprintf(fp, "BaseBot %ld: Received \"%s\" for chatId: %ld onAnyMessage as it arrived\n", time(0), pMsg->text.c_str(), pMsg->chat->id); fflush(fp);
        std::shared_ptr<BaseButton> pBaseBtn = nullptr;
        std::string strCmd = pMsg->text;

        // Skip everything for SKIP_INTERVAL secs
        if(!isSkipOver && (startSec + SKIP_INTERVAL) > time(NULL)) {
            std::stringstream ss; ss << "<b>Hi " << pMsg->chat->firstName << ", I was a bit busy, missed your last click. Click again pls..?</b>";
            pBot->getApi().sendMessage(pMsg->chat->id, ss.str(), false, 0, nullptr, "HTML");
            fprintf(fp, "BaseBot %ld: Skipping %s\n", time(0), pMsg->text.c_str()); fflush(fp);
            return;
        }
        isSkipOver = true;

        fprintf(fp, "BaseBot %ld: BaseBtnList Size %ld \n", time(0),  listBaseBtns.size()); fflush(fp);

        listBaseBtns[STR_BTN_PARSER]->init(pMsg, fp);
        std::map<std::string, std::shared_ptr<BaseButton>>::const_iterator itrBBtn;
        if( listBaseBtns.end() != (itrBBtn = listBaseBtns.find(std::to_string(pMsg->chat->id))) ||
            listBaseBtns.end() != (itrBBtn = listBaseBtns.find(strCmd)) ||
            listBaseBtns.end() != (itrBBtn = isEditMenu(listBaseBtns, strCmd)) ||
            listBaseBtns.end() != (itrBBtn = isSingleOrder(listBaseBtns, strCmd)) ||
            listBaseBtns.end() != (itrBBtn = listBaseBtns[STR_BTN_PARSER]->parse(pMsg->text, listBaseBtns, pMsg->chat->id, fp))) {
            fprintf(fp, "BaseBot %ld: Found \"%s\" button\n", time(0), pMsg->text.c_str()); fflush(fp);
            pBaseBtn = itrBBtn->second->getSharedPtr();
        } else {
            fprintf(fp, "BaseBot %ld: \"%s\" button missing\n", time(0), pMsg->text.c_str()); fflush(fp);
            pBaseBtn    = listBaseBtns[STR_BTN_MAINMENU];
        }

        // Initialize local data
        pBaseBtn->init(pMsg, fp);

        // On Click
        pBaseBtn->onClick(pMsg, fp);

        //  Prepare the Keyboards
        TgBot::GenericReply::Ptr pMenu = pBaseBtn->prepareMenu(listBaseBtns, pMsg, fp);

        // Send Snaps if any
        TgBot::InputFile::Ptr pFile;
        std::vector<TgBot::InputFile::Ptr> pFiles;

        pFile = pBaseBtn->getMedia(pMsg, fp);
        if(!pFile) { pFiles = pBaseBtn->getMedias(pMsg, fp);}

        if(pBaseBtn->isPlsWait()) {
            pthread_mutex_lock(&mtx_01);
            inTm.push_back(std::make_tuple(pMsg->chat->id, time(0) + MAX_WAIT_B4_RESP));
            pthread_mutex_unlock(&mtx_01);
        }

        int iLoop = 0, iRetry = 0;
        while(iRetry <= MAX_PIC_SEND_RETRIES) {
            try {
                if(pFile)   pBot->getApi().sendPhoto(pMsg->chat->id, pFile);
                else while(iLoop < pFiles.size()) {petWatchDog(fp); pBot->getApi().sendPhoto(pMsg->chat->id, pFiles[iLoop]); iLoop++;}
                break;
            } catch(std::exception &e) {
                fprintf(fp, "Exception : %s, while sending pic. So retrying...\n", e.what()); fflush(fp);
                iRetry++;   sleep(1);
            }
        }

        //  Now send the Message
        pBot->getApi().sendMessage(pMsg->chat->id, pBaseBtn->getMsg(), false, 0, pMenu, pBaseBtn->getParseMode());

        if(pBaseBtn->isPlsWait()) {
            pthread_mutex_lock(&mtx_01);
            std::vector< std::tuple<unsigned int, unsigned int> >::iterator itrWait;
            for(itrWait = inTm.begin(); inTm.end() != itrWait; itrWait++) if(std::get<0>(*itrWait) == pMsg->chat->id) { inTm.erase(itrWait); break; }
            pthread_mutex_unlock(&mtx_01);
        }

        pBaseBtn->cleanup(pMsg, listBaseBtns, fp);
        fprintf(fp, "BaseBot %ld: Done responding back\n", time(0)); fflush(fp);
    });

    pBot->getEvents().onCallbackQuery( [pBot, &listBaseBtns, fp](TgBot::CallbackQuery::Ptr pQuery) {
        petWatchDog(fp);
        std::shared_ptr<BaseButton> pBaseBtn = nullptr;

        fprintf(fp, "id : %s, inlineMessageId : %s, chatInstance : %s, data : %s\n",
                pQuery->id.c_str(), pQuery->inlineMessageId.c_str(), pQuery->chatInstance.c_str(), pQuery->data.c_str()); fflush(fp);
        std::map<std::string, std::shared_ptr<BaseButton>>::const_iterator itrBBtn;
        if(listBaseBtns.end() != (itrBBtn = listBaseBtns.find(pQuery->data))) {
            pBaseBtn = itrBBtn->second->getSharedPtr();
        } else {
            pBaseBtn    = listBaseBtns[STR_BTN_QUICK_MENU];
        }

        pQuery->message->text   = pQuery->data;
        pBaseBtn->init(pQuery->message, fp);
        pBaseBtn->onClick(pQuery->message, fp);
        TgBot::GenericReply::Ptr pMenu = pBaseBtn->prepareMenu(listBaseBtns, pQuery->message, fp);
        pBot->getApi().sendMessage(pQuery->message->chat->id, pBaseBtn->getMsg(), false, 0, pMenu, pBaseBtn->getParseMode());
    });
    //fprintf(fp, "BaseBot %ld: Bot username %s\n", time(0), pBot->getApi().getMe()->username.c_str()); fflush(fp);

    std::shared_ptr<TgBot::TgLongPoll> pLongPoll  = std::make_shared<TgBot::TgLongPoll>(*pBot, 100, 3);
	std::int32_t update_id = getUpdateId();
    while (true) {
        try {
            petWatchDog(fp);
    		pBot->getApi().deleteWebhook();
            update_id = pLongPoll->start(update_id);
			setUpdateId(update_id);
        } catch (std::exception& e) {
			if(update_id > 0) update_id++;
            fprintf(fp, "BaseBot %ld: An exception occured at longPoll %s\nUpdating id %d\n", time(0), e.what(), update_id); fflush(fp);
        }
    }
}

int main() {
    std::cout << "SGN" << std::endl;
    std::string root_path   = std::string(BOT_ROOT_PATH);
    std::string log_path    = std::string(BOT_LOG_PATH);
    std::string file_name   = std::string(BOT_LOG_FILE) + std::string("_") + std::to_string(time(0));
    std::string log_file    = root_path + log_path + file_name;
    FILE *fp = fopen(log_file.c_str(), "w");
    if(NULL == fp) {
        std::cout << "Error opening file " << log_file << std::endl;
        return -1;
    }

    fprintf(fp, "Daemon: Very start\n"); fflush(fp);

    pid_t pid;
    pid = fork();

    //  Exit the parent process
    if(0 < pid) {
        std::cout << "Exiting pid " << getpid() << std::endl;
        exit(EXIT_SUCCESS);
    }

    //  -ve return means error
    if(0 > pid) {
        std::cout << "Error creating child process" << std::endl;
        return -1;
    }

    //--------------Child Process---------------
    fprintf(fp, "Daemon: Successfully started Bot daemon\n"); fflush(fp);

    //  Start the main loop
    BotMainLoop(fp);

    fprintf(fp, "BaseBot: Stopping Bot daemon\n"); fflush(fp);
    fclose(fp);

    exit(EXIT_SUCCESS);
    return 0;
}


