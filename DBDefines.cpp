#include "DBDefines.h"
#include <QSqlQuery>
#include <QThread>
#include <QSqlError>
#include <QStringList>
#include <QDebug>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QDateTime>
#include <QSqlDatabase>
#include "trafficsettings.h"
const QString DBDefines::DBNAME = "dbstore.db3";
const QString DBDefines::VEHTABLE = "CREATE TABLE %1(ID BIGINT PRIMARY KEY,	\
											  DATETIME VARCHAR(255), \
											  LANE INT,	\
											  TYPE INT,	\
											  SPEED INT,	\
											  WIDTH INT,	\
											  HEIGHT INT,	\
											  LENGTH INT,	\
											  CARD	VACHAR(255))";

const QString DBDefines::SATICTABLE = "CREATE TABLE %1(ID INT,	\
												 DATETIME VARCHAR(255),	\
												 CYCLE	INT,	\
												 LANE	INT,	\
												 LANENUM	INT,	\
												 GENCHEBI	INT,	\
												 AVGDIS		INT,	\
												 TIMEPERCENT INT,	\
												 XIAOKESUM	INT,	\
												 XIAOKEAVGSPEED INT, \
												 XIAOHUOSUM	INT,	\
												 XIAOHUOAVGSPEED INT,	\
												 DAKESUM INT,	\
												 DAKEAVGSPEED INT,	\
												 ZHONGHUOSUM INT,\
												 ZHOUHUOAVGSPPED INT,	\
												 DAHUOSUM	INT,	\
												 DAHUOAVGSPEED INT,	\
												 TEDAHUOSUM	INT,	\
												 TEDAHUOAVGSPEED INT,	\
												 JIZHUXIANGSUM	INT,	\
												 JIZHUXIANGAVGSPEED INT,	\
												 TUOLAJISUM	INT,\
												 TUOLAJIAVGSPEED INT,\
												 MOTUOSUM	INT, \
												 MOTUOAVGSPEED INT,	\
												 PRIMARY KEY(ID,LANENUM))";

const  QString DBDefines::DATETIMEFORMAT = "yyyy-MM-dd hh:mm:ss:zzz";
const  QString DBDefines::DATEFORMAT;

DBDefines::DBDefines(QObject *parent)
	: QObject(parent)
{

}

DBDefines::~DBDefines()
{

}

QSqlDatabase DBDefines::database(QString default_db)
{
	 QSqlDatabase ret_db;
	 QStringList t_drivers = QSqlDatabase::drivers();
	if(t_drivers.contains("QSQLITE")){
		QString t_id = QString("%1").arg((qlonglong)QThread::currentThread());
		if(QSqlDatabase::contains(t_id)){
			ret_db = QSqlDatabase::database(t_id);	
		}else{
			ret_db = QSqlDatabase::addDatabase("QSQLITE",t_id);
			ret_db.setDatabaseName(default_db);
			
			bool bSuccess = ret_db.open();
			if(bSuccess){
				qDebug()<<DBDefines::DBNAME + " open success";	
			}else{
				qDebug()<<DBDefines::DBNAME + " open failure";
			}
		}	
	}else{
		qDebug()<<t_drivers;	
	}

	return ret_db;
}

bool DBDefines::createTable(QString tb_defines,QSqlDatabase db_using)
{
	QString t_id = QString("%1").arg((qlonglong)QThread::currentThread());
	if(db_using.isValid())
	{
		QSqlQuery query(db_using);
		bool ret =  query.exec(tb_defines);
		if(!ret){
			qDebug()<<query.lastError().text();
		}
		return ret;
	}	
	return false;
}

