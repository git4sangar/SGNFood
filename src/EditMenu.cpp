/*sgn
 * EditMenu.cpp
 *
 *  Created on: 02-Jul-2020
 *      Author: tstone10
 */




#include <iostream>
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>
#include <EditMenu.h>
#include <memory>

#include "pngwriter.h"

#include "BaseButton.h"
#include "Constants.h"

std::string EditMenu::STR_MSG_DEFF_RELEASE   = " ";

void EditMenu::create_product_table(std::string file_name, FILE *fp) {
    fprintf(fp, "BaseBot %ld: EditMenu::create_product_table {\n", time(0)); fflush(fp);

    pngwriter product_table(320, 320, 1.0, file_name.c_str());
    char toggle = 0;

    //  Headers
    std::string strFontFile = std::string(BOT_ROOT_PATH) + std::string(BOT_FONT_PATH) + std::string(BOT_FONT_FILE_BOLD);
    product_table.filledsquare(0, 300, 320, 320, 0.0, 0.5, 0.5);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12,     5, 305, 0.0, (char *)"Id", 1.0, 1.0, 1.0);
    std::string strTblHdr   = std::string("Menu : ") + getDBHandle()->getTmrwDate();
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12, 30+15, 305, 0.0, (char *)strTblHdr.c_str(), 1.0, 1.0, 1.0);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12, 245+5, 305, 0.0, (char *)"Price", 1.0, 1.0, 1.0);

    //  Highlight alternate rows
    int iLoop = 0, iIndex = 0;
    iIndex = (iSelPage - 1) * MAX_ITEMS_PER_PAGE;

    for(iLoop = 300; iLoop > 0; iLoop -= 30, toggle = 1 - toggle) {
        if(0 == toggle) product_table.filledsquare(0, iLoop, 320, iLoop-30, 0.9, 0.9, 0.9);
        if((iNoOfItems > iIndex) && (iIndex < (iSelPage * MAX_ITEMS_PER_PAGE))) {
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10,     5, iLoop-20, 0.0, (char *)std::to_string(products[iIndex]->m_ProductId).c_str(), 0, 0, 0);
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10,  30+5, iLoop-20, 0.0, (char *)products[iIndex]->m_Name.c_str(), 0, 0, 0);
            //product_table.plot_text_utf8((char *)strFontFile.c_str(), 10, 200+5, iLoop-20, 0.0, (char *)products[iIndex]->m_Code.c_str(), 0, 0, 0);
            std::string strPrice4Pk = std::to_string(products[iIndex]->m_Price) + std::string("/") + products[iIndex]->m_Pack;
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10, 245+5, iLoop-20, 0.0, (char *)(strPrice4Pk.c_str()), 0, 0, 0);
            iIndex++;
        }
    }

    //  Vertical lines
    product_table.line( 30, 320,  30, 0, 0, 0, 0);
    //product_table.line(200, 320, 200, 0, 0, 0, 0);
    product_table.line(245, 320, 245, 0, 0, 0, 0);

    product_table.close();
    fprintf(fp, "BaseBot %ld: EditMenu::create_product_table }\n", time(0)); fflush(fp);
}

TgBot::GenericReply::Ptr EditMenu::prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& lstBaseBtns, TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: EditMenu::prepareMenu {\n", time(0)); fflush(fp);

    std::string strText, strBtn;
    std::vector<TgBot::KeyboardButton::Ptr> row[MAX_BUTTON_ROWS];
    std::vector<std::string> strBtns;
    unsigned int iPrev, iNext, iLoop, iRowIndex = 0, iItemsPerRow = 0, iItems, iRowsReqd;
    TgBot::ReplyKeyboardMarkup::Ptr pMainMenu;

    iRowIndex = 0;
    if(0 < iNoOfItems) {
        for(iLoop = ((iSelPage - 1) * MAX_ITEMS_PER_PAGE);
                (iNoOfItems > iLoop) && (iLoop < (iSelPage * MAX_ITEMS_PER_PAGE)) && pMsg->text.compare(STR_BTN_SHOW_MENU);
                iLoop++) {
            strBtn  = std::string("+") + std::to_string(products[iLoop]->m_ProductId);
            strBtns.push_back(strBtn);
        }

        for(iLoop = ((iSelPage - 1) * MAX_ITEMS_PER_PAGE);
                (iNoOfItems > iLoop) && (iLoop < (iSelPage * MAX_ITEMS_PER_PAGE));
                iLoop++) {
            strBtn  = std::string("-") + std::to_string(products[iLoop]->m_ProductId);
            strBtns.push_back(strBtn);
        }
        iItems          = strBtns.size();
        iRowsReqd       = (iItems / MAX_ADD_RMV_BTNS_PER_ROW) + (0 != (iItems % MAX_ADD_RMV_BTNS_PER_ROW));
        iItemsPerRow    = (iItems / iRowsReqd) + (0 != (iItems % iRowsReqd) ? 1 : 0);
        for(iLoop = 0; iLoop < iItems; iLoop++) {
            createKBBtn(strBtns[iLoop], row[iLoop/iItemsPerRow], lstBaseBtns);
        }
        iRowIndex   = iRowsReqd;
    }

    //  Populate pages in next available row if no. of items more than MAX_ITEMS_PER_PAGE
    if(MAX_ITEMS_PER_PAGE < iNoOfItems) {
        iPrev       = (iSelPage == 1) ? 1 : (iSelPage - 1);
        strText     = std::string(ITEMS_PAGE) + std::string(" ") + std::to_string(iPrev);
        createKBBtn(strText, row[iRowIndex], lstBaseBtns, getSharedPtr());

        iNext       = (iNoOfItems > (iSelPage * MAX_ITEMS_PER_PAGE)) ? (iSelPage + 1) : iSelPage;
        strText     = std::string(ITEMS_PAGE) + std::string(" ") + std::to_string(iNext);
        createKBBtn(strText, row[iRowIndex], lstBaseBtns, getSharedPtr());
    }
    if(pMsg->text.compare(STR_BTN_SHOW_MENU))
        createKBBtn(STR_BTN_SHOW_MENU, row[iRowIndex], lstBaseBtns);
    else
        createKBBtn(STR_BTN_DONE_MENU, row[iRowIndex], lstBaseBtns);
    iRowIndex++;

    //  Populate the next available row
    createKBBtn(STR_BTN_MENU_MGMT, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_MAINMENU, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_ADMIN_PG, row[iRowIndex], lstBaseBtns);
    iRowIndex++;

    //  Add all the rows to main menu
    pMainMenu   = std::make_shared<TgBot::ReplyKeyboardMarkup>();
    for(iLoop = 0; iLoop < iRowIndex; iLoop++) {
        pMainMenu->keyboard.push_back(row[iLoop]);
    }

    fprintf(fp, "BaseBot %ld: Finishing EditMenu::prepareMenu }\n", time(0)); fflush(fp);
    return pMainMenu;
}

