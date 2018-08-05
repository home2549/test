#include "trafficalg.h"
#include "trafficsettings.h"
#ifdef DEV_TEST
#include "../virtualbackgrounddbg.h"
#include "DispatchMsg/thirddispatchmsg.h"
#else
#include "../virtualbackground.h"
#endif
#include <QTimer>
#include "../mathhelper.h"
#include <QDebug>

extern "C"{
#include "../dog/watchdog.h"
}
TrafficAlg::TrafficAlg(QObject *parent)
	: QObject(parent),g_leftXOffset(0),g_rightXOffset(0)
{
	qRegisterMetaType<VehicleInfo>("VehicleInfo");
#ifdef DEV_TEST
	m_vBackground = new VirtualBackGroundDbg(VirtualBackGroundDbg::V_DIR,this);
#else
	m_vBackground = new VirtualBackGround(this);
#endif
	m_vBackground->setStartPos(TrafficSettings::instance()->vLmsPtStart());
	m_vBackground->setEndPos(TrafficSettings::instance()->vLmsPtStop());
	m_vBackground->setExcleftPos(TrafficSettings::instance()->vLmsObstacleleft());
	m_vBackground->setExcrightPos(TrafficSettings::instance()->vLmsObstacleright());
	m_vBackground->setHeight(TrafficSettings::instance()->vLmsHeight());
	m_vBackground->setMid(TrafficSettings::instance()->vLmsPtMid());
	connect(m_vBackground,SIGNAL(signal_heightChanged(qint32)),TrafficSettings::instance(),SLOT(setvLmsHeight(qint32)));

#ifdef DEV_TEST
	m_qBackground = new VirtualBackGroundDbg(VirtualBackGroundDbg::Q_DIR,this);
#else
	m_qBackground = new VirtualBackGround(this);
#endif
	m_qBackground->setStartPos(TrafficSettings::instance()->qLmsPtStart());
	m_qBackground->setEndPos(TrafficSettings::instance()->qLmsPtStop());
	m_qBackground->setExcleftPos(TrafficSettings::instance()->qLmsObstacleleft());
	m_qBackground->setExcrightPos(TrafficSettings::instance()->qLmsObstacleright());
	m_qBackground->setHeight(TrafficSettings::instance()->qLmsHeight());
	m_qBackground->setMid(TrafficSettings::instance()->qLmsPtMid());
	connect(m_qBackground,SIGNAL(signal_heightChanged(qint32)),TrafficSettings::instance(),SLOT(setqLmsHeight(qint32)));

	connect(this,SIGNAL(signal_calc()),this,SLOT(process()));
	m_Vehicle.clear();

	VehicleInfo::InitStatic();
	
	m_dayTimer = new QTimer(this);
	connect(m_dayTimer,SIGNAL(timeout()),this,SLOT(slot_dayCheck()));
	m_dayTimer->start(1000);
	m_curDt= QDate::currentDate();

	//m_dogTimer = new QTimer(this);
	//connect(m_dogTimer,SIGNAL(timeout()),this,SLOT(slot_alive()));
	//m_dogTimer->start(890);
	//m_dogHandler = open_wtd();
	//set_wtd(m_dogHandler, 3);

#ifdef DEV_TEST
	ThirdDispatchMsg::reg_signal(this,SIGNAL(sig_dbg_veh_height(const QByteArray &)),ThirdDispatchMsg::VEH_INFO);
#endif
}

TrafficAlg::~TrafficAlg()
{
	delete m_vBackground;
	delete m_qBackground;
}

void TrafficAlg::slot_alive()
{
	feed_wtd(m_dogHandler);
}

void TrafficAlg::slot_dayCheck()
{
	if(m_curDt.daysTo(QDate::currentDate())){
		VehicleInfo::InitStatic();
		m_curDt = QDate::currentDate();
	}
}

void TrafficAlg::slot_vLastData(const LasterData & nDisInfo)
{
	static qint32 tTestIndex = 0;
	tTestIndex++;
	if(tTestIndex >= 11793){
		tTestIndex = tTestIndex;
	}
	m_vBackground->update(nDisInfo.data());	

	m_vCache.enqueue(nDisInfo);

	if(m_qCache.size()){
		if(m_qCache.size() > m_vCache.size() + 5){
			//clear the q 
			while(m_qCache.size() > m_vCache.size()){
				m_qCache.dequeue();
			}
		}
		emit signal_calc();
	}else if(m_vCache.size() > 15){

		qint32 nDel = 0;
		while(nDel < 10){
			m_vCache.dequeue();
			nDel++;
		}
	}
}

void TrafficAlg::slot_qLastData(const LasterData & nDisInfo)
{
	m_qBackground->update(nDisInfo.data());	

	m_qCache.enqueue(nDisInfo);

	if(m_vCache.size()){

		if(m_vCache.size() > m_qCache.size() + 5){
			//clear the q 
			while(m_vCache.size() > m_qCache.size()){
				m_vCache.dequeue();
			}
		}

		emit signal_calc();
	}else if(m_qCache.size() > 15){

		qint32 nDel = 0;
		while(nDel < 10){
			m_qCache.dequeue();
			nDel++;
		}
	}
}


bool TrafficAlg::trySplit(const PointStruct *src,const LasterData & ndata,QList<PointStruct> & _return)
{
	qint16 t_leftPt = src->u16Leftpt;
	qint16 t_rightPt = src->u16Rightpt;
//	qint32 t_defaultXdiff = 80;
	const LasterSettings &t_settings = ndata.settings();
	//大小车分，大车分，小车分
//	QList<qint32> t_xdiff;	
	QMap<qint32,qint32> t_segIndex;
	qint32 t_index = t_leftPt;
	const QList<qint32> &  t_xdis = ndata.xdata();
	while(t_leftPt < t_rightPt){	
		qint32 t_diff = t_xdis[t_leftPt+1] - t_xdis[t_leftPt];
		if(!t_xdis[t_leftPt+1] || !t_xdis[t_leftPt]){
//			t_xdiff.append(t_defaultXdiff); 
		}else{
//			t_xdiff.append(t_diff); 

			if(t_diff < 50){			
				if(t_segIndex.contains(t_index) && 
					(t_index+t_segIndex[t_index] == t_leftPt)
				  )
				{
					t_segIndex[t_index]++;
				}else{
					t_segIndex.insert(t_index,1);
				}
			}
		}				
		t_leftPt++;
	}	

	t_leftPt = src->u16Leftpt;
	t_rightPt = src->u16Rightpt;
	qint32 t_leftx = src->u16xLeft;

	const QList<qint32> & t_heights =  ndata.zdata();
	QMapIterator<qint32,qint32> iEveryItem(t_segIndex);
	while(iEveryItem.hasNext()){
		iEveryItem.next();
		qint32 t_edge = iEveryItem.key();

		if(abs(t_heights[t_edge] - t_heights[t_edge + iEveryItem.value()])>750)
		{
			PointStruct t_newSeg;
			t_newSeg.u16Leftpt = t_leftPt;

			if(t_heights[t_leftPt] > t_heights[t_edge + iEveryItem.value()]){
				t_newSeg.u16Rightpt = t_edge + iEveryItem.value();
			}else{
				t_newSeg.u16Rightpt = t_edge;
			}

			if(t_newSeg.u16Rightpt - t_newSeg.u16Leftpt > 5){
				t_newSeg.u16xLeft = t_leftx;
				t_newSeg.u16xRight = t_xdis[t_newSeg.u16Rightpt];
				t_newSeg.u16xDis = t_newSeg.u16xRight - t_newSeg.u16xLeft;
				t_newSeg.u16xMaxHt = GetMaxValue(t_heights,t_newSeg.u16Leftpt,t_newSeg.u16Rightpt);
				_return.append(t_newSeg);

				t_leftPt = t_newSeg.u16Rightpt+1;
				t_leftx = t_xdis[t_leftPt];			
			}			
		}
	}
	
	return false;
}

