#include "Storage.h"

Storage* Storage::p_storage = 0;

bool Storage::initDB()
{
	dbase = QSqlDatabase::addDatabase("QSQLITE");
	dbase.setDatabaseName("db.sqlite");
	if (!dbase.open()) 
	{
		qDebug() << "Что-то пошло не так!";
		return false;
	}
	QSqlQuery a_query;
	QString str = "CREATE TABLE main_table ("
		"number integer PRIMARY KEY NOT NULL, "
		"data VARCHAR(255) "
		");";
	if (!a_query.exec(str)) 
	{
		qDebug() << "Не удается создать таблицу!";
	}
	QString str_insert = "INSERT INTO main_table(number, data) "
		"VALUES (2, 'test_data2');";
	if (!a_query.exec(str_insert))
	{
		qDebug() << "Не удается добавить данные";
	}
	return true;
}

bool Storage::addString(const QString & str)
{

	return true;
}

QString Storage::getString()
{
	return QString();
}

Storage::Storage(QObject *parent)
	: QObject(parent)
{
	initDB();
}

Storage * Storage::instance()
{
	if (!p_storage) 
	{
		p_storage = new Storage;
	}
	return p_storage;
}

Storage::~Storage()
{
}
