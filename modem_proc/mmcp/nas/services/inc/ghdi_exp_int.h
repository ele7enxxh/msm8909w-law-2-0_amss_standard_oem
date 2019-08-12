#ifndef GHDI_EXP_INT_H
#define GHDI_EXP_INT_H
/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/ghdi_exp.h_v   1.9   17 Jul 2002 11:52:16   kishores  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/inc/ghdi_exp_int.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/26/04   ab      Added GSM GHDI_TURN_SPEECH_TCH_AMR_ON and
                     GHDI_TURN_SPEECH_AMR_OFF.
05/11/01   jca     Fixed Lint errors.
06/11/01   TS      Added GSM build support.
08/15/01   ab      Renamed all MMI references to CM
09/18/01   sbk     NV related functionality added and prototypes declared
01-Apr-2002  VT    GSM EFR: Added GHDI_TURN_SPEECH_TCH_EF_ON and 
                     GHDI_TURN_SPEECH_TCH_EF_OFF to the enum related to 
                     ghdi_tch_control_T.
                   Actually merged the above changes from rev 1.7.1.0 (branch) 
                     of this file.
05/01/02   rmc     Added ghdi_read_ue_revision_level function
===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "sys_stru_v.h"
#include "sys_type.h"
#include "sys.h"

#ifdef FEATURE_VOC_AMR_WB
#include "sys_cnst_v.h"
#endif /*end of FEATURE_VOC_AMR_WB */
#include "ghdi_exp_v.h"

#define GHDI_INVALID_DATA              2
#define GHDI_NOT_INITIALISED           3
#define GHDI_MESSAGE_TOO_BIG           4
#define GHDI_BUFFER_FULL               5
#define GHDI_BUFFER_EMPTY              6
#define GHDI_BREAK                     7
#define GHDI_READONLY                  8
#define GHDI_NOT_REGISTERED            9

enum
{
   GHDI_NVMEM_PARAM_DTMF_ENABLED,
   GHDI_NVMEM_PARAM_NET_SEL_MODE,
   GHDI_NVMEM_PARAM_BA,
   GHDI_NVMEM_MULTIBAND_CONFIG

};


typedef byte ghdi_ue_op_class_T;
enum
{
   ue_op_class_a = 1,
   ue_op_class_b,
   ue_op_class_c
};

typedef byte ghdi_rat_capability_T;
enum
{
   ue_gsm_only_rat = 1,
   ue_umts_only_rat,
   ue_gsm_umts_rat
};

/* the SMS-Value in the MS Radio Access Capabilities IE */ 
typedef byte ghdi_ue_msrac_sms_value_T;

/* the SM-Value in the MS Radio Access Capabilities IE */ 
typedef byte ghdi_ue_msrac_sm_value_T;

typedef byte                           ghdi_lcd_icon_id_T;
enum
{
/* GHDI_ICON_BAT_WARN must be the first in the enum. */
   GHDI_ICON_BAT_WARN,
   GHDI_ICON_BAT0,
   GHDI_ICON_BAT1,
   GHDI_ICON_BAT2,
   GHDI_ICON_BAT3,
   GHDI_ICON_BAT4,
   GHDI_ICON_BAT5,
   GHDI_ICON_SIG0,
   GHDI_ICON_SIG1,
   GHDI_ICON_SIG2,
   GHDI_ICON_SIG3,
   GHDI_ICON_SIG4,
   GHDI_ICON_SIG5,
   GHDI_ICON_IN_CALL,
   GHDI_ICON_ROAM,
   GHDI_ICON_REGISTERED,
   GHDI_ICON_NEW_SHORT_MESSAGE,
   GHDI_ICON_NEW_VOICE_MAIL
/* GHDI_ICON_NEW_VOICE_MAIL must be the last in the enum. */
};

#define GHDI_MIN_ICON_ID               ((int)GHDI_ICON_BAT_WARN)
#define GHDI_MAX_ICON_ID               ((int)GHDI_ICON_NEW_VOICE_MAIL)
#define GHDI_IS_VALID_ICON_ID(icon_id) \
        (((int)icon_id) >= GHDI_MIN_ICON_ID \
      && ((int)icon_id) <= GHDI_MAX_ICON_ID)

