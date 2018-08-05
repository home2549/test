#ifndef LASTERDEVUNIT_H
#define LASTERDEVUNIT_H

#include <QObject>
#include "lasterdata.h"

class DevChannel;
class DevProtocol;
class QTimer;
class QThread;

class LasterDevUnit : public QObject
{
	Q_OBJECT

public:
	enum{		
		SINGLE_FRAME,
		CONTINUE_FRAME,
	};
	LasterDevUnit(QString ip,qint32 port, quint8 mod = SINGLE_FRAME, QObject *parent = 0);
	~LasterDevUnit();

	void stopRequest(bool);
signals:
	void signal_request(const QByteArray &);

	void signal_data(const LasterData &);

	void signal_start();
	public slots:
		void slot_speedup(bool);
protected slots:
	void slot_request();
private:
	QString m_target;
	qint32 m_port;
	quint8 m_model;
	
	QTimer *m_requst;

	DevChannel *m_Channel;
	QThread *mChannelThread;

	DevProtocol *m_Decode;
};

#endif // LASTERDEVUNIT_H
