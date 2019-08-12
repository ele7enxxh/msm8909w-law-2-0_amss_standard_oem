#ifndef ONEX_NV_RD_WT_H
#define ONEX_NV_RD_WT_H



/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           N V   R E A D / W R I T E   I N T E R N A L   H E A D E R

GENERAL DESCRIPTION
  This module contains declarations pertaining to the nv read/write.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2013 - 2013 Qualcomm Technologies, Inc. 
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

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header $

when       who     what, where, why
--------   ---    ----------------------------------------------------------
07/01/13   fg     Initial revision.

===========================================================================*/
/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/
/* Common */
#include "comdef.h"
#include "modem_1x_defs.h"
#include "nv.h"
#include "nv_items.h"
#include "rex_types.h"
/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------
      Constants
-------------------------------------------------------------------------*/

/*encoding the options with a single byte*/
#define NV_WRT        128
#define	NV_RD         0
#define NV_MC         1
#define NV_MCCAP      2
#define NV_AUTH       3
#define NV_NONSIG     4
#define NV_SRCH       5

/* more operation types can be defined as the increasing number, up to 127 */

/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/
typedef char NV_OPTION;

/* Structure for holding EFS item file meta data */
typedef struct
{
  char* path;
    /* Complete path of the item file */

  uint32 size;
    /* Size in bytes of data to be stored in item file */

} efs_item_file_type;
/*===========================================================================

                    FUNCTION DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION onex_nv_read_write

DESCRIPTION
  This function read/write NV.

DEPENDENCIES
  The NV task has been started and is running.

RETURN VALUE
  false:    if the read, right fails.
  true:     if the operation succeeded

SIDE EFFECTS
  UNKOWN
===========================================================================*/
boolean onex_nv_read_write
(
  nv_items_enum_type  item_code,                    /* Item to access        */
  nv_item_type        *item_ptr,                    /* where to put the item */
  NV_OPTION           nv_op,                        /* option for cmd buffer */
  rex_sigs_type       (*wait_func)(rex_sigs_type),  /* the wait function     */
  nv_cmd_type         *nv_cmd_buf                   /* command buffer        */
);

/*===========================================================================

FUNCTION onex_nv_efs_init

DESCRIPTION
  This function inits efs (for mc, mux and srch only).

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void onex_nv_efs_init
(
  const char * conf_file,                   /* conf file name                */
  int file_max,                             /* Maximum number of file names  */
  const efs_item_file_type *efs_item_files  /* Array of pathnames of the EFS */
                                            /* item files*/
);

#endif

