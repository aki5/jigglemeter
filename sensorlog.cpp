#include "sensorlog.h"
#include <stdio.h>
#include <QDebug>

double
SensorLog::timeNow(void)
{
	return 1e-3*(QDateTime::currentMSecsSinceEpoch() - startTime);
}

SensorLog::SensorLog()
{
	startTime = QDateTime::currentMSecsSinceEpoch();
	accelSeries = new QLineSeries();
	accelSeries->setUseOpenGL(true);


	chart = new QChart();
	chart->legend()->hide();
	chart->addSeries(accelSeries);
	chart->createDefaultAxes();
	chart->axisX()->setRange(0,100);
	chart->axisY()->setRange(0,20);
	chart->setTitle("jiggle meter");

	connect(&accel, SIGNAL(readingChanged()), this, SLOT(readAccel()));
	accel.setProperty("alwaysOn", true);
	accel.start();

#if 0
	gps = QGeoPositionInfoSource::createDefaultSource(this);
	connect(gps, SIGNAL(positionUpdated(QGeoPositionInfo)), this, SLOT(readGps(QGeoPositionInfo)));
	gps->setUpdateInterval(1000);
	gps->startUpdates();

	connect(&compass, SIGNAL(readingChanged()), this, SLOT(readCompass()));
	compass.setProperty("alwaysOn", true);
	compass.start();

	connect(&gyro, SIGNAL(readingChanged()), this, SLOT(readGyro()));
	gyro.setProperty("alwaysOn", true);
	gyro.start();
#endif
	qDebug() << "startTime:" << 1e-3*startTime;

}

void
SensorLog::readAccel(void)
{
	QAccelerometerReading *rd;

	rd = accel.reading();
	if(rd == NULL)
			return;

	double x, y, z, mag;
	x = rd->x();
	y = rd->y();
	z = rd->z();
	mag = sqrt(x*x + y*y + z*z);

	double time;
	time = timeNow();
	*accelSeries << QPointF(time, mag);

	if(mag < minAccel)
		minAccel = mag;
	if(mag > maxAccel)
		maxAccel = mag;

	chart->axisX()->setRange(0,time);
	chart->axisY()->setRange(minAccel, maxAccel);

	//qDebug() << "accel: x:" << x << " y:" << y <<" z:" << z << " mag:" << mag;
}

#if 0
void
SensorLog::readGyro(void)
{
	QGyroscopeReading *rd;

	rd = gyro.reading();
	if(rd == NULL)
		return;

	double x, y, z, mag;
	x = rd->x();
	y = rd->y();
	z = rd->z();
	mag = sqrt(x*x + y*y + z*z);
	qDebug() << "gyro: x:" << x << " y:" << y <<" z:" << z << " mag:" << mag;
}

void
SensorLog::readCompass(void)
{
	QCompassReading *rd;

	rd = compass.reading();
	if(rd == NULL)
		return;

	double azim, calib;
	azim = rd->azimuth();
	calib = rd->calibrationLevel();
	qDebug() << "compass: azimuth: " << azim << "calibrationLevel: " << calib;

}


void
SensorLog::readGps(const QGeoPositionInfo &pos)
{
	QGeoCoordinate rd = pos.coordinate();

	double lat, lon, alt;
	lat = rd.latitude();
	lon = rd.longitude();
	alt = rd.altitude();

	qDebug() << "gps: latitude: " << lat << " longitude:" << lon << " altitude:" << alt;
}
#endif
