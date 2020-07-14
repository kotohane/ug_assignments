// Header: lcd drive UC1701X(��ST7565R����)
// File Name: cog12864.c
// Author: gx
// Date: 2020.06.23

//*****************************************************************************


//*****************************************************************************
//
// Include Files
//
//*****************************************************************************

#include "lcd_driver.h"
#include "lcdlib.h"
#include "math.h"
//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************
/* Global buffer for the display. This is especially useful on 1BPP, 2BPP, and 4BPP
  displays. This allows you to update pixels while reading the neighboring pixels
  from the buffer instead of a read from the LCD controller. A buffer is not required
  as a read followed by a write can be used instead.*/
uint8_t Template_Memory[(LCD_X_SIZE * LCD_Y_SIZE * BPP) / 8];
uint8_t INVERSE=0;

#ifdef USER_CODE			//���ڱ�ʾ��Щ���û�д�Ĵ��룬��Щ��ģ���Դ�����
//*****************************************************************************
//
// ms��ʱ
//
//*****************************************************************************
static void delay_COG12864(uint16_t ms)
{
	uint16_t k,j;
   for(k=0;k<ms;k++)
   	//_delay_cycles(SYSCLK);					// 25000����ǰʱ������25MHz
			for(j=0;j<5000;j++);
}
/*****************************************************************************
��         �ܣ�������λ 8bit ���λ��ǰ��
���÷�Χ��Template_Driver.c �ڲ�����
��         ���� byte: Ҫд��� 1 Byte����
�� ��  ֵ��   ��
 *****************************************************************************/
static void SendByte(uint8_t byte)
{
	unsigned char i=0;
	for(i=0;i<8;i++)
	{
		LCD_SCK_L;
		if((byte<<i)&0x80)
			LCD_SDA_H;
		else
			LCD_SDA_L;
		LCD_SCK_H;
	}
}
/*****************************************************************************
��         �ܣ���COG��Ļ������д��cnt������
���÷�Χ��Template_Driver.c �ڲ�����
��         ���� ptr: Ҫд�������ָ��
		cnt:ָ�����ݸ���
�� ��  ֵ��   ��
 *****************************************************************************/
void Write_Ndata( uint8_t *ptr ,uint16_t cnt)
{
	uint16_t i=0;
	uint8_t temp=0;
	uint8_t *tPtr;
	tPtr = ptr;
	LCD_CS_L;
	LCD_RS_H;
	for(i=0;i<cnt;i++)		// ����д��
	{
		temp = *tPtr++;		// ����д�����帲��֮ǰ��ͼ��
		SendByte(temp);
	}
	LCD_CS_H;
}

#endif
//*****************************************************************************
//
// Suggested functions to help facilitate writing the required functions below
//
//*****************************************************************************

// Writes data to the LCD controller
static void
WriteData(uint16_t usData)
{
	/* Write data to the LCD controller. For instance this can be bit banged 
	with 6800 or 8080 protocol or this could be the SPI routine for a SPI LCD */
#ifdef USER_CODE
	LCD_CS_L;									//��ʹ��
	LCD_RS_H;									//д���ݣ�AO��RS��Ϊ�ߵ�ƽ
	SendByte(usData);
	LCD_CS_H;
#endif

}

// Writes a command to the LCD controller
static void
WriteCommand(uint8_t ucCommand)
{
   /* This function is typically very similar (sometimes the same) as WriteData()
   The difference is that this is for the LCD to interpret commands instead of pixel
   data. For instance in 8080 protocol, this means pulling the DC line low.*/
#ifdef USER_CODE
	LCD_CS_L;									//��ʹ��
	LCD_RS_L;									//дָ�AO��RS��Ϊ�͵�ƽ
	SendByte(ucCommand);
	LCD_CS_H;
#endif
}

#ifdef USER_CODE
static void COGClear()// (��ֲ����Ķ�����)
{
	uint8_t x,y;
	 for(y=0;y<LCD_Y_SIZE/8;y++)
	  {
		WriteCommand(0xb0+y);
		WriteCommand(0x10);
	  	WriteCommand(0x00);
        for(x=0;x<LCD_X_SIZE;x++)
        {
					Template_Memory[y*128+x] = 0;
        	WriteData(0);
        }
	  }
}
#endif
// Sets the pixel address of the LCD driver
void SetAddress(int16_t lX, int16_t lY)
{
    /* This function typically writes commands (using WriteCommand()) to the 
	LCD to tell it where to move the cursor for the next pixel to be drawn. */
#ifdef USER_CODE
		WriteCommand(0xb0+lY);
		WriteCommand(0x10+(lX/16));
		WriteCommand(0x00+(lX%16));
#endif
}


