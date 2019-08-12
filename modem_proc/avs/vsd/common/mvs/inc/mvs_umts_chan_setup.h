#ifndef __MVS_UMTS_CHAN_SETUP_H__
#define __MVS_UMTS_CHAN_SETUP_H__

/*===========================================================================


DESCRIPTION 
  This header file defines the functions that are used for WCDMA
  and TDSCDMA channel registration and deregistration of uplink
  and downlink channels

REFERENCES
  None.

Copyright (C) 2002-2011, 2013 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/common/mvs/inc/mvs_umts_chan_setup.h#1 $
$Author: mplcsds1 $ 
 
============================================================================*/
 
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/26/13   sud     Updated Copyright.
07/05/11    db     Initial revision
===========================================================================*/
 
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "mvssup.h"

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION mvs_wcdma_set_ul_channel

DESCRIPTION
     Sets up the uplink channel for WCDMA  
DEPENDENCIES
  None

RETURN VALUE
  Fail - APR_EFAILED
  Success - APR_EOK

SIDE EFFECTS
  None

===========================================================================*/
extern int32_t mvs_wcdma_set_ul_channel (
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_umts_chan_class_type chan_class
);

/*===========================================================================

FUNCTION mvs_wcdma_set_dl_channel

DESCRIPTION
     Sets up the downlink channel for WCDMA  

DEPENDENCIES
  None

RETURN VALUE 
  Fail - APR_EFAILED
  Success - APR_EOK 
  
SIDE EFFECTS
  None

===========================================================================*/
extern int32_t mvs_wcdma_set_dl_channel (
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_umts_chan_class_type chan_class,
  mvssup_umts_blk_size_cb_set_type *blk_size_cb_set
);

/*===========================================================================

FUNCTION mvs_tdscdma_set_ul_channel

DESCRIPTION
     Sets up the uplink channel for TDSCDMA 

DEPENDENCIES
  None

RETURN VALUE
  Fail - APR_EFAILED
  Success - APR_EOK 
 
SIDE EFFECTS
  None

===========================================================================*/
extern int32_t mvs_tdscdma_set_ul_channel (
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_umts_chan_class_type chan_class
);

/*===========================================================================

FUNCTION mvs_tdscdma_set_dl_channel

DESCRIPTION
     Sets up the downlink channel TDSCDMA 

DEPENDENCIES
  None

RETURN VALUE
  Fail - APR_EFAILED
  Success - APR_EOK
 
SIDE EFFECTS
  None

===========================================================================*/
extern int32_t mvs_tdscdma_set_dl_channel (
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_umts_chan_class_type chan_class,
  mvssup_umts_blk_size_cb_set_type *blk_size_cb_set
);

#endif  /* */

