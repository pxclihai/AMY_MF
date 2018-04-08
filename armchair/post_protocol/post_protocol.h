#ifndef _POST_H
#define _POST_H
#include "stm32f10x.h"
//全局函数
void GATEWAY_heart_send_pack(void);
void GATEWAY_list_send_pack(void);
void GATEWAY_peizhi_pack(void);
void post_group(u8 *post_buf2,u16 post_buf1_len,u8 command);
void poll_list_send_pack(void);
void SIM_ID_send_pack(void);
void amy_off_send_pack(void);
//全局变量
extern u8 post_buf[];   // 组包暂存区
extern u16 post_len;				// 组包长度
extern u8 NO[16];
extern unsigned char CRC8_Table(unsigned char *p, char counter);  

#define POST_VER 0x22	 	//通信协议
#define POST_ADR 0x00	 	//设备地址
#define POST_CID1 0xd0	 	//控制标识码，按摩椅固定0XD0
#define AMY_HEART 0x1d	 	//按摩椅心跳
#define AMY_LIST 0x1c	 	//按摩椅心跳
#define AMY_STATE 0x1b	 	//按摩椅状态
#define AMY_SET 0x1e	 	//按摩椅设置
#define AMY_PEIZHI 0xfb	 	//按摩椅配置
#define AMY_addr_please 0x24	 	//按摩椅直接地址查询，地址设置时间，网关直接启动，网关直接查询按摩椅状态
#define AMY_list_state 0x25 //列表状态上传,和上面列表命令不一样，此命令只发生每台终端状态，不上传终端ID

#endif 
