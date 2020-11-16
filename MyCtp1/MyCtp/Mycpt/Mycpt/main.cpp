#include"ThostFtdcTraderApi.h"
#include"ThostFtdcUserApiDataType.h"
#include"ThostFtdcUserApiStruct.h"
#include"My_Ctp.cpp"

#include<iostream>

int main() {
	char front_addr[] = "tcp://218.202.237.33:10112";
	CThostFtdcMdApi* User_Api = CThostFtdcMdApi::CreateFtdcMdApi();//创建了一个Api操作对象，用于请求等操作，比如请求登录
	CThostFtdcMdSpi* User_Spi = new Ctp_Callback(User_Api);

	User_Api->RegisterSpi(User_Spi);
	User_Api->RegisterFront(front_addr);

	User_Api->Init();
	User_Api->Join();
}