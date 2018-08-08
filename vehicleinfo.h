#ifndef VEHICLEINFO_H
#define VEHICLEINFO_H

#include <QObject>
#include <QMap>
#include <QDateTime>
#include "../head_struct.h"
//typedef struct tagPointStruct
//{
//    qint32  u16xLeft;  //左侧位置
//    qint32  u16xRight; //右侧位置 //wzf
//    quint16  u16xMaxHt; //最大值Z
//
//    quint16   u16Leftpt; //左侧位置点
//    quint16   u16Rightpt; //右侧位置点
//    quint16  u16xDis;
//}PointStruct;

#define FRAME_MASK	   0x7F
#define FRAME_MAXCNT   0x7F //128
#define FRAME_BUFLEN   0x7F //128

class VehicleDataStruct{
public:
      quint16  u16EmptFrame; //垂直空白帧数
      PointStruct location; //车的左右位置

	 quint16 u16FrameCnt; //总帧数
     qint32  xdata[FRAME_MAXCNT];
     qint32  zdata[FRAME_MAXCNT][FRAME_BUFLEN];
     qint32  rdata[FRAME_MAXCNT][FRAME_BUFLEN];       
     qint32  zMax[FRAME_MAXCNT];
     qint64  tdata[FRAME_MAXCNT]; //时间
  

	 VehicleDataStruct()
         :location(PointStruct())
	 {
		u16FrameCnt = u16EmptFrame = 0;
		//memset(ydata,0,sizeof(qint32)*FRAME_MAXCNT);
		memset(xdata,0,sizeof(qint32)*FRAME_MAXCNT);
		memset(zMax,0,sizeof(qint32)*FRAME_MAXCNT);
		memset(tdata,0,sizeof(qint64)*FRAME_MAXCNT);
              
		qint32 nIndex = 0;
		while(nIndex < FRAME_MAXCNT){
			memset(zdata[nIndex++],0,sizeof(qint32)*FRAME_BUFLEN);
            memset(rdata[nIndex++],0,sizeof(qint32)*FRAME_BUFLEN);
		}
	 }
};


class VehicleInfo
{
public:

	typedef enum{
		NO_USED	 = 0x00,  //无效
		OCCURING_USED = 0x01,  //正在进行时
		PASSED_USED	 = 0x02  //已驶过
	}STATA_;

	VehicleInfo();//qint32 nType,qreal nSpeed,qreal nHeight,qreal nLength,qint32 nlane
	~VehicleInfo();

	VehicleInfo(const VehicleInfo & other){
		if(this != &other){
			*this = other;
		}
	}

	VehicleInfo  & operator=(const VehicleInfo & other);

	quint16 getFrame_v();
	void incFrame_v();

	quint16 getFrame_q();
	void incFrame_q();

	static qint32 Count();
	static void InitStatic();


public:

	bool operator==(const VehicleInfo & other);

	QString showType() const;
	
	qint32 u16Type;


    STATA_ u8Vstate; //有效标志 NO_USED,OCCURING_USED，VehicleInfo::PASSED_USED
    STATA_ u8Istate;

    qint32 u8Lane; //车道
    qreal  yLen;  //车身长度
    qreal  ZLen;  //车身的高度
    qreal  xLen;   //车身的宽度
    qint32  speed;
	   

    //qint32  VemptFrame; //垂直空白帧数
    //PointStruct locateX_V; //车的左右位置
    VehicleDataStruct  Vdata;  //垂直

    //qint32  IemptFrame;	//倾斜空白帧数
    //PointStruct locateX_I; //车的左右位置
    VehicleDataStruct  Incdata;	//倾斜

	//车辆首次进入方向
	enum{
		DIRECT_NO,
		DIRECT_V, //垂直
		DIRECT_Q //倾斜
	};
	quint8 direct;

	QString devID;
	qint32  dayNumber;
	QDateTime datetime;

	static qint32 m_totalCount;
};

class simpleStaticsInfo{
public:
	simpleStaticsInfo(){
		m_sum = 0;
		m_avgSpeed = 0;
	}
	simpleStaticsInfo(const simpleStaticsInfo & other){
		m_sum = other.m_sum;
		m_avgSpeed = other.m_avgSpeed;
	}

	qint32 m_sum;
	qint32 m_avgSpeed;
};

class VehStaticsInfo{

public:
	VehStaticsInfo();
	VehStaticsInfo(const VehStaticsInfo & other);
	virtual ~VehStaticsInfo();

	qint32  dayNumber;
	qint32  LANENUM;
	qint32	GENCHEBI;
	qint32	AVGDIS;
	qint32	TIMEPERCENT;	
	QMap<qint32,simpleStaticsInfo> m_VehTypes;
};

class TotoalStaticsInfo{
public:
	TotoalStaticsInfo(){
		m_LaneStatisc.clear();
		dayNumber = 0;
	}
	TotoalStaticsInfo(const TotoalStaticsInfo & other){
		m_LaneStatisc = other.m_LaneStatisc;
		dayNumber = other.dayNumber;
	}
	qint32  dayNumber;
	QList<VehStaticsInfo> m_LaneStatisc;
};
#endif // VEHICLEINFO_H
