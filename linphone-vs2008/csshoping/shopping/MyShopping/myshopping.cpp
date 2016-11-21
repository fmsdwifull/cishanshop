//������������ǰ�棬��Ȼ�ͱ�һ�ƴ���
#include "linphonecore.h"
#include "private.h"


#include "myshopping.h"

#include <QtCore>
#include <qmessagebox.h>
#include <QDir>
#include <QtDebug>
#include <QString>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QtNetwork>
#include <QMap>
#include <QStandardItemModel>
#include <QtGui/QDialog>
#include <QStringListModel>
#include <QScrollBar>
#include <QMovie>
#include <QDate>
#include <QDesktopWidget>


#include "DistributeDlg.h"
#include "shopsubmit.h"



#define PHONE_CORE_TIMER 100

static LinphoneCoreVTable vTable;
static const char *config_file = "shopcfg.ini";
static const char *factoryConfig = "shopcfg.ini";
LinphoneCore *the_core=NULL;




//������ť״̬
#define INIT_STATE     0   //��ʼ��״̬
#define CALLING_STATE  1   //����ͨ��״̬



//��ʾ�������Ǻ���
int callBtnFlag;

//���������û�������ˮ�ź�����Ա
Order2Sender os;

//�˺ŵ�½��Ϣ������sip�˺ţ���������ַ��
extern SipInfo  usrAuth; //�û���Ϣ




static void linphone_log_handler(OrtpLogLevel lev, const char *fmt, va_list args)
{
	const char *lname="undef";
	char *msg;
	char *msg_str;

	switch(lev)
	{
			case ORTP_DEBUG:
				lname="DEBUG";
				break;
			case ORTP_MESSAGE:
				lname="MESSAGE";
				break;
			case ORTP_WARNING:
				lname="WARING";
				break;
			case ORTP_ERROR:
				lname="ERROR";
				break;
			case ORTP_FATAL:
				lname="FATAL";
				break;
			default:
				lname = ("Bad level !");
	}
	msg=ortp_strdup_vprintf(fmt,args);
	msg_str = ortp_strdup_printf("%s: %s\r\n",lname,msg);
#ifdef _MSC_VER
//	OutputDebugString(msg_str);
#endif
	ms_free(msg);
	ms_free(msg_str);
}
//���磬�򵯳�����Ի���
static void showInterfaceCb(LinphoneCore *lc)
{

}

static  void inviteReceivedCb(LinphoneCore *lc, const char *from) 
{
	MyShopping *the_ui = (MyShopping *)linphone_core_get_user_data(lc);
	if(the_ui)
		the_ui->ui.callStateLabel->setText(from); 

	the_ui->ui.sipcallBtn->setText("���� ");
	callBtnFlag=CALLING_STATE;

	QString msg(from);

	for(int i=0; i<msg.length(); i++)
	{
			  if(msg.at(i) == '"')
			  {
				  int j=0;
				  while(msg.at(++j)!='"')
				  {
					  
					  the_ui->sipTel += msg.at(j);
				  }
			  }
			  break;
			}
	//QMessageBox::warning(NULL,"-----",fromNum);

	//msg.at(0)


	//QMessageBox::warning(,"xxxxxxxxx",from);
}

static void byeReceivedCb(LinphoneCore *lc, const char *from) 
{

}

static void displayStatusCb(LinphoneCore *lc, const char *message) 
{

	MyShopping *the_ui=(MyShopping*)linphone_core_get_user_data(lc);
	the_ui->ui.callStateLabel->setText(message);
#if 0
	CcoffeeroomDlg *the_ui = (CcoffeeroomDlg*)linphone_core_get_user_data(lc);
	CString msg(message);
	//AfxMessageBox(msg);
	if(msg.Find(_T("successful"))>=0)
	{
		CString usrsipTmp(usrAuth.usrname);
		the_ui->m_editInfo.SetWindowText(_T("ע��ɹ������ĺ���Ϊ��")+usrsipTmp);
	}
		//��ȡ�Է�����
	if(msg.Find(_T("@"))>=0)
	{	
			//fromNum.Empty();
			//��ȡ�Է��绰���룬��ȡ��ʽ�ǻ�ȡ˫�����ڵ����ݣ�����ǵ绰����
			for(int i=0; i<msg.GetLength(); i++)
			{
			  if(msg.GetAt(i) == '"')
			  {
				  int j=0;
				  while(msg.GetAt(++j)!='"')
				  {
					  fromNum += msg.GetAt(j);
				  }
			  }
			  break;
			}

			//�绰��������O
			if(msg.Find("is contacting you")>=0)
			{
				toNum.Empty();
				fromNum.Empty();

				for(int i=0; i<msg.GetLength(); i++)
				{
					if(msg.GetAt(i) == ':')
					  {
						  while(msg.GetAt(++i)!='@')
						  {
							  fromNum += msg.GetAt(i);
						  }
						  break;
					  }
					  
					}

			}

			
			//��ѯ����
			GUEST_t pLook;
			if(m_GuestMapNum.Lookup(fromNum,pLook))
			{
				the_ui->m_editInfo.SetWindowText( pLook.guestNum+ _T(" ����λ���ں���") );
			}
		}

		//�޷���ͨ
		if(msg.Find(_T("User is busy"))>=0)
		{
			the_ui->m_editInfo.SetWindowText( _T("�Է��绰�ѹػ������Ժ��ٲ���"));	
		}

		//���˽���
		if(msg.Find(_T("Call declined"))>=0)
		{
			the_ui->m_editInfo.SetWindowText( _T("�Է����ڣ��绰���˽�����"));	
		}

		//ͨ������
		if((msg.Find(_T("Call terminated"))>=0)||(msg.Find(_T("Call ended"))>=0))
		{
			the_ui->m_editInfo.SetWindowText( _T("ͨ��������"));
			::SendMessage(m_mainWnd,WM_MYMESSAGE,0,(LPARAM)"callover");
		}

		//���ں���
		if(msg.Find(_T("Remote ringing"))>=0)
		{
			the_ui->m_editInfo.SetWindowText( _T("���ں���")+toNum+_T(" ���Ժ�"));	
		}

		//����ͨ��
		if(msg.Find(_T("Connected"))>=0)
		{
			if(toNum.GetLength()!=0)
			{
				//��ѯ����
				UINT XXX=toNum.GetLength();
				GUEST_t pLook;
				if(m_GuestMapNum.Lookup(toNum,pLook))
				{
					the_ui->m_editInfo.SetWindowText( pLook.guestNum+ _T(" ����λ����ͨ��") );
					::SendMessage(m_mainWnd,WM_MYMESSAGE,0,(LPARAM)"talking");
				}
			}else{
				//��ѯ����
				//AfxMessageBox(fromNum);
				GUEST_t pLook;
				if(m_GuestMapNum.Lookup(fromNum,pLook))
				{
					the_ui->m_editInfo.SetWindowText( pLook.guestNum+ _T(" ����λ����ͨ��") );
					::SendMessage(m_mainWnd,WM_MYMESSAGE,0,(LPARAM)"talking");
				}
			}
		}

		//����ʧ��
		if(msg.Find(_T("Call failed"))>=0)
		{
			the_ui->m_editInfo.SetWindowText( _T("����ʧ�ܣ�"));

		}
#endif
}

