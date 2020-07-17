/*sgn
 * Products.cpp
 *
 *  Created on: 23-May-2020
 *      Author: tstone10
 */
#include <iostream>
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>
#include <ProductList.h>
#include <memory>

#include "pngwriter.h"

#include "ProductList.h"
#include "BaseButton.h"
#include "Constants.h"

std::string ProductList::STR_MSG_DEFF_RELEASE   = " ";

void ProductList::create_product_table(std::string file_name, FILE *fp) {
    fprintf(fp, "BaseBot %ld: ProductList::create_product_table {\n", time(0)); fflush(fp);

    pngwriter product_table(320, 320, 1.0, file_name.c_str());
    char toggle = 0;

    //  Headers
    std::string strFontFile = std::string(BOT_ROOT_PATH) + std::string(BOT_FONT_PATH) + std::string(BOT_FONT_FILE_BOLD);
    product_table.filledsquare(0, 300, 320, 320, 0.0, 0.5, 0.5);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12,     5, 305, 0.0, (char *)"SN", 1.0, 1.0, 1.0);
    std::string strHdr  = std::string("Tmrw: ") + getDBHandle()->getTmrwDate();
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12,  30+5, 305, 0.0, (char *)strHdr.c_str(), 1.0, 1.0, 1.0);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12, 190+5, 305, 0.0, (char *)"Code", 1.0, 1.0, 1.0);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12, 245+5, 305, 0.0, (char *)"Price", 1.0, 1.0, 1.0);

    //  Highlight alternate rows
    int iLoop = 0, iIndex = 0;
    iIndex = (iSelPage - 1) * MAX_ITEMS_PER_PAGE;

    for(iLoop = 300; iLoop > 0; iLoop -= 30, toggle = 1 - toggle) {
        if(0 == toggle) product_table.filledsquare(0, iLoop, 320, iLoop-30, 0.9, 0.9, 0.9);
        if((iNoOfItems > iIndex) && (iIndex < (iSelPage * MAX_ITEMS_PER_PAGE))) {
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10,     5, iLoop-20, 0.0, (char *)std::to_string(iIndex+1).c_str(), 0, 0, 0);
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10,  30+5, iLoop-20, 0.0, (char *)products[iIndex]->m_Name.c_str(), 0, 0, 0);
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10, 190+5, iLoop-20, 0.0, (char *)products[iIndex]->m_Code.c_str(), 0, 0, 0);
            std::string strPrice4Pk = std::to_string(products[iIndex]->m_Price) + std::string("/") + products[iIndex]->m_Pack;
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10, 245+5, iLoop-20, 0.0, (char *)(strPrice4Pk.c_str()), 0, 0, 0);
            iIndex++;
        }
    }

    //  Vertical lines
    product_table.line( 30, 320,  30, 0, 0, 0, 0);
    product_table.line(190, 320, 190, 0, 0, 0, 0);
    product_table.line(245, 320, 245, 0, 0, 0, 0);

    product_table.close();
    fprintf(fp, "BaseBot %ld: ProductList::create_product_table }\n", time(0)); fflush(fp);
}

TgBot::GenericReply::Ptr ProductList::prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& lstBaseBtns, TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: ProductList::prepareMenu {\n", time(0)); fflush(fp);

    std::string strText;
    std::vector<TgBot::KeyboardButton::Ptr> row[MAX_BUTTON_ROWS];
    unsigned int iLoop = 0, iPrev = 0, iNext = 0, iToggle = 0, iRowIndex = 0;
    TgBot::ReplyKeyboardMarkup::Ptr pMainMenu;

    iRowIndex = 0;
    for(iToggle = 0, iLoop = (iSelPage - 1) * MAX_ITEMS_PER_PAGE; (iNoOfItems > iLoop) && (iLoop < (iSelPage * MAX_ITEMS_PER_PAGE)); iLoop++, iToggle = (1-iToggle)) {
        createKBBtn(std::string("Buy ") + products[iLoop]->m_Code, row[iToggle], lstBaseBtns, getSharedPtr());
        iRowIndex   = (iRowIndex < (iToggle+1)) ? (iToggle+1) : iRowIndex;
    }

    //  Populate pages in next available row if no. of items more than MAX_ITEMS_PER_PAGE
    if(MAX_ITEMS_PER_PAGE < iNoOfItems) {
        iPrev       = (iSelPage == 1) ? 1 : (iSelPage - 1);
        strText     = std::string(PAGE_SUFFIX) + std::string(" ") + std::to_string(iPrev);
        createKBBtn(strText, row[iRowIndex], lstBaseBtns, getSharedPtr());

        iNext       = (iNoOfItems > (iSelPage * MAX_ITEMS_PER_PAGE)) ? (iSelPage + 1) : iSelPage;
        strText     = std::string(PAGE_SUFFIX) + std::string(" ") + std::to_string(iNext);
        createKBBtn(strText, row[iRowIndex], lstBaseBtns, getSharedPtr());

        //  Pick the next available rowa
        iRowIndex++;
    }

    //  Populate the next available row
    createKBBtn(STR_BTN_TOP_UP, row[iRowIndex], lstBaseBtns);
    iRowIndex++;

    if(isAdmin) createKBBtn(STR_BTN_ADMIN_PG, row[iRowIndex], lstBaseBtns);
    else createKBBtn(STR_BTN_FAQ, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_YOUR_ORDERS, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_VIEW_CART, row[iRowIndex], lstBaseBtns);
    iRowIndex++;

    //  Add all the rows to main menu
    pMainMenu   = std::make_shared<TgBot::ReplyKeyboardMarkup>();
    for(iLoop = 0; iLoop < iRowIndex; iLoop++) {
        pMainMenu->keyboard.push_back(row[iLoop]);
    }

    fprintf(fp, "BaseBot %ld: Finishing ProductList::prepareMenu }\n", time(0)); fflush(fp);
    pMainMenu->resizeKeyboard   = true;
    return pMainMenu;
}

