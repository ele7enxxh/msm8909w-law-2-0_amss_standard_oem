/*============================================================================
  @file GRLCGAN.H

  GRLC Generic Access

  FUNCTION SUMMARY
  
    x()

      Returns 

  Copyright (c) 2008-2013 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
                
============================================================================*/

/*============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/inc/grlcgan.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/19/08   hv      Created header file

===========================================================================*/



/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#ifndef INC_GRLCGAN_H
#define INC_GRLCGAN_H

#include "comdef.h"
#include "gu_qgan_api.h"
#include "grlcgan.h"
#include "grlcqgan.h"

/*****************************************************************************
***
***     Public type defines & functions
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION     GRLC_GAN_ROVE_IN
===
===  DESCRIPTION
===               Enter GAN access mode from NULL.
===               Expect to be in already in suspension, ready to receive and
===               deliver RAU
===
===  PARAMS
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    void
===
===  SIDE EFFECTS
===
===    xxxx
===
===========================================================================*/
void grlc_gan_rove_in (void);

/*===========================================================================
===
===  FUNCTION     GRLC_UL_HANDLE_GRR_GAN_NO_PS_ACCESS
===
===  DESCRIPTION
===               Handle the roving out of GAN
===
===  PARAMS
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    void
===
===  SIDE EFFECTS
===
===    xxxx
===
===========================================================================*/
void grlc_ul_handle_grr_gan_no_ps_access (void);

/*===========================================================================
===
===  FUNCTION     GRLC_GAN_XFER
===
===  DESCRIPTION
===               Send PDU(s) to the GAN client in ack (TCP) or unack mode (UDP).
===               In ack mode, the number of PDUs sent should not exceed buffer
===               of GAN client
===
===               Number of PDUs sent to GAN is limited by GAN_MAX_BUF_SIZE. 
===               GAN echoes back a confirmation when the PDU is transmitted.
===               The confirmation is IN THE ORDER of PDUs sent
===
===  PARAMS
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    void
===
===  SIDE EFFECTS
===
===    xxxx
===
===========================================================================*/
void grlc_gan_xfer (void);

/*===========================================================================
===
===  FUNCTION     GRLC_GAN_PROCESS_ACK
===
===  DESCRIPTION
===               Free the PDU acked by GAN 
===
===  PARAMS
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    void
===
===  SIDE EFFECTS
===
===    xxxx
===
===========================================================================*/
void grlc_gan_process_ack(const ql_msg_t* msg_u);

/*===========================================================================
===
===  FUNCTION     GRLC_GAN_STATUS_IND
===
===  DESCRIPTION
===               Free the PDU failed to be delivered by GAN
===               until cur_pdu ptr.
===
===  PARAMS
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    void
===
===  SIDE EFFECTS
===
===    xxxx
===
===========================================================================*/
void grlc_gan_status_ind(void);

/*===========================================================================
===
===  FUNCTION     GRLC_UL_HANDLE_GRR_GAN_NO_PS_ACCESS
===
===  DESCRIPTION
===               Handle the roving out of GAN
===
===  PARAMS
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    void
===
===  SIDE EFFECTS
===
===    xxxx
===
===========================================================================*/
void grlc_ul_handle_grr_gan_no_ps_access (void);

/*===========================================================================
===
===  FUNCTION     GRLC_GAN_HANDLE_GU_PS_CONNECT_REQ
===
===  DESCRIPTION
===               Handle the GU_PS_CONNECT_REQ ( PS_GAN_ULSWITCH_REQ)
===
===  PARAMS
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    void
===
===  SIDE EFFECTS
===
===    xxxx
===
===========================================================================*/
void grlc_gan_handle_gu_ps_connect_req(void);

/*===========================================================================
===
===  FUNCTION     GRLC_SEND_GRR_PS_ACCESS_CNF
===
===  DESCRIPTION
===               Send RLC_GRR_PS_ACCESS_CNF to GRR
===
===  PARAMS
===               None
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    void
===
===  SIDE EFFECTS
===
===    xxxx
===
===========================================================================*/
void grlc_send_grr_gprs_ps_access_cnf(void);

/*===========================================================================
===
===  FUNCTION     GRLC_SEND_GRR_GAN_NO_PS_ACCESS_CNF
===
===  DESCRIPTION
===               Send RLC_GRR_GAN_NO_PS_ACCESS_CNF to GRR
===
===  PARAMS
===               None
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    void
===
===  SIDE EFFECTS
===
===    xxxx
===
===========================================================================*/
void grlc_send_grr_gan_no_ps_access_cnf(void);

#endif /* INC_GRLCGAN_H */
