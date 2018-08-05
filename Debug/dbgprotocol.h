#ifndef DBGPROTOCOL_H
#define DBGPROTOCOL_H

#include <QObject>
#include <QDataStream>



class ReqMsg{
public :
	ReqMsg() {  mType = 0; m_op = 0;}
	ReqMsg(qint32 nType,qint32 nOp) {  mType = nType; m_op = nOp; }

	ReqMsg(const ReqMsg & other) { *this = other; }
	virtual ~ReqMsg() { }
	
	ReqMsg & operator=(const  ReqMsg & other){
		mType = other.mType;
		m_op = other.m_op;
		return *this;
	}
	qint32 mType;
	qint32 m_op; //1-Ñ¡Ôñ£¬0-È¡Ïû
};

inline QDataStream & operator>>(QDataStream & io,ReqMsg & data)
{
	io>>data.mType>>data.m_op;
	return io;
}

inline QDataStream & operator<<(QDataStream & io,const ReqMsg & data)
{
	io<<data.mType<<data.m_op;
	return io;
}


inline QByteArray serialize(const ReqMsg & data)
{
	QByteArray ret_data;
	QDataStream io(&ret_data,QIODevice::WriteOnly);
	io.setByteOrder(QDataStream::BigEndian);
	io<<data;
	return ret_data;

}
class DbgProtocol : public QObject
{
	Q_OBJECT

public:
	DbgProtocol(QObject *parent);
	~DbgProtocol();
	
signals:
	void sig_req_data(const ReqMsg & );
public slots:
	void slot_parse(const QByteArray &);
private:
	
};

#endif // DBGPROTOCOL_H