void EditMenu::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: EditMenu onClick pMsg %s {\n", time(0), pMsg->text.c_str()); fflush(fp);
    getDBHandle()->addNewUser(pMsg->chat->id, pMsg->from->firstName, fp);

    bool isAdmin = false;
    std::vector<unsigned int>::const_iterator itr;
    for(itr = adminChatIds.begin(); itr != adminChatIds.end(); itr++) if(*itr == pMsg->chat->id) { isAdmin = true; break; }

    if(!isAdmin) return;

    products    = getDBHandle()->getAllProducts(fp);
    iSelPage    = 1;
    iNoOfItems  = products.size();

    std::vector<Product::Ptr> actvProds = getDBHandle()->getAllActiveProducts(fp);
    if(!pMsg->text.compare(STR_BTN_SHOW_MENU)) {
        products    = actvProds;
        iSelPage    = 1;
        iNoOfItems  = products.size();
    }

    if(!pMsg->text.compare(STR_BTN_DONE_MENU)) {
        if(actvProds.empty()) {
            STR_MSG_DEFF_RELEASE = "<b>Tomorrow's menu is empty. Pls create it first.</b>";
        } else {
            std::vector<User::Ptr> users    = getDBHandle()->getAllUsers(fp);
            for(auto &user : users) { notifyMsgs[user->m_ChatId] = std::string("Hi ") + user->m_Name + ", tomorrow's menu is ready. Pls place your order before it is too late.";}
        }
    }

    //  Is pagination?
    unsigned int iLoop = 0;
    if(std::string::npos != pMsg->text.find(ITEMS_PAGE))
        for(iLoop = 1; iLoop <= MAX_NO_OF_PAGES; iLoop++) {
            std::stringstream ssCat;
            ssCat << ITEMS_PAGE << " " << iLoop;

            if(!pMsg->text.compare(ssCat.str())) {
                iSelPage    = iLoop;
                break;
            }
        }

    std::string strId;
    if('+' == pMsg->text[0] || '-' == pMsg->text[0]) {
        //  +1 or -1
        strId = pMsg->text.substr(1);  // yields 1
        for(iLoop = 0; iLoop < iNoOfItems; iLoop++) if(products[iLoop]->m_ProductId == std::stoi(strId)) break;

        if(iLoop < iNoOfItems) {
            iSelPage    = ((iLoop+1) / MAX_ITEMS_PER_PAGE) + (0 != ((iLoop+1) % MAX_ITEMS_PER_PAGE));

            std::stringstream ss;
            std::string strDate = getDBHandle()->getTmrwDate();
            if('+' == pMsg->text[0]) {
                getDBHandle()->activateProductForTomorrow(products[iLoop]->m_ProductId, fp);
                ss << "Added to " << strDate << " menu: " << products[iLoop]->m_Name;
            }
            if('-' == pMsg->text[0]) {
                getDBHandle()->removeProductFromTomorrow(products[iLoop]->m_ProductId, fp);
                ss << "Removed from " << strDate << " menu: " << products[iLoop]->m_Name;
            }
            STR_MSG_DEFF_RELEASE = ss.str();
        }
    }

    fprintf(fp, "BaseBot %ld: EditMenu onClick }\n", time(0)); fflush(fp);
}

TgBot::InputFile::Ptr EditMenu::getMedia(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: EditMenu getMedia {\n", time(0)); fflush(fp);
    TgBot::InputFile::Ptr pFile = nullptr;

    asset_file  = std::string(BOT_ROOT_PATH) + std::string(BOT_ASSETS_PATH) +
                    std::string("_all_product_list_") + std::to_string(iSelPage) + std::string(".png");
    create_product_table(asset_file, fp);

    if(isFileExists(asset_file)) pFile = TgBot::InputFile::fromFile(asset_file, "image/png");
    else { fprintf(fp, "Fatal Error: EditMenu::getMedia asset file, %s, missing\n", asset_file.c_str()); fflush(fp); }

    fprintf(fp, "BaseBot %ld: EditMenu getMedia }\n", time(0)); fflush(fp);
    return pFile;
}

