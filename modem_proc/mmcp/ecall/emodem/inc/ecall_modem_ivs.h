#ifndef ECALL_MODEM_IVS_H_
#define ECALL_MODEM_IVS_H_
/*===========================================================================

                          E C A L L _ M O D E M _ I V S. H

DESCRIPTION
                                     
REFERENCES
    3GPP TS 26.267

EXTERNALIZED FUNCTIONS
    - ecall_modem_ivs_init()
    - ecall_ivs_getData()
    - ecall_ivs_process()
    - ecall_ivs_process_start()
    - ecall_ivs_process_abort_tx()
    - ecall_ivs_process_abort_all()
    - ecall_modem_ivs_err()

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodem/inc/ecall_modem_ivs.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $ 
===========================================================================*/

/*===========================================================================

                         I N C L U D E   F I L E S

===========================================================================*/

#include "comdef.h"
#include "customer.h"

/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/

#ifdef FEATURE_ECALL_IVS

#include "ecall_defines.h"
#include "ecall_modem.h"
#include "ecall_modem_apis.h"
#include "ecall_sync.h"
#include "queue.h"
#include "ecall_control.h"

#ifndef FEATURE_ECALL_HAS_V1_HEADER
#include "aprv2_packet.h"
#include "apr_errcodes.h"
#include "aprv2_api_inline.h"
#endif /* FEATURE_ECALL_HAS_V1_HEADER */

/*----------------------------------------------------------------------*/
/*          pcm buffer type for transmitting uplink frames              */
/*----------------------------------------------------------------------*/
typedef struct {
  q_link_type   link;
  int16         pcm[ECALL_PCM_LENGTH];
} ecall_pcm_buf_type;

/*----------------------------------------------------------------------*/
/*    IVS can be in idle state waiting for start triggers or send       */
/*      state once it starts sending sync frame followed by MSD         */
/*----------------------------------------------------------------------*/
typedef enum {
  IvsIdle =0,
  IvsTrigger,
  IvsStart,
  IvsSendMsd,
  IvsAck,
  IvsHlack
} ecall_ivs_state;

typedef enum {
  IvsModNone = 0,
  IvsModFast,
  IvsModRobust
} ecall_ivs_modulator_e_type;

/*----------------------------------------------------------------------*/
/*     structure to keep track of what frames  need to be sent          */
/*----------------------------------------------------------------------*/
typedef struct {
  ecall_ivs_state   state;       /* IVS transmitter state */
  ecall_ctrl_tx_data ctrl;       /* IVS control struct */
  ecall_mod_state mod;           /* IVS modulator struct */
  ecall_ivs_modulator_e_type mod_type; /* IVS modulator type */

  int16 rv;                      /* redundancy version */
  int16 ulN;                     /* uplink number of frames */
  int16 ulIndex;                 /* uplink frame counter */
  int16 ulDelay;                 /* uplink transmit offset in samples */
  int16 dlMsgOld;                /* previous control message */
  
  boolean  pendingStart;         /* indicates pending START message */
  int16 overallNack;             /* cumulative NACK count */
  //int16 stateCntNack;            /* global NACK counter */

  int16 stateCnt[ECALL_SET_LLMSG + ECALL_SET_HLMSG + 1];
  int16 stateIgn[ECALL_SET_LLMSG + ECALL_SET_HLMSG + 1];

  uint1  memCode[ECALL_NRB_CODE_BUFFER];
  int16  memDelay[2*ECALL_PCM_LENGTH];
} ecall_ivs_tx_data;

/*----------------------------------------------------------------------*/
/*     structure to keep track of the coutners on received frames       */
/*----------------------------------------------------------------------*/
typedef struct {
  ecall_ivs_state    state;      /* IVS receiver state */
  ecall_ctrl_rx_data ctrl;       /* IVS control struct */
  ecall_sync_state   sync;       /* IVS sync struct */

  int16 dlData;          /* downlink message symbol */
  int16 dlIndex;                 /* donwlink frame counter */
  int16 dlMsgCnt;                /* downlink message counter */

  int16 memCtrl[ECALL_NRF_MEMCTRL*ECALL_PCM_LENGTH];
  int32 memSync[ECALL_NRS_MEMSYNC];

} ecall_ivs_rx_data;

