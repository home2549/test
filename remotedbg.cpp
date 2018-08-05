#include "remotedbg.h"
#include "../DeviceSim/device.h"
#include "DispatchMsg/thirddispatchmsg.h"
#include <QDebug> 
#include "DbgMsgTypeDefine.h"

RemoteDbg *RemoteDbg::m_ins = NULL;
RemoteDbg::RemoteDbg(QObject *parent)
	: QObject(parent)
{
	m_socketV = new QTcpServer(this);
	connect(m_socketV,SIGNAL(newConnection()),this,SLOT(slot_newVClient()));
	m_socketV->listen(QHostAddress::Any,9001);

	m_visitor.clear();

	ThirdDispatchMsg::reg_slot(this,SLOT(slot_dbg_v_bkg(const QByteArray &)),ThirdDispatchMsg::BKG_V_TYPE);
	ThirdDispatchMsg::reg_slot(this,SLOT(slot_dbg_q_bkg(const QByteArray &)),ThirdDispatchMsg::BKG_Q_TYPE);
	ThirdDispatchMsg::reg_slot(this,SLOT(slot_dbg_veh_height(const QByteArray &)),ThirdDispatchMsg::VEH_INFO);
	ThirdDispatchMsg::reg_slot(this,SLOT(slot_dbg_v_rtls(const QByteArray &)),ThirdDispatchMsg::RTLS_V_TYPE);
}

RemoteDbg::~RemoteDbg()
{
	ThirdDispatchMsg::reg_off(this,ThirdDispatchMsg::BKG_V_TYPE);
	ThirdDispatchMsg::reg_off(this,ThirdDispatchMsg::BKG_Q_TYPE);
	ThirdDispatchMsg::reg_off(this,ThirdDispatchMsg::VEH_INFO);
	ThirdDispatchMsg::reg_off(this,ThirdDispatchMsg::RTLS_V_TYPE);
}

RemoteDbg* RemoteDbg::instance()
{
	if(!m_ins){	
		m_ins = new RemoteDbg(0);
	}
	return m_ins;
}

void RemoteDbg::slot_dbg_v_bkg(const QByteArray & nData)
{
	emit signal_send(V_BKG_TYPE,nData);
}

void RemoteDbg::slot_dbg_q_bkg(const QByteArray & nData)
{
	emit signal_send(Q_BKG_TYPE,nData);
}

void RemoteDbg::slot_dbg_veh_height(const QByteArray & nData)
{
	emit signal_send(VEH_TYPE,nData);
}

void RemoteDbg::slot_dbg_v_rtls(const QByteArray & nData)
{
	emit signal_send(V_RTLS_TYPE,nData);
}


void RemoteDbg::slot_newVClient()
{
	while(m_socketV->hasPendingConnections()){
		//qDebug()<<"new dbg client detected";
		QTcpSocket *nSocket = m_socketV->nextPendingConnection();
		nSocket->setSocketOption(QAbstractSocket::LowDelayOption,true);
		Device *nDev = new Device(nSocket,this);
		connect(this,SIGNAL(signal_send(qint32,const QByteArray &)),nDev,SLOT(slot_send(qint32,const QByteArray &)));
		connect(nSocket,SIGNAL(disconnected()),this,SLOT(slot_VGone()));
		m_visitor.insert(nSocket,nDev);
	}
}
void RemoteDbg::slot_VGone()
{
	QTcpSocket *nSocket = qobject_cast<QTcpSocket *> (this->sender());

	if(m_visitor.contains(nSocket)){
	
		delete m_visitor[nSocket];
		
		nSocket->deleteLater();

		m_visitor.remove(nSocket);
	}
}
