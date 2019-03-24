

#include "EIT_AutoExposure.h"

int TargetMaxPixelValue = 180;
int RealMaxPixelValue;
int MaxPixelValueError;
float IntegrationTimeError;
uint8 IntegrationTime;
uint8 LastIntegrationTime = 10;
uint8 MaxPixelTopK[4];

//在四个最大灰度值中取平均
int MaxPixelAverage()
{
  int i,temp;
  int sum = 0;
  for(i = 0;i < 4;i ++)
  {
     sum += MaxPixelTopK[i];
  }
  temp = sum/4;
  return temp;
}

void BubbleSort(uint8 *data)
{
  int i,j;
  uint8 temp;
  for(i = 0;i < TSL1401_SIZE - 1;i ++)
  {
    for(j = i;j < TSL1401_SIZE;j ++)
    {
      if(data[i] <= data[j])
      {
        temp = data[i];
        data[i] = data[j];
        data[j] = temp;
      }
    }
  }
}
/*
//快排算法
void QuickSort(uint8 *data,uint8 left,uint8 right)
{
  if(left >= right)
  {
    return;
  }
  int i;
  int j;
  int key;
  i = left;
  j = right;
  key = data[left];
  while(i < j)
  {
    while((i < j) && (key >= data[j]))
    {
      j --;
    }
    data[i] = data[j];
    while((i < j) && (key <= data[i]))
    {
      i ++;
    }
    data[j] = data[i];
  }
  
  data[i] = key;
  QuickSort(data,left,i - 1);
  QuickSort(data,j + 1,right);
}
*/
void FindMaxPixelValue()
{
  int i;
  BubbleSort(Image_Data2[1]);
  for(i = 0;i < 4;i ++)
  {
    MaxPixelTopK[i] = Image_Data2[1][i];
    //printf("%d ",MaxPixelTopK[i]);
  }
  //printf("\r\n");
}
void IntegrationTimeControlTask(void)
{
  FindMaxPixelValue();
  RealMaxPixelValue = MaxPixelAverage();
  MaxPixelValueError = TargetMaxPixelValue - RealMaxPixelValue;
  IntegrationTimeError = MaxPixelValueError * gParam.IntegrationTimeKp;
  IntegrationTime = LastIntegrationTime + IntegrationTimeError;
  if(IntegrationTime < gParam.MinIntegrationTime)
    IntegrationTime = gParam.MinIntegrationTime;
  if(IntegrationTime > gParam.MaxIntegrationTime)
    IntegrationTime = gParam.MaxIntegrationTime;
  tsl1401_restet();
  tsl1401_set_time(IntegrationTime);
  LastIntegrationTime = IntegrationTime;
  
   printf("time: %d ",IntegrationTime);
}