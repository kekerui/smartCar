/*
 *******************************************************************************
 *                                EIT CarLib
 *                             lib FUNCTIONS
 *
 *                     (c) Copyright 2015-2020 Car@EIT 
 *                            All Rights Reserved
 * File      :CarTest.h
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
 * 2016-08-14     Zexi.Shao    Add Model_MAX
 *
 *******************************************************************************
 */
#ifndef __EIT_CAR_TEST_h__
#define __EIT_CAR_TEST_h__
#include "include.h"

#define  Model_MAX  8
extern   int32 Model;
extern int counttime;
extern int startcheckline;


void Car_Test(void);
#endif