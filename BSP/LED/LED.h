#ifndef _LED_H_
#define _LED_H_

#if CONF_LED_EN
/******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"

#define        LED						PINO(PIN_LED)
#define   	   GPIO_LED_INIT() 	    	_GPIO_LED_INIT()

void Led_Init(void);
void horse_race_lamp(void);

#ifdef __cplusplus
} // extern "C"
#endif
/******************************************************************************/
#endif
#endif

/**********************************************************end of line ********************************************************/
