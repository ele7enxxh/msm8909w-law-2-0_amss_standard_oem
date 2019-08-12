/******************************************************************************
  @file:  loc_pd.c
  @brief: Location Middleware PA module

  DESCRIPTION
  This module configures/retrieves the NV setting of location
  engine on the modem, and processes EFS data.

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  -----------------------------------------------------------------------------
  Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  Copyright (c) 2013-2014 QUALCOMM Atheros Incorporated.
  All Rights Reserved.QUALCOMM Atheros Proprietary and Confidential.
  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc. All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc. 
  -----------------------------------------------------------------------------
 ******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
06/28/16   yh       Parsing velocity ENU
04/13/16   yh       Always update GPS engine state
10/06/15   yh       QZSS
09/03/15   yh       Use the fatest TBF for pdsm_get_position option
08/26/15   pa       Modifying leap second to 17 and removing redundant constants
06/11/15   sjk      LB 2.0 Integration
03/24/12   ss       Supporting operation mode settting to effect Geofencing operation mode
03/10/14   ah       Added support for Get Avail WWAN Pos
03/14/13   ssu      Support for SUPL MT ES and improved SI rules.
04/11/11   rh       Fixed race condition issue when calling pdsm_client_init()
03/29/10   ns       Changed Loc MW to be an OEM type client

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_pd.c#1 $
$DateTime: 2016/12/13 07:59:45 $
======================================================================*/

#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"
#include "target.h"

#include "msg.h"
#include "queue.h"
#include "time_svc.h"

#include "math.h"

#include "aries_os_api.h"
#include "pdapi.h"
#include "pdsm_atl.h"
#include "loc_api_2.h"

#include "loc_task.h"
#include "loc_client.h"
#include "loc_pd.h"
#include "loc_conn.h"
#include "loc_xtra.h"
#include "loc_pa.h"
#include "loc_ni.h"
#include "loc_wifi.h"
#include "loc_api_internal.h"
#include "gnss_calendar_util.h"
#include "loc_conv_locEng_qmiLoc.h"
#include "loc_qmi_shim.h"
#include "gnss_consts.h"

#define NUM_OF_SVS_IN_GPS_SYSTEM 24

#define PD_DEFAULT_WAIT_TIMEOUT  120000 // 2 minutes
#define PD_DEFAULT_FIX_INTERVAL  1000   // 1 second

// Number of seconds between January 1, 1970 and January 6, 1980
#define GPS_TO_UTC_DELTA_SECONDS        (3657 * 24 * 60 * 60)
#define SECONDS_IN_ONE_DAY   (24 * 60 * 60)

#define LAT_LON_SCALE_FACTOR     100000000 // (See 80-VF767-1)

#define RAD2DEG                  (180.0 / 3.1415926535898 )
#define UNCERTAINTY_TABLE_SIZE   30
#define INV_SQRT_2               0.7071067811865

#define LOC_TO_SM_LAT_SCALE_FACTOR   (23860929.4222)   // 2^32/180
#define LOC_TO_SM_LON_SCALE_FACTOR   (11930464.7111)  // 2^31/180

/* offset (in meters) that should be subtracted from altitude sent
   in the pdsm position report to calculate the altitude w.r.t
   WGS-84 ellipsoid  */
#define LOC_PD_ALTITUDE_WRT_WGS84_ELLIPSOID_OFFSET (500)

// Ten by Ten Degree WGS-84 Geoid Separation from -180 to +170 Degrees of Longitude in meters (approximate)
// Source:  Defense Mapping Agency. 12 Jan 1987. GPS UE Relevant WGS-84 Data Base Package.
//                Washington, DC: Defense Mapping Agency
// Note:    Geodal Separation(GS) is the height of MSL above the ellipsoid.
//                Thus, MSL = HAE - GS
static const int positionUncertaintyTable[UNCERTAINTY_TABLE_SIZE] =
   {  1,    1,    1,   2,    2,      3,    4,    6,
      8,   12,   16,   24,   32,     48,   64,   96,
    128,  192,  256,  384,  512,    768, 1024, 1536,
   2048, 3072, 4096, 6144, 8192, 12288};

// Table used to compute Geoidal separation
static int GeoSep[19][36] =
{
    //    90 Degrees N Latitude
    { 13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13 } ,
    //    80 Degrees N Latitude
    { 3,1,-2,-3,-3,-3,-1,3,1,5,9,11,19,27,31,34,33,34,33,34,28,23,17,13,9,4,4,1,-2,-2,0,2,3,2,1,1 },
    //    70 Degrees N Latitude
    { 2,2,1,-1,-3,-7,-14,-24,-27,-25,-19,3,24,37,47,60,61,58,51,43,29,20,12,5,-2,-10,-14,-12,-10,-14,-12,-6,-2,3,6,4 },
    //    60 Degrees N Latitude
    {2,9,17,10,13,1,-14,-30,-39,-46,-42,-21,6,29,49,65,60,57,47,41,21,18,14,7,-3,-22,-29,-32,-32,-26,-15,-2,13,17,19,6},
    //    50 Degrees N Latitude
    {-8,8,8,1,-11,-19,-16,-18,-22,-35,-40,-26,-12,24,45,63,62,59,47,48,42,28,12,-10,-19,-33,-43,-42,-43,-29,-2,17,23,22,6,2},
    //    40 Degrees N Latitude
    {-12,-10,-13,-20,-31,-34,-21,-16,-26,-34,-33,-35,-26,2,33,59,52,51,52,48,35,40,33,-9,-28,-39,-48,-59,-50,-28,3,23,37,18,-1,-11},
    //    30 Degrees N Latitude
    {-7,-5,-8,-15,-28,-40,-42,-29,-22,-26,-32,-51,-40,-17,17,31,34,44,36,28,29,17,12,-20,-15,-40,-33,-34,-34,-28,7,29,43,20,4,-6},
    //    20 Degrees N Latitude
    {5,10,7,-7,-23,-39,-47,-34,-9,-10,-20,-45,-48,-32,-9,17,25,31,31,26,15,6,1,-29,-44,-61,-67,-59,-36,-11,21,39,49,39,22,10},
    //    10 Degrees N Latitude
    {13,12,11,2,-11,-28,-38,-29,-10,3,1,-11,-41,-42,-16,3,17,33,22,23,2,-3,-7,-36,-59,-90,-95,-63,-24,12,53,60,58,46,36,26},
    //    0 Degrees Latitude
    {22,16,17,13,1,-12,-23,-20,-14,-3,14,10,-15,-27,-18,3,12,20,18,12,-13,-9,-28,-49,-62,-89,-102,-63,-9,33,58,73,74,63,50,32},
    //    10 Degrees S Latitude
    {36,22,11,6,-1,-8,-10,-8,-11,-9,1,32,4,-18,-13,-9,4,14,12,13,-2,-14,-25,-32,-38,-60,-75,-63,-26,0,35,52,68,76,64,52},
    //    20 Degrees S Latitude
    {51,27,10,0,-9,-11,-5,-2,-3,-1,9,35,20,-5,-6,-5,0,13,17,23,21,8,-9,-10,-11,-20,-40,-47,-45,-25,5,23,45,58,57,63},
    //    30 Degrees S Latitude
    {46,22,5,-2,-8,-13,-10,-7,-4,1,9,32,16,4,-8,4,12,15,22,27,34,29,14,15,15,7,-9,-25,-37,-39,-23,-14,15,33,34,45},
    //    40 Degrees S Latitude
    {21,6,1,-7,-12,-12,-12,-10,-7,-1,8,23,15,-2,-6,6,21,24,18,26,31,33,39,41,30,24,13,-2,-20,-32,-33,-27,-14,-2,5,20},
    //    50 Degrees S Latitude
    {-15,-18,-18,-16,-17,-15,-10,-10,-8,-2,6,14,13,3,3,10,20,27,25,26,34,39,45,45,38,39,28,13,-1,-15,-22,-22,-18,-15,-14,-10},
    //    60 Degrees S Latitude
    {-45,-43,-37,-32,-30,-26,-23,-22,-16,-10,-2,10,20,20,21,24,22,17,16,19,25,30,35,35,33,30,27,10,-2,-14,-23,-30,-33,-29,-35,-43},
    //    70 Degrees S Latitude
    {-61,-60,-61,-55,-49,-44,-38,-31,-25,-16,-6,1,4,5,4,2,6,12,16,16,17,21,20,26,26,22,16,10,-1,-16,-29,-36,-46,-55,-54,-59},
    //    80 Degrees S Latitude
    {-53,-54,-55,-52,-48,-42,-38,-38,-29,-26,-26,-24,-23,-21,-19,-16,-12,-8,-4,-1,1,4,4,6,5,4,2,-6,-15,-24,-33,-40,-48,-50,-53,-52},
    //    90 Degrees S Latitude
    {-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30}
};

/* The following structure defines the a "cookie"
   whose address can be passed to any loc_pd commands,
   the cookie will be echoed back when the corresponding command callback
   is called from PDAPI. The information may be used to identify the
   client handle, ioctl and any other command specific information for
   the PDAPI command.
   Note: The cookie may not be echoed back in the event callback, it should
   not be relied upon from the event callback*/

typedef struct loc_pd_cmd_cb_data_type
{
   // loc MW client which issues the pd command
   loc_client_handle_type client_handle;

   //loc MW ioctl type which resulted in this pd command
   loc_ioctl_e_type ioctl_type;

   // pointer to any other command specific payload
   void * cmd_payload;

   //self referential pointer to check validity
   struct loc_pd_cmd_cb_data_type* pMe;

} loc_pd_cmd_cb_data;

/* Table that maps the PDAPI position source to QMI LOC technology mask */
static struct
{
   pdsm_pd_positioning_source_type pdapiPosSource;
   qmiLocPosTechMaskT_v02 qmiLocPostechMask;
}loc_pd_technology_mask_map[]= {

   /* GPS was used to calculate position */
   { PDSM_PD_POSITION_SOURCE_GPS, 
     QMI_LOC_POS_TECH_MASK_SATELLITE_V02 },

   /* GLONASS was used to calculate position */
   { PDSM_PD_POSITION_SOURCE_GLO,
     QMI_LOC_POS_TECH_MASK_SATELLITE_V02 },

   /* CELLID was used to calculate position */
   { PDSM_PD_POSITION_SOURCE_CELLID,
     QMI_LOC_POS_TECH_MASK_CELLID_V02 },

   /* WIFI was used to calculate position */
   { PDSM_PD_POSITION_SOURCE_WIFI,
     QMI_LOC_POS_TECH_MASK_WIFI_V02 },

   /* Sensors were used to calculate position */
   { PDSM_PD_POSITION_SOURCE_SENSORS,
     QMI_LOC_POS_TECH_MASK_SENSORS_V02 },

   /* Reference location was used to calculate position */
   { PDSM_PD_POSITION_SOURCE_DEFAULT,
     QMI_LOC_POS_TECH_MASK_REFERENCE_LOCATION_V02 },

   /* Injected position was used to calculate position */
   { PDSM_PD_POSITION_SOURCE_EPI,
     QMI_LOC_POS_TECH_MASK_INJECTED_COARSE_POSITION_V02 },

   /* HYBRID position */
   { PDSM_PD_LOCATION_SOURCE_HYBRID,
     QMI_LOC_POS_TECH_MASK_HYBRID_V02 },

   /* AFLT was used to calculate position */
   { PDSM_PD_POSITION_SOURCE_AFLT,
     QMI_LOC_POS_TECH_MASK_AFLT_V02 },

   /* BDS was used to calculate position */
   { PDSM_PD_POSITION_SOURCE_BDS,
     QMI_LOC_POS_TECH_MASK_SATELLITE_V02 },

};


/* struct that carries the client handle and QMI indication to be sent */
typedef struct
{
  loc_client_handle_type client_handle;
  qmiLocInjectWifiApDataIndMsgT_v02 ackWifiApInd;
}loc_pd_ack_wifi_ap_scan_info_inject_ind_struct_t; 


// TBD: Skip the intermediate report between intervals when the last fix is good

static void loc_pd_event_cb
(
      void                         *user_data,
      pdsm_pd_event_type           pd_event,
      const pdsm_pd_info_s_type    *pd_info_ptr
);

static void loc_pd_cmd_cb
(
      void                      *data_block_ptr,
      pdsm_pd_cmd_e_type        pd_cmd,
      pdsm_pd_cmd_err_e_type    pd_cmd_err_received
);

/* command callback called from PDAPI for a pd_command,
 the implementation will rely on the data_block_pointer to
 identify the client handle etc, and will not lock the critical
 section. */

static void loc_pd_cmd_cb_unlocked
(
      void                      *data_block_ptr,
      pdsm_pd_cmd_e_type        pd_cmd,
      pdsm_pd_cmd_err_e_type    pd_cmd_err_received
);

static void loc_pd_ext_event_cb
(
      void   *data_block_ptr,
      pdsm_ext_status_event_type  ext_status_cmd,
      const pdsm_ext_status_info_s_type *ext_status_info_ptr
);

static boolean loc_pd_notify_client (void);
static void loc_pd_notify_gnss_measurements (void);
static void loc_pd_notify_sv_polynomials(void);
static void loc_pd_reset_module (void);

static boolean loc_pd_get_report
(
      loc_parsed_position_s_type* parsed_report_ptr,
      loc_gnss_info_s_type*     gnss_report_ptr,
      loc_nmea_report_s_type   nmea_report_array[],
      int32 * nmea_sentence_counter_ptr
);


// Fill in the SV report from measurement report
static void loc_pd_get_sv_report 
(   
      loc_gnss_info_s_type*     gnss_report_ptr
);

static int loc_pd_process_ext_pos_injection
(
      loc_client_handle_type  client_handle,
      const loc_assist_data_pos_s_type* locapi_ext_pos_ptr
);


static loc_session_status_e_type loc_pd_xlate_fix_end_reason_code
(
      pdsm_pd_end_e_type pd_sess_end_status
);

static loc_session_status_e_type loc_pd_xlate_cmd_err_code_to_sess_status
(
      pdsm_pd_cmd_err_e_type pd_cmd_err
);

static uint32 loc_pd_xlate_cmd_err_code_to_loc_status
(
      pdsm_pd_cmd_err_e_type pd_cmd_err
);

static int loc_pd_xlate_external_pos_from_locapi_to_pdapi
(
      const loc_assist_data_pos_s_type*   loc_ext_pos_ptr,
      pdsm_pd_external_position_info_type* pdsm_external_pos_ptr
);

static float loc_pd_get_geoidal_separation
(
      double dbLatitude,
      double dbLongitude
);

static void loc_pd_queue_report_position_request (void);

// debug routines
static void loc_pd_dump_position_report (loc_parsed_position_s_type* parsed_report_ptr);
static void loc_pd_dump_sv_info         (loc_gnss_info_s_type*     gnss_report_ptr);

/* Get the best available position from the location engine */
static int loc_pd_process_get_best_available_position
(
  loc_client_handle_type                           client_handle,
  const qmiLocGetBestAvailablePositionReqMsgT_v02 *bap_req_ptr
);

/* Get the available WWAN position from the location engine */
static int loc_pd_process_get_available_wwan_position
(
  loc_client_handle_type                       client_handle,
  const qmiLocGetAvailWwanPositionReqMsgT_v02 *wwan_pos_req_ptr
);
/* Get the available  position from the location engine */
static int loc_pd_process_get_available_position
(
  loc_client_handle_type                       client_handle,
  const qmiLocGetAvailablePositionReqParamMsgT_v02 *pos_req_ptr
);

/* Inject GSM cellular information into the engine */
static int loc_pd_process_inject_gsm_cell_info_req
( 
  loc_client_handle_type                    client_handle,
  const qmiLocInjectGSMCellInfoReqMsgT_v02  *inject_gsm_cell_info_ptr
);

/* Inject NI Message into the engine */
static int loc_pd_process_inject_ni_msg_req
( 
  loc_client_handle_type                    client_handle,
  const qmiLocInjectNetworkInitiatedMessageReqMsgT_v02  *inject_ni_msg_ptr
);

/* Send WWAN OOS notification to the engine*/
static int loc_pd_process_wwan_oos_notification
( 
  loc_client_handle_type  client_handle
);

/**
 * called by PD task to send QMI Event Ind to LocMW 
 * task 
 *
 *  @return -- TRUE is IPC was sent successfully
 *          -- FALSE otherwise.
 */
static boolean loc_pd_event_wifi_ap_scan_info_inject_ind_proxy(void);


/**
 * called by PD task to send indication to LocMW task 
 *
 * @param client_handle
 *               LocAPI client handle
 * @param wifi_inj_ack
 *               Response/Ack indication sent from PDSM in
 *               locEng format
 *
 * @return 
 * TRUE - if IPC posted to LocMW success 
 * TRUE - if IPC posted to LocMW failed 
 */
static boolean loc_pd_ack_wifi_ap_scan_info_inject_ind_proxy(const loc_client_handle_type              client_handle,
                                                             const locEngInjectWifiApDataIndMsgT_v01   wifi_inj_ack);


// Initialize the location pd module
boolean loc_pd_init ()
{
   boolean  ret_val = FALSE;
   pdsm_client_status_e_type status = PDSM_CLIENT_OK;
   pdsm_pd_event_type pd_event_mask = PDSM_PD_EVENT_MASK;
   loc_pd_module_data_s_type *pd_module_data_ptr = &(loc_middleware_data.pd_module_data);

   do
   {
      pd_module_data_ptr->fix_in_progress     = FALSE;
      pd_module_data_ptr->mo_fix_in_progress  = FALSE;
      pd_module_data_ptr->supl_mt_fix_state = LOC_SUPL_MT_IDLE;
      pd_module_data_ptr->position_injection_in_progress = FALSE;
      pd_module_data_ptr->get_bap_in_progress            = FALSE; 
      pd_module_data_ptr->wifi_ap_scan_in_progress       = FALSE; 
      pd_module_data_ptr->get_wwan_pos_in_progress       = FALSE;
      pd_module_data_ptr->last_get_position_time = 0;
      pd_module_data_ptr->last_fix_arrival_time  = 0;
      pd_module_data_ptr->last_fix_ttf           = 0;

      pd_module_data_ptr->new_pd_info_received            = FALSE;
      pd_module_data_ptr->new_ext_status_info_received    = FALSE;
    pd_module_data_ptr->new_gnss_meas_info_received    = FALSE;
    pd_module_data_ptr->new_sv_poly_info_received = FALSE;
      pd_module_data_ptr->new_interm_pos_info_received    = FALSE;
      pd_module_data_ptr->new_nmea_info_received          = FALSE;
      pd_module_data_ptr->new_nmea_messages_counter       = 0;
      pd_module_data_ptr->new_pd_sess_end_status_received = FALSE;
      pd_module_data_ptr->new_pd_cmd_err_received         = FALSE;
      pd_module_data_ptr->session_time_out_ms             = 0;

      // Initialization (cannot be omitted)
      pd_module_data_ptr->pd_event_cb_hook                = NULL;

      memset (&(pd_module_data_ptr->pdsm_option), 0, sizeof (pdsm_pd_option_s_type));
      memset (&(pd_module_data_ptr->pdsm_qos), 0, sizeof (pdsm_pd_qos_type));
      memset (&(pd_module_data_ptr->pd_info), 0, sizeof (pdsm_pd_info_s_type));
      memset (&(pd_module_data_ptr->pd_info_copy), 0, sizeof (pdsm_pd_info_s_type));
      memset (&(pd_module_data_ptr->ext_status_info), 0, sizeof (pdsm_ext_status_info_s_type));
      memset (&(pd_module_data_ptr->ext_status_info_copy), 0, sizeof (pdsm_ext_status_info_s_type));

      memset (&(pd_module_data_ptr->cached_gnss_pos), 0, sizeof (pd_module_data_ptr->cached_gnss_pos));
      memset (&(pd_module_data_ptr->cached_nw_ref_pos), 0, sizeof (pd_module_data_ptr->cached_nw_ref_pos));
      pd_module_data_ptr->cached_gnss_pos_time = 0;
      pd_module_data_ptr->cached_nw_ref_pos_time = 0;

      pd_module_data_ptr->active_client_handle       = LOC_CLIENT_HANDLE_INVALID;
      pd_module_data_ptr->pos_inject_client_handle   = LOC_CLIENT_HANDLE_INVALID;
      pd_module_data_ptr->bap_client_handle          = LOC_CLIENT_HANDLE_INVALID;
      pd_module_data_ptr->wifi_ap_scan_client_handle = LOC_CLIENT_HANDLE_INVALID;
      pd_module_data_ptr->wwan_pos_client_handle     = LOC_CLIENT_HANDLE_INVALID;
      pd_module_data_ptr->engine_state = LOC_ENGINE_STATE_OFF;

      /* the fix timer should be non deferrable, i.e it should always wakeup the
         engine from suspended state */ 

      pd_module_data_ptr->fix_process_timer = 
         os_TimerCreateInCgpsTimerGroup( (uint32) LOC_MIDDLEWARE_TIMER_ID_FIX,
                                         (uint32) THREAD_ID_LOC_MIDDLEWARE );

      if (loc_middleware_data.pdapi_client_id == -1)
      {
        loc_middleware_data.pdapi_client_id = pdsm_client_init(PDSM_CLIENT_TYPE_PDA);
      }

      if (loc_middleware_data.pdapi_client_id == -1)
      {
         LOC_MSG_ERROR ( "pdsm_client_init failed", 0, 0, 0);
         break;
      }

      pd_event_mask |= PDSM_PD_EVENT_WPS_NEEDED;

      status = pdsm_client_pd_reg(loc_middleware_data.pdapi_client_id,
                                    NULL,
                                    loc_pd_event_cb,
                                    PDSM_CLIENT_EVENT_REG,
                                    pd_event_mask,
                                    NULL);
      if(status != PDSM_CLIENT_OK)
      {
         LOC_MSG_ERROR ("pdsm_client_pd_reg failed, error code = %d", status, 0, 0);
      }

      status = pdsm_client_ext_status_reg(loc_middleware_data.pdapi_client_id,
                                           NULL,
                                           loc_pd_ext_event_cb,
                                           PDSM_CLIENT_EVENT_REG,
                                           PDSM_EXT_STATUS_EVENT_MEASUREMENT
                                           | PDSM_EXT_STATUS_EVENT_POS_REPORT
                                           | PDSM_EXT_STATUS_EVENT_POS_INJECT_REQ
                                           | PDSM_EXT_STATUS_EVENT_BEST_AVAIL_POS_REPORT
                                           | PDSM_EXT_STATUS_EVENT_AVAIL_WWAN_POS_REPORT
                                           | PDSM_EXT_STATUS_EVENT_GNSS_MEASUREMENT
                                           | PDSM_EXT_STATUS_EVENT_SV_POLY
                                           | PDSM_EXT_STATUS_EVENT_AVAIL_POS_REPORT
                                           ,NULL);
      if (status != PDSM_CLIENT_OK)
      {
         LOC_MSG_ERROR ("pdsm_client_ext_status_reg failed, error code = %d", status, 0, 0);
      }

      status = pdsm_client_act(loc_middleware_data.pdapi_client_id);
      if(status != PDSM_CLIENT_OK)
      {
         LOC_MSG_ERROR ("pdsm_client_act failed, error code =%d", status, 0, 0);
         break;
      }

      ret_val = TRUE;
   } while (0);

   LOC_MSG_MED ("loc_pd_init returned = %d", status, 0, 0);

   return ret_val;
}

pdsm_client_status_e_type loc_pd_install_event_cb_hook(pdsm_pd_event_f_type *hook)
{
   pdsm_client_status_e_type rc = PDSM_CLIENT_OK;
   if (loc_middleware_data.pd_module_data.pd_event_cb_hook == NULL)
   {
      loc_middleware_data.pd_module_data.pd_event_cb_hook = hook;
   }
   else {
      rc = PDSM_CLIENT_ERR_CLIENT_ACTIVE;
   }
   return rc;
}

// Client registers for NMEA sentence.
boolean loc_pd_init_nmea ()
{
   pdsm_client_status_e_type status;

   status = pdsm_client_ext_status_reg(loc_middleware_data.pdapi_client_id,
                                       NULL,
                                       loc_pd_ext_event_cb,
                                       PDSM_CLIENT_EVENT_REG,
                                       PDSM_EXT_STATUS_EVENT_MASK,
                                       NULL);

   if (status != PDSM_CLIENT_OK)
   {
      LOC_MSG_ERROR ("nmea registration failed, error code = %d", status, 0, 0);
      return FALSE;
   }
   else
   {
      LOC_MSG_ERROR ("nmea registration successful", 0, 0, 0);
      return TRUE;
   }
}

// This function is used to process cmd request from location task.
int loc_pd_process_cmd_request (loc_cmd_s_type * const ptr_cmd)
{
   int ret_val = LOC_API_GENERAL_FAILURE;

   if(NULL != ptr_cmd)
   {
       const loc_cmd_type_e_type cmd_type = ptr_cmd->cmd_type;

       if (cmd_type == LOC_CMD_TYPE_PD_SCHEDULE)
       {
          loc_pd_process_fix_request (eLOC_PD_SCHEDULE);
          ret_val = LOC_API_SUCCESS;
       }
       else if (cmd_type == LOC_CMD_TYPE_REPORT_POSITION)
       {
          // Notify the requesting client of any new position data
          (void) loc_pd_notify_client ();
          ret_val = LOC_API_SUCCESS;
       }
       else
       {
           // internal error
           ret_val = LOC_API_UNSUPPORTED;
       }
   }
   else
   {
       // internal error
       ret_val = LOC_API_GENERAL_FAILURE;
   }

   if(LOC_API_SUCCESS != ret_val)
   {
       LOC_MSG_ERROR ("loc_pd_process_cmd_request internal error", 0, 0, 0);
   }

   return ret_val;
}

