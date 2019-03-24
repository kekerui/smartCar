#ifndef _EIT_KEY_H
#define _EIT_KEY_H

#include "include.h"

#define KEY_MSG_SIZE       20                          //������Ϣ���г���
#define TAB                6                           //��������
extern uint8 RunDebugFlag;

typedef enum                                         //��������
{
    KEY_EN,
    KEY_ACK,
    KEY_INC,
    KEY_DEC,
    KEY_OUT,
    KEYMAX,
}EitKey;

typedef enum
{
    KEYUP = 0,                                         //����û�а���
    KEYDOWN = 1,                                       //��������
}EitKeyStatus;

typedef struct                                       //������Ϣ�ṹ��
{
    EitKey           key;                              //�������
    EitKeyStatus     status;                           //����״̬
}KeyMSG;

extern void Key_Init(void);                          //������ʼ��
extern EitKeyStatus Key_Check(EitKey key);            //�������
extern void Key_Debug(void);                         //����ģʽ
#endif