/**********************************************
* File Name          : net.h
* Author             : 钱文力
* Description        : 以太网应用接口函数（MSG）
********************************************************************************/
#ifndef __AMY_NET_H__
#define __AMY_NET_H__
#include "stm32f10x.h"

typedef __packed struct
{
			u16 poll_max_time;//轮询最大时间
			u16 poll_time; //轮询时间
			u8 zd_list[13];			//终端列表
			u8 zd_state[25];	//终端状态
			u8 list_please_flag;//列表请求标志
			u8 state_send_flag;//状态发送标志
			u8 poll_deal;//轮询处理步骤
}POLL_ZD_TYPE;
extern POLL_ZD_TYPE poll_zd;

//按摩椅程序
void amyack_send_pack(u8 state);//状态发送组包
void amystate_send(u8 state);
void eth_rec_deal(u8 ch);
void GATEWAY_send(void);//对网关组网发送打包处理
void amy_state_send(void);//按摩椅状态发送
void amystate_send_pack(u8 state,u8 addr);//按摩椅状态打包
void USART3_sendstring(u8 *p);
void USART3_sendchar(u8 ch);
void usart3_init(void);
void amystate_pack_send(u8 state,u8 addr);
void usart5_init(void);
void wg_amy_state(void);//网关直连按摩椅查询状态



//按摩椅全局函数
extern u16 GATEWAY_heart_time_flag;   // 心跳发送延时计时，专用网关计时
extern u16 GATEWAY_heart_max_flag;   // 心跳发送延迟时间，默认60秒
extern u16 GATEWAY_list_time_flag;
extern u16  eth_Len;
extern u8	getwaylist_send_flag;//列表发送标志，不等于0时，要求列表上传


extern u16 GATEWAY_off_time_flag;
extern u8	getwayoff_send_flag;//列表发送标志，不等于0时，要求列表上传

extern u8 amy_check_ID[16];//按摩椅要查询的终端先缓存
extern u8 amy_config_rec_flag;//按摩椅配置标志位	

//extern u8 deal_process;//处理进程标志，邮电部要求一条一条发，所以用次标志

extern u8 plc_cofig_buf[16];//按摩椅配置暂时存储用于发送
extern u8 plc_cofig_buf_1[3];//按摩椅配置时间和模式


extern u16 eth_Len;

extern u8 amy_storflash_flag;//标志，0读flash，1循环第一次然后更新flash，2，等待新终端，然后更新flash

extern u8 amystate_send_flag[];//按摩椅状态发送标志

extern u8 CSQ;
extern u8 CSQ_DELAY;

#endif
