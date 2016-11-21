#include "shopsubmit.h"
#include <QMessageBox>
#include <QXmlStreamReader>

#include "gsoap\soapServicesSoapBindingProxy.h"
#include "gsoap\ServicesSoapBinding.nsmap"
#include "gsoap\shopinterface.h"
#include <QDesktopWidget>
#include <QUuid>

#include <QPixmap>

extern SipInfo  usrAuth; //用户信息


shopsubmit::shopsubmit(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
 
	fileDlg = new QFileDialog(this);//创建对话框

	classConn = new QNetworkAccessManager(this);
	classDelConn = new QNetworkAccessManager(this);
	productConn = new QNetworkAccessManager(this);//下载已经审核的产品
	shProductConn = new QNetworkAccessManager(this);//下载待已经审核的产品
	requireShConn = new QNetworkAccessManager(this);//请求产品审核

	uploadConn = new QNetworkAccessManager(this);//
	productMode = new QNetworkAccessManager(this);
	productDelConn = new QNetworkAccessManager(this);
	_uploadManager = new QNetworkAccessManager(this);
    

	//团购定时器
	tuanGouTimer=new QTimer();
    connect(tuanGouTimer, SIGNAL(timeout()), this, SLOT(setJiShi())); // ***就是你所说的响应函数

    connect(classConn, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyClassConn(QNetworkReply* )));
	connect(classDelConn, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyClassDelConn(QNetworkReply* )));

    connect(productConn, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyProductConn(QNetworkReply* )));
	connect(shProductConn, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyshProductConn(QNetworkReply* )));
	connect(requireShConn, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyRequireShConn(QNetworkReply* )));
	connect(productDelConn, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyProductDelConn(QNetworkReply* )));

	connect(uploadConn, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyUploadConn(QNetworkReply* )));
	connect(productMode, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyProductMode(QNetworkReply* )));

	QString ipStr(usrAuth.serverip);
	QString path("/OAapp/WebObjects/OAapp.woa?cjApply=classify");
    classConn->get(QNetworkRequest(QUrl(ipStr+path)));
	qDebug()<<"产品分类列表："<<ipStr+path;

	connect(ui.tableWidgetClass,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(getClassItem(QTableWidgetItem*)));
	connect(ui.tableWidgetProduct,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(getProductItem(QTableWidgetItem*)));
	connect(ui.tablesHProduct,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(getShProductItem(QTableWidgetItem*)));

	//connect(ui.manageBtn,SIGNAL(clicked()),this,SLOT(on_manageBtn_clicked()));
	//connect(ui.xgProductBtn,SIGNAL(clicked()),this,SLOT(on_productBtn_clicked()));

	//待审核产品下拉combox的值改变了
	connect(ui.sh_product_box, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(shBoxValueChanged(const QString &))); 

	connect(_uploadManager,SIGNAL(finished(QNetworkReply*)),SLOT(uploadFinished(QNetworkReply*)));

	//初始分类默认值
	ui.goodScaleLine->setText(tr("0"));
	ui.goodOrderLine->setText(tr("0"));

	//初始化productcombox
	QStringList liststr;
	liststr<<"预约"<<"实时"<<"团购";
	ui.comboproduct->addItems(liststr);
	ui.tableWidgetProduct->setSortingEnabled(false); 
	this->showMaximized();
	setAttribute(Qt::WA_DeleteOnClose);
}

shopsubmit::~shopsubmit()
{

	tuanGouTimer->stop();
	delete tuanGouTimer;
}

