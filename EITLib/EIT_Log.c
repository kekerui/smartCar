/*
 *******************************************************************************
 *                                EIT Car Project
 *                             Main FUNCTIONS
 *
 *                     (c) Copyright 2015-2020 Car@EIT 
 *                            All Rights Reserved
 * File      : EIT_Log.c
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
 * 2015-01-22    MENG.ZHAO   the first version
 * 2016-08-16    Zexi.Shao   Modify DataLogIn
 *
 *******************************************************************************
 */

#include  "EIT_Log.h"             //header用户应用程序

static int32 DataLog[LOG_COUNT][LOG_NUM_EACH];
static volatile int32 cnt = 0;
extern int32 gDir_Mid;

void DataLog_Init(void)
{
    gpio_init(LOG_EN, GPI, 0);
    port_init_NoALT(LOG_EN, PULLUP);
    cnt=0;  
}

int32 DataLog_CheckEN(void)
{
    static uint8 DataLogInLast = 1;
    int32 ret = 0;
    uint8 DataLogIn = gpio_get(LOG_EN);
    if(DataLogIn == 0 && DataLogInLast == 1)
        ret = 1;
    else
        ret = 0;
    DataLogInLast = DataLogIn;
    return ret;
}

void DataLog_Add(void)
{
    if(cnt >= LOG_COUNT)
        return;
    else
    {
//        /*eg:
//        DataLog[cnt][0]=gVar.time;
//        DataLog[cnt][1]=gVar.Car_Speed;
//        DataLog[cnt][2]=MotorR_PID.outVal;
         // DataLog[cnt][3]=gVar.MotorR_CntInTs ;
//        DataLog[cnt][4]=MotorR_PID.fbValFilter;
        //DataLog[cnt][0]=gDir_MidFilter;
        //DataLog[cnt][1]=angle;
        //DataLog[cnt][2]=gVar.InAngle*100;
        //DataLog[cnt][3]=gDir_FarFilter;
        DataLog[cnt][0]=MotorR_PID.fbValFilter;
        DataLog[cnt][1]=MotorR_PID.spValRamp;
        DataLog[cnt][2]=MotorL_PID.fbValFilter;;
        DataLog[cnt][3]=MotorL_PID.spValRamp;
        DataLog[cnt][4]=gDir_Mid;
        
//        DataLog[cnt][2]=gDir_FarFilter;
//        DataLog[cnt][3]=MotorR_PID.outVal;
//        DataLog[cnt][4]=MotorR_PID.outValFilterDiff;
//        DataLog[cnt][5]=MotorR_PID.spVal;
//        DataLog[cnt][6]=MotorR_PID.fbValFilter;
//        DataLog[cnt][7]=MotorR_PID.I;
//        DataLog[cnt][8]=MotorR_PID.P;
//        DataLog[cnt][0]=gDir_FarFilter;
//        DataLog[cnt][1]=gDir_MidFilter;
//        DataLog[cnt][2]=MotorR_PID.spValRamp;
//        DataLog[cnt][3]=MotorR_PID.fbValFilter;
//        DataLog[cnt][4]=MotorR_PID.outVal;
//        DataLog[cnt][5]=MotorR_PID.spVal;
//        DataLog[cnt][0]=gVar.time;
//        DataLog[cnt][1]=gVar.Car_Speed;
//        DataLog[cnt][2]=MotorR_PID.outVal;
//        DataLog[cnt][3]=gVar.MotorR_CntInTs ;
        cnt++;
    }
}

void DataLog_Print(void)
{
    int i,j;
    for(i = 0;i < LOG_COUNT;i++)
    {
        for(j = 0;j < LOG_NUM_EACH;j++)
            printf("%d ",DataLog[i][j]);
        printf("\n");
    }
}

void DataLog_Image2Computer(void)
{
    int H,W;
    for(H = 0;H < CAMERA_H;H++)
    {
        for(W = 0;W<CAMERA_W;W++)
        {
            printf("%d ",Image_Data[H][W]);
        }
        printf("\n");
    }
    printf("OK\n");
}