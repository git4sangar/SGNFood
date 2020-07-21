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
std::string Checkout::STR_BTN_GPAY_TOP_UP   = "Topped Up GPay";
std::string Checkout::STR_BTN_PAYTM_TOP_UP  = "Topped Up PayTM";
std::string Checkout::STR_BTN_BHIM_TOP_UP   = "Topped Up BHIM";
std::string Checkout::STR_BTN_CASH_TOP_UP   = "Topped Up Cash";

/*std::string Checkout::getPaymentString(unsigned int iOrderNo, std::string strName, std::string strAddress, unsigned int iTotal, FILE *fp) {
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
}*/

std::string Checkout::getPaymentString(unsigned int iOrderNo, std::string strName, std::string strAddress, int iTotal, FILE *fp) {
    std::stringstream ss;
    int iNewBal = pUser->m_WBalance - iTotal;

    ss << "Hi " << strName << ", Click \"Confirm Checkout\" to place order. Your Total bill is <b>₹ " << iTotal << "</b>\n";
    ss << "\nYour Shipping address is :\n<b>" << strAddress << ".</b>";
    ss << "\nYour Wallet has ₹ " << pUser->m_WBalance << ".\n";
    ss << "\nWallet Balance : " << pUser->m_WBalance << " - " << iTotal << " = " << static_cast<int>(pUser->m_WBalance - iTotal);

    if(0 > iNewBal) {
        ss << "\n<b>-VE BALANCE: " << static_cast<int>(pUser->m_WBalance - iTotal) << ".</b> Pls Top Up Wallet."
                << "\n<b>Note: Your order may still be delivered.</b>";
    }
    return ss.str();
}

std::string Checkout::getTopUpString() {
    std::stringstream ss;
    ss << "\n\n1) Transfer Top Up amount via GPay / PayTM apps or BHIM." <<
            "\n\nGPay : " << GPAY_MOBILE << ",\nPayTm : " << PAYTM_MOBILE << ",\nUPI id : " << BHIM_UPI <<
            "\n\n2) Mention <b>\"Transac " << pUser->m_TransacNo << " Top Up ₹\"</b> in desc." <<
            "\nGPay -> \"What is this for\"\nPayTM -> \"Add a Message\""<<
            "\n\nExample:\nTransac 1002 Top Up 1000\nTransac 1050 Top Up 5000\netc...\n" <<
            "\nAfter transferring, click a \"Topped Up\" button below." <<
            " Your Wallet will be updated after verification.";
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

    std::string strBtn;
    int iRowIndex   = 0, iLoop = 0;
    pMainMenu   = std::make_shared<TgBot::ReplyKeyboardMarkup>();

    pUser       = getDBHandle()->getUserForChatId(pMsg->chat->id, fp);
    iRowIndex   = 0;
    if(std::string::npos    != pMsg->text.find(STR_BTN_TOP_UP)) {
        std::string strAmt  = pMsg->text.substr(std::string(STR_BTN_TOP_UP).length());

        //  Is this req coming from OrderMgmt page? then strAmt will be empty
        if(strAmt.empty()) {
            STR_MSG_DEFF_RELEASE  = "Pls choose an amount to Top Up.";

            if(0 > pUser->m_WBalance) strBtn = std::string(STR_BTN_TOP_UP) + std::string(" ") + std::to_string(std::abs(pUser->m_WBalance));
            else strBtn = std::string(STR_BTN_TOP_UP) + " 1000";
            createKBBtn(strBtn, row[iRowIndex], lstBaseBtns, getSharedPtr());

            strBtn = std::string(STR_BTN_TOP_UP) + " 2000"; createKBBtn(strBtn, row[iRowIndex], lstBaseBtns, getSharedPtr());
            strBtn = std::string(STR_BTN_TOP_UP) + " 5000"; createKBBtn(strBtn, row[iRowIndex], lstBaseBtns, getSharedPtr());
        } else {
            STR_MSG_DEFF_RELEASE  = getTopUpString();
            strBtn = STR_BTN_GPAY_TOP_UP + strAmt;  createKBBtn(strBtn, row[iRowIndex], lstBaseBtns, getSharedPtr());
            strBtn = STR_BTN_PAYTM_TOP_UP + strAmt; createKBBtn(strBtn, row[iRowIndex], lstBaseBtns, getSharedPtr());
            strBtn = STR_BTN_BHIM_TOP_UP + strAmt;  createKBBtn(strBtn, row[iRowIndex], lstBaseBtns, getSharedPtr());
            strBtn = STR_BTN_CASH_TOP_UP + strAmt;  createKBBtn(strBtn, row[iRowIndex], lstBaseBtns, getSharedPtr());
        }
        iRowIndex++;
    }

    else if(std::string::npos != pMsg->text.find("Topped Up")) {
        STR_MSG_DEFF_RELEASE  = "Top Up request made. Once merchant confirms your payment, your Wallet will be updated. \nYou proceed to checkout.";
        createKBBtn(STR_BTN_CHECKOUT, row[iRowIndex], lstBaseBtns);
        createKBBtn(STR_BTN_YOUR_ORDERS, row[iRowIndex], lstBaseBtns);
        iRowIndex++;
    }

    else if(pUser->m_Address.empty() || !isMobileNoPresent(pUser->m_Address)) {
        m_Context[pMsg->chat->id] = USER_CTXT_ADDRESS;
        std::string strChatId   = std::to_string(pMsg->chat->id);
        lstBaseBtns[strChatId]  = lstBaseBtns[STR_BTN_SHPG_ADDRESS];
        STR_MSG_DEFF_RELEASE    = getAddressNotification();
        return std::make_shared<TgBot::ReplyKeyboardRemove>();
    }

    else if(0 == iNoOfItems) {
        STR_MSG_DEFF_RELEASE  = "Your Cart is empty. Pls add some products to cart by clicking <b>Main Menu</b> below.";
    }

    else if(!pMsg->text.compare(STR_BTN_CHECKOUT)) {
        STR_MSG_DEFF_RELEASE  = getPaymentString(pUser->m_OrderNo, pMsg->from->firstName, pUser->m_Address, iTotal, fp);

        if(0 > pUser->m_WBalance) std::string(STR_BTN_TOP_UP) + std::string(" ") + std::to_string(std::abs(pUser->m_WBalance));
        else strBtn = std::string(STR_BTN_TOP_UP) + " 1000";
        createKBBtn(strBtn, row[iRowIndex], lstBaseBtns, getSharedPtr());

        strBtn = std::string(STR_BTN_TOP_UP) + " 2000"; createKBBtn(strBtn, row[iRowIndex], lstBaseBtns, getSharedPtr());
        strBtn = std::string(STR_BTN_TOP_UP) + " 5000"; createKBBtn(strBtn, row[iRowIndex], lstBaseBtns, getSharedPtr());
        iRowIndex++;
        createKBBtn(STR_BTN_CNF_CHECKOUT, row[iRowIndex], lstBaseBtns, lstBaseBtns[STR_BTN_MAINMENU]);
        iRowIndex++;
    }

    //  To avoid exception
    if(STR_MSG_DEFF_RELEASE.empty()) STR_MSG_DEFF_RELEASE  = "Checkout";

    createKBBtn(STR_BTN_VIEW_CART, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_MAINMENU, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_SHPG_ADDRESS, row[iRowIndex], lstBaseBtns);
    iRowIndex++;

    //  Now add the relvant keyboards & send
    for(iLoop = 0; iLoop < iRowIndex; iLoop++) {
       pMainMenu->keyboard.push_back(row[iLoop]);
    }

    fprintf(fp, "BaseBot %ld: Finishing Checkout::prepareMenu\n", time(0)); fflush(fp);
    pMainMenu->resizeKeyboard   = true;
    return pMainMenu;
}

