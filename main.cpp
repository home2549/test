
#include <QtCore/QCoreApplication>
#include "trafficsurvey.h"
#include "remotedbg.h"
int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	TrafficSurvey nSurveyIns;

#ifdef DEV_TEST	
	RemoteDbg::Install();
#endif
	return a.exec();
}
