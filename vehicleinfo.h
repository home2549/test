#ifndef VEHICLEINFO_H
#define VEHICLEINFO_H

#include <QObject>
#include <QMap>
#include <QDateTime>
#include "../head_struct.h"
//typedef struct tagPointStruct
//{
//    qint32  u16xLeft;  //���λ��
//    qint32  u16xRight; //�Ҳ�λ�� //wzf
//    quint16  u16xMaxHt; //���ֵZ
//
//    quint16   u16Leftpt; //���λ�õ�
//    quint16   u16Rightpt; //�Ҳ�λ�õ�
//    quint16  u16xDis;
//}PointStruct;

#define FRAME_MASK	   0x7F
#define FRAME_MAXCNT   0x7F //128
#define FRAME_BUFLEN   0x7F //128

class VehicleDataStruct{
public:
      quint16  u16EmptFrame; //��ֱ�հ�֡��
      PointStruct location; //��������λ��

	 quint16 u16FrameCnt; //��֡��
     qint32  xdata[FRAME_MAXCNT];
     qint32  zdata[FRAME_MAXCNT][FRAME_BUFLEN];
     qint32  rdata[FRAME_MAXCNT][FRAME_BUFLEN];       
     qint32  zMax[FRAME_MAXCNT];
     qint64  tdata[FRAME_MAXCNT]; //ʱ��
  

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
		NO_USED	 = 0x00,  //��Ч
		OCCURING_USED = 0x01,  //���ڽ���ʱ
		PASSED_USED	 = 0x02  //��ʻ��
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


    STATA_ u8Vstate; //��Ч��־ NO_USED,OCCURING_USED��VehicleInfo::PASSED_USED
    STATA_ u8Istate;

    qint32 u8Lane; //����
    qreal  yLen;  //������
    qreal  ZLen;  //����ĸ߶�
    qreal  xLen;   //����Ŀ��
    qint32  speed;
	   

    //qint32  VemptFrame; //��ֱ�հ�֡��
    //PointStruct locateX_V; //��������λ��
    VehicleDataStruct  Vdata;  //��ֱ

    //qint32  IemptFrame;	//��б�հ�֡��
    //PointStruct locateX_I; //��������λ��
    VehicleDataStruct  Incdata;	//��б

	//�����״ν��뷽��
	enum{
		DIRECT_NO,
		DIRECT_V, //��ֱ
		DIRECT_Q //��б
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
