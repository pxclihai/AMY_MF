
#ifndef _M6312_H
#define _M6312_H
#include "stm32f10x.h"

void M6312_init(void);
u8 M6312_send(u8 *p,u16 len);
void M6312_reconnect(void);
//u16 strsearch_h(u8 *ptr2,u8 *ptr1_at);//查字符串*ptr2在*ptr1中的位置
void M6312_open_GPRS(void);


//void M6312_RESET(void);
//void Start_M6312(void);	


extern u8 M6312_RESET_flag;

#endif