void shopsubmit::on_manageBtn_clicked()
{
    ui.stackedWidget->setCurrentIndex(0);
	QString ipStr(usrAuth.serverip);
	QString path("/OAapp/WebObjects/OAapp.woa?cjApply=classify");
    classConn->get(QNetworkRequest(QUrl(ipStr+path)));

	//插入前先清空
	ui.tableWidgetProduct->clear();
	ui.tableWidgetProduct->setColumnCount(12);
	ui.tableWidgetProduct->setRowCount(250);
	ui.tableWidgetProduct->setColumnHidden(0,true);
	ui.tableWidgetProduct->horizontalHeader()->setResizeMode(QHeaderView::Stretch) ;//宽度自动适应

	QStringList headers; 
	headers <<"产品ID"<< "商品ID" <<"分类ID"<<"本地名称"<<"产品属性"<<"产品产地"<<"商品单位"<<"正常售价"<<"志愿者200小时售价"<<"志愿者500小时售价"<<"帮困售价"<<"商品毛重";
    
	ui.tableWidgetProduct->setHorizontalHeaderLabels(headers); 
	ui.tableWidgetProduct->setColumnHidden(0,true);
	ui.tableWidgetProduct->setColumnHidden(1,true);

	ui.tableWidgetProduct->setEditTriggers ( QAbstractItemView::NoEditTriggers );
	ui.tableWidgetProduct->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void shopsubmit::on_xgProductBtn_clicked()
{
	  //获取该类商品当前产品数，由此产生ID号，也可以是数量
	  //int productCount=map_productinfo.count();
	  int productCount=vector_productinfo.size();
	  int rowProduct=ui.tableWidgetProduct->currentRow();
	  int rowClass=ui.tableWidgetClass->currentRow();

      //待审核产品分类
	  int shrowClass=ui.sh_product_box->currentIndex();
	  //待审核产品
	  //QTableWidgetItem* item1=ui.tablesHProduct->item(currentShIndex,0);
	  //shProductIndex=item1->text();

	  if((rowClass==-1)&&(shrowClass==-1))
	  {
	  	  QMessageBox::warning(this,tr("温馨提示"),tr("您未选择分类，请先选择分类！"));
	  	  return ;  
	  }

	  QStringList liststr;
	  liststr << "暂无";
	  ui.comboBox->addItems(liststr);
      ui.stackedWidget->setCurrentIndex(2);

	  if((rowProduct!=-1)&&(vector_productinfo.size()!=0)) 
	  {
      
	   //这里好像有bug
        QString productId=ui.tableWidgetProduct->item(rowProduct,0)->text();
		qDebug()<<"============================="<<productId;

        int i;
		for( i=0;i<vector_productinfo.size();i++)
		{
			if(strcmp(productId.toStdString().c_str(),vector_productinfo.at(i).productId)==0)//xxxxx
				break;
		}

		ui.lineEdit_8->setText(vector_productinfo.at(i).Itemid);//ttttttttttt 
		ui.lineEdit_10->setText(vector_productinfo.at(i).Itemcode);
		ui.lineEdit_11->setText(vector_productinfo.at(i).Locname);
		ui.lineEdit_12->setText(vector_productinfo.at(i).Shortname);
		ui.lineEdit_13->setText(vector_productinfo.at(i).Djyzt);

		ui.lineEdit_14->setText(vector_productinfo.at(i).Origin);//

		ui.lineEdit_15->setText(vector_productinfo.at(i).Itemmodel);

		if(ui.lineEdit_15->text().isEmpty())
			ui.lineEdit_15->setText("暂无");


		ui.lineEdit_16->setText(vector_productinfo.at(i).Produce2);
		if(ui.lineEdit_16->text().isEmpty())
			ui.lineEdit_16->setText("暂无");

		ui.lineEdit_17->setText(vector_productinfo.at(i).Unit);
		ui.lineEdit_18->setText(vector_productinfo.at(i).Classid);
		ui.lineEdit_19->setText(vector_productinfo.at(i).PiclinkL);
		ui.lineEdit_20->setText(vector_productinfo.at(i).PiclinkS);
		ui.lineEdit_21->setText(vector_productinfo.at(i).Price1);
		ui.lineEdit_22->setText(vector_productinfo.at(i).Price2);
		ui.lineEdit_23->setText(vector_productinfo.at(i).Price3);
		ui.lineEdit_24->setText(vector_productinfo.at(i).Price4);
		ui.lineEdit_25->setText(vector_productinfo.at(i).weightvalue);
		ui.textEdit->setText(vector_productinfo.at(i).Dspjs);
		ui.textEdit_3->setText(vector_productinfo.at(i).Source);
	  }else if(!shProductIndex.isEmpty()){
        int j;
		//这里好像有bug
	    QString shproductId=ui.tablesHProduct->item(currentShIndex,0)->text();//byliu
		qDebug()<<"-------------------------------"<<shproductId;
		for( j=0;j<vector_shproductinfo.size();j++)
		{
			if(strcmp(shproductId.toStdString().c_str(),vector_shproductinfo.at(j).productId)==0)//xxxxx
				break;
		}


		  ui.lineEdit_8->setText(vector_shproductinfo.at(j).Itemid);//ttttttttttt 
		  ui.lineEdit_10->setText(vector_shproductinfo.at(j).Itemcode);
		  ui.lineEdit_11->setText(vector_shproductinfo.at(j).Locname);
		  ui.lineEdit_12->setText(vector_shproductinfo.at(j).Shortname);
		  ui.lineEdit_13->setText(vector_shproductinfo.at(j).Djyzt);
		  ui.lineEdit_14->setText(vector_shproductinfo.at(j).Origin);//
		  ui.lineEdit_15->setText(vector_shproductinfo.at(j).Itemmodel);
		  if(ui.lineEdit_15->text().isEmpty())
		   	ui.lineEdit_15->setText("暂无");
		  ui.lineEdit_16->setText(vector_shproductinfo.at(j).Produce2);
		  if(ui.lineEdit_16->text().isEmpty())
			ui.lineEdit_16->setText("暂无");
		  ui.lineEdit_17->setText(vector_shproductinfo.at(j).Unit);
		  ui.lineEdit_18->setText(vector_shproductinfo.at(j).Classid);
		  ui.lineEdit_19->setText(vector_shproductinfo.at(j).PiclinkL);
		  ui.lineEdit_20->setText(vector_shproductinfo.at(j).PiclinkS);
		  ui.lineEdit_21->setText(vector_shproductinfo.at(j).Price1);
		  ui.lineEdit_22->setText(vector_shproductinfo.at(j).Price2);
		  ui.lineEdit_23->setText(vector_shproductinfo.at(j).Price3);
		  ui.lineEdit_24->setText(vector_shproductinfo.at(j).Price4);
		  ui.lineEdit_25->setText(vector_shproductinfo.at(j).weightvalue);
		  ui.textEdit->setText(vector_shproductinfo.at(j).Dspjs);
		  ui.textEdit_3->setText(vector_shproductinfo.at(j).Source);
	  
	  }else{
		  ui.lineEdit_8->clear();

		ui.lineEdit_10->clear();

		ui.lineEdit_11->clear();
		ui.lineEdit_12->clear();
		ui.lineEdit_13->clear();
		ui.lineEdit_14->clear();
		ui.lineEdit_15->clear();
		ui.lineEdit_15->setText("暂无");
		ui.lineEdit_16->clear();
		ui.lineEdit_16->setText("暂无");
		ui.lineEdit_17->clear();
		//ui.lineEdit_18->setText(go_iter.value().Classid);
		ui.lineEdit_19->clear();
		ui.lineEdit_20->clear();
		ui.lineEdit_21->clear();
		ui.lineEdit_22->clear();
		ui.lineEdit_23->clear();
		ui.lineEdit_24->clear();
		ui.lineEdit_25->clear();
		ui.textEdit->clear();
		ui.textEdit_3->clear();
	  }
}

void shopsubmit::on_requireShBtn_clicked()
{
	QString ipStr(usrAuth.serverip);
	//QMessageBox::warning(this,"程序调试",shProductIndex);
	QString path("/OAapp/WebObjects/OAapp.woa?cjApply=productAudit&id=");
	QString str1(tr("&pwd="));
	QString str2(tr("1234"));
	qDebug()<<ipStr+path+shProductIndex+str1+str2;
    requireShConn->get(QNetworkRequest(QUrl(ipStr+path+shProductIndex+str1+str2)));
}

void shopsubmit::on_xgClassBtn_clicked()//on_addClassBtn_clicked
{
	 int row=ui.tableWidgetClass->currentRow();
	 if((row!=-1)&&(row<ui.tableWidgetClass->rowCount()))
	 {
		  ui.goodIDLine->setText(ui.tableWidgetClass->item(row,0)->text());
		  ui.goodNameLine->setText(ui.tableWidgetClass->item(row,1)->text());
	  }
	   ui.stackedWidget->setCurrentIndex(3);
}


//产品分类配送按钮
void shopsubmit::on_classOkBtn_clicked()
{
	char serverURL[128];
	char *serverpath="/OAapp/WebObjects/OAapp.woa/ws/Services";
	strcpy(serverURL,usrAuth.serverip);
	strcat(serverURL,serverpath);
	
	ServicesSoapBinding soap(serverURL);
	soap_set_mode(soap.soap, SOAP_C_UTFSTRING);
	GoogsClass gc;
	WebServiceRes servieRest;
	char *rep=NULL;
	char xmlbuf[512];
	if(ui.goodIDLine->text().isEmpty()||ui.goodNameLine->text().isEmpty()||ui.goodScaleLine->text().isEmpty()||ui.goodOrderLine->text().isEmpty())
	{
			QMessageBox::warning(this,tr("温馨提示！"),tr("输入有空值，请重新输入！"));
			return ;
	}
	
	strcpy(gc.goodsClassID,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.goodIDLine->text().toStdWString().c_str()));
	strcpy(gc.goodsClassName,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.goodNameLine->text().toStdWString().c_str()));
	
    if(isDigitStr(ui.goodScaleLine->text())==-1)
	{
		QMessageBox::warning(this,tr("温馨提示！"),tr("返利比率输入有误，请重新输入！"));
		return;
	}else
		strcpy(gc.goodsClassOrder,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.goodOrderLine->text().toStdWString().c_str()));


    if(isDigitStr(ui.goodOrderLine->text())==-1)
	{
		QMessageBox::warning(this,tr("温馨提示！"),tr("显示顺序输入有误，请重新输入！"));
		return ;
	}
	else
		strcpy(gc.goodsClassScal,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.goodScaleLine->text().toStdWString().c_str()));
	
	sprintf(xmlbuf,"<Classes><Class><Id>%s</Id><Name>%s</Name><RebateRate>%s</RebateRate><Sequence>%s</Sequence></Class></Classes>",gc.goodsClassID,gc.goodsClassName,gc.goodsClassOrder,gc.goodsClassScal);

	soap.ns1__updateClasses("abc",xmlbuf,rep);
	QXmlStreamReader resxml(rep);
	while(!resxml.atEnd())  
    {  
        resxml.readNext();  
        if(resxml.isStartElement())  
        {  
            if(resxml.name() == "Code")  
            {
				strcpy(servieRest.Code,resxml.readElementText().toAscii());
            }
			if(resxml.name() =="ErrDesc")
			{
				strcpy(servieRest.ErrDesc,resxml.readElementText().toAscii());
			}

        }  
    }  
	QMessageBox::warning(this,tr("数据提交提示"),servieRest.ErrDesc);
}


