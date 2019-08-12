/*===========================================================================

                        H D R R L P _ U T I L 
GENERAL DESCRIPTION
  This file contains the definition for hdrrlp utility functions.

EXTERNALIZED FUNCTIONS
 hdrrlp_util_is_task_hdrrx()
   Returns true if current task ptr is HDRRLP_TASK_PTR.
   
INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

 Copyright (c) 2008-2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/src/hdrrlp_util.c#1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/21/12   msh     TCB changes due to Core changes  
04/30/08   sjun    Initial file creation.
===========================================================================*/

#include "datamodem_variation.h"
#include "hdrrlp_util.h"
#include "data_msg.h"

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  #include "rcinit.h"
#endif /*FEATURE_MODEM_RCINIT_PHASE2*/

/*These variables are inited during hdr rlp init*/
static rex_tcb_type* hdrrlp_util_hdrrx_tcb; 
static rex_tcb_type* hdrrlp_util_hdrtx_tcb;

/*===========================================================================

FUNCTION HDRRLP_UTIL_INIT

DESCRIPTION
  This function gets the hdrrx and hdrtx tcb pointers and stores them locally

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

void hdrrlp_util_init 
(
  void
)
{
#ifdef FEATURE_MODEM_RCINIT_PHASE2
#ifndef TEST_FRAMEWORK
  hdrrlp_util_hdrrx_tcb = rcinit_lookup_rextask("hdrrx"); 
  
  if ( RCINIT_NULL == hdrrlp_util_hdrrx_tcb)
  {
    
    ASSERT(0);
  }

  hdrrlp_util_hdrtx_tcb = rcinit_lookup_rextask("hdrtx");
  if ( RCINIT_NULL == hdrrlp_util_hdrrx_tcb)
  {
    
    ASSERT(0);
  }
#endif
#else /*FEATURE_MODEM_RCINIT_PHASE2*/
  hdrrlp_util_hdrrx_tcb = &hdrrx_tcb;
  hdrrlp_util_hdrtx_tcb = &hdrtx_tcb;
#endif /*FEATURE_MODEM_RCINIT_PHASE2*/
    
} /*hdrrlp_util_init*/

/*===========================================================================

FUNCTION HDRRLP_UTIL_GET_HDRRX_TCB

DESCRIPTION
  This function returns the hdrrx_tcb pointer

DEPENDENCIES
  None

RETURN VALUE
  hdrrlp_util_hdrrx_tcb

SIDE EFFECTS
  None
===========================================================================*/

rex_tcb_type* hdrrlp_util_get_hdrrx_tcb 
(
  void
)
{
  return hdrrlp_util_hdrrx_tcb; 
}

/*===========================================================================

FUNCTION HDRRLP_UTIL_GET_HDRTX_TCB

DESCRIPTION
  This function returns the hdrtx_tcb pointer

DEPENDENCIES
  None

RETURN VALUE
  hdrrlp_util_hdrtx_tcb

SIDE EFFECTS
  None
===========================================================================*/

rex_tcb_type* hdrrlp_util_get_hdrtx_tcb 
(
  void
)
{
  return hdrrlp_util_hdrtx_tcb; 
}

/*===========================================================================

FUNCTION HDRRLP_UTIL_IS_TASK_HDRRX

DESCRIPTION
  This function determines whether the current task is HDRRX

DEPENDENCIES
  None

RETURN VALUE
  A boolean value representing whether the current task pointer is HDRRX.

SIDE EFFECTS
  None
===========================================================================*/

boolean hdrrlp_util_is_task_hdrrx()
{
	return (rex_self() == hdrrlp_util_hdrrx_tcb);
}

/*===========================================================================

FUNCTION HDRRLP_UTIL_IS_TASK_HDRTX

DESCRIPTION
  This function determines whether the current task is HDRTX

DEPENDENCIES
  None

RETURN VALUE
  A boolean value representing whether the current task pointer is HDRTX.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrrlp_util_is_task_hdrtx()
{
	return (rex_self() == hdrrlp_util_hdrtx_tcb);
}

/*===========================================================================
FUNCTION HDRUTILBIT_PACK8

DESCRIPTION
  This function calls the dsmbit_pack8 function, and cuases an ERR_FATAL
  if the total number of bits is not packed.

DEPENDENCIES
  None.
  
PARAMETERS
  item_ptr  - Pointer to dsm item where packed data should be placed.
  pack_data - Data to be packed
  offset    - Number of bits to skip before placing this data
  len       - Number of bits of pack_data to pack (LSB of the variable)

RETURN VALUE
  None.

SIDE_EFFECTS
  ERR_FATAL if the packing is unsuccessful

===========================================================================*/
void hdrutilbit_pack8
(
  dsm_item_type * item_ptr,
  uint8 pack_data,
  uint16 offset,
  uint16 len
)
{
  if (dsmbit_pack8_tail(item_ptr, pack_data, offset, len) != len)
  {
    ERR_FATAL("Message Pack Failure",0,0,0);
  }
} /* hdrutilbit_pack8() */   

/*===========================================================================
FUNCTION HDRUTILBIT_PACK32

DESCRIPTION
  This function calls the dsmbit_pack32 function, and causes an ERR_FATAL
  if the total number of bits is not packed.

DEPENDENCIES
  None.
  
PARAMETERS
  item_ptr  - Pointer to dsm item where packed data should be placed.
  pack_data - Data to be packed
  offset    - Number of bits to skip before placing this data
  len       - Number of bits of pack_data to pack (LSB of the variable)

RETURN VALUE
  None.

SIDE_EFFECTS
  ERR_FATAL if the packing is unsuccessful

===========================================================================*/
void hdrutilbit_pack32
(
  dsm_item_type * item_ptr,
  uint32 pack_data,
  uint16 offset,
  uint16 len
)
{
  if (dsmbit_pack32_tail(item_ptr, pack_data, offset, len) != len)
  {
    ERR_FATAL("Message Pack Failure",0,0,0);
  }
} /* hdrutilbit_pack32() */
