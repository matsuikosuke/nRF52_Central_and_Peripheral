/*----------------------------------------------------------
  Debug
----------------------------------------------------------*/
#define WITHOUT_DFU //DFU無効化
//#define WITHOUT_SCAN //SCAN無効化
#define BUG_AND_TEMPORARY_STOP //暫定的にバグのある処理を無効化する

/*----------------------------------------------------------
  Device Information
----------------------------------------------------------*/
#define		FIRMWARE_VERSION		"1.00.00-o"

#define		MY_COMPANY_IDENTIFIER           0xFFFF
#define		SOFTDEVICE_VERSION		"0101"
#define		BOOT_VERSION		        "52"

#define		FIRMWARE_VERSION_LENGTH   (9)
#define		DEVICE_ID_LENGTH          (8)
#define		IMSI_LENGTH	          (15)
#define		SOFTDEVICE_VERSION_LENGTH (4)
#define		BOOT_VERSION_LENGTH       (2)

/*----------------------------------------------------------
  BLE COMMAND
----------------------------------------------------------*/

// indication
#define MAX_INDICATION_RETRY 3
#define INDICATION_STATUS   0x10
#define INDICATION_OPEN     0x20
#define INDICATION_LOCK     0x40
#define INDICATION_DEV_INFO  0x80
#define INDICATION_INSPECT  0x30

// max length
#define BLE_WRITE_DATA_LENGTH (NRF_SDH_BLE_GATT_MAX_MTU_SIZE-3)
#define BLE_INDICATION_DATA_LENGTH (NRF_SDH_BLE_GATT_MAX_MTU_SIZE-3)
#define BLE_NOTIFICATION_DATA_LENGTH (NRF_SDH_BLE_GATT_MAX_MTU_SIZE-3)
#define BLE_READ_DATA_LENGTH (32)
#define BLE_ALL_DATA_MAX (100)

#define INDICATION_LENGTH_SERVER_HELLO_TYPE (19)
#define INDICATION_LENGTH_APPLICATION_TYPE  (267)

// INDEX
#define BLE_WRITE_ALL_PACKET_INDEX (0)
#define BLE_WRITE_NUM_PACKET_INDEX (1)

#define BLE_REASSEMBLY_WRITE_TYPE_INDEX (0)
#define BLE_REASSEMBLY_WRITE_LEN_UP_INDEX (1)
#define BLE_REASSEMBLY_WRITE_LEN_DOWN_INDEX (2)

#define BLE_INDICATE_TYPE_ALL_NUM   (0)
#define BLE_INDICATE_TYPE_PKT_NUM   (1)
#define BLE_INDICATE_TYPE_INDEX     (0)
#define BLE_INDICATE_LEN_UP_INDEX   (1)
#define BLE_INDICATE_LEN_DOWN_INDEX (2)

#define BLE_SERVER_HELLO_INDICATE_DATA_START_INDEX (3)

#define BLE_APPLICATION_INDICATE_RANDOM_INDEX (3)
#define BLE_APPLICATION_INDICATE_DATA_START_INDEX (11)

/*----------------------------------------------------------
  Timer
----------------------------------------------------------*/
// If all packets cannot be received within 1 second, an error event occurs
#define TIMER_NUM  (3)

#define BLE_TEST_TIMER (0)
#define BLE_TEST_LIMIT TM_10SEC

#define BLE_SCAN_CYC_TIMER (1)
#define BLE_SCAN_CYC_LIMIT TM_10SEC

#define BUZZER_SCALE_TIMER (2)


#define APP_TIMER_PRESCALER		( 0 ) // Value of the RTC1 PRESCALER register
//#define APP_TIMER_MAX_TIMERS	( 3+1 ) // Maximum number of simultaneously created timers
#define APP_TIMER_OP_QUEUE_SIZE	( 10 )  // Size of timer operation queues

