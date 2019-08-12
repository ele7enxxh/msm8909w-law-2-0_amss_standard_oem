#ifndef ECALL_EFS_H
#define ECALL_EFS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          E C A L L   P S A P   E F S
                            H E A D E R   F I L E

GENERAL DESCRIPTION 
This file contains the internal APIs between the Ecall APP/PSAP and the EFS to read test configuration.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodem/inc/ecall_efs.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $ 
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                         I N C L U D E   F I L E S

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_MMGSDI_SESSION_LIB
#include "mmgsdilib.h" 
#endif /* FEATURE_MMGSDI_SESSION_LIB */

/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/
#ifdef FEATURE_ECALL_IVS
typedef enum
{
  ECALL_DIAL_NORMAL = 0,
  ECALL_DIAL_OVERRIDE
}ecall_dial_num_type;

typedef struct
{
  boolean voc_config;
  ecall_dial_num_type ecall_dial_num;
  boolean canned_msd;
  uint8 digits[18];
  uint32 gnss_update_time_ms;
  uint32 test_only_callback_timer_ms;
  uint32 test_only_call_cleardown_timer_ms;
  uint32 test_only_session_timer_ms;
  mmgsdi_slot_id_enum_type  ecall_usim_slot_id;
}ecall_efs_config_type;

typedef struct
{
  boolean enable_slip;
  int16   rv;
  int16   numSamples;
  int16   frameIndex[3];  
}ecall_ivs_efs_sample_type;

#endif /* FEATURE_ECALL_IVS */


#if (defined(FEATURE_ECALL_APP) || defined(FEATURE_ECALL_PSAP))
#include "ecall_modem.h"
#include "ecall_msd_defines.h"
#include "ecall_control.h"

#define ECALL_GPS_PERIODIC_FIX_TIMEOUT (5*1000)        /* GPS Periodic Fix Timer of 5 sec */
#define ECALL_CALLBACK_TIMEOUT         (12*60*60*1000) /* 12 hours = 43,200,000 msecs */
#define ECALL_SESSION_TIMEOUT          (20*1000)       /* CEN EN 16062 T7: 20 secs */
#define ECALL_CALL_CLEARDOWN_TIMEOUT   (60*60*1000)    /* 1 hour = 3,600,000 msecs */

#define ECALL_MSG_0( lvl, fmt )                                          \
    MSG( MSG_SSID_ECALL, lvl, fmt )

#define ECALL_MSG_1( lvl ,fmt, a )                                       \
    MSG_1( MSG_SSID_ECALL, lvl, fmt, a )

#define ECALL_MSG_2( lvl, fmt, a, b )                                    \
    MSG_2( MSG_SSID_ECALL, lvl, fmt, a, b )

#define ECALL_MSG_3( lvl, fmt, a, b, c )                                 \
    MSG_3( MSG_SSID_ECALL, lvl, fmt, a, b, c )

#define ECALL_MSG_4( lvl, fmt, a, b, c, d )                              \
    MSG_4( MSG_SSID_ECALL, lvl, fmt, a, b, c, d )

#define ECALL_MSG_5( lvl, fmt, a, b, c, d, e )                           \
    MSG_5( MSG_SSID_ECALL, lvl, fmt, a, b, c, d, e )

#define ECALL_MSG_6( lvl, fmt, a, b, c, d, e, f )                           \
    MSG_6( MSG_SSID_ECALL, lvl, fmt, a, b, c, d, e, f )

/* Message levels */
#define ECALL_LOW        MSG_LEGACY_LOW
#define ECALL_MED        MSG_LEGACY_MED
#define ECALL_HIGH       MSG_LEGACY_HIGH
#define ECALL_ERROR      MSG_LEGACY_ERROR
#define ECALL_FATAL      MSG_LEGACY_FATAL

typedef enum 
{
  AUTOMATIC = 0,
  MANUAL
}call_type;

typedef struct
{
  boolean canned_msd;          /* TRUE/FALSE  (To choose between Canned MSD or real MSD) */
  boolean test_call;           /* TRUE/FALSE  (To choose between Regular eCall or Emergency eCall) */
  call_type type_of_call;      /* AUTO/MANUAL (To indicate whether is is a manual or Auto initiated eCall) */
}ecall_app_efs_struct_type;


#ifdef FEATURE_ECALL_PSAP

/*-------------------------------------------------------------------------
              define a structure to read the PSAP EFS data 
 --------------------------------------------------------------------------*/
#define ECALL_PSAP_CANNED_MSG         10
#define ECALL_PSAP_CANNED_MSG_CNT      2
#define ECALL_PSAP_NUM_SLIP_POSITIONS  3

