/**
 ***************************************************************************************************
 * @file    pwm_ctrl.h
 * @brief   PWM Control
 ***************************************************************************************************
 **************************************************************************************************/
#ifndef PWM_H
#define PWM_H

#ifdef PWM_GLOBAL
#define PWM_EXTERN
#else
#define PWM_EXTERN extern
#endif

/*--------------------------------------------------------------------------------------------------
- Function Declaration
--------------------------------------------------------------------------------------------------*/
PWM_EXTERN void pwm0_start(uint16_t duty, bool dir);
PWM_EXTERN void pwm1_start(uint16_t freq);
PWM_EXTERN void pwm0_stop(void);
PWM_EXTERN void pwm1_stop(void);


#endif /* PWM_H */