/*sgn
 * DBInerface.cpp
 *
 *  Created on: 21-May-2020
 *      Author: tstone10
 */

#include <iostream>
#include <sstream>
#include <sstream>
#include <time.h>

#include <SQLiteCpp/SQLiteCpp.h>
#include <tgbot/tgbot.h>

#include "Constants.h"
#include "DBInterface.h"

std::string User::USER_ID       = "user_id";
std::string User::USER_NAME     = "name";
std::string User::USER_CHAT_ID  = "chat_id";
std::string User::USER_MOBILE   = "mobile";
std::string User::USER_ORDER_NO = "order_no";
std::string User::USER_ADDRESS  = "address";
std::string User::USER_WBALANCE = "wbalance";
std::string User::USER_TRANSAC_NO = "transac_no";

std::string Viewers::VIEWER_ID      = "viewer_id";
std::string Viewers::VIEWER_USER_ID = "user_id";
std::string Viewers::VIEWER_DATE    = "date";
std::string Viewers::VIEWER_TIME    = "time";

std::string Category::CATEGORY_ID       = "category_id";
std::string Category::CATEGORY_NAME     = "name";
std::string Category::CATEGORY_CODE     = "code";
std::string Category::CATEGORY_PACKAGE  = "package";

std::string Product::PRODUCT_ID         = "product_id";
std::string Product::PRODUCT_CODE       = "code";
std::string Product::PRODUCT_NAME       = "name";
std::string Product::PRODUCT_PIC_FILE   = "pic_file";
std::string Product::PRODUCT_PRICE      = "price";
std::string Product::PRODUCT_PACK       = "pack";
std::string Product::PRODUCT_DATE       = "date";
std::string Product::PRODUCT_DESC       = "description";

std::string POrder::PORDER_ID       = "order_id";
std::string POrder::PORDER_USER_NAME= "name";
std::string POrder::PORDER_NO       = "order_no";
std::string POrder::PORDER_USER_ID  = "user_id";
std::string POrder::PORDER_ORDR_TM  = "ordered_at";
std::string POrder::PORDER_DLVR_TM  = "dlvrd_at";
std::string POrder::PORDER_PAY_GW   = "payment_gw";
std::string POrder::PORDER_AMOUNT   = "otp";
std::string POrder::PORDER_STATUS   = "status_id";
std::string POrder::PORDER_ADDRESS  = "address";
std::string POrder::PORDER_WBALANCE = "wbalance";

std::string Cart::CART_ID           = "cart_id";
std::string Cart::CART_PRODUCT_ID   = "product_id";
std::string Cart::CART_PRODUCT_PRICE= "price";
std::string Cart::CART_USER_ID      = "user_id";
std::string Cart::CART_STATUS       = "status_id";
std::string Cart::CART_QNTY         = "quantity";
std::string Cart::CART_ORDER_NO     = "order_no";

std::string Shipping::SHIPPING_ID       = "shipment_id";
std::string Shipping::SHIPPING_USER_ID  = "user_id";
std::string Shipping::SHIPPING_ADDRESS  = "address";

std::string Stars::STAR_ID                  = "star_id";
std::string Stars::STAR_USER_ID             = "user_id";
std::string Stars::STAR_DATE                = "date";
std::string Stars::STAR_TIME                = "time";
std::string Stars::STAR_PRODUCT_GROUP_ID    = "product_group_id";
std::string Stars::STAR_NO_OF_STARS         = "no_of_stars";

std::string Reviews::REVIEW_ID                  = "review_id";
std::string Reviews::REVIEW_USER_ID             = "user_id";
std::string Reviews::REVIEW_DATE                = "date";
std::string Reviews::REVIEW_TIME                = "time";
std::string Reviews::REVIEW_PRODUCT_GROUP_ID    = "product_group_id";
std::string Reviews::REVIEW_REVIEW              = "review   ";

DBInterface::DBInterface(std::string dbFileName, FILE *fp) {
    m_hDB   = std::make_shared<SQLite::Database>(dbFileName, SQLite::OPEN_READWRITE);
    m_Fp    = fp;
}

DBInterface::~DBInterface() {}

int DBInterface::getIntStatus(CartStatus stat) {
    int iStat;
    switch(stat) {
        case CartStatus::CARTED:
            iStat = 0;
            break;
        case CartStatus::PAYMENT_PENDING:
            iStat = 1;
            break;
        case CartStatus::READY_FOR_DELIVERY:
            iStat = 2;
            break;
        case CartStatus::DELIVERED:
            iStat = 3;
            break;
        case CartStatus::CANCELLED:
            iStat = 4;
            break;
        case CartStatus::NOTA:
            iStat = 5;
            break;
        default:
            iStat = 6;
    }
    return iStat;
}

unsigned int DBInterface::generateOrderNo(OrderType orderType, FILE *fp) {
    fprintf(fp, "BaseBot %ld: generateOrderNo\n", time(0)); fflush(fp);
    int invoice_no = 1000;
    std::string clmName;
    std::stringstream ss;

    //  Odd number for Orders ('O' to 'O')
    //  Even number for all Transactions
    if(orderType == OrderType::PORDER) {
        clmName = User::USER_ORDER_NO;
        invoice_no = 1001;
    } else if(orderType == OrderType::TOPUP) {
        clmName = User::USER_TRANSAC_NO;
    }

    ss << "SELECT MAX(" << clmName << ") FROM User;";
    SQLite::Statement query(*m_hDB, ss.str());
    if(query.executeStep()) {
        ss.str(std::string()); ss << "MAX(" << clmName << ")";
        invoice_no = query.getColumn(ss.str().c_str()).getUInt();
    }
    return invoice_no + 2;
}

void DBInterface::updateOrderNo(unsigned int iUserId, FILE *fp) {
    std::stringstream ss;
    unsigned int iOrderNo   = generateOrderNo(OrderType::PORDER, fp);
    SQLite::Transaction transaction(*m_hDB);
    ss << "UPDATE User SET " << User::USER_ORDER_NO << " = " << iOrderNo << " WHERE " << User::USER_ID << " = " << iUserId << ";";
    m_hDB->exec(ss.str());
    transaction.commit();
}