typedef byte                           ghdi_icon_operation_T;
enum
{
   GHDI_ICON_OP_SET,
   GHDI_ICON_OP_FLASH,
   GHDI_ICON_OP_CLEAR
};

typedef byte                           ghdi_tone_id_T;
enum
{
   GHDI_TONE_DTMF_1,
   GHDI_TONE_DTMF_2,
   GHDI_TONE_DTMF_3,
   GHDI_TONE_DTMF_4,
   GHDI_TONE_DTMF_5,
   GHDI_TONE_DTMF_6,
   GHDI_TONE_DTMF_7,
   GHDI_TONE_DTMF_8,
   GHDI_TONE_DTMF_9,
   GHDI_TONE_DTMF_0,
   GHDI_TONE_DTMF_A,
   GHDI_TONE_DTMF_B,
   GHDI_TONE_DTMF_C,
   GHDI_TONE_DTMF_D,
   GHDI_TONE_DTMF_STAR,
   GHDI_TONE_DTMF_HASH,
   GHDI_TONE_DIAL,
   GHDI_TONE_SUBSCRIBER_BUSY,
   GHDI_TONE_CONGESTION,
   GHDI_TONE_RADIO_PATH_ACK,
   GHDI_TONE_RADIO_PATH_NA,
   GHDI_TONE_ERROR,
   GHDI_TONE_CW,
   GHDI_TONE_RINGING,
   GHDI_TONE_RECALL_DIAL,
   GHDI_TONE_OFF_HOOK,
   GHDI_TONE_REMOTE_ALERT,
   GHDI_TONE_CONFIRMATION,
   GHDI_TONE_VALID_KEYPRESS,
   GHDI_TONE_INVALID_KEYPRESS,
   GHDI_TONE_LOW_BAT_EARPIECE,
   GHDI_TONE_LOW_BAT_RINGER,
   GHDI_TONE_CORRUPT,
   GHDI_TONE_FAILURE,
   GHDI_TONE_ALL
};



typedef byte                           ghdi_umts_voice_control_T;
enum
{
   GHDI_TURN_AMR_ON = 0,    /* UMTS/WCDMA AMR */
   GHDI_TURN_AMR_OFF
};

typedef byte                           ghdi_key_id_T;
enum
{
   GHDI_KEY_NUM = 0,
   GHDI_KEY_STAR,
   GHDI_KEY_HASH,
   GHDI_KEY_PWR,
   GHDI_KEY_UP,
   GHDI_KEY_DOWN,
   GHDI_KEY_LEFT,
   GHDI_KEY_RIGHT,
   GHDI_KEY_CLR,
   GHDI_KEY_SEND,
   GHDI_KEY_END,
   GHDI_KEY_PHONEBOOK,
   GHDI_KEY_VOLUME_INC,
   GHDI_KEY_VOLUME_DEC,
   GHDI_KEY_PLUS
};

#define GHDI_KEY_MUTE                  GHDI_KEY_UP
#define GHDI_KEY_MENU                  GHDI_KEY_DOWN
#define GHDI_KEY_PAUSE                 GHDI_KEY_RIGHT
#define GHDI_KEY_OK                    GHDI_KEY_SEND
#define GHDI_KEY_NO                    GHDI_KEY_END

typedef byte                           ghdi_key_action_T;
enum
{
   GHDI_KEY_PRESSED = 0,
   GHDI_KEY_RELEASED,
   GHDI_KEY_REPEAT
};

#define GHDI_MAX_LCD_TEXT_CHARS        16

typedef byte                           ghdi_battery_level_T;
#define GHDI_MAX_BATTERY_LEVEL         100

typedef byte ghdi_battery_warning_T;
enum
{
   GHDI_BATTERY_NO_WARNING = 0,
   GHDI_BATTERY_ALMOST_EMPTY,
   GHDI_BATTERY_EMPTY
};

typedef struct 
{
   boolean GEA_1_supported;
   boolean GEA_2_supported;
   boolean GEA_3_supported;
   boolean GEA_4_supported;
   boolean GEA_5_supported;
   boolean GEA_6_supported;
   boolean GEA_7_supported;
} gprs_hw_classmark_T;

   

