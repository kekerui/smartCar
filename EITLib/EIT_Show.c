/*
 *******************************************************************************
 *                                EIT Car Project
 *                                Main FUNCTIONS
 *
 *                     (c) Copyright 2015-2020 Car@EIT 
 *                            All Rights Reserved
 * File      : EIT_Show.c
 * This file is part of EIT Car Project
 * Embedded Innovation Team(EIT) - Car@EIT  
 * ----------------------------------------------------------------------------
 * LICENSING TERMS:
 * 
 *     CarLib is provided in source form for FREE evaluation and  educational 
 * use.
 *    If you plan on using  CarLib  in a commercial product you need to contact 
 * Car@EIT to properly license its use in your product. 
 * 
 * ----------------------------------------------------------------------------
 * Change Logs:
 * Date           Author       Notes
 * 2016-08-15     Zexi.Shao    the first version
 *
 *******************************************************************************
 */

#include "EIT_Show.h"

   // Site_t  Site_V            ={74,0};
   // Site_t  Site_VNum         ={94,0};
    Site_t  Site_MaxSpeed     = {0,0};               //参数坐标值
    Site_t  Site_MaxSpeedNum  = {72,0};

    Site_t  Site_MinSpeed     = {0,14};
    Site_t  Site_MinSpeedNum  = {72,14};

    Site_t  Site_KpIn         = {0,28};
    Site_t  Site_KpInNum      = {40,28};

    Site_t  Site_KdIn         = {64,28};
    Site_t  Site_KdInNum      = {104,28};

    Site_t  Site_MDSK         = {0,42};
    Site_t  Site_MDSKNum      = {40,42};
    Site_t  Site_V            ={0,66};/////////////
    Site_t  Site_VNum         ={0,80};/////////////
    Site_t  Site_Kp           = {64,42};
    Site_t  Site_KpNum        = {88,42};
    
    
    Site_t  Site_AutoStop     = {0,96};             //是否开启停车
    Site_t  Site_ENorNOT      = {0,112};
    
    
    Site_t  Site_NearDir      = {4,0};               //偏差坐标值
    Site_t  Site_NearDirNum   = {52,0};
    
    Site_t  Site_MidDir       = {40,14};
    Site_t  Site_MidDirNum    = {80,14};
    
    Site_t  Site_FarDir       = {74,0};
    Site_t  Site_FarDirNum    = {114,0};
    
    
    Size_t  imgsize           = {CAMERA_W, CAMERA_H};//图像大小
    Site_t  Site_A            = {36, 59};            //小图像左上角位置
    Size_t  Size_A;
    Site_t  Site_B            = {0, 32};             //大图像左上角位置
    Size_t  Size_B; 


void LCD_ShowSmallImag(void)
{
    int H;
    Site_t Linesite;
    Size_A.H = 69;                      
    Size_A.W = 92;
    LCD_Img_gray_Z(Site_A, Size_A, (uint8*)Image_Data, imgsize);
    for(H = 0;H < H_END;H = H++)
    {
        Linesite.x = Site_A.x + HBoundL[H] * 92/160;
        Linesite.y = Site_A.y + H * 69/120;
        LCD_point(Linesite, RED);

        Linesite.x = Site_A.x + HBoundR[H] * 92/160;
        Linesite.y = Site_A.y + H * 69/120;
        LCD_point(Linesite, BLUE);

        Linesite.x = Site_A.x + HBoundM[H] * 92/160;
        Linesite.y = Site_A.y + H * 69/120;
        LCD_point(Linesite, GREEN);
    }
    
}
//void LCD_showADC(void)                      //AD转换电源电压
//{
//    uint16 var;
//    uint32 U;      //电源电压
//        var = adc_once(ADC0_SE11, ADC_16bit);
//    // printf("\nADC采样结果为:%d",var);
//    //   printf("     相应电压值为%dmV",(3300*var)/((1<<8)-1));
//    U=(3300*var) * 11/((1<<16)-1) * 1.0821;
//
//    LCD_str(Site_V,"U:",WHITE,BLACK);
//    LCD_num(Site_VNum,U,WHITE,BLACK);
//    
//}

