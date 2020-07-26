/*sgn/*
 * WalletMgmt.cpp
 *
 *  Created on: 14-Jul-2020
 *      Author: tstone10
 */

#include <iostream>
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>
#include <ProductList.h>
#include <memory>

#include "pngwriter.h"

#include "BaseButton.h"
#include "Constants.h"
#include "WalletMgmt.h"

std::string WalletMgmt::STR_MSG_DEFF_RELEASE   = "New orders";

void WalletMgmt::create_order_table(std::string file_name, int iPageNo, FILE *fp) {
    fprintf(fp, "BaseBot %ld: WalletMgmt::create_product_table {\n", time(0)); fflush(fp);

    pngwriter product_table(320, 320, 1.0, file_name.c_str());
    std::string strName;
    char toggle = 0;

    //  Headers
    std::string strFontFile = std::string(BOT_ROOT_PATH) + std::string(BOT_FONT_PATH) + std::string(BOT_FONT_FILE_BOLD);
    product_table.filledsquare(0, 300, 320, 320, 0.0, 0.5, 0.5);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12,     5, 305, 0.0, (char *)"SN", 1.0, 1.0, 1.0);
    strName = pageName.substr(0, 10);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12,  35+5, 305, 0.0, (char *)strName.c_str(), 1.0, 1.0, 1.0);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12, 150+5, 305, 0.0, (char *)"No", 1.0, 1.0, 1.0);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12, 210+5, 305, 0.0, (char *)"Amt", 1.0, 1.0, 1.0);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12, 265+5, 305, 0.0, (char *)"PG", 1.0, 1.0, 1.0);

    //  Highlight alternate rows
    int iLoop = 0, iIndex = 0;
    iIndex = (iPageNo - 1) * MAX_ITEMS_PER_PAGE;
    for(iLoop = 300; iLoop > 0; iLoop -= 30, toggle = 1 - toggle) {
        if(0 == toggle) product_table.filledsquare(0, iLoop, 320, iLoop-30, 0.9, 0.9, 0.9);
        if((iNoOfItems > iIndex) && (iIndex < (iPageNo * MAX_ITEMS_PER_PAGE))) {
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10,     5, iLoop-20, 0.0, (char *)std::to_string(iIndex+1).c_str(), 0, 0, 0);

            strName = orders[iIndex]->m_Name.substr(0,12);
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10,  35+5, iLoop-20, 0.0, (char *)strName.c_str(), 0, 0, 0);
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10, 150+5, iLoop-20, 0.0, (char *)std::to_string(orders[iIndex]->m_OrderNo).c_str(), 0, 0, 0);
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10, 210+5, iLoop-20, 0.0, (char *)std::to_string(orders[iIndex]->m_Amt).c_str(), 0, 0, 0);
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10, 265+5, iLoop-20, 0.0, (char *)orders[iIndex]->m_PayGW.c_str(), 0, 0, 0);
            iIndex++;
        }
    }

    //  Vertical lines
    product_table.line( 35, 300,  35, 0, 0, 0, 0);
    product_table.line(150, 300, 150, 0, 0, 0, 0);
    product_table.line(210, 300, 210, 0, 0, 0, 0);
    product_table.line(265, 300, 265, 0, 0, 0, 0);

    product_table.close();
    fprintf(fp, "BaseBot %ld: WalletMgmt::create_product_table }\n", time(0)); fflush(fp);
}

