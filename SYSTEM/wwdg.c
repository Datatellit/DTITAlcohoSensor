#include "sys.h"
#include "wwdg.h"

#ifndef NDEBUG
#ifndef DEBUG_NO_WWDG
#define DEBUG_NO_WWDG
#endif
#endif

// Window Watchdog
#define WWDG_COUNTER                    0x7F
#define WWDG_WINDOW                     0x7F

uint8_t feedingDog = 0;

// Initialize Window Watchdog
void wwdg_init() {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	//WWDG_DeInit();
	WWDG_SetPrescaler(WWDG_Prescaler_8);
	WWDG_SetWindowValue(WWDG_WINDOW);
	WWDG_SetCounter(WWDG_COUNTER);
	feedingDog = 0;
#ifndef DEBUG_NO_WWDG  
	WWDG_Enable(WWDG_COUNTER);
#endif  
}

// Feed the Window Watchdog
void feed_wwdg(void) {
#ifndef DEBUG_NO_WWDG
 	if(feedingDog == 1) return;
	feedingDog = 1;
  	uint8_t cntValue = (WWDG->CR) & WWDG_COUNTER;
	if( cntValue < WWDG_WINDOW ) {
		WWDG_SetCounter(WWDG_COUNTER);
	}
	feedingDog = 0;
#endif  
}

void sys_restart(void)
{
    //Serial_PutString("restart...");
	WWDG->CR = 0x80;
}
