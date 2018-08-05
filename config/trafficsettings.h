#ifndef TRAFFICSETTINGS_H
#define TRAFFICSETTINGS_H

#include <QObject>
#include <QTimer>
#include <QMap>
//参数配置

class LasterSettings{

	friend class TrafficSettings;

public:
	QString	LmsIp() const;	
	qint32	LmsPort() const;	
	qint32	LmsPtStart() const;	
	qint32	LmsPtStop() const;	
	qint32	LmsPtMid() const;	
	qint32	LmsObstacleleft() const;	
	qint32	LmsObstacleright() const;	
	qint32	LmsHeight() const;	
private:
	QString m_LmsIp;
	qint32 m_LmsPort;
	qint32 m_LmsPtStart;
	qint32 m_LmsPtStop;
	qint32 m_LmsPtMid;
	qint32 m_LmsObstacleleft;
	qint32 m_LmsObstacleright;
	qint32 m_LmsHeight;
};

class DeviceSettings{

	friend class TrafficSettings;
public:
	QString	rdnum() const;	
	QString	devId() const;
	QString	svrIp() const;	
	qint32	InvContents() const;
	qint32	Cycle() const;
	qint32	DisTime() const;	
	qint32	TransMethod() const;	
	qreal	Longitude() const;	
	qreal	Latitude() const;	
	qint32	altitude() const;
	qint32	LaneNum() const;
private:
	QString m_rdnum;
	QString m_devId;
	QString m_svrIp;
	qint32 m_InvContents; //调查内容
	qint32 m_Cycle; //调查周期 
	qint32 m_DisTime; //跟车百分比鉴别时间
	qint32 m_TransMethod; //当前传输方式
	qreal  m_Longitude; //经度
	qreal m_Latitude; //纬度
	qint32 m_altitude;//高程
	qint32 m_LaneNum; //车道数	
};

class TrafficSettings : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString	rdnum READ rdnum WRITE setrdnum NOTIFY rdnumChanged);
	Q_PROPERTY(QString	devId READ devId WRITE setdevId NOTIFY devIdChanged);
	Q_PROPERTY(QString	svrIp READ svrIp WRITE setsvrIp NOTIFY svrIpChanged);
	Q_PROPERTY(qint32	svrPort READ svrPort WRITE setsvrPort NOTIFY svrPortChanged);
	Q_PROPERTY(QString	vLmsIp READ vLmsIp WRITE setvLmsIp NOTIFY vLmsIpChanged);
	Q_PROPERTY(qint32	vLmsPort READ vLmsPort WRITE setvLmsPort NOTIFY vLmsPortChanged);
	Q_PROPERTY(qint32	vLmsPtStart READ vLmsPtStart WRITE setvLmsPtStart NOTIFY vLmsPtStartChanged);
	Q_PROPERTY(qint32	vLmsPtStop READ vLmsPtStop WRITE setvLmsPtStop NOTIFY vLmsPtStopChanged);
	Q_PROPERTY(qint32	vLmsPtMid READ vLmsPtMid WRITE setvLmsPtMid NOTIFY vLmsPtMidChanged);
	Q_PROPERTY(qint32	vLmsObstacleleft READ vLmsObstacleleft WRITE setvLmsObstacleleft NOTIFY vLmsObstacleleftChanged);
	Q_PROPERTY(qint32	vLmsObstacleright READ vLmsObstacleright WRITE setvLmsObstacleright NOTIFY vLmsObstaclerightChanged);
	Q_PROPERTY(qint32	vLmsHeight READ vLmsHeight WRITE setvLmsHeight NOTIFY vLmsHeightChanged);
	Q_PROPERTY(QString	qLmsIp READ qLmsIp WRITE setqLmsIp NOTIFY qLmsIpChanged);
	Q_PROPERTY(qint32	qLmsPort READ qLmsPort WRITE setqLmsPort NOTIFY qLmsPortChanged);
	Q_PROPERTY(qint32	qLmsPtStart READ qLmsPtStart WRITE setqLmsPtStart NOTIFY qLmsPtStartChanged);
	Q_PROPERTY(qint32	qLmsPtStop READ qLmsPtStop WRITE setqLmsPtStop NOTIFY qLmsPtStopChanged);
	Q_PROPERTY(qint32	qLmsPtMid READ qLmsPtMid WRITE setqLmsPtMid NOTIFY qLmsPtMidChanged);
	Q_PROPERTY(qint32	qLmsObstacleleft READ qLmsObstacleleft WRITE setqLmsObstacleleft NOTIFY qLmsObstacleleftChanged);
	Q_PROPERTY(qint32	qLmsObstacleright READ qLmsObstacleright WRITE setqLmsObstacleright NOTIFY qLmsObstaclerightChanged);
	Q_PROPERTY(qint32	qLmsHeight READ qLmsHeight WRITE setqLmsHeight NOTIFY qLmsHeightChanged);
	Q_PROPERTY(qint32	InvContents READ InvContents WRITE setInvContents NOTIFY InvContentsChanged);
	Q_PROPERTY(qint32	LaneNum READ LaneNum WRITE setLaneNum NOTIFY LaneNumChanged);
	Q_PROPERTY(qint32	Cycle READ Cycle WRITE setCycle NOTIFY CycleChanged);
	Q_PROPERTY(qint32	DisTime READ DisTime WRITE setDisTime NOTIFY DisTimeChanged);
	Q_PROPERTY(qint32	TransMethod READ TransMethod WRITE setTransMethod NOTIFY TransMethodChanged);
	Q_PROPERTY(qint32	DisTime READ DisTime WRITE setDisTime NOTIFY DisTimeChanged);
	Q_PROPERTY(qreal	Longitude READ Longitude WRITE setLongitude NOTIFY LongitudeChanged);
	Q_PROPERTY(qreal	Latitude READ Latitude WRITE setLatitude NOTIFY LatitudeChanged);
	Q_PROPERTY(QString	user READ user WRITE setuser NOTIFY userChanged);
	Q_PROPERTY(QString	pwd READ pwd WRITE setpwd NOTIFY pwdChanged);

