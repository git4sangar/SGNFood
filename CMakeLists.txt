#sgn
cmake_minimum_required(VERSION 3.1)
project(BaseBot)

# compiling options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(/usr/include/freetype2)

message("project source dir: ${PROJECT_SOURCE_DIR}")

find_library(BOOST_SYSTEM NAMES boost_system)
find_library(BOOST_CHRONO NAMES boost_chrono)
find_library(BOOST_THREAD NAMES boost_thread)
find_package(OpenSSL REQUIRED)

add_executable(BaseBot
    src/Checkout.cpp
    src/Daemon.cpp
    src/DBInterface.cpp
    src/EditMenu.cpp
    src/FAQs.cpp
    src/MyAddress.cpp
    src/OrderMgmt.cpp
    src/PriceChange.cpp
    src/ProductList.cpp
    src/SingleOrder.cpp
    src/ViewCart.cpp
	)

target_link_libraries(BaseBot TgBot sqlite3 SQLiteCpp ssl crypto curl pthread PNGwriter png freetype ${CMAKE_THREAD_LIBS_INIT} ${BOOST_THREAD} ${BOOST_SYSTEM} ${BOOST_CHRONO})
