#ifndef TCXOMGR_H
#define TCXOMGR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                           T C X O    M A N A G E R

                           TCXO Manager Header File

GENERAL DESCRIPTION

  This module maintains the Recent Good System values, and VCO-Temperature
  tables, for proper TCXO operation.

EXTERNALIZED FUNCTIONS


REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document
  are confidential and proprietary information of Qualcomm Technologies
  Incorporated and all rights therein are expressly reserved.  By accepting
  this material the recipient agrees that this material and the information
  contained therein are held in confidence and in trust and will not be used,
  copied, reproduced in whole or in part, nor its contents revealed in any
  manner to others without the express written permission of Qualcomm
  Technologies Incorporated.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/tcxomgr.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     --------------------------------------------------------- 
01/30/15   tl      Remove obsolete init functions
12/05/14   tl      Add support for non-deferrable temperature reads for GPS
06/20/14   tl      Restore TCXOMGR_CLIENT_FREQ_EST in client enum
04/17/14   tl      Expose the FT curve rotator and uncertainty in returned vco
11/25/13   mn      Remove reference to mcs_variation.h.
10/17/13   mn      Calculate RGS uncertainty based on difference from
                   FT curve
12/19/12   sk      support for temp read subscription
09/12/13   mn      New Rpush API to support multi-SIM targets.
07/23/13   mn      New API to maintain tech specific RGS.
03/14/13   mn      Remove featurization flag from api header files.
03/04/13   mn      Enabling bring up flag for Triton.
10/29/12   sk      support for rpush request mode api
07/10/12   sg      support for crystal aging feature
04/23/12   sg      Support for factory fine cal removal
02/06/12   sr      RCInit Changes
01/27/12   ns      Support for IFC
01/15/12   sg      support for fast field cal v3.1
08/15/11   sg      Added support for TDSCDMA
06/10/11   sm      Change the rotator type from 16bits to 32 bits
03/30/11   ns      Added sentinel metacomment for tcxomgr_client_id_type
                   and tcxomgr_client_state_type
03/11/11   sb      Added new qualifier for extreme temps
02/18/11   sb      Remove the unused RPC metacomments
11/02/10   sb      Remove unused INLINE functions
10/09/09   va      Add API to send XO information to CGPS.
10/15/09   ag      Replaced static inline with INLINE
08/31/09   ag      Removed customer.h
05/01/09   ns      Feature wrap set_polarity changes
04/16/09   ns      Merged tcxo_set_polarity API from //depot
04/14/09   ns      Clean up to support SCMM and 8650 builds only
04/13/09   ag      Merged from //depot
12/10/08   sb      Add a field in tcxomgr_vco_indicator_type that indicates
                   if RGS is less than 30 seconds old.
12/05/08   sst     Update for QSC11x0 to us XO therm
10/20/08   adw     Replaced FEATURE_XO_DIAG w/ FEATURE_TCXOMGR_DIAG.
09/24/08   adw     Modified CLIENT_FREQ_EST featurization to be based on
                   XO_TASK as well as MCS_HAS_NO_GPS.
09/15/08   sst     Featurize CLIENT_FREQ_EST under XO_TASK
08/28/08   adw     Added FEATURE_XO_CTL_1X_ONLY and FEATURE_XO_DIAG to
                   featurize for ULC.
07/11/08   scm     Extern tcxomgr_reset_rgs_and_temp_table().
06/20/08   mca     Merge from //depot
06/04/08   sg      Added TCXOMGR_SOURCE_POWER_OFF source
05/06/08   rkc     Added function tcxomgr_get_time_and_temp()
04/07/08   ljl     Supported multi-carrier AFC log.
03/13/08   nnv     Added TCXOMGR_UBM_L1 enum for tcxomgr_priority_type.
11/14/07   sst     Updated comment to proper reflect PA temp units
10/31/07   va      Export API to retrieve freq err info
10/09/07   sst     Added function tcxomgr_ft_valid()
                   Updated function tcxomgr_ft_curr() outside of featurization
10/02/07   va      Added Freq Estimator to client list
09/25/07   cab     Added tcxomgr_ft_curr to get ft val at current temp
09/12/07   sst     Added UMB L1 to client list
09/06/07   sst     Lint fixes
07/26/07   sst     Moved prototype of tcxomgr_get_temp to tcxomgr_i.h
                   Re-ordered vco_indicator_type to correct uncertainity val
06/18/07   va      Support for XO Compensation unit test
06/12/07   cab     Added back in fac cal changes
06/12/07   cab     Reverted fac cal (temporarily), added rpush_get_flag
06/08/07   sst     Created tcxomgr_temp_set_type
                   Added tcxomgr_get_temp and tcxomgr_ft_eval prototypes
                   Move PPM_*_TO_PPM macros from _cal and _xo_trim
06/06/07   sst     Create new freq err types and update comments
06/05/07   sst     Added indicator enum values for Factory Calibration modes
05/17/07   cab     Added uncertainty for passive clients
05/16/07   cab     Updated rpush_set_flag to allow multiple client request
05/07/07   cab     Added rotator callback, get_current_rot
03/15/07   cab     Added back stub function for 6800 6.x build
03/12/07   cab     Change time value in rpush to signed, include adc.h.
03/08/07   grl     Fixed win32 compilation issue.
03/02/07   nnv     Modified tcxomgr_rpush and rpush_cb function signatures
                   and changed client state OOS to NO_INFO
03/02/07   nnv     Removed unused function declaration
11/03/06   cab     Added rotator pushing functionality
                   Added client TV OUT
10/19/06   cab     Added PPM client, stubbed old APIs for build issues
10/18/06   cab     added tcxomgr_log_freq_track_mode_enum_type and
                   tcxomgr_log_gain_mode_enum_type
10/10/06   cab     Initial version for tcxomgr v3
10/12/06   cab     Added priorities for WCDMA and GSM

============================================================================*/


/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/

#include "rex.h"
#include "qw.h"
#include "timetick.h"

/*============================================================================

                        TCXO Manager client interface

============================================================================*/

/* Maximum number of frequency entries per client in AFC log packet.  */
#define TCXOMGR_MAX_FREQ_ENTRIES 8

#define TCXOMGR_MAX_NUM_CARRIERS 3
  /* The maximum number of carriers */

  
/* therm definitions, therm bit position for therms should be according
   to the sequence the therm enum is defined in 'tcxomgr_adc_client.h' */
#define TCXOMGR_PMIC_THERM_MASK         (uint32) 0x1
#define TCXOMGR_XO_ACCURATE_THERM_MASK  (uint32) 0x2
#define TCXOMGR_XO_COARSE_THERM_MASK    (uint32) 0x4
#define TCXOMGR_PA_THERM_MASK           (uint32) 0x8
#define TCXOMGR_MSM_THERM_MASK          (uint32) 0x10
#define TCXOMGR_NONDEFER_MASK           (uint32) 0x80000000