void TrafficAlg::process()
{
	LasterData LMS_data_1 = m_vCache.dequeue();
	LasterData LMS_data_2 = m_qCache.dequeue();

	if(!m_qBackground->isReady() || !m_vBackground->isReady()){

		return;
	}

	qint64 Time_Vertical = LMS_data_1.timestamp();
	LMS_data_2.setTimestamp(Time_Vertical);

	int i;
	int j;

	int32 l_32tmp,l_tmp1,TempVaule1,TempVaule2;
	uint16 l_32tmp2;
	PointStruct l_u16PosVect[POINTSET_CNT];	//存放并车的位置信息
	uint16 l_u16index,l_u16tmp;

	uint16 l_leftPt = 0,l_rightPt = 0;	//左,右点数
	int32 l_leftX = 0,l_rightX = 0;    //左，右x距离
	uint16 l_leftPt_absV = 0,l_rightPt_absV = 0;
	uint16 l_leftPt_absI = 0,l_rightPt_absI = 0;

	bool t_matched = false;

	PointSet l_FrameInfo1,l_FrameInfo2;
	//帧数据进行分有车区域,记录每个区域的左右位置信息
	LMS_data_1.splitArea(TrafficSettings::instance()->getLasterV(),m_vBackground,&l_FrameInfo1);
	LMS_data_2.splitArea(TrafficSettings::instance()->getLasterQ(),m_qBackground,&l_FrameInfo2);
	//计算车辆的车道号时，车道计算的起始位置与上面计算的位置存在隔离区偏差
	//故而放在此处计算最为合适，不常变化，仅计算一次就OK。
	if(TrafficSettings::instance()->getLasterV().LmsObstacleleft() && !g_leftXOffset)
	{
		l_u16tmp = abs(TrafficSettings::instance()->getLasterV().LmsPtMid() - TrafficSettings::instance()->getLasterV().LmsObstacleleft());
		g_leftXOffset = MathHelper::Sin(LMS_data_1[TrafficSettings::instance()->getLasterV().LmsObstacleleft()],l_u16tmp);
		g_leftXOffset *=(-1);
	}

	if(TrafficSettings::instance()->getLasterV().LmsObstacleright() && !g_rightXOffset)
	{
		l_u16tmp = abs(TrafficSettings::instance()->getLasterV().LmsPtMid() - TrafficSettings::instance()->getLasterV().LmsObstacleright());
		g_rightXOffset = MathHelper::Sin(LMS_data_1[TrafficSettings::instance()->getLasterV().LmsObstacleright()],l_u16tmp);
	}

	l_leftPt_absV = LMS_data_1.GetLeft();
	l_rightPt_absV = LMS_data_1.GetRight();

	//上述区域进行拆分的过程中包括有几种情况：
	//1、打非点存在的情况下，计算的区域宽度较大
	//2、并车行驶时，区域连成一边，需要进行拆分
	qint32 g_totalVehicle = m_Vehicle.size();

	//对区域进行拆分操作
	for(l_u16index = 0;l_u16index < l_FrameInfo1.u8Sum;l_u16index++)
	{
		t_matched = false;  //匹配标识，成功置1。
		l_leftX = l_FrameInfo1.Ptdata[l_u16index].u16xLeft;
		l_rightX = l_FrameInfo1.Ptdata[l_u16index].u16xRight;
		if (!LasterData::Valid(&l_FrameInfo1.Ptdata[l_u16index],TrafficSettings::instance()->getLasterV()))
		{
			continue;
		}

		for(j = 0;j < g_totalVehicle;j++)
		{  	
			i = j;		
			if( (m_Vehicle[i]->u8Vstate == VehicleInfo::OCCURING_USED || m_Vehicle[i]->u8Istate != VehicleInfo::NO_USED) )
			{
				if(IS_INSIDE(l_leftX,l_rightX,m_Vehicle[i]->locateX.u16xLeft,m_Vehicle[i]->locateX.u16xRight))
				{
					t_matched = true;  //区域匹配成功
					m_Vehicle[i]->u8Vstate = VehicleInfo::OCCURING_USED;	

					if(l_FrameInfo1.Ptdata[l_u16index].u16xDis <= TrafficSettings::instance()->LaneWide())
					{  //单车	
SINGLE_V_LABEL:
						//						qDebug()<<"[sky2]match V";
						l_32tmp2 =m_Vehicle[i]->getFrame_v();
						if(l_32tmp2 == 0)
						{
							m_Vehicle[i]->locateX.u16xLeft = l_leftX;
							m_Vehicle[i]->locateX.u16xRight = l_rightX;
							m_Vehicle[i]->locateX.u16xDis = l_FrameInfo1.Ptdata[l_u16index].u16xDis;
							
							m_Vehicle[i]->locateX.u16Leftpt = l_FrameInfo1.Ptdata[l_u16index].u16Leftpt;
							m_Vehicle[i]->locateX.u16Rightpt = l_FrameInfo1.Ptdata[l_u16index].u16Rightpt;	

							l_leftPt = m_Vehicle[i]->locateX.u16Leftpt;
							l_rightPt = m_Vehicle[i]->locateX.u16Rightpt;

							const QList<qint32> & zBuffer = LMS_data_1.zdata();
							copy(&m_Vehicle[i]->Vdata.zdata[l_32tmp2][1],zBuffer.begin() + l_leftPt,zBuffer.begin() + l_rightPt);
							m_Vehicle[i]->Vdata.zdata[l_32tmp2][0]  = l_rightPt - l_leftPt;

							m_Vehicle[i]->Vdata.zMax[l_32tmp2] = l_FrameInfo1.Ptdata[l_u16index].u16xMaxHt; 
						}
						else
						{
							m_Vehicle[i]->locateX.u16xLeft = min(l_leftX,m_Vehicle[i]->locateX.u16xLeft);
							m_Vehicle[i]->locateX.u16xRight = max(l_rightX,m_Vehicle[i]->locateX.u16xRight);

							if(Time_Vertical == m_Vehicle[i]->Vdata.tdata[l_32tmp2 - 1])
							{
								l_32tmp2--;
								m_Vehicle[i]->Vdata.u16FrameCnt--;
								//同一帧中，有两个区域是同一车的,车辆较宽时
								m_Vehicle[i]->locateX.u16Leftpt = min(m_Vehicle[i]->locateX.u16Leftpt,
																	  l_FrameInfo1.Ptdata[l_u16index].u16Leftpt);

								m_Vehicle[i]->locateX.u16Rightpt = max(m_Vehicle[i]->locateX.u16Rightpt,
																	   l_FrameInfo1.Ptdata[l_u16index].u16Rightpt);

								l_leftPt = m_Vehicle[i]->locateX.u16Leftpt;
								l_rightPt = m_Vehicle[i]->locateX.u16Rightpt;
								
								const QList<qint32> & zBuffer = LMS_data_1.zdata();
								copy(&m_Vehicle[i]->Vdata.zdata[l_32tmp2][1],zBuffer.begin() + l_leftPt,zBuffer.begin() + l_rightPt);
								m_Vehicle[i]->Vdata.zdata[l_32tmp2][0]  = l_rightPt - l_leftPt;

								m_Vehicle[i]->Vdata.xdata[l_32tmp2] += l_FrameInfo1.Ptdata[l_u16index].u16xDis; 

								m_Vehicle[i]->Vdata.zMax[l_32tmp2] = max(l_FrameInfo1.Ptdata[l_u16index].u16xMaxHt,
																		 m_Vehicle[i]->Vdata.zMax[l_32tmp2]); 
							}
							else
							{
								m_Vehicle[i]->locateX.u16Leftpt = l_FrameInfo1.Ptdata[l_u16index].u16Leftpt;
								m_Vehicle[i]->locateX.u16Rightpt = l_FrameInfo1.Ptdata[l_u16index].u16Rightpt;

								l_leftPt = m_Vehicle[i]->locateX.u16Leftpt;
								l_rightPt = m_Vehicle[i]->locateX.u16Rightpt;

								const QList<qint32> & zBuffer = LMS_data_1.zdata();
								copy(&m_Vehicle[i]->Vdata.zdata[l_32tmp2][1],zBuffer.begin() + l_leftPt,zBuffer.begin() + l_rightPt);
								m_Vehicle[i]->Vdata.zdata[l_32tmp2][0]  = l_rightPt - l_leftPt;

								m_Vehicle[i]->Vdata.xdata[l_32tmp2] = l_FrameInfo1.Ptdata[l_u16index].u16xDis;
								m_Vehicle[i]->Vdata.zMax[l_32tmp2] = l_FrameInfo1.Ptdata[l_u16index].u16xMaxHt; 

							} //非同时间戳
							
						} //非第一帧
						m_Vehicle[i]->Vdata.tdata[l_32tmp2] = Time_Vertical;
						
						m_Vehicle[i]->incFrame_v();
						qint32 t_debug = m_Vehicle[i]->getFrame_v();
						if(t_debug >100){
							t_debug = t_debug;
						}
						m_Vehicle[i]->VemptFrame = 0;			
					}
					else
					{
						//并车情况：
						//	A:存在明显的分割点 
						// B不存在明显的分割点 --- 车在左侧，车在中间，车在右侧
						QList<PointStruct>	tmp_data;
						if(trySplit(&l_FrameInfo1.Ptdata[l_u16index],LMS_data_1,tmp_data)){
							QListIterator<PointStruct>	iEveryItem(tmp_data);
							while(iEveryItem.hasNext()){
								const PointStruct  & ref_item = iEveryItem.next();
								l_FrameInfo1.Ptdata[l_FrameInfo1.u8Sum++] = ref_item;
							}

						} else{
							//可能不是并车情况
							goto SINGLE_V_LABEL;
						}						
					}
					break;
				}
			}

		}

		if(!t_matched)
		{ 
			//进新车     
			//区域是否为并车
			if(l_FrameInfo1.Ptdata[l_u16index].u16xDis > TrafficSettings::instance()->LaneWide())  //大于车道宽度，认为是并车
			{
				QList<PointStruct>	tmp_data;
				if(trySplit(&l_FrameInfo1.Ptdata[l_u16index],LMS_data_1,tmp_data)){
							
					QListIterator<PointStruct>	iEveryItem(tmp_data);
					while(iEveryItem.hasNext()){
						const PointStruct  & ref_item = iEveryItem.next();
						l_FrameInfo1.Ptdata[l_FrameInfo1.u8Sum++] = ref_item;
					}
				} 
			} else{
				VehicleInfo* t_newVeh = new VehicleInfo();
				t_newVeh->direct = VehicleInfo::DIRECT_V;
				m_Vehicle.append(t_newVeh);
			
				//qDebug()<<"[V]new Vehicle detected";

				t_newVeh->u8Vstate = VehicleInfo::OCCURING_USED;

				l_leftPt = l_FrameInfo1.Ptdata[l_u16index].u16Leftpt;
				l_rightPt = l_FrameInfo1.Ptdata[l_u16index].u16Rightpt;
				l_leftX = l_FrameInfo1.Ptdata[l_u16index].u16xLeft;
				l_rightX = l_FrameInfo1.Ptdata[l_u16index].u16xRight;
				qint32 l_maxHt = l_FrameInfo1.Ptdata[l_u16index].u16xMaxHt;

				t_newVeh->locateX.u16xLeft = l_leftX;
				t_newVeh->locateX.u16xRight = l_rightX;
				t_newVeh->locateX.u16Leftpt = l_leftPt;
				t_newVeh->locateX.u16Rightpt = l_rightPt;
				t_newVeh->locateX.u16xDis = l_FrameInfo1.Ptdata[l_u16index].u16xDis;
							

				l_32tmp2 = t_newVeh->Vdata.u16FrameCnt = 0;

				const QList<qint32> & zBuffer = LMS_data_1.zdata();
				copy(&t_newVeh->Vdata.zdata[l_32tmp2][1],zBuffer.begin() + l_leftPt,zBuffer.begin() + l_rightPt);
				t_newVeh->Vdata.zdata[l_32tmp2][0] = l_rightPt - l_leftPt;
				t_newVeh->Vdata.xdata[l_32tmp2] = l_FrameInfo1.Ptdata[l_u16index].u16xDis;
				t_newVeh->Vdata.zMax[l_32tmp2] = l_maxHt;

				t_newVeh->Vdata.tdata[l_32tmp2] = Time_Vertical;
				t_newVeh->incFrame_v();                       

				t_newVeh->VemptFrame = 0;
			}
		} //end if进新车 
	}   //end for

	/**************************************************************************/
	/*************************倾斜激光器处理***********************************/

	l_leftPt_absI = LMS_data_2.GetLeft();
	l_rightPt_absI = LMS_data_2.GetRight();
	//对区域进行拆分操作
	for(l_u16index = 0;l_u16index < l_FrameInfo2.u8Sum ;l_u16index++)
	{
		t_matched = false;  //匹配标识，成功置1。
		l_leftX = l_FrameInfo2.Ptdata[l_u16index].u16xLeft;
		l_rightX = l_FrameInfo2.Ptdata[l_u16index].u16xRight;
		if (!LasterData::Valid(&l_FrameInfo2.Ptdata[l_u16index],TrafficSettings::instance()->getLasterQ()))
		{
			continue;
		}
		
		for(j = 0;j < g_totalVehicle;j++)
		{  	
			i = j;//g_VehicleSetIndex[j] - 1;		
			if((m_Vehicle[i]->u8Vstate != VehicleInfo::NO_USED || m_Vehicle[i]->u8Istate == VehicleInfo::OCCURING_USED))
			{
				if(IS_INSIDE(l_leftX,l_rightX,m_Vehicle[i]->locateX.u16xLeft,m_Vehicle[i]->locateX.u16xRight))
				{
					t_matched = true;  //区域匹配成功
					m_Vehicle[i]->u8Istate = VehicleInfo::OCCURING_USED;	
					if(l_FrameInfo2.Ptdata[l_u16index].u16xDis <= TrafficSettings::instance()->LaneWide())
					{  //单车
SINGLE_Q_LABEL:
						l_32tmp2 =m_Vehicle[i]->getFrame_q();
						if(l_32tmp2 == 0)
						{
							m_Vehicle[i]->locateX.u16xLeft = l_leftX;
							m_Vehicle[i]->locateX.u16xRight = l_rightX;
							m_Vehicle[i]->locateX.u16xDis = l_FrameInfo2.Ptdata[l_u16index].u16xDis;
							
							m_Vehicle[i]->locateX.u16Leftpt = l_FrameInfo2.Ptdata[l_u16index].u16Leftpt;
							m_Vehicle[i]->locateX.u16Rightpt = l_FrameInfo2.Ptdata[l_u16index].u16Rightpt;	

							l_leftPt = m_Vehicle[i]->locateX.u16Leftpt;
							l_rightPt = m_Vehicle[i]->locateX.u16Rightpt;

							const QList<qint32> & zBuffer = LMS_data_2.zdata();
							copy(&m_Vehicle[i]->Incdata.zdata[l_32tmp2][1],zBuffer.begin() + l_leftPt,zBuffer.begin() + l_rightPt);
							m_Vehicle[i]->Incdata.zdata[l_32tmp2][0]  = l_rightPt - l_leftPt;

							m_Vehicle[i]->Incdata.zMax[l_32tmp2] = l_FrameInfo2.Ptdata[l_u16index].u16xMaxHt; 
						}
						else
						{
							m_Vehicle[i]->locateX.u16xLeft = min(l_leftX,m_Vehicle[i]->locateX.u16xLeft);
							m_Vehicle[i]->locateX.u16xRight = max(l_rightX,m_Vehicle[i]->locateX.u16xRight);

							if(Time_Vertical == m_Vehicle[i]->Incdata.tdata[l_32tmp2 - 1])
							{
								l_32tmp2--;
								m_Vehicle[i]->Incdata.u16FrameCnt--;
								//同一帧中，有两个区域是同一车的,车辆较宽时
								m_Vehicle[i]->locateX.u16Leftpt = min(m_Vehicle[i]->locateX.u16Leftpt,
																	  l_FrameInfo2.Ptdata[l_u16index].u16Leftpt);

								m_Vehicle[i]->locateX.u16Rightpt = max(m_Vehicle[i]->locateX.u16Rightpt,
																	   l_FrameInfo2.Ptdata[l_u16index].u16Rightpt);

								l_leftPt = m_Vehicle[i]->locateX.u16Leftpt;
								l_rightPt = m_Vehicle[i]->locateX.u16Rightpt;
								
								const QList<qint32> & zBuffer = LMS_data_2.zdata();
								copy(&m_Vehicle[i]->Incdata.zdata[l_32tmp2][1],zBuffer.begin() + l_leftPt,zBuffer.begin() + l_rightPt);
								m_Vehicle[i]->Incdata.zdata[l_32tmp2][0]  = l_rightPt - l_leftPt;

								m_Vehicle[i]->Incdata.xdata[l_32tmp2] += l_FrameInfo2.Ptdata[l_u16index].u16xDis; 

								m_Vehicle[i]->Incdata.zMax[l_32tmp2] = max(l_FrameInfo2.Ptdata[l_u16index].u16xMaxHt,
																		 m_Vehicle[i]->Incdata.zMax[l_32tmp2]); 
							}
							else
							{
								m_Vehicle[i]->locateX.u16Leftpt = l_FrameInfo2.Ptdata[l_u16index].u16Leftpt;
								m_Vehicle[i]->locateX.u16Rightpt = l_FrameInfo2.Ptdata[l_u16index].u16Rightpt;

								l_leftPt = m_Vehicle[i]->locateX.u16Leftpt;
								l_rightPt = m_Vehicle[i]->locateX.u16Rightpt;

								const QList<qint32> & zBuffer = LMS_data_2.zdata();
								copy(&m_Vehicle[i]->Incdata.zdata[l_32tmp2][1],zBuffer.begin() + l_leftPt,zBuffer.begin() + l_rightPt);
								m_Vehicle[i]->Incdata.zdata[l_32tmp2][0]  = l_rightPt - l_leftPt;

								m_Vehicle[i]->Incdata.xdata[l_32tmp2] = l_FrameInfo2.Ptdata[l_u16index].u16xDis;
								m_Vehicle[i]->Incdata.zMax[l_32tmp2] = l_FrameInfo2.Ptdata[l_u16index].u16xMaxHt; 

							} //非同时间戳
							
						} //非第一帧
						m_Vehicle[i]->Incdata.tdata[l_32tmp2] = Time_Vertical;
						
						m_Vehicle[i]->incFrame_q();
						//m_Vehicle[i]->Incdata.u16FrameCnt  &=  FRAME_MASK;

						qint32 t_debug = m_Vehicle[i]->getFrame_q();
						if(t_debug >100){
							t_debug = t_debug;
						}
						m_Vehicle[i]->IemptFrame = 0;			
					}
					else
					{
						//并车情况：
						//	A:存在明显的分割点 
						// B不存在明显的分割点 --- 车在左侧，车在中间，车在右侧
						QList<PointStruct>	tmp_data;
						if(trySplit(&l_FrameInfo2.Ptdata[l_u16index],LMS_data_2,tmp_data)){
							QListIterator<PointStruct>	iEveryItem(tmp_data);
							while(iEveryItem.hasNext()){
								const PointStruct  & ref_item = iEveryItem.next();
								l_FrameInfo2.Ptdata[l_FrameInfo2.u8Sum++] = ref_item;
							}

						} else{
							//可能不是并车情况
							goto SINGLE_Q_LABEL;
						}
					}
					break;
				}
			}

		} //end for match exist veh


		if(!t_matched)
		{ 
			//进新车     
			//区域是否为并车
			if(l_FrameInfo2.Ptdata[l_u16index].u16xDis > TrafficSettings::instance()->LaneWide())  //大于车道宽度，认为是并车
			{
				QList<PointStruct>	tmp_data;
				if(trySplit(&l_FrameInfo2.Ptdata[l_u16index],LMS_data_2,tmp_data)){
							
					QListIterator<PointStruct>	iEveryItem(tmp_data);
					while(iEveryItem.hasNext()){
						const PointStruct  & ref_item = iEveryItem.next();
						l_FrameInfo2.Ptdata[l_FrameInfo2.u8Sum++] = ref_item;
					}
				} 
			} else{
				VehicleInfo* t_newVeh = new VehicleInfo();
				t_newVeh->direct = VehicleInfo::DIRECT_Q;
				m_Vehicle.append(t_newVeh);
			
				//qDebug()<<"[Q]new Vehicle detected";

				t_newVeh->u8Istate = VehicleInfo::OCCURING_USED;

				l_leftPt = l_FrameInfo2.Ptdata[l_u16index].u16Leftpt;
				l_rightPt = l_FrameInfo2.Ptdata[l_u16index].u16Rightpt;
				l_leftX = l_FrameInfo2.Ptdata[l_u16index].u16xLeft;
				l_rightX = l_FrameInfo2.Ptdata[l_u16index].u16xRight;
				qint32 l_maxHt = l_FrameInfo2.Ptdata[l_u16index].u16xMaxHt;

				t_newVeh->locateX.u16xLeft = l_leftX;
				t_newVeh->locateX.u16xRight = l_rightX;
				t_newVeh->locateX.u16Leftpt = l_leftPt;
				t_newVeh->locateX.u16Rightpt = l_rightPt;
				t_newVeh->locateX.u16xDis = l_FrameInfo2.Ptdata[l_u16index].u16xDis;
							

				l_32tmp2 = t_newVeh->Incdata.u16FrameCnt = 0;

				const QList<qint32> & zBuffer = LMS_data_2.zdata();
				copy(&t_newVeh->Incdata.zdata[l_32tmp2][1],zBuffer.begin() + l_leftPt,zBuffer.begin() + l_rightPt);
				t_newVeh->Incdata.zdata[l_32tmp2][0] = l_rightPt - l_leftPt;
				t_newVeh->Incdata.xdata[l_32tmp2] = l_FrameInfo2.Ptdata[l_u16index].u16xDis;
				t_newVeh->Incdata.zMax[l_32tmp2] = l_maxHt;

				t_newVeh->Incdata.tdata[l_32tmp2] = Time_Vertical;
				t_newVeh->incFrame_q();                       

				t_newVeh->IemptFrame = 0;
			}
		} //end if进新车 

	}   //end for


	QListIterator<VehicleInfo*> iEveryCompleteItem(m_Vehicle);
	while(iEveryCompleteItem.hasNext())
	{
		VehicleInfo* const & ref_VehItem = iEveryCompleteItem.next();

		if(ref_VehItem->u8Vstate != VehicleInfo::NO_USED || ref_VehItem->u8Istate != VehicleInfo::NO_USED)
		{
			if(ref_VehItem->VemptFrame > NORMAL_MAX_EMPTYFRAME && ref_VehItem->Vdata.u16FrameCnt > 1)
			{
				ref_VehItem->u8Vstate = VehicleInfo::PASSED_USED;  //已结束，可收尾的车
			}
			if(ref_VehItem->IemptFrame > NORMAL_MAX_EMPTYFRAME && ref_VehItem->Incdata.u16FrameCnt > 1)
			{
				ref_VehItem->u8Istate = VehicleInfo::PASSED_USED;  //已结束，可收尾的车
			}

			if(ref_VehItem->IemptFrame >ERR_MAX_EMPTYFRAME && ref_VehItem->VemptFrame >ERR_MAX_EMPTYFRAME)
			{
				delete ref_VehItem;
				m_Vehicle.removeOne(ref_VehItem);    
				//qDebug()<<"Vehicle ignored";
				continue;
			}
			else if(ref_VehItem->u8Istate == VehicleInfo::PASSED_USED && ref_VehItem->u8Vstate == VehicleInfo::PASSED_USED)
			{
				//收尾处理。
				//qDebug()<<"Vehicle accepted";
				VehModels2(ref_VehItem);
				delete ref_VehItem;
				m_Vehicle.removeOne(ref_VehItem);   
				continue;

			}

			if(ref_VehItem->locateX.u16xDis > 2800)
			{
				int32 t_midX = (ref_VehItem->locateX.u16xLeft + ref_VehItem->locateX.u16xRight)>>1;
				ref_VehItem->locateX.u16xLeft = t_midX - 1100;
				ref_VehItem->locateX.u16xRight  = t_midX + 1100;
				ref_VehItem->locateX.u16xDis = 2200;
			}

			ref_VehItem->VemptFrame++;
			ref_VehItem->IemptFrame++;
		}
	}

}


