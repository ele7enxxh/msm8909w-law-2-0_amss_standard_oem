/*****************************************************************************
***
*** TITLE
***
***  GPRS SNDCP LAYER
***
***
*** DESCRIPTION
***
***  Provides the SNDCP layer access functions to the
***  layer 2 control tasks, in the form of uplink and downlink
***  initialisation, and uplink and downlink event dispatch functions.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gsndcp/src/gsndcp.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 27/11/13   ws        CR582513 GERAN API Unification for modem data services
*** 11/14/02   rc      Added multimode support.
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

#define FEATURE_DSM_WM_CB

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

#include "comdef.h"
#include "gmutex.h"
#include "cfa.h"
#include "gs.h"
#include "ms.h"
#include "gllc_v.h"
#include "gllc_v_g.h"
#include "gprs_mem.h"
#include "gsndcp.h"
#include "gsndcpi.h"
#include "gsn_hdlr.h"
#include "gsni.h"
#include "gsndcp_v_g.h"
#include "dsm.h"
#include "string.h"
#include "time_svc.h"
#include "geran_dual_sim_g.h"
#include "assert.h"
#include "gsndcplog.h"

#if ((defined FEATURE_GSM_GPRS_SNDCP_PCOMP) || (defined FEATURE_GSM_GPRS_SNDCP_DCOMP))
#include "gsn_util.h"
#endif


/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/

/* Control primitives Ids send to SNDCP from SM and LLC layers */
typedef enum
{
  LL_CMD,    /* LL control primitive    */
  SM_CMD,    /* SM control primitive    */
  GSN_CMD    /* SNDCP internal message  */

} gsn_cmd_id_t;

/* The Control primitives that the SM and LLC layers (plus internal SNDCP cmd)
** send to SNDCP need to be unified and stored in a common queue (gsn_cmd_q).
** The following type serves this purpose.
*/
typedef struct
{
  q_link_type             link;
  gsn_cmd_id_t            cmd_id;

  union
  {
    gsmsn_mes_type        *sm_msg;
    gprs_ll_dl_sig_mes_t  *ll_msg;
    gsn_internal_msg_t    *gsn_msg;

  } ctrl_msg;

} gsn_ctrl_msg_t;

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

/* Queue that holds LL->SNDCP SN-PDUs (LL-DATA.IND and LL-UNITDATA.IND primitives) */
q_type              gsnll_data_q;
dsm_watermark_type  gsnll_data_wm;

/* Queue that holds SM->SNDCP and LLC->SNDCP commands (SNSM and LL control primitives)  */
q_type              gsn_cmd_q;

/* If TRUE, Indicates that the SNDCP layer has been initialised  */
boolean             sndcp_init = FALSE;

/* Pointer to the host task control block. Set upon SNDCP initialisation */
rex_tcb_type        *host_tcb_ptr = NULL;

#ifndef FEATURE_DATA_MM
/* Rex signal for the gsn_cmd_q queue */
rex_sigs_type       host_cmd_queue_sig;

/* Rex signal for the gsnll_data_q queue and associated flow control mask. */
rex_sigs_type       host_ll_dl_queue_sig;
#else

  comp_sig_enum_type host_cmd_queue_sig;
  comp_sig_enum_type host_ll_dl_queue_sig;

#endif /* FEATURE_DATA_MM */

boolean             gsn_ll_dl_susp_flag = FALSE;

/* Hold LLC SAPIs' N201_I and N201_U  */
uint16              gsn_n201_i[ NUM_GERAN_DATA_SPACES ][ GSN_SAPI_MAX ];
uint16              gsn_n201_u[ NUM_GERAN_DATA_SPACES ][ GSN_SAPI_MAX ];

uint32              process_delay_max[ NUM_GERAN_DATA_SPACES ]  = {0};

/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/

gas_id_t gsndcp_get_ll_dl_gas_id ( gprs_ll_dl_dat_mes_t  *mes_ptr );

geran_pdu_priority_t gsndcp_ul_pdu_priority_hdlr( dsm_item_type *npdu );


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

/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_HIWATER()
===
===  DESCRIPTION
===
===    To be called when a high watermark has been reached in the SNDCP to LLC
===    watermark interface
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
===    PDP UL data flow on the specified NSAPI will be suspended
===
===========================================================================*/

void gsn_process_hiwater( gas_id_t gas_id, gsn_sapi_t sapi, gsn_nsapi_mode_t  op_mode, uint8 susp_reason )
{

  gsn_nsapi_q_t     *item_ptr = NULL;
  gsn_sapi_e_t      *sapi_ptr = NULL;

  /* Lock/Protect this section. SNDCP could be in the process of deactivating
  ** PDP context and releasing actv_sapis[] and releasing queues/mutexes etc.
  ** while LLC calling this function.
  */
  GSN_LOCK();

  if( actv_sapis[gas_id][sapi] != NULL )
  {
    sapi_ptr = actv_sapis[gas_id][sapi];

    if ( sapi_ptr->is_nsapi_ptr_q_valid )
    {

      item_ptr = (gsn_nsapi_q_t*)q_check( &(sapi_ptr->nsapi_ptr_q) );

      if( item_ptr )
      {
        while( item_ptr != NULL )
        {
          if( item_ptr->nsapi_ptr->op_mode == op_mode )
          {
            /* Suspend UL data flow on the NSAPI */
            GSN_SET_SUSP_EVENT( gas_id,
                                item_ptr->nsapi_ptr->nsapi,
                                susp_reason );
          }
          /* Get next NSAPI from the list */
          item_ptr = (gsn_nsapi_q_t*)q_next( &(sapi_ptr->nsapi_ptr_q),
                                             &item_ptr->link );
        }
      }
      else
      {
        MSG_GERAN_LOW_1("GSN ""LL %d: No mapped NSAPIs", gsn_extern_sapi[sapi]);
      }
    }
    else
    {
      MSG_GERAN_ERROR_1("GSN ""LL %d: NSAPI ptr queue invalid", gsn_extern_sapi[sapi]);
    }
  }
  else
  {
    MSG_GERAN_LOW_1("GSN ""LL %d: Inactive SAPI", gsn_extern_sapi[sapi]);
  }

  GSN_UNLOCK();
}

/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_LOWATER()
===
===  DESCRIPTION
===
===    To be called when a low watermark has been reached in the SNDCP to LLC
===    watermark interface
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
===   PDP UL data flow on the specified NSAPI might be resumed
===
===========================================================================*/

void gsn_process_lowater( gas_id_t gas_id, gsn_sapi_t sapi, gsn_nsapi_mode_t op_mode, uint8 susp_reason )
{

  gsn_nsapi_q_t     *item_ptr = NULL;
  gsn_sapi_e_t      *sapi_ptr = NULL;

  /* Lock/Protect this section. SNDCP could be in the process of deactivating
  ** PDP context and releasing actv_sapis[] and releasing queues/mutexes etc.
  ** while LLC calling this function.
  */
  GSN_LOCK();

  if( actv_sapis[gas_id][sapi] != NULL )
  {
    sapi_ptr = actv_sapis[gas_id][sapi];

    if ( sapi_ptr->is_nsapi_ptr_q_valid )
    {

      item_ptr = (gsn_nsapi_q_t*)q_check( &(sapi_ptr->nsapi_ptr_q) );

      if( item_ptr )
      {
        while( item_ptr != NULL )
        {
          if( item_ptr->nsapi_ptr->op_mode == op_mode )
          {
            /* Resume UL data flow on the NSAPI */
            GSN_CLR_SUSP_EVENT( gas_id, item_ptr->nsapi_ptr->nsapi,
                                susp_reason );
          }
          /* Get next NSAPI from the list */
          item_ptr = (gsn_nsapi_q_t*)q_next( &(sapi_ptr->nsapi_ptr_q),
                                             &item_ptr->link );
        }
      }
      else
      {
        MSG_GERAN_LOW_1("GSN ""LL %d: No mapped NSAPIs", gsn_extern_sapi[sapi]);
      }
    }
    else
    {
      MSG_GERAN_ERROR_1("GSN ""LL %d: NSAPI ptr queue invalid", gsn_extern_sapi[sapi]);
    }
  }
  else
  {
    MSG_GERAN_LOW_1("GSN ""LL %d: Inactive SAPI", gsn_extern_sapi[sapi]);
  }

  GSN_UNLOCK();
}

/*===========================================================================
===
===  FUNCTION      GSN_LL_DL_SIG_GET_MES_BUF()
===
===  DESCRIPTION
===
===    To be called by LLC to obtain a message buffer to convey a downlink
===    signalling message, to SNCDP.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised.
===
===  RETURN VALUE
===
===    Pointer to allocated gprs_ll_dl_sig_mes_t buffer.
===    NULL is returned if memory is unavailable.
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

gprs_ll_dl_sig_mes_t *gsn_ll_dl_sig_get_mes_buf( void )
{

  return( GPRS_MEM_MALLOC( sizeof( gprs_ll_dl_sig_mes_t ) ) );
}

/*===========================================================================
===
===  FUNCTION      GSN_LL_DL_SIG_PUT_MES_BUF()
===
===  DESCRIPTION
===
===  This function puts a command buffer into snll_cmd_q.
===  Note that the command buffer must have been requested through a
===  call to gsn_ll_dl_sig_get_mes_buf()
===
===  DEPENDENCIES
===
===    SNDCP must be initialised.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    A signal is set for the host task and this might causes a context switch.
===
===========================================================================*/

void gsn_ll_dl_sig_put_mes_buf( gprs_ll_dl_sig_mes_t *mes_ptr )
{
  gsn_ctrl_msg_t  *adpt_msg;

  #ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif

  /* Adapt LL msg so that can be stored in gsn_cmd_q
  */

  /* Allocate memory to convey the addapted message */
  adpt_msg = GPRS_MEM_MALLOC( sizeof( gsn_ctrl_msg_t ) );
  GSN_ASSERT( adpt_msg != NULL );

  /* Message sent by LLC */
  adpt_msg->cmd_id = LL_CMD;

  adpt_msg->ctrl_msg.ll_msg = mes_ptr;

  /* Lock here as this function could be called simultaneously
     by either LLC task in DSDA.*/

  GSN_LOCK();

  /* Initialize the queue link before putting an item in a queue. */
  (void) q_link(adpt_msg, &(adpt_msg->link) );

  /* Put the item in the queue */
  q_put(&gsn_cmd_q, &(adpt_msg->link));

  /* Set the signal for this queue */
#ifdef FEATURE_DATA_MM
  COMP_SET_SIGNAL(host_cmd_queue_sig);
#else
  (void)rex_set_sigs(host_tcb_ptr, host_cmd_queue_sig);
#endif /* FEATURE_DATA_MM */

  GSN_UNLOCK();

}

/*===========================================================================
===
===  FUNCTION      GSN_LL_DL_DAT_PUT_MES_BUF()
===
===  DESCRIPTION
===
===    To be called by LLC for posting a downlink data message to SNCDP.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised.
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

void gsn_ll_dl_dat_put_mes_buf( gprs_ll_dl_dat_mes_t *mes_ptr )
{
  dsm_item_type *l3_pdu_ptr;

  if( mes_ptr != NULL )
  {

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */


    /* ...................................................................... */

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

    /* ...................................................................... */

    /* Tag this PDU with a timestamp to permit delayed processing detection. */

#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST
    (void)time_get(mes_ptr -> timestamp);
