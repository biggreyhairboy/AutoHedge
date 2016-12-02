//
// Created by biggreyhairboy on 7/27/16.
//

#include <cstring>
#include <iostream>
#include <thread>
#include "TradingHandle.h"
#include "ctpapi_linux64/ThostFtdcTraderApi.h"
using namespace std;

TradingHandle::TradingHandle(CThostFtdcTraderApi* iTraderApi, char* front_address, TThostFtdcBrokerIDType brokerid,
                             TThostFtdcInvestorIDType investorid, TThostFtdcPasswordType password, DBDriver *dbdriver,
                             TThostFtdcInstrumentIDType INSTRUMENT_ID,
                             TThostFtdcPriceType LIMIT_PRICE, int  quantity, TThostFtdcDirectionType DIRECTION)
{
    this->pTraderApi = iTraderApi;
    this->dbDrvier = dbdriver;
    strcpy(this->FRONT_ADDR_trade, front_address);
    strcpy(this->brokerIDType, brokerid);
    strcpy(this->investorIDType, investorid);
    strcpy(this->passwordType, password);

    strcpy(this->INSTRUMENT_ID, INSTRUMENT_ID);
    this->LIMIT_PRICE = LIMIT_PRICE;
    this->DIRECTION = DIRECTION;
    this->quantity = quantity;
    int iRequestID_trade = 0;
}
// �����ж�
bool IsFlowControl(int iResult)
{
    return ((iResult == -2) || (iResult == -3));
}

void TradingHandle::OnFrontConnected()
{
    cerr << "trade---->>> " << "OnFrontConnected" << endl;
    ///�û���¼����
    ReqUserLogin();
}

void TradingHandle::ReqUserLogin()
{
    CThostFtdcReqUserLoginField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, brokerIDType);
    strcpy(req.UserID, investorIDType);
    strcpy(req.Password, passwordType);
    int iResult = pTraderApi->ReqUserLogin(&req, ++iRequestID_trade);
    cerr << "trade---->>> �����û���¼����: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
}

void TradingHandle::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                                CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cerr << "trade---->>> " << "OnRspUserLogin" << endl;
    if (bIsLast && !IsErrorRspInfo(pRspInfo))
    {
        // ����Ự����
        FRONT_ID = pRspUserLogin->FrontID;
        SESSION_ID = pRspUserLogin->SessionID;
        int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
        iNextOrderRef++;
        sprintf(ORDER_REF, "%d", iNextOrderRef);
        sprintf(EXECORDER_REF, "%d", 1);
        sprintf(FORQUOTE_REF, "%d", 1);
        sprintf(QUOTE_REF, "%d", 1);
        ///��ȡ��ǰ������
        cerr << "trade---->>> ��ȡ��ǰ������ = " << pTraderApi->GetTradingDay() << endl;
        ///Ͷ���߽�����ȷ��
        ReqSettlementInfoConfirm();
    }
}

void TradingHandle::ReqSettlementInfoConfirm()
{
    CThostFtdcSettlementInfoConfirmField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, brokerIDType);
    strcpy(req.InvestorID, investorIDType);
    int iResult = pTraderApi->ReqSettlementInfoConfirm(&req, ++iRequestID_trade);
    cerr << "trade---->>> Ͷ���߽�����ȷ��: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
    ReqOrderInsert();
}

void TradingHandle::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cerr << "trade---->>> " << "OnRspSettlementInfoConfirm" << endl;
    if (bIsLast && !IsErrorRspInfo(pRspInfo))
    {
        ///�����ѯ��Լ
        ReqQryInstrument();
    }

}

void TradingHandle::ReqQryInstrument()
{
    CThostFtdcQryInstrumentField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.InstrumentID, INSTRUMENT_ID);
    while (true)
    {
        int iResult = pTraderApi->ReqQryInstrument(&req, ++iRequestID_trade);
        if (!IsFlowControl(iResult))
        {
            cerr << "trade---->>> �����ѯ��Լ: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
            break;
        }
        else
        {
            cerr << "trade---->>> �����ѯ��Լ: " << iResult << ", �ܵ�����" << endl;
            //Sleep(1000);
            std::this_thread::sleep_for(std::chrono::microseconds(1000));
        }
    } // while
}

