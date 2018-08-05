#include "trafficsettings.h"
#include <QSettings>
#include <QStringList>
#include "DBDefines.h"
QString	LasterSettings::LmsIp() const
{
	return m_LmsIp;
}

qint32	LasterSettings::LmsPort() const
{
	return m_LmsPort;
}
qint32	LasterSettings::LmsPtStart() const
{
	return m_LmsPtStart;
}

qint32	LasterSettings::LmsPtStop() const
{
	return m_LmsPtStop;
}

qint32	LasterSettings::LmsPtMid() const
{
	return m_LmsPtMid;
}

qint32	LasterSettings::LmsObstacleleft() const
{
	return m_LmsObstacleleft;
}

qint32	LasterSettings::LmsObstacleright() const
{
	return m_LmsObstacleright;
}

qint32	LasterSettings::LmsHeight() const
{
	return m_LmsHeight;
}



QString	DeviceSettings::rdnum() const
{
	return m_rdnum;
}

QString	DeviceSettings::devId() const
{
	return m_devId;
}

QString	DeviceSettings::svrIp() const
{
	return m_svrIp;
}
qint32	DeviceSettings::InvContents() const
{
	return m_InvContents;
}
qint32	DeviceSettings::Cycle() const
{
	return m_Cycle;
}
qint32	DeviceSettings::DisTime() const
{
	return m_DisTime;
}	
qint32	DeviceSettings::TransMethod() const
{
	return m_TransMethod;
}
qreal	DeviceSettings::Longitude() const
{
	return m_Longitude;
}	
qreal	DeviceSettings::Latitude() const
{
	return m_Latitude;
}
qint32	DeviceSettings::altitude() const
{
	return m_altitude;
}
qint32	DeviceSettings::LaneNum() const
{
	return m_LaneNum;
}

TrafficSettings * TrafficSettings::m_ins = NULL;
TrafficSettings::TrafficSettings(QObject *parent)
	: QObject(parent)
{
	m_reloadTimer.setSingleShot(true);
	connect(&m_reloadTimer,SIGNAL(timeout()),this,SLOT(slot_reload()));
	slot_reload();
	//m_reloadTimer.setInterval(60*60*1000);
}

TrafficSettings::~TrafficSettings()
{

}

TrafficSettings *TrafficSettings::instance()
{
	if(!m_ins){
		m_ins = new TrafficSettings(0);
	}
	return m_ins;
}

QString	TrafficSettings::rdnum() const
{
	return m_rdnum;
	
}
QString	TrafficSettings::devId() const
{
	return m_devId;
	
}
QString	TrafficSettings::svrIp() const
{
return m_svrIp;
	
}
qint32	TrafficSettings::svrPort() const
{
	return m_svrPort;	
}	


qint32	TrafficSettings::vLmsPtStart() const
{
	return m_vLmsPtStart;	
	
}
qint32	TrafficSettings::vLmsPtStop() const
{
	return  m_vLmsPtStop;
	
}	
qint32	TrafficSettings::vLmsPtMid() const
{
	return  m_vLmsPtMid;
	
}	
qint32	TrafficSettings::vLmsObstacleleft() const
{
	return  m_vLmsObstacleleft;
	
}	
qint32	TrafficSettings::vLmsObstacleright() const
{
	return m_vLmsObstacleright;
	
}	
qint32	TrafficSettings::vLmsHeight() const
{
	return m_vLmsHeight;
	
}
QString	TrafficSettings::qLmsIp() const
{
	return  m_qLmsIp;
	
}	
qint32	TrafficSettings::qLmsPort() const
{
	return m_qLmsPort;
}	
qint32	TrafficSettings::qLmsPtStart() const
{
	return m_qLmsPtStart;
}
qint32	TrafficSettings::qLmsPtStop() const
{
	return m_qLmsPtStop;
}	
qint32	TrafficSettings::qLmsPtMid() const
{
	return m_qLmsPtMid;
}
qint32	TrafficSettings::qLmsObstacleleft() const
{
	return m_qLmsObstacleleft;
}	
qint32	TrafficSettings::qLmsObstacleright() const
{
	return m_qLmsObstacleright;
}	
qint32	TrafficSettings::qLmsHeight() const
{
	return m_qLmsHeight;
}	
qint32	TrafficSettings::InvContents() const
{	
	return m_InvContents;	
}
qint32	TrafficSettings::LaneNum() const
{
	return m_LaneNum;	
}	
qint32	TrafficSettings::Cycle() const
{
	return m_Cycle;
}
qint32	TrafficSettings::DisTime() const
{
	return m_DisTime;
}	
qint32	TrafficSettings::TransMethod() const
{
	return m_TransMethod;
}	
qreal	TrafficSettings::Longitude() const
{
	return m_Longitude;
}	
qint32	TrafficSettings::LaneWide() const
{
	return m_LaneWide;
}	
qreal	TrafficSettings::Latitude() const
{
	return m_Latitude;
}
qint32	TrafficSettings::altitude() const
{
	return m_altitude;
}	


