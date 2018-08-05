#ifndef DATATETRY_H
#define DATATETRY_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QList>
#include <QDate>
template<class T>
class DataTetry : public QObject
{
public:
	DataTetry(QObject *parent = 0)
	{
		m_Caches.clear();
		m_SendTime.clear();
		m_tryCounts.clear();	
	}
	~DataTetry(){
	
	}
	void push(const T & nData){
		m_Caches.append(nData);
		m_SendTime.append(QDateTime::currentDateTime());
		m_tryCounts.append(0);

		if(m_Caches.size()==1){
			setShedule(true);
		}		
	}

	virtual void tryagin()= 0;
	virtual void fail() = 0;
	//³É¹¦
	virtual void success(QDate,qint32)=0;

	virtual void setShedule(bool) = 0;
	T tryData(){
		if(m_Caches.size())	{	
			return m_Caches[0];
		}
		return T();
	}

	T failData(){
		if(m_Caches.size()){		
			return m_Caches[0];
		}
		return T();
	}
	void remove(const T & nItem){
		qint32 t_pos = m_Caches.indexOf(nItem);
		if(t_pos != -1){
			m_Caches.removeAt(t_pos);
			m_SendTime.removeAt(t_pos);
			m_tryCounts.removeAt(t_pos);
		}

		if(m_Caches.size() == 0){		
			setShedule(false);
		}
	}
protected:
	 void sheduled(){
		
		if(m_SendTime.first().secsTo(QDateTime::currentDateTime()) > 20){
			if(m_tryCounts.first() < 3){
				m_tryCounts[0]++;
				m_SendTime[0] = QDateTime::currentDateTime();
				tryagin();				
			}else{
				QListIterator<T> iEveryItem(m_Caches);
				while(iEveryItem.hasNext()){
					fail();
					const T & ref_item = iEveryItem.next();
					m_Caches.removeOne(ref_item);
				}
				m_Caches.clear();
				m_SendTime.clear();
				m_tryCounts.clear();
				setShedule(false);
			}
		}
	}
protected:
	QList<T> m_Caches;
	QList<QDateTime> m_SendTime;
	QList<qint32> m_tryCounts;

	qint32 m_timerID;
};

#endif // DATATETRY_H
