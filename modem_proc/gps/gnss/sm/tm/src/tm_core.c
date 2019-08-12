/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TM Core

GENERAL DESCRIPTION
  This module makes up the TM-Core module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2015, 2016 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/30/16   skm     Added NV for default LPP12 2 timer feature config
07/05/16   rk      Pass all measurements to protocols. It will decide which measurements should be
                   reported to server.
04/27/16   ak      Populate ENU velocity and uncertainity in position report
04/15/16   ld      Added NHz NV support  
03/25/16   pk      During OTA Reset, Call PGI api to reset SIB8/16 Decode
01/20/16   mj      Add a new flag b_e911_session to indicate e911 session in progress
01/05/16   mc      Dynamically allocate variables for huge structures to reduce stack usage
01/04/16   mj      Move changes to check sub id sanity to the call back function
11/25/15   mc      removed debug message in tm_core_is_on_demand_session_on()
01/05/16   mc      Dynamically allocate variables for huge structures to reduce stack usage
01/04/16   mj      Move changes to check sub id sanity to the call back function
11/25/15   mc      removed debug message in tm_core_is_on_demand_session_on()
10/20/15   gk      request CPI for Standalone sessions
09/16/15   rk      Use the pre-defined GLONASS & 4G position method bits in NV 1920 to
			       enabled or advertise GLONASS & 4G position method support respectively.
09/16/15   skm     BDS Health delete fix
08/26/15   rk      Added new OTDOA stop engine request processing support.
08/26/15   muk     TL 1.0 integration
08/05/15   rh      Moved carrier-specific NV 1920 (pos mode support) to TM core 
08/04/15   js      Default Enablement of XTRA download 
07/31/15   mj      Changes to fix E911 on second SUB in G+G config
07/08/15   rh      Changed NV refresh to be done on one specifc sub
07/03/15   jv      Added QZSS Support
06/25/15   gk      Abort SVLTE session when 1x CP starts only when there is a ongoing session
06/22/15   gk      Write leap second change to NV at GPS off if there was a change during the session
06/09/15   ssu      Adding NV for GM filtering of WLAN freq. 
06/09/15   mj      Add support for W+W feature
04/30/15   mj      Add support for early session start for WCDMA for E911
04/16/15   jv      Lowering priority of some messages from MSG_ERROR to MSG_MED
03/14/15   rh      Added support for MultiSIM NV
01/16/15   rh      Fixed missing SVInfo issue
12/18/14   rh      Fixed wrong merge for LPP CP transmission delay EFS NV read
11/27/14   jv      expand PDSM_PA_DELETE_HEALTH to GLO, BDS and GAL too.
11/29/14   ssu     GM LOWI Integration
11/26/14   am      GAL lm delete request initialization was added
03/12/14   gk      Handle OTA reset when session is active. consider as end of the session
11/25/14   gk      NV item for greater than 24 AD 
11/14/14   gk      NV item for terminating SUPL after final fix 
11/13/14   rh      Added configurable LPP CP transmission delay (via EFS NV)
10/25/14   jv      Added Galileo Support
09/10/14   gk      Added delete throttle timer to prevent B-b delete commands
08/25/14   jv      16-bit SVID Support
08/19/14   ah      Copy pointer contents of GPS state info response once received
08/02/14   gk      do not abort early rcvr session with a NI session is started. 
07/21/14   ssu     Support for GM WWAN motion detector
06/07/14   ah      Map Premium Service NV value with correct SAP enablement value
05/20/14   hs      Replacing the the idx type from gnss_ to bcn_ for TDP support
05/20/14   ssu     Removing redundant code. 
05/19/14   gk     E911 NI during MO should be accepted
05/15/14   rh      Fixed Wiper not stopped issue if session is triggered as MSB initially
04/24/14   yy      Change prisoner sv to gnss sv blacklist
04/24/14   rh      Added delete prisoner SV support
04/17/14   skm     NV Async Read changes
04/01/14   ssu    Addressing a few KW errors .
04/01/14   ssu    Addressing a few KW errors .
03/26/14   ah      Added trigger of GTP WWAN session if PUNC>25km for Get Avail WWAN Pos
02/21/14   ssu     Adding NV for configuring motion sensing distance in case of challenging GPS env.
02/20/14   ssu     Adding a NV for modifying the medium responsiveness backoff value and gps challenging env backoff config.
03/10/14   ah      Added support for Set Premium Services Cfg & Get Avail WWAN Pos
02/10/14   ssu     Adding a NV for modifying the rate at which GM requests CPI injection. 
01/31/14   ss      Supporting Max acceptable GF PUNC confguration from QMI
01/27/14   mj      Remove featurization to fix issue where SGLTE code was featured out
01/13/14   rh      Added requesting XTRA download during SUPL feature 
01/10/14   ss      Supporting High Responsiveness Geofencing Config
12/23/13   mj      Remove FEATURE_DUAL_SIM and FEATURE_TRIPLE_SIM
12/02/13   ah      Support GPS State Info requests for multiple clients simultaneously
09/27/13   mj      TSTS support: Define tertiary mmgsdi sessions for triple SIM
09/12/13   mj      Additional check for app_type while retrieving mmgsdi session id
06/26/13   rh      Fixed a bug related to PA Delete bitmask which BDS-only deletion is failing
06/13/13   rh      Used same pdsm_GnssNavSvInfo in SM_API & PDAPI for SV info
06/12/13   ah      Added measurement report support for BDS standalone (not yet for AGNSS)
06/11/13   mj      Report intermediate fixes for protocols when sysd is enabled
06/07/13   rh      Added assistance data deletion support for BDS
06/07/13   rh      Removed diag timer and NO_FIX report handling
03/20/13   mj      Modified callback logging
03/01/13   kj      Move to the DOG Heartbeat mechanism on DIME and beyond
01/31/13   mj      Added new handler tm_core_api_callback_handler and tm_core_api_cb_timer_handler
02/21/13   mj      Adding missed code for GNSS_NV_EFS_SM_CONFIG_PREMIUM_SERVICES NV read support
01/22/13   ah      Support extraction of MPG event from union tm_info_s_type
01/06/13   gk     ECID changes
12/27/12   rk      Update fix report translation function to copy sensor (gyro/accel) 
                   bias report structure. 
12/04/12   rk      CR 408753 - Changes relating to extended SV parameter enhancement in GNSSFixReport.
11/06/12   mj      Fixed issue where NV_CGPS_UTC_GPS_TIME_OFFSET_I was not updated
10/09/12   rh      Added a few error code mappings for IS801 session
09/07/12   mj      Added tm_core_external_cell_db_proc and support for existing cell db proc for SGLTE
09/03/12   rh      Added support for NI message injection
08/01/12   mj      Added handler for GET PARAM request (tm_core_get_param_msg_handler)
07/18/12   mj      Added GNSS_NV_EFS_SM_CONFIG_PREMIUM_SERVICES NV read support
06/11/12   rk      Added GNSS_NV_EFS_SM_TM_ASSISTED_GLO_PROTOCOL_SELECT NV read support 
04/18/12   ssu     Using sm_PositionReportStructType when reporting GPS state from ME. 
03/01/12   rh      Added tm_core_handle_engine_error_recovery_report()
02/16/12   gk      Altitude also reported in float now
02/06/12   gk      LPP enablement via PDAPI
11/23/11   ssu     Adding GFC_QMI feature.
12/13/11   gk      LPP related fixes
11/14/11   gk      Added new variable for PDAPI position inj req event
07/26/11   rb      Added handling for ptr set to NULL in TM_PHONE_EVENT_DSDS_SUBS_CHANGED  
06/03/11   gk      Clear cached position  when delete param or gps lock is sent
06/16/11   rh      Added TM_PHONE_EVENT_OTASP_STATUS_COMMITTED event processing
05/18/11   rc      Lowering priority of some standalone messages from MSG_HIGH to MSG_LOW
05/03/11   gk      cache the positin info from AD request
01/03/11   gk      Eph throttling rework
11/23/10   rh      Added TM feature to send NO_FIX as Intermediate Report
10/05/10   ns      Port CL1333689: Save status of gps state info req by SYSD.
09/22/10   MG      Not Generating gpgga, gprmc when giving out Wi-Fi fixes.
08/23/10   gk      Added new handling for optimal modes
06/06/10   rb      Add tm_core_time_validity to determine if GPS time is valid
06/22/10   gk      Request postiion injection if MSB fails
06/03/10   bp      adding smart MSB support for V1/V2
05/25/10   gk      Add new NV item for XTRA validity
04/07/10   rb      Update UTC Time Offset NV item when needed
04/02/10   gk      Added an event callback to inject position
03/03/10   ns      DSDS Support
01/13/10   gk      end session forces receiver off if OEM client.
01/08/10   gk      add rf loss to c/no for the PDAPI measurement report.
01/06/10   jlp     Changed CGPS_WHOLE_EARTH_HEPE to CGPS_WHOLE_EARTH_CEP99.
12/02/09   gk      intermediate position fixes
11/11/09   gk      set optional flags for final position, also set the uncs correctly
11/10/09   atien   Check for com failure if allowing
                   MSB fallback to Standalone with XTRA download if no XTRA data
10/12/09   gk      Force receiver off
09/30/09   gk      Support ODP 2.0
07/27/09   gk      tm_core_translate_end_status() - translate TM core end session
                   error into PDSM PD End Event error reason
08/11/09   gk      Addition ODP 2.0 changes
07/27/09   gk      tm_core_translate_end_status() - translate TM core end session
                   error into PDSM PD End Event error reason
07/23/09   gk      Support ODP 2.0
07/23/09   gk      Added DELETE_SV_NO_EXIST flag for delete params
07/13/09   gk      process end session command even when no TM session
07/10/09   atien   Spirent requested feature:  allow XTRA dload after MSB dload comm failure
07/09/09   gk      ODP 1.1 related fixes for ready mode
07/08/09   gk      undo the previous change for not reporting >1 intermediate positions
06/10/09   gk      Added Get UTC offset from NV
06/10/09   gk      ODP 1.1 support
06/04/09   gk      other intermediate position changes
05/14/09   gk      fixed gps state info handling
04/16/09   gk      Fill in xtra data correctly in the gps state
04/09/09   ns      REF LR intermediate reported routed to tm_qwip_core.c to check
                   Wifi fix status.
03/27/09   jd      Added a generic abort handling mechanism for SysD
04/02/09   ns      support for sending diag info to sysd.
03/12/09   ns      Init Sysd triggered flag
03/04/09   ns      Added support for passing gps state info to qwip
02/04/09   atien   MO-LR vs AD Call Flow Select support
01/13/09   atien   XTRA-T support
01/25/09   ns      Added support for Qwip
09/15/08   gk      Rename the on-demand struct to sm_ from cgps_
08/26/08   gk      On-Demand GPS Lock Changes
08/20/08   cl      Set op_request flag in session continue request.
08/13/08   gk      Enable force receiver off
08/08/08   cl      Enable non-encrypted lat/long when security is activated
07/30/08   cl      Set elevation/azimuth only when u_SvDirValid is valid
07/11/08   cl      Inform 1x/UMTS modules when switching serving systems
07/02/08   ank     Added LBS support for CDG OMH R-UIM Spec
04/25/08   gk      On-demand changes
04/04/08   cl      Inform 1x_up when entering UMTS system
03/13/08   cl      For standalone pos, not encrypt.
02/06/08   cl      Added accommodating "forced TM termination".
01/22/08   gk      fixed compiler warning that didnt init the vel correctly
01/21/08   cl      Allow overlapping dload and fix (fix and dload) sessions
11/13/07   gk      do not set heading to 0 if speed is 0
10/30/07   cl      Added smart MSB logic
09/14/07    gk     NMEA CONFIG set parameters support
08/14/07   ank     Inform MGP about nonexistent SVs.
09/11/07   cl      Populate ext_data when security is not enabled
08/01/07   rw      Add support for XTRA feature
08/02/07   lt      Added handling of SV-not-exist message.
07/30/07   gk      Add handling for last position
07/26/07   cl      Populate client id for gps start and end events
07/25/07   cl      Add more mapping from interal end status to external end status
07/17/07   lt      L1 interface.
06/27/07   lt      Added the passing of WCDMA in-service to UMTS submodules.
06/18/07   ld      Added support for Deleting Cell DB Information.
06/12/07   cl      Only allow PD API access when TM task is active
05/30/07   lt      Mods. to make TM's RRC and MM iface symmetrical to CM and RR.
05/27/07   rw      Add support for JGPS (KDDI) module
05/23/07   mr      Added call to generate NMEA PSTIS string
05/16/07   mr      Removed NMEA string generation for intermediate fixes
05/16/07   mr      Added calls to generate NMEA string for MSA fix from server
04/24/07   ank     Change to use CGPS_SRV_SYS enum in place of TM_CM_SRV_SYS.
04/11/07   lt      Ready for activating UMTS CGPS protocol modules.
04/04/07   gk/mr   PC Time injection handling
04/03/07   cl      Feed LR to MGP.
02/15/07   cl      Moved pdsm_getbuf in tm_core_lm_sess_info_handler
02/15/07   cl      Add support to inject pre-fix ppm
02/01/07   cl      Add feature to check if CDMA is supported
02/01/07   lt      Added UTC model handling
01/31/07   cl      Added LCS handling
12/04/06   cl      Added NV functionality
12/04/06   cl      pd api event report interface change
10/13/06   cl      Change to use SM_TM for dog report
07/12/06   cl      Initial version

============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gps_variation.h"
#include "comdef.h"     /* Definition for basic types and macros */
#include "customer.h"   /* Customer configuration file */
#include <stdio.h>

#include "msg.h"
#include "event.h"
#include "aries_os_api.h"
#include "aries_ipc_diag_log.h"
#include "sm_nv.h"
#include "sm_log.h"
#include "tm_data.h"    /* Definition of TM task data structure */
#include "tm_common.h"    /* Definition of TM task data structure */
#include "lm_api.h"
#ifdef FEATURE_CGPS_PDCOMM
#include "pd_comms_api.h"
#endif /* FEATURE_CGPS_PDCOMM */
#include "tm_pdapi_client.h"
#include "tm_cm_iface.h"
#include "tm_prtl_iface.h"
#include "tm_standalone.h"
#include "tm_diag.h"
#include <math.h>
#include "time_svc.h"
#include "tm_nv.h"
#include "memory.h"
#include "gps_common.h"
#include "lsmp_api.h"
#include "tm_ruim.h"
#include "sm_util.h"

#ifdef FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH /* this would be defined in custwcdma.h */
#undef FEATURE_WCDMA_GPS_CELL_ID_READING /* this would be defined in custcgps.h */
#endif
   
#ifdef FEATURE_MMGSDI_SESSION_LIB
#include "mmgsdisessionlib.h"
#endif /* FEATURE_MMGSDI_SESSION_LIB */

#ifdef FEATURE_TM_SECURITY
#include "tm_security.h"
#endif /* FEATURE_TM_SECURITY */

#if defined (FEATURE_SUPPORT_CDMA_GPS_PROTOCOLS) && !defined (FEATURE_GNSS_LOW_MEMORY)
#include "tm_1x_cp.h"
#include "tm_1x_up.h"
#endif /* FEATURE_SUPPORT_CDMA_GPS_PROTOCOLS && ! FEATURE_GNSS_LOW_MEMORY */
#include "tm_nmea.h"
#include "pdapibuf.h"
#include "sm_dm_event.h"

#ifdef FEATURE_SUPPORT_UMTS_GPS_PROTOCOLS
#include "tm_umts_common_utils.h"
#ifdef FEATURE_CGPS_UMTS_UP
  #include "tm_umts_up_core_iface.h"
  #ifdef FEATURE_USER_PLANE_MRL
    #include "tm_l1_iface.h"
    #include "l1lsmif.h"
  #endif
#endif
#ifdef FEATURE_CGPS_UMTS_CP
  #include "tm_umts_cp_core_iface.h"
  #include "tm_mm_iface.h"
  #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
  #include "tm_rrc_iface.h"
  #endif
#endif
#endif


#ifdef FEATURE_CGPS_XTRA
#include "tm_xtra.h"
#endif /* FEATURE_CGPS_XTRA */

#include "tm_qwip_core.h"

#ifdef FEATURE_UIM_SUPPORT_LBS
#include "mgp_api.h"
#endif /* FEATURE_UIM_SUPPORT_LBS */

#ifdef FEATURE_CGPS_XTRA_T
#include "tm_xtra_t.h"
#endif /* FEATURE_CGPS_XTRA_T */

#define INV_SQRT_2 0.7071067811865
/* The Punc value threshold for Xtra T download request ( in meters ) */
#define PUNC_THRESHOLD_FOR_XTRA_T 25000

#include "sm_nv_efs.h"
#include "tm_xspi.h"

#include "tm_sim_utils.h"

#ifdef FEATURE_CGPS_XTRA_T
#include "tle_api.h"
#endif

#include "loc_wwan_me_api.h"

#include "gm_api.h"
#include "gfc_qmi.h"
#include "gnss_calendar_util.h"

extern void tm_lpp_tx_mgr_init(uint8 u_client_handle);  
extern void tm_lpp_cp_prtl_iface_init(void);

/*===========================================================================
                 Data Definition
===========================================================================*/
#define TM_CORE_PRTL_CB_ARR_INDEX_STANDALONE         0
#define TM_CORE_PRTL_CB_ARR_INDEX_1X_UP              1
#define TM_CORE_PRTL_CB_ARR_INDEX_1X_CP              2
#define TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP            3
#define TM_CORE_PRTL_CB_ARR_INDEX_UMTS_CP            4
#define TM_CORE_PRTL_CB_ARR_INDEX_DIAG               5
#define TM_CORE_PRTL_CB_ARR_INDEX_PPM                6
#define TM_CORE_PRTL_CB_ARR_INDEX_GFC_QMI            7
#define TM_CORE_PRTL_CB_ARR_INDEX_XTRA_T             8
#define TM_CORE_PRTL_CB_ARR_INDEX_LPP_CP             9
#define TM_CORE_PRTL_CB_ARR_INDEX_E911               10
#define TM_CORE_PRTL_CB_ARR_INDEX_MAX                10 /* Must be set to      */
                                                       /* same value as above */

#define PA_AIDING_DATA_DELETE_MASK  (  PDSM_PA_DELETE_TIME    \
                                     | PDSM_PA_DELETE_POS     \
                                     | PDSM_PA_DELETE_EPH     \
                                     | PDSM_PA_DELETE_ALM     \
                                     | PDSM_PA_DELETE_IONO    \
                                     | PDSM_PA_DELETE_UTC     \
                                     | PDSM_PA_DELETE_HEALTH  \
                                     | PDSM_PA_DELETE_SVDIR   \
                                     | PDSM_PA_DELETE_SVSTEER \
                                     | PDSM_PA_DELETE_SADATA  \
                                     | PDSM_PA_DELETE_RTI     \
                                     | PDSM_PA_DELETE_CELLDB_INFO \
                                     | PDSM_PA_DELETE_ALM_CORR \
                                     | PDSM_PA_DELETE_FREQ_BIAS_EST \
                                     | PDSM_PA_DELETE_SV_NO_EXIST \
                                     | PDSM_PA_DELETE_ALM_CORR_GLO \
                                     | PDSM_PA_DELETE_ALM_GLO \
                                     | PDSM_PA_DELETE_ALM_SBAS \
                                     | PDSM_PA_DELETE_EPH_GLO \
                                     | PDSM_PA_DELETE_EPH_SBAS \
                                     | PDSM_PA_DELETE_SVDIR_GLO \
                                     | PDSM_PA_DELETE_SVDIR_SBAS \
                                     | PDSM_PA_DELETE_SVSTEER_GLO \
                                     | PDSM_PA_DELETE_SVSTEER_SBAS \
                                     | PDSM_PA_DELETE_TIME_GLO \
                                     | PDSM_PA_DELETE_TIME_GPS \
                                     | PDSM_PA_DELETE_TIME_BDS \
                                     | PDSM_PA_DELETE_EPH_BDS \
                                     | PDSM_PA_DELETE_ALM_BDS \
                                     | PDSM_PA_DELETE_SVDIR_BDS \
                                     | PDSM_PA_DELETE_SVSTEER_BDS \
                                     | PDSM_PA_DELETE_ALM_CORR_BDS \
                                     | PDSM_PA_DELETE_IONO_QZSS \
                                     | PDSM_PA_DELETE_EPH_QZSS \
                                     | PDSM_PA_DELETE_ALM_QZSS \
                                     | PDSM_PA_DELETE_SVDIR_QZSS \
                                     | PDSM_PA_DELETE_SVSTEER_QZSS \
                                     | PDSM_PA_DELETE_HEALTH_QZSS \
                                     | PDSM_PA_DELETE_ALM_CORR_QZSS \
                                     | PDSM_PA_DELETE_TIME_GAL \
                                     | PDSM_PA_DELETE_EPH_GAL \
                                     | PDSM_PA_DELETE_ALM_GAL \
                                     | PDSM_PA_DELETE_SVDIR_GAL \
                                     | PDSM_PA_DELETE_SVSTEER_GAL \
                                     | PDSM_PA_DELETE_ALM_CORR_GAL \
                                     | PDSM_PA_DELETE_CLOCK_INFO \
                                     | PDSM_PA_DELETE_HEALTH_GLO\
                                     | PDSM_PA_DELETE_HEALTH_BDS\
                                     )

#define PA_AIDING_DATA_INJECT_MASK  (PDSM_PA_TIME_UNC | PDSM_PA_POS_UNC | PDSM_PA_TIME_OFFSET | PDSM_PA_POS_OFFSET )

#define C_MIN_EPH_HAVE_THRESHOLD 8
#define C_MAX_EPH_NEED_THRESHOLD 1
#define C_MIN_SV_IN_VIEW_THRESHOLD 6

/* for GLONASS, use the same ratio as used by GPS, adj. threshold with the total number   
   of SVs in GLONASS being 24*/
#define C_MIN_GLO_EPH_HAVE_THRESHOLD 6
#define C_MAX_GLO_EPH_NEED_THRESHOLD 1
#define C_MIN_GLO_SV_IN_VIEW_THRESHOLD 5

/* using 75% of the entire constellation, same percentage as   
  C_COMPLETE_CONSTALLATION_ALM_THRESHOLD to GPS */
#define C_COMPLETE_CONSTALLATION_GLO_ALM_THRESHOLD 18 

#define TM_CORE_MAX_NUM_API_CALLBACKS (10)
#define TM_CORE_API_CALLBACK_INTERVAL_MS (1000)

#define TM_CORE_CLIENT_BITMASK(x) (1 << (x))

/* This is a global variable which stores all the information of TM-core */
tm_core_param_s_type     tm_core_info;

/* EFS cert can be updated anytime to program SUPL CERT for a new subscription.
   This boolean will force an EFS read of SUPL cert if PDAPI wrote new SUPL CERT into EFS.
*/
boolean  tm_use_efs_for_supl_cert = TRUE;

/* Store current DD Subscription ID */
sys_modem_as_id_e_type      tm_current_dd_subs_id =  SYS_MODEM_AS_ID_NONE;

#ifdef FEATURE_MMGSDI_SESSION_LIB
/* Table to store MMGSDI session info for each card */
static tm_mmgsdi_session_id_table_entry_s_type tm_mmgsdi_session_id_table[DSDS_MAX_SESS_TYPES] =
{ {MMGSDI_GW_PROV_PRI_SESSION},
  {MMGSDI_1X_PROV_PRI_SESSION},
  {MMGSDI_GW_PROV_SEC_SESSION},
  {MMGSDI_1X_PROV_SEC_SESSION},
  {MMGSDI_GW_PROV_TER_SESSION},
  {MMGSDI_1X_PROV_TER_SESSION},
};
#endif /*FEATURE_MMGSDI_SESSION_LIB*/

/* Define nominal Time-Between-Fixes to be 1000ms
   Most applications use this. 
   For NHz operation, the TBF will be less than 1000ms.
   If NHz mode is not enable, default the TBF to 1000ms
*/
#define TM_CORE_NOMINAL_TBF_MS (1000)

/* NV buffer, used to read NV items */
static nv_item_type      tm_nv_read_buffer;

static tm_core_cached_pos_s_type z_cached_pos_info;
static tm_core_cached_meas_s_type z_cached_meas_info;
static tm_core_cached_meas_s_type z_cached_meas_info_nmea;
static tm_core_cached_ref_pos_s_type z_cached_ref_pos_info;

/* Counter for EVENT_GPS_PD_FIX_START */
uint16 tm_core_event_counter = 0;

static const uint8 u_DaysInMonth[]   = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static const uint8 u_DaysInMonthLeap[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static void  tm_core_xlate_gps_state_to_ext_status(tm_core_gps_state_info_s_type *p_gps_state_info,
  pdsm_pd_ext_gps_state_s_type *pd_gps_state_info);
static void tm_core_xlate_gps_state_to_on_demand_info(tm_core_gps_state_info_s_type *p_gps_state_info,
                                            sm_OnDemandPosPuncTuncStruct *p_on_demand_info);
static void tm_core_xlate_pd_ext_pos_to_qmi( const sm_GnssFixRptStructType *p_GnssFixRpt,
                                             pdsm_pd_ext_pos_rpt_s_type    *p_PdsmExtPos);

static float tm_core_calculate_2d_punc(const sm_GnssFixRptStructType *pz_GnssFixRpt);

/*Boolean to track whether reset location service is in progress*/
static boolean tm_reset_location_service_in_progress = FALSE;

static boolean tm_core_sess_req_continue_for_emergency_session(tm_prtl_type                       prtl_type,
                                  tm_sess_handle_type                sess_handle,
                                  tm_sess_req_start_param_s_type  sess_continue_param);

#define TM_MAX_PUNC_VALUE_FOR_POS_INJ_REQ 10000 /* 10km */

static uint8 tm_cb_timer_counter = 0;
static uint8 tm_call_back_log[GPS_MAX_CALLBACK_CLIENTS];
static boolean b_nv_gps_utc_offset_changed = FALSE;

/* MACROS for NMEA output */
#define TM_CORE_OUTPUT_NMEA_1(bit, str, meas_rpt) \
          if(tm_core_info.config_info.subs_cfgs[tm_core_info.config_info.active_subs_cfg].nmea_output_format & (bit)) \
          { \
            tm_nmea_##str##_put(meas_rpt); \
          }
#define TM_CORE_OUTPUT_NMEA_2(bit, str, fix_rpt, is_no_fix) \
          if(tm_core_info.config_info.subs_cfgs[tm_core_info.config_info.active_subs_cfg].nmea_output_format & (bit)) \
          { \
            tm_nmea_##str##_put(fix_rpt, is_no_fix); \
          }

#define TM_CORE_OUTPUT_NMEA_GPGSV_TRIGGER(bit,meas_rpt) \
          if(tm_core_info.config_info.subs_cfgs[tm_core_info.config_info.active_subs_cfg].nmea_output_format & (bit)) \
          { \
            tm_core_trigger_gpgsv(&z_cached_meas_info_nmea, meas_rpt); \
          }

#define TM_CORE_OUTPUT_NMEA_GPGSV(meas_rpt)  TM_CORE_OUTPUT_NMEA_GPGSV_TRIGGER(PDAPI_NMEA_GPGSV, meas_rpt)
#define TM_CORE_OUTPUT_NMEA_GLGSV(meas_rpt)  TM_CORE_OUTPUT_NMEA_1(PDAPI_NMEA_GLGSV, glgsv, meas_rpt)
#define TM_CORE_OUTPUT_NMEA_GAGSV(meas_rpt)  TM_CORE_OUTPUT_NMEA_1(PDAPI_NMEA_GAGSV, gagsv, meas_rpt)
#define TM_CORE_OUTPUT_NMEA_PQGSV(meas_rpt)  TM_CORE_OUTPUT_NMEA_1(PDAPI_NMEA_PQGSV, pqgsv, meas_rpt)
#define TM_CORE_OUTPUT_NMEA_GPGGA(fix_rpt, is_no_fix) TM_CORE_OUTPUT_NMEA_2(PDAPI_NMEA_GPGGA, gpgga, fix_rpt, is_no_fix)


#ifdef FEATURE_CGPS_LBS_TASK
#define TM_CORE_OUTPUT_NMEA_PQXFI(fix_rpt, is_no_fix) TM_CORE_OUTPUT_NMEA_2(PDAPI_NMEA_PQXFI, pqxfi, fix_rpt, is_no_fix)
#else
#define TM_CORE_OUTPUT_NMEA_PQXFI(fix_rpt, is_no_fix)
#endif /* FEATURE_CGPS_LBS_TASK */

#define TM_CORE_OUTPUT_NMEA_GNGNS(fix_rpt, is_no_fix) TM_CORE_OUTPUT_NMEA_2(PDAPI_NMEA_GNGNS, gngns, fix_rpt, is_no_fix)
#define TM_CORE_OUTPUT_NMEA_GPVTG(fix_rpt) TM_CORE_OUTPUT_NMEA_1(PDAPI_NMEA_GPVTG, gpvtg, fix_rpt)
#define TM_CORE_OUTPUT_NMEA_GPRMC(fix_rpt) TM_CORE_OUTPUT_NMEA_1(PDAPI_NMEA_GPRMC, gprmc, fix_rpt)
#define TM_CORE_OUTPUT_NMEA_GPGSA(fix_rpt) TM_CORE_OUTPUT_NMEA_1(PDAPI_NMEA_GPGSA, gpgsa, fix_rpt)
#define TM_CORE_OUTPUT_NMEA_GNGSA(fix_rpt) TM_CORE_OUTPUT_NMEA_1(PDAPI_NMEA_GNGSA, gngsa, fix_rpt)
#define TM_CORE_OUTPUT_NMEA_PQGSA(fix_rpt) TM_CORE_OUTPUT_NMEA_1(PDAPI_NMEA_PQGSA, pqgsa, fix_rpt)

static boolean b_otdoa_sess_start_sent = FALSE; 

/*===========================================================================

FUNCTION tm_core_trigger_gpgsv

DESCRIPTION
  This function is used to send GPGSV with GPS&SBAS information

  GPS and SBAS information is received in separate measurement reports, which
  may arrive in any order. Because of this  the first measurement needs to 
  be cached. When the missing part or last measurement in the set arrives GPGSV
  is sent out and cache is cleared.
 
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

static void tm_core_trigger_gpgsv(tm_core_cached_meas_s_type * p_meas_cache, const sm_GpsMeasRptStructType * p_current_meas)
{
  boolean v_force_gpgsv_send = FALSE;
  const sm_GpsMeasRptStructType *p_gps = NULL;
  const sm_GpsMeasRptStructType *p_sbas = NULL;
  uint32 q_current_meas_blk_src;

  if( (p_meas_cache == NULL) || (p_current_meas == NULL) )
  {
    MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL parameters", 0); 
    return;
  }
  
  /* check if current measurement is the last in the set*/
  if(p_current_meas->z_MeasBlk.u_SeqNum == p_current_meas->z_MeasBlk.u_MaxMessageNum)
  {
     v_force_gpgsv_send = TRUE; /* GPGSV should be sent with all available data*/
     MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Force GPGSV(%u/%u)",
            p_current_meas->z_MeasBlk.u_SeqNum, p_current_meas->z_MeasBlk.u_MaxMessageNum); 
  }

  q_current_meas_blk_src = p_current_meas->z_MeasBlk.e_MeasBlkSrc;

  /* Determine the type of current measurement */
  if(GNSS_MEAS_BLK_SRC_GPS == q_current_meas_blk_src)
  {
    p_gps = p_current_meas;
    MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Received GPS Meas Block SRC=%u", q_current_meas_blk_src); 
  }
  else if(GNSS_MEAS_BLK_SRC_QZSS_SBAS == q_current_meas_blk_src)
  {
    p_sbas = p_current_meas;
    MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Received QZSS Meas Block SRC=%u", q_current_meas_blk_src); 
  }
  else
  {
     MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Current Meas Block SRC=%u", q_current_meas_blk_src); 
  }

  if( (NULL == p_sbas) && (NULL == p_gps) && (FALSE == v_force_gpgsv_send) )
  {
  /* current measurement block is not neither sbas/qzss or gps and there's nothing to send*/ 
     MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Nothing to send", q_current_meas_blk_src); 
     return;
  }

  /* Get measurement from cache, if valid*/
  if(TRUE == p_meas_cache->cached_meas_valid)
  {  
    if((GNSS_MEAS_BLK_SRC_GPS == p_meas_cache->z_meas.z_MeasBlk.e_MeasBlkSrc) && (NULL == p_gps) )
    {
      p_gps = &(p_meas_cache->z_meas);
      MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Got GPS Meas Block from cache %d", p_meas_cache->z_meas.z_MeasBlk.e_MeasBlkSrc); 
    }
    else if((GNSS_MEAS_BLK_SRC_QZSS_SBAS == p_meas_cache->z_meas.z_MeasBlk.e_MeasBlkSrc) && (NULL == p_sbas))
    {
      p_sbas = &(p_meas_cache->z_meas);
      MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Got QZSS/SBAS Meas Block from cache %d", p_meas_cache->z_meas.z_MeasBlk.e_MeasBlkSrc); 
    }
    else
    {
      /* should not be possible*/
      MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Got Invalid Meas Block from cache %d", p_meas_cache->z_meas.z_MeasBlk.e_MeasBlkSrc); 
    }
  }
  else
  {
   /* nothing in cache*/
    if((GNSS_MEAS_BLK_SRC_QZSS_SBAS == q_current_meas_blk_src) ||
       (GNSS_MEAS_BLK_SRC_GPS == q_current_meas_blk_src))
    {
      /*store current  SBAS/QZSS or GPS measurement block to cache*/

      memscpy( (void*)&(p_meas_cache->z_meas) , sizeof(p_meas_cache->z_meas),
               (void*)p_current_meas, sizeof(sm_GpsMeasRptStructType));

      p_meas_cache->cached_meas_valid = TRUE;
      MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Stored Meas Block (%d) into cache", q_current_meas_blk_src); 
    }
  }

  /*Send GPGSV if both GPS and  SBAS measurements are available or current measurement is the last in the set*/
  if( ( (NULL != p_gps) && (NULL != p_sbas) ) ||
      ( (TRUE == v_force_gpgsv_send) && ( (NULL != p_gps) || (NULL != p_sbas) ) )
    )
  {
    sm_GpsMeasRptStructType sbas_copy; 

    if(NULL != p_sbas)
    {
      /* make copy of sbas/qzss data, as tm_util_clean_qzss_sbas_meas_rpt changes the input struct*/
      memscpy((void*)&sbas_copy,sizeof(sbas_copy),(void*)p_sbas,sizeof(sm_GpsMeasRptStructType));
      tm_util_clean_qzss_sbas_meas_rpt(&sbas_copy,TRUE);
      /* update pointer to copy*/
      p_sbas = &sbas_copy;
    }
   
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Sending GPGSV & Clearing measurement cache"); 
    
    tm_nmea_gpgsv_put(p_gps,p_sbas);
    /*There's no use for cached data anymore*/
    p_meas_cache->cached_meas_valid = FALSE;
  }

}

/*===========================================================================

FUNCTION tm_core_update_session_state

DESCRIPTION
  This function determines if a state transition request is valid and
  set tm session state accordingly.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_update_session_state
(
  tm_core_session_state_e_type new_state
)
{
  if(new_state != tm_core_info.session_info.session_state)
  {
    MSG_MED("tm_core state transition [%d] -> [%d] for handle [%d]",
            tm_core_info.session_info.session_state,
            new_state,
            tm_core_info.session_info.active_sess_handle);

    tm_core_info.session_info.session_state = new_state;
  }
}

/*===========================================================================

FUNCTION tm_core_read_carrier_specific_nv

DESCRIPTION
  This function read non-EFS NV items which may be carrier-specific
  (at bootup time or during SIM hot swap)
  Will read from hardware and may save to NV cache(if available)
  
  It will refresh NV for a specific sub

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static void tm_core_read_carrier_specific_nv(sys_modem_as_id_e_type sub)
{
  if ( (sub < SM_CONFIG_MIN_SUB_ID) || (sub > SM_CONFIG_MAX_SUB_ID) )
  {
    MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Read CS NV invalid sub %d", sub); 
    return;
  }

  /* GPS lock (NV #452 GPS LOCK) */
  (void)sm_nv_read_from_sub(NV_GPS1_LOCK_I, (void *)&tm_nv_read_buffer, sub);
  tm_core_info.config_info.subs_cfgs[sub].gps_lock = (pdsm_gps_lock_e_type)tm_nv_read_buffer.gps1_lock;

  /* GPS NMEA String Format (NV #6789  GNSS NMEA Sentence Type ID) */
  (void)sm_nv_read_from_sub(NV_GNSS_NMEA_SENTENCE_TYPE_I, (void *)&tm_nv_read_buffer, sub);
  tm_core_info.config_info.subs_cfgs[sub].nmea_output_format = (pdsm_pa_nmea_type)tm_nv_read_buffer.gnss_nmea_sentence_type;

  /* Glonass NMEA Extended String Format (NV #6790 GNSS NMEA Extended Sentence Type ID) */
  (void)sm_nv_read_from_sub(NV_GNSS_NMEA_EXTENDED_SENTENCE_TYPE_I, (void *)&tm_nv_read_buffer, sub);
  tm_core_info.config_info.subs_cfgs[sub].nmea_output_format |= ((pdsm_pa_nmea_type)tm_nv_read_buffer.gnss_nmea_extended_sentence_type) << 6; 

  /* Positioning Modes Supported (NV 1920)  */
  (void)sm_nv_read_from_sub(NV_AAGPS_POSITIONING_MODES_SUPPORTED_I, (void *)&tm_nv_read_buffer, sub);
  tm_core_info.config_info.subs_cfgs[sub].positioning_modes_supported = tm_nv_read_buffer.aagps_positioning_modes_supported;
}

/*===========================================================================

FUNCTION tm_core_read_carrier_specific_efs_nv

DESCRIPTION
  This function read EFS NV items which may be carrier-specific
  (at bootup time or during SIM hot swap)
  Will read from hardware and may save to EFS NV cache(if available)
  
  It will refresh NV for a specific sub.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static void tm_core_read_carrier_specific_efs_nv(sys_modem_as_id_e_type sub)
{
  if ( (sub < SM_CONFIG_MIN_SUB_ID) || (sub > SM_CONFIG_MAX_SUB_ID) )
  {
    MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Read CS EFS NV invalid sub %d", sub); 
    return;
  }

  /* NV #65811 GNSS 1X UP SUPL Enable */
  if ( sm_nv_efs_reg_item_read_from_sub(GNSS_NV_EFS_SM_TM_1X_UP_SUPL_ENABLE,
         sizeof(uint8), &tm_core_info.config_info.subs_cfgs[sub].gnss_1x_up_supl_enable, sub ) == FALSE)
  {
    MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Read EFS NV 1x UP failed (sub=%d)", sub); 

    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.subs_cfgs[sub].gnss_1x_up_supl_enable = GNSS_NV_EFS_SM_TM_1X_UP_SUPL_ENABLE_DEFAULT;
  }

  /* NV #067225 Assisted GPS LPP Positioning Protocol Select */
  if ( sm_nv_efs_reg_item_read_from_sub(GNSS_NV_EFS_SM_TM_LPP_ENABLE,
         sizeof(uint8), &tm_core_info.config_info.subs_cfgs[sub].gnss_lte_lpp_enable, sub ) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.subs_cfgs[sub].gnss_lte_lpp_enable = GNSS_NV_EFS_SM_TM_LPP_ENABLE_DEFAULT;
  }

  /* GNSS AGPS RAT Preference (NV #67217 GNSS AGPS RAT Preference) */
  if ( sm_nv_efs_reg_item_read_from_sub(GNSS_NV_EFS_SM_AGPS_RAT_PREF_CONFIG,
         sizeof(cgps_nv_agps_rat_pref_config), &tm_core_info.config_info.subs_cfgs[sub].agps_rat_pref,
           sub ) == FALSE)
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.subs_cfgs[sub].agps_rat_pref.umts_lte_prefered = GNSS_NV_EFS_SM_AGPS_RAT_PREF_UMTS_LTE_PREFERED;
    tm_core_info.config_info.subs_cfgs[sub].agps_rat_pref.agps_rat_enable_bit_mask = GNSS_NV_EFS_SM_AGPS_RAT_ENABLE_BIT_MASK;
  }

  /* NV #70192  Assisted GLONASS Positioning Protocol Select */
  if ( sm_nv_efs_reg_item_read_from_sub(GNSS_NV_EFS_SM_TM_ASSISTED_GLO_PROTOCOL_SELECT,
         sizeof(uint32), &tm_core_info.config_info.subs_cfgs[sub].asst_glonass_pos_protocol,
           sub ) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.subs_cfgs[sub].asst_glonass_pos_protocol = GNSS_NV_EFS_SM_TM_ASST_GLO_PROTOCOL_DEFAULT; 
  }

  /* for debugging purpose */
  MSG_3( MSG_SSID_GPSSM, MSG_LEGACY_MED, "Sub%d: 1xUP=%d LPP=%d", sub, 
    (int32)tm_core_info.config_info.subs_cfgs[sub].gnss_1x_up_supl_enable,
    (int32)tm_core_info.config_info.subs_cfgs[sub].gnss_lte_lpp_enable);

  MSG_3( MSG_SSID_GPSSM, MSG_LEGACY_MED, "   GLO=%d RAT=0x%x(LTE=%d)", 
    (int32)tm_core_info.config_info.subs_cfgs[sub].asst_glonass_pos_protocol,
    (uint32)tm_core_info.config_info.subs_cfgs[sub].agps_rat_pref.agps_rat_enable_bit_mask,
    (int32)tm_core_info.config_info.subs_cfgs[sub].agps_rat_pref.umts_lte_prefered);

  MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_MED, "   UDP=%d Emergency=0x%x", 
    (int32)tm_core_info.config_info.subs_cfgs[sub].gnss_sm_udp_allow,
    (uint32)tm_core_info.config_info.subs_cfgs[sub].gnss_sm_emergency_support_config);
  
}

/*===========================================================================

FUNCTION tm_core_read_nv

DESCRIPTION
  This function read NV items to set TM-Core configuration info structure.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_read_nv(void)
{
  uint32 j,k;
  sys_modem_as_id_e_type sub;

  /***************************************************************************
  * Non-EFS NV Items
  ***************************************************************************/
  sm_nv_init();
  
  for (sub = SM_CONFIG_MIN_SUB_ID; sub <= tm_core_info.config_info.max_valid_subs_id; sub++)
  {
    /* read carrier-specific NVs */
    tm_core_read_carrier_specific_nv(sub);
  }

  /* used by UMTS to determine CP/UP in an MO session */
  (void)sm_nv_read(NV_CGPS_MO_METHOD_I, (void *)&tm_nv_read_buffer);
  tm_core_info.config_info.u_mo_method = (uint32)tm_nv_read_buffer.cgps_mo_method;

  /* Glonass support */
  (void)sm_nv_read(NV_GNSS_GLO_CONTROL_I, (void *)&tm_nv_read_buffer);
  tm_core_info.config_info.gnss_glo_control = tm_nv_read_buffer.gnss_glo_control;

  /* NMEA string reporting type */
  (void)sm_nv_read(NV_CGPS_NMEA_CONFIG_INFO_I, (void *)&tm_nv_read_buffer);
  tm_core_info.config_info.nmea_config_type.pd_nmea_port_type = (pdsm_pa_nmea_port_e_type)tm_nv_read_buffer.cgps_nmea_config_info.nmea_port_type;

  tm_core_info.config_info.nmea_config_type.pd_nmea_reporting_type =  (pdsm_pa_nmea_reporting_e_type)tm_nv_read_buffer.cgps_nmea_config_info.nmea_reporting_type;

  /* Smart MSB */
  (void)sm_nv_read(NV_GPS1_DYNAMIC_MODE_I, (void *)&tm_nv_read_buffer);
  tm_core_info.config_info.smart_msb = (boolean)tm_nv_read_buffer.gps1_dynamic_mode;

  (void)sm_nv_read(NV_CGPS_ON_DEMAND_ENABLED_I, (void *)&tm_nv_read_buffer);
  tm_core_info.config_info.on_demand_lpm = (boolean)tm_nv_read_buffer.cgps_on_demand_enabled;
  
  #ifdef FEATURE_CGPS_ODP_2_0
  (void)sm_nv_read(NV_CGPS_USE_QWIP_FOR_ODP_I, (void *)&tm_nv_read_buffer);
  tm_core_info.config_info.use_qwip_for_odp = (boolean)tm_nv_read_buffer.cgps_use_qwip_for_odp;
  #else
  tm_core_info.config_info.use_qwip_for_odp = FALSE;
  #endif
  
  (void)sm_nv_read(NV_CGPS_UTC_GPS_TIME_OFFSET_I, (void *)&tm_nv_read_buffer);
  tm_core_info.config_info.nv_gps_utc_offset = tm_nv_read_buffer.cgps_utc_gps_time_offset;

  /* MOLR vs AD Call Flow Select */
  tm_core_info.config_info.call_flow_select_mask = 0;

  (void)sm_nv_read(NV_AAGPS_2G_MO_LRSUPPORT_I, (void *)&tm_nv_read_buffer);
  if ( tm_nv_read_buffer.aagps_2g_mo_lrsupport == NV_AAGPS_3G_MO_LRSUPPORT_V )
  {
    tm_core_info.config_info.call_flow_select_mask |= PDSM_PA_CALL_FLOW_SELECT_2G;
  }

  (void)sm_nv_read(NV_AAGPS_3G_MO_LRSUPPORT_I, (void *)&tm_nv_read_buffer);
  if ( tm_nv_read_buffer.aagps_3g_mo_lrsupport == NV_AAGPS_3G_MO_LRSUPPORT_V )
  {
    tm_core_info.config_info.call_flow_select_mask |= PDSM_PA_CALL_FLOW_SELECT_3G;
  }

#if 0
  /* disable till nvitems.h is updated */
  /* 1xUP MSA Disable */
  (void)sm_nv_read(NV_GNSS_1XUP_MSA_TRUSTED_MODE_DISABLE_I, (void *)&tm_nv_read_buffer);
  tm_core_info.config_info.up_1x_msa_disable = (boolean) tm_nv_read_buffer.gnss_1xup_msa_trusted_mode_disable;
#endif
  tm_core_info.config_info.up_1x_msa_disable = FALSE;

  (void)sm_nv_read(NV_AAGPS_EMERGENCY_SERVICES_SPPRT_I, (void *)&tm_nv_read_buffer);
  tm_core_info.config_info.aagps_emergency_services_spprt = tm_nv_read_buffer.aagps_emergency_services_spprt;
  /* initialize the new NV items that use EFS here */

  (void)sm_nv_read(NV_AAGPS_DEVELOPMENT_TEST_CONTROL5_I, (void *)&tm_nv_read_buffer);
  tm_core_info.config_info.aagps_development_test_control5 = (uint32)tm_nv_read_buffer.aagps_development_test_control5;

  /* OEM feature mask (NV 7165 32 bits) will be read by LM & TM */
  tm_core_info.config_info.accept_1xcp_mt_over_mo = FALSE;
  (void)sm_nv_read(NV_GNSS_OEM_FEATURE_MASK_I, (void *)&tm_nv_read_buffer);
  if ( tm_nv_read_buffer.gnss_oem_feature_mask & NV_GNSS_OEM_FEATURE_MASK_3_1XCPMT )
  {
    tm_core_info.config_info.accept_1xcp_mt_over_mo = TRUE; /* will accept 1X CP MT even if MO is active */
  }
  if ( tm_nv_read_buffer.gnss_oem_feature_mask & NV_GNSS_OEM_FEATURE_MASK_6_SUPLINIT_NOTIFY )
  {
    tm_core_info.config_info.b_supl_notify_to_ap = TRUE; /* will accept SUPL INIT even if GPS is locked */
  }

  if ( tm_nv_read_buffer.gnss_oem_feature_mask & NV_GNSS_OEM_FEATURE_MASK_7_AUTO_ACCEPT_MTLR_IN_EMERGENCY )
  {
    tm_core_info.config_info.b_auto_accept_mtlr = TRUE; /* will accept MTLR during emergency even if GPS is locked */
  }

  if ( tm_nv_read_buffer.gnss_oem_feature_mask & NV_GNSS_OEM_FEATURE_MASK_9_KILL_PERIODIC_AT_EMERG_END )
  {
    tm_core_info.config_info.b_end_lpp_periodic = TRUE; /* will end LPP CP periodic session after emergency end */
  }

  if ( (tm_nv_read_buffer.gnss_oem_feature_mask &
           NV_GNSS_OEM_FEATURE_MASK_11_DISABLE_GF_OEM_CONF) == 0 )
  {
    tm_core_info.config_info.gm_config.sm_gm_oem_control_mask = TRUE;
  }
  else
  {
    tm_core_info.config_info.gm_config.sm_gm_oem_control_mask = FALSE;
  }

  if (tm_nv_read_buffer.gnss_oem_feature_mask & NV_GNSS_OEM_FEATURE_MASK_13_END_SUPL_AFTER_FINAL_FIX)
  {
    tm_core_info.config_info.b_end_supl_after_final_fix = TRUE; /* will end SUPL session after final fix is reported to AP in MO case */
  }

  tm_core_info.config_info.q_gnss_oem_feature_mask = (uint32)tm_nv_read_buffer.gnss_oem_feature_mask;

  /***************************************************************************
  * EFS NV Items
  ***************************************************************************/
  sm_nv_efs_init();
  
  for (sub = SM_CONFIG_MIN_SUB_ID; sub <= tm_core_info.config_info.max_valid_subs_id; sub++)
  {
    /* read carrier-specific EFS NVs */
    tm_core_read_carrier_specific_efs_nv(sub);
  }
  
  if ( sm_nv_efs_reg_item_read(CGPS_NV_EFS_SM_XTRA_PREFERRED_MAX_VALID_AGE,
                               sizeof(uint16),
                               &tm_core_info.config_info.preferred_xtra_age) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.preferred_xtra_age = CGPS_NV_EFS_SM_XTRA_PREFERRED_MAX_VALID_AGE_DEFAULT;
  }

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_SEND_NO_FIX_REPORT,
                               sizeof(uint8), &tm_core_info.config_info.send_no_fix_report) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.send_no_fix_report = GNSS_NV_EFS_SM_SEND_NO_FIX_REPORT_DEFAULT;
  }

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_CONFIG_QMI_SERVICE_EXTERNAL,
                               sizeof(uint32), &tm_core_info.config_info.qmi_cfg.enableMask) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.qmi_cfg.enableMask = GNSS_NV_EFS_CONFIG_QMI_SERVICE_EXTERNAL_DEFAULT;
  }

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_CONFIG_PREMIUM_SERVICES,
                               sizeof(uint32), &tm_core_info.config_info.premium_services_config) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.premium_services_config = GNSS_NV_EFS_SM_CONFIG_PREMIUM_SERVICES_ENABLE_DEFAULT;
  }

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_TM_LPP_CP_TRANSPORT_TIMEOUT_SECS,
                               sizeof(uint8), &tm_core_info.config_info.gnss_lpp_cp_timeout_secs) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gnss_lpp_cp_timeout_secs = GNSS_NV_EFS_SM_TM_LPP_CP_TRANSPORT_TIMEOUT_SECS_DEFAULT;
  }

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_TM_EMERGENCY_PROTOCOL_SELECT,
                               sizeof(uint32), &tm_core_info.config_info.gnss_emergency_protocol_non_lte) == FALSE )
  {
    /* if returned false for any reason, force it to default value which is CP*/
    tm_core_info.config_info.gnss_emergency_protocol_non_lte = 0;
  }

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_GNSS_UNAVAIL_IND_TIMEOUT,
                               sizeof(uint32), &tm_core_info.config_info.gm_config.sm_gm_gnss_unavail_ind_timeout) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.sm_gm_gnss_unavail_ind_timeout = GNSS_NV_EFS_SM_GM_GNSS_UNAVAIL_IND_TIMEOUT_DEFAULT;
  }

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_MOTION_DETECTION_SOURCES,
                               sizeof(uint32), &tm_core_info.config_info.gm_config.sm_gm_motion_detection_sources) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.sm_gm_motion_detection_sources = GNSS_NV_EFS_SM_GM_MOTION_DETECTION_SOURCES_DEFAULT;
  }

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_POSITION_SOURCES,
                               sizeof(uint32), &tm_core_info.config_info.gm_config.sm_gm_position_sources) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.sm_gm_position_sources = GNSS_NV_EFS_SM_GM_POSITION_SOURCES_DEFAULT;
  }

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_GNSS_POS_QOS_SESS_TIMEOUT,
                               sizeof(uint8), &tm_core_info.config_info.gm_config.sm_gm_gnss_pos_qos_sess_timeout) == FALSE )
  {
    /* if returned false for any reason, force it to default value which is GNSS_NV_EFS_SM_GM_GNSS_POS_QOS_SESS_TIMEOUT_DEFAULT*/
    tm_core_info.config_info.gm_config.sm_gm_gnss_pos_qos_sess_timeout = GNSS_NV_EFS_SM_GM_GNSS_POS_QOS_SESS_TIMEOUT_DEFAULT;
  }

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_TM_WIFI_WAIT_TIMEOUT_SEC,
                               sizeof(uint8), &tm_core_info.config_info.wifi_wait_timeout_sec) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.wifi_wait_timeout_sec = GNSS_NV_EFS_SM_TM_WIFI_WAIT_TIMEOUT_SEC_DEFAULT;
  }
  else if ( tm_core_info.config_info.wifi_wait_timeout_sec > TM_WIFI_WAIT_TIMEOUT_SEC_MAX )
  {
    tm_core_info.config_info.wifi_wait_timeout_sec = TM_WIFI_WAIT_TIMEOUT_SEC_MAX;
  }

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_GNSS_MAX_POS_UNC_ACCEPTED,
                               sizeof(uint32), &tm_core_info.config_info.gm_config.sm_gm_gnss_max_pos_unc_accepted) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.sm_gm_gnss_max_pos_unc_accepted = GNSS_NV_EFS_SM_GM_GNSS_MAX_POS_UNC_ACCEPTED_DEFAULT;
  }

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_USE_NW_ASST_FIXES,
                               sizeof(uint8), &tm_core_info.config_info.gm_config.sm_gm_use_nw_asst_fixes) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.sm_gm_use_nw_asst_fixes = GNSS_NV_EFS_SM_GM_USE_NW_ASST_FIXES_DEFAULT;
  }

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_HIGH_RESPONSIVENESS_CONFIG,
                               sizeof(uint32), &tm_core_info.config_info.gm_config.sm_gm_high_responsiveness_config) == FALSE )
  {
    /* if returned false for any reason, force it to default value which is GNSS_NV_EFS_SM_GM_GNSS_POS_QOS_SESS_TIMEOUT_DEFAULT*/
    tm_core_info.config_info.gm_config.sm_gm_high_responsiveness_config = GNSS_NV_EFS_SM_GM_HIGH_RESPONSIVENESS_CONFIG_DEFAULT;
  }

#ifdef FEATURE_MO_GPS_FOR_ECALL
  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_E911_CONFIG,
                               sizeof(uint32), &tm_core_info.config_info.sm_e911_config) == FALSE )
  {
    /* if returned false for any reason, force it to default value which is GNSS_NV_EFS_SM_E911_CONFIG_DEFAULT*/
    tm_core_info.config_info.sm_e911_config = GNSS_NV_EFS_SM_E911_CONFIG_DEFAULT;
  }
#endif

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_CPI_REQUEST_RATE,
                               sizeof(uint32), &tm_core_info.config_info.gm_config.sm_gm_cpi_request_rate) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.sm_gm_cpi_request_rate = GNSS_NV_EFS_SM_GM_CPI_REQUEST_RATE_DEFAULT;
  }

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_MED_RESP_BACKOFF,
                               sizeof(uint32), &tm_core_info.config_info.gm_config.sm_gm_med_resp_backoff) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.sm_gm_med_resp_backoff = GNSS_NV_EFS_SM_GM_MED_RESP_BACKOFF_DEFAULT;
  }

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_BO_CONFIG,
                               sizeof(gm_chal_gps_env_backoff_config_s_type), &tm_core_info.config_info.gm_config.sm_gm_chal_gps_env_bo_config) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.sm_gm_chal_gps_env_bo_config.q_backoff_min = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_BO_MIN_DEFAULT;
    tm_core_info.config_info.gm_config.sm_gm_chal_gps_env_bo_config.q_backoff_max = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_BO_MAX_DEFAULT;

  }
  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_MS_DIST_CONFIG,
                               sizeof(gm_chal_gps_env_ms_dist_config_s_type),
                               &tm_core_info.config_info.gm_config.sm_gm_chal_gps_env_ms_dist_config) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.sm_gm_chal_gps_env_ms_dist_config.q_dist_used_for_gps = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_MS_DIST_GPS_DEFAULT;
    tm_core_info.config_info.gm_config.sm_gm_chal_gps_env_ms_dist_config.q_dist_used_for_cpi = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_MS_DIST_CPI_DEFAULT;
  }

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_MS_SPEED_CONFIG,
                               sizeof(gm_ms_speed_config_s_type),
                               &tm_core_info.config_info.gm_config.sm_gm_motion_state_speed_config) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.sm_gm_motion_state_speed_config.gm_ms_walk_speed = GNSS_NV_EFS_SM_GM_MS_WALK_SPEED_DEFAULT;
    tm_core_info.config_info.gm_config.sm_gm_motion_state_speed_config.gm_ms_run_speed = GNSS_NV_EFS_SM_GM_MS_RUN_SPEED_DEFAULT;
  }

  if ( sm_nv_efs_reg_item_read(
         GNSS_NV_EFS_SM_GM_IOD_CONFIG_SWITCHES_MASK,
         sizeof(tm_core_info.config_info.gm_config.q_sm_gm_iod_config_switch_mask),
         &tm_core_info.config_info.gm_config.q_sm_gm_iod_config_switch_mask) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.q_sm_gm_iod_config_switch_mask = GNSS_NV_EFS_SM_GM_IOD_CONFIG_SWITCHES_MASK_DEFAULT;
    tm_core_info.config_info.gm_config.q_sm_gm_iod_config_switch_mask = 5;
  }

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_COMP_LOGGING_RATE,
                               sizeof(uint32), &tm_core_info.config_info.gm_config.q_gm_comp_logging_rate) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.q_gm_comp_logging_rate = GNSS_NV_EFS_SM_GM_COMP_LOGGING_RATE_DEFAULT;
  }

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_TM_MEAS_QMI_REPORT_CONTROL,
                               sizeof(uint8), &tm_core_info.config_info.gnss_meas_rep_qmi_out_control) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gnss_meas_rep_qmi_out_control = GNSS_NV_EFS_SM_TM_MEAS_QMI_REPORT_DEFAULT;
  }

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_LPP_GRT_24_OTDOA_AD_SUPPORT,
                               sizeof(uint8), &tm_core_info.config_info.u_gnss_lpp_grt_24_otdoa) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.u_gnss_lpp_grt_24_otdoa = GNSS_NV_EFS_SM_LPP_GRT_24_OTDOA_AD_DEFAULT;
  }
  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_LOWI_REGISTER_ENABLE,
    sizeof(boolean), &tm_core_info.config_info.gm_config.sm_gm_lowi_reg_enable) == FALSE)
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.sm_gm_lowi_reg_enable = GNSS_NV_EFS_SM_GM_LOWI_REG_ENABLE_DEFAULT;
  }

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_LOWI_SUBSCRIPTION_MASK,
    sizeof(uint32), &tm_core_info.config_info.gm_config.sm_gm_lowi_sub_mask) == FALSE)
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.sm_gm_lowi_sub_mask = GNSS_NV_EFS_SM_GM_LOWI_SUB_MASK_DEFAULT;
  }

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_LOWI_REG_CONFIG,
                               sizeof(tm_core_info.config_info.gm_config.sm_gm_lowi_reg_config), 
                               &tm_core_info.config_info.gm_config.sm_gm_lowi_reg_config) == FALSE)
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.sm_gm_lowi_reg_config.q_enable_reg_timer_mask = GNSS_NV_EFS_SM_GM_ENABLE_TIMER_MASK_DEFAULT;
    tm_core_info.config_info.gm_config.sm_gm_lowi_reg_config.q_short_term_timer_max_retry_cnt = GNSS_NV_EFS_SM_GM_SHORT_TERM_TIMER_MAX_RETRY_CNT_DEFAULT;
  }

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_FILTER_WLAN_FREQ_MASK,
    sizeof(uint32), &tm_core_info.config_info.gm_config.q_sm_gm_filter_wlan_freq_mask) == FALSE)
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.q_sm_gm_filter_wlan_freq_mask = GNSS_NV_EFS_SM_GM_FILTER_WLAN_FREQ_MASK_DEFAULT;
  }

  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_TM_CONFIG_AGNSS_TRANSMISSION_DELAY,
                             sizeof(tm_config_agnss_transmission_delay_s_type), 
                             &tm_core_info.config_info.agnss_trans_delay_config) == FALSE)
  {
    for (k=0; k<GNSS_NV_TM_TRANS_DELAY_MAX_INDEX; k++)
    {
      for (j=0; j<GNSS_NV_TM_TRANS_DELAY_MAX_PLANE; j++)
      {
        tm_core_info.config_info.agnss_trans_delay_config.trans_delay[k][j] = GNSS_NV_EFS_SM_TM_CONFIG_AGNSS_TRANSMISSION_DELAY_DEFAULT;
      }
    }
  }
  /* basic sanity to make sure protocol transmission delay is within range */
  for (k=0; k<GNSS_NV_TM_TRANS_DELAY_MAX_INDEX; k++)
  {
    for (j=0; j<GNSS_NV_TM_TRANS_DELAY_MAX_PLANE; j++)
    {
      if (tm_core_info.config_info.agnss_trans_delay_config.trans_delay[k][j] >
          GNSS_NV_EFS_SM_TM_CONFIG_AGNSS_TRANSMISSION_DELAY_MAX)
      {
         tm_core_info.config_info.agnss_trans_delay_config.trans_delay[k][j] =
           GNSS_NV_EFS_SM_TM_CONFIG_AGNSS_TRANSMISSION_DELAY_DEFAULT;
      }
    }
  }

  /* Read the GNSS NHz Config */
  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_NHZ_CONFIG,
                               sizeof(uint32), &tm_core_info.config_info.gnss_nhz_config) == FALSE )
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gnss_nhz_config = GNSS_NV_EFS_SM_NHZ_CONFIG_DEFAULT;
  }

  if (FALSE == sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_LPP_2TIMER_SUPPORT, sizeof(uint8),
                              &tm_core_info.config_info.u_gnss_lpp_rel12_2timer_support)) 
  {
     /*If NV read fails, force it to default*/
     tm_core_info.config_info.u_gnss_lpp_rel12_2timer_support = 
                                                      GNSS_NV_EFS_SM_LPP_2TIMER_SUPPORT_DEFAULT;
  }
}
  
/*===========================================================================

FUNCTION tm_core_refresh_nv_notify_protocols

DESCRIPTION
  This function notifies protocols to refresh carrier-specific NVs (mainly 
  for SIM hot swap and DD change).

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
static void tm_core_refresh_nv_notify_protocols(sys_modem_as_id_e_type sub)
{
  prtl_event_u_type  nv_refresh_event_payload;
  int prtlTableIndex = -1;

  MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Sending NV Refresh to protcolos for DD sub %d", sub);

  /*Send the PRTL_EVENT_TYPE_CARRIER_NV_REFRESH for SUPL module*/
  nv_refresh_event_payload.nv_refresh_payload.read_from_hw = TRUE;
  prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_UMTS_UP);
  if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
  {
    if(tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL)
    {
      (void) tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(PRTL_EVENT_TYPE_CARRIER_NV_REFRESH,
                                                                          &nv_refresh_event_payload);
    }
  }

  /*Send the PRTL_EVENT_TYPE_CARRIER_NV_REFRESH for LPP CP module*/
  prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_LPP_CP);
  if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
  {
    if(tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL)
    {
      (void) tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(PRTL_EVENT_TYPE_CARRIER_NV_REFRESH,
                                                                          &nv_refresh_event_payload);
    }
  }

}


/*===========================================================================

FUNCTION tm_core_refresh_carrier_specific_nv

DESCRIPTION
  This function refresh carrier-specific NVs (mainly for SIM hot swap) for a
  specific subscription (passed in p_data)

  sm_nv_read() will always read from HW and save in cache. 
  sm_nv_read_ex() may read from cache if available.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_refresh_carrier_specific_nv(void* p_data)
{
  int i, j;

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  sys_modem_as_id_e_type sub;

  if (p_data == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NV Refresh NULL ptr");
    return;
  }

  /* only parameter is uint8 sub for now (0~2) */
  sub = (sys_modem_as_id_e_type)(* ((uint8 *)p_data) );

  /* read carrier-specific NV & EFS NVs only for one SUB */
  tm_core_read_carrier_specific_nv(sub);

  tm_core_read_carrier_specific_efs_nv(sub);

  /* if current DD sub is being refreshed, protocols need to be notified as well */
  if (sub == tm_current_dd_subs_id)
  {
    tm_core_refresh_nv_notify_protocols(sub); 
  }

  /* notify MCFG that NV refresh is done, each sub will be refreshed one by one */
  tm_ruim_carrier_nv_refresh_done();

#endif /* FEATURE_MODEM_CONFIG_REFRESH */


  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_TM_CONFIG_AGNSS_TRANSMISSION_DELAY,
                               sizeof(tm_config_agnss_transmission_delay_s_type), 
                               &tm_core_info.config_info.agnss_trans_delay_config) == FALSE)
  {
    for (i=0; i<GNSS_NV_TM_TRANS_DELAY_MAX_INDEX; i++)
	  {
	    for (j=0; j<GNSS_NV_TM_TRANS_DELAY_MAX_PLANE; j++)
	    {
	      tm_core_info.config_info.agnss_trans_delay_config.trans_delay[i][j] = GNSS_NV_EFS_SM_TM_CONFIG_AGNSS_TRANSMISSION_DELAY_DEFAULT;
	    }
	  }
  }
  /* basic sanity to make sure protocol transmission delay is within range */
  for (i=0; i<GNSS_NV_TM_TRANS_DELAY_MAX_INDEX; i++)
  {
    for (j=0; j<GNSS_NV_TM_TRANS_DELAY_MAX_PLANE; j++)
    {
      if (tm_core_info.config_info.agnss_trans_delay_config.trans_delay[i][j] >
          GNSS_NV_EFS_SM_TM_CONFIG_AGNSS_TRANSMISSION_DELAY_MAX)
      {
         tm_core_info.config_info.agnss_trans_delay_config.trans_delay[i][j] =
           GNSS_NV_EFS_SM_TM_CONFIG_AGNSS_TRANSMISSION_DELAY_DEFAULT;
      }
    }
  }

}


#ifdef FEATURE_UIM_SUPPORT_LBS
/*===========================================================================

FUNCTION tm_core_ruim_read

DESCRIPTION
  This function read NV items to set TM-Core configuration info structure.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_ruim_read(void)
{
  /* Smart MSB */
  (void)tm_core_nv_async_read(NV_GPS1_DYNAMIC_MODE_I,(void *)&tm_nv_read_buffer);
  /* Read the available Position Options.     */
  (void)tm_core_nv_async_read(NV_GPS1_SEEDPOS_OPTION_I,(void *)&tm_nv_read_buffer);
}
/*===========================================================================

FUNCTION tm_core_ruim_read

DESCRIPTION
  This function is called  to set TM-Core configuration info structure, after
  Async NV read for NV items is complete

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_ruim_read_complete(tm_core_ruim_read_buffer *tm_core_ruim_buff)
{
  uint16 w_CellDBPosOption = 0xFFFF;
  /* Smart MSB */
  tm_core_info.config_info.smart_msb = (boolean)tm_core_ruim_buff->dynamic_mode;
  MSG_MED("SMART MSB %d",(int)tm_core_info.config_info.smart_msb, 0, 0); 
  /* Read the available Position Options.     */
  if(tm_core_ruim_buff->seed_pos_options)
  {
     w_CellDBPosOption = (uint16)tm_core_ruim_buff->seed_pos_options;
     MSG_MED("Seed POSITION optiond %d", (int)w_CellDBPosOption, 0, 0); 
  }
  mgp_SeedPosOptionInfoUpdate(w_CellDBPosOption);
}

#endif /* FEATURE_UIM_SUPPORT_LBS */


/*===========================================================================

FUNCTION tm_core_get_gps_lock_state

DESCRIPTION
  This function determines if gps is locked

RETURN VALUE
  TRUE if gps is locked, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_get_gps_lock_state(void)
{
  if (TM_CORE_CONFIG_DD(gps_lock) != PDSM_GPS_LOCK_NONE)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}
/*===========================================================================

FUNCTION tm_core_get_on_demand_state

DESCRIPTION
  This function determines if on-demand positioning is enabled

RETURN VALUE
  TRUE if on-demand is enabled, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_get_on_demand_state(void)
{
  return (tm_core_info.config_info.on_demand_lpm);
}

/*===========================================================================

FUNCTION tm_core_get_on_demand_ready_state

DESCRIPTION
  This function determines if on-demand positioning is enabled

RETURN VALUE
  TRUE if on-demand ready is enabled, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_get_on_demand_ready_state(void)
{
  return (tm_core_info.config_info.on_demand_ready);
}


/*===========================================================================

FUNCTION tm_core_is_on_demand_session_on

DESCRIPTION
  This function determines if on-demand session is ongoing

RETURN VALUE
  TRUE if on-demand is running, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_is_on_demand_session_on(void)
{
  if (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_ON_DEMAND)
  {
    return TRUE;
  }
  else
    return FALSE;
}

/*===========================================================================

FUNCTION tm_core_is_internal_session_on

DESCRIPTION
  This function determines if internal session is ongoing

RETURN VALUE
  TRUE if internal session is running, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_is_internal_session_on(void)
{
  if (tm_core_info.internal_session)
  {
    return TRUE;
  }
  else
    return FALSE;
}

/*===========================================================================

FUNCTION tm_core_is_periodic_ppm_session_on

DESCRIPTION
  This function determines if periodic PPM session is ongoing

RETURN VALUE
  TRUE if periodic PPM is running, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_is_periodic_ppm_session_on(void)
{
#ifdef FEATURE_GNSS_PERIODIC_PPM
  #error code not present
#else
  return FALSE;
#endif
}

/*===========================================================================

FUNCTION tm_core_get_periodic_ppm_session_handle

DESCRIPTION
  This function provides the special P.PPM session handle.

RETURN VALUE
  TM_SESS_HANDLE_NONE         : If P.PPM is not ON.
  TM_SESS_HANDLE_PERIODIC_PPM : Periodic PPM Session handle if P.PPM is on


  if P.PPM is OFF.
DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
tm_sess_handle_type tm_core_get_periodic_ppm_session_handle(void)
{
  if(tm_core_is_periodic_ppm_session_on())
  {
    return TM_SESS_HANDLE_PERIODIC_PPM;
  }
  else
{
    return TM_SESS_HANDLE_NONE;
  }
}


#ifdef FEATURE_TM_SECURITY

/*===========================================================================

FUNCTION tm_core_read_security_setting

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_read_security_setting(void)
{
  tm_sec_enable_data_s_type  sec_enable_data;

  /******************************************
     Enable/Disable Security based on NV
  ******************************************/
  if(sm_nv_read( NV_GPSONE_PASSWORD_I,(void *)&tm_nv_read_buffer) == TRUE)
  {
    /* If Security NV has been written, Always enable security feature */
    MSG_HIGH("Enabling secure PD API layer ", 0, 0, 0);

    sec_enable_data.base_ssd.ssd_length = (uint8)tm_nv_read_buffer.gpsone_password.password_len;

    /* If length is not valid, tm_sec_enable will create random SSDs */
    if( (sec_enable_data.base_ssd.ssd_length > 0)
      &&(sec_enable_data.base_ssd.ssd_length <= PDSM_MAX_SSD_LEN))
    {
      memscpy((void *)sec_enable_data.base_ssd.ssd_data,
           sizeof(sec_enable_data.base_ssd.ssd_data),
             (void *)tm_nv_read_buffer.gpsone_password.password,
             sec_enable_data.base_ssd.ssd_length);
    }

    /* Read the Random number update/challenge rate */
    (void) sm_nv_read(NV_GPS1_SEC_UPDATE_RATE_I, (void*)&tm_nv_read_buffer);
    sec_enable_data.rand_update_rate.rate = tm_nv_read_buffer.gps1_sec_update_rate;

    sec_enable_data.data_validity_mask = TM_SEC_DATA_VALIDITY_SSD | TM_SEC_DATA_VALIDITY_RAND;

    tm_sec_enable(&sec_enable_data, GPSONE_SEC_FLAG);
  }
  else
  {
    /* If Security NV has never been written, disable security feature */
    MSG_HIGH("Disabling secure PD API layer ", 0, 0, 0);
  }
}
#endif /* FEATURE_TM_SECURITY */


#ifdef FEATURE_CGPS_XTRA


/*===========================================================================
FUNCTION tm_core_is_xtra_operation_allowed

DESCRIPTION
Check if assisted mode GPS session are in progress.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE
XTRA command status
===========================================================================*/
boolean tm_core_is_xtra_operation_allowed
(
  void
)
{
  /* Local Variable */
  boolean           ret_val = FALSE;

  /* Allow XTRA data or time dload during idle, standalone, or diag sessions */
  if(   tm_core_info.session_info.from_protocol == TM_PRTL_TYPE_NONE
     || tm_core_info.session_info.from_protocol == TM_PRTL_TYPE_STANDALONE
     || tm_core_info.session_info.from_protocol == TM_PRTL_TYPE_DIAG
    )
  {
    ret_val = TRUE;
  }
  else if (tm_core_info.session_info.msb_xtra_allowed == TRUE)
  {
    MSG_MED ("XTRA is allowed for MSB AGPS or after MSB DLOAD comm failue",0,0,0);
    ret_val = TRUE;
  }
  /* Else if the current session is 1x UP, 1x CP, UMTS UP, and UMTS CP */
  else if( tm_core_info.session_info.session_state == TM_CORE_SESS_STATE_INIT )
  {
    ret_val = TRUE;
  }

  return ret_val;
} /* End tm_core_is_xtra_operation_allowed()*/


/*===========================================================================

FUNCTION tm_xtra_init_and_read_nv

DESCRIPTION
  This function reads NV items to set TM-XTRA configuration info structure.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_xtra_init_and_read_nv
(
  void
)
{
  /* Local Variables */
  tm_xtra_state_type *xtra_state_ptr;
  nv_item_type       nv_write_buffer;

  /*----------------------------------------------------------------------
     Read all XTRA NV items
  ----------------------------------------------------------------------*/

  /* Enable/Disable XTRA module */
  (void) sm_nv_read(NV_GPS1_XTRA_ENABLED_I, (void *) &tm_nv_read_buffer);

#ifdef __GRIFFON__
  tm_nv_read_buffer.gps1_xtra_enabled = TRUE;
#endif  // __GRIFFON__

  if((boolean)tm_nv_read_buffer.gps1_xtra_enabled)
  {
    /*Dynamically allocate memory for XTRA state pointer*/
    if(!tm_xtra_init_state_ptr())
    {
      MSG_ERROR("Dynamic memory allocation for XTRA failed. Disabling XTRA feature",0,0,0);
        nv_write_buffer.gps1_xtra_enabled = 0;
      (void)tm_core_nv_write(NV_GPS1_XTRA_ENABLED_I,(void*)&nv_write_buffer );

      return;
    }
  }
  else
  {
   MSG_MED("XTRA feature disabled",0,0,0);
   return;
  }

  xtra_state_ptr = tm_xtra_get_state_ptr();

  if(xtra_state_ptr == NULL)
  {
  MSG_ERROR("XTRA STATE PTR found NULL",0,0,0);
  return;
  }

  xtra_state_ptr->xtra_enabled = (boolean)tm_nv_read_buffer.gps1_xtra_enabled;

  /* Number of hours between automatic downloads */
  (void) sm_nv_read(NV_GPS1_XTRA_DOWNLOAD_INTERVAL_I, (void *) &tm_nv_read_buffer);
  xtra_state_ptr->download_interval
                 = (uint16)tm_nv_read_buffer.gps1_xtra_download_interval;

  /* Number of unsuccessful download attempts before aborting (xtra client) */
  (void) sm_nv_read(NV_GPS1_XTRA_NUM_DOWNLOAD_ATTEMPTS_I, (void *) &tm_nv_read_buffer);
  xtra_state_ptr->num_download_attempts
                 = (uint8)tm_nv_read_buffer.gps1_xtra_num_download_attempts;

  /* Number of minutes between download attempts */
  (void) sm_nv_read(NV_GPS1_XTRA_TIME_BETWEEN_ATTEMPTS_I , (void *) &tm_nv_read_buffer);
  xtra_state_ptr->time_between_attempts
                 = (uint8)tm_nv_read_buffer.gps1_xtra_time_between_attempts;

  /* Enable/disable automatic periodic downloads of gpsOneXTRA files */
  (void) sm_nv_read(NV_GPS1_XTRA_AUTO_DOWNLOAD_ENABLED_I , (void *) &tm_nv_read_buffer);
  xtra_state_ptr->auto_download_enabled
                 = (boolean)tm_nv_read_buffer.gps1_xtra_auto_download_enabled;

  /* Primary XTRA Server URL */
  (void) sm_nv_read(NV_GPS1_XTRA_PRIMARY_SERVER_URL_I , (void *) &tm_nv_read_buffer);
  memscpy((void *) &xtra_state_ptr->primary_data_server_url[0],
         sizeof(xtra_state_ptr->primary_data_server_url),
         (const void *)&tm_nv_read_buffer.gps1_xtra_primary_server_url[0],
         sizeof(tm_nv_read_buffer.gps1_xtra_primary_server_url));

  /* Secondary XTRA Server URL */
  (void) sm_nv_read(NV_GPS1_XTRA_SECONDARY_SERVER_URL_I , (void *) &tm_nv_read_buffer);
  memscpy((void *)&xtra_state_ptr->secondary_data_server_url[0],
         sizeof(xtra_state_ptr->secondary_data_server_url),
         (const void *)&tm_nv_read_buffer.gps1_xtra_secondary_server_url[0],
         sizeof(tm_nv_read_buffer.gps1_xtra_secondary_server_url));

  /* Tertiary gpsOneXTRA Server URL */
  (void) sm_nv_read(NV_GPS1_XTRA_TERTIARY_SERVER_URL_I , (void *) &tm_nv_read_buffer);
  memscpy((void *)&xtra_state_ptr->tertiary_data_server_url[0],
         sizeof(xtra_state_ptr->tertiary_data_server_url),
         (const void *)&tm_nv_read_buffer.gps1_xtra_tertiary_server_url[0],
         sizeof(tm_nv_read_buffer.gps1_xtra_tertiary_server_url));


  /*----------------------------------------------------------------------
     Read all XTRA time NV items
  ----------------------------------------------------------------------*/
  /* Enable/Disable XTRA time info */
  (void) sm_nv_read(NV_GPS1_XTRA_TIME_INFO_ENABLED_I, (void *) &tm_nv_read_buffer);
  xtra_state_ptr->time_info_enabled
                         = (boolean)tm_nv_read_buffer.gps1_xtra_time_info_enabled;

  /* One-way delay failover threshold */
  (void) sm_nv_read(NV_GPS1_XTRA_TIME_INFO_DELAY_THRESH_I, (void *) &tm_nv_read_buffer);
  xtra_state_ptr->time_info_oneway_delay_thresh
                         = (uint32)tm_nv_read_buffer.gps1_xtra_time_info_delay_thresh;

  /* time uncertainty threshold */
  (void) sm_nv_read(NV_GPS1_XTRA_TIME_INFO_UNC_THRESH_I, (void *) &tm_nv_read_buffer);
  xtra_state_ptr->time_info_uncertainty_thresh
                          = (uint32)tm_nv_read_buffer.gps1_xtra_time_info_unc_thresh;

  /* Primary SNTP Server URL */
  (void) sm_nv_read(NV_GPS1_XTRA_PRIMARY_SNTP_SERVER_URL_I , (void *) &tm_nv_read_buffer);
  memscpy((void *)&xtra_state_ptr->primary_sntp_server_url[0],
         sizeof(xtra_state_ptr->primary_sntp_server_url),
         (const void *)&tm_nv_read_buffer.gps1_xtra_primary_sntp_server_url[0],
         sizeof(tm_nv_read_buffer.gps1_xtra_primary_sntp_server_url));

  /* Secondary SNTP Server URL */
  (void) sm_nv_read(NV_GPS1_XTRA_SECONDARY_SNTP_SERVER_URL_I , (void *) &tm_nv_read_buffer);
  memscpy((void *)&xtra_state_ptr->secondary_sntp_server_url[0],
         sizeof(xtra_state_ptr->secondary_sntp_server_url),
         (const void *)&tm_nv_read_buffer.gps1_xtra_secondary_sntp_server_url[0],
         sizeof(tm_nv_read_buffer.gps1_xtra_secondary_sntp_server_url));

  /* Tertiary SNTP Server URL */
  (void) sm_nv_read(NV_GPS1_XTRA_TERTIARY_SNTP_SERVER_URL_I , (void *) &tm_nv_read_buffer);
  memscpy((void *)&xtra_state_ptr->tertiary_sntp_server_url[0],
         sizeof(xtra_state_ptr->tertiary_sntp_server_url),
         (const void *)&tm_nv_read_buffer.gps1_xtra_tertiary_sntp_server_url[0],
         sizeof(tm_nv_read_buffer.gps1_xtra_tertiary_sntp_server_url));

#ifdef FEATURE_GNSS_XTRA3
  /* XTRA3 last key index */
  if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_XTRA_LAST_XTRA3_KEY_INDEX,
    sizeof(uint16),
    (void *)&xtra_state_ptr->xtra3_last_key_index) == FALSE)
  {
    /* if returned false for any reason, force it to default value */
    xtra_state_ptr->xtra3_last_key_index = GNSS_NV_EFS_SM_XTRA_LAST_XTRA3_KEY_INDEX_DEFAULT;
  }
#endif /* FEATURE_GNSS_XTRA3 */

  /* Initialize XTRA module */
  tm_xtra_init();

  return;
} /* End tm_xtra_read_nv() */

#ifdef FEATURE_UIM_SUPPORT_LBS
/*===========================================================================

FUNCTION tm_xtra_ruim_init_and_read

DESCRIPTION
  This function reads NV items to set TM-XTRA configuration info structure.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_xtra_ruim_init_and_read
(
  void
)
{
    /*----------------------------------------------------------------------
       Read all XTRA NV items
    ----------------------------------------------------------------------*/

    /* Enable/Disable XTRA module */
    (void) tm_core_nv_async_read(NV_GPS1_XTRA_ENABLED_I, (void *) &tm_nv_read_buffer);

    /* Number of hours between automatic downloads */
    (void) tm_core_nv_async_read(NV_GPS1_XTRA_DOWNLOAD_INTERVAL_I, (void *) &tm_nv_read_buffer);


    /* Number of unsuccessful download attempts before aborting (xtra client) */
    (void) tm_core_nv_async_read(NV_GPS1_XTRA_NUM_DOWNLOAD_ATTEMPTS_I, (void *) &tm_nv_read_buffer);


    /* Number of minutes between download attempts */
    (void) tm_core_nv_async_read(NV_GPS1_XTRA_TIME_BETWEEN_ATTEMPTS_I , (void *) &tm_nv_read_buffer);


    /* Enable/disable automatic periodic downloads of gpsOneXTRA files */
    (void) tm_core_nv_async_read(NV_GPS1_XTRA_AUTO_DOWNLOAD_ENABLED_I , (void *) &tm_nv_read_buffer);


    /* Primary XTRA Server URL */
    (void) tm_core_nv_async_read(NV_GPS1_XTRA_PRIMARY_SERVER_URL_I , (void *) &tm_nv_read_buffer);


    /* Secondary XTRA Server URL */
    (void) tm_core_nv_async_read(NV_GPS1_XTRA_SECONDARY_SERVER_URL_I , (void *) &tm_nv_read_buffer);


    /* Tertiary gpsOneXTRA Server URL */
    (void) tm_core_nv_async_read(NV_GPS1_XTRA_TERTIARY_SERVER_URL_I , (void *) &tm_nv_read_buffer);

    return;
}
/*===========================================================================

FUNCTION tm_xtra_ruim_read_complete

DESCRIPTION
  This function indicates NV Read completion and set TM-XTRA configuration 
  info structure.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_xtra_ruim_read_complete(tm_xtra_ruim_nv_buffer* ruim_nv_buffer)
{

  tm_xtra_state_type *xtra_state_ptr;
  nv_item_type       nv_write_buffer;

    if (ruim_nv_buffer == NULL) 
    {
       MSG_ERROR("XTRA RUIM NV Read from failed.",0,0,0);
       return;
    }

    xtra_state_ptr = tm_xtra_get_state_ptr();

    if((boolean)ruim_nv_buffer->xtra_enabled || xtra_state_ptr == NULL)
    {
      /*Dynamically allocate memory for XTRA state pointer*/
      if(!tm_xtra_init_state_ptr())
      {
        MSG_ERROR("Dynamic memory allocation for XTRA failed. Disabling XTRA feature",0,0,0);
        nv_write_buffer.gps1_xtra_enabled = 0;
        (void)tm_core_nv_write(NV_GPS1_XTRA_ENABLED_I,(void*)&nv_write_buffer );

        return;
      }
      MSG_MED("XTRA FEATURE ENABLE/DISABLE: NV Async READ : %d",(int)ruim_nv_buffer->xtra_enabled,0,0);
    }
    else
    {
       MSG_MED("XTRA feature disabled",0,0,0);
       return;
    }

    xtra_state_ptr->xtra_enabled = (boolean)ruim_nv_buffer->xtra_enabled;

    /* Number of hours between automatic downloads */
    xtra_state_ptr->download_interval
                   = (uint16)ruim_nv_buffer->download_interval;

    /* Number of unsuccessful download attempts before aborting (xtra client) */
    xtra_state_ptr->num_download_attempts
                   = (uint8)ruim_nv_buffer->num_download_attempts;

    /* Number of minutes between download attempts */
    xtra_state_ptr->time_between_attempts
                   = (uint8)ruim_nv_buffer->time_between_attempts;

    /* Enable/disable automatic periodic downloads of gpsOneXTRA files */
    xtra_state_ptr->auto_download_enabled
                   = (boolean)ruim_nv_buffer->auto_download_enabled;

    /* Primary XTRA Server URL */
    memscpy((void *) &xtra_state_ptr->primary_data_server_url[0],
       sizeof(xtra_state_ptr->primary_data_server_url),
           (const void *)&(ruim_nv_buffer->xtra_server_primary[0]),
           sizeof(ruim_nv_buffer->xtra_server_primary));

    /* Secondary XTRA Server URL */
    memscpy((void *)&xtra_state_ptr->secondary_data_server_url[0],
       sizeof(xtra_state_ptr->secondary_data_server_url),
           (const void *)&(ruim_nv_buffer->xtra_server_secondary[0]),
           sizeof(ruim_nv_buffer->xtra_server_secondary));

    /* Tertiary gpsOneXTRA Server URL */
    memscpy((void *)&xtra_state_ptr->tertiary_data_server_url[0],
       sizeof(xtra_state_ptr->tertiary_data_server_url),
           (const void *)&(ruim_nv_buffer->xtra_server_tertiary[0]),
           sizeof(ruim_nv_buffer->xtra_server_tertiary));

    mgp_RuimXtraInit();

    /* Initialize XTRA module */
    tm_xtra_init();

    return;
}
#endif /* FEATURE_UIM_SUPPORT_LBS */
#endif /* FEATURE_CGPS_XTRA */



#ifdef FEATURE_GNSS_PERIODIC_PPM
#error code not present
#endif


/*===========================================================================

FUNCTION tm_core_set_current_subs_cfg

DESCRIPTION
  This function is used to se the currently active subscription-specific
  configuration for MultiSIM EFS NVs.

  Usually DD sub configuration, if phone is OOS, active configuration will be 
  the last used configuration

  This should be the only function within SM to update the currently active
  subscription-specific configuration when DD sub is changed

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static void tm_core_set_current_subs_cfg(sys_modem_as_id_e_type dd_subs)
{
  if ((dd_subs < SM_CONFIG_MIN_SUB_ID) || (dd_subs > tm_core_info.config_info.max_valid_subs_id))
  {
     MSG_3( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "DD sub %d out of range 0~%d (Keeping current %d)",
              dd_subs, tm_core_info.config_info.max_valid_subs_id, tm_core_info.config_info.active_subs_cfg);
  }
  else
  {
    if (tm_core_info.session_info.active_sess_handle != TM_SESS_HANDLE_NONE)
    {
      // one active session is going on, print out warning
      MSG_3( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "DD changed during active session (state=%d handle=0x%x DD=%d)",
            tm_core_info.session_info.session_state,
            tm_core_info.session_info.active_sess_handle, dd_subs);
    }

    /* within valid range 0~MAX_SUB-1 0~1/0~2, set lower layer NV Subs ID */
    sm_nv_set_current_sub_id(dd_subs);

    if ((uint32)dd_subs != tm_core_info.config_info.active_subs_cfg )
    {
    tm_core_info.config_info.active_subs_cfg = (uint32)dd_subs;

      /* Let protocol refresh their own carrier-specific NVs for DD-sub since it's changed */
      /* tm_core_refresh_nv_notify_protocols(dd_subs); */
    }
  }
}

/*===========================================================================

FUNCTION tm_core_init

DESCRIPTION
  This function is to initialize TM-Core data structures.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_init(void)
{
  int i;
  nv_item_type      z_nv_qwip_read_buffer;
#ifdef FEATURE_MMGSDI_SESSION_LIB
  int index;
#endif /*FEATURE_MMGSDI_SESSION_LIB*/
  uint8 sub = 0;

  if (tm_core_device_config.tm_cm_device_info == TM_CORE_DEVICE_MODE_SS)
  {
    tm_core_info.config_info.max_valid_subs_id = SM_CONFIG_MIN_SUB_ID;
    sm_nv_set_max_sub_id(SM_CONFIG_MIN_SUB_ID); // Single SIM only
  }
  else
  {
    tm_core_info.config_info.max_valid_subs_id = SM_CONFIG_MAX_SUB_ID;
    sm_nv_set_max_sub_id(SM_CONFIG_MAX_SUB_ID); // DSDS or TSTS
  }

  /* set the DD sub configuration pointer to default sub at boot-up asap to avoid NULL pointer */
  tm_core_set_current_subs_cfg(SM_CONFIG_MIN_SUB_ID);

  /* Read NV settings */
  tm_core_read_nv();

  /* Initialize non-NV config info. Default enable GPS */
  tm_core_info.config_info.sv_poly_rep_constell_config= 0;
  tm_core_info.config_info.gnss_meas_rep_constell_config= 0;

  /* Initialize session info */
  tm_core_info.session_info.active_sess_handle      = TM_SESS_HANDLE_NONE;
  tm_core_info.session_info.op_mode                 = TM_SESS_OPERATION_MODE_NONE;
  tm_core_info.session_info.op_requested            = TM_OP_REQ_NONE;
  tm_core_info.session_info.from_protocol           = TM_PRTL_TYPE_NONE;
  tm_core_info.session_info.pd_param.client_type    = PDSM_CLIENT_TYPE_NONE;
  tm_core_info.session_info.pd_param.client_id      = -1;
  tm_core_info.session_info.mo_sess_handle_seed     = 0;
  tm_core_info.session_info.session_event_firing    = TRUE;
  tm_core_info.session_info.eph_svmask              = 0;
  tm_core_info.session_info.alm_svmask              = 0;
  tm_core_info.session_info.queued_request          = FALSE;
  tm_core_info.session_info.q_FixTimeRemainingMSec    = 0;
  tm_core_info.session_info.msb_xtra_allowed        = FALSE;
  tm_core_info.session_info.req_xtra_trig_by_agps   = FALSE;
  tm_core_info.session_info.end_odp_session         = FALSE;
  tm_core_info.session_info.force_eph_download      = FALSE;
  tm_core_info.session_info.glo_force_eph_download  = FALSE;
  tm_core_info.session_info.bds_force_eph_download  = FALSE;
  tm_core_info.session_info.gal_force_eph_download  = FALSE;
  tm_core_info.session_info.pos_inj_requested       = FALSE;
  tm_core_info.session_info.eph_need_sv_mask = 0;
  tm_core_info.session_info.glo_eph_need_sv_mask = 0;
  tm_core_info.session_info.bds_eph_need_sv_mask = 0;
  tm_core_info.session_info.gal_eph_need_sv_mask = 0;
  tm_core_info.session_info.sess_info_valid_flags = 0;
  tm_core_info.session_info.trusted_utc_time        = FALSE;
  tm_core_info.session_info.receiver_off_needed     = FALSE;
  tm_core_info.session_info.iono                    = 1; /* iono invalid, 1 means MGP has
                                                         requested iono from AGPS */
  tm_core_info.session_info.utc                     = 1; /* utc invalid, 1 means MGP has
                                                         requested utc from AGPS */
  tm_core_info.session_info.health_svmask           = 0;
  tm_core_info.session_info.valid_svmask            = 0;
  tm_core_info.session_info.sbas_eph_svmask         = 0;
  tm_core_info.session_info.sbas_alm_svmask         = 0;
  tm_core_info.session_info.glo_eph_svmask          = 0;
  tm_core_info.session_info.glo_alm_svmask          = 0;
  tm_core_info.session_info.bds_eph_svmask          = 0;
  tm_core_info.session_info.bds_alm_svmask          = 0;

  tm_core_info.session_info.qzss_eph_svmask         = 0;
  tm_core_info.session_info.qzss_alm_svmask         = 0;
  tm_core_info.session_info.gal_eph_svmask          = 0;
  tm_core_info.session_info.gal_alm_svmask          = 0;

  tm_core_info.ext_opt_field_mask = 0;

  memset((void *)&tm_core_info.pd_ext_data, 0, sizeof(tm_core_info.pd_ext_data));
  memset((void *)&tm_core_info.gps_meas_blk, 0, sizeof(tm_core_info.gps_meas_blk));
  memset((void *)&z_cached_pos_info, 0, sizeof(tm_core_cached_pos_s_type));
  memset((void *)&z_cached_meas_info, 0, sizeof(tm_core_cached_meas_s_type));
  memset((void *)&z_cached_ref_pos_info, 0, sizeof(tm_core_cached_ref_pos_s_type));

  /* Initialize the call back logs */
  memset((void *)tm_call_back_log,0, sizeof(tm_call_back_log));

  tm_core_info.e_cached_rcvr_state = C_GPS_RCVR_STATE_OFF;

  tm_core_update_session_state(TM_CORE_SESS_STATE_INIT);

  /* Initialize TM-Core / Protocol Interface */
  for(i = 0; i < TM_PRTL_NUM; i++)
  {
    tm_core_info.prtl_func_cb_table[i].init_fp             = NULL;
    tm_core_info.prtl_func_cb_table[i].start_sess_req_fp   = NULL;
    tm_core_info.prtl_func_cb_table[i].stop_sess_req_fp    = NULL;
    tm_core_info.prtl_func_cb_table[i].sess_req_data_fp    = NULL;
    tm_core_info.prtl_func_cb_table[i].sess_info_fp        = NULL;
    #ifdef  FEATURE_CGPS_PDCOMM
    tm_core_info.prtl_func_cb_table[i].data_pipe_handle_fp = NULL;
    #endif /* FEATURE_CGPS_PDCOMM */
    tm_core_info.prtl_func_cb_table[i].timer_cb_fp         = NULL;
    tm_core_info.prtl_func_cb_table[i].event_cb_fp         = NULL;
    tm_core_info.prtl_func_cb_table[i].general_query_fp    = NULL;
  }

  /* Setup init function for supported protocols */
  tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_STANDALONE].init_fp = tm_standalone_prtl_init;

#if defined (FEATURE_SUPPORT_CDMA_GPS_PROTOCOLS) && !defined (FEATURE_GNSS_LOW_MEMORY)
  tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_1X_UP].init_fp      = tm_1xup_prtl_init;
  tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_1X_CP].init_fp      = tm_1x_cp_prtl_init;
#endif /* FEATURE_SUPPORT_CDMA_GPS_PROTOCOLS && ! FEATURE_GNSS_LOW_MEMORY */

  tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_DIAG].init_fp       = tm_diag_prtl_init;

  #ifdef FEATURE_CGPS_UMTS_UP
/*** DO NOT reverse the order of UP and CP, since some common actions only need
     to be initialized once, and if both are defined, they are done in UP */
  tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP].init_fp = tm_umts_up_prtl_iface_init;

  #ifdef FEATURE_USER_PLANE_MRL
  tm_l1_iface_init();
  #endif

  #endif /* FEATURE_CGPS_UMTS_UP */

  /* Init the GFC_QMI module */
  tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_GFC_QMI].init_fp = tm_gfc_qmi_prtl_init;

  #ifdef FEATURE_CGPS_XTRA_T
  /* Init the XTRA-T module */
  tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_XTRA_T].init_fp = tm_xtra_t_init;
  #endif

  #if defined (FEATURE_CGPS_UMTS_CP) && !defined (FEATURE_GNSS_LOW_MEMORY)
  tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_CP].init_fp = tm_umts_cp_prtl_iface_init;

  tm_mm_iface_init();
  #endif /* FEATURE_CGPS_UMTS_CP && ! FEATURE_GNSS_LOW_MEMORY */

  #if defined (FEATURE_CGPS_UMTS_CP_WCDMA) || defined ( FEATURE_WCDMA_GPS_CELL_ID_READING )
  tm_rrc_iface_init(); /* registers call back with RRC */
  #endif
#if !defined(FEATURE_GNSS_SA) && defined(FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
  tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_LPP_CP].init_fp  = tm_lpp_cp_prtl_iface_init;

  /* Initialize the Transaction Manager */
  tm_lpp_tx_mgr_init(0);
#endif /*!FEATURE_GNSS_SA && FEATURE_CGPS_LTE_CELLDB && !FEATURE_GNSS_LOW_MEMORY*/

#if (defined(FEATURE_CGPS_UMTS_UP_SUPL) || defined(FEATURE_CGPS_XTRA_T)) 
#ifdef FEATURE_MMGSDI_SESSION_LIB
  /* Initialize MMGSDI */
  tm_sim_mmgsdi_init();
#endif /* FEATURE_MMGSDI_SESSION_LIB */
#endif /* FEATURE_CGPS_UMTS_UP_SUPL || FEATURE_CGPS_XTRA_T */

  /* Initialize supported protocol sub-modules */
  for(i = 0; i < TM_PRTL_NUM; i++)
  {
    if(tm_core_info.prtl_func_cb_table[i].init_fp != NULL)
    {
      tm_core_info.prtl_func_cb_table[i].init_fp();
    }
  }

#ifdef FEATURE_TM_SECURITY
  /* Initialize security layer */
  tm_sec_init();

  /* Make sure tm_sec_init is called before tm_core_read_security_nv */
  tm_core_read_security_setting();
#endif /* FEATURE_TM_SECURITY */

  if (tm_core_info.config_info.nmea_config_type.pd_nmea_port_type != PDSM_PA_NMEA_PORT_TYPE_NONE)
  {
    tm_nmea_diag_open_port((tm_nmea_diag_open_port_id_e_type)tm_core_info.config_info.nmea_config_type.pd_nmea_port_type);
  }
  else
  {
    tm_nmea_diag_close_port(NULL);
  }

  /* Initialize OEM DRE Tunnel Detection Enablement control flag */
  tm_core_info.config_info.oemdre_td_ctrl = FALSE;

#ifdef FEATURE_CGPS_XTRA
  /* Make sure tm_sec_init is called before tm_core_read_security_nv */
  tm_xtra_init_and_read_nv();
#endif /* FEATURE_CGPS_XTRA */

  (void)sm_nv_read(NV_CGPS_QWIP_LOC_ENGINE_CONFIG_I, (void *)&z_nv_qwip_read_buffer);
  MSG_HIGH("NV Read done, value %d",z_nv_qwip_read_buffer.cgps_qwip_loc_engine_config,0,0);
  tm_core_info.loc_engine_config = (tm_sysd_loc_engine_cfg_e_type) z_nv_qwip_read_buffer.cgps_qwip_loc_engine_config;

  /* Initialize to SYSD not triggered */
  tm_core_info.qwip_sysd_triggered = FALSE;
  /* Initialize to SYSD State info not requested */
  tm_core_info.qwip_st_info_req_status = SYSD_STATE_INFO_REQ_NOT_SET;

  tm_core_info.internal_session = FALSE;


  /* initialize XSPI module */
  tm_xspi_init_and_read_nv();

#ifdef FEATURE_GNSS_PERIODIC_PPM
  #error code not present
#endif
  
  tm_core_info.tm_task_info.task_active = TRUE;

#ifdef FEATURE_MMGSDI_SESSION_LIB
  /* Initialise MMGSDI table with default session info */
  for(index = 0; index < sizeof(tm_mmgsdi_session_id_table)/sizeof(tm_mmgsdi_session_id_table_entry_s_type);
      index++)
  {
    tm_mmgsdi_session_id_table[index].asubs_id = SYS_MODEM_AS_ID_NONE;
    tm_mmgsdi_session_id_table[index].session_id = (mmgsdi_session_id_type)MMGSDI_INVALID_SESSION_ID;

    tm_mmgsdi_session_id_table[index].session_id_set = FALSE;
  }
#endif /*FEATURE_MMGSDI_SESSION_LIB*/  
  
  /* initialize the delete all throttle timer */
  tm_core_info.tm_delete_param_throttle_timer = os_TimerCreate((uint32)TM_CORE_TIMER_ID_DELETE_THROTTLE_ID,(uint32)THREAD_ID_SM_TM);
  
}

/*===========================================================================

FUNCTION tm_core_get_prtl_idx_by_type

DESCRIPTION
  This function returns protocol array index based on protcol type

DEPENDENCIES

RETURN VALUE
  -1, if protocol is not supported.
  Otherwise, array index for the protocol type

SIDE EFFECTS

===========================================================================*/
int tm_core_get_prtl_idx_by_type(tm_prtl_type prtl_type)
{
  switch(prtl_type)
  {
    case TM_PRTL_TYPE_STANDALONE:
    return TM_CORE_PRTL_CB_ARR_INDEX_STANDALONE;
    #ifndef FEATURE_GNSS_LOW_MEMORY
    case TM_PRTL_TYPE_1X_UP:
    return TM_CORE_PRTL_CB_ARR_INDEX_1X_UP;

    case TM_PRTL_TYPE_1X_CP:
    return TM_CORE_PRTL_CB_ARR_INDEX_1X_CP;

    case TM_PRTL_TYPE_UMTS_CP:
    return TM_CORE_PRTL_CB_ARR_INDEX_UMTS_CP;
    #endif /* ! FEATURE_GNSS_LOW_MEMORY - For fall back to standalone*/
    #ifdef FEATURE_CGPS_UMTS_UP
    case TM_PRTL_TYPE_UMTS_UP:
    return TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP;
     #endif
    case TM_PRTL_TYPE_DIAG:
    return TM_CORE_PRTL_CB_ARR_INDEX_DIAG;

#ifdef FEATURE_GNSS_PERIODIC_PPM
    #error code not present
#endif
    case TM_PRTL_TYPE_GFC_QMI:
    return TM_CORE_PRTL_CB_ARR_INDEX_GFC_QMI;

#ifdef FEATURE_CGPS_XTRA_T
    case TM_PRTL_TYPE_XTRA_T:
    return TM_CORE_PRTL_CB_ARR_INDEX_XTRA_T;
#endif

#if !defined(FEATURE_GNSS_SA) && defined(FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
    case TM_PRTL_TYPE_LPP_CP:
    return TM_CORE_PRTL_CB_ARR_INDEX_LPP_CP;
#endif

    default:
    return -1;
  }
}

/*===========================================================================

FUNCTION tm_core_get_prtl_idx_by_lcs_cmd_type

DESCRIPTION
  This function returns protocol array index based on user response type.

DEPENDENCIES

RETURN VALUE
  protol array index if lcs_cmd_type is supported
  -1, otherwise

SIDE EFFECTS

===========================================================================*/
int tm_core_get_prtl_idx_by_lcs_cmd_type(pdsm_lcs_info_cmd_e_type  lcs_info_cmd_type)
{
  switch(lcs_info_cmd_type)
  {
    case PDSM_LCS_CMD_TYPE_VX_REQ:
    case PDSM_LCS_CMD_TYPE_VX_CONFIG:
    case PDSM_LCS_CMD_TYPE_SET_VX_CONFIG:
    case PDSM_LCS_CMD_TYPE_VX_SERVICE_INTERACTION:
      return TM_CORE_PRTL_CB_ARR_INDEX_1X_UP;

    case PDSM_LCS_CMD_TYPE_UMTS_CP_REQ:
      return TM_CORE_PRTL_CB_ARR_INDEX_UMTS_CP;

    case PDSM_LCS_CMD_TYPE_SUPL_REQ:
      return TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP;

    default:
      return -1;
  }
}

/*===========================================================================

FUNCTION tm_core_new_mo_sess_handle

DESCRIPTION
  This function generates a new MO session handle for incoming MO fix request.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
tm_sess_handle_type tm_core_new_mo_sess_handle(void)
{
  tm_sess_handle_type mo_sess_handle;

  tm_core_info.session_info.mo_sess_handle_seed %= 8;

  mo_sess_handle = 1 << tm_core_info.session_info.mo_sess_handle_seed;

  tm_core_info.session_info.mo_sess_handle_seed++;

  return mo_sess_handle;
}

/*===========================================================================

FUNCTION tm_core_get_lte_lpp_up_enable

DESCRIPTION
  This function determines if LPP protocol on LTE is enabled in NV 

 

RETURN VALUE
  TRUE if LPP is enabled, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_get_lte_lpp_up_enable(void)
{
  if (TM_CORE_CONFIG_DD(gnss_lte_lpp_enable) & PDSM_PA_LPP_IN_LTE_UP_ENABLE) 
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================
FUNCTION tm_core_get_lpp_cp_enable

DESCRIPTION
  This function determines if LPP Control Plane protocol is enabled in NV 

 

RETURN VALUE
  TRUE if LPP is enabled, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/

boolean tm_core_get_lpp_cp_enable(void)
{
  if (TM_CORE_CONFIG_DD(gnss_lte_lpp_enable) & PDSM_PA_LPP_IN_LTE_CP_ENABLE) 
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================
FUNCTION tm_core_get_gnss_emergency_protocol_non_lte

DESCRIPTION
  This function determines if Control Plane or UP protocol is used for E911 on non-LTE 

 

RETURN VALUE
  TRUE if UP is enabled, FALSE if CP

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
uint32 tm_core_get_gnss_emergency_protocol_non_lte(void)
{
  return tm_core_info.config_info.gnss_emergency_protocol_non_lte;
}

/*===========================================================================

FUNCTION tm_core_get_active_prtl_module

DESCRIPTION
  This function checks the acquired system and selects a proper protocol
  sub-module to handle incoming MO fix request.

DEPENDENCIES

RETURN VALUE
  protocol sub-module type.

SIDE EFFECTS

===========================================================================*/
tm_prtl_type tm_core_get_active_prtl_module (pdsm_mt_initiator_e_type  mt_initiator)
{

  tm_cm_phone_state_info_s_type phone_state_info;

  /* Query TM-CM module to get current acquired system */
  tm_cm_iface_get_phone_state(&phone_state_info);

  switch(phone_state_info.srv_system)
  {
    case CGPS_SRV_SYS_CDMA:
    case CGPS_SRV_SYS_HDR:
      if(TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable))
      {
        return TM_PRTL_TYPE_UMTS_UP;
      }
      return TM_PRTL_TYPE_1X_UP;
    case CGPS_SRV_SYS_GSM:
    case CGPS_SRV_SYS_WCDMA:
    case CGPS_SRV_SYS_GW:
      if ( mt_initiator == MT_NA ) /* not network-initiated */
      {
        if ( tm_core_info.config_info.u_mo_method == MO_UP )
          return TM_PRTL_TYPE_UMTS_UP;
        else
          return TM_PRTL_TYPE_UMTS_CP;
      }
      else /* network-initiated */
      {
        if ( mt_initiator == MT_UP )
          return TM_PRTL_TYPE_UMTS_UP;
        else
          return TM_PRTL_TYPE_UMTS_CP;
      }

    case CGPS_SRV_SYS_LTE:
      if ( mt_initiator == MT_NA ) /* not network-initiated */
        {
        if ( tm_core_info.config_info.u_mo_method == MO_UP )
          return TM_PRTL_TYPE_UMTS_UP;
        else
          return TM_PRTL_TYPE_NONE;
        }
      else /* network-initiated */
      {
        if ( mt_initiator == MT_UP )
        {
          return TM_PRTL_TYPE_UMTS_UP;
        }
        else if ((mt_initiator == MT_CP) && (tm_core_get_lpp_cp_enable() == TRUE))
        {
          return TM_PRTL_TYPE_LPP_CP;
        }
      }
      return TM_PRTL_TYPE_NONE;

    case CGPS_SRV_SYS_TDSCDMA:
      return TM_PRTL_TYPE_UMTS_UP; /* for now only SUPL for TDSCDMA */

    default:
      return TM_PRTL_TYPE_NONE;
  }
}

/*===========================================================================

FUNCTION tm_core_api_cb_timer_handler

DESCRIPTION
  This function is to handle api callbacks in TM

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_api_cb_timer_handler(void)
{
  tm_api_notifier_type api_notifier;

  api_notifier.pNotifierCb = NULL;
  api_notifier.clientIndex = 0;
  api_notifier.broadcast = TRUE;
    
  if (tm_cb_timer_counter < TM_CORE_MAX_NUM_API_CALLBACKS)
    {
    tm_core_api_callback_handler(&api_notifier);
  tm_cb_timer_counter++;
    }
  /* Timer expired, print the call back logs */
  else if (tm_cb_timer_counter == TM_CORE_MAX_NUM_API_CALLBACKS)
  {
    gnss_display_cb_logs(tm_call_back_log, (uint16)THREAD_ID_SM_TM);
  }
}

/*===========================================================================

FUNCTION tm_core_timer_handler

DESCRIPTION
  This function is to figure out which TM-Core timer has expired and call proper
  timer handler.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_timer_handler(const os_TimerExpiryType *timer_data)
{
  if(timer_data != NULL)
  {
    if((timer_data->q_TimerId & TM_CORE_TIMER_DISPATCH_MASK) == TM_CORE_TIMER_ID_RANGE)
    {
      switch(timer_data->q_TimerId)
      {
        case TM_CORE_TIMER_ID_API_CALLBACK:
          tm_core_api_cb_timer_handler();
        break;

        default:
          MSG_MED("Unknown timer expired", 0, 0, 0);
        break;
      }
    }
    else /* timer id range check */
    {
      MSG_ERROR("Can not proess invalid timer event", 0, 0, 0);
    }
  }
  else /* null pointer check */
  {
    MSG_ERROR("Can not proess null timer msg", 0, 0, 0);
  }
}

/*===========================================================================

FUNCTION tm_core_get_prtl_idx_by_timer_id

DESCRIPTION
  This function returns protocol array index based on timer ID

DEPENDENCIES

RETURN VALUE
  -1, if timer ID can not be found.
  Otherwise, protocol array index for the timer ID.

SIDE EFFECTS

===========================================================================*/
int tm_core_get_prtl_idx_by_timer_id(uint32 timer_id)
{
  switch(timer_id & TM_CORE_TIMER_DISPATCH_MASK)
  {
    case TM_STANDALONE_TIMER_ID_RANGE:
      return TM_CORE_PRTL_CB_ARR_INDEX_STANDALONE;

    case TM_1X_UP_TIMER_ID_RANGE:
      return TM_CORE_PRTL_CB_ARR_INDEX_1X_UP;

    case TM_1X_CP_TIMER_ID_RANGE:
      return TM_CORE_PRTL_CB_ARR_INDEX_1X_CP;

    case TM_UMTS_UP_TIMER_ID_RANGE:
      return TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP;

    case TM_UMTS_CP_TIMER_ID_RANGE:
      return TM_CORE_PRTL_CB_ARR_INDEX_UMTS_CP;

    case TM_DIAG_TIMER_ID_RANGE:
      return TM_CORE_PRTL_CB_ARR_INDEX_DIAG;

#ifdef FEATURE_GNSS_PERIODIC_PPM
    #error code not present
#endif
    case TM_LPP_CP_TIMER_ID_RANGE:
      return TM_CORE_PRTL_CB_ARR_INDEX_LPP_CP;

    default:
      return -1;
  }
}

/*===========================================================================

FUNCTION tm_core_timer_dispatcher

DESCRIPTION
  This function is to figure out which TM software sub-module the timer expiry event
  belongs to and call the proper timer handler (provided to TM-Core by each sub-module)

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_timer_dispatcher(void *timer_data)
{
  os_TimerExpiryType *timer_param = (os_TimerExpiryType *)timer_data;
  int prtlTableIndex;

  if(timer_param != NULL)
  {
    switch (timer_param->q_TimerId & TM_CORE_TIMER_DISPATCH_MASK)
    {
       case TM_CORE_TIMER_ID_RANGE:
         tm_core_timer_handler(timer_param);
         break;

       case TM_PDAPI_TIMER_ID_RANGE:
         //  pdApi_TimerHandler(timer_param);
         break;

       case TM_CM_IFACE_TIMER_ID_RANGE:
         tm_cm_handle_timer_expiry(timer_param);
         break;

#ifdef FEATURE_TM_SECURITY
       case TM_SEC_LAYER_TIMER_ID_RANGE:
         tm_sec_handle_timer_expiry(timer_param);
         break;
#endif /* FEATURE_TM_SECURITY */

#ifdef FEATURE_CGPS_XTRA
       case TM_XTRA_TIMER_ID_RANGE:
         tm_xtra_handle_timer_expiry(timer_param);
         break;
#endif /* FEATURE_CGPS_XTRA */

       case TM_SYSD_TIMER_ID_RANGE:
         tm_sysd_handle_timer_expiry(timer_param);
         break;

       case TM_XSPI_ID_RANGE:
         tm_xspi_handle_timer_expiry(timer_param);
         break;

       case TM_STANDALONE_TIMER_ID_RANGE:
       case TM_1X_UP_TIMER_ID_RANGE:
       case TM_1X_CP_TIMER_ID_RANGE:
       case TM_UMTS_UP_TIMER_ID_RANGE:
       case TM_UMTS_CP_TIMER_ID_RANGE:
       case TM_DIAG_TIMER_ID_RANGE:
       case TM_LPP_CP_TIMER_ID_RANGE:
         /* Timer for protocol sub-modules */
         prtlTableIndex = tm_core_get_prtl_idx_by_timer_id(timer_param->q_TimerId);

         if ((prtlTableIndex >= 0) && (prtlTableIndex < TM_PRTL_NUM))
         {
           if (tm_core_info.prtl_func_cb_table[prtlTableIndex].timer_cb_fp != NULL)
           {
             tm_core_info.prtl_func_cb_table[prtlTableIndex].timer_cb_fp(timer_data);
           }
         }
         break;

#ifdef FEATURE_GNSS_PERIODIC_PPM
       #error code not present
#endif

       case TM_CORE_TIMER_ID_DELETE_THROTTLE_ID:
         MSG_LOW("DELETE timer expired", 0, 0, 0);
         break;

      default:
        MSG_ERROR("Can not find proper timer handler", 0, 0, 0);
      break;
    }
  }
  else
  {
    MSG_ERROR("Can not dispatch null timer msg", 0, 0, 0);
  }
  return;
}
/*===========================================================================

FUNCTION tm_core_session_active

DESCRIPTION
  This function tells if Tm-Core is in any active PD session (MI or MT).

  If there is an active PD on-demand session, we return FALSE

  If there is an active Periodic PPM session, we return FALSE

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_session_active(void)
{
  /* check if there is a GPS session or on-demand session is active*/
  if(   (tm_core_info.session_info.session_state == TM_CORE_SESS_STATE_INIT)
     || (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_ON_DEMAND)
    )
  {
    return FALSE;
  }
  else
  {
    return TRUE; /*lint !e506 */
  }
}

/*===========================================================================

FUNCTION tm_core_session_ok_to_start_new_session

DESCRIPTION
  This function tells if Tm-Core is willing to start a new PD session

  If there is an already existing session, this function will return FALSE
  EXCEPT in the case of:~
    If there is an active PD on-demand session, we return FALSE
    If there is an active Periodic PPM session, we return FALSE

  A PD session will always preempt these two types of sessions.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_session_ok_to_start_new_session(void)
{
  /* check if there is a GPS session or on-demand session is active*/
  if(    (tm_core_info.session_info.session_state == TM_CORE_SESS_STATE_INIT)
      || (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_ON_DEMAND)
     )
  {
    return TRUE;
  }
  else
  {
    return FALSE; /*lint !e506 */
  }
}


/*===========================================================================

FUNCTION tm_core_translate_end_status

DESCRIPTION
  This function translate internal abort reason to external end status.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
pdsm_pd_end_e_type tm_core_translate_end_status
(
  tm_sess_stop_reason_e_type  abort_reason
)
{
  pdsm_pd_end_e_type ret_val = PDSM_PD_END_SESS_NONE;

  switch(abort_reason)
  {
    case TM_STOP_REASON_E911:
      ret_val = PDSM_PD_END_E911;
      break;

    case TM_STOP_REASON_USER_TERMINATED:
      ret_val = PDSM_PD_END_CLIENT_END;
      break;

    case TM_STOP_REASON_PRM_TIMEOUT:
    case TM_STOP_REASON_PPM_TIMEOUT:
    case TM_STOP_REASON_FIX_TIMEOUT:
      ret_val = PDSM_PD_END_TIMEOUT;
      break;
    case TM_STOP_REASON_GENERAL_ERROR:
      ret_val = PDSM_PD_END_FIX_ERROR;
      break;

    case TM_STOP_REASON_MPC_REJECTED:
      ret_val = PDSM_PD_END_VX_AUTH_FAIL;
      break;

    case TM_STOP_REASON_PDE_NO_CON:
      ret_val = PDSM_PD_END_NO_CON;
      break;
    case TM_STOP_REASON_PDE_CON_FAIL:
      ret_val = PDSM_PD_END_CON_FAIL;
      break;
    case TM_STOP_REASON_PDE_REJECT:
      ret_val = PDSM_PD_END_PDE_REJECT;
      break;
    case TM_STOP_REASON_STALE_BS_INFO:
      ret_val = PDSM_PD_END_STALE_BS_INFO;
      break;
    case TM_STOP_REASON_PPM_POST_FIX_TIMEOUT:
      ret_val = PDSM_PD_END_PPM_POST_FIX_TIMEOUT;
      break;
    case TM_STOP_REASON_PPM_PRE_FIX_TIMEOUT:
      ret_val = PDSM_PD_END_PPM_PRE_FIX_TIMEOUT;
      break;
    case TM_STOP_REASON_IS801_SERVER_TIMEOUT:
      ret_val = PDSM_PD_END_IS801_SERVER_TIMEOUT;
      break;
    case TM_STOP_REASON_IS801_PAYLOAD_ERROR:
      ret_val = PDSM_PD_END_IS801_PAYLOAD_ERROR;
      break;
    case TM_STOP_REASON_IS801_SEND_FAILURE:
      ret_val = PDSM_PD_END_IS801_SEND_FAILURE;
      break;
    case TM_STOP_REASON_PDE_END_SESSION:
      ret_val = PDSM_PD_END_PDE_END_SESSION;
      break;

    /*------------------------------------------------------------------
        JGPS specific end status reason
    ------------------------------------------------------------------*/
    case TM_STOP_REASON_JGPS_POSRESP_NG_LIS:
      ret_val = PDSM_PD_END_JGPS_POSRESP_NG_LIS;
      break;

    case TM_STOP_REASON_JGPS_POSRESP_NG_LSU:
      ret_val = PDSM_PD_END_JGPS_POSRESP_NG_LSU;
      break;

    case TM_STOP_REASON_JGPS_POSRESP_NG_MSI:
      ret_val = PDSM_PD_END_JGPS_POSRESP_NG_MSI;
      break;

    case TM_STOP_REASON_JGPS_STOPPOS_NG_LIS:
      ret_val = PDSM_PD_END_JGPS_STOPPOS_NG_LIS;
      break;

    case TM_STOP_REASON_JGPS_STOPPOS_NG_MSI:
      ret_val = PDSM_PD_END_JGPS_STOPPOS_NG_MSI;
      break;

    case TM_STOP_REASON_JGPS_POSCOMP_NG:
      ret_val = PDSM_PD_END_JGPS_POSCOMP_NG;
      break;

    case TM_STOP_REASON_JGPS_TIMEOUT_T04:
      ret_val = PDSM_PD_END_JGPS_TIMEOUT_T04;
      break;

    case TM_STOP_REASON_JGPS_TIMEOUT_STOPPOS:
      ret_val = PDSM_PD_END_JGPS_TIMEOUT_STOPPOS;
      break;

    case TM_STOP_REASON_JGPS_TIMEOUT_T02:
      ret_val = PDSM_PD_END_JGPS_TIMEOUT_T02;
      break;

    case TM_STOP_REASON_JGPS_TIMEOUT_IS801:
      ret_val = PDSM_PD_END_JGPS_TIMEOUT_IS801;
      break;

    case TM_STOP_REASON_JGPS_IS801_LRREJ:      /* LR Reject  reception */
    case TM_STOP_REASON_JGPS_IS801_AAREJ:      /* AA Reject  reception */
    case TM_STOP_REASON_JGPS_IS801_EPHREJ:     /* EPH Reject  reception */
    case TM_STOP_REASON_JGPS_IS801_ALMREJ:     /* ALM Reject  reception */
    case TM_STOP_REASON_JGPS_IS801_SEEDREJ:    /* SEED Reject  reception */
      ret_val = PDSM_PD_END_JGPS_IS801_LRREJ;
      break;

    case TM_STOP_REASON_JGPS_IS801_AA_FAIL:
      ret_val = PDSM_PD_END_JGPS_IS801_AA_FAIL;
      break;

    case TM_STOP_REASON_JGPS_PPP:
      ret_val = PDSM_PD_END_JGPS_PPP;
      break;

    case TM_STOP_REASON_JGPS_TCP_MS:
      ret_val = PDSM_PD_END_JGPS_TCP_MS;
      break;

    case TM_STOP_REASON_JGPS_TCP_SERV:
      ret_val = PDSM_PD_END_JGPS_TCP_SERV;
      break;

    case TM_STOP_REASON_JGPS_ACQRESP_NG_LIS:
      ret_val = PDSM_PD_END_JGPS_ACQRESP_NG_LIS;
      break;

    case TM_STOP_REASON_JGPS_ACQRESP_NG_LSU:
      ret_val = PDSM_PD_END_JGPS_ACQRESP_NG_LSU;
      break;

    case TM_STOP_REASON_JGPS_ACQRESP_NG_MSI:
      ret_val = PDSM_PD_END_JGPS_ACQRESP_NG_MSI;
      break;

    case TM_STOP_REASON_JGPS_STOPACQ_NG_LIS:
      ret_val = PDSM_PD_END_JGPS_STOPACQ_NG_LIS;
      break;

    case TM_STOP_REASON_JGPS_STOPACQ_NG_MSI:
      ret_val = PDSM_PD_END_JGPS_STOPACQ_NG_MSI;
      break;

    case TM_STOP_REASON_JGPS_TIMEOUT_T20:
      ret_val = PDSM_PD_END_JGPS_TIMEOUT_T20;
      break;

    case TM_STOP_REASON_JGPS_TIMEOUT_STOPACQ:
      ret_val = PDSM_PD_END_JGPS_TIMEOUT_STOPACQ;
      break;

    case  TM_STOP_REASON_SS_MOLR_ERRORS_SYSTEM_FAILURE:
      ret_val = PDSM_PD_END_SS_MOLR_ERRORS_SYSTEM_FAILURE; /* mapped to corresponding SS-molr-error error code */
    break;
    case  TM_STOP_REASON_SS_MOLR_ERRORS_UNEXPECTED_DATA_VALUE:
      ret_val = PDSM_PD_END_SS_MOLR_ERRORS_UNEXPECTED_DATA_VALUE;
      break;
    case  TM_STOP_REASON_SS_MOLR_ERRORS_DATA_MISSING:
      ret_val  = PDSM_PD_END_SS_MOLR_ERRORS_DATA_MISSING;
      break;
    case  TM_STOP_REASON_SS_MOLR_ERRORS_FACILITY_NOT_SUPPORTED:
      ret_val  = PDSM_PD_END_SS_MOLR_ERRORS_FACILITY_NOT_SUPPORTED;
      break;
    case  TM_STOP_REASON_SS_MOLR_ERRORS_SS_SUBSCRIPTION_VIOLATION:
      ret_val  = PDSM_PD_END_SS_MOLR_ERRORS_SS_SUBSCRIPTION_VIOLATION;
      break;
    case  TM_STOP_REASON_SS_MOLR_ERRORS_POSITION_METHOD_FAILURE:
      ret_val  = PDSM_PD_END_SS_MOLR_ERRORS_POSITION_METHOD_FAILURE;
      break;
    case  TM_STOP_REASON_SS_MOLR_ERRORS_UNDEFINED:
      ret_val  = PDSM_PD_END_SS_MOLR_ERRORS_UNDEFINED;
      break;

    case TM_STOP_REASON_ON_RESET_LOCATION_SERVICE:
      ret_val = PDSM_PD_END_RESET_LOCATION_SERVICE;
      break;
    default:
      ret_val = PDSM_PD_END_SESS_NONE;
      break;
  }


  /* Translate JGPS specific error codes */
/*lint -e{56} valid types */
/*lint -e{58} valid types */
  if (    abort_reason >= TM_STOP_REASON_JGPS_DMSB_ERROR
       && abort_reason <= (TM_STOP_REASON_JGPS_DPOSCOMP_NG
                            + PDSM_PD_DLOAD_EPHEMERIS
                            + PDSM_PD_DLOAD_ALMANAC
                            + PDSM_PD_DLOAD_LOCATION)
     )
  {
    ret_val = (pdsm_pd_end_e_type)abort_reason;
  }

  return ret_val;
}

/*===========================================================================

FUNCTION tm_core_abort_recover

DESCRIPTION
  This function performs TM-Core software recovery.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_abort_recover
(
  boolean                       ask_prtl,
  tm_sess_stop_reason_e_type    abort_reason
)
{
  lm_session_request_info_s_type lm_sess_req_param;
  int                            prtlTableIndex = -1;
  boolean                        prtl_abort_session = TRUE;
  pdsm_client_id_type            client_id;
  prtl_event_u_type              event_payload;
  tm_pdapi_pd_event_s_type       *pz_pd_event_type = NULL;
  boolean                        fire_event;
  boolean                        fire_nmea                = TRUE;
  lm_request_info_s_type               lm_info_req_param;
  pdsm_client_type_e_type           client_type;
  sysd_cntrl_func_packet_type sysd_cntrl_payload;
  tm_cm_phone_state_info_s_type phone_state_info;
  pdsm_pd_end_e_type            e_end_status = PDSM_PD_END_SESS_NONE;

  /* Init */
  pz_pd_event_type = (tm_pdapi_pd_event_s_type *)os_MemAlloc(sizeof(tm_pdapi_pd_event_s_type), OS_MEM_SCOPE_TASK);
  if (NULL == pz_pd_event_type)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Cannot allocate memory for tm_pdapi_pd_event_s_type in tm_core_abort_recover");
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Falling back on stack memory");
  }
  else
  {
    memset(pz_pd_event_type, 0, sizeof(*pz_pd_event_type));
  }
  tm_cm_iface_get_phone_state(&phone_state_info);

  sysd_cntrl_payload.event = SYSD_CNTRL_EVT_SESS_ABORT;
  sysd_cntrl_payload.cmd.abort_reason = abort_reason; // Check if sysd_cntrl_payload.cmd.pdsm_cmd_type is being used in handler below - jdoshi
  if (tm_core_info.session_info.active_sess_handle != TM_SESS_HANDLE_ON_DEMAND &&
      tm_core_info.session_info.active_sess_handle != TM_SESS_HANDLE_PERIODIC_PPM)
  {
    tm_sysd_controller_handler(&sysd_cntrl_payload);
  }

  MSG_HIGH("TM-Core - Abort and Recover. Reason: %u, Ask Prtl: %u", abort_reason, ask_prtl, 0);

#ifdef FEATURE_CGPS_XTRA
  /* Handle the abort reason for XTRA sessions */
  tm_xtra_handle_core_abort_recover(abort_reason);
#endif /* FEATURE_CGPS_XTRA */


  /* stop on-demand timers */
  tm_core_on_demand_abort(abort_reason);

  /* Check if tm_core is in active state or if on-demand session is on or if we got a set params
  for delete parms abort the session */
  if( tm_core_session_active()          ||
      tm_core_is_on_demand_session_on() ||
      abort_reason == TM_STOP_REASON_ON_DEMAND_DELETE_PARAMS)
  {
    prtlTableIndex = tm_core_get_prtl_idx_by_type(tm_core_info.session_info.from_protocol);

    if(((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM)) ||
       (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_E911 ))
    {

      /* If an abort request need a protocol sub-modules' consent, only proceed if a protocol sub-moulde allows it */
      if((ask_prtl) && (tm_core_info.session_info.active_sess_handle != TM_SESS_HANDLE_E911))
      {
        /* Set reason to general error for now, if return value */
        if(tm_core_info.prtl_func_cb_table[prtlTableIndex].stop_sess_req_fp)
        {
          prtl_abort_session =
            tm_core_info.prtl_func_cb_table[prtlTableIndex].stop_sess_req_fp(
                                   tm_core_info.session_info.active_sess_handle,
                                   PRTL_SESS_STOP_SINGLE_FIX,
                                   abort_reason);

          if(prtl_abort_session == TRUE)
          {
            MSG_MED("Protocol aborted the session", 0, 0, 0);
          }
          else
          {
            MSG_MED("Protocol continue with the session", 0, 0, 0);
          }
        }
        else
        {
          MSG_ERROR("No protocol callback installed 2", 0, 0, 0);                      
        }
      }  /* if(ask_prtl == TRUE)  */
      else if ((ask_prtl == TRUE) &&
               (phone_state_info.e911_state != TM_CM_E911_STATE_NONE) &&
               (phone_state_info.srv_system == CGPS_SRV_SYS_WCDMA) &&
               (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_E911) &&
               (abort_reason != TM_STOP_REASON_LTE_TO_UMTS_CSFB))
      {
        /* Sending a callback to SUPL to replace the certificate in the cache with
              the one sent in the set_parameters request */
        MSG_MED("Aborting WCDMA CP emergency session reason %d", abort_reason, 0 ,0);
        prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_UMTS_CP);
        if(tm_core_info.prtl_func_cb_table[prtlTableIndex].stop_sess_req_fp)
        {
          prtl_abort_session =
              tm_core_info.prtl_func_cb_table[prtlTableIndex].stop_sess_req_fp(
                                     tm_core_info.session_info.active_sess_handle,
                                     PRTL_SESS_STOP_SINGLE_FIX,
                                     abort_reason);

          if(prtl_abort_session == TRUE)
          {
            MSG_MED("Protocol aborted the session", 0, 0, 0);
          }
        }
      }
      else
      {
        tm_core_update_session_state(TM_CORE_SESS_STATE_RESOURCE_CLEANUP);

        fire_event = tm_core_info.session_info.session_event_firing;
        fire_nmea  = TRUE;


        if(tm_core_is_internal_session_on())
        {

          fire_event = FALSE;
          fire_nmea  = FALSE;

          /* Notify the GM if the HLOS has ended the session ( USER_TERMINATED ) or
             if the HLOS has started a new session in which case the internal session is terminated (ABORT_INTERNAL_SESSION)*/
          if((abort_reason == TM_STOP_REASON_USER_TERMINATED )||
             (abort_reason == TM_STOP_REASON_ABORT_INTERNAL_SESSION) ||
             (abort_reason == TM_STOP_REASON_INTERNAL_SESSION_DELETE_PARAM))
          {
            MSG_MED("Sending abort reason %d to GM ", abort_reason, 0, 0);
            gm_position_fix_request_fail(GEOFENCE_POS_FIX_SRC_GNSS);
          }
        }

        if (NULL != pz_pd_event_type)
        {
          pz_pd_event_type->pd_event_data.pd_info_type.client_id = tm_core_info.session_info.pd_param.client_id;
          pz_pd_event_type->pd_event_data.pd_info_type.pd_info.end_status = PDSM_PD_END_SESS_NONE;

          /* Fire End event if fix does not complete successfully */
          if( (abort_reason != TM_STOP_REASON_COMPLETED) &&
              (fire_event == TRUE)
            )
          {
            if(tm_core_info.session_info.pd_param.pd_option.session == PDSM_PD_SESS_TYPE_DATA_DLOAD)
            {
              pz_pd_event_type->e_pd_event = PDSM_PD_DLOAD_EVENT_END;
            }
            else
            {
              pz_pd_event_type->e_pd_event = PDSM_PD_EVENT_END;
            }
            pz_pd_event_type->pd_event_data.pd_info_type.pd_info.end_status = tm_core_translate_end_status(abort_reason);
            tm_pdapi_pd_event_callback(pz_pd_event_type, TM_PDAPI_SESS_TYPE_NONE);
          }
          e_end_status = pz_pd_event_type->pd_event_data.pd_info_type.pd_info.end_status;
        }
        else
        {
          tm_pdapi_pd_event_s_type       z_pd_event_type = {0};

          z_pd_event_type.pd_event_data.pd_info_type.client_id = tm_core_info.session_info.pd_param.client_id;
          z_pd_event_type.pd_event_data.pd_info_type.pd_info.end_status = PDSM_PD_END_SESS_NONE;
          
          /* Fire End event if fix does not complete successfully */
          if( (abort_reason != TM_STOP_REASON_COMPLETED) &&
               (fire_event == TRUE)
             )
          {
            if(tm_core_info.session_info.pd_param.pd_option.session == PDSM_PD_SESS_TYPE_DATA_DLOAD)
            {
              z_pd_event_type.e_pd_event = PDSM_PD_DLOAD_EVENT_END;
            }
            else
            {
              z_pd_event_type.e_pd_event = PDSM_PD_EVENT_END;
            }
            z_pd_event_type.pd_event_data.pd_info_type.pd_info.end_status = tm_core_translate_end_status(abort_reason);
            tm_pdapi_pd_event_callback(&z_pd_event_type, TM_PDAPI_SESS_TYPE_NONE);
          }
          e_end_status = z_pd_event_type.pd_event_data.pd_info_type.pd_info.end_status;
        }
        lm_sess_req_param.e_req_type = LM_SESSION_REQUEST_STOP;
        lm_sess_req_param.q_handle   = tm_core_info.session_info.active_sess_handle;
        client_type = pdsmclient_get_client_type_map(tm_core_info.session_info.pd_param.client_id);

        /* if abort reason is delete parms, check if on demand is enabled. we need to force the receiver
        to go off before honoring the delete parms.
        if the user terminated the session, check if the client type is OEM. we
        need to turn off the rcvr off immediately */
        if ((abort_reason == TM_STOP_REASON_ON_DEMAND_DELETE_PARAMS &&
            (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_ON_DEMAND)) ||
              ((abort_reason == TM_STOP_REASON_USER_TERMINATED ) &&
               (client_type == PDSM_CLIENT_TYPE_OEM)))
        {
          lm_sess_req_param.q_req_mask = (uint32)LM_SESSION_REQUEST_FIX;
          lm_sess_req_param.z_request.z_fix_request.u_force_mgp_off = TRUE;
          (void)lm_session_request(&lm_sess_req_param);
        }
        /* When we receive an abort from the protocols with ask_prtl as FALSE , we
           kill the app tracking logic in LM if requested */
        else if((abort_reason == TM_STOP_REASON_USER_TERMINATED) &&
                tm_core_info.session_info.receiver_off_needed)
        {
          lm_sess_req_param.q_req_mask = (uint32)LM_SESSION_REQUEST_FIX;
          lm_sess_req_param.z_request.z_fix_request.u_force_mgp_off = FALSE;
          (void)lm_session_request(&lm_sess_req_param);
        }
        /* Clean up the session when we do CSFB from LTE to UMTS */
        else if((TM_STOP_REASON_LTE_TO_UMTS_CSFB == abort_reason) &&
                tm_core_info.session_info.receiver_off_needed)
        {
          lm_sess_req_param.q_req_mask = (uint32)LM_SESSION_REQUEST_FIX;
          lm_sess_req_param.z_request.z_fix_request.u_force_mgp_off = FALSE;
          (void)lm_session_request(&lm_sess_req_param);
        }
        /* for OTDOA only, LM is not involved. No need to start LM session */
        if ((tm_core_info.session_info.op_mode != TM_SESS_OPERATION_MODE_OTDOA ) ||
            (tm_core_info.session_info.op_mode  == TM_SESS_OPERATION_MODE_OTDOA &&
             tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_UMTS_UP_NI))
        {
        lm_sess_req_param.q_req_mask = (uint32)LM_SESSION_REQUEST_SESSION;
        (void) lm_session_request(&lm_sess_req_param);
        }

        /*If any of the NMEA strings are enabled, generate the proprietary NMEA message*/
        if((TM_CORE_CONFIG_DD(nmea_output_format) != 0) &&
           (tm_core_info.config_info.nmea_config_type.pd_nmea_reporting_type == PDSM_PA_NMEA_REPORT_AT_1HZ_INTERVAL) &&
           (fire_nmea == TRUE) )
        {
          /*Generate PSTIS NMEA string indicating session start*/
          if (TM_CORE_CONFIG_DD(nmea_output_format) & PDAPI_NMEA_PSTIS)
          {
            tm_nmea_pstis_put();
          }
        }

        /*Cleanup the cached MEAS inforation*/
        z_cached_meas_info.cached_meas_valid = FALSE;
        memset((void *)&z_cached_meas_info, 0, sizeof(tm_core_cached_meas_s_type));
        memset((void *)&z_cached_ref_pos_info, 0, sizeof(tm_core_cached_ref_pos_s_type));

          /* Fire EVENT_GPS_PD_FIX_END */
        sm_report_event_gps_fix_end(e_end_status);

        client_id  = tm_core_info.session_info.pd_param.client_id;

        /* Go back to clean state */
        tm_core_info.session_info.active_sess_handle      = TM_SESS_HANDLE_NONE;
        tm_core_info.session_info.op_mode                 = TM_SESS_OPERATION_MODE_NONE;
        tm_core_info.session_info.op_requested            = TM_OP_REQ_NONE;
        tm_core_info.session_info.from_protocol           = TM_PRTL_TYPE_NONE;
        tm_core_info.session_info.pd_param.client_type    = PDSM_CLIENT_TYPE_NONE;
        tm_core_info.session_info.pd_param.client_id      = -1;
        tm_core_info.session_info.session_event_firing    = TRUE;
        tm_core_info.session_info.eph_svmask              = 0;
        tm_core_info.session_info.alm_svmask              = 0;
        tm_core_info.session_info.msb_xtra_allowed        = FALSE;
        tm_core_info.session_info.req_xtra_trig_by_agps   = FALSE;
        tm_core_info.session_info.end_odp_session         = FALSE;
        tm_core_info.session_info.force_eph_download      = FALSE;
        tm_core_info.session_info.glo_force_eph_download  = FALSE;
        tm_core_info.session_info.bds_force_eph_download  = FALSE;
        tm_core_info.session_info.gal_force_eph_download  = FALSE;
        tm_core_info.session_info.eph_need_sv_mask = 0;
        tm_core_info.session_info.glo_eph_need_sv_mask = 0;
        tm_core_info.session_info.bds_eph_need_sv_mask = 0;
        tm_core_info.session_info.gal_eph_need_sv_mask = 0;
        tm_core_info.session_info.sess_info_valid_flags = 0;

        tm_core_info.session_info.q_ReqClientMask         = SM_GPS_STATE_INFO_REQ_TYPE_NONE;
        tm_core_info.session_info.pos_inj_requested       = FALSE;
        tm_core_info.session_info.receiver_off_needed     = FALSE;
        tm_core_info.session_info.iono                    = 1; /* iono invalid, 1 means MGP has
                                                               requested iono from AGPS */
        tm_core_info.session_info.utc                     = 1; /* utc invalid, 1 means MGP has
                                                               requested utc from AGPS */
        tm_core_info.session_info.health_svmask           = 0;
        tm_core_info.session_info.valid_svmask            = 0;
        tm_core_info.session_info.sbas_eph_svmask         = 0;
        tm_core_info.session_info.sbas_alm_svmask         = 0;
        tm_core_info.session_info.glo_eph_svmask          = 0;
        tm_core_info.session_info.glo_alm_svmask          = 0;
        tm_core_info.session_info.bds_eph_svmask          = 0;
        tm_core_info.session_info.bds_alm_svmask          = 0;
        tm_core_info.session_info.gal_eph_svmask          = 0;
        tm_core_info.session_info.gal_alm_svmask          = 0;

        tm_core_info.internal_session = FALSE;

        tm_core_update_session_state(TM_CORE_SESS_STATE_INIT);

        /* Fire Done event */
        if (NULL != pz_pd_event_type)
        {
          if(TRUE == fire_event)
          {
            pz_pd_event_type->pd_event_data.pd_info_type.client_id = client_id;
            if(PDSM_PD_SESS_TYPE_DATA_DLOAD == tm_core_info.session_info.pd_param.pd_option.session)
            {
              pz_pd_event_type->e_pd_event = PDSM_PD_EVENT_DLOAD_DONE;
              tm_pdapi_pd_event_callback(pz_pd_event_type, TM_PDAPI_SESS_TYPE_NONE);
            }
            else
            {
              pz_pd_event_type->e_pd_event = PDSM_PD_EVENT_DONE;
              tm_pdapi_pd_event_callback(pz_pd_event_type, TM_PDAPI_SESS_TYPE_NONE);

              /* Check prtl_func_cb_table bounds */
              if ((prtlTableIndex >=0) && (prtlTableIndex < TM_PRTL_NUM))
              {
                /* This is added for internal Vx NI client */
                if(NULL != tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp)
                {
                  event_payload.pd_event_payload.pd_event = PDSM_PD_EVENT_DONE;
                  (void) tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(PRTL_EVENT_TYPE_PD,
                                                                          &event_payload);
                }
              }
              else
              {
                MSG_ERROR("Attempt for an out of bound access to prtl_func_cb_table with index = %d", prtlTableIndex, 0, 0);
              }
            }

          }
          if( (TRUE == fire_event) ||
              (TM_STOP_REASON_INTERNAL_SESSION_DELETE_PARAM == abort_reason)
            )
          {          
            /*Send Done event to GM core as well*/
            pz_pd_event_type->pd_event_data.pd_info_type.client_id = client_id;
            pz_pd_event_type->e_pd_event = PDSM_PD_EVENT_DLOAD_DONE;
            gm_update_pd_event(pz_pd_event_type->e_pd_event);
          }
        }
        else
        {
          tm_pdapi_pd_event_s_type       z_pd_event_type = {0};
          
          if(TRUE == fire_event)
          {
            z_pd_event_type.pd_event_data.pd_info_type.client_id = client_id;
            if(PDSM_PD_SESS_TYPE_DATA_DLOAD == tm_core_info.session_info.pd_param.pd_option.session)
            {
              z_pd_event_type.e_pd_event = PDSM_PD_EVENT_DLOAD_DONE;
              tm_pdapi_pd_event_callback(&z_pd_event_type, TM_PDAPI_SESS_TYPE_NONE);
            }
            else
            {
              z_pd_event_type.e_pd_event = PDSM_PD_EVENT_DONE;
              tm_pdapi_pd_event_callback(&z_pd_event_type, TM_PDAPI_SESS_TYPE_NONE);

              /* Check prtl_func_cb_table bounds */
              if ((prtlTableIndex >=0) && (prtlTableIndex < TM_PRTL_NUM))
              {
                /* This is added for internal Vx NI client */
                if(tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL)
                {
                  event_payload.pd_event_payload.pd_event = PDSM_PD_EVENT_DONE;
                  (void) tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(PRTL_EVENT_TYPE_PD,
                                                                            &event_payload);
                }
              }
              else
              {
                MSG_ERROR("Attempt for an out of bound access to prtl_func_cb_table with index = %d", prtlTableIndex, 0, 0);
              }
            }
          }
          if( (fire_event == TRUE) ||
              (abort_reason == TM_STOP_REASON_INTERNAL_SESSION_DELETE_PARAM)
            )
          {          
            /*Send Done event to GM core as well*/
            z_pd_event_type.pd_event_data.pd_info_type.client_id = client_id;
            z_pd_event_type.e_pd_event = PDSM_PD_EVENT_DLOAD_DONE;
            gm_update_pd_event(z_pd_event_type.e_pd_event);
          }        
        }

        if(tm_core_info.session_info.queued_request == TRUE)
        {
          MSG_MED("Resume pending get position fix request", 0, 0 ,0);
          (void) tm_core_resume_get_pos();
        }
      } /* If(ask_prtl == FALSE) */
    }
  }
  else
  {
    MSG_HIGH("TM-Core not in active state", 0, 0, 0);

    /* send the end session to all protocols. for UMTS-CP, we need to reset the allow-rrc bit in the cases
           where we havnet received the MC yet for either 3rd party mo-lr or MT sessions*/
    for (prtlTableIndex =0; prtlTableIndex < TM_CORE_PRTL_CB_ARR_INDEX_MAX; prtlTableIndex++)
    {

      /* If an abort request is coming from the user, send it to protocol modules */
      if (abort_reason == TM_STOP_REASON_USER_TERMINATED || abort_reason == TM_STOP_REASON_ON_RESET_LOCATION_SERVICE)
      {
        /* Set reason to general error for now, if return value */
        if(tm_core_info.prtl_func_cb_table[prtlTableIndex].stop_sess_req_fp)
        {
          prtl_abort_session = tm_core_info.prtl_func_cb_table[prtlTableIndex].stop_sess_req_fp(tm_core_info.session_info.active_sess_handle,
                                                                                                PRTL_SESS_STOP_SINGLE_FIX,
                                                                                                abort_reason);
          if(prtl_abort_session == TRUE)
          {
            MSG_MED("Protocol aborted the session", 0, 0, 0);
          }
          else
          {
            MSG_MED("Protocol continue with the session", 0, 0, 0);
          }
        }
        else
        {
          MSG_ERROR("No protocol callback installed 3", 0, 0, 0);                      
        }
      }
    }
    if (abort_reason == TM_STOP_REASON_USER_TERMINATED || abort_reason == TM_STOP_REASON_ON_RESET_LOCATION_SERVICE)
    {
      /* inform LM to turn off the receiver if user sent an end session */
      /*Do Force Receiver off only if OEM client. Else to normal receiver off*/
      if(pdsmclient_get_client_id_map(tm_core_info.session_info.pd_param.client_id) == PDSM_CLIENT_TYPE_OEM)
      {
        lm_info_req_param.e_req_type = LM_REQUEST_FORCE_RECVR_OFF;
      }
      else
      {     
        lm_info_req_param.e_req_type = LM_REQUEST_RECVR_OFF;
      }

      if ((tm_core_info.session_info.op_mode != TM_SESS_OPERATION_MODE_OTDOA) ||
            (tm_core_info.session_info.op_mode  == TM_SESS_OPERATION_MODE_OTDOA &&
             tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_UMTS_UP_NI)) 
      {
        (void)lm_request(&lm_info_req_param);
      }

      if (NULL != pz_pd_event_type)
      {
        pz_pd_event_type->pd_event_data.pd_info_type.client_id = tm_core_info.session_info.pd_param.client_id;
        if(tm_core_info.session_info.pd_param.pd_option.session == PDSM_PD_SESS_TYPE_DATA_DLOAD)
        {
          pz_pd_event_type->e_pd_event = PDSM_PD_DLOAD_EVENT_END;
        }
        else
        {
          pz_pd_event_type->e_pd_event = PDSM_PD_EVENT_END;
        }
        pz_pd_event_type->pd_event_data.pd_info_type.pd_info.end_status = tm_core_translate_end_status(abort_reason);
        tm_pdapi_pd_event_callback(pz_pd_event_type, TM_PDAPI_SESS_TYPE_NONE);
      }
      else
      {
        tm_pdapi_pd_event_s_type       z_pd_event_type = {0};
        
        z_pd_event_type.pd_event_data.pd_info_type.client_id = tm_core_info.session_info.pd_param.client_id;
        if(tm_core_info.session_info.pd_param.pd_option.session == PDSM_PD_SESS_TYPE_DATA_DLOAD)
        {
          z_pd_event_type.e_pd_event = PDSM_PD_DLOAD_EVENT_END;
        }
        else
        {
          z_pd_event_type.e_pd_event = PDSM_PD_EVENT_END;
        }      
        z_pd_event_type.pd_event_data.pd_info_type.pd_info.end_status = tm_core_translate_end_status(abort_reason);
        tm_pdapi_pd_event_callback(&z_pd_event_type, TM_PDAPI_SESS_TYPE_NONE);
      }
    }

    /*If any of the NMEA strings are enabled, generate the proprietary NMEA message*/
    if((TM_CORE_CONFIG_DD(nmea_output_format) != 0) &&
     (tm_core_info.config_info.nmea_config_type.pd_nmea_reporting_type == PDSM_PA_NMEA_REPORT_AT_1HZ_INTERVAL))
    {
      /*Generate PSTIS NMEA string indicating session start*/
      if (TM_CORE_CONFIG_DD(nmea_output_format) & PDAPI_NMEA_PSTIS)
      {
        tm_nmea_pstis_put();
      }
    }

    /*Cleanup the cached MEAS inforation*/
    z_cached_meas_info.cached_meas_valid = FALSE;
    memset((void *)&z_cached_meas_info, 0, sizeof(tm_core_cached_meas_s_type));
    memset((void *)&z_cached_ref_pos_info, 0, sizeof(tm_core_cached_ref_pos_s_type));

    /* Go back to clean state */
    tm_core_info.session_info.active_sess_handle      = TM_SESS_HANDLE_NONE;
    tm_core_info.session_info.op_mode                 = TM_SESS_OPERATION_MODE_NONE;
    tm_core_info.session_info.op_requested            = TM_OP_REQ_NONE;
    tm_core_info.session_info.from_protocol           = TM_PRTL_TYPE_NONE;
    tm_core_info.session_info.pd_param.client_type    = PDSM_CLIENT_TYPE_NONE;
    tm_core_info.session_info.pd_param.client_id      = -1;
    tm_core_info.session_info.eph_svmask              = 0;
    tm_core_info.session_info.alm_svmask              = 0;
    tm_core_info.session_info.q_ReqClientMask         = SM_GPS_STATE_INFO_REQ_TYPE_NONE;
    tm_core_info.session_info.msb_xtra_allowed        = FALSE;
    tm_core_info.session_info.req_xtra_trig_by_agps   = FALSE;
    tm_core_info.session_info.receiver_off_needed     = FALSE;
    tm_core_info.session_info.end_odp_session         = FALSE;
    tm_core_info.session_info.force_eph_download      = FALSE;
    tm_core_info.session_info.glo_force_eph_download  = FALSE;
    tm_core_info.session_info.bds_force_eph_download  = FALSE;
    tm_core_info.session_info.gal_force_eph_download  = FALSE;
    tm_core_info.session_info.pos_inj_requested       = FALSE;
    tm_core_info.session_info.eph_need_sv_mask = 0;
    tm_core_info.session_info.glo_eph_need_sv_mask = 0;
    tm_core_info.session_info.bds_eph_need_sv_mask = 0;
    tm_core_info.session_info.gal_eph_need_sv_mask = 0;
    tm_core_info.session_info.sess_info_valid_flags = 0;
    tm_core_info.internal_session = FALSE;

    tm_core_update_session_state(TM_CORE_SESS_STATE_INIT);
  }
  if (NULL != pz_pd_event_type)
  {
    (void)os_MemFree((void **)&pz_pd_event_type);
  }

}

static lm_mo_mt_mode_e_type tm_core_mo_mt_mode(uint32 q_sessHandle)
{

  /* LPP CP Session handle may conflict with the MO session handle if included in the next if condition, hence check seperately */
  if (q_sessHandle == TM_SESS_HANDLE_LPP_CP_NI)
  {
    return LM_MO_MT_MODE_MT;
  }
    /* Set the call type to MO/MT based on the session handle. E911 session type should be considered as MT so the HEPE checks at
     timeout are ignored */
  if (0 != (q_sessHandle &
     (TM_SESS_HANDLE_1X_CP_MT | TM_SESS_HANDLE_1X_UP_NI |
      TM_SESS_HANDLE_UMTS_CP_NI | TM_SESS_HANDLE_UMTS_UP_NI | 
      TM_SESS_HANDLE_E911 ))) {
    return LM_MO_MT_MODE_MT;
  }

  return LM_MO_MT_MODE_MO;
}

/*===========================================================================

FUNCTION tm_core_convert_op_mode

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
lm_gps_mode_e_type tm_core_convert_op_mode(tm_sess_operation_mode_e_type op_mode,
                                           tm_prtl_type                  prtl_type)
{

  tm_cm_phone_state_info_s_type phone_state_info;

  /* Query TM-CM module to get current acquired system */
  tm_cm_iface_get_phone_state(&phone_state_info);

  switch(op_mode)
  {
    case TM_SESS_OPERATION_MODE_MSA:
      if((
            (tm_cm_iface_is_serving_system_avail(CGPS_SRV_SYS_CDMA)) ||
            (tm_cm_iface_is_serving_system_avail(CGPS_SRV_SYS_HDR))
          )&&
          (
            (prtl_type == TM_PRTL_TYPE_1X_CP) ||
            (prtl_type == TM_PRTL_TYPE_1X_UP)
          )
        )
      {
        /*If 1x is available and protocol for this sesstion is 1x,
          then use 1x_MSA*/
          return LM_GPS_MODE_1X_MSA;
      }
      else if ( ( phone_state_info.srv_system == CGPS_SRV_SYS_WCDMA ) ||
                ( phone_state_info.srv_system == CGPS_SRV_SYS_GSM )  ||
                ( phone_state_info.srv_system == CGPS_SRV_SYS_LTE ) ||
                ( phone_state_info.srv_system == CGPS_SRV_SYS_TDSCDMA )
         )
      {
        return LM_GPS_MODE_MSA;
      }
      else
      {
        return LM_GPS_MODE_1X_MSA;
      }
#ifdef FEATURE_GNSS_PERIODIC_PPM
    #error code not present
#endif
    case TM_SESS_OPERATION_MODE_MSB:
    case TM_SESS_OPERATION_MODE_MSB_OPT:
      return LM_GPS_MODE_MSB;

    case TM_SESS_OPERATION_MODE_STANDALONE:
      return LM_GPS_MODE_STANDALONE;

    case TM_SESS_OPERATION_MODE_FTM:
      return LM_GPS_MODE_FTM;

    case TM_SESS_OPERATION_MODE_ODP:
      return LM_GPS_MODE_ODP;

    case TM_SESS_OPERATION_MODE_OTDOA:
      return LM_GPS_MODE_MSA;
    default:
     return LM_GPS_MODE_NONE;
  }
}

/*===========================================================================

FUNCTION tm_core_map_dm_event_op_mode

DESCRIPTION
  This function maps correct op mode for reporting to QXDM

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
sm_event_pd_fix_start_payload_e_type tm_core_map_dm_event_op_mode
(
  tm_sess_operation_mode_e_type op_mode
)
{
  switch(op_mode)
  {
    case TM_SESS_OPERATION_MODE_MSA:
    case TM_SESS_OPERATION_MODE_OTDOA:
     return MS_ASSISTED_OP;

    default:
      return MS_BASED_OP;
  }
}

/*===========================================================================

FUNCTION tm_core_fix_time_get

DESCRIPTION
  This function gets Fix Time information in the extended PD INFO buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tm_core_fix_time_get(const sm_GnssNavPosition *nav_ptr)
{
  uint32 q_Msecs, q_Hours, q_Mins, q_Secs, q_Tenths;


  if ((nav_ptr == NULL) || (nav_ptr->q_UtcTimeMs == 0L))
  {
    qword           milliseconds;
    qword           timestamp;
    me_Clock        z_Clock;
    uint32          UtcFixTime, gpsLeapSec;

    (void) time_get( timestamp );
    gps_total_msec( timestamp, milliseconds );
    gps_week_ms( milliseconds, &(z_Clock.z_Time.w_GpsWeek),
                &(z_Clock.z_Time.q_GpsMsec) );
    /* Check for week rollover */
    gps_MsecWeekLimit((int32 *) &z_Clock.z_Time.q_GpsMsec, &z_Clock.z_Time.w_GpsWeek );
    gpsLeapSec =   cgps_GetNvUtcOffset(); /* See the above warning */

    if (z_Clock.z_Time.q_GpsMsec/1000L < gpsLeapSec)
    {
      UtcFixTime = z_Clock.z_Time.q_GpsMsec +
                   (WEEK_SECS - gpsLeapSec) * 1000L;
    }
    else
    {
      UtcFixTime = z_Clock.z_Time.q_GpsMsec - gpsLeapSec * 1000L;
    }
    q_Msecs    = UtcFixTime % DAY_MSECS;
  }
  else
  {
    q_Msecs    = nav_ptr->q_UtcTimeMs % DAY_MSECS;
  }

  /* Break into hours, mins, secs and secs/10 */
  q_Hours     = q_Msecs / HOUR_MSECS;
  q_Msecs     -= q_Hours * HOUR_MSECS;

  q_Mins      = q_Msecs / MIN_MSECS;
  q_Msecs     -= q_Mins * MIN_MSECS;

  q_Secs      = q_Msecs / SEC_MSECS;
  q_Msecs     -= q_Secs * SEC_MSECS;

  q_Tenths    = q_Msecs / TENTH_MSECS;


  tm_core_info.pd_ext_data.hour   = (uint8) q_Hours;
  tm_core_info.pd_ext_data.minute = (uint8) q_Mins;
  tm_core_info.pd_ext_data.sec    = (uint16) (q_Secs * 100 + q_Tenths * 10);

} /* tm_core_fix_time_get */


/*===========================================================================

FUNCTION tm_core_date_get

DESCRIPTION
  This function gets Date information in the
  NMEA Extended PD INFO buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tm_core_date_get(const sm_GnssNavPosition *nav_ptr)
{
  uint32 q_UtcDays, q_FourYears, q_Month,  q_Year;
  uint16 w_UtcWn;
  const uint8 *p_DaysInMonth;

  q_UtcDays   = 0;
  q_Month     = 0;
  q_Year      = 0;

  if (nav_ptr != NULL)
  {
   /* UTC time and GPS time are generally in the same week. Handle the
      13 to 14 second time span when this is not actually the case. GPS
      time leads UTC time (at least in our lifetimes), so checking for
      UTC bigger than GPS is sufficient */

   w_UtcWn = nav_ptr->w_GpsWeek;

   if( nav_ptr->q_GpsTimeMs < nav_ptr->q_UtcTimeMs )
   {
     w_UtcWn --;
   }

   q_UtcDays = (7 * w_UtcWn) + (nav_ptr->q_UtcTimeMs / DAY_MSECS);

   /* This is the number of days since the GPS clock started on
      Jan 6th, 1980. Add 5 days to make day 0 == Jan 1st */

   q_UtcDays += 5;

   /* How many 4 year cycles have occured. */
   q_FourYears = q_UtcDays / (4*365 + 1);

   /* q_UtcDays represents the # of days into this 4 year period */
   q_UtcDays   -= q_FourYears * (4*365 + 1);

   q_Year = 1980 + q_FourYears * 4;

   /* Day 0 to 365 of q_FourYears is in the leap year */
   if( q_UtcDays >= 366 )
   {
     uint32 q_YearsInPeriod;

     /* This year is not a leap year */
     q_Year    += 1;
     q_UtcDays -= 366;

     q_YearsInPeriod = q_UtcDays / 365;
     q_Year    += q_YearsInPeriod;
     q_UtcDays -= q_YearsInPeriod * 365;

     p_DaysInMonth = u_DaysInMonth;
   }
   else
   {
     /* This year is a leap year */
     p_DaysInMonth = u_DaysInMonthLeap;
   }

   /* Subtract days in months for this year. */
   for( q_Month = 1;
      q_Month < 12 && q_UtcDays >= *p_DaysInMonth; q_Month++ )
   {
     q_UtcDays -= *p_DaysInMonth;
     p_DaysInMonth++;
   }

   /* Need to add 1 to account for the fact the days in month
      start at 1 and not 0 */
   q_UtcDays ++;
  }

  tm_core_info.pd_ext_data.date = (uint32) ((q_UtcDays * 1000000) + (q_Month * 10000) + q_Year);

} /* tm_core_date_get */

/*===========================================================================

FUNCTION tm_core_pos_get

DESCRIPTION
  This function update the GPS Position information in the
  NMEA Extended PD INFO buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tm_core_pos_get(const sm_GnssNavPosition *nav_ptr)
{
  int64 d_Lat = 0;
  int64 d_Lon = 0;
  FLT   f_MagDev = 0.0;

  if (nav_ptr == NULL)
  {
    tm_core_info.pd_ext_data.lat   = d_Lat;
    tm_core_info.pd_ext_data.lon   = d_Lon;
    tm_core_info.pd_ext_data.magnetic_variation = (int64)f_MagDev;
  }
  else
  {
    d_Lat  = (int64) (nav_ptr->d_PosLla[0] * (180.0 / C_PI) * (double) PDSM_LATLONG_SCALE + 0.5);
    d_Lon  = (int64) (nav_ptr->d_PosLla[1] * (180.0 / C_PI) * (double) PDSM_LATLONG_SCALE + 0.5);

    tm_core_info.pd_ext_data.lat = d_Lat;
    tm_core_info.pd_ext_data.lon = d_Lon;

    if (nav_ptr->u_MagDevGood==TRUE)
    {
      tm_core_info.pd_ext_data.magnetic_variation = (int64)(nav_ptr->f_MagDeviation * 10.0 + 0.5);  /* [x10 deg] */
    }
    else
    {
      /* If Mag Dev is not available, attempt to compute it */
      if (cgps_MagDeviation( nav_ptr->d_PosLla[0], nav_ptr->d_PosLla[1], &f_MagDev ) == TRUE)
      {
        tm_core_info.pd_ext_data.magnetic_variation = (int64)(f_MagDev * 10.0 + 0.5);
      }
      else
      {
        tm_core_info.pd_ext_data.magnetic_variation = 0;
      }
    }

    MSG_MED("Position: MagDev: %d, MagDevGood: %d",
          tm_core_info.pd_ext_data.magnetic_variation,
          nav_ptr->u_MagDevGood, 0);
  }

} /* tm_core_pos_get */

/*===========================================================================

FUNCTION tm_core_velocity_get

DESCRIPTION
  This function udpates the velocity information in the
  NMEA Extended PD INFO buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tm_core_velocity_get(const sm_GpsFixRptStructType *fix_ptr)
{
  double  d_NorthVel, d_EastVel, d_Knots, d_Speed, d_Azimuth;
  const sm_GnssNavPosition *nav_ptr = &(fix_ptr->z_NavPos);
  uint8 loop_count = 0;

  if ((nav_ptr == NULL) ||
      !nav_ptr->z_PosFlags.b_IsVelValid)
  {
    tm_core_info.pd_ext_data.velocity_include   = 0;
    tm_core_info.pd_ext_data.speed_knot         = 0;
    tm_core_info.pd_ext_data.speed_kmh          = 0;
    tm_core_info.pd_ext_data.heading_true       = 0;
    tm_core_info.ext_opt_field_mask             = 0;
  }
  else
  {
    tm_core_info.pd_ext_data.velocity_include = 1;

    tm_util_calculate_heading(fix_ptr, &d_Azimuth);

    d_EastVel  = nav_ptr->f_VelEnu[0];
    d_NorthVel = nav_ptr->f_VelEnu[1];

    /* East, North, Up velocity in m/s  */
    for(loop_count=0; loop_count<3 ; loop_count++)
    {
       tm_core_info.pd_ext_data.f_VelocityENUMps[loop_count] = nav_ptr->f_VelEnu[loop_count];
    }

    /* East, North, Up velocity uncertainty in m/s  */
    for(loop_count=0; loop_count<2 ; loop_count++)
    {
       tm_core_info.pd_ext_data.f_VelocityUncENUMps[loop_count] = nav_ptr->f_VuncEastNorthMps[loop_count];
    }

    tm_core_info.pd_ext_data.f_VelocityUncENUMps[2] = nav_ptr->f_VuncVertMps;

    tm_core_info.pd_ext_data.heading_true = (uint16) (d_Azimuth * 10.0 + 0.5);        /* [x10 deg] */

    if(fix_ptr->z_NavPos.z_PosFlags.b_IsHeadingValid)
    {
      tm_core_info.ext_opt_field_mask = PDSM_PD_HEADING_VALID;
    }

    d_Speed = sqrt( d_NorthVel * d_NorthVel + d_EastVel * d_EastVel );
    if (d_Speed > TM_CORE_MIN_VELOCTY)
    {
      d_Knots = d_Speed * (3600.0/1852.0);

      /* Azimuth moves from north (0) to east (90), wrapping around.
       * Note that we are ignoring the vertical velocity.
      */

      tm_core_info.pd_ext_data.speed_knot   = (uint32) (d_Knots * 10.0 + 0.5);          /* [x10 knots] */
      tm_core_info.pd_ext_data.speed_kmh    = (uint32) ((d_Speed * 3.6) * 10.0 + 0.5);  /* [x10 km/h]  */
    }
    else  /* Speed is too slow for a meaningful course. */
    {
      tm_core_info.pd_ext_data.speed_knot = 0;
      tm_core_info.pd_ext_data.speed_kmh  = 0;
    }
  }
} /* tm_core_velocity_get */


/*===========================================================================

FUNCTION tm_sv_in_view_get

DESCRIPTION
  This function updates the SV in view info in the NMEA Extended PD INFO buffer.
  SVs of all constellations that MGP is using must be updated.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tm_sv_in_view_get(gnss_MeasBlkStructType *p_MeasBlk, uint8 u_MaxChan)
{
  uint8  u_i;        /* u_i iterates over satellites stored in the meas blk structures in tm_core_info */
  uint8  u_Chan;   /* u_Chan iterates over all satellites stored in one of the meas blk structures in tm_core_info */
  uint32 mask;
  pdsm_sv_inview_s_type      *p_Sv      = NULL;
  gnss_MeasStructType    *p_zMeas = NULL;

  p_zMeas = p_MeasBlk->z_Gnss;
  u_i = tm_core_info.pd_ext_data.num_sv_in_view;  // current index

  for(u_Chan = 0; (u_Chan < p_MeasBlk->u_NumSvs && u_Chan < u_MaxChan) && (u_i < PDSM_GNSS_MAX_SATELLITES); u_Chan++)
  {
    mask = p_zMeas[u_Chan].q_MeasStatus & (SM_MEAS_STATUS_INVALID_FLAGS|SM_MEAS_STATUS_SM_VALID);

    if(mask == SM_MEAS_STATUS_SM_VALID )
    {
      p_Sv = &tm_core_info.pd_ext_data.sv[u_i];
      /* PRN, elevation, azimuth and SNR (C/No): */
      p_Sv->sv_num       = p_zMeas[u_Chan].w_Sv;

      if( p_zMeas[u_Chan].u_SvDirValid && ( p_zMeas[u_Chan].f_Elevation > 0))
      {
        p_Sv->elev         = (uint8)(p_zMeas[u_Chan].f_Elevation * RAD2DEG);

        /* azimuth_snr contains azimuth + snr infomation, multiply azimuth by 100 and add SNR */
        /* azimuth can be obtained by doing azimuth_snr / 100 and snr can be obtained by doing azimuth % 100 */
        p_Sv->azimuth_snr  = (uint16)(p_zMeas[u_Chan].f_Azimuth * RAD2DEG) * 100;

        if (p_zMeas[u_Chan].w_Cno)
        {
          p_Sv->azimuth_snr += (uint16)(p_zMeas[u_Chan].w_Cno / 10);
        }
      }
      else
      {
        p_Sv->elev  = 0;
        p_Sv->azimuth_snr = 0;
      }

      u_i++;
    }
  }

  tm_core_info.pd_ext_data.num_sv_in_view = u_i;

} /* tm_sv_in_view_get() */


/*===========================================================================

FUNCTION tm_core_generate_ext_pd_data

DESCRIPTION
  This function generates extended pd data

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_generate_ext_pd_data( const sm_GnssFixRptStructType *fix_ptr)
{
  memset((void *)&tm_core_info.pd_ext_data, 0, sizeof(pdsm_pd_data_ext_s_type));
  tm_core_info.ext_opt_field_mask = 0;

  tm_core_fix_time_get(&(fix_ptr->z_NavPos));

  tm_core_date_get(&(fix_ptr->z_NavPos));

  if(fix_ptr == NULL)
  {
    tm_core_info.pd_ext_data.fix_type       = PDSM_PD_FIX_TYPE_UNKNOWN;
    tm_core_info.pd_ext_data.fix_quality    = PDSM_PD_FIX_QUALITY_UNKNOWN;
    tm_core_info.pd_ext_data.selection_type = PDSM_PD_FIX_SELECTION_UNKNOWN;
  }
  else
  {
    /* Update Lat/Long */
    tm_core_pos_get(&(fix_ptr->z_NavPos));

    /* Height Above Ellipsoid (HAE) */
    tm_core_info.pd_ext_data.altitude = (int32) (fix_ptr->z_NavPos.d_PosLla[2] * 10.0 + 0.5);

     /* Velocity estimate */
    tm_core_velocity_get(fix_ptr);

    /* 1 - no fix, 2 - 2D, 3 - 3D */

    if (fix_ptr->z_NavPos.z_PosFlags.b_Is3D)
    {
      tm_core_info.pd_ext_data.fix_type = PDSM_PD_FIX_TYPE_3D;
    }
    else
    {
      tm_core_info.pd_ext_data.fix_type = PDSM_PD_FIX_TYPE_2D;
    }

    /* hdop, pdop, vdop [x10] */
    tm_core_info.pd_ext_data.h_dop = (uint32) (fix_ptr->z_NavPos.f_HDOP * 10.0 + 0.5);
    tm_core_info.pd_ext_data.p_dop = (uint32) (fix_ptr->z_NavPos.f_PDOP * 10.0 + 0.5);
    tm_core_info.pd_ext_data.v_dop = (uint32) (fix_ptr->z_NavPos.f_VDOP * 10.0 + 0.5);

    //<<<[???  by Liao] How do we get this info??? Hardcode it now

    /* Manual or Auto 2D/3D mode */
    //pe_ConfigGet( &z_peConfig );

    //if (z_peConfig.u_AltitudeHold == PE_ALT_FILTERED ||
    //    z_peConfig.u_AltitudeHold == PE_ALT_HOLD_AUTO  )
    //{
    //  tm_core_info.pd_ext_data.selection_type = PDSM_PD_FIX_SELECTION_AUTO;
    //}
    //else
    //{
    //  tm_core_info.pd_ext_data.selection_type = PDSM_PD_FIX_SELECTION_MANUAL;
    //}
    tm_core_info.pd_ext_data.selection_type = PDSM_PD_FIX_SELECTION_AUTO;
    //>>>[??? End]

    /* GPS quality issues */
    if (fix_ptr->z_NavPos.z_PosFlags.b_IsValid)
    {
//      if ((fix_ptr->z_NavPos.z_PosFlags..w_PositionFlags & NF_DIFFERENTIAL) ||
//         /* Set the MS-Assisted Fixes as DGPS fixes also */
//          (fix_ptr->z_NavSolution.w_PositionFlags & NF_EXTERNAL_UPDATE))
      {
        tm_core_info.pd_ext_data.fix_quality = PDSM_PD_FIX_QUALITY_DGPS;  /* DGPS fix */
      }
//      else
      {
//        tm_core_info.pd_ext_data.fix_quality = PDSM_PD_FIX_QUALITY_GPS;  /* Non DGPS fix */
      }
    }
    else
    {
      tm_core_info.pd_ext_data.fix_quality = PDSM_PD_FIX_QUALITY_UNKNOWN; /* No fix at all */
    }
  } /* fix available? */

  /* Populate pd_ext_data's SV info array */
  tm_core_info.pd_ext_data.num_sv_in_view = 0;
  tm_sv_in_view_get(&tm_core_info.gps_meas_blk, N_ACTIVE_GPS_CHAN);
  tm_sv_in_view_get(&tm_core_info.glo_meas_blk, N_ACTIVE_GLO_CHAN);
  tm_sv_in_view_get(&tm_core_info.bds_meas_blk, N_ACTIVE_BDS_CHAN);
  tm_sv_in_view_get(&tm_core_info.qzss_meas_blk, N_ACTIVE_CHAN);
  tm_sv_in_view_get(&tm_core_info.gal_meas_blk, N_ACTIVE_GAL_CHAN);
}


static lm_wishlist_src_req_e_type tm_core_xlate_prtl_to_wishlist_src
(
  tm_sess_operation_mode_e_type op_mode
)
{
  switch(op_mode)
  {
    /* Stateful query for MSB */
    case(TM_SESS_OPERATION_MODE_MSB):
      return LM_ASSIST_DATA_REQ_MSB;

    /* Always a stateless query everything else */
    case(TM_SESS_OPERATION_MODE_STANDALONE):
    case(TM_SESS_OPERATION_MODE_ODP):
    case(TM_SESS_OPERATION_MODE_MSA):
    case(TM_SESS_OPERATION_MODE_OTDOA):   
    case(TM_SESS_OPERATION_MODE_NONE):
    case(TM_SESS_OPERATION_MODE_MSB_OPT):
    case(TM_SESS_OPERATION_MODE_FTM):
    case(TM_SESS_OPERATION_MODE_PERIODIC_PPM):
    default:
      return LM_ASSIST_DATA_REQ_QUERY;
  }
}

/*===========================================================================

FUNCTION tm_core_sess_req_start

DESCRIPTION
  This function is to figure out if TM-Core accepts session start request from
  protocol sub-module.

DEPENDENCIES

RETURN VALUE
  true  - if the session was started successfully
  false - otherwise

SIDE EFFECTS

===========================================================================*/
boolean tm_core_sess_req_start(tm_prtl_type                    prtl_type,
                               tm_sess_handle_type             sess_handle,
                               tm_sess_req_start_param_s_type  sess_start_param)
{
  boolean retVal = FALSE;
  lm_session_request_info_s_type       lm_sess_req_param;
  lm_request_info_s_type               lm_info_req_param;
  sm_event_pd_fix_start_payload_e_type dm_op_mode;

  tm_cm_phone_state_info_s_type        phone_state_info;
  pdsm_delete_parms_type               delete_param;
  FLT f_punc;
  FLT f_tunc;

  lm_info_s_type                       lm_info_data;

  MSG_MED("tm_core entering function [tm_core_sess_req_start]  sess_handle is %d, prtl is %d", sess_handle, prtl_type, 0);

  memset((void *)&lm_sess_req_param, 0, sizeof(lm_session_request_info_s_type));
  memset((void *)&lm_info_req_param, 0, sizeof(lm_request_info_s_type));
  tm_cm_iface_get_phone_state(&phone_state_info);

  if(tm_core_is_reset_location_service_in_progress())
  {
    boolean session_allowed = FALSE;    
    /*If reset location service is in progress, only sessions that are allowed
      are emergency sessions.*/
    if (phone_state_info.e911_state != TM_CM_E911_STATE_NONE)
    {
      if( (sess_handle == TM_SESS_HANDLE_1X_CP_MT ) ||
          (sess_handle == TM_SESS_HANDLE_UMTS_CP_NI) ||
          (sess_handle == TM_SESS_HANDLE_LPP_CP_NI) ||
          (sess_handle == TM_SESS_HANDLE_UMTS_UP_NI)
        )
      {
        MSG_HIGH("Allow emergency call flows during location service reset",0,0,0);
        session_allowed = TRUE;
      }
    }

    if(session_allowed == FALSE)
    {
      MSG_HIGH("Not allowing positioning sessions during location service reset",0,0,0);
      return FALSE;
    }
  }
#ifdef FEATURE_JCDMA_GPSONE
  /* Abort reason is for JGPS module to end 1x UP during 1x CP */
  if(sess_start_param.abort_curr_sess == TRUE)
  {
    /* Abort any current sessions and recover to allow other session type */
    tm_core_abort_recover(TRUE, TM_STOP_REASON_JGPS_ABORT_SESS); /*lint !e506 !e730 */
  }
#endif /* FEATURE_JCDMA_GPSONE */

  
  if (phone_state_info.e911_state != TM_CM_E911_STATE_NONE)
  {
    boolean active_sess_is_ni =
        ((tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_LPP_CP_NI)  ||
         (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_1X_CP_MT) ||
         (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_UMTS_UP_NI) ||
         (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_UMTS_CP_NI)) ;

    boolean new_sess_prtl_ni  =
        ((prtl_type == TM_PRTL_TYPE_UMTS_CP) ||
         (prtl_type == TM_PRTL_TYPE_1X_CP)   ||
         (prtl_type == TM_PRTL_TYPE_UMTS_UP) ||
         (prtl_type == TM_PRTL_TYPE_LPP_CP));

    if ( active_sess_is_ni && new_sess_prtl_ni )
    {
     /* emergency session was ongoing on LPP_CP or SUPL, now a new GPS session was started on a different RAT
        started by the network, continue the session with the new session handle */
      MSG_HIGH("Emergency session on going on %d, new protocol %d. Abort ongoing ",
               tm_core_info.session_info.active_sess_handle, prtl_type,0);
      /* Abort any current sessions and recover to allow other session type */
      tm_core_abort_recover(TRUE, TM_STOP_REASON_E911); /*lint !e506 !e730 */
    }

    #ifdef FEATURE_MO_GPS_FOR_ECALL
    if (!active_sess_is_ni && new_sess_prtl_ni &&
       (tm_core_info.session_info.active_sess_handle != TM_SESS_HANDLE_E911) &&
        (tm_core_info.session_info.active_sess_handle != TM_SESS_HANDLE_NONE))
    {
     /* MO session was ongoing in E911, however network session started on 
      * W/1X/LPP. Abort the ongoing session and accept the new session */
      MSG_HIGH("Abort ongoing session. Handle(%d), New protocol (%d)",
               tm_core_info.session_info.active_sess_handle, prtl_type,0);
      /* Abort any current sessions and recover to allow other session type */
      tm_core_abort_recover(TRUE, TM_STOP_REASON_E911); /*lint !e506 !e730 */
    }
    #endif

    if ( (tm_core_info.session_info.active_sess_handle != TM_SESS_HANDLE_NONE) &&
         ((phone_state_info.srv_system == CGPS_SRV_SYS_LTE) &&
        ((prtl_type == TM_PRTL_TYPE_UMTS_CP) ||
           (prtl_type == TM_PRTL_TYPE_1X_CP))) )
    {
     /* emergency session was started on SVLTE when camped on LTE, however network session started on W/1X
        abort the ongoing session and accept the new session */
      MSG_HIGH("Emergency session on going on %d, new protocol %d. Abort ongoing ",
               tm_core_info.session_info.active_sess_handle, prtl_type, 0);
      /* Abort any current sessions and recover to allow other session type */
      tm_core_abort_recover(TRUE, TM_STOP_REASON_E911); /*lint !e506 !e730 */
    }

    /* if MO is ongoing and a new NI session is started */
    if (((tm_core_info.session_info.active_sess_handle < TM_SESS_HANDLE_1X_CP_MT) && 
         (tm_core_info.session_info.active_sess_handle != TM_SESS_HANDLE_NONE)) && 
        ((prtl_type == TM_PRTL_TYPE_UMTS_CP) ||
         (prtl_type == TM_PRTL_TYPE_1X_CP) ||
         (prtl_type == TM_PRTL_TYPE_LPP_CP) ||
         (prtl_type == TM_PRTL_TYPE_UMTS_UP))) 
    {
     /* MO was ongoing, however network session started on W/G/1X/LTE E911 session
        abort the ongoing session and accept the new session. */
      MSG_HIGH("MO session on going on %d, new E911 session started by protocol %d. Abort ongoing ",
               tm_core_info.session_info.active_sess_handle, prtl_type,0);
      /* Abort any current sessions and recover to allow other session type */
      tm_core_abort_recover(TRUE, TM_STOP_REASON_E911); /*lint !e506 !e730 */
    }
  }
  /* cancel any ongoing on-demand session */
  if ((sess_handle != TM_SESS_HANDLE_ON_DEMAND) &&
       (tm_core_get_on_demand_state() == TRUE))
  {
    MSG_MED("Calling on-demand abort ", 0,0,0);
    tm_core_abort_recover(TRUE, TM_STOP_REASON_ON_DEMAND_OVERRIDE);
  }
#ifdef FEATURE_GNSS_PERIODIC_PPM
  #error code not present
#endif

  if(tm_core_is_internal_session_on() == TRUE)
  {
    /*Abort the ongoing internal session*/
    MSG_MED("Aborting on-going internal session", 0, 0, 0);
    tm_core_abort_recover(TRUE, TM_STOP_REASON_ABORT_INTERNAL_SESSION);
  }

  if ((phone_state_info.e911_state != TM_CM_E911_STATE_NONE) &&
      (tm_core_is_ni_resp_pending() == TRUE) &&
      ((sess_handle == TM_SESS_HANDLE_UMTS_CP_NI) ||
       (sess_handle == TM_SESS_HANDLE_LPP_CP_NI)))
  {
    /* NI response is pending from AP, but a new CP NI LR came in, we need to honor CP NILR and when
       the NI Response comes back, we need to ensure CP NI LR is not preempted */
    tm_core_set_ni_resp_pending(FALSE);
  }

  /* Accept requests when there is no active session, by starting a new LM-TM
   * session */
  if( (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_NONE)
    &&(tm_core_info.session_info.session_state == TM_CORE_SESS_STATE_INIT))
  {

    /* TM-Core accepts the incoming request, so update TM-Core session info */
    tm_core_info.session_info.active_sess_handle = sess_handle;
    tm_core_info.session_info.from_protocol      = prtl_type;
    tm_core_info.session_info.op_mode            = sess_start_param.op_mode;

    tm_core_update_session_state(TM_CORE_SESS_STATE_ACCESS_GRANTED);

    if ( sess_start_param.op_req & TM_OP_REQ_DEL_ASSIST )
    {

      /* OTA delete all assistance data */
      /* if currently MGP is not in OFF state, force it to OFF by first using
         the force-to-idle API then indicating force-to-off in the function
         of tm_core_delete_inject_aiding_data() */
      if ( tm_core_info.e_cached_rcvr_state != C_GPS_RCVR_STATE_OFF )
      {
        mgp_ReceiverForcedIdle();
        mgp_ReceiverForcedOff();
      }

      memset( &delete_param, 0, sizeof(pdsm_delete_parms_type) );
      tm_core_delete_inject_aiding_data(&delete_param, FALSE, TRUE, PDSM_PA_DELETE_PARAMS_EX);
      return TRUE;  /* no further action needed */

    }


    /*
      Now request session start to LM
      If life is good, LM should accept it.
      If there is something wrong, LM rejects it (asynchronously), we need to make sure we do recovery properly
    */
    lm_sess_req_param.q_handle = sess_handle;
    lm_sess_req_param.e_req_type = LM_SESSION_REQUEST_START;
    lm_sess_req_param.q_req_mask = (uint32)LM_SESSION_REQUEST_SESSION;

	if (phone_state_info.e911_state != TM_CM_E911_STATE_NONE)
    {
	  lm_sess_req_param.z_request.z_session_request.b_e911_session = TRUE;
	}
	
    if (sess_handle == TM_SESS_HANDLE_ON_DEMAND)
    {
      lm_sess_req_param.z_request.z_fix_request.u_on_demand = TRUE;

      tm_on_demand_get_punc_tunc(&f_punc, &f_tunc);
      lm_sess_req_param.z_request.z_fix_request.q_Punc = (uint32)f_punc;
      lm_sess_req_param.z_request.z_fix_request.q_Tunc = (uint32)f_tunc;
    }
    else
    {
      lm_sess_req_param.z_request.z_fix_request.u_on_demand = FALSE;
    }


    if( (tm_core_info.session_info.from_protocol == TM_PRTL_TYPE_1X_CP)
      ||(tm_core_info.session_info.from_protocol == TM_PRTL_TYPE_UMTS_CP)
      ||(tm_core_info.session_info.from_protocol == TM_PRTL_TYPE_LPP_CP))
    {
      lm_sess_req_param.z_request.z_session_request.e_up_cp = LM_UP_CP_MODE_CP;
    }
    else
    {
      lm_sess_req_param.z_request.z_session_request.e_up_cp = LM_UP_CP_MODE_UP;
    }


    lm_sess_req_param.z_request.z_session_request.e_mode = tm_core_convert_op_mode(sess_start_param.op_mode, prtl_type);
    lm_sess_req_param.z_request.z_session_request.e_mo_mt = tm_core_mo_mt_mode(sess_handle);
    lm_sess_req_param.z_request.z_session_request.q_num_fixes = sess_start_param.num_fixes;
    lm_sess_req_param.z_request.z_session_request.q_tbf = sess_start_param.tbf_ms;

    /* Check to see if NHz operating mode is supported.
       If it is not, restrict the TBF to 1000ms minimum 
    */
    if( !(tm_core_info.config_info.gnss_nhz_config & GNSS_NHZ_ENABLED)
        && (sess_start_param.tbf_ms < TM_CORE_NOMINAL_TBF_MS ) )
    {
      lm_sess_req_param.z_request.z_session_request.q_tbf = TM_CORE_NOMINAL_TBF_MS;
    }
    
    /* Determine if data demod is allow for MSB */
    if(sess_start_param.op_mode == TM_SESS_OPERATION_MODE_MSB ||
       sess_start_param.op_mode == TM_SESS_OPERATION_MODE_MSB_OPT)
    {
      if( (tm_core_get_active_prtl_module(MT_NA) == TM_PRTL_TYPE_1X_UP)
        &&(tm_core_info.config_info.smart_msb == FALSE))
      {
        MSG_MED("Data demod disabled", 0, 0, 0);
        lm_sess_req_param.z_request.z_session_request.u_do_data_demod = FALSE;
      }
      else
      {
        lm_sess_req_param.z_request.z_session_request.u_do_data_demod = TRUE;
      }
    }
    else
    {
      lm_sess_req_param.z_request.z_session_request.u_do_data_demod = TRUE;
    }


    if(sess_start_param.op_req & TM_OP_REQ_PPM)
    {
      lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_CDMA;
      lm_sess_req_param.z_request.z_cdma_request.q_timeout = sess_start_param.ppm_qos * 1000;
    }

    if(sess_start_param.op_req & TM_OP_REQ_PRM)
    {
      lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_GPS;
      lm_sess_req_param.z_request.z_gps_request.q_timeout = sess_start_param.prm_qos * 1000;
      lm_sess_req_param.z_request.z_gps_request.u_qos     = sess_start_param.prm_prq;
      lm_sess_req_param.z_request.z_gps_request.multi_report_msa = sess_start_param.multi_report_msa;
    }

    if(sess_start_param.op_req & TM_OP_REQ_LR)
    {
      lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_FIX;
      lm_sess_req_param.z_request.z_fix_request.q_timeout = sess_start_param.lr_qos * 1000;
      lm_sess_req_param.z_request.z_fix_request.q_qos_accuracy_threshold  = sess_start_param.accuracy_threshold;
    lm_sess_req_param.z_request.z_fix_request.u_dynamic_qos_enabled  = sess_start_param.dynamic_qos_enabled;
    }
    /* for MO OTDOA no need to start LM session */
    if ((sess_start_param.op_mode != TM_SESS_OPERATION_MODE_OTDOA) ||
        (sess_start_param.op_mode == TM_SESS_OPERATION_MODE_OTDOA &&
         tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_UMTS_UP_NI))
    {
    retVal = lm_session_request(&lm_sess_req_param);
    tm_core_info.session_info.op_requested |= sess_start_param.op_req;

    }
    else
    {
      tm_core_info.session_info.op_requested = TM_OP_REQ_NONE;
      retVal = TRUE;
    }

    /* Fire Evt PD_FIX start, req Aiding Data Wishlist, Ref Loc, and start the
     * diag pos timer only if session is not PERIODIC_PPM */
    if( (sess_handle != TM_SESS_HANDLE_PERIODIC_PPM) &&
        (retVal == TRUE) ) /*lint !e506 !e731 */
    {
      /* Fire EVENT_GPS_PD_FIX_START */
      dm_op_mode = tm_core_map_dm_event_op_mode(tm_core_info.session_info.op_mode);
      sm_report_event_gps_fix_start(tm_core_event_counter++, dm_op_mode);

      /*
         Always get aiding data wishlist update
         so that we can populate alm_svmask and eph_svmask in PRM report.
         However, if protocol modules does not ask for aiding data update,
         don't forward aiding data wishlist to protocol modules,
         when it comes back from MGP
      */
        lm_info_req_param.e_req_type = LM_REQUEST_AIDING_DATA_WISHLIST;
        lm_info_req_param.z_request.e_wishlist_req_type =
                 tm_core_xlate_prtl_to_wishlist_src(sess_start_param.op_mode);
       /* for OTDOA no need to start LM session */
      if (sess_start_param.op_mode != TM_SESS_OPERATION_MODE_OTDOA)
      {
        retVal = lm_request(&lm_info_req_param);
      }

      if(sess_start_param.op_req & TM_OP_REQ_REF_LOC)
      {
        lm_info_req_param.e_req_type = LM_REQUEST_POS_ESTIMATE;
        retVal = lm_request(&lm_info_req_param);
      }
      tm_core_info.session_info.q_FixTimeRemainingMSec = tm_core_info.session_info.pd_param.pd_qos.gps_session_timeout * 1000;
      if (sess_start_param.op_mode == TM_SESS_OPERATION_MODE_MSB ||
          sess_start_param.op_mode == TM_SESS_OPERATION_MODE_MSA)
      {
        /* Query TM-CM module to get current acquired system */
        tm_cm_iface_get_phone_state(&phone_state_info); 
        if (phone_state_info.srv_system == CGPS_SRV_SYS_LTE)
        {
          /* tell MGP to start LTE time transfer need API from Bora */
          lm_info_data.e_info_type = LM_SESSION_INFO_SESSION_TYPE;
          lm_info_data.z_info.z_sess_info.e_mode = tm_core_convert_op_mode(sess_start_param.op_mode, prtl_type);
          lm_info_data.z_info.z_sess_info.e_srv_system = phone_state_info.srv_system;
          retVal = lm_info(&lm_info_data);
        }
      }
#ifdef FEATURE_CGPS_XTRA_T
      if (sess_start_param.op_mode == TM_SESS_OPERATION_MODE_STANDALONE)
      {
        /* TODO : ahmad to add new TLE API call here! */
      }
#endif
    }

    if(retVal == FALSE)
    {
      /* LM does not allow the request, abort and recover */
      tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
    }
  }
  else if ((tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_E911) &&
           ((prtl_type == TM_PRTL_TYPE_LPP_CP) ||
           (prtl_type == TM_PRTL_TYPE_UMTS_UP) ||
           (prtl_type == TM_PRTL_TYPE_UMTS_CP)))
  {
   /* emergency session was started at start of emergency call indication. now the GPS session was 
      started by the network, continue the session with the new session handle */
    MSG_HIGH("Emergency session on going ", 0,0,0);
    tm_core_sess_req_continue_for_emergency_session(prtl_type, sess_handle, sess_start_param);
    retVal = TRUE;
  }
  else
  {
    MSG_ERROR("Reject start req because TM-Core already is in another session", 0, 0, 0);
    retVal = FALSE;
  }

  return retVal;
}


/*===========================================================================

FUNCTION tm_core_sess_req_continue_for_emergency_session

DESCRIPTION
  This function accepts session continue request for an emergency session. At 
  start of an emergency call, TM starts a dummy E911 session with LM, once the
  network starts positioning, TM will send this continue session with updated
  session handle and session parameters.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

boolean tm_core_sess_req_continue_for_emergency_session(tm_prtl_type prtl_type,
                                  tm_sess_handle_type                sess_handle,
                                  tm_sess_req_start_param_s_type  sess_start_param)
{

  
  lm_session_request_info_s_type lm_sess_req_param;
  lm_request_info_s_type         lm_info_req_param;  
  tm_op_req_type                 new_op_req;
  tm_op_req_type                 existing_op_req;  
  tm_cm_phone_state_info_s_type phone_state_info;
  boolean retVal = TRUE;
  
  MSG_MED("TM-Core Recv'd Sess Continue Req for emergency %lu %lu, %u", prtl_type, sess_handle, tm_core_info.session_info.session_state);

  memset((void *)&lm_sess_req_param, 0, sizeof(lm_session_request_info_s_type));
  memset((void *)&lm_info_req_param, 0, sizeof(lm_request_info_s_type));  

  /* TM-Core accepts the incoming request, so update TM-Core session info */
  tm_core_info.session_info.active_sess_handle = sess_handle;
  tm_core_info.session_info.from_protocol      = prtl_type;
  
  /* If operation mode has been changed (Optimal mode fallback sessions, or E911 that starts with MSB and now the network
     requests MSA for example ), notify LM to change the mode*/
  if ((tm_core_info.session_info.op_mode != sess_start_param.op_mode)
      && (sess_start_param.op_mode != TM_SESS_OPERATION_MODE_OTDOA))
  {

    MSG_4(MSG_SSID_GPSSM, MSG_LEGACY_MED,
          "Exiting op mode (%u) != cont.op_mode (%u), tm_core.op_requested = %x",
          tm_core_info.session_info.op_mode,
          sess_start_param.op_mode,
          tm_core_info.session_info.op_requested,
          sess_start_param.op_req);

    tm_core_info.session_info.op_mode = sess_start_param.op_mode;
    lm_sess_req_param.q_handle = sess_handle;
    lm_sess_req_param.e_req_type = LM_SESSION_REQUEST_CONTINUE;
    lm_sess_req_param.q_req_mask = (uint32)LM_SESSION_REQUEST_SESSION;
    lm_sess_req_param.z_request.z_session_request.e_mode = tm_core_convert_op_mode(sess_start_param.op_mode, prtl_type);
    lm_sess_req_param.z_request.z_session_request.e_mo_mt = tm_core_mo_mt_mode(sess_handle);

    /* Determine if data demod is allow for MSB */
    if (sess_start_param.op_mode == TM_SESS_OPERATION_MODE_MSB)
    {
      if ((tm_core_get_active_prtl_module(MT_NA) == TM_PRTL_TYPE_1X_UP)
          && (tm_core_info.config_info.smart_msb == FALSE))
      {
        MSG_MED("Data demod disabled", 0, 0, 0);
        lm_sess_req_param.z_request.z_session_request.u_do_data_demod = FALSE;
      }
      else
      {
        lm_sess_req_param.z_request.z_session_request.u_do_data_demod = TRUE;
      }
    }
    else
    {
      lm_sess_req_param.z_request.z_session_request.u_do_data_demod = TRUE;
    }

    retVal = lm_session_request(&lm_sess_req_param);
  }

  if (retVal)
  {
    lm_sess_req_param.q_req_mask = 0;

    /* 
       For operatons which have not been previously started, use start to LM
       For operatons which have already been started, use continue to LM
    */
    existing_op_req = sess_start_param.op_req & tm_core_info.session_info.op_requested;
    new_op_req      = sess_start_param.op_req & ~(existing_op_req);

    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED,
          "continue_param.op_req = %x, tm_core_info.session_info.op_requested = %x",
          sess_start_param.op_req,
          tm_core_info.session_info.op_requested);

    /* 
       For operatons which have not been previously started, use start to LM
    */
    if (new_op_req & (TM_OP_REQ_PPM | TM_OP_REQ_PRM | TM_OP_REQ_LR))
    {
      lm_sess_req_param.q_handle   = sess_handle;
      lm_sess_req_param.e_req_type = LM_SESSION_REQUEST_START;

      if (new_op_req & TM_OP_REQ_PPM)
      {
        lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_CDMA;
        lm_sess_req_param.z_request.z_cdma_request.q_timeout = sess_start_param.ppm_qos * 1000;
      }

      if (new_op_req & TM_OP_REQ_PRM)
      {
        lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_GPS;
        lm_sess_req_param.z_request.z_gps_request.q_timeout = sess_start_param.prm_qos * 1000;
        lm_sess_req_param.z_request.z_gps_request.u_qos     = sess_start_param.prm_prq;
        lm_sess_req_param.z_request.z_gps_request.multi_report_msa = sess_start_param.multi_report_msa;
      }

      if (new_op_req & TM_OP_REQ_LR)
      {
        lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_FIX;
        lm_sess_req_param.z_request.z_fix_request.q_timeout = sess_start_param.lr_qos * 1000;
        lm_sess_req_param.z_request.z_fix_request.q_qos_accuracy_threshold  = sess_start_param.accuracy_threshold;
        lm_sess_req_param.z_request.z_fix_request.u_dynamic_qos_enabled  = sess_start_param.dynamic_qos_enabled;
      }

      retVal = lm_session_request(&lm_sess_req_param);

    }

    tm_core_info.session_info.op_requested |= new_op_req;


    /* 
       For operatons which have already been previously started, use continue to LM
    */
    if (existing_op_req & (TM_OP_REQ_PPM | TM_OP_REQ_PRM | TM_OP_REQ_LR))
    {
      lm_sess_req_param.q_handle   = sess_handle;
      lm_sess_req_param.e_req_type = LM_SESSION_REQUEST_CONTINUE;

      if (existing_op_req & TM_OP_REQ_PPM)
      {
        lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_CDMA;
        lm_sess_req_param.z_request.z_cdma_request.q_timeout = sess_start_param.ppm_qos * 1000;
      }

      if (existing_op_req & TM_OP_REQ_PRM)
      {
        lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_GPS;
        lm_sess_req_param.z_request.z_gps_request.q_timeout = sess_start_param.prm_qos * 1000;
        lm_sess_req_param.z_request.z_gps_request.u_qos     = sess_start_param.prm_prq;
        lm_sess_req_param.z_request.z_gps_request.multi_report_msa = sess_start_param.multi_report_msa;
      }

      if (existing_op_req & TM_OP_REQ_LR)
      {
        lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_FIX;
        lm_sess_req_param.z_request.z_fix_request.q_timeout = sess_start_param.lr_qos * 1000;
        lm_sess_req_param.z_request.z_fix_request.q_qos_accuracy_threshold  = sess_start_param.accuracy_threshold;
        lm_sess_req_param.z_request.z_fix_request.u_dynamic_qos_enabled  = sess_start_param.dynamic_qos_enabled;
      }

      retVal = lm_session_request(&lm_sess_req_param);
    }
  }

  if (retVal == TRUE) /*lint !e506 !e731 */
  {
    if (sess_start_param.op_req & TM_OP_REQ_AIDING_DATA_STATUS)
    {
      lm_info_req_param.e_req_type = LM_REQUEST_AIDING_DATA_WISHLIST;
      lm_info_req_param.z_request.e_wishlist_req_type =
         tm_core_xlate_prtl_to_wishlist_src(sess_start_param.op_mode);

      retVal = lm_request(&lm_info_req_param);
    }

    if (sess_start_param.op_req & TM_OP_REQ_REF_LOC)
    {
      lm_info_req_param.e_req_type = LM_REQUEST_POS_ESTIMATE;
      retVal = lm_request(&lm_info_req_param);
    }
    if (sess_start_param.op_req & TM_OP_REQ_RF_INFO)
    {
      /* Query TM-CM module to get current acquired system */
      tm_cm_iface_get_phone_state(&phone_state_info);

      lm_info_req_param.e_req_type = LM_REQUEST_RF_INFO;

      lm_info_req_param.z_request.z_rf_info_request.e_serv_sys_type
         =  phone_state_info.srv_system;
      retVal = lm_request(&lm_info_req_param);
    }
  }
  return retVal;
}

/*===========================================================================

FUNCTION tm_core_sess_req_continue

DESCRIPTION
  This function is to figure out if TM-Core accepts session continue request from
  protocol sub-module.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_sess_req_continue(tm_prtl_type                       prtl_type,
                                  tm_sess_handle_type                sess_handle,
                                  tm_sess_req_continue_param_s_type  sess_continue_param)
{
  boolean retVal = TRUE;
  lm_session_request_info_s_type lm_sess_req_param;
  lm_request_info_s_type         lm_info_req_param;
  tm_op_req_type                 new_op_req;
  tm_op_req_type                 existing_op_req;
  tm_cm_phone_state_info_s_type phone_state_info;

  (void)prtl_type;
  MSG_MED("TM-Core Recv'd Sess Continue Req: %lu, %lu, %u", prtl_type, sess_handle, tm_core_info.session_info.session_state);

  memset((void *)&lm_sess_req_param, 0, sizeof(lm_session_request_info_s_type));
  memset((void *)&lm_info_req_param, 0, sizeof(lm_request_info_s_type));

  #ifdef FEATURE_GNSS_PERIODIC_PPM
  #error code not present
#endif
  /*
     This is the case where protocol sub-modules need to perform authetication procedure
     Protocol sub-modules finished authenticaion process and now is ready to kick off PD session
     At this time, end-to-end session to LM has NOT been established yet
  */

 /* TM-Core accepts the incoming request, so update TM-Core session info */
  if ((TM_SESS_HANDLE_E911 == tm_core_info.session_info.active_sess_handle) &&
      (prtl_type == TM_PRTL_TYPE_UMTS_CP) &&
      (sess_continue_param.op_req  != TM_OP_REQ_DEL_ASSIST) 
      )
  {
    tm_core_info.session_info.active_sess_handle = sess_handle;
    tm_core_info.session_info.from_protocol      = prtl_type;
    }
  if ((sess_handle == tm_core_info.session_info.active_sess_handle) &&
      (tm_core_info.session_info.session_state == TM_CORE_SESS_STATE_WAIT_AUTH))
  {
    tm_core_update_session_state(TM_CORE_SESS_STATE_ACCESS_GRANTED);

    /* Update information in TM-Core */
    tm_core_info.session_info.op_mode            = sess_continue_param.op_mode;

    /*
      Now request session start to LM
      If life is good, LM should accept it.
      If there is something wrong, LM rejects it (asynchronously), we need to make sure we do recovery properly
    */

    lm_sess_req_param.q_handle = sess_handle;
    /*
     Since smart MSB is supported for 1x non trusted, we send REQUEST_CONTINUE for the MSB case and
     REQUEST_START for everything else.
    */
    if (tm_core_info.session_info.pd_param.pd_option.operation_mode == PDSM_SESSION_OPERATION_MSBASED)
    {
      lm_sess_req_param.e_req_type = LM_SESSION_REQUEST_CONTINUE;
    }
    else
    {
    lm_sess_req_param.e_req_type = LM_SESSION_REQUEST_START;
     MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Setting continue to START: mode=%u", 
           tm_core_info.session_info.pd_param.pd_option.operation_mode);
    }
    lm_sess_req_param.q_req_mask = (uint32)LM_SESSION_REQUEST_SESSION;
    lm_sess_req_param.z_request.z_session_request.e_mode = tm_core_convert_op_mode(sess_continue_param.op_mode, prtl_type);
    lm_sess_req_param.z_request.z_session_request.e_mo_mt = tm_core_mo_mt_mode(sess_handle);
    lm_sess_req_param.z_request.z_session_request.q_num_fixes = sess_continue_param.num_fixes;
    lm_sess_req_param.z_request.z_session_request.q_tbf = sess_continue_param.tbf_ms;

    if( (tm_core_info.session_info.from_protocol == TM_PRTL_TYPE_1X_CP)
      ||(tm_core_info.session_info.from_protocol == TM_PRTL_TYPE_UMTS_CP))
    {
      lm_sess_req_param.z_request.z_session_request.e_up_cp = LM_UP_CP_MODE_CP;
    }
    else
    {
      lm_sess_req_param.z_request.z_session_request.e_up_cp = LM_UP_CP_MODE_UP;
    }

    /*
      For the MSB mode, we fall through to the next (ACCESS_GRANTED) case, and set these request
      parameters there.
    */
    if (tm_core_info.session_info.pd_param.pd_option.operation_mode != PDSM_SESSION_OPERATION_MSBASED)
    {
      if(sess_continue_param.op_req & TM_OP_REQ_PPM)
      {
        lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_CDMA;
        lm_sess_req_param.z_request.z_cdma_request.q_timeout = sess_continue_param.ppm_qos * 1000;
      }

      if(sess_continue_param.op_req & TM_OP_REQ_PRM)
      {
        lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_GPS;
        lm_sess_req_param.z_request.z_gps_request.q_timeout = sess_continue_param.prm_qos * 1000;
        lm_sess_req_param.z_request.z_gps_request.u_qos     = sess_continue_param.prm_prq;
        lm_sess_req_param.z_request.z_gps_request.multi_report_msa = sess_continue_param.multi_report_msa;
      }

      if(sess_continue_param.op_req & TM_OP_REQ_LR)
      {
        lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_FIX;
        lm_sess_req_param.z_request.z_fix_request.q_timeout = sess_continue_param.lr_qos * 1000;
        lm_sess_req_param.z_request.z_fix_request.q_qos_accuracy_threshold  = sess_continue_param.accuracy_threshold;
        lm_sess_req_param.z_request.z_fix_request.u_dynamic_qos_enabled  = sess_continue_param.dynamic_qos_enabled;
      }
    }

    /* Determine if data demod is allow for MSB */
    if(sess_continue_param.op_mode == TM_SESS_OPERATION_MODE_MSB)
    {
      if( (tm_core_get_active_prtl_module(MT_NA) == TM_PRTL_TYPE_1X_UP)
        &&(tm_core_info.config_info.smart_msb == FALSE))
      {
        MSG_MED("Data demod disabled", 0, 0, 0);
        lm_sess_req_param.z_request.z_session_request.u_do_data_demod = FALSE;
      }
      else
      {
        lm_sess_req_param.z_request.z_session_request.u_do_data_demod = TRUE;
      }
    }
    else
    {
      lm_sess_req_param.z_request.z_session_request.u_do_data_demod = TRUE;
    }

    retVal = lm_session_request(&lm_sess_req_param);
    /*
     For the MSB case, the op_requested field is populated in the next block instead of here.
     We do so because the next block expects the tm_core_info.session_info.op_requested value
     to be untouched since the start of the function.
    */
    if (tm_core_info.session_info.pd_param.pd_option.operation_mode != PDSM_SESSION_OPERATION_MSBASED)
    {
    tm_core_info.session_info.op_requested |= sess_continue_param.op_req;
    }
    if(retVal == TRUE) /*lint !e506 !e731 */
    {
      /*
      Always get aiding data wishlist update
      so that we can populate alm_svmask and eph_svmask in PRM report.
      However, if protocol modules does not ask for aiding data update,
      don't forward aiding data wishlist to protocol modules,
      when it comes back from MGP
      */
      lm_info_req_param.e_req_type = LM_REQUEST_AIDING_DATA_WISHLIST;
      lm_info_req_param.z_request.e_wishlist_req_type =
                    tm_core_xlate_prtl_to_wishlist_src(sess_continue_param.op_mode);

      retVal = lm_request(&lm_info_req_param);

      /*
      For the MSB case, these lm_request operations are sent in the next block instead of here,
      since we fall through to that block anyways.
      */
      if (tm_core_info.session_info.pd_param.pd_option.operation_mode != PDSM_SESSION_OPERATION_MSBASED)
      {
        if(sess_continue_param.op_req & TM_OP_REQ_REF_LOC)
        {
          lm_info_req_param.e_req_type = LM_REQUEST_POS_ESTIMATE;
          retVal = lm_request(&lm_info_req_param);
        }

        if(sess_continue_param.op_req & TM_OP_REQ_RF_INFO)
        {
          /* Query TM-CM module to get current acquired system */
          tm_cm_iface_get_phone_state(&phone_state_info);

          lm_info_req_param.e_req_type = LM_REQUEST_RF_INFO;
        #ifdef FEATURE_CGPS_CDMA_IF
          if(phone_state_info.srv_system == CGPS_SRV_SYS_CDMA)
          {
            /* If HDR Call is connected, overwrite the srv_sys to HDR */
            if(tm_cm_iface_hdr_call_connected() == TRUE)
            {
              MSG_MED("HDR call connected",0,0,0);
              phone_state_info.srv_system = CGPS_SRV_SYS_HDR;
            }
          }
         #endif /* FEATURE_CGPS_CDMA_IF */
          lm_info_req_param.z_request.z_rf_info_request.e_serv_sys_type
                               =  phone_state_info.srv_system;
          retVal = lm_request(&lm_info_req_param);
        }
      }
    }

    if(retVal == FALSE)
    {
      /* LM does not allow the request, abort and recover */
      tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
      return retVal;
    }
    /* for the non MSB cases we return right here so as to not fall through to the next block*/
    if (tm_core_info.session_info.pd_param.pd_option.operation_mode != PDSM_SESSION_OPERATION_MSBASED)
    {
      return retVal;
    }
  }
  /*
    Protocol sub-modules has already start a end-to-end session to LM,
    It now calls continue to perform the call flow.

    Note: we intentionally fall through from the previous case into this block for MSB.
    The fall through happens since tm_core_info.session_info.session_state was set to
    TM_CORE_SESS_STATE_ACCESS_GRANTED in the previous block.
    Most of the requested aiding data flags (PPM, PRM, LR) are populated here.
    For the case when we fallback from V2 to SA, only the LR request flag is set in the
    incoming request.
  */
  if( (sess_handle == tm_core_info.session_info.active_sess_handle)
         &&(tm_core_info.session_info.session_state == TM_CORE_SESS_STATE_ACCESS_GRANTED))
  {
#ifdef FEATURE_CGPS_XTRA
    if ( tm_xtra_is_enable()  &&
         (tm_core_info.session_info.msb_xtra_allowed == FALSE) &&
         (sess_continue_param.op_mode == TM_SESS_OPERATION_MODE_MSB) )
    {
      /* code below (featurized by FEATURE_CGPS_XTRA) is for feature controlled by OEM_FEAUTRE_NV(7165) bit 0x200:
         request XTRA download if XTRA data is invalid when MO MSB SUPL is initiated
         For 1x UP (MO or NI), controlled by bit 0x800 of NV 7165 */
      if ( ( (tm_core_info.config_info.q_gnss_oem_feature_mask & NV_GNSS_OEM_FEATURE_MASK_10_REQ_XTRA_DURING_SUPL) &&
             (tm_core_mo_mt_mode(sess_handle) == LM_MO_MT_MODE_MO) ) ||
           ( (tm_core_info.config_info.q_gnss_oem_feature_mask & NV_GNSS_OEM_FEATURE_MASK_12_REQ_XTRA_DURING_1XUP) &&
             (tm_core_mo_mt_mode(sess_handle) == LM_MO_MT_MODE_MO) ) )
      {
        MSG_HIGH("Allow XTRA for AGPS MSB session (oemmask=0x%x handle=0x%x)", 
                 tm_core_info.config_info.q_gnss_oem_feature_mask, sess_handle, 0);
        tm_core_info.session_info.msb_xtra_allowed = TRUE;
        if (tm_xtra_data_validity_check(GPS_TIME_CHECK_SOURCE_PER_FIX))
        {
          // Successfully sent time request to MGP, set the flag
          tm_core_info.session_info.req_xtra_trig_by_agps = TRUE;
        }
      }
      else
      {
        MSG_LOW("No XTRA for MSB session (oemmask=0x%x opmode=%d handle=0x%x)", 
                tm_core_info.config_info.q_gnss_oem_feature_mask, sess_continue_param.op_mode, sess_handle);
      }
    }
#endif /* FEATURE_CGPS_XTRA */
 
    /*
      Now forward continue request to LM
      If life is good, LM should accept it.
      If there is something wrong, LM rejects it (asynchronously), we need to make sure we do recovery properly
    */

    if (tm_core_info.session_info.op_mode == TM_SESS_OPERATION_MODE_MSB_OPT &&
        sess_continue_param.op_mode == TM_SESS_OPERATION_MODE_MSB)
    {
      /* if protocol sent a continue for the optimal modes or MSB, just continue */
      tm_core_info.session_info.op_mode            = sess_continue_param.op_mode;
    }
    else
    {
      /* If operation mode has been changed (Optimal mode fallback sessions, or E911 that starts with MSB and now the network
         requests MSA for example ), notify LM to change the mode*/
      if ((tm_core_info.session_info.op_mode != sess_continue_param.op_mode )
          && (sess_continue_param.op_mode != TM_SESS_OPERATION_MODE_OTDOA))
      {
      
        MSG_4(MSG_SSID_GPSSM, MSG_LEGACY_MED, 
              "Exiting op mode (%u) != cont.op_mode (%u), tm_core.op_requested = %x",
              tm_core_info.session_info.op_mode, 
              sess_continue_param.op_mode,
              tm_core_info.session_info.op_requested,
              sess_continue_param.op_req);  
      
        tm_core_info.session_info.op_mode = sess_continue_param.op_mode;
        lm_sess_req_param.q_handle = sess_handle;
        lm_sess_req_param.e_req_type = LM_SESSION_REQUEST_CONTINUE;
        lm_sess_req_param.q_req_mask = (uint32)LM_SESSION_REQUEST_SESSION;
        lm_sess_req_param.z_request.z_session_request.e_mode = tm_core_convert_op_mode(sess_continue_param.op_mode, prtl_type);
        lm_sess_req_param.z_request.z_session_request.e_mo_mt = tm_core_mo_mt_mode(sess_handle);

        /* Determine if data demod is allow for MSB */
        if (sess_continue_param.op_mode == TM_SESS_OPERATION_MODE_MSB)
        {
          if ( (tm_core_get_active_prtl_module(MT_NA) == TM_PRTL_TYPE_1X_UP)
               &&(tm_core_info.config_info.smart_msb == FALSE))
          {
            MSG_MED("Data demod disabled", 0, 0, 0);
            lm_sess_req_param.z_request.z_session_request.u_do_data_demod = FALSE;
          }
          else
          {
            lm_sess_req_param.z_request.z_session_request.u_do_data_demod = TRUE;
          }
        }
        else
        {
          lm_sess_req_param.z_request.z_session_request.u_do_data_demod = TRUE;
        }

        retVal = lm_session_request(&lm_sess_req_param);
      }

      if (retVal)
      {
        lm_sess_req_param.q_req_mask = 0;

        /*
           For operatons which have not been previously started, use start to LM
           For operatons which have already been started, use continue to LM
        */
        existing_op_req = sess_continue_param.op_req & tm_core_info.session_info.op_requested;
        new_op_req      = sess_continue_param.op_req & ~(existing_op_req);
        
        MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED,
              "continue_param.op_req = %lu, tm_core_info.session_info.op_requested = %lu",
              sess_continue_param.op_req, tm_core_info.session_info.op_requested);

        /*
           For operatons which have not been previously started, use start to LM
        */
        if (new_op_req & (TM_OP_REQ_PPM | TM_OP_REQ_PRM | TM_OP_REQ_LR))
        {
          lm_sess_req_param.q_handle   = sess_handle;
          lm_sess_req_param.e_req_type = LM_SESSION_REQUEST_START;

          if (new_op_req & TM_OP_REQ_PPM)
          {
            lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_CDMA;
            lm_sess_req_param.z_request.z_cdma_request.q_timeout = sess_continue_param.ppm_qos * 1000;
          }

          if (new_op_req & TM_OP_REQ_PRM)
          {
            lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_GPS;
            lm_sess_req_param.z_request.z_gps_request.q_timeout = sess_continue_param.prm_qos * 1000;
            lm_sess_req_param.z_request.z_gps_request.u_qos     = sess_continue_param.prm_prq;
            lm_sess_req_param.z_request.z_gps_request.multi_report_msa = sess_continue_param.multi_report_msa;
          }

          if (new_op_req & TM_OP_REQ_LR)
          {
            lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_FIX;
            lm_sess_req_param.z_request.z_fix_request.q_timeout = sess_continue_param.lr_qos * 1000;
            lm_sess_req_param.z_request.z_fix_request.q_qos_accuracy_threshold  = sess_continue_param.accuracy_threshold;
            lm_sess_req_param.z_request.z_fix_request.u_dynamic_qos_enabled  = sess_continue_param.dynamic_qos_enabled;
          }

          retVal = lm_session_request(&lm_sess_req_param);

        }

        tm_core_info.session_info.op_requested |= new_op_req;


        /*
           For operatons which have already been previously started, use continue to LM
        */
        if (existing_op_req & (TM_OP_REQ_PPM | TM_OP_REQ_PRM | TM_OP_REQ_LR))
        {
          lm_sess_req_param.q_handle   = sess_handle;
          lm_sess_req_param.e_req_type = LM_SESSION_REQUEST_CONTINUE;

          if (existing_op_req & TM_OP_REQ_PPM)
          {
            lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_CDMA;
            lm_sess_req_param.z_request.z_cdma_request.q_timeout = sess_continue_param.ppm_qos * 1000;
          }

          if (existing_op_req & TM_OP_REQ_PRM)
          {
            lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_GPS;
            lm_sess_req_param.z_request.z_gps_request.q_timeout = sess_continue_param.prm_qos * 1000;
            lm_sess_req_param.z_request.z_gps_request.u_qos     = sess_continue_param.prm_prq;
            lm_sess_req_param.z_request.z_gps_request.multi_report_msa = sess_continue_param.multi_report_msa;
          }

          if (existing_op_req & TM_OP_REQ_LR)
          {
            lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_FIX;
            lm_sess_req_param.z_request.z_fix_request.q_timeout = sess_continue_param.lr_qos * 1000;
            lm_sess_req_param.z_request.z_fix_request.q_qos_accuracy_threshold  = sess_continue_param.accuracy_threshold;
            lm_sess_req_param.z_request.z_fix_request.u_dynamic_qos_enabled  = sess_continue_param.dynamic_qos_enabled;
          }

          retVal = lm_session_request(&lm_sess_req_param);
        }
      }
    }

    if(retVal == TRUE) /*lint !e506 !e731 */
    {
      if(sess_continue_param.op_req & TM_OP_REQ_AIDING_DATA_STATUS)
      {
        lm_info_req_param.e_req_type = LM_REQUEST_AIDING_DATA_WISHLIST;
        lm_info_req_param.z_request.e_wishlist_req_type =
                 tm_core_xlate_prtl_to_wishlist_src(sess_continue_param.op_mode);

        retVal = lm_request(&lm_info_req_param);
      }

      if(sess_continue_param.op_req & TM_OP_REQ_REF_LOC)
      {
        lm_info_req_param.e_req_type = LM_REQUEST_POS_ESTIMATE;
        retVal = lm_request(&lm_info_req_param);
      }
      if(sess_continue_param.op_req & TM_OP_REQ_RF_INFO)
      {
        /* Query TM-CM module to get current acquired system */
        tm_cm_iface_get_phone_state(&phone_state_info);

        lm_info_req_param.e_req_type = LM_REQUEST_RF_INFO;

#ifdef FEATURE_CGPS_CDMA_IF
        if(phone_state_info.srv_system == CGPS_SRV_SYS_CDMA)
        {
          /* If HDR Call is connected, overwrite the srv_sys to HDR */
          if(tm_cm_iface_hdr_call_connected() == TRUE)
          {
            MSG_MED("HDR call connected",0,0,0);
            phone_state_info.srv_system = CGPS_SRV_SYS_HDR;
          }
        }
#endif /* FEATURE_CGPS_CDMA_IF */
        lm_info_req_param.z_request.z_rf_info_request.e_serv_sys_type
                               =  phone_state_info.srv_system;
        retVal = lm_request(&lm_info_req_param);
      }
    }

    if(retVal == FALSE)
    {
      /* LM does not allow the request, abort and recover */
      tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
    }
  }
  /*
    Return error if either the session handle is invalid or the session state was
    neither WAIT_AUTH nor GRANTED.
 */
  if( (sess_handle != tm_core_info.session_info.active_sess_handle) ||
      ((tm_core_info.session_info.session_state != TM_CORE_SESS_STATE_WAIT_AUTH) &&
       (tm_core_info.session_info.session_state != TM_CORE_SESS_STATE_ACCESS_GRANTED)))
  {
    MSG_ERROR("Continue req rejected", 0, 0, 0);
    retVal = FALSE;
  }

  return retVal;
}

/*===========================================================================

FUNCTION tm_core_sess_req_polling_data

DESCRIPTION
  This function is used to poll LM is data is available

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_sess_req_polling_data(tm_prtl_type                           prtl_type,
                                      tm_sess_handle_type                    sess_handle,
                                      tm_sess_req_polling_data_param_s_type  polling_data_param)
{
  boolean     retVal         = FALSE;

  lm_session_request_info_s_type lm_sess_req_param;

  MSG_MED("TM-Core Recv'd Polling Data Req", 0, 0, 0);

  memset((void *)&lm_sess_req_param, 0, sizeof(lm_session_request_info_s_type));

  /*
   * Only accept if
   *** continue req belongs to active session and session is in access granted state OR
   *** continue req belongs to Periodic PPM Session
   */
  if(
     ((sess_handle == tm_core_info.session_info.active_sess_handle) &&
      (tm_core_info.session_info.session_state == TM_CORE_SESS_STATE_ACCESS_GRANTED))
    )
  {
    if(polling_data_param.op_req & TM_OP_REQ_PPM)
    {
      lm_sess_req_param.q_handle    = tm_core_info.session_info.active_sess_handle;
      lm_sess_req_param.e_req_type  = LM_SESSION_REQUEST_CONTINUE;
      lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_CDMA_GET_RESULTS;
        retVal = lm_session_request(&lm_sess_req_param);
      }

    if(retVal == FALSE)
    {
      /* LM does not allow the request, abort and recover */
      tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
    }
  }
  else
  {
    MSG_ERROR("Polling Data req rejected", 0, 0, 0);
    retVal = FALSE;
  }

  return retVal;
}
/*===========================================================================

FUNCTION tm_core_msb_sess_handover

DESCRIPTION
  This function is called when standalone module decide other protocol sub-module
  should be handling MSB session.

  It pickup the current active protocol sub-module to proceed with MSB session.

  The transition is invisible to applicatons

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_msb_sess_handover(void)
{
  int                                prtl_tbl_idx = -1;
  tm_prtl_type                       active_prtl;
  prtl_start_sess_req_result_e_type  req_result;
  tm_sess_req_start_param_s_type     start_actions;
  tm_sess_req_continue_param_s_type  continue_param;
  boolean                            retVal = FALSE;

  /* the client type here does not matter, as long as it is not NI */
  active_prtl = tm_core_get_active_prtl_module(MT_NA);

  prtl_tbl_idx = tm_core_get_prtl_idx_by_type(active_prtl);

  if((prtl_tbl_idx >= 0) && (prtl_tbl_idx < TM_PRTL_NUM))
  {
    if(tm_core_info.prtl_func_cb_table[prtl_tbl_idx].start_sess_req_fp)
    {
      req_result = tm_core_info.prtl_func_cb_table[prtl_tbl_idx].start_sess_req_fp(
                    tm_core_info.session_info.active_sess_handle,
                    &(tm_core_info.session_info.pd_param),
                    &start_actions);

      if(req_result == PRTL_START_SESS_REQ_ALLOWED)
      {
        /* TM-Core is already in a active session, we call continue to do seemless transition */ 
        continue_param.op_mode            = start_actions.op_mode;
        continue_param.op_req             = start_actions.op_req;
        continue_param.accuracy_threshold = start_actions.accuracy_threshold;
        continue_param.ppm_qos            = start_actions.ppm_qos;
        continue_param.prm_qos            = start_actions.prm_qos;
        continue_param.lr_qos             = start_actions.lr_qos;
        continue_param.dynamic_qos_enabled  = start_actions.dynamic_qos_enabled;
        continue_param.multi_report_msa     = start_actions.multi_report_msa;

        tm_core_info.session_info.from_protocol      = active_prtl;

        retVal = tm_core_sess_req_continue(active_prtl,
                                           tm_core_info.session_info.active_sess_handle,
                                           continue_param);
      }
      else if(req_result == PRTL_START_SESS_REQ_WAIT)
      {
        /* TM-Core is already in a active session, we call continue to do seemless transition */ 
        continue_param.op_mode            = start_actions.op_mode;
        continue_param.op_req             = start_actions.op_req;
        continue_param.accuracy_threshold = start_actions.accuracy_threshold;
        continue_param.ppm_qos            = start_actions.ppm_qos;
        continue_param.prm_qos            = start_actions.prm_qos;
        continue_param.lr_qos             = start_actions.lr_qos;      
        continue_param.dynamic_qos_enabled = start_actions.dynamic_qos_enabled;      
        continue_param.multi_report_msa    = start_actions.multi_report_msa;
      
        tm_core_info.session_info.from_protocol      = active_prtl;
        retVal = tm_core_sess_req_continue(active_prtl, tm_core_info.session_info.active_sess_handle, continue_param);

        /* If prototocol return PRTL_START_SESS_REQ_WAIT, set tm-core session state to wait */
        /* Protocol sub-modules should call session continue to continue this session */
        tm_core_info.session_info.from_protocol      = active_prtl;
        tm_core_update_session_state(TM_CORE_SESS_STATE_WAIT_AUTH);
      }
      else
      {
        /* If protocol return PRTL_START_SESS_REQ_NOT_ALLOWED, report to PD API */
        MSG_ERROR("Protocol sub-module rejects start session request", 0, 0, 0);
        retVal = FALSE;
      }
    }
    else
    {
      MSG_ERROR("Invalid prtlTableIndex", 0, 0, 0);
      retVal = FALSE;
    }

  }
  else
  {

    MSG_ERROR("Invalid prtlTableIndex", 0, 0, 0);
    retVal = FALSE;
  }

  if(retVal == FALSE)
  {
    //<<<[+ by Liao] Revist to clean LM and protocols properly
    tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
    //>>>[+ End]
  }
}


/*===========================================================================

FUNCTION tm_core_sess_req_stop

DESCRIPTION
  This function is to figure out if TM-Core accepts stop request from
  protocol sub-module.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_sess_req_stop(tm_prtl_type                       prtl_type,
                              tm_sess_handle_type                sess_handle,
                              tm_sess_req_stop_param_s_type      sess_stop_param)
{
  boolean retVal = FALSE;
  lm_session_request_info_s_type lm_sess_req_param;

  MSG_MED("TM-Core Recv'd Sess Stop Req from %d, sess_handle %d", prtl_type, sess_handle, 0);
  memset((void *)&lm_sess_req_param, 0, sizeof(lm_session_request_info_s_type));

    /* If stop req belongs to active session and session is not in init state */
  if( (sess_handle == tm_core_info.session_info.active_sess_handle)
    &&(tm_core_info.session_info.session_state != TM_CORE_SESS_STATE_INIT))
  {
    if(sess_stop_param.stop_type == TM_STOP_TYPE_OPERATION)
    {
      if( sess_stop_param.op_to_stop & (TM_OP_REQ_PPM | TM_OP_REQ_PRM | TM_OP_REQ_LR))
      {
        lm_sess_req_param.e_req_type = LM_SESSION_REQUEST_STOP;
        lm_sess_req_param.q_handle   = sess_handle;

        if(sess_stop_param.op_to_stop & TM_OP_REQ_PPM)
        {
          lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_CDMA;
        }

        if(sess_stop_param.op_to_stop & TM_OP_REQ_PRM)
        {
          lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_GPS;
        }

        if(sess_stop_param.op_to_stop & TM_OP_REQ_LR)
        {
          lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_FIX;
        }

        tm_core_info.session_info.op_requested &= ~sess_stop_param.op_to_stop;

        retVal = lm_session_request(&lm_sess_req_param);
      }

      if(retVal == FALSE)
      {
        tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
      }
    }
    else if(sess_stop_param.stop_type == TM_STOP_TYPE_SESSION)
    {
      /* MSB session handover logic
         Only allow session handover when all the following conditions are satisfied
         1. OP mode is MSB
         2. Phone is not in OoS (because data download is needed)
      */
      if(sess_stop_param.stop_reason == TM_STOP_REASON_MSB_NEED_DLOAD)
      {
        tm_core_msb_sess_handover();
      }
      else
      {
        /* No need to abort protocol sub-module again, because protocol sub-module initiated the abort */
        tm_core_abort_recover(FALSE, sess_stop_param.stop_reason); /*lint !e506 !e730 */
      }
      retVal = TRUE; /*lint !e506 */
    }
  }
  else if( (sess_handle == TM_SESS_HANDLE_FORCE_STOP)
         &&(tm_core_info.session_info.session_state != TM_CORE_SESS_STATE_INIT)
         &&(sess_stop_param.stop_type == TM_STOP_TYPE_SESSION))
  {
    MSG_HIGH("Protocl forced session termination", 0, 0, 0);
    tm_core_abort_recover(TRUE, sess_stop_param.stop_reason); /*lint !e506 !e730 */
    retVal = TRUE;
  }
  else
  {
    MSG_HIGH("Stop req rejected", 0, 0, 0);
    retVal = FALSE;
  }
#ifdef FEATURE_CGPS_XTRA_T
  /* Notify TLE's XTM task that thre is no active AGPS session. */
  if (retVal == TRUE)
  {
    if (sess_stop_param.stop_type == TM_STOP_TYPE_SESSION)
    {
      (void) tle_SendAGPSEvents(C_TLE_EVENT_AGPS_PDCOMM_DISCONNECTED);
    }

  }
#endif
  return retVal;
}


/*===========================================================================

FUNCTION tm_core_recv_data

DESCRIPTION
  This function is to handle data sent from protocol sub-modules.

DEPENDENCIES

RETURN VALUE
  TURE: If TM-Core sub-module accepts it
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
boolean tm_core_recv_data(tm_prtl_type                     prtl_type,
                          tm_sess_handle_type              sess_handle,
                          tm_post_data_e_type              data_type,
                          const tm_post_data_payload_type  *data_payload_ptr
                         )
{
  boolean                  retVal        = FALSE;
  lm_info_s_type           *lm_data_ptr  = NULL;
  sm_GnssFixRptStructType  z_fix;
  gnss_AssistDataUnionType *p_AssistData = NULL;
  sm_gm_pos_fix_info       fix_info;

  MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_MED, "TM-Core Recv'd data type %d from protocol %d", data_type, prtl_type );

  if ( data_payload_ptr == NULL )
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL ptr" );
    return ( FALSE );
  }

  /* Post Data is allowed only when session is in access granted state */
  if ( (sess_handle == tm_core_info.session_info.active_sess_handle)
       && (tm_core_info.session_info.session_state == TM_CORE_SESS_STATE_ACCESS_GRANTED) )
  {
    lm_data_ptr = (lm_info_s_type *)pdsm_getbuf( sizeof(lm_info_s_type) );

    if ( lm_data_ptr == NULL )
    {
      MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Fail to get buffer" );
      tm_core_abort_recover( TRUE, TM_STOP_REASON_GENERAL_ERROR ); /*lint !e506 !e730 */
      return ( FALSE );
    }
    memset( lm_data_ptr, 0, sizeof(*lm_data_ptr) );

    p_AssistData = &(lm_data_ptr->z_info.z_aiding_data.z_assist_data.z_AssistData);
    switch ( data_type )
    {
       case TM_POST_DATA_TYPE_AA:
         lm_data_ptr->e_info_type = LM_SESSION_INFO_AIDING_DATA;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_AssistanceType = MGP_GNSS_ASSIST_DATA_ACQ_ASSIST;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_GnssType = MGP_GNSS_TYPE_GPS;
         p_AssistData->z_SvAcqAssistInfo = data_payload_ptr->gnss_aa_data;
         retVal = lm_info( lm_data_ptr );
         break;

       case TM_POST_DATA_TYPE_SV_DIR:
         lm_data_ptr->e_info_type = LM_SESSION_INFO_AIDING_DATA;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_AssistanceType = MGP_GNSS_ASSIST_DATA_SV_DIR;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_GnssType = MGP_GNSS_TYPE_GPS;
         p_AssistData->z_GnssSvDirInfo = data_payload_ptr->gnss_sv_dir;
         retVal = lm_info( lm_data_ptr );
         break;

       case TM_POST_DATA_TYPE_SA:
         /*AHSIAU_OBSOLETE?
       lm_data_ptr->e_info_type = LM_SESSION_INFO_AIDING_DATA;
       lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_AssistanceType = MGP_GNSS_ASSIST_DATA_SA_DATA;
       lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_GnssType = MGP_GNSS_TYPE_GPS;
       p_AssistData->z_SADataInfo = data_payload_ptr->sa_data;
       retVal = lm_info(lm_data_ptr);
*/      break;

       case TM_POST_DATA_TYPE_IONO:
         lm_data_ptr->e_info_type = LM_SESSION_INFO_AIDING_DATA;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_AssistanceType = MGP_GNSS_ASSIST_DATA_IONO;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_GnssType = MGP_GNSS_TYPE_GPS;
         p_AssistData->z_IonoInfo = data_payload_ptr->iono_data;
         retVal = lm_info( lm_data_ptr );
         break;

       case TM_POST_DATA_TYPE_EPH:
         lm_data_ptr->e_info_type = LM_SESSION_INFO_AIDING_DATA;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_AssistanceType = MGP_GNSS_ASSIST_DATA_EPH;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_GnssType = MGP_GNSS_TYPE_GPS;
         p_AssistData->z_EphInfo = data_payload_ptr->eph_data;
         retVal = lm_info( lm_data_ptr );
         break;

       case TM_POST_DATA_TYPE_ALM:
         lm_data_ptr->e_info_type = LM_SESSION_INFO_AIDING_DATA;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_AssistanceType = MGP_GNSS_ASSIST_DATA_ALM;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_GnssType = MGP_GNSS_TYPE_GPS;
         p_AssistData->z_AlmInfo = data_payload_ptr->alm_data;
         retVal = lm_info( lm_data_ptr );
         break;

       case TM_POST_DATA_TYPE_SV_NO_EXIST:
         lm_data_ptr->e_info_type = LM_SESSION_INFO_AIDING_DATA;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_AssistanceType = MGP_GNSS_ASSIST_DATA_SV_NO_EXIST;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_GnssType = MGP_GNSS_TYPE_GPS;
         p_AssistData->z_SvNoExistInfo = data_payload_ptr->sv_no_exist_data;
         retVal = lm_info( lm_data_ptr );
         break;

       case TM_POST_DATA_TYPE_GNSS_SV_NO_EXIST:
         lm_data_ptr->e_info_type = LM_SESSION_INFO_AIDING_DATA;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_AssistanceType = MGP_GNSS_ASSIST_DATA_SV_NO_EXIST;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_GnssType = MGP_GNSS_TYPE_GLO;
         p_AssistData->z_GloNonExistenceInfo = data_payload_ptr->sv_no_exist_glo_data;
         retVal = lm_info( lm_data_ptr );
         break;

       case TM_POST_DATA_TYPE_SEED:
       case TM_POST_DATA_TYPE_LR:

         /*Generate NMEA*/
         if ( data_type == TM_POST_DATA_TYPE_LR )
         {

           /* even if no NMEA output, still need to call this function
              as z_fix will be used in tm_core_generate_ext_pd_data() call */
           tm_nmea_get_gps_fix_from_refpos( &(data_payload_ptr->seed_data),
                                            &z_fix );

           TM_CORE_OUTPUT_NMEA_GPGGA( &z_fix, FALSE );
           TM_CORE_OUTPUT_NMEA_PQXFI( &z_fix, FALSE );
           TM_CORE_OUTPUT_NMEA_GNGNS( &z_fix, FALSE );
           TM_CORE_OUTPUT_NMEA_GPVTG( &z_fix );
           TM_CORE_OUTPUT_NMEA_GPRMC( &z_fix );
           TM_CORE_OUTPUT_NMEA_GPGSA( &z_fix );
           TM_CORE_OUTPUT_NMEA_GNGSA( &z_fix );
           TM_CORE_OUTPUT_NMEA_PQGSA( &z_fix );

           /* Generate extended pd data */
           tm_core_generate_ext_pd_data( &z_fix );

           /*Send the LR to Geofence module*/
           fix_info.fix_type = GEOFENCE_POS_FIX_TYPE_FINAL;
           fix_info.y_pos_fix_ext_info.z_gnss_fix_ext_info.b_is_nw_asst_fix = TRUE;
           fix_info.fix_src = GEOFENCE_POS_FIX_SRC_GNSS;
           fix_info.pos_fix = z_fix;

           (void)gm_position_update( &fix_info );
         }

         /*Send seed or reference position over to LM*/
         lm_data_ptr->e_info_type = LM_SESSION_INFO_AIDING_DATA;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_AssistanceType = MGP_GNSS_ASSIST_DATA_REF_LOC;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_GnssType = MGP_GNSS_TYPE_GPS;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.u_Force = FALSE;
         p_AssistData->z_RefLocInfo = data_payload_ptr->seed_data;
         retVal = lm_info( lm_data_ptr );
         /* Force TM to sleep for 2ms so that LM can finish processing ref Loc.
         Without this delay LM is unable to consume IPCs as fast as TM sends them
         resulting in IPC mem exhaustion. Tests have shown that LM can sometimes take
         upto 2ms to finish processing ref Loc.
         */
         (void)os_SleepMs_NonDeferrable( 2 );
         break;

       case TM_POST_DATA_TYPE_REFTIME:
         lm_data_ptr->e_info_type = LM_SESSION_INFO_AIDING_DATA;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_AssistanceType = MGP_GNSS_ASSIST_DATA_REF_TIME;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_GnssType = MGP_GNSS_TYPE_GPS;
         p_AssistData->z_RefTimeInfo = data_payload_ptr->reftime_data;
         retVal = lm_info( lm_data_ptr );
         /* Force TM to sleep for 2ms so that LM can finish processing ref Time.
         Without this delay LM is unable to consume IPCs as fast as TM sends them
         resulting in IPC mem exhaustion. Tests have shown that LM can sometimes take
         upto 2ms to finish processing refTime.
         */
         (void)os_SleepMs_NonDeferrable( 2 );
         break;

       case TM_POST_DATA_TYPE_UTC_MODEL:
         lm_data_ptr->e_info_type = LM_SESSION_INFO_AIDING_DATA;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_AssistanceType = MGP_GNSS_ASSIST_DATA_UTC;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_GnssType = MGP_GNSS_TYPE_GPS;
         p_AssistData->z_UtcInfo = data_payload_ptr->utc_model_data;
         retVal = lm_info( lm_data_ptr );
         break;

       case TM_POST_DATA_TYPE_PREFIX_PPM:
         lm_data_ptr->e_info_type = LM_SESSION_INFO_AIDING_DATA;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_AssistanceType = MGP_GNSS_ASSIST_DATA_PRE_FIX_PPM;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_GnssType = MGP_GNSS_TYPE_GPS;
         p_AssistData->z_PreFixPpmInfo = data_payload_ptr->ppm_data;
         retVal = lm_info( lm_data_ptr );
         break;

       case TM_POST_DATA_TYPE_SV_UNHEALTHY:
         lm_data_ptr->e_info_type = LM_SESSION_INFO_AIDING_DATA;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_AssistanceType = MGP_GNSS_ASSIST_DATA_SV_UNHEALTHY;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_GnssType = MGP_GNSS_TYPE_GPS;
         p_AssistData->z_SvUnhealthyInfo = data_payload_ptr->sv_unhealthy_data;
         retVal = lm_info( lm_data_ptr );
         break;

         /**** Glonass assistance data *****/
       case TM_POST_DATA_TYPE_REFTIME_GLO:
         lm_data_ptr->e_info_type = LM_SESSION_INFO_AIDING_DATA;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_AssistanceType = MGP_GNSS_ASSIST_DATA_REF_TIME;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_GnssType = MGP_GNSS_TYPE_GLO;
         p_AssistData->z_GloRefTimeInfo = data_payload_ptr->reftime_glo_data;
         retVal = lm_info( lm_data_ptr );
         break;

       case TM_POST_DATA_TYPE_EPH_GLO:
         lm_data_ptr->e_info_type = LM_SESSION_INFO_AIDING_DATA;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_AssistanceType = MGP_GNSS_ASSIST_DATA_EPH;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_GnssType = MGP_GNSS_TYPE_GLO;
         p_AssistData->z_GloEphInfo = data_payload_ptr->eph_glo_data;
         retVal = lm_info( lm_data_ptr );
         break;

       case TM_POST_DATA_TYPE_ALM_GLO:
         lm_data_ptr->e_info_type = LM_SESSION_INFO_AIDING_DATA;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_AssistanceType = MGP_GNSS_ASSIST_DATA_ALM;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_GnssType = MGP_GNSS_TYPE_GLO;
         p_AssistData->z_GloAlmInfo = data_payload_ptr->alm_glo_data;
         retVal = lm_info( lm_data_ptr );
         break;

       case TM_POST_DATA_TYPE_UTC_MODEL_GLO:
         lm_data_ptr->e_info_type = LM_SESSION_INFO_AIDING_DATA;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_AssistanceType = MGP_GNSS_ASSIST_DATA_UTC;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_GnssType = MGP_GNSS_TYPE_GLO;
         p_AssistData->z_GloUtcInfo = data_payload_ptr->utc_model_glo_data;
         retVal = lm_info( lm_data_ptr );
         break;

       case TM_POST_DATA_TYPE_TIME_MODEL_GLO:
         lm_data_ptr->e_info_type = LM_SESSION_INFO_AIDING_DATA;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_AssistanceType = MGP_GNSS_ASSIST_DATA_GNSS_TIME_MODEL;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_GnssType = MGP_GNSS_TYPE_GLO;
         p_AssistData->z_TimeModelInfo = data_payload_ptr->gnss_time_model_data;
         retVal = lm_info( lm_data_ptr );
         break;

       case TM_POST_DATA_TYPE_AA_GLO:
         lm_data_ptr->e_info_type = LM_SESSION_INFO_AIDING_DATA;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_AssistanceType = MGP_GNSS_ASSIST_DATA_ACQ_ASSIST;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_GnssType = MGP_GNSS_TYPE_GLO;
         p_AssistData->z_SvAcqAssistInfo = data_payload_ptr->gnss_aa_data;
         retVal = lm_info( lm_data_ptr );
         break;

       case TM_POST_DATA_TYPE_SV_DIR_GLO:
         lm_data_ptr->e_info_type = LM_SESSION_INFO_AIDING_DATA;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_AssistanceType = MGP_GNSS_ASSIST_DATA_SV_DIR;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_GnssType = MGP_GNSS_TYPE_GLO;
         p_AssistData->z_GnssSvDirInfo = data_payload_ptr->gnss_sv_dir;
         retVal = lm_info( lm_data_ptr );
         break;

       case TM_POST_DATA_TYPE_GNSS_SV_HEALTH_INFO: /* GLONASS RTI only */
         MSG_HIGH( "Sending GNSS (%d) Unhealhty SV info to LM 0x%x", data_payload_ptr->gnss_sv_health_info.e_GnssType,
                   data_payload_ptr->gnss_sv_health_info.q_BadSvMask, 0 );
         lm_data_ptr->e_info_type = LM_SESSION_INFO_AIDING_DATA;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_AssistanceType = MGP_GNSS_ASSIST_DATA_ADV_HEALTH;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_GnssType = data_payload_ptr->gnss_sv_health_info.e_GnssType;
         p_AssistData->z_GloHealthInfo = data_payload_ptr->gnss_sv_health_info;
         retVal = lm_info( lm_data_ptr );
         break;

       case TM_POST_DATA_TYPE_REFPOS_RPT:
         tm_nmea_get_gps_fix_from_refpos( &(data_payload_ptr->seed_data),
                                          &z_fix );

         /* Generate extended pd data */
         tm_core_generate_ext_pd_data( &z_fix );

         /*Send the LR to Geofence module*/
         fix_info.fix_type = GEOFENCE_POS_FIX_TYPE_FINAL;
         fix_info.y_pos_fix_ext_info.z_gnss_fix_ext_info.b_is_nw_asst_fix = TRUE;
         fix_info.fix_src = GEOFENCE_POS_FIX_SRC_GNSS;
         fix_info.pos_fix = z_fix;

         (void)gm_position_update( &fix_info );

         break;

       case TM_POST_DATA_TYPE_OTDOA_AD:
#ifdef FEATURE_LOC_LTE_OTDOA
         // send to SM_OTDOA module.
         tm_core_info.session_info.op_requested |= TM_OP_REQ_OTDOA;
         retVal = sm_loc_lte_otdoa_start_cmd( &data_payload_ptr->otdoa_assist_data.otdoa_assist_data,
                                              data_payload_ptr->otdoa_assist_data.q_operation_mode,
                                              data_payload_ptr->otdoa_assist_data.q_sess_type );
          b_otdoa_sess_start_sent = TRUE;
#endif
         break;

       case TM_POST_DATA_TYPE_OTDOA_STOP:
#ifdef FEATURE_LOC_LTE_OTDOA
         /* Send OTDOA stop command if the engine is running */
         if (  TRUE == b_otdoa_sess_start_sent )
         {
           retVal = sm_loc_lte_otdoa_stop_cmd( data_payload_ptr->otdoa_stop.e_stop_reason, prtl_type );
           b_otdoa_sess_start_sent = FALSE;
         }
         else
         {
           MSG( MSG_SSID_GPSSM, MSG_LEGACY_MED, "OTDOA engine already stopped or not started " );
           retVal = TRUE; /* If OTDOA engine already stopped or not started isn't an error case. */
         }
#endif
         break;

       case TM_POST_DATA_TYPE_FTA_DATA:
         lm_data_ptr->e_info_type = LM_SESSION_INFO_AIDING_DATA;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_AssistanceType = MGP_GNSS_ASSIST_DATA_FTA_DATA;
         lm_data_ptr->z_info.z_aiding_data.z_assist_data.e_GnssType = MGP_GNSS_TYPE_GPS;
         p_AssistData->z_FTAInfo = data_payload_ptr->fta_assist_data;
         retVal = lm_info( lm_data_ptr );
         break;

       default:
         break;
    }

    (void)pdsm_freebuf( (char *)lm_data_ptr );

    if ( retVal == TRUE ) /*lint !e506 !e731 */
    {
      /* Notify PDAPI if needed */

      /* Fire Diag Events */
    }
    else
    {
      /* LM does not allow the request, abort and recover */
      tm_core_abort_recover( TRUE, TM_STOP_REASON_GENERAL_ERROR ); /*lint !e506 !e730 */
      retVal = FALSE;
    }
  }
  else
  {
    MSG_3( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Data rejected. Sess %lu, Active %lu, State %d", sess_handle,
           tm_core_info.session_info.active_sess_handle, tm_core_info.session_info.session_state );
    retVal = FALSE;
  }
  return ( retVal );
}

/*===========================================================================

FUNCTION tm_core_recv_event

DESCRIPTION
  This function is to handle PD API Events sent from protocol sub-modules.

DEPENDENCIES

RETURN VALUE
  TRUE: If TM-Core sub-module accepts it
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
boolean tm_core_recv_event
(
  tm_prtl_type               prtl_type,
  tm_sess_handle_type        sess_handle,
  pdsm_pd_event_type         pd_event,
  pdsm_pd_info_s_type        *pd_info_ptr,
  gnss_NavSlnFlagsStructType* p_PosSrcFlags
)
{
  tm_pdapi_pd_event_s_type p_pd_info_type;
  boolean req_pos = FALSE;
  pdsm_ext_status_info_s_type   pd_ext_status_info;
  utc_time z_utc_time;
  uint32 gps_utc_offset;
  (void)prtl_type;

  /* Init */
  memset(&p_pd_info_type, 0, sizeof(p_pd_info_type));

  MSG_MED("TM-Core Recv'd PD events (%d) from protocol", pd_event, 0, 0);


  if(pd_info_ptr == NULL)
  {
    MSG_ERROR("Null pd_info_ptr", 0, 0, 0);
    return FALSE;
  }

  if ((pd_event == PDSM_PD_EVENT_COMM_FAILURE) &&
      ((tm_core_info.session_info.from_protocol == TM_PRTL_TYPE_1X_UP) ||
      (tm_core_info.session_info.from_protocol == TM_PRTL_TYPE_UMTS_UP)) &&
      (tm_core_info.session_info.op_mode == TM_SESS_OPERATION_MODE_MSB) &&
      (tm_core_info.session_info.msb_xtra_allowed == FALSE))
  {
#ifdef FEATURE_CGPS_XTRA
    MSG_HIGH("XTRA - allow XTRA after DLOAD COMM failure", 0,0,0);
    tm_core_info.session_info.msb_xtra_allowed = TRUE;

    if(!tm_xtra_data_validity_check(GPS_TIME_CHECK_SOURCE_PER_FIX))
    {
#ifdef FEATURE_CGPS_XTRA_T
      if(!tm_xtra_invoke_xtrat(PDSM_XTRA_CMD_ERR_NONE))
      {
        MSG_ERROR("Failed to invoke XTRA-T for MSB fallback", 0, 0, 0);
      }
      else
      {
        MSG_MED("Invoke XTRA-T for MSB fallback", 0, 0, 0);
      }
#endif /* FEATURE_CGPS_XTRA_T */
    }
    else
    {
      // Successfully sent time request to MGP, clear the flag
      tm_core_info.session_info.req_xtra_trig_by_agps = FALSE;
    }

#endif /* FEATURE_CGPS_XTRA */
    /* if MSB communication to network fails, request a position injection
       if PUNC is > 10km */
    if(tm_core_info.session_info.sess_info_valid_flags & TM_CORE_SESS_INFO_CACHED_INFO_STATE_VALID)
    {
      MSG_MED("Cached info valid", 0,0,0);
      pd_ext_status_info.client_id =  tm_core_info.session_info.pd_param.client_id;
      pd_ext_status_info.ext_status_type = PDSM_EXT_STATUS_POS_INJECT_REQUEST;
      pd_ext_status_info.ext_status_info.ext_pos_inj_req_type.flags = 0;
      if ((tm_core_info.gps_state_info.flags & SM_GPS_STATE_PUNC_VALID ) &&
          (tm_core_info.gps_state_info.pos_unc > TM_MAX_PUNC_VALUE_FOR_POS_INJ_REQ))
      {
        MSG_MED("Cached info state is valid. pos inj required",0,0,0);
        req_pos = TRUE;
        /* copy the pos/time from the cached state info */
        pd_ext_status_info.ext_status_info.ext_pos_inj_req_type.flags |= PDSM_PD_EXT_POS_INJ_POS_UNC_VALID;

        pd_ext_status_info.ext_status_info.ext_pos_inj_req_type.pos_unc = tm_core_info.gps_state_info.pos_unc;
        if(tm_core_info.gps_state_info.flags & SM_GPS_STATE_POS_VALID)
        {

          pd_ext_status_info.ext_status_info.ext_pos_inj_req_type.flags |= PDSM_PD_EXT_POS_INJ_LAT_LONG_VALID;
          pd_ext_status_info.ext_status_info.ext_pos_inj_req_type.lat = tm_core_info.gps_state_info.lat;
          pd_ext_status_info.ext_status_info.ext_pos_inj_req_type.lon = tm_core_info.gps_state_info.lon;
        }

         if (tm_core_info.gps_state_info.flags & SM_GPS_STATE_TIME_VALID )
         {
           pd_ext_status_info.ext_status_info.ext_pos_inj_req_type.flags |= PDSM_PD_EXT_POS_INJ_TIME_STAMP_VALID;
           pd_ext_status_info.ext_status_info.ext_pos_inj_req_type.timestamp_gps_week =
               tm_core_info.gps_state_info.timestamp_gps_week;
           pd_ext_status_info.ext_status_info.ext_pos_inj_req_type.timestamp_tow_ms =
             tm_core_info.gps_state_info.timestamp_tow_ms;
         }
      }
      else if(!(tm_core_info.gps_state_info.flags & SM_GPS_STATE_PUNC_VALID))
      {
        MSG_MED("Cached info state is valid., punc invalid pos inj required",0,0,0);

        /* position is not valid. set all the pos/time as invalid in the request */
        req_pos = TRUE;
        MSG_MED("Punc info unvalid. pos inj required",0,0,0);
        if(tm_core_info.gps_state_info.flags & SM_GPS_STATE_POS_VALID)
        {
          pd_ext_status_info.ext_status_info.ext_pos_inj_req_type.flags = PDSM_PD_EXT_POS_INJ_LAT_LONG_VALID;
          pd_ext_status_info.ext_status_info.ext_pos_inj_req_type.lat = tm_core_info.gps_state_info.lat;
          pd_ext_status_info.ext_status_info.ext_pos_inj_req_type.lon = tm_core_info.gps_state_info.lon;
        }

        if (tm_core_info.gps_state_info.flags & SM_GPS_STATE_TIME_VALID )
        {
          pd_ext_status_info.ext_status_info.ext_pos_inj_req_type.flags |= PDSM_PD_EXT_POS_INJ_TIME_STAMP_VALID;
          pd_ext_status_info.ext_status_info.ext_pos_inj_req_type.timestamp_gps_week =
              tm_core_info.gps_state_info.timestamp_gps_week;
          pd_ext_status_info.ext_status_info.ext_pos_inj_req_type.timestamp_tow_ms =
            tm_core_info.gps_state_info.timestamp_tow_ms;
        }
      }
    }
    if(req_pos)
    {
      MSG_HIGH("REQUEST POSITION INJECTION",0,0,0);
      tm_pdapi_ext_status_event_callback(PDSM_EXT_STATUS_POS_INJECT_REQUEST,
        &pd_ext_status_info);
      tm_core_info.session_info.pos_inj_requested =  TRUE;
    }

  }



  if ((pd_event & PDSM_PD_EVENT_POSITION) ||
      (pd_event & PDSM_PD_EVENT_VELOCITY) ||
      (pd_event & PDSM_PD_EVENT_HEIGHT))
  {
    /* For MSA Sessions, see if we have UTCOffset available and can compute UTCTime */
    if(tm_core_info.session_info.op_mode==TM_SESS_OPERATION_MODE_MSA)
    {
      if (tm_core_get_trusted_utctime()==TRUE)
      {
        pd_info_ptr->pd_info.pd_data.opt_field_mask |= PDSM_PD_UTCOFFSET_VALID;
        tm_core_get_nv_utc_offset((uint8*)&pd_info_ptr->pd_info.pd_data.gpsUtcOffset);
      }
      else
      {
        pd_info_ptr->pd_info.pd_data.opt_field_mask &= ~PDSM_PD_UTCOFFSET_VALID;
      }

      /* Compute UTC Time, regardless of if the offset is valid or not */
      pd_info_ptr->pd_info.pd_data.ext_opt_field_mask |= PDSM_PD_UTC_TIME_VALID;
      z_utc_time = tm_util_construct_utc_time(pd_info_ptr->pd_info.pd_data.ext_pos_data.timestamp_gps_week,
                                              pd_info_ptr->pd_info.pd_data.ext_pos_data.timestamp_tow_ms,
                                              pd_info_ptr->pd_info.pd_data.ext_pos_data.timestamp_tow_ms -
                                                tm_core_info.config_info.nv_gps_utc_offset * SEC_MSECS);

      pd_info_ptr->pd_info.pd_data.ext_pos_data.utc_time.hour = (uint8)z_utc_time.q_hours;
      pd_info_ptr->pd_info.pd_data.ext_pos_data.utc_time.minute = (uint8)z_utc_time.q_mins;
      pd_info_ptr->pd_info.pd_data.ext_pos_data.utc_time.sec = (uint16)(z_utc_time.q_secs * 100);

      pd_info_ptr->pd_info.pd_data.ext_pos_data.utc_time.date =
        (uint32) ((z_utc_time.q_utc_days * 1000000) + (z_utc_time.q_month * 10000) + z_utc_time.q_year);
    }

    /* Copy ext data over */
    memscpy((void *)&(pd_info_ptr->pd_info.pd_ext_data),
           sizeof(pd_info_ptr->pd_info.pd_ext_data),
           (void *)&(tm_core_info.pd_ext_data),
           sizeof(tm_core_info.pd_ext_data));

    if(tm_core_info.ext_opt_field_mask & PDSM_PD_HEADING_VALID)
      pd_info_ptr->pd_info.pd_data.ext_opt_field_mask |= PDSM_PD_HEADING_VALID;

    if (p_PosSrcFlags == NULL)
    {
      tm_pdapi_pos_log_generation(pd_event,
                                  pd_info_ptr,
                                  0,
                                  0,
                                  TM_PDAPI_SESS_TYPE_NONE);
    }
    else
    {
      tm_pdapi_pos_log_generation(pd_event,
                                  pd_info_ptr,
                                  p_PosSrcFlags->b_IsSft,
                                  p_PosSrcFlags->b_IsKf,
                                  TM_PDAPI_SESS_TYPE_NONE);
    }

#ifdef FEATURE_TM_SECURITY

#ifdef FEATURE_GPSONE_SECURITY_OPEN_STANDALONE
    if(tm_core_info.session_info.pd_param.pd_option.operation_mode != PDSM_SESSION_OPERATION_STANDALONE_ONLY)
    {
#endif /* FEATURE_GPSONE_SECURITY_OPEN_STANDALONE */
      /* Reset pd_ext_data */
      memset((void *)&(tm_core_info.pd_ext_data), 0, sizeof(pdsm_pd_data_ext_s_type));
      tm_core_info.ext_opt_field_mask = 0;

      /* Encrypt PD Data if security layer is enabled */
      tm_sec_encrypt_pd_info(pd_info_ptr);
#ifdef FEATURE_GPSONE_SECURITY_OPEN_STANDALONE
    }
    else
    {
      pd_info_ptr->pd_info.encrypt_data.encrypt_alg = PDSM_PD_CRYPT_ALG_NONE;
    }
#endif /* FEATURE_GPSONE_SECURITY_OPEN_STANDALONE */

#else

    pd_info_ptr->pd_info.encrypt_data.encrypt_alg = PDSM_PD_CRYPT_ALG_NONE;

#endif /* FEATURE_TM_SECURITY */

    z_cached_pos_info.cached_pos_valid = TRUE;
    memscpy(&z_cached_pos_info.pd_info, sizeof(z_cached_pos_info.pd_info), pd_info_ptr, sizeof(*pd_info_ptr));
    z_cached_pos_info.pd_event_type = pd_event;
    if (p_PosSrcFlags == NULL)
    {
      memset(&z_cached_pos_info.z_pos_flags, 0, sizeof(z_cached_pos_info.z_pos_flags));
    }
    else
    {
      z_cached_pos_info.z_pos_flags = *p_PosSrcFlags;
    }
    tm_pdapi_pos_event_callback(pd_event, pd_info_ptr, TM_PDAPI_SESS_TYPE_NONE);

    /* Set the updated UTC offset if different */
    gps_utc_offset = z_cached_pos_info.pd_info.pd_info.pd_data.gpsUtcOffset;
    if ((gps_utc_offset != 0) && (gps_utc_offset != tm_core_info.config_info.nv_gps_utc_offset))
    {
      MSG_MED("TM: Set new utc time offset value: %d", gps_utc_offset, 0, 0);
      tm_core_info.config_info.nv_gps_utc_offset = gps_utc_offset;
      b_nv_gps_utc_offset_changed = TRUE;
    }
    }
    else
    {
    p_pd_info_type.e_pd_event = pd_event;
    p_pd_info_type.pd_event_data.pd_info_type = *pd_info_ptr;
      tm_pdapi_pd_event_callback(&p_pd_info_type, TM_PDAPI_SESS_TYPE_NONE);

#ifdef FEATURE_CGPS_XTRA_T
    /* Notify TLE's XTM task that AGPS session is in progress. */
    if (pd_event == PDSM_PD_EVENT_COMM_CONNECTED)
    {
      (void) tle_SendAGPSEvents(C_TLE_EVENT_AGPS_PDCOMM_CONNECTED);
    }
#endif

  }
  return TRUE;
}

/*===========================================================================

FUNCTION tm_core_translate_lm_error

DESCRIPTION
  This function is to translate LM error to tm_core - prtl error cause

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
tm_sess_stop_reason_e_type tm_core_translate_lm_error
(
  tm_session_error_cause_e_type lm_err
)
{
  switch(lm_err)
  {
    case TM_ERROR_CAUSE_PPM_TIMEOUT:
    return TM_STOP_REASON_PPM_TIMEOUT;

    case TM_ERROR_CAUSE_PRM_TIMEOUT:
    return TM_STOP_REASON_PRM_TIMEOUT;

    case TM_ERROR_CAUSE_FIX_TIMEOUT:
    return TM_STOP_REASON_FIX_TIMEOUT;

    default:
    return TM_STOP_REASON_GENERAL_ERROR;
  }
}

/*===========================================================================

FUNCTION tm_core_lm_sess_req_handler

DESCRIPTION
  This function is to handle sess request from LM

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_lm_sess_req_handler(void *lm_sess_req_data)
{
  tm_session_request_s_type *sess_req_ptr = (tm_session_request_s_type *)lm_sess_req_data;
  int prtlTableIndex = tm_core_get_prtl_idx_by_type(tm_core_info.session_info.from_protocol);
  tm_sess_stop_reason_e_type stop_reason;

  if(sess_req_ptr)
  {
    switch(sess_req_ptr->e_request_type)
    {
      case TM_REQUEST_ERROR_NOTIFY:
        if(sess_req_ptr->q_handle == tm_core_info.session_info.active_sess_handle)
        {
          /* Send abort request to Protocol sub-module, and let protocl sub-module decide what to do */

          if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
          {
            if(tm_core_is_internal_session_on() == TRUE)
            {
              /* An internal session is going ON . Could be started from GM . Send the fix fail
               indication to it */
              MSG_MED("Sending position fix fail update to GM . Reason %d",
                    sess_req_ptr->z_request.e_errorCause, 0, 0);
              gm_position_fix_fail_update(GEOFENCE_POS_FIX_SRC_GNSS, GEOFENCE_POS_FIX_FAIL_REASON_TIMEOUT);

            }
            /* Set reason to general error for now, if return value */
            if(tm_core_info.prtl_func_cb_table[prtlTableIndex].stop_sess_req_fp)
            {
              MSG_HIGH("Got error notification from LM, foward to protocol modules", 0, 0, 0);
              stop_reason = tm_core_translate_lm_error(sess_req_ptr->z_request.e_errorCause);
              tm_core_abort_recover(TRUE, stop_reason); /*lint !e506 !e730 */
            }
            else
            {
              MSG_ERROR("No protocol callback installed 4", 0, 0, 0);                      
              tm_core_abort_recover(FALSE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
            }
          }
          else
          {
            if (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_E911)
            {
              /* E911 is a dummy protocol, no protocol call backs are needed. Just clean up the session 
                 if no actual LPP session was rcvd from the network after E911 */
              MSG_HIGH("E911 ended without network request. Cleanup session", 0, 0, 0);
              tm_core_abort_recover(TRUE, TM_STOP_REASON_COMPLETED); /*lint !e506 !e730 */
            }

            else
            {
              MSG_ERROR("Invalid prtl index", 0, 0, 0);
            }
            }
        } /* sess_req_ptr->q_handle == tm_core_info.session_info.active_sess_handle */
        else if(tm_core_info.session_info.active_sess_handle != TM_SESS_HANDLE_NONE)
        {
          MSG_ERROR("LM - TM state not in sync, abort everything", 0, 0, 0);
          tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
        }
      break;

    case TM_REQUEST_SA_DATA:
        if(sess_req_ptr->q_handle == tm_core_info.session_info.active_sess_handle)
        {
          /* Send abort request to Protocol sub-module, and let protocl sub-module decide what to do */

          if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
          {
            /* Set reason to general error for now, if return value */
            if(tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_req_data_fp)
            {
              MSG_MED("Got SA request from LM", 0, 0, 0);
              (void) tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_req_data_fp(
                                                                               tm_core_info.session_info.active_sess_handle,
                                                                               PRTL_AIDING_DATA_REQ_SA
                                                                              );
            }
            else
            {
              MSG_ERROR("No protocol callback installed 5", 0, 0, 0);                      
              tm_core_abort_recover(FALSE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
            }
          }
          else
          {
            MSG_ERROR("Invalid prtl index", 0, 0, 0);
          }
        } /* sess_req_ptr->q_handle == tm_core_info.session_info.active_sess_handle */
        else if(tm_core_info.session_info.active_sess_handle != TM_SESS_HANDLE_NONE)
        {
          MSG_ERROR("LM - TM state not in sync, abort everything", 0, 0, 0);
          tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
        }

        break;

      default:
        MSG_ERROR("Unknown LM Sess Requst", 0, 0, 0);
      break;
    }
  }
  else
  {
    MSG_ERROR("Null sess_req_ptr", 0, 0, 0);
    tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
  }
}

/*===========================================================================

FUNCTION tm_core_xate_sv_state

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
pdsm_sv_state tm_core_xate_sv_state(gnss_SvStateEnumType in_sv_state)
{
  switch(in_sv_state)
  {
    case C_GNSS_SV_STATE_IDLE:
      return PDSM_SV_STATE_IDLE;

    case C_GNSS_SV_STATE_SEARCH:
      return PDSM_SV_STATE_SEARCH;

    case C_GNSS_SV_STATE_SEARCH_VERIFY:
      return PDSM_SV_STATE_SEARCH_VERIFY;

    case C_GNSS_SV_STATE_GLO_10MSEC_BIT_EDGE:
    case C_GNSS_SV_STATE_BIT_EDGE:
      return PDSM_SV_STATE_BIT_EDGE;

    case C_GNSS_SV_STATE_VERIFY_TRACK:
    case C_GNSS_SV_STATE_TRACK:
    case C_GNSS_SV_STATE_DPO_TRACK:
      return PDSM_SV_STATE_TRACK;

    default:
      return PDSM_SV_STATE_IDLE;
  }
}

/*===========================================================================

FUNCTION tm_core_xlate_gnss_prm_to_pd_meas

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static uint8 tm_core_xlate_gnss_prm_to_pd_meas
(
  bcn_IdxEnumType              e_GnssIdx,
  pdsm_pd_meas_s_type           *report,
  const gnss_MeasStructType     *p_GnssMeas,
  uint8                         u_NumSvs,
  uint8                         max_svs
)
{
  uint8  i;
  uint8  valid_sv_count = 0;
  uint32 q_prm;
  uint8  u_x;
  FLT    f_prm_sigma = 0.0, f_prm_limit = 0.0, f_y = 0.0;

  if ((NULL == report) || (NULL == p_GnssMeas) || (u_NumSvs > N_ACTIVE_CHAN))
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,
      "Invalid Arg: NULL Ptr Checks %d %d, NumSvs=%u", 
      NULL==report, NULL==p_GnssMeas, u_NumSvs);
	return 0;
  }

  for(i = 0; i < u_NumSvs && valid_sv_count < max_svs; i++)
  {
    /* Report only valid measurements, validity check already performed in lm_mgp.c */
    report->flags       = 1;
    report->sv_id         = p_GnssMeas[i].w_Sv;
    report->c_no          = p_GnssMeas[i].w_Cno;
    report->elevation = 0;
    report->azimuth   = 0;
    report->elevation_rad = 0;
    report->azimuth_rad = 0;

    if (p_GnssMeas[i].u_SvDirValid && (p_GnssMeas[i].f_Elevation > 0))
    {
      report->elevation     = p_GnssMeas[i].f_Elevation * RAD2DEG; /*lint !e524 */
      report->azimuth       = p_GnssMeas[i].f_Azimuth   * RAD2DEG; /*lint !e524 */
      report->elevation_rad = p_GnssMeas[i].f_Elevation;
      report->azimuth_rad   = p_GnssMeas[i].f_Azimuth;
    }

    report->latency_ms = (int16)p_GnssMeas[i].l_LatencyMs;
    report->sv_state   = tm_core_xate_sv_state((gnss_SvStateEnumType)p_GnssMeas[i].q_SvState);

    report->chan_rpt_type.ps_doppler = (int32)(p_GnssMeas[i].z_FiltMeas.f_SvSpeed * C_MsToL1Hz * (-5.0));

    if (BCN_IDX_GPS == e_GnssIdx || BCN_IDX_QZSS == e_GnssIdx)
    {
      report->sv_freq_num = 0xff; /* No frequency number for GPS/QZSS */

      q_prm = (uint32) ((1.0 - p_GnssMeas[i].z_FiltMeas.z_SvTime.f_SubMs) * 1047552.0); // 1023*1024
      report->chan_rpt_type.sv_code_ph_wh = (uint16)(q_prm >> 10);

      if (report->chan_rpt_type.sv_code_ph_wh == (CA_CHIPS_MSEC*CA_PERIOD_MSEC) ) // GPS # of C/A chips per msec
      {
        report->chan_rpt_type.sv_code_ph_wh = 0;
      }

      report->chan_rpt_type.sv_code_ph_fr = (int16) (q_prm & 0x3ff);
    }
    else if (BCN_IDX_GLO == e_GnssIdx)
    {
      report->sv_freq_num = p_GnssMeas[i].b_FreqNum; /* GLONASS frequency numbers [-7:6] */

      q_prm = (uint32) ((1.0 - p_GnssMeas[i].z_FiltMeas.z_SvTime.f_SubMs) * 261632.0); // 511*512
      report->chan_rpt_type.sv_code_ph_wh = (uint16)(q_prm >> 9);

      if (report->chan_rpt_type.sv_code_ph_wh == (GLO_CA_CHIPS_MSEC*GLO_CA_PERIOD_MSEC) ) // GLO # of C/A chips per msec
      {
        report->chan_rpt_type.sv_code_ph_wh = 0;
      }

      report->chan_rpt_type.sv_code_ph_fr = (int16) (q_prm & 0x1ff);
    }
    else if (BCN_IDX_BDS == e_GnssIdx)
    {
      report->sv_freq_num = 0xff; /* No frequency number for BDS */

      q_prm = (uint32) ((1.0 - p_GnssMeas[i].z_FiltMeas.z_SvTime.f_SubMs) * 4190208.0); // 2046*2048
      report->chan_rpt_type.sv_code_ph_wh = (uint16)(q_prm >> 11);

      if (report->chan_rpt_type.sv_code_ph_wh == (BDS_CA_CHIPS_MSEC*BDS_CA_PERIOD_MSEC) ) // BDS # of C/A chips per msec
      {
        report->chan_rpt_type.sv_code_ph_wh = 0;
      }

      report->chan_rpt_type.sv_code_ph_fr = (int16) (q_prm & 0x7ff);
    }
    else if (BCN_IDX_GAL == e_GnssIdx)
    {
      report->sv_freq_num = 0xff; /* No frequency number for GAL */ 
      q_prm = (uint32) ((1.0 - p_GnssMeas[i].z_FiltMeas.z_SvTime.f_SubMs) * 1047552.0); // 1023*1024 
      q_prm += (3 - (p_GnssMeas[i].z_FiltMeas.z_SvTime.q_Ms % 4))*1023*1024;
      report->chan_rpt_type.sv_code_ph_wh = (uint16)(q_prm >> 10); 
 
      if (report->chan_rpt_type.sv_code_ph_wh == (GAL_CA_CHIPS_MSEC*GAL_CA_PERIOD_MSEC) ) // GAL # of C/A chips per msec 
      { 
         report->chan_rpt_type.sv_code_ph_wh = 0; 
      } 

      report->chan_rpt_type.sv_code_ph_fr = (int16) (q_prm & 0x3ff); 
    }

    if ((p_GnssMeas[i].u_Observe == 1) && (p_GnssMeas[i].u_GoodObs == 1))
    {
      /* Early exit passed after first sniff */
      report->chan_rpt_type.mul_path_ind = 0;
    }
    else if ((p_GnssMeas[i].u_PreInt == 160) && (BCN_IDX_GLO != e_GnssIdx))
    {
      /* SA Data was used */
      report->chan_rpt_type.mul_path_ind = 7;
    }
    else
    {
      uint32 q_MeasSrchTime;

      q_MeasSrchTime = p_GnssMeas[i].u_PreInt * p_GnssMeas[i].w_PostInt;

      if( q_MeasSrchTime < 2560 )
      {
        report->chan_rpt_type.mul_path_ind = 4;
      }
      else if( q_MeasSrchTime < 5120 )
      {
        report->chan_rpt_type.mul_path_ind = 5;
      }
      else
      {
        report->chan_rpt_type.mul_path_ind = 6;
      }
    }

    /* Calculate range uncertainty. */
    f_prm_sigma = p_GnssMeas[i].z_FiltMeas.f_SvTimeUncMs * (FLT)LIGHT_MSEC;

    /* Following legacy code, report XCorrs with Saturated RMSE */
    if (p_GnssMeas[i].q_MeasStatus & SM_MEAS_STATUS_XCORR)
    {
      report->chan_rpt_type.ps_range_rms_er = 63;
    }
    /* First do settings at low and high end of the allowed pseudorange unc. range */
    else if (f_prm_sigma < 0.5)
    {
      report->chan_rpt_type.ps_range_rms_er = 0;
    }
    else if ((f_prm_sigma < 112.0) && (f_prm_sigma >= 104.0))
    {
      report->chan_rpt_type.ps_range_rms_er = 62;
    }
    else if (f_prm_sigma >= 112.0)
    {
      /* Any value greater than 112m should be mapped to the 62 because this
         will give the PDM the possible chance of using this measurement with a
         lower weight */
      report->chan_rpt_type.ps_range_rms_er = 62;
    }
    else
    {
      for (report->chan_rpt_type.ps_range_rms_er = 1;
           report->chan_rpt_type.ps_range_rms_er < 62;
           report->chan_rpt_type.ps_range_rms_er++)
      {
        /* Get the exponent part */
        u_x = report->chan_rpt_type.ps_range_rms_er >> 3;
        /* Get the mantissa part. */
        f_y = (FLT) (report->chan_rpt_type.ps_range_rms_er & 7);
        f_prm_limit = (0.5F + f_y * 0.0625F) * (FLT)(1<<u_x);

        if(f_prm_sigma < f_prm_limit)
        {
          break;
        }
      }
    }

    valid_sv_count++;
    report++;
  }   /* end for loop iterating over SV prms */
  return valid_sv_count;
}

/*===========================================================================

FUNCTION tm_core_xlate_prms_to_ext_status

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static void tm_core_xlate_prms_to_ext_status
(
  pdsm_pd_ext_meas_s_type       *dest_buf_ptr,
  const gnss_MeasBlkStructType  *source_buf_ptr_gps,
  const gnss_MeasBlkStructType  *source_buf_ptr_glo,
  const gnss_MeasBlkStructType  *source_buf_ptr_bds,
  const gnss_MeasBlkStructType  *source_buf_ptr_gal,
  const gnss_MeasBlkStructType  *source_buf_ptr_qzss
)
{
  uint8 valid_sv_count = 0;

  if((dest_buf_ptr == NULL)       ||
     (source_buf_ptr_gps == NULL) ||
     (source_buf_ptr_glo == NULL) ||
     (source_buf_ptr_bds == NULL) ||
     (source_buf_ptr_qzss == NULL) ||
     (source_buf_ptr_gal == NULL))
  {
    return;
  }


  dest_buf_ptr->flags         = 0;
  dest_buf_ptr->eph_svmask    = tm_core_info.session_info.eph_svmask;
  dest_buf_ptr->alm_svmask    = tm_core_info.session_info.alm_svmask;
  dest_buf_ptr->health_svmask = tm_core_info.session_info.health_svmask;

  dest_buf_ptr->iono_valid    = 1;
  if (tm_core_info.session_info.iono) {
    dest_buf_ptr->iono_valid = 0;
  }

  dest_buf_ptr->time_unc_ms   = (uint32)source_buf_ptr_gps->z_MeasClock.z_Time.z_GpsTime.f_ClkTimeUncMs;
  if (dest_buf_ptr->time_unc_ms < 20)
  {
    dest_buf_ptr->time_unc_ms = 20;
  }
  dest_buf_ptr->glo_eph_svmask = tm_core_info.session_info.glo_eph_svmask;
  dest_buf_ptr->glo_alm_svmask = tm_core_info.session_info.glo_alm_svmask;

  dest_buf_ptr->bds_eph_svmask = tm_core_info.session_info.bds_eph_svmask;
  dest_buf_ptr->bds_alm_svmask = tm_core_info.session_info.bds_alm_svmask;
 
  dest_buf_ptr->qzss_eph_svmask = tm_core_info.session_info.qzss_eph_svmask;
  dest_buf_ptr->qzss_alm_svmask = tm_core_info.session_info.qzss_alm_svmask;
  dest_buf_ptr->qzss_iono = tm_core_info.session_info.qzss_iono;
  dest_buf_ptr->qzss_health_svmask = tm_core_info.session_info.qzss_health_svmask;
  
  dest_buf_ptr->gal_eph_svmask = tm_core_info.session_info.gal_eph_svmask;
  dest_buf_ptr->gal_alm_svmask = tm_core_info.session_info.gal_alm_svmask;

  /**********************************************************************************
   *  The order of the two following function calls is important, do not swap without
   *  changing parameters
   *********************************************************************************/

  /*
   * Translate gps prms to pdsm_pd_meas_s_type, and store in ext_meas_report_type array
   * starting from index 0.
   */
  valid_sv_count += tm_core_xlate_gnss_prm_to_pd_meas
                    (
                    BCN_IDX_GPS,
                    &dest_buf_ptr->ext_meas_report_type[0],
                    source_buf_ptr_gps->z_Gnss,
                    source_buf_ptr_gps->u_NumSvs,
                    PDSM_EXT_MEAS_NUM_SVS
                    );  /* Number of entries available for gps is PDSM_EXT_MEAS_NUM_SVS */

  /*
   * Translate glo prms to pdsm_pd_meas_s_type, and store in ext_meas_report_type array
   * starting from the first free index after gps prms were used up.
   */
  if (valid_sv_count < PDSM_EXT_MEAS_NUM_SVS)
  {
    valid_sv_count += tm_core_xlate_gnss_prm_to_pd_meas
                      (
                        BCN_IDX_GLO,
                        &dest_buf_ptr->ext_meas_report_type[valid_sv_count],
                        source_buf_ptr_glo->z_Gnss,
                        source_buf_ptr_glo->u_NumSvs,
                        PDSM_EXT_MEAS_NUM_SVS - valid_sv_count
                      ); /* Number of entries available for glo is PDSM_EXT_MEAS_NUM_SVS -
                            Number of entries used by GPS */
  }
  else
  {
    MSG_ERROR("No space for GLO prms: GPS SVs > PDSM_EXT_MEAS_NUM_SVS", 0, 0, 0);
  }

  /*
   * Translate bds prms to pdsm_pd_meas_s_type, and store in ext_meas_report_type array
   * starting from the first free index after glo prms were used up.
   */
  if (valid_sv_count < PDSM_EXT_MEAS_NUM_SVS)
  {
    valid_sv_count += tm_core_xlate_gnss_prm_to_pd_meas
                      (
                        BCN_IDX_BDS,
                        &dest_buf_ptr->ext_meas_report_type[valid_sv_count],
                        source_buf_ptr_bds->z_Gnss,
                        source_buf_ptr_bds->u_NumSvs,
                        PDSM_EXT_MEAS_NUM_SVS - valid_sv_count
                      ); /* Number of entries available for bds is PDSM_EXT_MEAS_NUM_SVS -
                            Number of entries used by GPS & GLO */
  }
  else
  {
    MSG_ERROR("No space for BDS prms: GPS/GLO SVs > PDSM_EXT_MEAS_NUM_SVS", 0, 0, 0);
  }

  /*
   * Translate qzss prms to pdsm_pd_meas_s_type, and store in ext_meas_report_type array
   * starting from the first free index after bds prms were used up.
   */
  if (valid_sv_count < PDSM_EXT_MEAS_NUM_SVS)
  {
    valid_sv_count += tm_core_xlate_gnss_prm_to_pd_meas
                      (
                        BCN_IDX_QZSS,
                        &dest_buf_ptr->ext_meas_report_type[valid_sv_count],
                        source_buf_ptr_qzss->z_Gnss,
                        source_buf_ptr_qzss->u_NumSvs,
                        PDSM_EXT_MEAS_NUM_SVS - valid_sv_count
                      ); /* Number of entries available for qzss is PDSM_EXT_MEAS_NUM_SVS -
                            Number of entries used by GPS & GLO & BDS */
  }
  else
  {
    MSG_ERROR("No space for QZSS prms: GPS/GLO/BDS SVs > PDSM_EXT_MEAS_NUM_SVS", 0, 0, 0);
  }

   /*
   * Translate gal prms to pdsm_pd_meas_s_type, and store in ext_meas_report_type array
   * starting from the first free index after glo prms were used up.
   */
  if (valid_sv_count < PDSM_EXT_MEAS_NUM_SVS)
  {
    valid_sv_count += tm_core_xlate_gnss_prm_to_pd_meas
                      (
                        BCN_IDX_GAL,
                        &dest_buf_ptr->ext_meas_report_type[valid_sv_count],
                        source_buf_ptr_gal->z_Gnss,
                        source_buf_ptr_gal->u_NumSvs,
                        PDSM_EXT_MEAS_NUM_SVS - valid_sv_count
                      ); /* Number of entries available for gal is PDSM_EXT_MEAS_NUM_SVS -
                            Number of entries used by GPS, GLO, BDS & QZSS*/
  }
  else
  {
    MSG_ERROR("No space for GAL prms: GPS/GLO/BDS/QZSS SVs > PDSM_EXT_MEAS_NUM_SVS", 0, 0, 0);
  }

  dest_buf_ptr->num_svs = valid_sv_count;

} /* tm_core_xlate_prm_to_ext_status() */


/*===========================================================================

FUNCTION tm_core_xlate_gnss_prms_to_pd_gnss_meas

DESCRIPTION: Helper function to translate internal SV measurement in to raw data output QMI PD

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static boolean tm_core_xlate_gnss_prms_to_pd_gnss_meas(const gnss_MeasStructType     *p_MeasSrc, pdsm_LocSVMeasurementStructType *p_pdsmSVMeasDst,
                                                       uint32 e_MeasBlkSrc)
{


     DBL d_BoundMsecs, d_SvMeas;
     const gnss_TimeSpeedStructType * p_SVTimeSpeed = NULL;
     uint32 eph_svmask=0, alm_svmask=0, health_svmask=0;
     uint16 u_svIdMinVal=0;
     uint32 q_MeasStat, q_MeasStat2;

     if((p_MeasSrc == NULL)          ||
        (p_pdsmSVMeasDst == NULL))
     {
          MSG_HIGH("tm_core_xlate_gnss_prms_to_pd_gnss_meas received NULL pointer",0,0,0);
            return(FALSE);
     }

     p_SVTimeSpeed = &p_MeasSrc->z_UnfiltMeas;

     p_pdsmSVMeasDst->w_gnssSvId = p_MeasSrc->w_Sv;

     if(e_MeasBlkSrc == GNSS_MEAS_BLK_SRC_GLO)
     {
          /* Map signed glonass frequency to postive mapping (between 1 to 14) */
          p_pdsmSVMeasDst->u_gloFrequency = p_MeasSrc->b_FreqNum + GLO_FREQ_NUMBER_OFFSET + 1;
     }
     else
     {
          p_pdsmSVMeasDst->u_gloFrequency = 0;
     }

     switch (tm_core_xate_sv_state(p_MeasSrc->q_SvState))
     {
          case PDSM_SV_STATE_TRACK:
               p_pdsmSVMeasDst->e_svStatus = PD_LOC_SV_SRCH_STATUS_TRACK;
               break;
          case PDSM_SV_STATE_BIT_EDGE:
          case PDSM_SV_STATE_SEARCH:
          case PDSM_SV_STATE_SEARCH_VERIFY:
               p_pdsmSVMeasDst->e_svStatus = PD_LOC_SV_SRCH_STATUS_SEARCH;
               break;
          case PDSM_SV_STATE_IDLE:
               p_pdsmSVMeasDst->e_svStatus = PD_LOC_SV_SRCH_STATUS_IDLE;
               break;
          default:
               p_pdsmSVMeasDst->e_svStatus = PD_LOC_SV_SRCH_STATUS_SEARCH;
               break;
     }

     /* DON'T USE flags needs special handling. As per QMI definition all don't use flags are moved up in status2 word.
         So, mask them from q_MeasStatus of measurement block and mark them up in q_MeasStatus2 bit position */
     q_MeasStat = p_MeasSrc->q_MeasStatus;
     q_MeasStat2 = p_MeasSrc->q_MeasStatus2;

     /* Top 10 MS bits of q_MeasStat2 are ear-marked as Reserved bits for DON'T use flags (per QMI definition). */
     q_MeasStat2 &= ~(PD_LOC_MEAS_STATUS2_DONT_USE_BITS);
     
     /* Evalute DONT_USE bit in q_MeasStat, if set, lit corresponding reserved DONT_USE bit in q_MeasStat2
        And clear the original DONT_USE bit from q_MeasStat
        Code is intentionally constructued to evaluate individual bits, to support any possibility of discontinuity
        in bit position assignment */
     if(q_MeasStat & PD_LOC_MEAS_STATUS_DONT_USE_X)
     {
           q_MeasStat &= ~PD_LOC_MEAS_STATUS_DONT_USE_X;
           q_MeasStat2 |= PD_LOC_MEAS_STATUS2_DONT_USE_X;
     }
     if(q_MeasStat & PD_LOC_MEAS_STATUS_DONT_USE_M)
     {
           q_MeasStat &= ~PD_LOC_MEAS_STATUS_DONT_USE_M;
           q_MeasStat2 |= PD_LOC_MEAS_STATUS2_DONT_USE_M;
     }
     if(q_MeasStat & PD_LOC_MEAS_STATUS_DONT_USE_D)
     {
           q_MeasStat &= ~PD_LOC_MEAS_STATUS_DONT_USE_D;
           q_MeasStat2 |= PD_LOC_MEAS_STATUS2_DONT_USE_D;
     }
     if(q_MeasStat & PD_LOC_MEAS_STATUS_DONT_USE_S)
     {
           q_MeasStat &= ~PD_LOC_MEAS_STATUS_DONT_USE_S;
           q_MeasStat2 |= PD_LOC_MEAS_STATUS2_DONT_USE_S;
     }
     if(q_MeasStat & PD_LOC_MEAS_STATUS_DONT_USE_P)
     {
           q_MeasStat &= ~PD_LOC_MEAS_STATUS_DONT_USE_P;
           q_MeasStat2 |= PD_LOC_MEAS_STATUS2_DONT_USE_P;
     }

     /* Combine Meas Status2 and Meas status in a single 64 bit status */

     /* First copy meas status2 */
     p_pdsmSVMeasDst->t_measurementStatus = (uint64)q_MeasStat2;
     /* Shift the bits up by 32 in meas startus2 position */
     p_pdsmSVMeasDst->t_measurementStatus <<= 32;

     /* Insert meas status mask position */
     p_pdsmSVMeasDst->t_measurementStatus |= (uint64)q_MeasStat;

     /* Set validity bits corresponding to status2 */
     p_pdsmSVMeasDst->t_measurementStatusValid     = (uint64)(PD_LOC_MEAS_STATUS2_DONT_USE_X | PD_LOC_MEAS_STATUS2_DONT_USE_M |
                                                              PD_LOC_MEAS_STATUS2_DONT_USE_D | PD_LOC_MEAS_STATUS2_DONT_USE_S |
                                                              PD_LOC_MEAS_STATUS2_DONT_USE_P);
     /* Shift the bits up by 32 in status2 position */
     p_pdsmSVMeasDst->t_measurementStatusValid <<= 32;

     /* Insert validity bits of status mask position */
     p_pdsmSVMeasDst->t_measurementStatusValid |= (uint64)(PD_LOC_MEAS_STATUS_SM_VALID | PD_LOC_MEAS_STATUS_SB_VALID |
                                                           PD_LOC_MEAS_STATUS_MS_VALID | PD_LOC_MEAS_STATUS_BE_CONFIRM |
                                                           PD_LOC_MEAS_STATUS_VELOCITY_VALID | PD_LOC_MEAS_STATUS_VELOCITY_FINE |
                                                           PD_LOC_MEAS_STATUS_FROM_RNG_DIFF | PD_LOC_MEAS_STATUS_FROM_VE_DIFF);

#if 0
          MSG_MED("RAW_CLK_MEAS:Incoming:SVID:%d,MStat2:%u,MStat:%u",p_pdsmSVMeasDst->w_gnssSvId,p_MeasSrc->q_MeasStatus2,p_MeasSrc->q_MeasStatus);
          MSG_MED("RAW_CLK_MEAS:Outgoing:SVID:%d,MStat2:%u,MStat:%u",p_pdsmSVMeasDst->w_gnssSvId,q_MeasStat2,q_MeasStat);
          MSG_MED("RAW_CLK_MEAS:SVID:%d,MStatValid:%llu",p_pdsmSVMeasDst->w_gnssSvId,p_pdsmSVMeasDst->t_measurementStatusValid,0);
#endif


     switch(e_MeasBlkSrc)
     {
          case GNSS_MEAS_BLK_SRC_GPS:
               eph_svmask = tm_core_info.session_info.eph_svmask;
               alm_svmask = tm_core_info.session_info.alm_svmask;
               health_svmask = tm_core_info.session_info.health_svmask;
               u_svIdMinVal = PDSM_PD_SV_ID_GPS_MIN;
          break;
          case GNSS_MEAS_BLK_SRC_GLO:
               eph_svmask = tm_core_info.session_info.glo_eph_svmask;
               alm_svmask = tm_core_info.session_info.glo_alm_svmask;
               health_svmask = tm_core_info.session_info.glo_health_svmask;
               u_svIdMinVal = PDSM_PD_SV_ID_GLO_MIN;
          break;
          case GNSS_MEAS_BLK_SRC_BDS:
               eph_svmask = tm_core_info.session_info.bds_eph_svmask;
               alm_svmask = tm_core_info.session_info.bds_alm_svmask;
               health_svmask = tm_core_info.session_info.bds_health_svmask;
               u_svIdMinVal = PDSM_PD_SV_ID_BDS_MIN;
          break;

          case GNSS_MEAS_BLK_SRC_QZSS_SBAS:
               eph_svmask = tm_core_info.session_info.qzss_eph_svmask;
               alm_svmask = tm_core_info.session_info.qzss_alm_svmask;
               health_svmask = tm_core_info.session_info.qzss_health_svmask;
               u_svIdMinVal = PDSM_PD_SV_ID_QZSS_MIN;
          break;

//#if defined(FEATURE_GNSS_RAW_MEAS_OUTPUT_GAL_SUPPORT)
          case GNSS_MEAS_BLK_SRC_GAL:
               eph_svmask = tm_core_info.session_info.gal_eph_svmask;
               alm_svmask = tm_core_info.session_info.gal_alm_svmask;
               health_svmask = tm_core_info.session_info.gal_health_svmask;
               u_svIdMinVal = PDSM_PD_SV_ID_GAL_MIN;         
          break;
//#endif
          default:
               return(FALSE);
          break;
     }


      /* Populate ephemeris, Almanac status */
      p_pdsmSVMeasDst->e_svInfoMask = 0;
     if ((eph_svmask & (1 << (p_MeasSrc->w_Sv - u_svIdMinVal)) ) != 0)
     {
        p_pdsmSVMeasDst->e_svInfoMask |= PD_LOC_SVINFO_MASK_HAS_EPHEMERIS;     /* Present */
     }
     else
     {
        p_pdsmSVMeasDst->e_svInfoMask &= ~PD_LOC_SVINFO_MASK_HAS_EPHEMERIS;     /* Not present */
     }

     if ((alm_svmask & (1 << (p_MeasSrc->w_Sv - u_svIdMinVal)) ) != 0)
     {
        p_pdsmSVMeasDst->e_svInfoMask |= PD_LOC_SVINFO_MASK_HAS_ALMANAC;     /*present */
     }
     else
     {
        p_pdsmSVMeasDst->e_svInfoMask &= ~PD_LOC_SVINFO_MASK_HAS_ALMANAC;     /* Not present */
     }

     /* Populate health status & validity. In theory health could be unknown also, however tm_core structure doesn't have a validity indicator */
     p_pdsmSVMeasDst->b_healthStatus_valid = 1;
    if ((health_svmask & (1 << (p_MeasSrc->w_Sv - u_svIdMinVal)) ) != 0)
    {
       p_pdsmSVMeasDst->u_healthStatus = 1;
    }
    else
    {
       p_pdsmSVMeasDst->u_healthStatus = 0;
    }

     p_pdsmSVMeasDst->w_CNo = p_MeasSrc->w_Cno;
     p_pdsmSVMeasDst->w_gloRfLoss = p_MeasSrc->w_RfLoss;

     p_pdsmSVMeasDst->b_lossOfLockValid = FALSE;
     p_pdsmSVMeasDst->b_lossOfLock = 0; // Loss of lock Logic to be added here

     p_pdsmSVMeasDst->x_measLatency = p_MeasSrc->l_LatencyMs;

     /* Measurement */
     p_pdsmSVMeasDst->z_svTimeSpeed.q_svMs = p_MeasSrc->z_UnfiltMeas.z_SvTime.q_Ms;
     p_pdsmSVMeasDst->z_svTimeSpeed.f_svSubMs = p_MeasSrc->z_UnfiltMeas.z_SvTime.f_SubMs;
     p_pdsmSVMeasDst->z_svTimeSpeed.f_svTimeUncMs = p_MeasSrc->z_UnfiltMeas.f_SvTimeUncMs;
     p_pdsmSVMeasDst->z_svTimeSpeed.f_dopplerShift = p_MeasSrc->z_UnfiltMeas.f_SvSpeed;
     p_pdsmSVMeasDst->z_svTimeSpeed.f_dopplerShiftUnc = p_MeasSrc->z_UnfiltMeas.f_SvSpeedUnc;


     p_pdsmSVMeasDst->b_multipathEstValid = FALSE;		 
	 
     if( (p_MeasSrc->q_MeasStatus & MEAS_STATUS_VE_FINE) &&
	  (p_MeasSrc->q_MeasStatus & MEAS_STATUS_VE_VALID) ) 		 
     {
        p_pdsmSVMeasDst->b_fineSpeedValid = TRUE;
        p_pdsmSVMeasDst->f_fineSpeed = p_MeasSrc->f_FineSpeed;
        p_pdsmSVMeasDst->b_fineSpeedUncValid = TRUE;
        p_pdsmSVMeasDst->f_fineSpeedUnc = p_MeasSrc->f_FineSpeedUnc;
        p_pdsmSVMeasDst->b_dopplerAccelValid = FALSE;
			   
        p_pdsmSVMeasDst->b_carrierPhaseValid = TRUE; 
        p_pdsmSVMeasDst->d_carrierPhase = p_MeasSrc->d_CarrierPhase;	  
     }
     if(p_MeasSrc->q_MeasStatus & MEAS_STATUS_LP_VALID)
     {
        p_pdsmSVMeasDst->b_cycleSlipCountValid = TRUE;
        p_pdsmSVMeasDst->u_cycleSlipCount = p_MeasSrc->u_CSlipCount;
     }

     if(p_MeasSrc->u_SvDirValid)
     {
          p_pdsmSVMeasDst->b_svDirectionValid = TRUE;
          p_pdsmSVMeasDst->f_svAzimuth = p_MeasSrc->f_Azimuth;
          p_pdsmSVMeasDst->f_svElevation = p_MeasSrc->f_Elevation;
     }


#if FEATURE_GNSS_RAW_MEAS_OUTPUT_DEBUG
{
     char buf[1024];
     snprintf(buf, sizeof(buf),"RAW_CLK_MEAS:%d,%.2f,%x,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f\n",
                                p_pdsmSVMeasDst->w_gnssSvId,p_pdsmSVMeasDst->w_CNo,p_pdsmSVMeasDst->t_measurementStatus,
                                p_pdsmSVMeasDst->x_measLatency,p_pdsmSVMeasDst->z_svTimeSpeed.q_svMs,p_pdsmSVMeasDst->z_svTimeSpeed.f_svSubMs,
                                p_pdsmSVMeasDst->z_svTimeSpeed.f_svTimeUncMs, p_pdsmSVMeasDst->z_svTimeSpeed.f_dopplerShift,p_pdsmSVMeasDst->z_svTimeSpeed.f_dopplerShiftUnc);

     pe_DebugPrint(0,buf);
}
#endif
     return(TRUE);
} /*tm_core_xlate_gnss_prms_to_pd_gnss_meas */


/*===========================================================================

FUNCTION tm_core_xlate_prms_to_ext_raw_clock_meas

DESCRIPTION: Translates internal GNSS raw clock & measurement structure in a form suitable for transmission over QMI

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
//#define	FEATURE_GNSS_RAW_MEAS_OUTPUT_DEBUG

static boolean tm_core_xlate_prms_to_ext_raw_clock_meas
(
  pdsm_pd_ext_gnss_meas_s_type       *dest_buf_ptr,
  const gnss_MeasBlkStructType  *source_buf_ptr
)
{

  uint8 u_NumSvs, index, u_GoodSVCount = 0;

  pdsm_LocSVMeasurementStructType *p_pdsmSVMeasDst;
  const gnss_MeasStructType     *p_MeasSrc;

  static uint8 u_partCount = 0;
  uint8 u_MaxPartCount = 0;

  uint8 u_SourceOfTime = 0;
  uint16 w_EnabledConstell = tm_core_info.config_info.gnss_meas_rep_constell_config;

#ifdef FEATURE_GNSS_RAW_MEAS_OUTPUT_DEBUG
  MSG_MED("Entered tm_core_xlate_prms_to_ext_raw_clock_meas,op_mode%f",tm_core_info.session_info.op_mode,0,0);
#endif

  if((dest_buf_ptr == NULL)       ||
     (source_buf_ptr == NULL))
  {
     MSG_HIGH("tm_core_xlate_prms_to_ext_raw_clock_meas received NULL pointer",0,0,0);
     return(FALSE);
  }

  /* If the operation mode is MSA don't send out GNSS measurement block. This is to protect from
       unpropagated measurement (received from ME) from going out. 
  */
  if(tm_core_info.session_info.op_mode==TM_SESS_OPERATION_MODE_MSA)
  {
        MSG_MED("tm_core_xlate_prms_to_ext_raw_clock_meas:MSA mode, No GNSS measurement report",0,0,0);
        return(FALSE);
  }

     /* Start with all parameters NULLed out, especially validity flags */
     memset(dest_buf_ptr, 0, sizeof(pdsm_pd_ext_gnss_meas_s_type));

     /* Evaluate constellations enabled for raw measurement reporting and determine max message part
         Note that this can be different from maxMessage number reported by SM, as enabled constellation
         for reporting can be different.
     */

     if(w_EnabledConstell & NV_GPS_MEAS_REPORT_ENABLED)
     {
          u_MaxPartCount++;
     }
     if(w_EnabledConstell & NV_GLO_MEAS_REPORT_ENABLED)
     {
          u_MaxPartCount++;
     }
     if(w_EnabledConstell & NV_BDS_MEAS_REPORT_ENABLED)
     {
          u_MaxPartCount++;
     }
     if(w_EnabledConstell & NV_GAL_MEAS_REPORT_ENABLED)
     {
          u_MaxPartCount++;
     }
     if(w_EnabledConstell & NV_QZSS_MEAS_REPORT_ENABLED)
     {
          u_MaxPartCount++;
     }

     /* Populate message sequence number of maximum number of messages of upcoming transmission.
         Populate with minimum of max parts count determined by this function and max message number
         as received by this function. This is to protect from differences between enabled constellation to ME
         vs enabled constellation for measurement reporting.
     */
     dest_buf_ptr->u_maxMessageNum = MIN(u_MaxPartCount,source_buf_ptr->u_MaxMessageNum);

     /* Will set sequence number after evaluating if constellation is enabled for output or not */

     /* Evaluate if present constellation is of interest for reporting */
     /**** Place holder code. Need to be changed to compare against NV for selection of constellation of interest for reporting */

     switch(source_buf_ptr->e_MeasBlkSrc)
     {
          case GNSS_MEAS_BLK_SRC_GPS:
               if( !(w_EnabledConstell & NV_GPS_MEAS_REPORT_ENABLED))
                    return (FALSE);
               break;
          case GNSS_MEAS_BLK_SRC_GLO:
               if( !(w_EnabledConstell & NV_GLO_MEAS_REPORT_ENABLED))
                         return (FALSE);
               break;
          case GNSS_MEAS_BLK_SRC_BDS:
               if( !(w_EnabledConstell & NV_BDS_MEAS_REPORT_ENABLED))
                              return(FALSE);
               break;
          case GNSS_MEAS_BLK_SRC_QZSS_SBAS:
               if( !(w_EnabledConstell & NV_QZSS_MEAS_REPORT_ENABLED))
                              return(FALSE);

          case GNSS_MEAS_BLK_SRC_GAL:
               if( !(w_EnabledConstell & NV_GAL_MEAS_REPORT_ENABLED))
                              return(FALSE);
               break;

          default:
               return (FALSE);
     }

     /* For every enabled constellation, increment part count by one */
     u_partCount++;
     dest_buf_ptr->u_seqNum = u_partCount;

     /* Reset part count number, when it becomes equal to max parts planned */
     if(dest_buf_ptr->u_maxMessageNum == u_partCount)
     {
          u_partCount = 0;
     }


#ifdef FEATURE_GNSS_RAW_MEAS_OUTPUT_DEBUG

     MSG_MED("RAW_CLK_MEAS: MeasBlk for constellation [%d], Seq#[%d] of MaxNum[%d]",
          source_buf_ptr->e_MeasBlkSrc,source_buf_ptr->u_SeqNum,source_buf_ptr->u_MaxMessageNum);
#endif

     /* Process leap second, Inter-system time bias information when GPS measurement block is received.
         No particular reason for having choosen GPS measurement block to populate these parameters though */
     dest_buf_ptr->b_LeapSecValid = FALSE;
     if(source_buf_ptr->e_MeasBlkSrc == GNSS_MEAS_BLK_SRC_GPS)
     {
          dest_buf_ptr->b_LeapSecValid = TRUE;
          dest_buf_ptr->z_LeapSec.u_leapSec = source_buf_ptr->z_MeasClock.z_Time.z_GnssTimeBias.u_DeltaLeapSec;
          dest_buf_ptr->z_LeapSec.u_leapSecUnc= source_buf_ptr->z_MeasClock.z_Time.z_GnssTimeBias.u_DeltaLeapSecUnc;


          /* Process Inter-system time report. Output this information, irrespective of current meas block source, but conditioned on if constellations of
              interest are enabled for reporting
          */

          /* GPS, GLO Inter-system bias */
          dest_buf_ptr->z_gpsGloInterSystemBias.u_validMask = 0;
          if((w_EnabledConstell & NV_GPS_MEAS_REPORT_ENABLED) && (w_EnabledConstell & NV_GLO_MEAS_REPORT_ENABLED))
          {
               const gnss_InterGnssTimeBias * p_src_ptr = &source_buf_ptr->z_MeasClock.z_Time.z_GnssTimeBias;
               pdsm_InterSystemBiasStructType * p_dst_ptr = &dest_buf_ptr->z_gpsGloInterSystemBias;

               p_dst_ptr->u_validMask = (PD_LOC_SYS_TIME_BIAS_VALID|PD_LOC_SYS_TIME_BIAS_UNC_VALID);
               p_dst_ptr->f_sys1Sys2TB = p_src_ptr->z_GpsGlo.f_TBMs;
               p_dst_ptr->f_sys1Sys2TBUnc= p_src_ptr->z_GpsGlo.f_TBUncMs;
          }


          /* GPS, BDS Inter-system bias */
          dest_buf_ptr->z_gpsBdsInterSystemBias.u_validMask = 0;
          if((w_EnabledConstell & NV_GPS_MEAS_REPORT_ENABLED) && (w_EnabledConstell  & NV_BDS_MEAS_REPORT_ENABLED ))
          {
               const gnss_InterGnssTimeBias * p_src_ptr = &source_buf_ptr->z_MeasClock.z_Time.z_GnssTimeBias;
               pdsm_InterSystemBiasStructType * p_dst_ptr = &dest_buf_ptr->z_gpsBdsInterSystemBias;

               p_dst_ptr->u_validMask = (PD_LOC_SYS_TIME_BIAS_VALID|PD_LOC_SYS_TIME_BIAS_UNC_VALID);
               p_dst_ptr->f_sys1Sys2TB = p_src_ptr->z_GpsBds.f_TBMs;
               p_dst_ptr->f_sys1Sys2TBUnc= p_src_ptr->z_GpsBds.f_TBUncMs;
          }

          /* BDS, GLO Inter-system bias */
          dest_buf_ptr->z_bdsGloInterSystemBias.u_validMask = 0;
          if((w_EnabledConstell & NV_BDS_MEAS_REPORT_ENABLED) && ( w_EnabledConstell &  NV_GLO_MEAS_REPORT_ENABLED ))
          {
               const gnss_InterGnssTimeBias * p_src_ptr = &source_buf_ptr->z_MeasClock.z_Time.z_GnssTimeBias;
               pdsm_InterSystemBiasStructType * p_dst_ptr = &dest_buf_ptr->z_bdsGloInterSystemBias;

               p_dst_ptr->u_validMask = (PD_LOC_SYS_TIME_BIAS_VALID|PD_LOC_SYS_TIME_BIAS_UNC_VALID);
               p_dst_ptr->f_sys1Sys2TB = p_src_ptr->z_BdsGlo.f_TBMs;
               p_dst_ptr->f_sys1Sys2TBUnc= p_src_ptr->z_BdsGlo.f_TBUncMs;
          }


//#if defined(FEATURE_GNSS_RAW_MEAS_OUTPUT_GAL_SUPPORT)     // To be enabled when GAL support is enabled. Note name of the members of source pointer may need to be fixed, based on GAL implementaiton

          /* GPS, GAL Inter-system bias */
          dest_buf_ptr->z_gpsGalInterSystemBias.u_validMask = 0;
          if((w_EnabledConstell & NV_GPS_MEAS_REPORT_ENABLED) && (w_EnabledConstell & NV_GAL_MEAS_REPORT_ENABLED ))
          {
               const gnss_InterGnssTimeBias * p_src_ptr = &source_buf_ptr->z_MeasClock.z_Time.z_GnssTimeBias;
               pdsm_InterSystemBiasStructType * p_dst_ptr = &dest_buf_ptr->z_gpsGalInterSystemBias;

               p_dst_ptr->u_validMask = (PD_LOC_SYS_TIME_BIAS_VALID|PD_LOC_SYS_TIME_BIAS_UNC_VALID);
               p_dst_ptr->f_sys1Sys2TB = p_src_ptr->z_GpsGal.f_TBMs;
               p_dst_ptr->f_sys1Sys2TBUnc= p_src_ptr->z_GpsGal.f_TBUncMs;
          }


          /* GAL, GLO Inter-system bias */
          dest_buf_ptr->z_galGloInterSystemBias.u_validMask = 0;
          if((w_EnabledConstell & NV_GPS_MEAS_REPORT_ENABLED) && ( w_EnabledConstell &  NV_BDS_MEAS_REPORT_ENABLED ))
          {
               const gnss_InterGnssTimeBias * p_src_ptr = &source_buf_ptr->z_MeasClock.z_Time.z_GnssTimeBias;
               pdsm_InterSystemBiasStructType * p_dst_ptr = &dest_buf_ptr->z_galGloInterSystemBias;

               p_dst_ptr->u_validMask = (PD_LOC_SYS_TIME_BIAS_VALID|PD_LOC_SYS_TIME_BIAS_UNC_VALID);
               p_dst_ptr->f_sys1Sys2TB = p_src_ptr->z_GalGlo.f_TBMs;
               p_dst_ptr->f_sys1Sys2TBUnc= p_src_ptr->z_GalGlo.f_TBUncMs;
          }

          /* GAL, BDS Inter-system bias */
          dest_buf_ptr->z_galBdsInterSystemBias.u_validMask = 0;
          if((w_EnabledConstell & NV_GPS_MEAS_REPORT_ENABLED) && ( w_EnabledConstell & NV_BDS_MEAS_REPORT_ENABLED ))
          {
               const gnss_InterGnssTimeBias * p_src_ptr = &source_buf_ptr->z_MeasClock.z_Time.z_GnssTimeBias;
               pdsm_InterSystemBiasStructType * p_dst_ptr = &dest_buf_ptr->z_galBdsInterSystemBias;

               p_dst_ptr->u_validMask = (PD_LOC_SYS_TIME_BIAS_VALID|PD_LOC_SYS_TIME_BIAS_UNC_VALID);
               p_dst_ptr->f_sys1Sys2TB = p_src_ptr->z_GalBds.f_TBMs;
               p_dst_ptr->f_sys1Sys2TBUnc= p_src_ptr->z_GalBds.f_TBUncMs;
          }
//#endif
     }
/* Populate frequency information. This will be sent, irrespective of constellation
*/

     dest_buf_ptr->b_clockFreqValid = TRUE;
     dest_buf_ptr->z_clockFreq.f_clockDrift = source_buf_ptr->z_MeasClock.z_Freq.f_ClkFreqBias;
     dest_buf_ptr->z_clockFreq.f_clockDriftUnc = source_buf_ptr->z_MeasClock.z_Freq.f_ClkFreqUnc;

     /* Map source of frequency internal to external */

     switch(source_buf_ptr->z_MeasClock.z_Freq.e_SourceOfFreq)
     {
          case C_FREQ_SRC_INVALID:
               dest_buf_ptr->z_clockFreq.e_sourceOfFreq = PD_LOC_FREQ_SOURCE_INVALID;
               break;
          case C_FREQ_SRC_EXTERNAL:
               dest_buf_ptr->z_clockFreq.e_sourceOfFreq = PD_LOC_FREQ_SOURCE_EXTERNAL;
               break;
          case C_FREQ_SRC_PE_CLK_REPORT:
               dest_buf_ptr->z_clockFreq.e_sourceOfFreq = PD_LOC_FREQ_SOURCE_PE_CLK_REPORT;
               break;
          default:
               dest_buf_ptr->z_clockFreq.e_sourceOfFreq = PD_LOC_FREQ_SOURCE_UNKNOWN;
               break;
     }


/* Process measurement structure */
     dest_buf_ptr->z_GnssMeas.b_isSystemTimeValid = TRUE;
     dest_buf_ptr->z_GnssMeas.z_systemTime.q_refFCount = source_buf_ptr->z_MeasClock.q_RefFCount;
     dest_buf_ptr->z_GnssMeas.z_systemTime.b_SystemRtcValid = source_buf_ptr->z_MeasClock.u_SystemRtcValid;
     dest_buf_ptr->z_GnssMeas.z_systemTime.t_systemRtcMs = source_buf_ptr->z_MeasClock.t_SystemRtcMs;

     /* Process constellation specific information */
     switch(source_buf_ptr->e_MeasBlkSrc)
     {
          case GNSS_MEAS_BLK_SRC_GPS:

               dest_buf_ptr->z_GnssMeas.e_system = PD_LOC_SV_SYSTEM_GPS;

               /* Clock processing */
               dest_buf_ptr->z_GnssMeas.z_systemTime.w_systemWeek = source_buf_ptr->z_MeasClock.z_Time.z_GpsTime.w_GpsWeek;
               dest_buf_ptr->z_GnssMeas.z_systemTime.q_systemMsec = source_buf_ptr->z_MeasClock.z_Time.z_GpsTime.q_GpsMsec;
               dest_buf_ptr->z_GnssMeas.z_systemTime.f_systemClkTimeBias = source_buf_ptr->z_MeasClock.z_Time.z_GpsTime.f_ClkTimeBias;
               dest_buf_ptr->z_GnssMeas.z_systemTime.f_systemClkTimeUncMs = source_buf_ptr->z_MeasClock.z_Time.z_GpsTime.f_ClkTimeUncMs;
               u_SourceOfTime = source_buf_ptr->z_MeasClock.z_Time.z_GpsTime.e_SourceOfTime;

               break;
          case GNSS_MEAS_BLK_SRC_GLO:

               dest_buf_ptr->z_GnssMeas.e_system = PD_LOC_SV_SYSTEM_GLONASS;

               /* Clock processing */
               /* Mark GPS week as unknow for GLONASS constellation */
               dest_buf_ptr->z_GnssMeas.z_systemTime.w_systemWeek = C_GPS_WEEK_UNKNOWN;

               dest_buf_ptr->z_GnssMeas.z_systemTime.u_gloFourYear = source_buf_ptr->z_MeasClock.z_Time.z_GloTime.u_FourYear;
               dest_buf_ptr->z_GnssMeas.z_systemTime.w_gloDays = source_buf_ptr->z_MeasClock.z_Time.z_GloTime.w_Days;
               dest_buf_ptr->z_GnssMeas.z_systemTime.q_systemMsec = source_buf_ptr->z_MeasClock.z_Time.z_GloTime.q_GloMsec;
               dest_buf_ptr->z_GnssMeas.z_systemTime.f_systemClkTimeBias = source_buf_ptr->z_MeasClock.z_Time.z_GloTime.f_ClkTimeBias;
               dest_buf_ptr->z_GnssMeas.z_systemTime.f_systemClkTimeUncMs = source_buf_ptr->z_MeasClock.z_Time.z_GloTime.f_ClkTimeUncMs;
               u_SourceOfTime = source_buf_ptr->z_MeasClock.z_Time.z_GloTime.e_SourceOfTime;
          break;
          case GNSS_MEAS_BLK_SRC_BDS:

               dest_buf_ptr->z_GnssMeas.e_system = PD_LOC_SV_SYSTEM_BDS;

               /* Clock processing */
               dest_buf_ptr->z_GnssMeas.z_systemTime.w_systemWeek = source_buf_ptr->z_MeasClock.z_Time.z_BdsTime.w_BdsWeek;
               dest_buf_ptr->z_GnssMeas.z_systemTime.q_systemMsec = source_buf_ptr->z_MeasClock.z_Time.z_BdsTime.q_BdsMsec;
               dest_buf_ptr->z_GnssMeas.z_systemTime.f_systemClkTimeBias = source_buf_ptr->z_MeasClock.z_Time.z_BdsTime.f_ClkTimeBias;
               dest_buf_ptr->z_GnssMeas.z_systemTime.f_systemClkTimeUncMs = source_buf_ptr->z_MeasClock.z_Time.z_BdsTime.f_ClkTimeUncMs;
               u_SourceOfTime = source_buf_ptr->z_MeasClock.z_Time.z_BdsTime.e_SourceOfTime;
               break;
          case GNSS_MEAS_BLK_SRC_QZSS_SBAS:

               /* No Clock processing needed for
                  dest_buf_ptr->z_GnssMeas.e_system = PD_LOC_SV_SYSTEM_QZSS;
               */

               break;


          case GNSS_MEAS_BLK_SRC_GAL:

               dest_buf_ptr->z_GnssMeas.e_system = PD_LOC_SV_SYSTEM_GALILEO;

               /* Clock processing */
               dest_buf_ptr->z_GnssMeas.z_systemTime.w_systemWeek = source_buf_ptr->z_MeasClock.z_Time.z_GalTime.w_GalWeek;
               dest_buf_ptr->z_GnssMeas.z_systemTime.q_systemMsec = source_buf_ptr->z_MeasClock.z_Time.z_GalTime.q_GalMsec;
               dest_buf_ptr->z_GnssMeas.z_systemTime.f_systemClkTimeBias = source_buf_ptr->z_MeasClock.z_Time.z_GalTime.f_ClkTimeBias;
               dest_buf_ptr->z_GnssMeas.z_systemTime.f_systemClkTimeUncMs = source_buf_ptr->z_MeasClock.z_Time.z_GalTime.f_ClkTimeUncMs;
               u_SourceOfTime = source_buf_ptr->z_MeasClock.z_Time.z_GalTime.e_SourceOfTime;
               break;

          default:
                    return(FALSE);
          }

          /* Mapping of source of time from internal to external shall be done in QMI_SHIM function, reusing existing mapping frunction
              used in position report QMI message
          */

          dest_buf_ptr->z_GnssMeas.z_systemTime.e_SourceOfTime = u_SourceOfTime;


          /* Measurement processing */
          u_NumSvs = MIN(source_buf_ptr->u_NumSvs,PDSM_EXT_GNSS_MEAS_NUM_SVS);


#ifdef FEATURE_GNSS_RAW_MEAS_OUTPUT_DEBUG

{
          char buf[1024];

          for(index=0;index<u_NumSvs;index++)
          {
               snprintf(buf, sizeof(buf),"SV_TIME_AF:%d,%d,%.4f,%d\n",source_buf_ptr->z_Gnss[index].w_Sv,source_buf_ptr->z_Gnss[index].z_UnfiltMeas.z_SvTime.q_Ms,source_buf_ptr->z_Gnss[index].z_UnfiltMeas.z_SvTime.f_SubMs,source_buf_ptr->z_Gnss[index].x_LatencyMs);
               pe_DebugPrint(0,buf);
          }

          snprintf(buf, sizeof(buf),"GPS_TIME_AF:%d,%.4f\n",source_buf_ptr->z_MeasClock.z_Time.z_GpsTime.q_GpsMsec,source_buf_ptr->z_MeasClock.z_Time.z_GpsTime.f_ClkTimeBias);
          pe_DebugPrint(0,buf);
}
#endif
          u_GoodSVCount = 0;

          for(index=0;index<u_NumSvs;index++)
          {
               p_MeasSrc = &source_buf_ptr->z_Gnss[index];
               p_pdsmSVMeasDst = &dest_buf_ptr->z_GnssMeas.z_svMeasurement[u_GoodSVCount];

               /* To reduce data traffic and transfer, filter for good SV measurement for reporting */
               if( (PRN_OK( p_MeasSrc->w_Sv ) || BDS_ID_OK( p_MeasSrc->w_Sv )     || ((SLOTID_OK( p_MeasSrc->w_Sv )
                     || ((GLO_SLOTID_UNKNOWN == p_MeasSrc->w_Sv)     && !GLO_SIGNED_FREQ_INVALID( p_MeasSrc->b_FreqNum )))
                      && p_MeasSrc->w_Cno > 0))
                 && (~p_MeasSrc->q_MeasStatus & MEAS_STATUS_ACQ_OR_PROBATION)
                 && (~p_MeasSrc->q_MeasStatus & MEAS_STATUS_FROM_RNG_DIFF)
                 && (~p_MeasSrc->q_MeasStatus & MEAS_STATUS_DONT_USE)
                 && (~p_MeasSrc->q_MeasStatus & MEAS_STATUS_XCORR)
                 && (~p_MeasSrc->q_MeasStatus & MEAS_STATUS_NEED_SIR_CHECK))
               {
                    if(FALSE == tm_core_xlate_gnss_prms_to_pd_gnss_meas(p_MeasSrc, p_pdsmSVMeasDst, source_buf_ptr->e_MeasBlkSrc))
                    {
                         return (FALSE);
                    }
                    u_GoodSVCount++;
               }
          }

     dest_buf_ptr->z_GnssMeas.u_NumSvs = u_GoodSVCount;
//#if FEATURE_GNSS_RAW_MEAS_OUTPUT_DEBUG
#if 1
     MSG_MED("RAW_CLK_MEAS: FCount[%d], Processing Meas for [%d] SVs",source_buf_ptr->z_MeasClock.q_RefFCount,dest_buf_ptr->z_GnssMeas.u_NumSvs,0);
#endif
     dest_buf_ptr->b_GnssMeasValid = TRUE;
     return(TRUE);
     }

/*===========================================================================

FUNCTION tm_core_xlate_svpoly_to_ext_svpoly_rpt

DESCRIPTION: Translates internal GNSS SV polynomial structure in a form suitable for transmission over QMI

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
#define     FEATURE_GNSS_RAW_SVPOLY_OUTPUT_DEBUG

static boolean tm_core_xlate_svpoly_to_ext_svpoly_rpt
(
  pdsm_pd_ext_SvPoly_s_type       *dest_buf_ptr,
  const sm_SvPolyReportType  *source_buf_ptr
)
{
  int8 i;
  uint16 w_EnabledConstell = tm_core_info.config_info.sv_poly_rep_constell_config;

#ifdef FEATURE_GNSS_RAW_SVPOLY_OUTPUT_DEBUG
  MSG_MED("Entered tm_core_xlate_svpoly_to_ext_svpoly_rpt",0,0,0);
#endif

  if((dest_buf_ptr == NULL)       ||
     (source_buf_ptr == NULL))
  {
     MSG_HIGH("tm_core_xlate_svpoly_to_ext_svpoly_rpt received NULL pointer",0,0,0);
     return(FALSE);
  }

  dest_buf_ptr->b_FreqNum = 0;

  /* Report polynomials for only the SVs for which reporting is configured */
  if(PRN_OK( source_buf_ptr->z_SvPolyReport.w_SvId ))
  {
       if(!(w_EnabledConstell & NV_GPS_MEAS_REPORT_ENABLED))
            return(FALSE);
  }
  else if(BDS_ID_OK( source_buf_ptr->z_SvPolyReport.w_SvId ))
  {
       if(!(w_EnabledConstell & NV_BDS_MEAS_REPORT_ENABLED))
            return(FALSE);
  }
  else if(GAL_ID_OK( source_buf_ptr->z_SvPolyReport.w_SvId ))
  {
       if(!(w_EnabledConstell & NV_GAL_MEAS_REPORT_ENABLED))
            return(FALSE);
  }
  else if(SLOTID_OK( source_buf_ptr->z_SvPolyReport.w_SvId ) || ((GLO_SLOTID_UNKNOWN == source_buf_ptr->z_SvPolyReport.w_SvId)
       && !GLO_SIGNED_FREQ_INVALID( source_buf_ptr->z_SvPolyReport.b_FreqNum )))
  {
       if(!(w_EnabledConstell & NV_GLO_MEAS_REPORT_ENABLED))
            return(FALSE);

          /* Map signed glonass frequency to postive mapping (between 1 to 14) */
          dest_buf_ptr->b_FreqNum = source_buf_ptr->z_SvPolyReport.b_FreqNum + GLO_FREQ_NUMBER_OFFSET + 1;
  }
#ifdef FEATURE_GNSS_RAW_SVPOLY_OUTPUT_DEBUG
     MSG_MED("Qualified SV in tm_core_xlate_svpoly_to_ext_svpoly_rpt",0,0,0);
#endif


  dest_buf_ptr->u_Sv= source_buf_ptr->z_SvPolyReport.w_SvId;
  dest_buf_ptr->w_SvPolyFlags= source_buf_ptr->z_SvPolyReport.u_SvPolyFlags;
  dest_buf_ptr->u_Iode= source_buf_ptr->z_SvPolyReport.w_Iode;
  dest_buf_ptr->d_T0= source_buf_ptr->z_SvPolyReport.d_T0;
  for(i=0;i<3;i++)
  {
       dest_buf_ptr->pd_xyz0[i]= source_buf_ptr->z_SvPolyReport.pd_xyz0[i];
  }
  for(i=0;i<9;i++)
  {
       dest_buf_ptr->pd_xyzN[i]= source_buf_ptr->z_SvPolyReport.pd_xyzN[i];
  }
  for(i=0;i<4;i++)
  {
       dest_buf_ptr->pf_other[i]= source_buf_ptr->z_SvPolyReport.pf_other[i];
  }

  dest_buf_ptr->f_PosUnc= source_buf_ptr->z_SvPolyReport.f_PosUnc;
  dest_buf_ptr->f_IonoDelay= source_buf_ptr->z_SvPolyReport.f_IonoDelay;
  dest_buf_ptr->f_IonoDot= source_buf_ptr->z_SvPolyReport.f_IonoDot;
  dest_buf_ptr->f_sbasIonoDelay= source_buf_ptr->z_SvPolyReport.f_sbasIonoDelay;
  dest_buf_ptr->f_sbasIonoDot= source_buf_ptr->z_SvPolyReport.f_sbasIonoDot;
  dest_buf_ptr->f_TropoDelay= source_buf_ptr->z_SvPolyReport.f_TropoDelay;
  dest_buf_ptr->f_Elevation= source_buf_ptr->z_SvPolyReport.f_Elevation;
  dest_buf_ptr->f_ElevDot= source_buf_ptr->z_SvPolyReport.f_ElevDot;
  dest_buf_ptr->f_ElevUnc= source_buf_ptr->z_SvPolyReport.f_ElevUnc;
  for(i=0;i<12;i++)
  {
       dest_buf_ptr->d_VelCoef[i]= source_buf_ptr->z_SvPolyReport.d_VelCoef[i];
  }


  return(TRUE);
}


/*===========================================================================

FUNCTION tm_core_xlate_pos_to_ext_status

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_xlate_pos_to_ext_status
(
  pdsm_pd_ext_pos_s_type  *dest_buf_ptr,
  sm_GnssFixRptStructType *source_buf_ptr,
  uint32                  remaing_fix_time
)
{
  tm_lr_resp_type    *lr_resp_ptr = NULL;
  qword              qw_time;
  uint32             orig_time;
  uint32             gps_utc_offset;
  pdsm_xtra_download_info_s_type p_download_info ;
  boolean xtra_valid = TRUE;
  float              f_tmp;

  if((dest_buf_ptr == NULL) || (source_buf_ptr == NULL))
  {
    return;
  }

  lr_resp_ptr = (tm_lr_resp_type *)pdsm_getbuf( sizeof(tm_lr_resp_type)); /*lint !e826 */

  if(lr_resp_ptr != NULL)
  {

    memset(lr_resp_ptr, 0, sizeof(tm_lr_resp_type));
    dest_buf_ptr->fixTimeRemaining = (uint32)remaing_fix_time / 1000; // time in seconds

    /* sanity check: If pos is not valid, SvInfo should be invalid */
    if (!source_buf_ptr->z_NavPos.z_PosFlags.b_IsValid)
    {
      dest_buf_ptr->pd_data.z_NavSvInfo.u_SvInfoValid = FALSE;
    }

    if(tm_util_convert_lr(source_buf_ptr, lr_resp_ptr))
    {    
      qw_time[0] = lr_resp_ptr->timestamp[0];
      qw_time[1] = lr_resp_ptr->timestamp[1];

      (void) qw_div_by_power_of_2(qw_time, qw_time, 16 ); /* obtain time in 1.25 ms */

      /* obtain time in second's */
      (void) qw_div( qw_time, qw_time, 800 );

      /* Save the current time, so we can check for underflow/overflow */
      orig_time = qw_time[0];

      /* Now clear the lower 14 bits of time stamp */
      qw_time[0] &= 0xffffc000;

      /* Now copy the time reference provided from PDE.
      */
      qw_time[0] |= ( lr_resp_ptr->time_ref_cdma & 0x3fff );

      /* Check for Overflow/Underflow
      */
      if( (int32)(qw_time[0] - orig_time) > 0x2000 )
      {
        qw_time[0] -= 0x4000;
      }
      else if ( (int32)(orig_time - qw_time[0]) > 0x2000 )
      {
        qw_time[0] += 0x4000;
      }

      dest_buf_ptr->pd_data.meas_data              = NULL;
      dest_buf_ptr->pd_data.pm_data                = NULL;
      dest_buf_ptr->pd_data.lat                    = lr_resp_ptr->latitude;
      dest_buf_ptr->pd_data.lon                    = lr_resp_ptr->longitude;
      dest_buf_ptr->pd_data.loc_uncertainty_a      = lr_resp_ptr->loc_uncrtnty_a;
      dest_buf_ptr->pd_data.loc_uncertainty_ang    = lr_resp_ptr->loc_uncrtnty_ang;
      dest_buf_ptr->pd_data.loc_uncertainty_p      = lr_resp_ptr->loc_uncrtnty_p;

      dest_buf_ptr->pd_data.time_stamp = qw_time[0];
      dest_buf_ptr->pd_data.opt_field_mask = PDSM_PD_UNCERTAINTY_VALID;

      dest_buf_ptr->pd_data.ext_opt_field_mask = PDSM_PD_LAT_LONG_VALID;
      dest_buf_ptr->pd_data.ext_opt_field_mask |= PDSM_PD_GPS_TIME_VALID;
      dest_buf_ptr->pd_data.ext_opt_field_mask |= PDSM_PD_UTC_TIME_VALID;

      /* Copy msec field received from PE
      */
      if( lr_resp_ptr->msec_incl )
      {
        dest_buf_ptr->pd_data.opt_field_mask |= PDSM_PD_MSEC_VALID;
        dest_buf_ptr->pd_data.time_stamp_msec = lr_resp_ptr->timestamp_msec;
      }

      /* Check if Velocity info is available.
      */
      // always populate velocity fields even if it's invalid
      dest_buf_ptr->pd_data.velocity_hor       = lr_resp_ptr->velocity_hor;
      dest_buf_ptr->pd_data.heading            = lr_resp_ptr->heading;

      /* multiply by 100.0 to convert speed from m/s to cm/s */
      f_tmp = 100.0 * sqrt(source_buf_ptr->z_NavPos.f_VelEnu[0] *
                           source_buf_ptr->z_NavPos.f_VelEnu[0] +
                           source_buf_ptr->z_NavPos.f_VelEnu[1] *
                           source_buf_ptr->z_NavPos.f_VelEnu[1]);
      if (f_tmp > 0x7FFF)
      {
        f_tmp = 0x7FFF;
      }
      else if (f_tmp < (float)(TM_CORE_MIN_VELOCTY * 100) )
      {
        f_tmp = 0.0;
      }
      dest_buf_ptr->pd_data.ext_pos_data.velHor  = (int16)f_tmp;

      /* Calculate horizontal velocity uncertainty(cm/s) */
      f_tmp = tm_util_calc_vel_horiz_uncertainty(&(source_buf_ptr->z_NavPos)) * 100.0;
      if (f_tmp > (float)CGPS_MAX_UINT16)
      {
        dest_buf_ptr->pd_data.ext_pos_data.velUncHoriz = CGPS_MAX_UINT16;
      }
      else
      {
        dest_buf_ptr->pd_data.ext_pos_data.velUncHoriz = (uint16)f_tmp;
      }

      dest_buf_ptr->pd_data.velocity_ver = lr_resp_ptr->velocity_ver;

      if((source_buf_ptr->z_NavPos.f_VelEnu[2] * 100) > CGPS_MAX_INT16)
      {
        dest_buf_ptr->pd_data.ext_pos_data.velVert  = CGPS_MAX_INT16;
      }
      else
      {
        dest_buf_ptr->pd_data.ext_pos_data.velVert  =
          (int16)source_buf_ptr->z_NavPos.f_VelEnu[2] * 100;
      }

      /* Calculate vertical velocity uncertainty(cm/s) */
      f_tmp = tm_util_calc_vel_vert_uncertainty(&(source_buf_ptr->z_NavPos)) * 100.0;
      if (f_tmp > (float)CGPS_MAX_UINT16)
      {
        dest_buf_ptr->pd_data.ext_pos_data.velUncVert = CGPS_MAX_UINT16;
      }
      else
      {
        dest_buf_ptr->pd_data.ext_pos_data.velUncVert = (uint16)f_tmp;
      }

      if ( lr_resp_ptr->velocity_incl )
      {
        dest_buf_ptr->pd_data.opt_field_mask     |= PDSM_PD_VELOCITY_VALID;

        if(lr_resp_ptr->heading_valid)
        {
          dest_buf_ptr->pd_data.ext_opt_field_mask |= PDSM_PD_HEADING_VALID;
        }

        /* Overloaded fix type for VX spec's.
        */
        if (lr_resp_ptr->fix_type)
        {
          dest_buf_ptr->pd_data.fix_type     = TRUE;
        }
        else
        {
          dest_buf_ptr->pd_data.fix_type = FALSE;
        }
      } /* If velocity included is TRUE */

      /* Check if Height info is available.
      */
      if ( lr_resp_ptr->height_incl )
      {
        dest_buf_ptr->pd_data.opt_field_mask     |= PDSM_PD_ALTITUDE_VALID;
        dest_buf_ptr->pd_data.altitude           = lr_resp_ptr->height;
        dest_buf_ptr->pd_data.ext_pos_data.f_altitude = lr_resp_ptr->f_altitude;
        dest_buf_ptr->pd_data.loc_uncertainty_v  = lr_resp_ptr->loc_uncrtnty_v;
      }
      else
      {
        dest_buf_ptr->pd_data.ext_pos_data.f_altitude           = 0;
        dest_buf_ptr->pd_data.altitude           = 0.0;
        dest_buf_ptr->pd_data.loc_uncertainty_v  = 0;
      }

      /* Initialize pos info */
      dest_buf_ptr->pd_data.position_type      = PDSM_PD_POSITION_TYPE_NEW;
      dest_buf_ptr->pd_data.positioning_source = 0;

      dest_buf_ptr->pd_data.positioning_source |= (source_buf_ptr->z_NavPos.z_PosFlags.b_IsGpsUsed)     ?
                                    PDSM_PD_POSITION_SOURCE_GPS : 0;
      dest_buf_ptr->pd_data.positioning_source |= (source_buf_ptr->z_NavPos.z_PosFlags.b_IsGloUsed)     ?
                                    PDSM_PD_POSITION_SOURCE_GLO : 0;
      dest_buf_ptr->pd_data.positioning_source |= (source_buf_ptr->z_NavPos.z_PosFlags.b_IsBdsUsed)     ?
                                                   PDSM_PD_POSITION_SOURCE_BDS     : 0;
      dest_buf_ptr->pd_data.positioning_source |= (source_buf_ptr->z_NavPos.z_PosFlags.b_IsQzssUsed)     ?
                                                   PDSM_PD_POSITION_SOURCE_QZSS     : 0;
      dest_buf_ptr->pd_data.positioning_source |= (source_buf_ptr->z_NavPos.z_PosFlags.b_IsGalUsed)     ?
                                                   PDSM_PD_POSITION_SOURCE_GAL     : 0;
      dest_buf_ptr->pd_data.positioning_source |= (source_buf_ptr->z_NavPos.z_SmPosFlags.b_IsWifi)      ?
                                                   PDSM_PD_POSITION_SOURCE_WIFI    : 0;
      dest_buf_ptr->pd_data.positioning_source |= (source_buf_ptr->z_NavPos.z_SmPosFlags.b_IsCellId)    ?
                                                   PDSM_PD_POSITION_SOURCE_CELLID  : 0;
      dest_buf_ptr->pd_data.positioning_source |= (source_buf_ptr->z_NavPos.z_SmPosFlags.b_IsSensors)   ?
                                                   PDSM_PD_POSITION_SOURCE_SENSORS : 0;
      dest_buf_ptr->pd_data.positioning_source |= (source_buf_ptr->z_NavPos.z_SmPosFlags.b_IsRefLoc)    ?
                                                   PDSM_PD_POSITION_SOURCE_DEFAULT : 0;
      dest_buf_ptr->pd_data.positioning_source |= (source_buf_ptr->z_NavPos.z_SmPosFlags.b_IsCoarsePos) ?
                                    PDSM_PD_POSITION_SOURCE_EPI : 0;

      if(tm_core_info.session_info.op_mode == TM_SESS_OPERATION_MODE_MSA ||
         tm_core_info.session_info.op_mode == TM_SESS_OPERATION_MODE_OTDOA)
      {
        dest_buf_ptr->pd_data.position_mode = PDSM_PD_POSITION_MODE_MSASSISTED;
      }
      else
      {
        dest_buf_ptr->pd_data.position_mode = PDSM_PD_POSITION_MODE_STANDALONE;
      }

      dest_buf_ptr->pd_data.loc_uncertainty_conf = LOC_UNC_CONFIDENCE;

      dest_buf_ptr->pd_data.opt_field_mask       |=PDSM_PD_UTCOFFSET_VALID;

      /* Set gpsUtcOffset in milliseconds */
      if(source_buf_ptr->z_NavPos.q_GpsTimeMs >= source_buf_ptr->z_NavPos.q_UtcTimeMs)
      {
        gps_utc_offset = source_buf_ptr->z_NavPos.q_GpsTimeMs - source_buf_ptr->z_NavPos.q_UtcTimeMs;
      }
      else
      {
        gps_utc_offset = source_buf_ptr->z_NavPos.q_GpsTimeMs + MS_PER_WEEK - source_buf_ptr->z_NavPos.q_UtcTimeMs;
      }

      dest_buf_ptr->pd_data.gpsUtcOffset         = (int8) (gps_utc_offset / 1000);

    }

    dest_buf_ptr->pd_data.opt_field_mask |= PDSM_PD_EXT_STATUS_VALID;
    dest_buf_ptr->pd_data.ext_pos_data.lat = (int32)(C_2_32_OVER_PI * source_buf_ptr->z_NavPos.d_PosLla[0]);
    dest_buf_ptr->pd_data.ext_pos_data.lon = (int32)(C_2_31_OVER_PI * source_buf_ptr->z_NavPos.d_PosLla[1]);
      if (source_buf_ptr->z_NavPos.f_ErrorEllipse[1] > CGPS_WHOLE_EARTH_UNC)
      {
        source_buf_ptr->z_NavPos.f_ErrorEllipse[1] = CGPS_WHOLE_EARTH_UNC;
        MSG_ERROR("Error Ellipse along greater than earth radius",0,0,0);
      }
      if (source_buf_ptr->z_NavPos.f_ErrorEllipse[2] > CGPS_WHOLE_EARTH_UNC)
      {
        source_buf_ptr->z_NavPos.f_ErrorEllipse[2] = CGPS_WHOLE_EARTH_UNC;
        MSG_ERROR("Error Ellipse perp greater than earth radius",0,0,0);
      }
      dest_buf_ptr->pd_data.ext_pos_data.loc_unc_horizontal =
       (uint32)sqrt((source_buf_ptr->z_NavPos.f_ErrorEllipse[1] *
                     source_buf_ptr->z_NavPos.f_ErrorEllipse[1]) +
                    (source_buf_ptr->z_NavPos.f_ErrorEllipse[2] *
                     source_buf_ptr->z_NavPos.f_ErrorEllipse[2]));
    dest_buf_ptr->pd_data.ext_pos_data.alongAxisUnc = CGPS_MAX_UINT16;
    if (source_buf_ptr->z_NavPos.f_ErrorEllipse[1] <  CGPS_MAX_UINT16)
    {
      dest_buf_ptr->pd_data.ext_pos_data.alongAxisUnc = (uint16)source_buf_ptr->z_NavPos.f_ErrorEllipse[1];
    }
    dest_buf_ptr->pd_data.ext_pos_data.perpAxisUnc = CGPS_MAX_UINT16;
    if (source_buf_ptr->z_NavPos.f_ErrorEllipse[2] <  CGPS_MAX_UINT16)
    {
      dest_buf_ptr->pd_data.ext_pos_data.perpAxisUnc = (uint16)source_buf_ptr->z_NavPos.f_ErrorEllipse[2];
    }
    if (dest_buf_ptr->pd_data.ext_pos_data.perpAxisUnc == CGPS_MAX_UINT16 ||
        dest_buf_ptr->pd_data.ext_pos_data.alongAxisUnc == CGPS_MAX_UINT16)
    {
      dest_buf_ptr->pd_data.opt_field_mask &= ~PDSM_PD_UNCERTAINTY_VALID;
    }
    dest_buf_ptr->pd_data.ext_pos_data.timestamp_gps_week = source_buf_ptr->z_NavPos.w_GpsWeek;
    dest_buf_ptr->pd_data.ext_pos_data.timestamp_tow_ms = source_buf_ptr->z_NavPos.q_GpsTimeMs;
    dest_buf_ptr->pd_data.ext_pos_data.time_stamp_unc = (uint32) source_buf_ptr->z_NavPos.f_ClockBiasUncMs;
    dest_buf_ptr->pd_data.ext_pos_data.h_dop = (uint32)(source_buf_ptr->z_NavPos.f_HDOP * 10.0 + 0.5);
    dest_buf_ptr->pd_data.ext_pos_data.v_dop = (uint32)(source_buf_ptr->z_NavPos.f_VDOP * 10.0 + 0.5);
    dest_buf_ptr->pd_data.ext_pos_data.p_dop = (uint32)(source_buf_ptr->z_NavPos.f_PDOP * 10.0 + 0.5);

    if (tm_util_calculate_utc(source_buf_ptr, &dest_buf_ptr->pd_data.ext_pos_data.utc_time, NULL) == FALSE)
    {
      MSG_MED("GPS/GLO time both invalid, UTC invalid! GPSWeek=%d GLO4Year=%d",
              source_buf_ptr->z_NavPos.w_GpsWeek,
              source_buf_ptr->z_NavPos.u_GloNumFourYear,0);
      dest_buf_ptr->pd_data.ext_opt_field_mask &= ~(PDSM_PD_UTC_TIME_VALID);
    }

    xtra_valid = tm_xtra_set_xtra_time_and_validity(&p_download_info);
    /* Init the time for user.
    */
    if (xtra_valid)
    {
      dest_buf_ptr->pd_data.ext_opt_field_mask |= PDSM_PD_XTRA_DATA_VALID;
      dest_buf_ptr->pd_data.ext_pos_data.xtra_start_gps_minutes = p_download_info.startGpsMinutes;
      dest_buf_ptr->pd_data.ext_pos_data.xtra_start_gps_week = p_download_info.startGpsWeek;
      dest_buf_ptr->pd_data.ext_pos_data.xtra_valid_duration_hours = p_download_info.validDurationHours;
    }
    /* time uncertainty */
    dest_buf_ptr->pd_data.ext_pos_data.gps_time_unc_ms = source_buf_ptr->z_NavPos.f_ClockBiasUncMs;

    if (source_buf_ptr->z_NavPos.w_GpsWeek == C_GPS_WEEK_UNKNOWN)
    {
      // If GPS time is unknown, mark all GPS-time related fields as invalid for Intermediate Report
      dest_buf_ptr->pd_data.opt_field_mask &= ~(PDSM_PD_UTCOFFSET_VALID | PDSM_PD_MSEC_VALID);
      dest_buf_ptr->pd_data.ext_opt_field_mask &= ~(PDSM_PD_GPS_TIME_VALID);
    }

    /* Extended time source */
    dest_buf_ptr->pd_data.ext_pos_data.extended_time_source =
      (pdsm_extended_time_src_e_type)source_buf_ptr->z_NavPos.u_InitGpsTimeSrc;

    dest_buf_ptr->pd_data.e_HoriRelIndicator =
      source_buf_ptr->z_NavPos.e_HoriRelIndicator;
    dest_buf_ptr->pd_data.e_VertRelIndicator =
      source_buf_ptr->z_NavPos.e_VertRelIndicator;
    dest_buf_ptr->pd_data.w_SensorAidingIndicator =
      source_buf_ptr->z_NavPos.w_SensorAidingIndicator;
    dest_buf_ptr->pd_data.w_SensorDataUsageMask =
      source_buf_ptr->z_NavPos.w_SensorDataUsageMask;

    /* heading & heading uncertainty */
    dest_buf_ptr->pd_data.ext_pos_data.heading_rad = source_buf_ptr->z_NavPos.f_HeadingRad;
    dest_buf_ptr->pd_data.ext_pos_data.heading_unc_rad = source_buf_ptr->z_NavPos.f_HeadingUncRad;
    dest_buf_ptr->pd_data.ext_pos_data.gnss_heading_rad = source_buf_ptr->z_NavPos.f_GnssHeadingRad;
    dest_buf_ptr->pd_data.ext_pos_data.gnss_heading_unc_rad = source_buf_ptr->z_NavPos.f_GnssHeadingUncRad;

    /* GNSS - SVs, FineSpeec & GoodParity */
    dest_buf_ptr->pd_data.z_NavSvInfo = source_buf_ptr->z_NavSvInfo;

    /* Sensor bias report */
    dest_buf_ptr->pd_data.z_OnDeviceSensorParam.w_SeqenceNumber = source_buf_ptr->z_OnDeviceSensorParam.w_SeqenceNumber;

    if ( source_buf_ptr->z_OnDeviceSensorParam.z_AccelBias.w_SensorValidMask != 0 )
    {
      dest_buf_ptr->pd_data.z_OnDeviceSensorParam.z_AccelBias.w_SensorValidMask = source_buf_ptr->z_OnDeviceSensorParam.z_AccelBias.w_SensorValidMask;
      dest_buf_ptr->pd_data.z_OnDeviceSensorParam.z_AccelBias.f_XaxisValue = source_buf_ptr->z_OnDeviceSensorParam.z_AccelBias.f_XaxisValue;
      dest_buf_ptr->pd_data.z_OnDeviceSensorParam.z_AccelBias.f_YaxisValue = source_buf_ptr->z_OnDeviceSensorParam.z_AccelBias.f_YaxisValue;
      dest_buf_ptr->pd_data.z_OnDeviceSensorParam.z_AccelBias.f_ZaxisValue = source_buf_ptr->z_OnDeviceSensorParam.z_AccelBias.f_ZaxisValue;
    }

    if ( source_buf_ptr->z_OnDeviceSensorParam.z_GyroBias.w_SensorValidMask != 0 )
    {
      dest_buf_ptr->pd_data.z_OnDeviceSensorParam.z_GyroBias.w_SensorValidMask = source_buf_ptr->z_OnDeviceSensorParam.z_GyroBias.w_SensorValidMask;
      dest_buf_ptr->pd_data.z_OnDeviceSensorParam.z_GyroBias.f_XaxisValue = source_buf_ptr->z_OnDeviceSensorParam.z_GyroBias.f_XaxisValue;
      dest_buf_ptr->pd_data.z_OnDeviceSensorParam.z_GyroBias.f_YaxisValue = source_buf_ptr->z_OnDeviceSensorParam.z_GyroBias.f_YaxisValue;
      dest_buf_ptr->pd_data.z_OnDeviceSensorParam.z_GyroBias.f_ZaxisValue = source_buf_ptr->z_OnDeviceSensorParam.z_GyroBias.f_ZaxisValue;
    }

    (void)pdsm_freebuf((char *)lr_resp_ptr);
  }
}


/*===========================================================================

FUNCTION tm_core_xlate_best_avail_pos_to_ext_status

DESCRIPTION convert a BestAvailPos fix report report to pd_ext_status report

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_xlate_best_avail_pos_to_ext_status
(
  pdsm_pd_ext_pos_s_type    *dest_buf_ptr,
  sm_GnssFixRptStructType   *source_buf_ptr,
  uint32                    remaing_fix_time
)
{
  if (dest_buf_ptr == NULL || source_buf_ptr == NULL)
  {
    MSG_MED("NULL ptr!", 0, 0, 0);
    return;
  }

  memset(dest_buf_ptr, 0, sizeof(pdsm_pd_ext_pos_s_type));

  tm_core_xlate_pos_to_ext_status(dest_buf_ptr, source_buf_ptr, remaing_fix_time);

  // sanity checking for VALID flags since this is BestAvailPos
  if (!source_buf_ptr->z_NavPos.z_PosFlags.b_IsValid)
  {
    /* position invalid ? */
    dest_buf_ptr->pd_data.ext_opt_field_mask  &= ~(PDSM_PD_LAT_LONG_VALID);
    dest_buf_ptr->pd_data.opt_field_mask      &= ~(PDSM_PD_UNCERTAINTY_VALID);
    dest_buf_ptr->pd_data.loc_uncertainty_conf = 0; // otherwise PE: 39, ME: 63
  return;
  }
  if (!source_buf_ptr->z_NavPos.z_SmPosFlags.b_IsPuncValid)
  {
    /* PUNC invalid ? */
    dest_buf_ptr->pd_data.opt_field_mask &= ~(PDSM_PD_UNCERTAINTY_VALID);
    dest_buf_ptr->pd_data.loc_uncertainty_conf = 0; // otherwise PE: 39, ME: 63
  }
  if (!source_buf_ptr->z_NavPos.z_SmPosFlags.b_IsGpsTimeValid)
    {
    /* GPS time invalid ? mark all related fields invalid */
    dest_buf_ptr->pd_data.opt_field_mask &= ~(PDSM_PD_UTCOFFSET_VALID | PDSM_PD_MSEC_VALID);
    dest_buf_ptr->pd_data.ext_opt_field_mask &= ~(PDSM_PD_GPS_TIME_VALID);
  }
  if (!source_buf_ptr->z_NavPos.z_PosFlags.b_IsVelValid)
      {
    /* velocity invalid ? */
    dest_buf_ptr->pd_data.opt_field_mask &= ~(PDSM_PD_VELOCITY_VALID);
      }
  if (!source_buf_ptr->z_NavPos.z_PosFlags.b_Is3D)
  {
    /* Altitude invalid ? */
    dest_buf_ptr->pd_data.opt_field_mask &= ~(PDSM_PD_ALTITUDE_VALID);
      // make sure altitude is 0 (tm_util_convert_lr may put in 500)
      dest_buf_ptr->pd_data.ext_pos_data.f_altitude           = 0.0;
      dest_buf_ptr->pd_data.altitude           = 0;
      dest_buf_ptr->pd_data.loc_uncertainty_v  = 0;
  dest_buf_ptr->pd_data.fix_type = FALSE; // 2D fix
  }
  if (!source_buf_ptr->z_NavPos.z_PosFlags.b_IsHeadingValid)
  {
    /* heading invalid ? */
    dest_buf_ptr->pd_data.ext_opt_field_mask &= ~(PDSM_PD_HEADING_VALID);
  }

  /* RH TODO Add pos inject req if PUNC is too large (in original diag pos) */
  }


/*===========================================================================

FUNCTION tm_core_despatch_ppm_rpt_to_prtl

DESCRIPTION
  This function sends a PPM report to a protocol, using the provided
  sess_info function pointer.

DEPENDENCIES

RETURN VALUE
  FALSE: Protocol callback does not accept the ppm
  TRUE: all other cases

SIDE EFFECTS

===========================================================================*/
static boolean tm_core_despatch_ppm_rpt_to_prtl
(
  prtl_session_info_f_type          *sess_info_fp,
  prtl_sess_info_param_u_type       *sess_info_param_ptr,
  srch_ppm_rpt_type                 *ppm_rpt,
  tm_sess_handle_type                sess_handle
)
{
  boolean retVal = FALSE;

  if(sess_info_fp == NULL)
  {
    MSG_ERROR("No protocol callback installed 6", 0, 0, 0);
    return TRUE;
  }

  if(sess_info_param_ptr == NULL || ppm_rpt == NULL)
  {
    MSG_ERROR("Null ptrs passed", 0, 0, 0);
    return TRUE;
  }

  memscpy( (void *) &(sess_info_param_ptr->ppm_data),
           sizeof(sess_info_param_ptr->ppm_data),
          (void *) ppm_rpt,
          sizeof(*ppm_rpt));

  retVal = sess_info_fp(
                 sess_handle,
                 PRTL_SESS_INFO_PPM,
                 sess_info_param_ptr);

  if(retVal == FALSE)
  {
    MSG_ERROR("Protocol rejects PPM", 0, 0, 0);
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

FUNCTION tm_core_lm_sess_update_handler

DESCRIPTION
  This function is to handle sess update from LM

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_lm_sess_update_handler(void *lm_sess_update_data)
{
  tm_session_update_info_s_type *sess_update_ptr = (tm_session_update_info_s_type *)lm_sess_update_data;
  int                           prtlTableIndex = tm_core_get_prtl_idx_by_type(tm_core_info.session_info.from_protocol);
  prtl_sess_info_param_u_type   *sess_info_param_ptr = NULL;
  boolean                       retVal = FALSE;
  pdsm_ext_status_info_s_type   *pz_pd_ext_status_info;

  sm_GpsMeasRptStructType       *p_MeasRptSrc = NULL;
  gnss_MeasBlkStructType        *p_MeasBlkSrc = NULL;
  sm_GnssFixRptStructType       *p_zFix       = NULL;
  prtl_sess_info_e_type         sess_info_type = PRTL_SESS_INFO_NONE;
  sm_gm_pos_fix_info            fix_info;


  sm_SvPolyReportType			*p_SvPolySrc= NULL;

  pz_pd_ext_status_info = (pdsm_ext_status_info_s_type *)os_MemAlloc(sizeof(pdsm_ext_status_info_s_type), OS_MEM_SCOPE_TASK);
  if (NULL == pz_pd_ext_status_info)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Cannot allocate memory for pdsm_ext_status_info_s_type in tm_core_lm_sess_update_handler");
    tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
    return;
  }
  memset( pz_pd_ext_status_info, 0, sizeof(*pz_pd_ext_status_info) );

  if(sess_update_ptr)
  {
    if(sess_update_ptr->q_handle == tm_core_info.session_info.active_sess_handle)
    {
      sess_info_param_ptr = (prtl_sess_info_param_u_type *)pdsm_getbuf(sizeof(prtl_sess_info_param_u_type));

      if(sess_info_param_ptr == NULL)
      {
        MSG_ERROR("Failed to get buffer for sess_info_param_ptr", 0, 0, 0);
        (void)os_MemFree((void **)&pz_pd_ext_status_info);
        tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
        return;
      }

      switch(sess_update_ptr->e_update_type)
      {
          //<<<[+ by Liao] Talk to Sanjeev and Manan to get rid of ack/nack
          //Reason 1. TM does not know which ack/nack belongs to which request
          //Reason 2. There is already another API (sess_req) which we can use to indicate errors.
          case TM_INFO_STATUS_REPORT:
            if(sess_update_ptr->z_update.z_status_rpt.e_status == TM_STATUS_NACK)
            {
              MSG_ERROR("Got NACK from LM, abort fix", 0, 0, 0);
              tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
            }
            break;
          //>>>[+ End]

          case TM_INFO_PPM_REPORT:
            if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
            {
              MSG_LOW("Despatch PPM Rpt", 0, 0, 0);
              /* Set reason to general error for now, if return value */
                retVal = tm_core_despatch_ppm_rpt_to_prtl(
                           tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp,
                           sess_info_param_ptr,
                           &(sess_update_ptr->z_update.z_ppm_rpt.z_PpmRpt),
                           tm_core_info.session_info.active_sess_handle);

                if(retVal == FALSE)
                {
                  tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
                }
            }
            else
            {
              MSG_ERROR("Invalid prtl index, can not report PPM", 0, 0, 0);
              }

            #ifdef FEATURE_GNSS_PERIODIC_PPM
            #error code not present
#endif
      

            break;

          case TM_INFO_PRM_REPORT:

            if ((prtlTableIndex <= -1) || (prtlTableIndex >= TM_PRTL_NUM))
            {
              MSG_ERROR("Invalid prtl index, can not report PRM", 0, 0, 0);
            }
            else
            {
              p_MeasRptSrc = &sess_update_ptr->z_update.z_measure_rpt.z_GpsMeasRpt;
              p_MeasBlkSrc = &sess_update_ptr->z_update.z_measure_rpt.z_GpsMeasRpt.z_MeasBlk;

              /* Update the FCount and measurement freshness for NMEA (GPS or GLO only), no NMEA support for BDS yet
                 TBD: Is this required for GAL? */
              if (GNSS_MEAS_BLK_SRC_GPS == p_MeasBlkSrc->e_MeasBlkSrc ||
                  GNSS_MEAS_BLK_SRC_GLO == p_MeasBlkSrc->e_MeasBlkSrc)
              {
                tm_nmea_set_gnss_meas_update(p_MeasRptSrc);
              }

              /* Set reason to general error for now, if return value */
              if(tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp)
              {
                memscpy((void *)&(sess_info_param_ptr->prm_data),
                        sizeof(sess_info_param_ptr->prm_data),
                        (void *)p_MeasRptSrc,
                        sizeof(*p_MeasRptSrc));

                MSG_LOW("Check for FTEST Sess: %d, Prot: %d, Meas Sys: %d (GPS/GLO/BDS)", tm_core_info.session_info.active_sess_handle,
                         tm_core_info.session_info.from_protocol, p_MeasBlkSrc->e_MeasBlkSrc);

                if( (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_FTEST) && 
                   (tm_core_info.session_info.from_protocol == TM_PRTL_TYPE_DIAG) )
                {
                   tm_core_info.session_info.op_requested &= ~TM_OP_REQ_PRM;
                }     

                retVal = TRUE;

                /* Send all measurements (GPS/GLO/BDS/GAL) to AGNSS protocols to decide which
                   one to use & report to server */
                  retVal = tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp(tm_core_info.session_info.active_sess_handle,
                      PRTL_SESS_INFO_PRM,
                      sess_info_param_ptr);

                if(retVal == FALSE)
                {
                  MSG_ERROR("Protocol rejects PRM", 0, 0, 0);
                  tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
                }
                else
                {
                  /* save measurement to tm_core_info */
                  switch (p_MeasBlkSrc->e_MeasBlkSrc)
                  {
                    case GNSS_MEAS_BLK_SRC_GPS:
                      memscpy((void *)&(tm_core_info.gps_meas_blk), sizeof(tm_core_info.gps_meas_blk),
                             (void *)p_MeasBlkSrc, sizeof(*p_MeasBlkSrc));
                      break;

                    case GNSS_MEAS_BLK_SRC_GLO:
                      memscpy((void *)&(tm_core_info.glo_meas_blk), sizeof(tm_core_info.glo_meas_blk),
                             (void *)p_MeasBlkSrc, sizeof(*p_MeasBlkSrc));
                      break;

                    case GNSS_MEAS_BLK_SRC_BDS:
                      memscpy((void *)&(tm_core_info.bds_meas_blk), sizeof(tm_core_info.bds_meas_blk),
                             (void *)p_MeasBlkSrc, sizeof(*p_MeasBlkSrc));
                      break;

                    case GNSS_MEAS_BLK_SRC_QZSS_SBAS:
                      memscpy((void *)&(tm_core_info.qzss_meas_blk), sizeof(tm_core_info.qzss_meas_blk),
                             (void *)p_MeasBlkSrc, sizeof(*p_MeasBlkSrc));
                      break;

                    case GNSS_MEAS_BLK_SRC_GAL:
                      memscpy((void *)&(tm_core_info.gal_meas_blk), sizeof(tm_core_info.gal_meas_blk),
                             (void *)p_MeasBlkSrc, sizeof(*p_MeasBlkSrc));
                      break;

                    default:
                      MSG_ERROR("tm_core_lm_sess_update_handler: Unknown PRM source", 0, 0, 0);
                      tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
                  }

                  if (p_MeasRptSrc->e_MeasRptReason == GPS_MEAS_RPT_MEAS_DONE)
                  {
                    /*Generate NMEA*/
                    TM_CORE_OUTPUT_NMEA_GAGSV(p_MeasRptSrc);
                    TM_CORE_OUTPUT_NMEA_GPGSV(p_MeasRptSrc);
                    TM_CORE_OUTPUT_NMEA_GLGSV(p_MeasRptSrc);
                    TM_CORE_OUTPUT_NMEA_PQGSV(p_MeasRptSrc);
                  }
                  else
                  {
                    if(tm_core_info.config_info.nmea_config_type.pd_nmea_reporting_type == PDSM_PA_NMEA_REPORT_AT_1HZ_INTERVAL)
                    {
                      TM_CORE_OUTPUT_NMEA_GAGSV(p_MeasRptSrc);
                      TM_CORE_OUTPUT_NMEA_GPGSV(p_MeasRptSrc);
                      TM_CORE_OUTPUT_NMEA_GLGSV(p_MeasRptSrc);
                      TM_CORE_OUTPUT_NMEA_PQGSV(p_MeasRptSrc);
                    }
                    else
                    {
                      /*Cache the measurement report to generate GSV at the FINAL_FIX*/
                      if( (TM_CORE_CONFIG_DD(nmea_output_format) & PDAPI_NMEA_GAGSV) ||
                          (TM_CORE_CONFIG_DD(nmea_output_format) & PDAPI_NMEA_GPGSV) ||
                          (TM_CORE_CONFIG_DD(nmea_output_format) & PDAPI_NMEA_PQGSV) ||
                          (TM_CORE_CONFIG_DD(nmea_output_format) & PDAPI_NMEA_GLGSV))
                      {
                        z_cached_meas_info.cached_meas_valid = TRUE;
                        z_cached_meas_info.z_meas = *p_MeasRptSrc;
                      }
                    }
                  } /* close "else ( if meas. is not of MEAS_DONE )" */

                  /* Send extra pdapi event for PRM */
                  if (p_MeasBlkSrc->u_SeqNum == p_MeasBlkSrc->u_MaxMessageNum)
                  {
                    pz_pd_ext_status_info->client_id       = tm_core_info.session_info.pd_param.client_id;
                    pz_pd_ext_status_info->ext_status_type = PDSM_EXT_STATUS_MEASUREMENT;

                    memset(&(pz_pd_ext_status_info->ext_status_info.ext_meas_report_type), 0, sizeof(pdsm_pd_ext_meas_s_type));

                    tm_core_xlate_prms_to_ext_status(&(pz_pd_ext_status_info->ext_status_info.ext_meas_report_type),
                      &tm_core_info.gps_meas_blk,
                      &tm_core_info.glo_meas_blk,
                      &tm_core_info.bds_meas_blk,
                      &tm_core_info.gal_meas_blk,
                      &tm_core_info.qzss_meas_blk);

                    tm_pdapi_ext_status_event_callback(PDSM_EXT_STATUS_MEASUREMENT,
                      pz_pd_ext_status_info);
                  }

                  /* Output GNSS Measurement report only if NV is enabled */
                  if(1 == tm_core_info.config_info.gnss_meas_rep_qmi_out_control)
                  {
                    /* Send extra pdapi event for Raw measurement, as individual constellation measurements arrive */
                    memset( pz_pd_ext_status_info, 0, sizeof(*pz_pd_ext_status_info) );

                    MSG_HIGH("tm_core PDSM_EXT_STATUS_GNSS_MEASUREMENT",0,0,0);
                    pz_pd_ext_status_info->client_id		 = tm_core_info.session_info.pd_param.client_id;
                    pz_pd_ext_status_info->ext_status_type = PDSM_EXT_STATUS_GNSS_MEASUREMENT;

                    memset(&(pz_pd_ext_status_info->ext_status_info.gnss_meas_report_type), 0, sizeof(pdsm_pd_ext_gnss_meas_s_type));

                    if(TRUE == tm_core_xlate_prms_to_ext_raw_clock_meas(&(pz_pd_ext_status_info->ext_status_info.gnss_meas_report_type),
                                                                         p_MeasBlkSrc))
                    {

                      tm_pdapi_ext_status_event_callback(PDSM_EXT_STATUS_GNSS_MEASUREMENT,pz_pd_ext_status_info);
                    }
                  }

                } /* close "else ( if protocol module accepted sess_info )" */
              } /* close "if(tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp)" */
              else
              {
                MSG_ERROR("No protocol callback installed 8", 0, 0, 0);
              }
            }
            break;

          case TM_INFO_SV_POLY_REPORT:
            if ((prtlTableIndex <= -1) || (prtlTableIndex >= TM_PRTL_NUM))
            {
              MSG_ERROR("Invalid prtl index, can not report SV_POLY, Index:%d", prtlTableIndex, 0, 0);
            }
            else
            {
              if(1 == tm_core_info.config_info.gnss_meas_rep_qmi_out_control)
              {

                p_SvPolySrc = &sess_update_ptr->z_update.z_sv_poly_rpt.z_SvPolyRpt;


                MSG_MED("Received SV_POLY_REPORT in TM", 0,0,0);

                memset( pz_pd_ext_status_info, 0, sizeof(*pz_pd_ext_status_info) );

                pz_pd_ext_status_info->client_id	   = tm_core_info.session_info.pd_param.client_id;
                pz_pd_ext_status_info->ext_status_type = PDSM_EXT_STATUS_SV_POLY_REPORT;

                if(TRUE == tm_core_xlate_svpoly_to_ext_svpoly_rpt(&(pz_pd_ext_status_info->ext_status_info.ext_sv_poly_type),p_SvPolySrc))
                {

                  tm_pdapi_ext_status_event_callback(PDSM_EXT_STATUS_SV_POLY_REPORT, pz_pd_ext_status_info);
                }
              }
            }
            break;

          case TM_INFO_LATE_PRM_REPORT:

            if ((prtlTableIndex <= -1) || (prtlTableIndex >= TM_PRTL_NUM))
            {
              MSG_ERROR("Invalid prtl index, can not report LATE PRM", 0, 0, 0);
            }
            else
            {
              p_MeasRptSrc = &sess_update_ptr->z_update.z_measure_rpt.z_GpsMeasRpt;
              p_MeasBlkSrc = &sess_update_ptr->z_update.z_measure_rpt.z_GpsMeasRpt.z_MeasBlk;

              /* Set reason to general error for now, if return value */
              if(tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp)
              {
                memscpy((void *)&(sess_info_param_ptr->prm_data),
                sizeof(sess_info_param_ptr->prm_data),
                       (void *)p_MeasRptSrc,
                       sizeof(*p_MeasRptSrc));

                MSG_LOW("Check for FTEST Sess: %d, Prot: %d, Meas Sys: %d (GPS/GLO/BDS)", tm_core_info.session_info.active_sess_handle,
                         tm_core_info.session_info.from_protocol, p_MeasBlkSrc->e_MeasBlkSrc);

                switch (p_MeasBlkSrc->e_MeasBlkSrc)
                {
                  case GNSS_MEAS_BLK_SRC_GPS:

                    memscpy((void *)&(tm_core_info.gps_meas_blk),
                            sizeof(tm_core_info.gps_meas_blk),
                           (void *)p_MeasBlkSrc,
                           sizeof(*p_MeasBlkSrc));
                    break;

                  case GNSS_MEAS_BLK_SRC_GLO:

                    memscpy((void *)&(tm_core_info.glo_meas_blk),
                            sizeof(tm_core_info.glo_meas_blk),
                           (void *)p_MeasBlkSrc,
                           sizeof(*p_MeasBlkSrc));
                    break;

                  case GNSS_MEAS_BLK_SRC_BDS:

                    memscpy((void *)&(tm_core_info.bds_meas_blk),
                            sizeof(tm_core_info.bds_meas_blk),
                           (void *)p_MeasBlkSrc,
                           sizeof(*p_MeasBlkSrc));
                    break;

                  case GNSS_MEAS_BLK_SRC_QZSS_SBAS:

                    memscpy((void *)&(tm_core_info.qzss_meas_blk),
                            sizeof(tm_core_info.qzss_meas_blk),
                           (void *)p_MeasBlkSrc,
                           sizeof(*p_MeasBlkSrc));
                    break;

                  case GNSS_MEAS_BLK_SRC_GAL:

                    memscpy((void *)&(tm_core_info.gal_meas_blk),
                            sizeof(tm_core_info.gal_meas_blk),
                           (void *)p_MeasBlkSrc,
                           sizeof(*p_MeasBlkSrc));
                    break;

                  default:

                    MSG_ERROR("tm_core_lm_sess_update_handler: Unknown PRM source", 0, 0, 0);
                    tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
                }

                if( (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_FTEST) && 
                   (tm_core_info.session_info.from_protocol == TM_PRTL_TYPE_DIAG) )
                {
                  tm_core_info.session_info.op_requested &= ~TM_OP_REQ_PRM;
                }

                retVal = TRUE;

                if ( GNSS_MEAS_BLK_SRC_GPS == p_MeasBlkSrc->e_MeasBlkSrc || GNSS_MEAS_BLK_SRC_GLO == p_MeasBlkSrc->e_MeasBlkSrc ||
                    GNSS_MEAS_BLK_SRC_BDS == p_MeasBlkSrc->e_MeasBlkSrc )
                {
                  /* Send all measurements (GPS/GLO/BDS) to AGNSS protocols to decide which one to use 
                   Note: GAL does not support AGNSS */
                  retVal = tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp(tm_core_info.session_info.active_sess_handle,
                                                                                        PRTL_SESS_INFO_PRM,
                                                                                        sess_info_param_ptr);
                }
                else if (GNSS_MEAS_BLK_SRC_QZSS_SBAS == p_MeasBlkSrc->e_MeasBlkSrc )
                {
                  MSG_LOW("Protocol does not support QZSS", 0, 0, 0);
                }

                if(retVal == FALSE)
                {
                  MSG_ERROR("Protocol rejects PRM", 0, 0, 0);
                  tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
                }

                /* Output GNSS Measurement report only if NV is enabled */
                if(tm_core_info.config_info.gnss_meas_rep_qmi_out_control)
                {
                  /* Send extra pdapi event for Raw measurement, as individual constellation measurements arrive */
                  MSG_HIGH("tm_core memset",0,0,0);
                  memset( pz_pd_ext_status_info, 0, sizeof(*pz_pd_ext_status_info) );

                  MSG_HIGH("tm_core Late PDSM_EXT_STATUS_GNSS_MEASUREMENT",0,0,0);
                  pz_pd_ext_status_info->client_id	   = tm_core_info.session_info.pd_param.client_id;
                  pz_pd_ext_status_info->ext_status_type = PDSM_EXT_STATUS_GNSS_MEASUREMENT;

                  memset(&(pz_pd_ext_status_info->ext_status_info.gnss_meas_report_type), 0, sizeof(pdsm_pd_ext_gnss_meas_s_type));

                  if(TRUE == tm_core_xlate_prms_to_ext_raw_clock_meas(&(pz_pd_ext_status_info->ext_status_info.gnss_meas_report_type),
                      p_MeasBlkSrc))
                  {

                    tm_pdapi_ext_status_event_callback(PDSM_EXT_STATUS_GNSS_MEASUREMENT,
                    pz_pd_ext_status_info);
                  }
                }


              } /* close "if(tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp)" */
              else
              {
                MSG_ERROR("No protocol callback installed 9", 0, 0, 0);
              }
            }
            break;

          case TM_INFO_FINAL_FIX_REPORT:

            p_zFix = &sess_update_ptr->z_update.z_fix_rpt.z_GpsFixRpt;
            sess_info_type = PRTL_SESS_INFO_LR;

            if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
            {
              /* Set reason to general error for now, if return value */
              if(tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp)
              {
                /*Generate NMEA*/
                if((tm_core_info.config_info.nmea_config_type.pd_nmea_reporting_type == PDSM_PA_NMEA_REPORT_AT_FINAL_POSITION) &&
                    (z_cached_meas_info.cached_meas_valid == TRUE))
                {
                  TM_CORE_OUTPUT_NMEA_GAGSV(&(z_cached_meas_info.z_meas));
                  TM_CORE_OUTPUT_NMEA_GPGSV(&(z_cached_meas_info.z_meas));
                  TM_CORE_OUTPUT_NMEA_GLGSV(&(z_cached_meas_info.z_meas));
                  TM_CORE_OUTPUT_NMEA_PQGSV(&(z_cached_meas_info.z_meas));
                }

                TM_CORE_OUTPUT_NMEA_GPGGA(p_zFix, FALSE);
                TM_CORE_OUTPUT_NMEA_PQXFI(p_zFix, FALSE);
                
                TM_CORE_OUTPUT_NMEA_GNGNS(p_zFix, FALSE);
                TM_CORE_OUTPUT_NMEA_GPVTG(p_zFix);
                TM_CORE_OUTPUT_NMEA_GPRMC(p_zFix);
                TM_CORE_OUTPUT_NMEA_GPGSA(p_zFix);
                TM_CORE_OUTPUT_NMEA_GNGSA(p_zFix);
                TM_CORE_OUTPUT_NMEA_PQGSA(p_zFix);

                /* Generate extended pd data */
                tm_core_generate_ext_pd_data(p_zFix);

                memscpy((void *)&(sess_info_param_ptr->lr_data), sizeof(sess_info_param_ptr->lr_data),
                        (void *)p_zFix, sizeof(*p_zFix));

                tm_core_info.session_info.op_requested &= ~TM_OP_REQ_LR;

                retVal = tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp(tm_core_info.session_info.active_sess_handle,
                            sess_info_type,
                            sess_info_param_ptr);

                if(retVal == FALSE)
                {
                  MSG_ERROR("Protocol rejects LR", 0, 0, 0);
                  tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
                }
              }
              else
              {
                MSG_ERROR("No protocol callback installed 10 %d", prtlTableIndex, 0, 0);
              }
            }
            else
            {
              if (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_E911)
              {
                /* E911 is a dummy protocol, no protocol call backs are needed. Just clean up the session
                   if no actual LPP session was rcvd from the network after E911 */
                MSG_HIGH("E911 ended without network request. Cleanup session", 0, 0, 0);
                tm_core_abort_recover(TRUE, TM_STOP_REASON_COMPLETED); /*lint !e506 !e730 */
              }
              else
              {
                MSG_ERROR("Invalid prtl index can not report Final Position", 0, 0, 0);
              }
            }

            /* This part of code is moved from standalone protocol into tm_core */
            //if (TM_SESS_OPERATION_MODE_STANDALONE == tm_core_info.session_info.op_mode) // RH changed for CR 628169
            if (TRUE == tm_core_info.qwip_sysd_triggered)
            {
              MSG_HIGH("Trigger GPS Final Fix info (type %d)", sess_info_type, 0, 0);
              tm_sysd_inject_gnss_lr_data(sess_info_param_ptr,&sess_info_type);
            }
            
            fix_info.fix_type = GEOFENCE_POS_FIX_TYPE_FINAL;        
            fix_info.y_pos_fix_ext_info.z_gnss_fix_ext_info.b_is_nw_asst_fix = FALSE;
            fix_info.fix_src = GEOFENCE_POS_FIX_SRC_GNSS;
            fix_info.pos_fix = *(&(sess_info_param_ptr->lr_data));

            /*Send the LR to Geofence module*/
            (void)gm_position_update(&fix_info);

            MSG_HIGH("Timesource Final Fix: 0x%x Tunc(ns): %d Wk: %d",
                   (uint8)  sess_update_ptr->z_update.z_fix_rpt.z_GpsFixRpt.z_NavPos.u_InitGpsTimeSrc,
                   (uint32)(sess_update_ptr->z_update.z_fix_rpt.z_GpsFixRpt.z_NavPos.f_ClockBiasUncMs*1000) ,
                            sess_update_ptr->z_update.z_fix_rpt.z_GpsFixRpt.z_NavPos.w_GpsWeek);

            break;

          case TM_INFO_INTERMEDIATE_FIX_REPORT:
          {
            boolean bBestAvailPos              = FALSE;
            sm_GnssFixRptStructType *p_zUTCFix = NULL;

            p_zFix = &sess_update_ptr->z_update.z_fix_rpt.z_GpsFixRpt;
            sess_info_type = PRTL_SESS_INFO_INTERM_LR;

            if (p_zFix->z_NavPos.z_SmPosFlags.b_IsPosBestAvail)
            {
              bBestAvailPos = TRUE; /* original NO_FIX report: CellDB fix, WiFi fix etc non-GNSS fix */
            }
                
            if (p_zFix->z_NavPos.w_GpsWeek != C_GPS_WEEK_UNKNOWN)
            {     
              /* According to PE, if GPSWeek is valid, UTC time is guaranteed to be valid, 
                can output UTC time in NMEA, otherwise empty UTC */
              p_zUTCFix = p_zFix;
            }

            memset(&(pz_pd_ext_status_info->ext_status_info.ext_pos_report_type), 0, sizeof(pdsm_pd_ext_pos_data_s_type));
                
            if (tm_core_info.config_info.nmea_config_type.pd_nmea_reporting_type == PDSM_PA_NMEA_REPORT_AT_1HZ_INTERVAL)
            {     
              /* Generate NULL NMEA strings for intermediate fixes atleast for now */

              if (FALSE == tm_core_info.qwip_sysd_triggered ||
                  TM_SESS_OPERATION_MODE_STANDALONE != tm_core_info.session_info.op_mode)
              {
                /* Send this as a NO-FIX to NMEA so that only UTC filed is populated in GGA */
                TM_CORE_OUTPUT_NMEA_GPGGA(p_zUTCFix, TRUE);
                TM_CORE_OUTPUT_NMEA_PQXFI(p_zUTCFix, TRUE);
              }

              TM_CORE_OUTPUT_NMEA_GNGNS(p_zUTCFix, TRUE);
              TM_CORE_OUTPUT_NMEA_GPVTG(NULL);

              if (FALSE == tm_core_info.qwip_sysd_triggered ||
                  TM_SESS_OPERATION_MODE_STANDALONE != tm_core_info.session_info.op_mode)
              {
                TM_CORE_OUTPUT_NMEA_GPRMC(NULL);
              }

              TM_CORE_OUTPUT_NMEA_GPGSA(NULL);
              TM_CORE_OUTPUT_NMEA_GNGSA(NULL);
              TM_CORE_OUTPUT_NMEA_PQGSA(NULL);
            }

            tm_core_info.session_info.q_FixTimeRemainingMSec = sess_update_ptr->z_update.z_fix_rpt.q_FixTimeRemainingMSec;

            /** not all procotol submodules consumes INTERM_LR: UMTS does */
            if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
            {
              memscpy((void *)&(sess_info_param_ptr->lr_data), sizeof(sess_info_param_ptr->lr_data),
                     (void *)&(sess_update_ptr->z_update.z_fix_rpt.z_GpsFixRpt),
                     sizeof(sess_update_ptr->z_update.z_fix_rpt.z_GpsFixRpt));

              /* for BestAvailPos (originally NO_FIX), let AGPS handles it like NO_FIX
                 since it's not GNSS fix */
              (void) tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp(
                     tm_core_info.session_info.active_sess_handle,
                     (bBestAvailPos ? PRTL_SESS_INFO_NOFIX : PRTL_SESS_INFO_INTERM_LR),
                                                                           sess_info_param_ptr);
            }
            else
            {
              MSG_HIGH("Invalid prtl index, can not report TM_INFO_INTERMEDIATE_FIX_REPORT", 0, 0, 0);
            }

            if(FALSE == tm_core_info.qwip_sysd_triggered)
            {
              /* Send extra pdapi event for intermediate fix report */
              pz_pd_ext_status_info->client_id       = tm_core_info.session_info.pd_param.client_id;
              pz_pd_ext_status_info->ext_status_type = PDSM_EXT_STATUS_POS_REPORT;

              if (FALSE == bBestAvailPos)
              {
                tm_core_xlate_pos_to_ext_status(&(pz_pd_ext_status_info->ext_status_info.ext_pos_report_type),
                                                p_zFix,
                                                sess_update_ptr->z_update.z_fix_rpt.q_FixTimeRemainingMSec);

                 tm_pdapi_ext_status_event_callback(PDSM_EXT_STATUS_POS_REPORT,
                                                    pz_pd_ext_status_info);
              }
              else /* if (tm_core_info.config_info.send_no_fix_report == 1) */  // RH TODO : always send now
              {
                tm_core_xlate_best_avail_pos_to_ext_status(&(pz_pd_ext_status_info->ext_status_info.ext_pos_report_type),
                                                            p_zFix,
                                                            sess_update_ptr->z_update.z_fix_rpt.q_FixTimeRemainingMSec);
               
                tm_pdapi_ext_status_event_callback(PDSM_EXT_STATUS_POS_REPORT,
                                                    pz_pd_ext_status_info);
              }
            }

            /* This part of code has been moved from standalone protocol into tm_core */
            if (TRUE == tm_core_info.qwip_sysd_triggered)
            {
              MSG_HIGH("QWiP: Trigger GPS Fix info (type %d)",sess_info_type,0,0);
              tm_sysd_inject_gnss_lr_data(sess_info_param_ptr,&sess_info_type);
            }
            if ((TRUE == tm_core_info.qwip_sysd_triggered) && (TM_SESS_OPERATION_MODE_STANDALONE == tm_core_info.session_info.op_mode))
            {
              TM_CORE_OUTPUT_NMEA_GPGGA(NULL, TRUE);
              TM_CORE_OUTPUT_NMEA_PQXFI(NULL, TRUE);
              TM_CORE_OUTPUT_NMEA_GPRMC(NULL);
            }
                
            if(bBestAvailPos == FALSE)
            {     
              /* Send the LR to Geofence module */
              fix_info.fix_type = GEOFENCE_POS_FIX_TYPE_INTERMEDIATE;                      
              fix_info.y_pos_fix_ext_info.z_gnss_fix_ext_info.b_is_nw_asst_fix = FALSE;
              fix_info.fix_src = GEOFENCE_POS_FIX_SRC_GNSS;              
              fix_info.pos_fix = *(&(sess_info_param_ptr->lr_data));

              (void)gm_position_update(&fix_info);
            }

            MSG_HIGH("Timesource Int Fix: 0x%x Tunc(ns): %d Wk: %d",
                   (uint8)  p_zFix->z_NavPos.u_InitGpsTimeSrc,
                   (uint32)(p_zFix->z_NavPos.f_ClockBiasUncMs*1000),
                            p_zFix->z_NavPos.w_GpsWeek);
          }
          break;
      }

      (void) pdsm_freebuf((char *)sess_info_param_ptr);
    } /* sess_update_ptr->q_handle == tm_core_info.session_info.active_sess_handle */
#ifdef FEATURE_GNSS_PERIODIC_PPM
    #error code not present
#endif
    else
    {
      MSG_MED("Rejecting Invalid Handle %d ActiveHandle %d UpdateType %d",
                 sess_update_ptr->q_handle,
                 tm_core_info.session_info.active_sess_handle,
                 sess_update_ptr->e_update_type);
    }
  }
  (void)os_MemFree((void **)&pz_pd_ext_status_info);
}

/*===========================================================================

FUNCTION tm_core_lm_sess_info_handler

DESCRIPTION
  This function is to handle session information from LM. The session info
  is available assitance data status from PE (CD).

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_lm_sess_info_handler(void *lm_sess_info_data)
{
  tm_info_s_type *sess_info_ptr = (tm_info_s_type *)lm_sess_info_data;
  int prtlTableIndex = tm_core_get_prtl_idx_by_type(tm_core_info.session_info.from_protocol);
  prtl_sess_info_param_u_type  *sess_info_param_ptr = NULL;
  sm_InternalAssistDataStatusStructType *p_sm_AssistDataStatus = NULL;
  uint32 q_long_word2 = 0;
  uint8  u_sv_cnt = 0, u_i;
  boolean retVal = FALSE;
  prtl_event_u_type event_payload;
  pdsm_ext_status_info_s_type   *pz_pd_ext_status_info;
  nv_item_type                  nv_write_buffer;

  lm_request_info_s_type               lm_info_req_param;
  boolean pos_inj_req = FALSE;

  pz_pd_ext_status_info = (pdsm_ext_status_info_s_type *)os_MemAlloc(sizeof(pdsm_ext_status_info_s_type), OS_MEM_SCOPE_TASK);
  if (NULL == pz_pd_ext_status_info)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Cannot allocate memory for pdsm_ext_status_info_s_type in tm_core_lm_sess_info_handler");
    tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
    return;
  }

  memset((void *)&lm_info_req_param, 0, sizeof(lm_request_info_s_type));
  memset( pz_pd_ext_status_info, 0, sizeof(*pz_pd_ext_status_info) );

  if(sess_info_ptr)
  {
    sess_info_param_ptr = (prtl_sess_info_param_u_type *)pdsm_getbuf(sizeof(prtl_sess_info_param_u_type));

    if(sess_info_param_ptr == NULL)
    {
      MSG_ERROR("Failed to get buffer for sess_info_param_ptr", 0, 0, 0);
      (void)os_MemFree((void **)&pz_pd_ext_status_info);
      tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
      return;
    }

    switch(sess_info_ptr->e_info_type)
    {
      case TM_INFO_AIDING_DATA_WISHLIST:

        if ((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
        {
          /* Set reason to general error for now, if return value */
          if (tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp)
          {
            memscpy((void *)&(sess_info_param_ptr->aiding_data_status),
                   sizeof(sess_info_param_ptr->aiding_data_status),
                   (void *)&(sess_info_ptr->z_info.z_wishlist),
                   sizeof(sess_info_ptr->z_info.z_wishlist));

            p_sm_AssistDataStatus = &(sess_info_param_ptr->aiding_data_status.z_assist_data_status);

            MSG_4(MSG_SSID_GPSSM, MSG_LEGACY_MED,"TmCore proc. LM wishlist: 0x%X (GPS) 0x%X (GLONASS) 0x%X (BDS), 0x%X (QZSS)",
                    p_sm_AssistDataStatus->q_WishListMask,
                    p_sm_AssistDataStatus->z_GloAssistData.q_WishListMask,
                    p_sm_AssistDataStatus->z_BdsAssistData.q_WishListMask,
                    p_sm_AssistDataStatus->z_QzssAssistData.q_WishListMask);

            MSG_MED("TmCore proc. LM wishlist: 0x%lX (GAL)", p_sm_AssistDataStatus->z_GalAssistData.q_WishListMask,
                    0,0);

  

            tm_core_info.session_info.eph_svmask = p_sm_AssistDataStatus->q_EphHaveList;
            tm_core_info.session_info.alm_svmask = p_sm_AssistDataStatus->q_AlmHaveList;
            tm_core_info.session_info.iono = 0;
            if (p_sm_AssistDataStatus->q_WishListMask & C_GPS_ASSIS_IONO)
            {
              tm_core_info.session_info.iono = 1;
            }

            tm_core_info.session_info.utc = 0;
            if (p_sm_AssistDataStatus->q_WishListMask & C_GPS_ASSIS_UTC)
            {
              tm_core_info.session_info.utc = 1;
            }
            tm_core_info.session_info.health_svmask = p_sm_AssistDataStatus->q_HealthHaveList;

            tm_core_info.session_info.glo_eph_svmask = p_sm_AssistDataStatus->z_GloAssistData.q_EphHaveList;
            tm_core_info.session_info.glo_alm_svmask = p_sm_AssistDataStatus->z_GloAssistData.q_AlmHaveList;

            tm_core_info.session_info.bds_eph_svmask = p_sm_AssistDataStatus->z_BdsAssistData.t_EphHaveList;
            tm_core_info.session_info.bds_alm_svmask = p_sm_AssistDataStatus->z_BdsAssistData.t_AlmHaveList;

            tm_core_info.session_info.qzss_eph_svmask = p_sm_AssistDataStatus->z_QzssAssistData.q_EphHaveList;
            tm_core_info.session_info.qzss_alm_svmask = p_sm_AssistDataStatus->z_QzssAssistData.q_AlmHaveList;
            tm_core_info.session_info.gal_eph_svmask = p_sm_AssistDataStatus->z_GalAssistData.t_EphHaveList;
            tm_core_info.session_info.gal_alm_svmask = p_sm_AssistDataStatus->z_GalAssistData.t_AlmHaveList;

            /* find out if GPS almanac assistance is needed */            
            if ( p_sm_AssistDataStatus->q_WishListMask & C_GPS_ASSIS_ALM )
            {
              /* when PE sets this flag, it means there is at least one SV having "need_alm"
                 status. find out how many SVs have valid almanac */
              u_sv_cnt = tm_util_num_bits_set_in_long(p_sm_AssistDataStatus->q_AlmHaveList);

              if ( u_sv_cnt >= C_COMPLETE_CONSTALLATION_ALM_THRESHOLD )
              {
                /* enough alm., consider having a complete constallation */
                p_sm_AssistDataStatus->q_WishListMask &= ~C_GPS_ASSIS_ALM;
                MSG_MED("LM's alm. req suppressed by TM-core: %u", u_sv_cnt, 0, 0 );
              }
              else
              {
                MSG_MED("TmCore passing alm. req. to protocol", 0, 0, 0);
              }

            }

            /* find out if GLONASS almanac assistance is needed */            
            if ( p_sm_AssistDataStatus->z_GloAssistData.q_WishListMask & C_GPS_ASSIS_ALM )
            {
              /* when PE sets this flag, it means there is at least one SV having "need_alm"
                 status. find out how many SVs have valid almanac */
              u_sv_cnt = tm_util_num_bits_set_in_long(p_sm_AssistDataStatus->z_GloAssistData.q_AlmHaveList);

              if ( u_sv_cnt >= C_COMPLETE_CONSTALLATION_GLO_ALM_THRESHOLD )
              {
                /* enough alm., consider having a complete constallation */
                p_sm_AssistDataStatus->z_GloAssistData.q_WishListMask &= ~C_GPS_ASSIS_ALM;
                MSG_MED("LM's GLO alm. req suppressed by TM-core: %u", u_sv_cnt, 0, 0 ); 
              }
              else
              {
                MSG_MED("TmCore passing GLO alm. req. to protocol", 0, 0, 0);
              }

            }

            // RH TODO: add code here to support BDS Alm/Eph etc
            // JV TODO: add code here to support QZSS Alm/Eph etc (future)

            /* TBD set ephemeris/almanac needed flags here for Glonass also */
            /* find out if ephemeris assistance is needed */
            /* when PE sets this flag, it means there is at least one SV having "need_eph"  status */
            if ( p_sm_AssistDataStatus->q_WishListMask & C_GPS_ASSIS_NAVMODL ||
                tm_core_info.session_info.force_eph_download == TRUE )
            {
              uint8 u_min_eph_have_threshold = C_MIN_EPH_HAVE_THRESHOLD;
              uint8 u_max_eph_need_threshold = C_MAX_EPH_NEED_THRESHOLD;
              uint8 u_num_eph_have_known_in_view = 0; /* number of SVs known in view needing eph. */
              uint8 u_num_eph_need_known_in_view = 0; /* number of SVs know in view having eph. */
              uint8 u_test_decision = FALSE;

              /* count number of known in-view SVs */
              u_sv_cnt = 0;
              for ( u_i=0; u_i<32; u_i++)
              {
                if ( p_sm_AssistDataStatus->q_VisibleSVList & 1<<u_i )
                {
                  u_sv_cnt++;
                }
              }

              if ( u_sv_cnt < C_MIN_EPH_HAVE_THRESHOLD )
              {
                /* this would cause eph. request if any of the in-view SV needs eph. */
                u_min_eph_have_threshold = u_sv_cnt;
                u_max_eph_need_threshold = 0;
              }

              /* The "need list" and "have list" are not gated with any elevation information,
                 use the visibility information (known visible) to remove any not-known-to-be-in-view SVs
                 from these lists. count number of known in-view need-eph SVs */
              u_num_eph_need_known_in_view = tm_util_num_bits_set_in_long(p_sm_AssistDataStatus->q_EphNeedList &
                                                                          p_sm_AssistDataStatus->q_VisibleSVList);
               MSG_MED("Force eph download flag is %d", tm_core_info.session_info.force_eph_download, 0, 0);

              /* count number of known in-view have-eph SVs */
              u_num_eph_have_known_in_view = tm_util_num_bits_set_in_long(p_sm_AssistDataStatus->q_EphHaveList &
                                                                          p_sm_AssistDataStatus->q_VisibleSVList);
              tm_core_info.gps_state_info.visible_svmask = u_num_eph_have_known_in_view;
              MSG_HIGH("Visible SVs in view %d", u_num_eph_have_known_in_view, 0,0);
              /* decide if request of eph. is to be forwarded to the protocol module */
              if ( ( ( u_num_eph_need_known_in_view >= u_max_eph_need_threshold ) &&
                     ( u_num_eph_have_known_in_view < u_min_eph_have_threshold )
                   ) ||
                   ( u_sv_cnt < C_MIN_SV_IN_VIEW_THRESHOLD )
                 )
              {
 
                MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"GPS Eph. req to protocol-sub: Nd=%x,HV=%x,VB=%x", 
                        p_sm_AssistDataStatus->q_EphNeedList,
                        p_sm_AssistDataStatus->q_EphHaveList,
                        p_sm_AssistDataStatus->q_VisibleSVList);

                u_test_decision = TRUE;

                if (tm_core_info.session_info.force_eph_download == FALSE)
                {
                  tm_core_info.session_info.force_eph_download = TRUE;
                  /* save the eph need request from PE because when then protocol asks again, this will
                        be not be set again */
                  tm_core_info.session_info.eph_need_sv_mask = p_sm_AssistDataStatus->q_EphNeedList;
                  MSG_MED("Force-GPS-eph_dld set to TRUE", 0, 0, 0);  
                }
                else
                {
                  /* if force eph flag was set, make sure we copy the request mask that was saved above. also
                  the wish list needs to be updated with the nav model request */
                  MSG_MED("Force-GPS-eph_dld TRUE, ignore suppress, NeedList", tm_core_info.session_info.eph_need_sv_mask, 0, 0);
                  p_sm_AssistDataStatus->q_WishListMask |= C_GPS_ASSIS_NAVMODL;
                  p_sm_AssistDataStatus->q_EphNeedList =  tm_core_info.session_info.eph_need_sv_mask;
                }
              }
              else
              {
                if (tm_core_info.session_info.force_eph_download == FALSE)
                {
                  p_sm_AssistDataStatus->q_WishListMask &= ~C_GPS_ASSIS_NAVMODL;
                  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"LM's GPS eph. req suppressed: Nd=%x,HV=%x,VB=%x", 
                    p_sm_AssistDataStatus->q_EphNeedList,
                    p_sm_AssistDataStatus->q_EphHaveList,
                    p_sm_AssistDataStatus->q_VisibleSVList);
                  u_test_decision = FALSE;
                }
                else
                {
                  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED,"Force-GPS-eph-download TRUE, ignore suppression, overriding need list: %x", 
                          tm_core_info.session_info.eph_need_sv_mask);  
                  u_test_decision = TRUE;

                  /* use the eph need list from the previous report becuase the 2nd request from the protocol
                      will get a empty need list. we need to make sure protocol gets the right eph mask */
                   p_sm_AssistDataStatus->q_WishListMask |= C_GPS_ASSIS_NAVMODL;
                   p_sm_AssistDataStatus->q_EphNeedList =  tm_core_info.session_info.eph_need_sv_mask;
                }
              }

              MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Revised wishlist 0x%X (GPS)", p_sm_AssistDataStatus->q_WishListMask);

              {
                 uint8 i;
                 sm_log_eph_download_data logdata;
                 memset(&logdata, 0, sizeof(logdata));
                 logdata.u_TestDecision  = u_test_decision;
                 logdata.q_EphHaveList   = p_sm_AssistDataStatus->q_EphHaveList;
                 logdata.q_VisibleSVList = p_sm_AssistDataStatus->q_VisibleSVList;
                 logdata.q_SvNoExistMask = p_sm_AssistDataStatus->q_SvNoExistMask;
                 logdata.u_min_eph_have_threshold = u_min_eph_have_threshold;
                 logdata.u_max_eph_need_threshold = u_max_eph_need_threshold;
                 logdata.u_num_eph_have_known_in_view = u_num_eph_have_known_in_view;
                 logdata.u_num_eph_need_known_in_view = u_num_eph_need_known_in_view;
                 logdata.u_sv_cnt        = u_sv_cnt;
                 logdata.w_OldestToe     = p_sm_AssistDataStatus->w_Toe;
                 logdata.w_GpsWeek       = p_sm_AssistDataStatus->w_GpsWeek;
                 if ( p_sm_AssistDataStatus->u_EphInfoIncl )
                 {
                    for (i=0; i<N_SV; i++)
                    {
                       logdata.z_SvEphIode[i].w_Sv   = p_sm_AssistDataStatus->z_SvEphIode[i].w_Sv;
                       logdata.z_SvEphIode[i].w_Iode = p_sm_AssistDataStatus->z_SvEphIode[i].w_Iode;
                    }
                 }
                 sm_log_eph_download(&logdata);
                 /* log_pff_msg( THREAD_ID_SM_TM, TM_CORE_EPH_RANDOMIZATION_DEBUG_LOG, sizeof(logdata), (byte *)&logdata ); */
              }

            } /* close if-LM-indicate-at-least-one-SV-needing-eph */
            MSG_HIGH("health noex 0x%x, health have 0x%x, health bad 0x%x", p_sm_AssistDataStatus->q_HealthNoExList, p_sm_AssistDataStatus->q_HealthHaveList , p_sm_AssistDataStatus->q_HealthBadList);


            /* GLONASS eph. request decision */ 
            /* find out if ephemeris assistance is needed */
            /* when PE sets this flag, it means there is at least one SV having "need_eph"  status */
            if ( p_sm_AssistDataStatus->z_GloAssistData.q_WishListMask & C_GPS_ASSIS_NAVMODL ||
                tm_core_info.session_info.glo_force_eph_download == TRUE )
            {
              uint8 u_min_eph_have_threshold = C_MIN_GLO_EPH_HAVE_THRESHOLD;
              uint8 u_max_eph_need_threshold = C_MAX_GLO_EPH_NEED_THRESHOLD;
              uint8 u_num_eph_have_known_in_view = 0; /* number of SVs known in view needing eph. */
              uint8 u_num_eph_need_known_in_view = 0; /* number of SVs know in view having eph. */
              //uint8 u_test_decision = FALSE;  /* Removing set-but-not-used variable */

              /* count number of known in-view SVs */
              u_sv_cnt = 0;
              for ( u_i=0; u_i<32; u_i++)
              {
                if ( p_sm_AssistDataStatus->z_GloAssistData.q_VisibleSVList & 1<<u_i )
                {
                  u_sv_cnt++;
                }
              }

              if ( u_sv_cnt < C_MIN_GLO_EPH_HAVE_THRESHOLD )
              {
                /* this would cause eph. request if any of the in-view SV needs eph. */
                u_min_eph_have_threshold = u_sv_cnt;
                u_max_eph_need_threshold = 0;
              }

              /* The "need list" and "have list" are not gated with any elevation information,
                 use the visibility information (known visible) to remove any not-known-to-be-in-view SVs 
                 from these lists. count number of known in-view need-eph SVs */
              u_num_eph_need_known_in_view = tm_util_num_bits_set_in_long(p_sm_AssistDataStatus->z_GloAssistData.q_EphNeedList & 
                                                                          p_sm_AssistDataStatus->z_GloAssistData.q_VisibleSVList);
               MSG_MED("Forced-GLO-eph_dld flag is %d", tm_core_info.session_info.glo_force_eph_download, 0, 0);

              /* count number of known in-view have-eph SVs */
              u_num_eph_have_known_in_view = tm_util_num_bits_set_in_long(p_sm_AssistDataStatus->z_GloAssistData.q_EphHaveList & 
                                                                          p_sm_AssistDataStatus->z_GloAssistData.q_VisibleSVList);

              // tm_core_info.gps_state_info.visible_svmask = u_num_eph_have_known_in_view;  /* no counterpart for GLO yet */

              MSG_HIGH("Visible GLO SVs in view %d", u_num_eph_have_known_in_view, 0,0);
              /* decide if request of eph. is to be forwarded to the protocol module */
              if ( ( ( u_num_eph_need_known_in_view >= u_max_eph_need_threshold ) && 
                     ( u_num_eph_have_known_in_view < u_min_eph_have_threshold )
                   ) ||
                   ( u_sv_cnt < C_MIN_GLO_SV_IN_VIEW_THRESHOLD )
                 )
              {
                MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"GLO Eph. req to protocol-sub: Nd=%x,HV=%x,VB=%x", 
                        p_sm_AssistDataStatus->z_GloAssistData.q_EphNeedList, 
                        p_sm_AssistDataStatus->z_GloAssistData.q_EphHaveList, 
                        p_sm_AssistDataStatus->z_GloAssistData.q_VisibleSVList);
                
                //u_test_decision = TRUE;  /* Removing set-but-not-used variable */
                
                if (tm_core_info.session_info.glo_force_eph_download == FALSE)
                {
                  tm_core_info.session_info.glo_force_eph_download = TRUE;
                  /* save the eph need request from PE because when then protocol asks again, this will
                        be not be set again */
                  tm_core_info.session_info.glo_eph_need_sv_mask = p_sm_AssistDataStatus->z_GloAssistData.q_EphNeedList;
                  MSG_MED("Forced-GLO-eph_dld set to TRUE", 0, 0, 0);  
                }
                else
                {
                  /* if force eph flag was set, make sure we copy the request mask that was saved above. also
                  the wish list needs to be updated with the nav model request */
                  MSG_MED("Force-GLO-eph_dld TRUE, ignore suppress, NeedList", tm_core_info.session_info.glo_eph_need_sv_mask, 0, 0);
                  p_sm_AssistDataStatus->z_GloAssistData.q_WishListMask |= C_GPS_ASSIS_NAVMODL;  
                  p_sm_AssistDataStatus->z_GloAssistData.q_EphNeedList =  tm_core_info.session_info.glo_eph_need_sv_mask;
                }
              }
              else
              {
                if (tm_core_info.session_info.glo_force_eph_download == FALSE)
                {
                  p_sm_AssistDataStatus->z_GloAssistData.q_WishListMask &= ~C_GPS_ASSIS_NAVMODL;  
                  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"LM's GLO eph. req suppressed: Nd=%x,HV=%x,VB=%x", 
                    p_sm_AssistDataStatus->z_GloAssistData.q_EphNeedList, 
                    p_sm_AssistDataStatus->z_GloAssistData.q_EphHaveList, 
                    p_sm_AssistDataStatus->z_GloAssistData.q_VisibleSVList);  
                  //u_test_decision = FALSE;  /* Removing set-but-not-used variable */
                }
                else
                {
                  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED,"Force-GLO-eph_dld TRUE, ignore suppress, NeedList=%x", 
                          tm_core_info.session_info.glo_eph_need_sv_mask);  
                  //u_test_decision = TRUE;  /* Removing set-but-not-used variable */

                  /* use the eph need list from the previous report becuase the 2nd request from the protocol
                      will get a empty need list. we need to make sure protocol gets the right eph mask */
                  p_sm_AssistDataStatus->z_GloAssistData.q_WishListMask |= C_GPS_ASSIS_NAVMODL;  
                  p_sm_AssistDataStatus->z_GloAssistData.q_EphNeedList =  tm_core_info.session_info.glo_eph_need_sv_mask;
                }  
              }

              MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Revised wishlist 0x%X (GLO)",
                    p_sm_AssistDataStatus->z_GloAssistData.q_WishListMask);

              /* TBD: may need to handle sm_log_eph_download() for GLONASS */

            } /* close if-LM-indicate-at-least-one-SV-needing-GLO-eph */

            /* TBD: note current sm_api does not have q_HealthNoExList member */
            MSG_HIGH("GLO: health-have 0x%x, health-ad 0x%x", 
                     p_sm_AssistDataStatus->z_GloAssistData.q_HealthHaveList, 
                     p_sm_AssistDataStatus->z_GloAssistData.q_HealthBadList,
                     0);
          
            /* end GLONASS eph. request decision*/

            if(p_sm_AssistDataStatus->z_TimeUnc.u_Valid)
            {
              tm_core_info.gps_state_info.flags |= SM_GPS_STATE_TUNC_VALID;
              tm_core_info.gps_state_info.time_unc_us = p_sm_AssistDataStatus->z_TimeUnc.f_TimeUnc;
              tm_core_info.session_info.sess_info_valid_flags = TM_CORE_SESS_INFO_CACHED_INFO_STATE_VALID;
            }
            if (  p_sm_AssistDataStatus->q_WishListMask & C_GPS_ASSIS_REFTIME )
            {
              MSG_MED("PE forces TM to request refT", 0, 0, 0);
            }
            else if ( ( p_sm_AssistDataStatus->z_TimeUnc.u_Valid == FALSE ) ||
                      ( p_sm_AssistDataStatus->z_TimeUnc.f_TimeUnc > 10000 )
                    )
            {
              /* this will force the standalone module to flag as "need dload" if UMTS; it will
                 then be checked in UMTS using time unc. threhold if validity is not false */
              p_sm_AssistDataStatus->q_WishListMask |= C_GPS_ASSIS_REFTIME;
              MSG_MED("PE info. shows time invalid or unc > 10s", 0, 0, 0);
            }
            if(p_sm_AssistDataStatus->z_PosUnc.u_Valid)
            {
              if(p_sm_AssistDataStatus->z_PosUnc.f_PosUnc >TM_MAX_PUNC_VALUE_FOR_POS_INJ_REQ )
              {
                pos_inj_req = TRUE;
              }
              tm_core_info.gps_state_info.flags |= SM_GPS_STATE_PUNC_VALID;
            }
            else
            {
              /* if no position, ask for position injection */
              pos_inj_req = TRUE;
              tm_core_info.gps_state_info.flags &= ~SM_GPS_STATE_PUNC_VALID;
            }
            MSG_MED("Posinj req = %d, punc flag = %d, cached flag = %d", pos_inj_req, tm_core_info.gps_state_info.flags, tm_core_info.gps_state_info.flags);
            /* if MSB and pos_inj_req is true, save the data so if the netwrok assistance failes, we
               can ask the app to inject a position */
            if((pos_inj_req == TRUE) &&
                (tm_core_info.session_info.op_mode == TM_SESS_OPERATION_MODE_MSB) &&
                (!(tm_core_info.session_info.sess_info_valid_flags & TM_CORE_SESS_INFO_CACHED_INFO_STATE_VALID)))
            {
              MSG_MED("Setting cached info flag to valid", 0,0,0);
              tm_core_info.session_info.sess_info_valid_flags |= TM_CORE_SESS_INFO_CACHED_INFO_STATE_VALID;
            }
            else if ( (TRUE == pos_inj_req) &&
                      (TM_SESS_OPERATION_MODE_STANDALONE == tm_core_info.session_info.op_mode) )
            {
              pdsm_ext_status_info_s_type   *pz_pd_ext_status_info_pos_inj;

              pz_pd_ext_status_info_pos_inj = (pdsm_ext_status_info_s_type *)os_MemAlloc(sizeof(pdsm_ext_status_info_s_type), OS_MEM_SCOPE_TASK);
              if (NULL == pz_pd_ext_status_info_pos_inj)
              {
                MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Cannot allocate memory for pdsm_ext_status_info_s_type in tm_core_lm_sess_info_handler");
              } 
              else
              {
                memset(pz_pd_ext_status_info_pos_inj, 0, sizeof(*pz_pd_ext_status_info_pos_inj));
                pz_pd_ext_status_info_pos_inj->ext_status_type = PDSM_EXT_STATUS_POS_INJECT_REQUEST;
                pz_pd_ext_status_info_pos_inj->client_id =  tm_core_info.session_info.pd_param.client_id;

                MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "REQUEST POSITION INJECTION"); 
                tm_pdapi_ext_status_event_callback(PDSM_EXT_STATUS_POS_INJECT_REQUEST,
                   pz_pd_ext_status_info_pos_inj);
                tm_core_info.session_info.pos_inj_requested =  TRUE;
                (void)os_MemFree((void **)&pz_pd_ext_status_info_pos_inj);
              }
            }
            if (tm_core_info.session_info.op_requested & TM_OP_REQ_AIDING_DATA_STATUS)
            {
              tm_core_info.session_info.op_requested &= ~TM_OP_REQ_AIDING_DATA_STATUS;

              MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Wishlist to protocol-module: 0x%X (GPS) 0x%X (GLONASS)",
                    sess_info_param_ptr->aiding_data_status.z_assist_data_status.q_WishListMask,
                    sess_info_param_ptr->aiding_data_status.z_assist_data_status.z_GloAssistData.q_WishListMask);

              retVal = tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp(tm_core_info.session_info.active_sess_handle,
                                                                                    PRTL_SESS_INFO_STATUS,
                                                                                    sess_info_param_ptr);
              if (retVal == FALSE)
              {
                MSG_HIGH("Protocol rejects Aiding Data Status", 0, 0, 0);
                tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
              }
            }
          }
          else
          {
            MSG_ERROR("No protocol callback installed 11", 0, 0, 0);                      
          }
        }
        if ( (tm_core_info.session_info.q_ReqClientMask & (TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE))) ||
             (tm_core_info.session_info.q_ReqClientMask & (TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_ON_DEMAND)))   ||
             (tm_core_info.session_info.q_ReqClientMask & (TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_GET_PUNC_TUNC_FOR_WIFI)))
           )
        {
          uint8 u_num_eph_have_known_in_view = 0; /* number of SVs know in view having eph. */
          sm_OnDemandPosPuncTuncStruct on_demand_info;
          memset( &on_demand_info, 0, sizeof(on_demand_info));
          memscpy((void *)&(sess_info_param_ptr->aiding_data_status),
              sizeof(sess_info_param_ptr->aiding_data_status),
              (void *)&(sess_info_ptr->z_info.z_wishlist),
              sizeof(sess_info_ptr->z_info.z_wishlist));

          p_sm_AssistDataStatus = &(sess_info_param_ptr->aiding_data_status.z_assist_data_status);
          MSG_HIGH("health noex 0x%x, health have 0x%x, health bad 0x%x", p_sm_AssistDataStatus->q_HealthNoExList, p_sm_AssistDataStatus->q_HealthHaveList , p_sm_AssistDataStatus->q_HealthBadList);
          tm_core_info.gps_state_info.alm_svmask = p_sm_AssistDataStatus->q_AlmHaveList;
          tm_core_info.gps_state_info.eph_svmask = p_sm_AssistDataStatus->q_EphHaveList;
          tm_core_info.gps_state_info.health_svmask = p_sm_AssistDataStatus->q_HealthHaveList;
          tm_core_info.gps_state_info.health_bad_svmask = p_sm_AssistDataStatus->q_HealthBadList;
          tm_core_info.gps_state_info.health_noex_svmask = p_sm_AssistDataStatus->q_HealthNoExList;
          tm_core_info.gps_state_info.health_unk_svmask = p_sm_AssistDataStatus->q_HealthNeedList;
          tm_core_info.gps_state_info.iono_valid = 1;

          q_long_word2 = p_sm_AssistDataStatus->q_EphHaveList & p_sm_AssistDataStatus->q_VisibleSVList;

          tm_core_info.gps_state_info.flags |= PDSM_PD_EXT_GPS_STATE_EPH_VALID;
          tm_core_info.gps_state_info.flags |= PDSM_PD_EXT_GPS_STATE_ALM_VALID;
          tm_core_info.gps_state_info.flags |= PDSM_PD_EXT_GPS_STATE_IONO_VALID;
          tm_core_info.gps_state_info.flags |= PDSM_PD_EXT_GPS_STATE_VISIBLE_SV_VALID;
          tm_core_info.gps_state_info.flags |= PDSM_PD_EXT_GPS_STATE_HEALTH_VALID;

          u_num_eph_have_known_in_view = 0;
          for ( u_i=0; u_i<32; u_i++)
          {
            if ( q_long_word2 & 1<<u_i )
            {
              u_num_eph_have_known_in_view++;
            }
          }
          MSG_HIGH("Visible SVs with eph 0x%x, eph list 0x%x, visible sv list 0x%x", u_num_eph_have_known_in_view,
              p_sm_AssistDataStatus->q_EphHaveList,
              p_sm_AssistDataStatus->q_AlmHaveList );
          tm_core_info.gps_state_info.visible_svmask = p_sm_AssistDataStatus->q_VisibleSVList ;
          if(p_sm_AssistDataStatus->q_WishListMask & C_GPS_ASSIS_IONO)
          {
            tm_core_info.gps_state_info.iono_valid = 0;
          }

          // todo. need to hook up with the pv mgr
          if (tm_core_info.session_info.q_ReqClientMask & (TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE)))
          {
            tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE].w_TmRcvdDataMask |= TM_RCVD_ASSIST_DATA_FROM_PE;
            if (tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE].w_TmRcvdDataMask == TM_RCVD_ALL)
            {
              pz_pd_ext_status_info->client_id = tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE].q_ClientId;
              tm_core_xlate_gps_state_to_ext_status(&tm_core_info.gps_state_info, &(pz_pd_ext_status_info->ext_status_info.ext_gps_state_type));
              tm_pdapi_ext_status_event_callback(PDSM_EXT_STATUS_GPS_STATE_INFO, pz_pd_ext_status_info);
              tm_core_info.session_info.q_ReqClientMask &= ~(TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE));
              tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE].w_TmRcvdDataMask = 0;
            }
          }
          if (tm_core_info.session_info.q_ReqClientMask & (TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_GET_PUNC_TUNC_FOR_WIFI)))
          {
            tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_PUNC_TUNC_FOR_WIFI].w_TmRcvdDataMask |= TM_RCVD_ASSIST_DATA_FROM_PE;
            if (tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_PUNC_TUNC_FOR_WIFI].w_TmRcvdDataMask == TM_RCVD_ALL)
            {
              sysd_cntrl_func_packet_type sysd_cntrl_payload;
              tm_SysdPosPuncTunc SysdPosPuncTunc;
              SysdPosPuncTunc.f_Tunc = tm_core_info.gps_state_info.time_unc_us;
              SysdPosPuncTunc.f_Punc = tm_core_info.gps_state_info.pos_unc;
              if ((tm_core_info.gps_state_info.flags & PDSM_PD_EXT_GPS_STATE_POS_VALID ) &&
                  (tm_core_info.gps_state_info.flags & PDSM_PD_EXT_GPS_STATE_TUNC_VALID))
              {
                SysdPosPuncTunc.b_Valid = TRUE;
              }
              else
              {
                SysdPosPuncTunc.b_Valid = FALSE ;
              }

              sysd_cntrl_payload.cmd.p_SysdPosPuncTunc = &SysdPosPuncTunc;
              sysd_cntrl_payload.event = SYSD_CNTRL_EVT_GPS_STATE_INFO;
              sysd_cntrl_payload.req_type = SYSD_CNTRL_POS_REQ_TYPE_PDAPI;

              tm_sysd_controller_handler(&sysd_cntrl_payload);
              tm_core_info.session_info.q_ReqClientMask &= ~(TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_GET_PUNC_TUNC_FOR_WIFI));
              tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_PUNC_TUNC_FOR_WIFI].w_TmRcvdDataMask = 0;
            }
          }
          if (tm_core_info.session_info.q_ReqClientMask & (TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_ON_DEMAND)))
          {
            tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_ON_DEMAND].w_TmRcvdDataMask |= TM_RCVD_ASSIST_DATA_FROM_PE;
            if (tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_ON_DEMAND].w_TmRcvdDataMask == TM_RCVD_ALL)
            {
              tm_core_xlate_gps_state_to_on_demand_info(&tm_core_info.gps_state_info, &on_demand_info);
              tm_core_info.session_info.q_ReqClientMask &= ~(TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_ON_DEMAND));
              tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_ON_DEMAND].w_TmRcvdDataMask = 0;
            }
          }
        }
        break;

      case TM_INFO_PD_API_EVENT:
      {
        tm_pdapi_pd_event_s_type *pz_pd_event_info;

        pz_pd_event_info = (tm_pdapi_pd_event_s_type *)os_MemAlloc(sizeof(tm_pdapi_pd_event_s_type), OS_MEM_SCOPE_TASK);
        if (NULL == pz_pd_event_info)
        {
          MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Cannot allocate memory for tm_pdapi_pd_event_s_type in tm_core_lm_sess_info_handler");
        } 
        else
        {
          memset( pz_pd_event_info, 0, sizeof( *pz_pd_event_info ) );
        }
        if(sess_info_ptr->z_info.e_pdapi_event == TM_PD_EVENT_GPS_BEGIN)
        {
          MSG_MED("TM: Received event TM_PD_EVENT_GPS_BEGIN from LM", 0, 0, 0);
          /*update the cached MGP receiver sate*/
          tm_core_info.e_cached_rcvr_state = C_GPS_RCVR_STATE_ON;

          if (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_ON_DEMAND)
          {
            prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_STANDALONE);
            event_payload.on_demand_event_payload.event_id = PRTL_ON_DEMAND_RCVR_STATE;
            event_payload.on_demand_event_payload.event_payload.rcvr_state = TRUE;
            if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
            {
              if(tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL)
              {
                (void) tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(
                                                                           PRTL_EVENT_TYPE_ON_DEMAND_POS,
                                                                           &event_payload);
              }
            }

          } 
          if (NULL != pz_pd_event_info)
          {
            pz_pd_event_info->e_pd_event = PDSM_PD_EVENT_GPS_BEGIN;
            pz_pd_event_info->pd_event_data.pd_info_type.client_id = tm_core_info.session_info.pd_param.client_id;

            tm_pdapi_pd_event_callback(pz_pd_event_info, TM_PDAPI_SESS_TYPE_NONE);
          }
        }
        else if(sess_info_ptr->z_info.e_pdapi_event == TM_PD_EVENT_GPS_DONE)
        {
          uint8 gps_utc_offset = 0;

          MSG_MED("TM: Received event TM_PD_EVENT_GPS_DONE from LM", 0, 0, 0);
          /*update the cached MGP receiver state*/
          tm_core_info.e_cached_rcvr_state = C_GPS_RCVR_STATE_OFF;

          if (tm_core_info.config_info.on_demand_lpm == TRUE)
          {
            prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_STANDALONE);
            event_payload.on_demand_event_payload.event_id = PRTL_ON_DEMAND_RCVR_STATE;
            event_payload.on_demand_event_payload.event_payload.rcvr_state = FALSE;
            if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
            {
              if(tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL)
              {
                (void) tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(
                                                                           PRTL_EVENT_TYPE_ON_DEMAND_POS,
                                                                           &event_payload);
              }
            }

          }
          if (NULL != pz_pd_event_info)
          {
            pz_pd_event_info->e_pd_event = PDSM_PD_EVENT_GPS_DONE;
            pz_pd_event_info->pd_event_data.pd_info_type.client_id = tm_core_info.session_info.pd_param.client_id;
  
            tm_pdapi_pd_event_callback(pz_pd_event_info, TM_PDAPI_SESS_TYPE_NONE);
          }
          /* When GPS is finished, check if the gpsUtcOffset is different than what
             is stored in the NV item.  If different, update the NV item */
          gps_utc_offset = z_cached_pos_info.pd_info.pd_info.pd_data.gpsUtcOffset;
          if ((gps_utc_offset != 0) && (TRUE == b_nv_gps_utc_offset_changed))
          {
            MSG_MED("TM: Set new utc time offset value: %d", gps_utc_offset, 0, 0);
            nv_write_buffer.cgps_utc_gps_time_offset = gps_utc_offset;
            (void)tm_core_nv_write( NV_CGPS_UTC_GPS_TIME_OFFSET_I,
                                    (void*)&nv_write_buffer );
            tm_core_info.config_info.nv_gps_utc_offset = gps_utc_offset;
            b_nv_gps_utc_offset_changed = FALSE;
          }
        }
        else if(sess_info_ptr->z_info.e_pdapi_event == TM_PD_EVENT_GPS_IDLE)
        {
          MSG_MED("TM: Received event TM_PD_EVENT_GPS_IDLE from LM", 0, 0, 0);
          /*update the cached MGP receiver sate. There is no corresponding PDAPI event
           to be generated*/
          tm_core_info.e_cached_rcvr_state = C_GPS_RCVR_STATE_IDLE;
        }
        if (NULL != pz_pd_event_info)
        {
          (void)os_MemFree((void **)&pz_pd_event_info);
        }
      }
      break;

      case TM_NMEA_DATA:
      {
        if(TM_NMEA_DEBUG == sess_info_ptr->z_info.z_nmea_data.e_sentence_type)
        {
          tm_nmea_debug(&(sess_info_ptr->z_info.z_nmea_data));
        }
      }

      break;

      case TM_INFO_POS_ESTIMATE:
        {
          boolean u_valid_fix = TRUE;
          if (!sess_info_ptr->z_info.z_pos_estimate.z_GpsFixRpt.z_NavPos.z_PosFlags.b_IsValid)
          {
            if ((sess_info_ptr->z_info.z_pos_estimate.z_GpsFixRpt.z_NavPos.w_GpsWeek == C_GPS_WEEK_UNKNOWN) &&
                (sess_info_ptr->z_info.z_pos_estimate.z_GpsFixRpt.z_NavPos.q_GpsTimeMs == 0))
            {
              u_valid_fix = FALSE;
            }
          }
          if ((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
          {
            MSG_MED("Rcvd Pos Estimate result from MGP, fix valid %d", u_valid_fix,0,0);
            if (u_valid_fix)
            {
              /* Set reason to general error for now, if return value */
              if (tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp)
              {
                memscpy((void *)&(sess_info_param_ptr->ref_lr_data),
                       sizeof(sess_info_param_ptr->ref_lr_data),
                       (void *)&(sess_info_ptr->z_info.z_pos_estimate),
                       sizeof(sess_info_ptr->z_info.z_pos_estimate));
                retVal = tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp(tm_core_info.session_info.active_sess_handle,
                                                                                      PRTL_SESS_INFO_REF_LR,
                                                                                      sess_info_param_ptr);
                if (retVal == FALSE)
                {
                  MSG_ERROR("Protocol rejects Ref Loc data", 0, 0, 0);
                  tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
                }
              }

              else
              {
                MSG_ERROR("No protocol callback installed 12", 0, 0, 0);                      
              }
            }
          }
          else if (tm_core_info.session_info.q_ReqClientMask != SM_GPS_STATE_INFO_REQ_TYPE_NONE)
          {
            sm_OnDemandPosPuncTuncStruct on_demand_info;

            memset( &on_demand_info, 0, sizeof(on_demand_info));
            memscpy((void *)&(sess_info_param_ptr->ref_lr_data),
                   sizeof(sess_info_param_ptr->ref_lr_data),
                   (void *)&(sess_info_ptr->z_info.z_pos_estimate),
                   sizeof(sess_info_ptr->z_info.z_pos_estimate));
            if (u_valid_fix)
            {
              memscpy((void *)&(sess_info_param_ptr->ref_lr_data),
                       sizeof(sess_info_param_ptr->ref_lr_data),
                       (void *)&(sess_info_ptr->z_info.z_pos_estimate),
                       sizeof(sess_info_ptr->z_info.z_pos_estimate));
              tm_core_info.gps_state_info.lat = (int32)(C_2_32_OVER_PI * sess_info_param_ptr->ref_lr_data.z_NavPos.d_PosLla[0]);
              tm_core_info.gps_state_info.lon = (int32)(C_2_31_OVER_PI * sess_info_param_ptr->ref_lr_data.z_NavPos.d_PosLla[1]);
              tm_core_info.gps_state_info.flags |= PDSM_PD_EXT_GPS_STATE_POS_VALID;
              tm_core_info.gps_state_info.flags |= PDSM_PD_EXT_GPS_STATE_ALT_VALID;
              tm_core_info.gps_state_info.altitude = (int32)(sess_info_param_ptr->ref_lr_data.z_NavPos.d_PosLla[2] * 10.0 + 0.5);
              tm_core_info.gps_state_info.loc_unc_vertical = (uint16)sess_info_param_ptr->ref_lr_data.z_NavPos.f_FilteredAltUnc;
              tm_core_info.gps_state_info.pos_unc = (uint32) sqrt((sess_info_param_ptr->ref_lr_data.z_NavPos.f_ErrorEllipse[1] *
                                                                   sess_info_param_ptr->ref_lr_data.z_NavPos.f_ErrorEllipse[1]) +
                                                                  (sess_info_param_ptr->ref_lr_data.z_NavPos.f_ErrorEllipse[2] *
                                                                   sess_info_param_ptr->ref_lr_data.z_NavPos.f_ErrorEllipse[2]));
              if (!sess_info_param_ptr->ref_lr_data.z_NavPos.z_PosFlags.b_Is3D)
              {
                tm_core_info.gps_state_info.flags &= ~PDSM_PD_EXT_GPS_STATE_ALT_VALID;
              }
            }
            else
            {
              tm_core_info.gps_state_info.flags &= ~PDSM_PD_EXT_GPS_STATE_POS_VALID;
              tm_core_info.gps_state_info.flags &= ~PDSM_PD_EXT_GPS_STATE_ALT_VALID;
            }

            if (tm_core_info.session_info.q_ReqClientMask & (TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE)))
            {
              tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE].w_TmRcvdDataMask |= TM_RCVD_POS_FROM_PE;
              if (tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE].w_TmRcvdDataMask == TM_RCVD_ALL)
              {
                pz_pd_ext_status_info->client_id = tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE].q_ClientId;
                tm_core_xlate_gps_state_to_ext_status(&tm_core_info.gps_state_info, &(pz_pd_ext_status_info->ext_status_info.ext_gps_state_type));
                tm_pdapi_ext_status_event_callback(PDSM_EXT_STATUS_GPS_STATE_INFO, pz_pd_ext_status_info);
                tm_core_info.session_info.q_ReqClientMask &= ~(TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE));
                tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE].w_TmRcvdDataMask = 0;
              }
            }
            if (tm_core_info.session_info.q_ReqClientMask & (TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_ON_DEMAND)))
              {
              tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_ON_DEMAND].w_TmRcvdDataMask |= TM_RCVD_POS_FROM_PE;
              if (tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_ON_DEMAND].w_TmRcvdDataMask == TM_RCVD_ALL)
              {
                tm_core_xlate_gps_state_to_on_demand_info(&tm_core_info.gps_state_info, &on_demand_info);
                tm_core_info.session_info.q_ReqClientMask &= ~(TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_ON_DEMAND));
                tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_ON_DEMAND].w_TmRcvdDataMask = 0;
              }
              }
            if (tm_core_info.session_info.q_ReqClientMask & (TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_GET_PUNC_TUNC_FOR_WIFI)))
            {
              tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_PUNC_TUNC_FOR_WIFI].w_TmRcvdDataMask |= TM_RCVD_POS_FROM_PE;
              if (tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_PUNC_TUNC_FOR_WIFI].w_TmRcvdDataMask == TM_RCVD_ALL)
              {
                sysd_cntrl_func_packet_type sysd_cntrl_payload;
                tm_SysdPosPuncTunc SysdPosPuncTunc;
                SysdPosPuncTunc.f_Punc = tm_core_info.gps_state_info.pos_unc;
                SysdPosPuncTunc.f_Tunc = tm_core_info.gps_state_info.time_unc_us;
                if ((tm_core_info.gps_state_info.flags & PDSM_PD_EXT_GPS_STATE_POS_VALID ) &&
                    (tm_core_info.gps_state_info.flags & PDSM_PD_EXT_GPS_STATE_TUNC_VALID))
                {
                  SysdPosPuncTunc.b_Valid = TRUE;
                }
                else
                {
                  SysdPosPuncTunc.b_Valid = FALSE ;
                }

                sysd_cntrl_payload.cmd.p_SysdPosPuncTunc = &SysdPosPuncTunc;
                sysd_cntrl_payload.event = SYSD_CNTRL_EVT_GPS_STATE_INFO;
                sysd_cntrl_payload.req_type = SYSD_CNTRL_POS_REQ_TYPE_PDAPI;

                tm_sysd_controller_handler(&sysd_cntrl_payload);
                tm_core_info.session_info.q_ReqClientMask &= ~(TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_GET_PUNC_TUNC_FOR_WIFI));
                tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_PUNC_TUNC_FOR_WIFI].w_TmRcvdDataMask = 0;
              }
            }
          }
          else
          {
            MSG_ERROR("Invalid prtl index, can not report aiding data status", 0, 0, 0);
          }
        }
        break;

      case TM_RF_INFO:

        if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
        {
          /* Set reason to general error for now, if return value */
          if(tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp)
          {

            memscpy((void *)&(sess_info_param_ptr->rf_info_data),
                   sizeof(sess_info_param_ptr->rf_info_data),
                   (void *)&(sess_info_ptr->z_info.z_rf_info.z_rf_info_struct),
                    sizeof(sess_info_ptr->z_info.z_rf_info.z_rf_info_struct));

            retVal = tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp(tm_core_info.session_info.active_sess_handle,
                                                                                  PRTL_SESS_INFO_RF_INFO,
                                                                                  sess_info_param_ptr);
            if(retVal == FALSE)
            {
              MSG_ERROR("Protocol rejects RF info data", 0, 0, 0);
              tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
            }
          }
          else
          {
            MSG_ERROR("No protocol callback installed 13", 0, 0, 0);                      
          }
        }
        else
        {
          MSG_ERROR("Invalid prtl index, can not report aiding data status", 0, 0, 0);
        }
      break;

      case TM_GPS_TIME:

        #ifdef FEATURE_CGPS_XTRA
          tm_xtra_gps_time_report_handler (
            &(sess_info_ptr->z_info.z_gps_time_info.z_gps_time_info_struct.GpsClock.z_Time)
          );
        #endif /* FEATURE_CGPS_XTRA */
 #if 0 /* disable this code for gen8 for now. it should never be exercised anyway */
        /* for ODP session, check if the clock unc is low */
        if (tm_core_info.session_info.op_mode == TM_SESS_OPERATION_MODE_ODP)
        {
          FLT f_punc, f_tunc, f_tunc_ms;
          tm_on_demand_get_punc_tunc(&f_punc, &f_tunc);

          MSG_HIGH("tm_core: ODP fix clock update. At start of fix TUNC = %d us, PUNC = %d m. Current TUNC = %d us",
            (uint32)f_tunc, (uint32)f_punc,
            (uint32)(sess_info_ptr->z_info.z_gps_time_info.z_gps_time_info_struct.GpsClock.z_Time.f_ClkTimeUncMs * 1000));
          if ((uint32)f_punc < C_GPS_ODP_MODE1_PUNC_UPPER_LIMIT)
          {
            /* clock only fix is required, check if the TUNC is low */
            if (sess_info_ptr->z_info.z_gps_time_info.z_gps_time_info_struct.source == GPS_TIME_CHECK_SOURCE_ODP_SESSION)
            {
              f_tunc_ms = sess_info_ptr->z_info.z_gps_time_info.z_gps_time_info_struct.GpsClock.z_Time.f_ClkTimeUncMs;
              f_tunc = f_tunc_ms * 1000.0; /* in us */
              if ((uint32)f_tunc <= C_GPS_ODP_MODE1_TUNC_LOWER_LIMIT)
              {
                MSG_HIGH("tm_core: ODP clock only fix successful %d us. End session", f_tunc, 0,0);
                if (tm_core_info.session_info.end_odp_session == TRUE)
                {
                  tm_core_abort_recover(TRUE, TM_STOP_REASON_COMPLETED); /*lint !e506 !e730 */
                }
                else
                {
                  tm_core_info.session_info.end_odp_session = TRUE ;
                }
              }
            }
          }
        }
#endif
        break;

    case TM_INFO_MGP_EVENT:
      {
        tm_info_mgp_event_s_type* sess_info_ptr = (tm_info_mgp_event_s_type*)lm_sess_info_data;

        switch (sess_info_ptr->z_mgp_event.e_EventType)
      {
      case SM_EVENT_PE_NEW_NAVDATA:
            MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"NEW EPH Decoded 0x%x", sess_info_ptr->z_mgp_event.q_EventData);
            tm_core_info.session_info.eph_svmask |= 1 << (sess_info_ptr->z_mgp_event.q_EventData - 1);
        break;

      case SM_EVENT_PE_NEW_ALMANAC:
            MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"NEW ALM Decoded %x", sess_info_ptr->z_mgp_event.q_EventData);
            tm_core_info.session_info.alm_svmask |= 1 << (sess_info_ptr->z_mgp_event.q_EventData - 1);
        break;

      case SM_EVENT_PE_NEW_IONO_UTC:
            if (sess_info_ptr->z_mgp_event.q_EventData > 0)
          {
              uint8 gps_utc_offset = 0;
        
              MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"NEW IONO/UTC Decoded %d", sess_info_ptr->z_mgp_event.q_EventData);

              gps_utc_offset = (uint8)(sess_info_ptr->z_mgp_event.q_EventData);
              if ((gps_utc_offset != 0) && (gps_utc_offset!=tm_core_info.config_info.nv_gps_utc_offset))
              {
              MSG_MED("TM: Set new utc time offset value: %d", gps_utc_offset, 0, 0);
              nv_write_buffer.cgps_utc_gps_time_offset = gps_utc_offset;
                (void)tm_core_nv_write( NV_CGPS_UTC_GPS_TIME_OFFSET_I, (void*)&nv_write_buffer );
            tm_core_info.session_info.trusted_utc_time = TRUE;
              tm_core_info.config_info.nv_gps_utc_offset = gps_utc_offset;
              }
          }
          break;

      default:
          break;
      }
      }
      break;

      case TM_INFO_OEMDRE_TD_CTRL:

        /* Set the OEM DRE Tunnel Detection Enablement control flag */
        if (sess_info_ptr->z_info.e_oemdre_td_ctrl == TM_OEMDRE_TD_CTRL_ON)
        {
          tm_core_info.config_info.oemdre_td_ctrl = TRUE;
        }
        else
        {
          tm_core_info.config_info.oemdre_td_ctrl = FALSE;
        }

        MSG_MED("OEMDRETD: NEW CTRL Flag decoded as %d", sess_info_ptr->z_info.e_oemdre_td_ctrl, 0, 0);
        break;

    }  /* switch(sess_info_ptr->e_info_type) */

    (void) pdsm_freebuf((char *)sess_info_param_ptr);
  }
  (void)os_MemFree((void **)&pz_pd_ext_status_info);
}

/*===========================================================================

FUNCTION tm_core_delete_inject_aiding_data

DESCRIPTION
  This function is called to delete or inject aiding data.

DEPENDENCIES

RETURN

SIDE EFFECTS

===========================================================================*/
void tm_core_delete_inject_aiding_data
(
  pdsm_delete_parms_type *delete_param_ptr,
  boolean force_rcvr_off,
  boolean delete_all,
  pdsm_pa_e_type delete_type
)
{
  lm_request_info_s_type lm_delete_req_info;
  lm_request_info_s_type lm_delete_req_info_glo;
  lm_request_info_s_type lm_delete_req_info_bds;
  lm_request_info_s_type lm_delete_req_info_gal;
  lm_request_info_s_type lm_delete_req_info_sbas;
  lm_info_s_type         *lm_inject_req_info_ptr;
  boolean b_delete_mgp_gps = FALSE;
  boolean b_delete_mgp_glo = FALSE;
  boolean b_delete_mgp_bds = FALSE;
  boolean b_delete_mgp_qzss = FALSE;
  boolean b_delete_mgp_gal = FALSE;
  boolean b_delete_mgp_sbas = FALSE;

  memset(&lm_delete_req_info, 0, sizeof(lm_request_info_s_type)); //for GPS & QZSS
  memset(&lm_delete_req_info_glo, 0, sizeof(lm_request_info_s_type));
  memset(&lm_delete_req_info_bds, 0, sizeof(lm_request_info_s_type));
  memset(&lm_delete_req_info_gal, 0, sizeof(lm_request_info_s_type));
  memset(&lm_delete_req_info_sbas, 0, sizeof(lm_request_info_s_type));
  if(delete_param_ptr == NULL)
  {
    MSG_ERROR("Null delete_param_ptr", 0, 0, 0);
    return;
  }

  if (delete_all)
  {
    MSG_HIGH("Deleting ALL aiding data", 0, 0, 0);

    lm_delete_req_info.e_req_type = LM_REQUEST_DELETE_MGP_INFO;
    lm_delete_req_info.z_request.z_delete_request.delete_all = TRUE;
    lm_delete_req_info.z_request.z_delete_request.force_rcvr_off = force_rcvr_off;

    /* Deletes GPS and Glonass data */
    lm_delete_req_info.z_request.z_delete_request.z_mgp_info.e_GnssType = MGP_GNSS_TYPE_ALL;

    tm_core_clear_cached_pos();
    (void)lm_request(&lm_delete_req_info);
    return;
  }

  if(delete_param_ptr->pdsm_delete_parms_flags & PA_AIDING_DATA_DELETE_MASK)
  {
    MSG_HIGH("Deleting aiding data", 0, 0, 0);

    lm_delete_req_info.e_req_type = LM_REQUEST_DELETE_MGP_INFO;
    lm_delete_req_info.z_request.z_delete_request.delete_all = FALSE;
    lm_delete_req_info.z_request.z_delete_request.force_rcvr_off = force_rcvr_off;

    lm_delete_req_info_glo.e_req_type = LM_REQUEST_DELETE_MGP_INFO;
    lm_delete_req_info_glo.z_request.z_delete_request.delete_all = FALSE;
    lm_delete_req_info_glo.z_request.z_delete_request.force_rcvr_off = force_rcvr_off;

    lm_delete_req_info_bds.e_req_type = LM_REQUEST_DELETE_MGP_INFO;
    lm_delete_req_info_bds.z_request.z_delete_request.delete_all = FALSE;
    lm_delete_req_info_bds.z_request.z_delete_request.force_rcvr_off = force_rcvr_off;

    lm_delete_req_info_gal.e_req_type = LM_REQUEST_DELETE_MGP_INFO;
    lm_delete_req_info_gal.z_request.z_delete_request.delete_all = FALSE;
    lm_delete_req_info_gal.z_request.z_delete_request.force_rcvr_off = force_rcvr_off;

    lm_delete_req_info_sbas.e_req_type = LM_REQUEST_DELETE_MGP_INFO;
    lm_delete_req_info_sbas.z_request.z_delete_request.delete_all = FALSE;
    lm_delete_req_info_sbas.z_request.z_delete_request.force_rcvr_off = force_rcvr_off;

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_TIME)
    {
      MSG_LOW("Deleting Time", 0, 0, 0);
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteClockMask = C_DELETE_INFO_CLOCK_INFO;
      /* Time/clock info is independent of GPS/GLO, so technically we don't need to mark GPS.
         But we need this otherwise no call will be made to delete data. */
      b_delete_mgp_gps = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_CLOCK_INFO)
    {
      MSG_LOW("Deleting GPS/GLONASS/BDS Time using mask", 0, 0, 0);
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteClockMask = delete_param_ptr->delete_clock_info_mask;
      /* Clock is structure is common for GPS and Glo, hence calling either delete is fine*/
      b_delete_mgp_gps = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_TIME_GPS)
    {
      MSG_LOW("Deleting GPS Time", 0, 0, 0);
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteClockMask |= C_DELETE_INFO_CLOCK_GPSTIME_EST;
      b_delete_mgp_gps = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_TIME_GLO)
    {
      MSG_LOW("Deleting GLO Time", 0, 0, 0);
      lm_delete_req_info_glo.z_request.z_delete_request.z_mgp_info.q_DeleteClockMask |= C_DELETE_INFO_CLOCK_GLOTIME_EST;
      b_delete_mgp_glo = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_TIME_BDS)
    {
      MSG_LOW("Deleting BDS Time", 0, 0, 0);
      lm_delete_req_info_bds.z_request.z_delete_request.z_mgp_info.q_DeleteClockMask |= C_DELETE_INFO_CLOCK_BDSTIME_EST;
      b_delete_mgp_bds = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_TIME_GAL)
    {
      MSG_LOW("Deleting GAL Time", 0, 0, 0);
      lm_delete_req_info_gal.z_request.z_delete_request.z_mgp_info.q_DeleteClockMask |= C_DELETE_INFO_CLOCK_GALTIME_EST;
      b_delete_mgp_gal = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_POS)
    {
      MSG_LOW("Deleting Pos", 0, 0, 0);
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeletePosMask = C_DELETE_INFO_POS_INFO;
      /* Position is independent of GPS/GLO, so technically we don't need to mark GPS.
         But we need this otherwise no call will be made to delete data. */
      b_delete_mgp_gps = TRUE;
      tm_core_clear_cached_pos();
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_EPH)
    {
      MSG_LOW("Deleting GPS EPH", 0, 0, 0);
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteEphMask = delete_param_ptr->eph_sv_mask;
      b_delete_mgp_gps = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_EPH_QZSS)
    {
      MSG_LOW("Deleting QZSS EPH", 0, 0, 0);
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteEphMask |= ((uint64)delete_param_ptr->qzss_eph_sv_mask)<<32;
      b_delete_mgp_qzss = TRUE;
    }


    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_EPH_GLO)
    {
        MSG_LOW("Deleting Glo EPH", 0, 0, 0);
        lm_delete_req_info_glo.z_request.z_delete_request.z_mgp_info.t_DeleteEphMask = delete_param_ptr->glo_eph_sv_mask;
        b_delete_mgp_glo = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_EPH_SBAS)
    {
      MSG_LOW("Deleting SBAS EPH", 0, 0, 0);
      lm_delete_req_info_sbas.z_request.z_delete_request.z_mgp_info.q_DeleteSbasEphMask = delete_param_ptr->sbas_eph_sv_mask;
      b_delete_mgp_sbas = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_EPH_BDS)
    {
      MSG_LOW("Deleting BDS EPH", 0, 0, 0);
      lm_delete_req_info_bds.z_request.z_delete_request.z_mgp_info.t_DeleteBdsEphMask = delete_param_ptr->bds_eph_sv_mask;
      b_delete_mgp_bds = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_EPH_GAL)
    {
      MSG_LOW("Deleting GAL EPH", 0, 0, 0);
      lm_delete_req_info_gal.z_request.z_delete_request.z_mgp_info.t_DeleteGalEphMask = delete_param_ptr->gal_eph_sv_mask;
      b_delete_mgp_gal = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_ALM)
    {
      MSG_LOW("Deleting GPS ALM", 0, 0, 0);
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteAlmMask |= (uint64)delete_param_ptr->alm_sv_mask;
      b_delete_mgp_gps = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_ALM_QZSS)
    {
      MSG_LOW("Deleting QZSS ALM", 0, 0, 0);
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteAlmMask |= ((uint64)delete_param_ptr->qzss_alm_sv_mask)<<32;
      b_delete_mgp_qzss = TRUE;
    }


    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_ALM_GLO)
     {
       MSG_LOW("Deleting Glo ALM", 0, 0, 0);
       lm_delete_req_info_glo.z_request.z_delete_request.z_mgp_info.t_DeleteAlmMask = delete_param_ptr->glo_alm_sv_mask;
       b_delete_mgp_glo = TRUE;
     }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_ALM_SBAS)
    {
      MSG_LOW("Deleting SBAS ALM", 0, 0, 0);
      lm_delete_req_info_sbas.z_request.z_delete_request.z_mgp_info.q_DeleteSbasAlmMask = delete_param_ptr->sbas_alm_sv_mask;
      b_delete_mgp_sbas = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_ALM_BDS)
    {
      MSG_LOW("Deleting BDS ALM", 0, 0, 0);
      lm_delete_req_info_bds.z_request.z_delete_request.z_mgp_info.t_DeleteBdsAlmMask = delete_param_ptr->bds_alm_sv_mask;
      b_delete_mgp_bds = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_ALM_GAL)
    {
      MSG_LOW("Deleting GAL ALM", 0, 0, 0);
      lm_delete_req_info_gal.z_request.z_delete_request.z_mgp_info.t_DeleteGalAlmMask = delete_param_ptr->gal_alm_sv_mask;
      b_delete_mgp_gal = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_ALM_CORR)
    {
      MSG_LOW("Deleting GPS XTRA", 0, 0, 0);
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.u_DeleteXtra = TRUE;
      b_delete_mgp_gps = TRUE;
    }
    //QZSS XTRA deletion For later phase
#if 0
    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_ALM_CORR_QZSS)
    {
      MSG_LOW("Deleting QZSS XTRA", 0, 0, 0);
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.u_DeleteXtra = TRUE;
      b_delete_mgp_QZSS = TRUE;
    }
#endif

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_ALM_CORR_GLO)
    {
      MSG_LOW("Deleting GLO XTRA", 0, 0, 0);
      lm_delete_req_info_glo.z_request.z_delete_request.z_mgp_info.u_DeleteXtra = TRUE;
      b_delete_mgp_glo = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_ALM_CORR_BDS)
    {
      MSG_LOW("Deleting BDS XTRA ", 0, 0, 0);
      lm_delete_req_info_bds.z_request.z_delete_request.z_mgp_info.u_DeleteXtra = TRUE;
      b_delete_mgp_bds = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_ALM_CORR_GAL)
    {
      MSG_LOW("Deleting GAL XTRA ", 0, 0, 0);
      lm_delete_req_info_gal.z_request.z_delete_request.z_mgp_info.u_DeleteXtra = TRUE;
      b_delete_mgp_gal = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_IONO)
    {
      MSG_LOW("Deleting IONO", 0, 0, 0);
      //TODO: q_DeleteIonoMask should be 64-bit
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteIonoMask = C_DELETE_INFO_IONO_INFO;
      b_delete_mgp_gps = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_IONO_QZSS)
    {
      MSG_LOW("Deleting QZSS IONO", 0, 0, 0);
      //TODO: q_DeleteIonoMask should be 64-bit
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteIonoMask = C_DELETE_INFO_IONO_INFO;
      b_delete_mgp_qzss = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_RTI)
    {
      MSG_LOW("Deleting RTI", 0, 0, 0);
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteRtiMask = C_DELETE_INFO_RTI_INFO;
      b_delete_mgp_gps = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_UTC)
    {
      MSG_LOW("Deleting UTC", 0, 0, 0);
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.u_DeleteUtc = TRUE;
      lm_delete_req_info_glo.z_request.z_delete_request.z_mgp_info.u_DeleteUtc = TRUE;
      lm_delete_req_info_bds.z_request.z_delete_request.z_mgp_info.u_DeleteUtc = TRUE;
      lm_delete_req_info_gal.z_request.z_delete_request.z_mgp_info.u_DeleteUtc = TRUE;
      b_delete_mgp_gps = TRUE;
      b_delete_mgp_glo = TRUE;
      b_delete_mgp_bds = TRUE;
      b_delete_mgp_gal = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_HEALTH)
    {
      MSG_LOW("Deleting HEALTH", 0, 0, 0);
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteSvHealthMask = C_DELETE_INFO_SV_HEALTH_INFO;
      lm_delete_req_info_bds.z_request.z_delete_request.z_mgp_info.t_DeleteBdsSvHealthMask = C_DELETE_INFO_BDS_SV_HEALTH_INFO;
      lm_delete_req_info_gal.z_request.z_delete_request.z_mgp_info.t_DeleteGalSvHealthMask = C_DELETE_INFO_GAL_SV_HEALTH_INFO;
 
      b_delete_mgp_gps = TRUE;
      b_delete_mgp_bds = TRUE;
      b_delete_mgp_gal = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_HEALTH_QZSS)
    {
      MSG_LOW("Deleting QZSS HEALTH", 0, 0, 0);
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteSvHealthMask |= (uint64)C_DELETE_INFO_SV_HEALTH_INFO<<32;
      b_delete_mgp_qzss = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_SVDIR)
    {
      MSG_LOW("Deleting SV DIR GPS", 0, 0, 0);
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteSvDirMask = (uint64)C_DELETE_INFO_SV_DIR_INFO&0x00000000FFFFFFFFULL;
      b_delete_mgp_gps = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_SVDIR_QZSS)
    {
      MSG_LOW("Deleting SV DIR QZSS", 0, 0, 0);
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteSvDirMask |= (uint64)C_DELETE_INFO_SV_DIR_INFO<<32;
      b_delete_mgp_qzss = TRUE;
    }


    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_SVDIR_GLO)
    {
      MSG_LOW("Deleting SV DIR GLO", 0, 0, 0);
      lm_delete_req_info_glo.z_request.z_delete_request.z_mgp_info.t_DeleteSvDirMask = C_DELETE_INFO_SV_DIR_INFO;
      b_delete_mgp_glo = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_SVDIR_SBAS)
    {
      MSG_LOW("Deleting SV DIR SBAS", 0, 0, 0);
      lm_delete_req_info_sbas.z_request.z_delete_request.z_mgp_info.q_DeleteSbasSvDirMask = C_DELETE_INFO_SBAS_SV_DIR_INFO; 
      b_delete_mgp_sbas = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_SVDIR_BDS)
    {
      MSG_LOW("Deleting SV DIR BDS", 0, 0, 0);
      lm_delete_req_info_bds.z_request.z_delete_request.z_mgp_info.t_DeleteBdsSvDirMask = C_DELETE_INFO_BDS_SV_DIR_INFO;
      b_delete_mgp_bds = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_SVDIR_GAL)
    {
      MSG_LOW("Deleting SV DIR GAL", 0, 0, 0);
      lm_delete_req_info_gal.z_request.z_delete_request.z_mgp_info.t_DeleteGalSvDirMask = C_DELETE_INFO_GAL_SV_DIR_INFO;
      b_delete_mgp_gal = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_SVSTEER)
    {
      MSG_LOW("Deleting SV STEER GPS", 0, 0, 0);
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteSvSteerMask = (uint64)C_DELETE_INFO_SV_STEER_INFO&0x00000000ffffffffULL;
      b_delete_mgp_gps = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_SVSTEER_QZSS)
    {
      MSG_LOW("Deleting SV STEER QZSS", 0, 0, 0);
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteSvSteerMask |= (uint64)C_DELETE_INFO_SV_STEER_INFO<<32;
      b_delete_mgp_qzss = TRUE;
    }


    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_SVSTEER_GLO)
    {
      MSG_LOW("Deleting SV STEER GLO", 0, 0, 0);
      lm_delete_req_info_glo.z_request.z_delete_request.z_mgp_info.t_DeleteSvSteerMask = C_DELETE_INFO_SV_STEER_INFO;
      b_delete_mgp_glo = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_SVSTEER_SBAS)
    {
      MSG_LOW("Deleting SV STEER SBAS", 0, 0, 0);
      lm_delete_req_info_sbas.z_request.z_delete_request.z_mgp_info.q_DeleteSbasSvSteerMask = C_DELETE_INFO_SBAS_SV_STEER_INFO; 
      b_delete_mgp_sbas = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_SVSTEER_BDS)
    {
      MSG_LOW("Deleting SV STEER BDS", 0, 0, 0);
      lm_delete_req_info_bds.z_request.z_delete_request.z_mgp_info.t_DeleteBdsSvSteerMask = C_DELETE_INFO_BDS_SV_STEER_INFO;
      b_delete_mgp_bds = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_SVSTEER_GAL)
    {
      MSG_LOW("Deleting SV STEER GAL", 0, 0, 0);
      lm_delete_req_info_gal.z_request.z_delete_request.z_mgp_info.t_DeleteGalSvSteerMask = C_DELETE_INFO_GAL_SV_STEER_INFO;
      b_delete_mgp_gal = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_SADATA)
    {
      MSG_LOW("Deleting SV DATA", 0, 0, 0);
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteSaDataMask = C_DELETE_INFO_SA_DATA_INFO;
      b_delete_mgp_gps = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_FREQ_BIAS_EST)
    {
      MSG_LOW("Deleting Freq Bias Est ", 0, 0, 0);
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.u_DeleteFreqBiasEst = C_DELETE_INFO_FREQ_BIAS_EST;
      b_delete_mgp_gps = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_CELLDB_INFO)
    {
      MSG_HIGH("Deleting CellDB information. Mask %08X", delete_param_ptr->delete_celldb_mask, 0, 0);

      /* Translate the Delete Mask from PDAPI definitions to GPS core mask definitions */
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteCellDBMask = 0;

      /* Celldb is independent of GPS/GLO, so technically we don't need to mark GPS.
         But we need this otherwise no call will be made to delete data. */
      b_delete_mgp_gps = TRUE;

      if( delete_param_ptr->delete_celldb_mask & PDSM_PA_DELETE_CELLDB_MASK_POS_INFO )
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteCellDBMask |= C_DELETE_INFO_CELLDB_POS;
      }
      if( delete_param_ptr->delete_celldb_mask & PDSM_PA_DELETE_CELLDB_MASK_LATEST_GPS_POS_INFO )
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteCellDBMask |= C_DELETE_INFO_CELLDB_LATEST_GPS_POS;
      }
      if( delete_param_ptr->delete_celldb_mask & PDSM_PA_DELETE_CELLDB_MASK_OTA_POS_INFO )
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteCellDBMask |= C_DELETE_INFO_CELLDB_OTA_POS;
      }
      if( delete_param_ptr->delete_celldb_mask & PDSM_PA_DELETE_CELLDB_MASK_EXT_REF_POS_INFO )
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteCellDBMask |= C_DELETE_INFO_CELLDB_EXT_REF_POS;
      }
      if( delete_param_ptr->delete_celldb_mask & PDSM_PA_DELETE_CELLDB_MASK_TIMETAG_INFO )
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteCellDBMask |= C_DELETE_INFO_CELLDB_TIMETAG;
      }
      if( delete_param_ptr->delete_celldb_mask & PDSM_PA_DELETE_CELLDB_MASK_CELLID_INFO )
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteCellDBMask |= C_DELETE_INFO_CELLDB_CELLID;
      }
      if( delete_param_ptr->delete_celldb_mask & PDSM_PA_DELETE_CELLDB_MASK_CACHED_CELLID_INFO )
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteCellDBMask |= C_DELETE_INFO_CELLDB_CACHED_CELLID;
      }
      if( delete_param_ptr->delete_celldb_mask & PDSM_PA_DELETE_CELLDB_MASK_LAST_SRV_CELL_INFO )
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteCellDBMask |= C_DELETE_INFO_CELLDB_LAST_SRV_CELL;
      }
      if( delete_param_ptr->delete_celldb_mask & PDSM_PA_DELETE_CELLDB_MASK_CUR_SRV_CELL_INFO )
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteCellDBMask |= C_DELETE_INFO_CELLDB_CUR_SRV_CELL;
      }
      if( delete_param_ptr->delete_celldb_mask & PDSM_PA_DELETE_CELLDB_MASK_NEIGHBOR_INFO_INFO )
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteCellDBMask |= C_DELETE_INFO_CELLDB_NEIGHBOR_INFO;
      }
    }

    if (delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_SV_NO_EXIST )
    {
      MSG_HIGH("Deleting SV No Exist ", 0, 0, 0);
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteSvNoExistMask = C_DELETE_INFO_SV_NON_EXIST;
      b_delete_mgp_gps = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GPS)
    {
      MSG_HIGH("Deleting GNSS SV blacklist GPS", 0, 0, 0);
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteBlacklistGpsMask |= (uint64)delete_param_ptr->gps_gnss_sv_blacklist_mask;
      b_delete_mgp_gps = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_GNSS_SV_BLACKLIST_QZSS)
    {
      MSG_HIGH("Deleting GNSS SV blacklist QZSS", 0, 0, 0);
      lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteBlacklistGpsMask |= ((uint64)delete_param_ptr->qzss_gnss_sv_blacklist_mask)<<32;
      b_delete_mgp_qzss = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GLO)
    {
      MSG_HIGH("Deleting GNSS SV blacklist GLO", 0, 0, 0);
      lm_delete_req_info_glo.z_request.z_delete_request.z_mgp_info.t_DeleteBlacklistGloMask = delete_param_ptr->glo_gnss_sv_blacklist_mask;
      b_delete_mgp_glo = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_GNSS_SV_BLACKLIST_BDS)
    {
      MSG_HIGH("Deleting GNSS SV blacklist BDS", 0, 0, 0);
      lm_delete_req_info_bds.z_request.z_delete_request.z_mgp_info.t_DeleteBlacklistBdsMask = delete_param_ptr->bds_gnss_sv_blacklist_mask;
      b_delete_mgp_bds = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GAL)
    {
      MSG_HIGH("Deleting GNSS SV blacklist GAL", 0, 0, 0);
      lm_delete_req_info_gal.z_request.z_delete_request.z_mgp_info.t_DeleteBlacklistGalMask = delete_param_ptr->gal_gnss_sv_blacklist_mask;
      b_delete_mgp_gal = TRUE;
    }

    if (delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_HEALTH_GLO) 
    {
      MSG_HIGH("Deleting GNNS SV Health GLO",0,0,0);
      lm_delete_req_info_glo.z_request.z_delete_request.z_mgp_info.t_DeleteSvHealthMask = C_DELETE_INFO_SV_HEALTH_INFO;
      b_delete_mgp_glo = TRUE;
    }

    if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_DELETE_HEALTH_BDS)
    {
      MSG_HIGH("Deleting GNSS SV HEALTH BDS", 0, 0, 0);
      lm_delete_req_info_bds.z_request.z_delete_request.z_mgp_info.t_DeleteBdsSvHealthMask = C_DELETE_INFO_BDS_SV_HEALTH_INFO;
      b_delete_mgp_bds = TRUE;
    }

    /*check runtime constellation configuration*/

    if (FALSE == sm_is_constellation_supported(SM_CCFG_RT_GPS))
    {
      b_delete_mgp_bds = FALSE;
    }

    if (FALSE == sm_is_constellation_supported(SM_CCFG_RT_GLO))
    {
      b_delete_mgp_glo = FALSE;
    }

    if (FALSE == sm_is_constellation_supported(SM_CCFG_RT_BDS))
    {
      b_delete_mgp_bds = FALSE;
    }

    if (FALSE == sm_is_constellation_supported(SM_CCFG_RT_QZSS))
    {
      b_delete_mgp_qzss = FALSE;
    }

    if (FALSE == sm_is_constellation_supported(SM_CCFG_RT_GAL))
    {
      b_delete_mgp_gal = FALSE;
    }


    if( FALSE == b_delete_mgp_gps && TRUE == b_delete_mgp_qzss)
    {
       //Only QZSS - clear GPS bits
       lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteEphMask &=0xFFFFFFFF00000000ULL;
       lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteAlmMask &=0xFFFFFFFF00000000ULL;
       lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteSvSteerMask &=0xFFFFFFFF00000000ULL;
       lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteSvDirMask &=0xFFFFFFFF00000000ULL;
       lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteBlacklistGpsMask &=0xFFFFFFFF00000000ULL;
       //TODO QZSS: lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteIonoMask &=0xFFFFFFFF00000000ULL;
    }
    else if (TRUE == b_delete_mgp_gps && FALSE == b_delete_mgp_qzss)
    {
       //Only GPS - clear QZSS bits
       lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteEphMask &=0x00000000FFFFFFFFULL;
       lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteAlmMask &=0x00000000FFFFFFFFULL;
       lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteSvSteerMask &=0x00000000FFFFFFFFULL;
       lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteSvDirMask &=0x00000000FFFFFFFFULL;
       lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteBlacklistGpsMask &=0x00000000FFFFFFFFULL;
       //TODO QZSS: lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteIonoMask &=0x00000000FFFFFFFFULL;
    }

    /* Delete GPS and QZSS specific data in same request */
    if(b_delete_mgp_gps || b_delete_mgp_qzss)
    {   
        /* GnssType for both GPS and QZSS is always MGP_GNSS_TYPE_GPS */
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.e_GnssType = MGP_GNSS_TYPE_GPS;
        (void)lm_request(&lm_delete_req_info);
    }

    /* Delete GLO specific data */
    if(b_delete_mgp_glo)
    {
        lm_delete_req_info_glo.z_request.z_delete_request.z_mgp_info.e_GnssType = MGP_GNSS_TYPE_GLO;
        (void)lm_request(&lm_delete_req_info_glo);
    }

    /* Delete BDS specific data */
    if(b_delete_mgp_bds)
    {
        lm_delete_req_info_bds.z_request.z_delete_request.z_mgp_info.e_GnssType = MGP_GNSS_TYPE_BDS;
        (void)lm_request(&lm_delete_req_info_bds);
    }

    /* Delete GAL specific data */
    if(b_delete_mgp_gal)
    {
        lm_delete_req_info_gal.z_request.z_delete_request.z_mgp_info.e_GnssType = MGP_GNSS_TYPE_GAL;
        (void)lm_request(&lm_delete_req_info_gal);
    }

    /* Delete SBAS specific data */
    if(b_delete_mgp_sbas)
    {
        lm_delete_req_info_sbas.z_request.z_delete_request.z_mgp_info.e_GnssType = MGP_GNSS_TYPE_SBAS;
        (void)lm_request(&lm_delete_req_info_sbas);
    }


  }

  if(delete_param_ptr->pdsm_delete_parms_flags & PA_AIDING_DATA_INJECT_MASK)
  {
    MSG_HIGH("Inject aiding data", 0, 0, 0);

    lm_inject_req_info_ptr = (lm_info_s_type *)pdsm_getbuf(sizeof(lm_info_s_type));

    if(lm_inject_req_info_ptr != NULL)
    {
      if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_TIME_UNC)
      {
        MSG_HIGH("Injecting time unc", 0, 0, 0);
        lm_inject_req_info_ptr->e_info_type = LM_SESSION_INFO_TIME_UNC;
        if (delete_type == PDSM_PA_DELETE_PARAMS_EX1)
        {
          lm_inject_req_info_ptr->z_info.f_new_time_unc = delete_param_ptr->f_time_unc;
        }
        else
        {
          lm_inject_req_info_ptr->z_info.f_new_time_unc = delete_param_ptr->time_unc;
        }
        (void)lm_info(lm_inject_req_info_ptr);
      }

      if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_POS_UNC)
      {
        MSG_HIGH("Injecting POS unc", 0, 0, 0);
        lm_inject_req_info_ptr->e_info_type = LM_SESSION_INFO_POS_UNC;

        if (delete_type == PDSM_PA_DELETE_PARAMS_EX1)
        {
          lm_inject_req_info_ptr->z_info.f_new_pos_unc = delete_param_ptr->f_pos_unc;
        }
        else
        {
          lm_inject_req_info_ptr->z_info.f_new_pos_unc = delete_param_ptr->pos_unc;
        }
        (void)lm_info(lm_inject_req_info_ptr);
      }

      if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_TIME_OFFSET)
      {
        MSG_HIGH("Injecting time offset", 0, 0, 0);
        lm_inject_req_info_ptr->e_info_type = LM_SESSION_INFO_TIME_OFFSET;

        if (delete_type == PDSM_PA_DELETE_PARAMS_EX1)
        {
          lm_inject_req_info_ptr->z_info.f_time_offset = delete_param_ptr->f_time_offset;
        }
        else
        {
          lm_inject_req_info_ptr->z_info.f_time_offset = delete_param_ptr->time_offset;
        }
        (void)lm_info(lm_inject_req_info_ptr);
      }

      if(delete_param_ptr->pdsm_delete_parms_flags & PDSM_PA_POS_OFFSET)
      {
         MSG_HIGH("Injecting POS offset", 0, 0, 0);
        lm_inject_req_info_ptr->e_info_type = LM_SESSION_INFO_POS_OFFSET;
        lm_inject_req_info_ptr->z_info.q_pos_offset = delete_param_ptr->pos_offset;
        (void)lm_info(lm_inject_req_info_ptr);
      }

      (void) pdsm_freebuf((char *)lm_inject_req_info_ptr);
    }
    else
    {
      MSG_ERROR("Failed to get buffer for lm_inject_req_info_ptr", 0, 0, 0);
    }

  }

  return;
}


/*===========================================================================

FUNCTION tm_core_inject_ext_time

DESCRIPTION
  This function is called to inject external time into MGP through LM

DEPENDENCIES

RETURN

SIDE EFFECTS

===========================================================================*/
void tm_core_inject_ext_time
(
  pdsm_pd_external_time_info_type *ext_time_ptr
)
{
  lm_request_info_s_type lm_ext_time_info;

  if(ext_time_ptr == NULL)
  {
    MSG_ERROR("Null ext_time_ptr", 0, 0, 0);
    return;
  }

  lm_ext_time_info.e_req_type = LM_REQUEST_SLOW_CLK_TIME_INJECT;

  lm_ext_time_info.z_request.z_slow_clk_inject_request.z_slow_clk_time.t_Milliseconds  =
      ext_time_ptr->time_msec;

  lm_ext_time_info.z_request.z_slow_clk_inject_request.z_slow_clk_time.q_Uncertainty  =
      ext_time_ptr->time_unc_msec;

  lm_ext_time_info.z_request.z_slow_clk_inject_request.z_slow_clk_time.e_TimeSource  =
      (gps_SlowClkExtTimeSourceType) ext_time_ptr->time_source;

  lm_ext_time_info.z_request.z_slow_clk_inject_request.z_slow_clk_time.b_TimeIsGps  =
      ext_time_ptr->time_is_gps;

  /* force source to be either GPS or UTC */
  lm_ext_time_info.z_request.z_slow_clk_inject_request.e_source =
     ext_time_ptr->time_is_gps ? GNSS_SLOW_CLOCK_SRC_GPS : GNSS_SLOW_CLOCK_SRC_UTC;

  lm_ext_time_info.z_request.z_slow_clk_inject_request.z_slow_clk_time.u_DiscontinuityCnt  =
      ext_time_ptr->discontinuity_count;

  lm_ext_time_info.z_request.z_slow_clk_inject_request.z_slow_clk_time.b_ForceFlag  =
      ext_time_ptr->force_flag;

   MSG_HIGH("Injecting External time from source: %d", (int)ext_time_ptr->time_source, 0, 0);

  (void)lm_request(&lm_ext_time_info);
}


/*===========================================================================

FUNCTION tm_core_inject_ext_pos

DESCRIPTION
  This function is called to inject external position into MGP through LM

DEPENDENCIES

RETURN

SIDE EFFECTS

===========================================================================*/
void tm_core_inject_ext_pos
(
  pdsm_cmd_s_type *p_cmd_type
)
{
  pdsm_pd_external_position_info_type *ext_pos_ptr;
  lm_request_info_s_type lm_ext_pos_info;
  gnss_ExternCoarsePosInjectionType   *p_lm_extern_coase_pos_inj;

  lm_request_info_s_type       lm_info_req_param;
  tm_pdapi_pd_event_s_type     pd_event_info;
  boolean                      u_sanity_ok = TRUE;
  float                        f_scale_up_to_99pct_conf;

  ext_pos_ptr = &p_cmd_type->cmd.pd.info.ext_pos_info;

  sm_log_extern_coarse_pos_inj_start(p_cmd_type); /* DM log */
  sm_report_event_coase_pos_inj_start(); /* DM event */

  memset((void *)&lm_info_req_param, 0, sizeof(lm_request_info_s_type));
  memset( &pd_event_info, 0, sizeof( tm_pdapi_pd_event_s_type ) );

  pd_event_info.pd_event_data.pd_info_type.client_id =
    p_cmd_type->cmd.pd.client_id;

  if(ext_pos_ptr == NULL)
  {
    MSG_ERROR("Null ext_pos_ptr", 0, 0, 0);
    u_sanity_ok = FALSE;
  }
  else
  {
    if ( !( ext_pos_ptr->opt_field_mask &
          PDSM_PD_EXTERN_COARSE_POS_LAT_LONG_VALID ) &&
         !( ext_pos_ptr->opt_field_mask &
          PDSM_PD_EXTERN_COARSE_POS_ALT_VALID )
       )
    {
      MSG_HIGH("No hor. or vert. pos", 0, 0, 0);
      u_sanity_ok = FALSE; /* no hor. or vert. pos. info., reject */
    }
    if ( ( ext_pos_ptr->opt_field_mask &
          PDSM_PD_EXTERN_COARSE_POS_HORIZ_CONF_VALID ) &&
        ( ( ext_pos_ptr->confidence_hor == 0 ) ||
          ( ext_pos_ptr->confidence_hor > 100 )))
    {
      u_sanity_ok = FALSE;
    }
    if ( ( ext_pos_ptr->opt_field_mask &
          PDSM_PD_EXTERN_COARSE_POS_CIRCULAR_UNC_VALID ) &&
          ( ext_pos_ptr->loc_unc_hor == 0 ))
    {
      u_sanity_ok = FALSE; /* hor. unc = 0 unreasonable, reject */
    }
    if ( ( ext_pos_ptr->opt_field_mask &
          PDSM_PD_EXTERN_COARSE_POS_ALT_VALID ) &&
         ( ( ext_pos_ptr->loc_unc_vert == 0 ) ||
           ( ext_pos_ptr->confidence_vert == 0 ) ||
           ( ext_pos_ptr->confidence_vert > 100 )
         )
       )
    {
      u_sanity_ok = FALSE; /* vert. unc = 0 unreasonable, reject */
    }
    if ( ( ext_pos_ptr->opt_field_mask &
          PDSM_PD_EXTERN_COARSE_POS_ELLIPTICAL_UNC_VALID ) &&
        ( ( ext_pos_ptr->horizontal_unc_elliptical.semimajor == 0 ) ||
          ( ext_pos_ptr->horizontal_unc_elliptical.semiminor == 0 )))
    {
      u_sanity_ok = FALSE;
  }
  }

  if ( !u_sanity_ok ) /* reject the injection command due to sanity failure */
  {
    pd_event_info.e_pd_event = PDSM_PD_EVENT_COARSE_POS_INJ_ERROR;
    tm_pdapi_pd_event_callback(&pd_event_info, TM_PDAPI_SESS_TYPE_NONE);
    return;
  }

  tm_gfc_qmi_inject_ext_pos(ext_pos_ptr);

  /* regardless if the corresponding position info. is valid, it does not hurt
     to rescale the confidence to 99 if 100 is given */
  if ( ext_pos_ptr->confidence_hor == 100 )
  {
    ext_pos_ptr->confidence_hor = 99;
  }
  if ( ext_pos_ptr->confidence_hor_raw == 100 )
  {
    ext_pos_ptr->confidence_hor_raw = 99;
  }
  if ( ext_pos_ptr->confidence_vert == 100 )
  {
    ext_pos_ptr->confidence_vert = 99;
  }



  /* populate the LM interface structure */

  p_lm_extern_coase_pos_inj =
    &(lm_ext_pos_info.z_request.z_extern_coarse_pos_inject_request.z_extern_coarse_pos);

  lm_ext_pos_info.e_req_type = LM_REQUEST_EXTERNAL_COARSE_POS_INJECT;

  p_lm_extern_coase_pos_inj->opt_field_mask = (gps_extern_coarse_pos_opt_field)ext_pos_ptr->opt_field_mask;

  p_lm_extern_coase_pos_inj->time_info_type = (gps_extern_coarse_pos_time_stamp_e_type)ext_pos_ptr->time_info_type;

  p_lm_extern_coase_pos_inj->timestamp_sec = ext_pos_ptr->timestamp_sec;

  p_lm_extern_coase_pos_inj->source = (gps_extern_coarse_pos_source_e_type)ext_pos_ptr->source;

  p_lm_extern_coase_pos_inj->latitude = ext_pos_ptr->latitude;

  p_lm_extern_coase_pos_inj->longitude = ext_pos_ptr->longitude;

  /* scale position unc. to that of 99% confidence level */
  f_scale_up_to_99pct_conf = tm_util_get_cep_scale_ratio_from_pct( (float)ext_pos_ptr->confidence_hor * 0.01f, 0.99f);

  p_lm_extern_coase_pos_inj->loc_unc_hor = ( (float)ext_pos_ptr->loc_unc_hor * (float)f_scale_up_to_99pct_conf );

  f_scale_up_to_99pct_conf = tm_util_get_cep_scale_ratio_from_pct( (float)ext_pos_ptr->confidence_hor_raw * 0.01f, 0.99f);

  p_lm_extern_coase_pos_inj->loc_unc_hor_raw = ( (float)ext_pos_ptr->loc_unc_hor_raw * (float)f_scale_up_to_99pct_conf );

  p_lm_extern_coase_pos_inj->alt_def = (gps_extern_coarse_pos_alt_def_e_type)ext_pos_ptr->alt_def;

  p_lm_extern_coase_pos_inj->altitude = ext_pos_ptr->altitude;

  /* TBD: scale position unc. to that of 99% confidence level */
  p_lm_extern_coase_pos_inj->loc_unc_vert = ext_pos_ptr->loc_unc_vert;

  p_lm_extern_coase_pos_inj->e_HoriRelIndicator = ext_pos_ptr->e_HoriRelIndicator;
  p_lm_extern_coase_pos_inj->e_VertRelIndicator = ext_pos_ptr->e_VertRelIndicator;

  p_lm_extern_coase_pos_inj->e_AltitudeSource   = ext_pos_ptr->e_AltitudeSource;
  p_lm_extern_coase_pos_inj->e_AltHorLinkage    = ext_pos_ptr->e_AltHorLinkage;
  p_lm_extern_coase_pos_inj->e_AltUncCoverage   = ext_pos_ptr->e_AltUncCoverage;

  p_lm_extern_coase_pos_inj->t_TimeTickMsec = cgps_TimeTickGetMsec();

  p_lm_extern_coase_pos_inj->horizontal_unc_elliptical.semiMajorUnc = ext_pos_ptr->horizontal_unc_elliptical.semimajor * f_scale_up_to_99pct_conf;
  p_lm_extern_coase_pos_inj->horizontal_unc_elliptical.semiMinorUnc = ext_pos_ptr->horizontal_unc_elliptical.semiminor * f_scale_up_to_99pct_conf;
  p_lm_extern_coase_pos_inj->horizontal_unc_elliptical.semiMajorAzimuthDegrees = ext_pos_ptr->horizontal_unc_elliptical.semimajor_azimuth;

  MSG_HIGH("Injecting external coarse pos", 0, 0, 0);


  if ( lm_request(&lm_ext_pos_info) )
  {
    /* Request the current GPS time from MGP */
    //lm_info_req_param.e_req_type = LM_REQUEST_POS_ESTIMATE;

    /* Send request to LM->MGP */
    //lm_request(&lm_info_req_param);

    pd_event_info.e_pd_event = PDSM_PD_EVENT_COARSE_POS_INJ_DONE;
    tm_pdapi_pd_event_callback(&pd_event_info, TM_PDAPI_SESS_TYPE_NONE);
    sm_report_event_coase_pos_inj_end(DM_EVT_EXTERN_COARSE_POS_INJ_END_REASON_DONE);
  }

  else
  {
    pd_event_info.e_pd_event = PDSM_PD_EVENT_COARSE_POS_INJ_ERROR;
    tm_pdapi_pd_event_callback(&pd_event_info, TM_PDAPI_SESS_TYPE_NONE);
    sm_report_event_coase_pos_inj_end(DM_EVT_EXTERN_COARSE_POS_INJ_END_REASON_ERROR);
  }

}

/*===========================================================================

FUNCTION tm_core_get_gps_state

DESCRIPTION
  This function is called to get the GPS Receiver state information

DEPENDENCIES

RETURN

SIDE EFFECTS

===========================================================================*/
void tm_core_get_gps_state
(
  tm_core_get_gps_state_param_s_type *p_get_gps_state_param
)
{
  boolean ret_val = FALSE;
  lm_request_info_s_type               lm_info_req_param;
  pdsm_cmd_s_type *p_cmd_type;
  boolean error = FALSE, request_wishlist = TRUE, request_pos_estimate = TRUE;

  MSG_MED("tm_core entering function [tm_core_get_gps_state] source %d",
          p_get_gps_state_param->source, 0, 0);

  memset((void *)&lm_info_req_param, 0, sizeof(lm_request_info_s_type));
  memset(&tm_core_info.gps_state_info, 0, sizeof(tm_core_info.gps_state_info));

  tm_core_info.session_info.q_ReqClientMask |= (TM_CORE_CLIENT_BITMASK(p_get_gps_state_param->source));
  tm_core_info.session_info.z_ReqClients[p_get_gps_state_param->source].w_TmRcvdDataMask = FALSE;

  p_cmd_type = p_get_gps_state_param->cmd.p_cmd_type;

  switch (p_get_gps_state_param->source)
  {
    case SM_GPS_STATE_INFO_REQ_TYPE_GET_PUNC_TUNC_FOR_WIFI:
      /* Indicate that the data needs to be sent to PDAPI = FALSE */
      tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_PUNC_TUNC_FOR_WIFI].q_ClientId = 0xFFFF;
      break;
    case SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE:
      /* Indicate that the data needs to be sent to PDAPI = TRUE */
      tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE].q_ClientId = p_cmd_type->cmd.pd.client_id;
      break;
    case SM_GPS_STATE_INFO_REQ_TYPE_ON_DEMAND:
      /* Indicate that the data needs to be sent to PDAPI = FALSE */
      tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_ON_DEMAND].q_ClientId = TM_ODP_CLIENT_ID;
      break;
    case SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_QMI:
      /* Indicate that the data needs to be sent to PDAPI = TRUE */
      tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_QMI].q_ClientId = p_cmd_type->cmd.pd.client_id;
      break;
    case SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_XTRA_T:
      /* Indicate that the data needs to be sent to PDAPI = FALSE */
      tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_XTRA_T].q_ClientId = 0xFFFF;
      request_wishlist = FALSE;
      request_pos_estimate = FALSE;
      break;      
    case SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_QMI:
      /* Indicate that the data needs to be sent to PDAPI = TRUE */
      tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_QMI].q_ClientId = p_cmd_type->cmd.pd.client_id;
      break;
    case SM_GPS_STATE_INFO_REQ_TYPE_GET_GNSS_AVAIL_POS_QMI:
      /* Indicate that the data needs to be sent to PDAPI = TRUE */
      tm_core_info.session_info.z_ReqClients[p_get_gps_state_param->source].q_ClientId = p_cmd_type->cmd.pd.client_id;
      break;
    case SM_GPS_STATE_INFO_REQ_TYPE_DIAG_POS:
      request_wishlist = FALSE;
      request_pos_estimate = FALSE;
      break;
    default:
      error = TRUE;
      break;
  }

  /* get the position, time, punc and tunc from MGP */
  if(FALSE == error)
  {
    mgp_GetGPSStateInfo(p_get_gps_state_param->source);

    if(request_wishlist)
    {
    lm_info_req_param.e_req_type = LM_REQUEST_AIDING_DATA_WISHLIST;
    lm_info_req_param.z_request.e_wishlist_req_type =
                 tm_core_xlate_prtl_to_wishlist_src(TM_SESS_OPERATION_MODE_NONE);

    ret_val = lm_request(&lm_info_req_param);
    }

    if(request_pos_estimate)
    {
#ifdef FEATURE_CGPS_ODP_2_0
    // need to ask PE for the position now not MGP
    lm_info_req_param.e_req_type = LM_REQUEST_POS_ESTIMATE;
    ret_val = lm_request(&lm_info_req_param);
#endif
  }
  }

  if ((error==TRUE)||(FALSE == ret_val))
  {
    MSG_HIGH("TM_Core: COuld not request WISH List info(err %d, retval %d Src %d)",
      error,ret_val,p_get_gps_state_param->source);
  }
}

/*===========================================================================

FUNCTION tm_core_get_prtl_idx_by_tr_id

DESCRIPTION
  This function is to get protocol array index by transaction ID

DEPENDENCIES

RETURN VALUE
  -1, if transaction ID mask can not be found.
  Otherwise, protocol array index for the transaction ID

SIDE EFFECTS

===========================================================================*/
int tm_core_get_prtl_idx_by_tr_id(uint32 transaction_id)
{
  switch(transaction_id & TM_CORE_TRANSACTION_ID_DISPATCH_MASK)
  {
    case TM_1X_UP_TRANSACTION_ID_RANGE:
      return TM_CORE_PRTL_CB_ARR_INDEX_1X_UP;

    case TM_UMTS_UP_TRANSACTION_ID_RANGE:
      return TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP;

    default:
      return -1;
  }
}

/*===========================================================================

FUNCTION tm_core_pdcomm_evt_handler

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_pdcomm_evt_handler(uint32 msg_id, void *p_Data)
{
  uint32  *tr_id_ptr;
  int prtlTableIndex;

  if(p_Data == NULL)
  {
    MSG_ERROR("Null p_Data", 0, 0, 0);
    return;
  }

  tr_id_ptr = (uint32 *)p_Data;

  prtlTableIndex = tm_core_get_prtl_idx_by_tr_id(*tr_id_ptr);

  if((prtlTableIndex >= 0) && (prtlTableIndex < TM_PRTL_NUM))
  {
    #ifdef FEATURE_CGPS_PDCOMM
    if(tm_core_info.prtl_func_cb_table[prtlTableIndex].data_pipe_handle_fp != NULL)
    {
      (void)tm_core_info.prtl_func_cb_table[prtlTableIndex].data_pipe_handle_fp(
              (pd_comms_client_ipc_message_type)(msg_id - TM_CORE_PDCOMM_MSG_ID_BASE),
              p_Data);
    }
    #endif
  }
  return;
}

/*===========================================================================

FUNCTION tm_core_get_prtl_idx_by_gm_tr_id

DESCRIPTION
  This function is to get protocol array index by transaction ID

DEPENDENCIES

RETURN VALUE
  -1, if transaction ID mask can not be found.
  Otherwise, protocol array index for the transaction ID

SIDE EFFECTS

===========================================================================*/
int tm_core_get_prtl_idx_by_gm_tr_id(uint32 transaction_id)
{
  switch(transaction_id & TM_CORE_TRANSACTION_ID_DISPATCH_MASK)
  {
    case TM_UMTS_UP_GM_TRANSACTION_ID_BASE:
      return TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP;
    case TM_GFC_QMI_GM_TRANSACTION_ID_BASE:
      return TM_CORE_PRTL_CB_ARR_INDEX_GFC_QMI;
#ifdef FEATURE_CGPS_XTRA_T      
    case TM_XTRA_T_GM_TRANSACTION_ID_BASE:
      return TM_CORE_PRTL_CB_ARR_INDEX_XTRA_T;      
#endif      
    default:
      return -1;
  }
}

/*===========================================================================

FUNCTION tm_core_gm_evt_handler

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_gm_evt_handler(uint32 msg_id, void *p_Data)
{
  uint32  *tr_id_ptr;
  int prtlTableIndex;

  if(p_Data == NULL)
  {
    MSG_ERROR("Null p_Data", 0, 0, 0);
    return;
  }

  tr_id_ptr = (uint32 *)p_Data;

  prtlTableIndex = tm_core_get_prtl_idx_by_gm_tr_id(*tr_id_ptr);

  if((prtlTableIndex >= 0) && (prtlTableIndex < TM_PRTL_NUM))
  {
    if(tm_core_info.prtl_func_cb_table[prtlTableIndex].gm_event_handle_fp!= NULL)
    {
      (void)tm_core_info.prtl_func_cb_table[prtlTableIndex].gm_event_handle_fp(
              (uint32)(msg_id - TM_CORE_GM_MSG_ID_BASE),
              p_Data);
    }
  }
  return;
}

/*===========================================================================

FUNCTION tm_core_cm_broadcast_event

DESCRIPTION
  This function is called by TM-CM module to inform TM-Core phone events

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_cm_broadcast_event
(
  uint32                        prtl_mask,
  prtl_event_u_type            *prtl_event_payload_ptr
)
{
  int i;
  uint32 prtl_type = 0;
  int prtlTableIndex;

  if(prtl_event_payload_ptr == NULL)
  {
    MSG_ERROR("prtl_event_payload_ptr NULL",0,0,0);
    return;
  }

  for(i  = 0; i < TM_PRTL_NUM; i++)
  {
    prtl_type = prtl_mask & (1 << i);

    if(prtl_type)
    {
      prtlTableIndex = tm_core_get_prtl_idx_by_type(prtl_type);

      if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
      {
        if(tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL)
        {
          (void)tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(
          PRTL_EVENT_TYPE_PHONE,
          prtl_event_payload_ptr);
        }
      }
    }
  }
}

/*===========================================================================

FUNCTION tm_core_cm_notify_event

DESCRIPTION
  This function is called by TM-CM module to inform TM-Core phone events

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_cm_notify_event
(
  tm_phone_event_e_type          phone_event_type,
  tm_phone_event_payload_u_type  *evt_payload_ptr
)
{
  boolean prtl_abort_session = TRUE;
  int prtlTableIndex = -1;
  prtl_event_u_type  *event_payload_ptr = NULL;
  sm_gm_srv_sys_notification_type z_gm_srv_sys_notify;

  MSG_HIGH("CM event %d",phone_event_type,0,0);
  event_payload_ptr = (prtl_event_u_type *)pdsm_getbuf(sizeof(prtl_event_u_type));
  if(event_payload_ptr == NULL)
  {
    MSG_ERROR("Cant get buffer",0,0,0);
    return;
  }
  switch(phone_event_type)
  {
    case TM_PHONE_EVENT_E911_CALL_ORIG:
#ifdef FEATURE_MO_GPS_FOR_ECALL
      if ( tm_core_info.config_info.aagps_emergency_services_spprt !=
             AAGPS_NV_EMERGENCY_SUPPORT_LOW_PRIORITY )
      {
        /* Emergency call!! Stop all ongoing GPS Sessions */
      tm_core_abort_recover(TRUE, TM_STOP_REASON_E911); /*lint !e506 !e730 */
      }
      else
      {
        /* We allow MO GPS even in E911 */
        if ((tm_core_info.config_info.sm_e911_config & 
             TM_CORE_DONT_ABORT_GPS_SESSION_ON_E911) == 0x0)
        {
          /* If emergency config set to DONT_ABORT_GPS_SESSION_ON_E911, do not 
           * abort any ongoing session */
          tm_core_abort_recover(TRUE, TM_STOP_REASON_E911); /*lint !e506 !e730 */
        }       
      }
#else
      /* Emergency call!! Stop all ongoing GPS Sessions */
      tm_core_abort_recover(TRUE, TM_STOP_REASON_E911); /*lint !e506 !e730 */
#endif
      event_payload_ptr->phone_event_payload.event = phone_event_type;
      tm_core_cm_broadcast_event(TM_PRTL_TYPE_ALL,event_payload_ptr);
    break;

    case TM_PHONE_EVENT_E911CB_ENTER:
      event_report(EVENT_GPS_E911_START);
      event_payload_ptr->phone_event_payload.event = phone_event_type;
      tm_core_cm_broadcast_event(TM_PRTL_TYPE_ALL,event_payload_ptr);
    break;

    case TM_PHONE_EVENT_E911CB_EXIT:
      event_report(EVENT_GPS_E911_END);
      event_payload_ptr->phone_event_payload.event = phone_event_type;
      tm_core_cm_broadcast_event(TM_PRTL_TYPE_ALL,event_payload_ptr);
    break;

    case TM_PHONE_EVENT_E911_CALL_END:
      event_payload_ptr->phone_event_payload.event = phone_event_type;
      tm_core_cm_broadcast_event(TM_PRTL_TYPE_ALL,event_payload_ptr);
    break;

    case TM_PHONE_EVENT_IN_1X_COVERAGE:
#ifdef FEATURE_CGPS_UMTS_CP
      if(NULL!= evt_payload_ptr)
      {
  	 MSG_HIGH("Sys_mode: %d, Prev_sys_mode: %d", evt_payload_ptr->ss_info.prev_sys_mode, evt_payload_ptr->ss_info.sys_mode, 0);
  	 if((evt_payload_ptr->ss_info.prev_sys_mode == SYS_SYS_MODE_LTE) && (evt_payload_ptr->ss_info.sys_mode == SYS_SYS_MODE_CDMA))
  	  {
         tm_cm_phone_state_info_s_type phone_state_info;
           /* Query TM-CM module to get current acquired system */
         tm_cm_iface_get_phone_state(&phone_state_info);

         if (phone_state_info.e911_state != TM_CM_E911_STATE_NONE)
         {
           if ((tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_E911 ) ||
               (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_LPP_CP_NI) ||
               (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_UMTS_UP_NI))
           {
             /* if we are going to CSFB, cancel the ongoing fake emergency session */
             MSG_MED("RAT Change from LTE to 1X. Stop ongoing session.", 0, 0, 0);
             tm_core_abort_recover(TRUE, TM_STOP_REASON_SYS_MODE_CHANGE); /*lint !e506 !e730 */
           }
         }	  	  
      }	
      }
#endif /* FEATURE_CGPS_UMTS_CP */
      if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_1X_UP].event_cb_fp != NULL)
      {
        event_payload_ptr->phone_event_payload.event   = phone_event_type;
        (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_1X_UP].event_cb_fp(
                PRTL_EVENT_TYPE_PHONE,
                event_payload_ptr);
      }

      if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP].event_cb_fp != NULL)
      {
        event_payload_ptr->phone_event_payload.event = phone_event_type;
        (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP].event_cb_fp(
                PRTL_EVENT_TYPE_PHONE,
                event_payload_ptr);
      }

      #ifdef FEATURE_GNSS_PERIODIC_PPM
      #error code not present
#endif
    break;

    case TM_PHONE_EVENT_TC_UP:
    case TM_PHONE_EVENT_TC_EXIT:
    case TM_PHONE_EVENT_NEW_SS_INFO:
      if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_1X_UP].event_cb_fp != NULL)
      {
        event_payload_ptr->phone_event_payload.event   = phone_event_type;
        (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_1X_UP].event_cb_fp(
                PRTL_EVENT_TYPE_PHONE,
                event_payload_ptr);
      }
    break;

    case TM_PHONE_EVENT_IN_UMTS_COVERAGE:
    case TM_PHONE_EVENT_IN_TDS_COVERAGE:
#ifdef FEATURE_CGPS_UMTS_CP
      if(NULL!= evt_payload_ptr)
      {
		MSG_HIGH("TM Prev_sys_mode: %u, Sys_mode: %u", evt_payload_ptr->ss_info.prev_sys_mode, evt_payload_ptr->ss_info.sys_mode, 0);
  	    if((evt_payload_ptr->ss_info.prev_sys_mode == SYS_SYS_MODE_LTE) && 
           ((evt_payload_ptr->ss_info.sys_mode == SYS_SYS_MODE_GSM) || 
			(evt_payload_ptr->ss_info.sys_mode == SYS_SYS_MODE_WCDMA) ||
			(evt_payload_ptr->ss_info.sys_mode == SYS_SYS_MODE_TDS)))
  	    {
          tm_cm_phone_state_info_s_type phone_state_info;
          /* Query TM-CM module to get current acquired system */
          tm_cm_iface_get_phone_state(&phone_state_info);

          if (phone_state_info.e911_state != TM_CM_E911_STATE_NONE)
          {
            if ((tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_E911) ||
                (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_LPP_CP_NI) ||
                (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_UMTS_UP_NI))
            {
              /* if we are going to CSFB, cancel the ongoing fake emergency session*/
		      MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "RAT Change from LTE to W/G. Stop ongoing session.");
              tm_core_abort_recover(TRUE, TM_STOP_REASON_LTE_TO_UMTS_CSFB); /*lint !e506 !e730 */
            }  

			/* Send an event to do an E911 early session start during CSFB for UMTS*/
			if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_CP].event_cb_fp != NULL)
      		{
			  MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Sending early session start during CSFB from L to W");
        	  event_payload_ptr->phone_event_payload.event = phone_event_type;
        	  (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_CP].event_cb_fp(
             	  	  PRTL_EVENT_TYPE_PHONE,
               		  event_payload_ptr);
      		}				              
          }	  	  
  	    }
      }
#endif /*FEATURE_CGPS_UMTS_CP*/
      if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP].event_cb_fp != NULL)
      {
        event_payload_ptr->phone_event_payload.event = phone_event_type;
        (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP].event_cb_fp(
                PRTL_EVENT_TYPE_PHONE,
                event_payload_ptr);
      }

      if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_1X_UP].event_cb_fp != NULL)
      {
        event_payload_ptr->phone_event_payload.event = phone_event_type;
        (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_1X_UP].event_cb_fp(
                PRTL_EVENT_TYPE_PHONE,
                event_payload_ptr);
      }
      break;


    case TM_PHONE_EVENT_TC_CONNECTED:
    case TM_PHONE_EVENT_TC_DISCONNECTED:
        if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_1X_CP].event_cb_fp != NULL)
        {
          event_payload_ptr->phone_event_payload.event = phone_event_type;
          (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_1X_CP].event_cb_fp(
                  PRTL_EVENT_TYPE_PHONE,
                  event_payload_ptr);
        }
        #ifdef FEATURE_GNSS_PERIODIC_PPM
        #error code not present
#endif

    break;

    case TM_PHONE_EVENT_MT_MSG:
      if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_1X_CP].event_cb_fp != NULL)
      {
        event_payload_ptr->phone_event_payload.event = phone_event_type;
        event_payload_ptr->phone_event_payload.payload.dbm_msg.length = evt_payload_ptr->dbm_msg.length;
        memscpy((void *)(event_payload_ptr->phone_event_payload.payload.dbm_msg.payload),
          sizeof(event_payload_ptr->phone_event_payload.payload.dbm_msg.payload),
          (const void*)evt_payload_ptr->dbm_msg.payload, evt_payload_ptr->dbm_msg.length);
        (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_1X_CP].event_cb_fp(
                PRTL_EVENT_TYPE_PHONE,
                event_payload_ptr);
      }
      break;

    case TM_PHONE_EVENT_SENT_MSG_STATUS:
      if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_1X_CP].event_cb_fp != NULL)
      {
        event_payload_ptr->phone_event_payload.event = phone_event_type;
        event_payload_ptr->phone_event_payload.payload.sent_msg_info.msg_status = evt_payload_ptr->sent_msg_info.msg_status;
        event_payload_ptr->phone_event_payload.payload.sent_msg_info.user_data  = evt_payload_ptr->sent_msg_info.user_data;
        (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_1X_CP].event_cb_fp(
                PRTL_EVENT_TYPE_PHONE,
                event_payload_ptr);
      }
      break;

   case TM_PHONE_EVENT_SS_SRV_STATUS:
   case TM_PHONE_EVENT_SS_SRV_DOMAIN:
      if (phone_event_type == TM_PHONE_EVENT_SS_SRV_STATUS)
      {
        /* For the following service transitions abort the currently ongoing session */
        if ( ((evt_payload_ptr->ss_info.prev_sys_mode == SYS_SYS_MODE_GSM) && (evt_payload_ptr->ss_info.sys_mode == SYS_SYS_MODE_WCDMA)) ||
             ((evt_payload_ptr->ss_info.prev_sys_mode == SYS_SYS_MODE_GSM) && (evt_payload_ptr->ss_info.sys_mode == SYS_SYS_MODE_NO_SRV)) ||
             ((evt_payload_ptr->ss_info.prev_sys_mode == SYS_SYS_MODE_WCDMA) && (evt_payload_ptr->ss_info.sys_mode == SYS_SYS_MODE_GSM)) ||
             ((evt_payload_ptr->ss_info.prev_sys_mode == SYS_SYS_MODE_WCDMA) && (evt_payload_ptr->ss_info.sys_mode == SYS_SYS_MODE_NO_SRV)) )
        {
          if ( tm_core_session_active() )
          {
            prtlTableIndex = tm_core_get_prtl_idx_by_type(tm_core_info.session_info.from_protocol);

            if (prtlTableIndex == TM_CORE_PRTL_CB_ARR_INDEX_UMTS_CP)
            {
              if(tm_core_info.prtl_func_cb_table[prtlTableIndex].stop_sess_req_fp)
              {
                MSG_HIGH("sys_mode change from %d to %d. Abort ongoing UMTS CP assisted sess",
                    evt_payload_ptr->ss_info.prev_sys_mode, evt_payload_ptr->ss_info.sys_mode, 0);

                prtl_abort_session = tm_core_info.prtl_func_cb_table[prtlTableIndex].stop_sess_req_fp(
                                                           tm_core_info.session_info.active_sess_handle,
                                                           PRTL_SESS_STOP_SINGLE_FIX,
                                                           TM_STOP_REASON_SYS_MODE_CHANGE);
                if(prtl_abort_session == TRUE)
                {
                  MSG_HIGH("UMTS CP sess aborted due to sys_mode chg", 0, 0, 0);
                }
                else
                {
                  MSG_HIGH("UMTS CP sess could not abort", 0, 0, 0);
                }
              }
            }
          }
        }
        // Update the SRV system changed information to GM module 
        MSG_MED("Serving system %d update to GM", evt_payload_ptr->ss_info.sys_mode, 0, 0);
        z_gm_srv_sys_notify.e_curr_srv_sys = evt_payload_ptr->ss_info.sys_mode;
        gm_srv_sys_update(&z_gm_srv_sys_notify);
      }

      if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_CP].event_cb_fp != NULL)
      {
        event_payload_ptr->phone_event_payload.event = phone_event_type;
        memscpy((void *)&(event_payload_ptr->phone_event_payload.payload.ss_info),
                sizeof(event_payload_ptr->phone_event_payload.payload.ss_info),
                (const void*)&(evt_payload_ptr->ss_info),sizeof(evt_payload_ptr->ss_info));
        (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_CP].event_cb_fp(
                PRTL_EVENT_TYPE_PHONE,
                event_payload_ptr);
      }
      break;

#ifdef FEATURE_CGPS_UMTS_CP
   case TM_PHONE_EVENT_LCS_MOLR:
   case TM_PHONE_EVENT_LCS_MOLR_COMPLETED:
   case TM_PHONE_EVENT_LCS_MTLR_NTFY_RES:
      if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_CP].event_cb_fp != NULL)
      {
        event_payload_ptr->phone_event_payload.event = phone_event_type;
        (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_CP].event_cb_fp(
                PRTL_EVENT_TYPE_PHONE,
                event_payload_ptr);
      }
      break;

   case TM_PHONE_EVENT_LCS_MTLR_NTFY_IND:
      if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_CP].event_cb_fp != NULL)
      {
        event_payload_ptr->phone_event_payload.event = phone_event_type;
        memscpy((void *)&(event_payload_ptr->phone_event_payload.payload.sups_data),
               sizeof(event_payload_ptr->phone_event_payload.payload.sups_data),
               (const void*)&(evt_payload_ptr->sups_data),sizeof(evt_payload_ptr->sups_data));
        (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_CP].event_cb_fp(
                PRTL_EVENT_TYPE_PHONE,
                event_payload_ptr);
      }
      break;

   case TM_PHONE_EVENT_LCS_MOLR_CONF:
      if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_CP].event_cb_fp != NULL)
      {
        event_payload_ptr->phone_event_payload.event = phone_event_type;
        memscpy((void *)&(event_payload_ptr->phone_event_payload.payload.sups_data),
                sizeof(event_payload_ptr->phone_event_payload.payload.sups_data),
                (const void*)&(evt_payload_ptr->sups_data), sizeof(evt_payload_ptr->sups_data));
        (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_CP].event_cb_fp(
                PRTL_EVENT_TYPE_PHONE,
                event_payload_ptr);
      }
      break;
#endif /* FEATURE_CGPS_UMTS_CP */
   case TM_PHONE_EVENT_OOS:
   case TM_PHONE_EVENT_SYS_MODE_LPM:
      if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP].event_cb_fp != NULL)
      {
       event_payload_ptr->phone_event_payload.event = phone_event_type;
       (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP].event_cb_fp(
               PRTL_EVENT_TYPE_PHONE,
               event_payload_ptr);
      }
     #ifdef FEATURE_GNSS_PERIODIC_PPM
      #error code not present
#endif /* FEATURE_GNSS_PERIODIC_PPM */
      {
        cgps_CellInfoCachedStruct cell_db_data;

        memset((void*)&cell_db_data, 0, sizeof(cell_db_data));
        
        cell_db_data.e_ActiveRAT = CELLDB_ACTIVERAT_NONE;
        
        /*Send the update to Geofencing module*/
        (void)gm_cell_db_update(CELLDB_ACTIVERAT_NONE, &cell_db_data);

        
        MSG_MED("Serving system %d update to GM", SYS_SYS_MODE_NO_SRV, 0, 0);
        z_gm_srv_sys_notify.e_curr_srv_sys = SYS_SYS_MODE_NO_SRV;
        gm_srv_sys_update(&z_gm_srv_sys_notify);
      }
      break;



   case TM_PHONE_EVENT_DSDS_SUBS_CHANGED:
        if ( tm_core_session_active() )
        {
          // If subscription changed, end the current session.
          prtlTableIndex = tm_core_get_prtl_idx_by_type(tm_core_info.session_info.from_protocol);

          if (prtlTableIndex == TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP)
          {
            if(tm_core_info.prtl_func_cb_table[prtlTableIndex].stop_sess_req_fp)
            {
              #ifdef FEATURE_CGPS_UMTS_CP
              MSG_HIGH("DSDS Subscription changed. Stop UP Session", 0, 0, 0);
              #endif

              prtl_abort_session = tm_core_info.prtl_func_cb_table[prtlTableIndex].stop_sess_req_fp(
                                                         tm_core_info.session_info.active_sess_handle,
                                                         PRTL_SESS_STOP_SINGLE_FIX,
                                                         TM_STOP_REASON_SYS_MODE_CHANGE);
              if(prtl_abort_session == TRUE)
              {
                MSG_HIGH("UMTS UP sess aborted due to dsds subscription change", 0, 0, 0);
              }
              else
              {
                MSG_HIGH("UMTS UP sess could not abort", 0, 0, 0);
              }
            }
          }
        }
      break;


   case TM_PHONE_EVENT_OTASP_STATUS_COMMITTED:
     /* Send OTASP Status Committed event to 1X UP for potential MSID update */
     if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_1X_UP].event_cb_fp != NULL)
     {
       event_payload_ptr->phone_event_payload.event   = phone_event_type;
       (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_1X_UP].event_cb_fp(
               PRTL_EVENT_TYPE_PHONE,
               event_payload_ptr);
     }
     break;

    case TM_PHONE_EVENT_HO_COMPLETE:

      if ((tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_E911 ) ||
          (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_1X_CP_MT) ||
          (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_UMTS_CP_NI) ||
          (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_LPP_CP_NI) ||
          (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_UMTS_UP_NI))
      {
        /* if we are going to CSFB, cancel the ongoing fake emergency session */
        MSG_HIGH("SRVCC handover. Stop ongoing session.", 0, 0, 0);
        tm_core_abort_recover(TRUE, TM_STOP_REASON_SYS_MODE_CHANGE); /*lint !e506 !e730 */
      }
      break;
   default:
     MSG_MED("Phone event %d not handled",phone_event_type,0,0);
     break;
  }
  (void) pdsm_freebuf((char *)event_payload_ptr);
}


/*===========================================================================

FUNCTION tm_core_rr_notify_event

DESCRIPTION
  This function is called by TM-RR IFACE to inform TM-Core of RR events

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_rr_notify_event( prtl_rr_event_s_type * p_event )
{
  (void)tm_core_info.prtl_func_cb_table[ TM_CORE_PRTL_CB_ARR_INDEX_UMTS_CP ].event_cb_fp(
          PRTL_EVENT_TYPE_RR,
          (prtl_event_u_type *) p_event);
}

/*===========================================================================

FUNCTION tm_core_rrc_notify_event

DESCRIPTION
  This function is called by TmCore's IPC processor to inform TmCore of
  an incoming RRC message.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void tm_core_rrc_notify_event( prtl_rrc_event_s_type *p_event )
{

  if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_CP].event_cb_fp != NULL)
  {
    (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_CP].event_cb_fp(
            PRTL_EVENT_TYPE_RRC,
            (prtl_event_u_type *) p_event );
  }

}


/*===========================================================================

FUNCTION tm_core_mm_notify_event

DESCRIPTION
  This function is called by TmCore's IPC processor to inform TmCore of
  an incoming MM message.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void tm_core_mm_notify_event( prtl_mm_event_s_type  *p_event )
{

  if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_CP].event_cb_fp != NULL)
  {
    (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_CP].event_cb_fp(
            PRTL_EVENT_TYPE_MM,
            (prtl_event_u_type *)p_event );
  }

  if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP].event_cb_fp != NULL)
  {
    (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP].event_cb_fp(
            PRTL_EVENT_TYPE_MM,
            (prtl_event_u_type *)p_event );
  }


}

#ifdef  FEATURE_CGPS_LTE_CELLDB   
/*===========================================================================

FUNCTION tm_core_lte_nas_ota_reset_notify_event

DESCRIPTION
  This function is called by TmCore's IPC processor to inform TmCore of 
  an incoming LTE OTA message. 

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_lte_nas_ota_reset_notify_event( prtl_event_e_type const p_PrtlEvent )
  {
  
  if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP].event_cb_fp != NULL)
  {
    (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP].event_cb_fp(p_PrtlEvent, NULL);
  }      

    if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_LPP_CP].event_cb_fp != NULL)
    {
    (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_LPP_CP].event_cb_fp(p_PrtlEvent, NULL);
    }  

  /* MGP state isn't relevant when LTE OTA OTDOA reset is triggered */
  if ( p_PrtlEvent != PRTL_EVENT_TYPE_LTE_OTA_OTDOA_RESET)
  {
  /* if currently MGP is not in OFF state, force it to OFF by first using 
           the force-to-idle API then indicating force-to-off in the function
           of tm_core_delete_inject_aiding_data() */
   if ( tm_core_info.e_cached_rcvr_state != C_GPS_RCVR_STATE_OFF )
    {
     mgp_ReceiverForcedIdle();
     mgp_ReceiverForcedOff();
    }
  }
}
#endif


/*===========================================================================

FUNCTION tm_core_cell_db_proc

DESCRIPTION
  This function is called by TmCore's IPC processor to inform TmCore of
  an incoming cell database update message.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void  tm_core_cell_db_proc( void *cell_db_info_ptr )
{
  prtl_event_u_type  *event_payload_ptr = NULL;

  MSG_HIGH("TM got CellDB update", 0, 0, 0);

  /* allocate buffer for sending event to protocol submodule */
  event_payload_ptr = (prtl_event_u_type *)pdsm_getbuf(sizeof(prtl_event_u_type));
  if ( event_payload_ptr == NULL )
  {
    MSG_ERROR("Can't get buffer", 0, 0, 0);
    return;
  }

  event_payload_ptr->cell_db_event_payload.cell_db_event = PRTL_MAIN_MODEM_CELL_DB_UPD;
  event_payload_ptr->cell_db_event_payload.cell_db_event_payload.main_modem_cell_db_payload.cell_db_event_data_ptr = 
                &(((tm_cell_info_update_type *)cell_db_info_ptr)->cellInfoCached);
  event_payload_ptr->cell_db_event_payload.cell_db_event_payload.main_modem_cell_db_payload.e_TriggerRAT =
                (((tm_cell_info_update_type *)cell_db_info_ptr)->e_TriggerRAT);
  

  if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP].event_cb_fp != NULL)
  {
    (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP].event_cb_fp(
            PRTL_EVENT_TYPE_CELL_DB,
            event_payload_ptr);
  }
  if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_LPP_CP].event_cb_fp != NULL)
  {
    (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_LPP_CP].event_cb_fp(
            PRTL_EVENT_TYPE_CELL_DB,
            event_payload_ptr);
  }      

  /*Send the CellDB update to Geofencing module*/
  (void)gm_cell_db_update(
    event_payload_ptr->cell_db_event_payload.cell_db_event_payload.main_modem_cell_db_payload.e_TriggerRAT, 
    event_payload_ptr->cell_db_event_payload.cell_db_event_payload.main_modem_cell_db_payload.cell_db_event_data_ptr);

  (void) pdsm_freebuf((char *)event_payload_ptr);

}


/*===========================================================================

FUNCTION tm_core_lpp_cp_evt_handler

DESCRIPTION
  This function is called by TmCore's IPC processor to inform TmCore of
  an incoming LPP Control Plane Protocol events.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_lpp_cp_evt_handler( uint32 msg_id, void *lpp_cp_evt_info_ptr )
{
  prtl_event_u_type  *event_payload_ptr = NULL;

  MSG_HIGH("TM got LPP CP Protocol event update", 0, 0, 0);

  /* allocate buffer for sending event to protocol submodule */
  event_payload_ptr = (prtl_event_u_type *)pdsm_getbuf(sizeof(prtl_event_u_type));
  if ( event_payload_ptr == NULL )
  {
    MSG_ERROR("Can't get buffer", 0, 0, 0);
    return;
  }

  // copy the LPP CP event payload (carried in IPC payload)
  memscpy((void *)&event_payload_ptr->lpp_cp_event_payload.lpp_cp_event_data,
         sizeof(event_payload_ptr->lpp_cp_event_payload.lpp_cp_event_data),
         (void *)lpp_cp_evt_info_ptr,
         sizeof(event_payload_ptr->lpp_cp_event_payload.lpp_cp_event_data));
  event_payload_ptr->lpp_cp_event_payload.lpp_cp_event_id = 
    (prtl_lpp_cp_event_e_type)(msg_id - TM_CORE_MSG_ID_NAS_EVENT_BASE);

  if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_LPP_CP].event_cb_fp != NULL)
  {
    (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_LPP_CP].event_cb_fp(
            PRTL_EVENT_TYPE_LPP_CP,
            event_payload_ptr);
  }      

  (void) pdsm_freebuf((char *)event_payload_ptr);

} /* tm_core_lpp_cp_evt_handler */
/*===========================================================================

FUNCTION tm_core_l1_notify_event

DESCRIPTION
  This function is called by TmCore's IPC processor to inform TmCore of
  an incoming message from L1.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void tm_core_l1_notify_event ( prtl_l1_event_s_type *p_event )
{

  if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP].event_cb_fp != NULL)
  {
    (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP].event_cb_fp(
            PRTL_EVENT_TYPE_L1,
            (prtl_event_u_type *) p_event );
  }
#ifdef FEATURE_CGPS_UMTS_CELLDB
  /* Send the WCDMA cell meas to GM module */
  MSG_HIGH("SUPL: Send WCDMA cell meas to GM", 0, 0, 0);
  gm_wcdma_cell_meas_update(((prtl_event_u_type *)p_event)->l1_event_payload.l1_event_data_ptr->u.l1_cell_meas_update_ptr);
#endif

}


/*===========================================================================

FUNCTION tm_core_lte_l1_notify_event

DESCRIPTION
  This function is called by TmCore's IPC processor to inform TmCore of
  an incoming message from LTE L1.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void tm_core_lte_l1_notify_event ( prtl_lte_l1_event_s_type *p_event )
{

  if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP].event_cb_fp != NULL)
  {
    (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP].event_cb_fp(
            PRTL_EVENT_TYPE_LTE_L1,
            (prtl_event_u_type *) p_event );
  }
#ifdef FEATURE_CGPS_LTE_CELLDB
  /* Send the LTE cell meas to GM module */
  MSG_HIGH("SUPL: Send LTE cell meas to GM", 0, 0, 0);
  gm_lte_cell_meas_update(((prtl_event_u_type *)p_event)->lte_l1_event_payload.l1_event_data_ptr->u.l1_cell_meas_update_ptr);
#endif
}



/*===========================================================================

FUNCTION tm_core_get_last_position

DESCRIPTION
  This function is called by TmCore to return a cached position

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

boolean tm_core_get_last_position (tm_core_cached_pos_s_type *pd_cached_pos)
{
  if (z_cached_pos_info.cached_pos_valid == TRUE)
  {
    memscpy(pd_cached_pos, sizeof(*pd_cached_pos), &z_cached_pos_info, sizeof(z_cached_pos_info));
    return TRUE;
  }
  return FALSE;
}


/*===========================================================================

FUNCTION tm_core_xlate_gps_state_to_on_demand_info

DESCRIPTION
  This function is called by TmCore to translate a gps state information
to on-demand status

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static void tm_core_xlate_gps_state_to_on_demand_info(tm_core_gps_state_info_s_type *p_gps_state_info,
                                            sm_OnDemandPosPuncTuncStruct *p_on_demand_info)
{
  p_on_demand_info->b_Valid = TRUE;
  p_on_demand_info->f_Punc = p_gps_state_info->pos_unc;
  p_on_demand_info->f_Tunc = p_gps_state_info->time_unc_us ;
  p_on_demand_info->eph_svmask = p_gps_state_info->eph_svmask;
  p_on_demand_info->alm_svmask = p_gps_state_info->alm_svmask;
  p_on_demand_info->health_svmask = p_gps_state_info->health_svmask;
  p_on_demand_info->health_bad_svmask = p_gps_state_info->health_bad_svmask;
  p_on_demand_info->health_noex_svmask = p_gps_state_info->health_noex_svmask;
  p_on_demand_info->health_unk_svmask = p_gps_state_info->health_unk_svmask;
  p_on_demand_info->visible_svmask = p_gps_state_info->visible_svmask;
  tm_core_on_demand_pos_punc_tunc_proc ((void*)p_on_demand_info);
}

/*===========================================================================

FUNCTION tm_core_xlate_gps_state_to_ext_status

DESCRIPTION
  This function is called by TmCore to translate a gps state information
to external status

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void  tm_core_xlate_gps_state_to_ext_status(tm_core_gps_state_info_s_type *p_gps_state_info,
  pdsm_pd_ext_gps_state_s_type *pd_gps_state_info)
{
  pdsm_xtra_download_info_s_type p_download_info ;
  boolean xtra_valid;

  pd_gps_state_info->flags = p_gps_state_info->flags;
  pd_gps_state_info->lat = p_gps_state_info->lat;
  pd_gps_state_info->lon = p_gps_state_info->lon;
  pd_gps_state_info->altitude = p_gps_state_info->altitude;
  pd_gps_state_info->loc_unc_horizontal = p_gps_state_info->pos_unc;
  pd_gps_state_info->loc_unc_vertical = p_gps_state_info->loc_unc_vertical;

  pd_gps_state_info->timestamp_tow_ms = p_gps_state_info->timestamp_tow_ms;
  pd_gps_state_info->timestamp_gps_week = p_gps_state_info->timestamp_gps_week;  /*GPS week number */
  /*Time uncertainty in milli seconds, floor at 20 ms */
  if (p_gps_state_info->time_unc_us < 20000)
  {
    pd_gps_state_info->time_unc_ms = 20;
  }
  else
  {
    pd_gps_state_info->time_unc_ms = p_gps_state_info->time_unc_us/1000;
  }
  pd_gps_state_info->engine_state = p_gps_state_info->engine_state; /*  GPS engine state on/off. always present, no flag */
  pd_gps_state_info->eph_svmask = p_gps_state_info->eph_svmask; /* SV Mask for the ephemeris */
  pd_gps_state_info->alm_svmask = p_gps_state_info->alm_svmask; /* SV Mask for the Almanac */
  pd_gps_state_info->iono_valid = p_gps_state_info->iono_valid; /* IONO */
  pd_gps_state_info->health_svmask = p_gps_state_info->health_svmask; /* SV mask for the health */
  pd_gps_state_info->visible_svmask = p_gps_state_info->visible_svmask; /* sv mask for visible SVs*/
  xtra_valid = tm_xtra_set_xtra_time_and_validity(&p_download_info);
  if (xtra_valid)
  {
    pd_gps_state_info->flags |= PDSM_PD_EXT_GPS_STATE_XTRA_VALID;
    pd_gps_state_info->xtra_start_gps_week = tm_core_info.gps_state_info.start_gps_week = p_download_info.startGpsWeek;
    pd_gps_state_info->xtra_start_gps_minutes = tm_core_info.gps_state_info.start_gps_minutes = p_download_info.startGpsMinutes;
    pd_gps_state_info->xtra_valid_duration_hours = tm_core_info.gps_state_info.valid_duration_hours =
    p_download_info.validDurationHours;
  }
}


/*===========================================================================

FUNCTION tm_core_xlate_pd_ext_pos_to_qmi

DESCRIPTION
  This function is called by TM Core to translate the SM position report
  structure to a QMI defined position report.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static void tm_core_xlate_pd_ext_pos_to_qmi
(
  const sm_GnssFixRptStructType *p_GnssFixRpt,
  pdsm_pd_ext_pos_rpt_s_type    *p_PdsmExtPos
)
{
  double f_tmp = 0, f_vel_east = 0, f_vel_north = 0;
  uint32 i = 0, count = 0;
  float f_Punc;
  pdsm_utc_time_s_type z_pdsm_utc_time;

  /* UTC timestamp in millisecond since Jan 1st, 1970 */
  uint64 t_timestamp_utc_ms, t_timestamp_utc_s;                 

  f_Punc = tm_core_calculate_2d_punc(p_GnssFixRpt);
  memset(p_PdsmExtPos, 0, sizeof(pdsm_pd_ext_pos_rpt_s_type));
  if (p_GnssFixRpt->z_NavPos.z_PosFlags.b_IsValid)
  {
    // Convert the latitude from radians into degrees. 
    // radians = degrees * (PI/180) 
    // degrees = (radians * 180)/PI

    /* Latitude */
    p_PdsmExtPos->latitude = ( p_GnssFixRpt->z_NavPos.d_PosLla[0] * 180 )/C_PI;
    p_PdsmExtPos->latitude_valid = TRUE;
  
    /*   Longitude */
    // Convert the longitude from radians into degrees. 
    p_PdsmExtPos->longitude =  ( p_GnssFixRpt->z_NavPos.d_PosLla[1] * 180 )/C_PI;
    p_PdsmExtPos->longitude_valid = TRUE;
  
    /*   Circular Horizontal Position Uncertainty */
    if (p_GnssFixRpt->z_NavPos.z_SmPosFlags.b_IsPuncValid)
    {
      p_PdsmExtPos->horUncCircular = f_Punc;
      p_PdsmExtPos->horUncCircular_valid = TRUE;
    }

    /*  Horizontal Elliptical Uncertainty (Semi-Minor Axis) */
    p_PdsmExtPos->horUncEllipseSemiMajor = p_GnssFixRpt->z_NavPos.f_ErrorEllipse[2];
    p_PdsmExtPos->horUncEllipseSemiMajor_valid = TRUE;
  
    /*  Horizontal Elliptical Uncertainty (Semi-Major Axis) */
    p_PdsmExtPos->horUncEllipseSemiMinor = p_GnssFixRpt->z_NavPos.f_ErrorEllipse[1];
    p_PdsmExtPos->horUncEllipseSemiMinor_valid = TRUE;
    /*  Elliptical Horizontal Uncertainty Azimuth */
    p_PdsmExtPos->horUncEllipseOrientAzimuth = p_GnssFixRpt->z_NavPos.f_ErrorEllipse[0];
    p_PdsmExtPos->horUncEllipseOrientAzimuth_valid = TRUE;

    /*  Horizontal circular confidence */
    p_PdsmExtPos->horCircularConfidence = 63; // Hard coded as of now
    p_PdsmExtPos->horCircularConfidence_valid = TRUE;

    /*  Horizontal Elliptical Confidence */
    p_PdsmExtPos->horEllipticalConfidence = 39;
    p_PdsmExtPos->horEllipticalConfidence_valid = TRUE;
  
    /*  Horizontal Reliability */
    switch(p_GnssFixRpt->z_NavPos.e_HoriRelIndicator)
    {
      case PDSM_POSITION_RELIABILITY_NOT_SET:
        /* Not set, reliability info is not present */
        p_PdsmExtPos->horReliability = eLOC_ENG_RELIABILITY_NOT_SET_V01;
        break;

      case PDSM_POSITION_RELIABILITY_VERY_LOW:
        /* Not checked, use at user's own risk */
        p_PdsmExtPos->horReliability = eLOC_ENG_RELIABILITY_VERY_LOW_V01;
        break;

      case PDSM_POSITION_RELIABILITY_LOW:
        /* Limited cross-checked */
        p_PdsmExtPos->horReliability = eLOC_ENG_RELIABILITY_LOW_V01;
        break;

      case PDSM_POSITION_RELIABILITY_MEDIUM:
        /* Limited cross-checked */
        p_PdsmExtPos->horReliability = eLOC_ENG_RELIABILITY_MEDIUM_V01;
        break;

      case PDSM_POSITION_RELIABILITY_HIGH:
        /* Robust cross-checked */
        p_PdsmExtPos->horReliability = eLOC_ENG_RELIABILITY_HIGH_V01;
        break;

      default:
        /* Default reliability . Not set */
        p_PdsmExtPos->horReliability = eLOC_ENG_RELIABILITY_NOT_SET_V01;
        break;
    }
    p_PdsmExtPos->horReliability_valid = TRUE;
  
    /* Compute horizontal velocity from components and scale
    * to units of 0.25 m/s.
    */
  
    f_vel_east = p_GnssFixRpt->z_NavPos.f_VelEnu[0];
    f_vel_north = p_GnssFixRpt->z_NavPos.f_VelEnu[1];

    f_tmp = sqrt( f_vel_east * f_vel_east + f_vel_north * f_vel_north );

    if (f_tmp < TM_CORE_MIN_VELOCTY)
    {
      p_PdsmExtPos->horSpeed = 0;
    }
    else
    {
      p_PdsmExtPos->horSpeed =
      (float) tm_util_nearest_long( 4.0 * f_tmp);
    }
    p_PdsmExtPos->horSpeed_valid = TRUE;

    /*  Horizontal Speed Uncertainty */
    p_PdsmExtPos->horSpeedUnc = (float) f_tmp;
    /* suds84. This is actually 3d uncertainty??*/
    p_PdsmExtPos->horSpeedUnc_valid = TRUE;

    /*  Altitude With Respect to Ellipsoid */
    p_PdsmExtPos->altitudeWrtEllipsoid = p_GnssFixRpt->z_NavPos.d_PosLla[2];
    p_PdsmExtPos->altitudeWrtEllipsoid_valid = TRUE;

    /*  Altitude With Respect to Sea Level */
    p_PdsmExtPos->altitudeWrtMeanSeaLevel = p_GnssFixRpt->z_NavPos.d_PosLla[2] + 500;
    p_PdsmExtPos->altitudeWrtMeanSeaLevel_valid = TRUE;
  
    /*  Vertical Uncertainty */
    p_PdsmExtPos->vertUnc = p_GnssFixRpt->z_NavPos.f_VuncVertMps;
    p_PdsmExtPos->vertUnc_valid = TRUE;
  
    /*  Vertical Confidence */
    p_PdsmExtPos->vertConfidence = 68;
    p_PdsmExtPos->vertConfidence_valid = TRUE;
 
    /*  Vertical Reliability */
    switch(p_GnssFixRpt->z_NavPos.e_VertRelIndicator)
    {
      case PDSM_POSITION_RELIABILITY_NOT_SET:
        /* Not set, reliability info is not present */
        p_PdsmExtPos->vertReliability = eLOC_ENG_RELIABILITY_NOT_SET_V01;
        break;

      case PDSM_POSITION_RELIABILITY_VERY_LOW:
        /* Not checked, use at user's own risk */
        p_PdsmExtPos->vertReliability = eLOC_ENG_RELIABILITY_VERY_LOW_V01;
        break;

      case PDSM_POSITION_RELIABILITY_LOW:
        /* Limited cross-checked */
        p_PdsmExtPos->vertReliability = eLOC_ENG_RELIABILITY_LOW_V01;
        break;

      case PDSM_POSITION_RELIABILITY_MEDIUM:
        /* Limited cross-checked */
        p_PdsmExtPos->vertReliability = eLOC_ENG_RELIABILITY_MEDIUM_V01;
        break;

      case PDSM_POSITION_RELIABILITY_HIGH:
        /* Robust cross-checked */
        p_PdsmExtPos->vertReliability = eLOC_ENG_RELIABILITY_HIGH_V01;
        break;

      default:
        /* Default reliability . Not set */
        p_PdsmExtPos->vertReliability = eLOC_ENG_RELIABILITY_NOT_SET_V01;
        break;
    }
    p_PdsmExtPos->vertReliability_valid = TRUE;

    /*  Vertical Speed */
    p_PdsmExtPos->vertSpeed = p_GnssFixRpt->z_NavPos.f_VelEnu[2];
    p_PdsmExtPos->vertSpeed_valid = TRUE;

    /*  Vertical Speed Uncertainty */
    p_PdsmExtPos->vertSpeedUnc = p_GnssFixRpt->z_NavPos.f_VuncVertMps;;
    p_PdsmExtPos->vertSpeedUnc_valid = TRUE;

    /*  Heading */
    p_PdsmExtPos->heading = p_GnssFixRpt->z_NavPos.f_HeadingRad;
    p_PdsmExtPos->heading_valid = TRUE;
  
    /*  Heading Uncertainty */
    p_PdsmExtPos->headingUnc = p_GnssFixRpt->z_NavPos.f_HeadingUncRad;
    p_PdsmExtPos->headingUnc_valid = TRUE;
  
    /*  Magnetic Deviation */
    p_PdsmExtPos->magneticDeviation = p_GnssFixRpt->z_NavPos.f_MagDeviation;
    p_PdsmExtPos->magneticDeviation_valid = TRUE;
  
    /*  Technology Used */
    p_PdsmExtPos->technologyMask_valid = TRUE;
    p_PdsmExtPos->technologyMask = 0;

    if (p_GnssFixRpt->z_NavPos.z_SmPosFlags.b_IsCellId)
    {
      p_PdsmExtPos->technologyMask |= LOC_ENG_POS_TECH_MASK_CELLID_V01;
    }

    if (p_GnssFixRpt->z_NavPos.z_SmPosFlags.b_IsWifi)
    {
      p_PdsmExtPos->technologyMask |= LOC_ENG_POS_TECH_MASK_WIFI_V01;
    }

    if (p_GnssFixRpt->z_NavPos.z_SmPosFlags.b_IsSensors)
    {
      p_PdsmExtPos->technologyMask |= LOC_ENG_POS_TECH_MASK_SENSORS_V01;
    }

    if (p_GnssFixRpt->z_NavPos.z_SmPosFlags.b_IsSatellite)
    {
      p_PdsmExtPos->technologyMask |= LOC_ENG_POS_TECH_MASK_SATELLITE_V01;
    }

    if (p_GnssFixRpt->z_NavPos.z_SmPosFlags.b_IsRefLoc)
    {
      p_PdsmExtPos->technologyMask |= LOC_ENG_POS_TECH_MASK_REFERENCE_LOCATION_V01;
    }

    if (p_GnssFixRpt->z_NavPos.z_SmPosFlags.b_IsCoarsePos)
    {
      p_PdsmExtPos->technologyMask |= LOC_ENG_POS_TECH_MASK_INJECTED_COARSE_POSITION_V01;
    }

    /*Set the technology valid mask to FALSE if we could not get the technology used*/
    if (p_PdsmExtPos->technologyMask == 0)
    {
      p_PdsmExtPos->technologyMask_valid = FALSE;
    }
      
    /* Position dilution of precision.*/
    p_PdsmExtPos->DOP.PDOP = p_GnssFixRpt->z_NavPos.f_PDOP;
    /* Horizontal dilution of precision.*/
    p_PdsmExtPos->DOP.HDOP = p_GnssFixRpt->z_NavPos.f_HDOP;
    /* Vertical dilution of precision. */
    p_PdsmExtPos->DOP.VDOP = p_GnssFixRpt->z_NavPos.f_VDOP;
    p_PdsmExtPos->DOP_valid = TRUE;

    /*  UTC Timestamp */    
    if(tm_util_calculate_utc(p_GnssFixRpt, &z_pdsm_utc_time, NULL)== TRUE)
    {
      // We have the UTC time in YearMonthHourMinuteSecond calculated from GPS time ( Since Jan 6 1980 ) .
      // Convert the UTC time we have into UNIX timestamp ( Since Jan1 1970 ) 
      gnss_GetUnixEpochFromUTC( &z_pdsm_utc_time,
                                &t_timestamp_utc_s) ;
      /* Scale it to milliseconds, as the TimeStamp now is in seconds. */
      t_timestamp_utc_ms = t_timestamp_utc_s * 1000;
      /* Now lets add more precision.
         utc_time.sec --> is seconds scaled by 100.
                          ( ie. 1sec is reported as 100 &
                            1.99 sec is reported as 199 which is 1990msec)
         GetUnixEpochFromUTC gives us back in seconds. So we can add the tenths
         of a second and hundredth of a second.
   
         Note: Currently the precision of hundredth of a second is lost in
               translation in SM.
      */
      t_timestamp_utc_ms +=
                     (( z_pdsm_utc_time.sec % 100 ) * 10 ) ;
      p_PdsmExtPos->timestampUtc = t_timestamp_utc_ms;
      p_PdsmExtPos->timestampUtc_valid = TRUE;
    }
    else
    {
      p_PdsmExtPos->timestampUtc_valid = FALSE;
    }
  
    /*  GPS Time */
    if (p_GnssFixRpt->z_NavPos.z_SmPosFlags.b_IsGpsTimeValid)
    {
      /* Week number of the fix*/
      p_PdsmExtPos->gpsTime.gpsWeek = p_GnssFixRpt->z_NavPos.w_GpsWeek;
      p_PdsmExtPos->gpsTime.gpsTimeOfWeekMs = p_GnssFixRpt->z_NavPos.q_GpsTimeMs;
      p_PdsmExtPos->gpsTime_valid = TRUE;
    }

    /*  Time Uncertainty */
    if (p_GnssFixRpt->z_NavPos.z_SmPosFlags.b_IsTuncValid)
    {
      p_PdsmExtPos->timeUnc = p_GnssFixRpt->z_NavPos.f_ClockBiasUncMs;
      p_PdsmExtPos->timeUnc_valid = TRUE;
    }

    /*  Time Source */
    /* Convert the time source which is of the type pdsm_extended_time_src_e_type into
       the type used by extended status report */
    switch(p_GnssFixRpt->z_NavPos.u_InitGpsTimeSrc)
    {
      case PDSM_GNSS_TIME_SRC_UNKNOWN:
        p_PdsmExtPos->timeSrc = eLOC_ENG_TIME_SRC_UNKNOWN_V01;
        break;
      case PDSM_GNSS_TIME_SRC_RESERVED:
        p_PdsmExtPos->timeSrc = eLOC_ENG_TIME_SRC_SYSTEM_TIMETICK_V01;
        break;
      case PDSM_GNSS_TIME_SRC_NETWORK_TIME_TRANSFER:
        p_PdsmExtPos->timeSrc = eLOC_ENG_TIME_SRC_NETWORK_TIME_TRANSFER_V01;
        break;
      case PDSM_GNSS_TIME_SRC_NETWORK_TIME_TAGGING:
        p_PdsmExtPos->timeSrc = eLOC_ENG_TIME_SRC_NETWORK_TIME_TAGGING_V01;
        break;
      case PDSM_GNSS_TIME_SRC_EXTERNAL_INPUT:
        p_PdsmExtPos->timeSrc = eLOC_ENG_TIME_SRC_EXTERNAL_INPUT_V01;
        break;
      case PDSM_GNSS_TIME_SRC_GPS_TOW_DECODE:
        p_PdsmExtPos->timeSrc = eLOC_ENG_TIME_SRC_TOW_DECODE_V01;
        break;
      case PDSM_GNSS_TIME_SRC_TOW_CONFIRMED:
        p_PdsmExtPos->timeSrc = eLOC_ENG_TIME_SRC_TOW_CONFIRMED_V01;
        break;
      case PDSM_GNSS_TIME_SRC_TOW_WEEK_CONFIRMED:
        p_PdsmExtPos->timeSrc = eLOC_ENG_TIME_SRC_TOW_AND_WEEK_CONFIRMED_V01;
        break;
      case PDSM_GNSS_TIME_SRC_NAV_SOLUTION:
        p_PdsmExtPos->timeSrc = eLOC_ENG_TIME_SRC_NAV_SOLUTION_V01;
        break;
      case PDSM_GNSS_TIME_SRC_SOLVE_FOR_TIME:
        p_PdsmExtPos->timeSrc = eLOC_ENG_TIME_SRC_SOLVE_FOR_TIME_V01;
        break;
      case PDSM_GNSS_TIME_SRC_GLO_TOW_DECODE:
        p_PdsmExtPos->timeSrc = eLOC_ENG_TIME_SRC_GLO_TOW_DECODE_V01;
        break;
      case PDSM_GNSS_TIME_SRC_TIME_TRANSFORM :
        p_PdsmExtPos->timeSrc = eLOC_ENG_TIME_SRC_TIME_TRANSFORM_V01;
        break;
      case PDSM_GNSS_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING:
        p_PdsmExtPos->timeSrc = eLOC_ENG_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING_V01;
        break;
      case PDSM_GNSS_TIME_SRC_GSM_SLEEP_TIME_TAGGING:
        p_PdsmExtPos->timeSrc = eLOC_ENG_TIME_SRC_GSM_SLEEP_TIME_TAGGING_V01;
        break;
      case PDSM_GNSS_TIME_SRC_QZSS_TOW_DECODE:
        p_PdsmExtPos->timeSrc = eLOC_ENG_TIME_SRC_QZSS_TOW_DECODE_V01;
        break;
      case  PDSM_GNSS_TIME_SRC_BDS_TOW_DECODE:
        p_PdsmExtPos->timeSrc = eLOC_ENG_TIME_SRC_BDS_TOW_DECODE_V01;
        break;
#if 0 //QZSS Phase 2
      case  PDSM_GNSS_TIME_SRC_QZSS_TOW_DECODE:
        p_PdsmExtPos->timeSrc = eLOC_ENG_TIME_SRC_QZSS_TOW_DECODE_V01;
        break;
#endif
      case  PDSM_GNSS_TIME_SRC_GAL_TOW_DECODE:
        p_PdsmExtPos->timeSrc = eLOC_ENG_TIME_SRC_GAL_TOW_DECODE_V01;
        break;




      default:
        p_PdsmExtPos->timeSrc = eLOC_ENG_TIME_SRC_INVALID_V01;
        break;
    }   
    if(p_PdsmExtPos->timeSrc != eLOC_ENG_TIME_SRC_INVALID_V01)
    {
      p_PdsmExtPos->timeSrc_valid = TRUE;
    }
    
    /*  Sensor Data Usage */  
    p_PdsmExtPos->sensorDataUsage.usageMask = p_GnssFixRpt->z_NavPos.w_SensorDataUsageMask;
    p_PdsmExtPos->sensorDataUsage.aidingIndicatorMask = p_GnssFixRpt->z_NavPos.w_SensorAidingIndicator;
    p_PdsmExtPos->sensorDataUsage_valid = TRUE;

    /*  SV's Used to Calculate the Fix */  
    //LOC_ENG_MAX_SV_USED_LIST_LENGTH_V01;

    count = 0;  /* up to 80 SVs allowed in QMI, won't overflow (16+14+16=46) */
  // Pack the SV used information ( GNSS and GLO SVs ) into the same array for the extended status callback 
    // GPS SVs used information
    for (i = 0 ; i < ARR_SIZE(p_GnssFixRpt->z_NavSvInfo.z_GpsSvInfo); i++)
    {
      if (p_GnssFixRpt->z_NavSvInfo.z_GpsSvInfo[i].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX)
    {
        p_PdsmExtPos->gnssSvUsedList[count++] = (uint16)p_GnssFixRpt->z_NavSvInfo.z_GpsSvInfo[i].w_SvId;
      }
    }
    // GLO SVs used information 
    for (i = 0 ; i < ARR_SIZE(p_GnssFixRpt->z_NavSvInfo.z_GloSvInfo); i++)
    {
      if (p_GnssFixRpt->z_NavSvInfo.z_GloSvInfo[i].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX)
      {
        p_PdsmExtPos->gnssSvUsedList[count++] = (uint16)p_GnssFixRpt->z_NavSvInfo.z_GloSvInfo[i].w_SvId;
      }
    }
    // BDS SVs used information
    for (i = 0 ; i < ARR_SIZE(p_GnssFixRpt->z_NavSvInfo.z_BdsSvInfo); i++)
    {
      if (p_GnssFixRpt->z_NavSvInfo.z_BdsSvInfo[i].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX)
    {
        p_PdsmExtPos->gnssSvUsedList[count++] = (uint16)p_GnssFixRpt->z_NavSvInfo.z_BdsSvInfo[i].w_SvId;
      }
    }

    // QZSS SVs used information
    for (i = 0 ; i < ARR_SIZE(p_GnssFixRpt->z_NavSvInfo.z_QzssSvInfo); i++)
    {
      if (p_GnssFixRpt->z_NavSvInfo.z_QzssSvInfo[i].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX)
    {
        p_PdsmExtPos->gnssSvUsedList[count++] = (uint16)p_GnssFixRpt->z_NavSvInfo.z_QzssSvInfo[i].w_SvId;
      }
    }

    // GAL SVs used information
    for (i = 0 ; i < ARR_SIZE(p_GnssFixRpt->z_NavSvInfo.z_GalSvInfo); i++)
    {
      if (p_GnssFixRpt->z_NavSvInfo.z_GalSvInfo[i].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX)
      {
        p_PdsmExtPos->gnssSvUsedList[count++] = (uint16)p_GnssFixRpt->z_NavSvInfo.z_GalSvInfo[i].w_SvId; 
      }
    }

    p_PdsmExtPos->gnssSvUsedList_len = count;
    p_PdsmExtPos->gnssSvUsedList_valid = TRUE;
  } // end of if (p_GnssFixRpt->z_NavPos.z_PosFlags.b_IsValid)
}

/*===========================================================================

FUNCTION tm_core_calculate_2d_punc

DESCRIPTION
  This function is used to calculate the 2D Punc from the
  gnss_NavSolutionStructType data.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
float tm_core_calculate_2d_punc(const sm_GnssFixRptStructType *pz_GnssFixRpt)
{
  float f_Punc = CGPS_WHOLE_EARTH_CEP99;
  if (pz_GnssFixRpt->z_NavPos.z_PosFlags.b_IsValid)
  {
  f_Punc = (FLT)sqrt(pz_GnssFixRpt->z_NavPos.f_ErrorEllipse[1] *
                     pz_GnssFixRpt->z_NavPos.f_ErrorEllipse[1] +
                     pz_GnssFixRpt->z_NavPos.f_ErrorEllipse[2] *
                     pz_GnssFixRpt->z_NavPos.f_ErrorEllipse[2]);
  }
  return f_Punc;
}

/*===========================================================================
tm_core_handle_gps_state_info_req_type_diag_pos

Description:
   This function is used to handle the GPS state information repsonse to the request made by
   1Hz Diag timer in SM. 

Parameters: 
  e_PosInfoSrc: Source of the position estimate from MGP. 
  p_gnss_fix_rpt: The position report from MGP. 

Return value: 
  Void.
=============================================================================*/
static void tm_core_handle_gps_state_info_req_type_diag_pos(sm_GnssFixRptStructType *p_gnss_fix_rpt)
{
  pdsm_ext_status_info_s_type   pd_ext_status_info;

  prtl_sess_info_param_u_type  *sess_info_param_ptr = NULL;

  prtl_sess_info_e_type   sess_info_type = PRTL_SESS_INFO_DIAG_INFO;
  pdsm_ext_status_info_s_type   pd_ext_status_info_pos_inj;
  float f_Punc;

  qword           milliseconds;
  qword           timestamp;
  uint32 q_gps_msec = 0;
  uint16 w_gps_week = 0;
  uint32 q_leap_msec = 0;

  boolean pos_inj_req = FALSE;

  //uint8 e_PosInfoSrc = 0;  /* Removing set-but-not-used variable */
  uint32 q_gps_state_flags = 0;
  gnss_NavSlnFlagsStructType z_saved_flags;

  memset( &pd_ext_status_info, 0, sizeof(pd_ext_status_info) );
  memset( &z_saved_flags,   0x00, sizeof(z_saved_flags) );

  f_Punc = tm_core_calculate_2d_punc(p_gnss_fix_rpt);

  /* The request for best available position came from the 1Hz Diag timer from SM */
    sess_info_param_ptr = (prtl_sess_info_param_u_type *)pdsm_getbuf(sizeof(prtl_sess_info_param_u_type));
    if(sess_info_param_ptr == NULL)
    {
      MSG_ERROR("Failed to get buffer for sess_info_param_ptr", 0, 0, 0);
      return;
    }

    memset(sess_info_param_ptr,0,sizeof(prtl_sess_info_param_u_type));

  /* Copy the fix report information received */
  memscpy(&sess_info_param_ptr->lr_data, sizeof(sess_info_param_ptr->lr_data), p_gnss_fix_rpt, sizeof(*p_gnss_fix_rpt));

  pd_ext_status_info.client_id = tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_DIAG_POS].q_ClientId = tm_core_info.session_info.pd_param.client_id;
    pd_ext_status_info.ext_status_type = PDSM_EXT_STATUS_POS_REPORT;

  /* Set the GPS state flags based on the information received */
  if (p_gnss_fix_rpt->z_NavPos.z_PosFlags.b_IsValid)
    {
      MSG_LOW("Pos valid", 0,0,0);
    q_gps_state_flags |= SM_GPS_STATE_POS_VALID;
    if (p_gnss_fix_rpt->z_NavPos.z_SmPosFlags.b_IsPuncValid)
    {
      q_gps_state_flags |= SM_GPS_STATE_PUNC_VALID;
    }

    /* The position is valid , so get the source of the position estimate */
    //e_PosInfoSrc = (p_gnss_fix_rpt->z_NavPos.z_PosFlags.b_IsExternal) ?  /* Removing set-but-not-used variable */
    //(uint8)C_POS_INFO_SRC_EXTERNAL : (uint8)C_POS_INFO_SRC_PE;

    /* Check the validity of TUNC */
    if (p_gnss_fix_rpt->z_NavPos.z_SmPosFlags.b_IsTuncValid)
      {
      q_gps_state_flags |= SM_GPS_STATE_TUNC_VALID;
      }

    /* Check if the GPS time is valid */
    if (p_gnss_fix_rpt->z_NavPos.z_SmPosFlags.b_IsGpsTimeValid)
      {
      q_gps_state_flags |= SM_GPS_STATE_TIME_VALID;
      }

    if (f_Punc > CGPS_WHOLE_EARTH_CEP99)
      {
        MSG_ERROR( "ME Reported PUNC greater than Earth Radius ",0, 0, 0);
      f_Punc = CGPS_WHOLE_EARTH_CEP99;
      }

    if (!(p_gnss_fix_rpt->z_NavPos.z_SmPosFlags.b_IsPuncValid))
      {
      f_Punc = CGPS_WHOLE_EARTH_CEP99;
      }

    if (!(p_gnss_fix_rpt->z_NavPos.z_SmPosFlags.b_IsGpsTimeValid))
    {
      (void) time_get( timestamp );
      gps_total_msec( timestamp, milliseconds );
      gps_week_ms( milliseconds, &(w_gps_week),
                  &(q_gps_msec) );
      /* Check for week rollover */
      gps_MsecWeekLimit((int32 *) &q_gps_msec, &w_gps_week );

      p_gnss_fix_rpt->z_NavPos.w_GpsWeek = w_gps_week;
      p_gnss_fix_rpt->z_NavPos.q_GpsTimeMs = q_gps_msec;

    q_leap_msec = 1000 *  cgps_GetNvUtcOffset();
    if (q_gps_msec < q_leap_msec )
    {
        p_gnss_fix_rpt->z_NavPos.q_UtcTimeMs = p_gnss_fix_rpt->z_NavPos.q_GpsTimeMs + (WEEK_MSECS - q_leap_msec);
    }
    else
    {
        p_gnss_fix_rpt->z_NavPos.q_UtcTimeMs = p_gnss_fix_rpt->z_NavPos.q_GpsTimeMs - q_leap_msec;
      }
    }
  } // end of if (p_gnss_fix_rpt->z_NavPos.z_PosFlags.b_IsValid)

    sess_info_type =  PRTL_SESS_INFO_DIAG_INFO;
    /* For now directly call tm_sysd_gnss inject data function.*/
    /* This should be replaced with prtl submodule call */
    if(TRUE == tm_core_info.qwip_sysd_triggered)
    {
      z_saved_flags = p_gnss_fix_rpt->z_NavPos.z_PosFlags;
//      p_gnss_fix_rpt->z_NavPos.z_PosFlags. = POS_VEL_FLAG_FIX_ME_GENERATED;
      sess_info_type =  PRTL_SESS_INFO_DIAG_INFO;
      tm_sysd_inject_gnss_lr_data(sess_info_param_ptr,
                                  &sess_info_type);
    }

    if((FALSE == tm_core_info.qwip_sysd_triggered)||
        (PRTL_SESS_INFO_DIAG_INFO == sess_info_type))
    {

      tm_core_xlate_pos_to_ext_status(&(pd_ext_status_info.ext_status_info.ext_pos_report_type),
      (sm_GnssFixRptStructType*)p_gnss_fix_rpt,
        tm_core_info.session_info.q_FixTimeRemainingMSec);

    // Set the time validity flags 
    if (!p_gnss_fix_rpt->z_NavPos.z_SmPosFlags.b_IsGpsTimeValid)
    {
      pd_ext_status_info.ext_status_info.ext_pos_report_type.pd_data.ext_opt_field_mask &= ~PDSM_PD_GPS_TIME_VALID;
      pd_ext_status_info.ext_status_info.ext_pos_report_type.pd_data.opt_field_mask &= ~PDSM_PD_MSEC_VALID;
      pd_ext_status_info.ext_status_info.ext_pos_report_type.pd_data.ext_opt_field_mask &= ~PDSM_PD_UTC_TIME_VALID;
    }

      // Indicate that the SV use in FIX report is invalid for Cell Id fixes . 
    pd_ext_status_info.ext_status_info.ext_pos_report_type.pd_data.z_NavSvInfo.u_SvInfoValid = FALSE;
    pd_ext_status_info.ext_status_info.ext_pos_report_type.pd_data.position_type = PDSM_PD_POSITION_TYPE_NEW;

    if (!p_gnss_fix_rpt->z_NavPos.z_PosFlags.b_IsValid || /* Position invalid */
        !p_gnss_fix_rpt->z_NavPos.z_PosFlags.b_IsVelValid)
      {
         /* Sysd Modified the diag info data with wiper data*/
      pd_ext_status_info.ext_status_info.ext_pos_report_type.pd_data.positioning_source = PDSM_PD_LOCATION_SOURCE_HYBRID;
      }
      else
      {
         MSG_MED("Cell db position is propagated fix",0,0,0);
      p_gnss_fix_rpt->z_NavPos.z_PosFlags = z_saved_flags;
      pd_ext_status_info.ext_status_info.ext_pos_report_type.pd_data.positioning_source = PDSM_PD_POSITION_SOURCE_UNKNOWN;
      }

    if (p_gnss_fix_rpt->z_NavPos.z_PosFlags.b_IsValid)
      {
        /* request a position injection if punc is > 10k */
      if ((uint32)f_Punc > TM_MAX_PUNC_VALUE_FOR_POS_INJ_REQ)
        {
          pos_inj_req = TRUE;
        }
      }
    else
    {
      pos_inj_req = TRUE;
    }
      pd_ext_status_info.client_id =  tm_core_info.session_info.pd_param.client_id;

      /* if MSB and pos_inj_req is true, save the data so if the netwrok assistance failes, we
         can ask the app to inject a position */
      if((pos_inj_req == TRUE) &&
         (tm_core_info.session_info.op_mode == TM_SESS_OPERATION_MODE_MSB) &&
         (!(tm_core_info.session_info.sess_info_valid_flags & TM_CORE_SESS_INFO_CACHED_INFO_STATE_VALID)))
      {
      tm_core_info.gps_state_info.flags = q_gps_state_flags;
      tm_core_info.gps_state_info.lat = (int32)(p_gnss_fix_rpt->z_NavPos.d_PosLla[0] * C_2_32_OVER_PI);
      tm_core_info.gps_state_info.lon = (int32)(p_gnss_fix_rpt->z_NavPos.d_PosLla[1] * C_2_31_OVER_PI);
      tm_core_info.gps_state_info.pos_unc = (uint32)f_Punc;

      tm_core_info.gps_state_info.timestamp_gps_week   = p_gnss_fix_rpt->z_NavPos.w_GpsWeek;
      tm_core_info.gps_state_info.timestamp_tow_ms     = p_gnss_fix_rpt->z_NavPos.q_UtcTimeMs;
        tm_core_info.session_info.sess_info_valid_flags |= TM_CORE_SESS_INFO_CACHED_INFO_STATE_VALID;
      }

      /* ask for a position injection in Standalone if the PUNC is > 10km */
      if (tm_core_info.session_info.pos_inj_requested == FALSE &&
          tm_core_info.session_info.op_mode == TM_SESS_OPERATION_MODE_STANDALONE )
      {
        /* check if a position injection is required from the APPS. */
        if (pos_inj_req == TRUE)
        {
          memset(&pd_ext_status_info_pos_inj, 0, sizeof(pd_ext_status_info_pos_inj));
          pd_ext_status_info_pos_inj.ext_status_type = PDSM_EXT_STATUS_POS_INJECT_REQUEST;
        pd_ext_status_info_pos_inj.client_id =  tm_core_info.session_info.pd_param.client_id;

          pd_ext_status_info_pos_inj.ext_status_info.ext_pos_inj_req_type.flags = PDSM_PD_EXT_POS_INJ_LAT_LONG_VALID;
          pd_ext_status_info_pos_inj.ext_status_info.ext_pos_inj_req_type.flags |= PDSM_PD_EXT_POS_INJ_POS_UNC_VALID;
          pd_ext_status_info_pos_inj.ext_status_info.ext_pos_inj_req_type.lat = (int32)(p_gnss_fix_rpt->z_NavPos.d_PosLla[0] * C_2_32_OVER_PI);
          pd_ext_status_info_pos_inj.ext_status_info.ext_pos_inj_req_type.lon = (int32)(p_gnss_fix_rpt->z_NavPos.d_PosLla[1] * C_2_31_OVER_PI);
        pd_ext_status_info_pos_inj.ext_status_info.ext_pos_inj_req_type.pos_unc = (uint32)f_Punc;

          if (p_gnss_fix_rpt->z_NavPos.z_SmPosFlags.b_IsGpsTimeValid)
          {
            pd_ext_status_info_pos_inj.ext_status_info.ext_pos_inj_req_type.flags |= PDSM_PD_EXT_POS_INJ_TIME_STAMP_VALID;
            pd_ext_status_info_pos_inj.ext_status_info.ext_pos_inj_req_type.timestamp_gps_week = p_gnss_fix_rpt->z_NavPos.w_GpsWeek;
            pd_ext_status_info_pos_inj.ext_status_info.ext_pos_inj_req_type.timestamp_tow_ms = p_gnss_fix_rpt->z_NavPos.q_GpsTimeMs;
          }
          MSG_HIGH("REQUEST POSITION INJECTION",0,0,0);
          tm_pdapi_ext_status_event_callback(PDSM_EXT_STATUS_POS_INJECT_REQUEST,
             &pd_ext_status_info_pos_inj);
          tm_core_info.session_info.pos_inj_requested =  TRUE;
        }
      }
      pd_ext_status_info.ext_status_type = PDSM_EXT_STATUS_POS_REPORT;
      if ((z_cached_ref_pos_info.cached_ref_pos_valid == TRUE) &&
          (z_cached_ref_pos_info.ref_pos_reported == FALSE))
      {
        tm_pdapi_ext_status_event_callback(PDSM_EXT_STATUS_POS_REPORT,
           &z_cached_ref_pos_info.pd_ext_status_info);
        z_cached_ref_pos_info.ref_pos_reported = TRUE;
      }
      else
      {
        tm_pdapi_ext_status_event_callback(PDSM_EXT_STATUS_POS_REPORT,
           &pd_ext_status_info);
      }
    }
    (void) pdsm_freebuf((char *)sess_info_param_ptr);

  tm_core_info.session_info.q_ReqClientMask &= ~(TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_DIAG_POS));
  tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_DIAG_POS].w_TmRcvdDataMask = 0;
  }

/*===========================================================================
tm_core_internal_update_gps_state_data

Description:
   This function is used to update the internal state of TM's GPS state info data.

Parameters: 
  p_gnss_fix_rpt: The position report from MGP.  

Return value: 
  Void.
=============================================================================*/
void tm_core_internal_update_gps_state_data(const sm_GnssFixRptStructType *p_gnss_fix_rpt)
  {
  float f_Punc = 0;

  f_Punc = tm_core_calculate_2d_punc(p_gnss_fix_rpt);
     /* Convert latitude from radians to degrees and scale. */
  if (p_gnss_fix_rpt->z_NavPos.z_PosFlags.b_IsValid)
    {
       tm_core_info.gps_state_info.flags |= PDSM_PD_EXT_GPS_STATE_POS_VALID;
    }

  if (p_gnss_fix_rpt->z_NavPos.f_PuncVertMeters < 10000)
    {
    tm_core_info.gps_state_info.loc_unc_vertical = (uint16)p_gnss_fix_rpt->z_NavPos.f_PuncVertMeters;
      tm_core_info.gps_state_info.flags |= PDSM_PD_EXT_GPS_STATE_ALT_VALID;
    }
    else
    {
      tm_core_info.gps_state_info.loc_unc_vertical = 10000;
    }
  tm_core_info.gps_state_info.lat = (int32)( p_gnss_fix_rpt->z_NavPos.d_PosLla[0] * C_2_32_OVER_PI);

  /* Convert longitude from radians to degrees and scale */
  tm_core_info.gps_state_info.lon = (int32)( p_gnss_fix_rpt->z_NavPos.d_PosLla[1] * C_2_31_OVER_PI);
  tm_core_info.gps_state_info.pos_unc  = (uint32)f_Punc;
  tm_core_info.gps_state_info.altitude = (int16)(tm_util_nearest_long(p_gnss_fix_rpt->z_NavPos.d_PosLla[2]));
    tm_core_info.gps_state_info.engine_state = tm_core_info.e_cached_rcvr_state;

  tm_core_info.gps_state_info.loc_unc_vertical = (uint16)(p_gnss_fix_rpt->z_NavPos.f_PuncVertMeters);
  if (p_gnss_fix_rpt->z_NavPos.z_SmPosFlags.b_IsGpsTimeValid)
    {
      tm_core_info.gps_state_info.flags |= PDSM_PD_EXT_GPS_STATE_TIME_MS_VALID;
      tm_core_info.gps_state_info.flags |= PDSM_PD_EXT_GPS_STATE_TIME_WN_VALID;
    }
  tm_core_info.gps_state_info.timestamp_gps_week = p_gnss_fix_rpt->z_NavPos.w_GpsWeek;
  tm_core_info.gps_state_info.timestamp_tow_ms = p_gnss_fix_rpt->z_NavPos.q_UtcTimeMs;

  if (p_gnss_fix_rpt->z_NavPos.z_SmPosFlags.b_IsTuncValid)
    {
      tm_core_info.gps_state_info.flags |= PDSM_PD_EXT_GPS_STATE_TUNC_VALID;
    }
  tm_core_info.gps_state_info.time_unc_us = (uint32)(p_gnss_fix_rpt->z_NavPos.f_ClockBiasUncMs);
}

/*===========================================================================
tm_core_handle_gps_state_info_req_type_query_state

Description:
   This function is used to handle the GPS state information repsonse to the request made by
   querying from PDAPI. 

Parameters: 
  p_gnss_fix_rpt: The position report from MGP.  

Return value: 
  Void.
=============================================================================*/
static void tm_core_handle_gps_state_info_req_type_query_state(const sm_GnssFixRptStructType *p_gnss_fix_rpt)
{
  pdsm_ext_status_info_s_type  pd_ext_status_info;
  tm_core_internal_update_gps_state_data(p_gnss_fix_rpt);

  if (tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE].w_TmRcvdDataMask == TM_RCVD_ALL)
    {
        pd_ext_status_info.ext_status_type = PDSM_EXT_STATUS_GPS_STATE_INFO;
        tm_core_xlate_gps_state_to_ext_status(&tm_core_info.gps_state_info,
          &pd_ext_status_info.ext_status_info.ext_gps_state_type);
        tm_pdapi_ext_status_event_callback(PDSM_EXT_STATUS_GPS_STATE_INFO, &pd_ext_status_info);
    tm_core_info.session_info.q_ReqClientMask &= ~(TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE));
    tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE].w_TmRcvdDataMask = 0;
      }
}

/*===========================================================================
tm_core_handle_gps_state_info_req_type_on_demand

Description:
   This function is used to handle the GPS state information repsonse to the request made by
   on demand. 

Parameters: 
  p_gnss_fix_rpt: The position report from MGP.  

Return value: 
  Void.
=============================================================================*/
static void tm_core_handle_gps_state_info_req_type_on_demand(const sm_GnssFixRptStructType *p_gnss_fix_rpt)
      {
  sm_OnDemandPosPuncTuncStruct on_demand_info;
  memset( &on_demand_info, 0, sizeof(on_demand_info) );
  tm_core_internal_update_gps_state_data(p_gnss_fix_rpt);

  if (tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_ON_DEMAND].w_TmRcvdDataMask == TM_RCVD_ALL)
        {
          tm_core_xlate_gps_state_to_on_demand_info(&tm_core_info.gps_state_info, &on_demand_info);
    tm_core_info.session_info.q_ReqClientMask &= ~(TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_ON_DEMAND));
    tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_ON_DEMAND].w_TmRcvdDataMask = 0;
        }
      }

/*===========================================================================
tm_core_handle_gps_state_info_req_type_wifi

Description:
   This function is used to handle the GPS state information repsonse to the request made by
   Wifi module ( QWIP )  

Parameters: 
  p_gnss_fix_rpt: The position report from MGP. 

Return value: 
  Void.
=============================================================================*/
static void tm_core_handle_gps_state_info_req_type_wifi(const sm_GnssFixRptStructType *p_gnss_fix_rpt)
  {
  float f_Punc;
  sysd_cntrl_func_packet_type sysd_cntrl_payload;
  tm_SysdPosPuncTunc SysdPosPuncTunc;

  f_Punc = tm_core_calculate_2d_punc(p_gnss_fix_rpt);
  tm_core_info.gps_state_info.time_unc_us = (uint32)(p_gnss_fix_rpt->z_NavPos.f_ClockBiasUncMs);
  tm_core_info.gps_state_info.pos_unc  = (uint32)(f_Punc);
    tm_core_info.gps_state_info.engine_state = tm_core_info.e_cached_rcvr_state;
  tm_core_info.gps_state_info.timestamp_gps_week = p_gnss_fix_rpt->z_NavPos.w_GpsWeek;
  tm_core_info.gps_state_info.timestamp_tow_ms = p_gnss_fix_rpt->z_NavPos.q_UtcTimeMs;

  if (p_gnss_fix_rpt->z_NavPos.z_SmPosFlags.b_IsTuncValid)
    {
      tm_core_info.gps_state_info.flags |= PDSM_PD_EXT_GPS_STATE_TUNC_VALID;
    }

  if (tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_PUNC_TUNC_FOR_WIFI].w_TmRcvdDataMask == TM_RCVD_ALL)
    {
    SysdPosPuncTunc.f_Punc = f_Punc;
    SysdPosPuncTunc.f_Tunc = p_gnss_fix_rpt->z_NavPos.f_ClockBiasUncMs;

      if ((tm_core_info.gps_state_info.flags & PDSM_PD_EXT_GPS_STATE_POS_VALID)&&
        (tm_core_info.gps_state_info.flags & PDSM_PD_EXT_GPS_STATE_TUNC_VALID))
      {
        SysdPosPuncTunc.b_Valid = TRUE;
      }
      else
      {
        SysdPosPuncTunc.b_Valid = FALSE ;
      }

      sysd_cntrl_payload.cmd.p_SysdPosPuncTunc = &SysdPosPuncTunc;
      sysd_cntrl_payload.event = SYSD_CNTRL_EVT_GPS_STATE_INFO;
      sysd_cntrl_payload.req_type = SYSD_CNTRL_POS_REQ_TYPE_PDAPI;
      tm_sysd_controller_handler(&sysd_cntrl_payload);

    tm_core_info.session_info.q_ReqClientMask &= ~(TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_GET_PUNC_TUNC_FOR_WIFI));
    tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_PUNC_TUNC_FOR_WIFI].w_TmRcvdDataMask = 0;
    }
  }

/*===========================================================================
tm_core_handle_gps_state_info_req_type_best_avail_pos_qmi

Description:
   This function is used to handle the GPS state information repsonse to the request made for
   best available position from  PDAPI.

Parameters: 
  p_gnss_fix_rpt: The position report from MGP. 

Return value: 
  Void.
=============================================================================*/
static void tm_core_handle_gps_state_info_req_type_best_avail_pos_qmi(const sm_GnssFixRptStructType *p_gnss_fix_rpt)
{
  pdsm_ext_status_info_s_type  pd_ext_status_info;
  pdsm_pd_cmd_err_e_type      e_cmd_err = PDSM_PD_CMD_ERR_NOERR;
  //pdsm_cmd_s_type *p_cmd_type;
#ifdef FEATURE_CGPS_XTRA_T
  float                       f_Punc = tm_core_calculate_2d_punc(p_gnss_fix_rpt);
#endif

  // This is in response from ME to the get best available position request from PDAPI .
  // Convert the data into the format of QMI and send it as an extended status event callback.
  pd_ext_status_info.client_id       = tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_QMI].q_ClientId;
  pd_ext_status_info.ext_status_type = PDSM_EXT_STATUS_BEST_AVAIL_POS_REPORT;
  tm_core_xlate_pd_ext_pos_to_qmi( p_gnss_fix_rpt,
                                   &pd_ext_status_info.ext_status_info.ext_pos_report_qmi_type);
  e_cmd_err = tm_core_get_best_avail_pos_err_check();
  if(e_cmd_err == PDSM_PD_CMD_ERR_NOERR)
  {
    tm_pdapi_ext_status_event_callback(PDSM_EXT_STATUS_BEST_AVAIL_POS_REPORT, &pd_ext_status_info);
#ifdef FEATURE_CGPS_XTRA_T 
    tle_StartKeepWarmPeriod();
    MSG_MED("tm_core PUNC:%u", (uint32) f_Punc, 0, 0);
    if (f_Punc > PUNC_THRESHOLD_FOR_XTRA_T)
    {
      MSG_MED("tm_core: initiate a GTP session", 0, 0, 0);
      // Punc is greater than 25 Km.
      // Start the Xtra-T download session.  
      tle_StartPuncBasedSession(f_Punc); 
    }
    else
    {
      MSG_MED("tm_core: No need to initiate GTP session", 0, 0, 0);
    }
#endif    
  }
  else
  {
    // Notify the client that there was an error
    //tm_pd_client_cmd_status( p_cmd_type, e_cmd_err );
  }

  tm_core_info.session_info.q_ReqClientMask &= ~(TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_QMI));
  tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_QMI].w_TmRcvdDataMask = 0;
}

/*===========================================================================
tm_core_handle_gps_state_info_req_type_avail_wwan_pos_qmi

Description:
   This function is used to handle the GPS state information repsonse to the request made for
   available WWAN position from PDAPI.

Parameters:
  p_gnss_fix_rpt: The position report from MGP.

Return value:
  Void.
=============================================================================*/
static void tm_core_handle_gps_state_info_req_type_avail_wwan_pos_qmi(const sm_GnssFixRptStructType *p_gnss_fix_rpt)
{
  pdsm_ext_status_info_s_type pd_ext_status_info;
  pdsm_pd_cmd_err_e_type      e_cmd_err = PDSM_PD_CMD_ERR_NOERR;
#ifdef FEATURE_CGPS_XTRA_T
  float                       f_Punc = tm_core_calculate_2d_punc(p_gnss_fix_rpt);
#endif

  // This is in response from ME to the get available WWAN position request from PDAPI.
  // Convert the data into the format of QMI and send it as an extended status event callback.
  pd_ext_status_info.client_id       = tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_QMI].q_ClientId;
  pd_ext_status_info.ext_status_type = PDSM_EXT_STATUS_AVAIL_WWAN_POS_REPORT;
  tm_core_xlate_pd_ext_pos_to_qmi( p_gnss_fix_rpt,
                                   &pd_ext_status_info.ext_status_info.ext_pos_report_qmi_type);
  e_cmd_err = tm_core_get_best_avail_pos_err_check();
  if (e_cmd_err == PDSM_PD_CMD_ERR_NOERR)
  {
    tm_pdapi_ext_status_event_callback(PDSM_EXT_STATUS_AVAIL_WWAN_POS_REPORT, &pd_ext_status_info);
#ifdef FEATURE_CGPS_XTRA_T
    tle_StartKeepWarmPeriod();
    MSG_MED("tm_core PUNC:%u", (uint32) f_Punc, 0, 0);
    if (f_Punc > PUNC_THRESHOLD_FOR_XTRA_T)
    {
      // Punc is greater than 25 Km.
      // Start the Xtra-T download session.
      MSG_MED("tm_core: initiate a GTP session", 0, 0, 0);
      tle_StartPuncBasedSession(f_Punc);
    }
    else
    {
      MSG_MED("tm_core: No need to initiate GTP session", 0, 0, 0);
    }
#endif    
  }

  tm_core_info.session_info.q_ReqClientMask &= ~(TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_QMI));
  tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_QMI].w_TmRcvdDataMask = 0;
}
/*===========================================================================
tm_core_handle_gps_state_info_req_type_available_pos_qmi

Description:
   This function is used to handle the GPS state information repsonse to the request made for
   available GNSS position from PDAPI.

Parameters:
  p_gnss_fix_rpt: The position report from MGP.
  b_ReqType:    The position report source type from MGP.

Return value:
  Void.
=============================================================================*/
static void tm_core_handle_gps_state_info_req_type_available_pos_qmi(const sm_GnssFixRptStructType *p_gnss_fix_rpt,sm_GpsStateInfoReqEnumType b_ReqType)
{
  pdsm_ext_status_info_s_type pd_ext_status_info = {0};
  pdsm_pd_cmd_err_e_type      e_cmd_err = PDSM_PD_CMD_ERR_NOERR;
  
  if (NULL == p_gnss_fix_rpt)
  {
    MSG_ERROR("tm_core_handle_gps_state_info_req_type_available_pos_qmi: p_gnss_fix_rpt NULL argument received", 0, 0, 0);
    return;
  }
  
#ifdef FEATURE_CGPS_XTRA_T
  float  f_Punc = tm_core_calculate_2d_punc(p_gnss_fix_rpt);
#endif

  // This is in response from ME to the get available position request from PDAPI.
  // Convert the data into the format of QMI and send it as an extended status event callback.
  pd_ext_status_info.client_id       = tm_core_info.session_info.z_ReqClients[b_ReqType].q_ClientId;
  pd_ext_status_info.ext_status_type = PDSM_EXT_STATUS_AVAIL_POS_REPORT;
  tm_core_xlate_pd_ext_pos_to_qmi( p_gnss_fix_rpt,
                                   &pd_ext_status_info.ext_status_info.ext_pos_report_qmi_type);
  e_cmd_err = tm_core_get_best_avail_pos_err_check();
  if (e_cmd_err == PDSM_PD_CMD_ERR_NOERR)
  {
    tm_pdapi_ext_status_event_callback(PDSM_EXT_STATUS_AVAIL_POS_REPORT, &pd_ext_status_info);
#ifdef FEATURE_CGPS_XTRA_T
    tle_StartKeepWarmPeriod();
    MSG_MED("tm_core PUNC:%u", (uint32) f_Punc, 0, 0);
    if (f_Punc > PUNC_THRESHOLD_FOR_XTRA_T)
    {
      // Punc is greater than 25 Km.
      // Start the Xtra-T download session.
      MSG_MED("tm_core: initiate a GTP session", 0, 0, 0);
      tle_StartPuncBasedSession(f_Punc);
    }
    else
    {
      MSG_MED("tm_core: No need to initiate GTP session", 0, 0, 0);
    }
#endif    
  }

  tm_core_info.session_info.q_ReqClientMask &= ~(TM_CORE_CLIENT_BITMASK(b_ReqType));
  tm_core_info.session_info.z_ReqClients[b_ReqType].w_TmRcvdDataMask = 0;
}

/*===========================================================================

FUNCTION tm_core_report_gps_state_info

DESCRIPTION
  This function is called by MGP to return the gps state info 
  for on-demand positioning and user requested state info
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_report_gps_state_info(void *pos_data)
{
  sm_PositionReportStructType z_GpsBestAvailPos;
  sm_GnssFixRptStructType*    p_SmGnssFixRpt = &z_GpsBestAvailPos.z_SmGnssFixRpt;
  //float f_Punc;  /* Remove set-but-not-used variable */

  // suds84. Call the function to convert from gnss_NavSolutionStructType to sm_GnssFixRptStructType

  /* Function arguments sanity check */
  if (NULL == pos_data)
  {
    MSG_ERROR("tm_core_report_gps_state_info: NULL argument received", 0, 0, 0);
    return;
  }

  /* Copy contents of pointer argument passed by MGP before it gets overwritten */
  z_GpsBestAvailPos = *((sm_PositionReportStructType*)pos_data);

  MSG_MED("tm_core_report_gps_state_info: Rcvd response for request %d from MGP", z_GpsBestAvailPos.b_ReqType, 0, 0);

  tm_core_info.session_info.z_ReqClients[z_GpsBestAvailPos.b_ReqType].w_TmRcvdDataMask |= TM_RCVD_TUNC_FROM_ME;
#ifndef FEATURE_CGPS_ODP_2_0
  tm_core_info.session_info.z_ReqClients[z_GpsBestAvailPos.b_ReqType].w_TmRcvdDataMask |= TM_RCVD_POS_FROM_PE;
#endif

  //f_Punc = tm_core_calculate_2d_punc(p_SmGnssFixRpt);  /* Remove set-but-not-used variable */

  switch(z_GpsBestAvailPos.b_ReqType)
  {
    case SM_GPS_STATE_INFO_REQ_TYPE_DIAG_POS:
      MSG_MED("Gps state info for Diag Pos recv. ", 0,0,0);
      tm_core_handle_gps_state_info_req_type_diag_pos(p_SmGnssFixRpt);
      break;
    case SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE:
      MSG_MED("Gps state info for query state recv. ", 0,0,0);
      tm_core_handle_gps_state_info_req_type_query_state(p_SmGnssFixRpt);
      break;
    case SM_GPS_STATE_INFO_REQ_TYPE_ON_DEMAND:
      MSG_MED("Gps state info for on demand recv. ", 0,0,0);
      tm_core_handle_gps_state_info_req_type_on_demand(p_SmGnssFixRpt);
      break;
    case SM_GPS_STATE_INFO_REQ_TYPE_GET_PUNC_TUNC_FOR_WIFI:
      MSG_MED("Gps state info for Wifi recv. ", 0,0,0);
      tm_core_handle_gps_state_info_req_type_wifi(p_SmGnssFixRpt);
      break;
    case SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_QMI:
      MSG_MED("Gps state info for Best avail Pos recv for QMI. ", 0,0,0);
      tm_core_handle_gps_state_info_req_type_best_avail_pos_qmi(p_SmGnssFixRpt);
      break;
    case SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_QMI:
      MSG_MED("Gps state info for Avail WWAN Pos recv for QMI. ", 0,0,0);
      tm_core_handle_gps_state_info_req_type_avail_wwan_pos_qmi(p_SmGnssFixRpt);
      break;
  case SM_GPS_STATE_INFO_REQ_TYPE_GET_GNSS_AVAIL_POS_QMI:
      MSG_MED("Gps state info for Avail Pos recv for QMI. ", 0,0,0);
   	  tm_core_handle_gps_state_info_req_type_available_pos_qmi(p_SmGnssFixRpt,z_GpsBestAvailPos.b_ReqType);
      break;
#ifdef FEATURE_CGPS_XTRA_T      
    case SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_XTRA_T:
      MSG_MED("Gps state info for Best avail Pos recv for XTRA_T. ", 0,0,0);
      tm_xtra_t_handle_best_avail_pos(p_SmGnssFixRpt);

      tm_core_info.session_info.q_ReqClientMask &= ~(TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_XTRA_T));
      tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_XTRA_T].w_TmRcvdDataMask = 0;
      break;      
#endif      
    default:
      MSG_ERROR("Get GPS State unknown Req Type %d",z_GpsBestAvailPos.b_ReqType,0,0);
      break;
  }
}

/*===========================================================================

FUNCTION tm_core_on_demand_pos_punc_tunc_proc

DESCRIPTION
  This function is called by MGP to return the position and its associated punc
  tunc for on-demand positioning

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void  tm_core_on_demand_pos_punc_tunc_proc( void *pos_data)
{

  int prtlTableIndex;

  prtl_event_u_type event_payload;

  event_payload.on_demand_event_payload.event_id = PRTL_ON_DEMAND_POS_PUNC_TUNC;

  memscpy(&event_payload.on_demand_event_payload.event_payload, sizeof(event_payload.on_demand_event_payload.event_payload),
          pos_data, sizeof(sm_OnDemandPosPuncTuncStruct));
  prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_STANDALONE);
  if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
  {
    if(tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL)
    {
      (void) tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp( PRTL_EVENT_TYPE_ON_DEMAND_POS,
                                                                 &event_payload);
    }
  }
}



/*===========================================================================

FUNCTION tm_core_on_demand_charger_connected

DESCRIPTION
 this function is called when the battery charger is connected

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void  tm_core_on_demand_charger_connected( void )
{

  int prtlTableIndex;

  prtl_event_u_type event_payload;

  event_payload.on_demand_event_payload.event_id = PRTL_ON_DEMAND_BATTERY_STATE;

  event_payload.on_demand_event_payload.event_payload.battery_state = PRTL_EVENT_ON_DEMAND_CHARGER_CONNECTED;
  prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_STANDALONE);
  if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
  {
    if(tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL)
    {
      (void) tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp( PRTL_EVENT_TYPE_ON_DEMAND_POS,
                                                                 &event_payload);
    }
  }
}





/*===========================================================================

FUNCTION tm_core_on_demand_charger_disconnected

DESCRIPTION
 this function is called when the battery charger is disconnected
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void  tm_core_on_demand_charger_disconnected( void )
{

  int prtlTableIndex;

  prtl_event_u_type event_payload;

  event_payload.on_demand_event_payload.event_id = PRTL_ON_DEMAND_BATTERY_STATE;

  event_payload.on_demand_event_payload.event_payload.battery_state = PRTL_EVENT_ON_DEMAND_CHARGER_DISCONNECTED;
  prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_STANDALONE);
  if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
  {
    if(tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL)
    {
      (void) tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp( PRTL_EVENT_TYPE_ON_DEMAND_POS,
                                                                 &event_payload);
    }
  }
}


/*===========================================================================

FUNCTION tm_core_on_demand_charger_connected

DESCRIPTION
 this function is called when the battery charger is connected and phone is
 fully charged

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void  tm_core_on_demand_charger_charged( void )
{

  int prtlTableIndex;

  prtl_event_u_type event_payload;

  event_payload.on_demand_event_payload.event_id = PRTL_ON_DEMAND_BATTERY_STATE;

  event_payload.on_demand_event_payload.event_payload.battery_state = PRTL_EVENT_ON_DEMAND_CHARGER_DONE;
  prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_STANDALONE);
  if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
  {
    if(tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL)
    {
      (void) tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp( PRTL_EVENT_TYPE_ON_DEMAND_POS,
                                                                 &event_payload);
    }
  }
}


/*===========================================================================

FUNCTION tm_core_on_demand_abort

DESCRIPTION
 this function is called when tm core wants to abort an on-demand session
 fully charged

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_on_demand_abort(tm_sess_stop_reason_e_type reason)
{

  int               prtlTableIndex;
  prtl_event_u_type event_payload;

  /* stop any on-demand timers even though no on-demand session is active */
  if (tm_core_get_on_demand_state() == TRUE )
  {
    prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_STANDALONE);
    if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
    {
      if(tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL)
      {
        event_payload.on_demand_event_payload.event_id = PRTL_ON_DEMAND_ABORT;
        event_payload.on_demand_event_payload.event_payload.reason = reason;
        (void) tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(PRTL_EVENT_TYPE_ON_DEMAND_POS,
                                                                   &event_payload);
      }
    }
  }
}

/*===========================================================================

FUNCTION tm_core_get_nv_utc_offset

DESCRIPTION
  This function returns the nv value of gps UTC offset

RETURN VALUE
  uint8 call flow select mask

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_get_nv_utc_offset(uint8 *p_utc_offset)
{
  *p_utc_offset = tm_core_info.config_info.nv_gps_utc_offset;
  return;
}

/*===========================================================================

FUNCTION tm_core_get_call_flow_select_mask

DESCRIPTION
  This function returns the UMTS CP MOLR vs AD call flow select mask in TM data

RETURN VALUE
  uint8 call flow select mask

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
uint8 tm_core_get_call_flow_select_mask(void)
{
  return (tm_core_info.config_info.call_flow_select_mask);
}


/*===========================================================================

FUNCTION tm_core_get_gps_measurement_data

DESCRIPTION
  This function returns the cachec gps measurement block

RETURN VALUE
  uint8 call flow select mask

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_get_gps_measurement_data(gnss_MeasBlkStructType *gps_meas_blk)
{
  *gps_meas_blk = tm_core_info.gps_meas_blk;
}

/*===========================================================================

FUNCTION tm_core_save_ref_position

DESCRIPTION
  This function saves the reference position received from the network

RETURN VALUE


DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_save_ref_position( pdsm_ext_status_info_s_type   *pd_ext_status_info)
{

  z_cached_ref_pos_info.cached_ref_pos_valid = TRUE;
  memscpy(&z_cached_ref_pos_info.pd_ext_status_info, sizeof(z_cached_ref_pos_info.pd_ext_status_info),
          pd_ext_status_info, sizeof(*pd_ext_status_info));

  z_cached_ref_pos_info.ref_pos_reported = FALSE;
}

/*===========================================================================

FUNCTION tm_core_get_efs_supl_cert_status

DESCRIPTION
  This function tells the caller whether it must use SUPL Cert from EFS or not.

RETURN VALUE


DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_get_efs_supl_cert_status( void)
{
  return ( tm_use_efs_for_supl_cert );
}
/*===========================================================================

FUNCTION tm_core_set_efs_supl_cert_status

DESCRIPTION
 IF tm_use_efs_for_supl_cert == TRUE, supl session must read supl cert from EFS.

RETURN VALUE


DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_set_efs_supl_cert_status(boolean new_efs_cert_value )
{
  tm_use_efs_for_supl_cert = new_efs_cert_value;
}

/*===========================================================================

FUNCTION tm_core_get_current_dd_subs_id

DESCRIPTION
  This function returns the ID for current DD subscription

RETURN VALUE


DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
sys_modem_as_id_e_type tm_core_get_current_dd_subs_id( void)
{
  return( tm_current_dd_subs_id);
}
/*===========================================================================

FUNCTION tm_core_set_current_dd_subs_id

DESCRIPTION
  This function updates the stored value for current DD subscription

  This should be the only function to update TM's DD sub info, all triggering
  conditions should eventually lead to this function to update DD sub.
  (If not, carrier-specific EFS NVs may fail)
 
RETURN VALUE


DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_set_current_dd_subs_id( sys_modem_as_id_e_type asid)
{
  MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "DD sub being changed to %d (from %d)", asid, tm_current_dd_subs_id);

  tm_current_dd_subs_id = asid;

  /* set currently active subs-specific configuration after DD change */
  tm_core_set_current_subs_cfg(asid);
}


#ifdef FEATURE_MMGSDI_SESSION_LIB
/*====================================================================
FUNCTION tm_mmgsdi_set_session_id

DESCRIPTION

  This function updates the tm mmgsdi array with session id info for
  the given session type.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE


SIDE EFFECTS
  none

=====================================================================*/
void tm_mmgsdi_set_session_id
(
  mmgsdi_session_type_enum_type tm_mmgsdi_session_type,
  mmgsdi_session_id_type session_id
)
{
  int index;

  for(index = 0; index < sizeof(tm_mmgsdi_session_id_table)/sizeof(tm_mmgsdi_session_id_table_entry_s_type); index++)
  {
    if ( tm_mmgsdi_session_type == tm_mmgsdi_session_id_table[index].session_type )
    {
      tm_mmgsdi_session_id_table[index].session_id = session_id;
      tm_mmgsdi_session_id_table[index].session_id_set = TRUE;
      break;
    }
  }
} /* tm_mmgsdi_set_session_id */

/*====================================================================
FUNCTION tm_mmgsdi_get_session_id_status

DESCRIPTION

  This function informs whether the session id for a given
  mmgsdi session type has been set or not.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  TRUE : Session ID was programmed earlier
  FALSE : Session ID not programmed.

SIDE EFFECTS
  none

=====================================================================*/
boolean tm_mmgsdi_get_session_id_status
(
  mmgsdi_session_type_enum_type tm_mmgsdi_session_type
)
{
  int index;
  boolean status = FALSE;

  for(index = 0; index < sizeof(tm_mmgsdi_session_id_table)/sizeof(tm_mmgsdi_session_id_table_entry_s_type); index++)
  {
    if ( tm_mmgsdi_session_type == tm_mmgsdi_session_id_table[index].session_type )
    {
      status = tm_mmgsdi_session_id_table[index].session_id_set;
      break;
    }
  }
  return (status);
} /* tm_mmgsdi_get_session_id_status */

/*====================================================================
FUNCTION tm_mmgsdi_close_session_id

DESCRIPTION

  This function close a particular session id and reset its status

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  TRUE : Session ID was closed successfully
  FALSE : Session ID not found

SIDE EFFECTS
  none

=====================================================================*/
boolean tm_mmgsdi_close_session_id
(
   mmgsdi_session_id_type session_id
)
{
  int index;

  for(index = 0; index < sizeof(tm_mmgsdi_session_id_table)/sizeof(tm_mmgsdi_session_id_table_entry_s_type); index++)
  {
    if (session_id == tm_mmgsdi_session_id_table[index].session_id) 
    {
      tm_mmgsdi_session_id_table[index].session_id = 0; // reset to 0
      tm_mmgsdi_session_id_table[index].session_id_set = FALSE;
      return TRUE;
    }
  }

  return FALSE;
}

/*====================================================================
FUNCTION tm_mmgsdi_get_session_id_ptr

DESCRIPTION

  This function returns a pointer to the session ID for the specified
  session type.  At most one session ID is associated with each session
  type.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  TRUE : If session id was found
  FALSE : Session id for this session type not found.

SIDE EFFECTS
  none

=====================================================================*/
boolean tm_mmgsdi_get_session_id_ptr
(
  mmgsdi_session_type_enum_type tm_mmgsdi_session_type,
  mmgsdi_session_id_type     ** tm_mmgsdi_session_id_ptr
)
{
  int index;

  for(index = 0; index < sizeof(tm_mmgsdi_session_id_table)/sizeof(tm_mmgsdi_session_id_table_entry_s_type); index++)
  {
    if ( tm_mmgsdi_session_type == tm_mmgsdi_session_id_table[index].session_type ) 
    {
          *tm_mmgsdi_session_id_ptr = &(tm_mmgsdi_session_id_table[index].session_id);
          return TRUE;
    }
  }

  return FALSE;

} /* tm_mmgsdi_get_session_id_ptr */

/*====================================================================
FUNCTION tm_mmgsdi_get_sim_info_from_asid

DESCRIPTION

  This function returns a pointer to the session ID for the specified
  session type.  At most one session ID is associated with each session
  type.


DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  TRUE : If session id was found
  FALSE : Session id for this session type not found.

SIDE EFFECTS
  none

=====================================================================*/
boolean tm_mmgsdi_get_sim_info_from_asid
(
  sys_modem_as_id_e_type      asubs_id,
  mmgsdi_session_id_type     *tm_mmgsdi_session_id_ptr,
  mmgsdi_app_enum_type        *app_type_ptr,
  mmgsdi_app_enum_type      app_type
)
{
  int index;
  boolean status = FALSE;
  for(index = 0; index < sizeof(tm_mmgsdi_session_id_table)/sizeof(tm_mmgsdi_session_id_table_entry_s_type); index++)
  {
    if ( (asubs_id == tm_mmgsdi_session_id_table[index].asubs_id) &&
     (app_type == tm_mmgsdi_session_id_table[index].app_type) &&
         (tm_mmgsdi_session_id_table[index].session_id_set == TRUE)
       )
    {
          *tm_mmgsdi_session_id_ptr =tm_mmgsdi_session_id_table[index].session_id;
          *app_type_ptr = tm_mmgsdi_session_id_table[index].app_type;
          status = TRUE;
      break;
    }

  }

  return status;

} /* tm_mmgsdi_get_sim_info_from_asid */

/*====================================================================
FUNCTION tm_mmgsdi_set_subscription_id

DESCRIPTION

  This function updates the tm mmgsdi session info table with the
  subscription id assigned to a given session type.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE


SIDE EFFECTS
  none

=====================================================================*/
void tm_mmgsdi_set_subscription_id
(
  mmgsdi_session_type_enum_type tm_mmgsdi_session_type,
  sys_modem_as_id_e_type        asubs_id
)
{
  int index;

  for(index = 0; index < sizeof(tm_mmgsdi_session_id_table)/sizeof(tm_mmgsdi_session_id_table_entry_s_type); index++)
  {
    if ( tm_mmgsdi_session_type == tm_mmgsdi_session_id_table[index].session_type )
    {
      tm_mmgsdi_session_id_table[index].asubs_id = asubs_id;
      break;
    }
  }
} /* tm_mmgsdi_set_subscription_id */


/*====================================================================
FUNCTION tm_mmgsdi_get_subscription_id

DESCRIPTION
  This function gets the subscription id assigned to a given session ID
  from tm mmgsdi session info table.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  TRUE : subscription id had been found and returned
  FALSE : failed to find subscription id for a particular session id.

SIDE EFFECTS
  none

=====================================================================*/
boolean tm_mmgsdi_get_subscription_id
(
   mmgsdi_session_id_type     session_id,
   sys_modem_as_id_e_type     *asubs_id
)
{
  int index;

  if (asubs_id == NULL)
  {
    MSG_ERROR("asusbs_id is NULL", 0,0,0);
    return FALSE;
  }

  for(index = 0; index < sizeof(tm_mmgsdi_session_id_table)/sizeof(tm_mmgsdi_session_id_table_entry_s_type); index++)
  {
    if ( session_id == tm_mmgsdi_session_id_table[index].session_id )
    {
      *asubs_id = tm_mmgsdi_session_id_table[index].asubs_id;
      return TRUE;
    }
  }

  return FALSE;

} /* tm_mmgsdi_get_subscription_id */


/*====================================================================
FUNCTION tm_mmgsdi_set_app_type

DESCRIPTION

  This function updates the tm mmgsdi session info table with the
  app type for a given session id.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE


SIDE EFFECTS
  none

=====================================================================*/
void tm_mmgsdi_set_app_type
(
  mmgsdi_session_id_type session_id,
  mmgsdi_app_enum_type         app_type
)
{
  int index;

  for(index = 0; index < sizeof(tm_mmgsdi_session_id_table)/sizeof(tm_mmgsdi_session_id_table_entry_s_type); index++)
  {
    if ( (session_id == tm_mmgsdi_session_id_table[index].session_id) &&
         (tm_mmgsdi_session_id_table[index].session_id_set == TRUE))
    {
      tm_mmgsdi_session_id_table[index].app_type = app_type;
      break;
    }
  }
} /* tm_mmgsdi_set_app_type */

#endif /*FEATURE_MMGSDI_SESSION_LIB*/


/*====================================================================
FUNCTION tm_core_get_xtra_validity_age

DESCRIPTION

  This function gets the NV item for the xtra validity age

DEPENDENCIES

RETURN VALUE


SIDE EFFECTS
  none

=====================================================================*/
void tm_core_get_xtra_validity_age(uint16 *p_age)
{

  if(p_age)
  {
    *p_age = tm_core_info.config_info.preferred_xtra_age;
  }

}


/*===========================================================================

FUNCTION tm_core_time_validity

DESCRIPTION
   Reports whether the time from time_get_ms is valid or not

DEPENDENCIES

RETURN VALUE
  boolean: TRUE - time is valid
           FALSE - time is not valid

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_core_time_validity()
{
  time_type curr_time;
  time_get_ms(curr_time);

  if ( curr_time[1] > MSB_MSEC_ELAPSED_SINCE_DEFAULT )
  {
    MSG_HIGH("TSG: Time is valid",0,0,0);
    return TRUE;
  }
  else
  {
    MSG_HIGH("TSG: Time is invalid",0,0,0);
    return FALSE;
  }
}

/*===========================================================================

FUNCTION tm_core_check_mt_up_allowed

DESCRIPTION
  Checks whether a mobile terminated User plane
  session can be allowed or not based on the service set as Designated Data
  by the user. E.g. MT V1/V2 will only be allowed if designated data is 1x.
  Similarly MT SUPL will only be allowed if designated data is GSM.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_check_mt_up_allowed
(
  tm_prtl_type  prtl_type
)
{
  boolean              single_standby = tm_cm_dsds_at_in_single_standby();
  boolean              retval         = FALSE;
  sys_sys_mode_e_type  dd_sys_mode    = SYS_SYS_MODE_NO_SRV;

  /* Consider only 1xUP and UMTS_UP protocol types for MT UP sessions, return
   * false for all other protocols.
   * In Single standby mode, always honor an MT req, since there is only one
   * active subscription service, and its available for data. */
  switch(prtl_type)
  {
    case(TM_PRTL_TYPE_1X_UP):
    {
      if(single_standby)
      {
        retval = TRUE;
      }
      else
      {
        /* DD must be CDMA for an MT 1xUP req to be honoured */
        dd_sys_mode = tm_cm_get_current_dd_sys_mode();
        if(dd_sys_mode == SYS_SYS_MODE_CDMA)
          retval = TRUE;
      }
      break;
    }

    case(TM_PRTL_TYPE_UMTS_UP):
    {
      if(single_standby)
      {
        retval = TRUE;
      }
      else
      {
        /* DD must be W, G or WG for an MT UMTS UP req to be honoured */
        dd_sys_mode = tm_cm_get_current_dd_sys_mode();
        if( dd_sys_mode == SYS_SYS_MODE_GSM ||
            dd_sys_mode == SYS_SYS_MODE_WCDMA ||
            dd_sys_mode == SYS_SYS_MODE_GW )
        {
          retval = TRUE;
        }
      }
      break;
    }

    /* MT UP req for these prtcls is always an error */
    case(TM_PRTL_TYPE_STANDALONE):
    case(TM_PRTL_TYPE_1X_CP):
    case(TM_PRTL_TYPE_UMTS_CP):
    case(TM_PRTL_TYPE_DIAG):
    default:
      {
        MSG_ERROR("Unexpected MT UP request by prtl %d", prtl_type, 0, 0);
      }
      break;
  } /* switch(prtl_type) */

  return retval;
}

/*===========================================================================

FUNCTION tm_agps_emergency_call_simulate

DESCRIPTION
   Reports whether emergency call simulation is enabled or not

DEPENDENCIES

RETURN VALUE
  boolean: TRUE - emergency call simulation enabled
           FALSE - emergency call simulation disabled

SIDE EFFECTS
  None

===========================================================================*/

boolean tm_agps_emergency_call_simulate(void)
{

  if ( tm_core_info.config_info.aagps_development_test_control5 &
       NV_AAGPS_TEST5_EMERGENCY_CALL_SIM )
  {
    return TRUE;
  }
  else
    return FALSE;
}

/*===========================================================================
FUNCTION tm_core_clear_cached_pos

DESCRIPTION Clears the RAM copy of the position

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_clear_cached_pos(void)
{
  MSG_MED("TM_CORE: clearing cached position ",0,0,0);
  memset((void *)&z_cached_pos_info, 0, sizeof(tm_core_cached_pos_s_type));
  memset((void *)&z_cached_ref_pos_info, 0, sizeof(tm_core_cached_ref_pos_s_type));
}

/*===========================================================================
FUNCTION tm_core_get_trusted_utctime

DESCRIPTION
  This function determines if the utcoffset is

 (i.e. decoded over the air)

RETURN VALUE
  TRUE if utcoffset is trusted, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_get_trusted_utctime(void)
{
  return (tm_core_info.session_info.trusted_utc_time);
}

/*===========================================================================
FUNCTION tm_core_is_standalone_active

DESCRIPTION
Check if Standalone session is in progress.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE
XTRA command status
===========================================================================*/
boolean tm_core_is_standalone_active(void)
{
  if (tm_core_info.session_info.from_protocol == TM_PRTL_TYPE_STANDALONE)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* tm_core_is_standalone_active */


/*===========================================================================
FUNCTION tm_core_get_asst_glo_lpp_up_enable

DESCRIPTION
  This function determines the positioning protocol for Assisted GLONASS U-plane
  is LPP.

RETURN VALUE
  TRUE if LPP is selected, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/

boolean tm_core_get_asst_glo_lpp_up_enable(void)
{
  if ( TM_CORE_CONFIG_DD( asst_glonass_pos_protocol ) & PDSM_PA_ASST_GLO_PROTOCOL_LPP_UP )
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================
FUNCTION tm_core_get_asst_glo_lpp_cp_enable

DESCRIPTION
  This function determines the positioning protocol for Assisted GLONASS C-plane
  is LPP.

RETURN VALUE
  TRUE if LPP is selected, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/

boolean tm_core_get_asst_glo_lpp_cp_enable(void)
{
  if ( TM_CORE_CONFIG_DD( asst_glonass_pos_protocol ) & PDSM_PA_ASST_GLO_PROTOCOL_LPP_CP )
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================
FUNCTION tm_core_lpp_grt_24_admeas_enabled

DESCRIPTION
  Return if the LPP greater than 24 AD & RSTD measurement processing is
  supported by UE.

RETURN VALUE
  TRUE if LPP > 24 AD feature is supported LPP, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/

boolean tm_core_lpp_grt_24_admeas_enabled(void)
{
  MSG_MED( "NV ITEM for > 24 AD_Meas configured as %d", tm_core_info.config_info.u_gnss_lpp_grt_24_otdoa, 0, 0 );
  return ( (0 == tm_core_info.config_info.u_gnss_lpp_grt_24_otdoa) ? FALSE : TRUE ); 
}

/*===========================================================================

FUNCTION tm_core_otdoa_evt_handler

DESCRIPTION
  This function receives OTDOA measurements from the OTDOA interface and sends
  them to the appropriate protocol
 

RETURN VALUE
  None 

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_otdoa_evt_handler(sm_loc_lte_otdoa_meas_info_type *p_otdoa_measurement_info)
{
  int prtlTableIndex;
  prtl_sess_info_param_u_type   *sess_info_param_ptr = NULL;
  boolean retVal = FALSE;
  
  MSG_HIGH("Received OTDOA measurements from SM_LOC",0,0,0);
  if (p_otdoa_measurement_info == NULL)
  {
    MSG_ERROR("No OTDOA measurment info",0,0,0);
    return;
  }
  prtlTableIndex = tm_core_get_prtl_idx_by_type(p_otdoa_measurement_info->u_sess_type);   
  
  if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
  {
    
    sess_info_param_ptr = (prtl_sess_info_param_u_type *)pdsm_getbuf(sizeof(prtl_sess_info_param_u_type));

    if(sess_info_param_ptr == NULL)
    {
      MSG_ERROR("Failed to get buffer for sess_info_param_ptr", 0, 0, 0);
      tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
      return;
    }

    /* send the data to the protocol regardless of if this is a TM session */
    /* Set reason to general error for now, if return value */
    if(tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp)
    {
      /* Internal p_meas_info pointer storage is owned by LOC OTDOA & persistant until the SM LPP CP session ends */
      memscpy((void *)&(sess_info_param_ptr->otdoa_data), sizeof(sess_info_param_ptr->otdoa_data),
              (void *)(p_otdoa_measurement_info), sizeof(*p_otdoa_measurement_info));

      if (tm_core_info.session_info.active_sess_handle != TM_SESS_HANDLE_NONE)
      {
        tm_core_info.session_info.op_requested &= ~TM_OP_REQ_OTDOA;
      }
      retVal = tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp(tm_core_info.session_info.active_sess_handle,
        PRTL_SESS_INFO_OTDOA,
        sess_info_param_ptr);
      if (retVal == FALSE)
      {
        MSG_HIGH("Could not send the data to protocol",0,0,0);
      }
    }
    
    (void) pdsm_freebuf((char *)sess_info_param_ptr);
  }
}

#if defined FEATURE_WCDMA_GPS_CELL_ID_READING
/*===========================================================================

FUNCTION tm_core_rrc_cell_info_dch_report_handler

DESCRIPTION
  This function handles the rrc cell info DCH report.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_rrc_cell_info_dch_report_handler(tm_rrc_cell_info_dch_report_type* rrc_cell_info_p)
{
  /*Pass the cell info to protocol modules*/

  /*Pass it to SUPL module*/
  tm_rrc_iface_cell_info_dch_report(rrc_cell_info_p->cell_id_info, rrc_cell_info_p->status);

  /*Pass it to GM module*/
  gm_rrc_cell_info_dch_report(rrc_cell_info_p->cell_id_info, rrc_cell_info_p->status);

  #ifdef FEATURE_CGPS_XTRA_T
  /*Pass it to XTRA-T*/
  tle_rrc_cell_info_dch_report(rrc_cell_info_p->cell_id_info, rrc_cell_info_p->status);
  #endif
  
}
#endif /*FEATURE_WCDMA_GPS_CELL_ID_READING*/

/*===========================================================================
FUNCTION tm_core_is_agps_allowed_on_serv_sys

DESCRIPTION
Check if AGPS is allowed on a serving system

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE
XTRA command status
===========================================================================*/
boolean tm_core_is_agps_allowed_on_serv_sys(cgps_SrvSystemType serv_sys)
{
  uint16 rat_mask = 0;
  
  switch (serv_sys)
  {
    case CGPS_SRV_SYS_CDMA:   rat_mask = TM_CORE_NV_RAT_ENABLE_CDMA_MASK; break;
    case CGPS_SRV_SYS_HDR:    rat_mask = TM_CORE_NV_RAT_ENABLE_HDR_MASK; break;
    case CGPS_SRV_SYS_GSM:    rat_mask = TM_CORE_NV_RAT_ENABLE_GSM_MASK; break;    
    case CGPS_SRV_SYS_WCDMA:  rat_mask = TM_CORE_NV_RAT_ENABLE_WCDMA_MASK; break;
    case CGPS_SRV_SYS_LTE:    rat_mask = TM_CORE_NV_RAT_ENABLE_LTE_MASK; break;
    case CGPS_SRV_SYS_TDSCDMA:    rat_mask = TM_CORE_NV_RAT_ENABLE_TDSCDMA_MASK; break;
    case CGPS_SRV_SYS_NONE:
    case CGPS_SRV_SYS_OTHER:  rat_mask = TM_CORE_NV_RAT_ENABLE_ALL_MASK; break;
    default:                  rat_mask = 0;
  }

  MSG_3( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "NV Enable bit mask %d Rat Mask %d Rat %d",
      TM_CORE_CONFIG_DD(agps_rat_pref).agps_rat_enable_bit_mask,
      rat_mask,
      serv_sys);
  if(TM_CORE_CONFIG_DD(agps_rat_pref).agps_rat_enable_bit_mask & rat_mask)
  {
    MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "AGPS on %d RAT allowed",serv_sys);
    return TRUE;
  }
  
  MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "AGPS on %d RAT not allowed",serv_sys);
  return FALSE;
}


/*===========================================================================
FUNCTION tm_core_is_umts_prefered

DESCRIPTION
Check if UMTS/LTE is prefered over other RAT's

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE
XTRA command status
===========================================================================*/
boolean tm_core_is_umts_prefered(void)
{
  if(TM_CORE_CONFIG_DD(agps_rat_pref).umts_lte_prefered)
  {
    MSG_HIGH("UMTS LTE prefered over other RAT",0,0,0);
    return TRUE;
  }

  MSG_HIGH("UMTS LTE not prefered over other RAT",0,0,0);
  return FALSE;
}

/*===========================================================================
FUNCTION tm_core_is_ni_standalone

DESCRIPTION
  This function checks if this is an 1x UP NI triggered session which has fallen
  back to standalone
  
DEPENDENCIES

RETURN VALUE
  Active Sess Handle

SIDE EFFECTS

===========================================================================*/
boolean tm_core_is_ni_standalone(void)
{
  tm_prtl_type active_prtl;
  active_prtl = tm_core_get_active_prtl_module(tm_core_info.session_info.queued_get_pos_param.mt_initiator);

  if ((tm_core_info.session_info.from_protocol == TM_PRTL_TYPE_STANDALONE) &&
      (active_prtl == TM_PRTL_TYPE_1X_UP) &&
        (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_1X_UP_NI))
      return TRUE;
  else
      return FALSE;
} /* tm_core_is_ni_standalone */

/*===========================================================================
FUNCTION tm_core_reset_location_service_in_progress

DESCRIPTION
  This function checks if reset location service is in progress
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_is_reset_location_service_in_progress(void)
{
  return tm_reset_location_service_in_progress; 
}

/*===========================================================================
FUNCTION tm_core_reset_location_service_in_progress

DESCRIPTION
  This function checks if reset location service is in progress
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_set_reset_location_service_in_progress(boolean reset_location_service_in_progress)
{
  MSG_HIGH("Setting Reset Location Services in progress to %d", reset_location_service_in_progress,0,0);
  tm_reset_location_service_in_progress = reset_location_service_in_progress;
}

/*===========================================================================

FUNCTION tm_core_reset_location_service_done_handler

DESCRIPTION
  This function handles the reset locaiton service done indication

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_reset_location_service_done_handler(pdsm_pd_ext_reset_location_service_status_e_type* status)
{
  pdsm_ext_status_e_type e_ext_status_event = PDSM_EXT_STATUS_RESET_LOCATION_SERVICE;
  pdsm_ext_status_info_s_type ext_status_info_type;

  ext_status_info_type.ext_status_type = PDSM_EXT_STATUS_RESET_LOCATION_SERVICE;
  ext_status_info_type.ext_status_info.ext_reset_location_service_status_type = *status;

  MSG_HIGH("Reset Location Service Done Status %d",*status,0,0 );

  /*Send Event that Reset Location Service is done*/
  sm_report_event_reset_location_service_done((uint32)*status);

  
  /*Send indication to PDAPI*/
  tm_pdapi_ext_status_event_callback(e_ext_status_event,&ext_status_info_type);

  /*Record that reset location service is completed*/ 
  tm_core_set_reset_location_service_in_progress(FALSE);

}

/*===========================================================================
FUNCTION tm_core_handle_engine_error_recovery_report

DESCRIPTION
  This function handles engine error recovery report (usually it'll be sent to
  QMI)
  
DEPENDENCIES

RETURN VALUE
  TRUE if success

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_engine_error_recovery_report(gnss_engine_err_recovery_rpt *pErrReport)
{
  if (pErrReport == NULL)
  {
    MSG_ERROR("NULL engine error recovery report",0,0,0);
    return FALSE;
  }

#if defined(__GRIFFON__) || !defined(FEATURE_CGPS_LBS_TASK)
  return TRUE;
#else
  return gnss_CQmiSvcSendEngineErrorRecoveryReport(pErrReport);
#endif
}

/*===========================================================================
FUNCTION tm_core_handle_engine_dpo_status_report

DESCRIPTION
  This function handles engine DPO status report from MGP(usually it'll be sent to
  QMI)
  
DEPENDENCIES

RETURN VALUE
  TRUE if success

SIDE EFFECTS

===========================================================================*/
extern void tm_core_handle_engine_dpo_status_report(sm_DpoStatusReportStructType *p_DpoStatusReport)
{
  pdsm_ext_status_info_s_type   pd_ext_status_info;
  memset(&pd_ext_status_info, 0, sizeof(pdsm_ext_status_info_s_type));
  if (p_DpoStatusReport == NULL)
  {
    MSG_ERROR("NULL DPO status report from MGP",0,0,0);
  }
  else
  {
    MSG_MED("Rcvd DPO status report from MGP Engaged %x DisEngagement Reason %x", p_DpoStatusReport->dpo_status_mask, p_DpoStatusReport->dpo_disengage_reason, 0);
    pd_ext_status_info.ext_status_type = PDSM_EXT_STATUS_DPO_STATUS;
    memscpy(&pd_ext_status_info.ext_status_info.ext_dpo_status_report_type,
            sizeof(pd_ext_status_info.ext_status_info.ext_dpo_status_report_type),
            p_DpoStatusReport, sizeof(*p_DpoStatusReport));
    /*Send the DPO status indication to Geofence Module*/
    gm_update_dpo_status_indication(p_DpoStatusReport);
  }
  pd_ext_status_info.client_id =  tm_core_info.session_info.pd_param.client_id;
  tm_pdapi_ext_status_event_callback(PDSM_EXT_STATUS_DPO_STATUS,
                                     &pd_ext_status_info);
  
}

/*===========================================================================
FUNCTION tm_core_handle_best_avail_pos

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  TRUE if best avail pos was requested
SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_best_avail_pos
(
  tm_core_get_best_avail_pos_param_s_type *p_get_best_avail_pos_param
)
{
  tm_core_get_gps_state_param_s_type get_gps_state_param;

  if( p_get_best_avail_pos_param == NULL)
  {
    MSG_ERROR("Null input pointer", 0, 0, 0);
    return FALSE;
  }
  MSG_HIGH("Calling tm_core_handle_best_avail_pos() for source %d ", p_get_best_avail_pos_param->source, 0, 0);

  get_gps_state_param.source = p_get_best_avail_pos_param->source;
  get_gps_state_param.cmd.p_cmd_type = p_get_best_avail_pos_param->cmd.p_cmd_type;
  tm_core_get_gps_state(&get_gps_state_param);
  return TRUE; /*lint !e506 */
}

/*===========================================================================
FUNCTION tm_core_handle_avail_wwan_pos

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  TRUE if avail WWAN pos was requested
SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_avail_wwan_pos
(
  tm_core_get_avail_wwan_pos_param_s_type *p_GetAvailWwanPosParam
)
{
  tm_core_get_gps_state_param_s_type z_GetGpsStateParam;

  if (p_GetAvailWwanPosParam == NULL)
  {
    MSG_ERROR("Null input pointer", 0, 0, 0);
    return FALSE;
  }
  MSG_HIGH("Calling tm_core_handle_avail_wwan_pos() for source %d", p_GetAvailWwanPosParam->source, 0, 0);

  z_GetGpsStateParam.source = p_GetAvailWwanPosParam->source;
  z_GetGpsStateParam.cmd.p_cmd_type = p_GetAvailWwanPosParam->cmd.p_cmd_type;
  tm_core_get_gps_state(&z_GetGpsStateParam);
  return TRUE; /*lint !e506 */
}

/*===========================================================================

FUNCTION tm_core_get_param_msg_handler

DESCRIPTION
  This function handles the get param ipc msg, and sends an ipc msg as a response
  with the param value retrieved.
  
DEPENDENCIES

RETURN VALUE TRUE/FALSE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_get_param_msg_handler(void *p_Data)
{
  os_IpcMsgType             *ipc_msg_ptr = NULL;
  lsmp_get_msg_type         *get_msg = NULL;

  uint32  length = 0;
  boolean status = FALSE;
  uint8   i      = 0;

  lsmp_premium_services_param_type *premium_services = NULL;
  lsmp_device_config_param_s_type *device_config = NULL;

  if (!p_Data)
  {
    MSG_ERROR("NULL input p_Data ptr",0,0,0);
  return status;
  }

  get_msg = (lsmp_get_msg_type*)p_Data;

  switch (get_msg->param)
  {
    case LSMP_PREMIUM_SERVICES_TYPE:
    {
      length = sizeof(lsmp_premium_services_param_type);
      ipc_msg_ptr = os_IpcCreate(length, IPC_ALLOCATION_DYNAMIC,(uint32)get_msg->thread_id);
    if (!ipc_msg_ptr)
      {
        MSG_ERROR("IPC message allocation failed", 0, 0, 0);
      return status;
      }
      premium_services = (lsmp_premium_services_param_type*)ipc_msg_ptr->p_Data;

      /* Map Premium Service NV value with correct SAP enablement value */
      if (GNSS_NV_EFS_SM_CONFIG_PREMIUM_SERVICES_DISABLE == tm_core_info.config_info.premium_services_config)
      {
        *premium_services = LSMP_PREMIUM_SERVICES_TYPE_DISABLE;
      }
      else if (GNSS_NV_EFS_SM_CONFIG_PREMIUM_SERVICES_ENABLE_PREMIUM == tm_core_info.config_info.premium_services_config)
      {
        *premium_services = LSMP_PREMIUM_SERVICES_TYPE_ENABLE_PREMIUM;
      }
      else
      {
        /* Default case is Enable Basic */
        *premium_services = LSMP_PREMIUM_SERVICES_TYPE_ENABLE_BASIC;
      }
    }
    break;

	case LSMP_DEVICE_CONFIG_TYPE:
    {
      length = sizeof(lsmp_device_config_param_s_type);
      ipc_msg_ptr = os_IpcCreate(length, IPC_ALLOCATION_DYNAMIC,(uint32)get_msg->thread_id);
	  
      if (!ipc_msg_ptr)
      {
        MSG_ERROR("IPC message allocation failed", 0, 0, 0);
        return status;
      }
      device_config = (lsmp_device_config_param_s_type*)ipc_msg_ptr->p_Data;
      device_config->lsmp_device_mode = (lsmp_device_mode_e_type)(tm_core_device_config.tm_cm_device_info);
      device_config->lsmp_overall_device_feature = tm_core_device_config.tm_cm_overall_device_feature;
      device_config->lsmp_no_of_sims = tm_core_device_config.no_of_sims;
      device_config->lsmp_no_of_active_subs = tm_core_device_config.no_of_active_subs;
      for(i = 0; i < tm_core_device_config.no_of_sims; i++)
      {
        device_config->lsmp_subs_feature[i] = tm_core_device_config.tm_cm_subs_feature[i];
        device_config->lsmp_subs_ue_mode[i] = tm_core_device_config.tm_cm_subs_ue_mode[i];
      }	
	  }
    break;

  default:
    MSG_ERROR("Unknown parameter type sent", 0, 0, 0);

    return status;
  }    

  ipc_msg_ptr->q_MsgId  = get_msg->msg_id;
  status    = os_IpcSend(ipc_msg_ptr, (uint32)get_msg->thread_id);

  if (status == FALSE)
  {
    MSG_ERROR(
    "Error in sending LSMP GET PARAM response msg_id=%d, to thread id=%d",ipc_msg_ptr->q_MsgId,ipc_msg_ptr->q_DestThreadId,0);
    status = os_IpcDelete(ipc_msg_ptr);
    ipc_msg_ptr = NULL;
  } 
  else
  {
    MSG_MED(
    "Sending LSMP GET PARAM response msg_id=%d, to thread id=%d", ipc_msg_ptr->q_MsgId,ipc_msg_ptr->q_DestThreadId,0);
  status = TRUE;
  } 

  return status;
}

/*===========================================================================
FUNCTION tm_core_enable_DpoStatusReport

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  TRUE is DPO Status Report was enabled

SIDE EFFECTS

===========================================================================*/
boolean tm_core_enable_DpoStatusReport(boolean enable)
{
  return mgp_EnableDpoStatusReport(enable);
}


/*===========================================================================
FUNCTION tm_core_handle_inject_ni_msg

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  TRUE if NI message has been injected successfully

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_inject_ni_msg(pdsm_ni_message_s_type *ni_msg)
{
  boolean bRetVal = TRUE;
  prtl_event_u_type  *event_payload_ptr = NULL;

  if (ni_msg == NULL || ni_msg->len <= 0 || ni_msg->ni_data == NULL)
  {
    return FALSE;
  }
  
  MSG_HIGH("TM got NI message injection (type=%d len=%d)", ni_msg->type, ni_msg->len, 0);

  /* allocate buffer for sending event to protocol submodule */
  event_payload_ptr = (prtl_event_u_type *)pdsm_getbuf(sizeof(prtl_event_u_type));
  if ( event_payload_ptr == NULL )
  {
    MSG_ERROR("Can't get buffer", 0, 0, 0);
    bRetVal = FALSE;
  }
  else
  {
    switch(ni_msg->type)
    {
    case NI_MESSAGE_SUPL:
      event_payload_ptr->sms_event_payload.length = ni_msg->len;
      event_payload_ptr->sms_event_payload.data = ni_msg->ni_data; // this is a pointer to buffer (from pdsm_getbuf)
    
      if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP].event_cb_fp != NULL)
      {
        (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP].event_cb_fp(
              PRTL_EVENT_TYPE_SMS, event_payload_ptr);
      }      
      break;

      /* probably can add V1/V2 NI message here in the future */
    default:
      MSG_ERROR("Unknown NI message type %d", ni_msg->type, 0, 0);
      bRetVal = FALSE;

    } /* switch  */

    (void) pdsm_freebuf((char *)event_payload_ptr);  // free this event buffer
  }

  /* free buffer if something is wrong, otherwise buffer will be freed by PDCOMM WMS */
  if (bRetVal == FALSE && ni_msg->ni_data != NULL)
  {
    os_MemFree((void **)&(ni_msg->ni_data));  // free this NI message buffer
    ni_msg->ni_data = NULL;
  }

  return bRetVal;
}


/*===========================================================================
FUNCTION tm_core_handle_inject_wifi_on_demand_msg

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  TRUE if WiFi-on-deman-scan message has been injected successfully

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_inject_wifi_on_demand_msg(pdsm_wifi_on_demand_info_list_s_type *wifi_on_demand_msg)
{
  boolean bRetVal = TRUE;
  prtl_event_u_type  *event_payload_ptr = NULL;

  if (wifi_on_demand_msg == NULL)
  {
    MSG_ERROR("Null pointer passed into tm_core_handle_inject_wifi_on_demand_msg", 0, 0, 0);
    return FALSE;
  }

  if (wifi_on_demand_msg->q_num_wifi_aps == 0 || 
      wifi_on_demand_msg->q_num_wifi_aps > QMI_LOC_WIFI_MAX_REPORTED_APS_PER_MSG_V01 || wifi_on_demand_msg->first_wifi_ap_ptr == NULL)
  {

    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Invalid WiFi-on-demand-scan-result-injection (numWifiAps=%u first_wifi_ap_ptr=%x)", 
              wifi_on_demand_msg->q_num_wifi_aps,
              wifi_on_demand_msg->first_wifi_ap_ptr);
    return FALSE;
  }
  
  MSG_MED("TM got WiFi-on-demand-scan-result injection (numWifiAps=%u)", wifi_on_demand_msg->q_num_wifi_aps, 0, 0);

  /* allocate buffer for sending event to protocol submodule */
  event_payload_ptr = (prtl_event_u_type *)pdsm_getbuf(sizeof(prtl_event_u_type));
  if ( event_payload_ptr == NULL )
  {
    MSG_ERROR("Can't get buffer", 0, 0, 0);
    bRetVal = FALSE;
  }
  else
  {
      event_payload_ptr->wifi_scan_event_payload.q_num_wifi_aps = wifi_on_demand_msg->q_num_wifi_aps;
      event_payload_ptr->wifi_scan_event_payload.first_wifi_ap_ptr = wifi_on_demand_msg->first_wifi_ap_ptr; // this is a pointer to buffer (from pdsm_getbuf)
    
      if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP].event_cb_fp != NULL)
      {
        (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP].event_cb_fp(
              PRTL_EVENT_TYPE_WIFI_SCAN_RESULT, event_payload_ptr);
      } 
           
      (void) pdsm_freebuf((char *)event_payload_ptr);  // free this event buffer
  }

  return bRetVal;
}



/*===========================================================================

FUNCTION tm_core_external_cell_db_proc

DESCRIPTION
  This function is called by TmCore's IPC processor to inform TmCore of 
  an incoming external cell database update message. 
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

boolean tm_core_external_cell_db_proc( pdsm_cell_info_s_type *external_cell_db_info_ptr )
{
  prtl_event_u_type  *event_payload_ptr = NULL;
  boolean ret_val = FALSE;

  //Update tm_cm with external RAT changes
  tm_cm_process_external_srv_changed(external_cell_db_info_ptr);

  MSG_HIGH("TM got external CellDB update", 0, 0, 0);

  
  MSG_HIGH("External cell info: MCC = %d, MNC = %d, LAC = %d",
        external_cell_db_info_ptr->cellInfo.gsmCell.MCC,
        external_cell_db_info_ptr->cellInfo.gsmCell.MNC,
        external_cell_db_info_ptr->cellInfo.gsmCell.LAC);

  MSG_HIGH("External cell info: CID = %d, serviceStatus = %d, RAT = %d",
        external_cell_db_info_ptr->cellInfo.gsmCell.CID,
        external_cell_db_info_ptr->serviceStatus,
        external_cell_db_info_ptr->activeRAT);

  
  /* allocate buffer for sending event to protocol submodule */
  event_payload_ptr = (prtl_event_u_type *)pdsm_getbuf(sizeof(prtl_event_u_type));
  if ( event_payload_ptr == NULL )
  {
    MSG_ERROR("Can't get buffer", 0, 0, 0);
    return FALSE;
  }

  event_payload_ptr->cell_db_event_payload.cell_db_event = PRTL_EXTERNAL_CELL_DB_UPD;
  event_payload_ptr->cell_db_event_payload.cell_db_event_payload.external_modem_cell_db_payload.cellInfo = 
                external_cell_db_info_ptr->cellInfo;
  event_payload_ptr->cell_db_event_payload.cell_db_event_payload.external_modem_cell_db_payload.serviceStatus =
                external_cell_db_info_ptr->serviceStatus;
  event_payload_ptr->cell_db_event_payload.cell_db_event_payload.external_modem_cell_db_payload.activeRAT=
                external_cell_db_info_ptr->activeRAT;
  

  if(tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP].event_cb_fp != NULL)
  {
    (void)tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_UMTS_UP].event_cb_fp(
            PRTL_EVENT_TYPE_CELL_DB,
            event_payload_ptr);
  ret_val = TRUE;
  }      
  else
  {
    ret_val = FALSE;
  }
  
  (void) pdsm_freebuf((char *)event_payload_ptr);

  return ret_val;

}

/*===========================================================================

FUNCTION tm_core_api_callback_handler

DESCRIPTION
  This function handles calling the client callbacks

DEPENDENCIES

RETURN VALUE TRUE/FALSE

SIDE EFFECTS

===========================================================================*/

boolean tm_core_api_callback_handler(const tm_api_notifier_type *p_api_notifier)
{

  uint8 errCode = 0;
  uint8 cb_index;
  static uint8 broadcast_counter = 0;
  static uint8 tm_index = 0;
  uint8 cb_bitmap = 0;
  const uint8 CLIENT_INDEX_SHIFT_VALUE = 4;
  const uint8 BROADCAST_SHIFT_VALUE = 6;
  const uint8 TM_SHIFT_VALUE = 7;

  if (p_api_notifier)
  {
    if (p_api_notifier->broadcast)
    {
      if (broadcast_counter == 0)
      {
        /* Create a high priority timer once for calling api callbacks */
      tm_core_info.tm_task_info.api_callback_timer =
        os_TimerCreateInCgpsTimerGroup((uint32)TM_CORE_TIMER_ID_API_CALLBACK,(uint32)THREAD_ID_SM_TM);
      }
    broadcast_counter++;

      /* Call all callbacks in the array if registered */
    for (cb_index = 0; cb_index < TM_UNSUPPORTED_CLIENT_INDEX; cb_index++)
    {
      if (pz_SmNotifierCb[cb_index] != NULL)
      {
        pz_SmNotifierCb[cb_index]();

      /* Log the call back status */
      /* bit 0-3 for timer, bit 4-5 for client index, bit 6 for broadcast or not, bit 7 for TM/GM */
      cb_bitmap |= tm_cb_timer_counter;
      cb_bitmap |= (cb_index << CLIENT_INDEX_SHIFT_VALUE);
      cb_bitmap |= ((uint8)p_api_notifier->broadcast) << BROADCAST_SHIFT_VALUE;
      cb_bitmap |= ((uint8)1) << TM_SHIFT_VALUE;

      if (tm_index < GPS_MAX_CALLBACK_CLIENTS)
      {
        tm_call_back_log[tm_index] = cb_bitmap;
      tm_index++;
      }
      else
      {
        errCode = 3;
      }

      /* Set to NULL after callback */
      pz_SmNotifierCb[cb_index] = NULL;
    }
    }

    if(os_TimerStart(tm_core_info.tm_task_info.api_callback_timer, TM_CORE_API_CALLBACK_INTERVAL_MS, 0) == FALSE)
      {
        ERR_FATAL("Failed start api_callback timer!", 0, 0, 0);
      return FALSE;
    }
    }
    else
    {
      /* Call clients callback right away since we receive this IPC only if TM is ready */
      p_api_notifier->pNotifierCb();

    /* Log the call back status */
    /* bit 0-3 for timer (0xF if not broadcast), bit 4-5 for client index, bit 6 for broadcast or not, bit 7 for TM/GM */
    cb_bitmap |= 0x0F;
    cb_bitmap |= (p_api_notifier->clientIndex << CLIENT_INDEX_SHIFT_VALUE);
    cb_bitmap |= ((uint8)p_api_notifier->broadcast) << BROADCAST_SHIFT_VALUE;
    cb_bitmap |= ((uint8)1) << TM_SHIFT_VALUE;

    if (tm_cb_timer_counter < TM_CORE_MAX_NUM_API_CALLBACKS)
    {
      /* We are here only during initial 10 seconds after bootup.
                At this time, the timer is still running. Log it in buffer to display the callback logs after timer is done*/
      if (tm_index < GPS_MAX_CALLBACK_CLIENTS)
      {
        tm_call_back_log[tm_index] = cb_bitmap;
        tm_index++;
      }
      else
      {
        errCode = 2;
      }
    }
    else
    {
      /* We are here after we are done running timer after bootup.
             Timer is done, display the callback log now */
      MSG_HIGH("Callback Log report for TM: 0x%x", cb_bitmap,0,0);
    }

      /* Set pz_SmNotifierCb array index to NULL after callback */
      pz_SmNotifierCb[p_api_notifier->clientIndex] = NULL;
  }
  }
  else
  { /* p_api_notifier == NULL */
    errCode = 1;
  }

  if (errCode > 0)
  {
    MSG_ERROR("tm_core_api_callback_handler failed with error code = %d", errCode, 0, 0);
  return FALSE;
  }
  else
  {
    return TRUE;
  }
}


/*===========================================================================
FUNCTION tm_core_start_emergency_session

DESCRIPTION
  This function starts a new GPS session with TM-LM for a emergency positioning
  at the start of the emergency call. This is to basically do an early receiver
  ON because, there might be a UP/CP GPS session from network. Once the network
  sends a SUPL/CP GPS session, the session will be updated.

DEPENDENCIES

RETURN VALUE
  TRUE if session continue ok

SIDE EFFECTS

===========================================================================*/

boolean tm_core_start_emergency_session(void)
{
   boolean                                   accepted = 0;
  tm_sess_req_param_u_type                  req_param;
  uint8                                     acc_thresh = 0;
  tm_sess_req_start_param_s_type            *p_start_params = NULL;


  /*
   *  Create a pointer to the START SESSION request parameters to make
   *  the code a bit cleaner and easier to read later in this function.
   */
  p_start_params = &req_param.start_param;
  
  /*
   *  Start out with an accuracy threshold of 0 meters when starting
   *  an MT session so that the MGP receiver will stay on indefinitely.
   *  
   *  The accuracy threshold will get set to the desired value later
   *  when we receive the Request Location message from the E-SMLC.
   */
  acc_thresh = 0;

#ifdef FEATURE_SUPPORT_UMTS_GPS_PROTOCOLS
  #ifdef FEATURE_MO_GPS_FOR_ECALL
  /* If both LPP UP and LPP CP disabled, OR
   * LTE MSB disabled OR
   * AAGPS NV support set to Reject All or Low Priority (NI or MO), 
   * dont do early start */
  if ( ((tm_core_get_lte_lpp_up_enable() == FALSE)  &&
       (tm_core_get_lpp_cp_enable() == FALSE)) ||
      ((tm_umts_common_is_lte_msb_enabled() == FALSE) ||
       (tm_core_info.config_info.aagps_emergency_services_spprt == AAGPS_NV_EMERGENCY_SUPPORT_REJECT_ALL) ||
       (tm_core_info.config_info.aagps_emergency_services_spprt == AAGPS_NV_EMERGENCY_SUPPORT_LOW_PRIORITY)))
  {
    MSG_MED("Early rcvr not started in emergency on LTE due to NV",0,0,0);
    return TRUE;
  }
  #else
  if ( ((tm_core_get_lte_lpp_up_enable() == FALSE)  &&
       (tm_core_get_lpp_cp_enable() == FALSE)) ||
      ((tm_umts_common_is_lte_msb_enabled() == FALSE) ||
       (tm_core_info.config_info.aagps_emergency_services_spprt == AAGPS_NV_EMERGENCY_SUPPORT_REJECT_ALL) ))
  {
    MSG_MED("Early rcvr not started in emergency on LTE due to NV",0,0,0);
    return TRUE;
  }
  #endif /* FEATURE_MO_GPS_FOR_ECALL */
#endif /* FEATURE_SUPPORT_UMTS_GPS_PROTOCOLS */

  p_start_params->op_mode = TM_SESS_OPERATION_MODE_MSB;
  p_start_params->op_req  = TM_OP_REQ_LR |TM_OP_REQ_AIDING_DATA_STATUS;
  p_start_params->lr_qos  = TM_MAX_EMERGENCY_SESSION_TIMEOUT;

  /*
   *  Set up parameters that are common to all modes
   */
  p_start_params->abort_curr_sess     = TRUE; /* ayt_todo */
  p_start_params->num_fixes           = 1;
  p_start_params->tbf_ms              = 1000;
  p_start_params->accuracy_threshold  = acc_thresh;
  p_start_params->ppm_qos             = 0;
  p_start_params->prm_prq             = 0;
  p_start_params->dynamic_qos_enabled = FALSE;
  p_start_params->multi_report_msa = FALSE;

  /*
   *  Tell TM-Core to start a session now. Use a dummy protocol type. THis protocol type will
      be changed once the network starts a LPP CP or UP session.
   */
  accepted = tm_sess_req( TM_PRTL_TYPE_E911,  
                          TM_SESS_HANDLE_E911,
                          TM_SESS_REQ_START,
                          &req_param );

  if( accepted )
  {
    MSG_MED("TM_CORE: tm_sess_req() STARTED", 0, 0, 0);
    tm_core_send_pdapi_emerg_sess_type(PDSM_PD_EXT_STATUS_EMERG_SESS_START);
 
  }
  else
  {
    MSG_MED("TM_CORE: tm_sess_req() REJECTED", 0, 0, 0);
  }

  return accepted;
}

/*===========================================================================

FUNCTION tm_core_ecid_evt_handler

DESCRIPTION
  This function receives ECID measurements from the ECID interface and sends
  them to the appropriate protocol
 

RETURN VALUE
  None 

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_ecid_evt_handler(gnss_lpp_ecid_meas_info_type *p_ecid_measurement_info)
{
  int prtlTableIndex;
  prtl_sess_info_param_u_type   *sess_info_param_ptr = NULL;
  boolean retVal = FALSE;
  
  MSG_HIGH("Received ECID measurements from ML1",0,0,0);
  if (p_ecid_measurement_info == NULL)
  {
    MSG_ERROR("No ECID measurment info",0,0,0);
    return;
  }
  prtlTableIndex = tm_core_get_prtl_idx_by_type(p_ecid_measurement_info->q_sess_type);   
  
  if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
  {
    
    sess_info_param_ptr = (prtl_sess_info_param_u_type *)pdsm_getbuf(sizeof(prtl_sess_info_param_u_type));

    if(sess_info_param_ptr == NULL)
    {
      MSG_ERROR("Failed to get buffer for sess_info_param_ptr", 0, 0, 0);
      tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
      return;
    }

    /* send the data to the protocol regardless of if this is a TM session */
    /* Set reason to general error for now, if return value */
    if(tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp)
    {
      memscpy((void *)&(sess_info_param_ptr->ecid_data),
              sizeof(sess_info_param_ptr->ecid_data),
      &p_ecid_measurement_info->z_meas_info ,
              sizeof(p_ecid_measurement_info->z_meas_info));

      if (tm_core_info.session_info.active_sess_handle != TM_SESS_HANDLE_NONE)
      {
        tm_core_info.session_info.op_requested &= ~TM_OP_REQ_ECID;
      }
      retVal = tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp(tm_core_info.session_info.active_sess_handle,
        PRTL_SESS_INFO_ECID,
        sess_info_param_ptr);
      if (retVal == FALSE)
      {
        MSG_HIGH("Could not send the data to protocol",0,0,0);
      }
    }
    
    (void) pdsm_freebuf((char *)sess_info_param_ptr);
  }
}

/*===========================================================================

FUNCTION tm_core_retrieve_imsi

DESCRIPTION
  This function is called by TLE to request for IMSI retrieval

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_retrieve_imsi( tm_RetrieveIMSIType * tm_retrieve_imsi_info_ptr )
{
  cgps_IMSIType z_IMSIInfo;
  tm_RetrieveIMSICb pCallback = tm_retrieve_imsi_info_ptr->pz_RetrieveIMSICb;

  memset(&z_IMSIInfo, 0x0, sizeof(z_IMSIInfo));

  z_IMSIInfo.sub = tm_retrieve_imsi_info_ptr->sub;

  if (!tm_sim_retrieve_imsi(&z_IMSIInfo))
  {
    MSG_HIGH("IMSI retrieval failed", 0, 0, 0);
  }
  else
  {
    if(NULL != pCallback)
    {
      (*pCallback)(&z_IMSIInfo);
    }
    else
    {
      MSG_HIGH("Null callback passed", 0, 0, 0);
    }
  }
}


/*===========================================================================

FUNCTION tm_copy_prm_to_sm_meas_struct

DESCRIPTION
  This function copies the ME provided PRM data to SM GNSS Measurement report struct.

DEPENDENCIES

RETURN VALUE
  Return GPS or GLONASS measurement index upon success, else return invalid if
  PRM data isn't of GPS or GLONASS type.
SIDE EFFECTS

===========================================================================*/
int32 tm_copy_prm_to_sm_meas_struct(const sm_GpsMeasRptStructType *const p_prm_data,
                                    SmGnssMeasReportStructType *p_gnss_meas_report)
{
  if ((p_prm_data != NULL) && (p_gnss_meas_report != NULL))
  {
    if (GNSS_MEAS_BLK_SRC_GPS == p_prm_data->z_MeasBlk.e_MeasBlkSrc)
    {
      /* Set measurement report bitmap to indicate GPS meas is copied over (below) */
      p_gnss_meas_report->u_meas_reports_bmap |= SM_GNSS_MEAS_REPORTS_BMAP_GPS;

      memset(&p_gnss_meas_report->z_meas_reports[SM_GNSS_MEAS_SYSTEM_GPS], 0,
             sizeof(p_gnss_meas_report->z_meas_reports[SM_GNSS_MEAS_SYSTEM_GPS]));
      memscpy(&p_gnss_meas_report->z_meas_reports[SM_GNSS_MEAS_SYSTEM_GPS],
             sizeof(p_gnss_meas_report->z_meas_reports[SM_GNSS_MEAS_SYSTEM_GPS]),
             p_prm_data, sizeof(*p_prm_data));

      return SM_GNSS_MEAS_SYSTEM_GPS;
    }
    else if (GNSS_MEAS_BLK_SRC_GLO == p_prm_data->z_MeasBlk.e_MeasBlkSrc)
    {
      /* Set measurement report bitmap to indicate GLONASS meas is copied over (below) */
      p_gnss_meas_report->u_meas_reports_bmap |= SM_GNSS_MEAS_REPORTS_BMAP_GLONASS;

      memset(&p_gnss_meas_report->z_meas_reports[SM_GNSS_MEAS_SYSTEM_GLONASS], 0,
             sizeof(p_gnss_meas_report->z_meas_reports[SM_GNSS_MEAS_SYSTEM_GLONASS]));
      memscpy(&p_gnss_meas_report->z_meas_reports[SM_GNSS_MEAS_SYSTEM_GLONASS],
             sizeof(p_gnss_meas_report->z_meas_reports[SM_GNSS_MEAS_SYSTEM_GLONASS]),
             p_prm_data, sizeof(*p_prm_data));

      return SM_GNSS_MEAS_SYSTEM_GLONASS;
    }
    else if (GNSS_MEAS_BLK_SRC_BDS == p_prm_data->z_MeasBlk.e_MeasBlkSrc)
    {
      /* Set measurement report bitmap to indicate BDS meas is copied over (below) */
      p_gnss_meas_report->u_meas_reports_bmap |= SM_GNSS_MEAS_REPORTS_BMAP_BDS;

      memset(&p_gnss_meas_report->z_meas_reports[SM_GNSS_MEAS_SYSTEM_BDS], 0,
             sizeof(p_gnss_meas_report->z_meas_reports[SM_GNSS_MEAS_SYSTEM_BDS]));
      memscpy(&p_gnss_meas_report->z_meas_reports[SM_GNSS_MEAS_SYSTEM_BDS],
           sizeof(p_gnss_meas_report->z_meas_reports[SM_GNSS_MEAS_SYSTEM_BDS]),
           p_prm_data, sizeof(*p_prm_data));

      return SM_GNSS_MEAS_SYSTEM_BDS;
    }

    else if (GNSS_MEAS_BLK_SRC_GAL == p_prm_data->z_MeasBlk.e_MeasBlkSrc)
    {
      /* Set measurement report bitmap to indicate GAL meas is copied over (below) */
      p_gnss_meas_report->u_meas_reports_bmap |= SM_GNSS_MEAS_REPORTS_BMAP_GAL;

      memset(&p_gnss_meas_report->z_meas_reports[SM_GNSS_MEAS_SYSTEM_GAL], 0,
             sizeof(p_gnss_meas_report->z_meas_reports[SM_GNSS_MEAS_SYSTEM_GAL]));
      memscpy(&p_gnss_meas_report->z_meas_reports[SM_GNSS_MEAS_SYSTEM_GAL],
           sizeof(p_gnss_meas_report->z_meas_reports[SM_GNSS_MEAS_SYSTEM_GAL]),
           p_prm_data, sizeof(*p_prm_data));

      return SM_GNSS_MEAS_SYSTEM_GAL;
    }
    else if (GNSS_MEAS_BLK_SRC_QZSS_SBAS == p_prm_data->z_MeasBlk.e_MeasBlkSrc)
    {
      /* Set measurement report bitmap to indicate QZSS/SBAS meas is copied over (below) */
      p_gnss_meas_report->u_meas_reports_bmap |= SM_GNSS_MEAS_REPORTS_BMAP_QZSS;

      memset(&p_gnss_meas_report->z_meas_reports[SM_GNSS_MEAS_SYSTEM_QZSS], 0,
             sizeof(p_gnss_meas_report->z_meas_reports[SM_GNSS_MEAS_SYSTEM_QZSS]));
      memscpy(&p_gnss_meas_report->z_meas_reports[SM_GNSS_MEAS_SYSTEM_QZSS],
           sizeof(p_gnss_meas_report->z_meas_reports[SM_GNSS_MEAS_SYSTEM_QZSS]),
           p_prm_data, sizeof(*p_prm_data));

      return SM_GNSS_MEAS_SYSTEM_QZSS;
    }
  }
  return SM_GNSS_MEAS_SYSTEM_INVALID;
}

/*===========================================================================
FUNCTION tm_core_send_pdapi_emerg_sess_type

DESCRIPTION
  This function sends a PDAPI event of the type of ongoing GPS session during
  emergency positioning

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_send_pdapi_emerg_sess_type(pdsm_ext_status_emerg_sess_e_type sess_type)
{
  pdsm_ext_status_e_type e_ext_status_event = PDSM_EXT_STATUS_EMERG_POS_SESSION_TYPE;
  pdsm_ext_status_info_s_type ext_status_info_type;

  memset((void*)&ext_status_info_type, 0, sizeof (pdsm_ext_status_info_s_type));
  ext_status_info_type.client_id = pdsm_ni_client_id_get();
  ext_status_info_type.ext_status_type = PDSM_EXT_STATUS_EMERG_POS_SESSION_TYPE;
  ext_status_info_type.ext_status_info.ext_emerg_sess_type.sess_type = sess_type;
  /*Send indication to PDAPI*/
  tm_pdapi_ext_status_event_callback(e_ext_status_event,&ext_status_info_type);

}


/*===========================================================================
FUNCTION tm_core_is_supl_ni_proc

DESCRIPTION
  This function checks if this is an SUPL NI trigger should be reported to AP always

DEPENDENCIES

RETURN VALUE
  Active Sess Handle

SIDE EFFECTS

===========================================================================*/
boolean tm_core_is_supl_ni_proc(void)
{
  MSG_MED("TM_CORE: SUPL NI setting %d", tm_core_info.config_info.b_supl_notify_to_ap,0,0);
  return(tm_core_info.config_info.b_supl_notify_to_ap);
}

/*===========================================================================
FUNCTION tm_core_mtlr_auto_accept

DESCRIPTION
  This function checks if this is an MTLR should be accepted without AP notification
  during emergency

DEPENDENCIES

RETURN VALUE
  Active Sess Handle

SIDE EFFECTS

===========================================================================*/
boolean tm_core_mtlr_auto_accept (void)
{
  MSG_MED("TM_CORE: MTLR setting %d", tm_core_info.config_info.b_auto_accept_mtlr,0,0);
  return(tm_core_info.config_info.b_auto_accept_mtlr);
}

/*===========================================================================

FUNCTION tm_core_get_gm_engine_config

DESCRIPTION
  This function returns the GM Engine configuration

RETURN VALUE
  The timeout value in secs.

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_get_gm_engine_config(geofence_engine_config_s_type **gm_config)
{
  *gm_config = &(tm_core_info.config_info.gm_config);
}

/*===========================================================================

FUNCTION tm_core_is_wiper_running

DESCRIPTION
 Function to get whether Wiper is running

DEPENDENCIES

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_core_is_wiper_running(void)
{
  return tm_sysd_is_wiper_running();
}

/*===========================================================================
FUNCTION tm_core_set_ni_resp_pending

DESCRIPTION
  This function checks if this is an SUPL NI trigger should be reported to AP always
  
DEPENDENCIES

RETURN VALUE
  Active Sess Handle

SIDE EFFECTS

===========================================================================*/
void tm_core_set_ni_resp_pending(boolean b_pending)
{
  tm_core_info.b_pending_ni_resp_from_client =  b_pending;
}


/*===========================================================================
FUNCTION tm_core_is_ni_resp_pending

DESCRIPTION
  This function checks if this is an SUPL NI trigger should be reported to AP always
  
DEPENDENCIES

RETURN VALUE
  Active Sess Handle

SIDE EFFECTS

===========================================================================*/
boolean tm_core_is_ni_resp_pending(void)
{
  return (tm_core_info.b_pending_ni_resp_from_client);
}

/*===========================================================================
FUNCTION tm_core_get_lpp_periodic_end_flag

DESCRIPTION
  This function checks if LPP periodic session should end after emergency call endss
  
DEPENDENCIES

RETURN VALUE
  Active Sess Handle

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_get_lpp_periodic_end_flag (void)
{
  return(tm_core_info.config_info.b_end_lpp_periodic);
}

/********************************************************************
*
* tm_handle_lte_ota_gps_reset
*
* Function description:
*  Handles the Test OTA reset message for GPS
*
* Parameters: 
*
*
* Return:
*  none
*  
*********************************************************************
*/  
void tm_handle_lte_ota_gps_reset(void)
{
  tm_cm_phone_state_info_s_type phone_state_info;
  pdsm_delete_parms_type        delete_param;

  memset((void*)&phone_state_info,0,sizeof(phone_state_info));
  /* Query TM-CM module to get current acquired system */
  tm_cm_iface_get_phone_state(&phone_state_info);
  
  if ((phone_state_info.e911_state != TM_CM_E911_STATE_NONE) &&
       (tm_core_session_active()))
  {
     MSG_HIGH("TM_CORE: OTA Reset during emergency not allowed",0,0,0);
     return;
  }

  /* OTA GPS reset message */
  if (tm_core_session_active())
  {
    MSG_MED("OTA Reset during session. Abort ongoing session",0,0,0);
    tm_core_abort_recover(TRUE, TM_STOP_REASON_PDE_END_SESSION);
  }
    if ( tm_core_info.e_cached_rcvr_state != C_GPS_RCVR_STATE_OFF )
    {
      mgp_ReceiverForcedIdle();
      mgp_ReceiverForcedOff();
    }
    memset( &delete_param, 0, sizeof(pdsm_delete_parms_type) );
#ifdef  FEATURE_CGPS_LTE_CELLDB 
   tm_core_lte_nas_ota_reset_notify_event(PRTL_EVENT_TYPE_LTE_OTA_GPS_RESET);
#endif /* FEATURE_CGPS_LTE_CELLDB  */
	tm_core_delete_inject_aiding_data(&delete_param, FALSE, TRUE, PDSM_PA_DELETE_PARAMS_EX);

    /* For LTE OTA GPS Reset, also refresh SIB8/16 Info */
    mgp_pgi_SendSibTimeRefreshMsg();

  }	

/********************************************************************
* tm_core_handle_wifi_attachment_status_ind
*
* Function description:
* Handles Wifi attachment status indication info
*
* Parameters: 
*  p_wifi_attachment_status_ind_info - Pointer to Wifi attachment status inidcation info stuct
*
* Return:
* void
*  
*********************************************************************
*/  
void tm_core_handle_wifi_attachment_status_ind(pdsm_wifi_attachment_status_ind_info *p_wifi_attachment_status_ind_info)
{
  gm_wifi_attachment_status_ind_s_type z_gm_wifi_attach_info;
  memset(&z_gm_wifi_attach_info, 0, sizeof(z_gm_wifi_attach_info));

  /* Convert attachment info into GM wifi attachment info*/

  memscpy(&z_gm_wifi_attach_info.z_attach_info, 
          sizeof(z_gm_wifi_attach_info.z_attach_info),
          p_wifi_attachment_status_ind_info,
          sizeof(pdsm_wifi_attachment_status_ind_info));
  /* Indicate that the attachment info is from HLOS */
  z_gm_wifi_attach_info.e_src = GM_WIFI_ATTACH_INFO_SRC_HLOS;

  (void)gm_inject_wifi_attach_status_ind(&z_gm_wifi_attach_info);
}

void tm_core_handle_wifi_enabled_status_ind(pdsm_wifi_enabled_status_ind_type e_wifi_enabled_status)
{
  gm_inject_wifi_enabled_status_ind(e_wifi_enabled_status);
}

/*===========================================================================
FUNCTION tm_core_get_end_supl_after_final_fix

DESCRIPTION
  This function checks SUPL can be terminated after final fix is sent
  
DEPENDENCIES

RETURN VALUE
  TRUE if NV is enabled, FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
boolean tm_core_get_end_supl_after_final_fix_nv(void)
{
  return (tm_core_info.config_info.b_end_supl_after_final_fix); 
}

/*===========================================================================
FUNCTION tm_core_handle_wifi_fix_response

DESCRIPTION
  This function handles Wifi fix Response
  
DEPENDENCIES

RETURN VALUE
  VOID

SIDE EFFECTS

===========================================================================*/
void tm_core_handle_wifi_fix_response(tm_wifi_fix_response_type *pz_wifi_fix_resp)
{
  if(NULL == pz_wifi_fix_resp)
  {
    MSG_ERROR("Null pointer in handling Wifi fix reponse",0,0,0);
    return;
  }

  if(WL_POS_REQ_SRC_BG_LOC_ENG == pz_wifi_fix_resp->e_wifi_pos_src)
  {    
    sm_gm_pos_fix_info fix_info = {0};
    /*Send the Wifi fix reponse to Geofencing engine*/       
    fix_info.y_pos_fix_ext_info.z_wifi_fix_ext_info.z_wifi_result = pz_wifi_fix_resp->z_wifi_result;
    fix_info.fix_src = GEOFENCE_POS_FIX_SRC_LPW;              
    fix_info.pos_fix = pz_wifi_fix_resp->z_PosFixRpt;
    fix_info.fix_type = GEOFENCE_POS_FIX_TYPE_FINAL;
    
    (void)gm_position_update(&fix_info);
  }
  else
  {
    MSG_ERROR("Unknown Source %d", pz_wifi_fix_resp->e_wifi_pos_src,0,0);
  }
}

/*===========================================================================
FUNCTION tm_core_lpp_earlytimer_enabled

DESCRIPTION
  Return if the LPP Rel12 2Timer feature is enabled. If enabled the Early
  Response timer in Qos is processed, else it is dropped. By default the
  feature is disabled.

RETURN VALUE
  TRUE  :If LPP Rel12 2Timer feature is enabled.
  FALSE :If disabled

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/

boolean tm_core_lpp_earlytimer_enabled(void)
{
  MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "NV ITEM for Rel12 2Timer configured as %d", 
           tm_core_info.config_info.u_gnss_lpp_rel12_2timer_support);
  return ( ( 0 == tm_core_info.config_info.u_gnss_lpp_rel12_2timer_support) ? FALSE : TRUE ); 
}