protected:
	TrafficSettings(QObject *parent);
	static TrafficSettings * m_ins;
public:
	static TrafficSettings *instance();
	~TrafficSettings();

	QString	rdnum() const;	
	QString	devId() const;
	QString	svrIp() const;	
	qint32	svrPort() const;
	qint32	InvContents() const;
	qint32	Cycle() const;
	qint32	DisTime() const;	
	qint32	TransMethod() const;	
	qreal	Longitude() const;	
	qreal	Latitude() const;	
	qint32	altitude() const;
	QString	pwd() const;
	QString	user() const;	


	qint32	LaneNum() const;
	QString	vLmsIp() const;	
	qint32	vLmsPort() const;	
	qint32	vLmsPtStart() const;	
	qint32	vLmsPtStop() const;	
	qint32	vLmsPtMid() const;	
	qint32	vLmsObstacleleft() const;	
	qint32	vLmsObstacleright() const;	
	qint32	vLmsHeight() const;	
	QString	qLmsIp() const;	
	qint32	qLmsPort() const;	
	qint32	qLmsPtStart() const;
	qint32	qLmsPtStop() const;	
	qint32	qLmsPtMid() const;	
	qint32	qLmsObstacleleft() const;	
	qint32	qLmsObstacleright() const;	
	qint32	qLmsHeight() const;	


	qint32 LaneWide() const;
	qint32 DisGap() const;

	qint32 Count() const;
public:
	void setConf(QString/*key*/,QString /*value*/);
	LasterSettings getLasterV();
	LasterSettings getLasterQ();

	DeviceSettings getDeviceInfo();

	qint32 FilterTypes(qint32);
	qint32 FilterSpeed(qint32);

	QString storepath();

	bool isdbg();
