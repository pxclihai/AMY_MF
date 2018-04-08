#include "config.h"
#include "iap_updata.h"
/*******************************************************************************/
/*Flash页大小*/
#ifdef STM32F10X_MD
	 #define PAGE_SIZE                         (0x400)
	 #define FLASH_SIZE                        (0x40000) /* 128K */
#elif defined STM32F10X_HD
	 #define PAGE_SIZE                         (0x800)
	 #define FLASH_SIZE                        (0x80000) /* 512K */
#elif defined STM32F10X_CL
	 #define PAGE_SIZE                         (0x800)
	 #define FLASH_SIZE                        (0x40000) /* 256K */
#else 
 	 #error "Please select first the STM32 device to be used (in stm32f10x.h)"    
#endif /* STM32F10X_MD */

/*程序开始地址0x8000*/
  #ifdef STM32_BOOT
	
			#if CONF_SPI_FLASH_EN
    	#define START_ADDR	((uint32_t*)(0x0))
				#else
			#define START_ADDR	((uint32_t*)(0x08020000))
			#endif 

		#else	
		#define START_ADDR	((uint32_t*)(0x08008000))
    #endif 


/*允许程序空间为218K*/	  
#define MAX_SIZE	1024 * 218	
	
#define PAGE_ADDR(ADDR)		(ADDR & (~(((uint32_t)PAGE_SIZE) - 1)))

/*******************************************************************************/
#define WRITE_FLASH(Addr,Data)	FLASH_ProgramWord(Addr,Data)
#define CLEAR_FLASH(Addr)		IAP_Page_Clear(Addr)
#define LOCK_FLASH()	     	FLASH_Lock()
#define UNLOCK_FLASH()	     	FLASH_Unlock()

/*******************************************************************************/
typedef __packed struct 
{
	uint32_t*	startAddr; 		//起始地址
	uint32_t	pageSize;		//页大小
	uint32_t*	endAddr;		//最大的地址

}tagUpdateArg;

/*******************************************************************************/
const static tagUpdateArg	   _gUpdateArg = {
	START_ADDR,
	PAGE_SIZE,					//块尺寸
	START_ADDR + (MAX_SIZE>>2)	//最大结束地址
};

/*******************************************************************************/
typedef __packed struct
{
	char* writePtr;						 //写指针
	char remainBuf[sizeof(uint32_t)];	 //剩余未写入flash的字符数组
	uint16_t	remainSize;				 //剩余未写入flash的字符数	
}tagUpdateVal;

/*******************************************************************************/
typedef __packed union _WRData_
{
    uint32_t Data32;	    
	char char8[sizeof(uint32_t)];  
}WRData;

union data  
{  
   unsigned long a;  
    unsigned char tab[4];  
}LONG_TO_CHAR4; 


/*******************************************************************************/
static tagUpdateVal _gUpdateVal = {
	(char*)START_ADDR,
	"",
	0
}; 


/*******************************************************************************/
static int IAP_Page_Clear(uint32_t startAddr)
{
	#if CONF_SPI_FLASH_EN
	#ifndef STM32_BOOT
	UNLOCK_FLASH();
	API_WatchDog_Feed();
	for(;FLASH_ErasePage(startAddr)!=FLASH_COMPLETE;)
		API_WatchDog_Feed();
	LOCK_FLASH();
	#endif
	#else
	UNLOCK_FLASH();
	API_WatchDog_Feed();
	for(;FLASH_ErasePage(startAddr)!=FLASH_COMPLETE;)
		API_WatchDog_Feed();
	LOCK_FLASH();
	#endif	
  	return 0;
	
}

