#ifndef DEVCHANNEL_H
#define DEVCHANNEL_H

#include <QObject>
#include <QDateTime>
#include <QTcpSocket>
//设备通路
class QTimer;

class DevChannel : public QObject
{
	Q_OBJECT

public:
	DevChannel(QString ip,qint32 port,QObject *parent = 0);
	DevChannel(QString nComX,QObject *parent = 0);
	~DevChannel();

signals:
	void sendError(QString);
	void newData(const QByteArray &);
	void signal_timeout();
	void signal_run();

	void connected();
public slots:
	
	void sendData(const QByteArray &);
protected slots:
	void cnnt_ok();
	void cnnt_break();
	void cnnt_error(QAbstractSocket::SocketError);
	void slot_dataRecv();
	void slot_timeoutCheck();
	void slot_run();
	void slot_dataRecvCom();
private:
	void cnnt(QString ip,quint16 port);
	void cnnt(QString comx);
private:
	QIODevice *m_io;
	QTimer *m_timer;
	QDateTime m_dateRecv;

	QString m_target;
	qint32 m_port;

	QString m_comx;

	QTimer *m_RecnntTimer;
	// read local file for sim
};

#endif // DEVCHANNEL_H
