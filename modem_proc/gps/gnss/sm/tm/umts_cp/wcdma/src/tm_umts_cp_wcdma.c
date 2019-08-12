/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TM-UMTS-CP-WCDMA sub-module

GENERAL DESCRIPTION
  This file implements TM UMTS-CP WCDMA sub-module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.
  Copyright (c) 2013 - 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_cp/wcdma/src/tm_umts_cp_wcdma.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  07/05/16   rk      Pass all measurements to protocols. It will decide which measurements should be
                     reported to server.
  08/26/15   gk      90% conf reporting changes
  08/18/15   mj      Add changes to cache as_id for W+W ahead when RRC sends the first message to TM
  07/25/15   rk      AD request Holdoff timer aren't needed for NI session.
  07/03/15   jv      Added QZSS Support
  06/09/15   mj      Add support for W+W feature
  12/18/14   rk      Don't set any GPS measurement flag in RRC LSM struct when there are zero GPS SVs.
  11/04/14   rk      Clear the measurement report structure once for each measurement sets.
  11/01/14   rk      Compute Glonass Time when GPS UTC & GPS Reference Time is injected in any order.
  08/19/14   rk      Fixed sending if there is atleast one measurement report type (GPS or GLO) encoded successfully.
  08/08/14   jv      Added Galileo Support
  07/17/14   rk      Fixed the issue in sending first e911 periodic position report.
  11/08/13   rk      Fixed UE sending all false AD request in MRM (missing assistance data) for AGPS.
  10/29/13   mj   Update WCDMA/TDSCDMA API calls for segment loading
  10/16/13   rk      Added RRC R8 A-GLO NILR MSA support.
  09/30/13   rk      Fixed RRC MRM ganssTimeModelGNSS-GNSS parameter to request GlonassTimeModel
  09/30/13    rk     Removed reference to deprecated NV item 3649 & use RRC API to get
                     the selected RRC protocol release major version.
  09/25/13   rk      Fixed RRC MRM error codes as per RRC spec 8.6.7.19.5
  05/29/13   rk      Added GPS RTI processing support.
  04/21/13   rk      Added RRC R8 GANSS Assistance data processing interfaces.
  09/17/12   mj     Modified gps_meas_status_bits_check call
  08/10/07   gk      set the position reported flag once the position is sent to PDAPI.
  07/27/09   gk      tm_umts_cp_wcdma_lcs_molr_conf_proc() - translate ReturnResult(error code)
                      to TM end session error code
  07/25/09   atien   Add tm_umts_cp_wcdma_report_ref_loc_in_ext_status_event()
  07/07/09   atien   Fix for DocoMo Case 180577 - MO-LR cancellation before MC case
  06/30/09   atien   add new function parameter to pass cf timer value in
                     tm_umts_cp_wcdma_set_cf_state
  05/12/08   LT      Ensured PDSM_PD_EVENT_END sent when no-fix.
  08/10/07   gk      dont send release complete until mo-lr ret result is rcvd.
  08/02/07   LT      Handling SVs non-exist.
  05/03/07   LT      Initial version

============================================================================*/

#include "gps_variation.h"
#include "customer.h"   /* Customer configuration file */
#ifdef FEATURE_CGPS_UMTS_CP_WCDMA

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
  #include "msg.h"
  #include <limits.h>
  #include "sm_nv.h"
  #include "sm_log.h"
  #include "tm_data.h"
  #include "tm_common.h"
  #include "tm_umts_common_utils.h"
  #include "math.h"

  #include "tm_rrc.h"
  #include "rrccmd.h"
  #include "rrcdata.h"
  #include "pdapibuf.h"

  #include "tm_umts_cp_submodule_iface.h"
  #include "tm_umts_cp_ss.h"

  #include "pdapi.h"
  #include "gnss_consts.h"
  #include "gnss_wwan_iface.h"

#define UMTS_CP_WCDMA_MAIN_VARIABLES_DEFINED
#include "tm_umts_cp_wcdma.h"
#undef UMTS_CP_WCDMA_MAIN_VARIABLES_DEFINED

/* =======================================================================
**           Local Function Prototypes
** ======================================================================= */


static void cp_wcdma_meas_control_proc( rrc_meas_ctrl_struct_type *p_rrc_meas_ctrl );
static void cp_wcdma_assist_delivery_proc( rrc_assistance_data_struct_type *p_assistance_data_params );
static boolean cp_wcdma_rrc_gps_assist_proc( rrc_assistance_data_struct_type *p_assist );
#ifdef FEATURE_GANSS_SUPPORT
void cp_wcdma_retain_slot_id_freq_map_only( prot_glo_eph_buffer_s_type *const p_ProtGloEphBuf );
static void cp_wcdma_rrc_ganss_assist_proc( rrc_meas_ctrl_struct_type const *const p_RrcMeasCtrl );
#endif
static boolean cp_wcdma_rrc_send_measrpt( rrc_meas_rpt_struct_type *p_measrpt );

static rrc_meas_rpt_struct_type* cp_wcdma_generate_default_save_response(
                                                                          uint8 u_MeasId, uint8 u_TransferMode, uint8 u_ReportMode );
#ifdef FEATURE_CGPS_DUAL_WCDMA
static boolean tm_umts_cp_wcdma_validate_as_id(sys_modem_as_id_e_type rrc_event_as_id);
#endif

/**********************************************************************************************************************
 * cp_wcdma_send_rrc_msrReport_meas
 *
 * Function description:
 *  This function encodes & decide whether to send the measurement report to WCDMA-RRC or save it internally.
 *  The measurement report could be GPS or GPS + GLO measurement depending on A-GPS or A-GNSS call flow.
 *  The measurement report will be sent only at the end of each epoch.
 *
 * Parameters:
 *  p_prm_data: GPS or GLONASS measurement report
 *  status_check_needed: Measurement status bit check needed.
 *
 * Return:
 *   TRUE: RRC LSM MRM with measurement is sent
 *   FALSE: In A-GPS scenario RRC LSM MRM with measurement failed to send.
 *          In A-GNSS scenario, the first GPS measurement is encoded successfully
 *          & waiting for Glonass measurements to arrive.
 *          (or)
 *          In A-GNSS scenario, tell the calling function to wait for 'C_UMTS_CP_WCDMA_CF_STATE_TIMER_RRC_REPORT'
 *          timer expiry & send MRM with appropiate error code.
 *
 **********************************************************************************************************************
 */
static boolean cp_wcdma_send_rrc_msrReport_meas( sm_GpsMeasRptStructType *p_prm_data, boolean status_check_needed );
static void cp_wcdma_action_upon_ni_notif_verif_accepted( void );
static void cp_wcdma_action_upon_ni_notif_verif_denied( void );

/*
 ******************************************************************************
 *
 * cp_wcdma_rrc_gps_refloc_assist_proc
 *
 * Function description:
 *  This function processes the OSS ASN.1 decoded RRC gpsReferenceLocation data and
 *  prepares such assistance data for injecting to MGP.
 *
 * Parameters:
 *  b_GpsRefTimeValid: Whether GPS Reference time is included (w_GpsRefTimeWeekNum & q_GpsRefTimeTowMs is trustable)
 *  w_GpsRefTimeWeekNum: GPS time week number
 *  q_GpsRefTimeTowMs: GPS of week in milli second (0..604799999)
 *  p_GpsRefLoc: pointer to RRC LSM GPS Reference Location AD structure.
 *
 * Return value:
 *    None
 *
 ******************************************************************************
*/
static void cp_wcdma_rrc_gps_refloc_assist_proc( boolean b_GpsRefTimeValid, uint16 const w_GpsRefTimeWeekNum,
                                                 uint32 const q_GpsRefTimeTowMs,
                                                 rrc_gps_reference_loc_struct_type const *const p_GpsRefLoc );

/*===========================================================================

FUNCTION tm_umts_cp_wcdma_handle_multi_periodic_rpt

DESCRIPTION Send multiple perioid report to network & when specified num of measurement
            is sent-out, close the session.

DEPENDENCIES
  None.

RETURN VALUE
  none.

SIDE EFFECTS
  None.

===========================================================================*/
void tm_umts_cp_wcdma_handle_multi_periodic_rpt( umts_cp_wcdma_session_status_s_type *const p_umts_cp_wcdma_session_status,
                                                 sm_GpsMeasRptStructType *const p_prm_data, boolean const status_check_needed,
                                                 boolean const b_first_done_meas );

/*===========================================================================

FUNCTION cp_wcdma_mcm_rpt_quantity_modified

DESCRIPTION
  Verify Measurement Control Meassage (MCM) has modified reporting quantity which is valid.
  If the modified reporting quantity is valid, then update the position method & position
  method type for the current session.

DEPENDENCIES
  This function won't check for invalid position method. Call cp_wcdma_mcm_rpt_position_method_invalid()
  function to check for invalid position method.

RETURN VALUE
  none.

SIDE EFFECTS
  None.

===========================================================================*/
boolean cp_wcdma_mcm_rpt_quantity_modified( umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status,
                                            rrc_meas_ctrl_struct_type const *const p_rrc_meas_ctrl );

/*===========================================================================

FUNCTION cp_wcdma_accept_modified_mcm_rpt_quantity

DESCRIPTION
  Verify Measurement Control Meassage (MCM) has modified reporting quantity. If the modified
  reporting quantity is valid, then update the position method & position method type for the
  current session.

DEPENDENCIES
  This function won't check for invalid position method. Call cp_wcdma_mcm_rpt_position_method_invalid()
  function to check for invalid position method.

RETURN VALUE
  none.

SIDE EFFECTS
  None.

===========================================================================*/
boolean cp_wcdma_accept_modified_mcm_rpt_quantity( umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status,
                                                   rrc_meas_ctrl_struct_type const *const p_rrc_meas_ctrl );


/*===========================================================================

FUNCTION cp_wcdma_mcm_pos_method_support_by_ue

DESCRIPTION
  Validate the network requested position method type is supported by UE's NV settings.

DEPENDENCIES
  none.

RETURN VALUE
  none.

SIDE EFFECTS
  None.

===========================================================================*/
boolean cp_wcdma_mcm_pos_method_support_by_ue( uint32 const q_NvPositionModeSupported, rrc_positioning_method_type_enum_type const e_RrcMcmPositionMode );

/*===========================================================================

FUNCTION cp_wcdma_allowed_position_mode

DESCRIPTION
  Validate the network provided position mode (method & method type) & return the
  translated position mode for defining the call flow type.

DEPENDENCIES
  none.

RETURN VALUE
  Return the position mode. The position mode could be undefined as well.

SIDE EFFECTS
  None.

===========================================================================*/
wcdma_position_mode_e_type cp_wcdma_allowed_position_mode( rrc_meas_ctrl_struct_type const *const p_rrc_meas_ctrl );

/*===========================================================================

FUNCTION cp_wcdma_build_tm_session_msa_ptr

DESCRIPTION
  Helper function to populate the TM-Core session pointer with appropiate MSB parameters.

DEPENDENCIES
  none.

RETURN VALUE
  Return the TM-Core session type that should be requested.

SIDE EFFECTS
  None.

===========================================================================*/
tm_sess_req_type cp_wcdma_build_tm_session_msa_ptr( tm_sess_req_param_u_type *const p_req_param, umts_cp_wcdma_session_status_s_type *const p_umts_cp_wcdma_session_status );

/*===========================================================================

FUNCTION cp_wcdma_build_tm_session_msb_ptr

DESCRIPTION
  Helper function to populate the TM-Core session pointer with MSB parameters.

DEPENDENCIES
  none.

RETURN VALUE
  Return the TM-Core session type that should be requested.

SIDE EFFECTS
  None.

===========================================================================*/
tm_sess_req_type cp_wcdma_build_tm_session_msb_ptr( tm_sess_req_param_u_type *const p_req_param, umts_cp_wcdma_session_status_s_type *const p_umts_cp_wcdma_session_status );


/* =======================================================================
**                            Function Definitions
** ======================================================================= */

#ifdef FEATURE_GANSS_SUPPORT
/********************************************************************
*
* cp_wcdma_glonass_check
*
* Function description:
*   This function checks if MCM has GANSS fields & UE NV settings allows Glonass on RRC protocol
*
* Parameters:
*   p_rrc_meas_ctrl : Pointer to MCM (Measurement Contol message)
*
* Return:
*  True if glonass processing can be done, else false.
*
*********************************************************************
*/
static GNSS_INLINE boolean cp_wcdma_glonass_check( rrc_meas_ctrl_struct_type const *const p_rrc_meas_ctrl )
{
  /* The presence of ganss_pos_method_incl indicates RRC gANSSPositioningMethods IE sent by network, which overwrites
        the position method in A-GPS positionMethod IE. */
  if ( (TRUE == p_rrc_meas_ctrl->rpt_quan.ganss_pos_method_incl) && tm_umts_common_glonass_enabled() &&
       tm_umts_cp_wcdma_a_glo_rrc_cp_enabled() )
  {
    return ( TRUE );
  }

  return ( FALSE );
}

/************************************************************************************************************
*
* cp_wcdma_ganss_pos_method_is_agnss
*
* Function description:
*   This function verifies positionMethod is set to A-GNSS type (GPS + GLO). The lower WCDMA-RRC layer
*   should return MCF for unsupported position methods (like Galileo, SBAS, M-GPS, QZSS).
*   See FRS section 4.5.2 for more details.
*
* Parameters:
*   p_rrc_meas_ctrl : Pointer to MCM (Measurement Contol message)
*
* Return:
*  True if positionMethod is valid A-GNSS type (i.e. GPS + GLONASS)
*
* Dependency:
*    Depends on whether GANSS parameter is provides. So call cp_wcdma_glonass_check() before
*    using this function.
*
*************************************************************************************************************
*/
GNSS_INLINE boolean cp_wcdma_ganss_pos_method_is_agnss( rrc_meas_ctrl_struct_type const *const p_rrc_meas_ctrl )
{
  /* Is GPS & GLONASS position method selected?. For GPS-only, gANSSPositioningMethods IE shouldn't be present. */
  if ( p_rrc_meas_ctrl->rpt_quan_incl && p_rrc_meas_ctrl->rpt_quan.ganss_pos_method_incl &&
       (p_rrc_meas_ctrl->rpt_quan.ganss_pos_method & GANSS_POSITIONING_METHOD_GPS_BMASK) &&
       (p_rrc_meas_ctrl->rpt_quan.ganss_pos_method & GANSS_POSITIONING_METHOD_GLONASS_BMASK) )
  {
    return ( TRUE );
  }

  return ( FALSE );
}

/************************************************************************************************************
*
* cp_wcdma_rrc_inject_glonass_acq_assist_to_tmcore
*
* FUNCTION DESCRIPTION:
*   Validate & inject Glonass acquisition assistance data.
*
* PARAMETERS:
*  none
*
* RETURN:
*  none
*
* DEPENDENCY:
*    none
*
*************************************************************************************************************
*/
void cp_wcdma_rrc_inject_glonass_acq_assist_to_tmcore( void )
{
  umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status = &z_umts_cp_wcdma_session_status;
  tm_post_data_payload_type           z_tm_post_data_payload;

  /* Inject Glonass Acquisition Assistance - SV Dir & Steering - if deltaT/leap second (from GPS UTCModel or Glonass TimeModel AD)
  *  + Raw AcqAssist (a.k.a Reference Measurement Info) + Glo AuxiInfo available.
  */
  if ( p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED )
  {
    memset(&z_tm_post_data_payload, 0x00, sizeof(tm_post_data_payload_type));

    /* Associates Sv ID with the frequencey number from the stored AA & AuxiInfo structure */
    tm_util_map_glo_acqast_svid_to_freq_num(&p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_buf,
                                            &p_umts_cp_wcdma_session_status->z_ProtGloSvCnBuf);

    /* For ME to generate GLONASS measurements both SV Dir & Steering should be injected successful. */

    /* Inject SV direction first */
    if ( (p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_buf.gnss_sv_dir.u_NumSvs > 0) &&
         (p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_buf.gnss_sv_dir.u_NumSvs <= N_GLO_SV) &&
         (0 == p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_buf.m.svDirInjected) )
    {

      if ( TRUE == p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_buf.u_freq_num_valid )
      {
        /* Copy SV direction into TM post struct */
        memscpy(&z_tm_post_data_payload.gnss_sv_dir, sizeof(z_tm_post_data_payload.gnss_sv_dir),
                &p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_buf.gnss_sv_dir,
                sizeof(p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_buf.gnss_sv_dir));

        /* Post sv dir */
        if ( TRUE == tm_post_data(TM_PRTL_TYPE_UMTS_CP,
                                  p_umts_cp_wcdma_session_status->tm_active_session_handle,
                                  TM_POST_DATA_TYPE_SV_DIR_GLO,
                                  &z_tm_post_data_payload) )
        {
          MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Glo AA SV Dir (total %u SVs) posted to TmCore",
                z_tm_post_data_payload.gnss_sv_dir.u_NumSvs);
          /* Mark as injected for this session */
          p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_buf.m.svDirInjected = 1;
        }
        else
        {
          /* This doesn't mean failure. The dependent GloAuxiInfo isn't available yet. */
          MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Glo AA SV Dir (total %u SVs) isn't posted to Tmcore",
                z_tm_post_data_payload.gnss_sv_dir.u_NumSvs);
        }
      }

    } /* AA SV-Dir sanity check end */

    if ( (p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_buf.z_steering_glo_data.u_NumSvs > 0) &&
         (p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_buf.z_steering_glo_data.u_NumSvs <= N_GLO_SV) &&
         (0 == p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_buf.m.svSteeringInjected) )
    {
      /* Inject Steering */
      if ( TRUE == tm_util_map_and_inject_glo_steering_to_tmcore(&p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_buf,
                                                                 &p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_subset,
                                                                 &p_umts_cp_wcdma_session_status->z_ProtGloSvCnBuf,
                                                                 &p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp,
                                                                 TM_PRTL_TYPE_UMTS_CP,
                                                                 p_umts_cp_wcdma_session_status->tm_active_session_handle,
                                                                 &z_tm_post_data_payload) )
      {
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Glo AA Steering (total %u SVs) posted to TmCore", z_tm_post_data_payload.gnss_aa_data.u_NumSvs);

        /* Mark as injected for this session */
        p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_buf.m.svSteeringInjected = 1;

        /* Reset buffer containing AA data so that they won't get injected again later */
        // p_gnss_time->m.glonassTimeValid = FALSE;
        memset(&p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_buf, 0, sizeof(p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_buf));

        /* Release AcqAssist Subset struct if its stored internally */
        if ( p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_subset.p_svid_icp != NULL )
        {
          os_MemFree((void **)&(p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_subset.p_svid_icp));
          p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_subset.p_svid_icp = NULL;
          p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_subset.u_cnt = 0;
        }
      }
      else
      {
        /* This doesn't mean failure. The dependent GloAuxiInfo and/or LeapSecond (from GPS UTCModel) isn't available yet. */
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Glo AA Steering (total %u SVs) isn't posted to Tmcore",
              z_tm_post_data_payload.gnss_aa_data.u_NumSvs);
      }
    } /* AA Steering sanity check end */
  }
}

/************************************************************************************************************
*
* cp_wcdma_encode_rrc_msrReport_glo_meas
*
* FUNCTION DESCRIPTION:
*   Encode GLONASS measurement report into RRC LSM MRM structure.
*
* PARAMETERS:
*  p_prm_data: Source measurement report.
*  p_measrpt: Target RRC LSM MRM structure
*
* RETURN:
*  True if the translation is successful.
*
* DEPENDENCY:
*    none
*
*************************************************************************************************************
*/
static boolean cp_wcdma_encode_rrc_msrReport_glo_meas( sm_GpsMeasRptStructType const *const p_prm_data, boolean const status_check_needed,
                                                       rrc_meas_rpt_struct_type *const p_measrpt )
{
  rrc_ganssMeasurementSignalList_struct_type *pGloMeasSig           = NULL;
  uint8                                      u_TotalGloSvs          = 0,
                                             u_TotalGoodGloSVs      = 0;
  float                                      f_Num;
  double                                     d_AdjSubMsec,
                                             d_Num;
  int16                                      x_Word;
  uint8                                      u_i;
  const gnss_MeasStructType                  *p_GloMeasPerSv        = NULL;
  const gnss_MeasBlkStructType               *p_GloMeasRpt          = NULL;
  rrc_ganssMeasurementParameters_struct_type *pGloMeasSigMeasParams = NULL;

  if ( p_prm_data->z_MeasBlk.e_MeasBlkSrc != GNSS_MEAS_BLK_SRC_GLO )
  {
    return ( FALSE );
  }

  p_GloMeasPerSv = &p_prm_data->z_MeasBlk.z_Gnss[0];
  p_GloMeasRpt = &p_prm_data->z_MeasBlk;

  if ( p_GloMeasRpt != NULL )
  {
    /* if this number is ever greater than 24, limit to the first 24 */
    if ( p_GloMeasRpt->u_NumSvs < N_ACTIVE_GLO_CHAN )
    {
      u_TotalGloSvs = p_GloMeasRpt->u_NumSvs;
    }
    else
    {
      u_TotalGloSvs = N_ACTIVE_GLO_CHAN;
    }


    /* Count good SVs (non-zero CNo & status check). Also translate the qualifying measurement into RRC GANSS struct */
    u_TotalGoodGloSVs = 0;

    /* RRC measurement structs */
    p_measrpt->ganss_measured_results_incl = FALSE;

    pGloMeasSig = &p_measrpt->ganss_measured_results.ganssGenericMeasurementInfo[0].ganssMeasurementSignalList[0];
    pGloMeasSigMeasParams = &pGloMeasSig->ganssMeasurementParameters[0];

    for ( u_i = 0; (u_i < u_TotalGloSvs); ++u_i, ++p_GloMeasPerSv )
    {
      if ( (p_GloMeasPerSv->w_Cno > 0) &&
           (gnss_meas_status_bits_check(p_GloMeasPerSv->q_MeasStatus, status_check_needed)) )
      {
        ++u_TotalGoodGloSVs; /* count good SVs */

        /* RG : Do not include code phase ambiguity and integer code phase for LPP and RRC; but include for RRLP and set them to 1 and 0 respectively. */
        /* RG : ganssCodePhaseAmbiguity not included, therefore, it takes the default value of 1 ms. */

        /* RG : integer code phase isn't support. Only fractional code phase is included in the GNSS measurements but not the integer code phase */

        /* RRC R8 ganssCodePhaseAmbiguityExt not supported */
        /* RRC R8 ganssIntegerCodePhaseExt not supported */

        /* Protocol : satId (0..63) */
        pGloMeasSigMeasParams->satId = (uint8)p_GloMeasPerSv->w_Sv - (GLO_SLOTID_OFFSET + 1);
        /* Protocol : cSurNzero/Cno typical levels will be in the range of 20 – 50 dB-Hz */
        pGloMeasSigMeasParams->cSurNzero = (uint32)((float)(p_GloMeasPerSv->w_Cno + p_GloMeasRpt->z_Gnss[u_i].w_RfLoss) * 0.1);

        /* doppler: provided in m/s f_Num = f_SvSpeed *(FLT)(-1.0); */
        f_Num = (p_GloMeasPerSv->z_FiltMeas.f_SvSpeed - p_GloMeasRpt->z_MeasClock.z_Freq.f_ClkFreqBias);
        x_Word = (int16)(f_Num * 25.0f); /* resolution .04 m/s */
        /* Protocol : doppler    (-32768..32767) m/s */
        pGloMeasSigMeasParams->doppler = x_Word * (-1);

        /* need to change polarity since protocol definition of doppler is reverse of what ME is using */

        /* code phase: Q: this following uses GPS time. In the cases where no GPS measurements included,
           and GNASS_TOD has to be used, the processing would be different */

        /** Code Phase **/
        /* We tell location server that the reference point of the measurement is
           at TOW, which is now contained in p_msrset->gpsTOW,
           but the actually GPS moment of the measurement is instead at
           the FCOUNT of the measurement, whose GPS msec is
           p_zMeasGpsTime->q_Msec - p_zMeasGpsTime->f_ClkBias.
           We can adjust each SV's submsec value by adding
           p_zMeasGpsTime->f_ClkBias to it. */

        d_AdjSubMsec = (double)p_GloMeasPerSv->z_FiltMeas.z_SvTime.f_SubMs
                       + (double)p_GloMeasRpt->z_MeasClock.z_Time.z_GpsTime.f_ClkTimeBias;

        if ( d_AdjSubMsec > 1.0 )
        {
          /* if the adjustment results in the submsec value above one, at the
             reference moment (TOW) SV time's msec should be one msec more,
             thus we need to subtract 1 ms from the submsec variable */

          d_AdjSubMsec -= 1.0;
        }
        else if ( d_AdjSubMsec < 0 )
        {
          /** or if the adjustment results in submsec value negative, at the
            reference moment (TOW) SV time's msec should be one msec less,
            thus we need to use one minus the resuting "submsec" value for
            the submsec value */

          d_AdjSubMsec = 1.0 + d_AdjSubMsec;
        }

        /* both whole chip and fraction of a chip are based on "code phase", so
           convert submsec to code phase */
        d_AdjSubMsec = 1.0 - d_AdjSubMsec;
        /* whole chips: whole chip representation of the code phase */
        d_Num = d_AdjSubMsec * 2097152.0;
        /* Protocol : (0..2097151) msec. whole chips in msec, floating */
        pGloMeasSigMeasParams->ganssCodePhase = (uint32)d_Num; /* whole chips in msec, U10 */

        /** Multipath **/
        if ( p_GloMeasPerSv->u_MultipathEstValid == FALSE )
        {
          pGloMeasSigMeasParams->multipathIndicator = 0;
        }
        else if ( p_GloMeasPerSv->f_MultipathEst < 5.0f )
        {
          pGloMeasSigMeasParams->multipathIndicator = 1;
        }
        else if ( p_GloMeasPerSv->f_MultipathEst < 43.0f )
        {
          pGloMeasSigMeasParams->multipathIndicator = 2;
        }
        else
        {
          pGloMeasSigMeasParams->multipathIndicator = 2;
        }

        /* PseudoRange RMS error: converted from SV time uncertainty */
        f_Num = (float)(p_GloMeasPerSv->z_FiltMeas.f_SvTimeUncMs * (FLT)LIGHT_MSEC);
        pGloMeasSigMeasParams->codePhaseRmsError = tm_umts_common_utils_psudoRMS_formater(f_Num);

        if ( pGloMeasSigMeasParams->codePhaseRmsError > 62 )
        {
          pGloMeasSigMeasParams->codePhaseRmsError = 62; /* to avoid being considered as xcorelation by some pdm */
        }

        ++pGloMeasSigMeasParams;
        ++pGloMeasSig->meas_param_cnt; /* <= 24 */

        if ( pGloMeasSig->meas_param_cnt >= N_GLO_SV )
        {
          break; /* destination can't take more than N_GLO_SV (24) */
        }
      } /* only good SVs*/
    } /* loop through all SVs */

    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Glonass total good meas %u; total meas %u", u_TotalGoodGloSVs, u_TotalGloSvs);

    if ( u_TotalGoodGloSVs == 0 )
    {
      return ( FALSE ); /* not a single reportable Glonass SV, abort */
    }

    /* At this point atleast one measurement is good */

    /* Set other WCDMA-RRC Glonass variables */
    p_measrpt->ganss_measured_results_incl = TRUE;
    p_measrpt->ganss_measured_results.meas_cnt = 1;
    p_measrpt->ganss_measured_results.ganssGenericMeasurementInfo[0].ganssId_incl = TRUE;
    p_measrpt->ganss_measured_results.ganssGenericMeasurementInfo[0].ganssId = RRC_GLONASS;
    p_measrpt->ganss_measured_results.ganssGenericMeasurementInfo[0].list_cnt = 1;
    pGloMeasSig->ganssSignalId_incl = TRUE;
    pGloMeasSig->ganssSignalId = RRC_GANSS_GLONASS_SIGNAL_ID_G1;


    /* By defauly Glonass time is populated.  We will decide to update MRM clock info
    *  with GPS Time by calling cp_wcdma_mrm_update_glo_meas_time() later.
    */
    /* Use Glonass time from the glonass measurement block */

    /* Sanity check */
    if ( (p_GloMeasRpt->z_MeasClock.z_Time.z_GloTime.u_FourYear == 255) ||
         (p_GloMeasRpt->z_MeasClock.z_Time.z_GloTime.w_Days == 65535) ||
         (p_GloMeasRpt->z_MeasClock.z_Time.z_GloTime.q_GloMsec > DAY_MSECS)
        )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,
            "GlonassTOD invalid: Num4Year=%u, NumDaysIn4Year=%u, ToD_ms=%lu",
            p_GloMeasRpt->z_MeasClock.z_Time.z_GloTime.u_FourYear,
            p_GloMeasRpt->z_MeasClock.z_Time.z_GloTime.w_Days,
            p_GloMeasRpt->z_MeasClock.z_Time.z_GloTime.q_GloMsec);
      return ( FALSE );
    }
    else
    {
      p_measrpt->ganss_measured_results.referenceTime = GANSS_REFERENCE_TIME_ONLY;
      /* Protocol : (0..3599999) */
      p_measrpt->ganss_measured_results.u.ganss_ReferenceTimeOnly.gANSS_tod = p_GloMeasRpt->z_MeasClock.z_Time.z_GloTime.q_GloMsec % HOUR_MSECS;
      p_measrpt->ganss_measured_results.u.ganss_ReferenceTimeOnly.gANSS_timeId_incl = TRUE;
      p_measrpt->ganss_measured_results.u.ganss_ReferenceTimeOnly.gANSS_timeId = RRC_GLONASS_SYSTEM_TIME;

      /* tod_uncertainty */
      p_measrpt->ganss_measured_results.u.ganss_ReferenceTimeOnly.gANSS_tod_uncertainty_incl = TRUE;
      /* Protocol (0..127) */
      p_measrpt->ganss_measured_results.u.ganss_ReferenceTimeOnly.gANSS_tod_uncertainty = tm_rrc_utils_microsec_to_gadk(p_GloMeasRpt->z_MeasClock.z_Time.z_GloTime.f_ClkTimeUncMs * 1000);
    }


  } /* Glonass Meas Report end */
  else
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "No GLONASS measurement block");
    return ( FALSE );
  }

  return ( TRUE );
}

/************************************************************************************************************
*
* cp_wcdma_encode_rrc_msrReport_glo_meas
*
* FUNCTION DESCRIPTION:
*   Update the RRC LSM Glonass measurement block with GPS time. Measurements are generated at a specific
*   reference GPS time.
*
* PARAMETERS:
*  p_GpsTime4GloMeas: GPS measurement time information.
*  p_measrpt: Update the RRC LSM MRM structure with GPS time.
*
* RETURN:
*  none
*
* DEPENDENCY:
*    none
*
*************************************************************************************************************
*/
void cp_wcdma_mrm_update_glo_meas_time( gps_TimeStructType const *const p_GpsTime4GloMeas, rrc_meas_rpt_struct_type *const p_measrpt )
{
  float  f_GnssTodFracPart;
  uint16 w_GnssTodFrac250ns;
  uint32 q_GnssTodMsecModHour;

  if ( (p_GpsTime4GloMeas != NULL) && (p_GpsTime4GloMeas->q_GpsMsec != 0) && (p_GpsTime4GloMeas->w_GpsWeek != 0) )
  {
    /* Using GPS Time from the Glonass measurement block */
    f_GnssTodFracPart = -(p_GpsTime4GloMeas->f_ClkTimeBias);
    q_GnssTodMsecModHour = p_GpsTime4GloMeas->q_GpsMsec;
    if ( f_GnssTodFracPart < 0.0f )
    {
      w_GnssTodFrac250ns = (uint16)((1.0f + f_GnssTodFracPart) * 4000);
      q_GnssTodMsecModHour -= 1;
      tm_umts_common_utils_GpsMsecWeekLimit((int32 *)&q_GnssTodMsecModHour, NULL);  /* Handles roll overs */
    }
    else
    {
      w_GnssTodFrac250ns = (uint16)(f_GnssTodFracPart * 4000);
    }
    if ( w_GnssTodFrac250ns > 3999 )
    {
      w_GnssTodFrac250ns = 3999;
    }
    q_GnssTodMsecModHour = q_GnssTodMsecModHour % HOUR_MSECS;
    if ( q_GnssTodMsecModHour > 3599999 )
    {
      q_GnssTodMsecModHour = 3599999;
    }

    p_measrpt->ganss_measured_results.referenceTime = GPS_REFERENCE_TIME_ONLY;
    /* Protocol : (0..3599999) */
    p_measrpt->ganss_measured_results.u.ganss_ReferenceTimeOnly.gANSS_tod = q_GnssTodMsecModHour;
    p_measrpt->ganss_measured_results.u.ganss_ReferenceTimeOnly.gANSS_timeId_incl = TRUE;
    p_measrpt->ganss_measured_results.u.ganss_ReferenceTimeOnly.gANSS_timeId = RRC_GPS_SYSTEM_TIME;

    /* tod_uncertainty */
    p_measrpt->ganss_measured_results.u.ganss_ReferenceTimeOnly.gANSS_tod_uncertainty_incl = TRUE;
    /* Accuracy of the GANSS-tod. Protocol (0..127) */
    p_measrpt->ganss_measured_results.u.ganss_ReferenceTimeOnly.gANSS_tod_uncertainty = tm_rrc_utils_microsec_to_gadk(p_GpsTime4GloMeas->f_ClkTimeUncMs * 1000);
  }
  else
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "GPS time invalid %p", p_GpsTime4GloMeas);
  }
}

#endif /* End of FEATURE_GANSS_SUPPORT */

/********************************************************************
*
* cp_wcdma_pos_method_is_gps
*
* Function description:
*   This function checks given position method ('Method' IE) is of GPS type.
*
* Parameters:
*   p_rrc_meas_ctrl : Pointer to MCM (Measurement Contol message)
*
* Return:
*  True if position method is valid GPS type
*
* Dependency:
*    None
*
*********************************************************************
*/
GNSS_INLINE boolean cp_wcdma_pos_method_is_gps( rrc_meas_ctrl_struct_type const *const p_rrc_meas_ctrl )
{
  if ( p_rrc_meas_ctrl->rpt_quan_incl &&
       ((RRC_GPS == p_rrc_meas_ctrl->rpt_quan.method) || (RRC_OTDOAorGPS == p_rrc_meas_ctrl->rpt_quan.method)) )
  {
    return ( TRUE );
  }

  return ( FALSE );
}

/************************************************************************************************************
*
* cp_wcdma_mcm_rpt_position_method_invalid
*
* FUNCTION DESCRIPTION:
*   Encode GLONASS measurement report into RRC LSM MRM structure.
*
* PARAMETERS:
*  p_rrc_meas_ctrl: Source RRC LSM MRM structure
*
* RETURN:
*  True if position method is valid.
*
* DEPENDENCY:
*    none
*
*************************************************************************************************************
*/
boolean cp_wcdma_mcm_rpt_position_method_invalid( rrc_meas_ctrl_struct_type const *const p_rrc_meas_ctrl )
{
  if ( p_rrc_meas_ctrl->rpt_quan_incl )
  {
    if ( (RRC_OTDOA == p_rrc_meas_ctrl->rpt_quan.method) || (RRC_CELLID == p_rrc_meas_ctrl->rpt_quan.method) )
    {
      return ( TRUE );
    }

#ifdef FEATURE_GANSS_SUPPORT
    if ( cp_wcdma_glonass_check(p_rrc_meas_ctrl) && p_rrc_meas_ctrl->rpt_quan.ganss_pos_method_incl )
    {
      /* Anything other than GPS+GLO is requested or Glonass only requested */
      if ( (p_rrc_meas_ctrl->rpt_quan.ganss_pos_method & ~(GANSS_POSITIONING_METHOD_GPS_BMASK | GANSS_POSITIONING_METHOD_GLONASS_BMASK)) ||
           (GANSS_POSITIONING_METHOD_GLONASS_BMASK == p_rrc_meas_ctrl->rpt_quan.ganss_pos_method)
          )
      {
        return ( TRUE );
      }
    }
#endif

  }

  return ( FALSE );
}

/********************************************************************
*
* tm_umts_cp_wcdma_session_cleanup
*
* Function description:
*   This function will release any dynamic memory & initializes CP-WCDMA session related variables..
*
* Parameters:
*   None.
*
* Return:
*  None
*
*********************************************************************
*/
void tm_umts_cp_wcdma_session_cleanup( void )
{
  umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status = &z_umts_cp_wcdma_session_status;

  /* Release any dynamic memories */

#ifdef FEATURE_GANSS_SUPPORT

  /* Release AcqAssist subset struct */
  if ( p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_subset.p_svid_icp != NULL )
  {
    os_MemFree((void **)&(p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_subset.p_svid_icp));
    p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_subset.p_svid_icp = NULL;
    p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_subset.u_cnt = 0;
  }
#endif

  /* Initialize with zeroes */
  memset(p_umts_cp_wcdma_session_status, 0, sizeof(umts_cp_wcdma_session_status_s_type));

  memset(&z_rrc_params, 0, sizeof(rrc_params_type));

  /* "allow_rrc" set to the NV default */
  p_umts_cp_wcdma_session_status->allow_rrc = z_umts_nv_item_ram_copy.default_allow_rrc;

  /* defaults to the NV item, may be changed by pdapi or network */
  p_umts_cp_wcdma_session_status->z_session_qos.q_hor_acc_meters =
                                                                   z_umts_nv_item_ram_copy.aagps_default_qos_unc;

  (void)os_TimerStop(umts_cp_wcdma_cf_timer);
  tm_umts_cp_wcdma_cancel_max_mgp_on_timer();

}


/********************************************************************
*
* tm_umts_cp_wcdma_pdapi_mapping
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
boolean tm_umts_cp_wcdma_pdapi_mapping( prtl_start_sess_req_param_s_type *start_sess_req_param )
{
  umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status = &z_umts_cp_wcdma_session_status;
  boolean                             u_ret_val                       = FALSE;

  if ( start_sess_req_param->pd_option.session == PDSM_PD_SESS_TYPE_DATA_DLOAD )
  {
    if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_3G_CP_UE_BASED) )
    {
      p_umts_cp_wcdma_session_status->e_gnssPosMode = eWCDMA_POSITION_MODE_AGPS_UE_BASED;
      u_ret_val = TRUE;
    }
  }

  else if ( start_sess_req_param->pd_option.session == PDSM_PD_SESS_TYPE_NEW )
  {

    if ( (start_sess_req_param->pd_option.operation_mode == PDSM_SESSION_OPERATION_MSBASED) ||
         (start_sess_req_param->pd_option.operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_SPEED)
        )
    {
      if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_3G_CP_UE_BASED) )
      {
        p_umts_cp_wcdma_session_status->e_gnssPosMode = eWCDMA_POSITION_MODE_AGPS_UE_BASED;
        u_ret_val = TRUE;
      }
      else if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_STANDALONE) )
      {
        p_umts_cp_wcdma_session_status->e_gnssPosMode = eWCDMA_POSITION_MODE_AGPS_CONV;
        u_ret_val = TRUE;
      }
    }
    else if ( (start_sess_req_param->pd_option.operation_mode == PDSM_SESSION_OPERATION_MSASSISTED) ||
              (start_sess_req_param->pd_option.operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_ACCURACY)
             )
    {
      if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_3G_CP_UE_ASSISTED) )
      {
        p_umts_cp_wcdma_session_status->e_gnssPosMode = eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED;
        u_ret_val = TRUE;
      }
    }
    else if ( start_sess_req_param->pd_option.operation_mode == PDSM_SESSION_OPERATION_REF_POSITION )
    {
      if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_3G_CP_UE_BASED) )
      {
        p_umts_cp_wcdma_session_status->e_gnssPosMode = eWCDMA_POSITION_MODE_AGPS_UE_BASED;
        p_umts_cp_wcdma_session_status->u_force_request_ref_location = TRUE;
        u_ret_val = TRUE;
      }
    }
    else if ( (start_sess_req_param->pd_option.operation_mode == PDSM_SESSION_OPERATION_STANDALONE_ONLY) ||
              (start_sess_req_param->pd_option.operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_DATA)
             )
    {
      if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_STANDALONE) )
      {
        /* note standalone_only should not reach here as it is handled by TM core and is not passed down to
           protocol modules; but in case it gets here, handle it gracefully */
        p_umts_cp_wcdma_session_status->e_gnssPosMode = eWCDMA_POSITION_MODE_AGPS_CONV;
        u_ret_val = TRUE;
      }
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

  if ( u_ret_val )
  {
    p_umts_cp_wcdma_session_status->q_pdapi_client_id = start_sess_req_param->client_id;
    p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs = start_sess_req_param->pd_qos.gps_session_timeout;
    p_umts_cp_wcdma_session_status->z_session_qos.q_hor_acc_meters = start_sess_req_param->pd_qos.accuracy_threshold;

    if ( start_sess_req_param->pd_option.fix_rate.num_fixes > 1 )
    {
      p_umts_cp_wcdma_session_status->u_appbased_tracking = TRUE;
    }
  }

  return ( u_ret_val );

}


