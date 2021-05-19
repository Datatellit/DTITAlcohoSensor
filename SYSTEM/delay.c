#include "delay.h"
#include "wwdg.h"

/******************************************************************************

* @ SysTick定时器 相关控制寄存器说明

@ 1、SysTick控制及状态寄存器（地址：0xE000_E010）复位值为0

	bit16 COUNTFLAG(R）  -> 如果在上次读取本寄存器后，SysTick已经数到了0，则该位为1。如果读取该位，该位将自动清零
	bit2  CLKSOURCE(R/W) -> 0=外部时钟源(STCLK)。1=内核时钟(FCLK)
	bit1  TICKINT(R/W)   -> 1=SysTick倒数到0时产生SysTick异常请求,0=数到0时无动作 
	bit0  ENABLE(R/W)    -> SysTick定时器的使能位

@ 2、SysTick重装载数值寄存器（地址：0xE000_E014）复位值为0

	[23:0] RELOAD(R/W) -> 当倒数至零时，将被重装载的值

@ 3、SysTick当前数值寄存器（地址：0xE000_E018） 复位值为0

	[23:0] CURRENT(R/Wc) -> 读取时返回当前倒计数的值，写它则使之清零，同时还会清除在SysTick 控制及状态寄存器中的COUNTFLAG标志

@ 4、SysTick校准数值寄存器（地址：0xE000_E01C）复位值: bit31未知。bit30未知。[23:0]为0

	bit32 NOREF(R)    -> 1=没有外部参考时钟（STCLK不可用）。0=外部参考时钟可用
	bit30 SKEW(R)     -> 1=校准值不是准确的10ms。0=校准值是准确的10ms
	[23:0] TENMS(R/W) -> 10ms的时间内倒计数的格数。芯片设计者应该通过Cortex‐M3的输入信号提供该数值。若该值读回零，则表示无法使用校准功能
	
******************************************************************************/ 
static uint16_t fac_us = 0;         // us延时倍乘数

// Init clock & delay parameters
// SYSTICK的时钟固定为HCLK时钟
// SYSCLK:系统时钟
void delay_init()	 
{
	/* 根据SysTick定时器的时钟分频来确定重装值 */
	/* 8分频时除以8000‘000，1分频时除以1000’000 */
#ifdef SYSCLOCK_DIV_8
	//选择外部时钟，HCLK/8
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	fac_us = SystemCoreClock / 8000000;	// 为系统时钟的1/8
#else
	// 选择时钟源，注意：不要分频，以免影响ADC
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	fac_us = SystemCoreClock / 1000000;
#endif
}								    

// 延时nus
// nus为要延时的us数.	
void delay_us(const uint32_t nus)
{
 	uint32_t temp;
	SysTick->LOAD = nus * fac_us;                       // 时间加载	  		 
	SysTick->VAL = 0x00;                                // 清空计数器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;          // 开始倒数	 
	do {
		temp = SysTick->CTRL;
	}
	while(temp&0x01 && !(temp&(1<<16)));                // 等待时间到达   
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;          // 关闭计数器
	SysTick->VAL = 0X00;                                // 清空计数器
	feed_wwdg();
}

// 延时nms
// 注意nms的范围
// SysTick->LOAD为24位寄存器,所以, 最大延时为:
// nms<=0xffffff*8*1000/SYSCLK
// SYSCLK单位为Hz,nms单位为ms
// 对72M条件下, nms<=1864
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
