/*****************************************************************************
***
*** TITLE  gllcmsg.c
***
***  GPRS LLC Interlayer Messaging Transmission Functions.
***
***
*** DESCRIPTION
***
***  These functions collate all pertinent information necessary to compose
***  the associated message, control the LLC event masking schemes where
***  appropriate, and initiate the process of sending (or resending in some
***  cases) the message to it's appropriate destination.
***
***
*** EXTERNALIZED FUNCTIONS
***
***  gllc_lle_send_llgmm_status_ind_mes()
***  gllc_llme_send_llgmm_trigger_ind_mes()
***  gllc_llme_send_llgmm_rdy_tmr_ind_mes()
***  gllc_llme_send_llgmm_suspend_cnf_mes()
***  gllc_llme_send_llgmm_new_tlli_ind_mes()
***
***  gllc_lle_send_ll_rst_ind_mes()
***  gllc_lle_send_ll_est_ind_mes()
***  gllc_lle_send_ll_est_cnf_mes()
***  gllc_lle_send_ll_rel_ind_mes()
***  gllc_lle_send_ll_rel_cnf_mes()
***  gllc_lle_send_ll_xid_ind_mes()
***  gllc_lle_send_ll_xid_cnf_mes()
***  gllc_lle_send_ll_data_ind_mes()
***  gllc_lle_send_ll_data_cnf_mes()
***  gllc_lle_send_ll_unitdata_ind_mes()
***  gllc_lle_send_ll_status_ind_mes()
***
***  gllc_lle_send_sabm_cmd_mes()
***  gllc_lle_resend_sabm_cmd_mes()
***  gllc_lle_send_disc_cmd_mes()
***  gllc_lle_resend_disc_cmd_mes()
***  gllc_lle_send_xid_cmd_mes()
***  gllc_lle_resend_xid_cmd_mes()
***  gllc_lle_send_null_cmd_mes()
***  gllc_lle_send_ua_res_mes()
***  gllc_lle_send_dm_res_mes()
***  gllc_lle_send_xid_res_mes()
***  gllc_lle_send_frmr_res_mes()
***
***  gllc_llme_send_grr_suspend_req_mes()
***  gllc_llme_send_grr_resume_req_mes()
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  None.
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gllc/src/gllcmsg.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10/12/13   ra      CR563511
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

#include "comdef.h"
#include "gmutex.h"
#include "cfa.h"
#include "gs.h"
#include "ms.h"
#include "gllci.h"
#include "gprs_mem.h"
#include "gprs_pdu.h"
#include "gllsap.h"
#include "gmmllcif.h"
#include "mmtask.h"
#include "mmtask_v.h"
#include "geran_msgs.h"
#include "string.h"
#include "stringl.h"
#include "com_iei.h"
#include "geran_tasks.h"
#include "gsndcp_v_g.h"

#if defined TEST_FRAMEWORK
#error code not present
#endif 

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Macros
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LLGMM_STATUS_IND_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required GMM
===   message and posts this message to the GMM entity.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_send_llgmm_status_ind_mes
(
  gas_id_t                    gas_id,
  gllc_ll_sapi_t              lle,
  uint32                      tlli,
  gllcmm_status_cause_e_type  cause
)
{
  mm_cmd_type *llgmm_dl_mes;

  llgmm_dl_mes = mm_gllc_get_cmd_buf( (uint8)GLLCMM_LLGMM_STATUS_IND );

  if ( llgmm_dl_mes != NULL )
  {
    switch ( cause )
    {
    case GLLCMM_NO_PEER_RESPONSE:
      MSG_GERAN_HIGH_1_G( "GLC LL %d: Sent LLGMM-STAT-IND No Peer",
        gllc_sapi_addr_lookup[lle]);
      break;

    case GLLCMM_INVALID_XID_RESP:
      MSG_GERAN_HIGH_1_G( "GLC LL %d: Sent LLGMM-STAT-IND Invld XID",
        gllc_sapi_addr_lookup[lle]);
      break;

    case GLLCMM_MULTIPLE_TLLI_USERS:
      MSG_GERAN_HIGH_1_G( "GLC LL %d: Sent LLGMM-STAT-IND Multi TLLI",
        gllc_sapi_addr_lookup[lle]);
      break;

    case GLLCMM_FRAME_REJECT_COND:
      MSG_GERAN_HIGH_1_G( "GLC LL %d: Sent LLGMM-STAT-IND Frmr Cond",
        gllc_sapi_addr_lookup[lle]);
      break;

    case GLLCMM_FRAME_REJECT_RESP:
      MSG_GERAN_HIGH_1_G( "GLC LL %d: Sent LLGMM-STAT-IND Frmr Resp",
        gllc_sapi_addr_lookup[lle]);
      break;

    case GLLCMM_L2_REESTABLISHMENT:
      MSG_GERAN_HIGH_1_G( "GLC LL %d: Sent LLGMM-STAT-IND L2 Re-est",
        gllc_sapi_addr_lookup[lle]);
      break;

    default:
      MSG_GERAN_HIGH_2_G( "GLC LL %d: Sent LLGMM-STAT-IND %d",
        gllc_sapi_addr_lookup[lle], cause);
    }

    llgmm_dl_mes -> cmd.llgmm_status_ind.tlli  = tlli;
    llgmm_dl_mes -> cmd.llgmm_status_ind.cause = cause;
#ifdef FEATURE_DUAL_SIM
    llgmm_dl_mes -> cmd.llgmm_status_ind.as_id =
      geran_map_gas_id_to_nas_id(gas_id);
#endif /* FEATURE_DUAL_SIM */

    cfa_log_packet_ex
    (
      GS_QUEUE_MM, MS_LLC_GMM, (uint8)GLLCMM_LLGMM_STATUS_IND,
      sizeof( gllcmm_llgmm_status_ind_type ),
      (uint8 *)&llgmm_dl_mes -> cmd.llgmm_status_ind
    );

    mm_put_cmd( llgmm_dl_mes );
  }
  else
  {
#ifndef TEST_FRAMEWORK
    ERR_GERAN_FATAL_0_G( "GLC ME: GLLCMM Get: No buffers");
#else
    #error code not present
#endif
  }

} /* end of gllc_lle_send_llgmm_status_ind_mes() */

/*===========================================================================
===
===  FUNCTION      GLLC_LLME_SEND_LLGMM_STATUS_IND_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required GMM
===   message and posts this message to the GMM entity.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_llme_send_llgmm_status_ind_mes
(
  gas_id_t                gas_id,
  uint8                   protocol_disc,
  uint8                   transaction_id,
  gprs_grr_status_cause_t cause
)
{
  mm_cmd_type *llgmm_dl_mes;

  llgmm_dl_mes = mm_gllc_get_cmd_buf( (uint8)GLLCMM_LLGMM_STATUS_IND );

  if ( llgmm_dl_mes != NULL )
  {

    llgmm_dl_mes -> cmd.llgmm_status_ind.tlli           = gllc_llme[gas_id].tlli;
    llgmm_dl_mes -> cmd.llgmm_status_ind.protocol_disc  = protocol_disc;
    llgmm_dl_mes -> cmd.llgmm_status_ind.transaction_id = transaction_id;
    llgmm_dl_mes -> cmd.llgmm_status_ind.cause          = 
      (cause == GRR_PDU_TX_SUCCESS ? GLLCMM_L2_TX_SUCCESS : GLLCMM_L2_TX_FAILURE);
#ifdef FEATURE_DUAL_SIM
    llgmm_dl_mes -> cmd.llgmm_status_ind.as_id          = geran_map_gas_id_to_nas_id( gas_id );
#endif /* FEATURE_DUAL_SIM */

    cfa_log_packet_ex
    (
      GS_QUEUE_MM, MS_LLC_GMM, (uint8)GLLCMM_LLGMM_STATUS_IND,
      sizeof( gllcmm_llgmm_status_ind_type ),
      (uint8 *)&llgmm_dl_mes -> cmd.llgmm_status_ind
    );

    mm_put_cmd( llgmm_dl_mes );
  }
  else
  {
#ifndef TEST_FRAMEWORK
    ERR_GERAN_FATAL_0_G( "GLC ME: GLLCMM Get: No buffers");
#else
    #error code not present
#endif
  }

} /* end of gllc_llme_send_llgmm_status_ind_mes() */

/*===========================================================================
===
===  FUNCTION      GLLC_LLME_SEND_LLGMM_TRIGGER_IND_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required GMM
===   message and posts this message to the GMM entity.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_llme_send_llgmm_trigger_ind_mes( gas_id_t gas_id )
{
  mm_cmd_type *llgmm_dl_mes;

  llgmm_dl_mes = mm_gllc_get_cmd_buf( (uint8)GLLCMM_LLGMM_TRIGGER_IND );

  if ( llgmm_dl_mes != NULL )
  {
    MSG_GERAN_MED_0_G( "GLC ME: Sent LLGMM-TRIG-IND");

    llgmm_dl_mes -> cmd.llgmm_trigger_ind.tlli = gllc_llme[gas_id].tlli;
#ifdef FEATURE_DUAL_SIM
    llgmm_dl_mes -> cmd.llgmm_trigger_ind.as_id = geran_map_gas_id_to_nas_id( gas_id );
#endif /* FEATURE_DUAL_SIM */

    cfa_log_packet_ex
    (
      GS_QUEUE_MM, MS_LLC_GMM, (uint8)GLLCMM_LLGMM_TRIGGER_IND,
      sizeof( gllcmm_llgmm_trigger_ind_type ),
      (uint8 *)&llgmm_dl_mes -> cmd.llgmm_trigger_ind
    );

    mm_put_cmd( llgmm_dl_mes );
  }
  else
  {
#ifndef TEST_FRAMEWORK
    ERR_GERAN_FATAL_0_G( "GLC ME: GLLCMM Get: No buffers");
#else
    #error code not present
#endif
  }

} /* end of gllc_llme_send_llgmm_trigger_ind_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_SEND_LLGMM_RDY_TMR_IND_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required GMM
===   message and posts this message to the GMM entity.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_llme_send_llgmm_rdy_tmr_ind_mes( gas_id_t gas_id )
{
  mm_cmd_type *llgmm_dl_mes;

  llgmm_dl_mes = mm_gllc_get_cmd_buf( (uint8)GLLCMM_LLGMM_RDY_TMR_IND );

  if ( llgmm_dl_mes != NULL )
  {
    MSG_GERAN_MED_0_G( "GLC ME: Sent LLGMM-RDY-TMR-IND");

    llgmm_dl_mes -> cmd.llgmm_rdy_tmr_ind.tlli = gllc_llme[gas_id].tlli;
#ifdef FEATURE_DUAL_SIM
    llgmm_dl_mes -> cmd.llgmm_rdy_tmr_ind.as_id = geran_map_gas_id_to_nas_id( gas_id );
#endif /* FEATURE_DUAL_SIM */

    cfa_log_packet_ex
    (
      GS_QUEUE_MM, MS_LLC_GMM, (uint8)GLLCMM_LLGMM_RDY_TMR_IND,
      sizeof( gllcmm_llgmm_rdy_tmr_ind_type ),
      (uint8 *)&llgmm_dl_mes -> cmd.llgmm_rdy_tmr_ind
    );

    mm_put_cmd( llgmm_dl_mes );
  }
  else
  {
#ifndef TEST_FRAMEWORK
    ERR_GERAN_FATAL_0_G( "GLC ME: GLLCMM Get: No buffers");
#else
    #error code not present
#endif
  }

} /* end of gllc_llme_send_llgmm_rdy_tmr_ind_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_SEND_LLGMM_SUSPEND_CNF_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required GMM
===   message and posts this message to the GMM entity.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_llme_send_llgmm_suspend_cnf_mes( gas_id_t gas_id )
{
  mm_cmd_type *llgmm_dl_mes;

  llgmm_dl_mes = mm_gllc_get_cmd_buf( (uint8)GLLCMM_LLGMM_SUSPEND_CNF );

  if ( llgmm_dl_mes != NULL )
  {
    MSG_GERAN_MED_0_G( "GLC ME: Sent LLGMM-SUSPEND-CNF");

    llgmm_dl_mes -> cmd.llgmm_suspend_cnf.tlli = gllc_llme[gas_id].tlli;
#ifdef FEATURE_DUAL_SIM
    llgmm_dl_mes -> cmd.llgmm_suspend_cnf.as_id = geran_map_gas_id_to_nas_id( gas_id );
#endif /* FEATURE_DUAL_SIM */

    cfa_log_packet_ex
    (
      GS_QUEUE_MM, MS_LLC_GMM, (uint8)GLLCMM_LLGMM_SUSPEND_CNF,
      sizeof( gllcmm_llgmm_suspend_cnf_type ),
      (uint8 *)&llgmm_dl_mes -> cmd.llgmm_suspend_cnf
    );

    mm_put_cmd( llgmm_dl_mes );
  }
  else
  {
#ifndef TEST_FRAMEWORK
    ERR_GERAN_FATAL_0_G( "GLC ME: GLLCMM Get: No buffers");
#else
    #error code not present
#endif
  }

} /* end of gllc_llme_send_llgmm_suspend_cnf_mes() */

