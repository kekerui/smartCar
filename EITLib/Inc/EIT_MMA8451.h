/*
 *******************************************************************************
 *                                EIT Car Project
 *                             Main FUNCTIONS
 *
 *                     (c) Copyright 2015-2020 Car@EIT 
 *                            All Rights Reserved
 * File      : main.c
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
 * 2015-01-22    ZhuJianNing   the first version
 *
 *******************************************************************************
 */
#ifndef __EIT_MMA8451_H__
#define __EIT_MMA8451_H__

#include   "common.h"
#include   "MK60_i2c.h"

#define   MMA8451_I2C    I2C0
#define   MMA8451_FREQ   400*1000
#define   MMA8451_IRQ    PTD10                 //INT2

#define   MMA8451_REG_STATUS         0x00         //״̬�Ĵ���
#define   MMA8451_REG_OUTX_MSB       0x01         //14λ���������8λ[7:0]
#define   MMA8451_REG_OUTX_LSB       0x02         //14λ���������6λ[7:2]
#define   MMA8451_REG_OUTY_MSB       0x03         //14λ���������8λ[7:0]
#define   MMA8451_REG_OUTY_LSB       0x04         //14λ���������6λ[7:2]
#define   MMA8451_REG_OUTZ_MSB       0x05         //14λ���������8λ[7:0]
#define   MMA8451_REG_OUTZ_LSB       0x06         //14λ���������6λ[7:2]
#define   MMA8451_REG_SYSMOD         0x0B         //��ǰϵͳģʽ
#define   MMA8451_REG_INT_SOURCE     0x0C         //�ж�Դ�Ĵ���                 
#define   MMA8451_REG_WHOAMI         0x0D         //�豸ID�Ĵ���
#define   MMA8451_REG_CTRL_REG1      0x2A         //ϵͳ���ƼĴ���1
#define   MMA8451_REG_CTRL_REG2      0x2B         //ϵͳ���ƼĴ���2
#define   MMA8451_REG_CTRL_REG3      0x2C         //ϵͳ���ƼĴ���3                   //����ֵĬ�ϣ��͵�ƽ��
#define   MMA8451_REG_CTRL_REG4      0x2D         //ϵͳ���ƼĴ���4                   //0x01
#define   MMA8451_REG_CTRL_REG5      0x2E         //ϵͳ���ƼĴ���5                   //Ĭ��INT2

#define   MMA8451_DEV_ADDR   0x1C                 //�豸��ַ
#define   MMA8451_DEV_WRITE  MMA8451_DEV_ADDR<<1 | 0x00    
#define   MMA8451_DEV_READ   MMA8451_DEV_ADDR<<1 | 0x01
#define   MMA8451_DEV_ID     0x1A                 //MMA8451�豸ID

#define   MMA8451_DATA_READY       0x01
#define   MMA8451_STATUS_X_READY   0x01
#define   MMA8451_STATUS_Y_READY   0x02
#define   MMA8451_STATUS_Z_READY   0x04
#define   MMA8451_STATUS_XYZ_READY 0x08
#define   MMA8451_STATUS_X_OW      0x10
#define   MMA8451_STATUS_Y_OW      0x20
#define   MMA8451_STATUS_Z_OW      0x40
#define   MMA8451_STATUS_XYZ_OW    0x80

extern int16 XOUT;
extern int16 YOUT;
extern int16 ZOUT;

void  EIT_MMA8451_Write(uint8,uint8);
uint8 EIT_MMA8451_Init(void);
uint8 EIT_MMA8451_Read(uint8);
int16 EIT_MMA8451_GetResult(uint8);
void EIT_MMA8451_IRQ(void);

#endif