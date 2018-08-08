#ifndef __HEAD_STRUCT__H
#define __HEAD_STRUCT__H

#define  NEW_TSINGOAL

#include <QObject>
#include <QVector>

typedef qint32 int32;
typedef quint32 uint32;
typedef quint16 uint16;
typedef qint16 int16;
typedef quint8 uint8;

#ifdef NEW_TSINGOAL
typedef struct tagPointStruct
{
    int32  u16xLeft;  //���λ��
    int32  u16xRight; //�Ҳ�λ�� //wzf
    uint16  u16xMaxHt; //���ֵZ

    uint16   u16Leftpt; //���λ�õ�
    uint16   u16Rightpt; //�Ҳ�λ�õ�
    uint16  u16xDis;

    //QVector<qint32> m_values;
	tagPointStruct(){
		u16xLeft = u16xRight = u16xMaxHt = u16Leftpt = u16Rightpt = u16xDis = 0;
      //  m_values.clear();
	}

    tagPointStruct & operator=(const tagPointStruct & other){        
        u16xLeft = other.u16xLeft;
        u16xRight = other.u16xRight;
        u16xMaxHt = other.u16xMaxHt;
        u16Leftpt =other.u16Leftpt;
        u16Rightpt = other.u16Rightpt;
        u16xDis = other.u16xDis;
        return *this;
    }

}PointStruct;
#else
typedef struct tagPointStruct
{
    int32  u16xLeft;  //���λ��
    int32  u16xRight; //�Ҳ�λ�� //wzf
    uint16  u16xMaxHt; //���ֵZ

    uint16   u16Leftpt; //���λ�õ�
    uint16   u16Rightpt; //�Ҳ�λ�õ�
    uint16  u16xDis;

	tagPointStruct(){
		u16xLeft = u16xRight = u16xMaxHt = u16Leftpt = u16Rightpt = u16xDis = 0;
	}
}PointStruct;
#endif

#define POINTSET_MASK	0x0F
#define POINTSET_CNT	0x10
typedef struct tagPointSet
{
    uint8  u8Sum;	   //�г�������
    uint8   uValid[POINTSET_CNT]; //�������Ч��ʶ
    PointStruct	Ptdata[POINTSET_CNT];

	tagPointSet(){
		u8Sum = 0;
		memset(uValid,0,POINTSET_CNT);
	}
}PointSet;

#define POINT_SUM	361
#define FLY_LIMIT_VALUE	100	//��ǵ����ֵ
#define SMALL_AREA	400

#define VEHICHLE_DISTANT_GAP 1000 //
#define HORIZONTAL_LIMIT 300

#define VEHICLE_MASK	0x0F
#define VEHICLE_MAX		0x10

#define NORMAL_MAX_EMPTYFRAME	0x05  //�����հ�֡����
#define ERR_MAX_EMPTYFRAME		0x32  //�������հ�֡����

enum{
	TYPE_XIAO_KE = 0x01, 	//С�ͳ�
	TYPE_DA_KE, 					//��ͳ�
	TYPE_XIAO_HUO, 				//С����
	TYPE_ZHONG_HUO, 			//���ͻ���
	TYPE_DA_HUO, 					//���ͻ���
	TYPE_TE_DA_HUO, 			//�ش��ͻ���
	TYPE_JI_ZHUANG_XIANG, //��װ�䳵
	TYPE_TUO_TA_JI, 			//������
	TYPE_MO_TUO, 					//Ħ�г�
	TYPE_UNKNOWN
};
#define max(x,y)	(x > y ? x:y)
#define min(x,y)	(x > y ? y:x)
#define MAX_U(x,y)	(abs(x) > abs(y) ? x:y)
#define MIN_U(x,y)	(abs(x) > abs(y) ? y:x)

#define IS_INSIDE(x,y,dx,dy)	( (min(dx,dy) > max(x,y) || max(dx,dy) < min(x,y)) ? 0:1 )


typedef struct __TrafficParam{			

	uint8 nSiteNum[16];//վ���� 15 λ

	uint8 nDeviceSN[17]; //�豸���ʶ����

	uint8 nDscIP[5]; //DSC�ϴ���������ַ

	//�������� 
	//01:��ʾ�������������Ŀ��������Ԥ���ֶ�
	//02:��ʾ�������������Ŀ������Ԥ���ֶ� 
	//11:ֻ�������У�������Ԥ���ֶ� 
	//12:ֻ�������У�����Ԥ���ֶ�
	//21:ֻ�������У�������Ԥ���ֶ�
	//22:ֻ�������У�����Ԥ���ֶ�			
	uint8 nInvContents; 

	uint8 nLane_Num; //�����������복������й�

	uint16 nProCycle; //�������ݴ������� ��λ:���� 0x01-0x3C

	uint8  nDisTime; //�����ٷֱȼ���ʱ��������λ���� FF>= XX >00

	uint8  nTransMethod;  //8����ǰ���䷽ʽ

	double  nJingDu;//10��γ������

	double  nWeiDu;

	uint16 nGaoCheng; //�߳�

	//12��Ȩ�޲����û���������
	uint8  nUser[256];

	uint8  nPwd[256];  

}TrafficParam;

