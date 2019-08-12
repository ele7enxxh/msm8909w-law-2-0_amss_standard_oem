#ifndef TCXOMGR_I_H
#define TCXOMGR_I_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                           T C X O    M A N A G E R

                           TCXO Manager Header File

GENERAL DESCRIPTION

  This module maintains the internal definitions for the txcomgr subsystem

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/tcxomgr/inc/tcxomgr_i.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
--------     ---     -----------------------------------------------------------
08/04/15     tl      Adjust XO trim in field if not factory calibrated
06/04/15     tl      Update log packet structs to log RGS time in milliseconds
04/29/15     tl      Clean up Tcxomgr task signals
03/18/15     tl      Add limits on RGS in and out
03/05/15     tl      Add calibration notification function
01/30/15     tl      Remove obsolete featurization
01/23/15     tl      Add argument to write field cal params EFS asynchronously
01/07/15     tl      Remove obsolete function prototype
12/05/14     tl      Add support for non-deferrable temperature reads for GPS
09/25/14     sg      Remove call to CGPS API to get time
08/07/14     tl      Add argument to tcxomgr_efs_vco_write() to specify table
05/01/14     tl      Change FT bins to 0.5 C across the entire temperature range
02/21/14     tl      Store calibration data as saved in flash
02/13/14     tl      Add function to calculate FT uncertainty
12/19/13     mn      TCXOMGR should update field cal coefficients to GPS when
                     IFC is running.
01/06/14     tl      Support chipxn compensation using VSTMR
11/18/13     mn      Support for client and subscription RGS retrieval log packet.
10/29/13     sm      remove bring up flag for bolt
10/23/13     mn      Calculate RGS uncertainty based on difference from FT curve.
10/22/13     sm      enable bring up flag for bolt
10/04/13     mn      Support to save and retrieve subscription and system
                     RGS to/from EFS.
01/29/13     sk      Support for frequency estimate separation
09/12/13     mn      New Rpush API to support multi-SIM targets.
07/23/13     mn      New API to maintain tech specific RGS.
07/16/13     mn      Sync EFS data after the first C1 cal.
04/18/13     mn      Change lower temp allowed to -45C
03/19/13     mn      C3 calibration failure recovery when GPS is not in session.
03/14/13     mn      Remove featurization flag from api header files.
02/04/13     mn      New log packet for Retrieve RGS.
12/10/12     mn      Logging optimization on tcxomgr messages.
12/10/12     mn      TCXOMGR should flush XO NV contents during powerdown.
11/29/12     sk      nv item in tcxomgr structure changed to pointer
11/19/12     sk      FT1 broken down int c0 & c1 in tcxomgr_int_xo_vco_type enum
10/29/12     sk      Support for exposing the freq error estimation api for FTM tests
10/29/12     sk      support for getting rpush request priority mode
10/18/12     sk      Changed epoc time to Jan 6, 2012
10/18/12     mn      Added t0 to the field cal log packet.
09/14/12     sm      Added tcxomgr_get_time to ensure consistency
09/13/12     sk      support for dividing/adding update rgs function to task queue
08/13/12     sk      check for factory calibration version added
07/13/12     gs      Support for RGS log packet
07/12/12     sg      Support for crystal aging feature
06/15/12     gs      Added support for new offline protocol
05/22/12     mn      Support to reset RGS through the SET_UNCERTAINTY diag command.
04/23/12     sg      Support for factory fine cal removal
03/24/12     sm      Enable FEATURE_TCXOMGR_BRINGUP for Dime bringup
02/17/11     mn      Extrapolate RGS XO temp based on old and new cached temp values.
02/10/12     sr      TCXOMGR Feature Cleanup
02/08/12     sr      Code refractoring for rcinit changes.
01/31/12     ns      Move IFC logging structures to IFC internal header files
01/27/12     ns      Support for IFC
01/15/12     sg      support for fast field cal v3.1
11/28/11     mn      Moved macros temp range check macros from tcxomgr_data.c file.
11/17/11     mn      Got rid of the tcb_ptr argument to tcxomgr_nv_init
11/01/11     ns      Add support for Tcxo mode
10/20/11     mn      Added structure for storing tcxomgr factory cal data in efs file.
09/26/11     ag      Make rot 16 bit in freqest log packet
09/14/11     ag      Changes to enable chipxn compensation.
07/08/11     ns      Move fast field cal logging function declaration under
                     XO TASK feature flag
05/25/11     ns      Adding RGS updates as log packet and also enchanced
                     Field cal log packets
07/07/11     ag      Exposed cmd q free api.
06/27/11     ag      Exposed the temp cache apis so it can be called in task.
04/19/11     sg      Changed to CX8 compensation using Polling
04/06/11     sm      Use latest temperature for freq estimation
04/06/11     sg      Added support for CX8 compensation using SWC
03/30/10     ns      Changes for Rpush RPC
03/28/11     ag      Added apis for version number efs file handling.
10/02/10     ns      Validate tcxomgr structure
02/03/10     ns      Fast field cal support for 2.5 degree c1 calibration
12/01/10     ag      Added mutex to protect NV read/writes.
11/29/10     hh      CMI phase-4 changes.
11/20/10     ag      Field for storing time when cached pmic temp is updated.
11/04/10     sm      Added Support For Fast Field Cal
11/02/10     ns      Increase temp table range
11/02/10     ns      Removed coarse cache variable.
10/14/10     hh      Included Coarse and fine cache temp variables.
08/09/10     ns      Added chipx8 xo compensation timer
07/14/10     sg      Added a variable to tcxo mgr struct to hold last known temp
03/02/10     ns      IQ logging support
11/25/09     ag      Wrapping ADC DAL with FEATURE_TCXOMGR_ADCDAL
11/13/09     ag      Added modem_mcs_defs.h for GENESIS modem.
10/21/09     ag      Corrected the TCXMOGR_LEAVE_ISR_MSM_LOCK_SECT() for Q6.
09/10/09     ag      Merged private code from tcxomgr_task.h.
08/31/09     ag      Replaced ts.h with time_svc.h
06/19/09     ag      Macros used for critical section which defines to rex
                     critical section on Q6 targets and INTLOCK on non Q6.
05/01/09     ns      Feature wrap set_polarity changes
04/13/09     ag      Merged from //depot
07/11/08     scm     Define DBG_STR4() macro.
06/20/08     mca     Merge from //depot
06/04/08     sg      Removed Sleep state, added vco_source to tcxomgr struct
05/06/08     rkc     Removed function tcxomgr_get_time_and_temp()
04/15/08     sg      Added cached working_restr member
03/03/08     va      Use temperature set for RGS
11/16/07     va      XO Freq Est logging Support
10/16/07     sst     Move XO Trim NV functions to tcxomgr_data.c
10/02/07     va      Move slope estimation suff to tcxomgr_frq_est.c
                     Added prototypes of new functions
09/27/07     cab     Include msg.h for MSG macros
09/25/07     cab     Moved tcxomgr global object reference to header file
                     Moved field cal to tcxomgr task
09/19/07     va      Slope estimation needs rot client state
09/06/07     sst     Lint fixes
09/05/07     cab     Field cal logging support
                     Cache therm read due to timeline impacts
07/26/07     sst     Change get_time_and_temp to use temp_set_type
                     Moved tcxomgr_get_temp here from tcxomgr.h
07/13/07     cab     Added DBG_4 macro
06/26/07     sst     Correct prototype
06/18/07     aps     Added factory cal NV read/write functions
06/18/07     va      Support for XO Compensation
06/13/07     sst     Combine all cal members into one structure
06/08/07     sst     Added cal_coefs member to tcxomgr_struct_type
06/06/07     sst     Added Saturated and NULL ptr check macros
05/17/07     cab     Added uncertainty for passive clients
05/16/07     cab     Updated rpush_set_flag to allow multiple client request
01/19/07     cab     Added xo_trim vars to tcxomgr_struct_type
11/03/06     cab     Added rpush flag to monitor rotator pushing
10/10/06     cab     Initial version for tcxomgr v3

============================================================================*/



/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/

#include "mcs_variation.h"
#include "customer.h"

#include "fs_sys_types.h"
#include "fs_public.h"
#include "tcxomgr.h"
#include "tcxomgr_adc_client.h"
#include "nv.h"
#include "msg.h"
#include "modem_mcs_defs.h"
#include "time_svc.h"
#include "tcxomgr_ifc_interface.h"

#include "tcxomgr_task.h"
#include "rex.h"
#include "timer.h"
#include "queue.h"
#include "tcxomgr_cal.h"
#include "tcxomgr_cal_i.h"
#include "tcxomgr_xo_trim_i.h"

/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

============================================================================*/

/* If RGS < 30 secs old, return RGS_RECENT */
#define TCXOMGR_RGS_RECENT_THRESHOLD    30      /* Threshold in seconds   */
#define TCXOMGR_TEMP_XO_MIN             -46080  /* -45  degree PPM        */
#define TCXOMGR_TEMP_XO_MAX             128000  /* +125 degree PPM        */
#define TCXOMGR_TEMP_PMIC_MIN           -46080  /* -45  degree .001 units */
#define TCXOMGR_TEMP_PMIC_MAX           150000  /* +150 degree .001 units */

/* Extreme temperatures at which L1s have to use
   wider bins instead of regular bins..
  Below -10C or above 70C- wide bins have to
  be used */
#define TCXOMGR_EXTREME_TEMP_LOW        -10240
#define TCXOMGR_EXTREME_TEMP_HIGH       71680

