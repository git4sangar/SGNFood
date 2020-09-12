/*sgn
 * QuickMenu.cpp
 *
 *  Created on: 27-Jul-2020
 *      Author: tstone10
 */




#include <iostream>
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>
#include <ProductList.h>
#include <memory>

#include "BaseButton.h"
#include "Constants.h"
#include "QuickMenu.h"

std::string QuickMenu::STR_MSG_DEFF_RELEASE   = " ";

TgBot::GenericReply::Ptr QuickMenu::prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& lstBaseBtns, TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: QuickMenu::prepareMenu {\n", time(0)); fflush(fp);

    std::string strText;
    TgBot::InlineKeyboardButton::Ptr inlineBtn;
    std::vector<TgBot::InlineKeyboardButton::Ptr> row[MAX_BUTTON_ROWS];
    unsigned int iLoop = 0, iRowIndex = 0;
    TgBot::InlineKeyboardMarkup::Ptr pMainMenu;
    std::map<std::string, int>::iterator itrQty = qty.begin(), itrEnd = qty.end();

    if(STR_MSG_DEFF_RELEASE.empty()) STR_MSG_DEFF_RELEASE   = "Choose a quantity.";

    if(std::string::npos != pProd->m_Pack.find("kg"))   {itrQty = qty.begin(); itrEnd = qty.end();}
    if(std::string::npos != pProd->m_Pack.find("pk"))   {itrQty = pcs.begin(); itrEnd = pcs.end();}

    for(iRowIndex = 0, iLoop = 1; itrEnd != itrQty; itrQty++, iLoop++) {
        inlineBtn               = std::make_shared<TgBot::InlineKeyboardButton>();
        inlineBtn->callbackData = itrQty->first;
        inlineBtn->text         = itrQty->first;
        row[iRowIndex].push_back(inlineBtn);
        if(iLoop >= QTY_PER_ROW) {iRowIndex++; iLoop = 0;}
    }

    //  Add all the rows to main menu
    pMainMenu   = std::make_shared<TgBot::InlineKeyboardMarkup>();
    for(iLoop = 0; iLoop < iRowIndex; iLoop++) {
        pMainMenu->inlineKeyboard.push_back(row[iLoop]);
    }

    fprintf(fp, "BaseBot %ld: Finishing QuickMenu::prepareMenu }\n", time(0)); fflush(fp);
    return pMainMenu;
}

void QuickMenu::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: QuickMenu onClick pMsg %s {\n", time(0), pMsg->text.c_str()); fflush(fp);

    unsigned int iProdId = 0, iQty = 0;
    std::map<std::string, int>::iterator itrQty;
    std::stringstream ss;

    if(qty.end() != (itrQty = qty.find(pMsg->text)) || pcs.end() != (itrQty = pcs.find(pMsg->text))) {
        fprintf(fp, "Picked up qty %d\n", itrQty->second); fflush(fp);
        //  Get product_id from User, Add it to cart, Clear product_id
        iProdId = getDBHandle()->getChosenProduct(pMsg->chat->id, fp);
        pProd   = getDBHandle()->getProductById(iProdId, fp);
        iQty    = getDBHandle()->addProductToCart(iProdId, itrQty->second, pProd->m_Price, pMsg->chat->id, fp);

        ss << iQty << " * " << pProd->m_Name
#ifdef AURA
                << "(" << pProd->m_Pack << ")"
#endif
                << " added to Cart.\nClick below to increase qty or Click <b>View Cart</b> when done.";
        STR_MSG_DEFF_RELEASE= ss.str();
    }

    //  Product name or Product description
    std::vector<Product::Ptr> products  = getDBHandle()->getAllActiveProducts(fp);
    for(auto &prod : products) {

        // Check if incoming string is available in descToCode
        // If so check, corresponding desc is in actv-product & remember the result
        bool isFound = (descToCode.end() != descToCode.find(pMsg->text)) ?
                (std::string::npos != prod->m_Code.find(descToCode[pMsg->text])) : false;

        //  First check if the desc matches the product name
        if(!prod->m_Name.compare(pMsg->text) || isFound ) {
            getDBHandle()->retainProdId(pMsg->chat->id, prod->m_ProductId, fp);
            pProd   = prod;
            break;
        }
    }

    fprintf(fp, "BaseBot %ld: QuickMenu onClick }\n", time(0)); fflush(fp);
}