// Initializes the pins required for the GPIO-based LCD interface.
// This function configures the GPIO pins used to control the LCD display
// when the basic GPIO interface is in use. On exit, the LCD controller
// has been reset and is ready to receive command and data writes.
static void
InitGPIOLCDInterface(void)
{
    /* Initialize the hardware to configure the ports properly for communication */
#ifdef USER_CODE

//  CS  RST   A0  SCK   SDA
//PTA13	PTD5 PTD0 PTD2 PTD3
//PTC11 C9   C8   A4   A5
	//enable clock to PORTA /D 
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK+SIM_SCGC5_PORTA_MASK;
	PORTC->PCR[11] = 0x0100;
	FGPIOC->PSOR |= BIT11;
	FGPIOC->PDDR |= BIT11;
	PORTC->GPCLR = 0x03000100; // PTC8 9  ->GPIO
	PORTA->GPCLR = 0x00300100; // PTA4 5  ->GPIO
	FGPIOC->PSOR |= BIT8+BIT9; //output 1 
	FGPIOC->PDDR |= BIT8+BIT9; //output 1 
	FGPIOA->PSOR |= BIT4+BIT5; // PTD0 2 3 5 OUTPUT
	FGPIOA->PDDR |= BIT4+BIT5; // PTD0 2 3 5 OUTPUT

//ROM-CS ROM-SCK  ROM-OUT  ROM-IN 
// PTA12    D4       A2       A1   
	SIM->SCGC5 |=SIM_SCGC5_PORTD_MASK+SIM_SCGC5_PORTA_MASK;;
	PORTA->PCR[12] = 0x0100;
	PORTD->PCR[4] = 0x0100;
	PORTA->PCR[2] = 0x0100;
	PORTA->PCR[1] = 0x0100;
	FGPIOA->PSOR |= BIT12+BIT1;
	FGPIOA->PDDR |= BIT12+BIT1;
	FGPIOD->PSOR |= BIT4;
	FGPIOD->PDDR |= BIT4;
	FGPIOA->PDDR &=~BIT2;
#endif
}

// Initialize DisplayBuffer.
// This function initializes the display buffer and discards any cached data.
static void
InitLCDDisplayBuffer(void *pvDisplayData, uint16_t ulValue)
{
#ifdef USER_CODE

	uint16_t i=0,j=0;
	 for(j=0;j<LCD_Y_SIZE/8;j++)
	   for(i=0;i<LCD_X_SIZE;i++)
	   	   Template_Memory[j*128+i]=ulValue;

#endif
}

// Initializes the display driver.
// This function initializes the LCD controller
//
// TemplateDisplayFix
void
LCD_DriverInit(void)
{
	/*Initialize the LCD controller.
	This typically looks like:
	
    InitGPIOLCDInterface();
    InitLCDDisplayBuffer();
	// Init LCD controller parameters
	// Enable LCD
	// Init Backlight
	// Clear Screen
	*/
	InitGPIOLCDInterface();
	InitLCDDisplayBuffer(0,0);
#ifdef USER_CODE
	LCD_CS_L;
	LCD_RST_L;
	delay_COG12864(20);
	LCD_RST_H;
	delay_COG12864(20);

	WriteCommand(0xe2);	//system reset
	delay_COG12864(200);
	WriteCommand(0xe2);	 //��λ
	delay_COG12864(5);
	WriteCommand(0x2c);  //��ѹ����1
	delay_COG12864(50);	
	WriteCommand(0x2e);  //��ѹ����2
	delay_COG12864(50);
	WriteCommand(0x2f);  //��ѹ����3
	delay_COG12864(5);
	WriteCommand(0x23);  //�ֵ��Աȶȣ������÷�Χ0x20��0x27
	WriteCommand(0x81);  //΢���Աȶ�
	WriteCommand(0x28);  //΢���Աȶȵ�ֵ�������÷�Χ0x00��0x3f
	WriteCommand(0xa2);  //1/9ƫѹ�ȣ�bias��
	WriteCommand(0xc8);  //��ɨ��˳�򣺴��ϵ���
	WriteCommand(0xa0);  //��ɨ��˳�򣺴�����
	WriteCommand(0x40);  //��ʼ�У���һ�п�ʼ
	WriteCommand(0xaf);  //����ʾ

	COGClear();
#endif
}