/* therm masks for GPS use */
#define TCXOMGR_GPS_ON_THERM_MASK       TCXOMGR_PMIC_THERM_MASK | TCXOMGR_XO_ACCURATE_THERM_MASK
#define TCXOMGR_GPS_OFF_THERM_MASK      TCXOMGR_XO_COARSE_THERM_MASK
#define TCXOMGR_GPS_APQ_THERM_MASK	    TCXOMGR_MSM_THERM_MASK


/* Means "tcxomgr_temp_type" temperature is invalid or unavailable. */
#define TCXOMGR_INVALID_TEMP      ((tcxomgr_temp_type)0x80000000)

/*set specific bit(s) of a 32 bit unsigned flag using mask*/
#define  SET_BIT_MASK(flag, mask) (flag |= (mask)) 

/*set specific bit of a 32 bit unsigned flag using bit position*/
#define  SET_BIT_POS(flag, pos) (flag |= ((uint32)(0x1<<pos))) 

/*clear specific bit(s) of a 32 bit unsigned flag using mask*/
#define  CLR_BIT_MASK(flag, mask) ( flag &= (~mask)) 

/*clear specific bit of a 32 bit unsigned flag using bit position*/
#define  CLR_BIT_POS(flag, pos) ( flag &= ( ~((uint32)(0x1<<pos)) )) 

/*check specific bit(s) of a 32 bit unsigned flag using mask*/
#define  CHECK_BIT_MASK(flag, mask) ( (flag) & (mask)) 

/*check specific bit of a 32 bit unsigned flag using bit position*/
#define  CHECK_BIT_POS(flag, pos) ( (flag) & ((uint32)(0x1<<pos))) 

/*---------------------------------------------------------------------------
  Simple scalar types
---------------------------------------------------------------------------*/

/* VCTCXO value type
   Sensitivity: 2^-20 PPM per LSB */
typedef int32                     tcxomgr_vco_type;


/* Rotator value type
   Sensitivity: 2^-10 PPM per LSB */
typedef int32                     tcxomgr_rotator_type;

/* PPM value type
   Sensitivity: 2^-10 PPM per LSB */
typedef int32                     tcxomgr_ppm_type;

/* High Resolution PPM value type
   Sensitivity: 2^-12 PPM per LSB */
typedef int32                     tcxomgr_hr_ppm_type;

/* Frequency value type
   Sensitivity: 1 Hz per LSB */
typedef int32                     tcxomgr_hz_type;

/* Temperature value type
   Sensitivity: 2^-10 deg C per LSB */
typedef int32                     tcxomgr_temp_type;

/* Search space for WWANs
   Sensitivity: PPM  */
typedef uint8                     tcxomgr_uncertainty_type;

/* Search space for GPS
   Sensitivity: PPB  */
typedef uint32                    tcxomgr_ppb_uncertainty_type;


/* Derivative of the FT curve at a given temp
   Sensitivity: HRPPB/C */
typedef uint32                    tcxomgr_slope_uncertainity_type;


typedef struct
{
  tcxomgr_slope_uncertainity_type  wwan_slope_uncertainty; /* WWAN Coef */
  tcxomgr_slope_uncertainity_type  gps_slope_uncertainty;  /* GPS Coef  */

}tcxomgr_ext_slope_uncertainty_type;


typedef struct
{
  tcxomgr_temp_type          xo;                 /* XTAL temp
                                                    (see below for units)    */
  tcxomgr_temp_type          pmic;               /* 2^-10 C, PMIC temp       */
  tcxomgr_temp_type          pa;                 /* 1 C,     PA temp         */
} tcxomgr_temp_set_type;

/* XO temp is in units of 2^-10 degC */
#define TCXOMGR_XO_TEMP_TO_DEGC( temp )  ( (temp) >> 10 )

/*---------------------------------------------------------------------------
  Rpush Request Priority mode enum
---------------------------------------------------------------------------*/

typedef enum
{
  TCXOMGR_RPUSH_MODE_LOW,
  TCXOMGR_RPUSH_MODE_HIGH,
  TCXOMGR_RPUSH_MODE_OFF
}tcxomgr_rpush_req_mode_enum;

/*---------------------------------------------------------------------------
  Client ID's
---------------------------------------------------------------------------*/

/* The number of clients must not exceed the number of bits in
   tcxomgr.rpush_mask (32). If it does, rpush_mask's size will need
   to be increased */
typedef enum
{
  TCXOMGR_CLIENT_NONE,
  TCXOMGR_CLIENT_GPS,
  TCXOMGR_CLIENT_BT,
  TCXOMGR_CLIENT_MEDIA_FLO,
  TCXOMGR_CLIENT_GSM,
  TCXOMGR_CLIENT_WCDMA,
  TCXOMGR_CLIENT_CDMA_1X,
  TCXOMGR_CLIENT_CDMA_HDR,
  TCXOMGR_CLIENT_PPM,
  TCXOMGR_CLIENT_TV_OUT,
  TCXOMGR_CLIENT_WLAN,
  TCXOMGR_CLIENT_UBM_L1,
  TCXOMGR_CLIENT_FREQ_EST,
  TCXOMGR_CLIENT_LTE,
  TCXOMGR_CLIENT_TDSCDMA,
  TCXOMGR_CLIENT_IFC,
  TCXOMGR_NUM_CLIENTS
} tcxomgr_client_id_type;
/*~ SENTINEL tcxomgr_client_id_type.TCXOMGR_NUM_CLIENTS */

/*---------------------------------------------------------------------------
  Priorities
---------------------------------------------------------------------------*/

typedef enum
{
  TCXOMGR_NO_PRIORITY,
  TCXOMGR_DEFAULT_PRIORITY,
  TCXOMGR_UBM_L1,
  TCXOMGR_1X_OL,
  TCXOMGR_HDR_OL,
  TCXOMGR_1X_CL,
  TCXOMGR_HDR_CL,
  TCXOMGR_GSM,
  TCXOMGR_WCDMA,
  TCXOMGR_LTE,
  TCXOMGR_TDSCDMA,
  TCXOMGR_HIGHEST_PRIORITY,
  TCXOMGR_MAX_PRIORITY
} tcxomgr_priority_type;

/*---------------------------------------------------------------------------
  Source Type
---------------------------------------------------------------------------*/

/* List of possible vco sources */
typedef enum
{
  TCXOMGR_SOURCE_NONE,           /* no control of TCXO */
  TCXOMGR_SOURCE_OVERRIDE,       /* control of TCXO using override reg */
  TCXOMGR_SOURCE_1X_DSP,         /* control of TCXO using 1X DSP */
  TCXOMGR_SOURCE_HDR_DSP,        /* control of TCXO using HDR DSP */
  TCXOMGR_SOURCE_UMTS_DSP,       /* control of TCXO using UMTS DSP */
  TCXOMGR_SOURCE_POWER_OFF,      /* turn off TCXO */
  TCXOMGR_MAX_SOURCE
} tcxomgr_source_type;