void DBInterface::updateTransacNo(unsigned int iUserId, FILE *fp) {
    std::stringstream ss;
    unsigned int iTransacNo   = generateOrderNo(OrderType::TOPUP, fp);
    SQLite::Transaction transaction(*m_hDB);
    ss << "UPDATE User SET " << User::USER_TRANSAC_NO << " = " << iTransacNo << " WHERE " << User::USER_ID << " = " << iUserId << ";";
    m_hDB->exec(ss.str());
    transaction.commit();
}

bool DBInterface::addNewUser(int64_t chatId, std::string fname, FILE *fp) {
    fprintf(fp, "BaseBot %ld: addNewUser fname :%s\n", time(0), fname.c_str()); fflush(fp);
    std::stringstream ss;
    ss << "SELECT * FROM User WHERE " << User::USER_CHAT_ID << " = " << chatId << ";";
    SQLite::Statement query(*m_hDB, ss.str());
    if(!query.executeStep()) {
        fprintf(fp, "BaseBot %ld: New User %s\n", time(0), fname.c_str()); fflush(fp);
        //      A new user. So add him / her to database
        SQLite::Transaction transaction(*m_hDB);
        ss.str(std::string());
        unsigned int order_no = generateOrderNo(OrderType::PORDER, fp);
        ss << " INSERT INTO User (" << User::USER_NAME << ", " << User::USER_CHAT_ID << ", " << User::USER_ORDER_NO << ", " << User::USER_TRANSAC_NO
                << ") VALUES (\"" << fname << "\", " << chatId << ", " << order_no << ", " << generateOrderNo(OrderType::TOPUP, fp) << ");";
        m_hDB->exec(ss.str());
        transaction.commit();
    } else {
        return false;
    }
    return true;
}

User::Ptr DBInterface::getUser(SQLite::Statement *pQuery) {
    User::Ptr pUser     = std::make_shared<User>();
    pUser->m_UserId     = pQuery->getColumn(User::USER_ID.c_str()).getInt();
    pUser->m_Name       = pQuery->getColumn(User::USER_NAME.c_str()).getString();
    pUser->m_ChatId     = pQuery->getColumn(User::USER_CHAT_ID.c_str()).getInt();
    pUser->m_OrderNo    = pQuery->getColumn(User::USER_ORDER_NO.c_str()).getInt();
    pUser->m_Mobile     = pQuery->getColumn(User::USER_MOBILE.c_str()).getInt64();
    pUser->m_Address    = pQuery->getColumn(User::USER_ADDRESS.c_str()).getString();
    pUser->m_WBalance   = pQuery->getColumn(User::USER_WBALANCE.c_str()).getInt();
    pUser->m_TransacNo  = pQuery->getColumn(User::USER_TRANSAC_NO.c_str()).getInt();
    return pUser;
}

std::vector<User::Ptr> DBInterface::getAllUsers(FILE *fp) {
    std::vector<User::Ptr> users;
    User::Ptr pUser;
    std::stringstream ss;

    ss << "SELECT * FROM User;";
    SQLite::Statement query(*m_hDB, ss.str());

    while(query.executeStep()) {
        pUser   = getUser(&query);
        users.push_back(pUser);
    }
    return users;
}

int DBInterface::getWalletBalance(unsigned int iUserId, FILE *fp) {
    int iBal = 0;
    std::stringstream ss;

    ss << "SELECT * FROM User WHERE " << User::USER_ID << " = " << iUserId << ";";
    SQLite::Statement query(*m_hDB, ss.str());
    if(query.executeStep()) {
        User::Ptr pUser   = getUser(&query);
        iBal    = pUser->m_WBalance;
    }
    return iBal;
}

User::Ptr DBInterface::forceWalletBalance(std::string strUserId, std::string strWBal, FILE *fp) {
    std::stringstream ss;
    unsigned int iUserId = 0;
    int wBal = 0;   // can be -ve
    User::Ptr pUser = nullptr;
    SQLite::Transaction transaction(*m_hDB);

    //  string to int conversion
    try { iUserId = std::stoi(strUserId); wBal = std::stoi(strWBal); }
    catch(std::exception &e) {iUserId = 0; wBal = 0;}

    pUser = nullptr;
    if(0 < iUserId && MAX_FORCE_AMOUNT > std::abs(wBal)) {
        pUser   = getUserForUserId(iUserId, fp);
        if(nullptr == pUser) return nullptr;

        ss.str("");
        ss << "UPDATE User SET " << User::USER_WBALANCE << " = " << wBal << " WHERE "
                    << User::USER_ID << " = " << pUser->m_UserId << ";";
        m_hDB->exec(ss.str());

        ss.str("");
        ss << "INSERT INTO POrder (" <<
            POrder::PORDER_NO << ", " <<
            POrder::PORDER_USER_ID << ", " <<
            POrder::PORDER_AMOUNT << ", " <<
            POrder::PORDER_WBALANCE << ", " <<
            POrder::PORDER_USER_NAME << ", " <<
            POrder::PORDER_ORDR_TM << ", " <<
            POrder::PORDER_DLVR_TM << ", " <<
            POrder::PORDER_PAY_GW << ", " <<
            POrder::PORDER_STATUS << ", " <<
            POrder::PORDER_ADDRESS << ") VALUES (" <<
            pUser->m_TransacNo << ", " << pUser->m_UserId << ", 0, " << wBal << ", \"" << pUser->m_Name
            << "\", \"" << getCurTime() << "\", \"-\", \"Force\", " <<
            getIntStatus(CartStatus::READY_FOR_DELIVERY) << ", \"" << pUser->m_Address << "\" );";
        m_hDB->exec(ss.str());
        transaction.commit();

        updateTransacNo(pUser->m_UserId, fp);

        //  Get latest transac no, in case
        pUser   = getUserForUserId(pUser->m_UserId, fp);
    }
    return pUser;
}

int DBInterface::getNoOfUsers(FILE *fp) {
    int iNoOfUsers = 0;

    SQLite::Statement query(*m_hDB, "SELECT COUNT(*) FROM User;");
    if(query.executeStep()) {
        iNoOfUsers = query.getColumn("COUNT(*)").getUInt();
    }
    return iNoOfUsers;
}

User::Ptr DBInterface::getUserForUserId(unsigned int iUserId, FILE *fp) {
    std::stringstream ss;
    User::Ptr pUser;
    ss << "SELECT * FROM User WHERE " << User::USER_ID << " = " << iUserId << ";";
    SQLite::Statement query(*m_hDB, ss.str());
    if(query.executeStep()) {
        pUser   = getUser(&query);
    }
    return pUser;
}

