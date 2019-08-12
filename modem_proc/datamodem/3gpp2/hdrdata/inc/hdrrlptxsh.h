#ifndef HDRRLPTXSH_H
#define HDRRLPTXSH_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               H D R   R A D I O   L I N K   P R O T O C O L  
               
    T R A N S M I T  S E T  H A N D L E  M A N A G E M E N T  M O D U L E

GENERAL DESCRIPTION
  Whenever data is given to PCP to be transmitted, it is associated with a 
  handle. The handle is used to identify the data that is given to PCP for 
  a particular carrier, we call this as a set of handles.
  
  This module manages the processing of each sets and their processing when 
  PCP returns the status of each of these sets. 
  
  Uses of the handles:
    The handles are part of the internal data structure that RLP maintains
    regarding the status of transmission  of a particular RLP data. 
    While processing after receiving the status. The following conditions are 
    possible for each set:
    1. Packet transmission was not attempted (that is packet was cancelled)
    2. Packet transmission was attempted.
    3. Packet transmission was attempted, but was not successful. (Packet was
       MARQ-ed)  
  
EXTERNALIZED FUNCTIONS
  hdrrlptxsh_powerup_init

INITIALIZATION AND SEQUENCING REQUIREMENTS
  hdrrlptxph_powerup_init() needs to have been called before any of the
  other functions can be used.

 Copyright (c) 2005 - 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrrlptxsh.h#1 $

when       who   what, where, why
--------   ---   -----------------------------------------------------------
11/18/11   vpk   HDR SU API cleanup
05/22/07   spn   Created module.

===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "customer.h"
#include "dsrlpi.h"
#include "hdrrlptxq.h"
#include "hdrcom_api.h"
#include "queue.h"
#include "hdrrlptxph.h"


/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/*
 * Typedef for the handle id
 */
typedef uint32 hdrrlptxsh_handle_type;

/* 
 * Data type storing packet for each carrier, that we have build packets for.
 * handle_id --> uniquely identifies a set of handles for a given 
 *               ( combo, carrier ) combination.
 * pkt_handles_q --> Queue of packet handles associated with this set/carrier.
 *                  Each element is of type hdrrlptxph_type.
 */
typedef struct 
{
  q_link_type                 link_next;
  hdrrlptxsh_handle_type      handle_id;
  q_type                      pkt_handles_q;
}hdrrlptxsh_per_carrier_type;



/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/


/*===========================================================================

FUNCTION HDRRLPTXSH_POWERUP_INIT

DESCRIPTION
  Called to initialize the handle module at powerup  

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void hdrrlptxsh_powerup_init
(
  void
);

/*===========================================================================

FUNCTION HDRRLPTXSH_INIT

DESCRIPTION
  Called to initialize the module at powerup for a particular ROUTE

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void hdrrlptxsh_init
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr    
);

/*===========================================================================

FUNCTION HDRRLPTXSH_RESET

DESCRIPTION
  Called to cleanup the module at for a particular ROUTE

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void hdrrlptxsh_reset
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr   
);

/*===========================================================================

FUNCTION HDRRLPTXSH_PARSE_SETID

DESCRIPTION
  Called to parse the setid to return the required information.

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS
===========================================================================*/
void hdrrlptxsh_parse_setid
(
  hdrrlptxsh_handle_type setid,
  uint8* combo,
  uint8* carrier,
  uint8* marq_id,
  uint8* marq_index
);

/*===========================================================================

FUNCTION HDRRLPTXSH_ADD_PACKET_HANDLE

DESCRIPTION
  Called to insert a new packet handle into the corresponding set. 

DEPENDENCIES
  None.

RETURN VALUE
  Id for the set of handles that the packet handle is inserted to. This is 
  the handle that needs to be returned to PCP.

SIDE EFFECTS
===========================================================================*/
hdrrlptxsh_handle_type hdrrlptxsh_add_packet_handle
(
  uint8 combo,
  uint8 carrier,
  hdrrlptxph_type*  pkt_handle_ptr,
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr
);

/*===========================================================================

FUNCTION HDRRLPTXSH_PROCESS_STATUS

DESCRIPTION
  Called after PCP has processed the data given to it. This function is a 
  callback from PCP to process the status of all the handles given to it
  in one go.
  STATUS IS NOT RETURNED FOR all the combos given to PCP. Status is returned
  for the accepted combo.  In the accepted combo the status of all the sets
  ( accepted / cancelled ) are returned.  The order of the sets in the 
  parameter "tx_data_handle" is in the order that RLP has given to PCP.
  IF NONE OF THE COMBOS are accepted then a special COMBO_ID is returned.

DEPENDENCIES
  None.

RETURN VALUE
  Return TRUE if the status was processed correctly, 
  Return FALSE if not.
  IF false was returned, then the parent function should ASSERT(), as 
  there is no way we can recover.

SIDE EFFECTS
===========================================================================*/
void hdrrlptxsh_process_status
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr,          /* RLP-specific ctl block    */
  uint8 combo,
  hdrcp_pcp_carrier_tx_outcome_type tx_data_handle[],
  uint8 num_sets
);



#endif /* _HDRRLPTXSH_H */