/*===========================================================================

FUNCTION tm_umts_cp_wcdma_inform_core_nt_assist_end

DESCRIPTION
  This function informs the TM core that network communication is over.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/

void tm_umts_cp_wcdma_inform_core_nt_assist_end( boolean u_permit_qostime_reset )
{
  umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status = &z_umts_cp_wcdma_session_status;

  tm_sess_req_param_u_type            req_param;
  tm_sess_req_param_u_type            *p_req_param                    = &req_param;

  tm_sess_req_type                    tm_session_req;

  if ( (!u_permit_qostime_reset &&
        p_umts_cp_wcdma_session_status->qos_time_started
       ) ||
       (p_umts_cp_wcdma_session_status->u_pdapi_fix_reported)
      )
  {
    /* qos_time has started, or fix has been achieved, no need to continue */
    return;
  }
  p_umts_cp_wcdma_session_status->qos_time_started = TRUE;

  /* if TmCore session has not yet started, START; otherwise, CONTINUE */
  if ( !p_umts_cp_wcdma_session_status->tm_core_session_started )
  {
    tm_session_req = TM_SESS_REQ_START;
  }
  else
  {
    tm_session_req = TM_SESS_REQ_CONTINUE;
  }


  if ( p_umts_cp_wcdma_session_status->u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_UI )
  {

    p_req_param->continue_param.accuracy_threshold =
                                                     p_umts_cp_wcdma_session_status->z_session_qos.q_hor_acc_meters;
    p_req_param->continue_param.dynamic_qos_enabled = TRUE;

    if ( (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED) ||
         (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED) )
    {
      p_req_param->continue_param.op_mode = TM_SESS_OPERATION_MODE_MSA;
      p_req_param->continue_param.op_req = TM_OP_REQ_PRM;

      p_req_param->continue_param.prm_qos =
                                            p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs;

      p_req_param->continue_param.multi_report_msa = FALSE;

      MSG_MED("UMTS-CP-W: ntwk over, cont, prm_qos=%u, acc=%lu",
              p_req_param->continue_param.prm_qos,
              p_req_param->continue_param.accuracy_threshold,
              0);
    }
    else
    {
      p_req_param->continue_param.op_mode = TM_SESS_OPERATION_MODE_MSB;
      p_req_param->continue_param.op_req = TM_OP_REQ_LR;
      p_req_param->continue_param.dynamic_qos_enabled = TRUE;

      p_req_param->continue_param.lr_qos =
                                           p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs;

      MSG_MED("UMTS-CP-W: ntwk over, cont, lr_qos=%u, acc=%lu",
              p_req_param->continue_param.lr_qos,
              p_req_param->continue_param.accuracy_threshold,
              0);
    }

    (void)tm_sess_req(TM_PRTL_TYPE_UMTS_CP,
                      p_umts_cp_wcdma_session_status->tm_active_session_handle,
                      tm_session_req,
                      p_req_param
                     );


  } /* if UI */

  else if ( p_umts_cp_wcdma_session_status->u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_NI )
  {

    if ( tm_session_req == TM_SESS_REQ_START )
    {
      p_req_param->start_param.accuracy_threshold =
                                                    p_umts_cp_wcdma_session_status->z_session_qos.q_hor_acc_meters;

      if ( (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED) ||
           (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED) )
      {
        p_req_param->start_param.op_mode = TM_SESS_OPERATION_MODE_MSA;
        p_req_param->start_param.op_req = TM_OP_REQ_PRM;
        p_req_param->start_param.prm_qos = TM_MAX_SESSION_TIMEOUT; /* let RRC reporting timeout first, which will terminate session  */
        p_req_param->start_param.multi_report_msa = FALSE;

        if ( (z_rrc_params.rrc_periodic_multi_report == TRUE) &&
             (p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining > 1)
            )
        {
          /* Non-zero value of "timeout_value_secs" flags the state in which
             the last RRC measureControl has been processed and hence reporting
             is allowed. If it is still zero at this point, it means the phone
             did not know if the last MCM was the last one as it started checking
             with LM to see if additional request was needed. */
          if ( p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs == 0 )
          {
            if ( p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long != 0 )
            {
              p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs =
                                                                                               p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long;
            }
            else
            {
              p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs =
                                                                                               z_umts_nv_item_ram_copy.aagps_default_qos_time;
            }
          }

          p_req_param->start_param.multi_report_msa = TRUE; /* MSA multiple report */
          p_req_param->start_param.prm_qos = TM_MULTI_REPT_MAX_SESSION_TIMEOUT;  /* 600 seconds */

        } /* if multiple periodic */

        if ( tm_umts_common_utils_emergency_effect() !=
                C_EMERGENCY_HANDLING_NULL )
        {
          /* overloading this multi_report_msa that is defined to indicate
             control plane multiple reporting, for the purpose of letting
             mgp to run all the way to timeout without declaring done when
             a qualified measurement is reached */
          p_req_param->start_param.multi_report_msa = TRUE;
        }

        p_req_param->start_param.abort_curr_sess = FALSE;
        p_req_param->start_param.num_fixes = 1;
        p_req_param->start_param.tbf_ms = 1000;
        p_req_param->start_param.ppm_qos = 0;
        p_req_param->start_param.lr_qos = 0;
        p_req_param->start_param.prm_prq = 0;
        p_req_param->start_param.dynamic_qos_enabled = TRUE;

        MSG_MED("UMTS-CP-W: ntwk over, start, prm_qos=%u, acc=%lu, req=%u",
                p_req_param->start_param.prm_qos,
                p_req_param->start_param.accuracy_threshold,
                tm_session_req);
      }
      else  /* UeBased */
      {
        p_req_param->start_param.op_mode = TM_SESS_OPERATION_MODE_MSB;
        p_req_param->start_param.op_req = TM_OP_REQ_LR;

        /* let LM timeout first so that if there has been disqualified fix,
           report it before RRC times out */
        if ( p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long != 0 )
        {
          if ( p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long > 3 )
          {
            p_req_param->start_param.lr_qos =
                                              p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long - 2;
          }
          else
          {
            p_req_param->start_param.lr_qos = 1;
          }
        }
        else /* rrc response time not defined by network */
        {
          if ( z_umts_nv_item_ram_copy.aagps_default_qos_time > 3 )
          {
            p_req_param->start_param.lr_qos = z_umts_nv_item_ram_copy.aagps_default_qos_time - 2;
          }
          else
          {
            p_req_param->start_param.lr_qos = 1;
          }
        }

#ifdef FEATURE_EXTENDED_RESP_TIME
        p_req_param->start_param.lr_qos = p_umts_cp_wcdma_session_status->u_extended_ni_resp_time_sec;
#endif

        /********* for multiple periodic ****************************/
        if ( (z_rrc_params.rrc_periodic_multi_report == TRUE) &&
             (p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining > 1)
            )
        {
          /* Non-zero value of "timeout_value_secs" flags the state in which
             the last RRC measureControl has been processed and hence reporting
             is allowed. If it is still zero at this point, it means the phone
             did not know if the last MCM was the last one as it started checking
             with LM to see if additional request was needed. */
          if ( p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs == 0 )
          {
            if ( p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long != 0 )
            {
              p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs =
                                                                                               p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long;
            }
            else
            {
              p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs =
                                                                                               z_umts_nv_item_ram_copy.aagps_default_qos_time;
            }
          }
          p_req_param->start_param.lr_qos = TM_MULTI_REPT_MAX_SESSION_TIMEOUT;  /* 600 seconds */
          p_req_param->start_param.accuracy_threshold = 0;

        } /* if multiple periodic */

        p_req_param->start_param.abort_curr_sess = FALSE;
        p_req_param->start_param.num_fixes = 1;
        p_req_param->start_param.tbf_ms = 1000;
        p_req_param->start_param.ppm_qos = 0;
        p_req_param->start_param.prm_qos = 0;
        p_req_param->start_param.prm_prq = 0;
        p_req_param->start_param.dynamic_qos_enabled = TRUE;

        MSG_MED("UMTS-CP-W: ntwk over, start, lr_qos=%u, acc=%lu",
                p_req_param->start_param.lr_qos,
                p_req_param->start_param.accuracy_threshold,
                0);

      }  /* if UE-Based */

    } /* if TM_SESS_REQ_START */

    else  /************************* Continue ********************************/
    {

      p_req_param->continue_param.accuracy_threshold =
                                                       p_umts_cp_wcdma_session_status->z_session_qos.q_hor_acc_meters;
      p_req_param->continue_param.dynamic_qos_enabled = TRUE;

      if ( (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED) ||
           (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED) )
      {
        p_req_param->continue_param.op_mode = TM_SESS_OPERATION_MODE_MSA;
        p_req_param->continue_param.op_req = TM_OP_REQ_PRM;
        p_req_param->continue_param.prm_qos = TM_MAX_SESSION_TIMEOUT; /* let RRC reporting timeout first, which will terminate session  */
        p_req_param->continue_param.multi_report_msa = FALSE;

        if ( (z_rrc_params.rrc_periodic_multi_report == TRUE) &&
             (p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining > 1)
            )
        {
          /* Non-zero value of "timeout_value_secs" flags the state in which
             the last RRC measureControl has been processed and hence reporting
             is allowed. If it is still zero at this point, it means the phone
             did not know if the last MCM was the last one as it started checking
             with LM to see if additional request was needed. */
          if ( p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs == 0 )
          {
            if ( p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long != 0 )
            {
              p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs =
                                                                                               p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long;
            }
            else
            {
              p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs =
                                                                                               z_umts_nv_item_ram_copy.aagps_default_qos_time;
            }
          }

          p_req_param->continue_param.multi_report_msa = TRUE; /* MSA multiple report */
          p_req_param->continue_param.prm_qos = TM_MULTI_REPT_MAX_SESSION_TIMEOUT;  /* 600 seconds */

        } /* if multiple periodic */

        if ( tm_umts_common_utils_emergency_effect() !=
                C_EMERGENCY_HANDLING_NULL )
        {
          /* overloading this multi_report_msa that is defined to indicate
             control plane multiple reporting, for the purpose of letting
             MGP to run all the way to timeout without declaring DONE when
             a qualified measurement is reached */
          p_req_param->continue_param.multi_report_msa = TRUE;
        }

        MSG_MED("UMTS-CP-W: ntwk over, cont, prm_qos=%u, acc=%lu, req=%u",
                p_req_param->continue_param.prm_qos,
                p_req_param->continue_param.accuracy_threshold,
                tm_session_req);
      }
      else  /* UeBased */
      {
        p_req_param->continue_param.op_mode = TM_SESS_OPERATION_MODE_MSB;
        p_req_param->continue_param.op_req = TM_OP_REQ_LR;
        p_req_param->continue_param.dynamic_qos_enabled = TRUE;

        /* let LM timeout first so that if there has been disqualified fix, report it before RRC times out */
        if ( p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long != 0 )
        {
          if ( p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long > 3 )
          {
            p_req_param->continue_param.lr_qos = p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long - 2;
          }
          else
          {
            p_req_param->continue_param.lr_qos = 1;
          }
        }
        else /* rrc response time not defined by network */
        {
          if ( z_umts_nv_item_ram_copy.aagps_default_qos_time > 3 )
          {
            p_req_param->continue_param.lr_qos = z_umts_nv_item_ram_copy.aagps_default_qos_time - 2;
          }
          else
          {
            p_req_param->continue_param.lr_qos = 1;
          }
        }

#ifdef FEATURE_EXTENDED_RESP_TIME
        p_req_param->continue_param.lr_qos = p_umts_cp_wcdma_session_status->u_extended_ni_resp_time_sec;
#endif

        /**** if multiple periodic, set lr_qos to max, and accuracy to 0 *****/
        if ( (z_rrc_params.rrc_periodic_multi_report == TRUE) &&
             (p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining > 1)
            )
        {
          /* Non-zero value of "timeout_value_secs" flags the state in which
             the last RRC measureControl has been processed and hence reporting
             is allowed. If it is still zero at this point, it means the phone
             did not know if the last MCM was the last one as it started checking
             with LM to see if additional request was needed. */
          if ( p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs == 0 )
          {
            if ( p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long != 0 )
            {
              p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs =
                                                                                               p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long;
            }
            else
            {
              p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs =
                                                                                               z_umts_nv_item_ram_copy.aagps_default_qos_time;
            }
          }
          p_req_param->continue_param.lr_qos = TM_MULTI_REPT_MAX_SESSION_TIMEOUT;
          p_req_param->continue_param.accuracy_threshold = 0;
        }

        p_req_param->continue_param.dynamic_qos_enabled = TRUE;

        MSG_MED("UMTS-CP-W: ntwk over, cont, lr_qos=%u, acc=%lu",
                p_req_param->continue_param.lr_qos,
                p_req_param->continue_param.accuracy_threshold,
                0);
      }

    } /* if TM_SESS_REQ_CONTINUE */

    if ( tm_session_req == TM_SESS_REQ_CONTINUE )
    {
      (void)tm_sess_req(TM_PRTL_TYPE_UMTS_CP,
                        p_umts_cp_wcdma_session_status->tm_active_session_handle,
                        tm_session_req,
                        p_req_param
                       );
    }
    else
    {
#if 0
      /* to start a network started session, use the NI client so that TmCore
         will have a client ID and can be canceled by pdapi. If this is used,
         the tm_sess_req() call needs to be removed. */
      tm_sess_operation_mode_e_type e_op_mode;
      if ( p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED )
      {
        e_op_mode = TM_SESS_OPERATION_MODE_MSA;
      }
      else
      {
        e_op_mode = TM_SESS_OPERATION_MODE_MSB;
      }
      (void)tm_umts_common_ni_client_start_session
      ( e_op_mode,
       p_umts_cp_wcdma_session_status->z_session_qos.q_hor_acc_meters,
       MT_CP
       );
#endif
      (void)tm_sess_req(TM_PRTL_TYPE_UMTS_CP,
                        p_umts_cp_wcdma_session_status->tm_active_session_handle,
                        tm_session_req,
                        p_req_param
                       );
    }

  } /* if NI */

  /**** When this function is called, it is assumed that the UE is no longer
   expecting more assistance data from the network. Based on the number of alm.
   and eph. SVs, the protocol module can derive which SVs don't exist, and should
   inform the MGP about this */





}


/*===========================================================================

FUNCTION
  tm_umts_cp_wcdma_lcs_loc_notif_ind_proc

DESCRIPTION
  Processing SS LCS_Location_Notification_Ind

PARAMETERS
   p_sup_data - Pointer to an SS data structure.

RETURN VALUE
  Void

===========================================================================*/

void tm_umts_cp_wcdma_lcs_loc_notif_ind_proc( tm_cm_sups_data_s_type *p_sups_data )
{

  if ( p_sups_data->lcs_mess.location_notification_ind.notification_type ==
          NOTIFY_LOCATION_ABORTED )
  {
    /* A Release Complete or Reject has been received from the network,
       stop MTLR guard timer in case it is on */
    tm_umts_cp_wcdma_terminate_timer_cancel_cf_state(C_UMTS_CP_WCDMA_CF_STATE_MT_GUARD_TIME);
  }

}


/********************************************************************
*
* tm_umts_cp_wcdma_set_cf_state
*
* Function description:
*   Sets the nature of the call flow for the current session.
*
* Parameters:
*   enum of None, NetworkInitiated, SetInitiated, or KeepWarm.
*   boolean u_cf_molr_invoke_timer  [sec]
*     If TRUE,  use NV default NV_AAGPS_DEFAULT_SMLC_COMM_TIMEOUT_V for cf timer
*     If FALSE, use the CP_WCDMA_CF_TIMER_WAIT_MC_DEFAULT
*
* Return:
*  None
*
*********************************************************************
*/
void tm_umts_cp_wcdma_set_cf_state( umts_cp_wcdma_cf_state_enum_type e_new_cf_state, boolean u_cf_molr_invoke_timer )
{
#ifdef FEATURE_WCDMA_CP_CF_WAIT_RRC_TIMER
  #error code not present
#endif
  umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status = &z_umts_cp_wcdma_session_status;

  /* this should not happen */
  if ( e_new_cf_state == C_UMTS_CP_WCDMA_CF_STATE_NONE )
    return;

  (void)os_TimerStop(umts_cp_wcdma_cf_timer); /* stop current running timer, if running */

  if ( (e_new_cf_state == C_UMTS_CP_WCDMA_CF_STATE_WAIT_RRC) ||
       (e_new_cf_state == C_UMTS_CP_WCDMA_CF_STATE_WAIT_RETURN_RESULT)
      )
  {
#ifdef FEATURE_WCDMA_CP_CF_WAIT_RRC_TIMER
    #error code not present
#else
    /* use the NV value for aagps_wait_smlc */
    (void)os_TimerStart(umts_cp_wcdma_cf_timer, z_umts_nv_item_ram_copy.default_smlc_comm_timeout_sec * 1000, NULL);

    if ( e_new_cf_state == C_UMTS_CP_WCDMA_CF_STATE_WAIT_RRC )
    {
      p_umts_cp_wcdma_session_status->allow_rrc = TRUE;
      MSG_MED("UMTS-CP-W: start WaitRRC timer: %u sec.", z_umts_nv_item_ram_copy.default_smlc_comm_timeout_sec, 0, 0);
    }
    else if ( e_new_cf_state == C_UMTS_CP_WCDMA_CF_STATE_WAIT_RETURN_RESULT )
    {
      MSG_MED("UMTS-CP-W: start WaitMoLrRetRes timer: %u sec.", z_umts_nv_item_ram_copy.default_smlc_comm_timeout_sec, 0, 0);
    }
#endif /* FEATURE_WCDMA_CP_CF_WAIT_RRC_TIMER */
  }

  else if ( e_new_cf_state & C_UMTS_CP_WCDMA_CF_STATE_TIMER_RRC_REPORT )
  {
    uint32 q_rrc_timer = 0;

#ifdef FEATURE_EXTENDED_RESP_TIME

    if ( p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long != 0 )
    {
      q_rrc_timer =
                    p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long -
                    p_umts_cp_wcdma_session_status->q_additional_req_margin_sec;
    }
    else /* w_rrc_reporting_interval_long == 0 means response time undefined by network */
    {
      q_rrc_timer =
                    z_umts_nv_item_ram_copy.aagps_default_qos_time -
                    p_umts_cp_wcdma_session_status->q_additional_req_margin_sec;
    }
#else
    if ( p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long != 0 )
    {
      q_rrc_timer =
                    p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long;
    }
    else /* w_rrc_reporting_interval_long == 0 means response time undefined by network */
    {
      q_rrc_timer =
                    z_umts_nv_item_ram_copy.aagps_default_qos_time;
    }
#endif

    (void)os_TimerStart(umts_cp_wcdma_cf_timer, q_rrc_timer * 1000, NULL);
    MSG_MED("UMTS-CP-W: start RRC report timer: %u sec.", q_rrc_timer, 0, 0);

  }

  else if ( e_new_cf_state & C_UMTS_CP_WCDMA_CF_STATE_MT_GUARD_TIME )
  {
    (void)os_TimerStart(umts_cp_wcdma_cf_timer, z_umts_nv_item_ram_copy.default_mtlr_guard_timer_sec * 1000, NULL);
    p_umts_cp_wcdma_session_status->allow_rrc = TRUE;

    MSG_MED("Start allowing RRC for the next %u seconds",
            z_umts_nv_item_ram_copy.default_mtlr_guard_timer_sec, 0, 0);
  }


  p_umts_cp_wcdma_session_status->e_cf_state = e_new_cf_state;

  MSG_MED("UMTS-CP-W: set timer; cf state = %u", (uint8)e_new_cf_state, 0, 0);

}


/********************************************************************
*
* tm_umts_cp_wcdma_callflow_disrupt_handler
*
* Function description:
*   this function properly handles call flow disruption.
*
* Parameters:
*   None
*
* Return:
*  None
*
*********************************************************************
*/

void tm_umts_cp_wcdma_callflow_disrupt_handler( void )
{
  umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status = &z_umts_cp_wcdma_session_status;

  if ( (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_BASED) &&
       (p_umts_cp_wcdma_session_status->u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_UI) )
  {
    z_umts_cp_wcdma_pd_info_buffer.client_id = z_umts_cp_wcdma_session_status.q_pdapi_client_id;
    if ( tm_umts_cp_ss_is_molr_on_comp() == TRUE )
    {
      /* After MOLR has been passed to CM, received CM indication of MOLR-complete
        without getting prior indication of receiving the FACILITY message, or
        without TM's prior action of actively asking CM for "release complete"
        --- this is an indication of a failure in actually sending MOLR to the
        network". */

      z_umts_cp_wcdma_pd_info_buffer.pd_info.end_status = PDSM_PD_END_MO_LR_NOT_DELIVERED;
    }
    else
    {
      z_umts_cp_wcdma_pd_info_buffer.pd_info.end_status = PDSM_PD_END_CON_FAIL;
    }

    (void)tm_post_pd_event(TM_PRTL_TYPE_UMTS_CP,
                           p_umts_cp_wcdma_session_status->tm_active_session_handle,
                           PDSM_PD_EVENT_UPDATE_FAILURE,
                           &z_umts_cp_wcdma_pd_info_buffer,
                           NULL);

    /* clear network related session timer and states */
    (void)os_TimerStop(umts_cp_wcdma_cf_timer);
    p_umts_cp_wcdma_session_status->e_cf_state = C_UMTS_CP_WCDMA_CF_STATE_NONE;

    p_umts_cp_wcdma_session_status->u_network_session_on = FALSE;

    #ifdef FEATURE_CGPS_DUAL_WCDMA
    tm_umts_common_utils_mm_session_status_notify(FALSE, z_umts_cp_wcdma_session_status.z_rrc_as_id_info.e_cached_rrc_as_id); /* inform MM LCS over */
	#else
	tm_umts_common_utils_mm_session_status_notify(FALSE); /* inform MM LCS over */
	#endif

    tm_umts_cp_wcdma_inform_core_nt_assist_end(FORBID_QOS_TIME_RESET); /* tell TmCore to work on its own */

    tm_umts_cp_ss_ss_completion_req(); /* inform SS of the end of network session */
  }
  else
  {
    tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
  }

}


/********************************************************************
*
* tm_umts_cp_wcdma_terminate_timer_cancel_cf_state
*
* Function description:
*   this function terminates timer(s) and cancels either one
*   call flow state, or all of them.
*
* Parameters:
*   e_timer_state_to_terminate - the call flow state to cancel; if
*     C_UMTS_CP_WCDMA_CF_STATE_NULL, cancel all.
*
* Return:
*  None
*
*********************************************************************
*/

void tm_umts_cp_wcdma_terminate_timer_cancel_cf_state( umts_cp_wcdma_cf_state_enum_type e_timer_state_to_terminate )
{
  umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status = &z_umts_cp_wcdma_session_status;

  (void)os_TimerStop(umts_cp_wcdma_cf_timer);

  /*lint -e{64} safe use as enum is used positive value */
  p_umts_cp_wcdma_session_status->e_cf_state &= ~e_timer_state_to_terminate;

}



/********************************************************************
*
* tm_umts_cp_wcdma_lcs_molr_conf_proc
*
* Function description:
*   this function properly handles information in SS lcs_molr_conf
*
* Parameters:
*
*
* Return:
*  None
*
*********************************************************************
*/

void tm_umts_cp_wcdma_lcs_molr_conf_proc( tm_cm_sups_data_s_type *p_sups_data )
{
  umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status;
  pdsm_pd_info_s_type                 *p_pd_info_buffer;
  lcs_molr_res_s_type                 *p_molr_res;
  uint8                               *p_data                         = NULL;
  uint32                              q_time_stamp_sec                = 0;
  pdsm_pd_event_type                  q_pd_event;   /* uint32 */
  lcs_molr_res_data_T                 *p_lcs_molr_res_data;
  gnss_time_info                      *p_msa_pos_time_stamp;
  tm_post_data_payload_type           z_tm_payload;
  boolean                             u_ref_loc_ok                    = FALSE;
  boolean                             u_pd_info_ok                    = FALSE;
  tm_sess_stop_reason_e_type          e_stop_reason                   = TM_STOP_REASON_COMPLETED;
  ie_cm_ss_error_T                    *p_molr_res_err;
  gnss_NavSlnFlagsStructType          z_pos_flags;

  /*
   *  Initialize pointers to make code easier to read/write
   */
  memset(&z_pos_flags, 0x00, sizeof(z_pos_flags));
  memset(&z_tm_payload, 0x00, sizeof(tm_post_data_payload_type));
  memset(&q_pd_event, 0x00, sizeof(pdsm_pd_event_type));

  p_umts_cp_wcdma_session_status = &z_umts_cp_wcdma_session_status;
  p_pd_info_buffer = &z_umts_cp_wcdma_pd_info_buffer;
  p_molr_res = &p_sups_data->lcs_mess.lcs_molr_res;
  p_lcs_molr_res_data = &p_sups_data->lcs_mess.lcs_molr_res.data.molr_res_data;
  p_msa_pos_time_stamp =
                         &p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp;

  /* when receiving this, network session is over */
  p_umts_cp_wcdma_session_status->u_network_session_on = FALSE;

  if ( p_umts_cp_wcdma_session_status->e_cf_state == C_UMTS_CP_WCDMA_CF_STATE_WAIT_RETURN_RESULT )
  {
    tm_umts_cp_wcdma_terminate_timer_cancel_cf_state(C_UMTS_CP_WCDMA_CF_STATE_WAIT_RETURN_RESULT);

    if ( p_molr_res->data_type == LCS_MOLR_INFO )
    {

      if ( p_lcs_molr_res_data->location_estimate.present )
      {
        /* Location Estimate Present */
        p_data = &p_lcs_molr_res_data->location_estimate.data[0];
      }
      else if ( p_lcs_molr_res_data->add_location_estimate.present )
      {
        /* Additional Location Estimate Present */
        p_data = &p_lcs_molr_res_data->add_location_estimate.data[0];
      }

      /* for AssistanceData mode, MOLR_ReturnResult without position will
         arrive soon after the last RRC message is delivered; if pdapi
         report has not been sent yet, treat this as the indicator of the
         end of assist. data from the network. See if info. about any SVs'
         non-existence can be deduced and if so inform MGP; The check is
         done so that even if the network in this case sends MOLR_ReturnResult
         with a position in it, the same mechanism of determining non-exist
         SVs will be apppied. */
      if ( p_umts_cp_wcdma_session_status->mo_lr_type == ASSISTANCE_DATA )
      {
        tm_util_post_non_exist_svs(TM_PRTL_TYPE_UMTS_CP,
                                   p_umts_cp_wcdma_session_status->tm_active_session_handle,
                                   p_umts_cp_wcdma_session_status->q_alm_delivered_sv_list,
                                   p_umts_cp_wcdma_session_status->q_eph_delivered_sv_list);

        tm_umts_cp_wcdma_inform_core_nt_assist_end(FORBID_QOS_TIME_RESET);
      }

      if ( p_data != NULL )
      {
        MSG_MED(" UMTS-CP-W: MOLR-RES LocEst received", 0, 0, 0);

        if ( p_umts_cp_wcdma_session_status->u_pdapi_fix_reported == FALSE )
        {
          memset(p_pd_info_buffer, 0, sizeof(pdsm_pd_info_s_type));

          if ( p_msa_pos_time_stamp->m.posTimeStampValid )
          {
            q_time_stamp_sec = (uint32)(p_msa_pos_time_stamp->t_pos_time_stamp_ms / 1000);

            /* clear the time stamp validity */
            p_msa_pos_time_stamp->m.posTimeStampValid = FALSE;
            p_msa_pos_time_stamp->m.refTimeWeekNumValid = FALSE;
          }

          /*
           *  Attempt to parse position information into a structure that
           *  can be used to POST position information to TM.  This will
           *  be used for 2 purposes:
           *    1) To provide EXTENDED PD data to PDAPI
           *    2) To provide SEED position to MGP
           */
          u_ref_loc_ok =
                         tm_umts_common_utils_gad_to_ref_loc(
                                                             p_data,
                                                             q_time_stamp_sec,
                                                             &z_tm_payload.seed_data);

          u_pd_info_ok = tm_umts_common_utils_gad_to_pdsm_pd_info
                         (
                          p_umts_cp_wcdma_session_status->q_pdapi_client_id,
                          p_data,
                          PDSM_PD_POSITION_TYPE_NEW,
                          PDSM_PD_POSITION_MODE_MSASSISTED,
                          PDSM_PD_POSITION_SOURCE_GPS | PDSM_PD_POSITION_SOURCE_GLO,
                          q_time_stamp_sec,
                          p_pd_info_buffer,
                          &q_pd_event
                         );
        }

        /*
         *  IF parsing was successful THEN
         *    post data to TM
         *    post event to PDAPI
         */
        if ( u_ref_loc_ok && u_pd_info_ok )
        {
          (void)tm_post_data(
                             TM_PRTL_TYPE_UMTS_CP,
                             p_umts_cp_wcdma_session_status->tm_active_session_handle,
                             TM_POST_DATA_TYPE_LR,
                             &z_tm_payload);

          if ( q_pd_event & PDSM_PD_EVENT_HEIGHT )
          {
            z_pos_flags.b_Is3D = TRUE;
          }
          if ( q_pd_event & PDSM_PD_EVENT_VELOCITY )
          {
            z_pos_flags.b_IsVelValid = TRUE;
          }

          (void)tm_post_pd_event(TM_PRTL_TYPE_UMTS_CP,
                                 p_umts_cp_wcdma_session_status->tm_active_session_handle,
                                 q_pd_event,
                                 p_pd_info_buffer,
                                 &z_pos_flags);
        }   /* if parsing was successful */

        /*
         *  Tell TmCore to end and send DONE event to pdapi, also clear
         *  UMTS-CP-WCDMA and UMTS-CP-SS
         */
        tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_COMPLETED);

      } /* p_data != NULL */

      else  /* no position info. available */
      {
        /* tell TmCore to end and send DONE event to pdapi, also clear UMTS-CP-WCDMA and UMTS-CP-SS */
        if ( (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED) ||
             (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED) )
        {
          if ( p_umts_cp_wcdma_session_status->rrc_msr_report_timeout )
          {
            tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_PRM_TIMEOUT);
          }
          else
          {
            tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
          }
        }
        else /* UeBased */
        {
          if ( p_umts_cp_wcdma_session_status->u_pdapi_fix_reported )
          {
            tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_COMPLETED);
          }
          else
          {
            if ( p_umts_cp_wcdma_session_status->mo_lr_type == LOCATION_ESTIMATE )
            {
              /* this is the case where UepErr-NotEnoughSvs was first sent to the
                 network and the network responds with MOLR-ReturnResult with no
                 location */
              tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_FIX_TIMEOUT);
            }
          }
        }

      } /* containing no position info */
    } /* p_molr_res->data_type == LCS_MOLR_INFO */

    else if ( p_molr_res->data_type == SS_ERROR_INFO )
    {
      p_molr_res_err = &p_molr_res->data.cm_ss_error;
      switch ( p_molr_res_err->error_code )
      {
         case SS_MOLR_ERRORS_SYSTEM_FAILURE:
         {
           e_stop_reason = TM_STOP_REASON_SS_MOLR_ERRORS_SYSTEM_FAILURE;
           break;
         }

         case SS_MOLR_ERRORS_UNEXPECTED_DATA_VALUE:
         {
           e_stop_reason = TM_STOP_REASON_SS_MOLR_ERRORS_UNEXPECTED_DATA_VALUE;
           break;
         }

         case SS_MOLR_ERRORS_DATA_MISSING:
         {
           e_stop_reason = TM_STOP_REASON_SS_MOLR_ERRORS_DATA_MISSING;
           break;
         }

         case SS_MOLR_ERRORS_FACILITY_NOT_SUPPORTED:
         {
           e_stop_reason = TM_STOP_REASON_SS_MOLR_ERRORS_FACILITY_NOT_SUPPORTED;
           break;
         }

         case SS_MOLR_ERRORS_SS_SUBSCRIPTION_VIOLATION:
         {
           e_stop_reason = TM_STOP_REASON_SS_MOLR_ERRORS_SS_SUBSCRIPTION_VIOLATION;
           break;
         }

         case SS_MOLR_ERRORS_POSITION_METHOD_FAILURE:
         {
           e_stop_reason = TM_STOP_REASON_SS_MOLR_ERRORS_POSITION_METHOD_FAILURE;
           break;
         }

         default:
         {
           e_stop_reason = TM_STOP_REASON_SS_MOLR_ERRORS_UNDEFINED;
           break;
         }
      } /* close switch-error-code */
      tm_umts_cp_wcdma_inform_core_stop_session(e_stop_reason);
    }
    else
    {
      tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);

    }
  } /* has been waiting for C_UMTS_CP_WCDMA_CF_STATE_WAIT_RETURN_RESULT */

  else /* not in that wait_for_return_result state */
  {
    /* Only MO will get MoLrReturnResult, keep the position for possible later
       use, if position is available */

    /* if pdapi report has already sent, end the session */
    if ( p_umts_cp_wcdma_session_status->u_pdapi_fix_reported )
    {
      tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_COMPLETED);
    }
  }

}


/********************************************************************
*
* tm_umts_cp_wcdma_rrc_proc
*
* Function description:
*   this function properly handles information in an RRC event.
*
* Parameters:  prtl_rrc_event_s_type *p_rrc_event
*
*
* Return:
*  None
*
*********************************************************************
*/

void tm_umts_cp_wcdma_rrc_proc( prtl_rrc_event_s_type *rrc_event_payload_ptr )
{

  rrc_meas_params_struct_type *p_rrc_meas_param = NULL;

  if (NULL == rrc_event_payload_ptr)
  {
    MSG_ERROR("CP-WCDMA: NULL ptr passed to tm_umts_cp_wcdma_rrc_proc",0, 0, 0);
    return;
  }

  if ( rrc_event_payload_ptr->rrc_event_data_ptr->e_event_type !=
          RRC_CGPS_RRC_MSG_EVENT
      )
  {
    MSG_ERROR("Undefined RRC event type: %u",
              rrc_event_payload_ptr->rrc_event_data_ptr->e_event_type, 0, 0);

    return;
  }

  #ifdef FEATURE_CGPS_DUAL_WCDMA
  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "CP-WCDMA: Handle_rrc_event() called with as_id %d ",(uint8)rrc_event_payload_ptr->rrc_event_data_ptr->e_event_as_id);
  z_umts_cp_wcdma_session_status.z_rrc_as_id_info.e_current_rrc_as_id = rrc_event_payload_ptr->rrc_event_data_ptr->e_event_as_id;

  /* Validate AS_ID */
  if (!tm_umts_cp_wcdma_validate_as_id(rrc_event_payload_ptr->rrc_event_data_ptr->e_event_as_id))
  {
    MSG_HIGH("CP-WCDMA: AS_ID sent from RRC failed validation", 0, 0, 0);
    return;
  }	
  #endif

  p_rrc_meas_param =
                     rrc_event_payload_ptr->rrc_event_data_ptr->u.rrc_meas_param_ptr;

  /************ log the arriving RRC message ********************************/
  /*Note : Use a new DM log Id if rrc_meas_params_struct_type struct size increase above 65K bytes */
  sm_log_svr_txrx_rep(C_LSM_LOG_SVR_RX_TYPE,
                      5,
                      C_LSM_SVR_PROTOCOL_TYPE_RRC_GANSS_LSM,
                      (uint16)(sizeof(rrc_meas_params_struct_type) & 0xFFFF),
                      (uint8 *)p_rrc_meas_param);


  if ( p_rrc_meas_param->params_type == RRC_MEAS_CTRL_PARMS )
  {
    MSG_MED("CP-WCDMA got RRC_MEAS_CTRL", 0, 0, 0);
    cp_wcdma_meas_control_proc(&p_rrc_meas_param->u.meas_ctrl_params);
  }

  else if ( p_rrc_meas_param->params_type == RRC_ASSISTANCE_DATA_PARMS )
  {
    MSG_MED("CP-WCDMA got RRC_ASSIST_DATA", 0, 0, 0);
    cp_wcdma_assist_delivery_proc(&p_rrc_meas_param->u.assistance_data_params);
  }

  else if ( p_rrc_meas_param->params_type == RRC_STATE_CHANGE_PARMS )
  {

    rrc_saved_meas_params_s_type *p_SavedParams = &z_rrc_params.z_saved_meas;

    MSG_MED("CP-WCDMA got RRC_STATE_CHANGE", 0, 0, 0);

    /* State change has occurred */
    switch ( p_rrc_meas_param->u.trans_params.trans_cmd_type )
    {
       case RRC_MEAS_DELETE:
       case RRC_MEAS_DELETE_ALL:
       {
         MSG_MED("RRC_STATE_CHANGE MeasDelete/MeasDeleteAll", 0, 0, 0);

         /* If the Measurement is currently saved, invalidate it */
         if ( (p_SavedParams->u_Saved) &&
              (p_SavedParams->u_MeasId == p_rrc_meas_param->u.trans_params.meas_id) )
         {
           if ( p_SavedParams->p_SavedMessBuff != NULL )
           {
             /* Free up saved message */
             os_MemFree((void **)&(p_SavedParams->p_SavedMessBuff));
             p_SavedParams->p_SavedMessBuff = NULL;
           }

           p_SavedParams->u_Saved = FALSE;
         }

         /* whenever we receive this, cancel any ongoing RRC measurement */
         tm_umts_cp_wcdma_callflow_disrupt_handler();

         break;
       }

       case RRC_MEAS_SAVE:
       {
         MSG_MED("RRC_STATE_CHANGE MeasSave", 0, 0, 0);

         /* Mark the fact that this measurement is currently suspended */
         p_SavedParams->u_Saved = TRUE;
         p_SavedParams->u_MeasId = p_rrc_meas_param->u.trans_params.meas_id;
         break;
       }

       case RRC_MEAS_RESUME:
       {

         MSG_MED("RRC_STATE_CHANGE MeasResume", 0, 0, 0);

         if ( (p_SavedParams->u_Saved == TRUE) &&
              (p_SavedParams->u_MeasId == p_rrc_meas_param->u.trans_params.meas_id) )
         {
           /* If there is a message saved, send it out now */
           if ( p_SavedParams->p_SavedMessBuff != NULL )
           {
             /* Send RRC whatever is in our saved measurement message buffer */
             (void)cp_wcdma_rrc_send_measrpt(p_SavedParams->p_SavedMessBuff);

             if ( z_umts_cp_wcdma_session_status.u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_UI )
             {
               /* wait for MOLR_ReturnResult */
               tm_umts_cp_wcdma_set_cf_state(C_UMTS_CP_WCDMA_CF_STATE_WAIT_RETURN_RESULT, FALSE);

               if ( (z_umts_cp_wcdma_session_status.e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED) ||
                    (z_umts_cp_wcdma_session_status.e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED) )
               {
                 /* a workaround to time stamp position to be sent from the server in SS MoLrReturnResult:
                    use the reference time's week number and the measurement report's msec. */
                 if ( z_umts_cp_wcdma_session_status.z_set_assist_pos_time_stamp.m.refTimeWeekNumValid )
                 {
                   z_umts_cp_wcdma_session_status.z_set_assist_pos_time_stamp.t_pos_time_stamp_ms =
                       tm_umts_common_utils_full_gps_ms(
                           z_umts_cp_wcdma_session_status.z_set_assist_pos_time_stamp.w_ref_time_week_num,
                           p_SavedParams->p_SavedMessBuff->gps_measured_results.u.gps_reference_time);

                   z_umts_cp_wcdma_session_status.z_set_assist_pos_time_stamp.m.posTimeStampValid = TRUE;
                 }
               } /* if UeAssisted */
             } /* if UI */
             else
             {
               /* NI: send DONE event to pdapi and end the session */
               tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_COMPLETED);
             }

             /* De-allocate saved message buffer */
             if ( p_SavedParams->p_SavedMessBuff != NULL )
             {
               os_MemFree((void **)&(p_SavedParams->p_SavedMessBuff));
               p_SavedParams->p_SavedMessBuff = NULL;
             }

           }

           /* Clear the saved measurement indicator */
           p_SavedParams->u_Saved = FALSE;
         }
         break;
       }

#ifdef FEATURE_LSM_PCH_REPORTING
#error code not present
#endif

    } /* switch */
  }

  else
  {
    /* shouldn't get here */
    MSG_HIGH("RRC meas. param. not to be processed: %u", p_rrc_meas_param->params_type, 0, 0);
  }

}



