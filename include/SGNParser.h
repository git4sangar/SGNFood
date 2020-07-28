/*sgn
 * SGNParser.h
 *
 *  Created on: 28-Jul-2020
 *      Author: tstone10
 */

#ifndef INCLUDE_SGNPARSER_H_
#define INCLUDE_SGNPARSER_H_

#include <iostream>
#include <vector>
#include <map>

#include "Constants.h"
#include "DBInterface.h"
#include "BaseButton.h"

enum class item { none, breakfast, lunch, sambar, rasam, curry, kootu, rice, quantity };

class SGNParser : public BaseButton, public std::enable_shared_from_this<SGNParser> {
    std::map<std::string, item> depot;
    std::map<std::string, int> qty;
    bool isDepotIntialized, isQtyInitialized;

public:
    SGNParser(DBInterface::Ptr hDB) : BaseButton(hDB), isDepotIntialized(false), isQtyInitialized(false) { }
    virtual ~SGNParser() {}

    //  This object is not created on every invocation. So clear it before using.
    void init(TgBot::Message::Ptr pMsg, FILE *fp) {initDepot(); initQty();}

    std::string getParseMode() {return "HTML";}
    std::shared_ptr<BaseButton> getSharedPtr() {return shared_from_this();}

    TgBot::GenericReply::Ptr prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& listKBBtns, TgBot::Message::Ptr pMsg, FILE *fp) {return nullptr;}
    void onClick(TgBot::Message::Ptr pMessage, FILE *fp) {}
    virtual std::string getMsg() {return "";}

    void initDepot();
    void initQty();
    std::string getItem(item itm, FILE *fp);
    int getQuantity(std::string strQty, FILE *fp);
    std::map<std::string, std::shared_ptr<BaseButton>>::const_iterator
            parse(std::string strOrder, std::map<std::string, std::shared_ptr<BaseButton>>& listKBBtns, unsigned int iChatId, FILE *fp);
    Product::Ptr getProduct(item itm, FILE *fp);
};


#endif /* INCLUDE_SGNPARSER_H_ */