/*----------------------------------------------------------------------*/
/*                      IVS modem structure                             */
/*----------------------------------------------------------------------*/
typedef struct {
  ecall_ivs_rx_data rx;           /* IVS receiver struct */
  ecall_ivs_tx_data tx;           /* IVS transmitter struct */
} ecall_ivs_data;

/*----------------------------------------------------------------------*/
/*                      Vocoder session mode                            */
/*----------------------------------------------------------------------*/
typedef enum
{
  ECALL_VOC_MODE_RX_ONLY = 0,
  ECALL_VOC_MODE_RX_TX
}ecall_voc_mode_type;

/*============================================================================*/
/* IVS declarations                                                           */
/*----------------------------------------------------------------------------*/

/*==========================================================================

  FUNCTION    ECALL_MODEM_IVS_INIT

  DESCRIPTION
             Initialises the PCM queue for UL data frames to vocoder.
             Intialises the defualt state of the vocoder and the modem. 

  DEPENDENCIES
    None

  RETURN VALUE
    None
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_modem_ivs_init
(
  void
);


/*==========================================================================

  FUNCTION    ECALL_IVS_GETDATA

  DESCRIPTION
        current state of the IVS modem

  DEPENDENCIES
    None.

  RETURN VALUE
      Returns the structure of the IVS modem containg current state

  SIDE EFFECTS
    None

===========================================================================*/
ecall_ivs_data* ecall_ivs_getData
(
  void
);


/*==========================================================================

  FUNCTION    ECALL_IVS_PROCESS

  DESCRIPTION
       Processes the received frame the vocoder and puts a frame onto the 
       pcm command queue for tranmission

   InOut:  int16* pcm  <-> input and output frame of 16bit PCM samples 

  DEPENDENCIES
    None.

  RETURN VALUE
    None       

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_ivs_process
(
  int16 *pcm,
  uint32 pcm_size
);


/*==========================================================================

  FUNCTION    ECALL_IVS_PROCESS_START

  DESCRIPTION
  This would be called by the ivs task when the app call starts_transmit_data
  a command is queued up in IVS_TASK to process the START_TRANSMIT_DATA

  DEPENDENCIES
    None.

  RETURN VALUE
    On error conditions it calls the APP registered callback indicating
    an error occured in the modem

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_ivs_process_start
(
  const uint8 *ecall_msd
);


/*==========================================================================

  FUNCTION    ECALL_IVS_PROCESS_ABORT_TX

  DESCRIPTION
   This would be called by the ivs task when the app call ECALL_ABORT_TRANSMIT_DATA
   a command is queued up in IVS_TASK to process the ECALL_ABORT_TRANSMIT_DATA
   This would de-register the tx vocoder and continue monitoring the DL frames.
   App would tell whether to discard the current msd or save it for later transmission

  DEPENDENCIES
    None.

  RETURN VALUE
    On error conditions it calls the APP registered callback indicating
    an error occured in the modem 

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_ivs_process_abort_tx
(
  boolean discard_msd
);


/*==========================================================================

  FUNCTION    ECALL_IVS_PROCESS_ABORT_ALL

  DESCRIPTION
   This would be called by the ivs task when the app call ECALL_ABORT_ALL
   a command is queued up in IVS_TASK to process the ECALL_ABORT_ALL
   This would de-register the tx and rx vocoder. Modem would no longer
   monitor the DL frames. Modem would discard the current MSD.

  DEPENDENCIES
    None.

  RETURN VALUE
   On error conditions it calls the APP registered callback indicating
   an error occured in the modem       

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_ivs_process_abort_all
(
  void
);


/*==========================================================================

  FUNCTION    ECALL_POST_CLIENT_NOTIFY

  DESCRIPTION
  posts a notification to ivs task about the current status.
  On processing this notification modem would inform all the registered
  clients about the status.

  DEPENDENCIES
    None.

  RETURN VALUE
    if there are no command buffers to post, will skip the notification
    else no return value
 
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_post_client_notify
(
 void
);


/*===========================================================================

  FUNCTION ECALL_MODEM_IVS_ERR

  DESCRIPTION
      This function would be called with in the IVS on failure cases, 
      it would send and ECALL_IVS_ERR to app which inturn abort the TX data
      
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_modem_ivs_err
(
  void
);

/*===========================================================================

  FUNCTION ECALL_MODEM_HANDLE_VOC_TIMER_EXPIRE

  DESCRIPTION
    This function will reset the voc_timer if ecall still registered to
    vocoder rx, increment the missed_rx_pcm_count and feed a dummy pcm to
    ecall_feed_rx_pcm() to preserve offset.
      
  DEPENDENCIES
    None.

  RETURN VALUE
    Boolean.

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_modem_handle_voc_timer_expire
(
  void
);

/*===========================================================================

  FUNCTION ECALL_MODEM_IVS_REPORT_LOSE_SYNC

  DESCRIPTION
    This function will report eCall IVS lose sync status to its clients.
      
  DEPENDENCIES
    None.

  RETURN VALUE
    Boolean.

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_modem_ivs_report_lose_sync
(
   void
);

#if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
/*===========================================================================

     A - F a m i l y   &   B - F a m i l y   C o m m o n   F u n c t i o n

===========================================================================*/

