#include "device.h"
#include "dbgprotocol.h"
#include "DbgMsgTypeDefine.h"

Device::Device(QTcpSocket *nSocket,QObject *parent)
	: QObject(parent),m_socket(nSocket)
{
	connect(nSocket,SIGNAL(readyRead()),this,SLOT(slot_Read()));

	m_pro_trans = new DbgProtocol(this);
	connect(this,SIGNAL(signal_data(const QByteArray &)),m_pro_trans,SLOT(slot_parse(const QByteArray &)));
	connect(m_pro_trans,SIGNAL(sig_req_data(const ReqMsg & )),this,SLOT(slot_req(const ReqMsg & )));
}

Device::~Device()
{

}

void Device::slot_send(qint32 nType,const QByteArray & nData)
{
	if(m_socket && m_watchs.contains(nType)){
		m_socket->write(nData);
	}
}

void Device::slot_Read()
{
	while (!m_socket->atEnd()) {
         
		 QByteArray data = m_socket->read(1024);

		 emit signal_data(data);
      }

}

void Device::slot_req(const ReqMsg & nMsg)
{
	if(nMsg.mType == ERROR_TYPE) {	

		m_watchs.insert(ERROR_TYPE);
		slot_send(ERROR_TYPE,QString("no known cmd").toAscii());

	} else if(nMsg.mType == V_BKG_TYPE){	

		if(nMsg.m_op){
			m_watchs.insert(nMsg.mType);
		} else {
			m_watchs.remove(nMsg.mType);
		}		

	} else if(nMsg.mType == Q_BKG_TYPE){

		if(nMsg.m_op){
			m_watchs.insert(nMsg.mType);
		} else {
			m_watchs.remove(nMsg.mType);
		}	
	} else if(VEH_TYPE == nMsg.mType){
		if(nMsg.m_op){
			m_watchs.insert(nMsg.mType);
		} else {
			m_watchs.remove(nMsg.mType);
		}
	} else if(V_RTLS_TYPE == nMsg.mType){
		if(nMsg.m_op){
			m_watchs.insert(nMsg.mType);
		} else {
			m_watchs.remove(nMsg.mType);
		}
	} else if(Q_RTLS_TYPE == nMsg.mType){
		if(nMsg.m_op){
			m_watchs.insert(nMsg.mType);
		} else {
			m_watchs.remove(nMsg.mType);
		}
	}
}