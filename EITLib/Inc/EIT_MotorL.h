/*
 *******************************************************************************
 *                                EIT CarLib
 *                             lib FUNCTIONS
 *
 *                     (c) Copyright 2015-2020 Car@EIT 
 *                            All Rights Reserved
 * File      : EIT_MotorL.h
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
 * 2015-01-24     Xian.Chen    the first version
 *
 *******************************************************************************
 */

#ifndef __EIT_MOTORL_DEF__
#define __EIT_MOTORL_DEF__
#include "include.h"


/*Motor Driver*/
#define    MOTORL_PWM_MAX   1000
#define    MOTORL_PWM_MIN   (-1000)
#define    MOTORL_PWM_FREQ  15000
#define    MOTORL_FTM    FTM0
#define    MOTORL_EN     PTA19
#define    MOTORL_PWMA   FTM_CH1
#define    MOTORL_PWMB   FTM_CH2
#define    MOTORL_PWMAIO PTA4
#define    MOTORL_PWMBIO PTA5


/*Encode */
#define    MOTORL_ENCODE_FTM             FTM2                                           //左编码器用FTM2
#define    MOTORL_GEAR_N                 9                                              //C车电机自带齿轮齿轮数
#define    ENCODL_GEAR_N                 31                                             //编码器齿数
#define    WHEELL_GEAR_N                 67                                             //左轮减速箱齿轮数 (28*24/10)
#define    ENCODL_CYCLE                  512                                            //编码器线数
#define    WHEELL_LENGTH                 17                                             //后轮周长16.7cm

#define    SPEEDL_FS                     100 //Hz

extern void MotorL_Init(void);
extern void MotorL_Run(int32 pwm);
extern void MotorL_Brake(void);
extern void MotorL_Slip(void);

extern int32 MotorL_GetWheelSpeed(int32 CntInTs);
extern int32 MotorL_GetTsCount(void);

#endif