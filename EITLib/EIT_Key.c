/*
 *******************************************************************************
 *                                EIT Car Project
 *                                Main FUNCTIONS
 *
 *                     (c) Copyright 2015-2020 Car@EIT 
 *                            All Rights Reserved
 * File      : EIT_Key.c
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

#include "EIT_Key.h"
//#include "EIT_Buzzer.h"

PTXn_e EitKey_PTxn[KEYMAX] = {PTD4,PTD2,PTD5,PTD6,PTD3};         //进入（中） 下一个（右） 加（左） 减（下） 退出（上）

uint8 RunDebugFlag = 0;
int  tab = 1;


void Key_Init(void)
{
    int i;
    for(i = 0;i < KEYMAX;i++)
    {
        gpio_init(EitKey_PTxn[i], GPI, 0);
        port_init_NoALT(EitKey_PTxn[i], PULLUP);
    }
}

EitKeyStatus Key_Check(EitKey key)
{
    static uint8 DebugInLast = 1;    
    EitKeyStatus ret;
    uint8 DebugIn;
    
    DebugIn = gpio_get(EitKey_PTxn[key]);
    if(DebugIn == 0)
    { 
        systick_delay_ms(70);
        DebugIn = gpio_get(EitKey_PTxn[key]);
    }
    if(DebugIn == 0 && DebugInLast == 1)
        ret = KEYDOWN;
    else
        ret = KEYUP;
    DebugInLast = DebugIn;
    return ret;
}

void Key_Debug(void)
{
    static uint8 FirstEnter = 1;
    if(RunDebugFlag)
    {
        if(FirstEnter)
        {
            LCD_ShowDebug();
            FirstEnter = 0;
        }
        else
            LCD_ShowSmallImag();
        gVar.time = 0;
        if(Key_Check(KEY_ACK) == KEYDOWN)                      //有问题是因为多个按键复用一个last
        {
            tab ++;
            if(tab > TAB)
              tab = 1;
            LCD_ShowDebug();
        }
        else if(Key_Check(KEY_OUT) == KEYDOWN)
        {
            RunDebugFlag = 0;
            FirstEnter = 1;
            LCD_init();
            return;
        }
        else switch(tab)
        {
            case 1:
                LCD_str(Site_Kp,"Kp:",WHITE,BLACK);
                LCD_str(Site_MaxSpeed,"MaxSpeed:",WHITE,BLUE);
                if(Key_Check(KEY_INC) == KEYDOWN)
                {
                    gParam.MaxSpeed += 10;
                    if(gParam.MaxSpeed >= 1000)
                        gParam.MaxSpeed = 1000;
                }
                if(Key_Check(KEY_DEC) == KEYDOWN)
                {
                    gParam.MaxSpeed -= 10;
                    if(gParam.MaxSpeed <=0)
                        gParam.MaxSpeed = 0;
                }
                LCD_str(Site_MaxSpeedNum,"    ",BLACK,BLACK);
                LCD_num(Site_MaxSpeedNum,gParam.MaxSpeed,WHITE,BLACK);
                break;
            case 2:
                LCD_str(Site_MaxSpeed,"MaxSpeed:",WHITE,BLACK);
                LCD_str(Site_MinSpeed,"MinSpeed:",WHITE,BLUE);
                if(Key_Check(KEY_INC) == KEYDOWN)
                {
                    gParam.MinSpeed += 10;
                    if(gParam.MinSpeed >= 1000)
                        gParam.MinSpeed = 1000;
                }
                if(Key_Check(KEY_DEC) == KEYDOWN)
                {
                    gParam.MinSpeed -= 10;
                    if(gParam.MinSpeed <=0)
                        gParam.MinSpeed = 0;
                }
                LCD_str(Site_MinSpeedNum,"    ",BLACK,BLACK);
                LCD_num(Site_MinSpeedNum,gParam.MinSpeed,WHITE,BLACK);
                break;
            case 3:
                LCD_str(Site_MinSpeed,"MinSpeed:",WHITE,BLACK);
                LCD_str(Site_KpIn,"KpIn:",WHITE,BLUE);
                if(Key_Check(KEY_INC) == KEYDOWN)
                    gParam.DIR_KpInAngle += 0.01;
                if(Key_Check(KEY_DEC) == KEYDOWN)
                    gParam.DIR_KpInAngle -= 0.01;
                LCD_str(Site_KpInNum,"   ",BLACK,BLACK);
                LCD_num(Site_KpInNum,gParam.DIR_KpInAngle * 100,WHITE,BLACK);
                break;
            case 4:
                LCD_str(Site_KpIn,"KpIn:",WHITE,BLACK);
                LCD_str(Site_KdIn,"KdIn:",WHITE,BLUE);
                if(Key_Check(KEY_INC) == KEYDOWN)
                    gParam.DIR_Kd += 0.01;
                if(Key_Check(KEY_DEC) == KEYDOWN)
                    gParam.DIR_Kd -= 0.01;
                LCD_str(Site_KdInNum,"   ",BLACK,BLACK);
                LCD_num(Site_KdInNum,gParam.DIR_Kd * 100,WHITE,BLACK);
                break;
            case 5:
                LCD_str(Site_KdIn,"KdIn:",WHITE,BLACK);
                LCD_str(Site_MDSK,"MDSK:",WHITE,BLUE);
                if(Key_Check(KEY_INC) == KEYDOWN)
                    gParam.MidDirSpeedK += 0.01;
                if(Key_Check(KEY_DEC) == KEYDOWN)
                    gParam.MidDirSpeedK -= 0.01;
                LCD_str(Site_MDSKNum,"   ",BLACK,BLACK);
                LCD_num(Site_MDSKNum,gParam.MidDirSpeedK * 100,WHITE,BLACK);
                break;
            case 6:
                LCD_str(Site_MDSK,"MDSK:",WHITE,BLACK);
                LCD_str(Site_Kp,"Kp:",WHITE,BLUE);
                if(Key_Check(KEY_INC) == KEYDOWN)
                    gParam.DIR_Kp += 0.01; 
                if(Key_Check(KEY_DEC) == KEYDOWN)
                    gParam.DIR_Kp -= 0.01; 
                LCD_str(Site_KpNum,"   ",BLACK,BLACK);
                LCD_num(Site_KpNum,gParam.DIR_Kp * 100,WHITE,BLACK);
                break;
            default:
                break;
        }
    }
    else
    {
        RunDebugFlag = 0;
        return;
    }
}