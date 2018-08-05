#ifndef VIRTUALBACKGROUND_H
#define VIRTUALBACKGROUND_H

#include <QObject>

class VirtualBackGround : public QObject
{
	Q_OBJECT

public:

	VirtualBackGround(QObject *parent);
	~VirtualBackGround();

	void setHeight(qint32 nheight);
	void setStartPos(qint32);
	void setEndPos(qint32);
	void setExcleftPos(qint32);
	void setExcrightPos(qint32);
	void setMid(qint32 nMid);

	virtual void update(const QList<qint32> &);

	bool isReady();
	const QList<qint32> & data() const;

	const qint32 operator[](qint32) const;
	
	qint32 height() const { return m_height;}
signals:
	void signal_heightChanged(qint32);

private:
	void buildRP();
#define LIMIT_CNT 1000
#define LIMIT_MAXLEVEL	350 //mm
	QList<qint32> m_dis;
	QList<qint32> m_static;

	qint32  m_height;
	qint32  m_start;
	qint32  m_stop;
	qint32	m_mid;
	qint32	m_excleft;
	qint32	m_excright;
	bool m_bReady;

	QList<qint32> m_RPDots; //»ù×¼²Î¿¼
};

#endif // VIRTUALBACKGROUND_H