static void displayMessageCb(LinphoneCore *lc, const char *message) 
{
	//QMessageBox::about(this,"Slot test","Ok2");
	printf("XXXXXXX2");
	QString path = QDir::currentPath();
	qDebug() << "currentPath:" << path; 
}
static void authInfoRequested(LinphoneCore *lc, const char *realm, const char *username) 
{
}
static void generalStateChange(LinphoneCore *lc, LinphoneGeneralState *gstate) 
{
}

static void network_quality(LinphoneCore *lc, int ms)
{
}

static void media_quality(LinphoneCore *lc, int jitter, float packet_lost)
{
}


MyShopping::MyShopping(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags)
{
	ui.setupUi(this);

	//��ʼ��linphonelib
	linphone_core_enable_logs_with_cb(linphone_log_handler);

	memset(&vTable,0,sizeof(vTable));
	vTable.show = showInterfaceCb;
	vTable.inv_recv = inviteReceivedCb;
	vTable.auth_info_requested = authInfoRequested;
	vTable.display_status = displayStatusCb;
	vTable.display_message = displayMessageCb;
	vTable.display_warning = displayMessageCb;
	vTable.general_state = generalStateChange;
	vTable.network_quality_monitor = network_quality;
	vTable.media_quality_monitor = media_quality;

	the_core = linphone_core_new(	&vTable
		,config_file
		,factoryConfig
		,this);
	linphone_core_set_playback_gain_db(the_core,1.0);

	//��ʼ����Ƶ��ʾ����
	//linphone_core_set_native_video_window_id(the_core,(unsigned long)ui.label_3->winId());
	QTimer *timer = new QTimer();
	
	//�½���ʱ��
	connect(timer,SIGNAL(timeout()),this,SLOT(linponeIterate()));
	timer->start(30); 

	//����Ĭ��ҳ��
	ui.stackedWidget->setCurrentIndex(0);
	
	//��Ʒ�б�ˮƽ��ֱ������λ��
	horposion=0;
	verposion=0;
	//��ʼ����ǰ���ѡ����Ϊ-1
	currentRow=-1;
	//��ʼ���绰����
	sipTel="-------------";

	//��ǰ��ѯ����
	currentQueryCombo=0; //Ĭ��Ϊȫ��

	//�����û�����xml
	manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinished(QNetworkReply*)));
	QString ipStr(usrAuth.serverip);
	QString tmpStr1(tr("/OAapp/WebObjects/OAapp.woa?cjApply=bizorder"));
    manager->get(QNetworkRequest(QUrl(ipStr+tmpStr1)));

	//������ʷ����
	managerHistory = new QNetworkAccessManager(this);
    connect(managerHistory, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyHisFinished(QNetworkReply*)));

	//�޸��û�����״̬
	googstate = new QNetworkAccessManager(this);
	connect(googstate, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyGoodState(QNetworkReply*)));

	//�����û�״̬xml
	goodsdic = new QNetworkAccessManager(this);
    //���������ֵ�
	QString tmpStr2(tr("/OAapp/WebObjects/OAapp.woa?cjApply=orderStaDic&ret=xml"));
    goodsdic->get(QNetworkRequest(QUrl(ipStr+tmpStr2)));
	qDebug()<<"״̬�ֵ�"<<ipStr+tmpStr2;
	connect(goodsdic, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyGoodDics(QNetworkReply*)));

	//��ѯ����comboBox�ۺ���
	connect(ui.queryComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(conditionProcess(int))); 

	//�û��������ʼ��
	initModel=new QStandardItemModel(50, 10, this);
    updateGuest();//byliu
	//��ʱˢ���û������б�
	QTimer *GuestTimer = new QTimer();
	connect(GuestTimer,SIGNAL(timeout()),this,SLOT(updateGuest()));
	GuestTimer->start(4*1000); 

	//��������
	callTimer = new QTimer();
	connect(callTimer,SIGNAL(timeout()),this,SLOT(updateColor()));
	callTimer->start(500); 

	ui.tableView->setEditTriggers ( QAbstractItemView::NoEditTriggers );
	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableView->setModel(initModel);
	ui.tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch) ;//����Զ���Ӧ

	 //��ʷ����
    historyModel=new QStandardItemModel(100, 13, this);

	ui.tableHistoryView->setEditTriggers ( QAbstractItemView::NoEditTriggers );
	ui.tableHistoryView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableHistoryView->setModel(historyModel);
	
	//��ʼѡ�е�һ��
	ui.tableView->selectRow(0);
	updateHistory();

    //���ú���
    //resizeColumnsToContents 
    //this->setCentralWidget(ui.tableView);
    //QModelIndex index = tableView->currentIndex();

    // ��ȡ��ͼ����Ŀѡ��ģ��
    QItemSelectionModel *selectionModel = ui.tableView->selectionModel();
    connect(selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)),this, SLOT(changeCurrent(QModelIndex,QModelIndex)));

	//��ʼ��listģ��
	listModel = new QStringListModel;

	//label��ͼƬ
	//pix=QPixmap("E:\QTpro\farmer\1.png");
    //ui->label->setPixmap(pix);
	//ui.statelabel->setPixmap(QPixmap("state.jpg"));

	QMovie *movie = new QMovie("E:/3.gif");
    ui.statelabel->setMovie(movie);
    movie->start();
	
	//��ʼ����������
	QStringList liststr;
	QMap<QString,GoodsDics>::const_iterator go_iter;
	int i=0;
	for(go_iter=map_PsstatesNo.constBegin(); go_iter!=map_PsstatesNo.constEnd(); go_iter++)
	{
		liststr<<go_iter.value().statename;
		if(go_iter.value().statename==tr("������"))
		{
			tmpSenderNum=i;
		}
		i++;
	}
	ui.comboBox->addItems(liststr);

	//��ʼ��������ť״̬
	callBtnFlag=INIT_STATE;

	//������
	connect(ui.tableView->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(verMove(int)));
	connect(ui.tableView->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(horMove(int)));//sliderMoved
}