/********************************************************************
*
* tm_umts_cp_wcdma_mm_proc
*
* Function description:
*   this function properly handles information in an MM event.
*
* Parameters:  prtl_mm_event_s_type *p_mm_event
*
*
* Return:
*  None
*
*********************************************************************
*/

void tm_umts_cp_wcdma_mm_proc( prtl_mm_event_s_type *p_mm_event )
{

  if ( p_mm_event->mm_event_data_ptr->e_event_type == MM_CGPS_MSG_EVENT_ABORT )
  {
    /* received LCS abort command from MM */
    MSG_HIGH("got LCS abort from MM -- no action needed", 0, 0, 0);
  }

  else if ( p_mm_event->mm_event_data_ptr->e_event_type == MM_CGPS_MSG_EVENT_RESET_UE_POS_INFO )
  {
    /* received "OTA cold start" command from MM */
    MSG_HIGH("got reset-UE-pos-info command from MM - already processed", 0, 0, 0);
  }

}


/********************************************************************
*
* tm_umts_cp_wcdma_pa_proc
*
* Function description:
*   this function properly handles information from pdapi pa events.
*
* Parameters:  prtl_mm_event_s_type *p_mm_event
*
*
* Return:
*  None
*
*********************************************************************
*/

void tm_umts_cp_wcdma_pa_proc( prtl_pa_event_s_type *p_pa_event )
{
  if ( (p_pa_event->pa_event == PDSM_PA_DELETE_PARAMS_EX) ||
       (p_pa_event->pa_event == PDSM_PA_DELETE_PARAMS) || /* PDSM_PA_DELETE_PARAMS is deprecated. */
       (p_pa_event->pa_event == PDSM_PA_DELETE_PARAMS_EX1) ) /* PDSM_PA_DELETE_PARAMS is deprecated.
                                                            use PDSM_PA_DELETE_PARAMS_EX1 instead */
  {
    /* cancel holdoff timer when the corresponding param. is deleted */

    MSG_5(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Clear WCDMA holdoff timers. PA delete param 0x%lX, Eph 0x%lX, Alm 0x%lX, GloEph 0x%lX, GloAlm 0x%lX",
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
      (void)os_TimerStop(cp_wcdma_eph_req_holdoff_timer);
    }
    if ( p_pa_event->pa_info_ptr->delete_params.pdsm_delete_parms_flags &
            PDSM_PA_DELETE_ALM
        )
    {
      (void)os_TimerStop(cp_wcdma_alm_req_holdoff_timer);
    }
    if ( p_pa_event->pa_info_ptr->delete_params.pdsm_delete_parms_flags &
            PDSM_PA_DELETE_IONO
        )
    {
      (void)os_TimerStop(cp_wcdma_iono_req_holdoff_timer);
    }
    if ( p_pa_event->pa_info_ptr->delete_params.pdsm_delete_parms_flags &
            PDSM_PA_DELETE_UTC
        )
    {
      (void)os_TimerStop(cp_wcdma_utc_req_holdoff_timer);
    }
    if ( p_pa_event->pa_info_ptr->delete_params.pdsm_delete_parms_flags &
            PDSM_PA_DELETE_POS
        )
    {
      (void)os_TimerStop(cp_wcdma_refloc_req_holdoff_timer);
    }

#ifdef FEATURE_GANSS_SUPPORT
    if ( tm_umts_common_glonass_enabled() && tm_umts_cp_wcdma_a_glo_rrc_cp_enabled() )
    {
      if ( p_pa_event->pa_info_ptr->delete_params.pdsm_delete_parms_flags & PDSM_PA_DELETE_EPH_GLO )
      {
        if ( p_pa_event->pa_info_ptr->delete_params.glo_eph_sv_mask != 0 )
        {
          (void)os_TimerStop(cp_wcdma_glo_eph_req_holdoff_timer);
          /* Clear the associated AD timers */
          (void)os_TimerStop(cp_wcdma_glo_auxi_info_req_holdoff_timer);
          (void)os_TimerStop(cp_wcdma_glo_time_model_req_holdoff_timer);
        }
      }

      if ( p_pa_event->pa_info_ptr->delete_params.pdsm_delete_parms_flags & PDSM_PA_DELETE_ALM_GLO )
      {
        if ( p_pa_event->pa_info_ptr->delete_params.glo_alm_sv_mask != 0 )
        {
          (void)os_TimerStop(cp_wcdma_glo_alm_req_holdoff_timer);
        }
      }
    }
#endif

  } /* delete param */
}

/*===========================================================================

FUNCTION tm_umts_cp_wcdma_handle_lr_data

DESCRIPTION handles TmCore provided LR data.

DEPENDENCIES
  None.

RETURN VALUE
  none.

SIDE EFFECTS
  None.

===========================================================================*/
void tm_umts_cp_wcdma_handle_lr_data( sm_GnssFixRptStructType *lr_data_ptr, boolean u_is_final )
{
  umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status = &z_umts_cp_wcdma_session_status;

  pdsm_pd_info_s_type                 *p_pd_info_buffer               = &z_umts_cp_wcdma_pd_info_buffer;
  pdsm_pd_event_type                  pd_event                        = PDSM_PD_EVENT_POSITION;

  gad_ellip_alt_unc_ellip_type        z_ellipse;

  tm_lr_resp_type                     *lr_resp_ptr                    = NULL;
  tm_pd_position_info_s_type          z_tm_pd_position_info;
  tm_ext_pos_data_s_type              z_tm_ext_pos_data;
  utc_time                            z_utc_time;
  boolean                             v_sess_type_ni = FALSE; 

  MSG_MED("CP-WCDMA: Handle LR data: Final=%u", u_is_final, 0, 0);

  if ( p_umts_cp_wcdma_session_status->u_pdapi_fix_reported &&
       !z_rrc_params.rrc_periodic_multi_report
      )
  {
    /* if position has been passed on to pdapi via TmCore already in this session,
       don't repeat that */
    MSG_MED("LR ignored: pdapi report already done", 0, 0, 0);
    return;
  }

  if ( NULL == lr_data_ptr )
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null data buffer");
    return; 
  }


  if ( C_UMTS_CP_WCDMA_SESSION_NI == p_umts_cp_wcdma_session_status->u_callflow_kind)
  {
    v_sess_type_ni =  TRUE;
  }

  if ( u_is_final == FALSE )  /* non-final LR, only for multiple periodic */
  {
    /* if multiple periodic */
    if ( z_rrc_params.rrc_periodic_multi_report )
    {
      /* if ready to report */
      if ( p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs != 0 )
      {
        if ( p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining > 0 )
        {
          /* Count the interval time (epoch) */
          p_umts_cp_wcdma_session_status->w_lm_report_count++;

          /* Wait for full interval time to get better intermediate fix  */
          if ( (p_umts_cp_wcdma_session_status->w_lm_report_count >= p_umts_cp_wcdma_session_status->q_cp_periodic_reporting_interval_sec) )
          {
            /* convert position in LR into GAD shape */
            (void)tm_umts_common_utils_construct_ellip_alt_unc_ellip_from_lr(&z_ellipse, lr_data_ptr, TM_CONF_REPORTED_FROM_PE, v_sess_type_ni); 

            /* report to RRC */
            (void)tm_umts_cp_wcdma_encode_rrc_msrReport_pos(&z_ellipse, lr_data_ptr->z_NavPos.q_GpsTimeMs, lr_data_ptr);

            /* For the first position report only */
            if ( p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining ==
                    p_umts_cp_wcdma_session_status->w_cp_periodic_reports_total_num )
            {
              tm_umts_cp_wcdma_terminate_timer_cancel_cf_state(C_UMTS_CP_WCDMA_CF_STATE_TIMER_RRC_REPORT);

              MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "1st periodic position reported");
            }

            p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining--;

            MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Periodic position report status: total=%u, remain=%u, lm_rpt_cnt=%u",
                  p_umts_cp_wcdma_session_status->w_cp_periodic_reports_total_num,
                  p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining,
                  p_umts_cp_wcdma_session_status->w_lm_report_count);

            /* reset interval count */
            p_umts_cp_wcdma_session_status->w_lm_report_count = 0;

            /* if done: close session, else keep waiting */
            if ( p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining == 0 )
            {
              tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_COMPLETED);
            }
          }
          else
          {
            /* Elapsed time isn't yet hit the interval time */
            MSG_MED("skipping subsqnt interm report: total=%u, remain=%u, lm_rpt_cnt=%u",
                    p_umts_cp_wcdma_session_status->w_cp_periodic_reports_total_num,
                    p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining,
                    p_umts_cp_wcdma_session_status->w_lm_report_count);
          }
        } /* if any periodic report is remaining */
      } /* if ready to report to RRC */
    } /* if multiple periodic  */
    return;
  } /* if position not FINAL */

  if ( !p_umts_cp_wcdma_session_status->u_pdapi_fix_reported )
  {
    lr_resp_ptr = (tm_lr_resp_type *)pdsm_getbuf(sizeof(tm_lr_resp_type)); /*lint !e826 */

    if ( lr_resp_ptr != NULL )
    {

      /* initialize buffer for pdapi */
      memset(p_pd_info_buffer, 0, sizeof(pdsm_pd_info_s_type));

      /* Convert from LM to IS801 format: simply to take advantage of the readily available
         function to convert position and timestamp to the format the TmCore interface uses */
      if ( tm_util_convert_lr(lr_data_ptr, lr_resp_ptr) )
      {
        tm_util_xlate_nav_soln_to_tm_pos_info(NULL,
                                              &z_tm_pd_position_info,
                                              &lr_data_ptr->z_NavPos,
                                              PDSM_PD_DLOAD_LOCATION,         /* dont care */
                                              PDSM_PD_DLOAD_SOURCE_STANDALONE /* dont care */);

        /* set up proper position mode in the pdapi report */
        if ( (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_BASED) ||
             (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_BASED) )
        {
          z_tm_pd_position_info.position_mode = PDSM_PD_POSITION_MODE_MSBASED;
        }
        else if ( (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED) ||
                  (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED) )
        {
          z_tm_pd_position_info.position_mode = PDSM_PD_POSITION_MODE_MSASSISTED;
        }
        else if ( p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_CONV )
        {
          z_tm_pd_position_info.position_mode = PDSM_PD_POSITION_MODE_STANDALONE;
        }
        else if ( p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGNSS_UNDEF )
        {
          z_tm_pd_position_info.position_mode = PDSM_PD_POSITION_MODE_UNKNOWN;
        }

        tm_util_xlate_nav_soln_to_tm_ext_pos(&z_tm_ext_pos_data,
                                             &lr_data_ptr->z_NavPos);

        z_tm_ext_pos_data.h_dop = lr_data_ptr->z_NavPos.f_HDOP;
        z_tm_ext_pos_data.v_dop = lr_data_ptr->z_NavPos.f_VDOP;
        z_tm_ext_pos_data.p_dop = lr_data_ptr->z_NavPos.f_PDOP;

        z_tm_ext_pos_data.pos_hepe = sqrt((lr_data_ptr->z_NavPos.f_ErrorEllipse[1] *
                                           lr_data_ptr->z_NavPos.f_ErrorEllipse[1]) +
                                          (lr_data_ptr->z_NavPos.f_ErrorEllipse[2] *
                                           lr_data_ptr->z_NavPos.f_ErrorEllipse[2]));

        z_utc_time = tm_util_construct_utc_time(lr_data_ptr->z_NavPos.w_GpsWeek,
                                                lr_data_ptr->z_NavPos.q_GpsTimeMs,
                                                lr_data_ptr->z_NavPos.q_UtcTimeMs);
        z_tm_ext_pos_data.utc_time.hour = z_utc_time.q_hours;
        z_tm_ext_pos_data.utc_time.minute = z_utc_time.q_mins;
        z_tm_ext_pos_data.utc_time.sec = z_utc_time.q_secs * 100;
        z_tm_ext_pos_data.utc_time.date = (uint32)((z_utc_time.q_utc_days * 1000000) + (z_utc_time.q_month * 10000) + z_utc_time.q_year);

        /* the pd_info function mark the utc & gps fields as invalid if glonass only fix is computed */
        pd_event = tm_util_pd_info_data_init(p_pd_info_buffer,
                                             lr_resp_ptr,
                                             p_umts_cp_wcdma_session_status->q_pdapi_client_id,
                                             NULL,
                                             &z_tm_pd_position_info,
                                             &z_tm_ext_pos_data);

        if ( (p_umts_cp_wcdma_session_status->u_network_session_on) &&
             (p_umts_cp_wcdma_session_status->mo_lr_type == LOCATION_ESTIMATE)
            )
        {
          p_pd_info_buffer->pd_info.pd_data.ext_pos_data.pos_reported_to_network = TRUE;
        }

        (void)tm_post_pd_event(TM_PRTL_TYPE_UMTS_CP,
                               p_umts_cp_wcdma_session_status->tm_active_session_handle,
                               pd_event,
                               p_pd_info_buffer,
                               &lr_data_ptr->z_NavPos.z_PosFlags);

        /* GPS Complete event */
        sm_log_event_report(SM_LOG_EVENT_ID_GPS_COMPLETE, 0);

        p_umts_cp_wcdma_session_status->u_pdapi_fix_reported = TRUE;

      } /* if the conversion function call returns TRUE */

      (void)pdsm_freebuf((char *)lr_resp_ptr);

    } /* if buffer allocation to tm_lr_resp_type successfull */

  } /* if !p_umts_cp_wcdma_session_status->u_pdapi_fix_reported */



  /*******  check to see if reporting to the network is needed *************/

  if ( p_umts_cp_wcdma_session_status->e_cf_state & C_UMTS_CP_WCDMA_CF_STATE_TIMER_RRC_REPORT )
  {
    /* yet to respond to netowork using RRC */

    MSG_MED("CP-WCDMA: send final position to RRC", 0, 0, 0);

    /* convert position in LR into GAD shape */
    (void)tm_umts_common_utils_construct_ellip_alt_unc_ellip_from_lr(&z_ellipse, lr_data_ptr, TM_CONF_REPORTED_FROM_PE, v_sess_type_ni); 

    /* report to RRC */
    (void)tm_umts_cp_wcdma_encode_rrc_msrReport_pos(&z_ellipse, lr_data_ptr->z_NavPos.q_GpsTimeMs, lr_data_ptr);

    if ( z_rrc_params.z_saved_meas.u_Saved == FALSE )
    {
      /* if not SAVED, proceed; otherwise stay in this state and wait for RESUME */
      if ( p_umts_cp_wcdma_session_status->u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_UI )
      {
        /* wait for MOLR_ReturnResult */
        tm_umts_cp_wcdma_set_cf_state(C_UMTS_CP_WCDMA_CF_STATE_WAIT_RETURN_RESULT, FALSE);
      }
      else
      {
        /* send DONE event to pdapi and end the session */
        tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_COMPLETED);
      }
    }

  } /* if rrc-measControl has been received */

  else if ( p_umts_cp_wcdma_session_status->u_network_session_on )
  {
    /* no rrc reporting request ever received, but already engaged in a network session,
       save the position as pending for network, in case this network session requires position going back
       to the network */
    (void)tm_umts_common_utils_construct_ellip_alt_unc_ellip_from_lr(&z_ellipse, lr_data_ptr, TM_CONF_REPORTED_FROM_PE, v_sess_type_ni); 

    p_umts_cp_wcdma_session_status->q_time_stamp_pos_pending_msec = lr_data_ptr->z_NavPos.q_GpsTimeMs;

    memscpy(&p_umts_cp_wcdma_session_status->z_pending_pos_to_network,
            sizeof(p_umts_cp_wcdma_session_status->z_pending_pos_to_network),
            &z_ellipse, sizeof(z_ellipse));
  }

  else  /* there is no network session */
  {
    /* send DONE event to pdapi */
    tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_COMPLETED);
  }


}


/*===========================================================================

FUNCTION tm_umts_cp_wcdma_handle_rpt_refpos

DESCRIPTION handles TmCore passed pdapi client command of reporting with
  cached reference position.

DEPENDENCIES
  None.

RETURN VALUE
  none.

SIDE EFFECTS
  None.

===========================================================================*/

void tm_umts_cp_wcdma_handle_rpt_refpos( void )
{
  uint8 u_ref_loc_avail = FALSE;

  u_ref_loc_avail = tm_umts_cp_wcdma_report_ref_loc_to_pdapi_if_avail();

  if ( u_ref_loc_avail &&
       (z_umts_cp_wcdma_session_status.e_cf_state &
           C_UMTS_CP_WCDMA_CF_STATE_TIMER_RRC_REPORT)
      )
  {

    tm_umts_cp_wcdma_terminate_timer_cancel_cf_state(C_UMTS_CP_WCDMA_CF_STATE_TIMER_RRC_REPORT);

    /* RefLoc from network always associated to GPS RefTime. */
    if ( !tm_umts_cp_wcdma_encode_rrc_msrReport_pos(&z_last_refloc_info.z_RefPos_GAD.gad_ellip_alt_unc_ellip,
                                                    z_last_refloc_info.z_RefPos_GAD.q_tow_ms, NULL)
        )
    {
      /* if sending of this message fails, stop the session */
      tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_COMPLETED);
    }
    else
    {

      if ( (z_umts_cp_wcdma_session_status.u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_UI) &&
           (z_umts_cp_wcdma_session_status.mo_lr_type == LOCATION_ESTIMATE)
          )
      {
        /* give the server a chance to see if it will provide some kind of position
         via return result. */
        tm_umts_cp_wcdma_set_cf_state(C_UMTS_CP_WCDMA_CF_STATE_WAIT_RETURN_RESULT, FALSE);
      }
      else
      {
        tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_COMPLETED);
      }
    }
  } /* if in a state that MR to network is needed */
}

/*===========================================================================

FUNCTION tm_umts_cp_wcdma_handle_prm_data

DESCRIPTION handles TmCore provided PRM data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void tm_umts_cp_wcdma_handle_prm_data( sm_GpsMeasRptStructType *p_prm_data )
{
  umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status = &z_umts_cp_wcdma_session_status;
  uint32                              q_session_remaining_time_msec;
  boolean                             status_check_needed             = TRUE;

  q_session_remaining_time_msec = os_TimerCheck(umts_cp_wcdma_cf_timer);

  if ( (p_umts_cp_wcdma_session_status->e_gnssPosMode != eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED) &&
       (p_umts_cp_wcdma_session_status->e_gnssPosMode != eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED) )
  {
    return;
  }

  if ( p_prm_data->e_MeasRptReason != GPS_MEAS_RPT_MEAS_DONE )
  {
    /* handle "multiple periodic" */
    if ( z_rrc_params.rrc_periodic_multi_report )
    {
      /*  only subsequent report would use non-DONE meas. */
      if ( p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining !=
              p_umts_cp_wcdma_session_status->w_cp_periodic_reports_total_num )
      {
        tm_umts_cp_wcdma_handle_multi_periodic_rpt(p_umts_cp_wcdma_session_status, p_prm_data, status_check_needed, FALSE);
      }
      return;
    }  /* if multiple periodic */

    if ( q_session_remaining_time_msec > 2500 )
    {
      /* Unless approaching end of session, not interested in anything other
         than DONE, ignore it */
      MSG_MED("disgarding non-DONE MeasRpt", 0, 0, 0);
      return;
    }
    else
    {
      MSG_HIGH("CP-W: proc non-DONE MeasRpt or send cached meas: %u", z_umts_cp_wcdma_session_status.z_cached_gnss_meas.u_meas_reports_bmap, 0, 0);

      /* if there is cached meas., it is for sure a DONE meas., use that
         one --- only E911 may have cached meas. */
      if ( (z_umts_cp_wcdma_session_status.z_cached_gnss_meas.u_meas_reports_bmap & (SM_GNSS_MEAS_REPORTS_BMAP_GPS | SM_GNSS_MEAS_REPORTS_BMAP_GLONASS)) &&
           (z_umts_cp_wcdma_session_status.e_cf_state & C_UMTS_CP_WCDMA_CF_STATE_TIMER_RRC_REPORT) )
      {
        /* The cached measurement should be sent to network & session closed */
        tm_umts_cp_wcdma_rrc_send_cached_gnss_meas_and_end_session();
        return;
      }

#ifdef FEATURE_WCDMA_REL7
#ifdef FEATURE_CGPS_DUAL_WCDMA
	  if ( gnss_wwan_iface_rrc_return_wcdma_rrc_version_sub(z_umts_cp_wcdma_session_status.z_rrc_as_id_info.e_cached_rrc_as_id) >= GNSS_RRC_RELEASE_VERSION_7 )
#else
	  if ( gnss_wwan_iface_rrc_return_wcdma_rrc_version() >= GNSS_RRC_RELEASE_VERSION_7 )
#endif
      {
        status_check_needed = FALSE;
      }
#endif
    }
  }  /* if not DONE meas. */
  else
  {
    MSG_MED("CP-W: processing DONE MeasRpt", 0, 0, 0);
  }

  /******************* multiple periodic *******************/
  if ( z_rrc_params.rrc_periodic_multi_report )
  {
    /* if first DONE meas report */
    if ( p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining ==
            p_umts_cp_wcdma_session_status->w_cp_periodic_reports_total_num )
    {
      tm_umts_cp_wcdma_handle_multi_periodic_rpt(p_umts_cp_wcdma_session_status, p_prm_data, status_check_needed, TRUE);
    }
    else
    {
      tm_umts_cp_wcdma_handle_multi_periodic_rpt(p_umts_cp_wcdma_session_status, p_prm_data, status_check_needed, FALSE);
    }

    return;
  } /* end multiple periodic reporting */

  /* In non-multiple periodic session */
  if ( ((tm_umts_common_utils_emergency_effect() != C_EMERGENCY_HANDLING_NULL) && (q_session_remaining_time_msec > 2500)) ||
       !(z_umts_cp_wcdma_session_status.e_cf_state & C_UMTS_CP_WCDMA_CF_STATE_TIMER_RRC_REPORT) )
  {
    /* cache it for later (at responseTime ) report used by E911 */
    if ( GNSS_MEAS_BLK_SRC_GPS == p_prm_data->z_MeasBlk.e_MeasBlkSrc )
    {
      z_umts_cp_wcdma_session_status.z_cached_gnss_meas.u_meas_reports_bmap |= SM_GNSS_MEAS_REPORTS_BMAP_GPS; /* True */
      memscpy(&z_umts_cp_wcdma_session_status.z_cached_gnss_meas.z_meas_reports[SM_GNSS_MEAS_SYSTEM_GPS],
              sizeof(z_umts_cp_wcdma_session_status.z_cached_gnss_meas.z_meas_reports[SM_GNSS_MEAS_SYSTEM_GPS]),
              p_prm_data, sizeof(*p_prm_data));
    }
    else if ( GNSS_MEAS_BLK_SRC_GLO == p_prm_data->z_MeasBlk.e_MeasBlkSrc )
    {
      z_umts_cp_wcdma_session_status.z_cached_gnss_meas.u_meas_reports_bmap |= SM_GNSS_MEAS_REPORTS_BMAP_GLONASS; /* True */
      memscpy(&z_umts_cp_wcdma_session_status.z_cached_gnss_meas.z_meas_reports[SM_GNSS_MEAS_SYSTEM_GLONASS],
              sizeof(z_umts_cp_wcdma_session_status.z_cached_gnss_meas.z_meas_reports[SM_GNSS_MEAS_SYSTEM_GLONASS]),
              p_prm_data, sizeof(*p_prm_data));
    }
    else if ( GNSS_MEAS_BLK_SRC_BDS == p_prm_data->z_MeasBlk.e_MeasBlkSrc )
    {
      /* The max message & sequence field in BDS is required to send meas report correctly. Its also required to count each epoch.
      *  Note the BDS measurement itself isn't processed & supported by this module
      */
      z_umts_cp_wcdma_session_status.z_cached_gnss_meas.u_meas_reports_bmap |= SM_GNSS_MEAS_REPORTS_BMAP_BDS; /* True */
      memscpy(&z_umts_cp_wcdma_session_status.z_cached_gnss_meas.z_meas_reports[SM_GNSS_MEAS_SYSTEM_BDS],
              sizeof(z_umts_cp_wcdma_session_status.z_cached_gnss_meas.z_meas_reports[SM_GNSS_MEAS_SYSTEM_BDS]),
              p_prm_data, sizeof(*p_prm_data));
    }
    else if ( GNSS_MEAS_BLK_SRC_GAL == p_prm_data->z_MeasBlk.e_MeasBlkSrc )
    {
      /* The max message & sequence field in BDS is required to send meas report correctly. Its also required to count each epoch.
      *  Note the GAL measurement itself isn't processed & supported by this module
      */
      z_umts_cp_wcdma_session_status.z_cached_gnss_meas.u_meas_reports_bmap |= SM_GNSS_MEAS_REPORTS_BMAP_GAL; /* True */
      memscpy(&z_umts_cp_wcdma_session_status.z_cached_gnss_meas.z_meas_reports[SM_GNSS_MEAS_SYSTEM_GAL],
              sizeof(z_umts_cp_wcdma_session_status.z_cached_gnss_meas.z_meas_reports[SM_GNSS_MEAS_SYSTEM_GAL]),
              p_prm_data, sizeof(*p_prm_data));
    }
    else if ( GNSS_MEAS_BLK_SRC_QZSS_SBAS == p_prm_data->z_MeasBlk.e_MeasBlkSrc )
    {
      /* The max message & sequence field in BDS is required to send meas report correctly. Its also required to count each epoch.
      *  Note the QZSS/SBAS measurement itself isn't processed & supported by this module
      */
      z_umts_cp_wcdma_session_status.z_cached_gnss_meas.u_meas_reports_bmap |= SM_GNSS_MEAS_REPORTS_BMAP_QZSS; /* True */
      memscpy(&z_umts_cp_wcdma_session_status.z_cached_gnss_meas.z_meas_reports[SM_GNSS_MEAS_SYSTEM_QZSS],
              sizeof(z_umts_cp_wcdma_session_status.z_cached_gnss_meas.z_meas_reports[SM_GNSS_MEAS_SYSTEM_QZSS]),
              p_prm_data, sizeof(*p_prm_data));
    }

    MSG_HIGH("Cached measurements %3u", z_umts_cp_wcdma_session_status.z_cached_gnss_meas.u_meas_reports_bmap, 0, 0);
    return;
  }


  if ( cp_wcdma_send_rrc_msrReport_meas(p_prm_data, status_check_needed) )
  {

    /* clear timer and call flow state. Note if the above function call returns FALSE,
       don't stop this state, just keep processing incoming measurements until timeout
       at which time UEP-Error will be sent */
    tm_umts_cp_wcdma_terminate_timer_cancel_cf_state(C_UMTS_CP_WCDMA_CF_STATE_TIMER_RRC_REPORT);

    /* GPS Complete event */
    sm_log_event_report(SM_LOG_EVENT_ID_GPS_COMPLETE, 0);

    if ( p_umts_cp_wcdma_session_status->u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_UI )
    {

      /* a workaround to time stamp position to be sent from the server in SS MoLrReturnResult:
         use the reference time's week number and the measurement report's msec. */
      if ( p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp.m.refTimeWeekNumValid )
      {
        p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp.t_pos_time_stamp_ms =
            tm_umts_common_utils_full_gps_ms(
                p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp.w_ref_time_week_num,
                p_prm_data->z_MeasBlk.z_MeasClock.z_Time.z_GpsTime.q_GpsMsec);

        p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp.m.posTimeStampValid = TRUE;
      }

      if ( z_rrc_params.z_saved_meas.u_Saved == FALSE )
      {
        tm_umts_cp_wcdma_set_cf_state(C_UMTS_CP_WCDMA_CF_STATE_WAIT_RETURN_RESULT, FALSE); /* start waiting for MoLrReturnResult */
      }

    } /* if UI */
    else
    {
      if ( z_rrc_params.z_saved_meas.u_Saved == FALSE )
      {
        /* tell TmCore to end and send DONE event to pdapi, also clear UMTS-CP-WCDMA and UMTS-CP-SS */
        tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_COMPLETED);
      }
    }
  }

  return;
}

/*===========================================================================

FUNCTION tm_umts_cp_wcdma_handle_nofix

DESCRIPTION Handles TmCore provided no-fix info: only in multiple periodic 
            position reporting scenario, appropiate error code is sent in MRM.

DEPENDENCIES
  None.

RETURN VALUE
  none.

SIDE EFFECTS
  None.

===========================================================================*/
void tm_umts_cp_wcdma_handle_nofix( void )
{

  if ( z_rrc_params.rrc_periodic_multi_report == FALSE )
  {
    return;
  }

  MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "CP-WCDMA: Handle no fix report");

  z_umts_cp_wcdma_session_status.w_lm_report_count++;

  if ( z_umts_cp_wcdma_session_status.z_session_qos.u_gps_session_timeout_value_secs != 0 )
  {
    if ( z_umts_cp_wcdma_session_status.w_lm_report_count >
            z_umts_cp_wcdma_session_status.q_cp_periodic_reporting_interval_sec )
    {
      /* For BestAvailPos (originally NO_FIX. Its non-GNSS fix) send MRM with error */
      if ( (eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED == z_umts_cp_wcdma_session_status.e_gnssPosMode) ||
           (eWCDMA_POSITION_MODE_A_GPS_GLO_UE_BASED == z_umts_cp_wcdma_session_status.e_gnssPosMode) )
      {
        (void)tm_umts_cp_wcdma_encode_rrc_msrReport_err(RRC_NOT_ENOUGH_GANSS_SATELLITES, NULL);
      }
      else
      {
        (void)tm_umts_cp_wcdma_encode_rrc_msrReport_err(RRC_NOT_ENOUGH_GPS_SATELLITES, NULL);
      }


      z_umts_cp_wcdma_session_status.w_cp_periodic_reports_remaining--;

      /* if done: close session, else keep waiting */
      if ( z_umts_cp_wcdma_session_status.w_cp_periodic_reports_remaining == 0 )
      {
        tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_COMPLETED);
      }

      /* reset lm_report_count */
      z_umts_cp_wcdma_session_status.w_lm_report_count = 0;

    } /* if it's time to report */

    else
    {
      MSG_MED("skipping subsqnt no-fix report: total=%u, remain=%u, lm_rpt_cnt=%u",
              z_umts_cp_wcdma_session_status.w_cp_periodic_reports_total_num,
              z_umts_cp_wcdma_session_status.w_cp_periodic_reports_remaining,
              z_umts_cp_wcdma_session_status.w_lm_report_count);

    }
  }
  return;
}



/********************************************************************
 *
 * tm_umts_cp_wcdma_notif_verif_resp_proc
 *
 * Function description:
 *  Processing notify-verify response from pdapi
 *
 * Parameters:
 *   pdapi_ver_resp_lcs_ptr: pointer to the structure of the pdapi
 *     command notif_verification_response
 *
 * Return: NONE.
 *
 *
 *********************************************************************
 */
void tm_umts_cp_wcdma_notif_verif_resp_proc( pdsm_lcs_notification_resp_s_type *p_user_resp )
{
  pdsm_lcs_umts_cp_notify_verify_req_s_type *p_req_data = &p_user_resp->lcs_info.umts_cp_req;
  boolean                                   u_accepted  = FALSE;

  if ( p_user_resp->resp == PDSM_LCS_NOTIFY_VERIFY_ACCEPT )
  {
    u_accepted = TRUE;
    MSG_MED("Notif/Verif ACCEPT", 0, 0, 0);
  }

  else if ( p_user_resp->resp == PDSM_LCS_NOTIFY_VERIFY_DENY )
  {
    if ( (p_req_data->notification_priv_type == PDSM_LCS_NO_NOTIFY_NO_VERIFY) ||
         (p_req_data->notification_priv_type == PDSM_LCS_PRIVACY_OVERRIDE) ||
         (p_req_data->notification_priv_type == PDSM_LCS_USER_NOTIFY_ONLY)
        )
    {
      /* NOTE: regardless what OEM says, always accept the request; in fact, by design
         in these cases, pdpai should always send ACCEPT */
      u_accepted = TRUE;
      MSG_MED("Notif/Verif REJECT overiden as ACCEPT due to priv_type", 0, 0, 0);
    }
    else
    {
      u_accepted = FALSE;
      MSG_MED("Notif/Verif REJECT", 0, 0, 0);
    }

  } /* OEM rejects */

  else /* no application response, or invalid responses */
  {
    if ( (p_req_data->notification_priv_type == PDSM_LCS_NO_NOTIFY_NO_VERIFY) ||
         (p_req_data->notification_priv_type == PDSM_LCS_PRIVACY_OVERRIDE) ||
         (p_req_data->notification_priv_type == PDSM_LCS_USER_NOTIFY_ONLY) ||
         (p_req_data->notification_priv_type == PDSM_LCS_USER_NOTIFY_VERIFY_ALLOW_NO_RESP)
        )
    {
      /* NOTE: for the top three, regardless what OEM says, always accept the request;
         in fact, by design in these cases, pdpai should always send ACCEPT;
         For the last one, no response is to be treated as ACCEPT by standards. */
      u_accepted = TRUE;

      if ( p_user_resp->resp == PDSM_LCS_NOTIFY_VERIFY_NORESP )
      {
        MSG_MED("Notif/Verif NORESP taken as ACCEPT due to priv_type", 0, 0, 0);
      }
      else
      {
        MSG_MED("Notif/Verif invalid resp. taken as ACCEPT due to priv_type", 0, 0, 0);
      }
    }
    else if ( p_req_data->notification_priv_type == PDSM_LCS_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP )
    {
      u_accepted = FALSE;
      if ( p_user_resp->resp == PDSM_LCS_NOTIFY_VERIFY_NORESP )
      {
        MSG_MED("Notif/Verif NORESP taken as REJECT due to priv_type", 0, 0, 0);
      }
      else
      {
        MSG_MED("Notif/Verif invalid resp. taken as REJECT due to priv_type", 0, 0, 0);
      }
    }
    else /* should never get here, only for completeness */
    {
      u_accepted = FALSE;
      MSG_MED("Notif/Verif no-resp with unexpected priv_type taken as REJECT", 0, 0, 0);
    }

  } /* OEM no response; or any other values that are not supposed to get here, treated as no-resp. */


  /* ask SS to send the response */
  (void)tm_umts_cp_request_lcs_loc_notif_return_result(p_user_resp);

  if ( u_accepted )
  {
    cp_wcdma_action_upon_ni_notif_verif_accepted();
  }
  else
  {
    cp_wcdma_action_upon_ni_notif_verif_denied();
  }

}


/********************************************************************
*
* tm_umts_cp_wcdma_callflow_kind_set
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

void tm_umts_cp_wcdma_callflow_kind_set( umts_cp_wcdma_callflow_kind_enum_type u_callflow_kind )
{
  umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status = &z_umts_cp_wcdma_session_status;

  p_umts_cp_wcdma_session_status->u_callflow_kind = u_callflow_kind;

  if ( u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_NI )
  {
    p_umts_cp_wcdma_session_status->q_pdapi_client_id = pdsm_ni_client_id_get();
    p_umts_cp_wcdma_session_status->tm_active_session_handle = TM_SESS_HANDLE_UMTS_CP_NI;
    p_umts_cp_wcdma_session_status->u_network_session_on = TRUE;
  }

}

/********************************************************************
*
* tm_umts_cp_wcdma_ad_req_holdoff_timer_check
*
* Function description:
*   Check the corresponding holdoff timers to decide whether the assisted
*   data should be part of server AD request list. Also start the holdoff
*   timer for the very first AD request and after timer expiry.
*   For NI call flows the holdoff timer should be used.
*
* Parameters:
*   p_wishlist: Pointer to the GPS wishlist to be checked and/or modified.
*   p_GloWishlist: Pointer to the GLONASS wishlist to be checked and/or modified.
*
* Return:
*  None
*
*********************************************************************
*/
void tm_umts_cp_wcdma_ad_req_holdoff_timer_check( uint32 *p_wishlist, uint32 *p_GloWishlist )
{
  int32  l_long_word = 0;

  /* Sanity */
  if ( ( NULL == p_wishlist )
#ifdef FEATURE_GANSS_SUPPORT
       || (NULL == p_GloWishlist)
#endif
      )
  {
    return;
  }

  /* The hold-off timer is applicable only in non-NI session. */
  if ( C_UMTS_CP_WCDMA_SESSION_NI == z_umts_cp_wcdma_session_status.u_callflow_kind )
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "AD request hold-off timer is inactive for NI");
    return; 
  }

  if ( *p_wishlist & C_GPS_ASSIS_ALM )
  {
    l_long_word = os_TimerCheck( cp_wcdma_alm_req_holdoff_timer );
    if ( l_long_word > 0 )
    {
      *p_wishlist &= ~C_GPS_ASSIS_ALM;
      MSG_MED( "GPS alm holdoff remaining = %ld msec", l_long_word, 0, 0 );
    }
  }
  if ( *p_wishlist & C_GPS_ASSIS_NAVMODL )
  {
    l_long_word = os_TimerCheck( cp_wcdma_eph_req_holdoff_timer );
    if ( l_long_word > 0 )
    {
      *p_wishlist &= ~C_GPS_ASSIS_NAVMODL;
      MSG_MED( "GPS eph holdoff remaining = %ld msec", l_long_word, 0, 0 );
    }
  }
  if ( *p_wishlist & C_GPS_ASSIS_IONO )
  {
    if ( os_TimerCheck( cp_wcdma_iono_req_holdoff_timer ) )
    {
      *p_wishlist &= ~C_GPS_ASSIS_IONO;
    }
  }
  if ( *p_wishlist & C_GPS_ASSIS_UTC )
  {
    if ( os_TimerCheck( cp_wcdma_utc_req_holdoff_timer ) )
    {
      *p_wishlist &= ~C_GPS_ASSIS_UTC;
    }
  }

  if ( z_umts_cp_wcdma_session_status.u_force_request_ref_location == FALSE )
  {
    if ( *p_wishlist & C_GPS_ASSIS_REFLOC )
    {
      if ( os_TimerCheck( cp_wcdma_refloc_req_holdoff_timer ) )
      {
        *p_wishlist &= ~C_GPS_ASSIS_REFLOC;
      }
    }
  }


