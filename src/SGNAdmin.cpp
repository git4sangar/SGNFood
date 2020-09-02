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
#include <algorithm>

#include "BaseButton.h"
#include "Constants.h"
#include "SGNAdmin.h"

std::string SGNAdmin::STR_MSG_DEFF_RELEASE   = " ";

std::vector<std::string> SGNAdmin::split_address(std::string strAddress) {
    std::vector<std::string> words;
    std::string strsub, strLine;
    std::string::size_type curPos;
    unsigned int len = 0;

    strAddress += ".";  //  To get the final word too
    while(std::string::npos != (curPos  = strAddress.find_first_of("\n\t ,."))) {
        if(curPos) {
            len += (curPos+1);  //  now len is size of string after concatenation
            if(MAX_SINGLE_LINE_WIDTH <= len) {words.push_back(strLine); strLine.clear();}

            strsub = strAddress.substr(0, curPos) + " ";
            strLine += strsub;

            if(MAX_SINGLE_LINE_WIDTH <= len) len = strsub.length();
        }
        strAddress  = strAddress.substr(curPos + 1);
    }
    if(!strLine.empty()) words.push_back(strLine);

    return words;
}

std::vector<unsigned int> SGNAdmin::getUserIds(std::string strMsg, FILE *fp, char delimiter = ',') {
    std::vector<unsigned int> iUserIds;
    unsigned int iUserId = 0;
    std::string token;
    std::stringstream ssMsg;

    strMsg = myTrim(strMsg);
    std::transform(strMsg.begin(), strMsg.end(), strMsg.begin(), ::tolower);
    if(!strMsg.compare("all")) {
        std::vector<User::Ptr> users = getDBHandle()->getAllUsers(fp);
        for(auto &user : users) iUserIds.push_back(user->m_UserId);
    } else {
        ssMsg.str(strMsg);
        while(std::getline(ssMsg, token, delimiter)) {
            try { iUserId = std::stoi(myTrim(token)); } catch(std::exception &e) {iUserId = 0;}
            iUserIds.push_back(iUserId);
        }
    }
    return iUserIds;
}

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
        STR_MSG_DEFF_RELEASE        = std::string("Send message to all or a list of users as follows.\n user-id1, user-id2 : Message") +
                                        std::string("\n\nExample:\nSending message to User-Id 2, 5\n2, 5 : Sri Gurubhyo Namaha.") +
                                        std::string("\nSending message to all uses\nall : Dear Customer, Sunday we don't server food.");
        return std::make_shared<TgBot::ReplyKeyboardRemove>();
    }
    if(!pMsg->text.compare(STR_BTN_USER_ORDERS)) {
        m_Context[pMsg->chat->id]   = USER_CTXT_USER_AC;
        lstBaseBtns[strChatId]      = getSharedPtr();
        STR_MSG_DEFF_RELEASE        = std::string("To see a user's stmt, type his/her userid & send");
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

    createKBBtn(STR_BTN_USER_ORDERS, row[iRowIndex], lstBaseBtns, getSharedPtr());
    createKBBtn(STR_BTN_DLVRY_LIST, row[iRowIndex], lstBaseBtns, getSharedPtr());
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
    int iOutstanding = 0, iLoop = 0;
    std::stringstream ss;

    if(m_Context.end() != (itrCntxt = m_Context.find(pMsg->chat->id))) {
        std::string strMsg = pMsg->text;
        int iPos = 0, iUserId = 0;
        std::vector<unsigned int> iUserIds;
        std::vector<POrder::Ptr> orders;

        if(USER_CTXT_SEND_MSG == itrCntxt->second && std::string::npos != (iPos = strMsg.find_first_of(":")) && (0 < iPos) && (strMsg.length() > (iPos+1))) {
            iUserIds = getUserIds(myTrim(strMsg.substr(0,iPos)), fp, ',');

            for(iLoop = 0; (iLoop < iUserIds.size()) && (0 < iUserIds[iLoop]) && (pUser = getDBHandle()->getUserForUserId(iUserIds[iLoop], fp)); iLoop++) {
                notifyMsgs[pUser->m_ChatId] = myTrim(strMsg.substr(iPos+1));
            }
            STR_MSG_DEFF_RELEASE = (notifyMsgs.empty()) ? "Failed sending msg." : std::string("Sent above message to ") + std::to_string(iLoop) + " user(s).";
            getDBHandle()->updateNotifications(notifyMsgs, fp); notifyMsgs.clear();
        } else if(USER_CTXT_USER_AC == itrCntxt->second) {
            iUserIds    = getUserIds(myTrim(strMsg), fp);
            pUser       = getDBHandle()->getUserForUserId(iUserIds[0], fp);
            if(pUser) orders      = getDBHandle()->getOrdersByUser(pUser->m_UserId, fp);
            if(!orders.empty()) {
                ss << "<b>" << pUser->m_Name << ", Id: " << pUser->m_UserId << "\n";
                ss  << std::setfill(' ') << std::setw(5) << "Date" << "    "
                    << std::setfill(' ') << std::setw(5) << "Ord" << "    "
                    << std::setfill(' ') << std::setw(5) << "Amt" << "    "
                    << std::setfill(' ') << std::setw(5) << "Bal" << "</b>\n";
                for(auto &order : orders) ss << std::setfill(' ') << std::setw(5) << order->m_OrdrTm.substr(5,5) << "    "
                                             << std::setfill(' ') << std::setw(5) << order->m_OrderNo << "    "
                                             << std::setfill(' ') << std::setw(5) << order->m_Amt << "    "
                                             << std::setfill(' ') << std::setw(5) << order->m_WBalance << "\n";
                notifyMsgs[pMsg->chat->id] = ss.str();
                getDBHandle()->updateNotifications(notifyMsgs, fp); notifyMsgs.clear();
            }
        } else STR_MSG_DEFF_RELEASE = "Invalid Format";
    }
    if(!pMsg->text.compare(STR_BTN_ALL_DLVRD)) {
        getDBHandle()->updateAllDelivered(fp);
        STR_MSG_DEFF_RELEASE = "All \"Confirmed-Yesterday's Order\" are marked as \"Delivered\".\n";
    }
	if(!pMsg->text.compare(STR_BTN_DLVRY_LIST)) {
        std::vector<POrder::Ptr> orders = getDBHandle()->getOrderByStatus(CartStatus::READY_FOR_DELIVERY, OrderType::PORDER, fp);
        for(iLoop = 0; iLoop < orders.size(); iLoop++) {
            std::stringstream ssMsg;
            ssMsg << orders[iLoop]->m_OrderNo << ", " << orders[iLoop]->m_Name.substr(0,10)
                    << ", Id: "<< orders[iLoop]->m_UserId << ", <b>₹ " << orders[iLoop]->m_Amt << "</b>";

            std::vector<std::string> lines  = split_address(orders[iLoop]->m_Address);
            for(auto &line : lines) ssMsg << "\n" << line;
            ssMsg  << "\n";

            std::vector<Cart::Ptr> cartItems    = getDBHandle()->getCartItemsForOrderNo(orders[iLoop]->m_OrderNo, fp);
            for(auto &cart: cartItems) {
                Product::Ptr pProd  = getDBHandle()->getProductById(cart->m_ProductId, fp);
                ssMsg << "\n₹ "
                        << std::setfill('0') << std::setw(4) << (pProd->m_Price * cart->m_Qnty) << " -:- "
                        << std::setfill('0') << std::setw(2) << cart->m_Qnty << " x " << pProd->m_Name;
            }
            notifyMsgs[iLoop] = ssMsg.str();
        }
        getDBHandle()->updateNotifications(notifyMsgs, fp); notifyMsgs.clear();
    }
    if(!pMsg->text.compare(STR_BTN_REMIND_CHKOUT)) {
        std::vector<User::Ptr> users = getDBHandle()->getCartedUsers(fp);
        for(auto &user : users) {
            notifyMsgs[user->m_ChatId] = user->m_Name + std::string(", Your Cart contains items. Forgot to click \"Confirm Checkout\"?\n") +
                                            std::string("\nClick buttons as follows to place order.\nView Cart -> Checkout -> Confirm Checkout.");
        }
        getDBHandle()->updateNotifications(notifyMsgs, fp); notifyMsgs.clear();
        STR_MSG_DEFF_RELEASE = std::string("Reminding all users, who has items in Cart, to Confirm Checkout\n");
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
