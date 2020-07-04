/*sgn
 * Version.h
 *
 *  Created on: 04-Jun-2020
 *      Author: tstone10
 */

#ifndef VERSION_H_
#define VERSION_H_

#include <iostream>
#include <vector>

#include "Constants.h"
#include "DBInterface.h"
#include "BaseButton.h"

class BotVersion : public BaseButton, public std::enable_shared_from_this<BotVersion> {

public:
    BotVersion(DBInterface::Ptr hDB) : BaseButton(hDB) {}
    virtual ~BotVersion() {}

    //  This object is not created on every invocation. So clear it before using.
    void init(TgBot::Message::Ptr pMsg, FILE *fp) {
        STR_MSG_DEFF_RELEASE   = std::string("Bot version is ") + std::string(SGN_BOT_VER);
    }
    std::string getMsg() { return STR_MSG_DEFF_RELEASE;}
    TgBot::GenericReply::Ptr prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& listKBBtns, TgBot::Message::Ptr pMsg, FILE *fp) {return nullptr;}
    void onClick(TgBot::Message::Ptr pMessage, FILE *fp) {}
    std::shared_ptr<BaseButton> getSharedPtr() {return shared_from_this();}
    TgBot::InputFile::Ptr getMedia(TgBot::Message::Ptr pMsg, FILE *fp) { return nullptr; }

    static std::string STR_MSG_DEFF_RELEASE;
};


#endif /* VERSION_H_ */
