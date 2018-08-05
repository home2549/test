#ifndef REMOTEDBG_H
#define REMOTEDBG_H

#include <QObject>
#include <QTcpServer>
class Device;

class RemoteDbg : public QObject
{
	Q_OBJECT

public:
	static void Install(){
		instance();
	}
	static RemoteDbg* instance();
	~RemoteDbg();

public slots:
	void slot_dbg_v_bkg(const QByteArray &);
	void slot_dbg_q_bkg(const QByteArray &);
	void slot_dbg_veh_height(const QByteArray &);
	void slot_dbg_v_rtls(const QByteArray &);

signals:
	void signal_send(qint32,const QByteArray &);
protected slots:
	void slot_newVClient();
	void slot_VGone();

protected:
	RemoteDbg(QObject *parent);
private:
	QTcpServer *m_socketV;

	QMap<QTcpSocket *,Device*> m_visitor;

	static RemoteDbg *m_ins;
};

#endif // REMOTEDBG_H
