#include "devprotocol.h"
#include <QDataStream>
#include <QDebug>
#include <QDateTime>
class FrameFormat{

public:
	enum{
		HEADER = 0x02,
		DATALEN = 361,
		F_SIZE = 831
	};

};

DevProtocol::DevProtocol(QObject *parent)
	: QObject(parent)
{

}

DevProtocol::~DevProtocol()
{

}

void DevProtocol::slot_NewFrame(const QByteArray & nData)
{
	qint32 tIndex = 0,tOffset = 0;
	quint8 theader = 0x00;
	quint16 nLen = 0;
	quint32 nTimestamp_noused = 0;
	quint16 nValue = 0;

	m_CacheData.append(nData);

	QDataStream out(m_CacheData);
	out.setByteOrder(QDataStream::BigEndian);

	tOffset = 0;

	do{
		tIndex = 0;
		while(tIndex<4 && !out.atEnd()){
			out>>theader;
			tOffset++;
			if(theader != FrameFormat::HEADER)
			{
				tIndex  = 0;
				continue;
			}
			tIndex++;
		}


		if(tIndex < 4 && tOffset >= m_CacheData.size()){			
			m_CacheData.clear();
			return;
		}else if(m_CacheData.size() - tOffset + 4 < FrameFormat::F_SIZE){
			//data is part,need wait more
			//qDebug()<<nData.toHex().toUpper();
			//qDebug()<<m_CacheData.toHex().toUpper();
			tOffset -= 4;
			m_CacheData.remove(0,tOffset);
			//qDebug()<<m_CacheData.toHex().toUpper();
			//qDebug()<<"data is part,need wait more";
			return;
		}

		out.skipRawData(42 - 4);
		out>>nTimestamp_noused;

		out.skipRawData(83 - (42 +4));
		out>>nLen;

		if(nLen != FrameFormat::DATALEN)
		{
			qDebug()<<m_CacheData.toHex().toUpper();
			qDebug()<<nLen;
			qDebug()<<"len err";
			m_CacheData.remove(0,tOffset);
			return;
		}
		else{
			
			//qDebug()<<m_CacheData.toHex().toUpper();
		}


		tIndex = 0;
		QList<qint32> tValuelst;
		while(tIndex < nLen){
			out>>nValue;			
			tValuelst.append(nValue);
			tIndex++;
		}

		out.skipRawData(24 - 14);
		//time real
		qint16 nYear = 0;
		qint8 nMonth = 0,nDay = 0,nHour = 0,nMin = 0,nSec = 0;
		quint32 nTimeUs = 0;
		out>>nYear>>nMonth>>nDay>>nHour>>nMin>>nSec>>nTimeUs;
		qint64 nabsTimestamp = QDateTime(QDate(nYear,nMonth,nDay),QTime(nHour,nMin,nSec,nTimeUs/1000)).toMSecsSinceEpoch();
		out.skipRawData(3);

		tOffset += (831 - 4);
		emit signal_data(LasterData(nabsTimestamp,tValuelst));

	}while(1);	
}

void DevProtocol::serialize(const QList<qint32> & nData,QByteArray & _return)
{
	QDataStream in_stream(&_return,QIODevice::WriteOnly);
	in_stream.setByteOrder(QDataStream::BigEndian);

	qint8 t_header = FrameFormat::HEADER;

	in_stream<<t_header
			<<t_header
			<<t_header
			<<t_header;

	qint16 t_index = 0;
	qint8 t_noUsed = 0;
	while(t_index < 83 - 4){
		in_stream<<t_noUsed;
		t_index++;
	}
	quint16 t_len = FrameFormat::DATALEN;
	in_stream<<t_len;

	t_index = 0;
	quint16 t_value = 0;
	QListIterator<qint32> iEveryItem( nData);
	while(iEveryItem.hasNext() && t_index < t_len){
		t_value = iEveryItem.next();	
		in_stream<<	t_value;
		t_index++;
	}

	t_value = 0;
	while(t_index < t_len){
		in_stream<<	t_value;
		t_index++;
	}
	t_index = 0;
	while(t_index < 24){
		in_stream<<t_noUsed;
		t_index++;
	}
}