#ifdef FEATURE_GANSS_SUPPORT
  /*
   * check GANSS (GLO) hold-off timers to see if any of them needs to hold off. Hold off period is 5 mins.
   */
  if ( *p_GloWishlist & C_GNSS_ASSIST_NAVMODL )
  {
    l_long_word = os_TimerCheck( cp_wcdma_glo_eph_req_holdoff_timer );
    if ( l_long_word > 0 )
    {
      *p_GloWishlist &= ~C_GNSS_ASSIST_NAVMODL;
      MSG_MED( "GANSS(GLO) eph holdoff remaining = %ld msec", l_long_word, 0, 0 );
    }
  }
  if ( *p_GloWishlist & C_GNSS_ASSIST_ALM )
  {
    l_long_word = os_TimerCheck( cp_wcdma_glo_alm_req_holdoff_timer );
    if ( l_long_word > 0 )
    {
      *p_GloWishlist &= ~C_GNSS_ASSIST_ALM;
      MSG_MED( "GANSS(GLO) alm. holdoff remaining = %ld msec", l_long_word, 0, 0 );
    }
  }
  if ( *p_GloWishlist & C_GNSS_ASSIST_AUXI_INFO )
  {
    l_long_word = os_TimerCheck( cp_wcdma_glo_auxi_info_req_holdoff_timer );
    if ( l_long_word > 0 )
    {
      *p_GloWishlist &= ~C_GNSS_ASSIST_AUXI_INFO;
      MSG_MED( "GANSS(GLO) auxi info holdoff remaining = %ld msec", l_long_word, 0, 0 );
    }
  }
  if ( *p_GloWishlist & C_GNSS_ASSIST_TIME_MODELS )
  {
    l_long_word = os_TimerCheck( cp_wcdma_glo_time_model_req_holdoff_timer );
    if ( l_long_word > 0 )
    {
      *p_GloWishlist &= ~C_GNSS_ASSIST_TIME_MODELS;
      MSG_MED( "GANSS(GLO) time model holdoff remaining = %ld msec", l_long_word, 0, 0 );
    }
  }
#endif

  MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_LOW, "MO LR type: %d", z_umts_cp_wcdma_session_status.mo_lr_type );

  /******* activate assistance data request holdoff timers *********/

  if ( z_umts_cp_wcdma_session_status.additional_assist_request_done ||
       (z_umts_cp_wcdma_session_status.mo_lr_type == ASSISTANCE_DATA)
      )
  {
    /* Request for additional data enables UE to ask for specific assistance;
       or MOLR-Invoke of assistanceData also enables UE to ask for specifics.
       Only when UE has asked for that specific assistance, should the timer
       for that data start */

    if ( *p_wishlist & C_GPS_ASSIS_ALM )
    {
      (void)os_TimerStart( cp_wcdma_alm_req_holdoff_timer, C_DEFER_ALM_REQ_MSEC, NULL );
    }
    if ( *p_wishlist & C_GPS_ASSIS_NAVMODL )
    {
      (void)os_TimerStart( cp_wcdma_eph_req_holdoff_timer, C_DEFER_EPH_REQ_MSEC, NULL );
    }
    if ( *p_wishlist & C_GPS_ASSIS_IONO )
    {
      (void)os_TimerStart( cp_wcdma_iono_req_holdoff_timer, C_DEFER_IONO_REQ_MSEC, NULL );
    }
    if ( *p_wishlist & C_GPS_ASSIS_UTC )
    {
      (void)os_TimerStart( cp_wcdma_utc_req_holdoff_timer, C_DEFER_UTC_REQ_MSEC, NULL );
    }

    if ( z_umts_cp_wcdma_session_status.u_force_request_ref_location == FALSE )
    {
      if ( *p_wishlist & C_GPS_ASSIS_REFLOC )
      {
        (void)os_TimerStart( cp_wcdma_refloc_req_holdoff_timer, C_DEFER_REFLOC_REQ_MSEC, NULL );
      }
    }


#ifdef FEATURE_GANSS_SUPPORT
    if ( *p_GloWishlist & C_GNSS_ASSIST_NAVMODL )
    {
      (void)os_TimerStart( cp_wcdma_glo_eph_req_holdoff_timer, C_DEFER_EPH_REQ_MSEC, NULL );
    }
    if ( *p_GloWishlist & C_GNSS_ASSIST_ALM )
    {
      (void)os_TimerStart( cp_wcdma_glo_alm_req_holdoff_timer, C_DEFER_ALM_REQ_MSEC, NULL );
    }
    if ( *p_GloWishlist & C_GNSS_ASSIST_AUXI_INFO )
    {
      (void)os_TimerStart( cp_wcdma_glo_auxi_info_req_holdoff_timer, C_DEFER_GLO_AUXI_INFO_REQ_MSEC, NULL );
    }
    if ( *p_GloWishlist & C_GNSS_ASSIST_TIME_MODELS )
    {
      (void)os_TimerStart( cp_wcdma_glo_time_model_req_holdoff_timer, C_DEFER_GLO_TIME_MODEL_REQ_MSEC, NULL );
    }
#endif

  }
}

/********************************************************************
*
* tm_umts_cp_wcdma_msb_assist_req_adj
*
* Function description:
*   Remove from the wishlist items already delivered by the network
*   in the current MSB session, and if after the adjustment there will
*   still something to request, always make sure ref. time is also
*   requested.
*   For A-GPS MSB only call flow ignore Glonass AD wishlist from MGP.
*
* Parameters:
*   Pointer to the wishlist to be checked and/or modified.
*
* Return:
*  None
*
*********************************************************************
*/

void tm_umts_cp_wcdma_msb_assist_req_adj( uint32 *p_wishlist, uint32 *p_GloWishlist, const wcdma_position_mode_e_type e_GnssPosMode )
{
#ifdef FEATURE_GANSS_SUPPORT
  uint32 q_glo_ad_delivered_ex_auxi_info_list = 0;
#endif

  /* Wishlist clean-up : remove anything that have been received in this session from the wishlist */
  *p_wishlist &= ~z_umts_cp_wcdma_session_status.q_assist_delivered_list;
#ifdef FEATURE_GANSS_SUPPORT
  if ( e_GnssPosMode != eWCDMA_POSITION_MODE_A_GPS_GLO_UE_BASED )
  {
    /* Ignore Glonass AD wishlist from MGP if the call type isn't A-GNSS (GPS+GLO) MSB */
    *p_GloWishlist = 0;
  }
  else
  {
    /*
    * Exclude Glo AuxiInfo from delivered list, since request for updated Glonass AuxiInfo solely depends on
    * Glonass NavModel request from MGP.
    */
    q_glo_ad_delivered_ex_auxi_info_list = (z_umts_cp_wcdma_session_status.q_glo_assists_delivered_list & ~C_GNSS_ASSIST_AUXI_INFO);
    if ( *p_GloWishlist & C_GNSS_ASSIST_NAVMODL )
    {
      *p_GloWishlist |= C_GNSS_ASSIST_AUXI_INFO;
    }

    *p_GloWishlist &= ~q_glo_ad_delivered_ex_auxi_info_list;
    MSG_4(MSG_SSID_GPSSM, MSG_LEGACY_MED, "AD delivered list 0x%lX (GPS), 0x%lX, 0x%lX (GLO). Ad req. %3d",
          z_umts_cp_wcdma_session_status.q_assist_delivered_list,
          z_umts_cp_wcdma_session_status.q_glo_assists_delivered_list, q_glo_ad_delivered_ex_auxi_info_list,
          z_umts_cp_wcdma_session_status.additional_assist_request_done);
  }
#endif

  /* if in emergency call, apply emergency condition minimum request */
  if ( tm_umts_common_utils_emergency_effect() != C_EMERGENCY_HANDLING_NULL )
  {
    /* during emergency, only refTime, refLoc, and NavModel will be requested if not available.
       If GANSS is supported GpsUtcModel, GanssNavModel, GanssAuxiInfo & GanssTimeModel will be requested. */
#ifdef FEATURE_GANSS_SUPPORT
    *p_wishlist &= C_GPS_ASSIS_NAVMODL | C_GPS_ASSIS_REFLOC | C_GPS_ASSIS_REFTIME | C_GPS_ASSIS_UTC;
    *p_GloWishlist &= C_GNSS_ASSIST_NAVMODL | C_GNSS_ASSIST_TIME_MODELS | C_GNSS_ASSIST_AUXI_INFO;
#else
    *p_wishlist &= C_GPS_ASSIS_REFTIME | C_GPS_ASSIS_REFLOC | C_GPS_ASSIS_NAVMODL;
#endif
  }
  else
  {
    /* for non-emergency (VAS), the mimimum set idea applies only to requesting addtional
     when server has not pushed such piece of assistance data */
    if ( z_umts_cp_wcdma_session_status.additional_assist_request_done )
    {
#ifdef FEATURE_GANSS_SUPPORT
      *p_wishlist &= C_GPS_ASSIS_REFTIME | C_GPS_ASSIS_REFLOC | C_GPS_ASSIS_NAVMODL | C_GPS_ASSIS_ALM | C_GPS_ASSIS_UTC;
      *p_GloWishlist &= C_GNSS_ASSIST_NAVMODL | C_GNSS_ASSIST_TIME_MODELS | C_GNSS_ASSIST_ALM | C_GNSS_ASSIST_AUXI_INFO;
#else
      *p_wishlist &= C_GPS_ASSIS_REFTIME | C_GPS_ASSIS_REFLOC | C_GPS_ASSIS_NAVMODL | C_GPS_ASSIS_ALM;
#endif
    }
  }

  /*  TM-Core forces RTI to be requested any time another quantity is
   *  requested.  If RTI is the only thing being requested, then set
   *  the WishListMask to 0.  We shouldn't ask for ONLY RTI.
   */
  if ( *p_wishlist == C_GPS_ASSIS_RTI )
  {
    *p_wishlist = 0;
  }


  /* 
  * Check the GNSS hold-off timers to see if any of them needs to hold off. 
  * The hold-off timer is applicable only in non-NI session.
  */
  tm_umts_cp_wcdma_ad_req_holdoff_timer_check( p_wishlist, p_GloWishlist ); 
}



/********************************************************************
 *
 * tm_umts_cp_wcdma_encode_rrc_msrReport_pos
 *
 * Function description:
 *  Form and send RRC MeasurementReport with location estimate.
 *
 * Parameters:
 *
 *  gad_ellip_alt_unc_ellip_type *p_ellipse: pointer to an array
 *     containing the GAD Shaped ( 9 ) position data.
 *
 *  q_source_toa_pos_ms: TOW in msec of the position to be reported.
 *
 * Return:
 *   TRUE: message sent
 *   FLASE: message not sent.
 *
 *********************************************************************
 */

boolean tm_umts_cp_wcdma_encode_rrc_msrReport_pos
(
  gad_ellip_alt_unc_ellip_type *p_ellipse,
  uint32 q_toa_pos_ms,
  sm_GpsFixRptStructType const *const lr_data_ptr
 )
{
  rrc_params_type                     *p_current_rrc_session_param    = &z_rrc_params;
  rrc_meas_rpt_struct_type            *p_measrpt                      = &z_measrpt;
  rrc_gps_reference_loc_struct_type   *p_gps_ref_location             = &p_measrpt->positioning_estimate.gps_ref_location;
  umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status = &z_umts_cp_wcdma_session_status;
  boolean                             ret_val                         = FALSE;

  memset((void *)p_measrpt, 0, sizeof(rrc_meas_rpt_struct_type));

  /*** components common to all incarnations of msrReport *********/

  /* reply with the same ID of the current measControl */
  p_measrpt->meas_id = p_current_rrc_session_param->u_meas_id;

  /* use the same report mode and transfer mode as indicated in measurementControl */
  p_measrpt->report_mode = p_current_rrc_session_param->u_report_mode;
  p_measrpt->tranfer_mode = p_current_rrc_session_param->u_transfer_mode;

  if ( p_current_rrc_session_param->rrc_periodic_multi_report &&
       (p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining > 1)
      )
  {
    /* periodic but not last one */
    p_measrpt->num_reports_ind = FALSE;
  }
  else
  {
    /* one-report only or the last one */
    p_measrpt->num_reports_ind = TRUE;
  }

  p_measrpt->positioning_estimate_incl = TRUE;

  /* GPS time & GPS position estimate will be over-written by Glonass metrics in the GANSS call flow & PE generates
  Glonass position. */
  p_measrpt->positioning_estimate.positioning_estimate = GPS_REFERENCE_TIME_ONLY;
  p_measrpt->positioning_estimate.positionData[0] = RRC_GANSS_POSITION_DATA_GPS_BMASK;   /* GPS only */
  /* report the time stamp of the GPS position record */
  p_measrpt->positioning_estimate.u.gps_reference_time = q_toa_pos_ms;

  /* position estimate choice */
  p_measrpt->positioning_estimate.pos_estimate_choice = ELLIPSOID_POINT_ALTITUDE_ELLIPSE;

  /********************** Latitude  ***************************************************/
  if ( p_ellipse->u_Lat0 & 0x80 )
  {
    p_gps_ref_location->latitude_sign = SOUTH;
  }
  else
  {
    p_gps_ref_location->latitude_sign = NORTH;
  }
  p_gps_ref_location->latitude = (uint32)p_ellipse->u_Lat0 << 16;
  p_gps_ref_location->latitude |= (uint32)p_ellipse->u_Lat1 << 8;
  p_gps_ref_location->latitude |= (uint32)p_ellipse->u_Lat2;
  p_gps_ref_location->latitude &= 0x7FFFFF; /* remove the possible sign bit */

  /********************** Longitude  ***************************************************/
  p_gps_ref_location->longitude = (uint32)p_ellipse->u_Long0 << 16;
  p_gps_ref_location->longitude |= (uint32)p_ellipse->u_Long1 << 8;
  p_gps_ref_location->longitude |= (uint32)p_ellipse->u_Long2;
  if ( p_gps_ref_location->longitude & 0x00800000 ) /* sign ext. sign bit */
  {
    p_gps_ref_location->longitude |= 0xFF000000;
  }

  /*********    Horizontal Uncertainty  *********************************************/
  p_gps_ref_location->uncertainity_semi_major = p_ellipse->u_UncMajor;
  p_gps_ref_location->uncertainity_semi_minor = p_ellipse->u_UncMinor;
  p_gps_ref_location->orientation_major_axis = p_ellipse->u_OrientMajor;

  /********************** Altitude  ************************************************/
  if ( p_ellipse->u_Alt0 & 0x80 )
  {
    p_gps_ref_location->altitude_direction = DEPTH; /* under sea level */
  }
  else
  {
    p_gps_ref_location->altitude_direction = HEIGHT; /* above sea level */
  }
  p_gps_ref_location->altitude = (uint32)p_ellipse->u_Alt0 << 8;
  p_gps_ref_location->altitude |= (uint32)p_ellipse->u_Alt1;
  p_gps_ref_location->altitude &= 0x7FFF; /* remove the possible sign bit */


  /************ altitude uncertainty ***********/
  p_gps_ref_location->uncertainity_altitude = (uint32)p_ellipse->u_UncAlt;

  /* confidence */
  p_gps_ref_location->confidence = p_ellipse->u_Confidence;


#ifdef FEATURE_GANSS_SUPPORT
  /************ GANSS (Glonass) ***********/
  if ( (lr_data_ptr != NULL) &&
       (eWCDMA_POSITION_MODE_A_GPS_GLO_UE_BASED == p_umts_cp_wcdma_session_status->e_gnssPosMode) )
  {
    /* MSB Bit 0 – OTDOA, Bit 1 – GPS,  Bit 6 – GLONASS */
    if ( lr_data_ptr->z_NavPos.z_PosFlags.b_IsGpsUsed && lr_data_ptr->z_NavPos.z_PosFlags.b_IsGloUsed )
    {
      /* GPS + GLONASS */
      p_measrpt->positioning_estimate.positionData[0] = RRC_GANSS_POSITION_DATA_GPS_BMASK | RRC_GANSS_POSITION_DATA_GLONASS_BMASK;
      /* GPS Time & GPS Position Estimate is filled above in GPS section */
    }
    else if ( lr_data_ptr->z_NavPos.z_PosFlags.b_IsGpsUsed )
    {
      /* GPS only */
      p_measrpt->positioning_estimate.positionData[0] = RRC_GANSS_POSITION_DATA_GPS_BMASK;
      /* GPS Time & GPS Position Estimate is filled above in GPS section */
    }
    else if ( lr_data_ptr->z_NavPos.z_PosFlags.b_IsGloUsed )
    {
      /* GLONASS only */
      /* Overwrite the position estimate */
      p_measrpt->positioning_estimate.positioning_estimate = GANSS_REFERENCE_TIME_ONLY;
      /* (0..3599999) GANSS Time of Day (modulo 1 hr) in milliseconds (rounded down to the nearest millisecond unit) */
      p_measrpt->positioning_estimate.u.ganss_ReferenceTimeOnly.gANSS_tod = tm_umts_common_glonass_tod_mod_hour(lr_data_ptr->z_NavPos.u_GloNumFourYear,
                                                                                                                lr_data_ptr->z_NavPos.w_GloNumDaysInFourYear,
                                                                                                                lr_data_ptr->z_NavPos.q_GloTimeMs);
      p_measrpt->positioning_estimate.u.ganss_ReferenceTimeOnly.gANSS_timeId_incl = TRUE;
      p_measrpt->positioning_estimate.u.ganss_ReferenceTimeOnly.gANSS_timeId = RRC_GLONASS_SYSTEM_TIME;
      p_measrpt->positioning_estimate.positionData[0] = RRC_GANSS_POSITION_DATA_GLONASS_BMASK;
      /*
       * gANSS_tod_uncertainty isn't available. Receiver clock uncertainty is less than 1 ms & can't be used for
       * Glonass System Time. The sub millisecond part is not available to use in 'Glonass System Time'.
       */
    }

    /* Velocity estimate (v770ext IE) */
    if ( lr_data_ptr->z_NavPos.z_PosFlags.b_IsVelValid )
    {
      p_measrpt->positioning_estimate.velocityEstimate_incl = TRUE;
      p_measrpt->positioning_estimate.velocityEstimate.velocity_estimate_choice =
                                                                                  HORIZONTAL_WITH_VERTICAL_VELOCITY_AND_UNCERTAINTY;

      tm_rrc_convert_velocity_estimate(lr_data_ptr->z_NavPos.f_VelEnu, lr_data_ptr->z_NavPos.f_Vunc3dMps,
                                       lr_data_ptr->z_NavPos.f_VuncVertMps,
                                       &p_measrpt->positioning_estimate.velocityEstimate.velocity_type);
    }
  }
  MSG_4(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Glo PE : %p, AGNSS Mode %d, PosData %3u, PE %3u",
        lr_data_ptr, p_umts_cp_wcdma_session_status->e_gnssPosMode, p_measrpt->positioning_estimate.positionData[0],
        p_measrpt->positioning_estimate.positioning_estimate);
#endif

  /* If this measurement is currently in a "saved" state, store this message for later
     transmission when we resume. Otherwise, send it immediately */

  if ( p_current_rrc_session_param->z_saved_meas.u_Saved == FALSE )
  {
    /* Send measurement report to RRC */
    if ( !cp_wcdma_rrc_send_measrpt(p_measrpt) )
    {
      MSG_ERROR("WCDMA-CP: Failure sending MeasReport to RRC", 0, 0, 0);
    }
    else
    {
      ret_val = TRUE;
    }
  }
  else
  {
    /* First free any existing message */
    if ( p_current_rrc_session_param->z_saved_meas.p_SavedMessBuff != NULL )
    {
      os_MemFree((void **)&(p_current_rrc_session_param->z_saved_meas.p_SavedMessBuff));
      p_current_rrc_session_param->z_saved_meas.p_SavedMessBuff = NULL;
    }

    /* Allocate space for the new message */
    p_current_rrc_session_param->z_saved_meas.p_SavedMessBuff =
                                                                (rrc_meas_rpt_struct_type *)os_MemAlloc(sizeof(rrc_meas_rpt_struct_type), OS_MEM_SCOPE_TASK);

    if ( p_current_rrc_session_param->z_saved_meas.p_SavedMessBuff != NULL )
    {
      /* Store the message */
      *p_current_rrc_session_param->z_saved_meas.p_SavedMessBuff = *p_measrpt;
    }
    else
    {
      MSG_ERROR("Malloc failed for storing measurement report", 0, 0, 0);
    }
  }

  return ( ret_val );

}


/********************************************************************
 *
 * tm_umts_cp_wcdma_encode_rrc_msrReport_err
 *
 * Function description:
 *  Form and send RRC MeasurementReport with UEP Error
 *
 * Parameters:
 *
 *  e_err_reason: any reason code defined in enum
 *                rrc_positioning_error_cause
 *
 *  p_assist_data_status: pointer to the structure containing info.
 *    about assistance data wishlist; Only used if e_err_reason is
 *    RRC_ASSISTANCE_DATA_MISSING, NULL otherwise.
 *
 * Return:
 *   TRUE: message sent
 *   FLASE: message not sent.
 *
 *********************************************************************
 */
boolean tm_umts_cp_wcdma_encode_rrc_msrReport_err( rrc_positioning_error_cause e_err_reason,
                                                   sm_InternalAssistDataStatusStructType const *const p_assist_data_status )
{
  rrc_params_type                                  *p_current_rrc_session_param    = &z_rrc_params;
  rrc_meas_rpt_struct_type                         *p_measrpt                      = &z_measrpt;
  rrc_additional_assistance_data_struct_type       *p_addtl_assist                 = &p_measrpt->pos_error.addtl_assistance_data;
  uint8                                            u_i                             = 0;
  uint32                                           q_wishlist_mask                 = 0;
  umts_cp_wcdma_session_status_s_type              *p_umts_cp_wcdma_session_status = &z_umts_cp_wcdma_session_status;
  boolean                                          ret_val                         = FALSE;
#ifdef FEATURE_GANSS_SUPPORT
  uint8                                            u_nsat                          = 0;
  rrc_ganss_additional_assistance_data_struct_type *p_ganss_addtl_ad               = &p_measrpt->pos_error.ganss_addtl_assistance_data;
#endif /*RRC R8 GANSS*/

  memset((void *)p_measrpt, 0, sizeof(rrc_meas_rpt_struct_type));


  /*** components common to all incarnations of msrReport *********/

  /* reply with the same ID of the current measControl */
  p_measrpt->meas_id = p_current_rrc_session_param->u_meas_id;

  /* use the same report mode and transfer mode as indicated in measurementControl */
  p_measrpt->report_mode = p_current_rrc_session_param->u_report_mode;
  p_measrpt->tranfer_mode = p_current_rrc_session_param->u_transfer_mode;


  p_measrpt->positioning_error_incl = TRUE;
  p_measrpt->pos_error.error_cause = e_err_reason;

  if ( p_measrpt->pos_error.error_cause == RRC_ASSISTANCE_DATA_MISSING )
  {
    p_measrpt->num_reports_ind = FALSE;  /* req. more data, not considered last in periodic */
  }
  else
  {
    if ( p_current_rrc_session_param->rrc_periodic_multi_report &&
         (p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining > 1)
        )
    {
      /* periodic but not last one */
      p_measrpt->num_reports_ind = FALSE;
    }
    else
    {
      /* one-report only or the last one */
      p_measrpt->num_reports_ind = TRUE;
    }
  }

  if ( p_measrpt->pos_error.error_cause == RRC_ASSISTANCE_DATA_MISSING )
  {
    /* in this case, p_assist_data_status cannot be NULL */
    if ( p_assist_data_status != NULL )
    {
      q_wishlist_mask = p_assist_data_status->q_WishListMask;

      /* GPS AD wishlist isn't empty ? */
      if ( q_wishlist_mask != 0 )
      {
        /* Determine what additional GPS assistance is needed */

        p_measrpt->pos_error.addtl_assistance_data_incl = TRUE;

        if ( q_wishlist_mask & C_GPS_ASSIS_ALM )
        {
          p_addtl_assist->almanac_request_incl = TRUE;
        }

        if ( q_wishlist_mask & C_GPS_ASSIS_UTC )
        {
          p_addtl_assist->utc_model_request_incl = TRUE;
        }

        if ( q_wishlist_mask & C_GPS_ASSIS_IONO )
        {
          p_addtl_assist->ionospheric_model_request_incl = TRUE;
        }

        if ( q_wishlist_mask & C_GPS_ASSIS_NAVMODL )
        {
          p_addtl_assist->navigation_model_request_incl = TRUE;
          p_addtl_assist->nac_model_data_req_incl = TRUE;

          /* if no SV is to be inlcuded in the targeted eph. list, these
             will be the values, no_of_satellite to be determined later */
          p_addtl_assist->navModelAddDataRequest.gps_week = 0;
          p_addtl_assist->navModelAddDataRequest.gps_tow = 0;
          p_addtl_assist->navModelAddDataRequest.age_tolerance = 0;
          p_addtl_assist->navModelAddDataRequest.no_of_satellites = 0;

          if ( p_assist_data_status->u_EphInfoIncl &&
               p_assist_data_status->u_NumSvs )
          {
            p_addtl_assist->navModelAddDataRequest.gps_week = p_assist_data_status->w_GpsWeek % 1024;

            /* toe in hours: w_toe unit is 16 sec . (1hour = 3600 sec, 3600/16 = 225 ) */
            p_addtl_assist->navModelAddDataRequest.gps_tow
               = p_assist_data_status->w_Toe / 225;

            p_addtl_assist->navModelAddDataRequest.age_tolerance
               = p_assist_data_status->w_ToeLimit & 0x0F;

            p_addtl_assist->navModelAddDataRequest.no_of_satellites =
                                                                      (p_assist_data_status->u_NumSvs <= MAX_NO_OF_SATELLITES) ?
                                                                      p_assist_data_status->u_NumSvs : MAX_NO_OF_SATELLITES;

            for ( u_i = 0;
                  u_i < p_addtl_assist->navModelAddDataRequest.no_of_satellites;
                  u_i++ )
            {

              p_addtl_assist->navModelAddDataRequest.sat_data[u_i].sat_id
                 = p_assist_data_status->z_SvEphIode[u_i].w_Sv - 1;

              p_addtl_assist->navModelAddDataRequest.sat_data[u_i].iode
                 = p_assist_data_status->z_SvEphIode[u_i].w_Iode;

#if 0 /* TEST ONLY: NEVER TO BE USED */
              /* test to see if a corrupted IODE would cause server to include this SV in the targeted eph. */
              if (u_i == (p_assist_data_status->u_NumSvs - 1) )
              {
                p_addtl_assist->navModelAddDataRequest.sat_data[u_i].iode += 1;
                MSG_HIGH("UMTS-CP-W: Corrupting Iode for Sv %u", p_assist_data_status->z_SvEphIode[u_i].w_Sv - 1);
              }
#endif

            } /* end SV for-loop */
          } /* end if eph-good-SV list not empty */
        } /* end if-navmodel included */

        if ( q_wishlist_mask & C_GPS_ASSIS_REFLOC )
        {
          p_addtl_assist->reference_location_request_incl = TRUE;
        }
        if ( q_wishlist_mask & C_GPS_ASSIS_REFTIME )
        {
          p_addtl_assist->reference_time_request_incl = TRUE;
        }
        if ( q_wishlist_mask & C_GPS_ASSIS_ACQ )
        {
          p_addtl_assist->aquisition_assistance_request_incl = TRUE;
        }
        if ( q_wishlist_mask & C_GPS_ASSIS_RTI )
        {
          p_addtl_assist->real_time_integrity_request_incl = TRUE;
        }
      }

#ifdef FEATURE_GANSS_SUPPORT
      /*** Glonass assistance data wishlist ***/
      if ( (p_assist_data_status->u_Valid & SM_ASSIST_DATA_VALID_MASK_GLONASS) &&
           ((eWCDMA_POSITION_MODE_A_GPS_GLO_UE_BASED == p_umts_cp_wcdma_session_status->e_gnssPosMode) ||
            (eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED == p_umts_cp_wcdma_session_status->e_gnssPosMode)) )
      {
        q_wishlist_mask = p_assist_data_status->z_GloAssistData.q_WishListMask;

        if ( q_wishlist_mask & C_GNSS_ASSIST_REFLOC )
        {
          p_measrpt->pos_error.ganss_addtl_assistance_data_incl = TRUE;
          p_ganss_addtl_ad->ganssreferenceLocation = TRUE;
        }
        if ( q_wishlist_mask & C_GNSS_ASSIST_REFTIME )
        {
          p_measrpt->pos_error.ganss_addtl_assistance_data_incl = TRUE;
          p_ganss_addtl_ad->ganssReferenceTime = TRUE;
        }
        if ( q_wishlist_mask & C_GNSS_ASSIST_IONO )
        {
          p_measrpt->pos_error.ganss_addtl_assistance_data_incl = TRUE;
          /* UE-Positioning-Error-r7 IE */
          p_ganss_addtl_ad->ganssIonosphericModel = TRUE;
          /* UE-Positioning-Error-v860ext IE */
          p_ganss_addtl_ad->ganssAddIonoModelReq_incl = TRUE;
        }

        if ( q_wishlist_mask & (C_GNSS_ASSIST_ALM | C_GNSS_ASSIST_UTC | C_GNSS_ASSIST_NAVMODL | C_GNSS_ASSIST_RTI |
                                   C_GNSS_ASSIST_AUXI_INFO | C_GNSS_ASSIST_TIME_MODELS | C_GNSS_ASSIST_ACQ) )
        {
          p_measrpt->pos_error.ganss_addtl_assistance_data_incl = TRUE;

          p_ganss_addtl_ad->list_cnt = 1;
          p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssId_incl = TRUE;
          p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssId = RRC_GLONASS;

          if ( q_wishlist_mask & C_GNSS_ASSIST_NAVMODL )
          {
            /* Set ganss_Req_gen_assis_Data[0].ganssNavigationModel to FALSE because
               UE-Positioning-Error-r7...ganssNavigationModel is Galileo-specific. */

            /* Maps to UE-Positioning-Error-v860ext */
            p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssAddNavigationModel_incl = TRUE;

            /* if no Glonass SV is to be inlcuded in the targeted eph. list, these will be the values,
            no_of_satellite to be determined later */
            p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssNavigationModelAdditionalData.ganssWeek = 0;
            p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssNavigationModelAdditionalData.ganssToe = 0;
            p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssNavigationModelAdditionalData.t_toeLimit = 0;
            p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssNavigationModelAdditionalData.satellitesListRelatedDataList_incl = FALSE;
            p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssNavigationModelAdditionalData.sat_cnt = 0;


            if ( p_assist_data_status->z_GloAssistData.u_EphInfoIncl &&
                 (p_assist_data_status->z_GloAssistData.u_NumSvs > 0) )
            {
              /* Maps to UE-Positioning-Error-r7. This indicates tareted glo eph. is included */
              p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssNavigationModelAdditionalData_incl = TRUE;

              /*(0..4095) calendar number of day within the four-year interval starting from 1st of January in a leap year*/
              p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssNavigationModelAdditionalData.ganssWeek = p_assist_data_status->z_GloAssistData.z_AssistDataTime.z_GloEphTime.w_Na;

              /* (0..167), the time of ephemeris in units of 15 minutes of the latest ephemeris set contained by the
                 UE (range 0 to 95 representing time values between 0 and 1425 minutes); in this case,
                 values 96 to 167 shall not be used by the sender.

                 MGP API for w_toe in GLONASS eph. time matches the above definition.
                 */
              p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssNavigationModelAdditionalData.ganssToe = (p_assist_data_status->z_GloAssistData.z_AssistDataTime.z_GloEphTime.w_Tb <= 95) ? p_assist_data_status->z_GloAssistData.z_AssistDataTime.z_GloEphTime.w_Tb : 95;

              /* (0..10), the ephemeris age tolerance of the UE to UTRAN in units of 30 min (range 0 to 10 representing
                 time values of  0 to 300 minutes)

                 MGP : Ephemeris Age tolerance, double sided. 1 = 30 min, 2 = 45 min, 3 = 60 min; 0 = reserved; other = invalid
               */
              if ( p_assist_data_status->z_GloAssistData.z_AssistDataTime.z_GloEphTime.w_ToeLimit == 1 )
              {
                p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssNavigationModelAdditionalData.t_toeLimit = 1;
              }
              else if ( p_assist_data_status->z_GloAssistData.z_AssistDataTime.z_GloEphTime.w_ToeLimit == 2 )
              {
                /* RRC does not have 45 min representation, collapse to 30 min. */
                p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssNavigationModelAdditionalData.t_toeLimit = 1;
              }
              else if ( p_assist_data_status->z_GloAssistData.z_AssistDataTime.z_GloEphTime.w_ToeLimit == 3 )
              {
                p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssNavigationModelAdditionalData.t_toeLimit = 2;
              }
              else
              {
                p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssNavigationModelAdditionalData.t_toeLimit = 0;
              }

              u_nsat = (p_assist_data_status->z_GloAssistData.u_NumSvs <= 15) ? p_assist_data_status->z_GloAssistData.u_NumSvs : 15;

              /* Satellite count */
              p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssNavigationModelAdditionalData.satellitesListRelatedDataList_incl = TRUE;
              p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssNavigationModelAdditionalData.sat_cnt = u_nsat;

              for ( u_i = 0; u_i < u_nsat; ++u_i )
              {
                /* satId - (0..63). Glonass satId 0..23. z_SvEphIode[u_i].w_Sv (65..88) */
                p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssNavigationModelAdditionalData.satellitesListRelatedDataList[u_i].satId =
                                                                                                                                            p_assist_data_status->z_GloAssistData.z_SvEphIode[u_i].w_Sv - (GLO_SLOTID_OFFSET + 1);

                /*iod - (0..1023) 10 LSBs of Issue of Data for SatID*/
                p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssNavigationModelAdditionalData.satellitesListRelatedDataList[u_i].iod =
                                                                                                                                          (uint16)p_assist_data_status->z_GloAssistData.z_SvEphIode[u_i].w_Iode;
              } /* for */
            } /* end - targeted navmodel included */
          } /* Glo NavModel */

          if ( q_wishlist_mask & C_GNSS_ASSIST_ACQ )
          {
            /* Maps to MRM UE-Positioning-Error-r7 IE */
            p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssReferenceMeasurementInfo = TRUE;
          }

          if ( q_wishlist_mask & C_GNSS_ASSIST_UTC )
          {
            /* Set ganss_Req_gen_assis_Data[0].ganssUTCModel to FALSE since UE-Positioning-Error-r7...ganssUTCModel
            is Galileo-specific */

            /* Maps to UE-Positioning-Error-v860ext */
            p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssAddUTCmodel_incl = TRUE;
          }

          if ( q_wishlist_mask & C_GNSS_ASSIST_AUXI_INFO )
          {
            /* Maps to UE-Positioning-Error-v860ext */
            p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssAuxInfo_incl = TRUE;
          }

          if ( q_wishlist_mask & C_GNSS_ASSIST_TIME_MODELS )
          {
            /* Maps to MRM UE-Positioning-Error-r7 IE */
            p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssTimeModelGNSS_GNSS_incl = TRUE;
            /* Bit 0 is set for GPS, Bit 3 is set for GLONASS */
            p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssTimeModelGNSS_GNSS = RRC_GANSS_TIMEMODEL_GPS_BMASK;
          }

          if ( q_wishlist_mask & C_GNSS_ASSIST_ALM )
          {
            /* UE-Positioning-Error-r7 IE */
            p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssAlmanac = TRUE;

            /* UE-Positioning-Error-v860ext almanacModelID is supported & by defauly native model is requested.*/
          }

          if ( q_wishlist_mask & C_GNSS_ASSIST_RTI )
          {
            /* Maps to MRM UE-Positioning-Error-r7 IE */
            p_ganss_addtl_ad->ganss_Req_gen_assis_Data[0].ganssRealTimeIntegrity = TRUE;
          }

        } /* Glo GanssGenericADList */

      } /* Valid GANSS call flow */
#endif /*RRC R8 GANSS*/
    } /* if p_assist_data_status != NULL */
    else
    {
      MSG_HIGH("AssistDataMissing but no data status given", 0, 0, 0);
    }

    /* If there is no missing assistance data to request from network return false */
    if ( (FALSE == p_measrpt->pos_error.addtl_assistance_data_incl)
#ifdef FEATURE_GANSS_SUPPORT
         && (FALSE == p_measrpt->pos_error.ganss_addtl_assistance_data_incl)
#endif
        )
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "No missing AD to request from network");
      return ( FALSE );
    }
  } /* end if-error-reason-is-RRC_ASSISTANCE_DATA_MISSING */

  /* If this measurement is currently in a "saved" state, store this message for later
     transmission when we resume. Otherwise, send it immediately */

  if ( z_rrc_params.z_saved_meas.u_Saved == FALSE )
  {
    /* Send measurement report to RRC */
    if ( !cp_wcdma_rrc_send_measrpt(p_measrpt) )
    {
      MSG_ERROR("WCDMA-CP: Failure sending MeasReport to RRC", 0, 0, 0);
    }
    else
    {
      ret_val = TRUE;
    }
  }
  else
  {
    /* First free any existing message */
    if ( p_current_rrc_session_param->z_saved_meas.p_SavedMessBuff != NULL )
    {
      os_MemFree((void **)&(p_current_rrc_session_param->z_saved_meas.p_SavedMessBuff));
      p_current_rrc_session_param->z_saved_meas.p_SavedMessBuff = NULL;
    }

    /* Allocate space for the new message */
    p_current_rrc_session_param->z_saved_meas.p_SavedMessBuff =
                                                                (rrc_meas_rpt_struct_type *)os_MemAlloc(sizeof(rrc_meas_rpt_struct_type), OS_MEM_SCOPE_TASK);

    if ( p_current_rrc_session_param->z_saved_meas.p_SavedMessBuff != NULL )
    {
      /* Store the message */
      *p_current_rrc_session_param->z_saved_meas.p_SavedMessBuff = *p_measrpt;
    }
    else
    {
      MSG_ERROR("Malloc failed for storing measurement report", 0, 0, 0);
    }
  }

  return ( ret_val );
}

/********************************************************************
*
* tm_umts_cp_wcdma_clear_all_holdoff
*
* Function description:
*   this function clears all holdoff timers.
*
* Parameters:  none.
*
* Return:
*  None
*
*********************************************************************
*/

