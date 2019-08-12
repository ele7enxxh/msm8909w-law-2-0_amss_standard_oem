#ifndef HDRRLPMARQ_H
#define HDRRLPMARQ_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               H D R   R A D I O   L I N K   P R O T O C O L  
               
                 M A R Q  M A N A G E M E N T  M O D U L E

GENERAL DESCRIPTION
  
EXTERNALIZED FUNCTIONS
  hdrrlpmarq_powerup_init

INITIALIZATION AND SEQUENCING REQUIREMENTS
  hdrrlpmarq_powerup_init() needs to have been called before any of the
  other functions can be used.

 Copyright (c) 2005,2006, 2007 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrrlpmarq.h#1 $

when       who   what, where, why
--------   ---   -----------------------------------------------------------
05/22/07   spn   Created module.

===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "customer.h"
#include "hdrrlptxq.h"
#include "dsrlpi.h"
#include "queue.h"
#include "hdrrlptxph.h"

/*===========================================================================
                        DATA  DECLARATIONS
===========================================================================*/

/*
 *  Maximum number of packets per subframe, that can be built across all RLPs
 */
#define HDRRLPMARQ_MAX_PKTS_PER_SUBFRAME 15

/*
 * Information for a particular carrier/set, that has been accepted. 
 * This is a part of a queue, pointed to by each element of the MARQ 
 * Array.
 * link_next --> required by the queue library
 * handle_id --> set_id which has been accepted.
 * pkt_info_handle_q --> Queue to packet handles, that have been transmitted 
 *                       in this set. hdrrlpmarq_pkt_info_type are elements 
 *                       of this queue.
 */
typedef struct 
{
  q_link_type link_next;
  uint32      handle_id;
  uint32      num_retx_data_units;
  q_type      pkt_info_handle_q;
}hdrrlpmarq_carrier_info_type;

/*
 * Information stored per packet that has been transmitted. 
 * link_next --> required by the queue library
 * first_seq --> First sequence number of the packet that is sent
 * last_seq  --> Last sequence number of the packet taht is sent
 * txed_info --> Pointer to an element in the TX'ed queue, 
 *               for quick reference, so that a linear search is 
 *               not done while retransmitting data.
 * NOTE: THE SEQ NUMBERS ARE REQUIRED TO ASSERT() THAT WHEN WE PROCESS
 *       FAIL WE ARE ACTUALLY REFERRING TO THE TXQ_ITEM, WE THINK WE ARE 
 *       REFERRING TO, AS THE TXQ_ITEM MAY BE REUSED FOR SOME OTHER "DATA"
 *       BUT THE POINTER IS STILL VALID.
 */
typedef struct 
{
  q_link_type link_next;
  uint32 first_seq;
  uint32 last_seq;
  hdrrlptxq_type* txed_info;
} hdrrlpmarq_pkt_info_type;

/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/


/*===========================================================================

FUNCTION HDRRLPMARQ_POWERUP_INIT

DESCRIPTION
  Called to initialize the handle module at powerup  

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void hdrrlpmarq_powerup_init
(
  void
);

/*===========================================================================

FUNCTION HDRRLPMARQ_INIT

DESCRIPTION
  Called to initialize the handle module at powerup  

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void hdrrlpmarq_init
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr
);

/*===========================================================================

FUNCTION HDRRLPMARQ_RESET

DESCRIPTION
  Called to cleanup the module at for a particular ROUTE

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void hdrrlpmarq_reset
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr   
);

/*===========================================================================

FUNCTION HDRRLPMARQ_GET_INFO

DESCRIPTION
  Called to return the MARQ index and the MARQ id for a given rrcb

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void hdrrlpmarq_get_info
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,
  uint8 *marq_id,
  uint8 *marq_index
);

/*===========================================================================
FUNCTION HDRRLPMARQ_ADD_PKT_INFO

DESCRIPTION
  Called to add a packet info to the MARQ Data Structure

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  This function will ASSERT if we cannot add the packet info.
===========================================================================*/
void hdrrlpmarq_add_pkt_info
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,
  uint32  setid,
  hdrrlptxph_type *tph_ptr,
  hdrrlptxq_type *txq_ptr  
);
/*===========================================================================
FUNCTION HDRRLPMARQ_PREPARE_NEXT_INDEX

DESCRIPTION
  Called to increment the index into the Array, and in process free the 
  elements in the next index. Make sure that the index wraps around, to 1.
  and increment the marq id as well.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  This function will ASSERT if we cannot add the packet info.
===========================================================================*/
void hdrrlpmarq_prepare_next_subframe
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr  
);

/*===========================================================================
FUNCTION HDRRLPMARQ_HANDLE_TRANSMISSION_FAIL

DESCRIPTION
  Called to Add MARQ'ed packets to the retransmit queue, so that they can 
  be sent in the next subframe.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  This function will ASSERT if we cannot find the packets in the Array. 
===========================================================================*/
void hdrrlpmarq_handle_transmission_fail
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,
  uint8 marq_index,
  uint32 tx_data_handle
);


/*===========================================================================
FUNCTION HDRRLPMARQ_ADD_RETX_INFO

DESCRIPTION
  Called to update the retransmit info maintained by the MARQ data structure.

DEPENDENCIES
  It will update the MARQ element pointed to by the index. Shall be called
  only for retransmit data handle type. The Caller needs to make sure 
  that the function is called for the correct handle

RETURN VALUE
  None.
  
SIDE EFFECTS
  None. 
===========================================================================*/
void hdrrlpmarq_add_retx_info
( 
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr, 
  uint32 curr_setid, 
  uint32 retx_data_units
);
#endif /* HDRRLPMARQ_H */
