#ifndef DEVPROTOCOL_H
#define DEVPROTOCOL_H

#include <QObject>
#include "lasterdata.h"

class DevProtocol : public QObject
{
	Q_OBJECT

public:
	DevProtocol(QObject *parent);
	~DevProtocol();

	static void serialize(const QList<qint32> &,QByteArray &);
signals:
	void signal_data(const LasterData &);
public slots:
	void slot_NewFrame(const QByteArray &);
private:
	QByteArray  m_CacheData;
	
};

#endif // DEVPROTOCOL_H
