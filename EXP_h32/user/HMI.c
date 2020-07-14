#include "user.h"
//---------------------��غ궨��-------------------------

#define MaxValue 7
//---------------------��ر�������-----------------------


char* cMark[] = {
    "",
    "#",
    "##",
    "###",
    "####",
    "#####",
    "######",
    "#######",
    "########",
    "#########",
    "##########",
	  "###########",
		"############",
		"#############",
		"##############",
		"###############",
		"################"
};
char* cResult[] = {
	"Normal",
	"Fidgeting",
	"Error"
};


void RGBLED_Init(void)
{
	//tricolor led: PTB18/19,PTD1 output 
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK + SIM_SCGC5_PORTD_MASK;//enable clock to PORT B/D 	
	PORTB->PCR[18] = 0x0100;// PTB18->GPIO
	PORTB->PCR[19] = 0x0100;// PTB19->GPIO
	PORTD->PCR[1]  = 0x0100;// PTD1->GPIO
	FPTB->PSOR |= BIT18+BIT19; //output high to turn off leds
	FPTD->PSOR |= BIT1; //led off	
	FPTB->PDDR |= BIT18+BIT19; // PTB18/19 OUTPUT
	FPTD->PDDR |= BIT1; // PTD1 OUTPUT	
}
	

static void ButtonKey_Init(void)
{		
  //key: PTA1 A2  D4 A12, INPUT PULL-UP
  //new: PTC7 C0 C3 C4
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	PORTC->PCR[7] = 0x0103; //PTA16 GPIO, Pull-up
	PORTC->PCR[0] = 0x0103; //PTA17 GPIO, Pull-up
	PORTC->PCR[3] = 0x0103; //PTA17 GPIO, Pull-up
	PORTC->PCR[4] = 0x0103; //PTA17 GPIO, Pull-up
	FPTC->PDDR &=~(BIT0+BIT3+BIT4+BIT7);	
}

void SysTick_Init(void) //10ms
{
	SysTick->LOAD=209715-1;
	SysTick->CTRL = 0x07; //start,clk=sysclk=20.97152MHz,INTEN
}

void Key_Init(void)
{	
	ButtonKey_Init();
	SysTick_Init();
}

static void KEY_Left_Onclick()  
{
	ui8Key = KeyY;
}

static void KEY_Right_Onclick()
{
	ui8Key = KeyA;
}
static void KEY_Up_Onclick()
{
	ui8Key = KeyX;
}
static void KEY_Down_Onclick()
{
	ui8Key = KeyB;
}

char* int2char(int datain)
{
	int i,buff,temp;
	static char dataout[10];
	temp = datain;
	buff = 1;
	for(i=0;i<5;i++)
	{
		if(temp/buff == 0)
			break;              //��γ������ʵ���Զ��жϸ������ж���λ��
		buff = buff * 10;
	}
	if(buff!= 1)
		buff = buff/10;
	i = 0;                     //���datain= 28,��buff=10�����datain = 288,��buff=100�����Կ���buff������ȡdatain�����λ�ġ�
	while(buff>=1)
	{
		dataout[i] = temp/buff+0x30;  //ȡ���ݵ����λ,���������0x30��ת��Ϊascii��
		temp %= buff;				  //�����ݵ����λȥ������288-->88
		buff /=10;                    //buffҲ����10�������´�����ȡ�������ݵ����λ��,��100-->10����temp= 88,buff=10����temp/buff = 8
		i++;
	}
	dataout[i] = '\0';                //�ǳ���Ҫ�����е��ַ���������'\0'��β����Ҳ������������ַ����ķ�����
	return dataout;                     //���ص����ַ�����������ĳ���
}

//===========��ʱɨ�跨�жϻ�е��������Ҫ����Systick�ж��е���============
void Key_IODect(void)
{
	static unsigned char KEY1_Now=0;	//����ֵ������ʱ�豣��
	unsigned char KEY1_Past=0;
	static unsigned char KEY2_Now=0;	//����ֵ������ʱ�豣��
	unsigned char KEY2_Past=0;
	static uint8_t KEY3_Now=0,KEY4_Now=0;
	uint8_t KEY3_Past=0,KEY4_Past=0;
	
	KEY1_Past=KEY1_Now;
	KEY2_Past=KEY2_Now;
	KEY3_Past = KEY3_Now;
	KEY4_Past = KEY4_Now;
	
	
	//-----��ѯPTA2������Ĵ���-----
	if(FPTC->PDIR & BIT0)
		KEY1_Now=1;
	else
		KEY1_Now=0;
	//-----ǰһ�θߵ�ƽ����һ�ε͵�ƽ��˵����������-----
	if((KEY1_Past==1)&&(KEY1_Now==0))
		KEY_Left_Onclick();

	//-----��ѯPTD4������Ĵ���-----
	if(FPTD->PDIR & BIT3)
		KEY2_Now=1;
	else
		KEY2_Now=0;
	//-----ǰһ�θߵ�ƽ����һ�ε͵�ƽ��˵����������-----
	if((KEY2_Past==1)&&(KEY2_Now==0))
		KEY_Right_Onclick();

		//-----��ѯPTA12������Ĵ���-----
	if(FPTC->PDIR & BIT4)
		KEY3_Now=1;
	else
		KEY3_Now=0;
	//-----ǰһ�θߵ�ƽ����һ�ε͵�ƽ��˵����������-----
	if((KEY3_Past==1)&&(KEY3_Now==0))
		KEY_Up_Onclick();
	
		//-----��ѯPTA1������Ĵ���-----
	if(FPTC->PDIR & BIT7)
		KEY4_Now=1;
	else
		KEY4_Now=0;
	//-----ǰһ�θߵ�ƽ����һ�ε͵�ƽ��˵����������-----
	if((KEY4_Past==1)&&(KEY4_Now==0))
		KEY_Down_Onclick();
}


void prints(char* str, int line){
	NORMALCOLOR;
	GrStringDraw(str,line,0);
}

void printinv(char* str, int line){
	INVERSECOLOR;
	GrStringDraw(str,line,0);
}

/*
* ccrt: remind time
* mdata: additional data
* mdata2: result: 0 Normal | 1 Fidget
*/
void DrawMenu(int ccrt, int mdata, int result)
{
	GrClearDisplay(0x00);							//����
	INVERSECOLOR;
	if(ccrt==-1){
		prints("Monitor",0);

		if(mdata==1){
			printinv("start",1);
		}
		else{
			prints("start",1);
		}

		if(mdata==2){
			printinv("example1",2);
		}
		else{
			prints("example1",2);
		}

		if(mdata==3){
			printinv("example2",3);
		}
		else{
			prints("example2",3);
		}
		return;
	}
	if(ccrt>=0){
		prints(int2char(ccrt),0);
		int mpb = ccrt*16/mdata;
		prints(cMark[mpb],1);
		prints(cResult[result],2);
	}
}