//����ʱ
static void 
delay_us(int n_us)               
{
	int j,k;
	for(j=0;j<n_us;j++)
	for(k=0;k<1;k++);
}


//��ָ��ֿ�IC
static void 
ROM_WriteCmd(uint8_t  datu )
{
	uint8_t i;
	for(i=0;i<8;i++ )
	{
		ROM_SCK_L;
		//delay_us(10);
		if((datu<<i)&0x80)
			ROM_IN_H;
		else
			ROM_IN_L;
		ROM_SCK_H;
	}
}
//���ֿ�IC��ȡ���ֻ��ַ����ݣ�1���ֽڣ�
static uint8_t ROM_ReadData(void)
{
	uint8_t i;
	uint8_t ret_data=0;
	for(i=0;i<8;i++)
	{
		ROM_SCK_L;
		delay_us(1);
		ret_data=ret_data<<1;
		if(ROM_OUT_DATA)
			ret_data=ret_data+1;
		else
			ret_data=ret_data+0;
		ROM_SCK_H;
		//delay_us(1);
	}
	return(ret_data);
}


//��ָ����ַ��������д��Һ����ָ����page,column)������
static void 
GrDisplay_ZK_16x16(uint32_t fontaddr,uint8_t page,uint8_t column)
{
	uint8_t i,j,disp_data;
	ROM_CS_L;
	ROM_WriteCmd(0x03);
	ROM_WriteCmd((fontaddr&0xff0000)>>16);	 //��ַ�ĸ�8λ,��24λ
	ROM_WriteCmd((fontaddr&0xff00)>>8);		 //��ַ����8λ,��24λ
	ROM_WriteCmd(fontaddr&0xff);				 //��ַ�ĵ�8λ,��24λ
	for(j=0;j<2;j++)
	{
		SetAddress(column,page+j);
		for(i=0; i<16; i++ )
	    {
			disp_data=ROM_ReadData();
			if(INVERSE)
			{
					Template_Memory[128*(page+j)+i+column] =~disp_data;
					WriteData(Template_Memory[128*(page+j)+i+column]);
			}
			else	
			{
					Template_Memory[128*(page+j)+i+column] = disp_data;
					WriteData(Template_Memory[128*(page+j)+i+column]);
			}
		}		
	}
	ROM_CS_H;
}
//��ָ����ַ��������д��Һ����ָ����page,column)������
static void 
GrDisplay_ZK_8x16(uint32_t fontaddr,uint8_t page,uint8_t column)
{
	uint8_t i,j,disp_data;
	ROM_CS_L;
	ROM_WriteCmd(0x03);
	ROM_WriteCmd((fontaddr&0xff0000)>>16);	 //��ַ�ĸ�8λ,��24λ
	ROM_WriteCmd((fontaddr&0xff00)>>8);		 //��ַ����8λ,��24λ
	ROM_WriteCmd(fontaddr&0xff);				 //��ַ�ĵ�8λ,��24λ
	for(j=0;j<2;j++)
	{
		SetAddress(column,page+j);
		for(i=0;i<8;i++)
		{		
			disp_data=ROM_ReadData();
			if(INVERSE)
			{
				Template_Memory[128*(page+j)+i+column] =~disp_data;
				WriteData(Template_Memory[128*(page+j)+i+column]);
			}
			else	
			{
				Template_Memory[128*(page+j)+i+column] = disp_data;
				WriteData(Template_Memory[128*(page+j)+i+column]);
			}
		}
	}
	ROM_CS_H;
}


