/*sgn
 * DBInterface.h
 *
 *  Created on: 20-May-2020
 *      Author: tstone10
 */

#ifndef DBINTERFACE_H_
#define DBINTERFACE_H_


#include <iostream>
#include <vector>
#include <memory>

#include <SQLiteCpp/SQLiteCpp.h>
#include <tgbot/tgbot.h>

#define ORGANIC_SOAP_ID_01  (79)
#define ORGANIC_SOAP_ID_02  (80)
#define ORGANIC_SOAP_ID_03  (81)
#define ORGANIC_SOAP_ID_04  (81)
#define ORGANIC_SOAP_ID_05  (83)
#define ORGANIC_SOAP_ID_06  (84)

#define SECS_IN_A_DAY   (60 * 60 * 24)

enum class OrderType{ PORDER, TOPUP, NOTA };

enum class CartStatus{CARTED, PAYMENT_PENDING, READY_FOR_DELIVERY, DELIVERED, CANCELLED, NOTA};

class User {
public:
    typedef std::shared_ptr<User> Ptr;
    unsigned int m_UserId, m_OrderNo, m_TransacNo;
    int64_t m_ChatId;
    int m_WBalance;
    unsigned int m_ProdId, m_isActive;
    unsigned long long m_Mobile;
    std::string m_Name, m_Address;

    User() : m_UserId {0}, m_ChatId {0}, m_OrderNo {0}, m_WBalance(0), m_ProdId {0}, m_Mobile {0}, m_TransacNo {0} {}

    static std::string USER_ID;
    static std::string USER_NAME;
    static std::string USER_CHAT_ID;
    static std::string USER_PROD_ID;
    static std::string USER_ORDER_NO;
    static std::string USER_ADDRESS;
    static std::string USER_WBALANCE;
    static std::string USER_TRANSAC_NO;
};

class Viewers {
public:
    typedef std::shared_ptr<Viewers> Ptr;
    unsigned int m_ViewerId, m_UserId;
    std::string m_Date, m_Time;

    static std::string VIEWER_ID;
    static std::string VIEWER_USER_ID;
    static std::string VIEWER_DATE;
    static std::string VIEWER_TIME;
};

class Category {
public:
    typedef std::shared_ptr<Category> Ptr;
    unsigned int m_CategoryId;
    std::string m_Name, m_Code, m_Package;

    static std::string CATEGORY_ID;
    static std::string CATEGORY_NAME;
    static std::string CATEGORY_CODE;
    static std::string CATEGORY_PACKAGE;
};

class Product {
public:
    typedef std::shared_ptr<Product> Ptr;
    unsigned int m_ProductId, m_Price;
    std::string m_Name, m_Code, m_Desc, m_PicFile, m_Pack, m_Date;

    static std::string PRODUCT_ID;
    static std::string PRODUCT_CODE;
    static std::string PRODUCT_NAME;
    static std::string PRODUCT_PIC_FILE;
    static std::string PRODUCT_PRICE;
    static std::string PRODUCT_PACK;
    static std::string PRODUCT_DESC;
    static std::string PRODUCT_DATE;
};

class POrder {
public:
    typedef std::shared_ptr<POrder> Ptr;
    unsigned int m_OrderId, m_OrderNo, m_UserId, m_Amt;
    std::string m_OrdrTm, m_DlvrdTm, m_PayGW, m_Address, m_Name;
    CartStatus m_Status;
    int m_WBalance;

    static std::string PORDER_ID;
    static std::string PORDER_USER_NAME;
    static std::string PORDER_NO;
    static std::string PORDER_USER_ID;
    static std::string PORDER_ORDR_TM;
    static std::string PORDER_DLVR_TM;
    static std::string PORDER_PAY_GW;
    static std::string PORDER_AMOUNT;
    static std::string PORDER_STATUS;
    static std::string PORDER_ADDRESS;
    static std::string PORDER_WBALANCE;
};

class Cart {
public:
    typedef std::shared_ptr<Cart> Ptr;
    unsigned int m_CartId, m_ProductId, m_UserId, m_Qnty, m_OrderNo, m_Price;
    CartStatus m_Status;