/*******************************************************************************/
DWORD IAP_Update_Start(BOOL force,char* buf,size_t size)
{
	BOOL reStart = true;
	if(!force)
	{
		if(size >= 8 && buf != NULL)
		{
			int i = 0;
			DEBUG("\n");
			for(i =0; i < 8; i++)
			{
				if(buf[i] != iap_data.WCDMAUpdateArg.md5[i])
					break;
			}
			if(i ==8)
			{

				reStart = false;//匹配 ,断点续传
			}
		}
	}
	_gUpdateVal.remainSize = 0;
	if(reStart)	
	{
		DEBUG("\nreStart to update\n");
	
		_gUpdateVal.writePtr = (char*)_gUpdateArg.startAddr;
		iap_data.WCDMAUpdateArg.length = 0;
		CLEAR_FLASH((uint32_t)_gUpdateVal.writePtr);
		#if CONF_SPI_FLASH_EN
		SST25_clr_256_BLOCK();
		#endif
		memcpy(iap_data.WCDMAUpdateArg.md5,buf,8);
		Save_Iap_Config();	
	}
	else
	{

	 	_gUpdateVal.writePtr = 	((char*)_gUpdateArg.startAddr) +  iap_data.WCDMAUpdateArg.length;
		DEBUG("\nstart to continue last update:%d\n",iap_data.WCDMAUpdateArg.length);
		return 	iap_data.WCDMAUpdateArg.length;	//返回已经上传的长度
	}	
	//要先进行擦除块操作
	return 0;
}

/*******************************************************************************/
int IAP_Update_Write(char* buf,size_t size)
{
	//u16 i;
		#if CONF_SPI_FLASH_EN
	u16 j;
	u8 data_buf[600]={0};
		#endif
	uint32_t startAddr =  (uint32_t)_gUpdateVal.writePtr;
	uint32_t endAddr = startAddr + size;
	uint32_t startPage = PAGE_ADDR(startAddr);
	uint32_t endPage = PAGE_ADDR(endAddr);
	uint32_t chAlign = 0;
	DWORD writenLength	= 0;		//已经写入到flash上的字节数

	WRData wrData;
	int count = 0;

	if(_gUpdateVal.remainSize >=  sizeof(uint32_t))
		return -1;

	API_WatchDog_Feed();

	if(startPage < endPage)
	{
		CLEAR_FLASH(endPage);	
	}
	else if(startPage > endPage)
	{
		return -1;	//应该不可能出现吧
	}
	chAlign = startAddr % sizeof(uint32_t);

	if(chAlign > 0)
	{
	 	return -1;	//字节未对齐，有bug 
	}

	writenLength = 	(_gUpdateVal.writePtr - ((char*)_gUpdateArg.startAddr)) + _gUpdateVal.remainSize ;
	
	#if CONF_SPI_FLASH_EN
	#ifndef STM32_BOOT
	UNLOCK_FLASH();
	#endif
	#else
	UNLOCK_FLASH();
	#endif	
	//先写上面留下来的多余的字节
	if(_gUpdateVal.remainSize > 0)
	{
		memcpy(wrData.char8,_gUpdateVal.remainBuf,_gUpdateVal.remainSize);
		count += _gUpdateVal.remainSize;
	}

	
	#if CONF_SPI_FLASH_EN
	{
		#ifdef STM32_BOOT

			qwl_sendchar(0x0d);
		qwl_sendchar(0x0a);
		qwl_sendstring1("长度:");
		qwl_sendchar(NO[(size>>12)&0x0f]);	
		qwl_sendchar(NO[(size>>8)&0x0f]);
		qwl_sendchar(NO[(size>>4)&0x0f]);
		qwl_sendchar(NO[(size>>0)&0x0f]);
		qwl_sendchar(0x0d);
		qwl_sendchar(0x0a);	
		SST25_W_BLOCKQ((u32)_gUpdateVal.writePtr,(u8*)buf,size);
		
		SST25_R_BLOCK((u32)_gUpdateVal.writePtr,data_buf,size);

		qwl_sendchar(0x0d);
		qwl_sendchar(0x0a);
		qwl_sendstring1("总长度:");
		qwl_sendchar(NO[((u32)_gUpdateVal.writePtr>>28)&0x0f]);
		qwl_sendchar(NO[((u32)_gUpdateVal.writePtr>>24)&0x0f]);
		qwl_sendchar(NO[((u32)_gUpdateVal.writePtr>>20)&0x0f]);
		qwl_sendchar(NO[((u32)_gUpdateVal.writePtr>>16)&0x0f]);
		qwl_sendchar(NO[((u32)_gUpdateVal.writePtr>>12)&0x0f]);
		qwl_sendchar(NO[((u32)_gUpdateVal.writePtr>>8)&0x0f]);
		qwl_sendchar(NO[((u32)_gUpdateVal.writePtr>>4)&0x0f]);
		qwl_sendchar(NO[((u32)_gUpdateVal.writePtr>>0)&0x0f]);
		qwl_sendchar(0x0d);
		qwl_sendchar(0x0a);
		for(j=0;j<size;j++)
		{
			if(buf[j]==data_buf[j])
			qwl_sendchar(data_buf[j]);
			else
			{
				qwl_sendstring("flash坏了");
			break;
			}
		}
		if(j!=size)
			return -1;
		qwl_sendchar(0x0d);
		qwl_sendchar(0x0a);

		SST25_R_BLOCK((u32)START_ADDR,data_buf,size);
		for(j=0;j<256;j++)
		qwl_sendchar(data_buf[j]);
		qwl_sendchar(0x0d);
		qwl_sendchar(0x0a);
		
		_gUpdateVal.writePtr += size;

		#else
		while(size > 0)
			{
				wrData.char8[count] = *buf;
				size--;
				buf++;
				count++;
				if(count == sizeof(uint32_t))
				{
						WRITE_FLASH((uint32_t)_gUpdateVal.writePtr,wrData.Data32);
					_gUpdateVal.writePtr += sizeof(uint32_t);
					count = 0;

				}

			}
		#endif
	}
	#else
	
	{
			while(size > 0)
			{
				wrData.char8[count] = *buf;
				size--;
				buf++;
				count++;
				if(count == sizeof(uint32_t))
				{
						WRITE_FLASH((uint32_t)_gUpdateVal.writePtr,wrData.Data32);
					_gUpdateVal.writePtr += sizeof(uint32_t);
					count = 0;

				}

			}
		}
		#endif	
/*	for(i=0;i<2;i++)
		{
		SST25_R_BLOCK(i*1024,data_buf,1024);
		for(j=0;j<1024;j++)
		qwl_sendchar(data_buf[j]);
		}*/
	_gUpdateVal.remainSize = count;
	if(_gUpdateVal.remainSize > 0)
	{
		memcpy(_gUpdateVal.remainBuf,wrData.char8,_gUpdateVal.remainSize);
		writenLength = 	(_gUpdateVal.writePtr - ((char*)_gUpdateArg.startAddr));
	}
	else
	{
		writenLength = 	(_gUpdateVal.writePtr - ((char*)_gUpdateArg.startAddr));
	}

	if(writenLength != iap_data.WCDMAUpdateArg.length)
	{
		iap_data.WCDMAUpdateArg.length = writenLength;
		Save_Iap_Config();	//储存长度数据

	}
#if CONF_SPI_FLASH_EN
	#ifndef STM32_BOOT
	LOCK_FLASH();
	#endif
	#else
   	LOCK_FLASH();
		#endif	
	API_WatchDog_Feed();
	return 0;
}

