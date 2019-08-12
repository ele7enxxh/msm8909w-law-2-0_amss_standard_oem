 /** @file gps_common.h
    This file contains the structure definitions and function
    prototypes relevant for the entire GPS module (essentially
    the shared structures).
 */
#ifndef GPS_COMMON_H
#define GPS_COMMON_H

/*=============================================================================

                             GPS Common Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes 
  relevant for the entire GPS module (essentially the shared structures).

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Copyright (c) 2015 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.



=============================================================================*/


/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/common/inc/gps_common.h#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 



when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/30/14   md      Updated TRM APIs for Jolokia modem
07/25/14   rk      Removed obsolete LTE band function (instead use the ext. LTE API)
05/19/14   gk      Added Band 26 for the LTE band table
05/07/14   rk      Enhanced earfcn to support extended value for LTE.
01/15/14   mj      Changing stucture cgps_SSInfoType that shares CM events with TLE and ME
03/20/13   mj      Increment GPS_MAX_CALLBACK_CLIENTS to 16
03/11/13   mj       Add set of max record lengths for different records for XTRA1
02/15/13   mj      Added new function gnss_display_cb_logs to display callbacks made
12/06/12   mj      Min and Max length of a single fit interval in hours for GPS
12/03/12   rk      Updated GPS fix report & measurement struct with SV extension parameters.
08/21/12   mj      Modified gps_meas_status_bits_check
01/03/11   gk      Eph throttling rework
09/16/10   jlp     Updating cgps_SSInfoType to contain NID as well has to always
                   contain the subscription id (whether DSDS or not).
06/04/10   rb      Added cgps_MagDeviation to compute Magnetic Deviation based on Lat/Lon
12/2/09    gk      intermediate position fixes. added earth radius
01/29/10   pn      Updated DmLog structs with PACKED_POST.
11/12/09    gk     Add max uint16 and uint32
10/30/09   jlp     Removed celldb delete masks for region.  These will be tied
                   to existing delete bits instead.
09/30/09   gk      On-demand Positioning 2.0 support
07/31/09   gk      On-demand Positioning support
06/17/09   jd      Added FEATURE_TRM_API_V3 - GPS support for new TRM APIs 
                   done for SVDO and PAM
06/08/09   jlp     Added structures for injecting sndb regions.
03/16/09    ns     Add POS_SRC_WIFI to 0x1370
12/11/08    jb     Log velocity Unc the same on 0x1370 and 0x13E1
07/28/08    gk     Add a flag for background fixes
06/12/08    lt     Support for external coarse position injection.
06/09/08    gk     Add a flag for background fixes
04/17/08    jb     Add a flag word to gps_NavSolutionExtra to be used for various new flags
03/21/08   JB      0x1370, CGPS Pos log, output on sensor-only, add sensor PV flags 
02/14/08    jw     Support for Optimistic Punc
09/14/07    ld     Added support for Deferred Receiver OFF functionality
09/07/07   br      Added support for programming Antenna 1 Clock Switch when
                   TRM lock is granted.
08/23/07    jw     1. Centralized the SV meas_status fields to this file so that
                   it will be common for both ME and PE.
                   2. Added carrier phase fields to the meas structure.
08/06/07   rw      Added support for XTRA
07/31/07    jw     Added Tentative MEAS status
07/22/07    df     Added carrier phase derived Doppler estimate (aka FineSpeed)
06/18/07    ld     Added support for Deleting Cell DB Information.
05/31/07   jw      Added function cgps_GpsTimeDiffMsec
05/07/07   br      Added support for running PPM in MC task.
05/04/07    ld     Added support CellDB in CDMA systems
04/24/07   ank     Added support for CELL_ID_DB feature.
04/17/07   ank     Added cgps_SrvSystemType enum.
04/13/07   jd      Merged the LogDMGpsFix function into the cgps_logGpsFix function
03/30/07   ld      TimeEstPut DM Log related changes
03/27/07   mr      Added cgps_logGpsFix() for loggin 0x1370 GPS position 
03/15/07   jw      Added Latched values to the TT info sent to MC.
03/13/07   br      Added more support for TCXO Manager Rotator Push.
02/22/07   jw      Added additional fields to the PRM report for measurement
                   combining.
02/20/07   br      Added support for TCXO Manager Rotator Push.
02/12/07   va      Added Pilot PN field in PPM report
02/08/07   ld      Added a macro to handle unused parameter lint errors.
02/05/07   br      Added CDMA RTC field in PPM report structure.
11/17/06   br      Added support for 1x SRCH interface.
09/12/06   jw      Added new RESTART SV state.
09/01/06   jw      Added new VERIFY_TRACK SV state.
03/21/06   ld      Initial version.
08/17/06   jgd     Added f_ElevUnc field to gps_AzElInfoStructType structure.
                   Corrected spelling of d_Latitude field in gps_RefLocStructType structure.

=============================================================================*/

#include "gps_variation.h"
#ifdef WIN32
#pragma once
#endif

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/

#include "customer.h"
#include "comdef.h"
#include "gps_types.h"
#include "gps_consts.h"

/* Added to support changes to gps_NavSolution definition */
#include "gnss_consts.h"

#include "cgps_api.h"
#include "tcxomgr.h"
#include "trm.h"
#include "sys.h"
#include "queue.h"
#include "qw.h"
#include "cmd.h"
#include "timetick.h"
#include "log.h"
#include "msg.h"
#include <stddef.h>
#include "memheap.h"
#include "err.h"
#include "pdapi.h"
#include "cm.h"
/* LTE API available always (even on non-LTE builds) */
#include "lte_as.h"

#if defined(FEATURE_CGPS_UMTS_CELLDB) || defined(FEATURE_FSM_V2)
#include "rr_lsm_if.h"
#include "rrclsmif.h"
#endif /* FEATURE_CGPS_UMTS_CELLDB */

#ifdef FEATURE_CGPS_LTE_CELLDB
  #include "lte_rrc_ext_api.h"
#endif /* FEATURE_CGPS_LTE_CELLDB */

/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*=============================================================================

       Constants

=============================================================================*/

#define C_GPS_SV_ALM_INVALID  255

/*=============================================================================

       Macros

=============================================================================*/

#ifndef CGPS_UNUSED_ARG
#define CGPS_UNUSED_ARG(x) x = *(&x);
#endif

#define MAX_NMEA_SENTENCE_LEN 200

/*=============================================================================

       Typedefs

=============================================================================*/

#define EARTH_RADIUS        6378000.0F  /* meters */
#define CGPS_WHOLE_EARTH_CEP99 (10000000.0F * 2.15f) /* in meters */
#define CGPS_WHOLE_EARTH_UNC (CGPS_WHOLE_EARTH_CEP99 *  0.7071067811865F) /* in meters */
#define SRCH_PPM_DB_MAX_PILOTS  25
  /* Maximum number of different pilots whose info and PPM can be stored in
     PPM Data Base. */

#define SRCH_MAX_PPM_RM_RPT_SAMPLES  SRCH_PPM_DB_MAX_PILOTS
  /* Maximum Representative measurements (of the Pilot phase measurements)
     that can be reported */

/*keep this synced with the PDSM_BSSR_MAX_PILOTS define in pdsmi.h*/
#define PDSM_SRCH_MAX_BSSR_PILOTS 0xF /*Max pilots supported in BSSR message*/

/* The invalid state for user heading */
#define CGPS_INVALID_USER_HEADING   -999.0F

#define NF_NO_INFO           0x00000000
#define NF_FIX_2D            0x00000001
#define NF_FIX_3D            0x00000002
#define NF_FIX_OVERD         0x00000004
#define NF_TOO_FEW_SVS       0x00000008
#define NF_DIFFERENTIAL      0x00000010
#define NF_NOT_UPDATED       0x00000020
#define NF_EXTERNAL_UPDATE   0x00000040
#define NF_VELOCITY_FIX      0x00000080
#define NF_ITAR_VIOLATION    0x00000100
#define NF_FIX_SFT           0x00000200
#define NF_FIX_PROPAGATED    0x00000400
#define NF_FIX_IS_KF         0x00000800
#define NF_WLS_FALL_BACK     0x00001000
#define NF_SV_SRC_ALM_CORR   0x00002000
#define NF_SV_SRC_HYBRID     0x00004000

/* Flags for q_GpsFlagsExtra in gps_NavSolutionExtra */
enum GpsFlagsExtra
{
  GPS_FLAGS_EXTRA_MODE_GNSS_INS       = 0x00000001, /* Position Source Hybrid mode, using GPS and INS */
  GPS_FLAGS_EXTRA_MODE_INS_ONLY       = 0x00000002, /* Position Source No GPS, using INS to report fixes */
  GPS_FLAGS_EXTRA_INS_MASK            = 0x00000003, /* Position Source Mask for INS flags */
  GPS_FLAGS_EXTRA_HEADING_VALID       = 0x00000004, /* Position Source bit for heading validity */
  GPS_FIX_EXTRA_GPS_ONLY              = 0x00000008, /* Solution is/is not GPS only */
  GPS_FIX_EXTRA_GLO_ONLY              = 0x00000010, /* Solution is/is not Glonass only */
  GPS_FIX_EXTRA_MIXED_MEAS            = 0x00000020, /* Solution is/is not mixed meas */
  GPS_FIX_EXTRA_ALT_CONSTR            = 0x00000040, /* Solution is/is not alt constrained */
  GPS_FIX_EXTRA_GGTB_CONSTR           = 0x00000080, /* Solution is/is not GGTB constrained */
  GPS_FIX_EXTRA_EPI_CONSTR            = 0x00000100, /* Solution does/does not use the EPI as a constraint */
  GPS_FIX_EXTRA_POS_SRC_GILE          = 0X00000200, /* Position Source is GILE  */
  GPS_FIX_EXTRA_PV_FILTERED           = 0X00000400, /* Solution is/is not PV filtered */
  GPS_FLAGS_EXTRA_SV_USAGE_INFO_VALID = 0X00000800, /* SV usage info is/is not valid */
  GPS_FLAGS_EXTRA_XMSI_KNOWN          = 0X00001000, /* eXternal Mounted State Indicator (XMSI) is known */
  GPS_FLAGS_EXTRA_XMSI_MOUNTED        = 0X00002000, /* XMSI is mounted */
  GPS_FLAGS_EXTRA_MSI_KNOWN           = 0X00004000, /* MSI is known */
  GPS_FLAGS_EXTRA_MSI_MOUNTED         = 0X00008000, /* MSI is mounted */
  GPS_FLAGS_EXTRA_CRD_DETECTED        = 0X00010000, /* Cradle Rotation Detector (CRD) */
  GPS_FIX_EXTRA_NHC_APPLIED           = 0X00020000,  /* Solution has Non-Holonomic Constraint (NHC) applied */
  GPS_FLAGS_EXTRA_WIFI_ONLY           = 0X00040000, /* Solution is/is not Wifi only */
  GPS_FLAGS_EXTRA_POS_SRC_PLE         = 0X00080000  /* Position source is PLE */
};

/* Flags for w_PosVelFlag in cgps_PosDmLog */
enum PosVelFlags
{
  POS_VEL_FLAG_NONE                   = 0x000,  /* No flags */

  /* Following flags valid for u_PosSrc == POS_SRC_WLS || u_PosSrc == POS_SRC_KF */
  POS_VEL_FLAG_ALTITUDE_AIDED         = 0x0001, /* Altitude aided */
  POS_VEL_FLAG_FIX_SFT                = 0x0002, /* Solve for time fix */
  POS_VEL_FLAG_FIX_OVERD              = 0x0004, /* Over-determined (WLS only) */
  POS_VEL_FLAG_POS_REQ_RESP           = 0x0008, /* 0=Periodic Position Report 1=Reponse to a Position Request */
  POS_VEL_FLAG_DIFFERENTIAL           = 0x0010, /* Differential correction */
  POS_VEL_FLAG_FIX_CLOCK_BIAS_HOLD    = 0x0020, /* Clock bias hold fix (WLS only) */
  POS_VEL_FLAG_FIX_PROPAGATED         = 0x0040, /* Position fix propagated (WLS only) */
  POS_VEL_FLAG_VELOCITY_FIX           = 0x0080, /* Velocity fix valid */
  POS_VEL_FLAG_FIX_FINAL              = 0x0100, /* 0=Unknown (sent from NF) 1=Final fix from LM */
  POS_VEL_FLAG_FIX_ALMANAC_CORR       = 0x0200, /* Position fix from almanac corrections */
  POS_VEL_FLAG_FIX_EPH_ALMANAC_CORR   = 0x0400, /* Position fix from ephemeris and almanac corrections */
  POS_VEL_FLAG_FIX_GPS_INS            = 0x0800, /* Position fix from inertial and GPS measurements (KF only) */
  POS_VEL_FLAG_FIX_INS_ONLY           = 0x1000, /* Position fix from inertial measurements only (KF only) */
  POS_VEL_FLAG_FIX_BACKGROUND         = 0x2000, /* 0=Not a background fix 1=Background fix */
  POS_VEL_FLAG_FIX_PE_GENERATED       = 0x4000, /* 1=PE Generated fix report, 0= LM generated fix report*/
  POS_VEL_FLAG_FIX_ME_GENERATED       = 0x8000, /* 1= ME Generated fix, */

  /* Following flags valid for u_PosSrc == POS_SRC_EXT || u_PosSrc == POS_SRC_INTDB */
  POS_VEL_FLAG_EXT_INT_POS_REQ_RESP   = 0x0080, /* 0=Periodic Position Report 1=Reponse to a Position Request */
};

/* The MEAS_STATUS defines identify the bits store in the w_MeasStatus field
   of the me_Meas data structure. */
#define MEAS_STATUS_NULL    0 /* The don't know anything state */
#define MEAS_STATUS_SM_VALID  (1<<0)  /* TRUE -> f_SatSubMs is valid */
#define MEAS_STATUS_SB_VALID  (1<<1)  /* TRUE -> sub bit time known */
#define MEAS_STATUS_MS_VALID  (1<<2)  /* TRUE -> satellite time is known */
#define MEAS_STATUS_BE_CONFIRM  (1<<3)  /* TRUE -> bit edge confirmed from signal */
#define MEAS_STATUS_VE_VALID  (1<<4)  /* TRUE -> measured velocity */
#define MEAS_STATUS_VE_FINE   (1<<5)  /* TRUE/FALSE -> fine/coarse velocity */
#define MEAS_STATUS_LP_VALID  (1<<6)  /* TRUE/FALSE -> Lock Point is valid/invalid */
#define MEAS_STATUS_LP_POS    (1<<7)  /* TRUE/FALSE -> Lock Point positive/negative */
#define MEAS_STATUS_GOOD_DATA (1<<8)  /* TRUE -> Good satellite data parity */
#define MEAS_STATUS_FROM_RNG_DIFF (1<<9)/* TRUE -> Last update from range diff, not range meas */
#define MEAS_STATUS_FROM_VE_DIFF (1<<10)/* TRUE -> Last update from velocity diff, not vel meas */
#define MEAS_STATUS_XCORR   (1<<11) /* TRUE -> Strong indication of cross correlation */
#define MEAS_STATUS_TENTATIVE (1<<12) /* TRUE -> Had multipath indicators */
#define MEAS_STATUS_DONT_USE  (1<<13) /* TRUE -> Dubious measurement, wait for another dwell */
#define MEAS_STATUS_NEED_SIR_CHECK (1<<14) /* TRUE -> An Sir check is needed for this SV */
#define MEAS_STATUS_ACQ_OR_PROBATION (1<<15) /* TRUE -> In ACQ or Sanity check puts meas in probation */

#define MEAS_STATUS2_SV_SRC_ALM_CORR (1<<0) /* TRUE -> Almanac corrections used for SV position source */
#define MEAS_STATUS2_SV_SRC_EPH (1<<1) /* TRUE -> Ephemeris used for SV position source */

/* Code below provides a mapping between ME version of time source vs 
   the time source that is seen by the app/client as part of the
   Time Source TLV.
*/

#define C_TIME_SRC_INVALID                    PDSM_GNSS_TIME_SRC_INVALID   
#define C_TIME_SRC_UNKNOWN                    PDSM_GNSS_TIME_SRC_UNKNOWN

#define C_TIME_SRC_GPS_TOW_CONFIRMED          PDSM_GNSS_TIME_SRC_TOW_CONFIRMED
#define C_TIME_SRC_GPS_TOW_WEEK_CONFIRMED     PDSM_GNSS_TIME_SRC_TOW_WEEK_CONFIRMED
#define C_TIME_SRC_GPS_OTA                    PDSM_GNSS_TIME_SRC_GPS_TOW_DECODE
#define C_TIME_SRC_GLO_OTA                    PDSM_GNSS_TIME_SRC_GLO_TOW_DECODE
#define C_TIME_SRC_PE_CLK_REPORT              PDSM_GNSS_TIME_SRC_NAV_SOLUTION
#define C_TIME_SRC_PE_SOLVE_FOR_TIME          PDSM_GNSS_TIME_SRC_SOLVE_FOR_TIME
#define C_TIME_SRC_TIME_TRANSFORM             PDSM_GNSS_TIME_SRC_TIME_TRANSFORM
#define C_TIME_SRC_EXT_INJECT                 PDSM_GNSS_TIME_SRC_EXTERNAL_INPUT 
#define C_TIME_SRC_BDS_TOW_CONFIRMED          PDSM_GNSS_TIME_SRC_TOW_CONFIRMED
#define C_TIME_SRC_BDS_TOW_WEEK_CONFIRMED     PDSM_GNSS_TIME_SRC_TOW_WEEK_CONFIRMED
#define C_TIME_SRC_BDS_OTA                    PDSM_GNSS_TIME_SRC_BDS_TOW_DECODE
#define C_TIME_SRC_QZSS_OTA                   PDSM_GNSS_TIME_SRC_QZSS_TOW_DECODE
#define C_TIME_SRC_GAL_TOW_CONFIRMED          PDSM_GNSS_TIME_SRC_TOW_CONFIRMED
#define C_TIME_SRC_GAL_TOW_WEEK_CONFIRMED     PDSM_GNSS_TIME_SRC_TOW_WEEK_CONFIRMED
#define C_TIME_SRC_GAL_OTA                    PDSM_GNSS_TIME_SRC_GAL_TOW_DECODE

