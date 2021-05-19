#include "delay.h"
#include "wwdg.h"

/******************************************************************************

* @ SysTick��ʱ�� ��ؿ��ƼĴ���˵��

@ 1��SysTick���Ƽ�״̬�Ĵ�������ַ��0xE000_E010����λֵΪ0

	bit16 COUNTFLAG(R��  -> ������ϴζ�ȡ���Ĵ�����SysTick�Ѿ�������0�����λΪ1�������ȡ��λ����λ���Զ�����
	bit2  CLKSOURCE(R/W) -> 0=�ⲿʱ��Դ(STCLK)��1=�ں�ʱ��(FCLK)
	bit1  TICKINT(R/W)   -> 1=SysTick������0ʱ����SysTick�쳣����,0=����0ʱ�޶��� 
	bit0  ENABLE(R/W)    -> SysTick��ʱ����ʹ��λ

@ 2��SysTick��װ����ֵ�Ĵ�������ַ��0xE000_E014����λֵΪ0

	[23:0] RELOAD(R/W) -> ����������ʱ��������װ�ص�ֵ

@ 3��SysTick��ǰ��ֵ�Ĵ�������ַ��0xE000_E018�� ��λֵΪ0

	[23:0] CURRENT(R/Wc) -> ��ȡʱ���ص�ǰ��������ֵ��д����ʹ֮���㣬ͬʱ���������SysTick ���Ƽ�״̬�Ĵ����е�COUNTFLAG��־

@ 4��SysTickУ׼��ֵ�Ĵ�������ַ��0xE000_E01C����λֵ: bit31δ֪��bit30δ֪��[23:0]Ϊ0

	bit32 NOREF(R)    -> 1=û���ⲿ�ο�ʱ�ӣ�STCLK�����ã���0=�ⲿ�ο�ʱ�ӿ���
	bit30 SKEW(R)     -> 1=У׼ֵ����׼ȷ��10ms��0=У׼ֵ��׼ȷ��10ms
	[23:0] TENMS(R/W) -> 10ms��ʱ���ڵ������ĸ�����оƬ�����Ӧ��ͨ��Cortex�\M3�������ź��ṩ����ֵ������ֵ�����㣬���ʾ�޷�ʹ��У׼����
	
******************************************************************************/ 
static uint16_t fac_us = 0;         // us��ʱ������

// Init clock & delay parameters
// SYSTICK��ʱ�ӹ̶�ΪHCLKʱ��
// SYSCLK:ϵͳʱ��
void delay_init()	 
{
	/* ����SysTick��ʱ����ʱ�ӷ�Ƶ��ȷ����װֵ */
	/* 8��Ƶʱ����8000��000��1��Ƶʱ����1000��000 */
#ifdef SYSCLOCK_DIV_8
	//ѡ���ⲿʱ�ӣ�HCLK/8
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	fac_us = SystemCoreClock / 8000000;	// Ϊϵͳʱ�ӵ�1/8
#else
	// ѡ��ʱ��Դ��ע�⣺��Ҫ��Ƶ������Ӱ��ADC
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	fac_us = SystemCoreClock / 1000000;
#endif
}								    

// ��ʱnus
// nusΪҪ��ʱ��us��.	
void delay_us(const uint32_t nus)
{
 	uint32_t temp;
	SysTick->LOAD = nus * fac_us;                       // ʱ�����	  		 
	SysTick->VAL = 0x00;                                // ��ռ�����
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;          // ��ʼ����	 
	do {
		temp = SysTick->CTRL;
	}
	while(temp&0x01 && !(temp&(1<<16)));                // �ȴ�ʱ�䵽��   
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;          // �رռ�����
	SysTick->VAL = 0X00;                                // ��ռ�����
	feed_wwdg();
}

// ��ʱnms
// ע��nms�ķ�Χ
// SysTick->LOADΪ24λ�Ĵ���,����, �����ʱΪ:
// nms<=0xffffff*8*1000/SYSCLK
// SYSCLK��λΪHz,nms��λΪms
// ��72M������, nms<=1864
void delay_ms(const uint16_t nms)
{
	/*
	uint32_t temp;
	SysTick->LOAD = 2000 * nms;
	SysTick->VAL = 0X00;
	SysTick->CTRL = 0X01;
	do {
		temp=SysTick->CTRL;
     	feed_wwdg();
	} while((temp&0x01) && (!(temp&(1<<16))));
	SysTick->CTRL = 0x00;
	SysTick->VAL = 0X00;
	*/
	
	uint16_t time_ms = nms;
	while(time_ms--) {
		delay_us(1000);
     	//feed_wwdg();
	}
}
