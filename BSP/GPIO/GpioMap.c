#include "config.h"



//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Gpio_Init
// 功能描述  : GPIO初始化
// 输入参数  : RCC_APB2Periph,时钟 GPIO_Port,端口 GPIO_Pin,引脚 GPIO_Mode,模式 GPIO_Speed,频率.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void Gpio_Init(uint32_t RCC_APB2Periph,
			   GPIO_TypeDef * GPIO_Port,
			   uint16_t GPIO_Pin,
			   GPIOMode_TypeDef GPIO_Mode,
			   GPIOSpeed_TypeDef GPIO_Speed
			    )
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph, ENABLE);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode;   
	GPIO_Init(GPIO_Port, &GPIO_InitStructure); 

}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
//                          程序结束(END)
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