void TrafficAlg::VehModels2(VehicleInfo *pVeh)
{
	int32	l_lenLasters = 0,l_u32index = 0;
	qint64 l_vTimer = 0,l_ITimer = 0;

	int32 VehWide = 0,VehHeight = 0,VehSpeed =0,i = 0,VehLength=0,tmp1 = 0,tmp2 = 0,tmpFlag = 0;//%车宽
	//	int32  l_anSidePlanenessFlag[250];   //所有帧的侧面平整度标识	 20130416
	uint8 	VehPattern=0,l_u8Pos = 0; //,Not_keche = 0,jizhuangxiang = 0
	//	uint8   l_u8BigArgNum = 0;//,jizhuangxiang_count = 0
	//	uint16 l_u16MinHeight = 0;
	//	uint32  l_u32ArgHeight = 0;	   //平均高度值
	//	uint32  l_u32BigArgHeight = 0;  //均值以上的所有值的均值
	//	uint32  l_u32SmallArgHeight = 0;  //均值以下的所有值的均值
	uint32	Veh_Num = pVeh->Vdata.u16FrameCnt & FRAME_MASK;
	uint32	Inc_Num = pVeh->Incdata.u16FrameCnt & FRAME_MASK;
	int32  l_un32LaserLen1;

	//计算车高,车宽	
	tmp1 = 0;
	for(l_u32index = 0;l_u32index < Veh_Num;l_u32index++)
	{
		if(pVeh->ZLen < pVeh->Vdata.zMax[l_u32index] &&
			abs(pVeh->Vdata.zMax[l_u32index] - TrafficSettings::instance()->getLasterV().LmsHeight()) > 1000
			)
		{
			pVeh->ZLen = pVeh->Vdata.zMax[l_u32index];
		}

		if(pVeh->xLen < pVeh->Vdata.xdata[l_u32index])
			pVeh->xLen = pVeh->Vdata.xdata[l_u32index];	
	}

	//计算两激光器地面间距
	l_un32LaserLen1 = TrafficSettings::instance()->getLasterQ().LmsHeight() * TrafficSettings::instance()->getLasterQ().LmsHeight() -  (TrafficSettings::instance()->getLasterV().LmsHeight() + TrafficSettings::instance()->DisGap())*(TrafficSettings::instance()->getLasterV().LmsHeight() + TrafficSettings::instance()->DisGap());

#if 0
	l_un32LaserLen2 = sqrt(l_un32LaserLen1);

	for(l_u32index = 0;l_u32index < Inc_Num;l_u32index++)
	{
		if(l_lenLasters < pVeh->Incdata.ydata[l_u32index])
			l_lenLasters = pVeh->Incdata.ydata[l_u32index];
	} 
	if(l_lenLasters < 6000) 
	{
		l_lenLasters = l_un32LaserLen2 - 500;
	}	
#else

	l_lenLasters = sqrt((float)l_un32LaserLen1);
	//对车辆行驶距离进行修正
	int16 t_fixIndex = 0;
	if(pVeh->direct == VehicleInfo::DIRECT_V)
	{
		t_fixIndex = 0;
	}
	else
	{
		t_fixIndex = pVeh->Incdata.u16FrameCnt - 1;
	}

	tmp1 = 0;
	for(l_u32index = 1;l_u32index < pVeh->Incdata.zdata[t_fixIndex][0];l_u32index++)
	{
		if(tmp1 < pVeh->Incdata.zdata[t_fixIndex][l_u32index] && 
			abs(TrafficSettings::instance()->getLasterQ().LmsHeight() - pVeh->Incdata.zdata[t_fixIndex][l_u32index]) > 1000)
		{
			tmp1 = pVeh->Incdata.zdata[t_fixIndex][l_u32index];
		}
	}

	//if( abs(TrafficSettings::instance()->getLasterQ().LmsHeight() - tmp1) < 1000 || tmp1 < 500) //偶尔会有小点，不清楚原因
	if(!tmp1)
	{
		tmp1 = pVeh->ZLen ? pVeh->ZLen : 1500;
	}
	tmp1 = tmp1*TrafficSettings::instance()->getLasterV().LmsHeight()/TrafficSettings::instance()->getLasterQ().LmsHeight();


	tmp2 = l_lenLasters;
	l_lenLasters = l_lenLasters*(TrafficSettings::instance()->getLasterV().LmsHeight() + TrafficSettings::instance()->DisGap() - tmp1)/(TrafficSettings::instance()->getLasterV().LmsHeight() + TrafficSettings::instance()->DisGap());
	//if(l_lenLasters < (tmp2>>1))
	//{
	//	l_lenLasters = tmp2 - 1500;
	//}
#endif

	//计算车速
	l_vTimer  = pVeh->Vdata.tdata[0];
	l_ITimer  = pVeh->Incdata.tdata[0];
	if(l_vTimer != l_ITimer)
		pVeh->speed = (36 * l_lenLasters)/(10*abs(l_ITimer - l_vTimer)); //km/h
	else
		pVeh->speed = 50;
	//    printf("pVeh->speed=%d ,%d,%d, %d\n",pVeh->speed,l_lenLasters,l_ITimer,l_vTimer);

	VehSpeed = pVeh->speed ;
	//计算车长		
	l_vTimer = pVeh->Vdata.tdata[0];
	l_ITimer = pVeh->Vdata.tdata[Veh_Num - 1];
	pVeh->yLen = pVeh->speed * abs(l_ITimer - l_vTimer); 
	pVeh->yLen = pVeh->yLen*10/36;


	VehHeight = pVeh->ZLen;
	VehWide = pVeh->xLen;
	VehLength = pVeh->yLen;

#if 0
	//车道号
	if(TrafficSettings::instance()->LaneWide())
	{
		tmp1 = (pVeh->locateX.u16xLeft+pVeh->locateX.u16xRight)>>1;
		pVeh->u8Lane = abs(tmp1) /TrafficSettings::instance()->LaneWide();				  	
	}
#else
	//根据车辆中心点位置，计算车所在的车道
	if(TrafficSettings::instance()->LaneWide())
	{
		tmp1 = (pVeh->locateX.u16xLeft+pVeh->locateX.u16xRight)>>1;
		if(tmp1  < 0){

			if(TrafficSettings::instance()->getLasterV().LmsObstacleleft())
			{
				tmp1 -= g_leftXOffset;
			} 

			pVeh->u8Lane = abs(tmp1)/TrafficSettings::instance()->LaneWide() + 1;   
			pVeh->u8Lane +=(TrafficSettings::instance()->LaneNum()>>1);
			if(pVeh->u8Lane > TrafficSettings::instance()->LaneNum()){
				pVeh->u8Lane = TrafficSettings::instance()->LaneNum();
			}
		}else{

			if(TrafficSettings::instance()->getLasterV().LmsObstacleright())
			{
				tmp1 -= g_rightXOffset;
			} 
			pVeh->u8Lane = abs(tmp1)/TrafficSettings::instance()->LaneWide() + 1; 
			if(pVeh->u8Lane > (TrafficSettings::instance()->LaneNum()>>1)){
				pVeh->u8Lane = TrafficSettings::instance()->LaneNum()>>1;
			}    
		}					
	}
#endif


	//qDebug()<<"GetVehPattern start";
	//1中小客 2小型货车 3 大客车 4中型货车 5大型货车 6特大型货车 7集装箱车  8拖拉机 9摩托车
	VehPattern = GetVehPattern(pVeh);
	//qDebug()<<"GetVehPattern end";
	//进行车型过滤
	VehPattern = TrafficSettings::instance()->FilterTypes(VehPattern);

	//速度过滤
	pVeh->speed = TrafficSettings::instance()->FilterSpeed(pVeh->speed);


	pVeh->u16Type = VehPattern;

	pVeh->devID = TrafficSettings::instance()->devId();
#if 0
	pVeh->dayNumber = TrafficSettings::instance()->Count();
	/////////////////////////////////////////////////////////////////////////////////////////////
	TrafficSettings::instance()->setCountInc();
#else
	pVeh->dayNumber = VehicleInfo::Count();
#endif
	
#ifdef DEV_TEST
	submit_veh_height(pVeh);
#endif
	emit signal_veh(*pVeh);

}