void tm_umts_cp_wcdma_clear_all_holdoff( void )
{
  /* GPS */
  (void)os_TimerStop(cp_wcdma_eph_req_holdoff_timer);
  (void)os_TimerStop(cp_wcdma_alm_req_holdoff_timer);
  (void)os_TimerStop(cp_wcdma_iono_req_holdoff_timer);
  (void)os_TimerStop(cp_wcdma_utc_req_holdoff_timer);
  (void)os_TimerStop(cp_wcdma_refloc_req_holdoff_timer);

#ifdef FEATURE_GANSS_SUPPORT
  /* GLONASS */
  (void)os_TimerStop(cp_wcdma_glo_eph_req_holdoff_timer);
  (void)os_TimerStop(cp_wcdma_glo_alm_req_holdoff_timer);
  (void)os_TimerStop(cp_wcdma_glo_auxi_info_req_holdoff_timer);
  (void)os_TimerStop(cp_wcdma_glo_time_model_req_holdoff_timer);
#endif
}


/********************************************************************
 *
 * cp_wcdma_encode_rrc_msrReport_common_meas_params
 *
 * Function description:
 *  Initialize the whole LSM measurement report structure & set generic MRM fields
 *  common to GNSS measurement.
 *
 * Parameters:
 *  None
 *
 * Return:
 *   None
 *
 *********************************************************************
 */
static void cp_wcdma_encode_rrc_msrReport_common_meas_params( void )
{
  rrc_meas_rpt_struct_type *p_measrpt                   = &z_measrpt;
  rrc_params_type          *p_current_rrc_session_param = &z_rrc_params;

  /* Initialize the whole LSM measurement report structure */
  memset((void *)p_measrpt, 0, sizeof(rrc_meas_rpt_struct_type)); 

  /*** Components common to all incarnations of msrReport *********/

  /* reply with the same ID, report mode and transfer mode of the current measControl */
  p_measrpt->meas_id = p_current_rrc_session_param->u_meas_id;
  p_measrpt->report_mode = p_current_rrc_session_param->u_report_mode;
  p_measrpt->tranfer_mode = p_current_rrc_session_param->u_transfer_mode;

  if ( p_current_rrc_session_param->rrc_periodic_multi_report &&
       (z_umts_cp_wcdma_session_status.w_cp_periodic_reports_remaining > 1)
      )
  {
    /* periodic but not last one */
    p_measrpt->num_reports_ind = FALSE;
  }
  else
  {
    /* one-report only or the last one */
    p_measrpt->num_reports_ind = TRUE;
  }
}

/********************************************************************
 *
 * cp_wcdma_encode_rrc_msrReport_gps_meas
 *
 * Function description:
 *  Encode GPS measurement report into RRC LSM MRM structure.
 *
 * Dependency :
 *  On cp_wcdma_encode_rrc_msrReport_common_meas_params function
 *
 * Parameters:
 *  None
 *
 * Return:
 *   None
 *
 *********************************************************************
 */
static boolean cp_wcdma_encode_rrc_msrReport_gps_meas( sm_GpsMeasRptStructType *p_prm_data, boolean status_check_needed )
{
  gnss_MeasStructType                     *p_prm_gps_meas_per_sv     = NULL;
  rrc_meas_rpt_struct_type                *p_measrpt                 = &z_measrpt;
  rrc_gps_measured_param_list_struct_type *p_measrpt_gps_meas_per_sv;

  uint8                                   u_i,
                                          u_num_svs;
  float                                   f_Num;
  double                                  d_Num,
                                          d_AdjSubMsec;

  /* if this number is ever greater than 16, limit to the first 16 */
  if ( p_prm_data->z_MeasBlk.u_NumSvs < 16 )
  {
    u_num_svs = p_prm_data->z_MeasBlk.u_NumSvs;
  }
  else
  {
    u_num_svs = C_RRC_MAX_SAT;
  }

  p_prm_gps_meas_per_sv = &p_prm_data->z_MeasBlk.z_Gnss[0]; /* point to first SV in prm */
  p_measrpt_gps_meas_per_sv = &p_measrpt->gps_measured_results.gps_measured_params[0]; /* point to first SV in RRC report */
  for ( u_i = 0; u_i < u_num_svs; u_i++ )
  {
    uint16 w_SvId;

    w_SvId = p_prm_gps_meas_per_sv->w_Sv;

    /*If the status check is needed, check for status bits*/
    if ( (p_prm_gps_meas_per_sv->w_Cno > 0) &&
         (gps_meas_status_bits_check(p_prm_gps_meas_per_sv->q_MeasStatus, status_check_needed))
        )
    {
      /* exclude any SVs with CN0 value of 0 in the report */

      p_measrpt_gps_meas_per_sv->sat_id = w_SvId - 1; /* (0..63) */
      p_measrpt->gps_measured_results.no_of_satellites++;

      p_measrpt_gps_meas_per_sv->c_no = (uint8)((float)(p_prm_gps_meas_per_sv->w_Cno + p_prm_data->z_MeasBlk.z_Gnss[u_i].w_RfLoss) * 0.1);

      /* doppler in Hertz:  f_Num = (FLT)p_zMeasPerSv->l_SvSpeed * (FLT)0.01 * (FLT)C_MsToL1Hz * (FLT)(-1.0) */
      /* adding clock bias to calibration doppler */
      f_Num = (p_prm_gps_meas_per_sv->z_FiltMeas.f_SvSpeed - p_prm_data->z_MeasBlk.z_MeasClock.z_Freq.f_ClkFreqBias)
              * (float)C_MsToL1Hz * (-1.0f);

      p_measrpt_gps_meas_per_sv->doppler = (int32)(f_Num * 5.0f); /* resolution 0.2 Hz, use *5 for /0.2 */


      /*** We tell server that the reference point of the measurement is p_measrpt->positioning_estimate.u.gps_reference_time,
         but the actually GPS moment of the measurement is instead at the FCOUNT of the measurement, whose GPS msec is
         p_zMeasGpsTime->q_Msec - p_zMeasGpsTime->f_ClkBias. We can adjust each SV's submsec value by adding
         p_zMeasGpsTime->f_ClkBias to it. */

      d_AdjSubMsec = (double)p_prm_gps_meas_per_sv->z_FiltMeas.z_SvTime.f_SubMs
                     + (double)p_prm_data->z_MeasBlk.z_MeasClock.z_Time.z_GpsTime.f_ClkTimeBias;

      if ( d_AdjSubMsec > 1.0 )
      {
        /* if the adjustment results in the submsec value above one, at the
           reference moment (TOW) SV time's msec should be one msec more,
           thus we need to subtract 1 ms from the submsec variable */

        d_AdjSubMsec -= 1.0;
      }

      else if ( d_AdjSubMsec < 0 )
      {

        /** or if the adjustment results in submsec value negative, at the
          reference moment (TOW) SV time's msec should be one msec less,
          thus we need to use one minus the resuting "submsec" value for
          the submsec value */

        d_AdjSubMsec = 1.0 + d_AdjSubMsec;

      }

      /* both whole chip and fraction of a chip are based on "code phase", so
         convert submsec to code phase */
      d_AdjSubMsec = 1.0 - d_AdjSubMsec;

      /* whole chips: whole chip representation of the code phase */
      d_Num = d_AdjSubMsec / C_MSEC_PER_CHIP; /* whole chips in msec, floating point */
      p_measrpt_gps_meas_per_sv->whole_gps_chips = (uint16)d_Num; /* whole chips in msec, U10 */
      if ( p_measrpt_gps_meas_per_sv->whole_gps_chips > 1022 )
      {
        /* this should not happen */
        MSG_ERROR("CP-WCDMA: WholeChip out of range", 0, 0, 0);
        return ( FALSE );
      }

      /* fraction of a chip: multiple of 1/1024 chips */
      d_Num = d_AdjSubMsec - (float)p_measrpt_gps_meas_per_sv->whole_gps_chips * (float)C_MSEC_PER_CHIP;
      p_measrpt_gps_meas_per_sv->fractional_gps_chips = (uint16)(d_Num / C_MSEC_FRAC_CHIP);

      /* multipath indicator */
      p_measrpt_gps_meas_per_sv->multipath_ind = 0; /* not used currently */

      /* PseudoRange RMS error: converted from SV time uncertainty */
      f_Num = p_prm_gps_meas_per_sv->z_FiltMeas.f_SvTimeUncMs * LIGHT_MSEC;
      p_measrpt_gps_meas_per_sv->rms_error = tm_umts_common_utils_psudoRMS_formater(f_Num);
      if ( p_measrpt_gps_meas_per_sv->rms_error > 62 )
      {
        p_measrpt_gps_meas_per_sv->rms_error = 62; /* to avoid being considered as xcorelation by some pdm */
      }

      p_measrpt_gps_meas_per_sv++;
    } /* close if-this-SV-good */

    p_prm_gps_meas_per_sv++;

  } /* close of for-per-SV-loop */

  if ( p_measrpt->gps_measured_results.no_of_satellites == 0 )
  {
    return ( FALSE ); /* if not a single SV is qualified, do not report this meas. */
  }

  /* At this point atleast 1 SV meas to report. Set the GPS meas include & time flag. */
  p_measrpt->gps_measured_results_incl = TRUE;
  p_measrpt->gps_measured_results.positioning_estimate = GPS_REFERENCE_TIME_ONLY;
  p_measrpt->gps_measured_results.u.gps_reference_time = p_prm_data->z_MeasBlk.z_MeasClock.z_Time.z_GpsTime.q_GpsMsec;

#ifdef FEATURE_WCDMA_REL7
  p_measrpt->gps_measured_results.gps_reference_time_uncertainity_incl = FALSE;

  if ( !status_check_needed )
  {
    #ifdef FEATURE_CGPS_DUAL_WCDMA  
    if ( gnss_wwan_iface_rrc_return_wcdma_rrc_version_sub(z_umts_cp_wcdma_session_status.z_rrc_as_id_info.e_cached_rrc_as_id) >= GNSS_RRC_RELEASE_VERSION_7 )
	#else
	if ( gnss_wwan_iface_rrc_return_wcdma_rrc_version() >= GNSS_RRC_RELEASE_VERSION_7 )
	#endif
    {
      /*Status check was not done, include time uncertainity*/
      p_measrpt->gps_measured_results.gps_reference_time_uncertainity_incl = TRUE;
      p_measrpt->gps_measured_results.gps_reference_time_uncertainity = tm_umts_common_utils_microsec_to_GADK(p_prm_data->z_MeasBlk.z_MeasClock.z_Time.z_GpsTime.f_ClkTimeUncMs * 1000);
    }
    else
    {
      MSG_HIGH("ERROR: Status check was needed if RRC REL7 is not supported", 0, 0, 0);
    }
  }
#else
  if ( !status_check_needed )
  {
    MSG_HIGH("ERROR: Status check was needed if RRC REL7 is not supported", 0, 0, 0);
  }
#endif

  return ( TRUE );
}

/**********************************************************************************************************************
 * cp_wcdma_send_rrc_msrReport_meas
 *
 * Function description:
 *  This function encodes & decide whether to send the measurement report to WCDMA-RRC or save it internally.
 *  The measurement report could be GPS or GPS + GLO measurement depending on A-GPS or A-GNSS call flow.
 *  The measurement report will be sent only at the end of each epoch.
 *
 * Parameters:
 *  p_prm_data: GPS or GLONASS measurement report
 *  status_check_needed: Measurement status bit check needed.
 *
 * Return:
 *   TRUE: Measurements converted to RRC LSM MRM structure & sent to WCDMA-RRC.
 *   FALSE: In A-GPS scenario RRC LSM MRM with measurement failed to send.
 *          In A-GNSS scenario, the first GPS measurement is encoded successfully
 *          & waiting for Glonass measurements to arrive.
 *          (or)
 *          In A-GNSS scenario, tell the calling function to wait for 'C_UMTS_CP_WCDMA_CF_STATE_TIMER_RRC_REPORT'
 *          timer expiry & send MRM with appropiate error code.
 *
 **********************************************************************************************************************
 */
static boolean cp_wcdma_send_rrc_msrReport_meas( sm_GpsMeasRptStructType *p_prm_data, boolean status_check_needed )
{
  rrc_params_type                     *p_current_rrc_session_param    = &z_rrc_params;
  rrc_meas_rpt_struct_type            *p_measrpt                      = &z_measrpt;
  umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status = &z_umts_cp_wcdma_session_status;
  boolean                             b_RetVal                        = FALSE;

  if ( NULL == p_prm_data )
  {
    return ( FALSE );
  }

  MSG_4(MSG_SSID_GPSSM, MSG_LEGACY_MED, "MeasSrc: %d, Seq#: %3u, MaxMsg#: %3u, CF: %d", p_prm_data->z_MeasBlk.e_MeasBlkSrc,
        p_prm_data->z_MeasBlk.u_SeqNum, p_prm_data->z_MeasBlk.u_MaxMessageNum, p_umts_cp_wcdma_session_status->e_gnssPosMode);

  /* Initialize the LSM measurement report struct & set generic fields _once_ for
  *  each epoch or measurement sets.
  */
  if ( 1 == p_prm_data->z_MeasBlk.u_SeqNum )
  {
    cp_wcdma_encode_rrc_msrReport_common_meas_params(); 
  }

  /* Atleast one measurement is available ? */
  if ( p_prm_data->z_MeasBlk.u_NumSvs > 0 )
  {
    switch ( p_prm_data->z_MeasBlk.e_MeasBlkSrc )
    {
       case GNSS_MEAS_BLK_SRC_GPS:
       {
        /* Encode measurement specific RRC LSM MRM Measurement variables (2/3) */

         /*
         * Propagate each SV's measurement to the common reference point: p_prm_data->z_MeasBlk.z_MeasClock.q_RefFCount.
         * gps_MeasBlkPropagate() would verify for N_ACTIVE_GPS_CHAN GPS SVs.
         */
         gps_MeasBlkPropagate(&p_prm_data->z_MeasBlk);

         /* Return false when GPS measurement encoding fails */
         if ( TRUE == cp_wcdma_encode_rrc_msrReport_gps_meas(p_prm_data, status_check_needed) )
         {
           p_umts_cp_wcdma_session_status->u_GnssMeasEncResultBmap |= SM_GNSS_MEAS_REPORTS_BMAP_GPS;
         }
         break;
       } /* GPS meas */

#ifdef FEATURE_GANSS_SUPPORT
       case GNSS_MEAS_BLK_SRC_GLO:
       {
         /* Process measurements if the call flow type indicate Glonass position method */
         if ( p_umts_cp_wcdma_session_status->e_gnssPosMode & WCDMA_POSITION_METHOD_A_GLO )
         {
           /* Encode measurement specific RRC LSM MRM Measurement variables (2/3) */

           /*
           * Check glonass measurement basic validity - Measurements can be valid non-empty or valid but there are
           * unqualified SVs to report. Measurements generated only after injecting AcqAsst data.
           * If Glonass AcqAsst isn't injected the measurement will be empty.
           */
           if ( (p_prm_data->z_MeasBlk.u_NumSvs > 0) && (p_prm_data->z_MeasBlk.u_NumSvs <= N_ACTIVE_GLO_CHAN) )
           {
             /*
             * Propagate each SV's measurement to the common reference point: p_prm_data->z_MeasBlk.z_MeasClock.q_RefFCount.
             */
             gnss_MeasBlkPropagate(&p_prm_data->z_MeasBlk);
             gnss_GloMeasBlkGGRfgdAdjust(&p_prm_data->z_MeasBlk);

             /* Encode glo meas */
             if ( TRUE == cp_wcdma_encode_rrc_msrReport_glo_meas(p_prm_data, status_check_needed, p_measrpt) )
             {
               p_umts_cp_wcdma_session_status->u_GnssMeasEncResultBmap |= SM_GNSS_MEAS_REPORTS_BMAP_GLONASS;
               /* Copy the GPS meas clock info */
               p_umts_cp_wcdma_session_status->z_GpsTime4GloMeas = p_prm_data->z_MeasBlk.z_MeasClock.z_Time.z_GpsTime;
             }
           }
           else
           {
             MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Num of measured Glonass SVs %d", p_prm_data->z_MeasBlk.u_NumSvs);
           }
         }
         break;
       } /* Glonass meas */
#endif

       default:
         MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Can't process measurement type %d", p_prm_data->z_MeasBlk.e_MeasBlkSrc);
         break;
    }
  }
  else
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "RRC: Empty measurement report");
  }


  /* Wait until all measurements are received from the engine. The engine should generate measurements only for the
  *  injected AD. For non-injected AD an empty measurement report will be generated.
  *  Note that the order of GPS/GLO/BDS measurement isn't warranted.
  */
  if ( p_prm_data->z_MeasBlk.u_SeqNum != p_prm_data->z_MeasBlk.u_MaxMessageNum )
  {
    return ( FALSE ); /* Wait for remaining GPS or GLO or BDS measurement report */
  }
  else
  {
    /* All measurement types in this epoch is received */

#ifdef FEATURE_GANSS_SUPPORT
    /* See if GPS Time is available to used in RRC MRM Glonass meas. report */
    if ( (p_umts_cp_wcdma_session_status->u_GnssMeasEncResultBmap & SM_GNSS_MEAS_REPORTS_BMAP_GPS) &&
         (p_umts_cp_wcdma_session_status->u_GnssMeasEncResultBmap & SM_GNSS_MEAS_REPORTS_BMAP_GLONASS) )
    {
      cp_wcdma_mrm_update_glo_meas_time(&p_umts_cp_wcdma_session_status->z_GpsTime4GloMeas, p_measrpt);
    }
#endif
  }


  /* At this point there can be one or more successfully encoded measurement report available to send
  *  (or)
  *  There are no measurement types (GPS, GLO) to report, possible due to poor measurements. Return false
  *  for this case to let 'CP_WCDMA_CF_TIMER_ID' call flow timer expire & send MRM with appropiate error code.
  */
  if ( p_umts_cp_wcdma_session_status->u_GnssMeasEncResultBmap )
  {
    b_RetVal = TRUE;

    /* If this measurement is currently in a "saved" state, store this message for later
       transmission when we resume. Otherwise, send it immediately */
    if ( p_current_rrc_session_param->z_saved_meas.u_Saved == FALSE )
    {
      MSG_MED("RRC-msrReport: Num of GPS SVs:%u, EncMeasBMap %d", p_measrpt->gps_measured_results.no_of_satellites,
              p_umts_cp_wcdma_session_status->u_GnssMeasEncResultBmap, 0);
      /* Send measurement report to RRC */
      if ( !cp_wcdma_rrc_send_measrpt(p_measrpt) )
      {
        MSG_ERROR("WCDMA-CP: Failure sending MeasReport to RRC", 0, 0, 0);
        b_RetVal = FALSE;
      }
    }
    else
    {
      /* First free any existing message */
      if ( p_current_rrc_session_param->z_saved_meas.p_SavedMessBuff != NULL )
      {
        os_MemFree((void **)&(p_current_rrc_session_param->z_saved_meas.p_SavedMessBuff));
        p_current_rrc_session_param->z_saved_meas.p_SavedMessBuff = NULL;
      }

      /* Allocate space for the new message */
      p_current_rrc_session_param->z_saved_meas.p_SavedMessBuff = (rrc_meas_rpt_struct_type *)
                                                                  os_MemAlloc(sizeof(rrc_meas_rpt_struct_type), OS_MEM_SCOPE_TASK);

      if ( p_current_rrc_session_param->z_saved_meas.p_SavedMessBuff != NULL )
      {
        /* Store the message */
        *p_current_rrc_session_param->z_saved_meas.p_SavedMessBuff = *p_measrpt;
      }
      else
      {
        MSG_ERROR("Malloc failed for storing measurement report", 0, 0, 0);
        b_RetVal = FALSE;
      }
    }

    p_umts_cp_wcdma_session_status->u_GnssMeasEncResultBmap = 0; /* reset */
  }

  return ( b_RetVal );
}


/********************************************************************
 *
 * cp_wcdma_rrc_send_measrpt
 *
 * Function description:
 *  Send a Measurement Report message to RRC for subsequent transmission to the network.
 *
 * Parameters:
 *   p_measrpt - Pointer to OSS
 *
 * Return:
 *   TRUE - If succeeded, FALSE otherwise
 *
 *********************************************************************
 */
static boolean cp_wcdma_rrc_send_measrpt( rrc_meas_rpt_struct_type *p_measrpt )
{
  rrc_cmd_type             *rrc_cmd_ptr  = NULL;
  rrc_meas_rpt_struct_type *meas_rpt_ptr;

  /* Allocate some memory for the command */
  rrc_cmd_ptr = gnss_wwan_iface_rrc_get_lsm_cmd_buf();

  if ( rrc_cmd_ptr == NULL )
  {
    MSG_ERROR("UMTS-CP-W: Failed to get RRC command buffer!", 0, 0, 0);
    return ( FALSE );
  }

  rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_MEASUREMENT_IND;

  /* Below two features defined in custwcdma.h, which customer.h includes */
#ifdef FEATURE_WCDMA_USE_MODEM_HEAP
#ifdef FEATURE_RRC_CALLOC
  meas_rpt_ptr = (rrc_meas_rpt_struct_type *)modem_mem_calloc(1,
                                                              sizeof(rrc_meas_rpt_struct_type),
                                                              MODEM_MEM_CLIENT_WCDMA_RRC_CMD_CRIT);
#else
  meas_rpt_ptr = (rrc_meas_rpt_struct_type *)modem_mem_alloc(sizeof(rrc_meas_rpt_struct_type),
                                                             MODEM_MEM_CLIENT_WCDMA_RRC_CMD_CRIT);
#endif

#else/* end #ifdef FEATURE_WCDMA_USE_MODEM_HEAP */
  meas_rpt_ptr = (rrc_meas_rpt_struct_type *)os_MemAlloc(sizeof(rrc_meas_rpt_struct_type), OS_MEM_SCOPE_TASK);
#endif

  if ( NULL == meas_rpt_ptr )
  {
    MSG_ERROR("Failed to allocate memory of size 0x%lX", sizeof(rrc_meas_rpt_struct_type), 0, 0);
    return ( FALSE );
  }

  /* Copy the passed down RRC Structure to the newly allocated structure */
  *meas_rpt_ptr = *p_measrpt;

  /* Fill in the pointer to this structure in the cmd structure. RRC will
     free this memory when it has received the message */
  rrc_cmd_ptr->cmd.meas_rpt_ind.meas_rpt = meas_rpt_ptr;

  /* Note : Use a new DM log Id if rrc_meas_params_struct_type struct size increase above 65K bytes */
  sm_log_svr_txrx_rep(C_LSM_LOG_SVR_TX_TYPE, 3, C_LSM_SVR_PROTOCOL_TYPE_RRC_GANSS_LSM,
                      (uint16)(sizeof(rrc_meas_rpt_struct_type) & 0xFFFF),
                      (uint8 *)meas_rpt_ptr);

  /* Send the command to RRC */
  #ifdef FEATURE_CGPS_DUAL_WCDMA
  gnss_wwan_iface_rrc_put_lsm_cmd_sub(rrc_cmd_ptr,z_umts_cp_wcdma_session_status.z_rrc_as_id_info.e_cached_rrc_as_id);
  #else
  gnss_wwan_iface_rrc_put_lsm_cmd(rrc_cmd_ptr);
  #endif

  MSG_MED("UMTS-CP-W: Sent response to RRC", 0, 0, 0);

  return ( TRUE );
}


/********************************************************************
 *
 * cp_wcdma_generate_default_save_response
 *
 * Function description:
 *  This function generates the default response to be sent to the network
 *  upon resuming a saved measurement. In cases where TM will still continue the
 *  session, this message may be replaced with another message such as a position fix.
 *
 * Parameters:
 *   Pointer to received Measurement Parameters structure
 *
 * Return:
 *  u_MeasId - Measurement ID
 *  u_TransferMode - Transfer Mode
 *  u_ReportMode - ReportMode;
 *
 *********************************************************************
 */
rrc_meas_rpt_struct_type* cp_wcdma_generate_default_save_response( uint8 u_MeasId, uint8 u_TransferMode,
                                                                   uint8 u_ReportMode )
{
  rrc_meas_rpt_struct_type *p_measrpt;

  p_measrpt = NULL;

  /* Allocate memory for meas report */
  p_measrpt = (rrc_meas_rpt_struct_type *)os_MemAlloc(sizeof(rrc_meas_rpt_struct_type), OS_MEM_SCOPE_TASK);

  if ( p_measrpt == NULL )
  {
    MSG_ERROR("Failed to allocate Meas Rpt Buffer!!", 0, 0, 0);
  }
  else
  {
    /* Send a Measurement Report with Undefined Error */
    memset((void *)p_measrpt, 0, sizeof(rrc_meas_rpt_struct_type));

    p_measrpt->meas_id = u_MeasId;
    p_measrpt->num_reports_ind = TRUE; /* UE only supports periodic reporting
                                          of one report, so this has to be
                                          the last one */

    p_measrpt->report_mode = u_ReportMode;

    p_measrpt->positioning_error_incl = TRUE;
    p_measrpt->pos_error.addtl_assistance_data_incl = FALSE;
    p_measrpt->pos_error.error_cause = RRC_UNDEFINED_ERROR;

    p_measrpt->tranfer_mode = u_TransferMode;
  }

  return ( p_measrpt );
}


/*===========================================================================

FUNCTION tm_umts_cp_wcdma_inform_core_stop_session

DESCRIPTION
  This function informs the TM core that the current session should
  terminate.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/

void tm_umts_cp_wcdma_inform_core_stop_session( tm_sess_stop_reason_e_type stop_reason )
{

  umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status = &z_umts_cp_wcdma_session_status;

  tm_sess_req_param_u_type            req_param;
  tm_sess_req_param_u_type            *p_req_param                    = &req_param;

  p_req_param->stop_param.stop_type = TM_STOP_TYPE_SESSION;
  p_req_param->stop_param.stop_reason = stop_reason;

  MSG_MED("UMTS-CP-W: Stop TmCore session", 0, 0, 0);

  (void)tm_sess_req(TM_PRTL_TYPE_UMTS_CP,
                    p_umts_cp_wcdma_session_status->tm_active_session_handle,
                    TM_SESS_REQ_STOP,
                    p_req_param
                   );

  #ifdef FEATURE_CGPS_DUAL_WCDMA
  tm_umts_common_utils_mm_session_status_notify(FALSE, z_umts_cp_wcdma_session_status.z_rrc_as_id_info.e_cached_rrc_as_id);
  #else
  tm_umts_common_utils_mm_session_status_notify(FALSE);
  #endif

  /* also clear protocol submodule session related variables */
  if ( z_rrc_params.z_saved_meas.p_SavedMessBuff != NULL )
  {
    os_MemFree((void **)&(z_rrc_params.z_saved_meas.p_SavedMessBuff));
    z_rrc_params.z_saved_meas.p_SavedMessBuff = NULL;
  }
  tm_umts_cp_wcdma_session_cleanup();
  tm_umts_cp_submodule_end_session();
}


/*===========================================================================

FUNCTION tm_umts_cp_wcdma_report_ref_loc_to_pdapi_if_avail

DESCRIPTION
  This function reports the reference position provided by the network, if
  available, as the final position to TmCore.

DEPENDENCIES: none.

RETURN VALUE: TRUE - reference position reported to pdapi;
              FALSE - no reference position reported to pdapi.

SIDE EFFECTS: none.

===========================================================================*/

boolean tm_umts_cp_wcdma_report_ref_loc_to_pdapi_if_avail( void )
{
  uint32                     q_time_stamp_sec = 0;
  pdsm_pd_event_type         q_pd_event; /* pdsm_pd_event_type is uint32 */
  gnss_NavSlnFlagsStructType z_pos_flags;

  memset(&z_pos_flags, 0x00, sizeof(z_pos_flags));
  if ( !((z_umts_cp_wcdma_session_status.e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_BASED) &&
         (z_umts_cp_wcdma_session_status.q_assist_delivered_list & C_GPS_ASSIS_REFLOC)) ||
       (z_umts_cp_wcdma_session_status.u_appbased_tracking == TRUE) )
  {
    return ( FALSE );
  }

  if ( z_umts_cp_wcdma_session_status.q_assist_delivered_list & C_GPS_ASSIS_REFTIME )
  {
    q_time_stamp_sec = (uint32)(tm_umts_common_utils_full_gps_ms(z_last_refloc_info.z_RefPos_GAD.w_week,
                                                                 z_last_refloc_info.z_RefPos_GAD.q_tow_ms
                                                                ) / SEC_MSECS );
  }

  (void)tm_umts_common_utils_gad_to_pdsm_pd_info(z_umts_cp_wcdma_session_status.q_pdapi_client_id,
                                                 (uint8 *)&z_last_refloc_info.z_RefPos_GAD.gad_ellip_alt_unc_ellip,
                                                 PDSM_PD_POSITION_TYPE_NEW,
                                                 PDSM_PD_POSITION_MODE_UNKNOWN,
                                                 PDSM_PD_POSITION_SOURCE_DEFAULT,
                                                 q_time_stamp_sec,
                                                 &z_umts_cp_wcdma_pd_info_buffer,
                                                 &q_pd_event);
  if ( q_pd_event & PDSM_PD_EVENT_HEIGHT )
  {
    z_pos_flags.b_Is3D = TRUE;
  }

  (void)tm_post_pd_event(TM_PRTL_TYPE_UMTS_CP,
                         z_umts_cp_wcdma_session_status.tm_active_session_handle,
                         q_pd_event,
                         &z_umts_cp_wcdma_pd_info_buffer,
                         &z_pos_flags);

  z_umts_cp_wcdma_session_status.u_pdapi_fix_reported = TRUE;

  return ( TRUE );
}

/*===========================================================================

FUNCTION tm_umts_cp_wcdma_report_ref_loc_in_ext_status_event

DESCRIPTION
  This function reports the reference position provided by the network in
  the format for Extended Status Position Event.

DEPENDENCIES: none.

RETURN VALUE: TRUE - reference position report success
              FALSE - no reference position report failure

SIDE EFFECTS: none.

===========================================================================*/

boolean tm_umts_cp_wcdma_report_ref_loc_in_ext_status_event( void )
{
  uint32             q_time_stamp_sec = 0;
  pdsm_pd_event_type q_pd_event; /* pdsm_pd_event_type is uint32 */

  if ( !(((z_umts_cp_wcdma_session_status.e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_BASED) ||
          (z_umts_cp_wcdma_session_status.e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_BASED)) &&
         (z_umts_cp_wcdma_session_status.q_assist_delivered_list & C_GPS_ASSIS_REFLOC)
        )
      )
  {
    return ( FALSE );
  }

  if ( z_umts_cp_wcdma_session_status.q_assist_delivered_list & C_GPS_ASSIS_REFTIME )
  {

    q_time_stamp_sec = (uint32)(tm_umts_common_utils_full_gps_ms(z_last_refloc_info.z_RefPos_GAD.w_week,
                                                                 z_last_refloc_info.z_RefPos_GAD.q_tow_ms
                                                                ) / SEC_MSECS
                               );

  }

  (void)tm_umts_common_utils_gad_to_pdsm_pd_info
     (z_umts_cp_wcdma_session_status.q_pdapi_client_id,
      (uint8 *)&z_last_refloc_info.z_RefPos_GAD.gad_ellip_alt_unc_ellip,
      PDSM_PD_POSITION_TYPE_NEW,
      PDSM_PD_POSITION_MODE_UNKNOWN,
      PDSM_PD_POSITION_SOURCE_DEFAULT,
      q_time_stamp_sec,
      &z_umts_cp_wcdma_pd_info_buffer,
      &q_pd_event
     );


  tm_post_ext_status_ref_loc_rep_event(
                                       TM_PRTL_TYPE_UMTS_CP,
                                       z_umts_cp_wcdma_session_status.tm_active_session_handle,
                                       &z_umts_cp_wcdma_pd_info_buffer.pd_info.pd_data);

  return ( TRUE );

} /* tm_umts_cp_wcdma_report_ref_loc_in_ext_status_event */