void ProductList::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: ProductList onClick pMsg %s {\n", time(0), pMsg->text.c_str()); fflush(fp);
    getDBHandle()->addNewUser(pMsg->chat->id, pMsg->from->firstName, fp);

    std::vector<unsigned int>::const_iterator itr;
    for(itr = adminChatIds.begin(); itr != adminChatIds.end(); itr++) if(*itr == pMsg->chat->id) { isAdmin = true; break; }
    unsigned int iLoop = 0, iOrderNo;
    std::stringstream ss;

    products    = getDBHandle()->getAllActiveProducts(fp);
    iSelPage    = 1;
    iNoOfItems  = products.size();

    User::Ptr pUser = getDBHandle()->getUserForChatId(pMsg->chat->id, fp);

    //  Check if reached here through Category Paging Button
    if(std::string::npos != pMsg->text.find(PAGE_SUFFIX))
        for(iLoop = 1; iLoop <= MAX_NO_OF_PAGES; iLoop++) {
            std::stringstream ssCat;
            ssCat << PAGE_SUFFIX << " " << iLoop;

            if(!pMsg->text.compare(ssCat.str())) {
                iSelPage    = iLoop;
                break;
            }
        }

    std::string strCode;
    if(std::string::npos != pMsg->text.find("Buy")) {
        std::stringstream ss;
        //  Buy TF-01
        strCode = pMsg->text.substr(4);  // yields TF-01
        Product::Ptr pProd  = getDBHandle()->getProductForCode(strCode,fp);

        //  Find page number of this code
        for(iLoop = 0; iLoop < iNoOfItems; iLoop++) if(products[iLoop]->m_ProductId == pProd->m_ProductId) break;
        if(iLoop < iNoOfItems) iSelPage = ((iLoop+1) / MAX_ITEMS_PER_PAGE) + (0 != ((iLoop+1) % MAX_ITEMS_PER_PAGE));

        int iQty = getDBHandle()->addProductToCart(pProd->m_ProductId, pProd->m_Price, pMsg->chat->id, fp);
        ss << "<b>" << pProd->m_Name << " * " << iQty << "</b> added to Cart. Clicking it again, increases quantity.";
        STR_MSG_DEFF_RELEASE = ss.str();
    }

    if(std::string::npos != pMsg->text.find(STR_BTN_EMPTY_CART)) {
        getDBHandle()->emptyCartForUser(pUser->m_OrderNo, fp);
        STR_MSG_DEFF_RELEASE = std::string("Your cart is empty now.\n") + STR_MSG_DEFF_RELEASE;
    }

    if(!pMsg->text.compare(STR_BTN_CNF_CHECKOUT)) {
        int iTotal = 0;

        std::vector<Cart::Ptr> cartItems = getDBHandle()->getCartItemsForOrderNo(pUser->m_OrderNo, fp);
        iTotal = 0; for(auto &item : cartItems) iTotal += (item->m_Qnty * item->m_Price);
        getDBHandle()->insertToOrder(pUser, iTotal, CartStatus::PAYMENT_PENDING, STR_WALLET, OrderType::PORDER, fp);

        iOrderNo    = pUser->m_OrderNo;
        getDBHandle()->updateOrderNo(pUser->m_UserId, fp);

        ss << pUser->m_Name << " has made an order, " << iOrderNo << ", using " << STR_WALLET;
        for(auto &id : adminChatIds)  notifyMsgs[id] = ss.str();
        STR_MSG_DEFF_RELEASE = "Your order is placed. You will get a confirmation msg in a few hours.";
    }
    fprintf(fp, "BaseBot %ld: ProductList onClick }\n", time(0)); fflush(fp);
}

TgBot::InputFile::Ptr ProductList::getMedia(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: ProductList getMedia {\n", time(0)); fflush(fp);
    TgBot::InputFile::Ptr pFile = nullptr;

    asset_file  = std::string(BOT_ROOT_PATH) + std::string(BOT_ASSETS_PATH) +
                    std::string("active_product_list_") + std::to_string(iSelPage) + std::string(".png");
    create_product_table(asset_file, fp);

    if(isFileExists(asset_file)) pFile = TgBot::InputFile::fromFile(asset_file, "image/png");
    else { fprintf(fp, "Fatal Error: ProductList::getMedia asset file, %s, missing\n", asset_file.c_str()); fflush(fp); }

    fprintf(fp, "BaseBot %ld: ProductList getMedia }\n", time(0)); fflush(fp);
    return pFile;
}