#define C_TIME_SRC_TIME_TRANSFER              PDSM_GNSS_TIME_SRC_NETWORK_TIME_TRANSFER
#define C_TIME_SRC_WCDMA_TIMETAG              PDSM_GNSS_TIME_SRC_NETWORK_TIME_TAGGING
#define C_TIME_SRC_GSM_TIMETAG                PDSM_GNSS_TIME_SRC_NETWORK_TIME_TAGGING
#define C_TIME_SRC_LTE_TIMETAG                PDSM_GNSS_TIME_SRC_NETWORK_TIME_TAGGING
#define C_TIME_SRC_TDSCDMA_TIMETAG            PDSM_GNSS_TIME_SRC_NETWORK_TIME_TAGGING
#define C_TIME_SRC_WCDMA_SLEEP_TIMETAG        PDSM_GNSS_TIME_SRC_NETWORK_TIME_TAGGING
#define C_TIME_SRC_GSM_SLEEP_TIMETAG          PDSM_GNSS_TIME_SRC_NETWORK_TIME_TAGGING
#define C_TIME_SRC_LTE_SLEEP_TIMETAG          PDSM_GNSS_TIME_SRC_NETWORK_TIME_TAGGING
#define C_TIME_SRC_TDSCDMA_SLEEP_TIMETAG      PDSM_GNSS_TIME_SRC_NETWORK_TIME_TAGGING

#define C_TIME_SRC_SLOW_CLOCK                 PDSM_GNSS_TIME_SRC_RESERVED
#define C_TIME_SRC_TIME_ALIGN                 PDSM_GNSS_TIME_SRC_RESERVED
#define C_TIME_SRC_PATTERN_MATCH              PDSM_GNSS_TIME_SRC_RESERVED


#ifdef FEATURE_LEGACY_GPS_DPO
/* DPO related definitions */

/* 26 dBHz is 3 dB higher than the sensitivity of the Receiver with 200ms observation */
#define C_GPS_CNO_THRESH_FOR_DPO  260
/*DPO 2.0 definitions*/
#define C_GPS_DPO_USABLE_SV_CNO_THRESH 260

/* MC needs at least one SV with CNO > 28 to enter DPO. 
   This is required to get the fine speed estimate in DPO. 
   In DPO, CC collects IQ sums from the correct bin when GPS CNO
   for the first 60 msecs is above 28 dB-Hz .
   If a singe SV is available above 28 dB-Hz, then CC collects good
   IQ sums for all the SVs by using a offset. This improves the fine
   speed availability in DPO.
   Therefore with a 2 dBHz margin, we could get fine speed without
   worrying about temporary minor fluctuations in the signal conditions.
*/
#define C_GPS_DPO_FINE_SPEED_SV_CNO_THRESH_ENTRY 280 
#define C_GPS_DPO_FINE_SPEED_SV_CNO_THRESH_EXIT 260

/* Changing the value from previous 22 dBHz to 24 dBHz as the 700ms 
   DPO dwell is removed. */
#define C_GPS_DPO_ALL_KNOWN_USABLE_SV_CNO_THRESH 240
#define C_GPS_DPO_ALL_KNOWN_USABLE_SV_ELEV_THRESH 7.11f //5 degrees (5 * 256/180) 
#define C_GPS_DPO_NOT_ALL_KNOWN_USABLE_SV_CNO_THRESH 370

/* HEPE limit to allow DPO */
#define  C_GPS_HEPE_THRESH_TO_ALLOW_DPO (float)25.0
#endif

/*Fix age threshold*/
#define C_GPS_DPO_FIX_AGE_THRESH 12

/*DPO Almanac AGE Threshold in weeks*/
#define C_GPS_DPO_ALM_AGE_THRESH (float)52.0

#define C_GNSS_CNO_THRESH_FOR_DPO  260
#define C_GNSS_DPO_USABLE_SV_CNO_THRESH 260
#define C_GNSS_DPO_FINE_SPEED_SV_CNO_THRESH_ENTRY 280 
#define C_GNSS_DPO_FINE_SPEED_SV_CNO_THRESH_EXIT 260
#define C_GNSS_DPO_ALL_KNOWN_USABLE_SV_CNO_THRESH 240
#define C_GNSS_DPO_ALL_KNOWN_USABLE_SV_ELEV_THRESH 7.11f  /*5 degrees (5 * 256/180)  */
#define C_GNSS_DPO_NOT_ALL_KNOWN_USABLE_SV_CNO_THRESH 370
#define C_GNSS_HEPE_THRESH_TO_ALLOW_DPO (float)25.0


#define C_GPS_ODP_MODE1_PUNC_UPPER_LIMIT (9200) /* this value is based on 63% conf 
                                                   PUNC returned from ME. corresponds
                                                   to 20k at 99% conf */
#define C_GPS_ODP_MODE1_TUNC_LOWER_LIMIT (80)   /* in us */
#define C_GPS_ODP_MODE1_TUNC_UPPER_LIMIT (160)  /* in us */

#define C_GPS_ODP_MODE2_TUNC_LIMIT (250)        /* in us */

#define CGPS_DOPPLER_UNC_INCREASE (FLT)1.5 /* meters per second, 1-sigma */

#define GPS_CHIPS_PER_MS_FLT   (FLT)1023.0
#define CDMA_CHIPS_PER_MS_FLT  (FLT)1228.8
#define CHIPS_PER_CX16_FLT     (FLT)16.0
#define CHIPS_PER_CX80_FLT     (FLT)80.0

/* Min and Max length of a single fit interval in hours for GPS */
#define GPS_XTRA_MIN_HOURS_FIT_INTERVAL (1)
#define GPS_XTRA_MAX_HOURS_FIT_INTERVAL (12)

/* Maximum number of clients for callback */
#define GPS_MAX_CALLBACK_CLIENTS (16)

/* Following are "Data size to follow" for various record lengths as per the XTRA1 specification 80-VF336-1 B */
#define GPS_EPHEMERIS_MUL_FACTOR  (66) /*Ephemeris multiplication factor. Record length = (Number of SVs * 66) */
#define GPS_ALMANAC_MUL_FACTOR    (31) /*Almanac multiplication factor. Record length = (Number of SVs * 31) */
#define GPS_IONO_MODEL_MAX_LEN      (8)  /* IONO record */
#define GPS_UTC_MODEL_MAX_LEN   (14) /* UTC record */
#define GPS_HEALTH_PAGE_MAX_LEN   (41) /* GPS Health record */
#define GPS_COMBINED_ALM_CORR_MUL_FACTOR (1176) /* Combined Almanac corrections multiplication factor. 
                            Record length= ((Number of SVs * (42 * 28)) + 1) */
    
#define TM_CM_MAX_NO_SUBS      (3) /* Max number of subs a device can have */
#define TM_CM_MAX_NO_STACKS    (CM_NO_STACKS) /* Max number of stacks per sub can have */

/*--------------------------------------------------------------------------
                      SEARCH - PDSM COMMAND CODES

The following codes are for use with the 'cmd' field of the search - PDSM
command header type defined below, to define the operation requested by the
command.
--------------------------------------------------------------------------*/

typedef enum
{
  SRCH_PDSM_START_PPM_F,
  SRCH_PDSM_GET_PPM_RESULTS_F,
  SRCH_PDSM_END_PPM_F,
  SRCH_PDSM_GET_A_C_SET_F,
  SRCH_PDSM_START_FTCAL_F,
  SRCH_PDSM_MAX_F     /* Tells number of commands. MUST be last enum item. */
}srch_pdsm_cmd_code_typeVal;
typedef uint8 srch_pdsm_cmd_code_type;

/* PDSM command return status */
typedef enum
{
  PDSM_DONE_S,      /* Function completed successfully */
  PDSM_BUSY_S,      /* Command is pending on queue */
  PDSM_BADSEQ_S,    /* Illegal sequencing of commands */
  PDSM_ERR_S,       /* Internal error occurred */

  PDSM_MAX_S        /* Tells number of status codes. MUST be last item
                       in enum */
}pdsm_cmd_status_typeVal;
typedef uint8 pdsm_cmd_status_type;

/*--------------------------------------------------------------------------
                      SEARCH  PDSM COMMAND HEADER TYPE

This header type is contained in every search - PDSM command. It contains info
common to all commands.
--------------------------------------------------------------------------*/

#define PPM_REQUEST_FROM_PPPM        0x00001000
#define PPM_REQUEST_FROM_LM_TM       0x00010000

typedef struct
{
  cmd_hdr_type          cmd_hdr;
    /* Includes the queue links,  pointer to requesting task TCB,
       signal to set on completion,  and done queue */

  srch_pdsm_cmd_code_type    cmd;
    /* Specifies which command to perform. See enumerated command codes
       above. */

  pdsm_cmd_status_type  status;
    /* Specifies the status of the command. Initially, this field is set
       to SRCH_BUSY_S by function srch_cmd. When command processing is
       finished, this field is set to one of the statuses enumerated
       above. */

  uint32                q_requesting_task;
}
srch_pdsm_cmd_hdr_type;

typedef enum
{
  NORMAL_PPM,
  QUICK_PPM,
  FAST_TCAL
} ppm_search_duration_typeVal;
typedef uint8 ppm_search_duration_type;
/*--------------------------------------------------------------------------
                     PD SESSION MANAGER START PPM COMMAND
This command tells searcher to start Pilot Phase Measurements
--------------------------------------------------------------------------*/
typedef struct
{
  srch_pdsm_cmd_hdr_type    hdr;
   /* Common header */

  boolean                   ftest_mode;
   /* GPS Factory Test Mode */

  ppm_search_duration_type  ppm_search_duration;
   /* PPM searching duration */
}
srch_pdsm_start_ppm_cmd_type;

/*--------------------------------------------------------------------------
                     PD SESSION MANAGER GET PPM RESULTS COMMAND
This command tells searcher to report Pilot Phase Measurements results to
PD Session Manager
--------------------------------------------------------------------------*/
typedef struct
{
  srch_pdsm_cmd_hdr_type  hdr;
   /* Common header */

  boolean                 end_ppm;
   /* This indicates to the SRCH whether to end the PPM or not after
   ** delivering the PPM results */
}
srch_pdsm_get_ppm_rslt_cmd_type;

/*--------------------------------------------------------------------------
                     PD SESSION MANAGER END PPM COMMAND
This command tells searcher to stop Pilot Phase Measurements
--------------------------------------------------------------------------*/
typedef struct
{
  srch_pdsm_cmd_hdr_type  hdr;
   /* Common header */
}
srch_pdsm_end_ppm_cmd_type;

typedef enum 
{
    REF_PILOT_ONLY,
    A_SET_ONLY,
    A_C_SET
} srch_pdsm_a_c_set_typeVal;
typedef uint8 srch_pdsm_a_c_set_type;
/*--------------------------------------------------------------------------
                     PDSM GET PNs COMMAND 
This command tells searcher to give the PDSM PNs in active and candidate sets.
--------------------------------------------------------------------------*/
typedef struct
{
  srch_pdsm_cmd_hdr_type  hdr;
   /* Common header */

  srch_pdsm_a_c_set_type return_set_type;
  /* Enum indicating that SRCH should return REF_PILOT  Only, A-set only or A_C_SET 
  */
}
srch_pdsm_get_a_c_set_cmd_type;

/*--------------------------------------------------------------------------
                     START FAST TCAL COMMAND 
This command tells searcher to restart Fast TCal PPM session.
--------------------------------------------------------------------------*/
typedef struct
{
  srch_pdsm_cmd_hdr_type  hdr;
   /* Common header */
}
srch_pdsm_start_ftcal_cmd_type;

/*--------------------------------------------------------------------------
                    UNION OF ALL PDSM - SEARCH COMMANDS

--------------------------------------------------------------------------*/

typedef union
{
  srch_pdsm_cmd_hdr_type          hdr;
  srch_pdsm_start_ppm_cmd_type    start_ppm;
  srch_pdsm_get_ppm_rslt_cmd_type get_ppm_results;
  srch_pdsm_end_ppm_cmd_type      end_ppm;
  srch_pdsm_get_a_c_set_cmd_type  get_a_c_set;
  srch_pdsm_start_ftcal_cmd_type  start_ftcal;
}
gpssrch_pdsm_cmd_type;


typedef enum
{
  SRCH_PPM_PHASE_NOT_FOUND = 0, /* Phase not found neither in latched or current DB */
  SRCH_PPM_LATCHED_DB_EMPTY,    /* The latched DB is empty, phase not found */
  SRCH_PPM_PHASE_FOUND          /* Phase found either in latched DB or current */
}
srch_ppm_get_phase_status_typeVal;
typedef uint8 srch_ppm_get_phase_status_type;


/* Common Free queue used by searcher and pdsm to send req/rsp to each other.
*/
extern q_type pdsm_srch_msg_free_q;

/* Typedef enum of requests and responses between searcher and PDSM.
*/
typedef enum pdsm_msg_e {
    PDSM_SRCH_MSG_NONE = -1,        /* Internal use */
    PDSM_SRCH_MSG_PPM_RSP,          /* PPM response from Searcher to PDSM */
    PDSM_SRCH_MSG_PPM_EXIT,         /* Command from SRCH indicating exit of PPM */
    PDSM_SRCH_MSG_PN_INFO,          /* Message to give pn info back from srchr */
    PDSM_SRCH_MSG_E_END                     /* Internal use only */
} pdsm_srch_msg_e_typeVal;
typedef int8 pdsm_srch_msg_e_type;


/* Typedef header structure used in all Req/Rsp between searcher and PDSM.
*/
typedef struct pdsm_srch_hdr_s {
    q_link_type             link;       /* Link type element */
    pdsm_srch_msg_e_type    msg_type;   /* msg code type */
} pdsm_srch_hdr_s_type;

/* Typedef struct for Peak Pilot position.
*/
typedef struct cdma_peak {
    int32    peak_pos;
    uint16  peak_pn;
    byte    peak_rmse;
    byte    peak_ecio;
} cdma_peak_type;

/* typedef struct for PPM report from Searcher.
*/
typedef struct srch_ppm_rpt {
    qword          ts;
    boolean        offset_incl;
    word           mob_sys_t_offset;
    word           ref_pn;
    byte           ref_pn_ecio;
    byte           band_class;
    word           cdma_freq;
    byte           total_rx_pwr;
    uint32         cdma_rtc;
    byte           num_pilots;
    cdma_peak_type pilot_results[SRCH_MAX_PPM_RM_RPT_SAMPLES];
} srch_ppm_rpt_type;

typedef struct
{
  qword ts;
  word  ref_pn;
  word  mob_sys_t_offset;
} srch_msto_rpt_type;

/* typedef strut for PPM response from Searcher.
*/
typedef struct pdsm_srch_ppm_rsp {
    pdsm_srch_hdr_s_type       hdr;
    srch_ppm_rpt_type          srch_rpt;
    /* if msto_rpt_incl is TRUE, then the fields in the msto_rpt are */
    /* valid, and the results should be sent to the PDE.             */
    boolean                    msto_rpt_incl; 
    srch_msto_rpt_type         msto_rpt;
    /* The following variable is set TRUE if a HHO occurred */
    boolean                    add_pilots;
} pdsm_srch_ppm_rsp_type;

/* Return the current pns for Active and Candidate set 
*/
typedef struct pdsm_srch_pn_info {
  pdsm_srch_hdr_s_type      hdr;
  uint16                    num_pilots;
  word                      pn_offset[PDSM_SRCH_MAX_BSSR_PILOTS];
  word                      cdma_ch[PDSM_SRCH_MAX_BSSR_PILOTS];      /* CDMA channel number */
  byte                      band_class[PDSM_SRCH_MAX_BSSR_PILOTS];   /* Band class */
} pdsm_srch_pn_info_type;

/* Typedef struct for all the response/request from Searcher to PDSM.
*/
typedef union pdsm_srch_msg_u {
    pdsm_srch_hdr_s_type         hdr;       /* Commands with no data can just use this. */
    pdsm_srch_ppm_rsp_type       ppm_rsp;   /* PPM results from Searcher */
    pdsm_srch_pn_info_type       pn_info_rpt; /*report pns in active and candidate sets*/
} pdsm_srch_msg_u_type;  


#if defined(WIN32) && defined(ADS_120_COMPILER_4_BYTE_ALIGNMENT)
#pragma pack(push, GPS_COMMON_HH, 4)
#endif


/*=============================================================================
  Source of Time
=============================================================================*/

/*
typedef enum
{
  C_TIME_SRC_INVALID = 0,
  C_TIME_SRC_GPS_OTA,
  C_TIME_SRC_GLO_OTA,
  C_TIME_SRC_PE_CLK_REPORT,
  C_TIME_SRC_TIME_TRANSFORM,
  C_TIME_SRC_TIME_TRANSFER,
  C_TIME_SRC_WCDMA_TIMETAG,
  C_TIME_SRC_GSM_TIMETAG,
  C_TIME_SRC_WCDMA_SLEEP_TIMETAG,
  C_TIME_SRC_PUT_GSM_SLEEP_TIMETAG,
  C_TIME_SRC_UNKNOWN,
  C_TIME_SRC_MAX
} mgp_SrcOfTimeEnumType;  
*/

typedef enum
{
  C_FREQ_SRC_INVALID = 0,
  C_FREQ_SRC_TRANSFER,
  C_FREQ_SRC_EXTERNAL,
  C_FREQ_SRC_GPS_SV_MEAS,
  C_FREQ_SRC_GLO_SV_MEAS,
  C_FREQ_SRC_PE_CLK_REPORT,
  C_FREQ_SRC_UNKNOWN,
  C_FREQ_SRC_MAX
} mgp_SrcOfFreqEnumTypeVal;
typedef uint8 mgp_SrcOfFreqEnumType;  


/*=============================================================================
   GPS Clock structure

   The gps_Clock structure is used to describe the clock state in a
   number of other data structures. 

   Note that 

    TimeBias = UserTime - GpsTime 
   
    and

    FreqBias = UserFreq - GpsFreq

    Note that a positive frequency bias results in a clock that has
    a higher frequency than nominal. (Resulting in lower observed
    Dopplers).

    Note that the f_ClkTimeUncMs refers to the time in the week setting. 
    There are occasions when the time in the week is well known, but the
    GpsWeek has not been established. To account for this, the special
    value of C_GPS_WEEK_UNKNOWN (= 0xffff) is used.
=============================================================================*/

#define C_GPS_WEEK_UNKNOWN  0xFFFF

/**
  @brief GPS time data structure.  
*/
typedef struct
{
  uint16 w_GpsWeek;     /*!< GPS week number at reference tick [weeks] */
  uint32 q_GpsMsec;     /*!< GPS msec at reference tick [msecs] */
  FLT    f_ClkTimeBias; /*!< Clock bias [msecs]. Constrait to >-0.5 and <0.5 */
  FLT    f_ClkTimeUncMs;/*!< 1 sided max time bias uncertainty [msecs] */
  uint8                 e_SourceOfTime; /*!< Source of the time */
} gps_TimeStructType;

/**
  @brief GPS time data structure for DM logging 
*/
typedef PACKED struct PACKED_POST
{
  uint16 w_GpsWeek;     /*!< GPS week number at reference tick [weeks] */
  uint32 q_GpsMsec;     /*!< GPS msec at reference tick [msecs] */
  uint32 f_ClkTimeBias; /*!< Clock bias [msecs] */
  uint32 f_ClkTimeUncMs;/*!< 1 sided max time bias uncertainty [msecs] */
} gps_TimeStructType_ForDmLog;

