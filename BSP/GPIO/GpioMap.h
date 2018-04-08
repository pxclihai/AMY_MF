#ifndef GPIO_MAP_H
#define GPIO_MAP_H

/******************************************************************************/
/*GPIO口初始化处理*/
/******************************************************************************/
#define _MG(PORT,INDEX)	       P##PORT##INDEX
#define _MGIN(PORT,INDEX)	   P##PORT##INDEX##in

#define  MG(PORT,INDEX,XXX)	   _MG(PORT,INDEX)
#define  MGIN(PORT,INDEX,XXX)  _MGIN(PORT,INDEX)

void Gpio_Init(uint32_t RCC_APB2Periph,    
			   GPIO_TypeDef * GPIO_Port,  
			   uint16_t GPIO_Pin,         
			   GPIOMode_TypeDef GPIO_Mode, 
			   GPIOSpeed_TypeDef GPIO_Speed );

#define  _GPIO_INIT(PORT,INDEX,SPEED,MODE)    Gpio_Init(RCC_APB2Periph_GPIO##PORT ,\
                                              GPIO##PORT,INDEX, \
											  GPIO_Mode_##MODE, \
											  GPIO_Speed_##SPEED##MHz)
#define  _GPIO_INITS(PORT,SHILFT,INITVALUE,SPEED,MODE)  _GPIO_INIT(PORT,1<<SHILFT,SPEED,MODE); \
														MG(PORT,SHILFT,XXX)= INITVALUE;
#define  GPIO_INITS(PORTSHILFT,SPEED,MODE)	  _GPIO_INITS(PORTSHILFT,SPEED,MODE)

/*输出口定义*/
#define  PINO(P)	MG(P)
/*输入口定义*/
#define  PINI(P)	MGIN(P)
/******************************************************************************/ 
/*IO口定义     参数1:IO端口 参数2:引脚 参数3:初始化状态*/
/*注意: 在编辑下列端口定义时不要在逗号前后使用空格*/
/******************************************************************************/
#if	 BUILD_PROJECT == BOOT_HD

	#define		PIN_LED			D,13,0      /*LED*/	
	#define		PIN_armchair_switch			B,9,1      /*按摩椅启动停止*/	
	#define		PIN_armchair_status_IN			E,0,1      /*按摩椅状态输入*/
	#define		PIN_armchair_status_OUT			E,1,1      /*按摩椅状态输输出，闲置以后用*/
#endif	


/*蜂鸣器引脚初始化*/
	#define   _GPIO_LED_INIT() 	    GPIO_INITS(PIN_LED,50,Out_PP);
	
	#define   _GPIO_armchair_INIT() GPIO_INITS(PIN_armchair_switch,50,Out_PP); \
										GPIO_INITS(PIN_armchair_status_IN,50,Out_OD); \
										GPIO_INITS(PIN_armchair_status_OUT,50,Out_PP);
/******************************************************************************/	
#endif


