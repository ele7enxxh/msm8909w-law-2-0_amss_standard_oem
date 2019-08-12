#ifndef GLLC_V_G_H
#define GLLC_V_G_H
/*****************************************************************************
***
*** TITLE  gllc.h
***
***  GPRS LLC Layer Service Interface
***
***
*** DESCRIPTION
***
***  Provides the interface functions and type definitions which form the
***  GPRS LLC Layer Service Provision.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gllc/inc/gllc_v_g.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/
/*****************************************************************************
***
***     Service Overview Description
***
*****************************************************************************/
/*

   FEATURE SUPPORT
   ---------------

   FEATURE_GSM_GPRS                - Supports GPRS including GPRS LLC.

   FEATURE_GSM_GPRS_LLC_TOM        - Supports GPRS LLC Tunneling of Msgs (TOM)


   LOGICAL SERVICE PROVISION
   -------------------------

   The diagram below illustrates the logical direction of service provision
   by a lower logical layer to a higher logical client layer.

   This service is provided over two interface connection points. LLGMM is a
   'one to one' interface between the GMM client layer and the LLC serving
   layer. LL is a 'one to many' interface between multiple clients and the
   LLC serving layer. The range of LL clients supported by LLC are defined by
   LLC specification revision. However, the application environment may not
   necessarily embody all supported clients.

   The 'one to many' and 'configurable' characteristics of the LL service
   provision lends itself to a registered API implementation. An LL client
   must register for service at an LL interface connection point, and
   in doing so provide function pointers to the interface functions of the
   client layer.

                    GMM          SNDCP          GSMS          TOM

                     ^^            ^              ^            ^
   Client  ^         ||            |              |            |
   Domain  |         |+------------+------+-------+------------+
                     |                    |
                     |                    |
   Service
   Connection   == LLGMM ================ LL ========================
   Points
                     ^                    ^
                     |                    |
   Service |         +------------------->|
   Domain  v                              |
                                          |

                                         LLC

*/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif


#include "gmmllcif.h"
#include "gllsap.h"
#include "grlcllcif.h"
#include "gllcrlcif.h"

/* ========================================================================
   LL CLIENT DOWNLINK INTERFACE FUNCTION TYPE DEFINITIONS

   For a client layer of this service provision (eg SNDCP, GMM, GSMS, etc)
   to receive down link messages from this service, the client layer is
   required to provide interface functions which are compliant with the
   terms of this service provision. This interface function must be
   notified to this service via a client registration call at startup.

   Refer to function gllc_register_ll_client_XXXX() below.
   ======================================================================== */

/* ------------------------------------------------------------------------
   gllc_ll_dl_sig_get_mes_buf_t -

   A client of this service provision is required to provide a compliant
   interface function for obtaining a message buffer to convey a downlink
   signalling message, to a receiving client.
   ------------------------------------------------------------------------ */

typedef gprs_ll_dl_sig_mes_t *( *gllc_ll_dl_sig_get_mes_buf_t )( void );


/* ------------------------------------------------------------------------
   gprs_ll_dl_sig_put_mes_buf_t -

   A client of this service provision is required to provide a compliant
   interface function for the posting of a downlink signalling message by
   this service provision, to the receiving client.
   ------------------------------------------------------------------------ */

typedef void ( *gllc_ll_dl_sig_put_mes_buf_t )( gprs_ll_dl_sig_mes_t * );


/* ------------------------------------------------------------------------
   gprs_ll_dl_dat_put_mes_buf_t -

   A client of this service provision is required to provide a compliant
   interface function for the posting of a downlink data message by
   this service provision, to the receiving client. It is expected that this
   service provision performs the message buffer allocation from dynamic
   memory, whereupon the receiving client is expected to deallocate the
   message buffer back to dynamic memory.
   ------------------------------------------------------------------------ */

typedef void ( *gllc_ll_dl_dat_put_mes_buf_t )( gprs_ll_dl_dat_mes_t * );

/* ========================================================================
   LL CLIENT REGISTRATION INFORMATION ELEMENT DEFINITIONS

   Structure definitions to convey all registration information as pertinent
   to the registering client, when the client registers for service on the
   relevent LL interface connection points.

   Refer to function gllc_register_ll_client_XXXX() below.
   ======================================================================== */

