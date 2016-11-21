#include "linphonecore.h"
#include "private.h"

#include "common.h"
#include <string>

extern SipInfo  usrAuth; //�û���Ϣ
extern LinphoneCore *the_core;


//
bool WriteUserConfig()
{

   //����������ʱ�򣬼�������ļ��Ƿ���ڣ������������������д���ļ�
   //�������и�����Ҫ���ǣ����ԭ��Ϊʲô��dialoginit����ʹ������Ĵ���Ͳ����أ�����******

   //QSettings���캯���ĵ�һ��������ini�ļ���·��,�ڶ���������ʾ���ini�ļ�,��������������ȱʡ   
   QSettings *configIniWrite = new QSettings("userinfo.ini", QSettings::IniFormat);  
   //��ini�ļ���д������,setValue���������������Ǽ�ֵ��   
   //��ini�ļ��ĵ�һ����д������,ip���µĵ�һ������   
   configIniWrite->setValue("/sipuser/usr", "192.168.0.1");  
   //��ini�ļ��ĵ�һ����д������,ip���µĵڶ�������   
   configIniWrite->setValue("sipuser/pwd", "127.0.0.1");  
   //��ini�ļ��ĵڶ�����д������,port���µĵ�һ������   
   configIniWrite->setValue("sipuser/sipIP", "2222");  

   configIniWrite->setValue("sipuser/serverIP", "2222");  
   //д����ɺ�ɾ��ָ��   
   delete configIniWrite;  
   return true;
}

bool ReadUserConfig()
{

   QSettings *configIniRead = new QSettings("userinfo.ini", QSettings::IniFormat);  
   //����ȡ����ini�ļ�������QString�У���ȡֵ��Ȼ��ͨ��toString()����ת����QString����   
   QString usrStr = configIniRead->value("/sipuser/usr").toString();  
   strcpy(usrAuth.usrname,usrStr.toStdString().c_str());
  
   QString pwdStr = configIniRead->value("/sipuser/pwd").toString(); 
   strcpy(usrAuth.password,pwdStr.toStdString().c_str());
   
   QString sipIPStr = configIniRead->value("/sipuser/sipIP").toString(); 
   strcpy(usrAuth.sipip,sipIPStr.toStdString().c_str());

   QString serverIPStr = configIniRead->value("/sipuser/serverIP").toString(); 
   strcpy(usrAuth.serverip,serverIPStr.toStdString().c_str());

   //��ӡ�õ��Ľ��   
   qDebug()<<"sipIPStr" << usrAuth.usrname;  
   qDebug() <<"serverIPStr"<< serverIPStr;  
   //��������ɺ�ɾ��ָ��   
   delete configIniRead;  

   return true;
}

bool RegisterSip()
{
	char identity[128];
	ReadUserConfig();
	
	//���˳���ǰע��
	linphone_core_clear_proxy_config(the_core);
	
	//��ȡע����Ϣ
	//QString username,password,server,szidentity;

	//username.Format(_T("%s"),usrAuth.usrname);
	//password.Format(_T("%s"),usrAuth.password);
	//server.Format(_T("%s"),usrAuth.sipip);
	//if (username.IsEmpty() || password.IsEmpty() || server.IsEmpty())
	//{
	//	return -1;
	//}
	if((strlen(usrAuth.usrname)==0)||(strlen(usrAuth.password)==0)||(strlen(usrAuth.sipip)==0))
		return false;

	//����ע���ʽ
	//strcpy(szidentity,"sip:%s@%s");
	//strcat(szidentity,usrAuth.usrname);
	//strcat(szidentity,usrAuth.);
	sprintf(identity,"sip:%s@%s",usrAuth.usrname,usrAuth.sipip);

	//szidentity.Format(_T("sip:%s@%s"),username.GetString(),server.GetString());
	//AfxMessageBox(szidentity);

    //������ַ�תchar�ͣ��鷳���ˣ�
    //USES_CONVERSION;
	//const char *identity = (LPSTR)(LPCTSTR)szidentity;
	//const char *secret = (LPSTR)(LPCTSTR)password;
	//const char *host = (LPSTR)(LPCTSTR)server;



	//����Ƿ��Ѿ�ע�ᣬ���û��ע����ע��
	LinphoneProxyConfig* proxyCfg=NULL;	

	//LinphoneProxyConfig* proxyCfg=new LinphoneProxyConfig;	
	linphone_core_get_default_proxy(the_core,&proxyCfg);
	if (proxyCfg!=NULL)
	{
		if (linphone_proxy_config_is_registered(proxyCfg))
		{
			return false;
		}else
		{
			linphone_core_clear_proxy_config(the_core);
		}
		
	}
	//Ӧ����д������Ϣ
	proxyCfg = linphone_proxy_config_new();

	// add username password
	LinphoneAddress *from = linphone_address_new(identity);
	LinphoneAuthInfo *info;
	if (from !=0){
		info=linphone_auth_info_new(linphone_address_get_username(from),NULL,usrAuth.password,NULL,NULL);
		linphone_core_add_auth_info(the_core,info);
	}
	linphone_address_destroy(from);

	// configure proxy entries
	linphone_proxy_config_set_identity(proxyCfg,identity);
	linphone_proxy_config_set_server_addr(proxyCfg,usrAuth.sipip);
	linphone_proxy_config_enable_register(proxyCfg,true);
	linphone_proxy_config_set_dial_escape_plus(proxyCfg,TRUE);
	linphone_core_add_proxy_config(the_core,proxyCfg);

	//set to default proxy
	linphone_core_set_default_proxy(the_core,proxyCfg);
	return true;
}


