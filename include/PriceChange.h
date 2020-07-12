/*sgn
 * PriceChange.h
 *
 *  Created on: 06-Jun-2020
 *      Author: tstone10
 */

#ifndef PRICECHANGE_H_
#define PRICECHANGE_H_

#include <iostream>
#include <vector>

#include "DBInterface.h"
#include "BaseButton.h"

#define PROD_TOKENS (3)

class PriceChange : public BaseButton, public std::enable_shared_from_this<PriceChange> {
    Category::Ptr pCategory;
    std::vector<Product::Ptr> products;
    std::string asset_file;
    unsigned int iNoOfItems;
    bool plsWait;

public:
    PriceChange(DBInterface::Ptr hDB) : BaseButton(hDB), iNoOfItems(0), plsWait(true) {}
    virtual ~PriceChange() {}

    //  This object is not created on every invocation. So clear it before using.
    void init(TgBot::Message::Ptr pMsg, FILE *fp) {
        pCategory = nullptr; products.clear(); asset_file.clear(); iNoOfItems = 0;
        STR_MSG_DEFF_RELEASE.clear();
        plsWait = true;
    }
    std::string getParseMode() {return "HTML";}
    std::string getMsg() { return STR_MSG_DEFF_RELEASE;}
    TgBot::GenericReply::Ptr prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& listKBBtns, TgBot::Message::Ptr pMsg, FILE *fp);
    void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
    std::shared_ptr<BaseButton> getSharedPtr() {return shared_from_this();}
    std::vector<TgBot::InputFile::Ptr> getMedias(TgBot::Message::Ptr pMsg, FILE *fp);

    bool isPlsWait() {return plsWait;}
    void create_product_table(std::string file_name, int iPageNo, FILE *fp);

    //  Overriding cleanup, otherwise it clears up what is just set in prepareMenu
    void cleanup(TgBot::Message::Ptr pMsg, std::map<std::string, std::shared_ptr<BaseButton>>& listAuraBtns, FILE *fp){}

    static std::string STR_MSG_DEFF_RELEASE;
};



#endif /* PRICECHANGE_H_ */
