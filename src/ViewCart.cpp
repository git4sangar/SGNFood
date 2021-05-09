/*sgn
 * ViewCart.cpp
 *
 *  Created on: 25-May-2020
 *      Author: tstone10
 */

#include <iostream>
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>
#include <memory>

#include "pngwriter.h"

#include "BaseButton.h"
#include "Constants.h"
#include "Checkout.h"
#include "ViewCart.h"

std::string ViewCart::STR_MSG_DEFF_RELEASE  = "Your Cart";

void ViewCart::create_cart_table(std::string file_name, FILE *fp) {
    fprintf(fp, "BaseBot %ld: ViewCart::create_cart_table {\n", time(0)); fflush(fp);

    pngwriter cart_table(320, 320, 1.0, file_name.c_str());
    char toggle = 0;

    //  Headers
    std::string strOrderNo  = std::string("Order No: ") + std::to_string(cartItems[0]->m_OrderNo);
    std::string strFontFile = std::string(BOT_ROOT_PATH) + std::string(BOT_FONT_PATH) + std::string(BOT_FONT_FILE_BOLD);
    cart_table.filledsquare(0, 300, 320, 320, 0.0, 0.5, 0.5);
    cart_table.plot_text_utf8((char *)strFontFile.c_str(), 12,     5, 305, 0.0, (char *)" ", 1.0, 1.0, 1.0);
    cart_table.plot_text_utf8((char *)strFontFile.c_str(), 12,  30+5, 305, 0.0, (char *)strOrderNo.c_str(), 1.0, 1.0, 1.0);
    cart_table.plot_text_utf8((char *)strFontFile.c_str(), 12, 235+5, 305, 0.0, (char *)"Qty", 1.0, 1.0, 1.0);
    cart_table.plot_text_utf8((char *)strFontFile.c_str(), 12, 275+3, 305, 0.0, (char *)"Price", 1.0, 1.0, 1.0);

    //  Highlight alternate rows
    int iLoop = 0, iCount = 1, iQty = 0, iTotal = 0, iIndex = 0, iSum = 0;
    iIndex  = (iSelPage - 1) * (MAX_ITEMS_PER_PAGE-1);
    iCount  = ((iSelPage - 1) * (MAX_ITEMS_PER_PAGE-1)) + 1;
    for(iLoop = 300; iLoop > 30; iLoop -= 30, toggle = 1 - toggle) {
        if(0 == toggle) cart_table.filledsquare(0, iLoop, 320, iLoop-30, 0.9, 0.9, 0.9);
        if((iNoOfItems > iIndex) && (iIndex < (iSelPage * (MAX_ITEMS_PER_PAGE-1)))) {
            std::string strCount    = std::to_string(iCount);
            std::string strProdPack = products[iIndex]->m_Name
#ifdef AURA
                    + std::string(" (") + products[iIndex]->m_Pack + std::string(")");
#endif
                    ;
            std::string strQty      = std::to_string(cartItems[iIndex]->m_Qnty);
            iSum                    = cartItems[iIndex]->m_Qnty * cartItems[iIndex]->m_Price;
            std::string strPrice    = std::to_string(iSum);

            cart_table.plot_text_utf8((char *)strFontFile.c_str(), 10,     5, iLoop-20, 0.0, (char *)strCount.c_str(), 0, 0, 0);
            cart_table.plot_text_utf8((char *)strFontFile.c_str(), 10,  30+5, iLoop-20, 0.0, (char *)strProdPack.c_str(), 0, 0, 0);
            cart_table.plot_text_utf8((char *)strFontFile.c_str(), 10, 235+5, iLoop-20, 0.0, (char *)strQty.c_str(), 0, 0, 0);
            cart_table.plot_text_utf8((char *)strFontFile.c_str(), 10, 275+5, iLoop-20, 0.0, (char *)strPrice.c_str(), 0, 0, 0);
            iQty    += cartItems[iIndex]->m_Qnty;
            iCount++; iTotal += iSum; iIndex++;
        }
    }
    //  Vertical lines
    cart_table.line( 30, 320,  30, 30, 0, 0, 0);
    cart_table.line(235, 320, 235, 30, 0, 0, 0);
    cart_table.line(275, 320, 275, 30, 0, 0, 0);

    if(iNoOfItems <= (iSelPage * (MAX_ITEMS_PER_PAGE-1))) {
        for(iSum = iQty = iLoop = 0; iLoop < cartItems.size(); iLoop ++) {
            iQty    += cartItems[iLoop]->m_Qnty;
            iSum    += (cartItems[iLoop]->m_Qnty * cartItems[iLoop]->m_Price);
        }
        cart_table.plot_text_utf8((char *)strFontFile.c_str(), 12,   70, 10, 0.0, (char *)"Total ₹:", 0, 0, 0);
        cart_table.plot_text_utf8((char *)strFontFile.c_str(), 10, 140+5, 10, 0.0, (char *)std::to_string(iSum).c_str(), 0, 0, 0);
        cart_table.plot_text_utf8((char *)strFontFile.c_str(), 10, 225+5, 10, 0.0, (char *)std::to_string(iQty).c_str(), 0, 0, 0);
    } else {
        int iMaxPgs = (iNoOfItems / (MAX_ITEMS_PER_PAGE-1)) + (0 != (iNoOfItems % (MAX_ITEMS_PER_PAGE-1)));
        std::stringstream ss; ss << "Page - " << iSelPage << " of " << iMaxPgs;
        cart_table.plot_text_utf8((char *)strFontFile.c_str(), 10,   70, 10, 0.0, (char *)ss.str().c_str(), 0, 0, 0);
    }

    cart_table.close();
    fprintf(fp, "BaseBot %ld: ViewCart::create_cart_table }\n", time(0)); fflush(fp);
}

