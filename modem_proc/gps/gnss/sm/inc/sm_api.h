#ifndef SM_API_H
#define SM_API_H
/*------------------------------------------------------------------------------
  Copyright (c) 2009 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  Confidential and Proprietary - Qualcomm Technologies, Inc.   All Rights Reserved.
                  
  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
File comment
------------------------------------------------------------------------------*/

/**
  @file sm_api.h 
    
  SM interface exposed to other modules

*/

/*------------------------------------------------------------------------------
Mainpage
------------------------------------------------------------------------------*/
/**
  @mainpage SM Public API
  
*/

/**
     @defgroup SM Session Manager
     
     TODO: Add info about what session manager does here
*/

/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/inc/sm_api.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/28/15   mj     Add support for INS only fixes for SAP changes
07/09/15   rh      Keep sm_UpdateCellIDInfo() API for SMART even with CM CID change  
07/03/15   jv      QZSS API Changes
03/18/15   ss       Added function to report Wifi Pos fix response
09/15/14   jv      Galileo GNSS Support
08/08/14   jv      16-bit SVID support
07/24/14   jyw     Reserving IPC for CPE measurements 
03/10/14   ah      Added support for Get Avail WWAN Pos
12/02/13   ah      Support GPS State Info requests for multiple clients simultaneously
10/28/13   hm      Added SM error event report for signal processing memory allocation failure
06/13/13   rh      Added sm_GnssNavSvInfo for SV info & sm_BdsAssistDataStruct
06/07/13   rh      Removed NO_FIX report and changed FIX report structure 
03/20/13  zhangwen Updated the IPCs in correspondence to payload changes in nofix/fix reports
01/28/13   mj      Updated sm_RegisterApiNotifier to include new param clientId
02/06/13   ah      Added support for BDS (BeiDou)
04/18/12   ssu     Adding sm_PositionReportStructType for GPS state information.
03/14/12   ss      Support of reset location serive.
02/27/12   rh      Added sm_SendEngineErrorRecoveryReport() API 
03/28/11   rh      Added heading uncertainty
04/28/10   kgc     Added bad health list to gnss assistance data. redfined ipc
02/12/10   vp      Added Sv polynomial function for OEM DRE enablement
06/10/09   gk      ODP 2.0 support
06/10/09   gk      Added Get UTC offset from NV
06/10/09   gk      ODP 1.1 support
01/12/09   atien   XTRA-T support
09/15/08   gk      Rename the on-demand struct to sm_ from cgps_04/27/08   gk      On_Demand Changes
04/17/08   jb      Allow changing IPC log with modified gps_NavSolutionExtra structure
08/01/07   rw      Add support for XTRA feature
03/27/07   mr      Removed positionflag #define s from this file
02/08/07   br      Added field to PPM report structure.
12/19/06   br      Clean-up.
12/01/06   jw      Modified sm_GpsMeasRptStructType to include gps_MeasBlkStructType
                   instead of redefining each field.
10/20/06   br      Exchanged include pdsmsrch.h with gps_1xl1_api.h.
07/03/06   mr      Changes MSG-IDs from macros to enums
04/04/06   sk      Added more definitions
03/20/06   ld      Initial version.

=============================================================================*/


/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "gps_variation.h"
#include "customer.h"  /* Customer configuration file */
#include "comdef.h"    /* Definition for basic types and macros */
#include "gps_common.h"
#include "gnss_common.h"
#include "gnss_consts.h"
#include "aries_os_api.h"
#include "aries_os_globals.h"
#include "cgps_api.h"
#include "pdapi.h"
#include "gnss_cqsvc.h"  /* For Engine Error Recovery Report */
#include "gnss_lpp_ecid.h" /* LPP ECID definitions */
#include "wl_common.h" 
/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

/*=============================================================================

       Constants

=============================================================================*/
#define MAX_PPM_RM_RPT_SAMPLES 25
#define NUM_SOFT_DECISIONS 20

#ifdef FEATURE_SM_DEBUG
 #error code not present