void shopsubmit::on_goodsInfoOkBtn_clicked()
{
	char serverURL[128];
	char *serverpath="/OAapp/WebObjects/OAapp.woa/ws/Services";
	strcpy(serverURL,usrAuth.serverip);
	strcat(serverURL,serverpath);
	
	ServicesSoapBinding soap(serverURL);

	soap_set_mode(soap.soap, SOAP_C_UTFSTRING);
	WebServiceRes servieRest;
	ProductInfo gi;
	char *rep=NULL;
	char xmlbuf[2048];

	if(ui.lineEdit_8->text().isEmpty()||ui.lineEdit_10->text().isEmpty()||ui.lineEdit_11->text().isEmpty()||ui.lineEdit_12->text().isEmpty()||ui.lineEdit_13->text().isEmpty()||ui.lineEdit_14->text().isEmpty()||ui.lineEdit_15->text().isEmpty()||ui.lineEdit_16->text().isEmpty()||ui.lineEdit_17->text().isEmpty()||ui.lineEdit_18->text().isEmpty()||ui.lineEdit_19->text().isEmpty()||ui.lineEdit_20->text().isEmpty()||ui.lineEdit_21->text().isEmpty()||ui.lineEdit_22->text().isEmpty()||ui.lineEdit_23->text().isEmpty()||ui.lineEdit_24->text().isEmpty()||ui.lineEdit_25->text().isEmpty())
	{
		QMessageBox::warning(this,tr("警告！"),tr("输入不能有空值！"));
		return ;
	}

	//strcpy(gi.Itemid,ui.textEdit->toPlainText().toAscii());
	//strcpy(gc.goodsClassName,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.goodNameLine->text().toStdWString().c_str()));
	strcpy(gi.Itemid,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.lineEdit_8->text().toStdWString().c_str()));
	strcpy(gi.Deptcode,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.comboBox->currentText().toStdWString().c_str()));
	strcpy(gi.Itemcode,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.lineEdit_10->text().toStdWString().c_str()));
	strcpy(gi.Locname,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.lineEdit_11->text().toStdWString().c_str()));
	strcpy(gi.Shortname,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.lineEdit_12->text().toStdWString().c_str()));
	//strcpy(gi.Shortname,ui.lineEdit_35->text().toAscii());
	strcpy(gi.Djyzt,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.lineEdit_13->text().toStdWString().c_str()));
	strcpy(gi.Dspjs,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.textEdit->toPlainText().toStdWString().c_str()));
	strcpy(gi.Origin,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.lineEdit_14->text().toStdWString().c_str()));

	strcpy(gi.Itemmodel,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.lineEdit_15->text().toStdWString().c_str()));
	strcpy(gi.Produce2,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.lineEdit_16->text().toStdWString().c_str()));
	strcpy(gi.Unit,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.lineEdit_17->text().toStdWString().c_str()));
	strcpy(gi.Classid,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.lineEdit_18->text().toStdWString().c_str()));

	strcpy(gi.PiclinkL,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.lineEdit_19->text().toStdWString().c_str()));
	strcpy(gi.PiclinkS,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.lineEdit_20->text().toStdWString().c_str()));


	strcpy(gi.Source,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.textEdit_3->toPlainText().toStdWString().c_str()));

	if(isDigitStr(ui.lineEdit_21->text())==-1)
	{
		QMessageBox::warning(this,tr("警告！"),tr("正常售价输入有误，请重新输入！"));
		return ;
	}else
		strcpy(gi.Price1,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.lineEdit_21->text().toStdWString().c_str()));

	if(isDigitStr(ui.lineEdit_22->text())==-1)
	{
		QMessageBox::warning(this,tr("警告！"),tr("志愿者200小时输入有误，请重新输入！"));
		return ;
	}else
		strcpy(gi.Price2,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.lineEdit_22->text().toStdWString().c_str()));
	
	if(isDigitStr(ui.lineEdit_23->text())==-1)
	{
		QMessageBox::warning(this,tr("警告！"),tr("志愿者500小时输入有误，请重新输入！"));
		return ;
	}else
		strcpy(gi.Price3,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.lineEdit_23->text().toStdWString().c_str()));

	if(isDigitStr(ui.lineEdit_24->text())==-1)
	{
		QMessageBox::warning(this,tr("警告！"),tr("帮困售价输入有误，请重新输入！"));
		return ;
	}else
		strcpy(gi.Price4,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.lineEdit_24->text().toStdWString().c_str()));

	if(isDigitStr(ui.lineEdit_25->text())==-1)
	{
		QMessageBox::warning(this,tr("警告！"),tr("商品毛重输入有误，请重新输入！"));
		return ;
	}else
		strcpy(gi.weightvalue,WideCharToMulityByte(CP_UTF8,(wchar_t*)ui.lineEdit_25->text().toStdWString().c_str()));

	
	sprintf(xmlbuf,"<Items><Item><Itemid>%s</Itemid><Deptcode>%s</Deptcode><Itemcode>%s</Itemcode><Locname>%s</Locname><Shortname>%s</Shortname><Djyzt>%s</Djyzt><Dspjs>%s</Dspjs><Origin>%s</Origin><Itemmodel>%s</Itemmodel><Produce2>%s</Produce2><Unit>%s</Unit><Classid>%s</Classid><PiclinkL>%s</PiclinkL><PiclinkS>%s</PiclinkS><Source>%s</Source><Price1>%s</Price1><Price2>%s</Price2><Price3>%s</Price3><Price4>%s</Price4><weightvalue>%s</weightvalue></Item></Items>"
		,gi.Itemid,gi.Deptcode,gi.Itemcode,gi.Locname,gi.Shortname,gi.Djyzt,gi.Dspjs,gi.Origin,gi.Itemmodel,gi.Produce2,gi.Unit,gi.Classid,gi.PiclinkL,gi.PiclinkS,gi.Source,gi.Price1,gi.Price2,gi.Price3,gi.Price4,gi.weightvalue);


	soap.ns1__updateProducts("1234",xmlbuf,rep);
	//qDebug()<<xmlbuf;

	QXmlStreamReader resxml(rep);
	QString retStr;
	while(!resxml.atEnd())  
    {  
        resxml.readNext();  
        if(resxml.isStartElement())  
        {  
            if(resxml.name() == "Code")  
            {
				 retStr=resxml.readElementText();
				 strcpy(servieRest.Code,resxml.readElementText().toAscii());
            }
			if(resxml.name() =="ErrDesc")
			{
				strcpy(servieRest.ErrDesc,resxml.readElementText().toAscii());
			}
        }  
    }  
	 //QUuid id = QUuid::createUuid();  
     //QString strId = id.toString();
	 //ui.lineEdit_8->setText(strId);
	 QMessageBox::warning(this,tr("数据提交提示！"),servieRest.ErrDesc);
}

int shopsubmit::isDigitStr(QString src)  
{  
    QByteArray ba = src.toLatin1();//QString 转换为 char*  
     const char *s = ba.data();  
  
    while(*s && *s>='0' && *s<='9'||*s=='.') 
		s++;  
  
    if (*s)  
    { //不是纯数字  
        return -1;  
    }  
    else  
    { //纯数字  
        return 0;  
    }  
}

//解析分类xml
void shopsubmit::loadXmlClass()
{
    CMarkup xml;
	string str="class.xml";

    bool xx=xml.Load(str);
    xml.ResetMainPos();   
	map_classinfo.clear();

	while (xml.FindChildElem("spType"))
	{
		GoogsClass m_goodclass;
		xml.IntoElem();	
		xml.FindChildElem("spTypeCode");	
		string spTypeCodeStr= xml.GetChildData();
		strcpy(m_goodclass.goodsClassID,spTypeCodeStr.c_str());

		xml.FindChildElem("spTypeName");	
		string spTypeNameStr= xml.GetChildData();
		strcpy(m_goodclass.goodsClassName,spTypeNameStr.c_str());
		xml.OutOfElem();

		QString strTmp(m_goodclass.goodsClassID);
		map_classinfo.insert(strTmp,m_goodclass);//orderIdStr

	}
}