User::Ptr DBInterface::getUserForChatId(unsigned int iChatId, FILE *fp) {
    std::stringstream ss;
    User::Ptr pUser;
    ss << "SELECT * FROM User WHERE " << User::USER_CHAT_ID << " = " << iChatId << ";";
    SQLite::Statement query(*m_hDB, ss.str());
    if(query.executeStep()) {
        pUser   = getUser(&query);
    }
    return pUser;
}

int DBInterface::addProductToCart(unsigned int iProdId, unsigned qty, unsigned int iPrice, unsigned int chatId, FILE *fp) {
    std::stringstream ss;
    int iQty = 0;
    User::Ptr pUser = getUserForChatId(chatId, fp);
    ss << "SELECT * FROM Cart WHERE " <<
            Cart::CART_ORDER_NO << " = " << pUser->m_OrderNo <<
                    " AND " <<
            Cart::CART_PRODUCT_ID <<" = " << iProdId << ";";

    SQLite::Statement query(*m_hDB, ss.str());
    if(query.executeStep()) {
        iQty    = query.getColumn(Cart::CART_QNTY.c_str()).getInt();
        SQLite::Transaction transaction(*m_hDB);
        ss.str(std::string());
        ss << "UPDATE Cart SET " << Cart::CART_QNTY << " = " << Cart::CART_QNTY << " + 1 WHERE "
                << Cart::CART_PRODUCT_ID << " = " << iProdId << " AND " << Cart::CART_ORDER_NO << " = " << pUser->m_OrderNo << ";";
        m_hDB->exec(ss.str());
        transaction.commit();
    } else {
        SQLite::Transaction transaction(*m_hDB);
        ss.str(std::string());
        ss << "INSERT INTO Cart (" <<
                Cart::CART_PRODUCT_ID << ", " <<
                Cart::CART_PRODUCT_PRICE << ", " <<
                Cart::CART_USER_ID << ", " <<
                Cart::CART_QNTY << ", " <<
                Cart::CART_STATUS << ", " <<
                Cart::CART_ORDER_NO << ") VALUES (" <<
                iProdId << "," << iPrice << "," << pUser->m_UserId << ", " << qty << ", " <<
                getIntStatus(CartStatus::CARTED) << ", " << pUser->m_OrderNo << ");";
        m_hDB->exec(ss.str());
        transaction.commit();
    }

    return iQty+1;
}

bool DBInterface::incrementItemQty(int iProdId, unsigned int iOrderNo, FILE *fp) {
    std::stringstream ss;
    ss << "SELECT * FROM Cart WHERE " << Cart::CART_ORDER_NO << " = " << iOrderNo <<
                " AND " <<
        Cart::CART_PRODUCT_ID <<" = " << iProdId << ";";

    SQLite::Statement query(*m_hDB, ss.str());
    if(query.executeStep()) {
        SQLite::Transaction transaction(*m_hDB);
        ss.str(std::string());
        ss << "UPDATE Cart SET " << Cart::CART_QNTY << " = " << Cart::CART_QNTY << " + 1 WHERE "
                << Cart::CART_PRODUCT_ID << " = " << iProdId << " AND " << Cart::CART_ORDER_NO << " = " << iOrderNo << ";";
        m_hDB->exec(ss.str());
        transaction.commit();
    }
    return true;
}

bool DBInterface::removeItemFromCart(int iProdId, unsigned int iOrderNo, FILE *fp) {
    std::stringstream ss;
    Cart::Ptr pCart;

    ss << "SELECT * FROM Cart WHERE " <<
        Cart::CART_ORDER_NO << " = " << iOrderNo <<
                " AND " <<
        Cart::CART_PRODUCT_ID <<" = " << iProdId << ";";

    SQLite::Statement query(*m_hDB, ss.str());
    if(query.executeStep()) {
        pCart = getCart(&query);

        SQLite::Transaction transaction(*m_hDB);
        ss.str(std::string());
        ss << "DELETE FROM Cart WHERE " <<
                Cart::CART_ORDER_NO << " = " << iOrderNo <<
                        " AND " <<
                Cart::CART_PRODUCT_ID <<" = " << iProdId << ";";
        m_hDB->exec(ss.str());

        ss.str(std::string());
        ss << "UPDATE User SET " << User::USER_WBALANCE << " = "
                        << User::USER_WBALANCE << " + " << (pCart->m_Price * pCart->m_Qnty)
                << " WHERE " << User::USER_ID << " = " << pCart->m_UserId << ";";
        m_hDB->exec(ss.str());

        transaction.commit();
    }
    return true;
}

bool DBInterface::reduceCartQty(int iProdId, unsigned int iOrderNo, FILE *fp){
    std::stringstream ss;

    ss << "SELECT * FROM Cart WHERE " <<
        Cart::CART_ORDER_NO << " = " << iOrderNo <<
                " AND " <<
        Cart::CART_PRODUCT_ID <<" = " << iProdId << ";";

    SQLite::Statement query(*m_hDB, ss.str());
    if(query.executeStep()) {
        if(1 < query.getColumn(Cart::CART_QNTY.c_str()).getInt()) {
            SQLite::Transaction transaction(*m_hDB);
            ss.str(std::string());
            ss << "UPDATE Cart SET " << Cart::CART_QNTY << " = " << Cart::CART_QNTY << " - 1 WHERE "
                    << Cart::CART_PRODUCT_ID << " = " << iProdId << " AND " << Cart::CART_ORDER_NO << " = " << iOrderNo << ";";
            m_hDB->exec(ss.str());
            transaction.commit();
        } else {
            SQLite::Transaction transaction(*m_hDB);
            ss.str(std::string());
            ss << "DELETE FROM Cart WHERE " <<
                    Cart::CART_ORDER_NO << " = " << iOrderNo <<
                            " AND " <<
                    Cart::CART_PRODUCT_ID <<" = " << iProdId << ";";
            m_hDB->exec(ss.str());
            transaction.commit();
        }
        return true;
    }
    return false;
}

