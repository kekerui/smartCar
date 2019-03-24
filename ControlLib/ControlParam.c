#include "ControlParam.h"
EIT_PARAM gParam;
void ControlParam_Init(void)
{
  
        /*Left Motor*/	
	gParam.MotorL_PID_KP = 3.0;	
	gParam.MotorL_PID_KI = 4.0;	
	gParam.MotorL_PID_KD = 0.0; 
	gParam.MotorL_PID_Ts = MOTOR_PID_TS; /*Unit: s   */       
        gParam.MotorL_PID_UpRate = 500;/*ָ�����m/s^2*/                   //1000
        gParam.MotorL_PID_DnRate = -1000;/*ָ�����m/s^2*/                  //-2000
        
	/*Right Motor*/	
	gParam.MotorR_PID_KP=3.0;	
	gParam.MotorR_PID_KI=4.0;
	gParam.MotorR_PID_KD=0.0;
	gParam.MotorR_PID_Ts=MOTOR_PID_TS; /*Unit: s   */     
        gParam.MotorR_PID_UpRate = 500;/*ָ�����m/s^2*/                   //1000                  5000
        gParam.MotorR_PID_DnRate = -1000;/*ָ�����m/s^2*/                  //-2000                 -5000
        
        
        /*Current  Limited*/
        gParam.MaxCurrent =15;                   //������� ��ȥ��
        gParam.MaxCurrentInt=500;
        
        
        /* Speed */
        gParam.MaxSpeed       =  340;    
        gParam.MinSpeed       =  340;    
                        

        /*DIR control PD*/
        gParam.DIR_Kp         =  0.5;        
        gParam.DIR_KpInAngle  =  0.6;      
        gParam.DIR_Kd         =  1.5;
        
        gParam.SteerMid       =  255;    //�����ֵ
        gParam.SteerDeltaMax  =  30;//28;     //�����������    //����������������ת����ʲô����
        gParam.AngleMax       =  30;     //���ת��
        gParam.DIR_Dead       =  4;
        gParam.AngleDeltaMax  =  7;                    //8
        gParam.BackLength     =  7;     //������
        
        
        /*Dir Calculate*/
        gParam.NearStart=10;
        gParam.NearLen  =40;
        
        gParam.MidLen   =40;
        gParam.MidDirSpeedK=0.11;//0.2
        
        gParam.FarStart =180;
        gParam.FarLen   =20;
        
        gParam.InAngle_FarDir=12;               //16                    25
        gParam.OutAngle_FarDir=25;              //֮ǰ��30              20
        gParam.A=1.0;
        gParam.B=1.0;
}