// Function used to do next fix request. This function is responsible for multiplexing
// between multiple apps, set up fix config and issue fix request based on the requesting client.
// The scheduling algorithm is the min (client last fix requst time + fix internal)
int loc_pd_process_fix_request (loc_pd_fix_request_e_type fix_request_type)
{
   int32                     time_to_wait_ms = INT32_MAX;
   int32                     time_delta_ms;
   boolean                   status;

   // initialize variable to avoid warning on calling time_get_uptime_ms
   time_type                 time_ms = {0};

   uint64                    current_time_ms;
   loc_client_info_s_type*   client_ptr = NULL;
   boolean                   is_any_client_requesting_fix = FALSE;
   boolean                   is_stop_client_match_active = FALSE;
   boolean                   non_mo_fix_in_progress = FALSE;
   boolean                   call_end_session = FALSE;

   //end_session_type
   pdsm_pd_end_session_opt_s_type end_session_opt ;

   loc_pd_module_data_s_type *pd_module_data_ptr = &(loc_middleware_data.pd_module_data);

   LOC_MSG_MED ("loc_pd_process_fix_request: request_type = %d, fix_in_progress = %d mo_fix_in_progress = %d\n",
                fix_request_type,
                pd_module_data_ptr->fix_in_progress,
                pd_module_data_ptr->mo_fix_in_progress);

   os_TimerStop( pd_module_data_ptr->fix_process_timer );

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

   /* Process a stop request from a MO client*/

   if (eLOC_PD_STOP_REQUEST == fix_request_type)
   {
     /* get wait time for next fix to be queued */
     time_to_wait_ms = loc_client_get_min_wait_time(&client_ptr);

     /* check if any other active state client in queue */
     is_any_client_requesting_fix = loc_client_is_any_requesting_fix();

     /* check if client calling stop is same as active client that scheduled ongoing fix */
     if (loc_middleware_data.client_module_data.stop_client_handle == loc_client_get_active_client())
     {
       is_stop_client_match_active = TRUE;
     }

     /* check if any non-mo fix is in progress */
     non_mo_fix_in_progress = (TRUE == pd_module_data_ptr->fix_in_progress &&
                               FALSE == pd_module_data_ptr->mo_fix_in_progress);

     LOC_MSG_MED("loc_pd_process_fix_request(STOP): next fix wait time %d "
                 "is_any_client_requesting_fix %d is_stop_client_match_active %d \n",
                 time_to_wait_ms, is_any_client_requesting_fix, is_stop_client_match_active);

     /* Try to End the session
        if - there is no non-mo-fix-in-progress (!fix_in_progress || mo_fix_in_progress)
              if - there is no other ready state client
              if - there is no fix scheduled within 10sec threshold time
                   &&
                   client handle calling stop is same as active client
     */
     if (FALSE == non_mo_fix_in_progress)
     {
       if (FALSE == is_any_client_requesting_fix)
       {
         LOC_MSG_MED("loc_pd_process_fix_request: Processing stop when"
                     " is_any_client_requesting_fix: %d", is_any_client_requesting_fix, 0, 0);

         call_end_session = TRUE;
       }
       else if ((time_to_wait_ms > SESSION_END_THRESHOLD) && (TRUE == is_stop_client_match_active))
       {
         LOC_MSG_MED("loc_pd_process_fix_request: Processing stop when wait time > THRESHOLD and"
                     " is_stop_client_match_active = %d \n", is_stop_client_match_active, 0, 0);

         call_end_session = TRUE;
       }
       else
       {
         call_end_session = FALSE;
         LOC_MSG_MED("Not stopping the fix, is_any_client_requesting_fix %d,"
                     " wait time > THRESHOLD = %d, "
                     "is_stop_client_match_active = %d\n",
                     is_any_client_requesting_fix,
                     time_to_wait_ms > SESSION_END_THRESHOLD,
                     is_stop_client_match_active);
       }
       
       if (TRUE == call_end_session)
       {
         LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);

         end_session_opt.receiver_off = TRUE;

         pdsm_end_session_ex(loc_pd_cmd_cb,
                             PDSM_PD_END_SESSION_FIX_REQ,
                             &end_session_opt,
                             NULL,
                             loc_middleware_data.pdapi_client_id);

         LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);

         pd_module_data_ptr->fix_in_progress = FALSE;
       }
     }
     else
     {
       /* non-mo-session is in progress (NI session, GF, etc) */
       LOC_MSG_MED("loc_process_fix_request: Not stopping the engine as "
                   "an NI session (or non-mo) is active", 0, 0, 0);
     }
   }

   /* check if fix timed out without getting a position fix */
   else if ( eLOC_PD_FIX_TIMEOUT == fix_request_type &&
             TRUE == pd_module_data_ptr->mo_fix_in_progress)
   {
      // this state is hit because the MO positioning session timed out
      time_get_uptime_ms (time_ms);
      current_time_ms = QW_CVT_Q2N(time_ms);
   
      time_delta_ms = (int) (current_time_ms - pd_module_data_ptr->last_get_position_time);

      time_to_wait_ms =  pd_module_data_ptr->session_time_out_ms - time_delta_ms;
   
      if (time_to_wait_ms < 0)
      {
         LOC_MSG_ERROR ("Missing pd cmd err or pd done event\n", 0, 0, 0);

         // Inject timeout error to the app
         pd_module_data_ptr->pd_sess_end_status = PDSM_PD_END_TIMEOUT;
         pd_module_data_ptr->new_pd_sess_end_status_received = TRUE;
   
         // send error report, as an empty location report, to client
         loc_pd_queue_report_position_request ();
   
         // Force state reset
         loc_pd_reset_module ();
         LOC_MSG_MED("loc_process_fix_request: Timeout scheduled a position"
                     "report with timeout status, also re-scheduled "
                     "fix processing", 0,0,0);
         time_to_wait_ms = INT32_MAX; // Wait until client state changed
      }
   }
        
   else if (FALSE == pd_module_data_ptr->fix_in_progress)
   {
     // In case of back to back periodic fixes.
     // When the second request for periodic fix comes in , make sure we dont have an ongoing fix in progress.
     // If we have an ongoing fix in progress wait till the fix is done , in which case we get a session_done and
     // fix_in_progress is reset to FALSE. And then the scheduler decides which fix request to process next, in
     // which case the start fix request would be honoured.
     if ( (eLOC_PD_START_REQUEST == fix_request_type) ||
          ((eLOC_PD_SCHEDULE == fix_request_type) &&
           (TRUE == loc_client_is_any_requesting_fix())) ||
           (eLOC_PD_FIX_TIMEOUT == fix_request_type)
         )
     {

       /* Make a new position request if:
          a) A new start request was received
          b) A schedule request was received and there is atleast
             one MO client active
          c) The fix timer expired and no fix is in progress */

       // Let the GPS client manager picks the gps configuration to be used.
       // It will also figure out when pdsm_get_postion will be called.
       time_to_wait_ms = loc_client_setup_request_config ();
       if (time_to_wait_ms <= 0)
       {
         LOC_MSG_HIGH ("calling pdsm_get_position server option %d, mode =%d\n",
                        pd_module_data_ptr->pdsm_option.lsinfo.server_option,
                        pd_module_data_ptr->pdsm_option.operation_mode , 0 );
         pd_module_data_ptr->fix_in_progress = TRUE;
         pd_module_data_ptr->mo_fix_in_progress = TRUE;

         LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
         status = pdsm_get_position (loc_pd_cmd_cb,
                                     NULL, /* not used */
                                     &(pd_module_data_ptr->pdsm_option),
                                     &(pd_module_data_ptr->pdsm_qos),
                                     loc_middleware_data.pdapi_client_id);
         LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
         if (status == TRUE)
         {
           time_to_wait_ms = PD_DEFAULT_WAIT_TIMEOUT; // wait until position report has received
                                                   // or client state changes
           if (time_to_wait_ms < (pd_module_data_ptr->pdsm_qos.gps_session_timeout*1000 + 1000))
           {
             time_to_wait_ms = pd_module_data_ptr->pdsm_qos.gps_session_timeout*1000 + 1000;
           }
           // The loc mw will wait for the engine to time out, add a padding of 1 second
           pd_module_data_ptr->session_time_out_ms = time_to_wait_ms;
         }
         else
         {
           LOC_MSG_ERROR ("pdsm_get_postion returned %d \n, queuing an empty"
                          "position report and rescheduled fix processing ",
                          status, 0, 0);
           // send error report, as an empty location report, to client
           loc_pd_queue_report_position_request ();
           loc_pd_reset_module ();
           time_to_wait_ms = INT32_MAX;
         }
       }// end of if (time_to_wait <= 0)
     }// end of !((eLOC_PD_START_REQUEST == fix_request_type)&&(TRUE == pd_module_data_ptr->fix_in_progress))
     else
     {
       LOC_MSG_MED ("Not making a new position request though no fix in progress",
                     0, 0, 0);
     }
   }// end of (FALSE == pd_module_data_ptr->fix_in_progress)

   /* This state may be hit if NI fix is in progress and timer expires
   */

   else
   {
     LOC_MSG_HIGH ("loc_pd_process_fix_request : unknown state "
                   "fix_request_type = %d, fix in progress = %d,"
                   "mo fix in progress = %d", fix_request_type,
                   pd_module_data_ptr->fix_in_progress,
                   pd_module_data_ptr->mo_fix_in_progress);

     time_to_wait_ms = INT32_MAX;
   }

   LOC_MSG_HIGH ("loc_module::time_to_wait = %d \n", time_to_wait_ms, 0, 0);

   if ((time_to_wait_ms > 0) && (time_to_wait_ms != INT32_MAX))
   {
      os_TimerStart( pd_module_data_ptr->fix_process_timer, time_to_wait_ms, 0 );
   }

   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

   return 1;
}

// Notify client regarding position and sv information.
static boolean loc_pd_notify_client (void)
{
   loc_client_handle_type      active_client_handle;
   loc_parsed_position_s_type* parsed_report_ptr;
   loc_gnss_info_s_type*       gnss_report_ptr;
   loc_nmea_report_s_type     * nmea_report_array = NULL;
   int32 nmea_sentence_counter,i = 0;
   loc_pd_module_data_s_type*  pd_module_data_ptr;

   pd_module_data_ptr = &(loc_middleware_data.pd_module_data);
   parsed_report_ptr = &(pd_module_data_ptr->position_payload.parsed_location_report);
   gnss_report_ptr = &(pd_module_data_ptr->gnss_payload.gnss_report);

   nmea_sentence_counter = 0; //reinitialize the value

   // note this is a local, temporary memory buffer, which is allocated and freed later in the same function
   nmea_report_array = 
          (loc_nmea_report_s_type *)os_MemAlloc(
                                      sizeof (loc_nmea_report_s_type) * NMEA_BUFFER_MAX_SIZE,
                                      OS_MEM_SCOPE_TASK );
   if(NULL != nmea_report_array)
   {
       memset (nmea_report_array,   0, (sizeof (loc_nmea_report_s_type) * NMEA_BUFFER_MAX_SIZE));
   }
   else
   {
       LOC_MSG_ERROR ("loc_pd_notify_client, cannot allocate memory for NMEA feature", 0, 0, 0);
   }

   // the payloads will be memset in loc_pd_get_report
   loc_pd_get_report (parsed_report_ptr, gnss_report_ptr, nmea_report_array, &nmea_sentence_counter);

   active_client_handle = loc_client_get_active_client ();

   if(NULL != nmea_report_array )
   {
       if ((active_client_handle != LOC_CLIENT_HANDLE_INVALID) && ( nmea_sentence_counter  > 0) )
       {
           for( i=0; i < nmea_sentence_counter ;i++)
           {
             //copy the contents of each NMEA sentence into client structure format and invoke call back
             memscpy(&(pd_module_data_ptr->nmea_payload.nmea_report),
                     sizeof (pd_module_data_ptr->nmea_payload.nmea_report),
                     &nmea_report_array[i], sizeof(nmea_report_array[i]));

             // The message will be sent to the current active client if it has registered for LOC_EVENT_NMEA_1HZ_REPORT
             (void) loc_client_invoke_callback (active_client_handle,
                                                LOC_EVENT_NMEA_1HZ_REPORT,
                                                &(pd_module_data_ptr->nmea_payload));

             // The message will be sent to the current active client if it has registered for LOC_EVENT_NMEA_POSITION_REPORT
             // note that LOC_EVENT_NMEA_POSITION_REPORT was designed to be just NMEA for the final position report
             // but the first release of NMEA feature (LocMW 1.0u1) miss-treated it as the 1Hz report
             // we consider this as a bug and might change the behavior in future releases
             (void) loc_client_invoke_callback (active_client_handle,
                                                LOC_EVENT_NMEA_POSITION_REPORT,
                                                &(pd_module_data_ptr->nmea_payload));
           }
       }
    
       // after this point, nmea_report_array becomes invalid (nulled out)
       os_MemFree((void **) &nmea_report_array);
       nmea_report_array = NULL;
   }

   // Set the gnss report first (the one being populated from final position report), thent the final position report, so the client can
   if ((active_client_handle != LOC_CLIENT_HANDLE_INVALID) && (gnss_report_ptr->valid_mask != 0))
   {
      (void) loc_client_invoke_callback (active_client_handle, LOC_EVENT_SATELLITE_REPORT, &(pd_module_data_ptr->gnss_payload));
   }

   if ((active_client_handle != LOC_CLIENT_HANDLE_INVALID) && (parsed_report_ptr->valid_mask != 0))
   {
       // Before notifying the client about the final fix, update the state for single-shot client
       if ((parsed_report_ptr->valid_mask & LOC_POS_VALID_SESSION_STATUS) &&
           (parsed_report_ptr->session_status != LOC_SESS_STATUS_IN_PROGESS))
       {
          (void) loc_client_update_state_after_fix ();
       }

       (void) loc_client_invoke_callback (active_client_handle,
                                         LOC_EVENT_PARSED_POSITION_REPORT,
                                         &(pd_module_data_ptr->position_payload));
   }

   loc_pd_process_concurrent_cellid_request ();

   LOC_MSG_HIGH ("loc_pd_notify_client, position valid flags = 0x%x, sv valid flags = 0x%x",
                 parsed_report_ptr->valid_mask, gnss_report_ptr->valid_mask, 0);

   return TRUE;
}

// Report cell id reference position to the pending cell id request if:
// (1) from gnss position (final or intermediate) if less than 2 seconds old
// (2) from cell id reference position from MSB request if less than 30 seconds old
void loc_pd_process_concurrent_cellid_request ()
{
   time_type                 time_ms = {0};
   uint64                    current_time_ms;
   loc_event_payload_u_type* pos_payload_ptr = NULL;
   loc_pd_module_data_s_type *pd_module_data_ptr = &(loc_middleware_data.pd_module_data);
   
   time_get_uptime_ms (time_ms);
   current_time_ms = QW_CVT_Q2N(time_ms);

   // Report cached network reference position to cell id client 
   if ((pd_module_data_ptr->cached_gnss_pos_time + 2000) > current_time_ms)
   {
      pos_payload_ptr = &(pd_module_data_ptr->cached_gnss_pos);
   }
   else if ((pd_module_data_ptr->cached_nw_ref_pos_time + 30000) > current_time_ms)
   {
      pos_payload_ptr = &(pd_module_data_ptr->cached_nw_ref_pos);
   }

   if (pos_payload_ptr != NULL)
   {
      // Mark it as final report for cell id
      pos_payload_ptr->parsed_location_report.valid_mask |= LOC_POS_VALID_SESSION_STATUS;
      pos_payload_ptr->parsed_location_report.session_status =  LOC_SESS_STATUS_SUCCESS;
      loc_client_report_position_to_cellid_request (pos_payload_ptr, current_time_ms);
   }
}

// returns the leap seconds (for filling in other data structures)
static int8 loc_pd_fill_timestamp_utc(loc_parsed_position_s_type* parsed_report_ptr,
                                      const pdsm_pd_data_type *pd_data_ptr)
{
   int8 leap_seconds = 0;

   // Retrieve leap seconds
   if ((pd_data_ptr->opt_field_mask & PDSM_PD_UTCOFFSET_VALID) != 0)
   {
      parsed_report_ptr->valid_mask |= LOC_POS_VALID_LEAP_SECONDS;
      parsed_report_ptr->leap_seconds = leap_seconds = pd_data_ptr->gpsUtcOffset;
   }
   else
   {
      leap_seconds = GPS_UTC_OFFSET;
   }

   if ((pd_data_ptr->ext_opt_field_mask & PDSM_PD_GPS_TIME_VALID) != 0)
   {
      parsed_report_ptr->valid_mask |= LOC_POS_VALID_TIMESTAMP_UTC;
      parsed_report_ptr->timestamp_utc = pd_data_ptr->time_stamp + GPS_TO_UTC_DELTA_SECONDS - leap_seconds;
      parsed_report_ptr->timestamp_utc *= 1000;
      if ((pd_data_ptr->opt_field_mask & PDSM_PD_MSEC_VALID) != 0)
      {
         parsed_report_ptr->timestamp_utc += pd_data_ptr->time_stamp_msec;
      }
   }
   
   else if ( (pd_data_ptr->ext_opt_field_mask & PDSM_PD_UTC_TIME_VALID) != 0) 
   {
     /* We cannot calculate the calendar GPS time from UTC time as we dont
        have information for the leap seconds */
   
     (void) gnss_GetUnixEpochFromUTC( &(pd_data_ptr->ext_pos_data.utc_time),
                                         &(parsed_report_ptr->timestamp_utc) ) ;
     /* Scale it to milliseconds, as the TimeStamp now is in seconds. */
     parsed_report_ptr->timestamp_utc *= 1000;
   
     /* Now lets add more precision.
         utc_time.sec --> is seconds scaled by 100.
                          ( ie. 1sec is reported as 100 &
                            1.99 sec is reported as 199 which is 1990msec)
         GetUnixEpochFromUTC gives us back in seconds. So we can add the tenths
         of a second and hundredth of a second.
   
         Note: Currently the precision of hundredth of a second is lost in
               translation in SM.
     */
     parsed_report_ptr->timestamp_utc +=
                     (( pd_data_ptr->ext_pos_data.utc_time.sec % 100 ) * 10 ) ;
     /* The UTC Time Stamp is valid */
     parsed_report_ptr->valid_mask |= LOC_POS_VALID_TIMESTAMP_UTC;
   }
   else
   {
       // time is not valid, do nothing
      LOC_MSG_MED ("loc_pd_fill_timestamp_utc: time is invalid", 0, 0, 0);
   }

   // TBD: time uncertainty, not available yet
   return leap_seconds;
}

/*
static void dump_sv_details(const loc_sv_info_s_type * const ptr_sv_data)
{
    if(NULL != ptr_sv_data)
    {
        LOC_MSG_HIGH ("dump_sv_details: --------------------------------", 0, 0, 0);
        LOC_MSG_HIGH ("dump_sv_details: valid_mask: 0x%X, system: %d, prn: %d", (int)ptr_sv_data->valid_mask, (int)ptr_sv_data->system, (int)ptr_sv_data->prn);
        LOC_MSG_HIGH ("dump_sv_details: health: %d, process: %d, has eph: %d", (int)ptr_sv_data->health_status, (int)ptr_sv_data->process_status, (int)ptr_sv_data->has_eph);
        LOC_MSG_HIGH ("dump_sv_details: has alm: %d, elevation: %d, azimuth: %d", (int)ptr_sv_data->has_alm, (int)ptr_sv_data->elevation, (int)ptr_sv_data->azimuth);
        LOC_MSG_HIGH ("dump_sv_details: snr: %d", (int)ptr_sv_data->snr, 0, 0);
    }
    else
    {
        // internal error...
    }
}
*/

// Fill in the SV report from measurement report
static void loc_pd_get_sv_report (loc_gnss_info_s_type*     gnss_report_ptr)
{
   loc_pd_module_data_s_type   *pd_module_data_ptr = &(loc_middleware_data.pd_module_data);
   pdsm_pd_ext_meas_s_type     *ext_meas_report_ptr = NULL;
   int                          i_out, i_in;

   if( NULL == gnss_report_ptr )
   {
      LOC_MSG_ERROR("loc_pd_get_sv_report, NULL gnss_report_ptr",0,0,0);
      return;
   }

   ext_meas_report_ptr = &(pd_module_data_ptr->ext_status_info_copy.ext_status_info.ext_meas_report_type);

   memset (gnss_report_ptr, 0, sizeof (loc_gnss_info_s_type));
   gnss_report_ptr->valid_mask = 0;
   // Fill in the SV that belongs to GPS system
   gnss_report_ptr->valid_mask |= LOC_GNSS_INFO_VALID_SV_COUNT | LOC_GNSS_INFO_VALID_SV_LIST ;
   gnss_report_ptr->sv_count = 0;

   LOC_MSG_MED ("loc_pd_get_sv_report: health_svmask 0x%X", ext_meas_report_ptr->health_svmask,0,0 );

   // Fill in the SV that belongs to GPS system
   for (i_out = i_in = 0; (i_in < ext_meas_report_ptr->num_svs) && (i_out < LOC_API_MAX_SV_COUNT) && (i_in < PDSM_EXT_MEAS_NUM_SVS); i_in++)
   {
      const pdsm_pd_meas_s_type * const ptr_SV_in = &(ext_meas_report_ptr->ext_meas_report_type[i_in]);
      const gnss_sv_id_type sv_id = ptr_SV_in->sv_id;
      loc_sv_info_s_type * const ptr_SV_out = &(gnss_report_ptr->sv_list[i_out]);

      ptr_SV_out->valid_mask |= (LOC_SV_INFO_VALID_SYSTEM | LOC_SV_INFO_VALID_PRN );
      LOC_MSG_LOW ("loc_pd_get_sv_report: sv_id = %d", sv_id,0,0);

      if (sv_id >= PDSM_PD_SV_ID_GPS_MIN && sv_id <= PDSM_PD_SV_ID_GPS_MAX) // 1~32
      {
         ptr_SV_out->system = LOC_SV_SYSTEM_GPS;
         ptr_SV_out->prn = sv_id;

         ptr_SV_out->valid_mask |= (LOC_SV_INFO_VALID_HAS_EPH | LOC_SV_INFO_VALID_HAS_ALM);
         if ((ext_meas_report_ptr->eph_svmask & (1 << (sv_id - PDSM_PD_SV_ID_GPS_MIN)) ) != 0)
         {
            ptr_SV_out->has_eph = TRUE;
         }
         else
         {
            ptr_SV_out->has_eph = FALSE;
         }

         if ((ext_meas_report_ptr->alm_svmask & (1 << (sv_id - PDSM_PD_SV_ID_GPS_MIN)) ) != 0)
         {
            ptr_SV_out->has_alm = TRUE;
         }
         else
         {
            ptr_SV_out->has_alm = FALSE;
         }

         /* a not 0 health_svmask means svmask bit is set */
         if( ext_meas_report_ptr->health_svmask )
         {
           ptr_SV_out->valid_mask |= LOC_SV_INFO_VALID_HEALTH_STATUS;
           ptr_SV_out->health_status = (uint8)((ext_meas_report_ptr->health_svmask >> (sv_id - PDSM_PD_SV_ID_GPS_MIN)) & 1);
         }

      }
      else if (sv_id >= PDSM_PD_SV_ID_SBAS_MIN && sv_id <= PDSM_PD_SV_ID_SBAS_MAX) // 33~64
      {
         // SBAS: GPS RPN: 120-151,
         // In exteneded measurement report, we follow nmea standard, which is from 33-64.
         ptr_SV_out->prn = sv_id + (120-PDSM_PD_SV_ID_SBAS_MIN); // convert from 33~64 to 120~151
         ptr_SV_out->system = LOC_SV_SYSTEM_SBAS;
         ptr_SV_out->valid_mask |= (LOC_SV_INFO_VALID_HAS_EPH | LOC_SV_INFO_VALID_HAS_ALM);
         ptr_SV_out->has_eph = TRUE;
         ptr_SV_out->has_alm = TRUE;

#if (PDSM_API_MAJ_V > 7) || ((PDSM_API_MAJ_V == 7) && (PDSM_API_MIN_V >= 4))
         if ((ext_meas_report_ptr->sbas_eph_mask & (1 << (sv_id - PDSM_PD_SV_ID_SBAS_MIN)) ) != 0)
         {
            ptr_SV_out->has_eph = TRUE;
         }
         else
         {
            ptr_SV_out->has_eph = FALSE;
         }

         if ((ext_meas_report_ptr->sbas_alm_mask & (1 << (sv_id - PDSM_PD_SV_ID_SBAS_MIN)) ) != 0)
         {
            ptr_SV_out->has_alm = TRUE;
         }
         else
         {
            ptr_SV_out->has_alm = FALSE;
         }
#endif // PDAPI >= 7.4         
      }
      else if (sv_id >= PDSM_PD_SV_ID_GLO_MIN && sv_id <= PDSM_PD_SV_ID_GLO_MAX) // 65~96
      {
         // Gloness: Slot id: 1-32
         // In extended measurement report, we follow nmea standard, which is 65-96
         ptr_SV_out->prn = sv_id + (1-PDSM_PD_SV_ID_GLO_MIN);
         ptr_SV_out->system = LOC_SV_SYSTEM_GLONASS;

#if (PDSM_API_MAJ_V > 7) || ((PDSM_API_MAJ_V == 7) && (PDSM_API_MIN_V >= 4))
         ptr_SV_out->valid_mask |= (LOC_SV_INFO_VALID_HAS_EPH | LOC_SV_INFO_VALID_HAS_ALM);

         if ((ext_meas_report_ptr->glo_eph_svmask & (1 << (sv_id - PDSM_PD_SV_ID_GLO_MIN)) ) != 0)
         {
            ptr_SV_out->has_eph = TRUE;
         }
         else
         {
            ptr_SV_out->has_eph = FALSE;
         }

         if ((ext_meas_report_ptr->glo_alm_svmask & (1 << (sv_id - PDSM_PD_SV_ID_GLO_MIN)) ) != 0)
         {
            ptr_SV_out->has_alm = TRUE;
         }
         else
         {
            ptr_SV_out->has_alm = FALSE;
         }
#endif // PDAPI >= 7.4
      }
      else if (sv_id >= PDSM_PD_SV_ID_BDS_MIN && sv_id <= PDSM_PD_SV_ID_BDS_MAX) // 201~237
      {
         ptr_SV_out->prn = sv_id;
         ptr_SV_out->system = LOC_SV_SYSTEM_BDS;

#if (PDSM_API_MAJ_V > 7) || ((PDSM_API_MAJ_V == 7) && (PDSM_API_MIN_V >= 30))
         ptr_SV_out->valid_mask |= (LOC_SV_INFO_VALID_HAS_EPH | LOC_SV_INFO_VALID_HAS_ALM);

         /* 1ULL becuase BDS svmask is uint64 */
         if ((ext_meas_report_ptr->bds_eph_svmask & (1ULL << (sv_id - PDSM_PD_SV_ID_BDS_MIN)) ) != 0ULL)
         {
            ptr_SV_out->has_eph = TRUE;
         }
         else
         {
            ptr_SV_out->has_eph = FALSE;
         }

         if ((ext_meas_report_ptr->bds_alm_svmask & (1ULL << (sv_id - PDSM_PD_SV_ID_BDS_MIN)) ) != 0ULL)
         {
            ptr_SV_out->has_alm = TRUE;
         }
         else
         {
            ptr_SV_out->has_alm = FALSE;
         }
#endif // PDAPI >= 7.30
      }
      else if (sv_id >= PDSM_PD_SV_ID_GAL_MIN && sv_id <= PDSM_PD_SV_ID_GAL_MAX) // 301~336
      {
         ptr_SV_out->prn = sv_id;
         ptr_SV_out->system = LOC_SV_SYSTEM_GALILEO;

#if (PDSM_API_MAJ_V >= 8) 
         ptr_SV_out->valid_mask |= (LOC_SV_INFO_VALID_HAS_EPH | LOC_SV_INFO_VALID_HAS_ALM);

         /* 1ULL becuase GAL svmask is uint64 */
         if ((ext_meas_report_ptr->gal_eph_svmask & (1ULL << (sv_id - PDSM_PD_SV_ID_GAL_MIN)) ) != 0ULL)
         {
            ptr_SV_out->has_eph = TRUE;
         }
         else
         {
            ptr_SV_out->has_eph = FALSE;
         }

         if ((ext_meas_report_ptr->gal_alm_svmask & (1ULL << (sv_id - PDSM_PD_SV_ID_GAL_MIN)) ) != 0ULL)
         {
            ptr_SV_out->has_alm = TRUE;
         }
         else
         {
            ptr_SV_out->has_alm = FALSE;
         }
#endif // PDAPI >= 8.x
      }
      else if (sv_id >= PDSM_PD_SV_ID_QZSS_MIN && sv_id <= PDSM_PD_SV_ID_QZSS_MAX) // 193-197
      {
         ptr_SV_out->prn = sv_id;
         ptr_SV_out->system = LOC_SV_SYSTEM_QZSS;

#if (PDSM_API_MAJ_V > 8) || ((PDSM_API_MAJ_V == 8) && (PDSM_API_MIN_V >= 3)) 
         ptr_SV_out->valid_mask |= (LOC_SV_INFO_VALID_HAS_EPH | LOC_SV_INFO_VALID_HAS_ALM);

         if ((ext_meas_report_ptr->qzss_eph_svmask & (1 << (sv_id - PDSM_PD_SV_ID_QZSS_MIN)) ) != 0)
         {
            ptr_SV_out->has_eph = TRUE;
         }
         else
         {
            ptr_SV_out->has_eph = FALSE;
         }

         if ((ext_meas_report_ptr->qzss_alm_svmask & (1 << (sv_id - PDSM_PD_SV_ID_QZSS_MIN)) ) != 0)
         {
            ptr_SV_out->has_alm = TRUE;
         }
         else
         {
            ptr_SV_out->has_alm = FALSE;
         }
#endif // PDAPI >= 8.3
      }
      else
      {
         ptr_SV_out->valid_mask = 0;

         LOC_MSG_MED ("loc_pd_get_sv_report: skipping SV ID: %d\n", sv_id, 0, 0);

         // note that i_out won't be increases, so we will overwrite this same out record
         continue;
      }

      // azimuth and elevation in degrees
      if ((ptr_SV_in->elevation != 0) && 
          (ptr_SV_in->azimuth != 0)) 
      {
         ptr_SV_out->valid_mask |= (LOC_SV_INFO_VALID_ELEVATION | LOC_SV_INFO_VALID_AZIMUTH);
         ptr_SV_out->elevation = (float) (ptr_SV_in->elevation);
         ptr_SV_out->azimuth = (float)  (ptr_SV_in->azimuth);
      }

      // fill in SV cno, since PDAPI already filters out invalid measurement, cno will always be valid
      ptr_SV_out->valid_mask |= LOC_SV_INFO_VALID_SNR;
      ptr_SV_out->snr = ((float) ptr_SV_in->c_no) / 10;

      // fill in SV process status and c/no
      switch (ptr_SV_in->sv_state)
      {
      case PDSM_SV_STATE_TRACK:
          ptr_SV_out->valid_mask |= LOC_SV_INFO_VALID_PROCESS_STATUS;
          ptr_SV_out->process_status = LOC_SV_STATUS_TRACK;
          break;
      case PDSM_SV_STATE_BIT_EDGE:
          ptr_SV_out->valid_mask |= LOC_SV_INFO_VALID_PROCESS_STATUS;
          ptr_SV_out->process_status = LOC_SV_STATUS_SEARCH;
          break;
      case PDSM_SV_STATE_SEARCH:
      case PDSM_SV_STATE_SEARCH_VERIFY:
          ptr_SV_out->valid_mask |= LOC_SV_INFO_VALID_PROCESS_STATUS;
          ptr_SV_out->process_status = LOC_SV_STATUS_SEARCH;
          break;
      case PDSM_SV_STATE_IDLE:
          ptr_SV_out->valid_mask |= LOC_SV_INFO_VALID_PROCESS_STATUS;
          ptr_SV_out->process_status = LOC_SV_STATUS_IDLE;
          break;
      default:
          break;
      }

      ++i_out;
   }
   gnss_report_ptr->sv_count = i_out;

   // uncomment to dump detailed SV information on output end
   /*
   {
      int i = 0;
      for ( i = 0; i < gnss_report_ptr->sv_count; ++i )
      {
         dump_sv_details(&gnss_report_ptr->sv_list[i]);
      }
   }
   */
}

