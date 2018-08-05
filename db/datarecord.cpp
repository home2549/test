#include "datarecord.h"
#include <QDebug>
#include <QThread>
#include <QStringList>
#include "DBDefines.h"
#include <QSqlError>
#include <QFileInfo>
#include <QDate>
#include "trafficsettings.h"
DataRecord *DataRecord::m_ins = NULL;
DataRecord::DataRecord(QObject *parent)
	: QObject(parent)
{
	QFileInfo path(TrafficSettings::instance()->storepath());
	qDebug()<<path.absoluteFilePath();

	m_dbStore = DBDefines::database(TrafficSettings::instance()->storepath());	

	//test for saving
/*	 VehicleInfo  nVehInfo;
	 for(int i = 0;i<9;i++){
		 nVehInfo.dayNumber = 100+i+5;
		 nVehInfo.u16Type = TYPE_XIAO_KE+i;
		 nVehInfo.speed = 130;
		 nVehInfo.xLen = 1800;
		 nVehInfo.yLen = 4500;
		 nVehInfo.ZLen = 1600;
		 nVehInfo.u8Lane = 0x01;

		 slot_fail(nVehInfo);		 
	 }*/	
}

DataRecord::~DataRecord()
{

}

DataRecord *DataRecord::instance()
{
	if(!m_ins){
		m_ins = new DataRecord(0);
	}
	return m_ins;
}

void DataRecord::slot_veh(const VehicleInfo & nVehInfo)
{
	static QMap<QString,qint32> nTest;

	if(!nTest.contains(nVehInfo.showType())){
		nTest.insert(nVehInfo.showType(),0);
	}
	nTest[nVehInfo.showType()]++;
	//db savinng 
	qDebug()<<nVehInfo.showType() 
			 +" S:" + QString::number(nVehInfo.speed)
			 +" W:" + QString::number(nVehInfo.xLen)
			 +" L:" + QString::number(nVehInfo.yLen)
			 +" Z:" + QString::number(nVehInfo.ZLen)
			 +" Lane:"+QString::number(nVehInfo.u8Lane)
			 <<"====>"<<nTest[nVehInfo.showType()];

	QDate t_date = QDate::currentDate();
	QString tb_name = QString("vehicle_%1").arg(t_date.toString("yyyy_MM_dd"));
	if(m_dbStore.isValid()){
	
		QStringList tb_lst = m_dbStore.tables(QSql::Tables);
		if(!tb_lst.contains(tb_name)){
		
			bool ret = DBDefines::createTable((DBDefines::VEHTABLE).arg(tb_name),m_dbStore);
			if(ret){
				return;
			}
		}
		DBDefines::updateRecord(tb_name,nVehInfo,m_dbStore);
	}
}

void DataRecord::slot_fail(const VehicleInfo & nVehInfo)
{
	QDate t_date = QDate::currentDate();
	QString tb_name = QString("vehFail_%1").arg(t_date.toString("yyyy_MM_dd"));
	if(m_dbStore.isValid()){
	
		QStringList tb_lst = m_dbStore.tables(QSql::Tables);
		if(!tb_lst.contains(tb_name)){
		
			bool ret = DBDefines::createTable((DBDefines::VEHTABLE).arg(tb_name),m_dbStore);
			if(ret){
				return;
			}
		}
		DBDefines::updateRecord(tb_name,nVehInfo,m_dbStore);
	}

}

void DataRecord::slot_static(const TotoalStaticsInfo &)
{

}

void DataRecord::slot_fail(const TotoalStaticsInfo & nStaticsInfo)
{
	QDate t_date = QDate::currentDate();
	QString tb_name = QString("staticsfail_%1").arg(t_date.toString("yyyy_MM_dd"));
	if(m_dbStore.isValid()){
	
		QStringList tb_lst = m_dbStore.tables(QSql::Tables);
		if(!tb_lst.contains(tb_name)){
		
			bool ret = DBDefines::createTable((DBDefines::SATICTABLE).arg(tb_name),m_dbStore);
			if(ret){
				return;
			}
		}
		DBDefines::updateRecord(tb_name,nStaticsInfo,m_dbStore);
	}
}

bool DataRecord::VehiclTables(QList<QString> & _return)
{
	return tablesMatch(QRegExp("vehicle",Qt::CaseInsensitive),_return);
}
bool DataRecord::failVehtables(QList<QString> & _return)
{
	return tablesMatch(QRegExp("vehFail",Qt::CaseInsensitive),_return);
}

bool DataRecord::failStaticstables(QList<QString> & _return)
{
	return tablesMatch(QRegExp("staticsfail",Qt::CaseInsensitive),_return);
}

bool DataRecord::tablesMatch(QRegExp regexp,QList<QString> & _return)
{
	if(m_dbStore.isValid()){
	
		QStringList tb_list = m_dbStore.tables(QSql::Tables);
	
		_return = tb_list.filter(regexp);

		return true;
	}
	return false;
}

bool DataRecord::removeFailVehicleTable(QDate nDate)
{
	return removeTable(QString("vehFail_%1").arg(nDate.toString("yyyy_MM_dd")));
}
bool DataRecord::removeFailVehicleRecord(QDate t_date ,qint32 nKey)
{
	bool ret = removeRecord(QString("vehFail_%1").arg(t_date.toString("yyyy_MM_dd")),nKey);	
	return false; 
}

qint32 DataRecord::FailVehicleCount(QDate t_date)
{
	return TableCount(QString("vehFail_%1").arg(t_date.toString("yyyy_MM_dd")));
}

bool DataRecord::removeTable(QString tb_name)
{
	return DBDefines::removeTable(tb_name,m_dbStore);
}

bool DataRecord::removeRecord(QString tb_name,qint32 t_id)
{
	return DBDefines::removeRecord(tb_name,t_id,m_dbStore);
}

qint32 DataRecord::nextVehicleNumber()
{
	QDate t_date = QDate::currentDate();
	QString tb_name = QString("vehicle_%1").arg(t_date.toString("yyyy_MM_dd"));
	return TableCount(tb_name);
}

qint32 DataRecord::TableCount(QString tb_name)
{
	return DBDefines::TableCount(tb_name,m_dbStore);
}

VehicleInfo DataRecord::VehicleRecord(QString tb_name)
{
	VehicleInfo _ret;
	DBDefines::TableFirstRecord(tb_name,m_dbStore,_ret);
	return _ret;
}