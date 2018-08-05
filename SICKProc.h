#pragma once
/*
*SICK协议
*/
class CSICKProc
{
public:
	CSICKProc(void);
	~CSICKProc(void);
	static void SingleFrame(char *outData,int &len);
	static void ContinueFrame(char *outData,int &outlen,bool stopFlag);
	static char crc(char *outData,int len);
	
};