#define TM_STOP ( 0xFFFF )  // Timer Stop
#define TM_TMOUT  ( 0 )     // Timeout
#define TM_100MS  ( 2 )     // 50ms software timebase  : 100ms	
#define TM_300MS  ( 6 )     // 50ms software timebase  : 300ms	
#define	TM_500MS  ( 10 )    // 50ms software timebase  : 500ms
#define	TM_600MS  ( 12 )    // 50ms software timebase  : 600msec
#define	TM_1SEC   ( 20 )    // 50ms software timebase  : 1sec		
#define	TM_1500MS ( 30 )    // 50ms software timebase  : 1.5sec	
#define	TM_2SEC   ( 40 )    // 50ms software timebase  : 2sec		
#define	TM_3SEC   ( 60 )    // 50ms software timebase  : 3sec		
#define	TM_4SEC   ( 80 )    // 50ms software timebase  : 4sec	
#define	TM_4400MS ( 88 )    // 50ms software timebase  : 4400msec	
#define	TM_5SEC   ( 100 )   // 50ms software timebase  : 5sec
#define	TM_10SEC  ( 200 )   // 50ms software timebase  : 10sec
#define	TM_30SEC  ( 600 )   // 50ms software timebase  : 30sec
#define	TM_50SEC  ( 1000 )  // 50ms software timebase  : 50sec	
#define	TM_60SEC  ( 1200 )  // 50ms software timebase  : 60sec
#define	TM_2MIN   ( 2400 )  // 50ms software timebase  : 3min
#define	TM_3MIN   ( 3600 )  // 50ms software timebase  : 3min
#define	TM_5MIN   ( 6000 )  // 50ms software timebase  : 5min
#define	TM_10MIN   ( 12000 )  // 50ms software timebase  : 10min
#define	TM_20MIN   ( 24000 )  // 50ms software timebase  : 20min
#define	TM_29MIN   ( 34800 )  // 50ms software timebase  : 20min
#define	TM_30MIN   ( 36000 )  // 50ms software timebase  : 30min
#define	TM_50MIN   ( 60000 )  // 50ms software timebase  : 50min

#define MAIN_INTERVAL APP_TIMER_TICKS(50) // 50ms main interval
#define MOTOR_INTERVAL APP_TIMER_TICKS(1) // 1ms main interval
#define PWM_INTERVAL APP_TIMER_TICKS(10) // 10ms main interval

/*----------------------------------------------------------
  W25Q80
----------------------------------------------------------*/
#define WAIT_FOR_NEXT_W25CMD (3)
#define W25X_SectorErase 0xD8
#define FLASH_ID 0XC22014  //ィ莉カID
#define W25Q_Dummy_Byte 0XFF
#define BUSY_Flag  0x01
#define WEL_Flag  0x02

#define SPI_BUFSIZE 8 //SPI郛灘・、ァ蟆

#define W25Q_BUSY_MASK	0x01
#define W25Q_WEN_MASK	0x02

#define W25Q80DV_ID                   0xEF4014
#define W25Q_SECTOR_START             0x1000
#define W25Q_SECTOR_SIZE              0x0F00
#define W25Q_SECTOR_MAX               16
#define W25Q_BLOCK_START              0x10000
#define W25Q_BLOCK_SIZE               0x0FF00
#define W25Q_BLOCK_MAX                16
#define W25Q_PAGE_START               0x100
//#define W25Q_PAGE_SIZE                0xFF
#define W25Q_PAGE_MAX                 0x100