MyShopping::~MyShopping()
{

}


void MyShopping::linponeIterate()
{
	   if (the_core != NULL)
	   {
			linphone_core_iterate(the_core);
	   }
}

void MyShopping::on_addGoodsBtn_clicked()
{
	shopsubmit *ssDlg=new shopsubmit();
	ssDlg->show();
}

void MyShopping::on_homeBtn_clicked()
{
	ui.stackedWidget->setCurrentIndex(0);
}

void MyShopping::on_AppointBtn_clicked()
{
	ui.stackedWidget->setCurrentIndex(1);
}

void MyShopping::on_HistoryBtn_clicked()
{
	QString ipStr(usrAuth.serverip);	

	QString tmpStr1(tr("/OAapp/WebObjects/OAapp.woa?cjApply=bizorder&sta=03"));
	managerHistory->get(QNetworkRequest(QUrl(ipStr+tmpStr1)));
	qDebug()<<"ȫ����ʷ���������б�"<<ipStr+tmpStr1;
	//��ʷ���������ʼ��
	
	QStringList liststr;
	liststr<<"��ѯȫ��"<<"������ѯ";
	ui.queryComboBox->addItems(liststr);

	//ģʽ�����б�
	QStringList liststr2;
	liststr2<<"ԤԼ"<<"��ͨ"<<"�Ź�"<<"�˻�"<<"����"<<"δ����";
	ui.modelComboBox->addItems(liststr2);


	ui.stackedWidget->setCurrentIndex(2);
}


//byliu
void MyShopping::replyHisFinished(QNetworkReply *reply)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString all = codec->toUnicode(reply->readAll());

    QFile file("goodshisory.xml");  
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))  
    {  
        qDebug()<< "Open failed." ;  
        return;  
    }  
      
    QTextStream txtOutput(&file);  
    txtOutput << all << endl;  
    file.close(); 

#if 1
	//����Ӧ�û��е����⣬����xml�ͼ�����ʾ����ͬ��������ʱ���ٴ���
	updateHistory();

	//����xml
	historyXML();

	//����map��ʾ��������
	iterHistoryMap();
#endif
}

void MyShopping::replyFinished(QNetworkReply *reply)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString all = codec->toUnicode(reply->readAll());

    QFile file("goods.xml");  
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))  
    {  
        qDebug()<< "Open failed." ;  
        return;  
    }  
      
    QTextStream txtOutput(&file);  
    txtOutput << all << endl;  
      
    file.close(); 
}

void MyShopping::replyGoodDics(QNetworkReply *reply)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString all = codec->toUnicode(reply->readAll());

    QFile file("goodsdic.xml");  
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))  
    {  
        qDebug()<< "Open failed." ;  
        return;  
    }  
      
    QTextStream txtOutput(&file);  
    txtOutput << all << endl;  
    file.close(); 
}


void MyShopping::loadDicXML()
{
    CMarkup xml;
	string str="goodsdic.xml";
	bool xx=xml.Load(str);
    xml.ResetMainPos();   
	int id=0;

	while (xml.FindChildElem("orderSta"))
	{
		GoodsDics m_gd; 
		xml.IntoElem();


		xml.FindChildElem("ORDERSTACODE");	
		string codeStr= xml.GetChildData();
		strcpy(m_gd.statecode,codeStr.c_str());

		xml.FindChildElem("ORDERSTANAME");	
		string nameStr= xml.GetChildData();
		strcpy(m_gd.statename,nameStr.c_str());

		QString strTmp1(m_gd.statecode);
		QString strTmp2(m_gd.statename);

		//QMessageBox::warning(this,"666666",tr(m_gd.statecode)+tr(m_gd.statename));
		map_PsstatesNo.insert(strTmp1,m_gd);
		

		map_PsstatesMsg.insert(strTmp2,m_gd);
		xml.OutOfElem();
	}
}



