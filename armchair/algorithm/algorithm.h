#ifndef _ALGORITHM_H
#define _ALGORITHM_H
#include "stm32f10x.h"

u8 strsearch(u8 *ptr2,u8 *ptr1_at,u8 Rxx);
//u16 strsearch_h(u8 *ptr2,u8 *ptr1_at,u16 Rx);
u16 strsearch_qwl(u8 *ptr2,u8 *ptr1_at,u16 Rx,u16 ptr_len);
u8 strsearch_1278(u8 *ptr2,u8 *ptr1_at,u8 Rx);//查字符串*ptr2在*ptr1中的位置


#endif
