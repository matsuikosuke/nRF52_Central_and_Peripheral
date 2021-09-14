/**
 ***************************************************************************************************
 * @file    cpu_set.c
 * @brief   CPU Setting
 ***************************************************************************************************
 **************************************************************************************************/
#ifndef CPU_SET_H /* CPU_SET_H */
#define CPU_SET_H

#ifdef CPU_SET_GLOBAL
#define CPU_SET_EXTERN
#else
#define CPU_SET_EXTERN extern
#endif

#define WDT_RESTART()                                                                                                                                                                                                                                                                                      \
    NRF_WDT->RR[0] = WDT_RR_RR_Reload /**< WDT Reload命令                                                                                                                                                                                                                                                  \
/*--------------------------------------------------------------------------------------------------                                                                                                                                                                                                       \
- Function Declaration                                                                                                                                                                                                                                                                                     \
--------------------------------------------------------------------------------------------------*/
CPU_SET_EXTERN void cpu_init(void);
CPU_SET_EXTERN void forced_reset_check(void);


#endif /* CPU_SET_H */