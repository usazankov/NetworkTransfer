#pragma once

#include <QObject>
#include <QtSql>
#include <qmutex.h>

class Storage : public QObject
{
	Q_OBJECT
private:
	QMutex mutex;
	static Storage *p_storage;
	bool initDB();
	QSqlDatabase dbase;
public:
	bool addString(const QString& str);
	QString getString();
	Storage(QObject *parent = 0);
	static Storage *instance();
	virtual ~Storage();
};