//��ָ����ַ��������д��Һ����ָ����page,column)������
static void 
GrDisplay_ZK_5x8(uint32_t fontaddr,uint8_t page,uint8_t column) 
{
	uint8_t i,disp_data;
	ROM_CS_L;
	ROM_WriteCmd(0x03);
	ROM_WriteCmd((fontaddr&0xff0000)>>16);	 //��ַ�ĸ�8λ,��24λ
	ROM_WriteCmd((fontaddr&0xff00)>>8);		 //��ַ����8λ,��24λ
	ROM_WriteCmd(fontaddr&0xff);				 //��ַ�ĵ�8λ,��24λ
	SetAddress(column,page);
	for(i=0; i<5; i++ )
	{
		disp_data=ROM_ReadData();
		if(INVERSE)
		{
			Template_Memory[128*page+i+column] =~disp_data;
			WriteData(Template_Memory[128*page+i+column]);
		}
		else	
		{
			Template_Memory[128*page+i+column] = disp_data;
			WriteData(Template_Memory[128*page+i+column]);
		}
	}
	ROM_CS_H;
}

void GrString_GB2312(char *text,uint8_t page,uint8_t column)
{
	uint32_t  fontaddr=0;
	uint8_t i= 0;			
	column = column*8;
	while((((uint8_t)text[i])>0x00))
	{
		if(((((uint8_t)text[i])>=0xb0) &&(((uint8_t)text[i])<=0xf7))&&(((uint8_t)text[i+1])>=0xa1))
		{						
			//������壨GB2312���������ֿ�IC�еĵ�ַ�����¹�ʽ�����㣺
			//Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+ 846)*32+ BaseAdd;BaseAdd=0
			//���ڵ���8λ��Ƭ���г˷�������⣬���Է�����ȡ��ַ
			fontaddr = (((uint8_t)text[i])- 0xb0)*94; 
			fontaddr += (((uint8_t)text[i+1])-0xa1)+846;
			fontaddr = (uint32_t)(fontaddr*32);

			GrDisplay_ZK_16x16(fontaddr,page,column);	 //��ָ����ַ��������д��Һ����ָ����page,column)������
			i+=2;
			column+=16;
		}
		else if(((((uint8_t)text[i])>=0xa1) &&(((uint8_t)text[i])<=0xa3))&&(((uint8_t)text[i+1])>=0xa1))
		{						
			//������壨GB2312��15x16����ַ����ֿ�IC�еĵ�ַ�����¹�ʽ�����㣺
			//Address = ((MSB - 0xa1) * 94 + (LSB - 0xA1))*32+ BaseAdd;BaseAdd=0
			//���ڵ���8λ��Ƭ���г˷�������⣬���Է�����ȡ��ַ
			fontaddr = (((uint8_t)text[i])- 0xa1)*94; 
			fontaddr += (((uint8_t)text[i+1])-0xa1);
			fontaddr = (uint32_t)(fontaddr*32);
	
			GrDisplay_ZK_16x16(fontaddr,page,column);	 //��ָ����ַ��������д��Һ����ָ����page,column)������
			i+=2;
			column+=16;
		}
		else if((((uint8_t)text[i])>=0x20) &&(((uint8_t)text[i])<=0x7e))	
		{									
			fontaddr = (((uint8_t)text[i])- 0x20);
			fontaddr = (unsigned long)(fontaddr*16);
			fontaddr = (unsigned long)(fontaddr+0x3cf80);			

			GrDisplay_ZK_8x16(fontaddr,page,column);	 //��ָ����ַ��������д��Һ����ָ����page,column)������
			i+=1;
			column+=8;
		}
		else
			i++;	
	}
}
void GrString_5x8(char *text,uint8_t page,uint8_t column)
{
	uint32_t  fontaddr=0;
	unsigned char i= 0;
	column = column*8;
	while((text[i]>0x00))
	{
		
		if((((uint8_t)text[i])>=0x20) &&(((uint8_t)text[i])<=0x7e))	
		{						
			fontaddr = (((uint8_t)text[i])- 0x20);
			fontaddr = (unsigned long)(fontaddr*8);
			fontaddr = (unsigned long)(fontaddr+0x3bfc0);			

			GrDisplay_ZK_5x8(fontaddr,page,column);	 //��ָ����ַ��������д��Һ����ָ����page,column)������
			i+=1;
			column+=6;
		}
		else
		i++;	
	}

}


//*****************************************************************************
//
// All the following functions (below)
//
//*****************************************************************************