Cart::Ptr DBInterface::getCart(SQLite::Statement *pQuery) {
    Cart::Ptr pCart     = std::make_shared<Cart>();
    pCart->m_CartId     = pQuery->getColumn(Cart::CART_ID.c_str()).getInt();
    pCart->m_ProductId  = pQuery->getColumn(Cart::CART_PRODUCT_ID.c_str()).getInt();
    pCart->m_UserId     = pQuery->getColumn(Cart::CART_USER_ID.c_str()).getInt();
    pCart->m_Qnty       = pQuery->getColumn(Cart::CART_QNTY.c_str()).getInt();
    pCart->m_OrderNo    = pQuery->getColumn(Cart::CART_ORDER_NO.c_str()).getInt();
    pCart->m_Price      = pQuery->getColumn(Cart::CART_PRODUCT_PRICE.c_str()).getInt();
    pCart->m_Status     = static_cast<CartStatus>(pQuery->getColumn(Cart::CART_STATUS.c_str()).getInt());
    return pCart;
}

std::vector<Cart::Ptr> DBInterface::getCartItemsForOrderNo(unsigned int iOrderNo, FILE *fp) {
    std::vector<Cart::Ptr> cartItems;
    Cart::Ptr pCart;
    std::stringstream ss;

    ss << "SELECT * FROM Cart WHERE " << Cart::CART_ORDER_NO << " = " << iOrderNo << ";";
    SQLite::Statement query(*m_hDB, ss.str());

    while(query.executeStep()) {
        pCart   = getCart(&query);
        cartItems.push_back(pCart);
    }
    return cartItems;
}

bool DBInterface::emptyCartForUser(unsigned int iOrderNo, FILE *fp) {
    std::stringstream ss;
    ss << "DELETE FROM Cart WHERE " << Cart::CART_ORDER_NO << " = " << iOrderNo << ";";
    SQLite::Transaction transaction(*m_hDB);
    m_hDB->exec(ss.str());
    transaction.commit();
    return true;
}

std::vector<User::Ptr> DBInterface::getCartedUsers(FILE *fp) {
    std::vector<User::Ptr> users;
    User::Ptr pUser;
    std::stringstream ss;

    ss << "SELECT * FROM User WHERE " << User::USER_ID << " IN( SELECT " << Cart::CART_USER_ID << " FROM Cart WHERE "
            << Cart::CART_STATUS << " = " << getIntStatus(CartStatus::CARTED) << " );";

    SQLite::Statement query(*m_hDB, ss.str());

    while(query.executeStep()) {
        pUser   = getUser(&query);
        users.push_back(pUser);
    }
    return users;
}

Category::Ptr DBInterface::getCategory(SQLite::Statement *pQuery) {
    Category::Ptr pCategory = std::make_shared<Category>();
    pCategory->m_CategoryId = pQuery->getColumn(Category::CATEGORY_ID.c_str()).getInt();
    pCategory->m_Name       = pQuery->getColumn(Category::CATEGORY_NAME.c_str()).getString();
    pCategory->m_Code       = pQuery->getColumn(Category::CATEGORY_CODE.c_str()).getString();
    pCategory->m_Package    = pQuery->getColumn(Category::CATEGORY_PACKAGE.c_str()).getString();
    return pCategory;
}

std::vector<Category::Ptr> DBInterface::getCategories(FILE *fp) {
    std::vector<Category::Ptr> pCategories;
    std::stringstream ss;
    Category::Ptr pCategory;

    ss << "SELECT * FROM Category;";
    SQLite::Statement query(*m_hDB, ss.str());
    while(query.executeStep()) {
        pCategory = getCategory(&query);
        pCategories.push_back(pCategory);
    }
    return pCategories;
}

Category::Ptr DBInterface::getCategoryForCode(std::string strCode, FILE *fp) {
    Category::Ptr pCategory = nullptr;
    std::stringstream ss;

    ss << "SELECT * FROM Category WHERE " << Category::CATEGORY_CODE << " = \"" << strCode << "\";";
    SQLite::Statement query(*m_hDB, ss.str());
    if(query.executeStep()) {
        pCategory = getCategory(&query);
    }
    return pCategory;
}

Category::Ptr DBInterface::getCategoryById(int catId, FILE *fp) {
    Category::Ptr pCategory = nullptr;
    std::stringstream ss;

    ss << "SELECT * FROM Category WHERE " << Category::CATEGORY_ID << " = " << catId << ";";
    SQLite::Statement query(*m_hDB, ss.str());
    if(query.executeStep()) {
        pCategory = getCategory(&query);
    }
    return pCategory;
}

Category::Ptr DBInterface::getCategoryForName(std::string strName, FILE *fp) {
    Category::Ptr pCategory = nullptr;
    std::stringstream ss;

    ss << "SELECT * FROM Category WHERE " << Category::CATEGORY_NAME << " = \"" << strName << "\";";
    SQLite::Statement query(*m_hDB, ss.str());
    if(query.executeStep()) {
        pCategory = getCategory(&query);
    }
    return pCategory;
}

Product::Ptr DBInterface::getProduct(SQLite::Statement *pQuery) {
    Product::Ptr pProduct   = std::make_shared<Product>();
    pProduct->m_Name        = pQuery->getColumn(Product::PRODUCT_NAME.c_str()).getString();
    pProduct->m_Code        = pQuery->getColumn(Product::PRODUCT_CODE.c_str()).getString();
    pProduct->m_Pack        = pQuery->getColumn(Product::PRODUCT_PACK.c_str()).getString();
    pProduct->m_Desc        = pQuery->getColumn(Product::PRODUCT_DESC.c_str()).getString();
    pProduct->m_Date        = pQuery->getColumn(Product::PRODUCT_DATE.c_str()).getString();
    pProduct->m_PicFile     = pQuery->getColumn(Product::PRODUCT_PIC_FILE.c_str()).getString();

    pProduct->m_ProductId   = pQuery->getColumn(Product::PRODUCT_ID.c_str()).getInt();
    pProduct->m_Price       = pQuery->getColumn(Product::PRODUCT_PRICE.c_str()).getInt();

    //  If Agent, there is a discount
    pProduct->m_Price       = (isAgent) ? (pProduct->m_Price * (100 - DISCOUNT_PERCENT)) / 100 : pProduct->m_Price;
    return pProduct;
}

