/*sgn
 * SingleOrder.cpp
 *
 *  Created on: 07-Jun-2020
 *      Author: tstone10
 */


#include <iostream>
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>
#include <memory>

#include "pngwriter.h"

#include "BaseButton.h"
#include "Constants.h"
#include "Checkout.h"
#include "SingleOrder.h"

std::string SingleOrder::STR_MSG_DEFF_RELEASE  = "Your Cart";

void SingleOrder::init(TgBot::Message::Ptr pMsg, FILE *fp) {
   orderItems.clear(); products.clear(); pOrder = nullptr; asset_file.clear();
   pageName.clear(); iPgNo = 0; isPrint = false;
   iNoOfItems = 0; STR_MSG_DEFF_RELEASE  = "Your Order";
   notifyMsgs.clear();

   mapCmdToInt[STR_BTN_CNFM_ORDER]  = INT_CNFM_ORDER;
   mapCmdToInt[STR_BTN_CNCL_ORDER]  = INT_CNCL_ORDER;
   mapCmdToInt[SRT_BTN_DLVR_ORDER]  = INT_DLVR_ORDER;
}

std::vector<std::string> SingleOrder::split_address(std::string strAddress) {
    std::vector<std::string> words;
    std::string strsub, strLine;
    std::string::size_type curPos;
    unsigned int len = 0;

    strAddress += ".";  //  To get the final word too
    while(std::string::npos != (curPos  = strAddress.find_first_of("\n\t ,."))) {
        if(curPos) {
            len += (curPos+1);  //  now len is size of string after concatenation
            if(MAX_SINGLE_LINE_WIDTH <= len) {words.push_back(strLine); strLine.clear();}

            strsub = strAddress.substr(0, curPos) + " ";
            strLine += strsub;

            if(MAX_SINGLE_LINE_WIDTH <= len) len = strsub.length();
        }
        strAddress  = strAddress.substr(curPos + 1);
    }
    if(!strLine.empty()) words.push_back(strLine);

    return words;
}

int SingleOrder::addAddressToPNG(std::shared_ptr<pngwriter> pPNGWriter, int iPgLn, std::string strAddress, int iOrderNo, std::string strPayGW) {
    if(!pPNGWriter) return 0;

    std::vector<std::string> lines  = split_address(strAddress);
    std::stringstream ss; ss << "Order No: " << iOrderNo << ", Payment: " << strPayGW;
    lines.insert(lines.begin(), ss.str());

    std::string strFontFile = std::string(BOT_ROOT_PATH) + std::string(BOT_FONT_PATH) + std::string(BOT_FONT_FILE_BOLD);
    int yAxis = iPgLn - 15, iLoop = 0;
    for(iLoop = 0; iLoop < lines.size() && iLoop < MAX_ADDRESS_LINES; iLoop++) {
        pPNGWriter->plot_text_utf8((char *)strFontFile.c_str(), 10, 5, yAxis, 0.0, (char *)lines[iLoop].c_str(), 0, 0, 0);
        yAxis   -= 30;
    }

    //  We'v decremented it. So incrementing here to compensate.
    return (yAxis+15);
}

