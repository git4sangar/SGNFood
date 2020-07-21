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
    std::map<unsigned int, std::string> notifyMsgs;
public:
    SGNAdmin(DBInterface::Ptr hDB) : BaseButton(hDB) {
        notifyMsgs.clear();
    }
    virtual ~SGNAdmin() {}

    //  This object is not created on every invocation. So clear it before using.
    void init(TgBot::Message::Ptr pMsg, FILE *fp) {
        STR_MSG_DEFF_RELEASE.clear();
    }

    std::string getParseMode() {return "HTML";}
    std::string getMsg() { return STR_MSG_DEFF_RELEASE;}
    std::shared_ptr<BaseButton> getSharedPtr() {return shared_from_this();}

    TgBot::GenericReply::Ptr prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& listKBBtns, TgBot::Message::Ptr pMsg, FILE *fp);
    void onClick(TgBot::Message::Ptr pMessage, FILE *fp);

    std::map<unsigned int, std::string> getNotifyMsgs(TgBot::Message::Ptr pMessage, FILE *fp) { return notifyMsgs; }

    static std::string STR_MSG_DEFF_RELEASE;
};



#endif /* INCLUDE_SGNADMIN_H_ */