/*===========================================================================

FUNCTION cp_wcdma_meas_control_proc

DESCRIPTION
  This function processes RRC MeasurementControl messages.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
static void cp_wcdma_meas_control_proc( rrc_meas_ctrl_struct_type *p_rrc_meas_ctrl )
{
  rrc_params_type                     *p_rrc_params                   = &z_rrc_params;
  umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status = &z_umts_cp_wcdma_session_status;
  tm_sess_req_param_u_type            req_param;
  tm_sess_req_param_u_type            *p_req_param                    = &req_param;
  boolean                             b_requestMissingAssistData      = FALSE;
  emergency_handling_e_type           e_e911_effect                   = C_EMERGENCY_HANDLING_NULL;
  uint32                              q_rrc_timer                     = 0;
  uint8                               u_array_size                    = sizeof(w_RrcPeriodIntervalToMaxFixTimeout) / sizeof(uint16);
  gnss_time_info const *p_WcdmaSessGnssTimeAd                         = &z_umts_cp_wcdma_session_status.z_set_assist_pos_time_stamp;

  /* clear req_param first */
  memset(p_req_param, 0, sizeof(tm_sess_req_param_u_type));

  /* clear timer and call flow state */
  tm_umts_cp_wcdma_terminate_timer_cancel_cf_state(C_UMTS_CP_WCDMA_CF_STATE_WAIT_RRC);


  if ( p_rrc_meas_ctrl->meas_start_incl != TRUE )
  {
    /* We are receiving a Measurement Control with a validity that
    does not match the current RRC state. This results in an immediate
    SAVE. Generate a default response and start a new LSM session. If the LSM session
    finished before we resume, the default message will be replaced. */

    if ( p_rrc_params->z_saved_meas.u_Saved == TRUE &&
         p_rrc_meas_ctrl->meas_id != p_rrc_params->z_saved_meas.u_MeasId )

    {
      rrc_meas_rpt_struct_type *p_MeasRpt;

      /* We only support one saved measurement at a time. Reject the new measurement */
      p_MeasRpt = cp_wcdma_generate_default_save_response(p_rrc_meas_ctrl->meas_id,
                                                          p_rrc_meas_ctrl->tranfer_mode,
                                                          p_rrc_meas_ctrl->rpt_crit.rpt_mode);

      if ( p_MeasRpt != NULL )
      {
        (void)cp_wcdma_rrc_send_measrpt(p_MeasRpt);
        os_MemFree((void **)&p_MeasRpt);
      }
      else
      {
        MSG_ERROR("Meas Rpt pointer is NULL", 0, 0, 0);
      }

      return;

    }
    else
    {
      /* Mark the fact that this measurement is currently suspended */
      p_rrc_params->z_saved_meas.u_Saved = TRUE;
      p_rrc_params->z_saved_meas.u_MeasId = p_rrc_meas_ctrl->meas_id;
    }
  }
  else /* validity matches RRC state */
  {
    if ( p_rrc_params->z_saved_meas.u_Saved )
    {
      if ( p_rrc_meas_ctrl->meas_cmd == RRC_MEAS_SETUP )
      {
        /* We currently do not entertain another measurement while there is a previous
           measurement in the SAVE state */

        /* Return UEP Undefined Error */
        rrc_meas_rpt_struct_type *p_MeasRpt;

        p_MeasRpt = cp_wcdma_generate_default_save_response(p_rrc_meas_ctrl->meas_id,
                                                            p_rrc_meas_ctrl->tranfer_mode,
                                                            p_rrc_meas_ctrl->rpt_crit.rpt_mode);

        if ( p_MeasRpt != NULL )
        {
          (void)cp_wcdma_rrc_send_measrpt(p_MeasRpt);
          os_MemFree((void **)&p_MeasRpt);
        }
        else
        {
          MSG_ERROR("Meas Rpt pointer is NULL", 0, 0, 0);
        }

        return;

      }
      else
      {
        if ( p_rrc_params->u_meas_id == p_rrc_params->z_saved_meas.u_MeasId )
        {
          /* This is a modify message which is setting the state back to valid.
             Invalidate the saved state. */
          p_rrc_params->z_saved_meas.u_Saved = FALSE;
        }
      }
    } /* in SAVED state */
  } /* validity matches RRC state */

  p_rrc_params->u_rrc_param_type = RRC_MEAS_CTRL_PARMS;
  p_rrc_params->u_rrc_meas_cmd_type = p_rrc_meas_ctrl->meas_cmd;
  p_rrc_params->u_conf_ind = p_rrc_meas_ctrl->config_conf_ind;
  p_rrc_params->u_ack_num = p_rrc_meas_ctrl->config_ack_num;
  p_rrc_params->u_meas_id = p_rrc_meas_ctrl->meas_id;
  p_rrc_params->u_report_mode = p_rrc_meas_ctrl->rpt_crit.rpt_mode;
  p_rrc_params->u_transfer_mode = p_rrc_meas_ctrl->tranfer_mode;

  /* if  MEASUREMENT_CONTROL is of type RELEASE, cancel the ongoing
   session with the same ID */
  if ( (p_rrc_params->u_rrc_meas_cmd_type == RRC_MEAS_DELETE) ||
       (p_rrc_params->u_rrc_meas_cmd_type == RRC_MEAS_DELETE_ALL)
      )
  {
    rrc_saved_meas_params_s_type *p_SavedParams = &p_rrc_params->z_saved_meas;

    /* If the Measurement is currently saved, invalidate it */
    if ( (p_SavedParams->u_Saved) &&
         (p_SavedParams->u_MeasId == p_rrc_params->u_meas_id) )
    {
      if ( p_SavedParams->p_SavedMessBuff != NULL )
      {
        /* Free up saved message */
        os_MemFree((void **)&(p_SavedParams->p_SavedMessBuff));
        p_SavedParams->p_SavedMessBuff = NULL;
      }

      p_SavedParams->u_Saved = FALSE;
    }

    /* whenever we receive this, cancel any ongoing RRC measurement */
    tm_umts_cp_wcdma_callflow_disrupt_handler();

    return;

  } /* close if-MC-DELETE */

  /*** check if in emergency call , and if so, any special priority issues to handle */
  e_e911_effect = tm_umts_common_utils_emergency_effect();
  if ( e_e911_effect == C_EMERGENCY_HANDLING_REJ_ALL )
  {
    MSG_MED("RRC message rejected due to NV item %d",
            e_e911_effect, 0, 0);
    if ( p_rrc_params->u_report_mode != RRC_NO_RPT )
    {
      (void)tm_umts_cp_wcdma_encode_rrc_msrReport_err(RRC_UNDEFINED_ERROR, NULL);
    }
    return;
  }
  if ( (z_umts_nv_item_ram_copy.gps_lock == PDSM_GPS_LOCK_ALL) ||
       (z_umts_nv_item_ram_copy.gps_lock == PDSM_GPS_LOCK_MT)
      )
  {
    if ( (e_e911_effect != C_EMERGENCY_HANDLING_ACCEPT_ONLY_HIGH_PRIORITY) &&
         (e_e911_effect != C_EMERGENCY_HANDLING_ACCEPT_ALL)
        )
    {
      if ( p_rrc_params->u_report_mode != RRC_NO_RPT )
      {
        (void)tm_umts_cp_wcdma_encode_rrc_msrReport_err(RRC_UNDEFINED_ERROR, NULL);
      }
      return;
    }

  }


  /* Note: this NV item being FALSE is a special feature and this section is not part of a
     regular call flow. Note if the NV_ITEM sets allow_rrc to TRUE, no need to check the
     session based status. */
  if ( p_umts_cp_wcdma_session_status->allow_rrc == FALSE )
  {
    if ( (e_e911_effect == C_EMERGENCY_HANDLING_NULL) ||
         (e_e911_effect == C_EMERGENCY_HANDLING_REJ_ALL)
        )
    {
      /* reject this due to the current status of "allow_rrc" */
      if ( p_rrc_params->u_report_mode != RRC_NO_RPT )
      {
        (void)tm_umts_cp_wcdma_encode_rrc_msrReport_err(RRC_UNDEFINED_ERROR, NULL);
      }
      return;
    }
  }

  /* if RRC-MC-Setup and MTLR-GuardTimerState is ON, turn it off */
  if ( p_rrc_params->u_rrc_meas_cmd_type == RRC_MEAS_SETUP )
  {
    if ( p_umts_cp_wcdma_session_status->e_cf_state == C_UMTS_CP_WCDMA_CF_STATE_MT_GUARD_TIME )
    {
      MSG_MED("Stopping MTLR guard timer", 0, 0, 0);
      tm_umts_cp_wcdma_terminate_timer_cancel_cf_state(C_UMTS_CP_WCDMA_CF_STATE_MT_GUARD_TIME);
    }
  }


  /* Send PDAPI event when first RRC message from network is received. */
  if ( p_umts_cp_wcdma_session_status->u_pdapi_comm_begin_sent == FALSE )
  {
    p_umts_cp_wcdma_session_status->u_pdapi_comm_begin_sent = TRUE;

    if ( e_e911_effect != C_EMERGENCY_HANDLING_NULL )
    {
      tm_core_send_pdapi_emerg_sess_type(PDSM_PD_EXT_STATUS_EMERG_SESS_UMTS_CP);
    }
    /* tell TmCore to send pdapi event of EventGpsPdConnectionStart */
    tm_umts_common_send_pdapi_comm_event
       (PDSM_PD_COMM_PROTOCOL_UMTS_CP_WCDMA,
        C_COMM_BEGIN,
        p_umts_cp_wcdma_session_status->tm_active_session_handle,
        p_umts_cp_wcdma_session_status->q_pdapi_client_id
       );
  }

  /*
  *   WCDMA-RRC layer should handle all invalid/unsupported position methods. However verify before processing further.
  */
  if ( cp_wcdma_mcm_rpt_position_method_invalid(p_rrc_meas_ctrl) )
  {
    MSG_4(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "MCM position method %3u, type %3u, ganss_incl %d, method %u is unsupported",
          p_rrc_meas_ctrl->rpt_quan.method, p_rrc_meas_ctrl->rpt_quan.method_type,
          p_rrc_meas_ctrl->rpt_quan.ganss_pos_method_incl, p_rrc_meas_ctrl->rpt_quan.ganss_pos_method);

    /* Requested position method not supported */
    (void)tm_umts_cp_wcdma_encode_rrc_msrReport_err(RRC_UNDEFINED_ERROR, NULL);

    if ( p_umts_cp_wcdma_session_status->tm_core_session_started )
    {
      tm_umts_cp_wcdma_callflow_disrupt_handler();
    }

    return;
  }

  /********  First RRC-measureControl of an NI-LR or MT-LR or MO-LR (LocationEstimate) session **************/
  if ( p_umts_cp_wcdma_session_status->u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_NONE )
  {

    MSG_MED("UMTS-CP-W: first NILR/MTLR RRC meas.Control arrived", 0, 0, 0);

    /* Can the network requested position method supported by UE NV settings ? */
    if ( FALSE == cp_wcdma_mcm_pos_method_support_by_ue(TM_CORE_CONFIG_DD(positioning_modes_supported), 
                                                        p_rrc_meas_ctrl->rpt_quan.method_type)
        )
    {
      /* Network requested mode not supported */
      (void)tm_umts_cp_wcdma_encode_rrc_msrReport_err(RRC_UNDEFINED_ERROR, NULL);
      return;
    }

    /*Service interaction check should be done only if the TM_CORE is not started.
      If wcdma session status has tm_core_session_started as TRUE, the TM session could
      be only started from WCDMA CP during E911 call origination. So if that is the case,
      no need to check for service interaction.*/
    if ( p_umts_cp_wcdma_session_status->tm_core_session_started == FALSE )
    {
      /* possible service interation arbitration */
      if ( !tm_umts_common_srv_interact(C_UMTS_SESSION_WCDMA_CP_NI) )
      {
        if ( p_rrc_params->u_report_mode != RRC_NO_RPT )
        {
          (void)tm_umts_cp_wcdma_encode_rrc_msrReport_err(RRC_UNDEFINED_ERROR, NULL);
        }
        return;
      }
    }

    /* if allowed to proceed : */
    tm_umts_cp_wcdma_callflow_kind_set(C_UMTS_CP_WCDMA_SESSION_NI);

    /*Cancel ongoing max mgp on timer*/
    tm_umts_cp_wcdma_cancel_max_mgp_on_timer();


#ifdef FEATURE_EXTENDED_RESP_TIME
    /* extended_reporting_interval() is an OEM provided function
       that provides extended reporting time (used for network intiated) and
       additional_request_margin which is the number of seconds before
       RRC's ReportingIntervalLong expires, at which UE sends UEP error
       assistanceDataMissing asking for IONO; */
    tm_umts_cp_wcdma_extended_reporting_interval
       (&p_umts_cp_wcdma_session_status->u_extended_ni_resp_time_sec,
        &p_umts_cp_wcdma_session_status->q_additional_req_margin_sec
       );
#endif

    z_umts_cp_wcdma_pd_info_buffer.client_id = p_umts_cp_wcdma_session_status->q_pdapi_client_id;


    /* Notify MM that RRC messaging has started. This will set a 300 second
       guard timer in NAS that will protect against keeping the signaling channel
       up indefinately. If we did not notify NAS here, it would drop the channel
       10 seconds after NAS sends back the location notification return result;

       If NV Item default_allow_rrc is set to FALSE, the MT, MM notif. has been done
       when Location Notification Ind. was accepted; doesn't hurt to notify again. */
    #ifdef FEATURE_CGPS_DUAL_WCDMA   
    tm_umts_common_utils_mm_session_status_notify(TRUE, z_umts_cp_wcdma_session_status.z_rrc_as_id_info.e_cached_rrc_as_id);
	#else
	tm_umts_common_utils_mm_session_status_notify(TRUE);
	#endif

    /* Validate & set the allowed position mode */
    p_umts_cp_wcdma_session_status->e_gnssPosMode = cp_wcdma_allowed_position_mode(p_rrc_meas_ctrl);

    if ( eWCDMA_POSITION_MODE_AGNSS_UNDEF == p_umts_cp_wcdma_session_status->e_gnssPosMode )
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Invalid position mode");
      return;
    }

    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Choosen A-GNSS position method %d",
          p_umts_cp_wcdma_session_status->e_gnssPosMode);

    /*
    * Start the session with receiver in idle mode
    */
    /* since u_callflow_kind has not yet been set, this is MT */
    if ( (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED) ||
         (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED) )
    {
      tm_sess_req_type req_type = cp_wcdma_build_tm_session_msa_ptr(p_req_param, p_umts_cp_wcdma_session_status);

      if ( p_umts_cp_wcdma_session_status->tm_active_session_handle == TM_SESS_HANDLE_E911 )
      {
        p_umts_cp_wcdma_session_status->tm_active_session_handle = TM_SESS_HANDLE_UMTS_CP_NI;
      }
      if ( !tm_sess_req(TM_PRTL_TYPE_UMTS_CP,
                        p_umts_cp_wcdma_session_status->tm_active_session_handle,
                        req_type,
                        p_req_param
                       )
          )
      {
        if ( p_rrc_params->u_report_mode != RRC_NO_RPT )
        {
          (void)tm_umts_cp_wcdma_encode_rrc_msrReport_err(RRC_UNDEFINED_ERROR, NULL);
        }
        /* clear session related variables */
        if ( z_rrc_params.z_saved_meas.p_SavedMessBuff != NULL )
        {
          os_MemFree((void **)&(z_rrc_params.z_saved_meas.p_SavedMessBuff));
          z_rrc_params.z_saved_meas.p_SavedMessBuff = NULL;
        }

        if ( p_umts_cp_wcdma_session_status->tm_core_session_started )
        {
          tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
          return;
        }
        else
        {
          tm_umts_cp_wcdma_session_cleanup();
          return;
        }
      }
      p_umts_cp_wcdma_session_status->tm_core_session_started = TRUE;
    } /* UE-Assisted */
    else if ( (eWCDMA_POSITION_MODE_AGPS_UE_BASED == p_umts_cp_wcdma_session_status->e_gnssPosMode) ||
              (eWCDMA_POSITION_MODE_A_GPS_GLO_UE_BASED == p_umts_cp_wcdma_session_status->e_gnssPosMode) )
    {

      tm_sess_req_type req_type = cp_wcdma_build_tm_session_msb_ptr(p_req_param, p_umts_cp_wcdma_session_status);

      if ( !tm_sess_req(TM_PRTL_TYPE_UMTS_CP,
                        p_umts_cp_wcdma_session_status->tm_active_session_handle,
                        req_type,
                        p_req_param
                       )
          )
      {
        if ( p_rrc_params->u_report_mode != RRC_NO_RPT )
        {
          (void)tm_umts_cp_wcdma_encode_rrc_msrReport_err(RRC_UNDEFINED_ERROR, NULL);
        }
        /* clear session related variables */
        if ( z_rrc_params.z_saved_meas.p_SavedMessBuff != NULL )
        {
          os_MemFree((void **)&(z_rrc_params.z_saved_meas.p_SavedMessBuff));
          z_rrc_params.z_saved_meas.p_SavedMessBuff = NULL;
        }

        if ( p_umts_cp_wcdma_session_status->tm_core_session_started )
        {
          tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
          return;
        }
        else
        {
          tm_umts_cp_wcdma_session_cleanup();
          return;
        }
      }
      p_umts_cp_wcdma_session_status->tm_core_session_started = TRUE;

    } /* Ue-Based */

  }  /* if call-flow-type not yet established */


  /*
  *   Incoming RRC MCM position method and/or position method type is contradict with previously established mode?
  */
  if ( TRUE == cp_wcdma_mcm_rpt_quantity_modified(p_umts_cp_wcdma_session_status, p_rrc_meas_ctrl) )
  {
    /*
    *     RRC MCM has modified position method type(UE-A/UE-B) and/or position method (GPS/GLO)
    */
    if ( TRUE == cp_wcdma_accept_modified_mcm_rpt_quantity(p_umts_cp_wcdma_session_status, p_rrc_meas_ctrl)
        )
    {
      if ( (eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED == p_umts_cp_wcdma_session_status->e_gnssPosMode) ||
           (eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED == p_umts_cp_wcdma_session_status->e_gnssPosMode)
          )
      {

        if ( p_umts_cp_wcdma_session_status->tm_core_session_started )
        {
          /* turn the receiver to idle at this point, with default Qos values that
             should be updated later by the the network */
          cp_wcdma_build_tm_session_msa_ptr(p_req_param, p_umts_cp_wcdma_session_status);

          if ( !tm_sess_req(TM_PRTL_TYPE_UMTS_CP,
                            p_umts_cp_wcdma_session_status->tm_active_session_handle,
                            TM_SESS_REQ_CONTINUE,
                            p_req_param
                           )
              )
          {
            tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
            return;
          }
        } /* if TmCore session is already on */
      } /* RRC modified to UE-A */
      else if ( (eWCDMA_POSITION_MODE_AGPS_UE_BASED == p_umts_cp_wcdma_session_status->e_gnssPosMode) ||
                (eWCDMA_POSITION_MODE_A_GPS_GLO_UE_BASED == p_umts_cp_wcdma_session_status->e_gnssPosMode)
               )
      {
        if ( p_umts_cp_wcdma_session_status->tm_core_session_started )
        {
          cp_wcdma_build_tm_session_msb_ptr(p_req_param, p_umts_cp_wcdma_session_status);

          if ( !tm_sess_req(TM_PRTL_TYPE_UMTS_CP,
                            p_umts_cp_wcdma_session_status->tm_active_session_handle,
                            TM_SESS_REQ_CONTINUE,
                            p_req_param
                           )
              )
          {
            tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
            return;
          }

        } /* if TmCore session is already on */

      } /* RRC asked/modified to UE-B */
    }
    else
    {
      MSG_4(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "MCM position method %3u, type %3u, ganss_incl %d, method %u is unsupported",
            p_rrc_meas_ctrl->rpt_quan.method, p_rrc_meas_ctrl->rpt_quan.method_type,
            p_rrc_meas_ctrl->rpt_quan.ganss_pos_method_incl, p_rrc_meas_ctrl->rpt_quan.ganss_pos_method);
      tm_umts_cp_wcdma_callflow_disrupt_handler();
      return;
    }
  } /* handled modified MCM reporting quantity - pos method & type */


  /* provide whatever assistance data received so far via TmCore to MGP */
  if ( p_rrc_meas_ctrl->gps_assistance_data_incl == TRUE )
  {
    (void)cp_wcdma_rrc_gps_assist_proc(&p_rrc_meas_ctrl->gps_assistance_data);
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "GPS AD delivered list 0x%lX",
          p_umts_cp_wcdma_session_status->q_assist_delivered_list);
  }

#ifdef FEATURE_GANSS_SUPPORT
  /* Glonass assistance data provided when ganss_assistance_data_incl is set to true.
   * Process GANSS AD only when glonass NV is set & there is no position method mismatch.
   */
  if ( tm_umts_common_glonass_enabled() && tm_umts_cp_wcdma_a_glo_rrc_cp_enabled()
      )
  {
    /* Compute Glonass time from GPSUtcModel + GPSRefTime. It's used in injecting Glonass AA.
    *  Don't bother about return value. If conversion fails Glonass steering won't be posted to TM. 
    */
    if ( p_WcdmaSessGnssTimeAd->m.deltaTPresent && p_WcdmaSessGnssTimeAd->m.refTimeWeekNumValid &&
         p_WcdmaSessGnssTimeAd->m.posTimeStampValid )
    {
      tm_util_convert_to_glonass_time(p_WcdmaSessGnssTimeAd->b_deltaT, (gnss_time_info *)(p_WcdmaSessGnssTimeAd));
    }

    if ( p_rrc_meas_ctrl->ganss_assistance_data_incl == TRUE )
    {
      cp_wcdma_rrc_ganss_assist_proc(p_rrc_meas_ctrl);
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Glonass AD delivered list 0x%lX",
            p_umts_cp_wcdma_session_status->q_glo_assists_delivered_list);
    }

    /* Inject Glonass acquisition assistance data after processing both GPS (UTCModel) +
       GLONASS (TimeModel, AuxiInfo) ADs */
    (void)cp_wcdma_rrc_inject_glonass_acq_assist_to_tmcore();
  }
#endif

  /****************** set horizontal accuracy *******************************/
  if ( e_e911_effect != C_EMERGENCY_HANDLING_NULL )
  {
    /* if in emergency, let the session run all the way to timeout before
       announcing the result, for better accuracy; this is achieved by
       setting horizontal accuracy to 0 for LM, ensuring LM only to report
       at timeout */
    p_umts_cp_wcdma_session_status->z_session_qos.q_hor_acc_meters = 0;
  }
  else /* not in emergency call */
  {
    if ( p_umts_cp_wcdma_session_status->u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_NI )
    {

      if ( (p_rrc_meas_ctrl->rpt_quan_incl == TRUE) &&
           (p_rrc_meas_ctrl->rpt_quan.hor_accuracy_incl == TRUE)
          )
      {
        p_umts_cp_wcdma_session_status->z_session_qos.q_hor_acc_meters =
                                                                         (uint32)tm_umts_common_utils_GADK_to_meter(
                                                                                                                    TRUE,
                                                                                                                    p_rrc_meas_ctrl->rpt_quan.horizontal_accuracy);
      }
      else
      {
        /* RRC layer does not pass the optional UEP Accuracy in RRC measControl, so for
           network initiated call flows, use the NV item of aagps_default_qos_unc */
        p_umts_cp_wcdma_session_status->z_session_qos.q_hor_acc_meters =
                                                                         z_umts_nv_item_ram_copy.aagps_default_qos_unc;
      }

    }
  } /* else - not emergency */

  /** if RRC measurementControl says "don't respond", it means another one
    with more assistance is on the way,  */
  if ( p_rrc_params->u_report_mode == RRC_NO_RPT )
  {
    /* start waiting for something from the network */
    tm_umts_cp_wcdma_set_cf_state(C_UMTS_CP_WCDMA_CF_STATE_WAIT_RRC, FALSE);
  }

  else if ( p_rrc_params->u_report_mode == RRC_PERIODIC_RPT )
  {

    /* For non-e911, if this is for UeAssisted and there is cached GNSS
       mesareuement already, send the measurement and end the session */
    if ( (tm_umts_common_utils_emergency_effect() ==
             C_EMERGENCY_HANDLING_NULL) &&
         ((p_umts_cp_wcdma_session_status->e_gnssPosMode ==
              eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED) ||
          (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED)) &&
         ((z_umts_cp_wcdma_session_status.z_cached_gnss_meas.u_meas_reports_bmap & SM_GNSS_MEAS_REPORTS_BMAP_GPS) ||
          (z_umts_cp_wcdma_session_status.z_cached_gnss_meas.u_meas_reports_bmap & SM_GNSS_MEAS_REPORTS_BMAP_GLONASS))
        )
    {

      /* tell TmCore to send pdapi event of EventGpsPdConnectionDone */
      if ( !p_umts_cp_wcdma_session_status->u_pdapi_comm_done_sent )
      {
        tm_umts_common_send_pdapi_comm_event
           (PDSM_PD_COMM_PROTOCOL_UMTS_CP_WCDMA,
            C_COMM_DONE,
            p_umts_cp_wcdma_session_status->tm_active_session_handle,
            p_umts_cp_wcdma_session_status->q_pdapi_client_id
           );

        p_umts_cp_wcdma_session_status->u_pdapi_comm_done_sent = TRUE;
      }

      if ( (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED) ||
           (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED) )
      {
        tm_umts_cp_wcdma_rrc_send_cached_gnss_meas_and_end_session();
      }
      return;
    }

    /* Activate the RRC-repsonse timer so that in case nothing coming back
       from MGP with regard to the aiding data status, or no additional request is allowed to
       be sent this time, at its expiration an RRC message will be sent.
    */

    if ( w_RrcPeriodReportAmount[p_rrc_meas_ctrl->rpt_crit.u.periodic_crit.rpt_amount] == 1 )
    {
      /* See tm_rrc.h for the mapping used below */
      if ( (p_rrc_meas_ctrl->rpt_crit.u.periodic_crit.rpt_interval != 0) &&
           (p_rrc_meas_ctrl->rpt_crit.u.periodic_crit.rpt_interval <= ARR_SIZE(w_RrcPeriodIntervalToMaxFixTimeout)) &&
           (p_rrc_meas_ctrl->rpt_crit.u.periodic_crit.rpt_interval <= u_array_size) )
      {
        p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long =
                                                                        w_RrcPeriodIntervalToMaxFixTimeout[p_rrc_meas_ctrl->rpt_crit.u.periodic_crit.rpt_interval - 1];
      }
      else
      {
        p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long = 0; /* 0 means undefined */
      }
    }

    else  /* else: if multiple periodic */
    {
      if ( p_umts_cp_wcdma_session_status->u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_UI )
      {
        /* mulitple reporting not supported by this version if MO */
        (void)tm_umts_cp_wcdma_encode_rrc_msrReport_err(RRC_UNDEFINED_ERROR, NULL);
        tm_umts_cp_wcdma_callflow_disrupt_handler();
        return;
      }

      p_rrc_params->rrc_periodic_multi_report = TRUE;

      p_umts_cp_wcdma_session_status->w_cp_periodic_reports_total_num =
                                                                        w_RrcPeriodReportAmount[p_rrc_meas_ctrl->rpt_crit.u.periodic_crit.rpt_amount];

      p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining =
                                                                        p_umts_cp_wcdma_session_status->w_cp_periodic_reports_total_num;

      if ( (p_rrc_meas_ctrl->rpt_crit.u.periodic_crit.rpt_interval != 0) &&
           (p_rrc_meas_ctrl->rpt_crit.u.periodic_crit.rpt_interval <= ARR_SIZE(w_RrcPeriodIntervalToMaxFixTimeout)) &&
           (p_rrc_meas_ctrl->rpt_crit.u.periodic_crit.rpt_interval <= u_array_size) )
      {
        /* for the first report, reportingInterval is used as responseTime; from the 1st report on,
           LSM is to send a report every reportingInterval seconds */

        p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long =
                                                                        w_RrcPeriodIntervalToMaxFixTimeout[p_rrc_meas_ctrl->rpt_crit.u.periodic_crit.rpt_interval - 1];

        p_umts_cp_wcdma_session_status->q_cp_periodic_reporting_interval_sec =
                                                                               p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long;

        p_umts_cp_wcdma_session_status->q_stored_cp_periodic_reporting_interval_sec =
                                                                                      p_umts_cp_wcdma_session_status->q_cp_periodic_reporting_interval_sec;

      }

      else  /* interval as "UE behavior undefined", use NvItem's default QosTime */
      {

        p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long = 0; /* undefined, will be aagps_default_qos_time */

        p_umts_cp_wcdma_session_status->q_cp_periodic_reporting_interval_sec =
                                                                               z_umts_nv_item_ram_copy.aagps_default_qos_time;
      }
      MSG_MED("Proc. multiple rpt req. Int. long %u, cp %lu", p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long,
              p_umts_cp_wcdma_session_status->q_cp_periodic_reporting_interval_sec, 0);
    } /* close if-RRC-CP-multiple reporting */

    tm_umts_cp_wcdma_set_cf_state(C_UMTS_CP_WCDMA_CF_STATE_TIMER_RRC_REPORT, FALSE);

    /* tell TmCore to send pdapi event of EventGpsPdConnectionDone */
    if ( !p_umts_cp_wcdma_session_status->u_pdapi_comm_done_sent )
    {
      tm_umts_common_send_pdapi_comm_event
         (PDSM_PD_COMM_PROTOCOL_UMTS_CP_WCDMA,
          C_COMM_DONE,
          p_umts_cp_wcdma_session_status->tm_active_session_handle,
          p_umts_cp_wcdma_session_status->q_pdapi_client_id
         );
      p_umts_cp_wcdma_session_status->u_pdapi_comm_done_sent = TRUE;
    }

    if ( (eWCDMA_POSITION_MODE_AGPS_UE_BASED == p_umts_cp_wcdma_session_status->e_gnssPosMode) ||
         (eWCDMA_POSITION_MODE_A_GPS_GLO_UE_BASED == p_umts_cp_wcdma_session_status->e_gnssPosMode) )
    {

      if ( !p_umts_cp_wcdma_session_status->additional_assist_request_done &&
           p_rrc_meas_ctrl->rpt_quan.additional_assistance_data_request &&
           p_umts_cp_wcdma_session_status->tm_core_session_started &&
           !p_umts_cp_wcdma_session_status->u_pdapi_fix_reported
          )
      {

        p_req_param->continue_param.op_mode = TM_SESS_OPERATION_MODE_MSB;
        p_req_param->continue_param.op_req = TM_OP_REQ_LR;
        p_req_param->continue_param.lr_qos = TM_MAX_SESSION_TIMEOUT;
        p_req_param->continue_param.accuracy_threshold = 0;
        p_req_param->continue_param.num_fixes = 1;
        p_req_param->continue_param.tbf_ms = 1000; /* don't care */
        p_req_param->continue_param.dynamic_qos_enabled = TRUE;

        p_req_param->continue_param.op_req |= TM_OP_REQ_AIDING_DATA_STATUS;

        (void)tm_sess_req(TM_PRTL_TYPE_UMTS_CP,
                          p_umts_cp_wcdma_session_status->tm_active_session_handle,
                          TM_SESS_REQ_CONTINUE,
                          p_req_param
                         );

        /* only request additional assistance status from MGP once per session */
        p_umts_cp_wcdma_session_status->additional_assist_request_done = TRUE;

        b_requestMissingAssistData = TRUE;

        MSG_MED("UMTS-CP-W: req. TmCore for wishlist", 0, 0, 0);

        /* Determine if it has enough info. about some SVs' nonexistence and
           inform MGP about such info., if available */
        tm_umts_cp_wcdma_post_non_exist_gnss_svs(p_umts_cp_wcdma_session_status);

      }
    } /* UEB end. Still within periodic reporting */
    else if ( (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED) ||
              (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED)
             )
    {
      /* ask for additional assistance if allowed and have not yet done */
      if ( !p_umts_cp_wcdma_session_status->additional_assist_request_done &&
           p_rrc_meas_ctrl->rpt_quan.additional_assistance_data_request )
      {
        sm_InternalAssistDataStatusStructType z_temp_assist_data_status;

        memset(&z_temp_assist_data_status, 0, sizeof(z_temp_assist_data_status));

        /* check to see if refTime and Acquistion assistance have been delivered; if not, request addtional */
        if ( p_umts_cp_wcdma_session_status->e_gnssPosMode & WCDMA_POSITION_METHOD_A_GLO )
        {
          tm_util_cp_msa_gnss_mandatory_ad_needed(&z_temp_assist_data_status.q_WishListMask, &z_temp_assist_data_status.z_GloAssistData.q_WishListMask,
                                                  p_umts_cp_wcdma_session_status->q_assist_delivered_list, p_umts_cp_wcdma_session_status->q_glo_assists_delivered_list);
        }
        else
        {
          tm_util_cp_msa_gps_mandatory_ad_needed(&z_temp_assist_data_status.q_WishListMask, p_umts_cp_wcdma_session_status->q_assist_delivered_list);
        }


        if ( z_temp_assist_data_status.q_WishListMask != 0 )
        {
          z_temp_assist_data_status.u_Valid |= SM_ASSIST_DATA_VALID_MASK_GPS;
        }
        if ( z_temp_assist_data_status.z_GloAssistData.q_WishListMask != 0 )
        {
          z_temp_assist_data_status.u_Valid |= SM_ASSIST_DATA_VALID_MASK_GLONASS;
        }

        /* Send MRM requesting missing GPS and/or GLONASS assistance data if the wishlist is valid (isn't empty) */
        if ( z_temp_assist_data_status.u_Valid != 0 )
        {
          /* only request additional assistance once per session */
          p_umts_cp_wcdma_session_status->additional_assist_request_done = TRUE;

          if ( tm_umts_cp_wcdma_encode_rrc_msrReport_err(RRC_ASSISTANCE_DATA_MISSING,
                                                         &z_temp_assist_data_status) )
          {
            b_requestMissingAssistData = TRUE;
          }
          else
          {
            tm_umts_cp_wcdma_callflow_disrupt_handler();
          }

        }
      } /* if not-yet-asked-additional and additional-req-allowed */
    } /* UeAssisted mode */

    if ( !p_umts_cp_wcdma_session_status->u_pdapi_fix_reported )
    {

      /* modify Qos timer based on RRC reportingIntervalLong */

      if ( p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long != 0 )
      {
        q_rrc_timer = p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long;
      }
      else /* w_rrc_reporting_interval_long == 0 means response time undefined by network */
      {
        q_rrc_timer = z_umts_nv_item_ram_copy.aagps_default_qos_time;
      }

      if ( p_umts_cp_wcdma_session_status->u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_NI )
      {
        if ( p_rrc_params->rrc_periodic_multi_report == TRUE )
        {
          /* for multiple periodic, MGP will not determine qoP. This non-zero value is used
             to flag the state in which reporting is allowed */
          p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs = q_rrc_timer;
        }
        else
        {
          /* let MGP to time out 2 seconds before RRC timeout, so that MGP will
            relax its criteria and report otherwise unqualified fix at timeout */
          p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs = q_rrc_timer - 2;
        }
      } /* NI */
      else /* UI */
      {
        if ( (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED) ||
             (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED) )
        {
          /* for UeAssisted, use the shorter of pdapi-qos-timer and ( rrcTime - 2 ) */
          if ( p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs >
                  (q_rrc_timer - 2)
              )
          {
            p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs =
                                                                                             q_rrc_timer - 2; /* let MGP to time out 2 seconds before RRC timeout, so that MGP will
                                                                                                                 relax its criteria and report otherwise unqualified fix at timeout */
          }
        }

        /* note for UeBased, MGP will only time out at pdapi Qos timer; if it is longer than RRC timer,
           UE will report no fix to network but continue on as standalone until pdapi Qos timer exp. */

      }

      /* QoS timer starts, qoS accuracy gets updated. For multiple periodic report,
         set lr_qos to max (TM_MULTI_REPT_MAX_SESSION_TIMEOUT) and horizontal accuracy to 0 */
      if ( p_umts_cp_wcdma_session_status->u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_NI )
      {
        tm_umts_cp_wcdma_inform_core_nt_assist_end(PERMIT_QOS_TIME_RESET);
      }
      else
      {
        tm_umts_cp_wcdma_inform_core_nt_assist_end(FORBID_QOS_TIME_RESET);
      }
    } /* pdapi report has NOT been sent */
    else
    {
      (void)tm_umts_cp_wcdma_encode_rrc_msrReport_pos(&p_umts_cp_wcdma_session_status->z_pending_pos_to_network,
                                                      p_umts_cp_wcdma_session_status->q_time_stamp_pos_pending_msec,
                                                      NULL);

      if ( p_umts_cp_wcdma_session_status->u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_UI )
      {
        /* wait for MOLR_ReturnResult */
        tm_umts_cp_wcdma_set_cf_state(C_UMTS_CP_WCDMA_CF_STATE_WAIT_RETURN_RESULT, FALSE);
      }
      else
      {

        /* clear timer and call flow state */
        tm_umts_cp_wcdma_terminate_timer_cancel_cf_state(C_UMTS_CP_WCDMA_CF_STATE_TIMER_RRC_REPORT);

        /* send DONE event to pdapi and end the session */
        tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_COMPLETED);
      }

    }


    if ( !b_requestMissingAssistData ) /* not going to request additional assistance */
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "UMTS-CP-W: final msrControl processed");

      if ( !p_umts_cp_wcdma_session_status->u_pdapi_fix_reported )
      {
        /* Determine if it has enough info. about some SVs' nonexistence and
           inform MGP about such info., if available */
        tm_umts_cp_wcdma_post_non_exist_gnss_svs(p_umts_cp_wcdma_session_status);
      }

    } /* else: not going to request additional assistance */

  } /* close if-report-mode-periodic */


}



/*===========================================================================

FUNCTION cp_wcdma_assist_delivery_proc

DESCRIPTION
  This function processes RRC AssistanceDataDelivery.

DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/

static void cp_wcdma_assist_delivery_proc( rrc_assistance_data_struct_type *p_assistance_data_params )
{
  /* clear timer and call flow state */
  tm_umts_cp_wcdma_terminate_timer_cancel_cf_state(C_UMTS_CP_WCDMA_CF_STATE_WAIT_RRC);

  /* this is needed in case pdapi report is sent before AssistanceData arrives, and
     if SS_ReturnResult (empty) fails to arrive, the protocol module will be able
     to end the entire session gracefully */
  tm_umts_cp_wcdma_set_cf_state(C_UMTS_CP_WCDMA_CF_STATE_WAIT_RETURN_RESULT, FALSE);

  (void)cp_wcdma_rrc_gps_assist_proc(p_assistance_data_params);

}

/*
 ******************************************************************************
 *
 * cp_wcdma_rrc_gps_refloc_assist_proc
 *
 * Function description:
 *  This function processes the OSS ASN.1 decoded RRC gpsReferenceLocation data and
 *  prepares such assistance data for injecting to MGP.
 *
 * Parameters:
 *
 *  b_GpsRefTimeValid: Whether GPS Reference time is included (w_GpsRefTimeWeekNum & q_GpsRefTimeTowMs is trustable)
 *  w_GpsRefTimeWeekNum: GPS time week number
 *  q_GpsRefTimeTowMs: GPS of week in milli second (0..604799999)
 *  p_GpsRefLoc: pointer to RRC LSM GPS Reference Location AD structure.
 *
 * Return value:
 *    None
 *
 ******************************************************************************
*/

static void cp_wcdma_rrc_gps_refloc_assist_proc( boolean b_GpsRefTimeValid, uint16 const w_GpsRefTimeWeekNum,
                                                 uint32 const q_GpsRefTimeTowMs,
                                                 rrc_gps_reference_loc_struct_type const *const p_GpsRefLoc )
{
  tm_post_data_payload_type z_tm_post_data_payload;

  if ( p_GpsRefLoc != NULL )
  {
    if ( z_umts_cp_wcdma_session_status.u_force_request_ref_location == FALSE )
    {
      (void)os_TimerStart(cp_wcdma_refloc_req_holdoff_timer, C_DEFER_REFLOC_REQ_MSEC, NULL);
    }

    /* z_last_refloc_info.z_RefPos_GAD stores the reference position in the original GAD shape
       format, in case there is a need later to report this back to the
       network */

    if ( tm_rrc_refpos_to_TmCore_post_data_payload(p_GpsRefLoc, &z_tm_post_data_payload,
                                                   &z_last_refloc_info.z_RefPos_GAD, b_GpsRefTimeValid,
                                                   w_GpsRefTimeWeekNum, q_GpsRefTimeTowMs) )
    {

      if ( tm_post_data(TM_PRTL_TYPE_UMTS_CP,
                        z_umts_cp_wcdma_session_status.tm_active_session_handle,
                        TM_POST_DATA_TYPE_SEED,
                        &z_tm_post_data_payload
                       ) == FALSE
          )
      {
        MSG_HIGH("CP-WCDMA: refLoc to Tmcore data post failed", 0, 0, 0);
      }
      else
      {
        MSG_MED("CP-WCDMA: refLoc posted to TmCore", 0, 0, 0);
      }

    }

    /* Post reference position from network in extended status position event */
    if ( tm_umts_cp_wcdma_report_ref_loc_in_ext_status_event() == TRUE )
    {
      MSG_MED("CP-WCDMA:  success in posting refL from MC as extended status position event to OEM", 0, 0, 0);
    }
    else
    {
      MSG_MED("CP-WCDMA: did not post refL from MC as extended status position event to OEM", 0, 0, 0);
    }
  } /* close "if-refLoc-available */
}

/*
 ******************************************************************************
 *
 * cp_wcdma_rrc_gps_assist_proc
 *
 * Function description:
 *  This function processes the OSS ASN.1 decoded RRC gpsAssistance data and
 *  prepares such assistance data for injecting to MGP.
 *
 * Parameters:
 *
 *  p_assist: pointer to the OSS ASN.1 decoded RRC gpsAsssitance data.
 *
 * Return value:
 *
 *
 ******************************************************************************
*/

