#ifndef LASTERDATA_H
#define LASTERDATA_H

#include <QList>
#include "trafficsettings.h"
#include "head_struct.h"

class VirtualBackGround;

class LasterData
{
public:
	LasterData(qint64,const QList<qint32> &);
	LasterData(const LasterData & other);
	LasterData();
	~LasterData();

	qint64 timestamp() const;
	void setTimestamp(qint64);
	const QList<qint32> & data() const;
	const qint32 operator [](qint32) const;
	
	const LasterSettings & settings() const;
	quint16 GetLeft();

	quint16 GetRight();

	const QList<qint32> & zdata() const;
	const QList<qint32> & xdata() const;
    

	//分情况：
	//1、背景与测量值都正常，可获得高度、X位移
	//2、背景打飞，测量值正常，通过激光器高度值可获得高度、X位移
	//3、背景正常，测量值打飞，信息丢失，高度为激光器高度，x位移为0
	//4、背景与测量值都打飞，不可识别
	void splitArea(const LasterSettings &,VirtualBackGround *,PointSet *);


	void setScale(qreal nscale);

	static bool Valid(PointStruct *pt_info,/*const QList<qint32> & pt_array,*/const LasterSettings &);
private:
	

	quint16 GetLimitValueLeft(const LasterSettings &);

	quint16 GetLimitValueRight(const LasterSettings &);
private:
	qint64 m_timestamp;
	QList<qint32> m_data; //测距数据
	//计算处理后数据
	QList<qint32> m_xdata; 
	QList<qint32> m_zdata; 

	LasterSettings m_depends;
	qint32 m_left;
	qint32 m_right;

	qreal m_scale;
	const static qint32 ThresVehLow = 250;
};

#endif // LASTERDATA_H