TgBot::GenericReply::Ptr ViewCart::prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& lstBaseBtns, TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: ViewCart::prepareMenu {\n", time(0)); fflush(fp);

    unsigned int iPrev, iNext, iLoop, iRowIndex = 0, iItemsPerRow = 0, iItems, iRowsReqd;
    std::vector<TgBot::KeyboardButton::Ptr> row[MAX_BUTTON_ROWS];
    std::vector<std::string> strBtns;
    std::string strBtn;
    TgBot::ReplyKeyboardMarkup::Ptr pMainMenu;

    iRowIndex = 0;
    if(0 < cartItems.size()) {
        for(iLoop = ((iSelPage - 1) * (MAX_ITEMS_PER_PAGE-1));
                (iNoOfItems > iLoop) && (iLoop < (iSelPage * (MAX_ITEMS_PER_PAGE-1)));
                iLoop++) {
            strBtn  = std::string(STR_BTN_ADD) + std::string(" ") + std::to_string(iLoop+1);
            strBtns.push_back(strBtn);
        }

        for(iLoop = ((iSelPage - 1) * (MAX_ITEMS_PER_PAGE-1));
                (iNoOfItems > iLoop) && (iLoop < (iSelPage * (MAX_ITEMS_PER_PAGE-1)));
                iLoop++) {
            strBtn  = std::string(STR_BTN_REMOVE) + std::string(" ") + std::to_string(iLoop+1);
            strBtns.push_back(strBtn);
        }
        iItems          = strBtns.size();
        iRowsReqd       = (iItems / MAX_ADD_RMV_BTNS_PER_ROW) + ((0 != (iItems % MAX_ADD_RMV_BTNS_PER_ROW) ? 1 : 0));
        iItemsPerRow    = (iItems / iRowsReqd) + (0 != (iItems % iRowsReqd) ? 1 : 0);
        for(iLoop = 0; iLoop < iItems; iLoop++)
            createKBBtn(strBtns[iLoop], row[iLoop/iItemsPerRow], lstBaseBtns, getSharedPtr());

        iRowIndex   = iRowsReqd;
    } else STR_MSG_DEFF_RELEASE  = "Your Cart is empty. Clicking \"Buy\" button adds an item to Cart.";

    //  Populate pages in next available row if no. of items more than (MAX_ITEMS_PER_PAGE-1)
    if((MAX_ITEMS_PER_PAGE-1) < iNoOfItems) {
        iPrev   = (iSelPage == 1) ? 1 : (iSelPage - 1);
        createKBBtn(std::string(CART_PAGE) + std::string(" ") + std::to_string(iPrev), row[iRowIndex], lstBaseBtns, getSharedPtr());

        iNext   = (iNoOfItems > (iSelPage * (MAX_ITEMS_PER_PAGE-1))) ? (iSelPage + 1) : iSelPage;
        createKBBtn(std::string(CART_PAGE) + std::string(" ") + std::to_string(iNext), row[iRowIndex], lstBaseBtns, getSharedPtr());
        iRowIndex++;
    }

    //  Populate the next available row
    createKBBtn(STR_BTN_MAINMENU, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_EMPTY_CART, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_CHECKOUT, row[iRowIndex], lstBaseBtns);
    iRowIndex++;

    //  Add all the rows to main menu
    pMainMenu   = std::make_shared<TgBot::ReplyKeyboardMarkup>();
    for(iLoop = 0; iLoop < iRowIndex; iLoop++) {
       pMainMenu->keyboard.push_back(row[iLoop]);
    }

    //  The predefined address is contained in button text
    std::map<unsigned int, UserContext>::const_iterator itrCntxt;
    if(m_Context.end() != (itrCntxt = m_Context.find(pMsg->chat->id)) &&
        m_Context[pMsg->chat->id] == USER_CTXT_CLEANUP) {
        m_Context.erase(itrCntxt);

        std::map<std::string, std::shared_ptr<BaseButton>>::const_iterator itrBtn;
        if(lstBaseBtns.end() != (itrBtn = lstBaseBtns.find(pMsg->text)))
            lstBaseBtns.erase(itrBtn);
    }

    fprintf(fp, "BaseBot %ld: Finishing ViewCart::prepareMenu }\n", time(0)); fflush(fp);
    pMainMenu->resizeKeyboard   = true;
    return pMainMenu;
}

