
#include <QApplication>
#include "sensorlog.h"

int
main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	SensorLog log;

	QChartView chartView(&log.chart);
	chartView.setRenderHint(QPainter::Antialiasing);

	QMainWindow window;
	window.setCentralWidget(&chartView);
	window.showFullScreen();

	return app.exec();
}