uint16 TrafficAlg::GetVehPattern(VehicleInfo *pVeh)
{
	uint16 t_simpleType = simpleVehPattern(pVeh);
	if(t_simpleType != TYPE_UNKNOWN)
	{
		return t_simpleType;
	}

	int32 t_Max_v[FRAME_MAXCNT] = {0};
	int16 t_skCount = GetSkeletonMax_v(pVeh,t_Max_v); //车顶的轮廓
	if(t_skCount < 5)
	{
		return TYPE_XIAO_KE;
	}

	int32 t_sortMax_v[FRAME_MAXCNT] = {0};
	sortByDesc(t_Max_v,0,t_skCount,t_sortMax_v); //对非打飞点轮廓进行排序
	int32 t_diff = t_sortMax_v[0] - t_sortMax_v[t_skCount - 1];
	if(t_diff < HORIZONTAL_LIMIT) //最小与最大值较近
	{
		//非常有可能是小客、小货、中货、大客
		//低于1500情况，小货不常见
		if(t_sortMax_v[0] < 1500 && pVeh->yLen < 6000)
		{
			return TYPE_XIAO_KE;
		}

		//统计每帧中的打飞点百分比
		int32 t_UseLessPercent[FRAME_MAXCNT];
		GetUselessPercent(pVeh,t_UseLessPercent); 		

		//1、几乎每个帧都有打飞点，可推断为小客
		int16 t_UselessKick = 0;
		int t_index = 0;
		for(;t_index < pVeh->Vdata.u16FrameCnt;t_index++)
		{
			if(t_UseLessPercent[t_index])
			{
				t_UselessKick++;
			}
		}
		if(t_UselessKick > (pVeh->Vdata.u16FrameCnt>>1))
		{
			int32 t_meanZlen = GetMean(t_sortMax_v,0,t_skCount);
			if(t_meanZlen < 3000)
				return TYPE_XIAO_KE;
			else
				return TYPE_DA_KE;
		}

		//2、横向幅度相差较大，推断为小货的可能性高，
		int16 t_CrossKick = 0;
		int32 t_CrossBuf[FRAME_MAXCNT] = {0};
		int16 t_FrmIndex = 2;
		for(;t_FrmIndex<pVeh->Vdata.u16FrameCnt;t_FrmIndex++)
		{			
			int16 t_TopCount = GetTopCross_v(pVeh, t_FrmIndex ,t_CrossBuf);

			sortByDesc(t_CrossBuf,0,t_TopCount,t_sortMax_v);
			//去除两个最大值点和一个最小值点
			if(t_sortMax_v[2] - t_sortMax_v[t_TopCount - 2] > (HORIZONTAL_LIMIT<<1))
			{
				t_CrossKick++;					
			}
		}

		if(t_CrossKick > (pVeh->Vdata.u16FrameCnt>>2))
		{			
			if(pVeh->yLen > 5800)
			{				
				int16 t_DaKick = GetMaxThanValue(t_Max_v,t_skCount,3000);
				if(t_DaKick > (t_skCount>>1))
				{
					return TYPE_DA_HUO;
				}
				else
				{
					return TYPE_ZHONG_HUO;
				}
			}
			else
			{
				return TYPE_XIAO_HUO;
			}											
		}
		else if(t_CrossKick <= 2)
		{
			int32 t_meanZlen = GetMean(t_Max_v,0,t_skCount);
			if(t_meanZlen > 3000)
			{
				return TYPE_DA_KE;
			}
			else
			{
				return TYPE_XIAO_KE;
			}
		}


		if(pVeh->yLen < 6000)
		{
			if(t_Max_v[1] < 1600)
			{
				return TYPE_XIAO_KE;
			}
			else
			{
				return TYPE_XIAO_HUO;	
			}			 			
		}
		else
		{
			int16 t_DaKick = GetMaxThanValue(t_Max_v,t_skCount,3000);
			if(t_DaKick == t_skCount)
			{
				return TYPE_DA_KE;

			}
			else if(t_DaKick > (t_skCount>>1))
			{			 
				return TYPE_DA_HUO;
			}
			else
			{				 
				if(t_Max_v[0] > 2500)
				{
					return TYPE_ZHONG_HUO;
				}
				else 
				{
					return TYPE_XIAO_HUO;
				}				
			}

		}

	}
	//从轮廓上看不怎么平有 小货、中货、大货、特大货、集装箱
	if(pVeh->yLen > 12000)
	{
		//可能是特大货、或集装箱,大客

		int16 t_posIndex = GetBigDiffPos(t_Max_v,t_skCount);

		if(t_posIndex && abs(t_Max_v[t_posIndex] - t_Max_v[t_posIndex - 1]) < 1000)
		{
			uint16 tIsDaKe = CheckForDaKe(pVeh);
			if(TYPE_DA_KE == tIsDaKe)
			{
				return TYPE_DA_KE;
			}
		}

		int16 t_posIndex2 = t_posIndex;		 
		while(t_Max_v[t_posIndex2]< 2000)
		{
			t_posIndex2++;
			if(t_posIndex2 >= t_skCount)
			{
				t_posIndex2 = t_posIndex;
				break;
			}			
		}		
		sortByDesc(t_Max_v,t_posIndex2,t_skCount,t_sortMax_v);
		if(abs(t_sortMax_v[0] - t_sortMax_v[t_skCount - t_posIndex2 - 1]) > HORIZONTAL_LIMIT)
		{
			return TYPE_TE_DA_HUO;	
		}

		int32 t_HeaderMeanVal =  GetMean(t_Max_v,0,t_posIndex2);
		int32 t_TailMeanVal =  GetMean(t_Max_v,t_posIndex2,t_skCount);
		if(t_HeaderMeanVal - t_TailMeanVal > 0)
		{
			return TYPE_TE_DA_HUO;	
		}

		//集装箱
		if(t_sortMax_v[0] >= 3800 && t_sortMax_v[0] <= 4200)
		{
			return TYPE_JI_ZHUANG_XIANG;
		}
		else
		{
			return TYPE_TE_DA_HUO;
		}
	}
	else if(pVeh->yLen > 6000)
	{
		//可能是小货、中货、大货、大客
		//1、空货最容易判断
		uint16 tIsKongHuo =  CheckForKongHuo(t_Max_v,t_skCount);
		if(tIsKongHuo != TYPE_UNKNOWN)
		{
			return tIsKongHuo;
		}

		//2、统计飞点比重
		uint16 tIsDaKe = CheckForDaKe(pVeh);
		if(TYPE_UNKNOWN != tIsDaKe)
		{
			return tIsDaKe;
		}

		sortByDesc(t_Max_v,0,t_skCount,t_sortMax_v); //对非打飞点轮廓进行排序
		int32 t_diff01 = t_sortMax_v[0] - t_sortMax_v[t_skCount - 1];
		if(t_diff01 > 1000)
		{
			//一定是货车
			if(t_Max_v[1] < 2000 )
			{
				return TYPE_XIAO_HUO;
			} 
			else if(t_Max_v[1] < 2800)
			{
				return TYPE_ZHONG_HUO;
			}
			else
			{
				return TYPE_DA_HUO;
			}
		}

#if 0
		//2、横向幅度相差较大，推断为中、大的可能性高，
		int32 t_CrossDiff[FRAME_MAXCNT] = {0};
		int16 t_CrossKick = GetIrregularCrossOfTop(pVeh,t_CrossDiff);

		if(t_CrossKick <= 2)
		{		
			int32 t_meanZlen = GetMean(t_Max_v,0,t_skCount);
			if(t_meanZlen > 3000)
			{
				return TYPE_DA_KE;
			}							
		}
#endif
		int16 t_posIndex = GetBigDiffPos((int32*)(t_Max_v + 1),t_skCount -  1);			
		int32 t_HeaderMeanVal =  GetMean(t_Max_v,0,t_posIndex);
		int32 t_TailMeanVal =  GetMean(t_Max_v,t_posIndex,t_skCount);
		if(t_HeaderMeanVal < 2000 )
		{
			return TYPE_XIAO_HUO;
		} 
		else if(t_HeaderMeanVal < 2800)
		{
			return TYPE_ZHONG_HUO;
		}
		else
		{
			return TYPE_DA_HUO;
		}

	}
	else 
	{
		//1、两个邻近点差距最大的点位置在车头附近，车头均值比车尾均值高，空货
		uint16 tIsKongHuo =  CheckForKongHuo(t_Max_v,t_skCount);
		if(tIsKongHuo != TYPE_UNKNOWN)
		{
			return tIsKongHuo;
		}

		//2、横向幅度相差较大，推断为小货的可能性高，		 
		int32 t_CrossDiff[FRAME_MAXCNT] = {0};
		int16 t_CrossKick = GetIrregularCrossOfTop(pVeh,t_CrossDiff);

		if(t_CrossKick > (pVeh->Vdata.u16FrameCnt>>1))
		{	 //装有不规则货物时
			return TYPE_XIAO_HUO;
		}		 
		else if(t_CrossKick <= 2)
		{
			int32 t_meanZlen = GetMean(t_Max_v,0,t_skCount);
			if(t_meanZlen > 3000)
			{
				return TYPE_DA_KE;
			}	
			int16 tMaxPos = GetMaxPos(t_Max_v,t_skCount);
			if(t_Max_v[tMaxPos] < 1800)
			{
				return TYPE_XIAO_KE;
			}
		}

		//3、统计每帧中的打飞次数
		uint16 tIsDaKe = CheckForDaKe(pVeh);
		if(TYPE_UNKNOWN != tIsDaKe)
		{
			return tIsDaKe;
		}

		sortByDesc(t_Max_v,0,t_skCount,t_sortMax_v); //对非打飞点轮廓进行排序
		int32 t_diff02 = t_sortMax_v[0] - t_sortMax_v[t_skCount - 1];
		if(t_diff02 > (HORIZONTAL_LIMIT<<1))
		{			 
			//一定是货车
			if(t_Max_v[1] < 1000 )
			{
				return TYPE_XIAO_KE;
			} 
			else if(t_Max_v[1] < 2000 )
			{
				return TYPE_XIAO_HUO;
			} 
			else 
			{
				return TYPE_ZHONG_HUO;
			}			
		}

		//3、最小值点为第一个点，且高度不起过2.0m，认为是中小客
		sortByDesc(t_Max_v,0,t_skCount,t_sortMax_v);
		if(t_sortMax_v[0] - t_sortMax_v[t_skCount - 1] >1000)
		{			 
			if(t_sortMax_v[t_skCount - 2] < 2000)
				return TYPE_XIAO_HUO;
			else
				return TYPE_ZHONG_HUO;
		}

		//4、车头高于2.5m，中货
		if(t_Max_v[0] > 2800)
		{
			return TYPE_ZHONG_HUO;
		}

		//最小值点不在起点和终点，可能为小货的概率较大	  
		int32 t_diffSumInc = 0,t_diffSumDec = 0;
		int16 t_Inckick = 0;
		int16 t_sIndex = 1;
		for(;t_sIndex < t_skCount;t_sIndex++)
		{
			int32 t_diff = t_Max_v[t_sIndex] - t_Max_v[t_sIndex - 1];
			if(t_diff > HORIZONTAL_LIMIT)
			{				  
				t_diffSumInc += t_diff;
				t_Inckick ++;
			}
			else if(t_diff < (-1)*HORIZONTAL_LIMIT)
			{
				t_diffSumDec += t_diff;
				t_Inckick ++;
			}
		}

		if(t_diffSumInc + t_diffSumDec < 0 || t_Inckick > 2 )
		{	
			return TYPE_XIAO_HUO;
		}
		else
		{
			return TYPE_XIAO_KE;
		}

	}
}