#endif

    /* ...................................................................... */

    /* Lock here as this function could be called simultaneously
       by either LLC task in DSDA.*/

    GSN_LOCK();

    /* Configure 'app_field' of lead DSM item in message l3_pdu to 'bear'
      associated message, and enqueue the PDU to the LL DL DSM watermark. */

    switch ( mes_ptr -> mes.prim )
    {
    case LL_DATA_IND:

      l3_pdu_ptr = mes_ptr -> mes.data_ind_mes.l3_pdu;
      if ( gsnll_data_wm.current_cnt + GPRS_PDU_LENGTH(l3_pdu_ptr) <=
           gsnll_data_wm.dont_exceed_cnt                              )
      {
        mes_ptr -> mes.data_ind_mes.l3_pdu -> app_field = (uint32)mes_ptr;
        dsm_enqueue( &gsnll_data_wm, &l3_pdu_ptr );
      }
      else
      {
        MSG_GERAN_ERROR_1("GSN " "LL %d: DL Dat Put: LL-DATA-IND: Discard",
          mes_ptr -> sapi);
        GPRS_PDU_FREE( &l3_pdu_ptr );
        GPRS_MEM_FREE( mes_ptr );
      }
      break;

    case LL_UNITDATA_IND:

      l3_pdu_ptr = mes_ptr -> mes.unitdata_ind_mes.l3_pdu;
      if ( gsnll_data_wm.current_cnt + GPRS_PDU_LENGTH(l3_pdu_ptr) <=
           gsnll_data_wm.dont_exceed_cnt                              )
      {
        mes_ptr -> mes.unitdata_ind_mes.l3_pdu -> app_field = (uint32)mes_ptr;
        dsm_enqueue( &gsnll_data_wm, &l3_pdu_ptr );
      }
      else
      {
        MSG_GERAN_ERROR_1("GSN " "LL %d: DL Dat Put: LL-UDATA-IND: Discard",
          mes_ptr -> sapi);
        GPRS_PDU_FREE( &l3_pdu_ptr );
        GPRS_MEM_FREE( mes_ptr );
      }
      break;

    default:

      MSG_GERAN_ERROR_2("GSN " "LL %d: DL Dat Put: Invalid Prim %d",
        mes_ptr -> sapi, mes_ptr -> mes.prim);
      GSN_MEM_FREE( mes_ptr );
    }

    /* ...................................................................... */

    /* Set the signal for this queue */

    if ( !gsn_ll_dl_susp_flag )
    {
#ifdef FEATURE_DATA_MM
      COMP_SET_SIGNAL(host_ll_dl_queue_sig);
#else
      (void)rex_set_sigs(host_tcb_ptr, host_ll_dl_queue_sig);
#endif /* FEATURE_DATA_MM */
    }

    GSN_UNLOCK();
  }
  else
  {
     MSG_GERAN_ERROR_0("GSN " "LL DL Data Put: Invalid mes_ptr");
  }
}

/*===========================================================================
===
===  FUNCTION    GSN_LLx_UL_ACK_HIWATER()   / GSN_LLx_UL_ACK_LOWATER()
===              GSN_LLx_UL_UNACK_HIWATER() / GSN_LLx_UL_UNACK_LOWATER()
===
===  DESCRIPTION
===
===    High and low watermark callback functions for the eight ( 4 ACK and 4 UNACK)
===    watermark interfaces to LLC. Since the LLC watermark interface restrict the use
===    of any function parameters to be passed via the call back functions, SNDCP
===    needs to provide 16 callback functions upon registration with LLC (4 high watermark
===    and 4 low watermark functions for the 4 ACK LLC SAPs, plus 4 high watermark
===    and 4  low watermark functions for the 4 UNACK LLC SAPs).
===    Refer to function gllc_register_ll_client_sndcp() in gllc.h.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised.
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

void gsn_ll3_ul_ack_hiwater( dsm_watermark_type *wm_p, void *context )
{
  gas_id_t gas_id = ( gas_id_t )context;

  NOTUSED(wm_p);

   MSG_GERAN_LOW_0("GSN " "LL 3: Info ACK UL Suspend");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_hiwater( gas_id, GSN_SAPI_3, ACK, GSN_UL_SUSP_CTS_OFF );
}

/* ....................................................................... */

void gsn_ll3_ul_ack_lowater( dsm_watermark_type *wm_p, void *context )
{
  gas_id_t gas_id = ( gas_id_t )context;

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "GSN LL 3: Info ACK UL Resume");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_lowater( gas_id, GSN_SAPI_3, ACK, GSN_UL_SUSP_CTS_OFF );
}

/* ----------------------------------------------------------------------- */

void gsn_ll3_ul_unack_hiwater( dsm_watermark_type *wm_p, void *context )
{
  gas_id_t gas_id = ( gas_id_t )context;

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "LL 3: Info UNACK UL Suspend");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_hiwater( gas_id, GSN_SAPI_3, UNACK, GSN_UL_SUSP_CTS_OFF );

}

/* ....................................................................... */

void gsn_ll3_ul_unack_lowater( dsm_watermark_type *wm_p, void *context )
{
  gas_id_t gas_id = ( gas_id_t )context;

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "GSN LL 3: Info UNACK UL Resume");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_lowater( gas_id, GSN_SAPI_3, UNACK, GSN_UL_SUSP_CTS_OFF );

}

/* ======================================================================= */

void gsn_ll5_ul_ack_hiwater( dsm_watermark_type *wm_p, void *context )
{
  gas_id_t gas_id = ( gas_id_t )context;

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "LL 5: Info ACK UL Suspend");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_hiwater( gas_id, GSN_SAPI_5, ACK, GSN_UL_SUSP_CTS_OFF );

}

/* ....................................................................... */

void gsn_ll5_ul_ack_lowater( dsm_watermark_type *wm_p, void *context )
{
  gas_id_t gas_id = ( gas_id_t )context;

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "GSN LL 5: Info ACK UL Resume");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_lowater( gas_id, GSN_SAPI_5, ACK, GSN_UL_SUSP_CTS_OFF );

}

/* ----------------------------------------------------------------------- */

void gsn_ll5_ul_unack_hiwater( dsm_watermark_type *wm_p, void *context )
{
  gas_id_t gas_id = ( gas_id_t )context;

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "LL 5: Info UNACK UL Suspend");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_hiwater( gas_id, GSN_SAPI_5, UNACK, GSN_UL_SUSP_CTS_OFF );

}

/* ....................................................................... */

void gsn_ll5_ul_unack_lowater( dsm_watermark_type *wm_p, void *context )
{
  gas_id_t gas_id = ( gas_id_t )context;

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "GSN LL 5: Info UNACK UL Resume");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_lowater( gas_id, GSN_SAPI_5, UNACK, GSN_UL_SUSP_CTS_OFF );

}

/* ======================================================================= */

void gsn_ll9_ul_ack_hiwater( dsm_watermark_type *wm_p, void *context )
{
  gas_id_t gas_id = ( gas_id_t )context;

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "LL 9: Info ACK UL Suspend");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_hiwater( gas_id, GSN_SAPI_9, ACK, GSN_UL_SUSP_CTS_OFF );

}

/* ....................................................................... */

void gsn_ll9_ul_ack_lowater( dsm_watermark_type *wm_p, void *context )
{
  gas_id_t gas_id = ( gas_id_t )context;

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "GSN LL 9: Info ACK UL Resume");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_lowater( gas_id, GSN_SAPI_9, ACK , GSN_UL_SUSP_CTS_OFF);

}

/* ----------------------------------------------------------------------- */

void gsn_ll9_ul_unack_hiwater( dsm_watermark_type *wm_p, void *context )
{
  gas_id_t gas_id = ( gas_id_t )context;

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "LL 9: Info UNACK UL Suspend");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_hiwater( gas_id, GSN_SAPI_9, UNACK, GSN_UL_SUSP_CTS_OFF );

}

/* ....................................................................... */

void gsn_ll9_ul_unack_lowater( dsm_watermark_type *wm_p, void *context )
{
  gas_id_t gas_id = ( gas_id_t )context;

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "GSN LL 9: Info UNACK UL Resume");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_lowater( gas_id, GSN_SAPI_9, UNACK, GSN_UL_SUSP_CTS_OFF );

}

/* ----------------------------------------------------------------------- */

void gsn_ll11_ul_ack_hiwater( dsm_watermark_type *wm_p, void *context )
{
  gas_id_t gas_id = ( gas_id_t )context;

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "LL 11: Info ACK UL Suspend");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_hiwater( gas_id, GSN_SAPI_11, ACK, GSN_UL_SUSP_CTS_OFF );

}

/* ....................................................................... */

void gsn_ll11_ul_ack_lowater( dsm_watermark_type *wm_p, void *context )
{
  gas_id_t gas_id = ( gas_id_t )context;

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "GSN LL 11: Info ACK UL Resume");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_lowater( gas_id, GSN_SAPI_11, ACK, GSN_UL_SUSP_CTS_OFF );

}

/* ----------------------------------------------------------------------- */

void gsn_ll11_ul_unack_hiwater( dsm_watermark_type *wm_p, void *context )
{
  gas_id_t gas_id = ( gas_id_t )context;

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "LL 11: Info UNACK UL Suspend");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_hiwater( gas_id, GSN_SAPI_11, UNACK, GSN_UL_SUSP_CTS_OFF );

}

/* ....................................................................... */

void gsn_ll11_ul_unack_lowater( dsm_watermark_type *wm_p, void *context )
{
  gas_id_t gas_id = ( gas_id_t )context;

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "GSN LL 11: Info UNACK UL Resume");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_lowater( gas_id, GSN_SAPI_11, UNACK, GSN_UL_SUSP_CTS_OFF );

}

/* ....................................................................... */

void gsn_ll3_ul_ack_lowater_xid_reset( gas_id_t gas_id )
{
  gsn_process_lowater( gas_id, GSN_SAPI_3, ACK, GSN_UL_SUSP_XID_RESET );
}

/* ....................................................................... */

void gsn_ll3_ul_ack_hiwater_xid_reset( gas_id_t gas_id )
{
  gsn_process_hiwater( gas_id, GSN_SAPI_3, ACK, GSN_UL_SUSP_XID_RESET );
}

/* ....................................................................... */

void gsn_ll3_ul_unack_lowater_xid_reset( gas_id_t gas_id )
{
  gsn_process_lowater( gas_id, GSN_SAPI_3, UNACK, GSN_UL_SUSP_XID_RESET );
}

/* ....................................................................... */

void gsn_ll3_ul_unack_hiwater_xid_reset( gas_id_t gas_id )
{
  gsn_process_hiwater( gas_id, GSN_SAPI_3, UNACK, GSN_UL_SUSP_XID_RESET );
}

/* ....................................................................... */

void gsn_ll5_ul_ack_lowater_xid_reset( gas_id_t gas_id )
{
  gsn_process_lowater( gas_id, GSN_SAPI_5, ACK, GSN_UL_SUSP_XID_RESET );
}

/* ....................................................................... */

void gsn_ll5_ul_ack_hiwater_xid_reset( gas_id_t gas_id )
{
  gsn_process_hiwater( gas_id, GSN_SAPI_5, ACK, GSN_UL_SUSP_XID_RESET );
}

/* ....................................................................... */

void gsn_ll5_ul_unack_lowater_xid_reset( gas_id_t gas_id )
{
  gsn_process_lowater( gas_id, GSN_SAPI_5, UNACK, GSN_UL_SUSP_XID_RESET );
}

/* ....................................................................... */

void gsn_ll5_ul_unack_hiwater_xid_reset( gas_id_t gas_id )
{
  gsn_process_hiwater( gas_id, GSN_SAPI_5, UNACK, GSN_UL_SUSP_XID_RESET );
}

/* ....................................................................... */

void gsn_ll9_ul_ack_lowater_xid_reset( gas_id_t gas_id )
{
  gsn_process_lowater( gas_id, GSN_SAPI_9, ACK, GSN_UL_SUSP_XID_RESET );
}

/* ....................................................................... */

void gsn_ll9_ul_ack_hiwater_xid_reset( gas_id_t gas_id )
{
  gsn_process_hiwater( gas_id, GSN_SAPI_9, ACK, GSN_UL_SUSP_XID_RESET );
}

/* ....................................................................... */

void gsn_ll9_ul_unack_lowater_xid_reset( gas_id_t gas_id )
{
  gsn_process_lowater( gas_id, GSN_SAPI_9, UNACK, GSN_UL_SUSP_XID_RESET );
}

/* ....................................................................... */

void gsn_ll9_ul_unack_hiwater_xid_reset( gas_id_t gas_id )
{
  gsn_process_hiwater( gas_id, GSN_SAPI_9, UNACK, GSN_UL_SUSP_XID_RESET );
}

/* ....................................................................... */

void gsn_ll11_ul_ack_lowater_xid_reset( gas_id_t gas_id )
{
  gsn_process_lowater( gas_id, GSN_SAPI_11, ACK, GSN_UL_SUSP_XID_RESET );
}

/* ....................................................................... */

void gsn_ll11_ul_ack_hiwater_xid_reset( gas_id_t gas_id )
{
  gsn_process_hiwater( gas_id, GSN_SAPI_11, ACK, GSN_UL_SUSP_XID_RESET );
}

/* ....................................................................... */

void gsn_ll11_ul_unack_lowater_xid_reset( gas_id_t gas_id )
{
  gsn_process_lowater( gas_id, GSN_SAPI_11, UNACK, GSN_UL_SUSP_XID_RESET );
}

/* ....................................................................... */