#else
  #define SM_ASSERT( xx_exp ) \
     if( !(xx_exp) ) \
     { \
        ERR( "SM: Assertion " #xx_exp " failed",0,0,0 ); \
     }
#endif /* FEATURE_SMLM_DEBUG */

#define SM_GEN8_IPC_ID_START_RANGE 0x10000

/* The new master list */
/* Partial integration of the new IPC numbering scheme */

typedef enum
{
  M_ARIES_MSG_ID_MGP_LM_FIRST_0 = C_USR_MSG_ID_FIRST,
  /*********************************************************************
   ** MSG ID's: MGP Info reports MGP => LM
   **********************************************************************/
  M_ARIES_MSG_ID_MGP_LM_REPORT_GPS_MEAS_0_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_FIX_0_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_NO_FIX_0_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_UTC_INFO_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE_0_0,

  /***************************************************
   ** MSG ID's: MGP status reports MGP => LM
   ****************************************************/
  M_ARIES_MSG_ID_MGP_LM_REPORT_ASSISTANCE_STATUS_0_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_GENERAL_STATUS_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_EVENT_0,

  /***************************************************
   ** MSG ID's: PGI RF Info reports MGP => LM
   ****************************************************/
  M_ARIES_MSG_ID_MGP_LM_REPORT_RF_INFO_0,

  /***************************************************
   ** MSG ID's: CDMA reports AFLT => LM
   ****************************************************/
  M_ARIES_MSG_ID_AFLT_LM_REPORT_PPM_MEAS_0,
  M_ARIES_MSG_ID_AFLT_LM_REPORT_STATUS_0,
  M_ARIES_MSG_ID_AFLT_LM_REPORT_PPM_MEAS_IS801_0,

  /***************************************************
   ** MSG ID's: MGP data request MGP => LM
   ****************************************************/
  M_ARIES_MSG_ID_MGP_LM_REQ_SA_DATA_0,

  M_ARIES_MSG_ID_MGP_LM_REPORT_GPS_TIME_0_0,

  /***************************************************
   ** NOTE WELL:
   ** Revised/new messages - must go at end to avoid
   ** renumbering the messages above, s.t. tools like
   ** Apex and Playback can parse both Old and New
   ** files.
   ****************************************************/
  M_ARIES_MSG_ID_MGP_LM_REPORT_FIX_1_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE_1_0,

  M_ARIES_MSG_ID_MGP_LM_REPORT_FIX_2_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE_2_0,

  /** MSG from MC with the Cno, Freq info from the IQ test **/
  M_ARIES_MSG_ID_MGP_LM_REPORT_WB_IQ_PARAMS_0,
  M_ARIES_MSG_ID_MGP_LM_END_0,

  M_ARIES_MSG_ID_TM_LM_FIRST_0 = M_ARIES_MSG_ID_MGP_LM_END_0,
  M_ARIES_MSG_ID_TM_LM_SARF_MODE_SWITCH_0,
  M_ARIES_MSG_ID_TM_LM_PRESC_DWELL_0,
  M_ARIES_MSG_ID_TM_LM_IQ_TEST_0,
  M_ARIES_MSG_ID_TM_LM_1SV_STANDALONE_0, /* Also serves single sv */
  M_ARIES_MSG_ID_TM_LM_MEM_BW_TEST_0,
  M_ARIES_MSG_ID_TM_LM_TSG_CONFIG_0,
  M_ARIES_MSG_ID_TM_LM_RF_CONFIG_0,
  M_ARIES_MSG_ID_TM_LM_RFDEV_TEST_0, /* Callisto RF Dev Multichan Track test command message */
  M_ARIES_MSG_ID_TM_LM_PRX_RF_CONFIG_0,
  M_ARIES_MSG_ID_TM_LM_RF_LINEARITY_CONFIG_0,
  M_ARIES_MSG_ID_TM_LM_RF_DEV_FEATURE_CAPABILITIES_0, /* Query for build's RF Dev feature capabilities*/  
  M_ARIES_MSG_ID_TM_LM_GET_LATEST_WB_FFT_RESULTS_0,
  
  /* Following enums are reserved for OEMs usage */
  M_ARIES_MSG_ID_TM_LM_OEM_RESERVED_1,
  M_ARIES_MSG_ID_TM_LM_OEM_RESERVED_2,
  M_ARIES_MSG_ID_TM_LM_OEM_RESERVED_3,
  M_ARIES_MSG_ID_TM_LM_OEM_RESERVED_4,
  M_ARIES_MSG_ID_TM_LM_OEM_RESERVED_5,
  M_ARIES_MSG_ID_TM_LM_OEM_RESERVED_6,
  M_ARIES_MSG_ID_TM_LM_OEM_RESERVED_7,
  M_ARIES_MSG_ID_TM_LM_OEM_RESERVED_8,
  M_ARIES_MSG_ID_TM_LM_OEM_RESERVED_9,
  M_ARIES_MSG_ID_TM_LM_OEM_RESERVED_10,
  M_ARIES_MSG_ID_TM_LM_SPECTRUM_ANALYZER_CMD_0,
  M_ARIES_MSG_ID_TM_LM_END_0,
  
  M_C_SM_LM_MSG_FIRST_0 = M_ARIES_MSG_ID_TM_LM_END_0, 
  M_C_SM_LM_MSG_SESSION_REQUEST_0,    /* LM-TM session related request from TM to LM */
  M_C_SM_LM_MSG_REQUEST_0,            /* LM-TM session independent request from TM to LM */
  M_C_SM_LM_MSG_INFO_0,               /* LM-TM session independent info from TM to LM */
  M_C_SM_LM_DIAG_GSC_MSG_0,           /* GPS Session Control (GSC) DIAG packet from GPSDIAG to LM */ 
  M_C_SM_LM_SBAS_START_0,
  M_C_SM_LM_SBAS_STOP_0,
  M_C_SM_LM_SV_NO_EXIST_0,            /* Sv No Exist List updated from TM to LM */
  M_C_SM_LM_FORCE_DPO_FOR_POWER_MEAS_0,/* Force DPO for Power Meas Cmd from TM to LM */
  M_C_SM_LM_MSG_END_0,

  /****************************************************/
  /********** GEN8 MessageId Range Start **************/
  /****************************************************/
  M_ARIES_MSG_ID_MGP_LM_REPORT_GPS_MEAS_0 = SM_GEN8_IPC_ID_START_RANGE,
  M_ARIES_MSG_ID_MGP_LM_REPORT_FIX_3_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_NO_FIX_1_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE_3_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_ASSISTANCE_STATUS_1_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_GPS_TIME_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_ASSISTANCE_STATUS_0,
  M_MSG_ID_MGP_LM_REPORT_SV_POLY_0,
  M_SM_MSG_ID_MGP_LM_DRSYNC_CONFIG_0,
  M_SM_MSG_ID_MGP_LM_DRSYNC_PULSE_DESC_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_FIX_4_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_FIX_5_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE_4_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_FIX_6_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE_5_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_NO_FIX_2_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_FIX_7_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE_6_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_FIX_8_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE_0,
  M_SM_MSG_ID_MGP_LM_DEBUG_NMEA_0,
  M_SM_MSG_ID_MGP_LM_REPORT_XTRA_INJ_DONE_0,
  M_SM_MSG_ID_LM_MGP_REPORT_XTRA_INJ_DONE_0,
  M_SM_MSG_ID_MGP_LM_REPORT_NBIQ_PARAMS_0,
  M_SM_MSG_ID_MGP_LM_REPORT_GNSS_RF_STATUS_INFO_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_NO_FIX_3_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_FIX_9_0,
  M_SM_MSG_ID_MGP_LM_REPORT_GPS_EPHEMERIS_0,
  M_SM_MSG_ID_MGP_LM_REPORT_GLO_EPHEMERIS_0,
  M_SM_MSG_ID_MGP_LM_REPORT_GPS_SUBFRAME_0,
  M_SM_MSG_ID_MGP_LM_REPORT_GLO_STRING_0,
  M_SM_MSG_ID_MGP_RESET_LOCATION_SERVICE_DONE_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_NO_FIX_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_FIX_10_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_FIX_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_CPE_MEAS_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_FIX_1,
  M_ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE_1,
  M_MSG_ID_MGP_LM_REPORT_SV_POLY_1,
  M_ARIES_MSG_ID_MGP_LM_REPORT_GPS_MEAS_1,
  M_SM_MSG_ID_MGP_LM_REPORT_BDS_EPHEMERIS_0,
  M_SM_MSG_ID_MGP_LM_REPORT_GAL_EPHEMERIS_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_ASSISTANCE_STATUS_1,
  M_SM_MSG_ID_MGP_LM_REPORT_GAL_PAGE_0,
  M_ARIES_MSG_ID_MGP_LM_REPORT_GPS_TIME_1,
  M_SM_MSG_ID_MGP_LM_REPORT_QZSS_EPHEMERIS_0,

  /* When adding a new LM msg ID, you MUST insert any new items from the Master list, 
     before your new item (and update the Master list.) If you are not sure what the Master 
	 list is, ask your lead. */ 
	 
  M_C_SM_LM_IPC_MSG_END 
} m_lm_ipc_msg_id_type;


/* Old style IPC numbers allocation. Note: Only the end of the list is updated to new style */

typedef enum
{
ARIES_MSG_ID_MGP_LM_FIRST = C_USR_MSG_ID_FIRST,
/*********************************************************************
** MSG ID's: MGP Info reports MGP => LM
**********************************************************************/
ARIES_MSG_ID_MGP_LM_REPORT_GPS_MEAS_0,
ARIES_MSG_ID_MGP_LM_REPORT_FIX_0,
ARIES_MSG_ID_MGP_LM_REPORT_NO_FIX_0,
ARIES_MSG_ID_MGP_LM_REPORT_UTC_INFO,
ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE_0,

/***************************************************
** MSG ID's: MGP status reports MGP => LM
****************************************************/
ARIES_MSG_ID_MGP_LM_REPORT_ASSISTANCE_STATUS_0,
ARIES_MSG_ID_MGP_LM_REPORT_GENERAL_STATUS,
ARIES_MSG_ID_MGP_LM_REPORT_EVENT,

/***************************************************
** MSG ID's: PGI RF Info reports MGP => LM
****************************************************/
ARIES_MSG_ID_MGP_LM_REPORT_RF_INFO,

/***************************************************
** MSG ID's: CDMA reports AFLT => LM
****************************************************/
ARIES_MSG_ID_AFLT_LM_REPORT_PPM_MEAS,
ARIES_MSG_ID_AFLT_LM_REPORT_STATUS,
ARIES_MSG_ID_AFLT_LM_REPORT_PPM_MEAS_IS801,

/***************************************************
** MSG ID's: MGP data request MGP => LM
****************************************************/
ARIES_MSG_ID_MGP_LM_REQ_SA_DATA,

ARIES_MSG_ID_MGP_LM_REPORT_GPS_TIME_0,

/***************************************************
** NOTE WELL:
** Revised/new messages - must go at end to avoid
** renumbering the messages above, s.t. tools like
** Apex and Playback can parse both Old and New
** files.
****************************************************/
ARIES_MSG_ID_MGP_LM_REPORT_FIX_1,
ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE_1,

ARIES_MSG_ID_MGP_LM_REPORT_FIX_2,
ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE_2,

/** MSG from MC with the Cno, Freq info from the IQ test **/
ARIES_MSG_ID_MGP_LM_REPORT_WB_IQ_PARAMS,


ARIES_MSG_ID_MGP_LM_END,

/****************************************************/
/********** GEN8 MessageId Range Start **************/
/****************************************************/
ARIES_MSG_ID_MGP_LM_REPORT_GPS_MEAS_1 = SM_GEN8_IPC_ID_START_RANGE,
ARIES_MSG_ID_MGP_LM_REPORT_FIX_3,
ARIES_MSG_ID_MGP_LM_REPORT_NO_FIX_1,
ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE_3,
ARIES_MSG_ID_MGP_LM_REPORT_ASSISTANCE_STATUS_1,
ARIES_MSG_ID_MGP_LM_REPORT_GPS_TIME_1,
ARIES_MSG_ID_MGP_LM_REPORT_ASSISTANCE_STATUS_2,
MSG_ID_MGP_LM_REPORT_SV_POLY_0,
SM_MSG_ID_MGP_LM_DRSYNC_CONFIG,
SM_MSG_ID_MGP_LM_DRSYNC_PULSE_DESC,
ARIES_MSG_ID_MGP_LM_REPORT_FIX_4,
ARIES_MSG_ID_MGP_LM_REPORT_FIX_5,
ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE_4,
ARIES_MSG_ID_MGP_LM_REPORT_FIX_6,
ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE_5,
ARIES_MSG_ID_MGP_LM_REPORT_NO_FIX_2,
ARIES_MSG_ID_MGP_LM_REPORT_FIX_7,
ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE_6,
ARIES_MSG_ID_MGP_LM_REPORT_FIX_8,
ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE_7,
SM_MSG_ID_MGP_LM_DEBUG_NMEA,
SM_MSG_ID_MGP_LM_REPORT_XTRA_INJ_DONE,
SM_MSG_ID_LM_MGP_REPORT_XTRA_INJ_DONE,
SM_MSG_ID_MGP_LM_REPORT_NBIQ_PARAMS,
SM_MSG_ID_MGP_LM_REPORT_GNSS_RF_STATUS_INFO,
ARIES_MSG_ID_MGP_LM_REPORT_NO_FIX_3,
ARIES_MSG_ID_MGP_LM_REPORT_FIX_9,
SM_MSG_ID_MGP_LM_REPORT_GPS_EPHEMERIS,
SM_MSG_ID_MGP_LM_REPORT_GLO_EPHEMERIS,
SM_MSG_ID_MGP_LM_REPORT_GPS_SUBFRAME,
SM_MSG_ID_MGP_LM_REPORT_GLO_STRING,
SM_MSG_ID_MGP_RESET_LOCATION_SERVICE_DONE,
ARIES_MSG_ID_MGP_LM_REPORT_NO_FIX,
ARIES_MSG_ID_MGP_LM_REPORT_FIX_10,
ARIES_MSG_ID_MGP_LM_REPORT_FIX_11,
/* All additions below this line must be done using master list */
ARIES_MSG_ID_MGP_LM_REPORT_CPE_MEAS           = M_ARIES_MSG_ID_MGP_LM_REPORT_CPE_MEAS_0,
ARIES_MSG_ID_MGP_LM_REPORT_FIX                = M_ARIES_MSG_ID_MGP_LM_REPORT_FIX_1,
MSG_ID_MGP_LM_REPORT_SV_POLY                  = M_MSG_ID_MGP_LM_REPORT_SV_POLY_1,
ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE       = M_ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE_1,
ARIES_MSG_ID_MGP_LM_REPORT_GPS_MEAS           = M_ARIES_MSG_ID_MGP_LM_REPORT_GPS_MEAS_1,
SM_MSG_ID_MGP_LM_REPORT_BDS_EPHEMERIS         = M_SM_MSG_ID_MGP_LM_REPORT_BDS_EPHEMERIS_0,
SM_MSG_ID_MGP_LM_REPORT_GAL_EPHEMERIS         = M_SM_MSG_ID_MGP_LM_REPORT_GAL_EPHEMERIS_0,
ARIES_MSG_ID_MGP_LM_REPORT_ASSISTANCE_STATUS  = M_ARIES_MSG_ID_MGP_LM_REPORT_ASSISTANCE_STATUS_1,
ARIES_MSG_ID_MGP_LM_REPORT_GPS_TIME           = M_ARIES_MSG_ID_MGP_LM_REPORT_GPS_TIME_1,
SM_MSG_ID_MGP_LM_REPORT_GAL_PAGE              = M_SM_MSG_ID_MGP_LM_REPORT_GAL_PAGE_0,
SM_MSG_ID_MGP_LM_REPORT_QZSS_EPHEMERIS        = M_SM_MSG_ID_MGP_LM_REPORT_QZSS_EPHEMERIS_0,
C_SM_LM_IPC_MSG_END                           = M_C_SM_LM_IPC_MSG_END 
} mgp_lm_msg_id_type;



/*=============================================================================

       Macros

=============================================================================*/

/*=============================================================================

       Typedefs

=============================================================================*/

/*=============================================================================
   GNSS Measurement Report structure
=============================================================================*/

typedef struct
{
  uint32                 e_MeasRptReason; /* gnss_MeasRptReasonEnumType */ 
  gnss_MeasBlkStructType z_MeasBlk;

} sm_GpsMeasRptStructType;


typedef struct
{
  gnss_SvPoly z_SvPolyReport;

} sm_SvPolyReportType;


/**
* Structure containing Bias information for an individual sensor
**/
typedef struct 
{
  uint16  w_SensorValidMask; /* Indicated which of the axes have valid data
                                Use SENSOR_BIAS_VALID_xxx macro to find which
                                of the axes have valid data*/
  FLT f_XaxisValue; /*  X-axis Bias
                      - Units gyroscope:(radians)/(seconds)
                      - Units accelerometer:(meters)/(seconds^2)*/
  FLT f_YaxisValue; /*  Y-axis Bias
                      - Units gyroscope:(radians)/(seconds)
                      - Units accelerometer:(meters)/(seconds^2)*/
  FLT f_ZaxisValue; /*  Z-axis Bias
                      - Units gyroscope:(radians)/(seconds)
                      - Units accelerometer:(meters)/(seconds^2)*/   
} sm_SensorBiasValueStructType;

/**
 * On-device Sensor Bias data from PE
 */
typedef struct 
{
  uint16                       w_SeqenceNumber; /* Incremental packet serial number starting from 0, to identify missing packets  */
  sm_SensorBiasValueStructType z_GyroBias;      /* Contains axis validity mask + x,y,z values for Gyro bias */
  sm_SensorBiasValueStructType z_AccelBias;     /* Contains axis validity mask + x,y,z values for accel bias */
} sm_SensorParamRptStructType;

/* SM-specific navigation position flags. Each field is binary. The TRUE/FALSE state is described below. */
typedef struct
{
  /* GPS position report validity flags */
  uint64 b_IsItarViolated      : 1;
  uint64 b_IsGpsTimeValid      : 1;
  uint64 b_IsPuncValid         : 1;
  uint64 b_IsTuncValid         : 1;

  /* Technology used in computing the position fix flags */
  uint64 b_IsSatellite         : 1;   /* Satellite is used for computing position fix */
  uint64 b_IsCellId            : 1;   /* Cell Id is used for computing position fix */
  uint64 b_IsWifi              : 1;   /* Wifi is used for computing postion fix */
  uint64 b_IsSensors           : 1;   /* Sensors is used for computing position fix */
  uint64 b_IsRefLoc            : 1;   /* Reference location is used for computing position fix */
  uint64 b_IsCoarsePos         : 1;   /* Coarse position is used for computing position fix */

  uint64 b_IsPosBestAvail      : 1;   /* Pos is best available position */
  uint64 b_IsSensorPropogatedGNSSFix : 1;   /* Solution used Only INS ONLY fixes */

} sm_NavPosFlags;

/**   struct sm_GnssNavPosition describes the current position,
 *    velocity in the position engine. DO NOT change this
 *    structure without versioning the IPC logs which use this
 *    struct, which includes, but may not be limited to, via
 *    sm_GnssFixRptStructType
 *    ARIES_MSG_ID_MGP_LM_REPORT_FIX
 *    ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE
 * 
 *    This struct is derived from gps_NavSolution and differs
 *    only in that new fields have been added. this was done to
 *    help limit the changes cascading through the rest of SM.
 * 
 */
typedef struct
{
  /* Bit flags copied verbatim from gnss_NavSolutionStructType */
  gnss_NavSlnFlagsStructType z_PosFlags;

  /* Additional bit flags specific to SM */
  sm_NavPosFlags             z_SmPosFlags;

  /* GNSS Fix Time Information */
  uint16 w_GpsWeek;             /* GPS week number of fix */
  uint32 q_GpsTimeMs;           /* GPS time in week (milliseconds) of fix */
  uint8  u_GloNumFourYear;      /* Glonass four-year cycle number */
  uint16 w_GloNumDaysInFourYear;/* Glonass calendar day within the current four-year cycle */
  uint32 q_GloTimeMs;           /* Glonass time in day (milliseconds) */
  uint32 q_UtcTimeMs;           /* UTC time (milliseconds) adjusted for leap seconds */
  uint32 q_RefFCount;           /* Receiver's ms counter value */
  uint8  u_InitGpsTimeSrc;      /* Time source information (pdsm_extended_time_src_e_type) */

  /* GNSS Fix Position Information */
  uint32 q_PosCounter;          /* Position structure counter */
  DBL    d_PosEcef[3];          /* Earth Centered, Earth Fixed position fix (m) */
  DBL    d_PosLla[3];           /* Lat, Long, Alt */
  DBL    d_VelEcef[3];          /* ECEF velocity fix (m/s) */
  FLT    f_VelEnu[3];           /* East, North, Up velocity (m/s) */
  FLT    f_HeadingRad;          /* Heading angle, [0,2*pi] (rad). Referenced to North (0 rad) */
  FLT    f_HeadingUncRad;       /* Heading uncertainty (rad) */
  FLT    f_GnssHeadingRad;      /* Gnss Only heading */
  FLT    f_GnssHeadingUncRad;   /* Gnss Only heading uncertainty (rad) */

  FLT    f_Punc3dMeters;        /* Position standard deviation estimate (m) */
  FLT    f_PuncLatLonMeters[2]; /* Position (Lat, Long) uncertainty (m) */
  FLT    f_PuncVertMeters;      /* Vertical position uncertainty (m) */
  FLT    f_ErrorEllipse[3];     /* loc_uncrtnty_ang (degree), loc_uncrtnty_a (m), loc_uncrtnty_p (m) */
  FLT    f_Vunc3dMps;           /* Velocity uncertainty (m/s) */
  FLT    f_VuncEastNorthMps[2]; /* East, North velocity uncertainty (m/s) */
  FLT    f_VuncVertMps;         /* Vertical velocity uncertainty (m/s) */
  FLT    f_ClockBias;           /* Receiver's clock bias (m) */
  FLT    f_ClockBiasUncMs;      /* Receiver's clock uncertainty (m) */
  FLT    f_ClockDriftRate;      /* Receiver's clock drift (m/s) */
  FLT    f_ClockDriftRateUncMps;/* Receiver's clock frequency bias uncertainty (m/s) */
  gnss_PeTimeBiasStructType z_InterGnssTb[GNSS_TB_TOTNUM]; /* Inter-constellation Time Biases */
  FLT    f_FilteredAlt;         /* Filtered altitude (m) */
  FLT    f_FilteredAltUnc;      /* Filtered altitude uncertainty, one-sided (m) */
  FLT    f_UnfilteredAlt;       /* Unfiltered altitude (m) */
  FLT    f_UnfilteredAltUnc;    /* Unfiltered altitude uncertainty, one-sided (m) */
  FLT    f_MagDeviation;        /* Magnetic deviation from true north (degrees) */

  FLT    f_PDOP;                /* Position Dilution of Precision (unitless) */
  FLT    f_HDOP;                /* Horizontal DOP (unitless) */
  FLT    f_VDOP;                /* Vertial DOP (unitless) */

  uint8  u_FirstFix;            /* Set TRUE when we have are first fix */
  uint8  u_DriftEstimated;      /* Set TRUE when the clock drift rate is estimated from
                                 * a difference of two clock biases */

  uint8  u_MagDevGood;          /* TRUE if magnetic devation is good */
  uint8  u_IonoCorrected;       /* FALSE indicates that not all SV measurements were iono corrected */
  uint8  u_TropoCorrected;      /* FALSE indicates that not all SV measurements were tropo corrected */

  pdsm_pd_reliability_e_type e_HoriRelIndicator; /* horizontal reliability */
  pdsm_pd_reliability_e_type e_VertRelIndicator; /* vertical reliability */

  uint16 w_SensorDataUsageMask;    /* A bit set to 1 indicates that data from the corresponding 
                                      sensor was used in order to compute the content (position,
                                      velocity, heading) in this packet.
                                      0x0001 – Accelerometer
                                      0x0002 – Gyro
                                      0xFFFC - Reserved
                                      Future versions of the GPS service may use reserved bits 
                                      to represent new sensor types. A control point should 
                                      ignore any bits that it considers to be reserved. */
  uint16 w_SensorAidingIndicator;  /* A bit set to to 1 indicates that certain fields (position, 
                                      velocity, heading, etc) in parsed position data TLV were 
                                      aided with sensor data.
                                      0x0001 – Heading aided with sensor data
                                      0x0002 – Speed aided with sensor data
                                      0x0004 – Position aided with sensor data
                                      0x0008 – Velocity aided with sensor data
                                      0FFF0 – Reserved
                                      Future versions of the GPS service may use reserved bits 
                                      to represent new methods of sensor aiding (e.g. tightly 
                                      coupled measurement combinations). A control point should 
                                      ignore any bits that it considers to be reserved. */
} sm_GnssNavPosition;


/**
     struct containing fix info from PE

     This structure is used by PE to notify SM of periodic fixes
   
     @see sm_ReportGnssFixInfo
*/

/* for backward/forward compatibility, in case some day we may need to make 
   SM_API & PDAPI's NavInfo structs different */
#define SM_GNSS_SV_INFO_MEAS_VALID       PDSM_GNSS_SV_INFO_MEAS_VALID 
#define SM_GNSS_SV_INFO_USABLE           PDSM_GNSS_SV_INFO_USABLE 
#define SM_GNSS_SV_INFO_USED_IN_POS_FIX  PDSM_GNSS_SV_INFO_USED_IN_POS_FIX 
#define SM_GNSS_SV_INFO_USED_IN_VEL_FIX  PDSM_GNSS_SV_INFO_USED_IN_VEL_FIX 

/* use PDAPI SVINFO structure diretly since PDAPI is now internal API */
typedef pdsm_GnssNavSvInfo      sm_GnssNavSvInfo;
typedef pdsm_GnssSvInfoType     sm_GnssSvInfoType;


typedef struct
{
  sm_GnssNavPosition          z_NavPos;     /* position-related info */

  sm_GnssNavSvInfo            z_NavSvInfo;  /* SV-related info */

  sm_SensorParamRptStructType z_OnDeviceSensorParam; /* sensor-related info */
} sm_GnssFixRptStructType;

//DELME
typedef sm_GnssFixRptStructType   sm_GpsFixRptStructType;

/*=============================================================================
   GPS Assistance data status structure
=============================================================================*/
/* Bit-mask definitions for the Wish list */
#define C_GNSS_ASSIST_REFTIME  (1U<<0)      /// Reference Time (GPS or GLONASS)
#define C_GNSS_ASSIST_REFLOC   (1U<<1)      /// Reference Location
#define C_GNSS_ASSIST_ACQ      (1U<<2)      /// Acquisition Assistance contains SV direction & steering info
#define C_GNSS_ASSIST_SA        (1U<<3)     /// Sensitvity Assistance
#define C_GNSS_ASSIST_NAVMODL  (1U<<4)      /// Ephemeris a.k.a. NavigationModel
#define C_GNSS_ASSIST_ALM      (1U<<5)      /// Almanac 
#define C_GNSS_ASSIST_IONO     (1U<<6)      /// IonosphericModel (Propogated signal delay)
#define C_GNSS_ASSIST_RTI      (1U<<7)      /// RealTimeIntegrity a.k.a satellite health info
#define C_GNSS_ASSIST_UTC      (1U<<8)      /// UTC Time info (used to relate with GPS or GLONASS system)
#define C_GNSS_ASSIST_TIME_MODELS (1U<<9)   /// Time model (GNSS-GNSS system time offset)
#define C_GNSS_ASSIST_AUXI_INFO   (1U<<10)  /// Auxiliary Info (Used in conjunction with NavModel)

/* the following are retained for backwards compatibility
   but their use is deprecated. Use C_GNSS_ASSIST_xxx flags
   instead 
*/
#define C_GPS_ASSIS_REFTIME      C_GNSS_ASSIST_REFTIME  
#define C_GPS_ASSIS_REFLOC       C_GNSS_ASSIST_REFLOC   
#define C_GPS_ASSIS_ACQ          C_GNSS_ASSIST_ACQ      
#define C_GPS_ASSIS_SA           C_GNSS_ASSIST_SA       
#define C_GPS_ASSIS_NAVMODL      C_GNSS_ASSIST_NAVMODL  
#define C_GPS_ASSIS_ALM          C_GNSS_ASSIST_ALM      
#define C_GPS_ASSIS_IONO         C_GNSS_ASSIST_IONO     
#define C_GPS_ASSIS_RTI          C_GNSS_ASSIST_RTI      
#define C_GPS_ASSIS_UTC          C_GNSS_ASSIST_UTC      

typedef struct
{
  uint16 w_Sv;                     /* SV PRN */  
  uint16 w_Iode;                   /* Issue of data Ephemeris tag */

} sm_SvEphIodeStructType;

typedef struct
{
  FLT f_PosUnc;
  boolean u_Valid;
} sm_PosUncStructType;

typedef struct
{
  FLT f_TimeUnc;
  boolean u_Valid;
} sm_TimeUncStructType;

typedef struct
{
   uint16                 w_Week;     /* GPS Week of Ephemeris validity */
   uint16                 w_Toe;      /* Reference time of the latest Ephemeris */
   uint16                 w_ToeLimit; /* Ephemeris Age tolerance (hours) */

} sm_GpsEphTime;

typedef struct
{
  uint16                 w_N4;        /* GLONASS 4-Year count (N4)*/
  uint16                 w_Na;        /* GLONASS Day in 4-year cycle (Na) */
  uint16                 w_Tb;        /* Reference time of the latest Ephemeris: uinit: 15 minutes, range 0 to 95 */
  uint16                 w_ToeLimit;  /* Ephemeris Age tolerance, double sided. 
                                         1 = 30 min, 2 = 45 min, 3 = 60 min; 
                                         0 = reserved; other = invalid */
} sm_GloEphTime;

typedef struct
{
   uint16                 w_Week;     /* BDS Week of Ephemeris validity */
   uint16                 w_ToeLimit; /* Ephemeris Age tolerance (hours) */
   uint32                 q_Toe;      /* Reference time of the latest Ephemeris */
} sm_BdsEphTime;

typedef struct
{
   uint16                 w_Week;     /* GAL Week of Ephemeris validity */
   uint16                 w_ToeLimit; /* Ephemeris Age tolerance (hours) */
   uint32                 q_Toe;      /* Reference time of the latest Ephemeris */
} sm_GalEphTime;

typedef union
{
   sm_GpsEphTime z_GpsEphTime;
   sm_GloEphTime z_GloEphTime;
   sm_BdsEphTime z_BdsEphTime;
   sm_GalEphTime z_GalEphTime;
   /* add other constellation times here */
} sm_GnssEphTime;

typedef struct
{
   boolean                u_EphInfoIncl;     /**< TRUE if EPH information is included */
   uint8                  u_NumSvs;          /**< Number of SVs for which IODE info is provided */
   uint32                 q_WishListMask;    /**< Bit-mask indicating the type of data needed */
   uint32                 q_EphHaveList;     /**< Bit-mask of SVs for which Ephemeris is available */
   uint32                 q_EphNeedList;     /**< Bit-mask of SVs for which Ephemeris is needed */
   uint32                 q_AlmHaveList;     /**< Bit-mask of SVs for which Almanac is available */
   uint32                 q_AlmNeedList;     /**< Bit-mask of SVs for which Almanac is needed */
   uint32                 q_VisibleSVList;   /**< Bit-mask of all visible SVs*/
   uint32                 q_SvNoExistMask;   /**< Bit-mask of SVs which are non-existent */
   sm_GnssEphTime         z_AssistDataTime;  /**< time associated with navigation data */
   sm_TimeUncStructType   z_TimeUnc;         /**< Time uncertainty information */
   sm_PosUncStructType    z_PosUnc;          /**< Horizontal position uncertainty */ 
   uint32                 q_HealthHaveList;  /**< Good list */
   uint32                 q_HealthNeedList;  /**< Unknown */
   uint32                 q_HealthBadList;   /**< Bad SVs */
   sm_SvEphIodeStructType z_SvEphIode[N_SV];
} sm_GnssAssistDataStruct;

typedef struct
{
   boolean                u_EphInfoIncl;     /**< TRUE if EPH information is included */
   uint8                  u_NumSvs;          /**< Number of SVs for which IODE info is provided */
   uint32                 q_WishListMask;    /**< Bit-mask indicating the type of data needed */
   uint64                 t_EphHaveList;     /**< Bit-mask of SVs for which Ephemeris is available */
   uint64                 t_EphNeedList;     /**< Bit-mask of SVs for which Ephemeris is needed */
   uint64                 t_AlmHaveList;     /**< Bit-mask of SVs for which Almanac is available */
   uint64                 t_AlmNeedList;     /**< Bit-mask of SVs for which Almanac is needed */
   uint64                 t_VisibleSVList;   /**< Bit-mask of all visible SVs*/
   uint64                 t_SvNoExistMask;   /**< Bit-mask of SVs which are non-existent */
   sm_GnssEphTime         z_AssistDataTime;  /**< time associated with navigation data */
   sm_TimeUncStructType   z_TimeUnc;         /**< Time uncertainty information */
   sm_PosUncStructType    z_PosUnc;          /**< Horizontal position uncertainty */ 
   uint64                 t_HealthHaveList;  /**< Good list */
   uint64                 t_HealthNeedList;  /**< Unknown */
   uint64                 t_HealthBadList;   /**< Bad SVs */
   sm_SvEphIodeStructType z_SvEphIode[N_BDS_SV];
} sm_BdsAssistDataStruct;

typedef struct 
{
   boolean                u_EphInfoIncl;     /**< TRUE if EPH information is included */
   uint8                  u_NumSvs;          /**< Number of SVs for which IODE info is provided */
   uint32                 q_WishListMask;    /**< Bit-mask indicating the type of data needed */
   uint64                 t_EphHaveList;     /**< Bit-mask of SVs for which Ephemeris is available */
   uint64                 t_EphNeedList;     /**< Bit-mask of SVs for which Ephemeris is needed */
   uint64                 t_AlmHaveList;     /**< Bit-mask of SVs for which Almanac is available */
   uint64                 t_AlmNeedList;     /**< Bit-mask of SVs for which Almanac is needed */
   uint64                 t_VisibleSVList;   /**< Bit-mask of all visible SVs*/
   uint64                 t_SvNoExistMask;   /**< Bit-mask of SVs which are non-existent */
   sm_GnssEphTime         z_AssistDataTime;  /**< time associated with navigation data */
   sm_TimeUncStructType   z_TimeUnc;         /**< Time uncertainty information */
   sm_PosUncStructType    z_PosUnc;          /**< Horizontal position uncertainty */ 
   uint64                 t_HealthHaveList;  /**< Good list */
   uint64                 t_HealthNeedList;  /**< Unknown */
   uint64                 t_HealthBadList;   /**< Bad SVs */
   sm_SvEphIodeStructType z_SvEphIode[N_GAL_SV];
} sm_GalAssistDataStruct;

typedef struct
{
  boolean                 u_EphInfoIncl;     /**< TRUE if EPH information is included */
  uint8                   u_NumSvs;          /**< Number of SVs for which IODE info is provided */
  uint32                  q_WishListMask;    /**< Bit-mask indicating the type of data needed */
  uint32                  q_EphHaveList;     /**< Bit-mask of SVs for which Ephemeris is available */
  uint32                  q_EphNeedList;     /**< Bit-mask of SVs for which Ephemeris is needed */
  uint32                  q_AlmHaveList;     /**< Bit-mask of SVs for which Almanac is available */
  uint32                  q_AlmNeedList;     /**< Bit-mask of SVs for which Almanac is needed */
  uint32                  q_VisibleSVList;   /**< Bit-mask of all visible SVs*/
  uint32                  q_SvNoExistMask;   /**< Bit-mask of SVs which are non-existent */
  sm_GnssEphTime          z_AssistDataTime;  /**< time associated with navigation data */
  sm_TimeUncStructType    z_TimeUnc;         /**< Time uncertainty information */
  sm_PosUncStructType     z_PosUnc;          /**< Horizontal position uncertainty */
  uint32                  q_HealthHaveList;  /**< Good list */
  uint32                  q_HealthNeedList;  /**< Unknown */
  uint32                  q_HealthBadList;   /**< Bad SVs */
  sm_SvEphIodeStructType  z_SvEphIode[N_QZSS_SV];
} sm_QzssAssistDataStruct;





typedef struct 
{
  uint32                   q_WishListMask;   /**< Bit-mask indicating the type of data needed */
  uint32                   q_EphHaveList;    /**< Bit-mask of SVs for which Ephemeris is available */
  uint32                   q_EphNeedList;    /**< Bit-mask of SVs for which Ephemeris is needed */
  uint32                   q_AlmHaveList;    /**< Bit-mask of SVs for which Almanac is available */
  uint32                   q_AlmNeedList;    /**< Bit-mask of SVs for which Almanac is needed */
  uint32                   q_VisibleSVList;  /**< Bit-mask of all visible SVs*/
  uint32                   q_SvNoExistMask;  /**< Bit-mask of SVs which are non-existent */
  boolean                  u_EphInfoIncl;    /**< TRUE if EPH information is included */
  uint16                   w_GpsWeek;        /**< GPS Week of Ephemeris validity */
  uint16                   w_Toe;            /**< Reference time of the latest Ephemeris */
  uint16                   w_ToeLimit;    /**< Ephemeris Age tolerance (hours) */
  uint8                    u_NumSvs;      /**< Number of SVs for which IODE info is provided */
  sm_TimeUncStructType     z_TimeUnc;       /**< Time uncertainty information */
  sm_PosUncStructType      z_PosUnc;        /**< Horizontal position uncertainty */ 
  sm_SvEphIodeStructType   z_SvEphIode[N_SV];
  uint32                   q_HealthHaveList;
  uint32                   q_HealthNeedList;
  uint32                   q_HealthNoExList;
  uint32                   q_HealthBadList;

  sm_GnssAssistDataStruct  z_GloAssistData; /**< glonass assist data status structure */
  sm_BdsAssistDataStruct   z_BdsAssistData; /**< BDS assist data status structure */
  sm_GalAssistDataStruct   z_GalAssistData; /**< GAL assist data status structure */
  sm_QzssAssistDataStruct  z_QzssAssistData; /**< QZSS assist data status structure */
  uint8                    u_Valid; /**< which assistance data is valid. 
                                                  Bit 0 for gps,
                                                  bit 1 for Glonass
                                                  bit 2 for BDS
                                                  bit 3 for Galileo
                                                  bit 4 for QZSS
                                                  bit set=valid, not-set=invalid.
                                                  Use SM_ASSIST_DATA_VALID_xxx */
} sm_InternalAssistDataStatusStructType;

#define SM_ASSIST_DATA_VALID_MASK_GPS          ((uint16)(0x01<<0))
#define SM_ASSIST_DATA_VALID_MASK_GLONASS      ((uint16)(0x01<<1))
#define SM_ASSIST_DATA_VALID_MASK_BDS          ((uint16)(0x01<<2))
#define SM_ASSIST_DATA_VALID_MASK_GAL          ((uint16)(0x01<<3))
#define SM_ASSIST_DATA_VALID_MASK_QZSS         ((uint16)(0x01<<4))

     typedef struct
     {
        uint16                   w_Valid; /**< which assistance data is valid. 
                                                  Bit 0 for gps,
                                                  bit 1 for Glonass
                                                  bit 2 for BDS
                                                  bit 3 for GAL
                                                  bit 4 for QZSS
                                           bit set=valid, not-set=invalid.
                                           Use SM_ASSIST_DATA_VALID_xxx */

  sm_GnssAssistDataStruct    z_GpsAssistData;   /**< gps assistance data */
  sm_GnssAssistDataStruct    z_GloAssistData;   /**< glonass assistance data */
  sm_BdsAssistDataStruct     z_BdsAssistData;   /**< BDS assistance data */
  sm_GalAssistDataStruct     z_GalAssistData;   /**< GAL assistance data */
  sm_QzssAssistDataStruct    z_QzssAssistData;  /**< QZSS assistance data */
} sm_GnssAssistDataStatusStructType;


/*=============================================================================
   Pilot Phase Measurement Report structure
=============================================================================*/
typedef struct
{
  int32 l_PilotPnPhase;
  uint8 u_PilotRmse;
  uint8 u_PilotEcio;

} sm_PilotInfoStructType;

/** UTC information
 * 
 * UTC data decoded by PE off the air to SM. Since both GLONASS
 * and GPS times are consistent, leap second conversion is
 * unnecessary and the same API may be used. Refer MGP TO SM API
 * documentation for details
 * 
 */
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

} sm_UtcInfoRptStructType;

