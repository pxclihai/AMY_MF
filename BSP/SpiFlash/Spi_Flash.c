#include "config.h"
#include "SPI_Flash.h"


u8 SPI_Flash_ReadByte(void);		    //flash操作基本函数，读一个字节
u8 SPI_Flash_SendByte(u8 byte);		    //FLASH操作基本函数，发送一个字节

void wip(void);
void wen(void);
void wdis(void);
void wsr(void);
unsigned char rdsr(void);
unsigned char FlashReadID(void); 

unsigned char fac_id;		          //BFH: 工程码SST
unsigned char dev_id;	              //41H: 器件型号SST25VF016B
static unsigned char gFlashType = 0x25;
void block_clr(unsigned long a1);
/****************************************************************************
* 名    称：Delay_US_FALSH(void)
* 功    能：全模块解锁
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void Delay_US_FALSH(u16 us){
	u8 i;
	for(i=0;i<us;i++)
		Delay_2us();
	
}

/****************************************************************************
* 名    称：void unlock(void)
* 功    能：全模块解锁
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void unlock(void){
	if(gFlashType == 0x26)
	{
		Select_Flash();
		SPI_Flash_SendByte(0x98);
		NotSelect_Flash();
		wen();
		wsr();
		Delay_MS(1);
		wdis();
	}
}

/****************************************************************************
* 名    称：void wen(void)
* 功    能：写使能
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void wen(void){
	Select_Flash();
	SPI_Flash_SendByte(0x06);
	NotSelect_Flash();
}

/****************************************************************************
* 名    称：void wdis(void)
* 功    能：写禁止
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void wdis(void){

	Select_Flash();
	SPI_Flash_SendByte(0x04); 
	NotSelect_Flash();
	wip();
	
}	
/****************************************************************************
* 名    称：void wsr(void)
* 功    能：写状态
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void wsr(void){	
	if(gFlashType == 0x25)
	{
		Select_Flash();
		SPI_Flash_SendByte(0x50);
		NotSelect_Flash(); 
		Select_Flash();
		SPI_Flash_SendByte(0x01);
		SPI_Flash_SendByte(0x00);
		NotSelect_Flash();
		wip();
	}
	else
	{
		Select_Flash();
		SPI_Flash_SendByte(0x01);
		SPI_Flash_SendByte(0x00);
		SPI_Flash_SendByte(0x02);	
		NotSelect_Flash();
		wip();
	}
}
/****************************************************************************
* 名    称：void wip(void)
* 功    能：忙检测
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void wwen(void){
	unsigned char a=1;
	if(gFlashType == 0x26)
	{
		while((a&0x02)!=0x02) 
			a=rdsr();
	}

}

/****************************************************************************
* 名    称：void wip(void)
* 功    能：忙检测
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void wip(void){
	unsigned char a=1;
	if(gFlashType == 0x25)
	{
		while((a&0x01)==1) 
			a=rdsr();
	}
	else
	{
		while((a&0x01)>0) 
			a=rdsr();
	}

}
/****************************************************************************
* 名    称：unsigned char rdsr(void)
* 功    能：读状态寄存器
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
unsigned char rdsr(void){
	unsigned char busy;
	Select_Flash();
	SPI_Flash_SendByte(0x05);
	busy = SPI_Flash_ReadByte();
	NotSelect_Flash();
	API_WatchDog_Feed();
	return(busy);
	
}


/****************************************************************************
* 名    称：unsigned char rdsr(void)
* 功    能：读设置寄存器
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
unsigned char rdcr(void){
	unsigned char busy;
	Select_Flash();
	SPI_Flash_SendByte(0x35);
	busy = SPI_Flash_ReadByte();
	NotSelect_Flash();
	API_WatchDog_Feed();
	return(busy);
	
}
/****************************************************************************
* 名    称：void SST25_R_BLOCK(unsigned long addr, unsigned char *readbuff, unsigned int BlockSize)
* 功    能：页读
* 入口参数：unsigned long addr--页   unsigned char *readbuff--数组   unsigned int BlockSize--长度
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/  
void SST25_R_BLOCK(unsigned long addr, unsigned char *readbuff, unsigned int BlockSize){
	unsigned int i=0; 
	wip();	
	Select_Flash();
	SPI_Flash_SendByte(0x0b);
	SPI_Flash_SendByte((addr&0xffffff)>>16);
	SPI_Flash_SendByte((addr&0xffff)>>8);
	SPI_Flash_SendByte(addr&0xff);
	SPI_Flash_SendByte(0);
	API_WatchDog_Feed();
	while(i<BlockSize){	
		readbuff[i]=SPI_Flash_ReadByte();
		
		i++;
	}
	API_WatchDog_Feed();
	NotSelect_Flash();	 	
}
/****************************************************************************
* 名    称：SST25_clr_256_BLOCK()
* 功    能：删除全部页
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/  
int SST25_clr_256_BLOCK()
{
	u16 i;
	u16 j;
	u8 data_buf[512]={0};
	//unlock();
	for(i=0;i<4;i++)
		{
			checktest();
			
		
		//Delay_MS(100);
		API_WatchDog_Feed();
		
		for(i=0;i<512;i++)
	{SST25_R_BLOCK(i*512,data_buf,512);	
		for(j=0;j<512;j++)
		{if(data_buf[j]!=0xff)
			break;
		}
		if(j!=512)
		{
			qwl_sendstring("擦FLASH不成功");
			qwl_sendchar(NO[j>>4]);
			qwl_sendchar(NO[j&0x0f]);
			break;
			//return -1;
		}
	}
		if(i==512)
		{
			qwl_sendstring("升级擦FLASH成功");
			return 0;
		}
		
		}
		return -1;
}

/****************************************************************************
* 名    称：void SST25_W_BLOCKQ(uint32_t addr, u8 *readbuff, uint16_t BlockSize)
* 功    能：页写,不带擦除功能
* 入口参数：uint32_t addr--页   u8 *readbuff--数组   uint16_t BlockSize--长度    
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/  
void SST25_W_BLOCKQ(unsigned long addr, unsigned char *readbuff, unsigned int BlockSize){
	unsigned int i=0,a2;
	
	unsigned long addr2 = addr;
	//unsigned char readbuff2[16] = {0};
	unlock();
/*	sect_clr(addr);   								  //删除页	
	if(gFlashType == 0x26)
	{
		SST25_R_BLOCK(addr,readbuff2,16);
		for(j=0;j<16;j++)
		{
			if(readbuff2[j]!=0xff)
			{
				sect_clr(addr); 
				break;
			}
		}
	}*/
	if(gFlashType == 0x25)
	{
		wsr();
		wen();
		Select_Flash();    
		SPI_Flash_SendByte(0xad);
		SPI_Flash_SendByte((addr&0xffffff)>>16);
		SPI_Flash_SendByte((addr&0xffff)>>8);
		SPI_Flash_SendByte(addr&0xff);
		SPI_Flash_SendByte(readbuff[0]);
		SPI_Flash_SendByte(readbuff[1]);
		NotSelect_Flash();
		i=2;
		API_WatchDog_Feed();
		while(i<BlockSize){
			a2=120;
			while(a2>0) a2--;
			Select_Flash();
			SPI_Flash_SendByte(0xad);
			SPI_Flash_SendByte(readbuff[i++]);
			SPI_Flash_SendByte(readbuff[i++]);
			NotSelect_Flash();
		}
		API_WatchDog_Feed();
		a2=100;
		while(a2>0) a2--;
		wdis();	
		Select_Flash();
	}
	else
	{
//		unlock();		
//		wen();
//		wsr();	
		wen();
		i=0;		   		 
		API_WatchDog_Feed();
		//页操作一次只能写256个字节，需要要多次操作，第一个字节需要50us，其他字节4us
		//写满256个之后重新发送02H命令
		while(i<BlockSize)
		{
			if(!(i&0x00FF))
			{
				NotSelect_Flash();
				wip();
//				Delay_MS(2);
				wen();
				API_WatchDog_Feed();
				Select_Flash();				
				SPI_Flash_SendByte(0x02);
				SPI_Flash_SendByte((addr2&0xffffff)>>16);
				SPI_Flash_SendByte((addr2&0xffff)>>8);
				SPI_Flash_SendByte(addr2&0xff);
				 
				SPI_Flash_SendByte(readbuff[i]);
				i++;
//				a2=600;
//				while(a2>0) 
//					a2--;
				addr2 +=256;
				Delay_US_FALSH(56);
			}
			else
			{
				SPI_Flash_SendByte(readbuff[i]);
				i++;
				Delay_US_FALSH(8);
//				a2=48;
//				while(a2>0) 
//					a2--;
			}
			
		}
		NotSelect_Flash();
		API_WatchDog_Feed();
		a2=100;
		while(a2>0) 
			a2--;
//		wdis();	
//		Select_Flash();
	}
	wip();
}
/****************************************************************************
* 名    称：void SST25_W_BLOCK(uint32_t addr, u8 *readbuff, uint16_t BlockSize)
* 功    能：页写
* 入口参数：uint32_t addr--页   u8 *readbuff--数组   uint16_t BlockSize--长度    
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/  
void SST25_W_BLOCK(unsigned long addr, unsigned char *readbuff, unsigned int BlockSize){
	unsigned int i=0,a2,j;
	
	unsigned long addr2 = addr;
	unsigned char readbuff2[16] = {0};
	unlock();
	sect_clr(addr);   								  //删除页	
	if(gFlashType == 0x26)
	{
		SST25_R_BLOCK(addr,readbuff2,16);
		for(j=0;j<16;j++)
		{
			if(readbuff2[j]!=0xff)
			{
				sect_clr(addr); 
				break;
			}
		}
	}
	if(gFlashType == 0x25)
	{
		wsr();
		wen();
		Select_Flash();    
		SPI_Flash_SendByte(0xad);
		SPI_Flash_SendByte((addr&0xffffff)>>16);
		SPI_Flash_SendByte((addr&0xffff)>>8);
		SPI_Flash_SendByte(addr&0xff);
		SPI_Flash_SendByte(readbuff[0]);
		SPI_Flash_SendByte(readbuff[1]);
		NotSelect_Flash();
		i=2;
		API_WatchDog_Feed();
		while(i<BlockSize){
			a2=120;
			while(a2>0) a2--;
			Select_Flash();
			SPI_Flash_SendByte(0xad);
			SPI_Flash_SendByte(readbuff[i++]);
			SPI_Flash_SendByte(readbuff[i++]);
			NotSelect_Flash();
		}
		API_WatchDog_Feed();
		a2=100;
		while(a2>0) a2--;
		wdis();	
		Select_Flash();
	}
	else
	{
//		unlock();		
//		wen();
//		wsr();	
		wen();
		i=0;		   		 
		API_WatchDog_Feed();
		//页操作一次只能写256个字节，需要要多次操作，第一个字节需要50us，其他字节4us
		//写满256个之后重新发送02H命令
		while(i<BlockSize)
		{
			if(!(i&0x00FF))
			{
				NotSelect_Flash();
				wip();
//				Delay_MS(2);
				wen();
				API_WatchDog_Feed();
				Select_Flash();				
				SPI_Flash_SendByte(0x02);
				SPI_Flash_SendByte((addr2&0xffffff)>>16);
				SPI_Flash_SendByte((addr2&0xffff)>>8);
				SPI_Flash_SendByte(addr2&0xff);
				 
				SPI_Flash_SendByte(readbuff[i]);
				i++;
//				a2=600;
//				while(a2>0) 
//					a2--;
				addr2 +=256;
				Delay_US_FALSH(56);
			}
			else
			{
				SPI_Flash_SendByte(readbuff[i]);
				i++;
				Delay_US_FALSH(8);
//				a2=48;
//				while(a2>0) 
//					a2--;
			}
			
		}
		NotSelect_Flash();
		API_WatchDog_Feed();
		a2=100;
		while(a2>0) 
			a2--;
//		wdis();	
//		Select_Flash();
	}
	wip();
}

/****************************************************************************
* 名    称：void block_clr(unsigned long a1)
* 功    能：块擦除
* 入口参数：unsigned long a1--页   
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void block_clr(unsigned long a1){
	char byu1,byu2;
	if(gFlashType == 0x25)
	{
		wsr();
		wen();
	}
	else
	{
//		wen();
//		wsr();
		wen();
		wwen();
		
	}
	byu1 = rdsr();
	byu2 = rdcr();
	Select_Flash();	  
	SPI_Flash_SendByte(0xd8);
	SPI_Flash_SendByte((a1&0xffffff)>>16);       //addh
	SPI_Flash_SendByte((a1&0xffff)>>8);          //addl 
	SPI_Flash_SendByte(a1&0xff);                 //wtt
	NotSelect_Flash();
	wip();
	byu1 = rdsr();
	byu2 = rdcr();

	
}
/****************************************************************************
* 名    称：void sect_clr(unsigned long a1)
* 功    能：页擦除
* 入口参数：unsigned long a1--页   
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void sect_clr(unsigned long a1){
//	char byu1,byu2;
	if(gFlashType == 0x25)
	{
		wsr();
		wen();
	}
	else
	{
//		wen();
//		wsr();
		wen();
		wwen();
		
	}
	rdsr();//byu1 = rdsr();
	rdcr();//byu2 = rdcr();
	Select_Flash();	  
	SPI_Flash_SendByte(0x20);
	SPI_Flash_SendByte((a1&0xffffff)>>16);       //addh
	SPI_Flash_SendByte((a1&0xffff)>>8);          //addl 
	SPI_Flash_SendByte(a1&0xff);                 //wtt
	NotSelect_Flash();
	wip();
	rdsr();//byu1 = rdsr();
	rdcr();//byu2 = rdcr();

	
}
/****************************************************************************
* 名    称：unsigned char FlashReadID(void)
* 功    能：读工厂码及型号的函数
* 入口参数：  
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
unsigned char FlashReadID(void)
{
	Select_Flash();	
  	SPI_Flash_SendByte(0x9f);
  	fac_id= SPI_Flash_ReadByte();		          //BFH: 工程码SST
	dev_id= SPI_Flash_ReadByte();	              //41H: 器件型号SST25VF016B
	SPI_Flash_ReadByte();   	
  	NotSelect_Flash();
	//判断flash的类型
	if(dev_id == 0x25)
	{
		gFlashType = 0x25;
	}
	else
	{
		
		gFlashType = 0x26;
	}
	return dev_id;	
}
void checktest()
{
//	unsigned long addr = 0x0D9000;
//	unsigned int i=0,a2;
//	unsigned char readbuff[1024];
//	unsigned char readbuff2[4096] ={0x05};
//	unsigned char writebuff[4096] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
//	unsigned char busy;
	unlock();
	wen();
	wsr();
	wen();
	
	Select_Flash();	  
	SPI_Flash_SendByte(0xC7);
	NotSelect_Flash();
//	SST25_R_BLOCK(addr,readbuff2,4096);
//	busy = rdsr();
//	sect_clr(addr); 
//	SST25_R_BLOCK(addr,readbuff2,4096);
////	for(i=0;i<1024;i++)
////	{
//	SST25_W_BLOCK(addr+4096*i,writebuff,4096);
//	SST25_R_BLOCK(addr,readbuff2,4096);
//	sect_clr(addr);
//	SST25_R_BLOCK(addr,readbuff2,4096);
//	}
//	for(i=0;i<1024;i++)
//	{
//		SST25_R_BLOCK(4096*i,readbuff2,4096);
//		for(a2=0;a2<4096;a2++)
//		{
//			if(readbuff2[a2]!= writebuff[a2])
//				busy = 1;
//		}
//		//清空readbuff2
//		for(a2=0;a2<4096;a2++)
//			readbuff2[a2] = 0xff;
//	}
//	while(1);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_Init
* Description    : Initializes the peripherals used by the SPI FLASH driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_Flash_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
   
  /* 使能SPI1 时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC ,ENABLE);	
  /* 配置 SPI1 引脚: SCK, MISO 和 MOSI */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  

  /* 配置PA4为SST25VF016B的片选  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);   

   /* 配置PC4为SST25VF016B的片选  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure); 
 
  /* SPI1配置 */ 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);
  
  /* 使能SPI1  */
  SPI_Cmd(SPI1, ENABLE);   
  NotSelect_Flash();

  GPIO_SetBits(GPIOC, GPIO_Pin_4);			//禁止触摸电路的片选  

  FlashReadID();
  unlock();
//  checktest();
//  while(1);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_ReadByte
* Description    : Reads a byte from the SPI Flash.
*                  This function must be used only if the Start_Read_Sequence
*                  function has been previously called.
* Input          : None
* Output         : None
* Return         : Byte Read from the SPI Flash.
*******************************************************************************/
u8 SPI_Flash_ReadByte(void)
{
  return (SPI_Flash_SendByte(Dummy_Byte));
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendByte
* Description    : Sends a byte through the SPI interface and return the byte 
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
u8 SPI_Flash_SendByte(u8 byte)
{
  /* Loop while DR register in not emplty */
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI2 peripheral */
  SPI_I2S_SendData(SPI1, byte);

  /* Wait to receive a byte */
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}/******************* (C) COPYRIGHT 2011 奋斗STM32 *****END OF FILE****/



