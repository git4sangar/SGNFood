/*sgn/*
 * WalletMgmt.h
 *
 *  Created on: 14-Jul-2020
 *      Author: tstone10
 */

#ifndef INCLUDE_WALLETMGMT_H_
#define INCLUDE_WALLETMGMT_H_

#include <iostream>
#include <vector>

#include "Constants.h"
#include "DBInterface.h"
#include "BaseButton.h"

class WalletMgmt : public BaseButton, public std::enable_shared_from_this<WalletMgmt> {
    std::vector<POrder::Ptr> orders;
    std::string asset_file, pageName;
    unsigned int iNoOfItems, iSelPage;
    std::map<unsigned int, std::string> notifyMsgs;

public:
    WalletMgmt(DBInterface::Ptr hDB) : BaseButton(hDB), iNoOfItems(0), iSelPage(0) {}
    virtual ~WalletMgmt() {}

    //  This object is not created on every invocation. So clear it before using.
    void init(TgBot::Message::Ptr pMsg, FILE *fp) {
        orders.clear(); asset_file.clear(); pageName.clear(); iNoOfItems = 0;
        STR_MSG_DEFF_RELEASE   = "New / Confirmed / Delivered Orders";
        notifyMsgs.clear();
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

    std::map<unsigned int, std::string> getNotifyMsgs(TgBot::Message::Ptr pMessage, FILE *fp) { return notifyMsgs; }
    static std::string STR_MSG_DEFF_RELEASE;
};



#endif /* INCLUDE_WALLETMGMT_H_ */
