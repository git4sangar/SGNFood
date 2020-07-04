/*sgn
 * Address.h
 *
 *  Created on: 31-May-2020
 *      Author: tstone10
 */

#ifndef MYADDRESS_H_
#define MYADDRESS_H_

#include <iostream>
#include <vector>

#include "DBInterface.h"
#include "BaseButton.h"

class MyAddress : public BaseButton, public std::enable_shared_from_this<MyAddress> {
    unsigned int iUserId;
    std::string strAddress;
    bool isCartEmpty;
public:
    MyAddress(DBInterface::Ptr hDB) : BaseButton(hDB), iUserId(0), isCartEmpty(false) {}
    virtual ~MyAddress() {}

    //  This object is not created on every invocation. So clear it before using.
    void init(TgBot::Message::Ptr pMsg, FILE *fp) {
        iUserId = 0; isCartEmpty = false; strAddress.clear();
    }
    std::string getMsg() { return STR_MSG_DEFF_RELEASE;}
    TgBot::GenericReply::Ptr prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& listKBBtns, TgBot::Message::Ptr pMsg, FILE *fp);
    void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
    std::shared_ptr<BaseButton> getSharedPtr() {return shared_from_this();}
    TgBot::InputFile::Ptr getMedia(TgBot::Message::Ptr pMsg, FILE *fp) { return nullptr; }
    std::string getParseMode() {return "HTML";}


    void create_cart_table(std::string file_name, FILE *fp);
    unsigned int findAssociatedNumber(std::string strMsg, std::string strFind, int iMaxItems);

    //  Overriding cleanup, otherwise it clears up what is just set in prepareMenu
    void cleanup(TgBot::Message::Ptr pMsg, std::map<std::string, std::shared_ptr<BaseButton>>& listAuraBtns, FILE *fp){}

    bool isMobileNoPresent(std::string strAddress);
    static std::string STR_MSG_DEFF_RELEASE;
};

#endif /* MYADDRESS_H_ */
