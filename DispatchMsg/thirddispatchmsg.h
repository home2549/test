#ifndef THIRDDISPATCHMSG_H
#define THIRDDISPATCHMSG_H

#include <QObject>
#include <QMap>
#include <QList>

class DispatchMsgUnit;

class  ThirdDispatchMsg : public QObject
{
public:
	ThirdDispatchMsg(QObject *parent);
	~ThirdDispatchMsg();

	enum DispatchMsgType{	
		BKG_V_TYPE, //����
		BKG_Q_TYPE, //����
		VEH_INFO,
		RTLS_V_TYPE, //��ǰ
		RTLS_Q_TYPE  //��ǰ
	};
	static void reg_signal(QObject *sender,const char* to_emit,DispatchMsgType signal_type);
	static void reg_slot(QObject *receiver,const char* to_slot,DispatchMsgType slot_type);
	static void reg_off(QObject * off_obj,DispatchMsgType off_type);
private:
	static QMap<DispatchMsgType,QList<DispatchMsgUnit*> > mSignals;
	static QMap<DispatchMsgType,QList<DispatchMsgUnit*> > mSlots;
};

#endif // THIRDDISPATCHMSG_H
