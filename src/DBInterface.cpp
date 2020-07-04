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
std::string POrder::PORDER_OTP      = "otp";
std::string POrder::PORDER_STATUS   = "status_id";
std::string POrder::PORDER_ADDRESS  = "address";

std::string POrder::PORDER_PAYGW_VAL_PAYTM  = "Paytm";
std::string POrder::PORDER_PAYGW_VAL_GPAY   = "GPay";
std::string POrder::PORDER_PAYGW_VAL_BHIM   = "BHIM";
std::string POrder::PORDER_PAYGW_VAL_CASH   = "Cash";
std::string POrder::PORDER_PAYGW_VAL_NOTA   = "Unknown";

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
        case CartStatus::PAYMENT_PENDING:
            iStat = 0;
            break;
        case CartStatus::READY_FOR_DELIVERY:
            iStat = 1;
            break;
        case CartStatus::DELIVERED:
            iStat = 2;
            break;
        case CartStatus::CANCELLED:
            iStat = 3;
            break;
        case CartStatus::NOTA:
            iStat = 4;
            break;
        default:
            iStat = 5;
    }
    return iStat;
}

unsigned int DBInterface::generateOrderNo(FILE *fp) {
    fprintf(fp, "BaseBot %ld: generateOrderNo\n", time(0)); fflush(fp);
    int invoice_no = 0;
    std::stringstream ss;
    ss << "SELECT MAX(" << User::USER_ORDER_NO << ") from User;";
    SQLite::Statement query(*m_hDB, ss.str());
    if(query.executeStep()) {
        ss.str(std::string()); ss << "MAX(" << User::USER_ORDER_NO << ")";
        invoice_no = query.getColumn(ss.str().c_str()).getUInt();
    }
    return invoice_no + 1;
}