void gsn_ll11_ul_unack_hiwater_xid_reset( gas_id_t gas_id )
{
  gsn_process_hiwater( gas_id, GSN_SAPI_11, UNACK, GSN_UL_SUSP_XID_RESET );
}

/* ....................................................................... */

void gsn_ll3_ul_ack_lowater_nas_susp( gas_id_t gas_id )
{
  gsn_process_lowater( gas_id, GSN_SAPI_3, ACK, GSN_UL_SUSP_NAS_SUSP );
}

/* ....................................................................... */

void gsn_ll3_ul_ack_hiwater_nas_susp( gas_id_t gas_id )
{
  gsn_process_hiwater( gas_id, GSN_SAPI_3, ACK, GSN_UL_SUSP_NAS_SUSP );
}

/* ....................................................................... */

void gsn_ll3_ul_unack_lowater_nas_susp( gas_id_t gas_id )
{
  gsn_process_lowater( gas_id, GSN_SAPI_3, UNACK, GSN_UL_SUSP_NAS_SUSP );
}

/* ....................................................................... */

void gsn_ll3_ul_unack_hiwater_nas_susp( gas_id_t gas_id )
{
  gsn_process_hiwater( gas_id, GSN_SAPI_3, UNACK, GSN_UL_SUSP_NAS_SUSP );
}

/* ....................................................................... */

void gsn_ll5_ul_ack_lowater_nas_susp( gas_id_t gas_id )
{
  gsn_process_lowater( gas_id, GSN_SAPI_5, ACK, GSN_UL_SUSP_NAS_SUSP );
}

/* ....................................................................... */

void gsn_ll5_ul_ack_hiwater_nas_susp( gas_id_t gas_id )
{
  gsn_process_hiwater( gas_id, GSN_SAPI_5, ACK, GSN_UL_SUSP_NAS_SUSP );
}

/* ....................................................................... */

void gsn_ll5_ul_unack_lowater_nas_susp( gas_id_t gas_id )
{
  gsn_process_lowater( gas_id, GSN_SAPI_5, UNACK, GSN_UL_SUSP_NAS_SUSP );
}

/* ....................................................................... */

void gsn_ll5_ul_unack_hiwater_nas_susp( gas_id_t gas_id )
{
  gsn_process_hiwater( gas_id, GSN_SAPI_5, UNACK, GSN_UL_SUSP_NAS_SUSP );
}

/* ....................................................................... */

void gsn_ll9_ul_ack_lowater_nas_susp( gas_id_t gas_id )
{
  gsn_process_lowater( gas_id, GSN_SAPI_9, ACK, GSN_UL_SUSP_NAS_SUSP );
}

/* ....................................................................... */

void gsn_ll9_ul_ack_hiwater_nas_susp( gas_id_t gas_id )
{
  gsn_process_hiwater( gas_id, GSN_SAPI_9, ACK, GSN_UL_SUSP_NAS_SUSP );
}

/* ....................................................................... */

void gsn_ll9_ul_unack_lowater_nas_susp( gas_id_t gas_id )
{
  gsn_process_lowater( gas_id, GSN_SAPI_9, UNACK, GSN_UL_SUSP_NAS_SUSP );
}

/* ....................................................................... */

void gsn_ll9_ul_unack_hiwater_nas_susp( gas_id_t gas_id )
{
  gsn_process_hiwater( gas_id, GSN_SAPI_9, UNACK, GSN_UL_SUSP_NAS_SUSP );
}

/* ....................................................................... */

void gsn_ll11_ul_ack_lowater_nas_susp( gas_id_t gas_id )
{
  gsn_process_lowater( gas_id, GSN_SAPI_11, ACK, GSN_UL_SUSP_NAS_SUSP );
}

/* ....................................................................... */

void gsn_ll11_ul_ack_hiwater_nas_susp( gas_id_t gas_id )
{
  gsn_process_hiwater( gas_id, GSN_SAPI_11, ACK, GSN_UL_SUSP_NAS_SUSP );
}

/* ....................................................................... */

void gsn_ll11_ul_unack_lowater_nas_susp( gas_id_t gas_id )
{
  gsn_process_lowater( gas_id, GSN_SAPI_11, UNACK, GSN_UL_SUSP_NAS_SUSP );
}

/* ....................................................................... */

void gsn_ll11_ul_unack_hiwater_nas_susp( gas_id_t gas_id )
{
  gsn_process_hiwater( gas_id, GSN_SAPI_11, UNACK, GSN_UL_SUSP_NAS_SUSP );
}


/* ======================================================================= */


/*****************************************************************************
***
***     Public Data
***
*****************************************************************************/


/* SNDCP critical section to protect accesses to messages and internal vars
*/

rex_crit_sect_type gsn_crit_sec;


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/


/*===========================================================================
===
===  FUNCTION      GSNDCP_INITIALISE()
===
===  DESCRIPTION
===
===    Permits a given task context (PSMGR) to host SNDCP processing by
===    registering the host task's TCB pointer with SNDCP. The REX signals
===    that have been allocated by the host task, for scheduling the
===    processing of the SNDCP command queue and LL SAP downlink
===    queue, are also registered.
===
===  DEPENDENCIES
===
===    Must be called at host task initialization.
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
void gsndcp_initialise
(
#ifdef FEATURE_DATA_MM
  void
#else
  rex_tcb_type            *tcb_ptr,
  rex_sigs_type           cmd_queue_sig,
  rex_sigs_type           ll_dl_queue_sig
#endif /* FEATURE_DATA_MM */
)
{
  uint8                         i;
  gas_id_t                      gas_id;


  /* Initialise critical section */

  rex_init_crit_sect(&gsn_crit_sec);



#ifndef FEATURE_DATA_MM
  /* Get host task control block pointer and the REX signals related to SNDCP queues */
  host_tcb_ptr = tcb_ptr;
  host_cmd_queue_sig = cmd_queue_sig;
  host_ll_dl_queue_sig = ll_dl_queue_sig;
#else
  /* Set the REX signals related to SNDCP queues */
  host_cmd_queue_sig = COMP_SNDCP_CMD_Q_SIGNAL;
  host_ll_dl_queue_sig = COMP_SNDCP_LL_DL_Q_SIGNAL;

  /* Set the signal handler for the command Queue signal */
  (void)comp_set_sig_handler((comp_sig_enum_type)host_cmd_queue_sig,
                             gsndcp_process_cmd_queue, NULL);

  /* Set the signal handler for the dl data Queue signal */
  (void)comp_set_sig_handler((comp_sig_enum_type)host_ll_dl_queue_sig,
                             gsndcp_process_ll_dl_queue, NULL);

#endif /* FEATURE_DATA_MM */

  /* Initialise the queue that holds SM->SNDCP and LL->SNDCP control primitives*/
  (void) q_init( &gsn_cmd_q );

  /* Initialise the watermark that holds LL->SNDCP SN-PDUs (LL DL data
     primitives). Need to ensure that High Water, Low Water, and Don't Exceed
     counts for this watermark are initialised via a call to
     gllc_register_ll_client_sndcp( ) as is done below. */

  dsm_queue_init ( &gsnll_data_wm, 0, &gsnll_data_q );

  gsnll_data_wm.each_enqueue_func_ptr = NULL;
  gsnll_data_wm.lowater_func_ptr      = gllc_llme_clr_own_rcvr_busy_cond;
  gsnll_data_wm.lowater_func_data     = 0;
  gsnll_data_wm.gone_empty_func_ptr   = NULL;
  gsnll_data_wm.non_empty_func_ptr    = NULL;
  gsnll_data_wm.hiwater_func_ptr      = gllc_llme_set_own_rcvr_busy_cond;
  gsnll_data_wm.hiwater_func_data     = 0;
  gsnll_data_wm.lo_watermark          =
    ( gllc_ll_get_max_N201_U( gsn_extern_sapi[GSN_SAPI_3] ) * 2);

  gsnll_data_wm.hi_watermark          =
    ( gllc_ll_get_max_N201_U( gsn_extern_sapi[GSN_SAPI_3] ) * 4);

  gsnll_data_wm.dont_exceed_cnt       =
    ( gsnll_data_wm.hi_watermark * 28 );

  gsnll_data_wm.current_cnt           = 0;
#ifdef FEATURE_DSM_MEM_CHK
  gsnll_data_wm.highest_cnt           = 0;
  gsnll_data_wm.total_rcvd_cnt        = 0;
#endif /* FEATURE_DSM_MEM_CHK */

  /* Initialise all NSAPI entities */
  for( gas_id=0; gas_id < NUM_GERAN_DATA_SPACES; gas_id++ )
  {
    for( i=0; i < (uint8) GSN_NSAPI_MAX; i++ )
    {
      actv_nsapis[ gas_id ][i]  = NULL;
    }
  }

  /* Initialise all SAPI entities */
  for( gas_id=0; gas_id < NUM_GERAN_DATA_SPACES; gas_id++ )
  {
    for( i=0; i < (uint8) GSN_SAPI_MAX; i++ )
    {
      actv_sapis[gas_id][i]  = NULL;
    }
  }

  /* Initialise LLC SAPIs N201_I and N201_U */

  for( gas_id=0; gas_id < NUM_GERAN_DATA_SPACES; gas_id++ )
  {
    gsn_n201_i[gas_id][GSN_SAPI_3] = gllc_ll_get_default_N201_I( gsn_extern_sapi[GSN_SAPI_3] );
    gsn_n201_u[gas_id][GSN_SAPI_3] = gllc_ll_get_default_N201_U( gsn_extern_sapi[GSN_SAPI_3] );

    gsn_n201_i[gas_id][GSN_SAPI_5] = gllc_ll_get_default_N201_I( gsn_extern_sapi[GSN_SAPI_5] );
    gsn_n201_u[gas_id][GSN_SAPI_5] = gllc_ll_get_default_N201_U( gsn_extern_sapi[GSN_SAPI_5] );

    gsn_n201_i[gas_id][GSN_SAPI_9] = gllc_ll_get_default_N201_I( gsn_extern_sapi[GSN_SAPI_9] );
    gsn_n201_u[gas_id][GSN_SAPI_9] = gllc_ll_get_default_N201_U( gsn_extern_sapi[GSN_SAPI_9] );

    gsn_n201_i[gas_id][GSN_SAPI_11] = gllc_ll_get_default_N201_I( gsn_extern_sapi[GSN_SAPI_11] );
    gsn_n201_u[gas_id][GSN_SAPI_11] = gllc_ll_get_default_N201_U( gsn_extern_sapi[GSN_SAPI_11] );
  }

  sndcp_log_init();

#ifdef FEATURE_DATA_MM

  /* Enable the signals. The signals have to be enabled before they can be used */

#endif /* FEATURE_DATA_MM */

  /* Set SNDCP initialisation flag */
  sndcp_init = TRUE;

  MSG_GERAN_LOW_0("GSN " "ME: SNDCP Initialized");
}


