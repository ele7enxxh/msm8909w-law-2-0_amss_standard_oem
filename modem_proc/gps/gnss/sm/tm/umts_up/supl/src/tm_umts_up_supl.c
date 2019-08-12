/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TM-UMTS-UP-SUPL sub-module

GENERAL DESCRIPTION
  This file implements TM UMTS-UP SUPL sub-module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_up/supl/src/tm_umts_up_supl.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  08/30/16   skm     Fix to drop EarlyRsp when no PRM or OTDOA RSTD's in Hybrid/OTDOA only CF's 
  07/05/16   rk      Updated GAL/QZSS related traces.
  07/06/16   rn      Fix SM from sending out empty SUPL POS before OTDOA timer expiry when
                     server initially requests UEB agps mode and later changes to OTDOA only. 
  06/30/16   skm     OTDOA only call flow not working for early timer.
  06/30/16   skm     Early Response MSB not sent
  04/18/16   skm     Fix for crash seein > 24 AD test case SUPL message queuing
  03/29/16   gk      remove q_init at session cleanup
  02/12/16   skm     Add queuing support to send >24AD SUPL POS
  01/05/16	 mc 	 Dynamically allocate variables for huge structures to reduce stack usage
  10/26/15   rk      Support emergency simulation for missing SI MSA case
  09/10/15   rk      Use the pre-defined GLONASS & 4G position method bits in NV 1920 to enabled or
                     advertise GLONASS & 4G position method support respectively
  08/26/15   gk      90% conf reporting changes
  08/06/15   rk      Stop the OTDOA engine only if its running.
  07/25/15   rk      AD request Holdoff timer aren't needed for NI session.
  06/29/15   rk      Cleanup PDSM_PA_ASST_GLONASS_POS_PROTOCOL PA event handling.
  06/24/15   rh      Updated code to use CM-based Cell ID update
  06/22/15   gk      check for waiting for subsequent meas blocks modified.
  06/09/15   mj     Add support for W+W feature
  06/05/15   rk      Fix overwriting the RRLP protocol version in some cases.
  04/23/15   rk      In SI do not request GLO AD if the SLP told you in SUPL RESPONSE it wants to do GPS only.
  04/16/15   rk      When phone camps after 911 is dialed, set_ID by IMEI at E911 dialing shouldn't overwrite.
  04/06/15   skm     SUPL Service interaction broken for incoming MT-ES during ongoing MT session
  03/14/15   rh      Added support for MultiSIM-EFS NV
  03/10/15   lt      Proper handling of TLS version in SUPL messages for areaEventTriggered sessions.
  02/01/15   gk      Clean up tx mgr only if LPP is enabled.
  01/27/15   gk      reset session during E911 handler even if not camped on any RAT.
  01/24/15   gk      Vertical Velocity incorrectly encoded
  01/16/15   rk      Don't ignore good GPS measurements when subsequent GLO has zero SVs & vice-versa.
  01/14/15   rk      Added LPP RSTD >24 measurement processing support
  11/04/14   ah      Support SUPL TD-SCDMA LocID Config through OEM Feature Mask NV
  10/27/14   gk      NV item for terminating SUPL after final fix 
  10/21/14   mj      Add TDSCDMA handling for function tm_umts_up_supl_select_pos_prot
  09/24/14   rk      Added missing steering source type while injecting AcqAssist.
  07/17/14   gk      SUPL MO should not work when NO-SIM. 
  07/02/14   ss      Fixing issues with MSB and MSA request handling when MSA or MSB 
                     is disabled by NV and when LPP is disabled.
  06/30/14   rk      Fix horizontal velocity error computation.
  06/26/14   rk      Fixed Glonass measurement weren't propagated due to incorrect indexing in the SUPL module.
  06/12/14   skm     Fix WCDMA SUPL2.0 RRLP MSA Broken due to GLO meas request to ME in RRLP GPS Assistance Proc
  05/22/14  skm      Update SUPL session Status when session fails
  05/14/14   rk      Fixed the cached measurement report delete issue. Also added support
                     to receive & process the measurements (GPS, GLO) in any order.
  05/01/14   gk      Get the primary PLMN info to send the lte cell id
  04/03/14   ssu     Additional Fix details parameters for Geofence Breach reports
  12/23/13   mj      Remove FEATURE_DUAL_SIM and FEATURE_TRIPLE_SIM
  02/27/13  ssh      OTA Delete to clear away existing NI Geofences
  10/29/13   mj      Update WCDMA and TDSCDMA API calls for segment loading
  08/06/13   mj      Handle returned value from mcc_pd_get_bs_info
  07/12/13   rk      Don't overwrite the LPP error type when set by ReqLocInfo function.
  06/17/13   mj      Call api rr_ds_get_current_bs_info for DSDS target
  05/13/13  ssh      Using timestamp in SUPL_END while pushing the positon to TM.
  05/14/13  ssh      Handle TCP connections in back to back SUPL sessions
  02/22/13   rk      Use correct position method bitmask (against NV 1920) to set SUPL AGNSS posMethod (when LPP as position protocol).
                     Also allow OTDOA to continue in AGNSS settings. [CR 466845]
  02/22/13   rk      Bring back cached measurement report usage for LPP E911 scenario
  02/14/13   mj      Initialize rr_cell_info before accessing it
  11/04/12   mj      Added new function supl_chk_if_external_modem_OOS
  09/07/12   mj      Added change for external cell updates for SGLTE
  08/01/12   ssu     Changes in SUPL to make use of cached cell id while doing
                     a SUPL session till a valid cell-id is available.
  01/04/12   ssu     Obtaining th CELL id information in CELL_DCH from CM APIs
  12/13/11   gk      LPP related fixes
  04/06/11   LT      ASN1 migration.
  03/03/11   gk      Timer race condition fix
  01/03/11   gk      Eph throttling rework
  04/01/10   atien   Add NMR feature
  01/14/10   rb      Set PDSM_PD_UTC_TIME_VALID flag when UTC time is available
  12/14/09    gk     send comm failure event
  10/23/09   rb      Added SUPL Cell ID feature
  07/13/09   gk      Check the supl server address correctly
  05/29/09   lt      Controls for some SUPL optional field build time control.
  05/21/09   gk      Treat PDSM_SESS_TYPE_TRACK_IND as new
  04/21/09   gk      Check if we are in UMTS/GSM before starting 911 processing
  01/06/09   LT      Cached position is now included in SUPL_POS_INIT.
  05/19/08   LT      NI request rejection during emergency added.
  08/02/07   LT      Handling SVs non-existence case; Removal of alm. week extension.
  06/20/07   LT      Added handling of NV Item aagps_gps_lock_control.
  05/25/07   LT      Added GpsComplete event; Time-Stamping position info;
  09/14/06   LT      Initial version

============================================================================*/

#include "gps_variation.h"
#include "customer.h"   /* Customer configuration file */

#ifdef FEATURE_CGPS_UMTS_UP_SUPL

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stdio.h>

#include "msg.h"
#include "math.h"
#include <limits.h>

#include "mgp_api.h"
#include "sm_nv.h"
#include "tm_data.h"
#include "tm_common.h"

#include "uim.h"
#include "pbmlib.h"

#include "pd_comms_api.h"
#include "tm_asn1util.h"

#include "tm_data.h"
#include "tm_rrlp_asn1.h"

#include "tm_umts_common_utils.h" /* this files includes tm_rrlp.h */

#include "tm_sim_utils.h"

#include "sm_api.h" /* included only for the data structures defined there */
#include "sm_log.h"

#ifdef FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH /* this would be defined in custwcdma.h */
#undef FEATURE_WCDMA_GPS_CELL_ID_READING /* this would be defined in custcgps.h */
#endif

#define SUPL_MAIN_VARIABLES_DEFINED
#include "tm_umts_up_supl.h"    /* which includes "tm_supl_asn1.h" */
#undef SUPL_MAIN_VARIABLES_DEFINED

#include "tm_umts_up_supl_comm.h"
#include "pd_comms_api.h"
#include "pdapibuf.h"

#include "rrclsmif.h"

#ifdef FEATURE_SUPL1_TDS  /* defined in custcgps.h, if .builds defines FEATURE_CGPS_USES_TDS */
#include "tdsrrclsmif.h"
#endif

#include "fs_public.h"
#include "gps_fs_api.h"

#include "tm_prtl_iface.h"
#include "geran_eng_mode_read_api.h"

#if defined (FEATURE_CGPS_USES_CDMA) && !defined (FEATURE_GNSS_LOW_MEMORY)
  #include "tm_cm_iface.h"
  #include "tm_1x_up_is801_drv_iface.h"
  #include "tm_is801.h"
  #include "mccdma.h"
  #ifdef FEATURE_CMI
    #include "mccdma_v.h"
  #endif
#endif /* FEATURE_CGPS_USES_CDMA && !FEATURE_GNSS_LOW_MEMORY*/
#include "tm_lpp_tx_mgr.h"
#include "tm_lpp.h"
#include "tm_lpp_asn1util.h"
#include "tm_lpp_up.h"
#include "tm_lpp_common_utils.h"
#include "tm_pdapi_client.h"
#include "gnss_calendar_util.h"
#include "gnss_wwan_iface.h"
#include "tm_l1_iface.h"


/* same def. as in tm_core.c */
#define C_MIN_EPH_HAVE_THRESHOLD 8
#define C_MAX_EPH_NEED_THRESHOLD 1
#define C_MIN_SV_IN_VIEW_THRESHOLD 6

#define NMR_HANDOVER_LIMIT 3  /* we won't send NMR info if # of handovers is greater than this */

#define BIT22   0x200000
#define XTEND22 0xFFE00000

  /* sign extend 22 bit signed integer to signed long */
#define EXTEND22(dword22) \
  ( long ) (( dword22 & BIT22 ) ? ( dword22 | XTEND22 ) : dword22 )

#define BIT23   0x400000
#define XTEND23 0xFFC00000

  /* sign extend 23 bit signed integer to signed long */
#define EXTEND23(dword23) \
  ( long ) (( dword23 & BIT23 ) ? ( dword23 | XTEND23 ) : dword23 )


/* NMR Related
** NMR reported from GERAN can have up to ENG_MODE_MAX_NMR_ITEMS of neighboring cells excluding 1 serving cell
** However, the NMR FRS wants to support up to NMR_MAX_SIZE of cell in reporting.
*/
#define NMR_MAX_SIZE                   (15)   /* Maximum size of NMR in GSM Cell Info - supl_GsmCellInformation */
supl_NMR                               z_nmr;
supl_NMRelement                        nmr_list[NMR_MAX_SIZE];
eng_mode_scell_measurements_t          nmr_serving_cell_measurement;
eng_mode_nmr_info_t                    nmr_info;
uint32                                 timing_advance = 0;

wl1_lsm_cell_meas_update_type       z_DBCellMeas;
#ifdef FEATURE_CGPS_LTE_CELLDB
lte_ml1_gps_mrl_cell_meas_update_t  z_lte_mrl;
#endif

supl_IPAddress                      z_supl_IPAddress; /* for ASN1 encoding */
#define MAX_IMEI_LEN 8    /* Maximum number of bytes required to store the IMSI of the UE */

/* TBD: this and ver_maj_after_supl_init_glob need to go up to oem */
typedef struct
{
  boolean trigger_type_present;
  uint32  q_trigger_type;  /* 0: periodic; 1: areaEvent */
}supl2_specific_supl_init_ies;
supl2_specific_supl_init_ies z_supl_init_trigger_type;


uint32 q_supl_ver_maj_after_supl_init_glob; /* use to remember the agreement before
                                               the session is accepted */

/* Copy the original LPP RLI which has OTDOA request */  
tm_lpp_transaction_info_s_type  z_lpp_up_tx_rli;

/*************** functions internal to the SUPL module ******************/

static boolean supl_SuplInit_proc
( const supl_ULP_PDU* p_pdu_decoded,
  uint8* p_raw_pdu,
  uint16 w_raw_pdu_len,
  uint8* p_hash
);

static boolean supl_SuplResponse_proc( const supl_ULP_PDU* p_pdu_decoded );
static boolean supl_SuplPos_proc( const supl_ULP_PDU* p_pdu_decoded );

static boolean supl_SuplPos_rrlp_proc(uint8 *p_payload, const uint16 payload_length);
static boolean supl_SuplPos_is801_proc(uint8 *p_payload, const uint16 payload_length);
static void    supl_SuplEnd_proc ( const supl_ULP_PDU* p_pdu_decoded );
static boolean supl_SuplPos_RRLP_ACK_tx (void);

static boolean supl_SuplPos_rrlp_tx
( uint8* p_data,
  uint16 w_byte_cnt,
  supl_Velocity *p_velocity
);

static boolean supl_cell_info_sanity_chk_ok( const cgps_CellInfoCachedStruct *cell_db_data_ptr );

static boolean supl_cell_information_construct(supl_CellInfo* p_asn1_cell_info,
                                               supl_cell_info_struct_type* p_cell_info_allocated);

static uint8  supl_chk_and_build_all_mlid (supl_LocationIdData *p_supl_LocationIdData,
                                           supl_LocationId *p_supl_LocationID,
                                           supl_cell_info_struct_type* p_cell_info_allocated,
                                           uint8 u_num_allocated_array_elements);


static boolean supl_mlid_construct(supl_CellInfo* p_asn1_cell_info,
                                   supl_cell_info_struct_type* p_cell_info_allocated,
                                   uint8 u_mlid_buffer_index);

static void    supl_store_slp_sessionId(const supl_ULP_PDU* p_pdu);

static void    supl_util_server_session_id_organizer
( const supl_SessionID *p_source_session_id,
  supl_slp_session_id *p_slp_session_id
);

static boolean supl_rrlp_decode_proc ( tm_network_assist_s_type  *p_zSmlcAssist,
                                       uint8*            Data_ptr,
                                       uint16            w_max_len_byte,
                                       int*              oss_decode_status_for_calling_func_ptr );

static boolean supl_rrlp_msr_pos_req_proc( rrlp_MsrPosition_Req *p_mpr );

static void supl_rrlp_msr_pos_req_qos_proc( rrlp_MsrPosition_Req *p_mpr );

static uint8 supl_rrlp_gps_assist_proc ( rrlp_ControlHeader *p_assist );

static void supl_set_session_id_organizer ( const supl_SessionID *p_source_session_id,
                                            supl_set_session_id *p_set_session_id);

static slp_mess_header_check_result_e_type  supl_message_overhead_proc
( const supl_ULP_PDU* p_pdu_decoded,
  uint16*             p_provided_set_session_id
);

static void supl_send_notification_verifcation(const supl_ULP_PDU* p_pdu_decoded, const uint8 *p_hash);
static pdsm_lcs_supl_format_indicator_e_type supl_convert_format_indicator_pdsm (supl_FormatIndicator formatInd);

static void supl_action_upon_ni_notif_verif_accepted
( pdsm_lcs_supl_notify_verify_req_s_type *p_req_data,
  supl_slp_session_id                    *p_slp_id
);

static void supl_send_pos_rrlp_to_slp
( gad_ellip_alt_unc_ellip_type *p_gad_shape_pos,
  supl_Velocity *p_velocity,
  uint32 q_timestamp_ms,
  uint32 q_pos_sources,
  uint32 q_glonass_tod_modulo_1hour_ms);

static boolean supl_SuplPos_RRLP_protocol_err_tx (int8 b_RefNum, rrlp_ErrorCodes e_err_code );
static void supl_preempt_ongoing_session(supl_StatusCode e_cancel_reason );

static void supl_gad9_to_pos_est
( const gad_ellip_alt_unc_ellip_type *p_zEllipAltUncEllip,
  supl_PositionEstimate *p_positionInfo
);

void supl_convert_slp_id_from_notif_verif_resp
( pdsm_lcs_supl_notify_verify_req_s_type   *p_req_data,
  supl_slp_session_id                      *p_slp_id
);

static void supl_convert_velocity
( float f_vel_east,
  float f_vel_north,
  float f_vel_vertical,
  supl_Velocity *p_velocity
);

static void supl_convert_supl_end_pos_for_tm_core
(const supl_SUPLEND* p_suplEnd, gps_RefLocStructType *p_ref_loc );

static void supl_glo_eph_build
(rrlp_GANSSSatelliteElement *p_ganss_sat_element);

static void supl_glo_eph_svid_freq_num_map_aux
(struct rrlp_GANSS_ID3 *p_glo_aux );

static void supl_glo_eph_svid_freq_num_map_alm
(rrlp_Almanac_GlonassAlmanacSet *p_glo_alm_set);

static void supl_glo_acqasst_svid_freq_num_map_aux
(struct rrlp_GANSS_ID3 *p_glo_aux,
 prot_glo_acqast_buffer_s_type *p_glo_acqasst);

static void check_and_handle_handover (boolean *abort_sending_next_msg);
static tm_lpp_provide_cap_s_type z_lpp_provide_cap;


#if !defined (FEATURE_CGPS_USES_CDMA) || defined (FEATURE_GNSS_LOW_MEMORY)
// for strange build that has the feature above defined while
// no 1X AGPS code objs, comment the #ifndef out
static boolean is801_decode_fwd_link_pddm
( tm_prtl_type  prtl_type,
  byte          *data,
  uint16        length,
  byte          *offset
)
{
  return FALSE;
}
#endif  /* not-FEATURE_CGPS_USES_CDMA  Dummies: for UMTS-only builds */

static supl_init_trigger_type_enum_type supl_init_trigger_event_check (void);

static void  supl_init_trigger_event_proc
( pdsm_lcs_supl_notify_verify_req_s_type *p_req_data,
  supl_slp_session_id                    *p_slp_id,
  tm_cm_phone_state_info_s_type          phone_state_info
);

static void supl_set_cf_state_area_event (supl_cf_state_enum_type  e_new_supl_cf_state,
                                          uint8                    u_session_num );

static boolean supl_is_in_any_ut0 (void);

static boolean supl_SuplTriggerStart_tx (uint8 u_session_num);
static void supl_SuplTriggerResponse_proc ( const supl_ULP_PDU* p_pdu_decoded );
static void supl_SuplReport_proc( const supl_ULP_PDU* p_pdu_decoded );



static uint8 supl_area_event_session_match ( const supl_SessionID *p_source_session_id );
static void supl_SET_id_get(void);

static boolean supl_SLP_session_id_match_check
( supl_slp_session_id *p_cached_Slp_Sess_Id, supl_slp_session_id *p_incoming_Slp_Sess_Id );


static boolean tm_umts_up_supl_supl_report_prepare_or_tx
( uint8                              u_session_num,
  sm_gm_client_breach_notify_type*   p_gm_client_breach_notify,
  uint8*                             p_hash,
  supl_slp_session_id*               p_provided_slp_session_id
);

static boolean tm_umts_up_supl_supl_report_tx
( uint8                               u_session_num,
  sm_gm_client_breach_notify_type*    p_gm_client_breach_notify,
  uint8*                              p_hash,
  supl_slp_session_id*                p_provided_slp_session_id
);

static void  supl_session_query_proc( uint8 *p_hash,
                                      supl_slp_session_id* p_provided_slp_session_id,
                                      uint32 q_supl_version );

static void supl_SuplTrigStop_proc (const supl_ULP_PDU* p_pdu_decoded);

#if defined (FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
static boolean b_supl_lpp_tx_mgr_init = FALSE;

static void tm_umts_up_supl_lpp_tx_mgr_dereg(void);
#endif /* FEATURE_CGPS_LTE_CELLDB && !FEATURE_GNSS_LOW_MEMORY */

static boolean supl_chk_if_external_modem_OOS(const pdsm_cell_info_s_type external_cell_info);

static boolean supl_is_wifi_scan_needed (void);

static void supl_request_wifi_scan(void);

#define SUPL_VELOCITY_UPWARD 0
#define SUPL_VELOCITY_DOWNWARD 1

/*===========================================================================

FUNCTION supl_fill_ver2_ganss_set_capability

DESCRIPTION
  This function populates the SUPL GANSS SET Capability IEs in the SUPL_START &
  SUPL_POS_INIT message.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/

void supl_fill_ver2_ganss_set_capability( supl_SETCapabilities * const pz_SetCpblt, supl_GANSSPositionMethod   * const pz_ganssPositionMethod,
                                          supl_GANSSPositionMethods  *p_ganssPositionMethodsList, supl_callflow_kind_enum_type  const u_callflow_type )
{

  if ( ( NULL == pz_SetCpblt ) || (NULL == pz_ganssPositionMethod) || (NULL == p_ganssPositionMethodsList) )
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "SUPL ver2 ganss set capability cannot be populated" );
    return;
  }

  // RRLP only
  if ( tm_umts_up_supl_rrlp_assisted_glonass_enabled() )
  {
    // Replace with the current RRLP8 version
    pz_SetCpblt->posProtocol.ver2_PosProtocol_extension.posProtocolVersionRRLP.majorVersionField =
                                                                                                   SUPL_RRLP_CURRENT_VERSION_MAJOR;

    pz_SetCpblt->posProtocol.ver2_PosProtocol_extension.posProtocolVersionRRLP.technicalVersionField =
                                                                                                       SUPL_RRLP_CURRENT_VERSION_TECH;

    pz_SetCpblt->posProtocol.ver2_PosProtocol_extension.posProtocolVersionRRLP.editorialVersionField =
                                                                                                       SUPL_RRLP_CURRENT_VERSION_EDIT;
  } /* if Glonass and RRLP8_SUPL provisioned */

  // Common for LPP/RRLP8
  if ( (tm_umts_up_supl_rrlp_assisted_glonass_enabled()) ||
       (tm_core_get_asst_glo_lpp_up_enable() && tm_core_get_lte_lpp_up_enable()) )
  {
    z_lpp_provide_cap.u_glonass_supported = TRUE;

    pz_SetCpblt->posTechnology.m.ver2_PosTechnology_extensionPresent = 1;
    pz_SetCpblt->posTechnology.ver2_PosTechnology_extension.m.gANSSPositionMethodsPresent = 1;

    pz_ganssPositionMethod->m.ganssSBASidPresent = 0; /* no GANSS SBA Sid */
    pz_ganssPositionMethod->ganssId = C_SUPL2_GANSS_ID_GLONASS;

    /** LPP or RRLP **/
    if ( u_callflow_type == C_SUPL_SESSION_UI )
    {
      /* Set-Initiated */
      if ( ( z_supl_session_status.u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED ) &&
           tm_core_pos_mode_allows_agnss_msa_up( z_supl_session_status.u_position_protocol ) )
      {
        pz_ganssPositionMethod->gANSSPositioningMethodTypes.setAssisted = TRUE;
        z_lpp_provide_cap.z_agnss_capabilities.u_pos_mode_supported |= LPP_AGNSS_POS_MODE_UE_ASSISTED;
      }
      else if ( ( z_supl_session_status.u_agps_mode == C_SUPL_AGPS_MODE_UE_BASED ) &&
                tm_core_pos_mode_allows_agnss_msb_up( z_supl_session_status.u_position_protocol ) )
      {
        pz_ganssPositionMethod->gANSSPositioningMethodTypes.setBased = TRUE;
        z_lpp_provide_cap.z_agnss_capabilities.u_pos_mode_supported |= LPP_AGNSS_POS_MODE_UE_BASED;

        /* in case SLP wants to do auto. */
        pz_ganssPositionMethod->gANSSPositioningMethodTypes.autonomous = TRUE;
      }
    }
    else
    {
      /* posTechnology AGPS -- network-initiated: all method regardless
        what SUPL_INIT says, with PreferedMethod set to NoPref, per MSM6275 SUPL 1.0
        Support Reference Guide */
      /* Network-Initiated */
      if ( tm_core_pos_mode_allows_agnss_msa_up( z_supl_session_status.u_position_protocol ) )
      {
        pz_ganssPositionMethod->gANSSPositioningMethodTypes.setAssisted = TRUE;
        z_lpp_provide_cap.z_agnss_capabilities.u_pos_mode_supported |= LPP_AGNSS_POS_MODE_UE_ASSISTED;
      }
      if ( tm_core_pos_mode_allows_agnss_msb_up( z_supl_session_status.u_position_protocol ) )
      {
        pz_ganssPositionMethod->gANSSPositioningMethodTypes.setBased = TRUE;
        z_lpp_provide_cap.z_agnss_capabilities.u_pos_mode_supported |= LPP_AGNSS_POS_MODE_UE_BASED;
      }
      if ( TM_CONFIG_POS_MODE_BIT_IS_SET( PDSM_POS_MODE_STANDALONE ) )
      {
        pz_ganssPositionMethod->gANSSPositioningMethodTypes.autonomous = TRUE;
      }
    }

    pz_ganssPositionMethod->gANSSPositioningMethodTypes.extElem1.count = 0;

    pz_ganssPositionMethod->gANSSSignals.numbits = 1;
    pz_ganssPositionMethod->gANSSSignals.data[0] = LPP_GNSS_SIGNAL_ID_GLONASS_G1_BMASK; /* leftmost: G1 -- only G1 is supported */

    pz_ganssPositionMethod->extElem1.count = 0;

    p_ganssPositionMethodsList->elem = pz_ganssPositionMethod;
    p_ganssPositionMethodsList->n = 1;
    pz_SetCpblt->posTechnology.ver2_PosTechnology_extension.gANSSPositionMethods =
                                                                                   *p_ganssPositionMethodsList;
  } /* Common for RRLP8 & LPP */
}


/*===========================================================================

FUNCTION tm_umts_up_supl_session_init

DESCRIPTION
  This function initializes all protocol session related variables.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/

void tm_umts_up_supl_session_init(boolean session_clear_for_prempt)
{
  boolean u_supl_end_waiting = FALSE;
  supl_msg_queue_element *supl_msg = NULL;

  if(session_clear_for_prempt)
  {
    MSG_HIGH("SUPL Session status cleared for prempt",0,0,0);

    if(z_supl_session_status.e_cf_state & C_SUPL_CF_STATE_UT0_SUPLEND)
    {
      u_supl_end_waiting = TRUE;
    }
  }
  else
{
    MSG_HIGH("SUPL Session status cleared",0,0,0);
    memset ( z_pending_supl_end, 0, sizeof(pending_supl_end) * C_MAX_NUM_PENDING_SUPL_END );
  }

  if (z_supl_session_status.supl_msg_q_status == TRUE)
  {
    /*If SUPL session when pre-empted was in a MULTIPLE MEAS CF state 
    There could be messages in SUPL MSG queue. Free the PDSM_Buffers
    and Queue will be re-initialized in SUPL enqueue function in the next session*/
    while (0 != q_cnt(&(z_supl_session_status.supl_msg_q))) 
    {
      supl_msg = (supl_msg_queue_element *)q_get(&(z_supl_session_status.supl_msg_q));
      if (NULL!=supl_msg) 
      {
        (void)pdsm_freebuf((char*)supl_msg);
      }
    }
    /*Call Queue Destroy to release the mutex associated with it. 
       This call will release the Mutex assiciated with the queue,
       1)initialized at SUPl module INIT
       2)after a successful >24AD session. But not for all other SUPL sessions.
     */
     (void)q_destroy(&(z_supl_session_status.supl_msg_q));
  }

  /*All common SUPL initialization*/
  supl_session_status_reset();
  memset ( &z_supl_emergency_status, 0, sizeof(supl_emergency_struct_type));
  z_supl_session_status.z_network_assist.radio_link_prot_params.z_lpp_params.u_measure_response_time_sec = 0;
  z_supl_session_status.z_network_assist.radio_link_prot_params.z_rrlp_params.b_RefNum = -1;
  z_supl_session_status.wait_for_write_ack = FALSE;
  z_supl_session_status.supl_msg_q_status = FALSE;

  (void) os_TimerStop(l1_mrl_req_timer);
  (void) os_TimerStop(supl_lpp_otdoa_req_timer);
  (void) os_TimerStop(supl_lpp_otdoa_mul_meas_timer);
  (void) os_TimerStop(supl_lpp_early_response_timer);

  memset( &z_DBCellMeas, 0, sizeof (wl1_lsm_cell_meas_update_type) );

  #ifdef FEATURE_CGPS_LTE_CELLDB
  memset (&z_lte_mrl, 0, sizeof(lte_ml1_gps_mrl_cell_meas_update_t));
  #endif

  supl_SET_id_get();
  #if defined (FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
  if (b_supl_lpp_tx_mgr_init == TRUE) 
  {
    tm_lpp_tx_mgr_init(supl_lpp_handle); /* initialize all LPP transactions */
  }
  #endif /* FEATURE_CGPS_LTE_CELLDB && !FEATURE_GNSS_LOW_MEMORY */

  if(u_supl_end_waiting)
  {
    z_supl_session_status.e_cf_state = C_SUPL_CF_STATE_UT0_SUPLEND;
}
  else
  {
    tm_umts_up_supl_terminate_timer_cancel_cf_state ( C_SUPL_CF_STATE_NULL );
  }
}

/*===========================================================================

FUNCTION tm_umts_up_supl_get_accuracy_threshold

DESCRIPTION
  This function informs the TM core that network communication is over.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static uint32 tm_umts_up_supl_get_accuracy_threshold
(
  uint32  q_hor_acc_meters,
  boolean *p_multi_report_msa
)
{
  tm_cm_phone_state_info_s_type  phone_state_info;
  supl_status_s_type            *p_supl_session_status = &z_supl_session_status;

  tm_cm_iface_get_phone_state(&phone_state_info);

  if ((phone_state_info.e911_state != TM_CM_E911_STATE_NONE) ||
      ( z_umts_nv_item_ram_copy.aagps_development_test_control5 &
         NV_AAGPS_TEST5_EMERGENCY_CALL_SIM ))
  {
    /* For simualted E911 callflow, run to timeout by requesting periodic measurment report
       for msa & accuracy to zero for msb */
    *p_multi_report_msa = TRUE;
    q_hor_acc_meters = 0;
  }

  /* Non E911 callflow. Use the hor_accuracy provided by user */
  return q_hor_acc_meters;
}


/*===========================================================================

FUNCTION tm_umts_up_supl_inform_core_nt_assist_end

DESCRIPTION
  This function informs the TM core that network communication is over.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/

void tm_umts_up_supl_inform_core_nt_assist_end()
{
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;

  tm_sess_req_param_u_type req_param;
  tm_sess_req_param_u_type *p_req_param = &req_param;

  /* Query TM-CM module to get current acquired system */
  MSG_MED("TM_LPP_CP: inform_core_nt_assist_end", 0, 0, 0);
  if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801)
  {
    (void)supl_stop_is801_session( TM_STOP_REASON_GENERAL_ERROR );
    return;
  }

  /* if the association of GLONASS SV_ID and frequency established from multiple RRLP messages */
  if ( tm_umts_up_supl_rrlp_assisted_glonass_enabled() )
  {
    tm_post_data_payload_type    z_tm_post_data_payload;
    uint8 u_i;
    uint32 q_eph_injected_svs = 0;
    uint32 u_num_eph_injected = 0;

    /* No more assistance expected. Inject any buffered assistance data */
    for ( u_i = 0; u_i < N_GLO_SV; u_i++)
    {
      if ( ( p_supl_session_status->z_prot_glo_eph_buffer[u_i].z_eph_glo_data.u_SvId > GLO_SLOTID_OFFSET  ) &&
           ( p_supl_session_status->z_prot_glo_eph_buffer[u_i].z_eph_glo_data.u_SvId < (N_GLO_SV+GLO_SLOTID_OFFSET+1) ) &&
           ( p_supl_session_status->z_prot_glo_eph_buffer[u_i].u_freq_num_valid == TRUE)
         )
      {

        /* if this SV already has freq. number, inject it now;
           otherwise keep data in the buffer and inject when all
           assistance have been delivered */
        tm_rrlp_glo_eph_to_TmCore_post_data_payload
               (&p_supl_session_status->z_prot_glo_eph_buffer[u_i],
               &z_tm_post_data_payload );

        if ( tm_post_data ( TM_PRTL_TYPE_UMTS_UP,
                            p_supl_session_status->tm_active_session_handle,
                            TM_POST_DATA_TYPE_EPH_GLO,
                            &z_tm_post_data_payload
                          ) == FALSE
           )
        {
          MSG_HIGH("Glonass eph. data post failed: SvID=%u",
                    p_supl_session_status->z_prot_glo_eph_buffer[u_i].z_eph_glo_data.u_SvId,
                    0,
                    0);
        }
        else
        {
          u_num_eph_injected++;
          q_eph_injected_svs |=
            1L << ( p_supl_session_status->z_prot_glo_eph_buffer[u_i].z_eph_glo_data.u_SvId - GLO_SLOTID_OFFSET - 1 );
        }

        /* once processed, remove it from the buffer */
        /* 0 would be an invalid number because the api uses 65 and up for Glonass */
        p_supl_session_status->z_prot_glo_eph_buffer[u_i].z_eph_glo_data.u_SvId = 0;
        p_supl_session_status->z_prot_glo_eph_buffer[u_i].u_freq_num_valid = FALSE;

      } /* if this SV already has freq. number */

    } /* Glonass SV for-loop */

    if ( u_num_eph_injected != 0 )
    {
      MSG_MED("SUPL GLONASS eph posted to TmCore (assist end ) 0x%lx, totoal SVs = %u",
              q_eph_injected_svs, u_num_eph_injected, 0 );
    }
  }

  if ( (( p_supl_session_status->qos_timer_started ) ||
       ( p_supl_session_status->u_pdapi_fix_reported )) &&
      (p_supl_session_status->u_position_protocol != TM_POS_PRTL_LPP)
     )
  {
    /* qos_time has started, or fix has been achieved, no need to continue */
    return;
  }
  p_supl_session_status->qos_timer_started = TRUE;

  /* SUPL does not support "multiple MT reporting" */
  p_req_param->continue_param.multi_report_msa = FALSE;

  if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED )
  {
    p_req_param->continue_param.op_mode = TM_SESS_OPERATION_MODE_MSA;
    p_req_param->continue_param.op_req = TM_OP_REQ_PRM;
    p_req_param->continue_param.prm_qos =
      p_supl_session_status->z_session_qos.u_gps_session_timeout_value_secs;

    /* decide if we need to run to timeout or till QoS is met */
    p_req_param->continue_param.accuracy_threshold =
      tm_umts_up_supl_get_accuracy_threshold(
                        p_supl_session_status->z_session_qos.q_hor_acc_meters,
                       &p_req_param->continue_param.multi_report_msa);

    MSG_MED("SUPL: ntwk over. prm_qos=%u, acc=%lu",
          p_req_param->continue_param.prm_qos,
          p_req_param->continue_param.accuracy_threshold,
            0);
  }
  else if ( p_supl_session_status->u_agps_mode == C_SUPL_OTDOA_MODE )
  {
    p_req_param->continue_param.op_mode = TM_SESS_OPERATION_MODE_OTDOA;
    p_req_param->continue_param.op_req = TM_OP_REQ_OTDOA;
    p_req_param->continue_param.dynamic_qos_enabled = TRUE;

    p_req_param->continue_param.lr_qos =
      p_supl_session_status->z_session_qos.u_gps_session_timeout_value_secs;

    p_req_param->continue_param.accuracy_threshold = p_supl_session_status->z_session_qos.q_hor_acc_meters;

    MSG_MED("SUPL: ntwk over. lr_qos=%u, acc=%lu",
          p_req_param->continue_param.lr_qos,
          p_req_param->continue_param.accuracy_threshold,
            0);
  }
  else
  {
    p_req_param->continue_param.op_mode = TM_SESS_OPERATION_MODE_MSB;
    p_req_param->continue_param.op_req = TM_OP_REQ_LR;
    p_req_param->continue_param.dynamic_qos_enabled = TRUE;

    p_req_param->continue_param.lr_qos =
      p_supl_session_status->z_session_qos.u_gps_session_timeout_value_secs;

    /* decide if we need to run to timeout or till QoS is met */
    p_req_param->continue_param.accuracy_threshold =
      tm_umts_up_supl_get_accuracy_threshold(
                        p_supl_session_status->z_session_qos.q_hor_acc_meters,
                       &p_req_param->continue_param.multi_report_msa);

    MSG_MED("SUPL: ntwk over. lr_qos=%u, acc=%lu",
          p_req_param->continue_param.lr_qos,
          p_req_param->continue_param.accuracy_threshold,
            0);
  }



    (void) tm_sess_req ( TM_PRTL_TYPE_UMTS_UP,
                         p_supl_session_status->tm_active_session_handle,
                         TM_SESS_REQ_CONTINUE,
                         p_req_param
                       );

}



/*===========================================================================

FUNCTION tm_umts_up_supl_start_slp_contact

DESCRIPTION
  Knowing that the SET needs SLP assistance, this function start the contact.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/

void tm_umts_up_supl_start_slp_contact()
{
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;
  pd_comms_return_type          pdcomm_connect_ret_value;
  supl_callflow_kind_enum_type  u_callflow_type = p_supl_session_status->u_callflow_kind;
  pdsm_pd_comm_protocol_e_type protocol_type = PDSM_PD_COMM_PROTOCOL_UMTS_UP_SUPL;

  tm_cm_phone_state_info_s_type phone_state_info;

  tm_cm_iface_get_phone_state(&phone_state_info);


  /* Can't run SUPL over LTE if SUPL major version less than 2 */
  if ( (phone_state_info.srv_system == CGPS_SRV_SYS_LTE ) &&
       ( (p_supl_session_status->q_supl_version & 0xFF) < 2 ) &&
       ( u_callflow_type == C_SUPL_SESSION_UI )
     )
  {
    tm_umts_up_supl_callflow_disrupt_handler();

    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "LTE not supported on SUPL 1.0: %u,%lx",
          phone_state_info.srv_system, p_supl_session_status->q_supl_version );
    return;
  }

  if( (z_pdcomm_tcp_info.curr_state == PDCOMM_STATE_CONNECTED) ||
      (z_pdcomm_tcp_info.curr_state == PDCOMM_STATE_CONNECTING) ||
    (z_pdcomm_tcp_info.curr_state == PDCOMM_STATE_DISCONNECTING)
    )
  {
    /*TCP is already connected or connecting*/
    if( (z_pdcomm_tcp_info.emergency_connection == TRUE) ||
        (supl_session_is_emergency_session() == TRUE)
      )
    {
      /*Either previous or this session is an emergency session
            Hence we can not reuse the TCP connection
        */
      /*Mark the session state as pending for TCP connection*/
      p_supl_session_status->u_tcp_connection_attempt_pending = TRUE;

      MSG_HIGH("Marking connection pending in State %x", p_supl_session_status->e_cf_state,0,0);
      return;
    }
  }

  /* start SLP contact */
  MSG_MED("Start SLP contact",0,0,0);

  /*Reset Emergency connection boolean*/
  z_pdcomm_tcp_info.emergency_connection = FALSE;

  /* LSB in the variable passed contains "version number main" */ 
  pdcomm_connect_ret_value = 
    tm_umts_up_supl_comm_tcp_connect((uint8)(z_supl_session_status.q_supl_version & 0x000000FF));

  if (phone_state_info.srv_system == CGPS_SRV_SYS_LTE )
  {
    protocol_type = PDSM_PD_COMM_PROTOCOL_LTE_UP;
  }
  if ( PDCOMM_RESULT_OK == pdcomm_connect_ret_value ) /* Already got connected */
  {
    /* tell TmCore to send pdapi event of EventGpsPdConnectionStart and
       EventGpsPdConnectionEstablished */
    tm_umts_common_send_pdapi_comm_event
    (  protocol_type,
       C_COMM_BEGIN,
       p_supl_session_status->tm_active_session_handle,
       p_supl_session_status->q_pdapi_client_id
    );

    tm_umts_common_send_pdapi_comm_event
    (  protocol_type,
       C_COMM_CONNECTED,
       p_supl_session_status->tm_active_session_handle,
       p_supl_session_status->q_pdapi_client_id
    );

    if ( u_callflow_type == C_SUPL_SESSION_UI )
    {
      if ( supl_is_wifi_scan_needed() )
      {
        /* Send PDSM_PD_EVENT_REQUEST_WIFI_INFO_ON_DEMAND_INJECTION to pdapi client
           Start timer to wait for the scan result */
        supl_request_wifi_scan();
        supl_set_cf_state(C_SUPL_CF_STATE_WIFI_WAIT_TIMER_SUPLSTART, 0);
      }
      else
      {
      if ( !supl_SuplStart_tx())
      {
        tm_umts_up_supl_callflow_disrupt_handler();
      }
    }
    }
    else if ( u_callflow_type == C_SUPL_SESSION_NI )
    {

      /* Can't run SUPL over LTE if SUPL major version less than 2,
         even though previous negotiation with SUPL_INIT has resulted
         in running SUPL 1.0 */
      if ( (phone_state_info.srv_system == CGPS_SRV_SYS_LTE ) &&
           ( (p_supl_session_status->q_supl_version & 0xFF) < 2 )
         )
      {
        /* send SUPL_END, using the provisioned SUPL version in its
           message header, and abort this session */

        p_supl_session_status->q_supl_version =
         ( ( z_umts_nv_item_ram_copy.gnss_supl_version >> 16) & 0x000000FF ) |
         ( z_umts_nv_item_ram_copy.gnss_supl_version & 0x0000FF00 ) |
         ( ( z_umts_nv_item_ram_copy.gnss_supl_version << 16 ) & 0x00FF0000 );

        (void) tm_umts_up_supl_supl_end_tx(
                 &p_supl_session_status->z_session_id.z_Slp_Sess_Id,
                 NULL,
                 supl_versionNotSupported,
                 &p_supl_session_status->supl_init_hash[0],
                 p_supl_session_status->q_supl_version);

        tm_umts_up_supl_comm_close_connection();
        tm_umts_up_supl_callflow_disrupt_handler();

         return;
      }

      if ( supl_is_wifi_scan_needed() )
      {
        /* Send PDSM_PD_EVENT_REQUEST_WIFI_INFO_ON_DEMAND_INJECTION to pdapi client
           Start timer to wait for the scan result */
        supl_request_wifi_scan();
        supl_set_cf_state(C_SUPL_CF_STATE_WIFI_WAIT_TIMER_SUPLPOSINIT, 0);
      }
      else
      {
      if ( !supl_SuplPosInit_tx())
      {
        tm_umts_up_supl_callflow_disrupt_handler();
      }
      else /*SuplPosInit Sent*/
      {
        if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801)
        {
          /*Start IS801 state machine*/
          if( !supl_start_is801_session(TRUE, NULL))
          {
            /*Unable to start IS801 state machine*/
            tm_umts_up_supl_callflow_disrupt_handler();
          }
        }
      }
      } /* else if no wifi scan needed */
    } /* if NI */
  } /*Already got connected*/

  else if ( PDCOMM_RESULT_WAIT == pdcomm_connect_ret_value ) /* most likely */
  {
    if ( u_callflow_type == C_SUPL_SESSION_UI )
    {
      supl_set_cf_state(C_SUPL_CF_STATE_UT0_SUPLSTART, 0);
      p_supl_session_status->u_network_session_on = TRUE;
      MSG_MED("SUPL: UT0_SuplStart starts", 0, 0, 0);
    }
    else if ( u_callflow_type == C_SUPL_SESSION_NI )
    {
      supl_set_cf_state(C_SUPL_CF_STATE_UT0_SUPLPOSINIT, 0);
      MSG_MED("SUPL: UT0_SuplPosInit starts", 0, 0, 0);
    }

    /* tell TmCore to send pdapi event of EventGpsPdConnectionStart */
    tm_umts_common_send_pdapi_comm_event
    (  protocol_type,
       C_COMM_BEGIN,
       p_supl_session_status->tm_active_session_handle,
       p_supl_session_status->q_pdapi_client_id
    );


    if ( supl_is_wifi_scan_needed() )
    {
      /* Send PDSM_PD_EVENT_REQUEST_WIFI_INFO_ON_DEMAND_INJECTION to pdapi client
         Start timer ( | on top of the existing UT) to wait for the scan result */
       supl_request_wifi_scan();
       supl_set_cf_state(C_SUPL_CF_STATE_WIFI_WAIT_TIMER_NO_TCP, 0);
  }

  }

  else
  {
    /* not possible to connect at this point */
    tm_umts_up_supl_callflow_disrupt_handler();
    MSG_MED("SUPL: TCP conn. failed", 0, 0, 0);
  }
}


/*
******************************************************************************
* tm_umts_up_supl_suplpos_rrlp_mpr_err_tx
*
* Function description:
*
* Encode and transmit SUPL POS carrying RRLP-measurePosResp-error
*
* Parameters: NONE.
*
* Return value: TRUE or FALSE
*
******************************************************************************
*/

boolean tm_umts_up_supl_suplpos_rrlp_mpr_err_tx ( rrlp_LocErrorReason e_error_reason )
{


  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;
  boolean u_ret = FALSE;
  tm_pdu_buf_type   pdu_buff; /* including length and pointer to data */
  rrlp_PDU z_rrlp;
  rrlp_MsrPosition_Rsp z_rrlp_MsrPosition_Rsp;
  rrlp_MsrPosition_Rsp *p_mpr;
  int encode_status;


  memset(&z_rrlp, 0, sizeof(rrlp_PDU));
  memset(&z_rrlp_MsrPosition_Rsp, 0, sizeof(rrlp_MsrPosition_Rsp));
  memset(&pdu_buff, 0, sizeof(tm_pdu_buf_type));

  z_rrlp.component.u.msrPositionRsp = &z_rrlp_MsrPosition_Rsp;
  p_mpr = &z_rrlp_MsrPosition_Rsp;

  /* ref. num: the same as the one came from SMLC */
  z_rrlp.referenceNumber =
    p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.b_RefNum;

  z_rrlp.component.t = rrlp_msrPositionRsp_chosen;

  p_mpr->m.locationErrorPresent = 1;
  p_mpr->locationError.m.additionalAssistanceDataPresent = 0;
  p_mpr->locationError.locErrorReason = e_error_reason;

  /* Do the ASN.1 encoding for RRLP */
  encode_status = tm_osys_asn1_encode_pdu((void *)&z_rrlp, &pdu_buff, PROTO_RRLP);
  if ( (encode_status != 0) || (pdu_buff.value == NULL ) || (pdu_buff.length == 0) )
  {
    /* ASN.1 encoding failed */
    if (pdu_buff.value != NULL )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRLP ASN1 enc. err. Freeing: %d,%lx,%u",
            encode_status, pdu_buff.value, pdu_buff.length);

      tm_asn1_free_osys_buf( pdu_buff.value, PROTO_RRLP );
      pdu_buff.value = NULL;
    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRLP ASN1 enc. err. No alloc.: %d,%lx,%u",
            encode_status, pdu_buff.value, pdu_buff.length);
    }
    return FALSE;
  }

  tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER );

  if ( supl_SuplPos_rrlp_tx (pdu_buff.value, (uint16)pdu_buff.length, NULL) )
  {
    if ( e_error_reason != rrlp_notEnoughSats )
    {
      /* if not the final no-fix report, expect the SLP to correct its errors in
         the current SuplPos-rrlp-measurePosRequest */
      supl_set_cf_state(C_SUPL_CF_STATE_UT3_RRLP, 0);
    }
    else
    {
      /* if final no-fix report, give the SLP a chance to send fix from other sources */
      supl_set_cf_state(C_SUPL_CF_STATE_UT3, 0);
    }
    u_ret = TRUE;
  }
  else
  {
    u_ret = FALSE;
  }

  /*Free the encoded PDU*/
  tm_asn1_free_osys_buf( pdu_buff.value, PROTO_RRLP );
  pdu_buff.value = NULL;

  return u_ret;

}
/********************************************************************
*
* tm_umts_up_supl_send_comm_failure
*
* Function description:
*   this function sends comm failure events.
*
* Parameters:
*   None
*
* Return:
*  None
*
**********************************************************************/
void tm_umts_up_supl_send_comm_failure( )
{
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;

  z_supl_pd_info_buffer.client_id = p_supl_session_status->q_pdapi_client_id;
  (void)tm_post_pd_event ( TM_PRTL_TYPE_UMTS_UP,
                           p_supl_session_status->tm_active_session_handle,
                           PDSM_PD_EVENT_COMM_FAILURE,
                           &z_supl_pd_info_buffer,
                           NULL);

  (void)tm_post_pd_event ( TM_PRTL_TYPE_UMTS_UP,
                           p_supl_session_status->tm_active_session_handle,
                           PDSM_PD_EVENT_COMM_DONE,
                           &z_supl_pd_info_buffer,
                           NULL);
}

/********************************************************************
*
* tm_umts_up_supl_callflow_disrupt_handler
*
* Function description:
*   this function properly handles SUPL standard "UT 1-3" expiration.
*
* Parameters:
*   None
*
* Return:
*  None
*
*********************************************************************
*/

void tm_umts_up_supl_callflow_disrupt_handler( )
{
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;

  if ( p_supl_session_status->e_cf_state != C_SUPL_CF_STATE_NULL )
  {
    if ( ! (p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLEND ) )
    {
      tm_umts_up_supl_terminate_timer_cancel_cf_state ( C_SUPL_CF_STATE_NULL );

      /* close data link if connected or connecting */
      tm_umts_up_supl_comm_close_connection();
    }

    else
    {
      /* still waiting for sending SUPL_END, but clear all other states */
      p_supl_session_status->e_cf_state = C_SUPL_CF_STATE_UT0_SUPLEND;
    }
  }

  /*If its UI SUPL-IS801 optimal modes session, then let IS801 take care of the error */
  if ( (p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801) &&
       ( p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_UI ) &&
       ( p_supl_session_status->tm_sess_req_param.pd_option.operation_mode > PDSM_SESSION_OPERATION_MSASSISTED) &&
       ( p_supl_session_status->tm_sess_req_param.pd_option.operation_mode < PDSM_SESSION_OPERATION_REF_POSITION)
     )
  {
     (void)supl_stop_is801_session(TM_STOP_REASON_GENERAL_ERROR);
     (void) os_TimerStop(l1_mrl_req_timer);
     return;
  }

  if ( ( (p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_BASED ) &&
         ( p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_UI )
       ) ||
       ( tm_core_is_internal_session_on() == TRUE )
    )
  {

    z_supl_pd_info_buffer.client_id = p_supl_session_status->q_pdapi_client_id;

    (void) tm_post_pd_event ( TM_PRTL_TYPE_UMTS_UP,
                              p_supl_session_status->tm_active_session_handle,
                              PDSM_PD_EVENT_UPDATE_FAILURE,
                              &z_supl_pd_info_buffer,
                              NULL);

    p_supl_session_status->u_network_session_on = FALSE;
    if ((!p_supl_session_status->u_pdapi_fix_reported ) &&
        (!p_supl_session_status->supl_close_waiting ) &&
        (p_supl_session_status->u_position_protocol != TM_POS_PRTL_LPP))
    {
      tm_umts_up_supl_inform_core_nt_assist_end();
    }
    else if ( (!p_supl_session_status->u_pdapi_fix_reported ) &&
              (!p_supl_session_status->supl_close_waiting ) &&
         ((p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP) &&
          (p_supl_session_status->z_supl_lpp_data.b_tm_session_done != TRUE)))
    {
      tm_umts_up_supl_inform_core_nt_assist_end();
    }
    else
    {
      tm_sess_stop_reason_e_type stop_reason;
      if(!p_supl_session_status->u_pdapi_fix_reported )
      {
        stop_reason = TM_STOP_REASON_GENERAL_ERROR;
      }
      else
      {
        /*MO MSB and fix is already reported. Call it as succesfull session*/
        stop_reason = TM_STOP_REASON_COMPLETED;
      }

      tm_umts_up_supl_inform_core_stop_session(stop_reason);
      tm_umts_up_supl_session_init(FALSE);
    }
  }
  else if ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP )
  {
    tm_umts_up_supl_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
    tm_umts_up_supl_session_init(FALSE);

  }
  else
  {
    tm_umts_up_supl_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
    tm_umts_up_supl_session_init(FALSE);
  }

  (void) os_TimerStop(l1_mrl_req_timer);

}


/********************************************************************
*
* tm_umts_up_supl_terminate_timer_cancel_cf_state
*
* Function description:
*   this function terminates SUPL timer(s) and cancels either one
*   call flow state, or all of them.
*
* Parameters:
*   e_timer_state_to_terminate - the call flow state to cancel; if
*     C_SUPL_CF_STATE_NULL, cancel all.
*
* Return:
*  None
*
*********************************************************************
*/

void tm_umts_up_supl_terminate_timer_cancel_cf_state( supl_cf_state_enum_type e_timer_state_to_terminate )
{
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;
  pending_supl_end     *p_pending_supl_end = &z_pending_supl_end[0];

  if ( e_timer_state_to_terminate == C_SUPL_CF_STATE_WIFI_WAIT_TIMER_NO_TCP )
  {
    (void) os_TimerStop(supl_wifi_wait_timer);
  }
  else if (e_timer_state_to_terminate == C_SUPL_CF_STATE_OTDOA_EARLY_RESPONSE_REQ)
  {
     /*mark the EarlyResponse needed flag to false so that next 
       LPP message does not go out as early response.*/
     p_supl_session_status->z_supl_lpp_data.b_earlyResponse = FALSE;
     (void) os_TimerStop(supl_lpp_early_response_timer);
  }
  else
  {
    (void) os_TimerStop( supl_cf_timer );
  }


  if ( e_timer_state_to_terminate & C_SUPL_CF_STATE_OTDOA_MEAS_REQ  )
  {
    (void)os_TimerStop(supl_lpp_otdoa_req_timer);    
  }
  
  if ( C_SUPL_CF_STATE_OTDOA_MUL_MEAS_RECV & e_timer_state_to_terminate )
  {
    (void)os_TimerStop(supl_lpp_otdoa_mul_meas_timer);
  }

  /* if terminating UT0-SuplEnd, clear all the pending SuplEnd in the que */
  if ( e_timer_state_to_terminate & C_SUPL_CF_STATE_UT0_SUPLEND  )
  {
    memset ( p_pending_supl_end, 0, sizeof(pending_supl_end) * C_MAX_NUM_PENDING_SUPL_END );
  }

  if ( e_timer_state_to_terminate != C_SUPL_CF_STATE_NULL )
  {
    p_supl_session_status->e_cf_state &= (supl_cf_state_enum_type)~e_timer_state_to_terminate;
  }
  else
  {
    memset ( p_pending_supl_end, 0, sizeof(pending_supl_end) * C_MAX_NUM_PENDING_SUPL_END );
    p_supl_session_status->e_cf_state = C_SUPL_CF_STATE_NULL;
  }
}



/********************************************************************
*
* tm_umts_up_supl_pdapi_mapping
*
* Function description:
*   this function interprets PDAPI provided session parameters for
*   the SUPL protocol module.
*
* Parameters:
*   pointer to structure containing PDAPI session parameters.
*
* Return:
*  None
*
*********************************************************************
*/

boolean tm_umts_up_supl_pdapi_mapping (prtl_start_sess_req_param_s_type *start_sess_req_param)
{
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;
  boolean u_ret_val = TRUE;
  tm_cm_phone_state_info_s_type phone_state_info;
  boolean is_lpp_enabled = FALSE;
  boolean no_sim_found = FALSE;

  if ( NULL == start_sess_req_param )
  {
    MSG_HIGH("Null session req param", 0, 0, 0);
    return FALSE;
  }
  
  tm_cm_iface_get_phone_state( &phone_state_info );


  /* check for conditions that SUPL cannot run */
  if ( z_Set_Id.set_id_choice == C_SETID_UNKNOWN )
  {
     supl_SET_id_get();
    /* Retrieve the IMSI/IMEI value from the USIM*/
    if ( z_Set_Id.set_id_choice == C_SETID_UNKNOWN )
    {
      no_sim_found = TRUE; /* SUPL can't run: no IMSI hence no SIM card */
    }
  }

  if ( no_sim_found )
  {
    if ( start_sess_req_param->pd_option.operation_mode == PDSM_SESSION_OPERATION_MSASSISTED )
    {

      MSG_MED("No IMSI, can't run MSA", 0, 0, 0);
      return FALSE;
    }
    else
    {
      start_sess_req_param->pd_option.operation_mode =
        PDSM_SESSION_OPERATION_STANDALONE_ONLY;

      MSG_MED("No IMSI, default to standalone", 0, 0, 0);

    }
  }


  /* If provisioned not to support AGPS in GSM or WCDMA for SUPL */
  if ( ( (phone_state_info.srv_system == CGPS_SRV_SYS_GSM ) &&
         !( z_umts_nv_item_ram_copy.mo_lr_2g_support &
            NV_AAGPS_2G_MO_LR_HIGH_ACCURACY
          )
       )   ||
       ( (phone_state_info.srv_system == CGPS_SRV_SYS_WCDMA ) &&
         !( z_umts_nv_item_ram_copy.mo_lr_support &
            NV_AAGPS_3G_MO_LR_HIGH_ACCURACY
          )
       )
     )
  {
    start_sess_req_param->pd_option.operation_mode =
      PDSM_SESSION_OPERATION_STANDALONE_ONLY;

    MSG_MED("G/W AGPS provisioned out, default to standalone", 0, 0, 0);

  }

  if (( phone_state_info.srv_system == CGPS_SRV_SYS_LTE) && (tm_core_get_lte_lpp_up_enable() == TRUE) )
  {
    is_lpp_enabled = TRUE;
  }


  if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UNDEF )
  {

    if ( start_sess_req_param->pd_option.session == PDSM_PD_SESS_TYPE_DATA_DLOAD )
    {
      if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_UP_MS_BASED | PDSM_POS_MODE_4G_UP_MS_BASED ) != 0)
      {
        p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_BASED;
      }
      else
      {
        u_ret_val = FALSE;
      }
    }

    else if ( start_sess_req_param->pd_option.session == PDSM_PD_SESS_TYPE_NEW ||
             start_sess_req_param->pd_option.session == PDSM_PD_SESS_TYPE_TRACK_IND )
    {
      /* if the mode is not undef at this point, this is an MT session, and the mode has been
         set when the request is accepted. Going through the "NT client", from TmCore,
         start_sess_req_param->pd_option.operation_mode would be either MSBASED or MSASSISTED,
         and would not contain AUTONUMOUS. So if this mode has been set already, don't alter it */

      if ( ( start_sess_req_param->pd_option.operation_mode == PDSM_SESSION_OPERATION_MSBASED ) ||
           ( start_sess_req_param->pd_option.operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_SPEED )
         )
      {
        if ( ( is_lpp_enabled == TRUE ) && TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_4G_UP_MS_BASED) )
        {

          p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_BASED;
        }
        else if ( (is_lpp_enabled == FALSE) && 
                  TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_UP_MS_BASED) ) 
        {
          p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_BASED; 
        }
        else if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_STANDALONE) )
        {
          p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_AUTO;
        }
        else
        {
          u_ret_val = FALSE;
        }
      }
      else if ( ( start_sess_req_param->pd_option.operation_mode == PDSM_SESSION_OPERATION_MSASSISTED ) ||
                ( start_sess_req_param->pd_option.operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_ACCURACY )
              )
      {

        if ( ( is_lpp_enabled == TRUE ) &&
             TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_4G_UP_MS_ASSISTED) )
        {
          if (start_sess_req_param->pd_qos.gps_session_timeout == 0)
          {
            p_supl_session_status->u_agps_mode = C_SUPL_OTDOA_MODE;
          }
          else
          {
            p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_ASSISTED;
          }
        }
        else if ( is_lpp_enabled == FALSE )
        {
          if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_UP_MS_ASSISTED) )
          {
            p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_ASSISTED;
          }
        }
        else
        {
          u_ret_val = FALSE;
        }
      }
      else if  ( start_sess_req_param->pd_option.operation_mode == PDSM_SESSION_OPERATION_REF_POSITION )
      {
        if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_UP_MS_BASED | PDSM_POS_MODE_4G_UP_MS_BASED ) != 0)
        {
          p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_BASED;
          p_supl_session_status->u_force_request_ref_location = TRUE;
        }
        else
        {
          u_ret_val = FALSE;
        }

      }
      else if ( ( start_sess_req_param->pd_option.operation_mode == PDSM_SESSION_OPERATION_STANDALONE_ONLY ) ||
                ( start_sess_req_param->pd_option.operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_DATA )
              )
      {
        /* note standalone_only should not reach here as it is handled by TM core and is not passed down to
           protocol modules; but in case it gets here, handle it gracefully */
        if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_STANDALONE ) )
        {
          p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_AUTO;
        }
        else
        {
          if ( (start_sess_req_param->pd_option.operation_mode == PDSM_SESSION_OPERATION_STANDALONE_ONLY) &&
               (no_sim_found == TRUE) &&
               (TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_MSB_TO_STANDALONE_ALLOWED_IF_NO_SIM))
             )
          {
            p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_AUTO;
          }
          else
          {
          u_ret_val = FALSE;
        }
      }
      }
      else if ( start_sess_req_param->pd_option.operation_mode == PDSM_SESSION_OPERATION_CELL_ID )
      {
         p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_CELL_ID;
      }

      else
      {
        u_ret_val = FALSE;
      }

    } /* session == PDSM_PD_SESS_TYPE_NEW  */

    else /* all other session types */
    {
      u_ret_val = FALSE;
    }

  } /* if u_gps_mode undef */

  if ( u_ret_val )
  {
    p_supl_session_status->q_pdapi_client_id = start_sess_req_param->client_id;
    p_supl_session_status->z_session_qos.u_gps_session_timeout_value_secs = start_sess_req_param->pd_qos.gps_session_timeout;
    p_supl_session_status->z_session_qos.q_hor_acc_meters = start_sess_req_param->pd_qos.accuracy_threshold;

    if ( start_sess_req_param->pd_option.supl_application_id.u_validity &
         PDAPI_SUPL_APPLICATION_ID_VALID )
    {
      memscpy( &p_supl_session_status->z_supl_app_id,
              sizeof(p_supl_session_status->z_supl_app_id),
              &start_sess_req_param->pd_option.supl_application_id,
              sizeof(start_sess_req_param->pd_option.supl_application_id)
            );
    }

    if ( !( ( p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_NI ) &&
            ( p_supl_session_status->q_supl_version != 0 )
          )
       )
    {
      /* as long as this is not NI and supl version yet to be set, set supl
         version for this session now. Note for UI, call flow kind is not
         yet set at this point */
      p_supl_session_status->q_supl_version =
       ( ( z_umts_nv_item_ram_copy.gnss_supl_version >> 16) & 0x000000FF ) |
       ( z_umts_nv_item_ram_copy.gnss_supl_version & 0x0000FF00 ) |
       ( ( z_umts_nv_item_ram_copy.gnss_supl_version << 16 ) & 0x00FF0000 );
    }
  }

  return u_ret_val;

}

/*
 ******************************************************************************
 * Function description:
 *  This function handles the negotiation of SUPL version numbers between the
 *  SET and the SLP.
 *
 * Parameters:
 *
 *  p_pdu_decoded: pointer to decoded SLP message.
 *
 *  p_supl_version_for_calling_func: pointer for the function to populate for
 *    the calling function as to what version number to use.
 *
 *  u_session_num: applicable to areaEventTrigger session; use 0xFF otherwise.
 *
 * Return value:
 *
 *    slp_mess_header_check_result_e_type: to indication if the SLP message is
 *      to be rejected due to version not supported.
 *
 *
 ******************************************************************************
*/

slp_mess_header_check_result_e_type supl_version_negotiation
( supl_ULP_PDU* p_pdu_decoded,
  uint32 *p_supl_version_for_calling_func,
  uint8  u_session_num )
{

  uint32 q_negotiated_supl_version;
  uint32 q_provisioned_supl_ver_maj;
  slp_mess_header_check_result_e_type   e_slp_mess_header_check_result;
  uint32 q_existing_session_supl_version;

  boolean v_is_LTE = FALSE;
  tm_cm_phone_state_info_s_type phone_state_info;

  if ( ( NULL == p_pdu_decoded ) ||  ( NULL == p_supl_version_for_calling_func ) )
  {
    return C_SLP_MESS_VERSION_CHECK_FAILED;
  }

  tm_cm_iface_get_phone_state(&phone_state_info);
  if ( phone_state_info.srv_system == CGPS_SRV_SYS_LTE )
  {
    v_is_LTE = TRUE;
  }
  else
  {
    v_is_LTE = FALSE;
  }

  e_slp_mess_header_check_result = C_SLP_MESS_HEADER_STATUS_UNKNOWN;

  if ( u_session_num >= C_MAX_GEOFENCE_NUM ) /* in which case it should be 0xFF */
  {
    q_existing_session_supl_version = z_supl_session_status.q_supl_version;
  }
  else
  {
    q_existing_session_supl_version = z_supl_area_event_session_status[u_session_num].q_supl_version;
  }

  /* incoming message SUPL major version number */
  q_negotiated_supl_version = p_pdu_decoded->version.maj;

  /* for min. and servind, only the SET provisioned number are to be used */
  q_negotiated_supl_version |=
       ( z_umts_nv_item_ram_copy.gnss_supl_version & 0x0000FF00 ) |
       ( ( z_umts_nv_item_ram_copy.gnss_supl_version << 16 ) & 0x00FF0000 );

  /********  SUPL version negotiation: ************************/

  q_provisioned_supl_ver_maj =
    (z_umts_nv_item_ram_copy.gnss_supl_version >> 16) & 0x000000FF;

  /* all non-SUPL_INIT messages */
  if ( p_pdu_decoded->message.t != C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLINIT )
  {
    if ( p_pdu_decoded->version.maj !=
         (q_existing_session_supl_version & 0x000000FF))
    {
      e_slp_mess_header_check_result |= C_SLP_MESS_VERSION_UNSUPPORTED;
    }
    q_negotiated_supl_version = q_existing_session_supl_version;
  } /* Non-SUPL_INIT */

  else  /* for SUPL_INIT */
  {
    /* note only when the device is provisioned to be SUPL2 or higher, can it
       examine the Ver2Extension of "minimumMajorVersion" */
    if ( p_pdu_decoded->version.maj > q_provisioned_supl_ver_maj )
    {

      /* regardless if the SUPL_INIT can be entertained, device will respond
         with provisioned SUPL version number */
      q_negotiated_supl_version =
       ( ( z_umts_nv_item_ram_copy.gnss_supl_version >> 16) & 0x000000FF ) |
       ( z_umts_nv_item_ram_copy.gnss_supl_version & 0x0000FF00 ) |
       ( ( z_umts_nv_item_ram_copy.gnss_supl_version << 16 ) & 0x00FF0000 );


      if ( !( ( q_provisioned_supl_ver_maj >= 2 ) &&
              p_pdu_decoded->message.u.msSUPLINIT->m.ver2_SUPL_INIT_extensionPresent &&
              p_pdu_decoded->message.u.msSUPLINIT->ver2_SUPL_INIT_extension.m.minimumMajorVersionPresent &&
              ( p_pdu_decoded->message.u.msSUPLINIT->ver2_SUPL_INIT_extension.minimumMajorVersion <=
                q_provisioned_supl_ver_maj
              )
            )
         )
      {
        MSG_HIGH("SUPL version Major Ver. higher than supported", 0, 0,0);
        e_slp_mess_header_check_result |= C_SLP_MESS_VERSION_UNSUPPORTED;
      }
      q_supl_ver_maj_after_supl_init_glob = q_negotiated_supl_version; /* NV provisioned version used */

      if ( (q_negotiated_supl_version < 2) && v_is_LTE ) 
      {

        /* SUPL 1 does not support LTE: If after applying existing version negotiation,
           the agreed-on version number is lower than SUPL2, and it is on LTE, mark version
           number as 0 so that the call function will not attempt any SUPL activities. */
        q_negotiated_supl_version = 0;  
      }

    } /* SUPL_IINT's version > device provisioned version */

    else /* SUPL_INIT's version <= device provisioned version */
    {

      if ( (p_pdu_decoded->version.maj < 2) && v_is_LTE ) 
      {
        /* even if provisioned version is higher than SUPL_INIT's, if SUPL_INIT's asks for SUPL 1 and the device
           is camped on LTE, treat it as versionNotSupported with a SUPL_END of the provisioned version */
        e_slp_mess_header_check_result |= C_SLP_MESS_VERSION_UNSUPPORTED; 

        /* if provisioned major version is less than 2, in LTE, no SUPL is supported, mark the negotiated version to 0 */
        if ( q_provisioned_supl_ver_maj < 2 ) 
        {
          q_negotiated_supl_version = 0; /* the incoming SUPL_INIT is to be ignored */
        }
        else /* otherwise, respond with the provisioned version */
        {
          q_negotiated_supl_version =
           ( ( z_umts_nv_item_ram_copy.gnss_supl_version >> 16) & 0x000000FF ) |
           ( z_umts_nv_item_ram_copy.gnss_supl_version & 0x0000FF00 ) |
           ( ( z_umts_nv_item_ram_copy.gnss_supl_version << 16 ) & 0x00FF0000 );

          q_supl_ver_maj_after_supl_init_glob = q_negotiated_supl_version; /* SUPL_INIT's SUPL ver used */
        }

        MSG_HIGH("LTE not supported by SUPL 1: %u, %u, %lx", 
                 p_pdu_decoded->version.maj, 
                 q_provisioned_supl_ver_maj,
                 q_supl_ver_maj_after_supl_init_glob);

      } /* SUPL 1 and LTE */

      else /* normal case:  Not SUPL_INIT (1.x.x) in LTE */
      {
        q_supl_ver_maj_after_supl_init_glob = q_negotiated_supl_version; /* SUPL_INIT's SUPL ver used */
        MSG_HIGH("SUPL_INIT verMaj <= NV provisioned: %u, %u, %lx",
                  p_pdu_decoded->version.maj,
                  q_provisioned_supl_ver_maj,
                  q_supl_ver_maj_after_supl_init_glob);
      }

    } /* SUPL_INIT's version <= device provisioned version */

  } /* else : SUPL_INIT */

  *p_supl_version_for_calling_func = q_negotiated_supl_version;
  /* for the consumption of the calling func. which was defined by the adding
     global variable q_supl_ver_maj_after_supl_init_glob */

  return e_slp_mess_header_check_result;

}


/*
 ******************************************************************************
 * Function description:
 *  Decode ASN1 encoded SUPL messages coming from the TCP connection.
 *
 * Parameters:
 *
 *  Data_ptr: pointer to the lowest address byte in SUPL data buffer
 *
 *  w_data_len: length of the SUPL message
 *
 *
 * Return value:
 *
 *    TRUE: successfully docoded; FALSE: protocol error has occured.
 *
 *
 ******************************************************************************
*/

boolean  tm_umts_up_supl_message_decode ( uint8 *p_data, uint16 w_data_len )
{
  supl_ULP_PDU z_supl_pdu_decoded;
  supl_ULP_PDU* p_pdu_decoded = &z_supl_pdu_decoded;


  int asn1_decode_status;


  slp_mess_header_check_result_e_type   e_slp_mess_header_check_result = C_SLP_MESS_HEADER_STATUS_UNKNOWN;

  supl_slp_session_id  z_p_provided_slp_session_id;
  supl_slp_session_id* p_provided_slp_session_id = &z_p_provided_slp_session_id;

  uint32 q_supl_version = 0;

  tm_cm_phone_state_info_s_type phone_state_info;

  uint8 u_area_event_session_match_num = 0xFF;


  if ( NULL == p_data ) 
  {
    return FALSE;
  }


  memset(&z_supl_pdu_decoded, 0, sizeof(supl_ULP_PDU) );
  memset(p_provided_slp_session_id, 0, sizeof(supl_slp_session_id) );

  /*SUPL is not Enabled, ignore the message*/
  if(tm_umts_up_is_supl_enabled() == FALSE)
  {
    MSG_HIGH("SUPL message ignored: SUPL not Enabled", 0, 0, 0);
    return TRUE;
  }

  tm_osys_asn1_decode_pdu( p_data, w_data_len, (void *)p_pdu_decoded, PROTO_SUPL, &asn1_decode_status);


  /* log the message: note that the 2nd argument is not used other than for Snapper to
     display a corresponding string to indicate the "connection type", a concept that no longer
     is relevant. Snapper uses 5 for the string "SUPL" */
  sm_log_svr_txrx_rep( C_LSM_LOG_SVR_RX_TYPE, 5, C_LSM_SVR_PROTOCOL_TYPE_SUPL,
                       w_data_len, p_data );

  if ( asn1_decode_status == 0 )
  {
    /* find if the incoming message has SLP session ID identical to that of an existing area event triggered session */
    u_area_event_session_match_num = supl_area_event_session_match(&p_pdu_decoded->sessionID);

    if ( p_pdu_decoded->message.t != C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLINIT )
    {
      if ( ( z_supl_session_status.u_callflow_kind  == C_SUPL_SESSION_NONE ) &&
           ( u_area_event_session_match_num == 0xFF ) &&
           ( z_supl_session_status.e_cf_state == C_SUPL_CF_STATE_NULL )
         )
      {
        /* unexpected message, drop it */
        MSG_MED("SUPL message arrives unexpected, drop it", 0, 0, 0);
        tm_osys_asn1_free_pdu (p_pdu_decoded , PROTO_SUPL);
        return TRUE;
      }
    }

    tm_cm_iface_get_phone_state(&phone_state_info);

    /* If provisioned not to support AGPS in GSM or WCDMA for SUPL */
    if ( ( (phone_state_info.srv_system == CGPS_SRV_SYS_GSM ) &&
           ( z_umts_nv_item_ram_copy.mt_lr_support &
              NV_AAGPS_MT_LR_UP_GSM_WCDMA_BLOCKED
            )
         )   ||
         ( (phone_state_info.srv_system == CGPS_SRV_SYS_WCDMA ) &&
           ( z_umts_nv_item_ram_copy.mt_lr_support &
              NV_AAGPS_MT_LR_UP_GSM_WCDMA_BLOCKED
            )
         )
       )
    {
      MSG_MED("G/W AGPS provisioned out, SUPL_INIT ignored", 0, 0, 0);
      tm_osys_asn1_free_pdu (p_pdu_decoded , PROTO_SUPL);
      return TRUE; /* if SUPL NI for GSM/WCDMA is blocked by NV, drop SUPl_INIT */
    }

    if ( u_area_event_session_match_num == 0xFF )
    {
      /* Incoming message's SLP SessionID does not match any existing areaEvenTrigger session */
      e_slp_mess_header_check_result =  supl_message_overhead_proc(p_pdu_decoded, NULL);
    }
    else
    {
      /* Incoming message's SLP SessionID matches with an existing areaEvenTrigger session:  */
      if ( p_pdu_decoded->message.t != C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLINIT )
      {
      e_slp_mess_header_check_result = C_SLP_MESS_HEADER_OK_NEW;
    }
      else /* Need to send SUPL_END statusCode = sessionID mimatch */
      {
        MSG_HIGH("Incoming SUPL_INIT SLP SessionID same as existing areaEvent: slot %u", u_area_event_session_match_num, 0, 0);
        e_slp_mess_header_check_result = C_SLP_MESS_SLP_SESS_ID_PROBLEM_SUPLINIT_EXISTING_AREA_EVENT;
      }
    }
    e_slp_mess_header_check_result |= supl_version_negotiation(p_pdu_decoded,
                                                             &q_supl_version,
                                               u_area_event_session_match_num);

    /* if negotiated version is SUPL 1.0 and camped on LTE, the negotiation function would
       indicate q_supl_version being 0, drop the message */   
    if ( ( q_supl_version == 0 )  || 
         ( e_slp_mess_header_check_result & C_SLP_MESS_VERSION_CHECK_FAILED )
       )
    {
      MSG_MED("SUPL 1.0 attempt over LTE, or version check failed: SUPL_INIT ignored - %u, %lx", 
              q_supl_version, e_slp_mess_header_check_result, 0); 
      tm_osys_asn1_free_pdu (p_pdu_decoded , PROTO_SUPL);  
      return TRUE; 
    }

    if ( p_pdu_decoded->message.t == C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLTRIGGEREDSTOP  )
    {
      /* do not take action of any sessionID consistency because such ID may be
         for an existing areaEvent session whose sessionIDs may not neccessarily be
         the same as the one in the SUPL_INIT(sessionInforQuery) */
      e_slp_mess_header_check_result &= ~C_SLP_MESS_SLP_SESS_ID_PROBLEM;
      e_slp_mess_header_check_result &= ~C_SLP_MESS_SET_SESS_ID_PROBLEM;
    }

    if ( !( e_slp_mess_header_check_result & C_SLP_MESS_SLP_SESS_ID_MISSING ) )
    {
      /* for the consumption of later SUPL_END */
      supl_util_server_session_id_organizer ( &p_pdu_decoded->sessionID, p_provided_slp_session_id);
    }

    if ( ( e_slp_mess_header_check_result == C_SLP_MESS_HEADER_OK_NEW ) ||
         ( e_slp_mess_header_check_result == C_SLP_MESS_HEADER_OK_CONSISTENT )
       )
    {

      if ( e_slp_mess_header_check_result == C_SLP_MESS_HEADER_OK_NEW )
      {
        if ( p_pdu_decoded->message.t != C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLTRIGGEREDSTOP )
        {
          supl_store_slp_sessionId(p_pdu_decoded); /* stores SLP session ID */
        }
      }

      if ( p_pdu_decoded->message.t == C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLINIT)
      {
        /* it should only enter here if the call to hash generation function
           returned FALSE; otherwise, SUPL_INIT should be decoded in tm_umts_up_supl_hash_proc() */
        (void) supl_SuplInit_proc( p_pdu_decoded, p_data, w_data_len, NULL );
      }
      else if ( p_pdu_decoded->message.t == C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLRESPONSE )
      {
        (void) supl_SuplResponse_proc( p_pdu_decoded );
      }
      else if ( p_pdu_decoded->message.t == C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLPOS)
      {
        (void) supl_SuplPos_proc( p_pdu_decoded );
      }
      else if ( p_pdu_decoded->message.t == C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLEND )
      {
        supl_SuplEnd_proc ( p_pdu_decoded );
      }
      else if ( p_pdu_decoded->message.t == C_SUPL_ASN1_CHOICE_ULP_MESSAGE_AUTHRESP )
      {
        //process_supl_authresp( p_pdu_decoded );
      }
      else if ( p_pdu_decoded->message.t == C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLTRIGRESPONSE )
      {
        supl_SuplTriggerResponse_proc( p_pdu_decoded );
      }
      else if ( p_pdu_decoded->message.t == C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLREPORT  )
      {
        supl_SuplReport_proc( p_pdu_decoded );
      }
      else if ( p_pdu_decoded->message.t == C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLTRIGGEREDSTOP  )
      {
        supl_SuplTrigStop_proc( p_pdu_decoded );
      }
      else
      {
        MSG_MED("LSM: Invalid SUPL message received from PDCOMM", 0, 0, 0);
      }

    } /* close if-passed-overhead-check */

    else
    {
      if ( p_pdu_decoded->message.t != C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLEND )
      {

        if ( e_slp_mess_header_check_result & C_SLP_MESS_SLP_SESS_ID_MISSING )
        {
          /* p_provided_slp_session_id->present is FALSE in this case */
          (void)tm_umts_up_supl_supl_end_prepare_or_tx(p_provided_slp_session_id,
                                                       NULL,
                                                       supl_StatusCode_invalidSessionId,
                                                       NULL,
                                                       q_supl_version
                                                       );
        }

        else if ( e_slp_mess_header_check_result & C_SLP_MESS_SLP_SESS_ID_PROBLEM )
        {
          (void)tm_umts_up_supl_supl_end_prepare_or_tx(p_provided_slp_session_id,
                                                       NULL,
                                                       supl_StatusCode_invalidSessionId,
                                                       NULL,
                                                       q_supl_version);
        }

      else if ( e_slp_mess_header_check_result & C_SLP_MESS_SLP_SESS_ID_PROBLEM_SUPLINIT_EXISTING_AREA_EVENT )
      {
        supl_set_session_id z_set_session_id_immediate_use;

        memset (&z_set_session_id_immediate_use, 0, sizeof(supl_set_session_id));
        if ( z_Set_Id.set_id_choice == C_SETID_UNKNOWN )
        {
          /* if SET session ID (ismi/msisdn... is still unknow at this point,
             this function will retried it */
          supl_SET_id_get();
        }
        if ( z_Set_Id.set_id_choice != C_SETID_UNKNOWN )
        {
          z_set_session_id_immediate_use.w_Session_Id = 0xFFFF; /* since only up to 16 existing sessions supported, use 0xFFFF for this purpose */
          memscpy(&z_set_session_id_immediate_use.z_Set_Id, sizeof(z_set_session_id_immediate_use.z_Set_Id),
                 &z_Set_Id, sizeof(z_Set_Id) );

          (void) tm_umts_up_supl_supl_end_prepare_or_tx(p_provided_slp_session_id,
                                                 &z_set_session_id_immediate_use,
                                                 supl_StatusCode_invalidSessionId,
                                                 NULL,
                                                 q_supl_version);
        }
        else
        {
          MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SET_ID unavailable: %d, ignore SUPL_INT to be rejected",
                z_Set_Id.set_id_choice);

          return TRUE;
        }

      }

        else if ( e_slp_mess_header_check_result & C_SLP_MESS_SET_SESS_ID_MISSING )
        {
          (void)tm_umts_up_supl_supl_end_prepare_or_tx(p_provided_slp_session_id,
                                                       NULL,
                                                       supl_StatusCode_invalidSessionId,
                                                       NULL,
                                                       q_supl_version);
        }


        else if ( e_slp_mess_header_check_result & C_SLP_MESS_SET_SESS_ID_PROBLEM )
        {
          (void)tm_umts_up_supl_supl_end_prepare_or_tx(p_provided_slp_session_id,
                                                       &z_supl_session_status.z_session_id.z_Set_Sess_Id,
                                                       supl_StatusCode_invalidSessionId,
                                                       NULL,
                                                       q_supl_version);
        }

        else if ( e_slp_mess_header_check_result & C_SLP_MESS_VERSION_UNSUPPORTED )
        {
          (void)tm_umts_up_supl_supl_end_prepare_or_tx(p_provided_slp_session_id,
                                                       NULL,
                                                       supl_StatusCode_versionNotSupported,
                                                       NULL,
                                                       q_supl_version);
        }
        else
        {
          (void)tm_umts_up_supl_supl_end_prepare_or_tx(p_provided_slp_session_id,
                                                       NULL,
                                                       supl_StatusCode_unspecified,
                                                       NULL,
                                                       q_supl_version);
        }

      }
      else
      {
        MSG_MED("Invalid Session ID in SUPL_END: No need to resend a SUPL END",0,0,0);
      }

    } /* failed overhead check */

    tm_osys_asn1_free_pdu (p_pdu_decoded , PROTO_SUPL);

  } /* end-if-decoded-ok */

  else /* decode status showing error */
  {
    MSG_MED("SUPL: ASN1 decoding failed", 0,0,0);
    if ( asn1_decode_status != C_ASN1_DECODE_BUFFER_NOT_ALLOCATED )
    {
      tm_osys_asn1_free_pdu (p_pdu_decoded , PROTO_SUPL);
    }
  }

  return TRUE;
}


/*===========================================================================

FUNCTION tm_umts_up_supl_tcp_connect_notif

DESCRIPTION SUPL module's actions upon knowing TCP connection is up

DEPENDENCIES
  None.

RETURN VALUE
  none.

SIDE EFFECTS
  None.

===========================================================================*/

void  tm_umts_up_supl_tcp_connect_notif(void)
{
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;
  pending_supl_end     *p_pending_supl_end = &z_pending_supl_end[0];
  uint8                u_i;

  tm_cm_phone_state_info_s_type phone_state_info;
  pdsm_pd_comm_protocol_e_type protocol_type = PDSM_PD_COMM_PROTOCOL_UMTS_UP_SUPL;

  if ( !supl_is_in_any_ut0() &&
       ( p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_NONE )
     )
  {
    MSG_MED("Nothing to do with the tcp link, tear down", 0, 0, 0);
    tm_umts_up_supl_comm_close_connection();
    return;
  }

  tm_cm_iface_get_phone_state(&phone_state_info);

  if ( phone_state_info.srv_system == CGPS_SRV_SYS_LTE )
  {
    protocol_type = PDSM_PD_COMM_PROTOCOL_LTE_UP;
  }

  /* tell TmCore to send pdapi event of EventGpsPdConnectionEstablished */
  tm_umts_common_send_pdapi_comm_event
  (  protocol_type,
     C_COMM_CONNECTED,
     p_supl_session_status->tm_active_session_handle,
     p_supl_session_status->q_pdapi_client_id
  );


  /* TCP connected, send SUPL_END if it has been in UT0 */
  if ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLEND )
  {

    for ( u_i = C_MAX_NUM_PENDING_SUPL_END; u_i; u_i-- )
    {
      if ( p_pending_supl_end->u_validity == TRUE )
      {
        if ( p_pending_supl_end->hash != NULL )
        {
          (void) tm_umts_up_supl_supl_end_tx( &p_pending_supl_end->slp_session_id,
                           &p_pending_supl_end->set_session_id,
                           p_pending_supl_end->e_supl_status_code,
                           p_pending_supl_end->hash,
                           p_supl_session_status->q_supl_version );
        }
        else
        {
          (void) tm_umts_up_supl_supl_end_tx( &p_pending_supl_end->slp_session_id,
                           &p_pending_supl_end->set_session_id,
                           p_pending_supl_end->e_supl_status_code,
                           NULL,
                           p_supl_session_status->q_supl_version );
        }

        p_pending_supl_end->u_validity = FALSE; /* this pending SUPL_END has been sent */
      }
      p_pending_supl_end++;
    }

    tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_UT0_SUPLEND );

    /* if the connection was brought up only to send SUPL_END, terminate it now */
    if ( p_supl_session_status->e_cf_state == C_SUPL_CF_STATE_NULL )
    {
      /* close connection */
      tm_umts_up_supl_comm_close_connection();

      /* cancel all SUPL timer(s) and call flow states */
      tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_NULL);

      /* clear all protocol session related variables */
      if (p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_NONE  )
      {
        tm_umts_up_supl_session_init(FALSE);
      }

    }

    /*** Check if this SUPL_END is triggered because of a geofence being
         deleted *****/
    p_pending_supl_end = &z_pending_supl_end[0];
    for (u_i=0; u_i<C_MAX_GEOFENCE_NUM; u_i++)
    {
      if ( ( z_supl_area_event_session_status[u_i].u_delete_pending ) &&
           ( p_pending_supl_end->e_supl_status_code ==
              supl_StatusCode_ver2_sessionStopped
           )
         )
      {
        memset( &z_supl_area_event_session_status[u_i], 0,
                sizeof(supl_area_event_status_s_type) );
      }
      p_pending_supl_end++;
    }

  }

  /**** the following call flow states are mutually exclusive ***************/
  /*If connection attemp is pending, then close this connection and open a new connection*/
  if(p_supl_session_status->u_tcp_connection_attempt_pending)
  {

    MSG_HIGH("Different TCP Connection required",0,0,0);
    /* close connection */
    tm_umts_up_supl_comm_close_connection();

    p_supl_session_status->u_tcp_connection_attempt_pending = FALSE;

    /* Initializing the flags so that we can attempt a fresh
       connection*/
    z_pdcomm_tcp_info.tls_session_authentication_failure = FALSE;
    z_pdcomm_tcp_info.used_auto_configured_hslp_address = FALSE;

    /*Start SLP contact*/
    tm_umts_up_supl_start_slp_contact();

    return;

  }


  for ( u_i = 0; u_i < C_MAX_GEOFENCE_NUM; u_i++)
  {
    if ( z_supl_area_event_session_status[u_i].u_session_active )
    {

      if ( z_supl_area_event_session_status[u_i].e_cf_state &
           C_SUPL_CF_STATE_UT0_SUPLTRIGGERSTART )
      {
        tm_umts_up_supl_terminate_timer_cancel_cf_state_area_event( C_SUPL_CF_STATE_UT0_SUPLTRIGGERSTART, u_i );
        if ( !supl_SuplTriggerStart_tx(u_i))
        {
          /* if failing to send SUPL_TRIGGER_START */
          MSG_HIGH("SUPL: failed to send SUPL_TRIG_START: slot=%u", u_i, 0, 0);
        }
      }


      if ( z_supl_area_event_session_status[u_i].e_cf_state &
           C_SUPL_CF_STATE_UT0_SUPLREPORT_GF_BREACH )
      {
        tm_umts_up_supl_terminate_timer_cancel_cf_state_area_event( C_SUPL_CF_STATE_UT0_SUPLREPORT_GF_BREACH, u_i );
      }
      if ( z_supl_area_event_session_status[u_i].z_pending_supl_report_info.u_valid )
      {
        if ( tm_umts_up_supl_supl_report_tx
              (
                u_i,
                &z_supl_area_event_session_status[u_i].z_pending_supl_report_info.z_gm_client_breach_notify,
                NULL, NULL
              )
           )
        {
          z_supl_area_event_session_status[u_i].z_pending_supl_report_info.u_valid = FALSE;
        }

      } /* u_i areaEvent session has pending SUPL_REPORT to send */

    } /* u_i areaEvent session active */
  } /* geofene for-loop */

  /* TCP connected, send SUPL_START if it has been in UT0 */
  if ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLSTART )
  {

    if ( !(p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_WIFI_WAIT_TIMER_NO_TCP ) )
    {
      MSG_MED("Sending SUPL_START - in UT0_SUPLSTART: %x", p_supl_session_status->e_cf_state, 0, 0);
    if ( !supl_SuplStart_tx())
    {
      tm_umts_up_supl_callflow_disrupt_handler();
    }

    /*If SUPL START is succesful, then cftimer is now set to UT1,
      If SUPL START is unsuccesful, then cftimer is stopped.
      So need not take care of cftimer, just cancel the UT0 state
    */
    z_supl_session_status.e_cf_state &= (supl_cf_state_enum_type)~C_SUPL_CF_STATE_UT0_SUPLSTART;
  }
    else /* still wait for WiFi scan result */
    {
      tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_UT0_SUPLSTART );

      z_supl_session_status.e_cf_state = C_SUPL_CF_STATE_WIFI_WAIT_SUPLSTART; /* note state change but no timer change */
      MSG_MED("TCP connected, still waiting for WIFI scan result", 0, 0, 0);
    }

  }

  /* TCP connected, send SUPL_POSINIT if it has been in UT0 */
  else if ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLPOSINIT )
  {
    /* Can't run SUPL over LTE if SUPL major version less than 2,
       even though previous negotiation with SUPL_INIT has resulted
       in running SUPL 1.0 */
    if ( (phone_state_info.srv_system == CGPS_SRV_SYS_LTE ) &&
         ( (p_supl_session_status->q_supl_version & 0xFF) < 2 )
       )
    {

      p_supl_session_status->q_supl_version =
       ( ( z_umts_nv_item_ram_copy.gnss_supl_version >> 16) & 0x000000FF ) |
       ( z_umts_nv_item_ram_copy.gnss_supl_version & 0x0000FF00 ) |
       ( ( z_umts_nv_item_ram_copy.gnss_supl_version << 16 ) & 0x00FF0000 );

      /* send SUPL_END, using the provisioned SUPL version in its
         message header, and abort this session */
      (void) tm_umts_up_supl_supl_end_tx(
               &p_supl_session_status->z_session_id.z_Slp_Sess_Id,
               NULL,
               supl_versionNotSupported,
               &p_supl_session_status->supl_init_hash[0],
               p_supl_session_status->q_supl_version);

      tm_umts_up_supl_comm_close_connection();
      tm_umts_up_supl_callflow_disrupt_handler();

      return;
    }


    if ( !(p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_WIFI_WAIT_TIMER_NO_TCP ) )
    {
      MSG_MED("Sending SUPL_POSINIT - in UT0_SUPLPOSINIT: %x", p_supl_session_status->e_cf_state, 0, 0);
    if ( !supl_SuplPosInit_tx())
    {
      tm_umts_up_supl_callflow_disrupt_handler();
    }
    else /*SuplPosInit Sent*/
    {
      if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801 &&
        z_supl_session_status.u_callflow_kind == C_SUPL_SESSION_NI)
      {
        /*Start IS801 state machine*/
        if( !supl_start_is801_session(TRUE, NULL))
        {
          /*Unable to start IS801 state machine*/
          tm_umts_up_supl_callflow_disrupt_handler();
        }
      }
    }
    /*If SUPL POS INIT is succesful, then cftimer is now set to UT2,
      If SUPL POS INIT is unsuccesful, then cftimer is stopped.
      So need not take care of cftimer, just cancel the UT0 state
    */
    z_supl_session_status.e_cf_state &= (supl_cf_state_enum_type)~C_SUPL_CF_STATE_UT0_SUPLPOSINIT;
    } /* not waiting for wifi scan result */

    else /* still wait for WiFi scan result */
    {
      tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_UT0_SUPLPOSINIT );

      z_supl_session_status.e_cf_state = C_SUPL_CF_STATE_WIFI_WAIT_SUPLPOSINIT; /* note state change but no timer change */
      MSG_MED("TCP connected, still waiting for WIFI scan result", 0, 0, 0);
  }

  }

  else if ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLREPORT_QUERY )
  {
    tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_UT0_SUPLREPORT_QUERY );

    tm_umts_up_supl_supl_report_tx( NULL,
                                    NULL,
                                    &p_supl_session_status->supl_init_hash[0],
                                    NULL
                                  );
  }

}



/*===========================================================================

FUNCTION tm_umts_up_supl_handle_lr_data

DESCRIPTION handles TmCore provided LR data.

DEPENDENCIES
  None.

RETURN VALUE
  none.

SIDE EFFECTS
  None.

===========================================================================*/
void tm_umts_up_supl_handle_lr_data(
  sm_GnssFixRptStructType* lr_data_ptr,
  boolean u_final)
{
  supl_status_s_type*      p_supl_session_status = &z_supl_session_status;
  pdsm_pd_info_s_type*     p_pd_info_buffer = &z_supl_pd_info_buffer;
  pdsm_pd_event_type       pd_event = PDSM_PD_EVENT_POSITION;
  gad_ellip_alt_unc_ellip_type   z_ellipse;
  tm_lr_resp_type                *lr_resp_ptr = NULL;
  tm_pd_position_info_s_type     z_tm_pd_position_info;
  tm_ext_pos_data_s_type         z_tm_ext_pos_data;
  utc_time                       z_utc_time;
  uint32 q_LongWord;
  int32 l_LongWord;
  double d_Double;
  uint16 w_Word;
  uint32 q_pos_sources;
  uint32 q_glonass_tod_modulo_1hour_ms; /* position time stamp GLONASS TimeOfDay */
  boolean v_sess_type_ni = FALSE;


  if ( p_supl_session_status->u_pdapi_fix_reported )
  {
    /* if position has been passed on to pdapi via TmCore already in this session,
       don't repeat that */
    return;
  }


  if (NULL == lr_data_ptr)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null data buffer");
    return;
  }

  if ( ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_NO_POSITION ) ||
       ( p_supl_session_status->u_agps_mode != C_SUPL_AGPS_MODE_UE_BASED ) )
  {
    return; /* not supposed to receive any position from TmCore, ignore it */
  }

  if (C_SUPL_SESSION_NI == p_supl_session_status->u_callflow_kind)
  {
    v_sess_type_ni =  TRUE;
  }
  q_pos_sources = 0;
  if (lr_data_ptr->z_NavPos.z_PosFlags.b_IsGpsUsed)
  {
    q_pos_sources |= C_POS_SOURCE_GPS;
  }
  if (lr_data_ptr->z_NavPos.z_PosFlags.b_IsGloUsed)
  {
    q_pos_sources |= C_POS_SOURCE_GLO;

    /* Store glonass time to report to server */
    p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.q_GloTimeMs = lr_data_ptr->z_NavPos.q_GloTimeMs;
  }
  if (lr_data_ptr->z_NavPos.z_PosFlags.b_IsBdsUsed)
  {
    q_pos_sources |= C_POS_SOURCE_BDS;
  }

  p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.q_pos_sources =
  z_supl_cached_pos_for_net.z_pos_info.q_pos_sources = q_pos_sources;

  /* Save the valid Glonass TOD: sanity check first */
  if ( (lr_data_ptr->z_NavPos.u_GloNumFourYear == 255) ||
       (lr_data_ptr->z_NavPos.w_GloNumDaysInFourYear == 65535) ||
       (lr_data_ptr->z_NavPos.q_GloTimeMs  > DAY_MSECS) )
  {
    q_glonass_tod_modulo_1hour_ms = 0xFFFFFFFF;

    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,
          "GlonassTOD invalid: Num4Year=%u, NumDaysIn4Year=%u, ToD_ms=%lu",
          lr_data_ptr->z_NavPos.u_GloNumFourYear,
          lr_data_ptr->z_NavPos.w_GloNumDaysInFourYear,
          lr_data_ptr->z_NavPos.q_GloTimeMs);
  }
  else
  {
    q_glonass_tod_modulo_1hour_ms = lr_data_ptr->z_NavPos.q_GloTimeMs % HOUR_MSECS;
  }

  /* construct GAD shape data from LR for multiple usage below */
  (void)tm_umts_common_utils_construct_ellip_alt_unc_ellip_from_lr(&z_ellipse, lr_data_ptr, TM_CONF_REPORTED_FROM_PE, v_sess_type_ni); 

  /* Save the position for this session in case it will have to be reported to
     SLP at a later time */
    if ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP )
  {
    /* the values in GAD shape strucutre cannot be used for LPP as LPP expects them in a different
      format. save lpp lat/long/alt seperatelty */
    d_Double = lr_data_ptr->z_NavPos.d_PosLla[0] * C_LAT_RAD_TO_GAD;

    if ( lr_data_ptr->z_NavPos.d_PosLla[0] < 0 ) /* if latitude in the southern hemesphere */
    {
      /* then code the rest with the absolute value of the latitude */
      d_Double = -d_Double;
    }

    q_LongWord = (uint32)tm_util_nearest_long( d_Double );
    p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.q_Lat = q_LongWord;
    /********** longitude ************/

    d_Double = lr_data_ptr->z_NavPos.d_PosLla[1] * C_LON_RAD_TO_GAD;

    l_LongWord = tm_util_nearest_long( d_Double );

    p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.l_Long = l_LongWord;

    w_Word = (uint16) tm_util_nearest_long( fabs(lr_data_ptr->z_NavPos.d_PosLla[2]) );
    p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.w_Alt = w_Word;
  }

  p_supl_session_status->z_pending_new_pos_to_network.u_valid = TRUE;

  p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.q_fix_time_gps_msec =
    lr_data_ptr->z_NavPos.q_GpsTimeMs;

  p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.q_glonass_tod_modulo_1hour_ms =
    q_glonass_tod_modulo_1hour_ms;

  p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.w_GpsWeek =
    lr_data_ptr->z_NavPos.w_GpsWeek;

  p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.q_fix_time_utc_msec =
    lr_data_ptr->z_NavPos.q_UtcTimeMs;

  p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.f_ClockBiasUncMs =
    lr_data_ptr->z_NavPos.f_ClockBiasUncMs;

  p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.f_ClockBias =
    lr_data_ptr->z_NavPos.f_ClockBias;

  memscpy ( &p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.z_pos,
           sizeof(p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.z_pos),
           &z_ellipse, sizeof(z_ellipse) );


  if ( lr_data_ptr->z_NavPos.z_PosFlags.b_IsVelValid )
  {
    p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.vel_valid = TRUE;

    p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.f_VelEnu[0] =
      lr_data_ptr->z_NavPos.f_VelEnu[0];

    p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.f_VelEnu[1] =
      lr_data_ptr->z_NavPos.f_VelEnu[1];

    p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.f_VelEnu[2] =
      lr_data_ptr->z_NavPos.f_VelEnu[2];

  }

  if ( u_final == FALSE )
  {
    p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.u_final = FALSE;
    MSG_HIGH("Intermediate pos. save", 0, 0, 0);
    return;
  }

  /*If Final LR becomes available before Early timer expiry,Cancel the CF and timer. 
    Since in 911 we run until time out, explicit 911 check is not needed here*/
  if ( ( 0!= ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_OTDOA_EARLY_RESPONSE_REQ ) ) &&
       ( TM_POS_PRTL_LPP == p_supl_session_status->u_position_protocol ) ) 
  {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Cancel earlyResponse Timer");
      tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_OTDOA_EARLY_RESPONSE_REQ );
  }


  lr_resp_ptr = (tm_lr_resp_type *)pdsm_getbuf( sizeof(tm_lr_resp_type)); /*lint !e826 */

  if ( lr_resp_ptr != NULL )
  {

    /* initialize buffer for pdapi */
    memset(p_pd_info_buffer, 0, sizeof( pdsm_pd_info_s_type ) );

    /* Convert from LM to IS801 format: simply to take advantage of the readily available
       function to convert position and timestamp to the format the TmCore interface uses */
    if(tm_util_convert_lr(lr_data_ptr,lr_resp_ptr))
    {
      tm_util_xlate_nav_soln_to_tm_pos_info( NULL,
                                             &z_tm_pd_position_info,
                                             &lr_data_ptr->z_NavPos,
                                             PDSM_PD_DLOAD_LOCATION,         /* dont care */
                                             PDSM_PD_DLOAD_SOURCE_STANDALONE /* dont care */ );

      /* set up proper position mode in the pdapi report */
      if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_BASED )
      {
        z_tm_pd_position_info.position_mode = PDSM_PD_POSITION_MODE_MSBASED;
      }
      else if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED )
      {
        z_tm_pd_position_info.position_mode = PDSM_PD_POSITION_MODE_MSASSISTED;
      }
      else if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_AUTO )
      {
        z_tm_pd_position_info.position_mode = PDSM_PD_POSITION_MODE_STANDALONE;
      }
      else
      {
        z_tm_pd_position_info.position_mode = PDSM_PD_POSITION_MODE_UNKNOWN;
      }

      tm_util_xlate_nav_soln_to_tm_ext_pos(&z_tm_ext_pos_data,
                                           &lr_data_ptr->z_NavPos);

      z_tm_ext_pos_data.h_dop = lr_data_ptr->z_NavPos.f_HDOP;
      z_tm_ext_pos_data.v_dop = lr_data_ptr->z_NavPos.f_VDOP;
      z_tm_ext_pos_data.p_dop = lr_data_ptr->z_NavPos.f_PDOP;;
      z_tm_ext_pos_data.pos_hepe =
         sqrt((lr_data_ptr->z_NavPos.f_ErrorEllipse[1] *
               lr_data_ptr->z_NavPos.f_ErrorEllipse[1]) +
              (lr_data_ptr->z_NavPos.f_ErrorEllipse[2] *
               lr_data_ptr->z_NavPos.f_ErrorEllipse[2]));


      z_utc_time = tm_util_construct_utc_time(lr_data_ptr->z_NavPos.w_GpsWeek,
                                              lr_data_ptr->z_NavPos.q_GpsTimeMs,
                                              lr_data_ptr->z_NavPos.q_UtcTimeMs);
      z_tm_ext_pos_data.utc_time.hour = z_utc_time.q_hours;
      z_tm_ext_pos_data.utc_time.minute = z_utc_time.q_mins;
      z_tm_ext_pos_data.utc_time.sec = (uint16)((z_utc_time.q_secs * 100) + (z_utc_time.q_tenths * 10));
      z_tm_ext_pos_data.utc_time.date = (uint32) ((z_utc_time.q_utc_days * 1000000) + (z_utc_time.q_month * 10000) + z_utc_time.q_year);

      pd_event = tm_util_pd_info_data_init( p_pd_info_buffer,
                                            lr_resp_ptr,
                                            p_supl_session_status->q_pdapi_client_id,
                                            NULL,
                                            &z_tm_pd_position_info,
                                            &z_tm_ext_pos_data );

      /* flag if this position is to be also sent to the network */
      if ( ( p_supl_session_status->u_network_session_on ) &&
           ( p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_NI )
         )
      {
        z_supl_pd_info_buffer.pd_info.pd_data.ext_pos_data.pos_reported_to_network = TRUE;
      }

      (void) tm_post_pd_event ( TM_PRTL_TYPE_UMTS_UP,
                                p_supl_session_status->tm_active_session_handle,
                                pd_event,
                                &z_supl_pd_info_buffer,
                                &lr_data_ptr->z_NavPos.z_PosFlags);

      /* GPS Complete event */
      sm_log_event_report(SM_LOG_EVENT_ID_GPS_COMPLETE,0);
      p_supl_session_status->u_pdapi_fix_reported = TRUE;

    }

    (void)pdsm_freebuf((char *)lr_resp_ptr);

  }

  /* populate the cached position structure for possible future use for an NI
     session, in SUPL_POS_INIT */

  memset(&z_supl_cached_pos_for_net, 0, sizeof(supl_cached_pos_for_net_s_type));
  z_supl_cached_pos_for_net.u_valid = TRUE;
  z_supl_cached_pos_for_net.z_pos_info.q_fix_time_gps_msec =
      lr_data_ptr->z_NavPos.q_GpsTimeMs;
  z_supl_cached_pos_for_net.z_pos_info.w_GpsWeek =
      lr_data_ptr->z_NavPos.w_GpsWeek;
  z_supl_cached_pos_for_net.z_pos_info.q_fix_time_utc_msec =
      lr_data_ptr->z_NavPos.q_UtcTimeMs;

  if ( q_pos_sources & C_POS_SOURCE_GPS ) /* if GPS SV used */
  {
    z_supl_cached_pos_for_net.z_pos_info.q_pos_sources = C_POS_SOURCE_GPS;
  }
  if ( q_pos_sources & C_POS_SOURCE_GLO  ) /* if GLONASS */
  {
    z_supl_cached_pos_for_net.z_pos_info.q_pos_sources |= C_POS_SOURCE_GLO;
  }
  if ( q_pos_sources & C_POS_SOURCE_BDS  ) /* if BeiDou */
  {
    z_supl_cached_pos_for_net.z_pos_info.q_pos_sources |= C_POS_SOURCE_BDS;
  }

  memscpy ( &z_supl_cached_pos_for_net.z_pos_info.z_pos,
           sizeof(z_supl_cached_pos_for_net.z_pos_info.z_pos),
           &z_ellipse, sizeof(z_ellipse) );


  if ( lr_data_ptr->z_NavPos.z_PosFlags.b_IsVelValid )
  {
    z_supl_cached_pos_for_net.z_pos_info.vel_valid = TRUE;

    z_supl_cached_pos_for_net.z_pos_info.f_VelEnu[0] =
      lr_data_ptr->z_NavPos.f_VelEnu[0];

    z_supl_cached_pos_for_net.z_pos_info.f_VelEnu[1] =
      lr_data_ptr->z_NavPos.f_VelEnu[1];

    z_supl_cached_pos_for_net.z_pos_info.f_VelEnu[2] =
      lr_data_ptr->z_NavPos.f_VelEnu[2];
    z_supl_cached_pos_for_net.z_pos_info.f_Vunc3dMps = lr_data_ptr->z_NavPos.f_Vunc3dMps;
    z_supl_cached_pos_for_net.z_pos_info.f_VelVertUnc = lr_data_ptr->z_NavPos.f_VuncVertMps;
  }

  /*******  check to see if reporting to the network is needed *************/

  if ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER )
  {
    /* clear timer and call flow state */
    tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER);

    if ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_RRLP )
    {
      if ( lr_data_ptr->z_NavPos.z_PosFlags.b_IsVelValid )
      {

        supl_Velocity z_velocity;
        supl_Horandvervel z_horandvervel;

        memset(&z_velocity, 0, sizeof(supl_Velocity));
        memset(&z_horandvervel, 0, sizeof(supl_Horandvervel));

        z_velocity.u.horandvervel = &z_horandvervel;

        supl_convert_velocity ( lr_data_ptr->z_NavPos.f_VelEnu[0],
                                lr_data_ptr->z_NavPos.f_VelEnu[1],
                                lr_data_ptr->z_NavPos.f_VelEnu[2],
                                &z_velocity);

        supl_send_pos_rrlp_to_slp( &z_ellipse, &z_velocity,
                                   lr_data_ptr->z_NavPos.q_GpsTimeMs,
                                   q_pos_sources,
                                   q_glonass_tod_modulo_1hour_ms);

      }
      else
      {
        supl_send_pos_rrlp_to_slp( &z_ellipse, NULL,
                                   lr_data_ptr->z_NavPos.q_GpsTimeMs,
                                   q_pos_sources,
                                   q_glonass_tod_modulo_1hour_ms);
      }
    }

    #if defined (FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
    else if ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP )
    {

      tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER);
      supl_enc_lpp_prov_loc(p_supl_session_status, C_LPP_AGPS_MODE_UE_BASED);
      /* send SUPL POS with LPP Provide Location Info*/
      supl_SuplPos_lpp_tx(p_supl_session_status, NULL);
    }
    #endif /* FEATURE_CGPS_LTE_CELLDB && !FEATURE_GNSS_LOW_MEMORY */

    MSG_MED("Sent SUPL_POS to SLP", 0, 0, 0);


  } /* if rrpl-msrPosReq has been received */

  else if (p_supl_session_status->u_network_session_on)
  {
    /*If internal session is ON and LR is already received of NV is set to terminate SUPL 
      after final fix, kill the connection attempt and close the session*/
    if ((tm_core_is_internal_session_on() ||
          tm_core_get_end_supl_after_final_fix_nv()) && 
        ( (p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLSTART) ||
          (p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLPOSINIT) ||
          (p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLEND)
        )
       )
    {
      /* send DONE event to pdapi */
      tm_umts_up_supl_inform_core_stop_session( TM_STOP_REASON_COMPLETED );
      tm_umts_up_supl_comm_close_connection();
      /* cancel all SUPL timer(s) and call flow states */
      tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_NULL);
      /* clear all protocol session related variables */
      tm_umts_up_supl_session_init(FALSE);
    }
    else
    {
      /* no rrlp-msrPosReq ever received, but already engaged in a network session,
         save the position as pending for network, in case this network session requires position going back
         to the network */

      p_supl_session_status->z_pending_new_pos_to_network.u_valid = TRUE;
      p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.u_final = TRUE;
    }
  }

  else  /* there is no network session */
  {
    /* send DONE event to pdapi */
    tm_umts_up_supl_inform_core_stop_session( TM_STOP_REASON_COMPLETED );

    if ( !(z_supl_session_status.e_cf_state & C_SUPL_CF_STATE_UT3_EXPECT_SUPLREPORT ) &&
         !(z_supl_session_status.e_cf_state & C_SUPL_CF_STATE_UT0_SUPLEND )
       )
    {
      tm_umts_up_supl_session_init(FALSE);
    }
  }
}

/*===========================================================================

FUNCTION tm_supl_expect_further_meas

DESCRIPTION
  A-GPS+GLO call flow (only): Wait for GLONASS measurement if the protocol modules have successfully
  inject Glonass AA data into MGP & LS requested GLO measurements.*
  SM doesn't depend on the order of measurement report from ME. Below condition checks if both
  GPS + GLO meas are cached/stored internally when GLO meas report is expected.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean: TRUE if input validation is successful /FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_supl_expect_further_meas(supl_status_s_type const *const p_supl_session_status,
                                    sm_GpsMeasRptStructType const *const p_prm_data)
{
  if ( (NULL != p_supl_session_status)  && (NULL != p_prm_data))
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Wait for final PRM. Rcvd part (%d) of Total (%d)", 
          p_prm_data->z_MeasBlk.u_SeqNum,
          p_prm_data->z_MeasBlk.u_MaxMessageNum);

    if ( (SM_GNSS_EXPECT_GLO_MEAS & p_supl_session_status->u_glo_meas_status) &&
        (p_prm_data->z_MeasBlk.u_SeqNum < p_prm_data->z_MeasBlk.u_MaxMessageNum)
        )
    {
      return ( TRUE ); /* wait for GPS or GLONASS measurement report */
    }
  }
  return ( FALSE );
}


/*===========================================================================

FUNCTION tm_supl_need_to_store_meas

DESCRIPTION Decide if the measurement should be stored/cached internally.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_supl_need_to_store_meas( sm_GpsMeasRptStructType const *const p_gnss_prm_data,
                                    boolean const b_use_cached_sm_meas_struct )
{
  if ( p_gnss_prm_data != NULL)
  {
    if ( ((GNSS_MEAS_BLK_SRC_GPS == p_gnss_prm_data->z_MeasBlk.e_MeasBlkSrc) ||
          (GNSS_MEAS_BLK_SRC_GLO == p_gnss_prm_data->z_MeasBlk.e_MeasBlkSrc)) &&
         (FALSE == b_use_cached_sm_meas_struct) )
    {
      return ( TRUE );
    }
  }
  return ( FALSE );
}

/*===========================================================================

FUNCTION tm_umts_up_supl_handle_prm_data

DESCRIPTION handles TmCore provided PRM data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void tm_umts_up_supl_handle_prm_data ( sm_GpsMeasRptStructType *p_prm_data )
{
  supl_status_s_type*      p_supl_session_status = &z_supl_session_status;
  uint32                   q_session_remaining_time_msec;
  uint8                    u_i, u_list_len;
  uint8                    u_sv_cnt;
  tm_pdu_buf_type          pdu_buff; /* including length and pointer to data */
  rrlp_PDU z_rrlp;
  rrlp_MsrPosition_Rsp     *p_mpr;
  gnss_MeasStructType      *p_gps_meas_per_sv;
  rrlp_MsrPosition_Rsp     z_rrlp_MsrPosition_Rsp;
  rrlp_GPS_MsrSetElement   z_rrlp_GPS_MsrSetElement;
  rrlp_GPS_MsrElement      z_MsrElement[16]; /* max. 16 SVs */

  rrlp_GANSS_MsrSetElement *p_rrlp_GANSS_MsrSetElement = NULL;

  boolean status_check_needed = TRUE;
  uint32 flags = 0;
  int encode_status;
  #if defined (FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
  tm_lpp_transaction_info_s_type *p_lpp_tx_type = NULL;
  #endif /* FEATURE_CGPS_LTE_CELLDB && !FEATURE_GNSS_LOW_MEMORY */
  tm_cm_phone_state_info_s_type phone_state_info;
  boolean b_e911= FALSE;
  boolean b_send_cached = FALSE;
  sm_GpsMeasRptStructType *p_gnss_prm_data = NULL;
  boolean b_gps_meas_qualified = FALSE;
  boolean b_glonass_meas_qualified = FALSE;


  /* Query TM-CM module to get current acquired system */
  tm_cm_iface_get_phone_state(&phone_state_info);
  if ((phone_state_info.e911_state == TM_CM_E911_STATE_ORIG)||
      (phone_state_info.e911_state == TM_CM_E911_STATE_CB) ||
      ( z_umts_nv_item_ram_copy.aagps_development_test_control5 &
         NV_AAGPS_TEST5_EMERGENCY_CALL_SIM ))
  {
    b_e911 = TRUE;
    MSG_MED("SUPL: E911 ON",0,0,0);
  }

  memset(&pdu_buff, 0, sizeof(tm_pdu_buf_type));

  if ( p_supl_session_status->u_agps_mode != C_SUPL_AGPS_MODE_UE_ASSISTED )
  {
    return;
  }

  if (p_prm_data == NULL)
  {
    return;
  }

  q_session_remaining_time_msec = os_TimerCheck ( supl_cf_timer );

  if ( p_prm_data->e_MeasRptReason != GPS_MEAS_RPT_MEAS_DONE )
  {
    if ( q_session_remaining_time_msec > 2500 )
    {
      /* Unless approaching end of session, not interested in anything other
         than DONE, ignore it */
      MSG_MED("SUPL: disgarding non-DONE MeasRpt", 0, 0, 0);
      return;
    }
    else
    {
      /* if there is cached meas., it is for sure a DONE meas., use that
             one --- only E911 may have cached meas. */
      MSG_MED("SUPL: proc'ing non-DONE or Cached MeasRpt as sess. ending", 0, 0, 0);
      if ( (b_e911 == TRUE) &&
           (p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP ) &&
           ( p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_PRM_VALID ))
      {
        b_send_cached = TRUE;
        MSG_MED("TM_LPP_UP: Sending DONE Cached MeasRpt as sess. ending in E911", 0, 0, 0);
      }

      /*Approaching end of session, no status check needed*/
      status_check_needed = FALSE;
      MSG_MED("SUPL: proc'ing non-DONE MeasRpt as sess. ending", 0, 0, 0);

    }
  }
  else
  {
    MSG_MED("Proc. DONE MeasRpt or end-of-session None-Done: %u", p_prm_data->e_MeasRptReason, 0, 0);
    if ((p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_OTDOA_EARLY_RESPONSE_REQ)
        &&(p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP)
        && (b_e911 == FALSE)) 
    {
      MSG_MED("Cancel earlyResponse Timer", 0, 0, 0);
      tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_OTDOA_EARLY_RESPONSE_REQ );
  }
  }

  MSG_5(MSG_SSID_GPSSM, MSG_LEGACY_MED, "SUPL: MeasSrc %d (GPS/GLO), Filled MeasStruct 0x%x, StatusCheck %d, GloStatus %d, Use cached %d",
        p_prm_data->z_MeasBlk.e_MeasBlkSrc, p_supl_session_status->z_gnss_meas_report.u_meas_reports_bmap,
        status_check_needed, p_supl_session_status->u_glo_meas_status, b_send_cached);

  if (p_prm_data->z_MeasBlk.u_NumSvs == 0 )
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SUPL: 0 SV meas report is ignored later" );
  }

  if ( ! ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER ) )
  {
    MSG_MED("SUPL: GNSS meas. ignored - not expecting", 0, 0, 0);
    return;
  }

  /* Decide if received GPS or Glonass measurements should be copied/stored internally in SM measurement report struct */
  if ( tm_supl_need_to_store_meas(p_prm_data, b_send_cached) )
  {
    tm_copy_prm_to_sm_meas_struct(p_prm_data, &p_supl_session_status->z_gnss_meas_report);
  }

  /* For LPP E911 cache latest DONE measurement */
  if ( (b_e911 == TRUE) &&
       (p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP) &&
       (q_session_remaining_time_msec > 2500)
      )
  {
    MSG_HIGH("SUPL: LPP in E911, not rept. meas, wait to TO", 0, 0, 0);
    p_supl_session_status->z_supl_lpp_data.q_lpp_flags |= TM_SUPL_LPP_PRM_VALID;
    return;
    }

  /* A-GPS+GLO call flow (only): Wait for GLONASS measurement if the protocol modules have successfully inject
  *  Glonass AA data into MGP & LS requested GLO measurements.
  *  SM doesn't depend of the order of measurement report from ME. Below condition checks if both GPS + GLO meas are
  *  cached/stored internally when GLO meas report is expected.
  */
  if ( tm_supl_expect_further_meas(p_supl_session_status, p_prm_data) )
    {
    return; /* wait for GPS or GLONASS measurement report */
  }

  /* Propagate each GPS and/or GLONASS SV's measurement to the common reference point: p_prm_data->z_MeasBlk.z_MeasClock.q_RefFCount
  */
  if ( p_supl_session_status->z_gnss_meas_report.u_meas_reports_bmap & SM_GNSS_MEAS_REPORTS_BMAP_GPS )
  {
    gps_MeasBlkPropagate(&(p_supl_session_status->z_gnss_meas_report.z_meas_reports[SM_GNSS_MEAS_SYSTEM_GPS].z_MeasBlk));
    }
  if ( p_supl_session_status->z_gnss_meas_report.u_meas_reports_bmap & SM_GNSS_MEAS_REPORTS_BMAP_GLONASS )
  {
    gnss_MeasBlkPropagate(&(p_supl_session_status->z_gnss_meas_report.z_meas_reports[SM_GNSS_MEAS_SYSTEM_GLONASS].z_MeasBlk));
    gnss_GloMeasBlkGGRfgdAdjust(&(p_supl_session_status->z_gnss_meas_report.z_meas_reports[SM_GNSS_MEAS_SYSTEM_GLONASS].z_MeasBlk));
  }

  /* Protocol specific handling */
  if ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_RRLP )
  {
    memset(&pdu_buff, 0, sizeof(tm_pdu_buf_type));
    memset(&z_rrlp, 0, sizeof(rrlp_PDU));
    memset(&z_rrlp_MsrPosition_Rsp, 0, sizeof(rrlp_MsrPosition_Rsp));
    memset(&z_rrlp_GPS_MsrSetElement, 0, sizeof(rrlp_GPS_MsrSetElement));
    memset(&z_MsrElement[0], 0, (sizeof(rrlp_GPS_MsrElement)* 16) );

    z_rrlp.component.u.msrPositionRsp = &z_rrlp_MsrPosition_Rsp;
    z_rrlp_MsrPosition_Rsp.gps_MeasureInfo.gpsMsrSetList.n = 1;
    z_rrlp_MsrPosition_Rsp.gps_MeasureInfo.gpsMsrSetList.elem = &z_rrlp_GPS_MsrSetElement;
    z_rrlp_GPS_MsrSetElement.gps_msrList.elem = &z_MsrElement[0];
    p_mpr = &z_rrlp_MsrPosition_Rsp;

    /* ref. num: the same as the one came from SLP */
    z_rrlp.referenceNumber = p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.b_RefNum;

    z_rrlp.component.t = rrlp_msrPositionRsp_chosen;

    if ( p_supl_session_status->z_gnss_meas_report.u_meas_reports_bmap & SM_GNSS_MEAS_REPORTS_BMAP_GPS )
    {
      p_gnss_prm_data = &p_supl_session_status->z_gnss_meas_report.z_meas_reports[SM_GNSS_MEAS_SYSTEM_GPS];

      /* if this number is ever greater than 16, limit to the first 16 */
      if ( p_gnss_prm_data->z_MeasBlk.u_NumSvs < 16 )
      {
        u_list_len = p_gnss_prm_data->z_MeasBlk.u_NumSvs;
      }
      else
      {
        u_list_len = 16;
      }

      /* count to see if there is any non-zero CNo in the list */
      u_sv_cnt = 0;

      p_gps_meas_per_sv = &p_gnss_prm_data->z_MeasBlk.z_Gnss[0];
      for (u_i=0; u_i < u_list_len; u_i++)
      {
        if (  p_gps_meas_per_sv->w_Cno > 0  )
        {
          u_sv_cnt++;
        }
        p_gps_meas_per_sv++;
      }

      if ( u_sv_cnt != 0 )
      {
      if ( tm_rrlp_mprsp_gps_meas_build( p_mpr, p_gnss_prm_data,
                                         u_list_len,
                                         status_check_needed ) == 0)
      {
        MSG_HIGH("GPS meas. build aborted: not enough qualified SVs", 0, 0, 0);
      }
      else
      {
        b_gps_meas_qualified = TRUE;
      }
      }

    } /* if GPS available  */

    if ( p_supl_session_status->z_gnss_meas_report.u_meas_reports_bmap & SM_GNSS_MEAS_REPORTS_BMAP_GLONASS )
    {
      /* rel7 extension GANSS measurements are only for sending GLONASS measurements to the SLP.
         If both GPS and GANSS measurements are to be sent to SLP,
         the GPS measurements are sent in gps-MeasureInfo while the GLONASS measurements are sent in ganssMeasureInfo
       */

      if ( ( p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.z_rrlp_rel7_ext.rel7_ext_present == TRUE) &&
           ( p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.z_rrlp_rel7_ext.u_ganss_pos_methods_bitmap &
             RRLP_GANSS_POS_METHOD_GLONASS
           )
         )
      {

        p_gnss_prm_data = &p_supl_session_status->z_gnss_meas_report.z_meas_reports[SM_GNSS_MEAS_SYSTEM_GLONASS];


        p_rrlp_GANSS_MsrSetElement = os_MemAlloc ( sizeof(rrlp_GANSS_MsrSetElement), OS_MEM_SCOPE_TASK );

        if ( p_rrlp_GANSS_MsrSetElement != NULL )
        {
          memset(p_rrlp_GANSS_MsrSetElement, 0, sizeof(rrlp_GANSS_MsrSetElement) );

          if ( tm_rrlp_mprsp_glonass_meas_build( p_mpr, p_gnss_prm_data, status_check_needed,
                                                 b_gps_meas_qualified, p_rrlp_GANSS_MsrSetElement
                                               ) == FALSE
             )
          {
            MSG_HIGH("GLONASS meas. build aborted", 0, 0, 0);

            /* free p_rrlp_GANSS_MsrSetElement :  */
            os_MemFree ( (void **) &p_rrlp_GANSS_MsrSetElement);
          }
          else
          {
            b_glonass_meas_qualified = TRUE;
          }
        }
        else
        {
          MSG_ERROR("rrlp_GANSS_MsrSetElement allocation failure, skip GLO meas. report", 0, 0, 0);
        }

      }
    } /* if GLONASS available */

    if ( !b_gps_meas_qualified && !b_glonass_meas_qualified )
    {
      MSG_HIGH("Neither GPS or GLONASS meas. qualified", 0, 0, 0);
      return;
    }

    /* if REL-5 Extension has been included in the message sent from the network, include it
       in this uplink message too */
    if ( p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.z_rrlp_rel5_ext.rel5_ext_present )
    {
      p_mpr->m.rel_5_MsrPosition_Rsp_ExtensionPresent = TRUE;

      p_mpr->rel_5_MsrPosition_Rsp_Extension.m.extended_referencePresent = TRUE;

      p_mpr->rel_5_MsrPosition_Rsp_Extension.extended_reference.smlc_code =
        p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.z_rrlp_rel5_ext.rel5_ext_ref.smlc_code;

      p_mpr->rel_5_MsrPosition_Rsp_Extension.extended_reference.transaction_ID =
        p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.z_rrlp_rel5_ext.rel5_ext_ref.transaction_ID;

    }

    if(!status_check_needed)
    {
      /*Status check was not done, include time uncertainity*/
      p_mpr->m.rel_98_MsrPosition_Rsp_ExtensionPresent = TRUE;
      p_mpr->rel_98_MsrPosition_Rsp_Extension.m.timeAssistanceMeasurementsPresent = TRUE;
      p_mpr->rel_98_MsrPosition_Rsp_Extension.timeAssistanceMeasurements.m.gpsReferenceTimeUncertaintyPresent = TRUE;
      p_mpr->rel_98_MsrPosition_Rsp_Extension.timeAssistanceMeasurements.gpsReferenceTimeUncertainty = tm_umts_common_utils_microsec_to_GADK(p_gnss_prm_data->z_MeasBlk.z_MeasClock.z_Time.z_GpsTime.f_ClkTimeUncMs * 1000);
    }

    /* Do the RRLP encode */
    encode_status = tm_osys_asn1_encode_pdu((void *)&z_rrlp, &pdu_buff, PROTO_RRLP);

    if ( (encode_status != 0) || (pdu_buff.value == NULL ) || (pdu_buff.length == 0) )
    {
      /* ASN.1 encoding failed and pdu buffer allocated  */
      if (pdu_buff.value != NULL )
      {
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRLP ASN1 enc. err. Freeing: %d,%lx,%u",
              encode_status, pdu_buff.value, pdu_buff.length);

        tm_asn1_free_osys_buf( pdu_buff.value, PROTO_RRLP );
        pdu_buff.value = NULL;

      }
      else /* ASN.1 encoding failed but pdu buffer allocated  */
      {
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRLP ASN1 enc. err. No alloc.: %d,%lx,%u",
              encode_status, pdu_buff.value, pdu_buff.length);
      }
      return;
    } /* if pdu ASN1 encoding fails */

    else /* pdu ASN1 encoding successful */
    {

      /* clear timer and call flow state */
      tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER);

      if ( !supl_SuplPos_rrlp_tx (pdu_buff.value, (uint16)pdu_buff.length, NULL) )
      {
        tm_umts_up_supl_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
        tm_umts_up_supl_session_init(FALSE);
      }
      else
      {
        /* GPS Complete event */
        sm_log_event_report(SM_LOG_EVENT_ID_GPS_COMPLETE,0);

        /* QQQ: for possible time-stamping SET-Assisted position, until a UTC-->GPS conversion
           utility is available */
        if ( p_supl_session_status->z_set_assist_pos_time_stamp.m.refTimeWeekNumValid )
        {
          p_supl_session_status->z_set_assist_pos_time_stamp.t_pos_time_stamp_ms =
              tm_umts_common_utils_full_gps_ms(
                  p_supl_session_status->z_set_assist_pos_time_stamp.w_ref_time_week_num,
                  p_gnss_prm_data->z_MeasBlk.z_MeasClock.z_Time.z_GpsTime.q_GpsMsec);

          p_supl_session_status->z_set_assist_pos_time_stamp.m.posTimeStampValid = TRUE;
        }

      }
      /* Free the PDU */
      tm_asn1_free_osys_buf( pdu_buff.value, PROTO_RRLP );
      pdu_buff.value = NULL;
    }

    /* regardless whether pdu encoding is successful, free allocated IEs */
    if ( p_rrlp_GANSS_MsrSetElement != NULL)  /* allocated in this function */
    {
      if ( p_rrlp_GANSS_MsrSetElement->ganss_MsrElementList.elem != NULL )
      {
        if ( p_rrlp_GANSS_MsrSetElement->ganss_MsrElementList.elem->ganss_SgnTypeList.elem != NULL )
        {
          if ( p_rrlp_GANSS_MsrSetElement->ganss_MsrElementList.elem->ganss_SgnTypeList.elem->ganss_SgnList.elem != NULL )
          {
            os_MemFree ( (void **) &p_rrlp_GANSS_MsrSetElement->ganss_MsrElementList.elem->ganss_SgnTypeList.elem->ganss_SgnList.elem );
          }
          os_MemFree ( (void **) &p_rrlp_GANSS_MsrSetElement->ganss_MsrElementList.elem->ganss_SgnTypeList.elem);
        }
        os_MemFree ( (void **) &p_rrlp_GANSS_MsrSetElement->ganss_MsrElementList.elem );
      }

      os_MemFree ( (void **) &p_rrlp_GANSS_MsrSetElement); /* allocated in this function */

    }

  } /* if RRLP */

  else if ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP )
  {
       /* GPS Complete event */
    sm_log_event_report(SM_LOG_EVENT_ID_GPS_COMPLETE,0);

    MSG_5( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "GPSTime: Valid %1u - %u, %u, %lu. Lpp flag 0x%lX",
           p_supl_session_status->z_set_assist_pos_time_stamp.m.refTimeWeekNumValid,
           p_supl_session_status->z_set_assist_pos_time_stamp.w_ref_time_week_num,
            p_supl_session_status->z_gnss_meas_report.z_meas_reports[SM_GNSS_MEAS_SYSTEM_GPS].z_MeasBlk.z_MeasClock.z_Time.z_GpsTime.w_GpsWeek,
            p_supl_session_status->z_gnss_meas_report.z_meas_reports[SM_GNSS_MEAS_SYSTEM_GPS].z_MeasBlk.z_MeasClock.z_Time.z_GpsTime.q_GpsMsec,
            p_supl_session_status->z_supl_lpp_data.q_lpp_flags);

    /* Populate the GPS msec time from Measurement clock time */
    if ( p_supl_session_status->z_set_assist_pos_time_stamp.m.refTimeWeekNumValid )
    {
      p_supl_session_status->z_set_assist_pos_time_stamp.t_pos_time_stamp_ms =
          tm_umts_common_utils_full_gps_ms(
              p_supl_session_status->z_set_assist_pos_time_stamp.w_ref_time_week_num,
              p_supl_session_status->z_gnss_meas_report.z_meas_reports[SM_GNSS_MEAS_SYSTEM_GPS].z_MeasBlk.z_MeasClock.z_Time.z_GpsTime.q_GpsMsec);

      p_supl_session_status->z_set_assist_pos_time_stamp.m.posTimeStampValid = TRUE;
    }

    p_supl_session_status->z_supl_lpp_data.q_lpp_flags |= TM_SUPL_LPP_PRM_VALID ;
    p_supl_session_status->z_supl_lpp_data.b_status_check_needed = status_check_needed;

    /* Is both gnss & otdoa measurement are requested by LS (possible only in MSA) ? */
    if  (((p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_COMBINE_AGNSS_OTDOA)  &&
          ((p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_OTDOA_DELIVERED)) &&
    /* after the PRM is received, get the OTDOA measurements */
        !(p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_OTDOA_MEAS_REQ) ))
    {
      /* Stop SUPL callflow timer and remove C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER of CF state machine */
      tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER);

      supl_set_cf_state(C_SUPL_CF_STATE_OTDOA_MEAS_REQ, 0);  // start timer to wait for OTDOA meas report
      sm_loc_lte_otdoa_get_measurements(&(p_supl_session_status->z_gnss_meas_report.z_meas_reports[SM_GNSS_MEAS_SYSTEM_GPS].z_MeasBlk.z_MeasClock), TM_PRTL_TYPE_UMTS_UP,0);
    }
    else
    {
      /* handle case when LS request OTDOA measurement (in ReqLocInfo) without providing OTDOA assitance data (in ProvLocInfo) */
      if ( ((p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_OTDOA_RCVD)) &&
          (!(p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_OTDOA_DELIVERED) ))
      {
        p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = LPP_ERROR_TYPE_PROV_LOC_OTDOA_ERROR;
        p_supl_session_status->z_supl_lpp_data.u_lpp_error_code = LPP_OTDOA_CAUSE_AD_MISSING;
      }

      if ( p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_AGNSS_RCVD )
      {
        flags |= C_LPP_AGPS_MODE_UE_ASSISTED;
      }

      if  ((p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_OTDOA_RCVD) &&
           (p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_COMBINE_AGNSS_OTDOA))
      {
        flags |= C_LPP_MODE_UE_OTDOA;
      }

      /* Just return if no ProvLocInfo data to encode.
         This would happen when LS provided with all ADs, however request only for OTDOA measurement in ReqLocInfo. */
      if ( 0 == flags )
      {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "No LPP ProvLocInfo data to encode!" );
        return;
      }

      /* Stop SUPL callflow timer and remove C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER of CF state machine */
      tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER);

      #if defined (FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
      /* Encode LPP Provide location Info message */
      if (supl_enc_lpp_prov_loc(p_supl_session_status, flags) == TRUE)
      {
        /* send SUPL POS with LPP Provide Location Info*/
        (void)supl_SuplPos_lpp_tx(p_supl_session_status, NULL);
      }
      else
      {
        p_supl_session_status->z_supl_lpp_data.q_lpp_flags &= ~(TM_SUPL_LPP_PRM_VALID);
        p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = (uint8)LPP_ERROR_TYPE_LPP_ERROR;
        p_supl_session_status->z_supl_lpp_data.u_lpp_error_code = (uint8)LPP_ERROR_UNDEF;
        if (supl_pos_enc_lpp_error_tx(NULL, p_supl_session_status, &p_lpp_tx_type) == FALSE)
        {
          MSG_ERROR("TM_LPP_UP: Error encoding LPP Error message",0,0,0);
        }
      }


      p_supl_session_status->z_supl_lpp_data.u_lpp_error_type = 0;

      /* after the PRM is received, get the OTDOA measurements */
      if ( ((p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_COMBINE_AGNSS_OTDOA) == TM_SUPL_LPP_REQ_LOC_INFO_COMBINE_AGNSS_OTDOA) &&
        ((p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_OTDOA_DELIVERED)) &&
        ((p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_OTDOA_RCVD)))
      {
        p_supl_session_status->z_supl_lpp_data.q_lpp_flags |= TM_SUPL_LPP_PRM_VALID;
        supl_set_cf_state(C_SUPL_CF_STATE_OTDOA_MEAS_REQ, 0);
        sm_loc_lte_otdoa_get_measurements(&(p_supl_session_status->z_gnss_meas_report.z_meas_reports[SM_GNSS_MEAS_SYSTEM_GPS].z_MeasBlk.z_MeasClock), TM_PRTL_TYPE_UMTS_UP,0);
      }
      #endif /* FEATURE_CGPS_LTE_CELLDB && !FEATURE_GNSS_LOW_MEMORY */
    }
  } /* LPP */

  return;
}

/*===========================================================================

FUNCTION tm_umts_up_supl_hash_proc

DESCRIPTION : Decode SUPL_INIT returned in the HASH message, and take the next
              action in a Network Initiated call flow.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void tm_umts_up_supl_hash_proc ( uint8 *p_hash_data, uint16 w_supl_init_len )
{
  uint8*              p_hash;
  supl_ULP_PDU        z_pdu_decoded;
  supl_ULP_PDU*       p_pdu_decoded = &z_pdu_decoded;

  slp_mess_header_check_result_e_type   e_slp_mess_header_check_result;

  supl_slp_session_id  z_p_provided_slp_session_id;
  supl_slp_session_id* p_provided_slp_session_id = &z_p_provided_slp_session_id;

  supl_set_session_id  z_provided_set_session_id;

  supl_status_s_type*   p_supl_session_status = &z_supl_session_status;

  uint32 q_supl_version;
  int asn1_decode_status;
  tm_cm_phone_state_info_s_type phone_state_info;
  uint8 u_area_event_session_match_num = 0xFF;

  if ( p_hash_data == NULL )
  {
    MSG_HIGH("HASH resp data pointer NULL", 0, 0, 0);
    return;
  }

  memset(&z_pdu_decoded, 0, sizeof(supl_ULP_PDU) );
  memset(p_provided_slp_session_id, 0, sizeof(supl_slp_session_id) );

  MSG_MED("SUPL - recv'ed HASH resp", 0, 0, 0);

  MSG_HIGH( "Auth Fail Flag: %d Auto Config Hslp Addr Used :%d", z_pdcomm_tcp_info.tls_session_authentication_failure,
  z_pdcomm_tcp_info.used_auto_configured_hslp_address, 0 );

  p_hash =  p_hash_data + w_supl_init_len;


   /*SUPL is not Enabled, ignore the message*/
  if(tm_umts_up_is_supl_enabled() == FALSE)
  {
    MSG_HIGH("SUPL_INIT ignored: SUPL not Enabled", 0, 0, 0);
    return;
  }
    /* If an authentication failure had occured simply try to bring up the data link.
     There is no need to proceed with Supl Init processing since already done*/
  if (z_pdcomm_tcp_info.tls_session_authentication_failure == TRUE)
  {
    memscpy( p_supl_session_status->supl_init_hash, sizeof(p_supl_session_status->supl_init_hash),
             p_hash, sizeof(p_supl_session_status->supl_init_hash));
    p_supl_session_status->u_hash_valid = TRUE;
    tm_umts_up_supl_start_slp_contact();
    return;
  }

  tm_osys_asn1_decode_pdu( p_hash_data, w_supl_init_len, (void *)p_pdu_decoded, PROTO_SUPL, &asn1_decode_status);

  /* log the message: note that the 2nd argument is not used other than for Snapper to
     display a corresponding string to indicate the "connection type", a concept that no longer
     is relevant. Snapper uses 5 for the string "SUPL" */
  sm_log_svr_txrx_rep( C_LSM_LOG_SVR_RX_TYPE, 5, C_LSM_SVR_PROTOCOL_TYPE_SUPL,
                       w_supl_init_len, p_hash_data );

  if ( asn1_decode_status == 0 )
  {

    tm_cm_iface_get_phone_state(&phone_state_info);

    /* If provisioned not to support AGPS in GSM or WCDMA for SUPL */
    if ( ( (phone_state_info.srv_system == CGPS_SRV_SYS_GSM ) &&
           ( z_umts_nv_item_ram_copy.mt_lr_support &
              NV_AAGPS_MT_LR_UP_GSM_WCDMA_BLOCKED
            )
         )   ||
         ( (phone_state_info.srv_system == CGPS_SRV_SYS_WCDMA ) &&
           ( z_umts_nv_item_ram_copy.mt_lr_support &
              NV_AAGPS_MT_LR_UP_GSM_WCDMA_BLOCKED
            )
         )
       )
    {
      MSG_MED("G/W AGPS provisioned out, SUPL_INIT ignored", 0, 0, 0);
      tm_osys_asn1_free_pdu (p_pdu_decoded , PROTO_SUPL);
      return; /* if SUPL NI for GSM/WCDMA is blocked by NV, drop SUPl_INIT */
    }

    /* determine if the incoming SUPL_INIT has SLP session ID identical to that of an existing area event triggered session */
    u_area_event_session_match_num = supl_area_event_session_match(&p_pdu_decoded->sessionID);
    if ( u_area_event_session_match_num == 0xFF ) /* no match */
    {
    e_slp_mess_header_check_result =  supl_message_overhead_proc(p_pdu_decoded, NULL);
    }
    else /* a match found - need to reject this SUPL_INIT with SUPL_END  */
    {
      MSG_HIGH("Incoming SUPL_INIT SLP SessionID same as existing areaEvent: slot %u", u_area_event_session_match_num, 0, 0);
      e_slp_mess_header_check_result = C_SLP_MESS_SLP_SESS_ID_PROBLEM_SUPLINIT_EXISTING_AREA_EVENT;
    }

    e_slp_mess_header_check_result |= supl_version_negotiation(p_pdu_decoded, &q_supl_version, 0xFF);

    /* if negotiated version is SUPL 1.0 and camped on LTE, the negotiation function would
       indicate q_supl_version being 0, drop the message */   
    if ( ( q_supl_version == 0 )  || 
         ( e_slp_mess_header_check_result & C_SLP_MESS_VERSION_CHECK_FAILED )
       )
    {
      MSG_MED("SUPL 1.0 attempt over LTE, or version check failed: SUPL_INIT ignored - %u, %lx", 
              q_supl_version, e_slp_mess_header_check_result, 0); 
      tm_osys_asn1_free_pdu (p_pdu_decoded , PROTO_SUPL);  
      return; 
    }


    if ( !( e_slp_mess_header_check_result & C_SLP_MESS_SLP_SESS_ID_MISSING ) )
    {
      /* for the consumption of later SUPL_END */
      supl_util_server_session_id_organizer ( &p_pdu_decoded->sessionID, p_provided_slp_session_id);
    }


    if ( e_slp_mess_header_check_result == C_SLP_MESS_HEADER_OK_NEW )
    {
      /* if this function returns FALSE, either version or sessionID check
      has failed, simply ignore this message */

      /* received SUPL message */

      if ( p_pdu_decoded->message.t == C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLINIT)
      {
        (void) supl_SuplInit_proc( p_pdu_decoded, p_hash_data, w_supl_init_len, p_hash );
      }
      else
      {
        /* should not happen */
        MSG_ERROR("pdcomm hash return not SUPLINIT", 0, 0, 0);
      }
    }

    else  /* something wrong with SUPL_INIT in its header */
    {

      if ( e_slp_mess_header_check_result & C_SLP_MESS_SLP_SESS_ID_MISSING )
      {
        /* p_provided_slp_session_id->present is FALSE in this case */
        (void) tm_umts_up_supl_supl_end_prepare_or_tx( p_provided_slp_session_id,
                                                NULL,
                                                supl_StatusCode_invalidSessionId,
                                                NULL,
                                                q_supl_version
                                              );
      }

      else if ( e_slp_mess_header_check_result & C_SLP_MESS_SLP_SESS_ID_PROBLEM_SUPLINIT_EXISTING_AREA_EVENT )
      {
        supl_set_session_id z_set_session_id_immediate_use;

        memset (&z_set_session_id_immediate_use, 0, sizeof(supl_set_session_id));
        if ( z_Set_Id.set_id_choice == C_SETID_UNKNOWN )
        {
          /* if SET session ID (ismi/msisdn... is still unknow at this point,
             this function will retried it */
          supl_SET_id_get();
        }
        if ( z_Set_Id.set_id_choice != C_SETID_UNKNOWN )
        {
          z_set_session_id_immediate_use.w_Session_Id = 0xFFFF; /* since only up to 16 existing sessions supported, use 0xFFFF for this purpose */
          memscpy(&z_set_session_id_immediate_use.z_Set_Id, sizeof(z_set_session_id_immediate_use.z_Set_Id),
                 &z_Set_Id, sizeof(z_Set_Id) );

          (void) tm_umts_up_supl_supl_end_prepare_or_tx(p_provided_slp_session_id,
                                                 &z_set_session_id_immediate_use,
                                                 supl_StatusCode_invalidSessionId,
                                                 p_hash,
                                                 q_supl_version);
        }
        else
        {
          MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SET_ID unavailable: %d, ignore SUPL_INT to be rejected",
                z_Set_Id.set_id_choice);

          return;
        }

      }



      else if ( e_slp_mess_header_check_result & C_SLP_MESS_VERSION_UNSUPPORTED )
      {
        (void) tm_umts_up_supl_supl_end_prepare_or_tx(p_provided_slp_session_id,
                                               NULL,
                                               supl_StatusCode_versionNotSupported,
                                               p_hash,
                                               q_supl_version);
      }
      else if ( e_slp_mess_header_check_result & C_SLP_MESS_SUPLINIT_SET_SESS_ID_PROBLEM )
      {

        /* use the SLP-sent illegal SetSessionId in SUPL_END,
           so that SLP can match the session. */
        if ( p_pdu_decoded->sessionID.m.setSessionIDPresent )
        {
          supl_set_session_id_organizer ( &p_pdu_decoded->sessionID, &z_provided_set_session_id );
          (void) tm_umts_up_supl_supl_end_prepare_or_tx(p_provided_slp_session_id,
                                                       &z_provided_set_session_id,
                                                 supl_StatusCode_invalidSessionId,
                                                 p_hash,
                                                 q_supl_version);
        }
        else
        {
          (void) tm_umts_up_supl_supl_end_prepare_or_tx(p_provided_slp_session_id,
                                                                             NULL,
                                                 supl_StatusCode_invalidSessionId,
                                                 p_hash,
                                                 q_supl_version);
        }

      }
      else
      {
        (void) tm_umts_up_supl_supl_end_prepare_or_tx(p_provided_slp_session_id,
                                               NULL,
                                               supl_StatusCode_unspecified,
                                               p_hash,
                                               q_supl_version);
      }

      if ( p_pdu_decoded->message.t != C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLINIT)
      {
        tm_umts_up_supl_callflow_disrupt_handler();
      }

    } /* if something wrong in SUPL_INIT header */

    tm_osys_asn1_free_pdu (p_pdu_decoded , PROTO_SUPL);

  } /* SUPL_INIT decoded ok */

  else /* decode status showing error */
  {
    MSG_MED("SUPL: ASN1 decoding failed", 0,0,0);
    if ( asn1_decode_status != C_ASN1_DECODE_BUFFER_NOT_ALLOCATED )
    {
      tm_osys_asn1_free_pdu (p_pdu_decoded , PROTO_SUPL);
    }
  }

}


/********************************************************************
 *
 * tm_umts_up_supl_notif_verif_resp_proc
 *
 * Function description:
 *  API function for LSM core to ask LCS Agent to process the notification
 *  verification response.
 *
 * Parameters:
 *   pdapi_ver_resp_lcs_ptr: pointer to the lcs structure of the pdapi
 *     command notif_verification_response
 *
 * Return: NONE.
 *
 *
 *********************************************************************
 */
void tm_umts_up_supl_notif_verif_resp_proc(pdsm_lcs_notification_resp_s_type *p_user_resp)
{

  pdsm_lcs_supl_notify_verify_req_s_type  *p_req_data = &p_user_resp->lcs_info.supl_req;
  supl_slp_session_id                     z_slp_id;
  pdsm_ext_status_info_s_type ext_status_info_type;
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;

  memset(&z_slp_id, 0, sizeof(supl_slp_session_id) );
  supl_convert_slp_id_from_notif_verif_resp ( p_req_data, &z_slp_id );

  /* check if there modem still needs to process the response or not. If there was a CP session
     that preempted this before teh user responded, we need to reject this NI request */
  if ( (tm_core_is_ni_resp_pending() == FALSE ) &&
       ( p_user_resp->resp == PDSM_LCS_NOTIFY_VERIFY_ACCEPT))
  {
    MSG_MED("Notif/Verif accepted by user, but was preempted by another CP NILR session. Reject this session",
            0, 0, 0);
    (void) tm_umts_up_supl_supl_end_prepare_or_tx( &z_slp_id,
                                            NULL,
                                            supl_StatusCode_consentDeniedByUser,
                                            p_req_data->supl_hash,
                                            q_supl_ver_maj_after_supl_init_glob);

    /* TBD: may need to pass ver. number to pdpai; for now clear it after use */
    q_supl_ver_maj_after_supl_init_glob = 0;

    memset((void*)&ext_status_info_type, 0, sizeof(ext_status_info_type));
    ext_status_info_type.ext_status_type = PDSM_EXT_STATUS_SUPL_SESSION_END;
    ext_status_info_type.ext_status_info.ext_supl_session_end_ind_type.supl_end_result = PDSM_PD_SUPL_SESSION_END_RESULT_FAIL;

    if ( p_supl_session_status->q_pdapi_client_id != 0 )
    {
      ext_status_info_type.client_id = p_supl_session_status->q_pdapi_client_id;
    }
    else
    {
      ext_status_info_type.client_id = pdsm_ni_client_id_get();
    }

    /*Send SUPL Session End indication to PDAPI*/
    tm_pdapi_ext_status_event_callback(PDSM_EXT_STATUS_SUPL_SESSION_END, &ext_status_info_type);
    return;
  }


  if ( p_user_resp->resp == PDSM_LCS_NOTIFY_VERIFY_ACCEPT ) /* User accepts */
  {
    MSG_MED("Notif/Verif accepted by user", 0, 0, 0);
    supl_action_upon_ni_notif_verif_accepted( p_req_data, &z_slp_id );
  }

  else if ( p_user_resp->resp ==  PDSM_LCS_NOTIFY_VERIFY_DENY ) /* User denies */
  {
    if ( p_req_data->notification_priv_type == PDSM_LCS_PRIVACY_OVERRIDE )
    {
#ifndef FEATURE_CGPS_FW_NON_STANDARD_SUPPORT
      if (tm_core_is_ni_resp_pending() == TRUE )
      {
      supl_action_upon_ni_notif_verif_accepted( p_req_data, &z_slp_id );
      MSG_MED("NotifVerif rejection by user overiden due to priv_override", 0, 0, 0);
      }
      else
      {
        /* if the request was already preempted by a CP session, drop the SUPL INIT */
        MSG_MED("Notif/Verif for PO denied by user, but was preempted by another CP NILR session. Drop SUPL session",
                0, 0, 0);
      }
#else
      MSG_MED("NotifVerif rejection by user not overiden even for priv_override", 0, 0, 0);
#endif

    }

    else
    {
      MSG_MED("Notif/Verif rejected by user", 0, 0, 0);
      (void) tm_umts_up_supl_supl_end_prepare_or_tx( &z_slp_id,
                                              NULL,
                                              supl_StatusCode_consentDeniedByUser,
                                              p_req_data->supl_hash,
                                              q_supl_ver_maj_after_supl_init_glob);

      /* TBD: may need to pass ver. number to pdpai; for now clear it after use */
      q_supl_ver_maj_after_supl_init_glob = 0;

    }
  }

  else if ( p_user_resp->resp == PDSM_LCS_NOTIFY_VERIFY_NORESP ) /* no user response */
  {
    if ( p_req_data->notification_priv_type == PDSM_LCS_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP )
    {
      MSG_MED("Notif/Verif no resp, disallow per SUPL_INIT", 0, 0, 0);
      (void) tm_umts_up_supl_supl_end_prepare_or_tx( &z_slp_id,
                                              NULL,
                                              supl_StatusCode_consentDeniedByUser,
                                              p_req_data->supl_hash,
                                              q_supl_ver_maj_after_supl_init_glob);

      /* TBD: may need to pass ver. number to pdpai; for now clear it after use */
      q_supl_ver_maj_after_supl_init_glob = 0;
    }
    else
    {
      /* all other types should result in accepting upon no response */
      MSG_MED("Notif/Verif no resp, allow per SUPL_INIT", 0, 0, 0);
      supl_action_upon_ni_notif_verif_accepted( p_req_data, &z_slp_id );
    }
  }

  else
  {
    MSG_MED("SUPL: Notif/Verif response invalid, ignored", 0, 0, 0);
  }

}


/*
 ******************************************************************************
 *
 * tm_umts_up_supl_E911_handler
 *
 * Function description:
 *  This function decides the behavior of the SUPL submodule when an E911 call
 *  is originated.
 *
 *
 * Parameters:
 *   None.
 *
 * Return value:
 *   None.
 *
 ******************************************************************************
*/

void tm_umts_up_supl_E911_handler(void)
{
  tm_cm_phone_state_info_s_type phone_state_info;
  emergency_handling_e_type e_e911_effect = C_EMERGENCY_HANDLING_NULL;


  e_e911_effect = tm_umts_common_utils_emergency_effect();

  /* If MO Sessions are allowed for E911, and we do not want an ongoing MO
   * session to be aborted on E911 orig, no need to take any action on E911
   * orig */
  if ( (e_e911_effect == C_EMERGENCY_HANDLING_ACCEPT_ALL) &&
       (tm_core_info.config_info.sm_e911_config & TM_CORE_DONT_ABORT_GPS_SESSION_ON_E911))
  {
    MSG_MED("e911_effect %d and sm_e911_config %d. Dont abort MO on E911 orig",
             e_e911_effect, tm_core_info.config_info.sm_e911_config, 0);
    return;
  }

  MSG_MED("E911 handler called", 0, 0, 0);
  memset(&phone_state_info,0x00,sizeof(tm_cm_phone_state_info_s_type));

  tm_cm_iface_get_phone_state(&phone_state_info);

  if (phone_state_info.srv_system == CGPS_SRV_SYS_GSM ||
      phone_state_info.srv_system == CGPS_SRV_SYS_WCDMA ||
      phone_state_info.srv_system == CGPS_SRV_SYS_LTE
      /*SUPL will be active protocol in case of 1x-LTE interworking,
        so handle case of 1x also*/
      || ((TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable) != 0) &&
          (phone_state_info.srv_system == CGPS_SRV_SYS_CDMA ||
           phone_state_info.srv_system == CGPS_SRV_SYS_HDR))
      )
  {
    if (z_pdcomm_tcp_info.curr_state == PDCOMM_STATE_CONNECTED)
    {
      (void)tm_umts_up_supl_supl_end_tx(&z_supl_session_status.z_session_id.z_Slp_Sess_Id,
                                  NULL,
                                  supl_StatusCode_unspecified,
                                  NULL,
                                  z_supl_session_status.q_supl_version);
    }
    if (z_supl_session_status.e_cf_state != C_SUPL_CF_STATE_NULL)
    {
    tm_umts_up_supl_inform_core_stop_session(TM_STOP_REASON_E911);
    }
  }
  /* call session init to force re-reading IMSI if needed */
    tm_umts_up_supl_session_init(FALSE);
  return;
}



/*===========================================================================

FUNCTION tm_umts_up_supl_inform_core_stop_session

DESCRIPTION
  This function informs the TM core that the current session should
  terminate.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/

void  tm_umts_up_supl_inform_core_stop_session( tm_sess_stop_reason_e_type stop_reason)
{
  supl_status_s_type          *p_supl_session_status = &z_supl_session_status;
  tm_sess_req_param_u_type    req_param;
  tm_sess_req_param_u_type    *p_req_param           = &req_param;
  pdsm_ext_status_info_s_type *pz_ext_status_info_type;

  /*Tell the IS801 if SUPL Session is terminating abnormally and if IS801 session is running*/
  if( (stop_reason != TM_STOP_REASON_COMPLETED) &&
      (p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801)
    )
  {
    (void)supl_stop_is801_session( stop_reason );
  }

  /*memset req_param*/
  memset( (void *)p_req_param, 0, sizeof(tm_sess_req_param_u_type) );

  /* Send request to stop OTDOA engine before the TM session is stopped */
#if defined (FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
  if ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP )
  {
    tm_lpp_proc_otdoa_stop_req( p_supl_session_status->tm_active_session_handle, TM_STOP_REASON_COMPLETED, TM_PRTL_TYPE_UMTS_UP );
  }
#endif /* FEATURE_CGPS_LTE_CELLDB && ! FEATURE_GNSS_LOW_MEMORY */


  p_req_param->stop_param.stop_type = TM_STOP_TYPE_SESSION;
  p_req_param->stop_param.stop_reason = stop_reason;
  if ( p_supl_session_status->u_agps_mode == C_SUPL_OTDOA_MODE ||
       p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED )
  {
    p_req_param->stop_param.op_to_stop = TM_OP_REQ_OTDOA;
  }
  MSG_MED( "SUPL: Stop TmCore session", 0, 0, 0 );

  (void)tm_sess_req( TM_PRTL_TYPE_UMTS_UP,
                     p_supl_session_status->tm_active_session_handle,
                     TM_SESS_REQ_STOP,
                     p_req_param
                    );

  pz_ext_status_info_type = (pdsm_ext_status_info_s_type *)os_MemAlloc(sizeof(pdsm_ext_status_info_s_type), OS_MEM_SCOPE_TASK);
  if (NULL == pz_ext_status_info_type)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Cannot allocate memory for pdsm_ext_status_info_s_type in tm_umts_up_supl_inform_core_stop_session");
  }
  else
  {
    memset( pz_ext_status_info_type, 0, sizeof(*pz_ext_status_info_type) );

    pz_ext_status_info_type->ext_status_type = PDSM_EXT_STATUS_SUPL_SESSION_END;

  if ( stop_reason == TM_STOP_REASON_COMPLETED )
  {
      pz_ext_status_info_type->ext_status_info.ext_supl_session_end_ind_type.supl_end_result = PDSM_PD_SUPL_SESSION_END_RESULT_SUCCESS;
  }
  else
  {
      pz_ext_status_info_type->ext_status_info.ext_supl_session_end_ind_type.supl_end_result = PDSM_PD_SUPL_SESSION_END_RESULT_FAIL;
  }


  if ( p_supl_session_status->q_pdapi_client_id != 0 )
  {
      pz_ext_status_info_type->client_id = p_supl_session_status->q_pdapi_client_id;
  }
  else
  {
      pz_ext_status_info_type->client_id = pdsm_ni_client_id_get();
  }

  /*Send SUPL Session End indication to PDAPI*/
    tm_pdapi_ext_status_event_callback( PDSM_EXT_STATUS_SUPL_SESSION_END, pz_ext_status_info_type );
    (void)os_MemFree((void **)&pz_ext_status_info_type);
  }
  /* also clear protocol submodule session related variables */
  tm_umts_up_supl_callflow_kind_set( C_SUPL_SESSION_NONE );
  p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UNDEF;
  p_supl_session_status->u_pdapi_fix_reported =  FALSE;
  p_supl_session_status->u_network_session_on = FALSE;


  /* clear all SUPL related timers / call flow states, if not done yet, except UT0_SUPLEND or
     UT3_EXPECT_SUPLREPORT */
  if ( p_supl_session_status->e_cf_state != C_SUPL_CF_STATE_NULL )
  {
    if ( !( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLEND ) &&
         !( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT3_EXPECT_SUPLREPORT )
        )
    {
      tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_NULL );
    }
    else if ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLEND )
    {
      /* if still in UT0_SUPLEND, clear all but C_SUPL_CF_STATE_UT0_SUPLEND */
      p_supl_session_status->e_cf_state = C_SUPL_CF_STATE_UT0_SUPLEND;
    }
    else if ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT3_EXPECT_SUPLREPORT )
    {
      /* if in C_SUPL_CF_STATE_UT3_EXPECT_SUPLREPORT, clear all but C_SUPL_CF_STATE_UT3_EXPECT_SUPLREPORT */
      p_supl_session_status->e_cf_state = C_SUPL_CF_STATE_UT3_EXPECT_SUPLREPORT;
    }

  }

#if defined (FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
  if ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP )
  {
    tm_lpp_tx_mgr_init( supl_lpp_handle );
    tm_lpp_init_encoding_context();
    memset( &z_lpp_provide_cap, 0, sizeof(z_lpp_provide_cap) );
  }
#endif /* FEATURE_CGPS_LTE_CELLDB && ! FEATURE_GNSS_LOW_MEMORY */
}


/********************************************************************
*
* tm_umts_up_supl_callflow_kind_set
*
* Function description:
*   Sets the nature of the call flow for the current session.
*
* Parameters:
*   enum of None, NetworkInitiated, SetInitiated, or KeepWarm.
*
* Return:
*  None
*
*********************************************************************
*/

void tm_umts_up_supl_callflow_kind_set (supl_callflow_kind_enum_type u_callflow_kind)
{
  supl_status_s_type*   p_supl_session_status = &z_supl_session_status;

  p_supl_session_status->u_callflow_kind = u_callflow_kind;

  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Call flow type set to: %u",
        u_callflow_kind);

  if ( u_callflow_kind == C_SUPL_SESSION_NI )
  {
    p_supl_session_status->u_network_session_on = TRUE;
  }

}

/********************************************************************
*
* tm_umts_up_supl_get_pdcomm_tcp_handle
*
* Function description:
*   Upon getting "camped on WCDMA", if no pdcomm TCP handle has been
*   acquired, get it now.
*
* Parameters:
*
* Return:
*  None
*
*********************************************************************
*/

boolean tm_umts_up_supl_get_pdcomm_tcp_handle(void)
{
  pd_comms_get_handle_params_type     z_pdcomm_get_handle_params;
  boolean ret_val = TRUE;

  /**** init TCP *****************************************/
  z_pdcomm_tcp_info.curr_state = PDCOMM_STATE_DISCONNECTED;
  z_pdcomm_tcp_info.recv_state = TM_SUPL_COMM_RECV_HEADER;
  z_pdcomm_tcp_info.num_bytes_to_read = 0;
  memset((void *)&z_pdcomm_tcp_info.tx_buffer, 0, sizeof(tm_supl_comm_output_buffer));


  z_pdcomm_get_handle_params.q_base_msg_id = TM_CORE_PDCOMM_MSG_ID_BASE;
  z_pdcomm_get_handle_params.e_link_type = LINK_TCP;
  if ( pd_comms_app_get_handle (&z_pdcomm_get_handle_params, &z_pdcomm_handle_tcp ) != PDCOMM_RESULT_OK )
  {
    /* shouldn't happen, fatal */
    MSG_ERROR("fails to get pdcomm handle for TCP at WCDMA camp.", 0, 0, 0);
    ret_val = FALSE;
  }

  return ret_val;
}


/*
******************************************************************************
* tm_umts_up_supl_update_mlid_db
*
* Function description:
*
*   Update multi-location ID database
*
* Parameters: cell_db_data_ptr point to the cell database info.
*
* Return value: None.
*
******************************************************************************
*/

void tm_umts_up_supl_update_mlid_db (const cgps_CellInfoCachedStruct *cell_db_data_ptr )
{

  uint8  u_i, u_slot_of_existing_identical;
  uint8  u_first_free_mlid_slot;
  uint32 q_current_time_sec = 0;
  uint32 q_loc_id_age = 0;
  uint32 q_oldest_age = 0;
  uint8  u_oldest_slot = 0;
  uint8  u_mlid_buffer_array_size;
  mlid_buffer_strcut_type *p_multi_lid_buffer;
  boolean u_cell_info_identical = FALSE;

  /* stale info. will not be saved to MLID */
  if ( (cell_db_data_ptr->q_NumCellChangesCachedCellID > 1) ||
       (cell_db_data_ptr->u_CachedCellIDValid == FALSE )
     )
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "MLID buffer rejects invalid or stale cell ID: %u, %u",
          cell_db_data_ptr->u_CachedCellIDValid,
          cell_db_data_ptr->q_NumCellChangesCachedCellID);

    return;
  }

  if ( (cell_db_data_ptr->e_ActiveRAT != CELLDB_ACTIVERAT_WCDMA) &&
       (cell_db_data_ptr->e_ActiveRAT != CELLDB_ACTIVERAT_GSM) &&
       (cell_db_data_ptr->e_ActiveRAT != CELLDB_ACTIVERAT_LTE) &&
       (cell_db_data_ptr->e_ActiveRAT != CELLDB_WIFI)
     )
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "MLID buffer rejects unsupported RAT: %u",
          cell_db_data_ptr->e_ActiveRAT);
    return;
  }

  if ( cell_db_data_ptr->u_CachedCellIDValid &&
       cell_db_data_ptr->e_ActiveRAT == CELLDB_ACTIVERAT_WCDMA  &&
       cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.cell_id == 0xFFFFFFFF )
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "MLID buffer ignores WCDMA no-cell-info:    %u ",
          cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.cell_id);
    return;
  }


  u_mlid_buffer_array_size = ARR_SIZE(z_mlid_buffer);
  p_multi_lid_buffer = &z_mlid_buffer[0];
  q_current_time_sec = time_get_secs();


  /* if no free slot is found, it will remain 0xFF */
  u_first_free_mlid_slot = 0xFF;

  /* if no slot with identical cell info. found, it will remaing 0xFF */
  u_slot_of_existing_identical = 0xFF;

  /* check if any identical CellID in MLID database and mark the first
     available slot if possible */
  for (u_i=0; u_i<u_mlid_buffer_array_size; u_i++)
  {
    if ( p_multi_lid_buffer->loc_id.u_CachedCellIDValid == FALSE )
    {
      if ( u_first_free_mlid_slot == 0xFF )
      {
        u_first_free_mlid_slot = u_i; /* lowest free MLID slot */
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "MLID buffer free slot %u found",
              u_first_free_mlid_slot);

      }
    }

    else  /* there is existing cell info. in this slot */
    {

      /* always clears the "current serving cell" flag, will be updated below when needed */
      p_multi_lid_buffer->is_the_current_serving_cell = FALSE;

      /* compare incoming cell info. with exiting one in this slot */
      u_cell_info_identical = FALSE;
      if ( p_multi_lid_buffer->loc_id.e_ActiveRAT == cell_db_data_ptr->e_ActiveRAT )
      {
        if ( p_multi_lid_buffer->loc_id.e_ActiveRAT == CELLDB_ACTIVERAT_WCDMA )
        {
          /* making sure the elements we don't use in MLID will have the same values so it will pass comparision */
          memscpy(&p_multi_lid_buffer->loc_id.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.lac[0],
                  sizeof(p_multi_lid_buffer->loc_id.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.lac),
                 &cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.lac[0],
                  sizeof(cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.lac));

          if ( memcmp ( &p_multi_lid_buffer->loc_id.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal,
                        &cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal,
                        sizeof(cgps_CellInfoWCDMAGlobalStruct)
                      ) == 0
             )
          {
            u_cell_info_identical = TRUE;
          }
        }

        #ifdef FEATURE_CGPS_LTE_CELLDB
        else if ( p_multi_lid_buffer->loc_id.e_ActiveRAT == CELLDB_ACTIVERAT_LTE )
        {

          /* making sure the elements we don't use in MLID will have the same values so it will pass comparision */
          memscpy(&p_multi_lid_buffer->loc_id.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.freq,
                 sizeof(p_multi_lid_buffer->loc_id.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.freq),
                 &cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoLTEGlobal.freq,
                 sizeof(cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoLTEGlobal.freq));

          if ( memcmp ( &p_multi_lid_buffer->loc_id.z_CellInfoGlobalCached.z_CellInfoLTEGlobal,
                        &cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoLTEGlobal,
                        sizeof(cgps_CellInfoLTEGlobalStruct)
                      ) == 0
             )
          {
            u_cell_info_identical = TRUE;
          }
        }
        #endif

        else if ( p_multi_lid_buffer->loc_id.e_ActiveRAT == CELLDB_ACTIVERAT_GSM )
        {
          /* making sure the elements we don't use in MLID will have the same values so it will pass comparision */
          memscpy(&p_multi_lid_buffer->loc_id.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.arfcn,
                 sizeof(p_multi_lid_buffer->loc_id.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.arfcn),
                 &cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoGSMGlobal.arfcn,
                 sizeof(cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoGSMGlobal.arfcn));
          p_multi_lid_buffer->loc_id.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.bsic =
                 cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoGSMGlobal.bsic;
          p_multi_lid_buffer->loc_id.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.camped_on_flag =
                 cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoGSMGlobal.camped_on_flag;

          if ( memcmp ( &p_multi_lid_buffer->loc_id.z_CellInfoGlobalCached.z_CellInfoGSMGlobal,
                        &cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoGSMGlobal,
                        sizeof(cgps_CellInfoGSMGlobalStruct)
                      ) == 0
             )
          {
            u_cell_info_identical = TRUE;
          }
        }

        else if ( p_multi_lid_buffer->loc_id.e_ActiveRAT == CELLDB_WIFI )
        {
          if ( memcmp(&p_multi_lid_buffer->loc_id.z_CellInfoGlobalCached.z_wifi_ap_cell_info.mac_address[0],
                      &cell_db_data_ptr->z_CellInfoGlobalCached.z_wifi_ap_cell_info.mac_address[0],
                      sizeof(cell_db_data_ptr->z_CellInfoGlobalCached.z_wifi_ap_cell_info.mac_address)
                     ) == 0
             )
          {
            u_cell_info_identical = TRUE;
          }
        }

      } /* if RAT matches */

      if ( u_cell_info_identical == TRUE )
      {
        /* identical location id exists, update its timestamp */
        p_multi_lid_buffer->q_timestamp = q_current_time_sec;
        if (p_multi_lid_buffer->loc_id.e_ActiveRAT == CELLDB_WIFI)
        {
          /* non RAT WIFI is never serving cell */
          p_multi_lid_buffer->is_the_current_serving_cell = FALSE;
        }
        else
        {
          /* when first camped on, WWAN cell is always serving cell and serving cells
             are not to be used by MLID. When a difference cell comes in later, if
             this one is not yet expired, it will be marked as non-serviing at that
             time */
        p_multi_lid_buffer->is_the_current_serving_cell = TRUE;
        }

        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "updating identical MLID %u (RAT %u) to timestamp %lu ",
              u_i, p_multi_lid_buffer->loc_id.e_ActiveRAT, p_multi_lid_buffer->q_timestamp);


        /* make sure all other existing MLIDs will now be marked as not-current-serviing */
        u_slot_of_existing_identical = u_i;
      }
      else  /* incoming cell info. not the same as the one in this slot */
      {
        /* check age and clear the slot if expired */
        q_loc_id_age = q_current_time_sec - p_multi_lid_buffer->q_timestamp;

        if ( q_loc_id_age  > C_MAX_NUM_CELL_HIST_SEC )
        {
          /* check the age of the existing one that is not the same of the current:
             if older than 655 seconds (about 10 min.), discard it, and mark the slot
             available */
          memset(p_multi_lid_buffer, 0, sizeof(mlid_buffer_strcut_type) );
          if ( u_supl_global_num_mlid > 0 )
          {
            u_supl_global_num_mlid--;
          }
          if ( u_first_free_mlid_slot == 0xFF )
          {
            u_first_free_mlid_slot = u_i;
          }
          MSG_5( MSG_SSID_GPSSM, MSG_LEGACY_MED,
                 "MLID %u RAT(%u) obsoleted: Age= %lu, CurrentT=%lu, timestamp=%lu",
                 u_i, p_multi_lid_buffer->loc_id.e_ActiveRAT, q_loc_id_age, q_current_time_sec, p_multi_lid_buffer->q_timestamp );


        } /* existing cell info. expired, clean up the slot */
        else
        {
          MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "existing mlid slot %u (RAT=%u): non-current-serving", u_i, p_multi_lid_buffer->loc_id.e_ActiveRAT);
        }

      }   /* else, if - incoming cell info. not the same as the one in this slot */
    } /* else, if - this is existing cell info. in this slot */

    p_multi_lid_buffer++;

  } /* MLID for-loop */

  if ( u_slot_of_existing_identical == 0xFF ) /* no identical location id found in MLID database */
  {
    /* place this one in already found first free slot the MLID database */
    if ( u_first_free_mlid_slot < u_mlid_buffer_array_size )  /* not 0xFF, a free slot found */
    {
      z_mlid_buffer[u_first_free_mlid_slot].q_timestamp = q_current_time_sec;

      if (cell_db_data_ptr->e_ActiveRAT != CELLDB_WIFI)
      {
      z_mlid_buffer[u_first_free_mlid_slot].is_the_current_serving_cell = TRUE;
      }
      else
      {
        z_mlid_buffer[u_first_free_mlid_slot].is_the_current_serving_cell = FALSE;
      }
      memscpy (&z_mlid_buffer[u_first_free_mlid_slot].loc_id,
               sizeof(z_mlid_buffer[u_first_free_mlid_slot].loc_id),
              cell_db_data_ptr, sizeof(*cell_db_data_ptr));

      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "MLID buffer slot %u populated with new data ",
            u_first_free_mlid_slot);


      u_supl_global_num_mlid++;

    }
    else /* a free slot has not been found */
    {
      p_multi_lid_buffer = &z_mlid_buffer[0];
      q_oldest_age = p_multi_lid_buffer->q_timestamp;
      for (u_i=0; u_i<u_mlid_buffer_array_size; u_i++) /* find the oldest and replace it */
      {
        if ( p_multi_lid_buffer->q_timestamp < q_oldest_age )
        {
          u_oldest_slot = u_i;
          q_oldest_age = p_multi_lid_buffer->q_timestamp;
        }
        p_multi_lid_buffer++;
      }
      if ( ( u_oldest_slot < u_mlid_buffer_array_size ) &&
           !( ( z_mlid_buffer[u_oldest_slot].loc_id.e_ActiveRAT != CELLDB_WIFI ) &&
              ( cell_db_data_ptr->e_ActiveRAT == CELLDB_WIFI )
            )
         )
      {

        /* note if full, wifi should not override non-wifi because of number of wifi APs
           could be high */

        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Override the oldest MLID slot when full: %u ",
              u_oldest_slot);

        /* replace the oldest slot with this new cell info. */
        z_mlid_buffer[u_oldest_slot].q_timestamp = q_current_time_sec;
        z_mlid_buffer[u_oldest_slot].is_the_current_serving_cell = TRUE;

        if (cell_db_data_ptr->e_ActiveRAT != CELLDB_WIFI)
        {
          z_mlid_buffer[u_oldest_slot].is_the_current_serving_cell = TRUE;
        }
        else
        {
          z_mlid_buffer[u_oldest_slot].is_the_current_serving_cell = FALSE;
        }
        memscpy (&z_mlid_buffer[u_oldest_slot].loc_id,
                 sizeof(z_mlid_buffer[u_oldest_slot].loc_id),
                 cell_db_data_ptr, sizeof(*cell_db_data_ptr));

      }
    }
  }


}


/********************************************************************
*
* supl_cell_info_sanity_chk_ok
*
* Function description:
*   Sanity checking cell db update from MGP: WCDMA or LTE.
*
* Parameters:
*
* Return:
*  Boolean: TRUE - sanity check passed; FALSE - sanity check failed.
*
*********************************************************************
*/
static boolean supl_cell_info_sanity_chk_ok( const cgps_CellInfoCachedStruct *cell_db_data_ptr )
{
  boolean u_ret = TRUE;
  uint32 q_mcc, q_mnc;
  static char s_Buf[128];

  if (cell_db_data_ptr->u_CachedCellIDValid == TRUE)
  {

    #ifdef FEATURE_CGPS_UMTS_CELLDB
    if (cell_db_data_ptr->e_ActiveRAT == CELLDB_ACTIVERAT_WCDMA )
    {
      (void) tm_umts_supl_wcdma_plmn_id_organizer
       ( (rrc_plmn_identity_type *)&cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id,
          &q_mcc,
          &q_mnc
        );

      if ( ( q_mcc > C_MCC_MNC_UPLIMIT) || ( q_mnc > C_MCC_MNC_UPLIMIT) ||
           ( ( cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.num_mnc_digits != 2  ) &&
             ( cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.num_mnc_digits != 3  )
           )  ||
           ( cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.cell_id > C_WCDMA_CELL_ID_UPLIMIT ) ||
           ( cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.psc > C_WCDMA_CELL_PSC_UPLIMIT ) ||
           ( cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.freq > C_WCDMA_CELL_FREQ_UPLIMIT )
         )
      {

        MSG_6( MSG_SSID_GPSSM, MSG_LEGACY_MED,
                  "CM updating WCDMA CellDB with bogus data: MCC=%lu, MNC=%lu, mnc_digits=%u, CI=%lu, FREQ=%u, PSC=%u\n",
                  q_mcc,
                  q_mnc,
                  cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id.num_mnc_digits,
                  cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.cell_id,
                  cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.freq,
                  cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.psc);

        u_ret = FALSE;
      }
    }
    #endif

    #ifdef FEATURE_CGPS_LTE_CELLDB
    if (cell_db_data_ptr->e_ActiveRAT == CELLDB_ACTIVERAT_LTE )
    {

      (void) tm_umts_supl_lte_plmn_id_organizer
       ( (lte_rrc_plmn_s *)&cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn,
          &q_mcc,
          &q_mnc
        );


      if ( ( q_mcc > C_MCC_MNC_UPLIMIT) || ( q_mnc > C_MCC_MNC_UPLIMIT) ||
           ( ( cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.num_mnc_digits != 2  ) &&
             ( cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.num_mnc_digits != 3  )
           )   ||
           ( cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoLTEGlobal.cell_identity > C_LTE_CELL_GLOBAL_CELLID_UPLIMIT ) ||
           ( cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoLTEGlobal.cell_id > C_LTE_CELL_PHY_CELLID_UPLIMIT )
         )
      {
        uint32 q_num_mnc_digits;

        /* LTE API has either uint32 or uint8 depending on version; for compatibility with both, cast to uint32 here */
        q_num_mnc_digits = (uint32)cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.num_mnc_digits;

        MSG_6( MSG_SSID_GPSSM, MSG_LEGACY_MED,
                  "CM updating LTE CellDB with bogus data: MCC=%lu, MNC=%lu, mnc_digits=%lu, GlbCellID=%lu, PhyCellID=%u, tac=%u\n",
                  q_mcc,
                  q_mnc,
                  q_num_mnc_digits,
                  cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoLTEGlobal.cell_identity,
                  cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoLTEGlobal.cell_id,
                  cell_db_data_ptr->z_CellInfoGlobalCached.z_CellInfoLTEGlobal.tac);

        u_ret = FALSE;
      }
    }
    #endif

  }

  return u_ret;

}


/********************************************************************
*
* tm_umts_up_supl_cell_db_update
*
* Function description:
*   Processing cell db update from MGP: WCDMA, LTE or OOS.
*
* Parameters:
*
* Return:
*  None
*
*********************************************************************
*/
void tm_umts_up_supl_cell_db_update( const cgps_CellInfoCachedStruct *cell_db_data_ptr )
{
  uint32 q_mcc, q_mnc;
  static char s_Buf[128];

  if ( supl_cell_info_sanity_chk_ok(cell_db_data_ptr)== FALSE )
  {
    return;
  }

  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "CM cell DB update CellIDValid_ME : %u, CellIDValid_SUPL : %u",
          cell_db_data_ptr->u_CachedCellIDValid,
          z_cell_info_cached.u_CachedCellIDValid);
  // Check if the cell-id information received from CM is valid
  if(cell_db_data_ptr->u_CachedCellIDValid == FALSE)
  {
    // Copy the number of cell changes information to the cached cell info if the cached cell info is valid.
    if(z_cell_info_cached.u_CachedCellIDValid == TRUE)
    {
      z_cell_info_cached.q_NumCellChangesCachedCellID = cell_db_data_ptr->q_NumCellChangesCachedCellID;
    }
  }
  else
  {
    // The cell-id information received from CM is valid , so update the cached cell info .
    memscpy(&z_cell_info_cached, sizeof(z_cell_info_cached),
             cell_db_data_ptr, sizeof(*cell_db_data_ptr) );
  }

  tm_umts_up_supl_update_mlid_db(cell_db_data_ptr);

  /************** for displaying diag. message only *********************/
  q_mcc = 0;
  q_mnc = 0;

  if ( z_cell_info_cached.e_ActiveRAT == CELLDB_ACTIVERAT_WCDMA )
  {
    (void) tm_umts_supl_wcdma_plmn_id_organizer
     ( &z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id,
        &q_mcc,
        &q_mnc
      );
    MSG_7( MSG_SSID_GPSSM, MSG_LEGACY_MED,
              "CM updating WCDMA CellDB: Valid=%u,NChanges=%lu,MCC=%lu,MNC=%lu,CI=%lu,FREQ=%u,PSC=%u\n",
              z_cell_info_cached.u_CachedCellIDValid,
              z_cell_info_cached.q_NumCellChangesCachedCellID,
              q_mcc,
              q_mnc,
              z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.cell_id,
              z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.freq,
              z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.psc);

  }

  #ifdef FEATURE_CGPS_LTE_CELLDB

  else if ( z_cell_info_cached.e_ActiveRAT == CELLDB_ACTIVERAT_LTE )
  {

    (void) tm_umts_supl_lte_plmn_id_organizer
     ( &z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn,
        &q_mcc,
        &q_mnc
      );

    MSG_6( MSG_SSID_GPSSM, MSG_LEGACY_MED,
              "CM updating LTE CellDB: Valid=%u, MCC=%lu,MNC=%lu, GlbCellID=%lu, PhyCellID=%u, tac=%u\n",
              z_cell_info_cached.u_CachedCellIDValid,
              q_mcc,
              q_mnc,
              z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.cell_identity,
              z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.cell_id,
              z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.tac);

  }
  else   /* non-W non-L , could be among other things, an OOS indicator */
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "CM updating CellDB for RAT: %u, validity = %u",
          z_cell_info_cached.e_ActiveRAT,
          z_cell_info_cached.u_CachedCellIDValid);
  }

  #else /* not defined LTE_CELL_ID: */

  else  /* non-W, could be among other things, an OOS indicator */
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "CM updating CellDB for RAT: %u, validity = %u",
          z_cell_info_cached.e_ActiveRAT,
          z_cell_info_cached.u_CachedCellIDValid);
  }

  #endif
}


#ifdef FEATURE_CGPS_WCDMA
/*===========================================================================

FUNCTION
  tm_umts_up_supl_send_wcdma_cell_meas_req

DESCRIPTION
  This function requests cell measurement data from WCDMA L1.

PARAMETERS
   None.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None.

===========================================================================*/

void tm_umts_up_supl_send_wcdma_cell_meas_req(void)
{
  /* Send WCDMA cell measurement request */
  #ifdef FEATURE_CGPS_DUAL_WCDMA
  tm_l1_iface_send_wcdma_cell_meas_req(tm_core_get_current_dd_subs_id());
  #else
  tm_l1_iface_send_wcdma_cell_meas_req();
  #endif
}
#endif /* FEATURE_CGPS_WCDMA */

/*===========================================================================

FUNCTION
  tm_umts_up_supl_cell_meas_update

DESCRIPTION
  This function copies the cell-measurements sent from the WCDMA L1, into the
  database (if they are valid).

PARAMETERS

DEPENDENCIES
   None.

RETURN VALUE
   None

SIDE EFFECTS
   None.

===========================================================================*/

void tm_umts_up_supl_cell_meas_update( wl1_lsm_cell_meas_update_type   *cell_meas_update_ptr )
{
  MSG_MED("Update cell meas, num. cell = %u", cell_meas_update_ptr->num_cells, 0, 0 );
  if ( cell_meas_update_ptr->num_cells )
  {
    z_DBCellMeas = *cell_meas_update_ptr;
  }
}


#ifdef FEATURE_CGPS_LTE_CELLDB
/*===========================================================================

FUNCTION
  tm_umts_up_supl_lte_cell_meas_update

DESCRIPTION
  This function copies the cell-measurements sent from the LTE L1, into the
  database (if they are valid).

PARAMETERS

DEPENDENCIES
   None.

RETURN VALUE
   None

SIDE EFFECTS
   None.

===========================================================================*/

void tm_umts_up_supl_lte_cell_meas_update(lte_ml1_gps_mrl_cell_meas_update_t *cell_meas_update_ptr )
{
  if ( memcmp ( &z_lte_mrl, cell_meas_update_ptr, sizeof(lte_ml1_gps_mrl_cell_meas_update_t)) )
  {
    memscpy (&z_lte_mrl, sizeof(z_lte_mrl), cell_meas_update_ptr, sizeof(*cell_meas_update_ptr));
    MSG_HIGH("LTE MRL updated", 0, 0, 0);
  }
  else
  {
    MSG_HIGH("LTE MRL report, no new info", 0, 0, 0);
  }

}
#endif  /* SUPL2 + LTE_CELL_DB */



/********************************************************************
 *
 * tm_umts_up_supl_assist_req_holdoff_timers_check_start
 *
 * Function description:
 *  Check and Start assistance data request holdoff timers.
 *
 * Parameters:
 *  p_wishlist_mask: pionter to the assistance data wish list.
 *
 * Return:
 *  None.
 *
 *********************************************************************
 */

void  tm_umts_up_supl_assist_req_holdoff_timers_check_start ( uint32 *p_wishlist_mask )
{
  int32 l_long_word = 0;

  /* Sanity */
  if ( NULL == p_wishlist_mask )
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "NULL GPS AD wishlist");
    return;
  }

  /* 
  * Check the GPS hold-off timers to see if any of them needs to hold off. 
  * The hold-off timer is applicable only in non-NI session.
  */
  if ( C_SUPL_SESSION_NI == z_supl_session_status.u_callflow_kind )
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_MED, "AD request hold-off timer is inactive for NI" );
    return;
  }

  /* NOTE : GPS / GANSS uses the same bit mask for wishlist mask */
  if ( *p_wishlist_mask & C_GPS_ASSIS_ALM )
  {
    l_long_word = os_TimerCheck( supl_alm_req_holdoff_timer);
    if ( l_long_word > 0 )
    {
      *p_wishlist_mask &= ~C_GPS_ASSIS_ALM;
      MSG_MED("GPS alm holdoff remaining = %ld msec", l_long_word, 0, 0 );
    }
  }

  if ( *p_wishlist_mask & C_GPS_ASSIS_NAVMODL )
  {
    l_long_word = os_TimerCheck( supl_eph_req_holdoff_timer);
    if ( l_long_word > 0 )
    {
      *p_wishlist_mask &= ~C_GPS_ASSIS_NAVMODL;
      MSG_MED("GPS eph holdoff remaining = %ld msec", l_long_word, 0, 0 );
    }
  }
  if ( *p_wishlist_mask & C_GPS_ASSIS_IONO )
  {
    l_long_word = os_TimerCheck( supl_iono_req_holdoff_timer);
    if ( l_long_word > 0 ) {
      *p_wishlist_mask &= ~C_GPS_ASSIS_IONO;
      MSG_MED("GPS IONO holdoff remaining = %ld msec", l_long_word, 0, 0 );
    }
  }
  if ( *p_wishlist_mask & C_GPS_ASSIS_UTC )
  {
    l_long_word = os_TimerCheck( supl_utc_req_holdoff_timer);
    if ( l_long_word > 0 )
    {
      *p_wishlist_mask &= ~C_GPS_ASSIS_UTC;
      MSG_MED("GPS UTC holdoff remaining = %ld msec", l_long_word, 0, 0 );
    }
  }
  if ( *p_wishlist_mask & C_GPS_ASSIS_REFLOC )
  {
    l_long_word = os_TimerCheck( supl_refloc_req_holdoff_timer);
    if ( l_long_word > 0  )
    {
      *p_wishlist_mask &= ~C_GPS_ASSIS_REFLOC;
      MSG_MED("GPS RefL holdoff remaining = %ld msec", l_long_word, 0, 0 );
    }
  }

  /*** if any of eph. or alm. is requested, always request refT and refL */
  if ( ( *p_wishlist_mask & C_GPS_ASSIS_NAVMODL ) ||
       ( *p_wishlist_mask & C_GPS_ASSIS_ALM )
     )
  {
    *p_wishlist_mask |=  C_GPS_ASSIS_REFTIME |
                         C_GPS_ASSIS_REFLOC |
                         C_GPS_ASSIS_RTI;
  }

  /******* activate assistance data request holdoff timers *********/

  if ( *p_wishlist_mask & C_GPS_ASSIS_ALM )
  {
    (void) os_TimerStart( supl_alm_req_holdoff_timer, C_DEFER_ALM_REQ_MSEC, NULL );
  }

  if ( *p_wishlist_mask & C_GPS_ASSIS_NAVMODL )
  {
    (void) os_TimerStart( supl_eph_req_holdoff_timer, C_DEFER_EPH_REQ_MSEC, NULL );
  }

  if ( *p_wishlist_mask & C_GPS_ASSIS_IONO )
  {
    (void) os_TimerStart( supl_iono_req_holdoff_timer, C_DEFER_IONO_REQ_MSEC, NULL );
  }
  if ( *p_wishlist_mask & C_GPS_ASSIS_UTC )
  {
    (void) os_TimerStart( supl_utc_req_holdoff_timer, C_DEFER_UTC_REQ_MSEC, NULL );
  }
  if ( *p_wishlist_mask & C_GPS_ASSIS_REFLOC )
  {
    (void) os_TimerStart( supl_refloc_req_holdoff_timer, C_DEFER_REFLOC_REQ_MSEC, NULL );
  }

}



/********************************************************************
 *
 * tm_umts_up_supl_glo_assist_req_holdoff_timers_check_start
 *
 * Function description:
 *  Check and Start GLONASS assistance data request holdoff timers.
 *
 * Parameters:
 *  p_wishlist_mask: pionter to the assistance data wish list.
 *
 * Return:
 *  None.
 *
 *********************************************************************
 */

void  tm_umts_up_supl_glo_assist_req_holdoff_timers_check_start ( uint32 *p_wishlist_mask )
{
  int32 l_long_word = 0;

  /* Sanity */
  if ( NULL == p_wishlist_mask )
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "NULL GLO AD wishlist");
    return;
  }

  /* 
  * Check the GLONASS hold-off timers to see if any of them needs to hold off. 
  * The hold-off timer is applicable only in non-NI session.
  */
  if ( C_SUPL_SESSION_NI == z_supl_session_status.u_callflow_kind )
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_MED, "GLO AD request hold-off timer is inactive for NI" );
    return;
  }

  if ( tm_umts_common_glonass_enabled() )
  {
    /*** check the hold-off timers to see if any of them needs to hold off ***/
    /* NOTE : GPS / GANSS uses the same bit mask for wishlist mask */
    if ( *p_wishlist_mask & C_GPS_ASSIS_ALM )
    {
      l_long_word = os_TimerCheck( supl_glo_alm_req_holdoff_timer);
      if ( l_long_word > 0 )
      {
        *p_wishlist_mask &= ~C_GPS_ASSIS_ALM;
        MSG_MED("GLO alm holdoff remaining = %ld msec", l_long_word, 0, 0 );
      }
    }

    if ( *p_wishlist_mask & C_GPS_ASSIS_NAVMODL )
    {
      l_long_word = os_TimerCheck( supl_glo_eph_req_holdoff_timer);
      if ( l_long_word > 0 )
      {
        *p_wishlist_mask &= ~C_GPS_ASSIS_NAVMODL;
        MSG_MED("GLO eph holdoff remaining = %ld msec", l_long_word, 0, 0 );
      }
    }

    /******* activate assistance data request holdoff timers *********/

    if ( *p_wishlist_mask & C_GPS_ASSIS_ALM )
    {
      (void) os_TimerStart( supl_glo_alm_req_holdoff_timer, C_DEFER_ALM_REQ_MSEC, NULL );
    }

    if ( *p_wishlist_mask & C_GPS_ASSIS_NAVMODL )
    {
      (void) os_TimerStart( supl_glo_eph_req_holdoff_timer, C_DEFER_EPH_REQ_MSEC, NULL );
    }
  }
}


/********************************************************************
 *
 * tm_umts_up_supl_pa_proc
 *
 * Function description:
 *  Processes PA events from TM Core.
 *
 * Parameters:
 *
 * Return:
 *  None.
 *
 *********************************************************************
 */

void tm_umts_up_supl_pa_proc ( prtl_pa_event_s_type *p_pa_event  )
{
  uint8 nv_cgps_umts_pde_server_addr_url[C_MAX_URL_SIZE_BYTE];
  char port_string[6];
  uint16 i =0; uint16 j=0;

  if (p_pa_event == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL pointer");
    return;
  }

  /* NV item limit: 128 bytes */
  /* This setting to 0 in loop is needed to make Klockwork happy */
  for (i=0; i< C_MAX_URL_SIZE_BYTE; i++)
  {
    nv_cgps_umts_pde_server_addr_url[i] = 0;
  }
  port_string[0] = '\0';

  if(( p_pa_event->pa_event == PDSM_PA_DELETE_PARAMS_EX ) ||
     ( p_pa_event->pa_event == PDSM_PA_DELETE_PARAMS ) || /* PDSM_PA_DELETE_PARAMS is deprecated. */
     ( p_pa_event->pa_event == PDSM_PA_DELETE_PARAMS_EX1 )) /* PDSM_PA_DELETE_PARAMS is deprecated.
                                                           use PDSM_PA_DELETE_PARAMS_EX instead */
  {
    /* cancel holdoff timer when the corresponding param. is deleted */

    MSG_5(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Clear SUPL throttling timers. PA delete param 0x%lx, Eph 0x%lx, Alm 0x%lx, GloEph 0x%lx, GloAlm 0x%lx",
             p_pa_event->pa_info_ptr->delete_params.pdsm_delete_parms_flags,
          p_pa_event->pa_info_ptr->delete_params.eph_sv_mask,
          p_pa_event->pa_info_ptr->delete_params.alm_sv_mask,
          p_pa_event->pa_info_ptr->delete_params.glo_eph_sv_mask,
          p_pa_event->pa_info_ptr->delete_params.glo_alm_sv_mask
          );

    if ( p_pa_event->pa_info_ptr->delete_params.pdsm_delete_parms_flags &
         PDSM_PA_DELETE_EPH
       )
    {
      if ( p_pa_event->pa_info_ptr->delete_params.eph_sv_mask != 0 )
      {
        (void) os_TimerStop( supl_eph_req_holdoff_timer);
      }
    }
    if ( p_pa_event->pa_info_ptr->delete_params.pdsm_delete_parms_flags &
         PDSM_PA_DELETE_ALM
       )
    {
      if (p_pa_event->pa_info_ptr->delete_params.alm_sv_mask != 0 )
      {
        (void) os_TimerStop( supl_alm_req_holdoff_timer);
      }
    }

    if ( tm_umts_common_glonass_enabled() )
    {
      if ( p_pa_event->pa_info_ptr->delete_params.pdsm_delete_parms_flags &
           PDSM_PA_DELETE_ALM_GLO
         )
      {
        if (p_pa_event->pa_info_ptr->delete_params.glo_alm_sv_mask != 0 )
        {
          (void) os_TimerStop( supl_glo_alm_req_holdoff_timer);
        }
      }
      if ( p_pa_event->pa_info_ptr->delete_params.pdsm_delete_parms_flags &
           PDSM_PA_DELETE_EPH_GLO
         )
      {
        if ( p_pa_event->pa_info_ptr->delete_params.glo_eph_sv_mask != 0 )
        {
          (void) os_TimerStop( supl_glo_eph_req_holdoff_timer);
        }
      }
    }


    if ( p_pa_event->pa_info_ptr->delete_params.pdsm_delete_parms_flags &
         PDSM_PA_DELETE_IONO
       )
    {
        (void) os_TimerStop( supl_iono_req_holdoff_timer);
    }
    if ( p_pa_event->pa_info_ptr->delete_params.pdsm_delete_parms_flags &
         PDSM_PA_DELETE_UTC
       )
    {
      (void) os_TimerStop( supl_utc_req_holdoff_timer);
    }
    if ( p_pa_event->pa_info_ptr->delete_params.pdsm_delete_parms_flags &
         PDSM_PA_DELETE_POS
       )
    {
      (void) os_TimerStop( supl_refloc_req_holdoff_timer);
      memset(&z_supl_cached_pos_for_net, 0, sizeof(supl_cached_pos_for_net_s_type));
    }

  } /* delete param */


  else if (p_pa_event->pa_event == PDSM_PA_POSITION_MODE)
  {
    /* for UMTS CP and UP, it is also updated in the CP event handler
       in case only not both CP and UP are included in the build */
    TM_CORE_CONFIG_DD(positioning_modes_supported) = p_pa_event->pa_info_ptr->position_mode;
  }
  else if ( p_pa_event->pa_event == PDSM_PA_UMTS_PDE_NETWORK_ADRS )
  {
    /* for SUPL only */
    if ( p_pa_event->pa_info_ptr->server_address.server_addr_type ==
         PDSM_SERVER_TYPE_IPV4_ADDRESS )
    {
      z_umts_nv_item_ram_copy.slp_ip_addr =
        p_pa_event->pa_info_ptr->server_address.server_adrs.server_adrs_v4.server_adrs;
    }
    else if ( p_pa_event->pa_info_ptr->server_address.server_addr_type ==
              PDSM_SERVER_TYPE_URL_ADDRESS )
    {
      /* NV item limit: 128 bytes */
      memset( &nv_cgps_umts_pde_server_addr_url[0], 0, C_MAX_URL_SIZE_BYTE);

      if ( p_pa_event->pa_info_ptr->server_address.server_adrs.server_adrs_url.url_length >
           C_MAX_URL_SIZE_BYTE  )
      {
        MSG_HIGH("UMTS_PDE_NETWORK_ADRS too long for NV: %u",
                  p_pa_event->pa_info_ptr->server_address.server_adrs.server_adrs_url.url_length,
                  0,
                  0);
      }

    memscpy( &nv_cgps_umts_pde_server_addr_url[0],
      sizeof(nv_cgps_umts_pde_server_addr_url),
        &p_pa_event->pa_info_ptr->server_address.server_adrs.server_adrs_url.url[0],
        p_pa_event->pa_info_ptr->server_address.server_adrs.server_adrs_url.url_length);

      /* organize UMTS PDE server URL address */
      if ( ((nv_cgps_umts_pde_server_addr_url[0]=='h') || (nv_cgps_umts_pde_server_addr_url[0]=='H')) &&
           ((nv_cgps_umts_pde_server_addr_url[1]=='t') || (nv_cgps_umts_pde_server_addr_url[1]=='T')) &&
           ((nv_cgps_umts_pde_server_addr_url[2]=='t') || (nv_cgps_umts_pde_server_addr_url[2]=='T')) &&
           ((nv_cgps_umts_pde_server_addr_url[3]=='p') || (nv_cgps_umts_pde_server_addr_url[3]=='P')) &&
            (nv_cgps_umts_pde_server_addr_url[4]==':') && (nv_cgps_umts_pde_server_addr_url[5]=='/')  && (nv_cgps_umts_pde_server_addr_url[6]=='/') )
      {
        j = 7;  /* Start extracting hslp_address after the URL prefix. */
      }
      else
      {
        j = 0;  /* No http[colon][slash][slash] in nv_cgps_umts_pde_server_addr_url. */
      }

      i = j;
      while ( (nv_cgps_umts_pde_server_addr_url[i] != ':') && (nv_cgps_umts_pde_server_addr_url[i] != '\0') && ( i < ( C_MAX_URL_SIZE_BYTE - 1 ) ) )
      {
        z_umts_nv_item_ram_copy.slp_address[i-j] = nv_cgps_umts_pde_server_addr_url[i] ;
        i++;
      }

      /* this IF is only here to satisfy some sanity checks, it is guaranteed
         already by logic above */
      if ( i >= j )
      {
        z_umts_nv_item_ram_copy.slp_address[i-j] = '\0' ;  /* null terminated FQDN */
      }

      i++;
      if ( i >= C_MAX_URL_SIZE_BYTE ) /* logically, it couldn't be greater than 128 */
      {
        /* no port number in nv_item aagps_defaul_url, use OMA default */
        z_umts_nv_item_ram_copy.slp_port_number = SUPL_PORT_NUM_OFFICIAL_V1_0;
      }
      else
      {
        j = i;

        while ( (nv_cgps_umts_pde_server_addr_url[i] != '\0') &&
                ( i < ( C_MAX_URL_SIZE_BYTE - 1 ) ) &&
                ( (i-j) < 6)
              )
        {
          if ( ( (i-j) < 6) && ( (i-j) >= 0 ) )
          {
            port_string[i-j] = nv_cgps_umts_pde_server_addr_url[i];
          }
          i++;
        }

        if ( ( (i-j) < 6) && ( (i-j) >= 0 ) )
        {
          port_string[i-j] = '\0';  /* null terminated string */
        }

        z_umts_nv_item_ram_copy.slp_port_number = (uint16) atoi(port_string); /* no range check if port_string > "65535" */
        if ( z_umts_nv_item_ram_copy.slp_port_number == 0 )
        {
          /* no valid port number, use OMA default */
          z_umts_nv_item_ram_copy.slp_port_number = SUPL_PORT_NUM_OFFICIAL_V1_0;
        }
      }

    } /* UMTS PDE server URL */
  }
  else if (p_pa_event->pa_event == PDSM_PA_GPS_LOCK)
  {
      z_umts_nv_item_ram_copy.gps_lock = p_pa_event->pa_info_ptr->gps_lock;
  }
  else if (p_pa_event->pa_event == PDSM_PA_EFS_DATA)
  {
      /* SUPL Root Cert dir updated */
      tm_umts_up_supl_comm_root_certs_updated();
  }
  else if (p_pa_event->pa_event == PDSM_PA_SUPL_VERSION)
  {
      /* SUPL version info.: Bytes from MSB to LSB: rsvd, major, minor, serviceIndicator */
      z_umts_nv_item_ram_copy.gnss_supl_version = p_pa_event->pa_info_ptr->supl_version;
  }
  else if (p_pa_event->pa_event == PDSM_PA_SUPL_SECURITY)
  {
      z_umts_nv_item_ram_copy.aagps_use_transport_security = (uint8)p_pa_event->pa_info_ptr->supl_security;
  }
  #if defined (FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
  else if ( p_pa_event->pa_event == PDSM_PA_LPP_CONFIG_INFO )
  {
    if ( p_pa_event->pa_info_ptr->lpp_config_info & PDSM_PA_LPP_IN_LTE_UP_ENABLE)
    {
      tm_umts_up_supl_lpp_tx_mgr_init();
    }
    else
    {
      tm_umts_up_supl_lpp_tx_mgr_dereg();
    }
  }
  #endif /* FEATURE_CGPS_LTE_CELLDB && !FEATURE_GNSS_LOW_MEMORY */
  else
  {
    MSG_MED("PDAPI PA event not processed by SUPL: %u",
             p_pa_event->pa_event, 0, 0);
  }


}


/******************************************************************************
 * tm_umts_supl_efs_dir_init
 *
 * Function description:
 *   Ceate the SUPL directory in the EFS
 *
 *   Added EFS directories for chained certificate support.
 *
 * Parameters:   None
 *
 * Return value: TRUE: successful; FALSE: unsucessful
 *
 ******************************************************************************
*/

boolean tm_umts_supl_efs_dir_init(void)
{
  /* efs_mkdir returns -1 regardless of any error. WE cannot differentiate between
     "Directory already exists" or "unable to create directory" 'coz of any EFS issues.
   For now, just return TRUE regardless. */
  (void) efs_mkdir("/SUPL", ALLPERMS);

 /* Added EFS directories for chained certificate support. */
  (void) efs_mkdir("/client-cert", ALLPERMS);
  (void) efs_mkdir("/client-key", ALLPERMS);
  (void) efs_mkdir("/cert", ALLPERMS);

  return TRUE;

}


/******************************************************************************
 * tm_umts_supl_if_need_forced_wcmda_cell_inq
 *
 * Function description:
 *   Check to see if it needs to request WCDMA cell info. directly; Usually
 *   needed for CELL_DCH.
 *
 * Parameters:   None
 *
 * Return value: TRUE: Need direct request of WCDMA cell info.;
 *               FALSE: no need for direct request of WCDMA cell info.
 *
 ******************************************************************************
*/

boolean tm_umts_supl_if_need_forced_wcmda_cell_inq (void)
{
  boolean ret = FALSE;

  static char s_Buf[128];
  uint32 q_mcc, q_mnc;
  tm_cm_phone_state_info_s_type phone_state_info;

  /* Query TM-CM module to get current acquired system */
  tm_cm_iface_get_phone_state(&phone_state_info);

  if ( phone_state_info.srv_system == CGPS_SRV_SYS_WCDMA )
  {

    if ( !z_cell_info_cached.u_CachedCellIDValid )
    {
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "W-cell inq: no cached cell. %u",
              z_cell_info_cached.u_CachedCellIDValid);
        return TRUE;  /* no cell db while in WCDMA, need directy inq. */
    }


    if ( z_cell_info_cached.e_ActiveRAT !=
         CELLDB_ACTIVERAT_WCDMA )
    {

      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "W-cell inq: in W but cell_DB RAT = %u",
            z_cell_info_cached.e_ActiveRAT);

      return TRUE; /* need direct inq. for WCDMA cell info. */
    }


    /*** handle WCDMA RAT matching CELL ID RAT: check for staleness ***/

    (void) tm_umts_supl_wcdma_plmn_id_organizer
    ( &z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id,
      &q_mcc,
      &q_mnc );

    if ( z_cell_info_cached.q_NumCellChangesCachedCellID == 1 )
    {

      MSG_5( MSG_SSID_GPSSM, MSG_LEGACY_MED,
                "Valid cell_id avail: MCC=%lu,MNC=%lu,CI=%lu,FREQ=%u,PSC=%u\n",
                q_mcc,
                q_mnc,
                z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.cell_id,
                z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.freq,
                z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.psc);

      /* cached WCDMA cell info. valid, no need to directly request for it */

    }

    else /* stale cell info. */
    {
      MSG_6( MSG_SSID_GPSSM, MSG_LEGACY_MED,
                "Stale cell_id: NChanges=%lu,MCC=%lu,MNC=%lu,CI=%lu,FREQ=%u,PSC=%u\n",
                z_cell_info_cached.q_NumCellChangesCachedCellID,
                q_mcc,
                q_mnc,
                z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.cell_id,
                z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.freq,
                z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.psc);

      ret = TRUE; /* need direct WCDMA cell info. inq. */
    }

  } /* RAT is WCDMA */

  return ret;
}


/******************************************************************************
 * tm_umts_up_supl_ck_req_tdscmda_cell_info
 *
 * Function description:
 *   Check to see if it is on TDSCDMA and if so request cell information.
 *
 * Parameters:  None.
 *
 * Return: None.
 *
 ******************************************************************************
*/

void tm_umts_up_supl_ck_req_tdscmda_cell_info(void)
{

  #ifdef FEATURE_SUPL1_TDS /* defined in custcgps.h, if .builds defines FEATURE_CGPS_USES_TDS */

  tm_cm_phone_state_info_s_type phone_state_info;

  /* Query TM-CM module to get current acquired system */
  tm_cm_iface_get_phone_state(&phone_state_info);

  if ( phone_state_info.srv_system == CGPS_SRV_SYS_TDSCDMA )
  {

    tdsrrcmeas_plmn_cell_info_struct_type     tds_plmn_cell_info;

    memset(&tds_plmn_cell_info, 0, sizeof(tdsrrcmeas_plmn_cell_info_struct_type) );

    /* until there is any mechanism to get cell change indication and DCH cell info,
       use this camping event to cache cell info. first */

    if ( gnss_wwan_iface_tdsrrcmeas_get_cell_plmn_info(&tds_plmn_cell_info) )
    {

      z_cell_info_cached.u_CachedCellIDValid = TRUE;
      z_cell_info_cached.e_ActiveRAT = CELLDB_ACTIVERAT_TDSCDMA;

      z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.cell_id =
        tds_plmn_cell_info.cell_id;


      z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.lac[0] =
        tds_plmn_cell_info.lac[0];

      z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.lac[1] =
        tds_plmn_cell_info.lac[1];

      memscpy( &z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.plmn_id,
               sizeof(z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.plmn_id),
              &tds_plmn_cell_info.plmn_id, sizeof( tds_plmn_cell_info.plmn_id )
            );

      MSG_7(MSG_SSID_GPSSM, MSG_LEGACY_MED,
            "Camped on TDS - cell info.: MCC[0]=%u [1]=%u [2]=%u, MNC[0]=%u  [1]=%u [2]=%u%, N_MNC=%u",
            z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.plmn_id.mcc[0],
            z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.plmn_id.mcc[1],
            z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.plmn_id.mcc[2],
            z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.plmn_id.mnc[0],
            z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.plmn_id.mnc[1],
            z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.plmn_id.mnc[2],
            z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.plmn_id.num_mnc_digits
            );
      MSG_4(MSG_SSID_GPSSM, MSG_LEGACY_MED,
            "TDS - cell info.: LAC[0]=%u [1]=%u (LAC=%u), CI=%lu",
            z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.lac[0],
            z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.lac[1],
            z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.lac[0] * 256 +
            z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.lac[1],
            z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.cell_id
            );


    }
    else
    {
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,
            "Failed obtaining TDS cell info.: %u", phone_state_info.srv_system );
    }

  } /* camped on TDSCMDA */

  #endif /* #if defined feature */

}


/******************************************************************************
 * tm_umts_supl_wcdma_plmn_id_organizer
 *
 * Function description:
 *   Converts MCC and MNC stored in the format of rrcmeas_plmn_cell_info to
 *   uint32.
 *
 * Parameters:
 *  p_cell_id_info: pointer to the structure containing plmn cell
 *                 information;
 *  p_mcc: pointer for the this function to write converted MCC value to.
 *  p_mnc: pointer for the this function to write converted MNC value to.
 *
 * Return:
 *   TRUE/FALSE.
 *
 ******************************************************************************
*/

boolean tm_umts_supl_wcdma_plmn_id_organizer
( rrc_plmn_identity_type *p_plmn_id_info,
  uint32 *p_mcc,
  uint32 *p_mnc
)
{

  if ( (p_mcc == NULL) || (p_mnc==NULL) || (p_plmn_id_info == NULL) )
  {
    return FALSE;
  }

  *p_mcc = ( p_plmn_id_info->mcc[0] * 100 ) +
           ( p_plmn_id_info->mcc[1] * 10  ) +
           p_plmn_id_info->mcc[2];


  /* MNC: assume there will be 3 digits, so put the 1st 2 digits in the
     100s and 10s place. */
  *p_mnc = ( p_plmn_id_info->mnc[0] * 100 ) +
           ( p_plmn_id_info->mnc[1] * 10 );

  /* add in the 3rd digit if there is one */
  if ( p_plmn_id_info->num_mnc_digits == 3 )
  {
    *p_mnc += p_plmn_id_info->mnc[2];
  }
  else
  {
    /* divide by 10 if there are only 2 digits in the mnc */
    *p_mnc /= 10;
  }

  return TRUE;

}

#ifdef FEATURE_CGPS_LTE_CELLDB

/******************************************************************************
 * tm_umts_supl_lte_plmn_id_organizer
 *
 * Function description:
 *   Converts MCC and MNC stored in the format of lte_rrc_plmn_s to
 *   uint32.
 *
 * Parameters:
 *  p_plmn_id_info: pointer to the structure containing plmn cell
 *                 information;
 *  p_mcc: pointer for the this function to write converted MCC value to.
 *  p_mnc: pointer for the this function to write converted MNC value to.
 *
 * Return:
 *   TRUE/FALSE.
 *
 ******************************************************************************
*/

boolean tm_umts_supl_lte_plmn_id_organizer
( lte_rrc_plmn_s *p_plmn_id_info,
  uint32 *p_mcc,
  uint32 *p_mnc
)
{

  if ( (p_mcc == NULL) || (p_mnc==NULL) || (p_plmn_id_info == NULL) )
  {
    return FALSE;
  }

  *p_mcc = ( ( p_plmn_id_info->mcc[0] & 0x000F) * 100 ) +
           ( ( p_plmn_id_info->mcc[1] & 0x000F) * 10  ) +
           ( p_plmn_id_info->mcc[2] & 0x000F );


  /* MNC: assume there will be 3 digits, so put the 1st 2 digits in the
     100s and 10s place. */
  *p_mnc = ( ( p_plmn_id_info->mnc[0] & 0x000F) * 100 ) +
           ( ( p_plmn_id_info->mnc[1] & 0x000F) * 10 );

  /* add in the 3rd digit if there is one */
  if ( p_plmn_id_info->num_mnc_digits == 3 )
  {
    *p_mnc += p_plmn_id_info->mnc[2] & 0x000F;
  }
  else
  {
    /* divide by 10 if there are only 2 digits in the mnc */
    *p_mnc /= 10;
  }

  return TRUE;

}

#endif /* if SUPL2 + LTE_CELL_DB*/


/******************************************************************************
 * tm_umts_up_supl_req_cell_info_from_rrc
 *
 * Function description:
 *   Actively request cell info. from RRC
 *
 * Parameters:   None
 *
 * Return value: TRUE: request to RRC sent successfully;
 *               FALSE: request to RRC failed.
 *
 * Note: reason to featurize this function is the RRC counterpart is
 *       featurized in order to serve different targets some of which
 *       do not have this feature implemented on the GPS side.
 *
 ******************************************************************************
*/

boolean tm_umts_up_supl_req_cell_info_from_rrc(supl_cf_state_enum_type cf_state)
{
  supl_set_cf_state(cf_state, 0);

#ifdef FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH
  /* Request the new cell id information from CM */
  MSG_MED("SUPL requested DCH CID (cf_state=%d)",(uint32)cf_state,0,0);
  return tm_cm_request_wcdma_dch_cell_info();
#else
  MSG_ERROR("Can't request WCDMA DCH CID.", 0, 0, 0);
  return FALSE;
#endif /* FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH */
}

/*
******************************************************************************
* supl_SuplInit_emergency_bit_process
*
* Function description:
*
* Process incoming SUPL INIT message to see if emergency bit is set
*
* Parameters:  supl_init message
*
* Return value: none
*
******************************************************************************
*/

void supl_SuplInit_emergency_bit_process(const supl_SUPLINIT  *p_suplInit)
{

  if (p_suplInit->m.notificationPresent == 1  )
  {
    if ( p_suplInit->notification.m.ver2_Notification_extensionPresent == 1 )
    {
      /* if emergency call, then ensure the SUPL INIT cotnains the emergency positioning indication */
      if ( p_suplInit->notification.ver2_Notification_extension.m.emergencyCallLocationPresent == 1 )
      {
        z_supl_emergency_status.b_valid = TRUE;
        z_supl_emergency_status.b_emergency_ni = TRUE;
        if ( (p_suplInit->m.ver2_SUPL_INIT_extensionPresent == 1 ) &&
             (p_suplInit->ver2_SUPL_INIT_extension.m.e_SLPAddressPresent == 1))
        {
          memscpy(&z_supl_emergency_status.z_eslp_address, sizeof(z_supl_emergency_status.z_eslp_address),
            &p_suplInit->ver2_SUPL_INIT_extension.e_SLPAddress, sizeof(p_suplInit->ver2_SUPL_INIT_extension.e_SLPAddress));
        }
        MSG_MED("SUPL: Handle NI request with emergency bit set, eslp present %d",p_suplInit->ver2_SUPL_INIT_extension.m.e_SLPAddressPresent,0,0);
      }
    }
  }
}

/*
******************************************************************************
* supl_SuplInit_proc
*
* Function description:
*
* Process incoming SUPL INIT message
*
* Parameters: p_pdu_decoded: pointer to a structure holding decoded Supl PDU.
*             p_raw_pdu : pointer to raw pdu buffer.
*             w_raw_pdu_len: length of the raw pdu in bytes.
              p_hash: pointer to the first byte of hash - NULL if no HASH
*
* Return value: TRUE/FALSE
*
******************************************************************************
*/

static boolean supl_SuplInit_proc( const supl_ULP_PDU* p_pdu_decoded,
                                   uint8* p_raw_pdu,
                                   uint16 w_raw_pdu_len,
                                   uint8* p_hash )

{
  const supl_SUPLINIT  *p_suplInit = p_pdu_decoded->message.u.msSUPLINIT;

  supl_slp_session_id  z_p_provided_slp_session_id;
  supl_slp_session_id* p_provided_slp_session_id = &z_p_provided_slp_session_id;
  emergency_handling_e_type emergency_effect;

  tm_cm_phone_state_info_s_type phone_state_info;
  uint32  q_supl_status_code = 0xFFFFFFFF;

  uint32  q_supl_version;

  tm_cm_iface_get_phone_state(&phone_state_info);

  /*SUPL is not Enabled, ignore the message*/
  if(tm_umts_up_is_supl_enabled() == FALSE)
  {
  MSG_HIGH("SUPL_INIT ignored: SUPL not Enabled", 0, 0, 0);
  return FALSE;
  }

  if ( ( TM_CORE_CONFIG_DD(positioning_modes_supported) &
         ( PDSM_POS_MODE_UP_MS_ASSISTED | PDSM_POS_MODE_UP_MS_BASED |
           PDSM_POS_MODE_4G_UP_MS_ASSISTED |PDSM_POS_MODE_4G_UP_MS_BASED )
       ) == 0
     )
  {
    /* SUPL not provisioned by NV, ignore the message */
    MSG_HIGH("SUPL_INIT ignored: SUPL not provisioned, Supported modes %d",
             TM_CORE_CONFIG_DD(positioning_modes_supported), 0, 0);
    return FALSE;
  }

  MSG_MED("Recvd SUPL_INIT", 0, 0, 0);

  supl_util_server_session_id_organizer ( &p_pdu_decoded->sessionID, p_provided_slp_session_id);

  /* incoming message SUPL version number */
  q_supl_version = (uint32)p_pdu_decoded->version.servind <<16;
  q_supl_version |= (uint32)p_pdu_decoded->version.min << 8;
  q_supl_version |= (uint32)p_pdu_decoded->version.maj;


  if ( p_hash == NULL )
  {
    /* should not happen: HASH generation failed */
    MSG_MED("missing hash", 0, 0, 0);
    (void) tm_umts_up_supl_supl_end_prepare_or_tx( p_provided_slp_session_id,
                                            NULL,
                                            supl_StatusCode_unspecified,
                                            NULL,
                                            q_supl_version
                                           );
    return FALSE;
  }
  emergency_effect= tm_umts_common_utils_emergency_effect();
  MSG_MED("SUPL: Emergency affect %d", emergency_effect, 0, 0);


  /************* with valid HASH *******************************************/
  if ( p_suplInit->posMethod >= supl_posmethodOutOfRangeHexA )
  {
    MSG_MED("SUPL: end NI request due to posMethod out of range", 0, 0, 0);
    q_supl_status_code = supl_unexpectedDataValue;
  }
  else if (p_suplInit->sLPMode != supl_proxy )
  {
    /* Check SLP Mode. If not proxy then respond with an error */
    MSG_MED("SUPL: end NI request due to non-proxy not supported", 0, 0, 0);
    q_supl_status_code = supl_nonProxyModeNotSupported;
  }
  else if ( p_suplInit->notification.notificationType > supl_NotificationType_privacyOverride  )
  {
    /* this should not happen, as ASN.1 decoder should fail */
    MSG_MED("SUPL: end NI request due to notifType out of range", 0, 0, 0);
    q_supl_status_code = supl_unexpectedDataValue;
  }
  else if ( tm_core_is_supl_ni_proc() == TRUE )
  {
    MSG_HIGH("SUPL: NV item is set to process SUPL INIT always: Processing SUPL INIT", 0, 0, 0);
    supl_SuplInit_emergency_bit_process(p_suplInit);
  }
 else if ( emergency_effect == C_EMERGENCY_HANDLING_NULL )
  {
    if ( ( ( z_umts_nv_item_ram_copy.gps_lock == PDSM_GPS_LOCK_ALL ) ||
           ( z_umts_nv_item_ram_copy.gps_lock == PDSM_GPS_LOCK_MT )
         )
         &&
         (( p_suplInit->notification.notificationType !=
           supl_NotificationType_privacyOverride ) &&
          (( p_suplInit->notification.m.ver2_Notification_extensionPresent != 1 ) &&
           ( p_suplInit->notification.ver2_Notification_extension.m.emergencyCallLocationPresent != 1 )))
       )
    {
      /* NvItems gps1_lock_control has locked up GPS and the request is not privacyOverride */
      MSG_MED("SUPL: NV_Item locks up non-privacy-override NI %d and non-emergency NI %d %d",
               p_suplInit->notification.notificationType,
               p_suplInit->notification.m.ver2_Notification_extensionPresent,
               p_suplInit->notification.ver2_Notification_extension.m.emergencyCallLocationPresent);
      q_supl_status_code = supl_positioningNotPermitted;
    }
    else
    {
      MSG_MED("SUPL: non emergency session", 0,0,0);
      supl_SuplInit_emergency_bit_process(p_suplInit);
    }
  }
  else if ( emergency_effect != C_EMERGENCY_HANDLING_NULL )
  {
    /* if in emergency mode and can accept the SUPL trigger, check if we are in LTE or WCDMA with SUPL
       set as emergency, and the SUPL 2.0 has emergency bit set */
    if (emergency_effect != C_EMERGENCY_HANDLING_REJ_ALL  )
    {
      if ((phone_state_info.srv_system == CGPS_SRV_SYS_LTE) ||
          ((phone_state_info.srv_system == CGPS_SRV_SYS_WCDMA) ))
      {
        supl_SuplInit_emergency_bit_process(p_suplInit);
      }
      else
      {
        MSG_MED("SUPL: Ignore NI request due to emergency effect %d for ongoing emergency call ", emergency_effect,0,0);
        return FALSE;
      }
    }
    else
    {
      MSG_MED("SUPL: Ignore NI request due to emergency effect %d  for ongoing emergency call on sys=%d",
              emergency_effect,
              phone_state_info.srv_system, 0);
      return FALSE;
    }
  }
  else if(tm_prtl_check_mt_up_allowed(TM_PRTL_TYPE_UMTS_UP) == FALSE)
  {
    MSG_HIGH("SUPL MT session not allowed", 0, 0, 0);
    return FALSE;
  }

  if ( p_suplInit->posMethod == supl_ver2_sessioninfoquery )
  {
    supl_session_query_proc(p_hash, p_provided_slp_session_id, q_supl_version);
    return TRUE;  /* leave this function and do not proceed to notification */
  }

  if ( p_suplInit->posMethod == supl_ver2_historicalDataRetrieval)
  {

    (void) tm_umts_up_supl_supl_end_prepare_or_tx(p_provided_slp_session_id,
                                                  NULL,
                                                  supl_StatusCode_ver2_serviceNotSupported,
                                                  p_hash,
                                                  q_supl_version);
    MSG_HIGH("HistoricalDataRetrieval not supported, sending SUPL_END", 0, 0, 0);
    return TRUE;
  }



  /*** if no error detected so far, send SUPL_INIT along with hash to pdapi;
       otherwise, send SUPL_END to SLP *************************************/
  if ( q_supl_status_code == 0xFFFFFFFF )
  {

    /* TBD: this is better done after pdapi notification comes back, for
       service interaction purpose, but currently pdpai doesn't have this
       IE included  !! */
    /* if SUPL version for the session is not yet set, do it now */
    if ( z_supl_session_status.q_supl_version == 0 )
    {
      z_supl_session_status.q_supl_version = q_supl_version;
    }


    if ( p_pdu_decoded->message.u.msSUPLINIT->ver2_SUPL_INIT_extension.m.triggerTypePresent )
    {

      if ( p_pdu_decoded->message.u.msSUPLINIT->ver2_SUPL_INIT_extension.triggerType ==
           supl_TriggerType_periodic )
      {
        (void) tm_umts_up_supl_supl_end_prepare_or_tx(p_provided_slp_session_id,
                                                      NULL,
                                                      supl_StatusCode_ver2_serviceNotSupported,
                                                      p_hash,
                                                      q_supl_version);
        MSG_HIGH("NI periodic not supported, sending SUPL_END", 0, 0, 0);
        return TRUE;
      }

      /* TBD: in the future this may become part of the message to above pdapi */
      z_supl_init_trigger_type.trigger_type_present = TRUE;
      z_supl_init_trigger_type.q_trigger_type =
        p_pdu_decoded->message.u.msSUPLINIT->ver2_SUPL_INIT_extension.triggerType;

    }
    else
    {
      z_supl_init_trigger_type.trigger_type_present = FALSE;
    }

    if ( ( p_pdu_decoded->message.u.msSUPLINIT->ver2_SUPL_INIT_extension.m.historicReportingPresent ) ||
         ( ( p_pdu_decoded->message.u.msSUPLINIT->ver2_SUPL_INIT_extension.m.protectionLevelPresent &&
             p_pdu_decoded->message.u.msSUPLINIT->ver2_SUPL_INIT_extension.protectionLevel.protlevel !=
             supl_ProtLevel_nullProtection )
         )
       )
    {
        (void) tm_umts_up_supl_supl_end_prepare_or_tx(p_provided_slp_session_id,
                                                      NULL,
                                                      supl_StatusCode_ver2_incompatibleProtectionLevel,
                                                      p_hash,
                                                      q_supl_version);

        MSG_HIGH("Protection level imcompatible, sending SUPL_END", 0, 0, 0);
        return TRUE;
    }


    /* regardless what notification/verification type SUPL_INIT has, send the request
       via pdapi to the OEM, and let OEM decide */
    supl_send_notification_verifcation(p_pdu_decoded, p_hash);
     /* Set the XO Core to be ON in expectation of GPS activity */
    (void) mgp_ForceXoCoreOn();
    return TRUE;
  }
  else
  {
    (void) tm_umts_up_supl_supl_end_prepare_or_tx(p_provided_slp_session_id,
                                           NULL,
                                           q_supl_status_code,
                                           p_hash,
                                           q_supl_version);
    return FALSE;
  }

}


/*
******************************************************************************
* supl_SuplStart_tx
*
* Function description:
*
* Encode and transmit SUPL START
*
* Parameters: NONE.
*
* Return value: TRUE or FALSE
*
******************************************************************************
*/
boolean supl_SuplStart_tx (void)
{
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;
  boolean               u_ret                 = FALSE;

  tm_pdu_buf_type   pdu_buff; /* including length and pointer to data */
  supl_ULP_PDU      pdu;

  supl_SUPLSTART*   p_Suplstart;
  supl_SUPLSTART    z_Suplstart;

  supl_cell_info_struct_type  z_cell_info_allocated;

  uint8*           p_length;
  uint8            u_length_msb = 0;
  uint8            u_length_lsb = 0;

  tm_cm_phone_state_info_s_type phone_state_info;

  int encode_status;

  supl_Ver2_SUPL_START_extension z_supl_Ver2_supl_start_extension;

  uint8 u_num_mlid = 0;
  supl_LocationIdData *p_LocationIdData = NULL;
  supl_LocationId     *p_LocationId = NULL;
  supl_cell_info_struct_type  *p_cell_info_allocated_mlid = NULL;


  struct supl_GANSSPositionMethod   z_ganssPositionMethod;
  struct supl_GANSSPositionMethods  z_ganssPositionMethodsList;
  struct supl_GANSSPositionMethods  *p_ganssPositionMethodsList =
                                      &z_ganssPositionMethodsList;


  /* When ready to send SUPL_START, any timer/state waiting for WIFI MLID should be canceled */
  tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_WIFI_WAIT_TIMER_NO_TCP );
  tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_WIFI_WAIT_SUPLSTART );
  tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_WIFI_WAIT_TIMER_SUPLSTART );

  /* Query TM-CM module to get current acquired system */
  tm_cm_iface_get_phone_state(&phone_state_info);

  memset(&z_supl_Ver2_supl_start_extension, 0, sizeof(supl_Ver2_SUPL_START_extension));
  memset(&pdu, 0x00, sizeof(supl_ULP_PDU));
  memset(&z_cell_info_allocated, 0, sizeof(supl_cell_info_struct_type));
  memset(&z_Suplstart, 0x00, sizeof(supl_SUPLSTART));
  memset(&pdu_buff, 0, sizeof(tm_pdu_buf_type));

  memset(&z_supl_Ver2_supl_start_extension, 0, sizeof(supl_Ver2_SUPL_START_extension));
  memset(&z_ganssPositionMethod, 0, sizeof(z_ganssPositionMethod));
  memset(&z_ganssPositionMethodsList, 0, sizeof(z_ganssPositionMethodsList));

  p_Suplstart = &z_Suplstart;
  pdu.message.t = C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLSTART;
  pdu.message.u.msSUPLSTART = p_Suplstart;


  /* overhead build function has to be after we know what message is to be constructed */

  if ( !supl_header_build (&pdu, NULL, NULL, NULL, p_supl_session_status->q_supl_version) )
  {
    MSG_ERROR("SUPL: SuplStart attempt aborted: header", 0, 0, 0);
    return FALSE;
  }

  /* Positioning technology */
  p_Suplstart->sETCapabilities.posTechnology.aFLT = FALSE;
  p_Suplstart->sETCapabilities.posTechnology.eOTD = FALSE;
  p_Suplstart->sETCapabilities.posTechnology.oTDOA = FALSE;
  p_Suplstart->sETCapabilities.posTechnology.eCID = FALSE; /* maybe modified below */


  if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED )
  {
      p_Suplstart->sETCapabilities.posTechnology.agpsSETassisted = TRUE;
      if ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP )
      {
        p_Suplstart->sETCapabilities.posTechnology.oTDOA = TRUE;
      }
  }
  else if(p_supl_session_status->u_agps_mode == C_SUPL_AFLT_MODE)
  {
    if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801)
    {
      p_Suplstart->sETCapabilities.posTechnology.aFLT = TRUE;
    }
    if ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP )
    {
      p_Suplstart->sETCapabilities.posTechnology.oTDOA = TRUE;
    }
  }
  else if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_BASED )
  {
    p_Suplstart->sETCapabilities.posTechnology.agpsSETBased = TRUE;

    /*IS801 does not support Autonomous GPS mode*/
    if(p_supl_session_status->u_position_protocol != TM_POS_PRTL_IS801)
    {
      #ifndef FEATURE_CGPS_SUPL_VARIANT_SET_CAP_NO_AUTO /* NOT by default defined */
      p_Suplstart->sETCapabilities.posTechnology.autonomousGPS = TRUE;
      #endif
    }

  }
  else if (p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_CELL_ID)
  {
     p_Suplstart->sETCapabilities.posTechnology.agpsSETassisted = FALSE;
     p_Suplstart->sETCapabilities.posTechnology.autonomousGPS = FALSE;
     p_Suplstart->sETCapabilities.posTechnology.agpsSETBased = FALSE;
  }
  else if ( p_supl_session_status->u_agps_mode == C_SUPL_OTDOA_MODE )
  {
    if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP)
    {
      p_Suplstart->sETCapabilities.posTechnology.oTDOA = TRUE;
      // eCID is set later
    }
  }
  else
  {
    /* no other agps mode should generate SUPL_POS_INIT */
    MSG_ERROR("SUPL: SuplStart attempt aborted", 0, 0, 0);
    u_ret = FALSE;
  }

  p_Suplstart->sETCapabilities.prefMethod = supl_PrefMethod_noPreference;

  #ifdef FEATURE_CGPS_SUPL_VARIANT_SET_CAP_PREF_SET_BASED  /* NOT by defaut defined */
  #error code not present
#endif

  /* positioning protocol */
  // Initialize
  p_Suplstart->sETCapabilities.posProtocol.rrc = FALSE;
  p_Suplstart->sETCapabilities.posProtocol.tia801 = FALSE;
  p_Suplstart->sETCapabilities.posProtocol.rrlp = FALSE;

  if(TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable) != 0)
  {
    p_Suplstart->sETCapabilities.posProtocol.tia801 = TRUE;
  }

  if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP)
  {
    p_Suplstart->sETCapabilities.posProtocol.m.ver2_PosProtocol_extensionPresent = 1;
    p_Suplstart->sETCapabilities.posProtocol.ver2_PosProtocol_extension.lpp = TRUE;
  }
  else
  {
    p_Suplstart->sETCapabilities.posProtocol.rrlp = TRUE;
  }

  /**** SUPL-2 only: Service Capabilities ***/
  if ( (p_supl_session_status->q_supl_version & 0x000000FF) == 2 )
  {
    /***********  Service Capabilities optional SUPL2 extension ********/

    #if 0 /* For future dev. */
    p_Suplstart->sETCapabilities.ver2_SETCapabilities_extension.m.serviceCapabilitiesPresent = 1;
    p_Suplstart->sETCapabilities.ver2_SETCapabilities_extension.m.supportedBearersPresent = 1;

    /* neither reportingCapabilities or eventTriggerCapabilities is present */

    /* neither periodic trigger or area event trigger is supported */

    #endif
    p_Suplstart->sETCapabilities.posProtocol.m.ver2_PosProtocol_extensionPresent = 1;

    /***********  PosProtocolVersionRRLP SUPL2 extension in PosProtocol ****/
    if (p_Suplstart->sETCapabilities.posProtocol.rrlp == TRUE )
    {


      p_Suplstart->sETCapabilities.posProtocol.ver2_PosProtocol_extension.m.posProtocolVersionRRLPPresent = 1;

      /* default to the backward compatible version first, may be updated below */
      p_Suplstart->sETCapabilities.posProtocol.ver2_PosProtocol_extension.posProtocolVersionRRLP.majorVersionField =
        SUPL_RRLP_BACK_VERSION_MAJOR;
      p_Suplstart->sETCapabilities.posProtocol.ver2_PosProtocol_extension.posProtocolVersionRRLP.technicalVersionField =
        SUPL_RRLP_BACK_VERSION_TECH;
      p_Suplstart->sETCapabilities.posProtocol.ver2_PosProtocol_extension.posProtocolVersionRRLP.editorialVersionField =
        SUPL_RRLP_BACK_VERSION_EDIT;
    }

    if ( p_Suplstart->sETCapabilities.posProtocol.ver2_PosProtocol_extension.lpp == TRUE )
    {
      p_Suplstart->sETCapabilities.posProtocol.ver2_PosProtocol_extension.m.posProtocolVersionLPPPresent = 1;

      p_Suplstart->sETCapabilities.posProtocol.ver2_PosProtocol_extension.posProtocolVersionLPP.majorVersionField = LPP_CURRENT_VERSION_MAJOR;
      p_Suplstart->sETCapabilities.posProtocol.ver2_PosProtocol_extension.posProtocolVersionLPP.technicalVersionField = LPP_CURRENT_VERSION_TECH;
      p_Suplstart->sETCapabilities.posProtocol.ver2_PosProtocol_extension.posProtocolVersionLPP.editorialVersionField = LPP_CURRENT_VERSION_EDIT;
    }

    /* Application ID */
    if ( p_supl_session_status->z_supl_app_id.u_validity & PDAPI_SUPL_APPLICATION_ID_VALID )
    {
      p_Suplstart->ver2_SUPL_START_extension = &z_supl_Ver2_supl_start_extension;
      p_Suplstart->m.ver2_SUPL_START_extensionPresent = TRUE;

      /* making sure the app ID elements would never exceed their max. length */
      p_supl_session_status->z_supl_app_id.u_app_provider[PDAPI_SUPL_APPID_APP_PROVIDER_ARR_SIZE - 1] = 0;
      p_supl_session_status->z_supl_app_id.u_app_name[PDAPI_SUPL_APPID_APP_NAME_ARR_SIZE - 1] = 0;
      p_supl_session_status->z_supl_app_id.u_app_version[PDAPI_SUPL_APPID_APP_VERSION_ARR_SIZE - 1] = 0;

      z_supl_Ver2_supl_start_extension.m.applicationIDPresent = TRUE;

      z_supl_Ver2_supl_start_extension.applicationID.appProvider =
        (const char *)&p_supl_session_status->z_supl_app_id.u_app_provider[0];

      z_supl_Ver2_supl_start_extension.applicationID.appName =
        (const char*)&p_supl_session_status->z_supl_app_id.u_app_name[0];

      if ( p_supl_session_status->z_supl_app_id.u_validity & PDAPI_SUPL_APPLICATION_ID_APP_VERSION_VALID )
      {
        z_supl_Ver2_supl_start_extension.applicationID.m.appVersionPresent = TRUE;
        z_supl_Ver2_supl_start_extension.applicationID.appVersion =
          (const char *)&p_supl_session_status->z_supl_app_id.u_app_version[0];
      }
    }

    /****** ver2_PosTechnology SUPL2 extension in posTechnology **********/

    // Glonass enabled and Protocol is not IS801
    if ( tm_umts_common_glonass_enabled() && (p_supl_session_status->u_position_protocol != TM_POS_PRTL_IS801) )
    {
      supl_fill_ver2_ganss_set_capability( &(p_Suplstart->sETCapabilities), &z_ganssPositionMethod,
                                           p_ganssPositionMethodsList, C_SUPL_SESSION_UI );
    }
  }
  /* populate the relevant part of SUPL_START with the cached cell ID info. */
  if ( !supl_cell_information_construct(&p_Suplstart->locationId.cellInfo,
                                        &z_cell_info_allocated) )
  {
    MSG_HIGH("No Cell ID info.", 0, 0, 0);
    return FALSE;
  }

  #ifdef FEATURE_CGPS_LTE_CELLDB
  else if ( ( phone_state_info.srv_system == CGPS_SRV_SYS_LTE ) &&
            z_cell_info_cached.u_CachedCellIDValid  &&
            ( z_cell_info_cached.e_ActiveRAT == CELLDB_ACTIVERAT_LTE ) &&
            ( ( p_supl_session_status->q_supl_version & 0x000000FF ) == 2 )
          )
  {
    /* if SUPL2, and supl_cell_information_construct() returns TRUE */

    if ( ( p_Suplstart->locationId.cellInfo.u.ver2_CellInfo_extension->u.lteCell->m.rsrpResultPresent ||
           p_Suplstart->locationId.cellInfo.u.ver2_CellInfo_extension->u.lteCell->m.rsrqResultPresent ||
           p_Suplstart->locationId.cellInfo.u.ver2_CellInfo_extension->u.lteCell->m.tAPresent
         ) ||
         ( z_lte_mrl.num_cells > 1 )
       )
    {
      p_Suplstart->sETCapabilities.posTechnology.eCID = TRUE;
      p_supl_session_status->u_supl_posMethod_ecid_valid = TRUE;
    }


  }
  #endif   /* FEATURE LTE_CELL_DB */


  #ifdef FEATURE_USER_PLANE_MRL
  if ( ( phone_state_info.srv_system == CGPS_SRV_SYS_WCDMA ) &&
       z_cell_info_cached.u_CachedCellIDValid &&
       ( z_cell_info_cached.e_ActiveRAT == CELLDB_ACTIVERAT_WCDMA )
     )
  {
    p_Suplstart->sETCapabilities.posTechnology.eCID = TRUE;
    p_supl_session_status->u_supl_posMethod_ecid_valid = TRUE;
  }
  #endif

  if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_NMR_SUPPORTED) != 0 )
  {
    if (phone_state_info.srv_system == CGPS_SRV_SYS_GSM)
    {
      p_Suplstart->sETCapabilities.posTechnology.eCID = TRUE;
      p_supl_session_status->u_supl_posMethod_ecid_valid = TRUE;
    }
  }

  #ifdef FEATURE_CGPS_SUPL_VARIANT_NO_ECID /* NOT defined by default */
    #error code not present
#endif


  /* for SET-Initiated, always declare CURRENT */
  p_Suplstart->locationId.status = supl_Status_current;


  /* MLID for SUPL_START */
  if ( ( p_supl_session_status->q_supl_version & 0x000000FF ) >= 2 )
  {
    p_Suplstart->ver2_SUPL_START_extension = &z_supl_Ver2_supl_start_extension;

    if ( u_supl_global_num_mlid > 1 ) /* note being 1 means only the current serving cell is in the structure */
    {
      p_LocationIdData = (supl_LocationIdData*)os_MemAlloc((sizeof(supl_LocationIdData) * (u_supl_global_num_mlid - 1)), OS_MEM_SCOPE_TASK);
      p_LocationId = (supl_LocationId*)os_MemAlloc((sizeof(supl_LocationId) * (u_supl_global_num_mlid - 1)), OS_MEM_SCOPE_TASK);
      p_cell_info_allocated_mlid = (supl_cell_info_struct_type*)os_MemAlloc((sizeof(supl_cell_info_struct_type) * (u_supl_global_num_mlid - 1)), OS_MEM_SCOPE_TASK);

      if ( ( p_LocationIdData == NULL ) || (p_LocationId == NULL) || (p_cell_info_allocated_mlid == NULL ) )
      {
        u_num_mlid = 0;
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "mlid mem. allocation failed, set num_mlid to %u ", u_num_mlid);
      }
      else
      {
        memset(p_LocationIdData, 0, sizeof(supl_LocationIdData) * ( u_supl_global_num_mlid - 1) );
        memset(p_LocationId, 0, sizeof(supl_LocationId) * ( u_supl_global_num_mlid - 1) );
        memset(p_cell_info_allocated_mlid, 0, sizeof(supl_cell_info_struct_type) * ( u_supl_global_num_mlid - 1) );

        p_LocationIdData->locationId = p_LocationId;
        u_num_mlid = supl_chk_and_build_all_mlid(p_LocationIdData, p_LocationId,
                                                 p_cell_info_allocated_mlid, ( u_supl_global_num_mlid - 1) );
      }

    }
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "num_MLID %u ", u_num_mlid);

    if ( u_num_mlid > 0 )
    {
      if (p_LocationIdData != NULL)
      {
        p_Suplstart->m.ver2_SUPL_START_extensionPresent = TRUE;
        p_Suplstart->ver2_SUPL_START_extension = &z_supl_Ver2_supl_start_extension;
        z_supl_Ver2_supl_start_extension.m.multipleLocationIdsPresent = TRUE;
        z_supl_Ver2_supl_start_extension.multipleLocationIds.n = u_num_mlid;
        z_supl_Ver2_supl_start_extension.multipleLocationIds.elem = p_LocationIdData;
      }
      else
      {
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "p_LocationIdData not allocated: %lx ", p_LocationIdData);
      }
    }
  } /* running SUPL ver. >= 2 */


  /* QOP */
  #ifndef FEATURE_CGPS_SUPL_VARIANT_SUPLSTART_NO_QOP /* NOT by default defined */
  p_Suplstart->m.qoPPresent = 1;
  if (p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_CELL_ID)
  {
    p_Suplstart->qoP.horacc = 127; /* max. horacc for cell-ID request */
  }
  else
  {
    p_Suplstart->qoP.horacc = tm_umts_common_utils_meter_to_GADK( TRUE, p_supl_session_status->z_session_qos.q_hor_acc_meters );
  }

  /* delay: so that SLP can set a reasonable response time in RRLP */
  p_Suplstart->qoP.m.delayPresent = 1;
  p_Suplstart->qoP.delay =
    tm_umts_common_next_int_log2_max_128( p_supl_session_status->z_session_qos.u_gps_session_timeout_value_secs );


  /* for OTDOA only or AFLT only, the QoS is 0 from SI session. use the default 16 sec*/
  if (p_supl_session_status->z_session_qos.u_gps_session_timeout_value_secs == 0 )
  {
    p_Suplstart->qoP.delay = 4;

  }
  /* MaxLocAge: optional - maximum tolerable age of position estimates used for cached position
     fixes; units in seconds from 0 to 65535. --- OMA-TS-UPL-V1_0-20050627-D */
  p_Suplstart->qoP.m.maxLocAgePresent = 1;
  p_Suplstart->qoP.maxLocAge = 0; /* doesn't hurt to always include this and set it to 0 */
  #endif


  encode_status = tm_osys_asn1_encode_pdu((void *)&pdu, &pdu_buff, PROTO_SUPL);

  if ( (encode_status != 0) || (pdu_buff.value == NULL ) || (pdu_buff.length == 0) )
  {
    /* ASN.1 encoding failed */
    if (pdu_buff.value != NULL )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SUPL ASN1 enc. err. Freeing: %d,%lx,%u",
            encode_status, pdu_buff.value, pdu_buff.length);

      tm_asn1_free_osys_buf( pdu_buff.value, PROTO_SUPL );
      pdu_buff.value = NULL;

    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SUPL ASN1 enc. err. No alloc.: %d,%lx,%u",
            encode_status, pdu_buff.value, pdu_buff.length);
    }

    if ( p_cell_info_allocated_mlid != NULL )
    {
      os_MemFree((void**)&p_cell_info_allocated_mlid);
      p_cell_info_allocated_mlid = NULL;
    }
    if ( p_LocationId != NULL )
    {
      os_MemFree((void**)&p_LocationId);
      p_LocationId = NULL;
    }
    if ( p_LocationIdData != NULL)
    {
      p_LocationIdData->locationId = NULL;
      os_MemFree((void**)&p_LocationIdData);
      p_LocationIdData = NULL;
    }

    return FALSE;
  }

  else
  {
    pd_comms_return_type ret_val;

    p_length = (uint8 *)pdu_buff.value;
    u_length_msb = (pdu_buff.length >> 8) & 0x00FF;
    u_length_lsb = pdu_buff.length & 0x00FF;

    *p_length++ = u_length_msb;
    *p_length = u_length_lsb;

    ret_val = tm_umts_up_supl_comm_write( (uint8 *)pdu_buff.value, pdu_buff.length);

    if ( ( ret_val != PDCOMM_RESULT_WAIT ) && ( ret_val != PDCOMM_RESULT_OK ) )
    {
      /* message sending failed */
      MSG_ERROR("SUPL: SuplStart attempt aborted", 0, 0, 0);
    }
    else
    {
      supl_set_cf_state(C_SUPL_CF_STATE_UT1, 0);
      u_ret = TRUE;
    }

    /*Free the encoded PDU*/
    tm_asn1_free_osys_buf( pdu_buff.value, PROTO_SUPL );
    pdu_buff.value = NULL;

  }

  if ( p_cell_info_allocated_mlid != NULL )
  {
    os_MemFree((void**)&p_cell_info_allocated_mlid);
    p_cell_info_allocated_mlid = NULL;
  }
  if ( p_LocationId != NULL )
  {
    os_MemFree((void**)&p_LocationId);
    p_LocationId = NULL;
  }
  if ( p_LocationIdData != NULL)
  {
    p_LocationIdData->locationId = NULL;
    os_MemFree((void**)&p_LocationIdData);
    p_LocationIdData = NULL;
  }

  if ( u_ret )
  {
    p_supl_session_status->u_network_session_on = TRUE;
    p_supl_session_status->u_send_supl_end_at_fail = TRUE;
    MSG_MED("SuplStart sent ", 0, 0, 0);

  }

  return u_ret;
}


/*
******************************************************************************
* supl_SuplResponse_proc
*
* Function description:
*
* Process incoming SUPL RESPONSE message
*
* Parameters: p_pdu_decoded: pointer to a structure holding decoded Supl PDU.
*
* Return value: TRUE/FALSE
*
******************************************************************************
*/
static boolean supl_SuplResponse_proc( const supl_ULP_PDU* p_pdu_decoded )
{

  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;

  boolean u_ret = FALSE;

  const supl_SUPLRESPONSE *p_suplResponse;

  supl_store_slp_sessionId(p_pdu_decoded);

  p_suplResponse = p_pdu_decoded->message.u.msSUPLRESPONSE;

  MSG_MED("process_supl_response()", 0, 0, 0);


  if ( ! ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT1 ) )
  {
    (void) tm_umts_up_supl_supl_end_tx( &p_supl_session_status->z_session_id.z_Slp_Sess_Id,
                                 NULL,
                                 supl_StatusCode_unexpectedMessage,
                                 NULL,
                                 p_supl_session_status->q_supl_version);
    tm_umts_up_supl_callflow_disrupt_handler();
    return FALSE;
  }

  tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_UT1);


  /* if SUPL_RESPONSE and SUPL_START do not match in terms of posMethod, SUPL_END it.
     note if SLP asks for autonomous, the application must have requested SET-Based or
     it should be considered a mismatch */
  if (  ( ( p_suplResponse->posMethod == supl_agpsSETbased ) &&
          ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED )
        ) ||
        ( ( p_suplResponse->posMethod == supl_agpsSETassisted ) &&
          ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_BASED )
        ) ||
        ( ( p_suplResponse->posMethod == supl_autonomousGPS ) &&
         ( p_supl_session_status->u_agps_mode != C_SUPL_AGPS_MODE_UE_BASED )
        ) ||
        /* Proceed with SuplPosInit only if WCDMA Cell DB information is available*/
        ( ( p_suplResponse->posMethod == supl_eCID ) &&
          ( p_supl_session_status->u_supl_posMethod_ecid_valid != TRUE )
        ) ||
        /*AFLT only supported for IS801*/
        ( (p_suplResponse->posMethod == supl_aFLT) &&
            ( (p_supl_session_status->u_agps_mode != C_SUPL_AFLT_MODE) ||
              (p_supl_session_status->u_position_protocol!= TM_POS_PRTL_IS801)
            )
        ) ||
       /* OTDOA only when LPP is selected */
        ( (p_suplResponse->posMethod == supl_oTDOA) &&
         ((p_supl_session_status->u_agps_mode != C_SUPL_OTDOA_MODE) ||
          (p_supl_session_status->u_position_protocol != TM_POS_PRTL_LPP)
          )
         ) ||
        ( p_suplResponse->posMethod == supl_eOTD )
     )
  {
    (void) tm_umts_up_supl_supl_end_tx(&p_supl_session_status->z_session_id.z_Slp_Sess_Id,
                                NULL,
                                supl_posMethodMismatch,
                                NULL,
                                p_supl_session_status->q_supl_version);
    tm_umts_up_supl_callflow_disrupt_handler();
    return FALSE; /* don't proceed */
  }

  /* Determine if the H-SLP has GLONASS capability */
  if ( (p_suplResponse->posMethod != supl_PosMethod_ver2_agnssSETassisted) &&
       (p_suplResponse->posMethod != supl_PosMethod_ver2_agnssSETbased) &&
       (p_suplResponse->posMethod != supl_PosMethod_ver2_agnssSETassistedpref) &&
       (p_suplResponse->posMethod != supl_PosMethod_ver2_agnssSETbasedpref) &&
       (p_suplResponse->posMethod != supl_PosMethod_ver2_autonomousGNSS)
      )
  {
    p_supl_session_status->u_slp_support_glonass = FALSE;
  }
  else
  {
    p_supl_session_status->u_slp_support_glonass = TRUE;
  }

  if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_CELL_ID )
  {
     supl_SuplPosInit_tx();
     return TRUE;
  }

  if ( p_suplResponse->posMethod == supl_noPosition ) /* this posMethod makes no sense in SuplResponse */
  {
    (void) tm_umts_up_supl_supl_end_tx(&p_supl_session_status->z_session_id.z_Slp_Sess_Id,
                                NULL,
                                supl_unexpectedDataValue,
                                NULL,
                                p_supl_session_status->q_supl_version);
    tm_umts_up_supl_callflow_disrupt_handler();
    return FALSE; /* don't proceed */
  }

  /****** check SUPL-2 extension ************/
  if ( p_suplResponse->m.ver2_SUPL_RESPONSE_extensionPresent )
  {
    if ( p_suplResponse->ver2_SUPL_RESPONSE_extension.m.gnssPosTechnologyPresent )
    {
      if ( ( p_suplResponse->ver2_SUPL_RESPONSE_extension.gnssPosTechnology.gps
             == TRUE ) &&
           ( p_suplResponse->ver2_SUPL_RESPONSE_extension.gnssPosTechnology.galileo
              == FALSE ) &&
           ( p_suplResponse->ver2_SUPL_RESPONSE_extension.gnssPosTechnology.glonass
              == FALSE ) &&
           ( p_suplResponse->ver2_SUPL_RESPONSE_extension.gnssPosTechnology.modernized_gps
              == FALSE ) &&
           ( p_suplResponse->ver2_SUPL_RESPONSE_extension.gnssPosTechnology.qzss
              == FALSE ) &&
           ( p_suplResponse->ver2_SUPL_RESPONSE_extension.gnssPosTechnology.sbas
             == FALSE )
          )
      {
        /* TBD: not suppose to happen. This parameter is only meant to be sent to indicate
           at least one non-GPS is requested, and GPS may or may not have been request also;
           if only GPS is requested, this field should not be included */
        MSG_HIGH ("GPS only should not be an option for this field", 0, 0, 0);
      }
      else if ( ( p_suplResponse->ver2_SUPL_RESPONSE_extension.gnssPosTechnology.glonass
                  == FALSE ) &&
                 ( ( p_suplResponse->ver2_SUPL_RESPONSE_extension.gnssPosTechnology.galileo
                    == TRUE ) ||
                   ( p_suplResponse->ver2_SUPL_RESPONSE_extension.gnssPosTechnology.modernized_gps
                    == TRUE ) ||
                   ( p_suplResponse->ver2_SUPL_RESPONSE_extension.gnssPosTechnology.qzss
                    == TRUE ) ||
                   ( p_suplResponse->ver2_SUPL_RESPONSE_extension.gnssPosTechnology.sbas
                    == TRUE )
                 )
              )
      {
        if  ( p_suplResponse->ver2_SUPL_RESPONSE_extension.gnssPosTechnology.gps
              == TRUE )
        {
          /* will just do GPS and ignore all other unsupported techs. */
        }
        else
        {
          /* if SLP not supporting GPS, that means requesting just
             Galileo/mGPS/QZSS/SBAS, not supported here */
          /* need to send SUPL_END posMethodMismatch */
          (void) tm_umts_up_supl_supl_end_tx(
            &p_supl_session_status->z_session_id.z_Slp_Sess_Id,
            NULL,
            supl_StatusCode_posMethodMismatch,
            NULL,
            p_supl_session_status->q_supl_version);
        } /* if only the non-supported tech(s) indicated */

      } /* no Glonass, but at least one of the other non traditional GPS is indicated */


    }
  }

  /* agreement on posMethod has been reached */
  /* if assist data is needed, ask for assistance now  */
  if ( ( p_supl_session_status->z_GpsAssistData.u_Octet3_Whishlist_AH != 0 ) ||
       ( p_supl_session_status->z_GpsAssistData.u_Octet4_Whishlist_IP != 0 )
       ||
       ( p_supl_session_status->z_GloAssistData.u_Octet3_Whishlist_AH != 0 ) ||
       ( p_supl_session_status->z_GloAssistData.u_Octet4_Whishlist_IP != 0 )

        /*If IS801, then wishlist will be present with IS801, if we are here at this point then IS801
          needs network assistance*/
       || (p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801)
    )
  {
    if ( supl_SuplPosInit_tx() )
    {
      u_ret = TRUE;
      if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801)
      {
        /*Wait for write ack of SUPL_POS_INIT before notifying IS801 module*/
        z_supl_session_status.wait_for_write_ack = TRUE;
      }
    }
  }
  else
  {
    /* should not happen */
    u_ret = FALSE;
  }

  return u_ret;


}


/*
******************************************************************************
* supl_SuplPosInit_tx
*
* Function description:
*
* Encode and transmit SUPL_POS_INIT
*
* Parameters: NONE.
*
* Return value: TRUE or FALSE
*
******************************************************************************
*/

boolean supl_SuplPosInit_tx (void)
{
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;

  boolean u_ret = FALSE;


  tm_pdu_buf_type pdu_buff; /* including length and pointer to data */
  supl_ULP_PDU      pdu;
  supl_SUPLPOSINIT*   p_Suplposinit;
  supl_SUPLPOSINIT  z_Suplposinit;
  supl_cell_info_struct_type  z_cell_info_allocated;

  supl_callflow_kind_enum_type  u_callflow_type = p_supl_session_status->u_callflow_kind;

  uint8*           p_length;
  uint8            u_length_msb = 0;
  uint8            u_length_lsb = 0;
  uint8            u_i;

  char          timestamp[14];
  utc_time      z_utc_time;

  supl_cached_pos_for_net_s_type*  p_pos_suplposinit = NULL;
#if defined (FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
  boolean b_ret = TRUE;
#endif /* FEATURE_CGPS_LTE_CELLDB && !FEATURE_GNSS_LOW_MEMORY */

  supl_Velocity z_velocity; 
  supl_Horandvervel z_Horandvervel;
  int encode_status;

  uint8 u_num_mlid = 0;
  supl_LocationIdData *p_LocationIdData = NULL;
  supl_LocationId     *p_LocationId = NULL;
  supl_cell_info_struct_type  *p_cell_info_allocated_mlid = NULL;

  //uint16 w_word;  //used only for formulating ganss time assistance request
  struct supl_GANSSPositionMethod    z_ganssPositionMethod;
  struct supl_GANSSPositionMethods   z_ganssPositionMethodsList;
  struct supl_GANSSPositionMethods   *p_ganssPositionMethodsList =
                                      &z_ganssPositionMethodsList;
  struct supl_GanssReqGenericData    z_ganss_req_generic_data;
  struct supl_GanssRequestedGenericAssistanceDataList  z_ganss_requested_generic_assistance_data_list;
  supl_SatellitesListRelatedData     z_ganss_sat_info_element[15];
  supl_SatellitesListRelatedDataList ganss_sat_info;

  supl_SatelliteInfo sat_info;
  supl_SatelliteInfoElement   z_sat_info_element[15];

  boolean  asn1_range_error = FALSE;
  boolean abort_sending_supl_pos_init = FALSE;

  tm_cm_phone_state_info_s_type phone_state_info;
  supl_Ver2_PosPayLoad_extension z_lpp_payload;
  z_lpp_provide_cap.u_glonass_supported = FALSE;

  /***************************************************************************/

  (void) os_TimerStop( supl_tcp_stay_on_only_timer );
  MSG_MED("Stop tcp_stay_on timer if it's on", 0, 0, 0);

  /* When ready to send SUPL_POSINIT, any timer/state waiting for WIFI MLID should be canceled */
  tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_WIFI_WAIT_TIMER_NO_TCP );
  tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_WIFI_WAIT_SUPLPOSINIT );
  tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_WIFI_WAIT_TIMER_SUPLPOSINIT );

  /* check to see if an active one-shot session exists; if so process with
     this one-shot session; if not, look for an areaEventTrigger session */


  /* Query TM-CM module to get current acquired system */
  tm_cm_iface_get_phone_state(&phone_state_info);

  if ( ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UNDEF ) ||
       ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_NO_POSITION )
     )
  {
    MSG_ERROR("SuplPosInit attempt aborted: gps_mode = %u", p_supl_session_status->u_agps_mode, 0, 0);
    return FALSE;
  }

  check_and_handle_handover(&abort_sending_supl_pos_init);

  if(abort_sending_supl_pos_init)
  {
    return TRUE;
  }

  memset(&pdu, 0x00, sizeof(supl_ULP_PDU));
  memset(&z_cell_info_allocated, 0, sizeof(supl_cell_info_struct_type));
  memset(&z_Suplposinit, 0x00, sizeof(supl_SUPLPOSINIT));
  memset(&sat_info, 0, sizeof(supl_SatelliteInfo) );
  memset(&z_sat_info_element[0], 0, (sizeof(supl_SatelliteInfoElement) * 15) );
  memset(&pdu_buff, 0, sizeof(tm_pdu_buf_type));
  memset(&z_velocity, 0, sizeof(z_velocity)); 
  memset(&z_Horandvervel, 0, sizeof(supl_Horandvervel));
  memset(&z_lpp_payload, 0, sizeof(supl_Ver2_PosPayLoad_extension));
  memset(&z_ganss_req_generic_data, 0, sizeof(supl_GanssReqGenericData));
  memset(&z_ganss_sat_info_element[0], 0, sizeof(supl_SatellitesListRelatedData) * 15 );
  memset(&ganss_sat_info, 0, sizeof(supl_SatellitesListRelatedDataList));
  memset(&z_ganssPositionMethod, 0, sizeof(z_ganssPositionMethod));
  memset(&z_ganssPositionMethodsList, 0, sizeof(z_ganssPositionMethodsList));

  p_Suplposinit = &z_Suplposinit;
  pdu.message.t = C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLPOSINIT;
  pdu.message.u.msSUPLPOSINIT = p_Suplposinit;


  MSG_LOW("SuplPosInit Tx. CF type %u, LPP ProvCapflag 0x%lX, PosMod %d", u_callflow_type, z_lpp_provide_cap.q_flags,
          p_supl_session_status->u_agps_mode);

  /* overhead build function has to be after we know what message is to be constructed */
  if ( !supl_header_build (&pdu, NULL, NULL, NULL, p_supl_session_status->q_supl_version) )
  {
    MSG_ERROR("SUPL: SuplPosInit attempt aborted: header", 0, 0, 0);
    return FALSE;
  }

  /* Positioning technology */
  p_Suplposinit->sETCapabilities.posTechnology.aFLT = FALSE;
  p_Suplposinit->sETCapabilities.posTechnology.eOTD = FALSE;
  p_Suplposinit->sETCapabilities.posTechnology.oTDOA = FALSE;
  p_Suplposinit->sETCapabilities.posTechnology.eCID = FALSE; /* maybe modified below */

  /* posTechnology AGPS  -- if SET-initiated: capability - pdapi requested method that is
     provisioned; prefered method - NoPreference, per MSM6275 SUPL 1.0 Support Reference Guide */
  if ( ( u_callflow_type == C_SUPL_SESSION_UI ) || ( u_callflow_type == C_SUPL_SESSION_KEEP_WARM ) )
  {

    if ( u_callflow_type == C_SUPL_SESSION_KEEP_WARM )
    {
      p_Suplposinit->sETCapabilities.posTechnology.agpsSETBased = TRUE;
      z_lpp_provide_cap.z_agnss_capabilities.u_pos_mode_supported = LPP_POS_MODE_UE_BASED;
    }
    else if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED )
    {
      p_Suplposinit->sETCapabilities.posTechnology.agpsSETassisted = TRUE;
      z_lpp_provide_cap.z_agnss_capabilities.u_pos_mode_supported = LPP_POS_MODE_UE_ASSISTED;
      if ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP )
      {
        p_Suplposinit->sETCapabilities.posTechnology.oTDOA = TRUE;
        z_lpp_provide_cap.u_otdoa_supported = TRUE;
      }
    }
    else if ( p_supl_session_status->u_agps_mode == C_SUPL_AFLT_MODE )
    {
      if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801)
      {
          p_Suplposinit->sETCapabilities.posTechnology.aFLT = TRUE;
      }

      if ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP )
      {
        p_Suplposinit->sETCapabilities.posTechnology.oTDOA = TRUE;
        z_lpp_provide_cap.u_otdoa_supported = TRUE;
      }
    }
    else if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_BASED )
    {
      p_Suplposinit->sETCapabilities.posTechnology.agpsSETBased = TRUE;
      z_lpp_provide_cap.z_agnss_capabilities.u_pos_mode_supported = LPP_POS_MODE_UE_BASED;
      z_lpp_provide_cap.z_agnss_capabilities.u_pos_mode_supported |= LPP_POS_MODE_STANDALONE;

      /*IS801 does not support Autonomous GPS mode*/
      if(p_supl_session_status->u_position_protocol != TM_POS_PRTL_IS801)
      {
        #ifndef FEATURE_CGPS_SUPL_VARIANT_SET_CAP_NO_AUTO /* this is NOT by default defined */
        /* in case SLP wants to do auto. */
        p_Suplposinit->sETCapabilities.posTechnology.autonomousGPS = TRUE;
        #endif
      }
    }
    else if ( p_supl_session_status->u_agps_mode == C_SUPL_OTDOA_MODE )
    {
      if ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP )
      {
        z_lpp_provide_cap.z_agnss_capabilities.u_pos_mode_supported = 0;
        p_Suplposinit->sETCapabilities.posTechnology.oTDOA = TRUE;
        z_lpp_provide_cap.u_otdoa_supported = TRUE;
      }
    }
    else if (p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_CELL_ID)
    {
       p_Suplposinit->sETCapabilities.posTechnology.agpsSETassisted = FALSE;
       p_Suplposinit->sETCapabilities.posTechnology.autonomousGPS = FALSE;
       p_Suplposinit->sETCapabilities.posTechnology.agpsSETBased = FALSE;
    }

    else
    {
      /* no other agps mode should generate SUPL_POS_INIT */
      MSG_ERROR("SUPL: SuplPosInit attempt aborted", 0, 0, 0);
      return FALSE;
    }

    #ifdef FEATURE_CGPS_SUPL_VARIANT_SET_CAP_PREF_SET_BASED /* NOT defined by default */
    #error code not present
#else
    p_Suplposinit->sETCapabilities.prefMethod = supl_PrefMethod_noPreference;
    #endif


    /* positioning protocol */
    p_Suplposinit->sETCapabilities.posProtocol.rrc = FALSE;
    if(TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable) !=0)
    {
      p_Suplposinit->sETCapabilities.posProtocol.tia801 = TRUE;
    }
    else
    {
      p_Suplposinit->sETCapabilities.posProtocol.tia801 = FALSE;
    }
    if ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP )
    {
      p_Suplposinit->sETCapabilities.posProtocol.m.ver2_PosProtocol_extensionPresent = 1;
      p_Suplposinit->sETCapabilities.posProtocol.ver2_PosProtocol_extension.lpp = TRUE;
    }
    else
    {
      p_Suplposinit->sETCapabilities.posProtocol.rrlp = TRUE;
    }

    z_lpp_provide_cap.q_flags = LPP_REQ_CAP_AGNSS_REQ;
    if (z_lpp_provide_cap.u_otdoa_supported == TRUE)
    {
      z_lpp_provide_cap.q_flags |= LPP_REQ_CAP_OTDOA_REQ;
    }
  }

  else /*posTechnology AGPS -- network-initiated*/
  {
    if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_RRLP)
    {
      /* posTechnology AGPS -- network-initiated: all method regardless
         what SUPL_INIT says, with PreferedMethod set to NoPref, per MSM6275 SUPL 1.0
         Support Reference Guide */
        if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_UP_MS_ASSISTED) )
        {
          p_Suplposinit->sETCapabilities.posTechnology.agpsSETassisted = TRUE;
        }
        if ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP )
        {
          p_Suplposinit->sETCapabilities.posTechnology.oTDOA = TRUE;
        }

        if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_UP_MS_BASED) )
        {
          p_Suplposinit->sETCapabilities.posTechnology.agpsSETBased = TRUE;
        }

        if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_STANDALONE) )
        {
          p_Suplposinit->sETCapabilities.posTechnology.autonomousGPS = TRUE;
        }
    }
    else if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP)
    {
      /* posTechnology AGPS -- network-initiated: all method regardless
         what SUPL_INIT says, with PreferedMethod set to NoPref, per MSM6275 SUPL 1.0
         Support Reference Guide */
        if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_4G_UP_MS_ASSISTED) )
        {
          p_Suplposinit->sETCapabilities.posTechnology.agpsSETassisted = TRUE;
          z_lpp_provide_cap.z_agnss_capabilities.u_pos_mode_supported = LPP_POS_MODE_UE_ASSISTED;
          p_Suplposinit->sETCapabilities.posTechnology.oTDOA = TRUE;
          z_lpp_provide_cap.u_otdoa_supported = TRUE;
        }
        if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_4G_UP_MS_BASED) )
        {
          p_Suplposinit->sETCapabilities.posTechnology.agpsSETBased = TRUE;
          z_lpp_provide_cap.z_agnss_capabilities.u_pos_mode_supported |= LPP_POS_MODE_UE_BASED;
        }

        if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_STANDALONE) )
        {
          p_Suplposinit->sETCapabilities.posTechnology.autonomousGPS = TRUE;
          z_lpp_provide_cap.z_agnss_capabilities.u_pos_mode_supported |= LPP_POS_MODE_STANDALONE;
        }
        z_lpp_provide_cap.q_flags = LPP_REQ_CAP_AGNSS_REQ;
        if (z_lpp_provide_cap.u_otdoa_supported == TRUE)
        {
          z_lpp_provide_cap.q_flags |= LPP_REQ_CAP_OTDOA_REQ;
        }
    } /*RRLP or LPP as pos protocol*/
    else if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801) /*IS801 as pos protocol*/
    {
      if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED)
      {
        p_Suplposinit->sETCapabilities.posTechnology.agpsSETassisted = TRUE;
      }
      else if(p_supl_session_status->u_agps_mode == C_SUPL_AFLT_MODE)
      {
        p_Suplposinit->sETCapabilities.posTechnology.aFLT = TRUE;
      }
      else if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_BASED )
      {
        p_Suplposinit->sETCapabilities.posTechnology.agpsSETBased = TRUE;
      }
      else if (p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_CELL_ID )
      {
        /*For ECID, no capabilities should be shown in SUPL_INIT*/
      }
      else
      {
        /* no other agps mode should generate SUPL_POS_INIT */
        MSG_ERROR("SUPL: SuplPosInit attempt aborted", 0, 0, 0);
        return FALSE;
      }
    }/*IS801 as pos protocol*/
    else
    {
      MSG_ERROR("Pos Protocol not supported", 0,0,0);
    }

    p_Suplposinit->sETCapabilities.prefMethod = supl_PrefMethod_noPreference;

    /* positioning protocol */
    p_Suplposinit->sETCapabilities.posProtocol.rrc = FALSE;
    if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801)
    {
      p_Suplposinit->sETCapabilities.posProtocol.tia801 = TRUE;
    }
    else if ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP )
    {
      p_Suplposinit->sETCapabilities.posProtocol.ver2_PosProtocol_extension.lpp = TRUE;
    }
    else
    {
      p_Suplposinit->sETCapabilities.posProtocol.rrlp = TRUE;
    }

    z_lpp_provide_cap.q_flags = LPP_REQ_CAP_AGNSS_REQ;
    if (z_lpp_provide_cap.u_otdoa_supported == TRUE)
    {
      z_lpp_provide_cap.q_flags |= LPP_REQ_CAP_OTDOA_REQ;
    }
  }

  /**** SUPL-2 only: Service Capabilities ***/

  if ( (p_supl_session_status->q_supl_version & 0x000000FF) == 2 )
  {
    /***********  Service Capabilities optional SUPL2 extension ********/

    #if 0 /* For further dev. */

    p_Suplposinit->sETCapabilities.m.ver2_SETCapabilities_extensionPresent = 1;

    p_Suplposinit->sETCapabilities.ver2_SETCapabilities_extension.m.serviceCapabilitiesPresent = 1;

    /* neither reportingCapabilities or eventTriggerCapabilities is present */

    /* neither periodic trigger or area event trigger is supported */

    #endif

    /***********  PosProtocolVersionRRLP SUPL2 extension in PosProtocol ****/

    p_Suplposinit->sETCapabilities.posProtocol.m.ver2_PosProtocol_extensionPresent = 1;

    if ( p_Suplposinit->sETCapabilities.posProtocol.rrlp == TRUE )
    {
      p_Suplposinit->sETCapabilities.posProtocol.ver2_PosProtocol_extension.m.posProtocolVersionRRLPPresent = 1;

      /* default to the backward compatible version first, may be updated below */
      p_Suplposinit->sETCapabilities.posProtocol.ver2_PosProtocol_extension.posProtocolVersionRRLP.majorVersionField =
        SUPL_RRLP_BACK_VERSION_MAJOR;
      p_Suplposinit->sETCapabilities.posProtocol.ver2_PosProtocol_extension.posProtocolVersionRRLP.technicalVersionField =
        SUPL_RRLP_BACK_VERSION_TECH;
      p_Suplposinit->sETCapabilities.posProtocol.ver2_PosProtocol_extension.posProtocolVersionRRLP.editorialVersionField =
        SUPL_RRLP_BACK_VERSION_EDIT;
    }

    if ( p_Suplposinit->sETCapabilities.posProtocol.ver2_PosProtocol_extension.lpp == TRUE )
    {
      p_Suplposinit->sETCapabilities.posProtocol.ver2_PosProtocol_extension.m.posProtocolVersionLPPPresent = 1;
      p_Suplposinit->sETCapabilities.posProtocol.ver2_PosProtocol_extension.posProtocolVersionLPP.majorVersionField = LPP_CURRENT_VERSION_MAJOR;
      p_Suplposinit->sETCapabilities.posProtocol.ver2_PosProtocol_extension.posProtocolVersionLPP.technicalVersionField = LPP_CURRENT_VERSION_TECH;
      p_Suplposinit->sETCapabilities.posProtocol.ver2_PosProtocol_extension.posProtocolVersionLPP.editorialVersionField = LPP_CURRENT_VERSION_EDIT;
    }

    /****** ver2_PosTechnology SUPL2 extension in posTechnology **********/

    // Glonass enabled and Protocol is not IS801
    if ( tm_umts_common_glonass_enabled() && (p_supl_session_status->u_position_protocol != TM_POS_PRTL_IS801) )
    {
      supl_fill_ver2_ganss_set_capability( &(p_Suplposinit->sETCapabilities), &z_ganssPositionMethod,
                                           p_ganssPositionMethodsList, u_callflow_type );
    } /* Glonass */
    else
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Glonass not enabled.");
    }
  }

  if ( !supl_cell_information_construct(&p_Suplposinit->locationId.cellInfo,
                                        &z_cell_info_allocated) )
  {
    MSG_HIGH("No Cell ID info.", 0, 0, 0);
    return FALSE;
  }

  /* MLID for SUPL_POS_INIT */
  if ( ( p_supl_session_status->q_supl_version & 0x000000FF ) >= 2 )
  {
    if ( u_supl_global_num_mlid > 1 ) /* note being 1 means only the current serving cell is in the structure */
    {
      p_LocationIdData = (supl_LocationIdData*)os_MemAlloc((sizeof(supl_LocationIdData) * (u_supl_global_num_mlid - 1)), OS_MEM_SCOPE_TASK);
      p_LocationId = (supl_LocationId*)os_MemAlloc((sizeof(supl_LocationId) * (u_supl_global_num_mlid - 1)), OS_MEM_SCOPE_TASK);
      p_cell_info_allocated_mlid = (supl_cell_info_struct_type*)os_MemAlloc((sizeof(supl_cell_info_struct_type) * (u_supl_global_num_mlid - 1)), OS_MEM_SCOPE_TASK);

      if ( ( p_LocationIdData == NULL ) || (p_LocationId == NULL) || (p_cell_info_allocated_mlid == NULL ) )
      {
        u_num_mlid = 0;
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "mlid mem. allocation failed, set num_mlid to %u ", u_num_mlid);
      }
      else
      {
        memset(p_LocationIdData, 0, sizeof(supl_LocationIdData) * ( u_supl_global_num_mlid - 1) );
        memset(p_LocationId, 0, sizeof(supl_LocationId) * ( u_supl_global_num_mlid - 1) );
        memset(p_cell_info_allocated_mlid, 0, sizeof(supl_cell_info_struct_type) * ( u_supl_global_num_mlid - 1) );

        p_LocationIdData->locationId = p_LocationId;
        u_num_mlid = supl_chk_and_build_all_mlid(p_LocationIdData, p_LocationId,
                                                 p_cell_info_allocated_mlid, ( u_supl_global_num_mlid - 1) );
      }
    }
    if ( u_num_mlid > 0 ) /* valid mlid exists */
    {
      if (p_LocationIdData != NULL)
      {
        p_Suplposinit->m.ver2_SUPL_POS_INIT_extensionPresent = TRUE;
        p_Suplposinit->ver2_SUPL_POS_INIT_extension.m.multipleLocationIdsPresent = TRUE;
        p_Suplposinit->ver2_SUPL_POS_INIT_extension.multipleLocationIds.n = u_num_mlid;
        p_Suplposinit->ver2_SUPL_POS_INIT_extension.multipleLocationIds.elem = p_LocationIdData;
      }
      else
      {
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "p_LocationIdData not allocated: %lx ", p_LocationIdData);
      }
    }
  } /* running supl ver. >= 2 */

  if ( ( phone_state_info.srv_system == CGPS_SRV_SYS_WCDMA ) &&
       z_cell_info_cached.u_CachedCellIDValid &&
       ( z_cell_info_cached.e_ActiveRAT == CELLDB_ACTIVERAT_WCDMA )
     )
  {
    #ifdef FEATURE_USER_PLANE_MRL
    p_Suplposinit->sETCapabilities.posTechnology.eCID = TRUE;
    #endif

    #ifdef FEATURE_CGPS_SUPL_VARIANT_NO_ECID /* NOT defined by default */
    #error code not present
#endif

    if ( z_cell_info_cached.q_NumCellChangesCachedCellID > 1 )
    {
      /* when there is no cell change, this number is 1 */
      q_handovers_since_last_cell_id =
        z_cell_info_cached.q_NumCellChangesCachedCellID - 1 ;
    }
    else
    {
      /* current, or no info. available assume current */
      q_handovers_since_last_cell_id = 0;
    }
  } /* in WCDMA with valid cell info. */
  else if ((phone_state_info.srv_system == CGPS_SRV_SYS_GSM) &&
    (TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_NMR_SUPPORTED) != 0))
  {
    p_Suplposinit->sETCapabilities.posTechnology.eCID = TRUE;

    #ifdef FEATURE_CGPS_SUPL_VARIANT_NO_ECID /* NOT defined by default */
    #error code not present
#endif
  }

#ifdef FEATURE_CGPS_LTE_CELLDB
  else if ( ( phone_state_info.srv_system == CGPS_SRV_SYS_LTE ) &&
            z_cell_info_cached.u_CachedCellIDValid  &&
            ( z_cell_info_cached.e_ActiveRAT == CELLDB_ACTIVERAT_LTE ) &&
            ( ( p_supl_session_status->q_supl_version & 0x000000FF ) == 2 )
          )
  {
    /* if SUPL2, and supl_cell_information_construct() returns TRUE */

    if ( ( p_Suplposinit->locationId.cellInfo.u.ver2_CellInfo_extension->u.lteCell->m.rsrpResultPresent ||
           p_Suplposinit->locationId.cellInfo.u.ver2_CellInfo_extension->u.lteCell->m.rsrqResultPresent ||
           p_Suplposinit->locationId.cellInfo.u.ver2_CellInfo_extension->u.lteCell->m.tAPresent
         ) ||
         ( z_lte_mrl.num_cells > 1 )
       )
    {
      p_Suplposinit->sETCapabilities.posTechnology.eCID = TRUE;
      p_supl_session_status->u_supl_posMethod_ecid_valid = TRUE;
    }
  }

#endif


  /* set this to CURRENT if cell id is of the current cell; otherwise set
     it to STATE to prevent the network from using this cell id to generate
     cell-id based positions for NI */
  if ( q_handovers_since_last_cell_id == 0 )
  {
    p_Suplposinit->locationId.status = supl_Status_current;
  }
  else
  {
    if ( p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_NI )
    {
      p_Suplposinit->locationId.status = supl_Status_stale;
    }
    else
    {
      /* for SI, regardless the actual status, always use "current" and the SET
         will derate the reference position provided by the network */
      MSG_HIGH("Stale cell_id, but report current for MO", 0, 0, 0 );
      p_Suplposinit->locationId.status = supl_Status_current;
    }
  }

  /*Request Assistance Data only if RRLP is selected*/
  if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_RRLP)
  {
    /* AD Request. If positioning mode is currently undefined, do not request any
       assistance data at this time */
    if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_BASED )
    {
      if ( ( p_supl_session_status->z_GpsAssistData.u_Octet3_Whishlist_AH != 0 ) ||
           ( p_supl_session_status->z_GpsAssistData.u_Octet4_Whishlist_IP != 0 )
         )
      {
        p_Suplposinit->m.requestedAssistDataPresent = 1;
      }

      if ( p_supl_session_status->z_GpsAssistData.u_Octet3_Whishlist_AH &  C_G0931_GPS_ASSIST_ALM )
          p_Suplposinit->requestedAssistData.almanacRequested = TRUE;
      if ( p_supl_session_status->z_GpsAssistData.u_Octet3_Whishlist_AH &  C_G0931_GPS_ASSIST_UTC )
        p_Suplposinit->requestedAssistData.utcModelRequested = TRUE;
      if ( p_supl_session_status->z_GpsAssistData.u_Octet3_Whishlist_AH &  C_G0931_GPS_ASSIST_IONO )
        p_Suplposinit->requestedAssistData.ionosphericModelRequested = TRUE;
      if ( p_supl_session_status->z_GpsAssistData.u_Octet3_Whishlist_AH &  C_G0931_GPS_ASSIST_NAVMODL )
        p_Suplposinit->requestedAssistData.navigationModelRequested = TRUE;
      if ( p_supl_session_status->z_GpsAssistData.u_Octet3_Whishlist_AH &  C_G0931_GPS_ASSIST_REFLOC )
        p_Suplposinit->requestedAssistData.referenceLocationRequested = TRUE;
      if ( p_supl_session_status->z_GpsAssistData.u_Octet3_Whishlist_AH &  C_G0931_GPS_ASSIST_REFTIME )
        p_Suplposinit->requestedAssistData.referenceTimeRequested = TRUE;
      if ( p_supl_session_status->z_GpsAssistData.u_Octet4_Whishlist_IP & (C_G0931_GPS_ASSIST_RTI>>8) )
        p_Suplposinit->requestedAssistData.realTimeIntegrityRequested = TRUE;


      #ifndef FEATURE_CGPS_SUPL_VARIANT_NO_TARGETED_EPH  /* NOT defined by default */
      if ( p_Suplposinit->requestedAssistData.navigationModelRequested == TRUE )
      {
        p_Suplposinit->requestedAssistData.m.navigationModelDataPresent = 1;

        #if 0 /* TEST code only: protocol test code only: keep this here for possible quick protocol
                 verfication but make sure it is not activated; test is conducted by deleting
                 eph. first to clear the saved info. */

        p_supl_session_status->z_GpsAssistData.u_Octet8_NSAT_ToeLimit = 0x10; /* SV num. = 1 */
        p_supl_session_status->z_GpsAssistData.w_SvId_Iode[0] = 0x1207; /* SV ID: 7; IODE: 0x12 */
        p_supl_session_status->z_GpsAssistData.u_Octet6_GpsWeek_LSB8 = 0xFE; /* week: 1022 = 0x3FE */
        p_supl_session_status->z_GpsAssistData.u_Octet5_GpsWeek_MSB2 = 0x0C;  /* MSB of 10-bit moved to MSB of 8-bit */
        p_supl_session_status->z_GpsAssistData.u_Octet7_GpsToe = 150;
        p_supl_session_status->z_GpsAssistData.u_Octet8_NSAT_ToeLimit |= 0x5; /* ToeLimit: 5 */
        #endif


        if ( (p_supl_session_status->z_GpsAssistData.u_Octet8_NSAT_ToeLimit >> 4) > 0 )
        {
          uint8 u_nsat = 0;

          p_Suplposinit->requestedAssistData.navigationModelData.m.satInfoPresent = 1;

          p_Suplposinit->requestedAssistData.navigationModelData.gpsWeek =
            p_supl_session_status->z_GpsAssistData.u_Octet6_GpsWeek_LSB8;

          p_Suplposinit->requestedAssistData.navigationModelData.gpsWeek |=
            ( (uint16)p_supl_session_status->z_GpsAssistData.u_Octet5_GpsWeek_MSB2 ) << 2;

          p_Suplposinit->requestedAssistData.navigationModelData.gpsToe =
            p_supl_session_status->z_GpsAssistData.u_Octet7_GpsToe;

          u_nsat = p_supl_session_status->z_GpsAssistData.u_Octet8_NSAT_ToeLimit >> 4;
          p_Suplposinit->requestedAssistData.navigationModelData.nSAT =
            ( u_nsat <= 15 ) ? u_nsat : 15;

          p_Suplposinit->requestedAssistData.navigationModelData.toeLimit =
            p_supl_session_status->z_GpsAssistData.u_Octet8_NSAT_ToeLimit & 0x0F ;

          /* asn1 range check:
             gpsWeek   INTEGER(0..1023);
             gpsToe    INTEGER(0..167);
             nSAT      INTEGER(0..31);
             toeLimit  INTEGER(0..10)  */
          asn1_range_error = FALSE;
          if (  ( p_Suplposinit->requestedAssistData.navigationModelData.gpsWeek > 1023 ) ||
                ( p_Suplposinit->requestedAssistData.navigationModelData.gpsToe > 167 ) ||
                ( p_Suplposinit->requestedAssistData.navigationModelData.nSAT > 31 ) ||
                ( p_Suplposinit->requestedAssistData.navigationModelData.toeLimit > 10 )
             )
          {
            MSG_HIGH ("navModelData out of asn1 range !", 0,0,0);
            asn1_range_error = TRUE;
          }

          sat_info.n = p_Suplposinit->requestedAssistData.navigationModelData.nSAT;
          sat_info.elem = &z_sat_info_element[0];


          for ( u_i = 0; u_i < p_Suplposinit->requestedAssistData.navigationModelData.nSAT; u_i++ )
          {
            z_sat_info_element[u_i].satId =
              ( p_supl_session_status->z_GpsAssistData.w_SvId_Iode[u_i] & 0x003F ) -1 ;

            z_sat_info_element[u_i].iODE =
              p_supl_session_status->z_GpsAssistData.w_SvId_Iode[u_i] >> 8;

            /* asn1 range check:  satId  INTEGER(0..63),  iODE   INTEGER(0..255) */
            if ( ( z_sat_info_element[u_i].satId > 63 ) || ( z_sat_info_element[u_i].iODE > ((255)) ) )
            {
              MSG_HIGH ("navModelData out of asn1 range !", 0,0,0);
              asn1_range_error = TRUE;
            }

          }

          #if 0
          memscpy( &p_Suplposinit->requestedAssistData.navigationModelData.satInfo,
                  sizeof(p_Suplposinit->requestedAssistData.navigationModelData.satInfo),
                  &sat_info, sizeof(sat_info));
          #endif
          p_Suplposinit->requestedAssistData.navigationModelData.satInfo.n =
             p_Suplposinit->requestedAssistData.navigationModelData.nSAT;
          p_Suplposinit->requestedAssistData.navigationModelData.satInfo.elem =
             &z_sat_info_element[0];

          if ( asn1_range_error )
          {
            /* some vaule(s) out of range for ASN.1, don't encode with navModelData */
            p_Suplposinit->requestedAssistData.navigationModelData.m.satInfoPresent = 0;
            p_Suplposinit->requestedAssistData.navigationModelData.gpsWeek = 0;
            p_Suplposinit->requestedAssistData.navigationModelData.gpsToe = 0;
            p_Suplposinit->requestedAssistData.navigationModelData.nSAT = 0;
            p_Suplposinit->requestedAssistData.navigationModelData.toeLimit = 0;
          }

        } /* closing if-NavData included */

        else /* if no NavData included */
        {
          p_Suplposinit->requestedAssistData.navigationModelData.m.satInfoPresent = 0;
          p_Suplposinit->requestedAssistData.navigationModelData.gpsWeek = 0;
          p_Suplposinit->requestedAssistData.navigationModelData.gpsToe = 0;
          p_Suplposinit->requestedAssistData.navigationModelData.nSAT = 0;
          p_Suplposinit->requestedAssistData.navigationModelData.toeLimit = 0;

          p_Suplposinit->requestedAssistData.navigationModelData.satInfo.n = 0;
          p_Suplposinit->requestedAssistData.navigationModelData.satInfo.elem = NULL;
        }

      } /* closing if-NavModel request */
      #endif  /* feature_no_target_eph */

      if ( tm_umts_up_supl_rrlp_assisted_glonass_enabled() && (p_supl_session_status->u_position_protocol != TM_POS_PRTL_IS801) )
      {
        // Don't request GLO AD if UE's Glonass capability is disabled or H-SLP indicated no GLONASS support in UI call flow
        if ( FALSE == tm_up_slp_support_glonass( p_supl_session_status ) )
        {
          MSG( MSG_SSID_GPSSM, MSG_LEGACY_MED, "No GLO AD requested. H-SLP not capable of glonass" );
        }
        else
        {
        if ( ( p_supl_session_status->z_GloAssistData.u_Octet3_Whishlist_AH != 0 ) ||
             ( p_supl_session_status->z_GloAssistData.u_Octet4_Whishlist_IP != 0 )
           )
        {
          p_Suplposinit->m.requestedAssistDataPresent = 1;
          p_Suplposinit->requestedAssistData.m.ver2_RequestedAssistData_extensionPresent = 1;
        }

            /********  Common assistance request ****************************/

            if ( p_supl_session_status->z_GloAssistData.u_Octet3_Whishlist_AH &
                 C_G0931_GPS_ASSIST_REFLOC )
            {
              /* note there is no gnss refloc request in the supl-2 extension, since
                 ref loc. is generic to GPS or other technologies, the request goes
                 out only in the one originally for GPS. */
              p_Suplposinit->requestedAssistData.referenceLocationRequested = TRUE;
            }

            if ( p_supl_session_status->z_GloAssistData.u_Octet3_Whishlist_AH &
                 C_G0931_GPS_ASSIST_REFTIME )
            {
              /* instead of requesting GLONASS reference time, request GPS ref. time and UTC */
              p_Suplposinit->requestedAssistData.referenceTimeRequested = TRUE;
              p_Suplposinit->requestedAssistData.utcModelRequested = TRUE;
            }

          if ( p_supl_session_status->z_GloAssistData.u_Octet3_Whishlist_AH &
                  C_G0931_GPS_ASSIST_IONO )
            {
              p_Suplposinit->requestedAssistData.ver2_RequestedAssistData_extension.m.ganssRequestedCommonAssistanceDataListPresent = TRUE;
              p_Suplposinit->requestedAssistData.ver2_RequestedAssistData_extension.ganssRequestedCommonAssistanceDataList.ganssIonosphericModel = TRUE;
            }

            /********** Generic assistance request **************************/
            if (  p_supl_session_status->z_OrigWishList.z_GloAssistData.q_WishListMask & C_GNSS_ASSIST_TIME_MODELS )
            {
              p_supl_session_status->z_OrigWishList.z_GloAssistData.q_WishListMask &= ~C_GNSS_ASSIST_TIME_MODELS;
              z_ganss_req_generic_data.m.ganssTimeModelsPresent = TRUE;
              z_ganss_req_generic_data.ganssTimeModels.numbits = 16;
              z_ganss_req_generic_data.ganssTimeModels.data[0] = C_GANSS_SYSTEM_TIME_MODEL_REQUESTED_GPS;
              z_ganss_req_generic_data.ganssTimeModels.data[1] = 0x0;
            }

            if ( p_supl_session_status->z_GloAssistData.u_Octet3_Whishlist_AH &  C_G0931_GPS_ASSIST_NAVMODL )
            {
              z_ganss_req_generic_data.m.ganssNavigationModelDataPresent =  1;

              /* if nav. model is request, also request aux. which has freqency info. */
              z_ganss_req_generic_data.ganssAuxiliaryInformation = TRUE;
              z_ganss_req_generic_data.ganssNavigationModelData.extElem1.count = 0;

#ifndef FEATURE_CGPS_SUPL_VARIANT_NO_TARGETED_EPH  /* NOT defined by default */

#if 0 /* DONT REMOVE: TEST code only: protocol test code only: keep this here for possible
                         quick protocol verfication but make sure it is not activated; test is conducted by
                         deleting eph. first to clear the saved info. */

                p_supl_session_status->z_GloAssistData.u_Octet8_NSAT_ToeLimit = 0xF0; /* SV num. = 1 */

                p_supl_session_status->z_GloAssistData.w_SvId_Iode[0] = 0x1101; /* SV ID: 1; IODE: 0x12 */
                p_supl_session_status->z_GloAssistData.w_SvId_Iode[1] = 0x1202; /* SV ID: 7; IODE: 0x12 */
                p_supl_session_status->z_GloAssistData.w_SvId_Iode[2] = 0x1303; /* SV ID: 7; IODE: 0x12 */
                p_supl_session_status->z_GloAssistData.w_SvId_Iode[3] = 0x1404; /* SV ID: 7; IODE: 0x12 */
                p_supl_session_status->z_GloAssistData.w_SvId_Iode[4] = 0x1505; /* SV ID: 7; IODE: 0x12 */
                p_supl_session_status->z_GloAssistData.w_SvId_Iode[5] = 0x1606; /* SV ID: 7; IODE: 0x12 */
                p_supl_session_status->z_GloAssistData.w_SvId_Iode[6] = 0x1707; /* SV ID: 7; IODE: 0x12 */
                p_supl_session_status->z_GloAssistData.w_SvId_Iode[7] = 0x1808; /* SV ID: 7; IODE: 0x12 */
                p_supl_session_status->z_GloAssistData.w_SvId_Iode[8] = 0x1909; /* SV ID: 7; IODE: 0x12 */
                p_supl_session_status->z_GloAssistData.w_SvId_Iode[9] = 0x1A0A; /* SV ID: 7; IODE: 0x12 */
                p_supl_session_status->z_GloAssistData.w_SvId_Iode[10] = 0x1B0B; /* SV ID: 7; IODE: 0x12 */
                p_supl_session_status->z_GloAssistData.w_SvId_Iode[11] = 0x1C0C; /* SV ID: 7; IODE: 0x12 */
                p_supl_session_status->z_GloAssistData.w_SvId_Iode[12] = 0x1D0D; /* SV ID: 7; IODE: 0x12 */
                p_supl_session_status->z_GloAssistData.w_SvId_Iode[13] = 0x1E0E; /* SV ID: 7; IODE: 0x12 */
                p_supl_session_status->z_GloAssistData.w_SvId_Iode[14] = 0x1F0F; /* SV ID: 7; IODE: 0x12 */

                p_supl_session_status->z_GloAssistData.u_Octet6_GanssWeek_LSB8 = 0xFE; /* week: 1022 = 0x3FE */
                p_supl_session_status->z_GloAssistData.u_Octet5_GanssWeek_MSB4 = 0x0C;  /* MSB of 10-bit moved to MSB of 8-bit */
                p_supl_session_status->z_GloAssistData.u_Octet7_GanssToe = 150;
                p_supl_session_status->z_GloAssistData.u_Octet8_NSAT_ToeLimit |= 0x5; /* ToeLimit: 5 */

#endif /* TEST only code */

              if ( (p_supl_session_status->z_GloAssistData.u_Octet8_NSAT_ToeLimit >> 4) > 0 )
              {
                uint8 u_nsat = 0;

                z_ganss_req_generic_data.ganssNavigationModelData.m.satellitesListRelatedDataListPresent = 1;

                /* Refer to 80-V5430-3 A , Table 3-28
                     for the format of GpsAssisData_ganss_Gsm0931 for data processing below.
                   Note: ganssWeek is 12 bit field (gpsweek is 10 bit).
                         IODE is      10 bit field (gps IODE is 8 bit).
                */
                z_ganss_req_generic_data.ganssNavigationModelData.ganssWeek =
                  p_supl_session_status->z_GloAssistData.u_Octet6_GanssWeek_LSB8;
                z_ganss_req_generic_data.ganssNavigationModelData.ganssWeek |=
                                                                             ((uint16)p_supl_session_status->z_GloAssistData.u_Octet5_GanssWeek_MSB4 ) << 2;

                z_ganss_req_generic_data.ganssNavigationModelData.ganssToe =
                  p_supl_session_status->z_GloAssistData.u_Octet7_GanssToe;

                u_nsat = p_supl_session_status->z_GloAssistData.u_Octet8_NSAT_ToeLimit >> 4;
                z_ganss_req_generic_data.ganssNavigationModelData.satellitesListRelatedDataList.n =
                   ( u_nsat <= 15 ) ? u_nsat : 15;

                z_ganss_req_generic_data.ganssNavigationModelData.t_toeLimit =
                                                                             p_supl_session_status->z_GloAssistData.u_Octet8_NSAT_ToeLimit & 0x0F;

                /* asn1 range check:
                   ganssWeek   INTEGER(0..4095);
                   ganssToe    INTEGER(0..167);
                   toeLimit    INTEGER(0..15) )
                   NOTE - 80-V5430-3 A says range is 0..10 but tm_supl_asn1.c code allows up to 15
                */
                asn1_range_error = FALSE;
                if (  ( z_ganss_req_generic_data.ganssNavigationModelData.ganssWeek > 4095 ) ||
                      ( z_ganss_req_generic_data.ganssNavigationModelData.ganssToe > 167 ) ||
                      ( z_ganss_req_generic_data.ganssNavigationModelData.t_toeLimit > 15 )
                   )
                {
                MSG_HIGH( "GANSS navModelData out of asn1 range !", 0, 0, 0 );
                  asn1_range_error = TRUE;
                }
                ganss_sat_info.n = z_ganss_req_generic_data.ganssNavigationModelData.satellitesListRelatedDataList.n;
                ganss_sat_info.elem = &z_ganss_sat_info_element[0];
                for ( u_i = 0; u_i < ganss_sat_info.n; u_i++ )
                {
                  z_ganss_sat_info_element[u_i].satId =
                                                      ( p_supl_session_status->z_GloAssistData.w_SvId_Iode[u_i] & 0x003F ) - 1;

                  z_ganss_sat_info_element[u_i].iod =
                    p_supl_session_status->z_GloAssistData.w_SvId_Iode[u_i] >> 8; /* 8 LSB of 10 bit IODE */

                  z_ganss_sat_info_element[u_i].iod |=
                    (p_supl_session_status->z_GloAssistData.w_SvId_Iode[u_i] & 0x00C0) << 2; /* 2 MSB of 10 bit IODE */

                  /* asn1 range check:  satId  INTEGER(0..63),  iODE   INTEGER(0..1023) */
                  if ( ( z_ganss_sat_info_element[u_i].satId > 63 ) || ( z_ganss_sat_info_element[u_i].iod > 1023 ) )
                  {
                  MSG_HIGH( "navModelData out of asn1 range !", 0, 0, 0 );
                    asn1_range_error = TRUE;
                  }
                } /* for */
                z_ganss_req_generic_data.ganssNavigationModelData.satellitesListRelatedDataList.n =
                  ganss_sat_info.n;
                z_ganss_req_generic_data.ganssNavigationModelData.satellitesListRelatedDataList.elem =
                  ganss_sat_info.elem;
              }
              else
              {
                z_ganss_req_generic_data.ganssNavigationModelData.m.satellitesListRelatedDataListPresent = 0;
                z_ganss_req_generic_data.ganssNavigationModelData.ganssWeek = 0;
                z_ganss_req_generic_data.ganssNavigationModelData.ganssToe = 0;
                z_ganss_req_generic_data.ganssNavigationModelData.t_toeLimit = 0;
                z_ganss_req_generic_data.ganssNavigationModelData.satellitesListRelatedDataList.n = 0;
                z_ganss_req_generic_data.ganssNavigationModelData.satellitesListRelatedDataList.elem = NULL;
              }
#endif /* FEATURE_CGPS_SUPL_VARIANT_NO_TARGETED_EPH */

              /* 3GPP TS 44031-820.doc Table A.49.2
                 Model 4 - Satellite Navigation Model Using GLONASS Earth-Centered, Earth-fixed Parameters
              */
              z_ganss_req_generic_data.m.ganssAdditionalDataChoicesPresent = 1;
              z_ganss_req_generic_data.ganssAdditionalDataChoices.m.orbitModelIDPresent = 1;
              z_ganss_req_generic_data.ganssAdditionalDataChoices.orbitModelID = 3;
            }

            if ( p_supl_session_status->z_GloAssistData.u_Octet3_Whishlist_AH &  C_G0931_GPS_ASSIST_ALM )
            {
              z_ganss_req_generic_data.ganssAlmanac = TRUE;

              /* 3GPP TS 44031-820.doc Table A.54
                 Model 5 - GANSS Almanac Model Using GLONASS Keplerian Parameters
              */
              z_ganss_req_generic_data.m.ganssAdditionalDataChoicesPresent = 1;
              z_ganss_req_generic_data.ganssAdditionalDataChoices.m.almanacModelIDPresent = 1;
              z_ganss_req_generic_data.ganssAdditionalDataChoices.almanacModelID = 4;
            }

            z_ganss_req_generic_data.ganssAdditionalDataChoices.extElem1.count = 0;

            if ( p_supl_session_status->z_GloAssistData.u_Octet3_Whishlist_AH & C_G0931_GPS_ASSIST_UTC )
            {
              z_ganss_req_generic_data.m.ganssAdditionalDataChoicesPresent = 1;
              z_ganss_req_generic_data.ganssAdditionalDataChoices.m.utcModelIDPresent = 1;
              z_ganss_req_generic_data.ganssAdditionalDataChoices.utcModelID = 1; /* from RRLP-: 1 for Model 3: Glonass */
            }

          if ( p_supl_session_status->z_GloAssistData.u_Octet4_Whishlist_IP & (C_G0931_GPS_ASSIST_RTI >> 8) )
            {
              z_ganss_req_generic_data.ganssRealTimeIntegrity = TRUE;
            }

            /** TBD: MGP/LM/TM interface has not DGPS yet. Implement it when available */

            z_ganss_req_generic_data.ganssId = C_SUPL2_GANSS_ID_GLONASS; /* 4: Glonass */
            z_ganss_req_generic_data.extElem1.count = 0;

            z_ganss_requested_generic_assistance_data_list.elem = &z_ganss_req_generic_data;
            z_ganss_requested_generic_assistance_data_list.n = 1;

            p_Suplposinit->requestedAssistData.ver2_RequestedAssistData_extension.ganssRequestedGenericAssistanceDataList =
              z_ganss_requested_generic_assistance_data_list;

            p_Suplposinit->requestedAssistData.ver2_RequestedAssistData_extension.m.ganssRequestedGenericAssistanceDataListPresent = TRUE;
        } /* H-SLP support GLO */
        }

    } /* closing if SET-Based */

    else if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED )
    {
      p_Suplposinit->m.requestedAssistDataPresent = 1;
      p_Suplposinit->requestedAssistData.referenceTimeRequested = TRUE;
      p_Suplposinit->requestedAssistData.acquisitionAssistanceRequested = TRUE;

      if ( tm_umts_up_supl_rrlp_assisted_glonass_enabled() && (p_supl_session_status->u_position_protocol != TM_POS_PRTL_IS801) )
      {
        // Don't request GLO AD if UE's Glonass capability is disabled or H-SLP indicated no GLONASS support in UI call flow
        if ( FALSE == tm_up_slp_support_glonass( p_supl_session_status ) )
        {
           MSG( MSG_SSID_GPSSM, MSG_LEGACY_MED, "No GLO AD requested. H-SLP not capable of glonass" );
        }
        else
        {
        p_Suplposinit->requestedAssistData.m.ver2_RequestedAssistData_extensionPresent = 1;
        z_ganss_req_generic_data.ganssId = C_SUPL2_GANSS_ID_GLONASS; /* 4: Glonass */
        z_ganss_req_generic_data.extElem1.count = 0;

        /* if AA model is request, also request aux. which has freqency info. */
        z_ganss_req_generic_data.ganssAuxiliaryInformation = TRUE;

        /* UTC model for converting GPS reference time to GLONASS time */
        p_Suplposinit->requestedAssistData.utcModelRequested = TRUE;

        /* GANSS Time Model better quality conversion of GPS reference time
           to GLONASS time; reference system is C_SUPL2_GANSS_ID_GLONASS,
           wanting to convert to GNSS system of GPS */
        z_ganss_req_generic_data.m.ganssTimeModelsPresent = TRUE;
        z_ganss_req_generic_data.ganssTimeModels.data[0] = C_GANSS_SYSTEM_TIME_MODEL_REQUESTED_GPS;  /* GPS */
        z_ganss_req_generic_data.ganssTimeModels.data[1] = 0x0;
        z_ganss_req_generic_data.ganssTimeModels.numbits = 16;

        z_ganss_req_generic_data.ganssReferenceMeasurementInfo = TRUE;

        z_ganss_requested_generic_assistance_data_list.elem = &z_ganss_req_generic_data;
        z_ganss_requested_generic_assistance_data_list.n = 1;

        p_Suplposinit->requestedAssistData.ver2_RequestedAssistData_extension.ganssRequestedGenericAssistanceDataList =
          z_ganss_requested_generic_assistance_data_list;

        p_Suplposinit->requestedAssistData.ver2_RequestedAssistData_extension.m.ganssRequestedGenericAssistanceDataListPresent = TRUE;
        } /* H-SLP support GLO */
      }
    }
    else if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_AUTO )
    {
/* ayt - leave it commented out */
      #if 0 /* QQQ: test server would generate error if SuplPosInit contains no request
               for assistance. This server can choose to ignore this request for reference position;

               When this test only code is removed, for now only test with ULTS
            */
      p_Suplposinit->m.requestedAssistDataPresent = 1;
      p_Suplposinit->requestedAssistData.referenceLocationRequested = TRUE;
      #endif
    }
    else if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_CELL_ID )
    {
      /* space keeper: no assistance needed for cell ID */
    }
    else
    {
      /* shouldn't get here */
      MSG_ERROR("SuplPosInit attempt aborted: gps_mode = %u", p_supl_session_status->u_agps_mode, 0, 0);
      return FALSE;
    }
  }/*RRLP is pos protocol*/


  if ( ( p_supl_session_status->z_pending_new_pos_to_network.u_valid == TRUE ) ||
       ( z_supl_cached_pos_for_net.u_valid == TRUE )
     )
  {

    MSG_MED("new/cached pos. in SUPL_POS_INIT: %u, %u",
             p_supl_session_status->z_pending_new_pos_to_network.u_valid,
             z_supl_cached_pos_for_net.u_valid, 0);

    if ( p_supl_session_status->z_pending_new_pos_to_network.u_valid == TRUE )
    {
      p_pos_suplposinit = &p_supl_session_status->z_pending_new_pos_to_network;
    } /* new GPS position obtained prior to data link is up */

    else
    {
      /* If no new GPS, but there is a cached position, report this position
         in SUPL_POS_INIT */
      p_pos_suplposinit = &z_supl_cached_pos_for_net;
    } /* Cached position available */

    supl_gad9_to_pos_est ( &p_pos_suplposinit->z_pos_info.z_pos,
                           &p_Suplposinit->position.positionEstimate);


    /* asn1 range check:

        PositionEstimate ::= SEQUENCE {
          latitudeSign  ENUMERATED {north, south},
          latitude      INTEGER(0..8388607),
          longitude     INTEGER(-8388608..8388607),
          uncertainty
            SEQUENCE {uncertaintySemiMajor  INTEGER(0..127),
                      uncertaintySemiMinor  INTEGER(0..127),
                      orientationMajorAxis  INTEGER(0..180)} OPTIONAL,
          -- angle in degree between major axis and North
          confidence    INTEGER(0..100) OPTIONAL,
          altitudeInfo  AltitudeInfo OPTIONAL,
          ...}

        AltitudeInfo ::= SEQUENCE {
          altitudeDirection  ENUMERATED {height, depth},
          altitude           INTEGER(0..32767),
          altUncertainty     INTEGER(0..127),
          ...} -- based on 3GPP TS 23.032

        Note that supl_gad9_to_pos_est() sets all
          supl_uncertainty_present | supl_confidence_present | supl_altitudeInfo_present

     */

    asn1_range_error = FALSE;

    if ( ( p_Suplposinit->position.positionEstimate.latitude > 8388607 ) ||
         ( p_Suplposinit->position.positionEstimate.longitude < -8388608 ) ||
         ( p_Suplposinit->position.positionEstimate.longitude > 8388607  )||
         ( p_Suplposinit->position.positionEstimate.uncertainty.uncertaintySemiMajor > 127 ) ||
         ( p_Suplposinit->position.positionEstimate.uncertainty.uncertaintySemiMinor > 127 ) ||
         ( p_Suplposinit->position.positionEstimate.uncertainty.orientationMajorAxis > 180 ) ||
         ( p_Suplposinit->position.positionEstimate.confidence > 100 ) ||
         ( p_Suplposinit->position.positionEstimate.altitudeInfo.altitude > 32767 ) ||
         ( p_Suplposinit->position.positionEstimate.altitudeInfo.altUncertainty > 127 )
       )
    {
      asn1_range_error = TRUE;
    }

    if ( !asn1_range_error )
    {
      p_Suplposinit->m.positionPresent = 1;

      /***** process velocity info. *************/
      if ( p_pos_suplposinit->z_pos_info.vel_valid )
      {

        z_velocity.u.horandvervel = &z_Horandvervel; 

        p_Suplposinit->position.m.velocityPresent = 1;
        MSG_MED("Including velocity in SUPL_POS_INIT", 0, 0, 0);

        p_Suplposinit->position.velocity.t = C_SUPL_ASN1_CHOICE_VELCITY_HORANDVERVEL;
        p_Suplposinit->position.velocity = z_velocity;

        supl_convert_velocity(p_pos_suplposinit->z_pos_info.f_VelEnu[0],
                              p_pos_suplposinit->z_pos_info.f_VelEnu[1],
                              p_pos_suplposinit->z_pos_info.f_VelEnu[2], 
                              &z_velocity); 
        p_Suplposinit->position.velocity = z_velocity;

      }  /* velocity available */

      memset( timestamp, 0, 14 );
      z_utc_time =
        tm_util_construct_utc_time(p_pos_suplposinit->z_pos_info.w_GpsWeek,
          p_pos_suplposinit->z_pos_info.q_fix_time_gps_msec,
                                   p_pos_suplposinit->z_pos_info.q_fix_time_utc_msec);


      /* 2 digit year */
      z_utc_time.q_year = z_utc_time.q_year % 100;

      /* 10th digit year converted to ASCII */
      timestamp[0] = z_utc_time.q_year / 10 + 0x30;

      /* 1th digit year to ASCII */
      timestamp[1] = z_utc_time.q_year -
                     ( (uint32)( z_utc_time.q_year / 10 ) * 10  ) + 0x30;

      /* 10th digit month to ASCII */
      timestamp[2] = z_utc_time.q_month / 10 + 0x30;

      /* 1th digit month to ASCII */
      timestamp[3] = z_utc_time.q_month -
                     ( (uint32)( z_utc_time.q_month / 10 ) * 10 ) + 0x30;

      /* 10th digit day to ASCII */
      timestamp[4] = z_utc_time.q_utc_days / 10 + 0x30;

      /* 1th digit day to ASCII */
      timestamp[5] = z_utc_time.q_utc_days -
                     ( (uint32)( z_utc_time.q_utc_days / 10 ) * 10 ) + 0x30;

      /* 10th digit hour to ASCII */
      timestamp[6] = z_utc_time.q_hours / 10 + 0x30;

      /* 1th digit hour to ASCII */
      timestamp[7] = z_utc_time.q_hours -
                    ( (uint32)( z_utc_time.q_hours / 10 ) * 10 ) + 0x30;

      /* 10th digit minute to ASCII */
      timestamp[8] = z_utc_time.q_mins / 10 + 0x30;

      /* 1th digit minute to ASCII */
      timestamp[9] = z_utc_time.q_mins -
                     ( (uint32)( z_utc_time.q_mins / 10 ) * 10 ) + 0x30;

      /* 10th digit second to ASCII */
      timestamp[10] = z_utc_time.q_secs / 10 + 0x30;

      /* 1th digit second to ASCII */
      timestamp[11] = z_utc_time.q_secs -
                      ( (uint32)( z_utc_time.q_secs / 10 ) * 10 ) + 0x30;

      timestamp[12]='Z';

      p_Suplposinit->position.timestamp = &timestamp[0];
      MSG_MED("timestamp in SUPL_POS_INIT %s", timestamp, 0, 0);

    } /* if no asn1 range error */
    else
    {
      MSG_HIGH("Saved new pos. has out of ASN1 range elememt, ignored", 0, 0, 0);
    }



  } /* if either a new position has been obtained in this session,
       or a cached pos. available */
  #if defined (FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
  if ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP)
  {
    /* encode the SUPL POS within SUPLPOSINIT message with LPP payload*/
    b_ret = supl_enc_lpp_suplpos_suplposinit(p_supl_session_status, p_Suplposinit, &z_lpp_payload );

    if ( b_ret == TRUE)
    {
      p_Suplposinit->m.sUPLPOSPresent = 1;
      p_Suplposinit->sUPLPOS.m.velocityPresent = 0;
    }
    else
    {
      /* todo = if encoding fails cannot send supl pos init*/
      return u_ret;
    }
  }
  #endif /* FEATURE_CGPS_LTE_CELLDB && !FEATURE_GNSS_LOW_MEMORY */

  if ( ( u_callflow_type == C_SUPL_SESSION_NI ) &&
       p_supl_session_status->u_hash_valid
     )
  {
    p_Suplposinit->m.verPresent = 1;
    p_Suplposinit->ver.numbits = 64;
    memscpy( &p_Suplposinit->ver.data[0], sizeof(p_Suplposinit->ver.data),
         &p_supl_session_status->supl_init_hash[0], sizeof(p_supl_session_status->supl_init_hash));
  }


  encode_status = tm_osys_asn1_encode_pdu((void *)&pdu, &pdu_buff, PROTO_SUPL);

  if ( (encode_status != 0) || (pdu_buff.value == NULL ) || (pdu_buff.length == 0) )
  {
    /* ASN.1 encoding failed */
    if (pdu_buff.value != NULL )
    {
      if ( (p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP) &&
          (p_Suplposinit->m.sUPLPOSPresent == 1))
      {
        #if defined (FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
        if(p_Suplposinit->sUPLPOS.posPayLoad.u.ver2_PosPayLoad_extension)
        {
          tm_lpp_asn1_osys_lpppayload_free(&p_Suplposinit->sUPLPOS.posPayLoad.u.ver2_PosPayLoad_extension->lPPPayload);
        }
        #endif /* FEATURE_CGPS_LTE_CELLDB && !FEATURE_GNSS_LOW_MEMORY */
      }
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SUPL ASN1 enc. err. Freeing: %d,%lx,%u",
            encode_status, pdu_buff.value, pdu_buff.length);

      tm_asn1_free_osys_buf( pdu_buff.value, PROTO_SUPL );
      pdu_buff.value = NULL;

    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SUPL ASN1 enc. err. No alloc.: %d,%lx,%u",
            encode_status, pdu_buff.value, pdu_buff.length);
    }

    if ( p_cell_info_allocated_mlid != NULL )
    {
      os_MemFree((void**)&p_cell_info_allocated_mlid);
      p_cell_info_allocated_mlid = NULL;
    }
    if ( p_LocationId != NULL )
    {
      os_MemFree((void**)&p_LocationId);
      p_LocationId = NULL;
    }
    if ( p_LocationIdData != NULL)
    {
      p_LocationIdData->locationId = NULL;
      os_MemFree((void**)&p_LocationIdData);
      p_LocationIdData = NULL;
    }

    return FALSE;
  }

  else
  {
    pd_comms_return_type ret_val;
    p_length = (uint8 *)pdu_buff.value;
    u_length_msb = (pdu_buff.length >> 8) & 0x00FF;
    u_length_lsb = pdu_buff.length & 0x00FF;

    *p_length++ = u_length_msb;
    *p_length = u_length_lsb;

    ret_val = tm_umts_up_supl_comm_write( (uint8 *)pdu_buff.value, pdu_buff.length);

    if ( ( ret_val != PDCOMM_RESULT_WAIT ) && ( ret_val != PDCOMM_RESULT_OK ) )
    {
      /* message sending failed */
      MSG_ERROR("SUPL: SuplPosInit attempt aborted", 0, 0, 0);
    }
    else
    {
      supl_set_cf_state(C_SUPL_CF_STATE_UT2, 0);
      u_ret = TRUE;
    }

    if ( (p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP) &&
        (p_Suplposinit->m.sUPLPOSPresent == 1))
    {
      #if defined (FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
      if(p_Suplposinit->sUPLPOS.posPayLoad.u.ver2_PosPayLoad_extension)
      {
        tm_lpp_asn1_osys_lpppayload_free(&p_Suplposinit->sUPLPOS.posPayLoad.u.ver2_PosPayLoad_extension->lPPPayload);
      }
      #endif /* FEATURE_CGPS_LTE_CELLDB && !FEATURE_GNSS_LOW_MEMORY */
    }
    /*Free the encoded PDU*/
    tm_asn1_free_osys_buf( pdu_buff.value, PROTO_SUPL );
    pdu_buff.value = NULL;

  }


  if ( u_ret )
  {
    p_supl_session_status->u_send_supl_end_at_fail = TRUE;
    MSG_MED("SUPL: SuplPosInit sent ", 0, 0, 0);
  }


  /* after SUPL_POS_INIT, no need to acquire cell meas. for MRL anymore */
  (void) os_TimerStop(l1_mrl_req_timer);

  if ( p_cell_info_allocated_mlid != NULL )
  {
    os_MemFree((void**)&p_cell_info_allocated_mlid);
    p_cell_info_allocated_mlid = NULL;
  }
  if ( p_LocationId != NULL )
  {
    os_MemFree((void**)&p_LocationId);
    p_LocationId = NULL;
  }
  if ( p_LocationIdData != NULL)
  {
    p_LocationIdData->locationId = NULL;
    os_MemFree((void**)&p_LocationIdData);
    p_LocationIdData = NULL;
  }

  return u_ret;

}

/*
******************************************************************************
* supl_SuplPos_proc
*
* Function description:
*
* Process incoming SUPL POS message
*
* Parameters: p_pdu_decoded: pointer to a structure holding decoded Supl PDU.
*
* Return value: None
*
******************************************************************************
*/
static boolean supl_SuplPos_proc( const supl_ULP_PDU* p_pdu_decoded )
{
  uint8* p_payload = NULL;
  uint16 payload_length = 0;
  const supl_SUPLPOS* p_suplPos;
  supl_status_s_type* p_supl_session_status = &z_supl_session_status;
  boolean u_ret = TRUE;

  MSG_MED("LSM: process_supl_pos()", 0, 0, 0);

  p_suplPos = p_pdu_decoded->message.u.msSUPLPOS;


  if (  /*If not in UT2 (used only in RRLP case), UT3_RRLP or UT3 Ack*/
                ( !(p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT2) &&
                 (p_supl_session_status->u_position_protocol == TM_POS_PRTL_RRLP)
              ) &&
        !( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT3_RRLP ) &&
        !( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT3_ACK )
     )
  {
    (void) tm_umts_up_supl_supl_end_tx(&p_supl_session_status->z_session_id.z_Slp_Sess_Id,
                                NULL,
                                supl_StatusCode_unexpectedMessage,
                                NULL,
                                p_supl_session_status->q_supl_version);
    tm_umts_up_supl_callflow_disrupt_handler();
    return FALSE;
  }

  if ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT2 )
  {
    tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_UT2);
  }
  else if ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT3_RRLP )
  {
    tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_UT3_RRLP);
  }
  else if ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT3_ACK )
  {
    tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_UT3_ACK);
  }


  switch(p_suplPos->posPayLoad.t)
  {
    case C_SUPL_ASN1_CHOICE_RRLP_PAYLOAD:
      if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_RRLP)
      {
        p_payload = &p_suplPos->posPayLoad.u.rrlpPayload->data[0];
        payload_length = p_suplPos->posPayLoad.u.rrlpPayload->numocts;
        u_ret = supl_SuplPos_rrlp_proc(p_payload, payload_length);
      }
      break;

    case C_SUPL_ASN1_CHOICE_TIA801_PAYLOAD:
      if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801)
      {
        p_payload = &p_suplPos->posPayLoad.u.tia801payload->data[0];
        payload_length = p_suplPos->posPayLoad.u.tia801payload->numocts;
        u_ret = supl_SuplPos_is801_proc(p_payload, payload_length);
      }
      break;

    case C_SUPL_ASN1_CHOICE_RRC_PAYLOAD:
      (void) tm_umts_up_supl_supl_end_tx(&p_supl_session_status->z_session_id.z_Slp_Sess_Id,
                                  NULL,
                                  supl_posProtocolMismatch,
                                  NULL,
                                  p_supl_session_status->q_supl_version);
      tm_umts_up_supl_callflow_disrupt_handler();
      break;

    #if defined (FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
    case C_SUPL_ASN1_CHOICE_EXT_PAYLOAD:
      if ( p_suplPos->posPayLoad.u.ver2_PosPayLoad_extension->m.lPPPayloadPresent )
      {
        if ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP )
        {
          u_ret = supl_SuplPos_lpp_proc( &p_suplPos->posPayLoad.u.ver2_PosPayLoad_extension->lPPPayload);

        }
      }
      break;
    #endif /* FEATURE_CGPS_LTE_CELLDB && !FEATURE_GNSS_LOW_MEMORY */
  }

  return u_ret;
}


/*
******************************************************************************
* supl_SuplPos_rrlp_proc
*
* Function description:
*
* Process incoming SUPL POS message containing RRLP payload
*
* Parameters: p_payload: pointer to SUPL POS RRLP payload
*                        payload_length: Length of the payload
*
* Return value: TRUE or FALSE
*
******************************************************************************
*/
static boolean supl_SuplPos_rrlp_proc(uint8 *p_payload, const uint16 payload_length)
{

  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;
  int oss_decode_status = 0xFFFFFFFF;
  boolean u_ret = TRUE;


  if ( supl_rrlp_decode_proc (&p_supl_session_status->z_network_assist, p_payload, payload_length, &oss_decode_status )  )
  {


    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED,
          "RRLP choice of %u decode/proc ok",
          p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.u_rrlp_choice);

    /* check if a SUPL_POS-RRLP-ACK is needed to send back to the SLP */
    if ( p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.u_rrlp_choice
         == C_RRLP_CHOICE_ASSISTDATA
       )
    {
      if ( !supl_SuplPos_RRLP_ACK_tx() )
      {
        u_ret = FALSE;
      }
    }


    if ( ! ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT3_RRLP ) )
    {

      /* No more assistance from the SLP; tell TmCore to send pdapi event of
         EventGpsPdConnectionDone */
      tm_umts_common_send_pdapi_comm_event
      (  PDSM_PD_COMM_PROTOCOL_UMTS_UP_SUPL,
         C_COMM_DONE,
         p_supl_session_status->tm_active_session_handle,
         p_supl_session_status->q_pdapi_client_id
      );


      /* If there is a final position pending to be sent to the network, and
         this involves RRLP-msrPosReq, report the saved position to SLP now */
      if ( ( p_supl_session_status->z_pending_new_pos_to_network.u_valid == TRUE ) &&
           ( p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.u_final == TRUE )
         )
      {
        if ( p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.u_rrlp_choice ==
             C_RRLP_CHOICE_MSRPOSREQ )
        {

          if ( p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.vel_valid )
          {
            supl_Velocity z_velocity;

            supl_Horandvervel z_horandvervel;

            memset(&z_velocity, 0, sizeof(supl_Velocity));
            memset(&z_horandvervel, 0, sizeof(supl_Horandvervel));
            z_velocity.u.horandvervel = &z_horandvervel;

            supl_convert_velocity ( p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.f_VelEnu[0],
                                    p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.f_VelEnu[1],
                                    p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.f_VelEnu[2],
                                    &z_velocity);

            /* send the pending position out to the network */
            supl_send_pos_rrlp_to_slp
              ( &p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.z_pos,
                &z_velocity,
                p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.q_fix_time_gps_msec % C_FOUR_HRS_MSEC,
                p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.q_pos_sources,
                p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.q_glonass_tod_modulo_1hour_ms);
          }

          else  /* no velocity info */
          {
            /* send the pending position out to the network */
            supl_send_pos_rrlp_to_slp
              ( &p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.z_pos,
                NULL,
                p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.q_fix_time_gps_msec % C_FOUR_HRS_MSEC,
                p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.q_pos_sources,
                p_supl_session_status->z_pending_new_pos_to_network.z_pos_info.q_glonass_tod_modulo_1hour_ms);
          }
        }

      }

      if ( p_supl_session_status->u_agps_mode != C_SUPL_AGPS_MODE_UE_ASSISTED )
      {
        /* Determine if it has enough info. about some SVs' nonexistence and
           inform MGP about such info., if available */
        tm_util_post_non_exist_svs ( TM_PRTL_TYPE_UMTS_UP,
          p_supl_session_status->tm_active_session_handle,
          p_supl_session_status->q_alm_delivered_sv_list,
          p_supl_session_status->q_eph_delivered_sv_list );

        /* inform TM core to start session timer if not SET-Assisted; Note for
           SET-Assisted, this is done after steering has been injected */
        tm_umts_up_supl_inform_core_nt_assist_end();
      }
    }

    return TRUE;

  } /* close if-decoded-ok */

  else
  {

    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED,
          "RRLP choice of %u decode/proc encountered error",
          p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.u_rrlp_choice);

    u_ret = FALSE;

    #if 0 /* ayt - ignore this block */
    /* send appropriate RRLP protocol error */

    if ( oss_decode_status == OSS_DECODE_ERR_MORE_INPUT )
    {
      rrlp_protocol_reason = rrlp_messageTooShort;
    }
    else if ( oss_decode_status == OSS_DECODE_ERR_DATA_ERROR )
    {
      rrlp_protocol_reason = rrlp_incorrectData;
    }
    else
    {
      rrlp_protocol_reason = rrlp_ErrorCodes_unDefined;
    }

    // lsm_rrlp_encode_protocolErr (p_LsmControl, rrlp_protocol_reason, (int8)( (*p_suplPos->posPayLoad.u.rrlpPayload.value)>>5) );
    /* this function will set state to UT3_RRLP */

    #endif

  }

  return u_ret;
}


/*
******************************************************************************
* supl_SuplPos_is801_proc
*
* Function description:
*
* Process incoming SUPL POS message containing IS801 payload
*
* Parameters: p_payload: pointer to SUPL POS IS801 payload
*                        payload_length: Length of the payload
*
* Return value: TRUE or FALSE
*
******************************************************************************
*/
static boolean supl_SuplPos_is801_proc(uint8 *p_payload, const uint16 payload_length)
{

  byte bytes_consumed_801_1;

  /* Forward it to IS801 for decoding */
  return is801_decode_fwd_link_pddm(TM_PRTL_TYPE_1X_UP,
                                    p_payload,
                                    payload_length,
                                    &bytes_consumed_801_1);
}

/*
******************************************************************************
* supl_SuplPos_RRLP_ACK_tx
*
* Function description:
*
* Encode and transmit SUPL POS carrying RRLP-ACK
*
* Parameters: NONE.
*
* Return value: TRUE or FALSE
*
******************************************************************************
*/
static boolean supl_SuplPos_RRLP_ACK_tx (void)
{
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;

  boolean u_ret = FALSE;

  tm_pdu_buf_type   pdu_buff; /* including length and pointer to data */

  rrlp_PDU z_rrlp;

  int encode_status;

  /* Initialize OSS rrlp structure */
  memset( &z_rrlp, 0, sizeof( rrlp_PDU ));

  memset(&pdu_buff, 0, sizeof(tm_pdu_buf_type));

  /* ref. num: the same as the one came from SMLC */
  z_rrlp.referenceNumber =
    p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.b_RefNum;

  z_rrlp.component.t = rrlp_assistanceDataAck_chosen;

  /* Do the ASN.1 encoding for RRLP */
  encode_status = tm_osys_asn1_encode_pdu((void *)&z_rrlp, &pdu_buff, PROTO_RRLP);

  if ( (encode_status != 0) || (pdu_buff.value == NULL ) || (pdu_buff.length == 0) )
  {
    /* ASN.1 encoding failed */
    if (pdu_buff.value != NULL )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRLP ASN1 enc. err. Freeing: %d,%lx,%u",
            encode_status, pdu_buff.value, pdu_buff.length);

      tm_asn1_free_osys_buf( pdu_buff.value, PROTO_RRLP );
      pdu_buff.value = NULL;

    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRLP ASN1 enc. err. No alloc.: %d,%lx,%u",
            encode_status, pdu_buff.value, pdu_buff.length);
    }
    return FALSE;
  }

  if ( supl_SuplPos_rrlp_tx (pdu_buff.value, (uint16)pdu_buff.length, NULL) )
  {
    if ( ( p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_NI ) &&
         ( p_supl_session_status->u_current_is_for_area_event == FALSE )
       )
    {
      /* supl_SuplPos_rrlp_tx() only sets timer to UT3_RRLP if the incoming
         RRLP assist. data contains "more_to_come"; but if NI, it should enter
         this state regardless */
      supl_set_cf_state(C_SUPL_CF_STATE_UT3_RRLP, 0);
    }
    else if (  p_supl_session_status->u_current_is_for_area_event == TRUE )
    {
      if ( p_supl_session_status->z_network_assist.u_MoreToCome == TRUE )
      {
        supl_set_cf_state(C_SUPL_CF_STATE_UT3_RRLP, 0);
      }
      else
      {
        supl_set_cf_state(C_SUPL_CF_STATE_UT3_EXPECT_SUPLREPORT, 0);
      }
    }
    u_ret = TRUE;
  }
  else
  {
    tm_umts_up_supl_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
    tm_umts_up_supl_session_init(FALSE);
    u_ret = FALSE;
  }

  /*Free the encoded PDU*/
  tm_asn1_free_osys_buf( pdu_buff.value, PROTO_RRLP );
  pdu_buff.value = NULL;

  return u_ret;
}



/*
******************************************************************************
* supl_SuplPos_RRLP_protocol_err_tx
*
* Function description:
*
* Encode and transmit SUPL POS carrying RRLP-protocolError
*
* Parameters: NONE.
*
* Return value: TRUE or FALSE
*
******************************************************************************
*/

static boolean supl_SuplPos_RRLP_protocol_err_tx (int8 b_RefNum, rrlp_ErrorCodes e_err_code )
{

  boolean u_ret = FALSE;
  tm_pdu_buf_type   pdu_buff; /* including length and pointer to data */
  PDU z_rrlp;
  rrlp_ProtocolError z_protocol_error;
  rrlp_ProtocolError *p_prot_err ;
  int encode_status;


  memset(&z_rrlp, 0, sizeof(rrlp_PDU) );
  memset(&z_protocol_error, 0, sizeof(rrlp_ProtocolError) );
  memset(&pdu_buff, 0, sizeof(tm_pdu_buf_type));


  z_rrlp.referenceNumber = b_RefNum;
  z_rrlp.component.t = rrlp_protocolError_chosen;

  z_rrlp.component.u.protocolError = &z_protocol_error;
  p_prot_err = z_rrlp.component.u.protocolError;

  p_prot_err->m.extensionContainerPresent = 0;
  p_prot_err->m.rel_5_ProtocolError_ExtensionPresent = 0;
  p_prot_err->errorCause = e_err_code;


  /* Do the ASN.1 encoding for RRLP */

  encode_status = tm_osys_asn1_encode_pdu((void *)&z_rrlp, &pdu_buff, PROTO_RRLP);

  if ( (encode_status != 0) || (pdu_buff.value == NULL ) || (pdu_buff.length == 0) )
  {
    /* ASN.1 encoding failed */
    if (pdu_buff.value != NULL )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRLP ASN1 enc. err. Freeing: %d,%lx,%u",
            encode_status, pdu_buff.value, pdu_buff.length);

      tm_asn1_free_osys_buf( pdu_buff.value, PROTO_RRLP );
      pdu_buff.value = NULL;

    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRLP ASN1 enc. err. No alloc.: %d,%lx,%u",
            encode_status, pdu_buff.value, pdu_buff.length);
    }
    return FALSE;
  }


  if ( supl_SuplPos_rrlp_tx (pdu_buff.value, (uint16)pdu_buff.length, NULL) )
  {
    supl_set_cf_state(C_SUPL_CF_STATE_UT3_RRLP, 0);
    u_ret = TRUE;
  }
  else
  {
    tm_umts_up_supl_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
    tm_umts_up_supl_session_init(FALSE);
    u_ret = FALSE;
  }

  /*Free the encoded PDU*/
  tm_asn1_free_osys_buf( pdu_buff.value, PROTO_RRLP );
  pdu_buff.value = NULL;

  return u_ret;

}




/*
******************************************************************************
* static boolean supl_SuplPos_rrlp_tx
*
* Function description:
*
* Encode and transmit SUPL POS with RRLP payload.
* measurements.
*
* Parameters: p_rrlp_data - pointer to rrlp payload;
*             w_byte_cnt - num. of bytes in rrlp payload;
*             p_velocity - pointer the struture holding velocity (null if not available )
*
* Return value: TRUE or FALSE
*
******************************************************************************
*/
static boolean supl_SuplPos_rrlp_tx (uint8* p_rrlp_data, uint16 w_byte_cnt, supl_Velocity *p_velocity)
{
  supl_status_s_type*        p_supl_session_status = &z_supl_session_status;

  boolean u_ret = FALSE;


  tm_pdu_buf_type   pdu_buff; /* including length and pointer to data */
  supl_ULP_PDU      pdu;
  supl_SUPLPOS      z_SuplPos;
  supl_SUPLPOS*     p_SuplPos;

  uint8*           p_length;
  uint8            u_length_msb = 0;
  uint8            u_length_lsb = 0;

  supl_PosPayLoad_rrlpPayload z_rrlp_payload;

  int encode_status;


  memset(&pdu, 0x00, sizeof(supl_ULP_PDU));
  memset(&pdu_buff, 0, sizeof(tm_pdu_buf_type));
  memset(&z_SuplPos, 0x00, sizeof(supl_SUPLPOS) );
  memset(&z_rrlp_payload, 0x00, sizeof (supl_PosPayLoad_rrlpPayload) );

  pdu.message.t = T_supl_UlpMessage_msSUPLPOS;
  p_SuplPos = &z_SuplPos;
  pdu.message.u.msSUPLPOS = p_SuplPos;

  /* overhead build function has to be after we know what message is to be constructed */
  if ( !supl_header_build (&pdu, NULL, NULL, NULL, p_supl_session_status->q_supl_version) )
  {
    MSG_ERROR("SUPL: SuplPosRrlp attempt aborted: header", 0, 0, 0);
    return FALSE;
  }

  #ifndef FEATURE_CGPS_SUPL_VARIANT_SUPL_POS_NO_VEL /* NOT by default defined */
  if ( p_velocity == NULL )
  {
    p_SuplPos->m.velocityPresent = 0; /* no optional IE of velocity included */
  }
  else
  {
    p_SuplPos->m.velocityPresent = 1;
    p_SuplPos->velocity = *p_velocity;
  }
  #endif

  if (w_byte_cnt > 8192 ) return FALSE;
  z_rrlp_payload.numocts = w_byte_cnt;
  memscpy( z_rrlp_payload.data, sizeof(z_rrlp_payload.data), p_rrlp_data, w_byte_cnt);

  p_SuplPos->posPayLoad.t = C_SUPL_ASN1_CHOICE_RRLP_PAYLOAD;
  p_SuplPos->posPayLoad.u.rrlpPayload = &z_rrlp_payload;

  /* If encoding failed, pdu_buf.value will
  be NULL and pdu_buf.length is meaningless. If encoding succeeded,
  pdu_buf.value will be non-NULL and pdu_buf.length will contain the number
  of bytes in the encoded PDU. */


  encode_status = tm_osys_asn1_encode_pdu((void *)&pdu, &pdu_buff, PROTO_SUPL);

  if ( (encode_status != 0) || (pdu_buff.value == NULL ) || (pdu_buff.length == 0) )
  {
    /* ASN.1 encoding failed */
    if (pdu_buff.value != NULL )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SUPL ASN1 enc. err. Freeing: %d,%lx,%u",
            encode_status, pdu_buff.value, pdu_buff.length);

      tm_asn1_free_osys_buf( pdu_buff.value, PROTO_SUPL );
      pdu_buff.value = NULL;

    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SUPL ASN1 enc. err. No alloc.: %d,%lx,%u",
            encode_status, pdu_buff.value, pdu_buff.length);
    }
    return FALSE;
  }

  else
  {

    pd_comms_return_type ret_val;

    p_length = (uint8 *)pdu_buff.value;
    u_length_msb = (pdu_buff.length >> 8) & 0x00FF;
    u_length_lsb = pdu_buff.length & 0x00FF;

    *p_length++ = u_length_msb;
    *p_length = u_length_lsb;

    ret_val = tm_umts_up_supl_comm_write( (uint8 *)pdu_buff.value, pdu_buff.length);

    if ( ( ret_val != PDCOMM_RESULT_WAIT ) && ( ret_val != PDCOMM_RESULT_OK ) )
    {
      /* message sending failed */
      MSG_ERROR("SUPL: SuplPosRrlp attempt aborted", 0, 0, 0);
    }
    else if ( p_supl_session_status->u_current_is_for_area_event == FALSE )
    {
      if ( ( p_supl_session_status->z_network_assist.u_MoreToCome == TRUE ) ||
           ( ( p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_NI ) &&
             ( p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.u_rrlp_choice
                == C_RRLP_CHOICE_ASSISTDATA )
           )
         )
      {
        supl_set_cf_state(C_SUPL_CF_STATE_UT3_RRLP, 0);
      }
      else if ( p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.u_rrlp_choice
                == C_RRLP_CHOICE_ASSISTDATA )
      {
        supl_set_cf_state(C_SUPL_CF_STATE_UT3_ACK, 0);
      }
      else
      {
        supl_set_cf_state(C_SUPL_CF_STATE_UT3, 0);
      }
      u_ret = TRUE;
    }
    else
    {
      u_ret = TRUE;
    }

    /*Free the encoded PDU*/
    tm_asn1_free_osys_buf( pdu_buff.value, PROTO_SUPL );
    pdu_buff.value = NULL;

  }

  if ( u_ret )
  {

    MSG_MED("SUPL: SuplPos sent ", 0, 0, 0);
  }


  return u_ret;
}

/*
******************************************************************************
* static boolean tm_umts_up_supl_SuplPos_is801_tx
*
* Function description:
*
* Encode and transmit SUPL POS with IS801 payload.
* measurements.
*
* Parameters: p_rrlp_data - pointer to rrlp payload;
*             w_byte_cnt - num. of bytes in rrlp payload;
*
* Return value: TRUE or FALSE
*
******************************************************************************
*/
boolean tm_umts_up_supl_SuplPos_is801_tx  (uint8 *p_is801_data, const uint16 w_byte_cnt)
{

  #if defined (FEATURE_CGPS_USES_CDMA) && !defined (FEATURE_GNSS_LOW_MEMORY)

  supl_status_s_type*        p_supl_session_status = &z_supl_session_status;

  boolean u_ret = FALSE;

  tm_pdu_buf_type   pdu_buff; /* including length and pointer to data */
  supl_ULP_PDU      pdu;

  supl_SUPLPOS      z_SuplPos;
  supl_SUPLPOS*     p_SuplPos;

  uint8*           p_length;
  uint8            u_length_msb = 0;
  uint8            u_length_lsb = 0;


  supl_PosPayLoad_tia801payload z_is801_payload;
  int encode_status;

  memset(&pdu, 0x00, sizeof(supl_ULP_PDU));
  memset(&pdu_buff, 0, sizeof(tm_pdu_buf_type));
  memset(&z_SuplPos, 0x00, sizeof(supl_SUPLPOS) );
  memset(&z_is801_payload, 0x00, sizeof (supl_PosPayLoad_tia801payload) );

  pdu.message.t = T_supl_UlpMessage_msSUPLPOS;
  p_SuplPos = &z_SuplPos;
  pdu.message.u.msSUPLPOS = p_SuplPos;


  /* overhead build function has to be after we know what message is to be constructed */
  if ( !supl_header_build (&pdu, NULL, NULL, NULL, p_supl_session_status->q_supl_version) )
  {
    MSG_ERROR("SUPL: SuplPosIs801 attempt aborted: header", 0, 0, 0);
    return FALSE;
  }

#ifndef FEATURE_CGPS_SUPL_VARIANT_SUPL_POS_NO_VEL /* NOT by default defined */
  p_SuplPos->m.velocityPresent = 0; /* no optional IE of velocity included */
#endif

  if (w_byte_cnt > 8192 ) return FALSE;
  z_is801_payload.numocts =  w_byte_cnt;
  memscpy(z_is801_payload.data, sizeof(z_is801_payload.data), p_is801_data, w_byte_cnt);
  p_SuplPos->posPayLoad.t = C_SUPL_ASN1_CHOICE_TIA801_PAYLOAD;
  p_SuplPos->posPayLoad.u.tia801payload = &z_is801_payload;


  encode_status = tm_osys_asn1_encode_pdu((void *)&pdu, &pdu_buff, PROTO_SUPL);

  if ( (encode_status != 0) || (pdu_buff.value == NULL ) || (pdu_buff.length == 0) )
  {
    /* ASN.1 encoding failed */
    if (pdu_buff.value != NULL )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SUPL ASN1 enc. err. Freeing: %d,%lx,%u",
            encode_status, pdu_buff.value, pdu_buff.length);

      tm_asn1_free_osys_buf( pdu_buff.value, PROTO_SUPL );
      pdu_buff.value = NULL;

    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SUPL ASN1 enc. err. No alloc.: %d,%lx,%u",
            encode_status, pdu_buff.value, pdu_buff.length);
    }
    return FALSE;
  }

  else
  {
    pd_comms_return_type ret_val;

    p_length = (uint8 *)pdu_buff.value;
    u_length_msb = (pdu_buff.length >> 8) & 0x00FF;
    u_length_lsb = pdu_buff.length & 0x00FF;

    *p_length++ = u_length_msb;
    *p_length = u_length_lsb;

    ret_val = tm_umts_up_supl_comm_write( (uint8 *)pdu_buff.value, pdu_buff.length);

    if ( ( ret_val != PDCOMM_RESULT_WAIT ) && ( ret_val != PDCOMM_RESULT_OK ) )
    {
      /* message sending failed */
      MSG_ERROR("SUPL: SuplPosIs801 attempt aborted", 0, 0, 0);
    }
    else
    {
      u_ret = TRUE;
    }


    /*Free the encoded PDU*/
    tm_asn1_free_osys_buf( pdu_buff.value, PROTO_SUPL );
    pdu_buff.value = NULL;

  }

  if ( u_ret )
  {

    MSG_MED("SUPL: SuplPos sent ", 0, 0, 0);
  }


  return u_ret;

  #else
  return FALSE;  /* dummy for builds without 1X */
  #endif /* FEATURE_CGPS_USES_CDMA && ! FEATURE_GNSS_LOW_MEMORY */

}

/*
******************************************************************************
* supl_SuplEnd_proc
*
* Function description:
*
* Process incoming SUPL POS message
*
* Parameters: p_pdu_decoded: pointer to a structure holding decoded Supl PDU.
*
* Return value: None
*
******************************************************************************
*/

static void supl_SuplEnd_proc ( const supl_ULP_PDU* p_pdu_decoded )
{
  supl_status_s_type*      p_supl_session_status = &z_supl_session_status;
  pdsm_pd_info_s_type*     p_pd_info_buffer = &z_supl_pd_info_buffer;
  pdsm_pd_event_type       pd_event = PDSM_PD_EVENT_POSITION;

  tm_post_data_payload_type   z_tm_payload;
  const supl_SUPLEND* p_suplEnd;
  uint8 u_area_event_session_match_num = 0xFF;
  gnss_NavSlnFlagsStructType z_pos_flags;

  tm_cm_phone_state_info_s_type phone_state_info;
  pdsm_pd_comm_protocol_e_type protocol_type = PDSM_PD_COMM_PROTOCOL_UMTS_UP_SUPL;

  tm_cm_iface_get_phone_state(&phone_state_info);

  memset(&z_pos_flags, 0x00, sizeof(z_pos_flags));

  p_suplEnd = p_pdu_decoded->message.u.msSUPLEND;

  MSG_MED("process_supl_end()at CF state %x", p_supl_session_status->e_cf_state, 0, 0);

  /* no need to maintain the data connection anymore */
  tm_umts_up_supl_comm_close_connection();

  /* the network session is considered terminated */
  p_supl_session_status->u_network_session_on = FALSE;

   p_supl_session_status->u_send_supl_end_at_fail = FALSE;

  if ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT10_INFO_QUERY_END )
  {
    tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_UT10_INFO_QUERY_END );
    supl_session_status_reset();
    return; /* no further action needed */
  }


  if ( ( u_area_event_session_match_num =
         supl_area_event_session_match ( &p_pdu_decoded->sessionID )
       ) < C_MAX_GEOFENCE_NUM
     )
  {
    /* Session ID matches an existing areaEvent session */
    MSG_MED("SUPL_END from SLP ending areaEven session %u",
            u_area_event_session_match_num, 0, 0);

    /* terminate all session timer associated with this area event session */
    tm_umts_up_supl_terminate_timer_cancel_cf_state_area_event
          (C_SUPL_CF_STATE_NULL, u_area_event_session_match_num);

    /* clear session variables associated with this area event session */
    memset( &z_supl_area_event_session_status[u_area_event_session_match_num], 0,
            sizeof(supl_area_event_status_s_type) );

  }


  /* if position is included, "seed injection" and "LR post" */
  if ( p_suplEnd->m.positionPresent )
  {
    memset(p_pd_info_buffer, 0, sizeof( pdsm_pd_info_s_type ) );
    p_pd_info_buffer->client_id = p_supl_session_status->q_pdapi_client_id;
    p_supl_session_status->pd_info_buffer_valid = TRUE;

    /* this function populates the local structure of z_tm_payload for seed
       position post, and also populates part of the global structure of
       z_supl_pd_info_buffer pdapi event reporting including the extended pos.
       part of the structure; some other part of z_supl_pd_info_buffer will be
       determined later */
    supl_convert_supl_end_pos_for_tm_core ( p_suplEnd, &z_tm_payload.seed_data );

    (void) tm_post_data( TM_PRTL_TYPE_UMTS_UP,
                  p_supl_session_status->tm_active_session_handle,
                  TM_POST_DATA_TYPE_LR,
                  &z_tm_payload );

  }

  /* if network initiated, SUPL protocol module's job is done */
  if ( p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_NI )
  {
    if ( !p_supl_session_status->u_pdapi_fix_reported )
    {
      /* if pdapi report has not been sent, such as in NI-MSA case, report
         this position to pdapi */
      if ( p_suplEnd->m.positionPresent )
      {

        p_pd_info_buffer->pd_info.pd_data.position_mode = PDSM_PD_POSITION_MODE_MSASSISTED;

        /* SUPL_END does not provide source info. */
        p_pd_info_buffer->pd_info.pd_data.positioning_source = PDSM_PD_POSITION_SOURCE_GPS | PDSM_PD_POSITION_SOURCE_GLO;

        /* set up proper position type in the pdapi report */
        p_pd_info_buffer->pd_info.pd_data.position_type = PDSM_PD_POSITION_TYPE_NEW;

        pd_event = PDSM_PD_EVENT_POSITION;

        if ( p_suplEnd->position.positionEstimate.m.altitudeInfoPresent )
        {
          pd_event |= PDSM_PD_EVENT_HEIGHT;
          z_pos_flags.b_Is3D = TRUE;
        }
        if (p_suplEnd->position.m.velocityPresent)
        {
          pd_event |= PDSM_PD_EVENT_VELOCITY;
          z_pos_flags.b_IsVelValid = TRUE;
        }

        (void) tm_post_pd_event ( TM_PRTL_TYPE_UMTS_UP,
                                  p_supl_session_status->tm_active_session_handle,
                                  pd_event,
                                  &z_supl_pd_info_buffer,
                                  &z_pos_flags);

      } /* if position present */
    }   /* if pdpai report has not occured */

    if ( p_suplEnd->m.statusCodePresent )
    {
      /* network treats SUPL call flow as failure*/
      tm_umts_up_supl_inform_core_stop_session(TM_STOP_REASON_USER_TERMINATED);
      tm_umts_up_supl_session_init(FALSE);
    }
    else
    {
      /* network treats SUPL call flow as pass*/
      tm_umts_up_supl_inform_core_stop_session(TM_STOP_REASON_COMPLETED);
      tm_umts_up_supl_session_init(FALSE);
    }
  }

  else if ( p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_UI )
  {

    if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_BASED )
    {

      if ( p_supl_session_status->u_pdapi_fix_reported )
      {
        /* For MO MSB, if position has already been reported to pdapi,
           and SUPL_END arrives, end the session */
        tm_umts_up_supl_inform_core_stop_session(TM_STOP_REASON_COMPLETED);
        tm_umts_up_supl_session_init(FALSE);
        return;
      }

      if ( p_suplEnd->m.positionPresent )
      {

        MSG_MED("SUPL End Carrying Position",0,0,0);

        p_pd_info_buffer->pd_info.pd_data.position_mode = PDSM_PD_POSITION_MODE_MSBASED;

        /* SUPL_END does not provide source info. */
        p_pd_info_buffer->pd_info.pd_data.positioning_source = PDSM_PD_POSITION_SOURCE_GPS | PDSM_PD_POSITION_SOURCE_GLO;

        /* set up proper position type in the pdapi report */
        p_pd_info_buffer->pd_info.pd_data.position_type = PDSM_PD_POSITION_TYPE_NEW;

        /* If this position, most likely cell-id based, does not satisfy
           qoP, continue as standalone and if in the end there is no better one, use
           this one;  if this one satisfies qoP, end the session */

        if ( (  p_suplEnd->position.positionEstimate.m.uncertaintyPresent &&
                ( tm_umts_common_utils_GADK_to_meter
                 ( TRUE, p_suplEnd->position.positionEstimate.uncertainty.uncertaintySemiMinor ) <
               p_supl_session_status->z_session_qos.q_hor_acc_meters
                )
             ) ||
             ( tm_umts_common_utils_meter_to_GADK
                 ( TRUE, p_supl_session_status->z_session_qos.q_hor_acc_meters ) == 127
             ) ||
             ( (p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP) &&
               (p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_RCVD)
             )
           )
        {
          pd_event = PDSM_PD_EVENT_POSITION;

          if ( p_suplEnd->position.positionEstimate.m.altitudeInfoPresent )
          {
            z_pos_flags.b_Is3D = TRUE;
            pd_event |= PDSM_PD_EVENT_HEIGHT;
          }
          if (p_suplEnd->position.m.velocityPresent)
          {
            z_pos_flags.b_IsVelValid = TRUE;
            pd_event |= PDSM_PD_EVENT_VELOCITY;
          }

          (void) tm_post_pd_event ( TM_PRTL_TYPE_UMTS_UP,
                                    p_supl_session_status->tm_active_session_handle,
                                    pd_event,
                                    &z_supl_pd_info_buffer,
                                    &z_pos_flags);

          p_supl_session_status->u_pdapi_fix_reported = TRUE;

        } /* if the position in the message passes qoP */

      } /* if Position info. is in the message */

      if( ((p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801) ||
          (p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP))&&
          (p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT3) )
      {
        if(p_supl_session_status->u_pdapi_fix_reported == FALSE)
        {
          /*IS801/LPP is still working on position fix*/
          (void) os_TimerStop( supl_cf_timer );
          /* clear timer(s) and call flow states if not already done */
          tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_NULL);
          return;
        }
        else /*Fix is already reported*/
        {
          MSG_MED("Fix is already reported",0,0,0);
          if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801)
          {
            (void) os_TimerStop( supl_cf_timer );
            /* clear timer(s) and call flow states if not already done */
            tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_NULL);
            /*Stop IS801 session*/
            supl_stop_is801_session(TM_STOP_REASON_COMPLETED);
          }
        }
      }

      if ( ( p_supl_session_status->u_pdapi_fix_reported == TRUE ) ||
           ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT3 )
         )
      {
        /* Position already reported, or has received SUPL_END after sending
           SUPL_POS(RRLP-measPosResp-Loc/Err ) */

        /* if there is no active network session going, send DONE event to pdapi */

        tm_umts_up_supl_inform_core_stop_session( TM_STOP_REASON_COMPLETED );
        tm_umts_up_supl_session_init(FALSE);
      }
      else if ( ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT3_ACK ) ||
                ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT1 ) ||
                ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT2 ) ||
                ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT3_RRLP )
              )
      {
        /* Normal case: receiving SUPL_POS after sending SUPL_POS(RRLP-ACK):
           UT3_ACK; Exception case: UT1, UT2, or UT3_RRLP */

        /* inform TM core that network assistance has completed */
        tm_umts_up_supl_inform_core_nt_assist_end();

        /* clear timer(s) and call flow states if not already done */
        tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_NULL);
      }

    } /* close if-Ue-Based */

    else if ( (p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED) ||
              (p_supl_session_status->u_agps_mode == C_SUPL_AFLT_MODE) ||
               (p_supl_session_status->u_agps_mode == C_SUPL_OTDOA_MODE)
            )
    {
      /************************ only SET-assisted reaches below ********************/

      /********************** position *********************************************/

      if ( p_suplEnd->m.positionPresent )
      {

        p_pd_info_buffer->pd_info.pd_data.position_mode = PDSM_PD_POSITION_MODE_MSASSISTED;

        /* SUPL_END does not provide source info. */
        p_pd_info_buffer->pd_info.pd_data.positioning_source = PDSM_PD_POSITION_SOURCE_GPS | PDSM_PD_POSITION_SOURCE_GLO;

        /* set up proper position type in the pdapi report */
        p_pd_info_buffer->pd_info.pd_data.position_type = PDSM_PD_POSITION_TYPE_NEW;

        pd_event = PDSM_PD_EVENT_POSITION;

        if ( p_suplEnd->position.positionEstimate.m.altitudeInfoPresent )
        {
          z_pos_flags.b_Is3D = TRUE;
          pd_event |= PDSM_PD_EVENT_HEIGHT;
        }
        if (p_suplEnd->position.m.velocityPresent)
        {
          z_pos_flags.b_IsVelValid = TRUE;
          pd_event |= PDSM_PD_EVENT_VELOCITY;
        }

        (void) tm_post_pd_event ( TM_PRTL_TYPE_UMTS_UP,
                                  p_supl_session_status->tm_active_session_handle,
                                  pd_event,
                                  &z_supl_pd_info_buffer,
                                  &z_pos_flags);

        /* if there is no active network session going, send DONE event to pdapi */
        tm_umts_up_supl_inform_core_stop_session( TM_STOP_REASON_COMPLETED );
        tm_umts_up_supl_session_init(FALSE);
      } /* close if-Position-available */

      else /* if SUPL_END in a SET-Assisted call flow contains no position */
      {
        if ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT3 )
        {
          /*For IS801 session, SUPL end may not have the position*/
          if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801)
          {
            tm_umts_up_supl_inform_core_stop_session( TM_STOP_REASON_COMPLETED );
            tm_umts_up_supl_session_init(FALSE);
          }
          else
          {
            z_supl_pd_info_buffer.pd_info.end_status = PDSM_PD_END_NONSPECIFIED_ERROR;
            if ( p_suplEnd->m.statusCodePresent )
            {
              if ( p_suplEnd->statusCode == supl_StatusCode_posMethodFailure )
              {
                z_supl_pd_info_buffer.pd_info.end_status =
                  PDSM_PD_END_FIX_METHOD_FAILURE;
              }
            }
            tm_umts_up_supl_inform_core_stop_session( TM_STOP_REASON_GENERAL_ERROR );
          }
        }
        else
        {
          z_supl_pd_info_buffer.pd_info.end_status = PDSM_PD_END_UNKNWN_SYS_ERROR;
          tm_umts_up_supl_inform_core_stop_session( TM_STOP_REASON_GENERAL_ERROR );
        }

        /* clear timer(s) and call flow states if not already done */
        tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_NULL);

        /* clear all protocol session related variables */
        tm_umts_up_supl_session_init(FALSE);

      }

    } /* close-else-if-SET-Assisted */

    else if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_CELL_ID )
    {
      if ( phone_state_info.srv_system == CGPS_SRV_SYS_LTE )
      {
        protocol_type = PDSM_PD_COMM_PROTOCOL_LTE_UP;
      }
       tm_umts_common_send_pdapi_comm_event
          (  protocol_type,
          C_COMM_DONE,
          p_supl_session_status->tm_active_session_handle,
          p_supl_session_status->q_pdapi_client_id
          );

       if ( p_suplEnd->m.positionPresent )
       {

          p_pd_info_buffer->pd_info.pd_data.position_mode = PDSM_PD_POSITION_MODE_UNKNOWN;

          /* SUPL_END does not provide source info. */
          p_pd_info_buffer->pd_info.pd_data.positioning_source = PDSM_PD_POSITION_SOURCE_CELLID;

          /* set up proper position type in the pdapi report */
          p_pd_info_buffer->pd_info.pd_data.position_type = PDSM_PD_POSITION_TYPE_NEW;

          pd_event = PDSM_PD_EVENT_POSITION;

          if ( p_suplEnd->position.positionEstimate.m.altitudeInfoPresent )
          {
             pd_event |= PDSM_PD_EVENT_HEIGHT;
             z_pos_flags.b_Is3D = TRUE;
          }
          if (p_suplEnd->position.m.velocityPresent)
          {
             pd_event |= PDSM_PD_EVENT_VELOCITY;
             z_pos_flags.b_IsVelValid = TRUE;
          }

          (void) tm_post_pd_event ( TM_PRTL_TYPE_UMTS_UP,
                                    p_supl_session_status->tm_active_session_handle,
                                    pd_event,
                                    &z_supl_pd_info_buffer,
                                    &z_pos_flags);

          p_supl_session_status->u_pdapi_fix_reported = TRUE;

          /* if there is no active network session going, send DONE event to pdapi */
          tm_umts_up_supl_inform_core_stop_session( TM_STOP_REASON_COMPLETED );
          tm_umts_up_supl_session_init(FALSE);

       } /* if Position info. is in the message */
       else /* if SUPL_END in a CELL ID call flow contains no position */
       {
           z_supl_pd_info_buffer.pd_info.end_status = PDSM_PD_END_NONSPECIFIED_ERROR;
           if ( p_suplEnd->m.statusCodePresent )
           {
              z_supl_pd_info_buffer.pd_info.end_status = PDSM_PD_END_PDE_REJECT;
           }
           tm_umts_up_supl_inform_core_stop_session( TM_STOP_REASON_GENERAL_ERROR );

          /* clear timer(s) and call flow states if not already done */
          tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_NULL);

          /* clear all protocol session related variables */
          tm_umts_up_supl_session_init(FALSE);
       }

    } /* close if-CELL ID */
    else
    {
      MSG_HIGH("SUPL: unexpected agps mode: %u", p_supl_session_status->u_agps_mode, 0, 0 );
      return;
    }

  } /* close-if-UI */


}


/*
******************************************************************************
* tm_umts_up_supl_supl_end_prepare_or_tx
*
* Function description:
*
* Establish TCP link for sending SuplEnd or call tm_umts_up_supl_supl_end_tx to send
*   SuplEnd.
*
* Parameters:
*
*   p_provided_slp_session_id - pointer to the struture holding
*   slp session ID.
*
*   e_supl_status_code: as enum'ed supl_StatusCode,
*   use 0xFFFFFFFF if status code is to be absent.
*
*   p_hash: pointer to the first byte of HASH, use NULL if hash is to be absent.
*
* Return value: TRUE or FALSE
*
******************************************************************************
*/


boolean  tm_umts_up_supl_supl_end_prepare_or_tx( supl_slp_session_id* p_provided_slp_session_id,
                                                 supl_set_session_id* p_provided_set_session_id,
                                                 supl_StatusCode e_supl_status_code,
                                                 uint8* p_hash,
                                                 uint32 q_supl_version )
{
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;
  pd_comms_return_type pdcomm_connect_ret_value = PDCOMM_RESULT_ERROR_UNKNOWN;
  boolean u_ret = FALSE;
  pending_supl_end   *p_pending_supl_end;

  supl_set_session_id z_set_session_id_immediate_use;

  uint8  u_i;
  boolean u_need_cache = FALSE;

  memset (&z_set_session_id_immediate_use, 0, sizeof(supl_set_session_id));


  /* if no tcp link available yet, set up such link */
  if ( z_pdcomm_tcp_info.curr_state != PDCOMM_STATE_CONNECTED )
  {
    /* if not already in UT0 waiting for connection either, initiate
       connection now*/
    if ( !supl_is_in_any_ut0() )
    {
      /* LSB in the variable passed contains "version number main" */ 
      pdcomm_connect_ret_value = tm_umts_up_supl_comm_tcp_connect((uint8)(q_supl_version & 0x000000FF));

      if ( PDCOMM_RESULT_OK == pdcomm_connect_ret_value ) /* quite unlikely */
      {

        if ( p_provided_slp_session_id ==  NULL )
        {
          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,
                "SLP_sessionID NULL %u, %lx, %lx",
                pdcomm_connect_ret_value,
                p_provided_set_session_id,
                p_provided_slp_session_id);

          return FALSE;

        }

        if ( p_provided_set_session_id == NULL )
        {
          if ( z_Set_Id.set_id_choice == C_SETID_UNKNOWN )
          {
            /* if SET session ID (ismi/msisdn... is still unknow at this point,
               this function will retried it */
            supl_SET_id_get();
          }
          if ( z_Set_Id.set_id_choice != C_SETID_UNKNOWN )
          {
            z_set_session_id_immediate_use.w_Session_Id = 0;
            memscpy(&z_set_session_id_immediate_use.z_Set_Id,
                    sizeof(z_set_session_id_immediate_use.z_Set_Id),
                   &z_Set_Id, sizeof(z_Set_Id) );
          }
          else
          {
            MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SET_ID unavilable: %d",
                  z_Set_Id.set_id_choice);

            return FALSE;
          }
          if ( tm_umts_up_supl_supl_end_tx( p_provided_slp_session_id,
                                            &z_set_session_id_immediate_use,
                                            e_supl_status_code,
                                            p_hash,
                                            q_supl_version)
             )
          {
            u_ret = TRUE;
          }

        }

        else
        {
          if ( tm_umts_up_supl_supl_end_tx(p_provided_slp_session_id,
                                           p_provided_set_session_id,
                                           e_supl_status_code,
                                           p_hash,
                                           q_supl_version)
             )
          {
            u_ret = TRUE;
          }

        }
      } /* pdcomm status now shows connected */

      else if ( PDCOMM_RESULT_WAIT == pdcomm_connect_ret_value ) /* most likely */
      {
        u_need_cache = TRUE;

      } /* pdcomm now shows WAIT */

      else
      {
        /* not possible to connect, abort */
        MSG_HIGH("SUPL: TCP conn. failed", 0, 0, 0);
      }

    } /* not in any UT0*/

    else /* already in an UT0 statas, don't attempt to bring data link again */
    {
      u_need_cache = TRUE;
      MSG_MED("Already in UT0, cache SUPL_END", 0, 0, 0);
    } /* has been in UT0 already */

    if ( u_need_cache )
    {
      p_pending_supl_end = &z_pending_supl_end[0];
      z_supl_session_status.q_supl_version = q_supl_version;

      /* if the number of pending SUPL_END messages exceeds the maximum, this one
         will not be sent */
      for ( u_i = C_MAX_NUM_PENDING_SUPL_END; u_i; u_i--)
      {
        if ( p_pending_supl_end->u_validity == FALSE )
        {

          if (p_provided_set_session_id != NULL)
          {
            memscpy( &p_pending_supl_end->set_session_id,
                     sizeof(p_pending_supl_end->set_session_id),
                    p_provided_set_session_id,
                     sizeof(*p_provided_set_session_id) );
          }
          else
          {
            if ( z_Set_Id.set_id_choice == C_SETID_UNKNOWN )
            {
              /* if SET session ID (ismi/msisdn... is still unknow at this point,
                 this function will retried it */
              supl_SET_id_get();
            }
            if ( z_Set_Id.set_id_choice != C_SETID_UNKNOWN )
            {
              p_pending_supl_end->set_session_id.w_Session_Id = 0;
              memscpy(&p_pending_supl_end->set_session_id.z_Set_Id,
                      sizeof(p_pending_supl_end->set_session_id.z_Set_Id),
                      &z_Set_Id, sizeof(z_Set_Id) );
            }
            else
            {
              MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SET_ID unavilable: %d",
                    z_Set_Id.set_id_choice);

              return FALSE;
            }
          }

          if (p_provided_slp_session_id != NULL )
          {
            memscpy( &p_pending_supl_end->slp_session_id,
                     sizeof(p_pending_supl_end->slp_session_id),
                    p_provided_slp_session_id,
                     sizeof(*p_provided_slp_session_id) );
          }
          else
          {
            memscpy( &p_pending_supl_end->slp_session_id,
                     sizeof(p_pending_supl_end->slp_session_id),
                    &z_supl_session_status.z_session_id.z_Slp_Sess_Id,
                     sizeof(z_supl_session_status.z_session_id.z_Slp_Sess_Id) );
          }
          p_pending_supl_end->e_supl_status_code = e_supl_status_code;

          if ( p_hash != NULL )
          {
            p_pending_supl_end->u_is_hash_present = TRUE;
            memscpy(p_pending_supl_end->hash, sizeof(p_pending_supl_end->hash), p_hash, sizeof(p_pending_supl_end->hash));
          }
          else
          {
            p_pending_supl_end->u_is_hash_present = FALSE;
          }

          p_pending_supl_end->u_validity = TRUE;

          break;
        }
        else
        {
          p_pending_supl_end++;
        }

      }

      /* if not yet set state to include C_SUPL_CF_STATE_UT0_SUPLEND, do so now */
      if ( !( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLEND ) )
      {
        supl_set_cf_state(C_SUPL_CF_STATE_UT0_SUPLEND, 0);
        MSG_MED("SUPL: UT0-SUPLEND starts", 0, 0, 0);
      }

    } /* cache SUPL_END for later delivery */

  } /* close if-tcp-link-not-yet-available */

  else /* already connected */
  {
    if ( p_provided_slp_session_id ==  NULL )
    {
      /* SLP session ID being NULL is not used in this scenario */
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,
            "SLP_sessionID NULL %u, %lx, %lx",
            pdcomm_connect_ret_value,
            p_provided_set_session_id,
            p_provided_slp_session_id);

      return FALSE;

    }

    if (p_provided_set_session_id == NULL)
    {
      if ( z_Set_Id.set_id_choice == C_SETID_UNKNOWN )
      {
        /* if SET session ID (ismi/msisdn... is still unknow at this point,
           this function will retried it */
        supl_SET_id_get();
      }
      if ( z_Set_Id.set_id_choice != C_SETID_UNKNOWN )
      {
        z_set_session_id_immediate_use.w_Session_Id = 0;
        memscpy(&z_set_session_id_immediate_use.z_Set_Id,
                sizeof(z_set_session_id_immediate_use.z_Set_Id),
               &z_Set_Id, sizeof(z_Set_Id) );
      }
      else
      {
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SET_ID unavilable: %d",
              z_Set_Id.set_id_choice);

        return FALSE;
      }

      if ( tm_umts_up_supl_supl_end_tx( p_provided_slp_session_id,
                                        &z_set_session_id_immediate_use,
                                        e_supl_status_code,
                                        p_hash,
                                        q_supl_version)
         )
      {
        u_ret = TRUE;
      }

    }

    else
    {

      if ( tm_umts_up_supl_supl_end_tx( p_provided_slp_session_id,
                                        p_provided_set_session_id,
                                        e_supl_status_code,
                                        p_hash,
                                        q_supl_version)
         )
      {
        u_ret = TRUE;
      }
    }

  }

  return u_ret;

}


/*
******************************************************************************
* tm_umts_up_supl_supl_end_tx
*
* Function description:
*
* Encode and transmit SUPL END
*
* Parameters:
*
*   p_provided_slp_session_id - pointer to the struture holding
*   slp session ID.
*
*   e_supl_status_code: as enum'ed supl_StatusCode,
*   use 0xFFFFFFFF if status code is to be absent.
*
*   p_hash: pointer to the first byte of HASH, use NULL if hash is to be absent.
*
* Return value: TRUE or FALSE
*
******************************************************************************
*/

boolean tm_umts_up_supl_supl_end_tx (supl_slp_session_id* p_provided_slp_session_id,
                         supl_set_session_id*             p_provided_set_session_id,
                         supl_StatusCode e_supl_status_code,
                         uint8* p_hash,
                         uint32 q_supl_version)
{

  boolean u_ret = FALSE;

  tm_pdu_buf_type   pdu_buff; /* including length and pointer to data */
  supl_ULP_PDU      pdu;
  supl_SUPLEND*     p_Suplend;
  supl_SUPLEND      z_Suplend;

  uint8*           p_length;
  uint8            u_length_msb = 0;
  uint8            u_length_lsb = 0;

  uint16*       p_SET_SessionID_sessionId;
  supl_set_id*  p_SET_ID;

  int encode_status;

  memset(&pdu_buff, 0, sizeof( tm_pdu_buf_type ));
  memset(&pdu, 0x00, sizeof(supl_ULP_PDU));
  memset(&z_Suplend, 0, sizeof(supl_SUPLEND));

  pdu.message.t = C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLEND;
  p_Suplend = &z_Suplend;
  pdu.message.u.msSUPLEND = p_Suplend;

  if ( p_provided_set_session_id == NULL )
  {
    p_SET_SessionID_sessionId = NULL;
    p_SET_ID = NULL;
  }
  else
  {
    p_SET_SessionID_sessionId = &p_provided_set_session_id->w_Session_Id;
    p_SET_ID = &p_provided_set_session_id->z_Set_Id;
  }

  /* overhead build function has to be after we know what message is to be constructed */
  if ( !supl_header_build (&pdu, p_SET_SessionID_sessionId, p_SET_ID, p_provided_slp_session_id, q_supl_version) )
  {
    MSG_ERROR("SUPL: SuplEnd attempt aborted: header", 0, 0, 0);
    return FALSE;
  }

  if ( e_supl_status_code != 0xFFFFFFFF )
  {
    p_Suplend->m.statusCodePresent = 1;
    p_Suplend->statusCode = e_supl_status_code;
  }

  if ( p_hash != NULL )
  {
    p_Suplend->m.verPresent = 1;
    p_Suplend->ver.numbits = 64;
    memscpy( &p_Suplend->ver.data[0], sizeof(p_Suplend->ver.data), p_hash, sizeof(p_Suplend->ver.data));
  }

  /* If encoding failed, pdu_buf.value will
  be NULL and pdu_buf.length is meaningless. If encoding succeeded,
  pdu_buf.value will be non-NULL and pdu_buf.length will contain the number
  of bytes in the encoded PDU. */

  encode_status = tm_osys_asn1_encode_pdu((void *)&pdu, &pdu_buff, PROTO_SUPL);

  if ( (encode_status != 0) || (pdu_buff.value == NULL ) || (pdu_buff.length == 0) )
  {
    /* ASN.1 encoding failed */
    if (pdu_buff.value != NULL )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SUPL ASN1 enc. err. Freeing: %d,%lx,%u",
            encode_status, pdu_buff.value, pdu_buff.length);

      tm_asn1_free_osys_buf( pdu_buff.value, PROTO_SUPL );
      pdu_buff.value = NULL;

    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SUPL ASN1 enc. err. No alloc.: %d,%lx,%u",
            encode_status, pdu_buff.value, pdu_buff.length);
    }
    return FALSE;
  }

  else
  {
    pd_comms_return_type ret_val;

    p_length = (uint8 *)pdu_buff.value;
    u_length_msb = (pdu_buff.length >> 8) & 0x00ff;
    u_length_lsb = pdu_buff.length & 0x00ff;

    *p_length++ = u_length_msb;
    *p_length = u_length_lsb;

    ret_val = tm_umts_up_supl_comm_write( (uint8 *)pdu_buff.value, pdu_buff.length);

    if ( ( ret_val != PDCOMM_RESULT_WAIT ) && ( ret_val != PDCOMM_RESULT_OK ) )
    {
      MSG_ERROR("SUPL: SuplEnd attempt aborted", 0, 0, 0);
    }
    else
    {
      u_ret = TRUE;
    }

    /*Free the encoded PDU*/
    tm_asn1_free_osys_buf( pdu_buff.value, PROTO_SUPL );
    pdu_buff.value = NULL;

  }

  if ( u_ret )
  {

    MSG_MED("SuplEnd sent ", 0, 0, 0);
    z_supl_session_status.u_send_supl_end_at_fail = FALSE;
  }

  return u_ret;
}


/*
 ******************************************************************************
 * supl_header_build
 *
 * Function description:
 *   Provides information for building SUPL overhead: version, SessionID
 *
 * Parameters:
 *
 *  p_pdu: pointer to a structure holding to be encoded SUPL PDU
 *
 *  p_provided_set_session_id: pointer to the provided SET_SESSION_ID.sessionID;
 *
 *  p_provided_Set_Id: pointer to the SET_ID of the one saved provided by the SLP,
 *    mainly for error case handling.
 *
 *  p_provided_slp_session_id: pointer to a structure holding SLP Session ID;
 *    if NULL, this function will use SLP session ID info in z_supl_session_status.
 *
 * Return value:
 *  TRUE: header build successful; FALSE: header build failure.
 *
 *
 ******************************************************************************
*/
boolean supl_header_build
( supl_ULP_PDU*        p_pdu,
  uint16*              p_provided_set_session_id,
  supl_set_id*         p_provided_Set_Id,
  supl_slp_session_id* p_provided_slp_session_id,
  uint32               q_supl_version)
{
  uint32 cnt;
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;
  uint16               *p_set_session_id;  /* this is session_id portion of the SET_SESSION_ID */
  supl_slp_session_id  *p_slp_session_id;

  supl_set_id*  p_Set_Id;

  if ( p_provided_set_session_id == NULL )
  {
    /* this is used by the current session only (no long lasting areaEvent type)
       and is 0 all the time unless carrying a wrong session ID from the SLP and is
       building the header for SUPL_END in this case */
    p_set_session_id = &p_supl_session_status->z_session_id.z_Set_Sess_Id.w_Session_Id;
  }
  else
  {
    p_set_session_id = p_provided_set_session_id;
  }

  if ( p_provided_Set_Id == NULL )
  {
    if ( z_Set_Id.set_id_choice == C_SETID_UNKNOWN )
    {
      /* if SET session ID (ismi/msisdn... is still unknow at this point,
         this function will retried it */
      supl_SET_id_get();
    }
    if ( z_Set_Id.set_id_choice != C_SETID_UNKNOWN )
    {
      p_Set_Id = &z_Set_Id;
    }
    else
    {
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SET_ID unavilable: %d",
            z_Set_Id.set_id_choice);

      return FALSE; /* unable to get SET_ID */
    }
  }
  else
  {
    p_Set_Id = p_provided_Set_Id;
  }


  if ( p_provided_slp_session_id == NULL )
  {
    p_slp_session_id = &z_supl_session_status.z_session_id.z_Slp_Sess_Id;
  }
  else
  {
    p_slp_session_id = p_provided_slp_session_id;
  }


  /* init length to 0.  we will fill in the correct length later after it is encoded */
  p_pdu->length = 0;

  /* version  */
  p_pdu->version.maj = q_supl_version & 0x000000FF;
  p_pdu->version.min = (q_supl_version >> 8 ) & 0x000000FF;
  p_pdu->version.servind = (q_supl_version >> 16) & 0x000000FF ;

  p_pdu->sessionID.m.setSessionIDPresent = 1;

  /* SUPLSTART sets the SET session ID; incremented by 1 each time SUPLSTART is sent,
     using the range of 1 to 65535 */
  if ( p_pdu->message.t == C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLSTART )
  {
    /* SLP Session ID should never be present in SUPL START */
    p_slp_session_id->presence = FALSE;
  }

  if ( p_slp_session_id->presence == TRUE )
  {
    p_pdu->sessionID.m.slpSessionIDPresent = 1;
  }

  /******  SET session ID *******/
  p_pdu->sessionID.setSessionID.sessionId = *p_set_session_id;

  if ( p_provided_Set_Id == NULL )
  {
    /* if SET session ID (ismi/msisdn... is still unknow at this point,
       this function will retried it */
    supl_SET_id_get();
  }

  if ( p_Set_Id->set_id_choice == T_supl_SETId_msisdn )
  {
    p_pdu->sessionID.setSessionID.setId.t = T_supl_SETId_msisdn;
    p_pdu->sessionID.setSessionID.setId.u.msisdn =
      (supl_SETId_msisdn *) &p_Set_Id->u.msisdn;
  }

  else if ( p_Set_Id->set_id_choice == T_supl_SETId_imsi )
  {
    p_pdu->sessionID.setSessionID.setId.t = T_supl_SETId_imsi;
    p_pdu->sessionID.setSessionID.setId.u.imsi =
      (supl_SETId_imsi *) &p_Set_Id->u.imsi;

  }

  else if(p_Set_Id->u.other_setID.valid == TRUE)
  {
    /* neither MSISDN nor IMSI found: shouldn't happen unless in the case of
       test cases for error handling. Use the other_setID field
       of the session status structure because in this case the set ID field
       of the incoming message was decoded there and it is needed to return it
       in the SUPL End msg. */
    uint8 *p_src = &p_Set_Id->u.other_setID.raw_contents[0];
    uint8 *p_dest = (uint8 *)&p_pdu->sessionID.setSessionID.setId.u;

    p_pdu->sessionID.setSessionID.setId.t =
                     p_Set_Id->set_id_choice;

    for ( cnt = OTHER_SETID_SIZE; cnt; cnt-- )
    {
      *p_dest++ = *p_src++;
    }
  }


  /*** SLP Session ID  ****/
  if (  p_pdu->sessionID.m.slpSessionIDPresent == 1 )
  {

    p_pdu->sessionID.slpSessionID.sessionID.numocts = C_SUPL_SLP_SESSION_ID_BYTE_LENGTH;
    memscpy(p_pdu->sessionID.slpSessionID.sessionID.data, sizeof(p_pdu->sessionID.slpSessionID.sessionID.data),
            p_slp_session_id->session_Id, sizeof(p_slp_session_id->session_Id));

    p_pdu->sessionID.slpSessionID.slpId.t = p_slp_session_id->zSlpAddress.choice;

    if (p_pdu->sessionID.slpSessionID.slpId.t == T_supl_SLPAddress_iPAddress)
    {

      p_pdu->sessionID.slpSessionID.slpId.u.iPAddress = &z_supl_IPAddress;
      z_supl_IPAddress.t = p_slp_session_id->zSlpAddress.u.z_Ip_Address.choice;;

      /* IPV4 */
      if ( p_slp_session_id->zSlpAddress.u.z_Ip_Address.choice == C_SUPL_ASN1_CHOICE_IPV4ADDRESS)
      {
        z_supl_IPAddress.u.ipv4Address = &p_slp_session_id->zSlpAddress.u.z_Ip_Address.u.Ip_V4;
      }
      /* IPV6 */
      else if ( p_slp_session_id->zSlpAddress.u.z_Ip_Address.choice == T_supl_IPAddress_ipv6Address)
      {
        z_supl_IPAddress.u.ipv6Address = &p_slp_session_id->zSlpAddress.u.z_Ip_Address.u.Ip_V6;
      }
    }

    else if ( p_pdu->sessionID.slpSessionID.slpId.t == T_supl_SLPAddress_fQDN )
    {
      p_pdu->sessionID.slpSessionID.slpId.u.fQDN = (supl_FQDN)p_slp_session_id->zSlpAddress.u.z_fqdn.fqdn;

    }

    else
    {
      /* should not happen */
      return FALSE;
    }

  }

  return TRUE;
}

/********************************************************************
 *
 * supl_cell_info_construct_tds_gsm
 *
 * Function description:
 *   Construct cell information specific to active RAT = GSM.
 *
 * Parameters:
 *   p_Asn1CellInfo      : Pointer to a structure that contains
 *                         memory allocated for the ASN1 tool
 *
 *   p_CellInfoAllocated : Pointer to a structure to hold the
 *                         to-be-constructed cell information
 *
 *   q_CellId            : Cell Identity, range: (0...65535)
 *
 *   u_Lac0              : Location Area Code, byte 0
 *
 *   u_Lac1              : Location Area Code, byte 1
 *
 *   z_RrcPlmnId         : Structure that holds MCC/MNC parameters
 *
 * Return: TRUE/FALSE ( FALSE: no valid cell ID )
 *********************************************************************
 */
static boolean supl_cell_info_construct_tds_gsm
(
  supl_CellInfo*              p_Asn1CellInfo,
  supl_cell_info_struct_type* p_CellInfoAllocated,
  uint32                      q_CellId,
  uint8                       u_Lac0,
  uint8                       u_Lac1,
  rrc_plmn_identity_type      z_RrcPlmnId
)
{
  uint32                 q_test1, q_test2;
  rrc_plmn_identity_type z_PlmnId = z_RrcPlmnId;

  if ((NULL == p_Asn1CellInfo) || (NULL == p_CellInfoAllocated))
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "supl_cell_info_construct_tds_gsm: NULL arguments");
    return FALSE;
  }

  memset(&p_CellInfoAllocated->u1.gsmCell, 0, sizeof(supl_GsmCellInformation));

  p_Asn1CellInfo->t         = C_SUPL_ASN1_CHOICE_GSM_CELL;
  p_Asn1CellInfo->u.gsmCell = &p_CellInfoAllocated->u1.gsmCell;

  /* CI: use low 16 bits of TDS cell identity */
  p_Asn1CellInfo->u.gsmCell->refCI = q_CellId & 0xFFFF;

  /* LAC: tds_plmn_cell_info.lac is of uint8 lac[RRC_MAX_GSMMAP_LAC_LENGTH],
     where RRC_MAX_GSMMAP_LAC_LENGTH is defined as 2, in wcdma\api\rrcmmif.h;
     refLAC here is of uint16 */
  p_Asn1CellInfo->u.gsmCell->refLAC   = u_Lac0;
  p_Asn1CellInfo->u.gsmCell->refLAC <<= 8;
  p_Asn1CellInfo->u.gsmCell->refLAC  |= u_Lac1;

  /* MCC/MNC: convert to integers */
  if (!tm_umts_supl_wcdma_plmn_id_organizer(&z_PlmnId,
                                            &q_test1,
                                            &q_test2))
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Cached cell db plmn organizer failure RAT=: %u",
          z_cell_info_cached.e_ActiveRAT);
    return FALSE;
  }
  p_Asn1CellInfo->u.gsmCell->refMCC = (uint16)q_test1;
  p_Asn1CellInfo->u.gsmCell->refMNC = (uint16)q_test2;

  return TRUE;
}

/********************************************************************
 *
 * supl_cell_info_construct_tds_wcdma
 *
 * Function description:
 *   Construct cell information specific to active RAT = WCDMA.
 *
 * Parameters:
 *   p_Asn1CellInfo      : Pointer to a structure that contains
 *                         memory allocated for the ASN1 tool
 *
 *   p_CellInfoAllocated : Pointer to a structure to hold the
 *                         to-be-constructed cell information
 *
 *   q_CellId            : Cell Identity, range: (0...)
 *
 *   u_Lac0              : Location Area Code, byte 0
 *
 *   u_Lac1              : Location Area Code, byte 1
 *
 *   z_RrcPlmnId         : Structure that holds MCC/MNC parameters
 *
 * Return: TRUE/FALSE ( FALSE: no valid cell ID )
 *********************************************************************
 */
static boolean supl_cell_info_construct_tds_wcdma
(
  supl_CellInfo*              p_Asn1CellInfo,
  supl_cell_info_struct_type* p_CellInfoAllocated,
  uint32                      q_CellId,
  rrc_plmn_identity_type      z_RrcPlmnId
)
{
  uint32                 q_test1, q_test2;
  rrc_plmn_identity_type z_PlmnId = z_RrcPlmnId;

  if ((NULL == p_Asn1CellInfo) || (NULL == p_CellInfoAllocated))
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "supl_cell_info_construct_tds_wcdma: NULL arguments");
    return FALSE;
  }

  memset(&p_CellInfoAllocated->u1.wcdmaCell, 0, sizeof(supl_WcdmaCellInformation));
  memset(&p_CellInfoAllocated->u2.z_supl_wcdma_cell_info.z_frequency_info_fdd, 0,
         sizeof(supl_FrequencyInfoFDD));
  memset(&p_CellInfoAllocated->u2.z_supl_wcdma_cell_info.z_measured_results, 0,
         sizeof(supl_MeasuredResults));

  p_Asn1CellInfo->t           = C_SUPL_ASN1_CHOICE_WCDMA_CELL;
  p_Asn1CellInfo->u.wcdmaCell = &p_CellInfoAllocated->u1.wcdmaCell;

  /* Cell ID */
  p_Asn1CellInfo->u.wcdmaCell->refUC = q_CellId;

  /* MCC/MNC: convert to integers */
  if (!tm_umts_supl_wcdma_plmn_id_organizer(&z_PlmnId,
                                            &q_test1,
                                            &q_test2))
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Cached cell db plmn organizer failure RAT=: %u",
          z_cell_info_cached.e_ActiveRAT);
    return FALSE;
  }
  p_CellInfoAllocated->u1.wcdmaCell.refMCC = (uint16)q_test1;
  p_CellInfoAllocated->u1.wcdmaCell.refMNC = (uint16)q_test2;

  return TRUE;
}

/********************************************************************
 *
 * supl_cell_information_construct
 *
 * Function description:
 *   Construct cell information used by several SUPL messages.
 *
 * Parameters:
 *   p_asn1_cell_info      : pointer to a structure that contains 
 *                           memory allocated for the ASN1 tool
 *
 *   p_cell_info_allocated : pointer to a structure to hold the
 *                           to-be-constructed cell information
 *
 * Return: TRUE/FALSE ( FALSE: no valid cell ID )
 *********************************************************************
 */
static boolean supl_cell_information_construct(supl_CellInfo* p_asn1_cell_info,
                                               supl_cell_info_struct_type* p_cell_info_allocated)
{

  tm_cm_phone_state_info_s_type phone_state_info;

  #ifdef FEATURE_CGPS_UMTS_CELLDB
  rrc_plmn_identity_type   *p_wcdma_plmn_identity = &z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id;
  rrc_int_cell_id_type     *p_wcdma_int_cell_id   = &z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.cell_id;
  #endif

  byte                     *p_gsm_identity;
  current_bs_info_T        rr_cell_info;


  #ifdef FEATURE_SUPL1_TDS  /* defined in custcgps.h, if .builds defines FEATURE_CGPS_USES_TDS */
  rrc_plmn_identity_type   *p_tds_plmn_identity = &z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.plmn_id;
  tdsrrcmeas_plmn_cell_info_struct_type     tds_plmn_cell_info;
  #endif

  boolean ret_val = FALSE;

  uint8 num_cells = 0;
  uint8 i = 0;
  uint8 j = 0;

  uint32 q_test1, q_test2;


  if ((p_asn1_cell_info == NULL) || (p_cell_info_allocated == NULL))
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL pointer");
    return FALSE;
  }

  /* Query TM-CM module to get current acquired system */
  tm_cm_iface_get_phone_state(&phone_state_info);

  if ( ( phone_state_info.srv_system != CGPS_SRV_SYS_WCDMA ) &&
       ( phone_state_info.srv_system != CGPS_SRV_SYS_GSM ) &&
       ( phone_state_info.srv_system != CGPS_SRV_SYS_LTE ) &&
       ( phone_state_info.srv_system != CGPS_SRV_SYS_TDSCDMA ) &&
       ( ( TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable) == 0 ) ||
         ( ( TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable) != 0 )  &&
           ( phone_state_info.srv_system != CGPS_SRV_SYS_CDMA ) &&
           ( phone_state_info.srv_system != CGPS_SRV_SYS_HDR )
         )
       )
     )
  {
    return FALSE;
  }

  /* Initialize ASN.1 Cell ID Structure to zeroes */
  memset( p_asn1_cell_info, 0, sizeof( supl_CellInfo ) );

  if ( ( phone_state_info.srv_system == CGPS_SRV_SYS_WCDMA ) &&
       z_cell_info_cached.u_CachedCellIDValid &&
       ( z_cell_info_cached.e_ActiveRAT == CELLDB_ACTIVERAT_WCDMA )
     )
  {

    #ifdef FEATURE_CGPS_UMTS_CELLDB /* WCDMA cell info. provided by MGP when
                                       this feature defined */

    memset(&p_cell_info_allocated->u1.wcdmaCell, 0, sizeof (supl_WcdmaCellInformation) );

    memset(&p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_frequency_info_fdd, 0,
           sizeof(supl_FrequencyInfoFDD));

    memset(&p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_measured_results, 0,
           sizeof(supl_MeasuredResults) );

#ifdef FEATURE_USER_PLANE_MRL
    memset(&p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_cell_measured_results, 0,
           (sizeof(supl_CellMeasuredResults) * WL1_LSM_CELLS_MEAS_INFO_MAX) );

    memset(&p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_cell_measure_results_fdd, 0,
           (sizeof(supl_CellMeasuredResults_modeSpecificInfo_fdd) *
            WL1_LSM_CELLS_MEAS_INFO_MAX ));
#endif
    p_asn1_cell_info->t = C_SUPL_ASN1_CHOICE_WCDMA_CELL;
    p_asn1_cell_info->u.wcdmaCell = &p_cell_info_allocated->u1.wcdmaCell;
    p_asn1_cell_info->u.wcdmaCell->frequencyInfo.modeSpecificInfo.u.fdd =
      &p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_frequency_info_fdd;

    /* MCC/MNC: convert to integers */
    if ( !tm_umts_supl_wcdma_plmn_id_organizer ( p_wcdma_plmn_identity,
                                                 &q_test1,
                                                 &q_test2
                                               )
       )
    {
      return FALSE;
    }
    p_cell_info_allocated->u1.wcdmaCell.refMCC = (uint16)q_test1;
    p_cell_info_allocated->u1.wcdmaCell.refMNC = (uint16)q_test2;


    /* Cell ID */
    p_asn1_cell_info->u.wcdmaCell->refUC = *p_wcdma_int_cell_id;

    #ifdef FEATURE_USER_PLANE_MRL
    #ifndef FEATURE_CGPS_SUPL_VARIANT_NO_ECID /* NOT defined by default */

    /* NMR Information: Only send NMR info if number of handovers is <= NMR_HANDOVER_LIMIT and
       there is valid information */

    MSG_MED("SUPL MRL - Handovers since last cell id = %d",
             z_cell_info_cached.q_NumCellChangesCachedCellID, 0, 0);

    num_cells = z_DBCellMeas.num_cells;
    MSG_LOW("SUPL MRL - num cells = %d", num_cells, 0, 0);

    /* set to max */
    if ( num_cells > WL1_LSM_CELLS_MEAS_INFO_MAX )
    {
      num_cells = WL1_LSM_CELLS_MEAS_INFO_MAX;
    }

    if( num_cells > 0 )
    {
      /* frequency info */
      p_asn1_cell_info->u.wcdmaCell->m.frequencyInfoPresent = 1;
      p_asn1_cell_info->u.wcdmaCell->frequencyInfo.modeSpecificInfo.t =
        C_SUPL_ASN1_CHOICE_FREQINFO_MODE_SPECIFIC_INFO_FDD;
      p_asn1_cell_info->u.wcdmaCell->frequencyInfo.modeSpecificInfo.u.fdd->m.uarfcn_ULPresent = 0;
      p_asn1_cell_info->u.wcdmaCell->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL =
       z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.freq;

      /* scrambling code */
      p_asn1_cell_info->u.wcdmaCell->m.primaryScramblingCodePresent = 1;
      p_asn1_cell_info->u.wcdmaCell->primaryScramblingCode =
       z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.psc;

      /* measured results list */
      p_asn1_cell_info->u.wcdmaCell->m.measuredResultsListPresent = 1;
      p_asn1_cell_info->u.wcdmaCell->measuredResultsList.n = 1;
      p_asn1_cell_info->u.wcdmaCell->measuredResultsList.elem =
        &p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_measured_results;

      /** freq. measurement **/
      p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_measured_results.m.frequencyInfoPresent = 1;
      p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_measured_results.frequencyInfo.modeSpecificInfo.t =
        C_SUPL_ASN1_CHOICE_FREQINFO_MODE_SPECIFIC_INFO_FDD;
      p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_measured_results.frequencyInfo.modeSpecificInfo.u.fdd =
        &p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_frequency_info_fdd;
      p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_frequency_info_fdd.m.uarfcn_ULPresent = 0;
      p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_frequency_info_fdd.uarfcn_DL =
          z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.freq;


      /*** cell measurements ***/
      p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_measured_results.m.cellMeasuredResultsListPresent = 1;
      p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_measured_results.cellMeasuredResultsList.elem =
        &p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_cell_measured_results[0];

      for ( i = 0; i < num_cells; i++ )
      {

        p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_measured_results.cellMeasuredResultsList.n++;

        p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_cell_measured_results[i].m.cellIdentityPresent = 0;
        p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_cell_measured_results[i].modeSpecificInfo.t =
          C_ASN1_SUPL_FREQINFO_MODE_SPECIFIC_INFO_FDD;

        p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_cell_measured_results[i].modeSpecificInfo.u.fdd =
         &p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_cell_measure_results_fdd[i];
        p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_cell_measure_results_fdd[i].m.cpich_Ec_N0Present = 1;
        p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_cell_measure_results_fdd[i].m.cpich_RSCPPresent = 1;

        p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_cell_measure_results_fdd[i].primaryCPICH_Info.primaryScramblingCode =
             z_DBCellMeas.cells[i].pri_scr_code;

        p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_cell_measure_results_fdd[i].cpich_Ec_N0 =
           z_DBCellMeas.cells[i].ec_no_val;

        p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_cell_measure_results_fdd[i].cpich_RSCP =
           z_DBCellMeas.cells[i].rscp_val;

        /* map the delta RSCP, if available */
        if (z_DBCellMeas.cells[i].delta_rscp_included)
        {
          /* validate the delta RSCP range and map it */
          if ( ( z_DBCellMeas.cells[i].delta_rscp <= -1 ) && ( z_DBCellMeas.cells[i].delta_rscp >= -5) )
          {
            /* SUPL RSCP range is 0 thru 91 and 123 thru 127.  123 thru 127 maps to -5 thru -1 */
            p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_cell_measure_results_fdd[i].cpich_RSCP =
              z_DBCellMeas.cells[i].delta_rscp + 128;
          }
        }
      } /* for-loop-num-cells */
    } /* num of cells > 0 */

    else
    {
        MSG_LOW("SUPL - MRL num cells = 0.  Skipping MRL info.", 0, 0, 0);
    }
    #endif /* not FEATURE_CGPS_SUPL_VARIANT_NO_ECID */
    #endif /* FEATURE_USER_PLANE_MRL */

    ret_val = TRUE;

    #endif /* FEATURE_CGPS_UMTS_CELLDB */

  } /* on WCDMA as told by CM and having valid cell ID as told by MGP */

  else if ( phone_state_info.srv_system == CGPS_SRV_SYS_GSM )
  {

  if (tm_umts_up_supl_get_cell_db_type() == CELL_DB_EXTERNAL_MODEM)
  {
      if (z_external_cell_info_cached.serviceStatus == STATUS_CAMPED_ROAMING)
      {
        /* Bring down the session if external/main modem is in roaming state for SGLTE*/
    MSG_HIGH("Disabling SUPL when external/main modem is in roaming state for SGLTE",0,0,0);
        return FALSE;
      }
    else
    {
      memset(&p_cell_info_allocated->u1.gsmCell, 0, sizeof (supl_GsmCellInformation) );

        p_asn1_cell_info->t = C_SUPL_ASN1_CHOICE_GSM_CELL;
        p_asn1_cell_info->u.gsmCell = &p_cell_info_allocated->u1.gsmCell;

        /* CID */
        p_asn1_cell_info->u.gsmCell->refCI = z_external_cell_info_cached.cellInfo.gsmCell.CID;

        /* LAC */
        p_asn1_cell_info->u.gsmCell->refLAC = z_external_cell_info_cached.cellInfo.gsmCell.LAC;

      /*MCC*/
      p_asn1_cell_info->u.gsmCell->refMCC = z_external_cell_info_cached.cellInfo.gsmCell.MCC;

      /*MNC*/
      p_asn1_cell_info->u.gsmCell->refMNC = z_external_cell_info_cached.cellInfo.gsmCell.MNC;

        p_asn1_cell_info->u.gsmCell->m.tAPresent = FALSE;
      p_asn1_cell_info->u.gsmCell->m.nMRPresent = FALSE;

        ret_val = TRUE;
    }
  }
    else
    {
      memset(&rr_cell_info, 0 , sizeof(rr_cell_info)); 
    #ifndef FEATURE_CGPS_NO_UMTS_TDS
	    rr_ds_get_current_bs_info( tm_core_get_current_dd_subs_id(), &rr_cell_info );
    #endif    
      if ( rr_cell_info.camped_on_flag )
      {
        memset(&p_cell_info_allocated->u1.gsmCell, 0, sizeof (supl_GsmCellInformation) );

        p_asn1_cell_info->t = C_SUPL_ASN1_CHOICE_GSM_CELL;
        p_asn1_cell_info->u.gsmCell = &p_cell_info_allocated->u1.gsmCell;

        /* CI */
        p_asn1_cell_info->u.gsmCell->refCI = rr_cell_info.cell_identity;

        /* LAC */
        p_asn1_cell_info->u.gsmCell->refLAC =
          rr_cell_info.location_area_identification.location_area_code;

        p_gsm_identity = rr_cell_info.location_area_identification.plmn_id.identity;

        /* MCC / MNC is returned from RR in the following format:
        **                                        Bit
        **                 |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |
        **                 |===============================================|
        **         octet 1 |      MCC Digit 2      |      MCC Digit 1      |
        **                 |-----------------------------------------------|
        **         octet 2 |      MNC Digit 3      |      MCC Digit 3      |
        **                 |-----------------------------------------------|
        **         octet 3 |      MNC Digit 2      |      MNC Digit 1      |
        **                 |===============================================| */

        /* convert MCC to an integer */
        p_asn1_cell_info->u.gsmCell->refMCC = (p_gsm_identity[0] & 0x0F) * 100;
        p_asn1_cell_info->u.gsmCell->refMCC += (( p_gsm_identity[0] >> 4 ) & 0x0F) * 10;
        p_asn1_cell_info->u.gsmCell->refMCC += p_gsm_identity[1] & 0x0F;

        /* MNC */
        /* assume there will be 3 digits, so put the 1st 2 digits in the */
        /* 100s and 10s place. */
        p_asn1_cell_info->u.gsmCell->refMNC = (p_gsm_identity[2] & 0x0F) * 100;
        p_asn1_cell_info->u.gsmCell->refMNC += (( p_gsm_identity[2] >> 4 ) & 0x0F) * 10;

        /* is MNC3 present? */
        /* add in the 3rd digit if there is one */
        if ( ( ( p_gsm_identity[1] >> 4 ) & 0x0F ) != 0x0F )
        {
          p_asn1_cell_info->u.gsmCell->refMNC += ( p_gsm_identity[1] >> 4 ) & 0x0F;
        }
        else
        {
          /* divide by 10 if there are only 2 digits in the mnc */
          p_asn1_cell_info->u.gsmCell->refMNC /= 10;
        }

        /* TA */
		#ifndef FEATURE_CGPS_NO_UMTS_TDS
        geran_eng_mode_data_read(ENG_MODE_TIMING_ADVANCE, (void *) &timing_advance);
		#endif
        if (timing_advance != (uint32)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER)
        {
          p_asn1_cell_info->u.gsmCell->m.tAPresent = 1;
          p_asn1_cell_info->u.gsmCell->tA          = timing_advance;
        }

        if (TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_NMR_SUPPORTED) == 0)
        {
          MSG_HIGH("NMR feature disabled", 0,0,0);
          ret_val = TRUE;
          return ret_val;
        }

        /* NMR list construction */
		#ifndef FEATURE_CGPS_NO_UMTS_TDS
        geran_eng_mode_data_read(ENG_MODE_NMR_INFO, (void *) &nmr_info);
		#endif
        num_cells = nmr_info.num_of_items;
        MSG_HIGH("NMR - num cells = %d", num_cells, 0, 0);

        /* Add 1 to account for serving cell */
        num_cells += 1;

        if (nmr_info.num_of_items > NMR_MAX_SIZE)
        {
          num_cells =  NMR_MAX_SIZE;
        }

        if ((num_cells > 0) && (num_cells <= NMR_MAX_SIZE))
        {
          uint8 idx_dst, idx_src;

          memset((void *) &nmr_list[0], 0, sizeof(nmr_list[0])*NMR_MAX_SIZE);

          p_asn1_cell_info->u.gsmCell->m.nMRPresent = TRUE;
          p_asn1_cell_info->u.gsmCell->nMR.n = num_cells;
          p_asn1_cell_info->u.gsmCell->nMR.elem = &nmr_list[0];

          /* Add serving cell at the end of NMR Info List from GERAN */
          nmr_info.nmr[num_cells-1].arfcn  = rr_cell_info.arfcn.num;
          nmr_info.nmr[num_cells-1].bsic   = rr_cell_info.bsic;
		  #ifndef FEATURE_CGPS_NO_UMTS_TDS
          geran_eng_mode_data_read(ENG_MODE_SCELL_MEASUREMENTS, (void *) &nmr_serving_cell_measurement);
		  #endif
          nmr_info.nmr[num_cells-1].rxlev         = nmr_serving_cell_measurement.rx_lev;

          /* Order NMR from GERAN + serving cell measurement by decreasing channel # */
          for( i = 0; i < num_cells - 1; i++)
          {
            for( j = i + 1; j < num_cells; j++)
            {
              uint16 temp_arfcn;
              uint8  temp_bsic;
              uint8  temp_rxlev;

              if ( nmr_info.nmr[i].arfcn < nmr_info.nmr[j].arfcn )
              {
                temp_arfcn            = nmr_info.nmr[i].arfcn;
                nmr_info.nmr[i].arfcn = nmr_info.nmr[j].arfcn;
                nmr_info.nmr[j].arfcn = temp_arfcn;

                temp_bsic             = nmr_info.nmr[i].bsic;
                nmr_info.nmr[i].bsic  = nmr_info.nmr[j].bsic;
                nmr_info.nmr[j].bsic  = temp_bsic;

                temp_rxlev            = nmr_info.nmr[i].rxlev;
                nmr_info.nmr[i].rxlev = nmr_info.nmr[j].rxlev;
                nmr_info.nmr[j].rxlev = temp_rxlev;
              }
            }
          }  /* Order NMR from GERAN by decreasing channel # */

          /* Translate sorted GERAN NMR list info SUPL NMR list */
          for (idx_dst = 0, idx_src = 0; idx_dst < num_cells;  idx_dst++,  idx_src++)
          {
            nmr_list[idx_dst].aRFCN = (uint32) nmr_info.nmr[idx_src].arfcn;
            nmr_list[idx_dst].bSIC  = (uint32) nmr_info.nmr[idx_src].bsic;
            nmr_list[idx_dst].rxLev = (uint32) nmr_info.nmr[idx_src].rxlev;
          } /* for */
        }   /* num_cells > 0 */
        ret_val = TRUE;
      } /* camped on to GSM, as told by RR */
    } /* When there is no external modem */
  } /* on GSM as told by CM */


  #if defined (FEATURE_CGPS_USES_CDMA) && !defined (FEATURE_GNSS_LOW_MEMORY)
  else if (phone_state_info.srv_system == CGPS_SRV_SYS_CDMA ||
           phone_state_info.srv_system == CGPS_SRV_SYS_HDR)
  {
    mcc_pd_bs_info_type   is801_bs_info;
    memset(&is801_bs_info, 0, sizeof (mcc_pd_bs_info_type) );
   if (mcc_pd_get_bs_info( &is801_bs_info ))

   {
     qword milliseconds;
     uint16 gpsWeek;
     uint32 gpsMsec;
     int32 lat_extend = EXTEND22(is801_bs_info.base_lat);
     int32 long_extend = EXTEND23(is801_bs_info.base_long);
     uint32 base_lat, base_long;
     qword ts;
     base_lat = is801_bs_info.base_lat;
     if(lat_extend <= 0)
     {
       base_lat = lat_extend + 1296000;
     }


     base_long = is801_bs_info.base_long;
     if(long_extend <= 0)
     {
       base_long = long_extend + 2592000;
     }

     MSG_MED("Lat: %d Lat_Extend %d Base_Lat %d", is801_bs_info.base_lat, lat_extend, base_lat);
     MSG_MED("Long: %d Long_Extend %d Base_Long %d", is801_bs_info.base_long, long_extend, base_long);

     memset(&p_cell_info_allocated->u1.cdmaCell, 0, sizeof (supl_CdmaCellInformation) );
     p_asn1_cell_info->u.cdmaCell = &p_cell_info_allocated->u1.cdmaCell;

     p_asn1_cell_info->t = C_SUPL_ASN1_CHOICE_CDMA_CELL;

     p_asn1_cell_info->u.cdmaCell->refNID = is801_bs_info.nid;
     p_asn1_cell_info->u.cdmaCell->refSID = is801_bs_info.sid;
     p_asn1_cell_info->u.cdmaCell->refBASEID= is801_bs_info.base_id;
     p_asn1_cell_info->u.cdmaCell->refBASELAT= base_lat;
     p_asn1_cell_info->u.cdmaCell->reBASELONG = base_long;
     p_asn1_cell_info->u.cdmaCell->refREFPN= is801_bs_info.pilot_pn;
     ts[0] = is801_bs_info.ts[0];
	 ts[1] = is801_bs_info.ts[1];
     gps_total_msec(ts, milliseconds );
     gps_week_ms( milliseconds, &(gpsWeek), &(gpsMsec) );

     p_asn1_cell_info->u.cdmaCell->refWeekNumber= gpsWeek;
     p_asn1_cell_info->u.cdmaCell->refSeconds= gpsMsec/SEC_MSECS ;

     z_cell_info_cached.q_NumCellChangesCachedCellID = 0;

     if(is801_bs_info.stale_info)
     {
        /*Using stale cell id, set num cell change to 2, so as
            to set the uncertainity correctly*/
        z_cell_info_cached.q_NumCellChangesCachedCellID = 2;
     }
     ret_val = TRUE;
   }
   else
   {
     MSG_MED("No Base Station info provided",0,0,0);
   }
  }
  #endif  /* CDMA */

  else if ( phone_state_info.srv_system == CGPS_SRV_SYS_LTE )
  {
    #ifdef FEATURE_CGPS_LTE_CELLDB /* LTE cell info. provided by MGP when this
                                        feature is defined */

    /* Temporarily due to an issue where the TA is not getting correctly updated
        by LTE due to a cell change. always get the fresh CellId info from LTE
        instead of using cached information */
    {

      cgps_CellInfoLTEGlobalStruct z_CellInfoLTEGlobal_local;

      memset(&z_CellInfoLTEGlobal_local, 0, sizeof(cgps_CellInfoLTEGlobalStruct) );

      if ( lte_rrc_get_camped_cell_info_with_primary_plmn(&z_CellInfoLTEGlobal_local.selected_plmn,
                                        &z_CellInfoLTEGlobal_local.freq,
                                        &z_CellInfoLTEGlobal_local.cell_id,
                                        &z_CellInfoLTEGlobal_local.cell_identity,
                                        &z_CellInfoLTEGlobal_local.tac )
         )
      {
        z_cell_info_cached.u_CachedCellIDValid = TRUE;
        z_cell_info_cached.e_ActiveRAT = CELLDB_ACTIVERAT_LTE;
        z_cell_info_cached.q_NumCellChangesCachedCellID = 1;
        memscpy(&z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal,
                sizeof(z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal),
                &z_CellInfoLTEGlobal_local,
                sizeof (z_CellInfoLTEGlobal_local)
              );

        MSG_MED("Direct request to LTE for cell info. successful", 0, 0, 0);
      }
      else
      {
        MSG_HIGH("Direct request to LTE for cell info. failed", 0, 0, 0);
      }

    } /* end if-direct-LTE-request-needed */


    if (  z_cell_info_cached.u_CachedCellIDValid &&
         ( z_cell_info_cached.e_ActiveRAT == CELLDB_ACTIVERAT_LTE )
       )
    {

      if ( (z_supl_session_status.q_supl_version & 0x000000FF) == 2 )
      {

        if ( ( z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.num_mnc_digits <2 ) ||
             ( z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.num_mnc_digits > 3 )
           )
        {
          MSG_HIGH("MNC num. of digit incorrect: %u",
                   z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.num_mnc_digits,
                   0, 0);
          return FALSE;
        }

        memset(&p_cell_info_allocated->u1.ver2_CellInfo_extension, 0, sizeof (supl_Ver2_CellInfo_extension) );
        memset(&p_cell_info_allocated->u2.z_supl_lte_cell_info.z_supl_LteCellInformation, 0, sizeof(supl_LteCellInformation));

        p_asn1_cell_info->t = C_SUPL_ASN1_CHOICE_SUPL2EXT_CELLINFO;
        p_asn1_cell_info->u.ver2_CellInfo_extension = &p_cell_info_allocated->u1.ver2_CellInfo_extension;

        p_cell_info_allocated->u1.ver2_CellInfo_extension.t = C_SUPL_ASN1_CHOICE_LTE_CELL;
        p_cell_info_allocated->u1.ver2_CellInfo_extension.u.lteCell =
          &p_cell_info_allocated->u2.z_supl_lte_cell_info.z_supl_LteCellInformation;

        p_cell_info_allocated->u2.z_supl_lte_cell_info.z_lte_mcc[0] =
          z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.mcc[0];
        p_cell_info_allocated->u2.z_supl_lte_cell_info.z_lte_mcc[1] =
          z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.mcc[1];
        p_cell_info_allocated->u2.z_supl_lte_cell_info.z_lte_mcc[2] =
          z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.mcc[2];

        p_cell_info_allocated->u2.z_supl_lte_cell_info.z_lte_mnc[0] =
          z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.mnc[0];
        p_cell_info_allocated->u2.z_supl_lte_cell_info.z_lte_mnc[1] =
          z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.mnc[1];

        if ( z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.num_mnc_digits == 3 )
        {
          p_cell_info_allocated->u2.z_supl_lte_cell_info.z_lte_mnc[2] =
            z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.mnc[2];
        }


        /* Mandatory: global cell ID, length 28 bits. The CI includes the CSG bit
           p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->cellGlobalIdEUTRA.cellIdentity: ASN1BitStr32
           z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.cell_identity : unit32
        */
        p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->cellGlobalIdEUTRA.cellIdentity.data[0] =
          (uint8)(z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.cell_identity >> 20 ) & 0xFF;

        p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->cellGlobalIdEUTRA.cellIdentity.data[1] =
          (uint8)(z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.cell_identity >> 12 ) & 0xFF;

        p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->cellGlobalIdEUTRA.cellIdentity.data[2] =
          (uint8)(z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.cell_identity >> 4 ) & 0xFF;

        p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->cellGlobalIdEUTRA.cellIdentity.data[3] =
          (uint8)(z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.cell_identity << 4 ) & 0xF0;

        p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->cellGlobalIdEUTRA.cellIdentity.numbits = 28;

        /* Mandatory: physical cell ID: */
        p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->physCellId =
          z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.cell_id;

        /* Mandatory: Tracking Area Code - length 16 bits.
           p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->trackingAreaCode: ASN1BitStr32
           z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.tac : uint16
        */
        p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->trackingAreaCode.data[0] =
          (z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.tac >> 8) & 0xFF;

        p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->trackingAreaCode.data[1] =
          z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.tac & 0xFF;

        p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->trackingAreaCode.numbits = 16;

        /* check if rsrpResult, rsrqResult and TA are available for the serving cell */
        if ( z_lte_mrl.num_cells <= LTE_ML1_GPS_MRL_CELLS_MAX )
        {
          if ( z_lte_mrl.cell_mrl_info[0].phys_cell_id ==
                z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.cell_id )
          {
            /* serving cell info. in the LTE MRL report is valid */
            p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->rsrpResult =
              z_lte_mrl.cell_mrl_info[0].z_rsrp_rsrq.rsrp_result;

            p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->rsrqResult =
              z_lte_mrl.cell_mrl_info[0].z_rsrp_rsrq.rsrq_result;

            p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->m.rsrpResultPresent = TRUE;
            p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->m.rsrqResultPresent = TRUE;

            if ( ( z_lte_mrl.cell_mrl_info[0].timing_advance != 0xFFFF ) &&
                 ( z_lte_mrl.cell_mrl_info[0].timing_advance <= 1282 )
               )
            {
              /* valid TA */
              p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->tA =
                z_lte_mrl.cell_mrl_info[0].timing_advance;
              p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->m.tAPresent = TRUE;
            }
          } /* if ml1 report contains valid serving cell info. */
        } /* LTE MRL api num_cells within max. cell number limit */


        if ( ( z_lte_mrl.num_cells > 1 ) &&
             ( z_lte_mrl.num_cells <= LTE_ML1_GPS_MRL_CELLS_MAX )
           )
        {
          uint8 u_i, u_j;

          memset ( &p_cell_info_allocated->u2.z_supl_lte_cell_info.z_neighbor_EUTRA_link[0], 0,
                   sizeof(supl_MeasResultEUTRA) * (LTE_ML1_GPS_MRL_CELLS_MAX-1) );

          for ( u_i = 0; u_i < ( z_lte_mrl.num_cells - 1 ); u_i++)
          {
            u_j = u_i+1;
            if ( u_j < z_lte_mrl.num_cells )
            {

              p_cell_info_allocated->u2.z_supl_lte_cell_info.z_neighbor_EUTRA_link[u_i].physCellId =
                z_lte_mrl.cell_mrl_info[u_j].phys_cell_id;

              p_cell_info_allocated->u2.z_supl_lte_cell_info.z_neighbor_EUTRA_link[u_i].measResult.m.rsrpResultPresent = 1;
              p_cell_info_allocated->u2.z_supl_lte_cell_info.z_neighbor_EUTRA_link[u_i].measResult.m.rsrqResultPresent = 1;

              p_cell_info_allocated->u2.z_supl_lte_cell_info.z_neighbor_EUTRA_link[u_i].measResult.rsrpResult =
                z_lte_mrl.cell_mrl_info[u_j].z_rsrp_rsrq.rsrp_result;

              p_cell_info_allocated->u2.z_supl_lte_cell_info.z_neighbor_EUTRA_link[u_i].measResult.rsrqResult =
                z_lte_mrl.cell_mrl_info[u_j].z_rsrp_rsrq.rsrq_result;

            }
          }
        } /* LTE report contains sensible number of cells */

        p_asn1_cell_info->u.ver2_CellInfo_extension = &p_cell_info_allocated->u1.ver2_CellInfo_extension;

        p_cell_info_allocated->u1.ver2_CellInfo_extension.u.lteCell =
          &p_cell_info_allocated->u2.z_supl_lte_cell_info.z_supl_LteCellInformation;

        p_cell_info_allocated->u1.ver2_CellInfo_extension.t = C_SUPL_ASN1_CHOICE_LTE_CELL;

        p_cell_info_allocated->u2.z_supl_lte_cell_info.z_supl_LteCellInformation.cellGlobalIdEUTRA.plmn_Identity.m.mccPresent = 1;

        p_cell_info_allocated->u2.z_supl_lte_cell_info.z_supl_LteCellInformation.cellGlobalIdEUTRA.plmn_Identity.mcc.elem =
          &p_cell_info_allocated->u2.z_supl_lte_cell_info.z_lte_mcc[0];

        p_cell_info_allocated->u2.z_supl_lte_cell_info.z_supl_LteCellInformation.cellGlobalIdEUTRA.plmn_Identity.mcc.n = 3;

        p_cell_info_allocated->u2.z_supl_lte_cell_info.z_supl_LteCellInformation.cellGlobalIdEUTRA.plmn_Identity.mnc.elem =
          &p_cell_info_allocated->u2.z_supl_lte_cell_info.z_lte_mnc[0];

        if ( z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.num_mnc_digits == 3 )
        {
          p_cell_info_allocated->u2.z_supl_lte_cell_info.z_supl_LteCellInformation.cellGlobalIdEUTRA.plmn_Identity.mnc.n = 3;
        }
        else
        {
          p_cell_info_allocated->u2.z_supl_lte_cell_info.z_supl_LteCellInformation.cellGlobalIdEUTRA.plmn_Identity.mnc.n = 2;
        }

        if ( z_lte_mrl.num_cells > 1 )
        {
          p_cell_info_allocated->u2.z_supl_lte_cell_info.z_supl_LteCellInformation.m.measResultListEUTRAPresent = 1;

          p_cell_info_allocated->u2.z_supl_lte_cell_info.z_supl_LteCellInformation.measResultListEUTRA.elem =
            &p_cell_info_allocated->u2.z_supl_lte_cell_info.z_neighbor_EUTRA_link[0];

          p_cell_info_allocated->u2.z_supl_lte_cell_info.z_supl_LteCellInformation.measResultListEUTRA.n =
            z_lte_mrl.num_cells - 1; /* serving cell excluded */
        }

        ret_val = TRUE;

      }  /* if SUPL2 */

    } /* if valid plmn and cell_id available */

    #endif /* FEATURE_CGPS_LTE_CELLDB */

  } /* on LTE as told by CM and MGP has provided valid LTE cell DB */

  #ifdef FEATURE_SUPL1_TDS /* defined in custcgps.h, if .builds defines FEATURE_CGPS_USES_TDS */
  else if (phone_state_info.srv_system == CGPS_SRV_SYS_TDSCDMA)
  {
    memset(&tds_plmn_cell_info, 0, sizeof(tdsrrcmeas_plmn_cell_info_struct_type));

    if (gnss_wwan_iface_tdsrrcmeas_get_cell_plmn_info(&tds_plmn_cell_info))
    {
      MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "TDSCDMA cell info request returns valid CI: %u, LocID Cfg=%d",
            tds_plmn_cell_info.cell_id, (tm_core_info.config_info.q_gnss_oem_feature_mask & NV_GNSS_OEM_FEATURE_MASK_14_SUPL_TDS_LOCID_CFG));

      /* Check OEM DRE NV bit to determine compliance of Cell Info required for specific SUPL server access */
      if (tm_core_info.config_info.q_gnss_oem_feature_mask & NV_GNSS_OEM_FEATURE_MASK_14_SUPL_TDS_LOCID_CFG)
      {
        /* SUPL2.0 compliant treatment of TDSCDMA */
        /* if not provisioned as SUPL2, return FALSE */
        if (!supl_cell_info_construct_tds_wcdma(p_asn1_cell_info,
                                                p_cell_info_allocated,
                                                tds_plmn_cell_info.cell_id,
                                                tds_plmn_cell_info.plmn_id))
        {
          return FALSE;
        }
      }
      else
      {
        /* SUPL1.0 special non-compliant treatment of TDSCDMA */
        /* if not provisioned as SUPL1, return FALSE */
        if (!supl_cell_info_construct_tds_gsm(p_asn1_cell_info,
                                              p_cell_info_allocated,
                                              tds_plmn_cell_info.cell_id,
                                              tds_plmn_cell_info.lac[0],
                                              tds_plmn_cell_info.lac[1],
                                              tds_plmn_cell_info.plmn_id))
      {
        return FALSE;
      }
      }

      z_cell_info_cached.q_NumCellChangesCachedCellID = 0;
      ret_val = TRUE;
    } /* direct TDS-API returns valid cell info. */

    else
    {
      if (z_cell_info_cached.u_CachedCellIDValid)
      {
        if (z_cell_info_cached.e_ActiveRAT == CELLDB_ACTIVERAT_TDSCDMA)
        {
          MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Cached cell db contains TDSCDMA valid CI: %u, LocID Cfg=%d",
                z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.cell_id,
                (tm_core_info.config_info.q_gnss_oem_feature_mask & NV_GNSS_OEM_FEATURE_MASK_14_SUPL_TDS_LOCID_CFG));

          /* Check OEM DRE NV bit to determine compliance of Cell Info required for specific SUPL server access */
          if (tm_core_info.config_info.q_gnss_oem_feature_mask & NV_GNSS_OEM_FEATURE_MASK_14_SUPL_TDS_LOCID_CFG)
          {
            /* SUPL2.0 compliant treatment of TDSCDMA */
            /* if not provisioned as SUPL2, return FALSE */
            if (!supl_cell_info_construct_tds_wcdma(p_asn1_cell_info,
                                                    p_cell_info_allocated,
                                                    z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.cell_id,
                                                    z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.plmn_id))
            {
              return FALSE;
            }
          }
          else
          {
            /* SUPL1.0 special non-compliant treatment of TDSCDMA */
            /* if not provisioned as SUPL1, return FALSE */
            if (!supl_cell_info_construct_tds_gsm(p_asn1_cell_info,
                                                  p_cell_info_allocated,
                                                  z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.cell_id,
                                                  z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.lac[0],
                                                  z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.lac[1],
                                                  z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoTDSGlobal.plmn_id))
          {
            return FALSE;
          }
          }

          /* borrow the concept of this variable being greater than 1 meaning stale cell info */
          z_cell_info_cached.q_NumCellChangesCachedCellID = 2;
          ret_val = TRUE;
        } /* cached cell info is of TDS */

        else /* cached cell DB contains valid but non-TDS */
        {
          MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Cached cell db contains valid but non-TDSCDMA valid CI: %u",
                z_cell_info_cached.e_ActiveRAT);
        }
      } /* cached cell info valid */

      else /* no valid cached cell db */
      {
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "No valid cached cell db: RAT = %u",
              z_cell_info_cached.e_ActiveRAT);
      }
    } /* else: TDS API fails to return valid cell info */
  } /* on TDS as told by CM */
  #endif /* if FEATURE_SUPL1_TDS defined */

  return ret_val;
}


/********************************************************************
 *
 * supl_chk_and_build_all_mlid
 *
 * Function description:
 *  utility function to construct multiple location ID used by several
 *  SUPL messages.
 *
 * Parameters:
 *   p_supl_LocationIdData - pointer to a dynamically allocated array
 *      of structure of supl_LocationIdData.
 *
 *  p_supl_LocationId -- pointer to a dynamically allocated array
 *      of structure of supl_LocationData.
 *
 *  p_cell_info_allocated - pointer to a dynamically allocated array
 *     of structure supl_cell_info_struct_type.
 *
 *  u_num_allocated_array_elements - number of array elements allocated
 *     for each of the above three arrays.
 *
 * Return: number of Multiple Location ID entries
 *
 *********************************************************************
 */

static uint8  supl_chk_and_build_all_mlid (supl_LocationIdData *p_supl_LocationIdData,
                                           supl_LocationId *p_supl_LocationID,
                                           supl_cell_info_struct_type* p_cell_info_allocated,
                                           uint8 u_num_allocated_array_elements)
{
  uint32 q_current_time_sec;
  uint8 u_i, u_num_mlid;
  uint8 u_mlid_buffer_array_size;
  uint32 q_loc_id_age;

  mlid_buffer_strcut_type *p_multi_lid_buffer;


  p_multi_lid_buffer = &z_mlid_buffer[0];
  u_num_mlid = 0;
  u_mlid_buffer_array_size = ARR_SIZE(z_mlid_buffer);

  q_current_time_sec = time_get_secs();

  for (u_i=0; u_i<u_mlid_buffer_array_size; u_i++)
  {
    if ( p_multi_lid_buffer->loc_id.u_CachedCellIDValid == TRUE )
    {
      /* check age and clear the slot if expired */
      q_loc_id_age = q_current_time_sec - p_multi_lid_buffer->q_timestamp;

      if ( q_loc_id_age > C_MAX_NUM_CELL_HIST_SEC )
      {
        memset(p_multi_lid_buffer, 0, sizeof(mlid_buffer_strcut_type) );
        MSG_4( MSG_SSID_GPSSM, MSG_LEGACY_HIGH,
               "MLID %u obsoleted: Age= %lu, CurrentT=%lu, timestamp=%lu",
               u_i, q_loc_id_age, q_current_time_sec, p_multi_lid_buffer->q_timestamp );
        if ( u_supl_global_num_mlid > 0 )
        {
          u_supl_global_num_mlid--;
        }
      }
      else
      {
        if ( ( p_multi_lid_buffer->is_the_current_serving_cell == FALSE ) &&
             ( u_num_mlid < u_num_allocated_array_elements )
           )
        {

          p_supl_LocationIdData->m.relativetimestampPresent = TRUE;
          p_supl_LocationIdData->relativetimestamp = q_loc_id_age * 100;
          p_supl_LocationIdData->locationId = p_supl_LocationID;
          p_supl_LocationIdData->servingFlag = FALSE;

          if ( p_multi_lid_buffer->loc_id.e_ActiveRAT == CELLDB_WIFI  )
          {
            p_supl_LocationID->status = supl_Status_current;
          }
          else
          {
            p_supl_LocationID->status = supl_Status_stale; /* for WWAN, only previous cells reported */
          }

          /* populate the actual location id */
          supl_mlid_construct(&p_supl_LocationID->cellInfo,
                              p_cell_info_allocated, u_i);

          u_num_mlid++;

          if ( u_num_mlid < C_MAX_MLID_NUM )
          {
            if ( u_num_mlid < u_num_allocated_array_elements  )
            {
              /* buffers only allocated for enough for valid mlid:
                 u_num_allocated_array_elements = ( u_supl_global_num_mlid - 1) */
              p_supl_LocationIdData++;
              p_supl_LocationID++;
              p_supl_LocationIdData->locationId = p_supl_LocationID;
              p_cell_info_allocated++;
            }
          }
          else  /* should not get to this condition, just a safeguard */
          {
            return C_MAX_MLID_NUM;
          }
        } /* if not the current serving cell, report */

        else
        {
          if ( p_multi_lid_buffer->is_the_current_serving_cell != FALSE )
          {
            /* this slot contains the current serving cell, don't report */
            MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "MLID: skip current serving cell at slot: %u ",
                  u_i);
          }
          else if ( u_num_mlid >= u_num_allocated_array_elements )
          {
            /* for some reason there has been serving cell mistakenly marked as non-serving due to
               time stamp mechanism malfunctioning; skip it */
            MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "MLID: no more allocated memory for the cell, skip: %u ",
                  u_i);
          }

        }
      }
    }

    p_multi_lid_buffer++;
  }

  return u_num_mlid;
}


/********************************************************************
 *
 * supl_mlid_construct
 *
 * Function description:
 *  utility function to populate the structure of one supl_CellInfo in MLID.
 *
 * Parameters:
 *   p_asn1_cell_info - pointer to a structure to hold the to-be-constructed
 *       cell information.
 *
 *   p_cell_info_allocated -- pointer to a structure that contains memory
 *      allocated for the ASN1 tool
 * Return: TRUE/FALSE ( FALSE: no valid cell ID )
 *********************************************************************
 */
static boolean supl_mlid_construct(supl_CellInfo* p_asn1_cell_info,
                                   supl_cell_info_struct_type* p_cell_info_allocated,
                                   uint8 u_mlid_buffer_index)
{

  byte                     *p_gsm_identity;

  #ifdef FEATURE_CGPS_UMTS_CELLDB
  rrc_plmn_identity_type   *p_wcdma_plmn_identity = NULL;
  rrc_int_cell_id_type     *p_wcdma_int_cell_id   = NULL;
  #endif

  #ifdef FEATURE_CGPS_LTE_CELLDB /* LTE cell info. provided by MGP when this
                                        feature is defined */
  cgps_CellInfoLTEGlobalStruct *p_lte_cell_id = NULL;
  #endif

  boolean ret_val = FALSE;
  uint32 q_test1, q_test2;


  if ( u_mlid_buffer_index >= ARR_SIZE(z_mlid_buffer) )
    return FALSE;


  /* Initialize ASN.1 Cell ID Structure to zeroes */
  memset( p_asn1_cell_info, 0, sizeof( supl_CellInfo ) );

  if  ( z_mlid_buffer[u_mlid_buffer_index].loc_id.e_ActiveRAT == CELLDB_ACTIVERAT_WCDMA )
  {

    #ifdef FEATURE_CGPS_UMTS_CELLDB /* WCDMA cell info. provided by MGP when
                                       this feature defined */

    p_wcdma_plmn_identity = &z_mlid_buffer[u_mlid_buffer_index].loc_id.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id;
    p_wcdma_int_cell_id   = &z_mlid_buffer[u_mlid_buffer_index].loc_id.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.cell_id;

    memset(&p_cell_info_allocated->u1.wcdmaCell, 0, sizeof (supl_WcdmaCellInformation) );

    p_asn1_cell_info->t = C_SUPL_ASN1_CHOICE_WCDMA_CELL;
    p_asn1_cell_info->u.wcdmaCell = &p_cell_info_allocated->u1.wcdmaCell;
    p_asn1_cell_info->u.wcdmaCell->frequencyInfo.modeSpecificInfo.u.fdd =
      &p_cell_info_allocated->u2.z_supl_wcdma_cell_info.z_frequency_info_fdd;

    /* MCC/MNC: convert to integers */
    if ( !tm_umts_supl_wcdma_plmn_id_organizer ( p_wcdma_plmn_identity,
                                                 &q_test1,
                                                 &q_test2
                                               )
       )
    {
      return FALSE;
    }
    p_cell_info_allocated->u1.wcdmaCell.refMCC = (uint16)q_test1;
    p_cell_info_allocated->u1.wcdmaCell.refMNC = (uint16)q_test2;

    /* Cell ID */
    p_asn1_cell_info->u.wcdmaCell->refUC = *p_wcdma_int_cell_id;

    /* frequency info */
    p_asn1_cell_info->u.wcdmaCell->m.frequencyInfoPresent = 1;
    p_asn1_cell_info->u.wcdmaCell->frequencyInfo.modeSpecificInfo.t =
      C_SUPL_ASN1_CHOICE_FREQINFO_MODE_SPECIFIC_INFO_FDD;
    p_asn1_cell_info->u.wcdmaCell->frequencyInfo.modeSpecificInfo.u.fdd->m.uarfcn_ULPresent = 0;
    p_asn1_cell_info->u.wcdmaCell->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL =
     z_mlid_buffer[u_mlid_buffer_index].loc_id.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.freq;
    /* scrambling code */
    p_asn1_cell_info->u.wcdmaCell->m.primaryScramblingCodePresent = 1;
    p_asn1_cell_info->u.wcdmaCell->primaryScramblingCode =
     z_mlid_buffer[u_mlid_buffer_index].loc_id.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.psc;

    ret_val = TRUE;

    #endif /* FEATURE_CGPS_UMTS_CELLDB */

  } /* on WCDMA as told by CM and having valid cell ID as told by MGP */


  else if ( z_mlid_buffer[u_mlid_buffer_index].loc_id.e_ActiveRAT == CELLDB_ACTIVERAT_LTE )
  {
    #ifdef FEATURE_CGPS_LTE_CELLDB /* LTE cell info. provided by MGP when this
                                        feature is defined */

    p_lte_cell_id = &z_mlid_buffer[u_mlid_buffer_index].loc_id.z_CellInfoGlobalCached.z_CellInfoLTEGlobal;

    {

      if ( (z_supl_session_status.q_supl_version & 0x000000FF) == 2 )
      {

        if ( ( p_lte_cell_id->selected_plmn.num_mnc_digits <2 ) ||
             ( p_lte_cell_id->selected_plmn.num_mnc_digits > 3 )
           )
        {
          MSG_HIGH("MNC num. of digit incorrect: %u",
                   p_lte_cell_id->selected_plmn.num_mnc_digits,
                   0, 0);
          return FALSE;
        }

        memset(&p_cell_info_allocated->u1.ver2_CellInfo_extension, 0, sizeof (supl_Ver2_CellInfo_extension) );
        memset(&p_cell_info_allocated->u2.z_supl_lte_cell_info.z_supl_LteCellInformation, 0, sizeof(supl_LteCellInformation));

        p_asn1_cell_info->t = C_SUPL_ASN1_CHOICE_SUPL2EXT_CELLINFO;
        p_asn1_cell_info->u.ver2_CellInfo_extension = &p_cell_info_allocated->u1.ver2_CellInfo_extension;

        p_cell_info_allocated->u1.ver2_CellInfo_extension.t = C_SUPL_ASN1_CHOICE_LTE_CELL;
        p_cell_info_allocated->u1.ver2_CellInfo_extension.u.lteCell =
          &p_cell_info_allocated->u2.z_supl_lte_cell_info.z_supl_LteCellInformation;

        p_cell_info_allocated->u2.z_supl_lte_cell_info.z_lte_mcc[0] =
          p_lte_cell_id->selected_plmn.mcc[0];
        p_cell_info_allocated->u2.z_supl_lte_cell_info.z_lte_mcc[1] =
          p_lte_cell_id->selected_plmn.mcc[1];
        p_cell_info_allocated->u2.z_supl_lte_cell_info.z_lte_mcc[2] =
          p_lte_cell_id->selected_plmn.mcc[2];

        p_cell_info_allocated->u2.z_supl_lte_cell_info.z_lte_mnc[0] =
          p_lte_cell_id->selected_plmn.mnc[0];
        p_cell_info_allocated->u2.z_supl_lte_cell_info.z_lte_mnc[1] =
          p_lte_cell_id->selected_plmn.mnc[1];

        if ( p_lte_cell_id->selected_plmn.num_mnc_digits == 3 )
        {
          p_cell_info_allocated->u2.z_supl_lte_cell_info.z_lte_mnc[2] =
            p_lte_cell_id->selected_plmn.mnc[2];
        }


        /* Mandatory: global cell ID, length 28 bits. The CI includes the CSG bit
           p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->cellGlobalIdEUTRA.cellIdentity: ASN1BitStr32
           z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.cell_identity : unit32
        */
        p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->cellGlobalIdEUTRA.cellIdentity.data[0] =
          (uint8)(p_lte_cell_id->cell_identity >> 20 ) & 0xFF;

        p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->cellGlobalIdEUTRA.cellIdentity.data[1] =
          (uint8)(p_lte_cell_id->cell_identity >> 12 ) & 0xFF;

        p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->cellGlobalIdEUTRA.cellIdentity.data[2] =
          (uint8)(p_lte_cell_id->cell_identity >> 4 ) & 0xFF;

        p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->cellGlobalIdEUTRA.cellIdentity.data[3] =
          (uint8)(p_lte_cell_id->cell_identity << 4 ) & 0xF0;

        p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->cellGlobalIdEUTRA.cellIdentity.numbits = 28;

        /* Mandatory: physical cell ID: */
        p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->physCellId =
          p_lte_cell_id->cell_id;

        /* Mandatory: Tracking Area Code - length 16 bits.
           p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->trackingAreaCode: ASN1BitStr32
           z_cell_info_cached.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.tac : uint16
        */
        p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->trackingAreaCode.data[0] =
          (p_lte_cell_id->tac >> 8) & 0xFF;

        p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->trackingAreaCode.data[1] =
          p_lte_cell_id->tac & 0xFF;

        p_asn1_cell_info->u.ver2_CellInfo_extension->u.lteCell->trackingAreaCode.numbits = 16;

        p_asn1_cell_info->u.ver2_CellInfo_extension = &p_cell_info_allocated->u1.ver2_CellInfo_extension;

        p_cell_info_allocated->u1.ver2_CellInfo_extension.u.lteCell =
          &p_cell_info_allocated->u2.z_supl_lte_cell_info.z_supl_LteCellInformation;

        p_cell_info_allocated->u1.ver2_CellInfo_extension.t = C_SUPL_ASN1_CHOICE_LTE_CELL;

        p_cell_info_allocated->u2.z_supl_lte_cell_info.z_supl_LteCellInformation.cellGlobalIdEUTRA.plmn_Identity.m.mccPresent = 1;

        p_cell_info_allocated->u2.z_supl_lte_cell_info.z_supl_LteCellInformation.cellGlobalIdEUTRA.plmn_Identity.mcc.elem =
          &p_cell_info_allocated->u2.z_supl_lte_cell_info.z_lte_mcc[0];

        p_cell_info_allocated->u2.z_supl_lte_cell_info.z_supl_LteCellInformation.cellGlobalIdEUTRA.plmn_Identity.mcc.n = 3;

        p_cell_info_allocated->u2.z_supl_lte_cell_info.z_supl_LteCellInformation.cellGlobalIdEUTRA.plmn_Identity.mnc.elem =
          &p_cell_info_allocated->u2.z_supl_lte_cell_info.z_lte_mnc[0];

        if ( p_lte_cell_id->selected_plmn.num_mnc_digits == 3 )
        {
          p_cell_info_allocated->u2.z_supl_lte_cell_info.z_supl_LteCellInformation.cellGlobalIdEUTRA.plmn_Identity.mnc.n = 3;
        }
        else
        {
          p_cell_info_allocated->u2.z_supl_lte_cell_info.z_supl_LteCellInformation.cellGlobalIdEUTRA.plmn_Identity.mnc.n = 2;
        }

        ret_val = TRUE;

      }  /* if SUPL2 */

    } /* if valid plmn and cell_id available */

    #endif /* FEATURE_CGPS_LTE_CELLDB */

  } /* on LTE as told by CM and MGP has provided valid LTE cell DB */

  else if  ( z_mlid_buffer[u_mlid_buffer_index].loc_id.e_ActiveRAT == CELLDB_ACTIVERAT_GSM )
  {
    p_asn1_cell_info->t = C_SUPL_ASN1_CHOICE_GSM_CELL;
    p_asn1_cell_info->u.gsmCell = &p_cell_info_allocated->u1.gsmCell;

    p_cell_info_allocated->u1.gsmCell.refCI =
      z_mlid_buffer[u_mlid_buffer_index].loc_id.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.cell_identity;

    p_asn1_cell_info->u.gsmCell->refLAC =
      z_mlid_buffer[u_mlid_buffer_index].loc_id.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.location_area_identification.location_area_code;

    p_gsm_identity =
      &z_mlid_buffer[u_mlid_buffer_index].loc_id.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.location_area_identification.plmn_id.identity[0];

    /* convert MCC to an integer */
    p_asn1_cell_info->u.gsmCell->refMCC = (p_gsm_identity[0] & 0x0F) * 100;
    p_asn1_cell_info->u.gsmCell->refMCC += (( p_gsm_identity[0] >> 4 ) & 0x0F) * 10;
    p_asn1_cell_info->u.gsmCell->refMCC += p_gsm_identity[1] & 0x0F;

    /* MNC */
    /* assume there will be 3 digits, so put the 1st 2 digits in the */
    /* 100s and 10s place. */
    p_asn1_cell_info->u.gsmCell->refMNC = (p_gsm_identity[2] & 0x0F) * 100;
    p_asn1_cell_info->u.gsmCell->refMNC += (( p_gsm_identity[2] >> 4 ) & 0x0F) * 10;

    /* is MNC3 present? */
    /* add in the 3rd digit if there is one */
    if ( ( ( p_gsm_identity[1] >> 4 ) & 0x0F ) != 0x0F )
    {
      p_asn1_cell_info->u.gsmCell->refMNC += ( p_gsm_identity[1] >> 4 ) & 0x0F;
    }
    else
    {
      /* divide by 10 if there are only 2 digits in the mnc */
      p_asn1_cell_info->u.gsmCell->refMNC /= 10;
    }


  }

  else if ( z_mlid_buffer[u_mlid_buffer_index].loc_id.e_ActiveRAT == CELLDB_WIFI )
  {

    memset(&p_cell_info_allocated->u1.ver2_CellInfo_extension, 0, sizeof (supl_Ver2_CellInfo_extension) );
    memset(&p_cell_info_allocated->u2.z_supl_wlan_ap_info, 0, sizeof(supl_WlanAPInformation));

    p_asn1_cell_info->t = C_SUPL_ASN1_CHOICE_SUPL2EXT_CELLINFO;
    p_asn1_cell_info->t = T_supl_CellInfo_ver2_CellInfo_extension;
    p_asn1_cell_info->u.ver2_CellInfo_extension = &p_cell_info_allocated->u1.ver2_CellInfo_extension;


    p_cell_info_allocated->u1.ver2_CellInfo_extension.t = C_SUPL_ASN1_CHOICE_WLAN_CELL;

    p_cell_info_allocated->u1.ver2_CellInfo_extension.u.wlanAP = &p_cell_info_allocated->u2.z_supl_wlan_ap_info;
    p_cell_info_allocated->u2.z_supl_wlan_ap_info.m.apSignalStrengthPresent = 1;

    p_cell_info_allocated->u2.z_supl_wlan_ap_info.apMACAddress.numbits = 48;
    p_cell_info_allocated->u2.z_supl_wlan_ap_info.apMACAddress.data[5] =
      z_mlid_buffer[u_mlid_buffer_index].loc_id.z_CellInfoGlobalCached.z_wifi_ap_cell_info.mac_address[5];
    p_cell_info_allocated->u2.z_supl_wlan_ap_info.apMACAddress.data[4] =
      z_mlid_buffer[u_mlid_buffer_index].loc_id.z_CellInfoGlobalCached.z_wifi_ap_cell_info.mac_address[4];
    p_cell_info_allocated->u2.z_supl_wlan_ap_info.apMACAddress.data[3] =
      z_mlid_buffer[u_mlid_buffer_index].loc_id.z_CellInfoGlobalCached.z_wifi_ap_cell_info.mac_address[3];
    p_cell_info_allocated->u2.z_supl_wlan_ap_info.apMACAddress.data[2] =
      z_mlid_buffer[u_mlid_buffer_index].loc_id.z_CellInfoGlobalCached.z_wifi_ap_cell_info.mac_address[2];
    p_cell_info_allocated->u2.z_supl_wlan_ap_info.apMACAddress.data[1] =
      z_mlid_buffer[u_mlid_buffer_index].loc_id.z_CellInfoGlobalCached.z_wifi_ap_cell_info.mac_address[1];
    p_cell_info_allocated->u2.z_supl_wlan_ap_info.apMACAddress.data[0] =
      z_mlid_buffer[u_mlid_buffer_index].loc_id.z_CellInfoGlobalCached.z_wifi_ap_cell_info.mac_address[0];

    p_cell_info_allocated->u2.z_supl_wlan_ap_info.apSignalStrength =
      z_mlid_buffer[u_mlid_buffer_index].loc_id.z_CellInfoGlobalCached.z_wifi_ap_cell_info.ap_rssi;

  }

  return ret_val;
}


/********************************************************************
*
* supl_set_cf_state
*
* Function description:
*   this function properly sets SUPL call flow state machine states
*
* Parameters:
*  e_supl_cf_state [IN] -- SUPL call flow state to set to.
*  u_supl_cf_lm_timer_ms [IN] -- Set this given time for SUPL call flow timer.
*
* Return:
*  None
*
*********************************************************************
*/
void supl_set_cf_state(supl_cf_state_enum_type e_new_supl_cf_state, uint32 u_supl_cf_lm_timer_ms )
{
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;

  int16 x_temp = 0;
  uint8 u_TransmitDelayAdjRespTime = 0;
  uint8 u_GnssEngRespTime = 0;
  uint8 u_temp2 = 0;


  if ( e_new_supl_cf_state == C_SUPL_CF_STATE_NULL )
    return;

  /* note C_SUPL_CF_STATE_WIFI_WAIT_TIMER_NO_TCP state flag could be requested while in UT0, so
     don't terminate UT0 here */
  if (e_new_supl_cf_state != C_SUPL_CF_STATE_WIFI_WAIT_TIMER_NO_TCP)
  {
  (void) os_TimerStop( supl_cf_timer ); /* stop any current running timer */
  }

  if ( e_new_supl_cf_state & C_SUPL_CF_STATE_CELL_INFO_REQ )
  {
    (void) os_TimerStart(supl_cf_timer, 3000, NULL ); /* hardcode timer to 3 sec. */
  }

  else if ( ( e_new_supl_cf_state & C_SUPL_CF_STATE_UT0_SUPLSTART ) ||
            ( e_new_supl_cf_state & C_SUPL_CF_STATE_UT0_SUPLPOSINIT ) ||
            ( e_new_supl_cf_state & C_SUPL_CF_STATE_UT0_SUPLEND ) ||
            ( e_new_supl_cf_state & C_SUPL_CF_STATE_UT0_SUPLREPORT_QUERY )
          )
  {
    /* "UT0": waiting for TLS link to establish  */
    (void) os_TimerStart(supl_cf_timer, SUPL_TIMER_UT0_VALUE_MSEC, NULL);
  }

  else if ( e_new_supl_cf_state & C_SUPL_CF_STATE_UT1 )
  {
    /* use the NV value for UT1 */
    (void) os_TimerStart(supl_cf_timer, z_umts_nv_item_ram_copy.aagps_ut1_value * 1000, NULL);
  }

  else if ( ( e_new_supl_cf_state & C_SUPL_CF_STATE_UT2 )||
            ( e_new_supl_cf_state & C_SUPL_CF_STATE_UT10_INFO_QUERY_END )
          )
  {
    (void) os_TimerStart(supl_cf_timer, z_umts_nv_item_ram_copy.aagps_ut2_value * 1000, NULL);
  }

  else if ( ( e_new_supl_cf_state & C_SUPL_CF_STATE_UT3 ) ||
            ( e_new_supl_cf_state & C_SUPL_CF_STATE_UT3_RRLP ) ||
            ( e_new_supl_cf_state & C_SUPL_CF_STATE_UT3_ACK )
          )
  {
    (void) os_TimerStart(supl_cf_timer, z_umts_nv_item_ram_copy.aagps_ut3_value * 1000, NULL);
  }

  else if ( e_new_supl_cf_state & C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER )
  {
    /* Calling function knows what supl call flow/LM timer to set? */
    if ( u_supl_cf_lm_timer_ms != 0)
    {
      MSG_HIGH("SUPL CF timer = %d", u_supl_cf_lm_timer_ms, 0, 0);
      (void)os_TimerStart(supl_cf_timer, u_supl_cf_lm_timer_ms, NULL);
    }
    else
  {
    /* make it one second earlier, for better chance of being received by SLP; and
       to avoid race condition with LM timeout */
    if ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_RRLP )
    {
      x_temp = p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.u_measure_response_time_sec - 1;
    }
    else if ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP )
    {
      x_temp = p_supl_session_status->z_network_assist.radio_link_prot_params.z_lpp_params.u_measure_response_time_sec - 1;
      }

      if ( x_temp >= 0 )
      {
        u_TransmitDelayAdjRespTime = (uint8)x_temp;
      }
      else
      {
        u_TransmitDelayAdjRespTime = 0;
    }


    /* at this point u_gps_session_timeout_value_secs is set to the min of QoS timeout from UI
       and (u_measure_respone_time_sec - 2)*/
      u_GnssEngRespTime = p_supl_session_status->z_session_qos.u_gps_session_timeout_value_secs;
    /* use the smaller of the timers for SUPL so we can send a non-DONE mesaurement
       also ensure  supl timer is atleast greater than the LM timer to avoid race condition */
      if ( u_TransmitDelayAdjRespTime < u_GnssEngRespTime )
    {
        u_temp2 = u_TransmitDelayAdjRespTime;
    }
    else
    {
        /* make sure SUPL timer is atleast 1 second longer than the GNSSEngine/LM-timer but less
           than the N/W response time */
          if ( (u_TransmitDelayAdjRespTime - u_GnssEngRespTime) > 1 )
        {
            u_temp2 = u_GnssEngRespTime + 1;
        }
        else
        {
            u_temp2 = u_TransmitDelayAdjRespTime;
        }
    }
      MSG_HIGH("SUPL LM PRM Timer= %d, gps_sess_timer = %d, SUPL CF timer= %d", u_TransmitDelayAdjRespTime, u_GnssEngRespTime, u_temp2);
      if ( 
           #ifdef FEATURE_LTE
           (TRUE == b_lte_ota_otdoa_reset_flag) && 
           #endif /* FEATURE_LTE */
           (C_SUPL_OTDOA_MODE == p_supl_session_status->u_agps_mode) )
    {
      /* Set CF timer T-200 msec timeout for LTE OTA OTDOA reset command */
      (void)os_TimerStart(supl_cf_timer, ((p_supl_session_status->z_network_assist.radio_link_prot_params.z_lpp_params.u_measure_response_time_sec * 1000) - 200), NULL);
    }
    else
    {
      (void)os_TimerStart(supl_cf_timer, u_temp2 * 1000, NULL);
    }
  }
  }
  else if ( e_new_supl_cf_state & C_SUPL_CF_STATE_OTDOA_MEAS_REQ )
  {
    (void) os_TimerStop( supl_lpp_otdoa_req_timer );
    (void) os_TimerStart( supl_lpp_otdoa_req_timer, C_LPP_OTDOA_REQ_MSEC, NULL );
  }
  else if ( ( e_new_supl_cf_state & C_SUPL_CF_STATE_WIFI_WAIT_TIMER_NO_TCP ) ||
            ( e_new_supl_cf_state & C_SUPL_CF_STATE_WIFI_WAIT_SUPLSTART ) )
  {
    /* note C_SUPL_CF_STATE_WIFI_WAIT_SUPLSTART only sets timer if system never entered C_SUPL_CF_STATE_WIFI_WAIT_TIMER_NO_TCP */
    (void) os_TimerStart(supl_wifi_wait_timer, tm_core_info.config_info.wifi_wait_timeout_sec * 1000, NULL );
  }
  else if ( e_new_supl_cf_state & C_SUPL_CF_STATE_OTDOA_MUL_MEAS_RECV )
  {
    (void)os_TimerStop(supl_lpp_otdoa_mul_meas_timer);
    (void)os_TimerStart(supl_lpp_otdoa_mul_meas_timer, u_supl_cf_lm_timer_ms, NULL);
  }
  /*Start the early response timer, record the CF State and exit from here. 
    Final time out CF will be OR'ed with this. Just clear the early response timer
    when it expires and clear the State. Dont touch the Final response time/state.*/
  else if(e_new_supl_cf_state ==  C_SUPL_CF_STATE_OTDOA_EARLY_RESPONSE_REQ)
  {
     (void) os_TimerStart(supl_lpp_early_response_timer,u_supl_cf_lm_timer_ms, NULL ); 
  }
  p_supl_session_status->e_cf_state |= e_new_supl_cf_state;
  MSG_MED( "SUPL CF State: 0x%x", p_supl_session_status->e_cf_state, 0, 0);
}




/*
******************************************************************************
* supl_store_slp_sessionId
*
* Function description:
*   Stores the incoming SLP session ID
*
* Parameters:
*  pointer to supl_ULP_PDU
*
* Return value:
*  None
*
*
******************************************************************************
*/
static void supl_store_slp_sessionId(const supl_ULP_PDU* p_pdu )

{
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;
  supl_session_id      *p_session_id  = &p_supl_session_status->z_session_id;

  if (p_pdu->sessionID.m.slpSessionIDPresent)
  {
    supl_util_server_session_id_organizer ( &p_pdu->sessionID, &p_session_id->z_Slp_Sess_Id);
  } /* close if-SLP-sessionID present */

}


/*
******************************************************************************
* supl_util_server_session_id_organizer
*
* Function description:
*   This function re-packages server session ID
*
* Parameters:
*
* p_source_session_id : pointer to a structure holding decoded SessionID from
*   a source that is an IE in an incoming SUPL message.
*
* p_slp_session_id : pointer for the function to pass repackaged
*   slp session ID to the calling function.
*
* Return value:
*  None
*
*
******************************************************************************
*/

static void supl_util_server_session_id_organizer ( const supl_SessionID *p_source_session_id,
                                                    supl_slp_session_id *p_slp_session_id)
{

  memset( p_slp_session_id, 0, sizeof(supl_slp_session_id) );

  if ( p_source_session_id->m.slpSessionIDPresent == 1 )
  {
    p_slp_session_id->presence = TRUE;
  }
  else
  {
    return;
  }

  /* Copy Session ID */
  memscpy( p_slp_session_id->session_Id, sizeof(p_slp_session_id->session_Id),
          p_source_session_id->slpSessionID.sessionID.data,
          sizeof(p_source_session_id->slpSessionID.sessionID.data));

  if ( p_source_session_id->slpSessionID.slpId.t == C_SUPL_ASN1_CHOICE_SLPADDRESS_IPADDRESS )
  {
    /* IP Address */
    p_slp_session_id->zSlpAddress.choice = C_SUPL_ASN1_CHOICE_SLPADDRESS_IPADDRESS;

    if ( p_source_session_id->slpSessionID.slpId.u.iPAddress->t == C_SUPL_ASN1_CHOICE_IPV4ADDRESS )
    {
      /* IPV4 */
      p_slp_session_id->zSlpAddress.u.z_Ip_Address.choice = C_SUPL_ASN1_CHOICE_IPV4ADDRESS;
      p_slp_session_id->zSlpAddress.u.z_Ip_Address.u.Ip_V4.numocts = 4;
      memscpy( p_slp_session_id->zSlpAddress.u.z_Ip_Address.u.Ip_V4.data,
              sizeof(p_slp_session_id->zSlpAddress.u.z_Ip_Address.u.Ip_V4.data),
              p_source_session_id->slpSessionID.slpId.u.iPAddress->u.ipv4Address->data,
              sizeof(p_source_session_id->slpSessionID.slpId.u.iPAddress->u.ipv4Address->data));
    }
    else
    {
      /* IPV6 */
      p_slp_session_id->zSlpAddress.u.z_Ip_Address.choice = C_SUPL_ASN1_CHOICE_IPV6ADDRESS;
      p_slp_session_id->zSlpAddress.u.z_Ip_Address.u.Ip_V6.numocts = 16;
      memscpy( p_slp_session_id->zSlpAddress.u.z_Ip_Address.u.Ip_V6.data,
            sizeof(p_slp_session_id->zSlpAddress.u.z_Ip_Address.u.Ip_V6.data),
              p_source_session_id->slpSessionID.slpId.u.iPAddress->u.ipv6Address->data,
              sizeof(p_source_session_id->slpSessionID.slpId.u.iPAddress->u.ipv6Address->data));
    }
  }
  else
  {

    /* FQDN */
    p_slp_session_id->zSlpAddress.choice = C_SUPL_ASN1_CHOICE_SLPADRESS_FQDN;

    GNSS_STRLCPY (p_slp_session_id->zSlpAddress.u.z_fqdn.fqdn,
                 p_source_session_id->slpSessionID.slpId.u.fQDN,
                 C_SUPL_FQDN_VISIBLE_STRING_BYTE_LENGTH - 1);

    // NULL terminate
    p_slp_session_id->zSlpAddress.u.z_fqdn.fqdn[C_SUPL_FQDN_VISIBLE_STRING_BYTE_LENGTH - 1] = NULL;
    p_slp_session_id->zSlpAddress.u.z_fqdn.u_length = strlen (p_slp_session_id->zSlpAddress.u.z_fqdn.fqdn);
  }
}




/*
 ******************************************************************************
 *
 * supl_rrlp_gps_assist_proc
 *
 * Function description:
 *  This function processes the OSS ASN.1 decoded RRLP gpsAssistance data and
 *  prepares such assistance data for injecting to MGP.
 *
 * Parameters:
 *
 *  p_LsmControl: pointer to the lsm_ctrl_type structure that keeps the
 *  pointers of many structures accessed from different modules and
 *  functions.
 *
 *  p_assist: pointer to the OSS ASN.1 decoded RRLP gpsAsssitance data.
 *
 * Return value:
 *
 *
 ******************************************************************************
*/

static boolean supl_rrlp_gps_assist_proc( rrlp_ControlHeader *p_assist )
{
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;
  tm_post_data_payload_type    z_tm_post_data_payload;

  gps_RefTimeStructType  reftime_data; /* QQQ: temp, as we seem to need week number for eph. for now */

  if (NULL == p_assist)
  {
     MSG_ERROR("SUPL RRLP payload NULL", 0, 0, 0);
     return FALSE;
  }

  memset(&z_tm_post_data_payload, 0x00, sizeof(tm_post_data_payload_type));
  memset(&reftime_data, 0x00, sizeof(reftime_data));

  /************** REFERENCE TIME **********************************/

  if ( p_assist->m.referenceTimePresent )
  {
    uint32 q_GpsTow;

    /* QQQ: for possible time-stamping SET-Assisted position, until a UTC-->GPS conversion
       utility is available */
    p_supl_session_status->z_set_assist_pos_time_stamp.w_ref_time_week_num =
      gnss_GpsWeekExtend( (uint16)p_assist->referenceTime.gpsTime.gpsWeek );
    p_supl_session_status->z_set_assist_pos_time_stamp.m.refTimeWeekNumValid = TRUE;

    q_GpsTow = (uint32)p_assist->referenceTime.gpsTime.gpsTOW23b;
    q_GpsTow *= 80; /* gpsTow23b LSB = 0.08 sec */

    /* 23 bit Gps Time of Week converted to full milli-seconds of the week */
    tm_umts_common_utils_GpsMsecWeekLimit(
       (int32 *)&q_GpsTow,
       &p_supl_session_status->z_set_assist_pos_time_stamp.w_ref_time_week_num );

    p_supl_session_status->z_set_assist_pos_time_stamp.t_pos_time_stamp_ms =
        tm_umts_common_utils_full_gps_ms(
            p_supl_session_status->z_set_assist_pos_time_stamp.w_ref_time_week_num,
            q_GpsTow);

    p_supl_session_status->z_set_assist_pos_time_stamp.m.posTimeStampValid = TRUE;

    tm_rrlp_reftime_to_TmCore_post_data_payload( p_assist, &z_tm_post_data_payload);

    /* if ref time availabe, use that to set eph. week number, facilitating the process that
       otherwise can be performed by PE */
    memscpy( &reftime_data, sizeof(reftime_data),
             &z_tm_post_data_payload.reftime_data, sizeof( z_tm_post_data_payload.reftime_data ) );

    if ( tm_post_data ( TM_PRTL_TYPE_UMTS_UP,
                        p_supl_session_status->tm_active_session_handle,
                        TM_POST_DATA_TYPE_REFTIME,
                        &z_tm_post_data_payload
                      ) == FALSE
       )
    {
      MSG_HIGH("SUPL to Tmcore data post failed", 0, 0, 0);
    }
    else
    {
      MSG_MED("SUPL: refT posted to TmCore", 0, 0, 0);
    }


    if ( p_assist->referenceTime.m.gsmTimePresent )
    {
      /* Not supported */
    }

    if ( p_assist->referenceTime.m.gpsTowAssistPresent )
    {
      #if 0   /*** QQQ-PM: need to discuss how to handle sparse pattern match **/  /* ayt - ignore */

      lsm_raw_tow_type *p_zTow = &p_zSmlcAssist->z_SmlcTow[0];
      struct rrlp_GPSTOWAssist *p_rrlp_tow = p_assist->referenceTime.gpsTowAssist;

      /* Initialize TOW array */
      memset( p_zSmlcAssist->z_SmlcTow, 0, sizeof( p_zSmlcAssist->z_SmlcTow ) );

      while ( p_rrlp_tow != NULL )
      {
        /* SvId */
        p_zTow->w_SvId = (U8)p_rrlp_tow->value.satelliteID + 1;

        /* Tlm word */
        p_zTow->w_TlmWord = (U16)p_rrlp_tow->value.tlmWord;

        /* AntiSpoof */
        p_zTow->u_AntiSpoofFlag = (U8)p_rrlp_tow->value.antiSpoof;

        /* Alert */
        p_zTow->u_AlertFlag = (U8)p_rrlp_tow->value.alert;

        /* TlmRsvBits */
        p_zTow->u_TlmRsvBits = (U8)p_rrlp_tow->value.tlmRsvdBits;

        /* Update TOW pointer to next element in the list */
        p_zTow++;
        p_rrlp_tow = p_rrlp_tow->next;
      }

      #endif

    } /* close if-GpsToW-Assist-available */

  } /* close if-refTime-available */


  /************** REFERENCE LOCATION **********************************/

  if ( p_assist->m.refLocationPresent )
  {

    if (  tm_rrlp_refpos_to_TmCore_post_data_payload
            ( p_assist,
              NULL,
              &z_tm_post_data_payload,
              (uint8 *)&z_last_refloc_info.z_RefPos_GAD.gad_ellip_alt_unc_ellip,
              FALSE
            )
       )
    {

      uint32                q_time_stamp_sec = 0;
      pdsm_pd_event_type    q_pd_event; /* pdsm_pd_event_type is uint32 */

      if ( p_supl_session_status->z_set_assist_pos_time_stamp.m.refTimeWeekNumValid )
      {
        /* seconds since GPS birth */
        q_time_stamp_sec =  (uint32) (p_supl_session_status->z_set_assist_pos_time_stamp.t_pos_time_stamp_ms / SEC_MSECS);
      }

      (void)tm_umts_common_utils_gad_to_pdsm_pd_info(
        p_supl_session_status->q_pdapi_client_id,
        (uint8 *)&z_last_refloc_info.z_RefPos_GAD.gad_ellip_alt_unc_ellip,
        PDSM_PD_POSITION_TYPE_NEW,
        PDSM_PD_POSITION_MODE_UNKNOWN,
        PDSM_PD_POSITION_SOURCE_DEFAULT,
        q_time_stamp_sec,
        &z_supl_pd_info_buffer,
        &q_pd_event
      );

      tm_post_ext_status_ref_loc_rep_event(
        TM_PRTL_TYPE_UMTS_UP,
        p_supl_session_status->tm_active_session_handle,
        &z_supl_pd_info_buffer.pd_info.pd_data);

      if ( q_handovers_since_last_cell_id )
      {

       /* Increase the Reference Position uncertainty by # of cell changes
          times maximum cell diameter. NV item aagps_default_ref_position_unc
          represents the radius of the cell */

       z_tm_post_data_payload.seed_data.f_LocUncrtntySemiMajor +=
         (float)( q_handovers_since_last_cell_id *
                  z_umts_nv_item_ram_copy.aagps_default_ref_position_unc * 2 );

       z_tm_post_data_payload.seed_data.f_LocUncrtntySemiMinor +=
         (float) ( q_handovers_since_last_cell_id *
                   z_umts_nv_item_ram_copy.aagps_default_ref_position_unc * 2 );

      }


      if ( tm_post_data ( TM_PRTL_TYPE_UMTS_UP,
                          p_supl_session_status->tm_active_session_handle,
                          TM_POST_DATA_TYPE_SEED,
                          &z_tm_post_data_payload
                        ) == FALSE
         )
      {
        MSG_HIGH("SUPL to Tmcore data post failed", 0, 0, 0);
      }
      else
      {
        MSG_MED("SUPL: refL posted to TmCore", 0, 0, 0);

        /* this is for storing the reference position in the original GAD shape
           format, in case there is a need later to report this back to the
           network */
        z_last_refloc_info.z_RefPos_GAD.u_pos_valid = TRUE;
        if ( p_assist->m.referenceTimePresent )
        {
          z_last_refloc_info.z_RefPos_GAD.w_week =
          p_supl_session_status->z_set_assist_pos_time_stamp.w_ref_time_week_num;

          /* 23 bit Gps Time of Week : q_GpsTow23b LSB = 0.08 sec */
          z_last_refloc_info.z_RefPos_GAD.q_tow_ms =
           (uint32)p_assist->referenceTime.gpsTime.gpsTOW23b * 80;

          z_last_refloc_info.z_RefPos_GAD.u_timestamp_valid = TRUE;
        }

        /* associate reference position with cell info. */
        memscpy( &z_last_refloc_info.z_cell_info_refloc,
                 sizeof(z_last_refloc_info.z_cell_info_refloc),
                &z_cell_info_cached,
                sizeof (z_cell_info_cached) );
      }
    }

  } /* close "if-refLoc-available */


  /*********  DGPS *********************************************/

  if ( p_assist->m.dgpsCorrectionsPresent )
  {
    /* Not supported, ignored */
  }


  /************** Nav. Model **********************************/

  if ( p_assist->m.navigationModelPresent )
  {

    rrlp_SeqOfNavModelElement *navModelList;
    rrlp_UncompressedEphemeris *p_Ephem;
    rrlp_NavModelElement       *p_navModelList_elem;

    uint16 w_week; /* QQQ: for now we seem need the week number */
    uint8 u_i;

    uint32 q_eph_svs_injected = 0;

    navModelList = &p_assist->navigationModel.navModelList;
    p_navModelList_elem = navModelList->elem;
    for ( u_i=0; u_i < navModelList->n; u_i++ )
    {
      uint32 q_Word;

      /* SvId */
      q_Word = p_navModelList_elem->satelliteID + 1;

      z_tm_post_data_payload.eph_data.u_Sv = (uint8)q_Word; /* SV ID */

      /* SatStatus ChoiceTag - currently SMLC only supports
         newNavModelUC (2) */
      if ( p_navModelList_elem->satStatus.t == rrlp_oldSatelliteAndModel_chosen )
      {
        /* oldStatelliteAndModel: no encoded eph.
           values for this SV will follow */
        if ( u_i < ( navModelList->n - 1) )
          p_navModelList_elem++;
        continue;
      }

      if ( p_navModelList_elem->satStatus.t == rrlp_newSatelliteAndModelUC_chosen )
        p_Ephem = p_navModelList_elem->satStatus.u.newSatelliteAndModelUC;
      else if ( p_navModelList_elem->satStatus.t == rrlp_newNaviModelUC_chosen )
        p_Ephem = p_navModelList_elem->satStatus.u.newNaviModelUC;
      else
      {
        /* Unknown */
        MSG_HIGH("Unknown Sat Status",0,0,0);
        if ( u_i < ( navModelList->n - 1) )
          p_navModelList_elem++;
        continue;
      }

      if ( p_assist->m.referenceTimePresent )
      {
        w_week = tm_umts_common_utils_build_eph_week_from_ref_time ( p_Ephem->ephemToe,
                                                                     reftime_data.q_GpsMsec,
                                                                     reftime_data.w_GpsWeek );
      }
      else
      {
        MSG_MED("no eph. week since there is no ref time", 0, 0, 0);
        w_week = 0;
      }

      p_supl_session_status->q_eph_delivered_sv_list |= 1L << p_navModelList_elem->satelliteID;

      tm_rrlp_eph_to_TmCore_post_data_payload( p_Ephem, &z_tm_post_data_payload, w_week);

      if ( tm_post_data ( TM_PRTL_TYPE_UMTS_UP,
                          p_supl_session_status->tm_active_session_handle,
                          TM_POST_DATA_TYPE_EPH,
                          &z_tm_post_data_payload
                        ) == FALSE
         )
      {
        MSG_HIGH("SUPL to TmCore eph SV %u post failed", q_Word, 0, 0);
      }
      else
      {
        q_eph_svs_injected |= 1L << p_navModelList_elem->satelliteID;
      }

      /* Go to next element in the list */
      if ( u_i < ( navModelList->n - 1) )
        p_navModelList_elem++;

    } /* for-loop for the eph. SVs in the message */

    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED,
          "SUPL: GPS eph. posted to TmCore 0x%lx, total SVs = %u",
          q_eph_svs_injected, navModelList->n );

  } /* close if-eph-available */

  /****************************** IONO ****************************************/

  if ( p_assist->m.ionosphericModelPresent )
  {
    tm_rrlp_Iono_to_TmCore_post_data_payload( p_assist, &z_tm_post_data_payload );

    if ( tm_post_data ( TM_PRTL_TYPE_UMTS_UP,
                        p_supl_session_status->tm_active_session_handle,
                        TM_POST_DATA_TYPE_IONO,
                        &z_tm_post_data_payload
                      ) == FALSE
       )
    {
      MSG_HIGH("SUPL to Tmcore data post failed", 0, 0, 0);
    }
    else
    {
      MSG_MED("SUPL: IONO posted to TmCore", 0, 0, 0);
    }
  }


  /****************************** UTC ****************************************/

  if ( p_assist->m.utcModelPresent )
  {
    tm_rrlp_UtcModel_to_TmCore_post_data_payload( p_assist, &z_tm_post_data_payload );

    if ( tm_post_data ( TM_PRTL_TYPE_UMTS_UP,
                        p_supl_session_status->tm_active_session_handle,
                        TM_POST_DATA_TYPE_UTC_MODEL,
                        &z_tm_post_data_payload
                      ) == FALSE
       )
    {
      MSG_HIGH("SUPL to Tmcore data post failed", 0, 0, 0);
    }
    else
    {
      MSG_MED("SUPL: UTC posted to TmCore", 0, 0, 0);
    }


    /* Convert to glonass time & set the z_set_assist_pos_time_stamp struct. Later used in injecting AA.
       Don't bother about return value. If conversion fails Glo steering won't be posted to TM. */

    if ( (p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED) &&
         (FALSE == p_supl_session_status->z_set_assist_pos_time_stamp.m.glonassTimeValid)
        )
    {
      tm_util_convert_to_glonass_time ( p_assist->utcModel.utcDeltaTls,
                                        &p_supl_session_status->z_set_assist_pos_time_stamp);
    }


  }

  /******************* almanac ***************************************************/

  if ( p_assist->m.almanacPresent )
  {
    rrlp_SeqOfAlmanacElement *almanacList = &p_assist->almanac.almanacList;
    rrlp_AlmanacElement      *p_Almanac;
    uint16 w_AlmanacWeek;
    uint8  u_i;
    uint32 q_alm_svs_injected = 0;

    /* Almanac WNa: 8 LSBs of the full 10-bit week -- PE will solve the ambiguity  */
    w_AlmanacWeek = (int16)p_assist->almanac.alamanacWNa;

    /* start almanac list */
    p_Almanac = almanacList->elem;

    for ( u_i=0; u_i<almanacList->n; u_i++ )
    {
      uint16 w_SvId;

      w_SvId = (uint8)p_Almanac->satelliteID + 1; /* 6-bit unsigned */

      if ( w_SvId > N_SV )
      {
        if ( u_i < (almanacList->n - 1) )
          p_Almanac++;
        continue;
      }

      tm_rrlp_alm_to_TmCore_post_data_payload( p_Almanac, &z_tm_post_data_payload );
      z_tm_post_data_payload.alm_data.w_GpsWeek = w_AlmanacWeek;

      p_supl_session_status->q_alm_delivered_sv_list |= 1L << p_Almanac->satelliteID;

      if ( tm_post_data ( TM_PRTL_TYPE_UMTS_UP,
                          p_supl_session_status->tm_active_session_handle,
                          TM_POST_DATA_TYPE_ALM,
                          &z_tm_post_data_payload
                        ) == FALSE
         )
      {
        MSG_HIGH("SUPL to TmCore alm SV %u post failed", w_SvId, 0, 0);
      }
      else
      {
        q_alm_svs_injected |= 1L << p_Almanac->satelliteID;
      }

      /* Move to next element of the list */
      if ( u_i < (almanacList->n - 1) )
      {
        p_Almanac++;
      }

    } /* for-loop GPS alm SVs */

    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED,
          "SUPL: GPS alm. posted to TmCore 0x%lx, total SVs = %u",
          q_alm_svs_injected, almanacList->n);

  }

  /**************** acquisition assistance **********************/

  if ( p_assist->m.acquisAssistPresent )
  {
    if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED )
    {

      gps_RefTimeStructType  reftime_data1;

      memset(&reftime_data1, 0, sizeof( gps_RefTimeStructType ) );
      reftime_data1.w_GpsWeek = C_GPS_WEEK_UNKNOWN;

      tm_umts_up_supl_inform_core_nt_assist_end(); /* QQQ: need to consider what if ref time is coming in a later RRLP */
      /* receiver is now IDLE */

      if ( p_assist->m.referenceTimePresent )
      {
        memscpy( &reftime_data1, sizeof(reftime_data1), &z_tm_post_data_payload.reftime_data,
                 sizeof( z_tm_post_data_payload.reftime_data ) );
      }

      /* inject SV directions */
      tm_rrlp_AcqAssistSvDir_to_TmCore_post_data_payload( p_assist, &z_tm_post_data_payload );

      if ( q_handovers_since_last_cell_id != 0 )
      {
        uint8 u_i;

        /* Widen the Acq assistance windows by a factor of
           q_HandoversSinceLastCellID to account for the fact that the Acq
           Assistance was generated for a cell that the phone is no longer in */

        for ( u_i=0; u_i < z_tm_post_data_payload.gnss_aa_data.u_NumSvs; u_i++ )
        {
          z_tm_post_data_payload.gnss_aa_data.z_SvSteerInfo[u_i].f_SvTimeUnc *=
            q_handovers_since_last_cell_id + 1;
        }

      }

      if ( z_tm_post_data_payload.gnss_sv_dir.u_NumSvs ) /* Optional AdditionalAngle */
      {
        if ( tm_post_data ( TM_PRTL_TYPE_UMTS_UP,
                            p_supl_session_status->tm_active_session_handle,
                            TM_POST_DATA_TYPE_SV_DIR,
                            &z_tm_post_data_payload
                          ) == FALSE
           )
        {
          MSG_HIGH("SUPL to Tmcore data post failed", 0, 0, 0);
        }
        else
        {
          MSG_MED("SUPL: SvDir posted to TmCore", 0, 0, 0);
        }
      }


      /* inject steering */
      tm_rrlp_AcqAssistSteering_to_TmCore_post_data_payload( p_assist, &z_tm_post_data_payload, reftime_data1.w_GpsWeek );
      if ( tm_post_data ( TM_PRTL_TYPE_UMTS_UP,
                          p_supl_session_status->tm_active_session_handle,
                          TM_POST_DATA_TYPE_AA,
                          &z_tm_post_data_payload
                        ) == FALSE
         )
      {
        MSG_HIGH("SUPL to Tmcore data post failed", 0, 0, 0);
      }
      else
      {
        MSG_MED("SUPL: Steering posted to TmCore", 0, 0, 0);
      }

    } /* close if-SET-Assisted */

  } /* close if-acq-assist-available */

  /**************** Real Time Integrity ************************/


  if ( p_assist->m.realTimeIntegrityPresent )
  {
    rrlp_SeqOf_BadSatelliteSet *p_RTI = &p_assist->realTimeIntegrity;
    rrlp_SatelliteID           *p_SatelliteID;

    uint32 q_RealTimeIntegrity0 = 0;
    uint32 q_RealTimeIntegrity1 = 0;
    uint32 u_i;

    p_SatelliteID = p_RTI->elem;

    for ( u_i=0; u_i<p_RTI->n; u_i++ )
    {
      uint32 q_Word;

      q_Word = *p_SatelliteID + 1;

      if (q_Word <= 32)
      {
        q_RealTimeIntegrity0 |= 1 << (q_Word-1);
      }
      else
      {
        q_RealTimeIntegrity1 |= 1 << (q_Word-33);
      }

      /* Move to next element of the list */
      if ( u_i < (p_RTI->n - 1) )
      {
        p_SatelliteID++;
      }
    }

    MSG_MED("SUPL: Received RTI informationi %d", q_RealTimeIntegrity0,0,0);

    /* Determine if we have enough info. about some SVs' health and
       inform MGP about such info., if available */
    tm_util_post_unhealthy_svs ( TM_PRTL_TYPE_UMTS_UP,
                                p_supl_session_status->tm_active_session_handle,
                                q_RealTimeIntegrity0);
  }


  return ( TRUE );
}

/*
 ******************************************************************************
 *
 * supl_rrlp_ganss_assist_proc
 *
 * Function description:
 *  This function processes the OSS ASN.1 decoded RRLP ganssAssistance data
 *  and prepares such assistance data for injecting to MGP.
 *
 * Parameters:
 *
 *
 *  p_assist: pointer to the OSS ASN.1 decoded RRLP ganssAsssitance data.
 *
 * Return value: TRUE/FALSE
 *
 *
 ******************************************************************************
*/

static boolean supl_rrlp_ganss_assist_proc( rrlp_GANSS_ControlHeader *p_assist )
{

  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;
  tm_post_data_payload_type    z_tm_post_data_payload;

  struct rrlp_SeqOfGANSSGenericAssistDataElement *p_ganss_generic_assist_data_list = NULL;
  struct rrlp_SeqOfGANSSSatelliteElement  *p_ganss_sat_list = NULL;
  rrlp_GANSSSatelliteElement *p_ganss_sat_element = NULL;

  if ( p_assist->m.ganssCommonAssistDataPresent == 1 )
  {

    /* In gannCommon, only reference time and reference location are used */

    #if 0  // MGP does not process GLO ref. time at this point
    if ( p_assist->ganssCommonAssistData.m.ganssReferenceTimePresent == 1 )
    {
      tm_rrlp_glo_reftime_to_TmCore_post_data_payload( p_assist, &z_tm_post_data_payload);

      if ( tm_post_data ( TM_PRTL_TYPE_UMTS_UP,
                          p_supl_session_status->tm_active_session_handle,
                          TM_POST_DATA_TYPE_REFTIME_GLO,
                          &z_tm_post_data_payload
                        ) == FALSE
         )
      {
        MSG_HIGH("SUPL to Tmcore data post failed", 0, 0, 0);
      }
      else
      {
        MSG_MED("SUPL: Glonass refT posted to TmCore", 0, 0, 0);

      }

    }
    #endif

    if ( p_assist->ganssCommonAssistData.m.ganssRefLocationPresent == 1 )
    {
      /* GANSS ref. location has the same structure as that of the GPS'. Use the
         same interface to pass ref. location to MGP */
      tm_rrlp_refpos_to_TmCore_post_data_payload (
        NULL,
        p_assist,
        &z_tm_post_data_payload,
        (uint8 *)&z_last_refloc_info.z_RefPos_GAD.gad_ellip_alt_unc_ellip,
        FALSE);

      if ( tm_post_data ( TM_PRTL_TYPE_UMTS_UP,
                          p_supl_session_status->tm_active_session_handle,
                          TM_POST_DATA_TYPE_SEED,
                          &z_tm_post_data_payload
                        ) == FALSE
         )
      {
        MSG_HIGH("SUPL to Tmcore data post failed", 0, 0, 0);
      }
      else
      {
        MSG_MED("SUPL: Glonass refL posted to TmCore", 0, 0, 0);

      }
    } /* if ganssRefL present */

  } /* if ganssCommonAssistData present */


  if ( p_assist->m.ganssGenericAssistDataListPresent == 1 )
  {
    uint8 u_i;
    rrlp_GANSSGenericAssistDataElement  *p_ganss_generic_assist_data_elem = NULL;

    p_ganss_generic_assist_data_list = &p_assist->ganssGenericAssistDataList;
    p_ganss_generic_assist_data_elem = p_ganss_generic_assist_data_list->elem;

    for ( u_i = 0; u_i < p_ganss_generic_assist_data_list->n; u_i++, p_ganss_generic_assist_data_elem++)
    {

      /* only Glonass is supported */
      if ( ( p_ganss_generic_assist_data_elem->m.ganssIDPresent == 1) &&
           ( p_ganss_generic_assist_data_elem->ganssID == 3 )
         )
      {

        if (p_ganss_generic_assist_data_elem->m.ganssTimeModelPresent && p_ganss_generic_assist_data_elem->ganssTimeModel.n > 0 )
        {
          uint8 u_i = 0;
          rrlp_GANSSTimeModelElement *p_ganss_time_model;

          p_supl_session_status->z_OrigWishList.z_GloAssistData.q_WishListMask &= ~C_GNSS_ASSIST_TIME_MODELS;

          p_ganss_time_model = p_ganss_generic_assist_data_elem->ganssTimeModel.elem;
          for ( u_i=0; u_i < p_ganss_generic_assist_data_elem->ganssTimeModel.n; u_i++ )
          {
            /* interested only in GPS gnss-TO-ID time */
            if ( p_ganss_time_model != NULL )
            {
              if ( p_ganss_time_model->gnssTOID == C_RRLP_GNSS_TO_ID_GPS )
              {
                z_tm_post_data_payload.gnss_time_model_data.l_A0 = p_ganss_time_model->tA0;
                z_tm_post_data_payload.gnss_time_model_data.q_GanssTimeModelRefTime = p_ganss_time_model->ganssTimeModelRefTime;
                z_tm_post_data_payload.gnss_time_model_data.u_GnssToID = C_GNSS_TO_ID_GPS;

                z_tm_post_data_payload.gnss_time_model_data.w_DeltaT = C_GNSS_GANSST_DELTAT_UNKNOWN; /* until the MGP API proivdes a different way to identify, use 0xFFFF for NA */
                z_tm_post_data_payload.gnss_time_model_data.x_A1 = C_GNSS_GANSST_A1_UNKNOWN; /* until the MGP API proivdes a different way to identify, use 0x8000 for NA */
                z_tm_post_data_payload.gnss_time_model_data.x_A2 = C_GNSS_GANSST_A2_UNKNOWN; /* until the MGP API proivdes a different way to identify, use 0x8000 for NA */
                z_tm_post_data_payload.gnss_time_model_data.w_WeekNo = C_GNSS_GANSST_WEEK_UNKNOWN;  /* until the MGP API proivdes a different way to identify, use 0xFFFF for NA */

                if (TRUE == p_ganss_time_model->m.weekNumberPresent)
                {
                  z_tm_post_data_payload.gnss_time_model_data.w_WeekNo = p_ganss_time_model->weekNumber;
                }
                if (TRUE == p_ganss_time_model->m.tA1Present)
                {
                  z_tm_post_data_payload.gnss_time_model_data.x_A1 = p_ganss_time_model->tA1;
                }
                if (TRUE == p_ganss_time_model->m.tA2Present)
                {
                  z_tm_post_data_payload.gnss_time_model_data.x_A2 = p_ganss_time_model->tA2;
                }

                if (FALSE == tm_post_data(TM_PRTL_TYPE_UMTS_UP,
                                          p_supl_session_status->tm_active_session_handle,
                                          TM_POST_DATA_TYPE_TIME_MODEL_GLO,
                                          &z_tm_post_data_payload
                                          ))
                {
                  MSG_HIGH("TM_RRLP_UP: SUPL(Glo) TimeModel to Tmcore data post failed", 0, 0, 0);
                }
                else
                {
                  MSG_MED("TM_RRLP_UP: SUPL(Glo): TimeModel posted to TmCore", 0, 0, 0);
                }

              } /* ganss TOID = GPS */
            } /* pointer not NULL */
            p_ganss_time_model++;
          } /* for loop going through n members of rrlp_GANSSTimeModelElement */
        } /* ganssTime model present and n>0 */


        if ( p_ganss_generic_assist_data_elem->m.ganssNavigationModelPresent == 1 )
        {
          uint8  u_sat_i = 0;
          uint32 q_eph_svs_injected = 0;
          uint32 q_eph_svs_not_injected = 0;

          p_ganss_sat_list =
            &p_ganss_generic_assist_data_elem->ganssNavigationModel.ganssSatelliteList;

          p_ganss_sat_element = p_ganss_sat_list->elem;

          for (u_sat_i = 0; u_sat_i < p_ganss_sat_list->n; u_sat_i++, p_ganss_sat_element++)
          {
            if ( p_ganss_sat_element->svID >= N_GLO_SV )
            {
              /* should not happen */
              continue;
            }

            supl_glo_eph_build(p_ganss_sat_element);

            if ( p_supl_session_status->z_prot_glo_eph_buffer[p_ganss_sat_element->svID].u_freq_num_valid
                 == TRUE)
            {

              /* if this SV already has freq. number, inject it now;
                 otherwise keep data in the buffer and inject when all
                 assistance have been delivered */
              tm_rrlp_glo_eph_to_TmCore_post_data_payload
                ( &p_supl_session_status->z_prot_glo_eph_buffer[p_ganss_sat_element->svID],
                  &z_tm_post_data_payload );

              if ( tm_post_data ( TM_PRTL_TYPE_UMTS_UP,
                                  p_supl_session_status->tm_active_session_handle,
                                  TM_POST_DATA_TYPE_EPH_GLO,
                                  &z_tm_post_data_payload
                                ) == FALSE
                 )
              {
                /* Bound check for z_rrlp_glo_eph_buffer array */
                if (u_i < N_GLO_SV)
                {
                  MSG_HIGH("Glo eph. SV %u inj. to TmCore failed",
                           p_supl_session_status->z_prot_glo_eph_buffer[u_i].z_eph_glo_data.u_SvId, 0, 0);
                }
                else
                {
                  MSG_ERROR("Glo eph. SV inj. to TmCore failed and array z_prot_glo_eph_buffer out of bounds with index of %d", u_i, 0, 0);
                  continue;
                }
              }
              else
              {
                q_eph_svs_injected |= 1L << p_ganss_sat_element->svID;
              }

              /* once processed, remove it from the buffer */
              /* 0 would be an invalid number because the api uses 65 and up for Glonass */
              p_supl_session_status->z_prot_glo_eph_buffer[p_ganss_sat_element->svID].z_eph_glo_data.u_SvId = 0;
              p_supl_session_status->z_prot_glo_eph_buffer[p_ganss_sat_element->svID].u_freq_num_valid = FALSE;

            } /* if this SV already has freq. number */
            else
            {
              q_eph_svs_not_injected |= 1L << p_ganss_sat_element->svID;
            }
          } /* for */

          if ( q_eph_svs_injected != 0 )
          {
            MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED,
                  "SUPL GLONASS eph posted to TmCore (eph. recv) 0x%lx, total SVs = %u",
                  q_eph_svs_injected, p_ganss_sat_list->n);
          }
          if ( q_eph_svs_not_injected != 0 )
          {
            MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED,
                  "SUPL GLONASS eph pending posting to TmCore (eph. recv) 0x%lx, total SVs = %u",
                  q_eph_svs_not_injected, p_ganss_sat_list->n);
          }

        } /* ganssNavModel present */

        if ( p_ganss_generic_assist_data_elem->m.ganssAlmanacModelPresent == 1 )
        {
          struct rrlp_SeqOfGANSSAlmanacElement *p_ganss_alm_list =
            &p_ganss_generic_assist_data_elem->ganssAlmanacModel.ganssAlmanacList;

          rrlp_GANSSAlmanacElement *p_ganss_almanac = p_ganss_alm_list->elem;
          uint16 w_AlmanacWeek;
          uint8  u_alm_i;
          uint32 q_alm_svs_injected = 0;

          /* ayt - how to post weekNumber to MGP ? */
          /* Almanac WNa: 8 LSBs of the full 10-bit week -- PE will solve the ambiguity  */
          w_AlmanacWeek = (int16)p_ganss_generic_assist_data_elem->ganssAlmanacModel.weekNumber;

          /* start almanac list */
          for (u_alm_i = 0; u_alm_i < p_ganss_alm_list->n; u_alm_i++, p_ganss_almanac++ )
          {
            if ( p_ganss_almanac->t == T_rrlp_GANSSAlmanacElement_keplerianGLONASS )
            {
              supl_glo_eph_svid_freq_num_map_alm (p_ganss_almanac->u.keplerianGLONASS);

              tm_rrlp_glo_alm_to_TmCore_post_data_payload
                ( p_ganss_almanac->u.keplerianGLONASS, &z_tm_post_data_payload );
              z_tm_post_data_payload.alm_data.w_GpsWeek = w_AlmanacWeek;

              if ( tm_post_data ( TM_PRTL_TYPE_UMTS_UP,
                                  p_supl_session_status->tm_active_session_handle,
                                  TM_POST_DATA_TYPE_ALM_GLO,
                                  &z_tm_post_data_payload
                                ) == FALSE
                 )
              {
                MSG_HIGH("SUPL to Tmcore data post failed", 0, 0, 0);
              }
              else
              {
                q_alm_svs_injected |= 1L << ( p_ganss_almanac->u.keplerianGLONASS->gloAlmnA - 1 );
              }

            } /* if Model 5 ( GLONASS ) */
          } /* for-loop for ganssAlmanac list */

          MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED,
                "SUPL GLONASS alm. posted to TmCore 0x%lx, total SVs = %u",
                q_alm_svs_injected, p_ganss_alm_list->n);

        } /* ganssAlmanac present */

        if ( ( p_ganss_generic_assist_data_elem->m.ganssAddUTCModelPresent == 1) &&
             ( p_ganss_generic_assist_data_elem->ganssAddUTCModel.t == T_rrlp_GANSSAddUTCModel_utcModel3 )
           )
        {
              tm_rrlp_glo_addutc_to_TmCore_post_data_payload
                ( p_assist, &z_tm_post_data_payload );

              if ( tm_post_data ( TM_PRTL_TYPE_UMTS_UP,
                                  p_supl_session_status->tm_active_session_handle,
                              TM_POST_DATA_TYPE_UTC_MODEL_GLO,
                                  &z_tm_post_data_payload
                                ) == FALSE
                 )
              {
                MSG_HIGH("SUPL to Tmcore data post failed", 0, 0, 0);
              }
              else
              {
                MSG_MED("SUPL: GANSS Additional UTC posted to TmCore", 0, 0, 0);

              }


        } /* ganssAddUTC Model 3 (GLONASS) present */


        if  ( p_ganss_generic_assist_data_elem->m.ganssRefMeasurementAssistPresent )
        {

          if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED )
          {

            if ( ( p_ganss_generic_assist_data_elem->ganssRefMeasurementAssist.ganssSignalID == GANSS_SIGNAL_ID_GLONASS_G1 ) &&
                 ( p_ganss_generic_assist_data_elem->ganssRefMeasurementAssist.ganssRefMeasAssistList.elem != NULL ) &&
                 ( p_ganss_generic_assist_data_elem->ganssRefMeasurementAssist.ganssRefMeasAssistList.n > 0 )
               )
            {

                 /* Build SV directions (Azim & Elevation) & Steering information if Glonass time is available.
                    Else take care of Glonass time & freqNum mapping later when GPS TimeModel & AuxiInfo AD is received */

                if (p_supl_session_status->z_set_assist_pos_time_stamp.m.glonassTimeValid)
                {
                  tm_rrlp_glo_acq_assist_to_tmcore_post_data_payload (
                      &p_ganss_generic_assist_data_elem->ganssRefMeasurementAssist.ganssRefMeasAssistList,
                      &p_supl_session_status->z_prot_glo_acqasst_buffer,
                      p_supl_session_status->z_set_assist_pos_time_stamp.z_GlonassTime.q_GloMsec);
                }
                else
                {
                  MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "No Glonass time at present, preserve AA data");

                  /* Store AA data */
                }

                /* Pseudo segmented AA data? Correlate SvId-to-FreqNum if AA sent in a separate transaction
                     (different from AuxiInfo) */

            }  /* refMeasurement data valid */

          } /* is MSA mode */

        }  /* reference measurement assistance present */

        if ( ( p_ganss_generic_assist_data_elem->m.ganssAuxiliaryInfoPresent == 1) &&
             ( p_ganss_generic_assist_data_elem->ganssAuxiliaryInfo.t == T_rrlp_GANSSAuxiliaryInformation_ganssID3 )
           )
        {

          uint8 u_num_eph_injected = 0;
          uint32 q_eph_injected_svs = 0;

          supl_glo_eph_svid_freq_num_map_aux ( p_ganss_generic_assist_data_elem->ganssAuxiliaryInfo.u.ganssID3 );

          /* inject any Glonass SV eph. that didn't have freq.Num / svID association before */
          for ( u_i =0; u_i < N_GLO_SV; u_i++)
          {

            if ( ( p_supl_session_status->z_prot_glo_eph_buffer[u_i].z_eph_glo_data.u_SvId > GLO_SLOTID_OFFSET ) &&
                 ( p_supl_session_status->z_prot_glo_eph_buffer[u_i].z_eph_glo_data.u_SvId < (N_GLO_SV+GLO_SLOTID_OFFSET+1) ) &&
                 ( p_supl_session_status->z_prot_glo_eph_buffer[u_i].u_freq_num_valid == TRUE )
               )
            {
              /* inject this SV eph */
              tm_rrlp_glo_eph_to_TmCore_post_data_payload
                ( &p_supl_session_status->z_prot_glo_eph_buffer[u_i],
                  &z_tm_post_data_payload );

              if ( tm_post_data ( TM_PRTL_TYPE_UMTS_UP,
                                  p_supl_session_status->tm_active_session_handle,
                                  TM_POST_DATA_TYPE_EPH_GLO,
                                  &z_tm_post_data_payload
                                ) == FALSE
                 )
              {
                MSG_HIGH("Glo eph. SV %u inj. to TmCore failed",
                         p_supl_session_status->z_prot_glo_eph_buffer[u_i].z_eph_glo_data.u_SvId, 0, 0);
              }
              else
              {
                u_num_eph_injected++;
                q_eph_injected_svs |=
                  1L << ( p_supl_session_status->z_prot_glo_eph_buffer[u_i].z_eph_glo_data.u_SvId - GLO_SLOTID_OFFSET - 1 );
              }
              /* reset these flags so that they won't get injected again later */
              p_supl_session_status->z_prot_glo_eph_buffer[u_i].z_eph_glo_data.u_SvId = 0;
              p_supl_session_status->z_prot_glo_eph_buffer[u_i].u_freq_num_valid = FALSE;
            }

          }

          if ( u_num_eph_injected !=0 )
          {
            MSG_MED("SUPL GLONASS eph posted to TmCore (when getting aux) 0x%lx, totoal SVs = %u",
                    q_eph_injected_svs, u_num_eph_injected, 0 );
          }

        } /* rrlp_ganssAuxiliaryInfo for GLONASS present */
        else
        {
           MSG_HIGH("GanssAux not present or not for GLONASS: %u, %u",
                    p_ganss_generic_assist_data_elem->m.ganssAuxiliaryInfoPresent,
                    p_ganss_generic_assist_data_elem->ganssAuxiliaryInfo.t, 0);
        }

      } /* ganssID indicates GLONASS */

      else /* ganssID absent (Gaelio); ganssID other than Glonass(3) -- neither supported */
      {
        MSG_MED("Discard ganssAssist: ganssIdPresent=%u, ganssID=%u",
                 p_ganss_generic_assist_data_elem->m.ganssIDPresent,
                 p_ganss_generic_assist_data_elem->ganssID,
                 0 );
      }



      /* **** Inject Glonass SV directions **** */
      /* RefMeasurement is only usable for current MSA session:  match SV_ID with freq. number   */
      if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED )
      {

        if ( ( p_ganss_generic_assist_data_elem->m.ganssAuxiliaryInfoPresent == 1) &&
             ( p_ganss_generic_assist_data_elem->ganssAuxiliaryInfo.t == T_rrlp_GANSSAuxiliaryInformation_ganssID3 )
           )
        {
          supl_glo_acqasst_svid_freq_num_map_aux (p_ganss_generic_assist_data_elem->ganssAuxiliaryInfo.u.ganssID3,
                                                  &p_supl_session_status->z_prot_glo_acqasst_buffer);
        }
        else
        {
          MSG_HIGH("No valid aux. assistance available: %u,  %u",
                   p_ganss_generic_assist_data_elem->m.ganssAuxiliaryInfoPresent,
                   p_ganss_generic_assist_data_elem->ganssAuxiliaryInfo.t, 0);
        }

        if (TRUE == p_supl_session_status->z_prot_glo_acqasst_buffer.u_freq_num_valid)
        {
          if ((p_supl_session_status->z_prot_glo_acqasst_buffer.gnss_sv_dir.u_NumSvs > 0) &&
              (p_supl_session_status->z_prot_glo_acqasst_buffer.gnss_sv_dir.u_NumSvs <= N_GLO_SV))
          {
            // Copy SV direction into TM post struct
            memscpy(&z_tm_post_data_payload.gnss_sv_dir,
                   sizeof(z_tm_post_data_payload.gnss_sv_dir),
                   &p_supl_session_status->z_prot_glo_acqasst_buffer.gnss_sv_dir,
                   sizeof(p_supl_session_status->z_prot_glo_acqasst_buffer.gnss_sv_dir));

            if (FALSE == tm_post_data(TM_PRTL_TYPE_UMTS_UP,
                                      p_supl_session_status->tm_active_session_handle,
                                      TM_POST_DATA_TYPE_SV_DIR_GLO,
                                      &z_tm_post_data_payload
                                      ))
            {
              MSG_ERROR("SUPL RRLP(Glo): AA SV Dir (total %u SVs) Tmcore data post failed during aux. proc",
                        p_supl_session_status->z_prot_glo_acqasst_buffer.gnss_sv_dir.u_NumSvs, 0, 0);
            }
            else
            {
              MSG_HIGH("SUPL RRLP(Glo): AA SV Dir (total %u SVs) posted to Tmcore during aux. proc",
                       p_supl_session_status->z_prot_glo_acqasst_buffer.gnss_sv_dir.u_NumSvs, 0, 0);
            }
          }  /* if SV direction assistance available and valid */

          else
          {
            MSG_HIGH("SUPL RRLP(Glo): AA SV Dir (%u) aren't qualified to post during aux. proc",
                     p_supl_session_status->z_prot_glo_acqasst_buffer.gnss_sv_dir.u_NumSvs, 0, 0);
          }

          /***** Inject Glonass Steering data if there are SVs for this data yet to be injected *****/
          if ((p_supl_session_status->z_prot_glo_acqasst_buffer.z_steering_glo_data.u_NumSvs > 0) &&
              (p_supl_session_status->z_prot_glo_acqasst_buffer.z_steering_glo_data.u_NumSvs <= N_GLO_SV))
          {
            // Copy over the Glonass time to steering struct
            if (p_supl_session_status->z_set_assist_pos_time_stamp.m.glonassTimeValid)
            {
              /* Copy Steering into TM post struct */
              memscpy(&z_tm_post_data_payload.gnss_aa_data,
                     sizeof(z_tm_post_data_payload.gnss_aa_data),
                     &p_supl_session_status->z_prot_glo_acqasst_buffer.z_steering_glo_data,
                     sizeof(p_supl_session_status->z_prot_glo_acqasst_buffer.z_steering_glo_data));

              /* Fill Glonass time fields */
              z_tm_post_data_payload.gnss_aa_data.u_GloCycleValid =
                 (p_supl_session_status->z_set_assist_pos_time_stamp.z_GlonassTime.u_FourYear != C_GLO_FOUR_YEAR_UNKNOWN);
              z_tm_post_data_payload.gnss_aa_data.u_GloCalDayValid =
                 (p_supl_session_status->z_set_assist_pos_time_stamp.z_GlonassTime.w_Days != C_GLO_DAY_UNKNOWN);

              z_tm_post_data_payload.gnss_aa_data.u_GloNumCycles  = p_supl_session_status->z_set_assist_pos_time_stamp.z_GlonassTime.u_FourYear;
              z_tm_post_data_payload.gnss_aa_data.w_GloCalDay     = p_supl_session_status->z_set_assist_pos_time_stamp.z_GlonassTime.w_Days;
              z_tm_post_data_payload.gnss_aa_data.q_Msec          = p_supl_session_status->z_set_assist_pos_time_stamp.z_GlonassTime.q_GloMsec;
              z_tm_post_data_payload.gnss_aa_data.u_TimeRefIsGnss = TRUE;
              z_tm_post_data_payload.gnss_aa_data.q_SvSteerSrc    = MGP_GNSS_SV_STEER_SRC_GLO;

              MSG_6(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "SUPL RRLP(Glo): AA-Steering to post with Glonass Time: %d, %d, %u, %u, %d, %lu, %u",
                    z_tm_post_data_payload.gnss_aa_data.u_GloCycleValid,
                    z_tm_post_data_payload.gnss_aa_data.u_GloCalDayValid,
                    z_tm_post_data_payload.gnss_aa_data.u_GloNumCycles,
                    z_tm_post_data_payload.gnss_aa_data.w_GloCalDay,
                    z_tm_post_data_payload.gnss_aa_data.q_Msec,
                    z_tm_post_data_payload.gnss_aa_data.u_TimeRefIsGnss);

              /* Post Glonass AA data to TM. This would inturn ask LM to turn MGP receiver ON */
              if (FALSE == tm_post_data(TM_PRTL_TYPE_UMTS_UP,
                                        p_supl_session_status->tm_active_session_handle,
                                        TM_POST_DATA_TYPE_AA_GLO,
                                        &z_tm_post_data_payload
                                        ))
              {
                MSG_ERROR("SUPL RRLP(Glo): AA-Steering Tmcore data post failed", 0, 0, 0);
              }
              else
              {
                MSG_HIGH("SUPL RRLP(Glo): AA-Steering posted to TmCore", 0, 0, 0);

                /* For ME to generate GLONASS measurements both SV Dir & Steering should be injected successful. */
                p_supl_session_status->u_glo_meas_status = SM_GNSS_GLO_AA_INJECTED;

                /* Update flag to inform TM sess continue to set SM_GNSS_LS_REQ_GLO_MEAS in the LM message */
                p_supl_session_status->u_glo_meas_status |= SM_GNSS_LS_REQ_GLO_MEAS;


                /* reset these time & freqNum flags so that they won't get injected again later */
                p_supl_session_status->z_set_assist_pos_time_stamp.m.glonassTimeValid = FALSE;
                p_supl_session_status->z_prot_glo_acqasst_buffer.u_freq_num_valid = FALSE;
              }
            } /* GLONASS time valid */
          } /* if GLONASS steering assitance yet to be injected */
        } /* if freq. number / SVID association established */
        else
        {
          MSG_HIGH("Not injecting GLONASS ref. meas.: lack freq. svID association", 0, 0, 0);
        }

      } /* if MSA mode */


    } /* for-loop:  ganssGenericAssistDataList */

  } /* ganssGenericAssistDataList present */


  return TRUE;
}


/*
 ******************************************************************************
 *
 * supl_rrlp_msr_pos_req_proc
 *
 * Function description:
 *  This function processes the OSS ASN.1 decoded RRLP-msrPosReq and prepares
 *  such data for SUPL usage.
 *
 * Parameters:
 *
 *  p_mpr: pointer to the OSS ASN.1 decoded RRLP-msrPosReq data.
 *
 * Return value:
 *
 *
 ******************************************************************************
*/

static boolean supl_rrlp_msr_pos_req_proc( rrlp_MsrPosition_Req  *p_mpr )
{
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;
  rrlp_PositionInstruct *p_rrlp_pos_instruction = &p_mpr->positionInstruct;
  boolean u_ret = TRUE;


  if ( p_rrlp_pos_instruction->positionMethod == rrlp_PositionMethod_eotd  )
  {
    MSG_MED("SUPL: rrlp-EOTD not supported", 0, 0, 0);
    if ( !tm_umts_up_supl_suplpos_rrlp_mpr_err_tx(rrlp_methodNotSupported))
    {
      tm_umts_up_supl_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
      tm_umts_up_supl_session_init(FALSE);
    }
    return FALSE;
  }

  /* when RRLP measurePosReq is received, the "more-to-come" flag should
     be cleared */
  p_supl_session_status->z_network_assist.u_MoreToCome = FALSE;

  /**** SET-initiated: check consistency with regard to the gps mode */

  if  ( p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_UI )
  {

    if ( p_rrlp_pos_instruction->methodType.t == rrlp_msAssisted_chosen )
    {
      /** For SET-Initiated, check to see if the SLP still follows the previous
          agreement with the SET on methodType */
      if ( p_supl_session_status->u_agps_mode != C_SUPL_AGPS_MODE_UE_ASSISTED )
      {
        MSG_HIGH("SUPL: rrlp-msrPosReq methodType mismatch", 0, 0, 0);
        if ( !tm_umts_up_supl_suplpos_rrlp_mpr_err_tx(rrlp_methodNotSupported))
        {
          tm_umts_up_supl_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
          tm_umts_up_supl_session_init(FALSE);
        }
        return FALSE;
      }
    }

    else if ( p_rrlp_pos_instruction->methodType.t == rrlp_msBased_chosen )
    {
      if ( p_supl_session_status->u_agps_mode != C_SUPL_AGPS_MODE_UE_BASED )
      {
        MSG_HIGH("SUPL: rrlp-msrPosReq methodType mismatch", 0, 0, 0);
        if ( !tm_umts_up_supl_suplpos_rrlp_mpr_err_tx(rrlp_methodNotSupported))
        {
          tm_umts_up_supl_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
        }
        return FALSE;
      }
    } /* close else-if-rrlp_msBased_chosen */

    /* other two methodType.choice: msBasedPreferred and msAssistedPreferred should not alter
       the agreed mode */

  } /* close if-SET-Initiated */


  /**** For NI, this is where the final settlement of the gps mode takes place ***/

  else if ( p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_NI )
  {
    boolean u_ni_gps_method_settled = TRUE;

    /* if RRLP asks for a definitive gps mode, use that */
    if ( p_rrlp_pos_instruction->methodType.t == rrlp_msBased_chosen )
    {
      if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_UP_MS_BASED) )
      {
        /* may or may not be what SUPL_INIT has */
        p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_BASED;
      }
      else
      {
        u_ni_gps_method_settled = FALSE;
      }
    }
    else if ( p_rrlp_pos_instruction->methodType.t == rrlp_msAssisted_chosen )
    {
      if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_UP_MS_ASSISTED) )
      {
        /* may or may not be what SUPL_INIT has */
        p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_ASSISTED;
      }
      else
      {
        u_ni_gps_method_settled = FALSE;
      }
    }

    /* other two methodType.choice: msBasedPreferred and msAssistedPreferred should not alter
       the previously saved mode provided by SUPL_INIT. Only if agps_mode was not set during
       SUPL_INIT processing, because requested mode not supported, will it be set here */
    if ( p_rrlp_pos_instruction->methodType.t == rrlp_msBasedPref_chosen )
    {
      if (  p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UNDEF )
      {
        /* not chosen during SUPL_INIT processing, decide now */
        if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_UP_MS_BASED) )
        {
          p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_BASED;
        }
        else if (  TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_UP_MS_ASSISTED) )
        {
          p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_ASSISTED;
        }
        else
        {
          u_ni_gps_method_settled = FALSE;
        }
      } /* agps_mode not set during SUPL_INIT proc. */
    } /* msBasedPref */

    if ( p_rrlp_pos_instruction->methodType.t == rrlp_msAssistedPref_chosen )
    {
      if (  p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UNDEF )
      {
        /* not chosen during SUPL_INIT processing, decide now */
        if (  TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_UP_MS_ASSISTED) )
        {
          p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_ASSISTED;
        }
        else if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_UP_MS_BASED) )
        {
          p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_BASED;
        }
        else
        {
          u_ni_gps_method_settled = FALSE;
        }
      } /* agps_mode not set during SUPL_INIT proc. */
    } /* msAssistedPref */

    /* if the SET is provisioned to support neither msBased nor msAssisted, no gps session
       can proceed */
    if ( !( TM_CORE_CONFIG_DD(positioning_modes_supported) &
            ( PDSM_POS_MODE_UP_MS_BASED | PDSM_POS_MODE_UP_MS_ASSISTED )
          )
       )
    {
      u_ni_gps_method_settled = FALSE;
    }

    /* if RRLP requested agps mode is not supported, end the session */
    if ( ! u_ni_gps_method_settled )
    {
      if ( !tm_umts_up_supl_suplpos_rrlp_mpr_err_tx(rrlp_methodNotSupported))
      {
        tm_umts_up_supl_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
      }
      return FALSE;
    }

    /* if TM session was not started duing SUPL_INIT processing, because
       the SUPL_INIT requested gps_mode not provisioned, start it now */
    if (p_supl_session_status->tm_active_session_handle == TM_SESS_HANDLE_NONE)
    {
      p_supl_session_status->tm_active_session_handle = TM_SESS_HANDLE_UMTS_UP_NI;

      if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_BASED )
      {
        /* this will ask pdapi to generate pdsm_get_pos() to TmCore */
        (void )tm_umts_common_ni_client_start_session
                (TM_SESS_OPERATION_MODE_MSB, 0, TM_MAX_SESSION_TIMEOUT, 1, 0, MT_UP, FALSE);
      }
      else if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED )
      {
        /* this will ask pdapi to generate pdsm_get_pos() to TmCore */
        (void )tm_umts_common_ni_client_start_session
                (TM_SESS_OPERATION_MODE_MSA, 0,TM_MAX_SESSION_TIMEOUT, 1,0, MT_UP, FALSE);
      }
    } /* TM session not started */

  } /* close if-NI */

  /* action on the RRLP provided QoS */
  supl_rrlp_msr_pos_req_qos_proc(p_mpr);

  if ( p_mpr->m.referenceAssistDataPresent )
  {
    /* Not supported */
  }

  if ( p_mpr->m.msrAssistDataPresent )
  {
    /* Not supported */
  }

  if ( p_mpr->m.systemInfoAssistDataPresent )
  {
    /* Not supported */
  }

  if ( p_mpr->m.gps_AssistDataPresent )
  {
    u_ret = supl_rrlp_gps_assist_proc( &p_mpr->gps_AssistData.controlHeader );
  }

  if ( p_mpr->m.extensionContainerPresent )
  {
    /* Not supported */
  }

  if ( p_mpr->m.rel98_MsrPosition_Req_extensionPresent )
  {
    /* Not supported */
  }

  if ( p_mpr->m.rel5_MsrPosition_Req_extensionPresent )
  {
    p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.z_rrlp_rel5_ext.rel5_ext_present = TRUE;

    p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.z_rrlp_rel5_ext.rel5_ext_ref.smlc_code =
      p_mpr->rel5_MsrPosition_Req_extension.extended_reference.smlc_code;

    p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.z_rrlp_rel5_ext.rel5_ext_ref.transaction_ID =
      p_mpr->rel5_MsrPosition_Req_extension.extended_reference.transaction_ID;
  }

  if ( p_mpr->m.rel7_MsrPosition_Req_extensionPresent )
  {
    p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.z_rrlp_rel7_ext.rel7_ext_present = TRUE;

    if ( p_mpr->rel7_MsrPosition_Req_extension.m.velocityRequestedPresent )
    {
      p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.z_rrlp_rel7_ext.u_velocity_requested = TRUE;
    }

    if ( tm_umts_up_supl_rrlp_assisted_glonass_enabled() )
    {
      if ( p_mpr->rel7_MsrPosition_Req_extension.m.ganssPositionMethodPresent == 1)
      {

        p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.z_rrlp_rel7_ext.rel7_ext_present = TRUE;

        if ( p_mpr->rel7_MsrPosition_Req_extension.ganssPositionMethod.data[0] & rrlp_GANSSPositioningMethod_rrlp_gps )
        {
          p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.z_rrlp_rel7_ext.u_ganss_pos_methods_bitmap |=
            RRLP_GANSS_POS_METHOD_GPS;
        }
        if ( ( p_mpr->rel7_MsrPosition_Req_extension.ganssPositionMethod.data[0] & rrlp_GANSSPositioningMethod_rrlp_glonass ) &&
             (tm_core_pos_mode_allows_agnss_msb_up( TM_POS_PRTL_RRLP ) ||
              tm_core_pos_mode_allows_agnss_msa_up( TM_POS_PRTL_RRLP ) ||
              ( TM_CONFIG_POS_MODE_BIT_IS_SET( PDSM_POS_MODE_STANDALONE ) != 0)
             ) )
        {
          p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.z_rrlp_rel7_ext.u_ganss_pos_methods_bitmap |=
            RRLP_GANSS_POS_METHOD_GLONASS;
        }
      }

      if ( p_mpr->m.rel7_MsrPosition_Req_extensionPresent == 1)
      {

        u_ret = supl_rrlp_ganss_assist_proc (&p_mpr->rel7_MsrPosition_Req_extension.ganss_AssistData.ganss_controlHeader);
      }
    }

  }

  return u_ret;
}


/*
 ******************************************************************************
 *
 * supl_rrlp_msr_pos_req_qos_proc
 *
 * Function description:
 *  This function processes the OSS ASN.1 decoded RRLP-msrPosReq's
 *  PositionInstruct part, passes QoS to LM if needed, and sets RRLP response
 *  timer.
 *
 * Parameters:
 *
 *  p_mpr: pointer to the OSS ASN.1 decoded RRLP-msrPosReq.
 *
 * Return value:  none.
 *
 *
 ******************************************************************************
*/

static void supl_rrlp_msr_pos_req_qos_proc( rrlp_MsrPosition_Req *p_mpr )
{
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;

  rrlp_PositionInstruct *p_rrlp_pos_instruction = &p_mpr->positionInstruct;

  /* Determine whether the REL-7 responseTime is present */
  if ( ( p_mpr->m.rel7_MsrPosition_Req_extensionPresent ) &&
       ( p_mpr->rel7_MsrPosition_Req_extension.m.requiredResponseTimePresent )
     )
  {
    p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.u_measure_response_time_sec =
      p_mpr->rel7_MsrPosition_Req_extension.requiredResponseTime;
  }
  else
  {
    p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.u_measure_response_time_sec =
      (uint32)((uint32)1 << p_rrlp_pos_instruction->measureResponseTime);
  }

  if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED )
  {

    /* let LM times out first, so if there is a previously disqualified position, it can be reported at LM timeout */
    if ( p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_UI )
    {
      /* for UI, if the pdapi provided QoS timer is longer than RRLP timer ( minus 2 sec. ), use the RRLP timer */
      if ( p_supl_session_status->z_session_qos.u_gps_session_timeout_value_secs >
             (p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.u_measure_response_time_sec - 2)
         )
      {
        p_supl_session_status->z_session_qos.u_gps_session_timeout_value_secs =
          p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.u_measure_response_time_sec - 2;
      }
    }
    else /* for NI, RRC timer (minus 2 sec.) is the only timer */
    {
      p_supl_session_status->z_session_qos.u_gps_session_timeout_value_secs =
        p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.u_measure_response_time_sec - 2;
    }

    /* adopt the network provided accuracy threshold, if provided, though there should be no
       effect on MGP */

    /* the mode is set for msAssisted but the RRLP could have asked for either:
       msAssisted, msAssistedPref, or msBasedPref */
    if (  (p_rrlp_pos_instruction->methodType.t == rrlp_msAssisted_chosen ) &&
          ( p_rrlp_pos_instruction->methodType.u.msAssisted != NULL )
       )
    {
      if ( p_rrlp_pos_instruction->methodType.u.msAssisted->m.accuracyPresent )
      {
        p_supl_session_status->z_session_qos.q_hor_acc_meters =
        (uint32)tm_umts_common_utils_GADK_to_meter( TRUE, p_rrlp_pos_instruction->methodType.u.msAssisted->accuracy );
      }
    }
    else if (p_rrlp_pos_instruction->methodType.t == rrlp_msAssistedPref_chosen )
    {
      p_supl_session_status->z_session_qos.q_hor_acc_meters =
      (uint32)tm_umts_common_utils_GADK_to_meter( TRUE, p_rrlp_pos_instruction->methodType.u.msAssistedPref );
    }
    else if (p_rrlp_pos_instruction->methodType.t == rrlp_msBasedPref_chosen )
    {
      p_supl_session_status->z_session_qos.q_hor_acc_meters =
      (uint32)tm_umts_common_utils_GADK_to_meter( TRUE, p_rrlp_pos_instruction->methodType.u.msBased );
    }

    /* set the timer so that if no fix by its expiration, respond to the network with no-fix */
    supl_set_cf_state(C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER, 0);

  } /* end if SET-Assisted */

  else if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_BASED )
  {
    if ( !p_supl_session_status->u_pdapi_fix_reported )
    {
      /* Note for SI-SB, RRLP is supposed to be of AssistanceData. However, if measurePosReq
         arrives, it will be processed here too, and the SLP provided QoP shall override
         pdapi's original QoS. */

      /* let LM times out first, so if there is a previously disqualified position, it can be reported at LM timeout */
      p_supl_session_status->z_session_qos.u_gps_session_timeout_value_secs =
        p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.u_measure_response_time_sec - 2;

      /* adopt the network provided accuracy threshold */

      /* the mode is set for msBased the RRLP could have asked for either:
         msBased, msBasedPref, or msAssistedPref */

      if (p_rrlp_pos_instruction->methodType.t == rrlp_msBased_chosen )
      {
        p_supl_session_status->z_session_qos.q_hor_acc_meters =
        (uint32)tm_umts_common_utils_GADK_to_meter( TRUE, p_rrlp_pos_instruction->methodType.u.msBased );
      }
      else if (p_rrlp_pos_instruction->methodType.t == rrlp_msBasedPref_chosen )
      {
        p_supl_session_status->z_session_qos.q_hor_acc_meters =
        (uint32)tm_umts_common_utils_GADK_to_meter( TRUE, p_rrlp_pos_instruction->methodType.u.msBasedPref );
      }
      else if (p_rrlp_pos_instruction->methodType.t == rrlp_msAssistedPref_chosen )
      {
        p_supl_session_status->z_session_qos.q_hor_acc_meters =
        (uint32)tm_umts_common_utils_GADK_to_meter( TRUE, p_rrlp_pos_instruction->methodType.u.msAssistedPref );
      }


      /* set the timer so that if no fix by its expiration, respond to the network with no-fix. */
      supl_set_cf_state(C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER, 0);

      /* inform TmCore of session-Continue, using QoS saved in p_supl_session_status->z_session_qos */
      tm_umts_up_supl_inform_core_nt_assist_end();
    }
    else
    {
      MSG_MED("SUPL: pos. already obtained, no action on RRLP-mspReq QoP", 0, 0, 0 );
    }

  } /* close else-if-msBased */


}

/*
 ******************************************************************************
 * Function description:
 *  Decode ASN1 encoded RRLP messages and process the assistance data
 *
 * Parameters:
 *
 *  Data_ptr: pointer to the lowest address byte in RRLP data buffer
 *
 *  w_max_len_byte: maximum possible length of the RRLP message(s), possibly
 *                  multiple RRLP messages
 *
 *  oss_decode_status_for_calling_func: pointer to OSS decoder error code
 *
 * Return value:
 *
 *    TRUE: successfully docoded; FALSE: protocol error has occured.
 *
 *
 ******************************************************************************
*/
static boolean supl_rrlp_decode_proc ( tm_network_assist_s_type  *p_tm_network_assist,
                                       uint8*            Data_ptr,
                                       uint16            w_max_len_byte,
                                       int*              asn1_decode_status_for_calling_func_ptr )
{
  rrlp_PDU           z_rrlp_pdu_decoded;
  rrlp_PDU *p_rrlp = &z_rrlp_pdu_decoded;
  uint32 success = TRUE;
  int asn1_decode_status =  0xFFFFFFFF;
  rrlp_ErrorCodes rrlp_protocol_reason;

  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;

  memset(&z_rrlp_pdu_decoded, 0, sizeof(rrlp_PDU));

  tm_osys_asn1_decode_pdu( Data_ptr, w_max_len_byte, (void *)p_rrlp, PROTO_RRLP, &asn1_decode_status);

  *asn1_decode_status_for_calling_func_ptr = asn1_decode_status;

  if ( asn1_decode_status == 0 )
  {

    p_tm_network_assist->radio_link_prot_params.z_rrlp_params.b_RefNum = (int8)p_rrlp->referenceNumber;

    switch ( p_rrlp->component.t )
    {
      case rrlp_msrPositionReq_chosen:
      {
        p_tm_network_assist->radio_link_prot_params.z_rrlp_params.u_rrlp_choice = C_RRLP_CHOICE_MSRPOSREQ;

        if ( ! supl_rrlp_msr_pos_req_proc ( p_rrlp->component.u.msrPositionReq ) )
        {
          MSG_HIGH("SUPL: err RRLP-msrPosReq", 0, 0, 0);
          success = FALSE;
        }
        break;
      }

      case rrlp_msrPositionRsp_chosen:
      {
        /* We should never be receiving this */
        rrlp_protocol_reason = rrlp_ErrorCodes_unDefined;
        (void)supl_SuplPos_RRLP_protocol_err_tx( p_tm_network_assist->radio_link_prot_params.z_rrlp_params.b_RefNum,
                                          rrlp_protocol_reason );
        MSG_HIGH("SUPL: Unexpected RRLP-msrPosRsp received",0,0,0);
        success = FALSE;
        break;
      }

      case rrlp_assistanceData_chosen:
      {
        rrlp_AssistanceData *p_Assist = p_rrlp->component.u.assistanceData;

        p_tm_network_assist->radio_link_prot_params.z_rrlp_params.u_rrlp_choice = C_RRLP_CHOICE_ASSISTDATA;

        if ( p_Assist->m.gps_AssistDataPresent ) /* other types of assist. data ignored */
        {
          if ( !supl_rrlp_gps_assist_proc( &p_rrlp->component.u.assistanceData->gps_AssistData.controlHeader ))
          {
            success = FALSE;
          }
        }

        /* Before entering this next IF, make sure p_tm_network_assist->u_MoreToCome
           is set to FALSE because although SnapTrack SMLC always provide this
           optional sequence of "more-assistance-to-come" to indicate whether
           or not more is on the way, non-SnapTrack yet compliant SMLCs may not
           have that sequence, in which case it means no-more-to-come. */

        p_tm_network_assist->u_MoreToCome = FALSE;

        if ( p_Assist->m.moreAssDataToBeSentPresent )
        {
          if ( p_Assist->moreAssDataToBeSent == rrlp_MoreAssDataToBeSent_moreMessagesOnTheWay )
          {
            p_tm_network_assist->u_MoreToCome = TRUE;
          }
        }

        if ( p_Assist->m.rel5_AssistanceData_ExtensionPresent )
        {
          p_tm_network_assist->radio_link_prot_params.z_rrlp_params.z_rrlp_rel5_ext.rel5_ext_present = TRUE;

          p_tm_network_assist->radio_link_prot_params.z_rrlp_params.z_rrlp_rel5_ext.rel5_ext_ref.smlc_code =
            p_Assist->rel5_AssistanceData_Extension.extended_reference.smlc_code;

          p_tm_network_assist->radio_link_prot_params.z_rrlp_params.z_rrlp_rel5_ext.rel5_ext_ref.transaction_ID =
            p_Assist->rel5_AssistanceData_Extension.extended_reference.transaction_ID;
        }

        if ( tm_umts_up_supl_rrlp_assisted_glonass_enabled() &&
             (p_Assist->m.rel7_AssistanceData_ExtensionPresent == 1)
           )
        {
          if ( p_Assist->rel7_AssistanceData_Extension.m.ganss_AssistDataPresent == 1)
          {
            supl_rrlp_ganss_assist_proc(&p_Assist->rel7_AssistanceData_Extension.ganss_AssistData.ganss_controlHeader);
          }
        }

        break;
      }

      case rrlp_assistanceDataAck_chosen:
      {
        /* We should never be receiving this */
        rrlp_protocol_reason = rrlp_ErrorCodes_unDefined;
        (void)supl_SuplPos_RRLP_protocol_err_tx( p_tm_network_assist->radio_link_prot_params.z_rrlp_params.b_RefNum,
                                          rrlp_protocol_reason );
        MSG_HIGH("SUPL: Unexpected RRLP-msrPosRsp received",0,0,0);
        success = FALSE;
        break;
      }

      case rrlp_protocolError_chosen:
      {
        p_tm_network_assist->radio_link_prot_params.z_rrlp_params.u_rrlp_choice = C_RRLP_CHOICE_PROTCOL_ERR;
        MSG_HIGH("SUPL: SLP detects RRLP protocol error from SET", 0, 0, 0);
        tm_umts_up_supl_callflow_disrupt_handler(); /* the SET will not try to correct itself */
        success = FALSE;
        break;
      }

      /* "The RRLP Positioning Capability Transfer procedure introduced in RRLP
         Release 7 (section 2.3a in [3GPP RRLP]) SHALL NOT be used." */
      case rrlp_posCapabilityReq_chosen:
      case rrlp_posCapabilityRsp_chosen:
      {
        rrlp_protocol_reason = rrlp_ErrorCodes_unDefined;
        (void)supl_SuplPos_RRLP_protocol_err_tx( p_tm_network_assist->radio_link_prot_params.z_rrlp_params.b_RefNum,
                                          rrlp_protocol_reason );
        MSG_HIGH("SUPL: Unexpected RRLP-rrlp_posCapability %u received",p_rrlp->component.t,0,0);
        success = FALSE;
        break;
      }


      default:
      {
        /* Unknown type */
        MSG_LOW("LSM - Unknown RRLP-choice received",0,0,0);
        success = FALSE; /* shouldn't happen as OSS ASN.1 decoder will fail */
        break;
      }
    }

    tm_osys_asn1_free_pdu (p_rrlp , PROTO_RRLP);

  }

  else
  {
    MSG_HIGH("SUPL: RRLP Decode Failure",0,0,0);

    if (asn1_decode_status != C_ASN1_DECODE_BUFFER_NOT_ALLOCATED)
    {
      tm_osys_asn1_free_pdu (p_rrlp , PROTO_RRLP);
    }

    /* when this happens, there is no information about the current
       message's reference number which makes it impossible to return with RRLP-protocolError;
       Send SuplEnd, and hanle this as call flow disruption */
    (void) tm_umts_up_supl_supl_end_prepare_or_tx(&p_supl_session_status->z_session_id.z_Slp_Sess_Id,
                                           NULL,
                                           supl_StatusCode_protocolError,
                                           NULL,
                                           p_supl_session_status->q_supl_version);
    tm_umts_up_supl_callflow_disrupt_handler();

    return FALSE;
  }

  return ( success );
}



/*
******************************************************************************
* supl_set_session_id_organizer
*
* Function description:
*   This function re-packages set session ID
*
* Parameters:
*
* p_pdu_decoded : pointer to a structure holding decoded SUPL PDU
*
* p_set_session_id : pointer for the function to pass repackaged
*   set session ID to the calling function.
*
* Return value:
*  None
*
*
******************************************************************************
*/

static void supl_set_session_id_organizer ( const supl_SessionID *p_source_session_id,
                                            supl_set_session_id *p_set_session_id)
{
  uint32 choice;
  uint32 cnt;

  p_set_session_id->w_Session_Id = p_source_session_id->setSessionID.sessionId;

  choice = p_source_session_id->setSessionID.setId.t;
  p_set_session_id->z_Set_Id.set_id_choice = choice;

  /* We only support MSISDN and IMSI */
  if ( choice == C_SUPL_ASN1_CHOICE_SETID_MSISDN )
  {
    uint8 *p_src = p_source_session_id->setSessionID.setId.u.msisdn->data;
    uint8 *p_dest = p_set_session_id->z_Set_Id.u.msisdn.data;

    /* MSISDN */
    for ( cnt = 8; cnt; cnt-- )
    {
      *p_dest++ = *p_src++;
    }
    p_set_session_id->z_Set_Id.u.msisdn.numocts = 8;
  }
  else
  if ( choice == C_SUPL_ASN1_CHOICE_SETID_IMSI )
  {
    uint8 *p_src = p_source_session_id->setSessionID.setId.u.imsi->data;
    uint8 *p_dest = p_set_session_id->z_Set_Id.u.imsi.data;

    /* IMSI */
    for ( cnt = 8; cnt; cnt-- )
    {
      *p_dest++ = *p_src++;
    }
    p_set_session_id->z_Set_Id.u.imsi.numocts = 8;
  }
  else
  /* For any other setID choice a SUPL End msg will be generated with invalid session ID error.
     Save the raw contents of setID for this case too, so that it can be returned in SUPL End. */
  {
    uint8 *p_src = (uint8 *)&p_source_session_id->setSessionID.setId.u;
    uint8 *p_dest = &p_set_session_id->z_Set_Id.u.other_setID.raw_contents[0];

    for ( cnt = OTHER_SETID_SIZE; cnt; cnt-- )
    {
      *p_dest++ = *p_src++;
    }
    p_set_session_id->z_Set_Id.u.other_setID.valid = TRUE;
  }
}



/*
******************************************************************************
* supl_message_overhead_proc
*
* Function description:
*   This function checks the overhead of SUPL messages and
*   indicates to the calling function if it has passed the check.
*
* Parameters:
*
* p_pdu_decoded : pointer to a structure holding decoded SUPL PDU
*
* Return value:
*  slp_mess_header_check_result_e_type
*
******************************************************************************
*/

static slp_mess_header_check_result_e_type  supl_message_overhead_proc
( const supl_ULP_PDU*  p_pdu_decoded,
  uint16*              p_provided_set_session_id
)

{
  supl_status_s_type*     p_supl_session_status = &z_supl_session_status;
  supl_session_id*        p_session_id  = &p_supl_session_status->z_session_id;
  uint8                   u_session_id_consistent = TRUE;

  supl_slp_session_id  z_incoming_slp_session_id;
  supl_set_session_id  z_incoming_set_session_id;

  slp_mess_header_check_result_e_type  e_ret;

  uint16               *p_set_session_id;



  e_ret = C_SLP_MESS_HEADER_STATUS_UNKNOWN;

  if ( p_provided_set_session_id == NULL )
  {
    p_set_session_id = &p_supl_session_status->z_session_id.z_Set_Sess_Id.w_Session_Id;
  }
  else
  {
    p_set_session_id = p_provided_set_session_id;
  }


  memset(&z_incoming_slp_session_id, 0, sizeof(supl_slp_session_id));


  /* any message coming from SLP must contain SLP session ID */
  if ( !p_pdu_decoded->sessionID.m.slpSessionIDPresent )
  {
    MSG_HIGH("Missing SlpSessionID", 0, 0, 0);
    e_ret |= C_SLP_MESS_SLP_SESS_ID_MISSING;
  }

  /******************* Check SLP Session ID *****************************************/

  /* organized SLP session ID and place it in z_incoming_slp_session_id */
  if ( p_pdu_decoded->sessionID.m.slpSessionIDPresent )
  {
    supl_util_server_session_id_organizer ( &p_pdu_decoded->sessionID, &z_incoming_slp_session_id );
  }


  /* Save SLP session ID, even if the message is to be rejected using SUPL_END, since
     SULP_END needs this SLP_session_ID too. Note for SUPLINIT, no such check should be
     done, as it will be the first message on have SLP session ID. No need to check
     SUPL_TRIGGERED_STOP either.  */

  if ( ( p_pdu_decoded->message.t != C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLINIT ) &&
       ( p_pdu_decoded->message.t != C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLTRIGGEREDSTOP )
     )
  {
    if ( supl_SLP_session_id_match_check (&p_session_id->z_Slp_Sess_Id,
                                          &z_incoming_slp_session_id) == FALSE
       )
    {
      /* Callflow termination handled in this routine */
      e_ret |= C_SLP_MESS_SLP_SESS_ID_PROBLEM;
    }
    else
    {
      e_ret = C_SLP_MESS_HEADER_OK_CONSISTENT;
    }


  } /* close if-not-SuplInit or SUPL_TRIGGERED_STOP */

  else /* for SuplInit or SUPL_TRIGGERED_STOP*/
  {
    if ( e_ret == C_SLP_MESS_HEADER_STATUS_UNKNOWN )
    {
      e_ret = C_SLP_MESS_HEADER_OK_NEW;
    }
  }


  /******************************* check SET session ID *****************************/
  u_session_id_consistent = TRUE;

  if ( (p_pdu_decoded->message.t != C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLINIT ) &&
       ( p_pdu_decoded->message.t != C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLTRIGGEREDSTOP  )
     )
  {

   supl_set_session_id_organizer ( &p_pdu_decoded->sessionID, &z_incoming_set_session_id );


    /* anything other than SUPLINIT, incoming message must contain SetSessionId */
    if ( !p_pdu_decoded->sessionID.m.setSessionIDPresent )
    {
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,
            "SUPL: SLP mess. missing SetSessionID: %u",
            p_pdu_decoded->sessionID.m.setSessionIDPresent);

      e_ret |= C_SLP_MESS_SET_SESS_ID_MISSING;
      return e_ret;

    }

    /* anything other than SUPLINIT, incoming SET Session ID should match what the SET has */
    if ( *p_set_session_id !=
         z_incoming_set_session_id.w_Session_Id )
    {
      u_session_id_consistent = FALSE;

      MSG_HIGH( "SUPL: SLP mess SET SessID mismatch %u  (%u)",
                z_incoming_set_session_id.w_Session_Id,
                *p_set_session_id, 0);

    }

    if ( z_incoming_set_session_id.z_Set_Id.set_id_choice == C_SUPL_ASN1_CHOICE_SETID_MSISDN )
    {
      /* Verify MSISDN */
      if ( memcmp( z_incoming_set_session_id.z_Set_Id.u.msisdn.data,
                   z_Set_Id.u.msisdn.data,
                   8 ) )
      {
        u_session_id_consistent = FALSE;
      }
    }
    else
    if ( z_incoming_set_session_id.z_Set_Id.set_id_choice == C_SUPL_ASN1_CHOICE_SETID_IMSI )
    {
      /* Verify IMSI */
      if ( memcmp( z_incoming_set_session_id.z_Set_Id.u.imsi.data,
                   z_Set_Id.u.imsi.data,
                   8 ) )
      {
        u_session_id_consistent = FALSE;
        MSG_HIGH("IMSI mismatch: %u, %u", z_incoming_set_session_id.z_Set_Id.u.imsi.data[7],
                                          z_Set_Id.u.imsi.data[7],
                                          0 );
      }
    }
    else
    {
      /* Unsupported choice */
      u_session_id_consistent = FALSE;
    }

    if ( !u_session_id_consistent )
    {
      e_ret |= C_SLP_MESS_SET_SESS_ID_PROBLEM;

      /* use this in SUPL_END */
      memscpy( &p_supl_session_status->z_session_id.z_Set_Sess_Id,
               sizeof(p_supl_session_status->z_session_id.z_Set_Sess_Id),
              &z_incoming_set_session_id, sizeof(z_incoming_set_session_id));

    }
    else
    {
      if ( e_ret == C_SLP_MESS_HEADER_STATUS_UNKNOWN )
      {
        e_ret = C_SLP_MESS_HEADER_OK_CONSISTENT;
      }
    }

  } /* close if-not-SuplInit or SUPL_TRIGGERED_STOP*/

  else if (p_pdu_decoded->message.t == C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLINIT   ) /* for SuplInit: */
  {
    /* SUPLINIT must NOT contain SetSessionId */
    if ( p_pdu_decoded->sessionID.m.setSessionIDPresent )
    {
      e_ret |= C_SLP_MESS_SUPLINIT_SET_SESS_ID_PROBLEM;
    }
  }

  return e_ret;

}



/*
******************************************************************************
* supl_send_notification_verifcation
*
* Function description:
*   LCS Agent's sending of notification verification to PDAPI
*
* Parameters:
*
*  : pointer to a structure holding decoded Supl PDU
*
* Return value:
*  None
*
*
******************************************************************************
*/
static void supl_send_notification_verifcation(const supl_ULP_PDU* p_pdu_decoded, const uint8 *p_hash)
{

  pdsm_lcs_info_s_type z_notification_info;

  pdsm_lcs_supl_notify_verify_req_s_type *p_supl_pdapi_notif_verif =
    &z_notification_info.lcs_info.supl_req;

  const supl_SUPLINIT *p_suplInit;
  uint8 length = 0;

  supl_slp_session_id      z_local_copy_slp_session_id;

  p_suplInit = p_pdu_decoded->message.u.msSUPLINIT;

  /* initialize the notification info. structure */
  memset(&z_notification_info, 0, sizeof(pdsm_lcs_info_s_type) );

  z_notification_info.client_id = pdsm_ni_client_id_get();
  z_notification_info.lcs_cb_cmd_type = PDSM_LCS_CMD_TYPE_SUPL_REQ;

  MSG_MED("send_notif_verif: clientID=%d", z_notification_info.client_id, 0, 0);

  /* check if optional notification/verification IE is present */
  if ( p_suplInit->m.notificationPresent == 0)
  {
    MSG_MED("SUPL_INIT has No Notification IE", 0, 0, 0);

    /* for all practical purposes, treat this the same as a notification type
       of no-notification-no-verification */
    p_supl_pdapi_notif_verif->notification_priv_type = PDSM_LCS_NO_NOTIFY_NO_VERIFY;

  }

  else /* notif/verif IE is present */
  {

    /* notification_priv_type */
    switch ( p_suplInit->notification.notificationType )
    {
      case supl_noNotificationNoVerification:
      {
        p_supl_pdapi_notif_verif->notification_priv_type = PDSM_LCS_NO_NOTIFY_NO_VERIFY;
        break;
      }

      case supl_notificationOnly:
      {
         p_supl_pdapi_notif_verif->notification_priv_type = PDSM_LCS_USER_NOTIFY_ONLY;
         break;
      }

      case supl_notificationAndVerficationAllowedNA:
      {
         p_supl_pdapi_notif_verif->notification_priv_type = PDSM_LCS_USER_NOTIFY_VERIFY_ALLOW_NO_RESP;
         break;
      }

      case supl_notificationAndVerficationDeniedNA:
      {
         p_supl_pdapi_notif_verif->notification_priv_type = PDSM_LCS_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP;
         break;
      }

      case supl_privacyOverride:
      {
         p_supl_pdapi_notif_verif->notification_priv_type = PDSM_LCS_PRIVACY_OVERRIDE;
         break;
      }

      default:
      {
        /* should not happen */
        MSG_HIGH("Unknown Notif. type received. Denying request",0,0,0);
        break;
      }

    } /* close notification_priv_type */

  } /* close else ( if notification_priv_type is present ) */

  p_supl_pdapi_notif_verif->flags = 0;

  /* pass up the emergency services bit and SLP address */
  if (( p_suplInit->notification.m.ver2_Notification_extensionPresent == 1  ) &&
      (p_suplInit->notification.ver2_Notification_extension.m.emergencyCallLocationPresent == 1))
  {
    p_supl_pdapi_notif_verif->flags |= PDSM_LCS_SUPLINIT_VER2_EXT_EMERGENCY_PRESENT;

    if ( p_suplInit->ver2_SUPL_INIT_extension.m.e_SLPAddressPresent == 1 )
    {
      if ( p_suplInit->ver2_SUPL_INIT_extension.e_SLPAddress.t == 2 )
      {
        p_supl_pdapi_notif_verif->supl_init_ver2_ext_emergency.b_eslp_address_present = TRUE;
        p_supl_pdapi_notif_verif->supl_init_ver2_ext_emergency.e_slp_address.choice = PDSM_SERVER_TYPE_URL_ADDRESS;
        GNSS_STRLCPY (p_supl_pdapi_notif_verif->supl_init_ver2_ext_emergency.e_slp_address.supl_slp_address.server_adrs_url.url,
          p_suplInit->ver2_SUPL_INIT_extension.e_SLPAddress.u.fQDN,
                     C_SUPL_FQDN_VISIBLE_STRING_BYTE_LENGTH - 1);
        // NULL terminate
        p_supl_pdapi_notif_verif->supl_init_ver2_ext_emergency.e_slp_address.supl_slp_address.server_adrs_url.url[C_SUPL_FQDN_VISIBLE_STRING_BYTE_LENGTH - 1] = NULL;

        p_supl_pdapi_notif_verif->supl_init_ver2_ext_emergency.e_slp_address.supl_slp_address.server_adrs_url.url_length = strlen (p_suplInit->ver2_SUPL_INIT_extension.e_SLPAddress.u.fQDN);
      }
    }
  }
  /* encoding type */
  if (p_suplInit->notification.m.encodingTypePresent)
  {
    p_supl_pdapi_notif_verif->flags |= PDSM_LCS_ENCODING_TYPE_PRESENT;

    switch(p_suplInit->notification.encodingType)
    {
      case supl_ucs2:
         p_supl_pdapi_notif_verif->datacoding_scheme = PDSM_SUPL_UCS2;
         break;
      case supl_gsmDefault:
         p_supl_pdapi_notif_verif->datacoding_scheme = PDSM_SUPL_GSM_DEFAULT;
         break;
      case supl_utf8:
         p_supl_pdapi_notif_verif->datacoding_scheme = PDSM_SUPL_UTF8;
         break;
      default:
         p_supl_pdapi_notif_verif->datacoding_scheme = PDSM_SUPL_NOTIF_DISPLAYENCODINGTYPE_UNKNOWN_ENUMERATOR;
         break;
    }
  } /* close encoding-type */

  /* RequestorID */
  if (p_suplInit->notification.m.requestorIdPresent)
  {
    length = p_suplInit->notification.requestorId.numocts;
    p_supl_pdapi_notif_verif->flags |= PDSM_LCS_REQUESTOR_ID_PRESENT;

    /* make sure we don't exceed the local buffer size */
    if (length > PDSM_LCS_REQUESTOR_ID_LEN)  /* pdapi defines requestID array limit, currently 63 bytes */
    {
      length = PDSM_LCS_REQUESTOR_ID_LEN;
    }
    if (length >= ARR_SIZE(p_suplInit->notification.requestorId.data) )
    {
      /* SUPL defines this size of 50 */
      length = ARR_SIZE(p_suplInit->notification.requestorId.data);
    }

    /*  copy the string locally */
    memscpy((void*)&p_supl_pdapi_notif_verif->requestor_id.requestor_id_string[0],
            sizeof(p_supl_pdapi_notif_verif->requestor_id.requestor_id_string),
       p_suplInit->notification.requestorId.data, length);

    p_supl_pdapi_notif_verif->requestor_id.requestor_id_string[PDSM_LCS_REQUESTOR_ID_LEN - 1 ] = NULL;
    p_supl_pdapi_notif_verif->requestor_id.string_len = length;

  } /* close RequestorID */

  /* RequestorIDType */
  if (p_suplInit->notification.m.requestorIdTypePresent)
  {
    p_supl_pdapi_notif_verif->requestor_id.data_coding_scheme =
       (byte)supl_convert_format_indicator_pdsm (p_suplInit->notification.requestorIdType);
  } /* close RequestorIDType */

  /* ClientName */
  if (p_suplInit->notification.m.clientNamePresent)
  {
    length = p_suplInit->notification.clientName.numocts;
    p_supl_pdapi_notif_verif->flags |= PDSM_LCS_CLIENT_NAME_PRESENT;

    /* make sure we don't exceed the local buffer size */
    if (length > PDSM_LCS_CLIENT_NAME_LEN) /* pdapi defines clientName array to be of 63 bytes */
    {
       length = PDSM_LCS_CLIENT_NAME_LEN;
    }
    if (length >= ARR_SIZE(p_suplInit->notification.clientName.data) )
    {
      /* SUPL defines this size of 50 */
      length = ARR_SIZE(p_suplInit->notification.clientName.data);
    }

    /* copy the string locally */
    memscpy((void*)&p_supl_pdapi_notif_verif->client_name.client_name_string[0],
             sizeof(p_supl_pdapi_notif_verif->client_name.client_name_string),
       p_suplInit->notification.clientName.data, length);

    /* null terminate */
    p_supl_pdapi_notif_verif->client_name.client_name_string[PDSM_LCS_CLIENT_NAME_LEN - 1 ] = NULL;

    /* set length */
    p_supl_pdapi_notif_verif->client_name.string_len = length;

  } /* close ClientName */

  /* ClientName Type */
  if (p_suplInit->notification.m.clientNameTypePresent)
  {
    p_supl_pdapi_notif_verif->client_name.data_coding_scheme =
       (byte)supl_convert_format_indicator_pdsm (p_suplInit->notification.clientNameType);
  } /* close ClientName Type */


  /* convert slp session id into the proper format and populate the structure for pdapi */
  supl_util_server_session_id_organizer ( &p_pdu_decoded->sessionID, &z_local_copy_slp_session_id);

  if ( z_local_copy_slp_session_id.presence )
  {
    p_supl_pdapi_notif_verif->supl_slp_session_id.presence = TRUE;

    memscpy( &p_supl_pdapi_notif_verif->supl_slp_session_id.session_id[0],
        sizeof(p_supl_pdapi_notif_verif->supl_slp_session_id.session_id),
            &z_local_copy_slp_session_id.session_Id[0],
            sizeof(z_local_copy_slp_session_id.session_Id));

    if ( z_local_copy_slp_session_id.zSlpAddress.choice == C_SUPL_ASN1_CHOICE_SLPADDRESS_IPADDRESS )
    {
      if ( z_local_copy_slp_session_id.zSlpAddress.u.z_Ip_Address.choice == C_SUPL_ASN1_CHOICE_IPV4ADDRESS )
      {
        p_supl_pdapi_notif_verif->supl_slp_session_id.slp_address.choice = PDSM_SERVER_TYPE_IPV4_ADDRESS;

        memscpy(&p_supl_pdapi_notif_verif->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.server_adrs,
                sizeof(p_supl_pdapi_notif_verif->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.server_adrs),
                &z_local_copy_slp_session_id.zSlpAddress.u.z_Ip_Address.u.Ip_V4.data[0],
                sizeof(z_local_copy_slp_session_id.zSlpAddress.u.z_Ip_Address.u.Ip_V4.data)
              );
      }
      else if ( z_local_copy_slp_session_id.zSlpAddress.u.z_Ip_Address.choice == C_SUPL_ASN1_CHOICE_IPV6ADDRESS )
      {
        p_supl_pdapi_notif_verif->supl_slp_session_id.slp_address.choice = PDSM_SERVER_TYPE_IPV6_ADDRESS;
        memscpy(&p_supl_pdapi_notif_verif->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.server_adrs[0],
                sizeof(p_supl_pdapi_notif_verif->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.server_adrs),
                &z_local_copy_slp_session_id.zSlpAddress.u.z_Ip_Address.u.Ip_V6.data[0],
                sizeof(z_local_copy_slp_session_id.zSlpAddress.u.z_Ip_Address.u.Ip_V6.data)
              );
      }
      else
      {
        /* should never happen, as it wouldn't pass ASN1 decoder */
        MSG_ERROR("SUPL: invalid SLP IP_address type", 0, 0, 0);
      }
    } /* if slp address is of ip_address */

    else if ( z_local_copy_slp_session_id.zSlpAddress.choice == C_SUPL_ASN1_CHOICE_SLPADRESS_FQDN )
    {
      p_supl_pdapi_notif_verif->supl_slp_session_id.slp_address.choice = PDSM_SERVER_TYPE_URL_ADDRESS;

      p_supl_pdapi_notif_verif->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url_length =
        z_local_copy_slp_session_id.zSlpAddress.u.z_fqdn.u_length;

      memscpy (&p_supl_pdapi_notif_verif->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url[0],
               sizeof(p_supl_pdapi_notif_verif->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url),
               &z_local_copy_slp_session_id.zSlpAddress.u.z_fqdn.fqdn[0],
               z_local_copy_slp_session_id.zSlpAddress.u.z_fqdn.u_length
             );
    }
    else
    {
      /* should never happen, as it wouldn't pass ASN1 decoder */
      MSG_ERROR("SUPL: invalid SLP Address choice", 0, 0, 0);
    }

  } /* if SLP ID present */


  if ( p_suplInit->posMethod == supl_PosMethod_agpsSETassisted )
  {
    p_supl_pdapi_notif_verif->pos_method = POSMETHOD_AGPS_SETASSISTED;
  }
  else if ( p_suplInit->posMethod == supl_PosMethod_agpsSETbased )
  {
    p_supl_pdapi_notif_verif->pos_method = POSMETHOD_AGPS_SETBASED;

  }
  else if ( p_suplInit->posMethod == supl_PosMethod_agpsSETassistedpref )
  {
    p_supl_pdapi_notif_verif->pos_method = POSMETHOD_AGPS_SETASSISTED_PREF;

  }
  else if ( p_suplInit->posMethod == supl_PosMethod_agpsSETbasedpref )
  {
    p_supl_pdapi_notif_verif->pos_method = POSMETHOD_AGPS_SETBASED_PREF;

  }
  else if ( p_suplInit->posMethod == supl_PosMethod_autonomousGPS )
  {
    p_supl_pdapi_notif_verif->pos_method = POSMETHOD_AUTONOMOUS_GPS;

  }
  else if ( p_suplInit->posMethod == supl_PosMethod_aFLT )
  {
    p_supl_pdapi_notif_verif->pos_method = POSMETHOD_AFLT;

  }
  else if ( p_suplInit->posMethod == supl_PosMethod_eCID )
  {
    p_supl_pdapi_notif_verif->pos_method = POSMETHOD_ECID;

  }
  else if ( p_suplInit->posMethod == supl_PosMethod_eOTD )
  {
    p_supl_pdapi_notif_verif->pos_method = POSMETHOD_EOTD;

  }
  else if ( p_suplInit->posMethod == supl_PosMethod_oTDOA )
  {
    p_supl_pdapi_notif_verif->pos_method = POSMETHOD_OTDOA;

  }
  else if ( p_suplInit->posMethod == supl_PosMethod_noPosition )
  {
    p_supl_pdapi_notif_verif->pos_method = POSMETHOD_NO_POSITION;

  }
  else if ( p_suplInit->posMethod == supl_PosMethod_ver2_agnssSETbased )
  {
    p_supl_pdapi_notif_verif->pos_method = POSMETHOD_AGPS_SETBASED;
  }
  else if ( p_suplInit->posMethod == supl_PosMethod_ver2_agnssSETassisted )
  {
    p_supl_pdapi_notif_verif->pos_method = POSMETHOD_AGPS_SETASSISTED;
  }
  else if ( p_suplInit->posMethod == supl_PosMethod_ver2_agnssSETbasedpref )
  {
    p_supl_pdapi_notif_verif->pos_method = POSMETHOD_AGPS_SETBASED_PREF;
  }
  else if ( p_suplInit->posMethod == supl_PosMethod_ver2_agnssSETassistedpref )
  {
    p_supl_pdapi_notif_verif->pos_method = POSMETHOD_AGPS_SETASSISTED_PREF;
  }
  else if ( p_suplInit->posMethod == supl_PosMethod_ver2_autonomousGNSS )
  {
    p_supl_pdapi_notif_verif->pos_method = POSMETHOD_AUTONOMOUS_GPS;
  }

  else
  {
    /* supl_PosMethod_ver2_sessioninfoquery of supl_PosMethod_Ext which
       is supported on this version does not go to pdapi anyway; */
       MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,
             "posMethod not for NotifVerif: %d",
             p_suplInit->posMethod);
       return;
  }

  if ( p_suplInit->m.qoPPresent )
  {
    p_supl_pdapi_notif_verif->flags |= PDSM_LCS_SUPL_QOP_VALID;

    p_supl_pdapi_notif_verif->supl_qop.horacc = (uint8)p_suplInit->qoP.horacc;

    if ( p_suplInit->qoP.m.veraccPresent )
    {
      p_supl_pdapi_notif_verif->supl_qop.bit_mask = PDSM_LCS_SUPL_QOP_VERACC_VALID;
      p_supl_pdapi_notif_verif->supl_qop.veracc = (uint8)p_suplInit->qoP.veracc;
    }
    if ( p_suplInit->qoP.m.maxLocAgePresent )
    {
      p_supl_pdapi_notif_verif->supl_qop.bit_mask |= PDSM_LCS_SUPL_QOP_MAXAGE_VALID;
      p_supl_pdapi_notif_verif->supl_qop.maxLocAge = (uint16)p_suplInit->qoP.maxLocAge;
    }
    if ( p_suplInit->qoP.m.delayPresent )
    {
      p_supl_pdapi_notif_verif->supl_qop.bit_mask |= PDSM_LCS_SUPL_QOP_DELAY_VALID;
      p_supl_pdapi_notif_verif->supl_qop.delay = (uint8)p_suplInit->qoP.delay;
    }
  } /* if SuplInit contains the optional QoP */


  /* if trigger type present */
  if ( p_suplInit->ver2_SUPL_INIT_extension.m.triggerTypePresent )
  {
    if ( p_suplInit->ver2_SUPL_INIT_extension.triggerType ==
                    supl_TriggerType_periodic )
    {
      p_supl_pdapi_notif_verif->supl_init_ver2_ext.trigger_type =
                      PDAPI_SUPLINIT_TRIGGER_TYPE_PERIODIC;
    }
  else if ( p_suplInit->ver2_SUPL_INIT_extension.triggerType ==
                    supl_TriggerType_areaEvent )
  {
    p_supl_pdapi_notif_verif->supl_init_ver2_ext.trigger_type =
                      PDAPI_SUPLINIT_TRIGGER_TYPE_AREA_EVENT;
  }

    if (( p_suplInit->ver2_SUPL_INIT_extension.triggerType ==
                    supl_TriggerType_periodic) ||
    ( p_suplInit->ver2_SUPL_INIT_extension.triggerType ==
                    supl_TriggerType_areaEvent))
    {

    /* set validity for trigger type to valid */
    p_supl_pdapi_notif_verif->supl_init_ver2_ext.validity =
                    PDAPI_SUPLINIT_VER2_EXT_BITMAP_TRIGTYPE_VALID;

    /* set flag for SUPLINIT_VER2_EXT_PRESENT */
    p_supl_pdapi_notif_verif->flags |= PDSM_LCS_SUPLINIT_VER2_EXT_PRESENT;
    }
  else
  {
    MSG_MED("Unrecongnized trigger type with value %d, ignored",
             p_suplInit->ver2_SUPL_INIT_extension.triggerType, 0, 0 );
  }

  }  /* if trigger type present */


  p_supl_pdapi_notif_verif->user_response_timer = 2; /* QQQ: arbitary for now */

  p_supl_pdapi_notif_verif->supl_init_source = z_supl_init_source; // SUPL Init Source (1x-SMS/GSM-SMS/UDP/WapPush)
  z_supl_init_source = PDSM_SUPL_INIT_SOURCE_TYPE_UNKNOWN;

  memscpy(p_supl_pdapi_notif_verif->supl_hash, sizeof(p_supl_pdapi_notif_verif->supl_hash), p_hash, sizeof(p_supl_pdapi_notif_verif->supl_hash));


  (void) tm_request_app_response ( TM_PRTL_TYPE_UMTS_UP,
                            PDSM_LCS_EVENT_SUPL_NOTIFY_VERIFY_REQ,
                            &z_notification_info
                          );

   //lsm_log_notification_verification_request(&z_notification_info);    /* QQQ */

   //lsm_dm_ui_req(z_notification_info.client_id, &z_notification_info);


}


/*
******************************************************************************
* supl_convert_format_indicator_pdsm
*
* Function description:
*   Helper function to convert SUPL's FormatIndicator to the PDSM defined ones.
*
* Parameters:
*
*  : supl_FormatIndicator to be converted to pdsm_lcs_notify_verify_e_type
*
* Return value:
*  pdsm_lcs_supl_format_indicator_e_type
*
*
******************************************************************************
*/
static pdsm_lcs_supl_format_indicator_e_type supl_convert_format_indicator_pdsm (supl_FormatIndicator formatInd)
{
   pdsm_lcs_supl_format_indicator_e_type    lcsFormatInd = PDSM_SUPL_FORMAT_OSS_UNKNOWN;

   switch(formatInd)
   {
   case supl_logicalName:
      lcsFormatInd = PDSM_SUPL_FORMAT_LOGICAL_NAME;
      break;
   case supl_e_mailAddress:
      lcsFormatInd = PDSM_SUPL_FORMAT_EMAIL_ADDRESS;
      break;
   case supl_msisdn:
      lcsFormatInd = PDSM_SUPL_FORMAT_MSISDN;
      break;
   case supl_url:
      lcsFormatInd = PDSM_SUPL_FORMAT_URL;
      break;
   case supl_sipUrl:
      lcsFormatInd = PDSM_SUPL_FORMAT_SIP_URL;
      break;
   case supl_min:
      lcsFormatInd = PDSM_SUPL_FORMAT_MIN;
      break;
   case supl_mdn:
      lcsFormatInd = PDSM_SUPL_FORMAT_MDN;
      break;
   case supl_iMSPublicidentity:
      lcsFormatInd = PDSM_SUPL_FORMAT_IMSPUBLIC_IDENTITY;
      break;
   default:
      lcsFormatInd = PDSM_SUPL_FORMAT_OSS_UNKNOWN;
      break;
   }

   return lcsFormatInd;
}



/********************************************************************
*
* supl_action_upon_ni_notif_verif_accepted
*
* Function description:
*   actions to be taken once an NI request SUPL_INIT is accepted
*   by the OEM layer.
*
* Parameters:
*   p_req_data : pointer to a structure holding information from
*   pdapi that is the part of SUPL_INIT needed in the response to
*   be sent to the SLP.
*
* Return:
*  None
*
*********************************************************************
*/

static void supl_action_upon_ni_notif_verif_accepted
( pdsm_lcs_supl_notify_verify_req_s_type *p_req_data,
  supl_slp_session_id                    *p_slp_id
)
{

  supl_status_s_type*   p_supl_session_status = &z_supl_session_status;
  uint8                 u_hor_acc_threshold = 0;
  tm_cm_phone_state_info_s_type phone_state_info;

  supl_init_trigger_type_enum_type e_trigger_type;

  #if 0  /* Use these if direct call to start session if the concept of
            "network-owned" session is to be implemented */
  tm_sess_req_param_u_type z_tm_core_req_param;
  tm_sess_req_param_u_type *p_tm_core_req_param = &z_tm_core_req_param;
  #endif

  if (p_req_data ==  NULL || p_slp_id == NULL)
  {
    MSG_ERROR("SUPL: attempt to dereference NULL pointer", 0, 0, 0);
    return;
  }

  /* Query TM-CM module to get current acquired system */
  tm_cm_iface_get_phone_state(&phone_state_info);

  /* Handle the case where the request is for "no position", for which all the network needs
     is an accept or reject and no further action is needed */
  if ( p_req_data->pos_method == POSMETHOD_NO_POSITION )
  {
    if ( ( p_req_data->notification_priv_type == PDSM_LCS_USER_NOTIFY_ONLY ) ||
         ( p_req_data->notification_priv_type == PDSM_LCS_NO_NOTIFY_NO_VERIFY )
       )
    {
      /* SUPL_END with only VER in it */
      (void) tm_umts_up_supl_supl_end_prepare_or_tx( p_slp_id,
                                              NULL,
                                              0xFFFFFFFF,
                                              p_req_data->supl_hash,
                                              q_supl_ver_maj_after_supl_init_glob );

      q_supl_ver_maj_after_supl_init_glob = 0;

    }
    else
    {
      (void) tm_umts_up_supl_supl_end_prepare_or_tx( p_slp_id,
                                              NULL,
                                              supl_StatusCode_consentGrantedByUser,
                                              p_req_data->supl_hash,
                                              q_supl_ver_maj_after_supl_init_glob );
      q_supl_ver_maj_after_supl_init_glob = 0;

    }
    return;
  }

  /*********** preempt any existing SUPL session *********************/
  if ( p_supl_session_status->u_callflow_kind != C_SUPL_SESSION_NONE )
  {
    MSG_HIGH("SUPL: preempting existing session", 0, 0, 0);
    supl_preempt_ongoing_session(supl_StatusCode_resourceShortage);
  }

    /*Select the positioning protocol to use*/
  if ( (p_supl_session_status->u_position_protocol =
        tm_umts_up_supl_select_pos_prot()) == TM_POS_PRTL_NONE)
  {
    MSG_ERROR("SUPL: Position Protocol couldnt be determined", 0, 0, 0);
    (void) tm_umts_up_supl_supl_end_prepare_or_tx(p_slp_id,
                                                  NULL,
                                                  supl_StatusCode_unspecified,
                                                  NULL,
                                                  p_supl_session_status->q_supl_version);

    return;
  }
  /* Checking to see if this is area event trigger */
  e_trigger_type = supl_init_trigger_event_check();

  if ( e_trigger_type != C_SUPL_INIT_TRIG_TYPE_NULL )
  {
    if ( e_trigger_type == C_SUPL_INIT_TRIG_TYPE_AREA_EVENT )
    {
      supl_init_trigger_event_proc(p_req_data, p_slp_id, phone_state_info);
      q_supl_ver_maj_after_supl_init_glob = 0;

    }
    else
    {
      /* should never happen */
      MSG_ERROR ("trigger types not processed here: %u", e_trigger_type, 0, 0);
    }

    return;
  } /* for Triggered sessions, this function ends here */

 /*********** copy SUPL hash *********************/
  memscpy( p_supl_session_status->supl_init_hash, sizeof(p_supl_session_status->supl_init_hash),
           p_req_data->supl_hash, sizeof(p_req_data->supl_hash) );
  p_supl_session_status->u_hash_valid = TRUE;

  /************** store SLP session ID ************************************/
  p_supl_session_status->z_session_id.z_Slp_Sess_Id = *p_slp_id;
  p_supl_session_status->ni_pos_method = p_req_data->pos_method;

  p_supl_session_status->q_supl_version = q_supl_ver_maj_after_supl_init_glob;
  q_supl_ver_maj_after_supl_init_glob = 0;

  #ifdef FEATURE_USER_PLANE_MRL
  #ifdef FEATURE_CGPS_WCDMA
  if ( phone_state_info.srv_system == CGPS_SRV_SYS_WCDMA )
  {
    tm_umts_up_supl_send_wcdma_cell_meas_req();
    (void) os_TimerStart(l1_mrl_req_timer, 1000, NULL);
  }
  #endif /* FEATURE_CGPS_WCDMA */

  #ifdef FEATURE_CGPS_LTE_CELLDB
  if ( phone_state_info.srv_system == CGPS_SRV_SYS_LTE )
  {
    lte_ml1_request_gps_mrl();
    (void) os_TimerStart(l1_mrl_req_timer, 1000, NULL);
  }
  #endif

  #endif /* MRL */


  /* if this was an emergency session, copy over the SLP address if given by the server */
  if ( p_req_data->flags & PDSM_LCS_SUPLINIT_VER2_EXT_EMERGENCY_PRESENT)
  {
    p_supl_session_status->z_emergency_data.b_valid = TRUE;
    p_supl_session_status->z_emergency_data.b_emergency_ni = TRUE;
    if ( p_req_data->supl_init_ver2_ext_emergency.b_eslp_address_present == TRUE )
    {

      p_supl_session_status->z_emergency_data.b_eslp_present = TRUE;
      memscpy(&p_supl_session_status->z_emergency_data.z_eslp_address,
              sizeof(p_supl_session_status->z_emergency_data.z_eslp_address),
        &p_req_data->supl_init_ver2_ext_emergency.e_slp_address,
              sizeof(p_req_data->supl_init_ver2_ext_emergency.e_slp_address));
      MSG_MED("TM_UMTS_UP: SUPL ESLP present",0,0,0);
    }
  }
  else
  {
    p_supl_session_status->z_emergency_data.b_valid = FALSE;
    p_supl_session_status->z_emergency_data.b_emergency_ni = FALSE;
    p_supl_session_status->z_emergency_data.b_eslp_present = FALSE;
  }

  if(!(p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLEND) )
  {
    if( (z_pdcomm_tcp_info.emergency_connection == TRUE) ||
        (supl_session_is_emergency_session() == TRUE)
      )
    {
      /*Previous connection can not be reused*/
      tm_umts_up_supl_comm_close_connection();
    }
  }

  /******** set agps_mode ******************************************/
  if ( ( p_req_data->pos_method == POSMETHOD_AGPS_SETBASED ) ||
       ( p_req_data->pos_method == POSMETHOD_AGPS_SETBASED_PREF )
    )
  {

      /*NI MS BASED only if MS-BASED supported and RRLP is used as pos protocol*/
    if ( (TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_UP_MS_BASED)) &&
          ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_RRLP)
        )
    {
      p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_BASED; /* RRLP may alter */
      p_supl_session_status->tm_active_session_handle = TM_SESS_HANDLE_UMTS_UP_NI;

      /* horz. unc. 0 to prevent LM turning off MGP */
      u_hor_acc_threshold = 0;

      tm_umts_up_supl_callflow_kind_set ( C_SUPL_SESSION_NI );

      /* this will ask pdapi to generate pdsm_get_pos() to TmCore */
      if ( ! tm_umts_common_ni_client_start_session (TM_SESS_OPERATION_MODE_MSB,
                                                     u_hor_acc_threshold,
                                                     TM_MAX_SESSION_TIMEOUT, 1,0, MT_UP, FALSE)
         )
      {
        /* this attempt to start a new SUPL session fails, clear session status */
        memset ( &z_supl_session_status, 0, sizeof(supl_status_s_type) );
        return;
      }

    } /* MSB allowed and RRLP is pos protocol*/

    else if ( (TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_UP_MS_ASSISTED))  &&
             (p_supl_session_status->u_position_protocol != TM_POS_PRTL_LPP))
    {
       /* MSB not allowed but MSA is allowed or IS801 is pos protocol, start with MSA and will let SLP
          know that in SUPL_POS_INIT */
         p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_ASSISTED; /* RRLP may alter */
         p_supl_session_status->tm_active_session_handle = TM_SESS_HANDLE_UMTS_UP_NI;

         tm_umts_up_supl_callflow_kind_set ( C_SUPL_SESSION_NI );

         /* this will ask pdapi to generate pdsm_get_pos() to TmCore. Note accuracy_threshold
             does not matter for MSA, arbitary value of 100 used. */
        if ( ! tm_umts_common_ni_client_start_session(TM_SESS_OPERATION_MODE_MSA, 100,
                                                      TM_MAX_SESSION_TIMEOUT, 1,0,MT_UP, FALSE)
           )
        {
          /* this attempt to start a new SUPL session fails, clear session status */
          memset ( &z_supl_session_status, 0, sizeof(supl_status_s_type) );
          return;
        }

    } /* MSB not allowed but MSA is */
    else if  ( p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP )
    {
      if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_4G_UP_MS_BASED) )
      {

        p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_BASED; /* RRLP may alter */
        p_supl_session_status->tm_active_session_handle = TM_SESS_HANDLE_UMTS_UP_NI;

        /* horz. unc. 0 to prevent LM turning off MGP */
        u_hor_acc_threshold = 0;

        tm_umts_up_supl_callflow_kind_set ( C_SUPL_SESSION_NI );

        /* this will ask pdapi to generate pdsm_get_pos() to TmCore */
        if ( ! tm_umts_common_ni_client_start_session (TM_SESS_OPERATION_MODE_MSB, u_hor_acc_threshold,
                                                       TM_MAX_SESSION_TIMEOUT, 1,0,MT_UP, FALSE)
           )
        {
          /* this attempt to start a new SUPL session fails, clear session status */
          memset ( &z_supl_session_status, 0, sizeof(supl_status_s_type) );
          return;
        }
      }

      else if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_4G_UP_MS_ASSISTED) )
      {
       /* MSB not allowed but MSA is allowed or IS801 is pos protocol, start with MSA and will let SLP
          know that in SUPL_POS_INIT */
         p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_ASSISTED; /* RRLP may alter */
         p_supl_session_status->tm_active_session_handle = TM_SESS_HANDLE_UMTS_UP_NI;

         tm_umts_up_supl_callflow_kind_set ( C_SUPL_SESSION_NI );

         /* this will ask pdapi to generate pdsm_get_pos() to TmCore. Note accuracy_threshold
             does not matter for MSA, arbitary value of 100 used. */
        if ( ! tm_umts_common_ni_client_start_session(TM_SESS_OPERATION_MODE_MSA, 100,
                                                      TM_MAX_SESSION_TIMEOUT, 1,0,MT_UP, FALSE)
           )
        {
          /* this attempt to start a new SUPL session fails, clear session status */
          memset ( &z_supl_session_status, 0, sizeof(supl_status_s_type) );
          return;
        }

      } /* MSB not allowed but MSA is */
    } /* MSB allowed and LPP is pos protocol*/

  }  /* SUPL_INIT asking for MSB or MSB_pref */

  else if ( ( p_req_data->pos_method == POSMETHOD_AGPS_SETASSISTED ) ||
            ( p_req_data->pos_method == POSMETHOD_AGPS_SETASSISTED_PREF ) ||
            ( p_req_data->pos_method == POSMETHOD_AFLT ) ||
            ( p_req_data->pos_method == POSMETHOD_ECID ) ||
            ( p_req_data->pos_method == POSMETHOD_EOTD ) ||
            ( p_req_data->pos_method == POSMETHOD_OTDOA )
         )
  {

    if (p_supl_session_status->u_position_protocol != TM_POS_PRTL_LPP)
    {
      if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_UP_MS_ASSISTED) )
      {

        byte gps_session_timeout = TM_MAX_SESSION_TIMEOUT;
        tm_umts_up_supl_callflow_kind_set ( C_SUPL_SESSION_NI );
        p_supl_session_status->tm_active_session_handle = TM_SESS_HANDLE_UMTS_UP_NI;


        if( ( p_req_data->pos_method == POSMETHOD_AGPS_SETASSISTED ) ||
            ( p_req_data->pos_method == POSMETHOD_AGPS_SETASSISTED_PREF )
          )
        {
          if( (p_supl_session_status->u_position_protocol == TM_POS_PRTL_RRLP) ||
              (p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801)
            )
          {
            /*For both IS801 and RRLP, selected Assisted*/
            p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_ASSISTED;
          }
        } /*SETASSISTED or SETASSISTED_PREF*/

        else if(p_req_data->pos_method == POSMETHOD_AFLT ||
                p_req_data->pos_method == POSMETHOD_ECID ||
                p_req_data->pos_method == POSMETHOD_OTDOA) /*AFLT/ECID/OTDOA*/
        {
          if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_RRLP)
          {
              /* even for the AFLT, ECID, OTDOA will start as MSA first, the SLP
              has the chance to change to a supported mode once it gets SUPL_POS_INIT */
             p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_ASSISTED; /* RRLP may alter */
          }
          else if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_IS801)
          {

            if(p_req_data->pos_method == POSMETHOD_AFLT)
            {
              /*If IS801 is pos protocol and pos method is AFLT then set session agps mode to UE_ASSISTED*/
              p_supl_session_status->u_agps_mode = C_SUPL_AFLT_MODE;
              /*For AFLT, gps_session_timeout should be zero*/
              gps_session_timeout = 0;
            }
            else
            {
              /*ECID case*/
              p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_CELL_ID;
            }
          }
          else if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP)
          {
              /* even for the AFLT, ECID, will start as MSA first, the SLP
              has the chance to change to a supported mode once it gets SUPL_POS_INIT */
             p_supl_session_status->u_agps_mode = C_SUPL_OTDOA_MODE; /* RRLP may alter */
              /*For AFLT, gps_session_timeout should be zero*/
              gps_session_timeout = 0;

          }
          else
          {
            MSG_ERROR("Pos Protocol not supported",0,0,0);
          }
        }/*ECID/AFLT*/
        else /*all other*/
        {
          if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_RRLP ||
             p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP)
          {
              /* For all unsupported, will start as MSA first, the SLP
              has the chance to change to a supported mode once it gets SUPL_POS_INIT */
             p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_ASSISTED; /* RRLP may alter */
          }
        }


        if(p_supl_session_status->u_agps_mode != C_SUPL_AGPS_MODE_UNDEF)
        {
          /* this will ask pdapi to generate pdsm_get_pos() to TmCore. Note accuracy_threshold
             does not matter for MSA, arbitary value of 100 used. */
          if ( ! tm_umts_common_ni_client_start_session(TM_SESS_OPERATION_MODE_MSA, 100,
                                                        gps_session_timeout, 1,0,MT_UP, FALSE)
             )
          {
            /* this attempt to start a new SUPL session fails, clear session status */
            memset ( &z_supl_session_status, 0, sizeof(supl_status_s_type) );
            return;
          }
        }
      }
      /*For RRLP if MSA is not allowed and MSB is allowed, MSB is started.*/
      else if ( TM_CONFIG_POS_MODE_BIT_IS_SET( PDSM_POS_MODE_UP_MS_BASED | PDSM_POS_MODE_4G_UP_MS_BASED ) != 0 )
      {
        if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_RRLP ||
           p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP)
        {
           /* MSA not allowed but MSB is. Start with MSB and see if SLP changes its
           mind  */
          p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_BASED;
          p_supl_session_status->tm_active_session_handle = TM_SESS_HANDLE_UMTS_UP_NI;

          tm_umts_up_supl_callflow_kind_set ( C_SUPL_SESSION_NI );

          /* this will ask pdapi to generate pdsm_get_pos() to TmCore */
          /* horz. unc. 0 to prevent LM turning off MGP */
          if ( ! tm_umts_common_ni_client_start_session (TM_SESS_OPERATION_MODE_MSB, 0,
                                                         TM_MAX_SESSION_TIMEOUT, 1,0,MT_UP, FALSE)
             )
          {
            /* this attempt to start a new SUPL session fails, clear session status */
            memset ( &z_supl_session_status, 0, sizeof(supl_status_s_type) );
            return;
          }
        }

      }  /* MSA not allowed but MSB allowed */
    }

    if (p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP)
    {
      if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_4G_UP_MS_ASSISTED) )
      {

        byte gps_session_timeout = TM_MAX_SESSION_TIMEOUT;
        tm_umts_up_supl_callflow_kind_set ( C_SUPL_SESSION_NI );
        p_supl_session_status->tm_active_session_handle = TM_SESS_HANDLE_UMTS_UP_NI;


        if( ( p_req_data->pos_method == POSMETHOD_AGPS_SETASSISTED ) ||
            ( p_req_data->pos_method == POSMETHOD_AGPS_SETASSISTED_PREF )
          )
        {
          /*For both IS801 and RRLP, selected Assisted*/
          p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_ASSISTED;
        } /*SETASSISTED or SETASSISTED_PREF*/

        else if(p_req_data->pos_method == POSMETHOD_AFLT ||
                p_req_data->pos_method == POSMETHOD_OTDOA ||
                p_req_data->pos_method == POSMETHOD_ECID) /*AFLT/ECID*/
        {
          if (p_req_data->pos_method != POSMETHOD_ECID  )
          {
            p_supl_session_status->u_agps_mode = C_SUPL_OTDOA_MODE;
                /*For AFLT, gps_session_timeout should be zero*/
            gps_session_timeout = 0;
          }
          else
          {
            /*ECID case*/
            p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_CELL_ID;
          }
        }/*ECID/AFLT*/
        else /*all other*/
        {
          if (p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP)
          {
              /* For all unsupported, will start as MSA first, the SLP
              has the chance to change to a supported mode once it gets SUPL_POS_INIT */
             p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_ASSISTED; /* RRLP may alter */
          }
        }

        if(p_supl_session_status->u_agps_mode == C_SUPL_OTDOA_MODE)
        {
          /* this will ask pdapi to generate pdsm_get_pos() to TmCore. Note accuracy_threshold
             does not matter for MSA, arbitary value of 100 used. */
          if ( ! tm_umts_common_ni_client_start_session(TM_SESS_OPERATION_MODE_OTDOA, 100,
                                                        gps_session_timeout, 1,0, MT_UP, FALSE) )
          {
            /* this attempt to start a new SUPL session fails, clear session status */
            memset ( &z_supl_session_status, 0, sizeof(supl_status_s_type) );
            return;
          }
        }
        else if(p_supl_session_status->u_agps_mode != C_SUPL_AGPS_MODE_UNDEF)
        {
          /* this will ask pdapi to generate pdsm_get_pos() to TmCore. Note accuracy_threshold
             does not matter for MSA, arbitary value of 100 used. */
          if ( ! tm_umts_common_ni_client_start_session(TM_SESS_OPERATION_MODE_MSA, 100,
                                                        gps_session_timeout, 1,0,MT_UP, FALSE)
             )
          {
            /* this attempt to start a new SUPL session fails, clear session status */
            memset ( &z_supl_session_status, 0, sizeof(supl_status_s_type) );
            return;
          }
        }
      }
      /*For RRLP if MSA is not allowed and MSB is allowed, MSB is started.*/
      else if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_4G_UP_MS_BASED) )
      {
        if(p_supl_session_status->u_position_protocol == TM_POS_PRTL_LPP)
        {
          /* MSA not allowed but MSB is. Start with MSB and see if SLP changes its
             mind  */
          p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_BASED;
          p_supl_session_status->tm_active_session_handle = TM_SESS_HANDLE_UMTS_UP_NI;

          tm_umts_up_supl_callflow_kind_set ( C_SUPL_SESSION_NI );

          /* this will ask pdapi to generate pdsm_get_pos() to TmCore */
          /* horz. unc. 0 to prevent LM turning off MGP */
          if ( ! tm_umts_common_ni_client_start_session (TM_SESS_OPERATION_MODE_MSB, 0,
                                                         TM_MAX_SESSION_TIMEOUT, 1,0, MT_UP, FALSE)
             )
          {
            /* this attempt to start a new SUPL session fails, clear session status */
            memset ( &z_supl_session_status, 0, sizeof(supl_status_s_type) );
            return;
          }
        }

      } /* MSA not allowed but MSB allowed */
    } /* lpp*/

  } /* close if-assisted */


  else if ( p_req_data->pos_method == POSMETHOD_AUTONOMOUS_GPS )
  {
    MSG_MED("SUPL: accepting NI autonomous", 0, 0, 0);

    /* even for autonomous, with NI it will involve RRLP msrPosReq
       so it needs MSB to be supported; or MSA so that SLP will
       change its mind to ask for MSA; SET will inform SLP if stanalone
       is supported in SUPL_POS_INIT capability IE */

    /*For IS801 if pos method is autonomous, then MSA will be started*/

    if ( p_supl_session_status->u_position_protocol != TM_POS_PRTL_LPP )
    {
      if (  (TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_UP_MS_BASED)) &&
            (p_supl_session_status->u_position_protocol == TM_POS_PRTL_RRLP)
         )
      {
        p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_BASED;
        p_supl_session_status->tm_active_session_handle = TM_SESS_HANDLE_UMTS_UP_NI;

        /* horz. unc. 0 to prevent LM turning off MGP */
        u_hor_acc_threshold = 0;

        tm_umts_up_supl_callflow_kind_set ( C_SUPL_SESSION_NI );

        if ( ! tm_umts_common_ni_client_start_session (TM_SESS_OPERATION_MODE_MSB, u_hor_acc_threshold,
                                                       TM_MAX_SESSION_TIMEOUT, 1,0,MT_UP, FALSE)
           )
        {
          /* this attempt to start a new SUPL session fails, clear session status */
          memset ( &z_supl_session_status, 0, sizeof(supl_status_s_type) );
          return;
        }

      } /* if MSB provisioned */

      else if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_UP_MS_ASSISTED) )
      {

        /* MSB not allowed but MSA is. Start with MSA and if if SLP changes to MSA later */
        p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_ASSISTED;
        p_supl_session_status->tm_active_session_handle = TM_SESS_HANDLE_UMTS_UP_NI;

        tm_umts_up_supl_callflow_kind_set ( C_SUPL_SESSION_NI );

        /* this will ask pdapi to generate pdsm_get_pos() to TmCore. Note accuracy_threshold
           does not matter for MSA, arbitary value of 100 used. */
        if ( ! tm_umts_common_ni_client_start_session(TM_SESS_OPERATION_MODE_MSA, 100,
                                                      TM_MAX_SESSION_TIMEOUT, 1,0,MT_UP, FALSE)
           )
        {
          /* this attempt to start a new SUPL session fails, clear session status */
          memset ( &z_supl_session_status, 0, sizeof(supl_status_s_type) );
          return;
        }
      } /* MSB not allowed but MSA is */
    }
    else
    {
      /* LPP */
      if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_4G_UP_MS_BASED) )
      {
        p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_BASED;
        p_supl_session_status->tm_active_session_handle = TM_SESS_HANDLE_UMTS_UP_NI;

        /* horz. unc. 0 to prevent LM turning off MGP */
        u_hor_acc_threshold = 0;

        tm_umts_up_supl_callflow_kind_set ( C_SUPL_SESSION_NI );

        if ( ! tm_umts_common_ni_client_start_session (TM_SESS_OPERATION_MODE_MSB, u_hor_acc_threshold,
                                                       TM_MAX_SESSION_TIMEOUT, 1,0,MT_UP, FALSE)
           )
        {
          /* this attempt to start a new SUPL session fails, clear session status */
          memset ( &z_supl_session_status, 0, sizeof(supl_status_s_type) );
          return;
        }


      } /* if MSB provisioned */

      else if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_4G_UP_MS_ASSISTED) )
      {

        /* MSB not allowed but MSA is. Start with MSA and if if SLP changes to MSA later */
        p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_ASSISTED;
        p_supl_session_status->tm_active_session_handle = TM_SESS_HANDLE_UMTS_UP_NI;

        tm_umts_up_supl_callflow_kind_set ( C_SUPL_SESSION_NI );

        /* this will ask pdapi to generate pdsm_get_pos() to TmCore. Note accuracy_threshold
           does not matter for MSA, arbitary value of 100 used. */
        if ( ! tm_umts_common_ni_client_start_session(TM_SESS_OPERATION_MODE_MSA, 100,
                                                      TM_MAX_SESSION_TIMEOUT, 1,0,MT_UP, FALSE)
           )
        {
          /* this attempt to start a new SUPL session fails, clear session status */
          memset ( &z_supl_session_status, 0, sizeof(supl_status_s_type) );
          return;
        }
      } /* MSB not allowed but MSA is */
    }
  }
  else
  {
    /* no other mode is allowed, ignore the message if happens.  */
    MSG_HIGH("OEM returns unsupported pos_method: %u ", p_req_data->pos_method, 0, 0);
  }

  /* this should not happen */
  if ( p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UNDEF )
  {
      MSG_HIGH("agps cannot be established", 0, 0, 0);
      (void) tm_umts_up_supl_supl_end_prepare_or_tx( p_slp_id,
                                              NULL,
                                              supl_StatusCode_posMethodMismatch,
                                              p_req_data->supl_hash,
                                              p_supl_session_status->q_supl_version );
  }

}

/********************************************************************
*
* supl_send_pos_rrlp_to_slp
*
* Function description:
*   Send position in RRLP wrapped by SUPL_POS to SLP
*
* Parameters:
*   p_gad_shape_pos: pointer to the array holding the GAD shape;
*   p_velocity : pointer to a structure of the format for velocity
*                in SUPL POS
*   q_timestamp_ms : gps time of the position in msec.
*   q_pos_sources: GPS and/or GLONASS
*   q_glonass_tod_modulo_1hour_ms: GLONASS time stamp TimeOfDay
*
* Return:
*  None
*
*********************************************************************
*/

static void supl_send_pos_rrlp_to_slp
( gad_ellip_alt_unc_ellip_type *p_gad_shape_pos,
  supl_Velocity *p_velocity,
  uint32 q_timestamp_ms,
  uint32 q_pos_sources,
  uint32 q_glonass_tod_modulo_1hour_ms )
{
  supl_status_s_type*        p_supl_session_status = &z_supl_session_status;

  gad_velocity_hor_ver_s_type  z_gad_velocity_hor_ver;

  rrlp_PDU z_rrlp;
  rrlp_MsrPosition_Rsp z_rrlp_MsrPosition_Rsp;

  rrlp_MsrPosition_Rsp  *p_mpr;
  rrlp_LocationInfo     *p_locinfo;

  tm_pdu_buf_type   pdu_buff; /* including length and pointer to data */

  uint32 q_block_size;
  int encode_status;
  boolean ganss_pos_method = FALSE; /* TRUE if RRLP measurePosReq included GANSS posMethod in Rel7 extension */

  if ( p_gad_shape_pos == NULL )
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "ERROR: p_gad_shape_pos=%lx", p_gad_shape_pos);
    return;
  }

  /* initialize buffer for structure buffers used by OSYS ASN1 encoder */
  memset( &z_rrlp, 0, sizeof( z_rrlp ));
  memset( &z_rrlp_MsrPosition_Rsp, 0, sizeof(rrlp_MsrPosition_Rsp));
  memset(&pdu_buff, 0, sizeof(tm_pdu_buf_type));

  z_rrlp.component.u.msrPositionRsp = &z_rrlp_MsrPosition_Rsp;
  p_mpr  = z_rrlp.component.u.msrPositionRsp;


  /* ref. num: the same as the one came from SLP */
  z_rrlp.referenceNumber =
    p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.b_RefNum;

  z_rrlp.component.t = rrlp_msrPositionRsp_chosen;


  if (p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.z_rrlp_rel7_ext.rel7_ext_present == TRUE)
  {
    if ( (p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.z_rrlp_rel7_ext.u_ganss_pos_methods_bitmap &
          RRLP_GANSS_POS_METHOD_GPS ) ||
         (p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.z_rrlp_rel7_ext.u_ganss_pos_methods_bitmap &
          RRLP_GANSS_POS_METHOD_GLONASS )
       )
    {
      ganss_pos_method = TRUE;
    }
  }

  if (!ganss_pos_method) /* RRLP msrPosReq does not includes REL7-extension of GANSS_posMethod */
  {

    p_locinfo  = &p_mpr->locationInfo;

    /* it is garanteed by the definition of the structures that no capping
       is needed, but just in case */
    q_block_size = memscpy(&p_locinfo->posEstimate.data[0], sizeof (p_locinfo->posEstimate.data),
                           p_gad_shape_pos, sizeof(gad_ellip_alt_unc_ellip_type));
    p_locinfo->posEstimate.numocts = q_block_size;

    p_locinfo->refFrame = 65535; /* (42432..65535) will be ignored */
    #ifndef FEATURE_CGPS_SUPL_VARIANT_SUPL_POS_NO_TIMESTAMP /* NOT defined by default */
    p_locinfo->m.gpsTOWPresent = TRUE;
    p_locinfo->gpsTOW =  q_timestamp_ms % C_FOUR_HRS_MSEC;
    #endif
    p_locinfo->fixType = rrlp_threeDFix;

    p_mpr->m.locationInfoPresent = TRUE;

  }

  else /* RRLP msrPosReq includes REL-7 extension of GANSS_posMethod: only report pos in REL-7 extension */
  {

    p_mpr->m.rel_7_MsrPosition_Rsp_ExtensionPresent = TRUE;

    p_mpr->rel_7_MsrPosition_Rsp_Extension.m.ganssLocationInfoPresent = TRUE;

    /* .data[0]: GPS only: 0x40; GLONASS only: 0x02; if GPS+GLONASS: 0x42
       numbits:  GPS only: 2;    GLONASS only: 7;    GPS+GLONASS:    7
    **/
    if ( ( q_pos_sources & C_POS_SOURCE_GPS ) &&
         ( q_pos_sources & C_POS_SOURCE_GLO ) )
    {
      p_mpr->rel_7_MsrPosition_Rsp_Extension.ganssLocationInfo.posData.data[0] = 0x42;
      p_mpr->rel_7_MsrPosition_Rsp_Extension.ganssLocationInfo.posData.numbits = 7;
    }
    else if ( q_pos_sources & C_POS_SOURCE_GLO )
    {
      p_mpr->rel_7_MsrPosition_Rsp_Extension.ganssLocationInfo.posData.data[0] = 0x02;
      p_mpr->rel_7_MsrPosition_Rsp_Extension.ganssLocationInfo.posData.numbits = 7;
    }
    else if ( q_pos_sources & C_POS_SOURCE_GPS )
    {
      p_mpr->rel_7_MsrPosition_Rsp_Extension.ganssLocationInfo.posData.data[0] = 0x40;
      p_mpr->rel_7_MsrPosition_Rsp_Extension.ganssLocationInfo.posData.numbits = 2;
    }
    else /* default to GPS for now */
    {
      p_mpr->rel_7_MsrPosition_Rsp_Extension.ganssLocationInfo.posData.data[0] = 0x40;
      p_mpr->rel_7_MsrPosition_Rsp_Extension.ganssLocationInfo.posData.numbits = 2;
    }
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Pos Sources: %lu", q_pos_sources);

    p_mpr->rel_7_MsrPosition_Rsp_Extension.ganssLocationInfo.fixType = rrlp_threeDFix;

    /* it is garanteed by the definition of the structures that no capping
       is needed, but just in case */
    q_block_size = memscpy(&p_mpr->rel_7_MsrPosition_Rsp_Extension.ganssLocationInfo.posEstimate.data[0],
                           sizeof (p_mpr->rel_7_MsrPosition_Rsp_Extension.ganssLocationInfo.posEstimate.data),
                           p_gad_shape_pos, sizeof(gad_ellip_alt_unc_ellip_type));
    p_mpr->rel_7_MsrPosition_Rsp_Extension.ganssLocationInfo.posEstimate.numocts = q_block_size;

    if ( q_glonass_tod_modulo_1hour_ms <= 3599999 ) /* RRLP ASN1 limit*/
    {
      p_mpr->rel_7_MsrPosition_Rsp_Extension.ganssLocationInfo.m.ganssTODmPresent = 1;
      p_mpr->rel_7_MsrPosition_Rsp_Extension.ganssLocationInfo.ganssTODm = q_glonass_tod_modulo_1hour_ms;
    }
    else
    {
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "No GLO Timestamp: q_glonass_tod_modulo_1hour_ms=%lx",
            q_glonass_tod_modulo_1hour_ms);
    }

  }

  /* if REL-5 Extension has been included in the message sent from the network, include it
     in this uplink message too */
  if ( p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.z_rrlp_rel5_ext.rel5_ext_present )
  {
    p_mpr->m.rel_5_MsrPosition_Rsp_ExtensionPresent = TRUE;

    p_mpr->rel_5_MsrPosition_Rsp_Extension.m.extended_referencePresent = TRUE;

    p_mpr->rel_5_MsrPosition_Rsp_Extension.extended_reference.smlc_code =
      p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.z_rrlp_rel5_ext.rel5_ext_ref.smlc_code;

    p_mpr->rel_5_MsrPosition_Rsp_Extension.extended_reference.transaction_ID =
      p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.z_rrlp_rel5_ext.rel5_ext_ref.transaction_ID;

  }

  if ( p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.z_rrlp_rel7_ext.rel7_ext_present )
  {
    if ( ( p_supl_session_status->z_network_assist.radio_link_prot_params.z_rrlp_params.z_rrlp_rel7_ext.u_velocity_requested ) &&
         ( p_velocity != NULL )
       )
    {
      p_mpr->m.rel_7_MsrPosition_Rsp_ExtensionPresent = TRUE;
      p_mpr->rel_7_MsrPosition_Rsp_Extension.m.velEstimatePresent = TRUE;

      if ( p_velocity->t == C_SUPL_ASN1_CHOICE_VELCITY_HORANDVERVEL )
      {
        /* typedef rrlp_VelocityEstimate */
        z_gad_velocity_hor_ver.u_ShapeCode_verDir_MsbBearing = 0x10; /* hor. and ver. */

        if ( p_velocity->u.horandvervel->verdirect.data[0] & 0x1 )
        {
          z_gad_velocity_hor_ver.u_ShapeCode_verDir_MsbBearing |= 0x2; /* downward */
        }
        z_gad_velocity_hor_ver.u_hor_speed0 = (uint8)(p_velocity->u.horandvervel->horspeed.data[0]);
        z_gad_velocity_hor_ver.u_hor_speed1 = (uint8)(p_velocity->u.horandvervel->horspeed.data[1]);
        z_gad_velocity_hor_ver.u_ver_speed =  (uint8)(p_velocity->u.horandvervel->verspeed.data[0]);

        /* p_velocity->u.horandvervel.bearing.length = 9 */
        if ( p_velocity->u.horandvervel->bearing.data[0] & 0x80 )
        {
          z_gad_velocity_hor_ver.u_ShapeCode_verDir_MsbBearing |= 0x1;
        }
        z_gad_velocity_hor_ver.u_hor_bearing_Lsb8 = p_velocity->u.horandvervel->bearing.data[0];
        z_gad_velocity_hor_ver.u_hor_bearing_Lsb8 <<= 1;

        z_gad_velocity_hor_ver.u_hor_bearing_Lsb8 |= (p_velocity->u.horandvervel->bearing.data[1]) >> 7;

        /* it is garanteed by the definition of the structures that no capping
           is needed, but just in case */

        q_block_size = memscpy( &p_mpr->rel_7_MsrPosition_Rsp_Extension.velEstimate.data[0],
                          sizeof (p_mpr->rel_7_MsrPosition_Rsp_Extension.velEstimate.data),
                          &z_gad_velocity_hor_ver, sizeof( z_gad_velocity_hor_ver ));

        p_mpr->rel_7_MsrPosition_Rsp_Extension.velEstimate.numocts = q_block_size;

      }
      else
      {
        /* for now, other velocity shapes not reported */
        p_mpr->rel_7_MsrPosition_Rsp_Extension.m.velEstimatePresent = FALSE;

      }

      if ( ( p_mpr->rel_7_MsrPosition_Rsp_Extension.m.ganssLocationInfoPresent == FALSE ) &&
           ( p_mpr->rel_7_MsrPosition_Rsp_Extension.m.ganssMeasureInfoPresent == FALSE ) &&
           ( p_mpr->rel_7_MsrPosition_Rsp_Extension.m.velEstimatePresent == FALSE )
         )
      {
        /* if nothing is to be included in rel7 ext. don't include it */
        p_mpr->m.rel_7_MsrPosition_Rsp_ExtensionPresent = FALSE;
      }

    } /* RRLP velocity requested and velocity available */
  } /* Rel7 Extention */

  /* ASN1 encoding */
  encode_status = tm_osys_asn1_encode_pdu((void *)&z_rrlp, &pdu_buff, PROTO_RRLP);

  if ( (encode_status != 0) || (pdu_buff.value == NULL ) || (pdu_buff.length == 0) )
  {
    /* ASN.1 encoding failed */
    if (pdu_buff.value != NULL )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRLP ASN1 enc. err. Freeing: %d,%lx,%u",
            encode_status, pdu_buff.value, pdu_buff.length);

      tm_asn1_free_osys_buf( pdu_buff.value, PROTO_RRLP );
      pdu_buff.value = NULL;

    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRLP ASN1 enc. err. No alloc.: %d,%lx,%u",
            encode_status, pdu_buff.value, pdu_buff.length);
    }

    /* QQQ: return to SLP rrlp-msrPosResp-err */
    return;
  }

  if ( !supl_SuplPos_rrlp_tx (pdu_buff.value, (uint16)pdu_buff.length, p_velocity) )
  {
    tm_umts_up_supl_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
    tm_umts_up_supl_session_init(FALSE);
  }
  /* else, if successful, entering UT3 waiting for SUPL_END */

  /* Free the PDU */
  tm_asn1_free_osys_buf( pdu_buff.value, PROTO_RRLP );
  pdu_buff.value = NULL;

}



/*
 ******************************************************************************
 *
 * supl_preempt_ongoing_session
 *
 * Function description:
 *  This function preemts the ongoing SUPL session, as a new session has been
 *  accepted.
 *
 *
 * Parameters:
 *   None.
 *
 * Return value:
 *   None.
 *
 ******************************************************************************
*/

static void supl_preempt_ongoing_session(supl_StatusCode e_cancel_reason )
{
  supl_status_s_type*    p_supl_session_status = &z_supl_session_status;

  if ( p_supl_session_status->u_network_session_on == TRUE )
  {
    (void) tm_umts_up_supl_supl_end_prepare_or_tx ( &p_supl_session_status->z_session_id.z_Slp_Sess_Id,
                      NULL,
                      e_cancel_reason,
                      NULL,
                      p_supl_session_status->q_supl_version);
  }

  /* inform TmCore the current session has been preempted */
  tm_umts_up_supl_inform_core_stop_session( TM_STOP_REASON_USER_TERMINATED);

  tm_umts_up_supl_session_init(TRUE);

}

/*
 ******************************************************************************
 *
 * supl_gad9_to_pos_est
 *
 * Function description:
 *  This function converts a GAD Shape of Ellipsoid Point with altitude and
 *  uncertainty ellipsoid into supl_PositionEstimate.
 *  accepted.
 *
 *
 * Parameters:
 *   None.
 *
 * Return value:
 *   None.
 *
 ******************************************************************************
*/

static void supl_gad9_to_pos_est
( const gad_ellip_alt_unc_ellip_type *p_zEllipAltUncEllip,
  supl_PositionEstimate *p_positionInfo
)

{

  p_positionInfo->m.altitudeInfoPresent = 1;
  p_positionInfo->m.confidencePresent = 1;
  p_positionInfo->m.uncertaintyPresent = 1;

  /***************************** latitude *****************************/

  /** handle sign bit */

  if ( p_zEllipAltUncEllip->u_Lat0  & 0x80 )
    p_positionInfo->latitudeSign = supl_south;
  else
    p_positionInfo->latitudeSign = supl_north;

  p_positionInfo->latitude =
    ( (uint32)p_zEllipAltUncEllip->u_Lat0 & 0x0000007F ) << 16;
  p_positionInfo->latitude |=
    (uint32)p_zEllipAltUncEllip->u_Lat1  << 8;
  p_positionInfo->latitude |=
    (uint32)p_zEllipAltUncEllip->u_Lat2;

  /***************************** longitude *****************************/

  p_positionInfo->longitude =
    (uint32)p_zEllipAltUncEllip->u_Long0 << 16;

  p_positionInfo->longitude |=
    (uint32)p_zEllipAltUncEllip->u_Long1 << 8;

  p_positionInfo->longitude |=
    (uint32)p_zEllipAltUncEllip->u_Long2;

    /*** sign extend it **/
  if ( p_positionInfo->longitude & 0x00800000)
    p_positionInfo->longitude |= 0xFF800000;

    /* unc. major */
  p_positionInfo->uncertainty.uncertaintySemiMajor =
    p_zEllipAltUncEllip->u_UncMajor;

    /* unc. minor */
  p_positionInfo->uncertainty.uncertaintySemiMinor =
    p_zEllipAltUncEllip->u_UncMinor;

    /* unc. major axis orientation */
  p_positionInfo->uncertainty.orientationMajorAxis =
    p_zEllipAltUncEllip->u_OrientMajor;

    /****************** altitude ***********************************************/
  p_positionInfo->m.altitudeInfoPresent = 1;

  if ( p_zEllipAltUncEllip->u_Alt0 & 0x80 )
    p_positionInfo->altitudeInfo.altitudeDirection = supl_depth;
  else
    p_positionInfo->altitudeInfo.altitudeDirection = supl_height;

  p_positionInfo->altitudeInfo.altitude =
  ( (uint32)p_zEllipAltUncEllip->u_Alt0 & 0x0000007F ) << 8;

  p_positionInfo->altitudeInfo.altitude |=
  (uint32)p_zEllipAltUncEllip->u_Alt1;

  p_positionInfo->altitudeInfo.altUncertainty =
    (uint32)p_zEllipAltUncEllip->u_UncAlt;

  /********************** confidence ****************************************/
  p_positionInfo->confidence =
    (uint32)p_zEllipAltUncEllip->u_Confidence;

}

/*
 ******************************************************************************
 *
 * supl_convert_slp_id_from_notif_verif_resp
 *
 * Function description:
 *  This function converts SLP ID passed back to TM from Notif-Verif response
 *  into the format that can be used in SUPL messages.
 *
 *
 * Parameters:
 *   None.
 *
 * Return value:
 *   None.
 *
 ******************************************************************************
*/

void supl_convert_slp_id_from_notif_verif_resp
( pdsm_lcs_supl_notify_verify_req_s_type   *p_req_data,
  supl_slp_session_id                      *p_slp_id
)
{

  if ( p_req_data->supl_slp_session_id.presence )
  {

    p_slp_id->presence = TRUE;

    memscpy( &p_slp_id->session_Id[0], sizeof(p_slp_id->session_Id),
            &p_req_data->supl_slp_session_id.session_id[0],
            sizeof(p_req_data->supl_slp_session_id.session_id)
          );

    if ( p_req_data->supl_slp_session_id.slp_address.choice == PDSM_SERVER_TYPE_IPV4_ADDRESS )
    {
      p_slp_id->zSlpAddress.choice =
        C_SUPL_ASN1_CHOICE_SLPADDRESS_IPADDRESS;

      p_slp_id->zSlpAddress.u.z_Ip_Address.choice =
        C_SUPL_ASN1_CHOICE_IPV4ADDRESS;

      p_slp_id->zSlpAddress.u.z_Ip_Address.u.Ip_V4.numocts = C_SUPL_IPADDRESS_IPV4_BYTE_LENGTH;

      memscpy( &p_slp_id->zSlpAddress.u.z_Ip_Address.u.Ip_V4.data[0],
               sizeof(p_slp_id->zSlpAddress.u.z_Ip_Address.u.Ip_V4.data),
              &p_req_data->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.server_adrs,
              sizeof(p_req_data->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.server_adrs)
            );

    } /* SLP address being IP address, V4 */

    else if ( p_req_data->supl_slp_session_id.slp_address.choice == PDSM_SERVER_TYPE_IPV6_ADDRESS )
    {
      p_slp_id->zSlpAddress.choice =
        C_SUPL_ASN1_CHOICE_SLPADDRESS_IPADDRESS;

      p_slp_id->zSlpAddress.u.z_Ip_Address.choice =
        C_SUPL_ASN1_CHOICE_IPV6ADDRESS;

      p_slp_id->zSlpAddress.u.z_Ip_Address.u.Ip_V6.numocts = C_SUPL_IPADDRESS_IPV6_BYTE_LENGTH;


      memscpy( &p_slp_id->zSlpAddress.u.z_Ip_Address.u.Ip_V6.data[0],
               sizeof(p_slp_id->zSlpAddress.u.z_Ip_Address.u.Ip_V6.data),
              &p_req_data->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.server_adrs[0],
              sizeof(p_req_data->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.server_adrs)
            );

    } /* SLP address being IP address, V6 */

    else if ( p_req_data->supl_slp_session_id.slp_address.choice == PDSM_SERVER_TYPE_URL_ADDRESS )
    {
      p_slp_id->zSlpAddress.choice = C_SUPL_ASN1_CHOICE_SLPADRESS_FQDN;

      p_slp_id->zSlpAddress.u.z_fqdn.u_length =
        p_req_data->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url_length;

      memscpy( &p_slp_id->zSlpAddress.u.z_fqdn.fqdn[0],
               sizeof(p_slp_id->zSlpAddress.u.z_fqdn.fqdn),
              &p_req_data->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url[0],
              p_req_data->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url_length
            );
    }

  } /* if SLP ID present */

}

/*
 ******************************************************************************
 *
 * supl_convert_velocity
 *
 * Function description:
 *  This function converts velocity information into the velocity format
 *  of SUPL POS. The function currently only handles velocity type of
 *  horizontal_and_vertical.
 *
 *
 * Parameters:
 *   f_vel_east, f_vel_north, f_vel_vertical : float point velocity info.
 *   *p_velocity : pointer to the structure for SUPL POS velocity; note its
 *                 member u.horandvervel must be allocated by the calling
 *                 function.
 *
 * Return value:
 *   None.
 *
 ******************************************************************************
*/
static void supl_convert_velocity
(float f_vel_east,
 float f_vel_north,
 float f_vel_vertical,
 supl_Velocity *p_velocity
 )

{
  float f_heading_ang;
  uint16 w_hor_velocity;
  int16  s_bearing;
  int8   b_vertical_velocity;

  uint8            u_bearing_bit_string[2];
  uint8            u_hor_speed_bit_string[2];
  uint8            u_ver_direct_bit_string;
  uint8            u_ver_direct;
  uint8            u_ver_velocity_bit_string;

  float f_temp = 0;

  if ((p_velocity == NULL) || (p_velocity->u.horandvervel == NULL))
  {
    MSG_ERROR("Mem not allocated for vel.", 0, 0, 0);
    return;
  }


  tm_umts_common_convert_velocity(&f_heading_ang,
                                  &w_hor_velocity,
                                  &s_bearing,
                                  &b_vertical_velocity,
                                  &u_ver_direct,
                                  &u_ver_velocity_bit_string,
                                  &f_vel_east,
                                  &f_vel_north,
                                  &f_vel_vertical);

  p_velocity->t = C_SUPL_ASN1_CHOICE_VELCITY_HORANDVERVEL;

  /* vertical velocity direction */
  if (f_vel_vertical < 0.0f)
  {
    /* downward direction */
    u_ver_direct = SUPL_VELOCITY_DOWNWARD;
  }
  else
  { 
    /* upward direction */
    u_ver_direct = SUPL_VELOCITY_UPWARD ; 
  }
  /*   hor_speed        BIT STRING(SIZE (16)) */
  p_velocity->u.horandvervel->horspeed.numbits = 16;
  u_hor_speed_bit_string[0] = (w_hor_velocity >> 0x08) & 0xFF;
  /* high byte */
  u_hor_speed_bit_string[1] = w_hor_velocity & 0xFF;
  /* low byte */
  p_velocity->u.horandvervel->horspeed.data[0] = u_hor_speed_bit_string[0];
  p_velocity->u.horandvervel->horspeed.data[1] = u_hor_speed_bit_string[1];


  /*   bearing BIT STRING(SIZE (9)) */
  p_velocity->u.horandvervel->bearing.numbits = 9;
  /* high byte - upper 8 (of 9) bits */
  u_bearing_bit_string[0] = (s_bearing >> 0x01) & 0xFF;
  /* low byte - lowest 1 bit */
  u_bearing_bit_string[1] = s_bearing & 0x01;
  u_bearing_bit_string[1] <<= 0x07;
  p_velocity->u.horandvervel->bearing.data[0] = u_bearing_bit_string[0];
  p_velocity->u.horandvervel->bearing.data[1] = u_bearing_bit_string[1];


  /*   verspeed        BIT STRING(SIZE (8)) */
  p_velocity->u.horandvervel->verspeed.numbits = 8;
  p_velocity->u.horandvervel->verspeed.data[0] = u_ver_velocity_bit_string;

  /*   verdirect       BIT STRING(SIZE (1)) */
  p_velocity->u.horandvervel->verdirect.numbits = 1;
  u_ver_direct_bit_string = u_ver_direct << 7;
  p_velocity->u.horandvervel->verdirect.data[0] = u_ver_direct_bit_string;

}

/*
 ******************************************************************************
 *
 * supl_convert_supl_end_pos_for_tm_core
 *
 * Function description:
 *  This function converts position / velocity info in SUPL_END to that of
 *  pdsm_pd_info_s_type variable of z_supl_pd_info_buffer, and prepares the
 *  position for posting to TmCore as type LR.
 *
 * Parameters:
 *   p_suplEnd : pointer to SUPL_END message buffer.
 *   p_ref_loc : pointer of structre for seed location that
 *               is to be populated by this function.
 *
 * Return value:
 *   None.
 *
 ******************************************************************************
*/

static void supl_convert_supl_end_pos_for_tm_core
(const supl_SUPLEND* p_suplEnd, gps_RefLocStructType *p_ref_loc )
{

  pdsm_pd_info_s_type*     p_pd_info_buffer = &z_supl_pd_info_buffer;
  uint32                   q_temp = 0;
  uint16                   w_temp = 0;
  double  d_temp;
  float   f_temp;
  char    timestamp[14];
  uint16  day, month, year, hour, minute, second;
  uint16 gps_week = 0;
  uint32 gps_towms = 0;
  uint8 leap_seconds = 0;

  uint8 u_MajorUncK, u_MinorUncK, u_UncAngle, u_IS801Major, u_IS801Minor, u_IS801Angle;
  float f_major_unc_meters, f_minor_unc_meters;

  /*
   *  Initialize the return structure to 0.  This sets all validity
   *  flags to FALSE and all integer and FLT/DBL numbers to 0.
   */
  memset(p_ref_loc, 0, sizeof( *p_ref_loc ) );

  /*
   *  Initialize uncertainty to MAX values "just in case"
   */
  p_ref_loc->f_LocUncrtntySemiMajor = MAX_UNCERTAINTY;
  p_ref_loc->f_LocUncrtntySemiMinor = MAX_UNCERTAINTY;
  p_ref_loc->f_AltitudeUnc          = MAX_UNCERTAINTY;

  /* includes optional extended position fields */
  p_pd_info_buffer->pd_info.pd_data.opt_field_mask = PDSM_PD_EXT_STATUS_VALID;

  /* Set UTC Time */
  memscpy(timestamp, sizeof(timestamp), p_suplEnd->position.timestamp, sizeof(timestamp));
  year = (timestamp[0] - '0')*10 + (timestamp[1] - '0') + 2000; /* Convert 2 digit year to 4 digit year */
  month = (timestamp[2] - '0')*10 + (timestamp[3] - '0');
  day = (timestamp[4] - '0')*10 + (timestamp[5] - '0');
  z_supl_pd_info_buffer.pd_info.pd_data.ext_pos_data.utc_time.date =
      (uint32) ((day * 1000000) + (month * 10000) + year);
  hour = (timestamp[6] - '0')*10 + (timestamp[7] - '0');
  minute = (timestamp[8] - '0')*10 + (timestamp[9] - '0');
  second = (timestamp[10] - '0')*10 + (timestamp[11] - '0');
  z_supl_pd_info_buffer.pd_info.pd_data.ext_pos_data.utc_time.hour = hour;
  z_supl_pd_info_buffer.pd_info.pd_data.ext_pos_data.utc_time.minute = minute;
  z_supl_pd_info_buffer.pd_info.pd_data.ext_pos_data.utc_time.sec = second * 100;
  z_supl_pd_info_buffer.pd_info.pd_data.ext_opt_field_mask |= PDSM_PD_UTC_TIME_VALID;

  /*Calculate the GPS time from the SUPL END position UTC timestamp*/
  tm_core_get_nv_utc_offset((uint8*)&leap_seconds);

  if(gnss_ConvertUtcTimeStamptoGpsTime(&z_supl_pd_info_buffer.pd_info.pd_data.ext_pos_data.utc_time,
                                        leap_seconds ,
                                        &gps_week,
                                        &gps_towms))
  {
    z_supl_pd_info_buffer.pd_info.pd_data.ext_pos_data.timestamp_gps_week = gps_week;
    z_supl_pd_info_buffer.pd_info.pd_data.ext_pos_data.timestamp_tow_ms = gps_towms;

    p_ref_loc->w_GpsWeek = gps_week;

    p_ref_loc->q_GpsMsec = gps_towms;


    p_pd_info_buffer->pd_info.pd_data.time_stamp = (uint32)( tm_umts_common_utils_full_gps_ms( gps_week,
                                                              gps_towms ) / SEC_MSECS );

    MSG_HIGH("GPS Week %d GPS Tow MSec %d Full GPS Sec %d",
        p_ref_loc->w_GpsWeek,
        p_ref_loc->q_GpsMsec,
        p_pd_info_buffer->pd_info.pd_data.time_stamp);
    z_supl_pd_info_buffer.pd_info.pd_data.ext_opt_field_mask |= PDSM_PD_GPS_TIME_VALID;
  }

  if (tm_core_get_trusted_utctime()==TRUE)
  {
    z_supl_pd_info_buffer.pd_info.pd_data.opt_field_mask |= PDSM_PD_UTCOFFSET_VALID;
    tm_core_get_nv_utc_offset((uint8*)&z_supl_pd_info_buffer.pd_info.pd_data.gpsUtcOffset);
  }

  /* LATITUDE:  construct a 24-bit GAD shape Latitude with Bit 23 being the sign bit */
  q_temp = p_suplEnd->position.positionEstimate.latitude & 0x007FFFFF;
  q_temp |= p_suplEnd->position.positionEstimate.latitudeSign << 23;

  /* PDAPI lat. LSB is ( 180 / 2^25 ) degrees, and GAD shape lat. LSB
     is (90 / 2^23 ), so, GadShapeLat * (90/2^23 ) / (180 / 2^25 ) =
     GadShapeLat * 2
  */
  /* Absolute value of ext_pos_data.lat = (abs(lat rad) * C_2_32_OVER_PI);
     q_temp * LAT_IS801_SCALING ==> abs(lat degrees)
     LAT_IS801_SCALING = 90/2^23
     abs( ext_pos_data.lat ) =
       q_temp * 90 / 2^23 * 2^32 / PI * DEG_TO_RADIANS =
       q_temp * 90 * 2^9 * DEG_TO_RADIANS / PI =
       q_temp * 90 * 2^9 * PI / 180 / PI =
       q_temp * 2^8
  */


  if ( q_temp & 0x00800000 )
  {
    /* southern hemisphere */
    q_temp &= 0x007FFFFF;
    p_pd_info_buffer->pd_info.pd_data.lat  = (int32)( 0 - (int32)(q_temp * 2) );
    p_pd_info_buffer->pd_info.pd_data.ext_pos_data.lat = (int32)( 0 - (int32)(q_temp * 256));
    d_temp = (double) ( 0 - (int32)q_temp );
  }
  else
  {
    /* northern hemisphere */
    q_temp &= 0x007FFFFF;
    p_pd_info_buffer->pd_info.pd_data.lat  = (int32)( q_temp * 2 );
    p_pd_info_buffer->pd_info.pd_data.ext_pos_data.lat = (int32)(q_temp * 256);
    d_temp = (double) q_temp;
  }
  p_ref_loc->d_Latitude = d_temp * LAT_IS801_SCALING * DEG_TO_RADIANS;


  /* LONGITUDE: construct a 32-bit 2's complement ************************/
  q_temp = p_suplEnd->position.positionEstimate.longitude & 0x00FFFFFF;
  if (q_temp & 0x00800000)
  {
    q_temp |=  0xFF800000; /* sign extend */
  }

  /* PDAPI longitude has LSB of (180/2^25) degrees, and GAD shape lat. LSB
     is (360/2^24) degrees, so GadShapeLong * (360/2^24) / (180/2^25 ) =
     GadShapeLong * 4
  */
  /* ext_pos_data.lon = lon_rad * C_2_31_OVER_PI
     q_long_word * LON_IS801_SCALING ==> longitude degrees
     LON_IS801_SCALING = 180.0/(DBL)(1L<<23)
     ext_pos_data.lon =
       q_long_word * 180 / 2^23 * 2^31 / PI * DEG_TO_RADIANS =
       q_long_word * 180 * 2^8 * DEG_TO_RADIANS / PI =
       q_long_word * 180 * 2^8 * PI / 180 / PI =
       q_long_word * 2^8
  */
  p_pd_info_buffer->pd_info.pd_data.lon = (int32)q_temp * 4;
  p_pd_info_buffer->pd_info.pd_data.ext_pos_data.lon = (int32)(q_temp) * 256;
  d_temp = (DBL) ((int32) q_temp);
  p_ref_loc->d_Longitude = d_temp * LON_IS801_SCALING * DEG_TO_RADIANS;
  p_pd_info_buffer->pd_info.pd_data.ext_opt_field_mask |= PDSM_PD_LAT_LONG_VALID;

  if ( p_suplEnd->position.positionEstimate.m.altitudeInfoPresent )
  {

    /* 3D fix */
    p_pd_info_buffer->pd_info.pd_data.fix_type = 1;
    p_pd_info_buffer->pd_info.pd_data.opt_field_mask |= PDSM_PD_ALTITUDE_VALID;

    p_ref_loc->u_AltitudeIsValid = TRUE;

    q_temp = p_suplEnd->position.positionEstimate.altitudeInfo.altitude & 0x0000FFFF;
    q_temp |= p_suplEnd->position.positionEstimate.altitudeInfo.altitudeDirection << 15;

    if ( q_temp & 0x00008000 )
    {
      p_pd_info_buffer->pd_info.pd_data.altitude = -(int16)(q_temp & 0x00007FFF); /* signed, in meter */
      p_ref_loc->f_Altitude = -(float)(q_temp & 0x7FFF);

    }
    else
    {
      p_pd_info_buffer->pd_info.pd_data.altitude = (int16)(q_temp & 0x00007FFF); /* signed, in meter */
      p_ref_loc->f_Altitude = (float)(q_temp & 0x7FFF);

    }
    p_pd_info_buffer->pd_info.pd_data.altitude += 500; /* BREW needs 500-meter altitude offset */


    if ( p_suplEnd->position.positionEstimate.m.uncertaintyPresent )
    {
      uint8 u_temp;
      float f_alt_unc_meter;

      u_temp = p_suplEnd->position.positionEstimate.altitudeInfo.altUncertainty & 0x7F;
      f_alt_unc_meter =  tm_umts_common_utils_GADK_to_meter(FALSE, u_temp);

      p_pd_info_buffer->pd_info.pd_data.loc_uncertainty_v =
        (uint8) tm_util_select_pos_unc( f_alt_unc_meter );

      p_ref_loc->f_AltitudeUnc = f_alt_unc_meter;

    }

  } /* close if-altitude-present */


  if ( p_suplEnd->position.positionEstimate.m.uncertaintyPresent )
  {
    p_pd_info_buffer->pd_info.pd_data.opt_field_mask |= PDSM_PD_UNCERTAINTY_VALID;


    /* we get major axis unc, minor axis unc, and angle from North
     * of major axis (0 to 178 degrees). */
    u_MajorUncK = p_suplEnd->position.positionEstimate.uncertainty.uncertaintySemiMajor & 0x7F; /* Major axis unc */
    u_MinorUncK = p_suplEnd->position.positionEstimate.uncertainty.uncertaintySemiMinor & 0x7F; /* Minor axis unc */

    /* Angle from North of Major axis:
     *    2N < angle < 2(N+1)
     * where N = 0..89
     */
    u_UncAngle  = p_suplEnd->position.positionEstimate.uncertainty.orientationMajorAxis << 1 ;

    f_major_unc_meters = tm_umts_common_utils_GADK_to_meter( TRUE, u_MajorUncK );
    f_minor_unc_meters = tm_umts_common_utils_GADK_to_meter( TRUE, u_MinorUncK );
    p_ref_loc->f_LocUncrtntySemiMajor = f_major_unc_meters;
    p_ref_loc->f_LocUncrtntySemiMinor = f_minor_unc_meters;

    u_IS801Major = (uint8)tm_util_select_pos_unc( f_major_unc_meters );
    u_IS801Minor = (uint8)tm_util_select_pos_unc( f_minor_unc_meters );


    p_pd_info_buffer->pd_info.pd_data.ext_pos_data.loc_unc_horizontal =
     (uint32)sqrt((f_major_unc_meters * f_major_unc_meters) +
            (f_minor_unc_meters * f_minor_unc_meters) );
    if( u_UncAngle > (2*89) )
      u_UncAngle = 2*89;

    if( u_UncAngle >= 90 )
    {
      /* Major is perp axis, and minor is along axis.
       * Subtract 90 from angle */
      u_UncAngle -= 90;

      p_pd_info_buffer->pd_info.pd_data.loc_uncertainty_a = u_IS801Minor;
      p_pd_info_buffer->pd_info.pd_data.loc_uncertainty_p = u_IS801Major;

      /* ext_pos_data.alongAxisUnc = (uint16)ext_pos_data_ptr->ext_loc_uncrtnty_a */
      p_pd_info_buffer->pd_info.pd_data.ext_pos_data.alongAxisUnc =
        (uint16)f_minor_unc_meters;
      p_pd_info_buffer->pd_info.pd_data.ext_pos_data.perpAxisUnc =
        (uint16)f_major_unc_meters;

    }
    else
    {
      /* Major is along axis, and minor is perp axis. */
      p_pd_info_buffer->pd_info.pd_data.loc_uncertainty_a = u_IS801Major;
      p_pd_info_buffer->pd_info.pd_data.loc_uncertainty_p = u_IS801Minor;

      /* ext_pos_data.alongAxisUnc = (uint16)ext_pos_data_ptr->ext_loc_uncrtnty_a */
      p_pd_info_buffer->pd_info.pd_data.ext_pos_data.alongAxisUnc =
        (uint16)f_major_unc_meters;
      p_pd_info_buffer->pd_info.pd_data.ext_pos_data.perpAxisUnc =
        (uint16)f_minor_unc_meters;

    }



    /* The angle is defined as follows in IS801.
     * LOC_UNCRTNTY_ANG – Angle of axis with respect to True North for
     * position uncertainty. The mobile station shall set this field to
     * the angle of axis for position uncertainty, in units of 5.625
     * degrees, in the range from 0 to 84.375 degrees, where 0 degrees
     * is True North and the angle increases toward the East.
     *
     * The coded value of the range is hence from 0 to 15.  We'll
     * interpret this as:
     *
     *    N * 5.625 <= angle < (N+1) * 5.625
     *
     * where N is the coded value, since this would be the most logical way
     * of dividing up the coded space EVENLY.  This is therefore just a
     * simple truncation.
     */
    p_ref_loc->f_LocUncrtntyAng = (float)u_UncAngle;
    u_IS801Angle = (uint8)( (float)u_UncAngle / (float)5.625 );

    /* With the limit checking above, the u_UncAngle is guaranteed to
     * be LESS than 90, so the u_IS801Angle is in the correct range
     * of 0..15.  No further limit checking needed. */

    p_pd_info_buffer->pd_info.pd_data.loc_uncertainty_ang = u_IS801Angle;


    if ( p_suplEnd->position.positionEstimate.m.confidencePresent )
    {
      p_pd_info_buffer->pd_info.pd_data.loc_uncertainty_conf = p_suplEnd->position.positionEstimate.confidence & 0x7F;
    }
    else
    {
      p_pd_info_buffer->pd_info.pd_data.loc_uncertainty_conf = 0; /* 0 means "no information" */
    }

  } /* close if-uncertainty-present */

  /****************** if velocity is available *************************/

  if (p_suplEnd->position.m.velocityPresent)
  {
    uint16 w_horizontal_speed, w_heading_angle, w_speed_unc;
    uint8  u_vertical_speed;

    if ( p_suplEnd->position.velocity.t == C_SUPL_ASN1_CHOICE_VELCITY_HORVEL )
    {
      if ( p_suplEnd->position.velocity.u.horvel == NULL )
      {

        MSG_HIGH("velocity data buffer NULL, discarded", 0, 0, 0);
      }
      else if ( ( p_suplEnd->position.velocity.u.horvel->horspeed.numbits != 16 ) ||
                ( p_suplEnd->position.velocity.u.horvel->bearing.numbits != 9 )
              )
      {
        MSG_HIGH("velocity data length error, discarded", 0, 0, 0);
      }
      else
      {

        p_pd_info_buffer->pd_info.pd_data.opt_field_mask |= PDSM_PD_VELOCITY_VALID;
        p_ref_loc->u_VelocityIsValid = TRUE;

        /* horizontal speed: horspeed is defined to be of 16 bits */
        w_horizontal_speed = (uint16)(p_suplEnd->position.velocity.u.horvel->horspeed.data[0]);
        w_horizontal_speed <<= 8;
        w_horizontal_speed |= (uint16)(p_suplEnd->position.velocity.u.horvel->horspeed.data[1]);

        /* SUPL provides horspeed in the increment of 1 km/hour 16 bits: convert to meter/sec. */
        f_temp = (float)w_horizontal_speed * 0.277f;

        /* Compute horizontal velocity from components and scale to units of 0.25 m/s, as per PDAPI spec. */
        p_pd_info_buffer->pd_info.pd_data.velocity_hor =
          (uint16) tm_util_nearest_long ( 4.0f * f_temp );

        /* Compute horizontal velocity in pdsm_pd_ext_pos_data_s_type velHor
           from components and scale to units of 0.01 m/s, as per PDAPI spec.
           verHor is defined as int16 in pdapi while no bearing is included, so
           only magnitude of velocity should be reported here and should be limited
           to 0x7FFFF;
        */
        w_temp = (uint16) tm_util_nearest_long ( 100.0f * f_temp );
        if ( w_temp <= 0x7FFF )
        {
          p_pd_info_buffer->pd_info.pd_data.ext_pos_data.velHor = (int16)w_temp;
        }
        else
        {
          p_pd_info_buffer->pd_info.pd_data.ext_pos_data.velHor = (int16)0x7FFF;
        }

        p_ref_loc->f_VelHor = (float)f_temp;

        /*************** bearing ******************************************/
        w_heading_angle = (uint16)(p_suplEnd->position.velocity.u.horvel->bearing.data[0]);
        w_heading_angle <<= 8;
        w_heading_angle |= (uint16)( p_suplEnd->position.velocity.u.horvel->bearing.data[1]) ;
        w_heading_angle &= 0x01FF;

        p_pd_info_buffer->pd_info.pd_data.heading =
          (uint16) tm_util_nearest_long( w_heading_angle * HEADING_SCALE );

        p_ref_loc->u_HeadingIsValid = TRUE;
        p_ref_loc->f_Heading = (float)w_heading_angle;

      } /* supl_end structure data sanity check ok */

    }  /* close if C_SUPL_ASN1_CHOICE_VELCITY_HORVEL */

    else if ( p_suplEnd->position.velocity.t == C_SUPL_ASN1_CHOICE_VELCITY_HORANDVERVEL )
    {

      if ( p_suplEnd->position.velocity.u.horandvervel == NULL )
      {
        MSG_HIGH("velocity data buffer NULL, discarded", 0, 0, 0);
      }
      else if ( ( p_suplEnd->position.velocity.u.horandvervel->horspeed.numbits != 16 ) ||
           ( p_suplEnd->position.velocity.u.horandvervel->verspeed.numbits != 8 ) ||
           ( p_suplEnd->position.velocity.u.horandvervel->verdirect.numbits != 1 ) ||
           ( p_suplEnd->position.velocity.u.horandvervel->bearing.numbits != 9 )
         )
      {
        MSG_HIGH("SUPL_END velocity data length error, discarded", 0, 0, 0);
      }
      else
      {
        p_pd_info_buffer->pd_info.pd_data.opt_field_mask |= PDSM_PD_VELOCITY_VALID;
        p_ref_loc->u_VelocityIsValid = TRUE;

        /* Horizontal speed: horspeed is defined to be of 16 bits in its bitstring */
        w_horizontal_speed = (uint16)(p_suplEnd->position.velocity.u.horandvervel->horspeed.data[0]);
        w_horizontal_speed <<= 8;
        w_horizontal_speed |= (uint16)( p_suplEnd->position.velocity.u.horandvervel->horspeed.data[1]);

        /* SUPL provides horspeed in the increment of 1 km/hour 16 bits: convert to meter/sec. */
        f_temp = (float)w_horizontal_speed * 0.277f;

        /* Compute horizontal velocity from components and scale to units of 0.25 m/s, as per PDAPI spec. */
        p_pd_info_buffer->pd_info.pd_data.velocity_hor =
          (uint16) tm_util_nearest_long ( 4.0f * f_temp );

        /* Compute horizontal velocity in pdsm_pd_ext_pos_data_s_type velHor
           from components and scale to units of 0.01 m/s, as per PDAPI spec.
           verHor is defined as int16 in pdapi while no bearing is included, so
           only magnitude of velocity should be reported here and should be limited
           to 0x7FFFF
        */
        w_temp = (uint16) tm_util_nearest_long ( 100.0f * f_temp );
        if ( w_temp <= 0x7FFF )
        {
          p_pd_info_buffer->pd_info.pd_data.ext_pos_data.velHor = (int16)w_temp;
        }
        else
        {
          p_pd_info_buffer->pd_info.pd_data.ext_pos_data.velHor = (int16)0x7FFF;
        }

        p_ref_loc->f_VelHor = f_temp;

        /************* vertical speed ************************************************/

        u_vertical_speed = p_suplEnd->position.velocity.u.horandvervel->verspeed.data[0];

        /* SUPL provides verspeed in the increment of 1 km/hour 16 bits: convert to meter/sec. */
        f_temp = (float)u_vertical_speed * 0.277f;

        if ( p_suplEnd->position.velocity.u.horandvervel->verdirect.data[0] == 0 ) /* upward */
        {

          p_pd_info_buffer->pd_info.pd_data.velocity_ver =
            (int8) tm_util_nearest_long ( 2.0f * f_temp );

          /* Compute vertical velocity in pdsm_pd_ext_pos_data_s_type velVert
             from components and scale to units of 0.01 m/s, as per PDAPI spec. */
          p_pd_info_buffer->pd_info.pd_data.ext_pos_data.velVert =
            (int16) tm_util_nearest_long ( 100.0f * f_temp );
        }
        else /* downward */
        {

          p_pd_info_buffer->pd_info.pd_data.velocity_ver =
            0 - (int8) tm_util_nearest_long ( 2.0f * f_temp );

          /* Compute vertical velocity in pdsm_pd_ext_pos_data_s_type velVert
           from components and scale to units of 0.01 m/s, as per PDAPI spec. */
          p_pd_info_buffer->pd_info.pd_data.ext_pos_data.velVert =
            0 - (int16) tm_util_nearest_long ( 100.0f * f_temp );
        }
        p_ref_loc->f_VelVert = f_temp;

        /************ bearing *******************************************/

        w_heading_angle = (uint16)(p_suplEnd->position.velocity.u.horandvervel->bearing.data[0]);
        w_heading_angle <<= 8;
        w_heading_angle |= (uint16)(p_suplEnd->position.velocity.u.horandvervel->bearing.data[1]);
        w_heading_angle &= 0x07FF;

        p_pd_info_buffer->pd_info.pd_data.heading =
          (uint16) tm_util_nearest_long( w_heading_angle * HEADING_SCALE );

        p_ref_loc->u_HeadingIsValid = TRUE;
        p_ref_loc->f_Heading = (float)w_heading_angle;

      } /* supl_end structure data sanity check ok */

    }  /* close if C_SUPL_ASN1_CHOICE_VELCITY_HORANDVERVEL */

    else if ( p_suplEnd->position.velocity.t == C_SUPL_ASN1_CHOICE_VELCITY_HORVELUNCERT )
    {

      /**** horizontal speed **************/

      if ( p_suplEnd->position.velocity.u.horveluncert == NULL )
      {
        MSG_HIGH("velocity data buffer NULL, discarded", 0, 0, 0);
      }
      else if ( ( p_suplEnd->position.velocity.u.horveluncert->horspeed.numbits != 16 ) ||
                ( p_suplEnd->position.velocity.u.horveluncert->bearing.numbits != 9 ) ||
                ( p_suplEnd->position.velocity.u.horveluncert->uncertspeed.numbits !=8 )
              )
      {
        MSG_HIGH("velocity data length error, discarded", 0, 0, 0);
      }
      else
      {

        p_pd_info_buffer->pd_info.pd_data.opt_field_mask |= PDSM_PD_VELOCITY_VALID;
        p_ref_loc->u_VelocityIsValid = TRUE;

        w_horizontal_speed = (uint16)(p_suplEnd->position.velocity.u.horveluncert->horspeed.data[0]);
        w_horizontal_speed <<= 8;
        w_horizontal_speed |= (uint16)(p_suplEnd->position.velocity.u.horveluncert->horspeed.data[0]);

        /* SUPL provides verspeed in the increment of 1 km/hour 16 bits: convert to meter/sec. */
        f_temp = (float)w_horizontal_speed * 0.277f;

        /* Compute horizontal velocity from components and scale to units of 0.25 m/s, as per PDAPI spec. */
        p_pd_info_buffer->pd_info.pd_data.velocity_hor =
          (uint16) tm_util_nearest_long ( 4.0f * f_temp );

        /* Compute horizontal velocity in pdsm_pd_ext_pos_data_s_type velHor
           from components and scale to units of 0.01 m/s, as per PDAPI spec.
           verHor is defined as int16 in pdapi while no bearing is included, so
           only magnitude of velocity should be reported here and should be limited
           to 0x7FFFF
        */
        w_temp = (uint16) tm_util_nearest_long ( 100.0f * f_temp );
        if ( w_temp <= 0x7FFF )
        {
          p_pd_info_buffer->pd_info.pd_data.ext_pos_data.velHor = (int16)w_temp;
        }
        else
        {
          p_pd_info_buffer->pd_info.pd_data.ext_pos_data.velHor = (int16)0x7FFF;
        }
        p_ref_loc->f_VelHor = f_temp;

        /**** bearing ******/
        w_heading_angle = (uint16)(p_suplEnd->position.velocity.u.horveluncert->bearing.data[0]);
        w_heading_angle <<= 8;
        w_heading_angle |= (uint16) (p_suplEnd->position.velocity.u.horveluncert->bearing.data[1]);

        p_pd_info_buffer->pd_info.pd_data.heading =
          (uint16) tm_util_nearest_long( w_heading_angle * HEADING_SCALE );

        p_ref_loc->u_HeadingIsValid = TRUE;
        p_ref_loc->f_Heading = (float)w_heading_angle;

        /**** hor. velocity. unc ******/

        w_speed_unc = (uint16)(p_suplEnd->position.velocity.u.horveluncert->uncertspeed.data[0]);

        /* SUPL provides speedunc in the increment of 1 km/hour 16 bits: convert to meter/sec. */
        f_temp = (float)w_speed_unc * 0.277f;

        /* Compute vertical velocity in pdsm_pd_ext_pos_data_s_type velUncHor
           from components and scale to units of 0.01 m/s, as per PDAPI spec. */
        p_pd_info_buffer->pd_info.pd_data.ext_pos_data.velUncHoriz =
          (uint16) tm_util_nearest_long( 100.0f * f_temp );

      } /* supl_end structure data sanity check ok */
    }  /* close if supl_horveluncert */

    else if ( p_suplEnd->position.velocity.t == C_SUPL_ASN1_CHOICE_VELCITY_HORANDVERUNCERT )
    {

      /***** horizontal speed *************************/

      if ( p_suplEnd->position.velocity.u.horandveruncert == NULL )
      {
        MSG_HIGH("SUPL_END velocity data error, discarded", 0, 0, 0);
      }
      else if ( ( p_suplEnd->position.velocity.u.horandveruncert->horspeed.numbits != 16 ) ||
                ( p_suplEnd->position.velocity.u.horandveruncert->verspeed.numbits != 8 ) ||
                ( p_suplEnd->position.velocity.u.horandveruncert->verdirect.numbits != 1 )||
                ( p_suplEnd->position.velocity.u.horandveruncert->bearing.numbits !=9 ) ||
                ( p_suplEnd->position.velocity.u.horandveruncert->horuncertspeed.numbits !=8 ) ||
                ( p_suplEnd->position.velocity.u.horandveruncert->veruncertspeed.numbits !=8 )
         )
      {
        MSG_HIGH("SUPL_END velocity data error, discarded", 0, 0, 0);
      }

      else
      {
        p_pd_info_buffer->pd_info.pd_data.opt_field_mask |= PDSM_PD_VELOCITY_VALID;
        p_ref_loc->u_VelocityIsValid = TRUE;

        w_horizontal_speed = (uint16)(p_suplEnd->position.velocity.u.horandveruncert->horspeed.data[0]);
        w_horizontal_speed <<= 8;
        w_horizontal_speed |= (uint16)( p_suplEnd->position.velocity.u.horandveruncert->horspeed.data[1] );

        /* SUPL provides horspeed in the increment of 1 km/hour 16 bits: convert to meter/sec. */
        f_temp = (float)w_horizontal_speed * 0.277f;

        /* Compute horizontal velocity from components and scale to units of 0.25 m/s, as per PDAPI spec. */
        p_pd_info_buffer->pd_info.pd_data.velocity_hor =
          (uint16) tm_util_nearest_long ( 4.0f * f_temp );

        /* Compute horizontal velocity in pdsm_pd_ext_pos_data_s_type velHor
           from components and scale to units of 0.01 m/s, as per PDAPI spec.
           verHor is defined as int16 in pdapi while no bearing is included, so
           only magnitude of velocity should be reported here and should be limited
           to 0x7FFFF
        */
        w_temp = (uint16) tm_util_nearest_long ( 100.0f * f_temp );
        if ( w_temp <= 0x7FFF )
        {
          p_pd_info_buffer->pd_info.pd_data.ext_pos_data.velHor = (int16)w_temp;
        }
        else
        {
          p_pd_info_buffer->pd_info.pd_data.ext_pos_data.velHor = (int16)0x7FFF;
        }

        p_ref_loc->f_VelHor = f_temp;

        /***** vertical speed *************************/
        u_vertical_speed = p_suplEnd->position.velocity.u.horandveruncert->verspeed.data[0];

        /* SUPL provides verspeed in the increment of 1 km/hour 16 bits: convert to meter/sec. */
        f_temp = (float)u_vertical_speed * 0.277f;

        if ( p_suplEnd->position.velocity.u.horandveruncert->verdirect.data[0] == 0 ) /* upward */
        {
          p_pd_info_buffer->pd_info.pd_data.velocity_ver =
            (int8) tm_util_nearest_long ( 2.0f * f_temp );

          /* Compute vertical velocity in pdsm_pd_ext_pos_data_s_type velVert
             from components and scale to units of 0.01 m/s, as per PDAPI spec. */
          p_pd_info_buffer->pd_info.pd_data.ext_pos_data.velVert =
            (int16) tm_util_nearest_long ( 100.0f * f_temp );
        }
        else /* downward */
        {
          p_pd_info_buffer->pd_info.pd_data.velocity_ver =
            0 - (int8) tm_util_nearest_long ( 2.0f * f_temp );

          /* Compute vertical velocity in pdsm_pd_ext_pos_data_s_type velVert
             from components and scale to units of 0.01 m/s, as per PDAPI spec. */
          p_pd_info_buffer->pd_info.pd_data.ext_pos_data.velVert =
            0 - (int16) tm_util_nearest_long ( 100.0f * f_temp );
        }

        p_ref_loc->f_VelVert = f_temp;

        /************ bearing ********************************/

        w_heading_angle = (uint16)(p_suplEnd->position.velocity.u.horandveruncert->bearing.data[0]);
        w_heading_angle <<= 8;
        w_heading_angle |= (uint16) ( p_suplEnd->position.velocity.u.horandveruncert->bearing.data[0] );

        p_pd_info_buffer->pd_info.pd_data.heading =
          (uint16) tm_util_nearest_long( w_heading_angle * HEADING_SCALE );

        p_ref_loc->u_HeadingIsValid = TRUE;
        p_ref_loc->f_Heading = (float)w_heading_angle;

        /**** horizontal velocity unc ******/

        w_speed_unc = (uint16)(p_suplEnd->position.velocity.u.horandveruncert->horuncertspeed.data[0]);

        /* SUPL provides speedunc in the increment of 1 km/hour 16 bits: convert to meter/sec. */
        f_temp = (float)w_speed_unc * 0.277f;

        /* Compute vertical velocity in pdsm_pd_ext_pos_data_s_type velUncHor
           from components and scale to units of 0.01 m/s, as per PDAPI spec. */
        p_pd_info_buffer->pd_info.pd_data.ext_pos_data.velUncHoriz =
          (uint16) tm_util_nearest_long( 100.0f * f_temp );

        /**** vertical velocity unc ******/
        w_speed_unc = (uint16)(p_suplEnd->position.velocity.u.horandveruncert->veruncertspeed.data[0]);

        /* SUPL provides speedunc in the increment of 1 km/hour 16 bits: convert to meter/sec. */
        f_temp = (float)w_speed_unc * 0.277f;

        /* Compute vertical velocity unc. in pdsm_pd_ext_pos_data_s_type verUncVert
           from components and scale to units of 0.01 m/s, as per PDAPI spec. */
        p_pd_info_buffer->pd_info.pd_data.ext_pos_data.velUncVert =
          (uint16) tm_util_nearest_long( 100.0f * f_temp );

      } /* supl_end structure data sanity check ok */

    }  /* close if  supl_horandveruncert */

  }  /* close if-velocity available */

}


/*
 ******************************************************************************
 *
 * supl_glo_eph_build
 *
 * Function description:
 *  Buffers Glonass navigation model in a buffer that associates its SV ID with
 *  the corresponding frequency number
 *
 * Parameters:
 *   p_ganss_sat_element : pointer to the structure holding Glonass nav. model.
 *
 * Return value:
 *   None.
 *
 ******************************************************************************
*/

static void supl_glo_eph_build
(rrlp_GANSSSatelliteElement *p_ganss_sat_element)
{
  supl_status_s_type*    p_supl_session_status = &z_supl_session_status;
  glo_EphStructType*     p_eph_buffer_per_sv;

  /* TM API uses 65-88 for Glonass SVs, RRLP decoded SV ID ranges from 0-63 though
     Glonass uses only 24 SVs. */
  if ( p_ganss_sat_element->svID >= N_GLO_SV )
  {
    return;
  }

  /* init. */
  p_eph_buffer_per_sv =
    &p_supl_session_status->z_prot_glo_eph_buffer[p_ganss_sat_element->svID].z_eph_glo_data;

  p_eph_buffer_per_sv->z_Mask.u_FilledMask = 0;


  p_eph_buffer_per_sv->u_SvId = p_ganss_sat_element->svID + GLO_SLOTID_OFFSET + 1; /* start at 65 */

  /* Glonass eph. reference time */
  p_eph_buffer_per_sv->u_Tb = p_ganss_sat_element->iod;

  /* Glonass ICD: 4 LSB bits in svHealth are Ft;
     API: Ft is a parameter shown the URE at time tb: 4-bit LSBs;
     RRLP: svHealth BIT STRING (SIZE(5)) */
  p_eph_buffer_per_sv->u_Ft = (*(p_ganss_sat_element->svHealth.data) >> 3) & 0xF;
  p_eph_buffer_per_sv->z_Mask.z_FilledMask.u_FtFlag = TRUE;

  /* From RRLP: Bn(MSB) is Bit 1 of the 5-bit OSS_BITSTRING svHealth
     (Bit 5 being LSB); from API: Health flag Bn: 3-bit LSBs, only check bit 2 */
  p_eph_buffer_per_sv->u_Bn = (*p_ganss_sat_element->svHealth.data) >> 5 & 0x2;

  /* Gamma_n (ICD defines as 11 bits, scale factor 2^-40 ),
     RRLP decodes into 2's complement,
     MGP API: w_FreqOff using ICD format of sign+magnitude */
  p_eph_buffer_per_sv->w_FreqOff = tm_umts_common_utils_2s_comp_to_sign_mag(
            p_ganss_sat_element->ganssClockModel.u.glonassClockModel->gloGamma, 0x400);


  /* Tau_n: (IDC defines as 22 bits, scale factor 2^-30),
     RRLP decodes into 2's complement,
     MGP API: q_ClkCorr using ICD format of sign+magnitude */
  p_eph_buffer_per_sv->q_ClkCorr = tm_umts_common_utils_2s_comp_to_sign_mag(
            p_ganss_sat_element->ganssClockModel.u.glonassClockModel->gloTau, 0x200000);


  /* Delta Tau_n: (IDC defines as 5 bits, scale factor 2^-30); MGP API: u_L1L2; RRLP - optional */
  if ( p_ganss_sat_element->ganssClockModel.u.glonassClockModel->m.gloDeltaTauPresent  )
  {
    p_eph_buffer_per_sv->u_L1L2 = tm_umts_common_utils_2s_comp_to_sign_mag(
            p_ganss_sat_element->ganssClockModel.u.glonassClockModel->gloDeltaTau, 0x10);
  }
  else
  {
    /* temp. solution until gnss_common.h is updated with added FilledMask element to support this */
    p_eph_buffer_per_sv->u_L1L2 = 0;
  }


  /* En: 5 bits. RRLP: Integer (0...31) */
  p_eph_buffer_per_sv->u_En =
    p_ganss_sat_element->ganssOrbitModel.u.glonassECEF->gloEn;

  /* P1: 2 bits. RRLP: Bit string size 2 */
  p_eph_buffer_per_sv->u_P1 =
    *(p_ganss_sat_element->ganssOrbitModel.u.glonassECEF->gloP1.data);
  p_eph_buffer_per_sv->u_P1 >>= 6;

  /* P2: RRLP: boolean */
  p_eph_buffer_per_sv->u_P2 =
    p_ganss_sat_element->ganssOrbitModel.u.glonassECEF->gloP2;

  /* M: RRLP: Integer (0..3) */
    p_eph_buffer_per_sv->u_GloM =
    p_ganss_sat_element->ganssOrbitModel.u.glonassECEF->gloM;

    p_eph_buffer_per_sv->z_Mask.z_FilledMask.u_GloMFlag = 1;

  /* X: RRLP: Integer (-6710864..67108863);
        API expects ICD format 27 bits: sign magnitude */
  p_eph_buffer_per_sv->q_SatPos[0] =
    tm_umts_common_utils_2s_comp_to_sign_mag(
      p_ganss_sat_element->ganssOrbitModel.u.glonassECEF->gloX,
      0x04000000 );

  /* X-dot: RRLP: Integer ( -8388608..8388607)
     API expects ICD format 24 bits: sign-magnitude */
  p_eph_buffer_per_sv->q_SatVel[0] =
    tm_umts_common_utils_2s_comp_to_sign_mag(
      p_ganss_sat_element->ganssOrbitModel.u.glonassECEF->gloXdot,
      0x00800000 );

  /* X-dot-dot: RRLP: Integer ( -16..15 )
     API expects ICD format 5 bits: sign-magnitude */
  p_eph_buffer_per_sv->u_Acc[0] =
    tm_umts_common_utils_2s_comp_to_sign_mag(
      p_ganss_sat_element->ganssOrbitModel.u.glonassECEF->gloXdotdot,
      0x00000010 );

  /* Y: RRLP: Integer (-6710864..67108863)
     API expects ICD format 24 bits: sign-magnitude */
  p_eph_buffer_per_sv->q_SatPos[1] =
    tm_umts_common_utils_2s_comp_to_sign_mag(
      p_ganss_sat_element->ganssOrbitModel.u.glonassECEF->gloY,
      0x04000000 );

  /* Y-dot: RRLP: Integer ( -8388608..6388607)
     API expects ICD format 24 bits: sign-magnitude */
  p_eph_buffer_per_sv->q_SatVel[1] =
    tm_umts_common_utils_2s_comp_to_sign_mag(
      p_ganss_sat_element->ganssOrbitModel.u.glonassECEF->gloYdot,
      0x00800000 );

  /* Y-dot-dot: RRLP: Integer ( -16..15 )
     API expects ICD format 5 bits: sign-magnitude */
  p_eph_buffer_per_sv->u_Acc[1] =
    tm_umts_common_utils_2s_comp_to_sign_mag(
      p_ganss_sat_element->ganssOrbitModel.u.glonassECEF->gloYdotdot,
      0x00000010 );

  /* Z: RRLP: Integer (-6710864..67108863)
     API expects ICD format 24 bits: sign-magnitude */
  p_eph_buffer_per_sv->q_SatPos[2] =
    tm_umts_common_utils_2s_comp_to_sign_mag(
      p_ganss_sat_element->ganssOrbitModel.u.glonassECEF->gloZ,
      0x04000000 );

  /* Z-dot: RRLP: Integer ( -8388608..6388607)
     API expects ICD format 24 bits: sign-magnitude */
  p_eph_buffer_per_sv->q_SatVel[2] =
    tm_umts_common_utils_2s_comp_to_sign_mag(
      p_ganss_sat_element->ganssOrbitModel.u.glonassECEF->gloZdot,
      0x00800000 );

  /* Y-dot-dot: RRLP: Integer ( -16..15 )
     API expects ICD format 5 bits: sign-magnitude */
  p_eph_buffer_per_sv->u_Acc[2] =
    tm_umts_common_utils_2s_comp_to_sign_mag(
      p_ganss_sat_element->ganssOrbitModel.u.glonassECEF->gloZdotdot,
      0x00000010 );

  p_eph_buffer_per_sv->z_Mask.z_FilledMask.u_LnFlag = 0;
  p_eph_buffer_per_sv->z_Mask.z_FilledMask.u_NtFlag = 0;
  p_eph_buffer_per_sv->z_Mask.z_FilledMask.u_P4Flag = 0;
  p_eph_buffer_per_sv->z_Mask.z_FilledMask.u_TimeSrcPFlag = 0;

}


/*
 ******************************************************************************
 *
 * supl_glo_eph_svid_freq_num_map_aux
 *
 * Function description:
 *  Associates sv ID with a frequencey number for Glonass eph. data using
 *  information from auxilary assistance.
 *
 * Parameters:
 *   p_glo_aux : pointer to the structure holding Glonass aux. data.
 *
 * Return value:
 *   None.
 *
 ******************************************************************************
*/

static void supl_glo_eph_svid_freq_num_map_aux (struct rrlp_GANSS_ID3 *p_glo_aux )
{
  supl_status_s_type*    p_supl_session_status = &z_supl_session_status;
  struct rrlp_GANSS_ID3         *p_local_rrlp_aux_list = NULL;
  struct rrlp_GANSS_ID3_element *p_local_rrlp_aux_elem = NULL;
  uint8                         u_i;

  p_local_rrlp_aux_list = p_glo_aux;
  p_local_rrlp_aux_elem = p_local_rrlp_aux_list->elem;

  for ( u_i = 0; u_i < p_local_rrlp_aux_list->n; u_i++, p_local_rrlp_aux_elem++)
  {
    if (  ( p_local_rrlp_aux_elem->svID <= ( N_GLO_SV - 1 ) ) &&
          ( ( p_local_rrlp_aux_elem->channelNumber >= C_PROT_GLO_FREQ_MIN ) &&
            ( p_local_rrlp_aux_elem->channelNumber <= C_PROT_GLO_FREQ_MAX )
          )
       )
    {
      p_supl_session_status->z_prot_glo_eph_buffer[p_local_rrlp_aux_elem->svID].u_freq_num_valid = TRUE;

      p_supl_session_status->z_prot_glo_eph_buffer[p_local_rrlp_aux_elem->svID].z_eph_glo_data.u_FreqNum =
        p_local_rrlp_aux_elem->channelNumber & 0x1F; /* the MGP API expects 5 bits */
    }
    else
    {
      MSG_HIGH("Incorrect Glonass SV_id=%u or channel#=%d, u_i=%u",
               p_local_rrlp_aux_elem->svID, p_local_rrlp_aux_elem->channelNumber, u_i);
    }
  }
}

/*
 ******************************************************************************
 *
 * supl_glo_eph_svid_freq_num_map_alm
 *
 * Function description:
 *  Associates sv ID with a frequencey number for Glonass eph. data using
 *  information from auxilary assistance.
 *
 * Parameters:
 *   p_glo_alm_set : pointer to the structure holding Glonass alm. model.
 *
 * Return value:
 *   None.
 *
 ******************************************************************************
*/

static void supl_glo_eph_svid_freq_num_map_alm
(rrlp_Almanac_GlonassAlmanacSet *p_glo_alm_set)
{
  supl_status_s_type*    p_supl_session_status = &z_supl_session_status;

  if ( (p_glo_alm_set->gloAlmnA < 1) || ( p_glo_alm_set->gloAlmnA > 24) ) // gloAlmnA INTEGER (1..24)
  {
    MSG_ERROR("GLO Alm SvID invalid: %d", p_glo_alm_set->gloAlmnA, 0, 0 );
    return;
  }

  /* n_A is SV_ID :  gloAlmnA RRLP: INTEGER (1..24) */
  if (p_glo_alm_set->gloAlmnA - 1 < N_GLO_SV)
  {
    p_supl_session_status->z_prot_glo_eph_buffer[p_glo_alm_set->gloAlmnA - 1].u_freq_num_valid = TRUE;
    p_supl_session_status->z_prot_glo_eph_buffer[p_glo_alm_set->gloAlmnA -1 ].z_eph_glo_data.u_FreqNum =
        p_glo_alm_set->gloAlmHA;
  }
  else
  {
  MSG_ERROR("GLO Alm SvID out of bounds: %d", p_glo_alm_set->gloAlmnA, 0, 0 );
  }
}



/*
 ******************************************************************************
 *
 * supl_glo_acqasst_svid_freq_num_map_aux
 *
 * Function description:
 *  Associates sv ID with a frequencey number for Glonass reference measurement
 *  data using information from auxilary assistance.
 *
 * Parameters:
 *   p_glo_aux : pointer to the structure containing GLO aux. assistance;
 *   p_glo_acqasst: point to the structure that saves GLONASS acq. assist.
 *   measurement.
 *
 * Return value:
 *   None.
 *
 ******************************************************************************
*/
static void supl_glo_acqasst_svid_freq_num_map_aux (
   struct rrlp_GANSS_ID3 *p_glo_aux,
   prot_glo_acqast_buffer_s_type *p_glo_acqasst)
{

  uint8 u_index = 0, u_sv_iter = 0, u_save_loop = 0;
  rrlp_GANSS_ID3_element *p_gnss_glonass_sat_elem = NULL;
  uint8 u_glo_num_svs = 0;

  if ( ( p_glo_aux == NULL ) || ( p_glo_aux->elem == NULL ) || ( p_glo_acqasst == NULL ) )
  {
    MSG_ERROR( "Invalid pointer(s) found, exit supl_glo_acqassit_svid_freq_num_map_aux()",
                0, 0, 0 );
    return;
  }


  MSG_HIGH("Glonass AuxiInfo num of list = %u", p_glo_aux->n, 0, 0);

  /* Glonass Acq Assistance isn't received ? */
  if (0 == p_glo_acqasst->gnss_sv_dir.u_NumSvs)
  {
    MSG_HIGH( "Glonass Acq Assistance isn't received yet. Glo SvId to FreqNum correlation deferred.", 0, 0, 0 );
    return;
  }
  else
  {
    /* Glonass Acq Assistance is received already */

    /* Ensure the total SV count for SV-Dir & Steering are same. This help avoid looping twice. */
    u_glo_num_svs = p_glo_acqasst->gnss_sv_dir.u_NumSvs;

    if (p_glo_acqasst->gnss_sv_dir.u_NumSvs != p_glo_acqasst->z_steering_glo_data.u_NumSvs)
    {
      if (p_glo_acqasst->gnss_sv_dir.u_NumSvs > p_glo_acqasst->z_steering_glo_data.u_NumSvs)
      {
        u_glo_num_svs = p_glo_acqasst->z_steering_glo_data.u_NumSvs;
      }

      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SV-Dir & Steering SV count differs %u, %u. Using only %u AA SVs",
            p_glo_acqasst->gnss_sv_dir.u_NumSvs, p_glo_acqasst->z_steering_glo_data.u_NumSvs, u_glo_num_svs);
    }

    p_gnss_glonass_sat_elem = p_glo_aux->elem;

    for ( u_index = 0; u_index < p_glo_aux->n; ++u_index, ++p_gnss_glonass_sat_elem )
    {

      if ( p_gnss_glonass_sat_elem == NULL )
      {
        MSG_ERROR("Unexpected NULL pointer p_gnss_glonass_sat_elem !!", 0, 0, 0);
        break;
      }

      if (  ( p_gnss_glonass_sat_elem->svID <= ( N_GLO_SV - 1 ) ) &&
            ( ( p_gnss_glonass_sat_elem->channelNumber >= C_PROT_GLO_FREQ_MIN ) &&
              ( p_gnss_glonass_sat_elem->channelNumber <= C_PROT_GLO_FREQ_MAX )
            )
         )
      {
        u_save_loop = 0;

        /* Loop through AA SVs & set the corresponding FreqNum */
        for ( u_sv_iter = 0; u_sv_iter < u_glo_num_svs; ++u_sv_iter )
        {

          // AA-SvDir
          // local w_Sv (65..88) & protocol p_gnss_glonass_sat_elem->svID (0..23)
          if (p_glo_acqasst->gnss_sv_dir.z_SvAzElInfo[u_sv_iter].w_Sv == (p_gnss_glonass_sat_elem->svID + GLO_SLOTID_OFFSET + 1))
          {
            p_glo_acqasst->u_freq_num_valid = TRUE;
            /* Per GLONASS ICD page 10, after year 2005, GLO SV would use frequency ID -7 to +6. Freq channel +7 to +13 would not be used. */
            p_glo_acqasst->gnss_sv_dir.z_SvAzElInfo[u_sv_iter].b_GloFreqIndex = p_gnss_glonass_sat_elem->channelNumber; /* the MGP API expects 5 bits */
            u_save_loop |= 0x1;
          }

          // AA-Steering
          // local w_Sv (65..88) & protocol p_gnss_glonass_sat_elem->svID (0..23)
          if (p_glo_acqasst->z_steering_glo_data.z_SvSteerInfo[u_sv_iter].w_Sv == (p_gnss_glonass_sat_elem->svID + GLO_SLOTID_OFFSET + 1))
          {
            p_glo_acqasst->u_freq_num_valid = TRUE;
            p_glo_acqasst->z_steering_glo_data.z_SvSteerInfo[u_sv_iter].b_GloFreqIndex = p_gnss_glonass_sat_elem->channelNumber;
            u_save_loop |= 0x2;
          }

          if ( u_save_loop == 3)
          {
            break; // break when both freqNum are set, else loop to max.
          }
        } /* saved GLONASS AA data for-loop */

        MSG_HIGH("Mapped SV_id=%u and channel#=%d. u_i=%u", p_gnss_glonass_sat_elem->svID,
                     p_gnss_glonass_sat_elem->channelNumber, u_index);

      } /* if this SV's aux. assistance data valid*/
    } /* aux. assistance data for-loop */
  } /* save GLONASS AA data available */

}



/*
 ******************************************************************************
 *
 * tm_umts_up_supl_select_pos_prot
 *
 * Function description:
 *  Selects positioning protocol depending on the phone serving system.
 *  If LTE, RRLP is chosen as the positioning protocol.
 *  If 1x,HRPD,eHRPD IS801 is chosen as positioning protocol.
 *
 * Parameters:
 *  Void
 *
 * Return value:
 *   Positioning protocol selected.
 *
 ******************************************************************************
*/

uint8 tm_umts_up_supl_select_pos_prot(void)
{
    tm_cm_phone_state_info_s_type phone_state_info;
    /* Query TM-CM module to get current acquired system */
    tm_cm_iface_get_phone_state(&phone_state_info);

  if(TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable) != 0)
  {
    switch(phone_state_info.srv_system)
    {
      case CGPS_SRV_SYS_CDMA:
      case CGPS_SRV_SYS_HDR:
        MSG_HIGH("IS801 selected as positioning protocol",0,0,0);
        return TM_POS_PRTL_IS801;
      case CGPS_SRV_SYS_WCDMA:
      case CGPS_SRV_SYS_GSM:
      case CGPS_SRV_SYS_GW:
      case CGPS_SRV_SYS_TDSCDMA:	
            return TM_POS_PRTL_RRLP;
      case CGPS_SRV_SYS_LTE:
        if ( tm_core_get_lte_lpp_up_enable() == TRUE )  // lpp check for NV item
        {
          MSG_HIGH("LPP selected as positioning protocol",0,0,0);
          return TM_POS_PRTL_LPP;
        }
        MSG_HIGH("RRLP selected as positioning protocol",0,0,0);
        return TM_POS_PRTL_RRLP;
      default:
          return TM_POS_PRTL_NONE;
    }
  }
  if ( phone_state_info.srv_system == CGPS_SRV_SYS_LTE )
  {
    if ( tm_core_get_lte_lpp_up_enable() == TRUE )
    {
      return TM_POS_PRTL_LPP;
    }
  }

  return TM_POS_PRTL_RRLP;
}

/*
 ******************************************************************************
 *
 * supl_start_is801_session
 *
 * Function description:
 *  Start IS801 session
 *
 * Parameters:
 *   None.
 *
 * Return value:
 *    TRUE or FALSE
 *
 ******************************************************************************
*/

boolean  supl_start_is801_session(boolean wake_tm_core, tm_sess_req_start_param_s_type  *actions)
{
  #if defined (FEATURE_CGPS_USES_CDMA) && !defined (FEATURE_GNSS_LOW_MEMORY)

    tm_sess_handle_type sess_handle;
    tm_1x_up_is801_drv_req_param_u_type  req_param;

  if(z_supl_session_status.u_agps_mode != C_SUPL_AGPS_MODE_CELL_ID)
  {
    MSG_HIGH("Starting IS801 session", 0,0,0);

    sess_handle = z_supl_session_status.tm_active_session_handle;
    req_param.start_param.wake_tm_core = wake_tm_core;
    req_param.start_param.actions      = actions;
    req_param.start_param.start_sess_req_param = &(z_supl_session_status.tm_sess_req_param);

      return tm_1x_up_is801_drv_req(
                                     sess_handle,
                                     TM_IS801_DRV_REQ_START,
                                     req_param
                                   );
  }
  else
  {
    return FALSE;
  }
  #else
  return FALSE; /* dummy for builds without 1X */

  #endif /* FEATURE_CGPS_USES_CDMA && ! FEATURE_GNSS_LOW_MEMORY */
}


/*
 ******************************************************************************
 *
 * supl_stop_is801_session
 *
 * Function description:
 *  Stop IS801 session
 *
 * Parameters:
 *   None.
 *
 * Return value:
 *    TRUE or FALSE
 *
 ******************************************************************************
*/

boolean  supl_stop_is801_session(const tm_sess_stop_reason_e_type stop_reason)
{

  #if defined (FEATURE_CGPS_USES_CDMA) && !defined (FEATURE_GNSS_LOW_MEMORY)

    tm_sess_handle_type sess_handle;
    tm_1x_up_is801_drv_req_param_u_type  req_param;

    MSG_HIGH("Stopping IS801 sessionn. stop reason %d xlated to (TM_STOP_REASON_GENERAL_ERROR)",
              stop_reason, 0,0);

    sess_handle = z_supl_session_status.tm_active_session_handle;
    req_param.stop_param.stop_reason = TM_STOP_REASON_GENERAL_ERROR;
    req_param.stop_param.stop_type   = PRTL_SESS_STOP_SINGLE_FIX;

    return tm_1x_up_is801_drv_req(  sess_handle,
                                  TM_IS801_DRV_REQ_STOP,
                                  req_param );

  #else
  return FALSE; /* dummy for builds without 1X */

  #endif  /* FEATURE_CGPS_USES_CDMA && ! FEATURE_GNSS_LOW_MEMORY */

}


/*
 ******************************************************************************
 *
 * tm_umts_up_supl_is801_session_stop
 *
 * Function description:
 *  Handling termination of IS801 session
 *
 * Parameters:
 *   stop_reason: Indicating the reason of termination
 *
 * Return value:
 *    void
 *
 ******************************************************************************
*/
void tm_umts_up_supl_is801_session_stop (const tm_sess_stop_reason_e_type   stop_reason)
{
  if(z_supl_session_status.u_position_protocol == TM_POS_PRTL_IS801)
  {
    MSG_MED("SUPL-IS801 Stopping Reason %d SUPL CF state %d", stop_reason, z_supl_session_status.e_cf_state, 0);

    if(stop_reason == TM_STOP_REASON_COMPLETED)
    {
      /*SUPL is waiting for IS801 session to stop*/
      if(!(z_supl_session_status.e_cf_state & C_SUPL_CF_STATE_UT3))
      {
        tm_umts_up_supl_inform_core_stop_session( TM_STOP_REASON_COMPLETED );
        tm_umts_up_supl_session_init(FALSE);
      }
    }
    else
    {
      /*Handle the error conditions here*/
      if( !(z_supl_session_status.e_cf_state & C_SUPL_CF_STATE_UT3))
      {
        if( z_pdcomm_tcp_info.curr_state == PDCOMM_STATE_CONNECTED &&
           (z_supl_session_status.u_send_supl_end_at_fail))
        {
          (void) tm_umts_up_supl_supl_end_tx(&z_supl_session_status.z_session_id.z_Slp_Sess_Id,
                                      NULL,
                                      supl_StatusCode_unspecified,
                                      NULL,
                                      z_supl_session_status.q_supl_version);
        }

        tm_umts_up_supl_inform_core_stop_session(stop_reason);
        tm_umts_up_supl_session_init(FALSE);
      }
    }
  }
  else
  {
    MSG_FATAL("IS801 is not the selected protocol. This event is unexpected",0,0,0);
  }
}

/*
 ******************************************************************************
 *
 * tm_umts_up_supl_is801_nt_assist_done
 *
 * Function description:
 *  Handling close connection from IS801
 *
 * Parameters:
 *    sess_end_sent: Whether IS801 sent session end.
 * Return value:
 *    void
 *
 ******************************************************************************
*/
void tm_umts_up_supl_is801_nt_assist_done (boolean sess_end_sent)
{
  if(z_supl_session_status.u_position_protocol == TM_POS_PRTL_IS801)
  {
    if(!sess_end_sent)
    {
      if ( z_pdcomm_tcp_info.curr_state == PDCOMM_STATE_CONNECTED &&
         (!(z_supl_session_status.e_cf_state & C_SUPL_CF_STATE_UT3)) &&
         (z_supl_session_status.u_send_supl_end_at_fail))
      {
        (void) tm_umts_up_supl_supl_end_tx(&z_supl_session_status.z_session_id.z_Slp_Sess_Id,
                                    NULL,
                                                                supl_StatusCode_unspecified,
                                                                NULL,
                                                                z_supl_session_status.q_supl_version);

        tm_umts_up_supl_terminate_timer_cancel_cf_state ( C_SUPL_CF_STATE_NULL );

        tm_umts_up_supl_comm_close_connection();
      }
    }
    else
    {
      /*Session end is sent, set the state to UT3*/
      supl_set_cf_state(C_SUPL_CF_STATE_UT3, 0);
    }
  }
  else
  {
    MSG_FATAL("IS801 is not the selected protocol. This even is unexpected",0,0,0);
  }
}

/*
 ******************************************************************************
 *
 * tm_umts_up_supl_is801_pos_reported
 *
 * Function description:
 *  Handling Reporting of LR from IS801
 *
 * Parameters:
 *    void
 * Return value:
 *    void
 *
 ******************************************************************************
*/
void tm_umts_up_supl_is801_pos_reported (void)
{
  if(z_supl_session_status.u_position_protocol == TM_POS_PRTL_IS801)
  {
    z_supl_session_status.u_pdapi_fix_reported = TRUE;
  }
  else
  {
    MSG_FATAL("IS801 is not the selected protocol. This even is unexpected",0,0,0);
  }
}

/*
 ******************************************************************************
 *
 * tm_umts_up_supl_is801_pos_reported
 *
 * Function description:
 *  Handling Reporting of LR from IS801
 *
 * Parameters:
 *    void
 * Return value:
 *    void
 *
 ******************************************************************************
*/
void tm_umts_up_supl_is801_inform_1x_pos_method (supl_session_agps_mode_enum_type mode)
{
  z_supl_session_status.u_agps_mode = mode;
}


/*
 ******************************************************************************
 *
 * tm_umts_up_supl_is801_pos_reported
 *
 * Function description:
 *  Handling Reporting of LR from IS801
 *
 * Parameters:
 *    void
 * Return value:
 *    void
 *
 ******************************************************************************
*/
static void check_and_handle_handover (boolean *abort_sending_next_msg)
{
  uint8 new_pos_protocol, old_pos_protocol;
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;
  tm_sess_req_param_u_type req_param;
  tm_sess_req_param_u_type *p_req_param = &req_param;

  new_pos_protocol = tm_umts_up_supl_select_pos_prot();
  old_pos_protocol = p_supl_session_status->u_position_protocol;

  if(new_pos_protocol != old_pos_protocol)
  {
    /*Handover has happened*/
    if(old_pos_protocol == TM_POS_PRTL_RRLP && new_pos_protocol == TM_POS_PRTL_IS801)
    {
      /*RRLP to IS801*/
      if( /*NI MS_BASED OR AUTO*/
         (p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_NI &&
           p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_BASED
          ) /*NI MS_BASED*/ ||
          p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_AUTO /*AUTO*/)
      {
        /*IS801 does not support NI MS-BASED, start MSA instead*/
        p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_ASSISTED;
        p_req_param->continue_param.op_req  = TM_OP_REQ_NONE;
        p_req_param->continue_param.op_mode  = TM_SESS_OPERATION_MODE_MSA;
        p_req_param->continue_param.dynamic_qos_enabled = TRUE;

        if ( !tm_sess_req ( TM_PRTL_TYPE_UMTS_UP,
                            p_supl_session_status->tm_active_session_handle,
                            TM_SESS_REQ_CONTINUE,
                            p_req_param
                          )
            )
        {
          tm_umts_up_supl_callflow_disrupt_handler();
          *abort_sending_next_msg = TRUE;
          return;
        }

      }
      else if( /*NI AFLT OR NI ECID OR UI AFLT*/
         (p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_NI &&
          p_supl_session_status->ni_pos_method == POSMETHOD_AFLT
          ) /*NI AFLT*/ ||
         (p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_NI &&
          p_supl_session_status->ni_pos_method == POSMETHOD_ECID
          ) /*NI ECID*/ ||
         (p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_UI &&
          p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED &&
          p_supl_session_status->tm_sess_req_param.pd_qos.gps_session_timeout == 0
          ) /*UI AFLT*/)
      {
        if(p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED)
        {
          p_supl_session_status->u_agps_mode = C_SUPL_AFLT_MODE;
          p_supl_session_status->tm_sess_req_param.pd_qos.gps_session_timeout = 0;
        }
      }
    } /*RRLP to IS801*/

    else if(old_pos_protocol == TM_POS_PRTL_IS801 && new_pos_protocol == TM_POS_PRTL_RRLP)
    {
      /*If NI MS_BASED OR NI MS_BASED Pref  set mode to MS-BASED mode*/
      if(p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_NI &&
         (p_supl_session_status->ni_pos_method == POSMETHOD_AGPS_SETBASED ||
          p_supl_session_status->ni_pos_method == POSMETHOD_AGPS_SETASSISTED_PREF)
         )
      {
        p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_BASED;
      }
      /*Else if NI AFLT OR UI AFLT OR NI ECID or NI EoTD and NI OTDOA set mode to MS-ASSISTED*/
      else if( /*NI AFLT OR  NI ECID OR NI EOTD OR NI OTDOA OR UI AFLT*/
              (p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_NI &&
               (p_supl_session_status->ni_pos_method == POSMETHOD_AFLT ||
                p_supl_session_status->ni_pos_method == POSMETHOD_ECID ||
                p_supl_session_status->ni_pos_method == POSMETHOD_EOTD ||
                p_supl_session_status->ni_pos_method == POSMETHOD_OTDOA)) /*NI AFLT OR  NI ECID OR NI EOTD OR NI OTDOA*/ ||
              (p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_UI &&
               p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED &&
               p_supl_session_status->tm_sess_req_param.pd_qos.gps_session_timeout == 0
               ) /*UI AFLT*/)
      {
        p_supl_session_status->u_agps_mode = C_SUPL_AGPS_MODE_UE_ASSISTED;
      }

      /*if MS-BASED start MS-BASED*/
      if(p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_BASED)
      {
        p_req_param->continue_param.op_mode  = TM_SESS_OPERATION_MODE_MSB;
        p_req_param->continue_param.op_req = TM_OP_REQ_AIDING_DATA_STATUS | TM_OP_REQ_LR;
        p_req_param->continue_param.lr_qos = TM_MAX_SESSION_TIMEOUT;
        *abort_sending_next_msg = TRUE;
      }
      /*else if AUTO*/
      else if(p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_AUTO)
      {
        p_req_param->continue_param.op_mode  = TM_SESS_OPERATION_MODE_MSB;
        p_req_param->continue_param.op_req =  TM_OP_REQ_LR;
        if(p_supl_session_status->u_callflow_kind == C_SUPL_SESSION_NI)
        {
          p_req_param->continue_param.lr_qos = TM_MAX_SESSION_TIMEOUT;
        }
        else
        {
          p_req_param->continue_param.lr_qos = p_supl_session_status->z_session_qos.u_gps_session_timeout_value_secs;
        }
      }
      /*else if MS-ASSISTED*/
      else if(p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED)
      {
        p_req_param->continue_param.op_mode  = TM_SESS_OPERATION_MODE_MSA;
        p_req_param->continue_param.op_req = TM_OP_REQ_PPM;
        p_req_param->continue_param.prm_qos = TM_MAX_SESSION_TIMEOUT;

        /* the wishlist setting is not absolutely needed as it will be set like that when forming
           SuplPosInit */
        p_supl_session_status->z_GpsAssistData.u_Octet3_Whishlist_AH |= C_G0931_GPS_ASSIST_REFTIME;
        p_supl_session_status->z_GpsAssistData.u_Octet3_Whishlist_AH |= C_G0931_GPS_ASSIST_ACQ;
      }


      p_req_param->continue_param.accuracy_threshold = p_supl_session_status->z_session_qos.q_hor_acc_meters;
      p_req_param->continue_param.num_fixes = p_supl_session_status->tm_sess_req_param.pd_option.fix_rate.num_fixes;
      p_req_param->continue_param.tbf_ms = p_supl_session_status->tm_sess_req_param.pd_option.fix_rate.time_between_fixes_ms;
      p_req_param->continue_param.dynamic_qos_enabled = TRUE;

      if ( !tm_sess_req ( TM_PRTL_TYPE_UMTS_UP,
                          p_supl_session_status->tm_active_session_handle,
                          TM_SESS_REQ_CONTINUE,
                          p_req_param
                        )
          )
      {
        tm_umts_up_supl_callflow_disrupt_handler();
        *abort_sending_next_msg = TRUE;
        return;
      }
    } /*IS801 to RRLP*/

    p_supl_session_status->u_position_protocol = new_pos_protocol;
  }
}

/*
 ******************************************************************************
 *
 * supl_comm_getnext_gm_transaction_id
 *
 * Function description:
 *  Get next transaction id to send request to Geofence module
 *
 * Parameters:
 *    void
 * Return value:
 *    void
 *
 ******************************************************************************
*/
uint32 supl_comm_getnext_gm_transaction_id(void)
{
    if ( (gm_transaction_id < TM_UMTS_UP_GM_TRANSACTION_ID_BASE ) ||
         ( gm_transaction_id >=
           ( TM_UMTS_UP_GM_TRANSACTION_ID_BASE + TM_UMTS_UP_GM_TRANSACTION_ID_RANGE )
         )
       )
    {
      gm_transaction_id = TM_UMTS_UP_GM_TRANSACTION_ID_BASE;
    }
    else
    {
      gm_transaction_id++;
    }
    return gm_transaction_id;
}



/*
 ******************************************************************************
 *
 * supl_SET_id_get
 *
 * Function description:
 *   Retrieve SET ID information.
 *
 * Parameters:
 *    void
 * Return value:
 *    void
 *
 ******************************************************************************
*/


static void supl_SET_id_get(void)
{
  boolean b_ret = TRUE;
  byte stored_imei[MAX_IMEI_LEN+1] = { 0 };
  static boolean b_set_id_imei = FALSE; 
  
  tm_cm_phone_state_info_s_type phone_state_info;

  memset((void*)&phone_state_info, 0, sizeof(phone_state_info));
  tm_cm_iface_get_phone_state(&phone_state_info);
  
  if ((b_set_id_imei == TRUE) &&
      (phone_state_info.e911_state == TM_CM_E911_STATE_NONE))
  {
    /* if we stored IMEI as IMSI, reset it as we are no longer in 911 state */
    memset((void*)&z_Set_Id, 0, sizeof(z_Set_Id));
    b_set_id_imei = FALSE;
  }
  #ifndef FEATURE_CGPS_SUPL_MSISDN_FIRST /* default: not defined, will use IMSI first */

  /* decide SET ID if not yet available to SUPL, try to retrieve IMSI */
  if ( z_Set_Id.set_id_choice == C_SETID_UNKNOWN )
  {
    z_Set_Id.set_id_choice = T_supl_SETId_imsi;
    if (z_umts_nv_item_ram_copy.set_imsi[0] ==  '\0'  )
    {
      /* IMSI value has not yet been read from the SIM*/
      if ( tm_umts_up_supl_comm_retrieve_imsi() == FALSE )
      {
        z_Set_Id.set_id_choice = C_SETID_UNKNOWN;
      }
    }

    /* Retrieving the copy of the IMSI stored in the Ram */
    if ( z_Set_Id.set_id_choice == T_supl_SETId_imsi )
    {
      memscpy(z_Set_Id.u.imsi.data, sizeof(z_Set_Id.u.imsi.data),
             &z_umts_nv_item_ram_copy.set_imsi[0], sizeof(z_umts_nv_item_ram_copy.set_imsi));
      z_Set_Id.u.imsi.numocts = MAX_IMSI_LEN_BYTES;
    }
  } /* close if SETID unknown */

  /* if still no SET_ID, try MSISDN */
  if ( z_Set_Id.set_id_choice == C_SETID_UNKNOWN )
  {
    uint16 etsime_cnum_msisdn_no_rec = 0;
    pbm_return_type pbm_return;
    pbm_device_type pb_id = PBM_MSISDN;
    pbm_record_s_type msisdn_data;
    uint8 u_i, u_j;


    memset(z_Set_Id.u.msisdn.data, 0xFF, C_SUPL_SETID_MSISDN_BYTE_LENGTH);

    etsime_cnum_msisdn_no_rec = pbm_get_num_recs(pb_id);
    if(etsime_cnum_msisdn_no_rec != 0)
    {
      /* there could possible be more than one number, but
           only the first will be used here */
      pbm_return = pbm_find_location(pb_id, 1, &msisdn_data, NULL);

      if(pbm_return == PBM_SUCCESS)
      {
        if ( msisdn_data.number[0] == '+' )
        {
            u_i = 1;
        }
        else
        {
            u_i = 0;
        }
        u_j = 0;

        while ( u_j < C_SUPL_SETID_MSISDN_BYTE_LENGTH )
        {
          if ( ( msisdn_data.number[u_i]>= 0x30 ) && ( msisdn_data.number[u_i]<= 0x39 ) )
          {
            z_Set_Id.u.msisdn.data[u_j] = msisdn_data.number[u_i] & 0x0F;

            u_i++;

            if ( ( msisdn_data.number[u_i]>= 0x30 ) && ( msisdn_data.number[u_i]<= 0x39 ) )
            {
              z_Set_Id.u.msisdn.data[u_j] |= msisdn_data.number[u_i] << 4;
              u_i++;
            }
            else
            {
              z_Set_Id.u.msisdn.data[u_j] |= 0xF0; /* fill unused with F */
              break;
            }
            u_j++;
          }
          else /*  terminate if not 0-9 ascii */
          {
            break;
          }
        };

        if ( u_i > 4 ) /* at least there are 4 valid characters */
        {
          z_Set_Id.set_id_choice = T_supl_SETId_msisdn;
          z_Set_Id.u.msisdn.numocts = C_SUPL_SETID_MSISDN_BYTE_LENGTH; /* SUPL dictates 8 octects */
        }
      } /* if pbm return success */
    } /* num of rec. for ISDN not zero */
  } /* if still no SET_ID */


  #else  /************* Special feature: use MSISDN first *************/

  /* if no SET_ID, try MSISDN first  */
  if ( z_Set_Id.set_id_choice == C_SETID_UNKNOWN )
  {
    uint16 etsime_cnum_msisdn_no_rec = 0;
    pbm_return_type pbm_return;
    pbm_device_type pb_id = PBM_MSISDN;
    pbm_record_s_type msisdn_data;
    uint8 u_i, u_j;


    memset(z_Set_Id.u.msisdn.data, 0xFF, C_SUPL_SETID_MSISDN_BYTE_LENGTH);

    etsime_cnum_msisdn_no_rec = pbm_get_num_recs(pb_id);
    if(etsime_cnum_msisdn_no_rec != 0)
    {
      /* there could possible be more than one number, but
         only the first will be used here */
      pbm_return = pbm_find_location(pb_id, 1, &msisdn_data, NULL);

      if(pbm_return == PBM_SUCCESS)
      {
        if ( msisdn_data.number[0] == '+' )
        {
          u_i = 1;
        }
        else
        {
          u_i = 0;
        }
        u_j = 0;

        while ( u_j < C_SUPL_SETID_MSISDN_BYTE_LENGTH )
        {
          if ( ( msisdn_data.number[u_i]>= 0x30 ) && ( msisdn_data.number[u_i]<= 0x39 ) )
          {
            z_Set_Id.u.msisdn.data[u_j] = msisdn_data.number[u_i] & 0x0F;

            u_i++;

            if ( ( msisdn_data.number[u_i]>= 0x30 ) && ( msisdn_data.number[u_i]<= 0x39 ) )
            {
              z_Set_Id.u.msisdn.data[u_j] |= msisdn_data.number[u_i] << 4;
              u_i++;
            }
            else
            {
              z_Set_Id.u.msisdn.data[u_j]|= 0xF0; /* fill unused with F */
              break;
            }
            u_j++;
          }

          else /* terminate if not 0-9 ascii */
          {
            break;
          }
        };

        if ( u_i > 4 ) /* at least there are 4 valid characters */
        {
          z_Set_Id.set_id_choice = C_SUPL_ASN1_CHOICE_SETID_MSISDN;
          z_Set_Id.u.msisdn.numocts = C_SUPL_SETID_MSISDN_BYTE_LENGTH; /* SUPL dictates 8 octects */
        }
      } /* if pbm return success */
    } /* num of rec. for ISDN not zero */
  } /* if no SET_ID */

  /* Still no SET_ID available to SUPL, try to retrieve IMSI */
  if ( z_Set_Id.set_id_choice == C_SETID_UNKNOWN )
  {
    z_Set_Id.set_id_choice = T_supl_SETId_imsi;
    if (z_umts_nv_item_ram_copy.set_imsi[0] ==  '\0'  )
    {
        /* IMSI value has not yet been read from the SIM*/
        if ( tm_umts_up_supl_comm_retrieve_imsi() == FALSE )
        {
          z_Set_Id.set_id_choice = C_SETID_UNKNOWN;
        }
    }
    /* Retrieving the copy of the IMSI stored in the Ram */
    if ( z_Set_Id.set_id_choice == T_supl_SETId_imsi )
    {
      z_Set_Id.u.imsi.numocts = memscpy(z_Set_Id.u.imsi.data, sizeof(z_Set_Id.u.imsi.data),
             &z_umts_nv_item_ram_copy.set_imsi[0], sizeof(z_umts_nv_item_ram_copy.set_imsi);
    }
  } /* close if SETID unknown */

  #endif /* use MSIDSN first */


  /* if still no SET_ID, check if IMEI is available and use that in leiu of IMSI for E911 */
  if ((z_Set_Id.set_id_choice == C_SETID_UNKNOWN) &&
      (phone_state_info.e911_state != TM_CM_E911_STATE_NONE))
  {
    b_ret = tm_get_imei(stored_imei);
    if (b_ret == TRUE  && stored_imei[0] == MAX_IMEI_LEN)
    {
      z_Set_Id.set_id_choice = T_supl_SETId_imsi;
      /* imei[0] is the length  */
      memscpy(z_Set_Id.u.imsi.data, MAX_IMEI_LEN,
              &stored_imei[1], MAX_IMEI_LEN);
      z_Set_Id.u.imsi.numocts = MAX_IMEI_LEN;
      MSG_HIGH("SUPL SET_id Get IMEI succeeded", 0, 0, 0);
      b_set_id_imei = TRUE;
    }
  }

  #if (defined(T_WINNT) && defined(SMART_OSYS))
  #error code not present
#endif

}

/*
 ******************************************************************************
 *
 * supl_SLP_session_id_match_check
 *
 * Function description:
 *   Determine if the incoming message has matching SLP session ID with that of
 *   a cached copy.
 *
 * Parameters:
 *   p_cached_Slp_Sess_Id : pointer to the cached copy of the SLP session ID.
 *   p_incoming_Slp_Sess_Id: pointer to the incoming message's SLP session ID.
 *
 * Return value:
 *   TRUE: SLP session IDs match; FALSE: SLP session IDs do not match.
 *
 ******************************************************************************
*/


static boolean supl_SLP_session_id_match_check
( supl_slp_session_id *p_cached_Slp_Sess_Id, supl_slp_session_id *p_incoming_Slp_Sess_Id )
{
  boolean  u_session_id_consistent = TRUE;

  if ( ( p_cached_Slp_Sess_Id->presence == TRUE ) &&
       ( p_incoming_Slp_Sess_Id->presence == TRUE )
     )
  {
    /* Session ID */
    if ( memcmp((unsigned char *)p_cached_Slp_Sess_Id->session_Id,
          (unsigned char *)p_incoming_Slp_Sess_Id->session_Id,
          C_SUPL_SLP_SESSION_ID_BYTE_LENGTH ) != 0 )
    {
      u_session_id_consistent = FALSE;
    }

    /* SLP ID */
    if ( p_incoming_Slp_Sess_Id->zSlpAddress.choice == C_SUPL_ASN1_CHOICE_SLPADDRESS_IPADDRESS )
    {
      /* IP Address */
      if ( p_incoming_Slp_Sess_Id->zSlpAddress.u.z_Ip_Address.choice == C_SUPL_ASN1_CHOICE_IPV4ADDRESS )
      {
        /* IPV4 */
        p_cached_Slp_Sess_Id->zSlpAddress.u.z_Ip_Address.u.Ip_V4.numocts = 4;
        if ( memcmp( p_cached_Slp_Sess_Id->zSlpAddress.u.z_Ip_Address.u.Ip_V4.data,
                        p_incoming_Slp_Sess_Id->zSlpAddress.u.z_Ip_Address.u.Ip_V4.data,
                        4 ) ||
             ( p_cached_Slp_Sess_Id->zSlpAddress.choice != C_SUPL_ASN1_CHOICE_SLPADDRESS_IPADDRESS ) ||
             ( p_cached_Slp_Sess_Id->zSlpAddress.u.z_Ip_Address.choice != C_SUPL_ASN1_CHOICE_IPV4ADDRESS )
           )
        {
          u_session_id_consistent = FALSE;
        }
      }
      else
      {
        /* IPV6 */
        p_cached_Slp_Sess_Id->zSlpAddress.u.z_Ip_Address.u.Ip_V6.numocts = 16;
        if ( memcmp( p_cached_Slp_Sess_Id->zSlpAddress.u.z_Ip_Address.u.Ip_V6.data,
                      p_incoming_Slp_Sess_Id->zSlpAddress.u.z_Ip_Address.u.Ip_V6.data,
                      16 ) ||
             ( p_cached_Slp_Sess_Id->zSlpAddress.choice != C_SUPL_ASN1_CHOICE_SLPADDRESS_IPADDRESS ) ||
             ( p_cached_Slp_Sess_Id->zSlpAddress.u.z_Ip_Address.choice != C_SUPL_ASN1_CHOICE_IPV6ADDRESS )
           )
        {
          u_session_id_consistent = FALSE;
        }
      }
    }
    else
    {
      uint8 fqdn_len = p_cached_Slp_Sess_Id->zSlpAddress.u.z_fqdn.u_length;

      /* FQDN */
      if ( ( fqdn_len != p_incoming_Slp_Sess_Id->zSlpAddress.u.z_fqdn.u_length ) ||
           memcmp( p_cached_Slp_Sess_Id->zSlpAddress.u.z_fqdn.fqdn,
                      p_incoming_Slp_Sess_Id->zSlpAddress.u.z_fqdn.fqdn,
                      fqdn_len ) ||
           ( p_cached_Slp_Sess_Id->zSlpAddress.choice != C_SUPL_ASN1_CHOICE_SLPADRESS_FQDN )
         )
      {
        u_session_id_consistent = FALSE;
      }
    }

  }

  return  u_session_id_consistent;

}


/*
 ******************************************************************************
 *
 * supl_init_trigger_event_check
 *
 * Function description:
 *   Determine if event trigger session is requested.
 *
 * Parameters:
 *    void
 * Return value:
 *    Value of the supl_init_trigger_type_enum_type (NULL, areaEvent, periodic
 *    or wrong type indication ).
 *
 ******************************************************************************
*/


static supl_init_trigger_type_enum_type supl_init_trigger_event_check (void)
{

  if ( ( q_supl_ver_maj_after_supl_init_glob & 0x000000FF ) < 2 )
  {
    return C_SUPL_INIT_TRIG_TYPE_NULL;
  }

  if ( z_supl_init_trigger_type.trigger_type_present )
  {
    if ( z_supl_init_trigger_type.q_trigger_type == 1 )
    {
      return C_SUPL_INIT_TRIG_TYPE_AREA_EVENT ;
    }
    else if ( z_supl_init_trigger_type.q_trigger_type == 0  )
    {
      return C_SUPL_INIT_TRIG_TYPE_PERIODIC ;
    }
    else
    {
      return C_SUPL_INIT_TRIG_TYPE_WRONG_DEF;
    }
  }
  else
  {
    return C_SUPL_INIT_TRIG_TYPE_NULL;
  }

}


/*
 ******************************************************************************
 *
 * supl_init_trigger_event_proc
 *
 * Function description:
 *   Process trigger event from SUPL_INIT.
 *
 * Parameters:
 *    p_req_data: pointer to pdapi client provided data including info. from
 *                SUPL_INIT.
 *    p_slp_id:   pointer to SLP ID.
 *
 * Return value:
 *    None.
 *
 ******************************************************************************
*/


static void  supl_init_trigger_event_proc
( pdsm_lcs_supl_notify_verify_req_s_type *p_req_data,
  supl_slp_session_id                    *p_slp_id,
  tm_cm_phone_state_info_s_type phone_state_info
)
{

  uint8 u_i, u_session_num;

  /* for now only MSB supported */
  if ( !( ( p_req_data->pos_method == POSMETHOD_AGPS_SETBASED ) ||
           ( p_req_data->pos_method == POSMETHOD_AGPS_SETBASED_PREF )
        )
     )
  {

    MSG_HIGH ("Unsupported pos_mothod for areaTriggerEvent: %u",
               p_req_data->pos_method, 0, 0);

    (void) tm_umts_up_supl_supl_end_prepare_or_tx( p_slp_id,
                                            NULL,
                                            supl_StatusCode_posMethodMismatch,
                                            p_req_data->supl_hash,
                                            q_supl_ver_maj_after_supl_init_glob );
    return;
  }

  for ( u_i=0; u_i < C_MAX_GEOFENCE_NUM; u_i++)
  {
    z_supl_area_event_session_status[u_i].u_position_protocol = tm_umts_up_supl_select_pos_prot();
    /* find an open slot for the session */
    if ( z_supl_area_event_session_status[u_i].u_session_active == FALSE )
    {
      /* slot found, take this slot */
      z_supl_area_event_session_status[u_i].u_session_active = TRUE;


      if ( ( p_req_data->pos_method == POSMETHOD_AGPS_SETBASED ) ||
           ( p_req_data->pos_method == POSMETHOD_AGPS_SETBASED_PREF )
        )
      {

          /*NI MS BASED only if MS-BASED supported and RRLP is used as pos protocol*/
        if ( TM_CONFIG_POS_MODE_BIT_IS_SET( PDSM_POS_MODE_UP_MS_BASED | PDSM_POS_MODE_4G_UP_MS_BASED ) != 0 )
        {
          z_supl_area_event_session_status[u_i].u_agps_mode =
            C_SUPL_AGPS_MODE_UE_BASED;
        }
      }

      memscpy( z_supl_session_status.supl_init_hash, sizeof(z_supl_session_status.supl_init_hash),
               p_req_data->supl_hash, sizeof(p_req_data->supl_hash) );
      z_supl_session_status.u_hash_valid = TRUE;

      MSG_MED("SUPL_INIT takes %u slot for areaEvent session", u_i, 0, 0);
      break;
    }
  }

  if ( u_i >= C_MAX_GEOFENCE_NUM )
  {
    MSG_HIGH ("Exceeding number of areaTriggerEvent session : %u",
               u_i, 0, 0);

    (void) tm_umts_up_supl_supl_end_prepare_or_tx( p_slp_id,
                                            NULL,
                                            supl_StatusCode_resourceShortage,
                                            p_req_data->supl_hash,
                                            q_supl_ver_maj_after_supl_init_glob );
    return;
  }

  /************** store SLP session ID ************************************/

  /** for later use **/
  z_supl_area_event_session_status[u_i].z_session_id.z_Slp_Sess_Id = *p_slp_id;
  z_supl_area_event_session_status[u_i].q_supl_version = q_supl_ver_maj_after_supl_init_glob;

  /* for the immediate use of processing SLP messages */
  z_supl_session_status.z_session_id.z_Slp_Sess_Id = *p_slp_id;
  z_supl_session_status.q_supl_version = q_supl_ver_maj_after_supl_init_glob;

  q_supl_ver_maj_after_supl_init_glob = 0;
  u_session_num = u_i;

  #ifdef FEATURE_USER_PLANE_MRL
  #ifdef FEATURE_CGPS_WCDMA
  if ( phone_state_info.srv_system == CGPS_SRV_SYS_WCDMA )
  {
    tm_umts_up_supl_send_wcdma_cell_meas_req();
    (void) os_TimerStart(l1_mrl_req_timer, 1000, NULL);
  }
  #endif /* FEATURE_CGPS_WCDMA */
  #ifdef FEATURE_CGPS_LTE_CELLDB
  if ( phone_state_info.srv_system == CGPS_SRV_SYS_LTE )
  {
    lte_ml1_request_gps_mrl();
    (void) os_TimerStart(l1_mrl_req_timer, 1000, NULL);
  }
  #endif
  #endif /* MRL */


  /* check if cached cell ID is valid and not stale: if not, request new
     cell ID info. from L1 */
  if ( tm_umts_supl_if_need_forced_wcmda_cell_inq() == FALSE )
  {
    /* cell info. valid and current, start SLP contact */
    MSG_MED("start SLP contact",0,0,0);
    /* Initializing the flags so that we can attempt a fresh
       connection*/
    z_pdcomm_tcp_info.tls_session_authentication_failure = FALSE;
    z_pdcomm_tcp_info.used_auto_configured_hslp_address = FALSE;
    tm_umts_up_supl_start_slp_contact_area_event(u_session_num);
  }
  else
  {
    if ( !tm_umts_up_supl_req_cell_info_from_rrc(C_SUPL_CF_STATE_CELL_INFO_REQ_AREA_EVENT) )
    {
     /* Initializing the flags so that we can attempt a fresh
        connection*/
     z_pdcomm_tcp_info.tls_session_authentication_failure = FALSE;
     z_pdcomm_tcp_info.used_auto_configured_hslp_address = FALSE;
      /* effort to update cell info. fails, go ahead anyway hoping new
         update may come before it's ready to send the message */
      tm_umts_up_supl_start_slp_contact_area_event(u_session_num);
    }
    else /* not contacting SLP yet, give RRC chance to provide CELL info */
    {
      z_supl_area_event_session_status[u_session_num].q_slp_contact_pending |=
        C_AREA_EVENT_PENDING_SLP_CONTACT_TRIIGER_START;
    }
  }
}


/*
 ******************************************************************************
 *
 * tm_umts_up_supl_start_slp_contact_area_event
 *
 * Function description:
 *  Actions upon tcp connection is established for the use of area event
 *  sessions.
 *
 * Parameters:
 *   u_session_num: index to the active area event session.
 *                  0xFF: no knowledge of which area event session needing SLP
 *                        at the time of calling this function.
 *
 * Return value:
 *    None.
 *
 ******************************************************************************
*/


void tm_umts_up_supl_start_slp_contact_area_event(uint8 u_session_num)
{
  pd_comms_return_type          pdcomm_connect_ret_value;

  tm_cm_phone_state_info_s_type phone_state_info;
  uint8  u_i;

  if ( u_session_num == 0xFF )
  {
    /* need to search for the relevant areaEvent session */
    for (u_i=0; u_i< C_MAX_GEOFENCE_NUM; u_i++)
    {
      if ( z_supl_area_event_session_status[u_i].q_slp_contact_pending &
           C_AREA_EVENT_PENDING_SLP_CONTACT_TRIIGER_START )
      {
        u_session_num = u_i;
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED,
              "Identified AreaEventSession %u for SLP contact start",
              u_session_num);
        break;
      }
    }
  }

  if ( u_session_num >= C_MAX_GEOFENCE_NUM )
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,
          "Invalid areaEvent session num: %u",
          u_session_num);
    return;
  }

  tm_cm_iface_get_phone_state(&phone_state_info);

  pdcomm_connect_ret_value = tm_umts_up_supl_comm_tcp_connect( SUPL_VERSION_MAIN_2 );

  if ( PDCOMM_RESULT_OK == pdcomm_connect_ret_value ) /* Already got connected */
  {
    /* Can't run SUPL over LTE if SUPL major version less than 2,
       even though previous negotiation with SUPL_INIT has resulted
       in running SUPL 1.0 */
    if ( (phone_state_info.srv_system == CGPS_SRV_SYS_LTE ) &&
         ( (z_supl_area_event_session_status[u_session_num].q_supl_version & 0xFF) < 2 )
       )
    {
      /* send SUPL_END, using the provisioned SUPL version in its
         message header, and abort this session */

      z_supl_area_event_session_status[u_session_num].q_supl_version =
       ( ( z_umts_nv_item_ram_copy.gnss_supl_version >> 16) & 0x000000FF ) |
       ( z_umts_nv_item_ram_copy.gnss_supl_version & 0x0000FF00 ) |
       ( ( z_umts_nv_item_ram_copy.gnss_supl_version << 16 ) & 0x00FF0000 );

      (void) tm_umts_up_supl_supl_end_tx(
               &z_supl_area_event_session_status[u_session_num].z_session_id.z_Slp_Sess_Id,
               NULL,
               supl_versionNotSupported,
               &z_supl_area_event_session_status[u_session_num].supl_init_hash[0],
               z_supl_area_event_session_status[u_session_num].q_supl_version);

      tm_umts_up_supl_comm_close_connection();
      return;
    }

    /* send SUPL_TRIGGER_START */



  } /*Already got connected*/

  else if ( PDCOMM_RESULT_WAIT == pdcomm_connect_ret_value ) /* most likely */
  {
      supl_set_cf_state_area_event(C_SUPL_CF_STATE_UT0_SUPLTRIGGERSTART, u_session_num);
      MSG_MED("Starting UT0 for SUPL_TriggerStart", 0, 0, 0);
  }

  else
  {
    MSG_MED("SUPL: TCP conn. failed", 0, 0, 0);
  }
}


/*
 ******************************************************************************
 *
 * supl_set_cf_state_area_event
 *
 * Function description:
 *   Area event related session state handling.
 *
 * Parameters:
 *   u_session_num: index to the active area event session.
 *
 * Return value:
 *    None.
 *
 ******************************************************************************
*/


static void supl_set_cf_state_area_event
(supl_cf_state_enum_type e_new_supl_cf_state, uint8 u_session_num )
{

  if ( ( e_new_supl_cf_state == C_SUPL_CF_STATE_NULL ) ||
       ( u_session_num >= C_MAX_GEOFENCE_NUM )
     )
    return;

  (void) os_TimerStop( supl_cf_timer ); /* stop any current running timer */

  if ( ( e_new_supl_cf_state & C_SUPL_CF_STATE_UT0_SUPLTRIGGERSTART ) ||
       ( e_new_supl_cf_state & C_SUPL_CF_STATE_UT0_SUPLREPORT_GF_BREACH )
     )
  {
    /* "UT0": waiting for TLS link to establish  */
    (void) os_TimerStart(supl_cf_timer, SUPL_TIMER_UT0_VALUE_MSEC, NULL);
  }
  else if ( e_new_supl_cf_state & C_SUPL_CF_STATE_UT1_SUPLTRIGGERRESPONSE )
  {
    (void) os_TimerStart(supl_cf_timer, z_umts_nv_item_ram_copy.aagps_ut1_value * 1000, NULL);
  }
  else if ( e_new_supl_cf_state & C_SUPL_CF_STATE_UT3_LAST_SUPLREPORT )
  {
    (void) os_TimerStart(supl_cf_timer, z_umts_nv_item_ram_copy.aagps_ut3_value * 1000, NULL);
  }
  else
  {
    MSG_HIGH("Unexpected areaEvent cf_state: %u", e_new_supl_cf_state, 0, 0);
  }

  z_supl_area_event_session_status[u_session_num].e_cf_state |= e_new_supl_cf_state;
  MSG_MED( "SUPL CF State: 0x%x", z_supl_area_event_session_status[u_session_num].e_cf_state, 0, 0);

}


/*
 ******************************************************************************
 *
 * supl_is_in_any_ut0
 *
 * Function description:
 *   Check if it is waiting for data connection to be established.
 *
 * Parameters:
 *   None.
 *
 * Return value:
 *   TRUE: yes it is waiting for data connection to be established.
 *   FALSE: no it is not waiting.
 *
 ******************************************************************************
*/

static boolean supl_is_in_any_ut0 (void)
{
  uint8 u_i;
  supl_status_s_type*    p_supl_session_status = &z_supl_session_status;
  boolean u_ret = FALSE;

  if ( ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLSTART ) ||
       ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLPOSINIT ) ||
       ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLEND ) ||
       ( p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_UT0_SUPLREPORT_QUERY )
     )
  {
    return TRUE;
  }

  for ( u_i = 0; u_i < C_MAX_GEOFENCE_NUM; u_i++)
  {
    if ( z_supl_area_event_session_status[u_i].u_session_active )
    {

      if ( ( z_supl_area_event_session_status[u_i].e_cf_state &
             C_SUPL_CF_STATE_UT0_SUPLTRIGGERSTART ) ||
           ( z_supl_area_event_session_status[u_i].e_cf_state &
             C_SUPL_CF_STATE_UT0_SUPLTRIGSTOP ) ||
           ( z_supl_area_event_session_status[u_i].e_cf_state &
             C_SUPL_CF_STATE_UT0_SUPLREPORT_GF_BREACH )
         )
      {
        u_ret = TRUE;
        break;
      }
    }
  }

  return u_ret;

}


/*
 ******************************************************************************
 *
 * tm_umts_up_supl_terminate_timer_cancel_cf_state_area_event
 *
 * Function description:
 *   Area event related session timer termination.
 *
 * Parameters:
 *   u_session_num: index to the active area event session.
 *
 * Return value:
 *    None.
 *
 ******************************************************************************
*/


void tm_umts_up_supl_terminate_timer_cancel_cf_state_area_event
( supl_cf_state_enum_type      e_timer_state_to_terminate,
  uint8                        u_session_num )
{

  (void) os_TimerStop( supl_cf_timer );

  if ( u_session_num >= C_MAX_GEOFENCE_NUM )
    return;

  if ( e_timer_state_to_terminate != C_SUPL_CF_STATE_NULL )
  {
    z_supl_area_event_session_status[u_session_num].e_cf_state &=
      (supl_cf_state_enum_type)~e_timer_state_to_terminate;
  }
  else
  {
    z_supl_area_event_session_status[u_session_num].e_cf_state =
      C_SUPL_CF_STATE_NULL;
  }
}


/*
 ******************************************************************************
 *
 * supl_SuplTriggerStart_tx
 *
 * Function description:
 *   Formulating and sending of SUPL_TRIGGER_START.
 *
 * Parameters:
 *   u_session_num: index to the active area event session.
 *
 * Return value:
 *    None.
 *
 ******************************************************************************
*/


static boolean supl_SuplTriggerStart_tx (uint8 u_session_num)
{

  tm_pdu_buf_type pdu_buff; /* including length and pointer to data */
  supl_ULP_PDU      pdu;

  supl_Ver2_SUPLTRIGGEREDSTART*   p_SuplTriggerStart;
  supl_Ver2_SUPLTRIGGEREDSTART    z_SuplTriggerStart;
  supl_SETCapabilities            z_sETCapabilities;
  supl_SETCapabilities*           p_sETCapabilities;
  supl_ServiceCapabilities*       p_service_capabilities;

  supl_cell_info_struct_type  z_cell_info_allocated;

  uint8 u_num_mlid = 0;
  supl_LocationIdData *p_LocationIdData = NULL;
  supl_LocationId     *p_LocationId = NULL;
  supl_cell_info_struct_type  *p_cell_info_allocated_mlid = NULL;

  int                         encode_status;

  uint8*           p_length;
  uint8            u_length_msb = 0;
  uint8            u_length_lsb = 0;

  boolean          u_ret  = FALSE;

  if ( u_session_num >= C_MAX_GEOFENCE_NUM )
    return FALSE;

  memset(&pdu, 0x00, sizeof(supl_ULP_PDU));
  memset(&z_SuplTriggerStart, 0x00, sizeof(supl_Ver2_SUPLTRIGGEREDSTART));
  memset(&z_sETCapabilities, 0x00, sizeof(supl_SETCapabilities) );
  memset(&pdu_buff, 0, sizeof(tm_pdu_buf_type));

  p_SuplTriggerStart = &z_SuplTriggerStart;
  pdu.message.t = C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLTRIGSTART;
  pdu.message.u.msSUPLTRIGGEREDSTART = p_SuplTriggerStart;

  p_sETCapabilities = &z_sETCapabilities;
  z_SuplTriggerStart.sETCapabilities = p_sETCapabilities;

  /* overhead build function has to be after we know what message is to be constructed */
  z_supl_session_status.z_session_id.z_Set_Sess_Id.w_Session_Id = w_SetSessionID;
  z_supl_area_event_session_status[u_session_num].z_session_id.z_Set_Sess_Id.w_Session_Id = w_SetSessionID;

  supl_SET_id_get();
  memscpy( &z_supl_area_event_session_status[u_session_num].z_session_id.z_Set_Sess_Id.z_Set_Id,
          sizeof(z_supl_area_event_session_status[u_session_num].z_session_id.z_Set_Sess_Id.z_Set_Id),
          &z_Set_Id, sizeof(z_Set_Id)
        );

  w_SetSessionID++;

  if ( !supl_header_build ( &pdu,
                            &z_supl_area_event_session_status[u_session_num].z_session_id.z_Set_Sess_Id.w_Session_Id,
                            NULL,
                            &z_supl_area_event_session_status[u_session_num].z_session_id.z_Slp_Sess_Id,
                            z_supl_area_event_session_status[u_session_num].q_supl_version )
     )
  {
    MSG_ERROR("SUPL: SuplStart attempt aborted: header", 0, 0, 0);
    return FALSE;
  }


  /* Positioning technology */
  p_sETCapabilities->posTechnology.aFLT = FALSE;
  p_sETCapabilities->posTechnology.eOTD = FALSE;
  p_sETCapabilities->posTechnology.oTDOA = FALSE;
  p_sETCapabilities->posTechnology.eCID = FALSE; /* maybe modified below */


  if ( z_supl_area_event_session_status[u_session_num].u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED )
  {
      p_sETCapabilities->posTechnology.agpsSETassisted = TRUE;

      if(z_supl_area_event_session_status[u_session_num].u_position_protocol == TM_POS_PRTL_LPP)
      {
        p_sETCapabilities->posTechnology.oTDOA = TRUE;
  }
  }

  else if(z_supl_area_event_session_status[u_session_num].u_agps_mode == C_SUPL_AFLT_MODE)
  {
    if(z_supl_area_event_session_status[u_session_num].u_position_protocol == TM_POS_PRTL_IS801)
    {
      p_sETCapabilities->posTechnology.aFLT = TRUE;
    }
  }

  else if ( z_supl_area_event_session_status[u_session_num].u_agps_mode == C_SUPL_AGPS_MODE_UE_BASED )
  {
    p_sETCapabilities->posTechnology.agpsSETBased = TRUE;

    /*IS801 does not support Autonomous GPS mode*/
    if(z_supl_area_event_session_status[u_session_num].u_position_protocol != TM_POS_PRTL_IS801)
    {
      #ifndef FEATURE_CGPS_SUPL_VARIANT_SET_CAP_NO_AUTO /* NOT by default defined */
      p_sETCapabilities->posTechnology.autonomousGPS = TRUE;
      #endif
    }

  }
  else if (z_supl_area_event_session_status[u_session_num].u_agps_mode == C_SUPL_AGPS_MODE_CELL_ID)
  {
     p_sETCapabilities->posTechnology.agpsSETassisted = FALSE;
     p_sETCapabilities->posTechnology.autonomousGPS = FALSE;
     p_sETCapabilities->posTechnology.agpsSETBased = FALSE;
  }

  else if ( z_supl_area_event_session_status[u_session_num].u_agps_mode == C_SUPL_OTDOA_MODE )
  {
      p_sETCapabilities->posTechnology.oTDOA = TRUE;
  }
  else
  {
    /* no other agps mode should generate SUPL_POS_INIT */
    MSG_ERROR("SuplTriggerStart_tx attempt aborted", 0, 0, 0);
    return FALSE;
  }

  p_sETCapabilities->prefMethod = supl_PrefMethod_noPreference;

  #ifdef FEATURE_CGPS_SUPL_VARIANT_SET_CAP_PREF_SET_BASED  /* NOT by defaut defined */
  #error code not present
#endif


  if(z_supl_area_event_session_status[u_session_num].u_position_protocol == TM_POS_PRTL_LPP)
  {
    p_sETCapabilities->posProtocol.m.ver2_PosProtocol_extensionPresent = 1;
    p_sETCapabilities->posProtocol.ver2_PosProtocol_extension.lpp = TRUE;

    p_sETCapabilities->posProtocol.ver2_PosProtocol_extension.m.posProtocolVersionLPPPresent = 1;
    p_sETCapabilities->posProtocol.ver2_PosProtocol_extension.posProtocolVersionLPP.majorVersionField = LPP_CURRENT_VERSION_MAJOR;
    p_sETCapabilities->posProtocol.ver2_PosProtocol_extension.posProtocolVersionLPP.technicalVersionField = LPP_CURRENT_VERSION_TECH;
    p_sETCapabilities->posProtocol.ver2_PosProtocol_extension.posProtocolVersionLPP.editorialVersionField = LPP_CURRENT_VERSION_EDIT;
  }
  else
  {
    p_sETCapabilities->posProtocol.rrlp = TRUE;
        /***********  PosProtocolVersionRRLP SUPL2 extension in PosProtocol ****/

    p_sETCapabilities->posProtocol.m.ver2_PosProtocol_extensionPresent = 1;

    p_sETCapabilities->posProtocol.ver2_PosProtocol_extension.m.posProtocolVersionRRLPPresent = 1;

    /* default to the backward compatible version first, may be updated below */
    p_sETCapabilities->posProtocol.ver2_PosProtocol_extension.posProtocolVersionRRLP.majorVersionField =
      SUPL_RRLP_BACK_VERSION_MAJOR;
    p_sETCapabilities->posProtocol.ver2_PosProtocol_extension.posProtocolVersionRRLP.technicalVersionField =
      SUPL_RRLP_BACK_VERSION_TECH;
    p_sETCapabilities->posProtocol.ver2_PosProtocol_extension.posProtocolVersionRRLP.editorialVersionField =
      SUPL_RRLP_BACK_VERSION_EDIT;
  }
  /* positioning protocol */

  p_sETCapabilities->m.ver2_SETCapabilities_extensionPresent = 1;

  p_sETCapabilities->ver2_SETCapabilities_extension.m.serviceCapabilitiesPresent = 1;

  p_service_capabilities = &p_sETCapabilities->ver2_SETCapabilities_extension.serviceCapabilities;

  p_service_capabilities->servicesSupported.areaEventTrigger = 1;

  p_service_capabilities->m.eventTriggerCapabilitiesPresent = 1;

  p_service_capabilities->eventTriggerCapabilities.m.maxAreaIdListSupportedPresent = 1;
  p_service_capabilities->eventTriggerCapabilities.maxAreaIdListSupported = C_MAX_NUM_AREA_ID_LIST; /* 32 */

  p_service_capabilities->eventTriggerCapabilities.m.maxAreaIdSupportedPerListPresent = 1;
  p_service_capabilities->eventTriggerCapabilities.maxAreaIdSupportedPerList = C_MAX_NUM_AREA_ID_PER_LIST; /* 256 */

  p_service_capabilities->eventTriggerCapabilities.m.maxNumGeoAreaSupportedPresent = 1;
  p_service_capabilities->eventTriggerCapabilities.maxNumGeoAreaSupported = C_MAX_NUM_GEOAREA_PER_SUPL_INIT;  /* spec: 32; impl: 1 */

  p_service_capabilities->eventTriggerCapabilities.geoAreaShapesSupported.ellipticalArea = 0;
  p_service_capabilities->eventTriggerCapabilities.geoAreaShapesSupported.polygonArea = 0;

  p_service_capabilities->sessionCapabilities.maxNumberTotalSessions = C_MAX_GEOFENCE_NUM;
  p_service_capabilities->sessionCapabilities.maxNumberTriggeredSessions = C_MAX_GEOFENCE_NUM;
  p_service_capabilities->sessionCapabilities.maxNumberPeriodicSessions = 1; /* not used but need a INTEGER (1..32) */




  /* populate the relevant part of SUPL_START with the cached cell ID info. */
  if ( !supl_cell_information_construct(&p_SuplTriggerStart->locationId.cellInfo,
                                        &z_cell_info_allocated) )
  {
    MSG_HIGH("No Cell ID info.", 0, 0, 0);
    return FALSE;
  }

  /* MLID for SUPL_TRIGGER_START */
  if ( u_supl_global_num_mlid > 1 ) /* note being 1 means only the current serving cell is in the structure */
  {
    p_LocationIdData = (supl_LocationIdData*)os_MemAlloc((sizeof(supl_LocationIdData) * (u_supl_global_num_mlid - 1)), OS_MEM_SCOPE_TASK);
    p_LocationId = (supl_LocationId*)os_MemAlloc((sizeof(supl_LocationId) * (u_supl_global_num_mlid - 1)), OS_MEM_SCOPE_TASK);
    p_cell_info_allocated_mlid = (supl_cell_info_struct_type*)os_MemAlloc((sizeof(supl_cell_info_struct_type) * (u_supl_global_num_mlid - 1)), OS_MEM_SCOPE_TASK);

    if ( ( p_LocationIdData == NULL ) || (p_LocationId == NULL) || (p_cell_info_allocated_mlid == NULL ) )
    {
      u_num_mlid = 0;
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "mlid mem. allocation failed, set num_mlid to %u ", u_num_mlid);
    }
    else
    {
      memset(p_LocationIdData, 0, sizeof(supl_LocationIdData) * ( u_supl_global_num_mlid - 1) );
      memset(p_LocationId, 0, sizeof(supl_LocationId) * ( u_supl_global_num_mlid - 1) );
      memset(p_cell_info_allocated_mlid, 0, sizeof(supl_cell_info_struct_type) * ( u_supl_global_num_mlid - 1) );

      p_LocationIdData->locationId = p_LocationId;
      u_num_mlid = supl_chk_and_build_all_mlid(p_LocationIdData, p_LocationId,
                                               p_cell_info_allocated_mlid, ( u_supl_global_num_mlid - 1) );
    }
  }

  if ( u_num_mlid > 0 ) /* valid mlid exists */
  {
    if (p_LocationIdData != NULL)
    {
      p_SuplTriggerStart->m.multipleLocationIdsPresent = TRUE;
      p_SuplTriggerStart->multipleLocationIds.n = u_num_mlid;
      p_SuplTriggerStart->multipleLocationIds.elem = p_LocationIdData;
    }
    else
    {
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "p_LocationIdData not allocated: %lx ", p_LocationIdData);
    }
  }


  /* set this to CURRENT if cell id is of the current cell; otherwise set
     it to STATE to prevent the network from using this cell id to generate
     cell-id based positions for NI */
  p_SuplTriggerStart->locationId.status =
   ( q_handovers_since_last_cell_id == 0 ) ?
     supl_Status_current : supl_Status_stale;

  if ( z_supl_session_status.u_hash_valid )
  {
    p_SuplTriggerStart->m.verPresent = 1;
    p_SuplTriggerStart->ver.numbits = 64;
    memscpy ( &p_SuplTriggerStart->ver.data[0], sizeof(p_SuplTriggerStart->ver.data),
              &z_supl_session_status.supl_init_hash[0], sizeof(z_supl_session_status.supl_init_hash) );
    z_supl_session_status.u_hash_valid = FALSE; /* later SUPL_POS_INIT should not include HASH */
  }


  encode_status = tm_osys_asn1_encode_pdu((void *)&pdu, &pdu_buff, PROTO_SUPL);

  if ( (encode_status != 0) || (pdu_buff.value == NULL ) || (pdu_buff.length == 0) )
  {
    /* ASN.1 encoding failed */
    if (pdu_buff.value != NULL )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SUPL ASN1 enc. err. Freeing: %d,%lx,%u",
            encode_status, pdu_buff.value, pdu_buff.length);

      tm_asn1_free_osys_buf( pdu_buff.value, PROTO_SUPL );
      pdu_buff.value = NULL;

    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SUPL ASN1 enc. err. No alloc.: %d,%lx,%u",
            encode_status, pdu_buff.value, pdu_buff.length);
    }

    if ( p_cell_info_allocated_mlid != NULL )
    {
      os_MemFree((void**)&p_cell_info_allocated_mlid);
      p_cell_info_allocated_mlid = NULL;
    }
    if ( p_LocationId != NULL )
    {
      os_MemFree((void**)&p_LocationId);
      p_LocationId = NULL;
    }
    if ( p_LocationIdData != NULL)
    {
      p_LocationIdData->locationId = NULL;
      os_MemFree((void**)&p_LocationIdData);
      p_LocationIdData = NULL;
    }

    return FALSE;
  }

  else
  {
    pd_comms_return_type ret_val;

    p_length = (uint8 *)pdu_buff.value;
    u_length_msb = (pdu_buff.length >> 8) & 0x00FF;
    u_length_lsb = pdu_buff.length & 0x00FF;

    *p_length++ = u_length_msb;
    *p_length = u_length_lsb;

    ret_val = tm_umts_up_supl_comm_write( (uint8 *)pdu_buff.value, pdu_buff.length);

    if ( ( ret_val != PDCOMM_RESULT_WAIT ) && ( ret_val != PDCOMM_RESULT_OK ) )
    {
      /* message sending failed */
      MSG_ERROR("SUPL: SuplStart attempt aborted", 0, 0, 0);
    }
    else
    {
      supl_set_cf_state_area_event(C_SUPL_CF_STATE_UT1_SUPLTRIGGERRESPONSE, u_session_num);
      u_ret = TRUE;
    }

    /*Free the encoded PDU*/
    tm_asn1_free_osys_buf( pdu_buff.value, PROTO_SUPL );
    pdu_buff.value = NULL;

  }

  if ( u_ret )
  {
    MSG_MED("SuplTriggerStart sent ", 0, 0, 0);
  }

  if ( p_cell_info_allocated_mlid != NULL )
  {
    os_MemFree((void**)&p_cell_info_allocated_mlid);
    p_cell_info_allocated_mlid = NULL;
  }
  if ( p_LocationId != NULL )
  {
    os_MemFree((void**)&p_LocationId);
    p_LocationId = NULL;
  }
  if ( p_LocationIdData != NULL)
  {
    p_LocationIdData->locationId = NULL;
    os_MemFree((void**)&p_LocationIdData);
    p_LocationIdData = NULL;
  }

  return u_ret;


}


/*
******************************************************************************
* supl_SuplTriggerResponse_proc
*
* Function description:
*
*   Process incoming SUPL TRIGGER RESPONSE
*
* Parameters: p_pdu_decoded: pointer to a structure holding decoded Supl PDU.
*
* Return value: TRUE/FALSE
*
******************************************************************************
*/

static void supl_SuplTriggerResponse_proc ( const supl_ULP_PDU* p_pdu_decoded )
{

  const supl_Ver2_SUPLTRIGGEREDRESPONSE* p_suplTrigResponse;
  uint8      u_session_num = 0xFF;
  boolean    u_supl_trig_response_accept = FALSE;
  supl_AreaEventParams         *p_area_event_params;
  supl_CircularArea            *p_circular_area;
  supl_AreaIdList              *p_ArealIDList;
  supl_AreaId                  *p_AreaID_per_list;
  uint32                       transaction_id;
  sm_gm_add_request_type       gm_request;
  uint32                       q_temp;
  sint31                       l_temp;
  double                       d_temp;
  uint8                        error_code;
  uint8                        u_i;
  uint16                       w_j;
  uint16                       w_num_area_id;

  supl_gm_blob                 z_gm_blob;

  MSG_MED("SUPL_TRIG_RESPONSE rcvd", 0, 0, 0);


  p_suplTrigResponse = p_pdu_decoded->message.u.msSUPLTRIGGEREDRESPONSE;
  p_area_event_params = p_suplTrigResponse->triggerParams.u.areaEventParams;

  /* identify the active session slot */
  u_session_num = supl_area_event_session_match(&p_pdu_decoded->sessionID);

  if ( u_session_num >= C_MAX_GEOFENCE_NUM )
  {
    /* no matching areaEvent session found: it should return 0xFF */
    /* TBD: notify SLP */
    return;
  }

  tm_umts_up_supl_terminate_timer_cancel_cf_state_area_event
    (C_SUPL_CF_STATE_UT1_SUPLTRIGGERRESPONSE, u_session_num);


  transaction_id = supl_comm_getnext_gm_transaction_id();
  memset((void*)&gm_request, 0, sizeof(sm_gm_add_request_type));

  if ( p_suplTrigResponse->m.triggerParamsPresent )
  {
    if ( p_suplTrigResponse->triggerParams.t ==
         T_supl_TriggerParams_areaEventParams )
    {
      if ( p_area_event_params->areaEventType <= C_AREA_EVENT_TYPE_LEAVING )
      {

        u_supl_trig_response_accept = TRUE; /* all must-have are here and valid */


        gm_request.client_handle = supl_gm_handle;
        gm_request.q_transaction_id = transaction_id;

        if(p_area_event_params->areaEventType == supl_AreaEventType_enteringArea)
        {
          gm_request.geofence.event_type = GEOFENCE_EVENT_ENTERING_AREA;
        }
        else if(p_area_event_params->areaEventType == supl_AreaEventType_insideArea)
        {
          gm_request.geofence.event_type = GEOFENCE_EVENT_INSIDE_AREA;
        }
        else if(p_area_event_params->areaEventType == supl_AreaEventType_outsideArea)
        {
          gm_request.geofence.event_type = GEOFENCE_EVENT_OUTSIDE_AREA;
        }
        else if(p_area_event_params->areaEventType == supl_AreaEventType_leavingArea)
        {
          gm_request.geofence.event_type = GEOFENCE_EVENT_LEAVING_AREA;
        }
        else
        {
          MSG_ERROR("AreaEventType %d not recongnized", p_area_event_params->areaEventType,0,0);
        }
      }

      gm_request.geofence.location_estimate =
        ( p_area_event_params->locationEstimate ) ? TRUE : FALSE;


      if ( p_area_event_params->m.repeatedReportingParamsPresent )
      {

        gm_request.geofence.min_interval_time =
         ( p_area_event_params->repeatedReportingParams.minimumIntervalTime >=
           C_AREA_EVENT_MIN_REPORT_INTERVAL_FLOOR ) ?
         p_area_event_params->repeatedReportingParams.minimumIntervalTime :
           C_AREA_EVENT_MIN_REPORT_INTERVAL_FLOOR ;

         z_supl_area_event_session_status[u_session_num].q_min_interval_time =
           gm_request.geofence.min_interval_time;

        gm_request.geofence.max_num_reports =
          p_area_event_params->repeatedReportingParams.maximumNumberOfReports;
      }
      else
      {

        gm_request.geofence.max_num_reports = 1;

        /* since it is defined not to include value 0, use 0 to indicate its
           absence */
        z_supl_area_event_session_status[u_session_num].q_min_interval_time = 0;
      }

      gm_request.geofence.start_time =
        ( p_area_event_params->m.startTimePresent ) ?
          p_area_event_params->startTime : 0;

      gm_request.geofence.stop_time =
        ( p_area_event_params->m.stopTimePresent ) ?
          p_area_event_params->stopTime :
            C_AREA_EVENT_DEFAULT_STOP_TIME;

      if(gm_request.geofence.stop_time > gm_request.geofence.start_time)
      {
        gm_request.geofence.stop_time =  gm_request.geofence.stop_time - gm_request.geofence.start_time;
      }
      else
      {
        gm_request.geofence.stop_time = 0;
      }

      if ( p_area_event_params->m.geographicTargetAreaListPresent )
      {

        /* regardless of how many geographic areas the SLP gives, this
           implementation supports only one, so will only read the first one,
           and hence the follow param. is ignored.
           p_suplTrigResponse->triggerParams.u.areaEventParams->geographicTargetAreaList.n
         */

        if ( p_area_event_params->geographicTargetAreaList.elem->t !=
             T_supl_GeographicTargetArea_circularArea
           )
        {
          u_supl_trig_response_accept = FALSE;
        }
        else /* process circular: lat/long def. same as in PosEstimate -- GAD */
        {
          p_circular_area =
           p_area_event_params->geographicTargetAreaList.elem->u.circularArea;

          gm_request.geofence.target_area_provided = TRUE;
          gm_request.geofence.target_area.target_area_type =
            GEOFENCE_TARGET_AREA_CIRCULAR_TYPE;

          /* lat/long for geofence input: 2's complement */

          q_temp = ( p_circular_area->coordinate.latitudeSign ==
                     supl_Coordinate_latitudeSign_north ) ?
                    p_circular_area->coordinate.latitude :
                    ( p_circular_area->coordinate.latitude |
                      0x80000000
                    );

          if( q_temp & 0x80000000 )
          {
            d_temp = (double) ((int32) (q_temp & 0x7FFFFFFF) * -1);
          }
          else
          {
            d_temp = (double) q_temp;
          }

          /* restore degrees from GAD: * 90/2^23, and GM API unit for latitude
             being 180/2^25 degrees, so * 90/2^23 * 2^25 / 180 = * 2 */
          d_temp *=  2.0;

          gm_request.geofence.target_area.u.circ_target_area.latitude =
            tm_util_nearest_long(d_temp);

          MSG_HIGH("to GM - LaT GAD/circ:   %ld, %ld",
                   p_circular_area->coordinate.latitude,
                   gm_request.geofence.target_area.u.circ_target_area.latitude,
                   0);

          l_temp = tm_umts_common_utils_S32Convert( p_circular_area->coordinate.longitude, 24 );

          /*  restore degrees from GAD shape: * 180/2^23, and GM API unit for
              longitude being 180/2^25 degrees, so * 180/2^23 * 2^25 / 180 = 4 */

          d_temp = (double)l_temp * 4.0;

          gm_request.geofence.target_area.u.circ_target_area.longitude =
            tm_util_nearest_long(d_temp);

          gm_request.geofence.target_area.u.circ_target_area.radius =
            p_circular_area->radius;

          gm_request.geofence.num_of_area_ids = 0; /* this may change */

        } /* else: it is circular */
      } /* if - geographic target list */


      /*****  areaID List ****************************************************/

      if ( p_area_event_params->m.areaIdListsPresent )
      {
        uint8 u_num_areaIDLists = 0;
        uint16 w_num_areaID_per_list = 0;

        sm_gm_area_id *p_gm_area_id_list = NULL;
        /* points to the base of the entire list allocated for GM */

        sm_gm_area_id *p_gm_area_id = NULL;
        /* points to each GM area ID */

        u_num_areaIDLists =
           (p_area_event_params->areaIdLists.n <= C_MAX_NUM_AREA_ID_LIST) ?
           p_area_event_params->areaIdLists.n : C_MAX_NUM_AREA_ID_LIST;



        p_ArealIDList = p_area_event_params->areaIdLists.elem; /* point to 1st list */

        /**** determining total number of area IDs **************/
        w_num_area_id = 0;
        for (u_i=0; u_i< u_num_areaIDLists; u_i++)
        {
          w_num_areaID_per_list =
            ( p_ArealIDList->areaIdSet.n <= C_MAX_NUM_AREA_ID_PER_LIST ) ?
            p_ArealIDList->areaIdSet.n : C_MAX_NUM_AREA_ID_PER_LIST;

          p_AreaID_per_list = p_ArealIDList->areaIdSet.elem;

          for (w_j = 0; w_j < w_num_areaID_per_list; w_j++)
          {

            if ( w_j < (w_num_areaID_per_list  - 1) )
            {
              p_AreaID_per_list++;
            }
            w_num_area_id++;

          } /* areaID within one areaIDList for-loop */

          if ( u_i <  ( u_num_areaIDLists  -1) )
          {
            p_ArealIDList++; /* point to the next list */

          }
        } /* areaIdList for-loop */


        /**** allocating memory of total area IDs for GM *********/
        p_gm_area_id_list = (sm_gm_area_id*)os_MemAlloc((sizeof(sm_gm_area_id) * w_num_area_id), OS_MEM_SCOPE_TASK);


        if ( p_gm_area_id_list == NULL )
        {
          MSG_ERROR("failed to allocate mem. for GM areaID_list", 0, 0, 0);
          /* TBD: inform SLP */
          return;
        }

        memset( p_gm_area_id_list, 0, sizeof(sm_gm_area_id) * w_num_area_id );

        gm_request.geofence.area_id_list = p_gm_area_id_list;
        /*  base of the list */

        gm_request.geofence.num_of_area_ids = w_num_area_id;
        /* total number of area IDs */

        p_gm_area_id = p_gm_area_id_list;
        /* individual GM area ID starts at 1st */

        p_ArealIDList = p_area_event_params->areaIdLists.elem; /* re-point to 1st list */

        for (u_i=0; u_i< u_num_areaIDLists; u_i++)
        {

          w_num_areaID_per_list =
            ( p_ArealIDList->areaIdSet.n <= C_MAX_NUM_AREA_ID_PER_LIST ) ?
            p_ArealIDList->areaIdSet.n : C_MAX_NUM_AREA_ID_PER_LIST;

          p_AreaID_per_list = p_ArealIDList->areaIdSet.elem;

          MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED,
                "AreaList #%u containing %u areaIDs",
                u_i, w_num_areaID_per_list);

          for (w_j = 0; w_j < w_num_areaID_per_list; w_j++)
          {

            /* if there is no geographic target area, all areaIDs should
               be considered as within */
            if (p_area_event_params->m.geographicTargetAreaListPresent)
            {
              p_gm_area_id->area_id_type =
               ( p_ArealIDList->areaIdSetType == supl_AreaIdSetType_border ) ?
                 GEOFENCE_AREA_ID_BORDER_TYPE : GEOFENCE_AREA_ID_WITHIN_TYPE;
            }
            else
            {
              p_gm_area_id->area_id_type = GEOFENCE_AREA_ID_WITHIN_TYPE;
            }

            switch (p_AreaID_per_list->t)
            {

              case T_supl_AreaId_gSMAreaId:

                p_gm_area_id->area_id_rat_type = GEOFENCE_AREA_ID_RAT_GSM_TYPE;

                if ( p_AreaID_per_list->u.gSMAreaId->m.refMCCPresent )
                {
                  p_gm_area_id->u.gSMAreaId.mcc =
                    p_AreaID_per_list->u.gSMAreaId->refMCC;
                }
                else
                {
                  p_gm_area_id->u.gSMAreaId.mcc =
                   GEOFENCE_NO_MCC; /* indicate the absence of mcc */
                }

                if ( p_AreaID_per_list->u.gSMAreaId->m.refMNCPresent )
                {
                  p_gm_area_id->u.gSMAreaId.mnc =
                    p_AreaID_per_list->u.gSMAreaId->refMNC;
                }
                else
                {
                  p_gm_area_id->u.gSMAreaId.mnc =
                   GEOFENCE_NO_MNC; /* to indicate the absence of mnc */
                }

                if ( p_AreaID_per_list->u.gSMAreaId->m.refLACPresent )
                {
                  p_gm_area_id->u.gSMAreaId.lac =
                    p_AreaID_per_list->u.gSMAreaId->refLAC;
                }
                else
                {
                  p_gm_area_id->u.gSMAreaId.lac =
                   GEOFENCE_NO_LAC; /* use 0xFFFF to indicate the absence of lab: note TBD covering 65535 */
                }

                if ( p_AreaID_per_list->u.gSMAreaId->m.refCIPresent )
                {
                  p_gm_area_id->u.gSMAreaId.cell_identity =
                    p_AreaID_per_list->u.gSMAreaId->refCI;
                }
                else
                {
                  p_gm_area_id->u.gSMAreaId.cell_identity =
                   GEOFENCE_NO_GSM_CID; /* to indicate the absence of cell_identity */
                }

                break;

              case  T_supl_AreaId_wCDMAAreaId:

                p_gm_area_id->area_id_rat_type =
                  GEOFENCE_AREA_ID_RAT_WCDMA_TYPE;

                if ( p_AreaID_per_list->u.wCDMAAreaId->m.refMCCPresent )
                {
                  p_gm_area_id->u.wCDMAAreaId.mcc =
                    p_AreaID_per_list->u.wCDMAAreaId->refMCC;
                }
                else
                {
                  p_gm_area_id->u.wCDMAAreaId.mcc =
                   GEOFENCE_NO_MCC; /* to indicate the absence of mcc */
                }

                if ( p_AreaID_per_list->u.wCDMAAreaId->m.refMNCPresent )
                {
                  p_gm_area_id->u.wCDMAAreaId.mnc =
                    p_AreaID_per_list->u.wCDMAAreaId->refMNC;
                }
                else
                {
                  p_gm_area_id->u.wCDMAAreaId.mnc =
                   GEOFENCE_NO_MNC; /* to indicate the absence of mnc */
                }

                if ( p_AreaID_per_list->u.wCDMAAreaId->m.refLACPresent )
                {
                  p_gm_area_id->u.wCDMAAreaId.lac =
                    p_AreaID_per_list->u.wCDMAAreaId->refLAC;
                }
                else
                {
                  p_gm_area_id->u.wCDMAAreaId.lac =
                   GEOFENCE_NO_LAC; /* to indicate the absence of lab */
                }

                if ( p_AreaID_per_list->u.wCDMAAreaId->m.refUCPresent )
                {
                  p_gm_area_id->u.wCDMAAreaId.cell_identity =
                    p_AreaID_per_list->u.wCDMAAreaId->refUC;
                }
                else
                {
                  p_gm_area_id->u.wCDMAAreaId.cell_identity =
                   GEOFENCE_NO_WCDMA_CID; /* to indicate the absence of cell_identity */
                }

                break;

              case T_supl_AreaId_lTEAreaId:

                p_gm_area_id->area_id_rat_type =
                  GEOFENCE_AREA_ID_RAT_LTE_TYPE;

                if ( p_AreaID_per_list->u.lTEAreaId->m.refMCCPresent )
                {
                  p_gm_area_id->u.lTEAreaId.mcc =
                    p_AreaID_per_list->u.lTEAreaId->refMCC;
                }
                else
                {
                  p_gm_area_id->u.lTEAreaId.mcc =
                   GEOFENCE_NO_MCC; /* to indicate the absence of mcc */
                }

                if ( p_AreaID_per_list->u.lTEAreaId->m.refMNCPresent )
                {
                  p_gm_area_id->u.lTEAreaId.mnc =
                    p_AreaID_per_list->u.lTEAreaId->refMNC;
                }
                else
                {
                  p_gm_area_id->u.lTEAreaId.mnc =
                   GEOFENCE_NO_MNC; /* to indicate the absence of mnc */
                }

                if ( p_AreaID_per_list->u.lTEAreaId->m.refCIPresent )
                {
                  /* p_AreaID_per_list->u.lTEAreaId->refCI is of bitstring size 29: */

                  /* p_AreaID_per_list->u.lTEAreaId->refCI.data[0] holds the 8 msb
                     of the 29-bit refCI;
                   */
                  q_temp = (uint32)p_AreaID_per_list->u.lTEAreaId->refCI.data[0];
                  q_temp <<= 21;
                  q_temp &= 0x1FE00000;
                  p_gm_area_id->u.lTEAreaId.cell_identity = q_temp;

                  q_temp = (uint32)p_AreaID_per_list->u.lTEAreaId->refCI.data[1];
                  q_temp <<= 13;
                  q_temp &= 0x001FE000;
                  p_gm_area_id->u.lTEAreaId.cell_identity |= q_temp;

                  q_temp = (uint32)p_AreaID_per_list->u.lTEAreaId->refCI.data[2];
                  q_temp <<=5;
                  q_temp &= 0x00001FE0;
                  p_gm_area_id->u.lTEAreaId.cell_identity |= q_temp;

                  /* p_AreaID_per_list->u.lTEAreaId->refCI.data[3] holds the 5 lsb
                     of the 29-bit refCI on its left 5 bits;
                   */
                  q_temp = (uint32)p_AreaID_per_list->u.lTEAreaId->refCI.data[3];
                  q_temp >>= 3;
                  q_temp &= 0x0000001F;
                  p_gm_area_id->u.lTEAreaId.cell_identity |= q_temp;

                }
                else
                {
                  p_gm_area_id->u.lTEAreaId.cell_identity =
                   GEOFENCE_NO_LTE_CID; /* use max. allowed plus 1 to indicate the absence of lab */
                }

                break;

              default:
                MSG_HIGH("Unsupported RAT type for geofence: %u",
                          p_AreaID_per_list->t, 0, 0);


            } /* end of areaID RAT type switch */

            if ( w_j < (w_num_areaID_per_list  - 1) )
            {
              p_AreaID_per_list++;
              p_gm_area_id++;
            }

          } /* areaID within one areaIDList for-loop */

          if ( u_i <  ( u_num_areaIDLists  -1) )
          {
            p_ArealIDList++;
            p_gm_area_id++;
          }

        } /* areaIdList for-loop */


      } /* if areaIDList present */

      /****  Add this geofence ********/
      memscpy(&z_gm_blob.z_session_id, sizeof(z_gm_blob.z_session_id),
             &z_supl_area_event_session_status[u_session_num].z_session_id,
             sizeof(z_supl_area_event_session_status[u_session_num].z_session_id) );

      z_gm_blob.q_supl_version =
        z_supl_area_event_session_status[u_session_num].q_supl_version;

      z_gm_blob.w_SetSessionID =
        z_supl_area_event_session_status[u_session_num].z_session_id.z_Set_Sess_Id.w_Session_Id;

      /**** allocating memory of client blob for GM *********/
      gm_request.geofence.client_blob = (byte*)os_MemAlloc(sizeof(supl_gm_blob), OS_MEM_SCOPE_TASK);


      if (  gm_request.geofence.client_blob == NULL )
      {
        MSG_ERROR("failed to allocate mem. for GM client blob", 0, 0, 0);
        if( (gm_request.geofence.area_id_list != NULL) &&
            (gm_request.geofence.num_of_area_ids > 0)
          )
        {
          os_MemFree((void**)&(gm_request.geofence.area_id_list));
        }
        /* TBD: inform SLP */
        return;
      }
      memset( (void *)gm_request.geofence.client_blob, 0, sizeof(supl_gm_blob) );
      gm_request.geofence.client_blob_size = sizeof(supl_gm_blob);

      memscpy(&gm_request.geofence.client_blob[0], sizeof(gm_request.geofence.client_blob),
             &z_gm_blob, sizeof(z_gm_blob) );

      gm_request.geofence.persistency_needed = TRUE;
      gm_request.geofence.responsiveness = GEOFENCE_RESPONSIVENESS_HIGH;

      gm_request.geofence.hysteresis_required = TRUE;

      if(gm_add_geofence(&gm_request,&error_code) == FAILED)
      {
         MSG_HIGH("Not able to add Geofence, error_code %d", error_code, 0,0);
         /* TBD: response to SLP needed */
      }
      else
      {
        MSG_HIGH("Succesfully sent request to add Geofence",0,0,0);
        z_supl_area_event_session_status[u_session_num].u_gm_trasaction_id_valid = TRUE;
        z_supl_area_event_session_status[u_session_num].q_gm_transaction_id =
          gm_request.q_transaction_id;
      }



      /* determine if the tcp link is to be kept or disconnected, based on start time */
      if ( gm_request.geofence.start_time >=
           C_SUPL_DEFAULT_TCP_ON_ONLY_TIME_SEC  )
      {
        /* disconnect tcp for now */
        tm_umts_up_supl_comm_close_connection();
        MSG_MED("TCP disconnect: areaEvent start time: %u ",
                 p_area_event_params->startTime, 0,0);

      }
      else
      {
        /* If no single-shot session going on, start a timer for 130 sec.,
           if GM does not request anything by that time, disconnect tcp;
           the time is canceled when needing to send the message to SLP  */
        if ( !z_supl_session_status.u_network_session_on )
        {
          MSG_MED("TCP stayOnOnly timer start: areaEvent start time: %u ",
                   p_area_event_params->startTime, 0,0);
          (void) os_TimerStart( supl_tcp_stay_on_only_timer,
                                ( C_SUPL_DEFAULT_TCP_ON_ONLY_TIME_SEC * 1000),
                                NULL );
        }
      }


    } /* if - areaEventTrigger */
    else
    {
      /* periodic trigger not supported */
      (void) tm_umts_up_supl_supl_end_tx (
        &z_supl_area_event_session_status[u_session_num].z_session_id.z_Slp_Sess_Id,
        NULL,
        supl_ver2_serviceNotSupported,
        NULL,
        z_supl_area_event_session_status[u_session_num].q_supl_version);

      memset(&z_supl_area_event_session_status[u_session_num],
             0,
             sizeof(supl_area_event_status_s_type));

      tm_umts_up_supl_comm_close_connection();


    }

  } /* if - triggerParam present */

  else  /* triggerParam absent: for NI such IE is mandatory */
  {
    (void) tm_umts_up_supl_supl_end_tx (
      &z_supl_area_event_session_status[u_session_num].z_session_id.z_Slp_Sess_Id,
      NULL,
      supl_dataMissing,
      NULL,
      z_supl_area_event_session_status[u_session_num].q_supl_version);

    memset(&z_supl_area_event_session_status[u_session_num],
           0,
           sizeof(supl_area_event_status_s_type));

    tm_umts_up_supl_comm_close_connection();

  }

  if ( u_supl_trig_response_accept == FALSE )
  {
    MSG_HIGH("SUPL_TRIGGER_RESPONSE not accepted", 0, 0, 0);
  }

  return;
}

/*
******************************************************************************
* supl_SuplReport_proc
*
* Function description:
*
*   Process incoming SUPL TRIGGER RESPONSE
*
* Parameters: p_pdu_decoded: pointer to a structure holding decoded Supl PDU.
*
* Return value: TRUE/FALSE
*
******************************************************************************
*/

static void supl_SuplReport_proc( const supl_ULP_PDU* p_pdu_decoded )
{
  /* when receiving SUPL_REPORT from SLP, the assistance data related
     exchange with the SLP is complete within this current areaEvent
     trigger session */

  /* there is no way of knowing when a breach will happen, terminate
     TCP link now   */
  if ( z_supl_session_status.e_cf_state & C_SUPL_CF_STATE_UT3_EXPECT_SUPLREPORT )
  {
    tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_UT3_EXPECT_SUPLREPORT);
    tm_umts_up_supl_callflow_kind_set(C_SUPL_SESSION_NONE);
    tm_umts_up_supl_comm_close_connection();
    z_supl_session_status.u_current_is_for_area_event = FALSE;
    MSG_MED("Recvd SUPL_REPORT, disc. TCP", 0, 0, 0);
  }
  else
  {
    MSG_MED("Recvd unexpected SUPL_REPORT", 0, 0, 0);
  }
}

/*
******************************************************************************
* supl_SuplTrigStop_proc
*
* Function description:
*
*   Process incoming SUPL TRIGGER STOP
*
* Parameters: p_pdu_decoded: pointer to a structure holding decoded Supl PDU.
*
* Return value: none.
*
******************************************************************************
*/

static void supl_SuplTrigStop_proc (const supl_ULP_PDU* p_pdu_decoded)
{
  sm_gm_delete_request_type z_sm_gm_delete_request;
  supl_set_session_id z_provided_set_session_id;
  supl_slp_session_id z_provided_slp_session_id;
  uint8 u_error_code = 0;
  supl_StatusCode e_supl_status_code = 0xFFFFFFFF;
  uint8 u_session_num;

  memset(&z_provided_set_session_id, 0, sizeof(supl_set_session_id));
  memset(&z_provided_slp_session_id, 0, sizeof(supl_slp_session_id));

  z_sm_gm_delete_request.client_handle = supl_gm_handle;

  u_session_num = supl_area_event_session_match(&p_pdu_decoded->sessionID);

  if ( u_session_num != 0xFF ) /* found a match */
  {
    z_sm_gm_delete_request.q_transaction_id =
      z_supl_area_event_session_status[u_session_num].q_gm_transaction_id;

    z_sm_gm_delete_request.geofence_id =
      z_supl_area_event_session_status[u_session_num].q_geofence_id;


    if ( gm_delete_geofence(&z_sm_gm_delete_request, &u_error_code) ==
         FAILED )
    {
      MSG_ERROR( "Failure of geofence delete: ID %lu, error=%u",
                 z_sm_gm_delete_request.geofence_id, u_error_code, 0);
      e_supl_status_code = supl_StatusCode_unspecified;
    }
    else
    {
      MSG_MED("Goefence %u deleted", z_sm_gm_delete_request.geofence_id, 0, 0);
      z_supl_area_event_session_status[u_session_num].u_session_active = FALSE;
    }
  }

  /* Send SUPL_END using the trigged Session ID */

  supl_set_session_id_organizer ( &p_pdu_decoded->sessionID, &z_provided_set_session_id );
  supl_util_server_session_id_organizer ( &p_pdu_decoded->sessionID, &z_provided_slp_session_id );


 (void) tm_umts_up_supl_supl_end_prepare_or_tx( &z_provided_slp_session_id,
                                                &z_provided_set_session_id,
                                                e_supl_status_code,
                                                NULL,
                                                z_supl_session_status.q_supl_version
                                              );

}

/*
******************************************************************************
* supl_area_event_session_match
*
* Function description:
*
*   Identify the matching area event session when a new area event trigger
*   type of session message arrives.
*
* Parameters: p_source_session_id - pointer to a structure holding decoded
*   sessionID info. in the message from the SLP.
*
* Return value: matching session number; 0xFF - no match
*
******************************************************************************
*/

static uint8 supl_area_event_session_match ( const supl_SessionID *p_source_session_id )
{

  supl_slp_session_id  z_incoming_slp_session_id;

  supl_area_event_status_s_type *p_supl_event_status;


  uint8 u_i;
  uint8 u_ret_val = 0xFF;

  memset(&z_incoming_slp_session_id, 0, sizeof(supl_slp_session_id));

  /* organized SLP session ID and place it in z_incoming_slp_session_id */
  if ( p_source_session_id->m.slpSessionIDPresent )
  {
    supl_util_server_session_id_organizer ( p_source_session_id, &z_incoming_slp_session_id );
  }
  else
  {
    MSG_HIGH("SLP session ID missing, skip checking areaEvent match", 0, 0, 0);
    return u_ret_val;
  }

  p_supl_event_status = &z_supl_area_event_session_status[0];


  for (u_i=0; u_i<C_MAX_GEOFENCE_NUM; u_i++)
  {
    if ( p_supl_event_status->u_session_active )
    {
      /* check SLP session ID */

      if ( supl_SLP_session_id_match_check ( &p_supl_event_status->z_session_id.z_Slp_Sess_Id,
                                             &z_incoming_slp_session_id
                                           ) == TRUE
          )
      {

        u_ret_val = u_i; /* matching session found */
        MSG_MED("Active AreaEvent session #%u matches incoming message",
                 u_ret_val, 0, 0);
        break;
      }

    } /* if an active areaEvent session found */

    if ( u_i < ( C_MAX_GEOFENCE_NUM - 1 ) )
    {
      p_supl_event_status++;
    }

  } /* active areaEvent trigger session check for-loop */

  return u_ret_val;

}


/*
******************************************************************************
* tm_umts_up_supl_geofence_areaEventSession_match
*
* Function description:
*
*   Identify the matching area event session when GM asks SUPL for actions
*
* Parameters:
*
*  q_geofence_id: Geofence ID -- if 0xFFFFFFFF, no such info
*     provided in which case the function will find the first
*     available areaEvent session.
*
* Return value: identified active SUPL areaEvent session.
*               0xFF: no such session identified.
*
*
******************************************************************************
*/

uint8 tm_umts_up_supl_geofence_areaEventSession_match (uint32 q_geofence_id )
{
  uint8 u_i, u_session_num;

  u_session_num = 0xFF;


  for (u_i=0; u_i<C_MAX_GEOFENCE_NUM; u_i++)
  {
    if ( q_geofence_id != 0xFFFFFFFF )
    {
      if ( z_supl_area_event_session_status[u_i].u_geofence_id_valid &&
           ( z_supl_area_event_session_status[u_i].q_geofence_id ==
             q_geofence_id )
         )
      {
        u_session_num = u_i;
        break;
      }
    }
    else
    {
      if ( z_supl_area_event_session_status[u_i].u_geofence_id_valid )
      {
        u_session_num = u_i;
        break;
      }
    }
  }

  return u_session_num;

}


/*
******************************************************************************
* tm_umts_up_supl_area_event_to_active
*
* Function description:
*
*   Retrieve areaEvent session status info. for current active session use.
*
* Parameters: index to the areaEvent session to be used.
*
* Return value: None.
*
******************************************************************************
*/

void tm_umts_up_supl_area_event_to_active (uint8 u_session_num)
{

  if ( u_session_num >= C_MAX_GEOFENCE_NUM )
    return;

  z_supl_session_status.z_session_id.z_Set_Sess_Id.w_Session_Id =
    z_supl_area_event_session_status[u_session_num].z_session_id.z_Set_Sess_Id.w_Session_Id;

  memscpy( &z_supl_session_status.z_session_id.z_Slp_Sess_Id,
           sizeof(z_supl_session_status.z_session_id.z_Slp_Sess_Id),
          &z_supl_area_event_session_status[u_session_num].z_session_id.z_Slp_Sess_Id,
           sizeof(z_supl_area_event_session_status[u_session_num].z_session_id.z_Slp_Sess_Id) );

  z_supl_session_status.u_position_protocol = TM_POS_PRTL_RRLP;

  z_supl_session_status.u_current_is_for_area_event = TRUE;

}

/*
******************************************************************************
* tm_umts_up_supl_gf_breach_proc
*
* Function description:
*
*   Processes geofence breach info.
*
* Parameters: p_gm_client_notify - pointer to data of from the GM module.
*
* Return value: None.
*
******************************************************************************
*/

void tm_umts_up_supl_gf_breach_proc( sm_gm_client_breach_notify_type*  p_gm_client_breach_notify)
{
  uint8 u_session_num = 0xFF;

  u_session_num = tm_umts_up_supl_geofence_areaEventSession_match
                    (p_gm_client_breach_notify->geofence_id);

  if ( u_session_num == 0xFF )
  {
    MSG_HIGH("no matching SUPL session to proc. the GF breach %d",
              p_gm_client_breach_notify->geofence_id, 0, 0 );
  }
  else
  {

    if ( tm_umts_up_supl_supl_report_prepare_or_tx(u_session_num, p_gm_client_breach_notify, NULL, NULL) )
    {
      MSG_MED("SUPL_REPORT sent: %u", u_session_num, 0, 0);
    }
    else
    {
      MSG_MED("SUPL_REPORT pending: %u", u_session_num, 0, 0);
    }
  }

}

/*
******************************************************************************
* tm_umts_up_supl_gf_delete_proc
*
* Function description:
*
*   Processes geofence delete info..
*
* Parameters: p_gm_client_notify - pointer to data of from the GM module.
*
* Return value: None.
*
******************************************************************************
*/

void tm_umts_up_supl_gf_delete_proc( sm_gm_client_notification_type*  p_gm_client_notify)
{
  uint8 u_session_num = 0xFF;

  u_session_num = tm_umts_up_supl_geofence_areaEventSession_match
                    (p_gm_client_notify->notification.geofence_id);

  if ( u_session_num >= C_MAX_GEOFENCE_NUM )
  {
    MSG_MED("no matching SUPL session to proc. the GF breach %d",
             p_gm_client_notify->notification.geofence_id, 0, 0 );
  }
  else
  {
    if ( p_gm_client_notify->notification.return_code == GEOFENCE_DELETE_ON_MAX_REPORTS )
    {
      /* wait for SLP to send SUPL_END */
      MSG_MED("GF %u id=%u delete max. report reached - UT3 waiting starts",
               u_session_num, p_gm_client_notify->notification.geofence_id, 0);
    }
    else if ( p_gm_client_notify->notification.return_code == GEOFENCE_DELETE_ON_STOP_TIME )
    {
      if ( tm_umts_up_supl_supl_end_prepare_or_tx(
            &z_supl_area_event_session_status[u_session_num].z_session_id.z_Slp_Sess_Id,
            &z_supl_area_event_session_status[u_session_num].z_session_id.z_Set_Sess_Id,
            supl_StatusCode_ver2_sessionStopped,
            NULL,
            z_supl_area_event_session_status[u_session_num].q_supl_version )
          )
      {

        tm_umts_up_supl_comm_close_connection();

        MSG_MED( "SUPL_END AreaEventStopTime sent: %u, %u:",
                 u_session_num,
                 p_gm_client_notify->notification.geofence_id,
                 0 );

      }

      else
      {
        MSG_MED( "SUPL_END AreaEventStopTime pending: %u, %u:",
                 u_session_num,
                 p_gm_client_notify->notification.geofence_id,
                 0 );
      }

      /* clear the active session */
      if ( z_supl_area_event_session_status[u_session_num].u_session_active )
      {
        memset( &z_supl_area_event_session_status[u_session_num], 0,
                sizeof(supl_area_event_status_s_type) );
      }


    } /* if due to StopTime */
  }

}


/*
******************************************************************************
* tm_umts_up_supl_supl_report_prepare_or_tx
*
* Function description:
*
*   Processes prepare and/or send SUPL_REPORT.
*
* Parameters: p_gm_client_notify - pointer to data of from the GM module.
*
* Return value: TRUE - message sent in this function; FALSE - message not sent
*               yet.
*
******************************************************************************
*/


static boolean tm_umts_up_supl_supl_report_prepare_or_tx
( uint8                              u_session_num,
  sm_gm_client_breach_notify_type*   p_gm_client_breach_notify,
  uint8*                             p_hash,
  supl_slp_session_id*               p_provided_slp_session_id
)
{

  sm_gm_breach_ack_notify_type z_sm_gm_breach_ack_notify;
  uint8  u_sm_gm_error_code;

  pd_comms_return_type pdcomm_connect_ret_value = PDCOMM_RESULT_ERROR_UNKNOWN;
  boolean u_ret = FALSE;

  if ( u_session_num >= C_MAX_GEOFENCE_NUM )
    return FALSE;

  /* if no tcp link available yet, set up such link */
  if ( z_pdcomm_tcp_info.curr_state != PDCOMM_STATE_CONNECTED )
  {

    pdcomm_connect_ret_value = tm_umts_up_supl_comm_tcp_connect( SUPL_VERSION_MAIN_2 );

    if ( PDCOMM_RESULT_OK == pdcomm_connect_ret_value ) /* quite unlikely */
    {
      if ( tm_umts_up_supl_supl_report_tx( u_session_num,
                                           p_gm_client_breach_notify,
                                           p_hash,
                                           p_provided_slp_session_id
                                         )
         )
      {
        u_ret = TRUE;
      }
    }

    else if ( PDCOMM_RESULT_WAIT == pdcomm_connect_ret_value ) /* most likely */
    {
      if ( p_gm_client_breach_notify != NULL )
      {
        MSG_MED("Cache gm_breach_notify for later SUPL_REPORT", 0, 0, 0);
        z_supl_area_event_session_status[u_session_num].z_pending_supl_report_info.u_valid = TRUE;
        z_supl_area_event_session_status[u_session_num].z_pending_supl_report_info.z_gm_client_breach_notify =
          *p_gm_client_breach_notify;

        supl_set_cf_state_area_event(C_SUPL_CF_STATE_UT0_SUPLREPORT_GF_BREACH, u_session_num);

      }
      else if ( p_hash != NULL )
      {
        MSG_MED("Waiting TCP connection to respond to SessionInfoQuery", 0, 0, 0);
        supl_set_cf_state(C_SUPL_CF_STATE_UT0_SUPLREPORT_QUERY, 0);
      }

    }

    else
    {
      /* not possible to connect, abort */
      MSG_HIGH("SUPL: TCP conn. failed; NACK GM", 0, 0, 0);

      if ( p_gm_client_breach_notify != NULL )
      {
        /* for geofence breach, ack to GM now with NACK */

        z_sm_gm_breach_ack_notify.geofence_id =
          z_supl_area_event_session_status[u_session_num].q_geofence_id;

        /* SUPL_REPORT for breach report has failed to be sent from SM perspective */
        z_sm_gm_breach_ack_notify.accepted = FALSE;
        u_sm_gm_error_code = GEOFENCE_ERROR_UNSPECIFIED;

        gm_breach_ack(&z_sm_gm_breach_ack_notify, &u_sm_gm_error_code);
      }
    }

  } /* close if-tcp-link-not-yet-available */

  else /* already connected */
  {
    if ( tm_umts_up_supl_supl_report_tx( u_session_num,
                                         p_gm_client_breach_notify,
                                         p_hash,
                                         p_provided_slp_session_id
                                        )
       )
    {
      u_ret = TRUE;
    }
  }


  return u_ret;

}


/*
******************************************************************************
* tm_umts_up_supl_supl_report_tx
*
* Function description:
*
*   send SUPL_REPORT.
*
* Parameters:
*
* Return value: None.
*
******************************************************************************
*/


static boolean tm_umts_up_supl_supl_report_tx
( uint8                               u_session_num,
  sm_gm_client_breach_notify_type*    p_gm_client_breach_notify,
  uint8*                              p_hash,
  supl_slp_session_id*                p_provided_slp_session_id
)
{

  tm_pdu_buf_type pdu_buff; /* including length and pointer to data */
  supl_ULP_PDU      pdu;

  supl_Ver2_SUPLREPORT*   p_SuplReport;
  supl_Ver2_SUPLREPORT    z_SuplReport;
  supl_ReportData         z_supl_ReportData;
  supl_SessionInformation z_SessionInformation_array[C_MAX_GEOFENCE_NUM];
  supl_SessionInformation *p_SessionInformation_per_geofence;
  supl_IPAddress          z_supl_IPAddress_for_slp;


  uint32 q_temp;
  uint16 w_FixWn;
  uint32 q_fix_time_gps_msec, q_fix_time_utc_msec;


  int              encode_status;
  uint8*           p_length;
  uint8            u_length_msb = 0;
  uint8            u_length_lsb = 0;

  char          timestamp[14];
  utc_time      z_utc_time;
  uint64        t_TS;
  qword         qword_timestamp; /* in the format used by tm_lr_resp_type */
  uint32        q_Gps0P25Msec;

  supl_area_event_status_s_type* p_supl_area_event_status;
  uint8         u_i;

  boolean       u_ret  = FALSE;

  sm_gm_breach_ack_notify_type z_sm_gm_breach_ack_notify;
  uint8  u_sm_gm_error_code;


  /***************************************************************************/

  if ( u_session_num >= C_MAX_GEOFENCE_NUM )
    return FALSE;


  memset(&pdu, 0x00, sizeof(supl_ULP_PDU));
  memset(&pdu_buff, 0, sizeof(tm_pdu_buf_type));
  memset(&z_SuplReport, 0x00, sizeof(supl_Ver2_SUPLREPORT));
  memset(&z_supl_ReportData, 0x00, sizeof(supl_ReportData));
  memset(&z_SessionInformation_array[0], 0x00, (sizeof(supl_SessionInformation) * C_MAX_GEOFENCE_NUM) );
  memset(&z_supl_IPAddress_for_slp, 0, sizeof(supl_IPAddress));

  p_SuplReport = &z_SuplReport;
  pdu.message.t = C_SUPL_ASN1_CHOICE_ULP_MESSAGE_SUPLREPORT;
  pdu.message.u.msSUPLREPORT = p_SuplReport;


  if ( p_hash != NULL ) /* if HASH provided, it must be for SUPL_INFO_QUERY */
  {

    /* w_SetSessionID has incremented after last SUPL_TRIGGER_START, so this is a new number */
    if ( !supl_header_build (&pdu, &w_SetSessionID, NULL, p_provided_slp_session_id, z_supl_session_status.q_supl_version) )
    {
      MSG_ERROR("SUPL: SuplReport attempt aborted: header", 0, 0, 0);
      return FALSE;
    }
    z_supl_session_status.z_session_id.z_Set_Sess_Id.w_Session_Id = w_SetSessionID;

    if ( p_provided_slp_session_id != NULL )
    {
      memscpy(&z_supl_session_status.z_session_id.z_Slp_Sess_Id,
              sizeof(z_supl_session_status.z_session_id.z_Slp_Sess_Id),
             p_provided_slp_session_id,
             sizeof(*p_provided_slp_session_id) );
    }

    /* need SessionList: TBD need to handle mulitple active sessions */
    p_supl_area_event_status = &z_supl_area_event_session_status[0];
    p_SessionInformation_per_geofence = &z_SessionInformation_array[0];

    for ( u_i=0; u_i < C_MAX_GEOFENCE_NUM; u_i++)
    {

      if ( p_supl_area_event_status->u_session_active &&
           ( ( p_supl_area_event_status->z_session_id.z_Set_Sess_Id.z_Set_Id.set_id_choice ==
               T_supl_SETId_imsi
             ) ||
             ( p_supl_area_event_status->z_session_id.z_Set_Sess_Id.z_Set_Id.set_id_choice ==
               T_supl_SETId_msisdn
             )
           )
         )
      {

        z_SuplReport.m.sessionListPresent = TRUE;

        /* populate sessionInformation of the active areaEventTrigger sessions */
        p_SessionInformation_per_geofence->sessionID.m.setSessionIDPresent = TRUE;

        p_SessionInformation_per_geofence->sessionID.setSessionID.sessionId =
          p_supl_area_event_status->z_session_id.z_Set_Sess_Id.w_Session_Id;

        p_SessionInformation_per_geofence->sessionID.setSessionID.setId.t =
          p_supl_area_event_status->z_session_id.z_Set_Sess_Id.z_Set_Id.set_id_choice;

        if ( p_SessionInformation_per_geofence->sessionID.setSessionID.setId.t == T_supl_SETId_imsi )
        {
          p_SessionInformation_per_geofence->sessionID.setSessionID.setId.u.imsi =
            &p_supl_area_event_status->z_session_id.z_Set_Sess_Id.z_Set_Id.u.imsi;
        }
        else  /* ( z_SessionInformation.sessionID.setSessionID.setId.t == T_supl_SETId_msisdn ) */
        {
          p_SessionInformation_per_geofence->sessionID.setSessionID.setId.u.msisdn =
            &p_supl_area_event_status->z_session_id.z_Set_Sess_Id.z_Set_Id.u.msisdn;
        }

        p_SessionInformation_per_geofence->sessionID.m.slpSessionIDPresent = TRUE;

        p_SessionInformation_per_geofence->sessionID.slpSessionID.slpId.t =
          p_supl_area_event_status->z_session_id.z_Slp_Sess_Id.zSlpAddress.choice;

        p_SessionInformation_per_geofence->sessionID.slpSessionID.sessionID.numocts =
          C_SUPL_SLP_SESSION_ID_BYTE_LENGTH;

        memscpy(p_SessionInformation_per_geofence->sessionID.slpSessionID.sessionID.data,
                sizeof(p_SessionInformation_per_geofence->sessionID.slpSessionID.sessionID.data),
               p_supl_area_event_status->z_session_id.z_Slp_Sess_Id.session_Id,
               sizeof(p_supl_area_event_status->z_session_id.z_Slp_Sess_Id.session_Id));

        if ( p_SessionInformation_per_geofence->sessionID.slpSessionID.slpId.t
             == T_supl_SLPAddress_iPAddress)
        {
          p_SessionInformation_per_geofence->sessionID.slpSessionID.slpId.u.iPAddress =
            &z_supl_IPAddress_for_slp;

          z_supl_IPAddress_for_slp.t =
            p_supl_area_event_status->z_session_id.z_Slp_Sess_Id.zSlpAddress.u.z_Ip_Address.choice;

          /* IPV4 */
          if ( z_supl_IPAddress_for_slp.t == C_SUPL_ASN1_CHOICE_IPV4ADDRESS)
          {
            z_supl_IPAddress_for_slp.u.ipv4Address =
              &p_supl_area_event_status->z_session_id.z_Slp_Sess_Id.zSlpAddress.u.z_Ip_Address.u.Ip_V4;
          }
          /* IPV6 */
          else if ( z_supl_IPAddress_for_slp.t == T_supl_IPAddress_ipv6Address)
          {
            z_supl_IPAddress_for_slp.u.ipv6Address =
              &p_supl_area_event_status->z_session_id.z_Slp_Sess_Id.zSlpAddress.u.z_Ip_Address.u.Ip_V6;
          }
        }
        else if ( p_SessionInformation_per_geofence->sessionID.slpSessionID.slpId.t
                  == T_supl_SLPAddress_fQDN )
        {
          p_SessionInformation_per_geofence->sessionID.slpSessionID.slpId.u.fQDN =
           (supl_FQDN)p_supl_area_event_status->z_session_id.z_Slp_Sess_Id.zSlpAddress.u.z_fqdn.fqdn ;
        }

      } /* one existing geofence found */

      if ( (u_i + 1) < (C_MAX_GEOFENCE_NUM - 1 ) )
      {
        p_supl_area_event_status++;
        p_SessionInformation_per_geofence++;
      }

    } /* max. num. of geofence for-loop */

    if ( z_SuplReport.m.sessionListPresent == TRUE )
    {
      z_SuplReport.sessionList.n = 1; /* for current interoperative use, set to 1 */
      z_SuplReport.sessionList.elem = &z_SessionInformation_array[0];
    }

    z_SuplReport.m.verPresent = 1;
    z_SuplReport.ver.numbits = 64;
    memscpy ( &z_SuplReport.ver.data[0], sizeof(z_SuplReport.ver.data),
             &z_supl_session_status.supl_init_hash[0],
             sizeof(z_supl_session_status.supl_init_hash) );


  }

  else if  ( p_gm_client_breach_notify != NULL )
  {

    if ( !supl_header_build ( &pdu,
                              &z_supl_area_event_session_status[u_session_num].z_session_id.z_Set_Sess_Id.w_Session_Id,
                              NULL,
                              &z_supl_area_event_session_status[u_session_num].z_session_id.z_Slp_Sess_Id,
                              z_supl_area_event_session_status[u_session_num].q_supl_version )
       )
    {
      MSG_ERROR("SUPL: SuplReport attempt aborted: header", 0, 0, 0);

      /* NACK GM */
      z_sm_gm_breach_ack_notify.geofence_id =
        z_supl_area_event_session_status[u_session_num].q_geofence_id;

      /* SUPL_REPORT for breach report has failed to be sent from SM perspective */
      z_sm_gm_breach_ack_notify.accepted = FALSE;
      u_sm_gm_error_code = GEOFENCE_ERROR_UNSPECIFIED;

      gm_breach_ack(&z_sm_gm_breach_ack_notify, &u_sm_gm_error_code);

      return FALSE;
    }


    if ( p_gm_client_breach_notify->fix_included )
    {
      tm_lr_resp_type breach_pos_fix;
      
      tm_util_convert_lr(&p_gm_client_breach_notify->pos_fix_rpt, &breach_pos_fix);
      
      p_SuplReport->m.reportDataListPresent = TRUE;

      p_SuplReport->reportDataList.n = 1;
      p_SuplReport->reportDataList.elem = &z_supl_ReportData;

      z_supl_ReportData.m.positionDataPresent = TRUE;


      /* LAT: unit for latitude from GM breach (of the type of tm_lr_resp_type) :
         180/2^25 degrees, 2's compliment; obtain positionEstimate.latitude
         (GAD) from degrees:  * 2^23/90  --- so to convert from GM breach to
         positionEstimate.latitude:  /2
       */
      if (breach_pos_fix.latitude >= 0 )
      {
        z_supl_ReportData.positionData.position.positionEstimate.latitude =
          (OSUINT32)(breach_pos_fix.latitude >> 1);

        z_supl_ReportData.positionData.position.positionEstimate.latitudeSign =
          supl_PositionEstimate_latitudeSign_north;
      }
      else
      {
        q_temp = (uint32) ( 0 - breach_pos_fix.latitude );
        q_temp >>= 1;
        q_temp |= 0x80000000;

        z_supl_ReportData.positionData.position.positionEstimate.latitude
          = (OSUINT32)q_temp;
      }

      /* LONG: unit for longitude from GM breach (of the type of tm_lr_resp_type):
         180/2^25 degrees, 2's compliment; obtain positionEstimate.longitude
         (GAD) from degrees: * 2^23 / 180 -- so to convert from GM breach to
         positionEsimate.longitude: /4
       */
        z_supl_ReportData.positionData.position.positionEstimate.longitude =
          breach_pos_fix.longitude >>2;


        /* ALTITUDE : meters */
        if ( breach_pos_fix.height_incl )
        {
          z_supl_ReportData.positionData.position.positionEstimate.m.altitudeInfoPresent = TRUE;

          if ( (breach_pos_fix.height - 500) >= 0 )
          {
            z_supl_ReportData.positionData.position.positionEstimate.altitudeInfo.altitude =
              breach_pos_fix.height - 500; 

            z_supl_ReportData.positionData.position.positionEstimate.altitudeInfo.altitudeDirection =
              supl_AltitudeInfo_altitudeDirection_height;
          }
          else
          {
            q_temp = (uint32) ( 0 - (breach_pos_fix.height - 500) );
            z_supl_ReportData.positionData.position.positionEstimate.altitudeInfo.altitude =
             q_temp;

            z_supl_ReportData.positionData.position.positionEstimate.altitudeInfo.altitudeDirection =
              supl_AltitudeInfo_altitudeDirection_depth;

          }

        } /* if altitude included */

        /* TBD: unc. velocity, etc. */

        /* mandatory timestamp */
        memset( timestamp, 0, 14 );

        #if 0  /* test code */
        w_FixWn = 1637;
        q_fix_time_gps_msec = 525981;
        q_fix_time_utc_msec =  q_fix_time_gps_msec;
        #endif

        qw_equ(qword_timestamp, breach_pos_fix.timestamp);

        //t_TS = *((uint64 *)((void *)p_gm_client_breach_notify->pos_fix.timestamp));
        t_TS = *((uint64 *)( (void *)&qword_timestamp ));

        /* Section of code leverated from mc_pgi.c mc_pgi_tt_data_proc() */
        /* Compute the number of GPS weeks elapsed */
        w_FixWn = (uint16)((t_TS >> 16) / WEEK_1P25MSECS);
        /* Compute Time of Week in units of 1.25msec. */
        /*lint -e{647} milliseconds of week in 0.25msec unit can be fit into 32-bit integer */
        q_Gps0P25Msec = (uint32)(((t_TS >> 16) - (w_FixWn * WEEK_1P25MSECS)) * 5);
        /* Convert the TimeofWeek to millisecods of the week. */
        q_fix_time_gps_msec = q_Gps0P25Msec >> 2;

        q_fix_time_utc_msec = q_fix_time_gps_msec - 15000; /* 15 sec. lead time FOR NOW */
        z_utc_time = tm_util_construct_utc_time(w_FixWn,
                                                q_fix_time_gps_msec,
                                                  q_fix_time_utc_msec );

        /* 2 digit year */
        z_utc_time.q_year = z_utc_time.q_year % 100;

        /* 10th digit year converted to ASCII */
        timestamp[0] = z_utc_time.q_year / 10 + 0x30;

        /* 1th digit year to ASCII */
        timestamp[1] = z_utc_time.q_year -
                       ( (uint32)( z_utc_time.q_year / 10 ) * 10  ) + 0x30;

        /* 10th digit month to ASCII */
        timestamp[2] = z_utc_time.q_month / 10 + 0x30;

        /* 1th digit month to ASCII */
        timestamp[3] = z_utc_time.q_month -
                       ( (uint32)( z_utc_time.q_month / 10 ) * 10 ) + 0x30;

        /* 10th digit day to ASCII */
        timestamp[4] = z_utc_time.q_utc_days / 10 + 0x30;

        /* 1th digit day to ASCII */
        timestamp[5] = z_utc_time.q_utc_days -
                       ( (uint32)( z_utc_time.q_utc_days / 10 ) * 10 ) + 0x30;

        /* 10th digit hour to ASCII */
        timestamp[6] = z_utc_time.q_hours / 10 + 0x30;

        /* 1th digit hour to ASCII */
        timestamp[7] = z_utc_time.q_hours -
                      ( (uint32)( z_utc_time.q_hours / 10 ) * 10 ) + 0x30;

        /* 10th digit minute to ASCII */
        timestamp[8] = z_utc_time.q_mins / 10 + 0x30;

        /* 1th digit minute to ASCII */
        timestamp[9] = z_utc_time.q_mins -
                       ( (uint32)( z_utc_time.q_mins / 10 ) * 10 ) + 0x30;

        /* 10th digit second to ASCII */
        timestamp[10] = z_utc_time.q_secs / 10 + 0x30;

        /* 1th digit second to ASCII */
        timestamp[11] = z_utc_time.q_secs -
                        ( (uint32)( z_utc_time.q_secs / 10 ) * 10 ) + 0x30;

        timestamp[12]='Z';

        z_supl_ReportData.positionData.position.timestamp = &timestamp[0];

    } /* if position available */

  }  /* for geofence breach report */

  encode_status = tm_osys_asn1_encode_pdu((void *)&pdu, &pdu_buff, PROTO_SUPL);

  if ( (encode_status != 0) || (pdu_buff.value == NULL ) || (pdu_buff.length == 0) )
  {
    /* ASN.1 encoding failed */
    if (pdu_buff.value != NULL )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SUPL ASN1 enc. err. Freeing: %d,%lx,%u",
            encode_status, pdu_buff.value, pdu_buff.length);

      tm_asn1_free_osys_buf( pdu_buff.value, PROTO_SUPL );
      pdu_buff.value = NULL;

    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SUPL ASN1 enc. err. No alloc.: %d,%lx,%u",
            encode_status, pdu_buff.value, pdu_buff.length);
    }
  }

  else
  {
    pd_comms_return_type ret_val;

    p_length = (uint8 *)pdu_buff.value;
    u_length_msb = (pdu_buff.length >> 8) & 0x00FF;
    u_length_lsb = pdu_buff.length & 0x00FF;

    *p_length++ = u_length_msb;
    *p_length = u_length_lsb;

    ret_val = tm_umts_up_supl_comm_write( (uint8 *)pdu_buff.value, pdu_buff.length);

    if ( ( ret_val != PDCOMM_RESULT_WAIT ) && ( ret_val != PDCOMM_RESULT_OK ) )
    {
      /* message sending failed */
      MSG_ERROR("SUPL: SuplReport attempt aborted", 0, 0, 0);
    }
    else
    {
      if ( p_gm_client_breach_notify != NULL )
      {

        if ( p_gm_client_breach_notify->is_last_report )
        {
          /* if known to be the last breach report, keep tcp on for downlink SUPL_END */
          supl_set_cf_state_area_event (C_SUPL_CF_STATE_UT3_LAST_SUPLREPORT, u_session_num);
          MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED,
                "Keep tcp link after SUPL_REPORT - last rprt: %u",
                p_gm_client_breach_notify->is_last_report);

        }
        else
        {
          if ( !(z_supl_session_status.e_cf_state & C_SUPL_CF_STATE_UT3_EXPECT_SUPLREPORT ) )
          {
            tm_umts_up_supl_comm_close_connection();
            MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED,
                  "Disc. after SUPL_REPORT sent: %u, %u",
                  p_gm_client_breach_notify->is_last_report,
                  z_supl_session_status.e_cf_state);

          }
          else
          {
            MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED,
                  "Do NOT disconn due to UT3_EXPECT_SUPLREPORT on: %u, %u",
                  p_gm_client_breach_notify->is_last_report,
                  z_supl_session_status.e_cf_state);
          }
        }
      }

      if ( p_hash != NULL ) /* response to sessionInfoQuery */
      {
        supl_set_cf_state(C_SUPL_CF_STATE_UT10_INFO_QUERY_END, 0);
        /* UT10: use the same NV item for UT2 here */
      }

      MSG_HIGH("SUPL_REPORT sent", 0, 0, 0);
      u_ret = TRUE;
    }

    /*Free the encoded PDU*/
    tm_asn1_free_osys_buf( pdu_buff.value, PROTO_SUPL );
    pdu_buff.value = NULL;

  }

  if (p_gm_client_breach_notify != NULL)
  {

    z_sm_gm_breach_ack_notify.geofence_id =
      z_supl_area_event_session_status[u_session_num].q_geofence_id;

    if ( u_ret ) /* ACK GM */
    {
      /* SUPL_REPORT for breach report has been sent from SM perspective */
      z_sm_gm_breach_ack_notify.accepted = TRUE;
      u_sm_gm_error_code = GEOFENCE_ERROR_NONE;
    }

    else /* NACK GM*/
    {
      /* SUPL_REPORT for breach report has failed to be sent from SM perspective */
      z_sm_gm_breach_ack_notify.accepted = FALSE;
      u_sm_gm_error_code = GEOFENCE_ERROR_UNSPECIFIED;
    }

    gm_breach_ack(&z_sm_gm_breach_ack_notify, &u_sm_gm_error_code);
  }

  return u_ret;

}

/*
******************************************************************************
* tm_umts_up_supl_gm_init
*
* Function description:
*
*   Regigster SUPL with the Geofence Module
*
* Parameters: None.
*
* Return value: None.
*
******************************************************************************
*/

void tm_umts_up_supl_gm_init(void)
{
  uint8 error_code;
  sm_gm_client_type client;
  sm_gm_client_geofences_handles_request_type gm_get_handles_request;
  sm_gm_set_client_config_type gfClientConfig = {0};

  client.client_id = GEOFENCE_CLIENT_SUPL;
  client.q_base_msg_id = TM_CORE_GM_MSG_ID_BASE;
  client.q_base_transaction_id = TM_UMTS_UP_GM_TRANSACTION_ID_BASE;
  client.q_client_thread_id = THREAD_ID_SM_TM;

  client.client_cap.client_capabilites = 
      GEOFENCE_CLIENT_POS_FIX_CAPABLE|GEOFENCE_CLIENT_GEOFENCE_SERVICE_REQ;
  client.client_cap.pos_fix_capability.pos_fix_method_cap = GEOFENCE_CLIENT_POS_FIX_AGNSS_UP_CAPABLE | GEOFENCE_CLIENT_POS_FIX_CELL_ID_CAPABLE;
  client.client_cap.pos_fix_capability.pos_fix_accuracy_cap = GEOFENCE_CLIENT_POS_FIX_ACCURACY_HIGH;
  client.client_cap.pos_fix_capability.pos_fix_accuracy = 2; /*Minimum accuracy can be as low as 2 meters*/
  client.client_cap.pos_fix_capability.pos_fix_power_cap = GEOFENCE_CLIENT_POS_FIX_POWER_PROFILE_2;
  client.client_cap.pos_fix_capability.high_power_proc_wakeup_cap = GEOFENCE_CLIENT_HIGH_POWER_PROC_WAKEUP_PROFILE_1;  
  client.client_cap.pos_fix_capability.nw_usage_cap = GEOFENCE_CLIENT_NW_USAGE_PROFILE_1;
  
  client.client_cap.pos_fix_capability.is_imp_location_notification_required = FALSE;
  client.client_cap.pos_fix_capability.pos_fix_wait_secs = 60; /*Geofence core shall wait for 60 seconds for the fix*/
  client.client_cap.pos_fix_capability.is_prior_fix_request_indication_required = FALSE; /*Prior fix request indication required*/
  client.client_cap.pos_fix_capability.prior_fix_request_indication_secs = 0; /*Prior fix request indication required 2mins beforehand*/

  gfClientConfig.q_mask = (SM_GM_CLIENT_CONFIG_MAX_GEOFENCES_MASK|
                           SM_GM_CLIENT_CONFIG_GNSS_POS_QOS_SESS_TIMEOUT_MASK |
                           SM_GM_CLIENT_CONFIG_CPI_REQUEST_RATE_MASK |
                           SM_GM_CLIENT_CHAL_GPS_ENV_BO_MIN_MASK |
                           SM_GM_CLIENT_CHAL_GPS_ENV_BO_MAX_MASK |
                           SM_GM_CLIENT_CHAL_GPS_ENV_MS_DIST_GPS_MASK |
                           SM_GM_CLIENT_CHAL_GPS_ENV_MS_DIST_CPI_MASK |
                           SM_GM_CLIENT_CONFIG_GNSS_UNAVAIL_AGGR_EXIT_MASK);
  
  gfClientConfig.q_max_geofences = 20;
  gfClientConfig.q_gnss_pos_qos_sess_timeout = GNSS_NV_EFS_SM_GM_GNSS_POS_QOS_SESS_TIMEOUT_DEFAULT;
  gfClientConfig.q_cpi_request_rate = GNSS_NV_EFS_SM_GM_CPI_REQUEST_RATE_DEFAULT;
  gfClientConfig.q_chal_gps_env_bo_min = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_BO_MIN_DEFAULT;
  gfClientConfig.q_chal_gps_env_bo_max = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_BO_MAX_DEFAULT;
  gfClientConfig.q_chal_gps_env_ms_dist_gps = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_MS_DIST_GPS_DEFAULT;
  gfClientConfig.q_chal_gps_env_ms_dist_cpi = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_MS_DIST_CPI_DEFAULT;   
  gfClientConfig.gnss_unavail_aggresive_exit = FALSE;

  if(gm_client_register(&client,&supl_gm_handle,&gfClientConfig,&error_code) == FAILED)
  {
    MSG_HIGH("Not able to add SUPL as Geofence client, error_code %d", error_code,0,0 );
  }
  else
  {
    gm_get_handles_request.client_handle =  supl_gm_handle;
    gm_get_handles_request.q_transaction_id =  supl_comm_getnext_gm_transaction_id();

    if ( gm_get_client_geofences_handles(&gm_get_handles_request, &error_code) == FAILED )
    {
      MSG_HIGH("gm_client handle get failed, error_code %d", error_code,0,0 );
    }
    else
    {
      MSG_HIGH("gm_client handle get successful, error_code %d", error_code,0,0 );
    }
  }

}

/*
******************************************************************************
* supl_session_query_proc
*
* Function description:
*
*   Process SUPL_INIT - SessionInfoQuery
*
* Parameters: p_hash: pointer to HASH.
*
* Return value: None.
*
******************************************************************************
*/

static void  supl_session_query_proc( uint8 *p_hash,
                                      supl_slp_session_id* p_provided_slp_session_id,
                                      uint32 q_supl_version )
{
  if ( tm_umts_up_supl_supl_report_prepare_or_tx(0, NULL, p_hash, p_provided_slp_session_id ) )
  {
    MSG_MED("SUPL_REPORT for query sent", 0, 0, 0);
  }
  else
  {
    MSG_MED("SUPL_REPORT for query pending", 0, 0, 0);
    memscpy( &z_supl_session_status.z_session_id.z_Slp_Sess_Id,
         sizeof(z_supl_session_status.z_session_id.z_Slp_Sess_Id),
             p_provided_slp_session_id, sizeof(*p_provided_slp_session_id));
    memscpy( &z_supl_session_status.supl_init_hash[0], sizeof(z_supl_session_status.supl_init_hash), p_hash, sizeof(z_supl_session_status.supl_init_hash) );
    z_supl_session_status.u_hash_valid = TRUE;
    z_supl_session_status.q_supl_version = q_supl_version;
  }

}
/*
******************************************************************************
* supl_session_is_emergency_session
*
* Function description:
*
*   Return if the SUPL INIT is for an emergency session and if ESLP was provided
*
* Parameters:
*
* Return value: TRUE/FALSE
*
******************************************************************************
*/

boolean  supl_session_is_emergency_session( void)
{
  if ( (z_supl_session_status.z_emergency_data.b_valid == TRUE) &&
       (z_supl_session_status.z_emergency_data.b_emergency_ni == TRUE))
  {
    return TRUE;
  }
  return FALSE;

}


/*
******************************************************************************
* tm_umts_up_is_supl_enabled
*
* Function description:
*
*   Return if SUPL is enabled or not
*
* Parameters:
*
* Return value: TRUE/FALSE
*
******************************************************************************
*/

boolean tm_umts_up_is_supl_enabled(void)
{

    if ( ( (z_umts_nv_item_ram_copy.gnss_supl_version >> 16) & 0x000000FF) == 0 )
    {
      return FALSE;
    }

  return TRUE;
}

#if defined (FEATURE_CGPS_LTE_CELLDB) && !defined (FEATURE_GNSS_LOW_MEMORY)
/*
******************************************************************************
* tm_umts_up_supl_lpp_tx_mgr_init
*
* Function description:
*
*   Register SUPL with the LPP transaction manager Module
*
* Parameters: None.
*
* Return value: None.
*
******************************************************************************
*/

void tm_umts_up_supl_lpp_tx_mgr_init(void)
{
  tm_lpp_tx_mgr_client_type client;

  if (  tm_core_get_lte_lpp_up_enable())
  {
    if ( b_supl_lpp_tx_mgr_init == FALSE )
    {
      client.client_id = TM_LPP_TX_MGR_CLIENT_TYPE_SUPL;
      client.client_cb_func = &tm_umts_up_supl_lpp_cb_func;
      if(tm_lpp_tx_mgr_client_register(&client,&supl_lpp_handle) == FALSE)
      {
        MSG_HIGH("Not able to add SUPL as LPP tx mgr client", 0,0,0 );
      }
      else
      {
        b_supl_lpp_tx_mgr_init = TRUE;
      }
    }
  }
  else
  {
    MSG_HIGH("LPP not enabled, SUPL not registered with LPP tx mgr",0,0,0);
  }
}


/*===========================================================================

FUNCTION tm_umts_up_supl_refresh_efs_nv

DESCRIPTION
  This function handles the refresh of EFS NV items. 
  
DEPENDENCIES: none.

RETURN VALUE: none

SIDE EFFECTS: none.

===========================================================================*/
void tm_umts_up_supl_refresh_efs_nv(void )
{

  uint8 u_gnss_lte_lpp_enable = 0;
  uint8 u_supl_udp_enable_select = 0;

  sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_TM_LPP_ENABLE, sizeof(uint8),
                          &u_gnss_lte_lpp_enable);

  if ( (u_gnss_lte_lpp_enable & PDSM_PA_LPP_IN_LTE_UP_ENABLE) != 0 )
  {
    tm_umts_up_supl_lpp_tx_mgr_init();
  }
  else
  {
    tm_umts_up_supl_lpp_tx_mgr_dereg();
  }

}


/*
******************************************************************************
* tm_umts_up_supl_lpp_tx_mgr_dereg
*
* Function description:
*
*   DeRegister SUPL with the LPP transaction manager Module
*
* Parameters: None.
*
* Return value: None.
*
******************************************************************************
*/
void tm_umts_up_supl_lpp_tx_mgr_dereg(void)
{
  if ( b_supl_lpp_tx_mgr_init == TRUE )
  {
    if(tm_lpp_tx_mgr_client_deregister(supl_lpp_handle) == FALSE)
    {
      MSG_HIGH("Not able to remove SUPL as LPP tx mgr client", 0,0,0 );
    }
    b_supl_lpp_tx_mgr_init = FALSE;
  }
}

/*===========================================================================
FUNCTION tm_umts_up_supl_valid_otdoa_meas_state

DESCRIPTION
  Check whether the SUPL state machine is in valid OTDOA measurement state to receive
  measurements.

PARAMETERS:   
  eCurrentCfState [IN] : Take-in the SUPL current state.
 
DEPENDENCIES: none.

RETURN VALUE: Return TRUE, if it's valid state to receive OTDOA measurements, 
              else FALSE.

SIDE EFFECTS: none.

===========================================================================*/
boolean tm_umts_up_supl_valid_otdoa_meas_state( supl_cf_state_enum_type const eCurrentCfState )
{
  if ( eCurrentCfState & (C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER |
                             C_SUPL_CF_STATE_OTDOA_MEAS_REQ | C_SUPL_CF_STATE_OTDOA_EARLY_RESPONSE_REQ) )
  {
    /* Expecting one RSTD measurement report from OTDOA */
    return ( TRUE );
  }
  else if ( eCurrentCfState & C_SUPL_CF_STATE_OTDOA_MUL_MEAS_RECV )
  {
    /* Expecting multiple RSTD measurements from OTDOA */
    return ( TRUE );
  }

  return ( FALSE );
}

/*
******************************************************************************
* tm_umts_up_supl_handle_otoda_data
*
* Function description:
*
*   Handle OTDOA measurement data
*
* Parameters: None.
*
* Return value: None.
*
******************************************************************************
*/
void tm_umts_up_supl_handle_otdoa_data( sm_loc_lte_otdoa_meas_info_type const *p_OtdoaData)
{
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;
  tm_supl_lpp_data_type                     *p_supl_lpp_data       = &(z_supl_session_status.z_supl_lpp_data);
  uint32 flags;
  tm_lpp_transaction_info_s_type *p_lpp_tx_info = NULL;
  tm_lpp_transaction_info_s_type            *p_lpp_tx_rli          = NULL;
  sm_OTDOA_ProvideLocationInformation const *p_otdoa_meas          = NULL;
  boolean                                   v_OtdoaRstd            = FALSE;

  if ( (NULL == p_OtdoaData) || (NULL == p_OtdoaData->p_meas_info) )
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "No RSTD measurements" );
    return;
  }
  p_otdoa_meas = p_OtdoaData->p_meas_info;

  if ( !(p_supl_lpp_data->q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_OTDOA_RCVD) ||
       (FALSE == tm_umts_up_supl_valid_otdoa_meas_state( p_supl_session_status->e_cf_state )) )
  {
    MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "TM_SUPL_LPP: OTDOA measurements are not requested. Flags %d. Just drop the measurements. CF state %d",
           p_supl_lpp_data->q_lpp_flags, p_supl_session_status->e_cf_state );
    return;
  }

  if ( (p_OtdoaData->u_SeqNum == 0) ||
       (p_OtdoaData->u_SeqNum > TM_LPP_MAX_OTDOA_RSTD_MEAS_DATA) ||
       (p_OtdoaData->u_MaxMessageNum >  TM_LPP_MAX_OTDOA_RSTD_MEAS_DATA) )
  {
    MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Can't take more than 3 RSTD meas. SeqNum=%u, MaxMsgNum=%u",
           p_OtdoaData->u_SeqNum, p_OtdoaData->u_MaxMessageNum );
    return;
  }

  MSG_MED( "TM_SUPL_LPP:Rcvd OTDOA meas. Otdoa valid flag %d, error flag %d, supl flags %d",
           p_otdoa_meas->m.otdoaSignalMeasurementInformationPresent,
           p_otdoa_meas->m.otdoa_ErrorPresent, p_supl_lpp_data->q_lpp_flags );

  if ( p_otdoa_meas->otdoaSignalMeasurementInformation.neighbourMeasurementList.n >= 1 )
  {
     MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "TM_LPP_UP: SUPL OTDOA: RSTD's Valid");
     v_OtdoaRstd = TRUE;
  }

  /****************************************************************************
   * OTDOA measurements should be processed. Validation/sanity successful.    *
   ***************************************************************************/

  if((p_supl_session_status->e_cf_state & (C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER |
                                               C_SUPL_CF_STATE_OTDOA_MEAS_REQ)) &&
     (!p_supl_session_status->z_supl_lpp_data.b_earlyResponse)) 

  {
    /* clear timer and call flow state */
    tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_OTDOA_MEAS_REQ );
  }

  flags = C_LPP_MODE_UE_OTDOA;
  if  ( (p_supl_lpp_data->q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_MEAS_RCVD) &&
        (p_supl_lpp_data->q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_COMBINE_AGNSS_OTDOA) )
  {
    flags |= C_LPP_AGPS_MODE_UE_ASSISTED;
  }
  else  if  ( !(p_supl_lpp_data->q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_AGNSS_RCVD)
              && (FALSE == p_supl_session_status->z_supl_lpp_data.b_earlyResponse) )
  {
    tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_GNSS_MEAS_RESP_TIMER );
  }

  if ( ( 1 == p_OtdoaData->u_SeqNum ) &&
       ( TRUE == p_supl_session_status->z_supl_lpp_data.b_earlyResponse ) &&
       ( FALSE == tm_umts_up_supl_r12earlyResponse_valid( flags, v_OtdoaRstd ) ) )
  {
     tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_OTDOA_EARLY_RESPONSE_REQ );
     MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "TM_LPP_UP: SUPL OTDOA: Drop Early RSP");
     /* If No OTDOA Meas/RSTD's at early timer, then drop it / return
        else OTDOA reported before EarlyTimer expiry, due to OTDOA engine unable to use AD data
             or bad AD data or engine state .Ensure the error is sent out after cancelling the CF state*/
     if ( TRUE == p_OtdoaData->earlyMeasurement ) 
     {  
        return;
     }
  }
  
  if ( ( 0!= (p_supl_session_status->e_cf_state & C_SUPL_CF_STATE_OTDOA_EARLY_RESPONSE_REQ ) ) &&
       ( 1 == p_OtdoaData->u_SeqNum ) )
  {
    /*Copy the lpp flags, we need to set it back after Early response transaction is complete.*/
    p_supl_lpp_data->q_lpp_earlyResponseflags = p_supl_lpp_data->q_lpp_flags;
  }

  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "TM_LPP_SUPL: OTDOA_SEQ = %d",p_OtdoaData->u_SeqNum);
  
  if ( p_OtdoaData->u_SeqNum == 1 )
  {
    /* Find the original LPP RLI tx */
    p_lpp_tx_rli = tm_lpp_tx_mgr_find_entry_by_refloc( flags, TM_LPP_TX_MGR_CLIENT_TYPE_SUPL );
    if ( p_lpp_tx_rli != NULL )
    {
      /* Copy the original LPP RLI tx */
      z_lpp_up_tx_rli = *p_lpp_tx_rli;
      MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "Copy original LPP RLI transaction 0x%lX",
             z_lpp_up_tx_rli.z_lpp_data_type.req_loc_type );
    }
    if (TRUE == p_supl_session_status->z_supl_lpp_data.b_earlyResponse)
    {
      tm_up_supl_add_r12earlyResponse_tx();
    }
  }
  else if ( p_OtdoaData->u_SeqNum > 1 )
  {
    /* This is second or third LPP PLI with OTDOA measurement to be sent */
    p_lpp_tx_rli = NULL;
    tm_lpp_tx_mgr_insert_entry( &(z_lpp_up_tx_rli.z_current_trans_id), &p_lpp_tx_rli, z_lpp_up_tx_rli.u_client_handle );
    if ( NULL == p_lpp_tx_rli )
  {
      MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Can't add RLI entry as transaction" );
      return;
  }

    /* Update the reqLocInfo type & measurement available to OTDOA only */
    p_lpp_tx_rli->e_messages_id = TM_LPP_MESG_REQ_LOC_INFO;
    p_lpp_tx_rli->b_pending_ue_tx = TRUE;
    p_lpp_tx_rli->z_lpp_data_type.req_loc_type = C_LPP_MODE_UE_OTDOA;

    MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "Add new RLI entry to transaction table 0x%lX",
           p_lpp_tx_rli->z_lpp_data_type.req_loc_type );
  }

  /*if Early Response processing then do not terminate the MEAS callflows. */
  if (!p_supl_session_status->z_supl_lpp_data.b_earlyResponse) 
  {
  /* If multiple RSTD measurements are expected, then enter a new state 'C_LPP_CP_CF_STATE_OTDOA_MUL_MEAS_RECV' & set the timer */
  if ( p_OtdoaData->u_SeqNum == p_OtdoaData->u_MaxMessageNum )
  {
    tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_OTDOA_MUL_MEAS_RECV );

    /* Signal OTDOA engine to stop generating measurements when all meas are received */
    tm_lpp_proc_otdoa_stop_req( p_supl_session_status->tm_active_session_handle, TM_STOP_REASON_COMPLETED, TM_PRTL_TYPE_UMTS_UP );
  }
  else
  {
    /* Start multiple RSTD measurement guard timer */
    supl_set_cf_state( C_SUPL_CF_STATE_OTDOA_MUL_MEAS_RECV, C_LPP_OTDOA_MUL_MEAS_MSEC );
  }
  }

  //SUPL_ACK
  if (p_OtdoaData->u_SeqNum != p_OtdoaData->u_MaxMessageNum)
  {
     if (TRUE != p_supl_session_status->wait_for_write_ack) 
     {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED,"TM_LPP_UP: SUPL OTDOA: Start Enqueuing Packets");
        p_supl_session_status->wait_for_write_ack = TRUE;
     }
  }

  /****************************************************************************
  * Assign the OTDOA measurements and process further                         *
  ***************************************************************************/
  p_supl_lpp_data->p_otdoa_data = (sm_loc_lte_otdoa_meas_info_type *)p_OtdoaData;

  if ( supl_enc_lpp_prov_loc( p_supl_session_status, flags ) == TRUE )
  {
    /* send SUPL POS with LPP Provide Location Info*/
    (void)supl_SuplPos_lpp_tx( p_supl_session_status, NULL );
  }
  else
  {
    p_supl_lpp_data->u_lpp_error_type = (uint8)LPP_ERROR_TYPE_LPP_ERROR;
    p_supl_lpp_data->u_lpp_error_code = (uint8)LPP_ERROR_UNDEF;
    if ( supl_pos_enc_lpp_error_tx( NULL, p_supl_session_status, &p_lpp_tx_info ) == FALSE )
    {
      MSG_ERROR( "TM_LPP_UP: Error encoding LPP Error message", 0, 0, 0 );
    }
    p_supl_lpp_data->u_lpp_error_type = 0;
    }


  if ( p_OtdoaData->u_SeqNum != p_OtdoaData->u_MaxMessageNum )
  {
    /* No more A-GNSS measurement to include after sent the first LPP PLI */
      if (p_OtdoaData->u_SeqNum != 1) 
      {
    p_supl_lpp_data->q_lpp_flags &= ~TM_SUPL_LPP_PRM_VALID;
      }
    /* Set to OTDOA only for 2nd & 3rd LPP PLI for send only OTDOA data */
    p_supl_lpp_data->q_lpp_flags &= ~(TM_SUPL_LPP_REQ_LOC_INFO_AGNSS_RCVD | TM_SUPL_LPP_REQ_LOC_INFO_COMBINE_AGNSS_OTDOA);
    p_supl_lpp_data->q_lpp_flags |= TM_SUPL_LPP_REQ_LOC_INFO_OTDOA_RCVD;
  }
  else
  {
    /* After sending the last OTDOA measurement remove the OTDOA receive flag to allow back-to-back LPP RLI */
    p_supl_lpp_data->q_lpp_flags &= ~TM_SUPL_LPP_REQ_LOC_INFO_OTDOA_RCVD;
     if (TRUE == p_supl_session_status->z_supl_lpp_data.b_earlyResponse)
     {
         /*Restore Lpp flags after early response.*/
         p_supl_lpp_data->q_lpp_flags = p_supl_lpp_data->q_lpp_earlyResponseflags;
         MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "END early response send, clear 2timer, restore lpp flags");
         if ( p_supl_session_status->e_cf_state & (C_SUPL_CF_STATE_OTDOA_EARLY_RESPONSE_REQ) )
         {
            tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_OTDOA_EARLY_RESPONSE_REQ );
         }
  }
}

}

/*
******************************************************************************
* tm_umts_up_supl_lpp_get_cap
*
* Function description:
*
*   Returns the Capabilities  to the LPP Tx manager
*
* Parameters: None.
*
* Return value: None.
*
******************************************************************************
*/
tm_lpp_provide_cap_s_type *tm_umts_up_supl_lpp_get_cap(tm_lpp_transaction_info_s_type *p_lpp_tx_info)
{
  tm_cm_phone_state_info_s_type phone_state_info;

  tm_cm_iface_get_phone_state(&phone_state_info);

  MSG_HIGH("TM_UMTS_UP_SUPL: Get cap flags %d, ", z_lpp_provide_cap.q_flags, 0,0 );
  if ( phone_state_info.srv_system == CGPS_SRV_SYS_LTE )
  {
    if ( z_cell_info_cached.u_CachedCellIDValid == TRUE )
    {
      memscpy(&p_lpp_tx_info->z_lpp_data_type.z_cached_cell_info,
              sizeof(p_lpp_tx_info->z_lpp_data_type.z_cached_cell_info),
              &z_cell_info_cached, sizeof(z_cell_info_cached));
      p_lpp_tx_info->z_lpp_data_type.q_flags |= LPP_CELL_INFO_VALID;

    }
  }
  else
  {
    z_lpp_provide_cap.u_otdoa_supported = FALSE;
  }
  if ( p_lpp_tx_info->z_lpp_data_type.p_lpp_prov_cap_type != NULL )
  {

    MSG_HIGH("TM_UMTS_UP_SUPL: Get cap flags %d, %d ", z_lpp_provide_cap.q_flags,
                           p_lpp_tx_info->z_lpp_data_type.p_lpp_prov_cap_type->q_flags,0 );
    if ( p_lpp_tx_info->z_lpp_data_type.p_lpp_prov_cap_type->q_flags & LPP_REQ_CAP_AGNSS_REQ)
    {
      z_lpp_provide_cap.q_flags = LPP_REQ_CAP_AGNSS_REQ;
    }
    if ( p_lpp_tx_info->z_lpp_data_type.p_lpp_prov_cap_type->q_flags & LPP_REQ_CAP_OTDOA_REQ)
    {
      z_lpp_provide_cap.q_flags |= LPP_REQ_CAP_OTDOA_REQ;
    }
  }

  return &z_lpp_provide_cap;
}

/*
******************************************************************************
* tm_umts_up_supl_lpp_get_ad_list
*
* Function description:
*
*   Returns the built GPS & Glonass AD & wishlist based on supl session status
*   to the LPP Tx manager.
*
* Parameters: None.
*
* Return value: None.
*
******************************************************************************
*/
void tm_umts_up_supl_lpp_get_ad_list(tm_lpp_transaction_info_s_type *p_lpp_tx_info)
{
  tm_cm_phone_state_info_s_type phone_state_info;

  if ( NULL == p_lpp_tx_info )
  {
    return;
  }

  memset( &phone_state_info, 0, sizeof(phone_state_info) );
  tm_cm_iface_get_phone_state( &phone_state_info ); 

  p_lpp_tx_info->z_lpp_data_type.p_lpp_prov_cap_type = tm_umts_up_supl_lpp_get_cap(p_lpp_tx_info);

  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "SUPL wishlist: 0x%lX (GPS), 0x%lX (GLONASS)",
        z_supl_session_status.z_OrigWishList.q_WishListMask,
        z_supl_session_status.z_OrigWishList.z_GloAssistData.q_WishListMask );

  // Copy over base supl_session_status AD & wishlist data
  p_lpp_tx_info->z_lpp_data_type.p_AssistData = &z_supl_session_status.z_OrigWishList;

  if ( z_supl_session_status.u_agps_mode == C_SUPL_AGPS_MODE_UE_BASED )
  {
    // Gps
    if ( p_lpp_tx_info->z_lpp_data_type.p_AssistData->q_WishListMask & C_GPS_ASSIS_NAVMODL ||
         p_lpp_tx_info->z_lpp_data_type.p_AssistData->q_WishListMask & C_GPS_ASSIS_ALM )
    {
      p_lpp_tx_info->z_lpp_data_type.p_AssistData->q_WishListMask |= C_GPS_ASSIS_REFTIME;
    }

    // Don't request GLO AD if UE's Glonass capability is disabled or H-SLP indicated no GLONASS support in UI call flow
    if ( (FALSE == tm_umts_common_glonass_enabled()) || (FALSE == tm_up_slp_support_glonass(&z_supl_session_status)) )
    {
      MSG( MSG_SSID_GPSSM, MSG_LEGACY_MED, "No GLO AD requested. UE or H-SLP not capable of glonass" );
    }
    else
    {
      if ( p_lpp_tx_info->z_lpp_data_type.p_AssistData->z_GloAssistData.q_WishListMask & C_GNSS_ASSIST_NAVMODL )
      {
        // request AuxInfo when NavModel (ephemeris) is requested. AuxInfo used to associate Glonass channelNumber & SV-ID
        p_lpp_tx_info->z_lpp_data_type.p_AssistData->z_GloAssistData.q_WishListMask |= C_GNSS_ASSIST_AUXI_INFO;
      }
    }
  }
  else if ( z_supl_session_status.u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED )
  {
    p_lpp_tx_info->z_lpp_data_type.p_AssistData->q_WishListMask |= C_GPS_ASSIS_ACQ;
    p_lpp_tx_info->z_lpp_data_type.p_AssistData->q_WishListMask |= C_GPS_ASSIS_REFTIME;

    // Don't request GLO AD if UE's Glonass capability is disabled or H-SLP indicated no GLONASS support in UI call flow
    if ( (FALSE == tm_umts_common_glonass_enabled()) || (FALSE == tm_up_slp_support_glonass(&z_supl_session_status)) )
    {
      MSG( MSG_SSID_GPSSM, MSG_LEGACY_MED, "No GLO AD requested. UE or H-SLP not capable of glonass" );
    }
    else
    {
      p_lpp_tx_info->z_lpp_data_type.p_AssistData->z_GloAssistData.q_WishListMask |= C_GNSS_ASSIST_ACQ;
      // request AuxInfo to correllate the AcqAssist SvId to FreqNum/channelNumber
      p_lpp_tx_info->z_lpp_data_type.p_AssistData->z_GloAssistData.q_WishListMask |= C_GNSS_ASSIST_AUXI_INFO;
      p_lpp_tx_info->z_lpp_data_type.p_AssistData->z_GloAssistData.q_WishListMask |= C_GNSS_ASSIST_TIME_MODELS; // Primary AD to get Glonass time offset
      // Request _GPS_ TimeModel & UTCModel for A-Glonass MSA session
      p_lpp_tx_info->z_lpp_data_type.p_AssistData->q_WishListMask |= C_GPS_ASSIS_UTC;       // Secondary AD to get Glonass time offset
    }
  }

  if ( z_supl_session_status.u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED ||
      z_supl_session_status.u_agps_mode == C_SUPL_OTDOA_MODE )
  {
    if ( phone_state_info.srv_system == CGPS_SRV_SYS_LTE )
    {
      if (z_supl_session_status.z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_OTDOA_DELIVERED)
      {
        p_lpp_tx_info->z_lpp_data_type.b_otdoa_needed =  FALSE;
      }
      else
      {
        p_lpp_tx_info->z_lpp_data_type.b_otdoa_needed = TRUE;
      }

      if ( z_cell_info_cached.u_CachedCellIDValid == TRUE )
      {
        memscpy(&p_lpp_tx_info->z_lpp_data_type.z_cached_cell_info,
                sizeof(p_lpp_tx_info->z_lpp_data_type.z_cached_cell_info),
                &z_cell_info_cached, sizeof(z_cell_info_cached));
        p_lpp_tx_info->z_lpp_data_type.q_flags |= LPP_CELL_INFO_VALID;
      }
    }
    else
    {
      p_lpp_tx_info->z_lpp_data_type.b_otdoa_needed = FALSE;
      p_lpp_tx_info->z_lpp_data_type.q_flags &= ~LPP_CELL_INFO_VALID;
    }
  }

  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Revised LPP wishlist: 0x%lX (GPS), 0x%lX (GLONASS)",
        p_lpp_tx_info->z_lpp_data_type.p_AssistData->q_WishListMask,
        p_lpp_tx_info->z_lpp_data_type.p_AssistData->z_GloAssistData.q_WishListMask );
}


/*===========================================================================
FUNCTION tm_umts_up_supl_set_otdoa_tx_fields

DESCRIPTION
  Set or populate the OTDOA/RSTD measurement related fields in the transaction element.

PARAMETERS:   
  p_lpp_tx_info [IN] : Transaction element.
 
DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
void tm_umts_up_supl_set_otdoa_tx_fields( tm_lpp_transaction_info_s_type *const p_lpp_tx_info )
{
  /* OTDOA data valid? Copy the OTDOA data from SUPL session struct to the current transaction element */
  p_lpp_tx_info->z_lpp_data_type.b_earlyResponse_needed = z_supl_session_status.z_supl_lpp_data.b_earlyResponse;
  if ( z_supl_session_status.z_supl_lpp_data.p_otdoa_data != NULL )
  {
    p_lpp_tx_info->z_lpp_data_type.p_otoda_data = z_supl_session_status.z_supl_lpp_data.p_otdoa_data;

    if ( z_supl_session_status.z_supl_lpp_data.u_lpp_error_type & LPP_ERROR_TYPE_PROV_LOC_OTDOA_ERROR )
    {
      p_lpp_tx_info->z_lpp_data_type.lpp_error.u_lpp_error_type =  z_supl_session_status.z_supl_lpp_data.u_lpp_error_type;
      p_lpp_tx_info->z_lpp_data_type.lpp_error.u_lpp_error_value =  z_supl_session_status.z_supl_lpp_data.u_lpp_error_code;
      p_lpp_tx_info->z_lpp_data_type.q_flags |= LPP_ERROR_DATA_VALID;
    }
  }
  else
  {
    if ( z_supl_session_status.z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_OTDOA_DELIVERED )
    {
      p_lpp_tx_info->z_lpp_data_type.lpp_error.u_lpp_error_type =  z_supl_session_status.z_supl_lpp_data.u_lpp_error_type;
      p_lpp_tx_info->z_lpp_data_type.lpp_error.u_lpp_error_value =  z_supl_session_status.z_supl_lpp_data.u_lpp_error_code;
    }
    else
    {
      p_lpp_tx_info->z_lpp_data_type.lpp_error.u_lpp_error_type = z_supl_session_status.z_supl_lpp_data.u_lpp_error_type = LPP_ERROR_TYPE_PROV_LOC_OTDOA_ERROR;
      if ( z_supl_session_status.u_agps_mode == C_SUPL_AGPS_MODE_UE_BASED )
      {
        p_lpp_tx_info->z_lpp_data_type.lpp_error.u_lpp_error_value =  z_supl_session_status.z_supl_lpp_data.u_lpp_error_code = LPP_OTDOA_CAUSE_UNDEF;
      }
      else
      {
        p_lpp_tx_info->z_lpp_data_type.lpp_error.u_lpp_error_value =  z_supl_session_status.z_supl_lpp_data.u_lpp_error_code = LPP_OTDOA_CAUSE_AD_MISSING;
      }
    }
    p_lpp_tx_info->z_lpp_data_type.q_flags |= LPP_ERROR_DATA_VALID;
    p_lpp_tx_info->z_lpp_data_type.p_otoda_data = NULL;
  }
}

/*===========================================================================
FUNCTION tm_umts_up_supl_set_gnss_meas_tx_fields

DESCRIPTION
  Set or populate the GNSS measurement related fields in the transaction element

PARAMETERS:   
  p_lpp_tx_info [IN] : Transaction element.
 
DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
void tm_umts_up_supl_set_gnss_meas_tx_fields( tm_lpp_transaction_info_s_type *const p_lpp_tx_info )
{
  /*check for EarlyResponse flag and make sure we copy it.*/
  p_lpp_tx_info->z_lpp_data_type.b_earlyResponse_needed = z_supl_session_status.z_supl_lpp_data.b_earlyResponse;
  /* Measurement report valid ? */
  if ( z_supl_session_status.z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_PRM_VALID )
  {
    /* Atleast 1 measurement system type (GPS/Glonass) should be present. If GNSS measurement
    *  doesn't have valid measurement, set appropiate error code & check for OTDOA data
    */
    if ( 0 == z_supl_session_status.z_gnss_meas_report.u_meas_reports_bmap )
    {
      MSG_ERROR("TM_UMTS_UP_SUPL_LPP: Atleast GPS measurement should be available in supl status", 0, 0, 0);
    }
    else
    {
      p_lpp_tx_info->z_lpp_data_type.q_flags |= LPP_PRM_DATA_VALID;

      /* Point to Gnss measurement report struct */
      p_lpp_tx_info->z_lpp_data_type.p_gnss_meas_report = &z_supl_session_status.z_gnss_meas_report;
      p_lpp_tx_info->z_lpp_data_type.b_status_check_for_prm_data = z_supl_session_status.z_supl_lpp_data.b_status_check_needed;
    }
  }
  else
  {
    /* First preference given to error code & type set while decoding/processing 'request location info' msg in
   supl_lpp_req_loc_info_proc() function. If error type not set, then write with LPP_ERROR_TYPE_PROV_LOC_AGNSS_ERROR */
    if ( LPP_ERROR_NO_ERR == z_supl_session_status.z_supl_lpp_data.u_lpp_error_type )
    {
      p_lpp_tx_info->z_lpp_data_type.lpp_error.u_lpp_error_type = z_supl_session_status.z_supl_lpp_data.u_lpp_error_type = LPP_ERROR_TYPE_PROV_LOC_AGNSS_ERROR;
    }
    else
    {
      p_lpp_tx_info->z_lpp_data_type.lpp_error.u_lpp_error_type = z_supl_session_status.z_supl_lpp_data.u_lpp_error_type;
    }
    p_lpp_tx_info->z_lpp_data_type.lpp_error.u_lpp_error_value =  z_supl_session_status.z_supl_lpp_data.u_lpp_error_code;
    p_lpp_tx_info->z_lpp_data_type.q_flags |= LPP_ERROR_DATA_VALID;
  }
}
 
/*
******************************************************************************
* tm_umts_up_supl_lpp_prov_loc_info
*
* Function description: Returns the Provide location info to the LPP Tx manager
*                       after populating LPP transaction info struct (from
*                       z_supl_session_status) based on various criteria.
* Parameters:
*      p_lpp_tx_info: pointer to tm_lpp_transaction_info_s_type LPP transaction
*                     RLI structure, to be used to fill in SUPL data for
*                     ProvLocInfo 
*
* Return value: None.
*
******************************************************************************
*/
void tm_umts_up_supl_lpp_prov_loc_info(tm_lpp_transaction_info_s_type *p_lpp_tx_info)
  {
  /* Initialize */
  p_lpp_tx_info->z_lpp_data_type.q_flags = 0;
  p_lpp_tx_info->z_lpp_data_type.p_gnss_meas_report = NULL;

  /* check if there is an error */
  if ( z_supl_session_status.z_supl_lpp_data.u_lpp_error_type )
  {
        p_lpp_tx_info->z_lpp_data_type.lpp_error.u_lpp_error_type =  z_supl_session_status.z_supl_lpp_data.u_lpp_error_type ;
    p_lpp_tx_info->z_lpp_data_type.lpp_error.u_lpp_error_value =  z_supl_session_status.z_supl_lpp_data.u_lpp_error_code;
    p_lpp_tx_info->z_lpp_data_type.q_flags |= LPP_ERROR_DATA_VALID;
  }

  switch ( z_supl_session_status.u_agps_mode )
  {
  case C_SUPL_AGPS_MODE_UE_ASSISTED:
    p_lpp_tx_info->u_pos_mode = LPP_POS_MODE_UE_ASSISTED;
    break;
  case C_SUPL_AGPS_MODE_UE_BASED:
    p_lpp_tx_info->u_pos_mode = LPP_POS_MODE_UE_BASED;
    break;
  case C_SUPL_OTDOA_MODE:
    p_lpp_tx_info->u_pos_mode = LPP_POS_MODE_OTDOA_ONLY;
    break;
  default:
    p_lpp_tx_info->u_pos_mode = LPP_POS_MODE_STANDALONE;
    break;
  }

  MSG_HIGH("TM_UMTS_UP_SUPL_LPP: Req Loc info type %d, flags 0x%lX, Mode %d",
           p_lpp_tx_info->z_lpp_data_type.req_loc_type,
           z_supl_session_status.z_supl_lpp_data.q_lpp_flags, z_supl_session_status.u_agps_mode);

  /*** Decide what to send in ProvLocInfo message based on the ProvideLocatioInfo AD & ReqLocInfo message from LS ***/

  /* if ReqLocInfo contained AGNSS request loc. and otdoa in the same message then send AGNSS and OTDOA measurements
     in the ProvLocInfo */
  if ( ((z_supl_session_status.u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED) ||
    (z_supl_session_status.u_agps_mode == C_SUPL_OTDOA_MODE)) &&
       (z_supl_session_status.z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_COMBINE_AGNSS_OTDOA) &&
       (p_lpp_tx_info->z_lpp_data_type.req_loc_type & (C_LPP_AGPS_MODE_UE_ASSISTED | C_LPP_MODE_UE_OTDOA)) )
  {
    p_lpp_tx_info->z_lpp_data_type.q_flags |= LPP_ENC_PRM_DATA;
    p_lpp_tx_info->z_lpp_data_type.q_flags |= LPP_ENC_OTDOA_DATA;

    tm_umts_up_supl_set_gnss_meas_tx_fields(p_lpp_tx_info);

    tm_umts_up_supl_set_otdoa_tx_fields(p_lpp_tx_info);
    }
  /* if REQ loc info contained AGNSS request loc. and AGNSS is requested
     message then send AGNSS Measurements in the prov loc info */
  else if ( ((z_supl_session_status.u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED) ||
    (z_supl_session_status.u_agps_mode == C_SUPL_OTDOA_MODE)) &&
            (z_supl_session_status.z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_AGNSS_RCVD) &&
            (p_lpp_tx_info->z_lpp_data_type.req_loc_type & C_LPP_AGPS_MODE_UE_ASSISTED) )
  {
    p_lpp_tx_info->z_lpp_data_type.q_flags |= LPP_ENC_PRM_DATA;
    p_lpp_tx_info->z_lpp_data_type.q_flags &= ~LPP_ENC_OTDOA_DATA;

    tm_umts_up_supl_set_gnss_meas_tx_fields(p_lpp_tx_info);
    }
  /* if REQ loc info contained OTDOA request loc.then send OTDOA in
     the prov loc info */
  else if ( (z_supl_session_status.z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_OTDOA_RCVD) &&
            (p_lpp_tx_info->z_lpp_data_type.req_loc_type & C_LPP_MODE_UE_OTDOA) )
  {
    p_lpp_tx_info->z_lpp_data_type.q_flags |= LPP_ENC_OTDOA_DATA;

    tm_umts_up_supl_set_otdoa_tx_fields(p_lpp_tx_info);
      }
  /* if REQ loc info contained Position request loc.then send Position in
     the prov loc info */

  if ( z_supl_session_status.u_agps_mode == C_SUPL_AGPS_MODE_UE_BASED ||
       p_lpp_tx_info->z_lpp_data_type.req_loc_type & C_LPP_AGPS_MODE_UE_BASED )
  {
    p_lpp_tx_info->z_lpp_data_type.b_earlyResponse_needed = z_supl_session_status.z_supl_lpp_data.b_earlyResponse;
    if ( z_supl_session_status.z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_AGNSS_RCVD )
    {
      if ( p_lpp_tx_info->z_lpp_data_type.req_loc_type & C_LPP_AGPS_MODE_UE_BASED )
      {
      p_lpp_tx_info->z_lpp_data_type.q_flags |= LPP_ENC_POS_DATA;
      }
      if ( z_supl_session_status.z_pending_new_pos_to_network.u_valid == TRUE ) // Got LR from TM-Core?
      {
        p_lpp_tx_info->z_lpp_data_type.q_flags  |= LPP_POS_DATA_VALID;
        p_lpp_tx_info->z_lpp_data_type.z_pos_info.q_pos_sources = z_supl_session_status.z_pending_new_pos_to_network.z_pos_info.q_pos_sources;

        if ( ( z_supl_session_status.z_pending_new_pos_to_network.z_pos_info.q_pos_sources & C_POS_SOURCE_GLO ) && // Glonass pos source ?
             !( z_supl_session_status.z_pending_new_pos_to_network.z_pos_info.q_pos_sources & ~C_POS_SOURCE_GLO ) )  // No other pos sources are used ?
        {
          p_lpp_tx_info->z_lpp_data_type.z_pos_info.q_GloTodMs = z_supl_session_status.z_pending_new_pos_to_network.z_pos_info.q_GloTimeMs;
        }

        p_lpp_tx_info->z_lpp_data_type.z_pos_info.z_pos = z_supl_session_status.z_pending_new_pos_to_network.z_pos_info.z_pos;

        p_lpp_tx_info->z_lpp_data_type.z_pos_info.f_VelEnu[0] = z_supl_session_status.z_pending_new_pos_to_network.z_pos_info.f_VelEnu[0];
        p_lpp_tx_info->z_lpp_data_type.z_pos_info.f_VelEnu[1] = z_supl_session_status.z_pending_new_pos_to_network.z_pos_info.f_VelEnu[1];
        p_lpp_tx_info->z_lpp_data_type.z_pos_info.f_VelEnu[2] = z_supl_session_status.z_pending_new_pos_to_network.z_pos_info.f_VelEnu[2];
        p_lpp_tx_info->z_lpp_data_type.z_pos_info.f_Vunc3dMps = z_supl_session_status.z_pending_new_pos_to_network.z_pos_info.f_Vunc3dMps;
        p_lpp_tx_info->z_lpp_data_type.z_pos_info.f_velVertUnc = z_supl_session_status.z_pending_new_pos_to_network.z_pos_info.f_VelVertUnc;
        p_lpp_tx_info->z_lpp_data_type.z_pos_info.q_fix_time_gps_msec = z_supl_session_status.z_pending_new_pos_to_network.z_pos_info.q_fix_time_gps_msec;
        p_lpp_tx_info->z_lpp_data_type.z_pos_info.q_fix_time_utc_msec = z_supl_session_status.z_pending_new_pos_to_network.z_pos_info.q_fix_time_utc_msec;
        if ( (z_supl_session_status.z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_VEL_REQUESTED)  ||
          (z_supl_session_status.z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_MAY_RET_ADD_INFO))
        {
          p_lpp_tx_info->z_lpp_data_type.z_pos_info.vel_valid = z_supl_session_status.z_pending_new_pos_to_network.z_pos_info.vel_valid;
        }
        else
        {
          p_lpp_tx_info->z_lpp_data_type.z_pos_info.vel_valid = FALSE;
        }
        p_lpp_tx_info->z_lpp_data_type.z_pos_info.w_Alt = z_supl_session_status.z_pending_new_pos_to_network.z_pos_info.w_Alt;
        p_lpp_tx_info->z_lpp_data_type.z_pos_info.q_Lat = z_supl_session_status.z_pending_new_pos_to_network.z_pos_info.q_Lat;
        p_lpp_tx_info->z_lpp_data_type.z_pos_info.l_Long = z_supl_session_status.z_pending_new_pos_to_network.z_pos_info.l_Long;
        p_lpp_tx_info->z_lpp_data_type.z_pos_info.w_GpsWeek = z_supl_session_status.z_pending_new_pos_to_network.z_pos_info.w_GpsWeek;
        p_lpp_tx_info->z_lpp_data_type.z_pos_info.f_ClockBias = z_supl_session_status.z_pending_new_pos_to_network.z_pos_info.f_ClockBias;
        p_lpp_tx_info->z_lpp_data_type.z_pos_info.f_ClockBiasUncMs = z_supl_session_status.z_pending_new_pos_to_network.z_pos_info.f_ClockBiasUncMs;
      }
      else
      {
        /* First preference given to error code & type set while decoding/processing 'request location info' msg in
      supl_lpp_req_loc_info_proc() function. If error type not set, then write with LPP_ERROR_TYPE_PROV_LOC_AGNSS_ERROR */
        if ( LPP_ERROR_NO_ERR == z_supl_session_status.z_supl_lpp_data.u_lpp_error_type )
        {
          p_lpp_tx_info->z_lpp_data_type.lpp_error.u_lpp_error_type = z_supl_session_status.z_supl_lpp_data.u_lpp_error_type = LPP_ERROR_TYPE_PROV_LOC_AGNSS_ERROR;
        }
        else
        {
          p_lpp_tx_info->z_lpp_data_type.lpp_error.u_lpp_error_type = z_supl_session_status.z_supl_lpp_data.u_lpp_error_type;
        }

        p_lpp_tx_info->z_lpp_data_type.lpp_error.u_lpp_error_value =  z_supl_session_status.z_supl_lpp_data.u_lpp_error_code;
        p_lpp_tx_info->z_lpp_data_type.q_flags |= LPP_ERROR_DATA_VALID;
      }
    }
  }

  MSG_HIGH("TM_UMTS_UP_SUPL: LPP flag %d, %d", z_supl_session_status.z_supl_lpp_data.q_lpp_flags,
     p_lpp_tx_info->z_lpp_data_type.q_flags,0);
  return;
}
#endif /* FEATURE_CGPS_LTE_CELLDB && !FEATURE_GNSS_LOW_MEMORY */

/*
******************************************************************************
* tm_umts_up_supl_rrlp_assisted_glonass_enabled
*
* Function description:
*
* Check if RRLP for assisted GLONASS is provisioned for SUPL only when UE's
* position protocol is RRLP.
* 
* Parameters: None
* 
* Return value: TRUE - RRLP for assisted GLONASS is provisioned for SUPL.
*               FALSE - RRLP for assisted GLONASS is not provisioned for SUPL.
*
******************************************************************************
*/
boolean tm_umts_up_supl_rrlp_assisted_glonass_enabled (void)
{
  if ( TM_POS_PRTL_RRLP == z_supl_session_status.u_position_protocol )
  {
    if ( tm_umts_common_glonass_enabled() &&
         (TM_CORE_CONFIG_DD( asst_glonass_pos_protocol ) & PDSM_PA_ASST_GLO_PROTOCOL_RRLP_UP) &&
         ((z_supl_session_status.q_supl_version & 0x000000FF ) ==  2 )
        )
    {
      return ( TRUE );
    }
  }

  MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Pos protocol %u", z_supl_session_status.u_position_protocol );
  return ( FALSE );
}

/********************************************************************
* supl_chk_if_external_modem_OOS
*
* Function description:
*   Processing cell db update from external sources coming in from pdapi.
*
* Parameters: pdsm_cell_info_s_type
*
* Return:
*  TRUE/FALSE
*
*********************************************************************
*/
boolean supl_chk_if_external_modem_OOS(const pdsm_cell_info_s_type external_cell_info)
{
  boolean external_celldb_OOS = FALSE;

  /* Checking two cases to verify if external modem is OOS
        1. When the serviceStatus says OOS or UNK
        2. When LAC and CID is -1. This happens when we receive an inject external cell update
            that indicate that we are camped on a non-gsm RAT. And for SGLTE that means its camped on
            the main modem. If RAT is non-gsm then LAC and CID will be -1
    */
  if (((external_cell_info.serviceStatus == STATUS_OUT_OF_SERVICE) ||
       (external_cell_info.serviceStatus == STATUS_UNKNOWN)) ||
      ((external_cell_info.cellInfo.gsmCell.LAC == HEX_NEGATIVE_ONE) &&
       (external_cell_info.cellInfo.gsmCell.CID == HEX_NEGATIVE_ONE)))
  {
    external_celldb_OOS = TRUE;
  }

  return external_celldb_OOS;
}


/********************************************************************
*
* tm_umts_up_supl_external_cell_db_update
*
*   Processing cell db update from external sources coming in from pdapi.
*
* Parameters:
*
* Return:
*  None
*
*********************************************************************
*/
void tm_umts_up_supl_external_cell_db_update( const pdsm_cell_info_s_type external_cell_info )
{

  static char s_Buf[256];

  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED,
            "Cached external cellDB valid : %u, New external cellDB valid : %u",
              !supl_chk_if_external_modem_OOS(z_external_cell_info_cached),
              !supl_chk_if_external_modem_OOS(external_cell_info));


  MSG_HIGH("External cell info: MCC = %d, MNC = %d, LAC = %d",
        external_cell_info.cellInfo.gsmCell.MCC,
        external_cell_info.cellInfo.gsmCell.MNC,
        external_cell_info.cellInfo.gsmCell.LAC);

  MSG_HIGH("External cell info: CID = %d, serviceStatus = %d, RAT = %d",
        external_cell_info.cellInfo.gsmCell.CID,
        external_cell_info.serviceStatus,
        external_cell_info.activeRAT);

  /* Currently support external cell updates for GSM RAT only */
  if ((external_cell_info.activeRAT != PDSM_ACTIVERAT_GSM) &&
    (external_cell_info.activeRAT != PDSM_ACTIVERAT_NONE))
  {
  MSG_HIGH("Currently SGLTE architecture supports only GSM cell updates",0,0,0);
  MSG_HIGH("External celldb not updated",0,0,0);
  return;
  }

  // Check if the cell-id information received from external source is valid
  if (supl_chk_if_external_modem_OOS(external_cell_info))
  {
  MSG_HIGH("External Cell update is INVALID/OOS",0,0,0);
  }
  else
  {
  // The cell-id information received from external source is valid
  MSG_HIGH("External Cell update is VALID",0,0,0);
  }

  // Update cached celldb info if valid or not
  memscpy(&z_external_cell_info_cached, sizeof(z_external_cell_info_cached),
          &external_cell_info, sizeof(external_cell_info) );


  if ( z_external_cell_info_cached.activeRAT == PDSM_ACTIVERAT_GSM )
  {
    MSG_5( MSG_SSID_GPSSM, MSG_LEGACY_MED,
           "Cached External Cell Updates for GSM CellDB: Valid=%u,MCC=%lu,MNC=%lu,LAC=%lu,CID=%lu\n",
           supl_chk_if_external_modem_OOS(z_external_cell_info_cached),
           z_external_cell_info_cached.cellInfo.gsmCell.MCC,
           z_external_cell_info_cached.cellInfo.gsmCell.MNC,
           z_external_cell_info_cached.cellInfo.gsmCell.LAC,
           z_external_cell_info_cached.cellInfo.gsmCell.CID);

  }


  MSG_HIGH("Cached external cell info: MCC = %d, MNC = %d, LAC = %d",
      z_external_cell_info_cached.cellInfo.gsmCell.MCC,
      z_external_cell_info_cached.cellInfo.gsmCell.MNC,
      z_external_cell_info_cached.cellInfo.gsmCell.LAC);

  MSG_HIGH("External cell info: CID = %d, serviceStatus = %d, RAT = %d",
      z_external_cell_info_cached.cellInfo.gsmCell.CID,
      z_external_cell_info_cached.serviceStatus,
      z_external_cell_info_cached.activeRAT);



  /************** for displaying diag. message only *********************/
  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "External sources updating CellDB for RAT: %u, validity = %u",
      z_external_cell_info_cached.activeRAT,
    !supl_chk_if_external_modem_OOS(z_external_cell_info_cached));
}


/********************************************************************
*
* tm_umts_up_supl_get_cell_db_type
*
* Function description:
*   Decides which cell db should be considered for SUPL messages from cell_db_type
*
* Parameters: None
*
* Return:
*  One of the cell db types defined in cell_db_type
*
*********************************************************************
*/
cell_db_type tm_umts_up_supl_get_cell_db_type(void)
{

  cell_db_type cell_db = CELL_DB_NONE;

  /* case where there are no external Cell db updates or external cell DB is invalid (OOS/UKN) */
  if ((supl_chk_if_external_modem_OOS(z_external_cell_info_cached))  &&
      (z_cell_info_cached.u_CachedCellIDValid == TRUE))
  {
    cell_db = CELL_DB_MAIN_MODEM;
  }
  /* case where we have external cell db updates and they are valid
        ( camped-roaming/camped-not-roaming). While the main modem's
        cell db could be valid/invalid, we prefer the external cell db */
  else if (!supl_chk_if_external_modem_OOS(z_external_cell_info_cached))
  {
  cell_db = CELL_DB_EXTERNAL_MODEM;
  }
  /* case where both cell DB are invalid */
  else if ((supl_chk_if_external_modem_OOS(z_external_cell_info_cached))  &&
           (z_cell_info_cached.u_CachedCellIDValid == FALSE))
  {
  /* cell_db is already set to CELL_DB_NONE  */
  }

  return cell_db;
}

/********************************************************************
*
* supl_session_status_reset
*
* Function description:
*   Use this common garbage collector function to reset the
*   z_supl_session_status global object & any pointers to avoid memory leaks.
*
*********************************************************************
*/
void supl_session_status_reset( void )
{
  /* Zeroing */
  memset(&z_supl_session_status, 0, sizeof(supl_status_s_type)); 

  #ifdef FEATURE_LTE
  b_lte_ota_otdoa_reset_flag = FALSE;
  #endif /* FEATURE_LTE */
}

/********************************************************************
*
* tm_umts_up_supl_ota_delete_proc
*
* Function description:
*   Process OTA Delete
*
*
*********************************************************************
*/
void tm_umts_up_supl_ota_delete_proc ( void)
{
  sm_gm_purge_request_type z_gm_purge_request;
  uint8 u_error_code = 0;

 (void) os_TimerStop(supl_alm_req_holdoff_timer);
 (void) os_TimerStop(supl_eph_req_holdoff_timer);
 (void) os_TimerStop(supl_iono_req_holdoff_timer);
 (void) os_TimerStop(supl_utc_req_holdoff_timer);
 (void) os_TimerStop(supl_refloc_req_holdoff_timer);
 (void) os_TimerStop(supl_gf_holdoff_timer);
 MSG_MED("OTA-delete resets SUPL holdoff timers", 0, 0, 0);

 MSG_MED("Clearing away NI Geofences",0,0,0);
 memset(&z_supl_area_event_session_status[0],
        0,
        sizeof (supl_area_event_status_s_type) * C_MAX_GEOFENCE_NUM );

 memset(&z_gm_purge_request, 0, sizeof(sm_gm_purge_request_type));

 z_gm_purge_request.client_handle = supl_gm_handle;
 z_gm_purge_request.q_transaction_id = supl_comm_getnext_gm_transaction_id();
 (void)gm_purge_geofence(&z_gm_purge_request, &u_error_code);
}


/********************************************************************
*
* supl_is_wifi_scan_needed
*
* Function description:
*   Check if WiFi scan is needed
*
*
*********************************************************************
*/
boolean supl_is_wifi_scan_needed (void)
{
  if ( tm_core_info.config_info.wifi_wait_timeout_sec == 0 )
  {
    return FALSE;
  }
  else
  {
    if ( z_supl_session_status.u_wifi_scan_requested == FALSE )
    {
      z_supl_session_status.u_wifi_scan_requested = TRUE;
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
}


/********************************************************************
*
* supl_request_wifi_scan
*
* Function description:
*   Request WiFi scan
*
*
*********************************************************************
*/

void supl_request_wifi_scan(void)
{

  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;
  tm_pdapi_pd_event_s_type   event_info;

  memset (&event_info, 0, sizeof(event_info));

  event_info.e_pd_event = PDSM_PD_EVENT_REQUEST_WIFI_INFO_ON_DEMAND_INJECTION;
  event_info.pd_event_data.pd_info_type.client_id = p_supl_session_status->q_pdapi_client_id;

  tm_pdapi_pd_event_callback(&event_info, TM_PDAPI_SESS_TYPE_NONE);

  MSG_MED("WiFi scan request posted", 0, 0, 0);
}



/********************************************************************
*
* tm_umts_up_supl_wifi_scan_report_proc
*
* Function description:
*   Process WIFI scan result
*
*
*********************************************************************
*/

void tm_umts_up_supl_wifi_scan_report_proc  (prtl_wifi_scan_event_s_type z_wifi_scan_result)
{
  uint8 u_i;
  locEngWifiApInfoStructT_v01 *wifi_ap_ptr;
  cgps_CellInfoCachedStruct z_cgps_CellInfoCached;
  tm_pdapi_pd_event_s_type   event_info;


  memset (&event_info, 0, sizeof(event_info));
  event_info.e_pd_event = PDSM_PD_EVENT_ACK_WIFI_INFO_ON_DEMAND_INJECTION;
  event_info.pd_event_data.pd_info_type.client_id = z_supl_session_status.q_pdapi_client_id;


  if ( z_wifi_scan_result.q_num_wifi_aps == 0 ||
       z_wifi_scan_result.first_wifi_ap_ptr == NULL ||
       tm_core_info.config_info.wifi_wait_timeout_sec == 0 )
  {
    event_info.pd_event_data.pd_info_type.pd_info.wifi_inj_ack.status = eLOC_ENG_GENERAL_FAILURE_V01;
    tm_pdapi_pd_event_callback(&event_info, TM_PDAPI_SESS_TYPE_NONE);
    MSG_ERROR("Invalid Wifi AP info", 0, 0, 0);
    return;
  }

  wifi_ap_ptr = z_wifi_scan_result.first_wifi_ap_ptr;
  for ( u_i=0; u_i < z_wifi_scan_result.q_num_wifi_aps; u_i++ )
  {
    MSG_9(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "WIFI_AP %u, Mask %x, MAC: %u %u %u %u %u %u, AP_RSSI %d",
          u_i, wifi_ap_ptr->wifiInfoMask,
          wifi_ap_ptr->mac_address[5], wifi_ap_ptr->mac_address[4], wifi_ap_ptr->mac_address[3],
          wifi_ap_ptr->mac_address[2], wifi_ap_ptr->mac_address[1], wifi_ap_ptr->mac_address[0],
          wifi_ap_ptr->ap_rssi);


    memset(&z_cgps_CellInfoCached, 0, sizeof(z_cgps_CellInfoCached) );
    z_cgps_CellInfoCached.u_CachedCellIDValid = TRUE;
    z_cgps_CellInfoCached.e_ActiveRAT = CELLDB_WIFI;
    memscpy(&z_cgps_CellInfoCached.z_CellInfoGlobalCached.z_wifi_ap_cell_info,
            sizeof(z_cgps_CellInfoCached.z_CellInfoGlobalCached.z_wifi_ap_cell_info),
            wifi_ap_ptr,
            sizeof(locEngWifiApInfoStructT_v01) );
    tm_umts_up_supl_update_mlid_db (&z_cgps_CellInfoCached);


    wifi_ap_ptr++;
  }

  event_info.pd_event_data.pd_info_type.pd_info.wifi_inj_ack.status = eLOC_ENG_SUCCESS_V01;
  tm_pdapi_pd_event_callback(&event_info, TM_PDAPI_SESS_TYPE_NONE);


  if ( ( z_supl_session_status.e_cf_state & C_SUPL_CF_STATE_WIFI_WAIT_SUPLSTART ) ||
       ( z_supl_session_status.e_cf_state & C_SUPL_CF_STATE_WIFI_WAIT_TIMER_SUPLSTART )
     )
  {
    /* Got WiFi cell info, need to send SUPL_START now */
    MSG_MED("Sending SUPL_START with WIFI info as WIFI info. arrives at state of %x", z_supl_session_status.e_cf_state, 0, 0);
    if ( !supl_SuplStart_tx())
    {
      tm_umts_up_supl_callflow_disrupt_handler();
    }

    /*If SUPL START is succesful, then cftimer is now set to UT1,
      If SUPL START is unsuccesful, then cftimer is stopped.
      So need not take care of cftimer, just cancel the UT0 state if still there
    */
    z_supl_session_status.e_cf_state &= (supl_cf_state_enum_type)~C_SUPL_CF_STATE_UT0_SUPLSTART;
  }

  if ( ( z_supl_session_status.e_cf_state & C_SUPL_CF_STATE_WIFI_WAIT_SUPLPOSINIT ) ||
       ( z_supl_session_status.e_cf_state & C_SUPL_CF_STATE_WIFI_WAIT_TIMER_SUPLPOSINIT )
     )
  {

    if ( !supl_SuplPosInit_tx())
    {
      tm_umts_up_supl_callflow_disrupt_handler();
    }
    else /*SuplPosInit Sent*/
    {
      if( z_supl_session_status.u_position_protocol == TM_POS_PRTL_IS801 &&
          z_supl_session_status.u_callflow_kind == C_SUPL_SESSION_NI)
      {
        /*Start IS801 state machine*/
        if( !supl_start_is801_session(TRUE, NULL))
        {
          /*Unable to start IS801 state machine*/
          tm_umts_up_supl_callflow_disrupt_handler();
        }
      }
    }
    /*If SUPL POS INIT is succesful, then cftimer is now set to UT2,
      If SUPL POS INIT is unsuccesful, then cftimer is stopped.
      So need not take care of cftimer, just cancel the UT0 state
    */
    z_supl_session_status.e_cf_state &= (supl_cf_state_enum_type)~C_SUPL_CF_STATE_UT0_SUPLPOSINIT;
  }

  /* wait for WIFI scan result is over */
  if ( z_supl_session_status.e_cf_state & C_SUPL_CF_STATE_WIFI_WAIT_TIMER_NO_TCP)
  {
    tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_WIFI_WAIT_TIMER_NO_TCP );
  }
  if ( z_supl_session_status.e_cf_state & C_SUPL_CF_STATE_WIFI_WAIT_TIMER_SUPLSTART)
  {
    tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_WIFI_WAIT_TIMER_SUPLSTART );
  }

}

/*===========================================================================
FUNCTION tm_up_supl_handle_otdoa_mul_meas_timer_expiry

DESCRIPTION
  This function handles OTDOA timer expiry in the case of more than one
  OTDOA/RSTD measurements are received from OTDOA engine.

PARAMETERS:   
  none.
DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
void tm_up_supl_handle_otdoa_mul_meas_timer_expiry( void )
{
  supl_status_s_type *p_supl_session_status = &z_supl_session_status;

  MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "RSTD multiple meas timer expired");

  /* Send LPP PLI with OTDOA error message (UnableToMeasureSomeNeighbourCells) & EndT=TRUE */
  tm_umts_up_supl_terminate_timer_cancel_cf_state(C_SUPL_CF_STATE_OTDOA_MUL_MEAS_RECV);

  /* Set the appropiate OTDOA error code */
  p_supl_session_status->z_supl_lpp_data.u_lpp_error_type |= (uint8)LPP_ERROR_TYPE_PROV_LOC_OTDOA_ERROR;
  p_supl_session_status->z_supl_lpp_data.u_lpp_error_code  = (uint8)LPP_OTDOA_CAUSE_UNABLE_TO_MEAS_SOME_NEIGH;

  supl_enc_lpp_prov_loc(p_supl_session_status, C_LPP_MODE_UE_OTDOA);
}

/*
 ******************************************************************************
 *
 * tm_up_slp_support_glonass
 *
 * Function description:
 *   Based on the receive SUPL data decide whether SLP server has GLONASS capability.
 *
 * Parameters:
 *   
 *
 * Return value:
 *  True if SLP is GLONASS capable, so UE can ask for GLONASS AD. Otherwise False.
 *
 ******************************************************************************
*/
boolean tm_up_slp_support_glonass( supl_status_s_type const * const p_supl_session_status )
{
  if ( NULL == p_supl_session_status )
  {
    return ( FALSE );
  }

  /* Atpresent UE will know if H-SLP supports GLONASS in Set-Initiated call flow.
  *  For all other call flow types UE assumes H-SLP supports GLONASS, which is normal.
  */
  if ( C_SUPL_SESSION_UI == p_supl_session_status->u_callflow_kind )
  {
    return ( p_supl_session_status->u_slp_support_glonass );
  }

  return ( TRUE ); /* Retain the legacy behaviour of asking GLONASS if the SLP capability isn't known */
}

/*
 ******************************************************************************
 *
 * tm_umts_up_supl_enqueue_msg
 *
 * Function description:
 *   Enqueue the SUPL message in the case where we wait for ACK before sending the
 *   next message
 *
 * Parameters:
 *   tm_pdu_buf_type *pdu_buff
 *   
 * Return value:
 *  
 *
 ******************************************************************************/
void tm_umts_up_supl_enqueue_msg(tm_pdu_buf_type *supl_msg)
{

  supl_msg_queue_element *new_supl_msg;
  pd_comms_return_type ret_val;

  if (NULL == supl_msg)
  {
     MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Invalid Buffer");
     return;
  }

  /*Initialize the Queue here, tm_umts_up_supl_session_init () 
    will check for elements and destroy the queue.
    z_supl_session_status Data strcture is memset to '0'
    in supl_session_status_reset()*/
  if (FALSE == z_supl_session_status.supl_msg_q_status) 
  {
    (void)q_init(&(z_supl_session_status.supl_msg_q));
    z_supl_session_status.supl_msg_q_status = TRUE;
  }

  new_supl_msg = (supl_msg_queue_element*)pdsm_getbuf(sizeof(supl_msg_queue_element));
  
  if (NULL == new_supl_msg)
  {
     MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "PDSM Buffer get failed");
     return;
  }
  
  memscpy((void*)new_supl_msg->msg,MAX_SUPL_MESSAGE_SIZE,(void*)supl_msg->value, supl_msg->length);

  new_supl_msg->length = supl_msg->length;

  q_put(&(z_supl_session_status.supl_msg_q),&new_supl_msg->link);

  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "SUPL: Enqueue Packet of length %d",new_supl_msg->length);


  if (q_cnt(&(z_supl_session_status.supl_msg_q)) == 1) 
  {
      ret_val = tm_umts_up_supl_comm_write( (uint8 *)supl_msg->value, (uint16)supl_msg->length);
      if ( ( ret_val != PDCOMM_RESULT_WAIT ) && ( ret_val != PDCOMM_RESULT_OK ) )
      {
        /* message sending failed */
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SUPL: SuplPosLPP attempt aborted");
      }
  }
  else
  {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "SUPL: enqueued message, waiting for ack");
  }
  return;
}

/*
 ******************************************************************************
 *
 * tm_umts_up_supl_process_write_ack
 *
 * Function description:
 *   Dequeue the SUPL message, check if we need to send any more messages and
 *   send it to pd_comms.
 *
 * Parameters:
 * 
 *   
 * Return value:
 *  
 *
 ******************************************************************************
*/
void tm_umts_up_supl_process_write_ack(void *msg_data_ptr)
{
  pd_comms_ipc_write_ack_hdr_type *write_hdr_ptr;
  supl_msg_queue_element *supl_msg = NULL;
  uint16  bytes_written;
  pd_comms_return_type ret_val;

  if(NULL == msg_data_ptr)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SUPL: Received Null msg_data_ptr");
    return;
  }

  write_hdr_ptr = (pd_comms_ipc_write_ack_hdr_type*)msg_data_ptr;

  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "SUPL: write ack received for %d bytes",write_hdr_ptr->q_bytes_written);

  bytes_written = write_hdr_ptr->q_bytes_written;

  if(q_cnt(&(z_supl_session_status.supl_msg_q)) != 0)
  {
    supl_msg = (supl_msg_queue_element *)q_get(&(z_supl_session_status.supl_msg_q));
    if (NULL!=supl_msg) 
    {
       if(supl_msg->length == bytes_written)
       {
          MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "SUPL: SSL write ack received for %d bytes",supl_msg->length);
       }
       (void)pdsm_freebuf((char*)supl_msg);
    }
    
    if (q_cnt(&(z_supl_session_status.supl_msg_q)) !=0)
    {
      supl_msg = (supl_msg_queue_element *)q_check(&(z_supl_session_status.supl_msg_q));
      if (NULL!=supl_msg) 
      {
        ret_val = tm_umts_up_supl_comm_write( (uint8 *)supl_msg->msg, (uint16)supl_msg->length);
        if ( ( ret_val != PDCOMM_RESULT_WAIT ) && ( ret_val != PDCOMM_RESULT_OK ) )
        {
          /* message sending failed */
          MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SUPL: SuplPosLPP attempt aborted");
        }
        else
        {
          MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "SUPL: enqueued next SuplPOSLPP msg");
        }
      }
    }
    else
    {
      if(z_supl_session_status.wait_for_write_ack == TRUE) 
      {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "SUPL: No more Msg's to ACK, Clear WF-ACK Flag");
        z_supl_session_status.wait_for_write_ack = FALSE;
      }
    }
  }
  else
  {
     MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unexpected ACK");
  }
}
/* 
 ******************************************************************************
 *
 * tm_up_supl_handle_early_response_timer_expiry
 *
 * Function description:
 *        This function handles Early timer expiry. UE will provide LCS with
 *        MEAS/Position report based on the type of session MSA/MSB.
 *
 * Parameters: none.
 *   
 * Return value: none.
 *
 ******************************************************************************
*/ 
void tm_up_supl_handle_early_response_timer_expiry( void )
{
  supl_status_s_type *p_supl_session_status = &z_supl_session_status;
  uint32 flags = 0;
  boolean v_sendResponse = FALSE;

  MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Early Response timer expired");

  /*Request OTDOA measurements here, tm_loc_lte_otdoa_recv_measurements will process how to send this early response.*/
  /* Is both gnss & otdoa measurement are requested by Network.
     1) OTDOA assistance provided and Measurements requested
     2) OTDOA measurements requested and no AD provided*/
       if  (((p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_COMBINE_AGNSS_OTDOA)  &&
             (p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_OTDOA_DELIVERED))||
            ((p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_OTDOA_RCVD) &&
             (!(p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_OTDOA_DELIVERED))) ||
            (p_supl_session_status->u_agps_mode == C_SUPL_OTDOA_MODE))
       {
          if (p_supl_session_status->z_supl_lpp_data.q_lpp_flags& TM_SUPL_LPP_PRM_VALID) 
          {
            sm_loc_lte_otdoa_get_measurements(&(p_supl_session_status->z_gnss_meas_report.z_meas_reports[SM_GNSS_MEAS_SYSTEM_GPS].z_MeasBlk.z_MeasClock), 
                                              TM_PRTL_TYPE_UMTS_UP,1);
          }
          else
          {
            sm_loc_lte_otdoa_get_measurements(NULL,TM_PRTL_TYPE_UMTS_UP,1);
          }
       }
       else
       {
          /*No OTDOA Measurements requested. Send only GNSS MEAS/POS if we have any*/
          if (p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_BASED) 
          {
             if (TRUE == p_supl_session_status->z_pending_new_pos_to_network.u_valid) 
             {
                 v_sendResponse = TRUE;
                 flags |= C_SUPL_AGPS_MODE_UE_BASED;
             }
             else
             {
                MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "No Valid Position Estimate at Early Timer expiry");
             }
          }
          else if (p_supl_session_status->u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED)

          {
             if ((p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_PRM_VALID))
             {
                 v_sendResponse = TRUE;
                 flags |= C_SUPL_AGPS_MODE_UE_ASSISTED;
             }
             else
             {
                MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "No Valid Measurements at Early Timer expiry");
             }
          }
          else
          {
             MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Early Response timer expired,OTDOA NA & but no Meas to send");
          }

          if (TRUE == v_sendResponse) 
          {
               tm_up_supl_copy_rli(flags);
               tm_up_supl_add_r12earlyResponse_tx();
               if(supl_enc_lpp_prov_loc(p_supl_session_status, flags) == TRUE)
               {  /* send SUPL POS with LPP Provide Location Info*/
                      supl_SuplPos_lpp_tx(p_supl_session_status, NULL);
               }
               else
               {
                  MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Early Response timer expired,OTDOA NA & but no Meas to send");
               }
          }

          if ( p_supl_session_status->e_cf_state & (C_SUPL_CF_STATE_OTDOA_EARLY_RESPONSE_REQ) )
          {
             tm_umts_up_supl_terminate_timer_cancel_cf_state( C_SUPL_CF_STATE_OTDOA_EARLY_RESPONSE_REQ );
          }
       }
}

/* 
 ******************************************************************************
 *
 * tm_umts_up_supl_add_r12earlyResponse_tx
 *
 * Function description:
 *     This function duplicates the original transaction for sending out
 *     early Measurement report/Location estimate.
 *
 * Parameters: none.
 *   
 * Return value: none.
 *
 ******************************************************************************
*/ 
void tm_up_supl_add_r12earlyResponse_tx (void)
{
  tm_supl_lpp_data_type          *p_supl_lpp_data  = &(z_supl_session_status.z_supl_lpp_data);
  tm_lpp_transaction_info_s_type *p_lpp_tx_rli     = NULL;

  /*Add an entry into the transaction table to duplicate the original REQ_LOC
    We will send the POC_LOC with the same transaction ID*/
  tm_lpp_tx_mgr_insert_entry( &(z_lpp_up_tx_rli.z_current_trans_id), &p_lpp_tx_rli, z_lpp_up_tx_rli.u_client_handle );
  if ( NULL == p_lpp_tx_rli )
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Can't add RLI entry as transaction" );
    return;
  }
  /* Update the reqLocInfo type & measurement available to OTDOA only*/
  p_lpp_tx_rli->e_messages_id = TM_LPP_MESG_REQ_LOC_INFO;
  p_lpp_tx_rli->b_pending_ue_tx = TRUE;
  p_lpp_tx_rli->z_lpp_data_type.req_loc_gnss_method_bmap = z_lpp_up_tx_rli.z_lpp_data_type.req_loc_gnss_method_bmap;

  if(p_supl_lpp_data->q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_COMBINE_AGNSS_OTDOA)
  {
     p_lpp_tx_rli->z_lpp_data_type.req_loc_type = C_LPP_MODE_UE_OTDOA|C_LPP_AGPS_MODE_UE_ASSISTED;
  }
  else if ((p_supl_lpp_data->q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_AGNSS_RCVD)
           &&(z_supl_session_status.u_agps_mode == C_SUPL_AGPS_MODE_UE_ASSISTED))
  {
     p_lpp_tx_rli->z_lpp_data_type.req_loc_type = C_LPP_AGPS_MODE_UE_ASSISTED;
     MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Early PLI transaction %d",p_lpp_tx_rli->z_lpp_data_type.req_loc_type);
  }
  else if ((p_supl_lpp_data->q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_OTDOA_RCVD)
           &&(z_supl_session_status.u_agps_mode == C_SUPL_OTDOA_MODE)) 

  {
     p_lpp_tx_rli->z_lpp_data_type.req_loc_type = C_LPP_MODE_UE_OTDOA;
     MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Early PLI transaction %d",p_lpp_tx_rli->z_lpp_data_type.req_loc_type);
  }
  else if ((z_supl_session_status.u_agps_mode == C_SUPL_AGPS_MODE_UE_BASED)
           && (z_supl_session_status.z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_REQ_LOC_INFO_AGNSS_RCVD)) 
  {
     p_lpp_tx_rli->z_lpp_data_type.req_loc_type = C_LPP_AGPS_MODE_UE_BASED;
     MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Early PLI transaction %d",p_lpp_tx_rli->z_lpp_data_type.req_loc_type);
  }
  else
  {
     MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Early PLI transaction could not be added");
  }

  MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "Add new RLI entry to for early response 0x%lX",
         p_lpp_tx_rli->z_lpp_data_type.req_loc_type );
}

/* 
 ******************************************************************************
 *
 * tm_up_supl_copy_rli
 *
 * Function description:
 *         This function retrieves the Transaction for the original RLI based on
 *         rer_loc_type and saves it in local data structure.
 *
 * Parameters:
 *         flags: ref_loc_type to look up RLI.
 *   
 * Return value:
 *
 ******************************************************************************
*/
void tm_up_supl_copy_rli(uint32 const flags)
{
  tm_lpp_transaction_info_s_type            *p_lpp_tx_rli          = NULL;
  
  p_lpp_tx_rli = tm_lpp_tx_mgr_find_entry_by_refloc( flags, TM_LPP_TX_MGR_CLIENT_TYPE_SUPL );

  if ( p_lpp_tx_rli != NULL )
  {
     /* Copy the original LPP RLI tx */
     z_lpp_up_tx_rli = *p_lpp_tx_rli;
     MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "Copy original LPP RLI transaction 0x%lX",
            z_lpp_up_tx_rli.z_lpp_data_type.req_loc_type );
  }
  else
  {
     MSG( MSG_SSID_GPSSM, MSG_LEGACY_MED, "Copy original LPP RLI transaction failed");
  }

  return;
}

/* 
 ******************************************************************************
 *
 * tm_umts_up_supl_r12earlyResponse_valid
 *
 * Function description:
 *     This function check for Early Response validity in
 *     1) Hybrid CF: if atleast one of PRM or OTDOA RSTD is present.
 *     2) OTDOA  CF: if OTDOA RSTD's are present.
 *
 * Parameters:
 *    Boolean:  v_Rstd.
 *    Flags  :  q_flags.
 *   
 * Return Boolean:
 *    TRUE  : Send Early Response
 *    FALSE : Drop Early Response
 *
 ******************************************************************************
*/ 
boolean tm_umts_up_supl_r12earlyResponse_valid (uint32 q_flags, boolean v_Rstd)
{
  supl_status_s_type   *p_supl_session_status = &z_supl_session_status;

  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "TM_LPP_UP: SUPL OTDOA: flags : %u, RSTD's : %u",
        q_flags,v_Rstd);

  if ( TRUE == v_Rstd )
  {
     /*If RSTD's are valid, return right away, no need to check further*/
     return ( TRUE );
  }
  
  if ( 0 == ( q_flags & C_LPP_MODE_UE_OTDOA ) )
  {  
     /*This should not happen. 
       C_LPP_MODE_UE_ODTOA is flag is set in "tm_umts_up_supl_handle_otdoa_data"*/
     return ( FALSE );
  }

  /*No OTDOA Rstd's v_Rstd = FALSE in Hybrid or OTDOA only mode
    if call flow is Hybrid and no OTDOA RSTD's & no PRM measured drop the message */
  if ( ( 0 != ( q_flags & C_LPP_AGPS_MODE_UE_ASSISTED ) ) &&
       ( 0 == ( p_supl_session_status->z_supl_lpp_data.q_lpp_flags & TM_SUPL_LPP_PRM_VALID ) ) )
  {
     MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "TM_LPP_UP: SUPL OTDOA: No PRM & RSTD's drop ERsp");
     return ( FALSE );
  }
  /*if call flow is OTDOA only and no RSTD's measured drop the message*/
  else if ( 0 == ( q_flags & C_LPP_AGPS_MODE_UE_ASSISTED ) )
  {
     MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "TM_LPP_UP: SUPL OTDOA: No RSTD's drop ERsp");
     return ( FALSE );
  }

  /*Send Early Response*/
  return ( TRUE );
}

#endif /* #ifdef FEATURE_CGPS_UMTS_UP_SUPL */

