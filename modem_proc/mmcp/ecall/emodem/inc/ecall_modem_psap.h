#ifndef ECALL_MODEM_PSAP_H_
#define ECALL_MODEM_PSAP_H_
/*===========================================================================

                        E C A L L _ M O D E M _ P S A P . C

DESCRIPTION
    eCall PSAP modem                                      

REFERENCES
  3GPP TS 26.267 

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodem/inc/ecall_modem_psap.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $ 
===========================================================================*/

/*===========================================================================

                         I N C L U D E   F I L E S

===========================================================================*/
#ifdef FEATURE_ECALL_PSAP

#include "ecall_defines.h"
#include "ecall_control.h"
#include "ecall_modem.h"
#include "ecall_sync.h"

#include "queue.h"

#ifndef FEATURE_ECALL_HAS_V1_HEADER
#include "aprv2_packet.h"
#include "apr_errcodes.h"
#include "aprv2_api_inline.h"
#endif /* FEATURE_ECALL_HAS_V1_HEADER */

/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/
typedef struct {
  q_link_type   link;
  int16         pcm[ECALL_PCM_LENGTH];
} ecall_pcm_buf_type;

typedef enum {
  PsapIdle =0,
  PsapTrigger,
  PsapStart,
  PsapNack,
  PsapAck,
  PsapHlack
} ecall_psap_state;

typedef struct {
  ecall_ctrl_rx_data ctrl;       /* PSAP control struct */ 
  ecall_sync_state sync;         /* PSAP sync struct */
  ecall_mod_state  mod;          /* PSAP modulator struct */

  ecall_psap_state state;       /* receiver state */
  int16 rv;               /* redundancy version */
  int16 ulN;              /* uplink number of frames (without muting) */
  int16 ulIndex;          /* uplink frame counter */
  int16 mgIndex;          /* uplink position in muting gap table */
  int16 ulTrials;         /* uplink decoding trails */
  int16 ulSyncTail;       /* sync observation counter after sync success */
  int16 decStep;          /* decoding Steps */
  

  uint8 dlHlackData;      /* downlink higher-layer message (4 bits) */
  int16 dlData;           /* downlink message symbol */
  int16 dlIndex;          /* donwlink frame counter */
  int16 dlMsgCnt;         /* downlink message counter */

  uint8   *msd;            /* MSD in byte representation */
  uint32  *msdWord;         /* MSD in word representation */
  int16   *memCtrl;        /* buffer for control and data demodulation */

  char buffer[0
    + sizeof(uint32)* ECALL_NRW_INFO_CRC
    + sizeof(int16) * ECALL_NRF_MEMCTRL*ECALL_PCM_LENGTH
    + sizeof(int32) * ECALL_NRS_MEMSYNC
    + sizeof(int32) * 2*(ECALL_NRF_SYNC+1)];
} ecall_psap_rx_data;

typedef struct {
  ecall_ctrl_tx_data ctrl;        /* PSAP control struct */
} ecall_psap_tx_data;

typedef struct {
  ecall_psap_rx_data rx;          /* PSAP receiver struct */
  ecall_psap_tx_data tx;          /* PSAP transmitter struct */
} ecall_psap_data;


typedef enum {
  PULL_MODE = 0,
  PUSH_MODE
} ecall_psap_tx_mode_type;

/*==========================================================================
                          Internal Functions
===========================================================================*/

/*==========================================================================

  FUNCTION    ECALL_PSAP_RESET

  DESCRIPTION
    main synchronization function

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_reset
(
  void
);

/*==========================================================================

  FUNCTION    ECALL_PSAP_PROCESS

  DESCRIPTION
    main synchronization function

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_process
(
  int16 *pcm,
  uint32 pcm_size
);

/*==========================================================================

  FUNCTION    ECALL_PSAP_RX_RESET

  DESCRIPTION
   
  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_rx_reset
(
  void
);

/*==========================================================================

  FUNCTION    ECALL_PSAP_RX_PROCESS

  DESCRIPTION
   
  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_rx_process
(
  const int16 *pcm,
  uint32 pcm_size
);

/*==========================================================================

  FUNCTION    ECALL_PSAP_RX_UPLINK

  DESCRIPTION
    
  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_rx_uplink
(
  const int16 *pcm
);

/*==========================================================================

  FUNCTION    ECALL_PSAP_RX_DOWNLINK

  DESCRIPTION
    main synchronization function

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_rx_downlink
(
  void
);

/*==========================================================================

  FUNCTION    ECALL_PSAP_TX_RESET

  DESCRIPTION
   
  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_tx_reset
(
  void
);

/*==========================================================================

  FUNCTION    ECALL_PSAP_TX_PROCESS

  DESCRIPTION
    

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_tx_process
(
  int16 *pcm,
  uint32 pcm_size
);


/*==========================================================================

  FUNCTION ECALL_GET_CMD_BUF

  DESCRIPTION
    Returns a command pointer off of the free queue, or NULL on failure.

  DEPENDENCIES
    None.

  RETURN VALUE
    Command pointer or NULL

  SIDE EFFECTS
    None

===========================================================================*/
ecall_pcm_buf_type *ecall_get_pcm_buf
(
  void
);