    static std::string CART_ID;
    static std::string CART_PRODUCT_ID;
    static std::string CART_PRODUCT_PRICE;
    static std::string CART_USER_ID;
    static std::string CART_STATUS;
    static std::string CART_QNTY;
    static std::string CART_ORDER_NO;
};

class Shipping {
public:
    typedef std::shared_ptr<Shipping> Ptr;
    unsigned int m_ShippingId, m_UserId;
    std::string m_Address;

    static std::string SHIPPING_ID;
    static std::string SHIPPING_USER_ID;
    static std::string SHIPPING_ADDRESS;
};

class Stars {
public:
    typedef std::shared_ptr<Stars> Ptr;
    unsigned int m_StarId, m_UserId, m_ProductGroupId, m_NoOfStars;
    std::string m_Date, m_Time;

    static std::string STAR_ID;
    static std::string STAR_USER_ID;
    static std::string STAR_DATE;
    static std::string STAR_TIME;
    static std::string STAR_PRODUCT_GROUP_ID;
    static std::string STAR_NO_OF_STARS;
};

class Reviews {
public:
    typedef std::shared_ptr<Reviews> Ptr;
    unsigned int m_ReviewId, m_UserId, m_ProductGroupId;
    std::string m_Date, m_Time, m_Review;

    static std::string REVIEW_ID;
    static std::string REVIEW_USER_ID;
    static std::string REVIEW_DATE;
    static std::string REVIEW_TIME;
    static std::string REVIEW_PRODUCT_GROUP_ID;
    static std::string REVIEW_REVIEW;
};

class Notifs {
public:
    typedef std::shared_ptr<Notifs> Ptr;
    unsigned int m_NotifId, m_UserId;
    int64_t m_ChatId;
    std::string m_Name, m_Msg;

    static std::string NOTIF_ID;
    static std::string NOTIF_USER_ID;
    static std::string NOTIF_CHAT_ID;
    static std::string NOTIF_NAME;
    static std::string NOTIF_MSG;
};

class DBInterface {
    std::shared_ptr<SQLite::Database> m_hDB;
    FILE *m_Fp;

    std::vector<POrder::Ptr> getPOrdersForQuery(SQLite::Statement& query);

    User::Ptr       getUser(SQLite::Statement *pQuery);
    Category::Ptr   getCategory(SQLite::Statement *pQuery);
    POrder::Ptr     getPOrder(SQLite::Statement *pQuery);
    Product::Ptr    getProduct(SQLite::Statement *pQuery);
    Cart::Ptr       getCart(SQLite::Statement *pQuery);
    Notifs::Ptr     getNotif(SQLite::Statement *pQuery);

public:
    typedef std::shared_ptr<DBInterface> Ptr;

    DBInterface(std::string dbFileName, FILE *fp);
    ~DBInterface();

    std::string     getCurTime();
    std::string     getYstrDate();
    std::string     getTmrwDate();
    std::string     getTmrwDtTmSecsMilli();

    unsigned int generateOrderNo(OrderType orderType, FILE *fp);
    void updateOrderNo(unsigned int iUserId, FILE *fp);
    void updateTransacNo(unsigned int iUserId, FILE *fp);
    bool addNewUser(int64_t chatId, std::string fname, FILE *fp);
    void updateLeftUser(int64_t iChatId, FILE *fp);
	void deactivate(int64_t iChatId, FILE *fp);
    std::vector<User::Ptr> getAllUsers(FILE *fp);
    User::Ptr getUserForChatId(int64_t iChatId, FILE *fp);
    User::Ptr getUserForUserId(unsigned int iUserId, FILE *fp);
    User::Ptr getUserForOrderNo(unsigned int iOrderNo, FILE *fp);
    int getNoOfUsers(FILE *fp);
    int getIntStatus(CartStatus stat);
    int getWalletBalance(unsigned int iUserId, FILE *fp);
    User::Ptr forceWalletBalance(std::string strUserId, std::string strAmt, FILE *fp);
//    std::vector<std::string> getAddressesForUser(unsigned int iUserId, FILE *fp);

