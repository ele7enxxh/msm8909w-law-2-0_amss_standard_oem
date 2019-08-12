/*****************************************************************************
***
*** TITLE  gsndcp_v_g.h
***
***  GPRS SNDCP LAYER HEADER FILE ACCESSIBLE TO OTHER GERAN LAYERS.
***
***
*** DESCRIPTION
***
***  Accessibility function prototypes for: requesting message resource from
***  SNDCP, UL watermark callbacks, definitions for UL watermark levels.
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
*** $Header:
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 03-07-12   da      File checked-in.
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/

#ifndef INC_GSNDCP_V_G_H
#define INC_GSNDCP_V_G_H


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

#include "dsm.h"
#include "rex.h"

#include "gllc_v.h"
#include "gllc_v_g.h"

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


/* The maximum NPDU, for IP and PPP, is 1502 octets (TS23.060 Subclause 9.3)
** The minimum value of the N201_I is 140 octets. This means that the
** worst case scenario for segmenting a PPP PDU gives around 11 segments
** (1502/140). If we add the UNACK SN-PDU header of 4 octets per segment,
** then for the LLC UL queue to be able to hold a whole PPP packet it should
** be able to hold 1546 octets ( 11*4 + 1502 ).
**
** Discard count increased to 12*1546 octets to avoid LLC discarding UL PDUs.
*/

#define GSN_LL_MAX_OCTETS_PER_NPDU      1546

#define GSN_LL_LOWATER_CNT              ( 1 * GSN_LL_MAX_OCTETS_PER_NPDU )
#define GSN_LL_HIWATER_CNT              ( 2 * GSN_LL_MAX_OCTETS_PER_NPDU )
#define GSN_LL_DISCARD_CNT              ( 16 * GSN_LL_MAX_OCTETS_PER_NPDU )

#define GSN_LL3_ACK_LOWATER_CNT         GSN_LL_LOWATER_CNT
#define GSN_LL3_ACK_HIWATER_CNT         GSN_LL_HIWATER_CNT
#define GSN_LL3_ACK_DISCARD_CNT         GSN_LL_DISCARD_CNT

#define GSN_LL3_UNACK_LOWATER_CNT       GSN_LL_LOWATER_CNT
#define GSN_LL3_UNACK_HIWATER_CNT       GSN_LL_HIWATER_CNT
#define GSN_LL3_UNACK_DISCARD_CNT       GSN_LL_DISCARD_CNT

#define GSN_LL5_ACK_LOWATER_CNT         GSN_LL_LOWATER_CNT
#define GSN_LL5_ACK_HIWATER_CNT         GSN_LL_HIWATER_CNT
#define GSN_LL5_ACK_DISCARD_CNT         GSN_LL_DISCARD_CNT

#define GSN_LL5_UNACK_LOWATER_CNT       GSN_LL_LOWATER_CNT
#define GSN_LL5_UNACK_HIWATER_CNT       GSN_LL_HIWATER_CNT
#define GSN_LL5_UNACK_DISCARD_CNT       GSN_LL_DISCARD_CNT

#define GSN_LL9_ACK_LOWATER_CNT         GSN_LL_LOWATER_CNT
#define GSN_LL9_ACK_HIWATER_CNT         GSN_LL_HIWATER_CNT
#define GSN_LL9_ACK_DISCARD_CNT         GSN_LL_DISCARD_CNT

#define GSN_LL9_UNACK_LOWATER_CNT       GSN_LL_LOWATER_CNT
#define GSN_LL9_UNACK_HIWATER_CNT       GSN_LL_HIWATER_CNT
#define GSN_LL9_UNACK_DISCARD_CNT       GSN_LL_DISCARD_CNT

#define GSN_LL11_ACK_LOWATER_CNT        GSN_LL_LOWATER_CNT
#define GSN_LL11_ACK_HIWATER_CNT        GSN_LL_HIWATER_CNT
#define GSN_LL11_ACK_DISCARD_CNT        GSN_LL_DISCARD_CNT

#define GSN_LL11_UNACK_LOWATER_CNT      GSN_LL_LOWATER_CNT
#define GSN_LL11_UNACK_HIWATER_CNT      GSN_LL_HIWATER_CNT
#define GSN_LL11_UNACK_DISCARD_CNT      GSN_LL_DISCARD_CNT


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

extern void gsn_ll3_ul_ack_hiwater( dsm_watermark_type *wm_p, void *context );

extern void gsn_ll3_ul_ack_lowater( dsm_watermark_type *wm_p, void *context );

extern void gsn_ll3_ul_unack_hiwater( dsm_watermark_type *wm_p, void *context );

extern void gsn_ll3_ul_unack_lowater( dsm_watermark_type *wm_p, void *context );

extern void gsn_ll5_ul_ack_hiwater( dsm_watermark_type *wm_p, void *context );

extern void gsn_ll5_ul_ack_lowater( dsm_watermark_type *wm_p, void *context );

extern void gsn_ll5_ul_unack_hiwater( dsm_watermark_type *wm_p, void *context );

extern void gsn_ll5_ul_unack_lowater( dsm_watermark_type *wm_p, void *context );

