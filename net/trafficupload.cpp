#include "trafficupload.h"


#include "devchannel.h"
#include "trafficprotocol.h"
#include "../config/trafficsettings.h"
#include <QTimer>
#include <QThread>
#include "datarecord.h"
#include "../vehicletetry.h"
//--------------------------------------

#define DISABLE_RETRY

TrafficUpload::TrafficUpload(QString ip,qint32 port,QObject *parent)
	: QObject(parent)
{
	qRegisterMetaType<TotoalStaticsInfo>("TotoalStaticsInfo");

#ifndef DISABLE_RETRY
	//重传部分
	m_VehicleRetry = new VehicleTetry(this);
	//connect(m_VehicleRetry,SIGNAL(signal_fail(const VehicleInfo &)),this,SIGNAL(signal_veh(const VehicleInfo &)));
	connect(m_VehicleRetry,SIGNAL(signal_fail(const VehicleInfo &)),this,SLOT(slot_fail(const VehicleInfo &)));
	connect(m_VehicleRetry,SIGNAL(signal_retry(const VehicleInfo &)),this,SLOT(slot_retrySend(const VehicleInfo &)));
	//连通性发生变化
	connect(this,SIGNAL(signal_link(bool)),m_VehicleRetry,SLOT(slot_recovery(bool)));
#else
	m_VehicleRetry  =NULL;
#endif
	//--------------------------------------------
	if(TrafficSettings::instance()->TransMethod()){
		m_Channel = new DevChannel(ip,port,0); //tcp
	} else{
		m_Channel = new DevChannel("/dev/ttyTX1");//comX 
	}
		
	mChannelThread = new QThread();
	m_Channel->moveToThread(mChannelThread);
	m_Decode = new TrafficProtocol(this);
	m_Decode->setdbg(TrafficSettings::instance()->isdbg());

	connect(this,SIGNAL(signal_start()),m_Channel,SIGNAL(signal_run()));
	connect(this,SIGNAL(signal_send(const QByteArray &)),m_Channel,SLOT(sendData(const QByteArray &)));
	connect(m_Channel,SIGNAL(newData(const QByteArray &)),m_Decode,SLOT(slot_NewFrame(const QByteArray &)));
		
	connect(m_Decode,SIGNAL(signal_cmd02(const QString &,quint8)),this,SLOT(slot_cmd(const QString &,quint8)));
	connect(m_Decode,SIGNAL(signal_cmd03(const CMD_03 &)),this,SLOT(slot_cmd(const CMD_03 &)));
	connect(m_Decode,SIGNAL(signal_cmd04(const CMD_04 &)),this,SLOT(slot_cmd(const CMD_04 &)));
	connect(m_Decode,SIGNAL(signal_cmd05(const CMD_05 &)),this,SLOT(slot_cmd(const CMD_05 &)));
	connect(m_Decode,SIGNAL(signal_cmd06(const CMD_06 &)),this,SLOT(slot_cmd(const CMD_06 &)));
	connect(m_Decode,SIGNAL(signal_cmd07(const CMD_07 &)),this,SLOT(slot_cmd(const CMD_07 &)));
	connect(m_Decode,SIGNAL(signal_cmd0B(const CMD_0B &)),this,SLOT(slot_cmd(const CMD_0B &)));
	connect(m_Decode,SIGNAL(signal_cmd1A(const CMD_1A &)),this,SLOT(slot_cmd(const CMD_1A &)));

	m_Timer = new QTimer(this);
	connect(m_Timer,SIGNAL(timeout()),this,SLOT(slot_statics()));
	slot_cycleChanged(TrafficSettings::instance()->Cycle());
	

	connect(this,SIGNAL(signal_statics(const TotoalStaticsInfo &)),DataRecord::instance(),SLOT(slot_static(const TotoalStaticsInfo &)));
	connect(this,SIGNAL(signal_fail(const TotoalStaticsInfo &)),DataRecord::instance(),SLOT(slot_fail(const TotoalStaticsInfo &)));

	m_BlinkTimer = new QTimer(this);
	//m_BlinkTimer->setSingleShot(true);
	m_BlinkTimer->setInterval(20*1000);
	connect(m_BlinkTimer,SIGNAL(timeout()),this,SLOT(slot_blink()));
	

	mChannelThread->start();
	emit signal_start();

	/*QByteArray test_data;
	test_data.append(0x11);
	test_data.append(0x30);
	test_data.append(0x30);
	test_data.append(0x31);

	test_data.append(0x31);
	test_data.append(0x31);
	test_data.append(0x31);
	test_data.append(0x30);

	test_data.append(0x32);
	test_data.append(0x30);
	test_data.append(0x36);
	test_data.append(0x30);

	test_data.append(0x39);
	test_data.append(0x30);
	test_data.append(0x30);
	test_data.append(0x30);

	test_data.append(0x31);
	test_data.append(char(0x00));
	test_data.append(0xDD);
	test_data.append(0x07);

	test_data.append(0x05);
	test_data.append(0x01);
	test_data.append(0x0D);
	test_data.append(0x21);

		test_data.append(0x07);
	test_data.append(0xE7);
	test_data.append(0x03);
	test_data.append(0x40);

	test_data.append(0xe2);
	test_data.append(0x10);
	test_data.append(0x0b);
	test_data.append(0x03);

	test_data.append(0x96);
	test_data.append(0xbe);
	test_data.append(0xa9);
	test_data.append(0x41);
	
	test_data.append(0x31);
	test_data.append(0x32);
	test_data.append(0x33);
	test_data.append(0x34);

	test_data.append(0x35);
	test_data.append(char(0x00));
	test_data.append(char(0x00));
	test_data.append(char(0x00));

	test_data.append(char(0x00));
	test_data.append(char(0x00));
	test_data.append(char(0x00));
	test_data.append(char(0x00));
	test_data.append(char(0x00));

	QByteArray result_data;
	m_Decode->serialize(test_data,result_data);*/
 //00 00 BD D9 EE EE
}

