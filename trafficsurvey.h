#ifndef TRAFFICSURVEY_H
#define TRAFFICSURVEY_H

#include <QObject>
#include <QSet>
#include "../vehicleinfo.h"

class TrafficAlg;
class LasterDevUnit;
class TrafficUpload;
class QThread;

class TrafficSurvey : public QObject
{
	Q_OBJECT

public:
	TrafficSurvey(QObject *parent = 0);
	~TrafficSurvey();
signals:
	void signal_veh(const VehicleInfo &);
protected slots:
	void slot_request();
	void slot_veh(const VehicleInfo &);
private:

	TrafficAlg *m_alg;
	QThread *mAlgThread;
	//Vertical
	LasterDevUnit* m_Vdev;

	//slant
	LasterDevUnit* m_Qdev;

	TrafficUpload * m_uploader;
};

#endif // TRAFFICSURVEY_H