/**
  @brief GPS frequency data structure. 
*/
typedef struct
{
  FLT f_ClkFreqBias;  /*!< Clock frequency bias. [m/s] */
  FLT f_ClkFreqUnc;   /*!< 1 sided max freq bias uncertainty [m/s] */
  mgp_SrcOfFreqEnumType e_SourceOfFreq; /*!< Frequency source */
} gps_FreqStructType;

/**
  @brief GPS frequency data structure for DM logging 
*/
typedef PACKED struct PACKED_POST
{
  uint32 f_ClkFreqBias;  /*!< Clock frequency bias. [m/s] */
  uint32 f_ClkFreqUnc;   /*!< 1 sided max freq bias uncertainty [m/s] */
} gps_FreqStructType_ForDmLog;

/**
  @brief GPS clock struct contains GPS time and clock time 
         stamps
*/
typedef struct
{
  uint32  q_RefFCount;         /*!< FCount value at reference tick */
  boolean u_SystemRtcValid;    /*!< System RTC validity */
  uint64  t_SystemRtcMs;       /*!< System RTC value at clock validity */
  uint32  q_NumClockResets;    /*!< Cumulative # of Clock resets. */
  gps_TimeStructType z_Time;   /*!< Time at w_RefFCount */
  gps_FreqStructType z_Freq;   /*!< Freq at w_RefFCount */
} gps_ClockStructType;

/**
  @brief GPS clock struct contains GPS time and clock time 
         stamps for DM log
*/
typedef PACKED struct PACKED_POST
{
  uint32  q_RefFCount;      /*!< FCount value at reference tick */
  boolean u_SystemRtcValid; /*!< System RTC validity */
  uint64  t_SystemRtcMs;    /*!< System RTC value at clock validity */
  uint32  q_NumClockResets; /*!< Cumulative # of Clock resets. */
  gps_TimeStructType_ForDmLog z_Time;   /*!< Time at w_RefFCount */
  gps_FreqStructType_ForDmLog z_Freq;   /*!< Freq at w_RefFCount */
} gps_ClockStructType_ForDmLog;



/*
   NOTE WELL: When revising the follwoing structure, 
   you MUST change the IPC version 
   numbers of all affected IPC messages in nf_iface.h
   - see CL 526195 for an example of how this is done, but make sure this
   is done for all relevant messages
*/
typedef struct
{
  /* CDMA RTC value latched at the last Sync80 tick (in CDMA Chipx8) */
  uint32  q_CdmaRtcSync80CCx8;

  /* GPS RTC value at Sync80 */
  /* Incorrect name. This field contains GNSS RTC sub-msec value at Sync80
     in GPS Chipx1280 units */
  uint32  q_GpsRtcSubMsSync80GCx32;

  /* CDMA RTC value latched at the same time as GPS RTC (in CDMA Chipx32) */
  /* Override q_CdmaRtcLatchedCCx32 with q_CdmaRfFeVariableDelayCCx8192
     to avoid adding new field to the structure and breaking all existing
     tools. q_CdmaRfFeVariableDelayCCx8192 presents a variable portion of
     CDMA RF Front End propagation delay (in CDMA Chipx8192 units).
     This delay is measured as the difference between CDMA RTC and
     VSRC STMR. */
  uint32  q_CdmaRtcLatchedCCx32;

  /* GPS RTC value latched at the same time as CDMA RTC (in GPS Chipx32) */
  uint32  q_GpsRtcLatchedGCx32;

  uint32  q_FCount;

} gps_CDMAClockInfoStructType;


/* Structure to store CDMA System Time with respect to FCount */
typedef struct
{
  /* FCount Reference for the System Time */
  uint32 q_RefFCount;
  /* CDMA Time. This is the GPS Time maintained by the current Pilot base-station */
  /* Note that the SysTime is offset from TRUE-GPS by some amount because of processing
     in the Base-Station (this offset is also called Forward-Link calibration value).
     To associate System Time (of the Base-Station) to the local clock, we need to know
     the distance between Phone and Base-Station, or assume the Time is uncertain
     by the maximum delay that can happen in a given Cell (given by MAR).
  */
  gps_TimeStructType z_SysTime;

} gps_CDMASysTimeStructType;

/*=============================================================================
   SV Observation State enumeration
=============================================================================*/
typedef enum
{
  C_GPS_SV_STATE_IDLE,
  C_GPS_SV_STATE_SEARCH,
  C_GPS_SV_STATE_SEARCH_VERIFY,
  C_GPS_SV_STATE_BIT_EDGE,
  C_GPS_SV_STATE_VERIFY_TRACK,
  C_GPS_SV_STATE_TRACK,
  C_GPS_SV_STATE_RESTART,
  C_GPS_SV_STATE_DPO_TRACK,
  C_GPS_SV_STATE_MAX

} gps_SvStateEnumTypeVal;
typedef uint8 gps_SvStateEnumType;


/*=============================================================================
   GPS Rcvr State enumeration
=============================================================================*/
typedef enum 
{
  C_GPS_RCVR_STATE_OFF,                /* Receiver State is OFF */
  C_GPS_RCVR_STATE_IDLE,               /* Receiver State is IDLE */
  C_GPS_RCVR_STATE_ON,                 /* Receiver State is ON */
  C_GPS_RCVR_STATE_KEEP_WARM_DFRD_OFF,  /* Receiver State is Keep Warm (Deferred Rcvr OFF) */
  C_GPS_RCVR_STATE_KEEP_WARM_DFRD_IDLE, /* Receiver State is Keep Warm (Deferred Rcvr IDLE) */
  C_GPS_RCVR_STATE_DPO_SLEEP,
  C_GPS_RCVR_STATE_TRANSITIONING,      /* Receiver State is in Transition */
  C_GPS_RCVR_STATE_HARD_RESET,         /* Receiver state is hard reset */
  /*DO NOT CHANGE THE ORDER OF THE  ENUM. 
       ADD NEW ELEMENTS BELOW */

  C_GPS_RCVR_STATE_MAX

} gps_RcvrStateEnumTypeVal;
typedef uint8 gps_RcvrStateEnumType;


/*=============================================================================
   GPS SV Time structure
   This is the fundamental component of measurement time and Steering.
   It is defined in units of msecs to facilitate easy
   independent manipulation of the epoch and sub epoch components of time.
=============================================================================*/
typedef struct
{
  FLT f_SubMs; /* Range of 0 thru 0.99999 [msecs] */
  uint32 q_Ms; /* Range of 0 thru (WEEK_MSECS-1) [msecs] */

} gps_SvTime;

/*=============================================================================
   GPS SV Time structure
   This is the fundamental component of measurement time and Steering.
   It is defined in units of msecs to facilitate easy
   independent manipulation of the epoch and sub epoch components of time.

   NOTE WELL: When revising this structure, you MUST change the IPC version 
   numbers of all affected IPC messages in nf_iface.h
   - see CL 526195 for an example of how this is done, but make sure this
   is done for all relevant messages
=============================================================================*/
typedef struct
{
  gps_SvTime z_SvTime;            /* SV Time information */
  FLT        f_SvTimeUncMs;       /* Time uncertainty (msec) */
  FLT        f_SvSpeed;           /* Speed estimate (meters/sec) */
  FLT        f_SvSpeedUnc;        /* Speed uncertainty estimate (meters/sec) */

} gps_TimeSpeedStructType;

/*=============================================================================
   GPS Measurement Report structure   

   NOTE WELL: When revising this structure, you MUST change the IPC version 
   number in nf_iface.h - see CL 526195 for an example of how this is done
=============================================================================*/
typedef struct
{
  uint8   u_Sv;                /* SV PRN */
  gps_SvStateEnumType  e_SvState; /* SV Observation state */
  uint8   u_Observe;           /* Count of all observation (both success and failure) */
  uint8   u_GoodObs;           /* Count of Good observations */
  uint8   u_FilterN;           /* Carrier to Code filtering N count */
  uint8   u_PreInt;            /* Pre-Detection (Coherent) Interval (msecs) */
  uint8   u_CSlipCount;        /* Increments when a CSlip is detected */
  uint16  w_PostInt;           /* Num Post-Detections (uints of PreInts) */
  uint16  w_MeasStatus;        /* Bit mask indicating measurement status */
  uint16  w_MeasStatus2;       /* 2nd bit mask indicating measurement status */
  uint16  w_Cno;               /* CNo. Units of 0.1 dB */
  int16   x_LatencyMs;         /* Age of the measurement in msecs (+ve meas Meas precedes ref time) */
  gps_TimeSpeedStructType z_FiltMeas;   /* Filtered measurement */
  gps_TimeSpeedStructType z_UnfiltMeas; /* Un-filtered measurement */
  boolean u_MultipathEstValid; /* TRUE is Multipath estimate is valid */
  FLT     f_MultipathEst;      /* Multipath estimate (meters) */
  boolean u_SvDirValid;        /* SV direction validity flag */
  FLT     f_Azimuth;           /* Azimuth (radians) */
  FLT     f_Elevation;         /* Elevation (radians) */
  FLT     f_DopplerAccel;      /* Acceleration in Hz/Sec */
  FLT     f_FineSpeed;         /* Carrier phase derived speed */
  FLT     f_FineSpeedUnc;      /* Carrier phase derived speed UNC */
  DBL     d_CarrierPhase;      /* Carrier phase measurement [L1 cycles] */
  uint32  q_FCount;
  uint16  w_ParityErrorCount;  /* The 10 LSB to indicate parity errors in the 10 words of current subframe
                                  1: error  0: no error
                                  bit0 for word 1, …, bit9 for word 10  */
  uint8   u_GoodParity;        /* Provides indication whether any parity errors have been observed in 
                                  the last second for each GPS SV.  1:Good Parity 0:Bad Parity */
} gps_MeasStructType;


typedef enum 
{
  GPS_MEAS_RPT_PERIODIC,
  GPS_MEAS_RPT_MEAS_DONE,
  GPS_MEAS_RPT_MAX

} gps_MeasRptReasonEnumTypeVal;
typedef uint8 gps_MeasRptReasonEnumType;



/*=============================================================================
   GPS Measurement Block structure

   NOTE WELL: When revising this structure, you MUST change the IPC version 
   number in nf_iface.h - see CL 526195 for an example of how this is done
=============================================================================*/
typedef struct
{
  gps_ClockStructType z_MeasClock; /* GPS Clock estimate for the measurement block */
  gps_CDMAClockInfoStructType z_CDMAClockInfo; /* CDMA and RTC Values at Time Transfer */
  uint8               u_NumSvs;    /* Num SV for which measurements are valid */
  uint16              w_RFLoss;    /* RF Loss with reference to Antenna. Units of 0.1dB */
  gps_MeasStructType  z_Meas[N_ACTIVE_CHAN];/* SV Measurements */

} gps_MeasBlkStructType;

/*MGP data strucutres*/

#define MAX_NUM_SA_DATA_BITS 512

/*=============================================================================
  Structure has been copied from tm_rrlp_asn1.h. Used to send 
  raw TOW data over to MGP 
  NOTE WELL: When revising this structure, please make sure to 
  sync it with the one in tm_rrlp_asn1.h 
=============================================================================*/
typedef struct gps_rrlp_GPSTOWAssistElement 
{
  uint32  satelliteID;
  uint32  tlmWord;
  uint32  antiSpoof;
  uint32  alert;
  uint32  tlmRsvdBits;
} gps_rrlp_GPSTOWAssistElement;


/*************************************************************************/
/* Reference Time information                                            

   NOTE WELL: When revising this structure, you MUST change the IPC version 
   numbers of all affected IPC messages in nf_iface.h
   - see CL 526195 for an example of how this is done, but make sure this
   is done for all relevant messages
 *************************************************************************/
typedef struct 
{
  boolean u_GpsWeekIsValid;    /* TRUE if the week number specified is valid */
  uint16  w_GpsWeek;           /* Unambiguous GPS week */
  uint32  q_GpsMsec;           /* GPS milliseconds of the week */
  FLT     f_GpsTimeUnc;        /* Time uncertainty (msec) */
  uint32  q_NumTowAssistSv;    /* Num of SVs for which tow assist data is present. */
  gps_rrlp_GPSTOWAssistElement a_GpsTowAssist[N_SV];
} gps_RefTimeStructType;

/*************************************************************************/
/* Reference Location information                                        *

   NOTE WELL: When revising this structure, you MUST change the IPC version 
   numbers of all affected IPC messages in nf_iface.h
   - see CL 526195 for an example of how this is done, but make sure this
   is done for all relevant messages
 *************************************************************************/
typedef struct
{
  uint16  w_PostionFlags;      /* Position flags indicating the features/quality of the fix */
  uint16  w_GpsWeek;           /* Unambiguous GPS week number */
  uint32  q_GpsMsec;           /* GPS milliseconds of the week */
  FLT     f_TimeBias;          /* GPS time bias ([-0.5, 0.5]msec) */
  DBL     d_Latitude;          /* WGS-84 Latitude (radians) */
  DBL     d_Longitude;         /* WGS-84 Longitude (radians) */
  FLT     f_LocUncrtntyAng;    /* Ange of the uncertainty ellipse (radians), clockwise from North */

  /* On receipt of a reference location injection in PE, the 2D uncertainty 
    ellipse is by definition 99% confidence. All necessary scaling to adhere to 
    this definition is performed in SM. */
  FLT     f_LocUncrtntySemiMajor; /* Semi-Major axis of uncertainty ellipse (meters) */ 
  FLT     f_LocUncrtntySemiMinor; /* Semi-Minor axis of uncertainty ellipse (meters) */
  boolean u_AltitudeIsValid;     /* TRUE when Altitude and its uncertainty are valid */
  FLT     f_Altitude;          /* HAE - height above ellipsoid (meters). */
  FLT     f_AltitudeUnc;       /* Altitude uncertainty (meters) */
  boolean u_HeadingIsValid;    /* TRUE when Heading information is valid */
  FLT     f_Heading;           /* Heading in radians (angle from True North). */
  boolean u_VelocityIsValid;   /* TRUE when Velocity informations is valid */
  FLT     f_VelHor;            /* Horizontal Velocity (m/s) */
  FLT     f_VelVert;           /* Vertical Velocity (m/s) */
  U64     t_TimeTickMsec;      /* TimeTickMsec of current state (0 means unknown) */ 

} gps_RefLocStructType;

/*************************************************************************/
/* Acquisition Assistance (Steering) information                         */
/*************************************************************************/
typedef enum
{
  MGP_SV_STEER_SRC_NONE = 0,
  MGP_SV_STEER_SRC_GPS,
  MGP_SV_STEER_SRC_SBAS,
  MGP_SV_STEER_SRC_MAX

} gps_SvSteerSrcEnumTypeVal;
typedef uint8 gps_SvSteerSrcEnumType;

typedef struct
{
  uint8   u_Sv;                   /* SV PRN */
  boolean u_Modulo80ms;          /* Indicate whether q_Ms is a value modulo 80ms */
  FLT     f_SubMsec;              /* GPS Code Phase (0-0.999999ms) */
  uint32  q_Ms;                   /* GPS milliseconds of the week (msec) */
  FLT     f_SvTimeUnc;            /* Code Search window (msec) */
  FLT     f_SvSpeed;              /* Satellite Speed (meters/sec) */
  boolean u_SvAccelValid;         /* if TRUE, f_SvAccel has valid value */
  FLT     f_SvAccel;              /* Satellite Acceleration (meters/sec/sec) */
  boolean u_SvSpeedUncValid;      /* if TRUE, u_SvSpeedUncValid has valid value */
  FLT     f_SvSpeedUnc;           /* Speed Uncertainty (meters/sec) */
                            
} gps_SvSteerStructType;


typedef struct
{
  boolean               u_GpsWeekValid;      /* if TRUE: GPS week number in this struture is valid */
  uint16                w_GpsWeek;           /* GPS week number */
  uint32                q_GpsMsec;           /* GPS milliseconds of the week */
  uint32                q_TimeOfApp;         /* Time of applicability in CDMA units of 1.28sec (modulo 320ms) */
  boolean               u_TimeRefIsGps;      /* TRUE if the steering data is referenced to GPS time */
  uint16                w_RefPn;             /* CDMA Reference PN sequence offset */
  boolean               u_RefPnPhaseValid;   /* TRUE if Ref PN phase is valid. */
  int32                 l_RefPnPhase;        /* Ref PN position (CDMARTC_CCx8 % 80ms) */
  uint8                 u_NumSvs;            /* Number of SVs for which steering is provided */
  boolean               u_OptimisticPos;     /* AA using optimisitic posiiton - Only valid if u_TimeRefIsGps = True*/
  gps_SvSteerSrcEnumType e_SvSteerSrc;       /* Enumeration of SV steering source */
  gps_SvSteerStructType z_SvSteerInfo[N_SV]; /* Steering information per SV */

} gps_SvAcqAssistStructType;

/*************************************************************************/
/* SV Direction information                                              */
/*************************************************************************/
typedef struct
{
  uint8 u_Sv;                    /* SV PRN */
  FLT   f_Azimuth;               /* Azimuth (radians) */
  FLT   f_Elevation;             /* Elevation (radians) */
  boolean u_ElevUncValid;        /* if TRUE, u_ElevUnc has valid value */
  FLT   f_ElevUnc;               /* Elevation Uncertainty (radians) */

} gps_AzElInfoStructType;

typedef struct 
{
  uint8                  u_NumSvs; /* Numbers of SVs for which direction information is provided */
  gps_AzElInfoStructType z_SvAzElInfo[N_SV];

} gps_SvDirStructType;

/*************************************************************************/
/* SA Data information (per SV)                                          */
/*************************************************************************/
#define MAX_NAV_BITS_IN_DWORD ((MAX_NUM_SA_DATA_BITS+31)>>5)  /* 16 */
typedef struct 
{
  uint8  u_Sv;                    /* SV PRN */
  uint8     u_Valid;                           /* validity of the current data           */
  qword     z_FirstNavBitTsInBit;              /* Time stamp in bit corresponding to the first NAV bit */
  gps_TimeStructType     z_StartTs;            /* Time stamp in ms when SA become valid */
  gps_TimeStructType     z_ExpirationTs;       /* Time stamp in ms when SA will expire */
  uint16    w_RefBitNum;                       /* Reference bit number    */
  uint16    w_NumDataBits;                     /* Number of Data bits     */
  uint32    q_NavBits[MAX_NAV_BITS_IN_DWORD];  /* Navigation message bits */

} gps_SvSADataStructType;

typedef struct 
{
  uint8                  u_NumSvs; /* Numbers of SVs for which SA data information is provided */
  gps_SvSADataStructType z_SvSADataInfo[N_SV];

} gps_SADataStructType;

