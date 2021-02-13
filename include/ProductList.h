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

#define MAX_BTNS_PER_ROW    (5)
#ifdef AURA
    #define PAGE_SUFFIX     "Product Page"
#else
    #define PAGE_SUFFIX     "Menu Page"
#endif

class ProductList : public BaseButton, public std::enable_shared_from_this<ProductList> {
    std::vector<Product::Ptr> products;
    std::string asset_file, strAddress;
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
        STR_MSG_DEFF_RELEASE    = "Click a button to add to Cart. Click <b>View Cart</b> once done.";
        //STR_MSG_DEFF_RELEASE   = "\"Buy\" adds an item to Cart. Refer \"Code\" above. Click again to increase qty.";
        //STR_MSG_DEFF_RELEASE   = "<b>Type & send</b> your orders like how you do in WhatsApp. While typing use these words <b>breakfast, bisibelebath, lunch, sambar, kuzhambu, rasam, curry, kootu, rice</b>";
        isAdmin = false; notifyMsgs.clear(); strAddress.clear();
    }
    std::string getMsg() { return STR_MSG_DEFF_RELEASE;}
    TgBot::GenericReply::Ptr prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& listKBBtns, TgBot::Message::Ptr pMsg, FILE *fp);
    void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
    std::shared_ptr<BaseButton> getSharedPtr() {return shared_from_this();}
    TgBot::InputFile::Ptr getMedia(TgBot::Message::Ptr pMsg, FILE *fp);

    void cleanup(TgBot::Message::Ptr pMsg, std::map<std::string, std::shared_ptr<BaseButton>>& listAuraBtns, FILE *fp){}
    std::map<unsigned int, std::string> getNotifyMsgs(TgBot::Message::Ptr pMessage, FILE *fp) { return notifyMsgs; }
    void create_product_table(std::string file_name, std::string strHdr, FILE *fp);

	bool isMobileNoPresent(std::string strAddress);
	std::string getMobileNo(std::string strAddress);

    static std::string STR_MSG_DEFF_RELEASE;
};




#endif /* PRODUCTLIST_H_ */