void GrClear(uint16_t ulValue)
{
	uint8_t x,y;
	if(ulValue >=1)
		ulValue =0xff;
	for(y=0;y<LCD_Y_SIZE/8;y++)
	{
		WriteCommand(0xb0+y);
		WriteCommand(0x10);
		WriteCommand(0x00);
		for(x=0;x<LCD_X_SIZE;x++)
		{
			Template_Memory[y*128+x] = ulValue;
			WriteData(ulValue);
		}
	}
}

void AsciiDisplay8_16(uint16_t dat,uint16_t page,uint16_t column)
{
	uint8_t i,j,n;	
	column *= 8;					
	n = dat - 0x20;
	
	for(j=0;j<2;j++)
	{	
		SetAddress(column,page+j);
		for(i=0;i<8;i++)
		{		
			if(INVERSE)
			{
				Template_Memory[128*(page+j)+i+column] =~nAsciiDot[n].Msk[i+8*j];
				WriteData(Template_Memory[128*(page+j)+i+column]);
			}
			else	
			{
				Template_Memory[128*(page+j)+i+column] = nAsciiDot[n].Msk[i+8*j];
				WriteData(Template_Memory[128*(page+j)+i+column]);
			}
		}
	}
}
void AsciiDisplay6_12(uint16_t dat,uint16_t page,uint16_t column)
{
	uint8_t i,j,n;	
	column *= 8;					
	n = dat - 0x20;
	
	for(j=0;j<2;j++)
	{	
		SetAddress(column,page+j);
		for(i=0;i<8;i++)
		{	
			 
			if(INVERSE)
			{
				if(i==0 || i== 7)
				{
					Template_Memory[128*(page+j)+i+column] =~0x00;
					WriteData(Template_Memory[128*(page+j)+i+column]);
				}
				else
				{
					Template_Memory[128*(page+j)+i+column] =~nAsciiDots[n].Msk[i-1+6*j];
					WriteData(Template_Memory[128*(page+j)+i+column]);
				}
			}
			else	
			{
				if(i==0 || i== 7)
				 {
						Template_Memory[128*(page+j)+i+column] |= 0x00;
						WriteData(Template_Memory[128*(page+j)+i+column]);
				 }
				 else
				 {
						Template_Memory[128*(page+j)+i+column] |= nAsciiDots[n].Msk[i-1+6*j];
						WriteData(Template_Memory[128*(page+j)+i+column]);
				 }
			}
		}
	}
}

void GrAsciiDraw(char *dat,uint16_t page,uint16_t column)
{
	page *= 2;	
	while(*dat != '\0')
	{
		AsciiDisplay6_12(*dat++,page,column++);
		if(column>15)
		{
			page+=2;column = 0;
		}
	}
}

void HzDisplay16_16(char *dat,uint16_t page,uint16_t column)
{
	uint8_t i,j,n;
	if((page>=7)||(column>=15))
		return;		
// find HZ in the lcd lib
	n = 0;
	while(1)
	{
		if((GB_16[n].Index[0]&0xff) == (dat[0]&0xff))
			if((GB_16[n].Index[1]&0xff) == (dat[1]&0xff))
				break;
		n++;
		if(n == 200)
		{
			GrAsciiDraw("The Chinese is not exist",0,0);
			return;
		}
	}
//--**********write the ddram ***************************
	column*=8;
	for(j=0;j<2;j++)
	{
		SetAddress(column,page+j);
		for(i=0;i<16;i++)
		{		
			if(INVERSE)
			{
					Template_Memory[128*(page+j)+i+column] =~GB_16[n].Msk[i+j*16];
					WriteData(Template_Memory[128*(page+j)+i+column]);
			}
			else	
			{
					Template_Memory[128*(page+j)+i+column] = GB_16[n].Msk[i+j*16];
					WriteData(Template_Memory[128*(page+j)+i+column]);
			}
		}
	}	
}