/*==========================================================================
  FUNCTION ECALL_PROCESS_APR_PACKET

  DESCRIPTION
    This function will process the incoming APR packet.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_process_apr_packet
(
  aprv2_packet_t *packet
);

/*===========================================================================

  FUNCTION ECALL_MODEM_APR_INIT

  DESCRIPTION
    This function will initialize the APR interface for eCall

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_modem_apr_init 
( 
  void
);

/*==========================================================================
  FUNCTION ECALL_TX_PCM

  DESCRIPTION
    Pulls a PCM buffer off of the queue and copies it into the Vocoder's
    buffer for encoding and transmission.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
    The vocoder input takes 14 bit linear pcm samples and the 15th,16th bits
    are the sign extension bits. we need to downscale two bits to meet the vocoder format.
 
===========================================================================*/
void ecall_tx_pcm
(
  aprv2_packet_t* packet
);

/*==========================================================================
  FUNCTION ECALL_RX_PCM

  DESCRIPTION
    This function will copy the input data from packet, and post a command to
    process this PCM frame.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
   The vocoder output gives 14 bit linear pcm samples and the 15th,16th bits 
   are the sign extension bits. we need to up-scale two bits to get the actual data.
 
===========================================================================*/
void ecall_rx_pcm
(
  aprv2_packet_t* packet
);

/*==========================================================================
  FUNCTION ECALL_MODEM_MAP_SHARED_MEMORY

  DESCRIPTION
    This function will allocate a shared memory region from SMEM pool if not
    allocated yet, and send a map memory command to CVD thru APR.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_modem_map_shared_memory
(
  void
);

/*==========================================================================
  FUNCTION ECALL_MODEM_UNMAP_SHARED_MEMORY

  DESCRIPTION
    This function will send the unmap shared memory command to CVD thru APR.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_modem_unmap_shared_memory
(
  void
);

/*===========================================================================

  FUNCTION ECALL_MODEM_CVS_INIT

  DESCRIPTION
    This function will initialize the CVS session for eCall

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_modem_cvs_init 
( 
  void
);

/*==========================================================================
  FUNCTION ECALL_PROCESS_IBASIC_RSP_RESULT

  DESCRIPTION
    This function will process the incoming IBASIC_RSP_RESULT with
    the incoming APR packet.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_process_ibasic_rsp_result
(
  aprv2_packet_t *packet
);

/*==========================================================================
  FUNCTION ECALL_VOC_REGISTER

  DESCRIPTION
    Registers eCall functions with Vocoder so that eCall feeds PCM
    frames for output and receives decoded PCM input.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None

  SIDE EFFECTS
    None
 
===========================================================================*/
void ecall_voc_register
(
  ecall_voc_mode_type voc_mode
);

