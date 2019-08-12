/*============================================================================
  @file GRLCGRR.H

  GRLC to GRR Interface Definition

  FUNCTION SUMMARY
  
    Allows GRR to send messages to RLC

    grlc_get_cmd_buf_grr()
    grlc_put_cmd_grr()

  Copyright (c) 2008-2013 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
                
============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/inc/grlcgrr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
21/02/08   hv      Created header file

===========================================================================*/

#ifndef INC_GRLCGRR_H
#define INC_GRLCGRR_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "comdef.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "queue.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/*----------------------------------------------------------------
** Definitions of all signals sent from RLC to MAC on both uplink
** and downlink operations. The RL_ prefix denotes the direction
** the message.
**----------------------------------------------------------------
*/
typedef enum
{

  /*----------- UPLINK (only) -----*/

  /* Request RLC to enter GAN access mode */
  GRR_RLC_GAN_PS_ACCESS_REQ = 0x80,

  /* Request RLC to suspend GAN access mode */
  GRR_RLC_NO_GAN_PS_ACCESS_REQ = 0x81,

  /* Request RLC to exit GAN access mode */
  GRR_RLC_GPRS_PS_ACCESS_REQ = 0x82

} rl_sig_t;


/*----------------------------------------------------------------
** Structure for GRR to request RLC to enter GAN access mode
**
** rl: r for GRR, l for RLC
**----------------------------------------------------------------
*/
typedef struct
{
  /* signal id: 
  */
  rl_sig_t    sig;

} rl_gan_ps_access_req_t;

/*----------------------------------------------------------------
** Structure for GRR to request RLC to enter GAN access mode
**
** rl: r for GRR, l for RLC
**----------------------------------------------------------------
*/
typedef struct
{
  /* signal id: 
  */
  rl_sig_t    sig;

} rl_gan_no_ps_access_req_t;


/*----------------------------------------------------------------
** Structure for GRR to request RLC to exit GAN access mode
**
** rl: r for GRR, l for RLC
**----------------------------------------------------------------
*/
typedef struct
{
  /* signal id: 
  */
  rl_sig_t    sig;

} rl_gprs_ps_access_req_t;

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
    rl_sig_t                    sig;

    rl_gan_ps_access_req_t      gan_ps_access_req;
    rl_gan_no_ps_access_req_t   gan_no_ps_access_req;
    rl_gprs_ps_access_req_t     gprs_ps_access_req;

  } msg;

} grr_rlc_msg_t;




/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      RLC_GET_CMD_BUF_GRR()
===
===  DESCRIPTION
===
===   Return a pointer to a buffer for GRR to build a message destined 
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
grr_rlc_msg_t *grlc_get_cmd_buf_grr (void);


/*===========================================================================
===
===  FUNCTION      RLC_PUT_CMD_GRR()
===
===  DESCRIPTION
===
===   Return a buffer for GRR to build a message destined for RLC
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
void grlc_put_cmd_grr
(
  grr_rlc_msg_t *msg_ptr
);

#endif /* INC_GRLCGRR_H */

/*** EOF: don't remove! ***/
