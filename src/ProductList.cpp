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
#ifdef AURA
void ProductList::create_product_table(std::string file_name, std::string strHdr, FILE *fp) {
    fprintf(fp, "BaseBot %ld: ProductList::create_product_table {\n", time(0)); fflush(fp);

    pngwriter product_table(320, 320, 1.0, file_name.c_str());
    char toggle = 0;

    //  Headers
    std::string strFontFile = std::string(BOT_ROOT_PATH) + std::string(BOT_FONT_PATH) + std::string(BOT_FONT_FILE_BOLD);
    product_table.filledsquare(0, 300, 320, 320, 0.0, 0.5, 0.5);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12,     5, 305, 0.0, (char *)"SN", 1.0, 1.0, 1.0);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12,  30+5, 305, 0.0, (char *)strHdr.c_str(), 1.0, 1.0, 1.0);

    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12,  160+5, 305, 0.0, (char *)"SN", 1.0, 1.0, 1.0);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12,  190+5, 305, 0.0, (char *)strHdr.c_str(), 1.0, 1.0, 1.0);

    //  Highlight alternate rows
    int iLoop = 0, iIndex = 0, yAxis = 300;
    iIndex = (iSelPage - 1) * MAX_ITEMS_PER_PAGE;

    for(iLoop = 300; iLoop > 0; iLoop -= 30, toggle = 1 - toggle) {
        if(0 == toggle) product_table.filledsquare(0, iLoop, 320, iLoop-30, 0.9, 0.9, 0.9);
        if(iNoOfItems > iIndex) {
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10,     5, iLoop-20, 0.0, (char *)std::to_string(iIndex+1).c_str(), 0, 0, 0);
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10,  30+5, iLoop-20, 0.0, (char *)products[iIndex]->m_Name.c_str(), 0, 0, 0);
            if(iNoOfItems > (iIndex+1)) {
                product_table.plot_text_utf8((char *)strFontFile.c_str(), 10, 160+5, iLoop-20, 0.0, (char *)std::to_string(iIndex+2).c_str(), 0, 0, 0);
                product_table.plot_text_utf8((char *)strFontFile.c_str(), 10, 190+5, iLoop-20, 0.0, (char *)products[iIndex+1]->m_Name.c_str(), 0, 0, 0);
            }
            iIndex+=2; yAxis -= 30;
        }
    }

    //  Vertical lines
    product_table.line( 30, 320,  30, yAxis, 0, 0, 0);
    product_table.line(160, 320, 160, yAxis, 0, 0, 0);
    product_table.line(190, 320, 190, yAxis, 0, 0, 0);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 10, 5, 40, 0.0, (char *)"Price ₹ 130/pack", 0, 0, 0);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 10, 5, 10, 0.0, (char *)"A pack contains 2 soaps.", 0, 0, 0);

    product_table.close();
    fprintf(fp, "BaseBot %ld: ProductList::create_product_table }\n", time(0)); fflush(fp);
}
#else
void ProductList::create_product_table(std::string file_name, std::string strHdr, FILE *fp) {
    fprintf(fp, "BaseBot %ld: ProductList::create_product_table {\n", time(0)); fflush(fp);

    pngwriter product_table(320, 320, 1.0, file_name.c_str());
    char toggle = 0;

    //  Headers
    std::string strFontFile = std::string(BOT_ROOT_PATH) + std::string(BOT_FONT_PATH) + std::string(BOT_FONT_FILE_BOLD);
    product_table.filledsquare(0, 300, 320, 320, 0.0, 0.5, 0.5);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12,     5, 305, 0.0, (char *)"SN", 1.0, 1.0, 1.0);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12,  30+5, 305, 0.0, (char *)strHdr.c_str(), 1.0, 1.0, 1.0);
    //product_table.plot_text_utf8((char *)strFontFile.c_str(), 12, 190+5, 305, 0.0, (char *)"Code", 1.0, 1.0, 1.0);
    product_table.plot_text_utf8((char *)strFontFile.c_str(), 12, 245+5, 305, 0.0, (char *)"Price", 1.0, 1.0, 1.0);

    //  Highlight alternate rows
    int iLoop = 0, iIndex = 0;
    iIndex = (iSelPage - 1) * MAX_ITEMS_PER_PAGE;

    for(iLoop = 300; iLoop > 0; iLoop -= 30, toggle = 1 - toggle) {
        if(0 == toggle) product_table.filledsquare(0, iLoop, 320, iLoop-30, 0.9, 0.9, 0.9);
        if((iNoOfItems > iIndex) && (iIndex < (iSelPage * MAX_ITEMS_PER_PAGE))) {
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10,     5, iLoop-20, 0.0, (char *)std::to_string(iIndex+1).c_str(), 0, 0, 0);
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10,  30+5, iLoop-20, 0.0, (char *)products[iIndex]->m_Name.c_str(), 0, 0, 0);
            //product_table.plot_text_utf8((char *)strFontFile.c_str(), 10, 190+5, iLoop-20, 0.0, (char *)products[iIndex]->m_Code.c_str(), 0, 0, 0);
            std::string strPrice4Pk = std::to_string(products[iIndex]->m_Price) + std::string("/") + products[iIndex]->m_Pack;
            product_table.plot_text_utf8((char *)strFontFile.c_str(), 10, 245+5, iLoop-20, 0.0, (char *)(strPrice4Pk.c_str()), 0, 0, 0);
            iIndex++;
        }
    }

    //  Vertical lines
    product_table.line( 30, 320,  30, 0, 0, 0, 0);
    //product_table.line(190, 320, 190, 0, 0, 0, 0);
    product_table.line(245, 320, 245, 0, 0, 0, 0);

    product_table.close();
    fprintf(fp, "BaseBot %ld: ProductList::create_product_table }\n", time(0)); fflush(fp);
}
#endif