static boolean cp_wcdma_rrc_gps_assist_proc( rrc_assistance_data_struct_type *p_assist )
{
  umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status = &z_umts_cp_wcdma_session_status;
  tm_post_data_payload_type           z_tm_post_data_payload;
  gps_RefTimeStructType               reftime_data;
  uint8                               u_i                             = 0;

  /* Init */
  memset(&z_tm_post_data_payload, 0x00, sizeof(tm_post_data_payload_type));

  /**************** GPS Real Time Intergrity ****************/
  /*
  * Note : CD/ME would benefit only when RTI is injected before RefLoc, NavModel & Alm.
  */
  if ( p_assist->gps_real_time_integrity_incl )
  {
    uint32 q_RTIValidSvs = 0, q_RTIInValidSvs = 0;

    /* Bad sat id list */
    for ( u_i = 0; ((u_i < p_assist->gps_rti.no_of_satellites) && (u_i < MAX_NO_OF_SATELLITES)); ++u_i )
    {
      /* satellite_id 0..63 */
      if ( p_assist->gps_rti.satellite_id[u_i] < N_GPS_SV )
      {
        q_RTIValidSvs |= 1L << (uint32)(p_assist->gps_rti.satellite_id[u_i]);
      }
      else
      {
        q_RTIInValidSvs |= 1L << (uint32)(p_assist->gps_rti.satellite_id[u_i] - N_GPS_SV);
      }
    }

    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Received RRC GPS RTI information 0x%X, 0x%X (SV > 32)", q_RTIValidSvs,
          q_RTIInValidSvs);

    /* Determine if we have enough info. about some SVs' health and
       inform MGP about such info., if available */
    if ( q_RTIValidSvs )
    {
      tm_util_post_unhealthy_svs(TM_PRTL_TYPE_UMTS_CP,
                                 p_umts_cp_wcdma_session_status->tm_active_session_handle,
                                 q_RTIValidSvs);
    }
  } /* GPS RTI */

  /************** GPS REFERENCE TIME **********************************/

  if ( p_assist->gps_ref_time_incl )
  {
    p_umts_cp_wcdma_session_status->q_assist_delivered_list |= C_GPS_ASSIS_REFTIME;

    /* Store the GPS reference time */
    p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp.w_ref_time_week_num =
                                                                                      gnss_GpsWeekExtend((uint16)p_assist->gps_ref_time.week);
    p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp.m.refTimeWeekNumValid = TRUE;
    p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp.q_ref_time_tow_ms = p_assist->gps_ref_time.time;
    p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp.m.refTimeTowMsValid = TRUE;

    /* Compute GPSTime since birth */
    p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp.t_pos_time_stamp_ms =
        tm_umts_common_utils_full_gps_ms(
            p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp.w_ref_time_week_num,
            p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp.q_ref_time_tow_ms);
    p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp.m.posTimeStampValid = TRUE;

    tm_rrc_reftime_to_TmCore_post_data_payload(p_assist, &z_tm_post_data_payload);

    if ( tm_post_data(TM_PRTL_TYPE_UMTS_CP,
                      p_umts_cp_wcdma_session_status->tm_active_session_handle,
                      TM_POST_DATA_TYPE_REFTIME,
                      &z_tm_post_data_payload
                     ) == FALSE
        )
    {
      MSG_HIGH("CP-WCDMA: GPS refTime to Tmcore data post failed", 0, 0, 0);
    }
    else
    {
      MSG_MED("CP-WCDMA: GPS refTime posted to TmCore", 0, 0, 0);
    }

    if ( p_assist->gps_ref_time.no_of_gps_tow_assist > 0 )
    {
#if 0   /*** QQQ-PM: need to discuss how to handle sparse pattern match **/
      /* QQQ: interface to be developed */

#endif
    } /* close if-GpsToW-Assist-available */
  } /* close if-refTime-available */


  /************** GPS REFERENCE LOCATION **********************************/

  if ( p_assist->gps_ref_loc_incl )
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Prepare GPS RefLoc to post");
    z_umts_cp_wcdma_session_status.q_assist_delivered_list |= C_GPS_ASSIS_REFLOC;
    cp_wcdma_rrc_gps_refloc_assist_proc(p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp.m.refTimeWeekNumValid,
                                        p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp.w_ref_time_week_num,
                                        p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp.q_ref_time_tow_ms,
                                        &p_assist->gps_ref_loc);
  }


  /************** GPS Nav. Model **********************************/

  if ( p_assist->gps_navigation_model_incl )
  {
    uint16                                         w_eph_week_num    = 0;
    rrc_navigation_model_sat_info_list_struct_type *p_satellite_info;

    p_satellite_info = &p_assist->gps_navigation_model.satellite_info[0];
    u_i = 0;

    p_umts_cp_wcdma_session_status->q_assist_delivered_list |= C_GPS_ASSIS_NAVMODL;

    (void)os_TimerStart(cp_wcdma_eph_req_holdoff_timer, C_DEFER_EPH_REQ_MSEC, NULL);

    while ( u_i < p_assist->gps_navigation_model.no_of_satellites )
    {
      /* SV ID */
      z_tm_post_data_payload.eph_data.u_Sv = p_satellite_info->satellite_id + 1;

      if ( p_satellite_info->ephermeral_clock_params_incl )
      {
        rrc_ephermeral_clock_struct_type *p_ephermeral_clock_params =
                                                                      &p_satellite_info->ephermeral_clock_params;

        /* if reference time is available in the same message, provide eph. week number for each SV */
        if ( p_assist->gps_ref_time_incl )
        {
          w_eph_week_num = tm_umts_common_utils_build_eph_week_from_ref_time(p_ephermeral_clock_params->t_oe,
                                                                             p_assist->gps_ref_time.time,
                                                                             p_assist->gps_ref_time.week);
        }
        else
        {
          w_eph_week_num = 0;
        }

        w_eph_week_num = 0;
        tm_rrc_eph_to_TmCore_post_data_payload(p_ephermeral_clock_params, &z_tm_post_data_payload, w_eph_week_num);

        p_umts_cp_wcdma_session_status->q_eph_delivered_sv_list |= 1L << p_satellite_info->satellite_id;

        if ( tm_post_data(TM_PRTL_TYPE_UMTS_CP,
                          p_umts_cp_wcdma_session_status->tm_active_session_handle,
                          TM_POST_DATA_TYPE_EPH,
                          &z_tm_post_data_payload
                         ) == FALSE
            )
        {
          MSG_HIGH("GPS SV %u eph. post to TmCore failed", z_tm_post_data_payload.eph_data.u_Sv, 0, 0);
        }
        else
        {
          MSG_MED("GPS SV %u eph. posted to TmCore", z_tm_post_data_payload.eph_data.u_Sv, 0, 0);
        }

      } /* if ephermeral_clock_params_incl */

      p_satellite_info++;
      u_i++;

    }; /* sv list while-loop */

  } /* close if-eph-available */


  /****************************** GPS IONO ****************************************/
  /* Inject GPS IonoModel AD as it comes. Since GPS & Glonass Iono is same, a single holdoff timer is used. */
  if ( p_assist->gps_iono_incl )
  {

    p_umts_cp_wcdma_session_status->q_assist_delivered_list |= C_GPS_ASSIS_IONO;

    (void)os_TimerStart(cp_wcdma_iono_req_holdoff_timer, C_DEFER_IONO_REQ_MSEC, NULL);

    tm_rrc_Iono_to_TmCore_post_data_payload(&p_assist->gps_iono, &z_tm_post_data_payload);

    if ( tm_post_data(TM_PRTL_TYPE_UMTS_CP,
                      p_umts_cp_wcdma_session_status->tm_active_session_handle,
                      TM_POST_DATA_TYPE_IONO,
                      &z_tm_post_data_payload
                     ) == FALSE
        )
    {
      MSG_HIGH("CP-WCDMA GPS IONO to Tmcore data post failed", 0, 0, 0);
    }
    else
    {
      MSG_MED("CP-WCDMA: GPS IONO posted to TmCore", 0, 0, 0);
    }

  }


  /****************************** GPS UTC ****************************************/

  if ( p_assist->gps_utc_model_incl )
  {

    p_umts_cp_wcdma_session_status->q_assist_delivered_list |= C_GPS_ASSIS_UTC;

    (void)os_TimerStart(cp_wcdma_utc_req_holdoff_timer, C_DEFER_UTC_REQ_MSEC, NULL);

    tm_rrc_UtcModel_to_TmCore_post_data_payload(p_assist, &z_tm_post_data_payload);

    /* Store the leap second offset & later used to compute the Glonass time */
    p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp.m.deltaTPresent = 1;
    p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp.b_deltaT = p_assist->gps_utc_model.u_DeltaTls;

    if ( tm_post_data(TM_PRTL_TYPE_UMTS_CP,
                      p_umts_cp_wcdma_session_status->tm_active_session_handle,
                      TM_POST_DATA_TYPE_UTC_MODEL,
                      &z_tm_post_data_payload
                     ) == FALSE
        )
    {
      MSG_HIGH("CP-WCDMA: GPS UTC to Tmcore data post failed", 0, 0, 0);
    }
    else
    {
      MSG_MED("CP-WCDMA: GPS UTC posted to TmCore", 0, 0, 0);
    }

  }

  /******************* GPS Almanac ***************************************************/
  if ( p_assist->gps_almanac_incl )
  {
    uint16                           w_AlmanacWeek;
    uint8                            u_i;
    rrc_almanac_sat_info_struct_type *p_sat_info_list;

    p_umts_cp_wcdma_session_status->q_assist_delivered_list |= C_GPS_ASSIS_ALM;

    /* once delivered, subsequent sessions should not req. this assist. in the
     next certain minutes */
    (void)os_TimerStart(cp_wcdma_alm_req_holdoff_timer, C_DEFER_ALM_REQ_MSEC, NULL);

    /* Almanac WNa  */
    w_AlmanacWeek = (int16)p_assist->gps_almanc.wn_a; /* 8 LSBs of the full 10-bit alm. week number */

    p_sat_info_list = &p_assist->gps_almanc.almanac_sat_info_list[0];

    u_i = 0;
    while ( u_i < p_assist->gps_almanc.no_of_almanac_sat )
    {
      tm_rrc_alm_to_TmCore_post_data_payload(p_sat_info_list, &z_tm_post_data_payload);
      z_tm_post_data_payload.alm_data.w_GpsWeek = w_AlmanacWeek;

      z_tm_post_data_payload.alm_data.u_SvGhealthIsValid = FALSE; /* QQQ: if any server uses that, implement it later */

      p_umts_cp_wcdma_session_status->q_alm_delivered_sv_list |= 1L << p_sat_info_list->satellite_id;

      if ( tm_post_data(TM_PRTL_TYPE_UMTS_CP,
                        p_umts_cp_wcdma_session_status->tm_active_session_handle,
                        TM_POST_DATA_TYPE_ALM,
                        &z_tm_post_data_payload
                       ) == FALSE
          )
      {
        MSG_HIGH("CP-WCDMA: GPS SV %u alm post to TmCore failed", z_tm_post_data_payload.alm_data.u_Sv, 0, 0);
      }
      else
      {
        MSG_MED("CP-WCDMA: GPS SV %u alm. posted to TmCore", z_tm_post_data_payload.alm_data.u_Sv, 0, 0);
      }

      p_sat_info_list++;
      u_i++;

    } /* while-loop for SVs in alamanc list */

  } /* if almanac present */


  /**************** GPS acquisition assistance **********************/

  if ( p_assist->gps_acq_assistance_incl )
  {
    p_umts_cp_wcdma_session_status->q_assist_delivered_list |= C_GPS_ASSIS_ACQ;

    if ( (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED) ||
         (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED) )
    {

      /* if refTime is in this message, use week_number valid when injecting
         steering; otherwise set week_number invalid */
      memset(&reftime_data, 0, sizeof(gps_RefTimeStructType));
      reftime_data.w_GpsWeek = C_GPS_WEEK_UNKNOWN;

      if ( p_assist->gps_ref_time_incl )
      {
        memscpy(&reftime_data, sizeof(reftime_data), &z_tm_post_data_payload.reftime_data, sizeof(z_tm_post_data_payload.reftime_data));
      }

      /* inject SV directions */
      tm_rrc_AcqAssistSvDir_to_TmCore_post_data_payload(p_assist, &z_tm_post_data_payload);
      if ( z_tm_post_data_payload.gnss_sv_dir.u_NumSvs ) /* Optional AdditionalAngle */
      {
        if ( tm_post_data(TM_PRTL_TYPE_UMTS_CP,
                          p_umts_cp_wcdma_session_status->tm_active_session_handle,
                          TM_POST_DATA_TYPE_SV_DIR,
                          &z_tm_post_data_payload
                         ) == FALSE
            )
        {
          MSG_HIGH("CP-WCDMA: GPS SvDir to TmCore data post failed", 0, 0, 0);
        }
        else
        {
          MSG_MED("CP-WCDMA: GPS SvDir posted to TmCore", 0, 0, 0);
        }
      }


      /* inject steering */
      tm_rrc_AcqAssistSteering_to_TmCore_post_data_payload(p_assist, &z_tm_post_data_payload, reftime_data.w_GpsWeek);
      if ( tm_post_data(TM_PRTL_TYPE_UMTS_CP,
                        p_umts_cp_wcdma_session_status->tm_active_session_handle,
                        TM_POST_DATA_TYPE_AA,
                        &z_tm_post_data_payload
                       ) == FALSE
          )
      {
        MSG_HIGH("CP-WCDMA: GPS Steering to Tmcore data post failed", 0, 0, 0);
      }
      else
      {
        MSG_MED("CP-WCDMA: GPS Steering posted to TmCore", 0, 0, 0);
      }
    } /* close if-SET-Assisted */

  } /* close if-acq-assist-available */

  return ( TRUE );
}


#ifdef FEATURE_GANSS_SUPPORT

/*
 ******************************************************************************
 *
 * cp_wcdma_rrc_inject_glo_eph_to_tmcore
 *
 * Function description:
 *  This function processes the decoded RRC ganssAssistance data and injects
 *  into MGP.
 *
 * Parameters:
 *
 *  tm_active_session_handle: Pointer to active session.
 *  p_ProtGloEphBuf: Pointer to glonass eph. buffer
 *  pz_tm_post_data_payload : Pointer to TM post data
 *  p_glo_eph_injected_svs : Injected glonass eph. SVs.
 *
 * Return value:
 *  True if Glo eph. is injected to MGP, else false.
 *
 ******************************************************************************
*/
boolean cp_wcdma_rrc_inject_glo_eph_to_tmcore( const tm_sess_handle_type tm_active_session_handle,
                                               prot_glo_eph_buffer_s_type *p_ProtGloEphBuf,
                                               tm_post_data_payload_type *pz_tm_post_data_payload,
                                               uint32 *p_glo_eph_injected_svs )
{

  /* inject this Glo SV eph */
  memscpy(&pz_tm_post_data_payload->eph_glo_data, sizeof(pz_tm_post_data_payload->eph_glo_data),
          &p_ProtGloEphBuf->z_eph_glo_data, sizeof(p_ProtGloEphBuf->z_eph_glo_data));

  if ( FALSE == tm_post_data(TM_PRTL_TYPE_UMTS_CP,
                             tm_active_session_handle,
                             TM_POST_DATA_TYPE_EPH_GLO,
                             pz_tm_post_data_payload
                            ) )
  {
    /* MGP valid SatId range: 65-88 */
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "GLO NavModel SatId %u failed posting to TmCore",
          pz_tm_post_data_payload->eph_glo_data.u_SvId);

    /* reset these flags so that they won't get injected again later */
    p_ProtGloEphBuf->z_eph_glo_data.u_SvId = 0;
    p_ProtGloEphBuf->u_freq_num_valid = FALSE;
  }
  else
  {
    *p_glo_eph_injected_svs |= 1L << (pz_tm_post_data_payload->eph_glo_data.u_SvId - (GLO_SLOTID_OFFSET + 1));
    return ( TRUE );
  }

  return ( FALSE );
}

/*
 ******************************************************************************
 *
 * cp_wcdma_rrc_ganss_assist_proc
 *
 * Function description:
 *  This function processes the decoded RRC ganssAssistance data and  prepares such
 *  assistance data for injecting to MGP.
 *
 * Parameters:
 *
 *  p_RrcMeasCtrl: pointer to the decoded RRC ganssAsssitance data.
 *
 *
 *
 ******************************************************************************
*/

static void cp_wcdma_rrc_ganss_assist_proc( rrc_meas_ctrl_struct_type const *const p_RrcMeasCtrl )
{
  rrc_ganss_assis_data_struct_type const *p_assist                       = NULL;
  umts_cp_wcdma_session_status_s_type    *p_umts_cp_wcdma_session_status = &z_umts_cp_wcdma_session_status;
  tm_post_data_payload_type              z_tm_post_data_payload;
  uint32                                 q_index                         = 0;
  uint8                                  u_i                             = 0,
                                         u_GanssGenericADIndex           = 0;
  rrc_ganss_data_list_struct_type const  *p_GanssGenericAD               = NULL;

  if ( NULL == p_RrcMeasCtrl )
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "No RRC GANSS MeasCtrl to process.");
    return;
  }

  p_assist = &p_RrcMeasCtrl->ganss_assistance_data;

  memset(&z_tm_post_data_payload, 0x00, sizeof(tm_post_data_payload_type));

  /*
  * GANSS RealTime Integrity (GLO)
  * Note : CD would benefit only when RTI is injected before RefLoc, NavModel & Alm.
  */
  if ( (TRUE == p_assist->ganss_data_list_incl) && (p_assist->list_cnt > 0) )
  {
    /* Iterate through each generic AD list */
    for ( u_GanssGenericADIndex = 0;
          (u_GanssGenericADIndex < p_assist->list_cnt) && (p_assist->list_cnt <= MAX_GANSS);
          ++u_GanssGenericADIndex )
    {
      p_GanssGenericAD = &p_assist->ganss_data_list[u_GanssGenericADIndex];

      /* Interested only when GanssId is Glonass */
      if ( (TRUE == p_GanssGenericAD->ganss_id_incl) && (RRC_GLONASS == p_GanssGenericAD->ganssId) )
      {

        if ( (TRUE == p_GanssGenericAD->RealTime_Integrity_incl) && (p_GanssGenericAD->sat_cnt > 0) )
        {
          uint32 q_OtherRealTimeIntegrity = 0;

          z_tm_post_data_payload.gnss_sv_health_info.q_BadSvMask = 0;

          /** RTI isn't minimum required AD & won't be request. So no holdoff timer required. **/
          p_umts_cp_wcdma_session_status->q_glo_assists_delivered_list |= C_GNSS_ASSIST_RTI;

          for ( u_i = 0; u_i < p_GanssGenericAD->sat_cnt; ++u_i )
          {
            /* Satellite id range (0..63). For Glonass valid from (0..23)
            *  API: GLO bit 0-23 => Slot ID:1-24)
            */
            if ( p_GanssGenericAD->RealTime_Integrity[u_i].bad_ganss_satId < N_GLO_SV )
            {

              z_tm_post_data_payload.gnss_sv_health_info.q_BadSvMask |= 1L << (uint32)(p_GanssGenericAD->RealTime_Integrity[u_i].bad_ganss_satId);
            }
            else
            {
              q_OtherRealTimeIntegrity |= 1L << (uint32)(p_GanssGenericAD->RealTime_Integrity[u_i].bad_ganss_satId - N_GLO_SV);
            }
          } /* For each SatId */

          /* Determine if we have enough info. about some SVs' health and inform MGP about such info., if available */
          if ( z_tm_post_data_payload.gnss_sv_health_info.q_BadSvMask )
          {
            z_tm_post_data_payload.gnss_sv_health_info.e_GnssType = MGP_GNSS_TYPE_GLO;

            if ( FALSE == tm_post_data(TM_PRTL_TYPE_UMTS_CP,
                                       p_umts_cp_wcdma_session_status->tm_active_session_handle,
                                       TM_POST_DATA_TYPE_GNSS_SV_HEALTH_INFO,
                                       &z_tm_post_data_payload
                                      ) )
            {
              MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRC GLO SV RTI failed posting to TmCore");

            }
            else
            {
              MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "RRC GLO SV RTI 0x%lX posted to TmCore",
                    z_tm_post_data_payload.gnss_sv_health_info.q_BadSvMask);
            }
          }

          if ( q_OtherRealTimeIntegrity != 0 )
          {
            MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "RRC non-GLO SV RTI 0x%lX (SV > 24)", q_OtherRealTimeIntegrity);
          }
        } /* Glonass RTI */
      } /* GanssId == Glonass */
    } /* Iterate each Generic AD list */
  } /* Generic AD included */

  /************** GANSS REFERENCE TIME (GLO) **********************************/
  if ( TRUE == p_assist->ganss_ref_time_incl )
  {
    p_umts_cp_wcdma_session_status->q_glo_assists_delivered_list |= C_GNSS_ASSIST_REFTIME;

    tm_rrc_glo_reftime_to_tm_post_data_payload(&p_assist->ganss_ref_time, &z_tm_post_data_payload);

    if ( tm_post_data(TM_PRTL_TYPE_UMTS_CP,
                      p_umts_cp_wcdma_session_status->tm_active_session_handle,
                      TM_POST_DATA_TYPE_REFTIME_GLO,
                      &z_tm_post_data_payload
                     ) == FALSE
        )
    {
      MSG_HIGH("CP-WCDMA: GloRefTime failed post to Tmcore", 0, 0, 0);
    }
    else
    {
      MSG_MED("CP-WCDMA: GloRefTime posted to TmCore", 0, 0, 0);
    }

  } /* close if-refTime-available */


  /************** GANSS REFERENCE LOCATION (GLO) **********************************/
  /* A prior knowledge of UE 3-D position. If IE “GPS ReferencePosition” is present, this IE should not be included.
  *  The reference location can be included in either GPS or GANSS, standard allows sending only one.
  *  Only GPS reference time is associated to all ADs. So use GPSRefTime to process GanssRefLoc.
  */
  if ( TRUE == p_assist->ganss_ref_pos_incl )
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Prepare Glonass RefLoc to post");
    z_umts_cp_wcdma_session_status.q_glo_assists_delivered_list |= C_GNSS_ASSIST_REFLOC;

    /* Inject GloRefLocation as it comes.
       In case of 2 ref. position (GPS & Glonass) is sent by network, TM post the ref. position always as GPS Ref.Pos AD */
    cp_wcdma_rrc_gps_refloc_assist_proc(p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp.m.refTimeWeekNumValid,
                                        p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp.w_ref_time_week_num,
                                        p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp.q_ref_time_tow_ms,
                                        &p_assist->ganss_ref_pos);
  }


  /****************************** GANSS ADDIONOModel ****************************************/
  /* Inject GLONASS IonoModel AD as it comes. Since GPS & Glonass Iono is same, a single holdoff timer is used. */
  if ( TRUE == p_assist->ganss_addlono_model_incl )
  {
    z_umts_cp_wcdma_session_status.q_glo_assists_delivered_list |= C_GNSS_ASSIST_IONO;

    (void)os_TimerStart(cp_wcdma_iono_req_holdoff_timer, C_DEFER_IONO_REQ_MSEC, NULL);

    tm_rrc_Iono_to_TmCore_post_data_payload(&p_assist->ganss_addlono_model.alpha_beta_param, &z_tm_post_data_payload);

    if ( tm_post_data(TM_PRTL_TYPE_UMTS_CP,
                      p_umts_cp_wcdma_session_status->tm_active_session_handle,
                      TM_POST_DATA_TYPE_IONO,
                      &z_tm_post_data_payload
                     ) == FALSE
        )
    {
      MSG_HIGH("CP-WCDMA: GLO IONO to Tmcore data post failed", 0, 0, 0);
    }
    else
    {
      MSG_MED("CP-WCDMA: GLO IONO posted to TmCore", 0, 0, 0);
    }

  }

  /************** GANSS Generic Assistance Data **********************************/

  if ( (TRUE == p_assist->ganss_data_list_incl) && (p_assist->list_cnt > 0) )
  {
    uint8                                      u_num_glo_eph_injected = 0;
    uint32                                     q_eph_svs_not_injected = 0, q_glo_eph_injected_sv_list = 0;
    rrc_GANSS_add_navigation_models_type const *p_GloAddNavModel      = NULL;
    rrc_sat_info_nav_list_type const           *p_GloSvNavModel       = NULL;

    u_i = 0;
    u_GanssGenericADIndex = 0;
    p_GanssGenericAD = NULL;

    /* Iterate through each generic AD list */
    for ( u_GanssGenericADIndex = 0;
          (u_GanssGenericADIndex < p_assist->list_cnt) && (p_assist->list_cnt <= MAX_GANSS);
          ++u_GanssGenericADIndex )
    {
      p_GanssGenericAD = &p_assist->ganss_data_list[u_GanssGenericADIndex];

      /* Interested only when GanssId is Glonass */
      if ( (TRUE == p_GanssGenericAD->ganss_id_incl) && (RRC_GLONASS == p_GanssGenericAD->ganssId) )
      {
        /************** GANSS TimeModel (GLO) **********************************/
        if ( (TRUE == p_GanssGenericAD->time_mode_list_incl) && (p_GanssGenericAD->list_cnt > 0) )
        {
          (void)os_TimerStart(cp_wcdma_glo_time_model_req_holdoff_timer, C_DEFER_GLO_TIME_MODEL_REQ_MSEC, NULL);
          p_umts_cp_wcdma_session_status->q_glo_assists_delivered_list |= C_GNSS_ASSIST_TIME_MODELS;

          for ( u_i = 0; u_i < p_GanssGenericAD->list_cnt; ++u_i )
          {
            /* Relate GLONASS time to GPS */
            if ( GANSS_ID_GPS == p_GanssGenericAD->time_mode_list[u_i].ganss_to_id )
            {
              /* leap second isn't available in RRC R8 GANSS TimeModel. Expected in RRC R9 & then call
                 tm_util_convert_to_glonass_time() to compute the Glonass time */

              /* Init */
              memset(&z_tm_post_data_payload.gnss_time_model_data, 0,
                     sizeof(z_tm_post_data_payload.gnss_time_model_data));

              /* t-a0 (a.k.a TauGPS) : (-2147483648..2147483647), in seconds */
              z_tm_post_data_payload.gnss_time_model_data.l_A0 = p_GanssGenericAD->time_mode_list[u_i].ganss_t_a0;
              /* ganss-timeModelreferenceTime : GANSS Ref.Time Of Week (0..37799) */
              z_tm_post_data_payload.gnss_time_model_data.q_GanssTimeModelRefTime = p_GanssGenericAD->time_mode_list[u_i].ganss_model_ref_time;
              /* Only “gps” is supported to relate GLONASS to the selected time reference indicated by GPS.
                  The condition is already checked above.*/
              z_tm_post_data_payload.gnss_time_model_data.u_GnssToID = C_GNSS_TO_ID_GPS;

              if ( p_GanssGenericAD->time_mode_list[u_i].ganss_t_a1_incl )
              {
                /* (-8388608..8388607), in sec/sec */
                z_tm_post_data_payload.gnss_time_model_data.x_A1 = p_GanssGenericAD->time_mode_list[u_i].ganss_t_a1;
              }

              /* This IE is not applicable since there is no week number for GLONASS */
              if ( TRUE == p_GanssGenericAD->time_mode_list[u_i].ganss_wk_number_incl )
              {
                z_tm_post_data_payload.gnss_time_model_data.w_WeekNo = p_GanssGenericAD->time_mode_list[u_i].ganss_wk_number;
              }

              /* RG: t-a2 not supported by MPSS */
              /* DeltaT - leapSecondOffset isn't available from TimeModel until RRC R9 */

              if ( FALSE == tm_post_data(TM_PRTL_TYPE_UMTS_CP,
                                         p_umts_cp_wcdma_session_status->tm_active_session_handle,
                                         TM_POST_DATA_TYPE_TIME_MODEL_GLO,
                                         &z_tm_post_data_payload
                                        ) )
              {
                MSG_HIGH("RRC GLO TimeModel failed posting to TmCore", 0, 0, 0);
              }
              else
              {
                MSG_MED("RRC GLO TimeModel posted to TmCore", 0, 0, 0);
              }
            }
          }
        }

        /************** GANSS GLO Reference Measurement Info (Acquisition Assistance) **********************************/
        if ( TRUE == p_GanssGenericAD->ref_meas_info_incl )
        {
          /* Supported gnss-signalId 0 (G1) */
          if ( (TRUE == p_GanssGenericAD->ref_meas_info.ganss_sig_Id_incl) && (p_GanssGenericAD->ref_meas_info.ganss_sig_Id != RRC_GANSS_GLONASS_SIGNAL_ID_G1) )
          {
            MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unsupported Glonass signal %d in GLO AcqAsst", p_GanssGenericAD->ref_meas_info.ganss_sig_Id);
          }
          else
          {
            p_umts_cp_wcdma_session_status->q_glo_assists_delivered_list |= C_GNSS_ASSIST_ACQ;

            /* Build SV directions (Azim & Elevation) & Steering information if Glonass time is available.
               Else take care of Glonass time & freqNum mapping later when GPS TimeModel & Glo AuxiInfo AD is received */
            if ( p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp.m.glonassTimeValid )
            {
              tm_rrc_glo_acq_asst_to_tm_post_data_and_store(&p_GanssGenericAD->ref_meas_info,
                                                            &p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_buf,
                                                            p_umts_cp_wcdma_session_status->z_set_assist_pos_time_stamp.z_GlonassTime.q_GloMsec,
                                                            NULL);
            }
            else
            {
              /* Store the integer code phase required to derive the gnss ms of week */
              tm_rrc_glo_acq_asst_to_tm_post_data_and_store(&p_GanssGenericAD->ref_meas_info,
                                                            &p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_buf,
                                                            0xFFFFFFFF,
                                                            &p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_subset);
              MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "SvId & IntCodePhase stored internally %p",
                    p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_subset.p_svid_icp);

            }
          }
        } /* End of Glo Reference Measurement Info (Acquisition Assistance) */

        /************** GANSS Auxiliary Info (GLO) **********************************/
        if ( TRUE == p_GanssGenericAD->Auxiliary_Info_incl )
        {
          if ( (p_GanssGenericAD->Auxiliary_Info.sat_cnt > 0) &&
               (GANSS_ID3 == p_GanssGenericAD->Auxiliary_Info.Auxiliary_type) )
          {
            (void)os_TimerStart(cp_wcdma_glo_auxi_info_req_holdoff_timer, C_DEFER_GLO_AUXI_INFO_REQ_MSEC, NULL);
            p_umts_cp_wcdma_session_status->q_glo_assists_delivered_list |= C_GNSS_ASSIST_AUXI_INFO;

            if ( p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_BASED )
            {
              /* Map Glo Sv Id & Channel Frequency Number in Nav struct */
              tm_rrc_map_glo_eph_svid_to_freq_num_from_aux(p_GanssGenericAD->Auxiliary_Info.u.ganssID3,
                                                           p_GanssGenericAD->Auxiliary_Info.sat_cnt,
                                                           p_umts_cp_wcdma_session_status->z_prot_glo_eph_buf);
            }
            else if ( p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED )
            {
              /* Map Glo Sv Id & Channel Frequency Number if GloAcqAsst available, else store the GloAuxiInfo */
              tm_rrc_map_glo_acqast_svid_to_freq_num_from_aux(p_GanssGenericAD->Auxiliary_Info.u.ganssID3,
                                                              p_GanssGenericAD->Auxiliary_Info.sat_cnt,
                                                              &p_umts_cp_wcdma_session_status->z_prot_glo_acqasst_buf,
                                                              &p_umts_cp_wcdma_session_status->z_ProtGloSvCnBuf);
            }
          }
        } /* AuxiInfo */


        /************** GANSS AddUTCModel (GLO) **********************************/
        if ( TRUE == p_GanssGenericAD->add_utc_models_incl )
        {
          /** The leapSecondOffset (deltaTLS) in GPS UTCModel is used for Glonass time computation. So this
           *  GANSS AddUTCModel won't be request even when server haven't provided. Hence no holdoff timer. **/
          p_umts_cp_wcdma_session_status->q_glo_assists_delivered_list |= C_GNSS_ASSIST_UTC;

          if ( TRUE == tm_rrc_glo_utcmodel_to_tm_post_data_payload(&p_GanssGenericAD->add_utc_models,
                                                                   &z_tm_post_data_payload) )
          {
            if ( FALSE == tm_post_data(TM_PRTL_TYPE_UMTS_CP,
                                       p_umts_cp_wcdma_session_status->tm_active_session_handle,
                                       TM_POST_DATA_TYPE_UTC_MODEL_GLO,
                                       &z_tm_post_data_payload
                                      ) )
            {
              MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRC GLO AddUTCModel failed posting to TmCore");
            }
            else
            {
              MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "RRC GLO AddUTCModel posted to TmCore");
            }
          }
          else
          {
            MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRC GLO AddUTCModel conversion failure");
          }
        }

        /************** GANSS AddNavigationModel (GLO) **********************************/
        if ( TRUE == p_GanssGenericAD->add_navigation_models_incl )
        {
          u_num_glo_eph_injected = 0;
          u_i = 0;
          p_GloAddNavModel = &p_GanssGenericAD->add_navigation_models;
          p_GloSvNavModel = &p_GloAddNavModel->sat_info_list[0];

          (void)os_TimerStart(cp_wcdma_glo_eph_req_holdoff_timer, C_DEFER_EPH_REQ_MSEC, NULL);
          p_umts_cp_wcdma_session_status->q_glo_assists_delivered_list |= C_GNSS_ASSIST_NAVMODL;

          /* The satellite count - sat_cnt (1..64) */
          while ( u_i < p_GloAddNavModel->sat_cnt )
          {
            /* Satellite id range (0..63). For Glonass valid from (0..23) */
            if ( p_GloSvNavModel->sat_id < N_GLO_SV )
            {
              p_umts_cp_wcdma_session_status->q_glo_eph_delivered_sv_list |= 1L << p_GloSvNavModel->sat_id;

              tm_rrc_glo_eph_to_common_prot_struct(p_GloSvNavModel,
                                                   &p_umts_cp_wcdma_session_status->z_prot_glo_eph_buf[p_GloSvNavModel->sat_id].z_eph_glo_data);

              /* if this SV already has freq. number, inject it now; otherwise keep data in the buffer and inject when all
              *  assistance have been delivered. Remove the injected SV from buffer struct to avoid injecting multiple times.
              */
              if ( p_umts_cp_wcdma_session_status->z_prot_glo_eph_buf[p_GloSvNavModel->sat_id].u_freq_num_valid )
              {

                if ( TRUE == cp_wcdma_rrc_inject_glo_eph_to_tmcore(p_umts_cp_wcdma_session_status->tm_active_session_handle,
                                                                   &p_umts_cp_wcdma_session_status->z_prot_glo_eph_buf[p_GloSvNavModel->sat_id],
                                                                   &z_tm_post_data_payload,
                                                                   &q_glo_eph_injected_sv_list) )
                {
                  /* Clean-up the injected Glonass Eph. SV, leaving the FreqNum-to-SlotId mapping intact.
                  *  Since FreqNum-to-SlotId mapping is valid for atleast the time duration of Glo Almanac */
                  cp_wcdma_retain_slot_id_freq_map_only(&p_umts_cp_wcdma_session_status->z_prot_glo_eph_buf[p_GloSvNavModel->sat_id]);

                  ++u_num_glo_eph_injected;
                }
              } /* if this SV already has freq. number */
              else
              {
                q_eph_svs_not_injected |= 1L << p_GloSvNavModel->sat_id;
              }
            } /* Each Glonass SVs */

            /* Iterate to next SV's NavModel */
            ++p_GloSvNavModel;
            ++u_i;
          } /* sv list while-loop */

          if ( q_glo_eph_injected_sv_list != 0 )
          {
            MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,
                  "LS provided %u GLO SV NavModel.%3u (0x%lX) posted to TmCore", p_GloAddNavModel->sat_cnt,
                  u_num_glo_eph_injected, q_glo_eph_injected_sv_list);
          }
          if ( q_eph_svs_not_injected != 0 )
          {
            MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED,
                  "LS provided %u GLO SV NavModel. 0x%lX has no freq num. So pending posting to TmCore",
                  p_GloAddNavModel->sat_cnt, q_eph_svs_not_injected);
          }

        } /* close if-glo-eph-available */

        /************** GANSS Almanac (GLO) **********************************/
        if ( TRUE == p_GanssGenericAD->GANSS_Almanac_incl )
        {
          uint32 q_alm_svs_injected = 0;

          if ( (p_GanssGenericAD->GANSS_Almanac.sat_cnt > 0) &&
               (TRUE == p_GanssGenericAD->GANSS_Almanac.almanac_set_incl) )
          {
            p_umts_cp_wcdma_session_status->q_glo_assists_delivered_list |= C_GNSS_ASSIST_ALM;

            /* once delivered should not req. this assist. within next certain minutes */
            (void)os_TimerStart(cp_wcdma_glo_alm_req_holdoff_timer, C_DEFER_ALM_REQ_MSEC, NULL);

            /* Glonass doesn't have week number, so ignore ganss-wk-number i.e. p_GanssGenericAD->GANSS_Almanac.wk_number_incl
              ganss-wk-number is used inconjunction with other ganss satellite system */
            for ( q_index = 0; q_index < p_GanssGenericAD->GANSS_Almanac.sat_cnt; ++q_index )
            {
              /* gloAlmnA (1..24) - sv slot id */
              p_umts_cp_wcdma_session_status->q_glo_alm_delivered_sv_list |=
                                                                             1L << (p_GanssGenericAD->GANSS_Almanac.almanac_set[q_index].gloAlmnA - 1);

              /* Map Glo Sv Id & Channel Frequency Number */
              tm_umts_common_map_glo_eph_svid_to_freq_num_from_alm(p_GanssGenericAD->GANSS_Almanac.almanac_set[q_index].gloAlmnA,
                                                                   p_GanssGenericAD->GANSS_Almanac.almanac_set[q_index].gloAlmHA,
                                                                   p_umts_cp_wcdma_session_status->z_prot_glo_eph_buf);

              tm_rrc_glo_alm_to_tm_post_data_payload(&p_GanssGenericAD->GANSS_Almanac.almanac_set[q_index],
                                                     &z_tm_post_data_payload.alm_glo_data);

              if ( FALSE == tm_post_data(TM_PRTL_TYPE_UMTS_CP,
                                         p_umts_cp_wcdma_session_status->tm_active_session_handle,
                                         TM_POST_DATA_TYPE_ALM_GLO,
                                         &z_tm_post_data_payload
                                        ) )
              {
                MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "RRC GLO Alm. to Tmcore data post failed");
              }
              else
              {
                /* gloAlmnA is (1..24) - SV slot id */
                q_alm_svs_injected |= 1L << (p_GanssGenericAD->GANSS_Almanac.almanac_set[q_index].gloAlmnA - 1);
              }
            } /* For each alm. */

            MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "RRC GLO total %d Alm. posted to TmCore. 0x%lX",
                  p_GanssGenericAD->GANSS_Almanac.sat_cnt, q_alm_svs_injected);
          }
        }

      } /* GanssId == Glonass */
    } /* Iterate each Generic AD list */

    /*********************** Inject pending GLO AddNavigationModel(Ephermesis) ******************/

    if ( p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_BASED )
    {
      u_num_glo_eph_injected = 0;
      q_glo_eph_injected_sv_list = 0;

      /* See if any Glonass eph. SVs are left to be injected, which has freq Num/SV-ID mapping. */
      for ( u_i = 0; u_i < N_GLO_SV; ++u_i )
      {
        if ( (p_umts_cp_wcdma_session_status->z_prot_glo_eph_buf[u_i].z_eph_glo_data.u_SvId > GLO_SLOTID_OFFSET) &&
             (p_umts_cp_wcdma_session_status->z_prot_glo_eph_buf[u_i].z_eph_glo_data.u_SvId < (N_GLO_SV + GLO_SLOTID_OFFSET + 1)) &&
             (p_umts_cp_wcdma_session_status->z_prot_glo_eph_buf[u_i].u_freq_num_valid == TRUE)
            )
        {
          if ( TRUE == cp_wcdma_rrc_inject_glo_eph_to_tmcore(p_umts_cp_wcdma_session_status->tm_active_session_handle,
                                                             &p_umts_cp_wcdma_session_status->z_prot_glo_eph_buf[u_i],
                                                             &z_tm_post_data_payload,
                                                             &q_glo_eph_injected_sv_list) )
          {
            /* Clean-up the injected Glonass Eph. SV, leaving the FreqNum-to-SlotId mapping intact.
                   *  Since FreqNum-to-SlotId mapping is valid for atleast the time duration of Glo Almanac */
            cp_wcdma_retain_slot_id_freq_map_only(&p_umts_cp_wcdma_session_status->z_prot_glo_eph_buf[u_i]);

            ++u_num_glo_eph_injected;
          }
        }
      } /* For loop */

      if ( u_num_glo_eph_injected != 0 )
      {
        MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED,
              "Pending %3u (0x%lX) RRC Glo NavModel posted to TmCore", u_num_glo_eph_injected, q_glo_eph_injected_sv_list);
      }
    }

  } /* Generic AD included */

  return;
}


/*
 ******************************************************************************
 *
 * cp_wcdma_retain_slot_id_freq_map_only
 *
 * Function description:
 *  This function clean-up the injected Glonass Eph. SV and leave the FreqNum-to-SlotId mapping intact.
 *  The FreqNum-to-SlotId mapping is valid for atleast the time duration of Glo Almanac
 *
 * Parameters:
 *
 *  p_ProtGloEphBuf: pointer to Glonass Eph. buffer where the FreqNum-to-SlotId mapping is stored.
 *
 ******************************************************************************
*/

void cp_wcdma_retain_slot_id_freq_map_only( prot_glo_eph_buffer_s_type *const p_ProtGloEphBuf )
{
  uint8 u_TmpFreqNum = p_ProtGloEphBuf->z_eph_glo_data.u_FreqNum;

  memset(p_ProtGloEphBuf, 0, sizeof(prot_glo_eph_buffer_s_type));
  p_ProtGloEphBuf->z_eph_glo_data.u_FreqNum = u_TmpFreqNum;
  p_ProtGloEphBuf->u_freq_num_valid = TRUE;
}

#endif /* FEATURE_GANSS_SUPPORT */

/********************************************************************
*
* cp_wcdma_action_upon_ni_notif_verif_accepted
*
* Function description:
*   actions to be taken once an NI request SUPL_INIT is accepted
*   by the OEM layer.
*
* Parameters:
*   p_req_data : pointer to a structure holding information from
*                pdapi .
*
* Return:
*  None
*
*********************************************************************
*/

static void cp_wcdma_action_upon_ni_notif_verif_accepted( void )
{
  umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status = &z_umts_cp_wcdma_session_status;


#ifdef FEATURE_CONTINUE_MTLR_SESSION

  /* Flag will be cleared implicitly at session end when structure is zeroed out. */
  p_umts_cp_wcdma_session_status->u_mtlr_in_progress = TRUE;
  MSG_HIGH("CP-WCDMA: MT-LR sess started", 0, 0, 0);
#endif

  /* Note: this "allow_rrc" is a special feature. For regular call flow scenarios,
     the related NV item should be set to TRUE */
  if ( p_umts_cp_wcdma_session_status->allow_rrc == FALSE )
  {
    tm_umts_cp_wcdma_set_cf_state(C_UMTS_CP_WCDMA_CF_STATE_MT_GUARD_TIME, FALSE);

    /* If the default "allow_rrc" is "not allow" then notify NAS that
       a positioning session has started. This will allow TM to receive RRC connection release
       notifications in case a connection release occurs before the first Measurement Control.
       If no MC is received by the time the MTLR guard timer expires NAS will be notified
       that the positioning session has ended. If the default is "allow" then TM will instead
       notify NAS that the session has started when the first MC is received */
    #ifdef FEATURE_CGPS_DUAL_WCDMA
    tm_umts_common_utils_mm_session_status_notify(TRUE, z_umts_cp_wcdma_session_status.z_rrc_as_id_info.e_cached_rrc_as_id);
	#else
	tm_umts_common_utils_mm_session_status_notify(TRUE);
	#endif
  }

}

/********************************************************************
*
* cp_wcdma_action_upon_ni_notif_verif_denied
*
* Function description:
*   actions to be taken once an NI request is denied
*   by the OEM layer.
*
* Parameters:
*
* Return:
*  None
*
*********************************************************************
*/

