#ifndef __VSS_ICCM_PRIVATE_IF_H__
#define __VSS_ICCM_PRIVATE_IF_H__

/*
   Copyright (C) 2013 QUALCOMM Technologies Incorporated.
   All rights reserved.
   Qualcomm Confidential and Proprietary

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/vsd/inc/private/vss_iccm_private_if.h#1 $
   $Author: mplcsds1 $
*/

#include "apr_comdef.h"

/****************************************************************************
 * CVD CONCURRENCY MONITOR CORE APIS                                        *
 ****************************************************************************/

/**
  Event broadcast by the CCM to all active MVM sessions in the system,
  indicating the total number of nb/wb+ streams and vocprocs in the system.
 
  There are two possible events that can trigger the CCM to broadcast this event
  to all active MVM sessions:
 
    1. A #VSS_IMVM_EVT_VOICE_SESSION_ACTIVE event is receieved from an MVM
    session indicating to the CCM that it is going to voice active state.
 
    2. A #VSS_IMVM_EVT_VOICE_SESSION_INACTIVE event is received from an MVM
    session indicating to the CCM that it has gone to voice inactive state.
 
  Each MVM session receving this event utilizes the infomation provided to
  configure the voice path timing offsets for the streams/vocprocs connected
  to it.
 
  @par Payload
  #vss_iccm_evt_active_sessions_t
  
  @return 
  None.

  @dependencies
  None.
 
  @comments
  This event is sent only to sessions whose last status is known to CCM
  as active (i.e. their last notification was #VSS_IMVM_EVT_VOICE_SESSION_ACTIVE).
  
*/
#define VSS_ICCM_EVT_ACTIVE_SESSIONS ( 0x00012E6A )

/** @brief Type definition for #vss_iccm_evt_active_sessions_t.
*/
typedef struct vss_iccm_evt_active_sessions_t vss_iccm_evt_active_sessions_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ICCM_EVT_ACTIVE_SESSIONS event.
*/
struct vss_iccm_evt_active_sessions_t
{
  uint16_t num_voice_sessions;
    /**< Total number of voice sesssions in the system. */
  uint16_t num_nb_streams;
    /**< Total number of narrow-band (8 KHz) streams in the system. */
  uint16_t num_wb_plus_streams;
    /**< Total number of wide-band (16 KHz) or higher streams in the system. */
  uint16_t num_nb_vocprocs;
    /**< Total number of narrow-band (8 KHz) vocprocs in the system. */
  uint16_t num_wb_plus_vocprocs;
    /**< Total number of wide-band (16 Khz) or higher vocprocs in the system. */
  uint32_t per_hw_thread_kpps;
    /**< VDSP current KPPS per hardware thread. */
}
#include "apr_pack_end.h"
;

#endif /* __VSS_ICCM_PRIVATE_IF_H__ */