QString	TrafficSettings::pwd() const
{
	return m_pwd;
}
QString	TrafficSettings::user() const
{
	return m_user;
}	

QString	TrafficSettings::vLmsIp() const
{
	return m_vLmsIp;
}
qint32	TrafficSettings::vLmsPort() const
{
	return m_vLmsPort;
}

void TrafficSettings::setsvrPort(qint32 arg)
{
	if(m_svrPort != arg){
		m_svrPort = arg;
		emit svrPortChanged(arg);
	}
}
void TrafficSettings::setuser(QString arg)
{
	if(m_user != arg){
		m_user = arg;
		emit userChanged(arg);
	}
}

void TrafficSettings::setpwd(QString arg)
{
	if(m_pwd != arg){
		m_pwd = arg;
		emit pwdChanged(arg);
	}	
}

void TrafficSettings::setrdnum(QString arg)
{
	if(m_rdnum != arg){
		m_rdnum = arg;
		emit rdnumChanged(arg);
	}	
}
void TrafficSettings::setdevId(QString arg)
{
	if(m_devId != arg){
		m_devId = arg;
		emit devIdChanged(arg);
	}	
}
void TrafficSettings::setsvrIp(QString arg)
{
	if(this->m_svrIp != arg){
		this->m_svrIp = arg;
		emit svrIpChanged(arg);
	}
}	
void TrafficSettings::setvLmsIp(QString arg)
{
	if(m_vLmsIp != arg)
	{
		m_vLmsIp = arg;
		emit vLmsIpChanged(arg);
	}
}
void TrafficSettings::setvLmsPort(qint32 arg)
{
	if(m_vLmsPort != arg)
	{
		m_vLmsPort = arg;
		emit vLmsPortChanged(arg);
	}
}
void TrafficSettings::setvLmsPtStart(qint32 arg)
{
	if(this->m_vLmsPtStart != arg)
	{
		m_vLmsPtStart = arg;
		emit vLmsPtStartChanged(arg);
	}
}

void TrafficSettings::setvLmsPtStop(qint32 arg)
{
	if(this->m_vLmsPtStop != arg)
	{
		m_vLmsPtStop = arg;
		emit vLmsPtStopChanged(arg);
	}
}

void TrafficSettings::setvLmsPtMid(qint32 arg)
{
	if(this->m_vLmsPtMid != arg)
	{
		m_vLmsPtMid = arg;
		emit vLmsPtMidChanged(arg);
	}
}
void TrafficSettings::setvLmsObstacleleft(qint32 arg)
{
	if(this->m_vLmsObstacleleft != arg)
	{
		m_vLmsObstacleleft = arg;
		emit vLmsObstacleleftChanged(arg);
	}
};
void TrafficSettings::setvLmsObstacleright(qint32 arg)
{
	if(this->m_vLmsObstacleright != arg)
	{
		m_vLmsObstacleright = arg;
		emit vLmsObstaclerightChanged(arg);
	}
}
void TrafficSettings::setvLmsHeight(qint32 arg)
{
	if(this->m_vLmsHeight != arg)
	{
		m_vLmsHeight = arg;
		emit vLmsHeightChanged(arg);
	}
}	
void TrafficSettings::setqLmsIp(QString arg)
{
	if(this->m_qLmsIp != arg)
	{
		m_qLmsIp = arg;
		emit qLmsIpChanged(arg);
	}
}
void TrafficSettings::setqLmsPort(qint32 arg)
{
	if(this->m_qLmsPort != arg)
	{
		m_qLmsPort = arg;
		emit qLmsPortChanged(arg);
	}
}
void TrafficSettings::setqLmsPtStart(qint32 arg)
{
	if(this->m_qLmsPtStart != arg)
	{
		m_qLmsPtStart = arg;
		emit qLmsPtStartChanged(arg);
	}
}
void TrafficSettings::setqLmsPtStop(qint32 arg)
{
	if(this->m_qLmsPtStop != arg)
	{
		m_qLmsPtStop = arg;
		emit qLmsPtStopChanged(arg);
	}
}
void TrafficSettings::setqLmsPtMid(qint32 arg)
{
	if(this->m_qLmsPtMid != arg)
	{
		m_qLmsPtMid = arg;
		emit qLmsPtMidChanged(arg);
	}
}
void TrafficSettings::setqLmsObstacleleft(qint32 arg)
{
	if(this->m_qLmsObstacleleft != arg)
	{
		m_qLmsObstacleleft = arg;
		emit qLmsObstacleleftChanged(arg);
	}
}
void TrafficSettings::setqLmsObstacleright(qint32 arg)
{
	if(this->m_qLmsObstacleright != arg)
	{
		m_qLmsObstacleright = arg;
		emit qLmsObstaclerightChanged(arg);
	}
}
void TrafficSettings::setqLmsHeight(qint32 arg)
{
	if(this->m_qLmsHeight != arg)
	{
		m_qLmsHeight = arg;
		emit qLmsHeightChanged(arg);
	}
}
void TrafficSettings::setInvContents(qint32 arg)
{
	if(this->m_InvContents != arg)
	{
		m_InvContents = arg;
		emit InvContentsChanged(arg);
	}
}
void TrafficSettings::setLaneNum(qint32 arg)
{
	if(this->m_LaneNum != arg)
	{
		m_LaneNum = arg;
		emit LaneNumChanged(arg);
	}
}
void TrafficSettings::setCycle(qint32 arg)
{
	if(this->m_Cycle != arg)
	{
		m_Cycle = arg;
		emit CycleChanged(arg);
	}
}
void TrafficSettings::setDisTime(qint32 arg)
{
	if(this->m_DisTime != arg)
	{
		m_DisTime = arg;
		emit DisTimeChanged(arg);
	}
}
void TrafficSettings::setTransMethod(qint32 arg)
{
	if(this->m_TransMethod != arg)
	{
		m_TransMethod = arg;
		emit TransMethodChanged(arg);
	}
}
void TrafficSettings::setLongitude(qreal arg)
{
	if(this->m_Longitude != arg)
	{
		m_Longitude = arg;
		emit LongitudeChanged(arg);
	}	
}
void TrafficSettings::setLaneWide(qint32 arg)
{
	if(this->m_LaneWide != arg)
	{
		m_LaneWide = arg;
	}
}