//产品信息xml
void shopsubmit::loadShXmlProduct()
{
	CMarkup xml;
	string str="shproduct.xml";

    bool xx=xml.Load(str);
    xml.ResetMainPos();   
	vector_shproductinfo.clear();

	while (xml.FindChildElem("product"))
	{
		ProductInfo m_productInfo;

		xml.IntoElem();	
		xml.FindChildElem("productId");	
		string productId= xml.GetChildData();
		strcpy(m_productInfo.productId,productId.c_str());

		xml.FindChildElem("itemid");	
		string itemidStr= xml.GetChildData();
		strcpy(m_productInfo.Itemid,itemidStr.c_str());

		xml.FindChildElem("itemcode");	
		string itemcodeStr= xml.GetChildData();
		strcpy(m_productInfo.Itemcode,itemcodeStr.c_str());

		xml.FindChildElem("deptcode");	
		string deptcode= xml.GetChildData();
		strcpy(m_productInfo.Deptcode,deptcode.c_str());

		xml.FindChildElem("itemmodel");	
		string itemmodel= xml.GetChildData();
		strcpy(m_productInfo.Itemmodel,itemmodel.c_str());

		xml.FindChildElem("product2");	
		string product2= xml.GetChildData();
		strcpy(m_productInfo.Produce2,product2.c_str());

		xml.FindChildElem("classid");	
		string classid= xml.GetChildData();
		strcpy(m_productInfo.Classid,classid.c_str());

		xml.FindChildElem("locname");	
		string locname= xml.GetChildData();
		strcpy(m_productInfo.Locname,locname.c_str());

		xml.FindChildElem("shortname");	
		string shortname= xml.GetChildData();
		strcpy(m_productInfo.Shortname,shortname.c_str());

		xml.FindChildElem("djyzt");	
		string djyzt= xml.GetChildData();

		QString qdjyzt = QString::fromStdString(djyzt);
		if(qdjyzt.isEmpty())
			strcpy(m_productInfo.Djyzt,"00");
		else   
		    strcpy(m_productInfo.Djyzt,djyzt.c_str());

		xml.FindChildElem("dspjs");	
		string dspjs= xml.GetChildData();
		strcpy(m_productInfo.Dspjs,dspjs.c_str());

		xml.FindChildElem("origin");	
		string origin= xml.GetChildData();
		strcpy(m_productInfo.Origin,origin.c_str());

		xml.FindChildElem("unit");	
		string unit= xml.GetChildData();
		strcpy(m_productInfo.Unit,unit.c_str());

		xml.FindChildElem("piclinkL");	
		string piclinkL= xml.GetChildData();
		strcpy(m_productInfo.PiclinkL,piclinkL.c_str());

		xml.FindChildElem("piclinkS");	
		string piclinkS= xml.GetChildData();
		strcpy(m_productInfo.PiclinkS,piclinkS.c_str());

		xml.FindChildElem("source");	
		string source= xml.GetChildData();
		strcpy(m_productInfo.Source,source.c_str());

		xml.FindChildElem("price1");	
		string price1= xml.GetChildData();
		strcpy(m_productInfo.Price1,price1.c_str());

		xml.FindChildElem("price2");	
		string price2= xml.GetChildData();
		strcpy(m_productInfo.Price2,price2.c_str());

		xml.FindChildElem("price3");	
		string price3= xml.GetChildData();
		strcpy(m_productInfo.Price3,price3.c_str());

		xml.FindChildElem("price4");	
		string price4= xml.GetChildData();
		strcpy(m_productInfo.Price4,price4.c_str());


		xml.FindChildElem("weightvalue");	
		string weightvalue= xml.GetChildData();
		strcpy(m_productInfo.weightvalue,weightvalue.c_str());

		xml.FindChildElem("quantity");	
		string quantity= xml.GetChildData();
		strcpy(m_productInfo.quantity,quantity.c_str());


		xml.FindChildElem("pwd");	
		string pwd= xml.GetChildData();
		strcpy(m_productInfo.pwd,pwd.c_str());



		xml.FindChildElem("systemdate");	
		string systemdate= xml.GetChildData();
		strcpy(m_productInfo.systemdate,systemdate.c_str());

		xml.FindChildElem("limit");	
		string limit= xml.GetChildData();
		strcpy(m_productInfo.limit,limit.c_str());		

		xml.OutOfElem();
		//QString strTmp(m_productInfo.productId);
		vector_shproductinfo.append(m_productInfo);//orderIdStr

	}
}

//产品信息xml
void shopsubmit::loadXmlProduct()
{
    CMarkup xml;
	string str="product.xml";

    bool xx=xml.Load(str);
    xml.ResetMainPos();   
	vector_productinfo.clear();

	while (xml.FindChildElem("product"))
	{
		ProductInfo m_productInfo;

		xml.IntoElem();	
		xml.FindChildElem("productId");	
		string productId= xml.GetChildData();
		strcpy(m_productInfo.productId,productId.c_str());

		xml.FindChildElem("itemid");	
		string itemidStr= xml.GetChildData();
		strcpy(m_productInfo.Itemid,itemidStr.c_str());

		xml.FindChildElem("itemcode");	
		string itemcodeStr= xml.GetChildData();
		strcpy(m_productInfo.Itemcode,itemcodeStr.c_str());

		xml.FindChildElem("deptcode");	
		string deptcode= xml.GetChildData();
		strcpy(m_productInfo.Deptcode,deptcode.c_str());

		xml.FindChildElem("itemmodel");	
		string itemmodel= xml.GetChildData();
		strcpy(m_productInfo.Itemmodel,itemmodel.c_str());

		xml.FindChildElem("product2");	
		string product2= xml.GetChildData();
		strcpy(m_productInfo.Produce2,product2.c_str());

		xml.FindChildElem("classid");	
		string classid= xml.GetChildData();
		strcpy(m_productInfo.Classid,classid.c_str());

		xml.FindChildElem("locname");	
		string locname= xml.GetChildData();
		strcpy(m_productInfo.Locname,locname.c_str());

		xml.FindChildElem("shortname");	
		string shortname= xml.GetChildData();
		strcpy(m_productInfo.Shortname,shortname.c_str());


		xml.FindChildElem("djyzt");	
		string djyzt= xml.GetChildData();

		QString qdjyzt = QString::fromStdString(djyzt);
		if(qdjyzt.isEmpty())
			strcpy(m_productInfo.Djyzt,"00");
		else   
		    strcpy(m_productInfo.Djyzt,djyzt.c_str());

		xml.FindChildElem("dspjs");	
		string dspjs= xml.GetChildData();
		strcpy(m_productInfo.Dspjs,dspjs.c_str());

		xml.FindChildElem("origin");	
		string origin= xml.GetChildData();
		strcpy(m_productInfo.Origin,origin.c_str());

		xml.FindChildElem("unit");	
		string unit= xml.GetChildData();
		strcpy(m_productInfo.Unit,unit.c_str());

		xml.FindChildElem("piclinkL");	
		string piclinkL= xml.GetChildData();
		strcpy(m_productInfo.PiclinkL,piclinkL.c_str());

		xml.FindChildElem("piclinkS");	
		string piclinkS= xml.GetChildData();
		strcpy(m_productInfo.PiclinkS,piclinkS.c_str());

		xml.FindChildElem("source");	
		string source= xml.GetChildData();
		strcpy(m_productInfo.Source,source.c_str());

		xml.FindChildElem("price1");	
		string price1= xml.GetChildData();
		strcpy(m_productInfo.Price1,price1.c_str());

		xml.FindChildElem("price2");	
		string price2= xml.GetChildData();
		strcpy(m_productInfo.Price2,price2.c_str());

		xml.FindChildElem("price3");	
		string price3= xml.GetChildData();
		strcpy(m_productInfo.Price3,price3.c_str());

		xml.FindChildElem("price4");	
		string price4= xml.GetChildData();
		strcpy(m_productInfo.Price4,price4.c_str());


		xml.FindChildElem("weightvalue");	
		string weightvalue= xml.GetChildData();
		strcpy(m_productInfo.weightvalue,weightvalue.c_str());

		xml.FindChildElem("quantity");	
		string quantity= xml.GetChildData();
		strcpy(m_productInfo.quantity,quantity.c_str());


		xml.FindChildElem("pwd");	
		string pwd= xml.GetChildData();
		strcpy(m_productInfo.pwd,pwd.c_str());



		xml.FindChildElem("systemdate");	
		string systemdate= xml.GetChildData();
		strcpy(m_productInfo.systemdate,systemdate.c_str());

		xml.FindChildElem("limit");	
		string limit= xml.GetChildData();
		strcpy(m_productInfo.limit,limit.c_str());		

		xml.OutOfElem();
		//QString strTmp(m_productInfo.productId);
		vector_productinfo.append(m_productInfo);//orderIdStr

	}
}

void shopsubmit::initClassTable()
{
	QMap<QString,GoogsClass>::const_iterator go_iter;

	ui.tableWidgetClass->clear();

	ui.tableWidgetClass->setColumnCount(4);
	ui.tableWidgetClass->setRowCount(50);

	ui.tableWidgetClass->setEditTriggers ( QAbstractItemView::NoEditTriggers );
	ui.tableWidgetClass->setSelectionBehavior(QAbstractItemView::SelectRows);
	
	QStringList headers; 
	headers << "分类ID" << "分类名称"<<"返利比率"<<"显示顺序";
    ui.tableWidgetClass->setHorizontalHeaderLabels(headers); 
	ui.tableWidgetClass->horizontalHeader()->setResizeMode(QHeaderView::Stretch) ;//宽度自动适应

	int i=0;
	for( go_iter=map_classinfo.constBegin(); go_iter!=map_classinfo.constEnd(); go_iter++,i++)
	{
		ui.tableWidgetClass->setItem(i, 0, new QTableWidgetItem(go_iter.value().goodsClassID)); 
		ui.tableWidgetClass->setItem(i, 1, new QTableWidgetItem(go_iter.value().goodsClassName)); 
		ui.tableWidgetClass->setItem(i, 2, new QTableWidgetItem(tr("0"))); 
		ui.tableWidgetClass->setItem(i, 3, new QTableWidgetItem(tr("1"))); 
	}
}

