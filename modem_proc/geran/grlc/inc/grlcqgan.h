/*============================================================================
  @file GRLCQGAN.H

  GRLC to Qualcomm GAN Interface Definition

  FUNCTION SUMMARY
  
    Allows RLC to receive messages from (Q)GAN

    grlc_get_cmd_buf_qgan()
    grlc_put_cmd_qgan()

  Copyright (c) 2008-2013 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
                
============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/inc/grlcqgan.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
25/02/08   hv      Created header file

===========================================================================*/

#ifndef INC_GRLCQGAN_H
#define INC_GRLCQGAN_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "comdef.h"

#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "gu_qgan_api.h"
#include "grlci.h"
/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/
/*----------------------------------------------------------------
** Definitions of all signals sent from QGAN to RLC
**----------------------------------------------------------------
*/
typedef enum
{
  /* Acknowledge the Ack-mode PDU sent GAN for delivery as received
  */
  QGAN_GRR_DATA_CNF = 0x80,

  /* Acknowledge the Unack-mode PDU sent GAN for delivery as received
  */
  QGAN_GRR_UNITDATA_CNF = 0x81,
  
  /* Request RLC to exit GAN access mode */
  QGAN_PS_CONNECT_REQ = 0x82,

  /* GAN Sending residual PDU to RLC after GAN to GERAN Handover */
  QGAN_PDU_REDIRECT_PDU_REQ = 0x83,

  /* RLC sending CNF after DL_SWITCH , current sent imm. after receiving 
  ** QGAN_PDU_REDIRECT_PDU_REQ
  */
  QGAN_RLC_PS_CONNECT_CNF = 0x84,

  /* If the requested LLC PDU could not be successfully sent to the network
  ** (UDP_SendTo() failed), 
  */
  QGAN_GRR_STATUS_IND = 0x85

} ql_sig_t;

/*----------------------------------------------------------------
** Structure for QGAN to request RLC 
**
** ql: r for qGAN, l for RLC
**----------------------------------------------------------------
*/
typedef struct
{
  /* signal id: 
  */
  ql_sig_t    sig;

  uint8*      buf_ptr;

} ql_qgan_grr_data_cnf_t;

/*----------------------------------------------------------------
** Structure for QGAN to request RLC 
**
** rl: r for GRR, l for RLC
**----------------------------------------------------------------
*/
typedef struct
{
  /* signal id: 
  */
  ql_sig_t    sig;

  uint8*      buf_ptr;

} ql_qgan_grr_unitdata_cnf_t;

/*----------------------------------------------------------------
** Structure for QGAN to request RLC 
**
** rl: r for GRR, l for RLC
**----------------------------------------------------------------
*/
typedef struct
{
  /* signal id: 
  */
  ql_sig_t    sig;

} ql_qgan_ps_connect_req_t;


/*----------------------------------------------------------------
** Structure for QGAN to request RLC 
**
** rl: r for GRR, l for RLC
**----------------------------------------------------------------
*/
typedef struct
{
  /* signal id: 
  */
  ql_sig_t    sig;

} ql_qgan_pdu_redirect_pdu_req_t;

/*----------------------------------------------------------------
** Structure for QGAN to request RLC 
**
** rl: r for GRR, l for RLC
**----------------------------------------------------------------
*/
typedef struct
{
  /* signal id: 
  */
  ql_sig_t    sig;

} ql_qgan_ps_connect_cnf_t;

/*----------------------------------------------------------------
** Structure for QGAN to request RLC 
**
** rl: r for GRR, l for RLC
**----------------------------------------------------------------
*/
typedef struct
{
  /* signal id:
  */
  ql_sig_t    sig;
  uint8       cause;

} ql_qgan_grr_status_ind_t;

/*-------------------------------
** GRR to RLC message definition
**-------------------------------
*/
typedef struct
{
  q_link_type        link;

  /*---------------------------------------
  ** Union of all messages from GRR to RLC 
  **---------------------------------------
  */
  union
  {
    ql_sig_t                        sig;

    ql_qgan_grr_data_cnf_t          qgan_grr_data_cnf;
    ql_qgan_grr_unitdata_cnf_t      qgan_grr_unitdata_cnf;
    ql_qgan_ps_connect_req_t        qgan_ps_connect_req;
    ql_qgan_pdu_redirect_pdu_req_t  qgan_pdu_redirect_pdu_req;
    ql_qgan_ps_connect_cnf_t        qgan_ps_connect_cnf;
    ql_qgan_grr_status_ind_t        qgan_grr_status_ind;
  } msg;

} ql_msg_t;

/*===========================================================================
===
===  FUNCTION      RLC_GET_CMD_BUF_QGAN()
===
===  DESCRIPTION
===
===   Return a pointer to a buffer for GGAN to build a message destined 
===   for RLC
===    
===
===  DEPENDENCIES
===
===    none
===  
===  RETURN VALUE
===
===    pointer to a buffer
===
===  SIDE EFFECTS
===
===    none
===  
===========================================================================*/
ql_msg_t *grlc_get_cmd_buf_qgan (void);


/*===========================================================================
===
===  FUNCTION      RLC_PUT_CMD_QGAN()
===
===  DESCRIPTION
===
===   Return a buffer for QGAN to build a message destined for RLC
===    
===
===  DEPENDENCIES
===
===    none
===  
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===  
===========================================================================*/
void grlc_put_cmd_qgan
(
  ql_msg_t *msg_ptr
);


#endif /* INC_GRLCQGAN_H */
