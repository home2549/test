#include "vehicletetry.h"
#include "datarecord.h"
#include <QDebug>
VehicleTetry::VehicleTetry(QObject *parent)
	: DataTetry<VehicleInfo>(parent)
{
	connect(this,SIGNAL(signal_fail(const VehicleInfo &)),DataRecord::instance(),SLOT(slot_fail(const VehicleInfo &)));

	m_tryTimer = new QTimer(this);
	//m_tryTimer->setSingleShot(true);
	connect(m_tryTimer,SIGNAL(timeout()),this,SLOT(slot_schedule()));
}

VehicleTetry::~VehicleTetry()
{

}

void VehicleTetry::tryagin()
{
	qDebug()<<"try resend";
	VehicleInfo t_Vehinfo = tryData();

	emit signal_retry(t_Vehinfo);
}
void VehicleTetry::fail()
{
	qDebug()<<"resend fail";

	VehicleInfo t_Vehinfo = failData();
	//save to db
	emit signal_fail(t_Vehinfo);	
}
//成功
void VehicleTetry::success(QDate nDate,qint32 nDayNumber)
{
	QListIterator<VehicleInfo> iEveryItem(m_Caches);
	while(iEveryItem.hasNext()){
	
		const VehicleInfo & ref_item = iEveryItem.next();
		if(ref_item.dayNumber == nDayNumber){		
			remove(ref_item);

			DataRecord::instance()->removeFailVehicleRecord(nDate,nDayNumber);

			if(!DataRecord::instance()->FailVehicleCount(nDate)){
				DataRecord::instance()->removeFailVehicleTable(nDate);
				if(m_faultTbs.size()){
					m_faultTbs.removeFirst();
					nextTb();
				}				
			}else{
				nextTb();
			}
			break;
		}
	}
}

void VehicleTetry::slot_recovery(bool state)
{
	if(state){	
		if(!m_faultTbs.size()){
			//获取单车重传数据
			DataRecord::instance()->failVehtables(m_faultTbs);
		}
	
		if(!m_faultTbs.size()){
			//获取统计重传数据
			DataRecord::instance()->failStaticstables(m_faultTbs);
		}

		nextTb();		
	}
}

void VehicleTetry::nextTb()
{
	if(m_faultTbs.size()){		
		nextRecord(m_faultTbs.first());
		m_tryTimer->start(20);
	}
}

void VehicleTetry::nextRecord(QString tb_name)
{
	this->push(DataRecord::instance()->VehicleRecord(tb_name));
}
void VehicleTetry::slot_schedule()
{
	sheduled();
}

void VehicleTetry::setShedule(bool enabled)
{
	if(enabled){
		m_tryTimer->start(20*1000);
	}else{
		m_tryTimer->stop();	
	}
}