/*************************************************************************/
/* Ephemeris information                                                 */
/*************************************************************************/
typedef struct 
{
  uint8   u_Sv;                /* SV PRN */
  uint8   u_CodeL2;            /* Status for the L2 frequency (2-bits) */
  uint8   u_Accuracy;          /* User Range Accuracy (4-bits) */
  uint8   u_Health;            /* SV Health (6-bits) */
  uint16  w_Iodc;              /* Issue of Data clock (10-bits) */
  boolean u_PcodeFlagIsValid;  /* TRUE if P-Code indication is valid */
  uint8   u_PcodeFlag;         /* P-Code indication flag (1-bit) */
  boolean u_Sf1BitsAreValid;   /* TRUE if SF1 Reserved bits are valid */
  uint32  q_Sf1ResrvBits[3];   /* Reserved bits (87-bits. First bit in LSB) */
  boolean u_TgdIsValid;        /* Time of Group Delay validity flag */
  uint8   u_Tgd;               /* Time of Group Delay (8-bits) */
  uint16  w_Toc;               /* Time of clock (16-bits) */
  uint8   u_Af2;               /* Clock acceleration coefficient (8-bits) */
  uint16  w_Af1;               /* Clock drift coefficient (16-bits) */
  uint32  q_Af0;               /* Clock bias coefficient (22-bits) */
  uint8   u_Iode;              /* Issue of data ephemeris tag (8-bits) */
  uint16  w_Crs;               /* Amplitude of the sine harmonic correction term to Orbit Radius (16-bits) */
  uint16  w_DeltaN;            /* Mean motion difference from computed value (16-bits) */
  uint32  q_M0;                /* Mean anomaly at reference time (32-bits) */
  uint16  w_Cuc;               /* Amplitude of the cosine harmonic correction term to argument of latitude (16-bits) */
  uint32  q_E;                 /* Orbital Eccentricity (32-bits) */
  uint16  w_Cus;               /* Amplitude of the sine harmonic correction term to argument of latitude (16-bits) */
  uint32  q_SqrtA;             /* Square-root of the Semi-Major axis (32-bits) */
  uint16  w_Toe;               /* Reference time of Ephemeris (16-bits) */
  boolean u_FitIntervalIsValid;/* Fit Interval validity flag */
  uint8   u_FitIntervalFlag;   /* Fit Interval (1-bit) */
  boolean u_AodoIsValid;       /* AODO validity flag */
  uint8   u_Aodo;              /* Age of Data offset (5-bits) */
  uint16  w_Cic;               /* Amplitude of the cosine harmonic correction term to angle of inclination (16-bits) */
  uint32  q_Omega0;            /* Longitude of the ascending node of orbit plane at weekly epoch (32-bits) */
  uint16  w_Cis;               /* Amplitude of the sine harmonic correction term to angle of inclination (16-bits) */
  uint32  q_I0;                /* Inclination angle at reference time (32-bits) */
  uint16  w_Crc;               /* Amplitude of the cosine harmonic correction term to Orbit Radius (16-bits) */
  uint32  q_Omega;             /* Argument of Perigee (32-bits) */
  uint32  q_OmegaDot;          /* Rate of Right Ascension (24-bits) */
  uint16  w_Idot;              /* Rate of inclination angle (14-bits) */
  boolean u_GpsWeekIsValid;    /* TRUE if the week number specified is valid */
  uint16  w_GpsWeek;           /* Unambiguous GPS week */

} gps_EphStructType;

/*************************************************************************/
/* Almanac information                                                   */
/*************************************************************************/
typedef struct 
{
  uint8   u_Sv;                /* SV PRN */
  boolean u_DataIdIsValid;     /* Data ID validity flag */
  uint8   u_DataId;
  uint16  w_E;                 /* Orbital Eccentricity (16-bits) */
  uint8   u_Toa;               /* Reference time of Almanac (8-bits) */
  uint16  w_DeltaI;            /* Inclination at Reference angle (16-bits) */
  uint16  w_OmegaDot;          /* Rate of Right Ascension (16-bits) */
  uint8   u_Health;            /* SV Health (8-bits) */
  uint32  q_SqrtA;             /* Square-root of the Semi-Major axis (24-bits) */
  uint32  q_Omega0;            /* Longitude of the ascending node of orbit plane at weekly epoch (24-bits) */
  uint32  q_M0;                /* Mean anomaly at reference time (24-bits) */
  uint32  q_Omega;             /* Argument of Perigee (24-bits) */
  uint16  w_Af0;               /* Clock bias coefficient (11-bits) */
  uint16  w_Af1;               /* Clock drift coefficient (11-bits) */
  boolean u_SvGhealthIsValid;  /* SV Global health validity flag */
  uint32  q_SvGhealth[12];     /* Global health information (from Page 25 of SubFrame 4 and 5) */
  uint16  w_GpsWeek;           /* Unambiguous GPS week */

} gps_AlmStructType;

/*************************************************************************/
/* Ionospheric Corrections                                               */
/*************************************************************************/
typedef struct 
{
  uint8 u_Alpha0;                 /* Coefficients for ionospheric delay algorithm. */
  uint8 u_Alpha1;
  uint8 u_Alpha2;
  uint8 u_Alpha3;
  uint8 u_Beta0;
  uint8 u_Beta1;
  uint8 u_Beta2;
  uint8 u_Beta3;

} gps_IonoStructType;

/*************************************************************************/
/* Real-Time integrity information                                       */
/*************************************************************************/
typedef struct 
{
  uint16 w_RtiGpsWeek;            /* GPS Week at which RTI info is to be applied */
  uint32 q_RtiStartMsec;          /* GPS Milliseconds (of the week)at which RTI info is to be applied */
  uint32 q_RtiDurationMsec;       /* Duration (msec) for which RTI info is to be applied */
  uint32 q_RtiSvMask;             /* 32-bit Mask specifying SV's that failed integrity ('1' => Do not use) */

} gps_RtiInfoStructType;

/*************************************************************************/
/* SV non-existence information                                              */
/*************************************************************************/
typedef struct 
{
  uint32 q_SvNoExistMask;         /* Bit Mask specifying the SVs which do not exist ('1' => No exist) */

} gps_SvNoExistStructType;

/* Unhealthy SV information                                              */
/*************************************************************************/
typedef struct 
{
  uint32 q_SvUnhealthyMask;         /* Bit Mask specifying the SVs are not healthy ('1' => Unhealthy) */

} gps_SvUnhealthyStructType;

/*************************************************************************/
/* Pre-Fix PPM information                                              */
/*************************************************************************/
typedef struct 
{
  int32 l_RfCalibGCx1024;         /* Difference between CDMA and GPS RF chain delay */
  srch_ppm_rpt_type z_PpmRpt;     /*Pre-Fix PPM report*/

} gps_PreFixPpmSructType;

/*************************************************************************/
/* UTC Information                                                       */
/*************************************************************************/
typedef struct 
{
  uint32 q_A0;      /* UTC: convert from GPS time to UTC time. */
  uint32 q_A1;      
  uint8  u_DeltaTls;    
  uint8  u_Tot;      
  uint8  u_WNt;      
  uint8  u_WNlsf;    
  uint8  u_DN;     
  uint8  u_DeltaTlsf;  

} gps_UtcInfoRptStructType;

/*************************************************************************/
/* Union of all assistance data structures                               */
/*************************************************************************/
typedef union 
{
  gps_RefTimeStructType      z_RefTimeInfo;
  gps_RefLocStructType       z_RefLocInfo;
  gps_SvAcqAssistStructType  z_SvAcqAssistInfo;
  gps_SvDirStructType        z_SvDirInfo;
  gps_SADataStructType       z_SADataInfo;
  gps_EphStructType          z_EphInfo;
  gps_AlmStructType          z_AlmInfo;
  gps_IonoStructType         z_IonoInfo;
  gps_RtiInfoStructType      z_RtiInfo;
  gps_SvNoExistStructType    z_SvNoExistInfo;
  gps_PreFixPpmSructType     z_PreFixPpmInfo;
  gps_UtcInfoRptStructType   z_UtcInfo;
  gps_SvUnhealthyStructType  z_SvUnhealthyInfo;
} gps_AssistDataUnionType;


/*************************************************************************/
/* MGP Assistance data type                                              */
/*************************************************************************/
typedef enum
{
  MGP_ASSIST_DATA_NONE = 0,
  MGP_ASSIST_DATA_REF_TIME,
  MGP_ASSIST_DATA_REF_LOC,
  MGP_ASSIST_DATA_ACQ_ASSIST,
  MGP_ASSIST_DATA_SV_DIR,
  MGP_ASSIST_DATA_SA_DATA,
  MGP_ASSIST_DATA_EPH,
  MGP_ASSIST_DATA_ALM,
  MGP_ASSIST_DATA_IONO,
  MGP_ASSIST_DATA_RTI,
  MGP_ASSIST_DATA_SV_NO_EXIST,
  MGP_ASSIST_DATA_PRE_FIX_PPM,  
  MGP_ASSIST_DATA_UTC, 
  MGP_ASSIST_DATA_ALM_XTRA,    
  MGP_ASSIST_DATA_SV_UNHEALTHY,
  MGP_ASSIST_DATA_MAX

} gps_AssistDataEnumTypeVal;
typedef uint8 gps_AssistDataEnumType;


typedef struct 
{
  gps_AssistDataEnumType  e_AssistanceType;
  boolean                 u_Force; /* When true, the injected data will be accepted un-conditionally */
  gps_AssistDataUnionType z_AssistData;

} gps_AssistDataInfoStructType;

/*************************************************************************/
/* Structure gps_NavFix describes the current position, velocity in      */
/* the Position Engine.                                                  */
/*   DO NOT CHANGE this structure w/o versioning the IPC logs which use  */
/*   this struct, which includes (but may not be limited to)             */
/*     (via sm_GpsFixRptStructType)                                      */
/*       ARIES_MSG_ID_MGP_LM_REPORT_FIX                                  */
/*       ARIES_MSG_ID_MGP_LM_REPORT_PO_ESTIMATE                          */
/*************************************************************************/


/* In Gen 8, the gps_common.h file is modified ONLY in that gps_NavSolution 
  includes additional fields. This allows the definition to be backwards 
  compatible with Gen 7. */
typedef struct
{
  uint16 w_PositionFlags;         /* Flags as defined above. */
  uint16 w_FixWn;               /* Week number of fix. */
  
  uint32 q_FixTime;             /* Milliseconds into week for fix. */
  uint32 q_UtcFixTime;          /* Time adjusted for leap seconds. */
  uint32 q_RefFCount;           /* Receiver's ms counter value. */
  uint32 q_PosCounter;          /* Position structure counter. */
  DBL    d_PosEcef[3];          /* Earth Centered, Earth Fixed position fix (m). */
  DBL    d_PosLla[3];           /* Lat, Long, Alt. */
  DBL    d_VelEcef[3];          /* ECEF velocity fix (m/s). */
  FLT    f_VelEnu[3];           /* East, North, Up velocity (m/s). */

  FLT    f_PosSigma;            /* Position standard deviation estimate (m).  */
  FLT    f_PosUnc2D[2];         /* Lat, Long uncertainty (m). */
  FLT    f_PosUncVertical;      /* Vertial uncertainty (m).   */
  FLT    f_VelUnc;              /* Velocity uncertainty (m/s). */

  FLT    f_ClockBias;           /* Receiver's clock bias (m).    */
  FLT    f_ClockBiasUncMs;      /* Receiver's clock uncertainty (m). */  
  FLT    f_ClockDriftRate;      /* Receiver's clock drift (m/s). */
  FLT    f_ClockDriftRateUncMps;/* Receiver's clock frequency bias uncertainty (m/s). */  
  FLT    f_GGTBMeters;          /* GPS to Glonass bias (m). */
  FLT    f_GGTBUncMeters;       /* GPS to Glonass bias uncertainty (m). */
  
  FLT    f_FilteredAlt;         /* Filtered altitude (m). */
  FLT    f_FilteredAltUnc;      /* Filtered altitude uncertainty, one-sided (m). */
  FLT    f_MagDeviation;        /* Magnetic deviation from true north (degrees). */

  FLT    f_PDOP;                /* Position Dilution of Precision (unitless). */
  FLT    f_HDOP;                /* Horizontal DOP (unitless) */
  FLT    f_VDOP;                /* Vertial DOP (unitless) */

  uint8  u_FirstFix;            /* Set TRUE when we have are first fix. */
  uint8  u_DriftEstimated;      /* Set TRUE when the clock drift rate is estimated from
                                 * a difference of two clock biases. */

  uint8  u_MagDevGood;          /* TRUE if magnetic devation is good. */
  uint8  u_IonoCorrected;       /* FALSE indicates that not all SV measurements were iono corrected */
  uint8  u_TropoCorrected;      /* FALSE indicates that not all SV measurements were tropo corrected */
  uint8  u_nSVs;                /* Number of SVs used in the position fix. */
  uint8  u_SVs[N_SV];           /* Satellites used in position fix. */
  uint8  u_IODE[N_SV];          /* IODEs used in fix. */
  FLT    f_SvWeights[N_SV];     /* Weights used in fix. */
  FLT    f_Residuals[N_SV];     /* Measurement residuals. */

  uint8  u_NumGloSvs;      /* Number of Glonass SVs used in the position fix. */
  uint8  u_GloSvs[N_ACTIVE_GLO_CHAN];  /* Slot IDs of the Glonass SVs used in the position fix (65 - 88). */
  uint8  u_Tb[N_ACTIVE_GLO_CHAN];      /* Tbs of the Glonass SVs used in the position fix. */
  FLT    f_GloSvWeights[N_ACTIVE_GLO_CHAN];  /* Pseudo-range weights of the Glonass SVs used in the position fix. */
  FLT    f_GloResiduals[N_ACTIVE_GLO_CHAN];  /* Pseudo-range measurement residuals of the Glonass SVs used in the position fix. */
  
  uint8 u_NumGpsVelFixSvs;                    /* Number of GPS SVs used in the velocity fix. */
  uint8 u_GpsVelFixSv[N_ACTIVE_GPS_CHAN];     /* PRNs of the GPS SVs used in the velocity fix. */
  FLT   f_GpsFixPrrResMps[N_ACTIVE_GPS_CHAN]; /* Pseudo-range rate measurement residuals of the GPS SVs used in 
                                                  the velocity fix. */
  FLT   f_GpsFixPrrUncMps[N_ACTIVE_GPS_CHAN]; /* Pseudo-range rate uncertainties of the GPS SVs used in the 
                                                  velocity fix. */
  uint8 u_NumGloVelFixSvs;                    /* Number of Glonass SVs used in the velocity fix. */
  uint8 u_GloVelFixSv[N_ACTIVE_GLO_CHAN];     /* Slot IDs of the Glonass SVs used in the velocity fix. */
  FLT   f_GloFixPrrUncMps[N_ACTIVE_GLO_CHAN]; /* Pseudo-range rate uncertainties of the Glonass SVs used in the 
                                                  velocity fix. */
  FLT   f_GloFixPrrResMps[N_ACTIVE_GLO_CHAN]; /* Pseudo-range rate measurement residuals of the Glonass SVs used 
                                                  in the velocity fix. */  
} gps_NavSolution;

/*************************************************************************/
/* Extra Nav Fix information                                             */
/*   DO NOT CHANGE this structure w/o versioning the IPC logs which use  */
/*   this struct, which includes (but may not be limited to)             */
/*     (via sm_GpsFixRptStructType)                                      */
/*       ARIES_MSG_ID_MGP_LM_REPORT_FIX                                  */
/*       ARIES_MSG_ID_MGP_LM_REPORT_PO_ESTIMATE                          */
/*************************************************************************/

typedef struct
{
  uint32 q_RefFCount;               /* Receiver's ms counter value. */
  FLT    f_ErrorEllipse[3];         /* loc_uncrtnty_ang (degree), loc_uncrtnty_a (m), loc_uncrtnty_p (m) */    
  FLT    f_FilteredAlt;             /* Filtered altitude (m). */
  FLT    f_FilteredAltUnc;          /* Filtered altitude uncertainty, one-sided (m). */
  FLT    f_UnfilteredAlt;           /* Unfiltered altitude (m). */
  FLT    f_UnfilteredAltUnc;        /* Unfiltered altitude uncertainty, one-sided (m).  */
  uint8  u_nSVsDetected;            /* total number of SVs detected by searcher */
  uint32 q_GpsFlagsExtra;           /* Extension of PositionFlags, see above, GPS_FLAGS_EXTRA_... */
  FLT    f_VelUncEnu[3];            /* Velocity uncertainty for 0x1370 log */

  /* SV used in fix data from PE */
  uint32 q_GpsSvsUsedPrnsMask;        /* Bitmask for GPS SVs used for calculating position fix */ 
  uint32 q_GpsSvsUsablePrnsMask;      /* Bitmask for GPS SVs usable for calculating position fix */
  uint32 q_GloSvsUsedSlotIdsMask;     /* Bitmask for GLONASS SVs used for calculating position fix */
  uint32 q_GloSvsUsableSlotIdsMask;   /* Bitmask forGLONASS SVs useable for calculating position fix */

  uint8  u_GpsFineSpeedAvailable[N_ACTIVE_GPS_CHAN];/* Array of PRN for FineSpeed Availability received by NF. 
                                                       Use u_nSVsDetected as a limit to this struct */
  uint8  u_GloFineSpeedAvailable[N_ACTIVE_GLO_CHAN];/* Array of Glo SlotID for FineSpeed Availability received by NF. 
                                                       Use u_nGloSVsDetected as a limit to this struct */
  uint16 w_GpsParityErrorCount[N_ACTIVE_GPS_CHAN];  /* the 10 LSB to indicate parity errors in the 10 words of current subframe
                                                       1: error  0: no error
                                                       bit0 for word 1, …, bit9 for word 10  */
  uint16 w_GloParityErrorCount[N_ACTIVE_GLO_CHAN];  /* actually a boolean type, but is still defined as uint16
                                                       1: error      0: no error */
  uint8  u_GpsGoodParity[N_ACTIVE_GPS_CHAN];        /* Provides indication whether any parity errors have been observed in 
                                                       the last second for each GPS SV. 1:Good Parity 0:Bad Parity*/
  uint8  u_GloGoodParity[N_ACTIVE_GLO_CHAN];        /* Provides indication whether any parity errors have been observed in 
                                                       the last second for each GLO SV. 1:Good Parity 0:Bad Parity*/
} gps_NavSolutionExtra;
 
