#ifndef HDRRLPTXPH_H
#define HDRRLPTXPH_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               H D R   R A D I O   L I N K   P R O T O C O L  
               
      P A C K E T   H A N D L E   M A N A G E M E N T   M O D U L E

GENERAL DESCRIPTION
  Whenever data is given to PCP to be transmitted, it is associated with a 
  handle. The handle is used to identify the data that is given to PCP.
  
  This module manages the allocation and freeing of the handles associated
  with each of transmitted data.
  
  Uses of the handles:
    The handles are part of the internal data structure that RLP maintains
    regarding the status of transmission  of a particular RLP data. 
    While processing after receiving the status, the handle is used to 
    identify the RLP packet. The following conditions are possible for each 
    packet:
    1. Packet transmission was not attempted (that is packet was cancelled)
    2. Packet transmission was attempted.
    3. Packet transmission was attempted, but was not successful. (Packet was
       MARQ-ed)  
  
EXTERNALIZED FUNCTIONS
  hdrrlptxph_powerup_init
  hdrrlptxph_new_handle
  hdrrlptxph_free_handle
  hdrrlptxph_reset_handle

INITIALIZATION AND SEQUENCING REQUIREMENTS
  hdrrlptxph_powerup_init() needs to have been called before any of the
  other functions can be used.

 Copyright (c) 2005,2006 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrrlptxph.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when         who        what, where, why
--------     ---        -------------------------------------------------------
05/18/07     spn        Renamed the module from hdrrlphandle to hdrrlptph
05/14/07     spn        Redesigned the module to use the queuelibrary. 
12/21/05     sy         Redesigned the module to accomadate the handle reset
                        and made it a global pool of handles for new/rexmit/flush
                        for all the RLP's.
05/19/05     vas        Created module.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "queue.h"
#include "hdrrlprtx.h"
#include "hdrrlpi.h"


/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
   The handle can either be a data structure containing information regarding
   new data transmitted, or it can contain info about re-transmit data. This
   enum indicates the type of the handle 
---------------------------------------------------------------------------*/
/*
   Comments about the values chosen here:
   0 - Set to NULL handle
   1,2,3 chosen to represent three kinds of retransmit handle. This is done
   so as to be consistent with the hdrrlprtx_reason_enum_type
   4 - New data handle
   5 - Flush data handle

*/
typedef enum
{
  HDRRLP_NULL_HANDLE = 0,
  HDRRLP_RETX_HANDLE = 1,        /* Retransmit data handle type */
  HDRRLP_MARQ_DATA_HANDLE = 2,    /* Handle for data bytes that  
                                    are sent as a result of
                                    being MARQ'ed               */  
  HDRRLP_FALSE_RETX_HANDLE = 3,  /* Handle for data bytes that 
                                      have not been transmitted, but
                                      entered into the retx queue */                                  
  HDRRLP_NEW_DATA_HANDLE = 4,    /* New data handle type        */
  HDRRLP_FLUSH_DATA_HANDLE = 5  /* Flush data handle type      */
} hdrrlptxph_enum_type;

/*---------------------------------------------------------------------------
  Generic Handle head definition, that is included in specific handles 
  (like, newdata, retx etc ). This is used this way so that it could be used
  along with the Queue library, and hence we have the link_next which of 
  q_link_type.
  Because of this the hdrrlptxph_type, needs to be the first element in
  the "specific" handle itself.   
---------------------------------------------------------------------------*/
struct hdrrlptxph_type_node
{
  q_link_type  link_next;
  hdrrlptxph_enum_type handle_type;  
};
typedef struct hdrrlptxph_type_node hdrrlptxph_type;

struct hdrrlptxph_newdata_node
{
  /* This needs to be the first element */
  hdrrlptxph_type  handle_header;
  hdrrlpi_new_data_pkt_handle_type handle_info;
};
typedef struct hdrrlptxph_newdata_node hdrrlptxph_newdata_type;

struct hdrrlptxph_retx_node
{
  /* This needs to be the first element */
  hdrrlptxph_type  handle_header;
  hdrrlprtx_pkt_handle_type handle_info;
};
typedef struct hdrrlptxph_retx_node hdrrlptxph_retx_type;
/*===========================================================================

FUNCTION HDRRLPTXPH_POWERUP_INIT

DESCRIPTION
  Called to initialize the handle module at powerup  

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void hdrrlptxph_powerup_init(void);

/*===========================================================================

FUNCTION HDRRLPTXPH_GET_HANDLE

DESCRIPTION  Binds a handle to the given RLP
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
hdrrlptxph_type *hdrrlptxph_new_handle
(
   hdrrlptxph_enum_type handle_enum
);

/*===========================================================================

FUNCTION HDRRLPTXPH_FREE_HANDLE

DESCRIPTION Unbinds handle from the RLP.
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void hdrrlptxph_free_handle
(
   hdrrlptxph_type *del_ptr
);

/*===========================================================================

FUNCTION HDRRLPTXPH_GET_HANDLE_TYPE

DESCRIPTION Returns the type of handle
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/

hdrrlptxph_enum_type hdrrlptxph_get_handle_type
(   
   hdrrlptxph_type *handle
);

/*===========================================================================

FUNCTION HDRRLPTXPH_SET_RETX_HANDLE_TYPE

DESCRIPTION Sets the retransmit handle type depending the reason for which 
            this retransmit handle is populated
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void hdrrlptxph_set_retx_handle_type
(
  hdrrlptxph_type *handle,
  hdrrlprtx_reason_enum_type reason
);

/*===========================================================================

FUNCTION HDRRLPTXPH_GET_NUM_HANDLES_USED

DESCRIPTION Returns the number of handles used
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void hdrrlptxph_get_num_handles_used
(
  uint16* new_or_flush_handles_ptr,
  uint16* retx_handles_ptr
);
#endif /* _HDRRLPTXPH_H */
