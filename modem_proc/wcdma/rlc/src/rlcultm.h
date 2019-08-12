#ifndef RLCULTM_H
#define RLCULTM_H
/*===========================================================================
                  U P L I N K   T R A N S P A R E N T   M O D E   

            R A D I O   L I N K   C O N T R O L  : H E A D E R   F I L E 

DESCRIPTION
  This file contains data types and function prototypes associated with the
  Uplink Transparent Mode RLC module.
  
Copyright (c) 2001 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

Copyright (c) 2008-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/rlc/vcs/rlcultm.h_v   1.3   28 Jun 2001 11:15:28   rchar  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rlc/src/rlcultm.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/08/15   kc      Purge old/partial CCCH data only as part of PDU building.
01/21/11  vp/kvk   Added support for AMR WB packet simulation
12/07/10   vp      Added support for amr packet simulation in uplink
05/11/09   ssg     Updated Copyright Information
01/08/01   rc      Added description to the prototype.
11/22/00   rc      Created file.

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/

#include "wcdma_variation.h"
#include "rlcul.h"
#include "rlcrrcif_v.h"
#include "rlcdsapif.h"
#include "macrlcif.h"

#ifdef FEATURE_WCDMA_SIMULATE_AMR_PACKETS

#define WL2_MVS_CONVERT_BIT_TO_BYTES(no_bits)  ((no_bits + 4)/8)

#define MAX_MVS_FRAME_LEN_IN_BYTES (16)
#define MAX_MVS_PATTERN (11)

#define RLC_SID_LEN_IN_BYTES 5
#define RLC_SID_LEN_IN_BITS_NB 39
#define RLC_SID_LEN_IN_BITS_WB 40

#define RLC_SID_FIRST_FRAME_INDEX (0)
#define RLC_SID_UPD_FRAME_INDEX (3)

#define RLC_NO_DATA_LEN_IN_BYTES 1

#define MAX_MVS_CHANNELS (3)

typedef struct {
uint8 *mvs_buff;

uint8 len_in_bits;
}wl2_mvs_frame_struct_type;

typedef struct {
boolean always_no_data;

rlc_lc_id_type lch_id;
} wl2_mvs_ch_struct_type;

/* SID buffer to be filled by RLC */
extern uint8 rlc_sid_first_buff[RLC_SID_LEN_IN_BYTES];
extern uint8 rlc_sid_upd_buff[RLC_SID_LEN_IN_BYTES];
extern uint8 rlc_no_data_buff[RLC_NO_DATA_LEN_IN_BYTES];
extern uint32 wl2_mvs_frame_index;
extern mvssup_wcdma_chan_state_type mvs_ul_wcdma_chan_state;
extern wl2_mvs_frame_struct_type wl2_mvs_frame[MAX_MVS_PATTERN];

extern wl2_mvs_ch_struct_type wl2_mvs_ch[MAX_MVS_CHANNELS];

extern boolean rlc_mvs_acquired;

extern int32 sig_run_count;

#endif /*FEATURE_WCDMA_SIMULATE_AMR_PACKETS*/

/*===========================================================================

FUNCTION RLCI_GET_TM_BUFFER_STATUS

DESCRIPTION
  Calculates the number of SDUs and size of the SDU. MAC uses this 
  information to determine the number of PDUs to give to the channel.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_get_tm_buffer_status
(
  rlci_ul_ctrl_blk_type     *ctrl_blk,  /* Pointer to Uplink Control Block */
  rlc_ul_lc_buf_status_type *status_ptr, /* Pointer for buffer status       */
  rlc_ul_bo_query_source  query_from_bfi_flag
);



/*===========================================================================

FUNCTION rlci_ul_build_tm_pdus

DESCRIPTION
  Builds Uplink Transparent Mode PDUs.
  Gets the items from the Queue and sends it down to MAC.
  When segmentation is not performed, the SDU size should be equal to the 
  PDU size.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_ul_build_tm_pdus
(       
  rlci_ul_ctrl_blk_type      *ctrl_blk, /* Pointer to UL TM control Block  */
  rlc_ul_logchan_format_type *chan_fmt, /* PDU information                 */
  rlc_ul_logchan_data_type   *data_ptr  /* Pointer for storing PDUs        */
);


#ifdef FEATURE_WCDMA_HS_RACH

/*===========================================================================

FUNCTION RLCI_UL_UPDATE_FREE_TM_SDU_Q

DESCRIPTION
  API used to store the UL TM DSM item that needs to be freed in the next TTI.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlci_ul_update_free_tm_sdu_q(dsm_item_type *tm_dsm_item);

/*===========================================================================

FUNCTION RLCI_ENH_UL_BUILD_TM_PDUS

DESCRIPTION
 API to build Uplink Transparent Mode PDUs in case of HS-RACH.
  Gets the items from the Queue and sends it down to MAC.
  When segmentation is not performed, the SDU size should be equal to the 
  PDU size.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_enh_ul_build_tm_pdus (rlci_ul_ctrl_blk_type      *ctrl_blk, /* Pointer to UL TM control Block */
                                    rlc_ul_logchan_format_type *chan_fmt, /* PDU information                */
                                    l1_l2_data_type *l1_ul_tx_buf  /* Pointer for storing PDUs       */);
#endif /* FEATURE_WCDMA_HS_RACH */

#endif
