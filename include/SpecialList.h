/*sgn
 * SpecialList.h
 *
 *  Created on: 23-May-2020
 *      Author: tstone10
 */

#ifndef SPECIALLIST_H_
#define SPECIALLIST_H_

#include <iostream>
#include <vector>

#include "DBInterface.h"
#include "BaseButton.h"

#ifdef AURA
    #define PAGE_SUFFIX     "Product Page"
#else
    #define PAGE_SUFFIX     "Menu Page"
#endif

class SpecialList : public BaseButton, public std::enable_shared_from_this<SpecialList> {
    std::vector<std::string> categories;
    std::vector<Product::Ptr> products;
    std::string asset_file, category, strAddress;
    unsigned int iSelPage, iNoOfItems;
    std::map<int64_t, std::string> notifyMsgs;
    bool isAdmin;

public:
    SpecialList(DBInterface::Ptr hDB) : BaseButton(hDB), iSelPage(0), iNoOfItems(0), isAdmin(false) {}
    virtual ~SpecialList() {}

    std::string getParseMode() {return "HTML";}
    //  This object is not created on every invocation. So clear it before using.
    void init(TgBot::Message::Ptr pMsg, FILE *fp) {
        categories.clear(); category.clear(); strAddress.clear();
        products.clear(); asset_file.clear(); iSelPage = 0; iNoOfItems = 0;
        STR_MSG_DEFF_RELEASE   = "Diwali Sweets and Karam Each pack 250g. Marundu(legiyum) 50gms . Min order ₹875(5 packets). Delivery starts 22nd";
        isAdmin = false; notifyMsgs.clear();
    }
    std::string getMsg() { return STR_MSG_DEFF_RELEASE;}
    TgBot::GenericReply::Ptr prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& listKBBtns, TgBot::Message::Ptr pMsg, FILE *fp);
    void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
    std::shared_ptr<BaseButton> getSharedPtr() {return shared_from_this();}
    TgBot::InputFile::Ptr getMedia(TgBot::Message::Ptr pMsg, FILE *fp);

    std::map<int64_t, std::string> getNotifyMsgs(TgBot::Message::Ptr pMessage, FILE *fp) { return notifyMsgs; }
    void create_product_table(std::string file_name ,FILE *fp);
    void cleanup(TgBot::Message::Ptr pMsg, std::map<std::string, std::shared_ptr<BaseButton>>& listAuraBtns, FILE *fp){}
    static std::string STR_MSG_DEFF_RELEASE;
};




#endif /* SpecialList_H_ */