void TradingHandle::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cerr << "trade---->>> " << "OnRspQryInstrument" << endl;
    if (bIsLast && !IsErrorRspInfo(pRspInfo))
    {
        ///�����ѯ��Լ
        ReqQryTradingAccount();
    }
}

void TradingHandle::ReqQryTradingAccount()
{
    CThostFtdcQryTradingAccountField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, brokerIDType);
    strcpy(req.InvestorID, investorIDType);
    while (true)
    {
        int iResult = pTraderApi->ReqQryTradingAccount(&req, ++iRequestID_trade);
        if (!IsFlowControl(iResult))
        {
            //cerr << "trade---->>> �����ѯ�ʽ��˻�: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
            break;
        }
        else
        {
            //cerr << "trade---->>> �����ѯ�ʽ��˻�: " << iResult << ", �ܵ�����" << endl;
            //Sleep(1000);
            std::this_thread::sleep_for(std::chrono::microseconds(1000));
        }
    } // while
}

void TradingHandle::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cerr << "trade---->>> " << "OnRspQryTradingAccount" << endl;
    if (bIsLast && !IsErrorRspInfo(pRspInfo))
    {
        ///�����ѯͶ���ֲ߳�
        ReqQryInvestorPosition();
    }
}

void TradingHandle::ReqQryInvestorPosition()
{
    CThostFtdcQryInvestorPositionField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, brokerIDType);
    strcpy(req.InvestorID, investorIDType);
    strcpy(req.InstrumentID, INSTRUMENT_ID);
    while (true)
    {
        int iResult = pTraderApi->ReqQryInvestorPosition(&req, ++iRequestID_trade);
        if (!IsFlowControl(iResult))
        {
//            cerr << "trade---->>> �����ѯͶ���ֲ߳�: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
            break;
        }
        else
        {
//            cerr << "trade---->>> �����ѯͶ���ֲ߳�: " << iResult << ", �ܵ�����" << endl;
            std::this_thread::sleep_for(std::chrono::microseconds(1000));
        }
    }
}

void TradingHandle::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cerr << "trade---->>> " << "OnRspQryInvestorPosition" << endl;
    if (bIsLast && !IsErrorRspInfo(pRspInfo))
    {
        ///����¼������
        ReqOrderInsert();
        //ִ������¼������
        ReqExecOrderInsert();
        //ѯ��¼��
        ReqForQuoteInsert();
        //�����̱���¼��
        ReqQuoteInsert();
    }
}

void TradingHandle::ReqOrderInsert()
{
    CThostFtdcInputOrderField req;
    memset(&req, 0, sizeof(req));
    ///���͹�˾����
    strcpy(req.BrokerID, brokerIDType);
    ///Ͷ���ߴ���
    strcpy(req.InvestorID, investorIDType);
    ///��Լ����
    strcpy(req.InstrumentID, INSTRUMENT_ID);
    ///��������
    strcpy(req.OrderRef, ORDER_REF);
    ///�û�����
    //	TThostFtdcUserIDType	UserID;
    ///�����۸�����: �޼�
    req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
    ///��������: 
    req.Direction = DIRECTION;
    ///��Ͽ�ƽ��־: ����
    req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
    ///���Ͷ���ױ���־
    req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
    ///�۸�
    req.LimitPrice = LIMIT_PRICE;
    ///����: 1
    req.VolumeTotalOriginal = quantity;
    ///��Ч������: ������Ч
    req.TimeCondition = THOST_FTDC_TC_GFD;
    ///GTD����
    //	TThostFtdcDateType	GTDDate;
    ///�ɽ�������: �κ�����
    req.VolumeCondition = THOST_FTDC_VC_AV;
    ///��С�ɽ���: 1
    req.MinVolume = 1;
    ///��������: ����
    req.ContingentCondition = THOST_FTDC_CC_Immediately;
    ///ֹ���
    //	TThostFtdcPriceType	StopPrice;
    ///ǿƽԭ��: ��ǿƽ
    req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
    ///�Զ������־: ��
    req.IsAutoSuspend = 0;
    ///ҵ��Ԫ
    //	TThostFtdcBusinessUnitType	BusinessUnit;
    ///������
    //	TThostFtdcRequestIDType	RequestID;
    ///�û�ǿ����־: ��
    req.UserForceClose = 0;

    int iResult = pTraderApi->ReqOrderInsert(&req, ++iRequestID_trade);
    cerr << "trade---->>> ����¼������: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
}


