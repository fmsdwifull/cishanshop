#ifndef SHOPSUBMIT_H
#define SHOPSUBMIT_H

#include <QWidget>
#include <QMap>
#include <QVector>
#include <QFileDialog>
#include "ui_shopsubmit.h"
#include "httpdeal.h"
#include "common.h"
#include <QTimer>

#include <string>



class shopsubmit : public QWidget
{
	Q_OBJECT

public:
	shopsubmit(QWidget *parent = 0);
	~shopsubmit();

	int isDigitStr(QString src);
	void uploadfile(QString filename);
	bool uploadImage(std::string localPath);

	QTimer *tuanGouTimer;

	//�Ź�����ʱ��
	QString strBuffer1;
    QDateTime time1;
    QString strBuffer2;
    QDateTime time2;

    uint subSecs;
	uint currentShIndex;
	QString shKeyIndex;
	QString shProductIndex;

private:
	Ui::shopsubmit ui;
	HttpDeal *httpconn;

	QMap<QString,GoogsClass>  map_classinfo;//
	
	
	//QMap<QString,ProductInfo>  map_productinfo,map_shproductinfo;//Ĭ����������byliu
	//QHash<QString,ProductInfo>  map_productinfo,map_shproductinfo;//Ĭ����������QHash
	QVector<ProductInfo>  vector_productinfo,vector_shproductinfo;//Ĭ����������QHash

	//���ط�����Ϣxml
	QNetworkAccessManager *classConn;

	QNetworkAccessManager *classDelConn; //connect(classDelConn, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyClassDelConn(QNetworkReply* )));
	//���ز�Ʒ��Ϣxml
	QNetworkAccessManager *productConn;

	//���ز�Ʒ��Ϣxml
	QNetworkAccessManager *shProductConn;

	//http://222.73.197.174:88/OAapp/WebObjects/OAapp.woa?cjApply=productAudit&id=&pwd=
	//�����Ʒ���
	QNetworkAccessManager *requireShConn;

	//�ϴ�ͼƬ
	QNetworkAccessManager *uploadConn;

	//�޸Ĳ�Ʒ����
	QNetworkAccessManager *productMode;

	//ɾ���޸Ĳ�Ʒ
	QNetworkAccessManager *productDelConn;
	QNetworkAccessManager *_uploadManager;
    QNetworkReply *_reply;
   
	//�ļ��Ի���
	QFileDialog* fileDlg;

	void loadXmlClass();
	void initClassTable();

	void loadXmlProduct();
	void loadShXmlProduct();
	void initProductTable();
	void initProductShTable();

	//����������ͼƬ
	void setNetworkPic(QString szUrl);


	QString selectFile();

	//md5�����ַ�������
	QString getMd5Check( QString md5key );



private slots:
	void on_manageBtn_clicked();
	void on_addProductBtn_clicked();
	void on_xgProductBtn_clicked();//��Ʒ����
	void on_classOkBtn_clicked();
	void on_goodsInfoOkBtn_clicked();
	void on_bigpicBtn_clicked();
	void on_smallpicBtn_clicked();
	void on_productModeBtn_clicked();
	void on_goodsDelBtn_clicked();
	void on_requireShBtn_clicked();
    void on_goodsBackBtn_clicked();
    void on_spUploadBtn_clicked();


	void on_bigpicBtn_2_clicked();
	void on_smallpicBtn_2_clicked();
	void setJiShi();

	void on_shProductBtn_clicked(); //xgProductBtn
	void on_xgClassBtn_clicked();
	void on_addClassOkBtn_clicked();
	void on_classDelBtn_clicked();

	void getClassItem(QTableWidgetItem* item);
	void getProductItem(QTableWidgetItem* item);
	void getShProductItem(QTableWidgetItem* item);
	void replyClassConn(QNetworkReply *reply);
	void replyClassDelConn(QNetworkReply *reply);

	void replyProductConn(QNetworkReply* reply );//�����
	void replyshProductConn(QNetworkReply* reply );//�����
	void replyRequireShConn(QNetworkReply* reply );//������Ʒ���
	void replyUploadConn(QNetworkReply* reply );
	void replyProductMode(QNetworkReply* reply );
	void replyProductDelConn(QNetworkReply* reply);

	void uploadFinished( QNetworkReply* );

	void shBoxValueChanged(QString context);
	void initClassList();
	

};

#endif // SHOPSUBMIT_H
