/*sgn
 * OrderMgmt.cpp
 *
 *  Created on: 07-Jun-2020
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
#include "OrderMgmt.h"

std::string OrderMgmt::STR_MSG_DEFF_RELEASE   = "New orders";

UserContext OrderMgmt::getContextForPageName(std::string strPageName) {
    if(!strPageName.compare(STR_BTN_YOUR_ORDERS))    return USER_CTXT_YOUR_ORDER;
    if(!strPageName.compare(STR_BTN_NEW_ORDERS))     return USER_CTXT_NEW_ORDER;
    if(!strPageName.compare(STR_BTN_CNF_ORDERS))     return USER_CTXT_CNF_ORDER;
    return USER_CTXT_NOTA;
}

void OrderMgmt::create_order_table(std::string file_name, int iPageNo, FILE *fp) {
    fprintf(fp, "BaseBot %ld: OrderMgmt::create_product_table {\n", time(0)); fflush(fp);

    pngwriter product_table(320, 320, 1.0, file_name.c_str());
    std::string strName;
    char toggle = 0;

    //  Headers
    std::string strFontFile = std::string(BOT_ROOT_PATH) + std::string(BOT_FONT_PATH) + std::string(BOT_FONT_FILE_BOLD);
    product_table.filledsquare(0, 300, 320, 320, 0.0, 0.5, 0.5);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12,     5, 305, 0.0, (char *)"SN", 1.0, 1.0, 1.0);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12,  30+5, 305, 0.0, (char *)pageName.c_str(), 1.0, 1.0, 1.0);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12, 225+5, 305, 0.0, (char *)"Order No", 1.0, 1.0, 1.0);

    //  Highlight alternate rows
    int iLoop = 0, iIndex = 0;
    iIndex = (iPageNo - 1) * MAX_ITEMS_PER_PAGE;
    for(iLoop = 300; iLoop > 0; iLoop -= 30, toggle = 1 - toggle) {
        if(0 == toggle) product_table.filledsquare(0, iLoop, 320, iLoop-30, 0.9, 0.9, 0.9);
        if((iNoOfItems > iIndex) && (iIndex < (iPageNo * MAX_ITEMS_PER_PAGE))) {
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10,     5, iLoop-20, 0.0, (char *)std::to_string(iIndex+1).c_str(), 0, 0, 0);

            //  Who or When ordered it
            strName = orders[iIndex]->m_Name;
            if(!pageName.compare(STR_BTN_YOUR_ORDERS)) strName = orders[iIndex]->m_OrdrTm;
            else if(!pageName.compare(STR_BTN_DLVRD_ORDERS)) strName = orders[iIndex]->m_DlvrdTm;

            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10,  30+5, iLoop-20, 0.0, (char *)strName.c_str(), 0, 0, 0);
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10, 225+5, iLoop-20, 0.0, (char *)std::to_string(orders[iIndex]->m_OrderNo).c_str(), 0, 0, 0);
            iIndex++;
        }
    }

    //  Vertical lines
    product_table.line( 30, 320,  30, 0, 0, 0, 0);
    product_table.line(225, 320, 225, 0, 0, 0, 0);
    product_table.line(320, 320, 320, 0, 0, 0, 0);

    product_table.close();
    fprintf(fp, "BaseBot %ld: OrderMgmt::create_product_table }\n", time(0)); fflush(fp);
}

TgBot::GenericReply::Ptr OrderMgmt::prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& lstBaseBtns, TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: OrderMgmt::prepareMenu {\n", time(0)); fflush(fp);

    std::string strText;
    std::vector<TgBot::KeyboardButton::Ptr> row[MAX_BUTTON_ROWS];
    unsigned int iLoop = 0, iPrev = 0, iNext = 0, iToggle = 0, iRowIndex = 0;
    TgBot::ReplyKeyboardMarkup::Ptr pMainMenu;

    STR_MSG_DEFF_RELEASE    = pageName;
    iRowIndex = 0;
    if(0 < iNoOfItems) {
        for(iToggle = 0, iLoop = (iSelPage - 1) * MAX_ITEMS_PER_PAGE; (iNoOfItems > iLoop) && (iLoop < (iSelPage * MAX_ITEMS_PER_PAGE)); iLoop++, iToggle = (1-iToggle)) {
            strText = std::to_string(orders[iLoop]->m_OrderNo);
            //  If the button name is integer, Daemon will redirect it to SingleOrder page
            createKBBtn(strText, row[iToggle], lstBaseBtns);
        }
        iRowIndex   = (0 == row[1].size()) ? 1 : 2;

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
    } else STR_MSG_DEFF_RELEASE   = "No orders to display.";

    //  If it is "Your Order", meaning NOT Admin page
    if(!pageName.compare(STR_BTN_YOUR_ORDERS)) {
        createKBBtn(STR_BTN_MAINMENU, row[iRowIndex], lstBaseBtns);
        createKBBtn(STR_BTN_VIEW_CART, row[iRowIndex], lstBaseBtns);
        iRowIndex++;
    } else {
        createKBBtn(STR_BTN_NEW_ORDERS, row[iRowIndex], lstBaseBtns);
        createKBBtn(STR_BTN_CNF_ORDERS, row[iRowIndex], lstBaseBtns);
        createKBBtn(STR_BTN_MENU_MGMT, row[iRowIndex], lstBaseBtns);
        iRowIndex++;

        createKBBtn(STR_BTN_DLVRD_ORDERS, row[iRowIndex], lstBaseBtns);
        createKBBtn(STR_BTN_CNCLD_ORDERS, row[iRowIndex], lstBaseBtns);
        createKBBtn(STR_BTN_MAINMENU, row[iRowIndex], lstBaseBtns);
        iRowIndex++;
    }

    //  Add all the rows to main menu
    pMainMenu   = std::make_shared<TgBot::ReplyKeyboardMarkup>();
    for(iLoop = 0; iLoop < iRowIndex; iLoop++) {
        pMainMenu->keyboard.push_back(row[iLoop]);
    }

    fprintf(fp, "BaseBot %ld: Finishing OrderMgmt::prepareMenu }\n", time(0)); fflush(fp);
    return pMainMenu;
}

void OrderMgmt::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: OrderMgmt onClick pMsg %s {\n", time(0), pMsg->text.c_str()); fflush(fp);
    CartStatus crtStat  = CartStatus::PAYMENT_PENDING;
    User::Ptr pUser     = nullptr;
    int iMaxPages       = 0;

    //  Check if he is Admin
    bool isAdmin = false;
    std::vector<unsigned int>::const_iterator itr;
    for(itr = adminChatIds.begin(); itr != adminChatIds.end(); itr++) if(*itr == pMsg->chat->id) { isAdmin = true; break; }

    //  By default land in Your Orders
    pageName    = STR_BTN_YOUR_ORDERS;
    crtStat     = CartStatus::NOTA;
    iSelPage    = 1;

    //  Check if landed in this page via pagination
    if(std::string::npos != pMsg->text.find("page")) {
        unsigned int iLoop = 0;
        for(iLoop = 1; iLoop <= MAX_NO_OF_PAGES; iLoop++) {
            std::stringstream ssNewPg, ssCnfPg, ssUrPg, ssDlvrdPg, sslCncldPg;
            ssNewPg << STR_BTN_NEW_ORDERS << " page " << iLoop;
            ssCnfPg << STR_BTN_CNF_ORDERS << " page " << iLoop;
            ssUrPg  << STR_BTN_YOUR_ORDERS << " page " << iLoop;
            ssDlvrdPg  << STR_BTN_DLVRD_ORDERS << " page " << iLoop;
            sslCncldPg  << STR_BTN_CNCLD_ORDERS << " page " << iLoop;

            if(isAdmin && !pMsg->text.compare(ssNewPg.str())) {
                crtStat     = CartStatus::PAYMENT_PENDING;
                pageName    = STR_BTN_NEW_ORDERS;
                iSelPage    = iLoop;
                break;
            } else if(isAdmin && !pMsg->text.compare(ssCnfPg.str())) {
                crtStat     = CartStatus::READY_FOR_DELIVERY;
                pageName    = STR_BTN_CNF_ORDERS;
                iSelPage    = iLoop;
                break;
            } else if(isAdmin && !pMsg->text.compare(ssDlvrdPg.str())) {
                crtStat     = CartStatus::DELIVERED;
                pageName    = STR_BTN_DLVRD_ORDERS;
                iSelPage    = iLoop;
                break;
            } else if(isAdmin && !pMsg->text.compare(sslCncldPg.str())) {
                crtStat     = CartStatus::CANCELLED;
                pageName    = STR_BTN_CNCLD_ORDERS;
                iSelPage    = iLoop;
                break;
            } else if(!pMsg->text.compare(ssUrPg.str())) {
                crtStat     = CartStatus::NOTA;
                pageName    = STR_BTN_YOUR_ORDERS;
                iSelPage    = iLoop;
            }
        }
    } else if(isAdmin && (!pMsg->text.compare(STR_BTN_NEW_ORDERS) || !pMsg->text.compare(STR_BTN_ADMIN_PAGE))) {
        crtStat     = CartStatus::PAYMENT_PENDING;
        pageName    = STR_BTN_NEW_ORDERS;
        iSelPage    = 1;
    } else if(isAdmin && !pMsg->text.compare(STR_BTN_CNF_ORDERS)) {
        crtStat     = CartStatus::READY_FOR_DELIVERY;
        pageName    = STR_BTN_CNF_ORDERS;
        iSelPage    = 1;
    } else if(isAdmin && !pMsg->text.compare(STR_BTN_DLVRD_ORDERS)) {
        crtStat     = CartStatus::DELIVERED;
        pageName    = STR_BTN_DLVRD_ORDERS;
        iSelPage    = 1;
    } else if(isAdmin && !pMsg->text.compare(STR_BTN_CNCLD_ORDERS)) {
        crtStat     = CartStatus::CANCELLED;
        pageName    = STR_BTN_CNCLD_ORDERS;
        iSelPage    = 1;
    }

    if(CartStatus::NOTA == crtStat) {
        pUser       = getDBHandle()->getUserForChatId(pMsg->chat->id, fp);
        orders      = getDBHandle()->getOrdersByUser(pUser->m_UserId, fp);
        iNoOfItems  = orders.size();
    } else {
        orders      = getDBHandle()->getOrderByStatus(crtStat, fp);
        iNoOfItems  = orders.size();
    }

    //  Just a sanity
    iMaxPages   = (iNoOfItems / MAX_ITEMS_PER_PAGE) + (0 != (iNoOfItems % MAX_ITEMS_PER_PAGE));
    iSelPage    = (iMaxPages >= iSelPage) ? iSelPage : iMaxPages;

    fprintf(fp, "BaseBot %ld: OrderMgmt onClick }\n", time(0)); fflush(fp);
}

TgBot::InputFile::Ptr OrderMgmt::getMedia(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: OrderMgmt getMedias {\n", time(0)); fflush(fp);
    TgBot::InputFile::Ptr pFile;

    if(0 < iNoOfItems) {
        asset_file  = std::string(BOT_ROOT_PATH) + std::string(BOT_ASSETS_PATH) +
                std::string("order_page_") + std::to_string(iSelPage) + std::string(".png");
        create_order_table(asset_file, iSelPage, fp);
    }

    if(isFileExists(asset_file)) pFile = TgBot::InputFile::fromFile(asset_file, "image/png");

    fprintf(fp, "BaseBot %ld: OrderMgmt getMedia }\n", time(0)); fflush(fp);
    return pFile;
}