void MyShopping::loadXML()
{
    CMarkup xml;
	string str="goods.xml";

    bool xx=xml.Load(str);
    xml.ResetMainPos();   
	int id=0;
	while (xml.FindChildElem("order"))
	{
		id++;
		GuestOrder m_guestorder;
		xml.IntoElem();

		xml.FindChildElem("orderId");	
		string orderIdStr= xml.GetChildData();
		strcpy(m_guestorder.orderId,orderIdStr.c_str());

		xml.FindChildElem("buyer");	
		string buyerStr= xml.GetChildData();
		strcpy(m_guestorder.buyer,buyerStr.c_str());

		xml.FindChildElem("buyDate");	
		string buyDateStr= xml.GetChildData();
		strcpy(m_guestorder.buyDate,buyDateStr.c_str());

		xml.FindChildElem("bizDispatch");	
		string bizDispatchStr= xml.GetChildData();
		strcpy(m_guestorder.bizDispatch,bizDispatchStr.c_str());


		xml.FindChildElem("bizPay");	
		string bizPayStr= xml.GetChildData();
		strcpy(m_guestorder.bizPay,bizPayStr.c_str());


		//xml.FindChildElem("distance");	
		//string disStr= xml.GetChildData();
		//strcpy(m_guestorder.distance,disStr.c_str());

		xml.FindChildElem("ps_address");	
		string addressStr= xml.GetChildData();
		strcpy(m_guestorder.ps_address,addressStr.c_str());

		xml.FindChildElem("ps_datetime");	
		string datetimeStr= xml.GetChildData();
		strcpy(m_guestorder.ps_datetime,datetimeStr.c_str());

		xml.FindChildElem("SIPnum");	
		string sipnumStr= xml.GetChildData();
		strcpy(m_guestorder.SIPnum,sipnumStr.c_str());//<phone>15027060270</phone> 

		xml.FindChildElem("phone");	
		string phoneStr= xml.GetChildData();
		strcpy(m_guestorder.phone,phoneStr.c_str());


		xml.FindChildElem("ps_name");	
		string ps_nameStr= xml.GetChildData();
		strcpy(m_guestorder.ps_name,ps_nameStr.c_str());

		xml.FindChildElem("orderNum");	
		string orderNumStr= xml.GetChildData();
		strcpy(m_guestorder.orderNum,orderNumStr.c_str());

		xml.FindChildElem("from");	
		string fromStr= xml.GetChildData();
		//strcpy(m_guestorder.from,fromStr.c_str());
		if(tr("03")==QString::fromStdString(fromStr))
			strcpy(m_guestorder.from,"�ֻ�");
		else if(tr("10")==QString::fromStdString(fromStr))
			strcpy(m_guestorder.from,"������");
		else
			strcpy(m_guestorder.from,"δ����");


		xml.FindChildElem("orderStatus");	
		string stdStr= xml.GetChildData();
		QString orderStatusStr = QString::fromStdString(stdStr);
		QString stateStr;
		if(orderStatusStr.isEmpty())
		{
			strcpy(m_guestorder.orderStatus,"----");
		}else{
			QMap<QString,GoodsDics>::const_iterator go_iter;
			go_iter=map_PsstatesNo.find(orderStatusStr);
			strcpy(m_guestorder.orderStatus,go_iter.value().statename);
			//QMessageBox::warning(this,"xxxx",orderStatusStr+tr(go_iter.value().statename));//byliu
		}
		

		xml.FindChildElem("orderMode");	
		string orderModeStr= xml.GetChildData();
		//strcpy(m_guestorder.orderMode,orderModeStr.c_str());
		if(tr("00")==QString::fromStdString(orderModeStr))
			strcpy(m_guestorder.orderMode,"ԤԼ");
		else if(tr("01")==QString::fromStdString(orderModeStr))
			strcpy(m_guestorder.orderMode,"��ͨ");
		else if(tr("02")==QString::fromStdString(orderModeStr))
			strcpy(m_guestorder.orderMode,"�Ź�");
		else if(tr("03")==QString::fromStdString(orderModeStr))
			strcpy(m_guestorder.orderMode,"�˻�");
		else if(tr("04")==QString::fromStdString(orderModeStr))
			strcpy(m_guestorder.orderMode,"����");
		else
			strcpy(m_guestorder.orderMode,"δ����");
		
		int i=0;
		while (xml.FindChildElem("productsDetail"))
		{
			xml.IntoElem();
			xml.FindChildElem("homeBuyId");	
			string homeBuyIdStr= xml.GetChildData();
			strcpy(m_guestorder.guestGoods[i].homeBuyId,homeBuyIdStr.c_str());


			xml.FindChildElem("product");	
			string productStr= xml.GetChildData();
			strcpy(m_guestorder.guestGoods[i].product,productStr.c_str());
			
			xml.FindChildElem("unit");	
			string unitStr= xml.GetChildData();
			strcpy(m_guestorder.guestGoods[i].unit,unitStr.c_str());

			xml.FindChildElem("quantity");	
			string quantityStr= xml.GetChildData();
			strcpy(m_guestorder.guestGoods[i].quantity,quantityStr.c_str());

			xml.OutOfElem();

			i++;
			m_guestorder.totalPD=i;
		}

		xml.OutOfElem();

		QString strTmp(m_guestorder.orderNum);

		m_guestmap.insert(strTmp,m_guestorder);//orderIdStr

		QString strTmpTel(m_guestorder.SIPnum);
		m_guestmapTel.insert(strTmpTel,m_guestorder);//orderIdStr
	}
}


