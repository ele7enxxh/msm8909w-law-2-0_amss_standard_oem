/*===========================================================================

             GM_CONTEXT_BASED_BACKOFF_H Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for
  GM Context  Based Backoff

  Copyright (c) 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/12/15   ss   Initial creation of file.
===========================================================================*/
#ifndef GM_CONTEXT_BASED_BACKOFF_H
#define GM_CONTEXT_BASED_BACKOFF_H

#include "gm_core.h"
#include "gm_ebee.h"

/****************** Macros***************************************/
#define GM_MIN_DISTANCE_FOR_CONTEXT_BASED_BACKOFF   1000 /*1000 meters*/

/*===========================================================================

FUNCTION gm_context_based_backoff_predictor_init

DESCRIPTION
  This function enables the predictor
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
boolean gm_context_based_backoff_predictor_init(void);

#endif /*GM_CONTEXT_BASED_BACKOFF_H*/

