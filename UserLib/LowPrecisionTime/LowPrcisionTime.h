#ifndef _LOWPRCIISIONTIME_H
#define _LOWPRCIISIONTIME_H
#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"


/*******************************************************************************/
/*定时器链表节点结构*/
typedef __packed struct Node	{
  struct Node * next;  			/*下一个节点*/
  int time;		  				/*持续的时间*/
  int curTime;					/*持续时间计数器*/
  int circleCount;				/*循环次数*/
  TIMER_TYPE timeType;			/*时间触发类型*/
  void (*TimerEvent)(void);		/*回调函数*/
  void (*CircleEvent)(void);    /*循环结束回调函数*/	
}TimerNode;
/*******************************************************************************/
/*函数声明*/
void LowPrcisionTime_Init(void);
void LowPrcisionTime_Start(void);
TimerNode* CheckNode(TIMER_TYPE timerType);
int SetTimer(int time,int circleCount,TIMER_TYPE timerType,TimerEvent TEvent,TimerEvent CircleEvent);
int DelTimer(TIMER_TYPE timerType);
int SetTimerFast(int time,int circleCount,TIMER_TYPE timerType,TimerEvent TEvent,TimerEvent CircleEvent);
int DelTimerFast(TIMER_TYPE timerType);

void DoTimer(void);
/*******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
//                          程序结束(END)
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

