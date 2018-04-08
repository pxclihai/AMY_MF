#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************************************/
/*系统头文件**/
#include "stm32f10x.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*********************************************************************************************/

	#define DEBUG(format, ...) 


/*********************************************************************************************/
/*--底层驱动模块--*/
/*MCU模块*/
#include "./MCU/MCU.h"

/*功能模块开关头文件*/
#include "moduleSelect.h"

/*设备系统全局量头文件*/
#include "./data/data.h"

/*API函数头文件*/
#include "./api.h"

/*GPIO初始化定义*/
#include "./GPIO/GpioMap.h"	

/*系统定时器模块驱动头文件*/
#include "./LowPrecisionTime/LowPrcisionTime.h"		

/*WatchDog模块驱动头文件*/
#include "./WatchDog/WatchDog.h"

/*IAP模块驱动头文件*/
#include "./IAP/IAP.h"

/*公共函数头文件*/
#include "./basicFunc/basicFunc.h"

/*串口驱动头文件*/
#include "./USART/USART.h"

/*LED模块驱动头文件*/
#include "./LED/LED.h"

/*LED模块驱动头文件*/
#include "./CESHI/CESHI.h"

/*flash模块驱动头文件*/
#include "./SpiFlash/Spi_Flash.h"

/*协议解析驱动头文件*/
#include "./Protocol.h"

/*邮电部协议解析驱动头文件*/
#include "./PostProDa/PostProDa.h"

/*邮电部协议数据上报口协议头文件*/
#include "./PostProDa/Uploading_Pro.h"

/*IAP程序升级协议头文件*/
#include "./DownLoad/DownLoad.h"

/*升级协议函数头文件*/
#include "./DownLoad/DownLoad_Pro.h"

/*协议接收回调函数处理*/
#include "./ProtocolRecCallBack.h"

/*IAP升级驱动头文件*/
#include "./iap_updata/iap_updata.h"

/*网络模块驱动头文件*/
#include "./qwl_net.h"

/*NB模块驱动头文件*/
#include "./nblot/nblot.h"

/*CDMA模块驱动头文件*/
#include "./cdma/cdma.h"

/*G模块驱动头文件*/
#include "./gsm/M6312.h"

/*G模块驱动头文件*/
#include "./g510/G510.h"

/*协议主动发送上位机文件*/
#include "./Protocol_send.h"

/*按摩椅驱动头文件*/
#include "./armchair.h"

/*TM1618驱动头文件*/
#include "./TM1618/TM1618.h"

/*SX1278驱动头文件*/
#include "./433_2x1278/2x1278.h"

/*amy_net驱动头文件*/
#include "./amy_net/amy_net.h"

/*flash驱动头文件*/
#include "./flash/flash.h"

/*邮电部协议驱动头文件*/
#include "./post_protocol/post_protocol.h"

/*algorithm驱动头文件*/
#include "./algorithm/algorithm.h"

#include "Plug.h"
#include "Protool.h"
/*********************************************************************************************/
#ifdef __cplusplus
} // extern "C"
#endif

#endif