////����µ��㱨
//void TradingHandle::OnRtnOrder(CThostFtdcOrderField *pOrder)
//{
//
//}

//ִ������¼������
void TradingHandle::ReqExecOrderInsert()
{
    CThostFtdcInputExecOrderField req;
    memset(&req, 0, sizeof(req));
    ///���͹�˾����
    strcpy(req.BrokerID, brokerIDType);
    ///Ͷ���ߴ���
    strcpy(req.InvestorID, investorIDType);
    ///��Լ����
    strcpy(req.InstrumentID, INSTRUMENT_ID);
    ///��������
    strcpy(req.ExecOrderRef, EXECORDER_REF);
    ///�û�����
    //	TThostFtdcUserIDType	UserID;
    ///����
    req.Volume = 1;
    ///������
    //TThostFtdcRequestIDType	RequestID;
    ///ҵ��Ԫ
    //TThostFtdcBusinessUnitType	BusinessUnit;
    ///��ƽ��־
    req.OffsetFlag = THOST_FTDC_OF_Close;//���������������Ҫ��ƽ���ƽ��
    ///Ͷ���ױ���־
    req.HedgeFlag = THOST_FTDC_HF_Speculation;
    ///ִ������
    req.ActionType = THOST_FTDC_ACTP_Exec;//�������ִ������THOST_FTDC_ACTP_Abandon
    ///����ͷ������ĳֲַ���
    req.PosiDirection = THOST_FTDC_PD_Long;
    ///��Ȩ��Ȩ���Ƿ����ڻ�ͷ��ı��
    req.ReservePositionFlag = THOST_FTDC_EOPF_UnReserve;//�����н��������������֣������THOST_FTDC_EOPF_Reserve
    ///��Ȩ��Ȩ�����ɵ�ͷ���Ƿ��Զ�ƽ��
    req.CloseFlag = THOST_FTDC_EOCF_AutoClose;//�����н��������������֣������THOST_FTDC_EOCF_NotToClose

    int iResult = pTraderApi->ReqExecOrderInsert(&req, ++iRequestID_trade);
    cerr << "trade---->>> ִ������¼������: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
}