bool DBDefines::updateRecord(QString tb_name,const VehicleInfo & nVehInfo,QSqlDatabase db_using)
{
	QString t_id = QString("%1").arg((qlonglong)QThread::currentThread());
	if(db_using.isValid())
	{
		QSqlTableModel tb_model(0,db_using);
		tb_model.setTable(tb_name);

		tb_model.setFilter(QString("ID=%1").arg(nVehInfo.dayNumber));
		tb_model.select();
		if(!tb_model.rowCount()){
			qDebug()<<"not find";
		}else{
			qDebug()<<"vehicle find";
			//tb_model.clear();
			tb_model.removeRow(0);
		}

		QSqlRecord nItem = tb_model.record();

		nItem.setValue("ID",nVehInfo.dayNumber);
		nItem.setValue("DATETIME",QDateTime::currentDateTime().toString(DBDefines::DATETIMEFORMAT));
		nItem.setValue("LANE",nVehInfo.u8Lane);
		nItem.setValue("TYPE",nVehInfo.u16Type);
		nItem.setValue("SPEED",nVehInfo.speed);
		nItem.setValue("WIDTH",nVehInfo.xLen);
		nItem.setValue("HEIGHT",nVehInfo.ZLen);
		nItem.setValue("LENGTH",nVehInfo.yLen);
		nItem.setValue("CARD","");		
		bool ret = tb_model.insertRecord(-1,nItem);	
		if(!ret){
			qDebug()<<tb_model.lastError().text();
		}
		 return tb_model.submit();
		
	}	
	return false;

}

bool DBDefines::updateRecord(QString tb_name,const TotoalStaticsInfo & nVehStaticsInfo,QSqlDatabase db_using)
{
	QString t_id = QString("%1").arg((qlonglong)QThread::currentThread());
	if(db_using.isValid())
	{

		QSqlTableModel tb_model(0,db_using);
		tb_model.setTable(tb_name);

		tb_model.setFilter(QString("ID=%1").arg(nVehStaticsInfo.dayNumber));
		tb_model.select();
		if(!tb_model.rowCount()){
			qDebug()<<"not find";
		}else{
			qDebug()<<"vehicle find";
			//tb_model.clear();
			tb_model.removeRows(0,tb_model.rowCount());
		}

		const DeviceSettings & nSettings = TrafficSettings::instance()->getDeviceInfo();
		qint32 t_total = nVehStaticsInfo.m_LaneStatisc.size();

		QDateTime cur_tm = QDateTime::currentDateTime();
		QListIterator<VehStaticsInfo> iEveryItem(nVehStaticsInfo.m_LaneStatisc);
		while(iEveryItem.hasNext())
		{
			const VehStaticsInfo & ref_item = iEveryItem.next();

			QSqlRecord nItem = tb_model.record();

			nItem.setValue("ID",ref_item.dayNumber);
			nItem.setValue("DATETIME",cur_tm.toString(DBDefines::DATETIMEFORMAT));
		
			nItem.setValue("CYCLE",nSettings.Cycle());
			nItem.setValue("LANE",nSettings.LaneNum());
			nItem.setValue("LANENUM",ref_item.LANENUM);
			nItem.setValue("GENCHEBI",ref_item.GENCHEBI);
			nItem.setValue("AVGDIS",ref_item.AVGDIS);
			nItem.setValue("TIMEPERCENT",ref_item.TIMEPERCENT);
			qint32 t_devType = TYPE_XIAO_KE;
			nItem.setValue("XIAOKESUM",ref_item.m_VehTypes[t_devType].m_sum);	
			nItem.setValue("XIAOKEAVGSPEED",ref_item.m_VehTypes[t_devType++].m_avgSpeed);

			nItem.setValue("DAKESUM",ref_item.m_VehTypes[t_devType].m_sum);
			nItem.setValue("DAKEAVGSPEED",ref_item.m_VehTypes[t_devType++].m_avgSpeed);	

			nItem.setValue("XIAOHUOSUM",ref_item.m_VehTypes[t_devType].m_sum);
			nItem.setValue("XIAOHUOAVGSPEED",ref_item.m_VehTypes[t_devType++].m_avgSpeed);			

			nItem.setValue("ZHONGHUOSUM",ref_item.m_VehTypes[t_devType].m_sum);	
			nItem.setValue("ZHOUHUOAVGSPPED",ref_item.m_VehTypes[t_devType++].m_avgSpeed);

			nItem.setValue("DAHUOSUM",ref_item.m_VehTypes[t_devType].m_sum);
			nItem.setValue("DAHUOAVGSPEED",ref_item.m_VehTypes[t_devType++].m_avgSpeed);

			nItem.setValue("TEDAHUOSUM",ref_item.m_VehTypes[t_devType].m_sum);
			nItem.setValue("TEDAHUOAVGSPEED",ref_item.m_VehTypes[t_devType++].m_avgSpeed);	
		
			nItem.setValue("JIZHUXIANGSUM",ref_item.m_VehTypes[t_devType].m_sum);
			nItem.setValue("JIZHUXIANGAVGSPEED",ref_item.m_VehTypes[t_devType++].m_avgSpeed);

			nItem.setValue("TUOLAJISUM",ref_item.m_VehTypes[t_devType].m_sum);
			nItem.setValue("TUOLAJIAVGSPEED",ref_item.m_VehTypes[t_devType++].m_avgSpeed);

			nItem.setValue("MOTUOSUM",ref_item.m_VehTypes[t_devType].m_sum);	
			nItem.setValue("MOTUOAVGSPEED",ref_item.m_VehTypes[t_devType++].m_avgSpeed);	
			
			bool ret = tb_model.insertRecord(-1,nItem);	
			if(!ret){
				qDebug()<<tb_model.lastError().text();
			}
		}
	
		 return tb_model.submit();	
	}	
	return false;

}

