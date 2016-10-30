#include "sensorlog.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

static char *pfix = ",\n";
static int indl = 0;
static quint64 start_time;
static char buf[64*1024];
static int n;

static void
tryflush(int fd)
{
    if(n > sizeof buf - 1024){
        write(fd, buf, n);
        n = 0;
    }
}

static double
timenow(void)
{
    return 1e-3*(QDateTime::currentMSecsSinceEpoch() - start_time);
}

sensorlog::sensorlog()
{
	start_time = QDateTime::currentMSecsSinceEpoch();

    connect(&battery, SIGNAL(currentFlowChanged(int)), this, SLOT(readCurrent(int)));


    mkdir("/home/user/MyDocs/sensorlog", 0777);
    char name[128];
    snprintf(name, sizeof name, "/home/user/MyDocs/sensorlog/sensor%llu.json", start_time);
    fd = creat(name, 0666);

    connect(&accel, SIGNAL(readingChanged()), this, SLOT(readAccel()));
    accel.setProperty("alwaysOn", true);    // alwayson: keep running even when screen blank
    accel.start();


	gps = QGeoPositionInfoSource::createDefaultSource(this);
	connect(gps, SIGNAL(positionUpdated(QGeoPositionInfo)), this, SLOT(readGps(QGeoPositionInfo)));
    gps->setUpdateInterval(1000);
	gps->startUpdates();
/*
*/

        /*
	connect(&compass, SIGNAL(readingChanged()), this, SLOT(readCompass()));
	compass.setProperty("alwaysOn", true);
	compass.start();


	connect(&gyro, SIGNAL(readingChanged()), this, SLOT(readGyro()));
    gyro.setProperty("alwaysOn", true);
	gyro.start();
    */


    n += snprintf(buf + n, sizeof buf - n, "{");
    indl++;
    n += snprintf(buf + n, sizeof buf - n, "\n%*s\"start_time\": %.3f", indl, "", 1e-3*start_time);
}


void
sensorlog::readGyro(void)
{
	QGyroscopeReading *grd;
	grd = gyro.reading();
	if(grd == NULL)
		return;
    //n = snprintf(buf, sizeof buf, "time %.3f gyro x %.2f y %.2f z %.2f\n", timenow(), grd->x(), grd->y(), grd->z());
}


void
sensorlog::readCompass(void)
{
	QCompassReading *crd;

	crd = compass.reading();
	if(crd == NULL)
		return;
   // n += snprintf(buf, sizeof buf, "time %.3f compass azimuth %.0f calib %.2f\n", timenow(), crd->azimuth(), crd->calibrationLevel());
}

void
sensorlog::readAccel(void)
{
	QAccelerometerReading *ard;

	ard = accel.reading();
	if(ard == NULL)
	        return;
    n += snprintf(buf + n, sizeof buf - n, "%s%*s\"accel\": {", pfix, indl, "");
    indl++;
    n += snprintf(buf + n, sizeof buf - n, "\n%*s\"time\": %.3f", indl, "", timenow());
    n += snprintf(buf + n, sizeof buf - n, "%s%*s\"vec\": [%.2f, %.2f, %.2f]", pfix, indl, "", ard->x(), ard->y(), ard->z());
    indl--;
    n += snprintf(buf + n, sizeof buf - n, "\n%*s}", indl, "");
    tryflush(fd);
}

void
sensorlog::readGps(const QGeoPositionInfo &pos)
{
	QGeoCoordinate coord = pos.coordinate();

    n += snprintf(buf + n, sizeof buf - n, "%s%*s\"gps\": {", pfix, indl, "");
    indl++;
    n += snprintf(buf + n, sizeof buf - n, "\n%*s\"time\": %.3f", indl, "", 1e-3*(pos.timestamp().toMSecsSinceEpoch() - start_time));
    n += snprintf(buf + n, sizeof buf - n, "%s%*s\"lat\": %.6f", pfix, indl, "", coord.latitude());
    n += snprintf(buf + n, sizeof buf - n, "%s%*s\"lon\": %.6f", pfix, indl, "", coord.longitude());
    n += snprintf(buf + n, sizeof buf - n, "%s%*s\"ele\": %.1f", pfix, indl, "", coord.altitude());

    if(pos.hasAttribute(QGeoPositionInfo::Direction))
        n += snprintf(buf + n, sizeof buf - n, "%s%*s\"dir\": %.2f", pfix, indl, "", pos.attribute(QGeoPositionInfo::Direction));
    if(pos.hasAttribute(QGeoPositionInfo::GroundSpeed))
        n += snprintf(buf + n, sizeof buf - n, "%s%*s\"hvel\": %.2f", pfix, indl, "", pos.attribute(QGeoPositionInfo::GroundSpeed));
    if(pos.hasAttribute(QGeoPositionInfo::VerticalSpeed))
        n += snprintf(buf + n, sizeof buf - n, "%s%*s\"vvel\": %.2f", pfix, indl, "", pos.attribute(QGeoPositionInfo::VerticalSpeed));
    if(pos.hasAttribute(QGeoPositionInfo::MagneticVariation))
        n += snprintf(buf + n, sizeof buf - n, "%s%*s\"magvar\": %.2f", pfix, indl, "", pos.attribute(QGeoPositionInfo::MagneticVariation));
    if(pos.hasAttribute(QGeoPositionInfo::HorizontalAccuracy))
        n += snprintf(buf + n, sizeof buf - n, "%s%*s\"hprec\": %.2f", pfix, indl, "", pos.attribute(QGeoPositionInfo::HorizontalAccuracy));
    if(pos.hasAttribute(QGeoPositionInfo::VerticalAccuracy))
        n += snprintf(buf + n, sizeof buf - n, "%s%*s\"vprec\": %.2f", pfix, indl, "", pos.attribute(QGeoPositionInfo::VerticalAccuracy));
    indl--;
    n += snprintf(buf + n, sizeof buf - n, "\n%*s}", indl, "");
    tryflush(fd);
}

void
sensorlog::readCurrent(int curflow)
{
	n += snprintf(buf + n, sizeof buf - n, "%s%*s\"battery\": {", pfix, indl, "");
	indl++;
	n += snprintf(buf + n, sizeof buf - n, "\n%*s\"time\": %.3f", indl, "", timenow());
	n += snprintf(buf + n, sizeof buf - n, "%s%*s\"curflow\": %d", pfix, indl, "", curflow);
	n += snprintf(buf + n, sizeof buf - n, "%s%*s\"charge\": %d", pfix, indl, "", battery.remainingCapacity());
	n += snprintf(buf + n, sizeof buf - n, "%s%*s\"capacity\": %d", pfix, indl, "", battery.nominalCapacity());

	indl--;
	n += snprintf(buf + n, sizeof buf - n, "\n%*s}", indl, "");
	tryflush(fd);
}