#pragma pack(push) //�������״̬
#pragma pack(1)
//���ݰ���ͷ
struct	RD_data	{
                uint8	Type;					//���ݰ�����
                uint8	RD_ID[16];				//�豸���ʶ���룬����ASCII���ʾ
};

//03��ʽ���ݰ�
struct CMD_03	{
                struct	RD_data	RDHead;
                uint8	UserName[8];			//�û�����ASCII��
                uint8	KeyNum[8];				//����
                uint8	New_RD_Num[15];			//��RDվ���ţ�ASCII���ʾ�����㲿����0
};
//04��ʽ���ݰ�
struct CMD_04	{
                struct	RD_data	RDHead;
                uint8	UserName[8];			//�û�����ASCII��
                uint8	KeyNum[8];				//����
                uint8	New_DSC_Ip[4];			//��DSC IP
};

//05��ʽ���ݰ�
struct CMD_05	{
                struct	RD_data	RDHead;
                uint8	UserName[8];			//�û�����ASCII��
                uint8	KeyNum[8];				//����
                uint8	yearL;					//��ݣ���λ��ǰ����λ�ں�
                uint8	yearH;					//��ݣ���λ��ǰ����λ�ں�
                uint8	month; 					//�·�
                uint8	day;					//��
                uint8	hour;					//Сʱ
                uint8	min;					//����
                uint8   sec;					//��
};

//06��ʽ���ݰ�
struct CMD_06	{
                struct	RD_data	RDHead;
                uint8	UserName[8];			//�û�����ASCII��
                uint8	KeyNum[8];				//����
                uint8	New_InvContents; 		//�µ�������
                };

//07��ʽ���ݰ�
struct CMD_07	{
                struct	RD_data	RDHead;
                uint8	UserName[8];			//�û�����ASCII��
                uint8	KeyNum[8];				//����
                uint8	New_ProCycle;			//�½�ͨ���ݴ�������
                };

//0B��ʽ���ݰ�
struct CMD_0B	{
                struct	RD_data	RDHead;
                uint8	UserName[8];			//�û�����ASCII��
                uint8	KeyNum[8];				//����
                uint8	New_DisTime;			//�¸����ٷֱȼ���ʱ��
                };

//03��ʽ���ݰ�
struct CMD_19	{
                struct	RD_data	RDHead;
                uint8	begin_yearL;					//��ݣ���λ��ǰ����λ�ں�
				uint8	begin_yearH;					//��ݣ���λ��ǰ����λ�ں�
                uint8	begin_month; 					//�·�
                uint8	begin_day;					//��
                uint8	begin_hour;					//Сʱ
                uint8	begin_min;					//����
			    uint8	end_yearL;					//��ݣ���λ��ǰ����λ�ں�
				uint8	end_yearH;					//��ݣ���λ��ǰ����λ�ں�
                uint8	end_month; 					//�·�
                uint8	end_day;					//��
                uint8	end_hour;					//Сʱ
                uint8	end_min;					//����
};
struct CMD_1A	{
                struct	RD_data	RDHead;
                uint16	year;					//��ݣ���λ��ǰ����λ�ں�
                uint8	month; 					//�·�
                uint8	day;					//��
                uint8   n1;
				uint8   n2;
				uint8   n3;
				uint16  code;
};
#pragma pack(pop)

#define MAX_LANE	6
#define MAX_VEH_TYPE	9
typedef struct tagStaticsLane
{
	uint8 nLaneID; //������
	uint32 nTotalVehCnt; //�����ܳ�����
	uint32 nGenCheCnt; //�������� 
	int32  nLastVehEnterTm; //��һ������ͷ���ʱ��
	int32  nLastVehSpeed;  //��һ�����ĳ���
	uint32 nDisheadVeh; //��ͷ����ֵ
	uint32 nVehPassTm; //���г���ͨ�ж���ʱ���
	uint32 nVehSpeed[MAX_VEH_TYPE]; //ÿ�ֳ��͵ĳ��ٺ�ֵ	
	uint32 nVehCnt[MAX_VEH_TYPE]; //ÿ�ֳ��͵�ͨ������	

	tagStaticsLane()
	{
	nLaneID = 
	nTotalVehCnt = 
	nGenCheCnt = 
	nLastVehEnterTm = 
	nLastVehSpeed =
	nVehPassTm = 0;
	 
	nDisheadVeh = 0xFFFF;
	memset(nVehSpeed,0,sizeof(uint32)*MAX_VEH_TYPE);
	memset(nVehCnt,0,sizeof(uint32)*MAX_VEH_TYPE);
	}
}StaticsLaneStruct;

#define IS_FLY(x,h)	 (abs(x-h) < 1000)
#endif