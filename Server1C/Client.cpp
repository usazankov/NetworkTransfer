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
	m_nNextBlockSize = 0;
	QTcpSocket* m_client = (QTcpSocket*)sender();
	QDataStream in(m_client);
	in.setVersion(QDataStream::Qt_4_5);
	for (;;)
	{
		if (!m_nNextBlockSize)
		{
			if (m_client->bytesAvailable() < sizeof(quint16))
			{
				break;
			}
			in >> m_nNextBlockSize;
		}
		if (m_client->bytesAvailable() < m_nNextBlockSize)
		{
			break;
		}
		QString str;
		in >> str;
		qDebug() << str;
		Storage::instance()->addString(str);
		m_nNextBlockSize = 0;
		sendToClient(m_client,
			"Server Response: Received \"" + str + "\""
		);
	}

}

void Client::sendToClient(QTcpSocket * pSocket, const QString & str)
{
	QByteArray arrBlock;
	QDataStream out(&arrBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_5);
	out << quint16(0) << str;
	out.device()->seek(0);
	out << quint16(arrBlock.size() - sizeof(quint16));
	pSocket->write(arrBlock);
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
