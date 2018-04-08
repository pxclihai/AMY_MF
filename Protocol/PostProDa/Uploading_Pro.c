#include "config.h"
extern u8 amy_check_pointer;
//¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù
// º¯ÊıÃû³Æ  : Uploading_Send_50
// ¹¦ÄÜÃèÊö  : »ñÈ¡µØÖ·
// ÊäÈë²ÎÊı  : None.
// Êä³ö²ÎÊı  : None
// ·µ»Ø²ÎÊı  : None
//¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù
static void Uploading_Send_50(USART_FUNCTION uf,uchar *DaPtr,uint16 len)
{	
	PostSendHead(uf);
	PostSend_Word(uf,ModAddress);     // ËÍĞ­ÒéµØÖ·		
	PostSendDataLen(uf);
	PostSendPostChksum(uf);
	PostSendTail(uf);
}
//¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù
// º¯ÊıÃû³Æ  : Uploading_Send_F8
// ¹¦ÄÜÃèÊö  : »ñÈ¡µØÖ·
// ÊäÈë²ÎÊı  : None.
// Êä³ö²ÎÊı  : None
// ·µ»Ø²ÎÊı  : None
//¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù
static void Uploading_Send_F8(USART_FUNCTION uf,uchar *DaPtr,uint16 len)
{	uchar  Cmd = DaPtr[4];
	
			Run_User_Program_Falg = true;
	switch (uf)
	{
		case USART_FUNCTION_DEBUG:
		{
			#ifdef STM32_BOOT
			iap_data.copyflag = 1;
			iap_data.Address = ModAddress;
            API_goto_iap();
			#else
			PostSendHead(uf);
			PostSendByteASC(uf,Cmd);		
			PostSendDataLen(uf);
			PostSendPostChksum(uf);
			PostSendTail(uf);
			#endif
		}break;
		case USART_FUNCTION_WCDMA:
		{u16 i;
			software_version[0]=DaPtr[23];
				software_version[1]=DaPtr[24];
			process_F8(0x00);
			if(exchange_platform.Platform_Choice ==PLATFORM_CHOICE_PUBLIC)
			{
				qwl_sendstring1("ÇĞ»»µ½Éı¼¶Æ½Ì¨");
							for(i=0;i<22;i++)
			{ 
				update_ip[i]=DaPtr[25+i];
				if(update_ip[i]!=0)
				qwl_sendchar(update_ip[i]);
			}

			qwl_sendchar(0x0d);
			qwl_sendchar(0x0a);
				exchange_platform.UPDATE_link_overtime =0;
				exchange_platform.UPDATE_link_times =0;
			exchange_platform.Platform_Choice =PLATFORM_CHOICE_UPDATE;//ÇĞ»»µ½Éı¼¶Æ½Ì¨
			API_nblot_init_0();//³õÊ¼»¯Ä£¿é£¬¸´Î»¿ªÊ¼
			net_data_send_init();//µ½Éı¼¶Æ½Ì¨ºó£¬ĞèÒª·¢ËÍÎÕÊÖĞÅÏ¢£¬½øĞĞ³õÊ¼»¯
			Delay_MS(1000);
			}
			
			
		}break;
		default:break;
	}

}
/*******************************************************************************
* º¯ Êı Ãû         : OUT_AMY_addr_please_set(u8 *ch)
* º¯Êı¹¦ÄÜ		   : ¸ô±Ú°´Ä¦ÒÎµØÖ·ÇëÇó
* Êä    Èë         : ÎŞ
* Êä    ³ö         : ÎŞ
*******************************************************************************/
void OUT_AMY_addr_please_set(u8 *ch)
{

	u8 send_buf[13];
	u8 addr[3];
	u16 len=0;
	addr[0]=0;
	addr[1]=ch[8];
	addr[2]=q2x1278_config_buf[4];
			send_buf[len]=0;
		len++;
		send_buf[len]=ch[8];//Ä¿µÄµØÖ·£¬Æ½Ì¨ÅäÖÃ
		len++;
		send_buf[len]=q2x1278_config_buf[4];
		len++;

													amy_amy_please(send_buf,len,0xc1,addr);

}
/*******************************************************************************
* º¯ Êı Ãû         : AMY_addr_please_set(u8 *ch)
* º¯Êı¹¦ÄÜ		   : °´Ä¦ÒÎµØÖ·ÇëÇóÉè¶¨
* Êä    Èë         : ÎŞ
* Êä    ³ö         : ÎŞ
*******************************************************************************/
void AMY_addr_please_set(u8 *ch)
{
							u16 j;
						u16 i;

						u8 addr[3];
	switch (ch[0])
	{
		case 1://²éÑ¯ÖÕ¶Ë×´Ì¬
		{									
											
											j=ch[1]*21;
												if(j>0)
												{qwl_sendstring("ÕÒµ½ÖÕ¶ËµØÖ·");
													j=(j/21);
												config_recwait_buf[j]=0x01;
													addr[0]=0;
													addr[1]=j;
													addr[2]=q2x1278_config_buf[4];
													amy_config_please(&addr[0]);
													amystate_send_flag[j]=1;//°´Ä¦ÒÎ×´Ì¬·¢ËÍ±êÖ¾
												config_recwait_time=0;
												}
												if(j==0)
												{qwl_sendstring("Ã»ÕÒµ½ÖÕ¶ËµØÖ·");

													amystate_send(0x06);
												}
		}
			break;
		case 2:
		{
			qwl_sendstring("°´Ä¦ÒÎÊ±¼äÉè¶¨");

								for(i=0;i<3;i++)
								plc_cofig_buf_1[i]=ch[i+2];
								amy_config_rec_flag=1;

								amy_config_send_1(ch[1]);
		}
			break;
		case 3:
		{
			qwl_sendstring("Íø¹Ø-°´Ä¦ÒÎ×´Ì¬");
			wg_state_flag=1;
			
			
		}
			break;
		case 4:
		{
			qwl_sendstring("Íø¹Ø-°´Ä¦ÒÎÆô¶¯");
							amy_cotrol_time=(u16)(ch[2]<<8)+ch[3];
							if(amy_cotrol_time>0)//µ±·¢À´ÓĞÊ±¼ä²ÅÆô¶¯£¬
							{
								if(amy_cotrol_flag==1)
							getwayoff_send_flag=getwayoff_send_flag|0x02;

								Shared_Module_switch(0x00);//0Æô¶¯ 1¹Ø±Õ
								amy_cotrol_flag=1;
							amy_cotrol_time_flag=0;//°´Ä¦ÒÎ¼ÆÊ±ÇåÁã
							armchair_switch=0;//°´Ä¦ÒÎ³õÊ¼»¯£¬¸ßÍ£Ö¹£¬µÍÆô¶¯
								
							qwl_sendstring("°´Ä¦ÒÎ¿ªÊ¼¹¤×÷88");
							LED=0;
							}
						wg_state_flag=2;	
		}break;
		case 5:
		{
			qwl_sendstring("»ñÈ¡ÖÕ¶ËÁĞ±í");
							poll_zd.poll_max_time =(u16)(ch[1]<<8)+ch[2];//ÂÖÑ¯Ê±¼ä
							poll_zd.list_please_flag =1;
			for(i=0;i<13;i++)
			{
				poll_zd.zd_list[i]=ch[3+i];//°´Ä¦ÒÎÔÚÏßÁĞ±í
				amy_check_pointer=0;
				qwl_sendchar(NO[poll_zd.zd_list[i]>>4]);
				qwl_sendchar(NO[poll_zd.zd_list[i]&0x0f]);
			}
			if(poll_zd.poll_max_time>0xfff0)
				poll_zd.poll_max_time=0xfff0;
			poll_zd.poll_time =poll_zd.poll_max_time;
		}
			break;
		case 6://²éÑ¯SIM¿¨ºÅ
		{
			qwl_sendstring("»ñÈ¡¿¨ºÅ");
			SIM_GET_FLAG=1;
		}break;
		
		default:
		{
		}
			break;
	}
}
/*******************************************************************************
* º¯ Êı Ãû         : process_fb
* º¯Êı¹¦ÄÜ		   : °´Ä¦ÒÎÅäÖÃ
* Êä    Èë         : ÎŞ
* Êä    ³ö         : ÎŞ
*******************************************************************************/
void process_fb(u8 *n)
{
	u8 write_flash_fb=0;
	u16 j;
								qwl_sendstring("´®¿ÚÅäÖÃ");
//								for(j=3;j<m;j++)
//								qwl_sendchar(n[j]);//²âÊÔÓÃ
							GATEWAY_peizhi_pack();
							net_send(post_buf,post_len);
								
								if(n[12]&0x01)//±íÊ¾IDÓĞĞ§
								{qwl_sendstring("ÅäÖÃID");
											for(j=0;j<16;j++)
												{AmyID[j]=n[j+14];
												qwl_sendchar(AmyID[j]);													
												}
									write_flash_fb=1;
								}
										if(n[12]&0x02)//Èí¼ş°æ±¾
								{qwl_sendstring("ÅäÖÃÈí¼ş°æ±¾");
											for(j=0;j<2;j++)
												{software_version[j]=n[j+31];
												qwl_sendchar(NO[software_version[j]]);													
												}		
								write_flash_fb=1;												
								}
								
								if(n[12]&0x04)//±íÊ¾Ó²¼şÓĞĞ§
								{qwl_sendstring("ÅäÖÃÓ²¼ş°æ±¾");
											for(j=0;j<2;j++)
												{Hardware_version[j]=n[j+34];
												qwl_sendchar(NO[Hardware_version[j]]);													
												}	
									write_flash_fb=1;
								}
								if(n[12]&0x40)//ÉèÖÃ°´Ä¦ÒÎÊıÁ¿
								{qwl_sendstring("ÉèÖÃ°´Ä¦ÒÎÊıÁ¿");
									qwl_sendchar(NO[n[43]>>4]);
										qwl_sendchar(NO[n[43]&0x0f]);
								amy_max_total=n[43];
									zd_sl_flag=0xcc;
					
								}
								
								if(n[12]&0x80)//ĞŞ¸ÄÉı¼¶IP
								{qwl_sendstring("ÅäÖÃÉı¼¶IP");
											for(j=0;j<21;j++)
												{Upgrade_ip[j]=n[j+45];
													if(Upgrade_ip[j])
												qwl_sendchar(Upgrade_ip[j]);													
												}	
												
												Upgrade_ip[j]=0;
									write_flash_fb=1;
								}
									if(n[11]&0x01)//±íÊ¾ÅäÖÃTCP
								{qwl_sendstring("ÅäÖÃTCP");
											for(j=0;j<21;j++)
												{light_TCP[j]=n[j+67];
													if(light_TCP[j])
												qwl_sendchar(light_TCP[j]);													
												}	
												light_TCP[j]=0;
									write_flash_fb=1;
								}
								if(n[11]&0x02)//±íÊ¾UDPÓĞĞ§
								{qwl_sendstring("ÅäÖÃUDP");
											for(j=0;j<21;j++)
												{light_UDP[j]=n[j+89];
													if(light_UDP[j])
												qwl_sendchar(light_UDP[j]);													
												}	
												light_UDP[j]=0;
									write_flash_fb=1;
								}
								
								if(n[11]&0x10)//±íÊ¾UDPÓĞĞ§
								{qwl_sendstring("ÅäÖÃSIM¿¨ºÅ");
											for(j=0;j<20;j++)
												{SIM_ID[j]=n[j+115];
													if(SIM_ID[j])
												qwl_sendchar(SIM_ID[j]);													
												}	
									write_flash_fb=1;
								}
								
								if(n[11]&0x04)//±íÊ¾ÔËÓªÉÌÓĞĞ§
								{

									qwl_sendstring("ÅäÖÃÔËÓªÉÌ");
									carrieroperator=n[111];
									qwl_sendchar(NO[carrieroperator]);													

									write_flash_fb=1;
								}
									if(n[11]&0x08)//±íÊ¾Ä£¿éÀàĞÍÓĞĞ§
								{qwl_sendstring("ÅäÖÃÄ£¿éÀàĞÍ");
									module_name=n[113];
									qwl_sendchar(NO[module_name>>4]);
									qwl_sendchar(NO[module_name&0x0f]);											

									write_flash_fb=1;
								}
								
								if(write_flash_fb)
								{
									Write_light_idip();	
								}
								
									if(n[12]&0x38)//ÉèÖÃ433µØÖ·
								{qwl_sendstring("ÉèÖÃ433µØÖ·");
										
			q2x1278_config_buf[1]=0x00;//µØÖ·¸ßÎ»³õÊ¼»¯Ä¬ÈÏÎª00	
			q2x1278_config_buf[0]=0xc0;//433ÅäÖÃÃüÁî
			q2x1278_config_buf[3]=0x1A;//²¨ÌØÂÊ
			q2x1278_config_buf[5]=0xc0;//¶¨µã·¢ËÍ
			
									if(n[12]&0x20)//ÊÇÉèÖÃÍø¹Ø
									{qwl_sendstring("=Íø¹Ø");
										q2x1278_config_buf[2]=0xff;
									}
									else
									{
										q2x1278_config_buf[2]=n[37];
										qwl_sendchar(NO[n[37]>>4]);
										qwl_sendchar(NO[n[37]&0x0f]);
										qwl_sendstring("=ÖÕ¶Ë");
									}
									
										if(n[12]&0x10)//ÉèÖÃ433µØÖ·
								{qwl_sendstring("ÉèÖÃ433Æµ¶Î");
									qwl_sendchar(NO[n[39]>>4]);
										qwl_sendchar(NO[n[39]&0x0f]);
								q2x1278_config_buf[4]=n[39];
					
								}
									sx1278_fb_flag=1;
								}

}
/*******************************************************************************
* º¯ Êı Ãû         : process_00
* º¯Êı¹¦ÄÜ		   : ½ÓÊÜÕıÈ·
* Êä    Èë         : ÎŞ
* Êä    ³ö         : ÎŞ
*******************************************************************************/
void process_00(u8 *n)
{

									if(n[5]==0x06)
									{
											getwayheart_sendok_flag=1;

											if(n[6]==1)//ÍøÂç´æ´¢Ğ£ÑéÂë²»ÕıÈ·£¬ĞèÒªÖØĞÂÉÏ´«
											{
												getwaylist_send_flag=1;//ÁĞ±íÒªÇóÖØĞÂÉÏ´«±êÖ¾
												getwaylist_send_times=0;
											}
											GATEWAY_heart_max_flag=(u16)(n[7]<<8)+n[8];//ÉèÖÃĞÄÌøÊ±¼ä
											qwl_sendstring("ÉèÖÃĞÄÌøÊ±¼ä888");
											qwl_sendchar(NO[(GATEWAY_heart_max_flag>>12)&0x0f]);
											qwl_sendchar(NO[(GATEWAY_heart_max_flag>>8)&0x0f]);
											qwl_sendchar(NO[(GATEWAY_heart_max_flag>>4)&0x0f]);
											qwl_sendchar(NO[GATEWAY_heart_max_flag&0x0f]);
											//amystate_send(0x00);//ÃüÁîÕıÈ·
											net_error_flag=0;//½ÓÊÜµ½Êı¾İ£¬Çå³¬Ê±´ÎÊı£¬·ÀÖ¹Ä£¿éÖØÆô
									}
								else if(getwaylist_sendok_flag==2)
								{getwaylist_sendok_flag=1;
									getwayoff_sendok_flag=1;
									//amystate_send(0x00);//ÃüÁîÕıÈ·
								}
								else //if(getwayoff_sendok_flag==2)
								{getwayoff_sendok_flag=1;
									//amystate_send(0x00);//ÃüÁîÕıÈ·
								}
}
/*******************************************************************************
* º¯ Êı Ãû         : void process_12()
* º¯Êı¹¦ÄÜ		   : ½ÓÊÜ12ÃüÁî·´À¡
* Êä    Èë         : 
* Êä    ³ö         : ÎŞ
*******************************************************************************/
void process_12()
{  

		

	u8 i;

	u8 post_buf1[200];
	u16 post_buf1_len=0;
char SoftWareVer[7];
	char HardWareVer[7];
	char Serial[30] = {0};
	char Content[20] = {0};
/*¹«Ë¾Ãû³Æ*/
u8 CompName[20] = COMPANY_NAME;

	
	sprintf(SoftWareVer,"V%4.2f",SOFTWARE_VER);
	sprintf(HardWareVer,"V%4.2f",HARDWARE_VER);
   
		
			sprintf(Content,"%s %s",__DATE__,__TIME__);
	
				post_buf1[post_buf1_len]=0x00;
				post_buf1_len++;
				post_buf1[post_buf1_len]=ModAddress;
				post_buf1_len++;
				for(i=0;i<6;i++)
				{post_buf1[post_buf1_len]=SoftWareVer[i];
					post_buf1_len++;
				}
				for(i=0;i<6;i++)
				{post_buf1[post_buf1_len]=HardWareVer[i];
					post_buf1_len++;
				}
				for(i=0;i<20;i++)
				{post_buf1[post_buf1_len]=CompName[i];
					post_buf1_len++;
				}
				for(i=0;i<16;i++)
				{post_buf1[post_buf1_len]=AmyID[i];
					post_buf1_len++;
				}
				for(i=0;i<4;i++)
				{post_buf1[post_buf1_len]=0x0;
					post_buf1_len++;
				}
				for(i=0;i<24;i++)
				{post_buf1[post_buf1_len]=Serial[i];
					post_buf1_len++;
				}
				for(i=0;i<20;i++)
				{post_buf1[post_buf1_len]=Content[i];
					post_buf1_len++;
				}
				

	
			post_group(post_buf1,post_buf1_len,0x00);
   net_send(post_buf,post_len);
} 
//¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù
// º¯ÊıÃû³Æ  : Uploading_Pro
// ¹¦ÄÜÃèÊö  : ÉÏ±¨¿ÚĞ­Òé´¦Àí
// ÊäÈë²ÎÊı  : None.
// Êä³ö²ÎÊı  : None
// ·µ»Ø²ÎÊı  : None
//¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù
void Uploading_Pro(USART_FUNCTION uf,uchar *DaPtr,uint16 len)
{
	uchar  Cmd = DaPtr[4];
	switch(Cmd)
	{	
		case 0x50://»ñÈ¡µØÖ·
			Run_User_Program_Falg = true;
			Uploading_Send_50(uf,DaPtr,len);
			break;
		case 0xF8:
		{	
			if(Net_Data_Send.upload_handshake.REC_state == WAIT_REC)//ÅĞ¶ÏÊ×´ÎÎÕÊÖ£¬µÈ´ı½ÓÊÜÊı¾İ
			{
				exchange_platform.UPDATE_link_times=0;
				Net_Data_Send.upload_handshake.REC_state=REC_OK;//
			}			
		Uploading_Send_F8(uf,DaPtr,len);
			
		}
			break;
		case 0x1b://°´Ä¦ÒÎ×´Ì¬²éÑ¯
		{
			u16 i;
			u16 j;
			u8 addr[3];
			u8 zd_id[16]={0};
			for(i=0;i<16;i++)
									{
										
										zd_id[i]=DaPtr[i+7];//ÅäÖÃÊ±¼äµ±×´Ì¬ÇëÇó·¢ËÍ
									}
								if(CRC8_Table(zd_id, 16)!=DaPtr[23])
								{
									qwl_sendstring("¶ş´ÎĞ£Ñé´íÎó");
									break;
								}
									
								qwl_sendstring("ÇëÇó×´Ì¬");
								

								for(i=0;i<16;i++)
									{
										
										plc_cofig_buf[i]=DaPtr[i+7];//ÅäÖÃÊ±¼äµ±×´Ì¬ÇëÇó·¢ËÍ
									}
									

										j=strsearch_qwl(plc_cofig_buf,amy_record_buf,5250,16);
												if(j>0)
												{qwl_sendstring("ÕÒµ½ÖÕ¶ËµØÖ·");
													j=(j/21);
												config_recwait_buf[j]=0x01;
													addr[0]=0;
													addr[1]=j;
													addr[2]=q2x1278_config_buf[4];
													amy_config_please(&addr[0]);
													amystate_send_flag[j]=1;//°´Ä¦ÒÎ×´Ì¬·¢ËÍ±êÖ¾
																									config_recwait_time=0;
												}
												if(j==0)
												{qwl_sendstring("Ã»ÕÒµ½ÖÕ¶ËµØÖ·");

													amystate_send(0x06);
												}
		}
			break;
		case 0x1e://°´Ä¦ÒÎÊ±¼äÉèÖÃ
		{
			u16 i;
			//u16 j;
			//u8 addr[3];
			u8 zd_id[16]={0};
											for(i=0;i<16;i++)
									{
										
										zd_id[i]=DaPtr[i+7];//ÅäÖÃÊ±¼äµ±×´Ì¬ÇëÇó·¢ËÍ
									}
								if(CRC8_Table(zd_id, 16)!=DaPtr[23])
								{
									qwl_sendstring("¶ş´ÎĞ£Ñé´íÎó");
									break;
								}
								qwl_sendstring("°´Ä¦ÒÎÊ±¼äÉè¶¨");
								for(i=0;i<16;i++)
								{
								plc_cofig_buf[i]=DaPtr[i+7];
								
								}

								for(i=0;i<3;i++)
								plc_cofig_buf_1[i]=DaPtr[i+24];
								amy_config_rec_flag=1;

								amy_config_send();
								//amystate_send(0x00);
		}
			break;
		case 0xfb://°´Ä¦ÒÎÅäÖÃ
		{
			process_fb(&DaPtr[1]);
		}	
		case 0x12://°´Ä¦ÒÎÅäÖÃ
		{
			process_12();
		}	
			break;
		case 0x24://°´Ä¦ÒÎÖ±½ÓµØÖ·²éÑ¯£¬µØÖ·ÉèÖÃÊ±¼ä£¬Íø¹ØÖ±½ÓÆô¶¯£¬Íø¹ØÖ±½Ó²éÑ¯°´Ä¦ÒÎ×´Ì
		{
			u16 i;
			u8 zd_id[16]={0};
											for(i=0;i<16;i++)
									{
										
										zd_id[i]=DaPtr[i+7];//ÅäÖÃÊ±¼äµ±×´Ì¬ÇëÇó·¢ËÍ
									}
								if(CRC8_Table(zd_id, 16)!=DaPtr[23])
								{
									qwl_sendstring("¶ş´ÎĞ£Ñé´íÎó");
									break;
								}
								
								for(i=0;i<16;i++)
								{
									if(AmyID[i]!=DaPtr[i+7])
									{
										break;
									}
								}
								if(i!=16)
								{
									qwl_sendstring("ÓëÍø¹ØID²»Æ¥Åä");
									OUT_AMY_addr_please_set(&DaPtr[7]);//¸½½üIDÇëÇó
									break;
								}
								else
								AMY_addr_please_set(&DaPtr[24]);
		}
			break;
		
				case 0x25://°´Ä¦ÒÎÖ±½ÓµØÖ·²éÑ¯£¬µØÖ·ÉèÖÃÊ±¼ä£¬Íø¹ØÖ±½ÓÆô¶¯£¬Íø¹ØÖ±½Ó²éÑ¯°´Ä¦ÒÎ×´Ì
		{
			u16 i;
			u8 zd_id[16]={0};
											for(i=0;i<16;i++)
									{
										
										zd_id[i]=DaPtr[i+7];//ÅäÖÃÊ±¼äµ±×´Ì¬ÇëÇó·¢ËÍ
									}
								if(CRC8_Table(zd_id, 16)!=DaPtr[23])
								{
									qwl_sendstring("¶ş´ÎĞ£Ñé´íÎó");
									break;
								}
								
								for(i=0;i<16;i++)
								{
									if(AmyID[i]!=DaPtr[i+7])
									{
										break;
									}
								}
								if(i!=16)
								{
									qwl_sendstring("ÓëÍø¹ØID²»Æ¥Åä");
									break;
								}
								else
								AMY_addr_please_set(&DaPtr[24]);
		}
			break;
		case 0x00:
			process_00(&DaPtr[1]);
			break;
		case 0x01:
			qwl_sendstring("VER´íÎó");
			break;
		case 0x04:
			qwl_sendstring("CID2´íÎó");
			break;
		case 0x05:
			qwl_sendstring("ÃüÁî¸ñÊ½´íÎó");
			break;
		case 0x06:
			qwl_sendstring("ÎŞĞ§Êı¾İ");
			break;
		default:
            RTN= 0x05;//ÎŞĞ§ÃüÁî
			PostSendDefault(uf);
            break;
	}
}

//¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù
//                          ³ÌĞò½áÊø(END)
//¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù¡ù


