/*==============================================================================

                     G P S   N V   I T E M   L O A D E R

  DESCRIPTION

    The GPS NV item loader is used to set GPS related NV items to default values
    at boot time.

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

==============================================================================*/

/*==============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/common/src/gps_nv_loader.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

==============================================================================*/

#include "fs_public.h"
#include "comdef.h"
#include "aries_os_api.h"
#include "aries_os_globals.h"
#include "nv.h"
#include "gps_common.h"
#include "gps_fs_task.h"
#include "gnss_nv_loader.h"

/*
 * Constant definitions
*/
#define C_GPS_FS_NV_READ_SIGNAL   0x01

#define GPS_NV_CLIENT_HANDLE_MAX   1
#define GPS_NV_CLIENT_HANDLE_ERROR 0xFF
#define GPS_NV_VERIFY_COUNT        2  /* Verify item count */

/* List size should be twice, to implement reading optimization */
#define GPS_NV_MIN_LIST_SIZE      (GPS_NV_VERIFY_COUNT*2)

/* This is used to jump to the end of the list with user defined limit */
#define GPS_NV_LOOP_LIMIT         (GPS_NV_VERIFY_COUNT - 1)

#define NV_ITEM_NO_VAL -1
#define NV_ITEM         0
#define NV_ITEM_VAL     1
#define NV_ITEM_VAL2    2

#define NV_MAX_SIZE_TYPE_INT    2
#define NV_MAX_SIZE_TYPE_DUAL   3
#define NV_MAX_SIZE_TYPE_IPC_DM 33

/*
 * Structure definitions
*/

/* Type for gps_default NV write command callback function
 *
 * A client that wants to be notified of the result of a gps_default NV write
 * command needs to supply a pointer to a function of this type as the
 * only parameter to the gps_NvInit() function.
 */
typedef void (gps_nv_cmd_cb_f_type)
(
  /* NV item that this write command status is for */
  nv_items_enum_type    item,

  /* Pointer to gps_default NV module provided write data */
  void*                 data,

  /* Command status code indicating write pass/fail */
  nv_stat_enum_type     status
);

typedef struct
{
  q_link_type           link;
  nv_items_enum_type    item;
  nv_item_type          data;
  uint32                handle;
} gps_nv_item_s_type;

typedef struct
{
  boolean               active;
  os_ThreadIdEnumType   thread_id;
  gps_nv_cmd_cb_f_type* nv_cb_fn;
} gps_nv_clnt_s_type;

typedef struct
{
  uint32   nv_item;
  char*    nv_val;
} gps_nv_item_type_str;

typedef struct
{
  uint32   q_NvAdded;
  uint32   q_NvWrites;
  uint32   q_NvWritesDone;
} gps_NvDataDbType;

typedef enum
{
  GPS_NV_ITEMS_TYPE_INT,
  GPS_NV_ITEMS_TYPE_STR,
  GPS_NV_ITEMS_TYPE_DUAL,
  GPS_NV_ITEMS_TYPE_IPC_DM,
} gps_NvDataEnumType;

static gps_NvDataDbType   z_gps_NvDataDb;
static boolean            b_gps_NvInitialized = FALSE;
static nv_cmd_type        z_gps_NvCmd;
static q_type             z_gps_NvWriteCmdQueue;
static gps_nv_clnt_s_type z_gps_NvClientTable[GPS_NV_CLIENT_HANDLE_MAX];
static uint32             q_gps_NvHandle = -1;
static boolean            b_gps_NvNewItemsAdded = FALSE;
static boolean            b_gps_NvLastItemsChecking = FALSE;