typedef byte ghdi_sim_isr_cause_T;
enum
{
   GHDI_SIM_OUT = 0,
   GHDI_SIM_IN = 1
};

/*
** CLK SPEED OF SIM REQUIRED
*/
typedef enum
{
    CLK_AT_LEAST_1000_KHZ = 0,   /* min allowed freq                        */
    CLK_STOP_ANY_LEVEL    = 1,   /* stop clock at any level                 */
    CLK_STOP_HIGH         = 4,   /* stop clock at high level                */
    CLK_STOP_HIGH_PREF    = 5,   /* stop clock at any level, high preferred */
    CLK_STOP_LOW          = 8,   /* stop clock at low level                 */
    CLK_STOP_LOW_PREF     = 9,   /* stop clock at any level, low preferred  */
    CLK_AT_LEAST_1625_KHZ = 16,  /* 13/8 Mhz                                */
    CLK_AT_LEAST_3250_KHZ = 17   /* 13/4 Mhz                                */
}
clk_state_T;


/*
** VPP STATE FOR SMART-CARD'S VPP PIN
** note - for GSM smart cards, this parameter is not used.
*/

typedef enum
{
   GHDI_VPP_ACTIVE,
   GHDI_VPP_IDLE,
   GHDI_VPP_NO_CHANGE
}
vpp_state_T;

typedef enum
{
   SIM_3_VOLTS = 3,
   SIM_5_VOLTS = 5
}
sim_voltage_T;

#define MAX_ATR_LEN  33   /* see ISO 7816-3 & GSM 11.11 */

typedef enum
{
   F144,  /* 8 x 36 bit frames in 20ms */
   F96,   /* 4 x 60 bit frames in 20ms */
   F48,   /* 2 x 60 bit frames in 20ms */
   F24,   /* 2 x 36 bit frames in 20ms */
   H48,    /* 2 x 60 bit frames in 20ms */
   H24    /* 2 x 36 bit frames in 20ms */
}
radio_data_format_T;


extern const char * dai_mode_name[];

extern const char * loopback_type_name[];

/*  Serial IO buffers */
#ifdef HOST
#define GHDI_SERIAL_OP_BUFF_SIZE   2048
#define GHDI_SERIAL_IP_BUFF_SIZE   2048
#else
#ifdef MS_CONFIGURATION_P63
#error code not present
#else
#define GHDI_SERIAL_OP_BUFF_SIZE   2048
#define GHDI_SERIAL_IP_BUFF_SIZE   2048
#endif
#endif


typedef struct
{
   word      buffer[GHDI_SERIAL_OP_BUFF_SIZE];
   int       input_data_pointer;
   int       output_data_pointer;
   boolean   port_busy;
}
ghdi_serial_op_buffer_T;

typedef struct
{
   word      buffer[GHDI_SERIAL_IP_BUFF_SIZE];
   int       input_data_pointer;
   int       output_data_pointer;
}
ghdi_serial_ip_buffer_T;

/* SERIAL PORT CONTROL PINS FOR PROJECT P63 */
#define S0_RTS_PORT             P3
#define S0_RTS_DPORT            DP3
#define S0_RTS_PIN              7
#define S0_RTS_PIN_MASK         0x0080     /* 0000 0000 1000 0000 */
#define S0_CTS_PORT             P2
#define S0_CTS_DPORT            DP2
#define S0_CTS_PIN              11
#define S0_CTS_PIN_MASK         0x0800     /* 0000 1000 0000 0000 */
#define S0_DSR_PORT             P2
#define S0_DSR_DPORT            DP2
#define S0_DSR_PIN              10
#define S0_DSR_PIN_MASK         0x0400     /* 0000 0100 0000 0000 */
#define S0_DCD_PORT             P2
#define S0_DCD_DPORT            DP2
#define S0_DCD_PIN              9
#define S0_DCD_PIN_MASK         0x0200     /* 0000 0010 0000 0000 */
#define S0_DTR_PORT             P3
#define S0_DTR_DPORT            DP3
#define S0_DTR_PIN              5
#define S0_DTR_PIN_MASK         0x0020     /* 0000 0000 0010 0000 */
#define S0_RI_PORT              P3
#define S0_RI_DPORT             DP3
#define S0_RI_PIN               6
#define S0_RI_PIN_MASK          0x0040     /* 0000 0000 0100 0000 */