public slots:
	void setuser(QString);
	void setpwd(QString);
	void setrdnum(QString);
	void setdevId(QString);
	void setsvrIp(QString);	
	void setvLmsIp(QString);

	void setvLmsPort(qint32);
	void setvLmsPtStart(qint32);
	void setvLmsPtStop(qint32);
	void setvLmsPtMid(qint32);
	void setvLmsObstacleleft(qint32);
	void setvLmsObstacleright(qint32);
	void setvLmsHeight(qint32);	
	void setqLmsIp(QString);
	void setqLmsPort(qint32);
	void setqLmsPtStart(qint32);
	void setqLmsPtStop(qint32);
	void setqLmsPtMid(qint32);	
	void setqLmsObstacleleft(qint32);
	void setqLmsObstacleright(qint32);
	void setqLmsHeight(qint32);
	void setInvContents(qint32);
	void setLaneNum(qint32);
	void setCycle(qint32);
	void setDisTime(qint32);
	void setTransMethod(qint32);
	void setLongitude(qreal);
	void setLatitude(qreal);
	void setsvrPort(qint32);
	void setLaneWide(qint32);
	void setDisGap(qint32);
	void setCountInc();
protected slots:
	void slot_reload();
signals:
	void usdChanged(QString);
	void LongitudeChanged(qreal);			
	void vLmsPortChanged(qint32);
	void svrIpChanged(QString);
	void devIdChanged(QString);
	void rdnumChanged(QString);

	void vLmsPtMidChanged(qint32);
	void vLmsPtStartChanged(qint32);

	void vLmsPtStopChanged(qint32);
		void CycleChanged(qint32);	
	void DisTimeChanged(qint32);
	void TransMethodChanged(qint32);

	void qLmsPtMidChanged(qint32);
	void qLmsObstacleleftChanged(qint32);
	void qLmsObstaclerightChanged(qint32);
	void qLmsHeightChanged(qint32);
	void InvContentsChanged(qint32);
	void LaneNumChanged(qint32);

	void vLmsObstaclerightChanged(qint32);
	void vLmsHeightChanged(qint32);
	void qLmsIpChanged(QString);
	void qLmsPortChanged(qint32);
	void qLmsPtStartChanged(qint32);
	void qLmsPtStopChanged(qint32);
	void vLmsObstacleleftChanged(qint32);
	void svrPortChanged(qint32);
	void vLmsIpChanged(QString);
	void LatitudeChanged(qreal);
	void userChanged(QString);
	void pwdChanged(QString);

private:
	QString m_rdnum;
	QString m_devId;
	QString m_svrIp;
	qint32 m_svrPort;

	qint32 m_InvContents; //调查内容
	qint32 m_LaneNum; //车道数
	qint32 m_Cycle; //调查周期 
	qint32 m_DisTime; //跟车百分比鉴别时间
	qint32 m_TransMethod; //当前传输方式  0：网络TCP通信， 1：串口通信
	qreal  m_Longitude; //经度
	qreal m_Latitude; //纬度
	qint32 m_altitude;//高程
	QString m_user; //用户名
	QString m_pwd;	//密码
	qint32	m_LaneWide;
	qint32  m_disGap; //激光器间距

	QString m_vLmsIp;
	qint32 m_vLmsPort;
	qint32 m_vLmsPtStart;
	qint32 m_vLmsPtStop;
	qint32 m_vLmsPtMid;
	qint32 m_vLmsObstacleleft;
	qint32 m_vLmsObstacleright;
	qint32 m_vLmsHeight;

	QString m_qLmsIp;
	qint32 m_qLmsPort;
	qint32 m_qLmsPtStart;
	qint32 m_qLmsPtStop;
	qint32 m_qLmsPtMid;
	qint32 m_qLmsObstacleleft;
	qint32 m_qLmsObstacleright;
	qint32 m_qLmsHeight;

	QTimer m_reloadTimer;

	qint32 m_totalCount;
	qint32 m_minSpeed;
	qint32 m_maxSpeed;
	QMap<qint32/*待过滤车型*/,qint32/*转换后车型*/> m_FilterTypes;

	QString m_dbFile;

	bool m_dbg;
};

#endif // TRAFFICSETTINGS_H