/* Add new items at the End,else It wont write NV items, if build got loaded without erasenand */
static const int32 gps_basic_nv_items_type_int[][NV_MAX_SIZE_TYPE_INT] =
{
  { 400,           1 }, /* NV_GPS1_CAPABILITIES_I                        */
  { 449,          35 }, /* NV_GPS1_GPS_RF_LOSS_I                         */
  { 452,           0 }, /* NV_GPS1_LOCK_I                                */
  { 831,         200 }, /* NV_GPS1_NET_DBM_SIZE_I                        */
  { 1913,       1500 }, /* NV_AAGPS_MAX_OSC_UNC_I                        */
  { 1914,          1 }, /* NV_AAGPS_MAX_OSC_UNC_RATE_I                   */
  { 1916,         34 }, /* NV_AAGPS_MAX_PLATFORM_VELOCITY_I              */
  { 1917,          1 }, /* NV_AAGPS_MAX_PLATFORM_ACCL_I                  */
  { 1918,         89 }, /* NV_AAGPS_DEFAULT_QOS_TIME_I                   */
  { 1920,     0xFF7F }, /* NV_AAGPS_POSITIONING_MODES_SUPPORTED_I        */
  { 1921,        160 }, /* NV_AAGPS_DEFAULT_REF_TIME_UNC_I               */
  { 1922,      10000 }, /* NV_AAGPS_DEFAULT_REF_POSITION_UNC_I           */
  { 1923,         15 }, /* NV_AAGPS_APP_TRACKING_GPSIDLE_THSLD_I         */
  { 1924,          0 }, /* NV_AAGPS_GPS_LOCK_CONTROL_I                   */
  { 1929,          7 }, /* NV_AAGPS_2G_MO_LRSUPPORT_I                    */
  { 1930,          1 }, /* NV_AAGPS_EMERGENCY_SERVICES_SPPRT_I           */
  { 1935,         60 }, /* NV_AAGPS_APP_TRKG_PERIODIC_REQ_DLY_MARGIN_I   */
  { 1936,         50 }, /* NV_AAGPS_DEFAULT_QOS_UNC_I                    */
  { 1937,          5 }, /* NV_AAGPS_APP_TRACKING_GPSON_THSLD_I           */
  { 1940,          1 }, /* NV_AAGPS_MT_LRSUPPORT_I                       */
  { 1944,          1 }, /* NV_GPS1_CALL_RELATED_I                        */
  { 1960,          3 }, /* NV_AAGPS_3G_MO_LRSUPPORT_I                    */
  { 1991,          0 }, /* NV_AAGPS_DEVELOPMENT_TEST_CONTROL5_I          */
  { 1993,          0 }, /* NV_GPS1_VX_LCS_AGENT_I                        */
  { 1994,          0 }, /* NV_GPS1_VX_APP_TRUSTED_SETTINGS_I             */
  { 1995,      65001 }, /* NV_GPS1_VX_NI_TELESERVICE_ID_I                */
  { 1996,          0 }, /* NV_GPS1_VX_LCS_AGENT_PREV6_ONLY_I             */
  { 1997, 0xFFFFFFFF }, /* NV_GPS1_VX_MO_MAX_DURATION_I                  */
  { 1999,          0 }, /* NV_GPS1_VX_GPS_DURING_VOICE_CALL_I            */
  { 2784,          1 }, /* NV_AAGPS_DEFAULT_ALLOW_RRC_I                  */
  { 2785,         10 }, /* NV_AAGPS_DEFAULT_MTLR_GUARD_TIMER_I           */
  { 2786,         20 }, /* NV_AAGPS_DEFAULT_SMLC_COMM_TIMEOUT_I          */
  { 2787,         20 }, /* NV_AAGPS_DEFAULT_PRESUPL_UE_TIMER1_VALUE_I    */
  { 2788,         20 }, /* NV_AAGPS_DEFAULT_PRESUPL_UE_TIMER2_VALUE_I    */
  { 2789,         20 }, /* NV_AAGPS_DEFAULT_PRESUPL_UE_TIMER3_VALUE_I    */
  { 3297,   14400000 }, /* NV_AAGPS_RTI_VALIDITY_DUR_I                   */
  { 3354,          4 }, /* NV_GPS1_MSB_BACK_OFF_FACTOR_I                 */
  { 3355,         60 }, /* NV_GPS1_MSB_BACK_OFF_MIN_I                    */
  { 3356,        900 }, /* NV_GPS1_MSB_BACK_OFF_MAX_I                    */
  { 3357,        900 }, /* NV_GPS1_MSB_BACK_OFF_RESET_I                  */
  { 3358,          1 }, /* NV_GPS1_MSB_THROTTLE_ENABLE_I                 */
  { 3486,        500 }, /* NV_SIGMAREJECT_I                              */
  { 3487,     150000 }, /* NV_VELNOISEINONESECDRIVE_I                    */
  { 3488,      80000 }, /* NV_VELNOISEINONESECPED_I                      */
  { 3489,      15000 }, /* NV_VELNOISEINONESECSTATIC_I                   */
  { 3490,          0 }, /* NV_EXTRACLOCKBIASERROR_I                      */
  { 3491,        146 }, /* NV_CONFIGMASK_I                               */
  { 3492,        600 }, /* NV_MAXPROPAGATIONWONEWMEAS_I                  */
  { 3493,          1 }, /* NV_CONFIGCONTROLFLAG_I                        */
  { 3494,          0 }, /* NV_AAGPS_DEVELOPMENT_TEST_CONTROL6_I          */
  { 3520,     0xFFFF }, /* NV_GPS1_SEEDPOS_OPTION_I                      */
  { 3756,          1 }, /* NV_GPS1_DYNAMIC_MODE_I                        */
  { 3758,          1 }, /* NV_AAGPS_USE_TRANSPORT_SECURITY_I             */
  { 4627,          1 }, /* NV_GPS1_XTRA_ENABLED_I                        */
  { 4628,         24 }, /* NV_GPS1_XTRA_DOWNLOAD_INTERVAL_I              */
  { 4629,          3 }, /* NV_GPS1_XTRA_NUM_DOWNLOAD_ATTEMPTS_I          */
  { 4630,         10 }, /* NV_GPS1_XTRA_TIME_BETWEEN_ATTEMPTS_I          */
  { 4631,          0 }, /* NV_GPS1_XTRA_AUTO_DOWNLOAD_ENABLED_I          */
  { 4695,          0 }, /* NV_CGPS_1X_PDE_SERVER_ADDR_IPV4_I             */
  { 4698,          0 }, /* NV_CGPS_1X_MPC_SERVER_ADDR_IPV4_I             */
  { 4701,          0 }, /* NV_CGPS_UMTS_PDE_SERVER_ADDR_IPV4_I           */
  { 4704,          0 }, /* NV_CGPS_1X_PDE_SERVER_PORT_I                  */
  { 4705,          0 }, /* NV_CGPS_1X_MPC_SERVER_PORT_I                  */
  { 4706,          0 }, /* NV_CGPS_UMTS_PDE_SERVER_PORT_I                */
  { 4707,          1 }, /* NV_CGPS_MO_METHOD_I                           */
  { 4708,       0x1F }, /* NV_CGPS_NMEA_SENTENCE_TYPE_I                  */
  { 4709,         99 }, /* NV_CGPS_MAX_OSC_UNC_ACTIVE_SYS_I              */
  { 4710,         99 }, /* NV_CGPS_MAX_OSC_UNC_RGS_I                     */
  { 4711,        150 }, /* NV_CGPS_MAX_OSC_UNC_TEMP_TBL_I                */
  { 4712,        600 }, /* NV_CGPS_MAX_OSC_UNC_RGS_OLD_I                 */
  { 4713,          0 }, /* NV_CGPS_MAX_CARRIER_CODE_FILTER_I             */
  { 4714,      12000 }, /* NV_CGPS_MAX_INTEGRATE_MS_I                    */
  { 4715,          0 }, /* NV_CGPS_ME_RESERVED1_I                        */
  { 4716,          0 }, /* NV_CGPS_ME_RESERVED2_I                        */
  { 4717,          0 }, /* NV_CGPS_ME_RESERVED3_I                        */
  { 4718,          0 }, /* NV_CGPS_ME_RESERVED4_I                        */
  { 4906,          1 }, /* NV_CGPS_DGPS_CORRECTIONS_ALLOWED_I            */
  { 4907,       6000 }, /* NV_CGPS_MAXIMUM_DGPS_INTERVAL_I               */
  { 4908,          1 }, /* NV_CGPS_USE_FDIC_I                            */
  { 4909,          3 }, /* NV_CGPS_ALTITUDE_HOLD_MODE_I                  */
  { 4910,         12 }, /* NV_CGPS_PDOP_MASK_I                           */
  { 4911,          8 }, /* NV_CGPS_2D_PDOP_MASK_I                        */
  { 4912,          0 }, /* NV_CGPS_REFERENCE_MODE_I                      */
  { 4913,          1 }, /* NV_CGPS_OPERATION_MODE_I                      */
  { 4914,          5 }, /* NV_CGPS_ELEVATION_MASK_I                      */
  { 4915,         50 }, /* NV_CGPS_MAX_ALMANAC_AGE_IN_WEEKS_I            */
  { 4916,          1 }, /* NV_CGPS_STEERING_ON_STARTUP_I                 */
  { 4927,          1 }, /* NV_GPS1_XTRA_TIME_INFO_ENABLED_I              */
  { 4928,      12500 }, /* NV_GPS1_XTRA_TIME_INFO_UNC_THRESH_I           */
  { 4929,       1000 }, /* NV_GPS1_XTRA_TIME_INFO_DELAY_THRESH_I         */
  { 5034,          0 }, /* NV_CGPS_QOS_OVERRIDE_ACC_THRESHOLD_I          */
  { 5035,          0 }, /* NV_CGPS_QOS_OVERRIDE_TIME_I                   */
  { 5046,          0 }, /* NV_CGPS_MEM_CONFIG_I                          */
  { 5154,          2 }, /* NV_GPS_ENGINE_TYPE_I                          */
  { 5289,         90 }, /* NV_CGPS_VCO_AGE_I                             */
  { 5290,          1 }, /* NV_CGPS_OPTIMISTIC_PUNC_I                     */
  { 5594,          0 }, /* NV_CGPS_SBAS_ENABLED_I                        */
  { 5595,          0 }, /* NV_CGPS_SBAS_USER_PREFERENCE_I                */
  { 5596,          2 }, /* NV_CGPS_DPO_CONTROL_I                         */
  { 5599,          0 }, /* NV_CGPS_ON_DEMAND_ENABLED_I                   */
  { 6214,          1 }, /* NV_CGPS_ER_ENABLE_I                           */
  { 6267,          0 }, /* NV_CGPS_QWIP_LOC_ENGINE_CONFIG_I              */
  { 6273,          1 }, /* NV_GPS_DEFAULT_OPERATING_MODE_I               */
  { 6274,          1 }, /* NV_GPS_DEFAULT_TBF_I                          */
  { 6486,         17 }, /* NV_CGPS_UTC_GPS_TIME_OFFSET_I                 */
  { 6755,          0 }, /* NV_CGPS_SENSORS_ENABLE_I                      */
  { 6758,          0 }, /* NV_GNSS_GPS_RXD_ENABLE_I                      */
  { 6759,          2 }, /* NV_GNSS_SYSTEM_CONTROL_I                      */
  { 6760,          1 }, /* NV_GNSS_GLO_CONTROL_I                         */
  { 6761,         35 }, /* NV_GNSS_RXD_PATH_LOSS_I                       */
  { 6762,         35 }, /* NV_GNSS_HBW_PATH_LOSS_I                       */
  { 6763,         35 }, /* NV_GNSS_GLO_PATH_LOSS_CENTER_MINUS7_I         */
  { 6764,         35 }, /* NV_GNSS_GLO_PATH_LOSS_CENTER_0_I              */
  { 6765,         35 }, /* NV_GNSS_GLO_PATH_LOSS_CENTER_PLUS6_I          */
  { 6766,          0 }, /* NV_GNSS_LBW_RXD_GROUP_DELAY_DIFF_I            */
  { 6767,          0 }, /* NV_GNSS_TIMED_OUTPUT_MODE_SELECTION_I         */
  { 6768,          0 }, /* NV_GNSS_TIMED_OUTPUT_FIXED_BIAS_OFFSET_I      */
  { 6769,          1 }, /* NV_GNSS_MAX_M_RXD_TASKS_I                     */
  { 6770,         90 }, /* NV_GNSS_MAX_GNSS_TASKS_I                      */
  { 6771,         80 }, /* NV_GNSS_SHALLOW_FIRST_DWELL_DURATION_I        */
  { 6772,        320 }, /* NV_GNSS_SHALLOW_SUCCESSIVE_DWELLS_DURATION_I  */
  { 6773,       1000 }, /* NV_GNSS_DEEP_FIRST_DWELL_DURATION_I           */
  { 6774,       4000 }, /* NV_GNSS_DEEP_SECOND_DWELL_DURATION_I          */
  { 6775,      12000 }, /* NV_GNSS_DEEP_THIRD_DWELL_DURATION_I           */
  { 6776,        640 }, /* NV_GNSS_VERIFICATION_DWELL_DURATION_I         */
  { 6777,         80 }, /* NV_GNSS_PROBATIONARY_SCAN_DWELL_DURATION_I    */
  { 6778,          0 }, /* NV_GNSS_ACQUISITION_SAMPLING_MODE_I           */
  { 6779,          1 }, /* NV_GNSS_PP_RESERVED_I                         */
  { 6780,         16 }, /* NV_GNSS_MAX_NUM_GPS_SV_MEASUREMENTS_IN_FIX_I  */
  { 6781,          0 }, /* NV_GNSS_SPCFC_GPS_SV_MEASURES_NOT_IN_FIX_I    */
  { 6782,         14 }, /* NV_GNSS_MAX_NUM_GLO_SV_MEASUREMENTS_IN_FIX_I  */
  { 6783,          0 }, /* NV_GNSS_SPCFC_GLO_SV_MEASURES_NOT_IN_FIX_I    */
  { 6784,          0 }, /* NV_GNSS_DISABLE_GGTB_CONSTRAINT_IN_WLS_I      */
  { 6785,          0 }, /* NV_GNSS_DISABLE_GLO_ONLY_KF_I                 */
  { 6786,          0 }, /* NV_GNSS_ENABLE_EXTENDED_ON_FEATURE_FOR_GNSS_I */
  { 6787,          0 }, /* NV_GNSS_SPCFC_GLO_SV_NAV_DATA_NOT_DECODED_I   */
  { 6788,          0 }, /* NV_GNSS_SPCFC_GPS_SV_NAV_DATA_NOT_DECODED_I   */
  { 6789,       0x3F }, /* NV_GNSS_NMEA_SENTENCE_TYPE_I                  */
  { 6790,          0 }, /* NV_GNSS_NMEA_EXTENDED_SENTENCE_TYPE_I         */
  { 6791,          0 }, /* NV_GNSS_801_B_PROTOCOL_SUPPORT_I              */
  { 6792, 0x00020000 }, /* NV_GNSS_SUPL_VERSION_I                        */
  { 6793,          0 }, /* NV_GNSS_RRLP8_RRC8_SUPPORTED_I [OBSOLETE]     */
  { 6794,        200 }, /* NV_GNSS_DPO_DWELL_DURATION_I                  */
  { 6816,          1 }, /* NV_GNSS_MGP_ERROR_RECOVERY_I                  */
  { 6845,          0 }, /* NV_GNSS_EMBEDDED_XTRA_CLIENT_ENABLED_I        */
  { 6846,          0 }, /* NV_GNSS_WWAN_NETWORK_PREFERENCE_I             */
  { 6847,          0 }, /* NV_GNSS_MEDIUM_PREFERENCES_NUMBER_I           */
  { 6848,          0 }, /* NV_GNSS_MEDIUM_PREFERENCE_LIST_I              */
  { 6849,          1 }, /* NV_GNSS_AUTO_NMEA_COMPORT_ENABLED_I           */
  { 6851,          0 }, /* NV_GNSS_EMBEDDED_XTRA_TIME_CLIENT_ENABLED_I   */
  { 6852,          0 }  /* NV_GNSS_1XUP_MSA_TRUSTED_MODE_DISABLE_I       */

                        /* ADD FUTURE NV ITEMS HERE */
};

