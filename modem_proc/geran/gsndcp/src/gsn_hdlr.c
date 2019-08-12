/*****************************************************************************
***
*** TITLE
***
***   SNDCP FUNCTIONALITY HANDLER
***
*** DESCRIPTION
***
***   This module provides the core functionality of SNDCP layer
***
*** EXTERNALIZED FUNCTIONS
***
***   gsn_send_ll_est_req( )
***   gsn_retr_npdu_hdlr( )
***   gsn_sm_prim_hdlr( )
***   gsn_ll_ctrl_prim_hdlr( )
***   gsn_process_ll_data_cnf( )
***   gsn_process_sn_data_req( )
***   gsn_process_ll_data_ind( )
***   gsn_process_sn_udata_req( )
***   gsn_process_ll_udata_ind( )
***   gsn_send_snsm_stat_req( )
***
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***
***
*** Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gsndcp/src/gsn_hdlr.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** mm/dd/yy   xyz   changes
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#include "comdef.h"
#include "gmutex.h"

#define FEATURE_DSM_WM_CB

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST  */

#include <string.h>
#include "stringl.h"
#include "cfa.h"
#include "gs.h"
#include "ms.h"
#include "gllc_v.h"
#include "gllc_v_g.h"
#include "gsni.h"
#include "gsn_hdlr.h"
#include "gsn_util.h"
#include "gprs_mem.h"
#include "gprs_pdu.h"
#include "gsndcp.h"
#include "gsndcp_v_g.h"
#include "gsndcpi.h"
#include "gprsmod.h"
#include "rr_public_store.h" /* read Anite GCF flag */

#include "gsndcplog.h"


#if((defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)))

#ifdef FEATURE_GSM_SNDCP_IPHC
#include "ps_iphc.h"
#endif /* FEATURE_GSM_SNDCP_IPHC */
#include "gsn_comp.h"
#endif

#if(defined(FEATURE_GSM_SNDCP_IPHC) )
#include "sys.h"
#include "sys_v.h"
#endif /* FEATURE_GSM_SNDCP_IPHC || FEATURE_GSM_SNDCP_ROHC */

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


#define MAX_NPDU_SEG_ITEM_CNT  15
#define MAX_NPDU_REF_ITEM_CNT   5


/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/

/* For more information refer to relevant function headers below.
*/
static void gsn_enter_recovery( gas_id_t, gsn_nsapi_e_t * );

static void gsn_ul_suspend_dataflow( gas_id_t, gsn_sapi_t, const uint8 );

static void gsn_clr_nsapi_buf( gas_id_t gas_id, gsn_nsapi_t );

static void gsn_rel_nsapi_buf( gas_id_t gas_id, gsn_nsapi_t nsapi );

static void gsn_clr_sapi_buf( gas_id_t gas_id, gsn_sapi_t );

static void gsn_ul_prepare_retr( gas_id_t, gsn_nsapi_t );

static uint8  gsn_find_nsapi( const gsn_nsapi_q_t*, const gsn_nsapi_t * );

static void gsn_activate_nsapi( gas_id_t, gsnsm_prim_type, const void * );

static void gsn_activate_sapi(  gas_id_t, gsn_sapi_t, uint32 );

static void gsn_map_nsapi_on_sapi( gas_id_t, gsn_nsapi_t, gsn_sapi_t );

static void gsn_init_sapi( gas_id_t gas_id_t, gsn_sapi_e_t *sapi_ptr );

static void gsn_pcomp_dcomp_validity_check(gas_id_t gas_id,
                                           gsn_nsapi_e_t *nsapi_ptr,
                                           gsnsm_prim_type prim_type,
                                           const void *snsm_msg_ptr );
static void gsn_init_nsapi(gas_id_t         gas_id,
                           gsn_nsapi_e_t   *nsapi_ptr,
                           gsnsm_prim_type  prim_type,
                           const void      *snsm_msg_ptr );

static void gsn_deactv_nsapi( gas_id_t gas_id, gsn_nsapi_e_t *nsapi_ptr );

static void gsn_deactv_sapi( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr );

static boolean gsn_release_sapi( gas_id_t, gsn_nsapi_t, gsn_sapi_t, boolean );

static void gsn_remove_nsapi_from_sapi( gas_id_t, gsn_nsapi_t, gsn_sapi_t );

static boolean gsn_ack_nsapis_on_sapi( gas_id_t, gsn_nsapi_t, gsn_sapi_t );

static void gsn_process_ul_ack_npdu ( gas_id_t,
                                      gsn_nsapi_t,
                                      dsm_item_type **,
                                      gsn_npdu_item_t *,
                                      boolean,
                                      uint8,
                                      uint8,
                                      geran_pdu_priority_t
                                     );

static void gsn_process_pdp_dl_data( gas_id_t gas_id, gsn_nsapi_t, dsm_item_type **  );

static boolean gsn_dle_reas_ack_data
(
  gas_id_t gas_id, gsn_nsapi_t, dsm_item_type **, const gsn_data_pdu_hdr_t *
);

static boolean gsn_dle_reas_unack_data
(
  gas_id_t, gsn_nsapi_t, dsm_item_type **, const gsn_unitdata_pdu_hdr_t *
);



gsn_npdu_ref_item_t *gsn_dle_reorder_npdu_seg
(
  gas_id_t               gas_id,
  dsm_item_type          **sn_pdu,
  gsn_unitdata_pdu_hdr_t *sn_hdr
);

gsn_npdu_ref_item_t *gsn_dle_is_npdu_ref_item_in_queue
(
  gas_id_t       gas_id,
  gsn_nsapi_e_t *nsapi_ptr,
  uint16         npdu_num
);

boolean gsn_dle_add_npdu_seg_item
(
  gas_id_t               gas_id,
  dsm_item_type          **sn_pdu,
  gsn_npdu_ref_item_t    *npdu_ref_item_ptr,
  gsn_unitdata_pdu_hdr_t *sn_hdr
);

gsn_npdu_ref_item_t *gsn_dle_add_npdu_ref_item
(
  gas_id_t               gas_id,
  gsn_nsapi_e_t          *nsapi_ptr,
  gsn_unitdata_pdu_hdr_t *sn_hdr
);

gsn_npdu_ref_item_t *gsn_dle_remove_npdu_ref_item
(
  gas_id_t             gas_id,
  gsn_nsapi_e_t       *nsapi_ptr,
  gsn_npdu_ref_item_t *npdu_ref_item_ptr
);

void gsn_dle_free_npdu_ref_item
(
  gas_id_t             gas_id,
  gsn_nsapi_e_t       *nsapi_ptr,
  gsn_npdu_ref_item_t *npdu_ref_item_ptr
);

boolean gsn_dle_is_npdu_reordered
(
  gas_id_t             gas_id,
  gsn_nsapi_e_t       *nsapi_ptr,
  gsn_npdu_ref_item_t *npdu_ref_item_ptr
);



void gsn_unack_retr_npdu_hdlr( gas_id_t gas_id, gsn_nsapi_t nsapi );


/* LL primitive processing functions
*/
static void gsn_send_ll_est_res( gas_id_t, gsn_sapi_e_t *, dsm_item_type *);

static void gsn_send_ll_rel_req( gas_id_t, gsn_sapi_e_t *, boolean );

static void gsn_send_ll_xid_res( gas_id_t, gsn_sapi_e_t *, dsm_item_type *);

static void gsn_process_ll_rst_ind( gas_id_t, gsn_sapi_e_t *, ll_reset_ind_t  *);

static void gsn_process_ll_est_ind( gas_id_t, gsn_sapi_e_t *, ll_establish_ind_t  *);

static void gsn_process_ll_est_cnf( gas_id_t, gsn_sapi_e_t *, ll_establish_cnf_t  *);

static void gsn_process_ll_rel_ind( gas_id_t, gsn_sapi_e_t *, const ll_release_ind_t  *);

static void gsn_process_ll_rel_cnf( gas_id_t, gsn_sapi_e_t *, ll_release_cnf_t  *);

static void gsn_process_ll_xid_ind( gas_id_t, gsn_sapi_e_t *, ll_xid_ind_t  *);

static void gsn_process_ll_xid_cnf( gas_id_t, gsn_sapi_e_t *, ll_xid_cnf_t  *);

static void gsn_process_ll_stat_ind( gas_id_t, gsn_sapi_e_t *, const ll_status_ind_t  *);

static void gsn_process_ll_data_cnf( gas_id_t, gsn_sapi_e_t *, ll_data_cnf_t *);

void gsn_process_ll_udata_cnf( gas_id_t, gsn_sapi_e_t *, ll_unitdata_cnf_t *);


/* SNSM primitive processing functions
*/
static void gsn_process_snsm_actv_ind( gas_id_t, const gsnsm_activ_ind_type * );

static void gsn_process_snsm_deactv_ind( gas_id_t, const gsnsm_deactiv_ind_type * );

static void gsn_process_snsm_mod_ind( gas_id_t, const gsnsm_modify_ind_type * );

static void gsn_process_snsm_seq_ind( gas_id_t, const gsnsm_seq_ind_type * );

static void gsn_send_snsm_actv_res( gas_id_t, gsn_nsapi_t );

static void gsn_send_snsm_deactv_res( gas_id_t, gsn_nsapi_t );

static void gsn_send_snsm_mod_res( gas_id_t, gsn_nsapi_t );

static void gsn_send_snsm_seq_res( gas_id_t, gsn_nsapi_t, gsn_nsapi_mode_t );


/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/

/* Table of active NSAPIs. Order of array elements according to NSAPI number */
gsn_nsapi_e_t   *actv_nsapis[ NUM_GERAN_DATA_SPACES ][ GSN_NSAPI_MAX ];

/* Table of active SAPIs. Order of array elements according to SAPI number */
gsn_sapi_e_t    *actv_sapis[ NUM_GERAN_DATA_SPACES ][GSN_SAPI_MAX];

uint8 suspend_flag;

/* SNDCP to GPRS SAPI address translator. */
const uint8 gsn_extern_sapi[ GSN_SAPI_MAX ] =
{
  3,    /* GPRS LL SAPI 3   */
  5,    /* GPRS LL SAPI 5   */
  9,    /* GPRS LL SAPI 9   */
  11    /* GPRS LL SAPI 11  */
};


/* GPRS to SNDCP internal SAPI address translator. */
const uint8 gsn_intern_sapi[ 12 ] =
{
  GSN_INVAL_SAPI,
  GSN_INVAL_SAPI,
  GSN_INVAL_SAPI,
  (uint8) GSN_SAPI_3,
  GSN_INVAL_SAPI,
  (uint8)GSN_SAPI_5,
  GSN_INVAL_SAPI,
  GSN_INVAL_SAPI,
  GSN_INVAL_SAPI,
  (uint8)GSN_SAPI_9,
  GSN_INVAL_SAPI,
  (uint8)GSN_SAPI_11
};


/* For more information about diagnostic monitoring of SNDCP
** refer to gsndcp.h.
*/
gsndcp_diag_actv_nsapis_rpt_fnc_t actv_nsapis_rpt_fnc = NULL;

gsndcp_diag_nsapi_cntx_rpt_fnc_t  nsapi_cntx_rpt_fnc  = NULL;

gsndcp_diag_nsapi_state_rpt_fnc_t nsapi_state_rpt_fnc = NULL;


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

/*===========================================================================
===
===  FUNCTION      GSN_SEND_LL_ACTIVATE_IND()
===
===  DESCRIPTION
===
===   Sends LL_ACTIVATE_IND to LLC layer. LLC_ACTIVATE_IND primitive conveys
===   SAPI activation to the LLC layer, where not already conveyed via
===   LL_ESTABLISH_REQ or LL_XID_REQ.
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
static void gsn_send_ll_activate_ind( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr )
{
  gsn_nsapi_q_t         *nsapi_q_ptr;
  gsn_sapi_t            sapi = sapi_ptr->sapi;
  gprs_ll_ul_sig_mes_t  *ul_mes;

  /* Switch on SAPI's state */
  switch( sapi_ptr->state )
  {
/*-------------------------------------------------------------------------------*/
  case ( IDLE ):

    /* Get buffer to convey the message */
    ul_mes = gllc_ll_ul_sig_get_mes_buf( );
    GSN_ASSERT( ul_mes != NULL );

    /* load LLC message */
    ul_mes->sapi = gsn_extern_sapi[sapi];
    ul_mes->mes.act_ind_mes.prim = LL_ACTIVATE_IND;
    ul_mes->mes.act_ind_mes.tlli = sapi_ptr->tlli;
    ul_mes->mes.act_ind_mes.gas_id = gas_id;

    if ( sapi_ptr->is_nsapi_ptr_q_valid )
    {

      nsapi_q_ptr = q_check( &sapi_ptr->nsapi_ptr_q );

      if ( nsapi_q_ptr != NULL )
      {
        ul_mes->mes.act_ind_mes.peak_throughput =
          nsapi_q_ptr->nsapi_ptr->qos.peak_throughput;
        ul_mes->mes.act_ind_mes.radio_priority  =
          nsapi_q_ptr->nsapi_ptr->radio_pr;
        ul_mes->mes.act_ind_mes.pfi  =
          nsapi_q_ptr->nsapi_ptr->pfi;
      }
      else
      {
        ul_mes->mes.act_ind_mes.peak_throughput = 0;
        ul_mes->mes.act_ind_mes.radio_priority  = 1;
        ul_mes->mes.act_ind_mes.pfi             = 0;
      }
    }
    else
    {
      ul_mes->mes.act_ind_mes.peak_throughput = 0;
      ul_mes->mes.act_ind_mes.radio_priority  = 1;
      ul_mes->mes.act_ind_mes.pfi             = 0;
      MSG_GERAN_ERROR_2_G("GSN ""LL %d: NSAPI ptr queue invalid in state: %d",
                    gsn_extern_sapi[sapi],sapi_ptr->state);
    }

    /* Post message to LLC and log for call flow analysis. */
    MSG_GERAN_MED_1_G("GSN ""LL %d: Sent LL_ACT_IND",gsn_extern_sapi[sapi]);

    cfa_log_packet_ex
    (
      GS_QUEUE_LLC, MS_LLC_SNDCP, (uint8) LL_ACTIVATE_IND,
      sizeof( ll_activate_ind_t ), (uint8 *)&ul_mes -> mes.act_ind_mes
    );
#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST
    gllc_ll_ul_sig_put_mes_buf( gas_id, ul_mes );
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */
    break;

/*-------------------------------------------------------------------------------*/

  default:

    MSG_GERAN_ERROR_2_G("GSN ""LL %d: Rcvd LL_ACT_IND in state: %d",gsn_extern_sapi[sapi],sapi_ptr->state);
    break;

/*-------------------------------------------------------------------------------*/
  } /* switch switch( sapi_ptr->state ) */
}


/*===========================================================================
===
===  FUNCTION      GSN_SEND_LL_DEACTIVATE_IND()
===
===  DESCRIPTION
===
===   Sends LL_DEACTIVATE_IND to LLC layer. LLC_DEACTIVATE_IND primitive conveys
===   SAPI deactivation to the LLC layer.
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
static void gsn_send_ll_deactivate_ind( gas_id_t gas_id, const gsn_sapi_e_t *sapi_ptr )
{
  gsn_sapi_t            sapi = sapi_ptr->sapi;
  gprs_ll_ul_sig_mes_t  *ul_mes;

  /* Switch on SAPI's state */
  switch( sapi_ptr->state )
  {
/*-------------------------------------------------------------------------------*/
  case ( IDLE ):

    /* Get buffer to convey the message */
    ul_mes = gllc_ll_ul_sig_get_mes_buf( );
    GSN_ASSERT( ul_mes != NULL );

    /* load LLC message */
    ul_mes->sapi = gsn_extern_sapi[sapi];
    ul_mes->mes.deact_ind_mes.prim = LL_DEACTIVATE_IND;
    ul_mes->mes.deact_ind_mes.tlli = sapi_ptr->tlli;
    ul_mes->mes.deact_ind_mes.gas_id = gas_id;

    /* Post message to LLC and log for call flow analysis. */
    MSG_GERAN_MED_1_G("GSN ""LL %d: Sent LL_DEACT_IND",gsn_extern_sapi[sapi]);

    cfa_log_packet_ex
    (
      GS_QUEUE_LLC, MS_LLC_SNDCP, (uint8) LL_DEACTIVATE_IND,
      sizeof( ll_deactivate_ind_t ), (uint8 *)&ul_mes -> mes.deact_ind_mes
    );
#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST
    gllc_ll_ul_sig_put_mes_buf( gas_id, ul_mes );
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */
    break;

/*-------------------------------------------------------------------------------*/

  default:

    MSG_GERAN_ERROR_2_G("GSN ""LL %d: Rcvd LL_DEACT_IND in state: %d",gsn_extern_sapi[sapi],sapi_ptr->state);
    break;

/*-------------------------------------------------------------------------------*/
  } /* switch switch( sapi_ptr->state ) */
}

/*===========================================================================
===
===  FUNCTION      GSN_ENTER_RECOVERY()
===
===  DESCRIPTION
===
===   SNDCP NSAPI Down Link entity enters RECOVERY state forcing the processing of all
===   LL_DATA_IND already present in the SNDCP DL data queue.
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
static void gsn_enter_recovery( gas_id_t gas_id, gsn_nsapi_e_t *nsapi_ptr)
{
  /* Process LL_DATA_IND already present in the SNDCP DL data queue
  ** before entering RECOVERY state.
  */
#ifdef FEATURE_DATA_MM
    (void)gsndcp_process_ll_dl_queue(
      (comp_sig_enum_type)0,
       NULL);
#else
  gsndcp_process_ll_dl_queue();
#endif

  /* Check if re-assembly process has finished. If not, discard re-assembled PDUs */
  if( nsapi_ptr->dle.reas_state != DL_RCV_1ST_SEQ )
  {
    /* Enter Discard state */
    nsapi_ptr->dle.reas_state = DL_RCV_1ST_SEQ;

    if( nsapi_ptr->dle.reas_buff )
    {
      GPRS_PDU_FREE( &nsapi_ptr->dle.reas_buff );
      nsapi_ptr->dle.reas_buff = NULL;
    }
  }
  nsapi_ptr->dle.state = DL_RECOVERY;

  /* Update diag */
  gsn_encode_state_diag_rpt( gas_id, (uint8) nsapi_ptr->nsapi );
}

/*===========================================================================
===
===  FUNCTION      GSN_DLE_REAS_ACK_DATA()
===
===  DESCRIPTION
===
===   Reasembles Ack Down-link SN-DATA PDUs .
===
===   nsapi      : Nsapi number
===   *sn_pdu    : points to SN-DATA PDU
===   *sn_hdr    : points to internal representation of SN-DATA PDU header
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   TRUE if reassembly resulted in a complete N-PDU ready to be send to PDP
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

static boolean gsn_dle_reas_ack_data
(
  gas_id_t                  gas_id,
  gsn_nsapi_t               nsapi,
  dsm_item_type             **sn_pdu,
  const gsn_data_pdu_hdr_t  *sn_hdr
)
{
  gsn_nsapi_e_t   *nsapi_e_ptr;
  boolean         reas_comp = FALSE;

  /* Get active NSAPI entity's pointer and check for its validity */
  nsapi_e_ptr = actv_nsapis[ gas_id ][nsapi];
  if( nsapi_e_ptr == NULL )
  {
    MSG_GERAN_HIGH_1_G("GSN ""NS %d: Invalid pointer", GSN_EXTERN_NSAPI( nsapi ));
    return( reas_comp );
  }

  /* Switch on reassembly state */
  switch( nsapi_e_ptr->dle.reas_state )
  {
  case DL_RCV_1ST_SEQ:
    {
      /* Fisrt segment of an N-PDU expected */
      if( !sn_hdr->f )
      {
        /* Not the first segment discard SN-PDU and re-establish SAPI used.
        */
        MSG_GERAN_HIGH_1_G("GSN ""NS %d: Info 1st Seg expected", GSN_EXTERN_NSAPI( nsapi ));
        GPRS_PDU_FREE( sn_pdu );

        /* Post message to LLC */
        gsn_send_ll_est_req( gas_id, nsapi_e_ptr->sapi_ptr, NULL );

        break;
      }
      else
      {
        /* First segment. If last segment too then N-PDU just one seg. */
        if( !sn_hdr->m )
        {
          /* Store NPDU number. PCOMP and DCOMP will be used to
          ** idendify the decompressor.
          */
          nsapi_e_ptr->dle.first_seg_info.dcomp = sn_hdr->dcomp;
          nsapi_e_ptr->dle.first_seg_info.pcomp = sn_hdr->pcomp;
          nsapi_e_ptr->dle.first_seg_info.ack_npdu_num = sn_hdr->num;

          /* Reassembly buffer points to the SN-PDU */
          nsapi_e_ptr->dle.reas_buff = *sn_pdu;

          /* Delink pointer */
          *sn_pdu = NULL;

          /* Reassembly completed */
          reas_comp = TRUE;
        }
        else
        {
          /* Store 1st seg in to reassembly buffer */
          nsapi_e_ptr->dle.reas_buff = *sn_pdu;

          /* Store DCOMP, PCOMP and N-PDU number of first segment.
          ** These values will be compared against the relevant values of
          ** subsequent segments to avoid any reassembly errors.
          ** Also, PCOMP and DCOMP will be used to idendify the decompressor.
          */
          nsapi_e_ptr->dle.first_seg_info.dcomp = sn_hdr->dcomp;
          nsapi_e_ptr->dle.first_seg_info.pcomp = sn_hdr->pcomp;
          nsapi_e_ptr->dle.first_seg_info.ack_npdu_num = sn_hdr->num;

          /* Get Ready to receive next segment(s) */
          nsapi_e_ptr->dle.reas_state = DL_RCV_SUBS_SEQ;

          /* Delink pointer */
          *sn_pdu = NULL;
        }
      }
    }
    break;

  case DL_RCV_SUBS_SEQ:
    {
      /* Check if F=1 and DCOMP, PCOMP and N-PDU num different
      ** from those in the 1st segment. If so, the ACK LLC operation shall
      ** be re-established. See TS 04.65 par 6.7.4.2
      */
      if( sn_hdr->f )
      {
        if( (nsapi_e_ptr->dle.first_seg_info.dcomp != sn_hdr->dcomp) ||
            (nsapi_e_ptr->dle.first_seg_info.pcomp != sn_hdr->pcomp) ||
            (nsapi_e_ptr->dle.first_seg_info.ack_npdu_num != sn_hdr->num) )
        {
          MSG_GERAN_HIGH_1_G("GSN ""NS %d: Segment header dif from 1st seg", GSN_EXTERN_NSAPI( nsapi ));

          /* Discard SN-PDU and all buffered SN-PDUs
          */
          GPRS_PDU_FREE( sn_pdu );
          GPRS_PDU_FREE( &nsapi_e_ptr->dle.reas_buff );

          /* Get Ready to receive first segment of next N-PDU */
          nsapi_e_ptr->dle.reas_state = DL_RCV_1ST_SEQ;

          /* Post message to LLC */
          gsn_send_ll_est_req( gas_id, nsapi_e_ptr->sapi_ptr, NULL );

          break;
        }
      }

      /* Add seg in to reassembly buffer */
      gprs_pdu_augment_tail( &nsapi_e_ptr->dle.reas_buff, sn_pdu );

      if( !sn_hdr->m )
      {
        /* Last segment received. Reassembly completed */
        reas_comp = TRUE;

        /* Get Ready to receive first segment of next N-PDU */
        nsapi_e_ptr->dle.reas_state = DL_RCV_1ST_SEQ;
      }
    }
    break;

  default:

    MSG_GERAN_ERROR_0_G("GSN ""Invalid sate during reassembly in Unack mode");
    break;
  }

  return( reas_comp );
}


/*===========================================================================
===
===  FUNCTION      GSN_DLE_REAS_UNACK_DATA()
===
===  DESCRIPTION
===
===   Reasembles Unack Down-link SN-UNITDATA PDUs .
===
===   nsapi      : Nsapi number
===   *sn_pdu    : points to SN-UNITDATA PDU
===   *sn_hdr    : points to internal representation of SN-UNITDATA PDU header
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   TRUE if reassembly resulted in a complete N-PDU ready to be send to PDP
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

static boolean gsn_dle_reas_unack_data
(
  gas_id_t                      gas_id,
  gsn_nsapi_t                   nsapi,
  dsm_item_type                 **sn_pdu,
  const gsn_unitdata_pdu_hdr_t  *sn_hdr
)
{
  gsn_nsapi_e_t       *nsapi_e_ptr;
  boolean             reas_comp = FALSE;

  /* Get active NSAPI entity's pointer and check for its validity */
  nsapi_e_ptr = actv_nsapis[ gas_id ][nsapi];
  if( nsapi_e_ptr == NULL )
  {
    MSG_GERAN_ERROR_1_G("GSN ""NS %d: Invalid pointer", GSN_EXTERN_NSAPI( nsapi ));
    return( reas_comp );
  }

  /* Re-ordering of segments belonging to the same N-PDU is not
  ** supported in the first release of SNDCP.
  */
  switch( nsapi_e_ptr->dle.reas_state )
  {
  case DL_RCV_1ST_SEQ:

    /* First segment of an N-PDU expected */
    if( !sn_hdr->f )
    {
      /* Not the first segment. Enter DISCARD sate if "M" bit set to 1 */
      if( sn_hdr->m )
      {
        MSG_GERAN_HIGH_1_G("GSN ""NS %d in DISCARD state", GSN_EXTERN_NSAPI( nsapi ));

        nsapi_e_ptr->dle.reas_state = DL_DISCARD;
      }

      /* Discard SN-PDU and all buffered segments of the N-PDU */
      GPRS_PDU_FREE( sn_pdu );

      GPRS_PDU_FREE( &nsapi_e_ptr->dle.reas_buff );

      break;
    }
    else
    {
      /* First segment. If last segment too then N-PDU just one seg. */
      if( !sn_hdr->m )
      {
        /* Reassembly buffer points to the SN-PDU */
        nsapi_e_ptr->dle.reas_buff = *sn_pdu;

        /* Store first segment parameters. Will be used by the decompressor */
        nsapi_e_ptr->dle.first_seg_info.unack_npdu_num  = sn_hdr->num;
        nsapi_e_ptr->dle.first_seg_info.dcomp           = sn_hdr->dcomp;
        nsapi_e_ptr->dle.first_seg_info.pcomp           = sn_hdr->pcomp;

        /* Delink pointer */
        *sn_pdu = NULL;

        /* reassembly completed */
        reas_comp = TRUE;
      }
      else
      {
        /*  Not last segment. Store seg in to reassembly buffer */
        nsapi_e_ptr->dle.reas_buff = *sn_pdu;

        /* Delink pointer */
        *sn_pdu = NULL;

        /* Store first segment parameters */
        nsapi_e_ptr->dle.first_seg_info.unack_npdu_num  = sn_hdr->num;
        nsapi_e_ptr->dle.first_seg_info.dcomp           = sn_hdr->dcomp;
        nsapi_e_ptr->dle.first_seg_info.pcomp           = sn_hdr->pcomp;

        /* Get Ready to receive next segment(s) */
        nsapi_e_ptr->dle.prv_seg = sn_hdr->seg;
        nsapi_e_ptr->dle.reas_state = DL_RCV_SUBS_SEQ;
      }
    }

    break;

  case DL_RCV_SUBS_SEQ:

    /* increment previous segment number value */
    nsapi_e_ptr->dle.prv_seg = (nsapi_e_ptr->dle.prv_seg + 1) % UNACK_SEG_NUM_MAX;

    /* Check if segment number in sequence with previous
    ** segment number of the same NPDU.
    */
    if( (nsapi_e_ptr->dle.first_seg_info.unack_npdu_num != sn_hdr->num) ||
        (nsapi_e_ptr->dle.prv_seg                       != sn_hdr->seg)    )
    {
      /*  Segment out of sequence. Re-ordering is not supported.
      **  Discard all buffered segments of the N-PDU, and decide whether to
      **  discard the SN-PDU.
      */
      MSG_GERAN_HIGH_3_G("GSN ""NS %d: NPDU %d:%d out of SEQ",
                    GSN_EXTERN_NSAPI( nsapi ),sn_hdr->num, sn_hdr->seg );

      GPRS_PDU_FREE( &nsapi_e_ptr->dle.reas_buff );

      if ( sn_hdr->f && (nsapi_e_ptr->dle.first_seg_info.unack_npdu_num != sn_hdr->num) )
      {
        /* Although this segment is out of sequence, it is the first segment
        ** of a new NPDU. Process this as per the DL_RCV_1ST_SEQ state.
        */

        /* Store seg in to reassembly buffer */
        nsapi_e_ptr->dle.reas_buff = *sn_pdu;

        /* Delink pointer */
        *sn_pdu = NULL;

        /* Store first segment parameters */
        nsapi_e_ptr->dle.first_seg_info.unack_npdu_num  = sn_hdr->num;
        nsapi_e_ptr->dle.first_seg_info.dcomp           = sn_hdr->dcomp;
        nsapi_e_ptr->dle.first_seg_info.pcomp           = sn_hdr->pcomp;

        if( !sn_hdr->m )
        {
          /* Last segment received. Reassembly completed */
          reas_comp = TRUE;
          /* Get Ready to receive first segment of next N-PDU */
          nsapi_e_ptr->dle.reas_state = DL_RCV_1ST_SEQ;
        }
        else
        {
          /* Get Ready to receive next segment(s) */
          nsapi_e_ptr->dle.prv_seg = sn_hdr->seg;
        }
      }

      else if( sn_hdr->m )
      {
        /* More segments to follow, enter DISCARD state */
        nsapi_e_ptr->dle.reas_state = DL_DISCARD;
        GPRS_PDU_FREE( sn_pdu );
      }
      else
      {
        /* Last segment, get ready to receive first segment of next N-PDU */
        nsapi_e_ptr->dle.reas_state = DL_RCV_1ST_SEQ;
        GPRS_PDU_FREE( sn_pdu );
      }

      break;
    }

    /* Check if F=1 and DCOMP and PCOMP different
    ** from 1st segment. See TS 04.65 par 6.7.4.2
    */
    if( sn_hdr->f )
    {
      if( (nsapi_e_ptr->dle.first_seg_info.dcomp != sn_hdr->dcomp) ||
          (nsapi_e_ptr->dle.first_seg_info.pcomp != sn_hdr->pcomp)    )
      {
        /* discard SN-PDU and all the buffered SN-PDUs */
        MSG_GERAN_HIGH_1_G("GSN ""NS %d: Info HDR dif from 1st seg", GSN_EXTERN_NSAPI( nsapi ));
        GPRS_PDU_FREE( sn_pdu );
        GPRS_PDU_FREE( &nsapi_e_ptr->dle.reas_buff );

        if( sn_hdr->m )
        {
          /* More segments to follow, enter DISCARD state */
          nsapi_e_ptr->dle.reas_state = DL_DISCARD;
        }
        else
        {
          /* Last segment, get ready to receive first segment of next N-PDU */
          nsapi_e_ptr->dle.reas_state = DL_RCV_1ST_SEQ;
        }

        break;
      }
    }

    /* Add seg in to reassembly buffer */
    gprs_pdu_augment_tail( &nsapi_e_ptr->dle.reas_buff, sn_pdu );

    if( !sn_hdr->m )
    {
      /* Last segment received. Reassembly completed */
      reas_comp = TRUE;
      /* Get Ready to receive first segment of next N-PDU */
      nsapi_e_ptr->dle.reas_state = DL_RCV_1ST_SEQ;
    }

    break;

  case DL_DISCARD:

    if( !sn_hdr->m )
    {
      /* Last segment received, get ready to receive first segment of next N-PDU */
      nsapi_e_ptr->dle.reas_state = DL_RCV_1ST_SEQ;

      /* Ignore SN-PDU */
      MSG_GERAN_HIGH_1_G("GSN ""NS %d: Info Seg rcved in DISCARD state", GSN_EXTERN_NSAPI( nsapi ));
      GPRS_PDU_FREE( sn_pdu );
    }
    else if ( sn_hdr->f )
    {
      /* Although this segment is out of sequence, it is the first segment
      ** of a new NPDU. Process this as per the DL_RCV_1ST_SEQ state.
      */

      /* Store seg in to reassembly buffer */
      nsapi_e_ptr->dle.reas_buff = *sn_pdu;

      /* Delink pointer */
      *sn_pdu = NULL;

      /* Store first segment parameters */
      nsapi_e_ptr->dle.first_seg_info.unack_npdu_num  = sn_hdr->num;
      nsapi_e_ptr->dle.first_seg_info.dcomp           = sn_hdr->dcomp;
      nsapi_e_ptr->dle.first_seg_info.pcomp           = sn_hdr->pcomp;

      if( !sn_hdr->m )
      {
        /* Last segment received. Reassembly completed */
        reas_comp = TRUE;
        /* Get Ready to receive first segment of next N-PDU */
        nsapi_e_ptr->dle.reas_state = DL_RCV_1ST_SEQ;
      }
      else
      {
        /* Get Ready to receive next segment(s) */
        nsapi_e_ptr->dle.prv_seg = sn_hdr->seg;
        nsapi_e_ptr->dle.reas_state = DL_RCV_SUBS_SEQ;
      }
    }
    else
    {
      /* Ignore SN-PDU */
      MSG_GERAN_HIGH_1_G("GSN ""NS %d: Info Seg rcved in DISCARD state", GSN_EXTERN_NSAPI( nsapi ));
      GPRS_PDU_FREE( sn_pdu );
    }

    break;
  }

  return( reas_comp );
}


/*===========================================================================
===
===  FUNCTION      GSN_DLE_REORDER_NPDU_SEG()
===
===  DESCRIPTION
===
===   Re-orders down-link SN-UNITDATA PDU segments in unack mode
===   (Rel class - 3,4, and 5).
===
===   **sn_pdu : points to SN-UNITDATA PDU segment
===   *sn_hdr  : points to internal representation of SN-UNITDATA PDU header
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   TRUE if re-ordering resulted in a complete NPDU ready to be send to PDP.
===
===  SIDE EFFECTS
===
===   None
===
===========================================================================*/

gsn_npdu_ref_item_t *gsn_dle_reorder_npdu_seg
(
  gas_id_t               gas_id,
  dsm_item_type          **sn_pdu,
  gsn_unitdata_pdu_hdr_t *sn_hdr
)
{
  gsn_npdu_ref_item_t *npdu_ref_item_ptr = NULL;
  gsn_nsapi_e_t       *nsapi_ptr         = actv_nsapis[ gas_id ][sn_hdr->nsapi];
  boolean              is_npdu_reordered = FALSE;;

  /* For this segment, check if corresponding NPDU reference item exists in
  ** the NPDU reference item buffer.
  */
  npdu_ref_item_ptr = gsn_dle_is_npdu_ref_item_in_queue( gas_id,
                                                         nsapi_ptr,
                                                         sn_hdr->num
                                                       );

  if( npdu_ref_item_ptr )
  {
    /* NDPU ref item for the receieved segment exists. Add this segment to the
    ** segment re-order queue attached to this NPDU ref item.
    */
    is_npdu_reordered = gsn_dle_add_npdu_seg_item( gas_id,
                                                   sn_pdu,
                                                   npdu_ref_item_ptr,
                                                   sn_hdr
                                                 );
  }
  else
  {
    /* NDPU ref item for the receieved segment does not exists. Add a new
    ** NPDU ref item to the NPDU ref item queue.
    */
    npdu_ref_item_ptr = gsn_dle_add_npdu_ref_item( gas_id, nsapi_ptr, sn_hdr );

    /* Add received segment to the segment re-order queue of the newly
    ** added NPDU ref item.
    */
    is_npdu_reordered = gsn_dle_add_npdu_seg_item( gas_id,
                                                   sn_pdu,
                                                   npdu_ref_item_ptr,
                                                   sn_hdr
                                                 );
  }

  if( is_npdu_reordered )
  {

#ifdef GSNDCP_DL_REORDER_DEBUG
    MSG_GERAN_ERROR_2_G("GSN ""NS %d: NPDU %d is re-ordered !!!!",
      GSN_EXTERN_NSAPI( sn_hdr->nsapi ), npdu_ref_item_ptr->npdu_num);
#endif

    /* Received NPDU segment completed re-ordering for the NPDU concerned.
    ** Remove NPDU ref item from queue.
    */
    gsn_dle_remove_npdu_ref_item ( gas_id, nsapi_ptr, npdu_ref_item_ptr );
  }
  else
  {

#ifdef GSNDCP_DL_REORDER_DEBUG
    MSG_GERAN_ERROR_2_G("GSN ""NS %d: NPDU %d is not re-ordered !!!!",
      GSN_EXTERN_NSAPI( sn_hdr->nsapi ), npdu_ref_item_ptr->npdu_num);
#endif

    /* Received segment did not result in a completely re-ordered NPDU.
    ** Further segments are expected.
    */
    npdu_ref_item_ptr = NULL;
  }

  return npdu_ref_item_ptr;
}


/*===========================================================================
===
===  FUNCTION      GSN_DLE_IS_NPDU_REF_ITEM_IN_QUEUE()
===
===  DESCRIPTION
===
===   Checks whether indicated NPDU ref item already exists in the queue.
===
===   *nsapi_ptr : points to NSAPI entity
===   npdu_num   : NPDU number of the received segment
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   Pointer to NPDU ref item if it exists in queue. NULL if NPDU
===   ref item does not exist.
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

gsn_npdu_ref_item_t *gsn_dle_is_npdu_ref_item_in_queue
(
  gas_id_t       gas_id,
  gsn_nsapi_e_t *nsapi_ptr,
  uint16         npdu_num
)
{
  gsn_npdu_ref_item_t *npdu_ref_item_ptr = NULL;
  boolean              is_match_found    = FALSE;

  if( nsapi_ptr != NULL )
  {

    /* Check validity of npdu_ref_buff. */

    if( nsapi_ptr->dle.npdu_ref_buff_valid )
    {

      if( q_cnt ( &nsapi_ptr->dle.npdu_ref_buff ) > 0 )
      {

#ifdef GSNDCP_DL_REORDER_DEBUG
        MSG_GERAN_ERROR_1_G("GSN ""NS %d: Ref item queue is not empty",
          GSN_EXTERN_NSAPI( nsapi_ptr->nsapi ));
#endif
        /* Get pointer to the item at the head of ref item queue. */
        npdu_ref_item_ptr = q_check( &nsapi_ptr->dle.npdu_ref_buff );


        /* Traverse ref item queue and find a ref item with the same
        ** npdu number as in the segment just received.
        */
        while( npdu_ref_item_ptr != NULL )
        {
          if ( npdu_ref_item_ptr->npdu_num == npdu_num )
          {

            /* Matching ref item found. Break from loop and return ref
            ** item ptr.
            */
            is_match_found = TRUE;

#ifdef GSNDCP_DL_REORDER_DEBUG
            MSG_GERAN_ERROR_2_G("GSN ""NS %d: Ref item exists for NPDU %d",
              GSN_EXTERN_NSAPI( nsapi_ptr->nsapi ), npdu_num);
#endif
            break;
          }

          npdu_ref_item_ptr = q_next( &nsapi_ptr->dle.npdu_ref_buff,
                                      &npdu_ref_item_ptr->link
                                    );
        }
      }
      else
      {

#ifdef GSNDCP_DL_REORDER_DEBUG
        MSG_GERAN_ERROR_1_G("GSN ""NS %d: Ref item queue is empty",
          GSN_EXTERN_NSAPI( nsapi_ptr->nsapi ));
#endif
      }
    }
    else
    {

      MSG_GERAN_ERROR_1_G("GSN ""NS %d: npdu_ref_buff is invalid",
        GSN_EXTERN_NSAPI( nsapi_ptr->nsapi ));

    }
  }
  else
  {

    MSG_GERAN_ERROR_0_G("GSN ""Invalid NSAPI");

  }

  if( !is_match_found )
  {

#ifdef GSNDCP_DL_REORDER_DEBUG
    MSG_GERAN_ERROR_2_G("GSN ""NS %d: Ref item does not exist for NPDU %d",
      GSN_EXTERN_NSAPI( nsapi_ptr->nsapi ), npdu_num);
#endif

    npdu_ref_item_ptr = NULL;

  }

  return npdu_ref_item_ptr;

}


/*===========================================================================
===
===  FUNCTION      GSN_DLE_ADD_NPDU_SEG_ITEM()
===
===  DESCRIPTION
===
===   Add new segment item to segment re-order queue attached to the indicated
===   NPDU ref item.
===
===   **sn_pdu           : Pointer to the pointer of NPDU segment
===   *npdu_ref_item_ptr : pointer to the NPDU ref item to which NPDU segment
===                        is to be added.
===   *sn_hdr            : pointer to NPDU segment header
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   TRUE  : If re-ordering a NPDU is completed.
===   FALSE : If re-ordering a NPDU is not completed.
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

boolean gsn_dle_add_npdu_seg_item
(
  gas_id_t               gas_id,
  dsm_item_type          **sn_pdu,
  gsn_npdu_ref_item_t    *npdu_ref_item_ptr,
  gsn_unitdata_pdu_hdr_t *sn_hdr
)
{
  gsn_seg_item_t      *cur_npdu_seg_item_ptr  = NULL;
  gsn_seg_item_t      *next_npdu_seg_item_ptr = NULL;
  gsn_seg_item_t      *new_npdu_seg_item_ptr  = NULL;
  boolean              is_npdu_reordered      = FALSE;
  uint8                nsapi;

  if( ( npdu_ref_item_ptr == NULL ) || ( sn_hdr == NULL) )
  {
    MSG_GERAN_ERROR_0_G("GSN "" NPDU ref item ptr and/or sn_hdr is NULL");
  }
  else if ( npdu_ref_item_ptr->seg_reorder_buff_valid )
  {

    nsapi = sn_hdr->nsapi;

    /* Check number of segment items in the queue. If it already has max or
    ** more than max allowed, then it is an invalid NPDU. Remove NPDU ref item
    ** and free contents.
    */

    if( q_cnt( &npdu_ref_item_ptr->seg_reorder_buff ) >= MAX_NPDU_SEG_ITEM_CNT )
    {

      MSG_GERAN_ERROR_2_G("GSN ""NS %d: Max seg item cnt reached, remove NPDU ref item %d",
        GSN_EXTERN_NSAPI( nsapi ), npdu_ref_item_ptr->npdu_num);

      /* Free received segment as it is not a valid segment. */
      GPRS_PDU_FREE ( sn_pdu );

      /* Remove NPDU ref item from queue. */
      gsn_dle_remove_npdu_ref_item( gas_id, actv_nsapis[ gas_id ][nsapi], npdu_ref_item_ptr );

      /* Free NPDU ref item. */
      gsn_dle_free_npdu_ref_item( gas_id, actv_nsapis[ gas_id ][nsapi], npdu_ref_item_ptr );

    }
    else
    {

#ifdef GSNDCP_DL_REORDER_DEBUG
      MSG_GERAN_ERROR_3_G("GSN ""NS %d: Seg item cnt is %d in NPDU ref item %d",
                   GSN_EXTERN_NSAPI( nsapi ),
                   q_cnt( &npdu_ref_item_ptr->seg_reorder_buff ),
                   npdu_ref_item_ptr->npdu_num
                 );
#endif

      /* Allocate memory for new segment item */
      new_npdu_seg_item_ptr = GPRS_MEM_MALLOC( sizeof( gsn_seg_item_t ) );
      GSN_ASSERT( new_npdu_seg_item_ptr != NULL );

      /* Initialse allocated memory. */
      memset( new_npdu_seg_item_ptr, 0, sizeof( gsn_seg_item_t ));

      /* Populate segment item */
      new_npdu_seg_item_ptr->seg_ptr       = *sn_pdu;

      memscpy ( &new_npdu_seg_item_ptr->sn_hdr,
               sizeof (gsn_unitdata_pdu_hdr_t),
               sn_hdr,
               sizeof (gsn_unitdata_pdu_hdr_t)
             );

      /* Insert seg item in seg_reorder_buf. */
      if( q_cnt( &npdu_ref_item_ptr->seg_reorder_buff ) == 0 )
      {

        /* Empty queue, put new segment into segment re-order buffer. */

        /* Initialise the queue link. */
        (void)q_link( new_npdu_seg_item_ptr, &new_npdu_seg_item_ptr->link );

        /* Put new seg item into queue. */
        q_put(&npdu_ref_item_ptr->seg_reorder_buff, &new_npdu_seg_item_ptr->link);

#ifdef GSNDCP_DL_REORDER_DEBUG
        MSG_GERAN_ERROR_3_G("GSN ""NS %d: Added seg item %d to NPDU ref item %d",
                     GSN_EXTERN_NSAPI( nsapi ),
                     new_npdu_seg_item_ptr->sn_hdr.seg,
                     npdu_ref_item_ptr->npdu_num
                   );
#endif

      }
      else
      {

#ifdef GSNDCP_DL_REORDER_DEBUG
        MSG_GERAN_ERROR_3_G("GSN ""NS %d: Seg item queue for NPDU ref item %d has %d items",
                     GSN_EXTERN_NSAPI( nsapi ),
                     npdu_ref_item_ptr->npdu_num,
                     q_cnt( &npdu_ref_item_ptr->seg_reorder_buff )
                   );
#endif

        /* Traverse queue and find appropriate location to insert seg item. */
        cur_npdu_seg_item_ptr = q_check( &npdu_ref_item_ptr->seg_reorder_buff );

        if( cur_npdu_seg_item_ptr == NULL )
        {
          /* To satisfy KW freeing stored memory if not used */
          GPRS_MEM_FREE( new_npdu_seg_item_ptr );
        }

        while( cur_npdu_seg_item_ptr != NULL )
        {

          next_npdu_seg_item_ptr = q_next( &npdu_ref_item_ptr->seg_reorder_buff,
                                           &cur_npdu_seg_item_ptr->link);

          if( next_npdu_seg_item_ptr == NULL )
          {

            /* Either one seg item in queue or at end of queue. Search for correct
            ** location in order to insert in ascending order of seg number
            ** starting at head.
            */
            if( new_npdu_seg_item_ptr->sn_hdr.seg >
                cur_npdu_seg_item_ptr->sn_hdr.seg   )
            {

              /* Initialise segment queue link */
              (void)q_link(new_npdu_seg_item_ptr, &new_npdu_seg_item_ptr->link);

              /* Put new segment item into queue at tail. */
              q_put(&npdu_ref_item_ptr->seg_reorder_buff, &new_npdu_seg_item_ptr->link);

#ifdef GSNDCP_DL_REORDER_DEBUG
              MSG_GERAN_ERROR_3_G("GSN ""new seg item %d inserted after cur seg item %d ",
                           new_npdu_seg_item_ptr->sn_hdr.seg,
                           cur_npdu_seg_item_ptr->sn_hdr.seg,
                           0);
#endif

            }
            else
            {

              /* Insert the new segment item before cur seg item. */
              q_insert(
                &npdu_ref_item_ptr->seg_reorder_buff,
                &(new_npdu_seg_item_ptr->link),
                &(cur_npdu_seg_item_ptr->link) );

#ifdef GSNDCP_DL_REORDER_DEBUG
              MSG_GERAN_ERROR_3_G("GSN ""new seg item %d inserted before cur seg item %d ",
                           new_npdu_seg_item_ptr->sn_hdr.seg,
                           cur_npdu_seg_item_ptr->sn_hdr.seg,
                           0);
#endif

            }
          }
          else
          {

            /* More than one item in queue, add new segment item at appropriate
            ** location in order to insert in ascending order of seg number.
            ** starting at head.
            */
            if( new_npdu_seg_item_ptr->sn_hdr.seg <
                cur_npdu_seg_item_ptr->sn_hdr.seg   )
            {

              /* Insert the new segment item at head. */
              q_insert(
                &npdu_ref_item_ptr->seg_reorder_buff,
                &(new_npdu_seg_item_ptr->link),
                &(cur_npdu_seg_item_ptr->link) );

#ifdef GSNDCP_DL_REORDER_DEBUG
              MSG_GERAN_ERROR_3_G("GSN ""new seg item %d inserted before cur seg item %d ",
                           new_npdu_seg_item_ptr->sn_hdr.seg,
                           cur_npdu_seg_item_ptr->sn_hdr.seg,
                           0);
#endif

              break;

            }
            else if( ( new_npdu_seg_item_ptr->sn_hdr.seg >
                       cur_npdu_seg_item_ptr->sn_hdr.seg  ) &&
                     ( new_npdu_seg_item_ptr->sn_hdr.seg <
                       next_npdu_seg_item_ptr->sn_hdr.seg )    )
            {

              /* Insert the new segment item */
              q_insert(
                &npdu_ref_item_ptr->seg_reorder_buff,
                &(new_npdu_seg_item_ptr->link),
                &(next_npdu_seg_item_ptr->link) );

#ifdef GSNDCP_DL_REORDER_DEBUG
              MSG_GERAN_ERROR_3_G("GSN ""new seg item %d inserted in between cur seg item %d and next seg item %d",
                           new_npdu_seg_item_ptr->sn_hdr.seg,
                           cur_npdu_seg_item_ptr->sn_hdr.seg,
                           next_npdu_seg_item_ptr->sn_hdr.seg);
#endif

              break;

            }
          }

          /* Make next item the current item. */
          cur_npdu_seg_item_ptr = next_npdu_seg_item_ptr;
        }
      }

      /* New segment added. Check if re-ordering this NPDU is completed. */
      is_npdu_reordered =
        gsn_dle_is_npdu_reordered( gas_id, actv_nsapis[ gas_id ][nsapi], npdu_ref_item_ptr );

    }
  }
  else
  {

    MSG_GERAN_ERROR_0_G("GSN ""Seg_item_buff is invalid");

  }

  return is_npdu_reordered;
}


/*===========================================================================
===
===  FUNCTION      GSN_DLE_NPDU_REF_ITEM_ADD()
===
===  DESCRIPTION
===
===   Add NPDU reference item to reference item queue.
===
===   *nsapi_ptr: pointer to NSAPI entity
===   *sn_hdr   : pointer to segment header
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   Pointer to newly added NPDU reference item.
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

gsn_npdu_ref_item_t *gsn_dle_add_npdu_ref_item
(
  gas_id_t               gas_id,
  gsn_nsapi_e_t          *nsapi_ptr,
  gsn_unitdata_pdu_hdr_t *sn_hdr
)
{

  gsn_npdu_ref_item_t *cur_npdu_ref_item_ptr  = NULL;
  gsn_npdu_ref_item_t *new_npdu_ref_item_ptr  = NULL;

  /* Check validity of npdu_ref_buff */
  if( (nsapi_ptr != NULL) &&
      nsapi_ptr->dle.npdu_ref_buff_valid
    )
  {

    /* Check number of NPDU items in the queue. If it already has max or more
    ** than max allowed, then remove NPDU ref item at head of queue and free it.
    */
    if( q_cnt( &nsapi_ptr->dle.npdu_ref_buff ) >= MAX_NPDU_REF_ITEM_CNT )
    {
      /* Remove NPDU ref item from head of queue. */
      cur_npdu_ref_item_ptr = q_get( &nsapi_ptr->dle.npdu_ref_buff );

      if( cur_npdu_ref_item_ptr != NULL )
      {
        MSG_GERAN_ERROR_2_G("GSN ""NS %d: Max NPDU ref item cnt reached, free NPDU %d ",
          GSN_EXTERN_NSAPI( sn_hdr->nsapi ), cur_npdu_ref_item_ptr->npdu_num);
      }

      /* Free NPDU ref item. */
      gsn_dle_free_npdu_ref_item( gas_id, nsapi_ptr, cur_npdu_ref_item_ptr );
    }

    /* Create NPDU ref item */
    new_npdu_ref_item_ptr = GPRS_MEM_MALLOC( sizeof( gsn_npdu_ref_item_t ) );
    GSN_ASSERT( new_npdu_ref_item_ptr != NULL );

    /* Initialse allocated memory. */
    memset( new_npdu_ref_item_ptr, 0, sizeof( gsn_npdu_ref_item_t ));

    new_npdu_ref_item_ptr->npdu_num = sn_hdr->num;

    /* Initialise NPDU segment item queue for this NPDU ref item. */
    (void)q_init( &new_npdu_ref_item_ptr->seg_reorder_buff );

    new_npdu_ref_item_ptr->seg_reorder_buff_valid = TRUE;

    /* Initialise the NPDU ref item queue link. */
    (void)q_link(new_npdu_ref_item_ptr, &(new_npdu_ref_item_ptr->link));

    /* Put new item into NPDU ref item queue at tail. */
    q_put ( &nsapi_ptr->dle.npdu_ref_buff, &new_npdu_ref_item_ptr->link );

#ifdef GSNDCP_DL_REORDER_DEBUG
    MSG_GERAN_ERROR_2_G("GSN ""NS %d: Ref item buffer has %d ref items",
      GSN_EXTERN_NSAPI( sn_hdr->nsapi ), q_cnt( &nsapi_ptr->dle.npdu_ref_buff ));
#endif

  }
  else
  {
    MSG_GERAN_ERROR_1_G("GSN ""NS %d: npdu_ref_buff is invalid",
      GSN_EXTERN_NSAPI( sn_hdr->nsapi ));
  }

  return new_npdu_ref_item_ptr;
}


/*===========================================================================
===
===  FUNCTION      GSN_DLE_REMOVE_NPDU_REF_ITEM()
===
===  DESCRIPTION
===
===   Remove NPDU reference item from reference item queue.
===
===   *nsapi_ptr         : pointer to NSAPI entity
===   *npdu_ref_item_ptr : pointer to NPDU reference item that needs removing.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   Pointer to NPDU reference item which is removed from queue. Returns NULL
===   if specified item is not found.
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

gsn_npdu_ref_item_t *gsn_dle_remove_npdu_ref_item
(
  gas_id_t            gas_id,
  gsn_nsapi_e_t       *nsapi_ptr,
  gsn_npdu_ref_item_t *npdu_ref_item_ptr
)
{

  gsn_npdu_ref_item_t *cur_npdu_ref_item_ptr  = NULL;
  gsn_npdu_ref_item_t *next_npdu_ref_item_ptr = NULL;
  boolean              is_match_found         = FALSE;

  if( ( nsapi_ptr == NULL ) || ( npdu_ref_item_ptr == NULL ) )
  {

    MSG_GERAN_ERROR_0_G("GSN "" NSAPI ptr and/or NPDU ref item ptr invalid ");

  }
  else if( nsapi_ptr->dle.npdu_ref_buff_valid == TRUE )
  {

    /* Pointer to the NPDU ref item at the head of queue */
    cur_npdu_ref_item_ptr = q_check( &nsapi_ptr->dle.npdu_ref_buff );

    if( ( cur_npdu_ref_item_ptr                   == NULL ) ||
        ( q_cnt( &nsapi_ptr->dle.npdu_ref_buff )  == 0    )    )
    {

      /* Queue has gone empty. This cannot happen! */
      MSG_GERAN_ERROR_1_G("GSN ""NS %d: No NPDU ref items in queue ",
        GSN_EXTERN_NSAPI( nsapi_ptr->nsapi ));

    }
    else
    {

      /* Go through the queue, locate, and delete ref item. */
      while( cur_npdu_ref_item_ptr != NULL )
      {

        /* Get pointer to the next item in the queue. */
        next_npdu_ref_item_ptr = q_next( &nsapi_ptr->dle.npdu_ref_buff,
                                         &cur_npdu_ref_item_ptr->link  );

        if( ( cur_npdu_ref_item_ptr->npdu_num  ) ==
            ( npdu_ref_item_ptr->npdu_num      )    )
        {

          is_match_found = TRUE;

          /* Delete indicated NPDU ref item from queue */
          q_delete( &nsapi_ptr->dle.npdu_ref_buff, &cur_npdu_ref_item_ptr->link );

#ifdef GSNDCP_DL_REORDER_DEBUG
          MSG_GERAN_HIGH_3_G("GSN ""NS %d: NPDU ref item for NPDU %d is removed",
                            GSN_EXTERN_NSAPI( nsapi_ptr->nsapi ),
                            cur_npdu_ref_item_ptr->npdu_num,
                            0 );
#endif

          break;

        }

        cur_npdu_ref_item_ptr = next_npdu_ref_item_ptr;
      }
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G("GSN ""NS %d: npdu_ref_buff is invalid ",
                    GSN_EXTERN_NSAPI( nsapi_ptr->nsapi ));
  }

  if( !is_match_found )
  {
    cur_npdu_ref_item_ptr = NULL;
  }

  return cur_npdu_ref_item_ptr;
}


/*===========================================================================
===
===  FUNCTION      GSN_DLE_FREE_NPDU_REF_ITEM()
===
===  DESCRIPTION
===
===   Free indicated NPDU reference item which has been removed from queue.
===
===   *nsapi_ptr         : pointer to NSAPI entity
===   *npdu_ref_item_ptr : pointer to NPDU ref item to be removed.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

void gsn_dle_free_npdu_ref_item
(
  gas_id_t             gas_id,
  gsn_nsapi_e_t       *nsapi_ptr,
  gsn_npdu_ref_item_t *npdu_ref_item_ptr
)
{

  gsn_seg_item_t *npdu_seg_item_ptr  = NULL;

  if( ( nsapi_ptr == NULL ) || ( npdu_ref_item_ptr == NULL ) )
  {

    /* Invalid ref item ptr. This should not happen! */
    MSG_GERAN_ERROR_0_G("GSN ""NSAPI ptr and/or NPDU ref item ptr is NULL ");

  }
  else if( npdu_ref_item_ptr->seg_reorder_buff_valid )
  {

    /* Remove any seg items attached to this NPDU ref item. */
    npdu_seg_item_ptr = q_get( &npdu_ref_item_ptr->seg_reorder_buff );

    /* Go through the seg re-order queue and free seg items. */
    while( npdu_seg_item_ptr != NULL )
    {

#ifdef GSNDCP_DL_REORDER_DEBUG
      MSG_GERAN_HIGH_3_G("GSN ""NS %d: Free seg ref item %d of NPDU %d",
                       GSN_EXTERN_NSAPI( nsapi_ptr->nsapi ),
                       npdu_seg_item_ptr->sn_hdr.seg,
                       npdu_ref_item_ptr->npdu_num );
#endif

      /* Free NPDU segment. */
      GPRS_PDU_FREE ( &npdu_seg_item_ptr->seg_ptr );

      /* Free memory used by NPDU seg item. */
      GPRS_MEM_FREE(npdu_seg_item_ptr);

      /* Get pointer to the next seg item in the queue. */
      npdu_seg_item_ptr = q_get( &npdu_ref_item_ptr->seg_reorder_buff );
    }

    /* Destroy segment item queue. */
    q_destroy ( &npdu_ref_item_ptr->seg_reorder_buff );

#ifdef GSNDCP_DL_REORDER_DEBUG
    MSG_GERAN_HIGH_2_G("GSN ""NS %d: Free NPDU ref item %d ",
      GSN_EXTERN_NSAPI( nsapi_ptr->nsapi ), npdu_ref_item_ptr->npdu_num);
#endif

    /* Free memory used by NPDU ref item. */
    GPRS_MEM_FREE( npdu_ref_item_ptr);

    npdu_ref_item_ptr = NULL;

  }
  else
  {

    MSG_GERAN_ERROR_1_G("GSN ""NS: %d: NPDU seg reorder buff invalid ",
                    GSN_EXTERN_NSAPI( nsapi_ptr->nsapi ));

  }

}


/*===========================================================================
===
===  FUNCTION      GSN_DLE_IS_NPDU_REORDERED()
===
===  DESCRIPTION
===
===   Checks if a given NPDU in segment re-order buffer is fully re-ordered.
===
===   *npdu_ref_item_ptr: pointer to NPDU reference item
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   TRUE if NPDU is fully re-ordered. FALSE otherwise.
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

boolean gsn_dle_is_npdu_reordered( gas_id_t             gas_id,
                                   gsn_nsapi_e_t       *nsapi_ptr,
                                   gsn_npdu_ref_item_t *npdu_ref_item_ptr )
{
  gsn_seg_item_t *cur_npdu_seg_item_ptr;
  gsn_seg_item_t *next_npdu_seg_item_ptr;
  boolean         is_npdu_reordered    = FALSE;
  boolean         is_first_npdu_seg_ok = FALSE;
  boolean         is_last_npdu_seg_ok  = FALSE;
  boolean         npdu_seg_num_gaps    = FALSE;


  if( ( nsapi_ptr == NULL ) || ( npdu_ref_item_ptr == NULL ) )
  {

    MSG_GERAN_ERROR_0_G("GSN ""NSAPI ptr and/or NPDU ref item ptr is NULL ");

  }
  else if( ( q_cnt( &npdu_ref_item_ptr->seg_reorder_buff ) == 0 ) ||
           ( q_cnt( &npdu_ref_item_ptr->seg_reorder_buff ) > 15 ) )
  {

    /* Invalid NPDU. Remove and free NPDU item and all attached segments,
    ** if any.
    */

    MSG_GERAN_ERROR_2_G("GSN ""NS %: Invalid number of seg items, free NPDU ref item %d",
      GSN_EXTERN_NSAPI( nsapi_ptr->nsapi ), npdu_ref_item_ptr->npdu_num);

    gsn_dle_remove_npdu_ref_item( gas_id, nsapi_ptr, npdu_ref_item_ptr );

    gsn_dle_free_npdu_ref_item( gas_id, nsapi_ptr, npdu_ref_item_ptr );

  }
  else
  {

    if( q_cnt( &npdu_ref_item_ptr->seg_reorder_buff ) == 1 )
    {
      /* If there is only one item in segment queue, check if it is the
      ** first and the last. If so, indicate re-ordering completed.
      */
      cur_npdu_seg_item_ptr = q_check( &npdu_ref_item_ptr->seg_reorder_buff );

      if( cur_npdu_seg_item_ptr != NULL )
      {
        if( ( cur_npdu_seg_item_ptr->sn_hdr.f  ) &&
            ( !cur_npdu_seg_item_ptr->sn_hdr.m )    )
        {
          is_npdu_reordered = TRUE;
        }
      }
    }
    else
    {
      /* More than one segment item in queue. Check if re-ordering is
      ** completed.
      */
      cur_npdu_seg_item_ptr = q_check( &npdu_ref_item_ptr->seg_reorder_buff );

      if( cur_npdu_seg_item_ptr != NULL )
      {
        if( ( cur_npdu_seg_item_ptr->sn_hdr.f ) &&
            ( cur_npdu_seg_item_ptr->sn_hdr.m )    )
        {
          is_first_npdu_seg_ok = TRUE;
        }
      }

      /* Check if last NPDU segment meet criteria. */
      while( cur_npdu_seg_item_ptr != NULL )
      {

        /* Get pointer to the next seg item in the queue. */
        next_npdu_seg_item_ptr = q_next( &npdu_ref_item_ptr->seg_reorder_buff,
                                         &cur_npdu_seg_item_ptr->link
                                       );

        if( ( next_npdu_seg_item_ptr == NULL ) &&
            ( (!cur_npdu_seg_item_ptr->sn_hdr.f ) &&
              (!cur_npdu_seg_item_ptr->sn_hdr.m )    )
          )
        {
          is_last_npdu_seg_ok = TRUE;
        }

        cur_npdu_seg_item_ptr = next_npdu_seg_item_ptr;

      }


      /* Check if there are gaps in segment sequence numbers. */
      cur_npdu_seg_item_ptr = q_check( &npdu_ref_item_ptr->seg_reorder_buff );

      while( cur_npdu_seg_item_ptr != NULL )
      {

        /* Get pointer to the next seg item in the queue. */
        next_npdu_seg_item_ptr = q_next( &npdu_ref_item_ptr->seg_reorder_buff,
                                         &cur_npdu_seg_item_ptr->link        );

        if ( next_npdu_seg_item_ptr != NULL )
        {

          if ( ( next_npdu_seg_item_ptr->sn_hdr.seg -
                 cur_npdu_seg_item_ptr->sn_hdr.seg    ) != 1 )
          {
            npdu_seg_num_gaps = TRUE;
          }

        }

        cur_npdu_seg_item_ptr = next_npdu_seg_item_ptr;

      }


      if( is_first_npdu_seg_ok && is_last_npdu_seg_ok && !npdu_seg_num_gaps )
      {
        is_npdu_reordered = TRUE;
      }
    }
  }

  return is_npdu_reordered;
}


/*===========================================================================
===
===  FUNCTION      GSN_UL_SUSPEND_DATAFLOW()
===
===  DESCRIPTION
===
===   Suspends PDP UL data flow for all the NSAPIs mapped on the SAPI
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/
static void gsn_ul_suspend_dataflow( gas_id_t gas_id, gsn_sapi_t sapi, const uint8 event )
{
  gsn_sapi_e_t    *sapi_e_ptr = actv_sapis[gas_id][sapi];
  gsn_nsapi_q_t   *item_ptr;

  if( sapi_e_ptr != NULL )
  {
    if ( sapi_e_ptr->is_nsapi_ptr_q_valid )
    {
      item_ptr = q_check( &(sapi_e_ptr->nsapi_ptr_q) );
      if( item_ptr )
      {
        while( item_ptr != NULL )
        {
          /* Set Suspend flag  */
          GSN_SET_SUSP_EVENT( gas_id, (item_ptr->nsapi_ptr->nsapi), event );

          /* Get next NSAPI  */
          item_ptr = q_next( &(sapi_e_ptr->nsapi_ptr_q), &(item_ptr->link) );
        }
      }
      else
      {
        MSG_GERAN_ERROR_1_G("GSN ""LL %d: No mapped NSAPIs",gsn_extern_sapi[sapi]);
      }
    }
    else
    {
      MSG_GERAN_ERROR_1_G("GSN ""LL %d: No mapped NSAPIs",gsn_extern_sapi[sapi]);
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G("GSN ""LL %d: Invalid SAPI ptr",gsn_extern_sapi[sapi]);
  }
}


/*===========================================================================
===
===  FUNCTION      GSN_UL_CLR_EST_SUSP()
===
===  DESCRIPTION
===
===   Clear GSN_UL_SUSP_EST_PEND for all the NSAPIs mapped on the SAPI
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

void gsn_ul_clr_est_susp( gas_id_t gas_id, uint8 sapi )
{

  gsn_nsapi_q_t          *item_ptr;
  gsn_sapi_e_t           *sapi_e_ptr;
  gsn_sapi_t sapi_int  = gsn_intern_sapi[ sapi ];


  GSN_LOCK();

  sapi_e_ptr = actv_sapis[ gas_id ][ sapi_int ];

  if( sapi_e_ptr != NULL )
  {
    if ( sapi_e_ptr->is_nsapi_ptr_q_valid )
    {
      item_ptr = q_check( &(sapi_e_ptr->nsapi_ptr_q) );
      if( item_ptr )
      {
        while( item_ptr != NULL )
        {
          /* Set Suspend flag  */
          GSN_CLR_SUSP_EVENT( gas_id, (item_ptr->nsapi_ptr->nsapi), GSN_UL_SUSP_EST_PEND );

          /* Get next NSAPI  */
          item_ptr = q_next( &(sapi_e_ptr->nsapi_ptr_q), &(item_ptr->link) );
        }
      }
      else
      {
        MSG_GERAN_ERROR_1_G("GSN ""LL %d: No mapped NSAPIs",gsn_extern_sapi[sapi]);
      }
    }
    else
    {
      MSG_GERAN_ERROR_1_G("GSN ""LL %d: No mapped NSAPIs",gsn_extern_sapi[sapi]);
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G("GSN ""LL %d: Invalid SAPI ptr",gsn_extern_sapi[sapi]);
  }

  GSN_UNLOCK();
}


/*===========================================================================
===
===  FUNCTION      GSN_CLR_NSAPI_BUF()
===
===  DESCRIPTION
===
===   Clears (deallocates memory) NSAPI's N-PDU buffer, segment reference buffer,
===   and Down-link reassembly buffer.
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

static void gsn_clr_nsapi_buf( gas_id_t gas_id, gsn_nsapi_t nsapi )
{
  gsn_nsapi_e_t       *nsapi_e_ptr = actv_nsapis[ gas_id ][nsapi];
  gsn_npdu_item_t     *npdu_item_ptr;
  gsn_seg_ref_item_t  *ref_item_ptr;

  gsn_npdu_ref_item_t *npdu_ref_item_ptr;

  if( nsapi_e_ptr != NULL )
  {
    /* Suspend LLC task to prevent processing of LL_DATA_CNF
    ** that could modify the contents of the npdu_buff.
    */
    GSN_LOCK();

    /* If npdu_buff queue is valid, then access it.*/
    if( nsapi_e_ptr->ule.npdu_buff_valid == TRUE )
    {

      /* For every buffered N-PDU clear buffered reference info of SN-PDUs */
      npdu_item_ptr = q_get( &(nsapi_e_ptr->ule.npdu_buff) );

      while( npdu_item_ptr )
      {
        if( npdu_item_ptr->seg_ref_buff_valid == TRUE )
        {
          ref_item_ptr = q_get( &(npdu_item_ptr->seg_ref_buff) );
          while( ref_item_ptr )
          {
            /* Free memory used for queuing the item */
            GSN_MEM_FREE( ref_item_ptr );

            /* Get next item */
            ref_item_ptr = q_get( &(npdu_item_ptr->seg_ref_buff) );
          }

          /* Set seg_ref_buff as invalid for this NPDU */
          npdu_item_ptr->seg_ref_buff_valid = FALSE;

          /* Free memory,mutex etc. used by NPDU segment queue attached to NPDU */
          q_destroy ( &npdu_item_ptr->seg_ref_buff );
        }
        else
        {
          MSG_GERAN_MED_0_G("GSN ""No seg ref buff");
        }

        /* Free pdu item back to DSM mem pool */
        if( npdu_item_ptr->npdu_ptr )
          GPRS_PDU_FREE( &npdu_item_ptr->npdu_ptr );

        /* Free memory used for queuing the item */
        GSN_MEM_FREE( npdu_item_ptr );

        /* Get next item */
        npdu_item_ptr = q_get( &(nsapi_e_ptr->ule.npdu_buff) );
      }
    }
    else
    {
      MSG_GERAN_MED_0_G("GSN ""No npdu buff");
    }

    /* Free contents of npdu ref item and npdu_seg_item buffers and destroy
    ** relevant queues.
    */

    if ( nsapi_e_ptr->dle.npdu_ref_buff_valid )
    {
      npdu_ref_item_ptr = q_get( &(nsapi_e_ptr->dle.npdu_ref_buff) );

      while( npdu_ref_item_ptr )
      {

        /* Free this NPDU ref item and any seg items attached. */
        gsn_dle_free_npdu_ref_item( gas_id, nsapi_e_ptr, npdu_ref_item_ptr );

        npdu_ref_item_ptr = q_get( &(nsapi_e_ptr->dle.npdu_ref_buff) );

      }
    }



    /* Resume LLC task */
    GSN_UNLOCK();

    /* Free memory used by reassembly buffer */
    if( nsapi_e_ptr->dle.reas_buff )
      GPRS_PDU_FREE( &nsapi_e_ptr->dle.reas_buff );
  }
  else
  {
     MSG_GERAN_ERROR_2_G("GSN ""NS %d: Invalid NSAPI ptr",
      GSN_EXTERN_NSAPI(nsapi),0 );
  }
}


/*===========================================================================
===
===  FUNCTION      GSN_REL_NSAPI_BUF()
===
===  DESCRIPTION
===
===   Releases (destroys) NSAPI's N-PDU retx and N-PDU reference buffers which
===   are queues. This should only be called when NSAPI entity is no longer
===   required, i.e. PDP context is deactivated etc.
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

static void gsn_rel_nsapi_buf( gas_id_t gas_id, gsn_nsapi_t nsapi )
{
  gsn_nsapi_e_t *nsapi_e_ptr = actv_nsapis[ gas_id ][nsapi];

  if( nsapi_e_ptr != NULL )
  {
    /* Set flag to indicate that npdu_buff queue is invalid. */
    nsapi_e_ptr->ule.npdu_buff_valid = FALSE;

    /* Free memory, mutex etc. used by retx NPDU queue attached to NSAPI. */
    q_destroy ( &nsapi_e_ptr->ule.npdu_buff );


    /* Set flag to indicate that npdu_ref_buff queue is invalid. */
    nsapi_e_ptr->dle.npdu_ref_buff_valid = FALSE;

    /* Free memory, mutex etc. used by NPDU ref item queue attached to NSAPI.*/
    q_destroy ( &nsapi_e_ptr->dle.npdu_ref_buff );


  }
  else
  {
    MSG_GERAN_ERROR_2_G("GSN " "NS %d: Invalid NSAPI ptr", GSN_EXTERN_NSAPI(nsapi),0 );
  }
}


/*===========================================================================
===
===  FUNCTION      GSN_CLR_SAPI_BUF()
===
===  DESCRIPTION
===
===   Clears (deallocates memory) SAPI's dynamically allocated buffers.
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

static void gsn_clr_sapi_buf( gas_id_t gas_id, gsn_sapi_t sapi )
{
  gsn_sapi_e_t    *sapi_ptr = actv_sapis[gas_id][sapi];
  gsn_nsapi_q_t   *item_ptr;

  if( sapi_ptr != NULL )
  {

    if( sapi_ptr->is_nsapi_ptr_q_valid )
    {

      /* Delete all pointers of NSAPI entities linked to the SAPI
      */
      item_ptr = q_get( &sapi_ptr->nsapi_ptr_q );

      while( item_ptr != NULL )
      {
        /* Free memmory used for queuing the item */
        GSN_MEM_FREE( item_ptr );

        /* get next item */
        item_ptr = q_get( &sapi_ptr->nsapi_ptr_q );
      }

      /* Invalidate NSAPI pointer queue. */
      sapi_ptr->is_nsapi_ptr_q_valid = FALSE;

      /* Free memory, mutex etc. used by NSAPI queue attached to the SAPI. */
      q_destroy ( &sapi_ptr->nsapi_ptr_q );

    }
    else
    {
      MSG_GERAN_ERROR_2_G("GSN ""LL %d: NSAPI ptr queue invalid", gsn_extern_sapi[sapi],0 );
    }

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )

    /* Points to the latest requested XID block. */
    if( sapi_ptr->xid_req_dup_ptr)
       GPRS_PDU_FREE( &sapi_ptr->xid_req_dup_ptr );

#endif

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP
    {
      t2_alg_q_t *t2_item_ptr;
      uint8       alg, ent;

      for(alg = 0; alg <= T2_ALG_MAX; alg++ )
      {
        t2_item_ptr = q_get( &sapi_ptr->t2_alg_q[alg] );

        while( t2_item_ptr )
        {
          if( (alg == T2_ALG_RFC1144) &&
              (t2_item_ptr->pcomp_e_ptr->t2_alg == T2_ALG_RFC1144) )
          {
            if( sapi_ptr->pcomp_pool[t2_item_ptr->pcomp_e_ptr->alg.xid_rfc1144.pcomp1] )
              GSN_MEM_FREE( sapi_ptr->
                pcomp_pool[t2_item_ptr->pcomp_e_ptr->alg.xid_rfc1144.pcomp1] );

            if( sapi_ptr->pcomp_pool[t2_item_ptr->pcomp_e_ptr->alg.xid_rfc1144.pcomp2] )
              GSN_MEM_FREE( sapi_ptr->
                pcomp_pool[t2_item_ptr->pcomp_e_ptr->alg.xid_rfc1144.pcomp2] );

            MSG_GERAN_MED_2_G("GSN ""XID %d: Rmv PCOMP entity %d",
              gsn_extern_sapi[sapi],t2_item_ptr->pcomp_e_ptr->ent_num);

            gsn_slhc_free_mem(gas_id, (slhc_handle_type*)t2_item_ptr->pcomp_e_ptr->comp_e );
          }
          else if((alg == T2_ALG_RFC2507) &&
                  (t2_item_ptr->pcomp_e_ptr->t2_alg == T2_ALG_RFC2507) )
          {
            if( sapi_ptr->pcomp_pool[t2_item_ptr->pcomp_e_ptr->alg.xid_rfc2507.pcomp1] )
              GSN_MEM_FREE( sapi_ptr->
                pcomp_pool[t2_item_ptr->pcomp_e_ptr->alg.xid_rfc2507.pcomp1] );

            if( sapi_ptr->pcomp_pool[t2_item_ptr->pcomp_e_ptr->alg.xid_rfc2507.pcomp2] )
              GSN_MEM_FREE( sapi_ptr->
                pcomp_pool[t2_item_ptr->pcomp_e_ptr->alg.xid_rfc2507.pcomp2] );

            if( sapi_ptr->pcomp_pool[t2_item_ptr->pcomp_e_ptr->alg.xid_rfc2507.pcomp3] )
              GSN_MEM_FREE( sapi_ptr->
                pcomp_pool[t2_item_ptr->pcomp_e_ptr->alg.xid_rfc2507.pcomp3] );

            if( sapi_ptr->pcomp_pool[t2_item_ptr->pcomp_e_ptr->alg.xid_rfc2507.pcomp4] )
              GSN_MEM_FREE( sapi_ptr->
                pcomp_pool[t2_item_ptr->pcomp_e_ptr->alg.xid_rfc2507.pcomp4] );

            if( sapi_ptr->pcomp_pool[t2_item_ptr->pcomp_e_ptr->alg.xid_rfc2507.pcomp5] )
              GSN_MEM_FREE( sapi_ptr->
                pcomp_pool[t2_item_ptr->pcomp_e_ptr->alg.xid_rfc2507.pcomp5] );

            gsn_iphc_free(gas_id, t2_item_ptr->pcomp_e_ptr->comp_e);
          }
          else if ((alg == T2_ALG_RFC3095) &&
                   (t2_item_ptr->pcomp_e_ptr->t2_alg == T2_ALG_RFC3095) )
          {
            if( sapi_ptr->pcomp_pool[t2_item_ptr->pcomp_e_ptr->alg.xid_rfc3095.pcomp1] )
              GSN_MEM_FREE( sapi_ptr->
                pcomp_pool[t2_item_ptr->pcomp_e_ptr->alg.xid_rfc3095.pcomp1] );

            if( sapi_ptr->pcomp_pool[t2_item_ptr->pcomp_e_ptr->alg.xid_rfc3095.pcomp2] )
              GSN_MEM_FREE( sapi_ptr->
                pcomp_pool[t2_item_ptr->pcomp_e_ptr->alg.xid_rfc3095.pcomp2] );

            gsn_rohc_free(gas_id, t2_item_ptr->pcomp_e_ptr->comp_e);
          }
          else
            MSG_GERAN_ERROR_2_G("GSN "" Invalid T2 algorithm alg %d, t2_alg %d",
                            alg,t2_item_ptr->pcomp_e_ptr->t2_alg);

          /* Free memory used for queuing the T2 compression entity */
          GSN_MEM_FREE( t2_item_ptr );

          /* Get next entity from the queue */
          t2_item_ptr = q_get( &sapi_ptr->t2_alg_q[alg] );
        }

        /* Free memory, mutex etc. used by t2 alg queue attached to the SAPI.*/
        q_destroy ( &sapi_ptr->t2_alg_q[alg] );
      }

      for( ent = 0; ent <= ENT_NUM_MAX; ent++ )
      {
        if(sapi_ptr->pcomp_e_pool[ent] != NULL )
        {
          /* Free memory used in the entity pool */
          GSN_MEM_FREE( sapi_ptr->pcomp_e_pool[ent] );
        }
      }
    }

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP
    {
      t1_alg_q_t *t1_item_ptr;
      /* Change to uint8 when more data compression algorithms added and
      ** T1_ALG_MAX != 0. int8 is used to avoid unassigned comparison with 0 warning.
      */
      int8            alg, ent;

      for( alg = 0; alg <= T1_ALG_MAX; alg++ )
      {
        t1_item_ptr = q_get( &sapi_ptr->t1_alg_q[alg] );

        while( t1_item_ptr )
        {
          if( (alg == T1_ALG_V42BIS) &&
              (t1_item_ptr->dcomp_e_ptr->t1_alg == T1_ALG_V42BIS) )
          {
            if( sapi_ptr->dcomp_pool[t1_item_ptr->dcomp_e_ptr->alg.xid_v42bis.dcomp1] )
              GSN_MEM_FREE( sapi_ptr->
                dcomp_pool[t1_item_ptr->dcomp_e_ptr->alg.xid_v42bis.dcomp1] );

            gsn_v42_dealloc_tables( gas_id, &t1_item_ptr->dcomp_e_ptr->comp_e, GSN_V42_BOTH_DIR );

            MSG_GERAN_MED_2_G("GSN ""XID %d: Rmv DCOMP entity %d",
              gsn_extern_sapi[sapi],t1_item_ptr->dcomp_e_ptr->ent_num);

            /* Put entity back to the pool of unassigned entities */
            GSN_MEM_FREE( sapi_ptr->dcomp_e_pool[t1_item_ptr->dcomp_e_ptr->ent_num] );
          }
          else
            MSG_GERAN_ERROR_2_G("GSN "" Invalid T1 algorithm alg %d, t1_alg %d",
                            alg,t1_item_ptr->dcomp_e_ptr->t1_alg);

          /* Free memory used for queuing the T1 compression entity */
          GSN_MEM_FREE( t1_item_ptr );

          /* Get next entity from the queue */
          t1_item_ptr = q_get( &sapi_ptr->t1_alg_q[alg] );
        }

        /* Free memory, mutex etc. used by t1 alg queue attached to the SAPI.*/
        q_destroy ( &sapi_ptr->t1_alg_q[alg] );
      }

      for( ent = 0; ent <= ENT_NUM_MAX; ent++ )
      {
        if(sapi_ptr->dcomp_e_pool[ent] != NULL )
        {
          /* Free memory used in the entity pool */
          GSN_MEM_FREE( sapi_ptr->dcomp_e_pool[ent] );
        }
      }
    }
#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */
  }
  else
  {
     MSG_GERAN_ERROR_2_G("GSN ""LL %d: Invalid SAPI ptr", gsn_extern_sapi[sapi],0 );
  }
}

/*===========================================================================
===
===  FUNCTION      gsn_ul_prepare_retr()
===
===  DESCRIPTION
===
===   Checks NSAPI's retransmission buffer and determines
===   if retransmission of buffered N-PDUs should start.
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/
static void gsn_ul_prepare_retr( gas_id_t gas_id, gsn_nsapi_t nsapi )
{
  gsn_nsapi_e_t       *nsapi_e_ptr = actv_nsapis[ gas_id ][nsapi];
  gsn_seg_ref_item_t  *ref_item_ptr;
  gsn_npdu_item_t     *npdu_item_ptr;
  uint8               max_npdu_susp;
  uint8               seq_pend_susp;

  /* Check NSAPI pointer validity. If NULL, NSAPI does not exist and retx
  ** cannot proceed.
  */
  if( nsapi_e_ptr == NULL )
  {
    MSG_GERAN_ERROR_1_G("GSN " " NSAPI is invalid %d", GSN_EXTERN_NSAPI(nsapi));
  }
  else
  {

    /* Check if there are buffered N-PDUs that have not be confirmed */
    npdu_item_ptr = q_check( &(nsapi_e_ptr->ule.npdu_buff) );

    if( npdu_item_ptr )
    {
      /* There are buffered N-PDUs that have not be confirmed
      ** by peer entity. Clear Segment Reference buffers and prepare
      ** to re-transmit the buffered N-PDUs starting with oldest first
      */

      /* For every buffered N-PDU clear buffered reference info of SN-PDUs */
      while( npdu_item_ptr != NULL )
      {
        npdu_item_ptr->re_tr_flag = FALSE;

        npdu_item_ptr->npdu_acked = FALSE;

        if( npdu_item_ptr->seg_ref_buff_valid == TRUE )
        {

          /* Delete all bufferd reference info of SN-PDUs */
          ref_item_ptr = q_get( &(npdu_item_ptr->seg_ref_buff) );

          while( ref_item_ptr != NULL )
          {
            /* Free memory and get next item */
            GSN_MEM_FREE( ref_item_ptr );
            ref_item_ptr = q_get( &(npdu_item_ptr->seg_ref_buff) );
          }

          /* Set seg_ref_buff as invalid for this NPDU */
          npdu_item_ptr->seg_ref_buff_valid = FALSE;

          /* Free memory,mutex etc. used by NPDU segment queue attached to
          ** NPDU.
          */
          q_destroy ( &npdu_item_ptr->seg_ref_buff );
        }
        else
        {
          MSG_GERAN_MED_0_G("GSN ""No seg ref buff");
        }

        /* Get next N-PDU */
        npdu_item_ptr = q_next( &(nsapi_e_ptr->ule.npdu_buff), &(npdu_item_ptr->link) );
      }

      /* Re-transmit N-PDUs if UL data flow is not suspended */
      GSN_LOCK();
      max_npdu_susp = (nsapi_e_ptr->ule.susp_flag & GSN_UL_SUSP_MAX_NPDU_MASK);
      seq_pend_susp = (nsapi_e_ptr->ule.susp_flag & GSN_UL_SUSP_SEQ_PEND);
      GSN_UNLOCK();

      if( max_npdu_susp )
      {
        /* Data flow is suspended. Set flag so retransmission
        ** occurs when data flow is resumed. If SNSM_SEQUENSE_IND
        ** is pending, do not set the re-transmission flag.
        ** Re-transmission will be triggered after receiving the
        ** SNSM_SEQUENSE_IND primitive.
        */
        if( !seq_pend_susp )
          nsapi_e_ptr->ule.retr = TRUE;
      }
      else
      {
        /* Start retransmission */
        gsn_retr_npdu_hdlr( gas_id, nsapi );
      }
    }
  }
}

/*===========================================================================
===
===  FUNCTION      GSN_FIND_NSAPI()
===
===  DESCRIPTION
===
===   Compare function for Q_LINEAR_SEARCH function ( see queue.h ).
===   Finds NSAPI entity in the NSAPIs mapped to SAPI list
===
===   nsapi_pri_ptr     : ptr to NSAPIs mapped to SAPI list
===   nsapi_num_ptr     : ptr to the NSAPI number
===
===  DEPENDENCIES
===
===    Must comply with q_compare_func_type definition in queue.h
===
===  RETURN VALUE
===
===    Return non-zero if the NSAPI entity is on the list
===    Return 0 otherwise
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

static uint8 gsn_find_nsapi( const gsn_nsapi_q_t* nsapi_q_ptr, const gsn_nsapi_t *nsapi_num_ptr)
{
  if( nsapi_q_ptr->nsapi_ptr->nsapi == *nsapi_num_ptr )
    return( TRUE );
  else
    return( FALSE );
}


/*===========================================================================
===
===  FUNCTION      GSN_ACTIVATE_NSAPI()
===
===  DESCRIPTION
===
===   Activates and initialises the NSAPI entity.
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

static void gsn_activate_nsapi ( gas_id_t gas_id, gsnsm_prim_type prim_type, const void *smsn_mes_ptr )
{
  gsn_nsapi_t   nsapi;
  boolean       pcomp_enabled = FALSE;
  boolean       dcomp_enabled = FALSE;

  if( prim_type ==  GSNSM_ACTIVATE_IND )
    nsapi = GSN_INTERN_NSAPI( ((gsnsm_activ_ind_type*)smsn_mes_ptr)->nsapi );
  else
    nsapi = GSN_INTERN_NSAPI( ((gsnsm_modify_ind_type *)smsn_mes_ptr)->nsapi );

  /* check if NSAPI already active */
  if( actv_nsapis[ gas_id ][nsapi] == NULL )
  {
    /* Allocate memory for NSAPI entity */
    actv_nsapis[ gas_id ][nsapi] =
            GPRS_MEM_MALLOC( sizeof(gsn_nsapi_e_t) );
    GSN_ASSERT( actv_nsapis[ gas_id ][nsapi] != NULL );

    memset(actv_nsapis[ gas_id ][nsapi], 0, sizeof(gsn_nsapi_e_t));

    /* Initialise NSAPI entity */
    if( prim_type ==  GSNSM_ACTIVATE_IND )
    {
      gsn_init_nsapi( gas_id,
                      actv_nsapis[ gas_id ][nsapi],
                      prim_type,
                      (gsnsm_activ_ind_type *)smsn_mes_ptr);
    }
    else
    {
      gsn_init_nsapi( gas_id,
                      actv_nsapis[ gas_id ][nsapi],
                      prim_type,
                      (gsnsm_modify_ind_type *)smsn_mes_ptr);
    }

    /* Set the TX Data Signal for the NSAPI */
    actv_nsapis[ gas_id ][nsapi]->to_gsndcp_sig = comp_reserve_to_comp_tx_sig();

    /* Store NSAPI params */
    actv_nsapis[ gas_id ][nsapi]->nsapi      = nsapi;

    if( prim_type == GSNSM_ACTIVATE_IND )
    {

#ifdef FEATURE_DUAL_SIM
      /* Access stratum identity */
      actv_nsapis[ gas_id ][nsapi]->gas_id = gas_id;

      MSG_GERAN_MED_2_G("GSN ""GAS_ID: %d for NSAPI: %d ",
       gas_id, nsapi );
#endif /* FEATURE_DUAL_SIM */

      actv_nsapis[ gas_id ][nsapi]->radio_pr   =
        ((gsnsm_activ_ind_type*)smsn_mes_ptr)->radio_priority;
      actv_nsapis[ gas_id ][nsapi]->qos        =
        ((gsnsm_activ_ind_type*)smsn_mes_ptr)->qos;
      if((((gsnsm_activ_ind_type*)smsn_mes_ptr)->dcomp_alg
            ==SYS_PDP_DATA_COMP_V42_BIS)||
         (((gsnsm_activ_ind_type*)smsn_mes_ptr)->dcomp_alg
            ==SYS_PDP_DATA_COMP_ON))
      {
        dcomp_enabled = TRUE;
      }
      if((((gsnsm_activ_ind_type*)smsn_mes_ptr)->pcomp_alg
            >= SYS_PDP_HEADER_COMP_ON) &&
         (((gsnsm_activ_ind_type*)smsn_mes_ptr)->pcomp_alg
            <= SYS_PDP_HEADER_COMP_RFC3095))
      {
        pcomp_enabled = TRUE;
      }
      actv_nsapis[ gas_id ][nsapi]->pdp_type  =
        ((gsnsm_activ_ind_type*)smsn_mes_ptr)->pdp_type;

      actv_nsapis[ gas_id ][nsapi]->data_comp = dcomp_enabled;
      actv_nsapis[ gas_id ][nsapi]->pci_comp  = pcomp_enabled;

      actv_nsapis[ gas_id ][nsapi]->pfi        =
        ((gsnsm_activ_ind_type*)smsn_mes_ptr)->pfi;

      MSG_GERAN_MED_3_G("GSN ""Rel Class %d, PFI %d",
            ((gsnsm_activ_ind_type*)smsn_mes_ptr)->qos.reliability_class,
            ((gsnsm_activ_ind_type*)smsn_mes_ptr)->pfi,
            0);
    }
    else
    {

#ifdef FEATURE_DUAL_SIM
      /* Access stratum identity */
      actv_nsapis[ gas_id ][nsapi]->gas_id = gas_id;

      MSG_GERAN_MED_2_G("GSN ""GAS_ID: %d for NSAPI: %d ", gas_id, nsapi );
#endif /* FEATURE_DUAL_SIM */

      actv_nsapis[ gas_id ][nsapi]->radio_pr   =
        ((gsnsm_modify_ind_type *)smsn_mes_ptr)->radio_priority;
      actv_nsapis[ gas_id ][nsapi]->qos        =
        ((gsnsm_modify_ind_type *)smsn_mes_ptr)->qos;
      if((((gsnsm_modify_ind_type*)smsn_mes_ptr)->dcomp_alg
            ==SYS_PDP_DATA_COMP_V42_BIS)||
         (((gsnsm_modify_ind_type*)smsn_mes_ptr)->dcomp_alg
            ==SYS_PDP_DATA_COMP_ON))
      {
        dcomp_enabled = TRUE;
      }
      if((((gsnsm_modify_ind_type*)smsn_mes_ptr)->pcomp_alg
            >= SYS_PDP_HEADER_COMP_ON) &&
         (((gsnsm_modify_ind_type*)smsn_mes_ptr)->pcomp_alg
            <= SYS_PDP_HEADER_COMP_RFC3095))
      {
        pcomp_enabled = TRUE;
      }
      actv_nsapis[ gas_id ][nsapi]->pdp_type  =
        ((gsnsm_modify_ind_type*)smsn_mes_ptr)->pdp_type;

      actv_nsapis[ gas_id ][nsapi]->data_comp = dcomp_enabled;
      actv_nsapis[ gas_id ][nsapi]->pci_comp  = pcomp_enabled;


      actv_nsapis[ gas_id ][nsapi]->pfi        =
        ((gsnsm_modify_ind_type *)smsn_mes_ptr)->pfi;

      MSG_GERAN_MED_3_G("GSN ""Rel Class %d, PFI %d",
            ((gsnsm_modify_ind_type *)smsn_mes_ptr)->qos.reliability_class,
            ((gsnsm_modify_ind_type *)smsn_mes_ptr)->pfi,
            0);
    }

    /* Decode QoS parameters */
    if( (actv_nsapis[ gas_id ][nsapi]->qos.reliability_class == 0x1) ||
        (actv_nsapis[ gas_id ][nsapi]->qos.reliability_class == 0x2)
      )
    {
      actv_nsapis[ gas_id ][nsapi]->op_mode = ACK;
    }
    else
    {
      actv_nsapis[ gas_id ][nsapi]->op_mode = UNACK;
    }
  }
  else
  {
    /* ERROR. Activation requested on an active NSAPI */
    MSG_GERAN_ERROR_1_G("GSN ""ME: Activ req on active NSAPI %d",
                        GSN_EXTERN_NSAPI(nsapi));
  }
}

/*===========================================================================
===
===  FUNCTION      GSN_ACTIVATE_SAPI()
===
===  DESCRIPTION
===
===   Activates and initialises the SAPI entity.
===
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/
static void gsn_activate_sapi(  gas_id_t gas_id, gsn_sapi_t sapi, uint32 gsn_tlli )
{
  /*  Check if SAPI is active. SAPI might be used by other NSAPIs
  **  and been activated already.
  */
  if( actv_sapis[gas_id][sapi] == NULL )
  {
    /* Allocate memory for SAPI entity */
    actv_sapis[gas_id][sapi] =
          GPRS_MEM_MALLOC( sizeof(gsn_sapi_e_t) );
    GSN_ASSERT( actv_sapis[gas_id][sapi] != NULL );

    /* Heap memory allocated for sapi is initialised to avoid mutex creation
       issue in mustang builds caused by passing in non-null value for
       actv_sapis[gas_id][sapi]->nsapi_ptr_q.mutex into q_init(). */

    memset(actv_sapis[gas_id][sapi], 0, sizeof(gsn_sapi_e_t));

    /* Store SAPI's number */
    actv_sapis[gas_id][sapi]->sapi = sapi;

    /* Initialise SAPI entity */
    gsn_init_sapi( gas_id, actv_sapis[gas_id][sapi] );

    /* Store SAPI's tlli */
    actv_sapis[gas_id][sapi]->tlli   = gsn_tlli;

    actv_sapis[gas_id][sapi]->gas_id = gas_id;
  }
  else
  {
    if( actv_sapis[gas_id][sapi]->tlli != gsn_tlli )
    {
      MSG_GERAN_HIGH_1_G("GSN ""ME: Change of tlli %d", gsn_tlli);
      actv_sapis[gas_id][sapi]->tlli = gsn_tlli;
    }
  }
}

/*===========================================================================
===
===  FUNCTION      GSN_MAP_NSAPI_ON_SAPI()
===
===  DESCRIPTION
===
===   Links NSAPI with the assosiated SAPI
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/
static void gsn_map_nsapi_on_sapi( gas_id_t gas_id, gsn_nsapi_t nsapi, gsn_sapi_t sapi)
{
  gsn_nsapi_q_t     *nsapi_q_ptr = NULL;

  /*  Link NSAPI with the assosiated SAPI and add NSAPI's pointer to
  **  the SAPI's queue of pointrs of mapped NSAPIs on the SAPI.
  */

  if( actv_sapis[gas_id][sapi] != NULL )
  {
    if( actv_sapis[gas_id][sapi]->is_nsapi_ptr_q_valid )
    {
      actv_nsapis[ gas_id ][nsapi]->sapi_ptr = actv_sapis[gas_id][sapi];
      nsapi_q_ptr = GPRS_MEM_MALLOC( sizeof(gsn_nsapi_q_t) );
      GSN_ASSERT( nsapi_q_ptr != NULL );

      nsapi_q_ptr->nsapi_ptr = actv_nsapis[ gas_id ][nsapi];
      (void) q_link( nsapi_q_ptr, &(nsapi_q_ptr->link) );
      q_put( &(actv_sapis[gas_id][sapi]->nsapi_ptr_q), &(nsapi_q_ptr->link) );
    }
    else
    {
      MSG_GERAN_ERROR_0_G("GSN "" NSAPI ptr queue invalid");
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("GSN "" Invalid SAPI ");
  }
}

/*===========================================================================
===
===  FUNCTION      GSN_INIT_SAPI()
===
===  DESCRIPTION
===
===   Initialises activated SAPI
===
===  DEPENDENCIES
===
===    Expects RLC layer to be initialised
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

static void gsn_init_sapi( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr )
{

  /* Initial state of SAPI entity */
  sapi_ptr->state = IDLE;

  /* Initialise queue of pointers to active NSAPIs mapped on SAPI */
  (void) q_init( &(sapi_ptr->nsapi_ptr_q) );

  /* Indicate this queue is now active. */
  sapi_ptr->is_nsapi_ptr_q_valid = TRUE;

  /* Get Max length of SN-PDU in Ack and Unack operation from LLC */
#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
  #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  /* Counter for XID request renegotiation */
  sapi_ptr->xid_req_cnt = 0;

  /* Set TRUE in case of SAPI re-establishment  */
  sapi_ptr->re_establish = FALSE;

  /* Set TRUE if the link establishment procedure is initiated by the MS SNDCP */
  sapi_ptr->link_estab_originator = FALSE;

  /* Initialise XID state */
  sapi_ptr->xid_state = XID_IDLE;

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )

  sapi_ptr->xid_req_dup_ptr = NULL;

#endif

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP
  {
    uint8  i;

    /* Initialise PCI compression entities
    */
    for( i = 0; i <= ENT_NUM_MAX; i++ )
    {
      sapi_ptr->pcomp_e_pool[i] = NULL;
    }

    for( i = 0; i <= PCOMP_VAL_MAX; i++ )
    {
      sapi_ptr->pcomp_pool[i] = NULL;
    }

    for( i = 0; i <= T2_ALG_MAX; i++ )
    {
      (void) q_init( &(sapi_ptr->t2_alg_q[i]) );
    }
  }
#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP
  {
    /* Change to uint8 when more data compression algorithms added and
    ** T1_ALG_MAX != 0. int8 is used to avoid unassigned comparison with 0 warning.
    */
    int8  i;

    /* Initialise data compression entities
    */
    for( i = 0; i <= ENT_NUM_MAX; i++ )
    {
      sapi_ptr->dcomp_e_pool[i] = NULL;
    }

    for( i = 0; i <= DCOMP_VAL_MAX; i++ )
    {
      sapi_ptr->dcomp_pool[i] = NULL;
    }

    for( i = 0; i <= T1_ALG_MAX; i++ )
    {
      (void) q_init( &(sapi_ptr->t1_alg_q[i]) );
    }
  }
#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */

}

/*===========================================================================
===
===  FUNCTION      GSN_PCOMP_DCOMP_VALIDITY_CHECK()
===
===  DESCRIPTION
===
===   Validates pcomp and dcomp algorithm types received from SM.
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

static void gsn_pcomp_dcomp_validity_check(gas_id_t gas_id,
                                           gsn_nsapi_e_t *nsapi_ptr,
                                           gsnsm_prim_type prim_type,
                                           const void *snsm_msg_ptr )
{

  if(prim_type == GSNSM_ACTIVATE_IND)
  {
    /* Preferred T1 XID params */
    /* V42bis */

    sys_pdp_data_comp_e_type data_comp_alg =
                             ((gsnsm_activ_ind_type*)snsm_msg_ptr)->dcomp_alg;
    sys_pdp_header_comp_e_type hdr_comp_alg =
                             ((gsnsm_activ_ind_type*)snsm_msg_ptr)->pcomp_alg;
    gsnsm_pdp_type pdp_type = ((gsnsm_activ_ind_type*)snsm_msg_ptr)->pdp_type;

    if((data_comp_alg == SYS_PDP_DATA_COMP_V42_BIS) ||
       (data_comp_alg == SYS_PDP_DATA_COMP_ON))
    {
      nsapi_ptr->sn_xid_req.t1_alg = T1_ALG_V42BIS;
      nsapi_ptr->sn_xid_req.p0     = V42BIS_P0_DEF;
      nsapi_ptr->sn_xid_req.p1     = V42BIS_P1_DEF;
      nsapi_ptr->sn_xid_req.p2     = V42BIS_P2_DEF;
    }
    else if(data_comp_alg == SYS_PDP_DATA_COMP_OFF)
    {
     MSG_HIGH("No data comp",0,0,0);
     nsapi_ptr->sn_xid_req.t1_alg  = T1_ALG_NONE;
    }
    else
    {
     MSG_HIGH("Invalid T1 alg: %d for NSAPI: %d",nsapi_ptr->sn_xid_req.t1_alg,
                                                 nsapi_ptr->nsapi,
                                                 0);
     nsapi_ptr->sn_xid_req.t1_alg  = T1_ALG_NONE;
    }

    /* Preferred T2 XID params - PCI compression */
    if(((hdr_comp_alg == SYS_PDP_HEADER_COMP_ON)||
        (hdr_comp_alg == SYS_PDP_HEADER_COMP_RFC1144)) &&
        (pdp_type == GSNSM_PDP_IPV4))
    {
      nsapi_ptr->sn_xid_req.t2_alg    = T2_ALG_RFC1144;
      nsapi_ptr->sn_xid_req.s0_1      = RFC1144_S0_1_DEF;
    }
#ifdef FEATURE_GSM_SNDCP_IPHC
    else if((hdr_comp_alg == SYS_PDP_HEADER_COMP_RFC2507)&&
          ((pdp_type == GSNSM_PDP_IPV4) ||
           (pdp_type == GSNSM_PDP_IPV6)))
    {
      nsapi_ptr->sn_xid_req.t2_alg    = T2_ALG_RFC2507;
      nsapi_ptr->sn_xid_req.max_per   = RFC2507_MAX_PER_DEF;
      nsapi_ptr->sn_xid_req.max_time  = RFC2507_MAX_TIME_DEF;
      nsapi_ptr->sn_xid_req.max_hdr   = RFC2507_MAX_HDR_DEF;
      nsapi_ptr->sn_xid_req.tcp_sp    = RFC2507_TCP_SP_DEF;
      nsapi_ptr->sn_xid_req.ntcp_sp   = RFC2507_NTCP_SP_DEF;
    }
#endif /* FEATURE_GSM_SNDCP_IPHC */
    else if(hdr_comp_alg == SYS_PDP_HEADER_COMP_OFF)
    {
      MSG_HIGH("No hdr comp",0,0,0);
      nsapi_ptr->sn_xid_req.t2_alg    = T2_ALG_NONE;
    }
    else
    {
     MSG_HIGH("Invalid pcomp alg (%d) and pdp_type (%d) params ",
                 hdr_comp_alg,
                 pdp_type,
                 0);
     nsapi_ptr->sn_xid_req.t2_alg     = T2_ALG_NONE;
    }
  }
  else
  {

    sys_pdp_data_comp_e_type data_comp_alg =
                           ((gsnsm_modify_ind_type*)snsm_msg_ptr)->dcomp_alg;
    sys_pdp_header_comp_e_type hdr_comp_alg =
                           ((gsnsm_modify_ind_type*)snsm_msg_ptr)->pcomp_alg;
    gsnsm_pdp_type pdp_type = ((gsnsm_modify_ind_type*)snsm_msg_ptr)->pdp_type;

    /* Preferred T1 XID params */
    /* V42bis */

    if((data_comp_alg == SYS_PDP_DATA_COMP_V42_BIS) ||
       (data_comp_alg == SYS_PDP_DATA_COMP_ON))
    {
      nsapi_ptr->sn_xid_req.t1_alg = T1_ALG_V42BIS;
      nsapi_ptr->sn_xid_req.p0     = V42BIS_P0_DEF;
      nsapi_ptr->sn_xid_req.p1     = V42BIS_P1_DEF;
      nsapi_ptr->sn_xid_req.p2     = V42BIS_P2_DEF;
    }
    else if(data_comp_alg == SYS_PDP_DATA_COMP_OFF)
    {
      MSG_HIGH("No data comp",0,0,0);
      nsapi_ptr->sn_xid_req.t1_alg  = T1_ALG_NONE;
    }
    else
    {
      MSG_HIGH("Invalid T2 alg: %d for NSAPI: %d",nsapi_ptr->sn_xid_req.t1_alg,
                                                  nsapi_ptr->nsapi,
                                                  0);
      nsapi_ptr->sn_xid_req.t1_alg  = T1_ALG_NONE;
    }

    /* Preferred T2 XID params - PCI compression */
    if(((hdr_comp_alg == SYS_PDP_HEADER_COMP_ON)||
        (hdr_comp_alg == SYS_PDP_HEADER_COMP_RFC1144)) &&
        (pdp_type == GSNSM_PDP_IPV4))
    {
      nsapi_ptr->sn_xid_req.t2_alg    = T2_ALG_RFC1144;
      nsapi_ptr->sn_xid_req.s0_1      = RFC1144_S0_1_DEF;
    }
#ifdef FEATURE_GSM_SNDCP_IPHC
    else if((hdr_comp_alg == SYS_PDP_HEADER_COMP_RFC2507)&&
            ((pdp_type ==GSNSM_PDP_IPV4) ||
             (pdp_type ==GSNSM_PDP_IPV6)))
    {
      nsapi_ptr->sn_xid_req.t2_alg    = T2_ALG_RFC2507;
      nsapi_ptr->sn_xid_req.max_per   = RFC2507_MAX_PER_DEF;
      nsapi_ptr->sn_xid_req.max_time  = RFC2507_MAX_TIME_DEF;
      nsapi_ptr->sn_xid_req.max_hdr   = RFC2507_MAX_HDR_DEF;
      nsapi_ptr->sn_xid_req.tcp_sp    = RFC2507_TCP_SP_DEF;
      nsapi_ptr->sn_xid_req.ntcp_sp   = RFC2507_NTCP_SP_DEF;
    }
#endif /* FEATURE_GSM_SNDCP_IPHC */
    else if(hdr_comp_alg == SYS_PDP_HEADER_COMP_OFF)
    {
      MSG_HIGH("No hdr comp",0,0,0);
      nsapi_ptr->sn_xid_req.t2_alg    = T2_ALG_NONE;
    }
    else
    {
      MSG_HIGH("Invalid pcomp alg(%d) and pdp_type (%d) params",
                 hdr_comp_alg,
                 pdp_type,
                 0);
      nsapi_ptr->sn_xid_req.t2_alg     = T2_ALG_NONE;
    }
  }

} /* gsn_pcomp_dcomp_validity_check() */


/*===========================================================================
===
===  FUNCTION      GSN_INIT_NSAPI()
===
===  DESCRIPTION
===
===   Initialises NSAPI entity
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

static void gsn_init_nsapi(gas_id_t         gas_id,
                           gsn_nsapi_e_t   *nsapi_ptr,
                           gsnsm_prim_type  prim_type,
                           const void      *snsm_msg_ptr )
{

  /* initialise Up-Link entity */
  nsapi_ptr->ule.susp_flag        = 0x0;
  nsapi_ptr->ule.ack_send_npdu    = 0;
  nsapi_ptr->ule.unack_send_npdu  = 0;
  nsapi_ptr->ule.retr             = FALSE;

  /* Ready to receive the first DL N-PDU */
  nsapi_ptr->first_dl_npdu        = TRUE;

  /* Init N-PDU retransmission buffer */
  (void) q_init( &nsapi_ptr->ule.npdu_buff );

  /* Set flag to indicate npdu_buff is valid. */
  nsapi_ptr->ule.npdu_buff_valid = TRUE;


  /* Init NDPU ref item buffer which is used for segment re-ordering. */
  (void) q_init( &nsapi_ptr->dle.npdu_ref_buff );

  nsapi_ptr->dle.npdu_ref_buff_valid = TRUE;


  /* Init SN-PDU Reassembly buffer */
  nsapi_ptr->dle.reas_buff    = NULL;

  /* Initialise Down-Link entity */
  nsapi_ptr->dle.state        = DL_RECOVERY;
  nsapi_ptr->dle.reas_state   = DL_RCV_1ST_SEQ;
  nsapi_ptr->dle.ack_rcv_npdu = 0;
  nsapi_ptr->dle.unack_rcv_npdu = 0;
  nsapi_ptr->dle.first_seg_info.dcomp           = 0;
  nsapi_ptr->dle.first_seg_info.ack_npdu_num    = 0;
  nsapi_ptr->dle.first_seg_info.unack_npdu_num  = 0;
  nsapi_ptr->dle.first_seg_info.pcomp           = 0;
  nsapi_ptr->dle.prv_seg      = 0;

  /* Initialise SNDCP User registration functions */
  nsapi_ptr->pdp_ul_suspend_fnc  = NULL;
  nsapi_ptr->pdp_ul_resume_fnc   = NULL;
  nsapi_ptr->pdp_frw_dl_data_fnc = NULL;
  nsapi_ptr->pdp_reg_cnf_fnc     = NULL;

  /* Disable Data and PCI compression */
  nsapi_ptr->dcomp_alg = SYS_PDP_DATA_COMP_OFF;
  nsapi_ptr->pcomp_alg = SYS_PDP_HEADER_COMP_OFF;
  nsapi_ptr->pdp_type  = GSNSM_PDP_INVALID;
  nsapi_ptr->data_comp = FALSE;
  nsapi_ptr->pci_comp  = FALSE;

  /* Disable ciphering */
  nsapi_ptr->cipher    = FALSE;

  /* Set SAPI pointer to NULL */
  nsapi_ptr->sapi_ptr         = NULL;

  /* Set TRUE when NSAPI must be delinked from SAPI after
  ** LLC ACK relese is confirmed
  */
  nsapi_ptr->delink_nsapi     = FALSE;

  /* Set TRUE when NSAPI must be initialised after
  ** SM-DEACTIVATE-IND
  */
  nsapi_ptr->init_nsapi       = FALSE;

  /* Set TRUE when  SNSM-ACTIVATE.IND primitive is recieved  */
  nsapi_ptr->actv_resp_pend   = FALSE;

  /* Set TRUE when SNSM-DEACTIVATE.IND primitive is received */
  nsapi_ptr->deactv_resp_pend = FALSE;

  /* Set TRUE when SNSM-MODIFY.IND primitive is received */
  nsapi_ptr->modif_resp_pend  = FALSE;

  /* Set TRUE when LL_ESTABLISH.REQ primitive is send due to SNSM-MODIFY.IND */
  nsapi_ptr->modif_rel_resp_pend  = FALSE;

  /* Set TRUE when LL_RELEASE.REQ primitive is send due to SNSM-MODIFY.IND */
  nsapi_ptr->modif_est_resp_pend  = FALSE;

  /* Set TRUE when SN-XID.REQ primitive is received from PDP */
  nsapi_ptr->sn_xid_resp_pend     = FALSE;

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )

  /* Preferred T0 XID params - SNDCP Version number */
  nsapi_ptr->sn_xid_req.ver_num = GSN_VER_NUM_1; /* Rel-5 onwards. */

  gsn_pcomp_dcomp_validity_check(gas_id,
                                 nsapi_ptr,
                                 prim_type,
                                 (void *)snsm_msg_ptr);
#endif

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP
  {
    uint8  i;

    /* Initialise PCI compression entities linked to NSAPI */
    for( i = 0; i <= T2_ALG_MAX; i++ )
    {
      nsapi_ptr->pcomp_e_num[i] = NO_COMP_ENT;
    }
  }

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP
  {
    uint8  i;

     /* Initialise data compression entities linked to NSAPI */
    for( i = 0; i < (T1_ALG_MAX+1) ; i++ )
    {
      nsapi_ptr->dcomp_e_num[i] = NO_COMP_ENT;
    }
  }

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

  (void)memset
  (
    &nsapi_ptr->diag_pdu_stat_rsp, 0,
    sizeof( gsndcp_diag_pdu_stat_rsp_t )
  );


  /* Initialise SGSN initiated link est failure timer */

  rex_def_timer_ex
  (
    &nsapi_ptr->sgsn_initiated_link_est_fail_timer,
    gsn_timer_callback,
    (uint32)( ( ( gas_id << 24 ) & 0xFF000000 ) | ( (nsapi_ptr->nsapi << 16) & 0xFF0000 ) | SGSN_INITIATED_LINK_EST_FAIL )
  );
}

/*===========================================================================
===
===  FUNCTION      GSN_DEACTV_NSAPI()
===
===  DESCRIPTION
===
===   Deactivates NSAPI entity
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

static void gsn_deactv_nsapi( gas_id_t gas_id, gsn_nsapi_e_t *nsapi_ptr )
{
  if( nsapi_ptr != NULL )
  {

    /* Unreserve the PS to SNDCP Tx signal. */
    comp_unreserve_to_comp_tx_sig(
        actv_nsapis[ gas_id ][nsapi_ptr->nsapi]->to_gsndcp_sig);

    /*  Clear call back functions registered with to_gsndcp_wm */
    if(nsapi_ptr->to_gsndcp_wm != NULL)
    {
      nsapi_ptr->to_gsndcp_wm->each_enqueue_func_ptr = NULL;
      nsapi_ptr->to_gsndcp_wm->gone_empty_func_ptr = NULL;
      nsapi_ptr->to_gsndcp_wm = NULL;
    }
    nsapi_ptr->from_gsndcp_post_rx_func_ptr = NULL;
    nsapi_ptr->from_gsndcp_wm = NULL;



    /* NSAPI deactivated. Stop SGSN initiated link est failure timer. */
    (void)rex_clr_timer(
       &nsapi_ptr->sgsn_initiated_link_est_fail_timer );

    /* Clear all NPDUs and any segments in NSAPI buffers. */
    gsn_clr_nsapi_buf( gas_id, nsapi_ptr->nsapi );

    /* Release npdu buffer, i.e. destroy queue. */
    gsn_rel_nsapi_buf( gas_id, nsapi_ptr->nsapi );

    /* Dealocate NSAPI's entity memory  */
    actv_nsapis[ gas_id ][ nsapi_ptr->nsapi ] = NULL;
    GSN_MEM_FREE( nsapi_ptr );

  }
  else
  {
    MSG_GERAN_ERROR_0_G("GSN ""Invalid NSAPI ptr");
  }
}

/*===========================================================================
===
===  FUNCTION      GSN_DEACTV_SAPI()
===
===  DESCRIPTION
===
===   Deactivates SAPI entity
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/
static void gsn_deactv_sapi( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr )
{
  if( sapi_ptr != NULL )
  {

    /* Inform LLC */
    gsn_send_ll_deactivate_ind( gas_id, sapi_ptr );

    /* Clear SAPI's buffers/queues */
    gsn_clr_sapi_buf( gas_id, sapi_ptr->sapi );

    /* Dealocate memory used for SAPI entity  */
    actv_sapis[ gas_id ][ sapi_ptr->sapi ] = NULL;

    GSN_MEM_FREE( sapi_ptr );


  }
  else
    MSG_GERAN_ERROR_0_G("GSN ""Invalid SAPI ptr");

}

/*===========================================================================
===
===  FUNCTION      GSN_REMOVE_NSAPI_FROM_SAPI()
===
===  DESCRIPTION
===
===   Removes the NSAPI from the SAPI's list of associated NSAPIs.
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    SAPI will be deactivated if it is no used by any NSAPI
===
===========================================================================*/
static void gsn_remove_nsapi_from_sapi( gas_id_t gas_id, gsn_nsapi_t nsapi, gsn_sapi_t sapi)
{
  gsn_nsapi_q_t   *nsapi_q_ptr;
  gsn_sapi_e_t    *sapi_ptr  = actv_sapis[gas_id][sapi];

  /* Check if pointer to active SAPI entity is valid */
  GSN_ASSERT( sapi_ptr != NULL );

  if ( sapi_ptr->is_nsapi_ptr_q_valid )
  {

    /* LLC should not be allowed to access this queue while it is being
    ** destroyed.
    */
    GSN_LOCK();

    /* find position of NSAPI in the  list */
    nsapi_q_ptr = q_linear_search( &(sapi_ptr->nsapi_ptr_q),
                    (  q_compare_func_type)gsn_find_nsapi, &nsapi);

    if( nsapi_q_ptr == NULL )
    {
     MSG_GERAN_ERROR_2_G("GSN " "NSAPI %d isn't mapped to SAPI %d",
       GSN_EXTERN_NSAPI(nsapi), gsn_extern_sapi[sapi]);
    }
    else
    {

      /* remove NSAPI from SAPI's list */
      q_delete
        (
            &(sapi_ptr->nsapi_ptr_q),
            &(nsapi_q_ptr->link)
        );

      /* Free allocated memmory  */
      GSN_MEM_FREE( nsapi_q_ptr );


      if( q_check( &(sapi_ptr->nsapi_ptr_q) ) == NULL )
      {
        /*  Deactivate SAPI. It was only used by this NSAPI */
        gsn_deactv_sapi( gas_id, sapi_ptr );
      }

    }

    GSN_UNLOCK();
  }
  else
  {
    MSG_GERAN_ERROR_0_G("GSN ""NSAPI ptr queue invalid");
  }
}

/*===========================================================================
===
===  FUNCTION      GSN_RELEASE_SAPI()
===
===  DESCRIPTION
===
===   Releases the LLC ACK operation if no other ACK NSAPI is mapped to the SAPI.
===   If UNACK NSAPI then it is removed from SAPI's list of associated NSAPIs
===
===   nsapi : Affected NSAPI
===   sapi  : Affected SAPI
===   local : Set to TRUE if the release is the result of the SNSM_DEACTIVATE_IND.
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    Boolean. TRUE if LL_RELEASE_REQ is send to LLC
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/
static boolean gsn_release_sapi( gas_id_t gas_id, gsn_nsapi_t nsapi, gsn_sapi_t sapi, boolean local )
{
  boolean               sapi_uses_ack_op, rel_req_send = FALSE;
  gsn_sapi_e_t          *sapi_ptr  = actv_sapis[gas_id][sapi];

  /* Check if pointer to active SAPI entity is valid */
  if( sapi_ptr == NULL )
  {
    MSG_GERAN_ERROR_1_G("GSN ""LL %d: Invalid pointer", gsn_extern_sapi[sapi]);
    return( FALSE );
  }

  if( sapi_ptr->is_nsapi_ptr_q_valid )
  {

    if(  q_cnt( &(sapi_ptr->nsapi_ptr_q) )  > 1 )
    {
      /*  Other active NSAPIs are mapped on the SAPI.
      **  Check if any of them uses Ack operation before
      **  we release the LLC link ( if established ).
      */
      sapi_uses_ack_op = gsn_ack_nsapis_on_sapi( gas_id, nsapi, sapi );

      /*  If SAPI is used by another NSAPI for Ack operation, do not release
      **  the ACK operation on this SAPI, otherwise, release  it.
      */
      if( sapi_uses_ack_op == FALSE )
      {
        /* Release the ACK operation on this SAPI, if established. */
        if( sapi_ptr->state != IDLE )
        {
          rel_req_send = TRUE;

          /* Post message to LLC */
          gsn_send_ll_rel_req( gas_id, sapi_ptr, local );

        }
      }
    }
    else
    {
      /*  There is no other NSAPIs mapped on the SAPI. So release
      **  the Ack LLC operation on this SAPI. if established.
      */
      if( sapi_ptr->state != IDLE )
      {
        /* Release the Ack LLC operation on this SAPI */
        rel_req_send = TRUE;

        /* Post message to LLC */
        gsn_send_ll_rel_req( gas_id, sapi_ptr, local );

      }
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G("GSN ""LL %d: NSAPI ptr queue invalid", gsn_extern_sapi[sapi]);
  }

  /* If LL_RELEASE_REQ hasn't been send, remove NSAPI from SAPI's list.
  ** Otherwise, wait until LL_RELEASE_CNF to remove the NSAPI.
  */
  if( !rel_req_send )
  {
    gsn_remove_nsapi_from_sapi( gas_id, nsapi, sapi );
  }

  return( rel_req_send );
}

/*===========================================================================
===
===  FUNCTION      GSN_ACK_NSAPIS_ON_SAPI()
===
===  DESCRIPTION
===
===   Checks if any of the NSAPIs mapped on the SAPI uses Ack operation
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    Boolean. TRUE if an NSAPI that uses Ack operation is mapped on the SAPI
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/
static boolean gsn_ack_nsapis_on_sapi( gas_id_t gas_id, gsn_nsapi_t nsapi, gsn_sapi_t sapi )
{
  gsn_sapi_e_t *sapi_ptr = actv_sapis[gas_id][sapi];
  gsn_nsapi_q_t     *item_ptr = NULL;

  /* Check if pointer to active SAPI entity is valid */
  if( sapi_ptr == NULL )
  {
    MSG_GERAN_ERROR_1_G("GSN ""LL %d: Invalid pointer", gsn_extern_sapi[sapi]);
    return( FALSE );
  }

  if( sapi_ptr->is_nsapi_ptr_q_valid )
  {

    item_ptr = (gsn_nsapi_q_t*)q_check( &(sapi_ptr->nsapi_ptr_q) );

    if( item_ptr )
    {
      /* Check for valid pointer of active NSAPI entity  */
      GSN_ASSERT( item_ptr->nsapi_ptr != NULL );

      while( item_ptr != NULL )
      {
        if( item_ptr->nsapi_ptr->op_mode == ACK )
        {
          if( item_ptr->nsapi_ptr->nsapi != nsapi )
            /* Another Nsapi uses the SAPI for Ack operation */
            return( TRUE );
        }

        /* Get next NSAPI from the list */
        item_ptr = (gsn_nsapi_q_t*)q_next( &(sapi_ptr->nsapi_ptr_q),
                                           &item_ptr->link );

      } /* END while traversing the queue */
    }
    else
    {
      /* something is wrong!! no NSAPIs mapped on an active SAPI !! */
      MSG_GERAN_HIGH_1_G("GSN ""No NSAPIs mapped on an active SAPI= %d .", sapi);
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G("GSN ""LL %d: NSAPI ptr queue invalid", gsn_extern_sapi[sapi]);
  }

  return( FALSE );
}

/*===========================================================================
===
===  FUNCTION      GSN_SEND_LL_EST_RES()
===
===  DESCRIPTION
===
===   Sends LL_ESTABLISH response primitive to LLC after reception of the
===   LL_ESTABLISH indication primitive from LLC.
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
static void gsn_send_ll_est_res( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr, dsm_item_type *xid_neg_ptr )
{
  gsn_nsapi_q_t         *nsapi_q_ptr;
  gsn_sapi_t            sapi = sapi_ptr->sapi;
  gprs_ll_ul_sig_mes_t  *ul_mes;

  /* Switch on SAPI's state */
  switch( sapi_ptr->state )
  {

  /*-------------------------------------------------------------------------------*/
  case ( IDLE ):
  case ( EST_PEND ):
  case ( ESTABLISHED):
    {

      /* Print XID parameters in F3 message */
      gsn_print_xid_params ( gas_id, xid_neg_ptr, TRUE );

      /* Get message buffer */
      ul_mes = gllc_ll_ul_sig_get_mes_buf();
      GSN_ASSERT( ul_mes != NULL );

      /* Load msg */
      ul_mes->sapi                        = gsn_extern_sapi[sapi];
      ul_mes->mes.est_res_mes.prim        = LL_ESTABLISH_RES;
      ul_mes->mes.est_res_mes.l3_xid_neg  = xid_neg_ptr;
      ul_mes->mes.est_res_mes.tlli        = sapi_ptr->tlli;
      ul_mes->mes.est_res_mes.gas_id      = gas_id;

      if( sapi_ptr->is_nsapi_ptr_q_valid )
      {

        nsapi_q_ptr = q_check( &sapi_ptr->nsapi_ptr_q );

        if ( nsapi_q_ptr != NULL )
        {
          ul_mes->mes.est_res_mes.peak_throughput =
            nsapi_q_ptr->nsapi_ptr->qos.peak_throughput;
          ul_mes->mes.est_res_mes.radio_priority  =
            nsapi_q_ptr->nsapi_ptr->radio_pr;
          ul_mes->mes.est_res_mes.pfi  =
            nsapi_q_ptr->nsapi_ptr->pfi;

        }
        else
        {
          ul_mes->mes.est_res_mes.peak_throughput = 0;
          ul_mes->mes.est_res_mes.radio_priority  = 1;
          ul_mes->mes.est_res_mes.pfi             = 0;
        }
      }
      else
      {

        ul_mes->mes.est_res_mes.peak_throughput = 0;
        ul_mes->mes.est_res_mes.radio_priority  = 1;
        ul_mes->mes.est_res_mes.pfi             = 0;

        MSG_GERAN_ERROR_1_G("GSN ""LL %d: NSAPI ptr queue invalid",
                       gsn_extern_sapi[sapi]);
      }

      /* SAPI is established */
      sapi_ptr->state = ESTABLISHED;

      if( ul_mes->mes.est_res_mes.l3_xid_neg != NULL )
        /* XID negotiation completed  */
        sapi_ptr->xid_state = XID_NEGOTIATED;

      /* Post message to LLC and log for call flow analysis. */
      MSG_GERAN_MED_1_G("GSN ""LL %d: Sent LL_ESTABLISH_RES",gsn_extern_sapi[sapi]);

      cfa_log_packet_ex
      (
        GS_QUEUE_LLC, MS_LLC_SNDCP, (uint8) LL_ESTABLISH_RES,
        sizeof( ll_establish_res_t ), (uint8 *)&ul_mes -> mes.est_res_mes
      );

      gllc_ll_ul_sig_put_mes_buf( gas_id, ul_mes );
      break;
    }

  default:

    MSG_GERAN_ERROR_2_G("GSN ""LL %d: Rcvd LL_EST_RSP in state: %d",
      gsn_extern_sapi[sapi],sapi_ptr->state);
    break;

  }
}


/*===========================================================================
===
===  FUNCTION      GSN_SEND_LL_REL_REQ()
===
===  DESCRIPTION
===
===   Sends LL_RELEASE request to LLC to release ACK peer-to-peer operation
===   for a SAPI in the LLC layer.
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
static void gsn_send_ll_rel_req( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr, boolean local)
{
  gsn_sapi_t            sapi = sapi_ptr->sapi;
  gprs_ll_ul_sig_mes_t  *ul_mes;

  /* Switch on SAPI's state */
  switch( sapi_ptr->state )
  {

/*-------------------------------------------------------------------------------*/
  case ( EST_PEND ):
  case ( ESTABLISHED ):
    {
      /* Get message buffer */
      ul_mes = gllc_ll_ul_sig_get_mes_buf( );
      GSN_ASSERT( ul_mes != NULL );

      /* Load msg */
      ul_mes->sapi                   = gsn_extern_sapi[sapi];
      ul_mes->mes.rel_req_mes.prim   = LL_RELEASE_REQ;
      ul_mes->mes.rel_req_mes.local  = local;
      ul_mes->mes.rel_req_mes.tlli   = sapi_ptr->tlli;
      ul_mes->mes.rel_req_mes.gas_id = gas_id;

      /* change SAPI state */
      sapi_ptr->state = DISC_PEND;

      /* Post message to LLC and log for call flow analysis. */
      MSG_GERAN_MED_1_G("GSN ""LL %d: Sent LL_RELEASE_REQ",gsn_extern_sapi[sapi]);

      cfa_log_packet_ex
      (
        GS_QUEUE_LLC, MS_LLC_SNDCP, (uint8) LL_RELEASE_REQ,
        sizeof( ll_release_req_t ), (uint8 *)&ul_mes -> mes.rel_req_mes
      );

      gllc_ll_ul_sig_put_mes_buf( gas_id, ul_mes );
    }
    break;

/*-------------------------------------------------------------------------------*/
  default:

    MSG_GERAN_ERROR_2_G("GSN ""LL %d: Rcvd REL_REQ in state: %d",gsn_extern_sapi[sapi],sapi_ptr->state);
    break;

/*-------------------------------------------------------------------------------*/
  } /* switch switch( sapi_ptr->state ) */
}

/*===========================================================================
===
===  FUNCTION      GSN_SEND_LL_XID_RES()
===
===  DESCRIPTION
===
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
static void gsn_send_ll_xid_res( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr, dsm_item_type  *xid_neg )
{
  gsn_nsapi_q_t         *nsapi_q_ptr;
  gsn_sapi_t            sapi = sapi_ptr->sapi;
  gprs_ll_ul_sig_mes_t  *ul_mes;

  /* Switch on SAPI's state */
  switch( sapi_ptr->state )
  {
/*-------------------------------------------------------------------------------*/
  case ( IDLE ):
  case ( ESTABLISHED):
    {

      /* Print XID parameters in F3 message */
      gsn_print_xid_params ( gas_id, xid_neg, TRUE );

      /* Get buffer to convey the message */
      ul_mes = gllc_ll_ul_sig_get_mes_buf( );
      GSN_ASSERT( ul_mes != NULL );

      /* Load msg */
      ul_mes->sapi  = gsn_extern_sapi[sapi];
      ul_mes->mes.xid_res_mes.prim          = LL_XID_RES;
      ul_mes->mes.xid_res_mes.l3_xid_neg    = xid_neg;
      ul_mes->mes.xid_res_mes.tlli          = sapi_ptr->tlli;
      ul_mes->mes.xid_res_mes.gas_id        = gas_id;

      if( sapi_ptr->is_nsapi_ptr_q_valid )
      {

        nsapi_q_ptr = q_check( &sapi_ptr->nsapi_ptr_q );

        if ( nsapi_q_ptr != NULL )
        {
          ul_mes->mes.xid_res_mes.peak_throughput =
            nsapi_q_ptr->nsapi_ptr->qos.peak_throughput;
          ul_mes->mes.xid_res_mes.radio_priority  =
            nsapi_q_ptr->nsapi_ptr->radio_pr;
          ul_mes->mes.xid_res_mes.pfi  =
            nsapi_q_ptr->nsapi_ptr->pfi;
        }
        else
        {
          ul_mes->mes.xid_res_mes.peak_throughput = 0;
          ul_mes->mes.xid_res_mes.radio_priority  = 1;
          ul_mes->mes.xid_res_mes.pfi             = 0;
        }
      }
      else
      {

        ul_mes->mes.xid_res_mes.peak_throughput = 0;
        ul_mes->mes.xid_res_mes.radio_priority  = 1;
        ul_mes->mes.xid_res_mes.pfi             = 0;

        MSG_GERAN_ERROR_2_G("GSN ""LL %d: NSAPI ptr queue invalid in state: %d",
                      gsn_extern_sapi[sapi],sapi_ptr->state);

      }

      sapi_ptr->xid_state = XID_NEGOTIATED;

      /* Post message to LLC and log for call flow analysis. */
      MSG_GERAN_MED_1_G("GSN ""LL %d: Sent LL_XID_RES",gsn_extern_sapi[sapi]);

      cfa_log_packet_ex
      (
        GS_QUEUE_LLC, MS_LLC_SNDCP, (uint8) LL_XID_RES,
        sizeof( ll_xid_res_t ), (uint8 *)&ul_mes -> mes.xid_res_mes
      );

      gllc_ll_ul_sig_put_mes_buf( gas_id, ul_mes );
      break;
    }
/*-------------------------------------------------------------------------------*/
  default:

    MSG_GERAN_ERROR_2_G("GSN ""LL %d: Rcvd XID_REQ in state: %d",gsn_extern_sapi[sapi],sapi_ptr->state);
    break;

/*-------------------------------------------------------------------------------*/
  } /* switch switch( sapi_ptr->state ) */
}


/*===========================================================================
===
===  FUNCTION      GSN_SEND_LL_XID_RESET_RES()
===
===  DESCRIPTION
===
===   On receiving LL_RESET_IND, SNDCP sends LL_RESET_RES to LLC.
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
static void gsn_send_ll_reset_res( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr )
{
  gsn_sapi_t            sapi = sapi_ptr->sapi;
  gprs_ll_ul_sig_mes_t  *ul_mes;

  /* Switch on SAPI's state */
  switch( sapi_ptr->state )
  {

/*-------------------------------------------------------------------------------*/
  case ( IDLE ):
  case ( EST_PEND ):
  case ( ESTABLISHED):
    {
      /* Get message buffer */
      ul_mes = gllc_ll_ul_sig_get_mes_buf( );
      GSN_ASSERT( ul_mes != NULL );

      /* Load msg */
      ul_mes->sapi                    = gsn_extern_sapi[sapi];
      ul_mes->mes.reset_res_mes.prim  = LL_RESET_RES;
      ul_mes->mes.reset_res_mes.tlli  = sapi_ptr->tlli;
      ul_mes->mes.reset_res_mes.gas_id= gas_id;

      /* Post message to LLC and log for call flow analysis. */
      MSG_GERAN_MED_1_G("GSN ""LL %d: Sent LL_RESET_RES",gsn_extern_sapi[sapi]);

	  /*
      cfa_log_packet_ex
      (
        GS_QUEUE_LLC, MS_LLC_SNDCP, (uint8) LL_RESET_RES,
        sizeof( ll_release_req_t ), (uint8 *)&ul_mes -> mes.rel_req_mes
      );
      */

      gllc_ll_ul_sig_put_mes_buf( gas_id, ul_mes );
    }
    break;

/*-------------------------------------------------------------------------------*/
  default:

    MSG_GERAN_ERROR_2_G("GSN ""LL %d: Rcvd RESET_RES in state: %d",gsn_extern_sapi[sapi],sapi_ptr->state);
    break;

/*-------------------------------------------------------------------------------*/
  } /* switch switch( sapi_ptr->state ) */
}



/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_LL_RST_IND()
===
===  DESCRIPTION
===
===   Processes the LL_RESET indication primitive from LLC.
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
static void gsn_process_ll_rst_ind( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr, ll_reset_ind_t  *ll_mes )
{
  gsn_sapi_t         sapi = sapi_ptr->sapi;
  gsn_npdu_item_t    *npdu_item_ptr;
  gsn_seg_ref_item_t *seg_item_ptr;
  gsn_seg_ref_item_t *next_seg_item_ptr;
  boolean             b_anite_gcf_flag = FALSE;

  /* Switch on SAPI's state */
  switch( sapi_ptr->state )
  {
/*-------------------------------------------------------------------------------*/
  case( IDLE ):
  case( EST_PEND ):
  case( ESTABLISHED ):
  case( DISC_PEND ):
    {
      gsn_nsapi_q_t*    item_ptr;

      if( (ll_mes != NULL) && sapi_ptr->is_nsapi_ptr_q_valid )
      {
#ifdef FEATURE_GPRS_PS_HANDOVER
        MSG_GERAN_HIGH_1_G("GSN ""use_old_xid_ind = %d",ll_mes->use_old_xid_ind);
#endif /* FEATURE_GPRS_PS_HANDOVER */

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP
        {
          /* Reset all SNDCP XID parameters to their default values.
          ** The default value for a compression entity is non-existing.
          ** See TS 04.65 par 6.8.2
          */
          t2_alg_q_t        *q_item;
          t2_alg_q_t        *q_next_item;
          uint8             alg;

          for( alg = 0; alg <= T2_ALG_MAX; alg++ )
          {
            q_item = q_check( &sapi_ptr->t2_alg_q[alg] );
            while( q_item != NULL )
            {
              q_next_item = q_next( &sapi_ptr->t2_alg_q[alg], &q_item->link );

              /* old xid parameter is not set, reset to default xid params */
#ifdef FEATURE_GPRS_PS_HANDOVER
              if(ll_mes->use_old_xid_ind == FALSE)
              {
#endif /* FEATURE_GPRS_PS_HANDOVER */
                gsn_del_pcomp_e( gas_id, q_item->pcomp_e_ptr->ent_num, sapi_ptr );
#ifdef FEATURE_GPRS_PS_HANDOVER
              }
              else
              {
                /* old xid parameter is set, continue to use previously used xid
                parameters and reinitialise compression entities */
                switch(alg)
                {
                case T2_ALG_RFC1144:
                  /* free existing RFC1144 instance */
                  gsn_slhc_free_mem(gas_id, q_item->pcomp_e_ptr->comp_e);
                  /* create a new instance and init it with existing params */
                  (void)gsn_1144_comp_init(gas_id, q_item->pcomp_e_ptr,
                    (uint8) (q_item->pcomp_e_ptr->alg.xid_rfc1144.s0_1 + 1));
                  break;

                case T2_ALG_RFC2507:
                  /* free existing RFC2507 instance */
                  gsn_iphc_free(gas_id, q_item->pcomp_e_ptr->comp_e);
                  /* create a new instance and init it with existing params */
                  (void)gsn_iphc_comp_init(
                     gas_id, q_item->pcomp_e_ptr,
                    (gsn_xid_pcomp_2507_t *)(&q_item->pcomp_e_ptr->alg.xid_rfc2507));
                  break;

                case T2_ALG_RFC3095:
                  /* free existing RFC3095 instance */
                  gsn_rohc_free(gas_id, q_item->pcomp_e_ptr->comp_e);
                  /* create a new instance and init it with existing params */
                  (void)gsn_rohc_comp_init(gas_id, q_item->pcomp_e_ptr->comp_e,
                    (gsn_xid_pcomp_3095_t *) (&q_item->pcomp_e_ptr->alg.xid_rfc3095));
                  break;

                default:
                  MSG_GERAN_ERROR_0_G("GSN " "Invalid T2 alg");
                  break;
                }
              }
#endif /* FEATURE_GPRS_PS_HANDOVER */
              /* Get next queue item */
              q_item = q_next_item;
            }
          }
        }
#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP
        {
          /* Reset all SNDCP XID parameters to their default values.
          ** The default value for a compression entity is non-existing.
          ** See TS 04.65 par 6.8.2
          */
          t1_alg_q_t        *q_item;
          t1_alg_q_t        *q_next_item;
          int8              alg;

          for( alg = 0; alg <= T1_ALG_MAX; alg++ )
          {
            q_item = q_check( &sapi_ptr->t1_alg_q[alg] );
            while( q_item != NULL )
            {
              q_next_item = q_next( &sapi_ptr->t1_alg_q[alg], &q_item->link );

              /* old xid parameter is not set, reset to default xid params */
#ifdef FEATURE_GPRS_PS_HANDOVER
              if(ll_mes->use_old_xid_ind == FALSE)
              {
#endif /* FEATURE_GPRS_PS_HANDOVER */
                gsn_del_dcomp_e( gas_id, q_item->dcomp_e_ptr->ent_num, sapi_ptr );
#ifdef FEATURE_GPRS_PS_HANDOVER
              }
              else
              {
                switch(alg)
                {
                case T1_ALG_V42BIS:
                  /* Free memory allocated for dcomp tables */
                  gsn_v42_dealloc_tables( gas_id, &q_item->dcomp_e_ptr->comp_e,
                    q_item->dcomp_e_ptr->alg.xid_v42bis.p0 );
                  /* Create a new dcomp entity and init it with existing dcomp
                     XID params */
                  (void)gsn_v42bis_comp_init( gas_id, q_item->dcomp_e_ptr);
                  break;

                default:
                  MSG_GERAN_ERROR_0_G("GSN ""Invalid T1 alg");
                  break;
                }
              }
#endif /* FEATURE_GPRS_PS_HANDOVER */
              /* Get next queue item */
              q_item = q_next_item;
            }
          }
        }
#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */


        /* Treat all outstanding SNDCP->LLC request type primitives as not
        ** send.
        */
        if( (sapi_ptr->state == EST_PEND ) ||
            (sapi_ptr->state == DISC_PEND) )
        {
          sapi_ptr->state = IDLE;
        }

        sapi_ptr->xid_state = XID_IDLE;

        /*  For Every NSAPI, mapped to the SAPI, that uses Unack,
        **  set Send N-PDU Numbet to zero.
        **  For Every NSAPI, mapped to the SAPI, that uses Ack,
        **  enter RECOVERY state and suspend the trans of N-PDUs untill
        **  an SNSM_SEQUENCE.IND is received for the NSAPI
        */

        item_ptr = (gsn_nsapi_q_t*)q_check( &(sapi_ptr->nsapi_ptr_q) );

        if( item_ptr )
        {
          while( item_ptr != NULL )
          {
            if( item_ptr->nsapi_ptr->op_mode == UNACK )
            {
              item_ptr->nsapi_ptr->ule.unack_send_npdu = 0;
            }
            if( item_ptr->nsapi_ptr->op_mode == ACK )
            {
              gsn_enter_recovery( gas_id, item_ptr->nsapi_ptr );
              GSN_SET_SUSP_EVENT( gas_id,
                                  item_ptr->nsapi_ptr->nsapi,
                                  GSN_UL_SUSP_SEQ_PEND );
              GSN_SET_SUSP_EVENT( gas_id,
                                  item_ptr->nsapi_ptr->nsapi,
                                  GSN_UL_SUSP_EST_PEND );
            }
            /* Get next NSAPI from the list */
            item_ptr = (gsn_nsapi_q_t*)q_next( &(sapi_ptr->nsapi_ptr_q),
                                               &item_ptr->link );
          }
        }
        else
        {
          /* Something is wrong!! no NSAPIs mapped on an active SAPI */
          MSG_GERAN_ERROR_1_G("GSN ""LL %d: No mapped NSAPIs", gsn_extern_sapi[sapi]);
        }

        /* Initialise LLC SAPIs N201_I and N201_U */
        gsn_n201_i[gas_id][sapi] = gllc_ll_get_default_N201_I( gsn_extern_sapi[sapi] );
        gsn_n201_u[gas_id][sapi] = gllc_ll_get_default_N201_U( gsn_extern_sapi[sapi] );


        /* This reset indication affects every NSAPI operating in UNACK mode
        ** (i.e. Rel Class 3,4,5) that is connected to this SAPI. In each NSAPI
        ** operating in UNACK mode, remove segment references from each N-PDU
        ** item stored in it's retx buffer (npdu_buff). These are the N-PDU
        ** segments that RLC and LLC discarded due to XID reset. Hence,
        ** send these N-PDUs to LLC for retransmission.
        */

        item_ptr = (gsn_nsapi_q_t*)q_check( &(sapi_ptr->nsapi_ptr_q) );

        /* Read Anite GCF flag */
        rr_read_public_store( RRPS_MS_ANITE_GCF_FLAG, &b_anite_gcf_flag, gas_id );

        while( item_ptr != NULL )
        {
          if( item_ptr->nsapi_ptr->op_mode == UNACK )
          {
            /* Remove segment references in each N-PDU. */
            npdu_item_ptr = q_check( &(item_ptr->nsapi_ptr->ule.npdu_buff) );

            /* Free any segment references within this N-PDU. */
            while( npdu_item_ptr )
            {

              if( npdu_item_ptr->seg_ref_buff_valid == TRUE )
              {
                /* Get pointer to segment at the head of segment item queue. */
                seg_item_ptr = q_check( &(npdu_item_ptr->seg_ref_buff) );

                while( seg_item_ptr )
                {
                  /* Get pointer to the next segment in segment item queue. */
                  next_seg_item_ptr =
                    q_next( &npdu_item_ptr->seg_ref_buff, &seg_item_ptr->link );

                  /* Delete this segment of N-PDU from segment item queue. */
                  q_delete
                  (
                      &npdu_item_ptr->seg_ref_buff,
                      &seg_item_ptr->link
                  );

                  /* Free segment item. */
                  GSN_MEM_FREE( seg_item_ptr );

                  seg_item_ptr = next_seg_item_ptr;
                } /* while ( seg_item_ptr ) */

                /* Set seg_ref_buff as invalid for this NPDU */
                npdu_item_ptr->seg_ref_buff_valid = FALSE;

                /* Free memory, mutex etc. used by NPDU segment queue attached
                ** to NPDU.
                */
                q_destroy ( &npdu_item_ptr->seg_ref_buff );

              }
              else
              {
                MSG_GERAN_MED_0_G("GSN ""No seg ref buff");
              }

              /* Get pointer to next N-PDU item in this NSAPI's npdu_buff. */
              npdu_item_ptr =
                q_next( &(item_ptr->nsapi_ptr->ule.npdu_buff),
                        &npdu_item_ptr->link );
            } /* while( npdu_item_ptr ) */

            /* If GCF flag is not active then re-tx NPDUs in re-tx buffer. */
            if (!b_anite_gcf_flag)
            {
              /* Now retransmit N-PDUs in this NSAPI's npdu_buff to LLC. */
              gsn_unack_retr_npdu_hdlr( gas_id, item_ptr->nsapi_ptr->nsapi );
            }
          }
          /* Get next NSAPI queue item. */
          item_ptr = (gsn_nsapi_q_t*)q_next( &(sapi_ptr->nsapi_ptr_q),
                                               &item_ptr->link );
        } /* while( item_ptr != NULL ) */
      }
      else
      {
        MSG_GERAN_ERROR_2_G("GSN ""LL %d: NSAPI ptr queue invalid in SAPI state: %d",
          gsn_extern_sapi[sapi],sapi_ptr->state);
      }
    }
    break;

  default:

    MSG_GERAN_ERROR_2_G("GSN ""LL %d: Invalid SAPI state: %d",
      gsn_extern_sapi[sapi],sapi_ptr->state);

    break;

  }

  gsn_send_ll_reset_res( gas_id, sapi_ptr );
}


/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_LL_EST_IND()
===
===  DESCRIPTION
===
===   Processes LL_ESTABLISH indication send by LLC layer to inform SNDCP layer
===   about establishment or re-establishment of ACK peer-to-peer operation
===   for a SAPI in the LLC layer.
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
static void gsn_process_ll_est_ind( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr, ll_establish_ind_t  *ll_mes )
{
  dsm_item_type       *xid_neg_ptr = NULL;
  gsn_sapi_t          sapi = sapi_ptr->sapi;
  boolean             collision = FALSE;

  /* Check for collision */
  if( (sapi_ptr->xid_state == XID_CNF_PEND) || (sapi_ptr->xid_state == XID_EST_CNF_PEND) )
    collision = TRUE;   /* LL_EST_IND received after sending LL_XID_REQ */

   /* Switch on SAPI's state */
  switch( sapi_ptr->state )
  {
  case( IDLE ):
    {

      if( sapi_ptr->is_nsapi_ptr_q_valid )
      {

        gsn_nsapi_q_t           *item_ptr;

        /*
        ** Network initiated establishment during PDP Context Modification
        ** or local link establishment from LLC.
        */
        sapi_ptr->link_estab_originator = FALSE;

        sapi_ptr->tlli    = ll_mes->tlli;
        gsn_n201_u[gas_id][sapi]  = ll_mes->n201_u;
        gsn_n201_i[gas_id][sapi]  = ll_mes->n201_i;

        /* If XID block present, decode XID requested and send
        ** LL_ESTABLISH_RSP. Otherwise,just enter ESTABLISHED state.
        */
        if( ll_mes->l3_xid_req != NULL )
        {
          xid_neg_ptr = gsn_process_req_xid( gas_id, sapi_ptr, &ll_mes->l3_xid_req,
                                             collision, TRUE );

          /* Post message to LLC */
          gsn_send_ll_est_res( gas_id, sapi_ptr, xid_neg_ptr );
        }
        else
          sapi_ptr->state = ESTABLISHED;

        /* Check all the NSAPIs mapped to the SAPI for any response pending to
        ** SM.
        */
        item_ptr = (gsn_nsapi_q_t*)q_check( &(sapi_ptr->nsapi_ptr_q) );

        if( item_ptr )
        {
          while( item_ptr != NULL )
          {
            /* If XID collision, resume data flow */
            if( collision )
            {
              GSN_CLR_SUSP_EVENT( gas_id,
                                  item_ptr->nsapi_ptr->nsapi,
                                  GSN_UL_SUSP_XID_PEND );
            }

            if( item_ptr->nsapi_ptr->actv_resp_pend )
            {
              item_ptr->nsapi_ptr->actv_resp_pend = FALSE;

              /* Send SNSM_ACTIVATE_RSP to SM  */
              gsn_send_snsm_actv_res( gas_id, item_ptr->nsapi_ptr->nsapi );
            }

            if( (item_ptr->nsapi_ptr->modif_resp_pend    ) &&
                (item_ptr->nsapi_ptr->modif_est_resp_pend)    )
            {

              /* Peer link establishment indicated. Stop SGSN initiated
              ** link est failure timer.
              */
              (void)rex_clr_timer(
                 &item_ptr->nsapi_ptr->sgsn_initiated_link_est_fail_timer );

              item_ptr->nsapi_ptr->modif_est_resp_pend = FALSE;
              if(!(item_ptr->nsapi_ptr->modif_rel_resp_pend) )
              {
                /* Send SNSM_MODIFY_RSP to SM */
                item_ptr->nsapi_ptr->modif_resp_pend = FALSE;

                gsn_send_snsm_mod_res( gas_id, item_ptr->nsapi_ptr->nsapi);

                /* Modification procedure is completed, resume data flow */
                GSN_CLR_SUSP_EVENT( gas_id,
                                    item_ptr->nsapi_ptr->nsapi,
                                    GSN_UL_SUSP_EST_PEND );
              }
            }

            /* Get next NSAPI from the list */
            item_ptr = (gsn_nsapi_q_t*)q_next( &(sapi_ptr->nsapi_ptr_q),
                                               &item_ptr->link );

          } /* END while traversing the queue */
        }
        else
          MSG_GERAN_ERROR_1_G("GSN ""No NSAPIs mapped on an active SAPI= %d .", sapi);
      }
      else
      {
        MSG_GERAN_ERROR_1_G("GSN ""NSAPIs ptr queue invalid SAPI= %d .", sapi);
      }
    }

    break;
/*-------------------------------------------------------------------------------*/

  case( EST_PEND ):
    {

      if( sapi_ptr->is_nsapi_ptr_q_valid )
      {
        gsn_nsapi_q_t         *item_ptr;

        /* Collision. LL_EST_IND received after sending LL_EST_REQ */
        collision = TRUE;

        sapi_ptr->link_estab_originator = FALSE;

        /* Store primitive parameters */
        sapi_ptr->tlli    = ll_mes->tlli;
        gsn_n201_u[gas_id][sapi]  = ll_mes->n201_u;
        gsn_n201_i[gas_id][sapi]  = ll_mes->n201_i;

        /* if XID block present,decode XID requested and send LL_ESTABLISH_RSP.
        ** Otherwise, just enter ESTABLISHED state.
        */
        if( ll_mes->l3_xid_req != NULL )
        {
          /* Build XID response */
          xid_neg_ptr = gsn_process_req_xid( gas_id, sapi_ptr, &ll_mes->l3_xid_req,
                                             collision, TRUE );

          /* Post message to LLC */
          gsn_send_ll_est_res( gas_id, sapi_ptr, xid_neg_ptr );
        }
        else
          sapi_ptr->state = ESTABLISHED;

        /* Check all the NSAPIs mapped to the SAPI for masked
        ** UL input queue and Re-Tr events and any response pending to SM
        */
        item_ptr = (gsn_nsapi_q_t*)q_check( &(sapi_ptr->nsapi_ptr_q) );
        if( item_ptr )
        {
          while( item_ptr != NULL )
          {
            /* Clear suspend flag  */
            GSN_CLR_SUSP_EVENT( gas_id,
                                item_ptr->nsapi_ptr->nsapi,
                                GSN_UL_SUSP_EST_PEND );

            /* If XID collision, resume data flow */
            if( collision )
            {
              GSN_CLR_SUSP_EVENT( gas_id,
                                  item_ptr->nsapi_ptr->nsapi,
                                  GSN_UL_SUSP_XID_PEND );
            }

            if( (item_ptr->nsapi_ptr->op_mode == ACK) &&
                (sapi_ptr->re_establish ) )
            {
              /* NSAPI uses Ack mode and it's re-establishing,
              ** enter recovery state and prepare for Re-Tr N-PDU
              */
              sapi_ptr->re_establish = FALSE;
              gsn_enter_recovery( gas_id, item_ptr->nsapi_ptr );
              gsn_ul_prepare_retr( gas_id, item_ptr->nsapi_ptr->nsapi );
            }

            if( item_ptr->nsapi_ptr->actv_resp_pend )
            {
              item_ptr->nsapi_ptr->actv_resp_pend = FALSE;

              /* Send SNSM_ACTIVATE_RSP to SM  */
              gsn_send_snsm_actv_res( gas_id, item_ptr->nsapi_ptr->nsapi );
            }
            if( (item_ptr->nsapi_ptr->modif_resp_pend) &&
                (item_ptr->nsapi_ptr->modif_est_resp_pend) )
            {

              /* Peer link establishment indicated. Stop SGSN initiated
              ** link est failure timer.
              */

              (void)rex_clr_timer(
                 &item_ptr->nsapi_ptr->sgsn_initiated_link_est_fail_timer );

              item_ptr->nsapi_ptr->modif_est_resp_pend = FALSE;
              if(!(item_ptr->nsapi_ptr->modif_rel_resp_pend) )
              {
                item_ptr->nsapi_ptr->modif_resp_pend = FALSE;

                /* Send SNSM_MODIFY_RSP to SM   */
                gsn_send_snsm_mod_res( gas_id, item_ptr->nsapi_ptr->nsapi );

                /* Modification procedure is completed, resume data flow */
                GSN_CLR_SUSP_EVENT( gas_id,
                                    item_ptr->nsapi_ptr->nsapi,
                                    GSN_UL_SUSP_EST_PEND );
              }
            }
            /* Get next NSAPI from the list */
            item_ptr = (gsn_nsapi_q_t*)q_next( &(sapi_ptr->nsapi_ptr_q),
                                               &item_ptr->link );

          } /* END while traversing the queue */
        }
        else
        {
          /* something is wrong!! no NSAPIs mapped on an active SAPI !! */
          MSG_GERAN_ERROR_1_G("GSN ""No NSAPIs mapped on an active SAPI= %d .", sapi);
        }
      }
      else
      {
        MSG_GERAN_ERROR_1_G("GSN ""NSAPI ptr queue invalid SAPI= %d .", sapi);
      }
    }
    break;

/*-------------------------------------------------------------------------------*/
  case( ESTABLISHED ):
    {

      if( sapi_ptr->is_nsapi_ptr_q_valid )
      {

        gsn_nsapi_q_t         *item_ptr;

        /* Re-establishment initiated by LLC */
        sapi_ptr->re_establish = TRUE;
        sapi_ptr->link_estab_originator = FALSE;

        /* Store primitive parameters */
        sapi_ptr->tlli    = ll_mes->tlli;
        gsn_n201_u[gas_id][sapi]  = ll_mes->n201_u;
        gsn_n201_i[gas_id][sapi]  = ll_mes->n201_i;

        /* if XID block present, send LL_ESTABLISH_RSP, else just enter
        ** ESTABLISHED state
        */
        if( ll_mes->l3_xid_req != NULL )
        {
          xid_neg_ptr = gsn_process_req_xid( gas_id, sapi_ptr, &ll_mes->l3_xid_req,
                                             collision, TRUE );

          /* Post message to LLC */
          gsn_send_ll_est_res( gas_id, sapi_ptr, xid_neg_ptr );
        }
        else
          /* SAPI is established */
          sapi_ptr->state = ESTABLISHED;

        /* For every Ack NSAPI mapped to the SAPI,
        ** enter recovery state and set N-PDU Re-Tr event
        */
        item_ptr = (gsn_nsapi_q_t*)q_check( &(sapi_ptr->nsapi_ptr_q) );
        if( item_ptr )
        {
          while( item_ptr != NULL )
          {
            /* If XID collision, resume data flow */
            if( collision )
            {
              GSN_CLR_SUSP_EVENT( gas_id,
                                  item_ptr->nsapi_ptr->nsapi,
                                  GSN_UL_SUSP_XID_PEND );
            }

            if( item_ptr->nsapi_ptr->op_mode == ACK )
            {
              /* Clear establish pending suspension flag set upon receiving
              ** LL_RESET_IND
              */

              /*
              GSN_CLR_SUSP_EVENT( gas_id,
                                  item_ptr->nsapi_ptr->nsapi,
                                  GSN_UL_SUSP_EST_PEND );
              */

              /* NSAPI uses Ack mode and it's re-establishing, reset comp
              ** entities, enter recovery state and prepare for Re-Tr N-PDU
              */
              sapi_ptr->re_establish = FALSE;

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )
              gsn_reset_comp_ent( gas_id, sapi_ptr );
#endif  /* FEATURE_GSM_GPRS_SNDCP_PCOMP || FEATURE_GSM_GPRS_SNDCP_DCOMP */

              gsn_enter_recovery( gas_id, item_ptr->nsapi_ptr );

              /* Remove any feedback PDUs in retransmit buffer before
              ** retransmission begins.
              */
              gsn_ack_remove_fback_npdus_and_renumber( gas_id, item_ptr->nsapi_ptr );

              gsn_ul_prepare_retr(  gas_id, item_ptr->nsapi_ptr->nsapi );
            }
            /* Get next NSAPI from the list */
            item_ptr = (gsn_nsapi_q_t*)q_next( &(sapi_ptr->nsapi_ptr_q),
                                               &item_ptr->link );

          } /* END while traversing the queue */
        }
        else
        {
          /* something is wrong!! no NSAPIs mapped on an active SAPI !! */
          MSG_GERAN_ERROR_1_G("GSN ""LL %d: No NSAPIs mapped on active SAPI",
                        gsn_extern_sapi[sapi]);
        }
      }
      else
      {
        MSG_GERAN_ERROR_1_G("GSN ""NSAPI ptr queue invalid SAPI= %d .", sapi);
      }
    }
    break;

/*-------------------------------------------------------------------------------*/
  case( DISC_PEND ):

    MSG_GERAN_ERROR_0_G("GSN ""LL %d: Rcvd LL_EST_IND in state: DISC_PEND");
    break;

/*-------------------------------------------------------------------------------*/
  default:

    MSG_GERAN_ERROR_1_G("GSN ""EST_IND received in invalid SAPI state: %d", sapi_ptr->state);
    break;

/*-------------------------------------------------------------------------------*/
  }
}


/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_LL_EST_CNF()
===
===  DESCRIPTION
===
===   Processes LL_ESTABLISH confirm primitive send by LLC layer to inform
===   the SNDCP layer about successful initiation of ACK peer-to-peer operation
===   for a SAPI in the LLC layer.
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
static void gsn_process_ll_est_cnf( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr, ll_establish_cnf_t  *ll_mes )
{
  gsn_sapi_t    sapi = sapi_ptr->sapi;
  dsm_item_type *gsn_xid_req;

   /* Switch on SAPI's state */
  switch( sapi_ptr->state )
  {
/*-------------------------------------------------------------------------------*/
  case( EST_PEND ):
    {
      gsn_nsapi_q_t         *item_ptr;

      /* Store primitive parameters */
      sapi_ptr->tlli    = ll_mes->tlli;
      gsn_n201_u[gas_id][sapi]  = ll_mes->n201_u;
      gsn_n201_i[gas_id][sapi]  = ll_mes->n201_i;

      /*  If XID block present, check if the negotiated XID
      **  are supported by the SNDCP layer.
      */

      if( gsn_decode_neg_xid( gas_id, &ll_mes->l3_xid_neg, sapi_ptr ) )
      {
        /* XID negotiated have been accepted */
        sapi_ptr->state  = ESTABLISHED;
        if( sapi_ptr->xid_state != XID_IDLE )
          sapi_ptr->xid_state = XID_REQUESTED;

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )

        /* Delete duplicate of XID block requested */
        if( sapi_ptr->xid_req_dup_ptr )
          GPRS_PDU_FREE( &sapi_ptr->xid_req_dup_ptr );

#endif
      }
      else
      {
        /* XID negotiated differ from the XID requested. Reinitiate the XID
        ** request. If reinitiation failed for XID_REQ_MAX number of times,
        ** inform SM with SNSM_STATUS_REQ with cause "Invalid XID response".
        */

        if( sapi_ptr->xid_req_cnt < XID_REQ_MAX )
        {
          /* Increment counter */
          sapi_ptr->xid_req_cnt += 1;

          /* Set SAPI's state to IDLE */
          sapi_ptr->state = IDLE;

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )

          /* Duplicate XID pdu and then retransmit */
          if( sapi_ptr->xid_req_dup_ptr )
          {
            gsn_xid_req = gprs_pdu_duplicate( sapi_ptr->xid_req_dup_ptr, 0,
                            GPRS_PDU_LENGTH(sapi_ptr->xid_req_dup_ptr) );

            if( gsn_xid_req == NULL )
            {
              /* Run out of dsm resources. Send default xid parameters (no compression) */
              gsn_xid_req = gsn_encode_def_xid();

              /* Free dsm item conveying the xid pdu */
              GPRS_PDU_FREE( &sapi_ptr->xid_req_dup_ptr );

              MSG_GERAN_ERROR_0_G("GSN ""gprs_pdu_duplicate() failed. No dsm resources");
            }

          }
          else
          {
            gsn_xid_req = gsn_encode_def_xid();
          }
#else
          /* The default XID need to be sent to the peer */
          gsn_xid_req = gsn_encode_def_xid();
#endif

          /* Post message to LLC */
          gsn_send_ll_est_req( gas_id, sapi_ptr, gsn_xid_req );
        }
        else
        {
          /* initialise counter */
          sapi_ptr->xid_req_cnt = 0;

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )

          /* Delete duplicate of XID block requested */
          if( sapi_ptr->xid_req_dup_ptr )
            GPRS_PDU_FREE( &sapi_ptr->xid_req_dup_ptr );
#endif
          /* Inform SM with SNSM_STATUS_REQ with cause "Invalid XID response" */
          gsn_send_snsm_stat_req( gas_id,
                                  sapi_ptr->sapi,
                                  sapi_ptr->tlli,
                                  (uint8) SNSM_INVALID_XID_RESP );

        }
      }

      if( sapi_ptr->state == ESTABLISHED )
      {

        if( sapi_ptr->is_nsapi_ptr_q_valid )
        {
          /* Check all the NSAPIs mapped on the SAPI for suspended
          ** UL data flow and any response pending to SM.
          */
          item_ptr = (gsn_nsapi_q_t*)q_check( &(sapi_ptr->nsapi_ptr_q) );
          if( item_ptr )
          {
            while( item_ptr != NULL )
            {
              /* Clear suspend flag  */
              GSN_CLR_SUSP_EVENT( gas_id,
                                  item_ptr->nsapi_ptr->nsapi,
                                  GSN_UL_SUSP_EST_PEND );

              if( (item_ptr->nsapi_ptr->op_mode == ACK) &&
                  (sapi_ptr->re_establish ) )
              {
                /* NSAPI uses Ack mode and it's re-establishing, reset comp
                ** entities, enter recovery state and prepare to Re-Tr N-PDU
                */
                sapi_ptr->re_establish = FALSE;
#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )
                gsn_reset_comp_ent( gas_id, sapi_ptr );
#endif  /* FEATURE_GSM_GPRS_SNDCP_PCOMP || FEATURE_GSM_GPRS_SNDCP_DCOMP */
                gsn_enter_recovery( gas_id, item_ptr->nsapi_ptr );

                /* Remove any feedback PDUs in retransmit buffer before
                ** retransmission begins.
                */
                gsn_ack_remove_fback_npdus_and_renumber( gas_id, item_ptr->nsapi_ptr );
                gsn_ul_prepare_retr( gas_id, item_ptr->nsapi_ptr->nsapi );
              }

              if( item_ptr->nsapi_ptr->actv_resp_pend )
              {
                item_ptr->nsapi_ptr->actv_resp_pend = FALSE;

                /* Send SNSM_ACTIVATE_RSP to SM */
                gsn_send_snsm_actv_res( gas_id, item_ptr->nsapi_ptr->nsapi );
              }
              if( (item_ptr->nsapi_ptr->modif_resp_pend) &&
                  (item_ptr->nsapi_ptr->modif_est_resp_pend) )
              {
                item_ptr->nsapi_ptr->modif_est_resp_pend = FALSE;
                if(!item_ptr->nsapi_ptr->modif_rel_resp_pend )
                {
                  item_ptr->nsapi_ptr->modif_resp_pend = FALSE;

                  /* send SNSM_MODIFY_RSP to SM */
                  gsn_send_snsm_mod_res( gas_id, item_ptr->nsapi_ptr->nsapi );
                }
              }

              /* Get next NSAPI from the list */
              item_ptr = (gsn_nsapi_q_t*)q_next( &(sapi_ptr->nsapi_ptr_q), &item_ptr->link );

            } /* END while traversing the queue */

          }
          else
          {
            /* something is wrong!! no NSAPIs mapped on an active SAPI !! */
            MSG_GERAN_ERROR_1_G("GSN ""No NSAPIs mapped on an active SAPI= %d .", sapi);
          }
        }
        else
        {
          MSG_GERAN_ERROR_1_G("GSN ""NSAPI ptr queue invalid on SAPI= %d .", sapi);
        }
      }
    }
    break;

/*-------------------------------------------------------------------------------*/
  case( IDLE ):
  case( DISC_PEND ):

    MSG_GERAN_ERROR_1_G("GSN ""EST_CNF received in invalid SAPI state: %d", sapi_ptr->state);
    break;

/*-------------------------------------------------------------------------------*/
  default:

    MSG_GERAN_ERROR_1_G("GSN ""EST_CNF received in invalid SAPI state: %d", sapi_ptr->state);
    break;

/*-------------------------------------------------------------------------------*/
  }
}

/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_LL_REL_IND()
===
===  DESCRIPTION
===
===   Processes LL_RELEASE indication send by LLC layer to inform SNDCP layer
===   about termination of ACK peer-to-peer LLC operation for a SAPI.
===
===  DEPENDENCIES
===
===   The order of the enum types snsm_cause_T and ll_cause_t must be of the same.
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
static void gsn_process_ll_rel_ind( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr, const ll_release_ind_t  *ll_mes )
{
  gsn_sapi_t     sapi      = sapi_ptr->sapi;
  dsm_item_type  *xid_ptr  = NULL;


#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )
  /* All compression entities using ACK LLC operation on this SAPI are reset */
  gsn_reset_comp_ent( gas_id, sapi_ptr );
#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP || FEATURE_GSM_GPRS_SNDCP_DCOMP */

   /* Switch on SAPI's state */
  switch( sapi_ptr->state )
  {
/*-------------------------------------------------------------------------------*/
  /* TS 04.65 par 6.2.1.4 */
  case( IDLE ):

    MSG_GERAN_ERROR_0_G("GSN ""REL_IND received while SAPI in state: IDLE");
    break;

/*-------------------------------------------------------------------------------*/
  case( EST_PEND ):
  case( ESTABLISHED):
    {
      /* Establishment procedure failed
      ** see TS 04.65 par 6.2.1.4
      */
      switch( ll_mes->cause )
      {
      case GPRS_LL_NORMAL_RELEASE:
        {
          gsn_nsapi_q_t       *item_ptr = NULL;
          gsn_nsapi_t         nsapi;

          /* In TS 04.65/6.2.1.4 implies that "normal release" should be send in an
          ** established SAPI during PDP Context modification Procedure TBC.
          */
          if( sapi_ptr->state == EST_PEND )
          {
            MSG_GERAN_ERROR_1_G("GSN ""LL %d: Rcvd LL_REL_IND Normal release while EST_PEND",
                         gsn_extern_sapi[sapi]);
          }

          /* PDP Context Modification TBC/TBD
          ** Buffer all downlink N-PDUs for Ack NSAPI's using the affected SAPI
          ** Start data flow after LL_ESTABLISH_IND.
          ** Unack NSAPI's mapped to the SAPI shall not be affected.
          */

          /* SAPI state back to idle */
          sapi_ptr->state = IDLE;

          if( sapi_ptr->xid_state == XID_EST_CNF_PEND )
          {
            sapi_ptr->xid_state = XID_IDLE;

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )

            /* Delete all SELECTED compression entities on the SAPI */
            gsn_del_all_selected_comp_e( gas_id, sapi_ptr );
#endif
          }

          if( sapi_ptr->is_nsapi_ptr_q_valid )
          {

            /* Find the PDP context that is been modified
            ** and check if a response is pending to SM
            */
            item_ptr = (gsn_nsapi_q_t*)q_check( &(sapi_ptr->nsapi_ptr_q) );
            if( item_ptr )
            {
              /* check if active NSAPI entity has a valid pointer */
              GSN_ASSERT( item_ptr->nsapi_ptr != NULL );

              while( item_ptr != NULL )
              {
                /* Clear suspend flag  */
                GSN_CLR_SUSP_EVENT( gas_id,
                                    item_ptr->nsapi_ptr->nsapi,
                                    GSN_UL_SUSP_XID_PEND );

                nsapi = item_ptr->nsapi_ptr->nsapi;
                if( (item_ptr->nsapi_ptr->modif_resp_pend) &&
                    (item_ptr->nsapi_ptr->modif_rel_resp_pend) )
                {
                  item_ptr->nsapi_ptr->modif_rel_resp_pend = FALSE;
                  if(!(item_ptr->nsapi_ptr->modif_est_resp_pend) )
                  {
                    item_ptr->nsapi_ptr->modif_resp_pend = FALSE;

                    /* Send SNSM_MODIFY_RSP to SM */
                    gsn_send_snsm_mod_res( gas_id, item_ptr->nsapi_ptr->nsapi);

                    /* Modification procedure is completed, resume data flow */
                    GSN_CLR_SUSP_EVENT( gas_id,
                                        item_ptr->nsapi_ptr->nsapi,
                                        GSN_UL_SUSP_EST_PEND );
                  }

                  if( item_ptr->nsapi_ptr->delink_nsapi ||
                      item_ptr->nsapi_ptr->init_nsapi     )
                  {
                    item_ptr->nsapi_ptr->delink_nsapi = FALSE;

                    /* Remove NSAPI from SAPI */
                    gsn_remove_nsapi_from_sapi( gas_id, item_ptr->nsapi_ptr->nsapi,
                                                sapi);
                  }

                  if ( actv_nsapis[ gas_id ][nsapi] != NULL )
                  {
                    if( actv_nsapis[ gas_id ][nsapi]->init_nsapi )
                    {
                      actv_nsapis[ gas_id ][nsapi]->init_nsapi = FALSE;

                      /* Deactivate NSAPI entity */
                      gsn_deactv_nsapi( gas_id, actv_nsapis[ gas_id ][nsapi] );
                    }
                    /* Leave while loop */
                    break;
                  }
                }

                /* Get next NSAPI from the list */
                item_ptr = (gsn_nsapi_q_t*)q_next( &(sapi_ptr->nsapi_ptr_q),
                                                   &item_ptr->link );

              } /* END while traversing */
            }
          }
          else
          {
            MSG_GERAN_ERROR_0_G("GSN "" NSAPI ptr queue invalid");
          }
        }
        break;

      case GPRS_LL_NO_PEER_RESPONSE:

        /* Process this case if MS SNDCP is the originator of the link establishment */
        if( sapi_ptr->link_estab_originator == TRUE)
        {
          /* Re-invoke the establisment procedure and inform SM using SNSM-STATUS.REQ
          ** with cause "no peer response"
          */
          if( sapi_ptr->state == EST_PEND )
            sapi_ptr->state = IDLE;

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )

          if( sapi_ptr->xid_req_dup_ptr )
          {
            xid_ptr = sapi_ptr->xid_req_dup_ptr;

            sapi_ptr->xid_req_dup_ptr =
              gprs_pdu_duplicate( xid_ptr, 0, GPRS_PDU_LENGTH(xid_ptr) );

            if( sapi_ptr->xid_req_dup_ptr == NULL )
            {
              MSG_GERAN_ERROR_1_G("GSN " "gprs_pdu_duplicate() failed. No dsm resources",
                0);
            }
          }
          else
          {
            MSG_GERAN_HIGH_1_G("GSN " "LL %d: xid_req_dup_ptr is NULL",
              gsn_extern_sapi[sapi_ptr->sapi]);
          }

#endif
          gsn_send_snsm_stat_req( gas_id,
                                  sapi_ptr->sapi,
                                  sapi_ptr->tlli,
                                  (uint8) SNSM_NO_PEER_RESP );

          gsn_send_ll_est_req( gas_id, sapi_ptr, sapi_ptr->xid_req_dup_ptr );
        }
        break;

      case GPRS_LL_DM_RECEIVED:

        /* Process this case if MS SNDCP is the originator of the link establishment */
        if( sapi_ptr->link_estab_originator == TRUE)
        {
          sapi_ptr->state = IDLE;

          /* Inform SM using SNSM_STATUS_REQ with cause "DM received" */
          gsn_send_snsm_stat_req( gas_id,
                                  sapi_ptr->sapi,
                                  sapi_ptr->tlli,
                                  (uint8) SNSM_DM_RECEIVED );

          /* No need to clear/initialise NSAPI parameters. NSAPI shall be deactevated by SM
          ** and the deactivation procedure will initialise the NSAPI entity.
          */
        }
        break;

      case GPRS_LL_INVALID_XID_RESP:

         /* Process this case if MS SNDCP is the originator of the link establishment */
        if( sapi_ptr->link_estab_originator == TRUE)
        {

          sapi_ptr->state = IDLE;

          /* Inform SM using SNSM-STATUS.REQ with cause "Invalid XID response" */
          gsn_send_snsm_stat_req( gas_id,
                                  sapi_ptr->sapi,
                                  sapi_ptr->tlli,
                                  (uint8) SNSM_INVALID_XID_RESP );

          /* No need to clear/initialise NSAPI parameters. NSAPI shall be deactevated by SM
          ** and the deactivation procedure will initialise the NSAPI entity.
          */
        }
        break;

      default:
        MSG_GERAN_ERROR_1_G("GSN ""Invalid LL_RELEASE_IND cause: %d", ll_mes->cause );
        break;
      }
      break;
    }

/*-------------------------------------------------------------------------------*/
  case( DISC_PEND ):

    MSG_GERAN_ERROR_0_G("GSN ""REL_IND received while SAPI in state: DISC_PEND");
    break;

/*-------------------------------------------------------------------------------*/
  default:

    MSG_GERAN_ERROR_2_G("GSN ""LL %d: Rcvd REL_IND in SAPI state: %d",
      gsn_extern_sapi[sapi_ptr->sapi],sapi_ptr->state);
    break;

/*-------------------------------------------------------------------------------*/
  }
}


/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_LL_REL_CNF()
===
===  DESCRIPTION
===
===   Processes LL_RELEASE request send by the LLC layer to inform the SNDCP
===   layer about termination of ACK peer-to-peer operation for a SAPI
===   in the LLC layer.
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
static void gsn_process_ll_rel_cnf( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr, ll_release_cnf_t *mes )
{
  gsn_sapi_t          sapi = sapi_ptr->sapi;

  NOTUSED(mes);

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )
  /* All compression entities using ACK LLC operation on this SAPI are reset */
  gsn_reset_comp_ent( gas_id, sapi_ptr );
#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP || FEATURE_GSM_GPRS_SNDCP_DCOMP */

   /* Switch on SAPI's state */
  switch( sapi_ptr->state )
  {
/*-------------------------------------------------------------------------------*/
  case( IDLE ):
  case( EST_PEND ):
  case( ESTABLISHED ):

    MSG_GERAN_ERROR_1_G("GSN ""REL_CNF received while SAPI in state: %d",sapi_ptr->state);
    break;

/*-------------------------------------------------------------------------------*/
  case( DISC_PEND ):
    {
      gsn_nsapi_q_t       *item_ptr = NULL;
      gsn_nsapi_t         nsapi;

      /* SAPI state back to idle */
      sapi_ptr->state = IDLE;


      if( sapi_ptr->is_nsapi_ptr_q_valid )
      {
        /* Find the NSAPI that sent the LL_RELEASE_REQ
        ** and check if a response is pending to SM
        */
        item_ptr = (gsn_nsapi_q_t*)q_check( &(sapi_ptr->nsapi_ptr_q) );
        if( item_ptr )
        {
          /* check if active NSAPI entity has a valid pointer */
          GSN_ASSERT( item_ptr->nsapi_ptr != NULL );

          while( item_ptr != NULL )
          {
            if( item_ptr->nsapi_ptr->deactv_resp_pend ||
              ( (item_ptr->nsapi_ptr->modif_resp_pend) &&
              (item_ptr->nsapi_ptr->modif_rel_resp_pend) ) )
            {

              nsapi = item_ptr->nsapi_ptr->nsapi;

              if( item_ptr->nsapi_ptr->deactv_resp_pend )
              {
                item_ptr->nsapi_ptr->deactv_resp_pend = FALSE;

                /* Send SNSM_DEACTIVATE_RSP to SM */
                gsn_send_snsm_deactv_res( gas_id, item_ptr->nsapi_ptr->nsapi );

              }
              if( (item_ptr->nsapi_ptr->modif_resp_pend) &&
                  (item_ptr->nsapi_ptr->modif_rel_resp_pend) )
              {
                item_ptr->nsapi_ptr->modif_rel_resp_pend = FALSE;
                if(!(item_ptr->nsapi_ptr->modif_est_resp_pend) )
                {
                  item_ptr->nsapi_ptr->modif_resp_pend = FALSE;

                  /* Send SNSM_MODIFY_RSP to SM  */
                  gsn_send_snsm_mod_res( gas_id, item_ptr->nsapi_ptr->nsapi);
                }
              }

              if( item_ptr->nsapi_ptr->delink_nsapi ||
                  item_ptr->nsapi_ptr->init_nsapi     )
              {
                item_ptr->nsapi_ptr->delink_nsapi = FALSE;

                /* Delink NSAPI from SAPI */
                gsn_remove_nsapi_from_sapi( gas_id, item_ptr->nsapi_ptr->nsapi, sapi);
              }

              if( actv_nsapis[ gas_id ][nsapi]->init_nsapi)
              {
                actv_nsapis[ gas_id ][nsapi]->init_nsapi = FALSE;

                /* Deactivate NSAPI entity */
                gsn_deactv_nsapi( gas_id, actv_nsapis[ gas_id ][nsapi] );
              }
              /* Leave while loop */
              break;
            }
            /* Get next NSAPI from the list */
            item_ptr = (gsn_nsapi_q_t*)q_next( &(sapi_ptr->nsapi_ptr_q), &item_ptr->link );

          } /* END while traversing the queue */
        }
        else
        {
          /* something is wrong!! no NSAPIs mapped on an active SAPI !! */
          MSG_GERAN_ERROR_1_G("GSN ""No NSAPIs mapped on an active SAPI= %d .", sapi);
        }
      }
      else
      {
        MSG_GERAN_ERROR_1_G("GSN ""NSAPI ptr queue invalid on SAPI= %d", sapi);
      }
    }
    break;

/*-------------------------------------------------------------------------------*/
  default:

    MSG_GERAN_ERROR_1_G("GSN ""REL_CNF received in invalid SAPI state: %d", sapi_ptr->state);
    break;

/*-------------------------------------------------------------------------------*/
  }
}


/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_LL_XID_IND()
===
===  DESCRIPTION
===
===   Processes LL_XID indication sent by LLC layer to deliver the requested
===   SNDCP XID parameters to the SNDCP layer.
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
static void gsn_process_ll_xid_ind( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr, ll_xid_ind_t  *ll_mes )
{
  gsn_sapi_t          sapi = sapi_ptr->sapi;

   /* Switch on SAPI's state */
  switch( sapi_ptr->state )
  {
/*-------------------------------------------------------------------------------*/
  case( IDLE ):
  case( ESTABLISHED ):
    {
      dsm_item_type         *xid_neg_ptr;
      boolean               collision = FALSE;
      gsn_nsapi_q_t         *item_ptr;

      /* Network (or LLC) initiated XID negotiation. Store primitive parameters */
      sapi_ptr->tlli    = ll_mes->tlli;
      gsn_n201_u[gas_id][sapi]  = ll_mes->n201_u;
      gsn_n201_i[gas_id][sapi]  = ll_mes->n201_i;

      /* Check if XID block present in the primitive.
      ** No XID block means, do not send LL_XID_RES
      */
      if( ll_mes->l3_xid_req != NULL )
      {
        /* Check for collision */
        if( sapi_ptr->xid_state == XID_CNF_PEND )
        {
          collision = TRUE; /* LL_XID_IND received after sending LL_XID_REQ */

          if( sapi_ptr->is_nsapi_ptr_q_valid )
          {
            /* Resume data flow (suspended after sending LL_XID_REQ)
            ** on the NSAPI's mapped on the sapi and send any pending
            ** responses to SM.
            */
            item_ptr = (gsn_nsapi_q_t*)q_check( &(sapi_ptr->nsapi_ptr_q) );

            while( item_ptr != NULL )
            {
              GSN_ASSERT( item_ptr->nsapi_ptr != NULL );

              /* Clear suspend flag  */
              GSN_CLR_SUSP_EVENT( gas_id,
                                  item_ptr->nsapi_ptr->nsapi,
                                  GSN_UL_SUSP_XID_PEND );

              if( item_ptr->nsapi_ptr->actv_resp_pend )
              {
                item_ptr->nsapi_ptr->actv_resp_pend = FALSE;

                /* Send SNSM_ACTIVATE_RSP to SM */
                gsn_send_snsm_actv_res( gas_id, item_ptr->nsapi_ptr->nsapi );

              }
              if( (item_ptr->nsapi_ptr->modif_resp_pend) &&
                  (item_ptr->nsapi_ptr->modif_est_resp_pend) )
              {

                /* Stop SGSN initiated link est failure timer. */

                (void)rex_clr_timer(
                   &item_ptr->nsapi_ptr->sgsn_initiated_link_est_fail_timer );

                item_ptr->nsapi_ptr->modif_est_resp_pend = FALSE;
                if(!item_ptr->nsapi_ptr->modif_rel_resp_pend )
                {
                  item_ptr->nsapi_ptr->modif_resp_pend = FALSE;

                  /* Send SNSM_MODIFY_RSP to SM */
                  gsn_send_snsm_mod_res( gas_id, item_ptr->nsapi_ptr->nsapi );

                  /* Modification procedure is completed, resume data flow */
                  GSN_CLR_SUSP_EVENT( gas_id,
                                      item_ptr->nsapi_ptr->nsapi,
                                      GSN_UL_SUSP_EST_PEND );
                }
              }

              /* Get next NSAPI from the list */
              item_ptr = (gsn_nsapi_q_t*)q_next( &(sapi_ptr->nsapi_ptr_q), &item_ptr->link );
            }
          }
          else
          {
            MSG_GERAN_ERROR_2_G("GSN ""LL %d: NSAPI ptr queue invalid in state: %d",
              gsn_extern_sapi[sapi],sapi_ptr->state);
          }
        }

        /* Decode XID requested */
        xid_neg_ptr = gsn_process_req_xid( gas_id, sapi_ptr, &ll_mes->l3_xid_req,
                                           collision, FALSE );

        /* Send XID response to peer */
        gsn_send_ll_xid_res( gas_id, sapi_ptr, xid_neg_ptr);

      }
    }

    break;

  case( EST_PEND ):
  case( DISC_PEND ):

    MSG_GERAN_ERROR_2_G("GSN ""LL %d: Rcvd LL_XID_IND in state: %d",
      gsn_extern_sapi[sapi],sapi_ptr->state);
    break;

/*-------------------------------------------------------------------------------*/
  default:

    MSG_GERAN_ERROR_2_G("GSN ""LL %d: Rcvd LL_XID_IND in state: %d",
      gsn_extern_sapi[sapi],sapi_ptr->state);
    break;

/*-------------------------------------------------------------------------------*/
  }

  if( ll_mes->l3_xid_req )
  {
    /* Deallocate memory used for conveying the XID params */
    GPRS_PDU_FREE( &ll_mes->l3_xid_req );
  }
}


/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_LL_XID_CNF()
===
===  DESCRIPTION
===
===   Processes LL_XID confirm primitive send by LLC layer to deliver the
===   negotiated SNDCP XID parameters to the SNDCP layer.
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
static void gsn_process_ll_xid_cnf( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr, ll_xid_cnf_t  *ll_mes )
{
  gsn_nsapi_q_t         *item_ptr;
  dsm_item_type         *gsn_xid_req;

  if( sapi_ptr == NULL )
  {
    MSG_GERAN_ERROR_0_G("GSN ""Invalid SAPI ptr");
    return;
  }

   /* Switch on SAPI's state */
  switch( sapi_ptr->state )
  {
/*-------------------------------------------------------------------------------*/

  case( IDLE ):
  case( ESTABLISHED ):
  case( DISC_PEND ): /* XID Request to remove compression entity from SAPI before disconnecting */

    /* Store primitive parameters */
    sapi_ptr->tlli                      = ll_mes->tlli;
    gsn_n201_u[gas_id][sapi_ptr->sapi]  = ll_mes->n201_u;
    gsn_n201_i[gas_id][sapi_ptr->sapi]  = ll_mes->n201_i;

    /* LL_XID_CNF is valid if an LL_XID_REQ has been send */
    if( sapi_ptr->xid_state == XID_CNF_PEND )
    {
      if( gsn_decode_neg_xid( gas_id, &ll_mes->l3_xid_neg, sapi_ptr ) )
      {
        /* XID negotiated have been accepted */
        sapi_ptr->xid_state = XID_REQUESTED;

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )

        /* Delete duplicate of XID block requested */
        if( sapi_ptr->xid_req_dup_ptr )
          GPRS_PDU_FREE( &sapi_ptr->xid_req_dup_ptr );
#endif
      }
      else
      {
        /* XID negotiated differ from the XID requested. Reinitiate the XID
        ** request. If reinitiation failed for XID_REQ_MAX number of times,
        ** inform SM with SNSM_STATUS_REQ with cause "Invalid XID response".
        */
        if( sapi_ptr->xid_req_cnt < XID_REQ_MAX )
        {
          /* Increment counter */
          sapi_ptr->xid_req_cnt += 1;

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )

          if( sapi_ptr->xid_req_dup_ptr )
          {
            /* Duplicate XID pdu and then retransmit */
            gsn_xid_req = gprs_pdu_duplicate( sapi_ptr->xid_req_dup_ptr, 0,
                          GPRS_PDU_LENGTH(sapi_ptr->xid_req_dup_ptr) );

            if( gsn_xid_req == NULL )
            {
              /* Run out of dsm resources. Send default xid parameters (no compression) */
              gsn_xid_req = gsn_encode_def_xid();

              /* Free dsm item conveying the xid pdu */
              GPRS_PDU_FREE( &sapi_ptr->xid_req_dup_ptr );

              MSG_GERAN_ERROR_0_G("GSN ""gprs_pdu_duplicate() failed. No dsm resources ");
            }
          }
          else
          {
            gsn_xid_req = gsn_encode_def_xid();
          }
#else
          /* The default XID need to be sent to the peer.*/
          gsn_xid_req = gsn_encode_def_xid();
#endif
          /* Send message */
          gsn_send_ll_xid_req( gas_id, sapi_ptr, gsn_xid_req);
        }
        else
        {
          /* initialise counter */
          sapi_ptr->xid_req_cnt = 0;

          sapi_ptr->xid_state = XID_IDLE;

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )

          /* Delete duplicate of XID block requested */
          if( sapi_ptr->xid_req_dup_ptr )
            GPRS_PDU_FREE( &sapi_ptr->xid_req_dup_ptr );
#endif
          /* inform SM with SNSM_STATUS_REQ with cause "Invalid XID response" */
          gsn_send_snsm_stat_req( gas_id,
                                  sapi_ptr->sapi,
                                  sapi_ptr->tlli,
                                  (uint8) SNSM_INVALID_XID_RESP );
        }
      }

      if( sapi_ptr->xid_state == XID_REQUESTED )
      {

        if( sapi_ptr->is_nsapi_ptr_q_valid )
        {

          /* XID negotiation finished.
          ** Resume data flow (suspended after sending LL_XID_REQ)
          ** on the NSAPIs mapped on the sapi and send any pending
          ** responses to SM.
          */
          item_ptr = (gsn_nsapi_q_t*)q_check( &(sapi_ptr->nsapi_ptr_q) );

          while( item_ptr != NULL )
          {
            GSN_ASSERT( item_ptr->nsapi_ptr != NULL );

            /* Clear suspend flag  */
            GSN_CLR_SUSP_EVENT( gas_id,
                                item_ptr->nsapi_ptr->nsapi,
                                GSN_UL_SUSP_XID_PEND );

            if( item_ptr->nsapi_ptr->actv_resp_pend )
            {
              item_ptr->nsapi_ptr->actv_resp_pend = FALSE;

              /* Send SNSM_ACTIVATE_RSP to SM */
              gsn_send_snsm_actv_res( gas_id, item_ptr->nsapi_ptr->nsapi );
            }
            if( (item_ptr->nsapi_ptr->modif_resp_pend) &&
                (item_ptr->nsapi_ptr->modif_est_resp_pend) )
            {
              item_ptr->nsapi_ptr->modif_est_resp_pend = FALSE;
              if(!item_ptr->nsapi_ptr->modif_rel_resp_pend )
              {
                item_ptr->nsapi_ptr->modif_resp_pend = FALSE;

                /* Send SNSM_MODIFY_RSP to SM */
                gsn_send_snsm_mod_res( gas_id, item_ptr->nsapi_ptr->nsapi );

                /* Modification procedure is completed, resume data flow */
                GSN_CLR_SUSP_EVENT( gas_id,
                                    item_ptr->nsapi_ptr->nsapi,
                                    GSN_UL_SUSP_EST_PEND );
              }
            }
            if(item_ptr->nsapi_ptr->deactv_resp_pend)
            {
              /* Release SAPI Ack LLC operation if SAPI is used only by this NSAPI  */
              if(!gsn_release_sapi( gas_id,
                                    item_ptr->nsapi_ptr->nsapi,
                                    actv_nsapis[ gas_id ][item_ptr->nsapi_ptr->nsapi]->sapi_ptr->sapi,
                                    FALSE))
              {
                gsn_send_snsm_deactv_res( gas_id, item_ptr->nsapi_ptr->nsapi );
                item_ptr->nsapi_ptr->deactv_resp_pend = FALSE;
                /* Deactivate NSAPI entity */
                gsn_deactv_nsapi( gas_id, actv_nsapis[ gas_id ][item_ptr->nsapi_ptr->nsapi] );
              }
            }

            /* Get next NSAPI from the list */
            item_ptr = (gsn_nsapi_q_t*)q_next( &(sapi_ptr->nsapi_ptr_q),
                                               &item_ptr->link );
          }
        }
        else
        {
          MSG_GERAN_ERROR_1_G("GSN ""LL %d: NSAPI ptr queue invalid ",
            gsn_extern_sapi[sapi_ptr->sapi]);
        }
      }
    }
    else
    {
      MSG_GERAN_ERROR_1_G("GSN ""LL %d: invalid LL_XID_CNF ",
        gsn_extern_sapi[sapi_ptr->sapi]);
    }
    break;

  case( EST_PEND ):
  default:

    MSG_GERAN_ERROR_1_G("GSN ""LL %d: LL_XID_CNF in inval state ",
        gsn_extern_sapi[sapi_ptr->sapi]);

    break;

/*-------------------------------------------------------------------------------*/
  }

  if( ll_mes->l3_xid_neg )
  {
    /* Deallocate memory used for conveying the XID params */
    GPRS_PDU_FREE( &ll_mes->l3_xid_neg );
  }
}


/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_LL_STAT_IND()
===
===  DESCRIPTION
===
===   Processes LL_STATUS indication primitive sent by LLC layer to inform the
===   SNDCP layer when an error that cannot be corrected by the LLC has occurred.
===   The Cause parameter indicates the cause of the failure.
===
===  DEPENDENCIES
===
===   Enum types must have the same order :snsm_cause_T in sm_sndcp.h
===   and ll_cause_t in gllsap.h
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
static void gsn_process_ll_stat_ind( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr, const ll_status_ind_t  *ll_mes )
{
  gsn_nsapi_q_t       *item_ptr;

   /* Switch on SAPI's state */
  switch( sapi_ptr->state )
  {
/*-------------------------------------------------------------------------------*/
  case( IDLE ):
    {
      /* only valid if LL_XID.REQ has been send to the SAPI */
      if( sapi_ptr->xid_state == XID_CNF_PEND )
      {
        switch( ll_mes->cause )
        {
        case GPRS_LL_NO_PEER_RESPONSE:

          /* Inform SM using SNSM-STATUS.REQ with cause "no peer response"
          ** Set timer to re-invoke the XID negotiation procedure. TBC
          */

          gsn_send_snsm_stat_req( gas_id,
                                  sapi_ptr->sapi,
                                  sapi_ptr->tlli,
                                  (uint8) SNSM_NO_PEER_RESP );

          break;

        case GPRS_LL_DM_RECEIVED:

          sapi_ptr->state = IDLE;

          /* Inform SM using SNSM_STATUS_REQ with cause "DM received" */
          gsn_send_snsm_stat_req( gas_id,
                                  sapi_ptr->sapi,
                                  sapi_ptr->tlli,
                                  (uint8) SNSM_DM_RECEIVED );

          /* No need to clear/initialise NSAPI parameters. NSAPI shall be deactevated by SM
          ** and the deactivation procedure will initialise the NSAPI entity
          */

          break;

        case GPRS_LL_INVALID_XID_RESP:

          sapi_ptr->state = IDLE;

          /* Inform SM using SNSM-STATUS.REQ with cause "Invalid XID response" */
          gsn_send_snsm_stat_req( gas_id,
                                  sapi_ptr->sapi,
                                  sapi_ptr->tlli,
                                  (uint8) SNSM_INVALID_XID_RESP );

          /* No need to clear/initialise NSAPI parameters. NSAPI shall be deactevated by SM
          ** and the deactivation procedure will initialise the NSAPI entity
          */
          break;

        default:
          MSG_GERAN_ERROR_2_G("GSN ""ME: Rcvd Invalid LL_STATUS_IND cause: %d, state: %d",
                 ll_mes->cause ,sapi_ptr->state);
          break;
        }


        /* Delete all SELECTED compression entities on the SAPI */
        if( sapi_ptr->xid_state == XID_CNF_PEND )
        {
          sapi_ptr->xid_state = XID_IDLE;

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )

          /* Delete all SELECTED compression entities on the SAPI */
          gsn_del_all_selected_comp_e( gas_id, sapi_ptr );

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP || FEATURE_GSM_GPRS_SNDCP_DCOMP */

        }

        if( sapi_ptr->is_nsapi_ptr_q_valid )
        {

          /* XID negotiation finished.
          ** Resume data flow (suspended after sending LL_XID_REQ)
          ** on the NSAPIs mapped on the sapi and send any pending
          ** responses to SM.
          */
          item_ptr = (gsn_nsapi_q_t*)q_check( &(sapi_ptr->nsapi_ptr_q) );

          while( item_ptr != NULL )
          {
            GSN_ASSERT( item_ptr->nsapi_ptr != NULL );

            /* Clear suspend flag  */
            GSN_CLR_SUSP_EVENT( gas_id,
                                item_ptr->nsapi_ptr->nsapi,
                                GSN_UL_SUSP_XID_PEND );

            if( item_ptr->nsapi_ptr->actv_resp_pend )
            {
              item_ptr->nsapi_ptr->actv_resp_pend = FALSE;

              /* Send SNSM_ACTIVATE_RSP to SM */
              gsn_send_snsm_actv_res( gas_id, item_ptr->nsapi_ptr->nsapi );
            }
            if( (item_ptr->nsapi_ptr->modif_resp_pend) &&
                (item_ptr->nsapi_ptr->modif_est_resp_pend) )
            {

              /* Stop SGSN initiated link est failure timer. */
              (void)rex_clr_timer(
                 &item_ptr->nsapi_ptr->sgsn_initiated_link_est_fail_timer );

              item_ptr->nsapi_ptr->modif_est_resp_pend = FALSE;
              if(!item_ptr->nsapi_ptr->modif_rel_resp_pend )
              {
                item_ptr->nsapi_ptr->modif_resp_pend = FALSE;

                /* Send SNSM_MODIFY_RSP to SM */
                gsn_send_snsm_mod_res( gas_id, item_ptr->nsapi_ptr->nsapi );

                /* Modification procedure is completed, resume data flow */
                GSN_CLR_SUSP_EVENT( gas_id,
                                    item_ptr->nsapi_ptr->nsapi,
                                    GSN_UL_SUSP_EST_PEND );
              }
            }

            /* Get next NSAPI from the list */
            item_ptr = (gsn_nsapi_q_t*)q_next( &(sapi_ptr->nsapi_ptr_q),
                                               &item_ptr->link );
          }
        }
        else
        {
          MSG_GERAN_ERROR_1_G("GSN ""NSAPI ptr queue invalid in state: %d",
                        sapi_ptr->state);
        }
      }
    }
    break;

  case( EST_PEND ):
  case( DISC_PEND ):
  case( ESTABLISHED ):

    /* LL_STATUS_IND only valid in Unack NSAPIs i.e. only in SAPI's IDLE state.
    ** LL_RELEASE.IND for LLC status in Ack NSAPIs
    */
    MSG_GERAN_ERROR_1_G("GSN ""STATUS_IND received while SAPI in state: %d", sapi_ptr->state);
    break;

/*-------------------------------------------------------------------------------*/
    default:

    MSG_GERAN_ERROR_1_G("GSN ""STATUS_IND received in invalid SAPI state: %d", sapi_ptr->state);
    break;

/*-------------------------------------------------------------------------------*/
  }
}

/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_SNSM_ACTV_IND()
===
===  DESCRIPTION
===
===   Processes the SNSM_ACTIVATE indication primitive send by SM to inform
===   the SNDCP that an NSAPI has been activated for data transfer.
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
static void gsn_process_snsm_actv_ind( gas_id_t gas_id, const gsnsm_activ_ind_type  *smsn_mes_ptr )
{

  gsn_nsapi_e_t         *nsapi_e_ptr;
  gsn_nsapi_t           nsapi;
  gsn_sapi_t            new_sapi;
  dsm_item_type         *gsn_xid_req = NULL;


  /* Check if SAPI & NSAPI are valid */
  if( GSN_VALID_SAPI( smsn_mes_ptr->sapi  )  &&
      GSN_VALID_NSAPI( smsn_mes_ptr->nsapi) )
  {
    /* Translate SAPI & NSAPI values for internal use */
    nsapi = GSN_INTERN_NSAPI( smsn_mes_ptr->nsapi );
    new_sapi = (gsn_sapi_t) gsn_intern_sapi[ smsn_mes_ptr->sapi ];

#ifndef FEATURE_GSM_GPRS_SNDCP_PCOMP
    if((smsn_mes_ptr->pcomp_alg >= SYS_PDP_HEADER_COMP_ON) &&
       (smsn_mes_ptr->pcomp_alg <= SYS_PDP_HEADER_COMP_RFC3095))
    {
      /* Header compression requested while PCI compression
      ** feature is not switched on
      */
      MSG_GERAN_HIGH_0_G("GSN " "ME: PCI compression is not enabled");

    }
#endif

#ifndef FEATURE_GSM_GPRS_SNDCP_DCOMP
    if((smsn_mes_ptr->dcomp_alg == SYS_PDP_DATA_COMP_ON) ||
       (smsn_mes_ptr->dcomp_alg == SYS_PDP_DATA_COMP_V42_BIS))
    {
      /* Data compression requested while data compression
      ** feature is not switched on
      */
      MSG_GERAN_HIGH_0_G("GSN " "ME: Data compression is not enabled");

    }
#endif

    /* Activate NSAPI and SAPI entities */
    if ( actv_nsapis[ gas_id ][nsapi] == NULL )
    {
      gsn_activate_nsapi( gas_id, GSNSM_ACTIVATE_IND, (void *)smsn_mes_ptr );
    }
    else
    {
      /* Activation is attempted in an active NSAPI.
      ** The cleaner way to avoid ERROR_FATAL is to force an NSAPI
      ** deactivation by informing SM using SNSM_STATUS_REQ with
      ** cause "DM received".
      */
      gsn_send_snsm_stat_req( gas_id,
                              new_sapi,
                              smsn_mes_ptr->tlli,
                              (uint8) SNSM_DM_RECEIVED );

      /* ERROR. Activation requested on an active NSAPI */
      MSG_GERAN_ERROR_1_G("GSN ""ME: Activ req on active NSAPI %d",
                        GSN_EXTERN_NSAPI(nsapi));
      return;
    }
    gsn_activate_sapi ( gas_id, new_sapi, smsn_mes_ptr->tlli );

    /* link NSAPI with SAPI */
    gsn_map_nsapi_on_sapi( gas_id, nsapi, new_sapi );

    /* Get NSAPI entity pointer and check for its validity */
    nsapi_e_ptr = actv_nsapis[ gas_id ][nsapi];
    GSN_ASSERT( nsapi_e_ptr != NULL );

    /* SNSM_ACTIVATE_RSP is pending */
    nsapi_e_ptr->actv_resp_pend = TRUE;

    /* If Ack LLC operation required and is not established,
    ** establish Ack LLC operation on this SAPI.
    */
    if( (nsapi_e_ptr->op_mode == ACK) &&
        (nsapi_e_ptr->sapi_ptr->state == IDLE) )
    {

      /* Originate SNDCP XID. Encode XID to be requested */
      gsn_xid_req = gsn_encode_req_xid( gas_id, nsapi_e_ptr );

      /* Post message to LLC */
      gsn_send_ll_est_req(gas_id, nsapi_e_ptr->sapi_ptr, gsn_xid_req  );

    }
    else if( (nsapi_e_ptr->sapi_ptr->state == ESTABLISHED) ||
             ((nsapi_e_ptr->op_mode == UNACK)  &&
              (nsapi_e_ptr->sapi_ptr->state == IDLE))  )
    {
      /* Encode XID to be requested */
      gsn_xid_req = gsn_encode_req_xid( gas_id, nsapi_e_ptr );

      /* If there are XID to be proposed to the peer,
      ** then originate SNDCP XID request. Otherwise, send
      ** ACTIVATE RESPONSE to SM.
      */
      if( gsn_xid_req != NULL )
      {
        gsn_send_ll_xid_req( gas_id, nsapi_e_ptr->sapi_ptr, gsn_xid_req);
      }
      else
      {
        /* Inform LLC */
        gsn_send_ll_activate_ind( gas_id, nsapi_e_ptr->sapi_ptr );

        if( nsapi_e_ptr->actv_resp_pend )
        {
          gsn_send_snsm_actv_res( gas_id, nsapi_e_ptr->nsapi );

          /* Clear response pending flag */
          nsapi_e_ptr->actv_resp_pend = FALSE;
        }
      }
    }
    else
    {
      /* SM should not send an SNSM_ACTIVATE_IND while SNSM_ACTIVATE_RSP
      ** or SNSM_DEACTIVATE_RSP is pending on an NSAPI
      */
      MSG_GERAN_ERROR_2_G("GSN ""ME: NSAPI %d, SAPI %d in Inval state",
       GSN_EXTERN_NSAPI( nsapi ),gsn_extern_sapi[nsapi_e_ptr->sapi_ptr->sapi]);
    }
  }
  else
    MSG_GERAN_ERROR_2_G("GSN ""ME: Rcvd ACTIVATE_IND with inval NSAPI %d or SAPI %d",
        smsn_mes_ptr->nsapi,smsn_mes_ptr->sapi);
}


/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_SNSM_DEACTV_IND()
===
===  DESCRIPTION
===
===   Processes the SNSM_DEACTIVATE indication primitive send by SM to inform
===   the SNDCP that an NSAPI has been deactivated and cannot be used by
===   the SNDCP anymore.
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
static void gsn_process_snsm_deactv_ind( gas_id_t gas_id, const gsnsm_deactiv_ind_type  *mes_ptr )
{
  uint8               i;
  gsn_nsapi_t         nsapi;
  dsm_item_type       *req_xid_ptr;

  /* Get NSAPIs for deactivation */
  for( i = 0; i < mes_ptr->num_nsapi; i++ )
  {
    /* Check for invalid NSAPI number or inactive NSAPI */
    if( (!GSN_VALID_NSAPI( mes_ptr->nsapi[i] ) )  ||
        (actv_nsapis[ gas_id ][GSN_INTERN_NSAPI( mes_ptr->nsapi[i])] == NULL) )
    {
      MSG_GERAN_HIGH_1_G("GSN ""ME: Invalid or Inactive NSAPI %d",mes_ptr->nsapi[i]);
      gsn_send_snsm_deactv_res( gas_id, GSN_INTERN_NSAPI( mes_ptr->nsapi[i]) );
    }
#ifdef FEATURE_DUAL_SIM
    /* Proceed further only if received GAS ID is same as that stored in
    ** active NSAPI.
    */
    else if( actv_nsapis[ gas_id ][GSN_INTERN_NSAPI( mes_ptr->nsapi[i] )]->gas_id != gas_id )
    {
      MSG_GERAN_ERROR_3_G("GSN ""GSN: Invaild GAS_ID in Deact Ind, Curr: %d, Rcvd: %d",
        actv_nsapis[ gas_id ][GSN_INTERN_NSAPI( mes_ptr->nsapi[i] )]->gas_id,
        gas_id,
        0);
      gsn_send_snsm_deactv_res( gas_id, GSN_INTERN_NSAPI( mes_ptr->nsapi[i]) );
    }
#endif /* FEATURE_DUAL_SIM */
    else
    {
      nsapi = (gsn_nsapi_t) GSN_INTERN_NSAPI( mes_ptr->nsapi[i] );

      /* SNSM_DEACTIVATE_RSP is pending */
      actv_nsapis[ gas_id ][nsapi]->deactv_resp_pend = TRUE;

      /* NSAPI need to be initialized after LLC link is released */
      actv_nsapis[ gas_id ][nsapi]->init_nsapi = TRUE;

      /* If a compression entity is used by the NSAPI, remove the NSAPI
      ** from the "Applicable NSAPIs" of the compression entity and send
      ** the XID to the peer.
      ** Note that we only send the XID to the peer if LLC is not
      ** deactivating locally.
      */
      if( !mes_ptr->local )
      {
        req_xid_ptr = gsn_encode_rmv_nsapi_xid( gas_id, actv_nsapis[ gas_id ][nsapi] );

        if( req_xid_ptr != NULL )
          gsn_send_ll_xid_req( gas_id, actv_nsapis[ gas_id ][nsapi]->sapi_ptr, req_xid_ptr );
      }
      else
      {
        /* Release SAPI Ack LLC operation if SAPI is used only by this NSAPI  */
        if(!gsn_release_sapi( gas_id,
                              nsapi,
                              actv_nsapis[ gas_id ][nsapi]->sapi_ptr->sapi,
                              mes_ptr->local) )
        {
          gsn_send_snsm_deactv_res( gas_id, nsapi );
          actv_nsapis[ gas_id ][nsapi]->deactv_resp_pend = FALSE;
          /* Deactivate NSAPI entity */
          gsn_deactv_nsapi( gas_id, actv_nsapis[ gas_id ][nsapi] );
        }
      }
    }
  }
}

/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_SNSM_MOD_IND()
===
===  DESCRIPTION
===
===   Processes the SNSM_MODIFY indication primitive sent by SM to trigger a
===   change of the QoS profile for an NSAPI and indication of the SAPI to
===   be used.
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
static void gsn_process_snsm_mod_ind( gas_id_t gas_id, const gsnsm_modify_ind_type  *smsn_mes_ptr )
{
  gsn_nsapi_e_t       *nsapi_e_ptr;
  boolean             nsapi_ack_to_unack = FALSE;
  boolean             nsapi_unack_to_ack = FALSE;
  gsn_nsapi_t         nsapi;
  gsn_sapi_t          new_sapi, old_sapi;

  /* Check if SAPI & NSAPI are valid */
  if( !GSN_VALID_SAPI( smsn_mes_ptr->sapi  )  ||
      !GSN_VALID_NSAPI( smsn_mes_ptr->nsapi) )
  {
    MSG_GERAN_ERROR_2_G("GSN ""ME: Invalid SAPI %d or NSAPI %d",
        smsn_mes_ptr->sapi, smsn_mes_ptr->nsapi);
    return;
  }

  /* Translate SAPI & NSAPI values for internal use */
  nsapi     = ( gsn_nsapi_t) GSN_INTERN_NSAPI( smsn_mes_ptr->nsapi );
  new_sapi  = ( gsn_sapi_t ) gsn_intern_sapi[ smsn_mes_ptr->sapi ];

  /* Check if the proposed NSAPI is already active */
  if( actv_nsapis[ gas_id ][nsapi] == NULL )
  {
    /* Activate NSAPI and SAPI entities */
    gsn_activate_nsapi( gas_id, GSNSM_MODIFY_IND, (void*) smsn_mes_ptr );
    gsn_activate_sapi ( gas_id, new_sapi, smsn_mes_ptr->tlli );

    /* link NSAPI with SAPI */
    gsn_map_nsapi_on_sapi( gas_id, nsapi, new_sapi );

    /* Get active NSAPI entity's pointer and check for its validity */
    nsapi_e_ptr = actv_nsapis[ gas_id ][nsapi];
    GSN_ASSERT( nsapi_e_ptr != NULL );

    /* Resume flow on the new SAPI in case NSAPI flow was suspended due
    ** to high watermark on old SAPI.
    */
    gsn_process_lowater( gas_id, new_sapi, nsapi_e_ptr->op_mode, GSN_UL_SUSP_CTS_OFF );

    /* SNSM_MODIFY_RSP is pending */
    nsapi_e_ptr->modif_resp_pend = TRUE;

    /* see TS 04.65 par 5.1.2.23 */
    if(nsapi_e_ptr->op_mode == ACK)
    {
      nsapi_e_ptr->ule.ack_send_npdu = smsn_mes_ptr->snd_npdu_num;
      nsapi_e_ptr->dle.ack_rcv_npdu  = smsn_mes_ptr->rcv_npdu_num;
    }
    else
    {
      nsapi_e_ptr->ule.unack_send_npdu = 0;
      nsapi_e_ptr->dle.unack_rcv_npdu  = 0;
    }

    if( (nsapi_e_ptr->op_mode == ACK)   &&
        (actv_sapis[gas_id][new_sapi]->state == IDLE) )
    {
      /* Wait for LL_EST_IND before sending SMSN_MODIFY_RSP to SM.*/
      nsapi_e_ptr->modif_est_resp_pend = TRUE;

      /* Start SGSN initiated link est failure timer. */
      (void)rex_set_timer(
         &nsapi_e_ptr->sgsn_initiated_link_est_fail_timer,
         SGSN_INITIATED_LINK_EST_FAIL_TMR_VALUE );

      /* Suspend data flow until establishment is indicated */
      gsn_ul_suspend_dataflow( gas_id, new_sapi, GSN_UL_SUSP_EST_PEND );
    }
    else if ( (nsapi_e_ptr->op_mode == UNACK)  &&
              (actv_sapis[gas_id][new_sapi]->state == IDLE) )
    {
      /* Inform LLC */
      gsn_send_ll_activate_ind( gas_id, nsapi_e_ptr->sapi_ptr );
    }
  }
#ifdef FEATURE_DUAL_SIM
  /* Proceed further only if received AS ID is same as that stored in
  ** active NSAPI.
  */
  else if ( actv_nsapis[ gas_id ][nsapi]->gas_id != gas_id )
  {
    MSG_GERAN_ERROR_2_G("GSN ""ME: Invalid GAS_ID in Modify_Ind, Curr: %d, Rcvd: %d",
      actv_nsapis[ gas_id ][nsapi]->gas_id, gas_id);

    /* To avoid compiler warning */
    nsapi_e_ptr = actv_nsapis[ gas_id ][nsapi];
  }
#endif /* FEATURE_DUAL_SIM */
  else
  {
    /* NSAPI already exists. Get active NSAPI entity's pointer */
    nsapi_e_ptr = actv_nsapis[ gas_id ][nsapi];

    /* Store new QoS and Radio Priority parameters and Compression parameters */
    nsapi_e_ptr->qos        = smsn_mes_ptr->qos;
    nsapi_e_ptr->radio_pr   = smsn_mes_ptr->radio_priority;

    if((smsn_mes_ptr->dcomp_alg == SYS_PDP_DATA_COMP_ON) ||
       (smsn_mes_ptr->dcomp_alg == SYS_PDP_DATA_COMP_V42_BIS))
    {
      nsapi_e_ptr->data_comp = TRUE;
    }
    if((smsn_mes_ptr->pcomp_alg >= SYS_PDP_HEADER_COMP_ON) &&
       (smsn_mes_ptr->pcomp_alg <= SYS_PDP_HEADER_COMP_RFC3095))
    {
      nsapi_e_ptr->pci_comp = TRUE;
    }
    nsapi_e_ptr->pdp_type   = smsn_mes_ptr->pdp_type;

    nsapi_e_ptr->pfi        = smsn_mes_ptr->pfi;

    MSG_GERAN_MED_3_G("GSN ""Rel Class %d, PFI %d",
            smsn_mes_ptr->qos.reliability_class,
            smsn_mes_ptr->pfi,
            0);

    /* SNSM_MODIFY_RSP is pending */
    nsapi_e_ptr->modif_resp_pend = TRUE;

    /* store old SAPI number */
    old_sapi  = nsapi_e_ptr->sapi_ptr->sapi;

    /* Is NSAPI changing fron UnAck to Ack ? */
    if( (nsapi_e_ptr->op_mode == UNACK) &&
        ( (nsapi_e_ptr->qos.reliability_class == 0x1) ||
          (nsapi_e_ptr->qos.reliability_class == 0x2) ) )
    {
      nsapi_unack_to_ack = TRUE;
      nsapi_e_ptr->op_mode = ACK;

      /*  Set Send N-PDU number and Receive N-PDU number to zero
      **  See TS 04.64 5.1.23
      */
      nsapi_e_ptr->ule.ack_send_npdu = 0;
      nsapi_e_ptr->dle.ack_rcv_npdu  = 0;
    }
    /* Is NSAPI changing fron Ack to Unack ? */
    else if(  (nsapi_e_ptr->op_mode == ACK) &&
              ( (nsapi_e_ptr->qos.reliability_class != 0x1) &&
                (nsapi_e_ptr->qos.reliability_class != 0x2) ) )
    {
      nsapi_ack_to_unack = TRUE;
      nsapi_e_ptr->op_mode = UNACK;

      /*  Delete all buffered N-PDU and segments of an N-PDU ( SN-PDU ).
      **  Set Send N-PDU number Unack to zero. See TS 04.64 5.1.2.23
      */
      gsn_clr_nsapi_buf( gas_id, nsapi );
      nsapi_e_ptr->ule.unack_send_npdu = 0;
      nsapi_e_ptr->dle.unack_rcv_npdu  = 0;

      /* Clear Suspend data flow event due to MAX buffered N-PDUs.
      ** This suspend event is not valid for an UNACK NSAPI.
      */
      if( nsapi_e_ptr->ule.susp_flag & GSN_UL_SUSP_MAX_NPDU )
        nsapi_e_ptr->ule.susp_flag &= ~GSN_UL_SUSP_MAX_NPDU;
    }

    /* NSAPI is mapped to different SAPI */
    if( new_sapi != old_sapi )
    {
      /* NSAPI is mapped to different SAPI. If old SAPI establised,
      ** and not used by another NSAPI wait for LL_RELEASE_IND from peer,
      ** otherwise release old SAPI now.
      */
      if( (actv_sapis[gas_id][old_sapi]->state == ESTABLISHED) &&
          (!gsn_ack_nsapis_on_sapi( gas_id, nsapi, old_sapi )) )
      {
        nsapi_e_ptr->modif_rel_resp_pend = TRUE;

        /* NSAPI needs to be delinked from SAPI after LLC link is released */
        nsapi_e_ptr->delink_nsapi = TRUE;
      }
      else
        gsn_remove_nsapi_from_sapi( gas_id, nsapi, old_sapi);

      /* Activate new SAPI */
      gsn_activate_sapi ( gas_id, new_sapi, smsn_mes_ptr->tlli );

      /* link NSAPI with new SAPI */
      gsn_map_nsapi_on_sapi( gas_id, nsapi, new_sapi );

      /* Resume flow on the new SAPI in case NSAPI flow was suspended due
      ** to high watermark on old SAPI.
      */
      gsn_process_lowater( gas_id, new_sapi, nsapi_e_ptr->op_mode, GSN_UL_SUSP_CTS_OFF  );

      /*  If ACK operation is used both before and after the reception
      **  of SNSM_MODIFY.IND, NSAPI shall enter RECOVERY sate.
      */
      if( (nsapi_e_ptr->op_mode == ACK) &&
          (nsapi_unack_to_ack == FALSE) )
      {
        gsn_enter_recovery( gas_id, nsapi_e_ptr );

        /* Suspend data flow until the context
        ** modification procedure is completed
        */
        gsn_ul_suspend_dataflow( gas_id, new_sapi, GSN_UL_SUSP_EST_PEND );

        /* Prepare to re-transmit all buffered N-PDUs ( see 5.1.2.23 ) */
        gsn_ul_prepare_retr( gas_id, nsapi );
      }

      MSG_GERAN_HIGH_3_G("GSN "" nsapi_ack_to_unack = %d, sapi state = %d, ack nsapis on sapi = %d", nsapi_ack_to_unack,
                                                                                                     actv_sapis[gas_id][new_sapi]->state,
                                                                                                     gsn_ack_nsapis_on_sapi( gas_id, nsapi, new_sapi ) );

      /* Wait for LL_ESTABLISH_IND from peer if ACK NSAPI and
      ** SAPI is not established.
      */
      if( (nsapi_e_ptr->op_mode == ACK ) &&
          (actv_sapis[gas_id][new_sapi]->state == IDLE) )
      {
        nsapi_e_ptr->modif_est_resp_pend = TRUE;

        /* Start SGSN initiated link est failure timer. */
        (void)rex_set_timer(
           &nsapi_e_ptr->sgsn_initiated_link_est_fail_timer,
           SGSN_INITIATED_LINK_EST_FAIL_TMR_VALUE );

        /* Suspend data flow until establishment is indicated */
        gsn_ul_suspend_dataflow( gas_id, new_sapi, GSN_UL_SUSP_EST_PEND );
      }
      else if ( (nsapi_e_ptr->op_mode == UNACK) &&
                (actv_sapis[gas_id][new_sapi]->state == IDLE) )
      {

        /* Rel class changed from ACK to UNACK. Stop SGSN initiated
        ** link est failure timer.
        */
        (void)rex_clr_timer(
                &nsapi_e_ptr->sgsn_initiated_link_est_fail_timer );

         /* SAPI was not yet established so no need to wait for LL_RELEASE_IND
         ** from peer. Inform LLC that SAPI is now active for UNACK.
         */
         nsapi_e_ptr->modif_est_resp_pend = FALSE;

         nsapi_e_ptr->modif_rel_resp_pend = FALSE;

        gsn_send_ll_activate_ind( gas_id, nsapi_e_ptr->sapi_ptr );

         /* Modification procedure is completed, resume data flow. */
         GSN_CLR_SUSP_EVENT( gas_id, nsapi_e_ptr->nsapi, GSN_UL_SUSP_EST_PEND );
         GSN_CLR_SUSP_EVENT( gas_id, nsapi_e_ptr->nsapi, GSN_UL_SUSP_SEQ_PEND );
      }
    }
    else
    { /* No change of SAPI */

      MSG_GERAN_HIGH_3_G("GSN "" nsapi_ack_to_unack = %d, sapi state = %d, ack nsapis on sapi = %d", nsapi_ack_to_unack,
                                                                                                     actv_sapis[gas_id][new_sapi]->state,
                                                                                                     gsn_ack_nsapis_on_sapi( gas_id, nsapi, new_sapi ) );

      /* Check if NSAPI is linked to the SAPI */
      if( actv_nsapis[ gas_id ][nsapi]->sapi_ptr->sapi != new_sapi )
      {
        MSG_GERAN_ERROR_0_G("GSN ""ME: Rcvd Inval Modify Indication");
      }
      else
      {
        /* Use the new TLLI */
        actv_nsapis[ gas_id ][nsapi]->sapi_ptr->tlli = smsn_mes_ptr->tlli;

        if( nsapi_unack_to_ack )
        {
          /* NSAPI changed from Unack to Ack and uses the same SAPI.
          ** Wait for LL_ESTABLISH_IND from peer if SAPI is not established.
          */
          if( actv_sapis[gas_id][new_sapi]->state == IDLE )
          {
            nsapi_e_ptr->modif_est_resp_pend = TRUE;

            /* Start SGSN initiated link est failure timer. */
            (void)rex_set_timer(
               &nsapi_e_ptr->sgsn_initiated_link_est_fail_timer,
               SGSN_INITIATED_LINK_EST_FAIL_TMR_VALUE );

            /* Suspend data flow until establishment is indicated */
            gsn_ul_suspend_dataflow( gas_id, new_sapi, GSN_UL_SUSP_EST_PEND );
          }
        }
        else if( nsapi_ack_to_unack )
        {
          /*  NSAPI changed from Ack to Unack and uses the same SAPI.
          **  If SAPI is used by another NSAPI wait for LL_RELEASE_IND from peer.
          **  Also, if SAPI is used by another NSAPI but SAPI state is IDLE, then no
          **  need to wait for LL_RELEASE_IND. */

          if ( ( !gsn_ack_nsapis_on_sapi( gas_id, nsapi, new_sapi )          ) ||
               ( (  gsn_ack_nsapis_on_sapi( gas_id, nsapi, new_sapi  )) &&
                 (  actv_sapis[gas_id][new_sapi]->state == IDLE       )      )
             )
          {
            /* Check to ensure that SAPI was established prior to the
            ** ACK to UNACK modification.
            */
            if( actv_sapis[gas_id][new_sapi]->state == IDLE )
            {

              /* Rel class changed from ACK to UNACK. Stop SGSN initiated
              ** link est failure timer.
              */
              (void)rex_clr_timer(
                 &nsapi_e_ptr->sgsn_initiated_link_est_fail_timer );

              /* SAPI was not yet established so no need to wait for LL_RELEASE_IND
              ** from peer. Inform LLC that SAPI is now active for UNACK and send
              ** modification response now.
              */
              nsapi_e_ptr->modif_est_resp_pend = FALSE;

              nsapi_e_ptr->modif_rel_resp_pend = FALSE;

              gsn_send_ll_activate_ind( gas_id, nsapi_e_ptr->sapi_ptr );

              /* Modification procedure is completed, resume data flow. */
              GSN_CLR_SUSP_EVENT( gas_id, nsapi_e_ptr->nsapi, GSN_UL_SUSP_EST_PEND );
              GSN_CLR_SUSP_EVENT( gas_id, nsapi_e_ptr->nsapi, GSN_UL_SUSP_SEQ_PEND );
            }
            else
            {
              /* SAPI was already established so wait for LL_RELEASE_IND from
              ** peer.
              */
              nsapi_e_ptr->modif_rel_resp_pend = TRUE;
            }
          }
        }
      }
    }
  }

  /* If no response is pending, send SNSM_MODIFY_RSP to SM */
  if( (nsapi_e_ptr->modif_est_resp_pend == FALSE) &&
      (nsapi_e_ptr->modif_rel_resp_pend == FALSE) )
  {
    nsapi_e_ptr->modif_resp_pend = FALSE;

    gsn_send_snsm_mod_res( gas_id, nsapi_e_ptr->nsapi );
  }
}

/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_SNSM_SEQ_IND()
===
===  DESCRIPTION
===
===   Processes the SNSM_SEQUENCE indication primitive send by SM during
===   an inter-SGSN routing are update. It applies only to ACK NSAPIs.
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
static void gsn_process_snsm_seq_ind( gas_id_t gas_id, const gsnsm_seq_ind_type  *smsn_mes_ptr )
{
  gsn_nsapi_e_t       *nsapi_e_ptr;
  gsn_npdu_item_t     *item_ptr;
  gsn_nsapi_t         nsapi;
  gsn_seg_ref_item_t  *ref_item_ptr;


  /* Check if valid NSAPI */
  if( (!GSN_VALID_NSAPI(smsn_mes_ptr->nsapi) )   ||
      (actv_nsapis[ gas_id ][GSN_INTERN_NSAPI( smsn_mes_ptr->nsapi )] == NULL) )
  {
    MSG_GERAN_ERROR_1_G("GSN ""GSN: INVALID or INACTIVE  NSAPI %d",smsn_mes_ptr->nsapi);
  }
#ifdef FEATURE_DUAL_SIM
  /* Proceed further only if received AS ID is same as that stored in
  ** active NSAPI.
  */
  else if ( actv_nsapis[ gas_id ][GSN_INTERN_NSAPI( smsn_mes_ptr->nsapi )]->gas_id !=
              gas_id )
  {
    MSG_GERAN_ERROR_3_G("GSN ""Unexpected GAS_ID in Seq Ind, Curr: %d, Rcvd: %d",
      actv_nsapis[ gas_id ][GSN_INTERN_NSAPI( smsn_mes_ptr->nsapi )]->gas_id,
      gas_id,
      0);
  }
#endif /* FEATURE_DUAL_SIM */
  else
  {
    /* Get NSAPI number */
    nsapi = (gsn_nsapi_t) GSN_INTERN_NSAPI( smsn_mes_ptr->nsapi );

    nsapi_e_ptr = actv_nsapis[ gas_id ][nsapi];

    /* If NSAPI is in ACK mode, resume data transfer */
    if(actv_nsapis[ gas_id ][GSN_INTERN_NSAPI( smsn_mes_ptr->nsapi )]->op_mode == ACK)
    {
      /* Resume transmission of SN-PDUs and all buffered N-PDUs
      ** If suspended due to LL_RESET_IND.
      */
      GSN_CLR_SUSP_EVENT( gas_id, nsapi, GSN_UL_SUSP_SEQ_PEND );

      /* Suspend LLC task to prevent processing of LL_DATA_CNF
      ** that could modify the contents of the npdu_buff.
      */
      GSN_LOCK();

      /* If buffered N-PDU is confirmed by the Receive N-PDU Number,
      ** the N-PDU shall be deleted from the buffer.
      */
      item_ptr = q_check( &(nsapi_e_ptr->ule.npdu_buff) );
      while( item_ptr )
      {
        /* The receive N-PDU number from SGSN indicates the number
        ** of the next N-PDU expected. see TS 04.65 par 5.1.2.26 & 6.9.1
        */
        if( item_ptr->ack_send_npdu < smsn_mes_ptr->rcv_npdu_num )
        {
          if( item_ptr->seg_ref_buff_valid == TRUE )
          {
            ref_item_ptr = q_get( &(item_ptr->seg_ref_buff) );
            while( ref_item_ptr )
            {
              GSN_MEM_FREE( ref_item_ptr );   /* free memory */
              ref_item_ptr = q_get( &(item_ptr->seg_ref_buff) );
            }

            /* Set seg_ref_buff as invalid for this NPDU */
            item_ptr->seg_ref_buff_valid = FALSE;

            /* Free memory, mutex etc. used by NPDU segment queue attached to
            ** NPDU.
            */
            q_destroy ( &item_ptr->seg_ref_buff );
          }
          else
          {
            MSG_GERAN_MED_0_G("GSN ""No seg ref buff");
          }

          /* Delete N-PDU. It has been received by peer entity.
          ** Free memory back to DSM mem pool
          */
          GPRS_PDU_FREE( &item_ptr->npdu_ptr );

          /* Remove item from the queue */
          item_ptr = q_get( &(nsapi_e_ptr->ule.npdu_buff) );

          /* free memory */
          GSN_MEM_FREE( item_ptr );

          /* Check next item */
          item_ptr = q_check( &(nsapi_e_ptr->ule.npdu_buff) );
        }
        else
        {
          /* Get next item */
          item_ptr = q_next( &(nsapi_e_ptr->ule.npdu_buff), &(item_ptr->link)  );
        }

      } /* END while traversing the queue */

      /* Resume LLC task */
      GSN_UNLOCK();

      /* Remove any RFC 2507/3095 feedback NPDUs from retransmit buffer */
      gsn_ack_remove_fback_npdus_and_renumber( gas_id, nsapi_e_ptr );

      /* Prepare to retransmit the unconfirmed buffered N-PDUs,
      ** if no SAPI establishment is pending.
      */
      if( !(nsapi_e_ptr->ule.susp_flag & GSN_UL_SUSP_EST_PEND) )
      {
        gsn_ul_prepare_retr( gas_id, nsapi );
      }

      /* Send SNSM_SEQUENCE_RSP to SM   */
      gsn_send_snsm_seq_res( gas_id, nsapi_e_ptr->nsapi, (gsn_nsapi_mode_t) ACK);
    }
    else
    {
      /* No action is taken when NSAPI in UNACK mode. Just respond to SM */
      gsn_send_snsm_seq_res( gas_id, nsapi_e_ptr->nsapi, (gsn_nsapi_mode_t) UNACK);
    }
  }
}


/*===========================================================================
===
===  FUNCTION      GSN_SEND_SNSM_ACTV_RES()
===
===  DESCRIPTION
===
===   Sends the SNSM_ACTIVATE response primitive to inform SM that the
===   indicated NSAPI is now in use.
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
static void gsn_send_snsm_actv_res( gas_id_t gas_id, gsn_nsapi_t nsapi )
{
  sm_cmd_type  *snsm_dl_mes;

  /* Get buffer to convey the message */
  snsm_dl_mes = sm_get_cmd_buf();
  GSN_ASSERT( snsm_dl_mes != NULL );

  /* Load message */
  snsm_dl_mes->header.message_set           = MS_SNDCP_SM;
  snsm_dl_mes->header.cmd_id                = SNSM_ACTIVATE_RESP;
  snsm_dl_mes->cmd.sndcp_activate_resp.tlli = actv_nsapis[ gas_id ][nsapi]->sapi_ptr->tlli;
  snsm_dl_mes->cmd.sndcp_activate_resp.nsapi= (uint8) GSN_EXTERN_NSAPI(nsapi);
#ifdef FEATURE_DUAL_SIM
  snsm_dl_mes->cmd.sndcp_activate_resp.as_id= geran_map_gas_id_to_nas_id( gas_id );
#endif /* FEATURE_DUAL_SIM */

  /* Send SNSM_ACTIVATE_RSP to SM and log for call flow analysis. */
  MSG_GERAN_MED_1_G("GSN ""ME: Sent SNSM_ACTIVATE_RSP NSAPI%d",GSN_EXTERN_NSAPI(nsapi));

  cfa_log_packet_ex
  (
    GS_QUEUE_SM, MS_SNDCP_SM, (uint8) SNSM_ACTIVATE_RESP,
    sizeof( snsm_activate_rsp_T ),
    (uint8 *)&snsm_dl_mes -> cmd.sndcp_activate_resp
  );

  sm_put_cmd( snsm_dl_mes );
}

/*===========================================================================
===
===  FUNCTION      GSN_SEND_SNSM_DEACTV_RES()
===
===  DESCRIPTION
===
===   Sends the SNSM_DEACTIVATE response primitive to inform SM the the
===   NSAPI indicated is no longer in use.
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
static void gsn_send_snsm_deactv_res( gas_id_t gas_id, gsn_nsapi_t nsapi )
{
  sm_cmd_type  *snsm_dl_mes;

  /* Get buffer to convey the message */
  snsm_dl_mes = sm_get_cmd_buf();
  GSN_ASSERT( snsm_dl_mes != NULL );

  /* Load message */
  snsm_dl_mes->header.message_set             = MS_SNDCP_SM;
  snsm_dl_mes->header.cmd_id                  = SNSM_DEACTIVATE_RESP;
  snsm_dl_mes->cmd.sndcp_deactivate_resp.nsapi= (uint8) GSN_EXTERN_NSAPI(nsapi);

  /* In exceptional circumstances whereby SM has requested the deactivation
     of an already inactive NSAPI, a deactivation response message must still
     be sent. Check the active NSAPI pointer first before using it. */

  if ( actv_nsapis[ gas_id ][nsapi] != NULL )
  {
    snsm_dl_mes->cmd.sndcp_deactivate_resp.tlli  =
      actv_nsapis[ gas_id ][nsapi]->sapi_ptr->tlli;
#ifdef FEATURE_DUAL_SIM
    snsm_dl_mes->cmd.sndcp_deactivate_resp.as_id = geran_map_gas_id_to_nas_id( gas_id );
#endif /* FEATURE_DUAL_SIM */
  }
  else
    snsm_dl_mes->cmd.sndcp_deactivate_resp.tlli = 0;

  /* Send SNSM_DEACTIVATE_RESP to SM and log for call flow analysis. */
  MSG_GERAN_MED_1_G("GSN ""ME: Sent SNSM_DEACTIV_RESP NSAPI%d",GSN_EXTERN_NSAPI(nsapi));

  cfa_log_packet_ex
  (
    GS_QUEUE_SM, MS_SNDCP_SM, (uint8) SNSM_DEACTIVATE_RESP,
    sizeof( snsm_deactivate_rsp_T ),
    (uint8 *)&snsm_dl_mes -> cmd.sndcp_deactivate_resp
  );

  sm_put_cmd( snsm_dl_mes );
}

/*===========================================================================
===
===  FUNCTION      GSN_SEND_SNSM_MOD_RES()
===
===  DESCRIPTION
===
===   Sends the SNSM_MODIFY response primitive to inform SM that the indicated
===   NSAPI and QoS profile are now in use.
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
static void gsn_send_snsm_mod_res( gas_id_t gas_id, gsn_nsapi_t nsapi )
{
  sm_cmd_type  *snsm_dl_mes;

  /* Get buffer to convey the message */
  snsm_dl_mes = sm_get_cmd_buf();
  GSN_ASSERT( snsm_dl_mes != NULL );

  /* Load message */
  snsm_dl_mes->header.message_set           = MS_SNDCP_SM;
  snsm_dl_mes->header.cmd_id                = SNSM_MODIFY_RESP;
  snsm_dl_mes->cmd.sndcp_modify_resp.tlli   = actv_nsapis[ gas_id ][nsapi]->sapi_ptr->tlli;
  snsm_dl_mes->cmd.sndcp_modify_resp.nsapi  = (uint8) GSN_EXTERN_NSAPI(nsapi);
#ifdef FEATURE_DUAL_SIM
  snsm_dl_mes->cmd.sndcp_modify_resp.as_id  = geran_map_gas_id_to_nas_id( gas_id );
#endif /* FEATURE_DUAL_SIM */

  /* Send SNSM_MODIFY_RSP to SM and log for call flow analysis. */
  MSG_GERAN_MED_1_G("GSN ""ME: Sent SNSM_MODIFY_RESP NSAPI%d",GSN_EXTERN_NSAPI(nsapi));

  cfa_log_packet_ex
  (
    GS_QUEUE_SM, MS_SNDCP_SM, (uint8) SNSM_MODIFY_RESP,
    sizeof( snsm_modify_rsp_T ),
    (uint8 *)&snsm_dl_mes -> cmd.sndcp_modify_resp
  );

  sm_put_cmd( snsm_dl_mes );
}

/*===========================================================================
===
===  FUNCTION      GSN_SEND_SNSM_SEQ_RES()
===
===  DESCRIPTION
===
===   Sends the SNSM_SEQUENCE response primitive to SM. The primitive is
===   used to return the Receive N-PDU number to the SGSN during an ongoing
===   inter-SGSN routing area update.
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
static void gsn_send_snsm_seq_res( gas_id_t gas_id, gsn_nsapi_t nsapi, gsn_nsapi_mode_t nsapi_mode )
{
  sm_cmd_type  *snsm_dl_mes;

  /* Get buffer to convey the message */
  snsm_dl_mes = sm_get_cmd_buf();
  GSN_ASSERT( snsm_dl_mes != NULL );

  /* Load message */
  snsm_dl_mes->header.message_set           = MS_SNDCP_SM;
  snsm_dl_mes->header.cmd_id                = SNSM_SEQUENCE_RESP;
  snsm_dl_mes->cmd.sndcp_sequence_resp.tlli = actv_nsapis[ gas_id ][nsapi]->sapi_ptr->tlli;
  snsm_dl_mes->cmd.sndcp_sequence_resp.nsapi= (uint8) GSN_EXTERN_NSAPI(nsapi);
#ifdef FEATURE_DUAL_SIM
  snsm_dl_mes->cmd.sndcp_sequence_resp.as_id= geran_map_gas_id_to_nas_id( gas_id );
#endif /* FEATURE_DUAL_SIM */

  if( nsapi_mode == ACK )
  {
    snsm_dl_mes->cmd.sndcp_sequence_resp.rec_n_pdu = actv_nsapis[ gas_id ][nsapi]->dle.ack_rcv_npdu;
  }
  else
  {
    snsm_dl_mes->cmd.sndcp_sequence_resp.rec_n_pdu = actv_nsapis[ gas_id ][nsapi]->dle.unack_rcv_npdu;
  }

  /* Send SNSM_SEQUENCE_RESP to SM and log for call flow analysis. */
  MSG_GERAN_MED_1_G("GSN ""ME: Sent SNSM_SEQUENCE_RESP NSAPI%d",GSN_EXTERN_NSAPI(nsapi));

  cfa_log_packet_ex
  (
    GS_QUEUE_SM, MS_SNDCP_SM, (uint8) SNSM_SEQUENCE_RESP,
    sizeof( snsm_sequence_rsp_T ),
    (uint8 *)&snsm_dl_mes -> cmd.sndcp_sequence_resp
  );

  sm_put_cmd( snsm_dl_mes );
}



/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_UL_ACK_NPDU()
===
===  DESCRIPTION
===
===   Segments PDP UL N-PDU, if needed, forms SN-PDU(s) and
===   transmits LL-PDU(s) to ACK LLC.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===   None
===
===========================================================================*/

static void  gsn_process_ul_ack_npdu
(
 gas_id_t         gas_id,
 gsn_nsapi_t      nsapi,        /* Nsapi number */
 dsm_item_type    **npdu,       /* N-PDU to be processed */
 gsn_npdu_item_t  *item_ptr,    /* Points to the bufferd item */
 boolean          re_tx,        /* TRUE if N-PDU is retransmitted */
 uint8            pcomp,        /* PCOMP value */
 uint8                dcomp,        /* DCOMP value */
 geran_pdu_priority_t pdu_priority

)
{
  uint8                 seg_num, hdr_len;
  gsn_nsapi_e_t         *nsapi_e_ptr = actv_nsapis[ gas_id ][nsapi];
  gsn_data_pdu_hdr_t    sn_hdr;
  gprs_ll_ul_dat_mes_t  *ll_mes;
  ll_data_req_t         *data_mes;
  uint16                n201_i, len;
  uint8                 ext_hdr[ACK_PDU_HDR_1ST_SEG];
  gsn_seg_ref_item_t    *seg_ptr;

  /* Set N-PDU Number field in the SN-PDU header  */
  sn_hdr.num = item_ptr->ack_send_npdu;

  /* Set PCOMP & DCOMP values on SN-DATA header */
  sn_hdr.pcomp = pcomp;
  sn_hdr.dcomp = dcomp;

  /* initialise segment number */
  seg_num = 1;

  /* Get length of N-PDU */
  len = GPRS_PDU_LENGTH( *npdu );

  /* Update Diag statistics */
  if( re_tx )
  {
    /* N-PDU is retransmitted, update the relevant counters */
    nsapi_e_ptr->diag_pdu_stat_rsp.npdu_retx_cnt++;
    nsapi_e_ptr->diag_pdu_stat_rsp.npdu_octet_retx_cnt += len;
  }
  else
  {
    nsapi_e_ptr->diag_pdu_stat_rsp.npdu_tx_cnt++;
    nsapi_e_ptr->diag_pdu_stat_rsp.npdu_octet_tx_cnt += len;
  }

  /* Continue segmenting while some bytes in N-PDU */
  while( len )
  {
    /*  Get (N201-i minus SN-PDU Ack header) number of bytes
    **  from the head of the N-PDU to form the SN-PDU
    */
    n201_i  = gsn_n201_i[gas_id][nsapi_e_ptr->sapi_ptr->sapi];

    /* Determine the number of bytes in the SN-PDU header */
    if( seg_num == SN_FIRST_SEG )
      hdr_len = ACK_PDU_HDR_1ST_SEG;
    else
      hdr_len = ACK_PDU_HDR_NOT_1ST_SEG;

    /* Allocate memory to convey the message to LLC */
    ll_mes = GPRS_MEM_MALLOC( sizeof( gprs_ll_ul_dat_mes_t ) );
    GSN_ASSERT( ll_mes != NULL );

    /* Avoid long lines of code */
    data_mes = &ll_mes->mes.data_req_mes;

    /* Segment N-PDU. Length of segment must be less than N201_I
    ** after the LL_DATA header has been added
    */
    data_mes->l3_pdu = gprs_pdu_segment_head( npdu, (uint16) (n201_i - hdr_len) );

    if( data_mes->l3_pdu == NULL )
    {
      /* Free dsm item conveying the N-PDU */
      if( *npdu != NULL )
      {
        GPRS_PDU_FREE( npdu );
      }

      /* Free mem allocated to convay LL-PDU to LLC */
      GSN_MEM_FREE( ll_mes );

      /* gprs_pdu_duplicate() failed. Due to peer N-PDU sequence requirements,
      ** we can't just drop the NPDU and continue. The cleaner way to avoid
      ** ERROR_FATAL is to force an NSAPI deactivation by
      ** informing SM using SNSM_STATUS_REQ with cause "DM received".
      */
      gsn_send_snsm_stat_req( gas_id,
                              nsapi_e_ptr->sapi_ptr->sapi,
                              nsapi_e_ptr->sapi_ptr->tlli,
                              (uint8) SNSM_DM_RECEIVED );

      MSG_GERAN_ERROR_1_G("GSN ""NS %d: gprs_pdu_segment_head() failed. Force NSAPI deactivation",
          GSN_EXTERN_NSAPI(nsapi));

      return;
    }

    /* Update Diag statistics */
    if( re_tx )
    {
      /* SN-PDU is retransmitted, update the relevant counters */
      nsapi_e_ptr->diag_pdu_stat_rsp.snpdu_retx_cnt++;
      if( len > (n201_i - hdr_len ) )
        nsapi_e_ptr->diag_pdu_stat_rsp.snpdu_octet_retx_cnt += n201_i;
      else
        nsapi_e_ptr->diag_pdu_stat_rsp.snpdu_octet_retx_cnt += len + hdr_len;
    }
    else
    {
      nsapi_e_ptr->diag_pdu_stat_rsp.snpdu_tx_cnt++;
      if( len > (n201_i - hdr_len ) )
        nsapi_e_ptr->diag_pdu_stat_rsp.snpdu_octet_tx_cnt += n201_i;
      else
        nsapi_e_ptr->diag_pdu_stat_rsp.snpdu_octet_tx_cnt += len + hdr_len;
    }

    /* Calculate length of the N-PDU left after segmentation */
    len = (len > (n201_i - hdr_len)) ? (len - (n201_i - hdr_len)) : 0;

    if( (seg_num == SN_FIRST_SEG) && (len) )
    {
      /* First segment but more to follow */
      sn_hdr.f = SN_FIRST_SEG;
      sn_hdr.m = SN_MORE_SEG;
    }
    else if( (seg_num == SN_FIRST_SEG) && (!len) )
    {
      /* only one segment */
      sn_hdr.f = SN_FIRST_SEG;
      sn_hdr.m = SN_LAST_SEG;
    }
    else if( (seg_num != SN_FIRST_SEG) && (len) )
    {
      /* Not the First segment but more to follow */
      sn_hdr.f = SN_NOT_FIRST_SEG;
      sn_hdr.m = SN_MORE_SEG;
    }
    else if( (seg_num != SN_FIRST_SEG) && (!len)  )
    {
      /* Last segment */
      sn_hdr.f = SN_NOT_FIRST_SEG;
      sn_hdr.m = SN_LAST_SEG;
    }

    /* Set the rest of the fields in the SN-DATA PDU header */
    sn_hdr.x = 0; /* Shall be set to zero */

    /* SN-DATA PDU */
    sn_hdr.t = 0;

    /* Translate SNDCP NSAPI number to external NSAPI number */
    sn_hdr.nsapi = (uint8) GSN_EXTERN_NSAPI( nsapi );

    /* Encode SN-DATA PDU header to a byte stream */
    gsn_encode_data_pdu_hdr( &sn_hdr, ext_hdr);

    /* Append header to the N-PDU or segment of it. */
    if( !gprs_pdu_append_head( &data_mes->l3_pdu, ext_hdr, (uint16)hdr_len) )
    {
      /* Free dsm item conveying the N-PDU */
      if( *npdu != NULL )
      {
        GPRS_PDU_FREE( npdu );
      }

      /* Free mem allocated to convay LL-PDU to LLC */
      GSN_MEM_FREE( ll_mes );

      /* gprs_pdu_append_head() failed. Due to peer N-PDU sequence requirements,
      ** we can't just drop the NPDU and continue. The cleaner way to avoid
      ** ERROR_FATAL is to force an NSAPI deactivation by
      ** informing SM using SNSM_STATUS_REQ with cause "DM received".
      */
      gsn_send_snsm_stat_req( gas_id,
                              nsapi_e_ptr->sapi_ptr->sapi,
                              nsapi_e_ptr->sapi_ptr->tlli,
                              (uint8) SNSM_DM_RECEIVED );

      MSG_GERAN_ERROR_1_G("GSN ""NS %d: gprs_pdu_append_head() failed. Force NSAPI deactivation",
          GSN_EXTERN_NSAPI(nsapi));

      return;
    }

    /* Set reference number for the segment */
    data_mes->sndcp_ref.nsapi    = (uint8) GSN_EXTERN_NSAPI(nsapi);
    data_mes->sndcp_ref.npdu_num = sn_hdr.num;
    data_mes->sndcp_ref.seg_num  = seg_num;

    /* Add segment reference to the segment reference buffer for Ack */
    seg_ptr = GPRS_MEM_MALLOC( sizeof(gsn_seg_ref_item_t) );
    GSN_ASSERT( seg_ptr != NULL );
    seg_ptr->npdu_num = sn_hdr.num;
    seg_ptr->seg_num  = seg_num;
    (void) q_link( seg_ptr, &(seg_ptr->link) );
    q_put( &item_ptr->seg_ref_buff, &(seg_ptr->link) );



    /*  Load LL_DATA_REQ primitive.
    **  Reference number and l3_pdu are already assined
    */
    ll_mes->sapi               = gsn_extern_sapi[nsapi_e_ptr->sapi_ptr->sapi];
    data_mes->prim             = LL_DATA_REQ;
    data_mes->peak_throughput  = nsapi_e_ptr->qos.peak_throughput;
    data_mes->radio_priority   = nsapi_e_ptr->radio_pr;
    data_mes->tlli             = nsapi_e_ptr->sapi_ptr->tlli;
    data_mes->pfi              = nsapi_e_ptr->pfi;
    MSG_GERAN_MED_1_G("GSN ""PFI %d",
                 nsapi_e_ptr->pfi);

    data_mes->   gas_id        = gas_id;

    data_mes->   pdu_priority  = pdu_priority;

    if( sn_hdr.m == SN_LAST_SEG )
    {
      data_mes->final_seqment = TRUE;
    }
    else
    {
      data_mes->final_seqment = FALSE;
    }

    switch( nsapi_e_ptr->sapi_ptr->sapi )
    {
    case GSN_SAPI_3:
      GSN_MSG_MED
      (
        "LL 3: Sent LL_DATA_REQ %d:%d:%d",
        sn_hdr.nsapi,
        ll_mes->mes.data_req_mes.sndcp_ref.npdu_num,
        ll_mes->mes.data_req_mes.sndcp_ref.seg_num
      );
      break;

    case GSN_SAPI_5:
      GSN_MSG_MED
      (
        "LL 5: Sent LL_DATA_REQ %d:%d:%d",
        sn_hdr.nsapi,
        ll_mes->mes.data_req_mes.sndcp_ref.npdu_num,
        ll_mes->mes.data_req_mes.sndcp_ref.seg_num
      );
      break;

    case GSN_SAPI_9:
      GSN_MSG_MED
      (
        "LL 9: Sent LL_DATA_REQ %d:%d:%d",
        sn_hdr.nsapi,
        ll_mes->mes.data_req_mes.sndcp_ref.npdu_num,
        ll_mes->mes.data_req_mes.sndcp_ref.seg_num
      );
      break;

    case GSN_SAPI_11:
      GSN_MSG_MED
      (
        "LL 11: Sent LL_DATA_REQ %d:%d:%d",
        sn_hdr.nsapi,
        ll_mes->mes.data_req_mes.sndcp_ref.npdu_num,
        ll_mes->mes.data_req_mes.sndcp_ref.seg_num
      );
      break;

    default:
      break;
    }

    /* Send primitive to LLC */
    gllc_ll_ul_dat_put_mes_buf( gas_id, ll_mes );

    /* Increment segment number. No indication in TS 04.65
    ** of max number of segments for Ack mode.
    */
    seg_num += 1;

  } /* while () */
}

/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_PDP_DL_DATA()
===
===  DESCRIPTION
===
===   This function is called after completion of the N-PDU reassembly
===   process on the down link. The reassembled N-PDU is decompressed,
===   if needed, and forwarded to the PDP entity.
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/
static void gsn_process_pdp_dl_data( gas_id_t gas_id, gsn_nsapi_t nsapi, dsm_item_type **npdu  )
{
  dsm_item_type *temp_npdu = NULL;

  if( (actv_nsapis[ gas_id ][nsapi] != NULL)  &&
      /* Check if SNDCP Rx Function Ptr is NULL */
      (actv_nsapis[ gas_id ][nsapi]->from_gsndcp_post_rx_func_ptr != NULL) &&
      (actv_nsapis[ gas_id ][nsapi]->from_gsndcp_wm != NULL))
  {

    /* N-PDU data are decompressed first followed by decompression of PCI
    */
#ifdef  FEATURE_GSM_GPRS_SNDCP_DCOMP

    /* Check if packet is compressed. If so, decompress packet */
    if( actv_nsapis[ gas_id ][nsapi]->dle.first_seg_info.dcomp != 0 )
      gsn_data_decompress( gas_id, actv_nsapis[ gas_id ][nsapi], npdu );

#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */

#ifdef  FEATURE_GSM_GPRS_SNDCP_PCOMP

    /* Check if packet is compressed. If so, decompress packet */
    if( actv_nsapis[ gas_id ][nsapi]->dle.first_seg_info.pcomp != 0 )
    {
      if( !gsn_pci_decompress( gas_id, actv_nsapis[ gas_id ][nsapi], npdu ) )
      {
        /* Free mem conveying the N-PDU */
        if( *npdu != NULL )
          GPRS_PDU_FREE( npdu );

        MSG_GERAN_HIGH_1_G("GSN ""NS %d: DeComp failed. DL N-PDU is dropped",
          GSN_EXTERN_NSAPI(nsapi));
      }
    }

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

    if( *npdu != NULL )
    {
      sndcp_log_dl_tcp_hdr( *npdu );
      /* Update Diag statistics */
      actv_nsapis[ gas_id ][nsapi]->diag_pdu_stat_rsp.npdu_octet_rx_cnt +=
                                           GPRS_PDU_LENGTH( *npdu );
      actv_nsapis[ gas_id ][nsapi]->diag_pdu_stat_rsp.npdu_rx_cnt++;

      /* Delink re-assembly buffer */
      temp_npdu  = *npdu;
      *npdu      = NULL;

      /* Send N-PDU to PDP */
      /* Enqueue the packet on the DL Watermark */
      dsm_enqueue(actv_nsapis[ gas_id ][nsapi]->from_gsndcp_wm, &temp_npdu);

      /* Call the registered SNDCP Rx function ptr */
      actv_nsapis[ gas_id ][nsapi]->from_gsndcp_post_rx_func_ptr (0,0,actv_nsapis[ gas_id ][nsapi]->ds_context);
    }
#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif
  }
  else
  {
      MSG_GERAN_ERROR_3_G("GSN ""NS %d: Inact NSAPI=%p or Inval WM ptr=%p, drop DL NPDU",
                   GSN_EXTERN_NSAPI(nsapi),
                   actv_nsapis[ gas_id ][nsapi],
                   actv_nsapis[ gas_id ][nsapi]->from_gsndcp_wm);
    /* Free mem conveying the N-PDU */
    if( *npdu != NULL )
      GPRS_PDU_FREE( npdu );
  }
}

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GSN_SEND_LL_EST_REQ()
===
===  DESCRIPTION
===
===   Sends LL_ESTABLISH request to LLC layer to establish or -re-establish
===   ACK peer-to-peer operation for the SAPI in the LLC layer.
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
void gsn_send_ll_est_req( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr, dsm_item_type  *xid_req )
{
  gsn_nsapi_q_t         *nsapi_q_ptr;
  gsn_sapi_t            sapi = sapi_ptr->sapi;
  gprs_ll_ul_sig_mes_t  *ul_mes;

  /* Switch on SAPI's state */
  switch( sapi_ptr->state )
  {

  /*-------------------------------------------------------------------------------*/
  case ( IDLE ):
  case( ESTABLISHED ):
    {

      /* Print XID parameters in F3 message */
      gsn_print_xid_params ( gas_id, xid_req, TRUE );

      if( sapi_ptr->state == ESTABLISHED )
      {
        /* Re-establishment initiated by SNDCP */
        sapi_ptr->re_establish = TRUE;
      }

      /* Get message buffer */
      ul_mes = gllc_ll_ul_sig_get_mes_buf();
      GSN_ASSERT( ul_mes != NULL );

      /* load LLC message */
      ul_mes->sapi                       = gsn_extern_sapi[sapi];
      ul_mes->mes.est_req_mes.prim       = LL_ESTABLISH_REQ;
      ul_mes->mes.est_req_mes.tlli       = sapi_ptr->tlli;
      ul_mes->mes.est_req_mes.l3_xid_req = xid_req;
      ul_mes->mes.est_req_mes.gas_id     = gas_id;

      if( sapi_ptr->is_nsapi_ptr_q_valid )
      {

        nsapi_q_ptr = q_check( &sapi_ptr->nsapi_ptr_q );

        if ( nsapi_q_ptr != NULL )
        {
          ul_mes->mes.est_req_mes.peak_throughput =
            nsapi_q_ptr->nsapi_ptr->qos.peak_throughput;
          ul_mes->mes.est_req_mes.radio_priority  =
            nsapi_q_ptr->nsapi_ptr->radio_pr;
          ul_mes->mes.est_req_mes.pfi  =
            nsapi_q_ptr->nsapi_ptr->pfi;
        }
        else
        {
          ul_mes->mes.est_req_mes.peak_throughput = 0;
          ul_mes->mes.est_req_mes.radio_priority  = 1;
          ul_mes->mes.est_req_mes.pfi             = 0;
        }
      }
      else
      {
        MSG_GERAN_ERROR_2_G("GSN ""LL %d: NSAPI ptr queue invalid in state: %d",
          gsn_extern_sapi[sapi],sapi_ptr->state);
      }

      if( ul_mes->mes.est_req_mes.l3_xid_req != NULL )
        /* Set SAPI XID state */
        sapi_ptr->xid_state = XID_EST_CNF_PEND;

      /* Change SAPI state */
      sapi_ptr->state = EST_PEND;

      /* Suspend data flow until establishment is confirmed */
      gsn_ul_suspend_dataflow( gas_id, sapi, GSN_UL_SUSP_EST_PEND );

      /* Post message to LLC and log for call flow analysis. */
      MSG_GERAN_MED_1_G("GSN ""LL %d: Sent LL_ESTABLISH_REQ",gsn_extern_sapi[sapi]);

      cfa_log_packet_ex
      (
        GS_QUEUE_LLC, MS_LLC_SNDCP, (uint8) LL_ESTABLISH_REQ,
        sizeof( ll_establish_req_t ), (uint8 *)&ul_mes -> mes.est_req_mes
      );

      /* Record that the MS SNDCP initiated the establishment procedure */
      sapi_ptr->link_estab_originator = TRUE;

      gllc_ll_ul_sig_put_mes_buf( gas_id, ul_mes );
    }
    break;

/*-------------------------------------------------------------------------------*/
  default:

    MSG_GERAN_ERROR_2_G("GSN ""LL %d: Rcvd LL_EST_REQ in state: %d",
      gsn_extern_sapi[sapi],sapi_ptr->state);
    break;

/*-------------------------------------------------------------------------------*/
  } /* switch switch( sapi_ptr->state ) */
}


/*===========================================================================
===
===  FUNCTION      GSN_UNACK_RETR_NPDU_HDLR()
===
===  DESCRIPTION
===
===   When in unack mode, retransmits NPDUs bufferd in npdu_buff of a NSAPI
===   one at the time starting with oldest NPDU first.
===
===  DEPENDENCIES
===
===   None
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
void gsn_unack_retr_npdu_hdlr( gas_id_t gas_id, gsn_nsapi_t nsapi )
{
  gsn_nsapi_e_t          *nsapi_e_ptr;
  gsn_npdu_item_t        *npdu_item_ptr;
  uint8                  i;
  uint8                  cnt;
  geran_pdu_priority_t   pdu_priority = GPDU_LOW_PRIORITY;

  /* Get active NSAPI entity's pointer and check for its validity */
  nsapi_e_ptr = actv_nsapis[ gas_id ][nsapi];
  GSN_ASSERT( nsapi_e_ptr != NULL );

  if( nsapi_e_ptr->op_mode != UNACK )
  {
    MSG_GERAN_ERROR_1_G("GSN "" ReTr in Ack NSAPI %d",GSN_EXTERN_NSAPI(nsapi));
  }
  else
  {
    /* Check if there are any N-PDUs in the buffer */
    npdu_item_ptr = q_check( &( nsapi_e_ptr->ule.npdu_buff) );
    cnt           = (uint8) q_cnt( &( nsapi_e_ptr->ule.npdu_buff) );

    if( npdu_item_ptr )
    {
      for( i = 0; i < cnt; i++ )
      {

        /* Retransmit N-PDU. */
        gsn_process_sn_udata_req ( gas_id, nsapi, &npdu_item_ptr->npdu_ptr, FALSE, npdu_item_ptr, pdu_priority );

        /* get next N-PDU */
        npdu_item_ptr = q_next( &( nsapi_e_ptr->ule.npdu_buff),
                                    &(npdu_item_ptr->link) );

      } /* for( i = 0; i < cnt; i++ ) */
    }
    else
    {
      MSG_GERAN_ERROR_1_G("GSN ""NSAPI %d: No N-PDUs to re-transmit",GSN_EXTERN_NSAPI(nsapi));
    }
  }
}


/*===========================================================================
===
===  FUNCTION      GSN_RETR_NPDU_HDLR()
===
===  DESCRIPTION
===
===   Retransmits uncorfirmed N-PDUs, one at the time starting with oldest first,
===   bufferd in npdu_buff of the NSAPI.
===
===  DEPENDENCIES
===
===   None
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
void gsn_retr_npdu_hdlr( gas_id_t gas_id, gsn_nsapi_t nsapi )
{
  uint8               i, cnt;
  gsn_nsapi_e_t       *nsapi_e_ptr;
  gsn_npdu_item_t     *npdu_item_ptr;
  dsm_item_type       *dup_npdu_ptr = NULL;
  boolean             susp_flag     = FALSE;
  uint8               pcomp         = 0;
  uint8               dcomp         = 0;
  geran_pdu_priority_t pdu_priority  = GPDU_HIGH_PRIORITY;

  /* Get active NSAPI entity's pointer and check for its validity */
  nsapi_e_ptr = actv_nsapis[ gas_id ][nsapi];

  /* Check validity. If NULL, NSAPI does not exist and cannot proceed. */
  if( nsapi_e_ptr == NULL )
  {
    MSG_GERAN_ERROR_1_G("GSN " " NSAPI is invalid %d", GSN_EXTERN_NSAPI(nsapi));
  }
  else if( nsapi_e_ptr->op_mode != ACK )
  {
    MSG_GERAN_ERROR_1_G("GSN "" ReTr in Unack NSAPI %d",GSN_EXTERN_NSAPI(nsapi));
  }
  else
  {
    /* Check if there are any N-PDUs in the buffer */
    npdu_item_ptr = q_check( &( nsapi_e_ptr->ule.npdu_buff) );
    cnt           = (uint8) q_cnt( &( nsapi_e_ptr->ule.npdu_buff) );

      for( i = 0; i < cnt; i++ )
    {
        if( npdu_item_ptr != NULL )
      {
        /* Re-transmit N-PDUs if UL data flow is not suspended */
        if( (nsapi_e_ptr->ule.susp_flag & GSN_UL_SUSP_MAX_NPDU_MASK) )
        {
          susp_flag = TRUE;

          /* Set re-tx event flag to continue re-txing after data flow is resumed */
          nsapi_e_ptr->ule.retr = TRUE;

          /* UL suspended, break out from for loop */
          break;
        }

        /* check if N-PDU has been re-transmitted. If not, retransmit N-PDU */
        if( npdu_item_ptr->re_tr_flag == FALSE )
        {
          /* set flag */
          npdu_item_ptr->re_tr_flag = TRUE;

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )

          /* Copy the dsm item if PCI compression is required otherwise duplicate
          ** the dsm item. This is needed because the PCI compression algorithm
          ** modifies the source PDU chain, thereby potentially corrupting duplicate
          ** DSM references held by other entities.
          */
          if( nsapi_e_ptr->pci_comp )
          {
            dup_npdu_ptr = gprs_pdu_copy( npdu_item_ptr->npdu_ptr);
          }
          else
          {
            dup_npdu_ptr = gprs_pdu_duplicate( npdu_item_ptr->npdu_ptr, 0,
                            GPRS_PDU_LENGTH(npdu_item_ptr->npdu_ptr) );
          }

          if( dup_npdu_ptr == NULL )
          {
            /* Run out of  DSM resources. Do not compress the N-PDU */
            MSG_GERAN_HIGH_2_G("GSN ""NS %d: Run out of DSM resources. No comp for N-PDU %d ",
                GSN_EXTERN_NSAPI(nsapi), npdu_item_ptr->ack_send_npdu);
          }
          else
          {
            /* Compress N-PDU. Protocol compression first and then data compression
            */
#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP

            /* Compress PCI of the N-PDU */
            pcomp = gsn_pci_compress( gas_id, nsapi_e_ptr, &dup_npdu_ptr );

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP

            if( dup_npdu_ptr != NULL )
            {
              /* Compress data of the N-PDU */
              dcomp = gsn_data_compress( gas_id, nsapi_e_ptr, &dup_npdu_ptr );
            }
#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */

            /* Check if the dsm_item conveying the copy of the N-PDU
            ** has been released by the compression algorithm due to
            ** invalid TCP/UDP/IP packet structure. If so, abandon
            ** compression for the N-PDU.
            */
            if( dup_npdu_ptr == NULL )
            {
              pcomp = 0;
              dcomp = 0;

              MSG_GERAN_HIGH_2_G("GSN ""NS %d: comp failed. No comp for N-PDU %d",
                GSN_EXTERN_NSAPI(nsapi), npdu_item_ptr->ack_send_npdu);
            }
          }
#endif  /* #if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_ ... */

          if( dup_npdu_ptr == NULL )
          {
            /* Either no compression is enabled or compression failed.
            ** For both cases, duplicate the dsm item for re-transmission purposes.
            */
            dup_npdu_ptr = gprs_pdu_duplicate( npdu_item_ptr->npdu_ptr, 0,
                                  GPRS_PDU_LENGTH(npdu_item_ptr->npdu_ptr) );

            if( dup_npdu_ptr == NULL )
            {
              /* gprs_pdu_duplicate() failed. Due to peer N-PDU sequence requirements,
              ** we can't just drop the NPDU and continue. The cleaner way to avoid
              ** ERROR_FATAL is to force an NSAPI deactivation by
              ** informing SM using SNSM_STATUS_REQ with cause "DM received".
              */
              gsn_send_snsm_stat_req( gas_id,
                                      nsapi_e_ptr->sapi_ptr->sapi,
                                      nsapi_e_ptr->sapi_ptr->tlli,
                                      (uint8) SNSM_DM_RECEIVED );

              MSG_GERAN_ERROR_1_G("GSN ""NS %d: gprs_pdu_duplicate() failed. Force NSAPI deactivation",
                  GSN_EXTERN_NSAPI(nsapi));

              break;
            }
          }

          /* Creation of a new segment buffer is required. Ensure it is
          ** initialised.
          */
          memset(&npdu_item_ptr->seg_ref_buff, 0, sizeof(q_type));

          /* Iniatialise queue that holds NPDU segments */
          (void) q_init( &npdu_item_ptr->seg_ref_buff );

          /* Set flag to indicate seg_ref_buff queue is valid */
          npdu_item_ptr->seg_ref_buff_valid = TRUE;

          /* Segment N-PDU and send LL-PDUs to LLC */
          gsn_process_ul_ack_npdu( gas_id,
                                   nsapi,
                                   &dup_npdu_ptr,
                                   npdu_item_ptr,
                                   TRUE,
                                   pcomp,
                                   dcomp,
                                   pdu_priority );
        }

        /* get next N-PDU */
        npdu_item_ptr = q_next( &( nsapi_e_ptr->ule.npdu_buff),
                                   &(npdu_item_ptr->link) );

        /* If npdu_item_ptr is NULL before value of "i" reached "cnt - 1", then
        ** there is an actual pointer NULL in the buffer. In this case break
        ** out of "for" loop. This fix avoids printing out following error
        ** message each time this "for" loop reaches end of npdu_buff.
        */

        if((npdu_item_ptr == NULL) && ( i != (cnt - 1)))
        {
          MSG_GERAN_ERROR_1_G("GSN ""Invalid NPDU in npdu_buff of nsapi %d",
                                                GSN_EXTERN_NSAPI(nsapi));
          break;
        }
      }
      else
      {
        MSG_GERAN_ERROR_1_G("GSN ""NSAPI %d: No N-PDUs to re-transmit",GSN_EXTERN_NSAPI(nsapi));
      }

    }

      /* This point is reached only if all the N-PDUs in
      ** the buffer have been re-transmitted. Clear Re-Tr flag.
      */
      if ( !susp_flag )
        nsapi_e_ptr->ule.retr = FALSE;

    }
    }


/*===========================================================================
===
===  FUNCTION      GSN_SEND_RETR_REQ()
===
===  DESCRIPTION
===
===   Sends retransmit request to PS or COMP task.
===
===  DEPENDENCIES
===
===   None
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

void gsn_send_retr_req(gas_id_t gas_id, gsn_nsapi_t nsapi)
{
  gsn_internal_msg_t *msg_ptr;

  /* Allocate memory to convey SNDCP internal cmd */
  msg_ptr = gsn_int_get_mes_buf();
  GSN_ASSERT( msg_ptr != NULL );

  msg_ptr->cmd_id              = RE_TX_CMD;
  msg_ptr->gsn_cmd.re_tx.nsapi = nsapi;
  msg_ptr->gsn_cmd.re_tx.gas_id= gas_id;

  gsn_int_put_mes( msg_ptr );
}


/*===========================================================================
===
===  FUNCTION      GSN_LL_CTRL_PRIM_GET_GAS_ID()
===
===  DESCRIPTION
===
===   Processes LLC->SNDCP LL control primitives and extracts GAS_ID.
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

gas_id_t gsn_ll_ctrl_prim_get_gas_id( gprs_ll_dl_sig_mes_t *mes_ptr )
{

  gas_id_t              gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;

  switch( mes_ptr->mes.prim )
  {
  case LL_RESET_IND:

    gas_id = mes_ptr->mes.rst_ind_mes.gas_id;

    break;

  case LL_ESTABLISH_IND:

    gas_id = mes_ptr->mes.est_ind_mes.gas_id;

    break;

  case LL_ESTABLISH_CNF:

    gas_id = mes_ptr->mes.est_cnf_mes.gas_id;

    break;

  case LL_RELEASE_IND:

    gas_id = mes_ptr->mes.rel_ind_mes.gas_id;

    break;

  case LL_RELEASE_CNF:

    gas_id = mes_ptr->mes.rel_cnf_mes.gas_id;

    break;

  case LL_XID_IND:

    gas_id = mes_ptr->mes.xid_ind_mes.gas_id;

    break;

  case LL_XID_CNF:

    gas_id = mes_ptr->mes.xid_cnf_mes.gas_id;

    break;

  case LL_STATUS_IND:

    gas_id = mes_ptr->mes.status_ind_mes.gas_id;

    break;

  case LL_DATA_CNF:

    gas_id = mes_ptr->mes.data_cnf_mes.gas_id;

    break;

  case LL_UNITDATA_CNF:

    gas_id = mes_ptr->mes.udata_cnf_mes.gas_id;

    break;

  default:

    MSG_GERAN_ERROR_2_G("GSN ""LL %d: Inval control primitive %d",
      gsn_extern_sapi[mes_ptr->sapi],mes_ptr->mes.prim);

    break;
  }

  return ( gas_id );
}


/*===========================================================================
===
===  FUNCTION      GSN_LL_CTRL_PRIM_FREE_XID_PDU()
===
===  DESCRIPTION
===
===   Processes LLC->SNDCP LL control primitives and frees XID PDU.
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

void gsn_ll_ctrl_prim_free_xid_pdu( gprs_ll_dl_sig_mes_t *ll_msg_ptr )
{

  /* Free memory used to convey XID pdu */
  switch( ll_msg_ptr->mes.prim )
  {
    case  LL_ESTABLISH_IND:
      if( ll_msg_ptr->mes.est_ind_mes.l3_xid_req)
        GPRS_PDU_FREE( &ll_msg_ptr->mes.est_ind_mes.l3_xid_req);
      break;

    case LL_ESTABLISH_CNF:
      if( ll_msg_ptr->mes.est_cnf_mes.l3_xid_neg)
        GPRS_PDU_FREE( &ll_msg_ptr->mes.est_cnf_mes.l3_xid_neg);
      break;

    case LL_XID_IND:
      if( ll_msg_ptr->mes.xid_ind_mes.l3_xid_req)
        GPRS_PDU_FREE( &ll_msg_ptr->mes.xid_ind_mes.l3_xid_req);
              break;

    case LL_XID_CNF:
      if( ll_msg_ptr->mes.xid_cnf_mes.l3_xid_neg)
        GPRS_PDU_FREE( &ll_msg_ptr->mes.xid_cnf_mes.l3_xid_neg);
      break;

    default:
      break;
  }
}



/*===========================================================================
===
===  FUNCTION      GSN_LL_CTRL_PRIM_HDLR()
===
===  DESCRIPTION
===
===   Processes LLC->SNDCP LL control primitives.
===   Note, this function does not process LL-UNITDATA primitive. Refer to
===   gsndcp_process_ll_dl_queue().
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

void gsn_ll_ctrl_prim_hdlr
(
  gas_id_t               gas_id,
  gprs_ll_dl_sig_mes_u  *mes_ptr,   /* LLC->SNDCP control primitive */
  gsn_sapi_t             sapi       /* SAPI number */
)
{

  gsn_nsapi_q_t  *item_ptr;

  if( actv_sapis[gas_id][sapi] != NULL )
  {

    /* GAS_ID received is not the expected one, print this error message */
    if ( actv_sapis[gas_id][sapi]->gas_id != gas_id )
    {
      MSG_GERAN_ERROR_3("GSN ""Invalid GAS_ID: Cur = %d, Rcvd = %d",
         actv_sapis[gas_id][sapi]->gas_id,
         gas_id,
         0);
    }

    switch( mes_ptr->prim )
    {
    case LL_RESET_IND:

      MSG_GERAN_MED_1_G("GSN ""LL %d: Rcvd LL_RESET_IND",gsn_extern_sapi[sapi]);

      gsn_process_ll_rst_ind( gas_id, actv_sapis[gas_id][sapi], &mes_ptr->rst_ind_mes);

      break;

    case LL_ESTABLISH_IND:

      MSG_GERAN_MED_1_G("GSN ""LL %d: Rcvd LL_EST_IND",gsn_extern_sapi[sapi]);

      gsn_process_ll_est_ind( gas_id, actv_sapis[gas_id][sapi], &mes_ptr->est_ind_mes);

      break;

    case LL_ESTABLISH_CNF:

      MSG_GERAN_MED_1_G("GSN ""LL %d: Rcvd LL_EST_CNF",gsn_extern_sapi[sapi]);

      gsn_process_ll_est_cnf( gas_id, actv_sapis[gas_id][sapi], &mes_ptr->est_cnf_mes);

      break;

    case LL_RELEASE_IND:

      MSG_GERAN_MED_1_G("GSN ""LL %d: Rcvd LL_REL_IND",gsn_extern_sapi[sapi]);

      gsn_process_ll_rel_ind( gas_id, actv_sapis[gas_id][sapi], &mes_ptr->rel_ind_mes);

      break;

    case LL_RELEASE_CNF:

      MSG_GERAN_MED_1_G("GSN ""LL %d: Rcvd LL_REL_CNF",gsn_extern_sapi[sapi]);

      gsn_process_ll_rel_cnf( gas_id, actv_sapis[gas_id][sapi], &mes_ptr->rel_cnf_mes);

      break;

    case LL_XID_IND:

      MSG_GERAN_MED_1_G("GSN ""LL %d: Rcvd LL_XID_IND",gsn_extern_sapi[sapi]);

      gsn_process_ll_xid_ind( gas_id, actv_sapis[gas_id][sapi], &mes_ptr->xid_ind_mes);

      break;

    case LL_XID_CNF:

      MSG_GERAN_MED_1_G("GSN ""LL %d: Rcvd LL_XID_CNF",gsn_extern_sapi[sapi]);

      gsn_process_ll_xid_cnf( gas_id, actv_sapis[gas_id][sapi], &mes_ptr->xid_cnf_mes);

      break;

    case LL_STATUS_IND:

      MSG_GERAN_MED_1_G("GSN ""LL %d: Rcvd LL_STATUS_IND",gsn_extern_sapi[sapi]);

      gsn_process_ll_stat_ind( gas_id, actv_sapis[gas_id][sapi], &mes_ptr->status_ind_mes);

      break;

    case LL_DATA_CNF:

      /* MSG_GERAN_HIGH_1_G("GSN ""LL %d: Rcvd LL_DATA_CNF",gsn_extern_sapi[sapi]); */

      gsn_process_ll_data_cnf( gas_id, actv_sapis[gas_id][sapi], &mes_ptr->data_cnf_mes);

      break;

    case LL_UNITDATA_CNF:

     /* MSG_GERAN_HIGH_1_G("GSN ""LL %d: Rcvd LL_UDATA_CNF",gsn_extern_sapi[sapi]) */

      gsn_process_ll_udata_cnf( gas_id, actv_sapis[gas_id][sapi], &mes_ptr->udata_cnf_mes);

      break;

    default:

      MSG_GERAN_ERROR_2_G("GSN ""LL %d: Inval control primitive %d",
        gsn_extern_sapi[sapi],mes_ptr->prim);

      break;
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G("GSN " "Invalid SAPI %d", gsn_extern_sapi[sapi]);
  }

  /* Update diag for all NSAPIs mapped on the SAPI */

  if( actv_sapis[gas_id][sapi] != NULL )
  {

    if( actv_sapis[gas_id][sapi]->is_nsapi_ptr_q_valid )
    {
      item_ptr = (gsn_nsapi_q_t*)q_check( &(actv_sapis[gas_id][sapi]->nsapi_ptr_q) );

      if( item_ptr )
      {
        while( item_ptr != NULL )
        {
          gsn_encode_cntx_diag_rpt( gas_id, (uint8) item_ptr->nsapi_ptr->nsapi );
          gsn_encode_state_diag_rpt( gas_id, (uint8) item_ptr->nsapi_ptr->nsapi );

          /* Get next NSAPI from the list */
          item_ptr = (gsn_nsapi_q_t*)q_next( &(actv_sapis[gas_id][sapi]->nsapi_ptr_q),
                                             &item_ptr->link );
        }
      }
    }
    else
    {
      MSG_GERAN_ERROR_1_G("GSN " "NSAPI ptr queue invalid, SAPI %d", gsn_extern_sapi[sapi]);
    }
  }
}


/*===========================================================================
===
===  FUNCTION      GSN_SM_PRIM_GET_GAS_ID()
===
===  DESCRIPTION
===
===   Processes SM->SNDCP SNSM primitives and extract GAS_ID.
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
gas_id_t gsn_sm_prim_get_gas_id (const gsmsn_mes_type * mes_ptr)
{

  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;

  switch( mes_ptr->mes.prim )
  {
  case GSNSM_ACTIVATE_IND:

    gas_id = geran_map_nas_id_to_gas_id( mes_ptr->mes.snsm_activ_ind.as_id );

    break;

  case GSNSM_DEACTIVATE_IND:

    gas_id = geran_map_nas_id_to_gas_id( mes_ptr->mes.snsm_deactiv_ind.as_id );

    break;

  case GSNSM_MODIFY_IND:

    gas_id = geran_map_nas_id_to_gas_id( mes_ptr->mes.snsm_modify_ind.as_id );

    break;

  case GSNSM_SEQUENCE_IND:

    gas_id = geran_map_nas_id_to_gas_id( mes_ptr->mes.snsm_seq_ind.as_id );

    break;

  default:

    MSG_GERAN_ERROR_1_G("GSN ""ME: Rcvd Invalid prim: %d",mes_ptr->mes.prim);

    break;
  }

  return (gas_id);
}


/*===========================================================================
===
===  FUNCTION      GSN_SM_PRIM_HDLR()
===
===  DESCRIPTION
===
===   Processes SM->SNDCP SNSM primitives.
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
void gsn_sm_prim_hdlr( gas_id_t gas_id, const gsmsn_mes_type * mes_ptr)
{
  switch( mes_ptr->mes.prim )
  {
  case GSNSM_ACTIVATE_IND:

    MSG_GERAN_MED_2_G("GSN ""ME: Rcvd SNSM_ACTIV_IND for NSAPI%d, SAPI%d",
        mes_ptr->mes.snsm_activ_ind.nsapi, mes_ptr->mes.snsm_activ_ind.sapi);

    /* Process primitive */
    gsn_process_snsm_actv_ind( gas_id, &mes_ptr->mes.snsm_activ_ind );

    /* Update diag */
    gsn_encode_actv_nsapis_diag_rpt( gas_id );
    gsn_encode_cntx_diag_rpt( gas_id, mes_ptr->mes.snsm_activ_ind.nsapi );
    gsn_encode_state_diag_rpt( gas_id, mes_ptr->mes.snsm_activ_ind.nsapi );

    break;

  case GSNSM_DEACTIVATE_IND:

    if( mes_ptr->mes.snsm_deactiv_ind.num_nsapi )
    {
      MSG_GERAN_MED_1_G("GSN ""ME: Rcvd SNSM_DEACTIV_IND NSAPI%d",
        mes_ptr->mes.snsm_deactiv_ind.nsapi[0]);
    }
    else
    {
      MSG_GERAN_MED_0_G("GSN ""ME: Rcvd SNSM_DEACTIV_IND with num of NSAPIs=0");
    }

    /* Process primitive */
    gsn_process_snsm_deactv_ind( gas_id, &mes_ptr->mes.snsm_deactiv_ind );

    /* Update diag */
    gsn_encode_actv_nsapis_diag_rpt( gas_id );

    break;

  case GSNSM_MODIFY_IND:

    MSG_GERAN_MED_2_G("GSN ""ME: Rcvd SNSM_MODIFY_IND for NSAPI%d, SAPI%d",
      mes_ptr->mes.snsm_modify_ind.nsapi,mes_ptr->mes.snsm_modify_ind.sapi);

    /* Process primitive */
    gsn_process_snsm_mod_ind( gas_id, &mes_ptr->mes.snsm_modify_ind );

    /* Update diag */
    gsn_encode_actv_nsapis_diag_rpt( gas_id );
    gsn_encode_cntx_diag_rpt( gas_id, mes_ptr->mes.snsm_activ_ind.nsapi );
    gsn_encode_state_diag_rpt( gas_id, mes_ptr->mes.snsm_activ_ind.nsapi );

    break;

  case GSNSM_SEQUENCE_IND:

    MSG_GERAN_MED_1_G("GSN ""ME: Rcvd SNSM_SEQUENCE_IND for NSAPI%d",
      mes_ptr->mes.snsm_seq_ind.nsapi);

    /* Process primitive */
    gsn_process_snsm_seq_ind( gas_id, &mes_ptr->mes.snsm_seq_ind );

    /* Update diag */
    gsn_encode_state_diag_rpt( gas_id, mes_ptr->mes.snsm_activ_ind.nsapi );

    break;

  default:

    MSG_GERAN_ERROR_1_G("GSN ""ME: Rcvd Invalid prim: %d",mes_ptr->mes.prim);

    break;
  }
}


/*===========================================================================
===
===  FUNCTION      GSN_INT_PRIM_GET_GAS_ID()
===
===  DESCRIPTION
===
===   Processes SNDCP internal command and returns GAS_ID.
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
gas_id_t gsn_int_prim_get_gas_id ( gsn_internal_msg_t *mes_ptr )
{
  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;

  switch( mes_ptr->cmd_id )
  {
  case( SEND_XID_REQ ):

    gas_id = mes_ptr->gsn_cmd.xid_req.gas_id;

    break;

  case( PDP_REG_REQ ):

    gas_id = mes_ptr->gsn_cmd.pdp_reg_req.gas_id;

    break;

  case (RE_TX_CMD):

    gas_id = mes_ptr->gsn_cmd.re_tx.gas_id;

    break;

  case (TIMER_CMD):

    gas_id = mes_ptr->gsn_cmd.timer.gas_id;

    break;

  default:

    MSG_GERAN_ERROR_1_G("GSN ""Invalid SNDCP cmd id %d",
      mes_ptr->cmd_id);

    break;
  }

  return (gas_id);
}



/*===========================================================================
===
===  FUNCTION      GSN_INT_CMD_HDLR()
===
===  DESCRIPTION
===
===   Processes SNDCP internal commands.
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
void gsn_int_cmd_hdlr( gas_id_t gas_id, const gsn_internal_msg_t *mes_ptr)
{
  gsn_sapi_t     sapi;
  gsn_nsapi_t    gnsapi;
  boolean        valid_reg = TRUE;

  switch( mes_ptr->cmd_id )
  {
  case( SEND_XID_REQ ):

    /* Send XID request with App NSAPIs set to 0, to inform peer
    ** to unassign the PCI compression entity.
    */
    sapi = mes_ptr->gsn_cmd.xid_req.sapi;

    if( actv_sapis[gas_id][sapi] != NULL )
    {
      if( actv_sapis[gas_id][sapi]->state != IDLE )
      {
        /* If ACK SAPI, then re-establish link and include XID Req
        ** in the LL_ESTABLISH_REQ primitive.
        */
        gsn_send_ll_est_req( gas_id, actv_sapis[gas_id][sapi],
          mes_ptr->gsn_cmd.xid_req.xid_pdu );
      }
      else
      {
        /* If UNACK SAPI, then send XID Req primitive.
        */
        gsn_send_ll_xid_req( gas_id, actv_sapis[gas_id][sapi],
          mes_ptr->gsn_cmd.xid_req.xid_pdu );
      }
    }
    else
    {
      MSG_GERAN_ERROR_1_G("GSN "" Inval or Inactive SAPI %d ",gsn_extern_sapi[sapi]);
    }
    break;

  case( PDP_REG_REQ ):

    if( sndcp_init == FALSE )
    {
      MSG_GERAN_ERROR_1_G("GSN " "ME: Un-initialised NSAPI %d",mes_ptr->gsn_cmd.pdp_reg_req.nsapi);
      valid_reg = FALSE;

      /* Indicate result to client. */
      if ( mes_ptr->gsn_cmd.pdp_reg_req.pdp_reg_cnf_fnc_ptr != NULL )
        mes_ptr->gsn_cmd.pdp_reg_req.pdp_reg_cnf_fnc_ptr(
          mes_ptr->gsn_cmd.pdp_reg_req.ds_context, valid_reg);

    }
    else
    {
      /* Translate NSAPI number for SNDCP internal use */
      gnsapi = (gsn_nsapi_t)GSN_INTERN_NSAPI( mes_ptr->gsn_cmd.pdp_reg_req.nsapi );

      if ( GSN_VALID_NSAPI(mes_ptr->gsn_cmd.pdp_reg_req.nsapi) &&
           actv_nsapis[ gas_id ][gnsapi] != NULL                            )
      {
        MSG_GERAN_HIGH_1_G("GSN ""NS %d: Info PDP Registration", mes_ptr->gsn_cmd.pdp_reg_req.nsapi);

        if( mes_ptr->gsn_cmd.pdp_reg_req.pdp_ul_suspend_fnc_ptr == NULL ||
            mes_ptr->gsn_cmd.pdp_reg_req.pdp_ul_resume_fnc_ptr  == NULL ||
            mes_ptr->gsn_cmd.pdp_reg_req.pdp_dl_fnc_ptr         == NULL    )
        {
          MSG_GERAN_MED_3_G("GSN ""Info Reg func suspend_ptr=%p,resume_fnc_ptr=%p,pdp_dl_ptr=%p",
            mes_ptr->gsn_cmd.pdp_reg_req.pdp_ul_suspend_fnc_ptr,
              mes_ptr->gsn_cmd.pdp_reg_req.pdp_ul_resume_fnc_ptr,
                mes_ptr->gsn_cmd.pdp_reg_req.pdp_dl_fnc_ptr);
        }

        actv_nsapis[ gas_id ][gnsapi]->pdp_ul_suspend_fnc =
          mes_ptr->gsn_cmd.pdp_reg_req.pdp_ul_suspend_fnc_ptr;
        actv_nsapis[ gas_id ][gnsapi]->pdp_ul_resume_fnc =
          mes_ptr->gsn_cmd.pdp_reg_req.pdp_ul_resume_fnc_ptr;
        actv_nsapis[ gas_id ][gnsapi]->pdp_frw_dl_data_fnc =
          mes_ptr->gsn_cmd.pdp_reg_req.pdp_dl_fnc_ptr;
        actv_nsapis[ gas_id ][gnsapi]->pdp_reg_cnf_fnc =
          mes_ptr->gsn_cmd.pdp_reg_req.pdp_reg_cnf_fnc_ptr;
        actv_nsapis[ gas_id ][gnsapi]->ds_context =
          mes_ptr->gsn_cmd.pdp_reg_req.ds_context;
        actv_nsapis[ gas_id ][gnsapi]->cipher =
          mes_ptr->gsn_cmd.pdp_reg_req.cipher;

        if ( actv_nsapis[ gas_id ][gnsapi]->ule.susp_flag )
        {
          /* Reinforce flow control using the newly registered parameters. */
          GSN_SET_SUSP_EVENT( gas_id, gnsapi, actv_nsapis[ gas_id ][gnsapi]->ule.susp_flag );
        }
        else
        {
          /* PDP Registration Confirmation callback may be called now as
             as there are no NSAPI suspension events in effect given
             SNSM_DEACTIVATE_RESP is not pending. */
          if ( ( actv_nsapis[ gas_id ][gnsapi]->pdp_reg_cnf_fnc  != NULL  ) &&
               ( actv_nsapis[ gas_id ][gnsapi]->deactv_resp_pend == FALSE )
             )
          {
            actv_nsapis[ gas_id ][gnsapi]->pdp_reg_cnf_fnc(
              actv_nsapis[ gas_id ][gnsapi]->ds_context, valid_reg);
          }
          else if ( ( actv_nsapis[ gas_id ][gnsapi]->pdp_reg_cnf_fnc  != NULL ) &&
                    ( actv_nsapis[ gas_id ][gnsapi]->deactv_resp_pend == TRUE )
                  )
          {
            actv_nsapis[ gas_id ][gnsapi]->pdp_reg_cnf_fnc(
              actv_nsapis[ gas_id ][gnsapi]->ds_context, FALSE );

            MSG_GERAN_MED_1_G("GSN " "NS %d: PDP Reg failed",
                           GSN_EXTERN_NSAPI(gnsapi));
          }

          actv_nsapis[ gas_id ][gnsapi]->pdp_reg_cnf_fnc = NULL;
        }
      }
      else
      {
        valid_reg = FALSE;
        MSG_GERAN_ERROR_1_G("GSN " "ME: Invalid registration NSAPI %d",mes_ptr->gsn_cmd.pdp_reg_req.nsapi);

        /* Indicate result to client. */
        if ( mes_ptr->gsn_cmd.pdp_reg_req.pdp_reg_cnf_fnc_ptr != NULL )
            mes_ptr->gsn_cmd.pdp_reg_req.pdp_reg_cnf_fnc_ptr(
              mes_ptr->gsn_cmd.pdp_reg_req.ds_context, valid_reg);
      }
    }
    break;

  case (RE_TX_CMD):

    if(actv_nsapis[ gas_id ][ mes_ptr->gsn_cmd.re_tx.nsapi ]->ule.retr)
    {
      gsn_retr_npdu_hdlr(gas_id, mes_ptr->gsn_cmd.re_tx.nsapi);
    }
    break;

  case (TIMER_CMD):

    gnsapi = mes_ptr->gsn_cmd.timer.nsapi;

    if ( GSN_VALID_NSAPI ( GSN_EXTERN_NSAPI(gnsapi) ) )
    {
      if ( actv_nsapis[ gas_id ][gnsapi] != NULL )
      {

        /* Send status request to SM indicating timeout. SM will deactivate
        ** PDP context locally.
        */
        if ( mes_ptr->gsn_cmd.timer.tmr_id == SGSN_INITIATED_LINK_EST_FAIL )
        {
          gsn_send_snsm_stat_req( gas_id,
                                  actv_nsapis[ gas_id ][gnsapi]->sapi_ptr->sapi,
                                  actv_nsapis[ gas_id ][gnsapi]->sapi_ptr->tlli,
                                  SNSM_MODIFY_FAILURE
                                );
        }
        else
        {
          MSG_GERAN_ERROR_1_G("GSN " "Invalid timer id %d",
                          mes_ptr->gsn_cmd.timer.tmr_id);
        }
      }
      else
      {
        MSG_GERAN_ERROR_1_G("GSN " "Invalid NSAPI %d",
                        GSN_EXTERN_NSAPI(gnsapi));
      }
    }
    else
    {
      MSG_GERAN_ERROR_1_G("GSN " "Invalid NSAPI %d",
                      GSN_EXTERN_NSAPI(gnsapi));
    }
    break;

  default:

    MSG_GERAN_ERROR_1_G("GSN ""Invalid SNDCP cmd id %d",
      mes_ptr->cmd_id);

    break;
  }
}

/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_LL_DATA_CNF()
===
===  DESCRIPTION
===
===   To be called by LLC to inform SNDCP layer about successful
===   transmission of a SN-PDU. When the successful transmission of
===   the whole N-PDU has been confirmed, the buffered N-PDU shall be deleted.
===
===  DEPENDENCIES
===
===   SNDCP uses rex_suspend_task to suspend LLC task. Therefore, this function
===   must be called only from LLC task context and not in an ISR context.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   Runs in LLC task context
===
===========================================================================*/
void gsn_process_ll_data_cnf( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr, ll_data_cnf_t *ll_mes)
{
  uint8 npdu_num     = ll_mes->sndcp_ref.npdu_num;
  uint8 seg_num      = ll_mes->sndcp_ref.seg_num;
  gsn_nsapi_t nsapi;

  if(GSN_VALID_NSAPI( ll_mes->sndcp_ref.nsapi ) == FALSE)
  {
    MSG_GERAN_ERROR_3_G("GSN " "Invalid NSAPI %d ", ll_mes->sndcp_ref.nsapi,
                                      0,
                                      0);
    return;
  }

  nsapi = GSN_INTERN_NSAPI( ll_mes->sndcp_ref.nsapi);

  if((actv_nsapis[ gas_id ][nsapi] == NULL) ||
     (actv_nsapis[ gas_id ][nsapi]->sapi_ptr->sapi != sapi_ptr->sapi))
  {
    MSG_GERAN_ERROR_3_G("GSN " "Inactive NSAPI %d or invalid SAPI %d", ll_mes->sndcp_ref.nsapi,
                                                         sapi_ptr->sapi,
                                                         0);
    return;
  }

  switch( sapi_ptr->sapi )
  {
  case GSN_SAPI_3:
    MSG_GERAN_MED_3_G("GSN ""LL 3: Rcvd LL_DATA_CNF %d:%d:%d",
                  GSN_EXTERN_NSAPI(nsapi), npdu_num, seg_num );
    break;

  case GSN_SAPI_5:
    MSG_GERAN_MED_3_G("GSN ""LL 5: Rcvd LL_DATA_CNF %d:%d:%d",
                  GSN_EXTERN_NSAPI(nsapi), npdu_num, seg_num );
    break;

  case GSN_SAPI_9:
    MSG_GERAN_MED_3_G("GSN ""LL 9: Rcvd LL_DATA_CNF %d:%d:%d",
                  GSN_EXTERN_NSAPI(nsapi), npdu_num, seg_num );
    break;

  case GSN_SAPI_11:
    MSG_GERAN_MED_3_G("GSN ""LL 11: Rcvd LL_DATA_CNF %d:%d:%d",
                GSN_EXTERN_NSAPI(nsapi), npdu_num, seg_num );
    break;

  default:
    break;
  }

   /* Switch on SAPI's state */
  switch( sapi_ptr->state )
  {
/*-------------------------------------------------------------------------------*/
  case( ESTABLISHED ):
    {
      gsn_seg_ref_item_t  *ref_item_ptr;
      gsn_seg_ref_item_t  *ref_next_item_ptr;
      gsn_npdu_item_t     *npdu_item_ptr, *first_item_ptr,*seq_item_ptr;
      gsn_npdu_item_t     *npdu_next_item_ptr, *seq_next_item_ptr;
      gsn_nsapi_e_t       *nsapi_e_ptr = actv_nsapis[ gas_id ][nsapi];


      /* Get N-PDU from the buffer */
      npdu_item_ptr = q_check( &nsapi_e_ptr->ule.npdu_buff );

      while( npdu_item_ptr != NULL )
      {
        /* Get next item before deletion of the link
        ** reference of the current item.
        */
        npdu_next_item_ptr =
             q_next( &nsapi_e_ptr->ule.npdu_buff, &npdu_item_ptr->link );

        if( npdu_item_ptr->ack_send_npdu == npdu_num )
        {
          if( npdu_item_ptr->seg_ref_buff_valid == TRUE )
          {
            if( q_cnt( &(npdu_item_ptr->seg_ref_buff) ) )
            {

              /* Check if there are any segments to be confirmed */
              ref_item_ptr = q_check( &(npdu_item_ptr->seg_ref_buff) );

              while( ref_item_ptr )
              {
                /* Get next item before deletion of the link
                ** reference of the current item.
                */
                ref_next_item_ptr =
                  q_next( &npdu_item_ptr->seg_ref_buff, &ref_item_ptr->link );

                if( ref_item_ptr->seg_num == seg_num )
                {
                  /* Remove segment from the buffer.
                  */
                  q_delete
                    (
                        &npdu_item_ptr->seg_ref_buff,
                        &ref_item_ptr->link
                     );

                  GSN_MEM_FREE( ref_item_ptr );

                  if( !q_cnt( &(npdu_item_ptr->seg_ref_buff)  ) )
                  {
                    /* If the last segment of a buffered N-PDU has been
                    ** confirmed, delete the buffered NDU if it's the first in
                    ** sequence of the buffered NPDUs. Otherwise, just mark
                    ** the NPDU as acknowledged.
                    */
                    first_item_ptr = q_check( &nsapi_e_ptr->ule.npdu_buff );

                    if( first_item_ptr != NULL )
                    {

                    if( first_item_ptr->ack_send_npdu ==
                        npdu_item_ptr->ack_send_npdu )
                    {

                      /* Set seg_ref_buff as invalid for this NPDU */
                      npdu_item_ptr->seg_ref_buff_valid = FALSE;

                      /* Free memory, mutex etc. used by NPDU segment queue
                      ** attached to NPDU.
                      */
                      q_destroy ( &npdu_item_ptr->seg_ref_buff );

                      seq_item_ptr =
                        q_next( &nsapi_e_ptr->ule.npdu_buff, &first_item_ptr->link );

                      /* Remove N-PDU from buffer and free memory back to DSM mem pool */
                      q_delete
                        (
                            &nsapi_e_ptr->ule.npdu_buff,
                            &npdu_item_ptr->link
                         );

                      GPRS_PDU_FREE( &npdu_item_ptr->npdu_ptr );
                      GSN_MEM_FREE( npdu_item_ptr );

                      while( seq_item_ptr )
                      {
                        seq_next_item_ptr =
                          q_next( &nsapi_e_ptr->ule.npdu_buff, &seq_item_ptr->link );

                        if( ( seq_item_ptr->npdu_acked                 ) &&
                            ( seq_item_ptr->seg_ref_buff_valid == TRUE )    )
                        {

                          /* Set seg_ref_buff as invalid for this NPDU */
                          seq_item_ptr->seg_ref_buff_valid = FALSE;

                          /* Free memory,mutex etc. used by NPDU segment queue
                          ** attached to NPDU.
                          */
                          q_destroy ( &seq_item_ptr->seg_ref_buff );

                          /* Remove N-PDU from buffer and free memory back to DSM mem pool */
                          q_delete
                            (
                                &nsapi_e_ptr->ule.npdu_buff,
                                &seq_item_ptr->link
                             );

                          GPRS_PDU_FREE( &seq_item_ptr->npdu_ptr );
                          GSN_MEM_FREE( seq_item_ptr );

                           /* get next item */
                          seq_item_ptr = seq_next_item_ptr;

                          /* npdu next item ptr is no longer valid as NPDU was
                             deleted. Update it here. */
                          npdu_next_item_ptr = seq_item_ptr;
                        }
                        else
                          break;
                      } /* while */
                    }
                    else
                    {
                      /* Just mark the NPDU as acknowleaged */
                      npdu_item_ptr->npdu_acked = TRUE;
                    }
                    }
                    else
                    {
                      MSG_GERAN_HIGH_1_G("GSN ""NS %d: first_item_ptr is NULL !!",
                                                  GSN_EXTERN_NSAPI(nsapi));
                    }

                    /* Resume PDP UL data flow if suspented due to
                       GSN_UL_SUSP_MAX_NPDU event. */
                    if( q_cnt(&nsapi_e_ptr->ule.npdu_buff) < GPRS_MAX_RETR_NPDUS)
                    {
                      GSN_LOCK();
                      if( nsapi_e_ptr->ule.susp_flag & GSN_UL_SUSP_MAX_NPDU )
                      {
                        nsapi_e_ptr->ule.susp_flag &= ~GSN_UL_SUSP_MAX_NPDU;
                        if( !nsapi_e_ptr->ule.susp_flag )
                        {
                          /* Check for any data in the SMD->SNDCP WM. If any, set
                             each enqueue signal to resume data flow */
                          gsndcp_check_data_in_smd_to_sndcp_wm(gas_id, nsapi);
                          MSG_GERAN_MED_1_G("GSN ""NS %d: PDP UL Resume Req",
                                                    GSN_EXTERN_NSAPI(nsapi));
                        }

                        /* Update diag */
                        gsn_encode_state_diag_rpt( gas_id, (uint8) nsapi_e_ptr->nsapi );
                      }
                      GSN_UNLOCK();
                    }

                    /* Code execution is in here because
                    ** q_cnt( &(npdu_item_ptr->seg_ref_buff) = 0. That means no
                    ** further segments in this NPDU. If this is the case,
                    ** ref_next_item_ptr should be NULL.
                    ** To keep KlocWork happy, break out of while(ref_item_ptr)
                    ** loop and start with next NPDU, if any.
                    */
                    if (ref_next_item_ptr != NULL)
                    {
                      MSG_GERAN_HIGH_1_G("GSN ""NS %d, Seg cnt=0, but ref_next_item_ptr !=NULL",
                                             GSN_EXTERN_NSAPI(nsapi));
                    }
                    break;
                  }
                }

                /* Get next segment info */
                ref_item_ptr = ref_next_item_ptr;

              }/* while ( ref_item_ptr != NULL ) */
            }
            else
            {
              MSG_GERAN_HIGH_0_G("GSN ""LL_DATA_CNF received while no CNF required");
            }
          }
          else
          {
            MSG_GERAN_MED_0_G("GSN ""No seg ref buff");
          }
        }

        /* get next Buffered N-PDU */
        npdu_item_ptr = npdu_next_item_ptr;

      }/* while ( npdu_item_ptr != NULL ) */
    }

    break;

/*-------------------------------------------------------------------------------*/
    default:

    MSG_GERAN_ERROR_2_G("GSN ""LL %d: DATA_CNF Rcved in invalid SAPI state: %d",
                  sapi_ptr->sapi, sapi_ptr->state);
    break;

  }
}


/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_LL_UDATA_CNF()
===
===  DESCRIPTION
===
===   SNDCP processes LL-UNITDATA-CNF and removes indicated NPDU, if present,
===   from unack retx buffer.
===
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
void gsn_process_ll_udata_cnf( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr, ll_unitdata_cnf_t *ll_mes)
{
  uint16             npdu_num = ll_mes->sndcp_ref.npdu_num;
  uint8              seg_num  = ll_mes->sndcp_ref.seg_num;
  gsn_nsapi_t        nsapi;
  gsn_seg_ref_item_t *seg_item_ptr;
  gsn_seg_ref_item_t *next_seg_item_ptr;
  gsn_npdu_item_t    *npdu_item_ptr;
  gsn_npdu_item_t    *next_npdu_item_ptr;
  gsn_nsapi_e_t      *nsapi_e_ptr;


  if(GSN_VALID_NSAPI( ll_mes->sndcp_ref.nsapi ) == FALSE)
  {
    MSG_GERAN_ERROR_3_G("GSN " "Invalid NSAPI %d ", ll_mes->sndcp_ref.nsapi,
                                      0,
                                      0);
    return;
  }

  nsapi = GSN_INTERN_NSAPI( ll_mes->sndcp_ref.nsapi);

  if((actv_nsapis[ gas_id ][nsapi] == NULL) ||
     (actv_nsapis[ gas_id ][nsapi]->sapi_ptr->sapi != sapi_ptr->sapi))
  {
    MSG_GERAN_ERROR_3_G("GSN " "Inactive NSAPI %d or invalid SAPI %d", ll_mes->sndcp_ref.nsapi,
                                                         sapi_ptr->sapi,
                                                         0);
    return;
  }


  switch( sapi_ptr->sapi )
  {
    case GSN_SAPI_3:
      MSG_GERAN_MED_3_G("GSN ""LL 3: Rcvd LL_UDATA_CNF %d:%d:%d",
                    GSN_EXTERN_NSAPI(nsapi), npdu_num, seg_num );
    break;

    case GSN_SAPI_5:
      MSG_GERAN_MED_3_G("GSN ""LL 5: Rcvd LL_UDATA_CNF %d:%d:%d",
                    GSN_EXTERN_NSAPI(nsapi), npdu_num, seg_num );
      break;

    case GSN_SAPI_9:
      MSG_GERAN_MED_3_G("GSN ""LL 9: Rcvd LL_UDATA_CNF %d:%d:%d",
                    GSN_EXTERN_NSAPI(nsapi), npdu_num, seg_num );
      break;

    case GSN_SAPI_11:
      MSG_GERAN_MED_3_G("GSN ""LL 11: Rcvd LL_UDATA_CNF %d:%d:%d",
                    GSN_EXTERN_NSAPI(nsapi), npdu_num, seg_num );
      break;

    default:
      break;
  }


  /* Get NSAPI ptr. */
  nsapi_e_ptr = actv_nsapis[ gas_id ][nsapi];

  /* Get N-PDU from the buffer */
  npdu_item_ptr = q_check( &nsapi_e_ptr->ule.npdu_buff );

  while( npdu_item_ptr != NULL )
  {
    /* Get next item before deletion of the link reference of the current
    ** item.
    */
    next_npdu_item_ptr =
      q_next( &nsapi_e_ptr->ule.npdu_buff, &npdu_item_ptr->link );

    if( npdu_item_ptr->unack_send_npdu == npdu_num )
    {
      if( npdu_item_ptr->seg_ref_buff_valid == TRUE )
      {
        /* Check if there are any segments to be confirmed. */
        seg_item_ptr = q_check( &(npdu_item_ptr->seg_ref_buff) );
        if( seg_item_ptr )
        {
          while( seg_item_ptr )
          {
            /* Get next item before deletion of the link reference of the
            ** current item.
            */
            next_seg_item_ptr =
              q_next( &npdu_item_ptr->seg_ref_buff, &seg_item_ptr->link );

            if( seg_item_ptr->seg_num == seg_num )
            {
              /* Remove segment item ref from the buffer. */
              q_delete
              (
                  &npdu_item_ptr->seg_ref_buff,
                  &seg_item_ptr->link
              );

              GSN_MEM_FREE( seg_item_ptr );

              if( !q_cnt( &(npdu_item_ptr->seg_ref_buff)  ) )
              {
                /* Set seg_ref_buff as invalid for this NPDU */
                npdu_item_ptr->seg_ref_buff_valid = FALSE;

                /* Free memory, mutex etc. used by NPDU segment queue
                ** attached to NPDU.
                */
                q_destroy ( &npdu_item_ptr->seg_ref_buff );

                /* If the last segment of a buffered N-PDU has been confirmed,
                ** delete the buffered NDU.
                */

                /* Remove N-PDU from buffer and free memory back to DSM mem pool
                */
                q_delete
                (
                    &nsapi_e_ptr->ule.npdu_buff,
                    &npdu_item_ptr->link
                );

                GPRS_PDU_FREE( &npdu_item_ptr->npdu_ptr );
                GSN_MEM_FREE( npdu_item_ptr );

                break;
              }
            }
            /* Get next segment info */
            seg_item_ptr = next_seg_item_ptr;

          } /* while ( seg_item_ptr != NULL ) */
        }
        else
        {
          MSG_GERAN_HIGH_0_G("GSN ""LL_DATA_CNF received while no CNF required");
        }
      }
      else
      {
        MSG_GERAN_MED_0_G("GSN ""No seg ref buff");
      }
    }

    /* get next Buffered N-PDU */
    npdu_item_ptr = next_npdu_item_ptr;

  } /* while ( npdu_item_ptr != NULL ) */
}


/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_SN_DATA_REQ()
===
===  DESCRIPTION
===
===   Processes SN-DATA request from PDP for UL ACK data transmission.
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
void gsn_process_sn_data_req( gas_id_t      gas_id,
                              gsn_nsapi_t   nsapi,
                              dsm_item_type **npdu,
                              boolean              fback_pdu_flag,
                              geran_pdu_priority_t pdu_priority
                            )
{
  gsn_npdu_item_t     *item_ptr;
  gsn_nsapi_e_t       *nsapi_ptr = actv_nsapis[ gas_id ][nsapi];
  uint8               npdu_num;
  dsm_item_type       *dup_npdu_ptr = NULL;
  uint8               pcomp = 0;
  uint8               dcomp = 0;

  MSG_GERAN_LOW_1_G("GSN ""NS %d: Rcvd SN_DATA_REQ",GSN_EXTERN_NSAPI(nsapi));

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )

  /* For re-transmission purposes, copy the dsm item if PCI compression
  ** is required otherwise duplicate the dsm item. This is needed because
  ** the PCI compression algorithm modifies the source PDU chain, therby
  ** potentially corrupting duplicate DSM references held by other entities.
  */
  if ( nsapi_ptr->pci_comp )
  {
    dup_npdu_ptr = gprs_pdu_copy( *npdu );
  }
  else
  {
    dup_npdu_ptr = gprs_pdu_duplicate( *npdu, 0, GPRS_PDU_LENGTH(*npdu) );
  }

  if( dup_npdu_ptr == NULL )
  {
    /* Run out of DSM resource. Do not compress the N-PDU */
    MSG_GERAN_HIGH_2_G("GSN ""NS %d: Run out of DSM resources. No comp for N-PDU %d ",
        GSN_EXTERN_NSAPI(nsapi), nsapi_ptr->ule.ack_send_npdu);
  }
  else if ( !fback_pdu_flag )
  {
    /* Compress N-PDU. Protocol compression first and then data compression
    */
#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP

    /* Compress PCI of the N-PDU */
    pcomp = gsn_pci_compress( gas_id, nsapi_ptr, &dup_npdu_ptr );

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP

    if( dup_npdu_ptr != NULL )
    {
      /* Compress data of the N-PDU */
      dcomp = gsn_data_compress( gas_id, nsapi_ptr, &dup_npdu_ptr );
    }
#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */

    /* Check if the dsm_item conveying the copy of the N-PDU
    ** has been released by the compression algorithm due to
    ** invalid TCP/UDP/IP packet structure. If so, abandon
    ** compression for the N-PDU.
    */
    if( dup_npdu_ptr == NULL )
    {
      pcomp = 0;
      dcomp = 0;

      MSG_GERAN_HIGH_2_G("GSN ""NS %d: comp failed. No comp for N-PDU %d",
        GSN_EXTERN_NSAPI(nsapi), nsapi_ptr->ule.ack_send_npdu);
    }
  }
#endif  /* #if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_ ... */

  if( dup_npdu_ptr == NULL )
  {
    /* Either no compression is enabled or compression failed.
    ** For both cases, duplicate the dsm item for re-transmission purposes.
    */
    dup_npdu_ptr = gprs_pdu_duplicate( *npdu, 0, GPRS_PDU_LENGTH(*npdu) );

    if( dup_npdu_ptr == NULL )
    {
      /* gprs_pdu_duplicate() failed. Drop N-PDU and recover gracefully */
      if( *npdu )
      {
         GPRS_PDU_FREE( npdu );
      }

      MSG_GERAN_ERROR_2_G("GSN ""NS %d: gprs_pdu_duplicate() failed. N-PDU %d dropped",
          GSN_EXTERN_NSAPI(nsapi), nsapi_ptr->ule.ack_send_npdu);

      return;
    }
  }

  /* Increment Send N-PDU num modulo 256 */
  npdu_num = nsapi_ptr->ule.ack_send_npdu;
  nsapi_ptr->ule.ack_send_npdu = (nsapi_ptr->ule.ack_send_npdu + 1) ;

  /* Store N-PDU incase there is a need for retransmission.
  ** Must be deleted when it is acked.
  */

  /* Allocate memory for the item to be queued */
  item_ptr = GPRS_MEM_MALLOC( sizeof(gsn_npdu_item_t) );
  GSN_ASSERT( item_ptr != NULL );

  /* Heap memory allocated for sapi is initialised to avoid mutex creation
     issue in mustang builds caused by passing in non-null value for
     actv_sapis[gas_id][sapi]->nsapi_ptr_q.mutex into q_init(). */

  memset(item_ptr, 0, sizeof(gsn_npdu_item_t));

  /* Initialise item */
  item_ptr->ack_send_npdu = npdu_num;
  item_ptr->re_tr_flag    = FALSE;
  item_ptr->npdu_acked    = FALSE;
  item_ptr->fback_pdu_flag = fback_pdu_flag;

  /* Iniatialise queue that holds NPDU segments */
  (void) q_init( &item_ptr->seg_ref_buff );

  /* Set flag to indicate seg_ref_buff as valid for this NPDU */
  item_ptr->seg_ref_buff_valid = TRUE;

  /* Suspend LLC task to prevent processing of LL_DATA_CNF
  ** that could modify the contents of the npdu_buff.
  */
  GSN_LOCK();

  /* Put copy of the N-PDU in the re-transmission buffer
  */

  item_ptr->npdu_ptr = *npdu;

  /* Delink pointer */
  *npdu = NULL;

  (void) q_link(item_ptr, &(item_ptr->link));
  q_put( &(nsapi_ptr->ule.npdu_buff), &(item_ptr->link) );

  if( q_cnt( &(nsapi_ptr->ule.npdu_buff)) >= GPRS_MAX_RETR_NPDUS )
  {
    /* Suspend data flow untill one of the buffered N-PDUs gets acked
    */
    GSN_SET_SUSP_EVENT( gas_id, nsapi, GSN_UL_SUSP_MAX_NPDU );
  }

  /* Resume LLC task */
  GSN_UNLOCK();

  /* Segment N-PDU, if needed, and send LL-DATA primitives to LLC */
  gsn_process_ul_ack_npdu(  gas_id, nsapi, &dup_npdu_ptr, item_ptr, FALSE, pcomp, dcomp, pdu_priority );

}

/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_SN_UDATA_REQ()
===
===  DESCRIPTION
===
===   Processes SN-UNITDATA request from PDP for UL UNACK data transmission.
===   This function is also used in UNACK mode for re-transmitting N-PDUs in
===   retransmission buffer. Input parameter retx_npdu_item_ptr is set to a
===   N-PDU item from retransmission buffer (npdu_buff) when this function is
===   used for N-PDU retransmissions.
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
void gsn_process_sn_udata_req
(
  gas_id_t        gas_id,
  gsn_nsapi_t     nsapi,
  dsm_item_type   **orig_npdu,
  boolean         fback_pdu_flag,
  gsn_npdu_item_t      *retx_npdu_item_ptr,
  geran_pdu_priority_t pdu_priority
)
{
  gprs_ll_ul_dat_mes_t   *ll_msg;
  ll_unitdata_req_t      *udata_msg;
  uint16                 n201_u, len, stat_len;
  uint8                  ext_hdr[UNACK_PDU_HDR_1ST_SEG];
  gsn_unitdata_pdu_hdr_t sn_udata_hdr;
  uint8                  seg_num, hdr_len;
  boolean                first_seg;
  gsn_sapi_t             sapi;
  gsn_nsapi_e_t          *nsapi_ptr  = actv_nsapis[ gas_id ][nsapi];

  gsn_npdu_item_t        *npdu_item_ptr;
  gsn_npdu_item_t        *head_npdu_item_ptr;
  gsn_seg_ref_item_t     *seg_item_ptr;
  gsn_seg_ref_item_t     *next_seg_item_ptr;

  dsm_item_type          *cp_npdu = NULL;

  MSG_GERAN_LOW_1_G("GSN ""NS %d: Rcvd SN_UNITDATA_REQ",GSN_EXTERN_NSAPI(nsapi));

  /* Initialise PCOMP and DCOMP values */
  sn_udata_hdr.pcomp = 0;
  sn_udata_hdr.dcomp = 0;

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )

  /* Copy the dsm item if compression is enabled to avoid modifying
  ** any copies of the item that might been stored in upper layers i.e. TCP.
  */
  if( nsapi_ptr->data_comp || nsapi_ptr->pci_comp )
  {
    /* For re-transmission purposes, copy the dsm item if PCI compression
    ** is required otherwise duplicate the dsm item. This is needed because
    ** the PCI compression algorithm modifies the source PDU chain, thereby
    ** potentially corrupting duplicate DSM references held by other entities.
    */
    if ( nsapi_ptr->pci_comp )
    {
      cp_npdu = gprs_pdu_copy( *orig_npdu );
    }
    else
    {
      cp_npdu = gprs_pdu_duplicate( *orig_npdu, 0, GPRS_PDU_LENGTH(*orig_npdu) );
    }

    if( cp_npdu == NULL )
    {
      /* Run out of DSM resource. Do not compress the N-PDU */
      MSG_GERAN_HIGH_2_G("GSN ""NS %d: Run out of DSM resources. No comp for N-PDU %d ",
          GSN_EXTERN_NSAPI(nsapi), nsapi_ptr->ule.unack_send_npdu);
    }
    else if ( !fback_pdu_flag )
    {

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP

      /* Compress PCI of the N-PDU */
      sn_udata_hdr.pcomp = gsn_pci_compress( gas_id, nsapi_ptr, &cp_npdu );

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP

      if( cp_npdu != NULL )
      {
        /* Compress data of the N-PDU */
        sn_udata_hdr.dcomp = gsn_data_compress( gas_id, nsapi_ptr, &cp_npdu );
      }

      /* Check if the dsm_item conveying the N-PDU has been released by the
      ** compression algorithm due to invalid packet structure. If so, set
      ** pcomp/dcomp to 0.
      */

      if( cp_npdu == NULL )
      {
        sn_udata_hdr.pcomp = 0;
        sn_udata_hdr.dcomp = 0;

        MSG_GERAN_HIGH_2_G("GSN ""NS %d: comp failed. No comp for N-PDU %d",
          GSN_EXTERN_NSAPI(nsapi), nsapi_ptr->ule.unack_send_npdu);
      }

#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */
    }
  } /* if( nsapi_ptr->data_comp || nsapi_ptr->pci_comp ) */

#endif /* #if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP */

  if( cp_npdu == NULL )
  {
    /* Either compression is not enabled or compression has failed.
    ** In either case, duplicate dsm item for retransmission purposes.
    */

    cp_npdu = gprs_pdu_duplicate( *orig_npdu, 0, GPRS_PDU_LENGTH(*orig_npdu) );

    if( cp_npdu == NULL )
    {

      /* gprs_pdu_duplicate() failed. Drop N-PDU and recover gracefully. */
      if( *orig_npdu )
      {
         GPRS_PDU_FREE( orig_npdu );
      }

      MSG_GERAN_ERROR_2_G("GSN ""NS %d: gprs_pdu_duplicate() failed. N-PDU %d dropped ",
          GSN_EXTERN_NSAPI(nsapi), nsapi_ptr->ule.unack_send_npdu);

      return;
    }
  }

  /* Get SAPI's number */
  sapi = nsapi_ptr->sapi_ptr->sapi;

  /* Initialise segment number. */
  seg_num   = 0;

  /* Segment number is mod 16. Segment number 0 does not always mean first
  ** segment.
  */
  first_seg = TRUE;

  /* Get length of N-PDU */
  len = GPRS_PDU_LENGTH( cp_npdu );

  /* stat_len is used for correcting the N-PDU txed diag statistics in
  ** case the N-PDU is dropped during segmentation.
  */
  stat_len = len;

  /* Update Diag statistics */
  nsapi_ptr->diag_pdu_stat_rsp.npdu_tx_cnt++;
  nsapi_ptr->diag_pdu_stat_rsp.npdu_octet_tx_cnt += len;

  /* Assign N-PDU number */
  sn_udata_hdr.num = nsapi_ptr->ule.unack_send_npdu;

  /* Increment N-PDU Send num, modulo 4096 */
  nsapi_ptr->ule.unack_send_npdu = (nsapi_ptr->ule.unack_send_npdu + 1) %
                                      UNACK_NPDU_NUM_MAX;


  /* New N-PDU from Data Services. */
  if( retx_npdu_item_ptr == NULL )
  {

    /* Store N-PDU just received from Data Services in npdu_buff for
    ** retransmission purposes. This N-PDU must be deleted when it is either
    ** successfully OTA transmitted by RLC or when N-PDU count in npdu_buff
    ** is hit maximum limit and is overwritten by a new N-PDU from Data
    ** Services.
    */

    /* Allocate memory for the PDU item to be queued. */
    npdu_item_ptr = GPRS_MEM_MALLOC( sizeof(gsn_npdu_item_t) );
    GSN_ASSERT( npdu_item_ptr != NULL );

    memset(npdu_item_ptr, 0, sizeof(gsn_npdu_item_t));

    /* Initialise N-PDU item. */
    npdu_item_ptr->unack_send_npdu = sn_udata_hdr.num;

    /* Initialise queue that holds N-PDU segment references. */
    (void) q_init( &npdu_item_ptr->seg_ref_buff );

    /* Set flag to indicate seg_ref_buff as valid for this NPDU */
    npdu_item_ptr->seg_ref_buff_valid = TRUE;

    /* Place a copy of the N-PDU item in N-PDU buffer. */
    npdu_item_ptr->npdu_ptr = *orig_npdu;

    /* Delink pointer */
    *orig_npdu = NULL;

    /* If N-PDU count in npdu_buff is >= GPRS_MAX_RETR_NPDUS, remove the oldest
    ** item at the head of the N-PDU item queue via q_get() and remove any
    ** segments of that N-PDU from segment item reference buffer. Then insert
    ** new N-PDU at the tail of the N-PDU item queue via q_put() thus operating
    ** like a FIFO buffer.
    **
    ** This buffering is for promoting better TCP recovery following a purge of
    ** uplink PDUs following XID RESET. This retransmission mechanism is
    ** therefore limited to the last GPRS_MAX_RETR_NPDUS number of PDUs. This is
    ** not intended as a loss-less retransmission mechanism.
    */

    if( q_cnt( &(nsapi_ptr->ule.npdu_buff)) >= GPRS_MAX_RETR_NPDUS )
    {
      /* Get the oldest N-PDU item at the head of N-PDU item queue. */
      head_npdu_item_ptr = q_get( &(nsapi_ptr->ule.npdu_buff) );

      /* Free any segment references of this N-PDU in segment item reference
      ** buffer.
      */
      if( head_npdu_item_ptr )
      {
        if( head_npdu_item_ptr->seg_ref_buff_valid == TRUE )
        {
          /* Get pointer to segment ref at the head of segment item ref queue. */
          seg_item_ptr = q_check( &(head_npdu_item_ptr->seg_ref_buff) );

          /* Go through queue and remove items and free memory. */
          while( seg_item_ptr )
          {
            /* Get pointer to the next segment in segment item ref queue. */
            next_seg_item_ptr =
              q_next( &head_npdu_item_ptr->seg_ref_buff, &seg_item_ptr->link );

            /* Delete segment of N-PDU from segment item ref queue. */
            q_delete
              (
                  &head_npdu_item_ptr->seg_ref_buff,
                  &seg_item_ptr->link
              );

            /* Free segment item ref. */
            GSN_MEM_FREE( seg_item_ptr );

            /* Load next item. */
            seg_item_ptr = next_seg_item_ptr;
          } /* while (seg_item_ptr) */

          /* Set seg_ref_buff as invalid for this NPDU */
          head_npdu_item_ptr->seg_ref_buff_valid = FALSE;

          /* Free memory, mutex etc. used by NPDU segment queue attached to NPDU.
          */
          q_destroy ( &head_npdu_item_ptr->seg_ref_buff );
        }
        else
        {
          MSG_GERAN_MED_0_G("GSN " "No seg ref buff");
        }

        /* Free N-PDU item back to DSM mem pool. */
        if( head_npdu_item_ptr->npdu_ptr )
          GPRS_PDU_FREE( &head_npdu_item_ptr->npdu_ptr );

        /* Free memory used for queuing the item. */
        GSN_MEM_FREE( head_npdu_item_ptr );
      }
    }

    /* Put new N-PDU item at the tail of the N-PDU item queue. */
    (void) q_link(npdu_item_ptr, &(npdu_item_ptr->link));
    q_put( &(nsapi_ptr->ule.npdu_buff), &(npdu_item_ptr->link) );

  }
  else
  {
    /* Retransmission of an existing N-NPDU from retx buffer (npdu_buff). */
    npdu_item_ptr = retx_npdu_item_ptr;

    /* Initialise N-PDU item. */
    npdu_item_ptr->unack_send_npdu = sn_udata_hdr.num;

    /* Creation of a new segment buffer is required. Ensure it is initialised.
    */
    memset(&npdu_item_ptr->seg_ref_buff, 0, sizeof(q_type));

    /* Initialise queue that holds N-PDU segment references. */
    (void) q_init( &npdu_item_ptr->seg_ref_buff );

    /* Set flag to indicate seg_ref_buff as valid for this NPDU */
    npdu_item_ptr->seg_ref_buff_valid = TRUE;

  }


  /* Keep segmenting until no more bytes left in the N-PDU */
  while( len )
  {
    /*  Get (N201-U minus SN-PDU Unack header length) number of bytes
    **  from the head of the N-PDU to form the SN-PDU
    */
    n201_u  = gsn_n201_u[gas_id][nsapi_ptr->sapi_ptr->sapi];

    /* Determine the number of bytes in the SN-PDU header */
    if( first_seg )
      hdr_len = UNACK_PDU_HDR_1ST_SEG;
    else
      hdr_len = UNACK_PDU_HDR_NOT_1ST_SEG;

    /* Allocate memory to convey the message to LLC */
    ll_msg = GPRS_MEM_MALLOC( sizeof( gprs_ll_ul_dat_mes_t ) );
    GSN_ASSERT ( ll_msg != NULL );

    /* Avoid long lines of code */
    udata_msg = &ll_msg->mes.unitdata_req_mes;

    /* Segment N-PDU. Length of segment must be less than N201_U
    ** after the LL_UNITDATA header has been added
    */
    udata_msg->l3_pdu = gprs_pdu_segment_head( &cp_npdu,
                                     (uint16) (n201_u - hdr_len) );

    if( udata_msg->l3_pdu == NULL )
    {
      /* gprs_pdu_segment_head() failed. Original N-PDU which is already in
      ** npdu_buf will be overwritten. Free copy/duplicate of original N-PDU
      ** here.
      */
      if( cp_npdu != NULL )
      {
        GPRS_PDU_FREE( &cp_npdu );
      }

      /* Free mem allocated to convay LL-PDU to LLC */
      GSN_MEM_FREE( ll_msg );

      /* Correct the N-PDU txed diag statistics */
      nsapi_ptr->diag_pdu_stat_rsp.npdu_tx_cnt--;
      nsapi_ptr->diag_pdu_stat_rsp.npdu_octet_tx_cnt -= stat_len;

      /* Correct N-PDU Send number, modulo 4096 */
      nsapi_ptr->ule.unack_send_npdu = nsapi_ptr->ule.unack_send_npdu ?
        (nsapi_ptr->ule.unack_send_npdu - 1) : (UNACK_NPDU_NUM_MAX - 1);

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP
      if( nsapi_ptr->pci_comp )
      {
        /* Inform PCI compressor/decompressor about the dropped frame */
        gsn_unack_npdu_dropped( gas_id, nsapi_ptr );
      }
#endif

      MSG_GERAN_ERROR_2_G("GSN ""NS %d: gprs_pdu_segment_head() failed. N-PDU %d dropped",
        GSN_EXTERN_NSAPI(nsapi),nsapi_ptr->ule.unack_send_npdu);

      return;
    }

    /* Update Diag statistics */
    nsapi_ptr->diag_pdu_stat_rsp.snpdu_tx_cnt++;
    if( len > (n201_u - hdr_len) )
      nsapi_ptr->diag_pdu_stat_rsp.snpdu_octet_tx_cnt += n201_u;
    else
      nsapi_ptr->diag_pdu_stat_rsp.snpdu_octet_tx_cnt += len + hdr_len;

    /* Calculate length of the N-PDU left after segmentation */
    len = (len > (n201_u - hdr_len)) ? (len - (n201_u - hdr_len)) : 0;

    /* Determine segment order */
    if( first_seg && len )
    {
      /* First segment but more to follow */
      sn_udata_hdr.f = SN_FIRST_SEG;
      sn_udata_hdr.m = SN_MORE_SEG;
      first_seg      = FALSE;
    }
    else if( first_seg  && !len )
    {
      /* only one segment */
      sn_udata_hdr.f = SN_FIRST_SEG;
      sn_udata_hdr.m = SN_LAST_SEG;
    }
    else if( !first_seg  && len )
    {
      /* Not the First segment but more to follow */
      sn_udata_hdr.f = SN_NOT_FIRST_SEG;
      sn_udata_hdr.m = SN_MORE_SEG;
    }
    else if( !first_seg  && !len  )
    {
      /* Last segment */
      sn_udata_hdr.f = SN_NOT_FIRST_SEG;
      sn_udata_hdr.m = SN_LAST_SEG;
    }

    /* Set the rest of the fields on the UnAck SN-PDU header
    */
    sn_udata_hdr.seg = seg_num;

    /* Shall be set to zero */
    sn_udata_hdr.x = 0;

    /* SN-UNITDATA PDU */
    sn_udata_hdr.t = 1;

    /* Translate SNDCP NSAPI number to external NSAPI number */
    sn_udata_hdr.nsapi = (uint8) GSN_EXTERN_NSAPI( nsapi );

    /* Encode header for transmission */
    gsn_encode_unitdata_pdu_hdr( gas_id, &sn_udata_hdr, ext_hdr);

    /* Append header to the N-PDU segment */
    if( !gprs_pdu_append_head( &udata_msg->l3_pdu, ext_hdr, (uint16)hdr_len) )
    {
      /* gprs_pdu_append_head() failed. Original N-PDU which is already in
      ** npdu_buf will be overwritten. Free copy/duplicate of original N-PDU
      ** here.
      */
      /* Free dsm item conveying the N-PDU */
      if( cp_npdu != NULL )
      {
        GPRS_PDU_FREE( &cp_npdu );
      }
      /* Free mem allocated to convey LL-PDU to LLC */
      GSN_MEM_FREE( ll_msg );

      /* Correct the N-PDU txed diag statistics */
      nsapi_ptr->diag_pdu_stat_rsp.npdu_tx_cnt--;
      nsapi_ptr->diag_pdu_stat_rsp.npdu_octet_tx_cnt -= stat_len;

      /* Correct N-PDU Send number, modulo 4096 */
      nsapi_ptr->ule.unack_send_npdu = nsapi_ptr->ule.unack_send_npdu ?
        (nsapi_ptr->ule.unack_send_npdu - 1) : (UNACK_NPDU_NUM_MAX - 1);

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP
      if( nsapi_ptr->pci_comp )
      {
        /* Inform PCI compressor/decompressor about the dropped frame */
        gsn_unack_npdu_dropped( gas_id, nsapi_ptr );
      }
#endif

      MSG_GERAN_ERROR_2_G("GSN ""NS %d: gprs_pdu_append_head() failed. N-PDU %d dropped",
        GSN_EXTERN_NSAPI(nsapi),nsapi_ptr->ule.unack_send_npdu);

      return;
    }


    /* Add segment reference to the segment item buffer */
    seg_item_ptr = GPRS_MEM_MALLOC( sizeof(gsn_seg_ref_item_t) );
    GSN_ASSERT( seg_item_ptr != NULL );

    seg_item_ptr->npdu_num = sn_udata_hdr.num;
    seg_item_ptr->seg_num  = seg_num;

    (void) q_link( seg_item_ptr, &(seg_item_ptr->link) );
    q_put( &npdu_item_ptr->seg_ref_buff, &(seg_item_ptr->link) );

    /*  Load LL_UNITDATA_REQ primitive. l3_pdu is already assigned.*/
    udata_msg->sndcp_ref.nsapi    = (uint8) GSN_EXTERN_NSAPI(nsapi);
    udata_msg->sndcp_ref.npdu_num = sn_udata_hdr.num;
    udata_msg->sndcp_ref.seg_num  = sn_udata_hdr.seg;

    ll_msg->sapi                  = gsn_extern_sapi[sapi];
    udata_msg->prim               = LL_UNITDATA_REQ;
    udata_msg->peak_throughput    = nsapi_ptr->qos.peak_throughput;
    udata_msg->reliability_class  = nsapi_ptr->qos.reliability_class;
    udata_msg->radio_priority     = nsapi_ptr->radio_pr;
    udata_msg->tlli               = nsapi_ptr->sapi_ptr->tlli;
    udata_msg->ciphered           = nsapi_ptr->cipher;
    udata_msg->pfi                = nsapi_ptr->pfi;
    udata_msg->gas_id             = gas_id;
    udata_msg->pdu_priority       = pdu_priority;


    switch( nsapi_ptr->sapi_ptr->sapi )
    {
    case GSN_SAPI_3:
      MSG_GERAN_HIGH_5_G
      (
        "GSN LL 3: Sent LL_UDATA_REQ %d:%d:%d:%d:%d",
        sn_udata_hdr.nsapi, sn_udata_hdr.num, sn_udata_hdr.seg,
        nsapi_ptr->qos.peak_throughput, nsapi_ptr->qos.reliability_class 
      );
      break;

    case GSN_SAPI_5:
      MSG_GERAN_HIGH_5_G
      (
        "GSN LL 5: Sent LL_UDATA_REQ %d:%d:%d:%d:%d",
        sn_udata_hdr.nsapi, sn_udata_hdr.num, sn_udata_hdr.seg,
        nsapi_ptr->qos.peak_throughput, nsapi_ptr->qos.reliability_class
      );
      break;

    case GSN_SAPI_9:
      MSG_GERAN_HIGH_5_G
      (
        "GSN LL 9: Sent LL_UDATA_REQ %d:%d:%d:%d:%d",
        sn_udata_hdr.nsapi, sn_udata_hdr.num, sn_udata_hdr.seg,
        nsapi_ptr->qos.peak_throughput, nsapi_ptr->qos.reliability_class
      );
      break;

    case GSN_SAPI_11:
      MSG_GERAN_HIGH_5_G
      (
        "GSN LL 11: Sent LL_UDATA_REQ %d:%d:%d:%d:%d",
        sn_udata_hdr.nsapi, sn_udata_hdr.num, sn_udata_hdr.seg,
        nsapi_ptr->qos.peak_throughput, nsapi_ptr->qos.reliability_class
      );
      break;

    default:
      break;
    }

    /* Send Primitive to LLC */
    gllc_ll_ul_dat_put_mes_buf( gas_id, ll_msg );

    /* increment segment number modulo 16 */
    seg_num = (seg_num + 1) % UNACK_SEG_NUM_MAX;

  } /* while( len ) */

}

/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_LL_DATA_IND()
===
===  DESCRIPTION
===
===   Processes the LL_DATA indication primitive used by the LLC
===   layer to deliver the successfully received SN-PDU to the SNDCP layer.
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
void gsn_process_ll_data_ind
(
  gas_id_t               gas_id,
  const gsn_sapi_e_t    *sapi_ptr,    /* SAPI entity that received the indication */
  ll_data_ind_t         *ll_mes_ptr   /* Points to the ll_data_ind primitive */
)
{
  gsn_data_pdu_hdr_t  sn_hdr;
  gsn_sapi_t          n_sapi;
  uint8               hdr_pkd[ACK_PDU_HDR_1ST_SEG]; /* max SN-PDU hedear in bytes */
  gsn_sapi_t          sapi = sapi_ptr->sapi;
  uint8               temp_nsapi;
  uint8               temp_npdu_num;

  /* Strip one byte of the header of the ACK SN-PDU and check if it
  ** is the first segment of the N-PDU. If so, the header
  ** length is 3 bytes, otherwise 1 byte long.
  */
  if( gprs_pdu_strip_head( &ll_mes_ptr->l3_pdu, hdr_pkd, ONE_BYTE) )
  {
    if( hdr_pkd[0] & (0x1 << F_BIT_POS) )
    {
      /* Get the rest of the header (2 bytes more ) */
      if( !gprs_pdu_strip_head( &ll_mes_ptr->l3_pdu,
                  &hdr_pkd[1], ACK_PDU_HDR_1ST_SEG - ONE_BYTE) )
      {
        if( ll_mes_ptr->l3_pdu != NULL )
            /* Release pdu */
            GPRS_PDU_FREE( &ll_mes_ptr->l3_pdu );

        MSG_GERAN_HIGH_1_G("GSN ""LL %d: Invalid pdu length in LL_DATA_IND",
                      gsn_extern_sapi[sapi]);

        return;
      }
    }

    /* Get SAPI, NSAPI, N-PDU number and determine if it's
    ** the first or last segment for debug info.
    */
    temp_nsapi    = (uint8) hdr_pkd[0] & 0x0f;

    /* Check if NSAPI is valid */
    if(!GSN_VALID_NSAPI(temp_nsapi))
    {
      MSG_GERAN_HIGH_2_G("GSN ""LL %d: Invalid NSAPI %d in DL sndcp hdr",
                    gsn_extern_sapi[sapi], temp_nsapi);
      /* Release pdu with invalid hdr */
      if( ll_mes_ptr->l3_pdu != NULL )
      {
        GPRS_PDU_FREE( &ll_mes_ptr->l3_pdu );
      }

      return;
    }

    /* temp_nsapi >= 5 is already checked by GSN_VALID_NSAPI above but
       Klocwork disagrees */
    if((temp_nsapi >= 5) &&
     (actv_nsapis[ gas_id ][GSN_INTERN_NSAPI(temp_nsapi)] != NULL))
    {
      if( hdr_pkd[0] & (0x1 << F_BIT_POS) )
      {
        temp_npdu_num = (uint8) hdr_pkd[2];

        MSG_GERAN_MED_3_G("GSN " "LL %d: Rcvd LL_DATA_IND %d:%d:F",
                    gsn_extern_sapi[sapi], temp_nsapi, temp_npdu_num );
        /* avoid compiler warnings about unused variables when MSG
           is defined away to nothing */
        (void)temp_npdu_num;
      }
      else
      {
        if( hdr_pkd[0] & (0x1 << M_BIT_POS) )
        {
          MSG_GERAN_MED_3_G("GSN " "LL %d: Rcvd LL_DATA_IND %d:%d:M",
          gsn_extern_sapi[sapi], temp_nsapi,
          actv_nsapis[ gas_id ][GSN_INTERN_NSAPI(temp_nsapi)]->dle.first_seg_info.ack_npdu_num );
        }
        else
        {
          MSG_GERAN_MED_3_G("GSN " "LL %d: Rcvd LL_DATA_IND %d:%d:L",
          gsn_extern_sapi[sapi], temp_nsapi,
          actv_nsapis[ gas_id ][GSN_INTERN_NSAPI(temp_nsapi)]->dle.first_seg_info.ack_npdu_num );
        }
      }
    }
    else
    {
      MSG_GERAN_HIGH_2_G("GSN ""LL %d: Inactive NSAPI %d for LL_DATA_IND",
        gsn_extern_sapi[sapi], temp_nsapi);
    }

    if( gsn_decode_data_pdu_hdr( gas_id, hdr_pkd, &sn_hdr) )
    {
      /* Check if NSAPI is active and mapped to the SAPI. In case of modified NSAPI
      ** We might receiving some LL_DATA.IND, LL_DATA.CNF, LL_UNITDATA.IND
      ** on the old SAPI. In this case the above primitives should be ignored
      */
      if( actv_nsapis[ gas_id ][sn_hdr.nsapi] != NULL )
      {
        /* Update Diag statistics */
        actv_nsapis[ gas_id ][sn_hdr.nsapi]->diag_pdu_stat_rsp.snpdu_rx_cnt++;
        if( sn_hdr.f == SN_FIRST_SEG )
          actv_nsapis[ gas_id ][sn_hdr.nsapi]->diag_pdu_stat_rsp.snpdu_octet_rx_cnt +=
                   GPRS_PDU_LENGTH( ll_mes_ptr->l3_pdu ) + ACK_PDU_HDR_1ST_SEG;
        else
          actv_nsapis[ gas_id ][sn_hdr.nsapi]->diag_pdu_stat_rsp.snpdu_octet_rx_cnt +=
                   GPRS_PDU_LENGTH( ll_mes_ptr->l3_pdu ) + ACK_PDU_HDR_NOT_1ST_SEG;

        n_sapi = actv_nsapis[ gas_id ][sn_hdr.nsapi]->sapi_ptr->sapi;

        if( n_sapi == sapi )
        {
          /* Reassemble acknowledged data */
          if( gsn_dle_reas_ack_data( gas_id, (gsn_nsapi_t) sn_hdr.nsapi,
                                      &ll_mes_ptr->l3_pdu, &sn_hdr ) )
          {
            /* Check if N-PDU should be forwarded to PDP */
            switch( actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.state )
            {
              case DL_NORMAL:

                if( GMODULO_LT
                         (
                            ACK_NPDU_NUM_MAX,
                            actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.first_seg_info.ack_npdu_num,
                            actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.ack_rcv_npdu
                         )
                  )
                {
                  /* NSAPI has received a duplicated N-PDU */
                  MSG_GERAN_MED_2_G("GSN ""LL %d: Rcvd duplicated N-PDU %d",
                  gsn_extern_sapi[sapi],
                  actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.first_seg_info.ack_npdu_num);

                  GPRS_PDU_FREE( &actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.reas_buff );

                  actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.reas_buff = NULL;
                }
                else
                {

                  /* Increment receive N-PDU number */
                  actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.ack_rcv_npdu += 1;

                  /* Send N-PDU to PDP */
                  MSG_GERAN_MED_2_G("GSN " "NS %d: Sent to PDP SN_DATA_IND NPDU %d ",
                                GSN_EXTERN_NSAPI(sn_hdr.nsapi),
                                actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.ack_rcv_npdu);

                  gsn_process_pdp_dl_data( gas_id, (gsn_nsapi_t) sn_hdr.nsapi,
                                        &actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.reas_buff );
                }

                break;

              case DL_RECOVERY:

                if( actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.first_seg_info.ack_npdu_num ==
                      actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.ack_rcv_npdu )
                {
                  /* Increment receive N-PDU number, modulo 256 */
                  actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.ack_rcv_npdu += 1;

                  /* The first DL N-PDU has been received */
                  if( actv_nsapis[ gas_id ][sn_hdr.nsapi]->first_dl_npdu == TRUE )
                    actv_nsapis[ gas_id ][sn_hdr.nsapi]->first_dl_npdu = FALSE;

                  /* Back to normal operation */
                  actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.state = DL_NORMAL;

                  /* Update diag */
                  gsn_encode_state_diag_rpt( gas_id, (uint8) sn_hdr.nsapi );

                  /* Send N-PDU to PDP */
                  MSG_GERAN_MED_2_G("GSN " "NS %d: Sent to PDP SN_DATA_IND NPDU %d ",
                                GSN_EXTERN_NSAPI(sn_hdr.nsapi),
                                actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.ack_rcv_npdu);

                  gsn_process_pdp_dl_data( gas_id, (gsn_nsapi_t) sn_hdr.nsapi,
                                        &actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.reas_buff );
                }
                else if( GMODULO_LT
                         (
                            ACK_NPDU_NUM_MAX,
                            actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.first_seg_info.ack_npdu_num,
                            actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.ack_rcv_npdu
                         )
                       )
                {
                  /* Agilent 8960 tester starts the transmission of
                  ** N-PDUs with the first N-PDU number != 0.
                  */
                  if( actv_nsapis[ gas_id ][sn_hdr.nsapi]->first_dl_npdu == TRUE )
                  {
                    actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.ack_rcv_npdu =
                    actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.first_seg_info.ack_npdu_num + 1;

                    /* The first DL N-PDU has been received */
                    actv_nsapis[ gas_id ][sn_hdr.nsapi]->first_dl_npdu = FALSE;

                    /* Back to normal operation */
                    actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.state = DL_NORMAL;

                    /* Update diag */
                    gsn_encode_state_diag_rpt( gas_id, (uint8) sn_hdr.nsapi );

                    /* Send N-PDU to PDP */
                    MSG_GERAN_MED_2_G("GSN " "NS %d: Sent to PDP SN_DATA_IND NPDU %d",
                                  GSN_EXTERN_NSAPI(sn_hdr.nsapi),
                                  actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.ack_rcv_npdu);

                    gsn_process_pdp_dl_data( gas_id, (gsn_nsapi_t) sn_hdr.nsapi,
                                          &actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.reas_buff );

                  }
                  else
                  {
                    /* NSAPI has received a duplicated N-PDU */
                    MSG_GERAN_MED_2_G("GSN ""LL %d: Rcvd duplicated N-PDU %d",
                    gsn_extern_sapi[sapi],
                    actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.first_seg_info.ack_npdu_num);

                    GPRS_PDU_FREE( &actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.reas_buff );

                    actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.reas_buff = NULL;
                  }
                }
                else
                {
                  /* NPDU number out of sequence. Some NPDU(s) has/have been lost.
                  ** Set receive N-PDU number equal to the NPDU number received plus one.
                  ** NOTE: This exceptional case is not handled in TS 04.65.
                  */
                  MSG_GERAN_HIGH_2_G("GSN " "NS %d: N_PDU number %d out of sequence",
                    GSN_EXTERN_NSAPI(sn_hdr.nsapi),
                    actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.ack_rcv_npdu);

                  actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.ack_rcv_npdu =
                  actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.first_seg_info.ack_npdu_num + 1;

                  /* The first DL N-PDU has been received */
                  if( actv_nsapis[ gas_id ][sn_hdr.nsapi]->first_dl_npdu == TRUE )
                    actv_nsapis[ gas_id ][sn_hdr.nsapi]->first_dl_npdu = FALSE;

                  /* Back to normal operation */
                  actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.state = DL_NORMAL;

                  /* Update diag */
                  gsn_encode_state_diag_rpt( gas_id, (uint8) sn_hdr.nsapi );

                  /* Send N-PDU to PDP */
                  MSG_GERAN_MED_2_G("GSN " "NS %d: Sent to PDP SN_DATA_IND NPDU %d",
                                GSN_EXTERN_NSAPI(sn_hdr.nsapi),
                                actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.ack_rcv_npdu);

                  gsn_process_pdp_dl_data( gas_id, (gsn_nsapi_t) sn_hdr.nsapi,
                                        &actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.reas_buff );
                }

                break;

              default:

                MSG_GERAN_ERROR_2_G("GSN ""NS %d: Invalid sate %d of DL Entity ",
                  actv_nsapis[ gas_id ][sn_hdr.nsapi]->dle.state,
                    GSN_EXTERN_NSAPI(sn_hdr.nsapi));
                break;

            } /* switch( actv_nsapis[ gas_id ][sn_hdr.nsapi].nsapi_e.dle.state ) */

          } /* if( gsn_dle_reas_ack_data... */
        }
        else
        {
          MSG_GERAN_HIGH_2_G("GSN ""NSAPI %d is not mapped to SAPI %d ",
             GSN_EXTERN_NSAPI(sn_hdr.nsapi), gsn_extern_sapi[n_sapi]);
        }
      }
      else
      {
        MSG_GERAN_HIGH_2_G("GSN ""LL %d: Inactive NSAPI %d for LL_DATA_IND",
          gsn_extern_sapi[sapi], GSN_EXTERN_NSAPI(sn_hdr.nsapi));
      }
    }
    else
    {
      /* invalid header ignore PDU without error notification */
      MSG_GERAN_HIGH_1_G("GSN ""LL %d: Invalid header of DL LL_DATA PDU ",
                      gsn_extern_sapi[sapi]);
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G("GSN ""LL %d: Invalid pdu length in LL_DATA_IND",
                      gsn_extern_sapi[sapi]);
  }

  if( ll_mes_ptr->l3_pdu != NULL )
    /* Release pdu */
    GPRS_PDU_FREE( &ll_mes_ptr->l3_pdu );
}

/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_LL_UDATA_IND()
===
===  DESCRIPTION
===
===   Processes the LL_UNITDATA indication primitive used by the LLC
===   layer to deliver the successfully received SN-PDU to the SNDCP layer.
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
void gsn_process_ll_udata_ind
(
  gas_id_t                 gas_id,
  const gsn_sapi_e_t      *sapi_ptr,    /* SAPI entity that received the indication */
  ll_unitdata_ind_t       *ll_mes_ptr   /* Points to the ll_unitdata_ind primitive */
)
{
  gsn_unitdata_pdu_hdr_t  sn_hdr;
  gsn_sapi_t              n_sapi;
  uint8                   hdr_pkd[UNACK_PDU_HDR_1ST_SEG]; /* max SN-PDU hedear in bytes */
  gsn_sapi_t              sapi = sapi_ptr->sapi;


  gsn_npdu_ref_item_t     *npdu_ref_item_ptr;
  gsn_seg_item_t          *seg_item_ptr;


  /* An active SAPI accepts UnAck data in any sate. Decode the
  ** SN-PDU header fields. Strip one byte of the header of the
  ** UNACK SN-PDU and check if it is the first segment ( F = 1) of the N-PDU.
  ** If so, the header length is 4 bytes long, otherwise 3 byte long.
  */
  if( gprs_pdu_strip_head( &ll_mes_ptr->l3_pdu,
                hdr_pkd, UNACK_PDU_HDR_NOT_1ST_SEG ) )
  {

    if( hdr_pkd[0] & (0x1 << F_BIT_POS ) )
    {
      /* Get the rest of the header */
      if( !gprs_pdu_strip_head( &ll_mes_ptr->l3_pdu,
                  &hdr_pkd[3], ONE_BYTE) )
      {
        if( ll_mes_ptr->l3_pdu != NULL )
          /* Release pdu */
          GPRS_PDU_FREE( &ll_mes_ptr->l3_pdu );

        MSG_GERAN_HIGH_1_G("GSN ""LL %d: Invalid pdu length in LL_UDATA_IND",
                          gsn_extern_sapi[sapi]);

        return;
      }
    }

    if( gsn_decode_unitdata_pdu_hdr( gas_id, hdr_pkd, &sn_hdr) )
    {
      /* Check if NSAPI is active and mapped to the SAPI. In case of modified NSAPI
      ** We might receiving some LL_DATA.IND, LL_DATA.CNF, LL_UNITDATA.IND
      ** on the old SAPI. In this case the above primitives should be ignored
      */
      if( actv_nsapis[ gas_id ][sn_hdr.nsapi] != NULL )
      {
        /* Update Diag statistics */
        actv_nsapis[ gas_id ][sn_hdr.nsapi]->diag_pdu_stat_rsp.snpdu_rx_cnt++;
        if( sn_hdr.f == SN_FIRST_SEG )
          actv_nsapis[ gas_id ][sn_hdr.nsapi]->diag_pdu_stat_rsp.snpdu_octet_rx_cnt +=
                 GPRS_PDU_LENGTH( ll_mes_ptr->l3_pdu ) + UNACK_PDU_HDR_1ST_SEG;
        else
          actv_nsapis[ gas_id ][sn_hdr.nsapi]->diag_pdu_stat_rsp.snpdu_octet_rx_cnt +=
                 GPRS_PDU_LENGTH( ll_mes_ptr->l3_pdu ) + UNACK_PDU_HDR_NOT_1ST_SEG;

        n_sapi = actv_nsapis[ gas_id ][sn_hdr.nsapi]->sapi_ptr->sapi;

        switch( sapi )
        {
        case GSN_SAPI_3:
          MSG_GERAN_MED_3_G("GSN "" LL 3: Rcvd LL_UDATA_IND %d:%d:%d",
                        GSN_EXTERN_NSAPI(sn_hdr.nsapi), sn_hdr.num, sn_hdr.seg );
          break;

        case GSN_SAPI_5:
          MSG_GERAN_MED_3_G("GSN ""LL 5: Rcvd LL_UDATA_IND %d:%d:%d",
                        GSN_EXTERN_NSAPI(sn_hdr.nsapi), sn_hdr.num, sn_hdr.seg );
          break;

        case GSN_SAPI_9:
          MSG_GERAN_MED_3_G("GSN "" LL 9: Rcvd LL_UDATA_IND %d:%d:%d",
                        GSN_EXTERN_NSAPI(sn_hdr.nsapi), sn_hdr.num, sn_hdr.seg );
          break;

        case GSN_SAPI_11:
          MSG_GERAN_MED_3_G("GSN "" LL 11: Rcvd LL_UDATA_IND %d:%d:%d",
                        GSN_EXTERN_NSAPI(sn_hdr.nsapi), sn_hdr.num, sn_hdr.seg );
          break;

        default:
          break;
        }

        if( n_sapi == sapi )
        {


          /* Re-order segments. Returning pointer is non-NULL when a fully
          ** re-ordered NPDU is ready.
          */
          npdu_ref_item_ptr = gsn_dle_reorder_npdu_seg( gas_id, &ll_mes_ptr->l3_pdu, &sn_hdr );

          /* When all segments of a NPDU is received and re-ordered, perform
          ** reassembly operation.
          */
          if( npdu_ref_item_ptr != NULL )
          {

            seg_item_ptr = q_get( &(npdu_ref_item_ptr->seg_reorder_buff ) );

            while( seg_item_ptr != NULL )
            {

              /* Reassemble unacknowledged data */
              if( gsn_dle_reas_unack_data(  gas_id,
                                            (gsn_nsapi_t) seg_item_ptr->sn_hdr.nsapi,
                                            &seg_item_ptr->seg_ptr,
                                            &seg_item_ptr->sn_hdr ))
              {
                /* Send N-PDU to PDP */
                MSG_GERAN_MED_3_G("GSN " "NS %d: Sent to PDP SN_UNITDATA_IND NPDU %d",
                              GSN_EXTERN_NSAPI(seg_item_ptr->sn_hdr.nsapi),
                              seg_item_ptr->sn_hdr.num,
                              0
                           );

                /* Check for lost or out of order DL SN-PDUs and inform the decompessor */
                if(
                    seg_item_ptr->sn_hdr.num !=
                    actv_nsapis[ gas_id ][seg_item_ptr->sn_hdr.nsapi]->dle.unack_rcv_npdu
                  )
                {
                  actv_nsapis[ gas_id ][seg_item_ptr->sn_hdr.nsapi]->dle.unack_rcv_npdu
                    = seg_item_ptr->sn_hdr.num;

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP
                  if(
                      actv_nsapis[ gas_id ][seg_item_ptr->sn_hdr.nsapi]->pci_comp &&
                      seg_item_ptr->sn_hdr.pcomp
                    )
                  {
                    /* Inform PCI compressor/decompressor about the dropped frame */
                    gsn_unack_npdu_dropped(
                      gas_id, actv_nsapis[ gas_id ][seg_item_ptr->sn_hdr.nsapi] );
                  }
#endif
                }

                /* Set Receive N-PDU number ready for the next N-PDU */
                actv_nsapis[ gas_id ][seg_item_ptr->sn_hdr.nsapi]->dle.unack_rcv_npdu =
                  ((actv_nsapis[ gas_id ][seg_item_ptr->sn_hdr.nsapi]->dle.unack_rcv_npdu + 1) %
                   UNACK_NPDU_NUM_MAX);

                gsn_process_pdp_dl_data
                (
                  gas_id, (gsn_nsapi_t) seg_item_ptr->sn_hdr.nsapi,
                  &actv_nsapis[ gas_id ][seg_item_ptr->sn_hdr.nsapi]->dle.reas_buff
                );

                if( seg_item_ptr->seg_ptr != NULL )
                  /* Release pdu */
                  GPRS_PDU_FREE( &seg_item_ptr->seg_ptr );
              }

              /* Free this segment */
              GPRS_PDU_FREE( &seg_item_ptr->seg_ptr );

              /* Free memory used by this segment. */
              GSN_MEM_FREE(seg_item_ptr);

              /* Get next segment from segment re-order queue. */
              seg_item_ptr = q_get( &(npdu_ref_item_ptr->seg_reorder_buff ) );
            }

            /* now destroy segment queue */
            q_destroy ( &npdu_ref_item_ptr->seg_reorder_buff );

            /* Free memory used by NPDU reference item. */
            GSN_MEM_FREE(npdu_ref_item_ptr);
          }


        }
        else
        {
          MSG_GERAN_HIGH_2_G("GSN ""NSAPI %d is not mapped to SAPI %d ",
            GSN_EXTERN_NSAPI(sn_hdr.nsapi), gsn_extern_sapi[sapi]);

          if( ll_mes_ptr->l3_pdu != NULL )
            /* Release pdu */
            GPRS_PDU_FREE( &ll_mes_ptr->l3_pdu );
        }
      }
      else
      {
        MSG_GERAN_HIGH_2_G("GSN ""LL %d: Inactive NSAPI %d for LL_UDATA_IND",
                    gsn_extern_sapi[sapi], GSN_EXTERN_NSAPI(sn_hdr.nsapi));

        if( ll_mes_ptr->l3_pdu != NULL )
          /* Release pdu */
          GPRS_PDU_FREE( &ll_mes_ptr->l3_pdu );
      }
    }
    else
    {
      /* invalid header ignore PDU without error notification */
      MSG_GERAN_HIGH_1_G("GSN ""LL %d: Invalid header of DL SN-UNITDATA PDU  ",
                          gsn_extern_sapi[sapi]);
      if( ll_mes_ptr->l3_pdu != NULL )
        /* Release pdu */
        GPRS_PDU_FREE( &ll_mes_ptr->l3_pdu );
    }
  }
  else
  {
    MSG_GERAN_HIGH_1_G("GSN ""LL %d: Invalid pdu length in LL_UDATA_IND",
                          gsn_extern_sapi[sapi]);




  if( ll_mes_ptr->l3_pdu != NULL )
    /* Release pdu */
    GPRS_PDU_FREE( &ll_mes_ptr->l3_pdu );
  }

}

/*===========================================================================
===
===  FUNCTION      GSN_SEND_LL_XID_REQ()
===
===  DESCRIPTION
===
===   Sends LL_XID_REQ to LLC layer. LLC_XID_REQ primitive conveys an XID block
===   to be send to the peer SNDCP layer to start the XID negotiation procedure.
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
void gsn_send_ll_xid_req( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr, dsm_item_type  *xid_req )
{
  gsn_nsapi_q_t         *nsapi_q_ptr;
  gsn_sapi_t            sapi = sapi_ptr->sapi;
  gprs_ll_ul_sig_mes_t  *ul_mes;

  /* Switch on SAPI's state */
  switch( sapi_ptr->state )
  {
/*-------------------------------------------------------------------------------*/
  case ( IDLE ):
  case ( ESTABLISHED):

    /* Print XID parameters in F3 message */
    gsn_print_xid_params ( gas_id, xid_req, TRUE );

    /* Get buffer to convey the message */
    ul_mes = gllc_ll_ul_sig_get_mes_buf( );
    GSN_ASSERT( ul_mes != NULL );

    /* load LLC message */
    ul_mes->sapi = gsn_extern_sapi[sapi];
    ul_mes->mes.xid_req_mes.prim        = LL_XID_REQ;
    ul_mes->mes.xid_req_mes.l3_xid_req  = xid_req;
    ul_mes->mes.xid_req_mes.tlli        = sapi_ptr->tlli;
    ul_mes->mes.xid_req_mes.gas_id      = gas_id;

    if( sapi_ptr->is_nsapi_ptr_q_valid )
    {

      nsapi_q_ptr = q_check( &sapi_ptr->nsapi_ptr_q );

      if ( nsapi_q_ptr != NULL )
      {
        ul_mes->mes.xid_req_mes.peak_throughput =
          nsapi_q_ptr->nsapi_ptr->qos.peak_throughput;
        ul_mes->mes.xid_req_mes.radio_priority  =
          nsapi_q_ptr->nsapi_ptr->radio_pr;
        ul_mes->mes.xid_req_mes.pfi  =
          nsapi_q_ptr->nsapi_ptr->pfi;
      }
      else
      {
        ul_mes->mes.xid_req_mes.peak_throughput = 0;
        ul_mes->mes.xid_req_mes.radio_priority  = 1;
        ul_mes->mes.xid_req_mes.pfi           = 0;
      }
    }
    else
    {

      ul_mes->mes.xid_req_mes.peak_throughput = 0;
      ul_mes->mes.xid_req_mes.radio_priority  = 1;
      ul_mes->mes.xid_req_mes.pfi           = 0;
      MSG_GERAN_ERROR_2_G("GSN ""LL %d: NSAPI ptr queue invalid in state: %d",
                    gsn_extern_sapi[sapi],sapi_ptr->state);
    }

    if( ul_mes->mes.xid_req_mes.l3_xid_req != NULL )
    {
      /* Set SAPI XID state */
      sapi_ptr->xid_state = XID_CNF_PEND;

      /* Suspend data flow until LL_XID_CNF or collision resolution */
      gsn_ul_suspend_dataflow( gas_id, sapi, GSN_UL_SUSP_XID_PEND );
    }

    /* Post message to LLC and log for call flow analysis. */
    MSG_GERAN_MED_1_G("GSN ""LL %d: Sent LL_XID_REQ",gsn_extern_sapi[sapi]);

    cfa_log_packet_ex
    (
      GS_QUEUE_LLC, MS_LLC_SNDCP, (uint8) LL_XID_REQ,
      sizeof( ll_xid_req_t ), (uint8 *)&ul_mes -> mes.xid_req_mes
    );

    gllc_ll_ul_sig_put_mes_buf( gas_id, ul_mes );
    break;

/*-------------------------------------------------------------------------------*/
  default:

    MSG_GERAN_ERROR_2_G("GSN ""LL %d: Rcvd XID_REQ in state: %d",gsn_extern_sapi[sapi],sapi_ptr->state);
    break;

/*-------------------------------------------------------------------------------*/
  } /* switch switch( sapi_ptr->state ) */
}
/*===========================================================================
===
===  FUNCTION      GSN_SEND_SNSM_STAT_REQ()
===
===  DESCRIPTION
===
===   Sends the SNSM_STATUS request primitive to inform SM that SNDCP cannot
===   continue its operation due to errors at the LLC or at the SNDCP. The Cause
===   parameter indicates the cause of the error.
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
void gsn_send_snsm_stat_req( gas_id_t gas_id, gsn_sapi_t sapi, uint32 tlli, uint8 cause)
{
  sm_cmd_type  *snsm_dl_mes;
#ifdef FEATURE_DUAL_SIM
  gsn_sapi_e_t  *sapi_ptr = NULL;
#endif /* FEATURE_DUAL_SIM */

  /* Get buffer to convey the message */
  snsm_dl_mes = sm_get_cmd_buf();
  GSN_ASSERT( snsm_dl_mes != NULL );

  /* Load message */
  snsm_dl_mes->header.message_set           = MS_SNDCP_SM;
  snsm_dl_mes->header.cmd_id                = SNSM_STATUS_REQ;
  snsm_dl_mes->cmd.sndcp_status_req.tlli    = tlli;
  snsm_dl_mes->cmd.sndcp_status_req.sapi    = gsn_extern_sapi[sapi];
  snsm_dl_mes->cmd.sndcp_status_req.cause   = (snsm_cause_T) cause;

#ifdef FEATURE_DUAL_SIM
  sapi_ptr = actv_sapis[gas_id][sapi];

  if ( sapi_ptr )
  {
    gsn_nsapi_q_t *item_ptr = NULL;
    gsn_nsapi_e_t  *nsapi_ptr = NULL;

    item_ptr = (gsn_nsapi_q_t*)q_check( &(sapi_ptr->nsapi_ptr_q) );

    if( item_ptr )
    {
      while( item_ptr )
      {
        if( item_ptr->nsapi_ptr )
        {
          nsapi_ptr = actv_nsapis[ gas_id ][ item_ptr->nsapi_ptr->nsapi ];
          if ( nsapi_ptr )
          {
            snsm_dl_mes->cmd.sndcp_status_req.as_id =
              geran_map_gas_id_to_nas_id (gas_id);
            break;
          }
        }
        /* Get next NSAPI from the list */
        item_ptr =
          (gsn_nsapi_q_t*)q_next( &(sapi_ptr->nsapi_ptr_q), &item_ptr->link );
      }
    }
    else
    {
      MSG_GERAN_LOW_1_G("GSN ""SAPI %d: has no mapped NSAPIs", gsn_extern_sapi[sapi]);
    }
  }
#endif /* FEATURE_DUAL_SIM */

  /* Send SNSM_STATUS_REQ to SM   */
  MSG_GERAN_MED_1_G("GSN ""ME: Sent SNSM_STATUS_REQ SAPI %d",gsn_extern_sapi[sapi]);
  sm_put_cmd( snsm_dl_mes );
}

/*===========================================================================
===
===  FUNCTION      GSN_ENCODE_ACTV_NSAPIS_DIAG_RPT()
===
===  DESCRIPTION
===
===   Encodes the active NSAPIs information report for diagnostic monitoring
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/
void gsn_encode_actv_nsapis_diag_rpt( gas_id_t gas_id )
{
  uint8                             nsapi;
  gsndcp_diag_actv_nsapis_rpt_t     actv_nsapis_bitmap;

  if( actv_nsapis_rpt_fnc != NULL )
  {
    actv_nsapis_bitmap = 0;

    for( nsapi = (uint8)GSN_NSAPI_5; nsapi < GSN_NSAPI_MAX; nsapi++ )
    {
      if( actv_nsapis[ gas_id ][nsapi] != NULL )
        actv_nsapis_bitmap |= (uint16)(0x01 << GSN_EXTERN_NSAPI(nsapi) );
    }

    actv_nsapis_rpt_fnc( &actv_nsapis_bitmap );
  }
}

/*===========================================================================
===
===  FUNCTION      GSN_ENCODE_CNTX_DIAG_RPT()
===
===  DESCRIPTION
===
===   Encodes context information report of the specified NSAPI
===   for diagnostic monitoring.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/
void gsn_encode_cntx_diag_rpt( gas_id_t gas_id, uint8 nsapi )
{
  gsndcp_diag_nsapi_cntx_rpt_t nsapi_cntx_rpt;
  gsn_nsapi_e_t                *gsn_nsapi_ptr;
  gsn_nsapi_t                   gnsapi;

  if ( nsapi_cntx_rpt_fnc != NULL )
  {
    /* check if valid NSAPI */
    if( GSN_VALID_NSAPI(nsapi))
    {
      gnsapi        = (gsn_nsapi_t) GSN_INTERN_NSAPI(nsapi);
      gsn_nsapi_ptr = actv_nsapis[ gas_id ][gnsapi];

      if( gsn_nsapi_ptr != NULL )
      {
        nsapi_cntx_rpt.nsapi_addr        = nsapi;   /* External address needed */
        nsapi_cntx_rpt.ciphering         = gsn_nsapi_ptr->cipher;
        nsapi_cntx_rpt.peak_throughput   =
                                gsn_nsapi_ptr->qos.peak_throughput;
        nsapi_cntx_rpt.radio_priority    = gsn_nsapi_ptr->radio_pr;
        nsapi_cntx_rpt.reliability_class =
                                gsn_nsapi_ptr->qos.reliability_class;

        nsapi_cntx_rpt.data_comp         = gsn_nsapi_ptr->data_comp;

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP
        if( gsn_nsapi_ptr->dcomp_e_num[T1_ALG_V42BIS] != NO_COMP_ENT )
        {
          nsapi_cntx_rpt.data_comp_alg  = T1_ALG_V42BIS;
          nsapi_cntx_rpt.data_comp_ent  = gsn_nsapi_ptr->dcomp_e_num[T1_ALG_V42BIS];
        }
        else
        {
          nsapi_cntx_rpt.data_comp_alg  = NO_COMP_ENT;
          nsapi_cntx_rpt.data_comp_ent  = NO_COMP_ENT;
        }
#else
        nsapi_cntx_rpt.data_comp_alg  = NO_COMP_ENT;
        nsapi_cntx_rpt.data_comp_ent  = NO_COMP_ENT;
#endif

        nsapi_cntx_rpt.protocol_comp     = gsn_nsapi_ptr->pci_comp;

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP
        if( gsn_nsapi_ptr->pcomp_e_num[T2_ALG_RFC1144] != NO_COMP_ENT )
        {
          nsapi_cntx_rpt.protocol_comp_alg = T2_ALG_RFC1144;
          nsapi_cntx_rpt.protocol_comp_ent = gsn_nsapi_ptr->pcomp_e_num[T2_ALG_RFC1144];
        }
        else
        {
          nsapi_cntx_rpt.protocol_comp_alg = NO_COMP_ENT;
          nsapi_cntx_rpt.protocol_comp_ent = NO_COMP_ENT;
        }
#else
        nsapi_cntx_rpt.protocol_comp_alg = NO_COMP_ENT;
        nsapi_cntx_rpt.protocol_comp_ent = NO_COMP_ENT;
#endif

        nsapi_cntx_rpt.sapi_addr  = gsn_extern_sapi[gsn_nsapi_ptr->sapi_ptr->sapi];
        nsapi_cntx_rpt.n201_i     = gsn_n201_i[gas_id][gsn_nsapi_ptr->sapi_ptr->sapi];
        nsapi_cntx_rpt.n201_u     = gsn_n201_u[gas_id][gsn_nsapi_ptr->sapi_ptr->sapi];

        nsapi_cntx_rpt_fnc( &nsapi_cntx_rpt );
      }
    }
  }
}

/*===========================================================================
===
===  FUNCTION      GSN_ENCODE_STATE_DIAG_RPT()
===
===  DESCRIPTION
===
===   Encodes state report of the specified NSAPI for diagnostic monitoring.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/
void gsn_encode_state_diag_rpt( gas_id_t gas_id, uint8 nsapi )
{
  gsndcp_diag_nsapi_state_rpt_t nsapi_state_rpt;
  gsn_nsapi_e_t                 *gsn_nsapi_ptr;
  gsn_nsapi_t                   gnsapi;

  if ( nsapi_state_rpt_fnc != NULL )
  {
    /* check if valid NSAPI */
    if( GSN_VALID_NSAPI(nsapi) )
    {
      gnsapi        = (gsn_nsapi_t) GSN_INTERN_NSAPI(nsapi);
      gsn_nsapi_ptr = actv_nsapis[ gas_id ][gnsapi];

      if( gsn_nsapi_ptr != NULL )
      {
        nsapi_state_rpt.nsapi_addr     = nsapi;   /* External address needed */
        nsapi_state_rpt.nsapi_dl_state = (uint8) gsn_nsapi_ptr->dle.state;
        nsapi_state_rpt.nsapi_ul_state = gsn_nsapi_ptr->ule.susp_flag;
        nsapi_state_rpt.sapi_state     = (uint8) gsn_nsapi_ptr->sapi_ptr->state;

        nsapi_state_rpt_fnc( &nsapi_state_rpt );
      }
    }
  }
}


/*** EOF: don't remove! ***/
