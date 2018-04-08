#ifndef _CESHI_H_
#define _CESHI_H_

#if CONF_CESHI_EN
/******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"

//extern u8 NO[16];
void qwl_sendchar(u8 ch);
void qwl_sendstring(u8 *p);
void qwl_sendstring1(u8 *p);
#ifdef __cplusplus
} // extern "C"
#endif
/******************************************************************************/
#endif
#endif

/**********************************************************end of line ********************************************************/
