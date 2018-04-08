#ifndef _flash_H
#define _flash_H
#include "stm32f10x.h"

	#define     HARDWARE_VER		  		1.00
	#define     SOFTWARE_VER		  		3.77
		#define		COMPANY_NAME				"ModouTech           "	 		/*公司名称*/
		#define		FACTNAME					"armchair gateway    "	 	                /*设备型号名称*/	


void Write_Flash(u8 *flash_data,u32 flash_adr,u16 flash_len);

void Write_Flash_ALLAMY(void);


void Write_light_idip(void);

void Init_Amy_Id(void);


extern u8 software_version[2];//软件版本
extern u8 Hardware_version[2];//硬件版本
extern u8 Upgrade_ip[22];//升级IP
extern u8 carrieroperator;//运营商
extern u8	module_name;//模块型号
extern u8 amy_max_total;//按摩椅总数可以设置
//extern u8 SIM_ID[20];//SIM卡ID
extern u8 zd_sl_flag;//终端数量刷入标志
//定义全局变量
//extern u32 flash_data;

#define flash_AMY_adr1 0x0803D000	//定义主存储器的保存数据的起始地址,0x0803D000~0x0803E800，一共6K，存储按摩椅ZHON数据

#define  AMYIDIP_adr 0x0803e800//按摩椅存储IDIP地址



#endif