void LCD_ShowBigImag(void)                      
{
    Size_B.H = 96;
    Size_B.W = 128;
    LCD_Img_gray_Z(Site_B, Size_B, (uint8*)Image_Data, imgsize);
}

void LCD_ShowDebug(void)
{   
     LCD_showADC();
  
    LCD_str(Site_MaxSpeed,"MaxSpeed:",WHITE,BLACK);
    LCD_num(Site_MaxSpeedNum,gParam.MaxSpeed,WHITE,BLACK);
    

    LCD_str(Site_MinSpeed,"MinSpeed:",WHITE,BLACK);
    LCD_num(Site_MinSpeedNum,gParam.MinSpeed,WHITE,BLACK);

    LCD_str(Site_KpIn ,"KpIn:",WHITE,BLACK);
    LCD_num(Site_KpInNum,gParam.DIR_KpInAngle*100,WHITE,BLACK);
    
    LCD_str(Site_KdIn,"KdIn:",WHITE,BLACK);
    LCD_num(Site_KdInNum, gParam.DIR_Kd*100,WHITE,BLACK);
    
    LCD_str(Site_MDSK ,"MDSK:",WHITE,BLACK);
    LCD_num(Site_MDSKNum,gParam.MidDirSpeedK*100,WHITE,BLACK);
    
    LCD_str(Site_Kp,"Kp:",WHITE,BLACK);
    LCD_num(Site_KpNum,gParam.DIR_Kp*100,WHITE,BLACK);
    
    LCD_str(Site_AutoStop,"STOP",GREEN,BLACK);
    if(Model == 0)
        LCD_str(Site_ENorNOT,"DIS",GREEN,BLACK);
    else if(Model == 1)
        LCD_str(Site_ENorNOT,"EN",RED,BLACK);
    
    LCD_ShowSmallImag();
}

void LCD_ShowBoundImag(void)
{
    int H;
    Site_t Linesite;
    
    
    if(gDir_Near >= 0)                                      //显示三段 Dir
        LCD_str(Site_NearDir,"Near: ",WHITE,BLACK);
    else
        LCD_str(Site_NearDir,"Near:-",WHITE,BLACK);
    LCD_str(Site_NearDirNum,"    ",WHITE,BLACK);
    LCD_num(Site_NearDirNum,int_abs(gDir_Near),WHITE,BLACK);
    
    if(gDir_Mid >= 0)
        LCD_str(Site_MidDir,"Mid: ",WHITE,BLACK);
    else
        LCD_str(Site_MidDir,"Mid:-",WHITE,BLACK);
    LCD_str(Site_MidDirNum,"    ",WHITE,BLACK);
    LCD_num(Site_MidDirNum,int_abs(gDir_Mid),WHITE,BLACK);
    
    if(gDir_Far >= 0)
        LCD_str(Site_FarDir,"Far: ",WHITE,BLACK);
    else
        LCD_str(Site_FarDir,"Far:-",WHITE,BLACK);
    LCD_str(Site_FarDirNum,"    ",WHITE,BLACK);
    LCD_num(Site_FarDirNum,int_abs(gDir_Far),WHITE,BLACK);
    
    
    LCD_ShowBigImag();                                    //显示图像及边界 中线
    for(H = 0;H < H_END;H = H++)
    {
        Linesite.x = Site_B.x + HBoundL[H] * 4/5;
        Linesite.y = Site_B.y + H * 4/5;
        LCD_point(Linesite, RED);

        Linesite.x = Site_B.x + HBoundR[H] * 4/5;
        Linesite.y = Site_B.y + H * 4/5;
        LCD_point(Linesite, BLUE);

        Linesite.x = Site_B.x + HBoundM[H] * 4/5;
        Linesite.y = Site_B.y + H * 4/5;
        LCD_point(Linesite, GREEN);
    }
}