void shopsubmit::initProductShTable()
{
	//QHash<QString,ProductInfo>::const_iterator go_iter;
	int i=0;
	//插入前先清空
	ui.tablesHProduct->clear();
	
	//list也清空
	ui.shDetailInfoList->clear();
    //图片也清空
	ui.shImageLabel->clear();

	ui.shImageLabel->setPixmap(QPixmap("./xx.jpg"));
	ui.tablesHProduct->setColumnCount(12);
	ui.tablesHProduct->setRowCount(250);
	ui.tablesHProduct->setColumnHidden(0,true);
	ui.tablesHProduct->horizontalHeader()->setResizeMode(QHeaderView::Stretch) ;//宽度自动适应

	QStringList headers; 
	headers <<"产品ID"<< "商品ID" <<"分类ID"<<"本地名称"<<"产品属性"<<"产品产地"<<"商品单位"<<"正常售价"<<"志愿者200小时售价"<<"志愿者500小时售价"<<"帮困售价"<<"商品毛重";
    ui.tablesHProduct->setHorizontalHeaderLabels(headers); 
	ui.tablesHProduct->setEditTriggers ( QAbstractItemView::NoEditTriggers );
	ui.tablesHProduct->setSelectionBehavior(QAbstractItemView::SelectRows);

	
	//for( go_iter=map_shproductinfo.constBegin(); go_iter!=map_shproductinfo.constEnd(); go_iter++,i++)
	for( int index=0;index < vector_shproductinfo.size(); index++,i++)
	{
		ui.tablesHProduct->setItem(i,0, new QTableWidgetItem(vector_shproductinfo.at(index).productId));
		ui.tablesHProduct->setItem(i,1, new QTableWidgetItem(vector_shproductinfo.at(index).Itemid));
		ui.tablesHProduct->setItem(i,2, new QTableWidgetItem(vector_shproductinfo.at(index).Classid)); 
		ui.tablesHProduct->setItem(i,3, new QTableWidgetItem(vector_shproductinfo.at(index).Locname));

		if(tr("00")==tr(vector_shproductinfo.at(index).Djyzt))
			ui.tablesHProduct->setItem(i,4, new QTableWidgetItem("预约")); 
		else if(tr("01")==tr(vector_shproductinfo.at(index).Djyzt))
			ui.tablesHProduct->setItem(i,4, new QTableWidgetItem("实时")); 
		else if(tr("02")==tr(vector_shproductinfo.at(index).Djyzt))
			ui.tablesHProduct->setItem(i,4, new QTableWidgetItem("团购")); 
		else
			ui.tablesHProduct->setItem(i,4, new QTableWidgetItem("未定义")); 

		ui.tablesHProduct->setItem(i,5, new QTableWidgetItem(vector_shproductinfo.at(index).Origin));

		ui.tablesHProduct->setItem(i,6, new QTableWidgetItem(vector_shproductinfo.at(index).Unit));
		ui.tablesHProduct->setItem(i,7, new QTableWidgetItem(vector_shproductinfo.at(index).Price1)); 
		ui.tablesHProduct->setItem(i,8, new QTableWidgetItem(vector_shproductinfo.at(index).Price2));
		ui.tablesHProduct->setItem(i,9, new QTableWidgetItem(vector_shproductinfo.at(index).Price3)); 
		ui.tablesHProduct->setItem(i,10, new QTableWidgetItem(vector_shproductinfo.at(index).Price4));
		ui.tablesHProduct->setItem(i,11, new QTableWidgetItem(vector_shproductinfo.at(index).weightvalue)); 
	}

	int index=ui.sh_product_box->currentIndex();
	ui.sh_product_box->setCurrentIndex(index);

}

void shopsubmit::initProductTable()
{
	QHash<QString,ProductInfo>::const_iterator go_iter;
	int i=0;

	//插入前先清空
	ui.tableWidgetProduct->clear();
	ui.tableWidgetProduct->setColumnCount(12);
	ui.tableWidgetProduct->setRowCount(250);
	ui.tableWidgetProduct->setColumnHidden(0,true);
	ui.tableWidgetProduct->horizontalHeader()->setResizeMode(QHeaderView::Stretch) ;//宽度自动适应

	QStringList headers; 
	headers <<"产品ID"<< "商品ID" <<"分类ID"<<"本地名称"<<"产品属性"<<"产品产地"<<"商品单位"<<"正常售价"<<"志愿者200小时售价"<<"志愿者500小时售价"<<"帮困售价"<<"商品毛重";
    ui.tableWidgetProduct->setHorizontalHeaderLabels(headers); 
	ui.tableWidgetProduct->setEditTriggers ( QAbstractItemView::NoEditTriggers );
	ui.tableWidgetProduct->setSelectionBehavior(QAbstractItemView::SelectRows);

	//for( go_iter=map_productinfo.constBegin(); go_iter!=map_productinfo.constEnd(); go_iter++,i++)
	for( int index=0;index < vector_productinfo.size(); index++,i++)
	{
		ui.tableWidgetProduct->setItem(i,0, new QTableWidgetItem(vector_productinfo.at(index).productId));
		ui.tableWidgetProduct->setItem(i,1, new QTableWidgetItem(vector_productinfo.at(index).Itemid));
		ui.tableWidgetProduct->setItem(i,2, new QTableWidgetItem(vector_productinfo.at(index).Classid)); 
		ui.tableWidgetProduct->setItem(i,3, new QTableWidgetItem(vector_productinfo.at(index).Locname));

		if(tr("00")==tr(vector_productinfo.at(index).Djyzt))
			ui.tableWidgetProduct->setItem(i,4, new QTableWidgetItem("预约")); 
		else if(tr("01")==tr(vector_productinfo.at(index).Djyzt))
			ui.tableWidgetProduct->setItem(i,4, new QTableWidgetItem("实时")); 
		else if(tr("02")==tr(vector_productinfo.at(index).Djyzt))
			ui.tableWidgetProduct->setItem(i,4, new QTableWidgetItem("团购")); 
		else
			ui.tableWidgetProduct->setItem(i,4, new QTableWidgetItem("未定义")); 

		ui.tableWidgetProduct->setItem(i,5, new QTableWidgetItem(vector_productinfo.at(index).Origin));

		ui.tableWidgetProduct->setItem(i,6, new QTableWidgetItem(vector_productinfo.at(index).Unit));
		ui.tableWidgetProduct->setItem(i,7, new QTableWidgetItem(vector_productinfo.at(index).Price1)); 
		ui.tableWidgetProduct->setItem(i,8, new QTableWidgetItem(vector_productinfo.at(index).Price2));
		ui.tableWidgetProduct->setItem(i,9, new QTableWidgetItem(vector_productinfo.at(index).Price3)); 
		ui.tableWidgetProduct->setItem(i,10, new QTableWidgetItem(vector_productinfo.at(index).Price4));
		ui.tableWidgetProduct->setItem(i,11, new QTableWidgetItem(vector_productinfo.at(index).weightvalue)); 
	}



}



void shopsubmit::getClassItem(QTableWidgetItem* item)
{
	 QString path1("/OAapp/WebObjects/OAapp.woa?cjApply=sales&spcode=");

	 int row=item->row();
	 QTableWidgetItem* item1=ui.tableWidgetClass->item(row,0);
	 QString path2=item1->text();
	 QString path3("&deviceId=home12");

	//下载产品xml
	QString ipStr(usrAuth.serverip);
	QString path("/OAapp/WebObjects/OAapp.woa?cjApply=classify");
    productConn->get(QNetworkRequest(QUrl(ipStr+path1+path2+path3)));
	qDebug()<<"产品列表："<<ipStr+path1+path2+path3;
}


