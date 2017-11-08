#pragma once

#include <QObject>
#include <QtNetwork\qtcpserver.h>
#include <QtNetwork\qtcpsocket.h>
#include <QtCore\qdebug.h>
#include <QtCore\qdatastream.h>
#include <QTime>
#include <QQueue>

class Server : public QObject
{
	Q_OBJECT

public:
	Server(int nPort, QObject *parent = 0);
	bool dataIsNotNull();
	QString readData();
	QString errorMessage()const;
	bool isError()const;
	bool start();
	bool stop();
	virtual ~Server();
private:
	QTcpServer *serv;
	quint16 m_nNextBlockSize;
	quint16 port;
	QQueue<QString> dat;
	void sendToClient(QTcpSocket* pSocket, const QString& str);
	QString errorMes;
	bool error;
public slots:
	void slotNewConnection();
	void slotReadClient();
signals:
	void test();
};