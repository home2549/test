#ifndef VIRTUALBACKGROUNDDBG_H
#define VIRTUALBACKGROUNDDBG_H

#include "virtualbackground.h"

class VirtualBackGroundDbg : public VirtualBackGround
{
	Q_OBJECT

public:
	enum{
		V_DIR,
		Q_DIR
	};
	VirtualBackGroundDbg(qint32,QObject *parent);
	~VirtualBackGroundDbg();

	virtual void update(const QList<qint32> &);	
signals:
	void signal_dbg(const QByteArray &);
private:
	qint32 m_type;
};

#endif // VIRTUALBACKGROUNDDBG_H
