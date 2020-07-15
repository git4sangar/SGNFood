/*sgn
 * Address.cpp
 *
 *  Created on: 31-May-2020
 *      Author: tstone10
 */

#include <iostream>
#include <memory>

#include "pngwriter.h"

#include "Constants.h"
#include "MyAddress.h"


std::string MyAddress::STR_MSG_DEFF_RELEASE = "Please enter your address";

std::string getAddressNotification() {
   return
   std::string("Type address & mobile no in short form & send. <b>MOBILE No IS MUST.</b>") +
   std::string("\nNote: This screen repeats until you give a 10 digit mobile no.") +
   std::string("\n\nExample:\nB6 105, SSM Nagar, Perungalathur, 98452 35323") +
   std::string("\n\n24A 101, Bollineni Hillside, Nookampalayam, 88765 89623") +
   std::string("\n\nD3F2 Navins Brookfield, Nanmangalam, 91765 66966") +
   std::string("\n\nM 104 Purva Fountain Square, Marathahalli, 70102 80604");
}

TgBot::GenericReply::Ptr MyAddress::prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& lstBaseBtns, TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: MyAddress::prepareMenu {\n", time(0)); fflush(fp);

    std::map<unsigned int, UserContext>::const_iterator itrCntxt;
    std::map<std::string, std::shared_ptr<BaseButton> >::const_iterator itrBtn;

    std::vector<TgBot::KeyboardButton::Ptr> row[MAX_BUTTON_ROWS];
    TgBot::ReplyKeyboardMarkup::Ptr pMainMenu;
    unsigned int iRowIndex = 0, iLoop = 0;
    std::string strChatId   = std::to_string(pMsg->chat->id);

    iRowIndex       = 0;
    if(strAddress.empty() || !pMsg->text.compare(STR_BTN_CHANGE_ADDRESS)) {
        fprintf(fp, "BaseBot %ld: Asking user to type the address\n", time(0)); fflush(fp);
        m_Context[pMsg->chat->id]   = USER_CTXT_ADDRESS;
        lstBaseBtns[strChatId]      = getSharedPtr();
        STR_MSG_DEFF_RELEASE        = getAddressNotification();
        return std::make_shared<TgBot::ReplyKeyboardRemove>();
    }

    else if(!pMsg->text.compare(STR_BTN_SHPG_ADDRESS)) {
        fprintf(fp, "BaseBot %ld: Got request to render addresses\n", time(0)); fflush(fp);
        STR_MSG_DEFF_RELEASE  = "Check your shipping address.";

        createKBBtn(strAddress, row[iRowIndex++], lstBaseBtns, (isCartEmpty) ? lstBaseBtns[STR_BTN_MAINMENU] : lstBaseBtns[STR_BTN_VIEW_CART]);
        m_Context[pMsg->chat->id] = USER_CTXT_CLEANUP;

        createKBBtn(STR_BTN_CHANGE_ADDRESS, row[iRowIndex++], lstBaseBtns);
    }

    else if(m_Context.end() != (itrCntxt = m_Context.find(pMsg->chat->id)) ) {
        fprintf(fp, "BaseBot %ld: Processing typed address\n", time(0)); fflush(fp);
        std::stringstream ss;
        ss << "Thanks. Your shipping address:\n<b>" << pMsg->text << "</b> is noted.";
        STR_MSG_DEFF_RELEASE  = ss.str();

        m_Context.erase(itrCntxt);
        if(lstBaseBtns.end() != (itrBtn = lstBaseBtns.find(strChatId))) lstBaseBtns.erase(itrBtn);

        createKBBtn(STR_BTN_CHANGE_ADDRESS, row[iRowIndex], lstBaseBtns);
        createKBBtn(STR_BTN_CHECKOUT, row[iRowIndex], lstBaseBtns);
        iRowIndex++;
        createKBBtn(STR_BTN_MAINMENU, row[iRowIndex], lstBaseBtns);
        createKBBtn(STR_BTN_VIEW_CART, row[iRowIndex], lstBaseBtns);
        iRowIndex++;
    }

    pMainMenu   = std::make_shared<TgBot::ReplyKeyboardMarkup>();
    for(iLoop = 0; iLoop < iRowIndex; iLoop++) {
       pMainMenu->keyboard.push_back(row[iLoop]);
    }

    fprintf(fp, "BaseBot %ld: MyAddress::prepareMenu }\n", time(0)); fflush(fp);
    return pMainMenu;
}

bool MyAddress::isMobileNoPresent(std::string strAddress) {
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

void MyAddress::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: MyAddress onClick pMessage %s {\n", time(0), pMsg->text.c_str()); fflush(fp);

    User::Ptr pUser = getDBHandle()->getUserForChatId(pMsg->chat->id, fp);
    if(!pUser) return;

    std::vector<Cart::Ptr> cartItems;
    cartItems   = getDBHandle()->getCartItemsForOrderNo(pUser->m_OrderNo, fp);
    isCartEmpty = (cartItems.size() == 0);
    iUserId     = pUser->m_UserId;
    if(isMobileNoPresent(pUser->m_Address)) strAddress  = pUser->m_Address;

    std::map<unsigned int, UserContext>::const_iterator itrCntxt;
    if(m_Context.end() != (itrCntxt = m_Context.find(pMsg->chat->id)) ) {
        if(isMobileNoPresent(pMsg->text)) {
            getDBHandle()->addAddressToShipping(iUserId, pMsg->text, fp);
            strAddress  = pMsg->text;
        } else {
            strAddress.clear();
        }
    }
    fprintf(fp, "BaseBot %ld: MyAddress onClick }\n", time(0)); fflush(fp);
}
