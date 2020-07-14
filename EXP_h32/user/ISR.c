#include "user.h"

uint16_t N_SysTick=0;

void SysTick_Handler(void)
{	 
	 N_SysTick++;
	 Key_IODect();
}


		  	