/*=============================================================================
   Receiver State Change event
=============================================================================*/
/**
     Receiver State Change event

     ME reports receiver state change and associated gpsRtc
*/
typedef struct 
{
  uint32                   q_GpsRtcMs;
  gps_RcvrStateEnumType e_CurrentRcvrState;
} sm_RcvrStateChangeStructType;

/**
     SM Event Report type

     ME and PE report back to SM when one of these events
     happens
*/
typedef enum
{
  /**< Event type */                 /**< When generated */
  SM_EVENT_MIN = 0,
  SM_EVENT_ME_TIME_TICK,          /**< Periodically in Receiver IDLE/ON modes */
  SM_EVENT_ME_NEW_MEAS,           /**< Periodically, if new measurements are obtained */
  SM_EVENT_ME_ASSIGN_CHANGE,      /**< Processing mode changes in ME */
  SM_EVENT_ME_CELL_CHANGE,        /**< When serving cell changes */
  SM_EVENT_ME_TIME_TRANSFER,      /**< New time-transfer took place. Cell changed or periodic update on current cell */
  SM_EVENT_ME_TIME_UPDATE,        /**< Clock update in ME, better (or forced) estimate received */
  SM_EVENT_ME_TIME_DISCONTINUITY, /**< ME detected a time discontinuity */
  SM_EVENT_ME_TIME_ERR_DETECT,    /**< ME detected an error in its clock estimate */
  SM_EVENT_ME_MEAS_SUSPENDED,     /**< Measurements are suspended (resources pre-empted) */
  SM_EVENT_ME_MEAS_ABORTED,       /**< Measurements are aborted (resources lost completely for the session) */
  SM_EVENT_ME_SA_DATA_NEEDED,     /**< ME needs SA data to improve the sensitivity */
  SM_EVENT_PE_NEW_NAVDATA,        /**< PE received new Ephemeris */
  SM_EVENT_PE_NEW_ALMANAC,        /**< PE received new Almanac */
  SM_EVENT_PE_NEW_HEALTH,         /**< PE received new SV Health info */
  SM_EVENT_PE_NEW_IONO_UTC,       /**< PE received new Iono/UTC info */
  SM_EVENT_PE_DEL_NAVDATA,        /**< Ephemeris data is deleted */
  SM_EVENT_PE_DEL_ALMANAC,        /**< Almanac data is deleted */
  SM_EVENT_PE_STITCH_FIRST_BP,    /**< PE has started Stitch back propogation */
  SM_EVENT_PE_STITCH_INCOMPLETE_BP,/**< PE has suspended Stitch back propogation */
  SM_EVENT_PE_STITCH_LAST_BP,     /**< PE has completed Stitch back propogation */
  SM_EVENT_PE_KF_RESET_OR_INIT,   /**< KF has been reset or initialized */
  SM_EVENT_PE_LOG_KF,             /**< KF data has been logged */
  SM_EVENT_ME_IQ_DONE,            /**< IQ Collect/FFT done */
  SM_EVENT_PE_NO_NEW_FIX,         /**< PE no new fix generated */
  SM_EVENT_PE_LOG_ALM_CORR,       /**< XTRA alm corr have been logged */
  SM_EVENT_ME_SIG_PROC_MEM_ALLOC_ERROR, /** Error event to indicate Session mode allocation failure for Signal Processing memory*/
  SM_EVENT_MAX

} sm_EventEnumType;