/*===========================================================================
===
===  FUNCTION      GLLC_LLME_SEND_LLGMM_NEW_TLLI_IND_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required GMM
===   message and posts this message to the GMM entity.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_llme_send_llgmm_new_tlli_ind_mes( gas_id_t gas_id, uint32 new_tlli )
{
  mm_cmd_type *llgmm_dl_mes;

  llgmm_dl_mes = mm_gllc_get_cmd_buf( (uint8)GLLCMM_LLGMM_NEW_TLLI_IND );

  if ( llgmm_dl_mes != NULL )
  {
    MSG_GERAN_MED_1("GLC ME: Sent LLGMM-NEW-TLLI-IND 0x%x", new_tlli);

    llgmm_dl_mes -> cmd.llgmm_new_tlli_ind.new_tlli = new_tlli;
#ifdef FEATURE_DUAL_SIM
    llgmm_dl_mes -> cmd.llgmm_new_tlli_ind.as_id =
      geran_map_gas_id_to_nas_id( gas_id );
#endif /* FEATURE_DUAL_SIM */

    cfa_log_packet_ex
    (
      GS_QUEUE_MM, MS_LLC_GMM, (uint8)GLLCMM_LLGMM_NEW_TLLI_IND,
      sizeof( gllcmm_llgmm_new_tlli_ind_type ),
      (uint8 *)&llgmm_dl_mes -> cmd.llgmm_new_tlli_ind
    );

    mm_put_cmd( llgmm_dl_mes );
  }
  else
  {
#ifndef TEST_FRAMEWORK
    ERR_GERAN_FATAL_0_G( "GLC ME: GLLCMM Get: No buffers");
#else
    #error code not present
#endif
  }

} /* end of gllc_llme_send_llgmm_new_tlli_ind_mes() */


#ifdef FEATURE_GPRS_PS_HANDOVER

/*===========================================================================
===
===  FUNCTION      GLLC_LLME_SEND_LLGMM_PSHO_CNF_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required GMM
===   message and posts this message to the GMM entity.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_llme_send_llgmm_psho_cnf_mes( gas_id_t gas_id )
{
  mm_cmd_type *llgmm_dl_mes;

  llgmm_dl_mes = mm_gllc_get_cmd_buf( (uint8)GLLCMM_LLGMM_PSHO_CNF );

  if ( llgmm_dl_mes != NULL )
  {
    MSG_GERAN_MED_0_G( "GLC ME: Sent LLGMM-PSHO-CNF");

#ifdef FEATURE_DUAL_SIM
    llgmm_dl_mes -> cmd.llgmm_psho_cnf.as_id = geran_map_gas_id_to_nas_id( gas_id );
#endif /* FEATURE_DUAL_SIM */

    cfa_log_packet_ex
    (
      GS_QUEUE_MM, MS_LLC_GMM, (uint8)GLLCMM_LLGMM_PSHO_CNF,
      sizeof( gllcmm_ll_psho_cnf_type ),
      (uint8 *)&llgmm_dl_mes -> cmd.llgmm_psho_cnf
    );

    mm_put_cmd( llgmm_dl_mes );
  }
  else
  {
#ifndef TEST_FRAMEWORK
    ERR_GERAN_FATAL_0_G( "GLC ME: GLLCMM Get: No buffers");
#else
    #error code not present
#endif
  }

} /* end of gllc_llme_send_llgmm_psho_cnf_mes() */