bool DBDefines::removeTable(QString tb_name,QSqlDatabase db_using)
{
	QSqlQuery query(db_using);

	return query.exec(QString("DROP TABLE IF EXISTS %1")
								.arg(tb_name));
}

bool DBDefines::removeRecord(QString tb_name ,qint32 tb_key ,QSqlDatabase db_using)
{
	QSqlQuery query(db_using);

	return query.exec(QString("DELETE FROM %1 WHERE ID=%2")
								.arg(tb_name)
								.arg(tb_key));
}

qint32 DBDefines::TableCount(QString tb_name,QSqlDatabase db_using)
{
	 QSqlQuery query(db_using);

	 bool ret = query.exec(QString("SELECT COUNT(*) FROM %1")
								.arg(tb_name));
	 if(ret && query.next()){
		return query.value(0).toInt();
	 }

	return 0;
}

bool DBDefines::TableFirstRecord(QString tb_name,QSqlDatabase db_using,VehicleInfo & nVehInfo)
{
	QSqlQueryModel tb_model(0);
	//tb_model.setTable(tb_name);
	tb_model.setQuery(QString("SELECT * FROM %1 ORDER BY ID ASC LIMIT 1").arg(tb_name),db_using);
	if(tb_model.rowCount()){

		QSqlRecord nItem  = tb_model.record(0);
		
		nVehInfo.dayNumber = nItem.value("ID").toInt();
		nVehInfo.datetime =  QDateTime::fromString(nItem.value("DATETIME").toString(),(DBDefines::DATETIMEFORMAT));
		nVehInfo.u8Lane = nItem.value("LANE").toInt();
		nVehInfo.u16Type = nItem.value("TYPE").toInt();
		nVehInfo.speed = nItem.value("SPEED").toInt();
		nVehInfo.xLen = nItem.value("WIDTH").toReal();
		nVehInfo.ZLen = nItem.value("HEIGHT").toReal();
		nVehInfo.yLen = nItem.value("LENGTH").toReal();

		return true;
	}
	return false;
}

bool DBDefines::TableFirstRecord(QString,QSqlDatabase db_using,TotoalStaticsInfo & _return)
{
	return false;
}