#ifndef DATARECORD_H
#define DATARECORD_H

#include <QObject>
#include "../vehicleinfo.h"
#include <QSqlDatabase>
#include <QDate>
//Êý¾Ý´æ´¢
class DataRecord : public QObject
{
	Q_OBJECT

public:
	static DataRecord *instance();

	~DataRecord();

	bool VehiclTables(QList<QString> & _return);
	bool failVehtables(QList<QString> & _return);
	bool failStaticstables(QList<QString> & _return);

	bool removeFailVehicleTable(QDate);
	bool removeFailVehicleRecord(QDate,qint32/*key*/);
	qint32 FailVehicleCount(QDate);

	VehicleInfo VehicleRecord(QString);

	qint32 nextVehicleNumber();
protected:
	DataRecord(QObject *parent);
	bool tablesMatch(QRegExp,QList<QString> & _return);

	bool removeTable(QString);
	qint32 TableCount(QString);
	bool removeRecord(QString/*tb*/,qint32/*key*/);
public slots:
	void slot_veh(const VehicleInfo &);
	void slot_fail(const VehicleInfo &);
	void slot_static(const TotoalStaticsInfo &);
	void slot_fail(const TotoalStaticsInfo &);
private:
	static DataRecord *m_ins;

	QSqlDatabase m_dbStore;
};

#endif // DATARECORD_H
