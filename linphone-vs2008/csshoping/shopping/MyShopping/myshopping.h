#ifndef MYSHOPPING_H
#define MYSHOPPING_H

#include <QtGui/QWidget>
#include "ui_myshopping.h"
#include "common.h"
#include "myshopping.h"


class QNetworkReply;
class QNetworkAccessManager;
class QStandardItemModel;
class QStringListModel;

class MyShopping : public QWidget
{
	Q_OBJECT

public:
	MyShopping(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MyShopping();
   
	Ui::MyShoppingClass ui;
    
	//���浱ǰ�Ķ�����
	QString tmpOrderId;

	//��¼�������ǵڼ���
	int tmpSenderNum;


	//��������λ�ü�¼
	int horposion;
	int verposion;

	QString sipTel;

	//����������ʱ��
	QTimer *callTimer;
	//�û������ֵ�
	QMap<QString,GuestOrder>  m_guestmap;
    //�Ե绰����Ϊ����
	QMap<QString,GuestOrder>  m_guestmapTel;
	
	
	//�û�����״̬�ֵ䣨�Ա��Ϊ������
	QMap<QString,GoodsDics>   map_PsstatesNo;
	//�û�����״̬�ֵ䣨�Լ�ֵΪ������
	QMap<QString,GoodsDics>   map_PsstatesMsg;




	//�û���ʷ�����б�
	QMap<QString,GuestOrder>   m_historymap;

	//��������,��ʼ�������б�
	void iterGuestMap();

	//������ʷ����
	void iterHistoryMap();

	//���󶩵��б�
	void loadXML();

	//���󶩵��б�
	void historyXML();
	
	//���󶩵�״̬�ֵ�
	void loadDicXML();

	//��������״̬
	void setSendCombox();

public  slots:
	void linponeIterate();

    //��ʱ���¶����б�
	void updateGuest();

	//��ʷ����
	void updateHistory();

	//�����������û�����ʱ������ɫ�����仯
	void updateColor();

	//������ĸ���ť
	void on_homeBtn_clicked();
	void on_addGoodsBtn_clicked();
	void on_AppointBtn_clicked();
	void on_sendBtn_clicked();

	void on_HistoryBtn_clicked();
	void on_pushButton_clicked();

	void on_productQueryBtn_clicked();
	
	//�����û�����xml��ɴ�����
	void replyFinished(QNetworkReply *reply);

    //�����û���ʷ����xml��ɴ�����
	void replyHisFinished(QNetworkReply *reply);

	//�޸Ķ���״̬������
	void replyGoodState(QNetworkReply *reply);
	void replyGoodDics(QNetworkReply *reply);

    //�����б�
	void changeCurrent(const QModelIndex &current,const QModelIndex &previous);
	void dealCall(const QString &str);

	//�����Ҷ�
	void on_sipcallBtn_clicked();
	void on_siphungupBtn_clicked();

	//������ѯcomboBox������
	void conditionProcess(int index);


	//
	void verMove(int);
	void horMove(int);

private:
	//Ui::MyShoppingClass ui;
	//���ض���xml
	QNetworkAccessManager *manager;

	//������ʷ����xml
	QNetworkAccessManager *managerHistory;
	//�ύ����״̬
	QNetworkAccessManager *googstate;
    //���ض���״̬�Զ�xml
	QNetworkAccessManager *goodsdic;

	QStandardItemModel* initModel;
	QStandardItemModel* historyModel;
	          
	QStringListModel* listModel;
	
	int currentRow; //��ǰѡ�е���
	int currentQueryCombo;
};

#endif // MYSHOPPING_H
