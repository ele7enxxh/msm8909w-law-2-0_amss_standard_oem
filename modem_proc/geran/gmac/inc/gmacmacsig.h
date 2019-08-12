/*****************************************************************************
***
*** TITLE
***
***  Internal signals
***
***
*** DESCRIPTION
***
***  Defines the ENUM for all the signals used for internal signaling  .
***
***  Defines the Union that holds all the signal struct definitions for
***  internal signaling within MAC
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/inc/gmacmacsig.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 10-03-30   tjw     Explicitly include dependent headers to make SU API files
***                    stand alone
*** 06/27/01   TLX     Created
***
*****************************************************************************/

#ifndef INC_GMACMACSIG_H
#define INC_GMACMACSIG_H

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


#include "queue.h"
#include "gmactimers.h"

#include "csn.h"

#include "grlcmac.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/


/*************************************************************************
* internal MAC signals
*************************************************************************/
typedef enum
{
  /* null mode to idle mode - numbering 0x00 - 0x0f */
  MAC_NI_UL_1P_ASS        = 0x00,
  MAC_NI_UL_2P_ASS        = 0x01,
  MAC_NI_DL_ASS           = 0x02,
  MAC_NT_UL_DYNAMIC       = 0x03,
  MAC_NT_DL_DYNAMIC       = 0x04,
#ifdef FEATURE_GPRS_PS_HANDOVER
  MAC_NT_PS_HANDOVER    = 0x05,
#endif /*FEATURE_GPRS_PS_HANDOVER*/
  /* idle mode to null mode - numbering 0x10 - 0x1f */
  MAC_IN_GPRS_SUSPENSION            = 0x10,
  MAC_IN_CAMPED_ON_CELL             = 0x11,
  MAC_IN_CONTRES_FAIL               = 0x12,
  MAC_IN_SEND_PCCF                  = 0x13,
  MAC_IN_NO_SERVICE                 = 0x14,
  MAC_IN_PCCO_CAMPED_ON_TARGET_CELL = 0x15,
  MAC_IN_PCCO_FAILED                = 0x16,
  MAC_IN_PCCO_CAMPED_ON_CELL        = 0x17,

  /* idle mode to transfer mode - numbering 0x20 - 0x2f */
  MAC_IT_UL_FIXED         = 0x20,
  MAC_IT_UL_DYNAMIC       = 0x21,
  MAC_IT_DL_FIXED         = 0x22,
  MAC_IT_DL_DYNAMIC       = 0x23,
  MAC_IT_UL_DYN_EGPRS_PUA_RCVD_IN_CONT_RES = 0x24,

  /* idle mode to idle mode - numbering 0x30 - 0x3f */
  MAC_II_RE_ESTABLISH     = 0x30,

  /* transfer mode to idle - numbering 0x40 - 0x4f */
  MAC_TI_RE_ESTABLISH     = 0x40,
  MAC_TI_T3164_UL_RETRY   = 0x41,
  MAC_TI_CELL_RESEL_IND   = 0x42,
  MAC_TI_PCCO_CELL_RESEL_IND     = 0x43,

  /* transfer mode to null - numbering 0x50 - 0x5f */
  MAC_TN_CELL_RESEL_IND   = 0x50,
  MAC_TN_GPRS_SUSPENSION  = 0x51,

  /********************************************************
  *                  IPA NEW SIGNALS                      *
  ********************************************************/
  MAC_TN_NO_SERVICE        =0x52,

#ifdef FEATURE_GSM_EDTM
  MAC_TN_EDTM_52MF_TO_51MF_CS_CALL_ACTIVE = 0x53,
#endif /*FEATURE_GSM_EDTM*/

#ifdef FEATURE_GSM_DTM
  MAC_NT_UL_DTM_DYNAMIC   = 0x60,
  MAC_NT_DL_DTM_DYNAMIC   = 0x61,
  MAC_NT_UL_DL_DTM_DYNAMIC = 0x62,
#endif /*FEATURE_GSM_DTM*/

  /* timer signals - numbering 0x80 - 0xff */
  MAC_TIMER_T3162         = 0x80, /* Wait for PUA mag after reception of PQN msg                */
  MAC_TIMER_T3164         = 0x81, /* Wait for USF After Assignment                              */
  MAC_TIMER_T3166         = 0x82, /* Wait for PUAN after sending of first data block            */
  MAC_TIMER_T3168         = 0x83, /* Wait for PUA message                                       */
  MAC_TIMER_T3170         = 0x84, /* Wait for PUA msg after (M+1) PCR or after reception of PAR */
  MAC_TIMER_T3172         = 0x85, /* Prohibit packet access in cell after PAR has been rxd      */
  MAC_TIMER_T3180         = 0x86, /* Wait for USF After Data Block                              */
  MAC_TIMER_T3184         = 0x87, /* No Ack/Nack Received                                       */
  MAC_TIMER_T3186         = 0x88, /* Supervision of the random access procedure                 */

  MAC_TIMER_T3200_1       = 0x89, /* RLC/MAC control message reassembly guard timers            */
  MAC_TIMER_T3200_2       = 0x8a,
  MAC_TIMER_T3200_3       = 0x8b,
  MAC_TIMER_T3200_4       = 0x8c,
  MAC_TIMER_T3200_5       = 0x8d,
  MAC_TIMER_T3200_6       = 0x8e,
  MAC_TIMER_T3200_7       = 0x8f,
  MAC_TIMER_T3200_8       = 0x90,

  MAC_TIMER_T3164_TRANSFER = 0x92

#ifdef FEATURE_GSM_EDTM
  ,MAC_TIMER_T3196            = 0x93,
   MAC_TIMER_PKT_CS_REQ_RETRY = 0x94
#endif /*FEATURE_GSM_EDTM*/
#ifdef FEATURE_GPRS_PS_HANDOVER
  ,MAC_TIMER_T3216            = 0x95,
   MAC_TIMER_T3218            = 0x96
#endif /*FEATURE_GPRS_PS_HANDOVER*/


}  mac_mac_sig_id_t;


/*************************************************************************
* Internal message declarations
*************************************************************************/

typedef struct
{
  uint8                                       idle_realloc_req_count;
  boolean                                     waiting_to_connect_realloc_flg;
  mr_ul_alloc_cnf_t                           rlc_realloc_data;

  boolean                                     pending_realloc_req;
} mac_it_ul_dynamic_t;

typedef struct
{
  packet_uplink_assignment_t                  *egprs_c_r_pua_ptr;
  uint8                                        idle_realloc_req_count;  
  
  boolean                                     pending_realloc_req;  
} mac_it_ul_dyn_egprs_pua_rcvd_in_cont_res_t;


typedef union
{
  mac_it_ul_dynamic_t                           mac_it_ul_dynamic;
  mac_it_ul_dyn_egprs_pua_rcvd_in_cont_res_t    mac_it_ul_epua_rcvd;   
} mac_mac_msg_u;


/*************************************************************************
*   GRR to MAC signal defintion
*************************************************************************/
typedef struct
{
  q_link_type        link;
  byte               task_id;
  mac_mac_sig_id_t   sig_id;
  mac_mac_msg_u      msg;
  gas_id_t           gas_id;
} mac_int_sig_t;

#endif /* INC_GMACMACSIG_H */
/*** EOF: don't remove! ***/