uint16 TrafficAlg::simpleVehPattern(VehicleInfo *pVeh)
{
	int16 t_vIndex = 0,t_vFrameUseless =  0;
	for(t_vIndex = 0;t_vIndex < pVeh->Vdata.u16FrameCnt;t_vIndex++)
	{
		//统计每帧的不用点信息量
		int16 t_vPtCount = pVeh->Vdata.zdata[t_vIndex][0];
		int16 t_vUselessCount = 0;
		int16 t_vPtIndex = 1;
		for(;t_vPtIndex < t_vPtCount;t_vPtIndex++)
		{
			if(abs(pVeh->Vdata.zdata[t_vIndex][t_vPtIndex] - TrafficSettings::instance()->getLasterV().LmsHeight()) < 1000)
			{
				t_vUselessCount++;
			}	
		}

		if((t_vUselessCount*100/t_vPtCount) > 40)
		{
			//超过60%的点都是打非点
			t_vFrameUseless++;
		}
	}

	if((t_vFrameUseless*100/pVeh->Vdata.u16FrameCnt) > 60)
	{		
		return TYPE_XIAO_KE; //小客
	}
	else if(pVeh->Vdata.u16FrameCnt <= 5 )
	{
		//有些计算异常情况发生
		//if(pVeh->yLen > 6000)
		//{
		//	pVeh->yLen = 3636;
		//}

		//if(pVeh->ZLen < 100)
		//{
		//	pVeh->ZLen = 1616;
		//}

		if(pVeh->xLen < 1100)
			return TYPE_MO_TUO; //摩的
		else 
			return TYPE_XIAO_KE;
	}else 
	{
		return TYPE_UNKNOWN;
	}
}

