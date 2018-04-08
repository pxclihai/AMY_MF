#include "config.h"

//用于能在undstand中测试
#define BUILD_PROJECT BOOT_HD

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : TimingDelay_Decrement
// 功能描述  : 使用SysTick来精确延时,TimingDelay：周期为1ms的相减基准变量
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
static __IO uint8_t KeySacnTimeNum = 0;

void TimingDelay_Decrement(void)
{
    if(KeySacnTimeNum++ >= 5)		//5ms进行扫描
	{
		KeySacnTimeNum = 0;
	}
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : InitChip
// 功能描述  : STM32芯片的初始化
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void InitChip(void)			
{

	/*初始化芯片*/
    SystemInit();


    #ifdef STM32_BOOT
    	/* Set the Vector Table base location at 0x08000000*/  
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8000);   
	
    #endif 


}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : InitHardWare
// 功能描述  : 包括串口、板载资源的硬件初始化
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void InitHardWare(void)			
{   
	/*STM32芯片的初始化*/
    InitChip();
	
		/*led初始化*/  
    API_Led_Init();
	
	/*按摩椅IO初始化*/  
	API_Armchair_IO_Init();

	/*flash初始化*/  
	API_SPI_Flash_Init();

	/*协议初始化*/
	API_Protocols_Init();


	/*定时器模块初始化*/
	API_Timers_Init();
	
	

}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : InitSoftSystem
// 功能描述  : 使用的功能模块进行配置
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void InitSoftSystem(void)		
{

	/*选择进入用户或引导程序*/
	API_SetTimer(2000,1,TIMER_TYPE_IAP_SELECT_PROGRAM,API_Select_Program_run,NULL);

	/*网络初始化*/
	API_qwl_net_init();
	
	/*网络数据发送初始化*/
	API_net_data_send_init();
	
	/*启动定时器开始运行*/
    API_StartRunTimer();
	
	/*按摩椅初始化*/  
	API_Armchair_Init();
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : main
// 功能描述  : 系统主函数
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int main(void)  
{ 
	/*包括串口、板载资源的硬件初始化*/		    
    InitHardWare();		    
	/*使用的功能模块进行配置*/
    InitSoftSystem();  
	DEBUG("time:%s\n",__TIME__); 
  mofanSDK();
	while (1)
	{	
		
	/*	u8 buf[1024]={0};
		//u8 buf1[1024]={0};
		u16 i=0;
		SST25_clr_256_BLOCK();
		//checktest();
		for(i=0;i<1024;i++)
		{buf[i]=0x44;
		}
		//Delay_MS(1000);
		//SST25_W_BLOCK(0x0,buf,1024);
		Delay_MS(1000);
		for(i=0;i<1024;i++)
		{buf[i]=0x55;
		}
		//SST25_W_BLOCK(2048,buf,1024);
		iap_data.copy_long = 0;
		Save_Iap_Config();
		SST25_R_BLOCK(0x00,buf,1024);
		for(i=0;i<1024;i++)
		qwl_sendchar(buf[i]);
		SST25_R_BLOCK(1024,buf,1024);
		for(i=0;i<1024;i++)
		qwl_sendchar(buf[i]);
		SST25_R_BLOCK(2048,buf,1024);
		for(i=0;i<1024;i++)
		qwl_sendchar(buf[i]);
		SST25_R_BLOCK(4096,buf,1024);
		for(i=0;i<1024;i++)
		qwl_sendchar(buf[i]);

		while(1);*/
		
		/*检查进入用户或引导程序标志*/
		API_Check_Run_User_Program_Falg();
		API_WatchDog_Feed();

		/*定时器模块处理*/
		DoTimer();
		API_WatchDog_Feed();
		
		/*用于分析接收到的数据*/
		API_DoProtocols();
		API_WatchDog_Feed();
		
		/*自动建立网络*/
		API_net_state();
		API_WatchDog_Feed();
		
		//#if CONF_ARMCHAIR_EN
		//if((amy_config&0x02)==0x02)//假如按摩椅是网关
		//#endif
		{
		/*用于发送数据*/
		API_net_data_send();
		API_WatchDog_Feed();
		}
		
		
		
		/*用于按摩椅处理*/
		API_ARMCHAIR_DEAL();
		API_WatchDog_Feed();

	}
}

/***********************************end of line ********************************/