/**   loc_pd_convert_technology_mask_from_pdapi
 
@brief : convert PDAPI position source to QMI LOC technology 
       mask.
@param [in]   pdapiPosSource : PDAPI position source 
 
@retval QMI_LOC technology mask
---------------------------------------------------------------------*/ 

static qmiLocPosTechMaskT_v02 loc_pd_convert_technology_mask_from_pdapi(
   pdsm_pd_positioning_source_type pdapiPosSource)
{
   int i;
   qmiLocPosTechMaskT_v02 posTechMask;

   memset(&posTechMask, 0, sizeof(posTechMask));

   for ( i = 0;
         i < ( (sizeof(loc_pd_technology_mask_map))/
               (sizeof(loc_pd_technology_mask_map[0])) );
         i++ )
   {
      if(pdapiPosSource & loc_pd_technology_mask_map[i].pdapiPosSource )
      {
         posTechMask |= loc_pd_technology_mask_map[i].qmiLocPostechMask;
      }
   }

   return posTechMask;
}

// Retrieves position (including sv information) from modem.
static boolean loc_pd_get_report (loc_parsed_position_s_type* parsed_report_ptr,
                                  loc_gnss_info_s_type*     gnss_report_ptr,
                                  loc_nmea_report_s_type   nmea_report_array[],
                                  int32 * nmea_sentence_counter_ptr)
{
   loc_pd_module_data_s_type *pd_module_data_ptr = &(loc_middleware_data.pd_module_data);

   pdsm_pd_info_s_type         *pd_info_ptr         = NULL;
   pdsm_pd_data_type           *pd_data_ptr         = NULL;
   pdsm_pd_data_ext_s_type     *pd_ext_data_ptr     = NULL;
   boolean                      process_pd_info = FALSE,
                                process_ext_status_info    = FALSE,
                                process_interm_pos_info    = FALSE,
                                process_pd_sess_end_status = FALSE,
                                process_pd_cmd_err         = FALSE;
   int                          leap_seconds;
   int                          set_valid_mask_for_intermediate_pos = 1;
   time_type                    time_ms = {0};

   if((NULL == parsed_report_ptr) || (NULL == gnss_report_ptr))
   {
       LOC_MSG_ERROR("loc_pd_get_report skip all pd reports", 0, 0, 0);
       return FALSE;
   }

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
   if (TRUE == pd_module_data_ptr->new_pd_info_received)
   {
      process_pd_info      = TRUE;
      memscpy (&(pd_module_data_ptr->pd_info_copy), sizeof(pd_module_data_ptr->pd_info_copy),
            &(pd_module_data_ptr->pd_info), sizeof (pd_module_data_ptr->pd_info));
      pd_module_data_ptr->new_pd_info_received = FALSE;
   }

   if (TRUE == pd_module_data_ptr->new_pd_sess_end_status_received)
   {
      process_pd_sess_end_status = TRUE;
      pd_module_data_ptr->pd_sess_end_status_copy = pd_module_data_ptr->pd_sess_end_status;
      pd_module_data_ptr->new_pd_sess_end_status_received = FALSE;
   }

   if (TRUE == pd_module_data_ptr->new_pd_cmd_err_received)
   {
      process_pd_cmd_err                  = TRUE;
      pd_module_data_ptr->pd_cmd_err_copy = pd_module_data_ptr->pd_cmd_err;
      pd_module_data_ptr->new_pd_cmd_err_received = FALSE;
   }

   if (TRUE == pd_module_data_ptr->new_ext_status_info_received)
   {
      pd_module_data_ptr->new_ext_status_info_received = FALSE;
      process_ext_status_info      = TRUE;
      memscpy (&(pd_module_data_ptr->ext_status_info_copy), sizeof(pd_module_data_ptr->ext_status_info_copy),
            &(pd_module_data_ptr->ext_status_info), sizeof (pd_module_data_ptr->ext_status_info));
   }

   if (TRUE == pd_module_data_ptr->new_interm_pos_info_received)
   {
      pd_module_data_ptr->new_interm_pos_info_received = FALSE;
      process_interm_pos_info      = TRUE;
      memscpy(&pd_module_data_ptr->interm_pos_info_copy, sizeof(pd_module_data_ptr->interm_pos_info_copy),
            &pd_module_data_ptr->interm_pos_info, sizeof(pd_module_data_ptr->interm_pos_info));  
   }

   if( (NULL != nmea_sentence_counter_ptr) && (NULL != nmea_report_array) )
   {
       if (pd_module_data_ptr->new_nmea_info_received == TRUE)
       {
         int i=0;
    
          * nmea_sentence_counter_ptr = pd_module_data_ptr->new_nmea_messages_counter; //Since we increment the counter by 1
                                                                                        //after every PDSM Nmea event
          for (i=0; i <  * nmea_sentence_counter_ptr; i++)
          {
            
            pdsm_pd_ext_nmea_s_type* pd_nmea = 
              &pd_module_data_ptr->nmea_info_buffer_in[i].ext_status_info.ext_nmea_pos_report_type;
    
             nmea_report_array[i].length = pd_nmea->nmea_length;
             GNSS_STRLCPY(nmea_report_array[i].nmea_sentences,
                   (char*) pd_nmea->nmea_data,
                   sizeof nmea_report_array[i].nmea_sentences);
    
             if (pd_nmea->nmea_length >= sizeof nmea_report_array[i].nmea_sentences)
             {
                LOC_MSG_HIGH("loc_pd_get_report, NMEA string too long, len=%d\n", pd_nmea->nmea_length, 0, 0);
             }
           }  
    
          pd_module_data_ptr->new_nmea_messages_counter = 0; //input buffer has been emptied
          pd_module_data_ptr->new_nmea_info_received          = FALSE; //reset the flag
       }
   }
   else
   {
       LOC_MSG_HIGH("loc_pd_get_report skip nmea report", 0, 0, 0);
   }

   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

   // Initialzie the parsed report structure
   memset (parsed_report_ptr, 0, sizeof (loc_parsed_position_s_type));
   memset (gnss_report_ptr,   0, sizeof (loc_gnss_info_s_type));
   parsed_report_ptr->valid_mask = 0;
   gnss_report_ptr->valid_mask = 0;

   // Filling intermediate position here (may be overridden by final positions below)
   if (!process_pd_info && process_interm_pos_info)
   {
      // interm_pos_info_copy
      pdsm_pd_ext_pos_s_type *ext_pos_report = &pd_module_data_ptr->interm_pos_info_copy.ext_status_info.ext_pos_report_type;
      pdsm_pd_data_type *pd_data_ptr = &ext_pos_report->pd_data;

      // Latitude and longitude
      if (pd_data_ptr->ext_opt_field_mask & PDSM_PD_LAT_LONG_VALID)
      {
         // Do not set valid mask for intermediate positions
         if (set_valid_mask_for_intermediate_pos)
         {
            parsed_report_ptr->valid_mask |= LOC_POS_VALID_LATITUDE;
            parsed_report_ptr->valid_mask |= LOC_POS_VALID_LONGITUDE;
         }

         /* Latitude: units in (2^-25 / Pi) rad, representing -90 to 90 degrees */
         /* Longitude: units in (2^-25 / Pi) rad, representing -180 to 180 degrees */
         parsed_report_ptr->latitude  = ((double) ((signed) pd_data_ptr->lat)) * 180.0 / (1 << 25);
         parsed_report_ptr->longitude = ((double) ((signed) pd_data_ptr->lon)) * 180.0 / (1 << 25);
       
         LOC_MSG_HIGH("loc_pd_get_report course position lat: 0x%X or %d (/100)", (int) pd_data_ptr->lat,
               (int) parsed_report_ptr->latitude * 100, 0);
         LOC_MSG_HIGH("                  course position lon: 0x%X or %d (/100)", (int) pd_data_ptr->lon,
               (int) parsed_report_ptr->longitude * 100, 0);
      }

      // Altitude
      if (pd_data_ptr->opt_field_mask & PDSM_PD_ALTITUDE_VALID)
      {
         // Populate the altitude field
         
         if (set_valid_mask_for_intermediate_pos)
         {
             parsed_report_ptr->valid_mask |= LOC_POS_VALID_ALTITUDE_WRT_ELLIPSOID;
         }
         parsed_report_ptr->altitude_wrt_ellipsoid = 
            (float)(pd_data_ptr->altitude - LOC_PD_ALTITUDE_WRT_WGS84_ELLIPSOID_OFFSET);

         if (pd_data_ptr->ext_opt_field_mask & PDSM_PD_LAT_LONG_VALID)
         {
            if (set_valid_mask_for_intermediate_pos)
            {
                parsed_report_ptr->valid_mask |= LOC_POS_VALID_ALTITUDE_WRT_MEAN_SEA_LEVEL;
            }
            parsed_report_ptr->altitude_wrt_mean_sea_level = parsed_report_ptr->altitude_wrt_ellipsoid -
            loc_pd_get_geoidal_separation (parsed_report_ptr->latitude, parsed_report_ptr->longitude);
         }
      }

      
      LOC_MSG_MED (" Validity Mask in position report from PDAPI = %d, %d ", 
                    pd_data_ptr->opt_field_mask, 
                    pd_data_ptr->ext_opt_field_mask, 
                    0);
      // Speed
      if (pd_data_ptr->opt_field_mask & PDSM_PD_VELOCITY_VALID)
      {
         // Do not set valid mask for intermediate positions
         if (set_valid_mask_for_intermediate_pos)
         {
         
            LOC_MSG_LOW (" Got SPEED UNC = %d and adding it", (int)pd_data_ptr->opt_field_mask, 0, 0);
            parsed_report_ptr->valid_mask |= LOC_POS_VALID_SPEED_HORIZONTAL;
            parsed_report_ptr->valid_mask |= LOC_POS_VALID_SPEED_VERTICAL;
            parsed_report_ptr->valid_mask |= LOC_POS_VALID_SPEED_UNC;
         }

         parsed_report_ptr->speed_horizontal  = ((float) pd_data_ptr->ext_pos_data.velHor)/100;  /* original in cm/s */
         parsed_report_ptr->speed_vertical  = ((float) pd_data_ptr->ext_pos_data.velVert)/100;   /* original in cm/s */

         parsed_report_ptr->speed_unc = sqrt(
               ((float) pd_data_ptr->ext_pos_data.velUncHoriz) * pd_data_ptr->ext_pos_data.velUncHoriz +
               ((float) pd_data_ptr->ext_pos_data.velUncVert) * pd_data_ptr->ext_pos_data.velUncVert ) / 100; /* original in cm/s */

      }
      
         // Heading
      if ( (set_valid_mask_for_intermediate_pos) &&
           (pd_data_ptr->ext_opt_field_mask & PDSM_PD_HEADING_VALID) )
         {
      
            LOC_MSG_LOW (" Got heading UNC = %d and try to add it", (int)pd_data_ptr->ext_opt_field_mask, 0, 0);
            // Heading is only valid if speed is valid and it is greater than 0.1m/sec
            if (parsed_report_ptr->speed_horizontal >= 0.1)
            {
                LOC_MSG_LOW (" Got heading UNC = %d and adding it", (int)pd_data_ptr->ext_opt_field_mask, 0, 0);
            
                parsed_report_ptr->valid_mask |= LOC_POS_VALID_HEADING;
                parsed_report_ptr->heading = ((float) pd_data_ptr->heading * 360 ) / (1<<10);

             parsed_report_ptr->valid_mask |= LOC_POS_VALID_HEADING_UNC;
             parsed_report_ptr->heading_unc = pd_data_ptr->ext_pos_data.heading_unc_rad;
            }

         
      }

      // PD uncertainty not available, generalize from HEPE
      if (!(pd_data_ptr->opt_field_mask & PDSM_PD_UNCERTAINTY_VALID))
      {
         if (set_valid_mask_for_intermediate_pos)
         {
            parsed_report_ptr->valid_mask |=
               LOC_POS_VALID_HOR_UNC_ELLI_SEMI_MAJ |
               LOC_POS_VALID_HOR_UNC_ELLI_SEMI_MIN;
         }
         parsed_report_ptr->hor_unc_ellipse_semi_major =
            parsed_report_ptr->hor_unc_ellipse_semi_minor =
               INV_SQRT_2 * pd_data_ptr->ext_pos_data.loc_unc_horizontal;
      }

      // PD uncertainty is available
      if (pd_data_ptr->opt_field_mask & PDSM_PD_UNCERTAINTY_VALID)
      {
         if (set_valid_mask_for_intermediate_pos)
         {
            parsed_report_ptr->valid_mask |= LOC_POS_VALID_HOR_UNC_CIRCULAR |
            LOC_POS_VALID_HOR_UNC_ELLI_SEMI_MAJ |
            LOC_POS_VALID_HOR_UNC_ELLI_SEMI_MIN |
            LOC_POS_VALID_HOR_UNC_ELLI_ORIENT_AZIMUTH |
            LOC_POS_VALID_VERTICAL_UNC |
            LOC_POS_VALID_CONFIDENCE_HORIZONTAL;
         }

         // Ellipse angle
         if(pd_data_ptr->loc_uncertainty_a > pd_data_ptr->loc_uncertainty_p)
         {
            parsed_report_ptr->hor_unc_ellipse_orient_azimuth = pd_data_ptr->loc_uncertainty_ang * 5.625;
         }
         else
         {
            parsed_report_ptr->hor_unc_ellipse_orient_azimuth = pd_data_ptr->loc_uncertainty_ang * 5.625 + 90;
         }

         // Elliptical and circular uncertainty
         parsed_report_ptr->hor_unc_ellipse_semi_major = pd_data_ptr->ext_pos_data.alongAxisUnc; /* meters */
         parsed_report_ptr->hor_unc_ellipse_semi_minor = pd_data_ptr->ext_pos_data.perpAxisUnc; /* meters */
         parsed_report_ptr->hor_unc_circular = pd_data_ptr->ext_pos_data.loc_unc_horizontal; /* meters */

         // Vertical uncertainty
         parsed_report_ptr->vert_unc = positionUncertaintyTable[pd_data_ptr->loc_uncertainty_v];

         // Horizontal confidence
         parsed_report_ptr->confidence_horizontal = pd_data_ptr->loc_uncertainty_conf;
      }

      // Before LocMW 1.0u4, (Lat==0 && Lon==0) was treated as error case and the session status was not set
      // the position report would be ignored by at least Android driver
      // This behavior has been said to be wrong by marketing and we are going to report Lat/Lon 0/0 as valid, wild guess of the current location
      // Set valid_mask non-zero so the report is sent to clients
      parsed_report_ptr->valid_mask |= LOC_POS_VALID_SESSION_STATUS;
      // avoid translation to windows mobile generall error code
      parsed_report_ptr->session_status = LOC_SESS_STATUS_IN_PROGESS;

      // timestamp_utc is always valid
      loc_pd_fill_timestamp_utc(parsed_report_ptr, pd_data_ptr);

      // Populate technology mask 
      parsed_report_ptr->technology_mask = 
         loc_pd_convert_technology_mask_from_pdapi(pd_data_ptr->positioning_source);

      if(0 != parsed_report_ptr->technology_mask )
      {
         parsed_report_ptr->valid_mask |= LOC_POS_VALID_TECHNOLOGY_MASK;
      }

      // no validity bit defined in pdsm_pd_data_type
      parsed_report_ptr->valid_mask |= (LOC_POS_VALID_SENSOR_AIDING_IND |
                                        LOC_POS_VALID_SENSOR_DATA_USAGE |
                                        LOC_POS_VALID_H_RELIABILITY_IND |
                                        LOC_POS_VALID_V_RELIABILITY_IND);
      parsed_report_ptr->sensor_aiding_indicator = pd_data_ptr->w_SensorAidingIndicator;
      parsed_report_ptr->sensor_data_usage_mask = pd_data_ptr->w_SensorDataUsageMask;
      parsed_report_ptr->hor_reliability = pd_data_ptr->e_HoriRelIndicator;
      parsed_report_ptr->vert_reliability = pd_data_ptr->e_VertRelIndicator;

      // no validity bit defined in pdsm_pd_data_type
      // PDAPI refers these fields as self-validation as there are value combinations
      // indicating it's invalid
      {
          parsed_report_ptr->valid_mask |= (LOC_POS_VALID_TIME_SOURCE | LOC_POS_VALID_EXT_TIME_SOURCE);

          parsed_report_ptr->time_soruce = pd_data_ptr->ext_pos_data.time_src;
          parsed_report_ptr->extended_time_source = pd_data_ptr->ext_pos_data.extended_time_source;
      }

      if( pd_data_ptr->ext_opt_field_mask & PDSM_PD_LAT_LONG_VALID)
      {
          parsed_report_ptr->valid_mask |= LOC_POS_VALID_DOP;
          parsed_report_ptr->position_dop = (float)(pd_data_ptr->ext_pos_data.p_dop) / 10.0f;
          parsed_report_ptr->hor_dop = (float)(pd_data_ptr->ext_pos_data.h_dop) / 10.0f;
          parsed_report_ptr->vert_dop = (float)(pd_data_ptr->ext_pos_data.v_dop) / 10.0f;
      }

      //copy the sv used information 
      {
         //"sv_id" in the PDAPI structure actually contains the mask of the sv's used in the 
         //fix
         // Copy the SV used information.
         memscpy(&parsed_report_ptr->z_NavSvInfo, sizeof(parsed_report_ptr->z_NavSvInfo),
                &ext_pos_report->pd_data.z_NavSvInfo, 
                sizeof(ext_pos_report->pd_data.z_NavSvInfo));
         if(parsed_report_ptr->z_NavSvInfo.u_SvInfoValid) 
         {
           parsed_report_ptr->valid_mask |= LOC_POS_VALID_SV_USED_MASK;
           LOC_MSG_MED("loc_pd_get_report (intermediate): SV used in pos fix GPS::%d, GLO::%d, BDS::%d\n",
                    parsed_report_ptr->z_NavSvInfo.u_GpsNumSvPosFix,
                    parsed_report_ptr->z_NavSvInfo.u_GloNumSvPosFix,
                    parsed_report_ptr->z_NavSvInfo.u_BdsNumSvPosFix);
         }         
       }

      // Cache network reference and GNSS position 
      if (pd_data_ptr->positioning_source == PDSM_PD_POSITION_SOURCE_DEFAULT ||
          pd_data_ptr->positioning_source == PDSM_PD_POSITION_SOURCE_AFLT)
      {
         time_get_uptime_ms (time_ms);
         pd_module_data_ptr->cached_nw_ref_pos_time = QW_CVT_Q2N(time_ms);
         memscpy (&(pd_module_data_ptr->cached_nw_ref_pos),
            sizeof (pd_module_data_ptr->cached_nw_ref_pos),
                 parsed_report_ptr, sizeof(loc_parsed_position_s_type));
      }
      else if (pd_data_ptr->positioning_source == PDSM_PD_POSITION_SOURCE_GPS)
      {
         time_get_uptime_ms (time_ms);
         pd_module_data_ptr->cached_gnss_pos_time = QW_CVT_Q2N(time_ms);
         memscpy (&(pd_module_data_ptr->cached_gnss_pos),
                 sizeof (pd_module_data_ptr->cached_gnss_pos),
                 parsed_report_ptr, sizeof(loc_parsed_position_s_type));
      }

   }/* Intermediate positions */

   // Fields are populated according to the order in GPS_POSITION
   if (TRUE == process_pd_info)
   {
      pd_info_ptr     = &(pd_module_data_ptr->pd_info_copy);
      pd_data_ptr     = &(pd_info_ptr->pd_info.pd_data);
      pd_ext_data_ptr = &(pd_info_ptr->pd_info.pd_ext_data);

      parsed_report_ptr->valid_mask |= LOC_POS_VALID_SESSION_STATUS;
      // avoid translation to windows mobile generall error code
      parsed_report_ptr->session_status = LOC_SESS_STATUS_SUCCESS;

      // fill in utc time and get the leap_seconds, this is based on GPS timestamp
      leap_seconds = loc_pd_fill_timestamp_utc(parsed_report_ptr, pd_data_ptr);

// in older Aries 4.2 targets, PDSM_PD_UTC_TIME_VALID is actually not defined
// the best thing we can do is to assume time is always valid
#ifdef PDSM_PD_UTC_TIME_VALID
      if (pd_data_ptr->ext_opt_field_mask & PDSM_PD_UTC_TIME_VALID)
#endif
      {
         // timestamp_calendar is always valid
         parsed_report_ptr->valid_mask |= LOC_POS_VALID_TIMESTAMP_CALENDAR;
         // pd_ext_data_ptr->date in format of ddmmyyyy
         parsed_report_ptr->timestamp_calendar.year   = (uint16)((pd_ext_data_ptr->date) % 10000);
         parsed_report_ptr->timestamp_calendar.month  = (unsigned char) ((pd_ext_data_ptr->date) / 10000 % 100);
         parsed_report_ptr->timestamp_calendar.day    = (unsigned char) ((pd_ext_data_ptr->date) / 1000000);
         // pd_ext_data_ptr->hour, range from 0-23
         parsed_report_ptr->timestamp_calendar.hour   = (unsigned char) pd_ext_data_ptr->hour;
         // pd_ext_data_ptr->minute, range from 0-59
         parsed_report_ptr->timestamp_calendar.minute = (unsigned char) pd_ext_data_ptr->minute;
         // pd_ext_data_ptr->sec, units of 1/100 seconds, in range from 0-5999
         parsed_report_ptr->timestamp_calendar.second = (unsigned char) (pd_ext_data_ptr->sec / 100);
         parsed_report_ptr->timestamp_calendar.millisecond = (uint16) pd_ext_data_ptr->sec % 100 * 10;
         // 1980/01/06 is a Sunday (0). No offset needed as compared to the 1980/01/06
         parsed_report_ptr->timestamp_calendar.day_of_week = (unsigned long) (((pd_data_ptr->time_stamp - leap_seconds) / SECONDS_IN_ONE_DAY) % DAYS_IN_ONE_WEEK);

         // TBD: time uncertainty, not available yet
      }

      // lat and lon is not encrypted
      if (PDSM_PD_CRYPT_ALG_NONE == pd_info_ptr->pd_info.encrypt_data.encrypt_alg)
      {
         // populate the latitude and longitude
         parsed_report_ptr->valid_mask |= LOC_POS_VALID_LATITUDE;
         parsed_report_ptr->valid_mask |= LOC_POS_VALID_LONGITUDE;
         parsed_report_ptr->latitude  = ((double) pd_ext_data_ptr->lat) / LAT_LON_SCALE_FACTOR;
         parsed_report_ptr->longitude = ((double) pd_ext_data_ptr->lon) / LAT_LON_SCALE_FACTOR;
      }
      else
      {
         LOC_MSG_HIGH ("loc_pd_get_report, position encrypted", 0, 0, 0);
      }

      // Populate the altitude field
      
      parsed_report_ptr->valid_mask |= LOC_POS_VALID_ALTITUDE_WRT_ELLIPSOID;
      parsed_report_ptr->altitude_wrt_ellipsoid = 
         (float)(pd_data_ptr->altitude - LOC_PD_ALTITUDE_WRT_WGS84_ELLIPSOID_OFFSET);

      parsed_report_ptr->valid_mask |= LOC_POS_VALID_ALTITUDE_WRT_MEAN_SEA_LEVEL;
      parsed_report_ptr->altitude_wrt_mean_sea_level = parsed_report_ptr->altitude_wrt_ellipsoid -
      loc_pd_get_geoidal_separation (parsed_report_ptr->latitude, parsed_report_ptr->longitude);

      if (pd_ext_data_ptr->velocity_include == 1)
      {
         parsed_report_ptr->valid_mask |= LOC_POS_VALID_SPEED_HORIZONTAL;
         parsed_report_ptr->valid_mask |= LOC_POS_VALID_SPEED_VERTICAL;
         parsed_report_ptr->valid_mask |= LOC_POS_VALID_MAGNETIC_VARIATION;
         parsed_report_ptr->speed_horizontal  = ((float) pd_data_ptr->velocity_hor)/4;
         parsed_report_ptr->speed_vertical  = ((float) pd_data_ptr->velocity_ver)/2;
         parsed_report_ptr->magnetic_deviation = ((float) pd_ext_data_ptr->magnetic_variation) / 10;
  
         parsed_report_ptr->valid_mask |= LOC_POS_VALID_SPEED_ENU;
         memscpy(parsed_report_ptr->f_VelocityENUMps,sizeof(parsed_report_ptr->f_VelocityENUMps),
                 pd_ext_data_ptr->f_VelocityENUMps, sizeof(pd_ext_data_ptr->f_VelocityENUMps));
         memscpy(parsed_report_ptr->f_VelocityUncENUMps,sizeof(parsed_report_ptr->f_VelocityUncENUMps),
                 pd_ext_data_ptr->f_VelocityUncENUMps, sizeof(pd_ext_data_ptr->f_VelocityUncENUMps));

         //copy speed unc from ext_pos_data
         if(pd_data_ptr->opt_field_mask & PDSM_PD_VELOCITY_VALID)
         {
            /* overwrite the speed values with EXT-POS if available */
            parsed_report_ptr->speed_horizontal  = ((float) pd_data_ptr->ext_pos_data.velHor)/100;  /* original in cm/s */
            parsed_report_ptr->speed_vertical  = ((float) pd_data_ptr->ext_pos_data.velVert)/100;   /* original in cm/s */  

            parsed_report_ptr->valid_mask |= LOC_POS_VALID_SPEED_UNC;
            /* convert PDAPI horizontal and vertical velocity unc to speed
               unc */
            parsed_report_ptr->speed_unc = 
               sqrt( ((float) pd_data_ptr->ext_pos_data.velUncHoriz) *
                      pd_data_ptr->ext_pos_data.velUncHoriz +
                     ((float) pd_data_ptr->ext_pos_data.velUncVert) *
                      pd_data_ptr->ext_pos_data.velUncVert ) / 100; 
         }

         // Heading is only valid if speed is valid and it is greater than 0.1m/sec
         if (parsed_report_ptr->speed_horizontal >= 0.1)
         {
             parsed_report_ptr->valid_mask |= LOC_POS_VALID_HEADING;
             parsed_report_ptr->heading = ((float) pd_ext_data_ptr->heading_true) / 10; 
             //copy heading unc from ext_pos_data
             parsed_report_ptr->valid_mask |= LOC_POS_VALID_HEADING_UNC;
             parsed_report_ptr->heading_unc = pd_data_ptr->ext_pos_data.heading_unc_rad;
         }
      }

      // populate the uncertainty field
      if ((pd_data_ptr->opt_field_mask & PDSM_PD_UNCERTAINTY_VALID) != 0)
      {
         parsed_report_ptr->valid_mask |= LOC_POS_VALID_HOR_UNC_ELLI_SEMI_MAJ;
         parsed_report_ptr->valid_mask |= LOC_POS_VALID_HOR_UNC_ELLI_SEMI_MIN;
         parsed_report_ptr->valid_mask |= LOC_POS_VALID_HOR_UNC_ELLI_ORIENT_AZIMUTH;
         parsed_report_ptr->valid_mask |= LOC_POS_VALID_HOR_UNC_CIRCULAR;
         parsed_report_ptr->valid_mask |= LOC_POS_VALID_VERTICAL_UNC;
         parsed_report_ptr->valid_mask |= LOC_POS_VALID_CONFIDENCE_HORIZONTAL;

         // Elliptical and circular uncertainty
         // Note that in PDAPI, the angle is always 0-90 degrees, and length 'along' might be actually shorter 
         // than length on the 'perpendicular' axis
         // However, we have decided to ensure the length 'along' to be longer, and change the range of angle
         // to 0-180 degrees
         if(pd_data_ptr->loc_uncertainty_a >= pd_data_ptr->loc_uncertainty_p)
         {
            parsed_report_ptr->hor_unc_ellipse_semi_major = pd_data_ptr->ext_pos_data.alongAxisUnc; /* meters */
            parsed_report_ptr->hor_unc_ellipse_semi_minor = pd_data_ptr->ext_pos_data.perpAxisUnc; /* meters */
            parsed_report_ptr->hor_unc_ellipse_orient_azimuth = pd_data_ptr->loc_uncertainty_ang * 5.625;
         }
         else
         {
            parsed_report_ptr->hor_unc_ellipse_semi_minor = pd_data_ptr->ext_pos_data.alongAxisUnc; /* meters */
            parsed_report_ptr->hor_unc_ellipse_semi_major = pd_data_ptr->ext_pos_data.perpAxisUnc; /* meters */
            parsed_report_ptr->hor_unc_ellipse_orient_azimuth = pd_data_ptr->loc_uncertainty_ang * 5.625 + 90;
         }
         parsed_report_ptr->hor_unc_circular = pd_data_ptr->ext_pos_data.loc_unc_horizontal; /* meters */

         // Vertical uncertainty
         parsed_report_ptr->vert_unc = positionUncertaintyTable[pd_data_ptr->loc_uncertainty_v];

         // Horizontal confidence
         parsed_report_ptr->confidence_horizontal = pd_data_ptr->loc_uncertainty_conf;
      }

      // Populate technology mask 
      parsed_report_ptr->technology_mask = 
         loc_pd_convert_technology_mask_from_pdapi(pd_data_ptr->positioning_source);

      if(0 != parsed_report_ptr->technology_mask )
      {
         parsed_report_ptr->valid_mask |= LOC_POS_VALID_TECHNOLOGY_MASK;
      }

      // Populate the SV info
      loc_pd_get_sv_report (gnss_report_ptr);

      // Altitude assumed or calcuated information
      gnss_report_ptr->valid_mask |= LOC_GNSS_INFO_VALID_ALTITUDE_ASSUMED;  /* for SV report */
      parsed_report_ptr->valid_mask |= LOC_POS_VALID_ALTITUDE_ASSUMED_MASK;  /* for position report */

      // Populate fix type, no dwValidFields for FixType
      if (PDSM_PD_FIX_TYPE_2D == pd_ext_data_ptr->fix_type)
      {
         /* for SV report */
         gnss_report_ptr->valid_mask |= LOC_GNSS_INFO_VALID_ALTITUDE_ASSUMED;
         gnss_report_ptr->altitude_assumed = TRUE;

         /* for Position report */
         parsed_report_ptr->valid_mask |= LOC_POS_VALID_ALTITUDE_ASSUMED_MASK;
         parsed_report_ptr->altitude_assumed = TRUE;

      }
      else if (PDSM_PD_FIX_TYPE_3D == pd_ext_data_ptr->fix_type)
      {
         /* for SV report */
         gnss_report_ptr->valid_mask |= LOC_GNSS_INFO_VALID_ALTITUDE_ASSUMED;
         gnss_report_ptr->altitude_assumed = FALSE;

         /* for Position report */
         parsed_report_ptr->valid_mask |= LOC_POS_VALID_ALTITUDE_ASSUMED_MASK;
         parsed_report_ptr->altitude_assumed = FALSE;
      }

      // no validity bit defined in pdsm_pd_data_type
      parsed_report_ptr->valid_mask |= (LOC_POS_VALID_SENSOR_AIDING_IND |
                                        LOC_POS_VALID_SENSOR_DATA_USAGE |
                                        LOC_POS_VALID_H_RELIABILITY_IND |
                                        LOC_POS_VALID_V_RELIABILITY_IND);
      parsed_report_ptr->sensor_aiding_indicator = pd_data_ptr->w_SensorAidingIndicator;
      parsed_report_ptr->sensor_data_usage_mask = pd_data_ptr->w_SensorDataUsageMask;
      parsed_report_ptr->hor_reliability = pd_data_ptr->e_HoriRelIndicator;
      parsed_report_ptr->vert_reliability = pd_data_ptr->e_VertRelIndicator;

      // no validity bit defined in pdsm_pd_data_type
      // PDAPI refers these fields as self-validation as there are value combinations
      // indicating it's invalid
      {
          parsed_report_ptr->valid_mask |= (LOC_POS_VALID_TIME_SOURCE | LOC_POS_VALID_EXT_TIME_SOURCE);

          parsed_report_ptr->time_soruce = pd_data_ptr->ext_pos_data.time_src;
          parsed_report_ptr->extended_time_source = pd_data_ptr->ext_pos_data.extended_time_source;
      }

      if( pd_data_ptr->ext_opt_field_mask & PDSM_PD_LAT_LONG_VALID)
      {
          parsed_report_ptr->valid_mask |= LOC_POS_VALID_DOP;
          parsed_report_ptr->position_dop = (float)(pd_data_ptr->ext_pos_data.p_dop) / 10.0f;
          parsed_report_ptr->hor_dop = (float)(pd_data_ptr->ext_pos_data.h_dop) / 10.0f;
          parsed_report_ptr->vert_dop = (float)(pd_data_ptr->ext_pos_data.v_dop) / 10.0f;
      }

      //copy the sv used information, 
      {
         // Copy the SV used information.
         memscpy(&parsed_report_ptr->z_NavSvInfo, sizeof(parsed_report_ptr->z_NavSvInfo),
                &pd_data_ptr->z_NavSvInfo, 
                sizeof(pd_data_ptr->z_NavSvInfo));
         if (parsed_report_ptr->z_NavSvInfo.u_SvInfoValid)
         {
           parsed_report_ptr->valid_mask |= LOC_POS_VALID_SV_USED_MASK;
           LOC_MSG_MED("loc_pd_get_report (final): gps_sv_mask=0x%x, glo_sv_mask=0x%x, bds_sv_mask=0x%llx\n",
                       pd_data_ptr->z_NavSvInfo.q_GpsSvMaskUsed,
                       pd_data_ptr->z_NavSvInfo.q_GloSvMaskUsed,
                       pd_data_ptr->z_NavSvInfo.t_BdsSvMaskUsed);
         }
      }

      // Cache network reference and GNSS position 
      if (pd_data_ptr->positioning_source == PDSM_PD_POSITION_SOURCE_DEFAULT ||
          pd_data_ptr->positioning_source == PDSM_PD_POSITION_SOURCE_AFLT)
      {
         time_get_uptime_ms (time_ms);
         pd_module_data_ptr->cached_nw_ref_pos_time = QW_CVT_Q2N(time_ms);
         memscpy (&(pd_module_data_ptr->cached_nw_ref_pos), 
                 sizeof (pd_module_data_ptr->cached_nw_ref_pos),
                 parsed_report_ptr, sizeof(loc_parsed_position_s_type));
      }
      else if (pd_data_ptr->positioning_source == PDSM_PD_POSITION_SOURCE_GPS)
      {
         time_get_uptime_ms (time_ms);
         pd_module_data_ptr->cached_gnss_pos_time = QW_CVT_Q2N(time_ms);
         memscpy (&(pd_module_data_ptr->cached_gnss_pos), 
                 sizeof (pd_module_data_ptr->cached_gnss_pos),
                 parsed_report_ptr, sizeof(loc_parsed_position_s_type));
      }

      loc_pd_dump_position_report (parsed_report_ptr);
   }

   // if we receive new meas report regarding sv info, get the satellite information from the meas report
   if ((FALSE == process_pd_info) && (process_ext_status_info == TRUE))
   {
      loc_pd_get_sv_report (gnss_report_ptr);
      loc_pd_dump_sv_info (gnss_report_ptr);
   }

   // process the error event
   if (TRUE == process_pd_sess_end_status)
   {
      parsed_report_ptr->valid_mask |= LOC_POS_VALID_SESSION_STATUS;
      // avoid translation to windows mobile generall error code
      parsed_report_ptr->session_status = loc_pd_xlate_fix_end_reason_code (pd_module_data_ptr->pd_sess_end_status_copy);
   }
   else if (TRUE == process_pd_cmd_err)
   {
      parsed_report_ptr->valid_mask |= LOC_POS_VALID_SESSION_STATUS;
      // avoid translation to windows mobile generall error code
      parsed_report_ptr->session_status = loc_pd_xlate_cmd_err_code_to_sess_status (pd_module_data_ptr->pd_cmd_err_copy);
   }

   return TRUE;
}

