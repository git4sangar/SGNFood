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
    TgBot::ReplyKeyboardMarkup::Ptr pMainMenu;

    if(STR_MSG_DEFF_RELEASE.empty()) STR_MSG_DEFF_RELEASE   = "Navigate to different Admin Pages.";

    iRowIndex = 0;
    createKBBtn(STR_BTN_NEW_ORDERS, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_CNF_ORDERS, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_DLVRD_ORDERS, row[iRowIndex], lstBaseBtns);
    iRowIndex++;

    createKBBtn(STR_BTN_NEW_TOPUPs, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_CNF_TOPUPs, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_CNCLD_TOPUPs, row[iRowIndex], lstBaseBtns);
    iRowIndex++;

    createKBBtn(STR_BTN_EDIT_MENU, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_ALL_DLVRD, row[iRowIndex], lstBaseBtns, getSharedPtr());
    createKBBtn(STR_BTN_OUTSTANDING, row[iRowIndex], lstBaseBtns, getSharedPtr());
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

    int iOutstanding = 0;
    std::stringstream ss;

    if(!pMsg->text.compare(STR_BTN_ALL_DLVRD)) {
        getDBHandle()->updateAllDelivered(fp);
        STR_MSG_DEFF_RELEASE = "All \"Confirmed-Yesterday's Order\" are marked as \"Delivered\".\n";
    }
    if(!pMsg->text.compare(STR_BTN_OUTSTANDING)) {
        iOutstanding = getDBHandle()->getAllOutstanding(fp);
        STR_MSG_DEFF_RELEASE = std::string("Total outstanding : ") + std::to_string(iOutstanding) + std::string("\n");
    }
    if(!pMsg->text.compare(STR_BTN_ORDR_SUMMRY)) {
        std::map<unsigned int, unsigned int>::iterator itr;
        std::map<unsigned int, unsigned int> ordrSum   = getDBHandle()->getOrderSummary(fp);
        for(itr =  ordrSum.begin(); itr != ordrSum.end(); itr++) {
            Product::Ptr pProd  = getDBHandle()->getProductById(itr->first, fp);
            if(pProd) ss << pProd->m_Name << " : " << itr->second << "\n";
        }
        STR_MSG_DEFF_RELEASE = std::string("<b>Order Summary</b>\n") + ss.str();
    }
    fprintf(fp, "BaseBot %ld: SGNAdmin onClick }\n", time(0)); fflush(fp);
}
