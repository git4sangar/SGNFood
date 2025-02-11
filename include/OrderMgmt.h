/*sgn
 * OrderMgmt.h
 *
 *  Created on: 07-Jun-2020
 *      Author: tstone10
 */

#ifndef ORDERMGMT_H_
#define ORDERMGMT_H_

#include <iostream>
#include <vector>

#include "Constants.h"
#include "DBInterface.h"
#include "BaseButton.h"

class OrderMgmt : public BaseButton, public std::enable_shared_from_this<OrderMgmt> {
    std::vector<POrder::Ptr> orders;
    std::string asset_file, pageName;
    unsigned int iNoOfItems, iSelPage;

public:
    OrderMgmt(DBInterface::Ptr hDB) : BaseButton(hDB), iNoOfItems(0), iSelPage(0) {}
    virtual ~OrderMgmt() {}

    //  This object is not created on every invocation. So clear it before using.
    void init(TgBot::Message::Ptr pMsg, FILE *fp) {
        orders.clear(); asset_file.clear(); pageName.clear(); iNoOfItems = 0;
        STR_MSG_DEFF_RELEASE   = "New / Confirmed / Delivered Orders";
    }
    std::string getParseMode() {return "HTML";}
    std::string getMsg() { return STR_MSG_DEFF_RELEASE;}
    TgBot::GenericReply::Ptr prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& listKBBtns, TgBot::Message::Ptr pMsg, FILE *fp);
    void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
    std::shared_ptr<BaseButton> getSharedPtr() {return shared_from_this();}
    TgBot::InputFile::Ptr getMedia(TgBot::Message::Ptr pMsg, FILE *fp);

    void create_order_table(std::string file_name, int iPageNo, FILE *fp);
    //UserContext getContextForPageName(std::string strPageName);

    //  Overriding cleanup, otherwise it clears up what is just set in prepareMenu
    void cleanup(TgBot::Message::Ptr pMsg, std::map<std::string, std::shared_ptr<BaseButton>>& listAuraBtns, FILE *fp){}

    static std::string STR_MSG_DEFF_RELEASE;
};

/*class YourOrder : public BaseButton, public std::enable_shared_from_this<YourOrder> {
    std::vector<POrder::Ptr> orders;
    std::string asset_file, pageName;
    unsigned int iNoOfItems, iSelPage;

public:
    YourOrder(DBInterface::Ptr hDB) : BaseButton(hDB), iNoOfItems(0), iSelPage(0) {}
    virtual ~YourOrder() {}

    //  This object is not created on every invocation. So clear it before using.
    void init(TgBot::Message::Ptr pMsg, FILE *fp) {
        orders.clear(); asset_file.clear(); pageName.clear(); iNoOfItems = 0;
        STR_MSG_DEFF_RELEASE   = "Choose following options to change price";
    }
    std::string getParseMode() {return "HTML";}
    std::string getMsg() { return STR_MSG_DEFF_RELEASE;}
    TgBot::GenericReply::Ptr prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& listKBBtns, TgBot::Message::Ptr pMsg, FILE *fp);
    void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
    std::shared_ptr<BaseButton> getSharedPtr() {return shared_from_this();}
    TgBot::InputFile::Ptr getMedia(TgBot::Message::Ptr pMsg, FILE *fp);

    void create_order_table(std::string file_name, int iPageNo, FILE *fp);
    UserContext getContextForPageName(std::string strPageName);

    //  Overriding cleanup, otherwise it clears up what is just set in prepareMenu
    //void cleanup(TgBot::Message::Ptr pMsg, std::map<std::string, std::shared_ptr<BaseButton>>& listAuraBtns, FILE *fp){}

    static std::string STR_MSG_DEFF_RELEASE;
};

UserContext OrderMgmt::getContextForPageName(std::string strPageName) {
    if(!strPageName.compare(STR_BTN_YOUR_ORDERS))    return USER_CTXT_YOUR_ORDER;
    if(!strPageName.compare(STR_BTN_NEW_ORDERS))     return USER_CTXT_NEW_ORDER;
    if(!strPageName.compare(STR_BTN_CNF_ORDERS))     return USER_CTXT_CNF_ORDER;
    return USER_CTXT_NOTA;
}

*/



#endif /* ORDERMGMT_H_ */
