#ifndef DBG_MSG_TYPE_DEFINE__H
#define DBG_MSG_TYPE_DEFINE__H

enum{
	ERROR_TYPE = 0x01,  //未识别
	V_BKG_TYPE, // 垂直背景数据
	Q_BKG_TYPE, // 倾斜背景数据
	VEH_TYPE,
	V_RTLS_TYPE,
	Q_RTLS_TYPE
};

#endif // DBG_MSG_TYPE_DEFINE__H