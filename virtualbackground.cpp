#include "virtualbackground.h"
#include "../mathhelper.h"
#include <QDebug>
VirtualBackGround::VirtualBackGround(QObject *parent)
	: QObject(parent),
	m_height(0),
	m_start(30),
	m_stop(300),
	m_excleft(0),
	m_excright(0),
	m_mid(180),
	m_bReady(false)
{
	m_dis.clear();
	m_static.clear();
}

VirtualBackGround::~VirtualBackGround()
{

}

void VirtualBackGround::setHeight(qint32 nheight)
{
	m_height =nheight; 
	if(m_height){
		m_bReady = true;
		buildRP();
	}	
}

void VirtualBackGround::setMid(qint32  nMid)
{
	m_mid =nMid; 
}

void VirtualBackGround::setStartPos(qint32 nValue)
{
	 m_start = nValue;	
}

void VirtualBackGround::setEndPos(qint32 nValue)
{
	 m_stop = nValue;
	
}
void VirtualBackGround::setExcleftPos(qint32 nValue)
{
	m_excleft = nValue;
	
}
void VirtualBackGround::setExcrightPos(qint32 nValue)
{
	m_excright = nValue;
}
bool VirtualBackGround::isReady()
{
	return m_bReady;
}
void VirtualBackGround::update(const QList<qint32> & nDisInfo)
{
	if(!m_dis.size())
	{ //首帧
		m_dis.append(nDisInfo);

		qint32 t_Count = m_dis.size();
		for(qint32 t_index = 0;t_index<t_Count;t_index++)
		{
			m_static.append(0);
		}		
		return;
	}

	qint32 t_index = 0;
	QListIterator<qint32> iEveryItem(nDisInfo);
#if 0
	while(iEveryItem.hasNext())
	{
		quint32 t_Dis = iEveryItem.next();

		quint32 t_bDis = m_dis.at(t_index);
		if(t_Dis > t_bDis + 250)
		{				
			if(m_static.at(t_index) < 0)
			{
				m_static.replace(t_index,0);
			}
			m_static[t_index]++;



			qint32 t_calcH01 = MathHelper::Cos(t_Dis,m_mid-t_index);
			qint32 t_calcH02 = MathHelper::Cos(t_bDis,m_mid-t_index);
			if(abs(t_calcH01 - m_height) < abs(t_calcH02 - m_height)){
				m_dis[t_index] = t_Dis;
				 m_static[t_index] = 0;
			}

			//if((t_bDis && m_static.at(t_index) > 50)) //LIMIT_CNT
			//{
			//	 m_dis[t_index] = t_Dis;
			//	 m_static[t_index] = 0;
			//}
		}
		else if(!t_Dis)
		{
			if(m_static.at(t_index) > 0)
			{
				m_static.replace(t_index,0);
			}
			m_static[t_index]--;


			if(m_static[t_index] < -100) //||  !m_height
			{
				 m_dis[t_index] = t_Dis;
				 m_static[t_index] = 0;
			}
		}
		else{
			m_static.replace(t_index,0);
		}

		t_index++;
	}
#else
	while(iEveryItem.hasNext())
	{
		quint32 t_Dis = iEveryItem.next();

		quint32 t_bDis = m_dis.at(t_index);

		if(t_Dis > t_bDis)
		{				
			if(m_static.at(t_index) < 0)
			{
				m_static.replace(t_index,0);
			}
			m_static[t_index]++;
		}
		else if(t_Dis < t_bDis)
		{
			if(m_static.at(t_index) > 0)
			{
				m_static.replace(t_index,0);
			}
			m_static[t_index]--;		
		}
		else{
			m_static.replace(t_index,0);
		}

		if(abs(m_static[t_index]) > 200){
			m_dis[t_index] = t_Dis;
			m_static[t_index] = 0;
		}

		t_index++;
	}

#endif
	//进行计算是否基准值已满足要示 	  	
	if(!m_bReady)
	{
		QList<qint32> t_heights;
		t_heights.clear();
		for(qint32 t_index = m_start; t_index<m_stop; t_index++)
		{
			if(t_index >= m_excleft  && t_index <= m_excright)
			{
				continue;
			}
			
			qint32 t_calcH = MathHelper::Cos(m_dis.at(t_index),m_mid-t_index);
			t_heights.append(t_calcH);
		}

		qint32 _height = 0;
		qint32 percent = MathHelper::Density(t_heights,_height);
		if(percent > 60)
		{			
			//ok
			m_bReady = true;
			if(!m_height){
				m_height = _height;
				emit signal_heightChanged(m_height);
			}
			buildRP();
			qDebug()<<"BackGround ready:"<<m_height;
		}
		else
		{
			//m_height = 0;
			m_bReady = false;
		}	 
	}

}

const QList<qint32> & VirtualBackGround::data() const
{
	return m_dis;
}
const qint32 VirtualBackGround::operator[](qint32 t_index) const
{
	return m_dis.at(t_index);
}

void VirtualBackGround::buildRP()
{

}