//ѯ��¼������
void TradingHandle::ReqForQuoteInsert()
{
    CThostFtdcInputForQuoteField req;
    memset(&req, 0, sizeof(req));
    ///���͹�˾����
    strcpy(req.BrokerID, brokerIDType);
    ///Ͷ���ߴ���
    strcpy(req.InvestorID, investorIDType);
    ///��Լ����
    strcpy(req.InstrumentID, INSTRUMENT_ID);
    ///��������
    strcpy(req.ForQuoteRef, EXECORDER_REF);
    ///�û�����
    //	TThostFtdcUserIDType	UserID;

    int iResult = pTraderApi->ReqForQuoteInsert(&req, ++iRequestID_trade);
    cerr << "trade---->>> ѯ��¼������: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
}
//����¼������
void TradingHandle::ReqQuoteInsert()
{
    CThostFtdcInputQuoteField req;
    memset(&req, 0, sizeof(req));
    ///���͹�˾����
    strcpy(req.BrokerID, brokerIDType);
    ///Ͷ���ߴ���
    strcpy(req.InvestorID, investorIDType);
    ///��Լ����
    strcpy(req.InstrumentID, INSTRUMENT_ID);
    ///��������
    strcpy(req.QuoteRef, QUOTE_REF);
    ///���۸�
    req.AskPrice = LIMIT_PRICE;
    ///��۸�
    req.BidPrice = LIMIT_PRICE - 1.0;
    ///������
    req.AskVolume = 1;
    ///������
    req.BidVolume = 1;
    ///������
    //TThostFtdcRequestIDType	RequestID;
    ///ҵ��Ԫ
    //TThostFtdcBusinessUnitType	BusinessUnit;
    ///����ƽ��־
    req.AskOffsetFlag = THOST_FTDC_OF_Open;
    ///��ƽ��־
    req.BidOffsetFlag = THOST_FTDC_OF_Open;
    ///��Ͷ���ױ���־
    req.AskHedgeFlag = THOST_FTDC_HF_Speculation;
    ///��Ͷ���ױ���־
    req.BidHedgeFlag = THOST_FTDC_HF_Speculation;

    int iResult = pTraderApi->ReqQuoteInsert(&req, ++iRequestID_trade);
    cerr << "trade---->>> ����¼������: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
}

void TradingHandle::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cerr << "trade---->>> " << "OnRspOrderInsert" << endl;
    IsErrorRspInfo(pRspInfo);
}

void TradingHandle::OnRspExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    //���ִ��������ȷ���򲻻����ûص�
    cerr << "trade---->>> " << "OnRspExecOrderInsert" << endl;
    IsErrorRspInfo(pRspInfo);
}

void TradingHandle::OnRspForQuoteInsert(CThostFtdcInputForQuoteField *pInputForQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    //���ѯ����ȷ���򲻻����ûص�
    cerr << "trade---->>> " << "OnRspForQuoteInsert" << endl;
    IsErrorRspInfo(pRspInfo);
}

void TradingHandle::OnRspQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    //���������ȷ���򲻻����ûص�
    cerr << "trade---->>> " << "OnRspQuoteInsert" << endl;
    IsErrorRspInfo(pRspInfo);
}

void TradingHandle::ReqOrderAction(CThostFtdcOrderField *pOrder)
{
    static bool ORDER_ACTION_SENT = false;		//�Ƿ����˱���
    if (ORDER_ACTION_SENT)
        return;

    CThostFtdcInputOrderActionField req;
    memset(&req, 0, sizeof(req));
    ///���͹�˾����
    strcpy(req.BrokerID, pOrder->BrokerID);
    ///Ͷ���ߴ���
    strcpy(req.InvestorID, pOrder->InvestorID);
    ///������������
    //	TThostFtdcOrderActionRefType	OrderActionRef;
    ///��������
    strcpy(req.OrderRef, pOrder->OrderRef);
    ///������
    //	TThostFtdcRequestIDType	RequestID;
    ///ǰ�ñ��
    req.FrontID = FRONT_ID;
    ///�Ự���
    req.SessionID = SESSION_ID;
    ///����������
    //	TThostFtdcExchangeIDType	ExchangeID;
    ///�������
    //	TThostFtdcOrderSysIDType	OrderSysID;
    ///������־
    req.ActionFlag = THOST_FTDC_AF_Delete;
    ///�۸�
    //	TThostFtdcPriceType	LimitPrice;
    ///�����仯
    //	TThostFtdcVolumeType	VolumeChange;
    ///�û�����
    //	TThostFtdcUserIDType	UserID;
    ///��Լ����
    strcpy(req.InstrumentID, pOrder->InstrumentID);

    int iResult = pTraderApi->ReqOrderAction(&req, ++iRequestID_trade);
    cerr << "trade---->>> ������������: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
    ORDER_ACTION_SENT = true;
}