void MyShopping::historyXML()
{
    CMarkup xml;
	string str="goodshisory.xml";

    bool xx=xml.Load(str);
    xml.ResetMainPos();   
	int id=0;
	while (xml.FindChildElem("order"))
	{
		id++;
		GuestOrder m_guestorder;
		xml.IntoElem();

		xml.FindChildElem("orderId");	
		string orderIdStr= xml.GetChildData();
		strcpy(m_guestorder.orderId,orderIdStr.c_str());

		xml.FindChildElem("buyer");	
		string buyerStr= xml.GetChildData();
		strcpy(m_guestorder.buyer,buyerStr.c_str());

		xml.FindChildElem("buyDate");	
		string buyDateStr= xml.GetChildData();
		strcpy(m_guestorder.buyDate,buyDateStr.c_str());

		xml.FindChildElem("bizDispatch");	
		string bizDispatchStr= xml.GetChildData();
		strcpy(m_guestorder.bizDispatch,bizDispatchStr.c_str());

		xml.FindChildElem("bizPay");	
		string bizPayStr= xml.GetChildData();
		strcpy(m_guestorder.bizPay,bizPayStr.c_str());

		xml.FindChildElem("distance");	
		string disStr= xml.GetChildData();
		strcpy(m_guestorder.distance,disStr.c_str());

		xml.FindChildElem("ps_address");	
		string addressStr= xml.GetChildData();
		strcpy(m_guestorder.ps_address,addressStr.c_str());

		xml.FindChildElem("ps_datetime");	
		string datetimeStr= xml.GetChildData();
		strcpy(m_guestorder.ps_datetime,datetimeStr.c_str());

		xml.FindChildElem("SIPnum");	
		string sipnumStr= xml.GetChildData();
		strcpy(m_guestorder.SIPnum,sipnumStr.c_str());


		xml.FindChildElem("ps_name");	
		string ps_nameStr= xml.GetChildData();
		strcpy(m_guestorder.ps_name,ps_nameStr.c_str());

		xml.FindChildElem("orderNum");	
		string orderNumStr= xml.GetChildData();
		strcpy(m_guestorder.orderNum,orderNumStr.c_str());

		xml.FindChildElem("from");	
		string fromStr= xml.GetChildData();
		//strcpy(m_guestorder.from,fromStr.c_str());
		if(tr("03")==QString::fromStdString(fromStr))
			strcpy(m_guestorder.from,"�ֻ�");
		else if(tr("10")==QString::fromStdString(fromStr))
			strcpy(m_guestorder.from,"������");
		else
			strcpy(m_guestorder.from,"δ����");

		xml.FindChildElem("orderStatus");	
		string stdStr= xml.GetChildData();
		QString orderStatusStr = QString::fromStdString(stdStr);

		QString stateStr;

		if(orderStatusStr.isEmpty())
		{
			strcpy(m_guestorder.orderStatus,"----");
		}else{
			QMap<QString,GoodsDics>::const_iterator go_iter;
			go_iter=map_PsstatesNo.find(orderStatusStr);
			strcpy(m_guestorder.orderStatus,go_iter.value().statename);
		}
		

		xml.FindChildElem("orderMode");	
		string orderModeStr= xml.GetChildData();
		//strcpy(m_guestorder.orderMode,orderModeStr.c_str());
		if(tr("00")==QString::fromStdString(orderModeStr))
			strcpy(m_guestorder.orderMode,"ԤԼ");
		else if(tr("01")==QString::fromStdString(orderModeStr))
			strcpy(m_guestorder.orderMode,"��ͨ");
		else if(tr("02")==QString::fromStdString(orderModeStr))
			strcpy(m_guestorder.orderMode,"�Ź�");
		else if(tr("03")==QString::fromStdString(orderModeStr))
			strcpy(m_guestorder.orderMode,"�˻�");
		else if(tr("04")==QString::fromStdString(orderModeStr))
			strcpy(m_guestorder.orderMode,"����");
		else
			strcpy(m_guestorder.orderMode,"δ����");

		int i=0;
		while (xml.FindChildElem("productsDetail"))
		{
			xml.IntoElem();
			xml.FindChildElem("homeBuyId");	
			string homeBuyIdStr= xml.GetChildData();
			strcpy(m_guestorder.guestGoods[i].homeBuyId,homeBuyIdStr.c_str());


			xml.FindChildElem("product");	
			string productStr= xml.GetChildData();
			strcpy(m_guestorder.guestGoods[i].product,productStr.c_str());
			
			xml.FindChildElem("unit");	
			string unitStr= xml.GetChildData();
			strcpy(m_guestorder.guestGoods[i].unit,unitStr.c_str());

			xml.FindChildElem("quantity");	
			string quantityStr= xml.GetChildData();
			strcpy(m_guestorder.guestGoods[i].quantity,quantityStr.c_str());

			xml.OutOfElem();

			i++;
			m_guestorder.totalPD=i;
		}

		xml.OutOfElem();

		QString strTmp(m_guestorder.orderNum);
		m_historymap.insert(strTmp,m_guestorder);//orderIdStr
	}
}

//����hismap
void MyShopping::iterHistoryMap()
{
	QMap<QString,GuestOrder>::const_iterator go_iter;
	int i=0;
   
    for( go_iter=m_historymap.constBegin(); go_iter!=m_historymap.constEnd(); go_iter++,i++)
	{
		historyModel->setData(historyModel->index(i,0),go_iter.value().orderNum);		
		historyModel->setData(historyModel->index(i,1),go_iter.value().buyer);
		historyModel->setData(historyModel->index(i,2),go_iter.value().buyDate);
		historyModel->setData(historyModel->index(i,3),go_iter.value().bizDispatch);
		historyModel->setData(historyModel->index(i,4),go_iter.value().bizPay);

		
		historyModel->setData(historyModel->index(i,5),go_iter.value().distance);
		historyModel->setData(historyModel->index(i,6),go_iter.value().ps_address);

		historyModel->setData(historyModel->index(i,7),go_iter.value().ps_datetime);

		//historyModel->item(i,7)->setForeground(QBrush(QColor(0, 255, 0)));  

		historyModel->setData(historyModel->index(i,8),go_iter.value().SIPnum);
		historyModel->setData(historyModel->index(i,9),go_iter.value().ps_name);
		historyModel->setData(historyModel->index(i,10),go_iter.value().from);
		
		historyModel->setData(historyModel->index(i,11),go_iter.value().orderStatus);

		//historyModel->item(i,11)->setForeground(QBrush(QColor(255, 0, 0)));  
		historyModel->setData(historyModel->index(i,12),go_iter.value().orderMode);

		for(int j=0;j<historyModel->columnCount();j++)
		{
			//if(1==i)
			//	historyModel->item(i,j)->setBackground(QBrush(QColor(23, 123, 0)));  
		}

	}
}