/* The absolute limits placed on RGS values received from clients. RGS values
 * outside these limits will be rejected outright. A properly-calibrated XO
 * with nominal coefficients should vary by no more than +/- 24 ppm at extreme
 * temperatures. Allow some margin up to +/- 30 ppm. */
#define MAX_ROT_IN_VALUE               30720 
#define MIN_ROT_IN_VALUE              -30720

/* The absolute limits placed on RGS values returned to clients. RGS values
 * outside these limits will cause Tcxomgr to assert. These limits are based
 * on the input RGS limits, plus 6 ppm to allow for variations in temperature.
 * Returned RGS should always be within VSTMR's limits.
 */
#define MAX_ROT_OUT_VALUE               (MAX_ROT_IN_VALUE + 6114)
#define MIN_ROT_OUT_VALUE               (MIN_ROT_IN_VALUE - 6114)

/* The minimum change in the calibrated FT curve before the new coefficients
 * will be written to flash */
#define TCXOMGR_MIN_FT_CALIBRATION_CHANGE 250

/* The minimum number of new FT points that must be populated to force writing
 * the current calibration to flash, regardless of the change in the FT curve
 */
#define TCXOMGR_MIN_FT_POINT_CHANGE       4

/*define the average value*/
#define  AVG(val1, val2)  ( ( (val1) + (val2) )>>1 )

/* Define Absolute value */
#define  ABS( x ) ( ((x) < 0) ? -(x) : (x) )

/* Define Sign */
#define  SIGN( x ) ( ((x) < 0) ? (-1) : (1) )

/*check bit of a 32 bit unsigned flag*/
#define  CHECK_FLAG_BIT(flag, bit) ( (flag) & ((uint32)(0x1<<bit)) )

/* macro to prevent compiler warnings when the payload parameter is not used
   by state entry, exit, and transition functions, or when a variable is only
   used in MSG_XXXX calls that may be compiled out */
#define TCXOMGR_NOTUSED(i) if(i){}

/* This Macro should be used to null check interfaces.  If a Void return
   value is desired, then pass in the RETURN_VOID macro */
#define TCXOMGR_NULL_PTR_CHK(ptr, retval)                                    \
  if (ptr == NULL)                                                           \
  {                                                                          \
    MSG(MSG_SSID_TCXOMGR, MSG_LEGACY_ERROR, "Unexpected Null Ptr");          \
    { return retval; }                                                       \
  }

#define RETURN_VOID /* null macro */

/*---------------------------------------------------------------------------
  Temperature
---------------------------------------------------------------------------*/

/* If we decide that some averaging is required when reading the thermistor
   then increase this value, otherwise leave at 1.  Keep in mind that each
   read is costly in terms of time. */
#define NUM_TEMP_SAMPLES  1


/*---------------------------------------------------------------------------
  Critical & Atomic Section Macros
---------------------------------------------------------------------------*/

/* Macros for mutual exclusion between two normal tasks */

#define TCXOMGR_INIT_CRIT_SECT(lock)  rex_init_crit_sect(&(lock))
#define TCXOMGR_ENTER_CRIT_SECT(lock) rex_enter_crit_sect(&(lock))
#define TCXOMGR_LEAVE_CRIT_SECT(lock) rex_leave_crit_sect(&(lock))

#define TCXOMGR_ENTER_ATOMIC_SECT()   REX_DISABLE_PREMPTION()
#define TCXOMGR_LEAVE_ATOMIC_SECT()   REX_ENABLE_PREMPTION()

#define TCXOMGR_ENTER_ISR_MSM_LOCK_SECT(lock) \
  REX_ISR_LOCK(&(lock)); \
  REX_DISABLE_PREMPTION()
#define TCXOMGR_LEAVE_ISR_MSM_LOCK_SECT(lock) \
  REX_ENABLE_PREMPTION(); \
  REX_ISR_UNLOCK(&(lock))

#define TCXOMGR_TEMP_VALID(temp) ((temp).xo != TCXOMGR_INVALID_TEMP)
/* Convert to centigrade only if the temperature is valid */
#define TCXOMGR_TEMP_TO_RGS_TEMP(temp)                                        \
        ( TCXOMGR_TEMP_VALID(temp) ?                                          \
        TCXOMGR_XO_TEMP_TO_DEGC((temp).xo):TCXOMGR_INVALID_TEMP )
#define TCXOMGR_UINT8_TO_TEMP(temp, temp_byte)                                \
        ((temp).pmic = (temp).xo = ((((tcxomgr_temp_type)(temp_byte))) << 10))
#define TCXOMGR_RGS_NOT_OLD(time, rgs_time )                                  \
        ( ((time)-(rgs_time)) <= TCXOMGR_RGS_DELTA_TIME_THRESH_FOR_XO )
#define TCXOMGR_RGS_TEMP_DIFF_SMALL( temp, rgs_temp )                         \
        ( ABS( (temp).xo - (rgs_temp).xo ) <=                                 \
        (TCXOMGR_RGS_DELTA_TEMP_THRESH_FOR_XO<<10) )
#define TCXOMGR_TEMP_RANGE_XO_INVALID(temp)                                   \
        ( ( (temp).xo >= TCXOMGR_TEMP_XO_MAX ||                               \
        (temp).xo <= TCXOMGR_TEMP_XO_MIN )? TRUE : FALSE )
#define TCXOMGR_TEMP_RANGE_PMIC_INVALID(temp)                                 \
        ( ( (temp).pmic >= TCXOMGR_TEMP_PMIC_MAX ||                           \
        (temp).pmic <= TCXOMGR_TEMP_PMIC_MIN )? TRUE : FALSE )
/* The temparature threshold for recalibration */
#define TCXO_MAX_RECAL_XO_TEMP_CHANGE (10<<10)

#define TCXOMGR_ROUNDED_PPB_TO_PPM(x)   ((((x) - (((x)>>10)<<10)) >= 512) ? \
                                          (((x)>>10)+1) : ((x>>10)))

/************** EFS defines  *****************/
#define  TCXOMGR_FAC_CAL_VER_PATH	     "/nv/item_files/mcs/tcxomgr/factory_cal_version"
#define  FIELD_CAL_EFS_PATH            "/nv/item_files/mcs/tcxomgr/field_cal_version"
#define  TCXOMGR_OPERATION_MODE        "/nv/item_files/mcs/tcxomgr/tcxo_mode"
#define  FAC_CAL_DEBUG_EFS_PATH        "/nv/item_files/mcs/tcxomgr/xo_factory_cal_debug"
#define  TCXOMGR_EFS_FIELD_CAL_PARAMS  "/nv/item_files/mcs/tcxomgr/field_cal_params"
#define  TCXOMGR_EFS_FIELD_AGING_DATA  "/nv/item_files/mcs/tcxomgr/field_aging_data"
#define  TCXOMGR_EFS_FT_TABLE_WWAN     "/nv/item_files/mcs/tcxomgr/ft_table_wwan"
#define  TCXOMGR_EFS_FT_TABLE_GPS      "/nv/item_files/mcs/tcxomgr/ft_table_gps"
#define  TCXOMGR_IFC_HIST_FILE         "/nv/item_files/mcs/tcxomgr/ifc_c1_history"
#define  TCXOMGR_IFC_VERSION_FILE      "/nv/item_files/mcs/tcxomgr/ifc_version"
#define  TCXOMGR_IFC_C1_FILTER_FILE    "/nv/item_files/mcs/tcxomgr/ifc_c1_filter"
#define  TCXOMGR_XO_CRYSTAL_TYPE_FILE  "/nv/item_files/mcs/tcxomgr/xo_crystal_type"
#define  TCXOMGR_RGS_DATA_FILE         "/nv/item_files/mcs/tcxomgr/rgs_data"
#define  TCXOMGR_EFS_CONF_FILE         "/nv/item_files/conf/mcs_tcxomgr.conf"

/*---------------------------------------------------------------------------
  TCXO Manager's NV access data
---------------------------------------------------------------------------*/

typedef struct
{
  /* Buffer to send command to NV */
  nv_cmd_type        cmd;

  /* Buffer to place NV items in */
  nv_item_type       item;

  /* Function to call for when waiting for NV read/write to complete */
  void               (*wait)(rex_sigs_type);

  /* Mutex to ensure mutual exclusion during NV read/writes */
  rex_crit_sect_type  crit_sect;
}
tcxomgr_nv_struct_type;

/*---------------------------------------------------------------------------
  TCXO Manager's RGS logging type
---------------------------------------------------------------------------*/
typedef enum
{
  TCXOMGR_RGS_UPDATE_LOG_CMD,
  TCXOMGR_RGS_RETRIEVAL_LOG_CMD,
  TCXOMGR_AGED_RGS_RETRIEVAL_LOG_CMD,
} tcxomgr_rgs_logging_enum_type;
/*---------------------------------------------------------------------------
  TCXO Manager's RGS Clients
---------------------------------------------------------------------------*/
typedef enum
{
  TCXOMGR_RGS_CLIENT_GPS,
  TCXOMGR_RGS_CLIENT_GSM,
  TCXOMGR_RGS_CLIENT_WCDMA,
  TCXOMGR_RGS_CLIENT_CDMA_1X,
  TCXOMGR_RGS_CLIENT_CDMA_HDR,
  TCXOMGR_RGS_CLIENT_LTE,
  TCXOMGR_RGS_CLIENT_TDSCDMA,
  TCXOMGR_RGS_CLIENT_MAX
} tcxomgr_rgs_client_enum_type;


