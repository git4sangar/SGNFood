/*sgn
 * ViewCart.h
 *
 *  Created on: 25-May-2020
 *      Author: tstone10
 */

#ifndef VIEWCART_H_
#define VIEWCART_H_

#include <iostream>
#include <vector>

#include "Constants.h"
#include "DBInterface.h"
#include "BaseButton.h"

#define CART_PAGE     "Cart Page"

class ViewCart : public BaseButton, public std::enable_shared_from_this<ViewCart> {
    std::vector<Cart::Ptr> cartItems;
    std::vector<Product::Ptr> products;
    std::string asset_file, strAddress;
    unsigned int iSelPage, iNoOfItems;

public:
    ViewCart(DBInterface::Ptr hDB) : BaseButton(hDB), iSelPage(0), iNoOfItems(0) {}
    virtual ~ViewCart() {}

    //  This object is not created on every invocation. So clear it before using.
    void init(TgBot::Message::Ptr pMsg, FILE *fp) {
        cartItems.clear(); products.clear(); asset_file.clear(); strAddress.clear();
        iSelPage = 0; iNoOfItems = 0; STR_MSG_DEFF_RELEASE  = "\"Add\" => increase quantity\n\"Rmv\" => decreases quantity.";
    }
    std::string getMsg() { return STR_MSG_DEFF_RELEASE;}
    TgBot::GenericReply::Ptr prepareMenu(std::map<std::string, std::shared_ptr<BaseButton>>& listKBBtns, TgBot::Message::Ptr pMsg, FILE *fp);
    void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
    std::shared_ptr<BaseButton> getSharedPtr() {return shared_from_this();}
    TgBot::InputFile::Ptr getMedia(TgBot::Message::Ptr pMsg, FILE *fp);

    void create_cart_table(std::string file_name, FILE *fp);
    unsigned int findAssociatedNumber(std::string strMsg, std::string strFind, int iMaxItems = MAX_NO_OF_PAGES);

    static std::string STR_MSG_DEFF_RELEASE;
};



#endif /* VIEWCART_H_ */