/*---------------------------------------------------------------------------
  Restriction Type
---------------------------------------------------------------------------*/

typedef enum
{
  TCXOMGR_RESTRICTION_NONE,    /* TCXO not restricted, anything goes */
  TCXOMGR_RESTRICTION_FAST,    /* TCXO restr to fast closed loop ctrl */
  TCXOMGR_RESTRICTION_SLOW,    /* TCXO restr to slow closed loop ctrl */
  TCXOMGR_RESTRICTION_SRL,     /* TCXO restr to SRL closed loop ctrl */
  TCXOMGR_RESTRICTION_FROZEN,  /* TCXO restr to be frozen */
  TCXOMGR_RESTRICTION_XO,      /* TCXO restr irrelevant, XO target (frozen) */
  TCXOMGR_MAX_RESTRICTION
} tcxomgr_restriction_type;


/*---------------------------------------------------------------------------
  State Type (used in rotator updates)
---------------------------------------------------------------------------*/

typedef enum
{
  TCXOMGR_STATE_IDLE,
  TCXOMGR_STATE_TRAFFIC,
  TCXOMGR_STATE_NO_INFO,
  TCXOMGR_NUM_STATES
} tcxomgr_client_state_type;
/*~ SENTINEL tcxomgr_client_state_type.TCXOMGR_NUM_STATES */

/*----------------------------------------------------------------------*
  TCXOmgr frequency tracking modes in AFC log packet
 *----------------------------------------------------------------------*/

typedef enum
{
  TCXOMGR_VCO_ONLY_TRK_MODE,
  TCXOMGR_FIXED_BW_RVAFC_TRK_MODE,
  TCXOMGR_VAR_BW_RVAFC_TRK_MODE,
  TCXOMGR_ROT_ONLY_TRK_MODE
} tcxomgr_log_freq_track_mode_enum_type;

/*----------------------------------------------------------------------*
  TCXOmgr gain modes in AFC log packet
 *----------------------------------------------------------------------*/

typedef enum
{
  TCXOMGR_ACQ_GAIN_MODE,
  TCXOMGR_TRK_GAIN_MODE
} tcxomgr_log_gain_mode_enum_type;

/*----------------------------------------------------------------------*
  TCXOmgr TRK_LO polarity
 *----------------------------------------------------------------------*/

typedef enum
{
  TCXOMGR_TRK_LO_POLARITY_NORMAL,
  TCXOMGR_TRK_LO_POLARITY_INVERTED
} tcxomgr_trk_lo_enum_type;

/*----------------------------------------------------------------------*
  TCXOmgr TRK_LO polarity return type
 *----------------------------------------------------------------------*/

typedef enum
{
  TCXOMGR_TRK_LO_POLARITY_SUCCESS,    /* Polarity & override value updated */
  TCXOMGR_TRK_LO_POLARITY_INVALID,    /* Polarity is invalid  */
  TCXOMGR_TRK_LO_PDM_VALUE_UNCHANGED  /* Polarity updated, value unchanged */
} tcxomgr_trk_lo_return_enum_type;

/*----------------------------------------------------------------------*
  TCXOmgr Access Stratum Identifiers (AS_ID)

  If a client does not have an associated AS_ID, it should use
  TCXOMGR_AS_ID_NONE as AS_ID.
 *----------------------------------------------------------------------*/

typedef enum
{
  TCXOMGR_AS_ID_NONE = -1,
  TCXOMGR_AS_ID_1,
  TCXOMGR_AS_ID_2,
  TCXOMGR_AS_ID_3,
  TCXOMGR_AS_ID_MAX
} tcxomgr_as_id_enum_type;

/*----------------------------------------------------------------------*
  TCXOmgr Client Info used to identify a particular client.
 *----------------------------------------------------------------------*/

typedef struct
{
  /* Client Tech Identifier */
  tcxomgr_client_id_type     id;

  /* Access Stratum Identifier (AS_ID) of the client */
  tcxomgr_as_id_enum_type    as_id;

} tcxomgr_client_info_struct_type;

/*----------------------------------------------------------------------*
  TCXOmgr Structure used by TCXOMGR to report Rpush to clients registering
  for Rpush callback.
 *----------------------------------------------------------------------*/

typedef struct
{
  tcxomgr_client_info_struct_type client_info;
  tcxomgr_vco_type                vco_val;
  tcxomgr_rotator_type            rot_val;
  uint32                          num_samples;
  timetick_type                   begin_acq_time;
  timetick_type                   end_acq_time;
  int32                           temp;
  tcxomgr_client_state_type       state;
} tcxomgr_rpush_cb_data_type;

/*----------------------------------------------------------------------*
  TCXOmgr Callback function type for Rpush reporting.
 *----------------------------------------------------------------------*/

typedef void (*tcxomgr_rpush_cb_func_type)
(
  tcxomgr_rpush_cb_data_type      rpush_data
);

/*----------------------------------------------------------------------*
  TCXOmgr structure used as input to tcxomgr_register_rpush_cb
 *----------------------------------------------------------------------*/

typedef struct
{
  /* Information about the client registering for Rpush */
  tcxomgr_client_info_struct_type client_info;

  /* Function pointer to be used for callback */
  tcxomgr_rpush_cb_func_type      func_ptr;

} tcxomgr_rpush_cb_registration_type;

/*----------------------------------------------------------------------*
  TCXOmgr structure used to denote the Rpush mode information.
 *----------------------------------------------------------------------*/
typedef struct
{
  tcxomgr_rpush_req_mode_enum     mode;

} tcxomgr_rpush_mode_cb_data_type;

/*----------------------------------------------------------------------*
  Callback function type for Rpush Mode reporting.
 *----------------------------------------------------------------------*/

typedef void (*tcxomgr_rpush_mode_cb_func_type)
(
  tcxomgr_rpush_mode_cb_data_type    mode_info
);

/*----------------------------------------------------------------------*
  TCXOmgr structure used as input to tcxomgr_register_rpush_mode_cb
 *----------------------------------------------------------------------*/

typedef struct
{
  /* Information about the client registering for Rpush */
  tcxomgr_client_info_struct_type client_info;

  /* Function pointer to be used for callback */
  tcxomgr_rpush_mode_cb_func_type func_ptr;

} tcxomgr_rpush_mode_cb_registration_type;

/*----------------------------------------------------------------------*
  TCXOmgr structure to be used by clients to report Rpush to TCXOMGR.
 *----------------------------------------------------------------------*/
typedef struct
{
  tcxomgr_client_info_struct_type client_info;
  tcxomgr_rotator_type            rot_val;
  uint32                          num_samples;
  timetick_type                   begin_acq_time;
  timetick_type                   end_acq_time;
  tcxomgr_client_state_type       state;
} tcxomgr_rpush_data_type;

/*---------------------------------------------------------------------------
  Frequency Entries (AFC logging)
---------------------------------------------------------------------------*/

