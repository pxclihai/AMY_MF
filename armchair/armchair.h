#ifndef _armchair_H
#define _armchair_H

#include "config.h"
//#define ac_status_in GPIO_Pin_9	 	//管脚宏定义PB9 控制输出开关
//#define ac_switch GPIO_Pin_0	 	//管脚宏定义PE0 状态输出
#define ac_status_out GPIO_Pin_1	 	//管脚宏定义PE1 状态输入脚
#define ac_on 0;
#define ac_off 1;

typedef enum {
		ruilaikesi,			//瑞莱克斯设备
	oxygen_plant,			//制氧机设备
	slot_machines,		//投币机设备
	Module_TYPE_MAX 		
}Module_TYPE;

typedef __packed struct
{

			Module_TYPE Module;
}Shared_TYPE;

extern Shared_TYPE Shared_Module;//设备选择
//全局函数
void Armchair_IO_Init(void);
void Armchair_Init(void);
void ARMCHAIR_TIME(void);
void ARMCHAIR_DEAL(void);
void ARMCHAIR_S_IQR(void);
void Shared_Module_switch(u8 module_state);
void slot_machines_data_send(void);
#define        armchair_status_IN					PINI(PIN_armchair_status_IN)
#define        armchair_switch						PINO(PIN_armchair_switch)
#define        armchair_status_OUT				PINO(PIN_armchair_status_OUT)
#define   	   GPIO_armchair_INIT() 	    _GPIO_armchair_INIT()
u8 armchair_io_state(void);
void Amy_Record_Store(u8 k);
u8 armchair_change_outIO(void);
u8 armchair_io_state_1(void);
//全局变量
extern u8 amy_config;
extern u8 amy_config1;
extern u8 PLC_time_flag;//终端定时发送定时
extern u8 time_share_process_flag;//按摩椅分时处理标识
extern u16 zd_overtime_flag;//终端超时标识，超时没有收到反馈，进行标志置位
extern u8 AmyID[16];
extern u8 ceshi;
extern u8 PLC_time_tiqian;

extern u8 amy_record_buf[];		//按摩椅数据缓存区
extern u8 amy_record_buf_1[];		//按摩椅数据缓存区的零时存放地址
extern u8 amy_rec_flag;				//按摩椅接受数据标志位
extern u8 amy_rec_flag_1;//按摩椅接受标志位正数计数	
extern u8 amy_total;//按摩椅总数
extern u8 armchair_time_flag;//按摩椅状态读取防止超时标志
extern u8 amy_CHKSUM;//按摩椅校验码，所以终端ID相加和
extern u8 getwaylist_send_times;
extern u16 amy_cotrol_time;//按摩椅控制时间
extern u16 amy_cotrol_time_flag;//定时器中计时
extern u8 amy_cotrol_flag;//按摩椅控制标志


extern u8 wg_ack_buf[200];//网关收到数据后，进行反馈缓存，放入主程序内处理
extern u8 wg_stor_pointer;//网关存储指针
extern u8 wg_deal_pointer;//网关处理指针，当前处理到的位置
extern u8 wg_ack_times;//需要反馈的条数

extern u8 wg_state_flag;//查询网关智联按摩椅状态标志
extern u8 wg_set_flag;//网关直接启动按摩椅标志

extern u8 zd_update_flag;//终端升级标志

extern u8 SIM_GET_FLAG;//获取SIM卡编号
extern u8 shebei_flag;//设备状态报告
#endif 