typedef struct
{
  gllc_ll_dl_sig_get_mes_buf_t  ll_dl_sig_get_mes_buf_fnc_ptr;
  gllc_ll_dl_sig_put_mes_buf_t  ll_dl_sig_put_mes_buf_fnc_ptr;

  gllc_ll_dl_dat_put_mes_buf_t  ll_dl_dat_put_mes_buf_fnc_ptr;
  dsm_watermark_type            *ll_dl_wm_ptr;

} gllc_sndcp_ll_reg_req_t;

typedef struct
{
  dsm_watermark_type            *ll_3_ul_ack_wm_ptr;
  dsm_watermark_type            *ll_3_ul_unack_wm_ptr;

  dsm_watermark_type            *ll_5_ul_ack_wm_ptr;
  dsm_watermark_type            *ll_5_ul_unack_wm_ptr;

  dsm_watermark_type            *ll_9_ul_ack_wm_ptr;
  dsm_watermark_type            *ll_9_ul_unack_wm_ptr;

  dsm_watermark_type            *ll_11_ul_ack_wm_ptr;
  dsm_watermark_type            *ll_11_ul_unack_wm_ptr;

} gllc_sndcp_ll_reg_rsp_t;

/* ------------------------------------------------------------------------ */



/* ========================================================================
   DIAGNOSTIC/STATISTICS MESSAGING DEFINTIONS

   Structure definitions to record and convey LLC internal statistics and
   state information for the purposes of diagnostic monitoring.

   ======================================================================== */

/* LLME diagnostic parameter definitions for unsolicited event reporting. */

typedef struct
{
  uint8   llme_state;
  uint8   llme_xid_version;
  uint32  llme_new_tlli;
  uint32  llme_old_tlli;
  uint8   llme_encrypt_alg;
  uint32  llme_encrypt_key[2];
  uint32  llme_encrypt_key128[4];
  uint32  llme_input_offset_iov_ui;

} gllc_diag_llme_state_rpt_t;

/* ------------------------------------------------------------------------ */

/* LLE diagnostic state parameter definitions for unsolicited event
   reporting. */

typedef struct
{
  uint8   lle_sapi_addr;

  uint8   lle_state;
  uint8   lle_substate;

} gllc_diag_lle_state_rpt_t;

/* ------------------------------------------------------------------------ */

/* LLE diagnostic XID parameter definitions for unsolicited event
   reporting. */

typedef struct
{
  uint8   lle_sapi_addr;

  uint16  pending_xid_bitmap;

  uint32  current_iov_i;

  uint32  current_t200;
  uint32  preferred_t200;

  uint8   current_n200;
  uint8   preferred_n200;

  uint16  current_n201_u;
  uint16  preferred_n201_u;

  uint16  current_n201_i;
  uint16  preferred_n201_i;

  uint16  current_md;
  uint16  preferred_md;

  uint16  current_mu;
  uint16  preferred_mu;

  uint8   current_kd;
  uint8   preferred_kd;

  uint8   current_ku;
  uint8   preferred_ku;

} gllc_diag_lle_xid_rpt_t;

/* ------------------------------------------------------------------------ */

/* Diagnostic callback function type definitions for unsolicited event
   reporting. */

typedef void ( * gllc_diag_rpt_llme_state_fnc_t )
(
  const gllc_diag_llme_state_rpt_t *rpt_ptr
);

typedef void ( * gllc_diag_rpt_lle_state_fnc_t )
(
  const gllc_diag_lle_state_rpt_t  *rpt_ptr
);

typedef void ( * gllc_diag_rpt_lle_xid_fnc_t )
(
  const gllc_diag_lle_xid_rpt_t    *rpt_ptr
);

/* ------------------------------------------------------------------------ */

/* LLE diagnostic PDU statistics parameter definitions for solicited
   responses. */

