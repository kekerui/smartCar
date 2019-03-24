/*
 *******************************************************************************
 *                                EIT Car Project
 *                             Main FUNCTIONS
 *
 *                     (c) Copyright 2015-2020 Car@EIT 
 *                            All Rights Reserved
 * File      : PORTn_handler.c
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
#include  "IntHandler.h"


void PORTA_handler(void)
{
    uint32 flag = PORTA_ISFR;
    PORTA_ISFR  = ~0; 
  
   if(flag & (1 << 29))                                 //PTA29触发中断
   {
       //gpio_set (PTA14, 0);
       ImageOver=0;
       camera_vsync();
       gVar.time++;
   }
   
}

/*!
 *  @brief      DMA0中断服务函数
 *  @since      v5.0
 */
void DMA0_IRQHandler()
{
    //gpio_set (PTA14, 1);
    camera_dma();
    ImageOver=1;
}

//Tasker Timer
void PIT0_IRQHandler(void)
{
   SpeedControlTask();
   //DistanceControlTask();
   PIT_Flag_Clear(PIT0);
}
void PORTE_IRQHandler()
{
    uint8  n;    //引脚号
    uint32 flag;

    flag = PORTE_ISFR;
    PORTE_ISFR  = ~0;                                   //清中断标志位

    n = 27;
    if(flag & (1 << n))                                 //PTE27触发中断
    {
        nrf_handler();
    }
}
