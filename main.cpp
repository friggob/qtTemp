#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	QCommandLineParser *cp = new QCommandLineParser();
	cp->addHelpOption();
	cp->addVersionOption();
	a.setApplicationName("qtTemp");
	a.setApplicationVersion(APP_VERSION);

	cp->process(a);

	delete cp;

	MainWindow w;
	w.show();

    int ret = a.exec();

    return ret;
}
