
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
#include "ControlGraphTask.h"
//int angleLast=0;
int angleLast=0; 
//int angle=0;
int angle=0;
int spSpeedL;
int spSpeedR;
int gDir_NearLast=0;
int gDir_MidLast=0;
int gDir_FarLast=0;

int gDir_MidFilter=0;
int gDir_MidFilterLast=0;
int gDir_MidFilterDiff=0;

int gDir_FarFilter=0;
int gDir_FarFilterLast=0;
int gDir_FarFilterDiff=0;

int gDir_FarFilterSlow=0;
int gDir_FarFilterSlowLast=0;
int gDir_FarFilterSlowDiff=0;

int flag=0;
int delay=0;
int startcheckline;
int counttime;


void GetSetPointMaxSpeed(void);
void GetSetPointMaxSpeed1(void);
void gDir_Filter(void);
void gDir_Filter2(void);
void SteerDirControl(void);
void SteerDirControl2(void);
void ControlRouter(void)
{
    Graph_FindMidLine();
    Graph_Calculate_Dir(gVar.Car_Speed);  
    gDir_Filter();
    SteerDirControl();
    GetSetPointMaxSpeed();
}
void ControlRouter1(void)
{
    Graph_FindMidLine1();
    Graph_Calculate_Dir(gVar.Car_Speed);  
    gDir_Filter();
    SteerDirControl();
    GetSetPointMaxSpeed();
}

void ControlRouter2(void)
{
    Graph_FindMidLine();
    Graph_Calculate_Dir(gVar.Car_Speed);  
    gDir_Filter();
    SteerDirControl();
    GetSetPointMaxSpeed1();
}

void ControlRouter3(void)
{
    Graph_FindMidLine1();
    Graph_Calculate_Dir(gVar.Car_Speed);  
    gDir_Filter();
    SteerDirControl();
    GetSetPointMaxSpeed1();
}

void ControlRouter4(void)
{
    Graph_FindMidLine2();
    Graph_Calculate_Dir(gVar.Car_Speed);  
    gDir_Filter();
    SteerDirControl();
    GetSetPointMaxSpeed();
}

void ControlRouter5(void)
{
    Graph_FindMidLine3();
    Graph_Calculate_Dir(gVar.Car_Speed);  
    gDir_Filter();
    SteerDirControl();
    GetSetPointMaxSpeed();
}

static int32 int_abs(int32 d)
{
   if (d<0)
      return -d;
   else
     return d;
}
static int32 int_delta_Limit(int32 x,int32 xLast,int32 deltaMax)
{
    if( x - xLast > deltaMax)
       x=xLast+deltaMax;
    else if(  xLast-x > deltaMax)
       x=xLast-deltaMax;
    
    return x;
}

void GetSetPointMaxSpeed(void)
{
    int MidSpeed;

    if( gVar.InAngle/*&&(!S)*/)
        MidSpeed = gParam.MinSpeed;
    else
        MidSpeed =gParam.MaxSpeed;
    if(Circle==1)
        MidSpeed = 280;
    spSpeedL =CarSpeed2LSpeed(MidSpeed,angle);;
    spSpeedR =CarSpeed2RSpeed(MidSpeed,angle);
    
    if(/*Graph_JudgeOut() || */(judge_startline()==1&&startcheckline==1))
    {
      flag=1;
    }
    if(flag==1)
      delay++;
    if(delay>=15)
    {
      MotorRPID_SetSpeed(0);
      MotorLPID_SetSpeed(0);//加的
      delay=15;
    }
    else
    {
      MotorRPID_SetSpeed(spSpeedR);
      MotorLPID_SetSpeed(spSpeedL);
    }
   // MotorLPID_SetSpeed(spSpeedL);
   // MotorRPID_SetSpeed(spSpeedR);
    /*MotorLPID_SetSpeed(MidSpeed);
    MotorRPID_SetSpeed(MidSpeed);*/
}

