#ifndef HTTPDEAL_H
#define HTTPDEAL_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QtNetwork>
class QNetworkReply;


class HttpDeal : public QObject
{
	Q_OBJECT

public:
	HttpDeal(QObject *parent);
	~HttpDeal();

	//�����û������б�
	QString serverIP;
   void setXmlFile(QString filename);
	void sendRequire(QString url);


private:
	QString downXmlUrl;//�����б����ص�ַ
	QNetworkAccessManager *manager;
	QString xmlfilename;
	
	void setUrl(QString url);

public slots:
	//�����û�����xml��ɴ�����
	void  replyFinished(QNetworkReply *reply);
	
};

#endif // HTTPDEAL_H