/*=============================================================================
   Structure for reporting events to SM
=============================================================================*/
typedef struct
{
  sm_EventEnumType e_EventType;
  uint32           q_EventData;
  uint32           q_EventTime;
} sm_ReportEventStructType;

typedef union
{
  cgps_1xRfInfoType   CDMARfInfo;
  cgps_EVDORfInfoType EVDORfInfo;
} sm_ReportRfInfoUnionType;

typedef struct
{
  gps_DataSourceType       RfInfoSource;
  sm_ReportRfInfoUnionType RfInfoData;
} sm_ReportRfInfoStructType;

/**
 * Time report from MGP/PE
 * 
 * Delivers estimate of current time to SM. PE calls this API in
 * response to a request from SM via mgp_ReqGnssTime
 */
typedef struct
{
   gps_TimeChkSourceType source;
   gnss_ClockStructType  gnssClock;
} sm_ReportGnssTimeStructType;


/**
 * SM_GEN7_COMPATIBILITY(sm_ReportGpsTimeStructType)
 * sm_ReportGpsTimeStructType is being retained for backwards
 * compatibility. Gen7 MGP APIs call this to report time.
 * this however, may be removed at a later date since the
 * data actually comes from PE.
 * See sm_ReportGnssTime in MGP to SM api documentation
 */
