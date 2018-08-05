#include "mathhelper.h"


unsigned short Tabsin[181] =
{			  //0:0.5:90 度； 乘以32768；2的15次方，运算时左移15位； Tabsin[ ]>>15;
        3,           286,         572,         858,         1144,        1429,        1715,        2000,        2286,
        2571,        2856,        3141,        3425,        3709,        3993,        4277,        4560,        4843,
        5126,        5408,        5690,        5971,        6252,        6533,        6813,        7092,        7371,
        7650,        7927,        8204,        8481,        8757,        9032,        9307,        9580,        9854,
       10126,       10397,       10668,       10938,       11207,       11476,       11743,       12010,       12275,
       12540,       12803,       13066,       13328,       13589,       13848,       14107,       14365,       14621,
       14876,       15131,       15384,       15636,       15886,       16136,       16384,       16631,       16877,
       17121,       17364,       17606,       17847,       18086,       18324,       18560,       18795,       19028,
       19261,       19491,       19720,       19948,       20174,       20399,       20622,       20843,       21063,
       21281,       21498,       21713,       21926,       22138,       22348,       22556,       22763,       22967,
       23170,       23372,       23571,       23769,       23965,       24159,       24351,       24542,       24730,
       24917,       25102,       25285,       25466,       25645,       25822,       25997,       26170,       26341,
       26510,       26677,       26842,       27005,       27166,       27325,       27482,       27636,       27789,
       27939,       28088,       28234,       28378,       28520,       28660,       28797,       28932,       29066,
       29197,       29325,       29452,       29576,       29698,       29818,       29935,       30050,       30163,
       30274,       30382,       30488,       30592,       30693,       30792,       30888,       30983,       31075,
       31164,       31251,       31336,       31419,       31499,       31576,       31651,       31724,       31795,
       31863,       31928,       31991,       32052,       32110,       32166,       32219,       32270,       32319,
       32365,       32408,       32449,       32488,       32524,       32557,       32588,       32617,       32643,
       32667,       32688,       32707,       32723,       32737,       32748,       32757,       32763,       32767,
       32768
};

unsigned short Tabcos[181] =
{
       32768,       32767,       32763,       32757,       32748,       32737,       32723,       32707,       32688,
       32667,       32643,       32617,       32588,       32557,       32524,       32488,       32449,       32408,
       32365,       32319,       32270,       32219,       32166,       32110,       32052,       31991,       31928,
       31863,       31795,       31724,       31651,       31576,       31499,       31419,       31336,       31251,
       31164,       31075,       30983,       30888,       30792,       30693,       30592,       30488,       30382,
       30274,       30163,       30050,       29935,       29818,       29698,       29576,       29452,       29325,
       29197,       29066,       28932,       28797,       28660,       28520,       28378,       28234,       28088,
       27939,       27789,       27636,       27482,       27325,       27166,       27005,       26842,       26677,
       26510,       26341,       26170,       25997,       25822,       25645,       25466,       25285,       25102,
       24917,       24730,       24542,       24351,       24159,       23965,       23769,       23571,       23372,
       23170,       22967,       22763,       22556,       22348,       22138,       21926,       21713,       21498,
       21281,       21063,       20843,       20622,       20399,       20174,       19948,       19720,       19491,
       19261,       19028,       18795,       18560,       18324,       18086,       17847,       17606,       17364,
       17121,       16877,       16631,       16384,       16136,       15886,       15636,       15384,       15131,
       14876,       14621,       14365,       14107,       13848,       13589,       13328,       13066,       12803,
       12540,       12275,       12010,       11743,       11476,       11207,       10938,       10668,       10397,
       10126,        9854,        9580,        9307,        9032,        8757,        8481,        8204,        7927,
        7650,        7371,        7092,        6813,        6533,        6252,        5971,        5690,        5408,
        5126,        4843,        4560,        4277,        3993,        3709,        3425,        3141,        2856,
        2571,        2286,        2000,        1715,        1429,        1144,         858,         572,         286,
           3
};

MathHelper::MathHelper(QObject *parent)
	: QObject(parent)
{

}

MathHelper::~MathHelper()
{

}

qint32 MathHelper::Cos(qint32 nVal,qint32 nAngle)
{
	return nVal*Tabcos[abs(nAngle)]>>15;
}
qint32 MathHelper::ArcCos(qint32 nVal,qint32 nAngle)
{
	return (nVal<<15)/Tabcos[abs(nAngle)];
}


qint32 MathHelper::Sin(qint32 nVal,qint32 nAngle)
{
	return nVal*Tabsin[abs(nAngle)]>>15;
}

qint32 MathHelper::Tan(qint32 nVal,qint32 nAngle)
{
	return nVal*Tabsin[abs(nAngle)]/Tabcos[abs(nAngle)];
}
qint32 MathHelper::Density(const QList<qint32> & ndatas,qint32 & out_value)
{
	QList<qint32> t_Statics;
	QList<qint32> t_avg;

	t_Statics.clear();
	t_avg.clear();
	qint32 t_MaxIndex = 0;
	QListIterator<qint32>  iEveryItem(ndatas);
	while(iEveryItem.hasNext())
	{
		qint32 t_value = iEveryItem.next();
		qint32 t_index = 0;
		if(HasNearBy(t_avg,t_value,t_index))
		{
			t_avg[t_index]  = (t_avg.at(t_index)*t_Statics.at(t_index) + t_value)/(t_Statics.at(t_index)+1);		
			t_Statics[t_index]++;
			if(t_Statics.at(t_index) > t_Statics.at(t_MaxIndex))
			{
				t_MaxIndex  = t_index;
			}
		}
		else{		
			t_avg.append(t_value);
			t_Statics.append(1);			
		}
	}

	if(t_Statics.size() == 1){
		out_value = t_avg.at(0);
		return 100;
	}else if(t_Statics.size() > 1){
		out_value = t_avg.at(t_MaxIndex);
		return t_Statics.at(t_MaxIndex)*100/ndatas.size();
	}

	return 0;
}

bool MathHelper::HasNearBy(const QList<qint32> & ndatas,qint32 nValue,qint32 & out_index)
{
	out_index = 0;
	QListIterator<qint32>  iEveryItem(ndatas);
	while(iEveryItem.hasNext())
	{
		qint32 t_value = iEveryItem.next();
		if(abs(t_value - nValue ) < t_value*3/100){
			return true;
		}
		out_index++;
	}

	return false;
}