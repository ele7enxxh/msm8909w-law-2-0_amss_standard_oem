/*****************************************************************************
***
*** TITLE
***
***  GPRS PDU UTILITY INTERFACE
***
***
*** DESCRIPTION
***
***  Provides a higher level abstracted interface to the Data Services
***  Memory Pool (DSM) which is tailored to the needs of the GPRS Protocol
***  Stack.
***
***  This abstracted interface serves to not only supplement the
***  functionality of the DSM, but also to wrap the DSM interface and
***  minimise the impact of changes to the DSM interface and operation.
***
*** FUNCTION/MACRO SUMMARY
***
***  gprs_pdu_construct()
***
***    - Creates a new PDU chain and copies octets from a given buffer.
***
***  gprs_pdu_copy()
***
***    - Creates a copy PDU chain of a given PDU chain.
***
***  gprs_pdu_duplicate()
***
***    - Creates a duplicate PDU chain reference to a given PDU chain.
***
***  GPRS_PDU_LENGTH()
***
***    - Returns PDU chain occupancy.
***
***  gprs_pdu_strip_head()
***
***    - Strips octets from the head of a PDU chain to a given buffer.
***
***  gprs_pdu_append_head()
***
***    - Appends octets to the head of a PDU chain from a given buffer.
***
***  gprs_pdu_append_tail()
***
***    - Appends octets to the tail of a PDU chain from a given buffer.
***
***  gprs_pdu_segment_head()
***
***    - Detaches a PDU chain segment from the head of a given PDU chain.
***
***  gprs_pdu_augment_tail()
***
***    - Attaches a given PDU chain segment to the tail of the given
***      target PDU chain.
***
***  GPRS_PDU_FREE()
***
***    - Deallocates the PDU chain resource.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gcommon/inc/gprs_pdu.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/

#ifndef INC_GPRS_PDU_H
#define INC_GPRS_PDU_H


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

#include "dsm.h"
#include "msg.h"


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
===  FUNCTION      GPRS_PDU_CONSTRUCT
===
===  DESCRIPTION
===
===    Allocates sufficient DSM resource to construct a new packet data
===    unit chain of <length> octets, and copies <length> octets from
===    <src_buf> to the new PDU chain.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Pointer handle to the constructed PDU chain.
===
===    If sufficient PDU memory resource is unavailable, a NULL handle is
===    returned.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

dsm_item_type *gprs_pdu_construct
(
  void   *src_buf,
  uint16 length
);

/*===========================================================================
=== Creating seperate pdu_construct functions for each GERAN entity, 
=== in order to help debug leakage of small DSM items CR#879738.
===========================================================================*/

dsm_item_type *gl1_pdu_construct
(
  void   *src_buf,
  uint16 length
);

dsm_item_type *grlc_pdu_construct
(
  void   *src_buf,
  uint16 length
);

dsm_item_type *gllc_pdu_construct
(
  void   *src_buf,
  uint16 length
);

dsm_item_type *gsn_pdu_construct
(
  void   *src_buf,
  uint16 length
);

/*===========================================================================
===
===  FUNCTION      GPRS_PDU_COPY
===
===  DESCRIPTION
===
===    Allocates sufficient new DSM resource to create a copy of the
===    specified PDU as pointed to by <pdu_handle>.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Pointer handle to the copy PDU chain.
===
===    If sufficient PDU memory resource is unavailable, a NULL handle is
===    returned.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

dsm_item_type *gprs_pdu_copy( dsm_item_type *pdu_handle );


/*===========================================================================
===
===  FUNCTION      GPRS_PDU_DUPLICATE
===
===  DESCRIPTION
===
===    Allocates sufficient new DSM header resource to create a duplicate
===    reference to the specified PDU as pointed to by <pdu_handle>, starting
===    from the given <offset> in octets from the start of the given PDU, for
===    the given length.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Pointer handle to the duplicate PDU chain.
===
===    If sufficient PDU memory resource is unavailable, or the offset or
===    length are outside the bounds of the PDU, a NULL handle is
===    returned.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

dsm_item_type *gprs_pdu_duplicate
(
  dsm_item_type *pdu_handle,
  uint16        offset,
  uint16        length
);


/*===========================================================================
===
===  MACRO         GPRS_PDU_LENGTH
===
===  DESCRIPTION
===
===    Returns the occupancy in octets of a PDU chain, as pointed to by
===    <pdu_handle>.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Occupancy of the PDU chain in octets.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

#define GPRS_PDU_LENGTH(pdu_handle)   dsm_length_packet((pdu_handle))


/*===========================================================================
===
===  FUNCTION      GPRS_PDU_STRIP_HEAD
===
===  DESCRIPTION
===
===    Strips <length> octets from the head of the specified PDU, as pointed
===    to by the indirect <pdu_handle> ptr, to the <tgt_buf>.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    TRUE if operation fully succeeded, FALSE otherwise.
===
===  SIDE EFFECTS
===
===    Some DSM item resource may be deallocated depending on the strip
===    length requirement, and <pdu_handle> may be altered.
===
===========================================================================*/