#define W25Q_REG_WRITE_ENABLE         0x06
#define W25Q_REG_WRITE_DISABLE        0x04
#define W25Q_REG_ReadStatus1          0x05
#define W25Q_REG_ReadStatus2          0x35
#define W25Q_REG_WriteStatus          0x01
#define W25Q_REG_PageProgram          0x02
#define W25Q_REG_SectorErase4KB       0x20
#define W25Q_REG_BlockErase32KB       0x52
#define W25Q_REG_BlockErase64KB       0xD8
#define W25Q_REG_ChipErase            0xC7
#define W25Q_REG_EraseProgramSuspend  0x75
#define W25Q_REG_EraseProgramResume   0x7A
#define W25Q_REG_Powerdown            0xB9
#define W25Q_REG_ReadData             0x03
#define W25Q_REG_FastRead             0x0B
#define W25Q_REG_ReleasePowerdown     0xAB
#define W25Q_REG_ManufacturerDevice   0x90
#define W25Q_REG_JEDEC_ID             0x9F
#define W25Q_REG_ReadUniqueID         0x4B
#define W25Q_REG_ReadSFDP             0x5A
#define W25Q_REG_EraseSecurity        0x44
#define W25Q_REG_ProgramSecurity      0x42
#define W25Q_REG_ReadSecurity         0x48
#define W25Q_REG_EnableReset          0x66
#define W25Q_REG_Reset                0x99


#define SERVER_PRIMARY_KEY            0
#define SERVER_SECONDARY_KEY          1
#define CHARI_ECDH_PUBLIC_KEY         2
#define CHARI_ECDH_PRIVATE_KEY        3
#define CHARI_PUBLIC_KEY              4
#define CHARI_PRIVATE_KEY             5
#define FLASH_CHECK_AREA              6

#define NUM_FROM_END_PUBLIC_KEY       18
#define NUM_FROM_END_PRIVATE_KEY      19

/*----------------------------------------------------------
  UART
----------------------------------------------------------*/
#define UART_TX_BUF_SIZE 256  /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256  /**< UART RX buffer size. */
#define RECEIVE_BUF_MAX 1200

#define UART_FAIL     0
#define UART_SUCCESS  1
#define UART_OK       1
#define UART_NONE    99



#define CHECK_CONFIG_SUCCESS      90
#define CHECK_CONFIG_FAIL         100

#define PUBLISH_NONE               0
#define PUBLISH_LIMIT              20
#define GPS_NONE                   0
#define GPS_LIMIT_ONCE             1
#define GPS_LIMIT_ACTIVE           15
#define GPS_LIMIT_LOWPOW           50
#define SUBSCRIBE_NONE             0
#define KEY_SUBSCRIBE_LIMIT        5
#define CYC_SUBSCRIBE_LIMIT        2
#define GPS_STOP                   50
#define PUBLISH_SUCCESS            100
#define SUBSCRIBE_SUCCESS          200

#define PUBLISH_UTC_LENGTH         24
#define PUBLISH_LATITUDE_LENGTH    9
#define PUBLISH_LONGITUDE_LENGTH   10

#define UTC_FAIL                   50
#define UTC_RETRY                  100

#define		QMTCONN_START     (95)
#define		QMTPUBEX_START    (40)
#define		QMTSUB_START      (34)

/*----------------------------------------------------------
  LIS3DH
----------------------------------------------------------*/
/* Common addresses definition for LIS3DH. */
#define LIS3DH_ADDR       0x19

