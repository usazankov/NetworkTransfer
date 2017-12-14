#pragma once

#include <QtCore/QObject>
#include <QQueue>
#include <QDataStream>
#include <QtNetwork/QTcpSocket>

class Client : public QObject
{
	Q_OBJECT

public:

	Client(qintptr socketDescriptor, QObject* parent = 0);

	public slots:

	void onRequest();

	void client2world();
	void world2client();

	void sendSocksAnsver();

	void onClientDisconnected();
	void onWorldDisconnected();

private:
	quint16 m_nNextBlockSize;
	void done();

private:
	QQueue<QString> dat;
	QTcpSocket m_client;
};

