#ifndef __IM_PROC__
#define __IM_PROC__
#include "imType.h"

#ifndef MK60F15
   #define CAMERA_H 120
   #define CAMERA_W 160
#else
#include "include.h"
#endif
#define   WHITE_VAL   255
#define   BLACK_VAL   0
#define   CAM_MAX_LENGTH_CM  200
#define   LINE_WIDTH 7
#define   AVG_COUNT  5

#define   W_START    0
#define   W_END      159
#define   W_MID      79

#define  H_START  0
#define  H_END    119

#define  MorphOpen_LEN  13
#define  MorphOpen_Size 11


extern imINT16  HBoundL[CAMERA_H];
extern imINT16  HBoundR[CAMERA_H];
extern imINT16  HBoundM[CAMERA_H];
extern imINT16  HBoundM_F[CAMERA_H];
extern imINT16  HBoundM_REAL[CAM_MAX_LENGTH_CM+1];
extern imUINT8  Image_DataF[CAMERA_H][CAMERA_W];
extern imINT32  gDir_Near;
extern imINT32  gDir_Mid;
extern imINT32  gDir_Far;
extern int MaxWhite;
extern int Out;
extern int S;           //S路标志位
extern int num;                                 //用于初步判断是否是圆环 
extern int Circle;
extern int Graph_FindMidLine();
extern int Graph_FindMidLine1();
extern void Graph_Calculate_Dir(int speed);
#endif