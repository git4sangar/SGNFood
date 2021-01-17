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

//------------------------------------AurA------------------------------------
#ifdef AURA
    #define BOT_TOKEN	    "655366902:AAGEaKH0q1xXu0Lwd5C5HBDZsUnPTjn13X4"
    #define BOT_DB_FILE     "sgn_aura.db"
    #define BOT_LOG_FILE    "log_file_aura.log"
    #define PAYTM_MOBILE    "99401 30135"
    #define GPAY_MOBILE     "99401 30135"
    #define PHONE_PE_NO     "99401 30135"
    #define WHATSAPP_NO     "99401 30135"
    #define BUSINESS_NAME   "AurA Organic Soaps"
    #define CASH_TEST_APP_ID        "3154900f9ad69d4ef8377bff894513"
    #define CASH_FREE_APP_ID        "760934e74bc9d12ac17abdb5439067"

    #define CASH_TEST_SECRET_KEY    "fbaf05426f3146c990630fd897db57e2d7ae7ddd"
    #define CASH_FREE_SECRET_KEY    "db7f536560a49628679db2d6926aeb4b7cedd374"

    #define BOT_MAIL                "thrayaa@gmail.com"
    #define REDIRECT_URL            "http://ohobot.com//"
    //#define CASH_FREE_URL           "https://test.cashfree.com/"
    #define CASH_FREE_URL         "https://api.cashfree.com/"

    #ifdef _VM_
        #define BIN_FILE_PATH       "/home/svaithiy/sgn/projs/SGNFood/bin/AuraBot"
        #define BOT_ROOT_PATH       "/home/svaithiy/sgn/projs/SGNFood/aura/"
    #endif

    #ifdef _Pi_
        #define BIN_FILE_PATH     "/home/sgn/sgn/projs/SGNFood/bin/AuraBot"
        #define BOT_ROOT_PATH     "/home/sgn/sgn/projs/SGNFood/aura/"
    #endif
#endif

//------------------------------------Mani Mama------------------------------------
#ifdef MANI_MAMA
    #define BOT_TOKEN       "1351042610:AAGlnvR26ZsX7ElSyU8l3PwIcJ8GMs48N0Q"
    #define BOT_DB_FILE     "sgn_mani.db"
    #define BOT_LOG_FILE    "log_file_mani.log"
    #define PAYTM_MOBILE    "93410 66700"
    #define GPAY_MOBILE     "93410 66700"
    #define PHONE_PE_NO     "93410 66700"
    #define WHATSAPP_NO     "97419 83633"
    #define BUSINESS_NAME   "Mani Iyer's Kitchen"
    #define CASH_FREE_APP_ID          "aura_app_id"
    #define CASH_FREE_SECRET_KEY      "aura_secret_key"
    #define REDIRECT_URL                "http://ohobot.com/"
    #define BOT_MAIL                    "bot@bot.com"
    #define CASH_FREE_URL               "https://test.cashfree.com/"
    //#define CASH_FREE_URL             "https://api.cashfree.com/"

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

#define BOT_DB_PATH     "db/"
#define BOT_LOG_PATH    "logs/"

#define BOT_FONT_PATH       "fonts/"
#define BOT_FONT_FILE_BOLD  "Raleway-Bold.ttf"
#define BOT_FAQ_FILE        "faq.txt"

#define BOT_ASSETS_PATH     "assets/"

#define ONE_KB              (1024)
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

#define STR_WALLET  "Wallet"

//  Commands to process
#define STR_BTN_ABOUT_US        "About Us"
#define STR_BTN_ADMIN_PG        "Admin Page"
#define STR_BTN_MENU_MGMT       "Menu Mgmt"
#define STR_BTN_PARSER          "Parser"
#define STR_BTN_FUND_ME         "Diwali Bakshanam"
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
#define STR_BTN_ALL_DLVRD       "All Dlvrd"
#define STR_BTN_YOUR_ORDERS     "A/C Stmt"

#define STR_BTN_TOP_UP          "Top Up"
#define STR_BTN_CNF_CHECKOUT    "Confirm Checkout"

#define STR_BTN_NEW_TOPUPs      "New TopUps"
#define STR_BTN_CNF_TOPUPs      "Confirmed TopUps"
#define STR_BTN_CNCLD_TOPUPs    "Cancelled TopUps"
#define STR_BTN_REMIND_CHKOUT   "Remind Checkout"
#define STR_BTN_FORCE_WALLET    "Force Wallet"
#define STR_BTN_CNFM_R_CNCL_TOPUP     "Cnfm / Cncl TopUp"

#define STR_BTN_SINGLE_ORDER    "Single Order"
#define STR_BTN_ADD_BY_SN       "Add to Cart by SN"
#define STR_BTN_CHANGE_ADDRESS  "Change Address"
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

extern std::map<unsigned int, UserContext> m_Context;
extern std::vector<unsigned int> adminChatIds;
//extern bool isAgent;

extern std::string getAddressNotification();
extern nlohmann::fifo_map<std::string, std::string> descToCode;

#endif /* CONSTANTS_H_ */