/*******************************************************************************/
void IAP_ReStart(void)
{
	__disable_fault_irq();



 #ifdef STM32_BOOT
	#if CONF_SPI_FLASH_EN
	iap_data.copyflag = 2;
	#else
	iap_data.copyflag = 0;
	#endif
	iap_data.writeflag = 0x00;  //进入程序哪个APP
	#else
	iap_data.copyflag = 0x01;	//拷贝数据标志清除
	iap_data.writeflag = 0x01;  //进入程序哪个APP
#endif 
	Save_Iap_Config();
	
	__set_FAULTMASK(1);      // 关闭所有中端
	NVIC_SystemReset();      // 复位
}

/*******************************************************************************/
int IAP_Update_End(void)
{

	DWORD fileLen = iap_data.WCDMAUpdateArg.length + 	_gUpdateVal.remainSize; 
	DEBUG("File length:%d\n",iap_data.WCDMAUpdateArg.length + 	_gUpdateVal.remainSize);
	if(	_gUpdateVal.remainSize > 0 && _gUpdateVal.remainSize < sizeof(uint32_t))
	{
		WRData wrData;
		memcpy(wrData.char8,_gUpdateVal.remainBuf,_gUpdateVal.remainSize);
				#if CONF_SPI_FLASH_EN
				{
					#ifdef STM32_BOOT
					LONG_TO_CHAR4.a =wrData.Data32;
					SST25_W_BLOCK((uint32_t)_gUpdateVal.writePtr,LONG_TO_CHAR4.tab ,sizeof(uint32_t));
					#else
					WRITE_FLASH((uint32_t)_gUpdateVal.writePtr,wrData.Data32);
					#endif
				}
				#else
				{	
					WRITE_FLASH((uint32_t)_gUpdateVal.writePtr,wrData.Data32);
				}
				#endif
	}

	
	memset(&iap_data.WCDMAUpdateArg,NULL,8);
	
	#ifdef STM32_BOOT
	#if CONF_SPI_FLASH_EN
	iap_data.copyflag = 2;
	#else
	iap_data.copyflag = 0;
	#endif
	iap_data.src_add = (u32)START_ADDR;
	iap_data.dest_add = 0x8008000;
	iap_data.copy_long = fileLen;
	Save_Iap_Config();
	#endif
	API_SetTimer(3000,1,TIMER_TYPE_IAP_UPDATE_END,NULL,IAP_ReStart);

	/*跳转到用户程序*/	

	return 0;
}

