#ifndef USART_H
#define USART_H

#include "protocol.h"

/*串口通信速率EEPROM存储地址,分配0x0091字节*/
#define	EEP_ADDR_COMMBPS    _EEP_ADDR_COMMBPS

/*********************************************************************************************/
 /*串口选择*/
typedef enum
{
	USART_TYPR_COM1,		  	//串口1		 
	USART_TYPR_COM2,	   		//串口2
	USART_TYPR_COM3,		   	//串口3
	USART_TYPR_COM4,		   	//串口4
	USART_TYPR_COM5,		   	//串口5
   	USART_TYPR_MAX
}USART_TYPE;

/*串口模式*/
typedef enum
{
	USART_MODE_TYPE_IRQ,
	USART_MODE_TYPE_DMA
}USART_MODE_TYPE;
/*********************************************************************************************/
typedef int (*UartFun)(void);			//串口发送和接收函数
typedef void (*USARTx_Init)(void);		//串口初始化函数类型
typedef void (*USARTx_IRQHandler)(void);//串口触发函数
/*********************************************************************************************/
/*串口参数控制结构体*/
typedef __packed struct {
	uint32_t baudRate;	 				//波特率
	uint16_t parity;					//校验位
	uint16_t wordLength;				//数据长度
	uint16_t stopBits;					//停止位
	uint16_t hardwareFlowControl;	   	//硬件流控制
	uint16_t mode;						//模式
}tagUartCommAag;

/*串口参数控制结构体*/
typedef __packed struct {
	USARTx_Init init;			 		//初始化函数
	USARTx_IRQHandler irqHandler;	   	//触发函数
	USART_TypeDef* USARTx;				//对应的串口地址
	tagUartCommAag UartCommAag;			//串口参数
}UartCom;

/*********************************************************************************************/
/*串口与功能映射*/
typedef __packed struct {
	USART_TYPE usartType;				//串口类型，Com1- Com5
	USART_MODE_TYPE mode;				//串口模式 IRQ DMA
	USART_FUNCTION 	usartFun;			//功能函数，WCDMA等
}UsartTypeToFunctionStruct;

/*********************************************************************************************/
/*串口映射功能结构体*/
typedef __packed struct 
{
   	UsartTypeToFunctionStruct* uTypeToFun;		//串口功能的映射	
	UartCom *uCom;								//只跟某个串口设置相关的参数
	BOOL ready;									//是否可用 
}UartStruct;
/*********************************************************************************************/
void Uart_Init(void);
int DMA_Usart_Init(USART_FUNCTION fun);
UartStruct* GetUartStructBaseFunction(USART_FUNCTION fun);	//根据协议类型获取相关的串口设置信息
U8 ReceiveDataFromUartStruct(UartStruct* us);				//从串口接收一个数据
int SendDataFromUartStruct(UartStruct* us, u8 data);  		//向串口发送一个数据
int EnableCom(UartCom* com,BOOL enable);  					//使能或者失效一个串口
int SetupComDir(UartStruct* us,BOOL State);   				//控制方向
void SettingSaveByUSART_FUNCTION(USART_FUNCTION uf);		//设置串口参数
/*********************************************************************************************/
#endif