void TrafficSettings::setLatitude(qreal arg)
{
	if(this->m_Latitude != arg)
	{
		m_Latitude = arg;
	}
	emit LatitudeChanged(arg);
}

void TrafficSettings::setConf(QString/*key*/,QString /*value*/)
{
	if(!m_reloadTimer.isActive())
	{
		m_reloadTimer.start();	
	}
}

void TrafficSettings::setDisGap(qint32 nVal)
{
	m_disGap = nVal;
}
qint32 TrafficSettings::DisGap() const
{
	return m_disGap;
}
void TrafficSettings::slot_reload()
{
	QSettings settings("cfg.ini",QSettings::IniFormat);

	m_rdnum = settings.value("TRAFFIC/RDNUM","G102L214130229").toString();
	m_devId = settings.value("TRAFFIC/DEVID","0171170313050005").toString();
	m_svrIp = settings.value("TRAFFIC/DSCIP","192.168.0.11").toString();
	m_svrPort = settings.value("TRAFFIC/DSCPORT","3134").toInt();
	m_InvContents = settings.value("TRAFFIC/INVCONTENT","1").toInt();
	m_LaneNum = settings.value("TRAFFIC/LANE","4").toInt();
	m_Cycle = settings.value("TRAFFIC/PROCYCLE","4").toInt();
	m_DisTime = settings.value("TRAFFIC/DISTIME","10").toInt();
	m_TransMethod = settings.value("TRAFFIC/CNNTYPE","0").toInt();
	m_Longitude = settings.value("TRAFFIC/LONGITUDE","0.0").toReal();
	m_Latitude = settings.value("TRAFFIC/LATITUDE","0.0").toReal();
	m_altitude = settings.value("TRAFFIC/ALTITUDE","0.0").toReal();
	m_user = settings.value("TRAFFIC/USER","admin").toString();
	m_pwd = settings.value("TRAFFIC/PWD","admin").toString();
	m_LaneWide = settings.value("TRAFFIC/LANEWIDE","3500").toInt();
	m_disGap = settings.value("TRAFFIC/LMS2DIS","300").toInt();

	m_vLmsIp = settings.value("TRAFFIC/LMSVIP","192.168.0.2").toString();
	m_vLmsPort = settings.value("TRAFFIC/LMSVPORT","2122").toInt();
	m_vLmsPtStart = settings.value("TRAFFIC/LMSVSTART","80").toInt();
	m_vLmsPtStop = settings.value("TRAFFIC/LMSVSTOP","300").toInt();
	m_vLmsPtMid = settings.value("TRAFFIC/LMSVMID","180").toInt();
	m_vLmsObstacleleft = settings.value("TRAFFIC/LMSVEXCL","0").toInt();
	m_vLmsObstacleright = settings.value("TRAFFIC/LMSVEXCR","0").toInt();
	m_vLmsHeight = settings.value("TRAFFIC/LMSVHIGHT","0").toInt();

	m_qLmsIp = settings.value("TRAFFIC/LMSQIP","192.168.0.3").toString();
	m_qLmsPort = settings.value("TRAFFIC/LMSQPORT","2122").toInt();
	m_qLmsPtStart = settings.value("TRAFFIC/LMSQSTART","80").toInt();
	m_qLmsPtStop = settings.value("TRAFFIC/LMSQSTOP","300").toInt();
	m_qLmsPtMid = settings.value("TRAFFIC/LMSQMID","180").toInt();
	m_qLmsObstacleleft = settings.value("TRAFFIC/LMSQEXCL","0").toInt();
	m_qLmsObstacleright = settings.value("TRAFFIC/LMSQEXCR","0").toInt();
	m_qLmsHeight = settings.value("TRAFFIC/LMSQHIGHT","0").toInt();

	//m_totalCount = settings.value("STATE/TOTAL","1").toInt();
	m_minSpeed = settings.value("FIX/MINSPEED","0").toInt();
	m_maxSpeed = settings.value("FIX/MAXSPEED","150").toInt();
		
	//车型过滤
	m_FilterTypes.clear();
	QString nfilter = settings.value("FIX/TYPES","").toString();
	if(!nfilter.isEmpty()){
	
		QStringList nfilterlist =	nfilter.split("*");

		QStringListIterator iEveryItem(nfilterlist);
		while(iEveryItem.hasNext()){
		
			QString ntypes = iEveryItem.next();
			QStringList nPair = ntypes.split(":");

			m_FilterTypes.insert(nPair[0].toInt(),nPair[1].toInt());
		}
	}

	m_dbFile = settings.value("DATA/DB","./"+DBDefines::DBNAME).toString();

	m_dbg = settings.value("DEBUG/STATE",false).toBool();
}

