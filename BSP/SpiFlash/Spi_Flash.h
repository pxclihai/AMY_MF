#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_  

#define FLASH_CHREAD 		0x0B
#define FLASH_CLREAD 		0x03
#define FLASH_PREAD			0xD2

#define FLASH_BUFWRITE1 	0x84
#define FLASH_IDREAD 		0x9F
#define FLASH_STATUS 		0xD7
#define PAGE_ERASE 			0x81
#define PAGE_READ 			0xD2
#define MM_PAGE_TO_B1_XFER  0x53				// 将主存储器的指定页数据加载到第一缓冲区
#define BUFFER_2_WRITE 		0x87				// 写入第二缓冲区
#define B2_TO_MM_PAGE_PROG_WITH_ERASE 0x86	    // 将第二缓冲区的数据写入主存储器（擦除模式）

#define Dummy_Byte 			0xA5

#define Select_Flash()       GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define NotSelect_Flash()    GPIO_SetBits(GPIOA, GPIO_Pin_4)

extern unsigned char fac_id;		    //BFH: 工程码SST
extern unsigned char dev_id;	        //41H: 器件型号SST25VF016B 

void SPI_Flash_Init(void);	            //SPI初始化
unsigned char FlashReadID(void);		//读取flashID四个字节
void sect_clr(unsigned long a1); 	
void SST25_R_BLOCK(unsigned long addr, unsigned char *readbuff, unsigned int BlockSize);
void SST25_W_BLOCK(unsigned long addr, unsigned char *readbuff, unsigned int BlockSize);
void SST25_W_BLOCKQ(unsigned long addr, unsigned char *readbuff, unsigned int BlockSize);
int SST25_clr_256_BLOCK(void);
void checktest(void);
#endif


