#ifndef GL1_HW_VS_G_H
#define GL1_HW_VS_G_H
/*========================================================================
                 GSM GPRS NPL1 DEFINITIONS FILE
DESCRIPTION
   This file contains definitions used by the Voice Services interface

Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
========================================================================== */

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdrivers/inc/gl1_hw_vs_g.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
06/12/13    pjr     CR581212: Added events to report status
11/11/13    ws      CR 573757 Featurise GSMTR-1 registers and Voice services driver
16-05-13    pjr     CR487402 updated function prototypes
03-04-13    PJR     CR470940 Initial revision
========================================================================== */
#ifdef CUST_H
#include "customer.h"
#endif

#if defined (FEATURE_DUAL_SIM) && defined (FEATURE_GSM_VS_DRIVER)
#include "gl1_defs_g.h"
#include "gl1_msg_g.h"


/*===========================================================================

                     Public Defines & Constants

===========================================================================*/
#define GL1_VS_EOK          ( 0x00000000 ) /* Success / completed / no errors. */
#define GL1_VS_EFAILED      ( 0x00000001 ) /* General failure. */


/* =======================================================================
**                   FUNCTIONS used to interact with VS interface
** ======================================================================= */
extern mvs_mode_type gl1_vs_get_mvs_mode(gl1_msg_chan_type chan_type, gl1_msg_voc_type voc_type);

extern uint32 gl1_vs_set_ul_amr_mode(gl1_msg_amr_mode_type  amr_mode);
extern uint32 gl1_vs_get_packet_from_vocoder(gsm_voc_packet_type* gsm_voc_packet);
extern uint32 gl1_vs_sent_packet_to_vocoder(gsm_voc_packet_type* voc_packet);
extern uint32 gl1_vs_set_dtx_mode(mvs_dtx_mode_type dtx_mode);
extern uint32 gl1_vs_acquire(void);
extern uint32 gl1_vs_release(void);
extern uint32 gl1_vs_standby(mvs_mode_type mvs_mode);
extern uint32 gl1_vs_stop_vocoder(void);
extern uint32 gl1_vs_start_vocoder(gl1_msg_voc_type vs_msg_voc_type, 
                                   gl1_msg_chan_type vs_msg_chan_type);

extern boolean gl1_vs_is_vocoder_ready(void);

/* =======================================================================
**                   ENUMERATIONS used to report events
** ======================================================================= */
typedef enum
{
  VS_EVENT_TYPE_ACQUIRE,      
  VS_EVENT_TYPE_RELEASE,      
  VS_EVENT_TYPE_START_VOCODER,
  VS_EVENT_TYPE_STOP_VOCODER, 
  VS_EVENT_TYPE_STANDBY,      
  VS_EVENT_TYPE_SET_AMR_MODE, 
  VS_EVENT_TYPE_READY_CB,     
  VS_EVENT_TYPE_NOT_READY_CB 
} gl1_vs_event_code_t;

typedef enum
{
  VS_EVENT_STATUS_SUCCESS,
  VS_EVENT_STATUS_FAIL,
  VS_EVENT_STATUS_FAIL_TO_ALLOC_BUFFER,
  VS_EVENT_STATUS_FAIL_TO_FLUSH_BUFFER,
  VS_EVENT_STATUS_ALREADY_OPEN,
  VS_EVENT_STATUS_VS_NOT_OPEN,
  VS_EVENT_STATUS_VS_NOT_READY,
  VS_EVENT_STATUS_INVALID_AS_ID,
  VS_EVENT_STATUS_INVALID_VOCODER_TYPE
} gl1_vs_event_status_t;

typedef enum
{
  VS_EVENT_MODE_NULL,
  VS_EVENT_MODE_AMR,
  VS_EVENT_MODE_EFR,
  VS_EVENT_MODE_HR,
  VS_EVENT_MODE_AMR_WB,
  VS_EVENT_MODE_FR,

  VS_EVENT_MODE_0475,
  VS_EVENT_MODE_0515,
  VS_EVENT_MODE_0590,
  VS_EVENT_MODE_0670,
  VS_EVENT_MODE_0740,
  VS_EVENT_MODE_0795,
  VS_EVENT_MODE_1020,
  VS_EVENT_MODE_1220,
  VS_EVENT_MODE_WB_0660,
  VS_EVENT_MODE_WB_0885,
  VS_EVENT_MODE_WB_1265,
} gl1_vs_event_mode_t;

#endif /*FEATURE_DUAL_SIM && FEATURE_GSM_VS_DRIVER*/
#endif /* GL1_HW_VS_G_H */

