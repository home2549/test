#include "virtualbackgrounddbg.h"
#include "devprotocol.h"
#include "DispatchMsg/thirddispatchmsg.h"
#include "DbgMsgTypeDefine.h"
VirtualBackGroundDbg::VirtualBackGroundDbg(qint32 nDir,QObject *parent)
	: VirtualBackGround(parent)
{
	if(nDir == V_DIR){
		ThirdDispatchMsg::reg_signal(this,SIGNAL(signal_dbg(const QByteArray)),ThirdDispatchMsg::BKG_V_TYPE);
	} else {
		ThirdDispatchMsg::reg_signal(this,SIGNAL(signal_dbg(const QByteArray)),ThirdDispatchMsg::BKG_Q_TYPE);
	}

}

VirtualBackGroundDbg::~VirtualBackGroundDbg()
{
	ThirdDispatchMsg::reg_off(this,ThirdDispatchMsg::BKG_V_TYPE);
	ThirdDispatchMsg::reg_off(this,ThirdDispatchMsg::BKG_Q_TYPE);
}

void VirtualBackGroundDbg::update(const QList<qint32> & nDisInfo)
{
	VirtualBackGround::update(nDisInfo);

	QByteArray _bufer;
	DevProtocol::serialize(data(),_bufer);
	emit signal_dbg(_bufer);
}