//Retrieves GNSS sv measurement report
static void loc_pd_get_gnss_meas_report (loc_gnss_meas_s_type*     gnss_report_ptr)


{
   loc_pd_module_data_s_type *pd_module_data_ptr = &(loc_middleware_data.pd_module_data);

   pdsm_ext_status_info_s_type         *pd_info_ptr         = NULL;
   pdsm_pd_ext_gnss_meas_s_type *pd_data_ptr         = NULL;
   //pdsm_pd_ext_gnss_meas_s_type *pd_ext_data_ptr     = NULL;
   boolean                      process_pd_info = FALSE,
                                process_gnss_meas_info    = FALSE,
                                process_interm_pos_info    = FALSE,
                                process_pd_sess_end_status = FALSE,
                                process_pd_cmd_err         = FALSE;
   int                          leap_seconds;
   int                          set_valid_mask_for_intermediate_pos = 1;
   time_type                    time_ms = {0};

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

   if (TRUE == pd_module_data_ptr->new_gnss_meas_info_received)
   {
      LOC_MSG_HIGH ("loc_pd_get_raw_sv_report \n", 0, 0, 0);
      pd_module_data_ptr->new_gnss_meas_info_received = FALSE;
      process_gnss_meas_info      = TRUE;
      memscpy (&(pd_module_data_ptr->gnss_meas_info_copy),
             sizeof(pd_module_data_ptr->gnss_meas_info_copy),
            &(pd_module_data_ptr->gnss_meas_info),
            sizeof (pdsm_ext_status_info_s_type));
   }

   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

   // Initialzie the report structure
   memset (gnss_report_ptr,   0, sizeof (loc_gnss_meas_s_type));

   if (TRUE == process_gnss_meas_info)
   {
     pd_info_ptr     = &(pd_module_data_ptr->gnss_meas_info_copy);
     pd_data_ptr     = &(pd_info_ptr->ext_status_info.gnss_meas_report_type);


         // Copy the SV used information.
         memscpy(&gnss_report_ptr->z_NavGnssMeas,
                 sizeof(gnss_report_ptr->z_NavGnssMeas),
                (pd_data_ptr), 
                sizeof(gnss_report_ptr->z_NavGnssMeas));
   }
//   return TRUE;
}


static void loc_pd_sv_poly_report (loc_sv_poly_s_type*     sv_poly_ptr)


{
   loc_pd_module_data_s_type *pd_module_data_ptr = &(loc_middleware_data.pd_module_data);

   pdsm_ext_status_info_s_type         *pd_info_ptr         = NULL;
   pdsm_pd_ext_SvPoly_s_type *pd_data_ptr         = NULL;
   boolean                      process_sv_poly_info    = FALSE;

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

   if (TRUE == pd_module_data_ptr->new_sv_poly_info_received)
   {
      LOC_MSG_HIGH ("loc_pd_sv_poly_report \n", 0, 0, 0);
      pd_module_data_ptr->new_sv_poly_info_received = FALSE;
      process_sv_poly_info      = TRUE;
      memscpy (&(pd_module_data_ptr->sv_poly_info_copy),
          sizeof(pd_module_data_ptr->sv_poly_info_copy),
            &(pd_module_data_ptr->sv_poly_info),
            sizeof (pdsm_ext_status_info_s_type));
   }

   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

   // Initialzie the parsed report structure
   memset (sv_poly_ptr,   0, sizeof (loc_sv_poly_s_type));

   if (TRUE == process_sv_poly_info)
   {
     pd_info_ptr     = &(pd_module_data_ptr->sv_poly_info_copy);
     pd_data_ptr     = &(pd_info_ptr->ext_status_info.ext_sv_poly_type);


         // Copy the SV used information.
         memscpy(&sv_poly_ptr->z_SvPoly,
                sizeof(sv_poly_ptr->z_SvPoly),
                (pd_data_ptr), 
                sizeof(sv_poly_ptr->z_SvPoly));
   }
//   return TRUE;
}


// Position injection IOCTL
int loc_pd_process_ioctl (loc_client_handle_type  client_handle,
                          loc_ioctl_e_type ioctl_type,
                          const loc_ioctl_data_u_type* ioctl_data_ptr)
{
   int                        ret_val            = LOC_API_GENERAL_FAILURE;

   switch (ioctl_type)
   {
   case LOC_IOCTL_INJECT_POSITION:
      if(NULL != ioctl_data_ptr)
      {
          ret_val = loc_pd_process_ext_pos_injection (client_handle, &(ioctl_data_ptr->assistance_data_position));
      }
      else
      {
          ret_val = LOC_API_INVALID_PARAMETER;
      }
      break;

   case LOC_IOCTL_GET_BEST_AVAILABLE_POSITION:
      ret_val = loc_pd_process_get_best_available_position( 
         client_handle, &(ioctl_data_ptr->bap_req));
      break;
     
   case LOC_IOCTL_GET_AVAILABLE_WWAN_POSITION:
      ret_val = loc_pd_process_get_available_wwan_position( 
         client_handle, &(ioctl_data_ptr->z_GetAvailWwanPosReq));
      break;
  case LOC_IOCTL_GET_AVAILABLE_POSITION_FROM_SRC:
      ret_val = loc_pd_process_get_available_position( 
         client_handle, &(ioctl_data_ptr->z_GetAvailablePosReq));
      break;

   case LOC_IOCTL_INJECT_GSM_CELL_INFO_REQ:
      ret_val = loc_pd_process_inject_gsm_cell_info_req( 
         client_handle, &(ioctl_data_ptr->inject_gsm_cell_info));
      break;

   case LOC_IOCTL_WWAN_OOS_NOTIFICATION_REQ:
      ret_val = loc_pd_process_wwan_oos_notification( 
         client_handle);
      break;

   case LOC_IOCTL_INJECT_NI_MESSAGE_REQ:
      ret_val = loc_pd_process_inject_ni_msg_req(
         client_handle, &(ioctl_data_ptr->inject_ni_msg) ); 
      break;
     
   default:
      break;
   }
   return ret_val;
}

// Function to process external coarse position injection request.
static int loc_pd_process_ext_pos_injection
(
      loc_client_handle_type            client_handle,
      const loc_assist_data_pos_s_type* locapi_ext_pos_ptr
)
{
   int                             ret_val;
   loc_pd_module_data_s_type*      pd_module_data_ptr = &(loc_middleware_data.pd_module_data);
   pdsm_pd_external_position_info_type  pdapi_external_pos;
   
   memset (&pdapi_external_pos, 0, sizeof(pdapi_external_pos));

   do
   {
   
     LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
      if (pd_module_data_ptr->position_injection_in_progress == TRUE)
      {
         ret_val = LOC_API_ENGINE_BUSY;
         LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
         break;
      }

      pd_module_data_ptr->position_injection_in_progress = TRUE;

      // Save the client handle for issuing callback later on
      pd_module_data_ptr->pos_inject_client_handle = client_handle;

      if (LOC_API_SUCCESS != loc_pd_xlate_external_pos_from_locapi_to_pdapi (locapi_ext_pos_ptr, &pdapi_external_pos))
      {
         ret_val = LOC_API_INVALID_PARAMETER;
         LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
         break;
      }

      LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
      if (FALSE == pdsm_pd_inject_external_position (loc_pd_cmd_cb,
                                                     NULL,           // user data pointer
                                                     &pdapi_external_pos,  // external position
                                                     loc_middleware_data.pdapi_client_id))    // Client id of user
      {
         ret_val = LOC_API_GENERAL_FAILURE;
         LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
         pd_module_data_ptr->position_injection_in_progress = FALSE;
         pd_module_data_ptr->pos_inject_client_handle = LOC_CLIENT_HANDLE_INVALID;         
         LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
         break;
      }
      ret_val = LOC_API_SUCCESS;
   } while (0);


   return ret_val;
}

// Routine to translate the client's fix criteria from LOC API to PD API. The translated PD API
// fix criteria will be used in the next position request.
void loc_pd_xlate_fix_config_from_locapi_to_pdapi (loc_client_info_s_type* client_ptr)
{
   loc_fix_criteria_s_type* fix_criteria_ptr;
   pdsm_pd_option_s_type*   pdsm_option_ptr;
   pdsm_pd_qos_type*        pdsm_qos_ptr;
   uint32                   q_fix_interval;

   fix_criteria_ptr = &(client_ptr->fix_criteria_info);
   pdsm_option_ptr  = &(loc_middleware_data.pd_module_data.pdsm_option);
   pdsm_qos_ptr     = &(loc_middleware_data.pd_module_data.pdsm_qos );

   pdsm_option_ptr->session                     = PDSM_PD_SESS_TYPE_NEW;
   pdsm_option_ptr->fix_rate.num_fixes          = UINT32_MAX;

   // Figure out fix interval
   if (fix_criteria_ptr->notify_type == LOC_NOTIFY_ON_DISTANCE)
   {
      q_fix_interval = fix_criteria_ptr->min_dist_sample_interval;
   }
   else if (LOC_SINGLE_FIX == fix_criteria_ptr->recurrence_type)
   {
      q_fix_interval = fix_criteria_ptr->min_interval;
   }
   else
   {
      q_fix_interval = loc_middleware_data.pd_module_data.loc_fix_criteria_rule.min_interval;
   }
   pdsm_option_ptr->fix_rate.time_between_fixes_ms = q_fix_interval;

   if (pdsm_option_ptr->fix_rate.time_between_fixes_ms < PDSM_MINIMUM_TBF_MS)
   {
      pdsm_option_ptr->fix_rate.time_between_fixes_ms = PDSM_MINIMUM_TBF_MS;
   }

   // copy the location server info from client
   memscpy (&(pdsm_option_ptr->lsinfo), sizeof(pdsm_option_ptr->lsinfo), &(client_ptr->server_info), sizeof (client_ptr->server_info));

   // translate the QOS info
   pdsm_qos_ptr->accuracy_threshold  = fix_criteria_ptr->preferred_accuracy;
   //preferred_response_time value check is done in loc_qmi_shim.c
   pdsm_qos_ptr->gps_session_timeout = (byte)(fix_criteria_ptr->preferred_response_time/1000);

   // For CP MO, floor qos timeout at 30 seconds 
   if (loc_middleware_data.pa_module_data.mo_method == MO_CP)
    {
      if (pdsm_qos_ptr->gps_session_timeout < LOC_CP_MO_RESPONSE_TIME/1000 )
      {
         pdsm_qos_ptr->gps_session_timeout = LOC_CP_MO_RESPONSE_TIME/1000;
      }
    }

   pdsm_option_ptr->operation_mode = loc_pd_xlate_fix_mode_from_locapi_to_pdapi (fix_criteria_ptr->preferred_operation_mode);

  /* Set QoS timeout to 0 if operation mode is WWAN */
   if(eQMI_LOC_OPER_MODE_WWAN_V02 == fix_criteria_ptr->preferred_operation_mode )
  {
      pdsm_qos_ptr->gps_session_timeout = 0;
  }

   // If mode is CELL ID (UMTS) or AFLT (MSA with QoS == 0), and if the engine is ON,
   // we change the mode to MSB ref loc, this is done so that the GPS engine can stay on
   // and in the meantime, we also get a ref loc from the network. Ref Location can be used
   // to give a good quality GPS fix with low power in this case.
   if (
       (pdsm_option_ptr->operation_mode == PDSM_SESSION_OPERATION_CELL_ID) ||
       (pdsm_option_ptr->operation_mode == PDSM_SESSION_OPERATION_MSASSISTED &&
        pdsm_qos_ptr->gps_session_timeout == 0))
  {
    if (loc_middleware_data.pd_module_data.engine_state == LOC_ENGINE_STATE_ON)
    {
         pdsm_option_ptr->operation_mode   = PDSM_SESSION_OPERATION_REF_POSITION;
         pdsm_qos_ptr->gps_session_timeout = LOC_MIN_RESPONSE_TIME/1000;
    }
  }

   LOC_MSG_MED ("loc pd,  operation mode = %d, time_between_fixes = %u, accuracy = %u",
                 pdsm_option_ptr->operation_mode,
				 pdsm_option_ptr->fix_rate.time_between_fixes_ms,
				 pdsm_qos_ptr->accuracy_threshold);
   LOC_MSG_MED ("loc pd,  timeout = %d, server opt = %d, server_addr = 0x%x",
                 pdsm_qos_ptr->gps_session_timeout,
				 pdsm_option_ptr->lsinfo.server_option,
				 pdsm_option_ptr->lsinfo.server_adrs.server_adrs.server_adrs_v4.server_adrs);

   /* populate the application ID */
   memset(&pdsm_option_ptr->supl_application_id, 0, 
          sizeof(pdsm_option_ptr->supl_application_id));

   if(fix_criteria_ptr->valid_mask & LOC_FIX_CRIT_VALID_APPLICATION_ID)
  {
      LOC_MSG_MED (" loc pd, application ID is valid version_valid = %d", 
                   fix_criteria_ptr->application_id.applicationVersion_valid, 0, 0);

      /* set the validity bit for mandatory information ( name and provider)*/
      pdsm_option_ptr->supl_application_id.u_validity |= 
         PDAPI_SUPL_APPLICATION_ID_VALID; 

      /* copy the application name */
      GNSS_STRLCPY((char *)pdsm_option_ptr->supl_application_id.u_app_name,
                   fix_criteria_ptr->application_id.applicationName,
                   sizeof(pdsm_option_ptr->supl_application_id.u_app_name));

       /* copy the application provider */
      GNSS_STRLCPY((char *)pdsm_option_ptr->supl_application_id.u_app_provider,
                   fix_criteria_ptr->application_id.applicationProvider,
                   sizeof(pdsm_option_ptr->supl_application_id.u_app_provider));

      if(fix_criteria_ptr->application_id.applicationVersion_valid)
    {
         // set the version validity mask
         pdsm_option_ptr->supl_application_id.u_validity |= 
            PDAPI_SUPL_APPLICATION_ID_APP_VERSION_VALID;

         /* copy the application provider */
         GNSS_STRLCPY((char *)pdsm_option_ptr->supl_application_id.u_app_version,
                      fix_criteria_ptr->application_id.applicationVersion,
                      sizeof(pdsm_option_ptr->supl_application_id.u_app_version));
    }
      
  }

}

