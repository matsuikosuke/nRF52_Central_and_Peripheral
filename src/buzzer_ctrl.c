/**
 ***************************************************************************************************
 * @file    buzzer_ctrl.c
 * @brief   BUZZER Control
 ***************************************************************************************************
 **************************************************************************************************/
#define BUZZER_GLOBAL
#include "common.h"

const int music_bigclock_data[][2] = {
    RE, 4,  /*	お	*/
    SO, 4,  /*	お	*/
    FAU, 2, /*	き	*/
    SO, 2,  /*	な	*/
    RAA, 4, /*	のっ	*/
    SO, 2,  /*	ぽ	*/
    RAA, 2, /*	の	*/
    SI, 2,  /*	ふ	*/
    SI, 2,  /*	る	*/
    DO, 2,  /*	ど	*/
    SI, 2,  /*	け	*/
    MI, 4,  /*	い	*/
    RAA, 2, /*	お	*/
    RAA, 2, /*	じ	*/
    SO, 4,  /*	い	*/
    SO, 2,  /*	さ	*/
    SO, 2,  /*	ん	*/
    FAU, 4, /*	の	*/
    MI, 2,  /*	と	*/
    FAU, 2, /*	け	*/
    SO, 10, /*	い	*/
    0, 2,
    //-1,	0,	/*	終了	*/

    RE, 2,  /*	ひゃ	*/
    RE, 2,  /*	く	*/
    SO, 4,  /*	ねん	*/
    FAU, 2, /*	い	*/
    SO, 2,  /*	つ	*/
    RAA, 4, /*	も	*/
    SO, 2,  /*	う	*/
    RAA, 2, /*	ご	*/
    SI, 4,  /*	い	*/
    DO, 2,  /*	て	*/
    SI, 2,  /*	い	*/
    MI, 4,  /*	た	*/
    RAA, 2, /*	ご	*/
    RAA, 2, /*	じ	*/
    SO, 4,  /*	ま	*/
    SO, 2,  /*	ん	*/
    SO, 2,  /*	の	*/
    FAU, 4, /*	と	*/
    MI, 2,  /*	け	*/
    FAU, 2, /*	い	*/
    SO, 10, /*	さ	*/
    0, 2,

    SO, 2,   /*	お	*/
    SI, 2,   /*	じ	*/
    RE, 4,   /*	い	*/
    SI, 2,   /*	さ	*/
    RAA, 2,  /*	ん	*/
    SO, 4,   /*	の	*/
    FAU, 2,  /*	う	*/
    SO, 2,   /*	ま	*/
    RAA, 2,  /*	れ	*/
    SO, 2,   /*	た	*/
    FAU, 2,  /*	あ	*/
    MI, 2,   /*	さ	*/
    RE, 4,   /*	に	*/
    SO, 2,   /*	か	*/
    SI, 2,   /*	っ	*/
    RE, 4,   /*	て	*/
    SI, 2,   /*	き	*/
    RAA, 2,  /*	た	*/
    SO, 4,   /*	と	*/
    FAU, 2,  /*	け	*/
    SO, 2,   /*	い	*/
    RAA, 10, /*	さ	*/
    0, 2,

    RE, 2,             /*	い	*/
    SO, 2,             /*	ま	*/
    SO, 2,             /*	は	*/
    0, 4, RAA, 2,      /*	もう	*/
    0, 2, 0, 4, SI, 2, /*	う	*/
    SI, 2,             /*	ご	*/
    DO, 2,             /*	か	*/
    SI, 2,             /*	な	*/
    MI, 4,             /*	い	*/
    RAA, 2,            /*	そ	*/
    RAA, 2,            /*	の	*/
    SO, 8,             /*	と	*/
    FAU, 8,            /*	け	*/
    SO, 8,             /*	い	*/
    0, 2,

    RE, 2,       /*	ひゃ	*/
    RE, 2,       /*	く	*/
    SO, 4,       /*	ねん	*/
    RE, 2,       /*	や	*/
    RE, 2,       /*	す	*/
    MI, 2,       /*	ま	*/
    RE, 2,       /*	ず	*/
    RE, 4,       /*	に	*/
    SI, 2,       /*	ちっく	*/
    0, 2, RE, 2, /*	たっく	*/
    0, 2, SI, 2, /*	ちっく	*/
    0, 2, RE, 2, /*	たっく	*/
    RE, 2,       /*	お	*/
    SO, 4,       /*	じー	*/
    RE, 2,       /*	さん	*/
    RE, 2,       /*	と	*/
    MI, 4,       /*	いっ	*/
    RE, 2,       /*	しょ	*/
    RE, 2,       /*	に	*/
    SI, 2,       /*	ちっく	*/
    0, 2, RE, 2, /*	たっく	*/
    0, 2, SI, 2, /*	ちっく	*/
    0, 2, RE, 2, /*	たっく	*/

    RE, 2,             /*	い	*/
    SO, 2,             /*	ま	*/
    SO, 2,             /*	は	*/
    0, 4, RAA, 2,      /*	もう	*/
    0, 2, 0, 4, SI, 2, /*	う	*/
    SI, 2,             /*	ご	*/
    DO, 2,             /*	か	*/
    SI, 2,             /*	な	*/
    MI, 4,             /*	い	*/
    RAA, 2,            /*	そ	*/
    RAA, 2,            /*	の	*/
    SO, 8,             /*	と	*/
    FAU, 8,            /*	け	*/
    SO, 12,            /*	い	*/
    0, 4,

    -1, 0 /*	終了	*/
};