typedef struct
{
  uint8   lle_sapi_addr;

  uint32  ack_l3pdu_tx_cnt;
  uint32  ack_octet_tx_cnt;
  uint32  ack_l3pdu_retx_cnt;
  uint32  ack_octet_retx_cnt;
  uint32  ack_l3pdu_rx_cnt;
  uint32  ack_octet_rx_cnt;

  uint32  unack_l3pdu_tx_cnt;
  uint32  unack_octet_tx_cnt;
  uint32  unack_l3pdu_rx_cnt;
  uint32  unack_octet_rx_cnt;

  uint32  llpdu_tx_cnt;
  uint32  llpdu_rx_cnt;

  uint32  llpdu_fcs_err_cnt;
  uint32  llpdu_frm_rej_cnt;
  uint32  llpdu_tlli_err_cnt;
  uint32  llpdu_addr_err_cnt;
  uint32  llpdu_short_err_cnt;

  uint32  ack_l3pdu_rerx_cnt;
  uint32  ack_octet_rerx_cnt;

} gllc_diag_pdu_stat_rsp_t;

/* ------------------------------------------------------------------------ */

/* LLE diagnostic state and ancilliary statistics parameter definitions for
   solicited responses. */

typedef struct
{
  uint8   lle_sapi_addr;

  /* LLE State Variables */

  uint16  unack_snd_seq_var_v_u;
  uint16  unack_rcv_seq_var_v_ur;

  uint32  unack_encipher_oc;
  uint32  unack_decipher_oc;

  uint16  ack_snd_seq_var_v_s;
  uint16  ack_rcv_seq_var_v_r;
  uint16  ack_seq_var_v_a;

  uint32  ack_encipher_oc;
  uint32  ack_decipher_oc;

  /* LLE Ancilliary Statistics */

  uint32  reestablishment_cnt;

  uint32  gen_purpose_a_cnt;
  uint32  gen_purpose_b_cnt;
  uint32  gen_purpose_c_cnt;

} gllc_diag_state_stat_rsp_t;


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


/*===========================================================================
===
===  FUNCTION      GLLC_REGISTER_LL_CLIENT_SNDCP()
===
===  DESCRIPTION
===
===   Provides a mechanism for an SNDCP client to register for this service
===   provision.
===
===   The registering client is required to submit function pointers for use
===   by the serving layer to convey signalling and data bearing messages to
===   the client for processing.
===
===   The registering client is also required to submit a pointer to the
===   client's downlink DSM watermark.
===
===   These functions must be compliant with the type definitions of this
===   service provision.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Pointer to a registration response structure containing pointers to
===    the Uplink DSM watermarks for the LL SAPIs 3, 5, 9, & 11. The
===    registering client is responsible for configuring the
===    lowater_func_ptr, hiwater_func_ptr, lo_watermark, hi_watermark and
===    dont_exceed_cnt elements of these watermarks.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

extern void gllc_register_ll_client_sndcp
(
  gas_id_t                      gas_id,
  gllc_ll_dl_sig_get_mes_buf_t  ll_dl_sig_get_mes_buf_fnc_ptr,
  gllc_ll_dl_sig_put_mes_buf_t  ll_dl_sig_put_mes_buf_fnc_ptr,
  gllc_ll_dl_dat_put_mes_buf_t  ll_dl_dat_put_mes_buf_fnc_ptr
);


/*===========================================================================
===
===  FUNCTION      GLLC_REGISTER_LL_CLIENT_GMM()
===
===  DESCRIPTION
===
===   Provides a mechanism for a GMM client to register for this service
===   provision.
===
===   The registering client is required to submit function pointers for use
===   by the serving layer to convey signalling and data bearing messages to
===   the client for processing.
===
===   These functions must be compliant with the type definitions of this
===   service provision.
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

extern void gllc_register_ll_client_gmm
(
  gas_id_t                      gas_id,
  gllc_ll_dl_sig_get_mes_buf_t  ll_dl_sig_get_mes_buf_fnc_ptr,
  gllc_ll_dl_sig_put_mes_buf_t  ll_dl_sig_put_mes_buf_fnc_ptr,
  gllc_ll_dl_dat_put_mes_buf_t  ll_dl_dat_put_mes_buf_fnc_ptr
);


/*===========================================================================
===
===  FUNCTION      GLLC_REGISTER_LL_CLIENT_GSMS()
===
===  DESCRIPTION
===
===   Provides a mechanism for a GSMS client to register for this service
===   provision.
===
===   The registering client is required to submit function pointers for use
===   by the serving layer to convey signalling and data bearing messages to
===   the client for processing.
===
===   These functions must be compliant with the type definitions of this
===   service provision.
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

extern void gllc_register_ll_client_gsms
(
  gas_id_t                      gas_id,
  gllc_ll_dl_sig_get_mes_buf_t  ll_dl_sig_get_mes_buf_fnc_ptr,
  gllc_ll_dl_sig_put_mes_buf_t  ll_dl_sig_put_mes_buf_fnc_ptr,
  gllc_ll_dl_dat_put_mes_buf_t  ll_dl_dat_put_mes_buf_fnc_ptr
);


/*===========================================================================
===
===  FUNCTION      GLLC_REGISTER_LL_CLIENT_TOM()
===
===  DESCRIPTION
===
===   Provides a mechanism for a TOM client to register for this service
===   provision.
===
===   The registering client is required to submit function pointers for use
===   by the serving layer to convey signalling and data bearing messages to
===   the client for processing.
===
===   These functions must be compliant with the type definitions of this
===   service provision.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Pointer to a registration response structure containing pointers to
===    the Uplink DSM watermarks for the LL SAPIs 2 & 8. The registering
===    client is responsible for configuring the lowater_func_ptr,
===    hiwater_func_ptr, lo_watermark, hi_watermark and dont_exceed_cnt
===    elements of these watermarks.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/



/*===========================================================================
===
===  FUNCTION      GLLC_LL_CLIENT_REGISTERED()
===
===  DESCRIPTION
===
===    Determines whether an LL client has registered for service at the
===    given LL SAP address.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    TRUE if the given client has registered for service, FALSE otherwise.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */


