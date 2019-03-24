#ifndef _EIT_KEY_H
#define _EIT_KEY_H

#include "include.h"

#define KEY_MSG_SIZE       20                          //定义消息队列长度
#define TAB                6                           //参数个数
extern uint8 RunDebugFlag;

typedef enum                                         //按键类型
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
    KEYUP = 0,                                         //按键没有按下
    KEYDOWN = 1,                                       //按键按下
}EitKeyStatus;

typedef struct                                       //按键消息结构体
{
    EitKey           key;                              //按键编号
    EitKeyStatus     status;                           //按键状态
}KeyMSG;

extern void Key_Init(void);                          //按键初始化
extern EitKeyStatus Key_Check(EitKey key);            //按键检测
extern void Key_Debug(void);                         //调试模式
#endif