/* General delete all bit mask */
#define C_DELETE_INFO_CLOCK_INFO          (0xFFFFFFFF)
#define C_DELETE_INFO_POS_INFO            (0xFFFFFFFF)
#define C_DELETE_INFO_EPH_INFO            (0x1FFFFFFFFFULL)
#define C_DELETE_INFO_ALM_INFO            (0x1FFFFFFFFFULL)
#define C_DELETE_INFO_GLO_EPH_INFO        (0xFFFFFFFF)
#define C_DELETE_INFO_GLO_ALM_INFO        (0xFFFFFFFF)
#define C_DELETE_INFO_IONO_INFO           (0xFFFFFFFF)
#define C_DELETE_INFO_IONO_INFO_GPS       (1UL << MGP_IONO_MODEL_GPS)
#define C_DELETE_INFO_IONO_INFO_QZSS      (1UL << MGP_IONO_MODEL_QZSS)

#define C_DELETE_INFO_SV_DIR_INFO         (0x1FFFFFFFFFULL)
#define C_DELETE_INFO_SV_STEER_INFO       (0x1FFFFFFFFFULL)
#define C_DELETE_INFO_SV_HEALTH_INFO      (0x1FFFFFFFFFULL)
#define C_DELETE_INFO_SA_DATA_INFO        (0xFFFFFFFF)
#define C_DELETE_INFO_RTI_INFO            (0xFFFFFFFF)
#define C_DELETE_INFO_FREQ_BIAS_EST       (0xFF)
#define C_DELETE_INFO_SV_NON_EXIST        (0x1FFFFFFFFFULL)

#define C_DELETE_INFO_GPS_SV_INFO         (0x00FFFFFFFFULL)
#define C_DELETE_INFO_QZSS_SV_INFO        (0x1F00000000ULL)

#define C_DELETE_INFO_SBAS_EPH_INFO       (0xFFFFFFFF)
#define C_DELETE_INFO_SBAS_ALM_INFO       (0xFFFFFFFF)
#define C_DELETE_INFO_SBAS_SV_DIR_INFO    (0xFFFFFFFF)
#define C_DELETE_INFO_SBAS_SV_STEER_INFO  (0xFFFFFFFF)
#define C_DELETE_INFO_SBAS_ACQ_INFO       (0xFFFFFFFF)

/* Lower 37 bits should be set for BDS */
#define C_DELETE_INFO_BDS_EPH_INFO        (0x0000001FFFFFFFFFULL)
#define C_DELETE_INFO_BDS_ALM_INFO        (0x0000001FFFFFFFFFULL)
#define C_DELETE_INFO_BDS_SV_DIR_INFO     (0x0000001FFFFFFFFFULL)
#define C_DELETE_INFO_BDS_SV_STEER_INFO   (0x0000001FFFFFFFFFULL)
#define C_DELETE_INFO_BDS_SV_HEALTH_INFO  (0x0000001FFFFFFFFFULL)

/* Lower 36 bits should be set for GAL */
#define C_DELETE_INFO_GAL_EPH_INFO        (0x0000000FFFFFFFFFULL)
#define C_DELETE_INFO_GAL_ALM_INFO        (0x0000000FFFFFFFFFULL)
#define C_DELETE_INFO_GAL_SV_DIR_INFO     (0x0000000FFFFFFFFFULL)
#define C_DELETE_INFO_GAL_SV_STEER_INFO   (0x0000000FFFFFFFFFULL)
#define C_DELETE_INFO_GAL_SV_HEALTH_INFO  (0x0000000FFFFFFFFFULL)

#define C_DELETE_INFO_UTC_INFO            (0xFF)  /* Obsolete definition since Gen 8 */
#define C_DELETE_INFO_CELLDB_INFO         (0xFFFFFFFF)

#define C_DELETE_INFO_SV_BLACKLIST_INFO   (0x1FFFFFFFFFULL) 

/* Delete position components bit mask */
#define C_DELETE_INFO_POS_ALT_INFO        (1U << 0)
#define C_DELETE_INFO_POS_LATLON_INFO     (1U << 1)

/* Clock info delete bit mask */
#define C_DELETE_INFO_CLOCK_TIME_EST        (1U << 0)  //All for GNSS
#define C_DELETE_INFO_CLOCK_FREQ_EST        (1U << 1)
#define C_DELETE_INFO_CLOCK_WEEK_NUMBER     (1U << 2)
#define C_DELETE_INFO_CLOCK_RTC_TIME        (1U << 3)
#define C_DELETE_INFO_CLOCK_TIME_TRANSFER   (1U << 4)
#define C_DELETE_INFO_CLOCK_GPSTIME_EST     (1U << 5)  
#define C_DELETE_INFO_CLOCK_GLOTIME_EST     (1U << 6)
#define C_DELETE_INFO_CLOCK_GLODAY_NUMBER   (1U << 7)
#define C_DELETE_INFO_CLOCK_GLO4YEAR_NUMBER (1U << 8)
#define C_DELETE_INFO_CLOCK_GLO_RF_GRP_DELAY (1U << 9)
#define C_DELETE_INFO_CLOCK_DISABLE_TT       (1U << 10)
#define C_DELETE_INFO_CLOCK_LEAPSEC          (1U << 11)
#define C_DELETE_INFO_CLOCK_GPSGLO_TB        (1U << 12)
#define C_DELETE_INFO_CLOCK_BDSTIME_EST      (1U << 13)
#define C_DELETE_INFO_CLOCK_GPSBDS_TB        (1U << 14)
#define C_DELETE_INFO_CLOCK_BDSGLO_TB        (1U << 15) /* Beidou to Glonass time bias-related */
#define C_DELETE_INFO_CLOCK_BDSWEEK_NUMBER   (1U << 16)
#define C_DELETE_INFO_CLOCK_BDS_RF_GRP_DELAY (1U << 17)

#define C_DELETE_INFO_CLOCK_GALTIME_EST      (1U << 18)
#define C_DELETE_INFO_CLOCK_GPSGAL_TB        (1U << 19) /* GPS to GAL time bias-related */
#define C_DELETE_INFO_CLOCK_GALGLO_TB        (1U << 20) /* GAL to GLO time bias-related */
#define C_DELETE_INFO_CLOCK_GALBDS_TB        (1U << 21) /* GAL to BDS time bias-related */
#define C_DELETE_INFO_CLOCK_GALWEEK_NUMBER   (1U << 22)
#define C_DELETE_INFO_CLOCK_GAL_RF_GRP_DELAY (1U << 23)

/* Cell DB info delete bit mask */
#define C_DELETE_INFO_CELLDB_POS            (1U << 0)
#define C_DELETE_INFO_CELLDB_LATEST_GPS_POS (1U << 1)
#define C_DELETE_INFO_CELLDB_OTA_POS        (1U << 2)
#define C_DELETE_INFO_CELLDB_EXT_REF_POS    (1U << 3)
#define C_DELETE_INFO_CELLDB_TIMETAG        (1U << 4)
#define C_DELETE_INFO_CELLDB_CELLID         (1U << 5)
#define C_DELETE_INFO_CELLDB_CACHED_CELLID  (1U << 6)
#define C_DELETE_INFO_CELLDB_LAST_SRV_CELL  (1U << 7)
#define C_DELETE_INFO_CELLDB_CUR_SRV_CELL   (1U << 8)
#define C_DELETE_INFO_CELLDB_NEIGHBOR_INFO  (1U << 9)

/* FIX_EVENT Mask*/
#define FIXEVENT_STD                   0
#define FIXEVENT_RAIM_APPLIED          (1U << 0)
#define FIXEVENT_MSEC_ERROR_DETECTED   (1U << 1)
#define FIXEVENT_CLOCK_RESET           (1U << 2)
#define FIXEVENT_GPS_GCF_ON            (1U << 3)
#define FIXEVENT_GLO_GCF_ON            (1U << 4)
#define FIXEVENT_BDS_GCF_ON            (1U << 5)
#define FIXEVENT_GAL_GCF_ON            (1U << 6)
#define FIXEVENT_QZSS_GCF_ON           (1U << 7)
/*bits 8 - 15 reserved for future use */


typedef struct
{
  uint32 q_DeleteClockMask;
  uint32 q_DeletePosMask;
  uint32 q_DeleteEphMask;
  uint32 q_DeleteAlmMask;
  uint32 q_DeleteIonoMask;
  uint32 q_DeleteSvDirMask;
  uint32 q_DeleteSvSteerMask;
  uint32 q_DeleteSvHealthMask;
  uint32 q_DeleteSaDataMask;
  uint32 q_DeleteRtiMask;
  uint32 q_DeleteCellDBMask;
  boolean u_DeleteUtc;
  boolean u_DeleteAlmCorr;
  boolean u_DeleteFreqBiasEst;
} gps_DeleteInfoStructType;

/* Structure used to process 16 bit complex quantities */
/*typedef struct
{
  int16  x_I;
  int16 x_Q;
} CplxS16;
*/
typedef struct
{
  uint32 q_ComputeTimeMs;
} FftRespMsg;

typedef enum
{
  C_GPSFFT_NO_WINDOW,
  C_GPSFFT_HANNING_WINDOW,
  C_GPSFFT_HAMMING_WINDOW
} gpsfft_WindowTypeVal;

typedef uint8 gpsfft_WindowType;

typedef struct
{
  uint8   u_Hanning;      /* TRUE applies a Hanning Window function to the input samples */
  uint8   u_InitFftTables;      /* TRUE initializes hanning table and bit reverse table */
  uint8   u_UseTwidFactTable;      /* FALSE initializes twiddle factors, else uses table */
  uint8   u_InitPowerSum;   /* TRUE initializes the p_Power array to 0 */
  uint8   u_DbOut;      /* Reports output in terms of dB – Should only be used if no further integration is required */
  uint8   u_FftShift;     /* Shifts the power sums to place DC at the array midpoint – Only use if no further integration required */
  uint8   u_Nu;       /* Exponent of FFT size. For example 15 =? 2**15  = 32768 point FFT */
  int16   *p_TwiddleFactors;    /* Pointer to the twiddle factor array of S!6. # of elements is N point */
  uint16    *p_BitReverse;      /* Pointer to the bit reverse array of U16. # of elements is N point */
  int16   *p_Hanning;     /* Pointer to the Hanning table window function. N point elements */
  CplxS16 *p_Input;     /* Pointer to the N point input complex vector. N point elements */
  FLT   *p_Power;     /* Pointer to the output power sums. (dB or linear). N point elements */
  uint32  q_ClientTaskId;
  uint32  q_MsgId;
  boolean   (*p_ClientApi)( FftRespMsg *p_FftRespMsg, uint32 q_DestThreadId, uint32 q_MsgId );  /* Pointer to results output routine */
  gpsfft_WindowType e_WindowType;
  boolean b_SpAnPowerNeeded;
} FftCmdMsg;

/*************************************************************************/
/* C-GPS API common definitions                                          */
/*************************************************************************/

typedef enum
{
  SOURCE_1X,
  SOURCE_EVDO,
  SOURCE_GSM,
  SOURCE_WCDMA,
  SOURCE_LTE,
  SOURCE_TDSCDMA  
} gps_DataSourceTypeVal;
typedef uint8 gps_DataSourceType;


/*************************************************************************/
/* C-GPS - TCXO Manager API definitions                                  */
/*************************************************************************/

typedef enum
{
  MC_PGI_TCXOMGR_RESTRICTION_SRL_GRANTED,
  MC_PGI_TCXOMGR_RESTRICTION_SRL_NOT_GRANTED
} cgps_TCXOMgrRestrictionTypeVal;
typedef uint8 cgps_TCXOMgrRestrictionType;

typedef struct
{
  cgps_TCXOMgrRestrictionType  TCXOMgrRestriction;
  timetick_type                restriction_not_granted_sclk_time;
} cgps_TCXOMgrRestrictionDataType;

typedef struct
{
  tcxomgr_client_id_type     source_id;
  tcxomgr_as_id_enum_type    as_id;
  tcxomgr_vco_type           vco_value;
  tcxomgr_rotator_type       average_rotator_value;
  uint32                     number_of_samples;
  tcxomgr_client_state_type  uncertainty_type;
  timetick_type              sclk_time_first_sample;
  timetick_type              sclk_time_last_sample;
  int32                      temperature;
} cgps_FreqEstimateDataType;

typedef struct
{
  tcxomgr_temp_set_type  temp; 
  uint32 temp_channel;
} cgps_FreqEstimateTempDataType;

/*************************************************************************/
/* C-GPS - TRM API definitions                                  */
/*************************************************************************/

typedef enum
{
  /* Unlock request cancelled.  You no-longer need to unlock. */
  MC_PGI_TRM_UNLOCK_CANCELLED,

  /* ... at the indicated time.  You can hold the lock until then. */
  MC_PGI_TRM_UNLOCK_BY,

  /* ... as soon as possible.  Release the lock when convenient, but soon. */
  MC_PGI_TRM_UNLOCK_REQUIRED,

  /* ... immediately for a page response!  Give it up ASAP! */
  MC_PGI_TRM_UNLOCK_IMMEDIATELY
}
cgps_TRMUnlockEventTypeVal;
typedef uint8 cgps_TRMUnlockEventType;

typedef struct
{
  cgps_TRMUnlockEventType  e_TRMUnlockEvent;
  uint32                   q_UnlockBySclk;
}
cgps_TRMUnlockDataType;


/*************************************************************************/
/* C-GPS - MDSP API definitions                                  */
/*************************************************************************/

#if defined(WIN32) && defined(ADS_120_COMPILER_4_BYTE_ALIGNMENT)
#pragma pack(pop, GPS_COMMON_HH)
#endif


typedef enum
{
  POS_SRC_NONE,
  POS_SRC_WLS,
  POS_SRC_KF,
  POS_SRC_EXT,
  POS_SRC_INTDB,
  POS_SRC_WIFI,
  POS_SRC_MAX
}cgps_PosSourceTypeVal;
typedef uint8 cgps_PosSourceType;  

typedef enum
{
  KF_FAILURE_CODE_NOT_APPLICABLE,
  KF_MATRIX_INVERSION,
  KF_HIGH_PDOP,                 // > 100.0
  KF_TOO_FEW_SVS,
  KF_NEED_MORE_EPH,
  KF_FDIC_ERROR,
  KF_HEPE_TOO_LARGE,
  KF_FIX_TOO_OLD,
  KF_ITAR_VEL_VIOLATION,
  KF_ITAR_POS_VIOLATION,
  KF_TIME_ERROR,
  KF_STATE_OUTLIER,
  KF_FAILURE_CODE_MAX  
}cgps_GpsNoFix_KF_Failure_Codes_TypeVal;
typedef uint8 cgps_GpsNoFix_KF_Failure_Codes_Type;

typedef enum
{
  WLS_FAILURE_CODE_NOT_APPLICABLE,
  WLS_MATRIX_INVERSION,
  WLS_HIGH_PDOP,                 // > 100.0
  WLS_TOO_FEW_SVS,
  WLS_NEED_MORE_EPH,
  WLS_FDIC_ERROR,
  WLS_CLK_ONLY_CORRECTION,
  WLS_CONV_FAILURE,
  WLS_ITAR_VEL_VIOLATION,
  WLS_ITAR_POS_VIOLATION,
  WLS_TIME_ERROR,
  WLS_FAILURE_CODE_MAX  
}cgps_GpsNoFix_WLS_Failure_Codes_TypeVal;
typedef uint8 cgps_GpsNoFix_WLS_Failure_Codes_Type;

typedef PACKED struct PACKED_POST
{
  uint8   u_SvUsed_SvId;
  uint8   u_IODE;
  uint32  q_FRR;
  uint32  q_FVR;
  uint32  q_FWeight;
} cgps_InfoPerSv_ForDmLog;

#ifdef FEATURE_CGPS_PLAYBACK
#pragma pack(push, GPS_COMMON_H_PLAYBACK, 1)
#endif /* #ifdef FEATURE_CGPS_PLAYBACK */

typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;  /* Standard DM header */
  uint8   u_Version;
  uint32  q_RefFCount;  /* FCount value at reference tick */
  uint8   u_PosSrc;
  uint16  w_PosVelFlag;
  uint8   u_FailureCode;
  uint16  w_FixEvents;
  uint16  w_WkNum;
  uint32  q_FixTimeMs;
  uint32  q_FixCnt;
  uint64  t_DLatLon[2];
  uint32  q_FAlt;
  uint32  q_FVelENU[3];
  uint32  q_VertPosSigma;
  uint32  q_FVelSigma[3];
  uint32  q_FClkBias;
  uint32  q_FClkDrift;
  uint32  q_FFiltAlt;
  uint32  q_FUnfiltAlt;
  uint32  q_FFiltAltSigma;
  uint32  q_FUnfiltAltSigma;
  uint32  q_FPDOP;
  uint32  q_FHDOP;
  uint32  q_FVDOP;
  uint8   u_EllipseConf;
  uint32  q_FEllipseAngle;
  uint32  q_FAlongAxisUnc;
  uint32  q_FPerpAxisUnc;
  uint32  q_Reserved;
  uint8   u_NumOfSvsUsed;
  uint8   u_TotalSvs;
  cgps_InfoPerSv_ForDmLog z_InfoPerSv[N_SV];
} cgps_PosDmLog;

#ifdef FEATURE_CGPS_PLAYBACK
#pragma pack(pop, GPS_COMMON_H_PLAYBACK)
#endif /* #ifdef FEATURE_CGPS_PLAYBACK */

/*
 *  gps_SlowClkExtTimeSourceType 
 *
 *  This type enumerates the possible slow clock external time sources.
 *
 *  HOST refers to a host processor.  This could be an app processor in
 *  a dual processor PDA or a laptop/PC in which a datacard is inserted or
 *  something similar which I haven't thought of.  It would most commonly
 *  refer to the laptop in which a datacard is inserted.  Basically, HOST
 *  refers to a relative time source outside of the MSM.
 *
 *  NETWORK refers to "absolute" time from a network.  Time is either
 *  GPS or UTC based.  The assumed uncertainty for this time source will
 *  be relatively large, but this time source is still treated as an
 *  "absolute" time source as opposed to a relative time source like HOST.
 *  The network could be wired or wireless.  It could be acquired from NTP,
 *  SNTP, etc. 
 *
 *  XTRA refers to time estimate coming from the XTRA client. Time is either
 *  GPS or UTC based and is absolute.
 * 
 *  XTRAT refers to time estimate coming from the XTRA-T server.  Time is
 *  GPS time based and is absolute.
 */
typedef enum
{
  MGP_SLOW_CLK_SRC_HOST,
  MGP_SLOW_CLK_SRC_NETWORK,
  MGP_SLOW_CLK_SRC_XTRA,
  MGP_SLOW_CLK_SRC_XTRAT
} gps_SlowClkExtTimeSourceTypeVal;
typedef uint8 gps_SlowClkExtTimeSourceType;

/*
 *  gps_SlowClkTimeInjectionType -
 *
 *  This data structure is used to hold injected time when passed into
 *  the gps_SlowClkExtTimePut() function.
 */