/* Register address definition for LIS3DH. */
#define LIS3DH_REG_STATUS_REG_AUX	0x07
#define LIS3DH_REG_OUT_ADC1_L           0x08
#define LIS3DH_REG_OUT_ADC1_H           0x09
#define LIS3DH_REG_OUT_ADC2_L           0x0A
#define LIS3DH_REG_OUT_ADC2_H           0x0B
#define LIS3DH_REG_OUT_ADC3_L           0x0C
#define LIS3DH_REG_OUT_ADC3_H           0x0D
#define LIS3DH_REG_WHO_AM_I             0x0F
#define LIS3DH_REG_CTRL_REG0            0x1E
#define LIS3DH_REG_TEMP_CFG_REG         0x1F
#define LIS3DH_REG_CTRL_REG1            0x20
#define LIS3DH_REG_CTRL_REG2            0x21
#define LIS3DH_REG_CTRL_REG3            0x22
#define LIS3DH_REG_CTRL_REG4            0x23
#define LIS3DH_REG_CTRL_REG5            0x24
#define LIS3DH_REG_CTRL_REG6            0x25
#define LIS3DH_REG_REFERENCE            0x26
#define LIS3DH_REG_STATUS_REG           0x27
#define LIS3DH_REG_OUT_X_L              0x28
#define LIS3DH_REG_OUT_X_H              0x29
#define LIS3DH_REG_OUT_Y_L              0x2A
#define LIS3DH_REG_OUT_Y_H              0x2B
#define LIS3DH_REG_OUT_Z_L              0x2C
#define LIS3DH_REG_OUT_Z_H              0x2D
#define LIS3DH_REG_FIFO_CTRL_REG	0x2E
#define LIS3DH_REG_FIFO_SRC_REG         0x2F
#define LIS3DH_REG_INT1_CFG             0x30
#define LIS3DH_REG_INT1_SRC             0x31
#define LIS3DH_REG_INT1_THS             0x32
#define LIS3DH_REG_INT1_DURATION	0x33
#define LIS3DH_REG_INT2_CFG             0x34
#define LIS3DH_REG_INT2_SRC             0x35
#define LIS3DH_REG_INT2_THS             0x36
#define LIS3DH_REG_INT2_DURATION	0x37
#define LIS3DH_REG_CLICK_CFG            0x38
#define LIS3DH_REG_CLICK_SRC            0x39
#define LIS3DH_REG_CLICK_THS            0x3A
#define LIS3DH_REG_TIME_LIMIT           0x3B
#define LIS3DH_REG_TIME_LATENCY         0x3C
#define LIS3DH_REG_TIME_WINDOW          0x3D
#define LIS3DH_REG_ACT_THS              0x3E
#define LIS3DH_REG_ACT_DUR              0x3F

/*----------------------------------------------------------
  Mounting position
----------------------------------------------------------*/
#define CITYBIKE_POSITION_X   0xD0
#define CITYBIKE_POSITION_Y   0x00
#define CITYBIKE_POSITION_Z   0x25
#define ELCTBIKE_POSITION_X   0xD0
#define ELCTBIKE_POSITION_Y   0x00
#define ELCTBIKE_POSITION_Z   0x25

#define ACCE_THRESHOLD_VALUE  (10)

#if defined(ELCTBIKE_MODE)
#define		ACCE_THRESHOLD_LIMIT       (72000/ACTIVE_CYC_LIMIT_ELCTBIKE)    
#else
#define		ACCE_THRESHOLD_LIMIT       (72000/ACTIVE_CYC_LIMIT_CITYBIKE)   
#endif

/*----------------------------------------------------------
  MOTOR
----------------------------------------------------------*/
#define NORMAL_SPEED      0x4FFF
#define LOW_SPEED         0x05FF

#define OPEN_ST_LV        1
#define LOCK_ST_LV        0

#define OPEN_POS_BIT      0x01
#define LOCK_POS_BIT      0x02

#define LOCK_DIRECTION      true
#define OPEN_DIRECTION      false

#define LOCK_POSITION      true
#define OPEN_POSITION      false

/*----------------------------------------------------------
  BUZZER
----------------------------------------------------------*/
#define MELODY_STOP       0x00
#define MELODY_OPEN       0x01
#define MELODY_CLOSE      0x02
#define MELODY_ALART      0x03
#define MELODY_ERROR      0x04
#define MELODY_TEST       0x05
#define MELODY_INSPECTION 0x06
#define MELODY_OPERATION  0x07

/*----------------------------------------------------------
  ブザーメロディー
----------------------------------------------------------*/
#define		DO			478/4
#define		DOU			451/4
#define		RE			426/4
#define		REU			402/4
#define		MI			379/4
#define		FA			358/4
#define		FAU			338/4
#define		SO			319/4
#define		SOU			301/4
#define		RAA			284/4
#define		RAU			268/4
#define		SI			253/4