void GetSetPointMaxSpeed1(void)
{
    int MidSpeed;

    if( gVar.InAngle/*&&(!S)*/)
        MidSpeed = gParam.MinSpeed;
    else
        MidSpeed =gParam.MaxSpeed;
    if(Circle==1)
        MidSpeed = 200;
    spSpeedL =CarSpeed2LSpeed(MidSpeed,angle);;
    spSpeedR =CarSpeed2RSpeed(MidSpeed,angle);
    
    if(/*Graph_JudgeOut() || */(judge_startline()==1&&startcheckline==1))
    {
      flag=1;
    }
    if(flag==1)
      delay++;
    if(delay>=15)
    {
      MotorRPID_SetSpeed(0);
      MotorLPID_SetSpeed(0);//加的
      delay=15;
    }
    else
    {
      MotorRPID_SetSpeed(spSpeedR);
      MotorLPID_SetSpeed(spSpeedL);
    }
   // MotorLPID_SetSpeed(spSpeedL);
   // MotorRPID_SetSpeed(spSpeedR);
    /*MotorLPID_SetSpeed(MidSpeed);
    MotorRPID_SetSpeed(MidSpeed);*/
}


void SteerDirControl(void)
{   
    int MidDir;
    
    MidDir=gDir_Mid;
    if(int_abs(MidDir)>=gParam.DIR_Dead)
    {
        if(MidDir>0)
           MidDir-=gParam.DIR_Dead;
        else
           MidDir+=gParam.DIR_Dead;
    }
    else
    {
        MidDir=0;
    }
//    if(Circle)
//        angle =-(int32)((float)(MidDir)*1.0);
    if(gVar.InAngle)
        angle =-(int32)((float)(MidDir)*gParam.DIR_KpInAngle+ (float)(gDir_MidFilterDiff)*gParam.DIR_Kd);
    else
        angle =-(int32)((float)(MidDir)*gParam.DIR_Kp+ (float)(gDir_MidFilterDiff)*gParam.DIR_Kd);
//    if(S==1)            //S路
//        angle=0;
    if (angle >gParam.AngleMax)
       angle =gParam.AngleMax;
    else if (angle <-gParam.AngleMax)
       angle =-gParam.AngleMax;
//    if(!Circle)
    angle=(double)int_delta_Limit(angle,angleLast, gParam.AngleDeltaMax);
    angleLast=angle;
    
    
  Steer_Run(gParam.SteerMid,angle*gParam.SteerDeltaMax/gParam.AngleMax);
    //Steer_Run(gParam.SteerMid,0*gParam.SteerDeltaMax/gParam.AngleMax);
    
}

void SteerDirControl2(void)
{   
    int MidDir;
    
    MidDir=gDir_Mid;
    if(int_abs(MidDir)>=gParam.DIR_Dead)
    {
        if(MidDir>0)
           MidDir-=gParam.DIR_Dead;
        else
           MidDir+=gParam.DIR_Dead;
    }
    else
    {
        MidDir=0;
    }
    
    if(gVar.InAngle)
        angle =-(int32)((float)(MidDir)*gParam.DIR_KpInAngle+ (float)(gDir_MidFilterDiff)*gParam.DIR_Kd);
    else
        angle =-(int32)((float)(MidDir)*gParam.DIR_Kp+ (float)(gDir_MidFilterDiff)*gParam.DIR_Kd);
    if(S==1)            //S路
        angle=0;
    if (angle >gParam.AngleMax)
       angle =gParam.AngleMax;
    else if (angle <-gParam.AngleMax)
       angle =-gParam.AngleMax;
    
    angle=(double)int_delta_Limit(angle,angleLast, gParam.AngleDeltaMax);
    angleLast=angle;
    
    
  Steer_Run(gParam.SteerMid,angle*gParam.SteerDeltaMax/gParam.AngleMax);
    //Steer_Run(gParam.SteerMid,0*gParam.SteerDeltaMax/gParam.AngleMax);
    
}