static const gps_nv_item_type_str gps_basic_nv_items_type_str[] =
{
  { 4632, NV_AAGPS_DEFAULT_XTRA_PRIMARY_SERVER_URL}, /* NV_GPS1_XTRA_PRIMARY_SERVER_URL_I        */
  { 4633, NV_AAGPS_DEFAULT_XTRA_SECONDARY_SERVER_URL}, /* NV_GPS1_XTRA_SECONDARY_SERVER_URL_I      */
  { 4634, NV_AAGPS_DEFAULT_XTRA_TERTIARY_SERVER_URL}, /* NV_GPS1_XTRA_TERTIARY_SERVER_URL_I       */
  { 4697, "\0"},                                    /* NV_CGPS_1X_PDE_SERVER_ADDR_URL_I         */
  { 4703, "\0"},                                    /* NV_CGPS_UMTS_PDE_SERVER_ADDR_URL_I       */
  { 4930, NV_AAGPS_DEFAULT_XTRA_PRIMARY_SNTP_SERVER_URL},  /* NV_GPS1_XTRA_PRIMARY_SNTP_SERVER_URL_I   */
  { 4931, NV_AAGPS_DEFAULT_XTRA_SECONDARY_SNTP_SERVER_URL},/* NV_GPS1_XTRA_SECONDARY_SNTP_SERVER_URL_I */
  { 4932, NV_AAGPS_DEFAULT_XTRA_TERTIARY_SNTP_SERVER_URL}  /* NV_GPS1_XTRA_TERTIARY_SNTP_SERVER_URL_I  */
};

