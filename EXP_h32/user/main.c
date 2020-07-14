//******************************************************************************
//  MKL25Z128VLK4 Demo -GPIO_key_led
//
//  Description: Poll P1.4 in a loop, if hi P1.0 is set, if low, P1.0 reset.
//  ACLK = n/a, MCLK = SMCLK = default DCO
//
//              MKL25Z128VLK4 
//            -----------------
//        /|\|            PTB18|-->LED RED
//         | |                 |
//         --|RST         PTB19|-->LED GREEN
//     /|\   |                 |
//      --o--|PTE4         PTD1|-->LED BLUE
//     \|/
//
//   GaoXiang
//   Nanjin Normal University
//   June 2020
//   Built with Keil MDK-ARM v5.29
//******************************************************************************


#include "user.h"

uint8_t Key_Prev = 1;
uint16_t adc_result;

int user_mode;
uint8_t ui8Key;

float Vin;

#define False 0
#define True 1
#define Error 2

float eg1data[30][3]={
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0}
};
int eg1len = 30;
float eg2data[30][3] = {
    {0,0,0},
    {3,3,-4},
    {6,-10,-10},
    {10,10,-10},
    {10,10,-10},
    {10,10,-10},
    {10,10,-10},
    {10,10,-10},
    {6,6,-3},
    {0,0,0},
    {3,3,-3},
    {10,10,-10},
    {10,10,-10},
    {10,10,-10},
    {10,10,-10},
    {10,10,-10},
    {10,10,-10},
    {10,10,-10},
    {10,10,-10},
    {10,10,-10},
    {10,10,-10},
    {10,10,-10},
    {10,10,-10},
    {10,10,-10},
    {10,10,-10},
    {10,10,-10},
    {10,10,-10},
    {10,10,-10},
    {10,10,-10},
    {10,10,-10}
};
int eg2len = 30;

int ccrt = -1, metamax = 30, result = False;
float rgx=0, rgy=0, rgz=0, rpart=3;
float accerth = 30;
int main_choose = 0;

int egindex = 0;

void RGB(uint16_t dat)
{
		if(dat&BIT2) //RED
				FPTB->PCOR = BIT18;  //PTB18=0 RED ON
		else
				FPTB->PSOR = BIT18;  //PTB18=1
		if(dat&BIT1) 
				FPTB->PCOR = BIT19; //PTB19=0 GREEN ON
		else
				FPTB->PSOR = BIT19;  //PTB19=1
		if(dat&BIT0) //RED
				FPTD->PCOR = BIT1;  //PTD1=0   BLUE ON
		else
				FPTD->PSOR = BIT1;  //PTD1=1
}


void delay(long  n)
{
	long  i;
	for(i=0;i<n;i++);
}

void delay_ms(uint16_t ms)
{
	uint16_t k,j;
   for(k=0;k<ms;k++)
   	//_delay_cycles(SYSCLK);					// 25000即当前时钟设在25MHz
			for(j=0;j<5000;j++);
}

void System_Init(void)
{
	LCD_DriverInit();
	Key_Init();
}

float absn(float a){
	if(a<0) return -a;
	return a;
}

float mergein(float gn, float rgn){
	float gr = (absn(gn)+rgn*rpart)/(1+rpart);
	return gr;
}

void accer_input(float gx, float gy, float gz){
	rgx = mergein(gx,rgx);
	rgy = mergein(gx,rgy);
	rgz = mergein(gx,rgz);
}

void get_accer(){
	if(user_mode==2){ //example1
		accer_input(eg1data[egindex][0],eg1data[egindex][1],eg1data[egindex][2]);
		if(++egindex>=eg1len){
			  egindex = 0;
			  ccrt = -1;
			  main_choose = 1;
		}
		return;
	}
	if(user_mode==3){ //example2
		accer_input(eg2data[egindex][0],eg2data[egindex][1],eg2data[egindex][2]);
		if(++egindex>=eg2len){
			egindex = 0;
			ccrt = -1;
			main_choose = 1;
		}
		return;
	}
}

int result_ju(){
	if(rgx*rgx+rgy*rgy+rgz*rgz > accerth){
		return True;
	}
	return False;
}

void key_reset(){
	ui8Key = 0;
	delay_ms(1);
}

int choose_rl(int a){
	if(a<=0){
		a=1;
	}
	if(a>=4){
		a=3;
	}
	return a;
}

int main(void)
{
	  RGBLED_Init();
	  RGB(0b010);
	
	  System_Init();
	  __enable_irq();
	
	  user_mode = 0;
	  main_choose = 1;
	  ui8Key = 0;
	  ccrt=-1;
	  while(1){
		    if(ccrt==-1){
						RGB(0);
			      DrawMenu(ccrt, main_choose, 0);
			      while(ui8Key==KeyNULL) 
				    delay_ms(1);
            if(ui8Key==KeyY){
			          ccrt = metamax;
				        user_mode  = main_choose;
                key_reset();
                continue;
            }
			      if(ui8Key==KeyX){
				        main_choose = choose_rl(--main_choose);
				        key_reset();
				        continue;
			      }
			      if(ui8Key==KeyB){
				        main_choose = choose_rl(++main_choose);
				        key_reset();
				        continue;
			      }
			      result = 0;
			      delay_ms(10);
        }
        if(ccrt>=0){
            if(ccrt==0 || ui8Key==KeyY || egindex>eg1len){
                ccrt = -1;
                DrawMenu(ccrt, metamax, result);
								main_choose = 1;
                key_reset();
                continue;
            }
			      get_accer();
			      result = result_ju();
			      DrawMenu(ccrt, metamax, result);
			      if(result){
				        RGB(0b100);
			      }
			      else{
				        RGB(0);
			      }
            ccrt -= 1;
						delay_ms(1000);
            continue;
        }
	  }
}
