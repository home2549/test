#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QTcpSocket>
#include <QSet>
 
class DbgProtocol;
class ReqMsg;

class Device : public QObject
{
	Q_OBJECT

public:
	Device(QTcpSocket *nSocket,QObject *parent);
	~Device();

signals:
	void signal_data(const QByteArray &);
public slots:
	void slot_send(qint32/*��ʶʲô���͵�����*/,const QByteArray &);
	void slot_Read();

	void slot_req(const ReqMsg & );
private:
	QTcpSocket *m_socket;

	DbgProtocol *m_pro_trans;
	QSet<qint32> m_watchs;
};

#endif // DEVICE_H
