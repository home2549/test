#include "devchannel.h"
#include "../com/qextserialport.h"
#include <QTimer>
#include <QDebug>
#define TIMEOUT_LIMIT	500 //ms

DevChannel::DevChannel(QString ip,qint32 port,QObject *parent)
	: QObject(parent),m_target(ip),m_port(port),m_io(NULL),m_comx("")
{
	m_dateRecv = QDateTime::currentDateTime();
	m_timer = new QTimer(this);
	m_timer->setInterval(50);
	connect(m_timer,SIGNAL(timeout()),this,SLOT(slot_timeoutCheck()));
	connect(this,SIGNAL(signal_run()),this,SLOT(slot_run()));

	m_RecnntTimer = new QTimer(this);
	m_RecnntTimer->setInterval(1500);
	connect(m_RecnntTimer,SIGNAL(timeout()),this,SLOT(slot_run()));
}


DevChannel::~DevChannel()
{

}

void DevChannel::cnnt(QString ip,quint16 port)
{
	if(!m_io)
	{
		//qDebug()<<"Cnnt create:"<<QThread::currentThreadId();
		QTcpSocket *t_sk = new QTcpSocket(this);
		t_sk->setSocketOption(QAbstractSocket::LowDelayOption,0);
		m_io = t_sk;		
		connect(m_io,SIGNAL(connected()),this,SLOT(cnnt_ok()));
		connect(m_io,SIGNAL(connected()),this,SIGNAL(connected()));
		//connect(m_io,SIGNAL(disconnected()),this,SLOT(cnnt_break()));
		connect(m_io,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(cnnt_error(QAbstractSocket::SocketError)));
		connect(m_io,SIGNAL(readyRead()),this,SLOT(slot_dataRecv()));
		//QTimer::singleShot(10,m_timer,SLOT(start()));
	}
	
	(qobject_cast<QTcpSocket*>(m_io))->connectToHost(ip,port);
	if ((qobject_cast<QTcpSocket*>(m_io))->waitForConnected(1000)){
		 qDebug()<<QString("Device %1:%2 Connected!").arg(ip).arg(port);
		 m_RecnntTimer->stop();
	} else{
		(qobject_cast<QTcpSocket*>(m_io))->close();
		qDebug()<<(qobject_cast<QTcpSocket*>(m_io))->errorString();
		qDebug()<<QString("Device %1:%2 Connect error!").arg(ip).arg(port);
		if(!m_RecnntTimer->isActive()){
			m_RecnntTimer->start();
		}
	}
	//else if(!m_timer->isActive()){
	//	m_timer->start(1000); //ms
	//}		
}

void DevChannel::slot_run()
{
	//qDebug()<<"slot_run:"<<QThread::currentThreadId();
	cnnt(m_target,m_port);
}

void DevChannel::cnnt_ok()
{
	qDebug()<<"cnnt_ok";
	m_dateRecv = QDateTime::currentDateTime();
	m_timer->start();	
}

void DevChannel::cnnt_break()
{
	qDebug()<<"cnnt_break:";
	m_timer->stop();
}

void DevChannel::cnnt_error(QAbstractSocket::SocketError err)
{
	//qDebug()<<"cnnt_error:"<<QThread::currentThreadId();
	m_io->close();
	//QString err_str = m_io->errorString();
	//qDebug()<<"Cnnt Error:"<<err_str;
	m_timer->stop();

	if(!m_RecnntTimer->isActive()){
		m_RecnntTimer->start();
	}
}

void DevChannel::sendData(const QByteArray & nData)
{
	//if((qobject_cast<QTcpSocket*>(m_io))->state() == QAbstractSocket::ConnectedState)
	if(m_io)
	{
		if(m_io->isWritable())
		{
			m_io->write(nData);
		}
		else 
		{
			emit sendError(m_io->errorString());
		}	
	}
	
}

void DevChannel::slot_dataRecv()
{	
	while (!m_io->atEnd()) {
         
		 QByteArray data = m_io->read(1024);

		 emit newData(data);
      }

	m_dateRecv = QDateTime::currentDateTime();
}

void DevChannel::slot_timeoutCheck()
{
	if(m_io)
	{
		QTcpSocket* sk = qobject_cast<QTcpSocket*>(m_io);
		if(sk && sk->state() == QAbstractSocket::ConnectedState){
			if(TIMEOUT_LIMIT < m_dateRecv.msecsTo(QDateTime::currentDateTime()))
			{
				emit signal_timeout();		
			}			
		}			
	}
}

//comx 
DevChannel::DevChannel(QString nComX,QObject *parent)
	:m_comx(nComX),m_target(""),m_port(0),m_io(NULL)
{
	m_dateRecv = QDateTime::currentDateTime();
}

void DevChannel::cnnt(QString nComX)
{
	if(!m_io)
	{
		m_io = new QextSerialPort(nComX, QextSerialPort::EventDriven);
		(qobject_cast<QextSerialPort*>(m_io))->setBaudRate(BAUD56000);
		(qobject_cast<QextSerialPort*>(m_io))->setFlowControl(FLOW_OFF);
		(qobject_cast<QextSerialPort*>(m_io))->setParity(PAR_NONE);
		(qobject_cast<QextSerialPort*>(m_io))->setDataBits(DATA_8);
		(qobject_cast<QextSerialPort*>(m_io))->setStopBits(STOP_2);
		(qobject_cast<QextSerialPort*>(m_io))->open(QIODevice::ReadWrite);
		connect(m_io,SIGNAL(readyRead()),this,SLOT(slot_dataRecv()));
	}

	m_io->open(QIODevice::ReadWrite);

	if(!((qobject_cast<QextSerialPort*>(m_io))->lineStatus() & LS_DSR)){
		qDebug()<<nComX + " open error";
	}
}

void DevChannel::slot_dataRecvCom()
{
	while (!m_io->atEnd()) {
         
		 QByteArray data = m_io->read(1024);

		 emit newData(data);
      }

	m_dateRecv = QDateTime::currentDateTime();
}