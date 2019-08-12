#ifndef DLPHCFG_H
#define DLPHCFG_H

/*============================================================================
                           D L _ P H C H _ C F G . H
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2007 by Qualcomm Technologies, Inc.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/dldec.h_v   1.33   11 Jul 2002 22:14:10   gurdeeps  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/dl_phch_cfg.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
04/02/13    pr      Mainlined FEATURE_WCDMA_FDPCH 
02/26/09    rgn     Added align type to phychan db
12/10/08    ks      Mainlining FEATURE_WCDMA_DL_ENHANCED
04/20/07    vb      added rf chan field to add struct
12/12/06    mc      Synced up for MBMS Phase2A integration
12/12/06    mc      Synced up for MBMS demo - 3042 baseline
11/29/06    mc      Initial version for MBMS demo integration.
============================================================================*/

#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "l1dlphychancfg.h"
#include "tlm.h"


typedef uint8 dl_phch_cfg_handle_type;

/*--------------------------------------------------------------------------
                  STRUCTURE: DL_PHYCHAN_ADD_TYPE

This structure contains the parameters for the dl_phychan_add().
--------------------------------------------------------------------------*/
typedef struct {
  /* the physical channel */
  l1_dl_phychan_enum_type phch;

  /* Indicate if physical channel DPCH type is of type F-DPCH */
  boolean dpch_chan_is_fdpch;

  /* the CCTrCH table index chosen by dl_dec for this channel */
  uint8 cctrch_index;

  /* align type for this channel */
  seq_align_type align_type;
 
  /* pointer to the dl phychan ordered config database */
  l1_dl_phychan_db_struct_type *phychan_db;

  /* frequency on which config goes on */
  uint16 rf_chan;

} dl_phychan_add_type;

typedef dl_phychan_add_type dl_phychan_recfg_type;

#endif /* DLPHCFG_H */

