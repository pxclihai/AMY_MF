#ifndef _GLOBE_H_
#define _GLOBE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include"config.h"

extern 	Date_Time			Sys_Date;           //系统时间
extern  __IO uint32_t       gTimingDelay;       //公用的时间延迟变量
extern uchar SysInitFlag;
extern uchar LcdType;
#ifdef 	__cplusplus
} // extern "C"
#endif

#endif