#define		const_duty music_bigclock_data[music_dim][0]

/*----------------------------------------------------------
  MODE
----------------------------------------------------------*/
#define MODE_ACTIVE       0x01
#define MODE_LOW_POWER    0x02
#define MODE_BATTERY_LOW  0x03

/*----------------------------------------------------------
  BATTERY
----------------------------------------------------------*/
#define BATTRY_LV1  0x01
#define BATTRY_LV2  0x02
#define BATTRY_LV3  0x03
#define BATTRY_LV4  0x04
#define BATTRY_LV5  0x05

#define IN_BATTRY_LV1_UP     0xA30

#define IN_BATTRY_LV2_UP     0xAD0
#define IN_BATTRY_LV2_DOWN   0xA30

#define IN_BATTRY_LV3_UP     0xBDA
#define IN_BATTRY_LV3_DOWN   0xAD0

#define IN_BATTRY_LV4_UP     0xCA0
#define IN_BATTRY_LV4_DOWN   0xBDA

#define IN_BATTRY_LV5_DOWN   0xCA0


#define EXT_BATTRY_LV1_UP     0xA70

#define EXT_BATTRY_LV2_UP     0xAD0
#define EXT_BATTRY_LV2_DOWN   0xA70

#define EXT_BATTRY_LV3_UP     0xBDA
#define EXT_BATTRY_LV3_DOWN   0xAD0

#define EXT_BATTRY_LV4_UP     0xCA0
#define EXT_BATTRY_LV4_DOWN   0xBDA

#define EXT_BATTRY_LV5_DOWN   0xCA0

/*----------------------------------------------------------
  BLE
----------------------------------------------------------*/
#define BLE_DISCONNECT  0x00
#define BLE_CONNECT     0x01


/*----------------------------------------------------------
  common
----------------------------------------------------------*/
#define EC_READY_RESET     0x0000
#define EC_POWER_ON        0x0001


/*----------------------------------------------------------
  observation
----------------------------------------------------------*/
#define OBSERVATION_SETTING       0x0002
#define OBSERVATION_RDY_ACCE      0x0004
#define OBSERVATION_RDY_BATT      0x0008
#define OBSERVATION_PUB_START     0x0010
#define OBSERVATION_PUB_END       0x0040


#define OBSERVATION_IGNORE       (OBSERVATION_SETTING | OBSERVATION_RDY_ACCE | OBSERVATION_RDY_BATT | OBSERVATION_PUB_START | OBSERVATION_PUB_END)

#define CYC_READY        (OBSERVATION_SETTING | OBSERVATION_RDY_ACCE | OBSERVATION_RDY_BATT | EC_POWER_ON)
#define CYC_DONE         (CYC_READY | OBSERVATION_PUB_START | OBSERVATION_PUB_END)
#define CYC_DOING        (CYC_READY | OBSERVATION_PUB_START)


/*----------------------------------------------------------
  registration
----------------------------------------------------------*/
#define REGISTRATION_SETTING           0x0080
#define REGISTRATION_PUBSUB_KEY_START  0x0100
#define REGISTRATION_PUB_END           0x0200
#define REGISTRATION_SUB_END           0x0400

#define REGISTRATION_PUBSUB_KEY_READY (REGISTRATION_SETTING | EC_POWER_ON)
#define REGISTRATION_PUBSUB_KEY_DONE  (REGISTRATION_PUBSUB_KEY_READY | REGISTRATION_PUBSUB_KEY_START | REGISTRATION_PUB_END | REGISTRATION_SUB_END)

#define KEY_GENERATION_SUCCESS  0x0000
#define KEY_GENERATION_ERROR    0x5555
#define KEY_GENERATION_DOING    0xFFFF

