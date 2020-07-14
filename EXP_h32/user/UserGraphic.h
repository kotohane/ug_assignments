#ifndef __UseGraphic_H__
#define __UseGraphic_H__

#include "user.h"

extern uint8_t ui8Task_Status;

void DrawMenu( uint8_t ui8SelectNum);
void GrTaskRectDraw(void);
void GrAxisDraw(void);
void GrStringNum(float datain,uint8_t Num,uint16_t page,uint16_t colomn);
#endif



