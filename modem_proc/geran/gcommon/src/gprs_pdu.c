/*****************************************************************************
***
*** TITLE
***
***  GPRS PDU UTILITY FUNCTIONS
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
*** EXTERNALIZED FUNCTIONS
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
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  DSM Memory Pool must be initialised before use. This is usually within
***  the scope of the TMC Task Mode Controller Task, and outside the scope
***  of this module.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gcommon/src/gprs_pdu.c#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** mm/dd/yy   xyz	   changes
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "comdef.h"
#include "dsm.h"
#include "msg.h"
#include "gprs_pdu.h"
#include "rex.h"
#include <stringl.h>

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Macros
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Functions
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
===    For a complete description refer to interface gprs_pdu.h.
===
===========================================================================*/

dsm_item_type *gprs_pdu_construct
(
  void   *src_buf,
  uint16 length
)
{
  dsm_item_type *pdu_handle = NULL;

  if
  (
    dsm_pushdown_tail
    (
      &pdu_handle, src_buf, length, DSM_DS_SMALL_ITEM_POOL
    )
    !=
    length
  )
  {
    /* Insufficient resource so free up that which has been allocated. */
    dsm_free_packet( &pdu_handle );
  }

  return ( pdu_handle );
}

/*===========================================================================
=== Creating seperate pdu_construct functions for each GERAN entity, 
=== in order to help debug leakage of small DSM items CR#879738.
===========================================================================*/

dsm_item_type *gl1_pdu_construct
(
  void   *src_buf,
  uint16 length
)
{
  dsm_item_type *pdu_handle = NULL;

  if
  (
    dsm_pushdown_tail
    (
      &pdu_handle, src_buf, length, DSM_DS_SMALL_ITEM_POOL
    )
    !=
    length
  )
  {
    /* Insufficient resource so free up that which has been allocated. */
    dsm_free_packet( &pdu_handle );
  }

  return ( pdu_handle );
}

dsm_item_type *grlc_pdu_construct
(
  void   *src_buf,
  uint16 length
)
{
  dsm_item_type *pdu_handle = NULL;

  if
  (
    dsm_pushdown_tail
    (
      &pdu_handle, src_buf, length, DSM_DS_SMALL_ITEM_POOL
    )
    !=
    length
  )
  {
    /* Insufficient resource so free up that which has been allocated. */
    dsm_free_packet( &pdu_handle );
  }

  return ( pdu_handle );
}

dsm_item_type *gllc_pdu_construct
(
  void   *src_buf,
  uint16 length
)
{
  dsm_item_type *pdu_handle = NULL;

  if
  (
    dsm_pushdown_tail
    (
      &pdu_handle, src_buf, length, DSM_DS_SMALL_ITEM_POOL
    )
    !=
    length
  )
  {
    /* Insufficient resource so free up that which has been allocated. */
    dsm_free_packet( &pdu_handle );
  }

  return ( pdu_handle );
}

dsm_item_type *gsn_pdu_construct
(
  void   *src_buf,
  uint16 length
)
{
  dsm_item_type *pdu_handle = NULL;

  if
  (
    dsm_pushdown_tail
    (
      &pdu_handle, src_buf, length, DSM_DS_SMALL_ITEM_POOL
    )
    !=
    length
  )
  {
    /* Insufficient resource so free up that which has been allocated. */
    dsm_free_packet( &pdu_handle );
  }

  return ( pdu_handle );
}


/*===========================================================================
===
===  FUNCTION      GPRS_PDU_COPY
===
===    For a complete description refer to interface gprs_pdu.h.
===
===========================================================================*/

dsm_item_type *gprs_pdu_copy( dsm_item_type *pdu_handle )
{
            dsm_item_type   *copy_handle = NULL;
  register  dsm_item_type   *item_p      = NULL;
  register  dsm_item_type   *prev_p      = NULL;

  register  uint32 size;
  register  uint8  *data_p;

  /* --------------------------------------------------------------------- */

  while ( pdu_handle != NULL )
  {
    data_p = pdu_handle -> data_ptr;
    size   = pdu_handle -> used;

    item_p = dsm_new_buffer( DSM_DS_POOL_SIZE(size) );

    if ( (item_p != NULL) && (item_p -> size >= size) )
    {
      if ( prev_p == NULL )
      {
        copy_handle       = item_p;
        prev_p            = item_p;
      }
      else
      {
        prev_p -> pkt_ptr = item_p;
        prev_p            = item_p;
      }

      // Buffer overflow cannot occur as we have checked out above how much we're going
      // to write but use memscpy anyway so all use of memcpy is expunged
      (void)memscpy( item_p -> data_ptr, size, data_p, size );
      item_p -> used = (uint16)size;
    }
    else
    {
      /* Insufficient DSM resource to complete the copy. Free and exit. */

      dsm_free_packet( &copy_handle );
      (void)dsm_free_buffer( item_p );
      return( copy_handle );
    }

    pdu_handle = pdu_handle -> pkt_ptr;
  }

  return ( copy_handle );

  /* --------------------------------------------------------------------- */
} /* end of gprs_pdu_copy() */


/*===========================================================================
===
===  FUNCTION      GPRS_PDU_DUPLICATE
===
===    For a complete description refer to interface gprs_pdu.h.
===
===========================================================================*/