const int music_open_data[][2] = {
    //	RE,	1,	/*	ひゃ	*/
    //	RE,	1,	/*	く	*/
    //	SO,	2,	/*	ねん	*/

    //	RE,	1,
    //	SO,	1,
    //	RE,	1,
    //	SO,	1,

    //	FAU,	1,
//    MI, 1, MI, 1, FAU, 2,
    //	MI,	1,

    //	MI,	1,
 
    MI, 2,
    0, 1, 
    MI, 2, 
    0, 3, 
    MI, 2, 

    0, 2, -1, 0, /*	終了	*/
};


const int music_close_data[][2] = {
    //DO, 1, MI, 1,
    MI, 3, MI, 3,
    //	DO,	1,	/*	い	*/
    //	RE,	1,
    //	DO,	2,	/*	い	*/
    //	MI,	2,	/*	い	*/
    //	FA,	2,	/*	ど	*/
    //	//SO,	2,	/*	ど	*/
    0, 2, -1, 0, /*	終了	*/
};


const int music_alart_data[][2] = {
    DO, 2,       /*	ど	*/
    DO, 2,       /*	ど	*/
    DO, 2,       /*	ど	*/
    0, 2, -1, 0, /*	終了	*/
};


const int music_error_data[][2] = {
    RE,	1,
    SO,	1,
    RE,	1,
    SO,	1,
    DO, 2,       /*	ど	*/
    0, 4, DO, 2, /*	ど	*/
    0, 4, DO, 2, /*	ど	*/
    0, 2, -1, 0, /*	終了	*/
};


const int music_inspection_data[][2] = {
    DO, 2,       /*	ど	*/
    0, 4, DO, 2, /*	ど	*/
    0, 4, DO, 2, /*	ど	*/
    0, 2, -1, 0, /*	終了	*/
};


void buzzer_setting(void)
{
    sys_timer_flag[BUZZER_SCALE_TIMER] = true;

    if (sys_timer_count[BUZZER_SCALE_TIMER] >= 2 * music_bigclock_data[music_dim][1])
    {
        // 次の音階をならす //
        sys_timer_count[BUZZER_SCALE_TIMER] = 0; //ブザー発報時間カウントクリア
        music_dim++;
        switch (buzzer_flag)
        {
            case MELODY_TEST:
                if (music_bigclock_data[music_dim][0] == -1)
                {
                    //      sys_timer_count[BUZZER_SCALE_TIMER] += 1;
                    //      music_dim = 0;
                    sys_timer_flag[BUZZER_SCALE_TIMER] = false;
                    music_dim = 0;
                    buzzer_flag = MELODY_STOP;
                }
                else
                {
                    // -1でないなら次の音階セット //
                    pwm1_start(music_bigclock_data[music_dim][0]);
                    // TDR00 = music_bigclock_data[music_dim][0];
                }
                break;

            case MELODY_OPEN:
                if (music_open_data[music_dim][0] == -1)
                {
                    sys_timer_flag[BUZZER_SCALE_TIMER] = false;
                    music_dim = 0;
                    buzzer_flag = MELODY_STOP;
                }
                else
                {
                    // -1でないなら次の音階セット //
                    pwm1_start(music_open_data[music_dim][0]);
                }
                break;

            case MELODY_CLOSE:
                if (music_close_data[music_dim][0] == -1)
                {
                    sys_timer_flag[BUZZER_SCALE_TIMER] = false;
                    music_dim = 0;
                    buzzer_flag = MELODY_STOP;
                }
                else
                {
                    // -1でないなら次の音階セット //
                    pwm1_start(music_close_data[music_dim][0]);
                }
                break;

            case MELODY_ALART:
                if (music_alart_data[music_dim][0] == -1)
                {
                    sys_timer_flag[BUZZER_SCALE_TIMER] = false;
                    music_dim = 0;
                    buzzer_flag = MELODY_STOP;
                }
                else
                {
                    // -1でないなら次の音階セット //
                    pwm1_start(music_alart_data[music_dim][0]);
                }
                break;

            case MELODY_ERROR:
                if (music_error_data[music_dim][0] == -1)
                {
                    sys_timer_flag[BUZZER_SCALE_TIMER] = false;
                    music_dim = 0;
                    buzzer_flag = MELODY_STOP;
                }
                else
                {
                    // -1でないなら次の音階セット //
                    pwm1_start(music_error_data[music_dim][0]);
                }
                break;

#if defined(INSPECTION_MODE)
            case MELODY_INSPECTION:
#else
            case MELODY_OPERATION:
#endif
                if (music_inspection_data[music_dim][0] == -1)
                {
                    sys_timer_flag[BUZZER_SCALE_TIMER] = false;
                    music_dim = 0;
                    //buzzer_flag = MELODY_STOP;
                }
                else
                {
                    // -1でないなら次の音階セット //
                    pwm1_start(music_inspection_data[music_dim][0]);
                }
                break;

            default:
                break;
        }
    }
}