/*===========================================================================
===
===  FUNCTION      GSNDCP_SNSM_GET_MES_BUF()
===
===  DESCRIPTION
===
===    Dynamically allocate sufficient memory from heap to accommodate a
===    message of type gsnsm_mes_type.
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    Pointer to allocated gsnsm_mes_type buffer.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

gsmsn_mes_type *gsndcp_snsm_get_mes_buf( void )
{
  return( GPRS_MEM_MALLOC( sizeof( gsmsn_mes_type ) ) );
}

/*===========================================================================
===
===  FUNCTION      GSN_INT_GET_MES_BUF()
===
===  DESCRIPTION
===
===    Dynamically allocate sufficient memory from heap to accommodate a
===    message of type gsn_internal_msg_t.
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    Pointer to allocated gsnsm_mes_type buffer.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

gsn_internal_msg_t *gsn_int_get_mes_buf( void )
{
  return( GPRS_MEM_MALLOC( sizeof( gsn_internal_msg_t ) ) );
}

/*===========================================================================
===
===  FUNCTION      GSNDCP_SNSM_PUT_MES_BUF()
===
===  DESCRIPTION
===
===    Post the SNSM message as given by the message pointer to the SNDCP
===    command queue.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised.
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

void gsndcp_snsm_put_mes( gsmsn_mes_type *mes_ptr )
{
  gsn_ctrl_msg_t  *adpt_msg;

  GSN_ASSERT( sndcp_init != FALSE );

  if( mes_ptr == NULL )
  {
    MSG_GERAN_ERROR_0("GSN " "NULL cmd SM => SNDCP ");
    return;
  }

  /* Log the message for call flow analysis. */

  switch ( mes_ptr -> mes.prim )
  {
  case GSNSM_ACTIVATE_IND:
    cfa_log_packet_ex
    (
      GS_QUEUE_SNDCP, MS_SNDCP_SM, (uint8)GSNSM_ACTIVATE_IND,
      sizeof(gsnsm_activ_ind_type), (uint8 *)&mes_ptr -> mes.snsm_activ_ind
    );
    break;

  case GSNSM_DEACTIVATE_IND:
    cfa_log_packet_ex
    (
      GS_QUEUE_SNDCP, MS_SNDCP_SM, (uint8)GSNSM_DEACTIVATE_IND,
      sizeof(gsnsm_deactiv_ind_type), (uint8 *)&mes_ptr -> mes.snsm_deactiv_ind
    );
    break;

  case GSNSM_MODIFY_IND:
    cfa_log_packet_ex
    (
      GS_QUEUE_SNDCP, MS_SNDCP_SM, (uint8)GSNSM_MODIFY_IND,
      sizeof(gsnsm_modify_ind_type), (uint8 *)&mes_ptr -> mes.snsm_modify_ind
    );
    break;

  case GSNSM_SEQUENCE_IND:
    cfa_log_packet_ex
    (
      GS_QUEUE_SNDCP, MS_SNDCP_SM, (uint8)GSNSM_SEQUENCE_IND,
      sizeof(gsnsm_seq_ind_type), (uint8 *)&mes_ptr -> mes.snsm_seq_ind
    );
    break;

  default:
    break;
  }

  /* Adapt SM msg so that can be stored in gsn_cmd_q
  */

  /* Allocate memory to convey the addapted message */
  adpt_msg = GPRS_MEM_MALLOC( sizeof( gsn_ctrl_msg_t ) );
  GSN_ASSERT( adpt_msg != NULL );

  /* Message sent by SM */
  adpt_msg->cmd_id = SM_CMD;

  adpt_msg->ctrl_msg.sm_msg = mes_ptr;

  /* Initialize the queue link before putting an item in a queue. */
  (void) q_link(adpt_msg, &(adpt_msg->link) );

  /* Put the item in the queue */
  q_put(&gsn_cmd_q, &(adpt_msg->link));

#ifdef FEATURE_DATA_MM
  /* Set the signal for this queue */
  COMP_SET_SIGNAL(host_cmd_queue_sig);
#else
  /* Set the signal for this queue */
  (void)rex_set_sigs(host_tcb_ptr, host_cmd_queue_sig);
#endif /* FEATURE_DATA_MM */

}


/*===========================================================================
===
===  FUNCTION      GSN_INT_PUT_MES_BUF()
===
===  DESCRIPTION
===
===    Post the SNDCP internal message as given by the message pointer
===    to the SNDCP command queue.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised.
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

void gsn_int_put_mes( gsn_internal_msg_t *mes_ptr )
{
  gsn_ctrl_msg_t  *adpt_msg;

  GSN_ASSERT( sndcp_init != FALSE );

  /* Adapt SNDCP msg so that can be stored in gsn_cmd_q
  */

  /* Allocate memory to convey the adapted message */
  adpt_msg = GPRS_MEM_MALLOC( sizeof( gsn_ctrl_msg_t ) );
  GSN_ASSERT( adpt_msg != NULL  );

  /* Message sent by SNDCP */
  adpt_msg->cmd_id = GSN_CMD;

  adpt_msg->ctrl_msg.gsn_msg = mes_ptr;

  /* Initialize the queue link before putting an item in a queue. */
  (void) q_link(adpt_msg, &(adpt_msg->link) );

  /* Put the item in the queue */
  q_put(&gsn_cmd_q, &(adpt_msg->link));

#ifdef FEATURE_DATA_MM
  /* Set the signal for this queue */
  COMP_SET_SIGNAL(host_cmd_queue_sig);
#else
  /* Set the signal for this queue */
  (void) rex_set_sigs(host_tcb_ptr, host_cmd_queue_sig);
#endif /* FEATURE_DATA_MM */
}


/*===========================================================================
===
===  FUNCTION      GSNDCP_SNSM_SEQ_IND()
===
===  DESCRIPTION
===
===    For use by SM as a static method of initiating an SNSM_SEQUENCE_IND
===    SNSM_SEQUENCE_RSP message exchange, but obtaining the Receive NPDU
===    number immediately by return, without waiting for the
===    SNSM_SEQUENCE_RSP message.
===
===    This is to accomodate some architectural restrictions in the GMM/SM
===    architecture.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised.
===
===  RETURN VALUE
===
===    The current Receive NPDU value for the given NSAPI address.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

uint16 gsndcp_snsm_seq_ind
(
  uint32  temp_tlli,           /* Temporary Logical Link Identifier*/
  uint8   nsapi,               /* NSAP Identifier Address          */
  uint8   sgsn_rcv_npdu_num,   /* Receive N-PDU number             */
  sys_modem_as_id_e_type as_id /* Access Stratum ID                */
)
{
  gsmsn_mes_type *msg_ptr;
  uint16         rcv_npdu_num = 0;
  uint8          int_nsapi;
  gas_id_t       gas_id       = geran_map_nas_id_to_gas_id( (sys_modem_as_id_e_type) as_id );



  /* Check if valid NSAPI */

  if (GSN_VALID_NSAPI(nsapi))
  {
    int_nsapi = (uint8)GSN_INTERN_NSAPI( nsapi );

    if (actv_nsapis[ gas_id ][ int_nsapi ] == NULL)
    {
      MSG_GERAN_ERROR_1("GSN ""GSN: INACTIVE NSAPI %d",nsapi);
    }
    else
    {
      /* Send an GSNSM_SEQUENCE_IND message to SNDCP for normal processing. */

      msg_ptr = gsndcp_snsm_get_mes_buf( );
      if(msg_ptr)
      {
        msg_ptr -> mes.snsm_seq_ind.prim          = GSNSM_SEQUENCE_IND;
        msg_ptr -> mes.snsm_seq_ind.tlli          = temp_tlli;
        msg_ptr -> mes.snsm_seq_ind.nsapi         = nsapi;
        msg_ptr -> mes.snsm_seq_ind.rcv_npdu_num  = sgsn_rcv_npdu_num;
#ifdef FEATURE_DUAL_SIM
        msg_ptr -> mes.snsm_seq_ind.as_id         = as_id;
#endif /* FEATURE_DUAL_SIM */

        gsndcp_snsm_put_mes( msg_ptr );

        /* Obtain local Receive NPDU Number for return to caller. */

        GSN_LOCK();

        if ( actv_nsapis[ gas_id ][int_nsapi]->op_mode == ACK )
          rcv_npdu_num = actv_nsapis[ gas_id ][int_nsapi]->dle.ack_rcv_npdu;
        else
          rcv_npdu_num = actv_nsapis[ gas_id ][int_nsapi]->dle.unack_rcv_npdu;

        GSN_UNLOCK();
      }
      else
      {
        MSG_GERAN_ERROR_1("GSN ""Buffer not available %d",nsapi);
      }
    }
  }
  else
  {
    MSG_GERAN_ERROR_1("GSN ""GSN: INVALID NSAPI %d",nsapi);
  }

  return ( rcv_npdu_num );

} /* end of gsndcp_snsm_seq_ind() */


/*===========================================================================
FUNCTION GSNDCP_TO_SNDCP_EACH_ENQUEUE_CB

DESCRIPTION
  This callback function is called when a packet is enqueued on the
  SNDCP UL Watermark.

PARAMETERS
   wm            : Pointer to the Tx watermark registered with SNDCP
   callback_data : NSAPI registered with the watermark callback

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -e818*/
void gsndcp_to_sndcp_each_enqueue_cb
(
  dsm_watermark_type *wm,
  /*lint -esym(715,wm) */
  void*              callback_data
)
{
  uint8    nsapi;
  uint8    external_nsapi;
  gas_id_t gas_id;

  /* To remove compiler warnings */
  uint32   rcvd_callback_data = (uint32)callback_data;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the NSAPI */
  nsapi          = (uint8)(rcvd_callback_data & 0xFF);

  /* Get GAS_ID */
  gas_id         = (gas_id_t)((rcvd_callback_data >> 8 ) & 0xFF);
  gas_id         = check_gas_id( gas_id );

  /* Get external NSAPI */
  external_nsapi = GSN_EXTERN_NSAPI( nsapi );

  if ( GSN_VALID_NSAPI(external_nsapi) )
  {
    if ( actv_nsapis[ gas_id ][nsapi] != NULL )
    {
      /* Set the TX DATA Signal for the nsapi */
      COMP_SET_SIGNAL(actv_nsapis[ gas_id ][nsapi]->to_gsndcp_sig);
    }
    else
    {
      MSG_GERAN_MED_1("GSN ""Invalid NSAPI %d", external_nsapi);
    }
  }
  else
  {
    MSG_GERAN_MED_1("GSN ""Invalid NSAPI %d", external_nsapi);
  }

} /* gsndcp_to_sndcp_each_enqueue_cb() */

/*===========================================================================
FUNCTION GSNDCP_TO_SNDCP_GONE_EMPTY_CB

DESCRIPTION
  This callback function is called when all the data is dequeued from the
  SNDCP UL Watermark.

PARAMETERS
   wm            : pointer to the watermark
   callback_data : Call instance registered with the watermark callback


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -e818*/
void gsndcp_to_sndcp_gone_empty_cb
(
  dsm_watermark_type *wm,
  /*lint -esym(715,wm) */
  void*              callback_data
)
{
  uint8    nsapi;
  uint8    external_nsapi;
  gas_id_t gas_id;

  /* To remove compiler warnings */
  uint32   rcvd_callback_data = (uint32)callback_data;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the NSAPI */
  nsapi          = (uint8)(rcvd_callback_data & 0xFF);

  /* Get GAS_ID */
  gas_id         = (gas_id_t)((rcvd_callback_data >> 8 ) & 0xFF);

  /* Get external NSAPI */
  external_nsapi = GSN_EXTERN_NSAPI( nsapi );

  if ( GSN_VALID_NSAPI(external_nsapi) )
  {
    if ( actv_nsapis[ gas_id ][nsapi] != NULL )
    {
      /* Clear the TX DATA Signal for the nsapi */
      COMP_CLR_SIGNAL(actv_nsapis[ gas_id ][nsapi]->to_gsndcp_sig);
    }
    else
    {
      MSG_GERAN_MED_1("GSN ""Invalid NSAPI %d", external_nsapi);
    }
  }
  else
  {
    MSG_GERAN_MED_1("GSN ""Invalid NSAPI %d", external_nsapi);
  }

} /* gsndcp_to_sndcp_gone_empty_cb() */

/*===========================================================================
FUNCTION DSGPSD_TO_SNDCP_SIG_HDLR

DESCRIPTION
  This function is called when the Tx DATA Signal is processed.

PARAMETERS
    Sig - Signal that was processed
    user_data_ptr - NSAPI for the signal that was processed.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
boolean gsndcp_to_sndcp_sig_hdlr
(
  comp_sig_enum_type sig,
  void          *user_data_ptr
)
{

  /* To remove compiler warnings */
  uint32   rcvd_callback_data = (uint32)user_data_ptr;

  /* Get the NSAPI */
  uint8    nsapi    = (uint8)(rcvd_callback_data & 0xFF );

  /* Get GAS_ID */
  gas_id_t gas_id   = (gas_id_t)( (rcvd_callback_data >> 8 ) & 0xFF);

  dsm_item_type *item_ptr;

  /* Get external NSAPI */
  uint8 external_nsapi = GSN_EXTERN_NSAPI( nsapi );


  if ( !GSN_VALID_NSAPI(external_nsapi) )
  {
    MSG_GERAN_MED_1("GSN ""Invalid NSAPI %d",external_nsapi);
    return FALSE;
  }

  if ( actv_nsapis[ gas_id ][nsapi]== NULL )
  {
    MSG_GERAN_MED_1("GSN ""Invalid NSAPI %d",external_nsapi);
    return FALSE;
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((!(actv_nsapis[ gas_id ][nsapi]->ule.susp_flag &
       GSN_UL_SUSP_ALL_REASONS_MASK)) &&
      (actv_nsapis[ gas_id ][nsapi]->to_gsndcp_wm != NULL))
  {
    /* Dequeue data from sndcp WM.*/
    item_ptr = (dsm_item_type *)dsm_dequeue(
                   actv_nsapis[ gas_id ][nsapi]->to_gsndcp_wm);

    /* Call function to process the Uplink packet.*/
    gsndcp_pdp_send_ul_npdu(gas_id, GSN_EXTERN_NSAPI(nsapi), &item_ptr);

    /* nsapi could get deactivated if SNSM_DEACTIVATE_IND is processed during
       cmd_queue processing in gsndcp_pdp_send_ul_npdu(). If nsapi is still
       valid then proceed with UL transfer. Otherwise, free PDU and return
       FALSE.
    */

    if ( actv_nsapis[ gas_id ][nsapi]== NULL )
    {
      MSG_GERAN_MED_1("GSN ""NSAPI %d got invalid",external_nsapi);

      /* Free dsm item */
      GPRS_PDU_FREE( &item_ptr );

      return FALSE;
    }

    /* If there is more data in the watermark, set the signal again.*/
    if (actv_nsapis[ gas_id ][nsapi]->to_gsndcp_wm->current_cnt != 0)
    {
      COMP_SET_SIGNAL(sig);
    }
  }
#if 0
  else
  {
    MSG_GERAN_ERROR_3("GSN ""ule.susp_flag %d, to_gsndcp_wm ptr %p ",
                          actv_nsapis[ gas_id ][nsapi]->ule.susp_flag,
                          actv_nsapis[ gas_id ][nsapi]->to_gsndcp_wm,
                          0);
  }
#endif

  return TRUE;

} /* gsndcp_to_sndcp_sig_hdlr() */



