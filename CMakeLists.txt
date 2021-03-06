cmake_minimum_required(VERSION 3.5)
project(AutoHedge)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)

SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "/usr/local/include/boost/")
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "/usr/local/lib/")
#下面这就话是有效的
include_directories(/usr/local/include/boost/)
#下面这就话是有效的
LINK_DIRECTORIES(/usr/local/lib/)
set(BOOST_ROOT /usr/local/include/boost/)
find_package(Boost 1.62 COMPONENTS log system thread filesystem)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES("/usr/local/include/boost/")
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    link_directories("/usr/local/lib/")
        set(CMAKE_BOOST_LINK)
    message("hello boost 1.62")
    message(${Boost_INCLUDE_DIR})
ENDIF()

INCLUDE_DIRECTORIES(${BOOST_ROOT})
LINK_DIRECTORIES("/usr/local/lib/")
find_package(Threads)

ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK)
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
set(SOURCE_FILES main.cpp MarketDataHandle.cpp MarketDataHandle.h TradingHandle.cpp TradingHandle.h DBDriver.cpp DBDriver.h HedgeDriver.h HedgeDriver.cpp HedgeDriver.h AtestClass.cpp AtestClass.h OptionCalculator.cpp OptionCalculator.h)

add_library(thostmduserapi SHARED IMPORTED)
add_library(thosttraderapi SHARED IMPORTED)
SET_TARGET_PROPERTIES(thostmduserapi PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/ctpapi_linux64/thostmduserapi.so)
SET_TARGET_PROPERTIES(thosttraderapi PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/ctpapi_linux64/thosttraderapi.so)

#add_library(boostlog SHARED IMPORTED)
#add_library(boostthread SHARED IMPORTED)
#set_target_properties(boostlog PROPERTIES IMPORTED_LOCATION /usr/local/lib/libboost_log.so)
#set_target_properties(boostthread PROPERTIES IMPORTED_LOCATION /usr/local/lib/libboost_thread.so)
add_executable(AutoHedge ${SOURCE_FILES})
message(${Boost_LOG_LIBRARY})
message(${Boost_THREAD_LIBRARY})
#target_link_libraries(AutoHedge QuantLib mysqlcppconn thostmduserapi thosttraderapi  ${Boost_LOG_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
#${CMAKE_THREAD_LIBS_INIT}
target_link_libraries(AutoHedge "/usr/local/lib/libQuantLib.so"  mysqlcppconn thostmduserapi thosttraderapi -lpthread "/usr/local/lib/libboost_thread.so" "/usr/local/lib/libboost_system.so"  "/usr/local/lib/libboost_log.so")