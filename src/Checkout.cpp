/*sgn
 * Checkout.cpp
 *
 *  Created on: 30-May-2020
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

std::string Checkout::STR_MSG_DEFF_RELEASE  = "";
std::string Checkout::STR_BTN_GPAY  = "GPay to\n" + std::string(GPAY_MOBILE);
std::string Checkout::STR_BTN_PAYTM = "PayTM to\n" + std::string(PAYTM_MOBILE);
std::string Checkout::STR_BTN_BHIM  = "BHIM to\n" + std::string(BHIM_UPI);
std::string Checkout::STR_BTN_CASH  = "Cash\non delivery";
std::string Checkout::STR_BTN_BACK  = "<-Address";

std::string Checkout::getPaymentString(unsigned int iOrderNo, std::string strName, std::string strAddress, unsigned int iTotal, FILE *fp) {
    std::stringstream ss;
    ss << "Hi " << strName << ", You are just a step away from making an order. Your Total bill is <b>₹ " << iTotal << "</b>\n";
    ss << "\nYour shipping address is: \n<b>" << strAddress;
    ss << "</b>\n\n1) Use one of the below payment methods to <b>CONFIRM</b> your Order." <<
                "\n2) Open GPay or PayTM app & pay ₹ " << iTotal <<
                "\n3) Mention the <b>Order no: " << iOrderNo << "</b> while paying." <<
                "\nGPay -> \"What is this for\", in PayTM -> \"Add a Message\" on right hand side."<<
                "\n\nYou will receive a confirmation message in a few hours, after the merchant verifies this order." <<
//                "\nProvide the OTP during delivery." <<
//                "\n\n<b>*</b> This bot is <b>not</b> integrated with payment banks like Paytm, GPay, etc...\n" <<
                "\n<b>Note: Merchant has the right to remove any of the items you ordered for valid reasons.</b>";
    return ss.str();
}

bool Checkout::isMobileNoPresent(std::string strAddress) {
    int iDigits = 0;

    for(char &c : strAddress) {
        if(std::isspace(c)) continue;
        if(std::isdigit(c)) iDigits++;
        else iDigits = 0;

        if(MAX_MOBILE_DIGITS <= iDigits) {
            break;
        }
    }
    return MAX_MOBILE_DIGITS <= iDigits;
}

TgBot::GenericReply::Ptr Checkout::prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& lstBaseBtns, TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: Checkout prepareMenu pMessage %s\n", time(0), pMsg->text.c_str()); fflush(fp);

    TgBot::ReplyKeyboardMarkup::Ptr pMainMenu;
    std::vector<TgBot::KeyboardButton::Ptr> row[MAX_BUTTON_ROWS];

    int iRowIndex   = 0, iLoop = 0;
    pMainMenu   = std::make_shared<TgBot::ReplyKeyboardMarkup>();

    iRowIndex   = 0;
    //  Handle empty Cart. Control shall not reach here. Checkout button should be visible only when cart has some items
    if(0 == iNoOfItems) {
        STR_MSG_DEFF_RELEASE  = "Your Cart is empty. Pls add some products to cart, before checking out.";
        createKBBtn(STR_BTN_MAINMENU, row[iRowIndex], lstBaseBtns);
        iRowIndex++;
    }

    else if(pUser->m_Address.empty() || !isMobileNoPresent(pUser->m_Address)) {
        STR_MSG_DEFF_RELEASE  = "Pls update your shipping address <b>WITH MOBILE NO</b> before checking out.";
        createKBBtn(STR_BTN_SHPG_ADDRESS, row[iRowIndex], lstBaseBtns);
        iRowIndex++;
        createKBBtn(STR_BTN_MAINMENU, row[iRowIndex], lstBaseBtns);
        createKBBtn(STR_BTN_VIEW_CART, row[iRowIndex], lstBaseBtns);
        iRowIndex++;
    }

    else if(!pMsg->text.compare(STR_BTN_CHECKOUT)) {
        STR_MSG_DEFF_RELEASE  = getPaymentString(pUser->m_OrderNo, pMsg->from->firstName, pUser->m_Address, iTotal, fp);
        createKBBtn(STR_BTN_GPAY, row[iRowIndex], lstBaseBtns, lstBaseBtns[STR_BTN_MAINMENU]);
        createKBBtn(STR_BTN_PAYTM, row[iRowIndex], lstBaseBtns, lstBaseBtns[STR_BTN_MAINMENU]);
        createKBBtn(STR_BTN_BHIM, row[iRowIndex], lstBaseBtns, lstBaseBtns[STR_BTN_MAINMENU]);
        createKBBtn(STR_BTN_CASH, row[iRowIndex], lstBaseBtns, lstBaseBtns[STR_BTN_MAINMENU]);
        iRowIndex++;

        createKBBtn(STR_BTN_VIEW_CART, row[iRowIndex], lstBaseBtns);
        createKBBtn(STR_BTN_MAINMENU, row[iRowIndex], lstBaseBtns);
        createKBBtn(STR_BTN_SHPG_ADDRESS, row[iRowIndex], lstBaseBtns);
        iRowIndex++;
    }

    //  Now add the relvant keyboards & send
    for(iLoop = 0; iLoop < iRowIndex; iLoop++) {
       pMainMenu->keyboard.push_back(row[iLoop]);
    }

    fprintf(fp, "BaseBot %ld: Finishing Checkout::prepareMenu\n", time(0)); fflush(fp);
    return pMainMenu;
}

void Checkout::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: Checkout onClick pMessage %s {\n", time(0), pMsg->text.c_str()); fflush(fp);

    unsigned int iLoop;
    std::map<unsigned int, UserContext>::const_iterator itr;

    pUser = getDBHandle()->getUserForChatId(pMsg->chat->id, fp);
    if(!pUser) return;

    cartItems = getDBHandle()->getCartItemsForOrderNo(pUser->m_OrderNo, fp);
    iNoOfItems= cartItems.size();
    for(iLoop = 0; iLoop < iNoOfItems; iLoop++) {
        iTotal += (cartItems[iLoop]->m_Qnty * cartItems[iLoop]->m_Price);
    }
    fprintf(fp, "BaseBot %ld: Checkout onClick }\n", time(0)); fflush(fp);
}





