/*===========================================================================
FUNCTION GSNDCP_TO_SNDCP_DEFAULT_SIG_HDLR

DESCRIPTION
  Default signal handler when the Tx DATA Signal is processed.

PARAMETERS
    Sig - Signal that was processed
    user_data_ptr - NSAPI for the signal that was processed.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
boolean gsndcp_to_sndcp_default_sig_hdlr
(
  comp_sig_enum_type sig,
  void *user_data_ptr
)
{
  NOTUSED(user_data_ptr);
  MSG_GERAN_ERROR_1("GSN ""PS->COMP sig %d, default hdlr called", sig);
  return TRUE;
} /* gsndcp_to_sndcp_default_sig_hdlr() */




/*===========================================================================
FUNCTION GSNDCP_CHECK_DATA_IN_SMD_TO_SNDCP_WM

DESCRIPTION
  This function is checks if there is any data in the SMD->SNDCP WM. If there
  is any, then process.

PARAMETERS
  nsapi.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/

void gsndcp_check_data_in_smd_to_sndcp_wm(gas_id_t gas_id, const uint8 nsapi)
{

  uint8 external_nsapi;

  /* Get external NSAPI */
  external_nsapi = GSN_EXTERN_NSAPI(nsapi);

  if ( GSN_VALID_NSAPI(external_nsapi) )
  {
    if ( actv_nsapis[ gas_id ][nsapi] != NULL )
    {
      if(actv_nsapis[ gas_id ][nsapi]->to_gsndcp_wm)
      {
        if (!dsm_is_wm_empty(actv_nsapis[ gas_id ][nsapi]->to_gsndcp_wm))
        {
          gsndcp_to_sndcp_each_enqueue_cb(actv_nsapis[ gas_id ][nsapi]->to_gsndcp_wm,
                                          (void *)( ( gas_id << 8 ) | ( nsapi ) ));
        }
      }
    }
    else
    {
      MSG_GERAN_MED_1("GSN ""Invalid NSAPI %d", external_nsapi);
    }
  }
  else
  {
    MSG_GERAN_MED_1("GSN ""Invalid NSAPI %d", external_nsapi);
  }

} /* gsndcp_check_data_in_smd_to_sndcp_wm()*/

/*===========================================================================
FUNCTION GSNDCP_REGISTER_WM_HDLR

DESCRIPTION
  This is the handler function for the register_wm command posted
  from the DS mode handler.

PARAMETERS
  cmd_ptr - Pointer to the information for registering watermarks
            with SNDCP

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if registration succeeds
  FALSE - if parameter validation fails

SIDE EFFECTS
  None
===========================================================================*/
boolean  gsndcp_register_wm_hdlr
(
  const comp_cmd_type *cmd_ptr
)
{
  uint8               nsapi;
  gas_id_t            gas_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Make sure the right command was received */
  if (cmd_ptr->hdr.cmd_id != GSNDCP_REG_WM)
  {
    MSG_GERAN_ERROR_1("GSN ""cmd id is not REG_WM %d", cmd_ptr->hdr.cmd_id);
    // ASSERT(0);
    return FALSE;
  }

  /* Get GAS_ID */
  gas_id = geran_map_nas_id_to_gas_id( cmd_ptr->cmd.gsndcp_reg_wm.subs_id );

  /* Check for GAS_ID validity */
  if ( gas_id > GERAN_ACCESS_STRATUM_ID_MAX )
  {
    MSG_GERAN_ERROR_0("GSN ""Invalid GAS_ID %d");
    return FALSE;
  }

  /* Get the right NSAPI */
  nsapi = GSN_INTERN_NSAPI(cmd_ptr->cmd.gsndcp_reg_wm.nsapi);

  if ( !GSN_VALID_NSAPI( cmd_ptr->cmd.gsndcp_reg_wm.nsapi ))
  {
    MSG_GERAN_ERROR_1("GSN ""Invalid NSAPI %d", cmd_ptr->cmd.gsndcp_reg_wm.nsapi);
    return FALSE;
  }

  if ( actv_nsapis[ gas_id ][nsapi] == NULL )
  {
    MSG_GERAN_ERROR_1("GSN ""Inactive NSAPI %d", cmd_ptr->cmd.gsndcp_reg_wm.nsapi);
    return FALSE;
  }

  /* Check for GAS_ID validity */
#ifdef FEATURE_DUAL_SIM
  if ( actv_nsapis[ gas_id ][nsapi]->gas_id != gas_id  )
#else
  if ( GERAN_ACCESS_STRATUM_ID_1 != gas_id  )
#endif /*FEATURE_DUAL_SIM*/
  {
    MSG_GERAN_ERROR_0("GSN ""Unexpected GAS_ID %d");
    return FALSE;
  }


  if((cmd_ptr->cmd.gsndcp_reg_wm.to_sndcp_wm_ptr == NULL) ||
     (cmd_ptr->cmd.gsndcp_reg_wm.from_sndcp_wm_ptr == NULL) ||
     (cmd_ptr->cmd.gsndcp_reg_wm.from_sndcp_post_rx_func_ptr == NULL))
  {
    MSG_GERAN_ERROR_0("GSN ""NULL ptr in wm reg");
    return FALSE;
  }

  /* Register both tx and rx watermarks and the callbacks with SNDCP */
  actv_nsapis[ gas_id ][nsapi]->to_gsndcp_wm =
                             cmd_ptr->cmd.gsndcp_reg_wm.to_sndcp_wm_ptr;
  actv_nsapis[ gas_id ][nsapi]->from_gsndcp_wm =
                            cmd_ptr->cmd.gsndcp_reg_wm.from_sndcp_wm_ptr;

  actv_nsapis[ gas_id ][nsapi]->from_gsndcp_rx_func_parm =
                      cmd_ptr->cmd.gsndcp_reg_wm.from_sndcp_rx_func_parm;
  actv_nsapis[ gas_id ][nsapi]->from_gsndcp_post_rx_func_ptr =
                      cmd_ptr->cmd.gsndcp_reg_wm.from_sndcp_post_rx_func_ptr;

  /* Include GAS_ID and NSAPI combination as input parameter for callback functions */
  actv_nsapis[ gas_id ][nsapi]->to_gsndcp_wm->gone_empty_func_data   =
                                         (void*) ( ( gas_id << 8 ) | ( nsapi ) );
  actv_nsapis[ gas_id ][nsapi]->to_gsndcp_wm->gone_empty_func_ptr    =
                                         gsndcp_to_sndcp_gone_empty_cb;

  actv_nsapis[ gas_id ][nsapi]->to_gsndcp_wm->each_enqueue_func_data =
                                         (void*) ( ( gas_id << 8 ) | ( nsapi ) );
  actv_nsapis[ gas_id ][nsapi]->to_gsndcp_wm->each_enqueue_func_ptr  =
                                         gsndcp_to_sndcp_each_enqueue_cb;


  /* Set the handler for the PS to SNDCP signal for the call. */
  (void)comp_set_sig_handler(
                actv_nsapis[ gas_id ][nsapi]->to_gsndcp_sig,
                gsndcp_to_sndcp_sig_hdlr,
                (void *) ( ( gas_id << 8 ) | ( nsapi ) ) );

  /* Check if data has already been enqueued on the UL WM, and if yes, process it. */

  if (!dsm_is_wm_empty(actv_nsapis[ gas_id ][nsapi]->to_gsndcp_wm))
  {
    gsndcp_to_sndcp_each_enqueue_cb( actv_nsapis[ gas_id ][nsapi]->to_gsndcp_wm,
                                     (void *)( ( gas_id << 8 ) | ( nsapi ) )   );
  }
  MSG_GERAN_HIGH_0("GSN ""SNDCP-SMD WM registered");

  return TRUE;
} /* gsndcp_register_wm_hdlr() */

/*===========================================================================
FUNCTION GSNDCP_DEREGISTER_WM_HDLR

DESCRIPTION
  This function de-registers the WMs between SMD & GSNDCP.

PARAMETERS
  cmd_ptr - Pointer to the information for de-registering watermarks
            with SNDCP

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void gsndcp_deregister_wm_hdlr
(
  const comp_cmd_type *cmd_ptr
)
{
  uint8    nsapi;
  gas_id_t gas_id;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the right command was received */
  if (cmd_ptr->hdr.cmd_id != GSNDCP_DEREG_WM)
  {
    MSG_GERAN_ERROR_1("GSN ""cmd id is not REG_WM %d", cmd_ptr->hdr.cmd_id);
    ASSERT(0);
    return;
  }

  /* Get GAS_ID */
  gas_id = geran_map_nas_id_to_gas_id( cmd_ptr->cmd.gsndcp_dereg_wm.subs_id );

  /* Check for GAS_ID validity */
  if ( gas_id > GERAN_ACCESS_STRATUM_ID_MAX )
  {
    MSG_GERAN_ERROR_0("GSN ""Invalid GAS_ID %d");
    return;
  }

  /* Get the right NSAPI */
  nsapi = GSN_INTERN_NSAPI(cmd_ptr->cmd.gsndcp_dereg_wm.nsapi);
  MSG_MED("GSNDCP dereg wm, nsapi: %d", nsapi, 0, 0);

  /* If there was no active NSAPI, return immediately */

  if ( !GSN_VALID_NSAPI( cmd_ptr->cmd.gsndcp_dereg_wm.nsapi ) )
  {
    MSG_GERAN_MED_1("GSN ""NSAPI %d is invalid ", cmd_ptr->cmd.gsndcp_dereg_wm.nsapi);
    return;
  }


  if ( actv_nsapis[ gas_id ][nsapi] == NULL )
  {
    MSG_GERAN_MED_1("GSN ""NSAPI %d is inactive ", cmd_ptr->cmd.gsndcp_dereg_wm.nsapi);
    return;
  }


  /* Check for GAS_ID validity */
#ifdef FEATURE_DUAL_SIM
  if ( actv_nsapis[ gas_id ][nsapi]->gas_id != gas_id  )
#else
  if ( GERAN_ACCESS_STRATUM_ID_1 != gas_id  )
#endif /*FEATURE_DUAL_SIM*/
  {
    MSG_GERAN_ERROR_0("GSN ""Unexpected GAS_ID %d");
    return;
  }

  /* GSNDCP can be in down state when sndcp receives this, hence this check */
  if ((actv_nsapis[ gas_id ][nsapi]->to_gsndcp_wm != NULL) &&
      (actv_nsapis[ gas_id ][nsapi]->from_gsndcp_wm != NULL) &&
      (actv_nsapis[ gas_id ][nsapi]->from_gsndcp_post_rx_func_ptr != NULL))
  {
    /* Reset both tx and rx wm to NULL.*/
    actv_nsapis[ gas_id ][nsapi]->to_gsndcp_wm->each_enqueue_func_ptr = NULL;
    actv_nsapis[ gas_id ][nsapi]->to_gsndcp_wm->gone_empty_func_ptr = NULL;
    actv_nsapis[ gas_id ][nsapi]->to_gsndcp_wm = NULL;
    actv_nsapis[ gas_id ][nsapi]->from_gsndcp_wm = NULL;
    actv_nsapis[ gas_id ][nsapi]->from_gsndcp_post_rx_func_ptr = NULL;
  }

  /* Clear SMD->SNDCP signal handler. */
  (void)comp_set_sig_handler(
                  actv_nsapis[ gas_id ][nsapi]->to_gsndcp_sig,
                  gsndcp_to_sndcp_default_sig_hdlr,
                  (void *) ( ( gas_id << 8 ) | ( nsapi ) ) );

  MSG_GERAN_HIGH_0("GSN ""SNDCP-SMD WM deregistered");
} /* gsndcp_deregister_wm_hdlr() */


