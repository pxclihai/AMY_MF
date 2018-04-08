#ifndef _IAP_H
#define _IAP_H
#ifdef __cplusplus
extern "C" {
#endif

#include"config.h"

#define iap_data_add  0x08007800    /*256个字节,128个字*/

typedef __packed struct
{
  	char md5[8];
	DWORD length;	//长度
}tagWCDMAUpdateArg;

/*iap用到的数据*/
typedef __packed struct
{
	/*串口通信参数*/
	uint16_t USART_num;   					/*串口号			   */
	uint16_t remap;						    /*是否重映射		   */
	uint32_t bps;         					/*波特率			   */
	uint16_t writecompleteflag;             /*用户程序完整性	   */
	uint16_t writeflag;					    /*用户请求编程标志	   */
	uint16_t InitUserParFlag;				/*初始化用户参数标志   */
	/*网络通信参数*/ 
	uint8_t rip[4];
	uint16_t rport; 
	uint8_t lip[12];
	uint8_t lown_hw_adr[6];
	/*程序复制*/
	u32 copyflag;							/*复制标志位 0有效 	   */
	u32 src_add;							/*源地址			   */
	u32 dest_add;							/*目标地址			   */
	u32 copy_long; 							/*长度，19+8=27个半字  */
	uint16_t Address;						/*设备地址*/
	tagWCDMAUpdateArg WCDMAUpdateArg;		/*应用程序升级,断点续传参数*/
	uint16 CheckSum;						/*校验和,此参数须在结构体尾部*/
}IAP;


#define iap_constlong  sizeof(IAP)/2        /*iap数据, 半字（16位）个数*/

extern IAP iap_data ;

void goto_iap(void) ;
int Save_Iap_Config(void);
int Get_Iap_Config(void);

#ifdef __cplusplus
}
#endif

#endif

