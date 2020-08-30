/*sgn
 * SGNParser.cpp
 *
 *  Created on: 28-Jul-2020
 *      Author: tstone10
 */
#include <iostream>
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>
#include <ProductList.h>
#include <memory>
#include <map>

#include "BaseButton.h"
#include "Constants.h"
#include "SGNParser.h"

void SGNParser::initDepot() {
    if(isDepotIntialized) return;
    depot.insert({"tiffin", item::breakfast});
    depot.insert({"tiffins", item::breakfast});
    depot.insert({"tiffen", item::breakfast});
    depot.insert({"tiffens", item::breakfast});
    depot.insert({"breakfast", item::breakfast});
    depot.insert({"breakfasts", item::breakfast});
    depot.insert({"bf", item::breakfast});
    depot.insert({"b/f", item::breakfast});
    depot.insert({"b/fs", item::breakfast});
    depot.insert({"bfs", item::breakfast});
    depot.insert({"idly", item::breakfast});
    depot.insert({"pongal", item::breakfast});
    depot.insert({"upma", item::breakfast});
    depot.insert({"poori", item::breakfast});
    depot.insert({"adai", item::breakfast});
    depot.insert({"dosa", item::breakfast});
    depot.insert({"dosai", item::breakfast});
    depot.insert({"chappati", item::breakfast});
    depot.insert({"chappathi", item::breakfast});
    depot.insert({"chapathi", item::breakfast});
    depot.insert({"chapati", item::breakfast});
    depot.insert({"bisibelebath", item::breakfast});
    depot.insert({"bisibelabath", item::breakfast});
    depot.insert({"bisibela", item::breakfast});    // ignore bath
    depot.insert({"bisibele", item::breakfast});
    depot.insert({"bisi", item::breakfast});        // ignore belebath or bele or bela or bath
    depot.insert({"lunch", item::lunch});           // ignore full
    depot.insert({"fulllunch", item::lunch});
    depot.insert({"fullunch", item::lunch});
    depot.insert({"meal", item::lunch});
    depot.insert({"meals", item::lunch});
    depot.insert({"sambar", item::sambar});
    depot.insert({"sambaar", item::sambar});
    depot.insert({"saambar", item::sambar});
    depot.insert({"sambhar", item::sambar});
    depot.insert({"saambaar", item::sambar});
    depot.insert({"morekuzhambu", item::sambar});
    depot.insert({"morkuzhambhu", item::sambar});
    depot.insert({"morkuzhambu", item::sambar});
    depot.insert({"mrkuzhambu", item::sambar});
    depot.insert({"kuzhambu", item::sambar});
    depot.insert({"vathakuzhambu", item::sambar});
    depot.insert({"vathalkuzhambu", item::sambar});
    depot.insert({"vathakuzhambu", item::sambar});
    depot.insert({"rasam", item::rasam});
    depot.insert({"curry", item::curry});
    depot.insert({"usili", item::curry});
    depot.insert({"kootu", item::kootu});
    depot.insert({"koottu", item::kootu});
    depot.insert({"aviyal", item::kootu});
    depot.insert({"avial", item::kootu});
    depot.insert({"rice", item::rice});
    depot.insert({"10", item::quantity});
    depot.insert({"2", item::quantity});
    depot.insert({"3", item::quantity});
    depot.insert({"4", item::quantity});
    depot.insert({"5", item::quantity});
    depot.insert({"6", item::quantity});
    depot.insert({"7", item::quantity});
    depot.insert({"8", item::quantity});
    depot.insert({"9", item::quantity});
    depot.insert({"1", item::quantity});
    depot.insert({"one", item::quantity});
    depot.insert({"two", item::quantity});
    depot.insert({"three", item::quantity});
    depot.insert({"four", item::quantity});
    depot.insert({"five", item::quantity});
    depot.insert({"six", item::quantity});
    depot.insert({"seven", item::quantity});
    depot.insert({"eight", item::quantity});
    depot.insert({"nine", item::quantity});
    depot.insert({"ten", item::quantity});
    depot.insert({"i", item::quantity});
    depot.insert({"l", item::quantity});

    isDepotIntialized = true;
}

void SGNParser::initQty() {
    if(isQtyInitialized) return;
    qty.insert({"10", 10});
    qty.insert({"2", 2});
    qty.insert({"3", 3});
    qty.insert({"4", 4});
    qty.insert({"5", 5});
    qty.insert({"6", 6});
    qty.insert({"7", 7});
    qty.insert({"8", 8});
    qty.insert({"9", 9});
    qty.insert({"1", 1});
    qty.insert({"i", 1});
    qty.insert({"l", 1});
    qty.insert({"one", 1});
    qty.insert({"two", 2});
    qty.insert({"three", 3});
    qty.insert({"four", 4});
    qty.insert({"five", 5});
    qty.insert({"six", 6});
    qty.insert({"seven", 7});
    qty.insert({"eight", 8});
    qty.insert({"nine", 9});
    qty.insert({"ten", 10});

    isQtyInitialized =  true;
}

