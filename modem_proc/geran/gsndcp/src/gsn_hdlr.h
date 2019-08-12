/*****************************************************************************
***
*** TITLE
***
***  SNDCP HANDLER HEADER FILE
***
***
*** DESCRIPTION
***
***  Header file for the SNDCP handler module.
***
***
*** Copyright (c) 2001-2015 Qualcomm Technologies, Inc.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gsndcp/src/gsn_hdlr.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/

#ifndef INC_GSN_HDLR_H
#define INC_GSN_HDLR_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "gsni.h"
#include "gsndcp.h"
#include "gllsap.h"
#include "gmutex.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/


/* SNDCP critical section to protect accesses to messages and internal vars
*/

extern rex_crit_sect_type gsn_crit_sec;


/* Table of active NSAPIs. Order of arrey elemets according to NSAPI number */
extern gsn_nsapi_e_t   *actv_nsapis[NUM_GERAN_DATA_SPACES][GSN_NSAPI_MAX];

/* Table of active SAPIs. Order of arrey elemets according to SAPI number */
extern gsn_sapi_e_t   *actv_sapis[NUM_GERAN_DATA_SPACES][GSN_SAPI_MAX];

extern uint8 suspend_flag;

/* Lookup tables for SAPI address translation
*/
extern const uint8 gsn_extern_sapi[];
extern const uint8 gsn_intern_sapi[];

/* For more information about diagnostic monitoring of SNDCP
** refer to gsndcp.h.
*/
extern gsndcp_diag_actv_nsapis_rpt_fnc_t  actv_nsapis_rpt_fnc;

extern gsndcp_diag_nsapi_cntx_rpt_fnc_t   nsapi_cntx_rpt_fnc;

extern gsndcp_diag_nsapi_state_rpt_fnc_t  nsapi_state_rpt_fnc;


/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/

/* This Macro suspends the Up-Link PDP data flow and sets the indicated event */
/*lint -esym(578, REXNT_IS_REX_THREAD)*/
#define GSN_SET_SUSP_EVENT( gas_id, nsapi, event )                                        \
      {                                                                                   \
        MSG_GERAN_LOW_3_G("GSN NSapi %d: ENTER set_susp: s_event=%x susp_flag=%x",        \
          GSN_EXTERN_NSAPI(nsapi), (event),                                               \
          actv_nsapis[ gas_id ][nsapi]->ule.susp_flag);                                   \
        GSN_LOCK();                                                                       \
        if( actv_nsapis[ gas_id ][nsapi] != NULL )                                        \
        {                                                                                 \
          actv_nsapis[ gas_id ][nsapi]->ule.susp_flag |= event;                           \
          if( actv_nsapis[ gas_id ][nsapi]->pdp_ul_suspend_fnc )                          \
          {                                                                               \
            actv_nsapis[ gas_id ][nsapi]->pdp_ul_suspend_fnc(                             \
              NULL, actv_nsapis[ gas_id ][nsapi]->ds_context);                            \
          }                                                                               \
          MSG_GERAN_HIGH_3_G("GSN NSapi %d: PDP ULSuspendReq susp_event=%x susp_flag=%x", \
            GSN_EXTERN_NSAPI(nsapi),                                                      \
            event,                                                                        \
            actv_nsapis[ gas_id ][nsapi]->ule.susp_flag);                                 \
        }                                                                                 \
        else                                                                              \
        {                                                                                 \
          MSG_GERAN_ERROR_1_G("GSN NS %d: Susp failed. Invalid NSAPI ptr",                \
                        GSN_EXTERN_NSAPI(nsapi));                                         \
        }                                                                                 \
        GSN_UNLOCK();                                                                     \
      }