typedef struct
{
  boolean canned_psap;          /* CANNED_PSAP     TRUE - will use the MSG sequence as specified in EFS */
  boolean enable_slip;          /* ENABLE_SLIP     TRUE - will read the ecall_psap_sample.txt */
  boolean invert_sig;           /* INVERT_SIG      TRUE - will invert the signal for tx */
  boolean force_pull_mode;      /* FORCE_PULL_MODE TRUE - will send out STARTs to PSAP after call connected */
  uint8   hlack_data;           /* HLACK_DATA      Bit 0 - Version; Bit 1 - Positive Ack, 1 - Clear-down */
  uint16  num_times_to_loop;
  uint32  restart_timer;        /* Start the timer after the succesful transmission */
  uint16  canned_msg_count;
  int16   canned_msg_array[ECALL_PSAP_CANNED_MSG][ECALL_PSAP_CANNED_MSG_CNT];
}ecall_psap_efs_struct_type;

typedef struct
{
  int16  sampleMsg;
  int16  numSamples;
  uint16 msgIndex;
  int16  frameIndex[ECALL_PSAP_NUM_SLIP_POSITIONS];  
}ecall_psap_efs_sample_type;


/*===========================================================================

                  F U N C T I O N   D E C L A R A T I O N S

===========================================================================*/

/*==========================================================================

  FUNCTION    ECALL_PSAP_EFS_READ_CANNED_MSG

  DESCRIPTION
    Read from the PSAP configuration from the efs and store it in a structure
    
  DEPENDENCIES
    FILE FORMAT:
       CANNED_PSAP  TRUE/FALSE  (To choose between Canned Messages or real PSAP)
       LOOP   4 2000    (Loops the PSAP behaviour 4 times with 2000 ms delay )  
       START  3         (Sends 3 START messages if CANNED_PSAP is TRUE)
       NACK   2  
       ......

  RETURN VALUE
    None
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_efs_read_canned_msg
(
  ecall_psap_efs_struct_type *ecall_psap_efs_config
);


/*==========================================================================

  FUNCTION    ECALL_PSAP_EFS_READ_SAMPLES

  DESCRIPTION
    Should have the ENABLE_SLIP TRUE in ecall_psap_efs.txt config file.
    If the ecall_psap_efs.txt doesn't exist slipping from PSAP TX is FALSE.
    If we have the flag set to ENABLE_SLIP TRUE and ecall_psap_sample.txt
    doesn't exist uses random numbers to generate slips
    
  DEPENDENCIES
    FILE FORMAT:
          MSG          ACK
          num_samples  1
          msgIndex     5
          frameIndex   6 8
         
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
===========================================================================*/
boolean ecall_psap_efs_read_samples
(
  ecall_psap_efs_sample_type *ecall_psap_efs_sample
);

#endif /* FEATURE_ECALL_PSAP */

/*===========================================================================
FUNCTION      ECALL_SAVE_MSD

DESCRIPTION   This function will read from the efs file all the necessary 
              data that will populate the EFS structure.

PARAMETERS      

DEPENDENCIES  None

RETURN VALUE  Returns True is if succesfully wrote to the efs

SIDE EFFECTS  None
===========================================================================*/
boolean ecall_save_msd
(
  const ecall_msd_structure_s_type *ecall_msd
);

/*===========================================================================

  FUNCTION    ecall_app_efs_read_canned_msd

  DESCRIPTION
        
  DEPENDENCIES
   

  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
===========================================================================*/
boolean ecall_app_efs_read_canned_msd
(
  ecall_msd_structure_s_type *ecall_msd
);

/*==========================================================================
  FUNCTION ECALL_MEMSCPY

  DESCRIPTION
    This function will use memscpy/memcpy accordingly by eCall feature flag

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_memscpy
(
  void *dst,
  size_t dst_size,
  const void *src,
  size_t src_size
);

/*==========================================================================
  FUNCTION ECALL_MEMSMOVE

  DESCRIPTION
    This function will use memsmove/memmove accordingly by eCall feature flag

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_memsmove
(
  void *dst,
  size_t dst_size,
  const void *src,
  size_t src_size
);

#endif /* defined(FEATURE_ECALL_APP) || defined(FEATURE_ECALL_PSAP) */

#ifdef FEATURE_ECALL_IVS
/*===========================================================================

  FUNCTION    ECALL_EFS_READ_ECALL_CONFIG

  DESCRIPTION
     Reads the vocoder configuration needs to be disabled  as 
     specified in the efs file
        
  DEPENDENCIES
   
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
===========================================================================*/
void ecall_efs_read_ecall_config
(
  ecall_efs_config_type *ecall_enable_config
);

boolean ecall_ivs_efs_read_samples
(
  ecall_ivs_efs_sample_type *ecall_ivs_efs_sample
);

boolean ecall_efs_get_config
(
  void
);

/*===========================================================================

  FUNCTION    ECALL_EFS_READ_ERA_GLONASS_CONFIG

  DESCRIPTION
     Reads the ERA-GLONASS configuration.
        
  DEPENDENCIES
   
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
===========================================================================*/
void ecall_efs_read_era_glonass_config
(
  ecall_efs_config_type *ecall_era_glonass_config
);

#endif /* FEATURE_ECALL_IVS */

#endif /* ECALL_EFS_H */