typedef PACKED struct
{
  /* vco accumulator value */
  uint16   vco_acc;

  /* rotator accumulator value */
  uint16   rot_acc;

  /* delta time */
  uint16   delta_time;

  /* signal strength indicator */
  uint16   signal_strength;

} tcxomgr_log_entries_struct_type;

/*---------------------------------------------------------------------------
  TCXO Manager Crystal Types
---------------------------------------------------------------------------*/

typedef enum
{
  XO_CRYSTAL,
  TCXO_CRYSTAL,
  VCTCXO_CRYSTAL,
  NO_CRYSTAL
} tcxomgr_crystal_type;

/*----------------------------------------------------------------------*
  TCXOmgr temp read subscribers enum 
  NOTE: Any addition/change within this enum also affects the temperature
  subscription log packet (0x1755). We should make sure that the log
  packet is in sync with the enum definition.
 *----------------------------------------------------------------------*/
typedef enum 
{
  /* NOTE: For GPS disabled build flavors, TCXOMGR re-uses client GPS to 
     register for temperature reads. That will ensure that its reading 
     the temperature, even if GPS frequency estimation is not registering
     for the same. */
  TCXOMGR_TEMP_READ_CLIENT_GPS,
  TCXOMGR_TEMP_READ_CLIENT_IFC,
  /* chipxn compensation is made last subscriber to make its call back
     function called at last. We make this because the cb function does
     more things than just copying the temp readings and takes time */
  TCXOMGR_TEMP_READ_CLIENT_CHIPXN_COMP,
  TCXOMGR_TEMP_READ_CLIENT_MAX
}tcxomgr_temp_read_subscriber_enum;

/*---------------------------------------------------------------------------
  Header Type (AFC logging)
---------------------------------------------------------------------------*/

typedef PACKED struct
{
  /* Client that is logging the AFC log */
  uint8    logging_client_id;

  /* Client that is controlling the VcTcxo */
  uint8    ctrl_client_id;

  /* Reserved field */
  uint8    reserved1;

  /* system time of first entry */
  qword    timestamp;

  /* frequency tracking mode - uses  tcxomgr_log_freq_track_mode_enum_type */
  uint8    afc_mode;

  /* frequency loop gain - uses tcxomgr_log_gain_mode_enum_type */
  uint8    afc_gain;

  /* SRL indicator */
  uint8    srl_indicator;

  /* Finger cross mode */
  uint8    cross_mode;

  /* rf band class */
  uint8    band;

  /* Reserved field */
  uint8   reserved;

  /* rf cdma channel */
  uint16   chan;

  /* Carrier frequency */
  uint32   carrier_freq;

  /* vco slope */
  uint16   vco_slope;

  /* Thermistor reading degC */
  uint16   therm;

  /* number of AFC entries */
  uint32   num_freq_entry;

} tcxomgr_log_header_struct_type;

/*---------------------------------------------------------------------------
  AFC log structure
---------------------------------------------------------------------------*/

typedef PACKED struct
{
  tcxomgr_log_header_struct_type      afc_header_info;

  tcxomgr_log_entries_struct_type     entries[TCXOMGR_MAX_FREQ_ENTRIES];

} tcxomgr_log_struct_type;

/*---------------------------------------------------------------------------
  Multi-carrier Frequency Entries (AFC logging)
---------------------------------------------------------------------------*/

typedef PACKED struct
{
  /* vco accumulator value */
  uint16   vco_acc;

  /* delta time */
  uint16   delta_time;

  /* signal strength indicator */
  uint16   signal_strength;

  /* rotator accumulator value */
  uint16   rot_acc[TCXOMGR_MAX_NUM_CARRIERS];

} tcxomgr_mc_log_entries_struct_type;

/*---------------------------------------------------------------------------
  Carrier Entries (AFC logging)
---------------------------------------------------------------------------*/

typedef PACKED struct
{
  /* rf band class */
  uint8    band;

  /* Reserved field */
  uint8    reserved1;

  /* rf cdma channel */
  uint16   chan;

  /* Carrier frequency */
  uint32   carrier_freq;

} tcxomgr_mc_log_carrier_struct_type;

typedef PACKED struct
{
  tcxomgr_temp_type          xo;                 /* 2^-10 C, XTAL temp       */
  tcxomgr_temp_type          pmic;               /* 2^-10 C, PMIC temp       */
  tcxomgr_temp_type          pa;                 /* 1 C,     PA temp         */
} tcxomgr_mc_log_temp_set_type;

/*---------------------------------------------------------------------------
  Header Type (AFC logging)
---------------------------------------------------------------------------*/

typedef PACKED struct
{

  /* Client that is logging the AFC log */
  uint8    logging_client_id;

  /* Client that is controlling the VcTcxo */
  uint8    ctrl_client_id;

  /* Carrier mask */
  uint8    carrier_mask;

  /* system time of first entry */
  qword    timestamp;

  /* frequency tracking mode - uses  tcxomgr_log_freq_track_mode_enum_type */
  uint8    afc_mode;

  /* frequency loop gain - uses tcxomgr_log_gain_mode_enum_type */
  uint8    afc_gain;

  /* SRL indicator */
  uint8    srl_indicator;

  /* Finger cross mode */
  uint8    cross_mode;

  /* vco slope */
  uint16   vco_slope;

  /* Thermistor reading degC */
  tcxomgr_mc_log_temp_set_type  therm;

  /* number of AFC entries */
  uint32   num_freq_entry;

} tcxomgr_mc_log_header_struct_type;

/*---------------------------------------------------------------------------
  MC AFC log structure
---------------------------------------------------------------------------*/

typedef PACKED struct
{
  tcxomgr_mc_log_header_struct_type      afc_header_info;

  /* carrier info */
  tcxomgr_mc_log_carrier_struct_type     carrier[TCXOMGR_MAX_NUM_CARRIERS];

  tcxomgr_mc_log_entries_struct_type     entries[TCXOMGR_MAX_FREQ_ENTRIES];

} tcxomgr_mc_log_struct_type;

/*---------------------------------------------------------------------------
  Client struct
---------------------------------------------------------------------------*/

typedef struct
{
  tcxomgr_client_id_type            id;
  tcxomgr_priority_type             priority;
  char                              name[10];
  tcxomgr_source_type               source;
  tcxomgr_restriction_type          restriction;
  boolean                           resp_req;
  tcxomgr_vco_type                (*vco_cb)        (void);
  void                            (*grant_cb)      (tcxomgr_source_type,
                                                    tcxomgr_restriction_type,
                                                    tcxomgr_vco_type);
  void                            (*change_cb)     (tcxomgr_source_type,
                                                    tcxomgr_restriction_type);
  void                            (*release_cb)    (void);
  void                            (*deny_cb)       (void);
  void                            (*notify_cb)     (tcxomgr_restriction_type);
  void                            (*rpush_flag_cb) (boolean);
  void                            (*rpush_cb)      (tcxomgr_client_id_type,
                                                    tcxomgr_vco_type,
                                                    tcxomgr_rotator_type,
                                                    uint32,
                                                    timetick_type,
                                                    timetick_type,
                                                    int32,
                                                    tcxomgr_client_state_type);
  tcxomgr_rotator_type            (*rot_cb)        (void);
} tcxomgr_client_type;

