#ifndef AUTHI_H
#define AUTHI_H
/*===========================================================================

               I N T E R N A L   A U T H E N T I C A T I O N
                      T A S K   D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the Authentication Task.

  Copyright (c) 1995 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/authi.h_v   1.0.2.0   30 Nov 2001 16:46:28   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/authi.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/26/09   adw     Removed unnecessary ulpn.h include.
06/29/95   dna     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "queue.h"
#include "qw.h"
#include "cmd.h"
#include "cai.h"
#include "caii.h"
#include "nv.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* Status of auth initialization */
typedef enum {
  AUTH_UNINITIALIZED,               /* Init has not completed successfully */
  AUTH_INIT_OK,                     /* Normal initialization has been done */
  AUTH_BAD_SSD                      /* No SSD has been generated */
} auth_init_status_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION AUTH_GET_NV_ITEM

DESCRIPTION
  This function gets a specific item from NV.

DEPENDENCIES
  None.

RETURN VALUE
  Status returned from the NV read request.  Status can be either:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active
  Any other status causes ERR_FATAL and the function does not return.

SIDE EFFECTS
  While this function is running all other task activities are
  suspended until the NV item is read in.  If a read status other
  than the ones listed above is returned, ERR_FATAL is called.

===========================================================================*/

extern nv_stat_enum_type auth_get_nv_item
(
  nv_items_enum_type  item_code,       /* Item to get */
  nv_item_type        *item_ptr        /* Pointer where to put the item */
);

/*===========================================================================

FUNCTION AUTH_PUT_NV_ITEM

DESCRIPTION
  This function puts a specific item to NV.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  While this function is running all other task activities are
  suspended until the NV item is written.

===========================================================================*/

extern void auth_put_nv_item
(
  nv_items_enum_type  item_code,       /* Item to put */
  nv_item_type        *item_ptr        /* Pointer where to get the item */
);

#endif /* AUTHI_H */
