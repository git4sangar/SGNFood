/*sgn
 * QuickMenu.h
 *
 *  Created on: 27-Jul-2020
 *      Author: tstone10
 */

#ifndef INCLUDE_QUICKMENU_H_
#define INCLUDE_QUICKMENU_H_

#include <map>
#include "fifo_map.h"

#define QTY_PER_ROW     (4)

//  Description column of Product Table
#define BREAKFAST       "Breakfast"
#define BISIBELEBATH    "Bisibele Bath"
#define SAMBAR          "Sambar"
#define MORKUZHAMBU     "Morkuzhambu"
#define VATHAKUZHAMBU   "Vathakuzhambu"
#define RASAM           "Rasam"
#define CURRY           "Curry"
#define KOOTU           "Kootu"
#define RICE            "Rice"
#define LUNCH           "Full Lunch"
#define ULUNDU_VADAI    "Ulundu Vadai(2)"
#define COCONUT_POLI    "Coconut Poli(3)"
#define DHAL_POLI       "Dhall Poli(3)"

class QuickMenu : public BaseButton, public std::enable_shared_from_this<QuickMenu> {
    nlohmann::fifo_map<std::string, int> qty, pcs;
    std::map<unsigned int, std::string> notifyMsgs;
    Product::Ptr pProd;
    std::map<std::string, std::string> descToCode;
public:
    QuickMenu(DBInterface::Ptr hDB) : BaseButton(hDB) {
    }
    virtual ~QuickMenu() {}

    //  This object is not created on every invocation. So clear it before using.
    void init(TgBot::Message::Ptr pMsg, FILE *fp) {
        STR_MSG_DEFF_RELEASE.clear();
        notifyMsgs.clear();
        pProd = nullptr;

        descToCode.clear();
        descToCode[BREAKFAST]       = "TF-";
        descToCode[BISIBELEBATH]    = "TF-";
        descToCode[SAMBAR]          = "SB-";
        descToCode[MORKUZHAMBU]     = "SB-";
        descToCode[VATHAKUZHAMBU]   = "SB-";
        descToCode[RASAM]           = "RS-";
        descToCode[CURRY]           = "CR-";
        descToCode[KOOTU]           = "KT-";
        descToCode[RICE]            = "RC-";
        descToCode[LUNCH]           = "LN-";
        descToCode[ULUNDU_VADAI]    = "Ulundu Vadai(2)";
        descToCode[COCONUT_POLI]    = "Coconut Poli(3)";
        descToCode[DHAL_POLI]       = "Dhall Poli(3)";

        //  Shall be in multiples of QTY_PER_ROW
        pcs.clear();
        pcs["1"]       = 1;
        pcs["2"]       = 2;
        pcs["3"]       = 3;
        pcs["4"]       = 4;
        pcs["5"]       = 5;
        pcs["6"]       = 6;
        pcs["7"]       = 7;
        pcs["8"]       = 8;

        qty.clear();
        qty["500g"]     = 500;
        qty["1kg"]      = 1000;
        qty["1.5kg"]    = 1500;
        qty["2kg"]      = 2000;
        qty["2.5kg"]    = 2500;
        qty["5kg"]      = 5000;
        qty["10kg"]     = 10000;
        qty["15kg"]     = 15000;
    }

    std::string getParseMode() {return "HTML";}
    std::string getMsg() { return STR_MSG_DEFF_RELEASE;}
    std::shared_ptr<BaseButton> getSharedPtr() {return shared_from_this();}

    TgBot::GenericReply::Ptr prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& listKBBtns, TgBot::Message::Ptr pMsg, FILE *fp);
    void onClick(TgBot::Message::Ptr pMessage, FILE *fp);

    std::map<unsigned int, std::string> getNotifyMsgs(TgBot::Message::Ptr pMessage, FILE *fp) { return notifyMsgs; }

    static std::string STR_MSG_DEFF_RELEASE;
};



#endif /* INCLUDE_QUICKMENU_H_ */
