/**
 ***************************************************************************************************
 * @file 		ad_ctrl.h
 * @brief 		AD Control
 ***************************************************************************************************
 **************************************************************************************************/
#ifndef AD_CTRL_H_ /* AD_CTRL_H_ */
#define AD_CTRL_H_

/**
 * @brief	extern declaration
 */
#ifdef AD_CTRL_GLOBAL
#define AD_CTRL_EXTERN
#else
#define AD_CTRL_EXTERN extern
#endif

/*--------------------------------------------------------------------------------------------------
- Function Declaration
--------------------------------------------------------------------------------------------------*/
AD_CTRL_EXTERN void saadc_init(void);
AD_CTRL_EXTERN void saadc0_init(void);
AD_CTRL_EXTERN void saadc1_init(void);
AD_CTRL_EXTERN void saadc_stop(void);

AD_CTRL_EXTERN void saadc_sampling_event_init(void);
AD_CTRL_EXTERN void saadc_sampling_event_enable(void);


#endif /* AD_CTRL_H_ */
/***************************************************************************************************
 **************************************************************************************************/