void gDir_Filter(void)
{
   static int MidDir[5];
   static int FarDir[15];
   
   MidDir[4]=MidDir[3];
   MidDir[3]=MidDir[2];
   MidDir[2]=MidDir[1];
   MidDir[1]=MidDir[0];
   MidDir[0]=gDir_Mid;
   
   gDir_MidFilterLast=gDir_MidFilter;
   gDir_MidFilter=(MidDir[0]+MidDir[1]+MidDir[2]+MidDir[3]+MidDir[4])/5;
   gDir_MidFilterDiff=gDir_MidFilter-gDir_MidFilterLast;
   

   FarDir[9]=FarDir[8];
   FarDir[8]=FarDir[7];
   FarDir[7]=FarDir[6];
   FarDir[6]=FarDir[5];
   FarDir[5]=FarDir[4];
   FarDir[4]=FarDir[3];
   FarDir[3]=FarDir[2];
   FarDir[2]=FarDir[1];
   FarDir[1]=FarDir[0];
   FarDir[0]=gDir_Far;
   
   gDir_FarFilterLast=gDir_FarFilter;
   gDir_FarFilter=(FarDir[0]+FarDir[1]+FarDir[2]+FarDir[3]+FarDir[4])/5;
   gDir_FarFilterDiff=gDir_FarFilter-gDir_FarFilterLast;
   
   gDir_FarFilterSlowLast=gDir_FarFilterSlow;
   gDir_FarFilterSlow=gDir_FarFilter/2+(FarDir[9]+FarDir[8]+FarDir[7]+FarDir[6]+FarDir[5])/10;
   gDir_FarFilterSlowDiff=gDir_FarFilterSlow-gDir_FarFilterSlowLast;
   
   switch(gVar.InAngle)
   {
       case 0:
          if(gDir_FarFilterDiff>0 && gDir_Far>gParam.InAngle_FarDir )
             gVar.InAngle=1;
          if(gDir_FarFilterDiff<0 && gDir_Far<-gParam.InAngle_FarDir)
             gVar.InAngle=1;
       break;
       case 1:
          if(gDir_FarFilterDiff<0 && gDir_FarFilter<gParam.OutAngle_FarDir && gDir_FarFilter>0)
             gVar.InAngle=0;
          if(gDir_FarFilterDiff>0 && gDir_FarFilter>-gParam.OutAngle_FarDir && gDir_FarFilter<0)
             gVar.InAngle=0;
     break;
   }
//   if(num==1)
//      gVar.InAngle=1;
   
}

void gDir_Filter2(void)
{
   static int MidDir[5];
   static int FarDir[15];
   
   MidDir[4]=MidDir[3];
   MidDir[3]=MidDir[2];
   MidDir[2]=MidDir[1];
   MidDir[1]=MidDir[0];
   MidDir[0]=gDir_Mid;
   
   gDir_MidFilterLast=gDir_MidFilter;
   gDir_MidFilter=(MidDir[0]+MidDir[1]+MidDir[2]+MidDir[3]+MidDir[4])/5;
   gDir_MidFilterDiff=gDir_MidFilter-gDir_MidFilterLast;
   

   FarDir[9]=FarDir[8];
   FarDir[8]=FarDir[7];
   FarDir[7]=FarDir[6];
   FarDir[6]=FarDir[5];
   FarDir[5]=FarDir[4];
   FarDir[4]=FarDir[3];
   FarDir[3]=FarDir[2];
   FarDir[2]=FarDir[1];
   FarDir[1]=FarDir[0];
   FarDir[0]=gDir_Far;
   
   gDir_FarFilterLast=gDir_FarFilter;
   gDir_FarFilter=(FarDir[0]+FarDir[1]+FarDir[2]+FarDir[3]+FarDir[4])/5;
   gDir_FarFilterDiff=gDir_FarFilter-gDir_FarFilterLast;
   
   gDir_FarFilterSlowLast=gDir_FarFilterSlow;
   gDir_FarFilterSlow=gDir_FarFilter/2+(FarDir[9]+FarDir[8]+FarDir[7]+FarDir[6]+FarDir[5])/10;
   gDir_FarFilterSlowDiff=gDir_FarFilterSlow-gDir_FarFilterSlowLast;
   
   switch(gVar.InAngle)
   {
       case 0:
          if(gDir_FarFilterDiff>0 && gDir_Far>gParam.InAngle_FarDir )
             gVar.InAngle=1;
          if(gDir_FarFilterDiff<0 && gDir_Far<-gParam.InAngle_FarDir)
             gVar.InAngle=1;
       break;
       case 1:
          if(gDir_FarFilterDiff<0 && gDir_FarFilter<gParam.OutAngle_FarDir && gDir_FarFilter>0)
             gVar.InAngle=0;
          if(gDir_FarFilterDiff>0 && gDir_FarFilter>-gParam.OutAngle_FarDir && gDir_FarFilter<0)
             gVar.InAngle=0;
      break;
   } 
}