static const int32 gps_basic_nv_items_type_dual[][NV_MAX_SIZE_TYPE_DUAL] =
{
  { 5047, 4, 0}, /* NV_CGPS_NMEA_CONFIG_INFO_I */
};

static const int32 gps_basic_nv_items_type_ipc_dm[][NV_MAX_SIZE_TYPE_IPC_DM] =
{
  { 4544, 0x00000000,
          0x000FFFEF, /* IPCs to MGP_MC task */
          0x00000003, /* IPCs to MGP_CC task */
          0x00000001, /* IPCs to MGP_PP task */
          0x00000000, /* IPCs to MGP_GPSFFT task */
          0xFFFFFFFF, /* IPCs to MGP_NF task */
          0xFFFFFFFF, /* IPCs to MGP_CD task */
          0x0008FFFF, /* IPCs to SM_LM task */
          0x0008FFFF, /* IPCs to SM_TM task */
          0x0008FFFF, /* IPCs to SM_PDCOMM_TCP task */
          0x0008FFFF, /* IPCs to SM_PDCOMM_TD1 task */
          0x0008FFFF, /* IPCs to GPSDIAG task */
          0x0008FFFF, /* IPCs to SM_PDCOMM_WMS task */
          0x00080000, /* IPCs to GPS_FS task */
          0x0008000F, /* IPCs to MGP_PGI task */
          0x00080000, /* IPCs to LOC_MIDDLEWARE task */
          0x0008FFFF, /* IPCs to LBS task */
          0x00FFFFFF, /* IPCs to TLM task */
          0x00FFFFFF, /* IPCs to XTM task */
          0xFFFFFFFF, /* IPCs to SDP task */
          0x00000000, /* IPCs to SM_GM task */
          0x00000000, /* IPCs to LOC_LTE_OTDOA_CONTROL task */
          0x00000000, /* IPCs to LOC_LTE_OTDOA_MEASPROC task */
          0x00000000, /* IPCs to GNSS_MSGR task */
          0x00000000, /* IPCs to ESP task */
          0xFFFFFFFF, /* IPCs to WLM task */
          0x00000000,
          0x00000000,
          0x00000000,
          0x00000000,
          0x00000000,
          0x00000000
  } /* NV_AAGPS_IPC_DM_THREAD_MASK_I */
};

