/*****************************************************************************
***
*** TITLE
***
***  GPRS LAYER 1 AND RLC COMMON DEFINITIONS
***
***
*** DESCRIPTION
***
***  This file contains the defines and data structures common to both GPRS
***  Portable Layer 1 and the RLC sub-layer.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/gl1rlccmn.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 23/07/14    pjr    CR682764 TP IR Feature - implement IR mem Full 
***                    handling from GFW to GRLC
*** 10/04/01    tjw    Add customer.h and comdef.h to all SU API files
*** 03/20/01    hv     created
*** 03/27/01    hv     Setup skeleton definitions to be refined by L1 experts
*** 02/28/03    hv     Added TXA_PENDING_PACK state for RLC data blocks
*** 23/01/07    og     Resolve RVCT2.1 compiler warnings, merge from the
***                    gpl1_hummer.10.00.43.01 branch.
***
*****************************************************************************/

#ifndef INC_GL1RLCCMN_H
#define INC_GL1RLCCMN_H

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


#include "gprsdef.h"
#include "gprsdef_g.h"
#include "grlcmaccmn.h"
#include "grlci.h"
#include "queue.h"
#include "csn.h"


/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/*---------------------------------------------------------
** Transmit acknowledge states.
** Possible values for each element of the uplink transmit
** acknowledge state array V(B).
**---------------------------------------------------------
*/
typedef enum
{
  /* Un-used state only valid during initialisation
  **
  ** Internal state.
  */
  TXA_INACTIVE,


  /* State in un-ack mode
  */
  TXA_UNACK,

  /* Negatively acked by the n/w (from RBB) and the t3198 timer fpr this
  ** block has also expired.
  */
  TXA_NACK,


  /* Data block ready for tx and has never been transmitted.
  */
  TXA_PTX,

  /* After a NACK block has been sent to the FIFO, it will hold PENDING_PACK status
  ** until it is confirmed sent (post_tx_actions()) which will make it PACK. This
  ** prevents a NACK block being sent again if PL1 is slow in sending it
  */
  TXA_PENDING_PACK,

  /* After a TXA_NACK block is transmitted it becomes TXA_PACK
  */
  TXA_PACK,


  /* A TXA_PACK data block becomes TXA_ACK when the n/w acknowledges
  ** it as being received via the RBB (rx bitmap) of the PUAck msg.
  */
  TXA_ACK,


  /* Element falls outside the active tx window
  */
  TXA_INVALID

} tx_ack_state_t;


/*----------------------------------------------------------------
** Define the EPDAN Request signal enumeration from PL1 to RLC DL 
**----------------------------------------------------------------
*/
typedef enum
{
  PR_DL_EPDAN_REQ      /* PL1 to EGPRS RLC DL EPDAN REQ */
} pr_sig_t;

/*----------------------------------------------------------------
** Define the EPDAN Request message structure
**----------------------------------------------------------------
*/
typedef struct
{
  q_link_type           link;

  pr_sig_t              sig;

  boolean               ms_out_of_mem;  /* TRUE if out of IR memory, FALSE otherwise */

  /* BSN of block with polling
  */
  uint16                bsn;

  /* available length EPDAN with quality report
  */
  uint16                avail_len_no_qrep;

  /* available length of EPDAN without quality report
  */
  uint16                avail_len_qrep;

  /* pointer to quality report to be passed to MAC by RLC (no processing by RLC)
  */
  egprs_chn_qual_rpt_t  *qrep_ptr;

  /* frame number, timeslot, RRBP and ESP bit for completeness
  */
  boolean               include_chan_req;
  uint32                frame_no;
  uint8                 timeslot;
  uint8                 rrbp;
  uint8                 esp;

} pl1_rlc_msg_t;

/*-------------------------------------------------------------------------
** Dummy message definition required to send messages to GTEST to simulate
** setting a signal mask in PL1 space.
**-------------------------------------------------------------------------
*/
typedef struct
{
  q_link_type         link;

  /* dummy parameter
  */
  uint32              prim;

} gprs_pl1_rlc_msg_t;



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

/*===========================================================================
===
===  MACRO         xxx
===
===  DESCRIPTION
===
===    xxx.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/




/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      xxx
===
===  DESCRIPTION
===
===    xxx.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/


#endif /* INC_GL1RLCCMN_H */

/*** EOF: don't remove! ***/