typedef struct
{
  gps_TimeChkSourceType    source;
  gps_ClockStructType      GpsClock;
} sm_ReportGpsTimeStructType;

/**
     Latest position and associated uncertainties

     This structure is used by ME to indicate the position and
     its associated uncertainties.
   
     @see sm_UpdatePosPuncTunc
*/
typedef struct cgps_OnDemandPosPuncTunc
{
//  sm_GpsFixRptStructType pos_info; /* null if no valid position is available */
  boolean b_Valid; /**< specifies if the pos punc/tunc are valid */
  FLT     f_Tunc; /**< in us */
  FLT     f_Punc; /**< in meters */
  uint32  eph_svmask; /**< SV Mask for the ephemeris */
  uint32  alm_svmask; /**< SV Mask for the Almanac */
  uint32  health_svmask; /**< SV Mask for the health */
  uint32  health_bad_svmask; /**< SV Mask for the health */
  uint32  health_noex_svmask; /**< SV Mask for the health */
  uint32  health_unk_svmask; /**< SV Mask for the health */
  uint32  visible_svmask; /**< SV mask for visible SVs*/
} sm_OnDemandPosPuncTuncStruct;

typedef struct
{
  /* GPS C/N0 results */
  FLT f_GpsCNoDBHz;  /* Signal strength calculated in 
                          dBHz as part of WBIQ test*/
  FLT f_GpsFreqHz; /* Freq in Hz calculated as part of WBIQ test */

  /* GLO C/N0 results */
  FLT f_GloCNoDBHz;  /* Signal strength calculated in 
                           dBHz as part of WBIQ test*/
  FLT f_GloFreqHz; /* Freq in Hz calculated as part of WBIQ test */

  /* BDS C/N0 results */
  FLT f_BdsCNoDBHz;  /* Signal strength calculated in 
                          dBHz as part of WBIQ test*/
  FLT f_BdsFreqHz; /* Freq in Hz calculated as part of WBIQ test */

  /* GAL C/N0 results */
  FLT f_GalCNoDBHz;  /* Signal strength calculated in 
                          dBHz as part of WBIQ test*/
  FLT f_GalFreqHz; /* Freq in Hz calculated as part of WBIQ test */

  uint8 u_GnssConfig;    /* Flags containing configured 
                            constellations */

  /* ADC mean/amp in units of 0.1 mV */
  int32 l_AdcMeanIp1mV; /* Mean Estimate (DC) of I component in 
                       ADC processor in 0.1 mV */
  int32 l_AdcMeanQp1mV; /* Mean Estimate (DC) of Q component in 
                       ADC processor in 0.1 mV */
  uint32 q_AdcAmpIp1mV; /* Amplitude estimate of I component in 
                       ADC processor in 0.1 mV */
  uint32 q_AdcAmpQp1mV; /* Amplitude estimate of Q component in 
                       ADC processor in 0.1 mV */

  uint8  u_AdcIqSatPerStatus;  /* Indicates validity/status of data.
                                  0 - ADC IQ saturation percentage is not valid
                                  3 – Number of samples in ADC IQ test was zero
                                  4 – ADC IQ saturation percentage is valid */
  uint32 q_ISatPer; /* Percentage of I samples saturated, in units of 0.001 */
  uint32 q_QSatPer; /* Percentage of Q samples saturated, in units of 0.001 */
  int32  l_AdcIqWbJammerPwrEstDb; /* Estimated wideband jammer power in units of 0.01 dB.
                                     Only valid if wideband jammer power estimation was enabled in the 
                                     ADC IQ test request.*/
} sm_ReportWBIQInfoStructType;

