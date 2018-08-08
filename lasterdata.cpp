#include "lasterdata.h"
#include "virtualbackground.h"
#include "../mathhelper.h"
#include <QDebug>

#define IS_ZERO(x)	 ( x < 300)

LasterData::LasterData(qint64 nTime,const QList<qint32> & nData)
{
	m_left = m_right = 0;
	m_zdata.clear();
	m_xdata.clear();
	//m_timestamp = nTime/1000;
	m_timestamp = nTime;
	m_data = nData;

	m_scale = 1.0;

	qint32 nIndex = 0;
	while(nIndex < POINT_SUM){	
		m_zdata.append(0);
		m_xdata.append(0);
		nIndex++;
	}
	
}

LasterData::LasterData(const LasterData & other)
{
	m_left = other.m_left;
	m_right = other.m_right;
	m_zdata = other.m_zdata;
	m_xdata = other.m_xdata;
	m_timestamp = other.m_timestamp;
	m_data = other.m_data;
	m_depends = other.m_depends;
	m_scale = other.m_scale;
}

LasterData::LasterData()
{
	m_left = m_right = 0;
	m_zdata.clear();
	m_xdata.clear();
	m_timestamp = 0;
	m_scale = 1.0;
	qint32 nIndex = 0;
	while(nIndex < POINT_SUM){	
		m_zdata.append(0);
		m_xdata.append(0);
		m_data.append(0);
		nIndex++;
	}
}
LasterData::~LasterData()
{

}

qint64 LasterData::timestamp() const
{
	return m_timestamp;
}

void LasterData::setTimestamp(qint64 nTimestamp)
{
	m_timestamp = nTimestamp;
}

const QList<qint32> & LasterData::data() const
{
	return m_data;
}
const QList<qint32> & LasterData::zdata() const
{
	return m_zdata;
}

void LasterData::setScale(qreal nscale)
{
	m_scale = nscale;

	qint32 tCount = m_zdata.size();
	qint32 tIndex = 0;
	while(tIndex < tCount){
		m_zdata[tIndex] *= nscale;
		tIndex++;
	}
}

const QList<qint32> & LasterData::xdata() const
{
	return m_xdata;
}
const qint32 LasterData::operator[](qint32 t_index) const
{
	return m_data.at(t_index);
}

quint16 LasterData::GetLimitValueLeft(const LasterSettings &nSettings)
{
	qint16 ret = nSettings.LmsPtStart();
	int l_midPt = nSettings.LmsPtMid();
	while(ret > 0)
	{
		if(m_zdata.at(ret) > ThresVehLow)
		{
			ret--;
		}
		else
		{
			break;
		}
	}
	return ret;
}
quint16 LasterData::GetLimitValueRight(const LasterSettings & nSettings)
{
	qint16 ret = nSettings.LmsPtStop();
	int l_midPt = nSettings.LmsPtMid();
	while(ret < m_zdata.size())
	{
		if(m_zdata.at(ret) > ThresVehLow)
		{
			ret++;
		}
		else
		{
			break;
		}
	}

	return ret;
}

quint16 LasterData::GetLeft()
{
	return m_left;
}

quint16 LasterData::GetRight()
{
	return m_right;
}

