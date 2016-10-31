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

	chart.legend()->hide();
	chart.addSeries(accelSeries);
	chart.createDefaultAxes();
	chart.axisX()->setRange(0, 100);
	chart.axisY()->setRange(0, 20);
	chart.setTitle("jiggle meter");

	connect(&accel, SIGNAL(readingChanged()), this, SLOT(readAccel()));
	accel.setAlwaysOn(true);
	accel.start();
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

	double curTime;
	curTime = timeNow();
	*accelSeries << QPointF(curTime, mag);

	if(mag < minAccel || minAccel == maxAccel)
		minAccel = mag;
	if(mag > maxAccel)
		maxAccel = mag;

	chart.axisX()->setRange(0, rint(curTime+0.5));
	chart.axisY()->setRange(rint(minAccel), rint(maxAccel+0.5));
}