int SingleOrder::create_order_items_table(std::shared_ptr<pngwriter> pPNGWriter, int yAxis, int iIndex, int iCurPg, int iMaxPg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: SingleOrder::create_order_items_table {\n", time(0)); fflush(fp);
    char toggle = 1;

    std::string strFontFile = std::string(BOT_ROOT_PATH) + std::string(BOT_FONT_PATH) + std::string(BOT_FONT_FILE_BOLD);

    //  Headers
    std::string strHeader   = (0 == pageName.compare(STR_BTN_YOUR_ORDERS)) ? (std::string("Order No: ") + std::to_string(iOrderNo)) : pageName;
    pPNGWriter->filledsquare(0, (yAxis-20), 320, yAxis, 0.0, 0.5, 0.5);
    pPNGWriter->plot_text_utf8((char *)strFontFile.c_str(), 12,     5, (yAxis-15), 0.0, (char *)"SN", 1.0, 1.0, 1.0);
    pPNGWriter->plot_text_utf8((char *)strFontFile.c_str(), 12,  35+5, (yAxis-15), 0.0, (char *)strHeader.c_str(), 1.0, 1.0, 1.0);
    pPNGWriter->plot_text_utf8((char *)strFontFile.c_str(), 12, 235+5, (yAxis-15), 0.0, (char *)"Qty", 1.0, 1.0, 1.0);
    pPNGWriter->plot_text_utf8((char *)strFontFile.c_str(), 12, 275+3, (yAxis-15), 0.0, (char *)"Price", 1.0, 1.0, 1.0);

    //  Highlight alternate rows
    int iLoop = 0, iQty = 0, iTotal = 0, iSum = 0;
    for(iLoop = (yAxis-20); iLoop > 30; iLoop -= 30, toggle = (1-toggle)) {
        if(0 == toggle) pPNGWriter->filledsquare(0, iLoop, 320, iLoop-30, 0.9, 0.9, 0.9);

        if(iNoOfItems > iIndex) {
            std::string strCount    = std::to_string(iIndex+1);
            std::string strProdPack = products[iIndex]->m_Name + std::string(" - ") + products[iIndex]->m_Pack;
            std::string strQty      = std::to_string(orderItems[iIndex]->m_Qnty);
            iSum                    = orderItems[iIndex]->m_Qnty * orderItems[iIndex]->m_Price;
            std::string strPrice    = std::to_string(iSum);

            pPNGWriter->plot_text_utf8((char *)strFontFile.c_str(), 10,     5, iLoop-20, 0.0, (char *)strCount.c_str(), 0, 0, 0);
            pPNGWriter->plot_text_utf8((char *)strFontFile.c_str(), 10,  35+5, iLoop-20, 0.0, (char *)strProdPack.c_str(), 0, 0, 0);
            pPNGWriter->plot_text_utf8((char *)strFontFile.c_str(), 10, 235+5, iLoop-20, 0.0, (char *)strQty.c_str(), 0, 0, 0);
            pPNGWriter->plot_text_utf8((char *)strFontFile.c_str(), 10, 275+5, iLoop-20, 0.0, (char *)strPrice.c_str(), 0, 0, 0);
            iQty    += orderItems[iIndex]->m_Qnty;
            iTotal += iSum; iIndex++;
        }
    }

    //  Vertical lines
    pPNGWriter->line( 35, yAxis,  35, 30, 0, 0, 0);
    pPNGWriter->line(235, yAxis, 235, 30, 0, 0, 0);
    pPNGWriter->line(275, yAxis, 275, 30, 0, 0, 0);

    //  Total amount and all quantiy
    if(iNoOfItems <= iIndex) {
        for(iSum = iQty = iLoop = 0; iLoop < orderItems.size(); iLoop ++) {
            iQty    += orderItems[iLoop]->m_Qnty;
            iSum    += (orderItems[iLoop]->m_Qnty * products[iLoop]->m_Price);
        }
        pPNGWriter->plot_text_utf8((char *)strFontFile.c_str(), 12,  70+5, 5, 0.0, (char *)"Total ₹: ", 0, 0, 0);
        pPNGWriter->plot_text_utf8((char *)strFontFile.c_str(), 10, 140+5, 5, 0.0, (char *)std::to_string(iSum).c_str(), 0, 0, 0);
        pPNGWriter->plot_text_utf8((char *)strFontFile.c_str(), 10, 235+5, 5, 0.0, (char *)std::to_string(iQty).c_str(), 0, 0, 0);
    } else {
        std::stringstream ss; ss << "Page - " << iCurPg << " of " << iMaxPg;
        pPNGWriter->plot_text_utf8((char *)strFontFile.c_str(), 10,   70, 10, 0.0, (char *)ss.str().c_str(), 0, 0, 0);
    }

    pPNGWriter->close();
    fprintf(fp, "BaseBot %ld: SingleOrder::create_order_items_table }\n", time(0)); fflush(fp);
    return iIndex;
}

TgBot::GenericReply::Ptr SingleOrder::prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& lstBaseBtns, TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: SingleOrder::prepareMenu {\n", time(0)); fflush(fp);

    if(0 == iNoOfItems) {
        STR_MSG_DEFF_RELEASE  = "No more orders";
        return nullptr;
    }

    unsigned int iPrev, iNext, iLoop, iRowIndex = 0, iItemsPerRow = 0, iItems, iRowsReqd;
    std::vector<TgBot::KeyboardButton::Ptr> row[MAX_BUTTON_ROWS];
    std::string strText;
    TgBot::ReplyKeyboardMarkup::Ptr pMainMenu;

    std::map<unsigned int, UserContext>::const_iterator itrCntxt;
    std::map<std::string, std::shared_ptr<BaseButton> >::const_iterator itrBtn;
    std::string strChatId       = std::to_string(pMsg->chat->id);

    iRowIndex = 0;
    if(USER_CTXT_NEW_ORDER == usrCtxt) {
        strText =   std::string(STR_BTN_CNFM_ORDER) + std::string(" ") + std::to_string(iOrderNo);
        createKBBtn(strText, row[iRowIndex], lstBaseBtns);
        strText =   std::string(STR_BTN_CNCL_ORDER) + std::string(" ") + std::to_string(iOrderNo);
        createKBBtn(strText, row[iRowIndex], lstBaseBtns);
        iRowIndex++;

    } else if(USER_CTXT_CNF_ORDER == usrCtxt) {
        strText =   std::string(SRT_BTN_DLVR_ORDER) + std::string(" ") + std::to_string(iOrderNo);
        createKBBtn(strText, row[iRowIndex], lstBaseBtns);
        iRowIndex++;
    }

    //  Easy for the user to navigate back to the orders page if there are more order-pages
    if(0 < iPgNo) {
        createKBBtn(pageName + std::string(" page ") + std::to_string(iPgNo), row[iRowIndex], lstBaseBtns, lstBaseBtns[STR_BTN_ORDERS]);
        iRowIndex++;
    }

    //  Populate the next available row
    if(USER_CTXT_YOUR_ORDER == usrCtxt) {
        createKBBtn(STR_BTN_YOUR_ORDERS, row[iRowIndex], lstBaseBtns);
        createKBBtn(STR_BTN_MAINMENU, row[iRowIndex], lstBaseBtns);
        createKBBtn(STR_BTN_VIEW_CART, row[iRowIndex], lstBaseBtns);
        iRowIndex++;
    } else {
        createKBBtn(STR_BTN_NEW_ORDERS, row[iRowIndex], lstBaseBtns);
        createKBBtn(STR_BTN_MAINMENU, row[iRowIndex], lstBaseBtns);
        createKBBtn(STR_BTN_ADMIN_PG, row[iRowIndex], lstBaseBtns);
        iRowIndex++;
    }

    //  Add all the rows to main menu
    pMainMenu   = std::make_shared<TgBot::ReplyKeyboardMarkup>();
    for(iLoop = 0; iLoop < iRowIndex; iLoop++) {
       pMainMenu->keyboard.push_back(row[iLoop]);
    }

    fprintf(fp, "BaseBot %ld: Finishing SingleOrder::prepareMenu }\n", time(0)); fflush(fp);
    return pMainMenu;
}

void SingleOrder::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: SingleOrder onClick pMessage %s {\n", time(0), pMsg->text.c_str()); fflush(fp);

    unsigned int iLoop = 0, iOrdrsSz = 0;
    std::vector<POrder::Ptr> orders;
    Product::Ptr pProduct;
    std::string strOrderNo, strCmd, strSNo;
    bool isAdmin = false;
    std::map<unsigned int, UserContext>::const_iterator itrCntxt;
    User::Ptr pUser = nullptr;

    //  Is this user an Admin?
    std::vector<unsigned int>::const_iterator itr;
    for(itr = adminChatIds.begin(); itr != adminChatIds.end(); itr++) if(*itr == pMsg->chat->id) { isAdmin = true; break; }
    fprintf(fp, "BaseBot %ld: SingleOrder onClick, isAdmin: %d\n", time(0), isAdmin); fflush(fp);


    //  Get the Order No. Possible values: "1087", "Confirm Order 1003", "Cancel Order 1342", "Edit Order 3214", "Print Order 4923", "Deliver Order 3387"
    std::string::size_type pos  = pMsg->text.find_first_of("0123456789");
    try {
        if(0 < pos) strCmd = pMsg->text.substr(0, pos-1);
        strOrderNo  = pMsg->text.substr(pos);
        iOrderNo    = std::stoi(strOrderNo);
    } catch(std::exception &e) {
        strCmd.clear();
        iOrderNo = 0;
    }

    //  Confirm, Print, Deliver or Cancel order accordingly
    if(!strCmd.empty() && 0 < iOrderNo) {
        pOrder  = getDBHandle()->getOrderForOrderNo(iOrderNo, fp);
        pUser   = getDBHandle()->getUserForUserId(pOrder->m_UserId, fp);
        switch(mapCmdToInt[strCmd]) {
            case INT_CNFM_ORDER:
                getDBHandle()->updateOrderStatus(iOrderNo, CartStatus::READY_FOR_DELIVERY, OrderType::PORDER, fp);
                notifyMsgs[pUser->m_ChatId] = std::string("Your order no: ") + std::to_string(iOrderNo) + std::string(" is Confirmed.");
                fprintf(fp, "BaseBot %ld: SingleOrder onClick, Confirming Order: %d\n", time(0), iOrderNo); fflush(fp);

                orders = getDBHandle()->getOrderByStatus(CartStatus::PAYMENT_PENDING, OrderType::PORDER, fp);
                iOrderNo = (orders.empty()) ? 0 : orders[0]->m_OrderNo;
                fprintf(fp, "BaseBot %ld: SingleOrder onClick, Picking next New Order: %d\n", time(0), iOrderNo); fflush(fp);
                break;

            case INT_CNCL_ORDER:
                getDBHandle()->updateOrderStatus(iOrderNo, CartStatus::CANCELLED, OrderType::PORDER, fp);
                notifyMsgs[pUser->m_ChatId] = std::string("Your order no: ") + std::to_string(iOrderNo) + std::string(" is Cancelled & Wallet updated. Please call the merchant for details");
                fprintf(fp, "BaseBot %ld: SingleOrder onClick, Canceling Order: %d\n", time(0), iOrderNo); fflush(fp);

                orders = getDBHandle()->getOrderByStatus(CartStatus::PAYMENT_PENDING, OrderType::PORDER, fp);
                iOrderNo = (orders.empty()) ? 0 : orders[0]->m_OrderNo;
                fprintf(fp, "BaseBot %ld: SingleOrder onClick, Picking next New Order: %d\n", time(0), iOrderNo); fflush(fp);
                break;

            case INT_DLVR_ORDER:
                getDBHandle()->updateOrderStatus(iOrderNo, CartStatus::DELIVERED, OrderType::PORDER, fp);
                notifyMsgs[pUser->m_ChatId] = std::string("Your order no: ") + std::to_string(iOrderNo) + std::string(" is Delivered.");
                fprintf(fp, "BaseBot %ld: SingleOrder onClick, Delivered Order: %d\n", time(0), iOrderNo); fflush(fp);

                orders = getDBHandle()->getOrderByStatus(CartStatus::READY_FOR_DELIVERY, OrderType::PORDER, fp);
                iOrderNo = (orders.empty()) ? 0 : orders[0]->m_OrderNo;
                fprintf(fp, "BaseBot %ld: SingleOrder onClick, Picking next Confirmed Order: %d\n", time(0), iOrderNo); fflush(fp);
                break;
        }
    }

    //  At this stage there shall be a valid order number
    if(0 == iOrderNo) return;
    pOrder  = getDBHandle()->getOrderForOrderNo(iOrderNo, fp);

    if(!isAdmin) {
        usrCtxt     = USER_CTXT_YOUR_ORDER;
        pageName    = STR_BTN_YOUR_ORDERS;
        pUser       = getDBHandle()->getUserForChatId(pMsg->chat->id, fp);
        if(orders.empty()) orders = getDBHandle()->getOrdersByUser(pUser->m_UserId, fp);
    }
    else if(pOrder && CartStatus::PAYMENT_PENDING == pOrder->m_Status) {
        usrCtxt     = USER_CTXT_NEW_ORDER;
        pageName    = STR_BTN_NEW_ORDERS;
        if(orders.empty()) orders = getDBHandle()->getOrderByStatus(CartStatus::PAYMENT_PENDING, OrderType::PORDER, fp);
    }
    else if(pOrder && CartStatus::READY_FOR_DELIVERY == pOrder->m_Status) {
        usrCtxt     = USER_CTXT_CNF_ORDER;
        pageName    = STR_BTN_CNF_ORDERS;
        if(orders.empty()) orders = getDBHandle()->getOrderByStatus(CartStatus::READY_FOR_DELIVERY, OrderType::PORDER, fp);
    }
    else if(pOrder && CartStatus::DELIVERED == pOrder->m_Status) {
        usrCtxt     = USER_CTXT_DLVRD_ORDER;
        pageName    = STR_BTN_DLVRD_ORDERS;
        if(orders.empty()) orders = getDBHandle()->getOrderByStatus(CartStatus::DELIVERED, OrderType::PORDER, fp);
    }
    else if(pOrder && CartStatus::CANCELLED == pOrder->m_Status) {
        usrCtxt     = USER_CTXT_CNCLD_ORDER;
        pageName    = STR_BTN_CNCLD_ORDERS;
        if(orders.empty()) orders = getDBHandle()->getOrderByStatus(CartStatus::CANCELLED, OrderType::PORDER, fp);
    }

    fprintf(fp, "BaseBot %ld: SingleOrder onClick, pageName: %s\n", time(0), pageName.c_str()); fflush(fp);

    //  Find which page this order no falls
    iOrdrsSz = orders.size();
    for(iLoop = 0; iLoop < iOrdrsSz; iLoop++) if(iOrderNo == orders[iLoop]->m_OrderNo) break;
    iLoop++;
    if(iLoop <= iOrdrsSz) iPgNo = (iLoop / MAX_ITEMS_PER_PAGE) + (0 != (iLoop % MAX_ITEMS_PER_PAGE));
    fprintf(fp, "BaseBot %ld: SingleOrder onClick, this order falls in page: %d\n", time(0), iPgNo); fflush(fp);

    //  Get all the cart items for this order
    orderItems  = getDBHandle()->getCartItemsForOrderNo(iOrderNo, fp);
    iNoOfItems  = orderItems.size();
    if(0 == iNoOfItems) return;

    //  Populate all the products in the cart
    for(iLoop = 0; iLoop < iNoOfItems; iLoop++) {
        pProduct    = getDBHandle()->getProductById(orderItems[iLoop]->m_ProductId, fp);
        products.push_back(pProduct);
    }

    fprintf(fp, "BaseBot %ld: SingleOrder onClick pMessage }\n", time(0)); fflush(fp);
}

std::vector<TgBot::InputFile::Ptr> SingleOrder::getMedias(TgBot::Message::Ptr pMsg, FILE *fp) {
    fprintf(fp, "BaseBot %ld: SingleOrder::getMedia {\n", time(0)); fflush(fp);
    std::vector<TgBot::InputFile::Ptr> pFiles;
    int iNoOfPages, iLoop, iIndex, yAxis, iPgLen;

    if(0 < iNoOfItems) {
        iNoOfPages  = ((iNoOfItems+MAX_ADDRESS_LINES) / (MAX_ITEMS_PER_PAGE-1)) + (0 != ((iNoOfItems+MAX_ADDRESS_LINES) % (MAX_ITEMS_PER_PAGE-1)));
        yAxis       = 320;
        if(isPrint) {
            fprintf(fp, "BaseBot %ld: SingleOrder getMedias printing\n", time(0)); fflush(fp);
            iNoOfPages  = 1;
            yAxis       = ((iNoOfItems + MAX_ADDRESS_LINES + 1) * 30) + 20;
        }

        //  First add the address to PNG
        asset_file  = std::string(BOT_ROOT_PATH) + std::string(BOT_ASSETS_PATH) + std::string("order_items_page_1.png");
        std::shared_ptr<pngwriter> pPNGWriter   = std::make_shared<pngwriter>(320, yAxis, 1.0, asset_file.c_str());
        yAxis   = addAddressToPNG(pPNGWriter, yAxis, pOrder->m_Address, iOrderNo, pOrder->m_PayGW);

        //  Now iteratively append all the items at the end of address
        for(iIndex  = 0, iLoop = 1; iLoop <= iNoOfPages; iLoop++, yAxis = 320) {
            //  On first iteration, append the items in rest of the space
            iIndex  = create_order_items_table(pPNGWriter, yAxis, iIndex, iLoop, iNoOfPages, fp);
            if(isFileExists(asset_file))
                pFiles.push_back(TgBot::InputFile::fromFile(asset_file, "image/png"));
            fprintf(fp, "BaseBot %ld: SingleOrder getMedias adding PNG %d\n", time(0), (iLoop)); fflush(fp);

            //  Create new one for next iteration
            asset_file  = std::string(BOT_ROOT_PATH) + std::string(BOT_ASSETS_PATH) +
                            std::string("order_items_page_") + std::to_string(iLoop+1) + std::string(".png");
            pPNGWriter   = std::make_shared<pngwriter>(320, 320, 1.0, asset_file.c_str());
        }
    }
    fprintf(fp, "BaseBot %ld: SingleOrder::getMedia }\n", time(0)); fflush(fp);
    return pFiles;
}