void TradingHandle::ReqExecOrderAction(CThostFtdcExecOrderField *pExecOrder)
{
    static bool EXECORDER_ACTION_SENT = false;		//�Ƿ����˱���
    if (EXECORDER_ACTION_SENT)
        return;

    CThostFtdcInputExecOrderActionField req;
    memset(&req, 0, sizeof(req));

    ///���͹�˾����
    strcpy(req.BrokerID, pExecOrder->BrokerID);
    ///Ͷ���ߴ���
    strcpy(req.InvestorID, pExecOrder->InvestorID);
    ///ִ�������������
    //TThostFtdcOrderActionRefType	ExecOrderActionRef;
    ///ִ����������
    strcpy(req.ExecOrderRef, pExecOrder->ExecOrderRef);
    ///������
    //TThostFtdcRequestIDType	RequestID;
    ///ǰ�ñ��
    req.FrontID = FRONT_ID;
    ///�Ự���
    req.SessionID = SESSION_ID;
    ///����������
    //TThostFtdcExchangeIDType	ExchangeID;
    ///ִ������������
    //TThostFtdcExecOrderSysIDType	ExecOrderSysID;
    ///������־
    req.ActionFlag = THOST_FTDC_AF_Delete;
    ///�û�����
    //TThostFtdcUserIDType	UserID;
    ///��Լ����
    strcpy(req.InstrumentID, pExecOrder->InstrumentID);

    int iResult = pTraderApi->ReqExecOrderAction(&req, ++iRequestID_trade);
    cerr << "trade---->>> ִ�������������: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
    EXECORDER_ACTION_SENT = true;
}

void TradingHandle::ReqQuoteAction(CThostFtdcQuoteField *pQuote)
{
    static bool QUOTE_ACTION_SENT = false;		//�Ƿ����˱���
    if (QUOTE_ACTION_SENT)
        return;

    CThostFtdcInputQuoteActionField req;
    memset(&req, 0, sizeof(req));
    ///���͹�˾����
    strcpy(req.BrokerID, pQuote->BrokerID);
    ///Ͷ���ߴ���
    strcpy(req.InvestorID, pQuote->InvestorID);
    ///���۲�������
    //TThostFtdcOrderActionRefType	QuoteActionRef;
    ///��������
    strcpy(req.QuoteRef, pQuote->QuoteRef);
    ///������
    //TThostFtdcRequestIDType	RequestID;
    ///ǰ�ñ��
    req.FrontID = FRONT_ID;
    ///�Ự���
    req.SessionID = SESSION_ID;
    ///����������
    //TThostFtdcExchangeIDType	ExchangeID;
    ///���۲������
    //TThostFtdcOrderSysIDType	QuoteSysID;
    ///������־
    req.ActionFlag = THOST_FTDC_AF_Delete;
    ///�û�����
    //TThostFtdcUserIDType	UserID;
    ///��Լ����
    strcpy(req.InstrumentID, pQuote->InstrumentID);

    int iResult = pTraderApi->ReqQuoteAction(&req, ++iRequestID_trade);
    cerr << "trade---->>> ���۲�������: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
    QUOTE_ACTION_SENT = true;
}

void TradingHandle::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cerr << "trade---->>> " << "OnRspOrderAction" << endl;
    IsErrorRspInfo(pRspInfo);
}

void TradingHandle::OnRspExecOrderAction(CThostFtdcInputExecOrderActionField *pInpuExectOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    //��ȷ�ĳ����������������ûص�
    cerr << "trade---->>> " << "OnRspExecOrderAction" << endl;
    IsErrorRspInfo(pRspInfo);
}

void TradingHandle::OnRspQuoteAction(CThostFtdcInputQuoteActionField *pInpuQuoteAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    //��ȷ�ĳ����������������ûص�
    cerr << "trade---->>> " << "OnRspQuoteAction" << endl;
    IsErrorRspInfo(pRspInfo);
}

