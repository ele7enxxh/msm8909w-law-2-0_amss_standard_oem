#ifndef MVSDSM_H
#define MVSDSM_H

/*===========================================================================

                  M U L T I M O D E   V O I C E   S E R V I C E S
                       D S M   P O O L   P R O C E S S I N G

DESCRIPTION
  This header file defines interfaces for processing packets in MVS DSM pools.

REFERENCES
  None.

Copyright (C) 2009 - 2014 QUALCOMM Technologies, Inc.
All rights reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/common/mvs/inc/mvsdsm.h#1 $
$Author: mplcsds1 $
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/26/13   sud     Updated Copyright.
08/18/09    sr     Adapting to CVD.
05/07/08    at     Removed FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING code.
01/28/05   ymc     Removed MSMAUD dependencies.
08/18/04   ymc     Added support for FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING.
04/17/03    sm     Added amrsup support.
            sm     Refeaturized code.
05/30/02   ymc     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"

#include "mvssup.h"
#include "mvsamr.h"
#include "amrsup.h"

#include "dsmutil.h"

/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/

typedef struct {
  dsm_watermark_type wm_a;
  dsm_watermark_type wm_b;
  dsm_watermark_type wm_c;
} mvs_dsm_amr_queue_type;

/* State machine for AMR DSM buffer management */
typedef enum {
  MVS_DSM_AMR_STATE_RESET,
  MVS_DSM_AMR_STATE_INIT,
  MVS_DSM_AMR_STATE_ACTIVE,       /* DSM buffers ready for receiving packets */
  MVS_DSM_AMR_STATE_UL_WAIT,      /* first uplink packet has been generated  */
  MVS_DSM_AMR_STATE_START         /* first uplink packet has been retrieved  */
} mvs_dsm_amr_state_type;

/* Loopback modes for testing */
typedef enum {
  MVS_DSM_LOOP_OFF,
  MVS_DSM_LOOP_UL2DL_VOC,
  MVS_DSM_LOOP_UL2DL_DSM,
  MVS_DSM_LOOP_DL2UL,
  MVS_DSM_LOOP_MAX
} mvs_dsm_loop_type;

/* <EJECT> */
/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION mvs_dsm_amr_init

DESCRIPTION
  Initialize AMR DSM buffers.

DEPENDENCIES
  To be called once only.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_dsm_amr_init(void);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_dsm_amr_activate

DESCRIPTION
  Activate or de-activate AMR DSM buffer management.
  When activated,
  uplink: queuing passed-in or DL2UL loopback AMR packet to uplink DSM buffer.
  downlink: extracting AMR packet from downlink DSM buffer or returning back
            UL2DL loopback AMR packet.

DEPENDENCIES
  mvs_dsm_amr_init has to be called first for initialization.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_dsm_amr_activate(
  boolean active_flag
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_dsm_amr_set_loop_mode

DESCRIPTION
  Setup AMR voice packet loop mode:

  UL2DL_VOC: route uplink vocoder packets into downlink
             (loopback without DSM queues),
             continue queuing uplink DSM packets.

  UL2DL_DSM: route uplink DSM packets into downlink
             (loopback through DSM queues),
             continue queuing uplink DSM packets.

  DL2UL: route downlink vocoder packets into uplink,
         continue decoding downlink vocoder packets.

DEPENDENCIES
  mvs_dsm_amr_init has to be called first for initialization.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_dsm_amr_set_loop_mode(
  mvs_dsm_loop_type loop_mode
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_dsm_amr_ul_proc

DESCRIPTION
  Put AMR packets into uplink DSM queues.  Put packets into loopback buffers
if necessary.

DEPENDENCIES
  mvs_dsm_amr_activate has to be called first for getting into ACTIVE state.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_dsm_amr_ul_proc(
  amrsup_core_speech_type *ul_speech,
  mvs_amr_chan_state_type chan_state  /* current channel A, B, C setup state */
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_dsm_amr_dl_proc

DESCRIPTION
  Extract and decode AMR packets from downlink DSM queues in normal mode.
  Extract and decode AMR packets from UL2DL DSM queues in UL2DL DSM loopback.
  Decode AMR packets from UL2DL VOC buffer in UL2DL VOC loopback.
  Put AMR packets to be decoded into DL2UL buffer in DL2UL loopback.

DEPENDENCIES
  mvs_dsm_amr_activate has to be called first for getting into ACTIVE state.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_dsm_amr_dl_proc(
  amrsup_core_speech_type *dl_speech,
  mvs_amr_frame_type *predicted_frame,
  boolean *is_bad_frame
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_dsm_amr_get_state

DESCRIPTION
  Return the current state of AMR DSM management.

DEPENDENCIES
  None

RETURN VALUE
  mvs_dsm_amr_state

SIDE EFFECTS
  None

===========================================================================*/
extern mvs_dsm_amr_state_type mvs_dsm_amr_get_state(void);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_dsm_amr_get_ul_q

DESCRIPTION
  Return AMR uplink DSM queues.

DEPENDENCIES
  mvs_dsm_amr_init has to be called first.

RETURN VALUE
  &mvs_dsm_amr_ul_q

SIDE EFFECTS
  None

===========================================================================*/
extern mvs_dsm_amr_queue_type* mvs_dsm_amr_get_ul_q(void);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_dsm_amr_get_dl_q

DESCRIPTION
  Return AMR downlink DSM queues.

DEPENDENCIES
  mvs_dsm_amr_init has to be called first.

RETURN VALUE
  &mvs_dsm_amr_dl_q

SIDE EFFECTS
  None

===========================================================================*/
extern mvs_dsm_amr_queue_type* mvs_dsm_amr_get_dl_q(void);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_clear_ul_dsm_amr_queue

DESCRIPTION
  Clears Ulink DSM queue. It prevents increase in RTD, by clearing UL DSM 
  Queue. It should be called before queuing first vocoder packet after silence frames. 

DEPENDENCIES
  mvs_dsm_amr_init has to be called first.

RETURN VALUE
  void

SIDE EFFECTS
  It has to be carefully called, ONLY when silence frame is/are
  present in UL DSM Queue.   
===========================================================================*/

extern void mvs_clear_ul_dsm_amr_queue( void );
#endif  /* MVSDSM_H */

