#ifndef _EIT_BUZZER_H
#define _EIT_BUZZER_H

#include "include.h"

#define BUZZER   PTE6
#define ENABLE   1
#define DISABLE  0

extern void Buzzer_Init(void);
extern void EnBuzzer(void);
extern void DisBuzzer(void);

#endif