/* This structure is used to pass on NBIQ data pointer
   and size of NBIQ data to SM from MGP */
typedef struct
{
  void*  p_NBIQDataPtr;
  uint32 q_NBIQDataSize;
} sm_ReportNBIQInfoStructType;


/* This structure is used to pass on BP Amp and 
   PGA gain information from MGP to SM. This can be extended
   later on if OEM wants other GNSS RF related data */
typedef struct
{
  int32   l_PGAGain;

  /* BP1 params */
  uint32  q_Bp1LbwAmplI;
  uint32  q_Bp1LbwAmplQ; 

  /* BP3 GLO processor status */
  uint32  q_Bp3GloAmplI;
  uint32  q_Bp3GloAmplQ; 
} sm_ReportGNSSRfStatusInfoStructType;

#define SM_GPS_STATE_POS_VALID 0x0001
#define SM_GPS_STATE_TIME_VALID 0x0002
#define SM_GPS_STATE_PUNC_VALID 0x0004
#define SM_GPS_STATE_TUNC_VALID 0x0008
#define SM_GPS_STATE_FUNC_VALID 0x0010
#define SM_GPS_STATE_FCLKDRT_VALID 0x0020
#define SM_GPS_STATE_CLKBIAS_VALID 0x0040

typedef enum
{
  SM_GPS_STATE_INFO_REQ_TYPE_NONE                      = 0,
  SM_GPS_STATE_INFO_REQ_TYPE_ON_DEMAND                 = 1,
  SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE               = 2,
  SM_GPS_STATE_INFO_REQ_TYPE_DIAG_POS                  = 3,
  SM_GPS_STATE_INFO_REQ_TYPE_GET_PUNC_TUNC_FOR_WIFI    = 4,
  SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_QMI    = 5,
  SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_XTRA_T = 6,
  SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_QMI     = 7,
  SM_GPS_STATE_INFO_REQ_TYPE_GET_GNSS_AVAIL_POS_QMI     = 8,
  SM_GPS_STATE_INFO_REQ_TYPE_GET_REFLOC_AVAIL_POS_QMI     = 9,
  SM_GPS_STATE_INFO_REQ_TYPE_GET_COARSE_AVAIL_POS_QMI     = 10,
  SM_GPS_STATE_INFO_REQ_TYPE_GET_WIFI_AVAIL_POS_QMI     = 11,
  SM_GPS_STATE_INFO_REQ_TYPE_GET_GILE_AVAIL_POS_QMI     = 12,
  SM_GPS_STATE_INFO_REQ_TYPE_MAX                       = 13
} sm_GpsStateInfoReqEnumType;

typedef enum
{
 SM_LOC_EST_SRC_GPS  = 1,
 SM_LOC_EST_SRC_WIFI  = 2
}sm_LocEstimateSrcEnumType;

typedef struct sm_GpsStateInfo
{
  sm_GpsStateInfoReqEnumType  b_ReqType;
  uint32                      q_Flags;      /* flags to indicate which fields are valid */
  uint8                       e_PosInfoSrc; /* Source of the Position Estimate */
  DBL                         d_PosLLa[3];
  FLT                         f_AltUnc;     /* Altitude Uncertainty in meters */
  FLT                         f_Tunc;       /* in us */
  FLT                         f_Punc;       /* in meters */

  uint16                      w_GpsWeek;    /* GPS Week time reference for the Position */
  uint32                      q_GpsMsec;    /* GPS Msec time reference for the Position */
  FLT                         f_Func;       /* 1 sided max freq bias uncertainty [m/s] */
  FLT                         f_ClkDrift;   /* Clock Drift in meters/sec */
  FLT                         f_ClkBias;    /* Clock frequency bias in meters/sec */
} sm_GpsStateInfoStruct;

/* Structure for reporting the best available fix to SM */
typedef struct
{
  sm_GpsStateInfoReqEnumType b_ReqType;      /* Source of the request for best available position */
  sm_GnssFixRptStructType    z_SmGnssFixRpt; /* Fix report information from the engine */
} sm_PositionReportStructType;

#ifdef FEATURE_CGPS_XTRA_T
/* Structure for transferring cell data from MC to SM */
typedef struct
{
  uint8  u_PartNumber; /* current part number of this structure */
  uint8  u_TotalParts; /* total number of parts in overall transfer */
  uint32 q_DataSize;   /* number of bytes of data pointed to by p_DataPtr */
  void*  p_DataPtr;    /* pointer to byte buffer containing cell data */
} sm_ReportCellDataStructType;
#endif /* FEATURE_CGPS_XTRA_T */

typedef struct
{
  uint8    u_Buffer[MAX_NMEA_SENTENCE_LEN]; /* debug NMEA payload */
  uint32   dwLength;    /* length of payload including the null termination */
  uint32   dwReserved;  /* reserved for future use */
} sm_DebugNMEA;

typedef struct
{
  uint16            w_Prn;
  gps_EphStructType z_GpsEphemeris;
} sm_GpsEphemeris;

typedef struct
{
  uint16            w_SlotId;
  glo_EphStructType z_GloEphemeris;
} sm_GloEphemeris;

typedef struct
{
  uint16            w_Prn;
  bds_EphStructType z_BdsEphemeris;
} sm_BdsEphemeris;

typedef struct
{
  uint16            w_Prn;  
  gal_EphStructType z_GalEphemeris;
} sm_GalEphemeris;

/*---------------------------------------------------------------------------
                      GPS Presc Dwell LOG TYPE
---------------------------------------------------------------------------*/
 /* -------------------------------------------------------------------
 ** NAME : LOG_GPS_PRESCRIBED_DWELL_RESULT
 **
 ** DESCRIPTION : It is logged everytime there's presc dwell result avaialable.
 **
 ** PERIODICITY : Async. Depends on dwell duration.
 ** ------------------------------------------------------------------- */

typedef PACKED struct PACKED_POST
{
  uint8  u_Version;
  uint8  u_RespType; /* 0 Dwell Results
                      * 1 Incoh Sums
                      * 2 Coh Sums
                      * 3-255 Reserved
                      */

  uint32  q_TotalPackets;
  uint32  q_CurrPacketNum;
} sm_PrescDwellCommon;

typedef PACKED struct PACKED_POST
{
  uint16  w_Sv; 
  uint32  q_GpsRtc;
  uint8   u_Flags;
  uint8   u_SrchModeIndex;
  uint16  w_PostDetN;
  uint16  w_CnoDb;
  uint32  f_SubMs;
  uint32  f_DopplerFreq;
  uint32  f_MultipathEst;
  uint32  q_Reserved0;
} sm_PrescDwellResult;

/* Final struct defining the DM log packet */
LOG_RECORD_DEFINE(LOG_GPS_PRESCRIBED_DWELL_RESULT_C)

  sm_PrescDwellCommon z_PrescDwellCommon;
  sm_PrescDwellResult z_PrescDwellResult;
LOG_RECORD_END

typedef LOG_GPS_PRESCRIBED_DWELL_RESULT_C_type sm_PrescDwellLogType;



/*---------------------------------------------------------------------------
                      GPS DEMOD SOFT DECISIONS LOG TYPE
---------------------------------------------------------------------------*/

/* -------------------------------------------------------------------
** NAME : log_gps_demod_soft_decisions_type
**
** DESCRIPTION : It is logged everytime GPS searcher accumulates
** 20 soft DPSK decisions from firmware.
**
** PERIODICITY : every 20 bits which is approximately 400ms for each
**   active channel.
** ------------------------------------------------------------------- */