/*============================================================================

                             DEBUG MESSAGE MACROS

============================================================================*/

#define DBG_0( LVL, FMT ) \
        MSG( MSG_SSID_TCXOMGR, MSG_LEGACY_ ## LVL, FMT )

#define DBG_1( LVL, FMT, A ) \
        MSG_1( MSG_SSID_TCXOMGR, MSG_LEGACY_ ## LVL, FMT, A )

#define DBG_2( LVL, FMT, A, B ) \
        MSG_2( MSG_SSID_TCXOMGR, MSG_LEGACY_ ## LVL, FMT, A, B )

#define DBG_3( LVL, FMT, A, B, C ) \
        MSG_3( MSG_SSID_TCXOMGR, MSG_LEGACY_ ## LVL, FMT, A, B, C )

#define DBG_4( LVL, FMT, A, B, C, D ) \
        MSG_4( MSG_SSID_TCXOMGR, MSG_LEGACY_ ## LVL, FMT, A, B, C, D )

#define DBG_5( LVL, FMT, A, B, C, D, E ) \
        MSG_5( MSG_SSID_TCXOMGR, MSG_LEGACY_ ## LVL, FMT, A, B, C, D, E )

#define DBG_6( LVL, FMT, A, B, C, D, E, F ) \
        MSG_6( MSG_SSID_TCXOMGR, MSG_LEGACY_ ## LVL, FMT, A, B, C, D, E, F )


/*---------------------------------------------------------------------------
  TCXO Manager's VCTCXO PDM value -vs- Temperature Table
---------------------------------------------------------------------------*/

/* The size of the vco-vs-temp table */
#define VCO_TEMP_TABLE_SIZE       256
#define VCO_TEMP_TABLE_MAX        (63 << 2)

/* Value to use for undefined table entries */
#define VCO_UNDEFINED             -32768


/*---------------------------------------------------------------------------
  TCXO Manager's RGS and Freq Err defines
---------------------------------------------------------------------------*/

/* The value to use if the NV_VCO_DEFAULT value is not defined.  This 16-bit
** signed value used by TCXO MGR corresponds to a 12-bit unsigned value of
** 2047, which corresponds to the mDSP's default value for the VCTCXO PDM. */
#define VCO_DEFAULT               -1

/* The value to use if the NV_VCO_SLOPE value is not defined */
#define VCO_SLOPE_DEFAULT         346

/* The value to use if the NV_VCO_SLOPE_RANGE value is not defined */
#define VCO_SLOPE_RANGE_DEFAULT   255

/* Rotator must be within this limit for VCO Temperature table update */
#define UPDATE_ROTATOR_PPM_LIMIT  307 /* 0.3ppm */

#define TCXOMGR_RGS_DELTA_TIME_THRESH              86400
#define TCXOMGR_RGS_DELTA_TEMP_THRESH              4
#define TCXOMGR_RGS_DELTA_TIME_THRESH_FOR_XO       10800
#define TCXOMGR_RGS_DELTA_TEMP_THRESH_FOR_XO       3
#define TCXOMGR_CACHED_TEMP_VALUE_TIME_HIGH_THRESH 5000
#define TCXOMGR_CACHED_TEMP_VALUE_TIME_LOW_THRESH  100

/* Threshold for accounting for aging of coeffecients
   the value is currently set to 6 months */
#define TCXOMGR_COEFF_DELTA_TIME_THRESH       15552000

/* If temperature of the phone is unavailable, for whatever reason, we will
** still consider RGS to be OK if it is not older than this in seconds. */
#define TCXOMGR_RGS_DELTA_TIME_NO_TEMP_THRESH  30

/* update_rgs_value() should be called every 30 seconds.
   Updates will be ignored if they occur too often. */
#define TCXOMGR_READ_THERM_SEC          30
#define TCXOMGR_UPDATE_TEMP_TBL_SEC     30

/* Set the update threshold to .5ppm or:
 * 2^20 / NV_VCO_SLOPE / 2 */
#define TCXOMGR_GEN_UPDATE_THRESH( x ) \
    ( (x == 0) ? 0 : ((tcxomgr_vco_type)((0x100000 / ( x )) / 2)))

/* Set the initial threshold to 5ppm or:
 * 5 * 2^20 / NV_VCO_SLOPE */
#define TCXOMGR_GEN_INITIAL_THRESH( x ) \
    ( (x == 0) ? 0 : ((tcxomgr_vco_type)(5 * 0x100000 / ( x ))))

/* VCO_TEMP IIR filter: Gain = 1/4 */
#define VCO_TEMP_FILT(old, x)     \
  ((tcxomgr_vco_type)(((int32)(old) * 3 + (int32)(x)) / 4))

#define FREQ_ERR_IS_SATURATED( acc ) \
        ( ( ABS(acc) > TCXOMGR_FT_MAX_PPM ) ? (TRUE) : (FALSE) )

/* ROT_TEMP filter: Gain = 1/16 */
#define ROT_TEMP_FILT(old, x)     \
  ((tcxomgr_vco_type)( ((x) - (old))/16 ))


/*---------------------------------------------------------------------------
  TCXO Manager's Temperature defines
---------------------------------------------------------------------------*/

/* Means "tcxomgr_temp_type" temperature is invalid or unavailable. */
#define TCXOMGR_INVALID_TEMP      ((tcxomgr_temp_type)0x80000000)
/* To change the temp range from 4-255 to -36 to 215 */
#define TCXOMGR_TEMP_INDEX_OFFSET 40

/* Convert thermistor value to table index  (saturated) */
#define TEMP_TO_INX(temp)                                              \
  ( ( ( (temp + TCXOMGR_TEMP_INDEX_OFFSET ) > 0 ) &&                   \
      ( (temp + TCXOMGR_TEMP_INDEX_OFFSET ) < VCO_TEMP_TABLE_MAX ) ) ? \
    ( (uint8)( (temp + TCXOMGR_TEMP_INDEX_OFFSET) >> 2 ) ) :           \
    ( (uint8)( 0 ) ) )

/* check for a valid temperature range */
#define VALID_TEMP( temp )                              \
  ( ( ( TEMP_TO_INX( temp ) == 0 ) ||                   \
      ( ((tcxomgr_temp_type)(temp)) == TCXOMGR_INVALID_TEMP ) ) ?        \
    FALSE : TRUE )

#define TCXOMGR_TEMP_PMIC_VALID(temp) ((temp).pmic != TCXOMGR_INVALID_TEMP)

/* Structure for storing temp read subscribers info */
typedef struct
{
  uint16		            read_period;    /* time interval between temp reads */
  uint32                            temp_channel;   /* bitmask indicating temp channels to read */
  uint32		            time_left;      /* time left for read period to be over */
  boolean 	                    report_temp;    /* indicate whether to report temp or not */
  tcxomgr_temp_read_subscribe_cb    cb_func;        /* call back function */
}tcxomgr_temp_read_subscriber_struct;

/*---------------------------------------------------------------------------
  TCXO Manager's Time defines
---------------------------------------------------------------------------*/
#define TCXOMGR_INVALID_TIME      ((uint32)0x0)
#define TCXOMGR_EPOC_TIME         ((uint32)1009152000)  /* Jan 6th 2012 */


/*---------------------------------------------------------------------------
  TCXO Manager's XO defines
---------------------------------------------------------------------------*/

/* set the default trim value to be equal to that of the HW default value */
#define XO_TRIM_DEFAULT_VALUE           32

/* Time between field cal updates in seconds */
#define FIELD_CAL_UPDATE_PERIOD         30

/***************** AGING DEFINES ***************/
#define TCXOMGR_AGING_PER_YEAR_THRESHOLD  4096         /* 1PPM/Year */
#define TCXOMGR_AGING_C0_TIME             5184000      /* 2 months  */
#define TCXOMGR_AGING_YEAR_IN_SECONDS     31536000     /* 1 year    */
#define TCXOMGR_AGING_C0_SATURATION_PPM   7            /* 7 PPM     */


/******************************************************************************
                 Signals used by TCXO Manager task
******************************************************************************/




/*  The following signals only have context between the receiving task       *
 *   server and the signaling client. These signaling masks are an API       *
 *   declaration between the technology team and their clients. The mask     *
 *   should be collected with the API that defines the context the signal    *
 *   mask is utilized in. Ex: Make sure if we are using RPC that we dont     *
 *   wait on  0x00800000 in tcxomgr                                          *
 *  #define REX_SLEEP_SIG                              0x80000000            *
 *  #define FS_OP_COMPLETE_SIG                         0x40000000            *
 *  #define VS_OP_COMPLETE_SIG                         0x20000000            *
 *  #define SEC_OP_COMPLETE_SIG                        0x10000000            *
 *  #define GRP_ACK_SIG                                0x08000000            *
 *  #define CAMERA_SIG                                 0x04000000            *
 *  #define MDP_COMPLETE_SIG                           0x02000000            *
 *  #define VIDEO_ENCODER_SIG                          0x01000000            *
 *  #define ONCRPC_REPLY_SIG                           0x00800000            */

/* signal to deregister for remote rpush */
#define TCXOMGR_RPC_RPUSH_DEREGISTER             0x00000001
/* signal to send RPUSH remotely         */
#define TCXOMGR_RPC_RPUSH_TRANSFER               0x00000002
/* field cal timer signal                */
#define TCXOMGR_FIELD_CAL_SIG                    0x00200000
#define TCXOMGR_TEMP_READ_SIG                    0x0100000
/* command signal                        */
#define TCXOMGR_CMD_SIG                          0x00400000

/* signal for indicating a MSGR message */
#define TCXOMGR_MSGR_Q_SIG                       0x00000004

#define TCXOMGR_CAL_SIG_WAIT_SIG                 0x00000010
#define TCXOMGR_CAL_CLK_WAIT_SIG                 0x00000020
#define TCXOMGR_CAL_SIG_WAIT_SIG2                0x00000040

#define TCXOMGR_TASK_RPT_TIMER_SIG    0x1000           /* WDOG timer signal */
#define TCXOMGR_TASK_STOP_SIG         0x4000
#define TCXOMGR_NV_CMD_SIG             0x08000000
#define TCXOMGR_TASK_SIG_MASK         (TCXOMGR_TASK_RPT_TIMER_SIG | \
                                       TCXOMGR_TASK_STOP_SIG)



#define TCXOMGR_ALL_SIGS 0xBFFFFFFF

#define TCXOMGR_SIGS   ( TCXOMGR_MSGR_Q_SIG                    | \
                         TCXOMGR_FIELD_CAL_SIG                 | \
                         TCXOMGR_CMD_SIG                       | \
                         TCXOMGR_TEMP_READ_SIG                 | \
                         (TCXOMGR_TASK_SIG_MASK)               | \
                         TCXOMGR_RPC_RPUSH_TRANSFER            | \
                         TCXOMGR_RPC_RPUSH_DEREGISTER )


extern timer_type  tcxomgr_field_cal_timer;  /* Fast Field Calibration timer */
extern timer_type  tcxomgr_temp_read_timer;

/* Timer group used for temperature reads. The timer in this group will be
 * made deferrable or non-deferrable as requested by GPS. */
extern timer_group_type tcxomgr_timer_group;

typedef PACK (struct)
{
  uint32                         timestamp_ms;
  uint64                         gps_time;
  int32                          temp_xo;
  int16                          rot_val;
  int16                          ft_est;
  int32                          vco_val;
  uint8                          accepted;
  uint8                          tech;
  uint8                          tech_state;
  uint8                          as_id;
}tcxomgr_log_rgs_data_struct_type;

/* Temp Subscription log */
#define TCXOMGR_TEMP_SUBSCRIPTION_LOG_PKT     5

typedef enum
{
  TCXOMGR_TEMP_READ_SUBSCRIBE,          /* temp read subscribing event logging */
  TCXOMGR_TEMP_READ_UNSUBSCRIBE,        /* temp read unsubscribing call logging */
  TCXOMGR_TEMP_READ_MODIFY,             /* modification of channel/period logging */
  TCXOMGR_TEMP_READ_TIMER,              /* timer expiry event logging */
  TCXOMGR_TEMP_READ_NOTIFY,             /* temp read complete, calling notification callbacks */
}tcxomgr_temp_subscription_log_event;

typedef PACK (struct)
{
  uint32                                time_stamp;             /* current time of logging */
  tcxomgr_temp_subscription_log_event   log_event;              /* reason for the logging */
  uint32                                temp_channels_to_read;  /* channels to read */
  uint32                                temp_channels_read;     /* channels that are read */
  union
  {
    /* Fields used for log events subscribe, unsubscribe, modify, and timer */
    struct
    {
      uint32                                timer_value;            /* timer run time */
      tcxomgr_temp_read_subscriber_struct   client[TCXOMGR_TEMP_READ_CLIENT_MAX];    /* current subscribers list attributes */
    } event;
    /* Fields used for log event notify */
    struct
    {
      int32                                 xo_temp;                /* current XO temperature */
      int32                                 pmic_temp;              /* current PMIC temperature */
    } notify;
  };
} tcxomgr_log_temp_subs_data_struct_type;

typedef PACK (struct)
{
  uint32                                  log_packet_timestamp; /* timestamp, in milliseconds, of this log packet */
  uint8                                   num_packets;
  tcxomgr_log_temp_subs_data_struct_type  temp_subscribers[TCXOMGR_TEMP_SUBSCRIPTION_LOG_PKT];
} tcxomgr_log_temp_subscription_struct_type;

/* Enum to illustrate the type of RGS retrieval API used */
typedef enum
{
  TCXOMGR_RGS_RETRIEVAL_SYSTEM_RGS,
  TCXOMGR_RGS_RETRIEVAL_CLIENT_RGS,
  TCXOMGR_RGS_RETRIEVAL_SUBSCRIPTION_RGS
} tcxomgr_rgs_retrieval_api_type;

typedef PACK (struct)
{
  uint32                          timestamp_ms;
  int32                           temp_xo;
  int32                           temp_pmic;
  uint32                          freq_uncertainty_ppb;
  int32                           vco_value;
  int32                           wwan_rot_val_est;
  int32                           gps_rot_val_est;
  int32                           rot_value;
  int32                           correction;
  uint8                           rgs_tech;
  uint8                           vco_type;
  uint8                           freq_uncertainty;
  uint8                           wwan_data_available;
  uint8                           state_wwan;
  uint8                           state_gps;
  uint8                           as_id;
  uint8                           api_type;
}tcxomgr_log_rgs_retrieval_data_struct_type;

typedef PACK (struct)
{
  uint32                          timestamp_ms;
  int32                           temp_xo;
  int32                           temp_pmic;
  uint32                          freq_uncertainty_ppb;
  int32                           vco_value;
  int32                           wwan_rot_val_est;
  int32                           gps_rot_val_est;
  uint32                          uncertainty;
  int32                           rot_value;
  int32                           correction;
  int32                           aging_rot_val;
  uint8                           rgs_tech;
  uint8                           vco_type;
  uint8                           freq_uncertainty;
  uint8                           wwan_data_available;
  uint8                           state_wwan;
  uint8                           state_gps;
}tcxomgr_log_aged_rgs_retrieval_data_struct_type;

/* TCXO Manager command codes */
typedef enum
{
  TCXOMGR_CMD_TEMP,        /* notify receiver of current XO/PMIC temperature */
  TCXOMGR_CMD_TEMP_SUBSCRIBE,
  TCXOMGR_CMD_RGS_UPDATE,
  TCXOMGR_CMD_RGS_LOG,
  TCXOMGR_CMD_CXNCOMP,     /* chipxn compensation */
  TCXOMGR_CMD_IFC,
  TCXOMGR_CMD_COARSE_CAL,
  TCXOMGR_CMD_RPUSH_DATA,
  TCXOMGR_CMD_FIELD_TRIM,
  TCXOMGR_CMD_MAX
}
tcxomgr_cmd_code_type;

/* Structure containing parameters specific to the TCXOMGR_NOTIFY_CMD command.
 */
typedef struct
{
  tcxomgr_restriction_type restriction;
}tcxomgr_notify_cmd_type;

typedef struct
{
  tcxomgr_temp_set_type         temp_set;
  uint32                        read_time;
  tcxomgr_adc_client_therm_type channel;
}tcxomgr_temp_cmd_type;

/* Structure containing parameters specific to the TCXOMGR_RPUSH_CMD command.
 */
typedef struct
{
  tcxomgr_client_id_type     source_id;
  tcxomgr_vco_type           vco_value;
  tcxomgr_rotator_type       average_rotator_value;
  uint32                     number_of_samples;
  tcxomgr_client_state_type  client_state;
  timetick_type              sclk_time_first_sample;
  timetick_type              sclk_time_last_sample;
  int32                      temperature;
}tcxomgr_rpush_cmd_type;


/*structure consisting of arguments for field_update_rgs function*/
typedef struct
{
  tcxomgr_update_rgs_struct_type data_to_update;
}tcxomgr_update_rgs_cmd_type;


/* structure for IFC */
typedef struct
{
  tcxomgr_client_id_type     client_id;
  tcxomgr_ifc_rpush_type     rpush_data;
}tcxomgr_ifc_cmd_type;

/* structure for Rpush Data */
typedef struct
{
  tcxomgr_rpush_data_type    data;
}tcxomgr_rotator_push_cmd_type;

/* structure for Factory Calibration Command */
typedef struct
{
  /* Unique Identifier for the command. The response for this
     command will have the same identifier. */
  uint32 id;

  /* Payload of the cmd */
  tcxomgr_coarse_dc_cal_req_data_type data;
}tcxomgr_coarse_cal_cmd_type;

typedef union
{
  tcxomgr_log_rgs_data_struct_type                rgs_data;
  tcxomgr_log_rgs_retrieval_data_struct_type      rgs_retrieval_data;
  tcxomgr_log_aged_rgs_retrieval_data_struct_type aged_rgs_retrieval_data;
} tcxomgr_rgs_logging_data_type;

typedef struct
{
  tcxomgr_rgs_logging_enum_type      type;
  tcxomgr_rgs_logging_data_type      data;
}tcxomgr_log_rgs_cmd_type;


typedef struct
{
  boolean       enable_comp;
  vstmr_rtc_id  rtc_id;
}tcxomgr_cxn_comp_cmd_type;

typedef struct
{
  tcxomgr_log_temp_subscription_struct_type temp_data;
}tcxomgr_temp_subs_cmd_type;

typedef struct
{
  int32         delta_c0;
} tcxomgr_field_trim_cmd_type;

/* TCXO Manager's generic command struct */
typedef struct
{
  /* Common fields. */
  q_link_type                   link;
  tcxomgr_cmd_code_type         cmd_id;

  /* Union of command structures. */
  union
  {
    tcxomgr_temp_cmd_type       temp_read;
    tcxomgr_temp_subs_cmd_type  temp_subscribe;
    tcxomgr_update_rgs_cmd_type rgs_data;
    tcxomgr_log_rgs_cmd_type    log_rgs_data;
    tcxomgr_cxn_comp_cmd_type   cxn;
    tcxomgr_ifc_cmd_type        ifc_data;
    tcxomgr_coarse_cal_cmd_type fac_cal_data;
    tcxomgr_rotator_push_cmd_type   rpush_data;
    tcxomgr_field_trim_cmd_type field_trim_data;

  } cmd;
}
tcxomgr_cmd_type;

/*---------------------------------------------------------------------------
  TCXO Manager State
---------------------------------------------------------------------------*/

typedef enum
{
  TCXOMGR_UNINITIALIZED,
  TCXOMGR_INITIALIZING,
  TCXOMGR_IDLE,
  TCXOMGR_REPLY_PENDING,
  TCXOMGR_CONTROLLED,
  TCXOMGR_MAX_STATE
} tcxomgr_state_type;

/*---------------------------------------------------------------------------
  TCXO Manager XO VCO Types
---------------------------------------------------------------------------*/

typedef enum
{
  TCXOMGR_INT_FT_QUAL_COARSE,
  TCXOMGR_INT_FT_QUAL_IFC,
  TCXOMGR_INT_FT_QUAL_FINE,
  TCXOMGR_INT_FT_QUAL_C1,
  TCXOMGR_INT_FT_QUAL_C0,
  TCXOMGR_INT_FT_QUAL_FT2_FIELD3,
  TCXOMGR_INT_FT_QUAL_FT2_FIELD4,
  TCXOMGR_INT_FT_QUAL_FT2_FIELD5,
  TCXOMGR_INT_QUAL_RGS,
  TCXOMGR_INT_QUAL_DEFAULT,
  TCXOMGR_INT_QUAL_LAST
} tcxomgr_int_xo_vco_type;

/*---------------------------------------------------------------------------
  TCXO Manager XO Temperature Types
---------------------------------------------------------------------------*/

typedef enum
{
  TCXOMGR_XO_TEMP_RANGE_1,
  TCXOMGR_XO_TEMP_RANGE_2,
  TCXOMGR_XO_TEMP_RANGE_3,
  TCXOMGR_XO_TEMP_RANGE_4,
  TCXOMGR_XO_TEMP_RANGE_5,
  TCXOMGR_XO_TEMP_RANGE_6,
  TCXOMGR_XO_TEMP_RANGE_LAST
} tcxomgr_int_xo_temp_range_type;


/*---------------------------------------------------------------------------
  TCXO Manager XO Coefficients Types
---------------------------------------------------------------------------*/

typedef enum
{
  TCXOMGR_XO_COEF_C0,
  TCXOMGR_XO_COEF_C1,
  TCXOMGR_XO_COEF_C2,
  TCXOMGR_XO_COEF_C3,
  TCXOMGR_XO_COEF_LAST
} tcxomgr_xo_coef_type;

/* Contents needed for fast field calibration */
/* this structure holds rotator information for
   each entry in temp table*/
typedef PACK (struct)
{
    /* Time when the last update was made to the rotator value.*/
    uint32  timeLastUpdated;
    /* The rotator value corresponding to the temperature as
       indicated by the array element. */
    int16   rotValue;
    /* Indicates the number of times the rotValue was reported.
       This serves as a metric to gauge the confidence about the rot val*/
    uint8   rotCount;
    /* If a RGS value reported exceeds the previously updated RGS value
       by a pre-determined threshold then the differece gets stored here */
    int16   offsetValue;
    /* Indicates the number of times the reported RGS exceeded the
       earlier RGS by a pre-determined threshold */
    uint8   offsetCount;

}tcxomgr_field_temptable_elem;


/* Field Calibration Data */
typedef struct
{
    tcxomgr_field_temptable_elem field_temp_table[VCO_TEMP_TABLE_SIZE];
    boolean                      hasDataChanged;
    /* How many elements of field_temp_table[] are saved in flash */
    int32                        ft_count_nv;
    tcxomgr_temp_type            last_temp_read;
    uint32                       last_temp_read_time;
    tcxomgr_cal_field_data_type    cal_data;

    /* The cal data that is currently saved in flash. This can be compared to
     * cal_data to see what, if any, difference there is. */
    tcxomgr_cal_field_data_type    cal_data_nv;

    tcxomgr_cal_aging_data_type    aging_data;
    tcxomgr_ft_table_type          table_id;
    uint8                          last_temp_index;
}tcxomgr_field_cal_info;

typedef struct
{
  uint32                 update_flag[TCXOMGR_TABLE_MAX];
  tcxomgr_field_cal_info fieldcal_data[TCXOMGR_TABLE_MAX];
} tcxomgr_field_cal_update_info_type;

/* Number of IQ samples to log in one iteration */
#define TCXOMGR_LOG_CAL_RAW_IQ_DATA 256

typedef PACK (struct)
{
  uint16          version;
  uint16          fragment_num;
  uint16          fragment_tot;
  uint16          uniq_samp_num;
  uint16          unit_size;
  uint16          tech;
  int32           temp_xo;
  int32           temp_pmic;
  int32           offset;
  uint32          rx_freq_khz;
  uint16          raw[TCXOMGR_LOG_CAL_RAW_IQ_DATA];
} tcxomgr_log_iq_data_struct_type;


#define TCXOMGR_FAST_FIELD_LOG_RGS_PACKETS 10

typedef PACK (struct)
{
  int32                      c3;
  int32                      c2;
  int32                      c1;
  int32                      c0;
  int32                      t0;
} tcxomgr_log_coeff_struct_type;

typedef PACK (struct)
{
  int32                         c1_temp_range;
  int32                         c1_std_dev_all;
  int32                         c1_std_dev_calc;
  int32                         c3_temp_range;
  int32                         c3_std_dev_all;
  int32                         c3_std_dev_calc;
  int16                         field_temp_table[VCO_TEMP_TABLE_SIZE];
}tcxomgr_log_field_cal_data_struct_type;


typedef PACK (struct)
{
  tcxomgr_log_coeff_struct_type  fac_coeff;
  tcxomgr_log_coeff_struct_type               field_coeff;
  uint8                                       state;
  uint8                                       ft_qual_ind;
  uint8                          xo_trim;
  tcxomgr_log_field_cal_data_struct_type      field_cal_data;
  uint8                                       table;
} tcxomgr_log_field_cal_struct_type;

#define TCXOMGR_LOG_RGS_PACKETS 10

typedef PACK (struct)
{
  uint8                                     num_packets;
  uint32                                    log_packet_timestamp_ms;
  tcxomgr_log_rgs_data_struct_type          rgs_data[TCXOMGR_LOG_RGS_PACKETS];
}tcxomgr_log_rgs_struct_type;

typedef PACK (struct)
{
  uint8                                      num_packets;
  uint32                                    log_packet_timestamp_ms;
  tcxomgr_log_rgs_retrieval_data_struct_type rgs_data[TCXOMGR_LOG_RGS_PACKETS];
}tcxomgr_log_rgs_retrieval_struct_type;

typedef PACK (struct)
{
  uint8                                           num_packets;
  uint32                                          log_packet_timestamp_ms;
  tcxomgr_log_aged_rgs_retrieval_data_struct_type rgs_data[TCXOMGR_LOG_RGS_PACKETS];
}tcxomgr_log_aged_rgs_retrieval_struct_type;

typedef PACK (struct)
{
  uint32                         c0;
  uint32                         c1;
  uint32                         c2;
  uint32                         c3;
  uint8                          xo_trim;
  uint32                         is_fac_cal;
} tcxomgr_efs_cal_data_type;

typedef PACK (struct)
{
  tcxomgr_cal_xo_cal_tech      tech;               /* Technology   */
  uint32                       rx_freq_khz;        /* Carrier Frequency */
  tcxomgr_hz_type              offset;             /* Hz, single tone freq
                                                     offset relative to the
                                                     carrier freq             */
  tcxomgr_temp_type            temp_span;          /* 2^-10 C, temp span for
                                                     factory calibration      */
  tcxomgr_cal_record_enum_type update_nv;          /* update NV items flag     */
  uint16                       chain;              /* Chain used for sample capture */
  uint16                       carrier;            /* Carrier used for capture */
  tcxomgr_cal_state_enum_type  state;              /* Factory cal state */
} tcxomgr_cal_fac_debug_param_type;


/* Structure to store temperature information that should help with debugging issues
   related to factory cal. */
typedef PACK (struct)
{
  tcxomgr_hz_type                       freq_est_raw;
                                              /* raw frequency estimate   */

  tcxomgr_hz_type                       freq_est;
                                              /* adj frequency estimate   */

  tcxomgr_hr_ppm_type                   freq_est_pmic_comp;
                                              /* adj frequency estimate   */
} tcxomgr_efs_cal_freq_debug_data_type;

typedef PACK (struct)
{
  tcxomgr_cal_fac_debug_param_type      fac_cal_param;
                                              /* Factory cal parameters */
  int32                                 max_phase_diff;
                                              /* Denotes the maximum phase jump in the
                                                 IQ samples. */
  tcxomgr_efs_cal_freq_debug_data_type  max_phase_freq_data;
                                              /* Captures frequency related debug information
                                                 for the sample where the maximum phase jump
                                                 occured */
  int32                                 min_phase_diff;
                                              /* Denotes the minimum phase jump in the
                                                 IQ samples. */
  tcxomgr_efs_cal_freq_debug_data_type  min_phase_freq_data;
                                              /* Captures frequency related debug information
                                                 for the sample where the minimum phase jump
                                                 occured */
  tcxomgr_temp_set_type                 init_temp;
                                              /* initial temp (XO,PMIC,PA)*/

  tcxomgr_temp_set_type                 final_temp;
                                              /* final temp (XO,PMIC,PA)  */
} tcxomgr_efs_cal_dc_debug_data_type;

typedef struct
{
  /* VCTCXO -vs- Temperature table */
  tcxomgr_vco_type                temp[ VCO_TEMP_TABLE_SIZE ];

  /* VCTCXO -vs- Temperature table */
  tcxomgr_vco_type                offset[ VCO_TEMP_TABLE_SIZE ];

  /* Count of the number of times rotator value is filtered */
  uint8                           rotCount[ VCO_TEMP_TABLE_SIZE ];

  /* Count of the number of times offset value is filtered */
  uint8                           offsetCount[ VCO_TEMP_TABLE_SIZE ];

  /* Default value for when table entry is VCO_UNDEFINED */
  tcxomgr_vco_type                dflt;

  /* Last update time (in seconds since 6 Jan 1980 00:00:00 */
  uint32                          last_update_secs;

  /* Slope: Sensitivity 2^-20 ppm per lsb of a 16bit PDM */
  uint16                          slope;

  /* Slope Range (%)  [255 == not calibrated] */
  uint8                           slope_range;
}
tcxomgr_vco_struct_type;


typedef PACK (struct)
{

  /* VCTCXO -vs- Temperature table */
  int16                temp[ VCO_TEMP_TABLE_SIZE ];

  /* Count of the number of times rotator value is filtered */
  uint8                rotCount[ VCO_TEMP_TABLE_SIZE ];

} tcxomgr_efs_vco_struct_type;


/*---------------------------------------------------------------------------
  TCXO Manager's Recent Good System Values
---------------------------------------------------------------------------*/

typedef struct
{
  /* System that set the most recent good system values */
  tcxomgr_client_id_type          id;

  /* Temperature measurement when VCO & ROT values were captured */
  tcxomgr_temp_set_type           temp;

  /* Time when most recent good system values were captured */
  uint32                          time;

  /* Time when most recent good system values were captured (in msec) */
  uint32                          time_in_msec;

  /* VCTCXO accumulator of the most recent good system */
  tcxomgr_vco_type                vco;

  /* VCTCXO accumulator of the most recent good system */
  tcxomgr_rotator_type            rot;

  /* Access stratum identifier of the client */
  tcxomgr_as_id_enum_type         as_id;
}
tcxomgr_rgs_struct_type;

/*---------------------------------------------------------------------------
  TCXO Manager's XO Trim/Capacitor/Coarse Rotator Values
---------------------------------------------------------------------------*/

typedef struct
{
  /* keep track if table has been updated */
  boolean                         switched;

  /* Current XO_trim value, as programmed/read from the PMIC */
  uint8                           curr_val;

  /* Current XO_trim value that has been applied to the temp/freq table */
  uint8                           table_val;
}
tcxomgr_xo_trim_struct_type;

/*---------------------------------------------------------------------------
  TCXO Manager static data
---------------------------------------------------------------------------*/

typedef struct
{
  /* TCXO Manager State */
  tcxomgr_state_type              state;

  /* Indicates if a service/restr request has arrived while waiting for ack */
  boolean                         pending_request;

  /* Indicates a pre-emption is in process */
  boolean                         restr_changed;

  /* rpush mask - each client has a unique bit, conglomerate rpush vote */
  uint32                          rpush_mask;

  /* Active client for the tcxomgr */
  tcxomgr_client_id_type          client_in_control;

  /* Client last commanded from whom we are expecting an ack */
  tcxomgr_client_id_type          ack_client;

  /* Active client for the tcxomgr */
  tcxomgr_client_id_type          best_client;

  /* Current restriction */
  tcxomgr_restriction_type        curr_restr;

  /* working restriction */
  tcxomgr_restriction_type        working_restr;

  /* VCTCXO -vs- Temperature table */
  tcxomgr_vco_struct_type         vco;

  /* VCTCXO -vs- Temperature table for GPS */
  tcxomgr_vco_struct_type         gps_vco;

  /* VCO Temp Initial threshold */
  tcxomgr_vco_type                vco_temp_initial_thresh;

  /* VCO Temp Update threshold */
  tcxomgr_vco_type                vco_temp_update_thresh;

  /* XO Trim/Capacitor/Coarse Rotator fields */
  tcxomgr_xo_trim_struct_type     xo_trim;

  /* Frequency Temperature Curve fields */
  tcxomgr_cal_fac_type            cal_data;

  /* Storage for reading/writing to NV, converted to pointer variable
     to make tcxomgr structure size independent to the changes in
     tcxomgr_nv_struct_type structure */
  tcxomgr_nv_struct_type          *nv;

  /* is tcxo manager currently in control and driving the 1x
     override register?  */
  boolean                         override;

  /* what value are we driving the override register with? */
  tcxomgr_vco_type                override_val;

  /* test case number, set via diag for unit testing only */
  uint8                           test_case;

  /* Client who is currently being signaled with a callback function */
  tcxomgr_client_id_type          cb_id;

  /* used by passive to determine worst case vco type being used by active */
  tcxomgr_vco_indicator_type      uncertainty;

  /* Revisit - store cached value of xo temp so no therm read in timeline */
  tcxomgr_temp_set_type           cached_xo_temp;

  /* Revisit - store timestamp of when cached xo temp value was read */
  uint32                          cached_xo_temp_time;

  /* store timestamp of when cached pmic temp value was updated */
  uint32                          cached_pmic_temp_time;

  /* Maintain last fine temp read time to decide if coarse temp reads can be
     used to update central cache */
  uint32                          cached_fine_temp_time;

  /* Current VCO Source */
  tcxomgr_source_type             vco_source;


  /* Current trk_lo polarity */
  tcxomgr_trk_lo_enum_type        polarity;


  /* Store the last known temparature value for all the therms.
   Useful to measure the temprature change between two reads */
  tcxomgr_adc_temp_type           tcxo_last_cal_therm_temp[TCXO_MAX_THERM];

  tcxomgr_field_cal_info          fieldcal_data[TCXOMGR_TABLE_MAX];

  /* Global to indicate the therm that is present in the device */
  tcxomgr_adc_client_therm_type    vctcxo_therm;

  /* Variable to indicate if tcxomgr is operating in vctcxo/xo mode */
  tcxomgr_crystal_type             tcxomgr_mode;
}
tcxomgr_struct_type;


/*===========================================================================
               Structure for performing EFS read & write
===========================================================================*/
#define MAX_EFS_BYTES 20
#define EFS_NAME_LEN 256

typedef struct
{
  char fname[EFS_NAME_LEN];       /* to store file name */
  int  fd;                        /* file descriptor to read */
  int return_size;                /* for error checking */
  struct fs_stat fstat;           /* for checking error conditions */
  uint8 buffer[MAX_EFS_BYTES];    /* memory area to read the efs item */
} tcxomgr_efs_type;

extern tcxomgr_struct_type tcxomgr;

/* Critical section lock */
extern rex_crit_sect_type   tcxomgr_crit_sect;

/* tcxomgr clients */
extern tcxomgr_client_type clients[TCXOMGR_NUM_CLIENTS];

/* TCB for tcxomgr task */
extern rex_tcb_type* tcxomgr_tcb_ptr;

/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/

/******************************************************************************
                 TCXO Manager Task prototypes.
******************************************************************************/
/*===========================================================================

FUNCTION TCXOMGR_CLR_SIGS

DESCRIPTION    This function clears the TCXO Manager task signals.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void tcxomgr_clr_sigs ( rex_sigs_type sigs );


/*===========================================================================

FUNCTION TCXOMGR_WAIT

DESCRIPTION
  This function just performs a REX wait as specified by the caller.

RETURN VALUE
  Returns the value returned by rex_wait

DEPENDENCIES

===========================================================================*/
extern rex_sigs_type tcxomgr_wait ( rex_sigs_type  sigs );

/*===========================================================================

FUNCTION      TCXOMGR_GET_CMD_BUF

DESCRIPTION

  Returns a pointer to a newly allocated command buffer.

DEPENDENCIES

  The buffer must eventually be returned to the free queue using
  tcxomgr_free_cmd_buf().

RETURN VALUE

  The newly allocated buffer. Null if there are no free buffers left.

SIDE EFFECTS

===========================================================================*/
tcxomgr_cmd_type* tcxomgr_get_cmd_buf(void);

/*===========================================================================

FUNCTION      TCXOMGR_FREE_CMD_BUF

DESCRIPTION

  This function returns a TCXOMGR command buffer to the free queue once it
  is no longer needed.

DEPENDENCIES

  tcxomgr_q_init() must be called prior to calling this function.

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/

void tcxomgr_free_cmd_buf(tcxomgr_cmd_type* buf);

/*===========================================================================

FUNCTION      TCXOMGR_SEND_CMD

DESCRIPTION

  Writes a command to the TCXOMGR command queue then
  raises the TCXOMGR_CMD_SIG signal.

DEPENDENCIES
  The pointer passed to the function must have been allocated using
  tcxomgr_get_cmd_buf.

RETURN VALUE
  TRUE if the command is successfully created and queued, otherwise FALSE.

SIDE EFFECTS

===========================================================================*/

extern void tcxomgr_send_cmd (tcxomgr_cmd_type *cmd_ptr );

/*============================================================================

FUNCTION TCXOMGR_NV_INIT

DESCRIPTION
  This function initializes the NV member of the TCXO manager

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_nv_init
(
  /* Signal to wait for, for NV reading */
  rex_sigs_type sig,

  /* Function to call to wait for NV reading to complete, eg) rex_wait */
  void (*wait)(rex_sigs_type)
);

/*============================================================================

FUNCTION TCXOMGR_NV_CMD

DESCRIPTION
  This function reads/write an item from/to the NV into tcxomgr.nv.item

DEPENDENCIES
  The caller must have tcxomgr.nv->crit_sect locked

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

extern nv_stat_enum_type tcxomgr_nv_cmd
(
  /* READ, WRITE, PEEK, POKE, etc. */
  nv_func_enum_type cmd,

  /* Item ID to read/write from/to NV */
  nv_items_enum_type id
);

/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_READ

DESCRIPTION
  This function reads the XO Trim/Capacitor/Coarse Rotator values from NV

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

extern void tcxomgr_xo_trim_read( void );

/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_WRITE

DESCRIPTION
  This function writes the XO Trim/Capacitor/Coarse Rotator values to NV

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

extern void tcxomgr_xo_trim_write( void );

/*============================================================================

FUNCTION TCXOMGR_NV_CAL_DATA_READ

DESCRIPTION
  This function reads the calibration data from NV.

  If NV read of factory cal data fails or if ft_qual of factory cal data is
  FAIL, ERR_FATAL the phone. If field cal data read fails, load the factory cal
  data into field cal structure.

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_nv_cal_data_read( void );

/*============================================================================

FUNCTION TCXOMGR_NV_CAL_DATA_WRITE

DESCRIPTION
  This function writes xo calibration data values to NV.

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_nv_cal_data_write( void );

/*============================================================================

FUNCTION TCXOMGR_NV_XO_TRIM_READ

DESCRIPTION
  This function reads the XO Trim/Capacitor/Coarse Rotator values from NV

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_nv_xo_trim_read( void );


/*============================================================================

FUNCTION TCXOMGR_NV_XO_TRIM_WRITE

DESCRIPTION
  This function writes the XO Trim/Capacitor/Coarse Rotator values to NV

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_nv_xo_trim_write( void );



/*============================================================================

FUNCTION TCXOMGR_GET_TEMP

DESCRIPTION
  This function reads the thermistors for the XO, PMIC, and PA

DEPENDENCIES
  None

RETURN VALUE
  temperature of XO, PMIC, and PA (in 2^-10 deg C)

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_temp_set_type tcxomgr_get_temp
(
   tcxomgr_adc_client_read_type   read_type
);

/*============================================================================

FUNCTION TCXOMGR_CHIPXN_COMP

DESCRIPTION
  Performs Chipxn compensation, called by a TCXOMGR_CHIPXN_COMP timer
  XO Compensation is a state machine with three states:
  1) Disabled : State when comp is turned off
  2) RESET    : We need to start compensation based on RGS. We read RGS
                and limit the change based on RGS.
                Once we have SW control we move to Enabled state
  3) ENABLED  : We have SW control and have a previous error which we
                corrected for. Will move back to reset state if we lose
                SW control.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_chipxn_comp
(
   tcxomgr_temp_set_type  temp,
   uint32                 temp_channel
);


/*============================================================================

FUNCTION TCXOMGR_GET_STRUCT_SIZE

DESCRIPTION
  This function returns the size of tcxomgr struct

DEPENDENCIES

RETURN VALUE
  Size of tcxomgr structure

SIDE EFFECTS
  None

============================================================================*/

size_t tcxomgr_get_struct_size(void);

/*===========================================================================

FUNCTION TCXOMGR_EFS_VERSION_READ

DESCRIPTION    This function reads version number from the EFS file

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
uint32 tcxomgr_efs_version_read
(
  tcxomgr_efs_type *efs,
  uint32 num_bytes
);

/*===========================================================================

FUNCTION TCXOMGR_EFS_VERSION_WRITE

DESCRIPTION    This function writes version number to the EFS file

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
int32 tcxomgr_efs_version_write
(
  tcxomgr_efs_type *efs,
  uint32 version
);

/*===========================================================================

FUNCTION TCXOMGR_VERIFY_FAC_CAL_VER

DESCRIPTION    This function compares fac cal version and resets
               field cal data if mismatch occurs

DEPENDENCIES   None.

RETURN VALUE   TRUE if matches, false otherwise

SIDE EFFECTS   None.

===========================================================================*/
boolean tcxomgr_verify_fac_cal_ver
(
  /*field cal version for comparision*/
  const uint32 field_cal_ver
);


/*===========================================================================

FUNCTION TCXOMGR_EFS_VERIFY_FAC_CAL_DATA

DESCRIPTION    This function writes version number to the EFS file

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/

boolean tcxomgr_verify_fac_cal_data(void);

/*============================================================================

FUNCTION TCXOMGR_FAST_FIELD_CAL_SET_DEFAULT

DESCRIPTION
  This function resets the field calibration to default state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_fast_field_cal_set_default(void);


/*============================================================================

FUNCTION TCXOMGR_UPDATE_TEMP_CACHE

DESCRIPTION
  This function updates the central cache time and temp with the passed values.

DEPENDENCIES
  Function assumes both XO and PMIC temp will be valid most of the time.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_update_temp_cache
(
  /* Temp reading result */
  tcxomgr_temp_set_type *temp,

  /* Time the temp was read in ms */
  uint32 read_time,

  /* Channel read */
  tcxomgr_adc_client_therm_type channel
);


/*============================================================================

FUNCTION TCXOMGR_EFS_FIELD_CAL_PARAM_READ

DESCRIPTION
  This function reads the field calibration parameters from the EFS.
  The parameters are read for GPS and WWAN sturctures.

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  If the EFS is not present or the read fails then default values are
  returned.

============================================================================*/
void tcxomgr_efs_field_cal_params_read( void );

/*============================================================================

FUNCTION TCXOMGR_EFS_FIELD_CAL_PARAM_WRITE

DESCRIPTION
  This function writes the field calibration parameters from the EFS.
  The parameters are written for GPS and WWAN sturctures.


DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_efs_field_cal_params_write( boolean async );

/*============================================================================

FUNCTION TCXOMGR_EFS_VCO_READ

DESCRIPTION
  This function reads the FT table and populatest the VCO structure.
  The VCO strucre is read by Field Cal during init and Field FT table
  is populated based on the data.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_efs_vco_read ( void );

/*============================================================================

FUNCTION TCXOMGR_EFS_FIELD_AGING_DATA_READ

DESCRIPTION
  This function reads the field aging data from the EFS.
  The parameters are read for GPS and WWAN sturctures.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the EFS is not present or the read fails then default values are
  returned.

============================================================================*/
void tcxomgr_efs_field_aging_data_read( void );

/*============================================================================

FUNCTION TCXOMGR_EFS_FIELD_AGING_DATA_WRITE

DESCRIPTION
  This function writes the field aging data from the EFS.
  The parameters are written for GPS and WWAN sturctures.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_efs_field_aging_data_write( void );

/*============================================================================

FUNCTION TCXOMGR_EFS_VCO_WRITE

DESCRIPTION
  This function copies the data from VCO structure to an EFS structure and
  uses efs ft write function to write the structure to the EFS.

  If table is TCXOMGR_TABLE_MAX, this function will synchronously write both
  the WWAN and GPS FT tables to EFS. This is intended for use during factory
  calibration.

  If table is TCXOMGR_TABLE_WWAN or TCXOMGR_TABLE_GPS, this function will
  asynchronously write the specified FT table to EFS. This is intended for use
  during field calibration, where the Tcxomgr task should not be blocked while
  the EFS write takes place.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_efs_vco_write ( tcxomgr_ft_table_type table );

/*============================================================================

FUNCTION TCXOMGR_EFS_WRITE_XTAL_TYPE

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  Result: True if successfull, False otherwise

SIDE EFFECTS
  None

============================================================================*/

int32 tcxomgr_efs_write_xtal_type
(
  uint32 xtal_type
);

/*===========================================================================

FUNCTION TCXOMGR_PROCESS_TEMP_CHANGE

DESCRIPTION    This function processes the new temp reading and informs the
  various users of temp reads like temp cache, freqest and adc recal.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void tcxomgr_process_temp_change
(
  tcxomgr_temp_cmd_type temp_cmd
);

/*============================================================================

FUNCTION TCXOMGR_PROCESS_CXN_COMP_CMD

DESCRIPTION
  Handles the chipxN compensation start/stop command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_process_cxn_comp_cmd
(
  tcxomgr_cxn_comp_cmd_type  cmd
);


/*============================================================================

FUNCTION TCXOMGR_LINEAR_REG

DESCRIPTION
    This function calculates the slope and intercept for the given x and y
    input arrays.

DEPENDENCIES
    None

RETURN VALUE
    This function returns true if the values of slope and intercept can
    be calculated based on the input values. The values of slope and
    intercept are assigned to the pointer locations passed as inputs
    to the function.
    This function returns false if the inputs provided to the function
    are incorrect.

SIDE EFFECTS
    None

============================================================================*/
boolean tcxomgr_linear_reg
(
const int32  in_count,         /* Number of samples collected     */
const int32* const temp,       /* Array of (t-t0) measurements    */
const int16* const freq,       /* Array of frequency measurements */
int32* slope,                  /* calulated   slope*/
int32* intercept,               /* calculated intercept */
int32  scaling_fac,             /* Scaling Factor to be used for Slope */
boolean scale_denom             /* Scaling Factor to be used for Slope */
);


/*============================================================================

FUNCTION TCXOMGR_GETSTDEV

DESCRIPTION
    This function calclulates the square of stdev for the input array. If the
    input array is null or f the count is zero, invalid stdev is returned.

DEPENDENCIES
    None

RETURN VALUE
    Square of Standard Deviation if inputs are valid
    Invalid STDEV if inputs are not valid

SIDE EFFECTS
    None

============================================================================*/
int32 tcxomgr_getstdev
(
  const int32 * const a,
  const int32 in_count
);


/*============================================================================

FUNCTION TCXOMGR_CHECK_IFC_STATUS

DESCRIPTION
  Return True if IFC should run
  return FALSE if IFC should stop

DEPENDENCIES
  None

RETURN VALUE
  True/False

SIDE EFFECTS
  None

============================================================================*/
boolean tcxomgr_check_ifc_status(void);


/*============================================================================

FUNCTION TCXOMGR_EFS_READ_CRYSTAL_TYPE

DESCRIPTION
  This function reads the NV to determine the crystal being supported

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_efs_read_crystal_type ( void );


/*============================================================================

FUNCTION TCXOMGR_FT_EVAL_TABLE

DESCRIPTION
  This function calculates the frequency error in PPM expected based on a
  given set of temperatures. This function is not supported outside of the
  FEATURE_XO define

DEPENDENCIES
  None

RETURN VALUE
  Frequency Error in PPM Units

SIDE EFFECTS
  The fixed point implementation loss is about 2e-4 ppm on the frequency
   estimate returned

============================================================================*/
tcxomgr_ppm_type tcxomgr_ft_eval_table
(
  tcxomgr_temp_set_type      temp,                /* current temperature  */
  tcxomgr_field_cal_info     *field_cal_data      /* Field Cal Ptr        */
);

/*============================================================================

FUNCTION TCXOMGR_FT_VALID_TABLE

DESCRIPTION
  This function returns TRUE if the FT curve has been calibrated and is valid

DEPENDENCIES
  None

RETURN VALUE
  True/False depending on FT cal state

SIDE EFFECTS
  None

============================================================================*/
boolean tcxomgr_ft_valid_table
(
  tcxomgr_ft_table_type    table
);


/*============================================================================
FUNCTION TCXOMGR_WRITE_MUX

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

============================================================================*/

void tcxomgr_write_mux
(
  tcxomgr_source_type    source
);


/*============================================================================

FUNCTION TCXOMGR_SEND_NOTIFICATIONS

DESCRIPTION
  This function sends a notification to all clients that have registered a
  notification callback (assuming the notification level has changed).

DEPENDENCIES
  TCXO Manager must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================*/
void tcxomgr_send_notifications
(
  tcxomgr_restriction_type  restriction
);

/*============================================================================

FUNCTION TCXOMGR_IFC_RESET_EFS_ITEMS

DESCRIPTION
  Resets the IFC related EFS items.
  Used to set IFC back to factory settings

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_ifc_reset_efs_items(void);


/*============================================================================

FUNCTION TCXOMGR_IFC_IS_RPUSH_PRIORITY_HIGH

DESCRIPTION
  if c1 has been calculated atleast three times by one of the enabled techs,
  then rpush priority for IFC is low otherwise it is high

DEPENDENCIES
  None

RETURN VALUE
  TRUE if IFC rpush priority is high
  FALSE otherwise

SIDE EFFECTS
  None

============================================================================*/
boolean tcxomgr_ifc_is_rpush_priority_high(void);

/*============================================================================

FUNCTION TCXOMGR_FLUSH_DATA_TO_NV

DESCRIPTION
  This function flushes TCXOMGR data to NV

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_flush_data_to_nv(void);

/*============================================================================

FUNCTION TCXOMGR_GET_STORED_VCO_PROCESSING

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_info_type

SIDE EFFECTS
  None

============================================================================*/

tcxomgr_vco_info_type tcxomgr_get_stored_vco_processing
(
  tcxomgr_log_rgs_retrieval_data_struct_type* log_ptr
);

/*============================================================================

FUNCTION TCXOMGR_INITIATE_TEMP_TEAD

DESCRIPTION
  This function is called everytime temp read timer expires. We check the subscribers
  list and get the temp channel info from subscribers that have their read period
  completed and mark them so that the temperature is reported after the channel
  read is completed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_initiate_temp_read(void);

boolean tcxomgr_cal_check_temp_span(tcxomgr_temp_type temp);
/*============================================================================

FUNCTION tmfe_is_gps_active

DESCRIPTION
  This function checks if the test tree is using the min required temp span

  Tells if GPS is in session or not.
  This function checks if the test tree is using the min required temp span

DEPENDENCIES
  None

RETURN VALUE
  boolean, TRUE if GPS is active, FALSE if not

SIDE EFFECTS
  None

============================================================================*/
boolean tmfe_is_gps_active(void);

/*===========================================================================

FUNCTION TCXOMGR_TEMP_SUBSCRIBE_INIT

DESCRIPTION    This function initializes temp subscription variables

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void tcxomgr_temp_subscribe_init( void );

/*===========================================================================
FUNCTION TCXOMGR_HANDLE_SIGS

DESCRIPTION
  This function handles TCXO Manager signals.

DEPENDENCIES
  REX is running.

RETURN VALUE


SIDE EFFECTS

===========================================================================*/

void tcxomgr_handle_sigs( rex_sigs_type sigs );
/*============================================================================

FUNCTION TCXOMGR_LOG_TEMP_SUBS_TO_DIAG

DESCRIPTION
    function to send the temp subscription log packets to diag

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None

============================================================================*/
void tcxomgr_log_temp_subs_to_diag
(
  tcxomgr_log_temp_subscription_struct_type *temp_data
);
/*============================================================================

FUNCTION TCXOMGR_SYNC_EFS_DATA

DESCRIPTION
  This function can be called after EFS writes to make sure that all data has
  been synced to the RFS.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_sync_efs_data (void);

/*============================================================================

FUNCTION TCXOMGR_DETERMINE_RPUSH_MODE

DESCRIPTION
 This function determines the current Rpush mode and also triggers an
 Rpush mode callback to clients to notify them of the changes.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_determine_rpush_mode( void );

/*============================================================================

FUNCTION TCXOMGR_SET_DEFAULT_RGS_INFO

DESCRIPTION
  This API resets the RGS values from the TCXOMGR data structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_set_default_rgs_info (tcxomgr_vco_info_type* ft);

/*============================================================================

FUNCTION TCXOMGR_GET_STORED_VCO_XO

DESCRIPTION
  VCO processing function for XO crystals.

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_info_type

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_vco_info_type tcxomgr_get_stored_vco_xo
(
  /* Pointer to the RGS values */
  tcxomgr_rgs_struct_type*                    rgs_ptr,

  /* Pointer to logging data                          */
  tcxomgr_log_rgs_retrieval_data_struct_type* log_ptr
);

/*============================================================================

FUNCTION TCXOMGR_FAST_FIELD_UPDATE_INFO

DESCRIPTION
  This function is used to update the field cal information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_fast_field_update_info (tcxomgr_field_cal_update_info_type* update_info);

void tcxomgr_update_temp_table
(
  /* VCTCXO accumulator value (TRK_LO_ADJ PDM value) */
  tcxomgr_vco_type vco_acc,

  /* Time (in seconds) since 6 JAN 1980 00:00:00 */
  uint32 time,

  /* Temperature */
  tcxomgr_temp_type temp32
);

