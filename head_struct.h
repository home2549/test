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
    int32  u16xLeft;  //左侧位置
    int32  u16xRight; //右侧位置 //wzf
    uint16  u16xMaxHt; //最大值Z

    uint16   u16Leftpt; //左侧位置点
    uint16   u16Rightpt; //右侧位置点
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
    int32  u16xLeft;  //左侧位置
    int32  u16xRight; //右侧位置 //wzf
    uint16  u16xMaxHt; //最大值Z

    uint16   u16Leftpt; //左侧位置点
    uint16   u16Rightpt; //右侧位置点
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
    uint8  u8Sum;	   //有车的数量
    uint8   uValid[POINTSET_CNT]; //区域的有效标识
    PointStruct	Ptdata[POINTSET_CNT];

	tagPointSet(){
		u8Sum = 0;
		memset(uValid,0,POINTSET_CNT);
	}
}PointSet;

#define POINT_SUM	361
#define FLY_LIMIT_VALUE	100	//打非点的阈值
#define SMALL_AREA	400

#define VEHICHLE_DISTANT_GAP 1000 //
#define HORIZONTAL_LIMIT 300

#define VEHICLE_MASK	0x0F
#define VEHICLE_MAX		0x10

#define NORMAL_MAX_EMPTYFRAME	0x05  //正常空白帧上限
#define ERR_MAX_EMPTYFRAME		0x32  //非正常空白帧上限

enum{
	TYPE_XIAO_KE = 0x01, 	//小客车
	TYPE_DA_KE, 					//大客车
	TYPE_XIAO_HUO, 				//小货车
	TYPE_ZHONG_HUO, 			//中型货车
	TYPE_DA_HUO, 					//大型货车
	TYPE_TE_DA_HUO, 			//特大型货车
	TYPE_JI_ZHUANG_XIANG, //集装箱车
	TYPE_TUO_TA_JI, 			//拖拉机
	TYPE_MO_TUO, 					//摩托车
	TYPE_UNKNOWN
};
#define max(x,y)	(x > y ? x:y)
#define min(x,y)	(x > y ? y:x)
#define MAX_U(x,y)	(abs(x) > abs(y) ? x:y)
#define MIN_U(x,y)	(abs(x) > abs(y) ? y:x)

#define IS_INSIDE(x,y,dx,dy)	( (min(dx,dy) > max(x,y) || max(dx,dy) < min(x,y)) ? 0:1 )


typedef struct __TrafficParam{			

	uint8 nSiteNum[16];//站点编号 15 位

	uint8 nDeviceSN[17]; //设备身份识别码

	uint8 nDscIP[5]; //DSC上传服务器地址

	//调查内容 
	//01:表示调查包括所有项目，不包含预留字段
	//02:表示调查包括所有项目，包含预留字段 
	//11:只调查上行，不包含预留字段 
	//12:只调查上行，包含预留字段
	//21:只调查下行，不包含预留字段
	//22:只调查下行，包含预留字段			
	uint8 nInvContents; 

	uint8 nLane_Num; //车道数量，与车道编号有关

	uint16 nProCycle; //交调数据处理周期 单位:分钟 0x01-0x3C

	uint8  nDisTime; //跟车百分比计算时间间隔，单位：秒 FF>= XX >00

	uint8  nTransMethod;  //8、当前传输方式

	double  nJingDu;//10、纬度坐标

	double  nWeiDu;

	uint16 nGaoCheng; //高程

	//12、权限操作用户名、密码
	uint8  nUser[256];

	uint8  nPwd[256];  

}TrafficParam;

#pragma pack(push) //保存对齐状态
#pragma pack(1)
//数据包包头
struct	RD_data	{
                uint8	Type;					//数据包类型
                uint8	RD_ID[16];				//设备身份识别码，需用ASCII码表示
};

//03格式数据包
struct CMD_03	{
                struct	RD_data	RDHead;
                uint8	UserName[8];			//用户名，ASCII码
                uint8	KeyNum[8];				//密码
                uint8	New_RD_Num[15];			//新RD站点编号，ASCII码表示，不足部分填0
};
//04格式数据包
struct CMD_04	{
                struct	RD_data	RDHead;
                uint8	UserName[8];			//用户名，ASCII码
                uint8	KeyNum[8];				//密码
                uint8	New_DSC_Ip[4];			//新DSC IP
};

//05格式数据包
struct CMD_05	{
                struct	RD_data	RDHead;
                uint8	UserName[8];			//用户名，ASCII码
                uint8	KeyNum[8];				//密码
                uint8	yearL;					//年份，低位在前、高位在后
                uint8	yearH;					//年份，低位在前、高位在后
                uint8	month; 					//月份
                uint8	day;					//日
                uint8	hour;					//小时
                uint8	min;					//分钟
                uint8   sec;					//秒
};

//06格式数据包
struct CMD_06	{
                struct	RD_data	RDHead;
                uint8	UserName[8];			//用户名，ASCII码
                uint8	KeyNum[8];				//密码
                uint8	New_InvContents; 		//新调查内容
                };

//07格式数据包
struct CMD_07	{
                struct	RD_data	RDHead;
                uint8	UserName[8];			//用户名，ASCII码
                uint8	KeyNum[8];				//密码
                uint8	New_ProCycle;			//新交通数据处理周期
                };

//0B格式数据包
struct CMD_0B	{
                struct	RD_data	RDHead;
                uint8	UserName[8];			//用户名，ASCII码
                uint8	KeyNum[8];				//密码
                uint8	New_DisTime;			//新跟车百分比鉴别时间
                };

//03格式数据包
struct CMD_19	{
                struct	RD_data	RDHead;
                uint8	begin_yearL;					//年份，低位在前、高位在后
				uint8	begin_yearH;					//年份，低位在前、高位在后
                uint8	begin_month; 					//月份
                uint8	begin_day;					//日
                uint8	begin_hour;					//小时
                uint8	begin_min;					//分钟
			    uint8	end_yearL;					//年份，低位在前、高位在后
				uint8	end_yearH;					//年份，低位在前、高位在后
                uint8	end_month; 					//月份
                uint8	end_day;					//日
                uint8	end_hour;					//小时
                uint8	end_min;					//分钟
};
struct CMD_1A	{
                struct	RD_data	RDHead;
                uint16	year;					//年份，低位在前、高位在后
                uint8	month; 					//月份
                uint8	day;					//日
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
	uint8 nLaneID; //车道号
	uint32 nTotalVehCnt; //车道总车数量
	uint32 nGenCheCnt; //跟车数量 
	int32  nLastVehEnterTm; //上一辆车车头检测时间
	int32  nLastVehSpeed;  //上一辆车的车速
	uint32 nDisheadVeh; //车头间距和值
	uint32 nVehPassTm; //所有车辆通行断面时间和
	uint32 nVehSpeed[MAX_VEH_TYPE]; //每种车型的车速和值	
	uint32 nVehCnt[MAX_VEH_TYPE]; //每种车型的通车数量	

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