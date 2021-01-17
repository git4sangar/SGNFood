/*sgn
 * SpecialList.cpp
 *
 *  Created on: 23-May-2020
 *      Author: tstone10
 */
#include <iostream>
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>
#include <memory>

#include "pngwriter.h"

#include "SpecialList.h"
#include "BaseButton.h"
#include "Constants.h"

std::string SpecialList::STR_MSG_DEFF_RELEASE   = " ";

void SpecialList::create_product_table(std::string file_name, FILE *fp) {
    fprintf(fp, "BaseBot %ld: SpecialList::create_product_table {\n", time(0)); fflush(fp);

    pngwriter product_table(320, 320, 1.0, file_name.c_str());
    char toggle = 0;

    //  Headers
    std::string strFontFile = std::string(BOT_ROOT_PATH) + std::string(BOT_FONT_PATH) + std::string(BOT_FONT_FILE_BOLD);
    product_table.filledsquare(0, 300, 320, 320, 0.0, 0.5, 0.5);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12,     5, 305, 0.0, (char *)"SN", 1.0, 1.0, 1.0);
    std::string strHdr  = products[0]->m_Code;
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12,  30+5, 305, 0.0, (char *)strHdr.c_str(), 1.0, 1.0, 1.0);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12, 230+5, 305, 0.0, (char *)"Price", 1.0, 1.0, 1.0);

    //  Highlight alternate rows
    int iLoop = 0, iIndex = 0;
    iIndex = (iSelPage - 1) * MAX_ITEMS_PER_PAGE;

    for(iLoop = 300; iLoop > 0; iLoop -= 30, toggle = 1 - toggle) {
        if(0 == toggle) product_table.filledsquare(0, iLoop, 320, iLoop-30, 0.9, 0.9, 0.9);
        if((iNoOfItems > iIndex) && (iIndex < (iSelPage * MAX_ITEMS_PER_PAGE))) {
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10,     5, iLoop-20, 0.0, (char *)std::to_string(iIndex+1).c_str(), 0, 0, 0);
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10,  30+5, iLoop-20, 0.0, (char *)products[iIndex]->m_Name.c_str(), 0, 0, 0);
            std::string strPrice4Pk = std::to_string(products[iIndex]->m_Price) + std::string("/") + products[iIndex]->m_Pack;
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10, 230+5, iLoop-20, 0.0, (char *)(strPrice4Pk.c_str()), 0, 0, 0);
            iIndex++;
        }
    }

    //  Vertical lines
    product_table.line( 30, 320,  30, 0, 0, 0, 0);
    product_table.line(230, 320, 230, 0, 0, 0, 0);

    product_table.close();
    fprintf(fp, "BaseBot %ld: SpecialList::create_product_table }\n", time(0)); fflush(fp);
}

TgBot::GenericReply::Ptr SpecialList::prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& lstBaseBtns, TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: SpecialList::prepareMenu {\n", time(0)); fflush(fp);

    std::string strText;
    std::vector<TgBot::KeyboardButton::Ptr> row[MAX_BUTTON_ROWS];
    unsigned int iLoop = 0, iPrev = 0, iNext = 0, iToggle = 0, iRowIndex = 0;
    TgBot::ReplyKeyboardMarkup::Ptr pMainMenu;
    std::map<unsigned int, UserContext>::const_iterator itrCntxt;
    std::map<std::string, std::shared_ptr<BaseButton> >::const_iterator itrBtn;

    iRowIndex = 0;
    if(0 < iNoOfItems) {
        for(auto &cat : categories) createKBBtn(cat, row[iRowIndex], lstBaseBtns, getSharedPtr());
        iRowIndex++;

        for(iToggle = iRowIndex, iLoop = 0; iNoOfItems > iLoop; iLoop++, iToggle = (3-iToggle)) {
            createKBBtn(products[iLoop]->m_Name, row[iToggle], lstBaseBtns, lstBaseBtns["Quick Menu"]);
            iRowIndex   = (iRowIndex < (iToggle+1)) ? (iToggle+1) : iRowIndex;
        }
    }

    createKBBtn(STR_BTN_VIEW_CART, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_MAINMENU, row[iRowIndex], lstBaseBtns);
    iRowIndex++;

    //  Add all the rows to main menu
    pMainMenu   = std::make_shared<TgBot::ReplyKeyboardMarkup>();
    for(iLoop = 0; iLoop < iRowIndex; iLoop++) {
        pMainMenu->keyboard.push_back(row[iLoop]);
    }

    fprintf(fp, "BaseBot %ld: Finishing SpecialList::prepareMenu }\n", time(0)); fflush(fp);
    pMainMenu->resizeKeyboard   = true;
    return pMainMenu;
}

void SpecialList::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: SpecialList onClick pMsg %s {\n", time(0), pMsg->text.c_str()); fflush(fp);

    categories  = getDBHandle()->getCategories(fp);
    category    = categories[0];
    iSelPage    = 1;

    //  Is category clicked
    for(auto &cat: categories)
        if(!pMsg->text.compare(cat)) {
            category   = cat;
            break;
        }

    //  is product clicked?
    Product::Ptr pProd;
    if(nullptr != (pProd = getDBHandle()->getProductByName(pMsg->text, fp))) {
        category    = pProd->m_Code;
    }

    products    = getDBHandle()->getProductsForCategory(category, fp);
    iNoOfItems  = products.size();

    fprintf(fp, "BaseBot %ld: SpecialList onClick }\n", time(0)); fflush(fp);
}

TgBot::InputFile::Ptr SpecialList::getMedia(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: SpecialList getMedia {\n", time(0)); fflush(fp);
    TgBot::InputFile::Ptr pFile = nullptr;

    if(0 < iNoOfItems) {
        asset_file  = std::string(BOT_ROOT_PATH) + std::string(BOT_ASSETS_PATH) +
                    std::string("active_product_list_") + std::to_string(iSelPage) + std::string(".png");
        create_product_table(asset_file, fp);

        if(isFileExists(asset_file)) pFile = TgBot::InputFile::fromFile(asset_file, "image/png");
        else { fprintf(fp, "Fatal Error: SpecialList::getMedia asset file, %s, missing\n", asset_file.c_str()); fflush(fp); }
    }
    fprintf(fp, "BaseBot %ld: SpecialList getMedia }\n", time(0)); fflush(fp);
    return pFile;
}