TrafficUpload::~TrafficUpload()
{

}

void TrafficUpload::slot_cycleChanged(qint32 min_cycle)
{
	m_Timer->stop();
	m_Timer->start(min_cycle*60*1000);
}
void TrafficUpload::slot_veh(const VehicleInfo & nVehInfo)
{	
	 //更新统计信息
	statics_update(&nVehInfo);

	//上传单车信息
	QByteArray out_data;
	m_Decode->serialize_11(&nVehInfo,out_data);
#ifndef DISABLE_RETRY
	m_VehicleRetry->push(nVehInfo);
#endif
	//m_WorKState.insert(RTLS_MOD,nVehInfo.dayNumber);
	emit signal_send(out_data);
}

void TrafficUpload::slot_retrySend(const VehicleInfo & nVehInfo)
{	
	//上传单车信息
	QByteArray out_data;
	m_Decode->serialize_11(&nVehInfo,out_data,false);

	emit signal_send(out_data);
}


void TrafficUpload::slot_cmd(const QString & devID,quint8 state)
{
	if(devID == TrafficSettings::instance()->devId()){
	
		switch(state){
		/*case 0x01:
			break;*/
		case 0x02:			
		case 0x03:
			{
			 //the link is ok			
			}
			break;
		case 0x09:
			answer_cmd08();
			break;
		default:
			qDebug()<<"recv wrong state:"<<state;
			return;
		}

	 emit signal_link(true);
	}
}

