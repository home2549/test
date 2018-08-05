#include "vehicleinfo.h"
#include "datarecord.h"

qint32 VehicleInfo::m_totalCount = 0;

VehicleInfo::VehicleInfo()
	:Vdata(VehicleDataStruct()),Incdata(VehicleDataStruct())
{
	u16Type = TYPE_UNKNOWN;

	u8Vstate = u8Istate = NO_USED;
    u8Lane = 0;
    yLen=ZLen=xLen = 0;   
    speed=VemptFrame=IemptFrame = 0;	
	devID="";
	dayNumber = 0;
	datetime = QDateTime::currentDateTime();

	
	direct = DIRECT_NO;

}

VehicleInfo::~VehicleInfo()
{

}

VehicleInfo  & VehicleInfo::operator=(const VehicleInfo & other)
{
	u16Type = other.u16Type;

	u8Vstate = other.u8Vstate;
	u8Istate = other.u8Istate;
	u8Lane =other.u8Lane;
	yLen= other.yLen;
	ZLen= other.ZLen;
	xLen = other.xLen;   
	speed= other.speed;
	VemptFrame= other.VemptFrame;
	IemptFrame = other.IemptFrame;	
	devID=other.devID;
	dayNumber = other.dayNumber;
	datetime = other.datetime;

	direct = other.direct;

	Vdata = other.Vdata;
	Incdata = other.Incdata;

	return *this;
}

quint16 VehicleInfo::getFrame_v()
{
	return Vdata.u16FrameCnt;
}

void VehicleInfo::incFrame_v()
{
	if(Vdata.u16FrameCnt < FRAME_MAXCNT){
		Vdata.u16FrameCnt++;
	}
}

quint16 VehicleInfo::getFrame_q()
{
	return Incdata.u16FrameCnt;
}

void VehicleInfo::incFrame_q()
{
	if(Incdata.u16FrameCnt < FRAME_MAXCNT){
		Incdata.u16FrameCnt++;
	}
}

qint32 VehicleInfo::Count()
{
	m_totalCount++;
	return m_totalCount;
}

void VehicleInfo::InitStatic()
{
	m_totalCount = DataRecord::instance()->nextVehicleNumber();
}

bool VehicleInfo::operator==(const VehicleInfo & other)
{
	return dayNumber == other.dayNumber;
}

QString VehicleInfo::showType() const
{
	switch(u16Type){
	case TYPE_XIAO_KE:
		return QString::fromLocal8Bit("С�ͳ�"); 
		break;
	case TYPE_DA_KE:
		return QString::fromLocal8Bit("��ͳ�"); 
		break;
	case TYPE_XIAO_HUO:
		return QString::fromLocal8Bit("С����"); 
		break;
	case TYPE_ZHONG_HUO:
		return QString::fromLocal8Bit("���ͻ���"); 
	case TYPE_DA_HUO:
		return QString::fromLocal8Bit("���ͻ���"); 
	case TYPE_TE_DA_HUO:
		return QString::fromLocal8Bit("�ش��ͻ���"); 
		break;
	case TYPE_JI_ZHUANG_XIANG:
		return QString::fromLocal8Bit("��װ�䳵"); 
		break;
	case TYPE_TUO_TA_JI:
		return QString::fromLocal8Bit("������"); 
		break;
	case TYPE_MO_TUO:
		return QString::fromLocal8Bit("Ħ�г�"); 
		break;
	default:
		return QString::fromLocal8Bit("δ֪"); 
	}
}

VehStaticsInfo::VehStaticsInfo()	
{
	dayNumber = 0;
	LANENUM = 0;
	GENCHEBI = 0;
	AVGDIS = 0;
	TIMEPERCENT = 0;	
	m_VehTypes.clear();
}

VehStaticsInfo::VehStaticsInfo(const VehStaticsInfo & other)
{	
	m_VehTypes = other.m_VehTypes;
	LANENUM =  other.LANENUM;
	GENCHEBI =  other.GENCHEBI;
	AVGDIS =  other.AVGDIS;
	TIMEPERCENT =  other.TIMEPERCENT;
	dayNumber =  other.dayNumber;
}

VehStaticsInfo::~VehStaticsInfo()
{


}