LOG_RECORD_DEFINE(LOG_GPS_DEMOD_SOFT_DECISIONS_C)

  uint8 u_VersionId;        /* The version of this log type */

  uint16 w_Sv;               /* SV PRN number */ 

  uint16 w_DemodId;         /* This ID will be used to correlate across demod-
                            ** related logs that come from the same channel. It
                            ** is incremented everytime any channel is newly started.
                            ** It has a different purpose than SV PRN number which
                            ** one might think can be used, but if let's say a
                            ** channel is aborted due to tuneback, we want to
                            ** distinguish between data coming from channel before
                            ** tuneback or after tuneback (since they have the same
                            ** SV PRN number) */

  uint32 q_BitId;           /* This ID will be used to enumerate this log, it
                            ** serves two purposes:
                            ** - to detect dropped packet of this log
                            ** - in addition to task id, to further correlate
                            **   this log with other demod related logs
                            ** It represents how many GPS bits have passed since
                            ** the start of the current demod session */

                            /* Soft DPSK decisions straight from firmware, untruncated */
  int16 x_SoftDecisions[NUM_SOFT_DECISIONS];

  uint8 u_NumBits;          /* Number of bits in the array */

LOG_RECORD_END

typedef LOG_GPS_DEMOD_SOFT_DECISIONS_C_type log_gps_demod_soft_decisions_type;

/* for engine error recovery report, QMI header file gnss_cqsvc.h uses it
   original struct defined in position_determination_service_v01.h  */
typedef  cqsvc_engine_err_recovery_rpt_ind_msg gnss_engine_err_recovery_rpt;

/* -----------------------------------------------------------------------*//**
@brief
  This callback function is called by TM once PDAPI is intialized. This is 
  used to notify of potential clients that PDAPI clients can now initialize. 
*//* ------------------------------------------------------------------------*/
typedef void (*sm_ApiInitNotifierCb)
(
  void
);

typedef pdsm_pd_ext_dpo_status_rpt sm_DpoStatusReportStructType;

/*=============================================================================

       Functions

=============================================================================*/

/**
 * This function sends the GNSS Measurements obtained by ME to
 * SM. PS Measurements are sent out periodically for NMEA
 * sentence generation and for diagnostic reasons (periodicity
 * is set by SM). They are also sent when measurement processing
 * is complete in 1x MS-Assisted mode of operation. The
 * measurement status report indicates the reason for sending
 * the measurements.
 * @param e_MeasRptReason
 * @param meas_ptr
 */
void sm_ReportGnssMeasInfo( gnss_MeasRptReasonEnumType    e_MeasRptReason,
                            const gnss_MeasBlkStructType* meas_ptr);

/**
  @brief   This function sends the GPS Measurements obtained by ME to SM.

  This function sends the Receiver State change event report to
  SM
  
  @return      
  @sideeffects None
  @see         
*/
void sm_ReportRcvrStateChange(
                             uint32 q_GpsRtcMs, 
                             /**< GNSS RTC msec at which this report was generated.  */
                             gps_RcvrStateEnumType e_CurRcvrState 
                             /**< new receiver state */
                             );


/**
  @brief  This function sends the GNSS SV poly report calculated by
          PE to SM. GNSS SV poly reports are sent periodically
          once MGP is turned ON

  @return
  @sideeffects None
  @see

*/
void sm_ReportSVPolynomialGNSS(const gnss_SvPoly *p_GnssSvPolyRpt);

/**
  @brief  This function sends the GPS Fix report calculated by
          PE to SM. GPS FIX/NO-FIX reports are sent periodically
          once MGP is turned ON
  
  @return      
  @sideeffects None
  @see 

*/
void sm_ReportGnssFixInfo(const sm_GnssFixRptStructType *p_GnssFixRpt
                          /**< Pointer to the structure containing the GPS Fix data */);

/**
  @brief  This function sends the GNSS position estimate
          maintained by PE to SM. PE responds using this API
          when SM requests position estimate using
          mgp_ReqPosEstimate.
  
* @return      void
  
  @sideeffects None
  @see sm_ReportGnssFixInfo

*/
void sm_ReportGnssPosEstimate(const sm_GnssFixRptStructType *p_GnssNoFixRpt
                              /**< Pointer to the structure containing the GPS Fix data */);


/*=============================================================================

FUNCTION
  sm_ReportUtcInfo

DESCRIPTION
  This function sends the UTC data decoded by PE off the air to SM.

DEPENDENCIES
  None.

PARAMETERS
  p_UtcInfo - Pointer to the structure containing the UTC info

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportUtcInfo
(
  const sm_UtcInfoRptStructType *p_UtcInfo
);

/**
* @brief  PE indicates to SM the status of current assistance
*         data and wish-list using this API
  
  @return    
  @sideeffects None
  @see 

*/
void sm_ReportGnssAssistDataStatus( const sm_GnssAssistDataStatusStructType *p_AssistDataInfo);

/**
  @brief  ME and PE typically inform SM about events of interest by calling this function.
          SM may call other APIs in response to an event of interest
  
  @return      
  @sideeffects None
  @see         
*/
void sm_ReportEvent(const sm_ReportEventStructType* p_ReportEvent
                          /**< payload denoting event of interest */);

/**
  @brief  This function sends RF Info data from PGI to SM
  
  @return      
  @sideeffects None
  @see
  @todo add more meaningful parameter info
*/
void sm_ReportRfInfo(const sm_ReportRfInfoStructType *p_ReportRfInfo
                     /**< Pointer to the structure containing RF Info data. */);


/**
* @brief  ME calls this function to request SA (assistance) data
*         from the network
  
* @return      void
  @sideeffects None
  @see


*/
void sm_RequestSAData(void);

/*=============================================================================

FUNCTION
  sm_StoreFtcalTTData

DESCRIPTION
  This function sends TT data from MC to SM.

DEPENDENCIES
  None.

PARAMETERS
  cgps_FtcalTTStruct 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_StoreFtcalTTData (cgps_FtcalTTStruct* p_TTData);

/*=============================================================================

FUNCTION
  sm_ReportWBIQInfo

DESCRIPTION
  This function sends Wide Band IQ Info data from MC to SM.

DEPENDENCIES
  None.

PARAMETERS
  p_ReportWBIQInfo - Pointer to the structure containing WBIQ params

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportWBIQInfo (const sm_ReportWBIQInfoStructType * p_ReportWBIQInfo);

/*=============================================================================

FUNCTION
  sm_ReportNBIQInfo

DESCRIPTION
  This function sends Narrow Band IQ Info data from MC to SM.

DEPENDENCIES
  None.

PARAMETERS
  p_ReportNBIQInfo - Pointer to the structure containing WBIQ params

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportNBIQInfo (const sm_ReportNBIQInfoStructType * p_ReportNBIQInfo);


/*=============================================================================

FUNCTION
  sm_ReportGNSSRfStatusInfo

DESCRIPTION
  This function sends GNSS RF status Info data from MC to SM.

DEPENDENCIES
  None.

PARAMETERS
  p_ReportGNSSRfInfo - Pointer to the structure containing GNSS RF status
  information 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportGNSSRfStatusInfo (const sm_ReportGNSSRfStatusInfoStructType * p_ReportGNSSRfInfo);

#ifdef FEATURE_CGPS_UMTS_CELLDB
/*=============================================================================

FUNCTION
  sm_UpdateCellIDInfo

DESCRIPTION
  This function sends the latest Cell ID information to SM. This information
  is maintained in MGP and is updated to SM whenever there is a cell change
  or when new Cell ID is received from RR/RRC.
 
  Latest Cell ID changes will come from Call Manager instead of MGP, but this
  API can be used to inject Cell ID change to SM (like in SMART)

DEPENDENCIES
  None.

PARAMETERS
  e_TriggerRAT - RAT that triggered the cell db update
  p_CellInfoCached - Pointer to the Cached Cell ID information.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_UpdateCellIDInfo( const cgps_ActiveRATEnumType e_TriggerRAT, const cgps_CellInfoCachedStruct * const p_CellInfoCached );
#endif /* FEATURE_CGPS_UMTS_CELLDB */

/**
  @brief  Delivers estimate of current time to SM. PE calls this
          API in response to a request from SM via
          mgp_ReqGnssTime (to be defined by MGP).
  
  @return  
  @sideeffects None
  @see 

*/
void sm_ReportGnssTime(const sm_ReportGnssTimeStructType *p_ReportGnssTime/**< gnss time struct */);


/*===========================================================================
FUNCTION sm_send_message

DESCRIPTION
  Function to send an IPC message to SM threads

RETURN VALUE
 ipc_msg_ptr = If IPC message was send successfully

DEPENDENCIES
  None

=============================================================================*/

os_IpcMsgType *sm_send_message (uint32 msg_id, uint32 size, uint32 thread, const void *data);

/*=============================================================================

FUNCTION
  sm_UpdateGpsState

DESCRIPTION
  This function sends the best available position fix information from engine to SM. 

DEPENDENCIES
  None.

PARAMETERS
  p_PosInfo - Pointer to the best available position fix info.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_UpdateGpsState( const sm_PositionReportStructType* const p_PosInfo );

#ifdef FEATURE_CGPS_XTRA_T
/*=============================================================================

FUNCTION
  sm_ReportCellData

DESCRIPTION
  This function sends cell data from MGP to SM.

DEPENDENCIES
  None.

PARAMETERS
  sm_ReportCellDataStructType - Pointer to the structure containing cell data

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportCellData(const sm_ReportCellDataStructType* const p_ReportCellData);
#endif /* FEATURE_CGPS_XTRA_T */

