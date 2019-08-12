#ifndef _HSU_AL_SER_ACM_H_
#define _HSU_AL_SER_ACM_H_

/*=============================================================================

Serial Functions over ACM Adaptation Layer for High Speed USB
H E A D E R   F I L E
=============================================================================*/
/**
@file hsu_al_ser_acm.h

Contains the API for the serial functions over the ACM adaptation layer.

Initialization and sequencing requirements:\n
hsu_al_ser_base_init must be called for the function instance before 
performing any other operations on the instance.
*/
/*=============================================================================
Copyright (c) 2006 - 2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_al_ser_acm.h#1 $
$DateTime: 2016/06/17 14:31:11 $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
06/24/10  lo  (Tech Pubs) Edited/added Doxygen comments and markup.
07/30/06 esh  First revision of file    
==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include <sio.h>

#include "uw_device_cdc.h"
#include "uw_device_cdc_acm.h"
#include "jtypes.h"
#include "jusb.h"

#include "dsm.h"

#include "rex.h" /*  For rex_timer_type and rex_timer_cnt_type */

/** @ingroup hsusb_qc_api
    @{
*/

/*=============================================================================
Constants and Macros
=============================================================================*/

/** DSM pool from which to allocate Rx buffers.
*/
#define ACM_RX_DSM_ITEM_POOL DSM_DS_LARGE_ITEM_POOL

/** Receive timeout in ms.
*/
#define   ACM_IOCTL_SET_RX_TIMEOUT (10000)

/*=============================================================================
Typedefs
=============================================================================*/

/* modem's stream mode specific data: */
/** Stream-mode-specific data when modem is in Raw Data mode. For Raw Data, the
field stream_mode_specific_data_ptr shall assume the type of this struct.
*/
typedef struct
{
  rex_timer_type          rawdata_mode_esc_seq_timer_cb_item;
    /**< REX timer to be used in Raw Data mode. */
  sio_vv_func_ptr_type    rawdata_mode_esc_sequence_cb_func;
    /**< Callback to be called on timer expiration. */
  uint8                   num_of_esc_seq_characters_seen;
    /**< Number of escape characters seen. */
  boolean                 should_wait_for_clk;
    /**< Receive data while waiting for inactivity. */
} rawdata_mode_data_struct;
  
/*=============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/

/** @} */ /* endgroup hsusb_qc_api */

#endif /* _HSU_AL_SER_ACM_H_ */