/*
 * Function definitions
*/
static uint32 gps_NvRegister (os_ThreadIdEnumType  e_ThreadId,
                              gps_nv_cmd_cb_f_type nv_clnt_cb_fn);
static boolean gps_NvAddtoWriteQueue (nv_items_enum_type e_NvItem,
                                      const void*        p_NvItem,
                                      uint32             q_Handle);
static void gps_NvWriteItem (const gps_nv_item_s_type* p_NvItemData);
static void gps_NvWriteDoneCb (nv_items_enum_type e_NvItem,
                               void*              p_Data,
                               nv_stat_enum_type  e_Status);
static nv_stat_enum_type gps_NvReadItem (nv_items_enum_type e_NvItem,
                                         void*              p_NvItem);
static uint32 gps_NvAddtoInternalQueue(gps_NvDataEnumType e_NvDataType,
                                       nv_items_enum_type e_NvItem,
                                       void*              p_NvItemData,
                                       uint32             q_TotalCount,
                                       uint32             q_CurrentItem);

/*===========================================================================
FUNCTION gps_NvInit

DESCRIPTION
  This function is used to initialize internal gps_default-nv data structure.
  It will be called when the first client calls gps_NvRegister.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
static void gps_NvInit (void)
{
  if (FALSE == b_gps_NvInitialized)
  {
    (void)q_init(&z_gps_NvWriteCmdQueue);
    memset(&z_gps_NvClientTable, 0, (sizeof(gps_nv_clnt_s_type) * GPS_NV_CLIENT_HANDLE_MAX));
    memset(&z_gps_NvDataDb,      0,  sizeof(gps_NvDataDbType));
    b_gps_NvInitialized = TRUE;
  }
} /* gps_NvInit */

/*===========================================================================
FUNCTION gps_NvRegister

DESCRIPTION
  This function is used to register a client that will use this NV module.
  It will register the client's callback function after an NV Write command
  has completed, and return a client handle that is needed for future writes.

RETURN VALUE
  Client handle value (or 0xFF if error)

DEPENDENCIES
  None
===========================================================================*/
static uint32 gps_NvRegister
(
  os_ThreadIdEnumType  e_ThreadId,
  gps_nv_cmd_cb_f_type nv_clnt_cb_fn
)
{
  uint32 q_Index = 0;

  /* Initialize GPS NV Loader Module Initialization */
  gps_NvInit();

  /* Passed argument sanity check */
  if (e_ThreadId >= THREAD_ID_MAX)
  {
    /* FAILED - Invalid ThreadId parameter */
    return (uint32)GPS_NV_CLIENT_HANDLE_ERROR;
  }

  /* Passed argument sanity check */
  if (NULL == nv_clnt_cb_fn)
  {
    /* FAILED - NULL parameter */
    return (uint32)GPS_NV_CLIENT_HANDLE_ERROR;
  }

  /* Look for room in the client table to register NV write cmd callback */
  for (q_Index = 0; q_Index < GPS_NV_CLIENT_HANDLE_MAX; q_Index++)
  {
    if (TRUE == z_gps_NvClientTable[q_Index].active)
    {
      if (e_ThreadId == z_gps_NvClientTable[q_Index].thread_id)
      {
        /* Already registered */
        return q_Index;
      }
    }
    else /* z_gps_NvClientTable[q_Index].active == FALSE */
    {
      z_gps_NvClientTable[q_Index].active    = TRUE;
      z_gps_NvClientTable[q_Index].thread_id = e_ThreadId;
      z_gps_NvClientTable[q_Index].nv_cb_fn  = nv_clnt_cb_fn;
      return q_Index;
    }
  }

  /* FAILED - Maximum number of clients registered */
  return (uint32)GPS_NV_CLIENT_HANDLE_ERROR;
} /* gps_NvRegister */

/*===========================================================================
FUNCTION gps_NvWriteDoneCb

DESCRIPTION
  This function handles the NV write done indication.

PARAMETERS
  e_NvItem -- NV item on which write is complete
  p_Data ---- Value of the Item.
  e_Status -- Completion Status (success or failure).

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
static void gps_NvWriteDoneCb
(
  nv_items_enum_type    e_NvItem,
  void*                 p_Data,
  nv_stat_enum_type     e_Status
)
{
  /* If write failed, send an error status */
  if (e_Status != NV_DONE_S)
  {
    MSG_ERROR("gps_NvWriteDoneCb :NV Write Failed on item %d Reason %d", e_NvItem, e_Status, 0);
  }

  /* Do nothing for now */
} /* gps_NvWriteDoneCb */

