#include "trafficprotocol.h"
#include <QDataStream>
#include <QDebug>
#include <QDateTime>
#include <QHostAddress>

#define CRC_OK      0
#define CRC_ERROR   1

const uint16 crctab[256] =
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

/* CRC  ��λ�ֽ�ֵ�� */
static unsigned char auchCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40
};
/* CRC ��λ�ֽ�ֵ��*/
static unsigned char auchCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
    0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
    0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
    0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
    0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
    0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
    0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
    0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
    0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
    0x40
};

//uint8 b2bcd(uint8 b)   //uint8ת��ΪBCD��
//{
//	return (((b/10)<<4)|(b%10));
//}
//========================================================================
// ��BCD��ת��������
void Bcd2Array(uint32 bcd, uint8 *buf, uint8 len)
{
    uint8 i;

    if (len == 0)
    {
        return;
    }

    for (i=0; i<len; i++)
    {
        buf[i] = ((bcd>>((len-1-i)<<2))&0x0f) + '0';
    }
}

uint8 bcd2b(uint8 bcd)
{
    return (((bcd>>4)&0x0f)*10 + (bcd&0x0f));
}


// NUM���BCD������
void Nm2Bcd(uint32 num, uint8 *buf, uint8 len)
{
    uint8 i;

    if (len == 0)
    {
        return;
    }

    // ��ʮ����ֵ
    for (i=0; i<len; i++)
    {
        buf[len-1-i] = (num%10);
        num = num/10;
    }
}
//======================================================================
// ����crc16��ֵ
uint16 crc_16(uint8 *ptr, uint16 len)
{
    uint8 da;
    uint16 crc;

    crc = 0;
    while ((len--) != 0)
    {
        // �ݴ�߰�λ
        da = (uint8)(crc>>8);
        // ���ư�λ
        crc <<= 8;
        // ����XOR
        crc ^= crctab[da^(*ptr)];
        ptr++;
    }

    return crc;
}

// ����У����
void crc_create(uint8 *ptr, uint16 len)
{
    uint16 crc;
    uint8 buf[5];

    if (len == 0)
    {
        return;
    }

    // �õ�CRC��
    crc = crc_16(ptr, len);
 
    // �����BCD
    Nm2Bcd(crc, buf, 5);

    *(ptr+len) = buf[0];
    *(ptr+len+1) = (buf[1] << 4)|buf[2];
    *(ptr+len+2) = (buf[3] << 4)|buf[4];

}

// ����CRCУ��
uint8 crc_check(uint8 *ptr, uint16 len)
{
    uint16 crc1, crc2;

    if (len <= 3)
    {
        return CRC_ERROR;
    }

    // ������ʵcrcֵ
    crc1 = crc_16(ptr, len-3);
    // ʵ��crcֵ
    crc2 = *(ptr+len-3)*10000 + bcd2b(*(ptr+len-2))*100 + bcd2b(*(ptr+len-1));

    if (crc1^crc2)
    {
        return CRC_ERROR;
    }
    else
    {
        return CRC_OK;
    }
}

unsigned short CRC16(unsigned char *puchMsg, unsigned short usDataLen)
//unsigned char *puchMsg; /*  Ҫ���� CRC У�����Ϣ */
//unsigned short usDataLen; /*  ��Ϣ���ֽ��� */
{
    unsigned char uchCRCHi = 0xFF ; /* ��CRC�ֽڳ�ʼ�� */
    unsigned char uchCRCLo = 0xFF ; /* ��CRC �ֽڳ�ʼ�� */
    unsigned uIndex ; /* CRCѭ���е����� */
    while (usDataLen--) /* ������Ϣ������ */
    {
        uIndex = uchCRCLo ^ *puchMsg++ ; /* ����CRC */
        uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex] ;
        uchCRCHi = auchCRCLo[uIndex] ;
    }
   // return (uchCRCHi << 8 | uchCRCLo) ;
	return (uchCRCHi  | (uchCRCLo << 8)) ;
}