/* Serial fifo bit significance */
#ifdef HOST
#define SER_IO_DATA       256
#define SER_IO_CTS        256
#define SER_IO_RTS        256
#define SER_IO_NO_DATA    512
#define SER_IO_DSR       1024
#define SER_IO_DTR       1024
#define SER_IO_RI        2048
#define SER_IO_DCD       4096
#define SER_IO_UNDR_SPD  8192
#define SER_IO_OVR_SPD  16384
#define SER_IO_BREAK    32768

#elif defined MS_CONFIGURATION_P63
#error code not present
#else  /* default = P63 */
#define SER_IO_CTS_DSR_DCD  0x0E00
#define SER_IO_CTS          S0_CTS_PIN_MASK /* match to the actual port pin */
#define SER_IO_DSR          S0_DSR_PIN_MASK /* match to the actual port pin */
#define SER_IO_DCD          S0_DCD_PIN_MASK /* match to the actual port pin */
#define SER_IO_DATA         0x0100
#define SER_IO_RI           0x0100   /* actual value not important */
#define SER_IO_DTR          0x0400   /* actual value not important */
#define SER_IO_RTS          0x0800   /* actual value not important */
#define SER_IO_NO_DATA      0x1000   /* actual value not important */
#define SER_IO_UNDR_SPD     0x2000   /* actual value not important */
#define SER_IO_OVR_SPD      0x4000   /* actual value not important */
#define SER_IO_BREAK        0x8000   /* actual value not important */
#endif

/*
 * Message definitions. (Message set MS_GHDI_EXPORT).
 ****************************************************
 */

#define MID_GHDI_CM_KEYPAD_IND        0x01
#define GHDI_CM_KEYPAD_IND            MID_GHDI_CM_KEYPAD_IND

typedef struct
{
   IMH_T                               IMH;
   ghdi_key_action_T                   key_action;
   ghdi_key_id_T                       key_id;
   char                                key_char;
} ghdi_cm_keypad_ind_T;


#define GHDI_BAT_IND                   0x02
#define MID_CM_BAT_IND                GHDI_BAT_IND   /* old name */
#define MID_GHDI_CM_BAT_IND           GHDI_BAT_IND   /* old name */
#define GHDI_CM_BAT_IND               GHDI_BAT_IND   /* old name */

typedef struct
{
   IMH_T                               IMH;
   ghdi_battery_level_T                battery_level;
   ghdi_battery_warning_T              battery_warning;
} ghdi_bat_ind_T;

typedef ghdi_bat_ind_T ghdi_cm_bat_ind_T;   /* old name */


#define GHDI_SIM_ISR_IND               0x10

typedef struct
{
   IMH_T                               message_header;
   ghdi_sim_isr_cause_T                cause;
}
ghdi_sim_isr_ind_T;

typedef struct
{
   IMH_T                               message_header;
}
ghdi_clasmark_changed_ind;


/*Data Services definitions*/

#define GHDI_SERIALDATA     0x81
#define GHDI_CODERDATA     0x82
#define MAX_SERIAL_DATA_LEN  16

typedef struct
{
   IMH_T                               message_header;
   byte                                data_length;
   byte                                data[MAX_SERIAL_DATA_LEN];
   byte                                flags;
}
ghdi_serial_data_T;

typedef struct
{
   IMH_T                               message_header;
   byte                                data_format;
   byte                                data[9][8];
}
ghdi_coder_data_T;

typedef  struct
{
   byte                                flag;
   ghdi_coder_data_T                   message;
}
ghdi_coder_test_buff_T;


/*data type to/ from channel coder*/
typedef byte ghdi_radio_data_T;

/*data type for V24 interface*/
typedef byte ghdi_async_data_word_T;

typedef struct
{
   byte               CTS;
   byte               DSR;
   byte               DCD;
   byte               BREAK;   /* used on serial output only */
   byte               DATA_INVALID;
   byte               RI;
} ghdi_async_control_T;

typedef struct
{
   byte          RTS;
   byte          DTR;
} ghdi_async_status_T;


