#ifndef _EIT_AUTOEXPOSURE_H
#define _EIT_AUTOEXPOSURE_H

#include "include.h"

extern int TargetMaxPixelValue;
extern int RealMaxPixelValue;
extern int MaxPixelError;
extern uint8 IntegrationTime;
extern float IntegrationTimeError;
extern uint8 LastIntegrationTime;
extern uint8 MaxPixelTopK[4];


int MaxPixelAverage();
void FindMaxPixelValue();
//void QuickSort(uint8 *data,uint8 left,uint8 right);
void BubbleSort(uint8 *data);
void IntegrationTimeControlTask(void);

#endif //_EIT_AUTOEXPOSURE_H