loc_supl_req_e_type loc_pd_continue_processing_supl_request (loc_ni_supl_notify_verify_req_s_type* const supl_req_ptr)
{
    loc_supl_req_e_type result = LOC_SUPL_REQ_DENY;
    int trace = 0;
    loc_pd_module_data_s_type*      pd_module_data_ptr = &(loc_middleware_data.pd_module_data);
    loc_supl_mt_state_e_type supl_mt_fix_state_copy = LOC_SUPL_MT_IDLE;
    LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
        supl_mt_fix_state_copy = pd_module_data_ptr->supl_mt_fix_state;
    LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

    LOC_MSG_HIGH ("loc_pd_continue_processing_supl_request,first MT = %d, 2nd MT ES = %d, PO = %d", 
                   supl_mt_fix_state_copy, 
                   supl_req_ptr->supl_emergency_valid, 
                   supl_req_ptr->notification_priv_type );

    if(supl_req_ptr->pos_method == LOC_NI_POSMETHOD_NO_POSITION)
    {
        trace = 5;
        // NO_POS is allowed no matter what session is going on
        LOC_MSG_HIGH ("loc_pd_continue_processing_supl_request, alway accept NO_POSITION",0,0,0);
        result = LOC_SUPL_REQ_ACCEPT;
    }
    else if ((LOC_SUPL_MT_ES_ABOUT_TO_START == supl_mt_fix_state_copy) 
        || (LOC_SUPL_MT_ES_IN_PROGRESS == supl_mt_fix_state_copy))
    {
      // There is a SUPL MT ES already running .
      trace = 6;
      // The second incoming session 
      if (TRUE == supl_req_ptr->supl_emergency_valid)
      {
        trace = 7;
        // ES is allowed
        LOC_MSG_HIGH ("loc_pd_continue_processing_supl_request, accept ES while ES is ongoing",0,0,0);
        result = LOC_SUPL_REQ_ACCEPT;
      }
      else
      {
        trace = 8;
        // Non-ES is not allowed, and dropped
        LOC_MSG_HIGH ("loc_pd_continue_processing_supl_request, drop non-ES while ES is ongoing",0,0,0);
        result = LOC_SUPL_REQ_DROP;
      }
    }
    else if ((LOC_SUPL_MT_PO_ABOUT_TO_START == supl_mt_fix_state_copy) 
        || (LOC_SUPL_MT_PO_IN_PROGRESS == supl_mt_fix_state_copy))
    {
        // There is a SUPL MT PO already running . 
        trace = 1;

        // The second incoming session
        if(TRUE == supl_req_ptr->supl_emergency_valid ||
         LOC_NI_USER_PRIVACY_OVERRIDE == supl_req_ptr->notification_priv_type)
        {
            trace = 2;
            // ES is allowed, PO is allowed
            result = LOC_SUPL_REQ_ACCEPT;
            LOC_MSG_HIGH ("loc_pd_continue_processing_supl_request, accept ES/PO while PO is ongoing",0,0,0);
        }
        else
        {
            trace = 3;
            // Regular non-PO is not allowed
            result = LOC_SUPL_REQ_DENY;
            LOC_MSG_HIGH ("loc_pd_continue_processing_supl_request, deny Regular(non-PO) while PO is ongoing", 0,0,0);
        }
    }
    else
    {
        // There is a SUPL MT non-PO already running . The second incoming session is always allowed
        trace = 4;

        result = LOC_SUPL_REQ_ACCEPT;
        LOC_MSG_MED ("loc_pd_continue_processing_supl_request, accept all type while non-PO is ongoing",0,0,0);
    }
    LOC_MSG_HIGH ("loc_pd_process_supl_request, trace = %d,result=%d", trace,result,0);
    return result;
}

void loc_pd_handle_service_interaction (pdsm_lcs_notification_resp_s_type* resp_info_ptr)
{
   int trace = 0;
   loc_pd_module_data_s_type*      pd_module_data_ptr = &(loc_middleware_data.pd_module_data);

   //end_session_type
   pdsm_pd_end_session_opt_s_type end_session_opt = {FALSE};
   boolean b_endsession = FALSE;

   loc_supl_mt_state_e_type supl_mt_fix_state_copy = LOC_SUPL_MT_IDLE;
   boolean mo_fix_in_progress_copy = FALSE;

   // Read the state of SUPL ES if it is going on .
   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
     supl_mt_fix_state_copy = pd_module_data_ptr->supl_mt_fix_state;
     mo_fix_in_progress_copy = pd_module_data_ptr->mo_fix_in_progress;
   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

   if (resp_info_ptr->lcs_info_cmd_type != PDSM_LCS_CMD_TYPE_SUPL_REQ)
   {
     // this is not a SUPL request. do nothing here
     trace = 1;
   }
   else if(resp_info_ptr->lcs_info.supl_req.pos_method == POSMETHOD_NO_POSITION)
   {
     trace = 8;
     // NO POSITION request goes through no matter what session we have now
   }
   else if(resp_info_ptr->lcs_info.supl_req.flags & PDSM_LCS_SUPLINIT_VER2_EXT_EMERGENCY_PRESENT)
   {
     // new incoming is SUPL MT ES - PO does not need to check accept or deny
     if ((resp_info_ptr->lcs_info.supl_req.notification_priv_type == PDSM_LCS_PRIVACY_OVERRIDE)
      || (resp_info_ptr->resp == PDSM_LCS_NOTIFY_VERIFY_ACCEPT)
      || (resp_info_ptr->resp == PDSM_LCS_NOTIFY_VERIFY_NORESP))
     {
       // all -> ES
       // End any ongoing session to honor incoming SUPL MT(ES) if PO or granted by user
       LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
         pd_module_data_ptr->supl_mt_fix_state = LOC_SUPL_MT_ES_ABOUT_TO_START;
       LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
       LOC_MSG_HIGH ("loc_pd_handle_service_interaction, killing running session to serve SUPL MT ES", 0,0,0); 
       b_endsession = TRUE;
       trace = 21;
     }
     else
     {
       // User has denied the NI session. So do not kill any sessions. Just continue with the normal flow .
       LOC_MSG_HIGH ("loc_pd_handle_service_interaction, not PO, user denied NI request. ", 0,0,0);
       trace = 22;
     }
   }
   else if(resp_info_ptr->lcs_info.supl_req.notification_priv_type == PDSM_LCS_PRIVACY_OVERRIDE)
   {
     // new incoming is SUPL MT PO
     if ((LOC_SUPL_MT_ES_ABOUT_TO_START == supl_mt_fix_state_copy)
      || (LOC_SUPL_MT_ES_IN_PROGRESS == supl_mt_fix_state_copy))
     {
       // ES -> PO
       // ES is already running / about to begin. Rejects incoming session locally.
       trace = 41;
       LOC_MSG_HIGH ("loc_pd_handle_service_interaction, denying SUPL MT PO , for a SUPL MT ES is in progress", 0,0,0);
       resp_info_ptr->resp = PDSM_LCS_NOTIFY_VERIFY_DENY;
     }
     else
     {
       // all others -> PO
       trace = 61;
       // Ends ongoing session to honor incoming session and serve this SUPL MT PO request
       LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
         pd_module_data_ptr->supl_mt_fix_state = LOC_SUPL_MT_PO_ABOUT_TO_START;
       LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
       LOC_MSG_HIGH ("loc_pd_handle_service_interaction,  killing any running session to serve SUPL MT PO", 0,0,0);
       b_endsession = TRUE;
     }
   }
   else
   {
     // new incoming is SUPL MT reqular session
     if ((LOC_SUPL_MT_ES_ABOUT_TO_START == supl_mt_fix_state_copy)
      || (LOC_SUPL_MT_ES_IN_PROGRESS == supl_mt_fix_state_copy)
      || (LOC_SUPL_MT_PO_ABOUT_TO_START == supl_mt_fix_state_copy)
      || (LOC_SUPL_MT_PO_IN_PROGRESS == supl_mt_fix_state_copy))
     {
       // ES/PO -> regular
       trace = 42;
       // ES/PO is already running / about to begin. Rejects incoming session locally.
       LOC_MSG_HIGH ("loc_pd_handle_service_interaction, denying SUPL MT regular , for a SUPL MT ES/PO is in progress", 0,0,0);
       resp_info_ptr->resp = PDSM_LCS_NOTIFY_VERIFY_DENY;
     }
     else if((resp_info_ptr->resp == PDSM_LCS_NOTIFY_VERIFY_ACCEPT) ||
             (resp_info_ptr->resp == PDSM_LCS_NOTIFY_VERIFY_NORESP))
     {
       // all others -> regular
       // User has accepted / no response for the NI session . So we allow the SUPL session.
       // We end the ongoing MO sessions to honor the SUPL(MT) regular session.
       // Note that we do not kill an ongoing CP NI-LR ( ES or non-ES mode ) 
       LOC_MSG_HIGH ("loc_pd_handle_service_interaction, user accepted/no-resp for NI request. ", 0,0,0);
       // the response is not for SUPL MT PO (Privacy Override)
       if(TRUE == mo_fix_in_progress_copy)
       {
         trace = 91;
         // even for normal SUPL request, we kill all MO requests passing through LocAPI
         LOC_MSG_MED ("loc_pd_handle_service_interaction,  killing any MO session to serve SUPL", 0,0,0);
         b_endsession = TRUE;
       }
       else
       {
         trace = 92;
         // don't touch anything, let user's intention fall through
       }
     }
     else
     {
       // User has denied the NI session . So do not kill any sessions . Just continue with the normal flow . 
       LOC_MSG_HIGH ("loc_pd_handle_service_interaction, user denied NI request. ", 0,0,0);
       trace = 93;
     }
   }

   if (TRUE == b_endsession)
   {
     LOC_MSG_MED ("loc_pd_handle_service_interaction, killing ongoing session", 0,0,0);
     end_session_opt.receiver_off = TRUE;
     pdsm_end_session_ex(loc_pd_cmd_cb,
                         PDSM_PD_END_SESSION_FIX_REQ,
                         &end_session_opt,
                         NULL,
                         loc_middleware_data.pdapi_client_id);
   }
   LOC_MSG_HIGH ("loc_pd_handle_service_interaction,  trace %d", trace,0,0);
}

// Routine to translate fix mode from Loc API to PD API.
pdsm_pd_session_operation_e_type  loc_pd_xlate_fix_mode_from_locapi_to_pdapi
   (qmiLocOperationModeEnumT_v02    loc_operation_mode)
{
   switch (loc_operation_mode)
   {
   case eQMI_LOC_OPER_MODE_STANDALONE_V02:
      return PDSM_SESSION_OPERATION_STANDALONE_ONLY;

   /* set default to MSB*/
   case eQMI_LOC_OPER_MODE_DEFAULT_V02:
   case eQMI_LOC_OPER_MODE_MSB_V02:
      return PDSM_SESSION_OPERATION_MSBASED;

   /* set WWAN mode to MSA, qoS = 0 will be set in the
      calling function */
   case eQMI_LOC_OPER_MODE_WWAN_V02:
   case eQMI_LOC_OPER_MODE_MSA_V02:
      return PDSM_SESSION_OPERATION_MSASSISTED;

   case eQMI_LOC_OPER_MODE_CELL_ID_V02:
      return PDSM_SESSION_OPERATION_CELL_ID;

 /*Set default to MS based */
   default:
      return PDSM_SESSION_OPERATION_MSBASED;
   }

}

// PD event callback, this is where the position report will come.
static void loc_pd_event_cb (void                         *user_data,
                             pdsm_pd_event_type           pd_event,
                             const pdsm_pd_info_s_type    *pd_info_ptr)
{
   loc_pd_module_data_s_type* pd_module_data_ptr = &(loc_middleware_data.pd_module_data);
   loc_cmd_s_type             cmd_data;
   loc_cmd_s_type*            cmd_ptr = &cmd_data;
   time_type                  time_ms = {0};
   uint64                     current_time_ms;
   int                        ioctl_status;
   boolean                    result = TRUE;
   loc_cmd_nty_client_s_type * ptr_nty_client_data = NULL;

   LOC_MSG_HIGH ("loc_pd_event_cb, pd_event=0x%x, client_id=%d", pd_event, pd_info_ptr->client_id, 0);

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
   memset (cmd_ptr, 0, sizeof (loc_cmd_s_type));
   // location engine is turned on, ignore client id
   if ((pd_event & PDSM_PD_EVENT_GPS_BEGIN) || (pd_event & PDSM_PD_EVENT_GPS_DONE))
   {
      // Queue the cmd to start another round of fix processing
      cmd_ptr->cmd_type = LOC_CMD_TYPE_NOTIFY_CLIENT;
      cmd_ptr->client_handle = LOC_HANDLE_REGISTERED_CLIENTS;
      cmd_ptr->ptr_data = NULL;
      ptr_nty_client_data = (loc_cmd_nty_client_s_type *)loc_calloc(sizeof(loc_cmd_nty_client_s_type));
      if(NULL != ptr_nty_client_data)
      {
          ptr_nty_client_data->event_type = LOC_EVENT_STATUS_REPORT;
          if (pd_event & PDSM_PD_EVENT_GPS_BEGIN)
          {
             LOC_MSG_MED ("loc_pd_event_cb, PDSM_PD_EVENT_GPS_BEGIN", 0,0,0);
                pd_module_data_ptr->engine_state = LOC_ENGINE_STATE_ON;
                ptr_nty_client_data->event_data.status_report.event = LOC_STATUS_EVENT_ENGINE_STATE;
                ptr_nty_client_data->event_data.status_report.payload.engine_state = pd_module_data_ptr->engine_state;
             }
          else
          {
             LOC_MSG_MED ("loc_pd_event_cb, PDSM_PD_EVENT_GPS_DONE", 0,0,0);
                pd_module_data_ptr->engine_state = LOC_ENGINE_STATE_OFF;
                ptr_nty_client_data->event_data.status_report.event = LOC_STATUS_EVENT_ENGINE_STATE;
                ptr_nty_client_data->event_data.status_report.payload.engine_state = pd_module_data_ptr->engine_state;
             }
          cmd_ptr->ptr_data = (void *)ptr_nty_client_data;
          if ( TRUE != loc_middleware_queue_cmd (cmd_ptr))
          {
              // reclaim resources allocated for the message
              loc_free(ptr_nty_client_data);
              ptr_nty_client_data = NULL;
              cmd_ptr->ptr_data = NULL;
              result = FALSE;
          }
      }
      else
      {
         // memory allocation error
         result = FALSE;
      }

      if(TRUE != result)
      {
         // TODO: what else can we do?
         LOC_MSG_ERROR ("loc_pd_event_cb, failed processing engine status event", 0, 0, 0);
      }
   }

   // gps session begins
   if (pd_event & PDSM_PD_EVENT_BEGIN)
   {
      // Check whether it is a recognised client. 
      if ((loc_middleware_data.pdapi_client_id != pd_info_ptr->client_id) &&
          (loc_middleware_data.ni_client_id != pd_info_ptr->client_id))
      {
         LOC_MSG_ERROR ("loc_pd_event_cb, dropped session_begin for unrecognized client", 
                        0, 0, 0);
      }
      else // recognised client
      {
         pd_module_data_ptr->fix_in_progress = TRUE;
         time_get_uptime_ms (time_ms);
         current_time_ms = QW_CVT_Q2N(time_ms);
         pd_module_data_ptr->last_get_position_time = current_time_ms;

         pd_module_data_ptr->fix_in_progress = TRUE;

         // will be set to true if sent by pdapi_client_id
         pd_module_data_ptr->mo_fix_in_progress = FALSE;

         result = TRUE; // will be set to FALSE for failure case explicitly

         if(loc_middleware_data.ni_client_id == pd_info_ptr->client_id)
         {
            int type = 0;
            // NI session starts
            if(LOC_SUPL_MT_ES_ABOUT_TO_START == pd_module_data_ptr->supl_mt_fix_state)
            {
               // move from ABOUT_TO_START to IN_PROGRESS
               // note we don't really know if this is the right session, but we can only guess
               pd_module_data_ptr->supl_mt_fix_state = LOC_SUPL_MT_ES_IN_PROGRESS;
               type = 2;
            }
            else if(LOC_SUPL_MT_PO_ABOUT_TO_START == pd_module_data_ptr->supl_mt_fix_state)
            {
               // move from ABOUT_TO_START to IN_PROGRESS
               // note we don't really know if this is the right session, but we can only guess
               pd_module_data_ptr->supl_mt_fix_state = LOC_SUPL_MT_PO_IN_PROGRESS;
               type = 1;
            }
            else
            {
               // ni but probably not our SUPL MT PO session or SUPL MT ES session
               // note we cannot be sure about this
               type = 3;
            }
            LOC_MSG_HIGH ("loc_pd_event_cb NI client session begin %d", type, 0, 0);
         }
         // notify the active client only in case of MO fixes
         else if (loc_middleware_data.pdapi_client_id == pd_info_ptr->client_id) 
         {
            // set mo fix flag to true
            pd_module_data_ptr->mo_fix_in_progress = TRUE;
            //send the fix session state event
            cmd_ptr->cmd_type = LOC_CMD_TYPE_NOTIFY_CLIENT;
            cmd_ptr->client_handle = loc_client_get_active_client();
            cmd_ptr->ptr_data = NULL;
            ptr_nty_client_data = (loc_cmd_nty_client_s_type *)loc_calloc(sizeof(loc_cmd_nty_client_s_type));
            if(NULL != ptr_nty_client_data)
            {
               ptr_nty_client_data->event_type = LOC_EVENT_STATUS_REPORT;
               ptr_nty_client_data->event_data.status_report.event = LOC_STATUS_EVENT_FIX_SESSION_STATE;
               ptr_nty_client_data->event_data.status_report.payload.fix_session_state = LOC_FIX_SESSION_STATE_BEGIN;

               cmd_ptr->ptr_data = (void *)ptr_nty_client_data;
               if ( TRUE != loc_middleware_queue_cmd (cmd_ptr))
               {
                  // IPC failure, reclaim resources
                  loc_free(ptr_nty_client_data);
                  ptr_nty_client_data = NULL;
                  cmd_ptr->ptr_data = NULL;
                  result = FALSE;
               }
            }
            else
            {
               // memory allocation failure
               result = FALSE;
            }
            if(TRUE != result)
            {
               // TODO: what else can we do?
               LOC_MSG_ERROR ("loc_pd_event_cb, failed processing session begin event", 0, 0, 0);
            }
         }// end if (loc_middleware_data.pdapi_client_id == pd_info_ptr->client_id) 
      }// end of recognised client 
   }// end of if (pd_event & PDSM_PD_EVENT_BEGIN)

   // gps session ends
   if (pd_event & PDSM_PD_EVENT_DONE)
   {
      result = TRUE; //will be explicity set to false in case of a failure

      // inform the client only if this was a MO fix
      if (pd_info_ptr->client_id == loc_middleware_data.pdapi_client_id )
      {
         cmd_ptr->cmd_type = LOC_CMD_TYPE_NOTIFY_CLIENT;
         cmd_ptr->client_handle = loc_client_get_active_client();
         cmd_ptr->ptr_data = NULL;
         ptr_nty_client_data = (loc_cmd_nty_client_s_type *)loc_calloc(sizeof(loc_cmd_nty_client_s_type));
         if(NULL != ptr_nty_client_data)
         {
            ptr_nty_client_data->event_type = LOC_EVENT_STATUS_REPORT;
            ptr_nty_client_data->event_data.status_report.event = LOC_STATUS_EVENT_FIX_SESSION_STATE;
            ptr_nty_client_data->event_data.status_report.payload.fix_session_state = LOC_FIX_SESSION_STATE_END;

            cmd_ptr->ptr_data = (void *)ptr_nty_client_data;
            if( TRUE != loc_middleware_queue_cmd(cmd_ptr))
            {
               // IPC failure, reclaim resources
               loc_free(ptr_nty_client_data);
               ptr_nty_client_data = NULL;
               cmd_ptr->ptr_data = NULL;
               result = FALSE;
            }
         }
         else
         {
            // memory allocation failure
            result = FALSE;
         }

         // positioning session is done, loc_pd_reset_module 
         // will send a request to update the scheduler
         loc_pd_reset_module ();
      }
      else if (pd_info_ptr->client_id == loc_middleware_data.ni_client_id)
      {
         // NI positioning session is done, reset the state
         /* NOTE: This will send a request to update the scheduler as well
         */
         loc_pd_reset_module ();
      }
      else
      {
         //got a session done event for an unknown client
         LOC_MSG_HIGH ("loc_pd_event_cb: dropping session done event , unknown client %d", 
                       pd_info_ptr->client_id, 0, 0);
         result = FALSE;
      }

      if(TRUE != result)
      {
         // TODO: what else can we do?
         LOC_MSG_ERROR ("loc_pd_event_cb, failed processing session done event", 0, 0, 0);
      }
   }

   // pd end event, record down the error code
   if (pd_event & PDSM_PD_EVENT_END)
   {
      if (pd_info_ptr->client_id == loc_middleware_data.pdapi_client_id)
      {
         LOC_MSG_ERROR ("loc_pd_event_cb, end status=%d\n", pd_info_ptr->pd_info.end_status, 0, 0);
         // update the position received time
         loc_middleware_data.pd_module_data.last_fix_ttf = 0;
         pd_module_data_ptr->pd_sess_end_status = pd_info_ptr->pd_info.end_status;
         pd_module_data_ptr->new_pd_sess_end_status_received = TRUE;
         loc_pd_queue_report_position_request ();
      }
   }

   // pd position event
   if((pd_event & PDSM_PD_EVENT_POSITION) ||
      (pd_event & PDSM_PD_EVENT_VELOCITY) ||
      (pd_event & PDSM_PD_EVENT_HEIGHT))
   {
      if (pd_info_ptr->client_id == loc_middleware_data.pdapi_client_id)
      {
         if (pd_info_ptr->pd_info.pd_data.position_type != PDSM_PD_POSITION_TYPE_INVALID)
         {
            // update the position received time
            time_get_uptime_ms (time_ms);
            pd_module_data_ptr->last_fix_arrival_time = QW_CVT_Q2N(time_ms);
            pd_module_data_ptr->last_fix_ttf = (dword) (pd_module_data_ptr->last_fix_arrival_time  - pd_module_data_ptr->last_get_position_time);

            memscpy (&pd_module_data_ptr->pd_info, sizeof(pd_module_data_ptr->pd_info), pd_info_ptr, sizeof (*pd_info_ptr));
            pd_module_data_ptr->new_pd_info_received = TRUE;

            loc_pd_queue_report_position_request ();
         }
      }
   }

   // wifi ap scan info injection needed. Ignore client id and send to all (NI/MO)
   if(pd_event & PDSM_PD_EVENT_REQUEST_WIFI_INFO_ON_DEMAND_INJECTION)
   {
      /* need to check for in_progress?? */

      LOC_MSG_MED ("loc_pd_event_cb PDSM_PD_EVENT_REQUEST_WIFI_INFO_ON_DEMAND_INJECTION", 0, 0, 0);

      /* calling proxy to post ipc to LocMW task */
      loc_pd_event_wifi_ap_scan_info_inject_ind_proxy();
   }

   // wifi ap scan info injection ack.
   if (pd_event & PDSM_PD_EVENT_ACK_WIFI_INFO_ON_DEMAND_INJECTION)
   {
      LOC_MSG_MED("loc_pd_event_cb PDSM_PD_EVENT_ACK_WIFI_INFO_ON_DEMAND_INJECTION", 0, 0, 0);

      /* calling proxy to post ipc to LocMW task */
      loc_pd_ack_wifi_ap_scan_info_inject_ind_proxy(pd_module_data_ptr->wifi_ap_scan_client_handle, pd_info_ptr->pd_info.wifi_inj_ack);

      /* reset progress flag*/
      pd_module_data_ptr->wifi_ap_scan_in_progress = FALSE;
   }

   // Forwarding the event to callback hook
   if (pd_module_data_ptr->pd_event_cb_hook != NULL)
   {
      pd_module_data_ptr->pd_event_cb_hook(user_data, pd_event, pd_info_ptr);
   }

   if (pd_info_ptr->client_id == loc_middleware_data.pdapi_client_id)
   {
      if ((pd_event & PDSM_PD_EVENT_COARSE_POS_INJ_DONE) ||
          (pd_event & PDSM_PD_EVENT_COARSE_POS_INJ_FAILED)||
          (pd_event & PDSM_PD_EVENT_COARSE_POS_INJ_ERROR))
      {
         if (pd_event & PDSM_PD_EVENT_COARSE_POS_INJ_DONE)
         {
            ioctl_status = LOC_API_SUCCESS;
         }
         else
         {
            ioctl_status = LOC_API_GENERAL_FAILURE;
         }

         (void) loc_client_queue_ioctl_callback (pd_module_data_ptr->pos_inject_client_handle ,
                                                 LOC_IOCTL_INJECT_POSITION,
                                                 ioctl_status,
                                                 NULL);
         pd_module_data_ptr->position_injection_in_progress = FALSE;
      }
   }
   else if (loc_middleware_data.ni_client_id == pd_info_ptr->client_id)
   {
      // event for NI client is also processed by us, so it's not counted toward id mismatch
   }
   else if(PDSM_PD_EVENT_WPS_NEEDED == pd_event)
   {
      // PDSM_PD_EVENT_WPS_NEEDED is sent to everyone with client id "-1"
   }
   else
   {
      LOC_MSG_HIGH ("loc_pd_event_cb client id mismatch for event %d", pd_event, 0, 0);
   }

   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
}

