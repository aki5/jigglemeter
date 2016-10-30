
#include <QApplication>
#include "sensorlog.h"

int
main(int argc, char *argv[])
{
	QApplication app(argc, argv);
    sensorlog log;
    //log.showFullScreen();
	return app.exec();
}
