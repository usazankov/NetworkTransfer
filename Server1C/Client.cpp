#include <cstdint>

#include <QtCore/QDebug>
#include <QtCore/QtEndian>
#include <QtNetwork/QHostAddress>

#include "Client.h"

Client::Client(qintptr socketDescriptor, QObject* parent) :
	QObject(parent)
{
	m_client.setSocketDescriptor(socketDescriptor);

	connect(&m_client, &QTcpSocket::readyRead,
		this, &Client::onRequest);

	connect(&m_client, &QTcpSocket::disconnected,
		this, &Client::onClientDisconnected);
}

void Client::onRequest()
{
	qDebug() << "void Server::slotReadClient()";
	m_nNextBlockSize = 0;
	QTcpSocket* m_client = (QTcpSocket*)sender();
	QDataStream in(m_client);
	in.setVersion(QDataStream::Qt_4_5);
	for (;;)
	{
		if (!m_nNextBlockSize)
		{
			qDebug() << "if (!m_nNextBlockSize)";
			if (m_client->bytesAvailable() < sizeof(quint16))
			{
				qDebug() << "if (pClientSocket->bytesAvailable() < sizeof(quint16))";
				qDebug() << m_client->bytesAvailable();
				break;
			}
			in >> m_nNextBlockSize;
			qDebug() << "in >> m_nNextBlockSize;";
			qDebug() << m_nNextBlockSize;
		}
		if (m_client->bytesAvailable() < m_nNextBlockSize)
		{
			qDebug() << m_client->bytesAvailable();
			qDebug() << "if (pClientSocket->bytesAvailable() < m_nNextBlockSize)";
			break;
		}
		QString str;
		in >> str;
		qDebug() << "str:";
		qDebug() << str;
		dat.enqueue(str);
		m_nNextBlockSize = 0;
		//sendToClient(pClientSocket,
		//	"Server Response: Received \"" + str + "\""
		//);
	}

}

void Client::sendSocksAnsver()
{
	
}

void Client::onClientDisconnected()
{

	done();
}

void Client::done()
{
	m_client.close();
	deleteLater();
}