//С��ģʽ
class FrameFormat{

public:
	enum{
		HEADER = 0xAAAA,
		TAIL = 0xEEEE,
		//�����뷴����
		CYCLE_U = 0x01,
		CYCLE_D = 0x0A,
		SINGLE_U = 0x11,
		SINGLE_D = 0x1A,
		//������
		DSC_D	=	0x02,
		RD_U	= 0x08,
		//ָ���뷴����
		CMD_RDNUM = 0x03, //DSC����RD���޸�RDվ����
		CMD_IP = 0x04, //DSC����RD���޸�RD����IP
		CMD_TIME = 0x05, //DSC����RD���޸�RDʱ��
		CMD_CTX = 0x06, //DSC����RD���޸�RD��ͨ���ݲɼ�����
		CMD_CYCLE = 0x07, //DSC����RD���޸�RD��ͨ���ݴ�������
		CMD_DISTIME = 0x0B, //DSC����RD���޸�RD�����ٷֱȵĳ�ͷʱ���б�ָ��
		CMD_ACK = 0x08, //RD�յ�DSC��ָ�����ݰ�����DSC������Ӧ�����ݰ�������DSCȷ��RD�޸ĺ�����в���
		CMD_STATIC_RESEND = 0x09, //DSC����RD������ȡ���ض�ʱ�ε�ʵʱ��ͨ����
		CMD_STATIC_U = 0x01, //RD����DSC�����ڴ���RD�ɼ��ʹ���Ľ�ͨ����
		CMD_STATIC_D	=	0x0A,//DSC����RD��ΪDSC�յ�RD��ʵʱ��ͨ���ݰ���ķ�����Ϣ
		CMD_SINGLE_RESEND	=	0x19,//������ͨ�������´���ָ�����ݰ�
		CMD_SINGLE_SEND	=	0x11,//������ͨ���ݰ�
		CMD_SINGLE_ACK	=	0x1A,//���յ�������У�鷴����Ϣ���ݰ�
	};

};


TrafficProtocol::TrafficProtocol(QObject *parent)
	: QObject(parent),m_dbg(false)
{
	 qRegisterMetaType<CMD_03>("CMD_03");
	 qRegisterMetaType<CMD_04>("CMD_04");
	 qRegisterMetaType<CMD_05>("CMD_05");
	 qRegisterMetaType<CMD_06>("CMD_06");
	 qRegisterMetaType<CMD_07>("CMD_07");
	 qRegisterMetaType<CMD_0B>("CMD_0B");
}

TrafficProtocol::~TrafficProtocol()
{

}

void TrafficProtocol::setdbg(bool en_out)
{
	m_dbg = en_out;
}
void TrafficProtocol::serialize(const QByteArray &data,QByteArray &_return)
{
	QDataStream out(&_return,QIODevice::WriteOnly);
	out.setByteOrder(QDataStream::LittleEndian);
	quint16 t_header = FrameFormat::HEADER;
	quint16 t_tail = FrameFormat::TAIL;
	quint16 t_length = data.size();

	out<<t_header<<t_length;
	out.writeRawData(data.data(),t_length);
	quint16 t_crc = CRC16((unsigned char *)_return.data()+2,t_length+2);
	out<<t_crc<<t_tail;
}

void TrafficProtocol::serialize_08(const DeviceSettings & nSettings ,QByteArray & _return)
{
	QByteArray t_body;
	QDataStream out(&t_body,QIODevice::WriteOnly);
	out.setByteOrder(QDataStream::LittleEndian);

	quint8 t_type = FrameFormat::RD_U;
	QDateTime cur_date = QDateTime::currentDateTime();
	quint16 t_year = cur_date.date().year();
	quint8 t_month = cur_date.date().month();
	quint8 t_day = cur_date.date().day();
	quint8 t_hour = cur_date.time().hour();
	quint8 t_min = cur_date.time().minute();
	quint8 t_sec = cur_date.time().second();
	out<<t_type;
	out.writeBytes(nSettings.devId().toAscii().data(),16);
	out<<t_year<<t_month<<t_day<<t_hour<<t_min<<t_sec;

	quint8 t_cycle = nSettings.Cycle();
	quint8 t_Invctx = nSettings.InvContents();
	quint32 t_ip = QHostAddress(nSettings.svrIp()).toIPv4Address();
	out<<t_cycle<<t_Invctx<<t_ip;
	out.writeBytes(nSettings.rdnum().toAscii().data(),15);

	quint8 t_DisTime = nSettings.DisTime();
	quint8 t_Trans = nSettings.TransMethod();
	qint16	t_altitude = nSettings.altitude();

	out<<t_DisTime<<t_Trans<<nSettings.Latitude()<<nSettings.Longitude()<<t_altitude;

	serialize(t_body,_return);
}
void TrafficProtocol::serialize_02(const DeviceSettings & nSettings ,QByteArray & _return)
{
	QByteArray t_body;

	QDataStream out(&t_body,QIODevice::WriteOnly);
	out.setByteOrder(QDataStream::LittleEndian);

	quint8 t_type = FrameFormat::DSC_D;
	quint8 t_code = 0x01;
	out<<t_type;
	out.writeBytes(nSettings.devId().toAscii().data(),16);
	out<<t_code;
	serialize(t_body,_return);
}