    void updateNotifications(std::map<int64_t, std::string> notifs, FILE *fp);
    std::vector<Notifs::Ptr> getNotifications(FILE *fp);
    void removeNotif(int64_t iNotifId, FILE *fp);

    std::vector<std::string> getCategories(FILE *fp);
    Category::Ptr getCategoryForCode(std::string strCode, FILE *fp);
    Category::Ptr getCategoryForName(std::string strName, FILE *fp);
    Category::Ptr getCategoryById(int catId, FILE *fp);

    bool insertNewProduct(std::string strCat, std::string strName, std::string strPrice, FILE *fp);
    std::vector<Product::Ptr> getActvProdsToSearch(FILE *fp);
    std::vector<Product::Ptr> getAllActiveProducts(FILE *fp);
    std::vector<Product::Ptr> getAllProducts(FILE *fp);
    void updateStock(unsigned int iProdId, std::string strQty, FILE *fp);
    Product::Ptr getProductForCode(std::string strCode, FILE *fp);
    void activateProductForTomorrow(unsigned int iProdId, FILE *fp);
    void removeProductFromTomorrow(unsigned int iProdId, FILE *fp);

    Product::Ptr getProductById(unsigned int iProdId, FILE *fp);
    Product::Ptr getProductByName(std::string strName, FILE *fp);

    void retainProdId(int64_t iChatId, unsigned int iProdId, FILE *fp);
    unsigned int getChosenProduct(int64_t iChatId, FILE *fp);

    std::vector<Product::Ptr> getProductsForCategory(std::string strCat, FILE *fp);
    std::vector<Product::Ptr> getOneProductForCode(std::string strCode, FILE *fp);
    std::vector<Product::Ptr> getOneProductForName(std::string strName, FILE *fp);
    bool updateProductPrice(std::string strPriceId, std::string strPrice, FILE *fp);

    int addProductToCart(unsigned int iProdId, unsigned qty, unsigned int iPrice, int64_t chatId, FILE *fp);
	bool isProdCarted(unsigned int iProdId, unsigned int iOrderNo, FILE *fp);
    bool reduceCartQty(int iProdId, unsigned int iOrderNo, FILE *fp);
    bool removeItemFromCart(int iProdId, unsigned int iOrderNo, FILE *fp);
    bool incrementItemQty(int iProdId, unsigned int iOrderNo, FILE *fp);
    std::vector<Cart::Ptr> getCartItemsForOrderNo(unsigned int iOrderNo, FILE *fp);
    bool emptyCartForUser(unsigned int iOrderNo, FILE *fp);
    std::vector<User::Ptr> getCartedUsers(FILE *fp);

    unsigned int addAddressToShipping(unsigned int iUserId, std::string strAddress, FILE *fp);
    void insertToOrder(User::Ptr pUser, unsigned int iAmt, CartStatus crtStatus, std::string strGW, OrderType ordrTyp, FILE *fp);
    void updateOrderStatus(int iOrderNo, CartStatus crtStatus, OrderType ordrTyp, FILE *fp);

    void confirmTopUpAmount(unsigned int iOrderNo, unsigned int iAmount, FILE *fp);
    bool isAnyPendingOrders(FILE *fp);
    POrder::Ptr getOrderForOrderNo(unsigned int iOrderNo, FILE *fp);
    std::vector<POrder::Ptr> getOrderByStatus(CartStatus crtStat, OrderType orderType, FILE *fp);
    std::vector<POrder::Ptr> getOrdersByUser(unsigned int iUserId, FILE *fp);
    void updateAllDelivered(FILE *fp);
    void clearAllCartedItems(FILE *fp);
    int getAllOutstanding(FILE *fp);
    std::map<unsigned int, unsigned int> getOrderSummary(FILE *fp);
    void setPaymentLink(unsigned int iOrderNo, std::string strPaymentLink, FILE *fp);
    std::string getPaymentLink(unsigned int iOrderNo, FILE *fp);
    int getLeftUserWBal(int64_t iChatId, FILE *fp);
	std::string updateDeliveryCharge(unsigned int iOrderNo, unsigned int iAmt, FILE *fp);
	std::string updateUserAddress(int iUserId, std::string strAddress, FILE *fp);
};


#endif /* DBINTERFACE_H_ */
