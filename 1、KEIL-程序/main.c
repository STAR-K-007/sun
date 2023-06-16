#include<reg52.h>
#include<stdio.h>
#include "PCF8591.H"

#define uchar unsigned char
#define uint  unsigned int

#define  PCF8591 0x90    //PCF8591 地址

sbit ENTER = P3^3;
sbit UP = P3^4;
sbit DOWN = P3^5;
sbit LEFT = P3^6;
sbit RIGHT = P3^7;

sbit LED1 = P1^2;
sbit LED2 = P1^3;
uchar Motor_Up[4]={0x08,0x04,0x02,0x01};
uchar Motor_Down[4]={0x01,0x02,0x04,0x08};


uchar Motor_Left[4]={0x10,0x20,0x40,0x80};
uchar Motor_Right[4]={0x80,0x40,0x20,0x10};

unsigned char AD_CHANNEL;
//延时函数
void Delay_ms(uint jj)
{
	uint ii;
	while(jj--)
		for(ii=0;ii<125;ii++);
}
void UART_Init(void)
{
     TMOD = 0x20;   //定时器模式选择
     PCON = 0x00;
     SCON = 0x50;			
     TH1 = 0xFd;    //设置波特率 9600
     TL1 = 0xFd;
     TR1 = 1;		//启动定时器1
	 ES = 1;		//开串口中断
	 EA = 1;		//开总中断		
}
/********************************************************************
* 名称   : PSend_Hex()
* 功能   : 发送字符串函数,可控制发送长度
* 变量   : 无
* 返回值 : 无
********************************************************************/
void Send_Hex(unsigned char *p,unsigned char num)
{
   	while(num--)   //剩余发送的字符数
	{
        SBUF = *p; //将要发送的数据赋给串口缓冲寄存器
		while(!TI);//等待发送结束
		TI = 0;    //软件清零
		p++;       //指针加一
	}	
}
void main()	   //函数功能：主函数
{	  
 
	uchar AD_Up,AD_Down,AD_Left,AD_Right;
	uchar i,j,k;
	uchar Table[4];
	Delay_ms(2000);
	UART_Init();	
	while(1)
	{
	    if(!ENTER)
		{
			Delay_ms(20);
			while(!ENTER);
			k++;	
		}
		if(k%2 == 0)
		{
		LED1 = 1;
		LED2 = 0;
		for(i=0;i<8;i++)
		{
			switch(AD_CHANNEL)
			{
				case 0: PCF8591_ISendByte(PCF8591,0x41);
				AD_Up=PCF8591_IRcvByte(PCF8591)*2;  //ADC0 模数转换1
				break;  
				
				case 1: PCF8591_ISendByte(PCF8591,0x42);
				AD_Down=PCF8591_IRcvByte(PCF8591)*2;  //ADC1  模数转换2
				break;  
				case 2: PCF8591_ISendByte(PCF8591,0x43);
				AD_Left=PCF8591_IRcvByte(PCF8591)*2;  //ADC2 模数转换3
				break;  
				
				case 3: PCF8591_ISendByte(PCF8591,0x40);
				AD_Right=PCF8591_IRcvByte(PCF8591)*2;  //ADC3  模数转换4
				break; 
			}

			Send_Hex(Table,4);
			if(++AD_CHANNEL>3)
			{
			 AD_CHANNEL=0;
			 Table[0] = AD_Up;
			Table[1] = AD_Down;
			Table[2] = AD_Left;
			Table[3] = AD_Right;
			 }
		}
		}
		if(k%2 == 1)
		{
			LED1 = 0;
			LED2 = 1;
			AD_Up = 0;	//上
			AD_Down = 0;//下
			AD_Left = 0;//左
			AD_Right = 0;//右	
		}		
        ///////////////////////////
	  
		if(((AD_Up<AD_Down)&&(AD_Down-AD_Up>10))||(!UP))//上转 
		{
			for(i=0;i<10;i++)
			{ 
				for(j=0;j<4;j++)
				{
					P2=Motor_Up[j];
					Delay_ms(10);
				}		
			}	
		}
		//////////////////////////////////////
		if(((AD_Up>AD_Down)&&(AD_Up-AD_Down>10))||(!DOWN))//下转
		{
			for(i=0;i<10;i++)
			{ 
				for(j=0;j<4;j++)
				{
					P2=Motor_Down[j];
					Delay_ms(10);
				}		
			}
		}
		/////////////////////////////
		if(((AD_Left<AD_Right)&&(AD_Right-AD_Left>10))||(!LEFT))//左转
		{
			for(i=0;i<10;i++)
			{ 
				for(j=0;j<4;j++)
				{
					P2=Motor_Left[j];
					Delay_ms(10);
				}		
			}
		
		}
		//////////////////////////////////////
		if(((AD_Left>AD_Right)&&(AD_Left-AD_Right>10))||(!RIGHT))//右转
		{
			for(i=0;i<10;i++)
			{ 
				for(j=0;j<4;j++)
				{
					P2=Motor_Right[j];
					Delay_ms(10);
				}		
			}
		}
		////////////////////////////////////
		Delay_ms(100);//延时，可适当修改
	}
}