//����map
void MyShopping::iterGuestMap()
{
	QMap<QString,GuestOrder>::const_iterator go_iter;
	int i=0;
   
    for( go_iter=m_guestmap.constBegin(); go_iter!=m_guestmap.constEnd(); go_iter++,i++)
	{
		initModel->setData(initModel->index(i,0),go_iter.value().orderNum);		
		//initModel->setData(initModel->index(i,1),go_iter.value().buyer);
		initModel->setData(initModel->index(i,1),go_iter.value().buyDate);
		initModel->setData(initModel->index(i,2),go_iter.value().bizDispatch);
		initModel->setData(initModel->index(i,3),go_iter.value().bizPay);
		//initModel->setData(initModel->index(i,5),go_iter.value().distance);
		initModel->setData(initModel->index(i,4),go_iter.value().ps_address);
		//initModel->setData(initModel->index(i,7),go_iter.value().ps_datetime);
		initModel->setData(initModel->index(i,5),go_iter.value().phone);
		initModel->setData(initModel->index(i,6),go_iter.value().ps_name);
		initModel->setData(initModel->index(i,7),go_iter.value().from);
		
		QString tmpStr(go_iter.value().orderStatus);

		if(tmpStr.isEmpty())
			initModel->setData(initModel->index(i,8),"������");
		else
			initModel->setData(initModel->index(i,8),go_iter.value().orderStatus);

		//initModel->item(i,11)->setForeground(QBrush(QColor(255, 0, 0)));  
		initModel->setData(initModel->index(i,9),go_iter.value().orderMode);

		for(int j=0;j<initModel->columnCount();j++)
		{
			//if(1==i)
			//	initModel->item(i,j)->setBackground(QBrush(QColor(23, 123, 0)));  
		}

		//�޸�ǰ��ɫ�����������ſ���
		if(tr("������")==tr(go_iter.value().orderStatus))
			for(int j=0;j<initModel->columnCount();j++)
				initModel->item(i,j)->setForeground(QBrush(QColor(255, 0, 0))); 
	}
}


void MyShopping::updateColor()
{
    //������һ��
	
	 //������һ��//byliu
	
	//QMessageBox::warning(this,"xxxxxx",index);
	bool isfind=false;
	QString myData;
	int i;
	for( i=0;i<m_guestmap.count();i++)
	{
		myData=ui.tableView->model()->index(i,8).data().toString();
		if(sipTel==myData)
		{
			//QMessageBox::warning(this,"-----",myData);
			isfind=true;
			break;
		}

	}
	if(!isfind)
		return ;

	//QMessageBox::warning(this,"xxxxx",QString("%1").arg(i+1));


	static int flag=0;
	flag++;
	if(flag%2==0)
	{
		for(int j=0;j<initModel->columnCount();j++)
			initModel->item(i,j)->setBackground(QBrush(QColor(124, 123, 125)));  
	}else{
		for(int j=0;j<initModel->columnCount();j++)
			initModel->item(i,j)->setBackground(QBrush(QColor(255, 255, 255))); 
	}
}

void MyShopping::updateGuest()
{
	//ɾ�����е���Ȼ���ؽ�
	int rowNum=initModel->rowCount();
	initModel->removeRows(0,rowNum);
	initModel->insertRows(0,rowNum);
   
	//��սṹ�����´����µ�����
	m_guestmap.clear();
	loadDicXML();

	//QMessageBox::warning(this,"+++++++","OK");
	initModel->setHeaderData(0, Qt::Horizontal, tr("�������"));
    ui.tableView->setColumnHidden(0,true);

	//initModel->setHeaderData(1, Qt::Horizontal, tr("�ͻ���ַ"));
    initModel->setHeaderData(1, Qt::Horizontal, tr("��������"));
    initModel->setHeaderData(2, Qt::Horizontal, tr("�ͻ���ʽ"));
    initModel->setHeaderData(3, Qt::Horizontal, tr("���ʽ"));
    //initModel->setHeaderData(5, Qt::Horizontal, tr("���;���"));
	initModel->setHeaderData(4, Qt::Horizontal, tr("���͵�ַ"));
	ui.tableView->setColumnHidden(4,true);
	//initModel->setHeaderData(7, Qt::Horizontal, tr("����Ա�ϱ�ʱ��"));
	initModel->setHeaderData(5, Qt::Horizontal, tr("�ͻ��绰"));
    initModel->setHeaderData(6, Qt::Horizontal, tr("����Ա����"));
	ui.tableView->setColumnHidden(6,true);
    initModel->setHeaderData(7, Qt::Horizontal, tr("������Դ"));
    initModel->setHeaderData(8, Qt::Horizontal, tr("����״̬"));
	initModel->setHeaderData(9, Qt::Horizontal, tr("����ģʽ"));
	
	//initModel->horizontalHeaderItem(0)->setForeground(QBrush (QColor(233, 0, 233))); //����������ɫ
    //initModel->horizontalHeaderItem(0)->setBackground(QBrush (QColor(255, 255, 0)));

	//���ض���xml
	QString ipStr(usrAuth.serverip);
	QString tmpStr(tr("/OAapp/WebObjects/OAapp.woa?cjApply=bizorder"));
	manager->get(QNetworkRequest(QUrl(ipStr+tmpStr)));
	qDebug()<<"��Ʒ�����б�"<<ipStr+tmpStr;

	//����xml
	loadXML();

	//����map��ʾ��������
	iterGuestMap();
	
	//�����к͹������г�ͻ��Ϊʲô
	//qDebug()<<"11111"<<horposion;
	int tmpPos=horposion;

	ui.tableView->selectRow(currentRow);

	//����ԭ���Ĺ���λ��
	ui.tableView->verticalScrollBar()->setSliderPosition(verposion);//�������//verposion
	ui.tableView->horizontalScrollBar()->setSliderPosition(tmpPos);//�������//horposion


#if 0
     QStandardItem *checkBox = new QStandardItem();
	 checkBox->setCheckState(Qt::Unchecked);
     initModel->setItem(0, 0, checkBox);
#endif

	 //������һ��//byliu
	//QString index=ui.tableView->model()->index(0,0).data().toString();
	//QMessageBox::warning(this,"xxxxxx",index);
}


