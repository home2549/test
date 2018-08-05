#include "lasterdevunit.h"

#include "net\devchannel.h"
#include "protocol/devprotocol.h"
#include <QTimer>
#include "SICKProc.h"
#include <QThread>
class requestFrame{
public:
	static  QByteArray SINGLEFRAME;
	static  QByteArray CONTINUEFRAME;
};
char RdToLMSSendData[24] = {0x02,0x02,0x02,0x02,0x00,0x00,0x00,0x0F,0x73,0x52,0x4E,0x20,0x4C,0x4D,0x44,0x73,0x63,0x61,0x6E,0x64,0x61,0x74
,0x61,0x05};
QByteArray requestFrame::SINGLEFRAME;
QByteArray requestFrame::CONTINUEFRAME;


LasterDevUnit::LasterDevUnit(QString ip,qint32 port, quint8 mod, QObject *parent)
	: QObject(parent),m_model(mod)
{

	qRegisterMetaType<LasterData>("LasterData");
	char buf[128] = {0};
	int len = 0;
	CSICKProc::SingleFrame(buf,len);
	requestFrame::SINGLEFRAME = QByteArray(buf,len);

	CSICKProc::ContinueFrame(buf,len,false);
	requestFrame::CONTINUEFRAME = QByteArray(buf,len);
	//connect to LMS devcies
	mChannelThread = new QThread(this);

	m_Channel = new DevChannel(ip,port,0);	
	m_Channel->moveToThread(mChannelThread);
	connect(mChannelThread,SIGNAL(destroyed()),m_Channel,SLOT(deleteLater()));

	m_Decode = new DevProtocol(0);
	m_Decode->moveToThread(mChannelThread);
	connect(mChannelThread,SIGNAL(destroyed()),m_Decode,SLOT(deleteLater()));

	connect(this,SIGNAL(signal_start()),m_Channel,SIGNAL(signal_run()));
	connect(m_Channel,SIGNAL(newData(const QByteArray &)),m_Decode,SLOT(slot_NewFrame(const QByteArray &)));
	connect(m_Decode,SIGNAL(signal_data(const LasterData &)),this,SIGNAL(signal_data(const LasterData &)));

	//超时重发
	connect(m_Channel,SIGNAL(signal_timeout()),this,SLOT(slot_request()));
	connect(this,SIGNAL(signal_request(const QByteArray &)),m_Channel,SLOT(sendData(const QByteArray &)));

	
	m_requst = new QTimer(this);
	connect(m_requst,SIGNAL(timeout()),this,SLOT(slot_request()));


	mChannelThread->start();
	emit signal_start();
	stopRequest(false);
}

LasterDevUnit::~LasterDevUnit()
{

}

void LasterDevUnit::stopRequest(bool bStoped)
{
	if(bStoped)
		m_requst->stop();
	else{
		if(m_model == SINGLE_FRAME){
			m_requst->start(20);
		}else{
			m_requst->setSingleShot(true);
			m_requst->start(200);
		}	
	}
}
void LasterDevUnit::slot_request()
{
	if(m_model == SINGLE_FRAME)
	{
		emit signal_request(requestFrame::SINGLEFRAME);
	}
	else
	{
		emit signal_request(requestFrame::CONTINUEFRAME);	
	}

}

void LasterDevUnit::slot_speedup(bool bspeed)
{
	if(!bspeed){
		slot_request();
	}
}