bool TrafficSettings::isdbg()
{
	return m_dbg;
}

LasterSettings TrafficSettings::getLasterV()
{
	LasterSettings ret_settings;
	ret_settings.m_LmsHeight = this->vLmsHeight() ;
	ret_settings.m_LmsIp = this->vLmsIp();
	ret_settings.m_LmsObstacleleft = this->vLmsObstacleleft();
	ret_settings.m_LmsObstacleright = this->vLmsObstacleright();
	ret_settings.m_LmsPort = this->vLmsPort();
	ret_settings.m_LmsPtMid = this->vLmsPtMid();
	ret_settings.m_LmsPtStart = this->vLmsPtStart();
	ret_settings.m_LmsPtStop = this->vLmsPtStop() ;

	return ret_settings;
}

LasterSettings TrafficSettings::getLasterQ()
{
	LasterSettings ret_settings;
	ret_settings.m_LmsHeight = this->qLmsHeight() ;
	ret_settings.m_LmsIp = this->qLmsIp();
	ret_settings.m_LmsObstacleleft = this->qLmsObstacleleft();
	ret_settings.m_LmsObstacleright = this->qLmsObstacleright();
	ret_settings.m_LmsPort = this->qLmsPort();
	ret_settings.m_LmsPtMid = this->qLmsPtMid();
	ret_settings.m_LmsPtStart = this->qLmsPtStart();
	ret_settings.m_LmsPtStop = this->qLmsPtStop() ;

	return ret_settings;
}

DeviceSettings TrafficSettings::getDeviceInfo()
{
	DeviceSettings ret_settings;

	ret_settings.m_rdnum = this->rdnum();
	ret_settings.m_devId = this->devId();
	ret_settings.m_svrIp = this->svrIp();
	ret_settings.m_InvContents = this->InvContents(); //调查内容
	ret_settings.m_Cycle = this->Cycle(); //调查周期 
	ret_settings.m_DisTime = this->DisTime(); //跟车百分比鉴别时间
	ret_settings.m_TransMethod = this->TransMethod(); //当前传输方式
	ret_settings.m_Longitude = this->Longitude(); //经度
	ret_settings.m_Latitude = this->Latitude(); //纬度
	ret_settings.m_altitude = this->altitude();//高程
	ret_settings.m_LaneNum = this->LaneNum(); //车道数	

	return ret_settings;
}
qint32 TrafficSettings::FilterTypes(qint32 nType)
{
	if(m_FilterTypes.contains(nType)){
		return m_FilterTypes.value(nType);
	}
	return nType;
}
qint32 TrafficSettings::FilterSpeed(qint32 nSpeed)
{
	if(m_minSpeed > nSpeed){		
		return m_minSpeed;
	} else if(m_maxSpeed < nSpeed){
		return m_maxSpeed;
	}

	return  nSpeed;
}

qint32 TrafficSettings::Count() const
{
	return m_totalCount;
}

void TrafficSettings::setCountInc()
{
	m_totalCount++;
}

QString TrafficSettings::storepath()
{
	return m_dbFile;
}