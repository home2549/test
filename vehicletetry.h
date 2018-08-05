#ifndef VEHICLETETRY_H
#define VEHICLETETRY_H

#include <QObject>
#include "datatetry.h"
#include "vehicleinfo.h"
#include <QDate>
#include <QTimer>

class VehicleTetry : public DataTetry<VehicleInfo>
{
	Q_OBJECT

public:
	VehicleTetry(QObject *parent = 0);
	~VehicleTetry();

	virtual void tryagin();
	virtual void fail();
	//³É¹¦
	virtual void success(QDate,qint32);

	virtual void setShedule(bool);
public slots:
	void slot_recovery(bool);
signals:
	void signal_fail(const VehicleInfo &);
	void signal_retry(const VehicleInfo &);
protected:
	void nextTb();
	void nextRecord(QString);

protected slots:
	void slot_schedule();
private:
	QList<QString> m_faultTbs;

	QTimer *m_tryTimer;
};

#endif // VEHICLETETRY_H
