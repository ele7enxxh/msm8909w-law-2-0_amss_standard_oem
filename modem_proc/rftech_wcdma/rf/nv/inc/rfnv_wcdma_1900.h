#ifndef RFNV_WCDMA_1900_H
#define RFNV_WCDMA_1900_H

/*!
  @file
  rfnv_wcdma_1900.h

  @brief

  @details
  This header file contains definitions  to interface with RF NV for WCDMA.

*/

/*===========================================================================

Copyright (c) 2008-2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/nv/inc/rfnv_wcdma_1900.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/10/14   rmb     Removed apt_commit API which is unused. 
07/08/13   kcj     Can't use designated initialization with OFT's compiler. 
06/05/13   kcj     Dynamically allocated NV struct memory. 
02/24/11   ad      Connected SMPS NV to RFC structure for WCDMA
09/30/10   ac      efs support
09/15/10   ac      add support for the nvs for EFS
08/13/09   ckl     Merge diversity NV items into main NV item list.
03/15/09   ckl     Deleted rfnv.h
03/09/09   ckl     Code cleanup
08/21/03   ap      Final modifications to support WCDMA 1900
06/17/03   eh      Initial Revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "rfnv_wcdma_types.h"

/*===========================================================================

                       PUBLIC DATA DECLARATIONS

===========================================================================*/

extern rfnv_wcdma_nv_items_list rfnv_wcdma_1900_nv_items_list;

/*===========================================================================

                         FUNCTION PROTOTYPES

===========================================================================*/

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_1900_init_nv_item_list(void);

#endif /* RFNV_WCDMA_1900_H */