quint16 TrafficProtocol::serialize_01(const StaticsLaneStruct*  g_StaticsInfo,const DeviceSettings & nSettings ,QByteArray & _return,TotoalStaticsInfo & db_info)
{   
	static int type2PkgIndex[] = { TYPE_XIAO_KE,TYPE_XIAO_HUO,TYPE_DA_KE,
		TYPE_ZHONG_HUO,TYPE_DA_HUO,TYPE_TE_DA_HUO,
		TYPE_JI_ZHUANG_XIANG,TYPE_TUO_TA_JI,TYPE_MO_TUO};
	uint8 type_cur = 0;
	uint8 i=0;
	uint8 j=0;
	uint8   frame[1024]={0};
	uint16  l_avgDis = 0;
	//      uint8   stat;
	uint16  len;

	QDateTime cur_tm = QDateTime::currentDateTime();
	quint16 t_year = cur_tm.date().year();
	quint8 t_month = cur_tm.date().month();
	quint8 t_day = cur_tm.date().day();

	uint16 TimeNum_temp =(cur_tm.time().hour()*60 + cur_tm.time().minute())/ nSettings.Cycle();//ʱ����ţ���1-1440

	len = 0;
	frame[len++] = FrameFormat::CYCLE_U;
	//�豸���ʶ����			
	memcpy(frame+len,nSettings.devId().toAscii().data(),16);
	len += 16;
	//վ����
	memcpy(frame+len,nSettings.rdnum().toAscii().data(),15);
	len += 15;
	//�豸Ӳ���������
	frame[len++] = 0x00;

	//��������
	frame[len++] = nSettings.InvContents();

	//��
	frame[len++] = t_year;
	frame[len++] = (t_year>>8);
	//��
	frame[len++] = t_month;
	//��
	frame[len++] = t_day;

	//��ͨ���ݴ�������
	frame[len++] = nSettings.Cycle();
	//ʱ����� ��λ��ǰ
	frame[len++] = TimeNum_temp&0xff;
	frame[len++] = (TimeNum_temp>>8)&0xff;

	//������
	frame[len++] = nSettings.LaneNum();  //42�ֽ�

	for(i=0;i<nSettings.LaneNum();i++)
	{	
		VehStaticsInfo t_dbItem;
		// ��������ת���ɹ��공����
		frame[len++] = get_lane(i+1);
		t_dbItem.LANENUM = get_lane(i+1);
		//�����ٷֱ�
		if(g_StaticsInfo[i].nTotalVehCnt){
			frame[len++] = g_StaticsInfo[i].nGenCheCnt*100/g_StaticsInfo[i].nTotalVehCnt;
			t_dbItem.GENCHEBI = frame[len - 1];
		} else {
			frame[len++] = 0;
			t_dbItem.GENCHEBI = 0;
		}

		//ƽ����ͷ���
		if(g_StaticsInfo[i].nTotalVehCnt){
			l_avgDis = g_StaticsInfo[i].nDisheadVeh/g_StaticsInfo[i].nTotalVehCnt;
			frame[len++] = l_avgDis&0xff;
			frame[len++] = (l_avgDis>>8)&0xff;
			t_dbItem.AVGDIS = l_avgDis;
		} else{
			l_avgDis = (nSettings.Cycle()*60*1000 +g_StaticsInfo[i].nLastVehEnterTm)*g_StaticsInfo[i].nLastVehSpeed/(3600);
			frame[len++] = l_avgDis&0xff;
			frame[len++] = (l_avgDis>>8)&0xff;
			t_dbItem.AVGDIS = l_avgDis;
		}

		//ʱ��ռ����
		if(g_StaticsInfo[i].nVehPassTm)
		{
			frame[len++] = g_StaticsInfo[i].nVehPassTm/(nSettings.Cycle()*60*1000);
			t_dbItem.TIMEPERCENT = frame[len -1];
		}
		else
		{
			frame[len++] = 0;
			t_dbItem.TIMEPERCENT = 0;
		}

		//uint32 g_total_veh[4][9]={0}; 
		//***�ų���������С�ͳ�	С����	��ͳ�	���ͻ���	���ͻ���	�ش��ͻ���	��װ�䳵	������	Ħ�г�
		//�����ͣ�����Э�飩��С�ͳ��� ���ͳ������ͳ����ش��ͳ�����������Ħ�г�

		//��С�ͽ�ͨ��
		for(j=0;j<MAX_VEH_TYPE;j++)
		{
			simpleStaticsInfo t_vehItem;
			type_cur = type2PkgIndex[j] - 1;
			frame[len++] = (g_StaticsInfo[i].nVehCnt[type_cur])&0xff;  //��ͨ��	 //��λ��ǰ
			frame[len++] = ((g_StaticsInfo[i].nVehCnt[type_cur])>>8)&0xff;
			t_vehItem.m_sum = g_StaticsInfo[i].nVehCnt[type_cur];

			if(g_StaticsInfo[i].nVehSpeed[type_cur]){
				frame[len++] = g_StaticsInfo[i].nVehSpeed[type_cur] / g_StaticsInfo[i].nVehCnt[type_cur];
				t_vehItem.m_avgSpeed = frame[len -1];
			}	 //ƽ���ص㳵��
			else{
				frame[len++] = 0;
				t_vehItem.m_avgSpeed = 0;
			}

			if(nSettings.InvContents() & 1) //01,11,21
			{
				; //��Ԥ���ֶ�
			}
			else
			{
				frame[len++]=0;		 //�ĸ�Ԥ���ֶ�
				frame[len++]=0;
				frame[len++]=0;
				frame[len++]=0;
			}

			t_dbItem.m_VehTypes.insert(j+1,t_vehItem);
		} //6*3=18

		db_info.m_LaneStatisc.append(t_dbItem);
	}

	serialize(QByteArray((char *)frame,len),_return);

	return TimeNum_temp;
}

