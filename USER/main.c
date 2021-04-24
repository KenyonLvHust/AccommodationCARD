#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "string.h"
#include "iic.h"
#include "oled.h"
#include "STEP.h"

#define N 7

/*********************
接线方式:
IN1 A---- PA1
IN2 B---- PA2
IN3 C---- PA3
IN4 D---- PA4
+   ---- +5V
-   ---- GND
*********************/

#define uchar unsigned char
#define uint  unsigned int
	
#define MotorData P0                    //步进电机控制接口定义
uchar phasecw[4] ={0x08,0x04,0x02,0x01};//正转 电机导通相序 D-C-B-A
uchar phaseccw[4]={0x01,0x02,0x04,0x08};//反转 电机导通相序 A-B-C-D

#define GPIO_LOCK GPIOA
#define PIN_LOCK GPIO_Pin_0

#define GPIO_LED GPIOC
#define PIN_LED GPIO_Pin_13

#define GPIO_STEP GPIOA
#define PIN_A GPIO_Pin_1
#define PIN_B GPIO_Pin_2
#define PIN_C GPIO_Pin_3
#define PIN_D GPIO_Pin_4

char ID[N][30]={
  "3F9682D3",
  "52E5B83D",
  "495EE600",
  "1975CD30",
	"3FB2CBD3",
	"2A4E2DE0",
	"BFD096B0",
};

void Open(char* ID_get);
int look_for(char* ID_get);
int strcmp_my(char *s1,char *s2);

void LED_GPIO_Config(void);

void LOCK_GPIO_Config(void);

void STEP_GPIO_Config(void);
void MotorCW(void);
void MotorCCW(void);
void MotorStop(void);

int main(void)
{	
	SystemInit(); 
	delay_init();//延时函数初始化	 

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(9600);//串口初始化为9600
	
	LED_GPIO_Config();
//	LOCK_GPIO_Config();
//	STEP_GPIO_Config();
	
	TIM3_PWM_Init(199,7199);	 //(199+1)*(7199+1)/72*10^6
  //上面一行求出0.02s,即20ms
	
	delay_ms(190);
	TIM_SetCompare2(TIM3, 190);//90度
		
//	IIC_Init();
//	OLED_Init();

	while(1) 
	{		
//		if(USART_RX_STA&0x8000 && get_ok==1)
		if(USART_RX_STA&0x8000)
		{					   
			Open((char*)USART_RX_BUF);
			USART_RX_STA=0;
			memset(USART_RX_BUF,0,9);
//			get_ok=0;
		}
	}
}



int look_for(char* ID_get)
{
  int i=0;
  for(i=0;i<N;i++)
  {
    if(strcmp_my(ID_get,ID[i])==1)
    return 1;
  }
  return 0;
}

/*
此处以90度为例。

答：PWM周期为20ms，所以占空比就应该为1.5ms/20ms = 7.5%，
所以TIM_SetCompare2的 TIMx 捕获比较 1 寄存器值就为200-200*7.5% = 185
*/
int ISopen=0;
void Open(char* ID_get)
{
   ISopen=look_for(ID_get);
   if(ISopen==1)
   {
    delay_ms(195);
		TIM_SetCompare2(TIM3, 193);//0度
		GPIO_ResetBits(GPIO_LED, PIN_LED);
		 
    delay_ms(3000);
		 
		delay_ms(190);
		TIM_SetCompare2(TIM3, 190);//90度
		GPIO_SetBits(GPIO_LED, PIN_LED);
		
	  ISopen=0;
	 }
}

int strcmp_my(char *s1,char *s2)
{
	int longs1=strlen(s1);
	int longs2=strlen(s2);
	
	int i;
	if(longs1!=longs1)
		return 0;
	else
	{
		for(i=1;i<longs2;i++)
		{
			if(s1[i]!=s2[i])
				return 0;
		}
		return 1;		
	}
}

 /***************  配置LED用到的I/O口 *******************/
void LED_GPIO_Config(void)	
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);  
  GPIO_InitStructure.GPIO_Pin = PIN_LED;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_LED, &GPIO_InitStructure);
  GPIO_SetBits(GPIO_LED, PIN_LED );
}

