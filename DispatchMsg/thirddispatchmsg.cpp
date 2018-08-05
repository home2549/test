#include "thirddispatchmsg.h"

class DispatchMsgUnit{

public:
	DispatchMsgUnit(QObject *nObj,const char* nhandler,ThirdDispatchMsg::DispatchMsgType nType){
		mobj = nObj;
		mhandle = nhandler;
		mMsgType = nType;
	}

	QObject *mobj;
	const char* mhandle;
	ThirdDispatchMsg::DispatchMsgType mMsgType;
};


QMap<ThirdDispatchMsg::DispatchMsgType,QList<DispatchMsgUnit*> > ThirdDispatchMsg::mSignals;

QMap<ThirdDispatchMsg::DispatchMsgType,QList<DispatchMsgUnit*> > ThirdDispatchMsg::mSlots;

ThirdDispatchMsg::ThirdDispatchMsg(QObject *parent)
	: QObject(parent)
{

}

ThirdDispatchMsg::~ThirdDispatchMsg()
{

}

void ThirdDispatchMsg::reg_signal(QObject *sender,const char* to_emit,DispatchMsgType signal_type)
{
	if(!mSignals.contains(signal_type)) {

		mSignals.insert(signal_type,QList<DispatchMsgUnit *>());
	}

	DispatchMsgUnit *tSignalUnit  = new DispatchMsgUnit(sender,to_emit,signal_type);
	mSignals[signal_type].append(tSignalUnit);

	if(mSlots.contains(signal_type)) {

		QListIterator<DispatchMsgUnit*> iEverySlotItem(mSlots[signal_type]);
		while(iEverySlotItem.hasNext()) {
		
			 DispatchMsgUnit* const & ref_slotUnit = iEverySlotItem.next();

			 QObject::connect(sender,to_emit,ref_slotUnit->mobj,ref_slotUnit->mhandle);
		}

	}
}

void ThirdDispatchMsg::reg_slot(QObject *receiver,const char* to_slot,DispatchMsgType slot_type)
{
	if(!mSlots.contains(slot_type)) {

		mSlots.insert(slot_type,QList<DispatchMsgUnit *>());
	}

	DispatchMsgUnit *tSlotUnit  = new DispatchMsgUnit(receiver,to_slot,slot_type);
	mSlots[slot_type].append(tSlotUnit);

	if(mSignals.contains(slot_type)) {

		QListIterator<DispatchMsgUnit*> iEverySignalItem(mSignals[slot_type]);
		while(iEverySignalItem.hasNext()) {
		
			 DispatchMsgUnit* const & ref_signalUnit = iEverySignalItem.next();

			 QObject::connect(ref_signalUnit->mobj,ref_signalUnit->mhandle,receiver,to_slot);
		}

	}
}

void ThirdDispatchMsg::reg_off(QObject * off_obj,DispatchMsgType off_type)
{

	if(mSignals.contains(off_type)){

		QList<DispatchMsgUnit*>::Iterator  iEveryItem = mSignals[off_type].begin();
		while(iEveryItem != mSignals[off_type].end()){

			 DispatchMsgUnit* &ref_SignalUnit = *iEveryItem;
			 if(ref_SignalUnit->mobj == off_obj)
			 {				 
				 if(mSlots.contains(off_type)) {

					QListIterator<DispatchMsgUnit*>  iEverySlotItem(mSlots[off_type]);
					while(iEverySlotItem.hasNext()){

						DispatchMsgUnit* const & ref_slotUnit  = iEverySlotItem.next();
						QObject::disconnect(ref_SignalUnit->mobj,ref_SignalUnit->mhandle,ref_slotUnit->mobj,ref_slotUnit->mhandle);
						//ref_SignalUnit->mobj->disconnect(ref_slotUnit->mobj);
					}
				 }

				 delete ref_SignalUnit;
				 mSignals[off_type].erase(iEveryItem);
				 return;
			 }
			 iEveryItem++;
		}
	}

	
	if(mSlots.contains(off_type)){

		QList<DispatchMsgUnit*>::Iterator  iEveryItem = mSlots[off_type].begin();
		while(iEveryItem != mSlots[off_type].end()){

			 DispatchMsgUnit* &ref_SlotUnit = *iEveryItem;
			 if(ref_SlotUnit->mobj == off_obj)
			 {				 
				 if(mSignals.contains(off_type)) {

					QListIterator<DispatchMsgUnit*>  iEverySignalItem(mSignals[off_type]);
					while(iEverySignalItem.hasNext()){

						DispatchMsgUnit* const & ref_SignalUnit  = iEverySignalItem.next();
						//off_obj->disconnect(ref_SignalUnit->mobj);//,ref_SignalUnit->mhandle
						QObject::disconnect(ref_SignalUnit->mobj,ref_SignalUnit->mhandle,ref_SlotUnit->mobj,ref_SlotUnit->mhandle);
					}
				 }

				 delete ref_SlotUnit;
				 mSlots[off_type].erase(iEveryItem);
				 return;
			 }
			
			 iEveryItem++;
		}
	}		
}