/*---------------------------------------------------------------------------
  Freq/Temp information Accumulator indicator values
---------------------------------------------------------------------------*/
  /* Enum is order from worst to best uncertianty
     NOTE: the *_NOT_CAL, *_FAC1, *_FT#, *_FAILED enum values apply when
           Factory Calibration of the Freq Temp table/curve is performed */

typedef enum
{
  TCXOMGR_FT_QUAL_FAILED,     /* Fac Cal Failed, phone should have been
                                  failed in the Factory
                                  56 PPM mode                                */
  TCXOMGR_FT_QUAL_NOT_CAL,    /* Factory Calibration has not been performed
                                  56 PPM mode                                */
  TCXOMGR_FT_QUAL_FAC1,       /* Intermediate step of Factory Calibration
                                  56 PPM mode                                */
  TCXOMGR_VCO_DEFAULT,        /* Default value was returned
                                  5 PPM mode                                 */
  TCXOMGR_FT_QUAL_DEFAULT = TCXOMGR_VCO_DEFAULT,

  TCXOMGR_FT_QUAL_WIDE_BINS,  /* Value from Calibrated Curve at extreme temp
                                    so wider bins can be used for acq        */

  TCXOMGR_FT_QUAL_COARSE,     /* Coarse Cal has been completed               */

  TCXOMGR_FT_QUAL_IFC,        /* IFC computed C1                             */

  TCXOMGR_FT_QUAL_FINE,       /* Fine Cal has been completed                 */

  TCXOMGR_FT_QUAL_FT1,        /* Value from calibrated curve with default C0
                                  5 PPM mode                                 */
  TCXOMGR_VCO_OLD_RGS,        /* Value from a not-so-recent Good System
                                  3 PPM mode                                 */
  TCXOMGR_FT_QUAL_OLD_RGS = TCXOMGR_VCO_OLD_RGS,

  TCXOMGR_VCO_INTERPOLATED,   /* Value was interpolated from adj value(s)
                                  3 PPM mode                                 */
  TCXOMGR_FT_QUAL_INTERP = TCXOMGR_VCO_INTERPOLATED,

  TCXOMGR_FT_QUAL_FT2,        /* Value from calibrated curve with known C0
                                  3 PPM mode - c0 state                      */
  TCXOMGR_VCO_EXACT,          /* Value for temp was found in freq/temp table
                                  3 PPM mode                                 */
  TCXOMGR_FT_QUAL_EXACT = TCXOMGR_VCO_EXACT,

  TCXOMGR_VCO_RGS,            /* Value was from the Recent Good System
                                  3 PPM mode                                 */
  TCXOMGR_FT_QUAL_RGS = TCXOMGR_VCO_RGS,

  TCXOMGR_VCO_RGS_RECENT,     /*  RGS, but less than 30 secs old             */

  TCXOMGR_VCO_LAST            /* last value in the enum                      */
}
tcxomgr_vco_indicator_type;

/*----------------------------------------------------------------------*
  Tcxomgr RGS fields type
 *----------------------------------------------------------------------*/
typedef enum
{
  TCXOMGR_RGS_FIELD_ID     = 1,
  TCXOMGR_RGS_FIELD_VCO    = 2,
  TCXOMGR_RGS_FIELD_ROT    = 4,
  TCXOMGR_RGS_FIELD_STATE  = 8,
  TCXOMGR_RGS_FIELD_TIME   = 16,
  TCXOMGR_RGS_FIELD_AS_ID  = 32,
}tcxomgr_rgs_fields_populated;

/*---------------------------------------------------------------------------
  Freq/Temp information struct
---------------------------------------------------------------------------*/

typedef struct
{
  /* VCO Type for WWANs */
  tcxomgr_vco_indicator_type      vco_type;

  /* VCO value for WWANs on VCTCXO target */
  tcxomgr_vco_type                vco_value;

  /* Rotator value for WWANs  */
  tcxomgr_rotator_type            rot_value;

  /* Frequency uncertainity based on VCO type and current temp */
  tcxomgr_uncertainty_type        freq_uncertainty;

  /* Frequency uncertainity based on VCO type and current temp */
  tcxomgr_ppb_uncertainty_type    freq_uncertainty_ppb;

  /* VCO type derived for GPS */
  tcxomgr_vco_indicator_type      gps_vco_type;

  /* Rotator value for GPS  */
  tcxomgr_rotator_type            gps_rot_value;

  /* Frequency uncertainity for GPS based on VCO type and current temp */
  tcxomgr_uncertainty_type        gps_freq_uncertainty;

  /* Frequency uncertainity for GPS based on VCO type and current temp */
  tcxomgr_ppb_uncertainty_type    gps_freq_uncertainty_ppb;

  /* Technology that provided the frequency error if the type is set to RGS
     Tech is set to TCXOMGR_CLIENT_NONE if estimates are from FT curve */
  tcxomgr_client_id_type          id;

  /* Access Stratum Identifier (AS_ID). */
  tcxomgr_as_id_enum_type         as_id;

  /* Temperature difference from last RGS. Value will be 0 (zero) is vco_type
     is not TCXOMGR_VCO_RGS or TCXOMGR_VCO_OLD_RGS */
  int32                           temp_diff;

  /* Time difference from last RGS. Value will be 0 (zero) is vco_type
     is not TCXOMGR_VCO_RGS or TCXOMGR_VCO_OLD_RGS */
  uint32                          time_diff;

   /* The rgs_offset field denots the signed uncertainty due to difference between
     the RGS and the FT curve and the temperature change since the RGS was reported.

     Note: This value will be populated only when field calibration was done
     atleast once and when the vco type is TCXOMGR_VCO_RGS. If TCXOMGR does not have
     confidence on the FT curve coefficients or if vco type is not TCXOMGR_VCO_RGS,
     the value of rgs_offset will be set to 0 (Zero).*/
  tcxomgr_rotator_type            rgs_offset;

  /* VCO Type from the FT curve, not taking any RGS into account */
  tcxomgr_vco_indicator_type      ft_vco_type;

  /* Rotator value from the FT curve, not taking any RGS into account */
  tcxomgr_rotator_type            ft_rot_value;

  /* Frequency uncertainity based on VCO type and current temp from the FT
   * curve, not taking any RGS into account */
  tcxomgr_ppb_uncertainty_type    ft_freq_uncertainty_ppb;

} tcxomgr_vco_info_type;


