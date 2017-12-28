#include "Server.h"

Server::Server(int nPort, QObject *parent)
	: QObject(parent)
{
	error = false;
	m_process = new QProcess;
	connect(m_process,
		SIGNAL(readyReadStandardOutput()), this,
		SLOT(slotDataOnStdout())
	);
}

bool Server::dataIsNotNull()
{
	return false;
}

QString Server::readData()
{
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
	QString str;
	//m_process->start("Server1C.exe -p7635");
	//m_process->start("cmd /C dir");
	//if (m_process->waitForReadyRead(1000)) 
	dbase = QSqlDatabase::addDatabase("QSQLITE");
	dbase.setDatabaseName("C:/Users/dell/Documents/Visual Studio 2015/Projects/NetworkTransfer/Win32/Release/db.sqlite");
	if (!dbase.open())
	{
		qDebug() << "Что-то пошло не так!";
		return false;
	}
	QSqlQuery a_query;
	if (!a_query.exec("SELECT * FROM main_table")) {
		qDebug() << "Даже селект не получается, я пас.";
		return 0;
	}
	QSqlRecord rec = a_query.record();
	int number = 0;
	QString data = "";

	while (a_query.next()) {
		number = a_query.value(rec.indexOf("number")).toInt();
		data = a_query.value(rec.indexOf("data")).toString();

		qDebug() << "number is " << number
			<< ". data" << data;
	}
	{
		//str = m_process->readAllStandardOutput();
		//if (str.contains("Started")) 
		{
			if(m_process->state() == QProcess::Running)
				return true;
		}
	}
	return true;
}

bool Server::stop()
{
	m_process->terminate();
	return true;
}

Server::~Server()
{
	m_process->kill();
	if (m_process->waitForFinished(10000)) 
	{
		if (m_process->state() == QProcess::NotRunning) 
		{
			qDebug() << "Process finished";
		}
		else 
		{
			qDebug() << "Fail stopped";
		}
	}
	m_process->deleteLater();
}

void Server::slotDataOnStdout()
{
	qDebug() << m_process->readAllStandardOutput();
}
