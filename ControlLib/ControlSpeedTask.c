/*
 *******************************************************************************
 *                                EIT CAM_WH4_MO2
 *                             lib FUNCTIONS
 *
 *                     (c) Copyright 2015-2020 Car@EIT 
 *                            All Rights Reserved
 * File      : EIT_Speed.c
 * This file is part of EIT Car Project
 * Embedded Innovation Team(EIT) - Car@EIT  
 * ----------------------------------------------------------------------------
 * LICENSING TERMS:
 * 
 *     CAM_WH4_MO2 is provided in source form for FREE evaluation and  educational 
 * use.
 *    If you plan on using  EITLib  in a commercial product you need to contact 
 * Car@EIT to properly license its use in your product. 
 * 
 * ----------------------------------------------------------------------------
 * Change Logs:
 * Date           Author       Notes
 * 2015-02-07     Xian.Chen    the first version
 *
 *******************************************************************************
 */
#include "include.h"

double RealDistance = 0;

static int32 int_abs(int32 d)
{
   if (d<0)
      return -d;
   else
     return d;
}

static double double_abs(double d)
{
   if (d<-0.00000001)
      return -d;
   else
     return d;
}

int32 CarSpeed2LSpeed(int32 CarSpeed,int angle)
{
  int32 LSpeed;
  int R;
    
  
  if(angle >= 0)//角度为正向左转
    R = (int)((-13.43 * angle + 1316) / (angle + 3.621));
  else
    R = (int)((-14.53 * angle - 1240) / (angle + 0.3417));
  if(angle>1)
  {
     LSpeed=CarSpeed*(R)/(R+gParam.BackLength/2);
    
  }
  else if(angle<-1)
  {
     LSpeed=CarSpeed*(R+gParam.BackLength)/(R+gParam.BackLength/2);
  }
  else
  {
     LSpeed=CarSpeed;
  }
  return LSpeed;
}
int32 CarSpeed2RSpeed(int32 CarSpeed,int angle)
{
  int32 RSpeed;
  int R;
    
  
  if(angle >= 0)
    R = (int)((-13.43 * angle + 1316) / (angle + 3.621));
  else
    R = (int)((-14.53 * angle - 1240) / (angle + 0.3417));
  if(angle>1)
  {
     RSpeed=CarSpeed*(R+gParam.BackLength)/(R+gParam.BackLength/2);
  }
  else if(angle<-1)
  {
     RSpeed=CarSpeed*(R)/(R+gParam.BackLength/2);
  }
  else
  {
     RSpeed=CarSpeed;
  }
  return RSpeed;
}

int32 My_CarSpeed2LSpeed(int32 CarSpeed,int angle)
{
    int32 LSpeed;
    int R;
    int L=20;              //前后轮的距离为19.8 cm
    double q;   //弧度
   // double A=1,B=1;
    gParam.A=1;
    gParam.B=1;
    q=(angle/180.0);
  //  R=(int)(L/tan(q));     //R为阿克曼转向模型中的前两轮中心与交点的连线的长度
    if(angle>1)
    {
        R=(int)(L/tan(q));     //R为阿克曼转向模型中的前两轮中心与交点的连线的长度
        LSpeed=gParam.A*CarSpeed*(gParam.B*R-gParam.BackLength/2)/R;
    }
    else if(angle<-1)
    {
        R=(int)(L/tan(-q));     //R为阿克曼转向模型中的前两轮中心与交点的连线的长度
        LSpeed=gParam.A*CarSpeed*(gParam.B*R+gParam.BackLength/2)/R;
    }
    else
    {
        LSpeed=CarSpeed;
    }
    return LSpeed;
}

int32 My_CarSpeed2RSpeed(int32 CarSpeed,int  angle)
{
    int32 RSpeed;
    int R;
    int L=20;              //前后轮的距离为19.8 cm
    double q;   //弧度
   // double A=1,B=1;
   // gParam.A=1;
    //gParam.B=1;
    q=(angle/180.0);
   // R=(int)(L/tan(q));     //R为阿克曼转向模型中的前两轮中心与交点的连线的长度
    if(angle>1)
    {
        R=(int)(L/tan(q));     //R为阿克曼转向模型中的前两轮中心与交点的连线的长度
        RSpeed=gParam.A*CarSpeed*(gParam.B*R+gParam.BackLength/2)/R;
    }
    else if(angle<-1)
    {
        R=(int)(L/tan(-q));     //R为阿克曼转向模型中的前两轮中心与交点的连线的长度
        RSpeed=gParam.A*CarSpeed*(gParam.B*R-gParam.BackLength/2)/R;
    }
    else
    {
        RSpeed=CarSpeed;
    }
    return RSpeed;
}


int CarSpeedIsBad(void)
{
    if( SpeedLeftNotOK() && SpeedRightNotOK())
      return 1;
    else 
      return 0;
}

