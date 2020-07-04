/*sgn
 * SingleOrder.h
 *
 *  Created on: 07-Jun-2020
 *      Author: tstone10
 */

#ifndef SINGLEORDER_H_
#define SINGLEORDER_H_

#include <iostream>
#include <vector>
#include <map>

#include "pngwriter.h"

#include "Constants.h"
#include "DBInterface.h"
#include "BaseButton.h"

#define ORDER_PAGE      "Order Page"
#define INT_CNFM_ORDER  (201)   //STR_BTN_CNFM_ORDER
#define INT_CNCL_ORDER  (202)   //STR_BTN_CNCL_ORDER
#define INT_EDIT_ORDER  (203)   //STR_BTN_EDIT_ORDER
#define INT_PRINT_ORDER (204)   //STR_BTN_PRINT_ORDER
#define INT_DLVR_ORDER   (205)   //SRT_BTN_DELIVERED

class SingleOrder : public BaseButton, public std::enable_shared_from_this<SingleOrder> {
    std::vector<Cart::Ptr> orderItems;
    std::vector<Product::Ptr> products;
    std::map<std::string, unsigned int> mapCmdToInt;
    std::map<unsigned int, std::string> notifyMsgs;
    POrder::Ptr pOrder;
    std::string asset_file, pageName;
    UserContext usrCtxt;
    unsigned int iNoOfItems, iOrderNo, iPgNo;
    bool isPrint;

public:
    SingleOrder(DBInterface::Ptr hDB) : BaseButton(hDB), usrCtxt(USER_CTXT_NOTA), iNoOfItems(0), iOrderNo(0), iPgNo(0), isPrint(false) {}
    virtual ~SingleOrder() {}

    //  This object is not created on every invocation. So clear it before using.
    void init(TgBot::Message::Ptr pMsg, FILE *fp);
    std::string getParseMode() {return "HTML";}
    std::string getMsg() { return STR_MSG_DEFF_RELEASE;}
    TgBot::GenericReply::Ptr prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& listKBBtns, TgBot::Message::Ptr pMsg, FILE *fp);
    void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
    std::shared_ptr<BaseButton> getSharedPtr() {return shared_from_this();}
    std::vector<TgBot::InputFile::Ptr> getMedias(TgBot::Message::Ptr pMsg, FILE *fp);

    int create_order_items_table(std::shared_ptr<pngwriter> pPNGWriter, int yAxis, int iIndex, int iCurPg, int iMaxPg,  FILE *fp);

    //  Overriding cleanup, otherwise it clears up what is just set in prepareMenu
    void cleanup(TgBot::Message::Ptr pMsg, std::map<std::string, std::shared_ptr<BaseButton>>& listAuraBtns, FILE *fp){}

    std::vector<std::string> split_address(std::string strAddress);
    int addAddressToPNG(std::shared_ptr<pngwriter> pPNGWriter, int iPgLn, std::string strAddress, int iOrderNo, std::string strPayGW);

    std::map<unsigned int, std::string> getNotifyMsgs(TgBot::Message::Ptr pMessage, FILE *fp) { return notifyMsgs; }
    static std::string STR_MSG_DEFF_RELEASE;
};


#endif /* SINGLEORDER_H_ */
