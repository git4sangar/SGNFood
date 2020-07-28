/*SGN
 * GenericButton.h
 *
 *  Created on: 20-May-2020
 *      Author: tstone10
 */

#ifndef BASEBUTTON_H_
#define BASEBUTTON_H_


#include <iostream>
#include <stdio.h>
#include <memory>
#include <vector>
#include <regex>
#include <vector>

#include <tgbot/tgbot.h>
#include <SQLiteCpp/SQLiteCpp.h>

#include "DBInterface.h"

class BaseButton {
protected:
    DBInterface::Ptr m_hDB;

public:
    BaseButton(std::shared_ptr<DBInterface> hDB) : m_hDB{hDB} {}
    virtual ~BaseButton() {}
    DBInterface::Ptr getDBHandle() {return m_hDB;}

    std::vector<TgBot::KeyboardButton::Ptr> getMainMenu() {
        std::vector<TgBot::KeyboardButton::Ptr> row;
        TgBot::KeyboardButton::Ptr kbBtnMMenu;
        kbBtnMMenu          = std::make_shared<TgBot::KeyboardButton>();
        kbBtnMMenu->text    = "Main Menu";
        row.push_back(kbBtnMMenu);
        return row;
    }

    bool isFileExists(std::string strFile) {
        FILE *fp = fopen(strFile.c_str(), "r");
        if(NULL != fp) { fclose(fp); return true; }
        return false;
    }
    virtual std::string getStr4Admins(unsigned int userParam) {return std::string();}
    virtual std::string getStrForUser(TgBot::Message::Ptr pMsg) {return std::string();}

    virtual std::string getParseMode() {return std::string();}
    virtual TgBot::GenericReply::Ptr prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& listAuraBtns,
                TgBot::Message::Ptr pMsg, FILE *fp) = 0;
    virtual TgBot::InputFile::Ptr getMedia(TgBot::Message::Ptr pMsg, FILE *fp) {return nullptr;}
    virtual std::vector<TgBot::InputFile::Ptr> getMedias(TgBot::Message::Ptr pMsg, FILE *fp) {return std::vector<TgBot::InputFile::Ptr>();}

    virtual std::vector<unsigned int> getChatIdsForNotification(TgBot::Message::Ptr pMessage, FILE *fp) {return std::vector<unsigned int>();}
    //std::vector<unsigned int> getNotifyUsers() {return m_hDB->getNotifyUsers();}
    virtual std::map<unsigned int, std::string> getNotifyMsgs(TgBot::Message::Ptr pMessage, FILE *fp) { return std::map<unsigned int, std::string>();}

    virtual bool isPlsWait() {return true;}
    virtual void init(TgBot::Message::Ptr pMsg, FILE *fp) = 0;
    virtual void onClick(TgBot::Message::Ptr pMessage, FILE *fp) = 0;
    virtual std::string getMsg() = 0;
    virtual std::shared_ptr<BaseButton> getSharedPtr() = 0;
    virtual void cleanup(TgBot::Message::Ptr pMsg, std::map<std::string, std::shared_ptr<BaseButton>>& listAuraBtns, FILE *fp);
    virtual std::map<std::string, std::shared_ptr<BaseButton>>::const_iterator
        parse(std::string strOrder, std::map<std::string, std::shared_ptr<BaseButton>>& listKBBtns, unsigned int iChatId, FILE *fp) {
        return std::map<std::string, std::shared_ptr<BaseButton>>::const_iterator();
    }

    std::string myTrim(std::string str) {
        str = std::regex_replace(str, std::regex("^\\s+"), std::string(""));
        str = std::regex_replace(str, std::regex("\\s+$"), std::string(""));
        return str;
    }

    virtual void createKBBtn(std::string strName, std::vector<TgBot::KeyboardButton::Ptr> &row,
                             std::map<std::string, std::shared_ptr<BaseButton>>& lstBaseBtns, std::shared_ptr<BaseButton> pBtn = nullptr);
};


#endif /* BASEBUTTON_H_ */
