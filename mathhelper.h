#ifndef MATHHELPER_H
#define MATHHELPER_H

#include <QObject>

class MathHelper : public QObject
{
public:
	MathHelper(QObject *parent);
	~MathHelper();
	static qint32 Cos(qint32 nVal,qint32 nAngle);
	static qint32 ArcCos(qint32 nVal,qint32 nAngle);

	static qint32 Sin(qint32 nVal,qint32 nAngle);
	static qint32 Tan(qint32 nVal,qint32 nAngle);
	static qint32 Density(const QList<qint32> &,qint32 & out_value);

	static bool HasNearBy(const QList<qint32> &,qint32 nValue,qint32 & out_index);
private:
	
};

#endif // MATHHELPER_H
