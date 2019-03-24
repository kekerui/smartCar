/************************************************************
*                                                           *
* @filename:main.c                                          *
*                                                           *
* @brief: SmartCar  Contral  Init                           *
*                                                           *
* @the library:  VCAN¡¢kinetis¡£                            *
*                                                           *
* @date:2016.08.13                                          *
*                                                           *
* @modification time:2016.08.13                             *
*                                                           *
* @author: Shao Zexi                                        *
*                                                           *
************************************************************/

#include "include.h"
#include "common.h"


void main(void)
{                                                               
     DisableInterrupts;  
     CarSystem_Init();
     EnableInterrupts;
     Car_Test();
     while(1);                                                  
}