/* This Macro clears the indicated event. If all events are cleared
** the Up-Link  PDP data flow is resumed.
*/
#define GSN_CLR_SUSP_EVENT( gas_id, nsapi, event )                                            \
      {                                                                                       \
        if( actv_nsapis[ gas_id ][nsapi] != NULL )                                            \
        {                                                                                     \
          MSG_GERAN_HIGH_3_G("GSN NS %d: ENTER clr_susp: s_event=%x susp_flag=%x",            \
            GSN_EXTERN_NSAPI(nsapi), (event), actv_nsapis[ gas_id ][nsapi]->ule.susp_flag);   \
          GSN_LOCK();                                                                         \
          actv_nsapis[ gas_id ][nsapi]->ule.susp_flag &= ~event;                              \
          suspend_flag = actv_nsapis[ gas_id ][nsapi]->ule.susp_flag;                         \
          GSN_UNLOCK();                                                                       \
          if( suspend_flag )                                                                  \
          {                                                                                   \
            if( ( suspend_flag & GSN_UL_SUSP_MAX_NPDU ) &&                                    \
                (!(suspend_flag & GSN_UL_SUSP_MAX_NPDU_MASK)) )                               \
            {                                                                                 \
              /* Do not resume PDP data flow. Two many N-PDU in                               \
              ** the SNDCP retransmition  buffer. If retransmit event                         \
              ** is set, start retransmiting N-PDU. */                                        \
              if( actv_nsapis[ gas_id ][nsapi]->ule.retr )                                    \
                gsn_send_retr_req(gas_id, nsapi);                                             \
            }                                                                                 \
          }                                                                                   \
          else                                                                                \
          {                                                                                   \
            /* If the PDP Registration Callback has not yet been called,                      \
            ** then this can now be called prior to resumption. */                            \
            if( (actv_nsapis[ gas_id ][nsapi]->pdp_reg_cnf_fnc           ) &&                 \
                (actv_nsapis[ gas_id ][nsapi]->deactv_resp_pend == FALSE )    )               \
            {                                                                                 \
              actv_nsapis[ gas_id ][nsapi]->pdp_reg_cnf_fnc(                                  \
                actv_nsapis[ gas_id ][nsapi]->ds_context, TRUE);                              \
              actv_nsapis[ gas_id ][nsapi]->pdp_reg_cnf_fnc = NULL;                           \
            }                                                                                 \
            else if( (actv_nsapis[ gas_id ][nsapi]->pdp_reg_cnf_fnc           ) &&            \
                     (actv_nsapis[ gas_id ][nsapi]->deactv_resp_pend == TRUE  )    )          \
            {                                                                                 \
              actv_nsapis[ gas_id ][nsapi]->pdp_reg_cnf_fnc(                                  \
                actv_nsapis[ gas_id ][nsapi]->ds_context, FALSE);                             \
              actv_nsapis[ gas_id ][nsapi]->pdp_reg_cnf_fnc = NULL;                           \
              MSG_GERAN_MED_1_G( "GSN NS %d: PDP Reg failed",                                 \
                             GSN_EXTERN_NSAPI(nsapi));                                        \
            }                                                                                 \
            /* Resume PDP data flow. If retransmit event is set,                              \
            ** start retransmiting bufferd N-PDUs. */                                         \
            if(actv_nsapis[ gas_id ][nsapi]->pdp_ul_resume_fnc)                               \
            {                                                                                 \
              actv_nsapis[ gas_id ][nsapi]->pdp_ul_resume_fnc(                                \
                NULL, actv_nsapis[ gas_id ][nsapi]->ds_context);                              \
            }                                                                                 \
            MSG_GERAN_HIGH_1_G("GSN NS %d: PDP UL Resume Req",                                \
                          GSN_EXTERN_NSAPI(nsapi));                                           \
            gsndcp_check_data_in_smd_to_sndcp_wm(gas_id, nsapi);                              \
            if( actv_nsapis[ gas_id ][nsapi]->ule.retr )                                      \
              gsn_send_retr_req(gas_id, nsapi);                                               \
          }                                                                                   \
          MSG_GERAN_LOW_3_G("GSN NS %d: EXIT clr_susp: susp_flag=%x, retr_flag=%d",           \
            GSN_EXTERN_NSAPI(nsapi),actv_nsapis[ gas_id ][nsapi]->ule.susp_flag,              \
            actv_nsapis[ gas_id ][nsapi]->ule.retr);                                          \
        }                                                                                     \
        else                                                                                  \
        {                                                                                     \
          MSG_GERAN_ERROR_1_G("GSN NS %d: Clr susp flag failed. Invalid NSAPI ptr",           \
                        GSN_EXTERN_NSAPI(nsapi));                                             \
        }                                                                                     \
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
void gsn_send_ll_est_req( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr, dsm_item_type  *xid_req );

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
void gsn_process_ll_data_ind( gas_id_t gas_id, const gsn_sapi_e_t *, ll_data_ind_t  *);

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
void gsn_process_ll_udata_ind( gas_id_t gas_id, const gsn_sapi_e_t *, ll_unitdata_ind_t  *);

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
void gsn_retr_npdu_hdlr( gas_id_t gas_id, gsn_nsapi_t nsapi );

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
void gsn_send_retr_req( gas_id_t gas_id, gsn_nsapi_t nsapi );

/*===========================================================================
===
===  FUNCTION      GSN_SM_PRIM_HDLR()
===
===  DESCRIPTION
===
===   Processes an SM->SNDCP SNSM primitive
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

void gsn_sm_prim_hdlr( gas_id_t, const gsmsn_mes_type * );


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
gas_id_t gsn_sm_prim_get_gas_id (const gsmsn_mes_type * mes_ptr);


/*===========================================================================
===
===  FUNCTION      GSN_LL_CTRL_PRIM_HDLR()
===
===  DESCRIPTION
===
===   Processes an LLC->SNDCP LL control primitive
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

void gsn_ll_ctrl_prim_hdlr( gas_id_t, gprs_ll_dl_sig_mes_u*, gsn_sapi_t );


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

void gsn_ll_ctrl_prim_free_xid_pdu( gprs_ll_dl_sig_mes_t *ll_msg_ptr );



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

gas_id_t gsn_ll_ctrl_prim_get_gas_id( gprs_ll_dl_sig_mes_t *mes_ptr );


/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_SN_DATA_REQ()
===
===  DESCRIPTION
===
===   Processes SN-DATA request from PDP.
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
void gsn_process_sn_data_req( gas_id_t gas_id, 
                              gsn_nsapi_t, 
                              dsm_item_type **, 
                              boolean, 
                              geran_pdu_priority_t pdu_priority );


/*===========================================================================
===
===  FUNCTION      GSN_INT_PRIM_GET_GAS_ID()
===
===  DESCRIPTION
===
===   Processes SNDCP internal command and extracts GAS_ID.
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
gas_id_t gsn_int_prim_get_gas_id ( gsn_internal_msg_t *mes_ptr );


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
void gsn_int_cmd_hdlr( gas_id_t gas_id, const gsn_internal_msg_t* );

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
  gas_id_t,
  gsn_nsapi_t, 
  dsm_item_type **,
  boolean,     
  gsn_npdu_item_t *,
  geran_pdu_priority_t
);

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
void gsn_send_ll_xid_req( gas_id_t, gsn_sapi_e_t *, dsm_item_type *);


/*===========================================================================
===
===  FUNCTION      GSN_SEND_SNSM_STAT_REQ()
===
===  DESCRIPTION
===
===   Sends the SNSM-STATUS-REQ primitive to SM.
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
void gsn_send_snsm_stat_req( gas_id_t, gsn_sapi_t, uint32, uint8 );


/*===========================================================================
===
===  FUNCTION      GSN_ENCODE_ACTV_NSAPIS_DIAG_RPT()
===
===  DESCRIPTION
===
===   Encodes the active NSAPIs information report for diagnostic monitoring.
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
void gsn_encode_actv_nsapis_diag_rpt( gas_id_t gas_id );

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
void gsn_encode_cntx_diag_rpt( gas_id_t gas_id, uint8 nsapi );

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
void gsn_encode_state_diag_rpt( gas_id_t, uint8 nsapi );


#endif /* INC_GSN_HDLR_H */

/*** EOF: don't remove! ***/
