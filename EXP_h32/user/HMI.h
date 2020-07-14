#ifndef __HMI_H__
#define __HMI_H__
//*****************************************************************************
//
// Include Files
//
//*****************************************************************************
#include "user.h"

#define 	KEY_LEFT_ENTER 		(BIT1)
#define 	KEY_RIGHT_EXIT 		(BIT2)

extern uint8_t ui8Value;
extern char *cMenuString[];

//-----�ⲿ��������-----
void SysTick_Init(void);
void Key_Init(void);
void RGBLED_Init(void);
void Key_IODect(void);
char* int2char(int datain);
void prints(char* str, int line);
void printinv(char* str, int line);
void DrawMenu(int ccrt, int mdata, int result);
#endif