// PD cmd callback, if error is indicated, no further processing of this pd cmd will be performed.
static void loc_pd_cmd_cb (void                      *data_block_ptr,
                           pdsm_pd_cmd_e_type        pd_cmd,
                           pdsm_pd_cmd_err_e_type    pd_cmd_err_received)
{
   loc_pd_module_data_s_type *pd_module_data_ptr = &(loc_middleware_data.pd_module_data);

   LOC_MSG_MED ("loc_pd_cmd_cb: cmd=%d, err=%d\n", pd_cmd, pd_cmd_err_received, 0);

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
   switch (pd_cmd)
   {
   // Get postion Command
   case PDSM_PD_CMD_GET_POS:
      if (PDSM_PD_CMD_ERR_NOERR != pd_cmd_err_received)
      {
         pd_module_data_ptr->new_pd_cmd_err_received = TRUE;
         pd_module_data_ptr->pd_cmd_err = pd_cmd_err_received;

         // send error report, as an empty location report, to client
         loc_pd_queue_report_position_request ();

         loc_pd_reset_module ();
      }
      break;

   // End PD session
   case PDSM_PD_CMD_END_SESSION:
      break;

   case PDSM_PD_CMD_INJECT_POS:
      if (PDSM_PD_CMD_ERR_NOERR != pd_cmd_err_received)
      {
         (void) loc_client_queue_ioctl_callback (
                         pd_module_data_ptr->pos_inject_client_handle ,
                         LOC_IOCTL_INJECT_POSITION,
                         loc_pd_xlate_cmd_err_code_to_loc_status (pd_cmd_err_received),
                         NULL);

         pd_module_data_ptr->position_injection_in_progress = FALSE;
      }
      break;
  
   case PDSM_PD_CMD_GET_BEST_AVAIL_POS:
      if (PDSM_PD_CMD_ERR_NOERR != pd_cmd_err_received &&
          TRUE == pd_module_data_ptr->get_bap_in_progress)
      {
         loc_ioctl_callback_data_u_type *ioctl_callback_data_ptr = 
            (loc_ioctl_callback_data_u_type *) loc_calloc(sizeof(*ioctl_callback_data_ptr));

         LOC_MSG_ERROR ("loc_pd_cmd_cb: Failed to get Best Available position error = %d",
                           pd_cmd_err_received,0,0); 
         if(NULL != ioctl_callback_data_ptr)
         {
             qmiLocGetBestAvailablePositionIndMsgT_v02 *bap_ind_ptr = 
                &(ioctl_callback_data_ptr->bap_ind);
   
            memset(bap_ind_ptr, 0, sizeof(*bap_ind_ptr));
   
            /* copy the transaction id which was saved in the incoming call*/
            bap_ind_ptr->transactionId_valid = 1;
            bap_ind_ptr->transactionId = pd_module_data_ptr->get_bap_txn_id;
            
            (void) loc_client_queue_ioctl_callback (
               pd_module_data_ptr->bap_client_handle,
               LOC_IOCTL_GET_BEST_AVAILABLE_POSITION,
               loc_pd_xlate_cmd_err_code_to_loc_status (pd_cmd_err_received),
               ioctl_callback_data_ptr);

            loc_free(ioctl_callback_data_ptr);
         }
         else
         {
            LOC_MSG_ERROR ("loc_pd_cmd_cb: "
                           "Failed to allocate memory for ioctl callback data",
                           0,0,0); 
         }
         /*this code is executed inside critical section so no need enter the critical 
           section here. */
         pd_module_data_ptr->get_bap_in_progress = FALSE;
      }
      break; 

   case PDSM_PD_CMD_GET_AVAIL_WWAN_POS:
      if (PDSM_PD_CMD_ERR_NOERR != pd_cmd_err_received && 
          TRUE == pd_module_data_ptr->get_wwan_pos_in_progress)
      {
         loc_ioctl_callback_data_u_type *ioctl_callback_data_ptr = 
            (loc_ioctl_callback_data_u_type *) loc_calloc(sizeof(*ioctl_callback_data_ptr));

         LOC_MSG_ERROR("loc_pd_cmd_cb: Failed to get Available WWAN position error = %d", pd_cmd_err_received, 0, 0);

         if (NULL != ioctl_callback_data_ptr)
         {
            qmiLocGetAvailWwanPositionIndMsgT_v02 *wwan_pos_ind_ptr = &(ioctl_callback_data_ptr->z_GetAvailWwanPosInd);
   
            memset(wwan_pos_ind_ptr, 0, sizeof(*wwan_pos_ind_ptr));
   
            /* copy the transaction id which was saved in the incoming call */
            wwan_pos_ind_ptr->transactionId_valid = 1;
            wwan_pos_ind_ptr->transactionId = pd_module_data_ptr->get_wwan_pos_txn_id;
            
            (void) loc_client_queue_ioctl_callback (
               pd_module_data_ptr->wwan_pos_client_handle,
               LOC_IOCTL_GET_AVAILABLE_WWAN_POSITION,
               loc_pd_xlate_cmd_err_code_to_loc_status (pd_cmd_err_received),
               ioctl_callback_data_ptr);

            loc_free(ioctl_callback_data_ptr);
         }
         else
         {
            LOC_MSG_ERROR ("loc_pd_cmd_cb: "
                           "Failed to allocate memory for ioctl callback data",
                           0,0,0);
         }
         /* this code is executed inside critical section so no need enter the critical section here. */
         pd_module_data_ptr->get_wwan_pos_in_progress = FALSE;
      }
      break;

   /* wifi on demand - PDSM_PD_CMD_INJECT_WIFI_ON_DEMAND - will never be called as we are passing null func pointer
      in the pdsm call. Failure QMI RESP is sent when pdsm call return failure. */

    case PDSM_PD_CMD_GET_AVAIL_POS_FOR_SRC:
      if (PDSM_PD_CMD_ERR_NOERR != pd_cmd_err_received && 
          TRUE == pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_in_progress)
      {
         loc_ioctl_callback_data_u_type *ioctl_callback_data_ptr = 
            (loc_ioctl_callback_data_u_type *) loc_calloc(sizeof(*ioctl_callback_data_ptr));

         LOC_MSG_ERROR("loc_pd_cmd_cb: Failed to get Available position estimate.  error = %d", pd_cmd_err_received, 0, 0);

         if (NULL != ioctl_callback_data_ptr)
         {
            qmiLocGetAvailablePositionIndParamMsgT_v02 *pos_ind_ptr = &(ioctl_callback_data_ptr->z_GetAvailablePosInd);
   
            memset(pos_ind_ptr, 0, sizeof(*pos_ind_ptr));
   
            /* copy the transaction id which was saved in the incoming call */
            pos_ind_ptr->transactionId_valid = 1;
            pos_ind_ptr->transactionId = pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_txn_id;
            pos_ind_ptr->nonce_valid = pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_is_Nonce_valid ; 
            if(pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_is_Nonce_valid)
            {
              pos_ind_ptr->nonce = pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_Nonce;
            }
            pos_ind_ptr->get_secure_available_pos_data_mode = pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_data_mode;
            (void) loc_client_queue_ioctl_callback (
               pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_client_handle,
               LOC_IOCTL_GET_AVAILABLE_POSITION_FROM_SRC,
               loc_pd_xlate_cmd_err_code_to_loc_status (pd_cmd_err_received),
               ioctl_callback_data_ptr);

            loc_free(ioctl_callback_data_ptr);
         }
         else
         {
            LOC_MSG_ERROR ("loc_pd_cmd_cb: "
                           "Failed to allocate memory for ioctl callback data",
                           0,0,0);
         }
         /* this code is executed inside critical section so no need enter the critical section here. */
         pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_in_progress = FALSE;
      }
      break;
        // No such command or not supported
   default:
      break;
   }

   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
}

/** loc_pd_cmd_cb_unlocked :
 @details: The command callback called in PDSM context, for a 
          command was previously sent to PDAPI. The function
          should generate an IPC to queue the ioctl callback for
          the corresponding loc MW IOCTL command. Extracts  the
          loc MW client and ioctl information from the
          pd_cmd_data_ptr without any need for locking the loc
          MW critical section.
   
 @warning: Should be invoked only for commands that don't have a
           corresponding event_cb, PDAPI gives out the correct
           data_block_ptr in command callbacks, but not in the
           event_callbacks.
  
 @param [in] : pd_cmd_data_ptr: data pointer passed in during 
        command invocation
 @param [in]: pd_cmd: pdsm command types for which this callback 
        is called
 @param [in]: pd_cmd_err_received: command processing result
*/ 
 
static void loc_pd_cmd_cb_unlocked (
   void                      *pd_cmd_data_ptr,
   pdsm_pd_cmd_e_type        pd_cmd,
   pdsm_pd_cmd_err_e_type    pd_cmd_err_received)
{
   
   loc_pd_cmd_cb_data *loc_cmd_data_ptr =  (loc_pd_cmd_cb_data *)pd_cmd_data_ptr;
   int32 loc_error = 
      loc_pd_xlate_cmd_err_code_to_loc_status (pd_cmd_err_received);


   // check callback data pointer
   if (NULL == loc_cmd_data_ptr ||
       loc_cmd_data_ptr != loc_cmd_data_ptr->pMe)
   {
      LOC_MSG_ERROR("loc_pd_cmd_cb_unlocked: invalid callback data %p ",
                    loc_cmd_data_ptr, 0, 0);
      return;
   }

   // debug message
   LOC_MSG_MED ("loc_pd_cmd_cb_unlocked: ioctl =%d, pd_cmd = %d, err=%u\n, ", 
                loc_cmd_data_ptr->ioctl_type, 
                pd_cmd, 
                loc_error);

   
   switch (loc_cmd_data_ptr->ioctl_type)
   {
      /* cases where the ioctl callback does not carry any other
         payload apart from the status */
      case  LOC_IOCTL_INJECT_GSM_CELL_INFO_REQ :
      case  LOC_IOCTL_INJECT_NI_MESSAGE_REQ :
      case  LOC_IOCTL_WWAN_OOS_NOTIFICATION_REQ :
      {
         (void) loc_client_queue_ioctl_callback (
                            loc_cmd_data_ptr->client_handle,
                            loc_cmd_data_ptr->ioctl_type,
                            loc_error,
                            NULL);
         break;
      }


      /* cases where ioctl callback carries some payload,
      may be added in future*/

      // default: output an error
      default:
      {
         LOC_MSG_ERROR("loc_pd_cmd_cb_unlocked: unknown ioctl %d ",
                    loc_cmd_data_ptr->ioctl_type, 0, 0);
         break;
      }
   }

   loc_free(loc_cmd_data_ptr);

}

// Process extended measurement report with SV info
// TBD: process intermediate position report
// TBD: skip intermediate report after a good report has been received for this client
static void loc_pd_ext_event_cb (void   *data_block_ptr,
                                 pdsm_ext_status_event_type  ext_status_cmd,
                                 const pdsm_ext_status_info_s_type *ext_status_info_ptr)
{
   loc_pd_module_data_s_type* pd_module_data_ptr = &(loc_middleware_data.pd_module_data);

   if(NULL == ext_status_info_ptr)
   {
      LOC_MSG_ERROR ("loc_pd_ext_event_cb NULL status info ptr\n", 0, 0, 0);
      return;
   }

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
   // Check that the client id matches the client id registered for the service,
   // Otherwise we may be getting events associated with another client
   if (ext_status_info_ptr->client_id == loc_middleware_data.pdapi_client_id)
   {
      LOC_MSG_HIGH ("loc_pd_ext_event_cb cmd=0x%x", ext_status_cmd, 0, 0);

      if (PDSM_EXT_STATUS_EVENT_MEASUREMENT == ext_status_cmd)
      {
         const pdsm_pd_ext_meas_s_type * const pMeasurementReport = &ext_status_info_ptr->ext_status_info.ext_meas_report_type;
         if(PDSM_MEAS_PPM_RPT_VALID & (pMeasurementReport->flags))
         {
            // only PPM information is valid in this report, everything else should be ignored
            // since we do not support PPM info now, there is no need to handle this event at all
            LOC_MSG_HIGH ("loc_pd_ext_event_cb ignoring empty measurement report (%d)\n", pMeasurementReport->flags, 0, 0);
         }
         else
         {
            // this is a normal SV measurement report
            memscpy (&(pd_module_data_ptr->ext_status_info), sizeof(pd_module_data_ptr->ext_status_info),
                    ext_status_info_ptr,
                    sizeof (*ext_status_info_ptr));
            pd_module_data_ptr->new_ext_status_info_received = TRUE;
            loc_pd_queue_report_position_request ();
         }
      }

      if (PDSM_EXT_STATUS_EVENT_POS_REPORT == ext_status_cmd)
      {
         memscpy (&pd_module_data_ptr->interm_pos_info, sizeof(pd_module_data_ptr->interm_pos_info),
                  ext_status_info_ptr,
                  sizeof (*ext_status_info_ptr));
         pd_module_data_ptr->new_interm_pos_info_received = TRUE;
         loc_pd_queue_report_position_request();
      }

      if (PDSM_EXT_STATUS_EVENT_NMEA_POS_REPORT == ext_status_cmd)
      {
         if(pd_module_data_ptr->new_nmea_messages_counter >= NMEA_BUFFER_MAX_SIZE)
         {
           //Buffer overflow has occurred
           LOC_MSG_ERROR ( "Nmea Buffer overflow: drop statement", 
                           0, 0, 0);
           pd_module_data_ptr->new_nmea_messages_counter = NMEA_BUFFER_MAX_SIZE;/* cap the buffer size at this point*/
         }
         else
         {
            memscpy (&pd_module_data_ptr->nmea_info_buffer_in[pd_module_data_ptr->new_nmea_messages_counter],
            sizeof(pd_module_data_ptr->nmea_info_buffer_in[pd_module_data_ptr->new_nmea_messages_counter]),
                   ext_status_info_ptr,
                   sizeof (*ext_status_info_ptr));
            pd_module_data_ptr->new_nmea_messages_counter++;
            pd_module_data_ptr->new_nmea_info_received = TRUE;

            if(pd_module_data_ptr->new_nmea_messages_counter <= 1)
            {
               LOC_MSG_MED ( "Notify client for new NMEA statement", 0, 0, 0);
               loc_pd_queue_report_position_request();
            }
            else
            {
               // NMEA buffer is not empty, which means we must already have some notification request in the command queue
               LOC_MSG_MED ( "NMEA messages counter: %d", pd_module_data_ptr->new_nmea_messages_counter, 0, 0);
            }
         }
      }

      if ( (PDSM_EXT_STATUS_EVENT_BEST_AVAIL_POS_REPORT == ext_status_cmd)
          && (TRUE == pd_module_data_ptr->get_bap_in_progress) )
      {
         int32 ioctl_status;

         loc_ioctl_callback_data_u_type *ioctl_callback_data_ptr = 
            (loc_ioctl_callback_data_u_type *) loc_calloc(sizeof(*ioctl_callback_data_ptr));

         LOC_MSG_MED("loc_pd_ext_event_cb: received best available position\n", 
                     0, 0, 0 );

         if(NULL != ioctl_callback_data_ptr)
         {
            qmiLocGetBestAvailablePositionIndMsgT_v02 *bap_ind_ptr =
               &(ioctl_callback_data_ptr->bap_ind);  
              
            memset(bap_ind_ptr, 0, sizeof(*bap_ind_ptr));

            if (convertLocEngPosToBapPos( &(ext_status_info_ptr->ext_status_info.ext_pos_report_qmi_type),
                                          bap_ind_ptr))
            {
               ioctl_status = LOC_API_SUCCESS;
            }
            else
            {
               ioctl_status = LOC_API_GENERAL_FAILURE;
            }
            
            /* copy the transaction id which was saved in the incoming call*/
            bap_ind_ptr->transactionId_valid = 1;
            bap_ind_ptr->transactionId = pd_module_data_ptr->get_bap_txn_id;
            
            (void) loc_client_queue_ioctl_callback (
               pd_module_data_ptr->bap_client_handle ,
               LOC_IOCTL_GET_BEST_AVAILABLE_POSITION,
               ioctl_status,
               ioctl_callback_data_ptr);

           loc_free(ioctl_callback_data_ptr);
         }
         else
         {
            LOC_MSG_ERROR(
               "loc_pd_ext_event_cb: Failed to allocate memory for ioctl data\n", 
               0, 0, 0 );
         }
         pd_module_data_ptr->get_bap_in_progress = FALSE;
      }

      if (PDSM_EXT_STATUS_EVENT_GNSS_MEASUREMENT == ext_status_cmd)
      {

         LOC_MSG_HIGH ("loc_pd_ext_event_cb PDSM_EXT_STATUS_EVENT_GNSS_MEASUREMENT",0, 0, 0);

         /*
         LOC_MSG_MED ("loc_pd_ext_event_cb measurement report: sv count: %d, alm mask: 0x%X, eph mask: 0x%X\n", 
                       pMeasurementReport->num_svs, pMeasurementReport->alm_svmask, pMeasurementReport->eph_svmask);
         */

         // this is a normal SV measurement report
         memscpy (&(pd_module_data_ptr->gnss_meas_info),
                 sizeof(pd_module_data_ptr->gnss_meas_info),
                 ext_status_info_ptr,
                 sizeof (pdsm_ext_status_info_s_type));
         pd_module_data_ptr->new_gnss_meas_info_received = TRUE;
         loc_pd_notify_gnss_measurements ();
      }

      if (PDSM_EXT_STATUS_EVENT_SV_POLY == ext_status_cmd)
      {
         LOC_MSG_HIGH ("loc_pd_ext_event_cb PDSM_EXT_STATUS_EVENT_SV_POLY",0, 0, 0);

         // this is a  SV poly report
         memscpy (&(pd_module_data_ptr->sv_poly_info),
                 sizeof(pd_module_data_ptr->sv_poly_info),
                 ext_status_info_ptr,
                 sizeof (pdsm_ext_status_info_s_type));
         pd_module_data_ptr->new_sv_poly_info_received = TRUE;
         loc_pd_notify_sv_polynomials();
      }

      if( (PDSM_EXT_STATUS_EVENT_AVAIL_WWAN_POS_REPORT == ext_status_cmd) &&
          (TRUE == pd_module_data_ptr->get_wwan_pos_in_progress) )
      {
         int32 ioctl_status;

         loc_ioctl_callback_data_u_type *ioctl_callback_data_ptr = 
           (loc_ioctl_callback_data_u_type *) loc_calloc(sizeof(*ioctl_callback_data_ptr));

         LOC_MSG_MED("loc_pd_ext_event_cb: received available WWAN position", 0, 0, 0 );

         if (NULL != ioctl_callback_data_ptr)
         {
            qmiLocGetAvailWwanPositionIndMsgT_v02 *wwan_pos_ind_ptr = &(ioctl_callback_data_ptr->z_GetAvailWwanPosInd);

            memset(wwan_pos_ind_ptr, 0, sizeof(*wwan_pos_ind_ptr));

            if (convertLocEngPosToAvailWwanPos( &(ext_status_info_ptr->ext_status_info.ext_pos_report_qmi_type),
                                              wwan_pos_ind_ptr))
            {
               ioctl_status = LOC_API_SUCCESS;
            }
            else
            {
               ioctl_status = LOC_API_GENERAL_FAILURE;
            }

            /* copy the transaction id which was saved in the incoming call */
            wwan_pos_ind_ptr->transactionId_valid = 1;
            wwan_pos_ind_ptr->transactionId = pd_module_data_ptr->get_wwan_pos_txn_id;

            (void) loc_client_queue_ioctl_callback( pd_module_data_ptr->wwan_pos_client_handle,
                                                  LOC_IOCTL_GET_AVAILABLE_WWAN_POSITION,
                                                  ioctl_status,
                                                  ioctl_callback_data_ptr);

            loc_free(ioctl_callback_data_ptr);
         }
         else
         {
            LOC_MSG_ERROR("loc_pd_ext_event_cb: Failed to allocate memory for ioctl data\n", 0, 0, 0);
         }
         pd_module_data_ptr->get_wwan_pos_in_progress = FALSE;
      }

      if( (PDSM_EXT_STATUS_EVENT_AVAIL_POS_REPORT == ext_status_cmd) &&
          (TRUE == pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_in_progress) )
      {
         int32 ioctl_status;

         loc_ioctl_callback_data_u_type *ioctl_callback_data_ptr = 
          (loc_ioctl_callback_data_u_type *) loc_calloc(sizeof(*ioctl_callback_data_ptr));

         LOC_MSG_MED("loc_pd_ext_event_cb: received available position Estimate", 0, 0, 0 );
         if (NULL != ioctl_callback_data_ptr)
         {
            qmiLocGetAvailablePositionIndParamMsgT_v02 *pos_ind_ptr = &(ioctl_callback_data_ptr->z_GetAvailablePosInd);

            memset(pos_ind_ptr, 0, sizeof(*pos_ind_ptr));

            if (convertLocEngPosToAvailablePosition( &(ext_status_info_ptr->ext_status_info.ext_pos_report_qmi_type),pos_ind_ptr))
            {
               ioctl_status = LOC_API_SUCCESS;
            }
            else
            {
               ioctl_status = LOC_API_GENERAL_FAILURE;
            }

            /* copy the transaction id which was saved in the incoming call */
            pos_ind_ptr->transactionId_valid = 1;
            pos_ind_ptr->transactionId = pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_txn_id;

            pos_ind_ptr->nonce_valid = pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_is_Nonce_valid ; 
            if(pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_is_Nonce_valid)
            {
               pos_ind_ptr->nonce = pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_Nonce;
            }
            pos_ind_ptr->get_secure_available_pos_data_mode = pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_data_mode;	
            //LOC_MSG_MED("loc_pd_ext_event_cb: Transaction ID %d Nonce %x", pos_ind_ptr->transactionId, pos_ind_ptr->nonce, 0 );
            LOC_MSG_MED("loc_pd_ext_event_cb: Status %d ", ioctl_status, 0, 0 );
            //LOC_MSG_MED("loc_pd_ext_event_cb: HorizontalConfidence_valid %d  %d", pos_ind_ptr->horCircularConfidence_valid, pos_ind_ptr->horCircularConfidence, 0 );	

            (void) loc_client_queue_ioctl_callback( pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_client_handle,
                                                  LOC_IOCTL_GET_AVAILABLE_POSITION_FROM_SRC,
                                                  ioctl_status,
                                                  ioctl_callback_data_ptr);

            loc_free(ioctl_callback_data_ptr);
         }
         else
         {
            LOC_MSG_ERROR("loc_pd_ext_event_cb: Failed to allocate memory for ioctl data\n", 0, 0, 0);
         }
         pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_in_progress = FALSE;
      }
   }

   /* No need to filter this Position Injection Request, As this can be an unsolicited request sent to 
     any client asking it to inject coarse Position */  
   if (PDSM_EXT_STATUS_EVENT_POS_INJECT_REQ == ext_status_cmd)
   {
      loc_pos_inj_request_s_type request = {0};
      const pdsm_pd_ext_pos_inj_req_s_type * const pPosInjReq = 
          &ext_status_info_ptr->ext_status_info.ext_pos_inj_req_type;
      LOC_MSG_MED ( "position injection request (1): flag 0x%X, lat: %d, lon: %d", pPosInjReq->flags, pPosInjReq->lat, pPosInjReq->lon);
      LOC_MSG_MED ( "position injection request (2): Punc %u, week: %u, ms: %u", pPosInjReq->pos_unc, pPosInjReq->timestamp_gps_week, pPosInjReq->timestamp_tow_ms);

      // TODO: interpretation of the fields can wait. for first release, a signal would be sufficient

      request.flags = 0;
      request.latitude = 0;
      request.longitude = 0;
      request.position_uncertainty = 0;
      request.timestamp = 0;

      loc_client_queue_pos_inj_request(&request);
   }

   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
}

// This function is called to reset the position determination module.
// This funciton will be called when:
// (1) pdsm_get_position returns false
// (2) pd cmd callback reports an error
// (3) PDSM_PD_EVENT_DONE is received
// (4) timer expires waiting for PDSM_PD_EVENT_DONE
static void loc_pd_reset_module (void)
{
   loc_cmd_s_type           cmd_data;
   loc_cmd_s_type           *cmd_ptr = &cmd_data;
   loc_pd_module_data_s_type *pd_module_data_ptr = &(loc_middleware_data.pd_module_data);

   LOC_MSG_HIGH ("loc_pd_reset_module \n", 0, 0, 0);

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
   pd_module_data_ptr->fix_in_progress = FALSE;
   pd_module_data_ptr->mo_fix_in_progress = FALSE;
   pd_module_data_ptr->supl_mt_fix_state = LOC_SUPL_MT_IDLE;
   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

   // Queue the cmd to start another round of fix processing
   memset (cmd_ptr, 0, sizeof (loc_cmd_s_type));
   cmd_ptr->cmd_type = LOC_CMD_TYPE_PD_SCHEDULE;
   cmd_ptr->ptr_data = NULL;
   // not much we can do if it fails. also no memory is allocated, so no resource to reclaim
   (void) loc_middleware_queue_cmd (cmd_ptr);
}

// This function is called to request next position and set up associated parameters.
static void loc_pd_queue_report_position_request (void)
{
   loc_cmd_s_type             cmd_data;
   loc_cmd_s_type*            cmd_ptr = &cmd_data;

   memset (cmd_ptr, 0, sizeof (loc_cmd_s_type));
   cmd_ptr->cmd_type = LOC_CMD_TYPE_REPORT_POSITION;
   cmd_ptr->ptr_data = NULL;
   // TODO: examine our options if this operation fails
   // no memory is allocated, so no resource to reclaim
   (void) loc_middleware_queue_cmd (cmd_ptr);
}