std::vector<Product::Ptr> DBInterface::getAllProducts(FILE *fp) {
    std::vector<Product::Ptr> pProducts;
    Product::Ptr pProduct;
    std::stringstream ss;

    ss << "SELECT * FROM Product;";
    SQLite::Statement query(*m_hDB, ss.str());
    while(query.executeStep()) {
        pProduct    = getProduct(&query);
        pProducts.push_back(pProduct);
    }
    return pProducts;
}
bool DBInterface::insertNewProduct(std::string strCat, std::string strName, std::string strPrice, FILE *fp) {
    int iFound = 0, iPrice;
    std::stringstream ss;

    //  Parse category code
    if(2 != strCat.length()) return false;
    std::vector<Product::Ptr> prods = getAllProducts(fp);
    for(auto &prod : prods) {
        if(!prod->m_Name.compare(strName)) return false;
        if(std::string::npos != prod->m_Code.find(strCat))
            iFound++;
    }
    if(0 != iFound) {
        iFound++;
        ss << strCat << "-" <<std::setfill('0') << std::setw(2) << iFound;
        strCat = ss.str();
    } else return false;

    //  Check name
    if(MAX_ITEM_LENGTH < strName.length()) return false;

    //  Check Price
    try { iPrice  = std::stoi(strPrice); } catch(std::exception &e) { return false; }

    SQLite::Transaction transaction(*m_hDB);
    ss.str(""); ss << "INSERT INTO Product (" <<
                Product::PRODUCT_CODE << ", " <<
                Product::PRODUCT_NAME << ", " <<
                Product::PRODUCT_PIC_FILE << ", " <<
                Product::PRODUCT_PRICE << ", " <<
                Product::PRODUCT_PACK<< ", " <<
                Product::PRODUCT_DESC << ", " <<
                Product::PRODUCT_DATE << ") VALUES (\"" << strCat << "\", \"" << strName << "\", \"file\", "
                << iPrice << ", \"1pk\", \"-\", \"1970-01-01 00:00:00:000\");";
    m_hDB->exec(ss.str());
    transaction.commit();
    return true;
}
std::vector<Product::Ptr> DBInterface::getAllActiveProducts(FILE *fp) {
    std::vector<Product::Ptr> pProducts;
    std::stringstream ss;
    Product::Ptr pProduct;
    std::string strDate = getCurTime();

    ss << "SELECT * FROM Product WHERE SUBSTR(" << Product::PRODUCT_DATE << ", 1, 10) = \"" << getTmrwDate() << "\" ORDER BY " << Product::PRODUCT_DATE << " ASC;";
#ifdef AURA
    ss.str(""); ss << "SELECT * FROM Product;";
#endif
    SQLite::Statement query(*m_hDB, ss.str());
    while(query.executeStep()) {
        pProduct    = getProduct(&query);
        pProducts.push_back(pProduct);
    }
    return pProducts;
}

void DBInterface::activateProductForTomorrow(unsigned int iProdId, FILE *fp) {
#ifndef AURA
    std::stringstream ss;
    ss << "UPDATE Product SET " << Product::PRODUCT_DATE <<  " = \"" << getTmrwDtTmSecsMilli() << "\" WHERE "
            << Product::PRODUCT_ID << " = " << iProdId << ";";
    SQLite::Transaction transaction(*m_hDB);
    m_hDB->exec(ss.str());
    transaction.commit();
#endif
}

void DBInterface::removeProductFromTomorrow(unsigned int iProdId, FILE *fp) {
#ifndef AURA
    std::stringstream ss;
    ss << "UPDATE Product SET " << Product::PRODUCT_DATE <<  " = \"1970-01-01 00:00:00:000\" WHERE "
            << Product::PRODUCT_ID << " = " << iProdId << ";";
    SQLite::Transaction transaction(*m_hDB);
    m_hDB->exec(ss.str());
    transaction.commit();
#endif
}

Product::Ptr DBInterface::getProductForCode(std::string strCode, FILE *fp) {
    Product::Ptr pProd;
    std::stringstream ss;

    ss << "SELECT * FROM Product WHERE " << Product::PRODUCT_CODE << " = \"" << strCode << "\";";
    SQLite::Statement query(*m_hDB, ss.str());
    if(query.executeStep()) {
        pProd    = getProduct(&query);
    }
    return pProd;
}

Product::Ptr DBInterface::getProductById(unsigned int iProdId, FILE *fp) {
    Product::Ptr pProduct;
    std::stringstream ss;

    ss << "SELECT * FROM Product WHERE " << Product::PRODUCT_ID << " = " << iProdId << ";";
    SQLite::Statement query(*m_hDB, ss.str());
    if(query.executeStep()) {
        pProduct    = getProduct(&query);
    }
    return pProduct;
}

std::vector<Product::Ptr> DBInterface::getOneProductForName(std::string strName, FILE *fp) {
    std::vector<Product::Ptr> oneProduct;
    Product::Ptr pProduct;
    std::stringstream ss;

    ss << "SELECT * FROM Product WHERE " << Product::PRODUCT_NAME << " = \"" << strName << "\";";
    SQLite::Statement query(*m_hDB, ss.str());
    while(query.executeStep()) {
        pProduct    = getProduct(&query);
        oneProduct.push_back(pProduct);
    }
    return oneProduct;
}

std::vector<Product::Ptr> DBInterface::getOneProductForCode(std::string strCode, FILE *fp) {
    std::vector<Product::Ptr> oneProduct;
    Product::Ptr pProduct;
    std::stringstream ss;

    ss << "SELECT * FROM Product WHERE " << Product::PRODUCT_CODE << " = \"" << strCode << "\";";
    SQLite::Statement query(*m_hDB, ss.str());
    while(query.executeStep()) {
        pProduct    = getProduct(&query);
        oneProduct.push_back(pProduct);
    }
    return oneProduct;
}

bool DBInterface::updateProductPrice(std::string strProdId, std::string strPrice, FILE *fp) {
    std::stringstream ss;
    int iProdId, iPrice;
    SQLite::Transaction transaction(*m_hDB);
    try {
        iProdId = std::stoi(strProdId);
        iPrice  = std::stoi(strPrice);
    } catch(std::exception &e) {
        return false;
    }
    ss << "UPDATE Product SET " << Product::PRODUCT_PRICE << " = " << iPrice << " WHERE "
            << Product::PRODUCT_ID << " = " << iProdId << ";";
    int iRows   = m_hDB->exec(ss.str());
    if(0 < iRows)   {
        ss.str("");
        ss << "UPDATE Cart SET " << Cart::CART_PRODUCT_PRICE << " = " << iPrice << " WHERE "
                << Cart::CART_PRODUCT_ID << " = " << iProdId << " AND "
                << Cart::CART_STATUS << " = " <<  getIntStatus(CartStatus::CARTED) << ";";
        m_hDB->exec(ss.str());
        transaction.commit();
    }
    return (0 < iRows);
}

