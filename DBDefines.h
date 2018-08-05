#ifndef DBDEFINES_H
#define DBDEFINES_H

#include <QObject>

#include "vehicleinfo.h"
 #include <QRegExp>
class QSqlDatabase;

class DBDefines : public QObject
{
public:
	DBDefines(QObject *parent);
	~DBDefines();

	const static QString DBNAME; 
	const static QString VEHTABLE;
	const static QString SATICTABLE;

	const static QString DATETIMEFORMAT;
	const static QString DATEFORMAT;
	static bool createTable(QString,QSqlDatabase db_using);
	static bool updateRecord(QString/*tb*/,const VehicleInfo & nVehInfo,QSqlDatabase db_using);
	static bool updateRecord(QString/*tb*/,const TotoalStaticsInfo & nVehInfo,QSqlDatabase db_using);
	static QSqlDatabase database(QString default_db="./dbstore.db3");

	static bool removeTable(QString,QSqlDatabase db_using);
	static bool removeRecord(QString,qint32,QSqlDatabase db_using);

	static qint32 TableCount(QString,QSqlDatabase db_using);
	static bool TableFirstRecord(QString,QSqlDatabase db_using,VehicleInfo & _return);
	static bool TableFirstRecord(QString,QSqlDatabase db_using,TotoalStaticsInfo & _return);
private:
	
};


#endif // DBDEFINES_H
