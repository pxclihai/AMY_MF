#include "LowPrcisionTime.h"

/*定时1MS计数，每1MS加1*/
__IO uint16_t Timer50msCount = 0;	
/*定时器链表头*/
TimerNode* _gTimerLinkerHeader = NULL;

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : LowPrcisionTime_Init
// 功能描述  : 初始化软定时器
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void LowPrcisionTime_Init(void)	
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);			//使能 TIM5 定时器
	TIM_DeInit(TIM5);
	
	TIM_TimeBaseStructure.TIM_Period		=	1000;		 		//在延迟使用的时候在进行设置 ,1ms时间
	TIM_TimeBaseStructure.TIM_Prescaler		=	71;		  			//36Mhz/(35+1)=1Mhz 即位1us ?? 仍然需要71分屏
	TIM_TimeBaseStructure.TIM_ClockDivision =	TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 	  	//向上计数模式
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);	
	TIM_ARRPreloadConfig(TIM5, DISABLE);						 	//直接设置ARPE = 0	  ,不经过缓存
	TIM_Cmd(TIM5, DISABLE);										 	//关闭定时器
	
	
	// 使能TIM5定时中断
	NVIC_InitStructure.NVIC_IRQChannel =  TIM5_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : TIM5_IRQHandler
// 功能描述  : 定时器1更新中断服务程序
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void TIM5_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) == SET)
	{		
		 TIM_ClearFlag(TIM5, TIM_FLAG_Update);
		 Timer50msCount++;
	}
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : LowPrcisionTime_Start
// 功能描述  : 启动软定时器
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void LowPrcisionTime_Start(void)
{
    /*开启定时器*/
	TIM_Cmd(TIM5, ENABLE);
}	

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : CheckNode
// 功能描述  : 检查定时器是否存在
// 输入参数  : timerType,唯一定时器类型
// 输出参数  : None
// 返回参数  : 存在返回pNode 不存在返回NULL
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
TimerNode* CheckNode(TIMER_TYPE timerType)
{
	TimerNode* 	pNode =   _gTimerLinkerHeader;
	while(pNode != NULL)
	{
		if(pNode->timeType == timerType)
			return pNode;
		pNode = pNode->next;
	}

	return NULL;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : SetTimer
// 功能描述  : 建立一个新的定时器，如果已经建立过则更新定时器参数
// 输入参数  : time 定时器时间; 
//             circleCount 循环次数, =-1时重复循环;
//             timerType 定时器类型;	
//             TimerEvent 单次循环执行回调函数; 
//             CircleEvent 循环结束执行回调函数;
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int SetTimer(int time,int circleCount,TIMER_TYPE timerType,TimerEvent TEvent,TimerEvent CircleEvent)
{
	TimerNode* pNode = NULL;
	if((pNode =CheckNode(timerType)) == NULL)
	{
		pNode = (TimerNode*)malloc(sizeof(TimerNode));
		memset(pNode,NULL,sizeof(TimerNode));
		if(_gTimerLinkerHeader == NULL) 
		{
			_gTimerLinkerHeader =  pNode;		
		} 
		else 
		{
			TimerNode* pN =   _gTimerLinkerHeader;
			while(pN->next != NULL) 
			{
				pN = pN->next;
			}
			pN->next = 	pNode;
			
		}
	}
	 	
	pNode->time = time;
	pNode->curTime = time;
	pNode->circleCount =  circleCount;
	pNode->timeType =  timerType;
	pNode->TimerEvent = TEvent;	
	pNode->CircleEvent = CircleEvent;	
	
	return 0;	
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : DelTimer
// 功能描述  : 删除一个定时器
// 输入参数  : timerType 唯一定时器类型
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int DelTimer(TIMER_TYPE timerType)
{
	TimerNode* 	pNode =   _gTimerLinkerHeader;
	if(pNode == NULL)
		return -1;
	if(pNode->timeType == timerType)	 //进行头部判断
	{
		_gTimerLinkerHeader =  pNode->next;		
		free(pNode);
		return 0;
	}
	while(pNode->next != NULL)
	{
		if(pNode->next->timeType == timerType)
		{
			TimerNode* 	pN =  pNode->next;
		   	pNode->next = pN->next;
			if(pN->CircleEvent != NULL)
			{
				pN->CircleEvent();
			}
			free(pN);
			return 0;	
		}
			
		pNode = pNode->next;
	}
	return -1;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : DoTimerCallBack
// 功能描述  : 单次执行回调函数
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
static void DoTimerCallBack(void (*TimerEvent)(void))
{
	 if(TimerEvent != NULL)
	 {
	 	  TimerEvent();
	 }
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : DoTimer
// 功能描述  : 定时器处理
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void DoTimer(void)
{
	TimerNode* 	pNode;

	while(Timer50msCount)
	{	
		Timer50msCount--;

		pNode =  _gTimerLinkerHeader;
		while(pNode != NULL)
		{
			TimerNode* pN =  pNode->next;

			if(pNode->curTime > 0)
			{
				  pNode->curTime--;
			} 
			else 
			{
				pNode->curTime = pNode->time;
				if(pNode->circleCount > 0)	        /*有限次数运行*/
				{ 		 
					pNode->circleCount--;
					DoTimerCallBack(pNode->TimerEvent);
				} 
				else if(pNode->circleCount == -1) /*永远运行下去*/
				{
					 DoTimerCallBack(pNode->TimerEvent);
				} 
				else			/*结束运行*/
				{
				  	 DelTimer(pNode->timeType);
				}
			}

			pNode =  pN;
		}

	}

}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
//                          程序结束(END)
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