/*===========================================================================
FUNCTION gps_NvWriteItem

DESCRIPTION
  This function is used to handle a NV Write command for NV. This function
  services the request at the head of the NV Write command queue and waits
  until NV returns with a response.

RETURN VALUE
  None

DEPENDENCIES
  gps default NV write callback needs to be registered via call to gps_NvWriteDone
===========================================================================*/
static void gps_NvWriteItem
(
  const gps_nv_item_s_type* p_NvItemData
)
{
  /* Send NV write command to NV module only if a request exists in queue */

  if (p_NvItemData != NULL)
  {
    /* Populate NV write command */
    z_gps_NvCmd.item       = p_NvItemData->item;
    z_gps_NvCmd.cmd        = NV_WRITE_F;
    z_gps_NvCmd.data_ptr   = (nv_item_type*)&(p_NvItemData->data);
    z_gps_NvCmd.tcb_ptr    = os_Self();
    z_gps_NvCmd.sigs       = C_OS_FLAG_NV_WRITE;
    z_gps_NvCmd.done_q_ptr = NULL;

    /* Send NV write command to NV module */
    nv_cmd(&z_gps_NvCmd);

    z_gps_NvDataDb.q_NvWrites++;
    MSG_HIGH("WRITING  GPS NV Item: %d [%d/%d]", p_NvItemData->item, z_gps_NvDataDb.q_NvWrites, z_gps_NvDataDb.q_NvAdded);
  }
  else
  {
    /* No GPS NV Items exist in the Queue, which means all NV items written successfully */
    MSG_HIGH("COMPLETED WRITING [%d/%d] GPS NV ITEMS TO DEFAULT VALUES at time=%d", z_gps_NvDataDb.q_NvWritesDone, z_gps_NvDataDb.q_NvWrites, timetick_get());
  }
} /* gps_NvWriteItem */

/*===========================================================================
FUNCTION gps_NvAddtoWriteQueue

DESCRIPTION
  This function is used to queue NV Write commands to NV. The writes to NV
  are performed in asynchronous fashion.

RETURN VALUE
  TRUE  - NV write command successfully sent to NV
  FALSE - NV write command failed to be sent to NV

DEPENDENCIES
  GPS DEFAULT NV write callback needs to be registered via call to gps_NvRegister
===========================================================================*/
static boolean gps_NvAddtoWriteQueue
(
  nv_items_enum_type e_NvItem,
  const void* p_NvItem,
  uint32 q_Handle
)
{
  gps_nv_item_s_type* nv_ptr = NULL;

  if (b_gps_NvInitialized)
  {
    /* Sanity checks for Passed arguments */
    if ((NULL == p_NvItem) || (q_Handle >= GPS_NV_CLIENT_HANDLE_MAX))
    {
      /* FAILED sanity check */
      return FALSE;
    }

    /* Since NV read and writes should normally be done in GPS idle state
     * (no sessions), no separate allocation of space is memory is done.
     * The buffers will be used from a dynamic buffer pool.
     */
    nv_ptr = (gps_nv_item_s_type*)os_MemAlloc(sizeof(gps_nv_item_s_type), OS_MEM_SCOPE_TASK);

    /* Sanity check for Dynamic buffer allocation */
    if (NULL == nv_ptr)
    {
      /* FAILED sanity check */
      return FALSE;
    }

    MSG_HIGH("QUEUEING GPS NV ITEM: %d", e_NvItem, 0, 0);

    /* Populate NV write command queue element */
    nv_ptr->item   = e_NvItem;
    nv_ptr->data   = *((nv_item_type*)p_NvItem);
    nv_ptr->handle = q_Handle;

    /* Queue a request for writing to NV. Put the command into the
     * NV Write command queue and services them one at a time.
     *
     *  NOTE - The funky check for nv_ptr->link == nv_ptr and the
     *  undesirable cast of nv_ptr to (q_link_type) is being done to
     *  resolve a Lint HIGH error.  Lint thinks there is a potential
     *  memory leak unless nv_ptr is passed to another function OR
     *  freed before returning from this function.  The IF check was
     *  added for safety since an explicit cast like this is very
     *  dangerous and will cause undesired behavior if nv_ptr->link
     *  and nv_ptr are not equivalent.  We will FATAL ERROR if they
     *  are not equivalent.
     */
    if ((void *)&nv_ptr->link == (void *) nv_ptr)
    {
      q_put(&z_gps_NvWriteCmdQueue, (q_link_type *) nv_ptr);
      z_gps_NvDataDb.q_NvAdded++;
    }
    else
    {
      ERR_FATAL("gps_NvAddtoWriteQueue: queue.h API structure invalid", 0, 0, 0);
    }

    return TRUE;
  }
  else
  {
    /* GPS NV Loader module not initialized */
    return FALSE;
  }
} /* gps_NvAddtoWriteQueue */

/*===========================================================================
FUNCTION gps_NvWriteDone

DESCRIPTION
  This function is used to handle NV write callback status. If there are any
  clients who issued write requests, they will be notified and any pending
  write requests will be subsequently handled.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void gps_NvWriteDone (void)
{
  /* First dequeue the write request at the head */
  gps_nv_item_s_type* nv_ptr = ((gps_nv_item_s_type*)q_get(&z_gps_NvWriteCmdQueue));

  if (NULL != nv_ptr)
  {
    /* Make sure first dequeued element is same as current NV write element */
    if (z_gps_NvCmd.item == nv_ptr->item)
    {
      /* Check NV write command result status */
      if (z_gps_NvCmd.status != NV_DONE_S)
      {
        /* NV write failed. */
        MSG_ERROR("gps_NvWriteDone: FAILED - Error Status %d", z_gps_NvCmd.status, 0, 0);
      }

      /* Out-of-bound array index sanity check */
      else if (nv_ptr->handle >= GPS_NV_CLIENT_HANDLE_MAX || NULL == &nv_ptr->data)
      {
        MSG_ERROR("gps_NvWriteDone: FAILED - bad parameters", 0, 0, 0);
      }
      else
      {
        /* Call appropriate client's registered callback function */
        z_gps_NvClientTable[nv_ptr->handle].nv_cb_fn(nv_ptr->item, (void*)&nv_ptr->data, z_gps_NvCmd.status);

        z_gps_NvDataDb.q_NvWritesDone++;

        /* Handle next NV write request */
        if (b_gps_NvInitialized)
        {
          gps_nv_item_s_type* p_NvItemData = NULL;
          p_NvItemData = (gps_nv_item_s_type*)q_check(&z_gps_NvWriteCmdQueue);
          gps_NvWriteItem(p_NvItemData);
        }
      }
    }
  }
  os_MemFree((void**)&nv_ptr);
} /* gps_NvWriteDone */

