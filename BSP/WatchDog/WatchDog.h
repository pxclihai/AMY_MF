#ifndef _WATCH_DOG_H_
#define _WATCH_DOG_H_
#if CONF_WATCHDOG_EN
#ifdef __cplusplus
extern "C" {
#endif

#include"config.h"

#define SOFTDOG_TIMEOUT			    5000  			//软件看门狗超时时间

extern __IO uint16_t gKickDogNum;
extern void WD_Init(void);           
extern void WD_ClockingPro(void);
extern void FeedWatchDog(void);

#ifdef __cplusplus
} // extern "C"
#endif
#endif
#endif

/**********************************************************end of line ********************************************************/