/*============================================================================

FUNCTION TCXOMGR_GET_INT_PPB_UNCERTAINITY

DESCRIPTION
   This function returns the uncertainity with the FT curve based on
   the state and temparature.

DEPENDENCIES
  None

RETURN VALUE
  uint8

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_ppm_type tcxomgr_get_int_ppb_uncertainty
(
  /* Temperature */
  tcxomgr_temp_type      temp,

  /* VCO type */
  tcxomgr_vco_type       vco_type,

  /*state*/
  tcxomgr_field_state_type  state
);

/*============================================================================

FUNCTION TCXOMGR_REPORT_CAL_COEFF_UPDATE

DESCRIPTION
  This function collects the cal data and calls the GPS call back function to
  report the current cal data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_report_cal_coeff_update (void);

/*============================================================================

FUNCTION TCXOMGR_SCHEDULE_FIELD_TRIM

DESCRIPTION
  This function sends a command to the Tcxomgr task to adjust the XO trim
  value prior to field calibration, if the device does not have a valid XO
  factory calibration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_schedule_field_trim(int32 delta_c0);

/*============================================================================

FUNCTION TCXOMGR_ADJUST_FIELD_TRIM

DESCRIPTION
  This function adjusts the XO trim value prior to field calibration, if the
  device does not have a valid XO factory calibration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  May adjust the XO trim value in NV

============================================================================*/
void tcxomgr_adjust_field_trim(int32 delta_c0);

#endif /* !TCXOMGR_I_H */

