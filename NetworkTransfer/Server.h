#pragma once

#include <QObject>
#include <QtCore\qdebug.h>
#include <QtCore\qdatastream.h>
#include <QTime>
#include <qprocess.h>
#include <QtSql>

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
	QString errorMes;
	bool error;
	QSqlDatabase dbase;
	QProcess* m_process;
public slots:
	void slotDataOnStdout();
signals:
	void test();
};
