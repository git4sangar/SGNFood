/*sgn
 * FAQs.h
 *
 *  Created on: 16-Jun-2020
 *      Author: tstone10
 */

#ifndef FAQS_H_
#define FAQS_H_

#include <iostream>
#include <vector>

#include "DBInterface.h"
#include "BaseButton.h"

#define FAQ_PAGE_PREFIX     "FAQ Page"
#define MAX_FAQ_FILE_SZ     (50*1024)
#define MAX_FAQ_PAGES       (4)

class FAQs : public BaseButton, public std::enable_shared_from_this<FAQs> {
    unsigned int iSelPage;
public:
    FAQs(DBInterface::Ptr hDB) : BaseButton(hDB), iSelPage(0) {}
    virtual ~FAQs() {}

    //  This object is not created on every invocation. So clear it before using.
    void init(TgBot::Message::Ptr pMsg, FILE *fp) {
        iSelPage = 0;
        STR_FAQ_MSG   = "FAQs";
    }
    std::string getMsg() { return STR_FAQ_MSG;}
    std::shared_ptr<BaseButton> getSharedPtr() {return shared_from_this();}

    TgBot::GenericReply::Ptr prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& listKBBtns, TgBot::Message::Ptr pMsg, FILE *fp);
    void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
    TgBot::InputFile::Ptr getMedia(TgBot::Message::Ptr pMsg, FILE *fp);

    void split_faqs(std::string strFAQLine);
    void create_faq_page(std::string file_name, FILE *fp);

    static std::string STR_FAQ_MSG;
};



#endif /* FAQS_H_ */
