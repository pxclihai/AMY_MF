
#include <stdio.h>
//#include <syslog.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "Protool.h"
#include "MF_Common.h"
char mEndType = 0;

//int GetEndType()
//{
//	 union w
//    {
//        int a;
//        char b;
//    }c;
//    c.a = 1;
//    if (c.b == 1)
//    {
//		printf("xiaoduan\n");
//		mEndType = LITTLE_END;
//    }
//	else
//	{
//		printf("daduan\n");
//		mEndType= BIG_END;
//		
//	}
//	return 0;
//}

U32 swapInt32(U32 value)
{
	if(mEndType ==LITTLE_END)
		return value;
	return ((value & 0x000000FF) << 24) |
	((value & 0x0000FF00) << 8)|
	((value & 0x00FF0000) >> 8)|
	((value&0xFF000000) >> 24);
}
/*U64 swapInt64(U64 value)
{
	if(mEndType ==LITTLE_END)
			return value;

	return ((value & 0x00000000000000FF) << 56)
	|((value & 0x000000000000FF00) << 40)
	|((value & 0x0000000000FF0000) << 24)
	|((value & 0x00000000FF000000) << 8)
	|((value&0x000000FF00000000) >> 8)
	|((value&0x0000FF0000000000) >> 24)
	|((value&0x00FF000000000000) >> 40)
	|((value&0xFF00000000000000) >> 56);
	return value;
}*/
u16 swapInt16(U16 value)
{
	if(mEndType ==LITTLE_END)
			return value;
	
	return ((value &0x00FF)<<8)|
				 ((value&0xFF00)>>8);
}
///*void testend()
//{
//	
//	GetEndType();
//	U16 a = 0x1122;
//	a = swapInt16(a);
//	printf("swapInt16 %x\n",a);
//	U32   c= 0x11223344;
//	U32   d= swapInt32(c);
//	printf("swapInt32 %x\n",d);
//	U64   e= 0x1122334455667788;
//	U64   f= swapInt64(e);
//	printf("swapInt64 %llX\n",f);
//	
//}*/
//	char		   *m_filePath=NULL; // 程序配置文件路径
//	void NewProgramProfile(const char *filePath)
//	{
//		int len = strlen(filePath);
//		m_filePath = (char *)malloc(len + 1);
//		strcpy(m_filePath, filePath);
//		m_filePath[len] = '\0';
//	}
//	
//	void DeleteProgramProfile()
//	{
//		if(m_filePath)
//			free(m_filePath);
//		m_filePath = NULL;
//	}
//	
//	
//	
//	int getKeyValueStr(const char *app, const char *key, 
//				const char *default_str, char *ret_str, unsigned int ret_str_size)
//	{
//		FILE *p = fopen(m_filePath, "r");
//		if (p == NULL) {
//			
//			printf("Can not fopen(\"%s\").", m_filePath);
//			if (default_str == NULL) {
//				printf("Not find Key=\"%s\" and no default_str to use.", key);
//				return -1;
//			}
//			int len = strlen(default_str);
//			len = ((unsigned int)len < (ret_str_size - 1) ? len : (ret_str_size - 1));
//			strncpy(ret_str, default_str, len);
//			ret_str[len] = '\0';
//			 
//			return -1;
//		}
//		char buffer[512], *pstr;
//		char app_found = false;
//		char use_default = true;
//		while (fgets(buffer, sizeof(buffer), p)) {
//			pstr = buffer;
//			while (*pstr == '\t' || *pstr == ' ')
//				pstr ++;
//			if (*pstr == '#') {
//				continue;
//			} else if (strstr(buffer, "[") != NULL) {
//				if (app_found)
//					break;
//				else if (strstr(buffer, app) != NULL)
//					app_found = true;
//			} else {
//				if (!app_found)
//					continue;
//				if (strstr(buffer, key) == NULL)
//					continue;
//				pstr = strstr(buffer, "=");
//				if (pstr == NULL)
//					break;
//				pstr ++;
//				while (*pstr == ' ')
//					pstr ++;
//				
//				unsigned int len = strlen(buffer);
//				do {
//					len --;
//				} while (buffer[len] == '\r' || buffer[len] == '\n' 
//					|| buffer[len] == '\t' || buffer[len] == ' ');
//				len ++;
//				
//				if (pstr < buffer + len) {
//					len = len - (pstr - buffer);
//					len = (len < (ret_str_size - 1) ? len : (ret_str_size - (unsigned int)1));
//					strncpy(ret_str, pstr, len);
//					ret_str[len] = '\0';
//					use_default = false;
//				}
//			}
//		}
//		if (use_default) {
//			if (default_str == NULL) {
//				printf("Not find Key=\"%s\" and no default_str to use.", key);
//				fclose(p);
//				return -1;
//			}
//			int len = strlen(default_str);
//			len = ((unsigned int)len < (ret_str_size - 1) ? len : (ret_str_size - 1));
//			strncpy(ret_str, default_str, len);
//			ret_str[len] = '\0';
//		}
//		//SPrint("ret_str=[%s]", ret_str);
//		fclose(p);
//		return 0;
//	}