void TrafficProtocol::serialize_11(const VehicleInfo * pVeh,QByteArray & _return,bool isNew)
{
	uint8 i,tIndex=0;
    uint16 year_temp;
	char buf[512] = {0};
   
	QDateTime cur_date = isNew ? QDateTime::currentDateTime():pVeh->datetime;
	
	quint16 t_year = cur_date.date().year();
	quint8 t_month = cur_date.date().month();
	quint8 t_day = cur_date.date().day();
	quint8 t_hour = cur_date.time().hour();
	quint8 t_min = cur_date.time().minute();
	quint8 t_sec = cur_date.time().second();
	quint16 t_msc = cur_date.time().msec();

    buf[tIndex++] = FrameFormat::SINGLE_U;

	memcpy(buf+tIndex,pVeh->devID.toAscii().data(),16);
	tIndex += 16;

    buf[tIndex++]=0;
    buf[tIndex++]=t_year&0xFF;
    buf[tIndex++]=(t_year>>8)&0xFF;
    buf[tIndex++]=t_month;
    buf[tIndex++]=t_day;
    buf[tIndex++]=t_hour;
    buf[tIndex++]=t_min;
    buf[tIndex++]=t_sec;
	buf[tIndex++]=t_msc&0xFF;
    buf[tIndex++]=(t_msc>>8)&0xFF;
    buf[tIndex++]=pVeh->dayNumber&0xFF;
    buf[tIndex++]=(pVeh->dayNumber>>8)&0xFF;
    buf[tIndex++]=(pVeh->dayNumber>>16)&0xFF;
    buf[tIndex++] = get_lane(pVeh->u8Lane);
    buf[tIndex++] = pVeh->u16Type;
    buf[tIndex++] = pVeh->speed;
    for(i=0;i<16;i++){
        buf[tIndex++]=0;
    }

	serialize(QByteArray(buf,tIndex),_return);
}

