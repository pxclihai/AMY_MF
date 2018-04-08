#include"IAP.h"


IAP iap_data = {0};

/************************烧写程序**********************************************
* 函数名   	     : goto_iap
* 说明   	     : 将权限交给iap模块，更新用户程序
*******************************************************************************/
void goto_iap(void)
{
	__disable_fault_irq();

	iap_data.writeflag=0x00;  //请求更新应用程序

	Save_Iap_Config();

	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x00);
	NVIC_SystemReset();
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Save_Iap_Config
// 功能描述  : 保存当前升级用户程序参数
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : 成功 0  失败 -1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int Save_Iap_Config(void)
{
	int i;
	char *ps = (char*)&iap_data;

	iap_data.CheckSum = 0;

	for(i = 0; i<sizeof(IAP)-2; i++)
	{
		iap_data.CheckSum += *ps;
		ps++;	
	}

	
	FLASH_Unlock();

	FLASH_ErasePage(iap_data_add);

	for(i=0;i<iap_constlong;i++)
	{
		FLASH_ProgramHalfWord( (iap_data_add+(i<<1)), *(u16*)((u32)&iap_data+(i<<1)));
	}

	FLASH_Lock();

	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Get_Iap_Config
// 功能描述  : 获取当前升级用户程序参数
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : 成功 0  失败 -1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int Get_Iap_Config(void)
{
	int i;
	uint16 CheckSum = 0;
	char *ps = (char*)&iap_data;

	memcpy(&iap_data,(IAP*)iap_data_add,sizeof(IAP));

	for(i = 0; i<sizeof(IAP)-2; i++)
	{
		CheckSum += *ps;
		ps++;	
	}
	if(CheckSum != iap_data.CheckSum)
	{
		return -1;
	}
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
//                          程序结束(END)
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※