unsigned int DBInterface::addAddressToShipping(unsigned int iUserId, std::string strAddress, FILE *fp) {
    std::stringstream ss;
    SQLite::Transaction transaction(*m_hDB);
    ss << "UPDATE User SET " << User::USER_ADDRESS << " = \"" << strAddress << "\" WHERE "
            << User::USER_ID << " = " << iUserId << ";";
    m_hDB->exec(ss.str());
    transaction.commit();
    return 0;
}

/*
+---------+-----------------------+-----------------------+
|         | POrder                | TopUp                 |
+---------+-----------------------+-----------------------+
| Confirm | Update Order, Cart    | Update Order, Cart    |
|         | Status                | Status                |
+---------+-----------------------+-----------------------+
| Cancel  | Update Order, Cart    | Update Order, Cart    |
|         | Status                | Status                |
|         | Increment Wallet      | Decrement Wallet      |
|         | Increment all transac | Decrement all transac |
+---------+-----------------------+-----------------------+
*/
void DBInterface::updateOrderStatus(int iOrderNo, CartStatus crtStatus, OrderType ordrTyp, FILE *fp) {
    std::stringstream ss;
    POrder::Ptr pOrder;
    int iNewAmt =  0, wBal = 0;
    SQLite::Transaction transaction(*m_hDB);

    ss << "SELECT * FROM POrder WHERE " << POrder::PORDER_NO << " = " << iOrderNo << ";";
    SQLite::Statement query(*m_hDB, ss.str());
    if(query.executeStep()) {
        pOrder  = getPOrder(&query);
        iNewAmt = pOrder->m_Amt;
        wBal    = pOrder->m_WBalance;

//--------------- Wallet & transactions update. Only applicable for Cancel request-----------------------------------------------
        if(CartStatus::CANCELLED == crtStatus) {
            std::string strPlusMinus;
            strPlusMinus = (OrderType::TOPUP == ordrTyp) ? " - " : " + ";

            ss.str(std::string());
            ss << "UPDATE User SET " << User::USER_WBALANCE << " = "  << User::USER_WBALANCE << strPlusMinus << iNewAmt
                    << " WHERE " << User::USER_ID << " = " << pOrder->m_UserId << ";";
            m_hDB->exec(ss.str());

            //  Increment / Decrement all the transactions after that
            ss.str(std::string());
            ss << "UPDATE POrder SET " << POrder::PORDER_WBALANCE << " = " << POrder::PORDER_WBALANCE << strPlusMinus << iNewAmt
                    << " WHERE " << POrder::PORDER_ORDR_TM << " > \"" << pOrder->m_OrdrTm << "\" AND "
                    << POrder::PORDER_USER_ID << " = " << pOrder->m_UserId << ";";
            m_hDB->exec(ss.str());

            wBal    = (OrderType::TOPUP == ordrTyp) ? (wBal - iNewAmt) : (wBal + iNewAmt);
            iNewAmt = 0;
        }

// ----------------------Update Order / Cart status. Common for all scenarios------------------------------------------------------
        ss.str(std::string());
        ss << "UPDATE POrder SET " << POrder::PORDER_STATUS << " = " << getIntStatus(crtStatus)
                << ", " << POrder::PORDER_AMOUNT << " = " << iNewAmt
                << ", " << POrder::PORDER_WBALANCE << " = " << wBal
                << " WHERE " << POrder::PORDER_NO << " = " << iOrderNo << ";";

        //  There is no delivered status for TopUp
        if(CartStatus::DELIVERED == crtStatus && (iOrderNo % 2)) {
            ss.str(std::string());ss << "UPDATE POrder SET " <<
                    POrder::PORDER_STATUS << " = " << getIntStatus(crtStatus) << ", " <<
                    POrder::PORDER_DLVR_TM << " = \"" << getCurTime() <<
                    "\" WHERE " << POrder::PORDER_NO << " = " << iOrderNo << ";";
        }
        m_hDB->exec(ss.str());

        if(OrderType::PORDER == ordrTyp) {
            ss.str(std::string());ss << "UPDATE Cart SET " << Cart::CART_STATUS << " = " << getIntStatus(crtStatus)
                        << " WHERE " << Cart::CART_ORDER_NO << " = " << iOrderNo << ";";
            m_hDB->exec(ss.str());
        }

        transaction.commit();
    }
}
std::string DBInterface::getYstrDate() {
    time_t t = time(NULL) - SECS_IN_A_DAY;
    char buffer[64];
    strftime(buffer, 64, "%Y-%m-%d", localtime(&t));
    return std::string(buffer);
}

std::string DBInterface::getTmrwDtTmSecsMilli() {
    std::stringstream ss;
    char buffer[64];
    struct timeval st;
    time_t t;

    //  Get date time with secs
    t = time(NULL) + SECS_IN_A_DAY;
    strftime(buffer, 64, "%Y-%m-%d %T", localtime(&t));

    //  Get millis
    gettimeofday(&st,NULL);
    unsigned long msecs = st.tv_usec / 1000;

    //  Append millis
    ss << buffer << ":" << msecs;
    return ss.str();
}

std::string DBInterface::getTmrwDate() {
    time_t t = time(NULL) + SECS_IN_A_DAY;
    char buffer[64];
    strftime(buffer, 64, "%Y-%m-%d", localtime(&t));
    return std::string(buffer);
}

std::string DBInterface::getCurTime() {
    time_t t = time(NULL);
    char buffer[64];
    strftime(buffer, 64, "%Y-%m-%d %T", localtime(&t));
    return std::string(buffer);
}