/*===========================================================================
===
===  FUNCTION      GLLC_LL_UL_SIG_GET_MES_BUF()
===
===  DESCRIPTION
===
===    Permits an LL client entity to request the dynamic allocation of
===    sufficient memory to accomodate a message for conveyance to this LL
===    service provision via the LL interface connection point.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Pointer to allocated gprs_ll_ul_sig_mes_t message space.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

extern gprs_ll_ul_sig_mes_t *gllc_ll_ul_sig_get_mes_buf( void );


/*===========================================================================
===
===  FUNCTION      GLLC_LL_UL_SIG_PUT_MES_BUF()
===
===  DESCRIPTION
===
===    Permits an LL client entity to convey a signalling message to this LL
===    service provision via the LL interface connection point.
===
===  DEPENDENCIES
===
===    The LL client must have previously registered for service at the LL
===    interface connection point via gllc_register_ll_client_XXXX().
===
===    Message pointer must have previously been allocated with a call to
===    gllc_ll_ul_sig_get_mes_buf().
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

extern void gllc_ll_ul_sig_put_mes_buf( gas_id_t gas_id, gprs_ll_ul_sig_mes_t *mes_ptr );


/*===========================================================================
===
===  FUNCTION      GLLC_LL_UL_DAT_PUT_MES_BUF()
===
===  DESCRIPTION
===
===    Permits an LL client entity to convey a data message to this LL
===    service provision via the LL interface connection point.
===
===  DEPENDENCIES
===
===    The LL client must have previously registered for service at the LL
===    interface connection point via gllc_register_ll_client_XXXX().
===
===    LL client entity is responsible for dynamic message resource
===    allocation for the given message.
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

extern void gllc_ll_ul_dat_put_mes_buf( gas_id_t gas_id, gprs_ll_ul_dat_mes_t *mes_ptr );


/*===========================================================================
===
===  FUNCTION      GLLC_LL_GET_DEFAULT_N201_I()
===
===  DESCRIPTION
===
===    Provides for a client of this service provision (eg GMM, SNDCP,
===    GSMS, etc) to directly obtain the default or initial setting for the
===    maximum layer 3 acknowledged PDU size (LLC layer parameter N201-I).
===
===    Should this parameter change during operation, then a registered
===    client can expect to be informed of the new parameter setting by way
===    of an LL-XID-IND service primitive.
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

extern uint16 gllc_ll_get_default_N201_I( uint8 sapi );


/*===========================================================================
===
===  FUNCTION      GLLC_LL_GET_DEFAULT_N201_U()
===
===  DESCRIPTION
===
===    Provides for a client of this service provision (eg GMM, SNDCP,
===    GSMS, etc) to directly obtain the default or initial setting for the
===    maximum layer 3 unacknowledged PDU size (LLC layer parameter N201-U).
===
===    Should this parameter change during operation, then a registered
===    client expect to be informed of the new paramater setting by way of
===    an LL-XID-IND service primitive.
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

extern uint16 gllc_ll_get_default_N201_U( uint8 sapi );


/*===========================================================================
===
===  FUNCTION      GLLC_LL_GET_MAX_N201_U()
===
===  DESCRIPTION
===
===    Provides for a client of this service provision (eg GMM, SNDCP,
===    GSMS, etc) to directly obtain the maximum setting for the
===    layer 3 acknowledged PDU size (LLC layer parameter N201-I).
===
===    Should this parameter change during operation, then a registered
===    client can expect to be informed of the new parameter setting by way
===    of an LL-XID-IND service primitive.
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

extern uint16 gllc_ll_get_max_N201_U( uint8 sapi );


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_GET_GEA_ALGORITHM()
===
===  DESCRIPTION
===
===    Provides GEA algorithm currently in use.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    GEA algorithm.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

extern gmmllc_gea_e_type gllc_llme_get_gea_algorithm( gas_id_t gas_id );


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_SET_OWN_RCVR_BUSY_COND()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

#ifdef FEATURE_DSM_WM_CB
extern void gllc_llme_set_own_rcvr_busy_cond
( 
  dsm_watermark_type *wm_p, 
  void *context
);
#else
extern void gllc_llme_set_own_rcvr_busy_cond( void );
#endif /* FEATURE_DSM_WM_CB */


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_CLR_OWN_RCVR_BUSY_COND()
===
===  Internal prototype.
===  Refer to gllcstate.c for full description.
===
===========================================================================*/

