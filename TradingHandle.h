//
// Created by biggreyhairboy on 7/27/16.
//

#ifndef AUTOHEDGE_TRADINGHANDLE_H
#define AUTOHEDGE_TRADINGHANDLE_H

#include "ctpapi_linux64/ThostFtdcTraderApi.h"
#include "DBDriver.h"

class TradingHandle : public CThostFtdcTraderSpi {
public:
    /// trader ����
    CThostFtdcTraderApi* pTraderApi;
    DBDriver *dbDrvier;

    /// ���ò���
    char FRONT_ADDR_trade[50];		// ǰ�õ�ַ
    TThostFtdcBrokerIDType brokerIDType;		// ���͹�˾����
    TThostFtdcInvestorIDType investorIDType;		// Ͷ���ߴ���
    TThostFtdcPasswordType passwordType;			// �û�����

    /// ������
    int iRequestID_trade;
    /// �Ự����
    TThostFtdcFrontIDType	FRONT_ID;	//ǰ�ñ��
    TThostFtdcSessionIDType	SESSION_ID;	//�Ự���
    TThostFtdcOrderRefType	ORDER_REF;	//��������
    TThostFtdcOrderRefType	EXECORDER_REF;	//ִ����������
    TThostFtdcOrderRefType	FORQUOTE_REF;	//ѯ������
    TThostFtdcOrderRefType	QUOTE_REF;	//��������
    ///��������
    TThostFtdcInstrumentIDType INSTRUMENT_ID;
    TThostFtdcPriceType LIMIT_PRICE;
    TThostFtdcDirectionType DIRECTION;
    int quantity;

    ///���캯��
    TradingHandle(CThostFtdcTraderApi*, char *, TThostFtdcBrokerIDType, TThostFtdcInvestorIDType, TThostFtdcPasswordType, DBDriver*,
                  TThostFtdcInstrumentIDType, TThostFtdcPriceType, int, TThostFtdcDirectionType);

    ///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
    virtual void OnFrontConnected();

    ///��¼������Ӧ
    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///Ͷ���߽�����ȷ����Ӧ
    virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///�����ѯ��Լ��Ӧ
    virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///�����ѯ�ʽ��˻���Ӧ
    virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///�����ѯͶ���ֲ߳���Ӧ
    virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///����¼��������Ӧ
    virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///ִ������¼��������Ӧ
    virtual void OnRspExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///ѯ��¼��������Ӧ
    virtual void OnRspForQuoteInsert(CThostFtdcInputForQuoteField *pInputForQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///����¼��������Ӧ
    virtual void OnRspQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///��������������Ӧ
    virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///ִ���������������Ӧ
    virtual void OnRspExecOrderAction(CThostFtdcInputExecOrderActionField *pInputExecOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///���۲���������Ӧ
    virtual void OnRspQuoteAction(CThostFtdcInputQuoteActionField *pInputQuoteAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///����Ӧ��
    virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
    virtual void OnFrontDisconnected(int nReason);

    ///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
    virtual void OnHeartBeatWarning(int nTimeLapse);

    ///����֪ͨ
    virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

    ///ִ������֪ͨ
    virtual void OnRtnExecOrder(CThostFtdcExecOrderField *pExecOrder);

    ///ѯ��֪ͨ
    virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp);

    ///����֪ͨ
    virtual void OnRtnQuote(CThostFtdcQuoteField *pQuote);

    ///�ɽ�֪ͨ
    virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

private:
    ///�û���¼����
    void ReqUserLogin();
    ///Ͷ���߽�����ȷ��
    void ReqSettlementInfoConfirm();
    ///�����ѯ��Լ
    void ReqQryInstrument();
    ///�����ѯ�ʽ��˻�
    void ReqQryTradingAccount();
    ///�����ѯͶ���ֲ߳�
    void ReqQryInvestorPosition();
    ///����¼������
    void ReqOrderInsert();
    ///ִ������¼������
    void ReqExecOrderInsert();
    ///ѯ��¼������
    void ReqForQuoteInsert();
    ///����¼������
    void ReqQuoteInsert();
    ///������������
    void ReqOrderAction(CThostFtdcOrderField *pOrder);
    ///ִ�������������
    void ReqExecOrderAction(CThostFtdcExecOrderField *pExecOrder);
    ///���۲�������
    void ReqQuoteAction(CThostFtdcQuoteField *pQuote);

    // �Ƿ��յ��ɹ�����Ӧ
    bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
    // �Ƿ��ҵı����ر�
    bool IsMyOrder(CThostFtdcOrderField *pOrder);
    // �Ƿ��ҵ�ִ������ر�
    bool IsMyExecOrder(CThostFtdcExecOrderField *pExecOrder);
    // �Ƿ��ҵı���
    bool IsMyQuote(CThostFtdcQuoteField *pQuote);
    // �Ƿ����ڽ��׵ı���
    bool IsTradingOrder(CThostFtdcOrderField *pOrder);
    // �Ƿ�δ������ִ������
    bool IsTradingExecOrder(CThostFtdcExecOrderField *pExecOrder);
    // �Ƿ�δ�����ı���
    bool IsTradingQuote(CThostFtdcQuoteField *pQuote);
};


#endif //CTPCLIENTDEMO_TRADINGHANDLE_H
