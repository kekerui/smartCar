#ifndef _EIT_SHOW_H
#define _EIT_SHOW_H

#include "include.h"
//#include "common.h"

extern Site_t  Site_V;
extern Site_t  Site_VNum;
extern Site_t  Site_MaxSpeed;           //��������ֵ
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


extern Site_t  Site_AutoStop;           //�Ƿ���ͣ��
extern Site_t  Site_ENorNOT;


extern Site_t  Site_NearDir;            //ƫ������ֵ
extern Site_t  Site_NearDirNum;
extern Site_t  Site_MidDir;
extern Site_t  Site_MidDirNum;
extern Site_t  Site_FarDir;
extern Site_t  Site_FarDirNum;


extern Size_t  imgsize;                //ͼ���С
extern Site_t  Site_A;                 //Сͼ�����Ͻ�λ��
extern Size_t  Size_A;
extern Site_t  Site_B;                 //��ͼ�����Ͻ�λ��
extern Size_t  Size_B; 


extern void   LCD_ShowSmallImag(void);
extern void   LCD_showADC(void);                      //ADת����Դ��ѹ
extern void   LCD_ShowBigImag(void);
extern void   LCD_ShowDebug(void);
extern void   LCD_ShowBoundImag(void);
#endif //_EIT_SHOW_H