// This function is called to notify gnss measurement reports.
static void loc_pd_notify_gnss_measurements (void)
{
   loc_client_handle_type      active_client_handle;
   loc_parsed_position_s_type* parsed_report_ptr;
   loc_gnss_meas_s_type*       p_gnss_report_ptr;
   loc_pd_module_data_s_type*  pd_module_data_ptr;

   pd_module_data_ptr = &(loc_middleware_data.pd_module_data);
   p_gnss_report_ptr = &(pd_module_data_ptr->gnss_meas_payload.gnss_meas_report);

   LOC_MSG_MED ("loc_pd_notify_gnss_measurements \n", 0, 0, 0);
   // the payloads will be memset in loc_pd_get_report
   loc_pd_get_gnss_meas_report (p_gnss_report_ptr);

    memscpy(&(pd_module_data_ptr->gnss_meas_payload.gnss_meas_report),
            sizeof(pd_module_data_ptr->gnss_meas_payload.gnss_meas_report),
            p_gnss_report_ptr, 
            sizeof (pd_module_data_ptr->gnss_meas_payload.gnss_meas_report));

   active_client_handle = loc_client_get_active_client ();

 

   // Set the gnss report first (the one being populated from final position report), thent the final position report, so the client can
   if ((active_client_handle != LOC_CLIENT_HANDLE_INVALID))
   {
       LOC_MSG_MED ("loc_pd_notify_gnss_measurements invoke callback \n", 0, 0, 0);
      (void) loc_client_invoke_callback (active_client_handle, LOC_EVENT_GNSS_MEASUREMENT_REPORT, &(pd_module_data_ptr->gnss_meas_payload));
   }

 //  return TRUE;
}

static void loc_pd_notify_sv_polynomials (void)
{
   loc_client_handle_type      active_client_handle;
   loc_sv_poly_s_type*       p_sv_poly_ptr;
   loc_pd_module_data_s_type*  pd_module_data_ptr;

   pd_module_data_ptr = &(loc_middleware_data.pd_module_data);
   p_sv_poly_ptr = &(pd_module_data_ptr->sv_poly_payload.sv_poly_report);

   LOC_MSG_HIGH ("loc_pd_notify_sv_polynomials \n", 0, 0, 0);
   // the payloads will be memset in loc_pd_get_report
   loc_pd_sv_poly_report (p_sv_poly_ptr);

    memscpy(&(pd_module_data_ptr->sv_poly_payload.sv_poly_report), 
            sizeof(pd_module_data_ptr->sv_poly_payload.sv_poly_report),
            p_sv_poly_ptr, 
            sizeof (pd_module_data_ptr->sv_poly_payload.sv_poly_report));

   active_client_handle = loc_client_get_active_client ();

 

   // Set the gnss report first (the one being populated from final position report), thent the final position report, so the client can
   if ((active_client_handle != LOC_CLIENT_HANDLE_INVALID))
   {
       LOC_MSG_HIGH ("loc_pd_notify_sv_polynomials invoke callback \n", 0, 0, 0);
      (void) loc_client_invoke_callback (active_client_handle, LOC_EVENT_SV_POLY_REPORT, &(pd_module_data_ptr->sv_poly_payload));
   }

 //  return TRUE;
}

// Calculate GEO ID based on latitude and longitude
static float loc_pd_get_geoidal_separation (double latitude, double longitude)
{
   double x1, y1, dZ0, dZ1, dZ2, dZ3, dXn, dYn, a0, a1, a2, a3;
   int i, j, iGH;

   // Interpolates between table entries
   // First gather up the table entries surrounding the position
   if (latitude < -90.0 || latitude > 90.0) return 0.0;
   if (longitude < -180.0 || longitude > 180.0) return 0.0;
   i = ((int)latitude / 10);
   x1 = i * 10;
   i = 9 - i;
   if (i >= 19 || i < 0) i = 0;
   j = ((int)longitude / 10);
   y1 = j * 10;
   j += 18;
   if (j >= 36 || j < 0) j = 0;
      dZ0 = GeoSep[i][j];
   if (--i < 0) i = 0;
      dZ3 = GeoSep[i][j];
   if (++j >= 36) j = 0;
      dZ2 = GeoSep[i][j];
   if (++i >= 19) i = 0;
      dZ1 = GeoSep[i][j];

   // This algorithm was taken from CLSInterpolator::Bilinear
   // Step 1. calculate dXn, and dYn which are non-dimensional (normalized quantities)
   dYn = (longitude - y1) / 10.0;
   dXn = (latitude - x1) / 10.0;

   // Step 2. compute the coefficients of the polynomial
   a0 = dZ0;                          //  SW corner
   a1 = dZ1 - dZ0;                    //  difference along X axis
   a2 = dZ3 - dZ0;                    //  difference along Y axis
   a3 = dZ0 + dZ2 - dZ1 - dZ3;  //  cross - derivative

   // Step 3. evaluate the polynomial and round to neareast meter
   iGH = (int)((a0 + a1 * dYn + a2 * dXn + a3 * dXn * dYn) + 0.5);

   return (float)iGH;
}

// Translate fix end reason into session status in the loc api position report
static loc_session_status_e_type loc_pd_xlate_fix_end_reason_code (pdsm_pd_end_e_type pd_sess_end_status)
{
   loc_session_status_e_type session_status;

   switch(pd_sess_end_status)
   {
   case PDSM_PD_END_SESS_NONE:
      session_status = LOC_SESS_STATUS_SUCCESS;
      break;

   case PDSM_PD_END_TIMEOUT:
      session_status = LOC_SESS_STATUS_TIMEOUT;
      break;

   case PDSM_PD_END_CLIENT_END:
      session_status = LOC_SESS_STATUS_USER_END;
      break;

   default:
      session_status = LOC_SESS_STATUS_GENERAL_FAILURE;
      break;
   }

   LOC_MSG_ERROR ("loc_pd_xlate_fix_end_reason_code, cmd err = %d, session status = %d", pd_sess_end_status, session_status ,0);

   return session_status;
}

// Translate PD cmd error into session status in the loc api position report
static loc_session_status_e_type loc_pd_xlate_cmd_err_code_to_sess_status  (pdsm_pd_cmd_err_e_type pd_cmd_err)
{
   loc_session_status_e_type session_status;

   switch(pd_cmd_err)
   {
   case PDSM_PD_CMD_ERR_SESS_TYPE_P:
   case PDSM_PD_CMD_ERR_OPERATION_P:
   case PDSM_PD_CMD_ERR_NUM_FIXES_P:
   case PDSM_PD_CMD_ERR_LSINFO_P:
   case PDSM_PD_CMD_ERR_TIMEOUT_P:
   case PDSM_PD_CMD_ERR_QOS_P:
      session_status = LOC_SESS_STATUS_BAD_PARAMETER;
        break;

   case PDSM_PD_CMD_ERR_OFFLINE_S:
      session_status = LOC_SESS_STATUS_PHONE_OFFLINE;
      break;

   case PDSM_PD_CMD_ERR_GPS_LOCK_S:
      session_status = LOC_SESS_STATUS_ENGINE_LOCKED;
      break;

   default:
      session_status = LOC_SESS_STATUS_GENERAL_FAILURE;
      break;
   }

   LOC_MSG_ERROR ("loc_pd_xlate_cmd_err_code_to_sess_status, cmd err = %d, session status = %d", pd_cmd_err, session_status ,0);
   return session_status;
}

// Translate PD cmd error into session status in the loc api position report
static uint32 loc_pd_xlate_cmd_err_code_to_loc_status  (pdsm_pd_cmd_err_e_type pd_cmd_err)
{
   switch(pd_cmd_err)
   {
   case PDSM_PD_CMD_ERR_NOERR:
        return LOC_API_SUCCESS;
   case PDSM_PD_CMD_ERR_SESS_TYPE_P:
   case PDSM_PD_CMD_ERR_OPERATION_P:
   case PDSM_PD_CMD_ERR_NUM_FIXES_P:
   case PDSM_PD_CMD_ERR_LSINFO_P:
   case PDSM_PD_CMD_ERR_TIMEOUT_P:
   case PDSM_PD_CMD_ERR_QOS_P:
        return LOC_API_INVALID_PARAMETER;

   case PDSM_PD_CMD_ERR_OFFLINE_S:
        return LOC_API_PHONE_OFFLINE;

   case PDSM_PD_CMD_ERR_GPS_LOCK_S:
        return LOC_API_UNSUPPORTED;

   default:
        return LOC_API_GENERAL_FAILURE;
   }
}

// Translate external coarse position from Loc api to PDAPI format
static int loc_pd_xlate_external_pos_from_locapi_to_pdapi (const loc_assist_data_pos_s_type*  loc_ext_pos_ptr,
                                                           pdsm_pd_external_position_info_type* pdsm_external_pos_ptr)
{
  int ret_val = LOC_API_SUCCESS;

  // pdsm_external_pos_ptr memset'ed in caller function
  
  do
  {
    pdsm_external_pos_ptr->opt_field_mask = 0;

    /* TIMESTAMP_UTC and TIMESTAMP_AGE are exclusive, honor TIMESTAMP_UTC first */ 
    if (loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_TIMESTAMP_UTC)
    {
      pdsm_external_pos_ptr->opt_field_mask |= PDSM_PD_EXTERN_COARSE_POS_GPS_TIME_VALID;
      pdsm_external_pos_ptr->time_info_type = PDSM_PD_EXTERN_COARSE_POS_TIME_UTC;
      // loc_ext_pos_ptr->pos_info.timestamp_utc: in milli-seconds, while pdsm_external_pos_ptr->timestamp_sec in seconds
      pdsm_external_pos_ptr->timestamp_sec = (loc_ext_pos_ptr->timestamp_utc + 500) / 1000 - GPS_TO_UTC_DELTA_SECONDS;
    }
    else if (loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_TIMESTAMP_AGE)
    {
      pdsm_external_pos_ptr->opt_field_mask |= PDSM_PD_EXTERN_COARSE_POS_GPS_TIME_VALID;
      pdsm_external_pos_ptr->time_info_type = PDSM_PD_EXTERN_COARSE_POS_TIME_AGE;
      pdsm_external_pos_ptr->timestamp_sec = loc_ext_pos_ptr->timestamp_age / 1000; /* convert from ms to sec */
    }

    if ((loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_LATITUDE) &&
        (loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_LONGITUDE) &&
        (loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_HOR_UNC_CIRCULAR))
    {
      pdsm_external_pos_ptr->opt_field_mask |= PDSM_PD_EXTERN_COARSE_POS_LAT_LONG_VALID;
      pdsm_external_pos_ptr->latitude  = loc_ext_pos_ptr->latitude;
      pdsm_external_pos_ptr->longitude = loc_ext_pos_ptr->longitude;

      pdsm_external_pos_ptr->opt_field_mask |= PDSM_PD_EXTERN_COARSE_POS_CIRCULAR_UNC_VALID;
      pdsm_external_pos_ptr->loc_unc_hor    = loc_ext_pos_ptr->hor_unc_circular;

      pdsm_external_pos_ptr->opt_field_mask |= PDSM_PD_EXTERN_COARSE_POS_HORIZ_CONF_VALID;
      if (loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_CONFIDENCE_HORIZONTAL)
      {
        pdsm_external_pos_ptr->confidence_hor = loc_ext_pos_ptr->confidence_horizontal;
      }
      else
      {
        // If not set, use default value 50
        pdsm_external_pos_ptr->confidence_hor = 50;
      }

      if(loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_RAW_HOR_UNC_CIRCULAR )
      {
        pdsm_external_pos_ptr->opt_field_mask |= PDSM_PD_EXTERN_COARSE_POS_RAW_CIRCULAR_UNC_VALID;
        pdsm_external_pos_ptr->loc_unc_hor_raw    = loc_ext_pos_ptr->raw_hor_unc_circular;

        if(loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_RAW_CONFIDENCE_HORIZONTAL )
        {
          pdsm_external_pos_ptr->confidence_hor_raw  = loc_ext_pos_ptr->raw_confidence_horizontal; 
          pdsm_external_pos_ptr->opt_field_mask |= PDSM_PD_EXTERN_COARSE_POS_RAW_HORIZ_CONF_VALID;
        }
        else
        {            
          // If not set, use default value 50
          pdsm_external_pos_ptr->confidence_hor_raw = 50;
        }
      }

    }
    else if ((loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_LATITUDE) ||
             (loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_LONGITUDE) ||
             (loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_HOR_UNC_CIRCULAR) ||
             (loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_CONFIDENCE_HORIZONTAL))
    {
      LOC_MSG_ERROR("position injection, invalid latitude and longitude combo", 0, 0, 0);
      LOC_MSG_ERROR("please provide valid latitude longitude, with unc circular and confidence horizontal", 0, 0, 0);
      ret_val = LOC_API_INVALID_PARAMETER;
      break;
    }

    if (((loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_ALTITUDE_WRT_ELLIPSOID) ||
         (loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_ALTITUDE_WRT_MEAN_SEA_LEVEL)) &&
        (loc_ext_pos_ptr->valid_mask  & LOC_ASSIST_POS_VALID_VERT_UNC))
    {
      pdsm_external_pos_ptr->opt_field_mask |= PDSM_PD_EXTERN_COARSE_POS_ALT_VALID;
      if (loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_ALTITUDE_WRT_ELLIPSOID)
      {
        pdsm_external_pos_ptr->altitude = loc_ext_pos_ptr->altitude_wrt_ellipsoid;
        pdsm_external_pos_ptr->alt_def = PDSM_PD_EXTERN_COARSE_POS_ALT_HAE;
      }
      // loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_ALTITUDE_WRT_MEAN_SEA_LEVEL
      else
      {
        pdsm_external_pos_ptr->altitude = loc_ext_pos_ptr->altitude_wrt_mean_sea_level;
        pdsm_external_pos_ptr->alt_def = PDSM_PD_EXTERN_COARSE_POS_ALT_MSL;
      }

      // no mask
      pdsm_external_pos_ptr->loc_unc_vert = loc_ext_pos_ptr->vert_unc;

      // no mask
      if (loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_CONFIDENCE_VERTICAL)
      {
        pdsm_external_pos_ptr->confidence_vert = loc_ext_pos_ptr->confidence_vertical;
      }
      else
      {
        // If not set, use default value 50
        pdsm_external_pos_ptr->confidence_vert = 50;
      }
    }
    else if ((loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_ALTITUDE_WRT_ELLIPSOID) ||
             (loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_ALTITUDE_WRT_MEAN_SEA_LEVEL) ||
             (loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_VERT_UNC) ||
             (loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_CONFIDENCE_VERTICAL))
    {

      LOC_MSG_ERROR("position injection, invalid altitude ", 0, 0, 0);
      LOC_MSG_ERROR("please provide valid altitude, with ver uncertainty and confidence vertical", 0, 0, 0);
      ret_val = LOC_API_INVALID_PARAMETER;
      break;
    }

    // no mask
    if (loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_H_RELIABILITY_IND)
    {
      pdsm_external_pos_ptr->e_HoriRelIndicator = loc_ext_pos_ptr->hor_reliability;
    }
    else
    {
      pdsm_external_pos_ptr->e_HoriRelIndicator = PDSM_POSITION_RELIABILITY_NOT_SET;
    }

    // no mask
    if (loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_V_RELIABILITY_IND)
    {
      pdsm_external_pos_ptr->e_VertRelIndicator = loc_ext_pos_ptr->vert_reliability;
    }
    else
    {
      pdsm_external_pos_ptr->e_VertRelIndicator = PDSM_POSITION_RELIABILITY_NOT_SET;
    }

    // no mask
    if (loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_ALTITUDE_SOURCE)
    {
      pdsm_external_pos_ptr->e_AltitudeSource = loc_ext_pos_ptr->altitude_source;
    }
    else
    {
      pdsm_external_pos_ptr->e_AltitudeSource = PDSM_ALTITUDE_SOURCE_UNKNOWN;
    }

    // no mask
    if (loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_ALT_H_LINKAGE)
    {
      pdsm_external_pos_ptr->e_AltHorLinkage = loc_ext_pos_ptr->altitude_hor_linkage;
    }
    else
    {
      pdsm_external_pos_ptr->e_AltHorLinkage = PDSM_ALT_HOR_LINK_NOT_SPECIFIED;
    }

    // no mask
    if (loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_ALT_UNC_COVERAGE)
    {
      pdsm_external_pos_ptr->e_AltUncCoverage = loc_ext_pos_ptr->altitude_unc_coverage;
    }
    else
    {
      pdsm_external_pos_ptr->e_AltUncCoverage = PDSM_ALT_COVERAGE_NOT_SPECIFIED;
    }

    // no mask
    if (loc_ext_pos_ptr->valid_mask & LOC_ASSIST_POS_VALID_POSITION_SRC)
    {
      pdsm_external_pos_ptr->source = loc_ext_pos_ptr->position_source;
    }
    else
    {
      pdsm_external_pos_ptr->source = PDSM_PD_EXTERN_COARSE_POS_SOURCE_UNKNOWN;
    }

    // Horizontal Elliptical Unc not supported by API
  }
  while (0);

  LOC_MSG_MED("position injection, result %d", ret_val, 0, 0);

  return ret_val; 
}

