#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>
#include <iostream>
#include "Server.h"

#ifdef ENABLE_EPOOL
#include "qeventdispatcher_epoll.h"
#endif

constexpr size_t DEFAULT_TREADS = 4;

constexpr size_t DEFAULT_PORT = 7036;

int main(int argc, char** argv)
{
#ifdef ENABLE_EPOOL
	QCoreApplication::setEventDispatcher(new QEventDispatcherEpoll);
#endif
	QCoreApplication app(argc, argv);
	QCoreApplication::setApplicationName("1CServerQt");
	QCoreApplication::setApplicationVersion("0.1");
	std::cout << "Started\n";
	qRegisterMetaType<qintptr>("qintptr");

	QCommandLineParser parser;
	parser.setApplicationDescription("Multi-threaded server on Qt");
	parser.addHelpOption();
	parser.addVersionOption();

	QCommandLineOption portOption(QStringList() << "p" << "port", "port",
		"port");
	parser.addOption(portOption);

	QCommandLineOption threadOption(QStringList() << "t" << "threads",
		"The number of worker threads", "count");
	parser.addOption(threadOption);

	parser.process(app);

	size_t port = parser.value(portOption).toUInt();
	if (port == 0)
	{
		port = DEFAULT_PORT;
	}

	size_t threads = parser.value(threadOption).toUInt();
	if (threads == 0)
	{
		threads = DEFAULT_TREADS;
	}

	Server server(threads);

	if (!server.listen(QHostAddress::LocalHost, port))
	{
		qCritical() << server.errorString();
		exit(1);
	}
	
	qDebug() << "Run server on" << port << "threads" << threads;
	return app.exec();
}
