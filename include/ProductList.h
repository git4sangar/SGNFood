/*sgn
 * Products.h
 *
 *  Created on: 23-May-2020
 *      Author: tstone10
 */

#ifndef PRODUCTLIST_H_
#define PRODUCTLIST_H_

#include <iostream>
#include <vector>

#include "DBInterface.h"
#include "BaseButton.h"

#define PAGE_SUFFIX     "Product Page"

class ProductList : public BaseButton, public std::enable_shared_from_this<ProductList> {
    std::vector<Product::Ptr> products;
    std::string asset_file;
    unsigned int iSelPage, iNoOfItems;
    std::map<unsigned int, std::string> notifyMsgs;
    bool isAdmin;

public:
    ProductList(DBInterface::Ptr hDB) : BaseButton(hDB), iSelPage(0), iNoOfItems(0), isAdmin(false) {}
    virtual ~ProductList() {}

    std::string getParseMode() {return "HTML";}
    //  This object is not created on every invocation. So clear it before using.
    void init(TgBot::Message::Ptr pMsg, FILE *fp) {
        products.clear(); asset_file.clear(); iSelPage = 0; iNoOfItems = 0;
        STR_MSG_DEFF_RELEASE   = "Click \"Buy\" button below to add an item, with above Code, to Cart.";
        isAdmin = false; notifyMsgs.clear();
    }
    std::string getMsg() { return STR_MSG_DEFF_RELEASE;}
    TgBot::GenericReply::Ptr prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& listKBBtns, TgBot::Message::Ptr pMsg, FILE *fp);
    void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
    std::shared_ptr<BaseButton> getSharedPtr() {return shared_from_this();}
    TgBot::InputFile::Ptr getMedia(TgBot::Message::Ptr pMsg, FILE *fp);

    std::map<unsigned int, std::string> getNotifyMsgs(TgBot::Message::Ptr pMessage, FILE *fp) { return notifyMsgs; }
    void create_product_table(std::string file_name ,FILE *fp);

    static std::string STR_MSG_DEFF_RELEASE;
};




#endif /* PRODUCTLIST_H_ */
