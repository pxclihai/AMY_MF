
#ifndef _nblot_H
#define _nblot_H
#include "stm32f10x.h"

void nblot_init(void);
u8 nblot_send(u8 *p,u16 len);
u8 nblot_send1(u8 *p,u16 len);
void nblot_reconnect(void);
//u16 strsearch_h(u8 *ptr2,u8 *ptr1_at);//查字符串*ptr2在*ptr1中的位置
void nblot_open_GPRS(void);
//void CDMA_IO_Init(void);
//void nblot_init_0(void);

void nblot_rec(u8 k);

void nblot_read(void);
//u8 nblot_group(void);
void nbiot_send_NSORF(void);
void nblot_init_0(void);


extern u8 nblot_rec_buf[];
//extern u16 nblot_rec_times;
extern u8 nblot_RESET_flag;
extern u8 nblot_read_flag1;
//extern u8 nblot_read_flag1_delay;
extern u8 NBIOT_SET_FALG;
extern u8 light_UDP[22];
//#define CDMA_RESET GPIO_Pin_14	 	//管脚宏定义PE13
//#define CDMA_ON_OFF GPIO_Pin_13	 	//管脚宏定义PE13
//#define CDMA_IO GPIOE	 	//管脚宏定义PE13

#endif

