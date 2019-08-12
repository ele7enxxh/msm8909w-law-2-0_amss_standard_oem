#ifndef HDRRLPMLNAK_H
#define HDRRLPMLNAK_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      R A D I O   L I N K   P R O T O C O L  N A K  P R O C E S S I N G

GENERAL DESCRIPTION
  This file contains HDR multilink nak-list processing functions.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  RLP configuration and initialization procedures.

 Copyright (c) 2007 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrrlpmlnak.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
06/19/07   da         Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"

#ifdef FEATURE_HDR_REVB

#include "dsrlpi.h"
#include "hdrrlpi.h"




/*===========================================================================

             PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
typedef enum
{
  HDRRLPMLNAK_TYPE_QUICK = 0,   /* MultiLink Nak from QuickNak indication  */
  HDRRLPMLNAK_TYPE_DELAYED      /* Delayed MultiLink Nak                   */

} hdrrlpmlnak_enum_type;


/*---------------------------------------------------------------------------
  Type for NAK list entry.  
---------------------------------------------------------------------------*/
typedef struct
{
  q_link_type    link;       /* Queue link used to queue/dequeue the entry */
  hdrrlpmlnak_enum_type nak_type;   /* Delayed or Quick multilink nak      */
  uint32                leading_edge;  /* Leading edge of multilink nak    */
  uint32                trailing_edge; /* Trailing edge of multilink nak   */  
  boolean               leading_edge_included; /* Is leading edge included */

} hdrrlpmlnak_list_entry_type;

/*---------------------------------------------------------------------------
  Type for Quick Repeat NAK list entry.  
---------------------------------------------------------------------------*/
typedef struct
{
  q_link_type               link;
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr;    /* RLP on which the NAK msg went    */
  uint8                     hmp_hdr;     /* HMP header used for original nak */
  uint8                     nak_count;
  uint32                    leading_edges[HDRRLPI_MAX_MULTILINK_NAK_RECORDS];
  uint32                    trailing_edges[HDRRLPI_MAX_MULTILINK_NAK_RECORDS];

} hdrrlpmlnak_quick_repeat_list_entry_type;

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                      FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*===========================================================================

FUNCTION       HDRRLPMLNAK_INIT

DESCRIPTION    Called at initialization, this takes all nak entries from
               the array and puts them onto the free queue.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void hdrrlpmlnak_init
(
  void
);

/*===========================================================================

FUNCTION       HDRRLPMLNAK_RESET

DESCRIPTION    Called when an RP instance is reset.  This takes any nak list
               entries from that RLP instance and puts them onto the 
               global free queue.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void hdrrlpmlnak_reset
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr         /* current RLP ctl blk        */
);

/*===========================================================================

FUNCTION       HDRRLPMLNAK_INSERT

DESCRIPTION    Insert this new entry into the nak list.  Inserted according
               to sequence number.  Nak entry is filled out with first nak
               round information.
               
               If entry already exists, then nothing is done.
               
DEPENDENCIES   Input rrcb_ptr is non-null.

RETURN VALUE   TRUE if the information could be inserted, else FALSE.

SIDE EFFECTS   NONE
===========================================================================*/
boolean hdrrlpmlnak_insert
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,          /* curr RLP ctl blk         */
  hdrrlpmlnak_enum_type    nak_type,           /* type of multilink nak    */
  uint32                   leading_edge,       /* leading edge seq num     */
  uint32                   trailing_edge,       /* trailing edge seq num   */
  boolean                  leading_edge_included /* is leading edge included */
);

/*===========================================================================

FUNCTION       HDRRLPMLNAK_FREE_NAK_ENTRY

DESCRIPTION    Returns the nak_entry to the hdrrlpmlnak_free_q
               
DEPENDENCIES   

RETURN VALUE   

SIDE EFFECTS   NONE
===========================================================================*/
void hdrrlpmlnak_free_nak_entry
(
  hdrrlpmlnak_list_entry_type *nak_entry_ptr  /* nak list entry to be freed */
);

/*===========================================================================

FUNCTION       HDRRLPMLNAK_GET_QUICK_REPEAT_ITEM

DESCRIPTION    Gets a free item from the hdrrlpmlnak_quick_repeat_free_q
               and returns it after setting nak_count to 0.
               
DEPENDENCIES   NONE
               
RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
hdrrlpmlnak_quick_repeat_list_entry_type* hdrrlpmlnak_get_free_quick_repeat_item
( 
  void 
);

/*===========================================================================

FUNCTION       HDRRLPMLNAK_FREE_QUICK_REPEAT_ITEM

DESCRIPTION    Puts the item back into the hdrrlpmlnak_quick_repeat_free_q
               
DEPENDENCIES   NONE
               
RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void hdrrlpmlnak_free_quick_repeat_item
(
  hdrrlpmlnak_quick_repeat_list_entry_type* item_ptr 
);

#endif /* FEATURE_HDR_REVB */

#endif /* HDRRLPMLNAK_H */