/*===========================================================================
===
===  FUNCTION      GSNDCP_PDP_REGISTER_REQ()
===
===  DESCRIPTION
===
===    Sends an internal message to SNDCP's command queue to registers a
===    PDP's uplink packet flow suspension and resumption callback functions
===    and associated parameters with the given NSAPI.
===
===    Registers a PDP's downlink packet receive callback function with the
===    given NSAPI.
===
===    Also indicates whether data is to be ciphered by LLC. The ciphering
===    requirement is applicable only to those PDP contexts for which SM has
===    indicated a QoS reliability class which requires LLC to send data as
===    unacknowledged.
===
===    The calling client is provided with a facility to register a
===    confirmation callback function which identifies the confirmation by
===    client context identifier and also indicates registration success.
===    Should the client not require a confirmation callback then the ptr
===    should be set as NULL.
===
===  DEPENDENCIES
===
===    Must be called after a PDP context has been activated by SM (Session
===    Management) for the given NSAPI address.
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

void gsndcp_pdp_register_req
(
  uint8                   nsapi,
  wm_cb_type              pdp_ul_suspend_fnc_ptr,
  wm_cb_type              pdp_ul_resume_fnc_ptr,
  void                    ( *pdp_dl_fnc_ptr )( void* ds_context, dsm_item_type **npdu ),
  void                    *ds_context,
  boolean                 cipher,
  void                    ( *pdp_reg_cnf_fnc_ptr )( void* ds_context, boolean success )
#ifdef FEATURE_DUAL_SIM
  , sys_modem_as_id_e_type  as_id
#endif
)
{
  gsn_internal_msg_t *msg_ptr;

  /* Obtain an internal message resource */

  msg_ptr = gsn_int_get_mes_buf();
  GSN_ASSERT( msg_ptr != NULL );

  /* Construct message and post to command queue for processing. */

  msg_ptr->cmd_id                                     = PDP_REG_REQ;
  msg_ptr->gsn_cmd.pdp_reg_req.nsapi                  = nsapi;
  msg_ptr->gsn_cmd.pdp_reg_req.pdp_ul_suspend_fnc_ptr = pdp_ul_suspend_fnc_ptr;
  msg_ptr->gsn_cmd.pdp_reg_req.pdp_ul_resume_fnc_ptr  = pdp_ul_resume_fnc_ptr;
  msg_ptr->gsn_cmd.pdp_reg_req.pdp_dl_fnc_ptr         = pdp_dl_fnc_ptr;
  msg_ptr->gsn_cmd.pdp_reg_req.ds_context             = ds_context;
  msg_ptr->gsn_cmd.pdp_reg_req.cipher                 = cipher;
  msg_ptr->gsn_cmd.pdp_reg_req.pdp_reg_cnf_fnc_ptr    = pdp_reg_cnf_fnc_ptr;
#ifdef FEATURE_DUAL_SIM
  msg_ptr->gsn_cmd.pdp_reg_req.gas_id                 = geran_map_nas_id_to_gas_id( as_id );
#else
  msg_ptr->gsn_cmd.pdp_reg_req.gas_id                 = GERAN_ACCESS_STRATUM_ID_1;
#endif /*FEATURE_DUAL_SIM*/

  gsn_int_put_mes( msg_ptr );

} /* end of gsndcp_pdp_register_req() */


/* Legacy PDP Registration Support */

boolean gsndcp_pdp_register
(
  uint8                   nsapi,
  wm_cb_type              pdp_ul_suspend_fnc_ptr,
  wm_cb_type              pdp_ul_resume_fnc_ptr,
  void                    ( *pdp_dl_fnc_ptr )( void* ds_context, dsm_item_type **npdu ),
  void                    *ds_context,
  boolean                 cipher,
  sys_modem_as_id_e_type  as_id
)
{
  gsndcp_pdp_register_req
  (
    nsapi,
    pdp_ul_suspend_fnc_ptr,
    pdp_ul_resume_fnc_ptr,
    pdp_dl_fnc_ptr,
    ds_context,
    cipher,
    NULL
#ifdef FEATURE_DUAL_SIM
    ,as_id
#endif /*FEATURE_DUAL_SIM*/
  );

  return ( TRUE );

} /* end of gsndcp_pdp_register() */


/*===========================================================================
===
===  FUNCTION      GSNDCP_UL_PDU_PRIORITY_HDLR()
===
===  DESCRIPTION
===
===    Determines priority of UL IP packets.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Prioerity of IP packet.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

geran_pdu_priority_t gsndcp_ul_pdu_priority_hdlr( dsm_item_type *npdu )
{
    dsm_item_type        *npdu_copy;
    uint8                ip_datagram[80];
    uint16               ip_total_len;
    uint8                ip_version;
    uint8                ip_hdr_len     = 0;
    uint8                ip_prtcol_type = 0;
    uint8                tcp_hdr_ctrl   = 0;
    geran_pdu_priority_t pdu_priority   = GPDU_LOW_PRIORITY;

    /* Duplicate only the top 80 bytes of the N-PDU. These include 20 bytes (IPV4)
    ** or 40 bytes (IPV6) of IP header plus the 20 bytes TCP header (without optional
    ** fields).
    */
    ip_total_len = GPRS_PDU_LENGTH( npdu );

    if ( ip_total_len > 80 )
      ip_total_len = 80;

    npdu_copy = gprs_pdu_duplicate( npdu, 0, ip_total_len );

    if( npdu_copy != NULL )
    {
      if( !gprs_pdu_strip_head( &npdu_copy, ip_datagram, ip_total_len ) )
      {
        MSG_GERAN_ERROR_0("GSN " "GSNDCP PDU priority hdlr: gprs_pdu_strip_head() failed");
      }
      else
      {

        ip_version = ( ( *ip_datagram ) >> 4 );

        if ( ip_version == 4 ) /* IPv4 */
        {
          ip_hdr_len          = (*ip_datagram & 0x0f ) * 4;
          ip_total_len        = (((uint16) ip_datagram[2] ) << 8 ) | ((uint16) ip_datagram[3] ) ;
          ip_prtcol_type      = (uint8) ip_datagram[9];

          if ( ip_prtcol_type == 1) /* ICMP protocol */
          {
            pdu_priority = GPDU_HIGH_PRIORITY;
          }
          else if( ip_prtcol_type == 6 ) /* TCP protocol */
          {

            tcp_hdr_ctrl = (uint8) ip_datagram[ip_hdr_len + 13];


            /* If ACK, PUSH, RESET, SYN, FIN, and PDU is less than 100 octets, then set high priority. */

            if ( ( ( tcp_hdr_ctrl & 0x10 ) ||
                   ( tcp_hdr_ctrl & 0x01 ) ||
                   ( tcp_hdr_ctrl & 0x02 ) ||
                   ( tcp_hdr_ctrl & 0x04 ) ||
                   ( tcp_hdr_ctrl & 0x08 )    ) &&
                   ( ip_total_len < 100  )
               )
            {
              pdu_priority = GPDU_HIGH_PRIORITY;
            }
          }
          else
          {

            /* All other protocol types are lower priority. */

            pdu_priority = GPDU_LOW_PRIORITY;

          }
        }
        else if ( ip_version == 6 )  /* IPv6 */
        {

          ip_hdr_len          = 40;
          ip_total_len        = (((uint16) ip_datagram[4] ) << 8 ) | ((uint16) ip_datagram[5] ) ;
          ip_prtcol_type      = (uint8) ip_datagram[6];

          if ( ip_prtcol_type == 1) /* ICMP protocol */
          {
            pdu_priority = GPDU_HIGH_PRIORITY;
          }
          else if( ip_prtcol_type == 6 ) /* TCP protocol */
          {

            tcp_hdr_ctrl = (uint8) ip_datagram[ip_hdr_len + 13];


            /* If ACK, PUSH, RESET, SYN, FIN, and PDU is less than 100 octets, then set high priority. */

            if ( ( ( tcp_hdr_ctrl & 0x10 ) ||
                   ( tcp_hdr_ctrl & 0x01 ) ||
                   ( tcp_hdr_ctrl & 0x02 ) ||
                   ( tcp_hdr_ctrl & 0x04 ) ||
                   ( tcp_hdr_ctrl & 0x08 )    ) &&
                   ( ip_total_len < 100  )
               )
            {
              pdu_priority = GPDU_HIGH_PRIORITY;
            }
          }
          else
          {

            /* All other protocol types are lower priority. */

            pdu_priority = GPDU_LOW_PRIORITY;

          }

        }
        else
        {
          MSG_GERAN_ERROR_0("GSN " "GSNDCP PDU priority handler: Invalid IP version");
        }

        MSG_GERAN_LOW_3("GSN " "GSNDCP PDU priority hdlr DBG: ip_hdr_len: %d, ip_total_len: %d, ip_prtcol_type: %d",
          ip_hdr_len,
          ip_total_len,
          ip_prtcol_type );

        MSG_GERAN_LOW_2("GSN " "GSNDCP PDU priority hdlr DBG: tcp_hdr_ctrl: 0x%x, pdu_priority: %d ", tcp_hdr_ctrl,
          pdu_priority );
      }
    }
    else
    {
      MSG_GERAN_ERROR_0("GSN " "GSNDCP PDU priority handler: run out of dup dsm items");
    }

    return pdu_priority;

} /* gsndcp_ul_pdu_priority_hdlr() */


/*===========================================================================
===
===  FUNCTION      GSN_TIMER_CALLBACK()
===
===  DESCRIPTION
===
===    Sends an internal message to SNDCP's command queue to indicate the
===    expiry of sgsn initiated rel class 2 link establishment timer.
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

void gsn_timer_callback( uint32 gasid_nsapi_and_timer_id )
{
  gsn_internal_msg_t *msg_ptr;

  gas_id_t    gas_id   = (gas_id_t)((gasid_nsapi_and_timer_id >> 24) & 0xFF);
  gsn_nsapi_t nsapi    = (gsn_nsapi_t)((gasid_nsapi_and_timer_id >> 16) & 0xFF);
  uint16      timer_id = (uint16)(gasid_nsapi_and_timer_id & 0xFFFF);


  /* Obtain an internal message resource */

  msg_ptr = gsn_int_get_mes_buf();
  GSN_ASSERT( msg_ptr != NULL );

  /* Construct message and post to command queue for processing. */

  msg_ptr->cmd_id               = TIMER_CMD;
  msg_ptr->gsn_cmd.timer.tmr_id = (gsn_timer_id_t) timer_id;
  msg_ptr->gsn_cmd.timer.nsapi  = nsapi;
  msg_ptr->gsn_cmd.timer.gas_id = gas_id;

  gsn_int_put_mes( msg_ptr );

} /* gsn_timer_callback( ) */