/*=============================================================================
FUNCTION
  sm_ReportPrescDwellResult

DESCRIPTION
  This function gets called when a presc dwell log is available.

DEPENDENCIES
  None.

PARAMETERS
  p_PrescDwellResult - Pointer to the structure containing presc dwell result

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportPrescDwellResult( const sm_PrescDwellLogType *p_PrescDwellResult );

/*=============================================================================

FUNCTION
  sm_ReportDemodSoftDecisions

DESCRIPTION
  This function gets called when demod soft-decision log is available.

DEPENDENCIES
  None.

PARAMETERS
  p_DemodResult - Pointer to the structure containing demod soft decision data

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportDemodSoftDecisions( const log_gps_demod_soft_decisions_type *p_DemodResult );

/*=============================================================================

FUNCTION
  sm_GetNvGpsUtcOffset

DESCRIPTION
  This function gets UTC offset configured in the NV.

DEPENDENCIES
  None.

PARAMETERS
  p_UtcOffset - Pointer to the UTC offset value

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void  sm_GetNvGpsUtcOffset(uint8 *p_UtcOffset);

/*=============================================================================

FUNCTION
  sm_EnableSensorLPM

DESCRIPTION
  This function turns on sensor low power mode.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void  sm_EnableSensorLPM( void );

/*=============================================================================

FUNCTION
  sm_DisableSensorLPM

DESCRIPTION
  This function turns off sensor low power mode.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void  sm_DisableSensorLPM( void );

/*=============================================================================

FUNCTION
  sm_EnableSensorFPM

DESCRIPTION
  This function turns on sensor low power mode.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void  sm_EnableSensorFPM( void );

/*=============================================================================

FUNCTION
  sm_DisableSensorFPM

DESCRIPTION
  This function turns off sensor low power mode.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void  sm_DisableSensorFPM( void );

/**
* @brief  Indicate to SM that it should request external SPI information
  
  @return    
  @sideeffects None
  @see 

*/
void sm_XspiStartInjection( void );

/**
* @brief  Indicate to SM that it should close the 
          SPI information injection stream
  
  @return    
  @sideeffects None
  @see 

*/
void sm_XspiStopInjection( void );

/*=============================================================================

FUNCTION
  sm_ReportDrSyncPulseDescriptor

DESCRIPTION
  

DEPENDENCIES
  None.

PARAMETERS
  

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportDrSyncPulseDescriptor(gnss_DRSyncPulseDescType* pDrSyncPulseDesc);

/*=============================================================================

FUNCTION
  sm_ReportDrSyncConfig

DESCRIPTION


DEPENDENCIES
  None.

PARAMETERS


RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportDrSyncConfig(gnss_DRSyncConfigType* pDrSyncConfig);

/*=============================================================================

FUNCTION
  sm_ReportDebugNMEA

DESCRIPTION
  Accepts formatted proprietary NMEA string for debug purpose.
  NMEA string is sent as IPC to the LM task by this API

DEPENDENCIES
  None.

PARAMETERS
  sm_DebugNMEA

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportDebugNMEA(sm_DebugNMEA* pDebugNMEA);

/*=============================================================================

FUNCTION
  sm_RegisterApiNotifierCb

DESCRIPTION
  Called by potential clients to register their callbacks. The callbacks are 
  used to notify the clients that the API/Engine is now ready to accept clients

  If the API/Engine is ready to accept clients, the callback will be called 
  inline.  The caller should be ready to handle this and queue an IPC to itself. 

DEPENDENCIES
  None.

PARAMETERS
  pNotifierCb - The callback of the client to be registered

RETURN VALUE
  TRUE:  registration has succeeded, or if Engine is ready the callback is 
         called immediately;
  FALSE:  registration failed

SIDE EFFECTS
  None.

=============================================================================*/
boolean sm_RegisterApiNotifier(sm_ApiInitNotifierCb pNotifierCb, os_ThreadIdEnumType clientThreadId);

/*=============================================================================

FUNCTION
  sm_ReportXtraInjDone

DESCRIPTION
  Indication from CD that xtra injection completed successfully. 

DEPENDENCIES
  None.

PARAMETERS
 None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportXtraInjDone( void );

/*=============================================================================

FUNCTION
  sm_ReportResetLocationServiceDone

DESCRIPTION
  Indication from MGP that location service reset is complete.
  MGP may have succesfully reset or partially reset.
  A status is passed to this function by MGP to let SM know the status of the
  reset location service.

DEPENDENCIES
  None.

PARAMETERS
 status: Status of reset location service.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportResetLocationServiceDone(pdsm_pd_ext_reset_location_service_status_e_type status);


/*=============================================================================
  
FUNCTION

  sm_SendEngineErrorRecoveryReport

DESCRIPTION
  ME will call this API to send engine error recovery report to SM so it'll be
  forwarded to QMI by calling gnss_CQmiSvcSendEngineErrorRecoveryReport()
  No sanity check will be performed for the error recovery report from ME

DEPENDENCIES
  None.

PARAMETERS
 None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_SendEngineErrorRecoveryReport( gnss_engine_err_recovery_rpt *pErrReport );

/*=============================================================================

FUNCTION
  sm_DpoStatusReport

DESCRIPTION
  Function to report periodic DPO status report.
  When DPO status reporting is enabled by the application, this function
  will be called at a nominal rate of 1 Hz. 

DEPENDENCIES
  None.

PARAMETERS
  Information about DPO disengagement from MGP to SM . 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_DpoStatusReport( sm_DpoStatusReportStructType *p_DpoStatusReport );

/*=============================================================================

FUNCTION
  sm_ReportGpsEphemeris

DESCRIPTION
  This function sends a GPS ephemeride from PE to SM.

DEPENDENCIES
  None.

PARAMETERSa
  w_Prn    - PRN of ephemeride being returned
  pz_GpsEph - Pointer to the ephemeris information

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportGpsEphemeris( const uint16 w_Prn, const gps_EphStructType *pz_GpsEph );

/*=============================================================================

FUNCTION
  sm_ReportQzssEphemeris

DESCRIPTION
  This function sends a QZSS ephemeris from PE to SM.

DEPENDENCIES
  None.

PARAMETERSa
  u_Prn    - PRN of ephemeride being returned
  pz_QzssEph - Pointer to the ephemeris information

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
     void sm_ReportQzssEphemeris(const uint8 u_Prn, const gps_EphStructType *pz_QzssEph);


/*=============================================================================

FUNCTION
  sm_ReportGloEphemeris

DESCRIPTION
  This function sends a GLO ephemeride from PE to SM.

DEPENDENCIES
  None.

PARAMETERSa
  w_SlotId  - Slot ID of ephemeride being returned
  pz_GloEph - Pointer to the ephemeris information

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportGloEphemeris( const uint16 w_SlotId, const glo_EphStructType *pz_GloEph );

/*
******************************************************************************
* sm_SvSframePut
*
* Function description:
*
*    This function is called by PE to provide a decoded subframe to PE.
*
* Parameters:
*
*  p_SvSframe -  Pointer to SV subframe structure
*
* Return value:
*
*  void
*
* ******************************************************************************
*/
void sm_SvSubframePut( const gnss_SvSubframeStruct *p_SvSubframe );

/*
******************************************************************************
* sm_SvGalPagePut
*
* Function description:
*
*    This function is called by PE to provide a decoded Galileo Page to PE.
*
* Parameters:
*
*  p_SvSframe -  Pointer to SV GAL Page structure
*
* Return value:
*
*  void
*
* ******************************************************************************
*/
void sm_SvGalPagePut( const gnss_GalPageStruct *p_SvGalPage);

/*
******************************************************************************
* sm_GloStringPut
*
* Function description:
*
*  This function is called by PE to provide a decoded GLONASS string to PE.
*
* Parameters:
*
*  pz_Str -  input, Pointer to a GLONASS string
*
* Return value:
*
*  void
*
* ******************************************************************************
*/
void sm_GloStringPut( const gnss_GloStringStruct* pz_Str );


/*===========================================================================
  sm_loc_lte_ecid_recv_measurements

  Description:

     This function is used by LOC_LTE_ECID module to send the ECID measurements
 Parameters:
    Reason: stop reason

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean sm_loc_lte_ecid_recv_measurements(
   gnss_lpp_ecid_meas_info_type *p_ecid_measurement_info); 


/*=============================================================================

FUNCTION
  sm_ReportBdsEphemeris

DESCRIPTION
  This function sends a BDS ephemeride from PE to SM.

DEPENDENCIES
  None.

PARAMETERSa
  w_Sv      - SV ID of ephemeride being returned
  pz_BdsEph - Pointer to the ephemeris information

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportBdsEphemeris( const uint16 w_Sv, const bds_EphStructType *pz_BdsEph );


/*=============================================================================

FUNCTION
  sm_ReportGalEphemeris

DESCRIPTION
  This function sends a GAL ephemeride from PE to SM.

DEPENDENCIES
  None.

PARAMETERSa
  w_Sv      - SV ID of ephemeride being returned
  pz_GalEph - Pointer to the ephemeris information

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportGalEphemeris( const uint16 w_Sv, const gal_EphStructType *pz_GalEph );

/*===========================================================================
  sm_ReportWifiFixResponse

  Description:

     This function is used by WiLE to provide Wifi fix response to SM
 Parameters:
   sm_GnssFixRptStructType: Pointer to Position Report (with legacy "Gnss" 
                            name.)
                            Note that this may (uncommonly) be non Wifi position
                            if there was a better position available in ALE.
    wifi_ResultFlagsType: Wifi Results Flag
          The result flag indicates any error during Wifi pos fix calculation
          like no modem cache, no measurements, etc. It also indicates 
          whether a better non Wifi fix was sent instead. 
    sm_WifiPosReqSrcType: Wifi Position fix source (originiator)
  Return value: 
   VOID
=============================================================================*/
void sm_ReportWifiFixResponse(const sm_GnssFixRptStructType * pz_PosFixRpt,
                              wl_ResultFlagsType z_wifi_result,
                              wl_PosReqSrcType e_wifi_pos_src);

/*===========================================================================
  sm_InitGnssNavPosition

  Description:

     Initalizes a sm_GnssNavPosition safely

 Parameters:

   pz_Pos:  pointer to the struct to initialize

  Return value: 
    boolean: true if successfully initialized

=============================================================================*/
boolean sm_InitGnssNavPosition(sm_GnssNavPosition * pz_Pos);

#ifdef FEATURE_LOCTECH_NHZ
/*===========================================================================
  sm_NhzConfigPut

  Description:

     Get the NHz config from SM

 Parameters:

   pq_NhzConfig:  pointer to the config

  Return value: 
    None

=============================================================================*/
void sm_NhzConfigPut(uint32 * pq_NhzConfig);

/*===========================================================================
  sm_NhzDefaultConfigPut

  Description:  Get the NHz default config from SM
  
  Parameters :  None

  Return value: Default config

=============================================================================*/
uint32 sm_NhzDefaultConfigPut();
#endif

#endif /* SM_API_H */