int16 TrafficAlg::GetSkeletonMax_v(VehicleInfo *pVeh,int32 *pOutData)
{
	int16 t_outIndex = 0;	

#if 0
	int16 t_vIndex = 0;
	for(t_vIndex = 0;t_vIndex < pVeh->Vdata.u16FrameCnt;t_vIndex++)
	{
		int16 t_vPtCount = pVeh->Vdata.zdata[t_vIndex][0];

		int16 t_roofStartPt = 1;						

		int32 t_vMax = 0;
		for(;t_roofStartPt < t_vPtCount;t_roofStartPt++)
		{
			if(abs(pVeh->Vdata.zdata[t_vIndex][t_roofStartPt] - TrafficSettings::instance()->getLasterV().LmsHeight()) < 1000)
			{
				continue;
			}

			if(pVeh->Vdata.zdata[t_vIndex][t_roofStartPt] > t_vMax)
			{
				t_vMax = pVeh->Vdata.zdata[t_vIndex][t_roofStartPt];
			}				

		}

		//存在找不到的情况
		if(t_vMax)
		{
			pOutData[t_outIndex++] = t_vMax;
		}	

	}
#else
	//平顶区域的起止点位置
	int16 t_roofStartPt = 0;	
	int16 t_roofEndPt = 0;

	//计算车辆经过的哪侧 	
	int16 t_passMidPt = (pVeh->locateX.u16Leftpt + pVeh->locateX.u16Rightpt)>>1;
	if(t_passMidPt < TrafficSettings::instance()->getLasterV().LmsPtMid())
	{
		//在左侧经过
		int16 t_vIndex = 0;
		for(t_vIndex = 0;t_vIndex < pVeh->Vdata.u16FrameCnt;t_vIndex++)
		{
			int16 t_vPtCount = pVeh->Vdata.zdata[t_vIndex][0];

			//计算平顶的区域百分比
			int16 t_roofPercent = 0;
			if(pVeh->Vdata.zMax[t_vIndex])
			{
				t_roofPercent = pVeh->xLen*100/(pVeh->Vdata.zMax[t_vIndex] + pVeh->xLen);
			}
			else
			{
				t_roofPercent = pVeh->xLen*100/(pVeh->ZLen + pVeh->xLen);
			}
			//平顶区域的起止点位置
			int16 t_roofStartPt = 1;	
			int16 t_roofEndPt = t_roofPercent * t_vPtCount /100;

			//对平顶区域进行排序
			int32 t_vMax = 0,t_useless = 0;
			for(;t_roofStartPt < t_roofEndPt;t_roofStartPt++)
			{
				if(abs(pVeh->Vdata.zdata[t_vIndex][t_roofStartPt] - TrafficSettings::instance()->getLasterV().LmsHeight()) < 1000)
				{
					t_useless++;
					continue;
				}

				if(pVeh->Vdata.zdata[t_vIndex][t_roofStartPt] > t_vMax)
				{
					t_vMax = pVeh->Vdata.zdata[t_vIndex][t_roofStartPt];
				}				

			}

			//存在找不到的情况
			if(t_vMax > 500)
			{
				pOutData[t_outIndex++] = t_vMax;
			}	

		}
	}
	else
	{
		//在右侧经过
		int16 t_vIndex = 0;
		for(t_vIndex = 0;t_vIndex < pVeh->Vdata.u16FrameCnt;t_vIndex++)
		{
			int16 t_vPtCount = pVeh->Vdata.zdata[t_vIndex][0];

			int16 t_roofPercent = 0;
			if(pVeh->Vdata.zMax[t_vIndex])
			{
				t_roofPercent = pVeh->xLen*100/(pVeh->Vdata.zMax[t_vIndex] + pVeh->xLen);
			}
			else
			{
				t_roofPercent = pVeh->xLen*100/(pVeh->ZLen + pVeh->xLen);
			}
			//平顶区域的起止点位置
			int16 t_roofStartPt = t_roofPercent * t_vPtCount /100;

			int16 t_roofEndPt = t_vPtCount;

			int16 t_count = t_roofEndPt - t_roofStartPt;
			//对平顶区域进行排序
			int32 t_vMax = 0,t_useless = 0;
			for(;t_roofStartPt < t_roofEndPt;t_roofStartPt++)
			{
				if(abs(pVeh->Vdata.zdata[t_vIndex][t_roofStartPt] - TrafficSettings::instance()->getLasterV().LmsHeight()) < 1000)
				{
					t_useless++;
					continue;
				}

				if(pVeh->Vdata.zdata[t_vIndex][t_roofStartPt] > t_vMax )
				{
					t_vMax = pVeh->Vdata.zdata[t_vIndex][t_roofStartPt];
				}				

			}

			//存在找不到的情况
			if(t_vMax > 500)
			{
				pOutData[t_outIndex++] = t_vMax;
			}	

		}
	}
#endif
	return t_outIndex;
}


