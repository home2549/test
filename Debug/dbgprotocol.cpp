#include "dbgprotocol.h"
#include "DbgMsgTypeDefine.h"

DbgProtocol::DbgProtocol(QObject *parent)
	: QObject(parent)
{

}

DbgProtocol::~DbgProtocol()
{

}

void DbgProtocol::slot_parse(const QByteArray & nData)
{
	QDataStream io(nData);
	io.setByteOrder( QDataStream::BigEndian);

	qint32 nType = 0;
	io>>nType;
	switch(nType){
		
	case V_BKG_TYPE:
	case Q_BKG_TYPE:
	case ERROR_TYPE:
	case VEH_TYPE:
	case V_RTLS_TYPE:
	case Q_RTLS_TYPE:
	
		{	
			QDataStream io_s2(nData);
			io_s2.setByteOrder( QDataStream::BigEndian);

			ReqMsg n_msg;
			io_s2>>n_msg;

			emit sig_req_data(n_msg);		
		}
	
		break;
	}
}