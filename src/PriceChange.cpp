/*sgn
 * PriceChange.cpp
 *
 *  Created on: 06-Jun-2020
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
#include "PriceChange.h"

std::string PriceChange::STR_MSG_DEFF_RELEASE   = "";

void PriceChange::create_product_table(std::string file_name, int iPageNo, FILE *fp) {
    fprintf(fp, "BaseBot %ld: ProductList::create_product_table {\n", time(0)); fflush(fp);

    pngwriter product_table(320, 320, 1.0, file_name.c_str());
    char toggle = 0;

    //  Headers
    std::string strFontFile = std::string(BOT_ROOT_PATH) + std::string(BOT_FONT_PATH) + std::string(BOT_FONT_FILE_BOLD);
    product_table.filledsquare(0, 300, 320, 320, 0.0, 0.5, 0.5);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12,     5, 305, 0.0, (char *)"Id", 1.0, 1.0, 1.0);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12,  40+5, 305, 0.0, (char *)"Product", 1.0, 1.0, 1.0);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12, 230+5, 305, 0.0, (char *)"Pack", 1.0, 1.0, 1.0);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12, 285+5, 305, 0.0, (char *)"Rs", 1.0, 1.0, 1.0);

    //  Highlight alternate rows
    int iLoop = 0, iIndex = 0;
    iIndex = (iPageNo - 1) * MAX_ITEMS_PER_PAGE;

    for(iLoop = 300; iLoop > 0; iLoop -= 30, toggle = 1 - toggle) {
        if(0 == toggle) product_table.filledsquare(0, iLoop, 320, iLoop-30, 0.9, 0.9, 0.9);
        if((iNoOfItems > iIndex) && (iIndex < (iPageNo * MAX_ITEMS_PER_PAGE))) {
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10,     5, iLoop-20, 0.0, (char *)std::to_string(products[iIndex]->m_ProductId).c_str(), 0, 0, 0);
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10,  40+5, iLoop-20, 0.0, (char *)products[iIndex]->m_Name.c_str(), 0, 0, 0);
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10, 230+5, iLoop-20, 0.0, (char *)products[iIndex]->m_Pack.c_str(), 0, 0, 0);
            std::string strPackPrice = std::to_string(products[iIndex]->m_Price);
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10, 285+5, iLoop-20, 0.0, (char *)strPackPrice.c_str(), 0, 0, 0);
            iIndex++;
        }
    }

    //  Vertical lines
    product_table.line( 40, 320,  40, 0, 0, 0, 0);
    product_table.line(230, 320, 230, 0, 0, 0, 0);
    product_table.line(285, 320, 285, 0, 0, 0, 0);
    product_table.line(320, 320, 320, 0, 0, 0, 0);

    product_table.close();
    fprintf(fp, "BaseBot %ld: ProductList::create_product_table }\n", time(0)); fflush(fp);
}

TgBot::GenericReply::Ptr PriceChange::prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& lstBaseBtns, TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: PriceChange::prepareMenu {\n", time(0)); fflush(fp);

    std::string strText;
    std::vector<TgBot::KeyboardButton::Ptr> row[MAX_BUTTON_ROWS];
    unsigned int iLoop = 0, iRowIndex = 0;
    TgBot::ReplyKeyboardMarkup::Ptr pMainMenu;

    std::map<unsigned int, UserContext>::const_iterator itrCntxt;
    std::map<std::string, std::shared_ptr<BaseButton> >::const_iterator itrBtn;
    std::string strChatId   = std::to_string(pMsg->chat->id);

    if(!pMsg->text.compare(STR_BTN_EDIT_PRICE)) {
        m_Context[pMsg->chat->id] = USER_CTXT_PRICE_CHANGE;
        lstBaseBtns[strChatId]  = getSharedPtr();
        STR_MSG_DEFF_RELEASE  = std::string("Change the price of product by typing,\n\"id - price, id - price, id - price...\"") +
                                        std::string("\n\nExample:\n<b> Id  |Product                   |  Pack| Rs</b>") +
                                        std::string("\n21 | Cauliflower Kootu |250ml| 50") +
                                        std::string("\n28 | Beetroot Curry        |200g  | 50") +
                                        std::string("\n\nTo change price for Cauliflower Kootu 1pk & Beetroot Curry 1pk") +
                                        std::string("\nType: <b>21-45, 28-60,</b> etc.. and send");
        return std::make_shared<TgBot::ReplyKeyboardRemove>();
    }

    if(!pMsg->text.compare(STR_BTN_ADD_PROD)) {
        m_Context[pMsg->chat->id]   = USER_CTXT_ADD_A_PROD;
        lstBaseBtns[strChatId]      = getSharedPtr();
        STR_MSG_DEFF_RELEASE  = std::string("Add a product by typing,\n\"Category(as below), Name (20 chars), Price (only numbers) & send\"") +
                                        std::string("\n\nExample:\nTF,   Chappathi, 60") +
                                        std::string("\n:\nNO,   Ama Vadai, 30") +
                                        std::string("\nCR,   Beans Curry,   30") +
                                        std::string("\nSB,   Onion Sambar,   30") +
                                        std::string("\nKT,   Tindora Kootu,   30") +
                                        std::string("\nRS,   Mysore Rasam,   30");
        return std::make_shared<TgBot::ReplyKeyboardRemove>();
    }

    iRowIndex = 0;
    if(m_Context.end() != (itrCntxt = m_Context.find(pMsg->chat->id))) {
        m_Context.erase(itrCntxt);
        if(lstBaseBtns.end() != (itrBtn = lstBaseBtns.find(strChatId))) lstBaseBtns.erase(itrBtn);
    }

    //  Populate the next available row
    createKBBtn(STR_BTN_ITEM_LIST, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_EDIT_MENU, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_EDIT_PRICE, row[iRowIndex], lstBaseBtns);
    iRowIndex++;

    createKBBtn(STR_BTN_ADD_PROD, row[iRowIndex], lstBaseBtns);
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

    fprintf(fp, "BaseBot %ld: Finishing PriceChange::prepareMenu }\n", time(0)); fflush(fp);
    pMainMenu->resizeKeyboard   = true;
    return pMainMenu;
}

void PriceChange::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: PriceChange onClick pMsg %s {\n", time(0), pMsg->text.c_str()); fflush(fp);
    std::map<unsigned int, UserContext>::const_iterator itrCntxt;

    if(m_Context.end() != (itrCntxt = m_Context.find(pMsg->chat->id))) {
        std::string tmpstr, id, price, prod, strMsg = pMsg->text;
        std::stringstream ssMsg;

        ssMsg.str(pMsg->text);
        if(USER_CTXT_PRICE_CHANGE == itrCntxt->second) {
            if(std::string::npos != pMsg->text.find_first_not_of("0123456789 -,")) {
                fprintf(fp, "BaseBot %ld: PriceChange invalid input: %s\n", time(0), pMsg->text.c_str()); fflush(fp);
                STR_MSG_DEFF_RELEASE = "\nInvalid Input";
                return;
            }
            while(std::getline(ssMsg, id, '-')) {
                std::getline(ssMsg, price, ',');

                id = myTrim(id); price = myTrim(price);
                bool isUpdated      = getDBHandle()->updateProductPrice(id, price, fp);
                Product::Ptr pProd  = getDBHandle()->getProductById(std::stoi(id), fp);

                if(!isUpdated || !pProd) STR_MSG_DEFF_RELEASE += (id + std::string("-") + price + " : Invalid\n");
                else STR_MSG_DEFF_RELEASE += (pProd->m_Name + std::string(", ") + pProd->m_Pack + std::string(" - ₹ ")
                                        + std::to_string(pProd->m_Price) + "\n");
            }
        }
        if(USER_CTXT_ADD_A_PROD == itrCntxt->second) {
            std::vector<std::string> tokens;
            bool isAdded = false;
            while(std::getline(ssMsg, prod, ',')) {
                tokens.push_back(myTrim(prod));
            }
            if(PROD_TOKENS == tokens.size()) {
                isAdded = getDBHandle()->insertNewProduct(tokens[0], tokens[1], tokens[2], fp);
            }
            if(!isAdded) STR_MSG_DEFF_RELEASE = "Something went wrong. Not added.";
            else  STR_MSG_DEFF_RELEASE = std::string("Successfully added item ") + tokens[1];
        }
    } else if(!pMsg->text.compare(STR_BTN_ITEM_LIST)) {
        products    = getDBHandle()->getAllProducts(fp);
        iNoOfItems  = products.size();
        STR_MSG_DEFF_RELEASE = "Price List";
    } else STR_MSG_DEFF_RELEASE = "Change product price";

    fprintf(fp, "BaseBot %ld: PriceChange onClick }\n", time(0)); fflush(fp);
}

std::vector<TgBot::InputFile::Ptr> PriceChange::getMedias(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: PriceChange getMedia {\n", time(0)); fflush(fp);
    std::vector<TgBot::InputFile::Ptr> pFiles;
    int iNoOfPages, iLoop;

    plsWait = false;
    iNoOfPages  = (iNoOfItems / MAX_ITEMS_PER_PAGE) + (0 != (iNoOfItems % MAX_ITEMS_PER_PAGE));
    for(iLoop = 1; iLoop <= iNoOfPages; iLoop++) {
        asset_file  = std::string(BOT_ROOT_PATH) + std::string(BOT_ASSETS_PATH) +
                        std::string("_price_list_") + std::to_string(iLoop) + std::string(".png");
        create_product_table(asset_file, iLoop, fp);
        if(isFileExists(asset_file)) pFiles.push_back(TgBot::InputFile::fromFile(asset_file, "image/png"));
    }

    fprintf(fp, "BaseBot %ld: PriceChange getMedia }\n", time(0)); fflush(fp);
    return pFiles;
}


