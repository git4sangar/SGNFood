/*sgn
 * EditMenu.h
 *
 *  Created on: 02-Jul-2020
 *      Author: tstone10
 */

#ifndef EDITMENU_H_
#define EDITMENU_H_

#include <iostream>
#include <vector>

#include "DBInterface.h"
#include "BaseButton.h"

#define ITEMS_PAGE     "Items Page"

class EditMenu : public BaseButton, public std::enable_shared_from_this<EditMenu> {
    std::vector<Product::Ptr> products;
    std::string asset_file;
    unsigned int iSelPage, iNoOfItems;
    std::map<unsigned int, std::string> notifyMsgs;

public:
    EditMenu(DBInterface::Ptr hDB) : BaseButton(hDB), iSelPage(0), iNoOfItems(0) {}
    virtual ~EditMenu() {}

    //  This object is not created on every invocation. So clear it before using.
    void init(TgBot::Message::Ptr pMsg, FILE *fp) {
        products.clear(); asset_file.clear(); iSelPage = 0; iNoOfItems = 0;
        STR_MSG_DEFF_RELEASE   = std::string("Make ") + getDBHandle()->getTmrwDate() + std::string(" menu by choosing Ids below\n")
                                + std::string("\"+\" => adds to menu\n\"-\" => removes from menu");
       notifyMsgs.clear();
    }
    std::string getMsg() { return STR_MSG_DEFF_RELEASE;}
    TgBot::GenericReply::Ptr prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& listKBBtns, TgBot::Message::Ptr pMsg, FILE *fp);
    void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
    std::shared_ptr<BaseButton> getSharedPtr() {return shared_from_this();}
    TgBot::InputFile::Ptr getMedia(TgBot::Message::Ptr pMsg, FILE *fp);
    std::string getParseMode() {return std::string("HTML");}
    void create_product_table(std::string file_name ,FILE *fp);

    std::map<unsigned int, std::string> getNotifyMsgs(TgBot::Message::Ptr pMessage, FILE *fp) { return notifyMsgs; }
    static std::string STR_MSG_DEFF_RELEASE;
};




#endif /* EDITMENU_H_ */