//�����ֽ�char*ת��Ϊ���ֽ�wchar_t*  
wchar_t* AnsiToUnicode( const char* szStr )  
{  
    int nLen = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szStr, -1, NULL, 0 );  
    if (nLen == 0)  
    {  
        return NULL;  
    }  
    wchar_t* pResult = new wchar_t[nLen];  
    MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szStr, -1, pResult, nLen );  
    return pResult;  
}  

//�����ֽ�wchar_t*ת��Ϊ���ֽ�char*  
char* UnicodeToAnsi( const wchar_t* szStr )  
{  
    int nLen = WideCharToMultiByte( CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL );  
    if (nLen == 0)  
    {  
        return NULL;  
    }  
    char* pResult = new char[nLen];  
    WideCharToMultiByte( CP_ACP, 0, szStr, -1, pResult, nLen, NULL, NULL );  
    return pResult;  
}  



std::string ws2s(const std::wstring& ws)
{
    std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
    setlocale(LC_ALL, "chs");
    const wchar_t* _Source = ws.c_str();
    size_t _Dsize = 2 * ws.size() + 1;
    char *_Dest = new char[_Dsize];
    memset(_Dest,0,_Dsize);
    wcstombs(_Dest,_Source,_Dsize);
    std::string result = _Dest;
    delete []_Dest;
    setlocale(LC_ALL, curLocale.c_str());
    return result;
}

  std::string WStringToString(const std::wstring &wstr)
  {
      std::string str(wstr.length(), ' ');
      std::copy(wstr.begin(), wstr.end(), str.begin());
      return str; 
  }

/*
std::wstring s2ws(const std::string& s)
{
    setlocale(LC_ALL, "chs"); 
    const char* _Source = s.c_str();
    size_t _Dsize = s.size() + 1;
    wchar_t *_Dest = new wchar_t[_Dsize];
    wmemset(_Dest, 0, _Dsize);
    mbstowcs(_Dest,_Source,_Dsize);
    std::wstring result = _Dest;
    delete []_Dest;
    setlocale(LC_ALL, "C");
    return result;
}
*/

std::string EncodeUtf8(std::wstring in)  
{  
    std::string s(in.length() * 3, ' ');  
    size_t len = ::WideCharToMultiByte(CP_UTF8, 0,  
                  in.c_str(), in.length(),  
                  &s[0], s.length(),  
                  NULL, NULL);  
    s.resize(len);  
    return s;  
}  

wchar_t* MulityByteToWideChar(int CodePage, char *str)
{
 DWORD dwNum = MultiByteToWideChar(CodePage, 0, str, -1, 0, 0);
 wchar_t *pwText = new wchar_t[dwNum];
 MultiByteToWideChar(CodePage, 0, str, -1, pwText, dwNum); 
 return pwText;
}


char* WideCharToMulityByte(int CodePage, wchar_t *str)
{
 int len = WideCharToMultiByte(CodePage, 0, str, -1, 0, 0, 0, 0); 
 char* output = new char[len + 2];
 WideCharToMultiByte(CodePage, 0, str, -1, output, len + 1, 0, 0); 
 return output;
}


/****
*para src fileinfo include path and filename,input
*para filename oubput
*para dirname output
*****/
bool GetFileInfo(char *src,char *filename,char *dirname)
{
    int total_len=0;
    int sub_len=0;
    char *p=NULL;
    total_len=strlen(src);
    p=strrchr(src,'/')+1;
    strcpy(filename,p);
    printf("file name is :%s\n",filename);
    sub_len=strlen(p);
    src[total_len-sub_len]='\0';
    strcpy(dirname,src);
    return true;
}