void TrafficUpload::slot_cmd(const CMD_03 & cmd_info)
{
	if(QString::fromAscii((char *)cmd_info.RDHead.RD_ID) == TrafficSettings::instance()->devId() &&
		QString::fromAscii((char *)cmd_info.UserName) == TrafficSettings::instance()->user() && 
		QString::fromAscii((char *)cmd_info.KeyNum) == TrafficSettings::instance()->pwd() )
	{
		TrafficSettings::instance()->setrdnum(QString::fromAscii((char *)cmd_info.New_RD_Num));

		answer_cmd08();
	}
}
void TrafficUpload::slot_cmd(const CMD_04 & cmd_info)
{
	if(QString::fromAscii((char *)cmd_info.RDHead.RD_ID) == TrafficSettings::instance()->devId() &&
		QString::fromAscii((char *)cmd_info.UserName) == TrafficSettings::instance()->user() && 
		QString::fromAscii((char *)cmd_info.KeyNum) == TrafficSettings::instance()->pwd() )
	{
		TrafficSettings::instance()->setsvrIp(QString("%1.%2.%3.%4")
											 .arg(cmd_info.New_DSC_Ip[0])
											 .arg(cmd_info.New_DSC_Ip[1])
											 .arg(cmd_info.New_DSC_Ip[2])
											 .arg(cmd_info.New_DSC_Ip[3]));

		answer_cmd08();
	}
}
void TrafficUpload::slot_cmd(const CMD_05 & cmd_info)
{
	if(QString::fromAscii((char *)cmd_info.RDHead.RD_ID) == TrafficSettings::instance()->devId() &&
		QString::fromAscii((char *)cmd_info.UserName) == TrafficSettings::instance()->user() && 
		QString::fromAscii((char *)cmd_info.KeyNum) == TrafficSettings::instance()->pwd() )
	{
		//修改时间

		answer_cmd08();
	}
}

void TrafficUpload::slot_cmd(const CMD_06 & cmd_info)
{
	if(QString::fromAscii((char *)cmd_info.RDHead.RD_ID) == TrafficSettings::instance()->devId() &&
		QString::fromAscii((char *)cmd_info.UserName) == TrafficSettings::instance()->user() && 
		QString::fromAscii((char *)cmd_info.KeyNum) == TrafficSettings::instance()->pwd() )
	{
		TrafficSettings::instance()->setInvContents(cmd_info.New_InvContents);

		answer_cmd08();
	}
}

void TrafficUpload::slot_cmd(const CMD_07 & cmd_info)
{
	if(QString::fromAscii((char *)cmd_info.RDHead.RD_ID) == TrafficSettings::instance()->devId() &&
		QString::fromAscii((char *)cmd_info.UserName) == TrafficSettings::instance()->user() && 
		QString::fromAscii((char *)cmd_info.KeyNum) == TrafficSettings::instance()->pwd() )
	{
		TrafficSettings::instance()->setCycle(cmd_info.New_ProCycle);

		answer_cmd08();
	}
}

void TrafficUpload::slot_cmd(const CMD_0B & cmd_info)
{
	if(QString::fromAscii((char *)cmd_info.RDHead.RD_ID) == TrafficSettings::instance()->devId() &&
		QString::fromAscii((char *)cmd_info.UserName) == TrafficSettings::instance()->user() && 
		QString::fromAscii((char *)cmd_info.KeyNum) == TrafficSettings::instance()->pwd() )
	{
		
		TrafficSettings::instance()->setDisTime(cmd_info.New_DisTime);

		answer_cmd08();
	}
}

void TrafficUpload::slot_cmd(const CMD_1A & cmd_info)
{	
	if(QString::fromAscii((char *)cmd_info.RDHead.RD_ID,16) == TrafficSettings::instance()->devId() )
	{
		int dayid = cmd_info.n3;
			dayid<<=8;
			dayid += cmd_info.n2;
			dayid<<=8;
			dayid += cmd_info.n1;		

			QDate t_date(cmd_info.year,cmd_info.month,cmd_info.day);
#ifndef DISABLE_RETRY
			m_VehicleRetry->success(t_date,dayid);
#endif
		if(cmd_info.code != 00){
			qDebug()<<"0x1A code error:"<<cmd_info.code;
		}
	}
}