uint16 TrafficAlg::sortByDesc(int32 *pArray,int32 nStartIndex,int32 nStopIndex,int32 *pOutData)
{	
	//插入排序
	int32 t_Counts = nStopIndex - nStartIndex;
	int32 t_index = 0;
	for(t_index = 0; t_index < t_Counts; t_index++)
	{
		pOutData[t_index] = pArray[t_index + nStartIndex];
	}

	for(t_index = 0; t_index < t_Counts; t_index++)
	{		
		int32 t_Pos = t_index + 1;
		for(; t_Pos < t_Counts; t_Pos++)
		{
			if( pOutData[t_index] < pOutData[t_Pos])
			{
				int32 t_copy = pOutData[t_index];
				pOutData[t_index] = pOutData[t_Pos];
				pOutData[t_Pos] = t_copy;				
			}
		}
	}
	return 0;
}

void TrafficAlg::GetUselessPercent(VehicleInfo *pVeh,int32 *pOutData)
{
	int16 t_vIndex = 0;
	for(t_vIndex = 0;t_vIndex < pVeh->Vdata.u16FrameCnt;t_vIndex++)
	{
		//统计每帧的不用点信息量
		int16 t_vPtCount = pVeh->Vdata.zdata[t_vIndex][0];
		int16 t_vUselessCount = 0;
		int16 t_vPtIndex = 1;
		for(;t_vPtIndex < t_vPtCount;t_vPtIndex++)
		{
			if(abs(pVeh->Vdata.zdata[t_vIndex][t_vPtIndex] - TrafficSettings::instance()->getLasterV().LmsHeight()) < 1000)
			{
				t_vUselessCount++;
			}		
		}

		pOutData[t_vIndex] = t_vUselessCount*100/t_vPtCount;		
	}
}

int32 TrafficAlg::GetMean(int32 * pArray,int16 nStartIndex,int16 nStopIndex)
{
	int32 t_ret = 0;
	int16 t_pos = nStartIndex;
	for(;t_pos<nStopIndex;t_pos++)
	{
		t_ret += pArray[t_pos];		
	}

	if(nStopIndex == nStartIndex)
	{
		t_ret = pArray[nStartIndex];
	}
	else
	{	
		t_ret = t_ret/(nStopIndex - nStartIndex);
	}
	return t_ret;	
}

int16 TrafficAlg::GetTopCross_v(VehicleInfo *pVeh,int16 nFrameIndex,int32 *pOutData)
{
	int16 t_outIndex = 0;	

	//平顶区域的起止点位置
	int16 t_roofStartPt = 0;	
	int16 t_roofEndPt = 0;

	//计算车辆经过的哪侧 	
	int16 t_passMidPt = (pVeh->locateX.u16Leftpt + pVeh->locateX.u16Rightpt)>>1;
	if(t_passMidPt < TrafficSettings::instance()->getLasterV().LmsPtMid())
	{
		//在左侧经过
		int16 t_vIndex = nFrameIndex;

		int16 t_vPtCount = pVeh->Vdata.zdata[t_vIndex][0];

		//计算平顶的区域百分比
		int16 t_roofPercent = 0;
		if(pVeh->Vdata.zMax[t_vIndex])
		{
			t_roofPercent = pVeh->xLen*100/(pVeh->Vdata.zMax[t_vIndex] + pVeh->xLen);
		}
		else
		{
			t_roofPercent = pVeh->xLen*100/(pVeh->ZLen + pVeh->xLen);
		}
		//平顶区域的起止点位置
		int16 t_roofStartPt = 1;	
		int16 t_roofEndPt = t_roofPercent * t_vPtCount /100;
		t_roofEndPt = 90*t_roofEndPt/100;
		//平顶区域		
		for(;t_roofStartPt < t_roofEndPt;t_roofStartPt++)
		{
			if(abs(pVeh->Vdata.zdata[t_vIndex][t_roofStartPt] - TrafficSettings::instance()->getLasterV().LmsHeight()) < 1000)
			{
				continue;
			}

			if(pVeh->Vdata.zdata[t_vIndex][t_roofStartPt] > 500) /*可能存在打飞或分车时，错误包含一些小点*/
			{
				pOutData[t_outIndex++] = pVeh->Vdata.zdata[t_vIndex][t_roofStartPt];
			}				

		}			
	}
	else
	{
		//在右侧经过
		int16 t_vIndex = nFrameIndex;
		int16 t_vPtCount = pVeh->Vdata.zdata[t_vIndex][0];

		int16 t_roofPercent = 0;
		if(pVeh->Vdata.zMax[t_vIndex])
		{
			t_roofPercent = pVeh->Vdata.zMax[t_vIndex]*100/(pVeh->Vdata.zMax[t_vIndex] + pVeh->xLen);
		}
		else
		{
			t_roofPercent = pVeh->ZLen*100/(pVeh->ZLen + pVeh->xLen);
		}
		//平顶区域的起止点位置
		int16 t_roofStartPt = t_roofPercent * t_vPtCount /100;
		t_roofStartPt = t_roofStartPt*110/100;
		int16 t_roofEndPt = t_vPtCount;

		//平顶区域		
		for(;t_roofStartPt < t_roofEndPt;t_roofStartPt++)
		{
			if(abs(pVeh->Vdata.zdata[t_vIndex][t_roofStartPt] - TrafficSettings::instance()->getLasterV().LmsHeight()) < 1000)
			{
				continue;
			}

			if(pVeh->Vdata.zdata[t_vIndex][t_roofStartPt] > 500) /*可能存在打飞或分车时，错误包含一些小点*/
			{
				pOutData[t_outIndex++] = pVeh->Vdata.zdata[t_vIndex][t_roofStartPt];
			}				

		}		
	}

	return t_outIndex;
}

int16 TrafficAlg::GetMaxThanValue(int32 *pArray,int nlen,int32 nVal)
{
	int16 retCount = 0;
	int16 t_pos = 0;
	for(;t_pos<nlen;t_pos++)
	{
		if(pArray[t_pos] > nVal)
		{
			retCount++;
		}
	}
	return retCount;
}

int16 TrafficAlg::GetBigDiffPos(int32 *pArray,int32 nlen)
{
	int16 retPos = 0;
	int32 t_MaxDiff = 0;
	int16 t_pos = 1;
	for(;t_pos<nlen;t_pos++)
	{
		if(abs(pArray[t_pos] - pArray[t_pos - 1]) > t_MaxDiff)
		{
			t_MaxDiff = abs(pArray[t_pos] - pArray[t_pos - 1]);
			retPos = t_pos;
		}
	}
	return retPos;
}