#endif /* FEATURE_GPRS_PS_HANDOVER */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_RST_IND_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required layer 3
===   message and posts this message to the layer 3 entity associated with
===   the given LLE.
===
===   Message information is collated from state information relating to
===   the LLE, and the given parameters.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_send_ll_rst_ind_mes( gas_id_t gas_id, gllc_ll_sapi_t lle, uint32 tlli )
{
  gprs_ll_dl_sig_mes_t *ll_dl_mes = gllc_ll_dl_sig_get_mes_buf( gas_id, lle );

  if ( ll_dl_mes != NULL )
  {
    ll_dl_mes -> sapi                            = gllc_sapi_addr_lookup[lle];
    ll_dl_mes -> mes.prim                        = LL_RESET_IND;
    ll_dl_mes -> mes.rst_ind_mes.tlli            = tlli;
#ifdef FEATURE_GPRS_PS_HANDOVER
    ll_dl_mes -> mes.rst_ind_mes.use_old_xid_ind = gllc_llme[gas_id].use_old_xid_ind;
#endif /* FEATURE_GPRS_PS_HANDOVER */

    ll_dl_mes -> mes.rst_ind_mes.gas_id          = gas_id;

    if ( GLLC_LLE_IS_3_5_9_11( lle ) )
    {
      cfa_log_packet_ex
      (
        GS_QUEUE_SNDCP, MS_LLC_SNDCP, (uint8)LL_RESET_IND,
        sizeof( ll_reset_ind_t ), (uint8 *)&ll_dl_mes -> mes.rst_ind_mes
      );
    }

    gllc_ll_dl_sig_put_mes_buf( gas_id, lle, ll_dl_mes );
  }

} /* end of gllc_lle_send_ll_rst_ind_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_EST_IND_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required layer 3
===   message and posts this message to the layer 3 entity associated with
===   the given LLE.
===
===   Message information is collated from state information relating to
===   the LLE, and the given parameters.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_send_ll_est_ind_mes
(
  gas_id_t       gas_id,
  gllc_ll_sapi_t lle,
  gllc_u_frame_t *u_frame,
  uint32         tlli
)
{
  gprs_ll_dl_sig_mes_t *ll_dl_mes = gllc_ll_dl_sig_get_mes_buf(gas_id, lle );

  if ( ll_dl_mes != NULL )
  {
    MSG_GERAN_MED_3_G
    (
      "GLC LL %d: Sent LL-EST-IND I%d U%d",
      gllc_sapi_addr_lookup[lle],
      gllc_lle[gas_id][lle].ack.max_info_octets_N201_I,
      gllc_lle[gas_id][lle].unack.max_info_octets_N201_U
    );

    ll_dl_mes -> sapi                       = gllc_sapi_addr_lookup[lle];
    ll_dl_mes -> mes.prim                   = LL_ESTABLISH_IND;
    ll_dl_mes -> mes.est_ind_mes.tlli       = tlli;
    ll_dl_mes -> mes.est_ind_mes.l3_xid_req = NULL;

    ll_dl_mes -> mes.est_ind_mes.n201_u =
      gllc_lle[gas_id][lle].unack.max_info_octets_N201_U;

    ll_dl_mes -> mes.est_ind_mes.n201_i =
      gllc_lle[gas_id][lle].ack.max_info_octets_N201_I;

    if ( u_frame != NULL )
    {
      if ( u_frame -> xid == GLLC_XID_REQ )
      {
        if ( u_frame -> info_field.xid_info.contents & GLLC_LAY3_XID )
        {
          if ( u_frame -> info_field.xid_info.l3_xid.params != NULL )
          {
            /* Any layer 3 content is expected to draw a response from
               layer 3. */

            gllc_lle_set_ul_event_mask_scheme(
              gas_id, lle, GLLC_LLE_SCHEME_WAIT_LAY3_RESP );

            /* The N201-U and N201-I parameters set above will not be the
               intended values if a response from layer 3 is required,
               as layer 2 XID parameters are not set until the response
               is received. If these are present in the generated XID
               negotiated response unnumbered information element of the
               LLE, then use these. */

            if ( gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_N201_U_XID )
              ll_dl_mes -> mes.est_ind_mes.n201_u =
                gllc_lle[gas_id][lle].unnum.xid_neg.n201_u_xid;

            if ( gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_N201_I_XID )
              ll_dl_mes -> mes.est_ind_mes.n201_i =
                gllc_lle[gas_id][lle].unnum.xid_neg.n201_i_xid;
          }

          ll_dl_mes -> mes.est_ind_mes.l3_xid_req =
            u_frame -> info_field.xid_info.l3_xid.params;

          u_frame -> info_field.xid_info.l3_xid.params = NULL;
        }
      }
    }

    ll_dl_mes -> mes.est_ind_mes.gas_id          = gas_id;

    (void)rex_clr_timer( &gllc_timer_t200[gas_id][lle] );
    GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_TIMER_T200_EVT );
    gllc_lle_clr_ul_event_mask_scheme( gas_id, lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP );

    cfa_log_packet_ex
    (
      GS_QUEUE_SNDCP, MS_LLC_SNDCP, (uint8)LL_ESTABLISH_IND,
      sizeof( ll_establish_ind_t ), (uint8 *)&ll_dl_mes -> mes.est_ind_mes
    );

    gllc_ll_dl_sig_put_mes_buf( gas_id, lle, ll_dl_mes );
  }

} /* end of gllc_lle_send_ll_est_ind_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_EST_CNF_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required layer 3
===   message and posts this message to the layer 3 entity associated with
===   the given LLE.
===
===   Message information is collated from state information relating to
===   the LLE, and the given parameters.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_send_ll_est_cnf_mes
(
  gas_id_t       gas_id,
  gllc_ll_sapi_t lle,
  gllc_u_frame_t *u_frame,
  uint32         tlli
)
{
  gprs_ll_dl_sig_mes_t *ll_dl_mes = gllc_ll_dl_sig_get_mes_buf(gas_id, lle );

  if ( ll_dl_mes != NULL )
  {
    MSG_GERAN_MED_3_G
    (
      "GLC LL %d: Sent LL-EST-CNF I%d U%d",
      gllc_sapi_addr_lookup[lle],
      gllc_lle[gas_id][lle].ack.max_info_octets_N201_I,
      gllc_lle[gas_id][lle].unack.max_info_octets_N201_U
    );

    ll_dl_mes -> sapi                       = gllc_sapi_addr_lookup[lle];
    ll_dl_mes -> mes.prim                   = LL_ESTABLISH_CNF;
    ll_dl_mes -> mes.est_cnf_mes.tlli       = tlli;
    ll_dl_mes -> mes.est_cnf_mes.l3_xid_neg = NULL;

    ll_dl_mes -> mes.est_cnf_mes.n201_u =
      gllc_lle[gas_id][lle].unack.max_info_octets_N201_U;

    ll_dl_mes -> mes.est_cnf_mes.n201_i =
      gllc_lle[gas_id][lle].ack.max_info_octets_N201_I;

    if ( u_frame != NULL )
    {
      if ( u_frame -> xid == GLLC_XID_NEG )
      {
        if ( u_frame -> info_field.xid_info.contents & GLLC_LAY3_XID )
        {
          ll_dl_mes -> mes.est_cnf_mes.l3_xid_neg =
            u_frame -> info_field.xid_info.l3_xid.params;

          u_frame -> info_field.xid_info.l3_xid.params = NULL;
        }
      }
    }

    ll_dl_mes -> mes.est_cnf_mes.gas_id = gas_id;

    (void)rex_clr_timer( &gllc_timer_t200[gas_id][lle] );
    GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_TIMER_T200_EVT );
    gllc_lle_clr_ul_event_mask_scheme( gas_id, lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP );

    cfa_log_packet_ex
    (
      GS_QUEUE_SNDCP, MS_LLC_SNDCP, (uint8)LL_ESTABLISH_CNF,
      sizeof( ll_establish_cnf_t ), (uint8 *)&ll_dl_mes -> mes.est_cnf_mes
    );

    gllc_ll_dl_sig_put_mes_buf( gas_id, lle, ll_dl_mes );
  }

} /* end of gllc_lle_send_ll_est_cnf_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_REL_IND_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required layer 3
===   message and posts this message to the layer 3 entity associated with
===   the given LLE.
===
===   Message information is collated from state information relating to
===   the LLE, and the given parameters.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_send_ll_rel_ind_mes
(
  gas_id_t       gas_id,
  gllc_ll_sapi_t lle,
  uint32         tlli,
  ll_cause_t     cause
)
{
  gprs_ll_dl_sig_mes_t *ll_dl_mes = gllc_ll_dl_sig_get_mes_buf(gas_id, lle );

  if ( ll_dl_mes != NULL )
  {
    switch ( cause )
    {
    case GPRS_LL_NORMAL_RELEASE:
      MSG_GERAN_MED_1_G( "GLC LL %d: Sent LL-REL-IND Norm Rel",
        gllc_sapi_addr_lookup[lle]);
      break;

    case GPRS_LL_NO_PEER_RESPONSE:
      MSG_GERAN_MED_1_G( "GLC LL %d: Sent LL-REL-IND No Peer",
        gllc_sapi_addr_lookup[lle]);
      break;

    case GPRS_LL_DM_RECEIVED:
      MSG_GERAN_MED_1_G( "GLC LL %d: Sent LL-REL-IND DM Rcvd",
        gllc_sapi_addr_lookup[lle]);
      break;

    case GPRS_LL_INVALID_XID_RESP:
      MSG_GERAN_MED_1_G( "GLC LL %d: Sent LL-REL-IND Invld XID",
        gllc_sapi_addr_lookup[lle]);
      break;

    default:
      MSG_GERAN_MED_2_G( "GLC LL %d: Sent LL-REL-IND %d",
        gllc_sapi_addr_lookup[lle], cause);
    }

    ll_dl_mes -> sapi                  = gllc_sapi_addr_lookup[lle];
    ll_dl_mes -> mes.prim              = LL_RELEASE_IND;
    ll_dl_mes -> mes.rel_ind_mes.tlli  = tlli;
    ll_dl_mes -> mes.rel_ind_mes.cause = cause;
    ll_dl_mes -> mes.rel_ind_mes.gas_id= gas_id;

    (void)rex_clr_timer( &gllc_timer_t200[gas_id][lle] );
    GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_TIMER_T200_EVT );
    gllc_lle_clr_ul_event_mask_scheme( gas_id, lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP );

    cfa_log_packet_ex
    (
      GS_QUEUE_SNDCP, MS_LLC_SNDCP, (uint8)LL_RELEASE_IND,
      sizeof( ll_release_ind_t ), (uint8 *)&ll_dl_mes -> mes.rel_ind_mes
    );

    gllc_ll_dl_sig_put_mes_buf( gas_id, lle, ll_dl_mes );
  }

} /* end of gllc_lle_send_ll_rel_ind_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_REL_CNF_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required layer 3
===   message and posts this message to the layer 3 entity associated with
===   the given LLE.
===
===   Message information is collated from state information relating to
===   the LLE, and the given parameters.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_send_ll_rel_cnf_mes( gas_id_t gas_id, gllc_ll_sapi_t lle, uint32 tlli )
{
  gprs_ll_dl_sig_mes_t *ll_dl_mes = gllc_ll_dl_sig_get_mes_buf(gas_id, lle );

  if ( ll_dl_mes != NULL )
  {
    MSG_GERAN_MED_1_G( "GLC LL %d: Sent LL-REL-CNF",
      gllc_sapi_addr_lookup[lle]);

    ll_dl_mes -> sapi                 = gllc_sapi_addr_lookup[lle];
    ll_dl_mes -> mes.prim             = LL_RELEASE_CNF;
    ll_dl_mes -> mes.rel_cnf_mes.tlli = tlli;
    ll_dl_mes -> mes.rel_cnf_mes.gas_id = gas_id;

    (void)rex_clr_timer( &gllc_timer_t200[gas_id][lle] );
    GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_TIMER_T200_EVT );
    gllc_lle_clr_ul_event_mask_scheme( gas_id, lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP );

    cfa_log_packet_ex
    (
      GS_QUEUE_SNDCP, MS_LLC_SNDCP, (uint8)LL_RELEASE_CNF,
      sizeof( ll_release_cnf_t ), (uint8 *)&ll_dl_mes -> mes.rel_cnf_mes
    );

    gllc_ll_dl_sig_put_mes_buf( gas_id, lle, ll_dl_mes );
  }

} /* end of gllc_lle_send_ll_rel_cnf_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_XID_IND_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required layer 3
===   message and posts this message to the layer 3 entity associated with
===   the given LLE.
===
===   Message information is collated from state information relating to
===   the LLE, and the given parameters.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_send_ll_xid_ind_mes
(
  gas_id_t          gas_id,
  gllc_ll_sapi_t    lle,
  gllc_u_frame_t    *u_frame,
  uint32            tlli,
  boolean           log_ll_xid_ind
)
{
  gprs_ll_dl_sig_mes_t *ll_dl_mes = gllc_ll_dl_sig_get_mes_buf(gas_id, lle );

  if ( ll_dl_mes != NULL )
  {
    if (TRUE == log_ll_xid_ind)
    {
      MSG_GERAN_MED_3_G
      (
        "GLC LL %d: Sent LL-XID-IND I%d U%d",
        gllc_sapi_addr_lookup[lle],
        gllc_lle[gas_id][lle].ack.max_info_octets_N201_I,
        gllc_lle[gas_id][lle].unack.max_info_octets_N201_U
      );
    }
    
    ll_dl_mes -> sapi                       = gllc_sapi_addr_lookup[lle];
    ll_dl_mes -> mes.prim                   = LL_XID_IND;
    ll_dl_mes -> mes.xid_ind_mes.tlli       = tlli;
    ll_dl_mes -> mes.xid_ind_mes.l3_xid_req = NULL;

    ll_dl_mes -> mes.xid_ind_mes.n201_u =
      gllc_lle[gas_id][lle].unack.max_info_octets_N201_U;

    ll_dl_mes -> mes.xid_ind_mes.n201_i =
      gllc_lle[gas_id][lle].ack.max_info_octets_N201_I;

    if ( u_frame != NULL )
    {
      if ( u_frame -> xid == GLLC_XID_REQ )
      {
        if ( u_frame -> info_field.xid_info.contents & GLLC_LAY3_XID )
        {
          if ( u_frame -> info_field.xid_info.l3_xid.params != NULL )
          {
            /* Any layer 3 content is expected to draw a response from
               layer 3. */

            gllc_lle_set_ul_event_mask_scheme(
              gas_id, lle, GLLC_LLE_SCHEME_WAIT_LAY3_RESP );

            /* The N201-U and N201-I parameters set above will not be the
               intended values if a response from layer 3 is required,
               as layer 2 XID parameters are not set until the response
               is received. If these are present in the generated XID
               negotiated response unnumbered information element of the
               LLE, then use these. */

            if ( gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_N201_U_XID )
              ll_dl_mes -> mes.xid_ind_mes.n201_u =
                gllc_lle[gas_id][lle].unnum.xid_neg.n201_u_xid;

            if ( gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_N201_I_XID )
              ll_dl_mes -> mes.xid_ind_mes.n201_i =
                gllc_lle[gas_id][lle].unnum.xid_neg.n201_i_xid;
          }

          ll_dl_mes -> mes.xid_ind_mes.l3_xid_req =
            u_frame -> info_field.xid_info.l3_xid.params;

          u_frame -> info_field.xid_info.l3_xid.params = NULL;
        }
      }
    }

    ll_dl_mes -> mes.xid_ind_mes.gas_id = gas_id;

    (void)rex_clr_timer( &gllc_timer_t200[gas_id][lle] );
    GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_TIMER_T200_EVT );
    gllc_lle_clr_ul_event_mask_scheme( gas_id, lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP );

    if ( GLLC_LLE_IS_3_5_9_11( lle ) )
    {
      cfa_log_packet_ex
      (
        GS_QUEUE_SNDCP, MS_LLC_SNDCP, (uint8)LL_XID_IND,
        sizeof( ll_xid_ind_t ), (uint8 *)&ll_dl_mes -> mes.xid_ind_mes
      );
    }

    gllc_ll_dl_sig_put_mes_buf( gas_id, lle, ll_dl_mes );
  }

} /* end of gllc_lle_send_ll_xid_ind_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_XID_CNF_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required layer 3
===   message and posts this message to the layer 3 entity associated with
===   the given LLE.
===
===   Message information is collated from state information relating to
===   the LLE, and the given parameters.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_send_ll_xid_cnf_mes
(
  gas_id_t       gas_id,
  gllc_ll_sapi_t lle,
  gllc_u_frame_t *u_frame,
  uint32         tlli
)
{
  gprs_ll_dl_sig_mes_t *ll_dl_mes = gllc_ll_dl_sig_get_mes_buf(gas_id, lle );

  if ( ll_dl_mes != NULL )
  {
    MSG_GERAN_MED_3_G
    (
      "GLC LL %d: Sent LL-XID-CNF I%d U%d",
      gllc_sapi_addr_lookup[lle],
      gllc_lle[gas_id][lle].ack.max_info_octets_N201_I,
      gllc_lle[gas_id][lle].unack.max_info_octets_N201_U
    );

    ll_dl_mes -> sapi                       = gllc_sapi_addr_lookup[lle];
    ll_dl_mes -> mes.prim                   = LL_XID_CNF;
    ll_dl_mes -> mes.xid_cnf_mes.tlli       = tlli;
    ll_dl_mes -> mes.xid_cnf_mes.l3_xid_neg = NULL;

    ll_dl_mes -> mes.xid_cnf_mes.n201_u =
      gllc_lle[gas_id][lle].unack.max_info_octets_N201_U;

    ll_dl_mes -> mes.xid_cnf_mes.n201_i =
      gllc_lle[gas_id][lle].ack.max_info_octets_N201_I;

    if ( u_frame != NULL )
    {
      if ( u_frame -> xid == GLLC_XID_NEG )
      {
        if ( u_frame -> info_field.xid_info.contents & GLLC_LAY3_XID )
        {
          ll_dl_mes -> mes.xid_cnf_mes.l3_xid_neg =
            u_frame -> info_field.xid_info.l3_xid.params;

          u_frame -> info_field.xid_info.l3_xid.params = NULL;
        }
      }
    }

    ll_dl_mes -> mes.xid_cnf_mes.gas_id = gas_id;

    (void)rex_clr_timer( &gllc_timer_t200[gas_id][lle] );
    GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_TIMER_T200_EVT );
    gllc_lle_clr_ul_event_mask_scheme( gas_id, lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP );

    if ( GLLC_LLE_IS_3_5_9_11( lle ) )
    {
      cfa_log_packet_ex
      (
        GS_QUEUE_SNDCP, MS_LLC_SNDCP, (uint8)LL_XID_CNF,
        sizeof( ll_xid_cnf_t ), (uint8 *)&ll_dl_mes -> mes.xid_cnf_mes
      );
    }

    gllc_ll_dl_sig_put_mes_buf( gas_id, lle, ll_dl_mes );
  }

} /* end of gllc_lle_send_ll_xid_cnf_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_CLR_EST_SUSP()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required layer 3
===   message and posts this message to the layer 3 entity associated with
===   the given LLE.
===
===   Message information is collated from state information relating to
===   the LLE, and the given parameters.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_clr_est_susp ( gas_id_t gas_id, gllc_ll_sapi_t lle )
{

  uint8 sapi = gllc_sapi_addr_lookup[ lle ]; 
  gsn_ul_clr_est_susp( gas_id, sapi );

}


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_DATA_IND_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required layer 3
===   message and posts this message to the layer 3 entity associated with
===   the given LLE.
===
===   Message information is collated from state information relating to
===   the LLE, and the given parameters.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_send_ll_data_ind_mes
(
  gas_id_t          gas_id,
  gllc_ll_sapi_t    lle,
  gllc_i_s_frame_t  *i_s_frame,
  uint32            tlli
)
{
  gprs_ll_dl_dat_mes_t *ll_dl_mes;

  ll_dl_mes = GPRS_MEM_MALLOC(sizeof(gprs_ll_dl_dat_mes_t) / sizeof(uint8));

  if ( ll_dl_mes != NULL )
  {
    MSG_GERAN_LOW_2_G( "GLC LL %d: Sent LL-DATA-IND S:%d",
      gllc_sapi_addr_lookup[lle], i_s_frame -> send_seq_num_N_S);

    ll_dl_mes -> sapi                    = gllc_sapi_addr_lookup[lle];
    ll_dl_mes -> mes.prim                = LL_DATA_IND;
    ll_dl_mes -> mes.data_ind_mes.tlli   = tlli;
    ll_dl_mes -> mes.data_ind_mes.l3_pdu = i_s_frame -> l3_pdu;
    i_s_frame -> l3_pdu                  = NULL;
    ll_dl_mes -> mes.data_ind_mes.gas_id       = gas_id;

    gllc_ll_dl_dat_put_mes_buf( gas_id, lle, ll_dl_mes );
  }
  else
  {
    ERR_GERAN_FATAL_1_G( "GLC LL %d: LL-DATA-IND: Heap Exhaustion",
      gllc_sapi_addr_lookup[lle]);
  }

} /* end of gllc_lle_send_ll_data_ind_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_DATA_CNF_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required layer 3
===   message and posts this message to the layer 3 entity associated with
===   the given LLE.
===
===   Message information is collated from state information relating to
===   the LLE, and the given parameters.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_send_ll_data_cnf_mes
(
  gas_id_t       gas_id,
  gllc_ll_sapi_t lle,
  ll_sndcp_ref_t *sndcp_ref
)
{
  gprs_ll_dl_sig_mes_t *ll_dl_mes;

  switch ( lle )
  {
  case GLLC_LL_SAPI_3_LL3:
    MSG_GERAN_LOW_3_G( "GLC LL 3: Sent LL-DATA-CNF %d:%d:%d",
      sndcp_ref -> nsapi, sndcp_ref -> npdu_num, sndcp_ref -> seg_num );
    break;

  case GLLC_LL_SAPI_5_LL5:
    MSG_GERAN_LOW_3_G( "GLC LL 5: Sent LL-DATA-CNF %d:%d:%d",
      sndcp_ref -> nsapi, sndcp_ref -> npdu_num, sndcp_ref -> seg_num );
    break;

  case GLLC_LL_SAPI_9_LL9:
    MSG_GERAN_LOW_3_G( "GLC LL 9: Sent LL-DATA-CNF %d:%d:%d",
      sndcp_ref -> nsapi, sndcp_ref -> npdu_num, sndcp_ref -> seg_num );
    break;

  case GLLC_LL_SAPI_11_LL11:
    MSG_GERAN_LOW_3_G( "GLC LL 11: Sent LL-DATA-CNF %d:%d:%d",
      sndcp_ref -> nsapi, sndcp_ref -> npdu_num, sndcp_ref -> seg_num );
    break;

  default:
    break;
  }

  ll_dl_mes = gllc_ll_dl_sig_get_mes_buf(gas_id, lle );

  if ( ll_dl_mes != NULL )
  {
    ll_dl_mes -> sapi                       = gllc_sapi_addr_lookup[lle];
    ll_dl_mes -> mes.prim                   = LL_DATA_CNF;
    ll_dl_mes -> mes.data_cnf_mes.tlli      = gllc_llme[gas_id].tlli;
    ll_dl_mes -> mes.data_cnf_mes.sndcp_ref = *sndcp_ref;
    ll_dl_mes -> mes.data_cnf_mes.gas_id    = gas_id;

    gllc_ll_dl_sig_put_mes_buf( gas_id, lle, ll_dl_mes );
  }
} /* end of gllc_lle_send_ll_data_cnf_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_UNITDATA_IND_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required layer 3
===   message and posts this message to the layer 3 entity associated with
===   the given LLE.
===
===   Message information is collated from state information relating to
===   the LLE, and the given parameters.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_send_ll_unitdata_ind_mes
(
  gas_id_t        gas_id,
  gllc_ll_sapi_t  lle,
  gllc_ui_frame_t *ui_frame,
  uint32          tlli
)
{
  gprs_ll_dl_dat_mes_t *ll_dl_mes;

  ll_dl_mes = GPRS_MEM_MALLOC(sizeof(gprs_ll_dl_dat_mes_t) / sizeof(uint8));

  if ( ll_dl_mes != NULL )
  {
    MSG_GERAN_LOW_2_G( "GLC LL %d: Sent LL-UDATA-IND U:%d",
      gllc_sapi_addr_lookup[lle], ui_frame -> send_seq_num_N_U);

    ll_dl_mes -> sapi                          = gllc_sapi_addr_lookup[lle];
    ll_dl_mes -> mes.prim                      = LL_UNITDATA_IND;
    ll_dl_mes -> mes.unitdata_ind_mes.tlli     = tlli;
    ll_dl_mes -> mes.unitdata_ind_mes.ciphered = ui_frame -> encrypted_E;
    ll_dl_mes -> mes.unitdata_ind_mes.l3_pdu   = ui_frame -> l3_pdu;
    ui_frame -> l3_pdu                         = NULL;
    ll_dl_mes -> mes.unitdata_ind_mes.gas_id   = gas_id;

    gllc_ll_dl_dat_put_mes_buf( gas_id, lle, ll_dl_mes );
  }

} /* end of gllc_lle_send_ll_unitdata_ind_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_UNITDATA_CNF_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required layer 3
===   message and posts this message to the layer 3 entity associated with
===   the given LLE.
===
===   Message information is collated from state information relating to
===   the LLE, and the given parameters.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_send_ll_unitdata_cnf_mes
(
  gas_id_t       gas_id,
  gllc_ll_sapi_t lle,
  ll_sndcp_ref_t *sndcp_ref
)
{
  gprs_ll_dl_sig_mes_t *ll_dl_mes;

  switch ( lle )
  {
  case GLLC_LL_SAPI_3_LL3:
    MSG_GERAN_LOW_3_G( "GLC LL 3: Sent LL-UDATA-CNF %d:%d:%d",
      sndcp_ref -> nsapi, sndcp_ref -> npdu_num, sndcp_ref -> seg_num );
    break;

  case GLLC_LL_SAPI_5_LL5:
    MSG_GERAN_LOW_3_G( "GLC LL 5: Sent LL-UDATA-CNF %d:%d:%d",
      sndcp_ref -> nsapi, sndcp_ref -> npdu_num, sndcp_ref -> seg_num );
    break;

  case GLLC_LL_SAPI_9_LL9:
    MSG_GERAN_LOW_3_G( "GLC LL 9: Sent LL-UDATA-CNF %d:%d:%d",
      sndcp_ref -> nsapi, sndcp_ref -> npdu_num, sndcp_ref -> seg_num );
    break;

  case GLLC_LL_SAPI_11_LL11:
    MSG_GERAN_LOW_3_G( "GLC LL 11: Sent LL-UDATA-CNF %d:%d:%d",
      sndcp_ref -> nsapi, sndcp_ref -> npdu_num, sndcp_ref -> seg_num );
    break;

  default:
    break;
  }

  ll_dl_mes = gllc_ll_dl_sig_get_mes_buf(gas_id, lle );

  if ( ll_dl_mes != NULL )
  {
    ll_dl_mes -> sapi                        = gllc_sapi_addr_lookup[lle];
    ll_dl_mes -> mes.prim                    = LL_UNITDATA_CNF;
    ll_dl_mes -> mes.udata_cnf_mes.tlli      = gllc_llme[gas_id].tlli;
    ll_dl_mes -> mes.udata_cnf_mes.sndcp_ref = *sndcp_ref;
    ll_dl_mes -> mes.udata_cnf_mes.gas_id    = gas_id;

    gllc_ll_dl_sig_put_mes_buf( gas_id, lle, ll_dl_mes );
  }
} /* end of gllc_lle_send_ll_udata_cnf_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_LL_STATUS_IND_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required layer 3
===   message and posts this message to the layer 3 entity associated with
===   the given LLE.
===
===   Message information is collated from state information relating to
===   the LLE, and the given parameters.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_send_ll_status_ind_mes
(
  gas_id_t       gas_id,
  gllc_ll_sapi_t lle,
  uint32         tlli,
  ll_cause_t     cause
)
{
  gprs_ll_dl_sig_mes_t *ll_dl_mes = gllc_ll_dl_sig_get_mes_buf(gas_id, lle );

  if ( ll_dl_mes != NULL )
  {
    switch ( cause )
    {
    case GPRS_LL_NORMAL_RELEASE:
      MSG_GERAN_MED_1_G( "GLC LL %d: Sent LL-STAT-IND Norm Rel",
        gllc_sapi_addr_lookup[lle]);
      break;

    case GPRS_LL_NO_PEER_RESPONSE:
      MSG_GERAN_MED_1_G( "GLC LL %d: Sent LL-STAT-IND No Peer",
        gllc_sapi_addr_lookup[lle]);
      break;

    case GPRS_LL_DM_RECEIVED:
      MSG_GERAN_MED_1_G( "GLC LL %d: Sent LL-STAT-IND DM Rcvd",
        gllc_sapi_addr_lookup[lle]);
      break;

    case GPRS_LL_INVALID_XID_RESP:
      MSG_GERAN_MED_1_G( "GLC LL %d: Sent LL-STAT-IND Invld XID",
        gllc_sapi_addr_lookup[lle]);
      break;

    default:
      MSG_GERAN_MED_2_G( "GLC LL %d: Sent LL-STAT-IND %d",
        gllc_sapi_addr_lookup[lle], cause);
    }

    ll_dl_mes -> sapi                        = gllc_sapi_addr_lookup[lle];
    ll_dl_mes -> mes.prim                    = LL_STATUS_IND;
    ll_dl_mes -> mes.status_ind_mes.tlli     = tlli;
    ll_dl_mes -> mes.status_ind_mes.cause    = cause;
    ll_dl_mes -> mes.status_ind_mes.gas_id   = gas_id;

    (void)rex_clr_timer( &gllc_timer_t200[gas_id][lle] );
    GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_TIMER_T200_EVT );
    gllc_lle_clr_ul_event_mask_scheme( gas_id, lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP );

    if ( GLLC_LLE_IS_3_5_9_11( lle ) )
    {
      cfa_log_packet_ex
      (
        GS_QUEUE_SNDCP, MS_LLC_SNDCP, (uint8)LL_STATUS_IND,
        sizeof( ll_status_ind_t ), (uint8 *)&ll_dl_mes -> mes.status_ind_mes
      );
    }

    gllc_ll_dl_sig_put_mes_buf( gas_id, lle, ll_dl_mes );
  }

} /* end of gllc_lle_send_ll_status_ind_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_SABM_CMD_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required unnumbered
===   SABM command message and notifies the MPEX transmit handler about the
===   transmission requirement.
===
===   Layer 3 XID content to be included is given explicitly, whereas
===   layer 2 XID content is derived from the LLE's pending XID negotiation
===   request store.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_send_sabm_cmd_mes
(
  gas_id_t       gas_id,
  gllc_ll_sapi_t lle,
  dsm_item_type  **l3_xid_req
)
{
  /* --------------------------------------------------------------------- */

  /* Clear U Frame buffer and collate basic contents. */

  if ( gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.
         info_field.xid_info.l3_xid.params != NULL )
  {
    GPRS_PDU_FREE(
      &gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.
        info_field.xid_info.l3_xid.params );
  }

  (void)memset
  (
    &gllc_lle[gas_id][lle].unnum.cmd_frame,
    0,
    ( sizeof(gllc_frame_t) / sizeof(uint8) )
  );

  gllc_lle[gas_id][lle].unnum.cmd_frame.format                = GLLC_U_FORMAT;
  gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.cmd_res_M4_M1 = GLLC_SABM;
  gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.cmd_or_res_CR = GLLC_COMMAND;
  gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.poll_final_PF = GLLC_PF_POLL;

  /* --------------------------------------------------------------------- */

  /* Include any layer 2 XID negotiation request in the U Frame buffer. */

  if ( gllc_lle[gas_id][lle].unnum.xid_req.contents )
  {
    gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.xid = GLLC_XID_REQ;
    gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.info_field.xid_info =
      gllc_lle[gas_id][lle].unnum.xid_req;

    /* Remove what can't be sent in the uplink direction. */

    gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.info_field.xid_info.contents &=
      ~( GLLC_RESET_XID | GLLC_IOV_UI_XID | GLLC_IOV_I_XID );
  }
  else
    gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.xid = GLLC_XID_NONE;

  /* --------------------------------------------------------------------- */

  /* Include any layer 3 XID parameter negotiation request in the U Frame
     buffer. */

  if ( l3_xid_req != NULL )
  {
    gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.xid = GLLC_XID_REQ;
    gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.
      info_field.xid_info.contents |= GLLC_LAY3_XID;

    if ( *l3_xid_req != NULL )
    {
      gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.
        info_field.xid_info.l3_xid.length =
          (uint8)GPRS_PDU_LENGTH( *l3_xid_req );

      gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.
        info_field.xid_info.l3_xid.params = *l3_xid_req;

      *l3_xid_req = NULL;
    }
    else
    {
      gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.
        info_field.xid_info.l3_xid.length = 0;

      gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.
        info_field.xid_info.l3_xid.params = NULL;
    }

    MSG_GERAN_MED_1("GLC LL %d: Sent SABM L3", gllc_sapi_addr_lookup[lle]);
  }
  else
  {
    MSG_GERAN_MED_1("GLC LL %d: Sent SABM L2", gllc_sapi_addr_lookup[lle]);
  }

  gllc_lle_report_l2_xid_content( gas_id, MSG_LEGACY_MED,
    lle, &gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.info_field.xid_info );

  /* --------------------------------------------------------------------- */

  /* Start the T200 retransmission timer and flag the transmission
     requirement to the MPEX trx handler. */

  gllc_lle[gas_id][lle].unnum.cause       = GRR_CAUSE_LLC_SIGNALLING;
  gllc_lle[gas_id][lle].unnum.retrx_ctr   = 0;

  (void)rex_set_timer
  (
    &gllc_timer_t200[gas_id][lle],
    gllc_lle[gas_id][lle].gen.retrx_interval_T200
  );
  GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_TIMER_T200_EVT );
  gllc_lle_set_ul_event_mask_scheme( gas_id, lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP );

  GLLC_SET_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_U_FR_CMD_EVT );

  /* --------------------------------------------------------------------- */

} /* end of gllc_lle_send_sabm_cmd_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_RESEND_SABM_CMD_MES()
===
===  DESCRIPTION
===
===   Initiates a retransmission of the U frame containing the SABM command.
===   If the number of retransmissions has exceeded the N200 limit, then a
===   retransmission is not initiated.
===
===  DEPENDENCIES
===
===   The LLE unnumbered information U Frame state element must contain the
===   original U frame SABM command in the .
===
===  RETURN VALUE
===
===   TRUE if retransmission is initiated, FALSE otherwise.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

boolean gllc_lle_resend_sabm_cmd_mes( gas_id_t gas_id, gllc_ll_sapi_t lle )
{
  if
  (
    (gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.cmd_res_M4_M1 == GLLC_SABM   ) &&
    (gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.cmd_or_res_CR == GLLC_COMMAND) &&
    (gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.poll_final_PF == GLLC_PF_POLL)
  )
  {
    if ( gllc_lle[gas_id][lle].unnum.retrx_ctr <
         gllc_lle[gas_id][lle].gen.max_retrx_cnt_N200 )
    {
      MSG_GERAN_HIGH_2_G( "GLC LL %d: Retx SABM #%d",
        gllc_sapi_addr_lookup[lle], gllc_lle[gas_id][lle].unnum.retrx_ctr+1 );

      gllc_lle[gas_id][lle].unnum.retrx_ctr++;
      (void)rex_set_timer
      (
        &gllc_timer_t200[gas_id][lle],
        gllc_lle[gas_id][lle].gen.retrx_interval_T200
      );
      GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_TIMER_T200_EVT );
      gllc_lle_set_ul_event_mask_scheme(gas_id, lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP);
      GLLC_SET_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_U_FR_CMD_EVT );
      return ( TRUE );
    }
    else
    {
      MSG_GERAN_HIGH_1_G( "GLC LL %d: Info SABM N200 Limit",
        gllc_sapi_addr_lookup[lle]);

      (void)rex_clr_timer( &gllc_timer_t200[gas_id][lle] );
      GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_TIMER_T200_EVT );
      gllc_lle_clr_ul_event_mask_scheme(gas_id, lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP);
      return ( FALSE );
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G("GLC LL %d: U Fr Retrx Buf Corrupt", gllc_sapi_addr_lookup[lle]);

    (void)rex_clr_timer( &gllc_timer_t200[gas_id][lle] );
    GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_TIMER_T200_EVT );
    gllc_lle_clr_ul_event_mask_scheme(gas_id, lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP);
    return ( FALSE );
  }

} /* end of gllc_lle_resend_sabm_cmd_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_DISC_CMD_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required unnumbered
===   DISC command message and notifies the MPEX transmit handler about the
===   transmission requirement.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_send_disc_cmd_mes( gas_id_t gas_id, gllc_ll_sapi_t lle )
{
  MSG_GERAN_MED_1("GLC LL %d: Sent DISC", gllc_sapi_addr_lookup[lle]);

  /* --------------------------------------------------------------------- */

  /* Clear U Frame buffer and collate basic contents. */

  if ( gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.
         info_field.xid_info.l3_xid.params != NULL )
  {
    GPRS_PDU_FREE(
      &gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.
        info_field.xid_info.l3_xid.params );
  }

  (void)memset
  (
    &gllc_lle[gas_id][lle].unnum.cmd_frame,
    0,
    ( sizeof(gllc_frame_t) / sizeof(uint8) )
  );

  gllc_lle[gas_id][lle].unnum.cmd_frame.format                = GLLC_U_FORMAT;
  gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.cmd_res_M4_M1 = GLLC_DISC;
  gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.cmd_or_res_CR = GLLC_COMMAND;
  gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.poll_final_PF = GLLC_PF_POLL;
  gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.xid           = GLLC_XID_NONE;

  /* --------------------------------------------------------------------- */

  /* Start the T200 retransmission timer and flag the transmission
     requirement to the MPEX trx handler. */

  gllc_lle[gas_id][lle].unnum.retrx_ctr   = 0;
  gllc_lle[gas_id][lle].unnum.cause       = GRR_CAUSE_LLC_SIGNALLING;

  (void)rex_set_timer
  (
    &gllc_timer_t200[gas_id][lle],
    gllc_lle[gas_id][lle].gen.retrx_interval_T200
  );
  GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_TIMER_T200_EVT );
  gllc_lle_set_ul_event_mask_scheme( gas_id, lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP );

  GLLC_SET_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_U_FR_CMD_EVT );

  /* --------------------------------------------------------------------- */

} /* end of gllc_lle_send_disc_cmd_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_RESEND_DISC_CMD_MES()
===
===  DESCRIPTION
===
===   Initiates a retransmission of the U frame containing the DISC command.
===   If the number of retransmissions has exceeded the N200 limit, then a
===   retransmission is not initiated.
===
===  DEPENDENCIES
===
===   The LLE unnumbered information U Frame state element must contain the
===   original U frame DISC command.
===
===  RETURN VALUE
===
===   TRUE if retransmission is initiated, FALSE otherwise.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

boolean gllc_lle_resend_disc_cmd_mes( gas_id_t gas_id, gllc_ll_sapi_t lle )
{
  if
  (
    (gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.cmd_res_M4_M1 == GLLC_DISC   ) &&
    (gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.cmd_or_res_CR == GLLC_COMMAND) &&
    (gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.poll_final_PF == GLLC_PF_POLL)
  )
  {
    if ( gllc_lle[gas_id][lle].unnum.retrx_ctr <
         gllc_lle[gas_id][lle].gen.max_retrx_cnt_N200 )
    {
      MSG_GERAN_HIGH_2_G( "GLC LL %d: Retx DISC #%d",
        gllc_sapi_addr_lookup[lle], gllc_lle[gas_id][lle].unnum.retrx_ctr+1 );

      gllc_lle[gas_id][lle].unnum.retrx_ctr++;
      (void)rex_set_timer
      (
        &gllc_timer_t200[gas_id][lle],
        gllc_lle[gas_id][lle].gen.retrx_interval_T200
      );
      GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_TIMER_T200_EVT );
      gllc_lle_set_ul_event_mask_scheme(gas_id, lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP);

      GLLC_SET_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_U_FR_CMD_EVT );
      return ( TRUE );
    }
    else
    {
      MSG_GERAN_HIGH_1_G( "GLC LL %d: Info DISC N200 Limit",
        gllc_sapi_addr_lookup[lle]);

      (void)rex_clr_timer( &gllc_timer_t200[gas_id][lle] );
      GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_TIMER_T200_EVT );
      gllc_lle_clr_ul_event_mask_scheme(gas_id, lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP);
      return ( FALSE );
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G("GLC LL %d: U Fr Retrx Buf Corrupt", 
      gllc_sapi_addr_lookup[lle] );

    (void)rex_clr_timer( &gllc_timer_t200[gas_id][lle] );
    GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_TIMER_T200_EVT );
    gllc_lle_clr_ul_event_mask_scheme(gas_id, lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP);
    return ( FALSE );
  }

} /* end of gllc_lle_resend_disc_cmd_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_XID_CMD_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required unnumbered
===   XID command message and notifies the MPEX transmit handler about the
===   transmission requirement.
===
===   Layer 3 XID content to be included is given explicitly, whereas
===   layer 2 XID content is derived from the LLE's pending XID negotiation
===   request store.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_send_xid_cmd_mes
(
  gas_id_t       gas_id,
  gllc_ll_sapi_t lle,
  dsm_item_type  **l3_xid_req
)
{
  /* --------------------------------------------------------------------- */

  /* Clear U Frame buffer and collate basic contents. */

  if ( gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.
         info_field.xid_info.l3_xid.params != NULL )
  {
    GPRS_PDU_FREE(
      &gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.
        info_field.xid_info.l3_xid.params );
  }

  (void)memset
  (
    &gllc_lle[gas_id][lle].unnum.cmd_frame,
    0,
    ( sizeof(gllc_frame_t) / sizeof(uint8) )
  );

  gllc_lle[gas_id][lle].unnum.cmd_frame.format                = GLLC_U_FORMAT;
  gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.cmd_res_M4_M1 = GLLC_XID;
  gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.cmd_or_res_CR = GLLC_COMMAND;
  gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.poll_final_PF = GLLC_PF_POLL;

  /* --------------------------------------------------------------------- */

  /* Include any layer 2 XID negotiation request in the U Frame buffer. */

  if ( gllc_lle[gas_id][lle].unnum.xid_req.contents )
  {
    gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.xid = GLLC_XID_REQ;
    gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.info_field.xid_info =
      gllc_lle[gas_id][lle].unnum.xid_req;

    /* Remove what can't be sent in the uplink direction. */

    gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.info_field.xid_info.contents &=
      ~( GLLC_RESET_XID | GLLC_IOV_UI_XID | GLLC_IOV_I_XID );
  }
  else
    gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.xid = GLLC_XID_NONE;

  /* --------------------------------------------------------------------- */

  /* Include any layer 3 XID parameter negotiation request in the U Frame
     buffer. */

  if ( l3_xid_req != NULL )
  {
    gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.xid = GLLC_XID_REQ;
    gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.
      info_field.xid_info.contents |= GLLC_LAY3_XID;

    if ( *l3_xid_req != NULL )
    {
      gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.
        info_field.xid_info.l3_xid.length =
          (uint8)GPRS_PDU_LENGTH( *l3_xid_req );

      gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.
        info_field.xid_info.l3_xid.params = *l3_xid_req;

      *l3_xid_req = NULL;
    }
    else
    {
      gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.
        info_field.xid_info.l3_xid.length = 0;

      gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.
        info_field.xid_info.l3_xid.params = NULL;
    }

    MSG_GERAN_MED_1_G( "GLC LL %d: Sent XID Cmd L3",
      gllc_sapi_addr_lookup[lle]);
  }
  else
  {
    MSG_GERAN_MED_1_G( "GLC LL %d: Sent XID Cmd L2",
      gllc_sapi_addr_lookup[lle]);
  }

  gllc_lle_report_l2_xid_content( gas_id, MSG_LEGACY_MED,
    lle, &gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.info_field.xid_info );

  /* --------------------------------------------------------------------- */

  /* Start the T200 retransmission timer and flag the transmission
     requirement to the MPEX trx handler. */

  gllc_lle[gas_id][lle].unnum.retrx_ctr = 0;
  
  if ( lle == GLLC_LL_SAPI_1_GMM )
    gllc_lle[gas_id][lle].unnum.cause = GRR_CAUSE_GMM_SIGNALLING;
  else
    gllc_lle[gas_id][lle].unnum.cause = GRR_CAUSE_LLC_SIGNALLING;

  (void)rex_set_timer
  (
    &gllc_timer_t200[gas_id][lle],
    gllc_lle[gas_id][lle].gen.retrx_interval_T200
  );
  GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_TIMER_T200_EVT );
  gllc_lle_set_ul_event_mask_scheme( gas_id, lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP );

  GLLC_SET_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_U_FR_CMD_EVT );

  /* --------------------------------------------------------------------- */

} /* end of gllc_lle_send_xid_cmd_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_RESEND_XID_CMD_MES()
===
===  DESCRIPTION
===
===   Initiates a retransmission of the U frame containing the XID command.
===   If the number of retransmissions has exceeded the N200 limit, then a
===   retransmission is not initiated.
===
===  DEPENDENCIES
===
===   The LLE unnumbered information U Frame state element must contain the
===   original U frame XID command.
===
===  RETURN VALUE
===
===   TRUE if retransmission is initiated, FALSE otherwise.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

boolean gllc_lle_resend_xid_cmd_mes( gas_id_t gas_id, gllc_ll_sapi_t lle )
{
  if
  (
    (gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.cmd_res_M4_M1 == GLLC_XID    ) &&
    (gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.cmd_or_res_CR == GLLC_COMMAND) &&
    (gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.poll_final_PF == GLLC_PF_POLL)
  )
  {
    if ( gllc_lle[gas_id][lle].unnum.retrx_ctr <
         gllc_lle[gas_id][lle].gen.max_retrx_cnt_N200 )
    {
      MSG_GERAN_HIGH_2_G( "GLC LL %d: Retx XID #%d",
        gllc_sapi_addr_lookup[lle], gllc_lle[gas_id][lle].unnum.retrx_ctr+1 );

      gllc_lle[gas_id][lle].unnum.retrx_ctr++;
      (void)rex_set_timer
      (
        &gllc_timer_t200[gas_id][lle],
        gllc_lle[gas_id][lle].gen.retrx_interval_T200
      );
      GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_TIMER_T200_EVT );
      gllc_lle_set_ul_event_mask_scheme(gas_id, lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP);

      GLLC_SET_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_U_FR_CMD_EVT );
      return ( TRUE );
    }
    else
    {
      MSG_GERAN_HIGH_1_G( "GLC LL %d: Info XID N200 Limit",
        gllc_sapi_addr_lookup[lle]);

      (void)rex_clr_timer( &gllc_timer_t200[gas_id][lle] );
      GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_TIMER_T200_EVT );
      gllc_lle_clr_ul_event_mask_scheme(gas_id, lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP);
      return ( FALSE );
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G("GLC LL %d: U Fr Retrx Buf Corrupt", 
      gllc_sapi_addr_lookup[lle] );

    (void)rex_clr_timer( &gllc_timer_t200[gas_id][lle] );
    GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_TIMER_T200_EVT );
    gllc_lle_clr_ul_event_mask_scheme(gas_id, lle, GLLC_LLE_SCHEME_WAIT_PEER_RESP);
    return ( FALSE );
  }

} /* end of gllc_lle_resend_xid_cmd_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_NULL_CMD_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required unnumbered
===   NULL command message and notifies the MPEX transmit handler about the
===   transmission requirement.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_send_null_cmd_mes( gas_id_t gas_id, gllc_ll_sapi_t lle )
{
  MSG_GERAN_MED_1("GLC LL %d: Sent NULL Cmd", gllc_sapi_addr_lookup[lle]);

  /* --------------------------------------------------------------------- */

  /* Clear U Frame buffer and collate basic contents. */

  if ( gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.
         info_field.xid_info.l3_xid.params != NULL )
  {
    GPRS_PDU_FREE(
      &gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.
        info_field.xid_info.l3_xid.params );
  }

  (void)memset
  (
    &gllc_lle[gas_id][lle].unnum.cmd_frame,
    0,
    ( sizeof(gllc_frame_t) / sizeof(uint8) )
  );

  gllc_lle[gas_id][lle].unnum.cmd_frame.format                = GLLC_U_FORMAT;
  gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.cmd_res_M4_M1 = GLLC_NULL;
  gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.cmd_or_res_CR = GLLC_COMMAND;
  gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.poll_final_PF = GLLC_PF_UNSOL;
  gllc_lle[gas_id][lle].unnum.cmd_frame.u_frame.xid           = GLLC_XID_NONE;

  /* --------------------------------------------------------------------- */

  /* A NULL command is sent with P=0 and so there is no requirement to
     invoke the T200 timer. Simply flag the transmission requirement to
     the MPEX trx handler. */

  gllc_lle[gas_id][lle].unnum.retrx_ctr = 0;
  GLLC_SET_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_U_FR_CMD_EVT );

  /* --------------------------------------------------------------------- */

} /* end of gllc_lle_send_null_cmd_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_UA_RES_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required unnumbered
===   UA response message and notifies the MPEX transmit handler about the
===   transmission requirement.
===
===   Layer 3 XID content to be included is given explicitly, whereas
===   layer 2 XID content is derived from the LLE's XID negotiation result
===   store.
===
===  DEPENDENCIES
===
===   Any layer 2 XID negotiation request must have been successfully
===   negotiated and the negotiation results must be lodged in the LLE's
===   XID negotiation result store.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_send_ua_res_mes
(
  gas_id_t       gas_id,
  gllc_ll_sapi_t lle,
  dsm_item_type  **l3_xid_neg
)
{
  /* --------------------------------------------------------------------- */

  /* Clear U Frame buffer and collate basic contents. */

  if ( gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.
         info_field.xid_info.l3_xid.params != NULL )
  {
    GPRS_PDU_FREE(
      &gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.
        info_field.xid_info.l3_xid.params );
  }

  (void)memset
  (
    &gllc_lle[gas_id][lle].unnum.sol_res_frame,
    0,
    ( sizeof(gllc_frame_t) / sizeof(uint8) )
  );

  gllc_lle[gas_id][lle].unnum.sol_res_frame.format                = GLLC_U_FORMAT;
  gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.cmd_res_M4_M1 = GLLC_UA;
  gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.cmd_or_res_CR = GLLC_RESPONSE;
  gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.poll_final_PF = GLLC_PF_FINAL;

  /* --------------------------------------------------------------------- */

  /* Include any layer 2 XID negotiation response in the U Frame buffer. */

  if ( gllc_lle[gas_id][lle].unnum.xid_neg.contents )
  {
    gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.xid = GLLC_XID_NEG;
    gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.info_field.xid_info =
      gllc_lle[gas_id][lle].unnum.xid_neg;

    /* Remove what can't be sent in the uplink direction. */

    gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.info_field.xid_info.contents &=
      ~( GLLC_RESET_XID | GLLC_IOV_UI_XID | GLLC_IOV_I_XID );
  }
  else
    gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.xid = GLLC_XID_NONE;

  /* --------------------------------------------------------------------- */

  /* Include any layer 3 XID parameter negotiation response in the U Frame
     buffer. */

  if ( l3_xid_neg != NULL )
  {
    gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.xid = GLLC_XID_NEG;
    gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.
      info_field.xid_info.contents |= GLLC_LAY3_XID;

    if ( *l3_xid_neg != NULL )
    {
      gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.
        info_field.xid_info.l3_xid.length =
          (uint8)GPRS_PDU_LENGTH( *l3_xid_neg );

      gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.
        info_field.xid_info.l3_xid.params = *l3_xid_neg;

      *l3_xid_neg = NULL;
    }
    else
    {
      gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.
        info_field.xid_info.l3_xid.length = 0;

      gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.
        info_field.xid_info.l3_xid.params = NULL;
    }

    MSG_GERAN_MED_1("GLC LL %d: Sent UA L3", gllc_sapi_addr_lookup[lle]);
  }
  else
  {
    MSG_GERAN_MED_1("GLC LL %d: Sent UA L2", gllc_sapi_addr_lookup[lle]);
  }

  gllc_lle_report_l2_xid_content( gas_id, MSG_LEGACY_MED,
    lle, &gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.info_field.xid_info );

  /* --------------------------------------------------------------------- */

  /* Signal new transmit requirement. */

  gllc_lle[gas_id][lle].unnum.retrx_ctr = 0;
  gllc_lle[gas_id][lle].unnum.cause     = GRR_CAUSE_LLC_SIGNALLING;

  gllc_lle_clr_ul_event_mask_scheme( gas_id, lle, GLLC_LLE_SCHEME_WAIT_LAY3_RESP );

  GLLC_SET_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_U_FR_SRES_EVT );

  /* --------------------------------------------------------------------- */

} /* end of gllc_lle_send_ua_res_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_DM_RES_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required unnumbered
===   DM response message and notifies the MPEX transmit handler about the
===   transmission requirement.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_send_dm_res_mes
(
  gas_id_t        gas_id,
  gllc_ll_sapi_t  lle,
  boolean         poll_final_PF
)
{
  MSG_GERAN_MED_2_G( "GLC LL %d: Sent DM F=%d",
    gllc_sapi_addr_lookup[lle], poll_final_PF);

  /* --------------------------------------------------------------------- */

  if ( poll_final_PF == GLLC_PF_FINAL )
  {
    /* Clear solicited response U Frame buffer and collate basic contents. */

    if ( gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.
           info_field.xid_info.l3_xid.params != NULL )
    {
      GPRS_PDU_FREE(
        &gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.
          info_field.xid_info.l3_xid.params );
    }

    (void)memset
    (
      &gllc_lle[gas_id][lle].unnum.sol_res_frame,
      0,
      ( sizeof(gllc_frame_t) / sizeof(uint8) )
    );

    gllc_lle[gas_id][lle].unnum.sol_res_frame.format                = GLLC_U_FORMAT;
    gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.cmd_res_M4_M1 = GLLC_DM;
    gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.cmd_or_res_CR = GLLC_RESPONSE;
    gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.xid           = GLLC_XID_NONE;
    gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.poll_final_PF = poll_final_PF;

    /* ................................................................... */

    /* Signal new transmit requirement. */

    gllc_lle[gas_id][lle].unnum.retrx_ctr = 0;
    gllc_lle[gas_id][lle].unnum.cause     = GRR_CAUSE_LLC_SIGNALLING;

    gllc_lle_clr_ul_event_mask_scheme( gas_id, lle, GLLC_LLE_SCHEME_WAIT_LAY3_RESP );

    GLLC_SET_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_U_FR_SRES_EVT );
  }

  /* --------------------------------------------------------------------- */

  else /* GLLC_PF_UNSOL */
  {
    /* Clear unsolicited response U Frame buffer and collate basic
       contents. */

    if ( gllc_lle[gas_id][lle].unnum.unsol_res_frame.u_frame.
           info_field.xid_info.l3_xid.params != NULL )
    {
      GPRS_PDU_FREE(
        &gllc_lle[gas_id][lle].unnum.unsol_res_frame.u_frame.
          info_field.xid_info.l3_xid.params );
    }

    (void)memset
    (
      &gllc_lle[gas_id][lle].unnum.unsol_res_frame,
      0,
      ( sizeof(gllc_frame_t) / sizeof(uint8) )
    );

    gllc_lle[gas_id][lle].unnum.unsol_res_frame.format                = GLLC_U_FORMAT;
    gllc_lle[gas_id][lle].unnum.unsol_res_frame.u_frame.cmd_res_M4_M1 = GLLC_DM;
    gllc_lle[gas_id][lle].unnum.unsol_res_frame.u_frame.cmd_or_res_CR = GLLC_RESPONSE;
    gllc_lle[gas_id][lle].unnum.unsol_res_frame.u_frame.xid           = GLLC_XID_NONE;
    gllc_lle[gas_id][lle].unnum.unsol_res_frame.u_frame.poll_final_PF = poll_final_PF;

    /* ................................................................... */

    /* Signal new transmit requirement. */

    gllc_lle[gas_id][lle].unnum.cause        = GRR_CAUSE_LLC_SIGNALLING;

    GLLC_SET_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_U_FR_URES_EVT );
  }

  /* --------------------------------------------------------------------- */

} /* end of gllc_lle_send_dm_res_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_XID_RES_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required unnumbered
===   XID response message and notifies the MPEX transmit handler about the
===   transmission requirement.
===
===   Layer 3 XID content to be included is given explicitly, whereas
===   layer 2 XID content is derived from the LLE's XID negotiation result
===   store.
===
===  DEPENDENCIES
===
===   Any layer 2 XID negotiation request must have been successfully
===   negotiated and the negotiation results must be lodged in the LLE's
===   XID negotiation result store.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_send_xid_res_mes
(
  gas_id_t       gas_id,
  gllc_ll_sapi_t lle,
  dsm_item_type  **l3_xid_neg
)
{
  /* --------------------------------------------------------------------- */

  /* Clear U Frame buffer and collate basic contents. */

  if ( gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.
         info_field.xid_info.l3_xid.params != NULL )
  {
    GPRS_PDU_FREE(
      &gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.
        info_field.xid_info.l3_xid.params );
  }

  (void)memset
  (
    &gllc_lle[gas_id][lle].unnum.sol_res_frame,
    0,
    ( sizeof(gllc_frame_t) / sizeof(uint8) )
  );

  gllc_lle[gas_id][lle].unnum.sol_res_frame.format                = GLLC_U_FORMAT;
  gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.cmd_res_M4_M1 = GLLC_XID;
  gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.cmd_or_res_CR = GLLC_RESPONSE;
  gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.poll_final_PF = GLLC_PF_FINAL;

  /* --------------------------------------------------------------------- */

  /* Include any layer 2 XID negotiation response in the U Frame buffer. */

  if ( gllc_lle[gas_id][lle].unnum.xid_neg.contents )
  {
    gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.xid = GLLC_XID_NEG;
    gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.info_field.xid_info =
      gllc_lle[gas_id][lle].unnum.xid_neg;

    /* Remove what can't be sent in the uplink direction. */

    gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.info_field.xid_info.contents &=
      ~( GLLC_RESET_XID | GLLC_IOV_UI_XID | GLLC_IOV_I_XID );
  }
  else
    gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.xid = GLLC_XID_NONE;

  /* --------------------------------------------------------------------- */

  /* Include any layer 3 XID parameter negotiation response in the U Frame
     buffer. */

  if ( l3_xid_neg != NULL )
  {
    gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.xid = GLLC_XID_NEG;
    gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.
      info_field.xid_info.contents |= GLLC_LAY3_XID;

    if ( *l3_xid_neg != NULL )
    {
      gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.
        info_field.xid_info.l3_xid.length =
          (uint8)GPRS_PDU_LENGTH( *l3_xid_neg );

      gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.
        info_field.xid_info.l3_xid.params = *l3_xid_neg;

      *l3_xid_neg = NULL;
    }
    else
    {
      gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.
        info_field.xid_info.l3_xid.length = 0;

      gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.
        info_field.xid_info.l3_xid.params = NULL;
    }

    MSG_GERAN_MED_1_G( "GLC LL %d: Sent XID Res L3",
      gllc_sapi_addr_lookup[lle]);
  }
  else
  {
    MSG_GERAN_MED_1_G( "GLC LL %d: Sent XID Res L2",
      gllc_sapi_addr_lookup[lle]);
  }

  gllc_lle_report_l2_xid_content( gas_id, MSG_LEGACY_MED,
    lle, &gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.info_field.xid_info );

  /* --------------------------------------------------------------------- */

  /* Signal new transmit requirement. */

  gllc_lle[gas_id][lle].unnum.retrx_ctr = 0;

  if ( lle == GLLC_LL_SAPI_1_GMM )
    gllc_lle[gas_id][lle].unnum.cause = GRR_CAUSE_GMM_SIGNALLING;
  else
    gllc_lle[gas_id][lle].unnum.cause = GRR_CAUSE_LLC_SIGNALLING;

  gllc_lle_clr_ul_event_mask_scheme( gas_id, lle, GLLC_LLE_SCHEME_WAIT_LAY3_RESP );

  GLLC_SET_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_U_FR_SRES_EVT );

  /* --------------------------------------------------------------------- */

} /* end of gllc_lle_send_xid_res_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SEND_FRMR_RES_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required unnumbered
===   FRMR response message and notifies the MPEX transmit handler about the
===   transmission requirement.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_send_frmr_res_mes
(
  gas_id_t         gas_id,
  gllc_ll_sapi_t   lle,
  gllc_rej_frame_t *rej_frame
)
{
  /* --------------------------------------------------------------------- */

  /* Clear U Frame buffer and collate basic contents. */

  if ( gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.
         info_field.xid_info.l3_xid.params != NULL )
  {
    GPRS_PDU_FREE(
      &gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.
        info_field.xid_info.l3_xid.params );
  }

  (void)memset
  (
    &gllc_lle[gas_id][lle].unnum.sol_res_frame,
    0,
    ( sizeof(gllc_frame_t) / sizeof(uint8) )
  );

  gllc_lle[gas_id][lle].unnum.sol_res_frame.format                = GLLC_U_FORMAT;
  gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.cmd_res_M4_M1 = GLLC_FRMR;
  gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.cmd_or_res_CR = GLLC_RESPONSE;
  gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.xid           = GLLC_XID_NONE;
  gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.poll_final_PF = GLLC_PF_FINAL;

  /* --------------------------------------------------------------------- */

  /* Collate rejected frame information. */

  gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.
    info_field.frmr_info.send_state_var_V_S =
      gllc_lle[gas_id][lle].ack.send_state_var_V_S;

  gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.
    info_field.frmr_info.recv_state_var_V_R =
      gllc_lle[gas_id][lle].ack.recv_state_var_V_R;

  if ( gllc_lle[gas_id][lle].state == GLLC_LLE_ABM )
  {
    gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.
      info_field.frmr_info.reject_status_W4_W1 =
        (rej_frame -> fr_dis_err_map | GLLC_FRMR_W4_IN_ABM) & 0xF;
  }
  else
  {
    gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.
      info_field.frmr_info.reject_status_W4_W1 =
        rej_frame -> fr_dis_err_map & 0xF;
  }

  gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.
    info_field.frmr_info.rejected_frame_CR =
      (gllc_cr_t)( (rej_frame -> rejected_frame_CR == GLLC_COMMAND) ? 0:1 );

  (void)memscpy
  (
    gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.info_field.frmr_info.rej_fr_ctrl_field,
    sizeof(gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.info_field.frmr_info.rej_fr_ctrl_field),
    rej_frame -> rej_fr_ctrl_field,
    6
  );

  /* --------------------------------------------------------------------- */

  MSG_GERAN_HIGH_1("GLC LL %d: Sent FRMR Res", gllc_sapi_addr_lookup[lle]);
  gllc_lle_report_frmr_content(
    gas_id, lle, &gllc_lle[gas_id][lle].unnum.sol_res_frame.u_frame.info_field.frmr_info );

  /* --------------------------------------------------------------------- */

  /* Signal new transmit requirement. */

  gllc_lle[gas_id][lle].unnum.cause = GRR_CAUSE_LLC_SIGNALLING;
  GLLC_SET_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_U_FR_SRES_EVT );

  /* --------------------------------------------------------------------- */

} /* end of gllc_lle_send_frmr_res_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_SEND_GRR_SUSPEND_REQ_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required GRR SAP
===   message and posts this message to the GRLC entity.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_llme_send_grr_suspend_req_mes( gas_id_t gas_id )
{
  gprs_grr_ul_mes_t  *grr_ul_mes;

  grr_ul_mes = GPRS_MEM_MALLOC( sizeof(gprs_grr_ul_mes_t)/sizeof(uint8) );

  if ( grr_ul_mes != NULL )
  {
    MSG_GERAN_MED_0_G( "GLC ME: Sent GRR-SUSPEND-REQ");

    grr_ul_mes -> prim            = GRR_SUSPEND_REQ;
    grr_ul_mes -> tlli            = gllc_llme[gas_id].tlli;
    grr_ul_mes -> peak_throughput = 0;                         /* N/A */
    grr_ul_mes -> radio_priority  = 0;                         /* N/A */
    grr_ul_mes -> llc_pdu_type    = GRR_LL_PDU_NON_ACKSACK;    /* N/A */
    grr_ul_mes -> cause           = GRR_CAUSE_LLC_SIGNALLING;  /* N/A */
    grr_ul_mes -> pdu_ptr         = NULL;                      /* N/A */
    grr_ul_mes -> gas_id          = gas_id;

    grr_ul_mes -> pdu_prio        = GPDU_LOW_PRIORITY;         /* N/A */

    /* Post GRR message to the RLC layer. */

    grlc_llc_ul_put_mes_buf( grr_ul_mes );
  }

} /* end of gllc_llme_send_grr_suspend_req_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_SEND_GRR_RESUME_REQ_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required GRR SAP
===   message and posts this message to the GRLC entity.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_llme_send_grr_resume_req_mes( gas_id_t gas_id )
{
  gprs_grr_ul_mes_t  *grr_ul_mes;

  grr_ul_mes = GPRS_MEM_MALLOC( sizeof(gprs_grr_ul_mes_t)/sizeof(uint8) );

  if ( grr_ul_mes != NULL )
  {
    MSG_GERAN_MED_0_G( "GLC ME: Sent GRR-RESUME-REQ");

    grr_ul_mes -> prim            = GRR_RESUME_REQ;
    grr_ul_mes -> tlli            = gllc_llme[gas_id].tlli;
    grr_ul_mes -> peak_throughput = 0;                         /* N/A */
    grr_ul_mes -> radio_priority  = 0;                         /* N/A */
    grr_ul_mes -> llc_pdu_type    = GRR_LL_PDU_NON_ACKSACK;    /* N/A */
    grr_ul_mes -> cause           = GRR_CAUSE_LLC_SIGNALLING;  /* N/A */
    grr_ul_mes -> pdu_ptr         = NULL;                      /* N/A */
    grr_ul_mes -> gas_id          = gas_id;

    grr_ul_mes -> pdu_prio        = GPDU_LOW_PRIORITY;         /* N/A */

    /* Post GRR message to the RLC layer. */

    grlc_llc_ul_put_mes_buf( grr_ul_mes );
  }

} /* end of gllc_llme_send_grr_resume_req_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_SEND_GRR_PURGE_SUSPENDED_DATA_REQ_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required GRR SAP
===   message and posts this message to the GRLC entity.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_llme_send_grr_purge_suspended_data_req_mes( gas_id_t gas_id )
{
  gprs_grr_ul_mes_t  *grr_ul_mes;

  grr_ul_mes = GPRS_MEM_MALLOC( sizeof(gprs_grr_ul_mes_t)/sizeof(uint8) );

  if ( grr_ul_mes != NULL )
  {
    MSG_GERAN_MED_0_G( "GLC ME: Sent GRR-PURGE-SUSPENDED-DATA-REQ");

    grr_ul_mes -> prim            = GRR_PURGE_SUSPENDED_DATA_REQ;
    grr_ul_mes -> tlli            = gllc_llme[gas_id].tlli;
    grr_ul_mes -> peak_throughput = 0;                         /* N/A */
    grr_ul_mes -> radio_priority  = 0;                         /* N/A */
    grr_ul_mes -> llc_pdu_type    = GRR_LL_PDU_NON_ACKSACK;    /* N/A */
    grr_ul_mes -> cause           = GRR_CAUSE_LLC_SIGNALLING;  /* N/A */
    grr_ul_mes -> pdu_ptr         = NULL;                      /* N/A */
    grr_ul_mes -> gas_id          = gas_id;                    /* N/A */

    grr_ul_mes -> pdu_prio        = GPDU_LOW_PRIORITY;         /* N/A */

    /* Post GRR message to the RLC layer. */

    grlc_llc_ul_put_mes_buf( grr_ul_mes );
  }

} /* end of gllc_llme_send_grr_purge_suspended_data_req_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_SEND_GRR_PURGE_REQ_MES()
===
===  DESCRIPTION
===
===   Collates the necessary information to compose the required GRR SAP
===   message and posts this message to the GRLC entity.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_llme_send_grr_purge_data_req_mes( gas_id_t gas_id, gllc_ll_sapi_t lle )
{
  gprs_grr_ul_mes_t  *grr_ul_mes;

  grr_ul_mes = GPRS_MEM_MALLOC( sizeof(gprs_grr_ul_mes_t)/sizeof(uint8) );

  if ( grr_ul_mes != NULL )
  {
    MSG_GERAN_MED_0_G( "GLC ME: Sent GRR-PURGE-DATA-REQ");

    grr_ul_mes -> prim            = GRR_PURGE_DATA_REQ;
    grr_ul_mes -> tlli            = gllc_llme[gas_id].tlli;
    grr_ul_mes -> peak_throughput = 0;                         /* N/A */
    grr_ul_mes -> radio_priority  = 0;                         /* N/A */
    grr_ul_mes -> llc_pdu_type    = GRR_LL_PDU_NON_ACKSACK;    /* N/A */
    grr_ul_mes -> cause           = GRR_CAUSE_LLC_SIGNALLING;  /* N/A */
    grr_ul_mes -> pdu_ptr         = NULL;                      /* N/A */
    grr_ul_mes -> sapi            = gllc_sapi_addr_lookup[lle];
    grr_ul_mes -> gas_id          = gas_id;

    grr_ul_mes -> pdu_prio        = GPDU_LOW_PRIORITY;         /* N/A */

    /* Post GRR message to the RLC layer. */

    grlc_llc_ul_put_mes_buf( grr_ul_mes );
  }

} /* end of gllc_llme_send_grr_purge_data_req_mes() */



/*** EOF: don't remove! ***/

