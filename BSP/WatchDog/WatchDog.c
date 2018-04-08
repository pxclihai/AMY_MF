#include "config.h"

#if CONF_WATCHDOG_EN

__IO uint16_t gKickDogNum = 0;

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : WD_Init
// 功能描述  : 看门狗初始化,包括窗口看门狗与独立看门狗一起动作
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void WD_Init(void)             
{
	//独立看门狗初始化
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);		//启动寄存器读写
	IWDG_SetPrescaler(IWDG_Prescaler_32);				//40K时钟32分频
	/*(分频系数/4)*0.1*装载值 = 喂狗时间MS  ,  0x4E2在32分频为1S；  625为500ms, 此参数最大值为0xFFF;*/
	IWDG_SetReload(0xfff);                 				//计数器数值
	IWDG_ReloadCounter();             					//重启计数器
	IWDG_Enable();                       				//启动看门狗
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : FeedWatchDog
// 功能描述  : 重启计数器（喂狗）
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void FeedWatchDog(void)
{
    gKickDogNum  = 0;
	IWDG_ReloadCounter();//重启计数器（喂狗）
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : WD_ClockingPro
// 功能描述  : 由于独立看门狗时间非常短，通过定时处理，将处理时间增长
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void WD_ClockingPro(void)
{
    static uint8_t  sTickNum = 0;
    
    if((gKickDogNum++ <= SOFTDOG_TIMEOUT)&&(sTickNum++ == 10))
	{
		IWDG_ReloadCounter();//重启计数器（喂狗）
   		sTickNum = 0;        //变量清零
	}   
}
#endif
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
//                          程序结束(END)
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
