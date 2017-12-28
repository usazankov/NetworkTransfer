#pragma once

#include <QtCore/QObject>
#include <QQueue>
#include <QDataStream>
#include <QtNetwork/QTcpSocket>
#include "Storage.h"

class Client : public QObject
{
	Q_OBJECT

public:

	Client(qintptr socketDescriptor, QObject* parent = 0);

public slots:
	void onRequest();
	void sendToClient(QTcpSocket* pSocket, const QString& str);
	void onClientDisconnected();

private:
	quint16 m_nNextBlockSize;
	void done();

private:
	QTcpSocket m_client;
};

