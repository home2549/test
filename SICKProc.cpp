#include <QByteArray>
#include "SICKProc.h"

 
const char FRAME_HEAD_ASC = 2;
const char FRAME_TAIL_ASC = 3;
CSICKProc::CSICKProc(void)
{
}


CSICKProc::~CSICKProc(void)
{
}

void CSICKProc::SingleFrame(char *outData,int &outlen)
{
	const char * Cmd_strOneFrame = "sRN LMDscandata";			// PC->LMS 请求-获取单帧数据

	int nSize = qstrlen(Cmd_strOneFrame);
	int pos = 0;
	outData[pos++] = FRAME_HEAD_ASC;
	outData[pos++] = FRAME_HEAD_ASC;
	outData[pos++] = FRAME_HEAD_ASC;
	outData[pos++] = FRAME_HEAD_ASC;
	outData[pos++] = (nSize>>24) & 0xFF;
	outData[pos++] = (nSize>>16) & 0xFF;
	outData[pos++] = (nSize>>8) & 0xFF;
	outData[pos++] = (nSize) & 0xFF;	
	//strcpy_s(&outData[pos],nSize,Cmd_strOneFrame);
	strcpy_s(&outData[pos],nSize+1,Cmd_strOneFrame);
	pos+=nSize;
	//nSize += 9;
	char crcKey =  crc(outData + 8,pos - 8);
	outData[pos++] = crcKey;

	outlen = pos;
}

void CSICKProc::ContinueFrame(char *outData,int &outlen,bool stopFlag)
{	
	const char * Cmd_strContinueFrame = "sEN LMDscandata";			// PC->LMS 请求-获取连续数据
	int nSize = strlen(Cmd_strContinueFrame)+2;
	int pos = 0;
	outData[pos++] = FRAME_HEAD_ASC;
	outData[pos++] = FRAME_HEAD_ASC;
	outData[pos++] = FRAME_HEAD_ASC;
	outData[pos++] = FRAME_HEAD_ASC;
	outData[pos++] = (nSize>>24) & 0xFF;
	outData[pos++] = (nSize>>16) & 0xFF;
	outData[pos++] = (nSize>>8) & 0xFF;
	outData[pos++] = (nSize) & 0xFF;	
	strcpy_s(&outData[pos],nSize+1,Cmd_strContinueFrame);
	pos+=nSize - 2;
	outData[pos++] = 0x20;
	outData[pos++] = stopFlag ? 0x00:0x01;
	//nSize += 9;
	char crcKey =  crc(outData + 8,pos - 8);
	outData[pos++] = crcKey;
	outlen = pos;
}

char CSICKProc::crc(char *outData,int len)
{
	char crcV = outData[0];
	for(int i = 1;i<len;i++){
		crcV ^= outData[i];
	}
	return crcV;
}