///����֪ͨ
void TradingHandle::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
    cerr << "trade---->>> " << "OnRtnOrder" << endl;
    if (IsMyOrder(pOrder))
    {
        if (IsTradingOrder(pOrder))
            ReqOrderAction(pOrder);
        else if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled)
            cout << "trade---->>> �����ɹ�" << endl;
    }
}

//ִ������֪ͨ
void TradingHandle::OnRtnExecOrder(CThostFtdcExecOrderField *pExecOrder)
{
    cerr << "trade---->>> " << "OnRtnExecOrder" << endl;
    if (IsMyExecOrder(pExecOrder))
    {
        if (IsTradingExecOrder(pExecOrder))
            ReqExecOrderAction(pExecOrder);
        else if (pExecOrder->ExecResult == THOST_FTDC_OER_Canceled)
            cout << "trade---->>> ִ�����泷���ɹ�" << endl;
    }
}

//ѯ��֪ͨ
void TradingHandle::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
    //�������н���ѯ��֪ͨͨ���ýӿڷ��أ�ֻ�������̿ͻ������յ���֪ͨ
    cerr << "trade---->>> " << "OnRtnForQuoteRsp" << endl;
}

//����֪ͨ
void TradingHandle::OnRtnQuote(CThostFtdcQuoteField *pQuote)
{
    cerr << "trade---->>> " << "OnRtnQuote" << endl;
    if (IsMyQuote(pQuote))
    {
        if (IsTradingQuote(pQuote))
            ReqQuoteAction(pQuote);
        else if (pQuote->QuoteStatus == THOST_FTDC_OST_Canceled)
            cout << "trade---->>> ���۳����ɹ�" << endl;
    }
}

///�ɽ�֪ͨ
void TradingHandle::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
    cerr << "trade---->>> " << "OnRtnTrade" << endl;
}

void TradingHandle::OnFrontDisconnected(int nReason)
{
    cerr << "trade---->>> " << "OnFrontDisconnected" << endl;
    cerr << "trade---->>> Reason = " << nReason << endl;
}

void TradingHandle::OnHeartBeatWarning(int nTimeLapse)
{
    cerr << "trade---->>> " << "OnHeartBeatWarning" << endl;
    cerr << "trade---->>> nTimerLapse = " << nTimeLapse << endl;
}

void TradingHandle::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cerr << "trade---->>> " << "OnRspError" << endl;
    IsErrorRspInfo(pRspInfo);
}

bool TradingHandle::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
    // ���ErrorID != 0, ˵���յ��˴������Ӧ
    bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
    if (bResult)
        cerr << "trade---->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
    return bResult;
}

bool TradingHandle::IsMyOrder(CThostFtdcOrderField *pOrder)
{
    return ((pOrder->FrontID == FRONT_ID) &&
            (pOrder->SessionID == SESSION_ID) &&
            (strcmp(pOrder->OrderRef, ORDER_REF) == 0));
}

bool TradingHandle::IsMyExecOrder(CThostFtdcExecOrderField *pExecOrder)
{
    return ((pExecOrder->FrontID == FRONT_ID) &&
            (pExecOrder->SessionID == SESSION_ID) &&
            (strcmp(pExecOrder->ExecOrderRef, EXECORDER_REF) == 0));
}

bool TradingHandle::IsMyQuote(CThostFtdcQuoteField *pQuote)
{
    return ((pQuote->FrontID == FRONT_ID) &&
            (pQuote->SessionID == SESSION_ID) &&
            (strcmp(pQuote->QuoteRef, QUOTE_REF) == 0));
}

bool TradingHandle::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
    return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
            (pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
            (pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}

bool TradingHandle::IsTradingExecOrder(CThostFtdcExecOrderField *pExecOrder)
{
    return (pExecOrder->ExecResult != THOST_FTDC_OER_Canceled);
}

bool TradingHandle::IsTradingQuote(CThostFtdcQuoteField *pQuote)
{
    return (pQuote->QuoteStatus != THOST_FTDC_OST_Canceled);
}