void MyShopping::updateHistory()
{
	//ɾ�����е���Ȼ���ؽ�
	int rowNum=initModel->rowCount();
	historyModel->removeRows(0,rowNum);
	historyModel->insertRows(0,rowNum);
   
	//��սṹ�����´����µ�����
	m_historymap.clear();

	//QMessageBox::warning(this,"+++++++","OK");
	historyModel->setHeaderData(0, Qt::Horizontal, tr("�������"));
	ui.tableView->setColumnHidden(0,true);
	//historyModel->setHeaderData(1, Qt::Horizontal, tr("�ͻ���ַ"));
    historyModel->setHeaderData(1, Qt::Horizontal, tr("��������"));
    historyModel->setHeaderData(2, Qt::Horizontal, tr("�ͻ���ʽ"));
    historyModel->setHeaderData(3, Qt::Horizontal, tr("���ʽ"));
    //historyModel->setHeaderData(5, Qt::Horizontal, tr("���;���"));
	historyModel->setHeaderData(4, Qt::Horizontal, tr("���͵�ַ"));

	//historyModel->setHeaderData(7, Qt::Horizontal, tr("����Ա�ϱ�ʱ��"));
	historyModel->setHeaderData(5, Qt::Horizontal, tr("�ͻ��绰"));
    historyModel->setHeaderData(6, Qt::Horizontal, tr("����Ա����"));
	ui.tableView->setColumnHidden(6,true);
    historyModel->setHeaderData(7, Qt::Horizontal, tr("������Դ"));
    historyModel->setHeaderData(8, Qt::Horizontal, tr("����״̬"));
	historyModel->setHeaderData(9, Qt::Horizontal, tr("����ģʽ"));
}



// �ı䵱ǰ��Ŀ
void MyShopping::changeCurrent(const QModelIndex &current,const QModelIndex &previous)
{
    currentRow=previous.row();

	QString index=ui.tableView->model()->index(current.row(),0).data().toString();
	if(!index.isEmpty())
	{
		QMap<QString,GuestOrder>::const_iterator go_iter;
		go_iter=m_guestmap.find(index);
		
		QStringList  goodsList;
		for(int i=0;i<go_iter.value().totalPD;i++)
		{
			char strlink[512];
			strcpy(strlink,"��Ʒ��ţ�");
			strcat(strlink,go_iter.value().guestGoods[i].homeBuyId);
			strcat(strlink,"       �������ƣ�");
			strcat(strlink,go_iter.value().guestGoods[i].product);
			
			strcat(strlink,"       �� ����");
			strcat(strlink,go_iter.value().guestGoods[i].quantity);
			strcat(strlink,"       �� �ۣ�");
			strcat(strlink,go_iter.value().guestGoods[i].unit);
			goodsList<<strlink;
		}
			
		listModel->setStringList(goodsList);
		ui.listView->setModel(listModel);

		ui.lineEdit_7->setText(go_iter.value().buyDate);
		//ui.lineEdit_2->setText(go_iter.value().buyer);
		ui.lineEdit->setText(go_iter.value().bizDispatch);
		ui.lineEdit_3->setText(go_iter.value().bizPay);

		//���ﱾ������ʵ�绰���룬��ʾ��Ҫ����Ϊsip����
		ui.lineEdit_8->setText(go_iter.value().SIPnum);

		//��ʱ������������Ա����ʱ���õ�
		tmpOrderId=go_iter.value().orderId;
	}
}

void MyShopping::on_sendBtn_clicked()
{	
	//DistributeDlg *mydlg=new DistributeDlg();
	//mydlg->show();
	QString strTmp=ui.comboBox->itemText(ui.comboBox->currentIndex());
	strcpy(os.orderID,tmpOrderId.toAscii());
	//QString strTmp(os.orderID);
	//QMessageBox::warning(this,"============",strTmp);
	
	char urlTmp[512];
	QString tmpstr2(os.orderID);
   // QMessageBox::warning(this,"-----------",orderIdStr);
	QString tmpURL;

	QString ipStr(usrAuth.serverip);
	QString tmpStr(tr("/OAapp/WebObjects/OAapp.woa?cjApply=orderSta&orderId="));
	
	//QString tmpstr1(tr(ipStr+tmpStr));

	QString tmpstr3(tr("&sta="));
	QString tmpstr4;

	QMap<QString,GoodsDics>::const_iterator go_iter;
	//char tmpStr[32];
	go_iter=map_PsstatesMsg.find(strTmp);
	//strcpy(tmpStr,go_iter.value().statecode);
	tmpstr4=tr(go_iter.value().statecode);
	googstate->get(QNetworkRequest(QUrl(ipStr+tmpStr+tmpstr2+tmpstr3+tmpstr4)));
	qDebug()<<ipStr+tmpStr+tmpStr+tmpstr2+tmpstr3+tmpstr4;

}


//�޸�״̬����ֵ
void MyShopping::replyGoodState(QNetworkReply *reply)
{
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString all = codec->toUnicode(reply->readAll());

	QString codeStr,msgStr;
	QXmlStreamReader resxml(all);
	while(!resxml.atEnd())  
    {  
        resxml.readNext();  
        if(resxml.isStartElement())  
        {  
            if(resxml.name() == "Code")  
            {
				codeStr=resxml.readElementText();
            }
			if(resxml.name() =="msg")
			{
				msgStr=resxml.readElementText();
			}

        }  
    }
	QMessageBox::warning(this,"��ʾ��",msgStr);
}