extern void gsn_ll9_ul_ack_hiwater( dsm_watermark_type *wm_p, void *context );

extern void gsn_ll9_ul_ack_lowater( dsm_watermark_type *wm_p, void *context );

extern void gsn_ll9_ul_unack_hiwater( dsm_watermark_type *wm_p, void *context );

extern void gsn_ll9_ul_unack_lowater( dsm_watermark_type *wm_p, void *context );

extern void gsn_ll11_ul_ack_hiwater( dsm_watermark_type *wm_p, void *context );

extern void gsn_ll11_ul_ack_lowater( dsm_watermark_type *wm_p, void *context );

extern void gsn_ll11_ul_unack_hiwater( dsm_watermark_type *wm_p, void *context );

extern void gsn_ll11_ul_unack_lowater( dsm_watermark_type *wm_p, void *context );


extern void gsn_ll3_ul_ack_lowater_xid_reset( gas_id_t gas_id );

extern void gsn_ll3_ul_ack_hiwater_xid_reset( gas_id_t gas_id );

extern void gsn_ll3_ul_unack_lowater_xid_reset( gas_id_t gas_id );

extern void gsn_ll3_ul_unack_hiwater_xid_reset( gas_id_t gas_id );

extern void gsn_ll5_ul_ack_lowater_xid_reset( gas_id_t gas_id );

extern void gsn_ll5_ul_ack_hiwater_xid_reset( gas_id_t gas_id );

extern void gsn_ll5_ul_unack_lowater_xid_reset( gas_id_t gas_id );

extern void gsn_ll5_ul_unack_hiwater_xid_reset( gas_id_t gas_id );

extern void gsn_ll9_ul_ack_lowater_xid_reset( gas_id_t gas_id );

extern void gsn_ll9_ul_ack_hiwater_xid_reset( gas_id_t gas_id );

extern void gsn_ll9_ul_unack_lowater_xid_reset( gas_id_t gas_id );

extern void gsn_ll9_ul_unack_hiwater_xid_reset( gas_id_t gas_id );

extern void gsn_ll11_ul_ack_lowater_xid_reset( gas_id_t gas_id );

extern void gsn_ll11_ul_ack_hiwater_xid_reset( gas_id_t gas_id );

extern void gsn_ll11_ul_unack_lowater_xid_reset( gas_id_t gas_id );

extern void gsn_ll11_ul_unack_hiwater_xid_reset( gas_id_t gas_id );


extern void gsn_ll3_ul_ack_lowater_nas_susp( gas_id_t gas_id );

extern void gsn_ll3_ul_ack_hiwater_nas_susp( gas_id_t gas_id );

extern void gsn_ll3_ul_unack_lowater_nas_susp( gas_id_t gas_id );

extern void gsn_ll3_ul_unack_hiwater_nas_susp( gas_id_t gas_id );

extern void gsn_ll5_ul_ack_lowater_nas_susp( gas_id_t gas_id );

extern void gsn_ll5_ul_ack_hiwater_nas_susp( gas_id_t gas_id );

extern void gsn_ll5_ul_unack_lowater_nas_susp( gas_id_t gas_id );

extern void gsn_ll5_ul_unack_hiwater_nas_susp( gas_id_t gas_id );

extern void gsn_ll9_ul_ack_lowater_nas_susp( gas_id_t gas_id );

extern void gsn_ll9_ul_ack_hiwater_nas_susp( gas_id_t gas_id );

extern void gsn_ll9_ul_unack_lowater_nas_susp( gas_id_t gas_id );

extern void gsn_ll9_ul_unack_hiwater_nas_susp( gas_id_t gas_id );

extern void gsn_ll11_ul_ack_lowater_nas_susp( gas_id_t gas_id );

extern void gsn_ll11_ul_ack_hiwater_nas_susp( gas_id_t gas_id );

extern void gsn_ll11_ul_unack_lowater_nas_susp( gas_id_t gas_id );

extern void gsn_ll11_ul_unack_hiwater_nas_susp( gas_id_t gas_id );


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
void gsn_ul_clr_est_susp( gas_id_t gas_id, uint8 sapi );


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

extern void gsn_ll_dl_dat_put_mes_buf( gprs_ll_dl_dat_mes_t *mes_ptr );


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

extern gprs_ll_dl_sig_mes_t *gsn_ll_dl_sig_get_mes_buf( void );


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

extern void gsn_ll_dl_sig_put_mes_buf( gprs_ll_dl_sig_mes_t *mes_ptr );

/*===========================================================================
FUNCTION GSNDCP_CHECK_DATA_IN_SMD_TO_SNDCP_WM

DESCRIPTION
  This function checks if there is any data in the SMD->SNDCP WM. If there
  is any, then process.

PARAMETERS
  nsapi.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
void gsndcp_check_data_in_smd_to_sndcp_wm
(
  gas_id_t    gas_id,
  const uint8 nsapi
); 

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

extern void gsndcp_pdp_send_ul_npdu
(
  gas_id_t             gas_id,
  uint8                nsapi,
  dsm_item_type        **npdu
);

#endif /* INC_GSNDCP_V_G_H */

/*** EOF: don't remove! ***/
