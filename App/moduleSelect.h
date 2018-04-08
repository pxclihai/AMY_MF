#ifndef MODULE_SELECT_H
#define MODULE_SELECT_H
#define 	BOOT_CL 		0
#define  	BOOT_HD 		1
//#if BUILD_PROJECT == BOOT_HD
#if 1
	/*--系统常用预定义变量--*/
	#define 	CONF_WATCHDOG_EN			  1			/*使能与否看门狗*/
	#define		CONF_UART1_EN						1			/*是否使用串口1功能*/
	#define		CONF_UART2_EN						1			/*是否使用串口2功能*/
	#define		CONF_UART3_EN						1			/*是否使用串口3功能*/
	#define		CONF_UART4_EN						0			/*是否使用串口4功能*/
	#define		CONF_UART5_EN						1			/*是否使用串口5功能*/
	#define		CONF_LED_EN							1			/*使能协议*/
	#define		CONF_PROTOCOL_EN				1			/*使能协议*/
	#define		CONF_TIMER_EN  					1			/*使能软定时器*/
	#define		CONF_IAP_EN						  1			/*程序更新使能*/
	#define		CONF_CESHI_EN						1			/*测试代码宏定义*/
	#define		CONF_QWL_NET_EN					1			/*测试代码宏定义*/
	#define		CONF_SPI_FLASH_EN				1			/*=1外部flash升级,=0内部flash升级*/
	#define		CONF_ARMCHAIR_EN				1			/*按摩椅使能*/	
/*********************************************************************************************/
#else
	/*--系统常用预定义变量--*/
	#define 	CONF_WATCHDOG_EN			        1			/*使能与否看门狗*/
	#define		CONF_UART1_EN						1			/*是否使用串口1功能*/
	#define		CONF_UART2_EN						0			/*是否使用串口2功能*/
	#define		CONF_UART3_EN						0			/*是否使用串口3功能*/
	#define		CONF_UART4_EN						0			/*是否使用串口4功能*/
	#define		CONF_UART5_EN						0			/*是否使用串口5功能*/
	#define		CONF_PROTOCOL_EN					1			/*使能协议*/
	#define		CONF_TIMER_EN  						1			/*使能软定时器*/
	#define		CONF_IAP_EN						    1			/*程序更新使能*/
#endif


#endif
