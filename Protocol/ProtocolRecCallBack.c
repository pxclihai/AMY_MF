#include "config.h"

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ReceivePro_Debug_Dispose
// 功能描述  : 收到的Debug口协议数据解析判断处理
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int ReceivePro_Debug_Dispose(USART_FUNCTION uf,PROTOCOL_TYPE Pro_Type,uchar* buffer, size_t size)
{
	switch(Pro_Type)
	{
		case PROTOCOL_TYPE_POSTS:	 //邮电部协议
			DealPostProtocol(uf,buffer,size);
		
			break;
		case PROTOCOL_TYPE_COMPANY:	 //公司协议							   	
			break;
		case PROTOCOL_TYPE_DOWNLOAD: //程序升级协议
			DownLoad_DealProtocol(uf,buffer,size);
			break;
		case PROTOCOL_TYPE_MF://无用pxc
			//DealMfPro(uf,buffer,size);
		default:break;
	}
	
	API_ResetProtocolBuffer(uf,RESETRTBUFF_TYPE_RX);

	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
//                          程序结束(END)
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

