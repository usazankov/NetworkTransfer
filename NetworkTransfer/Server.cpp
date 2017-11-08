#include "Server.h"

Server::Server(int nPort, QObject *parent)
	: QObject(parent)
{
	serv = new QTcpServer(this);
	port = (quint16)nPort;
	error = false;
}

bool Server::dataIsNotNull()
{
	return !dat.isEmpty() ? true : false;
}

QString Server::readData()
{
	if (!dat.isEmpty())
		return dat.dequeue();

		return QString();
}

QString Server::errorMessage() const
{
	return errorMes;
}

bool Server::isError() const
{
	if (error)
		return true;
	else
		return false;
}

bool Server::start()
{
	error = 0;
	qDebug() << "bool Server::start()";
	if (!serv->isListening())
	{
		if (!serv->listen(QHostAddress::Any, port)) 
		{
			qDebug() << "Server Error. Unable to start the server: ";
			qDebug() << serv->errorString();
			error = true;
			errorMes.clear();
			errorMes.append("Server Error. Unable to start the server: ");
			errorMes.append(serv->errorString());
			serv->close();
			return false;
		}
		qDebug() << "connect(serv, SIGNAL(newConnection()) this, SLOT(slotNewConnection())";
		connect(serv, SIGNAL(newConnection()),
			this, SLOT(slotNewConnection())
		);
	}
	return true;
}

bool Server::stop()
{
	error = 0;
	if(serv->isListening())
	{
		serv->close();
		if (serv->isListening()) 
		{
			errorMes.clear();
			errorMes.append("Server Error. Unable to stop the server: ");
			error = true;
			return false;
		}
		else
		{
			error = false;
			return true;
		}
	}
}

Server::~Server()
{
	if (serv->isListening()) 
	{
		serv->close();
	}
	delete serv;
}

void Server::slotNewConnection() 
{
	qDebug() << "void Server::slotNewConnection() ";
	QTcpSocket* pClientSocket = serv->nextPendingConnection();
	connect(pClientSocket, SIGNAL(disconnected()),
		pClientSocket, SLOT(deleteLater())
	);
	connect(pClientSocket, SIGNAL(readyRead()),
		this, SLOT(slotReadClient())
	);
	qDebug() << "sendToClient(pClientSocket, Server Response : Connected!);";
	sendToClient(pClientSocket, "Server Response: Connected!");
}

void Server::sendToClient(QTcpSocket * pSocket, const QString & str)
{
	qDebug() << "void Server::sendToClient(QTcpSocket * pSocket, const QString & str)";
	QByteArray arrBlock;
	QDataStream out(&arrBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_5);
	out << quint16(0) << str;
	out.device()->seek(0);
	out << quint16(arrBlock.size() - sizeof(quint16));
	pSocket->write(arrBlock);
}

void Server::slotReadClient()
{
	QTcpSocket* pClientSocket = (QTcpSocket*)sender();
	QDataStream in(pClientSocket);
	in.setVersion(QDataStream::Qt_4_5);
	for (;;)
	{
		if (!m_nNextBlockSize)
		{
			if (pClientSocket->bytesAvailable() < sizeof(quint16))
			{
				break;
			}
			in >> m_nNextBlockSize;
		}
		if (pClientSocket->bytesAvailable() < m_nNextBlockSize)
		{
			break;
		}
		QString str;
		in >> str;
		dat.enqueue(str);
		m_nNextBlockSize = 0;
		sendToClient(pClientSocket,
			"Server Response: Received \"" + str + "\""
		);
	}
}
