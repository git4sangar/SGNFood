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
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "pngwriter.h"

#include "BaseButton.h"
#include "Constants.h"
#include "Checkout.h"
#include "HttpClient.h"

std::string Checkout::STR_MSG_DEFF_RELEASE  = "";
std::string Checkout::STR_BTN_GPAY_TOP_UP   = "Topped Up GPay";
std::string Checkout::STR_BTN_PAYTM_TOP_UP  = "Topped Up PayTM";
std::string Checkout::STR_BTN_PHONEPE_TOP_UP= "Topped Up PhonePe";

std::string Checkout::getMobileNo(std::string strAddress) {
    int iDigits = 0;
    bool isDone = false;
    std::string strMobile;

    for(char &c : strAddress) {
        if(std::isspace(c)) continue;

        if(std::isdigit(c)) { iDigits++; strMobile += c; }
        else if(isDone) break;
        else { iDigits = 0; strMobile.clear(); }

        if(MAX_MOBILE_DIGITS <= iDigits) {
            isDone = true;
        }
    }
    return strMobile.substr(strMobile.length() - MAX_MOBILE_DIGITS);
}


#ifdef AURA
std::string Checkout::getPaymentString(unsigned int iWho, unsigned int iOrderNo, std::string strName, std::string strAddress, int iTotal, FILE *fp) {
    getPaymentLink(iWho, iOrderNo, iTotal, strName, strAddress, fp);
    std::stringstream ss;

    ss <<  "Hi " << strName << ",\n\n<b>Pls Wait a few secs..,</b>\n\nYou get a payment link. Pay by clicking it.";
    ss << "\n\nYour Total bill is <b>₹ " << iTotal << "</b>";
    ss << "\n\nYour Shipping address is :\n" << strAddress;
    ss << "\n\nNote: Payment link can also be seen at:\n" << STR_BTN_MAINMENU << " -> " << STR_BTN_YOUR_ORDERS << " -> " << iOrderNo;
    return ss.str();
}
#else
std::string Checkout::getPaymentString(unsigned int iWho, unsigned int iOrderNo, std::string strName, std::string strAddress, int iTotal, FILE *fp) {
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
#endif

void Checkout::getPaymentLink(unsigned int iWho, unsigned int iOrderNo, int iAmt, std::string strUserName, std::string strAddress,FILE *fp) {
    std::stringstream ssTmp, ssUrl;

    std::map<std::string, std::string> formData;
    formData["appId"]           = CASH_FREE_APP_ID;
    formData["secretKey"]       = CASH_FREE_SECRET_KEY;
    ssTmp << VENDOR_ID << "_" << iOrderNo;
    formData["orderId"]         = ssTmp.str();
    formData["orderAmount"]     = std::to_string(iAmt);
    formData["customerName"]    = strUserName;
    formData["customerPhone"]   = getMobileNo(strAddress);
    formData["customerEmail"]   = BOT_MAIL;
    formData["returnUrl"]       = REDIRECT_URL;
    formData["notifyUrl"]       = std::string("");

    ssUrl << CASH_FREE_BASE_URL << CASH_FREE_ORDER_API;
    pHttpClient->postReqFormData(ssUrl.str(), formData, iWho, iOrderNo);
}

//  Called from Http thread context. Accessing member variables is prohibited.
void Checkout::onDownloadSuccess(unsigned int iChatId, unsigned int iOrderNo, std::string strResp, FILE *fp) {
    boost::property_tree::ptree root;
    std::string strSuccess = "OK";  //  Case sensitive

    std::map<unsigned int, std::string> msgToUsers;
    std::stringstream ss, ssMsg1, ssMsg2; ss << strResp;
	User::Ptr   pUser   = getDBHandle()->getUserForChatId(iChatId, fp);

    boost::property_tree::read_json(ss, root);
    if(!strSuccess.compare(root.get<std::string>("status"))) {
        std::string strUrl  = root.get<std::string>("paymentLink");
		ssMsg1 << "Click the following link to make the payment for Top Up.\n" << strUrl;
        msgToUsers[iChatId] = ssMsg1.str();

        ssMsg2 << pUser->m_Name << " is attempting Top Up via CashFree. Pls verify payment after a few mins. His transac no is " << iOrderNo;
        for(auto &id : adminChatIds)  msgToUsers[id] = (iChatId == id) ? ssMsg1.str() : ssMsg2.str();
    } else {
        msgToUsers[iChatId] = "Something went wrong. Pls try after sometime.";
    }
    getDBHandle()->updateNotifications(msgToUsers, fp);
}

//  Called from Http thread context. Accessing member variables is prohibited.
void Checkout::onDownloadFailure(unsigned int iChatId, unsigned int iOrderNo, FILE *fp) {
    std::stringstream ssMsg1;
    std::map<unsigned int, std::string> msgToUsers;
    ssMsg1 << "Something went wrong. Pls try checking out again.\n";
    msgToUsers[iChatId] = ssMsg1.str() ;
    getDBHandle()->updateNotifications(msgToUsers, fp);
}

std::string Checkout::getTopUpString() {
    std::stringstream ss;
    ss << "\n\n1) Transfer via GPay, PayTM, PhonePe.\n(Long press below numbers to copy)" <<
            "\n        GPay :        " << GPAY_MOBILE << "\n        PayTm :      " << PAYTM_MOBILE << "\n        PhonePe : " << PHONE_PE_NO <<
            "\n\n2) Your Ref No: " << pUser->m_TransacNo << ". You <b>MUST MENTION " << pUser->m_TransacNo 
                    << "</b> while transfer.\n(Otherwise, we couldnt <b>Track your Payment</b>)" <<
            "\n\n3) WhatsApp Screen-Shot to "<< WHATSAPP_NO <<
            "\n\nAfter transferring, click appropriate \"Topped Up\" button below.";
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
	std::stringstream ss;
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
        	iRowIndex++;
        } else {
			getPaymentLink(pMsg->chat->id, pUser->m_TransacNo, std::stoi(strAmt), pUser->m_Name, pUser->m_Address, fp);

            getDBHandle()->insertToOrder(pUser, std::stoi(strAmt), CartStatus::PAYMENT_PENDING, "CFree", OrderType::TOPUP, fp);
            getDBHandle()->updateTransacNo(pUser->m_UserId, fp);

            ss.str(""); ss <<  "Hi " << pUser->m_Name << ",\n\n<b>Pls Wait a few secs..,</b>\n\nYou get a payment link. Pay by clicking it.";
            STR_MSG_DEFF_RELEASE  = ss.str();
        }
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
        STR_MSG_DEFF_RELEASE  = getPaymentString(pMsg->chat->id, pUser->m_OrderNo, pMsg->from->firstName, pUser->m_Address, iTotal, fp);
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
        if(std::string::npos != pMsg->text.find("PhonePe")) {strPGw  = "PhonePe"; strAmt = pMsg->text.substr(STR_BTN_PHONEPE_TOP_UP.length()+1);}

        //  Update database
        try{iAmt = std::stoi(strAmt);} catch(std::exception &e) {iAmt = 0;}
        if(0 < iAmt) {
            getDBHandle()->insertToOrder(pUser, iAmt, CartStatus::PAYMENT_PENDING, strPGw, OrderType::TOPUP, fp);
            getDBHandle()->updateTransacNo(pUser->m_UserId, fp);

            //  Notify admin
            ss << pUser->m_Name << " topped up Wallet using " << strPGw << ". Pls verify payment. His transac no is " << pUser->m_TransacNo;
            for(auto &id : adminChatIds)  notifyMsgs[id] = ss.str();
            getDBHandle()->updateNotifications(notifyMsgs, fp); notifyMsgs.clear();
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

TgBot::InputFile::Ptr Checkout::getMedia(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: Checkout getMedia {\n", time(0)); fflush(fp);
    TgBot::InputFile::Ptr pFile = nullptr;

    fprintf(fp, "BaseBot %ld: Checkout getMedia }\n", time(0)); fflush(fp);
    return pFile;
}