void SpeedControlTask(void)
{
/*
      gVar.MotorL_CntInTs = MotorL_GetTsCount();
      gVar.MotorL_Speed   = MotorL_GetWheelSpeed(gVar.MotorL_CntInTs);  
      gVar.MotorR_CntInTs = MotorR_GetTsCount();
      gVar.MotorR_Speed   = MotorR_GetWheelSpeed(gVar.MotorR_CntInTs); 
      
      
    PID_SetFbVal(&MotorL_PID,gVar.MotorL_Speed);
      
            gVar.MotorL_Current=(MotorL_PID.outVal-MotorL_PID.fbValFilter/2)/30;
      
      if(MotorL_PID.fbValFilter<50 && MotorL_PID.fbValFilter>=-50 )     //限流，防止电机堵转时电流过大烧电机
      {
          gParam.MaxCurrent=15;
          MotorL_PID.I=0;
      }
      else
      {
          gParam.MaxCurrent=30;
      }
      MotorL_PID.MAX_Val=(MotorL_PID.fbValFilter/2+gParam.MaxCurrent*30);
      if(MotorL_PID.MAX_Val>1000)
         MotorL_PID.MAX_Val=1000;
      
    MotorLPID_SpeedControl();
    
    
      PID_SetFbVal(&MotorR_PID,gVar.MotorR_Speed);
      gVar.MotorR_Current=(MotorR_PID.outVal-MotorR_PID.fbValFilter/2)/30;
      
      if(MotorR_PID.fbValFilter<50 && MotorR_PID.fbValFilter>=-50 )   
      {
          gParam.MaxCurrent=15;
          MotorR_PID.I=0;
      }
      else
      {
          gParam.MaxCurrent=30;
      }
      MotorR_PID.MAX_Val=(MotorR_PID.fbValFilter/2+gParam.MaxCurrent*30);
      if(MotorR_PID.MAX_Val>1000)
         MotorR_PID.MAX_Val=1000;
      
            MotorRPID_SpeedControl();
      gVar.Car_Speed=GetCarSpeed(MotorR_PID.fbValFilter,MotorL_PID.fbValFilter);

      //MotorR_Run(350);  
  */
  
      gVar.MotorL_CntInTs = MotorL_GetTsCount();
      gVar.MotorL_Speed   = MotorL_GetWheelSpeed(gVar.MotorL_CntInTs);
      //MotorL_PID.MAX_Val=(gVar.MotorL_Speed+gParam.MaxCurrent*80);

      gVar.MotorR_CntInTs = MotorR_GetTsCount();
      gVar.MotorR_Speed   = MotorR_GetWheelSpeed(gVar.MotorR_CntInTs);
      //MotorR_PID.MAX_Val=(gVar.MotorR_Speed+gParam.MaxCurrent*80);
      

      PID_SetFbVal(&MotorL_PID,gVar.MotorL_Speed);
      gVar.MotorL_Current=(MotorL_PID.outVal-MotorL_PID.fbValFilter/2)/30;      
      if(MotorL_PID.fbValFilter<30 && MotorL_PID.fbValFilter>=-30 )     //限流，防止电机堵转时电流过大烧电机
      {
          gParam.MaxCurrent=15;
          MotorL_PID.I=0;
      }
      else
      {
          gParam.MaxCurrent=30;
      }
      MotorL_PID.MAX_Val=(MotorL_PID.fbValFilter/2+gParam.MaxCurrent*30);
      if(MotorL_PID.MAX_Val>1000)
         MotorL_PID.MAX_Val=1000;
      MotorLPID_SpeedControl();
      
      
      PID_SetFbVal(&MotorR_PID,gVar.MotorR_Speed);
      gVar.MotorR_Current=(MotorR_PID.outVal-MotorR_PID.fbValFilter/2)/30;    
      if(MotorR_PID.fbValFilter<30 && MotorR_PID.fbValFilter>=-30 )   
      {
          gParam.MaxCurrent=15;
          MotorR_PID.I=0;
      }
      else
      {
          gParam.MaxCurrent=30;
      }
      MotorR_PID.MAX_Val=(MotorR_PID.fbValFilter/2+gParam.MaxCurrent*30);
      if(MotorR_PID.MAX_Val>1000)
         MotorR_PID.MAX_Val=1000;
      
      MotorRPID_SpeedControl();
      
      
      gVar.Car_Speed =(MotorR_PID.fbValFilter + MotorL_PID.fbValFilter)/2;
 /*     MotorL_Run(100);
      MotorR_Run(100);*/
}

void  DistanceControlTask(void)
{
    int CarCntInTs = 0;
    
    CarCntInTs = (gVar.MotorL_CntInTs + gVar.MotorR_CntInTs)/2;
    RealDistance += 100 * CarCntInTs * ENCODL_GEAR_N * WHEELL_LENGTH/WHEELL_GEAR_N/ENCODL_CYCLE;
    if((RealDistance/100) >= 400)
        EnBuzzer();
}