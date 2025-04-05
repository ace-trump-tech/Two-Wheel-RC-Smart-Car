#include "STC15F2K.h"

#define uchar unsigned char 
#define uint unsigned int

sbit z=P3^4;
sbit y=P3^5;

sbit d=P3^2;

sbit hwx=P3^3;		//红外接收数据引脚，

unsigned char ly_lhj[4];					//保存NEC的四个字节数据
bit ly_flag=0;								//成功接收标志

void Delayms(uint t)
{
 	uchar i;
	while(t--)
	   for(i=0;i<100;i++);
}

void init()
{
    TMOD= 0x11;				   

	EX1=1;  				//开启外部中断1
	IT1=1;        			//设置成下降沿触发方式

	TL0=0;
	TH0=0;					//定时/计数器0初始值
	
    EA = 1;
}

void main()
{
	z=y=d=0;
	P3M0=0xC0;
	P3M1=0x00;
	init();
	while(1)
	{
		Delayms(100);				
	}
}

void delayh(void){		
	unsigned int i=200;
	while(i--);
}
 /***********外中断0入口函数，P33引脚，红外线接收IC数据脚**************************/
void hongwai(void)   interrupt 2
{
	unsigned char i,ia;
	/**************开始判断是否为NEC波形引导码的前9MS和后4.5MS******************************/
	TL0=0;
	TH0=0;				//定时/计数器1初始值
	TR0=1;				//定时器以12M晶振12分频计数，即1us计数
	while(!hwx);
	TR0=0;				//信号翻转停止计数
	if(TH0<30||TH0>40)	//NEC引导码前9MS，计数约9000，TH1约等于35,给个误差值，用30-40之间来判断
		return;

	TH0=0;				//定时/计数器1初始值
	TR0=1;
	while(hwx){
		delayh();
		if(TH0>22)		//NEC引导码引导码的后4。5MS，计数约4500，TH1约等于17
			return;		
	}
	TR0=0;
	if(TH0<12)			//NEC引导码引导码的后4。5MS，计数约4500，TH1约等于17
		return;

	/***********开始接收四个字节内容**************************************/	
	for(i=0;i<4;i++){
		for(ia=0;ia<8;ia++){				
			while(!hwx);	//低电平开始，不处理只等待高电平
			
			TH0=0;
			TR0=1;			//高电平开始，启动计数
			while(hwx){
				delayh();
				if(TH0>15)		
					return;		
			}
			TR0=0;
			//高电平结束，判断数据1或0向变量移入
			ly_lhj[i]>>=1;			//数据由高位移入低位
			if(TH0>4)				//时间量TH1高于4，即高于1MS判断为1
				ly_lhj[i]|=0x80;
		}
	}	
	d=!d;

	if(ly_lhj[2]==0x40)//向前
	{
		z=y=0;
	}
	if(ly_lhj[2]==0x07)//左移
	{
		z=1; y=0;
		Delayms(1500);
		z=y=0;
	}

	if(ly_lhj[2]==0x09)//右移
	{
		y=1;z=0;
		Delayms(1500);
		z=y=0;	
	}

	if(ly_lhj[2]==0x44)//左转圈
	{
	    
		z=1; y=0;
	}
	if(ly_lhj[2]==0x43)//右转圈
	{
	    
	    y=1;z=0;
	}
    if(ly_lhj[2]==0x19)//停止
	{
		z=y=1;
	}

	if(ly_lhj[2]==0x15)//停止
	{
		z=y=1;
	}

	TF0=0;
}
