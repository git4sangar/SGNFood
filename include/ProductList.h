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

#define SGN_OPTION_01   "Need Coconut Soap"
#define SGN_OPTION_02   "Need Neem Soap"
#define SGN_OPTION_03   "Need Sandal Soap"
#define SGN_OPTION_04   "Need Kasturi Turmeric"
#define SGN_OPTION_05   "Not Interested"

#ifdef AURA
    #define PAGE_SUFFIX     "Product Page"
#else
    #define PAGE_SUFFIX     "Menu Page"
#endif

class ProductList : public BaseButton, public std::enable_shared_from_this<ProductList> {
    std::vector<Product::Ptr> products;
    std::string asset_file;
    unsigned int iSelPage, iNoOfItems;
    std::vector<std::string> survey_resp;
    std::map<unsigned int, std::string> notifyMsgs;
    bool isAdmin, isSurvey;

public:
    ProductList(DBInterface::Ptr hDB) : BaseButton(hDB), iSelPage(0), iNoOfItems(0), isAdmin(false), isSurvey(false) {}
    virtual ~ProductList() {}

    std::string getParseMode() {return "HTML";}
    //  This object is not created on every invocation. So clear it before using.
    void init(TgBot::Message::Ptr pMsg, FILE *fp) {
        //  Add it in the same order as MACROs above
        survey_resp.clear();
        survey_resp.push_back(SGN_OPTION_01);
        survey_resp.push_back(SGN_OPTION_02);
        survey_resp.push_back(SGN_OPTION_03);
        survey_resp.push_back(SGN_OPTION_04);
        survey_resp.push_back(SGN_OPTION_05);
        products.clear(); asset_file.clear(); iSelPage = 0; iNoOfItems = 0;
        STR_MSG_DEFF_RELEASE   = "\"Buy\" adds an item to Cart. Refer \"Code\" above. Click again to increase qty.";
//STR_MSG_DEFF_RELEASE   = "<b>Type & send</b> your orders like how you do in WhatsApp. While typing use these words <b>breakfast, bisibelebath, lunch, sambar, kuzhambu, rasam, curry, kootu, rice</b>";
        isAdmin = false; isSurvey = false; notifyMsgs.clear();
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