/*---------------------------------------------------------------------------
  Rotator/VCO information struct
---------------------------------------------------------------------------*/
typedef struct
{
  /* Which of the fields are filled by L1s */
  uint32                     valid_fields;

  /* client doing the update */
  tcxomgr_client_id_type     id;

  /* VCTCXO accumulator value (TRK_LO_ADJ PDM value) */
  tcxomgr_vco_type           vco_val;

  /* Rotator accumulator (in 2^-10 ppm units) */
  tcxomgr_rotator_type       rot_val;

  /* is RGS from traffic ? */
  tcxomgr_client_state_type  state;

  /* System time           */
  uint64                     time;

  /* Access Stratum Identifier (AS_ID). */
  tcxomgr_as_id_enum_type    as_id;

} tcxomgr_rgs_info_type;


/*---------------------------------------------------------------------------
  Recent Good System Values
---------------------------------------------------------------------------*/

typedef struct
{
  tcxomgr_client_id_type      system;
  int16                       temp_diff;
  uint32                      age_secs;
  tcxomgr_vco_type            vco;
  tcxomgr_rotator_type        rot;
}
tcxomgr_rgs_vals_struct_type;

/*============================================================================

                            MACROS AND DEFINES

============================================================================*/

/* If the age in the RGS is invalid, set it to this value */
#define TCXOMGR_INVALID_RGS_AGE  (0xFFFFFFFFUL)

/* Full (1)   PPM units to 2^-10 PPM units conversion */
#define PPM_FULL_TO_PPM( x )          ( (x) << 10 )

/* Half (1/2) PPM units to 2^-10 PPM units conversion */
#define PPM_HALF_TO_PPM( x )          ( (x) << 9 )

/* Full (1)   PPM units to 2^-12 High Resolution PPM units conversion */
#define PPM_FULL_TO_HR_PPM( x )       ( (x) << 12 )

/* Half (1/2) PPM units to 2^-12 High Resolution PPM units conversion */
#define PPM_HALF_TO_HR_PPM( x )       ( (x) << 11 )

/* 2^-10 PPM units to and from 2^-12 High Resolution PPM units */
#define PPM_PPM_TO_HR_PPM( x )        ( (x) << 2 )
#define PPM_HR_PPM_TO_PPM( x )        ( (x) >> 2 )
/* function pointer for subscribing to temp reads
   arguments:
    temp structure for temp data 
    temp_channel- enum representing what temp channels were read/valid
         bit 0- pmic
         bit 1- xo accurate
         bit 2- xo coarse
         bit 3- pa therm
         bit 4- msm therm*/

typedef void (*tcxomgr_temp_read_subscribe_cb) 
(
 tcxomgr_temp_set_type  temp,
 uint32                 temp_channel
);


/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/

/*============================================================================

FUNCTION TCXOMGR_TASKINIT

DESCRIPTION
  Task start up function where all the task initialization is done.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_taskinit( void );


/*============================================================================

FUNCTION TCXOMGR_GET_VCO_SLOPE

DESCRIPTION


DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

extern void tcxomgr_get_vco_slope
(
  /* vco slope */
  uint16 *vco_slope,

  /* vco range */
  uint8 *vco_range
);

/*============================================================================

FUNCTION TCXOMGR_GET_STORED_VCO

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_info_type

SIDE EFFECTS
  None

============================================================================*/

extern tcxomgr_vco_info_type tcxomgr_get_stored_vco (void);

/*============================================================================

FUNCTION TCXOMGR_GET_CLIENT_RGS

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_info_type

SIDE EFFECTS
  None

============================================================================*/

extern tcxomgr_vco_info_type tcxomgr_get_client_rgs (tcxomgr_client_info_struct_type client_info);

/*============================================================================

FUNCTION TCXOMGR_GET_SUBSCRIPTION_RGS

DESCRIPTION
  This API can be used to retrieve an AS_ID sepecific RGS from TCXOMGR

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_info_type

SIDE EFFECTS
  None

============================================================================*/

extern tcxomgr_vco_info_type tcxomgr_get_subscription_rgs (tcxomgr_as_id_enum_type as_id);

/*============================================================================

FUNCTION TCXOMGR_RESET_CLIENT_RGS_VALUES

DESCRIPTION
  This API resets the client RGS values from the TCXOMGR data structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_reset_client_rgs_values( tcxomgr_client_info_struct_type client_info );

/*============================================================================

FUNCTION TCXOMGR_GET_STORED_VCO_AGED

DESCRIPTION
  This function returns the VCO/ROT value after taking into account
  the aging of the XO crystal to the Clients.

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_info_type

SIDE EFFECTS
  None

============================================================================*/

extern tcxomgr_vco_info_type tcxomgr_get_stored_vco_aged(void);


/*============================================================================

FUNCTION TCXOMGR_GET_SLOPE_UNCERTAINITY

DESCRIPTION
  This function provides the slope uncertainity based on the FT curve
  and the temparature

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_slope_uncertainity_type

SIDE EFFECTS
  None

============================================================================*/

extern tcxomgr_ext_slope_uncertainty_type tcxomgr_get_slope_uncertainity(void);


/*============================================================================

FUNCTION TCXOMGR_GET_RGS_VALUES

DESCRIPTION
  This function retrieves the Recent Good System (RGS) values.

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_rgs_vals_struct_type *rgs - Return parameter with RGS data.

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_get_rgs_values
(
  tcxomgr_rgs_vals_struct_type *rgs /* Structure to store RGS values in */
);

/*============================================================================

FUNCTION TCXOMGR_GET_FREQ_ERR_TEMP

DESCRIPTION
  This function reads the trk_lo vs. temperature table and returns the trk_lo
  value corresponding to the current temperature of the phone.

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_indicator_type - Describes trk_lo type, DEFAULT, EXACT, etc.
  tcxomgr_vco_type *vco      - Return parameter with actual VCO value.

SIDE EFFECTS
  None

============================================================================*/
extern tcxomgr_vco_indicator_type tcxomgr_get_freq_err_temp
(
  tcxomgr_vco_type *vco /* VCTCXO Accumulator value for the current temp */
);

/*============================================================================

FUNCTION TCXOMGR_GET_DEFAULT_FREQ_ERR

DESCRIPTION
  This function returns the default value of the trk_lo PDM.

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_type - Default trk_lo value.

SIDE EFFECTS
  None

============================================================================*/
extern tcxomgr_vco_type tcxomgr_get_default_freq_err(void);

/*============================================================================

FUNCTION TCXOMGR_GET_UNCERTAINTY

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_indicator_type

SIDE EFFECTS
  None

============================================================================*/
extern tcxomgr_vco_indicator_type tcxomgr_get_uncertainty (void);


/*============================================================================

FUNCTION TCXOMGR_GET_CURRENT_VCO

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_type

SIDE EFFECTS
  None

============================================================================*/

extern tcxomgr_vco_type tcxomgr_get_current_vco (void);

/*============================================================================

FUNCTION TCXOMGR_GET_CURRENT_ROT

DESCRIPTION
  This function gets the ROT value from the active client by calling its
  registered callback function.  If no one is controlling the rgs value is
  returned.

DEPENDENCIES
  None

RETURN VALUE
  ROT value from the active client or the rgs ROT value if no active.

SIDE EFFECTS
  None.

============================================================================*/