typedef byte v24_user_rate_T;
enum
{
   GHDI_USER_RATE_115200,
   GHDI_USER_RATE_57600,
   GHDI_USER_RATE_38400,
   GHDI_USER_RATE_19200,
   GHDI_USER_RATE_9600,
   GHDI_USER_RATE_4800,
   GHDI_USER_RATE_2400,
   GHDI_USER_RATE_1200,
   GHDI_USER_RATE_600,
   GHDI_USER_RATE_300,
   GHDI_USER_RATE_1200_75
};

enum  /* parity */
{
   GHDI_NO_PARITY,
   GHDI_ODD_PARITY,
   GHDI_EVEN_PARITY,
   GHDI_FORCE_0_PARITY,
   GHDI_FORCE_1_PARITY
};

typedef  struct
{
   v24_user_rate_T   user_rate;
   byte              parity;
   byte              number_of_data_bits;
   byte              number_of_stop_bits;
} ghdi_serial_port_config_T;

enum
{
   GHDI_SIO_PORT_0,
   GHDI_SIO_PORT_1
};

typedef enum
{
   GHDI_INTERNAL_RESET_CARD,
   GHDI_ACTIVE_HIGH_RESET_CARD,
   GHDI_ACTIVE_LOW_RESET_CARD
}
ghdi_sim_reset_type_T;


/* <EJECT> */
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

#ifdef FEATURE_VOC_AMR_WB 
#ifdef FEATURE_CS_VOICE_OVER_HSPA
ghdi_status_T ghdi_control_umts_voice
(
  ghdi_umts_voice_control_T,
  umts_codecs_list_T,
  umts_wcdma_cs_voice_bearer_enum_T,
  sys_radio_access_tech_e_type current_rat
);
#else
ghdi_status_T ghdi_control_umts_voice
(
  ghdi_umts_voice_control_T,
  umts_codecs_list_T,
  sys_radio_access_tech_e_type current_rat
);
#endif
#else
#ifdef FEATURE_CS_VOICE_OVER_HSPA
ghdi_status_T ghdi_control_umts_voice
(
  ghdi_umts_voice_control_T,
  umts_wcdma_cs_voice_bearer_enum_T,
  sys_radio_access_tech_e_type current_rat
);
#else
ghdi_status_T ghdi_control_umts_voice
(
  ghdi_umts_voice_control_T,
  sys_radio_access_tech_e_type current_rat
);
#endif
#endif

extern ghdi_status_T ghdi_read_ue_gsm_op_class(ghdi_ue_op_class_T *gsm_op_class);

extern ghdi_status_T ghdi_read_ue_umts_op_class(ghdi_ue_op_class_T *umts_op_class);

extern ghdi_status_T ghdi_read_ue_rat_capability(ghdi_rat_capability_T *ue_rat_capability);

extern ghdi_status_T ghdi_read_ue_msrac_sms_value(ghdi_ue_msrac_sms_value_T *ue_msrac_sms_value);

extern ghdi_status_T ghdi_read_ue_msrac_sm_value(ghdi_ue_msrac_sm_value_T *ue_msrac_sm_value);


#if defined (FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE)
ghdi_status_T ghdi_nvmem_read_per_subs
(
  ghdi_nvmem_parameter_id_T           parameter_id,
  ghdi_nvmem_data_T                   *data_ptr,
  uint16                              context_id
);
ghdi_status_T ghdi_nvmem_write_per_subs
(
  ghdi_nvmem_parameter_id_T           parameter_id,
  ghdi_nvmem_data_T                   *data_ptr,
  uint16                              context_id
);
#endif

#ifdef FEATURE_GSM_MULTIBAND

extern uint32 ghdi_get_current_frame_number(void);

#endif

#ifdef FEATURE_DUAL_SIM
void nas_read_gprs_hw_classmark_per_subs(gprs_hw_classmark_T *classmark,
                                           sys_modem_as_id_e_type as_id);
extern void ghdi_clear_slot_mapping(sys_modem_as_id_e_type as_id);
#else
void nas_read_gprs_hw_classmark(gprs_hw_classmark_T *classmark);
#endif
#endif /* GHDI_EXP_V_H */