/*===========================================================================
===
===  FUNCTION      GSNDCP_PROCESS_CMD_QUEUE()
===
===  DESCRIPTION
===
===    To be called by the hosting task on activation of the
===    host_cmd_queue_sig that was registered with SNDCP on initialization.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised. The host task is expected to clear the
===    associated REX signal before calling this handler.
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

boolean gsndcp_process_cmd_queue
(
#ifdef FEATURE_DATA_MM
  comp_sig_enum_type sig,
  void             *pkt_instance
#else
 void
#endif /* FEATURE_DATA_MM */
)
{
  gsn_ctrl_msg_t        *adpt_msg_ptr;
  gprs_ll_dl_sig_mes_t  *ll_msg_ptr;
  gsmsn_mes_type        *sm_msg_ptr;
  gsn_internal_msg_t    *gsn_msg_ptr;
  gprs_ll_ul_sig_mes_t  *ul_mes;
  gsn_sapi_t            sapi;
  gas_id_t              gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;


#ifdef FEATURE_DATA_MM
  NOTUSED(sig);
  NOTUSED(pkt_instance);
#endif

  GSN_ASSERT( sndcp_init != FALSE );

  /* Process SM, LLC or SNDCP control primitives
  */
  while( q_cnt(&gsn_cmd_q) )
  {
    /* Read one item from the queue that holds the control primitives */
    adpt_msg_ptr = (gsn_ctrl_msg_t*) q_get( &gsn_cmd_q );

    if( adpt_msg_ptr == NULL )
    {
      MSG_GERAN_ERROR_0("GSN ""q_get() returned NULL");
    }
    /* Determine the sender of the primitive */
    else switch( adpt_msg_ptr->cmd_id )
    {
    case ( LL_CMD ):

      ll_msg_ptr = adpt_msg_ptr->ctrl_msg.ll_msg;

      gas_id = gsn_ll_ctrl_prim_get_gas_id( ll_msg_ptr );

      if ( gas_id >  GERAN_ACCESS_STRATUM_ID_MAX )
      {

        /* Free PDU if carrying XID */
        gsn_ll_ctrl_prim_free_xid_pdu( ll_msg_ptr );

        MSG_GERAN_ERROR_0("GSN ""Invalid GAS_ID: %d in LL msg");

      }
      /* Check if SAPI number is valid and if SAPI has been activated */
      else if( ( GSN_VALID_SAPI(ll_msg_ptr->sapi) ) &&
               ( actv_sapis[gas_id][gsn_intern_sapi[ll_msg_ptr->sapi]] != NULL) )
      {
        /* Translate LLC SAPI number to SNDCP internal SAPI number */
        sapi = (gsn_sapi_t) gsn_intern_sapi[ll_msg_ptr->sapi];

        /* Process the LL control primitive */
        gsn_ll_ctrl_prim_hdlr( gas_id, &ll_msg_ptr->mes, sapi);

      }
      else
      {
        /* The SAPI is not active. The only valid message from LLC at
        ** this stage is LL_XID_IND with no XID pdu, to inform SNDCP
        ** about the N201_I and N201_U values.
        */
        if( (ll_msg_ptr->mes.prim == LL_XID_IND)              &&
            (ll_msg_ptr->mes.xid_ind_mes.l3_xid_req == NULL)  &&
            (ll_msg_ptr->mes.xid_ind_mes.n201_i)              &&
            (ll_msg_ptr->mes.xid_ind_mes.n201_u) )
        {
          /* Get the N201_I and N201_U values */
          if( GSN_VALID_SAPI(ll_msg_ptr->sapi) )
          {
            gsn_n201_i[gas_id][gsn_intern_sapi[ll_msg_ptr->sapi]] =
                            ll_msg_ptr->mes.xid_ind_mes.n201_i;
            gsn_n201_u[gas_id][gsn_intern_sapi[ll_msg_ptr->sapi]] =
                            ll_msg_ptr->mes.xid_ind_mes.n201_u;
           /* MSG_GERAN_HIGH_1_G("GSN ""LL %d: Rcvd LL_XID_IND",ll_msg_ptr->sapi); */
          }
          else
            MSG_GERAN_ERROR_1("GSN ""Invalid SAPI: %d",ll_msg_ptr->sapi);
        }
        else
        {
          /* It is possible to rcv an LL_RESET_IND on an Inactive SAPI.
          ** Do not print a message in this case.
          */
          if( ll_msg_ptr->mes.prim != LL_RESET_IND)
          {
            MSG_GERAN_HIGH_1("GSN ""Inactive SAPI: %d",ll_msg_ptr->sapi);
          }

          gsn_ll_ctrl_prim_free_xid_pdu( ll_msg_ptr );

          /* For the  following two LL DL primitives, inform LLC about the
          ** inactive SAPI so that LLC state machine can get out of the
          ** "await response" state.
          **/
          if( (ll_msg_ptr->mes.prim == LL_XID_IND)     ||
              (ll_msg_ptr->mes.prim == LL_ESTABLISH_IND) )
          {
            /* Get buffer to convey the message */
            ul_mes = gllc_ll_ul_sig_get_mes_buf( );
            GSN_ASSERT( ul_mes != NULL );

            /* Load msg */
            ul_mes->sapi                        = ll_msg_ptr->sapi;
            ul_mes->mes.inact_sapi_res_mes.prim = LL_INACT_SAPI_RES;

            /* Send msg */
            gllc_ll_ul_sig_put_mes_buf( gas_id, ul_mes );
          }
        }
      }

      /* Free the memory used to convey the primitive */
      GSN_MEM_FREE( ll_msg_ptr );

      break;

    case ( SM_CMD ):

      sm_msg_ptr = adpt_msg_ptr->ctrl_msg.sm_msg;

      gas_id = gsn_sm_prim_get_gas_id ( sm_msg_ptr );

      if ( gas_id >  GERAN_ACCESS_STRATUM_ID_MAX )
      {
        MSG_GERAN_ERROR_0("GSN ""Invalid GAS_ID: %d in SM msg");
      }
      else
      {
        /* Process the SNSM primitive */
        gsn_sm_prim_hdlr( gas_id, sm_msg_ptr );
      }

      /* Free the memory used to convey the primitive */
      GSN_MEM_FREE( sm_msg_ptr );

      break;

    case ( GSN_CMD ):

      gsn_msg_ptr = adpt_msg_ptr->ctrl_msg.gsn_msg;

      gas_id = gsn_int_prim_get_gas_id ( gsn_msg_ptr );

      /* Process the SNDCP internal cmd */
      gsn_int_cmd_hdlr( gas_id, gsn_msg_ptr );

      /* Free the memory used to convey the primitive */
       GSN_MEM_FREE( gsn_msg_ptr );

      break;

    default:

      MSG_GERAN_ERROR_1("GSN ""ME: Invalid control prim id: %d",
                      adpt_msg_ptr->cmd_id);
      break;

    } /* End of switch( adpt_msg_ptr->cmd_id ) */

    /* Free the memory used to convey the adapted message */
    GSN_MEM_FREE( adpt_msg_ptr );
  }
  return TRUE;
}



/*===========================================================================
===
===  FUNCTION      GSNDCP_PROCESS_LL_DL_QUEUE()
===
===  DESCRIPTION
===
===    To be called by the hosting task on activation of the
===    host_ll_dl_queue_sig that was registered with SNDCP on initialization.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised. The host task is expected to clear the
===    associated REX signal before calling this handler.
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

boolean gsndcp_process_ll_dl_queue
(
#ifdef FEATURE_DATA_MM
  comp_sig_enum_type sig,
  void             *pkt_instance
#else
  void
#endif
)
{
  gsn_sapi_t            sapi;
  gprs_ll_dl_dat_mes_t  *mes_ptr;
  gsn_sapi_e_t          *sapi_ptr;
  dsm_item_type         *lead_item_ptr;

#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST
  #define GSN_PROC_DELAY_RPT_THRESHOLD_MS   300

  time_type             process_ts, process_delay_ts;
  uint32                process_delay;
#endif

  gas_id_t              gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;

#ifdef FEATURE_DATA_MM
  NOTUSED(sig);
  NOTUSED(pkt_instance);
#endif

  GSN_ASSERT( sndcp_init != FALSE );

  /* Extract first PDU from the LL downlink data watermark. */

  if ( !gsn_ll_dl_susp_flag )
    lead_item_ptr = dsm_dequeue( &gsnll_data_wm );
  else
    lead_item_ptr = NULL;

  while ( lead_item_ptr != NULL )
  {

    mes_ptr = ( gprs_ll_dl_dat_mes_t * )( lead_item_ptr -> app_field );

    /* Extract GAS_ID from DL user data PDU. */
    gas_id = gsndcp_get_ll_dl_gas_id ( mes_ptr );
    gas_id = check_gas_id( gas_id );

    if ( mes_ptr != NULL )
    {
      /* Check if SAPI is valid */
      if( ( !GSN_VALID_SAPI(mes_ptr->sapi) ) ||
          ( actv_sapis[gas_id][gsn_intern_sapi[mes_ptr->sapi]] == NULL) ||
          ( gas_id > GERAN_ACCESS_STRATUM_ID_MAX ))
      {
        MSG_GERAN_ERROR_2("GSN ""LL %d: Invalid/Inactive SAPI or GAS_ID %d",mes_ptr->sapi,gas_id);

        /* Free dsm item */
        if( mes_ptr->mes.prim == LL_DATA_IND )
        {
          if( mes_ptr->mes.data_ind_mes.l3_pdu )
            GPRS_PDU_FREE( &mes_ptr->mes.data_ind_mes.l3_pdu );
        }
        else if( mes_ptr->mes.prim == LL_UNITDATA_IND )
        {
          if( mes_ptr->mes.unitdata_ind_mes.l3_pdu )
            GPRS_PDU_FREE( &mes_ptr->mes.unitdata_ind_mes.l3_pdu );
        }
        else
          MSG_GERAN_ERROR_1("GSN ""LL %d: Invalid LL data prim",mes_ptr->sapi);
      }
      else
      {
        /* Translate LLC SAPI number to SNDCP internal SAPI number */
        sapi = (gsn_sapi_t) gsn_intern_sapi[mes_ptr->sapi];
        sapi_ptr = actv_sapis[gas_id][sapi];

        /* Process the LL data primitive */
        if( (mes_ptr->mes.prim == LL_DATA_IND) &&
            (mes_ptr->mes.data_ind_mes.l3_pdu != NULL) )
        {
          /* Check if SAPI is established */
          if( (sapi_ptr->state != ESTABLISHED) && (!sapi_ptr->re_establish) )
          {
            MSG_GERAN_ERROR_1("GSN ""LL %d: Not EST for LL_DATA_IND",
              gsn_extern_sapi[sapi]);

            GPRS_PDU_FREE( &mes_ptr->mes.data_ind_mes.l3_pdu );
          }
          else
          {
            /* Process DL ACK data */
            gsn_process_ll_data_ind ( gas_id, sapi_ptr, &mes_ptr->mes.data_ind_mes );
          }
        }
        else if( (mes_ptr->mes.prim == LL_UNITDATA_IND) &&
                 (mes_ptr->mes.unitdata_ind_mes.l3_pdu != NULL ) )
        {
          /* Process DL UNACK data */
          gsn_process_ll_udata_ind ( gas_id, sapi_ptr, &mes_ptr->mes.unitdata_ind_mes );
        }
        else
          MSG_GERAN_ERROR_1("GSN ""LL %d: Invalid data indication",mes_ptr->sapi);

        /* ................................................................. */

#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST
        /* Check for CPU loading as a delaying factor in getting DL PDUs
           processed. Establish the processing delay with respect to when the
           PDU was queued. */

        (void)time_get(process_ts);
        qw_sub(process_delay_ts, process_ts, mes_ptr -> timestamp);

        /* Take the low 32 bits from the total time and shift right by 16 so
           that it contains the number of 1.25ms increments that have elapsed. */

        process_delay = qw_lo(process_delay_ts) >> 16;

        /* Multiply total_time by 5/4 to convert from 1.25ms units to 1ms units */

        process_delay = (process_delay * 5) >> 2;

        /* HACK - If the processing delay is greater than 0xFFF0 then some
           spurious wrap condition has occured in the TS processing. When this
           occurs the delay was found to be less than 1ms, so force to 1ms. */

        if ( process_delay > 0xFFF0UL )
          process_delay = 1;

        if ( process_delay > process_delay_max[ gas_id ] )
          process_delay_max[ gas_id ] = process_delay;

        if ( process_delay > GSN_PROC_DELAY_RPT_THRESHOLD_MS )
        {
          MSG_GERAN_HIGH_3("GSN " "LL DL Proc Delay > %d ms - %d ms, Highest %d ms",
            GSN_PROC_DELAY_RPT_THRESHOLD_MS, process_delay, process_delay_max[ gas_id ] );
        }
#endif /* #ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST */

        /* ................................................................. */
      }

      /* Free the memory used to convey the primitive */
      GSN_MEM_FREE( mes_ptr );
    }
    else
    {
      MSG_GERAN_ERROR_0("GSN " "LL DL Proc: App Field Err");
      GPRS_PDU_FREE( &lead_item_ptr );
    }

    /* Extract next PDU from the LL downlink data watermark. */

    if ( !gsn_ll_dl_susp_flag )
      lead_item_ptr = dsm_dequeue( &gsnll_data_wm );
    else
      lead_item_ptr = NULL;
  }
  return TRUE;
}



