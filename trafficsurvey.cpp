#include "trafficsurvey.h"
#include "trafficsettings.h"

#include "alg/trafficalg.h"

#include "lasterdevunit.h"
#include "../net/trafficupload.h"
#include "datarecord.h"
#include <QThread>

TrafficSurvey::TrafficSurvey(QObject *parent)
	: QObject(parent)
{
	mAlgThread = new QThread(this);
	m_alg = new TrafficAlg(0);	
	m_alg->moveToThread(mAlgThread);
	connect(mAlgThread,SIGNAL(destroyed()),m_alg,SLOT(deleteLater()));
	mAlgThread->start();

#ifdef DEV_TEST
	m_Vdev = new LasterDevUnit(TrafficSettings::instance()->vLmsIp(),
							  TrafficSettings::instance()->vLmsPort(),
							  LasterDevUnit::CONTINUE_FRAME,this); //SINGLE_FRAME CONTINUE_FRAME
#else
	m_Vdev = new LasterDevUnit(TrafficSettings::instance()->vLmsIp(),
							  TrafficSettings::instance()->vLmsPort(),
							  LasterDevUnit::CONTINUE_FRAME,this); //SINGLE_FRAME CONTINUE_FRAME
#endif
	
	connect(m_Vdev,SIGNAL(signal_data(const LasterData &)),m_alg,SLOT(slot_vLastData(const LasterData &)));


#ifdef DBG_LASTER
	m_Vdev->stopRequest(true);
#endif

#ifdef DEV_TEST
	m_Qdev = new LasterDevUnit(TrafficSettings::instance()->qLmsIp(),
							   TrafficSettings::instance()->qLmsPort(),
							   LasterDevUnit::CONTINUE_FRAME,this);		
#else
	m_Qdev = new LasterDevUnit(TrafficSettings::instance()->qLmsIp(),
							   TrafficSettings::instance()->qLmsPort(),
							   LasterDevUnit::CONTINUE_FRAME,this);		
#endif
	connect(m_Qdev,SIGNAL(signal_data(const LasterData &)),m_alg,SLOT(slot_qLastData(const LasterData &)));

#ifdef DBG_LASTER
	connect(m_alg,SIGNAL(signal_VehicleDetect(bool)),m_Qdev,SLOT(slot_speedup(bool)));
#endif

	connect(m_alg,SIGNAL(signal_veh(const VehicleInfo &)),this,SLOT(slot_veh(const VehicleInfo &)));
	

	//m_uploader = new TrafficUpload(TrafficSettings::instance()->svrIp(),
	//							   TrafficSettings::instance()->svrPort(),this);
	//connect(this,SIGNAL(signal_veh(const VehicleInfo &)),m_uploader,SLOT(slot_veh(const VehicleInfo &)));

	connect(this,SIGNAL(signal_veh(const VehicleInfo &)),DataRecord::instance(),SLOT(slot_veh(const VehicleInfo &)));
}

TrafficSurvey::~TrafficSurvey()
{

}

void TrafficSurvey::slot_request()
{

}

void TrafficSurvey::slot_veh(const VehicleInfo & nVehInfo)
{
	//数据上传	数据存储
	emit signal_veh(nVehInfo);
}