extern tcxomgr_rotator_type tcxomgr_get_current_rot (void);


/*============================================================================

FUNCTION TCXOMGR_UPDATE_RGS_VALUES

DESCRIPTION
  This function updates the Recent Good System values

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

extern void tcxomgr_update_rgs_values
(
  tcxomgr_rgs_info_type *rgs
);

/*============================================================================

FUNCTION TCXOMGR_RESET_RGS_AND_TEMP_TABLE

DESCRIPTION
  This function blows away all Recent Good System data and the entire trk_lo
  vs. temperature table.  It should be called very rarely and very carefully.
  If some RAT (GSM, WCDMA, ...) feels TCXO Manager data is bad, corrupt,
  invalid, etc., and should no longer be used, it calls this function to dump
  all the data.

  Effects of this function take place immediately in RAM cached copies.  New
  data doesn't go to NV until power down.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_reset_rgs_and_temp_table
(
  /* Client resetting RGS and temperature table. */
  tcxomgr_client_id_type  client_id
);


/*============================================================================

FUNCTION TCXOMGR_SET_PRIORITY

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

============================================================================*/

extern void tcxomgr_set_priority
(
  tcxomgr_client_id_type    id,
  tcxomgr_priority_type     priority
);


/*============================================================================

FUNCTION TCXOMGR_CHANGE_SOURCE

DESCRIPTION
  For the client specified by id, who must be in control, it indicates a
  desire to change to a new tcxo control source (i.e. switch from DSP
  closed loop control to open loop control via the override register).

DEPENDENCIES
  Caller must have control of the tcxo

RETURN VALUE
  None

SIDE EFFECTS
  Changes the clients[].source data field

============================================================================*/

extern void tcxomgr_change_source
(
  tcxomgr_client_id_type  id,
  tcxomgr_source_type     source
);


/*============================================================================

FUNCTION TCXOMGR_SET_SERVICE_LEVEL

DESCRIPTION
  This function requests a service level for a particular Active Client.
  This will generate a command that is sent to the Active Client with the
  results of the request.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command may be sent to one or more active clients.

============================================================================*/

extern void tcxomgr_set_source
(
  tcxomgr_client_id_type  id,
  tcxomgr_source_type     source
);


/*============================================================================

FUNCTION TCXOMGR_SET_CALLBACKS

DESCRIPTION
  Sets the callback functions for the client indicated by id.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Installs callback functions into the clients[] data array

============================================================================*/

extern void tcxomgr_set_callbacks
(
  tcxomgr_client_id_type  id,
  tcxomgr_vco_type (*vco_cb)    (void),
  void             (*grant_cb)  (tcxomgr_source_type, tcxomgr_restriction_type,
                                 tcxomgr_vco_type),
  void             (*change_cb) (tcxomgr_source_type, tcxomgr_restriction_type),
  void             (*release_cb)(void),
  void             (*deny_cb)   (void),
  void             (*notify_cb) (tcxomgr_restriction_type),
  void             (*rpush_flag_cb) (boolean),
  void             (*rpush_cb)      (tcxomgr_client_id_type,
                                     tcxomgr_vco_type,
                                     tcxomgr_rotator_type,
                                     uint32,
                                     timetick_type,
                                     timetick_type,
                                     int32,
                                     tcxomgr_client_state_type),
  tcxomgr_rotator_type (*rot_cb) (void)
);


/*============================================================================

FUNCTION TCXOMGR_SET_NOTIFY_CALLBACK

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

============================================================================*/

extern void tcxomgr_set_notify_callback
(
  tcxomgr_client_id_type  id,
  void (*notify_cb) (tcxomgr_restriction_type)
);


/*============================================================================

FUNCTION TCXOMGR_RPUSH_SET_FLAG

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

============================================================================*/

extern void tcxomgr_rpush_set_flag
(
  tcxomgr_client_id_type    id,
  boolean                   rpush_flag
);


/*============================================================================

FUNCTION TCXOMGR_RPUSH_GET_FLAG

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

============================================================================*/

extern boolean tcxomgr_rpush_get_flag( void );


/*============================================================================

FUNCTION TCXOMGR_RPUSH_GET_MASK

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

============================================================================*/

extern uint32 tcxomgr_rpush_get_mask( void );


/*============================================================================

FUNCTION TCXOMGR_RPUSH

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

============================================================================*/

extern void tcxomgr_rpush
(
  tcxomgr_client_id_type    id,
  tcxomgr_rotator_type      rot_val,
  uint32                    num_samples,
  timetick_type             begin_acq_time,
  timetick_type             end_acq_time,
  tcxomgr_client_state_type state
);


/*============================================================================

FUNCTION TCXOMGR_ACK

DESCRIPTION
  This function acknowledges a recieved command.
  This may generate a command that is sent to the Active Client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

extern void tcxomgr_ack
(
  tcxomgr_client_id_type    id,
  tcxomgr_source_type       source,
  tcxomgr_restriction_type  restriction
);


/*============================================================================

FUNCTION TCXOMGR_RELEASE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_release
(
  tcxomgr_client_id_type    id,
  tcxomgr_vco_type          curr_vco_val
);


/*============================================================================

FUNCTION TCXOMGR_REPORT

DESCRIPTION
  This function updates an ack to a new restriction level

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_report
(
  tcxomgr_client_id_type    id,
  tcxomgr_source_type       source,
  tcxomgr_restriction_type  restriction
);


/*============================================================================

FUNCTION TCXOMGR_SET_RESTRICTION

DESCRIPTION
  This function requests a restriction level for a particular Passive Client.
  This will generate a command that is sent to the Active Client with the
  results of the request.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

extern void tcxomgr_set_restriction
(
  tcxomgr_client_id_type    id,
  tcxomgr_restriction_type  restriction
);


/*============================================================================

FUNCTION TCXOMGR_GET_RESTRICTION

DESCRIPTION

DEPENDENCIES
  TCXO Manager must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================*/
extern tcxomgr_restriction_type tcxomgr_get_restriction
(
  tcxomgr_client_id_type    id
);


/*============================================================================

FUNCTION TCXOMGR_FLUSH_CLIENT_LOG

DESCRIPTION
  This function sends the AFC log to diag if the log buffer is full or if
  logging client wanted it to be flushed because of AFC state change etc.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/

extern void tcxomgr_flush_client_log
(
  tcxomgr_log_struct_type     *afc_log_ptr
);


/*============================================================================

FUNCTION TCXOMGR_LOG_ENTRY_UPDATED

DESCRIPTION
  This function is used by clients to indicate that they have updated
  one frequency entry to their AFC log buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the AFC buffer is full, the packet is flushed.

============================================================================*/

extern void tcxomgr_log_entry_updated
(
  tcxomgr_log_struct_type     *afc_log_ptr
);