void DBInterface::insertToOrder(User::Ptr pUser, unsigned int uiAmt, CartStatus crtStatus, std::string strGW, OrderType ordrTyp, FILE *fp) {
    std::stringstream ss;
    time_t t    = time(NULL);
    int wBal    = (OrderType::TOPUP == ordrTyp) ? (pUser->m_WBalance + uiAmt) : static_cast<int>(pUser->m_WBalance - uiAmt);
    unsigned int uiOrdNo    =  (OrderType::TOPUP == ordrTyp) ? pUser->m_TransacNo : pUser->m_OrderNo;

    SQLite::Transaction transaction(*m_hDB);
    ss << "INSERT INTO POrder (" <<
            POrder::PORDER_NO << ", " <<
            POrder::PORDER_USER_ID << ", " <<
            POrder::PORDER_AMOUNT << ", " <<
            POrder::PORDER_WBALANCE << ", " <<
            POrder::PORDER_USER_NAME << ", " <<
            POrder::PORDER_ORDR_TM << ", " <<
            POrder::PORDER_DLVR_TM << ", " <<
            POrder::PORDER_PAY_GW << ", " <<
            POrder::PORDER_STATUS << ", " <<
            POrder::PORDER_ADDRESS << ") VALUES (" <<
            uiOrdNo << ", " << pUser->m_UserId << ", " << uiAmt << ", " << wBal << ", \"" << pUser->m_Name
            << "\", \"" << getCurTime() << "\", \"-\", \"" << strGW << "\", " <<
            getIntStatus(crtStatus) << ", \"" << pUser->m_Address << "\" );";
    m_hDB->exec(ss.str());

    if(OrderType::PORDER == ordrTyp) {
        ss.str(std::string());
        ss << "UPDATE Cart SET " << Cart::CART_STATUS << " = " << getIntStatus(CartStatus::PAYMENT_PENDING) << " WHERE "
                << Cart::CART_ORDER_NO << " = " << pUser->m_OrderNo << ";";
        m_hDB->exec(ss.str());
    }

    ss.str(std::string());
    ss << "UPDATE User SET " << User::USER_WBALANCE << " = " << wBal << " WHERE "
                << User::USER_ID << " = " << pUser->m_UserId << ";";
    m_hDB->exec(ss.str());

    transaction.commit();
}

POrder::Ptr DBInterface::getPOrder(SQLite::Statement *pQuery) {
    POrder::Ptr pOrder  = std::make_shared<POrder>();
    pOrder->m_Amt       = pQuery->getColumn(POrder::PORDER_AMOUNT.c_str()).getInt();
    pOrder->m_OrderId   = pQuery->getColumn(POrder::PORDER_ID.c_str()).getInt();
    pOrder->m_OrderNo   = pQuery->getColumn(POrder::PORDER_NO.c_str()).getInt();
    pOrder->m_UserId    = pQuery->getColumn(POrder::PORDER_USER_ID.c_str()).getInt();
    pOrder->m_Name      = pQuery->getColumn(POrder::PORDER_USER_NAME.c_str()).getString();
    pOrder->m_OrdrTm    = pQuery->getColumn(POrder::PORDER_ORDR_TM.c_str()).getString();
    pOrder->m_DlvrdTm   = pQuery->getColumn(POrder::PORDER_DLVR_TM.c_str()).getString();
    pOrder->m_PayGW     = pQuery->getColumn(POrder::PORDER_PAY_GW.c_str()).getString();
    pOrder->m_Address   = pQuery->getColumn(POrder::PORDER_ADDRESS.c_str()).getString();
    pOrder->m_WBalance  = pQuery->getColumn(POrder::PORDER_WBALANCE.c_str()).getInt();
    pOrder->m_Status    = static_cast<CartStatus>(pQuery->getColumn(POrder::PORDER_STATUS.c_str()).getInt());
    return pOrder;
}

void DBInterface::confirmTopUpAmount(unsigned int iOrderNo, unsigned int iAmount, FILE *fp) {
    std::stringstream ss;
    POrder::Ptr pOrder = nullptr;

    ss << "SELECT * FROM POrder WHERE " << POrder::PORDER_NO << " = " << iOrderNo << ";";
    SQLite::Statement query(*m_hDB, ss.str());
    if(query.executeStep()) {
        SQLite::Transaction transaction(*m_hDB);
        pOrder  = getPOrder(&query);

        ss.str("");
        ss << "UPDATE POrder SET " << POrder::PORDER_AMOUNT << " = " << iAmount
                << ", " << POrder::PORDER_DLVR_TM << " = \"" << getCurTime()
                << "\", " << POrder::PORDER_STATUS << " = " << getIntStatus(CartStatus::READY_FOR_DELIVERY)
                << " WHERE " << POrder::PORDER_NO << " = " << iOrderNo << ";";
        m_hDB->exec(ss.str());

        ss.str("");
        ss << "UPDATE User SET " << User::USER_WBALANCE << " = " << User::USER_WBALANCE << " + " << iAmount << " WHERE "
            << User::USER_ID << " = " << pOrder->m_UserId << ";";
        m_hDB->exec(ss.str());

        transaction.commit();
    }
}

bool DBInterface::isAnyPendingOrders(FILE *fp) {
    std::stringstream ss;

    ss << "SELECT * FROM POrder WHERE ("
            << POrder::PORDER_STATUS << " = " << getIntStatus(CartStatus::PAYMENT_PENDING) << " OR "
            << POrder::PORDER_STATUS << " = " << getIntStatus(CartStatus::READY_FOR_DELIVERY) << ") AND ("
            << POrder::PORDER_NO << " % 2) != 0;";
	    SQLite::Statement query(*m_hDB, ss.str());
    return query.executeStep();
}

//  Do not discrimiate if the order type is POrder or TopUp. Coz, user is going to see all
POrder::Ptr DBInterface::getOrderForOrderNo(unsigned int iOrderNo, FILE *fp) {
    POrder::Ptr pOrder = nullptr;
    std::stringstream ss;

    ss << "SELECT * FROM POrder WHERE " << POrder::PORDER_NO << " = " << iOrderNo << ";";
    SQLite::Statement query(*m_hDB, ss.str());
    if(query.executeStep()) {
        pOrder  = getPOrder(&query);
    }
    return pOrder;
}

std::vector<POrder::Ptr> DBInterface::getOrdersByUser(unsigned int iUserId, FILE *fp) {
    std::vector<POrder::Ptr> pOrders;
    User::Ptr   pUser;
    POrder::Ptr pOrder;
    std::stringstream ss;

    ss << "SELECT * FROM POrder WHERE " << POrder::PORDER_USER_ID << " = " << iUserId << " ORDER BY " << POrder::PORDER_ORDR_TM << " DESC;";
    SQLite::Statement query(*m_hDB, ss.str());

    int iLoop = 0;
    while(query.executeStep()) {
        pOrder  = getPOrder(&query);
        pOrders.push_back(pOrder);
        if(++iLoop >= MAX_YOUR_ORDERS) break;
    }
    return pOrders;
}

