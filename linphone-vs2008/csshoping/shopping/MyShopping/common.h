#ifndef _COMMON_H
#define _COMMON_H

#include <QSettings>  
#include <QDebug>

#include "Markup.h"
#include< iostream>
using namespace std;

//webservice�ӿڴ������
typedef struct WebServiceRes{
	char Code[8];
	char ErrDesc[128];
}WebServiceRes;

//sip������ע����Ϣ
typedef struct SipInfo{
	char usrname[64];
	char password[64];
	char sipip[64];
	char serverip[64];
}SipInfo;

//��Ʒ����
typedef struct GoogsClass{
	char goodsClassID[64];
	char goodsClassName[64];
	char goodsClassScal[64];
	char goodsClassOrder[64];
}GoogsClass;

//������Ʒ
typedef struct ProductsDetail{
	char homeBuyId[64];
	char product[64];
	char unit[64];
	char quantity[64];
}ProductsDetail;

//������Ϣ  
typedef struct GuestOrder{
	char orderId[64];
	char buyer[512];//��ַ��Ϣ
	char buyDate[64];
	char bizDispatch[32];
	char bizPay[32];
	char distance[32];
	char ps_address[128];
	char ps_datetime[128];
	char SIPnum[32];
	char phone[32];
	char ps_name[64];
	char orderNum[64];
	char from[64];//03�ֻ� 10������
	char orderStatus[32];//�Ѿ��ֵ�
	char orderMode[32];//00 ԤԼ 01 ��ͨ 02 �Ź� 03�˻� 04����
	ProductsDetail guestGoods[32];//�����Ǽ���˿�ͬʱ����64
	int totalPD;
}GuestOrder;

//����״̬�ֵ�
typedef struct GoodsDics{
	char statename[32];
	char statecode[32];
}GoodsDics;


//��Ʒ��Ϣ
typedef struct ProductInfo{
	char productId[64];
	char Itemid[64];
	char Deptcode[64];
	char Itemcode[64];
	char Locname[64];
	char Shortname[64];
	char Djyzt[64]; //00ʵʱ 01ԤԼ 02�Ź�
	char Dspjs[1024];
	char Origin[64];
	char Itemmodel[64];
	char Produce2[64];
	char Unit[64];
	char Classid[64];
	char PiclinkL[64];
	char PiclinkS[64];
	char Source[64];
	char Price1[64];
	char Price2[64];
	char Price3[64];
	char Price4[64];
	char weightvalue[64];

	//����
	char quantity[32];
	char pwd[32];

	char systemdate[64];
	char limit[64];
}ProductInfo;

//����Ա��Ϣ
typedef struct SenderInfo
{
	char senderId[64];
	char senderPhone[11];
	char senderName[32];
}SenderInfo;

//��������Ա��Ϣ
typedef struct Order2Sender
{	char orderID[64];
	char senderName[64];
}Order2Sender;

/*
//��ʼ��url
typedef struct UrlSets
{
	char *gsoapADDR;
	char *downddXML;//�����б����ص�ַ
	char *downdicsXML;
	char *changedsXML;
	char *
}UrlSets;
*/

bool WriteUserConfig();
bool ReadUserConfig();
bool RegisterSip();
bool GetFileInfo(char *src,char *filename,char *dirname);
wchar_t* AnsiToUnicode( const char* szStr );
char* UnicodeToAnsi( const wchar_t* szStr );
std::string ws2s(const std::wstring& ws);
std::string WStringToString(const std::wstring &wstr);
std::string EncodeUtf8(std::wstring in)  ;

wchar_t* MulityByteToWideChar(int CodePage, char *str);
char* WideCharToMulityByte(int CodePage, wchar_t *str);



#endif