/*******************************************************************************/
typedef  void (*pFunction)(void);

/*程序开始地址*/
#ifdef STM32_BOOT
uint32_t ApplicationAddress = 0x8008000;
uint32_t FlashDestination = 0x8020000;
#else
uint32_t ApplicationAddress = 0x8008000;
uint32_t FlashDestination = 0x8008000;
    #endif 


/*******************************************************************************/
uint32_t FLASH_PagesMask(__IO uint32_t Size)
{
  uint32_t pagenumber = 0x0;
  uint32_t size = Size;

  if ((size % PAGE_SIZE) != 0)
  {
    pagenumber = (size / PAGE_SIZE) + 1;
  }
  else
  {
    pagenumber = size / PAGE_SIZE;
  }
  return pagenumber;
}

/*******************************************************************************/
int IAP_copyflash(u32 src_add,u32 dest_add,u32 copy_long)
{	
	int32_t  j;
	uint32_t EraseCounter = 0x0;
	uint32_t RamSource;
	FLASH_Status FLASHStatus = FLASH_COMPLETE;
	uint32_t NbrOfPage = 0;
				if(iap_data.copyflag !=2)
				{
					if(dest_add<0x08008000||dest_add+copy_long>src_add) 
					{
						return -1;
					}
				}
	FLASH_Unlock();
	/* Erase the FLASH pages */
	NbrOfPage = FLASH_PagesMask(copy_long);
	ApplicationAddress = dest_add;
	FlashDestination = ApplicationAddress;
	for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
   	{
    	FLASHStatus = FLASH_ErasePage(FlashDestination + (PAGE_SIZE * EraseCounter));
		API_WatchDog_Feed();
   	}
	/*copy*/
	RamSource = src_add;
	for (j = 0;j < copy_long;j += 4)
    {
       /* Program the data received into STM32F10x Flash */
       FLASH_ProgramWord(FlashDestination, *(uint32_t*)RamSource);
       if (*(uint32_t*)FlashDestination != *(uint32_t*)RamSource)
	   {
	   		FLASH_Lock();
	   		return 3;
	   }
       FlashDestination += 4;
       RamSource += 4;
	   API_WatchDog_Feed();
    }
	FLASH_Lock();
	IAP_ReStart();

	return 0;
}
#if CONF_SPI_FLASH_EN
/*******************************************************************************/
int IAP_copyflash1(u32 src_add,u32 dest_add,u32 copy_long)
{	
	int32_t  j;
	u32 n;
	u16 i;
	uint32_t EraseCounter = 0x0;
	uint32_t RamSource;
	FLASH_Status FLASHStatus = FLASH_COMPLETE;
	uint32_t NbrOfPage = 0;
	u8 data_buf[1024]={0};
				if(iap_data.copyflag !=2)
				{
					if(dest_add<0x08008000||dest_add+copy_long>src_add) 
					{
						return -1;
					}
				}
FLASH_Unlock();
	/* Erase the FLASH pages */
	NbrOfPage = FLASH_PagesMask(copy_long);
	ApplicationAddress = dest_add;
	FlashDestination = ApplicationAddress;
	for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
   	{
    	FLASHStatus = FLASH_ErasePage(FlashDestination + (PAGE_SIZE * EraseCounter));
		API_WatchDog_Feed();
   	}
	/*copy*/
	RamSource = src_add;
	n=(copy_long/1024);
	for (j = 0;j < n;j++)
    {
			SST25_R_BLOCK(RamSource,data_buf,1024);
			RamSource += 1024;
			for(i=0;i<1024;i+=4)
			{
						 FLASH_ProgramWord(FlashDestination, *(u32*)&data_buf[i]);
					 if (*(uint32_t*)FlashDestination != *(u32*)&data_buf[i])
				 {
						FLASH_Lock();
						return 3;
				 }

				FlashDestination += 4;
				API_WatchDog_Feed();
			}
    }
			n=(copy_long%1024);
			SST25_R_BLOCK(RamSource,data_buf,n);
			RamSource += 1024;
			for(i=0;i<n;i+=4)
			{
						 FLASH_ProgramWord(FlashDestination, *(u32*)&data_buf[i]);
					 if (*(uint32_t*)FlashDestination != *(u32*)&data_buf[i])
				 {
						FLASH_Lock();
						return 3;
				 }

				FlashDestination += 4;
				API_WatchDog_Feed();
			}
FLASH_Lock();
	IAP_ReStart();

	return 0;
}
#endif
/*******************************************************************************/
int Jump_To_UserProgram(void)
{
	uint32_t JumpAddress;
	pFunction Jump_To_Application;

	/* Test if user code is programmed starting from address "ApplicationAddress" */
    if (((*(u32*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
    { 
		/* Jump to user application */
		JumpAddress = *(u32*) (ApplicationAddress + 4);
		Jump_To_Application = (pFunction) JumpAddress;
		__set_MSP(*(u32*) ApplicationAddress);
		Jump_To_Application();
    }

	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Select_Program_run
// 功能描述  : 选择进入用户或引导程序
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void Select_Program_run(void)
{
		
	/*获取程序升级参数*/
	if(API_Get_Iap_Config() > -1)
	{
		ModAddress = iap_data.Address;
		#ifndef STM32_BOOT
		/*有程序更新标志,进入升级*/
		if(iap_data.writeflag == 0)
		{
			if(iap_data.copyflag == 0)
			{
				/*复制程序，来源内部flash*/
				API_IAP_copyflash(iap_data.src_add,
	 							  iap_data.dest_add,
	 							  iap_data.copy_long );
			}
			else if(iap_data.copyflag == 1)
			{
			
			}
			else if(iap_data.copyflag == 2)
			{
			/*复制程序,来源外部flash*/
				API_IAP_copyflash1(iap_data.src_add,
	 							  iap_data.dest_add,
	 							  iap_data.copy_long );
			}
			else
			{
				/*进入boot进行升级*/
			}
		}
		

 
		/*直接进入用户程序*/															 
		else if(iap_data.writeflag ==1)
		{
			/*跳转到用户程序前必须将打开的所有外设关闭*/
			TIM_Cmd(TIM5, DISABLE);

			USART_Cmd(USART1, DISABLE);
			USART_Cmd(USART2, DISABLE);
			USART_Cmd(USART3, DISABLE);
			USART_Cmd(UART4, DISABLE);
			USART_Cmd(UART5, DISABLE);

			USART_DMACmd(USART1, USART_DMAReq_Rx, DISABLE);
			USART_DMACmd(USART2, USART_DMAReq_Rx, DISABLE);
			USART_DMACmd(USART3, USART_DMAReq_Rx, DISABLE);
			USART_DMACmd(UART4, USART_DMAReq_Rx, DISABLE);

			DMA_Cmd(DMA1_Channel5, DISABLE);
			DMA_Cmd(DMA1_Channel6, DISABLE);
			DMA_Cmd(DMA1_Channel3, DISABLE);
			DMA_Cmd(DMA2_Channel3, DISABLE);

			API_Jump_To_UserProgram();
		}

		else
		{
			/*进入boot进行升级*/
		}
			#endif

	}		
	/*看门狗初始化*/
    API_WatchDog_Init();
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Check_Run_User_Program_Falg
// 功能描述  : 检查进入用户或引导程序标志
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void Check_Run_User_Program_Falg(void)
{
	if(Run_User_Program_Falg == true)
	{
		Run_User_Program_Falg = false;
		API_KillTimer(TIMER_TYPE_IAP_SELECT_PROGRAM);	
	}
}
/*******************************************************************************/

