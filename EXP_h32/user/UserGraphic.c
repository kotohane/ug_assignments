#include "user.h"
#include <math.h>
//----�˵��ַ�������----
char* cMark[] = {"","#","##","###","####","#####","######","#######","########","#########","##########"};
char* cResult[] = {
	"Normal",
	"Fidgeting",
	"Error"
};


uint8_t ui8Task_Status = 1;
#define TOTAL_MENU 4

void DrawMenu(int ccrt, int mdata, int result)
{
	GrClearDisplay(0x00);							//????
	INVERSECOLOR;
	if(ccrt==-1){
		printinv("Monitor",0);

		if(mdata==1){
			printinv("??",2);
		}
		else{
			prints("??",2);
		}

		if(mdata==2){
			printinv("example1",4);
		}
		else{
			prints("example1",4);
		}

		if(mdata==3){
			printinv("example2",6);
		}
		else{
			prints("example2",6);
		}

		return;
	}
	if(ccrt>=0){
		prints(int2char(ccrt),0);
		int mpb = ccrt*10/mdata;
		prints(cMark[mpb],1);
		prints(cResult[result],2);
		printinv("Abort",3);
	}
}

 //--------------------------------------------------------------
 //	===============
 //  || TASK NAME ||
 //	===============
 //--------------------------------------------------------------

 void GrTaskRectDraw(void)
 {
 	GrString_GB2312(cMenuString[ui8Task_Status]+3,0,4);
 	GrRectDraw(12,63-0,112,63-14);
 	GrRectDraw(14,63-2,110,63-12);
 }

  /*
  * ��������������
  * =====================
  * ��
  * |
  * |
  * |������������
  * =====================
  */
 void GrAxisDraw(void)
 {
	 GrlineDraw(2,21,2,61);
	 GrlineDraw(2,84,2,61);
	 GrlineDraw(0,23,2,21);
	 GrlineDraw(4,23,2,21);
	 GrlineDraw(82,59,84,61);
	 GrlineDraw(82,63,84,61);
 }
/*
 * ������unsigned int intTochar(unsigned int datain,char dataout[])
 * ���÷�����len = intTochar(IntData,chardata);�������Ҫ������Ϣ�Ļ�������ֱ�ӣ�
 * 		  intTochar(IntData,chardata);
 * ���ã���һ��������10000���޷��ŵ���������ת��Ϊ�ַ�������
 * �βΣ���1��������޷��ŵ��������ݣ���2��������ַ������ݣ���Ϊָ�룬��Ϊ���飩
 * ����ֵ�����ַ�����������ĳ���
 * ʵ�������룺datin=28 �����"28"���߿��Կ���{'2','8','\0'},������3��
 *     ���룺288  �������288������{'2','8','8','\0'}������4.
 *     ���Կ������ú��������Զ������������ݵĴ�С����������ַ����ĳ���
 */
unsigned int intTochar(unsigned int datain,char dataout[])
{
	unsigned int i,buff,temp;
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
	return (i+1);                     //���ص����ַ�����������ĳ���
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
	unsigned long temp,buff;
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
	dataout[i++] = '.';	          //С����'.'
	temp_f = datain-(unsigned int)datain;  //����ΪС�����ֵĴ���
	buff = 10;
	for(j=0;j<Num;j++)
	{
		dataout[i++]= (unsigned int)(temp_f*buff) % 10 + '0';
		buff = buff *10;
	}
	dataout[i] = '\0';   //�ǳ���Ҫ�����е��ַ���������'\0'��β����Ҳ������������ַ����ķ�����
}
void GrStringNum(float datain,uint8_t Num,uint16_t page,uint16_t colomn)
{
	 char *dat;
	 floatTochar(datain,dat,Num);
	 GrString_GB2312(dat,page,colomn);
}


