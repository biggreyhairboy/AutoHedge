/****************************************************
*** Author: patrick Yang
*** Create Time: 2016/12/02
*** function: 从CTP获取行情数据，根据不同的对冲倾向来自动对冲配置文件中的欧式期权头寸
*** input：    期权要素与期货价格
*** out：      中性的对冲头寸，交易记录
*** other:
*** environment:  1. C++11中thread库 2. 使用quantlib来对期权进行定价与计算希腊字母值 3. boost1.58读取配置文件，写日志等等
*** todo:      1. 从数据库中读取欧式期权头寸（连接SQL SERVER的库文集再定
***            2. 可以对冲多个期权
               3. 从客户权益里扣除对冲的成本（这里主要指的是手续费）
               4. 假设开始的时候已经完成了最初的对冲
 */

#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/log/utility/setup/file.hpp>

#include "ctpapi_linux64/ThostFtdcMdApi.h"
#include "ctpapi_linux64/ThostFtdcTraderApi.h"
#include "MarketDataHandle.h"
#include "TradingHandle.h"
#include "HedgeDriver.h"

#include "DBDriver.h"

using namespace std;
namespace  logging = boost::log;

void split(const string &s, char delim, vector<string> &elems);
vector<string> split(const string &s, char delim);

void split(const string &s, char delim, vector<string> &elems){
    stringstream ss;
    ss.str(s);
    string item;
    while(getline(ss, item, delim)){
        elems.push_back(item);
    }
}

vector<string> split(const string &s, char delim){
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

void quoteThread(char* FRONT_ADDR_quote, TThostFtdcBrokerIDType brokerid, TThostFtdcInvestorIDType investorid,
                 TThostFtdcPasswordType password, DBDriver* dbdriver, vector<string> ppinsturment, int instrument )
{
    CThostFtdcMdApi* pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();
    CThostFtdcMdSpi *pMarketDataHandle = new MarketDataHandle(pUserApi, FRONT_ADDR_quote, brokerid, investorid,
                                                              password, dbdriver, ppinsturment, instrument);
    pUserApi->RegisterSpi(pMarketDataHandle);
    pUserApi->RegisterFront(FRONT_ADDR_quote);
    pUserApi->Init();
    pUserApi->Join();
}

void tradeThread(char* FRONT_ADDR_trade, TThostFtdcBrokerIDType brokerid, TThostFtdcInvestorIDType investorid,
                 TThostFtdcPasswordType password, DBDriver* dbdriver, TThostFtdcInstrumentIDType INSTRUMENT_ID,
                 TThostFtdcPriceType LIMIT_PRICE, int quantity, TThostFtdcDirectionType DIRECTION)
{
    CThostFtdcTraderApi* pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi();
    TradingHandle *pTradingHandle = new TradingHandle(pTraderApi, FRONT_ADDR_trade, brokerid, investorid, password, dbdriver,
                                                      INSTRUMENT_ID, LIMIT_PRICE, quantity,  DIRECTION);
    pTraderApi->RegisterSpi((CThostFtdcTraderSpi*) pTradingHandle);
    pTraderApi->SubscribePublicTopic(THOST_TERT_QUICK);
    pTraderApi->SubscribePrivateTopic(THOST_TERT_QUICK);
    pTraderApi->RegisterFront(FRONT_ADDR_trade);
    pTraderApi->Init();
    pTraderApi->Join();
    pTraderApi->Release();
}

int main() {
    cout << "AutoHedge" << endl;
    //配置文件加上default值，防止exception
    //read config
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini("/home/patrick/ClionProjects/AutoHedge/AutoHedge.ini", pt);
    //server
    string MF= pt.get<std::string>("Server_IP.MarketFront");
    string TF = pt.get<std::string>("Server_IP.TradeFront");
    const int chararraylength = 50;
    char FRONT_ADDR_quote[chararraylength];
    char FRONT_ADDR_trade[chararraylength];
    strcpy(FRONT_ADDR_quote, MF.c_str());
    strcpy(FRONT_ADDR_trade, TF.c_str());
    //account
    TThostFtdcBrokerIDType brokerIDType;
    strcpy(brokerIDType, pt.get<std::string>("Account.BrokerID").c_str());
    TThostFtdcInvestorIDType investorIDType;
    strcpy(investorIDType, pt.get<std::string>("Account.InvestorID").c_str());
    TThostFtdcPasswordType passwordType;
    strcpy(passwordType, pt.get<std::string>("Account.Password").c_str());
    //database
    string Server = pt.get<std::string>("Database.Server");
    string User = pt.get<std::string>("Database.User");
    string Password = pt.get<std::string>("Database.Password");
    string Scheme = pt.get<std::string>("Database.Scheme");
    DBDriver dbDriver(Server, User, Password, Scheme);
    //market
    int iInstrumentID = 2;
    string SubscribeSymbolList = pt.get<std::string>("MarketData.SubscribeSymbolList");
    vector<string> ppIntrumentID(split(SubscribeSymbolList, ','));
    
    //log
    logging::add_file_log(pt.get<std::string>("CTPClientDemo.LogPath"));

    //option import :BasicParameters
    string direction = pt.get<std::string>("BasicParameters.direction");
    int type = pt.get<int>("BasicParameters.type");
    string underlying = pt.get<std::string>("BasicParameters.underlying");
    double strikeprice = pt.get<double>("BasicParameters.strikeprice");
    double lastprice = pt.get<double>("BasicParameters.lastprice");
    string maturity = pt.get<std::string>("BasicParameters.maturity");
    double vol = pt.get<double>("BasicParameters.vol");
    double freedelta = pt.get<double>("BasicParameters.freedelta");
    int HedgeTendency = pt.get<int>("BasicParameters.HedgeTendency");
    int HedgeStrategy = pt.get<int>("BasicParameters.HedgeStrategy");
    int PreQuantity = pt.get<int>("BasicParameters.PreQuantity");
    Real dividend = 0.0;
    Real riskfreerate = 0.06;

    HedgeDriver hd(maturity, Option::Type(type), lastprice, strikeprice,dividend, riskfreerate, vol);

    BOOST_LOG_TRIVIAL(info)<<"quote thread started ...";
    //cout << "quote thread started .... " << endl;
    std::thread QuoteT(quoteThread, FRONT_ADDR_quote, brokerIDType, investorIDType, passwordType,
                       &dbDriver, ppIntrumentID,iInstrumentID);
    QuoteT.detach();

//    BOOST_LOG_TRIVIAL(info)<<"trade thread started ...";
//    std::thread TradingT(tradeThread, FRONT_ADDR_quote, brokerIDType, investorIDType, passwordType,
//                       &dbDriver, tinstrumemt, price, quantity, direction);
//    TradingT.detach();

    getchar();
    return 0;
}
