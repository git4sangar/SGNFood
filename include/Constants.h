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

#define SGN_BOT_VER "1.0.0"

#define MANI_MAMA
#define _VM_

//------------------------------------Veggies Daily------------------------------------
#ifdef VEGGIES
    #define BOT_TOKEN       "1094369057:AAE2fqPhC1xcYuEeYIIMIcUtxG7-TH7EhzA"
    #define BOT_DB_FILE     "sgn_veggies.db"
    #define BOT_LOG_FILE    "log_file_veggies.log"
    #define PAYTM_MOBILE    "99402 06626"
    #define GPAY_MOBILE     "99402 06626"
    #define BHIM_UPI        "99402 06626@ybl"

    #ifdef _VM_
        #define BIN_FILE_PATH       "/home/svaithiy/sgn/projs/SGNBot/bin/VeggiesBot"
        #define BOT_ROOT_PATH       "/home/svaithiy/sgn/projs/SGNBot/veggies_daily/"
    #endif

    #ifdef _Pi_
        #define BIN_FILE_PATH     "/home/pi/sgn/projs/SGNRetailerBot/bin/VeggiesBot"
        #define BOT_ROOT_PATH     "/home/pi/sgn/projs/SGNRetailerBot/veggies_daily/"
    #endif
#endif

//------------------------------------Mani Mama------------------------------------
#ifdef MANI_MAMA
    // Rajaveni: Deleted: #define BOT_TOKEN       "1240037726:AAFG227jcLfO_7tTsredLEE29R1SzAdDZ7c"
    #define BOT_TOKEN	"1351042610:AAGlnvR26ZsX7ElSyU8l3PwIcJ8GMs48N0Q"
    #define BOT_DB_FILE     "sgn_mani.db"
    #define BOT_LOG_FILE    "log_file_mani.log"
    #define PAYTM_MOBILE    "98406 25165"
    #define GPAY_MOBILE     "98406 25165"
    #define BHIM_UPI        "98406 25165@okaxis"

    #ifdef _VM_
        #define BIN_FILE_PATH       "/home/svaithiy/sgn/projs/SGNFood/bin/ManiBot"
        #define BOT_ROOT_PATH       "/home/svaithiy/sgn/projs/SGNFood/mani_mama/"
    #endif

    #ifdef _Pi_
        #define BIN_FILE_PATH     "/home/pi/sgn/projs/SGNFood/bin/ManiBot"
        #define BOT_ROOT_PATH     "/home/pi/sgn/projs/SGNFood/mani_mama/"
    #endif
#endif

//------------------------------------Chekk Boys------------------------------------
#ifdef CHEKKBOYS
    #define BOT_TOKEN       "1089935460:AAHqTEk2AX0hbmhHIer-ydcelqTTDru3I3Q"
    #define BOT_DB_FILE     "sgn_chekkboys.db"
    #define BOT_LOG_FILE    "log_file_chekkboys.log"
    #define GPAY_MOBILE     "98406 25165"
    #define PAYTM_MOBILE    "98406 25165"

    #ifdef _VM_
        #define BIN_FILE_PATH       "/home/svaithiy/sgn/projs/SGNBot/bin/ChekkboysBot"
        #define BOT_ROOT_PATH       "/home/svaithiy/sgn/projs/SGNBot/chekkboys/"
    #endif

    #ifdef _Pi_
        #define BIN_FILE_PATH     "/home/pi/sgn/projs/SGNRetailerBot/bin/ChekkboysBot"
        #define BOT_ROOT_PATH     "/home/pi/sgn/projs/SGNRetailerBot/chekkboys/"
    #endif
#endif

#define AURA_BOT        "655366902:AAGEaKH0q1xXu0Lwd5C5HBDZsUnPTjn13X4"
#define THRAYA_BOT      "664320846:AAHffJ_8LG0W8eGCuYsrgkK4fOCj9j2FgTo"

#define BOT_DB_PATH     "db/"
#define BOT_LOG_PATH    "logs/"

#define BOT_FONT_PATH       "fonts/"
#define BOT_FONT_FILE_BOLD  "Raleway-Bold.ttf"
#define BOT_FAQ_FILE        "faq.txt"

#define BOT_ASSETS_PATH     "assets/"

#define MAX_MOBILE_DIGITS   (10)
#define MAX_WAIT_B4_RESP    (10)
#define MIN_ITEMS_FOR_PNG   (4)
#define MAX_ITEMS_PER_PAGE  (10)
#define MAX_NO_OF_PAGES     (10)

#define MAX_PIC_SEND_RETRIES    (3)
#define SKIP_INTERVAL       (5)
#define MAX_BUTTON_ROWS     (5)

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
#define MAX_ADDRESS_LINES   (3)
#define MAX_YOUR_ORDERS     (100)

#define USER_CTXT_NOTA          (100)
#define USER_CTXT_ADDRESS       (101)
#define USER_CTXT_CLEANUP       (102)
#define USER_CTXT_PRICE_CHANGE  (103)
#define USER_CTXT_YOUR_ORDER    (104)
#define USER_CTXT_NEW_ORDER     (105)
#define USER_CTXT_CNF_ORDER     (106)
#define USER_CTXT_DLVRD_ORDER   (107)
#define USER_CTXT_CNCLD_ORDER   (108)
#define USER_CTXT_REVIEW        (109)
#define USER_CTXT_SHOW_PRICE    (110)

#define STR_GPAY    "GPay"
#define STR_PAYTM   "PayTM"
#define STR_BHIM    "BHIM"
#define STR_CASH    "Cash"

//  Commands to process
#define STR_BTN_CNFM_ORDER      "Confirm Order"
#define STR_BTN_CNCL_ORDER      "Cancel Order"
#define SRT_BTN_DLVR_ORDER      "Deliver Order"
#define STR_BTN_PRINT_ORDER     "Print Order"
#define STR_BTN_EDIT_ORDER      "Edit Order"

#define STR_BTN_MENU_MGMT       "Menu Mgmt"
#define STR_BTN_ITEM_LIST       "Item List"
#define STR_BTN_EDIT_PRICE      "Edit Price"
#define STR_BTN_EDIT_MENU       "Edit Menu"
#define STR_BTN_SHOW_MENU       "Show Menu"
#define STR_BTN_ORDERS          "Orders"

//  Category of orders
#define STR_BTN_NEW_ORDERS      "New Orders"
#define STR_BTN_CNF_ORDERS      "Confirmed Orders"
#define STR_BTN_DLVRD_ORDERS    "Delivered Orders"
#define STR_BTN_CNCLD_ORDERS    "Cancelled Orders"
#define STR_BTN_YOUR_ORDERS     "Your Orders"

#define STR_BTN_SINGLE_ORDER    "Single Order"
#define STR_BTN_ADMIN_PAGE      "Admin"
#define STR_BTN_ADD_BY_SN       "Add to Cart by SN"
#define STR_BTN_CHANGE_ADDRESS  "Change Address"
#define STR_BTN_ABOUT           "About"
#define STR_BTN_HELP_FAQ        "Help & FAQ"
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

//enum class UserContext {ADDRESS, CLEANUP, PRICE_CHANGE, NEW_ORDER, CNF_ORDER, YOUR_ORDER, REVIEW, NOTA};
typedef int UserContext;

extern std::map<unsigned int, UserContext> m_Context;
extern std::vector<unsigned int> adminChatIds;

#endif /* CONSTANTS_H_ */