const LasterSettings & LasterData::settings() const
{
	return m_depends;
}
#if 0
//分情况：
//1、背景与测量值都正常，可获得高度、X位移
//2、背景打飞，测量值正常，通过激光器高度值可获得高度、X位移
//3、背景正常，测量值打飞，信息丢失，高度为激光器高度，x位移为0
//4、背景与测量值都打飞，不可识别
void LasterData::splitArea(const LasterSettings & pLasterParm,VirtualBackGround *ptBaseArry,PointSet *pOutArea)
{

	int32 i = 0,l_leftPt =0,l_rightPt = 0,l_leftPt_absV =0,l_rightPt_absV= 0,l_32tmp = 0;
	qint16 l_u16index = 0,TempVaule1 =0,l_u16tmp =0;

	m_depends = pLasterParm;
	bool hasExclude = (pLasterParm.LmsObstacleleft() + pLasterParm.LmsObstacleright() > 0);
	//求高度值
	for(i = 0;i<POINT_SUM;i++)
	{
		if(hasExclude && i > pLasterParm.LmsObstacleleft() && i< pLasterParm.LmsObstacleright())
		{
			m_zdata[i] = m_xdata[i] = 0;
			continue;
		}


		TempVaule1 =  abs(i - pLasterParm.LmsPtMid());
		if(TempVaule1 <= 180)
		{
			m_zdata[i] = MathHelper::Cos(abs((*ptBaseArry)[i] - m_data[i]),TempVaule1);  
				

			if(!(*ptBaseArry)[i] && m_data[i])
			{ //背景打飞过车时不打飞情况，可以获得真实高度
				m_zdata[i] = abs(pLasterParm.LmsHeight() - m_zdata[i]);
			}
			
			//m_zdata[i] *=m_scale;

			m_xdata[i] = MathHelper::Sin( m_data[i],TempVaule1);
			m_xdata[i] *= (i < pLasterParm.LmsPtMid() ? -1 : 1);    
		}
		else
		{
			m_zdata[i] = m_xdata[i] = 0;
		}
	}
	
	bool bprint = false;
	if(bprint){
		qDebug()<<m_zdata;
	}

	//帧数据进行分有车区域,记录每个区域的左右位置信息
	m_left = l_leftPt = this->GetLimitValueLeft(pLasterParm);
	m_right = l_rightPt = this->GetLimitValueRight(pLasterParm);;
	l_leftPt_absV = l_leftPt;
	l_rightPt_absV = l_rightPt;

	l_u16index = pOutArea->u8Sum = 0;
	for(i = l_leftPt;i<l_rightPt;i++)
	{
		if((m_zdata[i] > ThresVehLow) )//	&& (g_ZdistanceV[i] < ThresVehHigh)
		{     
			if(!pOutArea->uValid[l_u16index])
			{
				pOutArea->u8Sum = ( pOutArea->u8Sum+1)&POINTSET_MASK;

				pOutArea->uValid[l_u16index] = 1;
				//针对打飞的点
				//if(abs(m_xdata[i]) <FLY_LIMIT_VALUE)
				if(IS_FLY(m_zdata[i],pLasterParm.LmsHeight()))
				{
					pOutArea->Ptdata[l_u16index].u16xMaxHt = 0; //给0
				}         
				else
				{							
					pOutArea->Ptdata[l_u16index].u16xMaxHt = m_zdata[i];				
				}                			

				pOutArea->Ptdata[l_u16index].u16xRight = pOutArea->Ptdata[l_u16index].u16xLeft = m_xdata[i];

				pOutArea->Ptdata[l_u16index].u16Leftpt  = i;
				pOutArea->Ptdata[l_u16index].u16Rightpt = i;
				pOutArea->Ptdata[l_u16index].u16xDis = 0;

			}
			else
			{
				pOutArea->Ptdata[l_u16index].u16xRight = m_xdata[i];
				pOutArea->Ptdata[l_u16index].u16Rightpt = i;
				// pOutArea->Ptdata[l_u16index].u16xDis = abs(m_xdata[i] -  pOutArea->Ptdata[l_u16index].u16xLeft);
				//寻找非打飞点的最大值，有可能找不到
				l_32tmp =  pOutArea->Ptdata[l_u16index].u16xMaxHt;

				if(m_zdata[i] > l_32tmp && !IS_FLY(m_zdata[i],pLasterParm.LmsHeight()))
				{
					pOutArea->Ptdata[l_u16index].u16xMaxHt = m_zdata[i];
				}
			}
		}
		else if(pOutArea->uValid[l_u16index])
		{
			l_u16tmp =  pOutArea->Ptdata[l_u16index].u16Rightpt;	
			if( i - l_u16tmp > 5)
			{ //实际行车概率性问题

				qint8 t_WaitFix = 0;
				//针对打飞的点
				l_u16tmp =  pOutArea->Ptdata[l_u16index].u16Leftpt;
				if(IS_FLY(m_zdata[l_u16tmp],pLasterParm.LmsHeight()))
				{
					if(l_u16tmp < pLasterParm.LmsPtMid()){					
						//左侧点修正
						if(pOutArea->Ptdata[l_u16index].u16xMaxHt){
							pOutArea->Ptdata[l_u16index].u16xLeft = MathHelper::Tan(pLasterParm.LmsHeight() - pOutArea->Ptdata[l_u16index].u16xMaxHt,pLasterParm.LmsPtMid() - l_u16tmp);
							pOutArea->Ptdata[l_u16index].u16xLeft *=(-1);
						}else{
							qint32 raw_x = 0;
							if(!IS_ZERO(m_data[l_u16tmp - 1])){
								raw_x = m_xdata[l_u16tmp - 1];
								pOutArea->Ptdata[l_u16index].u16xLeft = raw_x*17/20;
							}else{
								pOutArea->Ptdata[l_u16index].u16xLeft = (-1)*MathHelper::Tan(pLasterParm.LmsHeight() - 1600,pLasterParm.LmsPtMid() - l_u16tmp);
							}						
						}
					} else {
						//右侧点修正
						pOutArea->Ptdata[l_u16index].u16xLeft = m_xdata[l_u16tmp - 1];
					}	

				}

				l_u16tmp =  pOutArea->Ptdata[l_u16index].u16Rightpt;
				if(IS_FLY(m_zdata[l_u16tmp],pLasterParm.LmsHeight()))
				{
					if(l_u16tmp > pLasterParm.LmsPtMid()){					
						//右侧点修正
						if(pOutArea->Ptdata[l_u16index].u16xMaxHt){
							pOutArea->Ptdata[l_u16index].u16xRight = MathHelper::Tan(pLasterParm.LmsHeight() - pOutArea->Ptdata[l_u16index].u16xMaxHt,pLasterParm.LmsPtMid() - l_u16tmp);							
						}else{
							qint32 raw_x = 0;
							if(!IS_ZERO(m_data[l_u16tmp + 1])){
								raw_x = m_xdata[l_u16tmp + 1];
								pOutArea->Ptdata[l_u16index].u16xRight = raw_x*17/20;
							}else{
								pOutArea->Ptdata[l_u16index].u16xRight = MathHelper::Tan(pLasterParm.LmsHeight() - 1600,pLasterParm.LmsPtMid() - l_u16tmp);
							}						
						}
					}else{
						//左侧点修正
						pOutArea->Ptdata[l_u16index].u16xRight = m_xdata[l_u16tmp + 1];
					}
				}

				l_u16tmp =  pOutArea->Ptdata[l_u16index].u16Rightpt - pOutArea->Ptdata[l_u16index].u16Leftpt;
				if(l_u16tmp < 5)
				{
					pOutArea->Ptdata[l_u16index].u16xLeft = pOutArea->Ptdata[l_u16index].u16xRight = -1;
				}


				l_u16tmp = abs( pOutArea->Ptdata[l_u16index].u16xLeft -  pOutArea->Ptdata[l_u16index].u16xRight);
				pOutArea->Ptdata[l_u16index].u16xDis = l_u16tmp;

				if(l_u16tmp < SMALL_AREA)	  //有效宽度小 ,认为是无效区域								
				{
					//对于在车道边缘，由于测距限制，仅能获得侧面信息，人为对其进行修正
					l_u16tmp =  pOutArea->Ptdata[l_u16index].u16Rightpt - pOutArea->Ptdata[l_u16index].u16Leftpt;
					if(l_u16tmp > 10)
					{
						if(pOutArea->Ptdata[l_u16index].u16Leftpt < pLasterParm.LmsPtStart())	
						{
							pOutArea->Ptdata[l_u16index].u16xLeft =  pOutArea->Ptdata[l_u16index].u16xRight - 2300;
						}
						else if(pOutArea->Ptdata[l_u16index].u16Rightpt >  pLasterParm.LmsPtStop())
						{
							pOutArea->Ptdata[l_u16index].u16xRight = pOutArea->Ptdata[l_u16index].u16xLeft + 2000;
						}
						else
						{
							l_32tmp = (pOutArea->Ptdata[l_u16index].u16xLeft + pOutArea->Ptdata[l_u16index].u16xRight)>>1;
							pOutArea->Ptdata[l_u16index].u16xLeft = l_32tmp - 1000;
							pOutArea->Ptdata[l_u16index].u16xRight = l_32tmp + 1000;
						}
						l_u16index =  pOutArea->u8Sum;
					}
					else
					{
						memset(& (pOutArea->Ptdata[l_u16index]),0,sizeof(PointStruct));
						pOutArea->u8Sum--;								
						pOutArea->uValid[l_u16index] = 0;

					}
				}
				else if(Valid(&pOutArea->Ptdata[l_u16index],/*m_zdata,*/pLasterParm))
				{		 		
					l_u16index =  pOutArea->u8Sum;
				}	
				else
				{
					memset(& (pOutArea->Ptdata[l_u16index]),0,sizeof(PointStruct));
					pOutArea->u8Sum--;								
					pOutArea->uValid[l_u16index] = 0;
				}
			}
		}
	}

	for (l_u16index = 1;l_u16index< pOutArea->u8Sum;l_u16index++)
    {
        if(IS_INSIDE(pOutArea->Ptdata[l_u16index - 1].u16xLeft,pOutArea->Ptdata[l_u16index - 1].u16xRight,pOutArea->Ptdata[l_u16index].u16xLeft,pOutArea->Ptdata[l_u16index].u16xRight))
        {
            //
            pOutArea->Ptdata[l_u16index - 1].u16xRight = pOutArea->Ptdata[l_u16index].u16xRight;
            pOutArea->Ptdata[l_u16index - 1].u16Rightpt = pOutArea->Ptdata[l_u16index].u16Rightpt;
            pOutArea->Ptdata[l_u16index - 1].u16xMaxHt = max(pOutArea->Ptdata[l_u16index - 1].u16xMaxHt,pOutArea->Ptdata[l_u16index].u16xMaxHt);
            pOutArea->Ptdata[l_u16index - 1].u16xDis = abs(pOutArea->Ptdata[l_u16index - 1].u16xRight - pOutArea->Ptdata[l_u16index - 1].u16xLeft);
            for (l_u16tmp = l_u16index;l_u16tmp<pOutArea->u8Sum - 1;l_u16tmp++)
            {
                memcpy(&pOutArea->Ptdata[l_u16tmp],&pOutArea->Ptdata[l_u16tmp+1],sizeof(PointStruct));
            }
            memset(&pOutArea->Ptdata[l_u16tmp],0,sizeof(PointStruct));
            pOutArea->u8Sum--;

        }
    } 

	if(pOutArea->u8Sum){
	
		//对路边侧可能存在的复杂环境进行修正
		for(i = l_leftPt_absV;i<pLasterParm.LmsPtStart();i++)
		{
			qint16 t_angle = abs(pLasterParm.LmsPtMid()-i);
		
			if((*ptBaseArry)[i] && t_angle<=180)
			{
				int32 t_diff = (pLasterParm.LmsHeight() - MathHelper::Cos((*ptBaseArry)[i], t_angle));
				m_zdata[i] += t_diff;
			}
			else
			{
				break;
			}
		}
	
		for(i = pLasterParm.LmsPtStop();i<l_rightPt_absV;i++)
		{
			qint16 t_angle = abs(pLasterParm.LmsPtMid()-i);
			if((*ptBaseArry)[i] && t_angle<=180)
			{
				int32 t_diff = (pLasterParm.LmsHeight() -  MathHelper::Cos((*ptBaseArry)[i], t_angle));
				m_zdata[i] += t_diff;
			}
			else
			{
				break;
			}
		}
	
	}
	
}
#else
void LasterData::splitArea(const LasterSettings & pLasterParm,VirtualBackGround *ptBaseArry,PointSet *pOutArea)
{	
	
	int32 i = 0,l_leftPt =0,l_rightPt = 0,l_leftPt_absV =0,l_rightPt_absV= 0,l_32tmp = 0;
	qint16 l_u16index = 0,TempVaule1 =0,l_u16tmp =0;

	m_depends = pLasterParm;
	bool hasExclude = (pLasterParm.LmsObstacleleft() + pLasterParm.LmsObstacleright() > 0);
	//求高度值
	for(i = 0;i<POINT_SUM;i++)
	{
		if(hasExclude && i > pLasterParm.LmsObstacleleft() && i< pLasterParm.LmsObstacleright())
		{
			m_zdata[i] = m_xdata[i] = 0;
			continue;
		}


		TempVaule1 =  abs(i - pLasterParm.LmsPtMid());
		if(TempVaule1 <= 180)
		{
			m_zdata[i] = MathHelper::Cos(abs((*ptBaseArry)[i] - m_data[i]),TempVaule1);  
				

			if(!(*ptBaseArry)[i] && m_data[i])
			{ //背景打飞过车时不打飞情况，可以获得真实高度
				m_zdata[i] = abs(pLasterParm.LmsHeight() - m_zdata[i]);
			}
			
			//m_zdata[i] *=m_scale;

			m_xdata[i] = MathHelper::Sin( m_data[i],TempVaule1);
			m_xdata[i] *= (i < pLasterParm.LmsPtMid() ? -1 : 1);    
		}
		else
		{
			m_zdata[i] = m_xdata[i] = 0;
		}
	}
	
	bool bprint = false;
	if(bprint){
		qDebug()<<m_zdata;
	}

	//帧数据进行分有车区域,记录每个区域的左右位置信息
	m_left = l_leftPt = this->GetLimitValueLeft(pLasterParm);
	m_right = l_rightPt = this->GetLimitValueRight(pLasterParm);;
	l_leftPt_absV = l_leftPt;
	l_rightPt_absV = l_rightPt;

	l_u16index = pOutArea->u8Sum = 0;
	for(i = l_leftPt;i<l_rightPt;i++)
	{
		if((m_zdata[i] > ThresVehLow) )//	&& (g_ZdistanceV[i] < ThresVehHigh)
		{     
			if(!pOutArea->uValid[l_u16index])
			{
				pOutArea->u8Sum = ( pOutArea->u8Sum+1)&POINTSET_MASK;

				pOutArea->uValid[l_u16index] = 1;
				//针对打飞的点
				//if(abs(m_xdata[i]) <FLY_LIMIT_VALUE)
				if(IS_FLY(m_zdata[i],pLasterParm.LmsHeight()))
				{
					pOutArea->Ptdata[l_u16index].u16xMaxHt = 0; //给0
				}         
				else
				{							
					pOutArea->Ptdata[l_u16index].u16xMaxHt = m_zdata[i];				
				}                			

				pOutArea->Ptdata[l_u16index].u16xRight = pOutArea->Ptdata[l_u16index].u16xLeft = m_xdata[i];

				pOutArea->Ptdata[l_u16index].u16Leftpt  = i;
				pOutArea->Ptdata[l_u16index].u16Rightpt = i;
				pOutArea->Ptdata[l_u16index].u16xDis = 0;
			}
			else
			{
				pOutArea->Ptdata[l_u16index].u16xRight = m_xdata[i];
				pOutArea->Ptdata[l_u16index].u16Rightpt = i;
				// pOutArea->Ptdata[l_u16index].u16xDis = abs(m_xdata[i] -  pOutArea->Ptdata[l_u16index].u16xLeft);
				//寻找非打飞点的最大值，有可能找不到
				l_32tmp =  pOutArea->Ptdata[l_u16index].u16xMaxHt;

				if(m_zdata[i] > l_32tmp && !IS_FLY(m_zdata[i],pLasterParm.LmsHeight()))
				{
					pOutArea->Ptdata[l_u16index].u16xMaxHt = m_zdata[i];
				}
			}
		}
		else if(pOutArea->uValid[l_u16index])
		{
			l_u16tmp =  pOutArea->Ptdata[l_u16index].u16Rightpt;	
			if( i - l_u16tmp > 5)
			{ //实际行车概率性问题

				qint8 t_WaitFix = 0;
				//针对打飞的点
				l_u16tmp =  pOutArea->Ptdata[l_u16index].u16Leftpt;
				if(IS_FLY(m_zdata[l_u16tmp],pLasterParm.LmsHeight()))
				{
					if(l_u16tmp < pLasterParm.LmsPtMid()){					
						//左侧点修正
						if(pOutArea->Ptdata[l_u16index].u16xMaxHt){
							pOutArea->Ptdata[l_u16index].u16xLeft = MathHelper::Tan(pLasterParm.LmsHeight() - pOutArea->Ptdata[l_u16index].u16xMaxHt,pLasterParm.LmsPtMid() - l_u16tmp);
							pOutArea->Ptdata[l_u16index].u16xLeft *=(-1);
						}else{
							qint32 raw_x = 0;
							if(!IS_ZERO(m_data[l_u16tmp - 1])){
								raw_x = m_xdata[l_u16tmp - 1];
								pOutArea->Ptdata[l_u16index].u16xLeft = raw_x*17/20;
							}else{
								pOutArea->Ptdata[l_u16index].u16xLeft = (-1)*MathHelper::Tan(pLasterParm.LmsHeight() - 1600,pLasterParm.LmsPtMid() - l_u16tmp);
							}						
						}
					} else {
						//右侧点修正
						pOutArea->Ptdata[l_u16index].u16xLeft = m_xdata[l_u16tmp - 1];
					}	

				}

				l_u16tmp =  pOutArea->Ptdata[l_u16index].u16Rightpt;
				if(IS_FLY(m_zdata[l_u16tmp],pLasterParm.LmsHeight()))
				{
					if(l_u16tmp > pLasterParm.LmsPtMid()){					
						//右侧点修正
						if(pOutArea->Ptdata[l_u16index].u16xMaxHt){
							pOutArea->Ptdata[l_u16index].u16xRight = MathHelper::Tan(pLasterParm.LmsHeight() - pOutArea->Ptdata[l_u16index].u16xMaxHt,pLasterParm.LmsPtMid() - l_u16tmp);							
						}else{
							qint32 raw_x = 0;
							if(!IS_ZERO(m_data[l_u16tmp + 1])){
								raw_x = m_xdata[l_u16tmp + 1];
								pOutArea->Ptdata[l_u16index].u16xRight = raw_x*17/20;
							}else{
								pOutArea->Ptdata[l_u16index].u16xRight = MathHelper::Tan(pLasterParm.LmsHeight() - 1600,pLasterParm.LmsPtMid() - l_u16tmp);
							}						
						}
					}else{
						//左侧点修正
						pOutArea->Ptdata[l_u16index].u16xRight = m_xdata[l_u16tmp + 1];
					}
				}

				l_u16tmp =  pOutArea->Ptdata[l_u16index].u16Rightpt - pOutArea->Ptdata[l_u16index].u16Leftpt;
				if(l_u16tmp < 5)
				{
					pOutArea->Ptdata[l_u16index].u16xLeft = pOutArea->Ptdata[l_u16index].u16xRight = -1;
				}


				l_u16tmp = abs( pOutArea->Ptdata[l_u16index].u16xLeft -  pOutArea->Ptdata[l_u16index].u16xRight);
				pOutArea->Ptdata[l_u16index].u16xDis = l_u16tmp;

				if(l_u16tmp < SMALL_AREA)	  //有效宽度小 ,认为是无效区域								
				{
					//对于在车道边缘，由于测距限制，仅能获得侧面信息，人为对其进行修正
					l_u16tmp =  pOutArea->Ptdata[l_u16index].u16Rightpt - pOutArea->Ptdata[l_u16index].u16Leftpt;
					if(l_u16tmp > 10)
					{
						if(pOutArea->Ptdata[l_u16index].u16Leftpt < pLasterParm.LmsPtStart())	
						{
							pOutArea->Ptdata[l_u16index].u16xLeft =  pOutArea->Ptdata[l_u16index].u16xRight - 2300;
						}
						else if(pOutArea->Ptdata[l_u16index].u16Rightpt >  pLasterParm.LmsPtStop())
						{
							pOutArea->Ptdata[l_u16index].u16xRight = pOutArea->Ptdata[l_u16index].u16xLeft + 2000;
						}
						else
						{
							l_32tmp = (pOutArea->Ptdata[l_u16index].u16xLeft + pOutArea->Ptdata[l_u16index].u16xRight)>>1;
							pOutArea->Ptdata[l_u16index].u16xLeft = l_32tmp - 1000;
							pOutArea->Ptdata[l_u16index].u16xRight = l_32tmp + 1000;
						}
						l_u16index =  pOutArea->u8Sum;
					}
					else
					{
						memset(& (pOutArea->Ptdata[l_u16index]),0,sizeof(PointStruct));
						pOutArea->u8Sum--;								
						pOutArea->uValid[l_u16index] = 0;

					}
				}
				else if(Valid(&pOutArea->Ptdata[l_u16index],/*m_zdata,*/pLasterParm))
				{		 		
					l_u16index =  pOutArea->u8Sum;
				}	
				else
				{
					memset(& (pOutArea->Ptdata[l_u16index]),0,sizeof(PointStruct));
					pOutArea->u8Sum--;								
					pOutArea->uValid[l_u16index] = 0;
				}
			}
		}
	}

	for (l_u16index = 1;l_u16index< pOutArea->u8Sum;l_u16index++)
    {
        if(IS_INSIDE(pOutArea->Ptdata[l_u16index - 1].u16xLeft,pOutArea->Ptdata[l_u16index - 1].u16xRight,pOutArea->Ptdata[l_u16index].u16xLeft,pOutArea->Ptdata[l_u16index].u16xRight))
        {
            //
            pOutArea->Ptdata[l_u16index - 1].u16xRight = pOutArea->Ptdata[l_u16index].u16xRight;
            pOutArea->Ptdata[l_u16index - 1].u16Rightpt = pOutArea->Ptdata[l_u16index].u16Rightpt;
            pOutArea->Ptdata[l_u16index - 1].u16xMaxHt = max(pOutArea->Ptdata[l_u16index - 1].u16xMaxHt,pOutArea->Ptdata[l_u16index].u16xMaxHt);
            pOutArea->Ptdata[l_u16index - 1].u16xDis = abs(pOutArea->Ptdata[l_u16index - 1].u16xRight - pOutArea->Ptdata[l_u16index - 1].u16xLeft);
            for (l_u16tmp = l_u16index;l_u16tmp<pOutArea->u8Sum - 1;l_u16tmp++)
            {
                memcpy(&pOutArea->Ptdata[l_u16tmp],&pOutArea->Ptdata[l_u16tmp+1],sizeof(PointStruct));
            }
            memset(&pOutArea->Ptdata[l_u16tmp],0,sizeof(PointStruct));
            pOutArea->u8Sum--;

        }
    } 

	if(pOutArea->u8Sum){
	
		//对路边侧可能存在的复杂环境进行修正
		for(i = l_leftPt_absV;i<pLasterParm.LmsPtStart();i++)
		{
			qint16 t_angle = abs(pLasterParm.LmsPtMid()-i);
		
			if((*ptBaseArry)[i] && t_angle<=180)
			{
				int32 t_diff = (pLasterParm.LmsHeight() - MathHelper::Cos((*ptBaseArry)[i], t_angle));
				m_zdata[i] += t_diff;
			}
			else
			{
				break;
			}
		}
	
		for(i = pLasterParm.LmsPtStop();i<l_rightPt_absV;i++)
		{
			qint16 t_angle = abs(pLasterParm.LmsPtMid()-i);
			if((*ptBaseArry)[i] && t_angle<=180)
			{
				int32 t_diff = (pLasterParm.LmsHeight() -  MathHelper::Cos((*ptBaseArry)[i], t_angle));
				m_zdata[i] += t_diff;
			}
			else
			{
				break;
			}
		}
	
	}
}
#endif
bool LasterData::Valid(PointStruct *pt_info,/*const QList<qint32> & pt_array,*/const LasterSettings & nSettings)
{
	if(pt_info->u16Rightpt <= pt_info->u16Leftpt){
		return false;
	}

    if(pt_info->u16Rightpt - pt_info->u16Leftpt < 5){
       return false;
    }
	//if(pt_info->u16xRight <= pt_info->u16xLeft){		
	//	return  false;	
	//}

	//qint16 t_total = pt_info->u16Rightpt - pt_info->u16Leftpt;
	//	
	//if((pt_info->u16xDis/t_total) > 600){	
	//	return false;
	//}
	
	return true;

	//qint16 t_zero = 0;
	//int t_pt = pt_info->u16Leftpt;
	//for(;t_pt< pt_info->u16Rightpt;t_pt++)
	//{
	//	if(IS_ZERO(pt_array[t_pt]))
	//	{
	//		t_zero++;
	//	}	
	//}

	//if(t_total){
	//	return t_zero < (t_total>>1);
	//} else {
	//	return false;
	//}
}