void GrStringDraw( char *dat,uint16_t page,uint16_t column)
{
 	char  buff[3];
	page *= 2;	
 	while(*dat != '\0')
	{
 		if(((uint8_t)*dat)<0xA0) //ASCII
			AsciiDisplay8_16(*dat++,page,column++);//
		else          //??
		{
		 	buff[0] = *dat++;
			buff[1] = *dat++;
			buff[2] = '\0';
			if(column == 15)
			{
				column = 0;
			  page +=2;
			}
			HzDisplay16_16(buff,page,column);
			column += 2;
		}
		if(column>=16){
		   column = 0;
			page+=2;
			if(page>=8)
				page =0;		
		}
	}

}

void GrlineDraw(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2)
{
 	uint8_t x,y,start,end,page,column,buf;
	float kx,ky;
  if(x1 != x2)
	{
		if(x1<x2){
			start=x1;end=x2;}
		else{
			start=x2;end=x1;}	
		kx = (y2-y1)*1.0/(x2-x1);		
		for(x=start;x<=end;x++)
		{
			y=(uint8_t)(kx*(x-x1)+y1);	
			buf=(uint8_t)pow(2,(7-y%8));
			page=7-y/8;
			column = x;
			SetAddress(column,page);
			Template_Memory[128*page+column] |= buf;
			WriteData(Template_Memory[128*page+column]);
		}
	}
	if(y1 != y2)
	{
		if(y1<y2){
			start=y1;end=y2;}
		else{
			start=y2;end=y1;}
		ky = (x2-x1)*1.0/(y2-y1);
		for(y=start;y<=end;y++)
		{
			x=(uint8_t)(ky*(y-y1)+x1);	
			buf=(uint8_t)pow(2,(7-y%8));
			page=7-y/8;
			column = x;
			SetAddress(column,page);
			Template_Memory[128*page+column] |= buf;
			WriteData(Template_Memory[128*page+column]);
		}
	}
}


void GrRectDraw(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1)
{
  GrlineDraw(x0,y0,x1,y0);
	GrlineDraw(x0,y1,x1,y1);
	GrlineDraw(x0,y0,x0,y1);
	GrlineDraw(x1,y0,x1,y1);
}
/*
 * ������void floatTochar(float datain,char *dataout)
 * ���ø�ʽ��floatTochar(FloatData,chardata,Num);
 * ���ã���һ��������10000�ĸ�������ת��Ϊ�ַ������ݣ��䱣����С������Num����
 * �βΣ���1������ĸ�����datain����2��������ַ������ݣ���Ϊָ�룬��Ϊ���飩dataout����3������С����λ��Num
 * ����ֵ����
 * ʵ�������룺datin=28.1234,Num=2 �����"28.12"���߿��Կ���{'2','8','.','1','2','\0'}��
 *     ���룺datin=288.1234,Num=4 �������288.1234������{'2','8','8','.','1','2','3','4','\0'}.
 *     ���Կ������ú��������Զ������������ݵĴ�С�Լ�����С�����λ������������ַ����ĳ���
 */
void floatTochar(float datain,char dataout[],int Num)
{
	unsigned int i,j;
	unsigned int temp,buff;
	float temp_f;
	if(datain<0)
	{
		dataout[0] = '-';     //���datain < 0 ,���ַ�����һλΪ���š�-��
		i=1;
	}
	else
		i=0;
	temp = (unsigned int)fabs(datain);  //�������������ֵĴ���
	buff = 1;
	for(j=0;j<10;j++)
	{
		if(temp/buff == 0)
			break;              //������һ��
		buff = buff * 10;
	}
	if(buff != 1)
		buff = buff/10;
	i = 0;
	while(buff>=1)
	{
		dataout[i++] = temp/buff+0x30;
		temp %= buff;
		buff /=10;
	}
	if(Num>0)
	{
		dataout[i++] = '.';	          //С����'.'
		temp_f = datain-(unsigned int)datain;  //����ΪС�����ֵĴ���
		buff = 10;
		for(j=0;j<Num;j++)
		{
			dataout[i++]= (unsigned int)(temp_f*buff) % 10 + '0';
			buff = buff *10;
		}
	}
	dataout[i] = '\0';   //�ǳ���Ҫ�����е��ַ���������'\0'��β����Ҳ������������ַ����ķ�����
}
char databuf[10];
void GrStringNum(float datain,uint8_t Num,uint16_t page,uint16_t colomn)
{
	 floatTochar(datain,databuf,Num);
	 GrString_GB2312(databuf,page,colomn);
}
//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