TgBot::GenericReply::Ptr ProductList::prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& lstBaseBtns, TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: ProductList::prepareMenu {\n", time(0)); fflush(fp);

    std::vector<TgBot::KeyboardButton::Ptr> row[MAX_BUTTON_ROWS];
    unsigned int iLoop = 0, iPrev = 0, iNext = 0, iToggle = 0, iRowIndex = 0, iCurItms = 0, iCount = 0;
    TgBot::ReplyKeyboardMarkup::Ptr pMainMenu;
    std::stringstream ss;
    std::map<unsigned int, UserContext>::const_iterator itrCntxt;
    std::map<std::string, std::shared_ptr<BaseButton> >::const_iterator itrBtn;

    std::string strChatId   = std::to_string(pMsg->chat->id);
    if(!pMsg->text.compare(STR_BTN_MSG_ADMIN)) {
        m_Context[pMsg->chat->id]   = USER_CTXT_ADMING_MSG;
        lstBaseBtns[strChatId]      = getSharedPtr();
        ss << "Pls type whatever you want to convey <b>" << BUSINESS_NAME << "</b> & send. He will respond back.";
        STR_MSG_DEFF_RELEASE        = ss.str().c_str();
        return std::make_shared<TgBot::ReplyKeyboardRemove>();
    }

    if(m_Context.end() != (itrCntxt = m_Context.find(pMsg->chat->id))) {
        m_Context.erase(itrCntxt);
        if(lstBaseBtns.end() != (itrBtn = lstBaseBtns.find(strChatId))) lstBaseBtns.erase(itrBtn);
    }

    iRowIndex = 0;
    for(iLoop = 0, iToggle = (iNoOfItems/3) + (0 != (iNoOfItems%3)); iNoOfItems > iLoop; iLoop++) {
        createKBBtn(products[iLoop]->m_Desc, row[iRowIndex], lstBaseBtns, lstBaseBtns["Quick Menu"]);
        if((iLoop+1) % iToggle == 0) iRowIndex++;
    }

#ifdef MANI_MAMA
    createKBBtn(STR_BTN_FUND_ME, row[iRowIndex], lstBaseBtns, getSharedPtr());
#endif
    if(iToggle && (iLoop % iToggle)) iRowIndex++;

    //  Populate the next available row
    createKBBtn(STR_BTN_ABOUT_US, row[iRowIndex], lstBaseBtns, lstBaseBtns[STR_BTN_FAQ]);
#ifndef AURA
    createKBBtn(STR_BTN_TOP_UP, row[iRowIndex], lstBaseBtns);
#endif
    createKBBtn(STR_BTN_MSG_ADMIN, row[iRowIndex], lstBaseBtns, getSharedPtr());
    iRowIndex++;

    createKBBtn(STR_BTN_MAINMENU, row[iRowIndex], lstBaseBtns);
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
    std::map<unsigned int, UserContext>::const_iterator itrCntxt;
    for(itr = adminChatIds.begin(); itr != adminChatIds.end(); itr++) if(*itr == pMsg->chat->id) { isAdmin = true; break; }
    unsigned int iLoop = 0, iOrderNo;
    std::stringstream ss;

    if(!pMsg->text.compare(STR_BTN_MSG_ADMIN)) return;

#ifdef MANI_MAMA
    if(!pMsg->text.compare(STR_BTN_FUND_ME)) {
        ss << "Dear Valueable Customers,\n"
            << "    Mani Iyer's Carrier Services is looking for funds to serve you better.\n\n"
            << "        Flat <b>22% interest / annum (monthly 1.8%)</b>\n\n"
            << "You can customize your interest: monthly, quarterly or half-yearly."
            << " I will credit the interest to your a/c or I pay out in cash.\n\n"
            << "        <b>I'm ready to execute a promissory note.</b>\n\n"
            << "Pls call / msg me if you are interested.\n"
            << "-Santhosh Subramanian\nPh: 97419 83633, 93437 71700";
        STR_MSG_DEFF_RELEASE = ss.str();
        return;
    }
#endif

    User::Ptr pUser = getDBHandle()->getUserForChatId(pMsg->chat->id, fp);
    if(m_Context.end() != (itrCntxt = m_Context.find(pMsg->chat->id)) && USER_CTXT_ADMING_MSG == itrCntxt->second) {
        ss.str(""); ss << "Msg from " << pUser->m_Name << ", " << pUser->m_UserId << ", " << pUser->m_Address << "\n\n" << pMsg->text;
        for(itr = adminChatIds.begin(); itr != adminChatIds.end(); itr++) notifyMsgs[*itr] = ss.str();
        getDBHandle()->updateNotifications(notifyMsgs, fp); notifyMsgs.clear();
        STR_MSG_DEFF_RELEASE = std::string("Your msg is sent to Admin.\n") + STR_MSG_DEFF_RELEASE;
    }

    products    = getDBHandle()->getAllActiveProducts(fp);
    iSelPage    = 1;
    iNoOfItems  = products.size();
    if(0 == iNoOfItems) STR_MSG_DEFF_RELEASE = "Dear Customer,\nYou will get a msg once the Menu is Ready.";

    if(std::string::npos != pMsg->text.find(STR_BTN_EMPTY_CART)) {
        getDBHandle()->emptyCartForUser(pUser->m_OrderNo, fp);
        STR_MSG_DEFF_RELEASE = std::string("Your cart is empty now.\n") + STR_MSG_DEFF_RELEASE;
    }

    if(!pMsg->text.compare(STR_BTN_CNF_CHECKOUT)) {
        int iTotal = 0;

        std::vector<Cart::Ptr> cartItems = getDBHandle()->getCartItemsForOrderNo(pUser->m_OrderNo, fp);
        if(!cartItems.empty()) {
            iTotal = 0; for(auto &item : cartItems) iTotal += (item->m_Qnty * item->m_Price);
            getDBHandle()->insertToOrder(pUser, iTotal, CartStatus::PAYMENT_PENDING, STR_WALLET, OrderType::PORDER, fp);

            iOrderNo    = pUser->m_OrderNo;
            getDBHandle()->updateOrderNo(pUser->m_UserId, fp);

            ss << pUser->m_Name << " has made an order, " << iOrderNo;
            for(auto &id : adminChatIds)  notifyMsgs[id] = ss.str();
            getDBHandle()->updateNotifications(notifyMsgs, fp); notifyMsgs.clear();
            STR_MSG_DEFF_RELEASE = "Your order is placed. You will get a confirmation msg in a few hours.";
        }
    }
    fprintf(fp, "BaseBot %ld: ProductList onClick }\n", time(0)); fflush(fp);
}

