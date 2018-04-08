#ifndef PROTOCOL_SEND_H
#define  PROTOCOL_SEND_H
#include "config.h"
#include "stm32f10x.h"

typedef enum {

	SEND_READY,
	WAIT_ACK,
	STOP_SEND
}SEND_STATE;
typedef enum {

	WAIT_REC,
	REC_OK
}REC_STATE;
typedef __packed struct
{
	u8 TIMES;	
	u8 delays;
	u8 MAX_delays;
	SEND_STATE send_state;
	REC_STATE REC_state;
}SEND_TYPE;

typedef __packed struct
{
			
			SEND_TYPE upload_handshake;//升级握手
}PROTOCOL_SEND_TYPE;

extern PROTOCOL_SEND_TYPE Net_Data_Send;
void net_data_send(void);
void net_data_send_init(void);//网络参数初始化
void net_data_send_delay(void);
void process_F8(u8 RTN);
#endif 