typedef struct 
{
  /*
   *  IF source is HOST, then this field is "relative" not absolute time
   *  IF source is network, then this is milliseconds since 00:00:00
   *    Jan 6, 1980
   */
  uint64                          t_Milliseconds;

  /*
   *  Time Uncertainty in milliseconds
   *
   */
  uint32                          q_Uncertainty;

  /*
   *  Time Injection Source
   */
  gps_SlowClkExtTimeSourceType    e_TimeSource;

  /*
   *  TRUE => Time Base is GPS
   *  FALSE => Time Base is UTC
   *
   *  NOTE - This field is only applicable if e_TimeSource is "NETWORK"
   */
  boolean                         b_TimeIsGps;

  /*
   *  If TRUE, then force slow clock "time" to equal the injected time.
   */
  boolean                         b_ForceFlag;

  /*
   *  If the source is Host (ie. relative time), then it is very important to
   *  know about any discontinuities.  Signal them with this counter.  
   *  Each discontinuity should cause the Cnt to be incremented.
   */
  uint8                           u_DiscontinuityCnt;
} gps_SlowClkTimeInjectionType;


/* start Coarse Position Injection related */

typedef uint32 gps_extern_coarse_pos_opt_field;
#define EXTERN_COARSE_POS_TIME_VALID     0x0001 /** Corresponding PDSM name is 
                                                    incorrect, but meaning & 
                                                    bit# is safely same */
#define EXTERN_COARSE_POS_LAT_LONG_VALID 0x0002
#define EXTERN_COARSE_POS_ALT_VALID      0x0004
#define EXTERN_COARSE_POS_ELLIPTICAL     0x0008
#define EXTERN_COARSE_POS_CIRCULAR       0x0010
#define EXTERN_COARSE_POS_HOR_CONF       0x0020
#define EXTERN_COARSE_POS_CIRCULAR_RAW   0x0040
#define EXTERN_COARSE_POS_HOR_CONF_RAW   0x0080

/* Same defines are used from PDS to SM, therefore 0x0020 is reserved for indicating the presentse of confidence field.  */

/* Azimuthal direction of semi-major where major/minor match East/North.   */
#define CIRCULAR_CPI_AZIMUTH_RAD  (C_PI/2.f)

typedef enum
{
  EXTERN_COARSE_POS_SOURCE_UNKNOWN,
  EXTERN_COARSE_POS_SOURCE_GPS,
  EXTERN_COARSE_POS_SOURCE_CELLID,
  EXTERN_COARSE_POS_SOURCE_ENH_CELLID, 
  EXTERN_COARSE_POS_SOURCE_WIFI,
  EXTERN_COARSE_POS_SOURCE_TERRESTRIAL,
  EXTERN_COARSE_POS_SOURCE_GPS_TERRESTRIAL_HYBRID,
  EXTERN_COARSE_POS_POS_SOURCE_OTHER,
  EXTERN_COARSE_POS_SOURCE_MAX = 0x10000000  /* force enum to 32 bit */
}gps_extern_coarse_pos_source_e_typeVal;
typedef uint32 gps_extern_coarse_pos_source_e_type; 

typedef enum
{
  EXTERN_COARSE_POS_TIME_GPS,
  EXTERN_COARSE_POS_TIME_UTC,
  EXTERN_COARSE_POS_TIME_AGE,
  EXTERN_COARSE_POS_TIME_MAX = 0x10000000  /* force enum to 32 bit */
}gps_extern_coarse_pos_time_stamp_e_typeVal; 
typedef uint32 gps_extern_coarse_pos_time_stamp_e_type;

typedef enum
{
  EXTERN_COARSE_POS_ALT_HAE,  /* height above ellipsoid */
  EXTERN_COARSE_POS_ALT_MSL,  /* height above mean sea level */
  EXTERN_COARSE_POS_ALT_MAX = 0x10000000  /* force enum to 32 bit */ 
}gps_extern_coarse_pos_alt_def_e_typeVal; /* definition of altitude */ 
typedef uint32 gps_extern_coarse_pos_alt_def_e_type;

typedef struct gps_ExternCoarsePosInjectionType
{
  /* indicating the presence/absence of the optional fields */
  gps_extern_coarse_pos_opt_field          opt_field_mask; 

  /* info. of the time associated with the position. */
  gps_extern_coarse_pos_time_stamp_e_type  time_info_type;
  uint32                                   timestamp_sec; /** time of week or 
                                                              age, in sec. */

  gps_extern_coarse_pos_alt_def_e_type alt_def; /* def. of altitude */

  gps_extern_coarse_pos_source_e_type   source;

  int32   latitude;  /* Lat.: rad. LSB = pi/(2^32), 2's comp. */
  int32   longitude; /* Long.: rad, LSB = pi /(2^31), 2's comp. */ 

  /* horizontal unc: circular shape, radial */
  uint32  loc_unc_hor;   /* horizontal location unc.: meters */

  int16   altitude; /* alt: meters, positive height, negative depth  */ 

  /* vertical unc */
  uint16   loc_unc_vert;   /* loc unc vertical: meters */

  U64     t_TimeTickMsec;  /* TimeTickMsec of current state (0 means unknown) */ 

}gps_ExternCoarsePosInjectionType;   

/* end Coase Position Injection related */

typedef enum
{
  CGPS_SRV_SYS_NONE   = 0x0,          /* NO Service */
  CGPS_SRV_SYS_CDMA   = 0x1,          /* CDMA mode */
  CGPS_SRV_SYS_HDR    = 0x2,           /* HDR Mode */
  CGPS_SRV_SYS_GSM    = 0x4,           /* GSM mode */
  CGPS_SRV_SYS_WCDMA  = 0x8,         /* WCDMA mode */
  CGPS_SRV_SYS_GW     = 0x10,             /* GSM and WCDMA mode */
  CGPS_SRV_SYS_LTE    = 0x20,           /* LTE mode */
  CGPS_SRV_SYS_TDSCDMA = 0x40,       /* TD-SCDMA mode */
  CGPS_SRV_SYS_OTHER  = 0x80         /* All modes not covered above */
} cgps_SrvSystemTypeVal;
typedef uint8 cgps_SrvSystemType;

/* MAR values are in GPS Chip unit */
#define MAR_6KM   22
#define MAR_13KM  46
#define MAR_20KM  70
#define MAR_27KM  94
#define MAR_33KM  113
#define MAR_41KM  142
#define MAR_48KM  166

/*----------------------------------
  These are possible Serving System
  Info data types reported by TM_CM.
 *---------------------------------*/
typedef enum
{
  CGPS_SSINFO_SRV_SYS,
  CGPS_SSINFO_SID,
  CGPS_SSINFO_BS_LAT_LONG,
  CGPS_SSINFO_TIMING_INFO
} cgps_SSInfoDataTypeVal;
typedef uint8 cgps_SSInfoDataType;

/** Daylight savings adjustment; the values for the enumerated constants
    are dictated by 3GPP TS 24.008.
**/
typedef enum
{
  CGPS_DAY_SAV_ADJ_NONE          = -1,
  CGPS_DAY_SAV_ADJ_NO_ADJUSTMENT =  0,
  CGPS_DAY_SAV_ADJ_PLUS_ONE_HOUR =  1,
  CGPS_DAY_SAV_ADJ_PLUS_TWO_HOUR =  2,
  CGPS_DAY_SAV_ADJ_MAX
} cgps_DayLightSavingTimeAdjTypeVal;
typedef int8 cgps_DayLightSavingTimeAdjType;

typedef struct
{
  uint8                                     year;       /**< Year field. */
  uint8                                     month;      /**< Month field. */
  uint8                                     day;        /**< Day field. */
  uint8                                     hour;       /**< Hour field. */
  uint8                                     minute;     /**< Minute field. */
  uint8                                     second;     /**< Second field. */
  int32                                     time_zone;  /** Time zone, as an offset from universal time
                                                            (i.e., the difference between local time and
                                                             universal time), in increments of 15 minutes.
                                                             The format for this type is specified in
                                                             3G TS 24.008 and 23.040. */
} cgps_time_and_time_zone_s_type;

typedef struct
{
  /**< Whether the universal time is available. */
  boolean  univ_time_and_time_zone_avail;
  /**< Universal Time Coordinated (UTC) time zone information. */
  cgps_time_and_time_zone_s_type             univ_time_and_time_zone;

  /**< Whether the timezone is available. */
  boolean                                   time_zone_avail;
  /**< Current time zone information. */
  int32                                      time_zone;

  /**< Whether daylight saving information is available. */
  boolean                                   daylight_saving_adj_avail;

  /**< Daylight saving adjustment. */
  cgps_DayLightSavingTimeAdjType            daylight_saving_adj;
} cgps_TimeZoneInfoType;

typedef struct
{
  cgps_SSInfoDataType         ss_data_type;
  sys_sys_mode_e_type         sys_mode;
  sys_srv_status_e_type       srv_status;
  uint16                      sid;
    /* The following types should match these types defined in services\cm\sys.h
   * sys_srch_win_type, sys_base_lat_type, sys_base_long_type, sys_base_id_type
   */
  uint8             srch_win_n; /* neighbor set search window size table index */
  /* 
     base station latitude [0.25sec and already sign extended].
     0 if not reported
  */
  int32             base_lat;
  /* 
     base station longitude[0.25sec and already sign extended].
     0 if not reported
  */
  int32             base_long;
  uint16            base_id;    /* base station ID */

  uint16                      nid;
  uint64            changed_fields;
  cgps_TimeZoneInfoType       z_TimeZoneInfo;
}cgps_multisim_SSInfoType;

typedef struct
{
  sys_modem_as_id_e_type    subs_id;    
  uint8                       number_of_stacks;
  cgps_multisim_SSInfoType    multisim_ssInfo[TM_CM_MAX_NO_STACKS];
}cgps_SSInfoType; 


/* Struct used for sending DS info from SM to ME */
typedef struct {
  uint8   standby_pref;  /* Standby preference of the Phone */

  /* The active subscription in Single Standby mode
     only valid when standby_pref is SYS_MODEM_DS_PREF_SINGLE_STANDBY */
  uint8   active_subs;
  uint8   voice_subs;    /* Designated voice subscription */
  uint8   data_subs;     /* Designated data subscription */
  uint8   priority_subs; /* Priority subscription */
}cgps_DSPrefType;

/* Maximum number of bytes required to store the IMSI of the UE */
#define MAX_IMSI_LEN_BYTES     8    

/* Struct used for sending IMSI info from SM to TLE */
typedef struct {
  uint8   sub;
  uint8   imsi[MAX_IMSI_LEN_BYTES];  /* Standby preference of the Phone */
  uint8   numMNCdigits;

}cgps_IMSIType;

/* Define the maximum number elements in the Cell DB */
#ifndef FEATURE_CGPS_XTRA_T
#define C_CELLDB_MAX_CELL_INDEX 256
#else
#define C_CELLDB_MAX_CELL_INDEX 20
#endif

/* Enumeration of the Active Radio Access Type */
typedef enum
{
  CELLDB_ACTIVERAT_NONE,
  CELLDB_ACTIVERAT_CDMA,
  CELLDB_ACTIVERAT_WCDMA,
  CELLDB_ACTIVERAT_GSM,
  CELLDB_ACTIVERAT_LTE,  
  CELLDB_ACTIVERAT_TDSCDMA, 
  CELLDB_WIFI, /* WiFi cells for multiple location ID use, not as active RAT */
  CELLDB_ACTIVERAT_MAX
} cgps_ActiveRATEnumTypeVal;
typedef uint8 cgps_ActiveRATEnumType;

/* CDMA Local Cell information */
typedef struct 
{
  uint16 w_SID; /* System ID */

} cgps_CellInfoCDMAGlobalStruct;

#if defined(FEATURE_CGPS_UMTS_CELLDB) || defined(FEATURE_FSM_V2)
/* GSM Global Cell information struct */
typedef current_bs_info_T cgps_CellInfoGSMGlobalStruct;
/* WCDMA Global Cell information struct */
typedef rrcmeas_plmn_cell_info_struct_type cgps_CellInfoWCDMAGlobalStruct;
/* TDSCDMA Global Cell information struct: use rrcmeas_plmn_cell_info_struct_type because TDS team uses it in API */
typedef rrcmeas_plmn_cell_info_struct_type cgps_CellInfoTDSGlobalStruct;

#endif /* FEATURE_CGPS_UMTS_CELLDB */

#ifdef FEATURE_CGPS_LTE_CELLDB
typedef struct
{
  lte_rrc_plmn_s selected_plmn; /*selected PLMN to be filled*/
  lte_earfcn_t freq; /*DL Center frequency of cell */
  lte_phy_cell_id_t cell_id;/* Physical cell id of cell */
  lte_rrc_global_cell_id_t cell_identity; /* Global cell id (from SIB1) of cell */
  uint16 tac; /* Tracking area code of cell */

} cgps_CellInfoLTEGlobalStruct;

/*! LTE bands structure */
typedef struct {
  /*! Band id */
  sys_sband_lte_e_type  band_id;
  /*! Minimum DL earfcn */
  lte_earfcn_t          min_dl_earfcn;
  /*! Maximum DL earfcn */
  lte_earfcn_t          max_dl_earfcn;
  /*! Minimum DL frequency 100KHz */
  uint16                min_dl_freq_100KHz;
} gnss_lte_ml1_common_band_info_s;
/*! Band info */

#endif /* FEATURE_CGPS_LTE_CELLDB */

/* TD-SCDMA Global Cell information struct */


/* Local Copy of RRC structure - rrc_plmn_identity_type */
typedef struct
{
  /* Mobile Country Code - one digit per byte range 0..9)  */
  byte mcc[3];
  /* Number of MNC digits */
  byte num_mnc_digits;
  /* Mobile Network Code - one digit per byte (range 0..9) */
  byte mnc[3];
} cgps_RrcPlmnIdentityType;

/* Local Copy of TDS structure - tdsrrcmeas_plmn_cell_info_struct_type */
typedef struct
{
  uint32 cell_id;
  uint8 cpid;
  uint16 freq;
  cgps_RrcPlmnIdentityType plmn_id;
  uint8 lac[2];
} cgps_CellInfoTDSCDMAGlobalStruct;
   
/* Global Cell information struct. Union of CDMA/GSM/WCDMA */
typedef union
{
  cgps_CellInfoCDMAGlobalStruct z_CellInfoCDMAGlobal;
  #if defined(FEATURE_CGPS_UMTS_CELLDB) || defined (FEATURE_FSM_V2)
  cgps_CellInfoGSMGlobalStruct z_CellInfoGSMGlobal;
  cgps_CellInfoWCDMAGlobalStruct z_CellInfoWCDMAGlobal;
  cgps_CellInfoTDSGlobalStruct z_CellInfoTDSGlobal;
  #endif /* FEATURE_CGPS_UMTS_CELLDB */

  #ifdef FEATURE_CGPS_LTE_CELLDB
  cgps_CellInfoLTEGlobalStruct z_CellInfoLTEGlobal;
  #endif /* FEATURE_CGPS_LTE_CELLDB */

  cgps_CellInfoTDSCDMAGlobalStruct z_CellInfoTDSCDMAGlobal;

  locEngWifiApInfoStructT_v01 z_wifi_ap_cell_info;

} cgps_CellInfoGlobalStruct;

/* Cached CellInformation Structure */
typedef struct
{
  boolean u_CachedCellIDValid;
  cgps_ActiveRATEnumType e_ActiveRAT;
  uint32 q_NumCellChangesCachedCellID;
  cgps_CellInfoGlobalStruct z_CellInfoGlobalCached;
} cgps_CellInfoCachedStruct;

/*-*-*-* Cell DB Sources *-*-*-*/
typedef enum {
  CELLDB_UPDATE_SOURCE_UNKNOWN = 0,    /* unknown source */
  CELLDB_UPDATE_SOURCE_XTRA_T,         /* from XTRA-T injection */
  CELLDB_UPDATE_SOURCE_REAL_TIME,      /* from real-time operation */
  CELLDB_UPDATE_SOURCE_MAX
} cgps_CellDBUpdateSourceEnumVal;
typedef uint8 cgps_CellDBUpdateSourceEnum;

/* Typecast the CDMA Local information to be same as the Global. 
   CDMA Cell Entries are based only on SID
*/
typedef cgps_CellInfoCDMAGlobalStruct cgps_CellInfoCDMALocalStruct;

#ifdef FEATURE_CGPS_UMTS_CELLDB

/* GSM Local Cell information */
typedef struct 
{
  uint8 bsic;    /* Base Station Identification code */
  ARFCN_T arfcn; /* Frequency Channel Number */

} cgps_CellInfoGSMLocalStruct;

/* WCDMA Local Cell information */
typedef struct 
{
  uint16 psc;  /* Primary Scrambling code */
  uint16 freq; /* Frequency */

} cgps_CellInfoWCDMALocalStruct;

#endif /* FEATURE_CGPS_UMTS_CELLDB */

#ifdef FEATURE_CGPS_LTE_CELLDB
/* LTE Local Cell information */
typedef struct 
{
  lte_earfcn_t  freq;/* DL frequency. */
  uint16  cell_id;/* Cell ID */
  
} cgps_CellInfoLTELocalStruct;

#endif /*FEATURE_CGPS_LTE_CELLDB*/

#ifdef FEATURE_CGPS_TDSCDMA_CELLDB
/* TDSCDMA Local Cell information */
typedef struct 
{
  uint16  freq;/* DL frequency. */
  uint8  cpid;/* Cell ID */
  
} cgps_CellInfoTDSCDMALocalStruct;
#endif /*FEATURE_CGPS_TDSCDMA_CELLDB*/

/* Local Cell information struct. Union of CDMA/GSM/WCDMA */
typedef union 
{
  cgps_CellInfoCDMALocalStruct z_CellInfoCDMALocal;
  #ifdef FEATURE_CGPS_UMTS_CELLDB
  cgps_CellInfoGSMLocalStruct z_CellInfoGSMLocal;
  cgps_CellInfoWCDMALocalStruct z_CellInfoWCDMALocal;
  #endif /* FEATURE_CGPS_UMTS_CELLDB */
  
  #ifdef FEATURE_CGPS_LTE_CELLDB
  cgps_CellInfoLTELocalStruct z_CellInfoLTELocal;
  #endif /*FEATURE_CGPS_LTE_CELLDB*/

  #ifdef FEATURE_CGPS_TDSCDMA_CELLDB
  cgps_CellInfoTDSCDMALocalStruct z_CellInfoTDSCDMALocal;
  #endif /*FEATURE_CGPS_TDSCDMA_CELLDB*/
} cgps_CellInfoLocalStruct;