void TrafficProtocol::slot_NewFrame(const QByteArray & nData)
{
	if(m_dbg){
		qDebug()<<"recv :"<<nData.toHex().toUpper();
	}	

	QList<qint32> tValuelst;

	qint32 tIndex = 0,tOffset = 0;
	quint16 theader = 0x00;
	quint16 nLen = 0;
	quint8 nCMD = 0;
	quint64 nTimestamp = 0;
	quint16 nValue = 0;

	m_CacheData.append(nData);
		
	QDataStream out(m_CacheData);
	out.setByteOrder(QDataStream::LittleEndian);

	do{
		tOffset = 0;
		while(!out.atEnd()){
			out>>theader;
			tOffset += sizeof(theader);
			if(theader == FrameFormat::HEADER){
				break;
			}
		}

		if(out.atEnd()){
			m_CacheData.clear();
			return;
		}

		out>>nLen
		   >>nCMD;
		qint32 t_usedBytes = 0;
		switch(nCMD)
		{		
		case FrameFormat::CMD_SINGLE_RESEND://�ش�������Ϣ����
			{
				//ʱ���
				CMD_19  t_cmd;
				out.readRawData((char *)&t_cmd+1,sizeof(CMD_19) -1);
				t_cmd.RDHead.Type = nCMD;
				emit signal_cmd19(t_cmd);

				t_usedBytes = sizeof(CMD_19);

			}
			break;
		case FrameFormat::CMD_SINGLE_ACK: //�������յ���Ӧ
			{
				CMD_1A  t_cmd;
				out.readRawData((char *)&t_cmd+1,sizeof(CMD_1A)-1);
				t_cmd.RDHead.Type = nCMD;
				emit signal_cmd1A(t_cmd);

				t_usedBytes = sizeof(CMD_1A);
			}
			break;
		case FrameFormat::CMD_STATIC_RESEND://ͳ���ش�

			break;
		case FrameFormat::CMD_STATIC_D: //ͳ���ش���Ӧ

			break;
		//�����޸�ָ��
		case FrameFormat::CMD_RDNUM: //վ��ID
			{
				CMD_03 t_cmd;
				out.readRawData((char *)&t_cmd+1,sizeof(CMD_03)-1);
				t_cmd.RDHead.Type = nCMD;
				emit signal_cmd03(t_cmd);

				t_usedBytes = sizeof(CMD_03);
			}
			break;
		case FrameFormat::CMD_IP: //վ��ID
			{
				CMD_04 t_cmd;
				out.readRawData((char *)&t_cmd+1,sizeof(CMD_04) -1);
				t_cmd.RDHead.Type = nCMD;
				emit signal_cmd04(t_cmd);

				t_usedBytes = sizeof(CMD_04);
			}
		break;
		case FrameFormat::CMD_TIME: //վ��ID
			{
				CMD_05 t_cmd;
				out.readRawData((char *)&t_cmd+1,sizeof(CMD_05)-1);
				t_cmd.RDHead.Type = nCMD;
				emit signal_cmd05(t_cmd);
				t_usedBytes = sizeof(CMD_05);
			}
		break;
		case FrameFormat::CMD_CTX: //վ��ID
			{
				CMD_06 t_cmd;
				out.readRawData((char *)&t_cmd+1,sizeof(CMD_06)-1);
				t_cmd.RDHead.Type = nCMD;
				emit signal_cmd06(t_cmd);
			
				t_usedBytes = sizeof(CMD_06);
			}
		break;
		case FrameFormat::CMD_CYCLE: //վ��ID
			{
				CMD_07 t_cmd;
				out.readRawData((char *)&t_cmd+1,sizeof(CMD_07)-1);
				t_cmd.RDHead.Type = nCMD;
				emit signal_cmd07(t_cmd);

				t_usedBytes = sizeof(CMD_07);
			}
		break;
		case FrameFormat::CMD_DISTIME: //վ��ID
			{
				CMD_0B t_cmd;
				out.readRawData((char *)&t_cmd+1,sizeof(CMD_0B)-1);
				t_cmd.RDHead.Type = nCMD;
				emit signal_cmd0B(t_cmd);

				t_usedBytes = sizeof(CMD_0B);
			}
		break;
		case FrameFormat::DSC_D:
			{
				QByteArray devID;
				devID.resize(16);
				quint8 code;
				out.readRawData(devID.data(),16);
				out>>code;
				emit signal_cmd02(QString(devID),code);

				t_usedBytes = 17;
			}
			break;
		default:
			t_usedBytes = 4;
			break;
		}
		
		out.skipRawData(t_usedBytes+4);
		
	}while(1);	
}

void TrafficProtocol::decode_02(const QByteArray &)
{

}

uint8 TrafficProtocol::get_lane(uint8 olane) //����ӳ��
{
	if(TrafficSettings::instance()->LaneNum() == 2)
    {
        if(olane<=1)
        {
            return 0x01;
        }
        else
        {
            return 0x03;
        }
    }
    else
    {
        if(olane> (TrafficSettings::instance()->LaneNum()>>1))
        {
            return 30 + olane - (TrafficSettings::instance()->LaneNum()>>1);
        }
        else
        {
            return 10 + olane;
        }
    }

    return 11;
}

