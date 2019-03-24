#ifndef _EIT_SHOW_H
#define _EIT_SHOW_H

#include "include.h"
//#include "common.h"

extern Site_t  Site_V;
extern Site_t  Site_VNum;
extern Site_t  Site_MaxSpeed;           //参数坐标值
extern Site_t  Site_MaxSpeedNum;
extern Site_t  Site_MinSpeed;
extern Site_t  Site_MinSpeedNum;
extern Site_t  Site_KpIn;
extern Site_t  Site_KpInNum;
extern Site_t  Site_KdIn;
extern Site_t  Site_KdInNum;
extern Site_t  Site_MDSK;
extern Site_t  Site_MDSKNum;
extern Site_t  Site_Kp;
extern Site_t  Site_KpNum;


extern Site_t  Site_AutoStop;           //是否开启停车
extern Site_t  Site_ENorNOT;


extern Site_t  Site_NearDir;            //偏差坐标值
extern Site_t  Site_NearDirNum;
extern Site_t  Site_MidDir;
extern Site_t  Site_MidDirNum;
extern Site_t  Site_FarDir;
extern Site_t  Site_FarDirNum;


extern Size_t  imgsize;                //图像大小
extern Site_t  Site_A;                 //小图像左上角位置
extern Size_t  Size_A;
extern Site_t  Site_B;                 //大图像左上角位置
extern Size_t  Size_B; 


extern void   LCD_ShowSmallImag(void);
extern void   LCD_showADC(void);                      //AD转换电源电压
extern void   LCD_ShowBigImag(void);
extern void   LCD_ShowDebug(void);
extern void   LCD_ShowBoundImag(void);
#endif //_EIT_SHOW_H