/*===========================================================================
===
===  FUNCTION      GSNDCP_GET_LL_DL_GAS_ID()
===
===  DESCRIPTION
===
===    Extracts GAS_ID from the DL message containing a user data PDU.
===
===  DEPENDENCIES
===
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

gas_id_t gsndcp_get_ll_dl_gas_id ( gprs_ll_dl_dat_mes_t  *mes_ptr )
{
  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;

  if ( mes_ptr != NULL )
  {

    if ( mes_ptr->mes.prim == LL_DATA_IND )
    {
      gas_id = mes_ptr->mes.data_ind_mes.gas_id;
    }
    else if ( mes_ptr->mes.prim == LL_UNITDATA_IND )
    {
      gas_id = mes_ptr->mes.unitdata_ind_mes.gas_id;
    }

  }

  return ( gas_id );
} /* gsndcp_get_ll_dl_gas_id () */



/*===========================================================================
===
===  FUNCTION      GSNDCP_PDP_SEND_UL_NPDU()
===
===  DESCRIPTION
===
===    To be called by the PDP uplink packet sourcing function for onward
===    transmission via the given NSAPI address.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised, and the PDP must have been successfully
===    registered with SNDCP via a prior call to gsndcp_pdp_register().
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

void gsndcp_pdp_send_ul_npdu
(
  gas_id_t             gas_id,
  uint8                nsapi,
  dsm_item_type        **npdu
)
{
  gsn_nsapi_t          gnsapi;
  gsn_nsapi_e_t        *nsapi_ptr;
  geran_pdu_priority_t pdu_priority;

  GSN_ASSERT( sndcp_init != FALSE );

  /* PS controls UL data flow and SNDCP could be locked in a continuous UL data transmit
  ** thread while there are control commands from LLC (i.e. LL_RESET_IND) in the SNDCP
  ** command queue. Process any commands, if any, before the UL transmission commences.
  */
  if( q_cnt(&gsn_cmd_q) )
  {
#ifdef FEATURE_DATA_MM
    (void)gsndcp_process_cmd_queue((comp_sig_enum_type)0,NULL);
#else
    gsndcp_process_cmd_queue();
#endif
    MSG_GERAN_LOW_1("GSN ""Interrupt UL data thread for SNDCP cmd",nsapi);
  }

  if( (npdu != NULL) && (*npdu != NULL) )
  {
    sndcp_log_ul_tcp_hdr( *npdu );

    /* Determine PDU priority */
    pdu_priority = gsndcp_ul_pdu_priority_hdlr(*npdu);

    /* check if valid NSAPI */
    if( GSN_VALID_NSAPI(nsapi) )
    {
      /* Get NSAPI number */
      gnsapi = (gsn_nsapi_t) GSN_INTERN_NSAPI(nsapi);
      nsapi_ptr = actv_nsapis[ gas_id ][gnsapi];

      if( nsapi_ptr != NULL)
      {
        /* Process NPDU according to the mode of operation of the NSAPI (ACK/UNACK) */
        if( nsapi_ptr->op_mode == ACK )
        {
          /* Check if NSAPI is mapped on to a valid SAPI */
          if( (nsapi_ptr->sapi_ptr != NULL) &&
              (nsapi_ptr->sapi_ptr->state != ESTABLISHED) )
          {
            MSG_GERAN_ERROR_2("GSN ""NS %d: Rcvd SN_DATA for un-established SAPI %d",
              nsapi,gsn_extern_sapi[nsapi_ptr->sapi_ptr->sapi]);

            /* Free dsm item */
            GPRS_PDU_FREE( npdu );

            return;
          }
          else if( nsapi_ptr->sapi_ptr == NULL )
          {
            MSG_GERAN_ERROR_1("GSN ""NS %d: Rcvd SN_DATA for inalid SAPI - sapi_ptr NULL",
              nsapi);

            /* Free dsm item */
            GPRS_PDU_FREE( npdu );

            return;
          }

          /* Check if buffered NPDU, for re-transmission, excead the max allowed number */
          else if(q_cnt( &(nsapi_ptr->ule.npdu_buff)) >= GPRS_MAX_RETR_NPDUS )
          {
            MSG_GERAN_ERROR_1("GSN ""NS %d: N-PDU buff holds GPRS_MAX_RETR_NPDUS",nsapi);
          }

          /* Check if NSAPI is suspended */
          else if( nsapi_ptr->ule.susp_flag )
          {
            MSG_GERAN_HIGH_2("GSN ""NS %d: Rcvd SN_DATA_REQ while suspended: susp_flag %x",
              nsapi, nsapi_ptr->ule.susp_flag);
          }

          gsn_process_sn_data_req( gas_id, gnsapi, npdu, FALSE, pdu_priority );
        }
        else
        {
          /* Check if NSAPI is mapped on a valid SAPI */
          if( nsapi_ptr->sapi_ptr == NULL)
          {
            MSG_GERAN_ERROR_1("GSN ""NS %d: Rcvd SN_UDATA_REQ for inalid SAPI - sapi_ptr NULL",
              nsapi);

            /* Free dsm item */
            GPRS_PDU_FREE( npdu );

            return;
          }

          if( nsapi_ptr->ule.susp_flag ) /* Check if NSAPI is suspended */
          {
            MSG_GERAN_HIGH_2("GSN ""NS %d: Rcvd SN_UDATA_REQ while suspended: susp_flag %x",
                            nsapi, nsapi_ptr->ule.susp_flag);
          }

          gsn_process_sn_udata_req( gas_id, gnsapi, npdu, FALSE, NULL, pdu_priority );
        }

        /* Delink PDP NPDU */
        *npdu = NULL;
      }
      else
      {
        /* Free dsm item */
        GPRS_PDU_FREE( npdu );

        MSG_GERAN_ERROR_1("GSN ""ME: INVALID or INACTIVE  NSAPI %d",nsapi);
      }
    }
    else
    {
      /* Free dsm item */
      GPRS_PDU_FREE( npdu );

      MSG_GERAN_ERROR_1("GSN ""ME: INVALID or INACTIVE  NSAPI %d",nsapi);
    }
  }
  else
    MSG_GERAN_ERROR_1("GSN ""ME NSAPI %d: Inval UL NPDU",nsapi);
}

/*===========================================================================
===
===  FUNCTION      GSNDCP_PDP_DL_SUSPEND()
===
===  DESCRIPTION
===
===    Suspends downlink PDU flow to the PDP.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised.
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

void gsndcp_pdp_dl_suspend( void )
{
  MSG_GERAN_LOW_0("GSN " "LL: Info PDP DL Suspend");

  gsn_ll_dl_susp_flag = TRUE;

#ifdef FEATURE_DATA_MM
  COMP_CLR_SIGNAL(host_ll_dl_queue_sig);
#else
  (void) rex_clr_sigs(host_tcb_ptr, host_ll_dl_queue_sig);
#endif /* FEATURE_DATA_MM */
}


/*===========================================================================
===
===  FUNCTION      GSNDCP_PDP_DL_RESUME()
===
===  DESCRIPTION
===
===    Resumes downlink PDU flow to the PDP.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised.
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

void gsndcp_pdp_dl_resume( void )
{
  MSG_GERAN_LOW_0("GSN " "LL: Info PDP DL Resume");

  gsn_ll_dl_susp_flag = FALSE;

  if ( q_cnt(&gsnll_data_q) != 0 )
#ifdef FEATURE_DATA_MM
    /* Set the signal for this queue */
    COMP_SET_SIGNAL(host_ll_dl_queue_sig);
#else
    (void) rex_set_sigs(host_tcb_ptr, host_ll_dl_queue_sig);
#endif /* FEATURE_DATA_MM */
}


/*===========================================================================
===
===  FUNCTION      GSNDCP_FOLLOW_ON_REQ_ENQUIRY()
===
===  DESCRIPTION
===
===    For use by Data Services when faced with an enquiry by NAS for
===    Follow on Request feature.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Number of octets of signalling and data awaiting transmission in the
===    GPRS LLC and RLC entities.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

uint32 gsndcp_follow_on_req_enquiry( void )
{
  return ( gllc_llme_get_current_octet_count(0) );

} /* end of gsndcp_follow_on_req_enquiry() */


/*===========================================================================
===
===  FUNCTION      GSNDCP_DIAG_REGISTER()
===
===  DESCRIPTION
===
===    For use by the DIAG task implementation to register the necessary
===    callback functions for unsolicited event reporting.
===
===    Registering or re-registering with a NULL callback pointer
===    disables the associated unsolicited event reporting category.
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

void gsndcp_diag_register
(
  gsndcp_diag_actv_nsapis_rpt_fnc_t  actv_nsapis_rpt_fnc_ptr,
  gsndcp_diag_nsapi_cntx_rpt_fnc_t   nsapi_cntx_rpt_fnc_ptr,
  gsndcp_diag_nsapi_state_rpt_fnc_t  nsapi_state_rpt_fnc_ptr
)
{
  uint8 nsapi;
  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;

  actv_nsapis_rpt_fnc = actv_nsapis_rpt_fnc_ptr;
  nsapi_cntx_rpt_fnc  = nsapi_cntx_rpt_fnc_ptr;
  nsapi_state_rpt_fnc = nsapi_state_rpt_fnc_ptr;

  /* Report to diag
  */
  gsn_encode_actv_nsapis_diag_rpt( gas_id );

  for ( nsapi = (uint8)GSN_NSAPI_5; nsapi < GSN_NSAPI_MAX; nsapi++ )
  {
    if ( actv_nsapis[ gas_id ][nsapi] != NULL )
    {
      gsn_encode_cntx_diag_rpt( gas_id, (uint8) GSN_EXTERN_NSAPI(nsapi) );
      gsn_encode_state_diag_rpt( gas_id, (uint8) GSN_EXTERN_NSAPI(nsapi) );
    }
  }
}

/*===========================================================================
===
===  FUNCTION      GSNDCP_DIAG_GET_PDU_STATS()
===
===  DESCRIPTION
===
===    For use by the DIAG task implementation to request the current set of
===    PDU and octet statistics for the NSAPI indicated by the given NSAPI
===    address.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Pointer to structure of type gsndcp_diag_pdu_stat_rsp_t.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

const gsndcp_diag_pdu_stat_rsp_t *gsndcp_diag_get_pdu_stats
(
  uint8 nsapi_addr
)
{
  gsn_nsapi_e_t               *nsapi_ptr;
  gsndcp_diag_pdu_stat_rsp_t  *pdu_stat_rsp = NULL;
  uint8                       int_nsapi;
  gas_id_t                    gas_id = GERAN_ACCESS_STRATUM_ID_1;

  /* check if valid NSAPI */
  if( GSN_VALID_NSAPI(nsapi_addr) )
  {
    int_nsapi = (uint8)GSN_INTERN_NSAPI(nsapi_addr);
    nsapi_ptr = actv_nsapis[ gas_id ][ int_nsapi ];

    if( nsapi_ptr != NULL )
    {
      pdu_stat_rsp             = &nsapi_ptr->diag_pdu_stat_rsp;
      pdu_stat_rsp->nsapi_addr = (uint8) nsapi_ptr->nsapi;
    }
  }

  return( pdu_stat_rsp );
}

/*===========================================================================
===
===  FUNCTION      GSNDCP_DIAG_RESET_PDU_STATS()
===
===  DESCRIPTION
===
===    For use by the DIAG task implementation to request the reset of the
===    PDU and octet statistics for the NSAPI indicated by the given NSAPI
===    address.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gsndcp_diag_reset_pdu_stats
(
  uint8 nsapi_addr
)
{
  gsn_nsapi_e_t   *nsapi_ptr;
  uint8           int_nsapi;
  gas_id_t        gas_id = GERAN_ACCESS_STRATUM_ID_1;

  /* check if valid NSAPI */
  if( GSN_VALID_NSAPI(nsapi_addr) )
  {
    int_nsapi = (uint8)GSN_INTERN_NSAPI(nsapi_addr);
    nsapi_ptr = actv_nsapis[ gas_id ][ int_nsapi ];

    if( nsapi_ptr != NULL )
    {
      (void)memset
      (
        &nsapi_ptr->diag_pdu_stat_rsp, 0,
        sizeof(gsndcp_diag_pdu_stat_rsp_t)
      );
    }
    else
    {
      MSG_GERAN_ERROR_1("GSN ""ME NSAPI %d: Inactive NSAPI",nsapi_addr);
    }
  }
  else
  {
    MSG_GERAN_ERROR_1("GSN ""ME NSAPI %d: Invalid NSAPI",nsapi_addr);
  }
}



/*** EOF: don't remove! ***/