static void cp_wcdma_action_upon_ni_notif_verif_denied( void )
{
#if 0
  if( ( p_zSessionStatus->u_CollectInitiator == C_COLLECT_INITIATOR_NULL ) &&
     ( p_zSessionStatus->u_ThirdPartyMOLRInProgress == FALSE ) &&
     ( p_SessionConfig->u_periodic_app_tracking_on == FALSE )
     )
  {
    p_SessionConfig->u_allow_rrc = p_LsmNvItems->aagps_default_allow_rrc;
  }
#endif
}


#ifdef FEATURE_EXTENDED_RESP_TIME
/********************************************************************
*
* tm_umts_cp_wcdma_extended_reporting_interval
*
* Function description:
*
*   This is a sample function which is to be replaced by an OEM
*   provided one. OEM can use its equivalent function to provide
*   the value of its intended extended response time for network
*   initiated call flows, and the addtional requestion margin for
*   both NI and UI call flows.
*
* Parameters:
*
*   p_extended_rep_time: pointer to extended NI response time to
*     be provided by this function.
*
*   p_additional_req_margin_sec: pointer to additional_req_margin
*     which is the number of seconds before the expiration of RRC
*     measurement control's ReportingIntervalLong when the UE would
*     send request for IONO in order to buy more time.
*
* Return:
*  None
*
* Note:
*
*  This sample function uses the NV item of aagps_default_qos_time
*  for the NI extended response time, up to the maximum; and hard
*  codes additional request margin to 4 seconds.
*********************************************************************
*/

void tm_umts_cp_wcdma_extended_reporting_interval
( uint8 *p_extended_ni_rep_time, uint32 *p_additional_req_margin_sec )
{
  if ( z_umts_nv_item_ram_copy.aagps_default_qos_time < TM_MAX_SESSION_TIMEOUT )
  {
    *p_extended_ni_rep_time = z_umts_nv_item_ram_copy.aagps_default_qos_time;
  }
  else
  {
    *p_extended_ni_rep_time = TM_MAX_SESSION_TIMEOUT;
  }

  *p_additional_req_margin_sec = 4;
}

/********************************************************************
*
* tm_umts_cp_wcdma_req_extend_resp_time
*
* Function description:
*   Construct and send RRC msrReportError asking for IONO in order
*   to buy more time from the network.
*
* Parameters:
*   None.
*
* Return:
*  None.
*
*********************************************************************
*/

void tm_umts_cp_wcdma_req_extend_resp_time( void )
{
  sm_InternalAssistDataStatusStructType z_assist_data_status;

  memset(&z_assist_data_status, 0, sizeof(sm_InternalAssistDataStatusStructType));

  z_assist_data_status.q_WishListMask = C_GPS_ASSIS_IONO;

  /* send msrReportError asking for IONO to buy more time */
  tm_umts_cp_wcdma_encode_rrc_msrReport_err
     (RRC_ASSISTANCE_DATA_MISSING, &z_assist_data_status);

}

#endif


/*===========================================================================

FUNCTION tm_umts_cp_wcdma_start_max_mgp_on_timer

DESCRIPTION
  This function starts the max_MGP_ON timer

DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/
void tm_umts_cp_wcdma_start_max_mgp_on_timer( uint32 q_time_ms )
{
  (void)os_TimerStart(umts_cp_wcdma_max_mgp_on_timer, q_time_ms, NULL);

  MSG_MED("CP-WCDMA: Start max. MGP_ON timer; t=%d", q_time_ms, 0, 0);
}

/*===========================================================================

FUNCTION tm_umts_cp_wcdma_cancel_max_mgp_on_timer

DESCRIPTION
  This function cancels the max. MGP_ON timer

DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/
void tm_umts_cp_wcdma_cancel_max_mgp_on_timer( void )
{
  if ( os_TimerCheck(umts_cp_wcdma_max_mgp_on_timer) )
  {
    (void)os_TimerStop(umts_cp_wcdma_max_mgp_on_timer);
    MSG_MED("CP-WCDMA: Cancel max. MGP_ON timer", 0, 0, 0);
  }
}


/*===========================================================================
FUNCTION tm_umts_cp_wcdma_post_non_exist_gnss_svs

DESCRIPTION
  Post/inject the GPS & GLONASS non-exist SVs (not to confuse with
  GLONASS non-exist frequency).

PARAMETERS
  p_umts_cp_wcdma_session[IN] : CP WCDMA session pointer

RETURN VALUE : none

SIDE EFFECTS : none

===========================================================================*/

void tm_umts_cp_wcdma_post_non_exist_gnss_svs( umts_cp_wcdma_session_status_s_type const *p_umts_cp_wcdma_session )
{
  tm_util_post_non_exist_svs(TM_PRTL_TYPE_UMTS_CP,
                             p_umts_cp_wcdma_session->tm_active_session_handle,
                             p_umts_cp_wcdma_session->q_alm_delivered_sv_list,
                             p_umts_cp_wcdma_session->q_eph_delivered_sv_list);

#ifdef FEATURE_GANSS_SUPPORT
  tm_util_post_non_exist_glo_svs(TM_PRTL_TYPE_UMTS_CP,
                                 p_umts_cp_wcdma_session->tm_active_session_handle,
                                 p_umts_cp_wcdma_session->q_glo_alm_delivered_sv_list,
                                 p_umts_cp_wcdma_session->q_glo_eph_delivered_sv_list);
#endif
}

/*===========================================================================

FUNCTION tm_umts_cp_wcdma_handle_multi_periodic_rpt

DESCRIPTION Send multiple perioid report to network & when specified num of measurement
            is sent-out, close the session.

DEPENDENCIES
  None.

RETURN VALUE
  none.

SIDE EFFECTS
  None.

===========================================================================*/
void tm_umts_cp_wcdma_handle_multi_periodic_rpt( umts_cp_wcdma_session_status_s_type *const p_umts_cp_wcdma_session_status,
                                                 sm_GpsMeasRptStructType *const p_prm_data, boolean const status_check_needed,
                                                 boolean const b_first_done_meas )
{

  /* if ready to report */
  if ( p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs != 0 )
  {
    if ( (p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining > 0) &&
         (p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining <= p_umts_cp_wcdma_session_status->w_cp_periodic_reports_total_num) )
    {

      /* Use the measurements starting from q_cp_periodic_reporting_interval_sec second, but don't send until
      *  q_cp_periodic_reporting_interval_sec second is completed. The cp_wcdma_send_rrc_msrReport_meas() will send measurements only
      *  at the end of each epoch.
      */
      if ( (p_umts_cp_wcdma_session_status->w_lm_report_count >= p_umts_cp_wcdma_session_status->q_cp_periodic_reporting_interval_sec) ||
           (TRUE == b_first_done_meas) )
      {
        MSG_5(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Process meas: total=%u, remain=%u, lm_rpt_cnt=%u, first_done_meas=%3u, interval=%lu",
              p_umts_cp_wcdma_session_status->w_cp_periodic_reports_total_num,
              p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining,
              p_umts_cp_wcdma_session_status->w_lm_report_count, b_first_done_meas, p_umts_cp_wcdma_session_status->q_cp_periodic_reporting_interval_sec);

        /* propagate & send the measurement report */
        if ( cp_wcdma_send_rrc_msrReport_meas(p_prm_data, status_check_needed) )
        {
          /* clear timer and call flow state. Note if the above function call returns FALSE,
             don't stop this state, just keep processing incoming measurements until timeout
             at which time UEP-Error will be sent */
          tm_umts_cp_wcdma_terminate_timer_cancel_cf_state(C_UMTS_CP_WCDMA_CF_STATE_TIMER_RRC_REPORT);

          p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining--;

          /* reset lm_report_count */
          p_umts_cp_wcdma_session_status->w_lm_report_count = 0;
        } /* report sent ok */
      }   /* time to report */
      else
      {
        MSG_5(MSG_SSID_GPSSM, MSG_LEGACY_MED, "skipping meas: total=%u, remain=%u, lm_rpt_cnt=%u, first_done_meas=%3u, interval=%lu",
              p_umts_cp_wcdma_session_status->w_cp_periodic_reports_total_num,
              p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining,
              p_umts_cp_wcdma_session_status->w_lm_report_count, b_first_done_meas,
              p_umts_cp_wcdma_session_status->q_cp_periodic_reporting_interval_sec);

      }

      /* Count the finished seconds */
      if ( p_prm_data->z_MeasBlk.u_SeqNum == p_prm_data->z_MeasBlk.u_MaxMessageNum )
      {
        p_umts_cp_wcdma_session_status->w_lm_report_count++;
      }

      /* if done: close session, else keep waiting */
      if ( p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining == 0 )
      {
        tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_COMPLETED);
      }

    } /* still having remaining reports to do */
    else
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "RRC: All periodic MeasRpt sent");
    }
  } /* in a state for reporting */

}

/*===========================================================================

FUNCTION cp_wcdma_mcm_rpt_quantity_modified

DESCRIPTION
  Verify Measurement Control Meassage (MCM) has modified reporting quantity which is valid.
  If the modified reporting quantity is valid, then update the position method & position
  method type for the current session.

DEPENDENCIES
  This function won't check for invalid position method. Call cp_wcdma_mcm_rpt_position_method_invalid()
  function to check for invalid position method.

RETURN VALUE
  none.

SIDE EFFECTS
  None.

===========================================================================*/
boolean cp_wcdma_mcm_rpt_quantity_modified( umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status,
                                            rrc_meas_ctrl_struct_type const *const p_rrc_meas_ctrl )
{
  uint8 b_ModifiedPosMode = eWCDMA_POSITION_MODE_AGNSS_UNDEF;

  /* Check if MCM is modified */
  if ( (p_rrc_meas_ctrl->rpt_quan.method_type == RRC_UE_ASSISTED) ||
       (p_rrc_meas_ctrl->rpt_quan.method_type == RRC_UE_ASSISTED_PREFERED) )
  {
    b_ModifiedPosMode = WCDMA_POSITION_METHOD_TYPE_UE_ASSISTED;
  }
  else if ( (p_rrc_meas_ctrl->rpt_quan.method_type == RRC_UE_BASED) ||
            (p_rrc_meas_ctrl->rpt_quan.method_type == RRC_UE_BASED_PREFERED)
           )
  {
    b_ModifiedPosMode = WCDMA_POSITION_METHOD_TYPE_UE_BASED;
  }


#ifdef FEATURE_GANSS_SUPPORT
  /* Is GPS & GLONASS position method selected?. For GPS-only, gANSSPositioningMethods IE shouldn't be present. */
  if ( TRUE == cp_wcdma_ganss_pos_method_is_agnss(p_rrc_meas_ctrl) )
  {
    b_ModifiedPosMode |= (WCDMA_POSITION_METHOD_A_GPS | WCDMA_POSITION_METHOD_A_GLO);
  }
#endif

  if ( cp_wcdma_pos_method_is_gps(p_rrc_meas_ctrl) )
  {
    b_ModifiedPosMode |= WCDMA_POSITION_METHOD_A_GPS;
  }

  /* Position method and/or type is modified in MCM */
  if ( b_ModifiedPosMode != p_umts_cp_wcdma_session_status->e_gnssPosMode )
  {
    return ( TRUE );
  }

  return ( FALSE ); /* MCM Position method & type not modified */
}

/*===========================================================================

FUNCTION cp_wcdma_accept_modified_mcm_rpt_quantity

DESCRIPTION
  Verify Measurement Control Meassage (MCM) has modified reporting quantity. If the modified
  reporting quantity is valid, then update the position method & position method type for the
  current session.

DEPENDENCIES
  This function won't check for invalid position method. Call cp_wcdma_mcm_rpt_position_method_invalid()
  function to check for invalid position method.

RETURN VALUE
  none.

SIDE EFFECTS
  None.

===========================================================================*/
boolean cp_wcdma_accept_modified_mcm_rpt_quantity( umts_cp_wcdma_session_status_s_type *p_umts_cp_wcdma_session_status,
                                                   rrc_meas_ctrl_struct_type const *const p_rrc_meas_ctrl )
{
  uint8 b_realizedPositionMode = p_umts_cp_wcdma_session_status->e_gnssPosMode;

  /*** Check modified position method (GPS, GLO) is valid ***/
#ifdef FEATURE_GANSS_SUPPORT

  if ( ((p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED) ||
        (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_BASED)) &&
       (TRUE == cp_wcdma_ganss_pos_method_is_agnss(p_rrc_meas_ctrl))
      )
  {
    /* Acceptable position method from GPS to GNSS (GPS + GLO) */
    b_realizedPositionMode &= (~WCDMA_POSITION_METHOD_BMASK); /* Set position method bits to 0 */
    b_realizedPositionMode |= (WCDMA_POSITION_METHOD_A_GPS | WCDMA_POSITION_METHOD_A_GLO);
  }
  else if ( ((p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED) ||
             (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_BASED)) &&
            (cp_wcdma_pos_method_is_gps(p_rrc_meas_ctrl) && (FALSE == cp_wcdma_ganss_pos_method_is_agnss(p_rrc_meas_ctrl)))
           )
  {
    /* Acceptable position method from GNSS (GPS + GLO) to GPS */
    b_realizedPositionMode &= (~WCDMA_POSITION_METHOD_BMASK); /* Set position method bits to 0 */
    b_realizedPositionMode |= WCDMA_POSITION_METHOD_A_GPS;
  }
#endif


  /*** Check modified position method type (UE-A, UE-B) is valid ***/

  /* In case GPS mode request is changed from what was asked for in the first RRC */
  if ( (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_BASED) ||
       (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_BASED) )
  {
    if ( (p_rrc_meas_ctrl->rpt_quan.method_type == RRC_UE_ASSISTED) ||
         (p_rrc_meas_ctrl->rpt_quan.method_type == RRC_UE_ASSISTED_PREFERED) )
    {
      b_realizedPositionMode &= (~WCDMA_POSITION_METHOD_TYPE_BMASK); /* Set position method type bits to 0 */
      b_realizedPositionMode |= WCDMA_POSITION_METHOD_TYPE_UE_ASSISTED;
    }
  }
  else if ( (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED) ||
            (p_umts_cp_wcdma_session_status->e_gnssPosMode == eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED) )
  {
    if ( (p_rrc_meas_ctrl->rpt_quan.method_type == RRC_UE_BASED) ||
         (p_rrc_meas_ctrl->rpt_quan.method_type == RRC_UE_BASED_PREFERED)
        )
    {
      b_realizedPositionMode &= (~WCDMA_POSITION_METHOD_TYPE_BMASK); /* Set position method type bits to 0 */
      b_realizedPositionMode |= WCDMA_POSITION_METHOD_TYPE_UE_BASED;
    }
  }

  if ( b_realizedPositionMode != p_umts_cp_wcdma_session_status->e_gnssPosMode )
  {
    p_umts_cp_wcdma_session_status->e_gnssPosMode = b_realizedPositionMode;
    return ( TRUE );
  }

  return ( FALSE );
}

/*===========================================================================

FUNCTION cp_wcdma_mcm_pos_method_support_by_ue

DESCRIPTION
  Validate the network requested position method type is supported by UE's NV settings.

DEPENDENCIES
  none.

RETURN VALUE
  none.

SIDE EFFECTS
  None.

===========================================================================*/
boolean cp_wcdma_mcm_pos_method_support_by_ue( uint32 const q_NvPositionModeSupported, rrc_positioning_method_type_enum_type const e_RrcMcmPositionMode )
{

#ifdef FEATURE_GANSS_SUPPORT
  if ( ((e_RrcMcmPositionMode == RRC_UE_ASSISTED) &&
        !(q_NvPositionModeSupported & (PDSM_POS_MODE_3G_CP_UE_ASSISTED | PDSM_AGLO_POS_MODE_3G_CP_MS_ASSISTED))) ||
       ((e_RrcMcmPositionMode == RRC_UE_BASED) &&
        !(q_NvPositionModeSupported & (PDSM_POS_MODE_3G_CP_UE_BASED | PDSM_AGLO_POS_MODE_3G_CP_MS_BASED))) ||
       (!(q_NvPositionModeSupported &
             (PDSM_POS_MODE_3G_CP_UE_BASED | PDSM_POS_MODE_3G_CP_UE_ASSISTED | PDSM_AGLO_POS_MODE_3G_CP_MS_BASED | PDSM_AGLO_POS_MODE_3G_CP_MS_ASSISTED)))
      )
  {
    return ( FALSE );
  }
#else
  if ( ((e_RrcMcmPositionMode == RRC_UE_ASSISTED) &&
        !(q_NvPositionModeSupported & PDSM_POS_MODE_3G_CP_UE_ASSISTED)) ||
       ((e_RrcMcmPositionMode == RRC_UE_BASED) &&
        !(q_NvPositionModeSupported & PDSM_POS_MODE_3G_CP_UE_BASED)) ||
       (!(q_NvPositionModeSupported & (PDSM_POS_MODE_3G_CP_UE_BASED | PDSM_POS_MODE_3G_CP_UE_ASSISTED)))
      )
  {
    return ( FALSE );
  }
#endif

  return ( TRUE );
}

/*===========================================================================

FUNCTION cp_wcdma_build_tm_session_msb_ptr

DESCRIPTION
  Helper function to populate the TM-Core session pointer with MSB parameters.

DEPENDENCIES
  none.

RETURN VALUE
  Return the TM-Core session type that should be requested.

SIDE EFFECTS
  None.

===========================================================================*/
tm_sess_req_type cp_wcdma_build_tm_session_msb_ptr( tm_sess_req_param_u_type *const p_req_param, umts_cp_wcdma_session_status_s_type *const p_umts_cp_wcdma_session_status )
{
  tm_sess_req_type req_type = (tm_sess_req_type)TM_SESS_REQ_NONE;

  if ( p_umts_cp_wcdma_session_status->tm_core_session_started == FALSE )
  {
    req_type = TM_SESS_REQ_START;

    /* turn the receiver to ON at this point, with default Qos values that
       should be updated later by the the network */
    p_req_param->start_param.op_mode = TM_SESS_OPERATION_MODE_MSB;
    p_req_param->start_param.op_req = TM_OP_REQ_LR;
    p_req_param->start_param.lr_qos = TM_MAX_SESSION_TIMEOUT;
    p_req_param->start_param.accuracy_threshold = 0;
    p_req_param->start_param.abort_curr_sess = FALSE;
    p_req_param->start_param.num_fixes = 1;
    p_req_param->start_param.tbf_ms = 1000; /* don't care */
    p_req_param->start_param.dynamic_qos_enabled = TRUE;
  }
  else
  {
    req_type = TM_SESS_REQ_CONTINUE;

    p_req_param->continue_param.op_mode = TM_SESS_OPERATION_MODE_MSB;
    p_req_param->continue_param.op_req = TM_OP_REQ_LR;

    p_req_param->continue_param.lr_qos = TM_MAX_SESSION_TIMEOUT;
    p_req_param->continue_param.accuracy_threshold = 0;
    p_req_param->continue_param.dynamic_qos_enabled = TRUE;
  }
  return ( req_type );
}

/*===========================================================================

FUNCTION cp_wcdma_build_tm_session_msa_ptr

DESCRIPTION
  Helper function to populate the TM-Core session pointer with appropiate MSB parameters.

DEPENDENCIES
  none.

RETURN VALUE
  Return the TM-Core session type that should be requested.

SIDE EFFECTS
  None.

===========================================================================*/
tm_sess_req_type cp_wcdma_build_tm_session_msa_ptr( tm_sess_req_param_u_type *const p_req_param, umts_cp_wcdma_session_status_s_type *const p_umts_cp_wcdma_session_status )
{
  tm_sess_req_type req_type = (tm_sess_req_type)TM_SESS_REQ_NONE;

  if ( p_umts_cp_wcdma_session_status->tm_core_session_started == FALSE )
  {
    req_type = TM_SESS_REQ_START;

    p_req_param->start_param.op_mode = TM_SESS_OPERATION_MODE_MSA;
    p_req_param->start_param.op_req = TM_OP_REQ_PRM;
    p_req_param->start_param.prm_qos = TM_MAX_SESSION_TIMEOUT;
    p_req_param->start_param.accuracy_threshold = 0;
    p_req_param->start_param.abort_curr_sess = FALSE;
    p_req_param->start_param.num_fixes = 1;
    p_req_param->start_param.tbf_ms = 1000; /* don't care */
    p_req_param->start_param.dynamic_qos_enabled = TRUE;
    p_req_param->start_param.multi_report_msa = FALSE;

  }
  else
  {
    req_type = TM_SESS_REQ_CONTINUE;

    p_req_param->continue_param.op_mode = TM_SESS_OPERATION_MODE_MSA;
    p_req_param->continue_param.op_req = TM_OP_REQ_PRM;
    p_req_param->continue_param.prm_qos = TM_MAX_SESSION_TIMEOUT;
    p_req_param->continue_param.accuracy_threshold = 0;
    p_req_param->continue_param.dynamic_qos_enabled = TRUE;
    p_req_param->continue_param.multi_report_msa = FALSE;
  }

  /* turn the receiver to idle at this point, with default Qos values that
     should be updated later by the the network */
  if ( (z_rrc_params.rrc_periodic_multi_report == TRUE) &&
       (p_umts_cp_wcdma_session_status->w_cp_periodic_reports_remaining > 1)
      )
  {
    /* Non-zero value of "timeout_value_secs" flags the state in which
       the last RRC measureControl has been processed and hence reporting
       is allowed. If it is still zero at this point, it means the phone
       did not know if the last MCM was the last one as it started checking
       with LM to see if additional request was needed. */
    if ( p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs == 0 )
    {
      if ( p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long != 0 )
      {
        p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs =
                                                                                         p_umts_cp_wcdma_session_status->w_rrc_reporting_interval_long;
      }
      else
      {
        p_umts_cp_wcdma_session_status->z_session_qos.u_gps_session_timeout_value_secs =
                                                                                         z_umts_nv_item_ram_copy.aagps_default_qos_time;
      }
    }

    if ( p_umts_cp_wcdma_session_status->tm_core_session_started == FALSE )
    {
      p_req_param->start_param.multi_report_msa = TRUE; /* MSA multiple report */
      p_req_param->start_param.prm_qos = TM_MULTI_REPT_MAX_SESSION_TIMEOUT;  /* 600 seconds */
    }
    else
    {
      p_req_param->continue_param.multi_report_msa = TRUE; /* MSA multiple report */
      p_req_param->continue_param.prm_qos = TM_MULTI_REPT_MAX_SESSION_TIMEOUT;  /* 600 seconds */
    }

  } /* if multiple periodic */

  if ( tm_umts_common_utils_emergency_effect() != C_EMERGENCY_HANDLING_NULL )
  {
    /* overloading this multi_report_msa that is defined to indicate
       control plane multiple reporting, for the purpose of letting
       MGP to run all the way to timeout without declaring DONE when
       a qualified measurement is reached */
    p_req_param->start_param.multi_report_msa = TRUE;
  }

  return ( req_type );
}

/*===========================================================================

FUNCTION cp_wcdma_allowed_position_mode

DESCRIPTION
  Validate the network provided position mode (method & method type) & return the
  translated position mode for defining the call flow type.

DEPENDENCIES
  none.

RETURN VALUE
  Return the position mode. The position mode could be undefined as well.

SIDE EFFECTS
  None.

===========================================================================*/
wcdma_position_mode_e_type cp_wcdma_allowed_position_mode( rrc_meas_ctrl_struct_type const *const p_rrc_meas_ctrl )
{
  boolean                    b_GanssPosMethod   = FALSE;
  wcdma_position_mode_e_type e_GnssPositionMode = eWCDMA_POSITION_MODE_AGNSS_UNDEF;

#ifdef FEATURE_GANSS_SUPPORT

  /* The presence of ganss_pos_method_incl indicates RRC gANSSPositioningMethods IE sent by network, which overwrites
  the position method in A-GPS positionMethod IE. */
  if ( TRUE == cp_wcdma_glonass_check(p_rrc_meas_ctrl) )
  {
    /* Is GPS & GLONASS position method selected?. For GPS-only, gANSSPositioningMethods IE shouldn't be present. */
    if ( TRUE == cp_wcdma_ganss_pos_method_is_agnss(p_rrc_meas_ctrl) )
    {
      b_GanssPosMethod = TRUE;
    }
    else
    {
      /* Invalid position method should be handled in WCDMA-RRC by sending MCF with configuration incomplete. */
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Invalid GANSS position method 0x%uX", p_rrc_meas_ctrl->rpt_quan.ganss_pos_method);
      return ( eWCDMA_POSITION_MODE_AGNSS_UNDEF );
    }
  }
#endif


  if ( p_rrc_meas_ctrl->rpt_quan.method_type == RRC_UE_ASSISTED )
  {
    if ( b_GanssPosMethod )
    {
      /* GPS + GLO position method requested */
      e_GnssPositionMode = eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED;
    }
    else
    {
      e_GnssPositionMode = eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED;
    }
  }
  else if ( p_rrc_meas_ctrl->rpt_quan.method_type == RRC_UE_BASED )
  {
    if ( b_GanssPosMethod )
    {
      /* GPS + GLO position method requested */
      e_GnssPositionMode = eWCDMA_POSITION_MODE_A_GPS_GLO_UE_BASED;
    }
    else
    {
      e_GnssPositionMode = eWCDMA_POSITION_MODE_AGPS_UE_BASED;
    }
  }
  else if ( p_rrc_meas_ctrl->rpt_quan.method_type == RRC_UE_ASSISTED_PREFERED )
  {
    if ( b_GanssPosMethod )
    {
      /* GPS + GLO position method requested */
      if ( !TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_AGLO_POS_MODE_3G_CP_MS_ASSISTED) )
      {
        /* if UE-Assisted is not supported and it got this far, UE-Based must be supported */
        e_GnssPositionMode = eWCDMA_POSITION_MODE_A_GPS_GLO_UE_BASED;
      }
      else
      {
        /* if UE-Assisted is supported, treat UE-AssistedPreferred as UE-Assisted */
        e_GnssPositionMode = eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED;
      }
    }
    else
    {
      if ( !TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_3G_CP_UE_ASSISTED) )
      {
        /* if UE-Assisted is not supported and it got this far, UE-Based must be supported */
        e_GnssPositionMode = eWCDMA_POSITION_MODE_AGPS_UE_BASED;
      }
      else
      {
        /* if UE-Assisted is supported, treat UE-AssistedPreferred as UE-Assisted */
        e_GnssPositionMode = eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED;
      }
    }
  }
  else if ( p_rrc_meas_ctrl->rpt_quan.method_type == RRC_UE_BASED_PREFERED )
  {
    if ( b_GanssPosMethod )
    {
      /* GPS + GLO position method requested */
      if ( !TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_AGLO_POS_MODE_3G_CP_MS_BASED) )
      {
        /* if UE-Based is not supported and it got this far, UE-Assisted must be supported */
        e_GnssPositionMode = eWCDMA_POSITION_MODE_A_GPS_GLO_UE_ASSISTED;
      }
      else
      {
        /* if UE-Based is supported, treat UE-BasedPreferred as UE-Based */
        e_GnssPositionMode = eWCDMA_POSITION_MODE_A_GPS_GLO_UE_BASED;
      }
    }
    else
    {
      if ( !TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_3G_CP_UE_BASED) )
      {
        /* if UE-Based is not supported and it got this far, UE-Assisted must be supported */
        e_GnssPositionMode = eWCDMA_POSITION_MODE_AGPS_UE_ASSISTED;
      }
      else
      {
        /* if UE-Based is supported, treat UE-BasedPreferred as UE-Based */
        e_GnssPositionMode = eWCDMA_POSITION_MODE_AGPS_UE_BASED;
      }
    }
  }

  return ( e_GnssPositionMode );
}

/*
 ******************************************************************************
 *  tm_umts_cp_wcdma_end_session_after_cache_meas_proc
 *
 * Function description:
 *   Regardless of whether it succeeds in sending this RRC MRM from cached GNSS measurement,
 *   this should end the RRC session.
 *   In A-GNSS session this function should be called once after sending GPS + GLO measurments.
 *
 * Parameters:
 *    None.
 *
 * Return value:
 *    None.
 *
 ******************************************************************************
*/
void tm_umts_cp_wcdma_end_session_after_cache_meas_proc( void )
{
  /* GPS position method is only supported for UI (MT-LR & MO-LR) sessions. So use GPS measurement block to get the GPS time */
  sm_GpsMeasRptStructType *p_prm_data = &z_umts_cp_wcdma_session_status.z_cached_gnss_meas.z_meas_reports[SM_GNSS_MEAS_SYSTEM_GPS];

  /* clear timer and call flow state. Note if the above function call returns
     FALSE, don't stop this state, just keep processing incoming measurements
     until timeout at which time UEP-Error will be sent */
  tm_umts_cp_wcdma_terminate_timer_cancel_cf_state(C_UMTS_CP_WCDMA_CF_STATE_TIMER_RRC_REPORT);

  /* GPS Complete event */
  sm_log_event_report(SM_LOG_EVENT_ID_GPS_COMPLETE, 0);

  if ( z_umts_cp_wcdma_session_status.u_callflow_kind == C_UMTS_CP_WCDMA_SESSION_UI )
  {
    /* a workaround to time stamp position to be sent from the server in
       SS MoLrReturnResult: use the reference time's week number and the
       measurement report's msec. */
    if ( z_umts_cp_wcdma_session_status.z_set_assist_pos_time_stamp.m.refTimeWeekNumValid )
    {
      z_umts_cp_wcdma_session_status.z_set_assist_pos_time_stamp.t_pos_time_stamp_ms =
          tm_umts_common_utils_full_gps_ms(
              z_umts_cp_wcdma_session_status.z_set_assist_pos_time_stamp.w_ref_time_week_num,
              p_prm_data->z_MeasBlk.z_MeasClock.z_Time.z_GpsTime.q_GpsMsec);

      z_umts_cp_wcdma_session_status.z_set_assist_pos_time_stamp.m.posTimeStampValid = TRUE;
    }

    if ( z_rrc_params.z_saved_meas.u_Saved == FALSE )
    {
      /* start waiting for MoLrReturnResult */
      tm_umts_cp_wcdma_set_cf_state(C_UMTS_CP_WCDMA_CF_STATE_WAIT_RETURN_RESULT, FALSE);
    }

  } /* if UI */
  else
  {
    if ( z_rrc_params.z_saved_meas.u_Saved == FALSE )
    {
      /* tell TmCore to end and send DONE event to pdapi, also clear
         UMTS-CP-WCDMA and UMTS-CP-SS */
      tm_umts_cp_wcdma_inform_core_stop_session(TM_STOP_REASON_COMPLETED);
    }
  }
}

/********************************************************************
*
* tm_umts_cp_wcdma_rrc_send_cached_gnss_meas_and_end_session
*
* Function description:
*   Send RRC measurementReport with cached GPS and/or GLONASS measurement if available
*   & end the session.
*
* Parameters:
*  None
*
* Return:
*  True if atleast (GPS and/or GLO) measurement is sent out, else return false.
*
*********************************************************************
*/

boolean tm_umts_cp_wcdma_rrc_send_cached_gnss_meas_and_end_session( void )
{
  boolean                 b_GpsMeasStatus  = FALSE,
                          b_GloMeasStatus  = FALSE,
                          b_BdsMeasStatus  = FALSE,
                          b_GalMeasStatus  = FALSE,
                          b_QzssMeasStatus = FALSE;
  sm_GpsMeasRptStructType *p_prm_data      = NULL;

  if ( z_umts_cp_wcdma_session_status.z_cached_gnss_meas.u_meas_reports_bmap & SM_GNSS_MEAS_REPORTS_BMAP_GPS )
  {
    p_prm_data = &z_umts_cp_wcdma_session_status.z_cached_gnss_meas.z_meas_reports[SM_GNSS_MEAS_SYSTEM_GPS];
    z_umts_cp_wcdma_session_status.z_cached_gnss_meas.u_meas_reports_bmap &= (~SM_GNSS_MEAS_REPORTS_BMAP_GPS); /* False */

    /* propagate each SV's & send MRM if all measurements in the epoch is received. */
    /* cp_wcdma_send_rrc_msrReport_meas() return true if GPS measurement is sent. False indicates either GPS measurement
    * not sent due to impending GLO/BDS measurement (or) failure in processing/sending GPS meas. */
    b_GpsMeasStatus = cp_wcdma_send_rrc_msrReport_meas( p_prm_data, TRUE );
  }

  if ( z_umts_cp_wcdma_session_status.z_cached_gnss_meas.u_meas_reports_bmap & SM_GNSS_MEAS_REPORTS_BMAP_GLONASS )
  {
    p_prm_data = &z_umts_cp_wcdma_session_status.z_cached_gnss_meas.z_meas_reports[SM_GNSS_MEAS_SYSTEM_GLONASS];
    z_umts_cp_wcdma_session_status.z_cached_gnss_meas.u_meas_reports_bmap &= (~SM_GNSS_MEAS_REPORTS_BMAP_GLONASS); /* False */

    /* propagate each SV's & send MRM if all measurements in the epoch is received. */
    /* cp_wcdma_send_rrc_msrReport_meas() return true if GPS (and/or) GLO measurement is sent. False indicates either GLO measurement
    * not sent due to impending GPS/BDS measurement (or) failure in processing/sending GLO meas. */
    b_GloMeasStatus = cp_wcdma_send_rrc_msrReport_meas( p_prm_data, TRUE );
  }

  if ( z_umts_cp_wcdma_session_status.z_cached_gnss_meas.u_meas_reports_bmap & SM_GNSS_MEAS_REPORTS_BMAP_BDS )
  {
    p_prm_data = &z_umts_cp_wcdma_session_status.z_cached_gnss_meas.z_meas_reports[SM_GNSS_MEAS_SYSTEM_BDS];
    z_umts_cp_wcdma_session_status.z_cached_gnss_meas.u_meas_reports_bmap &= (~SM_GNSS_MEAS_REPORTS_BMAP_BDS); /* False */

    /* The max message & sequence field in BDS is required to send meas report correctly. Its also required to count each epoch.
     *  Note the BDS measurement itself isn't processed & supported by this module
     */
    b_BdsMeasStatus = cp_wcdma_send_rrc_msrReport_meas( p_prm_data, TRUE );
  }

  if ( z_umts_cp_wcdma_session_status.z_cached_gnss_meas.u_meas_reports_bmap & SM_GNSS_MEAS_REPORTS_BMAP_GAL )
  {
    p_prm_data = &z_umts_cp_wcdma_session_status.z_cached_gnss_meas.z_meas_reports[SM_GNSS_MEAS_SYSTEM_GAL];
    z_umts_cp_wcdma_session_status.z_cached_gnss_meas.u_meas_reports_bmap &= (~SM_GNSS_MEAS_REPORTS_BMAP_GAL);

    /* The max message & sequence field in BDS is required to send meas report correctly. Its also required to count each epoch.
     *  Note the GAL measurement itself isn't processed & supported by this module
     */
    b_GalMeasStatus = cp_wcdma_send_rrc_msrReport_meas( p_prm_data, TRUE );
  }

  if ( z_umts_cp_wcdma_session_status.z_cached_gnss_meas.u_meas_reports_bmap & SM_GNSS_MEAS_REPORTS_BMAP_QZSS )
  {
    p_prm_data = &z_umts_cp_wcdma_session_status.z_cached_gnss_meas.z_meas_reports[SM_GNSS_MEAS_SYSTEM_QZSS];
    z_umts_cp_wcdma_session_status.z_cached_gnss_meas.u_meas_reports_bmap &= (~SM_GNSS_MEAS_REPORTS_BMAP_QZSS);

    /* The max message & sequence field in BDS is required to send meas report correctly. Its also required to count each epoch.
     *  Note the QZSS/SBAS measurement itself isn't processed & supported by this module
     */
    b_QzssMeasStatus = cp_wcdma_send_rrc_msrReport_meas( p_prm_data, TRUE );
  }
  MSG_5( MSG_SSID_GPSSM, MSG_LEGACY_MED, "Cached meas report sent %d/%d/%d", b_GpsMeasStatus, b_GloMeasStatus,
         b_BdsMeasStatus, b_GalMeasStatus, b_QzssMeasStatus );

  /* end the session */
  tm_umts_cp_wcdma_end_session_after_cache_meas_proc();

  /* In A-GPS CF b_GpsMeasSent should be true. GPS + GLO call flow b_GloMeasStatus should be true & in A-GNSS (GPS,GLO,BDS) call flow b_BdsMeasStatus is true */
  return ( b_GpsMeasStatus | b_GloMeasStatus | b_BdsMeasStatus | b_GalMeasStatus | b_QzssMeasStatus );
}

#ifdef FEATURE_CGPS_DUAL_WCDMA
/*===========================================================================

FUNCTION tm_umts_cp_wcdma_validate_as_id

DESCRIPTION
  This function validates as_id for RRC events to be sure that the as_id matches with the ongoing session's as_id
  
DEPENDENCIES: none

RETURN VALUE: TRUE: If as_id matches with ongoing session's as_id
			   FALSE: If as_id does not match with ongoing session's as_id

SIDE EFFECTS: none

===========================================================================*/
static boolean tm_umts_cp_wcdma_validate_as_id(sys_modem_as_id_e_type rrc_event_as_id)
{
  boolean retValue = FALSE;

  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "CP-WCDMA: tm_umts_cp_wcdma_validate_as_id() called with as_id %u", (uint8)rrc_event_as_id);
  
  /* Confirm if the new event is related to an ongoing session */
  if (z_umts_cp_wcdma_session_status.tm_core_session_started)
  {
    if (!z_umts_cp_wcdma_session_status.z_rrc_as_id_info.u_rrc_event_as_id_received)
    {
      /* This is the first event from RRC with as_id. Cache the as_id, so that we can use it for rest of the session */
	  z_umts_cp_wcdma_session_status.z_rrc_as_id_info.e_cached_rrc_as_id = rrc_event_as_id;
      z_umts_cp_wcdma_session_status.z_rrc_as_id_info.u_rrc_event_as_id_received = TRUE;
	  MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "CP-WCDMA:First RRC event with as_id. AS_ID cached %u", (uint8)rrc_event_as_id);
      retValue = TRUE;
    }
	else if (rrc_event_as_id != z_umts_cp_wcdma_session_status.z_rrc_as_id_info.e_cached_rrc_as_id)
    {
      /* Events for new session on a different sub, do not handle them */	  
	  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "CP-WCDMA: Dropping event for as_id %u, ongoing session's as_id is %u", (uint8)rrc_event_as_id, 
	  			(uint8)z_umts_cp_wcdma_session_status.z_rrc_as_id_info.e_cached_rrc_as_id);
	  retValue = FALSE;
    }
	else
	{
	  /* Received event for ongoing session */
	  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "CP-WCDMA:Received event for ongoing CP-WCDMA session and cached as_id matches %u", (uint8)rrc_event_as_id);
	  retValue = TRUE;
	}
  }
  else /* New session on-going, let it through*/
  {
	/* Cache the as_id, so that we can use it for rest of the session */
    z_umts_cp_wcdma_session_status.z_rrc_as_id_info.e_cached_rrc_as_id = rrc_event_as_id;
    z_umts_cp_wcdma_session_status.z_rrc_as_id_info.u_rrc_event_as_id_received = TRUE;
	MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "CP-WCDMA: New session. AS_ID cached to %u", (uint8)rrc_event_as_id);
    retValue = TRUE;
  }
  
  return retValue;
}
#endif


#endif  /* FEATURE_CGPS_UMTS_CP_WCDMA */