TgBot::InputFile::Ptr ProductList::getMedia(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: ProductList getMedia {\n", time(0)); fflush(fp);
    TgBot::InputFile::Ptr pFile = nullptr;
    std::string strHdr;

    if(0 < iNoOfItems) {
        asset_file  = std::string(BOT_ROOT_PATH) + std::string(BOT_ASSETS_PATH) +
                        std::string("active_product_list_") + std::to_string(iSelPage) + std::string(".png");
#ifdef MANI_MAMA
        strHdr  = std::string("Tmrw: ") + getDBHandle()->getTmrwDate();
#endif
#ifdef AURA
        strHdr  = "AurA Soaps";
#endif
        create_product_table(asset_file, strHdr, fp);

        if(isFileExists(asset_file)) pFile = TgBot::InputFile::fromFile(asset_file, "image/png");
        else { fprintf(fp, "Fatal Error: ProductList::getMedia asset file, %s, missing\n", asset_file.c_str()); fflush(fp); }
    }
    fprintf(fp, "BaseBot %ld: ProductList getMedia }\n", time(0)); fflush(fp);
    return pFile;
}

/*
 *
 *    std::string strCode;
    if(std::string::npos != pMsg->text.find("Buy")) {
        std::stringstream ss;
        //  Buy TF-01
        strCode = pMsg->text.substr(4);  // yields TF-01
        Product::Ptr pProd  = getDBHandle()->getProductForCode(strCode,fp);

        bool isBuyable = true;
        if(std::string::npos != pMsg->text.find("Buy OR-")) {
            int iStock = 0;
            try {iStock = std::stoi(pProd->m_Desc);} catch(std::exception &e) {iStock = 0;}
            if(0 == iStock) isBuyable = false;
            else getDBHandle()->updateStock(pProd->m_ProductId, std::to_string(iStock-1), fp);
        }
        if(isBuyable) {
            //  Find page number of this code
            for(iLoop = 0; iLoop < iNoOfItems; iLoop++) if(products[iLoop]->m_ProductId == pProd->m_ProductId) break;
            if(iLoop < iNoOfItems) iSelPage = ((iLoop+1) / MAX_ITEMS_PER_PAGE) + (0 != ((iLoop+1) % MAX_ITEMS_PER_PAGE));

            int iQty = getDBHandle()->addProductToCart(pProd->m_ProductId, 1, pProd->m_Price, pMsg->chat->id, fp);
            ss << "<b>" << pProd->m_Name << " * " << iQty << "</b> added to Cart. Clicking it again, increases quantity.";
            STR_MSG_DEFF_RELEASE = ss.str();
        } else {
            notifyMsgs[adminChatIds[0]] = pUser->m_Name + std::string(", ") + std::to_string(pUser->m_UserId) + std::string(", wants ") + pProd->m_Name;
            getDBHandle()->updateNotifications(notifyMsgs, fp); notifyMsgs.clear();
            STR_MSG_DEFF_RELEASE = "Sorry out of stock. You get a msg when available.";
        }
    }

    if(!pMsg->text.compare(STR_BTN_FUND_ME)) {
        ss << "Dear valueable customers,\n"
            << "    Mani Iyer's Carrier Services is looking for funds to serve you better.\n\n"
            << "        Flat <b>22% interest / annum (monthly 1.8%)</b>\n\n"
            << "You can customize your interest: monthly, quarterly or half-yearly."
            << " I will credit the interest to your a/c or I pay out in cash.\n\n"
            << "        <b>I'm ready to execute a promissory note.</b>\n\n"
            << "Pls call / msg me if you are interested.\n"
            << "-Santhosh Subramanian\nPh: 97419 83633, 93437 71700";
        STR_MSG_DEFF_RELEASE = ss.str();
        return;
    }*/

