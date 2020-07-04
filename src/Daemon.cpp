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
#include "MyAddress.h"
#include "Constants.h"

#define MYPORT (4950)

std::map<unsigned int, UserContext> m_Context;
std::vector<unsigned int> adminChatIds;
std::string BotVersion::STR_MSG_DEFF_RELEASE;

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

pthread_mutex_t mtx;
std::vector< std::tuple<unsigned int, unsigned int> > inTm;
std::map<unsigned int, unsigned int> inMsgTmStamps;

void plsWaitThread(std::shared_ptr<TgBot::Bot> pBot, FILE *fp) {
    time_t curTm;
    std::vector<unsigned int> chatIds;
    std::vector< std::tuple<unsigned int, unsigned int> >::iterator itrWait;

    while(1) {
        chatIds.clear();
        pthread_mutex_lock(&mtx);
        for(itrWait = inTm.begin(); inTm.end() != itrWait;) {
            std::get<1>(*itrWait)   = std::get<1>(*itrWait) - 1;
            if(std::get<1>(*itrWait) <= time(0)) {
                chatIds.push_back(std::get<0>(*itrWait));
                inTm.erase(itrWait);
            } else itrWait++;
        }
        for(auto &chatId : chatIds)
            pBot->getApi().sendMessage(chatId, "<b>Delay in response is regretted...</b>", false, 0, nullptr, "HTML");

        pthread_mutex_unlock(&mtx);
        sleep(1);
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
        std::string::npos   != strCmd.find(STR_BTN_PRINT_ORDER) ||
        std::string::npos   != strCmd.find(STR_BTN_EDIT_ORDER) ||
        std::string::npos   != strCmd.find(STR_BTN_CNFM_ORDER) ||
        std::string::npos   != strCmd.find(STR_BTN_CNCL_ORDER) ||
        std::string::npos   != strCmd.find(SRT_BTN_DLVR_ORDER)
        ) {
        retItr = listBaseBtns.find(STR_BTN_SINGLE_ORDER);
    }
    return retItr;
}

