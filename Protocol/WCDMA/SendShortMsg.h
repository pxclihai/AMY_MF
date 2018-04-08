#ifndef _SENDWCDMASTATEVAL_H_
#define _SENDWCDMASTATEVAL_H_
#if CONF_WCDMA_EN
#ifdef __cplusplus
extern "C" {
#endif
#if CONF_WARN_SEND_EN
int AtCmdAnalysis_Receive_Warn_Delay(void);
void Receive_Warn(void);
#endif
int Msg_SendStatus_Notice(int status);
int Msg_Buffer_Analysis9A(USART_FUNCTION uf,uchar *DaPtr,uint16 len);
int Msg_Buffer_Analysis99(USART_FUNCTION uf,uchar *DaPtr,uint16 len);
	
int Msg_Buffer_PDUToUTF8(char* buf,size_t size,char* outbuf);
int Msg_Buffer_UTF8ToPDU(char* buf,size_t size,char* outbuf,int MaxSize);
int Msg_Buffer_Receive(char* phoneNum,uchar c);
int Msg_BufferQueue_Process(void);
int Msg_CnsModInit(const tagCnsModSet* mod, int size);

int Msg_Init(void);

#ifdef __cplusplus
} // extern "C"
#endif
#endif
#endif



