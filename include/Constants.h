/*sng
 * Constants.h
 *
 *  Created on: 14-May-2020
 *      Author: tstone10
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <map>
#include <vector>
#include "fifo_map.h"

#define SGN_BOT_VER "1.1.2"

#define MANI_MAMA 
#define _Pi_


//------------------------------------Mani Mama------------------------------------
#ifdef MANI_MAMA
    #define BOT_TOKEN       "1351042610:AAFJriXJPfpsZs--xaKVKp7kjVf7n7tQr7Q"	//	ManiMama_Bot
    //#define BOT_TOKEN       "2109953794:AAFYLFRhfanODUdjmkYmf8a3o_FC6xKQ1Oc"	//	ManiIyer Bot
    #define BOT_DB_FILE     "sgn_mani_no_git.db"
    //#define BOT_DB_FILE     "sgn_mani.db"
    #define BOT_LOG_FILE    "log_file_mani.log"
    #define PAYTM_MOBILE    "93410 66700"
    #define GPAY_MOBILE     "93410 66700"
    #define PHONE_PE_NO     "93410 66700"
    #define WHATSAPP_NO     "97419 83633"
	#define VENDOR_ID       (2)
    #define BUSINESS_NAME   "Mani Iyer's Kitchen"
    #define REDIRECT_URL                "https://ohobot.com/api/v1/cashfree/response"
    #define BOT_MAIL                    "bot@bot.com"

    #define CASH_TEST_APP_ID        "4948385cd24ff371a0e8c6b7e38494"
    #define CASH_TEST_SECRET_KEY    "e58d6fd7160a61a27acf3369d11a0e1c04eea697"

    #define CASH_FREE_APP_ID        "947884d0d4f82dff5d892238988749"
    #define CASH_FREE_SECRET_KEY    "8c7aa1bc36bc2578111d7dc882a4afb8be031a54"

    //#define CASH_FREE_URL               "https://test.cashfree.com/"
    #define CASH_FREE_URL             "https://api.cashfree.com/"

    #ifdef _VM_
        #define BIN_FILE_PATH       "/home/svaithiy/sgn/projs/SGNFood/bin/ManiBot"
        #define BOT_ROOT_PATH       "/home/svaithiy/sgn/projs/SGNFood/mani_mama/"
    #endif

    #ifdef _Pi_
        #define BIN_FILE_PATH     "/home/sgn/sgn/projs/SGNFood/bin/ManiBot"
        #define BOT_ROOT_PATH     "/home/sgn/sgn/projs/SGNFood/mani_mama/"
    #endif
#endif
#define AURA_BOT        "655366902:AAGEaKH0q1xXu0Lwd5C5HBDZsUnPTjn13X4"
#define THRAYA_BOT      "664320846:AAHffJ_8LG0W8eGCuYsrgkK4fOCj9j2FgTo"

//  CashFree
#define CASH_FREE_BASE_URL      CASH_FREE_URL
#define CASH_FREE_ORDER_API     "api/v1/order/create"
#define CASH_FREE_STATUS_API    "api/v1/order/info/status"
#define CA_CERT_FILE            "cacert.pem"
#define STR_WHATSAPP_LINK		"https://wa.me/"

#define BOT_DB_PATH     "db/"
#define BOT_LOG_PATH    "logs/"
#define BOT_NOTIFS_PATH "notifs/"

#define BOT_FONT_PATH       "fonts/"
#define BOT_FONT_FILE_BOLD  "Raleway-Bold.ttf"
#define BOT_FAQ_FILE        "faq.txt"

#define BOT_ASSETS_PATH     "assets/"

#define PROD_DLVRY_CHARGE	(132)
#define ONE_KB              (1024)
#define TEN_KB				(10*1024)
#define MAX_MOBILE_DIGITS   (10)
#define MAX_WAIT_B4_RESP    (10)
#define MIN_ITEMS_FOR_PNG   (4)
#define MAX_ITEMS_PER_PAGE  (10)
#define MAX_NO_OF_PAGES     (10)

#define MAX_FORCE_AMOUNT    (10000)
#define MAX_ITEM_LENGTH     (20)

#define MAX_PIC_SEND_RETRIES    (3)
#define SKIP_INTERVAL       (5)
#define MAX_BUTTON_ROWS     (5)
#define MAX_USERS           (200000)
#define MAX_USER_ID			(6000)

#define PNG_HDR_FONT_SZ     (12)
#define PNG_CNT_FONT_SZ     (10)

#define PNG_CAT_WIDTH       (225)
#define PNG_PROD_WIDTH      (180)
#define PNG_CODE_WIDTH      (55)

#define PNG_PADDING_X       (5)
#define PNG_PADDING_Y       (5)
#define PNG_HEADER_HEIGHT   (20)
#define PNG_ROW_HEIGHT      (30)
#define PNG_TOTAL_WIDTH     (320)

#define MAX_STARS           (5)
#define MAX_ADDRESSES       (3)
#define MAX_ADD_RMV_BTNS_PER_ROW    (7)
#define MAX_SINGLE_LINE_WIDTH   (40)
#define MAX_ADDRESS_LINES   (4)
#define MAX_YOUR_ORDERS     (100)
#define DISCOUNT_PERCENT    (33)

#define USER_CTXT_NOTA          (100)
#define USER_CTXT_ADDRESS       (101)
#define USER_CTXT_CLEANUP       (102)
#define USER_CTXT_PRICE_CHANGE  (103)
#define USER_CTXT_ADD_A_PROD    (104)
#define USER_CTXT_YOUR_ORDER    (105)
#define USER_CTXT_NEW_ORDER     (106)
#define USER_CTXT_CNF_ORDER     (107)
#define USER_CTXT_DLVRD_ORDER   (108)
#define USER_CTXT_CNCLD_ORDER   (109)
#define USER_CTXT_REVIEW        (110)
#define USER_CTXT_SHOW_PRICE    (111)
#define USER_CTXT_CNFM_CNCL     (112)
#define USER_CTXT_FORCE_WALLET  (113)
#define USER_CTXT_SEND_MSG      (114)
#define USER_CTXT_USER_AC       (115)
#define USER_CTXT_ADMING_MSG    (118)
#define USER_CTXT_DLVRY_CHARGE	(119)
#define USER_CTXT_NEW_ADDRESS	(120)

#define STR_WALLET  "Wallet"

//  Commands to process
#define STR_BTN_ABOUT_US        "About Us"
#define STR_BTN_ADMIN_PG        "Admin Page"
#define STR_BTN_MENU_MGMT       "Menu Mgmt"
#define STR_BTN_PARSER          "Parser"
#define STR_BTN_BAKSHANAM       "Bakshanam"
#define STR_BTN_MSG_ADMIN       "Msg Admin"

#define STR_BTN_CNFM_ORDER      "Confirm Order"
#define STR_BTN_CNCL_ORDER      "Cancel Order"
#define SRT_BTN_DLVR_ORDER      "Deliver Order"
#define STR_BTN_QUICK_MENU      "Quick Menu"
//#define STR_BTN_EDIT_ORDER      "Edit Order"

#define STR_BTN_ITEM_LIST       "Item List"
#define STR_BTN_ADD_PROD        "Add Item"
#define STR_BTN_EDIT_PRICE      "Edit Price"
#define STR_BTN_EDIT_MENU       "Edit Menu"
#define STR_BTN_SHOW_MENU       "Show Menu"
#define STR_BTN_DONE_MENU       "Menu Done"
#define STR_BTN_ORDERS          "Orders"
#define STR_BTN_SEND_MSG        "Send Msg"

//  Category of orders
#define STR_BTN_NEW_ORDERS      "New Orders"
#define STR_BTN_USER_ORDERS     "User Orders"
#define STR_BTN_CNF_ORDERS      "Confirmed Orders"
#define STR_BTN_DLVRD_ORDERS    "Delivered Orders"
#define STR_BTN_CNCLD_ORDERS    "Cancelled Orders"
#define STR_BTN_ORDR_SUMMRY     "Order Summary"
#define STR_BTN_DLVRY_LIST      "Delivery List"
#define STR_BTN_DLVRY_CHARGE    "Dlvry ₹"
#define STR_BTN_ALL_DLVRD       "All Dlvrd"
#define STR_BTN_YOUR_ORDERS     "A/C Stmt"

#define STR_BTN_TOP_UP          "Top Up"
//#define STR_BTN_CNF_CHECKOUT    "Confirm Checkout"

#define STR_BTN_NEW_TOPUPs      "New TopUps"
#define STR_BTN_CNF_TOPUPs      "Confirmed TopUps"
#define STR_BTN_CNCLD_TOPUPs    "Cancelled TopUps"
#define STR_BTN_REMIND_CHKOUT   "Remind Checkout"
#define STR_BTN_FORCE_WALLET    "Force Wallet"
#define STR_BTN_CNFM_R_CNCL_TOPUP     "Cnfm / Cncl TopUp"

#define STR_BTN_SINGLE_ORDER    "Single Order"
#define STR_BTN_ADD_BY_SN       "Add to Cart by SN"
#define STR_BTN_CHANGE_ADDRESS  "Change Address"
#define STR_BTN_UPDATE_ADDRESS  "New Address"
#define STR_BTN_ABOUT           "About"
#define STR_BTN_FAQ             "FAQs"
#define STR_BTN_SHPG_ADDRESS    "Shipping Address"
#define STR_BTN_MAINMENU        "Main Menu"
#define STR_BTN_VIEW_CART       "View Cart"
#define STR_BTN_EMPTY_CART      "Empty Cart"
#define STR_BTN_DELETE          "Del"
#define STR_BTN_REMOVE          "Rmv"
#define STR_BTN_ADD             "Add"
#define STR_BTN_ADD_ALL         "Add all"
#define STR_BTN_CHECKOUT        "Checkout"
#define STR_BTN_PRODUCT_LIST    "Product List"
#define SRT_BTN_SINGLE_ITEM     "Single Item"

//  Description column of Product Table
#define BREAKFAST       "Breakfast"
#define BISIBELEBATH    "Bisibele Bath"
#define SAMBAR          "Sambar"
#define MORKUZHAMBU     "Morkuzhambu"
#define VATHAKUZHAMBU   "Vathakuzhambu"
#define RASAM           "Rasam"
#define CURRY           "Curry"
#define KOOTU           "Kootu"
#define RICE            "Rice"
#define LUNCH           "Full Lunch"
#define SWEET           "Sweet"
#define PICKLE          "Pickle"
#define ULUNDU_VADAI    "Ulundu Vadai(2)"
#define COCONUT_POLI    "Coconut Poli(3)"
#define DHAL_POLI       "Dhall Poli(3)"

//enum class UserContext {ADDRESS, CLEANUP, PRICE_CHANGE, NEW_ORDER, CNF_ORDER, YOUR_ORDER, REVIEW, NOTA};
typedef int UserContext;

extern std::map<int64_t, UserContext> m_Context;
extern std::vector<int64_t> adminChatIds;
//extern bool isAgent;

extern std::string getAddressNotification();
extern std::string getMobileNo(std::string strAddress);
extern bool isMobileNoPresent(std::string strAddress);
extern nlohmann::fifo_map<std::string, std::string> descToCode;

#endif /* CONSTANTS_H_ */