/*===========================================================================
FUNCTION gps_NvReadItem

DESCRIPTION
  This function is used to perform a synchronous read from NV.

RETURN VALUE
  TRUE  - NV read success
  FALSE - NV read failure (read default value instead)

DEPENDENCIES
  None
===========================================================================*/
static nv_stat_enum_type gps_NvReadItem
(
  nv_items_enum_type e_NvItem,
  void* p_NvItem
)
{
  nv_cmd_type cmd_buf;

  /* Passed argument sanity check */
  if (NULL == p_NvItem)
  {
    /* FAILED - NULL parameter */
    return NV_BADPARM_S;
  }

  /* Populate NV read command */
  cmd_buf.item       = e_NvItem;
  cmd_buf.cmd        = NV_READ_F;
  cmd_buf.data_ptr   = (nv_item_type*)p_NvItem;
  cmd_buf.tcb_ptr    = os_Self();
  cmd_buf.sigs       = C_GPS_FS_NV_READ_SIGNAL;
  cmd_buf.done_q_ptr = NULL;

  /* Clear NV Read signal before waiting on it */
  (void)os_ClrLocalSignal(C_GPS_FS_NV_READ_SIGNAL);

  /* Send NV read command to NV module */
  nv_cmd(&cmd_buf);

  /* Block until NV Read signal is set */
  (void)os_WaitLocalSignal(C_GPS_FS_NV_READ_SIGNAL);

  /* Clear NV Read signal after it is read out */
  (void)os_ClrLocalSignal(C_GPS_FS_NV_READ_SIGNAL);

  return (nv_stat_enum_type)cmd_buf.status;
} /* gps_NvReadItem */

/*===========================================================================
FUNCTION gps_NvInitDefaultWrites

DESCRIPTION
  This function is used to read/write default nv items.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void gps_NvInitDefaultWrites (void)
{
  int32        n_i;
  int          n_TotalCount = 0;
  uint32       q_NextIndex;

  MSG_HIGH("STARTED QUEUEING GPS NV ITEMS at time=%d", timetick_get(), 0, 0);

  /* Register with NV */
  q_gps_NvHandle = gps_NvRegister(THREAD_ID_GPS_FS, gps_NvWriteDoneCb);

  /* Reset optimization flags */
  b_gps_NvNewItemsAdded     = FALSE;
  b_gps_NvLastItemsChecking = FALSE;

  /* Reading and Adding to Queue INT Type NV items */
  n_TotalCount = (sizeof(gps_basic_nv_items_type_int) / (sizeof(int32)*NV_MAX_SIZE_TYPE_INT));

  for (n_i = 0; n_i < n_TotalCount;)
  {
    q_NextIndex = gps_NvAddtoInternalQueue(GPS_NV_ITEMS_TYPE_INT,
                                           (nv_items_enum_type)gps_basic_nv_items_type_int[n_i][NV_ITEM],
                                           (void*)&gps_basic_nv_items_type_int[n_i][NV_ITEM_VAL],
                                           n_TotalCount,
                                           n_i);

    /* Update next index if it is read optimized (NVs already written previously) */
    (q_NextIndex != 1) ? (n_i = q_NextIndex) : (n_i++);

  } /* Reading and Adding to Queue INT Type NV items complete */

  /* Reset optimization flags */
  b_gps_NvNewItemsAdded     = FALSE;
  b_gps_NvLastItemsChecking = FALSE;

  /* Reading and Adding to Queue STRING Type NV items */
  n_TotalCount = (sizeof(gps_basic_nv_items_type_str) / (sizeof(gps_nv_item_type_str)));

  for (n_i = 0; n_i < n_TotalCount;)
  {
    q_NextIndex = gps_NvAddtoInternalQueue(GPS_NV_ITEMS_TYPE_STR,
                                           (nv_items_enum_type)gps_basic_nv_items_type_str[n_i].nv_item,
                                           (void*)gps_basic_nv_items_type_str[n_i].nv_val,
                                           n_TotalCount,
                                           n_i);

    /* Update next index if it is read optimized (NVs already written previously) */
    (q_NextIndex != 1) ? (n_i = q_NextIndex) : (n_i++);

  } /* Reading and Adding to Queue STRING Type NV items Complete */

  /* Reset optimization flags */
  b_gps_NvNewItemsAdded     = FALSE;
  b_gps_NvLastItemsChecking = FALSE;

  /* Reading and Adding to Queue DUAL Value Type NV items */
  n_TotalCount = (sizeof(gps_basic_nv_items_type_dual) / (sizeof(int32)*NV_MAX_SIZE_TYPE_DUAL));

  for (n_i = 0; n_i < n_TotalCount;)
  {
    q_NextIndex = gps_NvAddtoInternalQueue(GPS_NV_ITEMS_TYPE_DUAL,
                                           (nv_items_enum_type)gps_basic_nv_items_type_dual[n_i][NV_ITEM],
                                           (void*)&gps_basic_nv_items_type_dual[n_i],
                                           n_TotalCount,
                                           n_i);

    /* Update next index if it is read optimized (NVs already written previously) */
    (q_NextIndex != 1) ? (n_i = q_NextIndex) : (n_i++);

  } /* Reading and Adding to Queue DUAL Value Type NV items */

  /* Reset optimization flags */
  b_gps_NvNewItemsAdded     = FALSE;
  b_gps_NvLastItemsChecking = FALSE;

  /* Reading and Adding to Queue IPC DM Value Type NV item */
  n_TotalCount = 1;

  for (n_i = 0; n_i < n_TotalCount;)
  {
    q_NextIndex = gps_NvAddtoInternalQueue(GPS_NV_ITEMS_TYPE_IPC_DM,
                                           (nv_items_enum_type)gps_basic_nv_items_type_ipc_dm[n_i][NV_ITEM],
                                           (void*)&gps_basic_nv_items_type_ipc_dm[n_i],
                                           n_TotalCount,
                                           n_i);

    /* Update next index if it is read optimized (NVs already written previously) */
    (q_NextIndex != 1) ? (n_i = q_NextIndex) : (n_i++);

  } /* Reading and Adding to Queue DUAL Value Type NV items */

  MSG_HIGH("COMPLETED QUEUEING %d GPS NV ITEMS at time=%d", z_gps_NvDataDb.q_NvAdded, timetick_get(), 0);

  /* Starting Writing NV's one by one from Queue */
  if (b_gps_NvInitialized)
  {
    MSG_HIGH("STARTED WRITING %d GPS NV ITEMS TO DEFAULT VALUES at time=%d", z_gps_NvDataDb.q_NvAdded, timetick_get(), 0);
    gps_nv_item_s_type* p_NvItemData = NULL;
    p_NvItemData = (gps_nv_item_s_type*)q_check(&z_gps_NvWriteCmdQueue);
    gps_NvWriteItem(p_NvItemData);
  }
} /* gps_NvInitDefaultWrites */