TgBot::GenericReply::Ptr WalletMgmt::prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& lstBaseBtns, TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: WalletMgmt::prepareMenu {\n", time(0)); fflush(fp);

    std::string strText;
    std::vector<TgBot::KeyboardButton::Ptr> row[MAX_BUTTON_ROWS];
    unsigned int iLoop = 0, iPrev = 0, iNext = 0, iToggle = 0, iRowIndex = 0;
    TgBot::ReplyKeyboardMarkup::Ptr pMainMenu;

    std::map<unsigned int, UserContext>::const_iterator itrCntxt;
    std::map<std::string, std::shared_ptr<BaseButton> >::const_iterator itrBtn;

    std::string strChatId   = std::to_string(pMsg->chat->id);
    if(!pMsg->text.compare(STR_BTN_CNFM_R_CNCL_TOPUP)) {
        m_Context[pMsg->chat->id]   = USER_CTXT_CNFM_CNCL;
        lstBaseBtns[strChatId]      = getSharedPtr();
        STR_MSG_DEFF_RELEASE        = std::string("Is above said amount credited ?") +
                                        std::string("\nIf yes, confirm it or else cancel it as follows and send.") +
                                        std::string("\nExample:\n\tConfirm 1020\n\tCancel 1075");
        return std::make_shared<TgBot::ReplyKeyboardRemove>();
    }
    if(!pMsg->text.compare(STR_BTN_FORCE_WALLET)) {
        m_Context[pMsg->chat->id]   = USER_CTXT_FORCE_WALLET;
        lstBaseBtns[strChatId]      = getSharedPtr();
        STR_MSG_DEFF_RELEASE        = std::string("You have the User-Id to force? FYI: User-Id available in all Order Details.") +
                                        std::string("\nIf yes, type <b>User-Id : Amount</b> (amount -9999 to 9999 acceptable) and send.") +
                                        std::string("\nExample:\n\t21 : 1000\n\t114 : -2000");
        return std::make_shared<TgBot::ReplyKeyboardRemove>();
    }

    iRowIndex = 0;
    if(0 < iNoOfItems) {
        createKBBtn(STR_BTN_CNFM_R_CNCL_TOPUP, row[iRowIndex], lstBaseBtns, getSharedPtr());
        createKBBtn(STR_BTN_FORCE_WALLET, row[iRowIndex], lstBaseBtns, getSharedPtr());
        iRowIndex++;

        if(MAX_ITEMS_PER_PAGE < iNoOfItems) {
            iPrev       = (iSelPage == 1) ? 1 : (iSelPage - 1);
            strText     = pageName + std::string(" page ") + std::to_string(iPrev);
            createKBBtn(strText, row[iRowIndex], lstBaseBtns, getSharedPtr());

            iNext       = (iNoOfItems > (iSelPage * MAX_ITEMS_PER_PAGE)) ? (iSelPage + 1) : iSelPage;
            strText     = pageName + std::string(" page ") + std::to_string(iNext);
            createKBBtn(strText, row[iRowIndex], lstBaseBtns, getSharedPtr());

            //  Pick the next available row
            iRowIndex++;
        }
    } else STR_MSG_DEFF_RELEASE   = "No TopUp requests to display.";

    if(m_Context.end() != (itrCntxt = m_Context.find(pMsg->chat->id)) ) {
        m_Context.erase(itrCntxt);
        if(lstBaseBtns.end() != (itrBtn = lstBaseBtns.find(strChatId))) lstBaseBtns.erase(itrBtn);
    }

    createKBBtn(STR_BTN_NEW_TOPUPs, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_CNF_TOPUPs, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_CNCLD_TOPUPs, row[iRowIndex], lstBaseBtns);
    iRowIndex++;

    createKBBtn(STR_BTN_NEW_ORDERS, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_MAINMENU, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_ADMIN_PG, row[iRowIndex], lstBaseBtns);
    iRowIndex++;

    //  Add all the rows to main menu
    pMainMenu   = std::make_shared<TgBot::ReplyKeyboardMarkup>();
    for(iLoop = 0; iLoop < iRowIndex; iLoop++) {
        pMainMenu->keyboard.push_back(row[iLoop]);
    }

    if(STR_MSG_DEFF_RELEASE.empty()) STR_MSG_DEFF_RELEASE   = "Wallet Management.";

    fprintf(fp, "BaseBot %ld: Finishing WalletMgmt::prepareMenu }\n", time(0)); fflush(fp);
    pMainMenu->resizeKeyboard   = true;
    return pMainMenu;
}

