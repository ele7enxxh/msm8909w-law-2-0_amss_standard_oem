#ifndef GL1_MSG_H
#define GL1_MSG_H
/*========================================================================
                      GSM GPRS NPL1 MSG LAYER HEADER FILE
DESCRIPTION
   This file defines the interfaces to the GSM/GPRS NPL1 MSG LAYER. The GSM/GPRS
   NPL1 message layer abstracts all Non-Portable Layer 1 concepts. The message
   layer mainly deals processing that spans over multi-frames or blocks.

Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
========================================================================== */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/gl1_msg.h#1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
18-11-13   pjr      CR563429 Fixed vocoder handover on sub1 while sub 2 was in a call
21-10-13   pjr      CR564298 added function gl1_msg_is_l1_vocoder_control_enabled_multisim
11-03-01   tjw      API content split out from the original GERAN internal file
                    previously used by GERAN clients. 
========================================================================== */

#ifndef CUSTOMER_H
  #include "customer.h"
#endif

#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "mvs.h"

#include "gl1_defs.h"
#include "geran_dual_sim.h"

#define MAX_TCH_OCTETS_MVS 38   /* for the array size, the biggest value it can
                                   be though whole buffer may not be used */

/* Structures for support of MVS audio */
typedef struct {
  uint8 packet[MAX_TCH_OCTETS_MVS];
  mvs_frame_info_type frame_info;
  int32 packet_len;
  int32 packet_semaphore;
} gsm_voc_packet_type;

/*=============================================================================
FUNCTION gl1_msg_is_l1_vocoder_control_enabled

DESCRIPTION
   This function determines if L1 is in control of the vcoder

DEPENDENCIES

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
extern boolean gl1_msg_is_l1_vocoder_control_enabled(void);

/*=============================================================================
FUNCTION gl1_msg_is_l1_vocoder_control_enabled_multisim

DESCRIPTION
   This function determines if L1 is in control of the vcoder

DEPENDENCIES

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
extern boolean gl1_msg_is_l1_vocoder_control_enabled_multisim(sys_modem_as_id_e_type as_id);

/*=============================================================================
FUNCTION gl1_msg_vocoder_acquired

DESCRIPTION
   This function is used by NAS to indicate the vocoder has been
   acquired but not enabled yet

=============================================================================*/
extern void gl1_msg_vocoder_acquired( void );

/*=============================================================================
FUNCTION gl1_msg_vocoder_acquired_multisim

DESCRIPTION
   This function is used by NAS to indicate the vocoder has been
   acquired but not enabled yet

=============================================================================*/
extern void gl1_msg_vocoder_acquired_multisim( sys_modem_as_id_e_type as_id );

/*=============================================================================
FUNCTION gl1_msg_vocoder_change_complete

DESCRIPTION
   This function is used by NAS to indicate the vocoder change is complete
   so L1 can take appropriate action (example: stop vocoder data blanking)

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_vocoder_change_complete( mvs_event_type event,
                                      mvs_mode_type ghdi_mvs_mode);

/*=============================================================================
FUNCTION gl1_msg_vocoder_change_complete_multisim

DESCRIPTION
   This function is used by NAS to indicate the vocoder change is complete
   so L1 can take appropriate action (example: stop vocoder data blanking)

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void gl1_msg_vocoder_change_complete_multisim( mvs_event_type event,
                                               mvs_mode_type ghdi_mvs_mode,
                                               sys_modem_as_id_e_type as_id );

/*=============================================================================
FUNCTION gl1_msg_vocoder_released

DESCRIPTION
   This function is used by NAS to indicate the vocoder is being
   released so L1 can take appropriate action

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
extern void gl1_msg_vocoder_released( void );

/*=============================================================================
FUNCTION gl1_msg_vocoder_released_multisim

DESCRIPTION
   This function is used by NAS to indicate the vocoder is being
   released so L1 can take appropriate action

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
extern void gl1_msg_vocoder_released_multisim( sys_modem_as_id_e_type as_id );

#endif
