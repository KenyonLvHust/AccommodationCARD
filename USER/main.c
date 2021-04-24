#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "string.h"
#include "iic.h"
#include "oled.h"
#include "STEP.h"

#define N 7

/*********************
���߷�ʽ:
IN1 A---- PA1
IN2 B---- PA2
IN3 C---- PA3
IN4 D---- PA4
+   ---- +5V
-   ---- GND
*********************/

#define uchar unsigned char
#define uint  unsigned int
	
#define MotorData P0                    //����������ƽӿڶ���
uchar phasecw[4] ={0x08,0x04,0x02,0x01};//��ת �����ͨ���� D-C-B-A
uchar phaseccw[4]={0x01,0x02,0x04,0x08};//��ת �����ͨ���� A-B-C-D

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
	delay_init();//��ʱ������ʼ��	 

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);//���ڳ�ʼ��Ϊ9600
	
	LED_GPIO_Config();
//	LOCK_GPIO_Config();
//	STEP_GPIO_Config();
	
	TIM3_PWM_Init(199,7199);	 //(199+1)*(7199+1)/72*10^6
  //����һ�����0.02s,��20ms
	
	delay_ms(190);
	TIM_SetCompare2(TIM3, 190);//90��
		
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
�˴���90��Ϊ����

��PWM����Ϊ20ms������ռ�ձȾ�Ӧ��Ϊ1.5ms/20ms = 7.5%��
����TIM_SetCompare2�� TIMx ����Ƚ� 1 �Ĵ���ֵ��Ϊ200-200*7.5% = 185
*/
int ISopen=0;
void Open(char* ID_get)
{
   ISopen=look_for(ID_get);
   if(ISopen==1)
   {
    delay_ms(195);
		TIM_SetCompare2(TIM3, 193);//0��
		GPIO_ResetBits(GPIO_LED, PIN_LED);
		 
    delay_ms(3000);
		 
		delay_ms(190);
		TIM_SetCompare2(TIM3, 190);//90��
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

 /***************  ����LED�õ���I/O�� *******************/
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
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
 	LED_Init();		 //LED�˿ڳ�ʼ��
	KEY_Init();
	 
 	TIM3_PWM_Init(199,7199);	 //(199+1)*(7199+1)/72*10^6
      //����һ�����0.02s,��20ms
   	while(1)
	{		
     if (KEY0==0)
	{
		delay_ms(195);
		TIM_SetCompare2(TIM3, 195);//0��
		LED1=0;
	}

	if(KEY1==0)
	{
		delay_ms(190);
		TIM_SetCompare2(TIM3, 190);//90��
	}
	if(WK_UP==1)
	{
		delay_ms(10);
		TIM_SetCompare2(TIM3, 185);//90��
	}		
	}	 
 }

//------------------------------------------------------------
// t = 0.5ms������������-�����ת�� 0 ��
//t = 1.0ms������������-�����ת�� 45��
//t = 1.5ms������������-�����ת�� 90��
//t = 2.0ms������������-�����ת�� 135��
//t = 2.5ms������������-�����ת��180��
��������������������������������
��Ȩ����������ΪCSDN������-electronic-engineer����ԭ�����£���ѭCC 4.0 BY-SA��ȨЭ�飬ת���븽��ԭ�ĳ������Ӽ���������
ԭ�����ӣ�https://blog.csdn.net/qq_45941706/article/details/108951250
*/

// /***************  ���õ���õ���I/O�� *******************/
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

////˳ʱ��ת��
//void MotorCW(void)
//{
//	GPIO_ResetBits(GPIO_STEP, PIN_A);
//  GPIO_SetBits(GPIO_STEP, PIN_D);
//  delay_ms(L);//ת�ٵ���
//	GPIO_ResetBits(GPIO_STEP, PIN_D);
//	GPIO_SetBits(GPIO_STEP, PIN_C);
//  delay_ms(L);//ת�ٵ���
//	GPIO_ResetBits(GPIO_STEP, PIN_C);
//	GPIO_SetBits(GPIO_STEP, PIN_B);
//  delay_ms(L);//ת�ٵ���
//	GPIO_ResetBits(GPIO_STEP, PIN_B);
//	GPIO_SetBits(GPIO_STEP, PIN_A);
//  delay_ms(L);//ת�ٵ���
//}
////��ʱ��ת��
//void MotorCCW(void)
//{GPIO_ResetBits(GPIO_STEP, PIN_D);
//  GPIO_SetBits(GPIO_STEP, PIN_A);
//  delay_ms(L);//ת�ٵ���
//	GPIO_ResetBits(GPIO_STEP, PIN_A);
//	GPIO_SetBits(GPIO_STEP, PIN_B);
//  delay_ms(L);//ת�ٵ���
//	GPIO_ResetBits(GPIO_STEP, PIN_B);
//	GPIO_SetBits(GPIO_STEP, PIN_C);
//  delay_ms(L);//ת�ٵ���
//	GPIO_ResetBits(GPIO_STEP, PIN_C);
//	GPIO_SetBits(GPIO_STEP, PIN_D);
//  delay_ms(L);//ת�ٵ���
//}
////ֹͣת��
//void MotorStop(void)
//{
//	GPIO_ResetBits(GPIO_STEP, PIN_A);
//	GPIO_ResetBits(GPIO_STEP, PIN_B);
//	GPIO_ResetBits(GPIO_STEP, PIN_C);
//	GPIO_ResetBits(GPIO_STEP, PIN_D);
//}
