#include "EIT_SpeedR.h"

PID MotorR_PID;
void MotorRPID_Init(void)
{
    MotorRPID_InitParam();
    MotorRPID_InitState(0);
    MotorRPID_SetSpeed(0);
    PID_InitFbVal(&MotorR_PID,0);
}
void MotorRPID_InitParam(void)
{
	MotorR_PID.Kp = gParam.MotorR_PID_KP;
        MotorR_PID.Ki = gParam.MotorR_PID_KI*gParam.MotorR_PID_Ts;
	MotorR_PID.Kd = -gParam.MotorR_PID_KD/gParam.MotorR_PID_Ts;
	MotorR_PID.MAX_Val = MOTORR_PWM_MAX;
	MotorR_PID.MIN_Val = MOTORR_PWM_MIN;	
        MotorR_PID.spUpRate=(int32)(gParam.MotorR_PID_UpRate*gParam.MotorR_PID_Ts);
        MotorR_PID.spDnRate=(int32)(gParam.MotorR_PID_DnRate*gParam.MotorR_PID_Ts);
}
void MotorRPID_InitState(int32 I)
{
	MotorR_PID.I = I;
        MotorR_PID.spValRamp=0;//£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿
}
void MotorRPID_SetSpeed( int32 spSpeed)
{
	MotorR_PID.spVal = spSpeed;
}
void MotorRPID_SpeedControl(void)
{
	PID_Run_PI(&MotorR_PID);
        //MotorR_PID.outVal=400;
	MotorR_Run(MotorR_PID.outVal);
}

static int32 int_abs(int32 a)
{
   if (a <0)
     return -a;
   else
     return a;
}
int32 SpeedRightNotOK(void)
{
    static int cnt=0;
    
    if(int_abs(gVar.MotorR_Speed)< MIN_R_SPEED && int_abs(MotorR_PID.outVal)> MAX_R_PWM)
       cnt++;
    else
       cnt=0;
    if(cnt >MAX_R_NOT_OK_CN)
    {
       cnt=0;
       return 1;
    }
    else 
       return 0;
}