/**
 ***************************************************************************************************
 * @file    buzzer_ctrl.c
 * @brief   BUZZER Control
 ***************************************************************************************************
 **************************************************************************************************/
#ifndef BUZZER_H
#define BUZZER_H

#ifdef BUZZER_GLOBAL
#define BUZZER_EXTERN
#else
#define BUZZER_EXTERN extern
#endif

/*--------------------------------------------------------------------------------------------------
- Function Declaration
--------------------------------------------------------------------------------------------------*/
BUZZER_EXTERN const int music_bigclock_data[][2];
BUZZER_EXTERN void buzzer_setting(void);

#endif /* BUZZER_H */
/***************************************************************************************************
 **************************************************************************************************/