void shopsubmit::getShProductItem(QTableWidgetItem* item)
{
	 int row=item->row();
    currentShIndex=row;//产品修改的地方会用到
	//设置菜照路径

	 ui.shDetailInfoList->clear();
	 QTableWidgetItem* item1=ui.tablesHProduct->item(row,0);
	 shProductIndex=item1->text();

	 //QHash<QString,ProductInfo>::const_iterator go_iter;//vector_productinfo.at(index).
	 //go_iter=map_shproductinfo.find(shProductIndex);
	 int i;
	 for(i=0;i< vector_shproductinfo.size();i++)
	 {
		if(strcmp(vector_shproductinfo.at(i).productId,shProductIndex.toStdString().c_str())==0)
			break;
	
	 }

	 if(strlen(vector_shproductinfo.at(i).productId)!=0)
	 {

		//获取菜照路径
		 QString ipStr(usrAuth.serverip);
		 QString path(tr("/htoa/userImg/v/"));
		 QString imagefile=vector_shproductinfo.at(i).PiclinkL;

		// QMessageBox::warning(this,"程序调试---",ipStr+path+imagefile);
		 setNetworkPic(ipStr+path+imagefile);

		 //addItems
		 QStringList rowList;    
		 //这里可能有bug
		 rowList<<tr("  待审核产品详细信息，请您仔细核对检查：")<<tr("   名称拼音：    ")+vector_shproductinfo.at(i).Itemcode<<tr("   本地名称：    ")+vector_shproductinfo.at(i).Locname<<tr("   产品产地：   ")+vector_shproductinfo.at(i).Origin<<tr("   产品单位：   ")+vector_shproductinfo.at(i).Unit<<tr("   产品售价：   ")+vector_shproductinfo.at(i).Price1<<tr("   产品重量：   ")+vector_shproductinfo.at(i).weightvalue<<tr("   商家编码：   ")+vector_shproductinfo.at(i).pwd;
		 ui.shDetailInfoList->addItems(rowList);
	 }
}

void shopsubmit::getProductItem(QTableWidgetItem* item)
{
	 int row=item->row();
	 QTableWidgetItem* item1=ui.tableWidgetProduct->item(row,0);
	 QString path2=item1->text();

	 //QHash<QString,ProductInfo>::const_iterator go_iter;
	 //go_iter=map_productinfo.find(path2);
	 int i;
	 for( i=0;i< vector_productinfo.size();i++)
	 {
		if(strcmp(vector_productinfo.at(i).productId,path2.toStdString().c_str()))
			break;
	 }


	 QTableWidgetItem* item2=ui.tableWidgetProduct->item(row,4);

	 if(tr("团购")==item2->text())
	 {
			strBuffer1 = "2015-01-07 09:58:00";
			//time1 = QDateTime::fromString(strBuffer1, "yyyy-MM-dd hh:mm:ss");
			time1 = QDateTime::fromString(vector_productinfo.at(i).systemdate, "yyyy-MM-dd hh:mm:ss");
			subSecs=time1.toTime_t();

			strBuffer2 = "2015-01-01 10:54:00";
			//time2 = QDateTime::fromString(strBuffer2, "yyyy-MM-dd hh:mm:ss");
			time2 = QDateTime::fromString(vector_productinfo.at(i).limit, "yyyy-MM-dd hh:mm:ss");

			if( !tuanGouTimer->isActive())
			{	
				tuanGouTimer->start(1000); // 每隔1s
			}else{
				tuanGouTimer->stop();
				tuanGouTimer->start(1000); // 每隔1s

			}
	 }else{
			tuanGouTimer->stop();
			ui.countdownLabel->setText(tr("实时产品！"));
	 }
}


void shopsubmit::replyClassConn(QNetworkReply* reply)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString all = codec->toUnicode(reply->readAll());
	//qDebug()<<all;
    QFile file("class.xml");  
  
   if(!file.open(QIODevice::WriteOnly | QIODevice::Text))  
    {  
        qDebug()<< "Open failed." <<file.error(); 
        return;  
    }  
      
    QTextStream txtOutput(&file);  
    txtOutput << all << endl;  
      
    file.close(); 

	loadXmlClass();
	initClassTable();
}

void shopsubmit::replyClassDelConn(QNetworkReply *reply)
{
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QString all = codec->toUnicode(reply->readAll());
	QMessageBox::warning(this,"温馨提示！",all);
}


void shopsubmit::replyProductConn(QNetworkReply* reply )
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString all = codec->toUnicode(reply->readAll());
    QFile file("product.xml");  
  
   if(!file.open(QIODevice::WriteOnly | QIODevice::Text))  
    {  
        qDebug()<< "Open failed." <<file.error(); 
        return;  
    }  
      
    QTextStream txtOutput(&file);  
    txtOutput << all << endl;  
      
    file.close(); 
    
	loadXmlProduct();
	initProductTable();
}

void shopsubmit::replyshProductConn(QNetworkReply* reply)
{
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString all = codec->toUnicode(reply->readAll());
	//QMessageBox::warning(this,"温馨提示！",all);

    QFile file("shproduct.xml");  
  
   if(!file.open(QIODevice::WriteOnly | QIODevice::Text))  
    {  
        qDebug()<< "Open failed." <<file.error(); 
        return;  
    }  
      
    QTextStream txtOutput(&file);  
    txtOutput << all << endl;  
      
    file.close(); 
    
	loadShXmlProduct();
	initProductShTable();
}

////请求商品审核
void shopsubmit::replyRequireShConn(QNetworkReply* reply )
{
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString all = codec->toUnicode(reply->readAll());
	QMessageBox::warning(this,"提示信息！",all);

	//更新待审核列表
	QString tmpStr1(tr("/OAapp/WebObjects/OAapp.woa?cjApply=noAuditSaleList&classid="));
	QString tmpStr2(tr("&pwd="));
	QString tmpStr3(tr("1234"));

	QString ipStr(usrAuth.serverip);
	qDebug()<<ipStr+tmpStr1+shKeyIndex+tmpStr2+tmpStr3;
    shProductConn->get(QNetworkRequest(QUrl(ipStr+tmpStr1+shKeyIndex+tmpStr2+tmpStr3)));
}



void shopsubmit::replyProductDelConn(QNetworkReply* reply)
{
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString all = codec->toUnicode(reply->readAll());
	QMessageBox::warning(this,"温馨提示！",all);
}


void shopsubmit::replyUploadConn(QNetworkReply* reply )
{
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString all = codec->toUnicode(reply->readAll());
	QMessageBox::warning(this,"温馨提示！",all);
}

