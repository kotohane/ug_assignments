#include "user.h"
#include <math.h>
//----菜单字符串定义----
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
  * 绘制任务坐标轴
  * =====================
  * ↑
  * |
  * |
  * |—————→
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
 * 函数：unsigned int intTochar(unsigned int datain,char dataout[])
 * 调用方法：len = intTochar(IntData,chardata);如果不需要长度信息的话，可以直接：
 * 		  intTochar(IntData,chardata);
 * 作用：将一个不大于10000的无符号的整型数据转化为字符串数据
 * 形参：（1）输入的无符号的整型数据；（2）输出的字符串数据（可为指针，可为数组）
 * 返回值：是字符串所在数组的长度
 * 实例：输入：datin=28 输出："28"或者可以看做{'2','8','\0'},并返回3；
 *     输入：288  输出：“288”或者{'2','8','8','\0'}，返回4.
 *     可以看出，该函数可以自动根据输入数据的大小，调整输出字符串的长度
 */
unsigned int intTochar(unsigned int datain,char dataout[])
{
	unsigned int i,buff,temp;
	temp = datain;
	buff = 1;
	for(i=0;i<5;i++)
	{
		if(temp/buff == 0)
			break;              //这段程序可以实现自动判断该整数有多少位，
		buff = buff * 10;
	}
	if(buff!= 1)
		buff = buff/10;
	i = 0;                     //如果datain= 28,则buff=10，如果datain = 288,则buff=100，可以看出buff是用来取datain的最高位的。
	while(buff>=1)
	{
		dataout[i] = temp/buff+0x30;  //取数据的最高位,并将其加上0x30，转化为ascii码
		temp %= buff;				  //将数据的最高位去掉，如288-->88
		buff /=10;                    //buff也除以10，方便下次仍能取到新数据的最高位，,如100-->10，且temp= 88,buff=10，则temp/buff = 8
		i++;
	}
	dataout[i] = '\0';                //非常重要，所有的字符串必须以'\0'结尾，这也是区分数组和字符串的方法。
	return (i+1);                     //返回的是字符串所在数组的长度
}
/*
 * 函数：void floatTochar(float datain,char *dataout)
 * 调用格式：floatTochar(FloatData,chardata,Num);
 * 作用：将一个不大于10000的浮点数据转化为字符串数据，其保留其小数点由Num决定
 * 形参：（1）输入的浮点数datain；（2）输出的字符串数据（可为指针，可为数组）dataout；（3）保留小数点位数Num
 * 返回值：无
 * 实例：输入：datin=28.1234,Num=2 输出："28.12"或者可以看做{'2','8','.','1','2','\0'}；
 *     输入：datin=288.1234,Num=4 输出：“288.1234”或者{'2','8','8','.','1','2','3','4','\0'}.
 *     可以看出，该函数可以自动根据输入数据的大小以及保留小数电的位数，调整输出字符串的长度
 */
void floatTochar(float datain,char dataout[],int Num)
{
	unsigned int i,j;
	unsigned long temp,buff;
	float temp_f;
	if(datain<0)
	{
		dataout[0] = '-';     //如果datain < 0 ,则字符串第一位为负号“-”
		i=1;
	}
	else
		i=0;
	temp = (unsigned int)fabs(datain);  //首先是整数部分的处理
	buff = 1;
	for(j=0;j<10;j++)
	{
		if(temp/buff == 0)
			break;              //和整型一样
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
	dataout[i++] = '.';	          //小数点'.'
	temp_f = datain-(unsigned int)datain;  //以下为小数部分的处理
	buff = 10;
	for(j=0;j<Num;j++)
	{
		dataout[i++]= (unsigned int)(temp_f*buff) % 10 + '0';
		buff = buff *10;
	}
	dataout[i] = '\0';   //非常重要，所有的字符串必须以'\0'结尾，这也是区分数组和字符串的方法。
}
void GrStringNum(float datain,uint8_t Num,uint16_t page,uint16_t colomn)
{
	 char *dat;
	 floatTochar(datain,dat,Num);
	 GrString_GB2312(dat,page,colomn);
}


