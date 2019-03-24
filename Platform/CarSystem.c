/*
 *******************************************************************************
 *                                EIT CarLib
 *                             lib FUNCTIONS
 *
 *                     (c) Copyright 2015-2020 Car@EIT 
 *                            All Rights Reserved
 * File      :CarSystem.c
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

 * 2015-02-10     Xian.Chen    the first version
 *
 *******************************************************************************
 */
 
#include "include.h"

void Control_Init(void);
void Embedded_Init(void);
void CarSystem_Init(void)
{
    Control_Init(); 
    Embedded_Init();
}

void Control_Init(void)
{
    ControlVar_Init();
    ControlParam_Init();
    
    MotorLPID_Init();
    MotorRPID_Init();
}

void Embedded_Init(void)
{  
    LCD_init();
    camera_init(imgbuff0); 

    Steer_Init(gParam.SteerMid);
    //while(1);
    MotorL_Init();
    MotorR_Init(); 

    DataLog_Init();
    Buzzer_Init();
    Model_Init();
    Key_Init();

    set_vector_handler(PORTA_VECTORn,PORTA_handler);
    enable_irq(PORTA_IRQn);

    pit_init_ms(PIT0,10);
    set_vector_handler(PIT0_VECTORn,PIT0_IRQHandler);
    enable_irq(PIT0_IRQn);

    set_vector_handler(DMA0_VECTORn ,DMA0_IRQHandler);      //设置LPTMR的中断复位函数为 PORTA_IRQHandler
    enable_irq(DMA0_IRQn);

    //set_vector_handler(PORTD_VECTORn,PORTD_handler);
    //enable_irq(PORTD_IRQn);
}