/*----------------------------------------------------------
  registration
----------------------------------------------------------*/
#define GET_KEY_SETTING      0x0800
#define GET_KEY_START        0x1000
#define GET_KEY_END          0x2000
#define DOWNLOAD_XTRA        0x4000

#define GET_KEY_READY (GET_KEY_SETTING | EC_POWER_ON)
#define GET_KEY_DONE  (GET_KEY_READY | GET_KEY_START | GET_KEY_END | DOWNLOAD_XTRA)


#define KEY_GETTING_SUCCESS     0x0000
#define KEY_GETTING_DOING       0xFFFF;


/*----------------------------------------------------------
  ec check
----------------------------------------------------------*/
//#define EC_CHECK_END       0x0002
//
//#define EC_CHECK_READY  (EC_POWER_ON)
//#define EC_CHECK_DONE   (EC_CHECK_READY | EC_CHECK_END)

/*----------------------------------------------------------
  RSA/ECDH/OAEP
----------------------------------------------------------*/
#define RSA_IV_SIZE (4)
#define HKDF_SEED_KEY_SIZE (256)//(128)
#define RSA_HKDF_KEY_SIZE  (32)
#define ECDH_KEY_SIZE  (32)
#define SHARED_KEY_SIZE  (32)
#define RSA_GCM_INOUT_SIZE (512)//(256)//0x01cd//
#define CLIENT_HELLO_SIZE (259)
#define DECRYPT_CLIENT_HELLO_SIZE (136)
#define SERVER_HELLO_DATA_SIZE (128)
#define JWT_SIZE (500)
#define DUMMY_ENTROPY_THRESHOLD (32) //(134)
#define GENERATE_KEY_ENTROPY_THRESHOLD (32)
#define RSA_GCM_IV_SIZE (12)
#define RSA_GCM_RANDOM_SIZE (8)
#define RSA_GCM_TAG_SIZE (16)
#define NONCE_SIZE (16)

#define IV_HASH_BYTE_SIZE (2)
#define IV_HASH_BYTE_1 0x69
#define IV_HASH_BYTE_2 0x76
#define SECRET_HASH_BYTE_SIZE (3)
#define SECRET_HASH_BYTE_1 0x6b
#define SECRET_HASH_BYTE_2 0x65
#define SECRET_HASH_BYTE_3 0x79

/*----------------------------------------------------------
  Protocol Buf
----------------------------------------------------------*/
#define PROTOBUF_LENGTH_MAX (500)

/*----------------------------------------------------------
  Publish Event Reason
----------------------------------------------------------*/
#define EVENT_REASON_UNLOCKED_BY_USER_APP       (1)
#define EVENT_REASON_UNLOCKED_BY_OPERATION_APP  (2)
#define EVENT_REASON_LOCKED                     (3)

/*----------------------------------------------------------
  Publish Event Reason
----------------------------------------------------------*/
#define OPEN_EC_EVENT                          (1)
#define BATTERY_LOW_OPEN_EC_EVENT              (2)
#define USER_OPEN_EC_EVENT                     (3)
#define OPERATION_OPEN_EC_EVENT                (4)
#define BATTERY_LOW_USER_OPEN_EC_EVENT         (5)
#define BATTERY_LOW_OPERATION_OPEN_EC_EVENT    (6)
#define CLOSE_EC_EVENT                         (7)


/*----------------------------------------------------------
  EC25J STATUS
----------------------------------------------------------*/
#define EC_NO_PROBLEM              (0)
#define EC_HARDWARE_ERROR          (1)
#define EC_RADIO_ERROR             (2)
#define EC_SIM_ERROR               (3)
#define EC_IMSI_ERROR              (4)
#define EC_NET_REG_ERROR           (5)
#define EC_OPEN_ERROR              (6)

/*----------------------------------------------------------
  1984
----------------------------------------------------------*/
#define FLASH_CHECK "Two gin-scented tears trickled down the sides of his nose."
#define FLASH_CHECK_LENGTH (58)