#ifndef IAP_UPDATA_H
#define IAP_UPDATA_H
#include "config.h"
int IAP_Update_Write(char* buf,size_t size);
DWORD IAP_Update_Start(BOOL force,char* buf,size_t size);
int IAP_Update_End(void);
int IAP_copyflash(u32 src_add,u32 dest_add,u32 copy_long);
int IAP_copyflash1(u32 src_add,u32 dest_add,u32 copy_long);
int Jump_To_UserProgram(void);
void Select_Program_run(void);
void Check_Run_User_Program_Falg(void);

#endif