// Debug message to dump out position report
static void loc_pd_dump_position_report (loc_parsed_position_s_type* parsed_report_ptr)
{
   LOC_MSG_MED ("parsed report: mask = 0x%x, status = 0x%x",
                parsed_report_ptr->valid_mask, parsed_report_ptr->session_status, 0);

   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_SESSION_STATUS)
   {
      LOC_MSG_MED ("parsed report: status = 0x%x", parsed_report_ptr->session_status, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_LATITUDE)
   {
      LOC_MSG_MED ("parsed report: lat = %d,", (int)parsed_report_ptr->latitude, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_LONGITUDE)
   {
      LOC_MSG_MED ("parsed report: lon = %d", (int)parsed_report_ptr->longitude, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_ALTITUDE_WRT_ELLIPSOID)
   {
      LOC_MSG_MED ("parsed report: alt wrt ellipsoid = %d", (int)parsed_report_ptr->altitude_wrt_ellipsoid, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_TIMESTAMP_CALENDAR)
   {
      LOC_MSG_MED ("parsed report: timestamp_calendar year = %d, month = %d, day = %d",
                    parsed_report_ptr->timestamp_calendar.year,
                    parsed_report_ptr->timestamp_calendar.month,
                    parsed_report_ptr->timestamp_calendar.day);
      LOC_MSG_MED ("parsed report: timestamp_calendar hour = %d, min = %d, sec = %d",
                    parsed_report_ptr->timestamp_calendar.hour,
                    parsed_report_ptr->timestamp_calendar.minute,
                    parsed_report_ptr->timestamp_calendar.second);
      LOC_MSG_MED ("parsed report: timestamp_calendar msec = %d, day of week = %d",
                    parsed_report_ptr->timestamp_calendar.millisecond,
                    parsed_report_ptr->timestamp_calendar.day_of_week, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_TIMESTAMP_UTC)
   {
      LOC_MSG_MED ("parsed report: timestamp_utc = 0x%x%08x", (parsed_report_ptr->timestamp_utc>>32), (parsed_report_ptr->timestamp_utc&0xFFFFFFFF), 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_LEAP_SECONDS)
   {
      LOC_MSG_MED ("parsed report: leap_seconds = %d", (int)parsed_report_ptr->leap_seconds, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_TIME_UNC)
   {
      LOC_MSG_MED ("parsed report: time_unc = %d", (int)parsed_report_ptr->time_unc, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_ALTITUDE_WRT_MEAN_SEA_LEVEL)
   {
      LOC_MSG_MED ("parsed report: alt wrt sea level = %d", (int)parsed_report_ptr->altitude_wrt_mean_sea_level, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_SPEED_HORIZONTAL)
   {
      LOC_MSG_MED ("parsed report: speed_horizontal = %d", (int)parsed_report_ptr->speed_horizontal, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_SPEED_VERTICAL)
   {
      LOC_MSG_MED ("parsed report: speed_vertical = %d", (int)parsed_report_ptr->speed_vertical, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_HEADING)
   {
      LOC_MSG_MED ("parsed report: heading = %d", (int)parsed_report_ptr->heading, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_HOR_UNC_CIRCULAR)
   {
      LOC_MSG_MED ("parsed report: hor_unc_circular = %d", (int)parsed_report_ptr->hor_unc_circular, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_HOR_UNC_ELLI_SEMI_MAJ)
   {
      LOC_MSG_MED ("parsed report: hor_unc_ellipse_semi_major = %d", (int)parsed_report_ptr->hor_unc_ellipse_semi_major, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_HOR_UNC_ELLI_SEMI_MIN)
   {
      LOC_MSG_MED ("parsed report: hor_unc_ellipse_semi_minor = %d", (int)parsed_report_ptr->hor_unc_ellipse_semi_minor, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_HOR_UNC_ELLI_ORIENT_AZIMUTH)
   {
      LOC_MSG_MED ("parsed report: hor_unc_ellipse_orient_azimuth = %d", (int)parsed_report_ptr->hor_unc_ellipse_orient_azimuth, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_VERTICAL_UNC)
   {
      LOC_MSG_MED ("parsed report: vert_unc = %d", (int)parsed_report_ptr->vert_unc, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_SPEED_UNC)
   {
      LOC_MSG_MED ("parsed report: speed_unc = %d", (int)parsed_report_ptr->speed_unc, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_HEADING_UNC)
   {
      LOC_MSG_MED ("parsed report: heading_unc = %d", (int)parsed_report_ptr->heading_unc, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_CONFIDENCE_HORIZONTAL)
   {
      LOC_MSG_MED ("parsed report: confidence_horizontal = %d", (int)parsed_report_ptr->confidence_horizontal, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_CONFIDENCE_VERTICAL)
   {
      LOC_MSG_MED ("parsed report: confidence_vertical = %d", (int)parsed_report_ptr->confidence_vertical, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_MAGNETIC_VARIATION)
   {
      LOC_MSG_MED ("parsed report: magnetic_deviation = %d", (int)parsed_report_ptr->magnetic_deviation, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_TECHNOLOGY_MASK)
   {
      LOC_MSG_MED ("parsed report: technology_mask = 0x%x", parsed_report_ptr->technology_mask, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_SENSOR_AIDING_IND)
   {
      LOC_MSG_MED ("parsed report: sensor aiding indicator = 0x%x",
                    parsed_report_ptr->sensor_aiding_indicator, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_SENSOR_DATA_USAGE)
   {
      LOC_MSG_MED ("parsed report: sensor data usage mask = 0x%x",
                    parsed_report_ptr->sensor_data_usage_mask, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_H_RELIABILITY_IND)
   {
      LOC_MSG_MED ("parsed report: horizontal reliability indicator = %u",
                    parsed_report_ptr->hor_reliability, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_V_RELIABILITY_IND)
   {
      LOC_MSG_MED ("parsed report: vertical reliability indicator = %u",
                    parsed_report_ptr->vert_reliability, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_TIME_SOURCE)
   {
      LOC_MSG_MED ("parsed report: time source mask = 0x%x",
                    parsed_report_ptr->time_soruce, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_EXT_TIME_SOURCE)
   {
      LOC_MSG_MED ("parsed report: extended time source = %u",
                    parsed_report_ptr->extended_time_source, 0, 0);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_DOP)
   {
      LOC_MSG_MED ("parsed report: dop = (P)%d, (H)%d, (V)%d",
                    (int)parsed_report_ptr->position_dop,
                    (int)parsed_report_ptr->hor_dop,
                    (int)parsed_report_ptr->vert_dop);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_SV_USED_MASK)
   {
      LOC_MSG_MED ("parsed report: gps_sv_used_mask=0x%x, glo_sv_used_mask=0x%x, bds_sv_used_mask=0x%llx",
                    parsed_report_ptr->z_NavSvInfo.q_GpsSvMaskUsed,
                    parsed_report_ptr->z_NavSvInfo.q_GloSvMaskUsed,
                    parsed_report_ptr->z_NavSvInfo.t_BdsSvMaskUsed);
   }
   if (parsed_report_ptr->valid_mask & LOC_POS_VALID_SPEED_ENU)
   {
      LOC_MSG_MED ("parsed report: velocity ENU %d %d %d",
                    (int)parsed_report_ptr->f_VelocityENUMps[0],
                    (int)parsed_report_ptr->f_VelocityENUMps[1],
                    (int)parsed_report_ptr->f_VelocityENUMps[2]);
      LOC_MSG_MED ("parsed report: velocity ENU Uncertaunty %d %d %d",
                    (int)parsed_report_ptr->f_VelocityUncENUMps[0],
                    (int)parsed_report_ptr->f_VelocityUncENUMps[1],
                    (int)parsed_report_ptr->f_VelocityUncENUMps[2]);
   }
}

// Debug routine to dump out sv info, incomplete
static void loc_pd_dump_sv_info         (loc_gnss_info_s_type*     gnss_report_ptr)
{
   LOC_MSG_MED ("gnss report, mask = %d, num_sv = %d", gnss_report_ptr->valid_mask, gnss_report_ptr->sv_count, 0);
}

static int loc_pd_process_get_best_available_position
(
  loc_client_handle_type                           client_handle,
  const qmiLocGetBestAvailablePositionReqMsgT_v02 *bap_req_ptr
)
{
   int  ret_val = LOC_API_SUCCESS;
   loc_pd_module_data_s_type *pd_module_data_ptr = 
      &(loc_middleware_data.pd_module_data);
   do
   {
      if( NULL == bap_req_ptr || 
          LOC_CLIENT_HANDLE_INVALID == client_handle )
      {
         ret_val = LOC_API_INVALID_PARAMETER;
         break;
      }
      LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
      if (pd_module_data_ptr->get_bap_in_progress == TRUE)
      {
         LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
         ret_val = LOC_API_ENGINE_BUSY;
         break;
      }

      /* set the in progress flag and client handle since PDAPI can call
         the callback before this function returns*/
      pd_module_data_ptr->get_bap_in_progress = TRUE;
      pd_module_data_ptr->bap_client_handle = client_handle;
      pd_module_data_ptr->get_bap_txn_id = bap_req_ptr->transactionId;
      LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
     
      if( FALSE == pdsm_pd_get_best_avail_pos( 
         loc_pd_cmd_cb, 
         loc_middleware_data.pdapi_client_id,
         NULL ))
                                             
      {
         /* clear the in progress flag */
         LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
         pd_module_data_ptr->get_bap_in_progress = FALSE;

         LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

         ret_val = LOC_API_GENERAL_FAILURE;
         break;
      }
   }while(0);

   return ret_val;
}

static int loc_pd_process_get_available_wwan_position
(
  loc_client_handle_type                       client_handle,
  const qmiLocGetAvailWwanPositionReqMsgT_v02 *wwan_pos_req_ptr
)
{
  int ret_val = LOC_API_SUCCESS;
  loc_pd_module_data_s_type *pd_module_data_ptr = &(loc_middleware_data.pd_module_data);

  if (NULL == wwan_pos_req_ptr || LOC_CLIENT_HANDLE_INVALID == client_handle)
  {
    return LOC_API_INVALID_PARAMETER;
  }

  do
  {
    LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
    if (pd_module_data_ptr->get_wwan_pos_in_progress == TRUE)
    {
      LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
      ret_val = LOC_API_ENGINE_BUSY;
      break;
    }

    /* set the in progress flag and client handle since PDAPI can call
       the callback before this function returns */
    pd_module_data_ptr->get_wwan_pos_in_progress = TRUE;
    pd_module_data_ptr->wwan_pos_client_handle   = client_handle;
    pd_module_data_ptr->get_wwan_pos_txn_id      = wwan_pos_req_ptr->transactionId;
    LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

    if (FALSE == pdsm_pd_get_avail_wwan_pos(loc_pd_cmd_cb, loc_middleware_data.pdapi_client_id, NULL))
    {
      /* clear the in progress flag */
      LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
      pd_module_data_ptr->get_wwan_pos_in_progress = FALSE;
      LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

      ret_val = LOC_API_GENERAL_FAILURE;
      break;
    }
  } while(0);

  return ret_val;
}
static int loc_pd_process_get_available_position
(
  loc_client_handle_type                       client_handle,
  const qmiLocGetAvailablePositionReqParamMsgT_v02 *pos_req_ptr
)
{
  int ret_val = LOC_API_SUCCESS;
  loc_pd_module_data_s_type *pd_module_data_ptr = &(loc_middleware_data.pd_module_data);

  if (NULL == pos_req_ptr || LOC_CLIENT_HANDLE_INVALID == client_handle)
  {
    return LOC_API_INVALID_PARAMETER;
  }

  do
  {
    LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
    if (pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_in_progress == TRUE)
    {
      ret_val = LOC_API_ENGINE_BUSY;
      LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
      break;
    }

      LOC_MSG_MED("loc_pd_process_get_available_gnss_position transaction ID %d ",
                  pos_req_ptr->transactionId, 0, 0);

    /* set the in progress flag and client handle since PDAPI can call
       the callback before this function returns */
    pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_in_progress = TRUE;
    pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_client_handle   = client_handle;
    pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_txn_id      = pos_req_ptr->transactionId;
    pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_is_Nonce_valid      = pos_req_ptr->nonce_valid;
    if(pos_req_ptr->nonce_valid)
    {
      pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_Nonce      = pos_req_ptr->nonce;
    }
    if(pos_req_ptr->locPositionRptDataMode_valid )
    {
      pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_data_mode      = pos_req_ptr->locPositionRptDataMode;
    }
    else
    {
      pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_data_mode      = eQMI_LOC_SECURE_LOC_DATA_ENCRYPTED_V02;
    }
    if(pos_req_ptr->locPositionSource_valid )
    {
      pd_module_data_ptr->sec_avail_pos_client_payload.get_pos_data_source      = pos_req_ptr->locPositionSource;
    }
    else
    {
      pd_module_data_ptr->sec_avail_pos_client_payload.get_pos_data_source      = eQMI_LOC_POS_SRC_GNSS_V02;
    }

    LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

    if (FALSE == pdsm_pd_get_available_pos(loc_pd_cmd_cb, loc_middleware_data.pdapi_client_id, &(pd_module_data_ptr->sec_avail_pos_client_payload.get_pos_data_source)))
    {
      /* clear the in progress flag */
      LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
      pd_module_data_ptr->sec_avail_pos_client_payload.get_secure_available_pos_in_progress = FALSE;
      LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

      ret_val = LOC_API_GENERAL_FAILURE;
      break;
    }
  } while(0);

  return ret_val;
}

/*===========================================================================
FUNCTION loc_pd_process_inject_gsm_cell_info

DESCRIPTION
  Inject cellular information into the engine. 

DEPENDENCIES

INPUT PARAMS
  client_handle            : Identifies the loc API client injecting
  the GSM Cell information
  inject_gsm_cell_info_ptr : pointer to the GSM Cell Information 

RETURN VALUE
  LOC_API_SUCCESS           : processed command successfully
  LOC_API_GENERAL_FAILURE   : unspecified error command returned error.
  LOC_API_INVALID_PARAMETER : invalid parameter passed in.

SIDE EFFECTS

===========================================================================*/
static int loc_pd_process_inject_gsm_cell_info_req
( 
  loc_client_handle_type                    client_handle,
  const qmiLocInjectGSMCellInfoReqMsgT_v02  *inject_gsm_cell_info_ptr
)
{
   int ret_val = LOC_API_SUCCESS;
   loc_pd_cmd_cb_data *cmd_cb_data_ptr = {0};
   pdsm_cell_info_s_type pdsm_cell_info;

   do
   {
      if (NULL == inject_gsm_cell_info_ptr )
      {
          ret_val = LOC_API_INVALID_PARAMETER;
          break;
      }
      /* allocate the the callback data on heap
         to be freed in cmd_cb_unlocked or if there is
         an error. */
      cmd_cb_data_ptr = loc_calloc(sizeof(*cmd_cb_data_ptr));
      if (NULL == cmd_cb_data_ptr)
      {
         ret_val = LOC_API_GENERAL_FAILURE;
         break;
      }

      cmd_cb_data_ptr->client_handle = client_handle;
      cmd_cb_data_ptr->ioctl_type = LOC_IOCTL_INJECT_GSM_CELL_INFO_REQ;
      cmd_cb_data_ptr->cmd_payload = NULL;
      cmd_cb_data_ptr->pMe = cmd_cb_data_ptr;
      
      //set the RAT to GSM
      pdsm_cell_info.activeRAT = PDSM_ACTIVERAT_GSM;

      //set the service status
      pdsm_cell_info.serviceStatus = 
         (TRUE == inject_gsm_cell_info_ptr->roamingStatus) ? 
          STATUS_CAMPED_ROAMING : STATUS_CAMPED_NOT_ROAMING;

      // set the gsm cell information
      pdsm_cell_info.cellInfo.gsmCell.CID = 
         inject_gsm_cell_info_ptr->gsmCellId.CID;

      pdsm_cell_info.cellInfo.gsmCell.LAC = 
         inject_gsm_cell_info_ptr->gsmCellId.LAC;

      pdsm_cell_info.cellInfo.gsmCell.MCC = 
         inject_gsm_cell_info_ptr->gsmCellId.MCC;

      pdsm_cell_info.cellInfo.gsmCell.MNC = 
         inject_gsm_cell_info_ptr->gsmCellId.MNC;
     
      if (FALSE == pdsm_pd_inject_external_cell_info(
         loc_pd_cmd_cb_unlocked,
         cmd_cb_data_ptr,
         loc_middleware_data.pdapi_client_id,
         &pdsm_cell_info ))
      {
         ret_val = LOC_API_GENERAL_FAILURE;
         break;
      }

   }while(0);

   if (LOC_API_SUCCESS != ret_val)
   {
      LOC_MSG_ERROR ("loc_pd_process_inject_gsm_cell_info error %d ",
               ret_val, 0, 0);

      //free th cmd_cb_data_ptr in an error case
      if ( NULL !=  cmd_cb_data_ptr)
      {
          loc_free(cmd_cb_data_ptr);
      }
   }
   else
   {
      LOC_MSG_MED("loc_pd_process_inject_gsm_cell_info returns %d ",
                  ret_val, 0, 0);
   }
   return ret_val;
}

/*===========================================================================
FUNCTION loc_pd_process_inject_ni_msg

DESCRIPTION
  Inject external NI message into the engine. 

DEPENDENCIES

INPUT PARAMS
  client_handle            : Identifies the loc API client injecting
  the NI Msg
  inject_ni_msg_ptr :        pointer to the NI msg 

RETURN VALUE
  LOC_API_SUCCESS           : processed command successfully
  LOC_API_GENERAL_FAILURE   : unspecified error command returned error.
  LOC_API_INVALID_PARAMETER : invalid parameter passed in.

SIDE EFFECTS

===========================================================================*/
static int loc_pd_process_inject_ni_msg_req
( 
  loc_client_handle_type                    client_handle,
  const qmiLocInjectNetworkInitiatedMessageReqMsgT_v02  *inject_ni_msg_ptr
)
{
   int ret_val = LOC_API_SUCCESS;
   loc_pd_cmd_cb_data *cmd_cb_data_ptr = {0};
   pdsm_ni_message_s_type pdsm_ni_msg;

   do
   {
      if (NULL == inject_ni_msg_ptr )
      {
          ret_val = LOC_API_INVALID_PARAMETER;
          break;
      }
      /* allocate the the callback data on heap
         to be freed in cmd_cb_unlocked or if there is
         an error. */
      cmd_cb_data_ptr = loc_calloc(sizeof(*cmd_cb_data_ptr));

      if (NULL == cmd_cb_data_ptr)
      {
         ret_val = LOC_API_GENERAL_FAILURE;
         break;
      }

      cmd_cb_data_ptr->client_handle = client_handle;
      cmd_cb_data_ptr->ioctl_type = LOC_IOCTL_INJECT_NI_MESSAGE_REQ;
      cmd_cb_data_ptr->cmd_payload = NULL;
      cmd_cb_data_ptr->pMe = cmd_cb_data_ptr;
      
      switch (inject_ni_msg_ptr->injectedNIMessageType)
      {
         case  eQMI_LOC_INJECTED_NETWORK_INITIATED_MESSAGE_TYPE_SUPL_V02:
         {
            pdsm_ni_msg.type = NI_MESSAGE_SUPL;
            break;
         }

         default:
         {
            ret_val = LOC_API_INVALID_PARAMETER;
            break;
         }
      }
      //check for error case in the above switch statement
      if (LOC_API_SUCCESS != ret_val)
      {
          break;
      }

      pdsm_ni_msg.len = inject_ni_msg_ptr->injectedNIMessage_len;

      /* pass in the pointer to msg payload, PDAPI will make a copy */
      pdsm_ni_msg.ni_data = (uint8 *)inject_ni_msg_ptr->injectedNIMessage;
     
      if (FALSE == pdsm_pd_inject_ni_message(
         loc_pd_cmd_cb_unlocked,
         cmd_cb_data_ptr,
         loc_middleware_data.pdapi_client_id,
         &pdsm_ni_msg))
      {
         ret_val = LOC_API_GENERAL_FAILURE;
         break;
      }

   }while(0);

   if (LOC_API_SUCCESS != ret_val)
   {
      LOC_MSG_ERROR ("loc_pd_process_inject_ni_msg_req error %d ",
               ret_val, 0, 0);

      //free the cmd_cb_data_ptr in an error case
      if ( NULL !=  cmd_cb_data_ptr)
      {
          loc_free(cmd_cb_data_ptr);
      }
   }
   else
   {
      LOC_MSG_MED("loc_pd_process_inject_ni_msg_req returns %d ",
                  ret_val, 0, 0);
   }
   return ret_val;
}

/*===========================================================================
FUNCTION loc_pd_process_wwan_oos_notification

DESCRIPTION
  Inject WWAN Out of Service notification into the engine. 

DEPENDENCIES

INPUT PARAMS
  client_handle            : Identifies the loc API client injecting
  the OOS notification
  

RETURN VALUE
  LOC_API_SUCCESS           : processed command successfully
  LOC_API_GENERAL_FAILURE   : unspecified error command returned error.
  LOC_API_INVALID_PARAMETER : invalid parameter passed in.

SIDE EFFECTS

===========================================================================*/
static int loc_pd_process_wwan_oos_notification
( 
  loc_client_handle_type  client_handle
)
{
   int ret_val = LOC_API_SUCCESS;
   loc_pd_cmd_cb_data *cmd_cb_data_ptr;
   pdsm_cell_info_s_type pdsm_cell_info;

   do
   {
      /* allocate the the callback data on heap
         to be freed in cmd_cb_unlocked or if there is
         an error. */
      cmd_cb_data_ptr = loc_calloc(sizeof(*cmd_cb_data_ptr));

      if (NULL == cmd_cb_data_ptr)
      {
         ret_val = LOC_API_GENERAL_FAILURE;
         break;
      }

      cmd_cb_data_ptr->client_handle = client_handle;
      cmd_cb_data_ptr->ioctl_type = LOC_IOCTL_WWAN_OOS_NOTIFICATION_REQ;
      cmd_cb_data_ptr->cmd_payload = NULL;
      cmd_cb_data_ptr->pMe = cmd_cb_data_ptr;

      // clear all fields in pdsm_cell_info
      memset(&pdsm_cell_info, 0 , sizeof(pdsm_cell_info));

      pdsm_cell_info.serviceStatus =  STATUS_OUT_OF_SERVICE;

      if (FALSE == pdsm_pd_inject_external_cell_info(
         loc_pd_cmd_cb_unlocked,
         cmd_cb_data_ptr,
         loc_middleware_data.pdapi_client_id,
         &pdsm_cell_info ))
      {
         ret_val = LOC_API_GENERAL_FAILURE;
         break;
      }

   }while(0);

   if (LOC_API_SUCCESS != ret_val)
   {
      LOC_MSG_ERROR ("loc_pd_process_wwan_oos_notification error %d ",
               ret_val, 0, 0);

      //free the cmd_cb_data_ptr in an error case
      if ( NULL !=  cmd_cb_data_ptr)
      {
          loc_free(cmd_cb_data_ptr);
      }
   }
   else
   {
      LOC_MSG_MED("loc_pd_process_wwan_oos_notification returns %d ",
                  ret_val, 0, 0);
   }
   return ret_val;
}


/** locPdGenerateIPCtolocMW Generate an IPC to locMW task
 *  @param [in] msgId -- loc MW IPC message id
 *  @param [in] payloadSize -- payload size
 *  @param [in] pPayload --  pointer to the payload
 *  @return -- TRUE is IPC was sent successfully
 *          -- FALSE otherwise.
 **/
static boolean locPdGenerateIPCtolocMW(loc_middleware_msg_id_e_type msgId,
                                       size_t payloadSize,
                                       const void * const pPayload )
{
  os_IpcMsgType * const pIpcMsg = 
  os_IpcCreate(payloadSize, IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_LOC_MIDDLEWARE);

  LOC_MSG_MED ("locPdGenerateIPCtolocMW trying to send msgId: %d IPC to LoCMW", msgId, 0, 0);
  if(NULL != pIpcMsg )
  {
    /** copy the payload*/
    (void)memscpy(pIpcMsg->p_Data, payloadSize, pPayload, payloadSize);

    pIpcMsg->q_MsgId = msgId;
    return(os_IpcSend(pIpcMsg, THREAD_ID_LOC_MIDDLEWARE) );
  }
  return FALSE;
}

/**
 * called by PD task to send QMI Event Ind to LocMW 
 * task 
 *
 *  @return -- TRUE is IPC was sent successfully
 *          -- FALSE otherwise.
 */
static boolean loc_pd_event_wifi_ap_scan_info_inject_ind_proxy()
{
  qmiLocEventInjectWifiApDataReqIndMsgT_v02 eventWifiApReqInd;

  memset(&eventWifiApReqInd, 0, sizeof(eventWifiApReqInd));

  /** Generate an IPC to loc MW*/
  if(FALSE == locPdGenerateIPCtolocMW(
    LM_MIDDLEWARE_MSG_ID_PD_EVENT_WIFI_AP_SCAN_INJECT_REQ_IND, 
    sizeof(eventWifiApReqInd),
    (void *)&eventWifiApReqInd))
  {
    LOC_MSG_ERROR("loc_pd_event_wifi_ap_scan_info_inject_ind_proxy: could not send IPC to loc MW\n",
                  0,0,0);

    return FALSE;
  }
  
  return TRUE;
}


/**
 * called by PD task to send indication to LocMW task 
 *
 * @param client_handle
 *               LocAPI client handle
 * @param wifi_inj_ack
 *               Response/Ack indication sent from PDSM in
 *               locEng format
 *
 * @return 
 * TRUE - if IPC posted to LocMW success 
 * TRUE - if IPC posted to LocMW failed 
 */
static boolean loc_pd_ack_wifi_ap_scan_info_inject_ind_proxy(const loc_client_handle_type              client_handle,
                                                             const locEngInjectWifiApDataIndMsgT_v01   wifi_inj_ack)
{
  loc_pd_ack_wifi_ap_scan_info_inject_ind_struct_t ackWifiAPIndValue;

  LOC_MSG_MED("loc_pd_ack_wifi_ap_scan_info_inject_ind_proxy", 0,0,0);

  memset(&ackWifiAPIndValue, 0, sizeof(ackWifiAPIndValue));

  ackWifiAPIndValue.client_handle = client_handle;

  ackWifiAPIndValue.ackWifiApInd.status = convertLocEngStatus(wifi_inj_ack.status);

  /** Generate an IPC to loc MW*/
  if (FALSE == locPdGenerateIPCtolocMW(LM_MIDDLEWARE_MSG_ID_PD_INJECT_WIFI_AP_SCAN_IND,
                                       sizeof(ackWifiAPIndValue),
                                       (void *)&ackWifiAPIndValue))
  {
    LOC_MSG_ERROR("loc_pd_ack_wifi_ap_scan_info_inject_ind_proxy: could not send IPC to loc MW\n",
                  0, 0, 0);

    return FALSE;
  }

  return TRUE; 
}


/** loc_pd_event_wifi_ap_scan_info_inject_ind_handler
 *  
 *  Handle to send QMI Event IND to the client from LocMW.
 *  
 *  @param [in] pIpcMsg  Pointer to ipc message containing the
 *         indication
 *
 * @return 
 * TRUE - if the QMI IND has been sent successfully TRUE 
 * FALSE - if the QMI IND send failed 
 * */
void loc_pd_event_wifi_ap_scan_info_inject_ind_handler(const os_IpcMsgType*   const pIpcMsg)
{
  LOC_MSG_MED("loc_pd_event_wifi_ap_scan_info_inject_ind_handler", 0,0,0);

  if((NULL != pIpcMsg) && 
     (pIpcMsg->q_Size == sizeof(qmiLocEventInjectWifiApDataReqIndMsgT_v02)))
  {
    qmiLocEventInjectWifiApDataReqIndMsgT_v02 eventWifiApReqInd;
    locQmiShimIndInfoStructT shimIndInfo; 
   
    shimIndInfo.q_MsgLen = memscpy(&eventWifiApReqInd, sizeof(eventWifiApReqInd), pIpcMsg->p_Data, pIpcMsg->q_Size);
    
    shimIndInfo.p_Msg = &eventWifiApReqInd;
    shimIndInfo.q_Id = QMI_LOC_EVENT_INJECT_WIFI_AP_DATA_REQ_IND_V02;

    // Broadcast to all registered clients for now
    if(FALSE == locQmiShimBroadcastInd(
      QMI_LOC_EVENT_MASK_INJECT_WIFI_AP_DATA_REQ_V02,
      &shimIndInfo))
    {
      LOC_MSG_ERROR("loc_pd_event_wifi_ap_scan_info_inject_ind_handler: Could not send indication\n",
                    0,0,0);
    }
  }
  else
  {
    LOC_MSG_ERROR("loc_pd_event_wifi_ap_scan_info_inject_ind_handler: Invalid IPC message\n", 0,0,0);
  }

  return;
}


/** loc_pd_inject_wifi_ap_scan_info_ind_handler
 *  
 *  Handle to send QMI IND to the client from LocMW.
 *  
 *  @param [in] pIpcMsg  Pointer to ipc message containing the
 *         indication
 *
 * @return 
 * TRUE - if the QMI IND has been sent successfully TRUE 
 * FALSE - if the QMI IND send failed 
 * */
boolean loc_pd_inject_wifi_ap_scan_info_ind_handler(const os_IpcMsgType*  const pIpcMsg)
{
  LOC_MSG_MED("loc_pd_inject_wifi_ap_scan_info_ind_handler", 0,0,0);

  if((NULL != pIpcMsg) && 
     (pIpcMsg->q_Size == sizeof(loc_pd_ack_wifi_ap_scan_info_inject_ind_struct_t)))
  {
    loc_pd_ack_wifi_ap_scan_info_inject_ind_struct_t ackWifiAPIndValue;
    locQmiShimIndInfoStructT shimIndInfo; 
   
    shimIndInfo.q_MsgLen = memscpy(&ackWifiAPIndValue, sizeof(ackWifiAPIndValue), pIpcMsg->p_Data, pIpcMsg->q_Size);

    // send a unicast message
    shimIndInfo.p_Msg = &ackWifiAPIndValue.ackWifiApInd;
    shimIndInfo.q_Id = QMI_LOC_INJECT_WIFI_AP_DATA_IND_V02;

    LOC_MSG_MED("QMI_LOC_INJECT_WIFI_AP_DATA_IND_V02: calling locQmiShimSendInd client handle %d", ackWifiAPIndValue.client_handle, 0, 0);

    /* send indication */
    return locQmiShimSendInd(ackWifiAPIndValue.client_handle,
                           &shimIndInfo);
  }
  else
  {
    LOC_MSG_ERROR("loc_pd_event_wifi_ap_scan_info_inject_ind_handler: Invalid IPC message\n", 0,0,0);
    return FALSE;
  }
}

/**
 * Function to handle QMI command from control point to inject 
 * wifi ap data 
 *
 * @param client_ptr LocAPI client handle
 * @param pReq       QMI request
 * @param pResp      QMI response
 *
 * @return LOC_API_SUCCESS if the command has been handled successfully
 */
boolean loc_pd_process_inject_wifi_ap_data_req(loc_client_info_s_type     *const client_ptr,
                                               const qmiLocInjectWifiApDataReqMsgT_v02  *pReq,
                                               qmiLocGenRespMsgT_v02      *const pResp)
{
  pdsm_wifi_on_demand_info_list_s_type pdsm_inject_wifi;
  loc_pd_module_data_s_type *pd_module_data_ptr = &(loc_middleware_data.pd_module_data);
  locEngWifiApInfoStructT_v01 *wifiApArray = NULL;
  boolean  ret_val = FALSE;

  if (NULL == pResp)
  {
    LOC_MSG_ERROR("loc_pd_inject_wifi_ap_data_req: pResp is NULL \n", 0, 0, 0);
    return ret_val;
  }

  if ((NULL == client_ptr) || (NULL == pReq))
  {
    /** invalid parameters */
    pResp->resp.result = QMI_RESULT_FAILURE_V01;
    pResp->resp.error = QMI_ERR_INVALID_ARG_V01;
    LOC_MSG_ERROR("loc_pd_inject_wifi_ap_data_req: Invalid params\n", 0, 0, 0);
    return ret_val;
  }

  memset((void *)&pdsm_inject_wifi, 0, sizeof(pdsm_inject_wifi));

  do
  {
    LOC_MSG_MED("loc_pd_process_inject_wifi_ap_data_req. client: %d, num of APs %d",
                client_ptr->client_handle, pReq->wifiApInfo_len, 0);

    /* check the list length */
    if (pReq->wifiApInfo_len > QMI_LOC_WIFI_MAX_REPORTED_APS_PER_MSG_V02)
    {
      pResp->resp.result = QMI_RESULT_FAILURE_V01;
      pResp->resp.error = QMI_ERR_ARG_TOO_LONG_V01;

      LOC_MSG_ERROR("loc_pd_inject_wifi_ap_data_req: wifiApInfo_len greater than limit \n", 0, 0, 0);
      break;
    }

    LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
    /* check progress flag */
    if (pd_module_data_ptr->wifi_ap_scan_in_progress == TRUE)
    {
      pResp->resp.result = QMI_RESULT_FAILURE_V01;
      pResp->resp.error = QMI_ERR_DEVICE_IN_USE_V01;
      LOC_MSG_ERROR("loc_pd_inject_wifi_ap_data_req: wifi ap scan injection already in progress\n", 0, 0, 0);
      LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);
      break;
    }

    /* set progress flag*/
    pd_module_data_ptr->wifi_ap_scan_in_progress = TRUE;

    /* store the client handle for later use */
    pd_module_data_ptr->wifi_ap_scan_client_handle = client_ptr->client_handle;
    LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.pd_crit_sec);

    /* convert QMI to locEng/pdsm format */
    pdsm_inject_wifi.q_num_wifi_aps = pReq->wifiApInfo_len;

    wifiApArray = (locEngWifiApInfoStructT_v01*)os_MemAlloc(sizeof(locEngWifiApInfoStructT_v01)*pdsm_inject_wifi.q_num_wifi_aps, OS_MEM_SCOPE_TASK);
    if( NULL == wifiApArray)
    {
      LOC_MSG_ERROR("pdsm_pd_inject_wifi_info_on_demand os_MemAlloc returns NULL", 0, 0, 0);
      break;
    }
    convertQmiLocInjectWifiApDataReq(&pReq->wifiApInfo[0], wifiApArray, pdsm_inject_wifi.q_num_wifi_aps);

    pdsm_inject_wifi.first_wifi_ap_ptr = wifiApArray;

    // call pdsm
    /* return value is used to send QMI response. NO need for loc_pd_cmd_cb to handle again */
    if (TRUE == pdsm_pd_inject_wifi_info_on_demand(NULL,      //loc_pd_cmd_cb,
                                                   (void *)client_ptr,
                                                   loc_middleware_data.pdapi_client_id,
                                                   &pdsm_inject_wifi)) 
    {
      LOC_MSG_MED("pdsm_pd_inject_wifi_info_on_demand return success",
                  0, 0, 0);

      pResp->resp.result =  QMI_RESULT_SUCCESS_V01;
      pResp->resp.error = QMI_ERR_NONE_V01;
      ret_val = TRUE;
    }
    else
    {
      LOC_MSG_MED("pdsm_pd_inject_wifi_info_on_demand return failure",
                  0, 0, 0);

      /* loc_pd_cmd_cb will not be called for failure handling
         wifi on demand - PDSM_PD_CMD_INJECT_WIFI_ON_DEMAND - will never be called as we are passing null func pointer
        in the pdsm call. Failure QMI RESP is sent when pdsm call return failure. NO QMI IND is sent */

      LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);
      /* reset progress flag*/
      pd_module_data_ptr->wifi_ap_scan_in_progress = FALSE;
      pd_module_data_ptr->wifi_ap_scan_client_handle = LOC_CLIENT_HANDLE_INVALID;
      LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);

      pResp->resp.result =  QMI_RESULT_FAILURE_V01;
      pResp->resp.error = QMI_ERR_INTERNAL_V01;
    }
  }while(0);

  if(wifiApArray)
  {
    os_MemFree((void **)&wifiApArray);
  }

  return ret_val;
}



