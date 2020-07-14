
//*****************************************************************************
//
// Template_Driver.h - Display driver for any LCD Controller. This file serves as
//						a template for creating new LCD driver files
//
//*****************************************************************************

#ifndef __LCD_DRIVER_H__
#define __LCD_DRIVER_H__

#include "user.h"

#define USER_CODE

#ifdef USER_CODE


//*****************************************************************************
//
// 								管脚宏定义
//
//*****************************************************************************
//  CS  RST   A0  SCK   SDA
//PTA13 PTD5 PTD0 PTD2  PTD3  
//PTC11 C9   C8   A4    A5
#define LCD_CS_H 			FPTC->PSOR = BIT11 
#define LCD_CS_L 			FPTC->PCOR = BIT11

#define LCD_RST_H 		FPTC->PSOR = BIT9
#define LCD_RST_L 		FPTC->PCOR = BIT9

#define LCD_RS_H 			FPTC->PSOR = BIT8
#define LCD_RS_L 			FPTC->PCOR = BIT8

#define LCD_SDA_H 		FPTA->PSOR = BIT5
#define LCD_SDA_L 		FPTA->PCOR = BIT5

#define LCD_SCK_H 		FPTA->PSOR = BIT4
#define LCD_SCK_L 		FPTA->PCOR = BIT4

#define LCD_BKLED_H
#define LCD_BKLED_L


//ROM-CS ROM-SCK  ROM-OUT  ROM-IN 
// PTC9    C8       A5       A4
//PTA12			D4			A2			A1
#define ROM_CS_H 			FPTA->PSOR = BIT12   
#define ROM_CS_L 			FPTA->PCOR = BIT12 

#define ROM_SCK_H 		FPTD->PSOR = BIT4
#define ROM_SCK_L 		FPTD->PCOR = BIT4

#define ROM_OUT_DATA  (FPTD->PDIR & BIT4)  

#define ROM_IN_H 			FPTA->PSOR = BIT1
#define ROM_IN_L 			FPTA->PCOR = BIT1

//*****************************************************************************
//
// 								LCD命令宏定义
//
//*****************************************************************************

#define DISPLAY_ON 				0xAF	//显示开启
#define DISPLAY_OFF 			0xAE
#define DISPLAY_START_LINE 		0x40   //0
#define DISPLAY_PAGE 			0xB0
#define DISPLAY_ADC_L 			0xA0    //the LSB is first
#define DISPLAY_ADC_M 			0xA1	//负向，131-4
#define DISPLAY_REVERSE_OFF 	0xA6
#define DISPLAY_REVERSE_ON 		0xA7
#define DISPLAY_ENTIRE_ON  		0xA5
#define DISPLAY_ENTIRE_OFF 		0xA4	//非全屏显示
#define DISPLAY_SHL_NOM 		0xC0	//com0-com63
#define DISPLAY_SHL_REV 		0xC8
#define LCD_POWER_CV 			0x2C
#define LCD_POWER_RV 			0x2E
#define LCD_POWER_FV 			0x2F	//电源全开
#define LCD_REPOWER_1 			0x81	//亮度调节	 调节颜色 	不同的模块需要不同的  ELECTSET
#define LCD_REPOWER_2 			0x31
#define LCD_RESISTOR 			0x20
#define LCD_RESET 				0xE2
#define DISPLAY_COLUMN_MSB 		0x10    //1
#define DISPLAY_COLUMN_LSB 		0x00	//0

#define  Display  0xAF  		//显示开启
#define  Power    0x2F	   		//电源全开
#define  VO       0x24     		//对比度调节
#define  AllPoint 0xA4	   		//非全屏显示
#define  ADCset   0xA1    		//负向，131-4
#define  COMset   0xc0    		//com0-com63
#define  ELECTVO  0x81     		//亮度调节	 调节颜色 	不同的模块需要不同的  ELECTSET
#define  ELECTSET 0x2a	   		//亮度数值	 调节颜色 	不同的模块需要不同的	数值
#define  BIASSET  0xA2    		//占空比1/9

#endif


/*Put any necessary include files here*/

//*****************************************************************************
//
// User Configuration for the LCD Driver
//
//*****************************************************************************
// Number of pixels on LCD X-axis
// TemplateDisplayFix
#define LCD_X_SIZE	128

// Number of pixels on LCD Y-axis
// TemplateDisplayFix
#define LCD_Y_SIZE	64

// Number of bits required to draw one pixel on the LCD screen
// TemplateDisplayFix
#define BPP	1

extern uint8_t INVERSE;
#define INVERSECOLOR INVERSE=1
#define NORMALCOLOR  INVERSE=0


extern void LCD_DriverInit(void);
extern void GrClear(uint16_t ulValue);
extern void GrString_GB2312(char *text,uint8_t page,uint8_t column);
extern void GrString_5x8(char *text,uint8_t page,uint8_t column);
extern void GrlineDraw(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2);
extern void GrRectDraw(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1);
extern void GrStringNum(float datain,uint8_t Num,uint16_t page,uint16_t colomn);

#define GrClearDisplay(ulValue) GrClear(ulValue)


#endif // __Template_Driver_H__
