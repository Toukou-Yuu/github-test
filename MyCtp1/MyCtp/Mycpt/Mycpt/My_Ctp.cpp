#include"ThostFtdcMdApi.h"
#include<iostream>
#include<vector>
#include<stack>
#define MAX 1024
#pragma comment(lib,"thostmduserapi_se.lib")
#pragma comment(lib,"thosttraderapi_se.lib")
using namespace std;


class Ctp_Callback :public CThostFtdcMdSpi {
public:
	//创建api操作对象
	CThostFtdcMdApi* User_Api;
	//创建登陆请求
	CThostFtdcReqUserLoginField login_req;
	//创建登出请求
	CThostFtdcUserLogoutField logou_req;
	//合约数量/退订合约数量
	vector<CThostFtdcDepthMarketDataField> contract_array;
	vector<CThostFtdcDepthMarketDataField> un_contract_array;
	//订阅合约数
	//char* contract[MAX] = { 0 };
	stack<char*> s;
	int index = 0;
public:
	Ctp_Callback(CThostFtdcMdApi* User_Api) {
		this->User_Api = User_Api;
	}
public:
	//如果网络连接成功，这个函数被调用，并且请求登陆
	void OnFrontConnected() {
		cout << "=====连接成功=====" << endl;
		cout << "1、登陆     2、退出" << endl;
		int req;
		cin >> req;
		switch (req)
		{
		case 1:
			memset(&login_req, 0, sizeof(login_req));
			cout << "输入经纪商账号：";
			cin >> login_req.BrokerID;
			cout << "输入用户名：";
			cin >> login_req.UserID;
			cout << "输入密码：";
			cin >> login_req.Password;
			break;
		case 2:
			exit(0);
		default:
			cout << "输入错误!" << endl;
			exit(0);
			break;
		}

		//登陆请求响应
		static int RequestID = 0;
		int ret = User_Api->ReqUserLogin(&login_req, RequestID);
		switch (ret)
		{
		case 0:
			cout << "发送登陆请求成功" << endl;
			break;
		case -1:
			cout << "因网络原因发送登陆请求失败" << endl;
			break;
		case -2:
			cout << "未处理请求队列数量超限" << endl;
			break;
		case -3:
			cout << "每秒发送请求数量超限" << endl;
			break;
		default:
			cout << "未知错误" << endl;
			break;
		}

	}

	//如果网络连接失败，这个函数被调用，其中参数说明连接失败的原因
	void OnFrontDisconnected(int nReason) {
		switch (nReason)
		{
		case 0x1001:
			cout << "网络读失败" << endl;
			break;
		case 0x1002:
			cout << "网络写失败" << endl;
			break;
		case 0x2001:
			cout << "接收心跳超时" << endl;
			break;
		case 0x2002:
			cout << "发送心跳失败" << endl;
			break;
		case 0x2003:
			cout << "收到错误报文" << endl;
			break;
		default:
			cout << "未知错误" << endl;
			break;
		}

	}

	//交易系统对客户端的请求登录消息作出的响应
	//登陆成功后可以订阅、退订、登出、退出程序等操作
	void OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) {
		//登陆成功  ==0成功
		if (!pRspInfo->ErrorID) {
			cout << "=====登陆成功=====" << endl;
			cout << "欢迎" << pRspUserLogin->UserID << "!" << endl;
			cout << "交易日：" << pRspUserLogin->TradingDay << endl
				 << "登陆时间：" << pRspUserLogin->LoginTime << endl
				 << "经纪商：" << pRspUserLogin->BrokerID << endl;
		}
		//登录失败  !=0失败
		else {
			cout << "不合法的登录，请检查账号和密码" << endl;
			exit(0);
		}

		cout << "1、订阅     2、退订     3、登出     4、退出程序" << endl;
		int req;
		cin >> req;
		switch (req)
		{
		case 1: 
		{
			char* contract[MAX] = { 0 };
			int i = 0;
			cout << "订阅合约：" << endl;
			getchar();

			//输入合约名称
			while (i < MAX) {
				char* buffer = new char[10]{ 0 };
				cin >> buffer;
				s.push(buffer);
				if (!strcmp(buffer, "q"))
					break;
				contract[i] = buffer;
				i++;
			}
			contract_array.resize(i);

			//请求订阅
			int ret_sub = User_Api->SubscribeMarketData(contract, i);
			if (ret_sub == 0) {
				cout << "发送订阅请求成功！" << endl;
			}
			else {
				cout << "发送订阅请求失败！" << endl;
				cerr << "ErrorID:" << pRspInfo->ErrorID << " ErrorMessage:" << pRspInfo->ErrorMsg << endl;
			}
			break; 
		  }
		case 2:
		{//请求退订
			char* un_contract[MAX] = { 0 };
			int n = 0;
			cout << "合约退订：" << endl;
			getchar();
			while (n < MAX) {
				char* un_buffer = new char[10]{ 0 };
				cin >> un_buffer;
				s.push(un_buffer);
				if (!strcmp(un_buffer, "q"))
					break;
				un_contract[n] = un_buffer;
				n++;
			}
			un_contract_array.resize(n);
			int ret_unsub = User_Api->UnSubscribeMarketData(un_contract, n);
			if (ret_unsub == 0) {
				cout << "发送退订请求成功!" << endl;
			}
			else {
				cout << "发送退订请求失败!" << endl;
				cerr << "ErrorID:" << pRspInfo->ErrorID << " ErrorMessage:" << pRspInfo->ErrorMsg << endl;
			}
			break;
		}
		case 3:
		{
			static int un_RequestID = 0;
			//登出请求
			int ret_logout = User_Api->ReqUserLogout(&logou_req, un_RequestID);
			if (ret_logout == 0) {
				cout << "发出登出请求成功!" << endl;
			}
			else {
				cout << "发出登出请求失败!" << endl;
			}
			break;
		}
		case 4:
			exit(0);
			break;
		default:
			cout << "错误输入，结束程序！" << endl;
			exit(0);
			break;
		}

	}

	//登出请求响应
	void OnRspUserLogout(CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) {
		cout << "  sda";
		if (!pRspInfo->ErrorID) {
			cout << "登出成功" << endl;
		}
		else {
			cout << "登出失败" << endl;
		}
	}

	//响应订阅行情
	void OnRspSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) {
		bool back_result = pRspInfo && (pRspInfo->ErrorID != 0);
		if (back_result == 0) {
			cout << pSpecificInstrument->InstrumentID << "订阅成功！" << endl;
		}
		else {
			cout << "订阅失败！" << endl;
			cout << "ErrorID:" << pRspInfo->ErrorID << " ErrorMessage:" << pRspInfo->ErrorMsg << endl;
		}
	}

	//响应退订行情
	void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) {
		bool back_result = pRspInfo && (pRspInfo->ErrorID != 0);
		if (back_result == 0) {
			cout << pSpecificInstrument->InstrumentID << "退订成功！" << endl;
		}
		else {
			cout << "退订失败！" << endl;
			cout << "ErrorID:" << pRspInfo->ErrorID << " ErrorMessage:" << pRspInfo->ErrorMsg << endl;
		}
	}

	//深度行情显示
	void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData) {
		contract_array[index] = *pDepthMarketData;
		cout << "合约代码："<<pDepthMarketData->InstrumentID<< "    交易日："<<pDepthMarketData->ActionDay
			 <<"    最新价："<< pDepthMarketData->LastPrice << endl;
		index++;
		index%= contract_array.size();
	}

};