void TrafficUpload::answer_cmd08()
{
	QByteArray data;
	m_Decode->serialize_08(TrafficSettings::instance()->getDeviceInfo(),data);

	emit signal_send(data);
}
void TrafficUpload::statics_update(const VehicleInfo *pVeh)
{
	uint8 t_lane = pVeh->u8Lane - 1;
	int32 t_diff_tm = 0;
	uint16 t_FrameCnt = pVeh->Vdata.u16FrameCnt - 1;
	uint32 t_vehPassUsingTm = 0,t_vehEnterTm = 0;
	uint8 t_veh11Buf[50]={0};
	int t_sendlen = 0;
	g_StaticsInfo[t_lane].nLaneID = t_lane + 1; //车道编号
	
	//通车数量自增	
	g_StaticsInfo[t_lane].nVehCnt[pVeh->u16Type - 1]++; 
	g_StaticsInfo[t_lane].nTotalVehCnt++;
	
	g_StaticsInfo[t_lane].nVehSpeed[pVeh->u16Type - 1] += pVeh->speed; //通行车辆的速度和
    g_StaticsInfo[t_lane].nLastVehSpeed = pVeh->speed;
	
  //通行时间和
    if(pVeh->Vdata.tdata[0] > pVeh->Vdata.tdata[t_FrameCnt])
	{
        t_vehPassUsingTm = pVeh->Vdata.tdata[0] - pVeh->Vdata.tdata[t_FrameCnt];
        t_vehEnterTm = pVeh->Vdata.tdata[t_FrameCnt];
	}
	else
	{
        t_vehPassUsingTm = pVeh->Vdata.tdata[t_FrameCnt] - pVeh->Vdata.tdata[0];
        t_vehEnterTm = pVeh->Vdata.tdata[0];
	}
	g_StaticsInfo[t_lane].nVehPassTm += t_vehPassUsingTm;
	
	//跟车判断
	t_diff_tm = t_vehEnterTm - g_StaticsInfo[t_lane].nLastVehEnterTm ;
	if(t_diff_tm < TrafficSettings::instance()->DisTime()*1000 && pVeh->u16Type != TYPE_MO_TUO)
	{
		//认为是跟车了
		g_StaticsInfo[t_lane].nGenCheCnt++;
	}		
	//更新上一辆车进入断面时间
	g_StaticsInfo[t_lane].nLastVehEnterTm = t_vehEnterTm;
	
	//更新车头间距
	g_StaticsInfo[t_lane].nDisheadVeh += t_diff_tm *pVeh->speed/3600;	
}

void TrafficUpload::slot_statics()
{
	QByteArray data;
	TotoalStaticsInfo t_statics;
	m_Decode->serialize_01(g_StaticsInfo,TrafficSettings::instance()->getDeviceInfo(),data,t_statics);
	
	emit signal_statics(t_statics);

	resetStatics();
}

void TrafficUpload::resetStatics()
{
	for(int i=0;i<TrafficSettings::instance()->LaneNum();i++)
	{
		g_StaticsInfo[i].nTotalVehCnt = 0;
		g_StaticsInfo[i].nGenCheCnt = 0;
		g_StaticsInfo[i].nDisheadVeh = 0xffff;
		g_StaticsInfo[i].nVehPassTm = 0;
		for(int j=0;j<MAX_VEH_TYPE;j++)
		{
			g_StaticsInfo[i].nVehSpeed[j] = 0;
			g_StaticsInfo[i].nVehCnt[j] = 0;
		}			
	}
}

void TrafficUpload::slot_fail(const VehicleInfo &)
{
	if(!m_BlinkTimer->isActive()){
		m_BlinkTimer->start();
	}	
}

void TrafficUpload::slot_blink()
{
	QByteArray data;
	m_Decode->serialize_02(TrafficSettings::instance()->getDeviceInfo(),data);

	emit signal_send(data);
}