std::string SGNParser::getItem(item itm, FILE *fp) {
    switch(itm) {
        case item::none: return "none";
        case item::breakfast: return "breakfast";
        case item::lunch: return "lunch";
        case item::curry: return "curry";
        case item::kootu: return "kootu";
        case item::sambar: return "sambar";
        case item::rasam: return "rasam";
        case item::rice: return "rice";
    }
    return "none";
}

int SGNParser::getQuantity(std::string strQty, FILE *fp) {
    std::map<std::string, int>::iterator itr;
    for(itr = qty.begin(); itr != qty.end(); itr++) {
        if(!strQty.compare(itr->first)) return itr->second;
    }
    return -1;
}

Product::Ptr SGNParser::getProduct(item itm, FILE *fp) {
    std::string strPrfix;
    switch(itm) {
        case item::none: return 0;
        case item::breakfast:
            strPrfix = "TF";
            break;
        case item::lunch:
            strPrfix = "LN-03";
            break;
        case item::curry:
            strPrfix = "CR";
            break;
        case item::kootu:
            strPrfix = "KT";
            break;
        case item::sambar:
            strPrfix = "SB";
            break;
        case item::rasam:
            strPrfix = "RS";
            break;
        case item::rice:
            strPrfix = "RC";
            break;
    }
    std::vector<Product::Ptr> products = getDBHandle()->getAllActiveProducts(fp);
    for(auto &prod : products) if(std::string::npos != prod->m_Code.find(strPrfix)) return prod;
    return nullptr;
}

std::map<std::string, std::shared_ptr<BaseButton>>::const_iterator
    SGNParser::parse(std::string strOrder, std::map<std::string, std::shared_ptr<BaseButton>>& listKBBtns, unsigned int iChatId, FILE *fp) {
    std::vector<std::string> tokens;
    std::string token, strNum;
    int iNum = -1;
    std::string delimiter = " ,\t\n\r()-.";
    int pos1 = 0, iLoop = 0, iQty = 0, pos2 = 0;
    std::map<std::string, std::shared_ptr<BaseButton>>::const_iterator retItr;

    //  Convert all input to lowercase
    strOrder = strOrder + std::string(".");
    std::for_each(strOrder.begin(), strOrder.end(), [](char & c){if(std::isalpha(c)) c = ::tolower(c);});

    //  Parse all tokens
    while ((pos1 = strOrder.find_first_of(delimiter)) != std::string::npos) {
        if(0 == pos1) {if(1 == strOrder.length()) break; strOrder    = strOrder.substr(pos1+1);continue;}
        token = strOrder.substr(0, pos1);
        //std::cout << "token " << token << std::endl;

        //  is it an alphanumeric token like blr21 2sambar or 1rice
        if(std::string::npos != token.find_first_of("0123456789") && std::string::npos != token.find_first_not_of("0123456789")) {
            iNum = -1; std::stringstream ss;

            //  Where is the number starting from
            pos2 = token.find_first_of("0123456789");
            ss.str(token.substr(pos2));
            ss >> iNum;
            if(0 < iNum) {
                strNum  = std::to_string(iNum);
                tokens.push_back(strNum);
                //std::cout << "a number " << strNum << " found in " << token << "\nInserting " << strNum;

                //  The number shall either start at first or last
                if(0 == pos2) token = token.substr(strNum.length());
                else token = token.substr(0, token.length() - strNum.length());
                //std::cout << " and " << token << std::endl;
            }
        }

        tokens.push_back(token);
        strOrder    = strOrder.substr(pos1);
    }

    std::pair<item, int> ordr;
    std::map<std::string, item>::iterator itrDp;

    ordr    = std::make_pair(item::none, -1);
    retItr  = listKBBtns.end();
    for(iLoop = 0; iLoop < tokens.size(); iLoop++) {
        for(itrDp = depot.begin(); itrDp != depot.end(); itrDp++) {
            if(!tokens[iLoop].compare(itrDp->first)) {
//                std::cout << "Found " << itrDp->first << std::endl;
                if(item::quantity == itrDp->second) { ordr.second = getQuantity(itrDp->first, fp);}
                else {ordr.first    = itrDp->second;}

                if(item::none != ordr.first && -1 != ordr.second) {
//                    std::cout << getItem(ordr.first, fp) << " - " << ordr.second << std::endl;
                    fprintf(fp, "Parsed %s - %d\n", getItem(ordr.first, fp).c_str(), ordr.second); fflush(fp);

                    Product::Ptr pProd = getProduct(ordr.first, fp);
                    if(pProd) {
                        fprintf(fp, "Inserting product %s\n", pProd->m_Name.c_str()); fflush(fp);
                        getDBHandle()->addProductToCart(pProd->m_ProductId, ordr.second, pProd->m_Price, iChatId, fp);
                    }
                    ordr.first = item::none; ordr.second = -1;
                    retItr = listKBBtns.find(STR_BTN_VIEW_CART);
                }
            }
        }
    }
    return retItr;
}