unsigned int ViewCart::findAssociatedNumber(std::string strMsg, std::string strFind, int iMaxItems) {
    unsigned int iFound = 0, iLoop = 0;

    iFound = 0;
    if(std::string::npos != strMsg.find(strFind))
        for(iLoop = 1; iLoop <= iMaxItems; iLoop++) {
            std::stringstream ssPage;
            ssPage << strFind << " " << iLoop;

            if(!strMsg.compare(ssPage.str())) {
                iFound  = iLoop;
                break;
            }
        }
    //  Rate of user's button-clicks may be fast. So this check
    if(cartItems.size() < iFound) iFound = 0;
    return iFound;
}

void ViewCart::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: ViewCart onClick pMessage %s {\n", time(0), pMsg->text.c_str()); fflush(fp);

    unsigned int iFound = 0, iLoop = 0, iMaxPages = 0, iCrtSz = 0;
    std::vector<Cart::Ptr>::iterator itrCart;
    iSelPage = 1; iNoOfItems = 0;

    User::Ptr pUser = getDBHandle()->getUserForChatId(pMsg->chat->id, fp);
    if(pUser) {
        strAddress  = pUser->m_Address;
        cartItems   = getDBHandle()->getCartItemsForOrderNo(pUser->m_OrderNo, fp);
        if(!cartItems.size()) return;

        //  If the incoming msg is not "View Cart" check for Pages or Remove
        if(pMsg->text.compare(STR_BTN_VIEW_CART)) {
            //  Processing Cart Page
            if(0 < (iFound = findAssociatedNumber(pMsg->text, CART_PAGE))) {
                iSelPage  = iFound;
            }
            else if(0 < (iFound = findAssociatedNumber(pMsg->text, STR_BTN_ADD, (MAX_ITEMS_PER_PAGE-1) * MAX_NO_OF_PAGES))) {
                iSelPage  = 1 + ((iFound-1) / (MAX_ITEMS_PER_PAGE-1));
                getDBHandle()->incrementItemQty(cartItems[iFound-1]->m_ProductId, pUser->m_OrderNo, fp);
                cartItems = getDBHandle()->getCartItemsForOrderNo(pUser->m_OrderNo, fp);
            }
            else if(0 < (iFound = findAssociatedNumber(pMsg->text, STR_BTN_REMOVE, (MAX_ITEMS_PER_PAGE-1) * MAX_NO_OF_PAGES))) {
                getDBHandle()->reduceCartQty(cartItems[iFound-1]->m_ProductId, pUser->m_OrderNo, fp);
                cartItems   = getDBHandle()->getCartItemsForOrderNo(pUser->m_OrderNo, fp);
                iCrtSz      = cartItems.size();
                iMaxPages   = (iCrtSz / (MAX_ITEMS_PER_PAGE-1)) + (0 != (iCrtSz % (MAX_ITEMS_PER_PAGE-1)));
                iSelPage    = 1 + ((iFound-1) / (MAX_ITEMS_PER_PAGE-1));
                iSelPage    = (iSelPage > iMaxPages) ? iMaxPages : iSelPage;
            }
            if(0 == iFound) { fprintf(fp, "BaseBot %ld: Fatal error, iFound == 0, ViewCart onClick\n", time(0)); fflush(fp); }
        }

		//	Add delivery charges if not added already
        Product::Ptr pProduct;
		if(!cartItems.empty() && !getDBHandle()->isProdCarted(PROD_DLVRY_CHARGE, pUser->m_OrderNo, fp)) {
            pProduct    = getDBHandle()->getProductById(PROD_DLVRY_CHARGE, fp);
			getDBHandle()->addProductToCart(PROD_DLVRY_CHARGE, 1, pProduct->m_Price, pMsg->chat->id, fp);
			cartItems   = getDBHandle()->getCartItemsForOrderNo(pUser->m_OrderNo, fp);
		}

        //  Now Cart would have been updated after processing Remove
        for(itrCart = cartItems.begin(); itrCart != cartItems.end(); itrCart++) {
            pProduct    = getDBHandle()->getProductById((*itrCart)->m_ProductId, fp);
            products.push_back(pProduct);
            iNoOfItems++;
        }
    }
    fprintf(fp, "BaseBot %ld: ViewCart onClick pMessage }\n", time(0)); fflush(fp);
}

TgBot::InputFile::Ptr ViewCart::getMedia(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: ViewCart::getMedia {\n", time(0)); fflush(fp);
    TgBot::InputFile::Ptr pFile = nullptr;

    if(0 < cartItems.size()) {
        asset_file  = std::string(BOT_ROOT_PATH) + std::string(BOT_ASSETS_PATH) + std::string("cart_page_") +
                        std::to_string(iSelPage) + std::string(".png");
        create_cart_table(asset_file, fp);
    }

    if(0 < iNoOfItems) pFile = TgBot::InputFile::fromFile(asset_file, "image/png");
    fprintf(fp, "BaseBot %ld: ViewCart::getMedia }\n", time(0)); fflush(fp);
    return pFile;
}
