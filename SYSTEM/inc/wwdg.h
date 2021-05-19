#ifndef __WWDG_H
#define __WWDG_H

#ifdef __cplusplus
extern "C" {
#endif
	
void wwdg_init(void);
void feed_wwdg(void);
void sys_restart(void);

#ifdef __cplusplus
}
#endif

#endif /*__WWDG_H*/