/*===========================================================================
FUNCTION gps_NvAddtoInternalQueue

DESCRIPTION
  This function is used to add NV write requests to internal queue.

RETURN VALUE
  Index value of next element in Internal Queue

DEPENDENCIES
  None
===========================================================================*/
static uint32 gps_NvAddtoInternalQueue
(
  gps_NvDataEnumType e_NvDataType,
  nv_items_enum_type e_NvItem,
  void*              p_NvItemData,
  uint32             q_TotalCount,
  uint32             q_CurrentItem
)
{
  nv_stat_enum_type n_enum_state;
  nv_item_type      z_NvItemReadBuf;
  nv_item_type      z_NvItemWriteBuf;
  int32*            p_NvItemInt;
  uint32            q_I;

  n_enum_state = gps_NvReadItem(e_NvItem, (void*)&z_NvItemReadBuf);

  if ((NV_DONE_S == n_enum_state) && (FALSE == b_gps_NvNewItemsAdded))
  {
    /* LIST size should be twice to GPS_NV_VERIFY_COUNT to optimize Nv reading from 2nd boot */
    if (q_TotalCount > GPS_NV_MIN_LIST_SIZE)
    {
      /* If First GPS_NV_VERIFY_COUNT NV items Already written then jump to
         Total size - GPS_NV_VERIFY_COUNT to verify any new items added at End of list */
      if (q_CurrentItem >= GPS_NV_LOOP_LIMIT)
      {
        b_gps_NvLastItemsChecking = TRUE;

        if (q_CurrentItem < (q_TotalCount - GPS_NV_VERIFY_COUNT))
        {
          return (q_TotalCount - GPS_NV_VERIFY_COUNT);
        }
      }
    }
  }

  if ((NV_DONE_S != n_enum_state) && (NV_NOMEM_S != n_enum_state))
  {
    if (TRUE == b_gps_NvLastItemsChecking)
    {
      /* Means some items added at the end of list, so start reading from beginning of the list
         with out read optimization */
      b_gps_NvNewItemsAdded = TRUE;
      b_gps_NvLastItemsChecking = FALSE;
      return 0; /* Reset loop from beginning */
    }

    /* Writing to special NV item (Dual or IPC DM) */
    if ((GPS_NV_ITEMS_TYPE_DUAL == e_NvDataType) ||
        (GPS_NV_ITEMS_TYPE_IPC_DM == e_NvDataType) )
    {
      p_NvItemInt = (int32*)p_NvItemData;

      if (NV_CGPS_NMEA_CONFIG_INFO_I == (nv_items_enum_type)e_NvItem) /* NV ITEM: 5047 */
      {
        z_NvItemWriteBuf.cgps_nmea_config_info.nmea_port_type      = (uint32)p_NvItemInt[NV_ITEM_VAL];
        z_NvItemWriteBuf.cgps_nmea_config_info.nmea_reporting_type = (uint32)p_NvItemInt[NV_ITEM_VAL2];
      }
      else if (NV_AAGPS_IPC_DM_THREAD_MASK_I == (nv_items_enum_type)e_NvItem) /* NV ITEM: 4544 */
      {
        for (q_I = 0; q_I < NV_MAX_SIZE_TYPE_IPC_DM - 1; q_I++)
        {
          z_NvItemWriteBuf.aagps_ipc_dm_thread_mask[q_I] = (uint32)p_NvItemInt[q_I+1];
        }
      }

      if (FALSE == gps_NvAddtoWriteQueue((nv_items_enum_type)e_NvItem, (void*)&z_NvItemWriteBuf, q_gps_NvHandle))
      {
        MSG_ERROR("FAILED WRITE FOR GPS NV ITEM: %d, Continue...", e_NvItem, 0, 0);
      }
    }

    else if (FALSE == gps_NvAddtoWriteQueue((nv_items_enum_type)e_NvItem, (void*)p_NvItemData, q_gps_NvHandle))
    {
      MSG_ERROR("FAILED WRITE FOR GPS NV ITEM: %d, Continue...", e_NvItem, 0, 0);
    }
  }

  else if (NV_DONE_S == n_enum_state)
  {
    /* No OP */
  }

  else
  {
    MSG_ERROR("FAILED for GPS NV item: %d, ERROR CODE :%d", e_NvItem, n_enum_state, 0);
  }

  return 1;
} /* gps_NvAddtoInternalQueue */
