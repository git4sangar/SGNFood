/*sgn
 * Checkout.h
 *
 *  Created on: 30-May-2020
 *      Author: tstone10
 */

#ifndef CHECKOUT_H_
#define CHECKOUT_H_

#include <iostream>
#include <vector>

#include "DBInterface.h"
#include "BaseButton.h"

//#define STR_BTN_GPAY_TOP_UP     "Topped Up GPay"
//#define STR_BTN_BHIM_TOP_UP     "Topped Up BHIM"
//#define STR_BTN_PAYTM_TOP_UP    "Topped Up PayTM"

class Checkout : public BaseButton, public std::enable_shared_from_this<Checkout> {
    int iTotal;
    unsigned int iNoOfItems;
    User::Ptr pUser;
    std::vector<Cart::Ptr> cartItems;
    std::map<unsigned int, std::string> notifyMsgs;

public:
    Checkout(DBInterface::Ptr hDB) : BaseButton(hDB), iTotal(0), iNoOfItems(0), pUser(nullptr) {}
    virtual ~Checkout() {}

    //  This object is not created on every invocation. So clear it before using.
    void init(TgBot::Message::Ptr pMsg, FILE *fp) {
        pUser = nullptr;iTotal = 0;
        iNoOfItems = 0; cartItems.clear();
        notifyMsgs.clear();
    }
    bool isMobileNoPresent(std::string strAddress);
    std::string getMsg() { return STR_MSG_DEFF_RELEASE;}
    TgBot::GenericReply::Ptr prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& listKBBtns, TgBot::Message::Ptr pMsg, FILE *fp);
    void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
    std::shared_ptr<BaseButton> getSharedPtr() {return shared_from_this();}
    std::string getParseMode() {return "HTML";}

    std::string getPaymentString(unsigned int iOrderNo, std::string strName, std::string strAddress, int iTotal, FILE *fp);
    std::string getTopUpString();

    //  Overriding cleanup, otherwise it clears up what is just set in prepareMenu
    void cleanup(TgBot::Message::Ptr pMsg, std::map<std::string, std::shared_ptr<BaseButton>>& listAuraBtns, FILE *fp){}

    std::map<unsigned int, std::string> getNotifyMsgs(TgBot::Message::Ptr pMessage, FILE *fp) { return notifyMsgs; }
    TgBot::InputFile::Ptr getMedia(TgBot::Message::Ptr pMsg, FILE *fp);

    static std::string STR_MSG_DEFF_RELEASE;
    static std::string STR_BTN_GPAY_TOP_UP, STR_BTN_PAYTM_TOP_UP, STR_BTN_PHONEPE_TOP_UP;
};



#endif /* CHECKOUT_H_ */