void MyShopping::on_pushButton_clicked()
{
	strcpy(os.orderID,tmpOrderId.toAscii());
	DistributeDlg *dlg=new DistributeDlg();

	//�ѱ����ָ�봫��������
	dlg->setShop(this);
	dlg->show();
	//dlg->showMaximized();
}

void MyShopping::dealCall(const QString &str)
{
}

void MyShopping::on_sipcallBtn_clicked()
{
	 QMap<QString,GuestOrder>::const_iterator go_iter;
	 if(callBtnFlag==CALLING_STATE)
	 {
			callTimer->stop();
			linphone_core_accept_call(the_core,NULL);

	 }
	 if(callBtnFlag==INIT_STATE)
	 {
		 //QMessageBox::warning(this,"xxxxx","��ʼ����");

		char m_address[64];

		QString tmpStr=ui.lineEdit_8->text();
		//go_iter=m_guestmap.find(tmpStr);
 
		sprintf(m_address,"sip:%s@%s",tmpStr.toStdString().c_str(),usrAuth.sipip);
		QMessageBox::warning(this,tr("��ܰ��ʾ��"),tr("�����ں���")+tr(m_address)+tr(",��ȷ�ϣ�"));

		LinphoneProxyConfig* proxyCfg=NULL;	
		//get default proxy
		linphone_core_get_default_proxy(the_core,&proxyCfg);

		if(proxyCfg!=NULL)
		{
			linphone_core_invite(the_core,m_address) ;
		}
		 
	 }
}
void MyShopping::on_siphungupBtn_clicked()
{
	 linphone_core_terminate_call(the_core,NULL);
	 callBtnFlag=INIT_STATE;
	 ui.sipcallBtn->setText("����");
}


void MyShopping::setSendCombox()
{
	ui.comboBox->setCurrentIndex(tmpSenderNum);

	QString strTmp=ui.comboBox->itemText(ui.comboBox->currentIndex());
	strcpy(os.orderID,tmpOrderId.toAscii());
	//QString strTmp(os.orderID);
	//QMessageBox::warning(this,"============",strTmp);
	
	char urlTmp[512];
	QString tmpstr2(os.orderID);
   // QMessageBox::warning(this,"-----------",orderIdStr);
	QString tmpURL;
	
	QString ipStr(usrAuth.serverip);
	QString tmpstr1(tr("/OAapp/WebObjects/OAapp.woa?cjApply=orderSta&orderId="));

	QString tmpstr3(tr("&sta="));
	QString tmpstr4;

	QMap<QString,GoodsDics>::const_iterator go_iter;
	//char tmpStr[32];
	go_iter=map_PsstatesMsg.find(strTmp);
	//strcpy(tmpStr,go_iter.value().statecode);
	tmpstr4=tr(go_iter.value().statecode);
	googstate->get(QNetworkRequest(QUrl(ipStr+tmpstr1+tmpstr2+tmpstr3+tmpstr4)));
	qDebug()<<"�޸�״̬"<<ipStr+tmpstr1+tmpstr2+tmpstr3+tmpstr4;
}

//��ȡ��ֱ����λ��
void MyShopping::verMove(int value)
{
	verposion=value;
}

//��ȡˮƽ����λ��
void MyShopping::horMove(int value)
{
	horposion=value;
}


void MyShopping::on_productQueryBtn_clicked()
{
	QString ipStr(usrAuth.serverip);
	//��ȡ��ʼ����ʱ��
	QDate startDate=ui.startDateEdit->date();
	QDate endDate=ui.endDateEdit->date();
	qDebug()<<startDate.toString("yyyy-MM-dd")<<"----"<<endDate.toString("yyyy-MM-dd");
	
	//��ȡ��Ʒģʽ
	QString modelStr=ui.modelComboBox->currentText();
	qDebug()<<"��Ʒģʽ"<<modelStr;

	QString conditionStr=ui.queryComboBox->currentText();
	if(conditionStr.compare("��ѯȫ��")==0)
	{
		QString tmpStr1(tr("/OAapp/WebObjects/OAapp.woa?cjApply=bizorder&sta=03"));
		managerHistory->get(QNetworkRequest(QUrl(ipStr+tmpStr1)));
		qDebug()<<"ȫ����ʷ���������б�"<<ipStr+tmpStr1;
	}else{
		//http://192.168.0.174:88/OAapp/WebObjects/OAapp.woa?cjApply=bizorder&sta=03&start=2014-01-09&end=2015-01-10
		//���ض���xml


		QString tmpStr;
		if(modelStr.compare("ԤԼ")==0)
			tmpStr="00";
		else if(modelStr.compare("��ͨ")==0)
			tmpStr="01";
		else if(modelStr.compare("�Ź�")==0)
			tmpStr="02";
		else if(modelStr.compare("�˻�")==0)
			tmpStr="03";
		else if(modelStr.compare("����")==0)
			tmpStr="04";
		else
			QMessageBox::warning(this,"��ܰ��ʾ��","δ����ģʽ������ϵϵͳ������Ա");

		QString tmpStr1(tr("/OAapp/WebObjects/OAapp.woa?cjApply=bizorder&sta=")+tmpStr+tr("&start=")+startDate.toString("yyyy-MM-dd")+tr("&end=")+endDate.toString("yyyy-MM-dd"));
		managerHistory->get(QNetworkRequest(QUrl(ipStr+tmpStr1)));
		qDebug()<<"������ʷ���������б�"<<ipStr+tmpStr1;
	}

}

void MyShopping::conditionProcess(int index)
{
	currentQueryCombo=index;
	if(0==currentQueryCombo)
	{
		ui.startDateEdit->setDisabled(true);
		ui.endDateEdit->setDisabled(true);
		ui.modelComboBox->setDisabled(true);
	}else{
		ui.startDateEdit->setDisabled(false);
		ui.endDateEdit->setDisabled(false);
		ui.modelComboBox->setDisabled(false);
	}
}