uint16 TrafficAlg::CheckForDaKe(VehicleInfo *pVeh)
{
	int32 t_UseLessPercent[FRAME_MAXCNT];
	GetUselessPercent(pVeh,t_UseLessPercent); 
	int16 t_UselessKick = 0;
	int32 t_sum = 0,t_avg = 0;
	int t_index = 0;
	for(;t_index < pVeh->Vdata.u16FrameCnt;t_index++)
	{
		t_sum += t_UseLessPercent[t_index];

		if(t_UseLessPercent[t_index] > 10)
		{
			t_UselessKick++;
		}
	}
	t_avg = t_sum/pVeh->Vdata.u16FrameCnt;

	int32 t_Max_v[FRAME_MAXCNT] = {0};
	if(t_UselessKick >= (pVeh->Vdata.u16FrameCnt>>1) || t_avg > 15)
	{		
		int16 t_skCount = GetSkeletonMax_v(pVeh,t_Max_v); //车顶的轮廓**
		int32 t_meanZlen = GetMean(t_Max_v,0,t_skCount);
		if(t_meanZlen < 3000 /*&& pVeh->yLen < 6000*/)
			return TYPE_XIAO_KE;
		else
			return TYPE_DA_KE;	
	}		
	else if(t_UselessKick)
	{
		sortByDesc(t_UseLessPercent,0,pVeh->Vdata.u16FrameCnt,t_Max_v);
		if(t_Max_v[0] > 50)
		{
			int16 t_skCount = GetSkeletonMax_v(pVeh,t_Max_v); //车顶的轮廓**
			int32 t_meanZlen = GetMean(t_Max_v,0,t_skCount);
			if(t_meanZlen < 3000 && pVeh->yLen < 6000)
				return TYPE_XIAO_KE;
			else
				return TYPE_DA_KE;			
		}
		else
		{
			return TYPE_UNKNOWN;
		}
	}
	else
	{
		return TYPE_UNKNOWN;
	}
}

uint16 TrafficAlg::CheckForKongHuo(int32 *pArray,int32 nCounts)
{
	int16 t_posIndex = GetBigDiffPos((int32*)(pArray + 1),nCounts -  1);
	int32 t_HeaderMeanVal =  GetMean(pArray,1,t_posIndex);
	int32 t_TailMeanVal =  GetMean(pArray,t_posIndex,nCounts);
	if(t_HeaderMeanVal  - t_TailMeanVal > (HORIZONTAL_LIMIT<<1) ||
		pArray[t_posIndex] - t_TailMeanVal > (HORIZONTAL_LIMIT<<1))
	{
		if(t_HeaderMeanVal < 2000)
		{
			return TYPE_XIAO_HUO;
		}
		else
		{
			return TYPE_ZHONG_HUO;
		}
	}	
	else
	{
		return TYPE_UNKNOWN;
	}
}

//uint16 TrafficAlg::CheckForDaKe(VehicleInfo *pVeh)
//{
//	int32 t_UseLessPercent[FRAME_MAXCNT];
//	GetUselessPercent(pVeh,t_UseLessPercent); 
//	int16 t_UselessKick = 0;
//	int32 t_sum = 0,t_avg = 0;
//int t_index = 0;
//	for(;t_index < pVeh->Vdata.u16FrameCnt;t_index++)
//	{
//		t_sum += t_UseLessPercent[t_index];
//
//		if(t_UseLessPercent[t_index] > 10)
//		{
//			t_UselessKick++;
//		}
//	}
//	t_avg = t_sum/pVeh->Vdata.u16FrameCnt;
//	
//	 int32 t_Max_v[FRAME_MAXCNT] = {0};
//	if(t_UselessKick >= (pVeh->Vdata.u16FrameCnt>>1) || t_avg > 15)
//	{		
//		int16 t_skCount = GetSkeletonMax_v(pVeh,t_Max_v); //车顶的轮廓**
//		int32 t_meanZlen = GetMean(t_Max_v,0,t_skCount);
//		if(t_meanZlen < 3000 /*&& pVeh->yLen < 6000*/)
//			return TYPE_XIAO_KE;
//		else
//			return TYPE_DA_KE;	
//	}		
//	else if(t_UselessKick)
//	{
//		sortByDesc(t_UseLessPercent,0,pVeh->Vdata.u16FrameCnt,t_Max_v);
//		if(t_Max_v[0] > 50)
//		{
//			int16 t_skCount = GetSkeletonMax_v(pVeh,t_Max_v); //车顶的轮廓**
//			int32 t_meanZlen = GetMean(t_Max_v,0,t_skCount);
//			if(t_meanZlen < 3000 && pVeh->yLen < 6000)
//				return TYPE_XIAO_KE;
//			else
//				return TYPE_DA_KE;			
//		}
//		else
//		{
//			return TYPE_UNKNOWN;
//		}
//	}
//	else
//	{
//		return TYPE_UNKNOWN;
//	}
//}

int16 TrafficAlg::GetIrregularCrossOfTop(VehicleInfo *pVeh,int32 *pOutArray)
{
	int16 t_CrossKick = 0;
	int32 t_CrossData[FRAME_MAXCNT] = {0};
	int32 t_sort_v[FRAME_MAXCNT] = {0};
	int32 t_diff = 0;
	int16 t_FrmIndex = 2;
	for(;t_FrmIndex<pVeh->Vdata.u16FrameCnt;t_FrmIndex++)
	{			
		int16 t_TopCount = GetTopCross_v(pVeh, t_FrmIndex ,t_CrossData);
		if(t_TopCount > 10)
		{
			sortByDesc(t_CrossData,0,t_TopCount,t_sort_v);
			//去除两个最大值点和一个最小值点
			t_diff = t_sort_v[2] - t_sort_v[t_TopCount - 2];
			if(t_diff > (HORIZONTAL_LIMIT<<1))
			{
				pOutArray[t_CrossKick++] = t_diff;									
			}
		}		
	}
	return t_CrossKick;
}

uint16 TrafficAlg::GetPosFromXDistance(const QList<int32> & xDistent,uint16 start,uint16 end,int32 targetX/*wzf*/)
{
	uint16 ret = end,Index = start;
	uint16 l_16tmpValue = end;//SETUPALIAS.u16EndPtNum - SETUPALIAS.u16StartPtNum;
	uint16 l_minValue = 0xFFFF;

	while(Index < l_16tmpValue)
	{
		if(abs(xDistent[Index] - targetX) < l_minValue)
		{
			l_minValue = abs(xDistent[Index] - targetX);
			ret = Index;
		}
		//		else
		Index++;
	}

	return 	ret;
}

uint16 TrafficAlg::correctPt(const QList<int32>  &g_Zdist,uint16 *left,uint16 *right,LasterSettings *pLasterParm)
{
	uint16 midPt = (pLasterParm->LmsPtMid());
	int32 l_maxDis = 0,l_maxPt = 0;
	uint16 index = 0;

	if (*left > midPt)
	{
		for (index = *left + (*right - *left)*3/4;index < *right; index++)
		{
			if((g_Zdist[index] < 7000) && (g_Zdist[index - 1] < 7000) && (abs(g_Zdist[index]  - g_Zdist[index - 1]) > l_maxDis) )
			{
				l_maxDis = abs(g_Zdist[index]  - g_Zdist[index - 1]);
				l_maxPt = index - 1;
			}
		}

		if (l_maxPt && (l_maxDis > 1000))
			*right = l_maxPt;
	}
	else
	{
		for (index = *left + (*right - *left)/4;index > *left; index--)
		{
			if((g_Zdist[index] < 7000) && (g_Zdist[index + 1] < 7000) && (abs(g_Zdist[index]  - g_Zdist[index + 1]) > l_maxDis) )
			{
				l_maxDis = abs(g_Zdist[index]  - g_Zdist[index + 1]);
				l_maxPt = index + 1;
			}
		}

		if (l_maxPt)
			*left = l_maxPt;
	}
	return 1;
	//	return pt;
}

int32 TrafficAlg::GetMaxValue(const QList<int32> &pData,uint32 start,uint32 end)
{
	uint32 index;
	int32 retMax = pData[start];
	for(index = start+1;index<end;index++)
	{
		if(pData[index] > retMax)
			retMax = pData[index];
	}
	return retMax;
}

void TrafficAlg::clearVehicleErr()
{
	QListIterator<VehicleInfo*> iEveryItem(m_Vehicle);
	while(iEveryItem.hasNext())
	{	
		VehicleInfo* const & ref_item = iEveryItem.next();

		if (ref_item->u8Vstate == VehicleInfo::PASSED_USED  || ref_item->u8Istate == VehicleInfo::PASSED_USED)
		{
			if (ref_item->Vdata.u16FrameCnt< NORMAL_MAX_EMPTYFRAME && ref_item->Incdata.u16FrameCnt < NORMAL_MAX_EMPTYFRAME)
			{
				m_Vehicle.removeOne(ref_item);
				delete ref_item;				
			}
		}
	}	
}
int16 TrafficAlg::GetMaxPos(int32 *pArray,int16 nlen)
{
	int16 retPos = 0;
	int32 t_max = 0;
	int16 t_pos = 0;
	for(;t_pos<nlen;t_pos++)
	{
		if(pArray[t_pos] > t_max)
		{
			t_max = pArray[t_pos];
			retPos = t_pos;
		}
	}

	return retPos;
}

void TrafficAlg::copy(int32 *pArray,QList<qint32>::const_iterator & iStart,QList<qint32>::const_iterator & iEnd)
{
	qint32 nIndex = 0;	
	while(iStart != iEnd){		
		pArray[nIndex++] = *iStart;
		iStart++;
	}
}

#include "devprotocol.h"
void TrafficAlg::submit_veh_height(VehicleInfo *pVeh)
{
	qint32 cnt = pVeh->Vdata.u16FrameCnt;
	qint32 tIndex = 0;
	QList<qint32> tHeights;
	while(tIndex  < cnt){
		tHeights.append(pVeh->Vdata.zMax[tIndex]);
		tIndex++;
	}

	QByteArray _bufer;
	DevProtocol::serialize(tHeights,_bufer);

	emit sig_dbg_veh_height(_bufer);
}