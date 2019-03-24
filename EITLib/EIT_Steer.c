/*
 *******************************************************************************
 *                                EIT Car Project
 *                             Main FUNCTIONS
 *
 *                     (c) Copyright 2015-2020 Car@EIT 
 *                            All Rights Reserved
 * File      : EIT_header.c
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
 *
 *******************************************************************************
 */

#include  "EIT_Steer.h"             //header用户应用程序


//header初始化
void Steer_Init(int32 Mid)
{
  //port_init(PTA12, PULLUP );//舵机patA 
  FTM_PWM_init(STEER_FTM,STEER_PWM,STEER_PWM_FREQ,Mid);//舵机pwm初始化
}

void Steer_Run(int32 Mid,int32 Deg)
{
   FTM_PWM_Duty(STEER_FTM,STEER_PWM,Mid+Deg);
}
