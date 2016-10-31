#ifndef SENSORLOG_H
#define SENSORLOG_H

#include <QObject>
#include <QAccelerometer>
#include <QGyroscope>
#include <QCompass>
#include <QGeoPositionInfoSource>
#include <QVector>

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

QT_CHARTS_USE_NAMESPACE

class SensorLog : QObject {
	Q_OBJECT


public:
	SensorLog();
	QLineSeries *accelSeries;
	QChart *chart;

signals:
public slots:
	void readAccel(void);
#if 0
	void readGyro(void);
	void readCompass(void);
	void readGps(const QGeoPositionInfo &info);
#endif
private:
#if 0
	QGyroscope gyro;
	QCompass compass;
	QGeoPositionInfoSource *gps;
#endif
	QAccelerometer accel;
	quint64 startTime;
	double minAccel;
	double maxAccel;


	double timeNow(void);

};

#endif
