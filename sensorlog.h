#ifndef SENSORLOG_H
#define SENSORLOG_H



#include <QObject>

#include <QAccelerometer>
#include <QGyroscope>
#include <QCompass>
#include <QGeoPositionInfoSource>

#include <QSystemBatteryInfo>

QTM_USE_NAMESPACE



class sensorlog : QObject {
	Q_OBJECT

    int fd;

    QSystemBatteryInfo battery;
	QAccelerometer accel;
    QGyroscope gyro;
	QCompass compass;
	QGeoPositionInfoSource *gps;

public:
    sensorlog();
signals:
public slots:
	void readAccel(void);
    void readGyro(void);
	void readCompass(void);
	void readGps(const QGeoPositionInfo &info);
    void readCurrent(int);
};

#endif // SENSORLOG_H
