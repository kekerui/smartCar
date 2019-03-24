/*
 *******************************************************************************
 *                                EIT Car Project
 *                             Main FUNCTIONS
 *
 *                     (c) Copyright 2015-2020 Car@EIT 
 *                            All Rights Reserved
 * File      : EIT_MotorL.c
 * This file is part of EIT Car Project
 * Embedded Innovation Team(EIT) - Car@EIT  
 * ----------------------------------------------------------------------------
 * LICENSING TERMS:
 * 
 *     CarLib is provided in source form for FREE evaluation and  educational 
 * use.
 *    If you plan on using  EITLib  in a commercial product you need to contact 
 * Car@EIT to properly license its use in your product. 
 * 
 * ----------------------------------------------------------------------------
 * Change Logs:
 * Date           Author       Notes
 * 2015-02-08     Xian.Chen    the first version
 *
 *******************************************************************************
 */

#include "EIT_MotorL.h"
static volatile int32 cnt=0;

void MotorL_Init(void)
{
   /*Motor Drive*/
   gpio_init (MOTORL_EN, GPO,0);
   //port_init(MOTORL_PWMAIO, PULLUP );
   //port_init(MOTORL_PWMBIO, PULLUP );
   FTM_PWM_init(MOTORL_FTM, MOTORL_PWMA, MOTORL_PWM_FREQ,0);
   FTM_PWM_init(MOTORL_FTM, MOTORL_PWMB, MOTORL_PWM_FREQ,0);
   
   /*Speed Measure*/
   FTM_QUAD_Init(MOTORL_ENCODE_FTM );
}

void MotorL_Run(int32 pwm)
{
   uint32 PWM_A =0;
   uint32 PWM_B =0;
   
   if(pwm>MOTORL_PWM_MAX)
       pwm=MOTORL_PWM_MAX;
   else if(pwm<MOTORL_PWM_MIN)
       pwm=MOTORL_PWM_MIN;
   
   if ( pwm >0 )
   {
      PWM_A = pwm;
   }
   else if ( pwm <0 )
   {
      PWM_B = -pwm;
   }
      
   FTM_PWM_Duty(MOTORL_FTM, MOTORL_PWMA,PWM_A);
   FTM_PWM_Duty(MOTORL_FTM, MOTORL_PWMB,PWM_B);
   gpio_set  ( MOTORL_EN,1);
}
void MotorL_Brake(void)
{   
   FTM_PWM_Duty(MOTORL_FTM, MOTORL_PWMA,0);
   FTM_PWM_Duty(MOTORL_FTM, MOTORL_PWMB,0);
   gpio_set  ( MOTORL_EN,1);
}
void MotorL_Slip(void)
{
   gpio_set  (MOTORL_EN,0);
   FTM_PWM_Duty(MOTORL_FTM, MOTORL_PWMA,0);
   FTM_PWM_Duty(MOTORL_FTM, MOTORL_PWMB,0);
}
int32 MotorL_GetWheelSpeed(int32 CntInTs)
{
      int32 speed=0;
      speed = (int32)(CntInTs*ENCODL_GEAR_N*WHEELL_LENGTH*SPEEDL_FS/ENCODL_CYCLE/WHEELL_GEAR_N);
      return speed;
}

int32 MotorL_GetTsCount(void)
{
      int32 val;
      val = -FTM_QUAD_get(MOTORL_ENCODE_FTM);
      FTM_QUAD_clean(MOTORL_ENCODE_FTM);
      return val;
}