/* Enumeration for the Source of Position Information.
If adding any further TLE pos with source PE, InputToInitEpiData() in ale_proc.c
should be updated to ignore these TLE positions from being used as EPI data in PE*/
typedef enum
{
  C_POS_INFO_SRC_INVALID,
  C_POS_INFO_SRC_PE,
  C_POS_INFO_SRC_EXTERNAL,
  C_POS_INFO_SRC_OTA,
  C_POS_INFO_SRC_PE_PROP_CELLDB,
  C_POS_INFO_SRC_EXTERNAL_PROP_CELLDB,
  C_POS_INFO_SRC_OTA_PROP_CELLDB,
  C_POS_INFO_SRC_PE_PROP_TIME,
  C_POS_INFO_SRC_EXTERNAL_PROP_TIME,
  C_POS_INFO_SRC_OTA_PROP_TIME,
  C_POS_INFO_SRC_COUNTRY_TBL_SID,
  C_POS_INFO_SRC_COUNTRY_TBL_MCC,
  C_POS_INFO_SRC_WHOLE_EARTH,
  C_POS_INFO_SRC_PE_PROP_CELLID_LEVEL2,
  C_POS_INFO_SRC_EXTERNAL_PROP_CELLID_LEVEL2,
  C_POS_INFO_SRC_OTA_PROP_CELLID_LEVEL2,
  C_POS_INFO_SRC_PE_PROP_CELLID_LEVEL3,
  C_POS_INFO_SRC_EXTERNAL_PROP_CELLID_LEVEL3,
  C_POS_INFO_SRC_OTA_PROP_CELLID_LEVEL3,
  C_POS_INFO_SRC_TIME_ZONE_TBL,
  C_POS_INFO_SRC_TIME_ZONE_TBL_PROP_CELLDB,
  C_POS_INFO_SRC_COUNTRY_TABLE_PROP_CELLDB,
  C_POS_INFO_SRC_XTA,
  C_POS_INFO_SRC_XTA_PROP_CELLDB,
  C_POS_INFO_SRC_XTA_PROP_CELLID_LEVEL2,
  C_POS_INFO_SRC_XTA_PROP_CELLID_LEVEL3,
  C_POS_INFO_SRC_XTA_ECID,
  C_POS_INFO_SRC_LAST_CONSISTENT_TLE_POS,
  C_POS_INFO_SRC_MAX

} cgps_PosInfoSrcTypeVal;
typedef uint8 cgps_PosInfoSrcType;

/* Structure for the Cell DB reference Position */
typedef struct 
{
#ifndef WIN32
  cgps_PosInfoSrcType e_PosInfoSrc; /* Source of the Position Estimate */
#else
  uint8 e_PosInfoSrc; /* Source of the Position Estimate */
#endif
  boolean u_PosInfoValid; /* Validity indicator for the Position Information */
  boolean u_GpsTimeValid; /* GPS Time validity indicator for the Position */
  uint16  w_GpsWeek;      /* GPS Week time reference for the Position */
  uint32  q_GpsMsec;      /* GPS Msec time reference for the Position */
  /* Lalitude, Longitude in Radians and Altitude in meters */
  DBL     d_PosLLa[3];    
  FLT     f_PosUnc;       /* Horizontal Uncertainty in meters (circular) */
  FLT     f_AltUnc;       /* Altitude Uncertainty in meters */

} cgps_CellDBPosInfoStruct;


typedef union {

  cgps_GsmCellTimingType z_GsmCell;
  cgps_WcdmaCellTimingType z_WcdmaCell;
  cgps_LteCellTimingType z_LteCell;
  cgps_TDScdmaCellTimingType z_TDScdmaCell;    
} gnss_TimeTagCellInfoType;

/* Definitions for Time Tagging */
typedef enum {

  C_GNSS_TIMETAG_NULL,
  C_GNSS_TIMETAG_REQUESTED,
  C_GNSS_TIMETAG_TEST_REQUEST,
  C_GNSS_TIMETAG_CELL_CHANGE,
  C_GNSS_TIMETAG_COMPLETED

} gnss_TimeTagStateTypeVal;
typedef uint8 gnss_TimeTagStateType;

typedef struct {

  /* TRUE if this data base is valid */
  boolean u_Valid; 

  /* See enum above */
  gnss_TimeTagStateType e_TimeTagState;
  
  /* Time Inject request. If TRUE, inject time on completion of timetag */
  boolean u_TimeInjectReq;
  
  /* GPS time */
  boolean u_GpsTimeValid; /* Validity flag for GPS time */
  gps_TimeStructType z_GpsTime; /* Week, Msec, Bias and Unc */
  
  /* TRUE if the 32-bit FCount is valid */
  boolean u_FCount32Valid;

  /* 32-bit extended FCount value */
  uint32 q_FCount32;

  /* GPSRTC milliseconds from the TimeTag message. */
  uint32 q_GpsRtcMs;
  /* GPSRTC SubMs from the TimeTag message */
  uint32 q_SubMsCx80;

  /* USTMR count. Resolution of 19.2 MHz */
  uint32 q_UstmrCount;
  uint32 q_LpmCount;
  /* Active radio access technology. */
  /* 
     SYS_RAT_NONE,
     SYS_RAT_GSM_RADIO_ACCESS,
     SYS_RAT_UMTS_RADIO_ACCESS,
     SYS_RAT_LTE_RADIO_ACCESS,
     SYS_RAT_TDS_RADIO_ACCESS
  */
#ifndef FEATURE_GNSS_SA
  sys_radio_access_tech_e_type ActiveRAT; 
#endif                               
  /* Cell Information */
  boolean u_CellInfoValid; /* Validity flag for Cell Info */
  gnss_TimeTagCellInfoType z_Cell;

  /* Time-taick count at which the timetag database is valid */
  uint64 t_TimeTickCount;
  
  /* Timetick count at which the last timetag was requested */
  uint64 t_LastReqestTimeTickCount;

} gnss_TimeTagStructType;


/* Structure for the Cell DB element */
typedef struct 
{
  /* Validity indicator for the Cell Entry */
  boolean                    u_CellValid;         
  /* Validity indicator for the Global Cell Info */
  boolean                    u_CellGlobalInfoValid;
  /* Validity indicator for the GPS Time reference for Cell Entry */
  boolean                    u_GpsTimeValid;      
  /* Position Conflict indicator. TRUE is position estimate
     does not agree with the Country table
  */
  boolean                    u_PosConflict;
  /* Active Radio Access technology */
  cgps_ActiveRATEnumType     e_ActiveRAT;         
  /* Default Cell Radius in meters */
  FLT                        f_CellRadius;        
  /* Cell Information Local area */
  cgps_CellInfoLocalStruct     z_CellInfoLocal;     
  /* Cell Information Global area (includes globabl unique identifiers) */
  cgps_CellInfoGlobalStruct  z_CellInfoGlobal;    
  /* Position information associated with the cell */
  cgps_CellDBPosInfoStruct    z_CellDBPos;         

  #ifdef FEATURE_CGPS_TIME_TAGGING 
  /* Time-Tag information assiciated with the cell */
  gnss_TimeTagStructType       z_CellDBTimeTag;     
  #endif /* FEATURE_CGPS_TIME_TAGGING */

  /* Time-Tick count */
  uint64                     t_TimeTickCount;        
  /* GPS Week when Cell info is updated */
  uint16                     w_GpsWeek;           
  /* GPS Msec when Cell info is updated */
  uint32                     q_GpsMsec;           
  /* Bit mask of all neighbors of the cell */
  uint32                     q_NeighborList[(C_CELLDB_MAX_CELL_INDEX+31)>>5]; 

  #ifdef FEATURE_CGPS_XTRA_T
  /* CellDB entry usage status masks */
  uint8                      u_UsageStatus;
  /* Source of this CellDB entry */
  cgps_CellDBUpdateSourceEnum  e_UpdateSource;
  #endif /* FEATURE_CGPS_XTRA_T */

} cgps_CellDBType;


/* Fast TCal definitions */

/* FTCal Error values */
#define C_GNSS_FTCAL_BAD_NUM_OF_SV 0x1
#define C_GNSS_FTCAL_ZERO_PPM_MEAS 0x2
#define C_GNSS_FTCAL_ZERO_TIME_TXFR 0x4

typedef struct
{
  float    f_PpmAverToaCCx16;
  int32    l_PpmSrchWinCenterCCx16;
  uint32   q_PpmNumToaSamplesAccumulated;
  int16    x_PpmMaxToaCCx16;
  int16    x_PpmMinToaCCx16;
  float    f_PpmAverEcIodB;
  float    f_PpmMaxEcIodB;
  float    f_PpmMinEcIodB;
} cgps_FtcalPpmStruct;

typedef struct
{
  boolean  b_TtErrorDetected;
  float    f_TtGnssRtcS80SubMsecGCx80;
  float    f_TtRefCdmaRtcS80SubPnRollCCx16;
  float    f_TtRefCdmaRtcS80AdjustSubPnRollCCx16;
  uint32   q_TtRefGnssRtcs80FCount;
  uint32   q_TtNumTTSamplesAccumulated;
  float    f_TtMaxGnssRtcS80SubMsecGCx80;
  float    f_TtMinGnssRtcS80SubMsecGCx80;

} cgps_FtcalTTStruct;

typedef struct
{
  uint8    u_GpsSv; 
  float    f_GpsPeakCodePhSubMsec;
  uint32   q_GpsPeakCodePhMsec;
  uint32   q_GpsPeakFCount;
  float    f_GpsPeakCNodBHz;
  uint32   q_GpsPeakLatency;
  float    f_GpsSpeedMperSec;
  uint32   q_GpsParams;
} cgps_FtcalGpsStruct;

typedef struct
{
  uint8    u_ErrorCode;
  float    f_TCalAverValueNsec;
  uint32   q_TCalUncertaintyNsec;
  float    f_TCalMaxValueNsec;
  float    f_TCalMinValueNsec;
} cgps_FtcalStruct;

/* Packed version of CDMA Region Identifier */
typedef PACKED struct PACKED_POST
{
  uint16 w_SID;
} cgps_SndbPackedCDMARegionType;

#ifdef FEATURE_CGPS_UMTS_CELLDB

/* Packed version of GSM Region Identifier */
typedef PACKED struct PACKED_POST
{
  uint8 u_PlmnId[3];
  uint16 w_Lac;
} cgps_SndbPackedGSMRegionType;

/* Packed version of WCDMA Region Identifier */
typedef PACKED struct PACKED_POST
{
  /* Mobile Country Code - one digit per byte range 0..9)  */
  uint8 u_Mcc[RRC_MAX_MCC_DIGITS];
  /* Number of MNC digits */
  uint8 u_Num_Mnc_Digits;
  /* Mobile Network Code - one digit per byte (range 0..9) */
  uint8 u_Mnc[RRC_MAX_MNC_DIGITS];

  uint8 b_Lac[RRC_MAX_GSMMAP_LAC_LENGTH];
} cgps_SndbPackedWCDMARegionType;

/* Packed version of UMTS Region Identifier */
typedef PACKED union PACKED_POST
{
  cgps_SndbPackedGSMRegionType z_GsmRegionFormat;
  cgps_SndbPackedWCDMARegionType z_WcdmaRegionFormat;
} cgps_SndbPackedUMTSRegionType;

#endif /* FEATURE_CGPS_UMTS_CELLDB */

/* Packed version of Sparse Network DB Region Identifier */
typedef PACKED struct PACKED_POST
{
  uint8 u_ActiveRAT;                            /* The Cellular technology */

  PACKED union PACKED_POST
  {
    cgps_SndbPackedCDMARegionType z_CdmaID;
#ifdef FEATURE_CGPS_UMTS_CELLDB
    cgps_SndbPackedUMTSRegionType z_UmtsID;
#endif /* FEATURE_CGPS_UMTS_CELLDB */
  }z_Rat;
} cgps_SndbPackedRegionInfoType;

/* If this is updated, mgp_CellDBPosInfoStruct_ForDmLog also needs to be updated */
typedef PACKED struct PACKED_POST 
{
  uint8 e_PosInfoSrc; /* Source of the Position Estimate */
  boolean u_PosInfoValid; /* Validity indicator for the Position Information */
  boolean u_GpsTimeValid; /* GPS Time validity indicator for the Position */
  uint16  w_GpsWeek;      /* GPS Week time reference for the Position */
  uint32  q_GpsMsec;      /* GPS Msec time reference for the Position */
  /* Lalitude, Longitude in Radians and Altitude in meters */
  int64   r_PosLLa[3];    
  int32   l_PosUnc;       /* Horizontal Uncertainty in meters (circular) */
  int32   l_AltUnc;       /* Altitude Uncertainty in meters */
} cgps_CellDBPackedPosInfoStruct;

/* Packed version of Time stamp */
typedef PACKED struct PACKED_POST
{
  uint64 t_TimeTick;
  PACKED struct PACKED_POST
  {
    uint16 w_Week;
    uint32 q_MSec;
  } z_Gps;
} cgps_SndbPackedTimeStampType;

/* Packed version of SNDB Record */
typedef PACKED struct PACKED_POST
{
  cgps_SndbPackedRegionInfoType z_Region;
  cgps_CellDBPackedPosInfoStruct z_Pos;
  cgps_SndbPackedTimeStampType z_TimeStamp;
  uint32 q_RegionSize;
} cgps_SndbInjectedRecordType;

typedef struct
{
  uint16 w_GpsWeek;     /* GPS week number at reference tick [weeks] */
  uint32 q_GpsMsec;     /* GPS msec at reference tick [msecs] */
} gps_GpsTimeStructType;

/* Enum for GPS time check source
*/
typedef uint32 gps_TimeChkSourceType;

#define  GPS_TIME_CHECK_SOURCE_NONE          0x00000000
#define  GPS_TIME_CHECK_SOURCE_PERIODIC      0x00000001
#define  GPS_TIME_CHECK_SOURCE_PER_FIX       0x00000002
#define  GPS_TIME_CHECK_SOURCE_DATA_INJECT   0x00000004
#define  GPS_TIME_CHECK_SOURCE_TIME_INJECT   0x00000008

#define  GPS_TIME_CHECK_SOURCE_ODP_SESSION   0x00000010

/* Bit mask for clock synchronization status (used for Femtocell)*/
typedef uint32 gps_ClockSyncStatusFlags;

#define  GPS_CLOCK_STATUS_NONE          0x00000000

/* GPS clients perf state. Used to request NPA resources */
typedef enum
{
  C_GNSS_NPA_CPU_INACTIVE,             /* To be used by all clients */
  C_GNSS_NPA_CPU_ACQ,                  /* To be used by MC only */
  C_GNSS_NPA_CPU_TRK,                  /* To be used by MC only */
  C_GNSS_NPA_CPU_DPO,                  /* To be used by MC/NF only */
  C_GNSS_NPA_CPU_HIGH_NUM_TASKS_USED,  /* Not used on Dime */
  C_GNSS_NPA_CPU_LOW_NUM_TASKS_USED,   /* Not used on Dime */
  C_GNSS_NPA_CPU_NAV_CLKS_ON,          /* To be used by PGI/NavHW only */
  C_GNSS_NPA_CPU_NAV_CLKS_OFF          /* To be used by PGI/NavHW only */
} gnss_NpaCpuFreqEnumVal;

typedef uint8 gnss_NpaCpuFreqEnum;

typedef enum
{
  C_GNSS_NPA_BUS_INACTIVE,
  C_GNSS_NPA_BUS_ACQ,
  C_GNSS_NPA_BUS_TRK
} gnss_NpaBusFreqEnumVal;
typedef uint8 gnss_NpaBusFreqEnum;

typedef enum
{
  C_GNSS_NPA_CPU_LATENCY_INACTIVE,
  C_GNSS_NPA_CPU_LATENCY_ACTIVE
} gnss_NpaCpuLatencyEnumVal;
typedef uint8 gnss_NpaCpuLatencyEnum;
/*============================================================================
    Kalman Filter mode indicator (static/dynamic)
==============================================================================*/
typedef enum 
{
  GPS_KF_FILTER_MODE_UNINITIALIZED,
  GPS_KF_FILTER_MODE_DYNAMIC,
  GPS_KF_FILTER_MODE_STATIC_PRE_LOCKDOWN,
  GPS_KF_FILTER_MODE_STATIC_POST_LOCKDOWN,
  GPS_KF_FILTER_MODE_SENSOR_DYNAMIC,
  GPS_KF_FILTER_MODE_SENSOR_STATIC,
  GPS_KF_FILTER_MODE_MAX
} gps_KfFilterModeStatusEnumTypeVal;
typedef uint8 gps_KfFilterModeStatusEnumType;

typedef struct 
{
  uint8   u_NumSvPos; /* Numbers of SVs with Eph or XTRA info. */
  uint32  q_SvPosList; /* Bit-mask of SVs with Eph or XTRA info */
} gps_SvPosListStructType;

/*=============================================================================

       Functions

=============================================================================*/

/*
 ******************************************************************************
 * gps_GpsMsecWeekLimit
 *
 * Function description:
 *
 *  gps_GpsMsecWeekLimit is a helper function used to perform the 
 *  fairly common check to see if a msec is indeed with the allowable
 *  range of 0 thru WEEK_MSECS-1. Only values within a single week
 *  are entertained. (ie- Multiple week adjustments are not performed
 *
 *  Assigning a NULL pointer to p_GpsWeek will disable the week 
 *  adjustment logic ... ie - The p_GpsMsec will only be affected.
 *
 * Parameters: 
 *
 *  p_GpsMsecs - Pointer to the msec value under test
 *  p_GpsWeek - Pointer to week number which may be adjusted
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/

void gps_GpsMsecWeekLimit( int32 *p_GpsMsecs, uint16 *p_GpsWeek );

#if 0
/* no one uses this. Remove when confirmed */
/*=============================================================================

FUNCTION 
  gps_SvTimeDelta

DESCRIPTION
  gps_SvTimeDelta is a helper function used to apply an arbritrary time delta
  to a gps_SvTime structure. To accomodate large time deltas without loss of
  precision DBL arithmetic in used. The ability to operate only on the
  sub millisecond portion of the time is afforded by the u_EvalMs flag.

DEPENDENCIES
  None.

PARAMETERS
  p_SvTime - pointer to the gps_SvTime structure that is to be adjusted
  d_TimeDelta - Value of the time delta [msecs]
  u_EvalMs - TRUE, forces the msec time delta to be evaluated

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/

void gps_SvTimeDelta( gps_SvTime *p_SvTime, DBL d_TimeDelta, uint8 u_EvalMs );
#endif


/*
 ******************************************************************************
 * Function cgps_GpsTCXOManagerCB
 *
 * Description:
 *
 *  This is a callback function called by TCXO Manager to inform PGI module
 *  that the current TCXO Manager restriction has changed.
 *
 * Parameters: 
 *
 *  New TCXO Manager restriction.
 *
 * Dependencies:
 * 
 *  None
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/

extern void cgps_GpsTCXOManagerCB( tcxomgr_restriction_type );


/*
 ******************************************************************************
 * Function cgps_GpsTCXOManagerRPushCB
 *
 * Description:
 *
 *  This is a callback function called by TCXO Manager to send Frequency
 *  Estimate Update data (using Rotator Push interface) to PGI module.
 *
 * Parameters: 
 *
 *  Frequency Estimate Update data.
 *
 * Dependencies:
 * 
 *  None
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
extern void cgps_GpsTCXOManagerRPushCB
(
  tcxomgr_rpush_cb_data_type              cb_data
);

/*
 ******************************************************************************
 * Function cgps_GpsTCXOManagerTempCB
 *
 * Description:
 *
 *
 * Parameters: 
 *
 *  Frequency Estimate Update data.
 *
 * Dependencies:
 * 
 *  None
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
extern void cgps_GpsTCXOManagerTempCB
(
  tcxomgr_temp_set_type  temp, 
  uint32 temp_channel
);

/*
 ******************************************************************************
 * Function cgps_PpmTCXOManagerCB
 *
 * Description:
 *
 *  This is a callback function called by TCXO Manager to inform PPM module
 *  that the current TCXO Manager restriction has changed.
 *
 * Parameters: 
 *
 *  New TCXO Manager restriction.
 *
 * Dependencies:
 * 
 *  None
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/

extern void cgps_PpmTCXOManagerCB( tcxomgr_restriction_type );

/*
 ******************************************************************************
 * Function cgps_TRMLockGrantedCB
 *
 * Description:
 *
 *  This function is called by TRM module when TRM lock is granted to GPS
 *
 * Parameters: 
 *
 *  None
 *
 * Dependencies:
 *
 *  Caller to this function shall be under TASKLOCK context.
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/

extern void cgps_TRMLockGrantedCB
(
  /* The client which is being informed of an event */
  trm_client_enum_t,

  /* The event being sent to the client */
  trm_grant_event_enum_t,

  /* Anonymous payload echoed from trm_request_and_notify() */
  trm_request_tag_t
);

