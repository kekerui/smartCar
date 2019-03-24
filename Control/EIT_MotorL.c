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
   port_init(MOTORL_PWMAIO, PULLUP );
   port_init(MOTORL_PWMBIO, PULLUP );
   tpm_pwm_init(MOTORL_FTM, MOTORL_PWMA, MOTORL_PWM_FREQ,0);
   tpm_pwm_init(MOTORL_FTM, MOTORL_PWMB, MOTORL_PWM_FREQ,0);
   
   /*Speed Measure*/
   gpio_init (PTA1, GPI,0);
   port_init (MOTORL_SPD_CLK, IRQ_FALLING | PF | ALT1 | PULLUP );//spd_CLK1 A10
   gpio_init (MOTORL_SPD_DIR, GPI,0);							//正反转测量DIR_M1
}

void MotorL_Run(int32 pwm)
{
   uint32 PWM_A =0;
   uint32 PWM_B =0;
   if ( pwm >0 )
   {
      PWM_B = pwm;
   }
   else if ( pwm <0 )
   {
      PWM_A = -pwm;
   }
      
   tpm_pwm_duty(MOTORL_FTM, MOTORL_PWMA,PWM_A);
   tpm_pwm_duty(MOTORL_FTM, MOTORL_PWMB,PWM_B);
   gpio_set  ( MOTORL_EN,1);
}
void MotorL_Brake(void)
{   
   tpm_pwm_duty(MOTORL_FTM, MOTORL_PWMA,0);
   tpm_pwm_duty(MOTORL_FTM, MOTORL_PWMB,0);
   gpio_set  ( MOTORL_EN,1);
}
void MotorL_Slip(void)
{
   gpio_set  (MOTORL_EN,0);
   tpm_pwm_duty(MOTORL_FTM, MOTORL_PWMA,0);
   tpm_pwm_duty(MOTORL_FTM, MOTORL_PWMB,0);
}

int32 MotorL_GetWheelSpeed(void)
{
	int32 speed=0;
	speed = (int32)(cnt*ENCODL_GEAR_N*WHEELL_LENGTH*SPEEDL_FS/ENCODL_CYCLE/WHEELL_GEAR_N);
	return (speed*MotorL_GetDir());
}
int32 MotorL_GetTsCount(void)
{
	return cnt;
}
void MotorL_ClearCount(void)
{
	cnt=0;
}

void  MotorL_EncodeCountInc(void)
{
	cnt++;
}
int32 MotorL_GetDir(void)
{
	uint8 dir = gpio_get (MOTORL_SPD_DIR);
	if (dir)
		return -1;
	else
		return 1;
}
