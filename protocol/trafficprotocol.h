#ifndef TRAFFICPROTOCOL_H
#define TRAFFICPROTOCOL_H

#include <QObject>

#include "../config/trafficsettings.h"
#include "head_struct.h"
#include "../vehicleinfo.h"
//数据解析
class TrafficProtocol : public QObject
{
	Q_OBJECT

public:
	TrafficProtocol(QObject *parent);
	~TrafficProtocol();

	void serialize_08(const DeviceSettings & ,QByteArray & out);

	quint16 serialize_01(const StaticsLaneStruct* ,const DeviceSettings & nSettings ,QByteArray & out,TotoalStaticsInfo & db_info);

	void serialize_02(const DeviceSettings & nSettings ,QByteArray & out);

	void serialize_11(const VehicleInfo * ,QByteArray & out,bool isNew = true);

	static uint8 get_lane(uint8 olane); //车道映射

	void serialize(const QByteArray &data,QByteArray &out_data);

	void setdbg(bool en_out);
signals:
	void signal_cmd02(const QString &,quint8);
	void signal_cmd03(const CMD_03 &);
	void signal_cmd04(const CMD_04 &);
	void signal_cmd05(const CMD_05 &);
	void signal_cmd06(const CMD_06 &);
	void signal_cmd07(const CMD_07 &);
	void signal_cmd0B(const CMD_0B &);
	void signal_cmd19(const CMD_19 &);
	void signal_cmd1A(const CMD_1A &);
	
public slots:
	void slot_NewFrame(const QByteArray &);
//	void RD_DataPro(uint8 *p_data,uint16 len);

private:
	void decode_02(const QByteArray &);	
	
private:
	QByteArray  m_CacheData;
	bool m_dbg;
};

#endif // TRAFFICPROTOCOL_H
