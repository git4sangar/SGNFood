/*sgn
 * FAQs.cpp
 *
 *  Created on: 16-Jun-2020
 *      Author: tstone10
 */


#include <iostream>

#include "pngwriter.h"

#include "Constants.h"
#include "FAQs.h"

std::string FAQs::STR_FAQ_MSG = "FAQs";

TgBot::GenericReply::Ptr FAQs::prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& lstBaseBtns, TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: FAQs::prepareMenu {\n", time(0)); fflush(fp);

    unsigned int iPrev, iNext, iLoop, iRowIndex = 0;
    std::vector<TgBot::KeyboardButton::Ptr> row[MAX_BUTTON_ROWS];
    TgBot::ReplyKeyboardMarkup::Ptr pMainMenu;

    iRowIndex = 0;
    iPrev   = (iSelPage == 1) ? 1 : (iSelPage - 1);
    createKBBtn(std::string(FAQ_PAGE_PREFIX) + std::string(" ") + std::to_string(iPrev), row[iRowIndex], lstBaseBtns, getSharedPtr());

    iNext   = (iSelPage < MAX_FAQ_PAGES) ? (iSelPage + 1) : iSelPage;
    createKBBtn(std::string(FAQ_PAGE_PREFIX) + std::string(" ") + std::to_string(iNext), row[iRowIndex], lstBaseBtns, getSharedPtr());
    iRowIndex++;

    //  Populate the next available row
    createKBBtn(STR_BTN_MAINMENU, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_VIEW_CART, row[iRowIndex], lstBaseBtns);
    createKBBtn(STR_BTN_YOUR_ORDERS, row[iRowIndex], lstBaseBtns);
    iRowIndex++;

    //  Add all the rows to main menu
    pMainMenu   = std::make_shared<TgBot::ReplyKeyboardMarkup>();
    for(iLoop = 0; iLoop < iRowIndex; iLoop++) {
       pMainMenu->keyboard.push_back(row[iLoop]);
    }

    fprintf(fp, "BaseBot %ld: FAQs::prepareMenu }\n", time(0)); fflush(fp);
    pMainMenu->resizeKeyboard   = true;
    return pMainMenu;
}

void FAQs::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: FAQs onClick pMessage %s {\n", time(0), pMsg->text.c_str()); fflush(fp);
    unsigned int iLoop = 0;

    iSelPage    = 1;
    if(!pMsg->text.compare(STR_BTN_ABOUT_US))
        iSelPage = 4;
    if(std::string::npos != pMsg->text.find(FAQ_PAGE_PREFIX))
        for(iLoop = 1; iLoop <= MAX_NO_OF_PAGES; iLoop++) {
            std::stringstream ssPage;
            ssPage << FAQ_PAGE_PREFIX << " " << iLoop;

            if(!pMsg->text.compare(ssPage.str())) {
                iSelPage  = iLoop;
                break;
            }
        }
    fprintf(fp, "BaseBot %ld: FAQs onClick pMessage }\n", time(0)); fflush(fp);
}

TgBot::InputFile::Ptr FAQs::getMedia(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: FAQs::getMedia {\n", time(0)); fflush(fp);
    TgBot::InputFile::Ptr pFile = nullptr;

    std::string asset_file  = std::string(BOT_ROOT_PATH) + std::string(BOT_ASSETS_PATH) + std::string("faq_page_") +
                                std::to_string(iSelPage) + std::string(".png");

    pFile = TgBot::InputFile::fromFile(asset_file, "image/png");
    fprintf(fp, "BaseBot %ld: FAQs::getMedia }\n", time(0)); fflush(fp);
    return pFile;
}



/*
 *
 *
 *
 * void FAQs::create_faq_page(std::string file_name, FILE *fp) {
    fprintf(fp, "BaseBot %ld: FAQs::create_faq_page {\n", time(0)); fflush(fp);

    pngwriter cart_table(320, 320, 1.0, file_name.c_str());
    char toggle = 0;
    int iLoop   = 0, iIndex = 0;

    //  Headers

    std::string strFontFile = std::string(BOT_ROOT_PATH) + std::string(BOT_FONT_PATH) + std::string(BOT_FONT_FILE_BOLD);
    cart_table.filledsquare(0, 300, 320, 320, 0.0, 0.5, 0.5);

    std::string strFAQPg  = std::string(FAQ_PAGE_PREFIX) + std::string(" ")+ std::to_string(iSelPage);
    cart_table.plot_text_utf8((char *)strFontFile.c_str(), 12,  30+5, 305, 0.0, (char *)strFAQPg.c_str(), 1.0, 1.0, 1.0);

    iIndex  = (iSelPage - 1) * MAX_ITEMS_PER_PAGE;
    for(iLoop = 300; iLoop > 0; iLoop -= 30, toggle = (1-toggle) ) {
        //if(0 == toggle) cart_table.filledsquare(0, iLoop, 320, iLoop-30, 0.9, 0.9, 0.9);

        if((iNoOfLines > iIndex) && (iIndex < (iSelPage * MAX_ITEMS_PER_PAGE)) && faqLines[iIndex].compare("\n"))
            cart_table.plot_text_utf8((char *)strFontFile.c_str(), 10, 5, iLoop-20, 0.0, (char *)faqLines[iIndex].c_str(), 0, 0, 0);

        iIndex++;
    }
    cart_table.close();

    fprintf(fp, "BaseBot %ld: FAQs::create_faq_page }\n", time(0)); fflush(fp);
}

#define MAX_SINGLE_LINE_WIDTH_1 (50)
void FAQs::split_faqs(std::string strFAQLine) {
    std::string strsub, strLine;
    std::string::size_type curPos;
    unsigned int len = 0;

    strFAQLine += ".";  //  To get the final word too
    while(std::string::npos != (curPos  = strFAQLine.find_first_of("\n\t ,."))) {
        if(curPos) {
            len += (curPos + 1);  //  now len is size of string after concatenation
            if(MAX_SINGLE_LINE_WIDTH_1 <= len) {faqLines.push_back(strLine); strLine.clear();}

            strsub = strFAQLine.substr(0, curPos) + " ";
            strLine += strsub;

            if(MAX_SINGLE_LINE_WIDTH_1 <= len) len = strsub.length();
        }
        strFAQLine  = strFAQLine.substr(curPos + 1);
    }
    if(!strLine.empty()) faqLines.push_back(strLine);
}

    strFAQFile  = std::string(BOT_ROOT_PATH) + std::string(BOT_FAQ_FILE);
    FILE *fpFaq = fopen(strFAQFile.c_str(), "r");
    if(fpFaq) {
        char pBuffer[MAX_FAQ_FILE_SZ];
        int iLen        = fread(pBuffer, 1, MAX_FAQ_FILE_SZ-1, fpFaq);
        pBuffer[iLen]   = '\0';
        fclose(fpFaq);
        strFAQs         = std::string(pBuffer) + "\n";

        while(std::string::npos != (curPos  = strFAQs.find_first_of("\n"))) {
            if(curPos) {
                split_faqs(strFAQs.substr(0, curPos));
            } else faqLines.push_back("\n");
            strFAQs  = strFAQs.substr(curPos + 1);
        }
        for(auto &line : faqLines)
        fprintf(fp, "%s\n", line.c_str()); fflush(fp);
    }
    iNoOfLines  = faqLines.size();
 * */