void shopsubmit::uploadfile(QString filename)
{
	char *dataBuffer=NULL;
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    int fileLen = file.size();
    QDataStream in(&file);
    dataBuffer = new char[fileLen];
    in.readRawData( dataBuffer, fileLen);

    file.close();

	QString path1(usrAuth.serverip);
	QString path2(tr(""));


    QNetworkRequest request(QUrl(tr("http://10.10.100.49/myweb/qhttpupload.php?act=up&filename=2.jpg")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    QByteArray arr = QByteArray(dataBuffer, fileLen);
	uploadConn->post(request , arr);
}

void shopsubmit::on_bigpicBtn_clicked()
{
	QString ret=selectFile();
	ui.lineEdit_19->setText(ret);
}
void shopsubmit::on_smallpicBtn_clicked()
{
	QString ret=selectFile();
	ui.lineEdit_20->setText(ret);
}

void shopsubmit::on_bigpicBtn_2_clicked()
{ 
	//uploadImage(ui.lineEdit_19->text().toStdString());
	//uploadfile(ui.lineEdit_19->text());
}
void shopsubmit::on_smallpicBtn_2_clicked()
{
	ui.lineEdit_20->setText(ui.lineEdit_19->text());
}

QString shopsubmit::selectFile()
{
    QStringList    fileNameList;
    QString fileName0;
    QString fileName;
    QFileDialog* fd = new QFileDialog(this);//创建对话框
    fileDlg->resize(600,480);    //设置显示的大小
    fileDlg->setFilter( "Allfile(*.*);;jpgfile(*.jpg);;wmafile(*.wma);;wavefile(*.wav)"); //设置文件过滤器
    fileDlg->setViewMode(QFileDialog::List);  //设置浏览模式，有 列表（list） 模式和 详细信息（detail）两种方式
    if ( fileDlg->exec() == QDialog::Accepted )   //如果成功的执行
    {
        fileNameList = fileDlg->selectedFiles();      //返回文件列表的名称
        fileName = fileNameList[0];            //取第一个文件名
    }
    else
        fileDlg->close();
	return fileName;
}

void shopsubmit::replyProductMode(QNetworkReply* reply )
{
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString all = codec->toUnicode(reply->readAll());
	QMessageBox::warning(this,"------",all);
}

void shopsubmit::uploadFinished( QNetworkReply* reply)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString all = codec->toUnicode(reply->readAll());
    QMessageBox::warning(this,"fanhui",all);
}


void shopsubmit::on_productModeBtn_clicked()
{
	QString ipStr(usrAuth.serverip);
	QString path1("/OAapp/WebObjects/OAapp.woa?cjApply=productSta&id=");
	QString path2("&limit=");
	QString path3("&sta=");


	//获取用户选择的活动项目
	QString tmpStr,tmpCode;
	tmpStr=ui.comboproduct->currentText();
	if(tmpStr==tr("团购"))
		tmpCode=tr("02");
	else if(tmpStr==tr("实时"))
		tmpCode=tr("01");
	else if(tmpStr==tr("预约"))
		tmpCode=tr("00");
	else
		QMessageBox::warning(this,tr("警告！"),tr("服务器端数据已经改变，请联系修改！"));

	QString purchaseTime;

	if(tr("团购")==tmpStr)
	{	
		//获取团购日期
	
		purchaseTime=ui.purchaseLE->text();
		if(purchaseTime.isEmpty())
		{
			QMessageBox::warning(this,tr("警告！"),tr("请填写团购时间段！"));
			return;
		}

		//获取产品ID号
		QString productId;
		int row=ui.tableWidgetProduct->currentRow();
		if((row!=-1)&&(row<ui.tableWidgetProduct->rowCount()))
		{
			productId=ui.tableWidgetProduct->item(row,0)->text();
		}

		if((row==-1)||(productId.isEmpty()))
		{
			QMessageBox::warning(this,tr("警告！"),tr("请选择做活动的产品！"));
			return;
		}
		qDebug()<<ipStr+path1+productId+path2+purchaseTime+path3+tmpCode;
		productMode->get(QNetworkRequest(QUrl(ipStr+path1+productId+path2+purchaseTime+path3+tmpCode)));
	}else{
			//获取产品ID号
		QString productId;
		int row=ui.tableWidgetProduct->currentRow();
		if((row!=-1)&&(row<ui.tableWidgetProduct->rowCount()))
		{
			productId=ui.tableWidgetProduct->item(row,0)->text();
		}

		if((row==-1)||(productId.isEmpty()))
		{
			QMessageBox::warning(this,tr("警告！"),tr("请选择做活动的产品！"));
			return;
		}
		qDebug()<<ipStr+path1+productId+path2+purchaseTime+path3+tmpCode;
		productMode->get(QNetworkRequest(QUrl(ipStr+path1+productId+path3+tmpCode)));
	}


} 

//addProductBtn
void shopsubmit::on_addProductBtn_clicked()
{
	  //获取该类商品当前产品数，由此产生ID号，也可以是数量
	  int productCount=vector_productinfo.size();

	  int rowProduct=ui.tableWidgetProduct->currentRow();
	  int rowClass=ui.tableWidgetClass->currentRow();
	  if(rowClass==-1)
	  {
	  	  QMessageBox::warning(this,tr("温馨提示"),tr("您未选择分类，请先选择分类！"));
	  	  return ;  
	  }
	  
	  QStringList liststr;
	  liststr << "暂无";
	  ui.comboBox->addItems(liststr);
     // ui.stackedWidget->setCurrentIndex(1);

       //productCount

	  QUuid id = QUuid::createUuid();  
      QString strId = id.toString();
	  ui.lineEdit_8->setText(strId);
     
	  //ui.lineEdit_8->setText(QString("%1").arg(productCount+1, 10, 10, QChar('0')));
	  //ui.lineEdit_10->setText(tr(""));
	  ui.lineEdit_13->setText(tr("01"));
  	  ui.lineEdit_14->setText(tr("全国"));
	  ui.lineEdit_15->setText(tr("暂无"));
	  ui.lineEdit_16->setText(tr("暂无"));
      ui.lineEdit_18->setText(ui.tableWidgetClass->item(rowClass,0)->text());
	  ui.lineEdit_22->setText(tr("0.0"));
	  ui.lineEdit_23->setText(tr("0.0"));
	  ui.lineEdit_24->setText(tr("0.0"));
	  ui.lineEdit_25->setText(tr("0.0"));

	  ui.textEdit->setText(tr("暂无"));
	  ui.textEdit_3->setText(tr("暂无"));
}

void shopsubmit::on_goodsBackBtn_clicked()
{
	ui.stackedWidget->setCurrentIndex(0);
	QString ipStr(usrAuth.serverip);
	QString path("/OAapp/WebObjects/OAapp.woa?cjApply=classify");
    classConn->get(QNetworkRequest(QUrl(ipStr+path)));

	//插入前先清空
	ui.tableWidgetProduct->clear();
	ui.tableWidgetProduct->setColumnCount(12);
	ui.tableWidgetProduct->setRowCount(250);
	ui.tableWidgetProduct->setColumnHidden(0,true);
	ui.tableWidgetProduct->horizontalHeader()->setResizeMode(QHeaderView::Stretch) ;//宽度自动适应

	QStringList headers; 
	headers <<"产品ID"<< "商品ID" <<"分类ID"<<"本地名称"<<"产品属性"<<"产品产地"<<"商品单位"<<"正常售价"<<"志愿者200小时售价"<<"志愿者500小时售价"<<"帮困售价"<<"商品毛重";
    ui.tableWidgetProduct->setHorizontalHeaderLabels(headers); 
	ui.tableWidgetProduct->setEditTriggers ( QAbstractItemView::NoEditTriggers );
	ui.tableWidgetProduct->setSelectionBehavior(QAbstractItemView::SelectRows);

	//获取当前分类
	QString path1("/OAapp/WebObjects/OAapp.woa?cjApply=sales&spcode=");
	QString path2=ui.lineEdit_18->text();
	QString path3("&deviceId=home12");
	QModelIndex currentIndex=ui.tableWidgetClass->currentIndex();

	//设置分类为之前的分类
	ui.tableWidgetClass->setCurrentIndex(currentIndex);

	//下载产品xml
    productConn->get(QNetworkRequest(QUrl(ipStr+path1+path2+path3)));
	qDebug()<<"产品列表："<<ipStr+path1+path2+path3;

}



void shopsubmit::on_shProductBtn_clicked()
{
	  initClassList();
	  int productCount=vector_productinfo.size();

	  int rowProduct=ui.tableWidgetProduct->currentRow();
	  int rowClass=ui.tableWidgetClass->currentRow();

	  
	  QStringList liststr;
	  liststr << "暂无";
	  ui.comboBox->addItems(liststr);
      ui.stackedWidget->setCurrentIndex(1);

       //productCount

	  QUuid id = QUuid::createUuid();  
      QString strId = id.toString();
	  ui.lineEdit_8->setText(strId);
     
	  //ui.lineEdit_8->setText(QString("%1").arg(productCount+1, 10, 10, QChar('0')));
	  ui.lineEdit_10->setText(tr("暂无"));
	  ui.lineEdit_13->setText(tr("01"));
  	  ui.lineEdit_14->setText(tr("全国"));
	  ui.lineEdit_15->setText(tr("暂无"));
	  ui.lineEdit_16->setText(tr("暂无"));
     // ui.lineEdit_18->setText(ui.tableWidgetClass->item(rowClass,0)->text());
	  ui.lineEdit_22->setText(tr("0.0"));
	  ui.lineEdit_23->setText(tr("0.0"));
	  ui.lineEdit_24->setText(tr("0.0"));
	  ui.lineEdit_25->setText(tr("0.0"));

	  ui.textEdit->setText(tr("暂无"));
	  ui.textEdit_3->setText(tr("暂无"));
	  
	  //int index=ui.sh_product_box->currentIndex();
	  initProductShTable();
	  ui.stackedWidget->setCurrentIndex(1);
	  //ui.sh_product_box->setCurrentIndex(index);
	  
}



void shopsubmit::on_addClassOkBtn_clicked()
{
	  //ui.stackedWidget->setCurrentIndex(3);
	  //int classCount=map_classinfo.count();
	  //ui.goodIDLine->setText(QString("%1").arg(classCount+1, 4, 10, QChar('0')));// 4, 10, QChar('0')));
	  QUuid id = QUuid::createUuid();  
	  QString strId = id.toString();  
	  ui.goodIDLine->setText(strId);
}

void shopsubmit::on_classDelBtn_clicked()
{
	//商家编号
	QString pwdstr=tr("1234");
	
	//获取商品标识
	QString typeCode;
	typeCode=ui.goodIDLine->text();

	//获取当前日期
	QString todayStr;
	QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
	todayStr = time.toString("yyyy-MM-dd"); //设置显示格式

	QString md5Str=getMd5Check(todayStr+pwdstr);

	//http://localhost:88/OAapp/WebObjects/OAapp.woa?cjApply=delsptype&typecode=0051&pwd=1234 &checkstr=

	QString ipStr(usrAuth.serverip);
	QString path1("/OAapp/WebObjects/OAapp.woa?cjApply=delsptype&typecode=");
	QString path2("&pwd=");
	QString path3("&checkstr=");

	qDebug()<<ipStr+path1+typeCode+path2+pwdstr+path3+md5Str;
	QMessageBox::StandardButton rb = QMessageBox::question(NULL, tr("温馨提示"), tr("删除后不可恢复，您确定删除该分类吗？"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	if(rb == QMessageBox::Yes)
	{
			classDelConn->get(QNetworkRequest(QUrl(ipStr+path1+typeCode+path2+pwdstr+path3+md5Str)));
	}
}

void shopsubmit::on_goodsDelBtn_clicked()
{
	//商家编号
	QString pwdstr=tr("1234");
	
	//获取商品标识
	QString itemid;
	itemid=ui.lineEdit_8->text();

	//获取当前日期
	QString todayStr;
	QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
	todayStr = time.toString("yyyy-MM-dd"); //设置显示格式

	QString md5Str=getMd5Check(todayStr+pwdstr);

	QString ipStr(usrAuth.serverip);
	QString path1("/OAapp/WebObjects/OAapp.woa?cjApply=delproduct&itemid=");
	QString path2("&pwd=");
	QString path3("&checkstr=");

	qDebug()<<ipStr+path1+itemid+path2+pwdstr+path3+md5Str;

		QMessageBox::StandardButton rb = QMessageBox::question(NULL, tr("温馨提示！"), tr("删除后不可恢复，您确定删除该产品吗？"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	if(rb == QMessageBox::Yes)
	{
			productDelConn->get(QNetworkRequest(QUrl(ipStr+path1+itemid+path2+pwdstr+path3+md5Str)));
	}
}


void shopsubmit::on_spUploadBtn_clicked()
{
	//QMessageBox::warning(this,"程序调试","--------------------------");
	//获取图片路径名
	uploadImage(ui.lineEdit_20->text().toStdString());
}


void shopsubmit::setJiShi()
{
    subSecs+=1;
    //QDateTime	subDayTime=QDateTime::fromTime_t(subTime);
    int subTime=time2.toTime_t()-subSecs;
	if(subTime>=0)
	{
		uint days=subTime/86400;
		uint subdays=subTime%86400;
		uint hours=subdays/3600;
		uint subhours=subdays%3600;
		uint mins=subhours/60;
		uint submins=subhours%60;
		uint secs=submins;
		ui.countdownLabel->setText(QString("距离团购结束还有：%1 天 %2 小时 %3 分 %4 秒").arg(days).arg(hours).arg(mins).arg(secs));
		//qDebug()<<days<<":day"<<hours<<":hours"<<mins<<":mins"<<secs<<":secs";
	}else{
		ui.countdownLabel->setText(tr("团购结束！"));
		qDebug()<<"团购已结束秒数："<<subTime;
	}
}

void shopsubmit::shBoxValueChanged(QString context)
{
	//QMessageBox::warning(this,"程序调试",context);
   	QMap<QString,GoogsClass>::const_iterator go_iter;
	int i=0;
	for( go_iter=map_classinfo.constBegin(); go_iter!=map_classinfo.constEnd(); go_iter++,i++)
	{
		if(context.compare(go_iter.value().goodsClassName)==0){
			shKeyIndex=go_iter.value().goodsClassID;
			break;
		}
		
	}

	//http://222.73.197.174:88/OAapp/WebObjects/OAapp.woa?cjApply=noAuditSaleList&classid=&pwd=
	QString ipStr(usrAuth.serverip);
	QString tmpStr1(tr("/OAapp/WebObjects/OAapp.woa?cjApply=noAuditSaleList&classid="));
	QString tmpStr2(tr("&pwd="));
	QString tmpStr3(tr("1234"));

	qDebug()<<ipStr+tmpStr1+shKeyIndex+tmpStr2+tmpStr3;
    shProductConn->get(QNetworkRequest(QUrl(ipStr+tmpStr1+shKeyIndex+tmpStr2+tmpStr3)));
}

QString shopsubmit::getMd5Check( QString md5key )
{
    /**********加密开始***********/  
	QString md5Value;
    QByteArray bb;  
    bb = QCryptographicHash::hash(md5key.toAscii(),QCryptographicHash::Md5);  
    md5Value.append(bb.toHex());  
    /***********加密结束****************/  
	

    //QMessageBox::warning(this,"warning",md5Value.toUpper(),QMessageBox::Yes);  

    return  md5Value.toUpper();
}


void shopsubmit::initClassList()//QMap<QString,GoogsClass>  ;//map_classinfo
{
	QMap<QString,GoogsClass>::const_iterator go_iter;
	int i=0;
	ui.sh_product_box->clear();
	for( go_iter=map_classinfo.constBegin(); go_iter!=map_classinfo.constEnd(); go_iter++,i++)
	{
		ui.sh_product_box->addItem(go_iter.value().goodsClassName);
	}
	//ui.doc_list_box->setCurrentIndex(0);
}

//下载网络图片
void shopsubmit::setNetworkPic(QString szUrl)
{
    QUrl url(szUrl);
    QNetworkAccessManager manager;
    QEventLoop loop;

    qDebug() << "Reading picture form " << url;
    QNetworkReply *reply = manager.get(QNetworkRequest(url));
    //请求结束并下载完成后，退出子事件循环
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    //开启子事件循环
    loop.exec();

    QByteArray jpegData = reply->readAll();
    QPixmap pixmap;

    pixmap.loadFromData(jpegData);

	QPixmap retmap=pixmap.scaled(350,300);
    ui.shImageLabel->setPixmap(retmap); // 你在QLabel显示图片
} 


bool shopsubmit::uploadImage(std::string localPath)
{
    QFile file(QString(localPath.c_str()));

	//获取文件名
	char dst[128];
	char filename[128];
	char dirname[128];

	strcpy(dst,localPath.c_str());
	GetFileInfo(dst,filename,dirname);
	//qDebug()<<filename;

    if (!file.open(QIODevice::ReadOnly))
    {
        file.close();
        return false;
    }




    QString crlf="\r\n";
    qsrand(QDateTime::currentDateTime().toTime_t());
    QString b=QVariant(qrand()).toString()+QVariant(qrand()).toString()+QVariant(qrand()).toString();
    QString boundary="---------------------------"+b;

    QString endBoundary=crlf+"--"+boundary+"--"+crlf;
    QString contentType="multipart/form-data; boundary="+boundary;
    boundary="--"+boundary+crlf;
    QByteArray bond=boundary.toAscii();

    QByteArray send;
    send.append(bond);
    boundary = crlf + boundary;
    bond = boundary.toAscii();
    //send.append(QString("Content-Disposition: form-data; name=\"image\";filename="+QString(filename)+crlf).toAscii());
	send.append(QString("Content-Disposition: form-data; name=\"image\";filename=\""+QString(localPath.c_str())+QString("\"")+crlf).toAscii());
	//send.append(QString("Content-Disposition: form-data; name=\"image\";filename=").toAscii());
	//send.append("\"");
	//send.append(QString(filename).toAscii());
	//send.append("\"");
	//send.append(crlf.toAscii());

    send.append(QString("Content-Type: image/png"));
    send.append(bond);
    send.append(crlf.toAscii());

    qDebug()<<send;


    send.append(file.readAll());
    send.append(QString("\r\n").toAscii());
    send.append(endBoundary.toAscii());
    file.close();


    QNetworkRequest request(QUrl(tr("http://180.153.152.45:7070/vas/c2c/third/uploadResource.php")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "Application/octet-stream");
    request.setHeader(QNetworkRequest::ContentTypeHeader, contentType.toAscii());
    request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(send.size()).toString());
    _reply = _uploadManager->post(request , send);

    return true;

}