dsm_item_type *gprs_pdu_duplicate
(
  dsm_item_type *pdu_handle,
  uint16        offset,
  uint16        length
)
{
  dsm_item_type *dup_pdu_handle = NULL;

  if ( dsm_dup_packet( &dup_pdu_handle, pdu_handle, offset, length) !=
       length                                                          )
  {
    /* Insufficient resource so free up that which has been allocated. */

    dsm_free_packet( &dup_pdu_handle );
  }
  return ( dup_pdu_handle );

} /* end of gprs_pdu_duplicate() */


/*===========================================================================
===
===  FUNCTION      GPRS_PDU_STRIP_HEAD
===
===    For a complete description refer to interface gprs_pdu.h.
===
===========================================================================*/

boolean gprs_pdu_strip_head
(
  dsm_item_type **pdu_handle,
  void          *tgt_buf,
  uint16        length
)
{
  return ( dsm_pullup( pdu_handle, tgt_buf, length ) == length );
}


/*===========================================================================
===
===  FUNCTION      GPRS_PDU_APPEND_HEAD
===
===    For a complete description refer to interface gprs_pdu.h.
===
===========================================================================*/

boolean gprs_pdu_append_head
(
  dsm_item_type **pdu_handle,
  void          *src_buf,
  uint16        length
)
{
  return
  (
    (boolean)
    (
      dsm_pushdown(pdu_handle, src_buf, length, DSM_DS_POOL_SIZE(length)) ==
      length
    )
  );
}


/*===========================================================================
===
===  FUNCTION      GPRS_PDU_APPEND_TAIL
===
===    For a complete description refer to interface gprs_pdu.h.
===
===========================================================================*/

boolean gprs_pdu_append_tail
(
  dsm_item_type **pdu_handle,
  void          *src_buf,
  uint16        length
)
{
  return
  (
    (boolean)
    (
      dsm_pushdown_tail(pdu_handle, src_buf, length, DSM_DS_POOL_SIZE(length)) ==
      length
    )
  );
}


/*===========================================================================
===
===  FUNCTION      GPRS_PDU_SEGMENT_HEAD
===
===    For a complete description refer to interface gprs_pdu.h.
===
===========================================================================*/

dsm_item_type *gprs_pdu_segment_head
(
  dsm_item_type **src_pdu_handle,
  uint16        length
)
{
            dsm_item_type *seg_handle  = *src_pdu_handle;
  register  dsm_item_type *pdu_handle  = *src_pdu_handle;
            dsm_item_type *tmp_handle;
  register  uint16        remaining    = length;

  /* --------------------------------------------------------------------- */

  /* Check for the 'unusual' case of a requested segmentation length of 0. */

  if ( length == 0 )
  {
    seg_handle = NULL;
    pdu_handle = NULL;
  }

  /* --------------------------------------------------------------------- */

  while ( pdu_handle != NULL )
  {
    /* Search for the point of segmentation. */

    if ( remaining > pdu_handle -> used )
    {
      remaining  -= pdu_handle -> used;
      pdu_handle  = pdu_handle -> pkt_ptr;

      /* If the chain is too short to establish the segmentation point,
         then return the whole chain as the segment. */

      if ( pdu_handle == NULL)
        *src_pdu_handle = NULL;
    }

    /* ................................................................... */

    else if ( remaining == pdu_handle -> used )
    {
      /* Segmentation point established as being at a DSM item boundary,
         so split the PDU chain without any copying of octets. */

      *src_pdu_handle       = pdu_handle -> pkt_ptr;
      pdu_handle -> pkt_ptr = NULL;
      pdu_handle            = NULL;
    }

    /* ................................................................... */

    else
    {
      /* Segmentation point established as being within a DSM item, so
         split the DSM item chain. */

      *src_pdu_handle = NULL;

      if
      (
        dsm_dup_packet
        (
          src_pdu_handle,
          pdu_handle,
          remaining,
          (uint16)(pdu_handle -> used - remaining)
        )
        !=
        (uint16)(pdu_handle -> used - remaining)
      )
      {
        /* Insufficient resource so restore original PDU. */

        *src_pdu_handle = seg_handle;
        seg_handle      = NULL;
        break;
      }

      if (*src_pdu_handle)
      {
        (*src_pdu_handle) -> pkt_ptr = pdu_handle -> pkt_ptr;
        pdu_handle        -> pkt_ptr = NULL;

        tmp_handle = pdu_handle; /* Make a non-register based copy pointer */
        dsm_trim_packet( &tmp_handle, remaining );
        pdu_handle = NULL;
      }
    }
  }

  return ( seg_handle );

  /* --------------------------------------------------------------------- */
} /* end of gprs_pdu_segment_head() */


/*===========================================================================
===
===  FUNCTION      GPRS_PDU_AUGMENT_TAIL
===
===    For a complete description refer to interface gprs_pdu.h.
===
===========================================================================*/

void gprs_pdu_augment_tail
(
  dsm_item_type  **tgt_pdu_handle,
  dsm_item_type  **src_pdu_handle
)
{
  register dsm_item_type *pdu_handle = *tgt_pdu_handle;

  /* --------------------------------------------------------------------- */

  if ( pdu_handle == NULL )
  {
    *tgt_pdu_handle = *src_pdu_handle;
    *src_pdu_handle = NULL;
  }
  else
  {
    /* Find the end of the target PDU chain. */

    while ( pdu_handle -> pkt_ptr != NULL )
    {
      pdu_handle = pdu_handle -> pkt_ptr;
    }

    pdu_handle -> pkt_ptr = *src_pdu_handle;
    *src_pdu_handle       = NULL;
  }

  /* --------------------------------------------------------------------- */
} /* end of gprs_pdu_augment_tail() */



/*** EOF: don't remove! ***/