#ifdef FEATURE_DSM_WM_CB
extern void gllc_llme_clr_own_rcvr_busy_cond
(
  dsm_watermark_type *wm_p, 
  void *context
);
#else
extern void gllc_llme_clr_own_rcvr_busy_cond( void );
#endif /* FEATURE_DSM_WM_CB */


/*===========================================================================
===
===  FUNCTION      GLLC_GRR_UL_SUSPEND_IND()
===
===  DESCRIPTION
===
===    For use by the RLC layer implementation to signal a data 'suspend'
===    flow control condition to the LLC layer.
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

extern void gllc_grr_ul_suspend_ind( gas_id_t gas_id );


/*===========================================================================
===
===  FUNCTION      GLLC_GRR_UL_L3L2_SUSPEND_IND()
===
===  DESCRIPTION
===
===    For use by the RLC layer implementation to signal a data 'suspend'
===    flow control condition to the LLC layer AND the SNDCP layer.
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

extern void gllc_grr_ul_L3L2_suspend_ind( gas_id_t gas_id );


/*===========================================================================
===
===  FUNCTION      GLLC_GRR_UL_RESUME_IND()
===
===  DESCRIPTION
===
===    For use by the RLC layer implementation to signal a data 'resume'
===    flow control condition to the LLC layer.
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

extern void gllc_grr_ul_resume_ind( gas_id_t gas_id );


/*===========================================================================
===
===  FUNCTION      GLLC_GRR_UL_TBF_REL_IND()
===
===  DESCRIPTION
===
===    For use by the RLC layer implementation to inform the LLC layer of
===    UL TBF release.
===
===    This serves to terminate any active Etsi Test Mode A processes.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    TRUE if RLC must purge all queued RLC data as part of Etsi Test Mode A
===    completion, FALSE otherwise.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

extern boolean gllc_grr_ul_tbf_rel_ind( gas_id_t gas_id );


/*===========================================================================
===
===  FUNCTION      GLLC_GRR_UL_TRANSMIT_IND()
===
===  DESCRIPTION
===
===    For use by the RLC layer implementation to inform the LLC layer of
===    an LLC PDU transmission on the radio interface, and the transmission
===    cause.
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

extern void gllc_grr_ul_transmit_ind
(
  gas_id_t              gas_id,
  uint8                 sapi,
  uint8                 reliability_class,
  ll_sndcp_ref_t        *sndcp_ref,
  grr_data_req_cause_t  cause
);


/*===========================================================================
===
===  FUNCTION      GLLC_GRR_DL_PUT_MES_BUF()
===
===  DESCRIPTION
===
===    For use by the RLC layer implementation to put a dynamically
===    allocated message resource of type gprs_grr_dl_mes_t to the GRR
===    downlink message queue.
===
===  DEPENDENCIES
===
===    RLC entity is responsible for dynamic message resource allocation
===    for the given message.
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

extern void gllc_grr_dl_put_mes_buf( gprs_grr_dl_mes_t *mes_ptr );


/*===========================================================================
===
===  FUNCTION      GLLC_DIAG_REGISTER()
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
===    Where a registered callback is not NULL, first report(s) will be
===    generated instantaneously using the registered callback.
===
===========================================================================*/

