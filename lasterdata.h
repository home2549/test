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
    

	//�������
	//1�����������ֵ���������ɻ�ø߶ȡ�Xλ��
	//2��������ɣ�����ֵ������ͨ���������߶�ֵ�ɻ�ø߶ȡ�Xλ��
	//3����������������ֵ��ɣ���Ϣ��ʧ���߶�Ϊ�������߶ȣ�xλ��Ϊ0
	//4�����������ֵ����ɣ�����ʶ��
	void splitArea(const LasterSettings &,VirtualBackGround *,PointSet *);


	void setScale(qreal nscale);

	static bool Valid(PointStruct *pt_info,/*const QList<qint32> & pt_array,*/const LasterSettings &);
private:
	

	quint16 GetLimitValueLeft(const LasterSettings &);

	quint16 GetLimitValueRight(const LasterSettings &);
private:
	qint64 m_timestamp;
	QList<qint32> m_data; //�������
	//���㴦�������
	QList<qint32> m_xdata; 
	QList<qint32> m_zdata; 

	LasterSettings m_depends;
	qint32 m_left;
	qint32 m_right;

	qreal m_scale;
	const static qint32 ThresVehLow = 250;
};

#endif // LASTERDATA_H