void WalletMgmt::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: WalletMgmt onClick pMsg %s {\n", time(0), pMsg->text.c_str()); fflush(fp);
    CartStatus crtStat  = CartStatus::PAYMENT_PENDING;
    User::Ptr pUser     = nullptr;
    int iMaxPages       = 0, iOrderNo = 0;

    //  Check if he is Admin
    bool isAdmin = false;
    std::vector<unsigned int>::const_iterator itr;
    for(itr = adminChatIds.begin(); itr != adminChatIds.end(); itr++) if(*itr == pMsg->chat->id) { isAdmin = true; break; }
    if(!isAdmin) return;

    std::map<unsigned int, UserContext>::const_iterator itrCntxt;
    std::string strMsg = pMsg->text;
    if(m_Context.end() != (itrCntxt = m_Context.find(pMsg->chat->id)) ) {
        POrder::Ptr pOrder  = nullptr; User::Ptr pUser    = nullptr;
        CartStatus crtStat  = CartStatus::NOTA;

        //  Convert whatever into lower case
        std::for_each(strMsg.begin(), strMsg.end(), [](char & c){if(std::isalpha(c)) c = ::tolower(c);});

        if(USER_CTXT_CNFM_CNCL == m_Context[pMsg->chat->id]) {
            //  Parse OrderNo first
            if(std::string::npos != strMsg.find("confirm")) {
                try { iOrderNo  = std::stoi(myTrim(strMsg.substr(7))); } catch(std::exception &e) {iOrderNo = 0;}
                crtStat = CartStatus::READY_FOR_DELIVERY;
                strMsg  = "confirmed";
            }
            if(std::string::npos != strMsg.find("cancel")) {
                try { iOrderNo = std::stoi(myTrim(strMsg.substr(6))); } catch(std::exception &e) {iOrderNo = 0;}
                crtStat = CartStatus::CANCELLED;
                strMsg  = "cancelled";
            }

            pOrder  = getDBHandle()->getOrderForOrderNo(iOrderNo, fp);
            if(pOrder) {
                std::stringstream ss;
                getDBHandle()->updateOrderStatus(iOrderNo, crtStat, OrderType::TOPUP, fp);
                pUser    = getDBHandle()->getUserForUserId(pOrder->m_UserId, fp);
                ss << "Your TopUp req no " << iOrderNo << ", for amount ₹ " << pOrder->m_Amt << ", is " << strMsg;
                notifyMsgs[pUser->m_ChatId] = ss.str();
            } else {
                STR_MSG_DEFF_RELEASE   = "Invalid input.";
            }
        }

        std::stringstream ssMsg; ssMsg.str(strMsg);
        pUser    = nullptr;
        if(USER_CTXT_FORCE_WALLET == m_Context[pMsg->chat->id]) {
            std::vector<std::string> tokens;
            std::string token;
            while(std::getline(ssMsg, token, ':')) {
                tokens.push_back(myTrim(token));
            }
            if(FORCE_TOKENS == tokens.size()) {
                pUser = getDBHandle()->forceWalletBalance(tokens[TOKEN_USER_ID], tokens[TOKEN_AMOUNT], fp);
            }
            if(!pUser) { STR_MSG_DEFF_RELEASE = "Invalid Format or User-Id or Amount. Not forced."; }
            else  {
                STR_MSG_DEFF_RELEASE = pUser->m_Name + std::string("'s wallet balance is forced to ") + tokens[1];
                notifyMsgs[pUser->m_ChatId] = std::string("Your wallet balance is forced to ") + tokens[1] + std::string(" by merchant.");
            }
        }
    }

    //  By default land in Your Orders
    pageName    = STR_BTN_NEW_TOPUPs;
    crtStat     = CartStatus::PAYMENT_PENDING;
    iSelPage    = 1;

    //  Check if landed in this page via pagination
    if(std::string::npos != pMsg->text.find("page")) {
        unsigned int iLoop = 0;
        for(iLoop = 1; iLoop <= MAX_NO_OF_PAGES; iLoop++) {
            std::stringstream ssNewPg, ssCnfPg, sslCncldPg;

            ssNewPg     << STR_BTN_NEW_TOPUPs   << " page " << iLoop;
            ssCnfPg     << STR_BTN_CNF_TOPUPs   << " page " << iLoop;
            sslCncldPg  << STR_BTN_CNCLD_TOPUPs << " page " << iLoop;

            if(!pMsg->text.compare(ssNewPg.str())) {
                crtStat     = CartStatus::PAYMENT_PENDING;
                pageName    = STR_BTN_NEW_TOPUPs;
                iSelPage    = iLoop;
                break;
            } else if(!pMsg->text.compare(ssCnfPg.str())) {
                crtStat     = CartStatus::READY_FOR_DELIVERY;
                pageName    = STR_BTN_CNF_TOPUPs;
                iSelPage    = iLoop;
                break;
            } else if(!pMsg->text.compare(sslCncldPg.str())) {
                crtStat     = CartStatus::CANCELLED;
                pageName    = STR_BTN_CNCLD_TOPUPs;
                iSelPage    = iLoop;
                break;
            }
        }
    } else if(!pMsg->text.compare(STR_BTN_NEW_TOPUPs)) {
        crtStat     = CartStatus::PAYMENT_PENDING;
        pageName    = STR_BTN_NEW_TOPUPs;
        iSelPage    = 1;
    } else if(!pMsg->text.compare(STR_BTN_CNF_TOPUPs)) {
        crtStat     = CartStatus::READY_FOR_DELIVERY;
        pageName    = STR_BTN_CNF_TOPUPs;
        iSelPage    = 1;
    } else if(!pMsg->text.compare(STR_BTN_CNCLD_TOPUPs)) {
        crtStat     = CartStatus::CANCELLED;
        pageName    = STR_BTN_CNCLD_TOPUPs;
        iSelPage    = 1;
    }

    orders      = getDBHandle()->getOrderByStatus(crtStat, OrderType::TOPUP, fp);
    iNoOfItems  = orders.size();

    //  Just a sanity
    iMaxPages   = (iNoOfItems / MAX_ITEMS_PER_PAGE) + (0 != (iNoOfItems % MAX_ITEMS_PER_PAGE));
    iSelPage    = (iMaxPages >= iSelPage) ? iSelPage : iMaxPages;

    fprintf(fp, "BaseBot %ld: WalletMgmt onClick }\n", time(0)); fflush(fp);
}

TgBot::InputFile::Ptr WalletMgmt::getMedia(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: WalletMgmt getMedias {\n", time(0)); fflush(fp);
    TgBot::InputFile::Ptr pFile = nullptr;

    if(0 < iNoOfItems) {
        asset_file  = std::string(BOT_ROOT_PATH) + std::string(BOT_ASSETS_PATH) +
                std::string("wallet_page_") + std::to_string(iSelPage) + std::string(".png");
        create_order_table(asset_file, iSelPage, fp);
        if(isFileExists(asset_file)) pFile = TgBot::InputFile::fromFile(asset_file, "image/png");
    }

    fprintf(fp, "BaseBot %ld: WalletMgmt getMedia }\n", time(0)); fflush(fp);
    return pFile;
}


