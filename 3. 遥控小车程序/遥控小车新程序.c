#include "STC15F2K.h"

#define uchar unsigned char 
#define uint unsigned int

sbit z=P3^4;
sbit y=P3^5;

sbit d=P3^2;

sbit hwx=P3^3;		//��������������ţ�

unsigned char ly_lhj[4];					//����NEC���ĸ��ֽ�����
bit ly_flag=0;								//�ɹ����ձ�־

void Delayms(uint t)
{
 	uchar i;
	while(t--)
	   for(i=0;i<100;i++);
}

void init()
{
    TMOD= 0x11;				   

	EX1=1;  				//�����ⲿ�ж�1
	IT1=1;        			//���ó��½��ش�����ʽ

	TL0=0;
	TH0=0;					//��ʱ/������0��ʼֵ
	
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
 /***********���ж�0��ں�����P33���ţ������߽���IC���ݽ�**************************/
void hongwai(void)   interrupt 2
{
	unsigned char i,ia;
	/**************��ʼ�ж��Ƿ�ΪNEC�����������ǰ9MS�ͺ�4.5MS******************************/
	TL0=0;
	TH0=0;				//��ʱ/������1��ʼֵ
	TR0=1;				//��ʱ����12M����12��Ƶ��������1us����
	while(!hwx);
	TR0=0;				//�źŷ�תֹͣ����
	if(TH0<30||TH0>40)	//NEC������ǰ9MS������Լ9000��TH1Լ����35,�������ֵ����30-40֮�����ж�
		return;

	TH0=0;				//��ʱ/������1��ʼֵ
	TR0=1;
	while(hwx){
		delayh();
		if(TH0>22)		//NEC������������ĺ�4��5MS������Լ4500��TH1Լ����17
			return;		
	}
	TR0=0;
	if(TH0<12)			//NEC������������ĺ�4��5MS������Լ4500��TH1Լ����17
		return;

	/***********��ʼ�����ĸ��ֽ�����**************************************/	
	for(i=0;i<4;i++){
		for(ia=0;ia<8;ia++){				
			while(!hwx);	//�͵�ƽ��ʼ��������ֻ�ȴ��ߵ�ƽ
			
			TH0=0;
			TR0=1;			//�ߵ�ƽ��ʼ����������
			while(hwx){
				delayh();
				if(TH0>15)		
					return;		
			}
			TR0=0;
			//�ߵ�ƽ�������ж�����1��0���������
			ly_lhj[i]>>=1;			//�����ɸ�λ�����λ
			if(TH0>4)				//ʱ����TH1����4��������1MS�ж�Ϊ1
				ly_lhj[i]|=0x80;
		}
	}	
	d=!d;

	if(ly_lhj[2]==0x40)//��ǰ
	{
		z=y=0;
	}
	if(ly_lhj[2]==0x07)//����
	{
		z=1; y=0;
		Delayms(1500);
		z=y=0;
	}

	if(ly_lhj[2]==0x09)//����
	{
		y=1;z=0;
		Delayms(1500);
		z=y=0;	
	}

	if(ly_lhj[2]==0x44)//��תȦ
	{
	    
		z=1; y=0;
	}
	if(ly_lhj[2]==0x43)//��תȦ
	{
	    
	    y=1;z=0;
	}
    if(ly_lhj[2]==0x19)//ֹͣ
	{
		z=y=1;
	}

	if(ly_lhj[2]==0x15)//ֹͣ
	{
		z=y=1;
	}

	TF0=0;
}