boolean gprs_pdu_strip_head
(
  dsm_item_type **pdu_handle,
  void          *tgt_buf,
  uint16        length
);


/*===========================================================================
===
===  FUNCTION      GPRS_PDU_APPEND_HEAD
===
===  DESCRIPTION
===
===    Copies <length> octets from the <src_buf> and appends them to the
===    head of the PDU chain, as pointed to by the indirect <pdu_handle> ptr.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    TRUE if operation fully succeeded, FALSE otherwise.
===
===  SIDE EFFECTS
===
===    Additional DSM item resource may be allocated depending on the
===    length requirement, and <pdu_handle> may be altered.
===
===========================================================================*/

boolean gprs_pdu_append_head
(
  dsm_item_type **pdu_handle,
  void          *src_buf,
  uint16        length
);


/*===========================================================================
===
===  FUNCTION      GPRS_PDU_APPEND_TAIL
===
===  DESCRIPTION
===
===    Copies <length> octets from the <src_buf> and appends them to the
===    tail of the PDU chain, as pointed to by the indirect <pdu_handle> ptr.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    TRUE if operation fully succeeded, FALSE otherwise.
===
===  SIDE EFFECTS
===
===    Additional DSM item resource may be allocated depending on the
===    length requirement, and <pdu_handle> may be altered.
===
===========================================================================*/

boolean gprs_pdu_append_tail
(
  dsm_item_type **pdu_handle,
  void          *src_buf,
  uint16        length
);


/*===========================================================================
===
===  FUNCTION      GPRS_PDU_SEGMENT_HEAD
===
===  DESCRIPTION
===
===    Segments the PDU chain, as pointed to by the indirect
===    <src_pdu_handle> ptr, at a position given by the <length> from the
===    PDU chain head.
===
===    No data copying is undertaken.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Pointer to the PDU segment.
===
===    If segmentation length requirement exceeds the length of the PDU
===    then the entire PDU chain is returned as the segment.
===
===    Additional DSM item resource may be allocated depending on the
===    granularity, in terms of DSM items, of the segmentation requirements.
===    If no DSM item resource is available then the segment ptr is returned
===    as NULL.
===
===  SIDE EFFECTS
===
===    The <pdu_handle> will be modified to point to the remaining PDU chain.
===
===========================================================================*/

dsm_item_type *gprs_pdu_segment_head
(
  dsm_item_type **src_pdu_handle,
  uint16        length
);


/*===========================================================================
===
===  FUNCTION      GPRS_PDU_AUGMENT_TAIL
===
===  DESCRIPTION
===
===    Augments the target PDU, as pointed to by <tgt_pdu_handle>, by
===    linking the source PDU, as pointed to by the indirect
===    <src_pdu_handle> ptr, onto the tail of the target PDU chain.
===
===    No data copying is undertaken.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    <src_pdu_handle> is modified and set to NULL.
===
===========================================================================*/

void gprs_pdu_augment_tail
(
  dsm_item_type  **tgt_pdu_handle,
  dsm_item_type  **src_pdu_handle
);


/*===========================================================================
===
===  MACRO         GPRS_PDU_FREE
===
===  DESCRIPTION
===
===    Deallocates the DSM item resource contained in the PDU, as pointed
===    to by the indirect <pdu_handle> ptr.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    The <pdu_handle> will be set to NULL.
===
===========================================================================*/

#define GPRS_PDU_FREE(pdu_handle)   dsm_free_packet((pdu_handle))


#endif /* INC_GPRS_PDU_H */

/*** EOF: don't remove! ***/
