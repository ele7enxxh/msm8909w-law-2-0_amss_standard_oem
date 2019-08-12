/*****************************************************************************
***
*** TITLE  gsndcp.h
***
***  GPRS SNDCP LAYER HEADER FILE.
***
***
*** DESCRIPTION
***
***  Accessibility function prototypes for requesting message resource from
***  SNDCP, and registration definitions and prototypes for both task and
***  PDP interfacing.
***
***
***  FEATURE SUPPORT
***  ---------------
***
***   FEATURE_GSM_GPRS              - Supports GPRS including GPRS SNDCP.
***
***   FEATURE_GSM_GPRS_SNDCP_DCOMP  - Supports GPRS SNDCP Data Compression
***
***   FEATURE_GSM_GPRS_SNDCP_PCOMP  - Supports GPRS SNDCP Protocol Control
***                                   Information (PCI) Compression
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/gsndcp.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 27/11/13   ws        CR582513 GERAN API Unification for modem data services
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 11/14/02   rc      Added multimode support.
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/

#ifndef INC_GSNDCP_H
#define INC_GSNDCP_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#ifndef CUSTOMER_H
  #include "customer.h"
#endif

#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "gsmsnif.h"
#include "dsm.h"
#include "rex.h"

#include "comptask.h"
#include "comptask_api.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/* Union of all SM to SNDCP messages */

typedef union
{
  gsnsm_prim_type               prim;

  gsnsm_activ_ind_type          snsm_activ_ind;
  gsnsm_deactiv_ind_type        snsm_deactiv_ind;
  gsnsm_modify_ind_type         snsm_modify_ind;
  gsnsm_seq_ind_type            snsm_seq_ind;

} gsmsn_mes_u_type;


/* Queueable union of all SM to SNDCP messages */

typedef struct
{
  q_link_type                   link;
  gsmsn_mes_u_type              mes;

} gsmsn_mes_type;


/*-------------------------------------------------------------------------
   Structure definitions to record and convey SNDCP internal statistics and
   state information for the purposes of diagnostic monitoring.
---------------------------------------------------------------------------*/

/* Bit map of Active NSAPIs. High indicates an active NSAPI.
** MSB corresponds to NSAPI_15. Note that NSAPI_0 to NSAPI_4 are
** reserved and not used by SNDCP. Will be reported upon diag
** registration, context activation/deactivation and context modification.
*/

typedef uint16 gsndcp_diag_actv_nsapis_rpt_t;

/* NSAPI diagnostic context parameter definitions for unsolicited event
** reporting.
*/

typedef struct
{
  uint8    nsapi_addr;
  uint8    sapi_addr;
  uint16   n201_u;
  uint16   n201_i;
  uint8    reliability_class;
  uint8    peak_throughput;
  uint8    radio_priority;
  boolean  ciphering;
  boolean  data_comp;
  uint8    data_comp_ent;
  uint8    data_comp_alg;
  boolean  protocol_comp;
  uint8    protocol_comp_ent;
  uint8    protocol_comp_alg;

} gsndcp_diag_nsapi_cntx_rpt_t;

/* NSAPI diagnostic state parameter definitions for unsolicited event
** reporting.
*/

typedef struct
{
  uint8  nsapi_addr;
  uint8  nsapi_ul_state;
  uint8  nsapi_dl_state;
  uint8  sapi_state;

} gsndcp_diag_nsapi_state_rpt_t;


/* Diagnostic callback function type definitions for unsolicited event
** reporting.
*/

typedef void ( * gsndcp_diag_actv_nsapis_rpt_fnc_t )
(
  const gsndcp_diag_actv_nsapis_rpt_t *rpt_ptr
);

typedef void ( * gsndcp_diag_nsapi_cntx_rpt_fnc_t )
(
  const gsndcp_diag_nsapi_cntx_rpt_t  *rpt_ptr
);

typedef void ( * gsndcp_diag_nsapi_state_rpt_fnc_t )
(
  const gsndcp_diag_nsapi_state_rpt_t  *rpt_ptr
);


/* NSAPI diagnostic PDU statistics parameter definitions for solicited
** responses.
*/

typedef struct
{
  uint8   nsapi_addr;

  uint32  npdu_tx_cnt;
  uint32  npdu_octet_tx_cnt;
  uint32  npdu_retx_cnt;
  uint32  npdu_octet_retx_cnt;
  uint32  npdu_rx_cnt;
  uint32  npdu_octet_rx_cnt;

  uint32  snpdu_tx_cnt;
  uint32  snpdu_octet_tx_cnt;
  uint32  snpdu_retx_cnt;
  uint32  snpdu_octet_retx_cnt;
  uint32  snpdu_rx_cnt;
  uint32  snpdu_octet_rx_cnt;

  uint32  gen_purpose_a_cnt;
  uint32  gen_purpose_b_cnt;
  uint32  gen_purpose_c_cnt;

} gsndcp_diag_pdu_stat_rsp_t;


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


/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*---------------------------------------------------------------------------
**
** Function prototypes for SM interfacing.
**
**-------------------------------------------------------------------------*/

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

extern gsmsn_mes_type *gsndcp_snsm_get_mes_buf( void );


/*===========================================================================
===
===  FUNCTION      GSNDCP_SNSM_PUT_MES_BUF()
===
===  DESCRIPTION
===
===    Post the SNSM message as given by the message pointer to the SNDCP
===    SNSM message queue.
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

extern void gsndcp_snsm_put_mes( gsmsn_mes_type *mes_ptr );


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
  uint32  tlli,                /* Temporary Logical Link Identifier*/
  uint8   nsapi,               /* NSAP Identifier Address          */
  uint8   sgsn_rcv_npdu_num,   /* Receive N-PDU number             */
  sys_modem_as_id_e_type as_id /* Access Stratum ID       */
);


/*---------------------------------------------------------------------------
**
** Function prototypes for host task and PDP interfacing.
**
**-------------------------------------------------------------------------*/

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

extern void gsndcp_initialise
(
 void
);


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

extern void gsndcp_pdp_register_req
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
#endif /*FEATURE_DUAL_SIM*/
);

/* Legacy PDP Registration Support */

extern boolean gsndcp_pdp_register
(
  uint8                   nsapi,
  wm_cb_type              pdp_ul_suspend_fnc_ptr,
  wm_cb_type              pdp_ul_resume_fnc_ptr,
  void                    ( *pdp_dl_fnc_ptr )( void* ds_context, dsm_item_type **npdu ),
  void                    *ds_context,
  boolean                 cipher,
  sys_modem_as_id_e_type  as_id
);

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
);

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
);

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

extern void gsndcp_diag_register
(
  gsndcp_diag_actv_nsapis_rpt_fnc_t   actv_nsapis_rpt_fnc_ptr,
  gsndcp_diag_nsapi_cntx_rpt_fnc_t    nsapi_cntx_rpt_fnc_ptr,
  gsndcp_diag_nsapi_state_rpt_fnc_t   nsapi_state_rpt_fnc_ptr
);


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

extern const gsndcp_diag_pdu_stat_rsp_t *gsndcp_diag_get_pdu_stats
(
  uint8 nsapi_addr
);


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

extern void gsndcp_diag_reset_pdu_stats
(
  uint8 nsapi_addr
);

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

extern uint32 gsndcp_follow_on_req_enquiry( void );

#endif /* INC_GSNDCP_H */

/*** EOF: don't remove! ***/