/*============================================================================

FUNCTION TCXOMGR_MC_FLUSH_CLIENT_LOG

DESCRIPTION

  This function sends the mulitcarrier AFC log to diag if the log
  buffer is full or if logging client wanted it to be flushed because
  of AFC state change etc.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/

extern void tcxomgr_mc_flush_client_log
(
  tcxomgr_mc_log_struct_type     *afc_log_ptr
);


/*============================================================================

FUNCTION TCXOMGR_MC_LOG_ENTRY_UPDATED

DESCRIPTION

  This function is used by clients to indicate that they have updated
  one frequency entry to their multicarrier AFC log buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the AFC buffer is full, the packet is flushed.

============================================================================*/

extern void tcxomgr_mc_log_entry_updated
(
  tcxomgr_mc_log_struct_type     *afc_log_ptr
);


/*============================================================================

FUNCTION TCXOMGR_FT_EVAL

DESCRIPTION
  This function calculates the frequency error in PPM expected based on a
  given set of temperatures. This function is not supported outside of the
  FEATURE_XO_FACTORY_CAL define

DEPENDENCIES
  None

RETURN VALUE
  Frequency Error in PPM Units

SIDE EFFECTS
  The fixed point implementation loss is about 2e-4 ppm on the frequency
   estimate returned

============================================================================*/
extern tcxomgr_ppm_type tcxomgr_ft_eval
(
  tcxomgr_temp_set_type temp
);

/*============================================================================

FUNCTION TCXOMGR_FT_EVAL_GPS

DESCRIPTION
  This function calculates the frequency error in PPM expected based on a
  given set of temperatures. This function is not supported outside of the
  FEATURE_XO_FACTORY_CAL define

DEPENDENCIES
  None

RETURN VALUE
  Frequency Error in PPM Units

SIDE EFFECTS
  The fixed point implementation loss is about 2e-4 ppm on the frequency
   estimate returned

============================================================================*/
extern tcxomgr_ppm_type tcxomgr_ft_eval_gps
(
  tcxomgr_temp_set_type temp
);


/*============================================================================

FUNCTION TCXOMGR_FT_CURR

DESCRIPTION
  This function calculates the frequency error in PPM expected at the current
  temperature

DEPENDENCIES
  None

RETURN VALUE
  Frequency Error in PPM Units

SIDE EFFECTS
  The fixed point implementation loss is about 2e-4 ppm on the frequency
   estimate returned

============================================================================*/
extern tcxomgr_ppm_type tcxomgr_ft_curr( void );

/*============================================================================

FUNCTION TCXOMGR_FT_CURR_BLOCKING

DESCRIPTION
  This function calculates the frequency error in PPM expected at the current
  temperature using blocking temp reads for accuracy

DEPENDENCIES
  None

RETURN VALUE
  Frequency Error in PPM Units

SIDE EFFECTS
  The fixed point implementation loss is about 2e-4 ppm on the frequency
   estimate returned

============================================================================*/
extern tcxomgr_ppm_type tcxomgr_ft_curr_blocking( void );

/*============================================================================

FUNCTION TCXOMGR_FT_VALID

DESCRIPTION
  This function returns TRUE if the FT curve has been calibrated and is valid

DEPENDENCIES
  None

RETURN VALUE
  True/False depending on FT cal state

SIDE EFFECTS
  None

============================================================================*/
extern boolean tcxomgr_ft_valid( void );


/*============================================================================

FUNCTION tcxomgr_get_slope_err

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  boolean, TRUE if estimate available, FALSE if not

SIDE EFFECTS
  None

============================================================================*/

boolean tcxomgr_get_slope_err
(
  int32 *slope,
  int32 *outer_accum,
  timetick_type *timestamp
);


/*============================================================================

FUNCTION TCXOMGR_GET_TIME_AND_TEMP

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_get_time_and_temp
(
  uint32 *time,
  tcxomgr_temp_set_type *temp
);


/*============================================================================

FUNCTION TCXOMGR_SET_POLARITY

DESCRIPTION
  This function sets the polarity as requested and also sets the PDM value.
  The function should be called by the technology team (1x/HDR/WCDMA/GSM/....)
  to set the polarity required by it. It also changes the PDM value to reflect
  the new polarity.

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_trk_lo_return_enum_type which specifies if the polarity change was
  successful, could not change the polarity or an invalid polarity was passed.

SIDE EFFECTS
  None.

============================================================================*/

extern tcxomgr_trk_lo_return_enum_type tcxomgr_set_polarity
(
  tcxomgr_trk_lo_enum_type    polarity
);

/*============================================================================

FUNCTION TCXOMGR_SUBSCRIBE_TEMP_READS

DESCRIPTION
  This function is called to subscribe to the temp reads
  argument:
    temp channel-  bit mask representing channels to read
    read_interval- time period between temp reads in ms
    cb_func - call back function to report temp reads to gps
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

extern void tcxomgr_subscribe_temp_read
(
   tcxomgr_temp_read_subscriber_enum  client_id,
   uint32                             temp_channel,
   uint16                             read_interval,
   tcxomgr_temp_read_subscribe_cb     cb_func
);
/*============================================================================

FUNCTION TCXOMGR_GET_CRYSTAL_TYPE

DESCRIPTION
  Returns the crystal type 

DEPENDENCIES
  None

RETURN VALUE
  Crystal Type Enum

SIDE EFFECTS
  None

============================================================================*/
extern tcxomgr_crystal_type tcxomgr_get_crystal_type(void);

/*============================================================================

FUNCTION TCXOMGR_GET_RPUSH_REQ_MODE

DESCRIPTION
 If GPS is active, RPUSH priority is high
 if IFC has registered for RPUSH and none of the tech has calculated c1 for
 atleast 3 times, RPUSH priority is high
 low otherwise

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_rpush_req_mode_enum

SIDE EFFECTS

============================================================================*/

extern tcxomgr_rpush_req_mode_enum tcxomgr_get_rpush_req_mode (void);

/*============================================================================

FUNCTION TCXOMGR_REGISTER_RPUSH_CB

DESCRIPTION
 API to register for Rotator Push callbacks

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_register_rpush_cb
(
  tcxomgr_rpush_cb_registration_type reg_data
);

/*============================================================================

FUNCTION TCXOMGR_REGISTER_RPUSH_MODE_CB

DESCRIPTION
 API to register for Rpush mode change. TCXOMGR will call this callback
 every time there is a change in Rpush mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_register_rpush_mode_cb
(
  tcxomgr_rpush_mode_cb_registration_type reg_data
);

/*============================================================================

FUNCTION TCXOMGR_ROTATOR_PUSH

DESCRIPTION
 API to report Rpush to TCXOMGR. TCXOMGR will report Rpush to all clients that have
 registered for Rpush callbacks using the tcxomgr_register_rpush_cb API.

 RAT Layer1s can use any of the following mechanisms to check is Rpush is needed.
 * Calling tcxomgr_get_rpush_req_mode API
 * Register for Rpush mode change callback using tcxomgr_register_rpush_mode_info_cb()
   API.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_rotator_push
(
  tcxomgr_rpush_data_type rpush_data
);

#endif /* !TCXOMGR_H */
