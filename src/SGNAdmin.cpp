/*sgn/*
 * SGNAdmin.cpp
 *
 *  Created on: 15-Jul-2020
 *      Author: tstone10
 */

#include <iostream>
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>
#include <ProductList.h>
#include <memory>

#include "BaseButton.h"
#include "Constants.h"
#include "SGNAdmin.h"

std::string SGNAdmin::STR_MSG_DEFF_RELEASE   = " ";

TgBot::GenericReply::Ptr SGNAdmin::prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& lstBaseBtns, TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: SGNAdmin::prepareMenu {\n", time(0)); fflush(fp);

    std::string strText;
    std::vector<TgBot::KeyboardButton::Ptr> row[MAX_BUTTON_ROWS];
    unsigned int iLoop = 0, iRowIndex = 0;
    std::map<unsigned int, UserContext>::const_iterator itrCntxt;
    std::map<std::string, std::shared_ptr<BaseButton> >::const_iterator itrBtn;
    TgBot::ReplyKeyboardMarkup::Ptr pMainMenu;

    if(STR_MSG_DEFF_RELEASE.empty()) STR_MSG_DEFF_RELEASE   = "Navigate to different Admin Pages.";

    std::string strChatId   = std::to_string(pMsg->chat->id);
    if(!pMsg->text.compare(STR_BTN_SEND_MSG)) {
        m_Context[pMsg->chat->id]   = USER_CTXT_SEND_MSG;
        lstBaseBtns[strChatId]      = getSharedPtr();
        STR_MSG_DEFF_RELEASE        = std::string("Send message to a particular user as follows.\n User-Id : Message") +
                                        std::string("\n\nExample:\nSending message to User-Id 2\n2 : Sri Gurubhyo Namaha.") +
                                        std::string("\nSending message to User-Id 12342\n12342 : Could you please pay the balance soon?");
        return std::make_shared<TgBot::ReplyKeyboardRemove>();
    }
    if(m_Context.end() != (itrCntxt = m_Context.find(pMsg->chat->id))) {
        m_Context.erase(itrCntxt);
        if(lstBaseBtns.end() != (itrBtn = lstBaseBtns.find(strChatId))) lstBaseBtns.erase(itrBtn);
    }

    iRowIndex = 0;
    createKBBtn(STR_BTN_NEW_ORDERS, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_CNF_ORDERS, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_CNCLD_ORDERS, row[iRowIndex], lstBaseBtns);
    iRowIndex++;

    createKBBtn(STR_BTN_NEW_TOPUPs, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_CNF_TOPUPs, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_CNCLD_TOPUPs, row[iRowIndex], lstBaseBtns);
    iRowIndex++;

    createKBBtn(STR_BTN_SEND_MSG, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_ALL_DLVRD, row[iRowIndex], lstBaseBtns, getSharedPtr());
    createKBBtn(STR_BTN_REMIND_CHKOUT, row[iRowIndex], lstBaseBtns, getSharedPtr());
    iRowIndex++;

    createKBBtn(STR_BTN_MENU_MGMT, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_MAINMENU, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_ORDR_SUMMRY, row[iRowIndex], lstBaseBtns, getSharedPtr());
    iRowIndex++;

    //  Add all the rows to main menu
    pMainMenu   = std::make_shared<TgBot::ReplyKeyboardMarkup>();
    for(iLoop = 0; iLoop < iRowIndex; iLoop++) {
        pMainMenu->keyboard.push_back(row[iLoop]);
    }

    fprintf(fp, "BaseBot %ld: Finishing SGNAdmin::prepareMenu }\n", time(0)); fflush(fp);
    pMainMenu->resizeKeyboard   = true;
    return pMainMenu;
}

void SGNAdmin::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: SGNAdmin onClick pMsg %s {\n", time(0), pMsg->text.c_str()); fflush(fp);

    std::map<unsigned int, UserContext>::const_iterator itrCntxt;
    User::Ptr pUser = nullptr;
    int iOutstanding = 0;
    std::stringstream ss;

    if(m_Context.end() != (itrCntxt = m_Context.find(pMsg->chat->id)) && USER_CTXT_SEND_MSG == itrCntxt->second) {
        std::string strMsg = pMsg->text;
        int iPos = 0, iUserId = 0;
        if(std::string::npos != (iPos = strMsg.find_first_of(":")) && (0 < iPos) && (strMsg.length() > (iPos+1))) {
            try { iUserId = std::stoi(myTrim(strMsg.substr(0,iPos)));} catch(std::exception &e) { iUserId = 0; }
            if( (0 < iUserId) && (pUser = getDBHandle()->getUserForUserId(iUserId, fp)) ) { notifyMsgs[pUser->m_ChatId] = myTrim(strMsg.substr(iPos+1)); }
            STR_MSG_DEFF_RELEASE = (notifyMsgs.empty()) ? "Failed sending msg." : std::string("Sent above message to ") + pUser->m_Name;
        } else STR_MSG_DEFF_RELEASE = "Invalid Format";
    }
    if(!pMsg->text.compare(STR_BTN_ALL_DLVRD)) {
        getDBHandle()->updateAllDelivered(fp);
        STR_MSG_DEFF_RELEASE = "All \"Confirmed-Yesterday's Order\" are marked as \"Delivered\".\n";
    }
    if(!pMsg->text.compare(STR_BTN_REMIND_CHKOUT)) {
        std::vector<User::Ptr> users = getDBHandle()->getCartedUsers(fp);
        for(auto &user : users) {
            notifyMsgs[user->m_ChatId] = user->m_Name + std::string(", Your Cart contains items. Forgot to click \"Confirm Checkout\"?\n") +
                                            std::string("\nClick buttons as follows to place order.\nView Cart -> Checkout -> Confirm Checkout.");
        }
        STR_MSG_DEFF_RELEASE = std::string("Reminding all users, who has items in Cart, to Confirm Checkout\n");
        if(10 < users.size()) STR_MSG_DEFF_RELEASE += std::string("It will take some time. Pls wait..");
    }
    if(!pMsg->text.compare(STR_BTN_ORDR_SUMMRY)) {
        ss.str(std::string(""));

        ss << "<b>Total outstanding : </b>" << std::to_string(std::abs(getDBHandle()->getAllOutstanding(fp))) << ".\n\n";

        ss << "<b>No Of Users: </b>" << getDBHandle()->getNoOfUsers(fp) << ".\n\n";

        std::map<unsigned int, unsigned int>::iterator itr;
        std::map<unsigned int, unsigned int> ordrSum   = getDBHandle()->getOrderSummary(fp);
        ss << "<b>Order Summary</b>\n";
        for(itr =  ordrSum.begin(); itr != ordrSum.end(); itr++) {
            Product::Ptr pProd  = getDBHandle()->getProductById(itr->first, fp);
            if(pProd) ss << pProd->m_Name << " : " << itr->second << "\n";
        }
        STR_MSG_DEFF_RELEASE = ss.str();
    }

    fprintf(fp, "BaseBot %ld: SGNAdmin onClick }\n", time(0)); fflush(fp);
}