extern void gllc_diag_register
(
  gas_id_t                         gas_id,
  gllc_diag_rpt_llme_state_fnc_t   llme_state_fnc_p,
  gllc_diag_rpt_lle_state_fnc_t    lle_state_fnc_p,
  gllc_diag_rpt_lle_xid_fnc_t      lle_xid_fnc_p
);


/*===========================================================================
===
===  FUNCTION      GLLC_DIAG_GET_PDU_STATS()
===
===  DESCRIPTION
===
===    For use by the DIAG task implementation to request the current set of
===    PDU and octet statistics for the LLE indicated by the given SAPI
===    address.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Pointer to structure of type gllc_diag_pdu_stat_rsp_t.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

extern const gllc_diag_pdu_stat_rsp_t *gllc_diag_get_pdu_stats
(
  gas_id_t gas_id,
  uint8    sapi_addr
);


/*===========================================================================
===
===  FUNCTION      GLLC_DIAG_RESET_PDU_STATS()
===
===  DESCRIPTION
===
===    For use by the DIAG task implementation to request the reset of the
===    PDU and octet statistics for the LLE indicated by the given SAPI
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

extern void gllc_diag_reset_pdu_stats
(
  gas_id_t gas_id,
  uint8    sapi_addr
);


/*===========================================================================
===
===  FUNCTION      GLLC_DIAG_GET_STATE_STATS()
===
===  DESCRIPTION
===
===    For use by the DIAG task implementation to request the current set of
===    state variables and anciliary counts for the LLE indicated by the
===    given SAPI address.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Pointer to structure of type gllc_diag_state_stat_rsp_t.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

extern const gllc_diag_state_stat_rsp_t *gllc_diag_get_state_stats
(
  gas_id_t gas_id,
  uint8    sapi_addr
);


/*===========================================================================
===
===  FUNCTION      GLLC_DIAG_RESET_ANCILLIARY_STATS()
===
===  DESCRIPTION
===
===    For use by the DIAG task implementation to request the reset of the
===    anciliary statistics counters for the LLE indicated by the given SAPI
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

extern void gllc_diag_reset_ancilliary_stats
(
  gas_id_t gas_id,
  uint8    sapi_addr
);


/*===========================================================================
===
===  FUNCTION      GLLC_IS_GEA_SUPPORTED()
===
===  DESCRIPTION
===
===    Determines whether the indicated GPRS Encryption Algorithm (GEA) is
===    supported.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    TRUE if given GEA is supported, FALSE otherwise.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

extern boolean gllc_is_gea_supported
(
  gas_id_t          gas_id,
  gmmllc_gea_e_type gea
);

/*===========================================================================
===
===  FUNCTION      GLLC_LLME_GET_CURRENT_OCTET_COUNT()
===
===  DESCRIPTION
===
===    Provides for a client of this service provision (eg GMM, SNDCP,
===    GSMS, etc) to directly enquire as to the quantity of buffered data
===    and signalling awaiting transmission in the LLC and RLC entities.
===
===    A typical usage of this enquiry would be in support of the Follow On
===    Request feature.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Octet count of data in LLC watermark interfaces, and in RLC awaiting
===    transmission.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

extern uint32 gllc_llme_get_current_octet_count( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      GLLCLIENT_REG_GMM_GSMS_PROXY()
===
===  DESCRIPTION
===
===    For use by the GPRS LLC Task at initialisation to register the GMM &
===    GSMS proxy clients at the appropriate LL SAP service access point
===    addresses.
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

extern void gllclient_reg_gmm_gsms_proxy( gas_id_t gas_id );



#endif /* GLLC_V_H */

/*** EOF: don't remove! ***/
