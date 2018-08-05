#ifndef TRAFFICALG_H
#define TRAFFICALG_H
//数据处理层
#include <QObject>
#include <QQueue>
#include "lasterdata.h"
#include "../vehicleinfo.h"
#include "head_struct.h"

class VirtualBackGround;
class LasterSettings;
class QTimer;

class TrafficAlg : public QObject
{
	Q_OBJECT

public:

#define FLY_MIN_Z_VAL	150

	TrafficAlg(QObject *parent);
	~TrafficAlg();

signals:
	void signal_VehicleDetect(bool);

	void signal_veh(const VehicleInfo &);

	void signal_calc();

	void sig_dbg_veh_height(const QByteArray &);
	void sig_dbg_v_rtls(const QByteArray &);
public slots:
	void slot_vLastData(const LasterData &);
	void slot_qLastData(const LasterData &);

protected slots:
	void process();
	void slot_alive();
	void slot_dayCheck();
protected:
	uint16 GetVehPattern(VehicleInfo *pVeh);
	uint16 GetVehPattern2(VehicleInfo *pVeh);
	void VehModels2(VehicleInfo *pVeh);
	uint16 simpleVehPattern(VehicleInfo *pVeh);
	int16 GetSkeletonMax_v(VehicleInfo *pVeh,int32 *pOutData);
	uint16 sortByDesc(int32 *pArray,int32 nStartIndex,int32 nStopIndex,int32 *pOutData);
	void GetUselessPercent(VehicleInfo *pVeh,int32 *pOutData);
	int32 GetMean(int32 * pArray,int16 nStartIndex,int16 nStopIndex);
	int16 GetTopCross_v(VehicleInfo *pVeh,int16 nFrameIndex,int32 *pOutData);
	int16 GetMaxThanValue(int32 *pArray,int nlen,int32 nVal);
	int16 GetBigDiffPos(int32 *pArray,int32 nlen,int16 nstart = 1);
	uint16 CheckForDaKe(VehicleInfo *pVeh);
	uint16 CheckForKongHuo(int32 *pArray,int32 nCounts);
	int16 GetIrregularCrossOfTop(VehicleInfo *pVeh,int32 *pOutArray);
//	uint16 GetLimitValue(int* pdata,int len,int startPos);
	uint16 GetPosFromXDistance(const QList<int32> & xDistent,uint16 start,uint16 end,int32 targetX/*wzf*/);
	uint16 correctPt(const QList<int32>  &g_Zdist,uint16 *left,uint16 *right,LasterSettings *pLasterParm);
	int32 GetMaxValue(const QList<int32> &pData,uint32 start,uint32 end);
	void clearVehicleErr();
	int16 GetMaxPos(int32 *pArray,int16 nlen,int16 nstart = 0);
	int16 GetMinPos(int32 *pArray,int16 nlen,int16 nstart = 0);

	void copy(int32 *pArray,QList<qint32>::const_iterator & iStart,QList<qint32>::const_iterator & iEnd);
	
	bool trySplit(const PointStruct *,const LasterData &,QList<PointStruct> & _return);

	//debug
	void submit_veh_height(VehicleInfo *pVeh);
	void submit_v_rtls(const LasterData &);
	void submit_q_rtls(const LasterData &);


	//在数组pArray中找比nVal小的且最大值点的位置
	qint16 GetLessthanValue(int32 *pArray,qint16 nlen,int32 nVal,qint16 nstart = 0);
	qint16 GetRLessthanValue(int32 *pArray,qint16 nlen,int32 nVal,qint16 nstart = -1);
	qint16 GetRBigDiffPos(int32 *pArray,int32 nlen);
	//获取最小值位置
	qint16 GetMinPos(int32 *pArray,int32 nlen,qint16 nstart = 0);
	//获取数组中从哪个点开始，始终大于nVal
	qint16 GetContinueBigThanVal(int32 *pArray,int32 nlen,int32 nVal,qint16 nstart = 0);
private:
	QList<VehicleInfo*> m_Vehicle;


	QQueue<LasterData> m_vCache;
	VirtualBackGround *m_vBackground;

	QQueue<LasterData> m_qCache;
	VirtualBackGround *m_qBackground;

	qint32	g_leftXOffset;
	qint32	g_rightXOffset;

	QTimer *m_dogTimer;
	int m_dogHandler;

	QTimer *m_dayTimer;
	QDate m_curDt;
};

#endif // TRAFFICALG_H