void DBInterface::updateOrderNo(unsigned int iUserId, FILE *fp) {
    std::stringstream ss;
    unsigned int iOrderNo   = generateOrderNo(fp);
    SQLite::Transaction transaction(*m_hDB);
    ss << "UPDATE User set " << User::USER_ORDER_NO << " = " << iOrderNo << " WHERE " << User::USER_ID << " = " << iUserId << ";";
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
        unsigned int order_no = generateOrderNo(fp);
        ss << " INSERT INTO User (" << User::USER_NAME << ", " << User::USER_CHAT_ID << ", " << User::USER_ORDER_NO
                << ") VALUES (\"" << fname << "\", " << chatId << ", " << order_no << ");";
        m_hDB->exec(ss.str());
        transaction.commit();
    } else {
        fprintf(fp, "BaseBot %ld: User %s alreay existing\n", time(0), fname.c_str()); fflush(fp);
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
    return pUser;
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

void DBInterface::addProductsForCodeToCart(std::string strCode, unsigned int chatId, FILE *fp) {
    std::vector<unsigned int> iProdId, iPrice;
    unsigned int iLoop = 0, iNoOfItems = 0;

    //  Get all the products for that code
    std::stringstream ss;
    ss << "SELECT * from Product WHERE " << Product::PRODUCT_CODE << " = \"" << strCode << "\";";
    SQLite::Statement query(*m_hDB, ss.str());
    while(query.executeStep()) {
        iProdId.push_back(query.getColumn(Product::PRODUCT_ID.c_str()).getInt());
        iPrice.push_back(query.getColumn(Product::PRODUCT_PRICE.c_str()).getInt());
    }

    iNoOfItems = iProdId.size();
    for(iLoop = 0; iLoop < iNoOfItems; iLoop++) {
        addProductToCart(iProdId[iLoop], iPrice[iLoop], chatId, fp);
    }
}

int DBInterface::addProductToCart(unsigned int iProdId, unsigned int iPrice, unsigned int chatId, FILE *fp) {
    std::stringstream ss;
    int iQty = 0;
    User::Ptr pUser = getUserForChatId(chatId, fp);
    ss << "SELECT * from Cart WHERE " <<
            Cart::CART_ORDER_NO << " = " << pUser->m_OrderNo <<
                    " AND " <<
            Cart::CART_PRODUCT_ID <<" = " << iProdId << ";";

    SQLite::Statement query(*m_hDB, ss.str());
    if(query.executeStep()) {
        iQty    = query.getColumn(Cart::CART_QNTY.c_str()).getInt();
        SQLite::Transaction transaction(*m_hDB);
        ss.str(std::string());
        ss << "UPDATE Cart set " << Cart::CART_QNTY << " = " << Cart::CART_QNTY << " + 1 WHERE "
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
                iProdId << "," << iPrice << "," << pUser->m_UserId << ", 1, " <<
                getIntStatus(CartStatus::PAYMENT_PENDING) << "," << pUser->m_OrderNo << ");";
        m_hDB->exec(ss.str());
        transaction.commit();
    }
    return iQty+1;
}

bool DBInterface::incrementItemQty(int iProdId, unsigned int iOrderNo, FILE *fp) {
    std::stringstream ss;
    ss << "SELECT * from Cart WHERE " << Cart::CART_ORDER_NO << " = " << iOrderNo <<
                " AND " <<
        Cart::CART_PRODUCT_ID <<" = " << iProdId << ";";

    SQLite::Statement query(*m_hDB, ss.str());
    if(query.executeStep()) {
        SQLite::Transaction transaction(*m_hDB);
        ss.str(std::string());
        ss << "UPDATE Cart set " << Cart::CART_QNTY << " = " << Cart::CART_QNTY << " + 1 WHERE "
                << Cart::CART_PRODUCT_ID << " = " << iProdId << " AND " << Cart::CART_ORDER_NO << " = " << iOrderNo << ";";
        m_hDB->exec(ss.str());
        transaction.commit();
    }
    return true;
}

bool DBInterface::removeProductFromCart(int iProdId, unsigned int iOrderNo, FILE *fp){
    std::stringstream ss;

    ss << "SELECT * from Cart WHERE " <<
        Cart::CART_ORDER_NO << " = " << iOrderNo <<
                " AND " <<
        Cart::CART_PRODUCT_ID <<" = " << iProdId << ";";

    SQLite::Statement query(*m_hDB, ss.str());
    if(query.executeStep()) {
        if(1 < query.getColumn(Cart::CART_QNTY.c_str()).getInt()) {
            SQLite::Transaction transaction(*m_hDB);
            ss.str(std::string());
            ss << "UPDATE Cart set " << Cart::CART_QNTY << " = " << Cart::CART_QNTY << " - 1 WHERE "
                    << Cart::CART_PRODUCT_ID << " = " << iProdId << " AND " << Cart::CART_ORDER_NO << " = " << iOrderNo << ";";
            m_hDB->exec(ss.str());
            transaction.commit();
        } else {
            SQLite::Transaction transaction(*m_hDB);
            ss.str(std::string());
            ss << "DELETE from Cart WHERE " <<
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
    ss << "DELETE from Cart WHERE " << Cart::CART_ORDER_NO << " = " << iOrderNo << ";";
    SQLite::Transaction transaction(*m_hDB);
    m_hDB->exec(ss.str());
    transaction.commit();
    return true;
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

    ss << "SELECT * from Category;";
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
    pProduct->m_Price   = pQuery->getColumn(Product::PRODUCT_PRICE.c_str()).getInt();

    return pProduct;
}

std::vector<Product::Ptr> DBInterface::getAllProducts(FILE *fp) {
    std::vector<Product::Ptr> pProducts;
    Product::Ptr pProduct;
    std::stringstream ss;

    ss << "SELECT * from Product;";
    SQLite::Statement query(*m_hDB, ss.str());
    while(query.executeStep()) {
        pProduct    = getProduct(&query);
        pProducts.push_back(pProduct);
    }
    return pProducts;
}

std::vector<Product::Ptr> DBInterface::getAllActiveProducts(FILE *fp){
    std::vector<Product::Ptr> pProducts;
    std::stringstream ss;
    Product::Ptr pProduct;
    std::string strDate = getCurTime();

    ss << "SELECT * from Product WHERE " << Product::PRODUCT_DATE << " = \"" << getTmrwDate() << "\";";
    SQLite::Statement query(*m_hDB, ss.str());
    while(query.executeStep()) {
        pProduct    = getProduct(&query);
        pProducts.push_back(pProduct);
    }
    return pProducts;
}

void DBInterface::activateProductForTomorrow(unsigned int iProdId, FILE *fp) {
    std::stringstream ss;
    ss << "UPDATE Product set " << Product::PRODUCT_DATE <<  " = \"" << getTmrwDate() << "\" WHERE "
            << Product::PRODUCT_ID << " = " << iProdId << ";";
    SQLite::Transaction transaction(*m_hDB);
    m_hDB->exec(ss.str());
    transaction.commit();
}

void DBInterface::removeProductFromTomorrow(unsigned int iProdId, FILE *fp) {
    std::stringstream ss;
    ss << "UPDATE Product set " << Product::PRODUCT_DATE <<  " = \"1970-01-01\" WHERE "
            << Product::PRODUCT_ID << " = " << iProdId << ";";
    SQLite::Transaction transaction(*m_hDB);
    m_hDB->exec(ss.str());
    transaction.commit();
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

    ss << "SELECT * from Product WHERE " << Product::PRODUCT_ID << " = " << iProdId << ";";
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

    ss << "SELECT * from Product WHERE " << Product::PRODUCT_NAME << " = \"" << strName << "\";";
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

    ss << "SELECT * from Product WHERE " << Product::PRODUCT_CODE << " = \"" << strCode << "\";";
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
    ss << "UPDATE Product set " << Product::PRODUCT_PRICE << " = " << iPrice << " WHERE "
            << Product::PRODUCT_ID << " = " << iProdId << ";";
    int iRows   = m_hDB->exec(ss.str());
    if(0 < iRows)   transaction.commit();
    return (0 < iRows);
}

unsigned int DBInterface::addAddressToShipping(unsigned int iUserId, std::string strAddress, FILE *fp) {
    std::stringstream ss;
    SQLite::Transaction transaction(*m_hDB);
    ss << "UPDATE User set " << User::USER_ADDRESS << " = \"" << strAddress << "\" WHERE "
            << User::USER_ID << " = " << iUserId << ";";
    m_hDB->exec(ss.str());
    transaction.commit();
    return 0;
}

void DBInterface::updateOrderStatus(int iOrderNo, CartStatus crtStatus, FILE *fp) {
    std::stringstream ss;
    SQLite::Transaction transaction(*m_hDB);
    ss.str(std::string());
    ss << "UPDATE POrder set " << POrder::PORDER_STATUS << " = " << getIntStatus(crtStatus) << " WHERE "
            << POrder::PORDER_NO << " = " << iOrderNo << ";";
    if(CartStatus::DELIVERED == crtStatus) {
        ss.str(std::string());ss << "UPDATE POrder set " <<
                POrder::PORDER_STATUS << " = " << getIntStatus(crtStatus) << ", " <<
                POrder::PORDER_DLVR_TM << " = \"" << getCurTime() <<
                "\" WHERE " << POrder::PORDER_NO << " = " << iOrderNo << ";";
    }
    m_hDB->exec(ss.str());
    transaction.commit();
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
    strftime(buffer, 64, "%Y-%m-%d  -  %H:%M", localtime(&t));
    return std::string(buffer);
}

void DBInterface::insertToOrder(User::Ptr pUser, CartStatus crtStatus, std::string strGW, FILE *fp) {
    std::stringstream ss;
    time_t t = time(NULL);
    SQLite::Transaction transaction(*m_hDB);
    ss.str(std::string());
    ss << "INSERT INTO POrder (" <<
            POrder::PORDER_NO << ", " <<
            POrder::PORDER_USER_ID << ", " <<
            POrder::PORDER_USER_NAME << ", " <<
            POrder::PORDER_ORDR_TM << ", " <<
            POrder::PORDER_DLVR_TM << ", " <<
            POrder::PORDER_PAY_GW << ", " <<
            POrder::PORDER_STATUS << ", " <<
            POrder::PORDER_ADDRESS << ") VALUES (" <<
            pUser->m_OrderNo << ", " << pUser->m_UserId << ", \"" << pUser->m_Name
            << "\", \"" << getCurTime() << "\", \"-\", \"" << strGW << "\", " <<
            getIntStatus(crtStatus) << ", \"" << pUser->m_Address << "\" );";
    m_hDB->exec(ss.str());
    transaction.commit();
}

POrder::Ptr DBInterface::getPOrder(SQLite::Statement *pQuery) {
    POrder::Ptr pOrder  = std::make_shared<POrder>();
    pOrder->m_OTP       = pQuery->getColumn(POrder::PORDER_OTP.c_str()).getInt();
    pOrder->m_OrderId   = pQuery->getColumn(POrder::PORDER_ID.c_str()).getInt();
    pOrder->m_OrderNo   = pQuery->getColumn(POrder::PORDER_NO.c_str()).getInt();
    pOrder->m_UserId    = pQuery->getColumn(POrder::PORDER_USER_ID.c_str()).getInt();
    pOrder->m_Name      = pQuery->getColumn(POrder::PORDER_USER_NAME.c_str()).getString();
    pOrder->m_OrdrTm    = pQuery->getColumn(POrder::PORDER_ORDR_TM.c_str()).getString();
    pOrder->m_DlvrdTm   = pQuery->getColumn(POrder::PORDER_DLVR_TM.c_str()).getString();
    pOrder->m_PayGW     = pQuery->getColumn(POrder::PORDER_PAY_GW.c_str()).getString();
    pOrder->m_Address   = pQuery->getColumn(POrder::PORDER_ADDRESS.c_str()).getString();
    pOrder->m_Status    = static_cast<CartStatus>(pQuery->getColumn(POrder::PORDER_STATUS.c_str()).getInt());
    return pOrder;
}

POrder::Ptr DBInterface::getOrderForOrderNo(unsigned int iOrderNo, FILE *fp) {
    POrder::Ptr pOrder = nullptr;
    std::stringstream ss;

    ss << "SELECT * from POrder WHERE " << POrder::PORDER_NO << " = " << iOrderNo << ";";
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

    ss << "SELECT * from POrder WHERE " << POrder::PORDER_USER_ID << " = " << iUserId << " ORDER BY " << POrder::PORDER_NO << " DESC;";
    SQLite::Statement query(*m_hDB, ss.str());

    int iLoop = 0;
    while(query.executeStep()) {
        pOrder  = getPOrder(&query);
        pOrders.push_back(pOrder);
        if(++iLoop >= MAX_YOUR_ORDERS) break;
    }
    return pOrders;
}

std::vector<POrder::Ptr> DBInterface::getOrderByStatus(CartStatus crtStat, FILE *fp) {
    std::vector<POrder::Ptr> pOrders;
    User::Ptr   pUser;
    POrder::Ptr pOrder;
    std::stringstream ss;

    ss << "SELECT * from POrder WHERE " << POrder::PORDER_STATUS << " = " << getIntStatus(crtStat) << ";";

    if(CartStatus::PAYMENT_PENDING == crtStat || CartStatus::READY_FOR_DELIVERY == crtStat || CartStatus::CANCELLED == crtStat) {
        ss.str("");
        ss << "SELECT * from POrder WHERE " << POrder::PORDER_STATUS << " = " << getIntStatus(crtStat) << " ORDER BY " << POrder::PORDER_ORDR_TM << " DESC;";
    }
    if(CartStatus::DELIVERED == crtStat) {
        ss.str("");
        ss << "SELECT * from POrder WHERE " << POrder::PORDER_STATUS << " = " << getIntStatus(crtStat) << " ORDER BY " << POrder::PORDER_DLVR_TM << " DESC;";
    }

    SQLite::Statement query(*m_hDB, ss.str());

    int iLoop = 0;
    while(query.executeStep()) {
        pOrder  = getPOrder(&query);
        pOrders.push_back(pOrder);
        if(++iLoop >= MAX_YOUR_ORDERS) break;
    }
    return pOrders;
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