std::vector<POrder::Ptr> DBInterface::getOrderByStatus(CartStatus crtStat, OrderType orderType, FILE *fp) {
    std::vector<POrder::Ptr> pOrders;
    User::Ptr   pUser;
    POrder::Ptr pOrder;
    std::stringstream ss;

    ss << "SELECT * FROM POrder WHERE " << POrder::PORDER_STATUS << " = " << getIntStatus(crtStat) << ";";

    if(CartStatus::PAYMENT_PENDING == crtStat || CartStatus::READY_FOR_DELIVERY == crtStat || CartStatus::CANCELLED == crtStat) {
        ss.str("");
        ss << "SELECT * FROM POrder WHERE " << POrder::PORDER_STATUS << " = " << getIntStatus(crtStat) << " ORDER BY " << POrder::PORDER_ORDR_TM << " DESC;";
    }
    if(CartStatus::DELIVERED == crtStat) {
        ss.str("");
        ss << "SELECT * FROM POrder WHERE " << POrder::PORDER_STATUS << " = " << getIntStatus(crtStat) << " ORDER BY " << POrder::PORDER_DLVR_TM << " DESC;";
    }

    SQLite::Statement query(*m_hDB, ss.str());

    unsigned int iLoop = 0;
    unsigned int ordrRtpup  = (OrderType::PORDER == orderType) ? 1 : 0;
    while(query.executeStep()) {
        pOrder  = getPOrder(&query);
        if(ordrRtpup == (pOrder->m_OrderNo % 2)) {
            pOrders.push_back(pOrder);
            if(++iLoop >= MAX_YOUR_ORDERS) break;
        }
    }
    return pOrders;
}

void DBInterface::clearAllCartedItems(FILE *fp) {
    std::stringstream ss;
    SQLite::Transaction transaction(*m_hDB);

    //  Clear all carted items. We are done for the day.
    ss.str(std::string());
    ss << "DELETE FROM Cart WHERE " << Cart::CART_STATUS << " = " << getIntStatus(CartStatus::CARTED) << ";";
    m_hDB->exec(ss.str());

    transaction.commit();
}

void DBInterface::updateAllDelivered(FILE *fp) {
    std::stringstream ss;
    SQLite::Transaction transaction(*m_hDB);

    ss.str(std::string());
    ss << "UPDATE Cart SET " << Cart::CART_STATUS << " = " << getIntStatus(CartStatus::DELIVERED) << " WHERE "
                << Cart::CART_ORDER_NO << " IN (SELECT " << POrder::PORDER_NO << " FROM POrder WHERE "
                << POrder::PORDER_STATUS << " = " << getIntStatus(CartStatus::READY_FOR_DELIVERY) << " AND ("
                << POrder::PORDER_NO << " % 2) != 0 AND "
                << "SUBSTR(" << POrder::PORDER_ORDR_TM << ", 1, 10) = \"" << getYstrDate() << "\");";
    m_hDB->exec(ss.str());

    ss.str(std::string());
    ss << "UPDATE POrder SET "
            << POrder::PORDER_STATUS << " = " << getIntStatus(CartStatus::DELIVERED) << ", "
            << POrder::PORDER_DLVR_TM << " = \"" << getCurTime()
            << "\" WHERE "
                << POrder::PORDER_STATUS << " = " << getIntStatus(CartStatus::READY_FOR_DELIVERY) << " AND ("
                << POrder::PORDER_NO << " % 2) != 0 AND "
                << "SUBSTR(" << POrder::PORDER_ORDR_TM << ", 1, 10) = \"" << getYstrDate() << "\";";
    m_hDB->exec(ss.str());
    transaction.commit();
}

int DBInterface::getAllOutstanding(FILE *fp) {
    std::stringstream ss;
    int iOutstanding = 0;

    ss << "SELECT SUM(" << User::USER_WBALANCE << ") FROM User WHERE 0 > " << User::USER_WBALANCE << ";";
    SQLite::Statement query(*m_hDB, ss.str());

    if(query.executeStep()) {
        ss.str(std::string()); ss << "SUM(" << User::USER_WBALANCE << ")";
        iOutstanding = query.getColumn(ss.str().c_str()).getUInt();
    }
    return iOutstanding;
}

std::map<unsigned int, unsigned int> DBInterface::getOrderSummary(FILE *fp) {
    std::map<unsigned int, unsigned int> ordrSmry;
    Cart::Ptr pCart;
    std::stringstream ss;

    ss << "SELECT * FROM Cart WHERE " << Cart::CART_STATUS << " = " << getIntStatus(CartStatus::READY_FOR_DELIVERY) << ";";
    SQLite::Statement query(*m_hDB, ss.str());

    while(query.executeStep()) {
        pCart   = getCart(&query);
        if(pCart) ordrSmry[pCart->m_ProductId] += pCart->m_Qnty;
    }
    return ordrSmry;
}

/*
 *         static std::string PORDER_ID;
    static std::string PORDER_NO;
    static std::string PORDER_USER_ID;
    static std::string PORDER_DATE;
    static std::string PORDER_TIME;
    static std::string PORDER_PAY_GW;
    static std::string PORDER_OTP;
    static std::string PORDER_STATUS;
    static std::string PORDER_ADDRESS;
 *
 * unsigned int DBInterface::addAddressToShipping(unsigned int iUserId, std::string strAddress, FILE *fp) {
    std::stringstream ss;
    unsigned int iFound = 0;

    ss << "SELECT * from Shipping WHERE " << Shipping::SHIPPING_USER_ID << " = " << iUserId;
    SQLite::Statement query(*m_hDB, ss.str());
    while(query.executeStep()) {
        std::string strOld  = query.getColumn(Shipping::SHIPPING_ADDRESS.c_str()).getString();
        if(!strAddress.compare(strOld)) {
            iFound = query.getColumn(Shipping::SHIPPING_ID.c_str()).getInt();
            break;
        }
    }
    if(!iFound) {
        SQLite::Transaction transaction(*m_hDB);
        ss.str(std::string());
        ss << "INSERT INTO Shipping (" <<
                Shipping::SHIPPING_USER_ID << ", " <<
                Shipping::SHIPPING_ADDRESS << ") VALUES (" <<
                iUserId << ",\"" << strAddress << "\");";
        fprintf(fp, "DB - Inserting address %s\n", ss.str().c_str()); fflush(fp);
        m_hDB->exec(ss.str());
        iFound = m_hDB->getLastInsertRowid();
        transaction.commit();
    }
    return iFound;
}*/



