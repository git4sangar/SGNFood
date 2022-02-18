/*SGN
 * SGNAdmin.h
 *
 *  Created on: 15-Jul-2020
 *      Author: tstone10
 */

#ifndef INCLUDE_SGNADMIN_H_
#define INCLUDE_SGNADMIN_H_

#include <iostream>
#include <vector>

#include "Constants.h"
#include "DBInterface.h"
#include "BaseButton.h"

class SGNAdmin : public BaseButton, public std::enable_shared_from_this<SGNAdmin> {
    std::map<int64_t, std::string> notifyMsgs;
public:
    SGNAdmin(DBInterface::Ptr hDB) : BaseButton(hDB) {
    }
    virtual ~SGNAdmin() {}

    //  This object is not created on every invocation. So clear it before using.
    void init(TgBot::Message::Ptr pMsg, FILE *fp) {
        STR_MSG_DEFF_RELEASE.clear();
        notifyMsgs.clear();
    }

    std::string getParseMode() {return "HTML";}
    std::string getMsg() { return STR_MSG_DEFF_RELEASE;}
    std::shared_ptr<BaseButton> getSharedPtr() {return shared_from_this();}

    TgBot::GenericReply::Ptr prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& listKBBtns, TgBot::Message::Ptr pMsg, FILE *fp);
    void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
    void cleanup(TgBot::Message::Ptr pMsg, std::map<std::string, std::shared_ptr<BaseButton>>& listAuraBtns, FILE *fp) {}

    std::map<int64_t, std::string> getNotifyMsgs(TgBot::Message::Ptr pMessage, FILE *fp) { return notifyMsgs; }

    std::vector<std::string> split_address(std::string strAddress);
    std::vector<unsigned int> getUserIds(std::string strMsg, FILE *fp, char delimiter);
    static std::string STR_MSG_DEFF_RELEASE;
};



#endif /* INCLUDE_SGNADMIN_H_ */