#ifndef FEATURE_CGPS_PLAYBACK
/*
 ******************************************************************************
 * Function cgps_TRMAsyncEventCB
 *
 * Description:
 *  This function is called by TRM module when TRM lock is granted to GPS,
 *   or when any other common TRM event occurs.
 *
 * Parameters: 
 *  p_EventData - Pointer to TRM event data

 * Dependencies:
 *   This callback function must be attached during init.
 *
 * Return value: 
 *  None
 *
 ******************************************************************************
*/
void cgps_TRMAsyncEventCB(trm_async_callback_information_type* p_EventData);
#endif /* ! FEATURE_CGPS_PLAYBACK */

#ifdef FEATURE_GPSONE_PPM
/*===========================================================================

FUNCTION GPSSRCH_PDSM_CMD

DESCRIPTION    This function places a specified command onto the Search
      Task's PDSM command queue for processing by the Search Task.

DEPENDENCIES   This function assumes that the 'srch_pdsm_cmd_q' has been
      properly initialized.

RETURN VALUE   None.

SIDE EFFECTS   The Search Task dequeues and processes the specified command.
===========================================================================*/

extern void  gpssrch_pdsm_cmd
(
  gpssrch_pdsm_cmd_type  *cmd_ptr
    /* Pointer to searcher command. Upon queueing, the 'status'
       field of the command block is set to 'SRCH_BUSY_S'. */
);


/*===========================================================================

FUNCTION PDSM_SRCH_MSG

DESCRIPTION
  This function is used to send messages( req/rsp ) from Srch to PDSM

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

extern void pdsm_srch_msg( pdsm_srch_msg_u_type  *msg_ptr );

#endif /* FEATURE_GPSONE_PPM */

/*=============================================================================

FUNCTION
  sm_StoreFtcalPpmData

DESCRIPTION
  
DEPENDENCIES
  None.

PARAMETERS
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
extern void sm_StoreFtcalPpmData (cgps_FtcalPpmStruct* p_PpmData);

/*
 ******************************************************************************
 * Function cgps_logGpsFix
 *
 * Description:
 *
 *  This function is called by SM/PE to log GPS position(0x1370)
 *
 * Parameters: 
 *
 * p_NavSolution - Pointer to GPS fix data
 * p_NavSolutionExtra - Pointer to extra GPS fix data
 * p_Meas - Pointer to GPS measurement block information, if available
 * u_finalFix - Fix is final or intermediate. PE should always set this to FALSE.
 * u_backgroundFix - Fix is a background fix or a non-background fix. PE should always set to FALSE
 * u_peGeneratedFix - Fix is generated from PE or LM, PE should set it to TRUE, LM should set to FALSE
 * u_meGeneratedFix - is the fix report generated by ME 
 *
 * Dependencies:
 *
 *  None
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/

extern void cgps_logGpsFix
(
  /* Struct containing fix details */
  const gps_NavSolution       *p_NavSolution,
  /* The event being sent to the client */
  const gps_NavSolutionExtra  *p_NavSolutionExtra,
  /* GPS measurement info, if available */
  const gps_MeasBlkStructType *p_Meas,
  /*Is this a final or intermediate fix*/
  boolean u_finalFix,
  /* is this a background fix*/
  boolean u_backgroundFix,
  /* is the fix report generated by PR or LM */
  boolean u_peGeneratedFix,
  /* is the fix report generated by ME */
  boolean u_meGeneratedFix
);

/*
 ******************************************************************************
 * Function cgps_logGpsNoFix
 *
 * Description:
 *
 *  This function is called by PE to log GPS position(0x1370) when there is no fix
 *
 * Parameters: 
 *
 * p_NavSolution - Pointer to GPS fix data
 * p_Meas - Pointer to GPS measurement block information, if available
 * u_backgroundFix - true if this is a background fix
 * Dependencies:
 *
 *  None
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/

extern void cgps_logGpsNoFix
(
  /* Struct containing fix details */
  const gps_NavSolution       *p_NavSolution,
  /* GPS measurement info, if available */
  const gps_MeasBlkStructType *p_Meas,
  /* is this a background fix*/
  boolean u_backgroundFix
);

/******************************************************************************
 * Function description:
 *  cgps_GpsTimeDiffMsec() 
 *  
 * Parameters:      pz_GpsTimePlus  - Gps time from which time below is 
 *                                    subtracted
 *                  pz_GpsTimeMinus - The time to subtract from the above time
 *                  
 * Return value:  Time difference in millisecond (DBL to handle 
 *                                                large potential range)
 *
 ******************************************************************************
*/
extern DBL cgps_GpsTimeDiffMsec(const gps_TimeStructType * pz_GpsTimePlus,
                                const gps_TimeStructType * pz_GpsTimeMinus);

/******************************************************************************
 * Function description:
 *  cgps_GpsTimeDiffMsec() determine how many msec's p_ClockAhead is ahead of 
 *  p_ClockBehind, and place the result in pf_MsecAhead.  

 *  
 * Parameters:      p_ClockAhead  - Gps clock from which time below is 
 *                                    subtracted
 *                  p_ClockAhead - The clock to subtract from the above clock
 *                  pf_MsecAhead - The ms diffeence between the two clocks
 *                  
 * Return value:  Return true if a usable value is successfully
                  calculated, and zero otherwise. 
 *
 ******************************************************************************
*/
U8 cgps_GpsClockDiffMsec( const gps_ClockStructType *p_ClockAhead, 
                                 const gps_ClockStructType *p_ClockBehind, 
                                 FLT * pf_MsecAhead );

/*============================================================================
FUNCTION gps_meas_status_bits_check

DESCRIPTION
    Function checking SV measurement status bits. 

RETURN VALUE
    TRUE/FALSE: if TRUE, status is good enough to be included in a measurement
    report.

DEPENDENCIES

LIMITATIONS

=============================================================================*/

extern boolean gps_meas_status_bits_check ( const uint16 w_sv_meas_status, 
                                                 boolean status_check_needed );

/*===========================================================================

FUNCTION gnss_display_cb_logs

DESCRIPTION
  This function is to display the cb logs

PARAMETERS:
    
  call_back_log: Contains details on which callbacks have been called or not.
  clientThreadId: Id corresponding to each threads
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void gnss_display_cb_logs(uint8 call_back_log[GPS_MAX_CALLBACK_CLIENTS], uint16 clientThreadId);




/*
 * ****************************************************************************
 * cgps_FullGpsMs
 *
 * Function description:
 *
 *  Simple helper function to convert weeks/msecs into the number
 *  of msecs since GPS clock was started. 
 *
 * Parameters:
 *
 *  w_WeekNum - Number of weeks since GPS clock started
 *
 *  q_Ms - Number of msecs into the current week
 *
 * Return value:
 *
 *  Time in msecs since the GPS clock was started
 *
 ******************************************************************************
*/
extern DBL cgps_FullGpsMs( const uint16 w_WeekNum, const uint32 q_Ms);

/*
 ******************************************************************************
 * cgps_100log10
 *
 * Function description:
 *
 *  cgps_100log10 implements a 100*log10(x) approximation in integer math. This
 *  function finds favour with routines that calculate SNR. The standard
 *  deviation of error between this approximation and truth is 0.32 which is
 *  equivalent to 3/100ths of a dB. This is considered acceptable.
 *
 * Parameters: 
 *
 *  U32 - q_X. 
 *
 * Return value: 
 *
 *  q_Y = 100 * log10( q_X )
 *
 *  void
 *
 ******************************************************************************
*/

uint32 cgps_100log10( uint32 q_X );

void gps_PropTimeSpeedCarPhase( gps_MeasStructType *p_GpsMeas, 
                        FLT f_PropMs, FLT f_AbsPropMs );

/*
 ******************************************************************************
 * Function cgps_TimeTickMsecGet
 *
 * Description:
 *
 *  This function will return a 64 bit value representing milliseconds since 
 *  the device was powered on, created by extending the time-services
 *  32 bit millisecond counter
 *
 * Parameters: 
 *
 *  None
 *
 * Dependencies:
 *
 *  None
 *
 * Return value: 
 *
 *  A 64 bit timetick value
 *
 ******************************************************************************
*/

uint64 cgps_TimeTickGetMsec( void );

/*
 ******************************************************************************
 * Function mc_NpaRequestCpuFreq
 *
 * Description:
 *
 *  This function requests necessary level of CPU MIPS via
 *  NPA (Node Power Architecture) interface for proper GPS operation.
 *  If GPS is not active, this function cancels any CPU frequency
 *  requests.
 *  
 * Parameters:
 *
 *  Type of request.
 *
 * Return value:
 *
 *  TRUE if the the request is successful.
 *
 ******************************************************************************
*/

extern boolean mc_NpaRequestCpuFreq( gnss_NpaCpuFreqEnum );

/*
 ******************************************************************************
 * Function mc_NpaRequestBusFreq
 *
 * Description:
 *
 *  This function requests necessary level of AHB frequency via
 *  NPA (Node Power Architecture) interface for proper GPS operation.
 *  If GPS is not active, this function cancels any Bus frequency
 *  requests.
 *  
 * Parameters:
 *
 *  Type of request.
 *
 * Return value:
 *
 *  TRUE if the the request is successful.
 *
 ******************************************************************************
*/
extern boolean mc_NpaRequestBusFreq( gnss_NpaBusFreqEnum );

/*
 ******************************************************************************
 * Function mc_NpaRequestCpuLatency
 *
 * Description:
 *
 *  This function requests necessary level of CPU Latency via
 *  NPA (Node Power Architecture) interface for proper GPS operation.
 *  
 * Parameters:
 *
 *  Type of request.
 *
 * Return value:
 *
 *  None.
 *
 ******************************************************************************
*/

void mc_NpaRequestCpuLatency( gnss_NpaCpuLatencyEnum );

/******************************************************************************
 * cgps_MagDeviation() - Compute deviation (correction) for a magnetic compass.
 *
 * Function description:
 *  We are interpolating the magnetic deviation from true north via the above
 *  table.  The table was constructed using Geomagix using the WMM-2000 model
 *  at sea level.
 *
 * Parameters:
 *
 *  d_Latitude  - Input latitude of current position (radians).
 *  d_Longitude - Input longitude of current position (radians).
 *  f_magdev  - Output magnetic deviation for current position.
 *
 * Return value: TRUE if deviation was computed, else FALSE.
 *
 ******************************************************************************
*/
uint32 cgps_MagDeviation( DBL d_Latitude, DBL d_Longitude, FLT *f_magdev );

/*
 ******************************************************************************
 *  This function sets the freq of CPU to 500 MHz using CPU clk node. This node
 * should not be used if system is in DPO since its controlled in 
 * mc_NpaRequestCpuFreq().
 *  
 * Parameters: 
 *
 *  None.
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
 void mc_NpaRequestMaxQ6CpuSpeed( void );

/*
 ******************************************************************************
 * This handle should not be used if system is in DPO since its controlled in 
 * mc_NpaRequestCpuFreq().
 *  
 * Parameters: 
 *
 *  None.
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void mc_NpaCancelMaxQ6CpuSpeed( void );

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*lint +fva */
extern int snprintf(char *, size_t, const char *, ...);
/*lint -fva */

/*
 ******************************************************************************
 *
 * cgps_utils_meter_to_GADK
 *
 * Function description:
 * This is obtained from the GAD spec V3.2.0
 *
 * The uncertainty r, expressed in meters, is mapped to a number K, 
 * with the following formula:
 *     r = C [ (1 + x)^K - 1 ]
 *     with:
 *          1- Horizontal: C = 10 and x = 0.1
 *          2- Vertical:   C = 45 and x = 0.025
 *
 * The K value must be in the range 0 <= K <= 127.  If the uncertainty
 * does not exactly match a K mapping value, then we bump up to the
 * next higher K value.  This ensures that the reported UNC is not 
 * smaller than the actual UNC.  We keep K >= 1, as K=0 means 0 meters
 * of uncertainty, which is not reasonable for computed positions from 
 * measurements.
 *
 * This code/algorithm has been verified separately with a separate program.
 * The program took all values of K from 0 to 127.  The corresponding range
 * value from the mapping table was used as an input to test the code. 
 * For each value of K, the range value, the range value plus a small number, 
 * and the range value minus a small number were all input into the function
 * and the output K value was compared to what was desired.
 * 
 * 
 * Parameters:
 *
 *   f_Unc - Uncertainty value 
 *   u_Is_Horiz - If True, compute horizontal uncertainty. Else, compute vertical.
 *
 * Return value: 
 *
 *   K value, which has a range 0 <= K <= 127;
 *
 ******************************************************************************
*/
uint8 cgps_utils_meter_to_GADK( boolean u_Is_Horiz, uint32 q_hor_acc_meters);

/*
 ******************************************************************************
 *
 * cgps_utils_GADK_to_meter
 *
 * Function description:
 * This is obtained from the GAD spec V3.2.0
 *
 * The uncertainty r, expressed in meters, is mapped to a number K, 
 * with the following formula:
 *
 *     r = C [ (1 + x)^K - 1 ]
 *     with:
 *          1- Horizontal: C = 10 and x = 0.1
 *          2- Vertical:   C = 45 and x = 0.025
 *
 * The K value must be in the range 0 <= K <= 127.  
 *
 * This function simply returns the r value given a K value.
 * 
 * 
 * Parameters:
 *
 *   u_UncK - Coded unc value
 *   u_Horiz - If True, Return Horizontal Unc. Otherwise return Vertical.
 *
 * Return value: 
 *
 *   Unc associated with the input uncertainty K value
 *
 ******************************************************************************
*/
float cgps_utils_GADK_to_meter( uint8 u_Horiz, uint8 u_UncK );

/*
 ******************************************************************************
 *
 * cgps_utils_microsec_to_GADK
 *
 * Function description:
 * This is obtained from the GAD spec V3.2.0
 *
 * The uncertainty r, expressed in microseconds, is mapped to a number K, 
 * with the following formula:
 *
 *    r = C [ (1 + x)^K - 1 ]    with C = 0.0022 and x = 0.18  
 *
 * The K value must be in the range 0 <= K <= 127.  If the uncertainty
 * does not exactly match a K mapping value, then we bump up to the
 * next higher K value.  This ensures that the reported UNC is not 
 * smaller than the actual UNC.  We keep K >= 1, as K=0 means 0 microseconds
 * of uncertainty, which is not reasonable for computed measurements.
 *
 * This code/algorithm has been verified separately with a separate program.
 * The program took all values of K from 0 to 127.  The corresponding range
 * value from the mapping table was used as an input to test the code. 
 * For each value of K, the range value, the range value plus a small number, 
 * and the range value minus a small number were all input into the function
 * and the output K value was compared to what was desired.
 * 
 * 
 * Parameters:
 *
 *   q_time_unc_microseconds - Time uncertainity in microseconds 
 *
 * Return value: 
 *
 *   K value, which has a range 0 <= K <= 127;
 *
 ******************************************************************************
*/

uint8 cgps_utils_microsec_to_GADK( float f_time_unc_microseconds );

/*
 ******************************************************************************
 * gps_GetGpsRomAlmanac
 *
 * Function description:
 *
 *  gps_GetGpsRomAlmanac is a helper function used to retreive the default GPS
 *  ROM almanac.
 *
 * Parameters: 
 *
 *  pz_GpsRomAlm - Pointer to the GPS ROM almanac structure
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/

void gps_GetGpsRomAlmanac( gps_AlmStructType **pz_RomAlm );

/*
 ******************************************************************************
 * Function gps_Memcpy
 *
 * Description:
 *
 *  A helper utility to memcpy from one array to the other, bounded by 
 *  max length
 *
 * Parameters: 
 * [in]   q_DstLenMax       The max length that can be written
 * [out]  p_Dst             The destination buffer for the copy
 * [in]   q_SrcLen          The length to copy
 * [in]   p_Src             The source buffer to copy from
 
 *
 * Dependencies:
 *
 *  None
 *
 * Return value: 
 *
 *  The number of bytes copied to the destination
 *
 ******************************************************************************
*/
uint32 gps_Memcpy
( 
  uint32          q_DstLenMax,
  void*           p_Dst,
  uint32          q_SrcLen,
  const void*     p_Src
);

/******************************************************************************
* Function description:
*  wgs84_Ecef2LlaGet() converts an ECEF position into a WGS-84 Lat, Lon & Alt.
* 
 * Parameters: 
 *  d_Ecef    - XYZ position (m) in the ECEF coordinate system. 
 *  d_Lla   - Latitude (rad), longitude (rad) and altitude (m).
*
* Return value:  None 
 *
******************************************************************************
*/
void wgs84_Ecef2LlaGet( const DBL d_Ecef[], DBL d_Lla[] );

/******************************************************************************
* Function description:
*  wgs84_Lla2EcefGet() converts an LLA position into a ECEF.
* 
 * Parameters: 
 *  d_Lla   - Latitude (rad), longitude (rad) and altitude (m).
* d_Ecef    - XYZ position (m) in the ECEF coordinate system. 
 *
* Return value: None
*
******************************************************************************
*/
void wgs84_Lla2EcefGet( const DBL d_Lla[] , DBL d_Ecef[] );

#endif /* GPS_COMMON_H */