/*==========================================================================
  FUNCTION ECALL_PROCESS_IVPCM_EVT_NOTIFY

  DESCRIPTION
    This function will process the incoming IVPCM_EVT_NOTIFY every 20ms and
    start the RX/TX PCM process.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_process_ivpcm_evt_notify
(
  aprv2_packet_t* packet
);

/*==========================================================================
  FUNCTION ECALL_VOC_REGISTER_RX_TX

  DESCRIPTION
    Registers eCall functions with Vocoder RX and TX session so that eCall
    feeds PCM frames for output and receives decoded PCM input.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None

  SIDE EFFECTS
    None
 
===========================================================================*/
void ecall_voc_register_rx_tx
(
  void
);

/*==========================================================================
  FUNCTION ECALL_VOC_REGISTER_RX

  DESCRIPTION
    Registers eCall functions with Vocoder RX so that eCall receives decoded
    PCM input.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None

  SIDE EFFECTS
    None
 
===========================================================================*/
void ecall_voc_register_rx
(
  void
);

/*==========================================================================
  FUNCTION     ECALL_VOC_UNREGISTER

  DESCRIPTION
      This function will terminates an active vocoder PCM session.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

===========================================================================*/
void ecall_voc_unregister
(
  void
);

/*==========================================================================
  FUNCTION ECALL_PROCESS_IMEMORY_RSP_MAP

  DESCRIPTION
    This function will process the respond of VSS_IMEMORY_CMD_MAP_PHYSICAL
    and save the memory handle.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_process_imemory_rsp_map
(
  aprv2_packet_t* packet
);

#else /* Legacy Vocoder */
/*===========================================================================

                L e g a c y   V o c o d e r   F u n c t i o n

===========================================================================*/

/*==========================================================================

  FUNCTION ECALL_VOC_RX_REGISTER

  DESCRIPTION
    Registers eCall functions with Vocoder so that eCall feeds PCM
    frames for output and receives decoded PCM input.

  DEPENDENCIES
    Legacy Vocoder

  RETURN VALUE
    TRUE on success, FALSE on failure

  SIDE EFFECTS
    PCM input & output frames will be routed through eCall

===========================================================================*/
boolean ecall_voc_rx_register
(
  void
);

/*==========================================================================

  FUNCTION ECALL_VOC_TX_REGISTER

  DESCRIPTION
    Registers eCall functions with Vocoder so that eCall feeds PCM
    frames for output and receives decoded PCM input.

  DEPENDENCIES
    Legacy Vocoder

  RETURN VALUE
    TRUE on success, FALSE on failure

  SIDE EFFECTS
    PCM input & output frames will be routed through eCall

===========================================================================*/
boolean ecall_voc_tx_register
(
  void
);

/*==========================================================================

  FUNCTION ECALL_VOC_TX_UNREGISTER

  DESCRIPTION
    Removes registration of eCall functions with Vocoder so that eCall 
    will no longer be notified of incoming PCM frames and will also not
    be asked for PCM frames to send.

  DEPENDENCIES
    Legacy Vocoder

  RETURN VALUE
    None

  SIDE EFFECTS
    None.

===========================================================================*/
void ecall_voc_tx_unregister
(
  void
);

/*==========================================================================

  FUNCTION ECALL_VOC_RX_UNREGISTER

  DESCRIPTION
    Removes registration of eCall functions with Vocoder so that eCall 
    will no longer be notified of incoming PCM frames and will also not
    be asked for PCM frames to send.

  DEPENDENCIES
    Legacy Vocoder

  RETURN VALUE
    None

  SIDE EFFECTS
    None.

===========================================================================*/
void ecall_voc_rx_unregister
(
  void
);

#endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */

#else

void ecall_modem_ivs_dummy_func(void);

#endif /* End FEATURE_ECALL_IVS */

#endif /* ECALL_MODEM_IVS_H_ */