void BotMainLoop(FILE *fp) {
    std::string root_path   = std::string(BOT_ROOT_PATH);
    std::string db_path     = std::string(BOT_DB_PATH);
    std::string db_file     = std::string(BOT_DB_FILE);
    time_t startSec = time(NULL);

    fprintf(fp, "Main: %ld: Starting MainLoop\n", time(0)); fflush(fp);

    std::shared_ptr<TgBot::Bot> pBot = std::make_shared<TgBot::Bot>(THRAYA_BOT);
    std::thread recv_thread(&plsWaitThread, pBot, fp);

    DBInterface::Ptr hDB       = std::make_shared<DBInterface>(root_path + db_path + db_file, fp);
    std::map<std::string, std::shared_ptr<BaseButton>> listBaseBtns;

    listBaseBtns["version"]             = std::make_shared<BotVersion>(hDB);
    listBaseBtns["Version"]             = listBaseBtns["version"];

    listBaseBtns[STR_BTN_PRODUCT_LIST]  = std::make_shared<ProductList>(hDB);
    listBaseBtns[STR_BTN_MAINMENU]      = listBaseBtns[STR_BTN_PRODUCT_LIST];
    listBaseBtns["/start"]              = listBaseBtns[STR_BTN_MAINMENU];
    listBaseBtns["start"]               = listBaseBtns[STR_BTN_MAINMENU];
    listBaseBtns[STR_BTN_EMPTY_CART]    = listBaseBtns[STR_BTN_MAINMENU];
    listBaseBtns[STR_BTN_FAQ]           = std::make_shared<FAQs>(hDB);

    listBaseBtns[STR_BTN_VIEW_CART]     = std::make_shared<ViewCart>(hDB);
    listBaseBtns[STR_BTN_REMOVE]        = listBaseBtns[STR_BTN_VIEW_CART];

    listBaseBtns[STR_BTN_CHECKOUT]      = std::make_shared<Checkout>(hDB);

    listBaseBtns[STR_BTN_SHPG_ADDRESS]  = std::make_shared<MyAddress>(hDB);
    listBaseBtns[STR_BTN_CHANGE_ADDRESS]= listBaseBtns[STR_BTN_SHPG_ADDRESS];

    listBaseBtns[STR_BTN_ORDERS]        = std::make_shared<OrderMgmt>(hDB);
    listBaseBtns[STR_BTN_ADMIN_PAGE]    = listBaseBtns[STR_BTN_ORDERS];
    listBaseBtns[STR_BTN_NEW_ORDERS]    = listBaseBtns[STR_BTN_ORDERS];
    listBaseBtns[STR_BTN_CNF_ORDERS]    = listBaseBtns[STR_BTN_ORDERS];
    listBaseBtns[STR_BTN_YOUR_ORDERS]   = listBaseBtns[STR_BTN_ORDERS];
    listBaseBtns[STR_BTN_DLVRD_ORDERS]  = listBaseBtns[STR_BTN_ORDERS];
    listBaseBtns[STR_BTN_CNCLD_ORDERS]  = listBaseBtns[STR_BTN_ORDERS];

    listBaseBtns[STR_BTN_MENU_MGMT]     = std::make_shared<PriceChange>(hDB);
    listBaseBtns[STR_BTN_EDIT_PRICE]    = listBaseBtns[STR_BTN_MENU_MGMT];
    listBaseBtns[STR_BTN_ITEM_LIST]     = listBaseBtns[STR_BTN_MENU_MGMT];

    listBaseBtns[STR_BTN_EDIT_MENU]     = std::make_shared<EditMenu>(hDB);
    listBaseBtns[STR_BTN_SHOW_MENU]     = listBaseBtns[STR_BTN_EDIT_MENU];

    listBaseBtns[STR_BTN_SINGLE_ORDER]  = std::make_shared<SingleOrder>(hDB);
    listBaseBtns[STR_BTN_PRINT_ORDER]   = listBaseBtns[STR_BTN_SINGLE_ORDER];
    listBaseBtns[STR_BTN_EDIT_ORDER]    = listBaseBtns[STR_BTN_SINGLE_ORDER];
    listBaseBtns[STR_BTN_CNFM_ORDER]    = listBaseBtns[STR_BTN_SINGLE_ORDER];
    listBaseBtns[STR_BTN_CNCL_ORDER]    = listBaseBtns[STR_BTN_SINGLE_ORDER];

    //  Admin Chat Ids
    adminChatIds.push_back(550919816);      // Myself
    //adminChatIds.push_back(685954136);    // Sriram
    adminChatIds.push_back(1306802994);     // Sudarshan
    adminChatIds.push_back(1352652258); // Santosh

    pBot->getEvents().onAnyMessage( [pBot, &listBaseBtns, fp, &startSec](TgBot::Message::Ptr pMsg) {
        petWatchDog(fp);
        static bool isSkipOver = false;

        fprintf(fp, "BaseBot %ld: Received \"%s\" for chatId: %ld onAnyMessage as it arrived\n", time(0), pMsg->text.c_str(), pMsg->chat->id); fflush(fp);
        std::shared_ptr<BaseButton> pBaseBtn = nullptr;
        std::string strCmd = pMsg->text;

        // Skip everything for SKIP_INTERVAL secs
        if(!isSkipOver && (startSec + SKIP_INTERVAL) > time(NULL)) {
            std::stringstream ss; ss << "<b>Hi " << pMsg->chat->firstName << ", Sorry, I am skipping your last few requests...</b>";
            pBot->getApi().sendMessage(pMsg->chat->id, ss.str(), false, 0, nullptr, "HTML");
            fprintf(fp, "BaseBot %ld: Skipping %s\n", time(0), pMsg->text.c_str()); fflush(fp);
            return;
        }
        isSkipOver = true;

        fprintf(fp, "BaseBot %ld: BaseBtnList Size %ld \n", time(0),  listBaseBtns.size()); fflush(fp);

        std::map<std::string, std::shared_ptr<BaseButton>>::const_iterator itrBBtn;
        if( listBaseBtns.end() != (itrBBtn = listBaseBtns.find(strCmd)) ||
            listBaseBtns.end() != (itrBBtn = listBaseBtns.find(std::to_string(pMsg->chat->id))) ||
            listBaseBtns.end() != (itrBBtn = isEditMenu(listBaseBtns, strCmd)) ||
            listBaseBtns.end() != (itrBBtn = isSingleOrder(listBaseBtns, strCmd))) {
            fprintf(fp, "BaseBot %ld: Found \"%s\" button\n", time(0), pMsg->text.c_str()); fflush(fp);
            pBaseBtn = itrBBtn->second->getSharedPtr();
        } else {
            fprintf(fp, "BaseBot %ld: \"%s\" button missing\n", time(0), pMsg->text.c_str()); fflush(fp);
//            std::stringstream ss;
//            ss << "Hi " << pMsg->from->firstName << ", could not recognize what you say, pls use the buttons below.\nRegret the inconvenience caused.";
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
            pthread_mutex_lock(&mtx);
            inTm.push_back(std::make_tuple(pMsg->chat->id, time(0) + MAX_WAIT_B4_RESP));
            pthread_mutex_unlock(&mtx);
        }

        int iLoop = 0, iRetry = 0;
        while(iRetry <= MAX_PIC_SEND_RETRIES) {
            try {
                if(pFile)   pBot->getApi().sendPhoto(pMsg->chat->id, pFile);
                else while(iLoop < pFiles.size()) {petWatchDog(fp); pBot->getApi().sendPhoto(pMsg->chat->id, pFiles[iLoop++]);}
                break;
            } catch(std::exception &e) {
                fprintf(fp, "Exception : %s, while sending pic. So retrying...\n", e.what()); fflush(fp);
                iRetry++;   sleep(1);
            }
        }

        //  Now send the Message
        pBot->getApi().sendMessage(pMsg->chat->id, pBaseBtn->getMsg(), false, 0, pMenu, pBaseBtn->getParseMode());

        if(pBaseBtn->isPlsWait()) {
            pthread_mutex_lock(&mtx);
            std::vector< std::tuple<unsigned int, unsigned int> >::iterator itrWait;
            for(itrWait = inTm.begin(); inTm.end() != itrWait; itrWait++) if(std::get<0>(*itrWait) == pMsg->chat->id) { inTm.erase(itrWait); break; }
            pthread_mutex_unlock(&mtx);
        }

        //  Now broadcast notifications if any
        std::map<unsigned int, std::string>::iterator itrNtfy;
        std::map<unsigned int, std::string> ntfyMsgs    = pBaseBtn->getNotifyMsgs(pMsg, fp);
        for(itrNtfy = ntfyMsgs.begin(); ntfyMsgs.end() != itrNtfy; itrNtfy++)
            pBot->getApi().sendMessage(itrNtfy->first, itrNtfy->second, false, 0, nullptr, pBaseBtn->getParseMode());

        pBaseBtn->cleanup(pMsg, listBaseBtns, fp);
        fprintf(fp, "BaseBot %ld: Done responding back\n", time(0)); fflush(fp);
    });

    //fprintf(fp, "BaseBot %ld: Bot username %s\n", time(0), pBot->getApi().getMe()->username.c_str()); fflush(fp);

    std::shared_ptr<TgBot::TgLongPoll> pLongPoll  = std::make_shared<TgBot::TgLongPoll>(*pBot, 100, 3);
    while (true) {
        try {
            petWatchDog(fp);
            //pBot->getApi().deleteWebhook();
            //fprintf(fp, "BaseBot %ld: Long poll started\n", time(0)); fflush(fp);
            pLongPoll->start();
        } catch (std::exception& e) {
            fprintf(fp, "BaseBot %ld: An exception occured at longPoll %s\n", time(0), e.what()); fflush(fp);
        }
    }
}

int main() {
    std::cout << "SGN" << std::endl;
    std::string root_path   = std::string(BOT_ROOT_PATH);
    std::string log_path    = std::string(BOT_LOG_PATH);
    std::string file_name   = std::string(BOT_LOG_FILE);
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


