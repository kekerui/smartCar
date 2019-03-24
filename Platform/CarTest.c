/****************************************************************
*                                                               *
* @filename:CarTest.c                                           *
*                                                               *
* @brief:SmartCar Control Loop                                  *
*                                                               *
* @the library:PID OV7725 Image Processing                      *
*                                                               *
* @date:2016.08.14                                              *
*                                                               *
* @modification time:2016.08.14                                 *
*                                                               *
* @author: Shao Zexi                                            *
*                                                               *
*---------------------------------------------------------------*
*                                                               *
* Model0: Modify Control PD , Graph Show , DataLog              *
* Model1: Modify Control PD , Graph Show , DataLog , Start Line *
* Model2: FullScreen Graph  , LMRBound   , (Near Mid Far) Dir   *
* Model3: FullScreen Graph  , Image2Computer                    *
*                                                               *
*****************************************************************/

#include "CarTest.h"
#include "VCAN_NRF24L0.h"
#include "common.h"

extern int startcheckline;
extern int counttime;
int speed_flag=0;
int32 Model = 0;
void  Car_Test(void)
{
           uint32 i=0;
    uint8 buff[32];
    uint8 *str = "K60";

    Model = GPIO_GET_NBIT(4 ,Model1);  // 3 3
    if(Model >= Model_MAX)
        Model = 0;
    startcheckline=0;
    counttime=0;
    while(1)
    {
        if(ImageOver)
        {
          


  //  printf("\n\n\n***********无线模块NRF24L01+测试************");

  //  while(!nrf_init());                  //初始化NRF24L01+ ,等待初始化成功为止
  //  {
 //       printf("\n  NRF与MCU连接失败，请重新检查接线。\n");
//    }
  //  set_vector_handler(PORTE_VECTORn ,PORTE_IRQHandler);    			//设置 PORTE 的中断复位函数为 PORTE_VECTORn
  //  enable_irq(PORTE_IRQn);

  //  printf("\n      NRF与MCU连接成功！\n");
     //   sprintf((char *)buff,"%s%d",str,i);         //把str和i合并成一个字符串到buff里，再进行发送
   //     nrf_tx(buff,DATA_PACKET);                   //发送一个数据包：buff（包为32字节）

        //等待发送过程中，此处可以加入处理任务


       // while(nrf_tx_state() == NRF_TXING);         //等待发送完成

       
       // if( NRF_TX_OK == nrf_tx_state () )
      //  {
     //       printf("\n发送成功:%d",i);
     //       i++;                                    //发送成功则加1，可验证是否漏包
   //     }
   //     else
     //   {
       //     printf("\n发送失败:%d",i);
       // }
    //    DELAY_MS(10);
    
          
            ImageOver = 0;
            img_extract((uint8 *)Image_Data, (uint8 *)imgbuff0, CAMERA_SIZE);     //解压图像 一位变八位
            switch(Model)
            {
                case 0:                                                         //圆环右拐
                  if(gVar.MotorR_Speed>250)
                    speed_flag=1;
                    if(counttime<250&&speed_flag==1)
                      counttime++;
                    if(counttime==250)
                      startcheckline=1;
                      ControlRouter();                                          
                    if(DataLog_CheckEN())
                        DataLog_Print();
                    if((gVar.MotorL_CntInTs == 0)&&(Key_Check(KEY_EN) == KEYDOWN))
                        RunDebugFlag = 1;
                    //    printf("Cnt = %d, Speed = %d\n", gVar.MotorL_CntInTs, gVar.MotorL_Speed);
                      DataLog_Add();
                    Key_Debug();
                    //LCD_ShowBigImag();
                    //DataLog_Image2Computer();
                    break;
                case 1:                                                         //圆环左拐
                    if(gVar.MotorR_Speed>250)
                    speed_flag=1;
                    if(counttime<250&&speed_flag==1)
                      counttime++;
                    if(counttime==250)
                      startcheckline=1; 
                    ControlRouter1();                                          
                    if(DataLog_CheckEN())
                        DataLog_Print();
                    if((gVar.MotorL_CntInTs == 0)&&(Key_Check(KEY_EN) == KEYDOWN))
                        RunDebugFlag = 1;
                    DataLog_Add();
                    Key_Debug();
                 //   DataLog_Add();
                    break;
                case 2:
                    if(gVar.MotorR_Speed>250)
                    speed_flag=1;
                    if(counttime<250&&speed_flag==1)
                      counttime++;
                    if(counttime==250)
                      startcheckline=1; 
                    ControlRouter2();                                          
                    if(DataLog_CheckEN())
                        DataLog_Print();
                    if((gVar.MotorL_CntInTs == 0)&&(Key_Check(KEY_EN) == KEYDOWN))
                        RunDebugFlag = 1;
                    DataLog_Add();
                    Key_Debug();
                 //   DataLog_Add();
                    break;  
                   
                case 3:
                    if(gVar.MotorR_Speed>250)
                    speed_flag=1;
                    if(counttime<250&&speed_flag==1)
                      counttime++;
                    if(counttime==250)
                      startcheckline=1; 
                    ControlRouter3();                                          
                    if(DataLog_CheckEN())
                        DataLog_Print();
                    if((gVar.MotorL_CntInTs == 0)&&(Key_Check(KEY_EN) == KEYDOWN))
                        RunDebugFlag = 1;
                    DataLog_Add();
                    Key_Debug();
                 //   DataLog_Add();
                    break;  
                
                case 4:
                    if(gVar.MotorR_Speed>250)
                      speed_flag=1;
                    if(counttime<250&&speed_flag==1)
                      counttime++;
                    if(counttime==250)
                      startcheckline=1; 
                    ControlRouter4();                                          
                    if(DataLog_CheckEN())
                        DataLog_Print();
                    if((gVar.MotorL_CntInTs == 0)&&(Key_Check(KEY_EN) == KEYDOWN))
                        RunDebugFlag = 1;
                    DataLog_Add();
                    Key_Debug();
                 //   DataLog_Add();
                    break;  
              
                case 5:
                    if(gVar.MotorR_Speed>250)
                      speed_flag=1;
                    if(counttime<250&&speed_flag==1)
                      counttime++;
                    if(counttime==250)
                      startcheckline=1; 
                    ControlRouter5();                                          
                    if(DataLog_CheckEN())
                        DataLog_Print();
                    if((gVar.MotorL_CntInTs == 0)&&(Key_Check(KEY_EN) == KEYDOWN))
                        RunDebugFlag = 1;
                    DataLog_Add();
                    Key_Debug();
                 //   DataLog_Add();
                    break;  
                    
                default:
                    
                    break;
            }
        }
    }
}