/*==========================================================================

  FUNCTION ECALL_PUT_PCM_BUF

  DESCRIPTION
    Adds a PCM buffer to the outgoing queue

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_put_pcm_buf
(
  ecall_pcm_buf_type  *buf_ptr
);



/*==========================================================================
                          External Functions
===========================================================================*/

/*==========================================================================

  FUNCTION    PSAPGETDATA

  DESCRIPTION
    

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
ecall_psap_data* PsapGetData
(
  void
);

/*==========================================================================

  FUNCTION    ECALL_MODEM_PSAP_INIT

  DESCRIPTION
    main synchronization function

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_modem_psap_init
(
  void
);

/*==========================================================================
  FUNCTION    ECALL_PSAP_SESSION_STOP

  DESCRIPTION
   ends the ecall session and switches to voice
   Parameters: TRUE  -- Indicates to post restart cmd
               FALSE -- does not post restart cmd
 
  DEPENDENCIES
    None.

  RETURN VALUE
    None.
 
  SIDE EFFECTS
    None
===========================================================================*/
void ecall_psap_session_stop
(
 boolean restart_cmd
);

/*==========================================================================

  FUNCTION    ECALL_PSAP_SEND_START

  DESCRIPTION
    main synchronization function

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_send_start
(
  ecall_psap_tx_mode_type psap_tx_mode
);

/*==========================================================================

  FUNCTION ECALL_MODEM_VOC_UNREGISTER

  DESCRIPTION
    Removes registration of eCall functions with Vocoder so that eCall 
    will no longer be notified of incoming PCM frames and will also not
    be asked for PCM frames to send.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None.

===========================================================================*/
void ecall_voc_unregister
(
  void
);

/*===========================================================================

FUNCTION     ECALL_PSAP_EFS_READ

DESCRIPTION
    Reads the PSAP_EFS file to choose between CANNED msgs or the actual PSAP 
    behaviour.  It reads the Number of times to restart the PSAP with  a 
    restart timer (delay) after sending successful ACK messages.

    LOOP 4 2000 -- Repeats the PSAP behaviour 4 times with a 2 secs delay between
                  each repetition

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ecall_psap_efs_read
(
  void
);

/*===========================================================================

FUNCTION     ECALL_PSAP_GET_FORCE_PULL_MODE

DESCRIPTION
    This API gets the force_pull_mode info that eCall PSAP fetch from the EFS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean ecall_psap_get_force_pull_mode
(
  void
);


void ecall_psap_rx_setmod
(
  const ecall_sync_state *sync,
  ecall_mod_state *mod
);

/*===========================================================================

FUNCTION     ECALL_MODEM_PSAP_ERR

DESCRIPTION
    it will be called to abort the transmission if
    ecall modem library finds any invalid data
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ecall_modem_psap_err(void);

/*===========================================================================

FUNCTION     ECALL_MODEM_PSAP_FLUSH_QUEUE

DESCRIPTION
    This function will flush out all queued item in ecall_pcm_q
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ecall_modem_psap_flush_queue
(
  void
);

#if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
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
  FUNCTION ECALL_MODEM_MAP_SHARED_MEMORY

  DESCRIPTION
    This function will allocate a memory pool for APR interface to process
    the vocoder input/output PCM buffers.

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
    This function will unmap the memory pool for APR interface.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_modem_unmap_shared_memory
(
  void
);

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
  FUNCTION ECALL_RX_PCM

  DESCRIPTION
    This function will copy the input data from packet, and post a command to
    process this PCM frame

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_rx_pcm
(
  aprv2_packet_t *packet
);

/*==========================================================================
  FUNCTION ECALL_TX_PCM

  DESCRIPTION
    Pulls a PCM buffer off of the queue and copies it into the Vocoder's
    buffer for encoding and transmission.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_tx_pcm
(
  aprv2_packet_t *packet
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

  FUNCTION ECALL_VOC_REGISTER

  DESCRIPTION
    Registers eCall functions with Vocoder so that eCall feeds PCM
    frames for output and receives decoded PCM input.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
    PCM input & output frames will be routed through eCall

===========================================================================*/
void ecall_voc_register
(
  void
);

#else /* Legacy Vocoder */

/*==========================================================================

  FUNCTION ECALL_VOC_REGISTER

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
boolean ecall_voc_register
(
  void
);

#endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */

#else

void ecall_modem_psap_dummy_func(void);

#endif //End FEATURE_ECALL_PSAP

#endif

