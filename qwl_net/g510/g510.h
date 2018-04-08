
#ifndef _G510_H
#define _G510_H
#include "stm32f10x.h"

void G510_init(void);
u8 G510_send(u8 *p,u16 len);
void G510_reconnect(void);
//u16 strsearch_h(u8 *ptr2,u8 *ptr1_at);//查字符串*ptr2在*ptr1中的位置
void G510_open_GPRS(void);


//void G510_RESET(void);
//void Start_G510(void);	


extern u8 G510_RESET_flag;

#endif

