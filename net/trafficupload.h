#ifndef TRAFFICUPLOAD_H
#define TRAFFICUPLOAD_H
//上传服务通路
#include <QObject>
#include "../vehicleinfo.h"
#include <QMap>

class DevChannel;
class TrafficProtocol;
class QTimer;
class QThread;
class VehicleTetry;

class TrafficUpload : public QObject
{
	Q_OBJECT

public:
	TrafficUpload(QString ip,qint32 port,QObject *parent);
	~TrafficUpload();

signals:
	void signal_start();
	void signal_send(const QByteArray &);
	void signal_veh(const VehicleInfo &);
	void signal_statics(const TotoalStaticsInfo &);
	void signal_fail(const TotoalStaticsInfo &);

	void signal_link(bool);
public slots:
	void slot_veh(const VehicleInfo &);

	void slot_cmd(const QString &,quint8);
	void slot_cmd(const CMD_03 &);
	void slot_cmd(const CMD_04 &);
	void slot_cmd(const CMD_05 &);
	void slot_cmd(const CMD_06 &);
	void slot_cmd(const CMD_07 &);
	void slot_cmd(const CMD_0B &);
	void slot_cmd(const CMD_1A &);

	void slot_cycleChanged(qint32);
	void slot_statics();

	void slot_retrySend(const VehicleInfo &);
	void slot_fail(const VehicleInfo &);
protected slots:
	void slot_blink();
private:
	void statics_update(const VehicleInfo *pVeh);
	void gen_sv_fram();
	uint8 Send1Data(uint8 *p,uint16 len);

	void answer_cmd08();
	void resetStatics();
private:
	DevChannel *m_Channel;
	
	QThread *mChannelThread;

	TrafficProtocol *m_Decode;

	StaticsLaneStruct g_StaticsInfo[MAX_LANE];

	QTimer *m_Timer;

	QTimer *m_BlinkTimer;

	VehicleTetry	*m_VehicleRetry;
	//enum{
	//	RTLS_MOD, //实时传输
	//	REPEAT_MOD //重传
	//};
	//QMap<qint32 /*MOD*/,qint32/*ID*/> m_WorKState;
	//QTimer *m_retry;
};

#endif // TRAFFICUPLOAD_H
