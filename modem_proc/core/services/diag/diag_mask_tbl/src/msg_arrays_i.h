#ifndef MSG_ARRAYS_I_H
#define MSG_ARRAYS_I_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
         
        DIAGNOSTIC MESSAGE SERVICE: ARRAYS FOR BUILD/RUN-TIME MASKS


GENERAL DESCRIPTION
   Internal Header File. Contains structures/definitions for message masks.
  
Copyright (c) 2009-2013 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/diag_mask_tbl/src/msg_arrays_i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/01/13   sa      Support for F3 listeners mask
09/26/12   is      Support for preset masks
11/10/09   sg      Moved MSG_MASK_TBL_CNT to msgcfg.h
09/29/09   mad     Created from msgcfg.h.
===========================================================================*/

#include "comdef.h"
//#include "diagcomm_io.h"  /* For DIAG_MAX_STREAM_ID */

/*---------------------------------------------------------------------------
  This is the structure used for both the build mask array and the RT mask
  array.  It consists of a range of IDs and a pointer to an array indexed
  as follows:
  
  index:
  SSID - ssid_start
  
  Total # of entries:
  ssid_last - ssid_first + 1;
---------------------------------------------------------------------------*/
typedef struct
{
  uint16 ssid_first;      /* Start of range of supported SSIDs */
  uint16 ssid_last;       /* Last SSID in range */

  /* Array of (ssid_last - ssid_first + 1) masks */
  const uint32 *bt_mask_array;
  uint32 *rt_mask_array[2]; //ToDo: Replace with DIAG_MAX_STREAM_ID
  uint32 *rt_base_ptr[2];   //ToDo: Replace with DIAG_MAX_STREAM_ID
}
msg_mask_tbl_type;

typedef struct
{
  uint16 ssid_first;      /* Start of range of supported SSIDs */
  uint16 ssid_last;       /* Last SSID in range */
  /* Array of (ssid_last - ssid_first + 1) masks */
  uint32 *f3_listener_mask_array;
  uint32 *f3_listener_ext_mask_array;
}
listener_msg_mask_tbl_type;
extern listener_msg_mask_tbl_type listener_msg_mask_tbl[];
extern msg_mask_tbl_type msg_mask_tbl[];
/* Number of SSID range entries in table */

#endif /* MSG_ARRAYS_I_H */