/*
int main(void)
 {		
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 //串口初始化为115200
 	LED_Init();		 //LED端口初始化
	KEY_Init();
	 
 	TIM3_PWM_Init(199,7199);	 //(199+1)*(7199+1)/72*10^6
      //上面一行求出0.02s,即20ms
   	while(1)
	{		
     if (KEY0==0)
	{
		delay_ms(195);
		TIM_SetCompare2(TIM3, 195);//0度
		LED1=0;
	}

	if(KEY1==0)
	{
		delay_ms(190);
		TIM_SetCompare2(TIM3, 190);//90度
	}
	if(WK_UP==1)
	{
		delay_ms(10);
		TIM_SetCompare2(TIM3, 185);//90度
	}		
	}	 
 }

//------------------------------------------------------------
// t = 0.5ms——————-舵机会转动 0 °
//t = 1.0ms——————-舵机会转动 45°
//t = 1.5ms——————-舵机会转动 90°
//t = 2.0ms——————-舵机会转动 135°
//t = 2.5ms——————-舵机会转动180°
————————————————
版权声明：本文为CSDN博主「-electronic-engineer」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/qq_45941706/article/details/108951250
*/

// /***************  配置电机用到的I/O口 *******************/
//void LOCK_GPIO_Config(void)	
//{
//  GPIO_InitTypeDef GPIO_InitStructure;
//  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE); 
//  GPIO_InitStructure.GPIO_Pin = PIN_LOCK;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIO_LOCK, &GPIO_InitStructure); 
//  GPIO_ResetBits(GPIO_LOCK ,PIN_LOCK );
//}

//void STEP_GPIO_Config(void)	
//{
//  GPIO_InitTypeDef GPIO_InitStructure;
//  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE); 
//  GPIO_InitStructure.GPIO_Pin = PIN_A|PIN_B|PIN_C|PIN_D;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIO_STEP, &GPIO_InitStructure); 
//  GPIO_ResetBits(GPIO_STEP ,PIN_A|PIN_B|PIN_C|PIN_D );
//}

////顺时针转动
//void MotorCW(void)
//{
//	GPIO_ResetBits(GPIO_STEP, PIN_A);
//  GPIO_SetBits(GPIO_STEP, PIN_D);
//  delay_ms(L);//转速调节
//	GPIO_ResetBits(GPIO_STEP, PIN_D);
//	GPIO_SetBits(GPIO_STEP, PIN_C);
//  delay_ms(L);//转速调节
//	GPIO_ResetBits(GPIO_STEP, PIN_C);
//	GPIO_SetBits(GPIO_STEP, PIN_B);
//  delay_ms(L);//转速调节
//	GPIO_ResetBits(GPIO_STEP, PIN_B);
//	GPIO_SetBits(GPIO_STEP, PIN_A);
//  delay_ms(L);//转速调节
//}
////逆时针转动
//void MotorCCW(void)
//{GPIO_ResetBits(GPIO_STEP, PIN_D);
//  GPIO_SetBits(GPIO_STEP, PIN_A);
//  delay_ms(L);//转速调节
//	GPIO_ResetBits(GPIO_STEP, PIN_A);
//	GPIO_SetBits(GPIO_STEP, PIN_B);
//  delay_ms(L);//转速调节
//	GPIO_ResetBits(GPIO_STEP, PIN_B);
//	GPIO_SetBits(GPIO_STEP, PIN_C);
//  delay_ms(L);//转速调节
//	GPIO_ResetBits(GPIO_STEP, PIN_C);
//	GPIO_SetBits(GPIO_STEP, PIN_D);
//  delay_ms(L);//转速调节
//}
////停止转动
//void MotorStop(void)
//{
//	GPIO_ResetBits(GPIO_STEP, PIN_A);
//	GPIO_ResetBits(GPIO_STEP, PIN_B);
//	GPIO_ResetBits(GPIO_STEP, PIN_C);
//	GPIO_ResetBits(GPIO_STEP, PIN_D);
//}