void Checkout::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: Checkout onClick pMessage %s {\n", time(0), pMsg->text.c_str()); fflush(fp);

    unsigned int iLoop = 0, iAmt = 0; std::string strPGw;
    std::stringstream ss; unsigned int iOrderNo = 0;
    std::map<unsigned int, UserContext>::const_iterator itr;

    pUser = getDBHandle()->getUserForChatId(pMsg->chat->id, fp);
    if(!pUser) return;

    //  pMsg->text is "Topped Up GPay 1000" or "Topped Up PayTM 5000 or "Topped Up BHIM 2000
    if(std::string::npos != pMsg->text.find("Topped Up")) {
        std::string strAmt;
        if(std::string::npos != pMsg->text.find("GPay"))    {strPGw  = "GPay"; strAmt = pMsg->text.substr(STR_BTN_GPAY_TOP_UP.length()+1);}
        if(std::string::npos != pMsg->text.find("PayTM"))   {strPGw  = "PayTM";strAmt = pMsg->text.substr(STR_BTN_PAYTM_TOP_UP.length()+1);}
        if(std::string::npos != pMsg->text.find("BHIM"))    {strPGw  = "BHIM"; strAmt = pMsg->text.substr(STR_BTN_BHIM_TOP_UP.length()+1);}
        if(std::string::npos != pMsg->text.find("Cash"))    {strPGw  = "CASH"; strAmt = pMsg->text.substr(STR_BTN_CASH_TOP_UP.length()+1);}

        //  Update database
        try{iAmt = std::stoi(strAmt);} catch(std::exception &e) {iAmt = 0;}
	if(0 < iAmt) {
        	getDBHandle()->insertToOrder(pUser, iAmt, CartStatus::PAYMENT_PENDING, strPGw, OrderType::TOPUP, fp);
	        getDBHandle()->updateTransacNo(pUser->m_UserId, fp);

        	//  Notify admin
	        ss << pUser->m_Name << " topped up Wallet using " << strPGw << ". Pls verify payment. His transac no is " << pUser->m_TransacNo;
        	for(auto &id : adminChatIds)  notifyMsgs[id] = ss.str();
	} else {
        	STR_MSG_DEFF_RELEASE  = strAmt + " is not a valid amount.";
	}
    }

    cartItems = getDBHandle()->getCartItemsForOrderNo(pUser->m_OrderNo, fp);
    iNoOfItems= cartItems.size();
    for(iLoop = 0; iLoop < iNoOfItems; iLoop++) {
        iTotal += static_cast<int>(cartItems[iLoop]->m_Qnty * cartItems[iLoop]->m_Price);
    }
    fprintf(fp, "BaseBot %ld: Checkout onClick }\n", time(0)); fflush(fp);
}
