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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodem/src/ecall_modem_psap.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $ 
===========================================================================*/

/*===========================================================================

                         I N C L U D E   F I L E S

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_ECALL_PSAP

#include "msg.h"
#include "err.h"

#include "ecall_modem_psap.h"
#include "ecall_psap_task.h"
#include "ecall_fec.h"
#include "ecall_rom.h"
#include "ecall_efs.h"
#include "ecall_psap.h"
#include "event.h"
#include "secapi.h"

#ifdef FEATURE_ECALL_HAS_V1_HEADER
#include "voc.h"
#include "snd.h"
#else /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */
#include "vss_ivpcm_public_if.h"
#include "vss_common_public_if.h"
#include "vss_istream_public_if.h"
#include "smem.h"
#endif /* FEATURE_ECALL_HAS_V1_HEADER */

#ifdef FEATURE_ECALL_HAS_VOICE_A
#include "blast_memory.h"
#elif (defined FEATURE_ECALL_HAS_VOICE_B)
#include "blast.h"
#include "vss_imemory_public_if.h"
#endif /* FEATURE_ECALL_HAS_VOICE_A */

#ifdef FEATURE_MVS
#include "mvs.h"
#endif


/*-------------------------------------------------------------------------
              Buffers for transmitting PCM frames
 --------------------------------------------------------------------------*/
#define ECALL_PCM_BUF_COUNT     8

/* Outgoing PCM sample queue */
static ecall_pcm_buf_type   ecall_pcm_buf[ECALL_PCM_BUF_COUNT];
static q_type               ecall_pcm_free_q;
static q_type               ecall_pcm_q;

/*-------------------------------------------------------------------------
              define a structure to read the PSAP EFS data 
 --------------------------------------------------------------------------*/
static ecall_psap_efs_struct_type ecall_psap_efs_config;
  
static ecall_psap_efs_sample_type ecall_psap_efs_sample;  
/*-------------------------------------------------------------------------
   PSAP Structure used for maintiaing the received and transmitting states
 --------------------------------------------------------------------------*/
static ecall_psap_data psap;

/*-------------------------------------------------------------------------
  global variable to keep track of vocoder state
 --------------------------------------------------------------------------*/
static boolean ecall_psap_voc_registered = FALSE;

LOCAL struct
{
#ifdef FEATURE_ADIE_CODEC_OFF           /* Internal codec is disabled */
    uint16 volume;
#endif
#ifdef FEATURE_MVS
    mvs_dtx_mode_type mvs_dtx_mode;
    mvs_scr_mode_type mvs_scr_mode;
#endif
#ifdef FEATURE_ECALL_21KHZ_TONE
  int16  m_cToneEC;
#endif
  int16   pcm_tx_buf_cnt;
  boolean ready_to_deregister;
  uint32 rx_process_cnt;
  uint32 tx_process_cnt;
  #if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
  uint32 push_buf_mask;
  uint32 ecall_tx_out_buf_addr;
  uint32 ecall_tx_in_buf_addr;
  uint32 ecall_rx_out_buf_addr;
  uint32 ecall_rx_in_buf_addr;
  uint32 ecall_tx_out_buf_phy_addr;
  uint32 ecall_tx_in_buf_phy_addr;
  uint32 ecall_rx_out_buf_phy_addr;
  uint32 ecall_rx_in_buf_phy_addr;
  boolean smem_mapped;
  boolean smem_allocated;
  uint32 rx_evt_notify_cnt;
  uint32 tx_evt_notify_cnt;

  #endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */

  #ifdef FEATURE_ECALL_HAS_VOICE_B
  uint32_t ecall_memory_handle;
  #endif /* FEATURE_ECALL_HAS_VOICE_B */

}ecall_modem_psap;


#define ECALL_SAMPLE_BUF_LEN  480

static int16 ecallsample_buffer[ECALL_SAMPLE_BUF_LEN];
static int16 ecall_copy_position = 160;

#if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
/* Vocoder PCM parameters */
#define ECALL_SMEM_SIZE      4096
static uint16 ecall_modem_apr_src_addr;
static uint16 ecall_modem_apr_dest_addr;
static uint16 ecall_modem_apr_src_port = 0xDDDD;
static uint16 ecall_modem_apr_dest_port = 0;

static char_t ecall_modem_stream_name[] = "default modem voice";
static uint32 ecall_modem_apr_handle;
static char ecall_modem_psap_ecall_dns[] = "qcom.modem.ecall";
static char ecall_modem_psap_voc_dns[] = "qcom.adsp.cvs";

uint32 ecall_smem_virtual_addr_orig;
uint32 ecall_smem_physical_addr_orig;
uint32 ecall_smem_virtual_addr;
uint32 ecall_smem_physical_addr;
#endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */

#ifdef FEATURE_ECALL_HAS_VOICE_B
static uint16 ecall_modem_apr_mvm_addr;
static char ecall_modem_psap_mvm_dns[] = "qcom.audio.mvm";
static uint32 ecall_memory_table_virtual_addr_orig;
static uint32 ecall_memory_table_physical_addr_orig;
static uint32 ecall_memory_table_virtual_addr;
static uint32 ecall_memory_table_physical_addr;
#endif /* FEATURE_ECALL_HAS_VOICE_B */

typedef enum
{
  ECALL_APR_IVPCM_STOP = 1,
  ECALL_APR_IVPCM_START,
  ECALL_APR_MAP_MEMORY,
  ECALL_APR_UNMAP_MEMORY,
  ECALL_APR_PUSH_BUFFER,
  ECALL_APR_CREATE_PASSIVE_CONTROL_SESSION
}ecall_apr_token_type;

/*==========================================================================
              PSAP Internal reference 
 ==========================================================================*/

/*==========================================================================

  FUNCTION    ECALL_MODEM_PSAP_EVENT_REPORT

  DESCRIPTION
    Prints the event to the qxdm

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_modem_psap_event_report
(
  event_id_enum_type event_id
)
{
  byte event_payload[2];

  switch (event_id)
  {
    case EVENT_ECALL_PSAP_MSD_DECODE_SUCCESS:
      if(psap.rx.mod.type == Mod3bit4smp)
      {
        event_payload[0] = 0;        /* FAST Modulation */
      }
      else if(psap.rx.mod.type == Mod3bit8smp)
      {
        event_payload[0] = 1;        /* SLOW Modulation */
      }
      else
      {
        event_payload[0] = 2;        /* undefined modulation */
      }

      event_payload[1] = (byte)psap.rx.rv;             /* Number of Redundancy Versions  */
      event_report_payload(EVENT_ECALL_PSAP_MSD_DECODE_SUCCESS, 2, (void *)event_payload); 
      break;

    case EVENT_ECALL_PSAP_LOST_SYNC:
      event_payload[0] = 0;  
      event_report_payload(EVENT_ECALL_PSAP_LOST_SYNC, 1, (void *)event_payload);
      break;

    default:
      ECALL_MSG_0( ECALL_ERROR, "Unknow eCall event " );
      break;
  }
}

#ifdef FEATURE_ECALL_21KHZ_TONE
/*==========================================================================
  FUNCTION    ECALL_PCM_TONE

  DESCRIPTION
        transmits the 2.1 Khz tone to disable network Echo Cancellors

  DEPENDENCIES
    None.

  RETURN VALUE
    In:     uint8* pcmrev                                                                     
    out:    none 
    
  SIDE EFFECTS
    None
===========================================================================*/
void ecall_pcm_tone
(
  int16 *pcmrev
)
{
  int16 i, j, count, phase;

  if(ecall_modem_psap.m_cToneEC >= 0)
  {
    for (i = 0; i < 2; i++)
    {
      count = ecall_modem_psap.m_cToneEC*2 + i;
      phase = (((ECALL_TONE_OFFSET+count)/45)%2) ? -1 : 1;
    
      for (j = 0; j < ECALL_PCM_LENGTH/2; j++)
      {
        pcmrev[i*ECALL_PCM_LENGTH/2+j] = phase*ecall_tone2100[j];
      }
    }
    
    /* 450 ms phase reversal of 10 cycle */
    if ((++ecall_modem_psap.m_cToneEC)*2 >= 45*10)
    {
      ecall_modem_psap.m_cToneEC = -1;
    }
  }
  else
  {
    /* fail safe condition */
    memset(pcmrev, 0, sizeof(int16)*ECALL_PCM_LENGTH);
  }
}

#endif /*end of FEATURE_ECALL_21KHZ_TONE*/

/*==========================================================================

  FUNCTION    ECALL_PSAP_RESET

  DESCRIPTION
    Reset the PSAP modem and the RX, TX counters

  DEPENDENCIES
    None

  RETURN VALUE
    None
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_reset
(
  void 
)
{
  ecall_psap_rx_reset();
  ecall_psap_tx_reset();
}


/*==========================================================================
              PSAP internal reference 
 ==========================================================================*/
/*==========================================================================

  FUNCTION    ecall_psap_sendHlack

  DESCRIPTION
    
  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_sendHlack
(
  const uint8 data
)
{
  /* HLACK mode requires MSD to be successfully received */
  if (psap.rx.state == PsapAck)
  {
    psap.rx.state = PsapHlack;
    psap.rx.dlMsgCnt    = -1;
    psap.rx.dlHlackData = data & 0x0F;

    ECALL_MSG_1( ECALL_HIGH, "[PSAP] HLACK data %d received from control----",
                              data );
  }
  else
  {
    ECALL_MSG_0( ECALL_HIGH, "[PSAP] HLACK data received from control, ignored----" );
  }
}

/*==========================================================================

  FUNCTION    ECALL_PSAP_SEND_RESTART

  DESCRIPTION
    Restart the modem if running into too many redundant versions or
    bad sync's
 
  DEPENDENCIES
    None.

  RETURN VALUE
    None.
 
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_send_restart
(
  void
)
{
  if (psap.rx.state == PsapIdle)
  {
    /* START trigger out of IDLE state */
    ECALL_MSG_0( ECALL_HIGH, "[PSAP] SendStart received from control----" );
    ecall_psap_rx_reset();
    psap.rx.state = PsapTrigger;
  }
  else if (psap.rx.state == PsapNack)
  {
    /* START trigger out of NACK state */
    ECALL_MSG_0( ECALL_HIGH, "[PSAP] SendStart received, starting over...----" );
    ecall_sync_reset(&psap.rx.sync, NULL, NULL);
    psap.rx.state      = PsapTrigger;
    psap.rx.dlMsgCnt   = -1;
    psap.rx.rv         = 0;
    psap.rx.mgIndex    = 0;
    psap.rx.ulIndex    = 0;
    psap.rx.ulSyncTail = 0;
  }
  else
  { /* ignore otherwise */
    ECALL_MSG_0( ECALL_HIGH, "[PSAP] SendStart received from control, ignored.----" );
  }
}

/*==========================================================================

  FUNCTION    ECALL_PSAP_RX_RESET

  DESCRIPTION
    Reset the counters for receiveing data

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_rx_reset
(
  void
) 
{
  ecall_sync_state *sync = &psap.rx.sync;
  ecall_ctrl_rx_data *ctrl = &psap.rx.ctrl;

  /* reset state */
  psap.rx.state = PsapIdle;

  /* assign buffer pointers */
  psap.rx.msdWord = (uint32*)  &psap.rx.buffer[0];
  psap.rx.msd     = (uint8*) &psap.rx.buffer[sizeof(uint32)*ECALL_NRW_INFO_CRC];
  psap.rx.memCtrl = (int16*) &psap.rx.buffer[sizeof(uint32)*ECALL_NRW_INFO_CRC];
  
  /* reset control receiver */
  ecall_ctrl_rx_reset(ctrl, PortPsap, sync, psap.rx.memCtrl, ECALL_LOCK_START_UL);

  /* reset uplink counters */
  psap.rx.rv         = 0;
  psap.rx.ulIndex    = 0;
  psap.rx.mgIndex    = 0;
  psap.rx.ulTrials   = 0;
  psap.rx.ulSyncTail = 0;
  psap.rx.decStep    = 0;

  /* reset HLACK data and downlink counters */
  psap.rx.dlHlackData = 0;
  psap.rx.dlData      = DlMsgNoop;
  psap.rx.dlMsgCnt    = -1;
  psap.rx.dlIndex     = -1;

  /* reset sync */
  ecall_sync_reset(sync,
    (int32*)&psap.rx.buffer[0
    + sizeof(uint32)* ECALL_NRW_INFO_CRC
    + sizeof(int16) * ECALL_NRF_MEMCTRL*ECALL_PCM_LENGTH],
    (int32*)&psap.rx.buffer[0
    + sizeof(uint32)* ECALL_NRW_INFO_CRC
    + sizeof(int16) * ECALL_NRF_MEMCTRL*ECALL_PCM_LENGTH
    + sizeof(int32) * ECALL_NRS_MEMSYNC]);

  /* clear memory */
  memset(psap.rx.buffer, 0, sizeof(char)*(0
    + sizeof(uint32)* ECALL_NRW_INFO_CRC
    + sizeof(int16) * ECALL_NRF_MEMCTRL*ECALL_PCM_LENGTH
    + sizeof(int32) * ECALL_NRS_MEMSYNC
    + sizeof(int32) * 2*(ECALL_NRF_SYNC+1)));

  ECALL_MSG_0( ECALL_HIGH, "[PSAP] reset receiver" );
}

/*==========================================================================
  FUNCTION    ECALL_PSAP_TX_RESET

  DESCRIPTION
     Reset the counters for transmitting PCM data and set mnodem 
     state to idle

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_tx_reset
(
  void
)
{
  ecall_ctrl_tx_reset(&psap.tx.ctrl, PortPsap);
  ECALL_MSG_0( ECALL_HIGH, "[PSAP] reset transmitter" );
}

/*==========================================================================
  FUNCTION    ECALL_PROCESS_SAMPLE_BUFFER

  DESCRIPTION
     This is an intermediate buffer giving PCM TX frames to the PCM cmd q,
     we buffer two frames so we adjust slips or stuffs to be introduced on
     PSAP TX for test puposes.
 
     numSamples is +1 introduces one sample
     numSamples is -1 slips one sample to be sent
 
  DEPENDENCIES
    we need to have the ENABLE_SLIP TRUE in ecall_psap_efs.txt

  RETURN VALUE
    None.
 
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_process_sample_buffer
(
  int16 *pcm,
  uint32 pcm_size
)
{
  int16 length  =  0;

  ecall_ctrl_port *ptx = &psap.tx.ctrl.port;

  length = (sizeof(int16)*ECALL_SAMPLE_BUF_LEN -  sizeof(int16)*ECALL_PCM_LENGTH);

  /* 1st condtion is to check for START/ACK/NACK and HLACK could be anything between
     ECALL_SET_LLMSG and ECALL_SET_LLMSG + ECALL_SET_HLMSG */
  if( ptx->u.tx.dlData  == ecall_psap_efs_sample.sampleMsg ||
     (ecall_psap_efs_sample.sampleMsg == DlMsgHlack && ptx->u.tx.dlData >= ECALL_SET_LLMSG))
  {
    if(psap.rx.dlMsgCnt  == ecall_psap_efs_sample.msgIndex      && 
      (ptx->u.tx.dlIndex == ecall_psap_efs_sample.frameIndex[0] ||
       ptx->u.tx.dlIndex == ecall_psap_efs_sample.frameIndex[1] ||
       ptx->u.tx.dlIndex == ecall_psap_efs_sample.frameIndex[2]))
    {
      ECALL_MSG_0( ECALL_ERROR, "Slip/stuff sample " );
      ecall_copy_position = ecall_copy_position + ecall_psap_efs_sample.numSamples;
  
      ECALL_MSG_3( ECALL_HIGH, "length %d ecall_position %d  num_samples %d",
                                length,
                                ecall_copy_position,
                                ecall_psap_efs_sample.numSamples );
   
      ecall_memsmove ( &ecallsample_buffer[ecall_copy_position], sizeof(int16)*(ECALL_SAMPLE_BUF_LEN-ecall_copy_position),
                       &ecallsample_buffer[160], length );
    }
  }
  
  ecall_memscpy  ( pcm, pcm_size,
                   &ecallsample_buffer[0], ECALL_PCM_LENGTH_BYTES ); 

  ecall_memsmove ( &ecallsample_buffer[0], sizeof(ecallsample_buffer),
                   &ecallsample_buffer[160], length );
}

/*==========================================================================
  FUNCTION    ECALL_PSAP_PROCESS

  DESCRIPTION
      Process the received PCM frame and queue up the frame to be be 
      transmitted

  DEPENDENCIES
    None.

  RETURN VALUE
      int16* pcm  <-> input and output frame of 16bit PCM samples

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_process
(
  int16 *pcm,
  uint32 pcm_size
)
{
  ecall_ctrl_port *prx = &psap.rx.ctrl.port;
  ecall_ctrl_port *ptx = &psap.tx.ctrl.port;
  ecall_pcm_buf_type  *pcm_buf = NULL;
  int16 i;

  if(ecall_psap_efs_config.canned_psap == FALSE)
  {
    /* invert input signal if required */
    if (prx->invert == TRUE)
    {
      for (i = 0; i < ECALL_PCM_LENGTH; i++)
      {
         pcm[i] = (pcm[i] > ECALL_PCM_MIN) ? -pcm[i] : ECALL_PCM_MAX;
      }
    }

    ecall_psap_rx_process(pcm, pcm_size);
  
    if (prx->state == CtrlTxIdle || prx->state == CtrlTxSend)
    {
      /* port match, interface copy */
      ptx->state        = prx->state;
      ptx->u.tx.dlData  = prx->u.tx.dlData;
      ptx->u.tx.dlIndex = prx->u.tx.dlIndex;
    }
    else
    {
      /* port mismatch, no downlink payload */
      ptx->state        = CtrlTxIdle;
      ptx->u.tx.dlData  = DlMsgNoop;
      ptx->u.tx.dlIndex = -1;
    }
  }

  /* Get the free PCM buffer*/
  pcm_buf = ecall_get_pcm_buf();

  if (pcm_buf == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "pcm tx buffer ran out" );
    return;
  }

  if( ecall_psap_efs_config.enable_slip == TRUE)
  {
#ifdef FEATURE_ECALL_21KHZ_TONE
    if(ecall_modem_psap.m_cToneEC >= 0)
    {
      ecall_pcm_tone(&ecallsample_buffer[ecall_copy_position]);
    }
    else
#endif
    {
      ecall_psap_tx_process(&ecallsample_buffer[ecall_copy_position], ECALL_PCM_LENGTH_BYTES);
    }
    ecall_process_sample_buffer ( pcm_buf->pcm, sizeof(pcm_buf->pcm) ); 
  }
  else
  {
#ifdef FEATURE_ECALL_21KHZ_TONE
    if(ecall_modem_psap.m_cToneEC >= 0)
    {
      ecall_pcm_tone(pcm_buf->pcm);
    }
    else
#endif
    {
      ecall_psap_tx_process(pcm_buf->pcm, sizeof(pcm_buf->pcm));
    }
  }

  /* invert output signal if required */
  if (ptx->invert == TRUE ||
      ecall_psap_efs_config.invert_sig == TRUE)
  {
    for (i = 0; i < ECALL_PCM_LENGTH; i++)
    {
      pcm_buf->pcm[i] = (pcm_buf->pcm[i] > ECALL_PCM_MIN) ? -pcm_buf->pcm[i] : ECALL_PCM_MAX;      
    }
  }

  /* Put the TX frame in pcm queue*/
  ecall_put_pcm_buf(pcm_buf);
}

/*==========================================================================
  FUNCTION    ECALL_PSAP_RX_PROCESS

  DESCRIPTION
    main synchronization function

  DEPENDENCIES
    None.

  RETURN VALUE
      int16* pcm  <-> input and output frame of 16bit PCM samples

  SIDE EFFECTS
    None
===========================================================================*/
void ecall_psap_rx_process
(
  const int16 *pcm,
  uint32 pcm_size
)
{
  int16 i;

  /* PSAP receiver, copy PCM data to input buffer */
  for (i = 0; i < ECALL_NRF_MEMCTRL-1; i++)
  {
    ecall_memscpy  ( &psap.rx.memCtrl[i*ECALL_PCM_LENGTH], ECALL_PCM_LENGTH_BYTES,
                     &psap.rx.memCtrl[(i+1)*ECALL_PCM_LENGTH], ECALL_PCM_LENGTH_BYTES );
  }
  ecall_memscpy ( &psap.rx.memCtrl[(ECALL_NRF_MEMCTRL-1)*ECALL_PCM_LENGTH], ECALL_PCM_LENGTH_BYTES,
                  pcm, pcm_size );

  /* process uplink and downlink state machine */
  ecall_psap_rx_uplink(pcm);
  ecall_psap_rx_downlink();
}

/*==========================================================================
  FUNCTION    ECALL_PSAP_POST_RESTART_CMD

  DESCRIPTION
      Posts a command to ECALL task to set the restart for sending START 
      frames to IVS after the timer expiry

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_post_restart_cmd
(
  void
)
{
  ecall_cmd_type *cmd_ptr;

  if(ecall_psap_efs_config.num_times_to_loop <= 0)
  {
    ECALL_MSG_0( ECALL_HIGH, "Finished the restart " );
    return;
  }

  ecall_psap_efs_config.num_times_to_loop--;

  cmd_ptr = ecall_get_cmd_buf();

  if (cmd_ptr != NULL)
  {
    /* Post a command to the eCall task */
    cmd_ptr->cmd_id            = ECALL_CMD_PROCESS_RESTART_TIME;
    cmd_ptr->data.restart_time = ecall_psap_efs_config.restart_timer ;       

    ecall_put_cmd(cmd_ptr);
  }
  else
  {
    ECALL_MSG_0( ECALL_ERROR, "Dropped ECALL_CMD_PROCESS_RESTART_TIME command; ran out of buffers" );
  }
}

/*==========================================================================
  FUNCTION    ECALL_PSAP_GET_NEXT_CANNED_MSG

  DESCRIPTION
    Indicates what next canned msg to be sent

  DEPENDENCIES
    This will be called only is PSAP_CANNED is TRUE in the efs file

  RETURN VALUE
     next canned msg to be sent
      
  SIDE EFFECTS
    None
===========================================================================*/
void ecall_psap_get_next_canned_msg
(
  void
)
{
  ecall_ctrl_port *ptx = &psap.tx.ctrl.port;

  static uint8 ecall_psap_canned_msg_index = 0;
  static uint8 ecall_psap_canned_msg_count = 0;

  if(ecall_psap_canned_msg_count < ecall_psap_efs_config.canned_msg_array[ecall_psap_canned_msg_index][1])
  {
    ecall_psap_canned_msg_count++;
  }
  else
  {
    ecall_psap_canned_msg_index++;
    ecall_psap_canned_msg_count = 0;
    psap.rx.dlMsgCnt = -1;
  }

  if(ecall_psap_canned_msg_index >= ecall_psap_efs_config.canned_msg_count)
  {
    ecall_psap_canned_msg_index = 0;
    ecall_psap_canned_msg_count = 0;
    ECALL_MSG_0( ECALL_HIGH, "finished canned message sequence restarting" );

    ecall_psap_session_stop(TRUE);
  }

  ptx->u.tx.dlData = ecall_psap_efs_config.canned_msg_array[ecall_psap_canned_msg_index][0];


  switch (ptx->u.tx.dlData)
  {
    case DlMsgStart:
       ECALL_MSG_2( ECALL_HIGH, "canned msg index :%d sending START, tx.dlIndex:%d",
                                 ecall_psap_canned_msg_index,
                                 ptx->u.tx.dlIndex );
       break;

    case DlMsgNack:
       ECALL_MSG_2( ECALL_HIGH, "canned msg index :%d sending NACK, tx.dlIndex:%d",
                                 ecall_psap_canned_msg_index,
                                 ptx->u.tx.dlIndex );
       break;

    case DlMsgAck:
       ECALL_MSG_2( ECALL_HIGH, "canned msg index :%d sending ACK, tx.dlIndex:%d",
                                 ecall_psap_canned_msg_index,
                                 ptx->u.tx.dlIndex );
       break;

    case DlMsgHlack:
       ECALL_MSG_2( ECALL_HIGH, "canned msg index :%d sending HL-ACK, tx.dlIndex:%d",
                                 ecall_psap_canned_msg_index,
                                 ptx->u.tx.dlIndex );
       break;

    default:
       ECALL_MSG_3( ECALL_HIGH, "PSAP: error - unknown message canned msg index :%d, contents: %d , tx.dlIndex:%d",
                                 ecall_psap_canned_msg_index,
                                 ecall_psap_efs_config.canned_msg_array[ecall_psap_canned_msg_index],
                                 ptx->u.tx.dlIndex );
       break;
  }


  /* reset downlink index */
  ptx->u.tx.dlIndex = 0;
  psap.rx.dlMsgCnt++;
}

/*==========================================================================
  FUNCTION    ECALL_PSAP_TX_PROCESS

  DESCRIPTION
     processes the pcm frame to be sent

  DEPENDENCIES
    None.

  RETURN VALUE
      int16* pcm  <-> input and output frame of 16bit PCM samples

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_tx_process
(
  int16 *pcm,
  uint32 pcm_size
)
{
  ecall_ctrl_port *ptx = &psap.tx.ctrl.port;

  if (((ptx->u.tx.dlIndex >= ECALL_NRF_DLCHUNK) || (ptx->u.tx.dlIndex == 0))&&
       (ecall_psap_efs_config.canned_psap== TRUE))
  {
    ECALL_MSG_0( ECALL_HIGH, "Symbol transmitted, moving to next symbol" );
    ecall_psap_get_next_canned_msg();
  }

  ecall_ctrl_tx_process(&psap.tx.ctrl, pcm, pcm_size);

  if(ecall_psap_efs_config.canned_psap== TRUE)
  {
    ptx->u.tx.dlIndex++;
  }
}

/*==========================================================================
  FUNCTION    ECALL_PSAP_RX_UPLINK

  DESCRIPTION
    main synchronization function

  DEPENDENCIES
    None.

  RETURN VALUE
     const int16* pcm  -> input frame of 16bit PCM samples

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_rx_uplink
(
  const int16 *pcm
)
{
  int16  temp;
  boolean  crc = FALSE;
  ecall_IntLLR softBits[30];

  int16 mgTableVal;

  ecall_ctrl_rx_data *ctrl = &psap.rx.ctrl;
  ecall_sync_state *sync = &psap.rx.sync;
  ecall_mod_state  *mod  = &psap.rx.mod;

  /* process each state individually */
  switch (psap.rx.state)
  {
    case PsapIdle:
      /* run control receiver and watch for SEND signal */
      ecall_ctrl_rx_process(ctrl, pcm);

      if (ctrl->port.u.rx.dlData == DlMsgSend)
      {
        ECALL_MSG_1( ECALL_HIGH, "[PSAP] received SEND  (metric: %3d)",
                                  ctrl->port.u.rx.dlMetric );

        /* reset receiver and prepare for MSD reception */
        ecall_psap_rx_reset();
        psap.rx.state = PsapTrigger;
      }
      else if (ctrl->port.u.rx.dlData == DlMsgReset) {
        ecall_psap_reset();
      }
      break;

    case PsapTrigger:
      ECALL_MSG_0( ECALL_HIGH, "state is PsapTrigger: wait for IVS to transmit" );
      /* start sending control messages, wait for IVS to transmit */
      ctrl->port.state = CtrlTxIdle;
      break;

    case PsapStart:
      /* run sync with new input data */
      ctrl->port.state = CtrlTxIdle;
      ecall_sync(sync, pcm, "PSAP", ctrl->port.invert);
      ECALL_MSG_0( ECALL_HIGH, "state is PsapStart: run sync with new input data" );

      if (sync->flag == TRUE && sync->delay <= 0)
      {
        /* sync successful */
        ctrl->port.state = CtrlTxSend;

        /* apply sync inversion flag */
        if ((ctrl->port.invert = sync->invert) == TRUE) {
          ECALL_MSG_0( ECALL_HIGH, "[PSAP] codec inversion detected!----" );
        }

        /* start sending NACK messages */
        psap.rx.state = PsapNack;
        psap.rx.dlMsgCnt = -1;

        /* choose modulator */
        ecall_psap_rx_setmod(sync, mod);

        /* check for muting gaps passed already */
        if (sync->delay <=(-ECALL_PCM_LENGTH))
        {
          temp = MIN(-(sync->delay)/ECALL_PCM_LENGTH, mod->nfmute1);
          sync->delay += temp*ECALL_PCM_LENGTH;   /*lint !e734*/
          psap.rx.mgIndex += temp;
        }
      }
      break;

    case PsapNack:
      /* check for better sync for some more frames */
      if(psap.rx.ulSyncTail < ECALL_NRF_OBSERVE)
      {
        psap.rx.ulSyncTail++;

        /* run synchronization */
        ecall_sync(sync, pcm, "PSAP", ctrl->port.invert);

        if (sync->flag == TRUE && sync->delay <= 0)
        {
          sync->amplitude[2] = sync->amplitude[0];

          /* apply sync inversion flag */
          if ((ctrl->port.invert = sync->invert) == TRUE) {
            ECALL_MSG_0( ECALL_HIGH, "[PSAP] codec inversion detected (sync observer)!----" );
          }

          /* reset receiver, previous sync was likely too early */
          psap.rx.rv         = 0;
          psap.rx.ulIndex    = 0;
          psap.rx.mgIndex    = -1;
          psap.rx.ulSyncTail = 0;

          /* choose modulator */
          ecall_psap_rx_setmod(sync, mod);

          /* check for muting gaps passed already */
          if (sync->delay <= (-ECALL_PCM_LENGTH))
          {
            temp = MIN(-(sync->delay)/ECALL_PCM_LENGTH, mod->nfmute1);
            sync->delay += temp*ECALL_PCM_LENGTH;               /*lint !e734*/
            psap.rx.mgIndex += temp;
          }
        }
      } else if ((mod->mgTable[psap.rx.mgIndex]) > 512) {
        /* continue updating sync filter state for sync tracking purposes */
        ecall_sync_filter(sync, pcm, ctrl->port.invert);
      }
      /* run demodulation if not muting */

      if (psap.rx.mgIndex >= 0)
      {
        mgTableVal = (mod->mgTable[psap.rx.mgIndex])%512-7;

        if (mgTableVal >= 0)
        {
          ecall_psap_receiver(mod,
            &psap.rx.memCtrl[(ECALL_NRF_MEMCTRL-1)*ECALL_PCM_LENGTH + sync->delay],
            softBits);

          ecall_update_buffer(softBits, psap.rx.rv,
            (mod->mfpf)*(mod->bpsym)*psap.rx.ulIndex,
            (mod->mfpf)*(mod->bpsym));

          psap.rx.ulIndex++;
        }
        else if (mgTableVal == -6) {
          /* track sync peaks and correct delay if necessary */
          ecall_sync_track(sync, ctrl->port.invert);
          memset(sync->mem, 0, sizeof(int32)*ECALL_NRS_MEMSYNC);

          if (sync->flag == TRUE) {
            while (sync->delay > 0) {
              sync->delay -= ECALL_PCM_LENGTH;
              psap.rx.mgIndex--;
            }
            while (sync->delay <= -ECALL_PCM_LENGTH) {
              sync->delay += ECALL_PCM_LENGTH;
              psap.rx.mgIndex++;

              mgTableVal = (mod->mgTable[psap.rx.mgIndex])%512-7;

              if (mgTableVal >= 0) {
                
                ecall_psap_receiver(mod,
                  &psap.rx.memCtrl[(ECALL_NRF_MEMCTRL-1)*ECALL_PCM_LENGTH + sync->delay],
                  softBits);
                psap.rx.ulIndex++;
              }
            }
          }
          if (sync->checkCnt >= ECALL_SYNC_BADTRACK) {
            /* too many tracking failures */
            ECALL_MSG_0( ECALL_HIGH, "[PSAP] restart due to tracking failures!----" );
            ecall_psap_send_restart();
            break;
          }
        }
      }
      /* increase muting gap counter */
      psap.rx.mgIndex++;

      /* run decoder if desired */
      if ( (psap.rx.rv%ECALL_ARQ_MAX > 0) || (psap.rx.ulIndex == psap.rx.ulN) )
      {
        if ( (psap.rx.mgIndex > 0) && ((mod->mgTable[psap.rx.mgIndex-1]%512-7) >= 0) )
        {
          crc = ecall_fec_decode(psap.rx.msdWord, psap.rx.decStep);

          if ((++psap.rx.decStep) == ECALL_FEC_ITERATIONS*ECALL_DEC_STEPS) 
          {
            psap.rx.decStep = 0;
          }
          else
          {
            crc = FALSE;
          }
        }
        
        if (crc == FALSE)
        {
          /* full block was received, reinitialization necessary */
          if (psap.rx.ulIndex == psap.rx.ulN)
          { 
            ECALL_MSG_1( ECALL_HIGH, "[PSAP] CRC failed; rv = %d",
                                      psap.rx.rv );
            /* initialization for new redundancy version */
            psap.rx.ulIndex = 0;
            psap.rx.rv++;

            if ((psap.rx.rv%ECALL_ARQ_MAX) == 0)
            {
              /* too many retransmissions, sync must have failed */
              ecall_psap_send_restart();
              psap.rx.ulTrials++;
            }
            else
            {
              psap.rx.mgIndex = -(mod->nfmute4);
            }
          }
        }
        else
        {
          ECALL_MSG_1( ECALL_HIGH, "PSAP: successful transmission; %d number of retransmissions:",
                                    psap.rx.rv );
          ecall_modem_psap_event_report(EVENT_ECALL_PSAP_MSD_DECODE_SUCCESS);
          /* set ACK and convert binary MSD to ASCII */
          psap.rx.state = PsapAck;
          psap.rx.dlMsgCnt = -1;

          ecall_word2byte(psap.rx.msdWord, psap.rx.msd, ECALL_NRW_INFO);

          /* send MSD received to control unit */
          ecall_psap_receive_msd(psap.rx.msd, ECALL_MSD_MAX_LENGTH);
        }
      }
      break;

    case PsapAck:
      /* MSD was received, keep sending ACK messages */
      ECALL_MSG_0( ECALL_MED, "Rx PsapAck" );
      break;

    case PsapHlack:
      /* MSD was received, higher-layer message in progress */
      ECALL_MSG_0( ECALL_MED, "Rx PsapHlack" );
      break;

    default:
      /* error */ 
      ECALL_MSG_0( ECALL_ERROR, "PSAP state not recognized, exit." );
      break;
  }
}

/*==========================================================================
  FUNCTION    ECALL_PSAP_RX_DOWNLINK

  DESCRIPTION
 

  DEPENDENCIES
    None.

  RETURN VALUE
     const int16* pcm  -> input frame of 16bit PCM samples

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_rx_downlink
(
  void 
)
{
  ecall_ctrl_port *port = &psap.rx.ctrl.port;

  if(
    port->state == CtrlRxIdle ||
    port->state == CtrlRxSync ||
    port->state == CtrlRxLock)
  {
    /* PSAP in control receiver mode, port data was set above */
  }
  else if (psap.rx.state == PsapIdle && psap.rx.dlMsgCnt >= 0)
  {
    /* final message has been sent, stop output */
    port->u.tx.dlData  = DlMsgNoop;
    port->u.tx.dlIndex = -1;
  }
  else 
  {
    /* interface copy */
    port->u.tx.dlData  = psap.rx.dlData;
    port->u.tx.dlIndex = psap.rx.dlIndex;

    /* increase downlink index */
    psap.rx.dlIndex++;
    psap.rx.dlIndex %= ECALL_NRF_DLCHUNK;

    if (psap.rx.dlIndex == 0)
    {
      /* negative counter indicates new message */
      if (psap.rx.dlMsgCnt < 0)
      {
        psap.rx.dlMsgCnt = 0;
      }
      else
      {
        psap.rx.dlMsgCnt++;
      }
      /* check message counter and change state */
      switch (psap.rx.state)
      {
        case PsapTrigger:
          if (psap.rx.dlMsgCnt >= ECALL_LOCK_START_DL)
          {
            psap.rx.state = PsapStart;
            psap.rx.dlMsgCnt = 0;
          }
          else
          {
            ECALL_MSG_0( ECALL_HIGH, "[PSAP] sending  START" );
          }
          psap.rx.dlData = DlMsgStart;
          break;

        case PsapStart:
          if (psap.rx.dlMsgCnt >= (ECALL_PSAP_NUMSTART - ECALL_LOCK_START_DL))
          {
            psap.rx.state = PsapIdle;
            psap.rx.dlMsgCnt = 0;
            
            /* did not get sync from IVS restart after timer expiry*/
            ecall_psap_session_stop(TRUE);
          }
          break;

        case PsapAck:
          if (psap.rx.dlMsgCnt >= ECALL_PSAP_NUMACK)
          {
            ECALL_MSG_1( ECALL_MED, "sendHlack with hlack_data %d",
                                     ecall_psap_efs_config.hlack_data );
            ecall_psap_sendHlack(ecall_psap_efs_config.hlack_data);
          }
          break;

        case PsapHlack:
          if (psap.rx.dlMsgCnt >= ECALL_PSAP_NUMHLACK)
          {
           
            ECALL_MSG_2( ECALL_HIGH, "read to deregister index %d pcm_tx count %d",
                                      port->u.tx.dlIndex,
                                      q_cnt(&ecall_pcm_q) );

            ecall_modem_psap.pcm_tx_buf_cnt      = q_cnt(&ecall_pcm_q) + 1;
            ecall_modem_psap.ready_to_deregister = TRUE;

            psap.rx.state = PsapIdle;
            psap.rx.dlMsgCnt = 0;
          }
          else
          {
            if (psap.rx.dlMsgCnt == 0)
            {
              ECALL_MSG_1( ECALL_HIGH, "[PSAP] sending  HLACK; data: 0x0%X",
                                        psap.rx.dlHlackData );
            }
            else
            {
              ECALL_MSG_0( ECALL_HIGH, "[PSAP] sending  HLACK" );
            }
            /* add message offset */
            psap.rx.dlData = ECALL_SET_LLMSG + psap.rx.dlHlackData; /*lint !e64*/
          }
          break;

        default:
           /* other states do not require message counting */
          break;
      }
    }
    else if (psap.rx.dlIndex == ECALL_NRF_SYNC + ECALL_NRF_DLMUTE1LL) 
    {
      /* start of downlink data portion, set next message */
      switch (psap.rx.state)
      {
        case PsapTrigger:
          if (psap.rx.dlMsgCnt < 0)
          {
            psap.rx.dlData   = DlMsgStart;
            psap.rx.dlMsgCnt = 0;
            ECALL_MSG_0( ECALL_HIGH, "[PSAP] sending  START (restart)" );
          }
          break;
        case PsapStart:
          psap.rx.dlData = DlMsgStart;
          if (psap.rx.dlMsgCnt < 0)
          {
            psap.rx.dlMsgCnt = 0;
          }
          ECALL_MSG_0( ECALL_HIGH, "[PSAP] sending  START" );
          break;
        case PsapNack:
          psap.rx.dlData = DlMsgNack;
          if (psap.rx.dlMsgCnt < 0)
          {
            psap.rx.dlMsgCnt = 0;
          }
          ECALL_MSG_0( ECALL_HIGH, "[PSAP] sending  NACK" );
          break;
        case PsapAck:
          psap.rx.dlData = DlMsgAck;
          if (psap.rx.dlMsgCnt < 0)
          {
            psap.rx.dlMsgCnt = 0;
          }
          ECALL_MSG_0( ECALL_HIGH, "[PSAP] sending  ACK" );
          break;
        case PsapHlack:
          if (psap.rx.dlMsgCnt < 0) 
          {
            psap.rx.dlData = DlMsgAck;
            ECALL_MSG_0( ECALL_HIGH, "[PSAP] sending  ACK   (HLACK pending)" );
          }
          break;
        default:
          ECALL_MSG_0( ECALL_HIGH, "[PSAP] remaining in IDLE mode" );
          break;
      }
    }
  }
}

#ifdef FEATURE_ADIE_CODEC_OFF

/*==========================================================================
  FUNCTION      ECALL_PSAP_SND_VOL_CB_FUNC

  DESCRIPTION
   Call back called from snd_get_volume, which would give us the present 
   volume level

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
 /*lint -esym(715,status) -esym(715,client_data) */
void ecall_psap_snd_vol_cb_func
(
  const void      *client_data, /* pointer to Client data   */
  snd_status_type status,       /* Status returned by Sound */
  uint16          volume        /* Volume Level             */
)
{
  ECALL_MSG_1( ECALL_HIGH, "volume level %d",
                            volume );
  if(volume == 0)
  {
    volume++;
    ECALL_MSG_1( ECALL_HIGH, " make volume non zero %d",
                               volume );
    snd_set_volume( SND_DEVICE_AUXPCM_HANDSET, SND_METHOD_VOICE, volume , NULL, NULL); 
  }
  ecall_modem_psap.volume = volume;
}    

#endif

/*==========================================================================
  FUNCTION     ECALL_VOC_CONFIG_RESTORE

  DESCRIPTION
      This would be called after the abort_tx or abort_all to restore the
      original vocoder settings for speech before eCall modified the
      vocoder configuration

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

===========================================================================*/
void ecall_modem_psap_voc_restore
(
  void
)
{

  #ifdef FEATURE_MVS
  #if defined(MSMAUD_VOC_AMR) || defined(MSMAUD_VOC_HR) \
     || defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR) \
     || defined(MSMAUD_VOC_AMR_WB)

    ECALL_MSG_0( ECALL_HIGH, "restore AMR DTX " );
    mvssup_amr_set_scr_mode(ecall_modem_psap.mvs_scr_mode);

    ECALL_MSG_0( ECALL_HIGH, "restore GSM DTX " );
    mvssup_gsm_set_dtx_mode(ecall_modem_psap.mvs_dtx_mode);

  #endif
  #endif

}
 
/*==========================================================================
  FUNCTION     ECALL_MODEM_PSAP_VOC_CONFIGURE

  DESCRIPTION
    configure the vocoder as required for eCall transmission

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ecall_modem_psap_voc_configure
(
  void
)
{
#ifdef FEATURE_ADIE_CODEC_OFF
  ECALL_MSG_0( ECALL_HIGH, "get Handset volume" );
  snd_get_volume(SND_DEVICE_AUXPCM_HANDSET, SND_METHOD_VOICE,  ecall_psap_snd_vol_cb_func, NULL);

  ECALL_MSG_0( ECALL_HIGH, "selecting device SND_DEVICE_AUXPCM_HANDSET" );
  snd_set_device( SND_DEVICE_AUXPCM_HANDSET, SND_MUTE_UNMUTED, SND_MUTE_UNMUTED, NULL, NULL);
#endif

  #ifdef FEATURE_MVS
  #if defined(MSMAUD_VOC_AMR) || defined(MSMAUD_VOC_HR) \
     || defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR) \
     || defined(MSMAUD_VOC_AMR_WB)

  #if ( !(defined FEATURE_ECALL_HAS_VOICE_A) && !(defined FEATURE_ECALL_HAS_VOICE_B) ) /* Legacy Vocoder */
  ecall_modem_psap.mvs_dtx_mode  = mvssup_gsm_get_dtx_mode();
  ECALL_MSG_1( ECALL_HIGH, "get gsm DTX state %d",
                            ecall_modem_psap.mvs_dtx_mode );
  
  ecall_modem_psap.mvs_scr_mode  = mvssup_amr_get_scr_mode();
  ECALL_MSG_1( ECALL_HIGH, "get amr DTX state  %d",
                            ecall_modem_psap.mvs_scr_mode );
  #endif /* Legacy Vocoder */

  ECALL_MSG_0( ECALL_HIGH, "AMR DTX Disable" );
  mvssup_amr_set_scr_mode(MVS_SCR_DISABLE);

  ECALL_MSG_0( ECALL_HIGH, "GSM DTX OFF" );
  mvssup_gsm_set_dtx_mode(MVS_DTX_OFF);

  #endif
  #endif

}

/*==========================================================================
  FUNCTION ECALL_GET_PCM_BUF

  DESCRIPTION
    Returns a command pointer off of the free queue, or NULL on failure.

  DEPENDENCIES
    None.

  RETURN VALUE
    Command pointer or NULL

  SIDE EFFECTS
    None
===========================================================================*/
ecall_pcm_buf_type *ecall_get_pcm_buf(void)
{
  ecall_pcm_buf_type *buf_ptr;
  if ((buf_ptr = q_get(&ecall_pcm_free_q)) == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "No items on eCall task outgoing PCM buffer q" );
  }
  return buf_ptr;
} /* ecall_get_cmd_buf() */


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
)
{
  q_put(&ecall_pcm_q, &buf_ptr->link);
} /* ecall_put_pcm_buf() */


/*==========================================================================
              PSAP external reference 
 ==========================================================================*/

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
)
{
   return &psap;
}

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
)
{
  uint8 i=0;
  ecall_pcm_buf_type  *buf_ptr;

  /* Initialize outgoing PCM queues */
  (void) q_init(&ecall_pcm_q);
  (void) q_init(&ecall_pcm_free_q);

  for (i = 0; i < ECALL_PCM_BUF_COUNT; i++)
  {
    (void) q_link(&ecall_pcm_buf[i], &ecall_pcm_buf[i].link);
     q_put(&ecall_pcm_free_q, &ecall_pcm_buf[i].link);
  }

  while ((buf_ptr = (ecall_pcm_buf_type *) q_get(&ecall_pcm_q)) != NULL)
  {
    q_put(&ecall_pcm_free_q, &buf_ptr->link);
  }

  #if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
  ecall_modem_psap.ecall_rx_in_buf_addr      = 0;
  ecall_modem_psap.ecall_rx_out_buf_addr     = 0;
  ecall_modem_psap.ecall_tx_in_buf_addr      = 0;
  ecall_modem_psap.ecall_tx_out_buf_addr     = 0;
  ecall_modem_psap.ecall_tx_in_buf_phy_addr  = 0;
  ecall_modem_psap.ecall_tx_out_buf_phy_addr = 0;
  ecall_modem_psap.ecall_rx_in_buf_phy_addr  = 0;
  ecall_modem_psap.ecall_rx_out_buf_phy_addr = 0;
  ecall_modem_psap.rx_evt_notify_cnt         = 0;
  ecall_modem_psap.tx_evt_notify_cnt         = 0;
  ecall_modem_psap.rx_process_cnt            = 0;
  ecall_modem_psap.tx_process_cnt            = 0;
  #endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */
}

/*==========================================================================
  FUNCTION    ECALL_PSAP_EFS_READ

  DESCRIPTION
    Reads the PSAP_EFS file to choose between CANNED msgs or the actual PSAP 
    behaviour.  It reads the Number of times to restart the PSAP with  a 
    restart timer (delay) after sending successful ACK messages.

    LOOP 4 2000 -- Repeats the PSAP behaviour 4 times with a 2 secs delay between
                  each repetition

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None
===========================================================================*/
void ecall_psap_efs_read
(
  void
)
{
  /* Read from EFS to choose between canned data or real data. Also number of times to repeat the
     loop after the interval specified in the EFS file*/
  ecall_psap_efs_read_canned_msg( &ecall_psap_efs_config );
}

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
)
{
  return ecall_psap_efs_config.force_pull_mode;
}

/*==========================================================================
  FUNCTION    ECALL_PSAP_SESSION_STOP

  DESCRIPTION
   ends the ecall session and switches to voice

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None
===========================================================================*/
void ecall_psap_session_stop
(
 boolean restart_cmd
)
{
  if(ecall_psap_efs_config.num_times_to_loop <= 0)
  {
    ECALL_MSG_0( ECALL_MED, "No pending restart, deregister for vocoder" );
    /* de-register the Rx and TX with the vocoder */
    ecall_voc_unregister();
  }

  /* Clean up the variables */
  memset(ecallsample_buffer ,0 , ECALL_SAMPLE_BUF_LEN);
  ecall_copy_position   = 160;

  ecall_modem_psap.pcm_tx_buf_cnt      = 0;
  ecall_modem_psap.ready_to_deregister = FALSE;
  
  ecall_modem_psap.rx_process_cnt      = 0;
  ecall_modem_psap.tx_process_cnt      = 0;

  #if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
  ecall_modem_psap.rx_evt_notify_cnt   = 0;
  ecall_modem_psap.tx_evt_notify_cnt   = 0;
  #endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */

#ifdef FEATURE_ECALL_21KHZ_TONE
  /* reset the counter to send 2.1KHZ tone every time before START */
  ecall_modem_psap.m_cToneEC = 0;
#endif

  if(restart_cmd == TRUE)
  {
    /* Post a command to restart the ecall session after timer expiry*/
    ecall_psap_post_restart_cmd();
  }

}

/*==========================================================================
  FUNCTION    ECALL_PSAP_GET_RANDOM

  DESCRIPTION
    This function calls secapi_get_random() to get secure random number from
    Hardware Random Number Generator (PRNG), and return the random value.

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None
===========================================================================*/
uint8 ecall_psap_get_random
(
  void
)
{
  uint8 random_value = 0;
  secerrno_enum_type  sec_errno = E_FAILURE;

  sec_errno = secapi_get_random( SECAPI_SECURE_RANDOM,
                                 (uint8 *)&random_value, 
                                 sizeof(random_value) );  
  if ( sec_errno != E_SUCCESS )
  {
    ECALL_MSG_0( ECALL_ERROR, "Error generating random numbers" );
  }

  return random_value;
} /* ECALL_PSAP_GET_RANDOM */

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
)
{
  int16 i;

  ecall_psap_rx_reset();
  ecall_psap_tx_reset();

  /* If enable_slip falg is true in ecall_psap_efs.txt will read the
     ecall_psap_sample.txt file to populate where to introduce the slip/stuf
     if the file doesn't exists will use random number generator.
  */
  if ( (ecall_psap_efs_config.enable_slip == TRUE) && 
       (ecall_psap_efs_read_samples(&ecall_psap_efs_sample) == FALSE) )
  {
    ecall_psap_efs_sample.sampleMsg   = ecall_psap_get_random() % 3;
    ecall_psap_efs_sample.numSamples  = ecall_psap_get_random() % 3;
    ecall_psap_efs_sample.msgIndex    = ecall_psap_get_random() % 3;

    for(i=0;i<ecall_psap_efs_sample.numSamples;i++)
    {
      ecall_psap_efs_sample.frameIndex[i]  = ecall_psap_get_random() % 21;
    }

    /* Postive for stuff and negative for slips*/
    if ( ecall_psap_get_random() % 2 == 0 )
    {
      ecall_psap_efs_sample.numSamples *= 1 ;
    }
    else
    {
      ecall_psap_efs_sample.numSamples *= -1 ;    
    }
  }

  ECALL_MSG_3( ECALL_HIGH, "sample msgtype %d numsamples %d msgIndex %d", 
                            ecall_psap_efs_sample.sampleMsg, 
                            ecall_psap_efs_sample.numSamples,
                            ecall_psap_efs_sample.msgIndex );

  ECALL_MSG_3( ECALL_HIGH, "frameIndex[0] %d  frameIndex[1] %d  frameIndex[2] %d", 
                            ecall_psap_efs_sample.frameIndex[0], 
                            ecall_psap_efs_sample.frameIndex[1],
                            ecall_psap_efs_sample.frameIndex[2] );


  if (psap_tx_mode == PUSH_MODE) 
  {
    ECALL_MSG_0( ECALL_HIGH, "PSAP: Rx PUSH Mode " );
   
  }
  else
  {
    ECALL_MSG_0( ECALL_HIGH, "PSAP: Rx PULL Mode " );
    psap.rx.state = PsapTrigger;
  }

}


/*============================================================================*/
/* UTILITY FUNCTION: PsapRxSetMod                                             */
/*----------------------------------------------------------------------------*/
/* In:  const SyncState *sync  -> synchronization parameters and results      */
/* Out: ModState        *mod   -> current modulator parameters                */
/*----------------------------------------------------------------------------*/
void ecall_psap_rx_setmod
(
  const ecall_sync_state *sync,
  ecall_mod_state *mod
)
{
  int32 xCorrA, xCorrB;

  xCorrA =
    sync->memWakeup[0] +
    sync->memWakeup[2] +
    sync->memWakeup[4] +
    sync->memWakeup[6];
  xCorrB =
    sync->memWakeup[1] +
    sync->memWakeup[3] +
    sync->memWakeup[5] +
    sync->memWakeup[7];

  if (xCorrA/MAX(xCorrB, 1) > ECALL_PSAP_THRESHOLD) {
    ecall_set_mod_state(mod, Mod3bit4smp);
    ECALL_MSG_1( ECALL_HIGH, "[PSAP] fast modulator chosen (metric: %d)",
                              xCorrA/MAX(xCorrB, 1) );
  }
  else if (xCorrB/MAX(xCorrA, 1) > ECALL_PSAP_THRESHOLD) {
    ecall_set_mod_state(mod, Mod3bit8smp);
    ECALL_MSG_1( ECALL_HIGH, "[PSAP] robust modulator chosen (metric: %d)",
                              xCorrB/MAX(xCorrA, 1) );
  }
  else if (psap.rx.ulTrials == 0) {
    ecall_set_mod_state(mod, Mod3bit4smp);
    ECALL_MSG_1( ECALL_HIGH, "[PSAP] fast modulator chosen (metric: %d)",
                              xCorrA/MAX(xCorrB, 1) );
  }
  else {
    ecall_set_mod_state(mod, Mod3bit8smp);
    ECALL_MSG_1( ECALL_HIGH, "[PSAP] robust modulator chosen (metric: %d)",
                              xCorrB/MAX(xCorrA, 1) );
  }
  psap.rx.ulN = mod->nfdata;
}
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
void ecall_modem_psap_err(void)
{

  ECALL_MSG_0( ECALL_HIGH, "psap error. restart the transmission" );
  ecall_psap_post_restart_cmd();
}

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
)
{
  ecall_pcm_buf_type *buf_ptr;
  if (ecall_psap_efs_config.restart_timer == 0)
  {
    ECALL_MSG_0( ECALL_MED, "Request for restart right away, flush TX queue" );
    while (q_cnt(&ecall_pcm_q) > 0)
    {
      if ((buf_ptr = (ecall_pcm_buf_type *) q_get(&ecall_pcm_q)) != NULL)
      {
        q_put(&ecall_pcm_free_q, &buf_ptr->link);
      }
    }
  }
}


#if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
/*===========================================================================

     A - F a m i l y   &   B - F a m i l y   C o m m o n   F u n c t i o n

===========================================================================*/
/*==========================================================================
  FUNCTION ECALL_VOC_UNREGISTER

  DESCRIPTION
    This function will send a stop PCM session command thru APR interface
    to stop active vocoder PCM session.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B
 
  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_voc_unregister
(
  void
)
{
  int32 ret;
  ecall_pcm_buf_type *buf_ptr;

  if(ecall_psap_voc_registered == FALSE)
  {
    ECALL_MSG_0( ECALL_HIGH, "voc has already been deregistered" );
    return;
  }

  ecall_modem_psap_voc_restore();

  ret = __aprv2_cmd_alloc_send(
            ecall_modem_apr_handle,       /* handle       */
            APRV2_PKT_MSGTYPE_SEQCMD_V,   /* msg_type     */
            ecall_modem_apr_src_addr,     /* src_addr     */
            ecall_modem_apr_src_port,     /* src_port     */
            ecall_modem_apr_dest_addr,    /* dst_addr     */
            ecall_modem_apr_dest_port,    /* dst_port     */
            ECALL_APR_IVPCM_STOP,         /* token        */
            VSS_IVPCM_CMD_STOP,           /* opcode       */
            NULL,                         /* payload      */
            0 );                          /* payload_lize */
  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "__aprv2_cmd_alloc_send failed at ecall_voc_session_stop, ret %d",
                               ret );
  }

  ECALL_MSG_1( ECALL_MED, "ecall_pcm_q buffers  %d",
                           q_cnt(&ecall_pcm_q) );

  /* Move everything back to their free queues */
  while ((buf_ptr = (ecall_pcm_buf_type *) q_get(&ecall_pcm_q)) != NULL)
  {
    q_put(&ecall_pcm_free_q, &buf_ptr->link);
  }

} /* ECALL_VOC_UNREGISTER */

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
)
{
  int32 ret;

  ECALL_MSG_0( ECALL_MED, "Create a passive control session to get the dest port" );

  ret = __aprv2_cmd_alloc_send(
            ecall_modem_apr_handle,                         /* handle       */
            APRV2_PKT_MSGTYPE_SEQCMD_V,                     /* msg_type     */
            ecall_modem_apr_src_addr,                       /* src_addr     */
            ecall_modem_apr_src_port,                       /* src_port     */
            ecall_modem_apr_dest_addr,                      /* dst_addr     */
            APR_NULL_V,                                     /* dst_port     */
            ECALL_APR_CREATE_PASSIVE_CONTROL_SESSION,       /* token        */
            VSS_ISTREAM_CMD_CREATE_PASSIVE_CONTROL_SESSION, /* opcode       */
            ecall_modem_stream_name,                        /* payload      */
            sizeof(ecall_modem_stream_name));               /* payload_size */
  
  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "__aprv2_cmd_alloc_send(CREATE_PASSIVE_CONTROL_SESSION) failed, ret=%d",
                               ret );
  }

}

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
)
{
  aprv2_ibasic_rsp_result_t *rsp_result_ptr = NULL;
  uint32 status;

  /* Get the payload */
  if (packet == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "process_ibasic_rsp_result: packet is NULL" );
    return;
  }
  rsp_result_ptr = APRV2_PKT_GET_PAYLOAD(aprv2_ibasic_rsp_result_t, packet);
  status = APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, packet )->status;
  if(rsp_result_ptr == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "Failed to get payload at process_ibasic_rsp_result" );
  }
  
  switch (packet->token)
  {
    case ECALL_APR_MAP_MEMORY:
      if(status == APR_EOK)
      {
        ECALL_MSG_0( ECALL_HIGH, "Map memory process successfully" );
        ecall_modem_psap.smem_mapped = TRUE;
        ecall_voc_register();
      }
      else
      {
        ECALL_MSG_1( ECALL_ERROR, "Map memory process failed, error code %d",
                                   status );
      }
      __aprv2_cmd_free(ecall_modem_apr_handle, packet );
      break;

    case ECALL_APR_UNMAP_MEMORY:
      if(status == APR_EOK)
      {
        ECALL_MSG_0( ECALL_HIGH, "Unmap memory process successfully" );
        ecall_modem_psap.smem_mapped = FALSE;
      }
      else
      {
        ECALL_MSG_1( ECALL_ERROR, "Unmap memory process failed, error code %d",
                                   status );
      }
      __aprv2_cmd_free(ecall_modem_apr_handle, packet );
      break;

    case ECALL_APR_IVPCM_START:
      if(status == APR_EOK)
      {
        ECALL_MSG_0( ECALL_HIGH, "Successfully register with both RX/TX vocoder session" );
        ecall_psap_voc_registered = TRUE;
      }
      else
      {
        /* Let ecall clients know we failed to start the vocoder PCM session */
        ECALL_MSG_1( ECALL_ERROR, "Failed to register with both RX/TX vocoder session, error code %d",
                                   status );
      }
      __aprv2_cmd_free(ecall_modem_apr_handle, packet);
      break;

    case ECALL_APR_IVPCM_STOP:
      if(status == APR_EOK)
      {
        ECALL_MSG_0( ECALL_HIGH, "Successfully deregister with both RX/TX with vocoder" );
        ecall_psap_voc_registered = FALSE;
        ecall_modem_unmap_shared_memory();
      }
      else
      {
        /* Let ecall clients know we failed to end the vocoder PCM session */
        ECALL_MSG_1( ECALL_ERROR, "Failed to stop IVPCM session, error code %d",
                                   status );
      }
       __aprv2_cmd_free(ecall_modem_apr_handle, packet);
      break;

    case ECALL_APR_CREATE_PASSIVE_CONTROL_SESSION:
      if (status == APR_EOK)
      {
        ECALL_MSG_1( ECALL_HIGH, "Successfully create passive CVS control session, port %x",
                                  packet->src_port );
        ecall_modem_apr_dest_port = packet->src_port;
        ecall_voc_register();
      }
      else
      {
        ECALL_MSG_1( ECALL_ERROR, "Failed to create passive CVS control session, error code %d",
                                   status );
      }
      __aprv2_cmd_free(ecall_modem_apr_handle, packet);
      break;

    default:
      ECALL_MSG_1( ECALL_ERROR, "Received an unsupported IBASIC_RSP_RESULT %d",
                                 packet->token );
      __aprv2_cmd_end_command(ecall_modem_apr_handle, packet, APR_EUNSUPPORTED );
      break;
  }

} /* ECALL_PROCESS_IBASIC_RSP_RESULT */

#endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */

#ifdef FEATURE_ECALL_HAS_VOICE_A
/*===========================================================================

                     A - F a m i l y   F u n c t i o n

===========================================================================*/

/*==========================================================================
  FUNCTION ECALL_TX_PCM

  DESCRIPTION
    Pulls a PCM buffer off of the queue and copies it into the Vocoder's
    buffer for encoding and transmission.

  DEPENDENCIES
    FEATUR_ECALL_HAS_VOICE_A

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_tx_pcm
(
  aprv2_packet_t *packet
)
{
  int32 ret;
  vss_ivpcm_evt_notify_t *notify_evt_ptr;
  vss_ivpcm_evt_push_buffer_t push_buff;
  ecall_pcm_buf_type *buf_ptr;
  int16 pcm_temp[ECALL_PCM_LENGTH];
  int16 pcm_buffer[ECALL_PCM_LENGTH];
  uint32 i = 0;
  uint8 *tx_out_buf_ptr = NULL;
  uint8 *tx_in_buf_ptr = NULL;
  
  if(ecall_modem_psap.ready_to_deregister == TRUE )
  {
    if(ecall_modem_psap.pcm_tx_buf_cnt == 0)
    {
      ecall_modem_psap.ready_to_deregister = FALSE;
      ecall_psap_session_stop(TRUE);
    }
    ecall_modem_psap.pcm_tx_buf_cnt--;
  }

  ECALL_MSG_1( ECALL_MED, "ecall_tx_pcm count %d",
                           q_cnt(&ecall_pcm_q) );

  /* Get the payload */
  if (packet == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "ecall_tx_pcm: packet is NULL" );
    return;
  }
  notify_evt_ptr = APRV2_PKT_GET_PAYLOAD( vss_ivpcm_evt_notify_t, packet );
  if (notify_evt_ptr == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "tx_pcm(): Cannot get notify_evt payload out" );
    return;
  }

  if (notify_evt_ptr->request_buff_size != ECALL_PCM_LENGTH_BYTES)
  {
    ECALL_MSG_2( ECALL_ERROR, "tx_pcm: request_buff_size %d != ECALL_PCM_LENGTH_BYTES %d",
                               notify_evt_ptr->request_buff_size,
                               ECALL_PCM_LENGTH_BYTES );
    return;
  }
  push_buff.out_buf_size = notify_evt_ptr->request_buff_size;
  push_buff.in_buf_size  = notify_evt_ptr->request_buff_size;

  push_buff.tap_point       = VSS_IVPCM_TAP_POINT_TX_DEFAULT;  /* Tap Point */
  push_buff.sampling_rate   = notify_evt_ptr->sampling_rate;   /* Sampling Rate, VSS_IVPCM_SAMPLING_RATE_8K */
  push_buff.push_buf_mask   = 2;                               /* Push_buf_mask */
  push_buff.num_in_channels = 1;                               /* Num_in_channels */
  
  push_buff.out_buf_addr = ecall_modem_psap.ecall_tx_out_buf_phy_addr;
  push_buff.in_buf_addr = ecall_modem_psap.ecall_tx_in_buf_phy_addr;
  tx_out_buf_ptr = (uint8*)ecall_modem_psap.ecall_tx_out_buf_addr;
  tx_in_buf_ptr = (uint8*)ecall_modem_psap.ecall_tx_in_buf_addr;

  if (tx_in_buf_ptr == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "tx_pcm: tx_in_buff_ptr = NULL" );
    return;
  }

  if ((buf_ptr = (ecall_pcm_buf_type *) q_get(&ecall_pcm_q)) != NULL)
  {
    for (i = 0; i < ECALL_PCM_LENGTH; i++)
    {
      pcm_buffer[i] = (buf_ptr->pcm[i]>>2);
    }
    ecall_memscpy ( tx_in_buf_ptr, notify_evt_ptr->request_buff_size,
                    pcm_buffer, sizeof(pcm_buffer) );
    q_put(&ecall_pcm_free_q, &buf_ptr->link);
  }
  else if(psap.rx.state != PsapIdle)
  {
    ECALL_MSG_0( ECALL_HIGH, "ecall_tx_pcm no data, force to get data" );
   
    ecall_psap_rx_downlink();
    
    ecall_psap_tx_process(pcm_temp, sizeof(pcm_temp));

    for (i=0;i < ECALL_PCM_LENGTH;i++) 
    {
      pcm_buffer[i] = (pcm_temp[i]>>2);
    }
    ecall_memscpy ( tx_in_buf_ptr, notify_evt_ptr->request_buff_size,
                    pcm_buffer, sizeof(pcm_buffer) );
  }
  else
  {
    ECALL_MSG_0( ECALL_MED, "sending silence" );
    memset(tx_in_buf_ptr, 0, ECALL_PCM_LENGTH_BYTES);
  }

  ret = __aprv2_cmd_alloc_send(
            ecall_modem_apr_handle,              /* hanlde       */
            APRV2_PKT_MSGTYPE_EVENT_V,           /* msg_type     */
            ecall_modem_apr_src_addr,            /* src_addr     */
            ecall_modem_apr_src_port,            /* src_port     */
            ecall_modem_apr_dest_addr,           /* dst_addr     */
            ecall_modem_apr_dest_port,           /* dst_port     */
            ECALL_APR_PUSH_BUFFER,               /* token        */
            VSS_IVPCM_EVT_PUSH_BUFFER,           /* opcode       */
            &push_buff,                          /* payload      */
            sizeof(push_buff));                  /* payload_size */
  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "__aprv2_cmd_alloc_send failed, ret %d",
                               ret );
  }

  __aprv2_cmd_free(ecall_modem_apr_handle, packet);

} /* ECALL_TX_PCM */

/*==========================================================================
  FUNCTION ECALL_RX_PCM

  DESCRIPTION
    This function will copy the input data from packet, and post a command to
    process this PCM frame

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_rx_pcm
(
  aprv2_packet_t *packet
)
{
  uint8 i;
  int32 ret;
  int16 pcm_buffer[ECALL_PCM_LENGTH];
  uint8 *rx_in_buf_ptr = NULL;
  uint8 *rx_out_buf_ptr = NULL;
  ecall_cmd_type *cmd_ptr = NULL;
  vss_ivpcm_evt_notify_t *notify_evt_ptr;
  vss_ivpcm_evt_push_buffer_t push_buff;

  /* Get the payload */
  if (packet == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "ecall_rx_pcm: packet is NULL" );
    return;
  }
  notify_evt_ptr = APRV2_PKT_GET_PAYLOAD( vss_ivpcm_evt_notify_t, packet );
  if (notify_evt_ptr == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "rx_pcm(): Cannot get notify_evt payload out" );
    return;
  }

  if (notify_evt_ptr->request_buff_size != ECALL_PCM_LENGTH_BYTES)
  {
    ECALL_MSG_2( ECALL_ERROR, "request_buff_size %d is not equal to ECALL_PCM_LENGTH_BYTES %d",
                               notify_evt_ptr->request_buff_size,
                               ECALL_PCM_LENGTH_BYTES );
    return;
  }

  push_buff.out_buf_size = ECALL_PCM_LENGTH_BYTES;
  push_buff.in_buf_size  = ECALL_PCM_LENGTH_BYTES;

  push_buff.tap_point       = VSS_IVPCM_TAP_POINT_RX_DEFAULT;  /* Tap Point */
  push_buff.sampling_rate   = notify_evt_ptr->sampling_rate;   /* Sampling Rate, VSS_IVPCM_SAMPLING_RATE_8K */
  push_buff.push_buf_mask   = 1;                               /* Push_buf_mask */
  push_buff.num_in_channels = 1;                               /* Num_in_channels */

  push_buff.out_buf_addr = ecall_modem_psap.ecall_rx_out_buf_phy_addr;
  push_buff.in_buf_addr = ecall_modem_psap.ecall_rx_in_buf_phy_addr;
  rx_out_buf_ptr = (uint8*)ecall_modem_psap.ecall_rx_out_buf_addr;
  rx_in_buf_ptr = (uint8*)ecall_modem_psap.ecall_rx_in_buf_addr; 

  cmd_ptr = ecall_get_cmd_buf();
  if (cmd_ptr != NULL)
  {
    /* Post a command to the eCall task to process this PCM frame */
    cmd_ptr->cmd_id = ECALL_CMD_PROCESS_RX_PCM;

    if ( ((notify_evt_ptr->notify_mask & 0x01) == 0) && ((notify_evt_ptr->notify_mask & 0x02) == 0) )
    {
      ECALL_MSG_0( ECALL_MED, "rx_pcm: notify_mask is 0, feed dummy pcm to process" );
      for (i = 0; i < ECALL_PCM_LENGTH; i++)
      {
        cmd_ptr->data.pcm[i] = 0;
      }
    }
    else
    {
      ecall_memscpy ( pcm_buffer, sizeof(pcm_buffer),
                      rx_out_buf_ptr, notify_evt_ptr->request_buff_size );
      for (i = 0; i < ECALL_PCM_LENGTH; i++)
      { 
        /* The vocoder output gives 14 bit linear pcm samples and the 15th,16th bits are the 
          sign extension bits. we need to up-scale two bits to get the actual data*/
        cmd_ptr->data.pcm[i] = (pcm_buffer[i] << 2);
      }
    }

    ecall_put_cmd(cmd_ptr);
  }
  else
  {  
    ECALL_MSG_0( ECALL_ERROR, "ecall_get_cmd_buf() ran out of emodem command buffers at ecall_rx_pcm" );
    return;
  }

  ret = __aprv2_cmd_alloc_send(
            ecall_modem_apr_handle,              /* handle       */
            APRV2_PKT_MSGTYPE_EVENT_V,           /* msg_type     */
            ecall_modem_apr_src_addr,            /* src_addr     */
            ecall_modem_apr_src_port,            /* src_port     */
            ecall_modem_apr_dest_addr,           /* dst_addr     */
            ecall_modem_apr_dest_port,           /* dst_port     */
            ECALL_APR_PUSH_BUFFER,               /* token        */
            VSS_IVPCM_EVT_PUSH_BUFFER,           /* opcode       */
            &push_buff,                          /* payload      */
            sizeof(push_buff));                  /* payload_size */
  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "__aprv2_cmd_alloc_send failed, ret %d",
                               ret );
  }
  __aprv2_cmd_free(ecall_modem_apr_handle, packet);

} /* ECALL_RX_PCM */

/*==========================================================================
  FUNCTION ECALL_VOC_REGISTER

  DESCRIPTION
    This function will send a start PCM session command thru APR interface
    to start a vocoder PCM session with RX/TX mode.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_voc_register
(
  void
)
{
  int32 ret;
  vss_ivpcm_tap_point_t vss_tap_point_rx;
  vss_ivpcm_tap_point_t vss_tap_point_tx;
  uint32 target_payload_len;
  uint32 remaining_target_payload_len;
  uint32 num_tap_points;
  aprv2_packet_t *dst_packet = NULL;
  uint8_t *dst_payload;

  int16 pcm_temp[ECALL_PCM_LENGTH];
  ecall_pcm_buf_type *buf_ptr;

  if (ecall_modem_apr_dest_port == 0)
  {
    /* Initialize CVS stream to get the dest port
       will call ecall_modem_map_shared_memory after successfully created cvs session */
    ECALL_MSG_1( ECALL_MED, "voc_reg: dest_port is 0(%d). Will create passive control session to get port first",
                             ecall_modem_apr_dest_port );

    ecall_modem_apr_init();
    ecall_modem_cvs_init();
    return;
  }

  if (ecall_modem_psap.smem_mapped == FALSE)
  {
    ECALL_MSG_0( ECALL_MED, "voc_reg: memory not mapped. Will register after successfully mapped" );
    ecall_modem_map_shared_memory();
    return;
  }

  if (ecall_psap_voc_registered  == TRUE )
  {
    ECALL_MSG_0( ECALL_HIGH, "vocoder already registered !!" );
    return;
  }

  num_tap_points = 2; /* 2 tap point for TX and RX */
  target_payload_len = sizeof(num_tap_points) + num_tap_points*sizeof(vss_ivpcm_tap_point_t);
  remaining_target_payload_len = target_payload_len;

  /* Allocate the dest_packet */
  ret = __aprv2_cmd_alloc_ext(
            ecall_modem_apr_handle,         /* handle       */
            APRV2_PKT_MSGTYPE_SEQCMD_V,     /* msg_type     */
            ecall_modem_apr_src_addr,       /* src_addr     */
            ecall_modem_apr_src_port,       /* src_port     */
            ecall_modem_apr_dest_addr,      /* dst_addr     */
            ecall_modem_apr_dest_port,      /* dst_port     */
            ECALL_APR_IVPCM_START,    /* token        */
            VSS_IVPCM_CMD_START,            /* opcode       */
            target_payload_len,             /* payload_size */
            &dst_packet );                  /* ret_packet   */
  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "__aprv2_cmd_alloc_send failed, ret %d",
                               ret );
  }

  if (dst_packet == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "ecall_voc_register: dst_packet is NULL" );
    return;
  }
  dst_payload = APRV2_PKT_GET_PAYLOAD(uint8_t, dst_packet);

  /* copy num_tap_points to dst_payload */
  ecall_memscpy ( dst_payload, remaining_target_payload_len,
                  &num_tap_points, sizeof(num_tap_points) );
  dst_payload += sizeof(num_tap_points);
  remaining_target_payload_len -= sizeof(num_tap_points);

  /* Configure TX tap point */
  vss_tap_point_tx.tap_point = VSS_IVPCM_TAP_POINT_TX_DEFAULT;
  vss_tap_point_tx.direction = 1; /* input-output tap point, VSS_IVPCM_TAP_POINT_DIR_OUT_IN */
  vss_tap_point_tx.sampling_rate = VSS_IVPCM_SAMPLING_RATE_8K; 
  vss_tap_point_tx.duration = 0;

  /* copy TX tap point structure to destination payload */
  ecall_memscpy ( dst_payload, remaining_target_payload_len,
                  &vss_tap_point_tx, sizeof(vss_tap_point_tx) );
  dst_payload += sizeof( vss_tap_point_tx );
  remaining_target_payload_len -= sizeof(vss_tap_point_tx);

  /* Configure RX tap point */
  vss_tap_point_rx.tap_point = VSS_IVPCM_TAP_POINT_RX_DEFAULT;
  vss_tap_point_rx.direction = 0; /* output tap point, VSS_IVPCM_TAP_POINT_DIR_OUT */
  vss_tap_point_rx.sampling_rate = VSS_IVPCM_SAMPLING_RATE_8K; 
  vss_tap_point_rx.duration = 0;

  /* copy RX tap point structure to destination payload */
  ecall_memscpy ( dst_payload, remaining_target_payload_len,
                  &vss_tap_point_rx, sizeof(vss_tap_point_rx) );
  dst_payload += sizeof( vss_tap_point_rx );

  /* Send the IVPCM_CMD_START command */
  ret = __aprv2_cmd_forward( 
            ecall_modem_apr_handle,  /* handle */
            dst_packet);             /* packet */

  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "__aprv2_cmd_alloc_send failed, ret %d",
                               ret );
  }


  ECALL_MSG_1( ECALL_HIGH, "ecall_pcm_q buffers  %d",
                            q_cnt(&ecall_pcm_q) );

  ECALL_MSG_0( ECALL_HIGH, "PREFILL a buffer so that we dont have to force to get data" );
  memset(pcm_temp, 0, sizeof(int16)*ECALL_PCM_LENGTH);

  if ((buf_ptr = (ecall_pcm_buf_type *)q_get(&ecall_pcm_free_q)) != NULL)
  {
    ecall_memscpy ( buf_ptr->pcm, sizeof(buf_ptr->pcm),
                    pcm_temp,  sizeof(pcm_temp) );
  }

  q_put(&ecall_pcm_q, &buf_ptr->link);

} /* ECALL_VOC_REGISTER */

/*===========================================================================

  FUNCTION ECALL_MODEM_APR_ISR_CB

  DESCRIPTION
    This function will called whenever an APR ISR happens. Will copy everything
    and put into ecall_modem queue to process.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static int32 ecall_modem_apr_isr_cb 
(
  aprv2_packet_t* packet,
  void* dispatch_data
)
{
  ecall_cmd_type *cmd_ptr = NULL;
  int32 ret;
  vss_ivpcm_evt_notify_t* notify_evt;

  if (packet == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "ecall_modem_apr_isr_cb: packet is NULL" );
    return APR_EFAILED;
  }
  if(packet->opcode == VSS_IVPCM_EVT_NOTIFY)
  {
    notify_evt = APRV2_PKT_GET_PAYLOAD( vss_ivpcm_evt_notify_t, packet );
    if (notify_evt == NULL)
    {
      ECALL_MSG_0( ECALL_ERROR, "apr_isr_cb: Cannot get notify_evt payload out" );
      return APR_EFAILED;
    }

    if (notify_evt->tap_point == VSS_IVPCM_TAP_POINT_RX_DEFAULT)
    {
      ecall_modem_psap.rx_evt_notify_cnt++;
    }
    else if (notify_evt->tap_point == VSS_IVPCM_TAP_POINT_TX_DEFAULT)
    {
      ecall_modem_psap.tx_evt_notify_cnt++;
    }
    else
    {
      ECALL_MSG_0( ECALL_ERROR, "apr_isr_cb: unsupported tap point" );
    }
  }

  cmd_ptr = ecall_get_cmd_buf();
  if (cmd_ptr == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "ecall_get_cmd_buf() ran out of cmd buffer at ecall_modem_apr_isr_cb" );
    return APR_EFAILED;
  }
  else
  {
    /* Post a command to the eCall task to process this APR packet */
    cmd_ptr->cmd_id = ECALL_CMD_APR_PACKET;

    /* Copy all APR packet info and process at ecall_ivs_task */
    cmd_ptr->data.apr_packet = packet;
    
    ecall_put_cmd(cmd_ptr);
  }

  ret = __aprv2_cmd_accept_command(ecall_modem_apr_handle,packet);
  if(ret != APR_EOK)
  {
    ECALL_MSG_0( ECALL_ERROR, "__aprv2_cmd_accept_command failed at ecall_modem_apr_isr_cb" );
    return APR_EFAILED;
  }

  return APR_EOK;

} /* ECALL_MODEM_APR_ISR_CB */

/*==========================================================================
  FUNCTION ECALL_MODEM_MAP_SHARED_MEMORY

  DESCRIPTION
    This function will allocate a memory pool for APR interface to process
    the vocoder input/output PCM buffers.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_modem_map_shared_memory
(
  void
)
{
  int32 ret;
  void *ecall_smem_origin_ptr;
  void *ecall_smem_get_ptr;
  uint32 ecall_smem_size = 0;
  vss_icommon_cmd_map_memory_t mem_map_cmd;

  if (ecall_modem_psap.smem_mapped == TRUE)
  {
    ECALL_MSG_0( ECALL_ERROR, "map_apr_memory() called but was already mapped!" );
    return;
  }

  if (ecall_modem_psap.smem_allocated == FALSE)
  {
    ecall_smem_origin_ptr = smem_alloc(SMEM_ECALL, ECALL_SMEM_SIZE);
    if (ecall_smem_origin_ptr == NULL)
    {
      ECALL_MSG_0( ECALL_ERROR, "smem_alloc failed!" );
      return;
    }

    ecall_smem_virtual_addr_orig = (uint32)ecall_smem_origin_ptr;
    ecall_smem_physical_addr_orig = blast_lookup_physaddr(ecall_smem_virtual_addr_orig);
    ECALL_MSG_2( ECALL_HIGH,  "virt_addr_orig: 0x%08X, phys_addr_orig: 0x%08X", 
                               ecall_smem_virtual_addr_orig,
                               ecall_smem_physical_addr_orig );
    

    ecall_smem_get_ptr = smem_get_addr(SMEM_ECALL, &ecall_smem_size);
    ECALL_MSG_2( ECALL_HIGH,  "ecall_smem_get_ptr: 0x%08X, ecall_smem_size: %d",
                               ecall_smem_get_ptr,
                               ecall_smem_size );
    

    ecall_modem_psap.smem_allocated = TRUE;
  }

  ecall_smem_virtual_addr = ecall_smem_virtual_addr_orig + (ECALL_SMEM_SIZE - (ecall_smem_virtual_addr_orig % ECALL_SMEM_SIZE) );
  ecall_smem_physical_addr = ecall_smem_physical_addr_orig + (ECALL_SMEM_SIZE - (ecall_smem_physical_addr_orig % ECALL_SMEM_SIZE) );

  ECALL_MSG_2( ECALL_HIGH,  "virt_addr: 0x%08X, phys_addr: 0x%08X",
                             ecall_smem_virtual_addr,
                             ecall_smem_physical_addr );

  /* Initialize the mem_map object */
  mem_map_cmd.phys_addr = ecall_smem_physical_addr;
  mem_map_cmd.mem_size = ECALL_SMEM_SIZE*2;
  mem_map_cmd.mem_pool_id = VSS_ICOMMON_MAP_MEMORY_SHMEM8_4K_POOL;

  ECALL_MSG_3( ECALL_MED, "mem_map_cmd: phys_addr 0x%08X, mem_size %d, mem_pool_id %d",
                           mem_map_cmd.phys_addr,
                           mem_map_cmd.mem_size,
                           mem_map_cmd.mem_pool_id );
  
  memset( (uint8*)ecall_smem_virtual_addr, 0, 4096 );

  ecall_modem_psap.ecall_tx_out_buf_addr = ecall_smem_virtual_addr;
  ecall_modem_psap.ecall_tx_in_buf_addr = ecall_smem_virtual_addr + 1000; 
  ecall_modem_psap.ecall_rx_out_buf_addr = ecall_smem_virtual_addr + 2000; 
  ecall_modem_psap.ecall_rx_in_buf_addr = ecall_smem_virtual_addr + 3000; 

  ECALL_MSG_2( ECALL_MED, "ecall_tx_out_buf_addr: 0x%08X, tx_in_buf_addr: 0x%08X",
                           ecall_modem_psap.ecall_tx_out_buf_addr,
                           ecall_modem_psap.ecall_tx_in_buf_addr );
  ECALL_MSG_2( ECALL_MED, "ecall_rx_out_buf_addr: 0x%08X, rx_in_buf_addr: 0x%08X",
                           ecall_modem_psap.ecall_rx_out_buf_addr,
                           ecall_modem_psap.ecall_rx_in_buf_addr );

  ecall_modem_psap.ecall_tx_out_buf_phy_addr = ecall_smem_physical_addr;
  ecall_modem_psap.ecall_tx_in_buf_phy_addr = ecall_smem_physical_addr + 1000; 
  ecall_modem_psap.ecall_rx_out_buf_phy_addr = ecall_smem_physical_addr + 2000; 
  ecall_modem_psap.ecall_rx_in_buf_phy_addr = ecall_smem_physical_addr + 3000; 

  ECALL_MSG_2( ECALL_MED, "ecall_tx_out_buf_phy_addr: 0x%08X, tx_in_buf_phy_addr: 0x%08X",
                           ecall_modem_psap.ecall_tx_out_buf_phy_addr,
                           ecall_modem_psap.ecall_tx_in_buf_phy_addr );
  ECALL_MSG_2( ECALL_MED, "ecall_rx_out_buf_phy_addr: 0x%08X, rx_in_buf_phy_addr: 0x%08X",
                           ecall_modem_psap.ecall_rx_out_buf_phy_addr,
                           ecall_modem_psap.ecall_rx_in_buf_phy_addr );

  ret = __aprv2_cmd_alloc_send(
            ecall_modem_apr_handle,               /* handle       */
            APRV2_PKT_MSGTYPE_SEQCMD_V,           /* msg_type     */
            ecall_modem_apr_src_addr,             /* src_addr     */
            ecall_modem_apr_src_port,             /* src_port     */
            ecall_modem_apr_dest_addr,            /* dst_addr     */
            ecall_modem_apr_dest_port,            /* dst_port     */                       
            ECALL_APR_MAP_MEMORY,                 /* token        */
            VSS_ICOMMON_CMD_MAP_MEMORY,           /* opcode       */
            &mem_map_cmd,                         /* payload      */
            sizeof(mem_map_cmd) );                /* payload_size */

  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "__aprv2_cmd_alloc_send failed, ret %d",
                               ret );
  }

} /* ECALL_MODEM_MAP_SHARED_MEMORY */

/*==========================================================================
  FUNCTION ECALL_MODEM_UNMAP_SHARED_MEMORY

  DESCRIPTION
    This function will unmap the memory pool for APR interface.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_modem_unmap_shared_memory
(
  void
)
{
  vss_icommon_cmd_unmap_memory_t mem_unmap_cmd;
  int32 ret;
 
  if (ecall_modem_psap.smem_mapped == FALSE)
  {
    ECALL_MSG_0( ECALL_ERROR, "unmap_apr_memory() called but was already unmapped" );
    return;
  }
  else if (ecall_psap_voc_registered == TRUE)
  {
    ECALL_MSG_0( ECALL_ERROR, "unmap_apr_memory() called but still registered for vocoder" );
    return;
  }
   
  mem_unmap_cmd.phys_addr = ecall_smem_physical_addr;
  
  ret = __aprv2_cmd_alloc_send(
            ecall_modem_apr_handle,                /* handle       */
            APRV2_PKT_MSGTYPE_SEQCMD_V,            /* msg_type     */
            ecall_modem_apr_src_addr,              /* src_addr     */
            ecall_modem_apr_src_port,              /* src_port     */
            ecall_modem_apr_dest_addr,             /* dst_addr     */
            ecall_modem_apr_dest_port,             /* dst_port     */
            ECALL_APR_UNMAP_MEMORY,                /* token        */
            VSS_ICOMMON_CMD_UNMAP_MEMORY,          /* opcode       */
            &mem_unmap_cmd,                        /* payload      */
            sizeof(mem_unmap_cmd) );               /* payload_size */

  /* Reset the ecall rx and tx buf addr to NULL */
   ecall_modem_psap.ecall_rx_in_buf_addr      = 0;
   ecall_modem_psap.ecall_rx_out_buf_addr     = 0;
   ecall_modem_psap.ecall_tx_in_buf_addr      = 0;
   ecall_modem_psap.ecall_tx_out_buf_addr     = 0;
   ecall_modem_psap.ecall_tx_in_buf_phy_addr  = 0;
   ecall_modem_psap.ecall_tx_out_buf_phy_addr = 0;
   ecall_modem_psap.ecall_rx_in_buf_phy_addr  = 0;
   ecall_modem_psap.ecall_rx_out_buf_phy_addr = 0;

   /* Reset smem_mapped flag */
   ecall_modem_psap.smem_mapped = FALSE;

  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "__aprv2_cmd_alloc_send failed, ret %d",
                               ret );
  }

} /* ECALL_MODEM_UNMAP_SHARED_MEMORY */

/*===========================================================================

  FUNCTION ECALL_MODEM_APR_INIT

  DESCRIPTION
    This function will initialize the APR interface for eCall

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_modem_apr_init 
( 
  void
)
{
  int32 ret;
  /* Register with APR to receive messages. */
  ret = __aprv2_cmd_register2(
            &ecall_modem_apr_handle,            /* <- Return eCall handle */
            ecall_modem_psap_ecall_dns,         /* -> Name string of the eCall service */
            sizeof(ecall_modem_psap_ecall_dns), /* -> Size of the name string, including NULL */
            0,                                  /* -> Service address to register */
            ecall_modem_apr_isr_cb,             /* -> ISR dispatch callback to register */
            NULL,                               /* -> Static data to pass to the ISR callback */
            &ecall_modem_apr_src_addr);         /* <- Return pointer to the registered service address */
  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "ecall_modem_apr_init: __aprv2_cmd_register2 failed, ret %d",
                               ret );
  }

  ret = __aprv2_cmd_local_dns_lookup(
           ecall_modem_psap_voc_dns, 
           sizeof(ecall_modem_psap_voc_dns), 
           &ecall_modem_apr_dest_addr);
  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "ecall_modem_apr_init: __aprv2_cmd_local_dns_lookup failed, ret %d",
                               ret );
  }

} /* ECALL_MODEM_APR_INIT */

/*==========================================================================
  FUNCTION ECALL_PROCESS_APR_PACKET

  DESCRIPTION
    This function will process the incoming APR packet.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_process_apr_packet
(
  aprv2_packet_t *packet
)
{
  
  switch (packet->opcode)
  {
    case APRV2_IBASIC_EVT_ACCEPTED:
      __aprv2_cmd_free(ecall_modem_apr_handle, packet );
      break;

    case APRV2_IBASIC_RSP_RESULT:
      /* Will free the cmd in the function */
      ecall_process_ibasic_rsp_result(packet);
      break;

    case VSS_IVPCM_EVT_NOTIFY:
      ecall_process_ivpcm_evt_notify(packet);
      break;

    default:
      ECALL_MSG_1( ECALL_ERROR, "Received an unsupported command %d",
                                 packet->opcode );
      __aprv2_cmd_end_command(ecall_modem_apr_handle, packet, APR_EUNSUPPORTED );
      break;
  }

} /* ECALL_PROCESS_APR_PACKET */

/*==========================================================================
  FUNCTION ECALL_PROCESS_IVPCM_EVT_NOTIFY

  DESCRIPTION
    This function will process the incoming IVPCM_EVT_NOTIFY every 20ms and
    start the RX/TX PCM process.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_process_ivpcm_evt_notify
(
  aprv2_packet_t* packet
)
{
  vss_ivpcm_evt_notify_t* notify_evt;

  if (packet == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "process_evt_notify: packet is NULL" );
    return;
  }
  notify_evt = APRV2_PKT_GET_PAYLOAD( vss_ivpcm_evt_notify_t, packet );
  if (notify_evt == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "process_evt_notify: Cannot get notify_evt payload out" );
    return;
  }

  if (notify_evt->tap_point == VSS_IVPCM_TAP_POINT_RX_DEFAULT)
  {
    ecall_modem_psap.rx_process_cnt++;
    if (ecall_modem_psap.rx_evt_notify_cnt != ecall_modem_psap.rx_process_cnt)
    {
      ECALL_MSG_2( ECALL_ERROR, "process_evt_notify: rx_evt_notify_cnt %d != rx_process_cnt %d, drop this frame",
                                 ecall_modem_psap.rx_evt_notify_cnt,
                                 ecall_modem_psap.rx_process_cnt );
      __aprv2_cmd_free(ecall_modem_apr_handle, packet);
      return;
    }
    ecall_rx_pcm(packet);
  }
  else if (notify_evt->tap_point == VSS_IVPCM_TAP_POINT_TX_DEFAULT)
  {
    ecall_modem_psap.tx_process_cnt++;
    if (ecall_modem_psap.tx_evt_notify_cnt != ecall_modem_psap.tx_process_cnt)
    {
      ECALL_MSG_2( ECALL_ERROR, "process_evt_notify: tx_evt_notify_cnt %d != tx_process_cnt %d, drop this frame",
                                 ecall_modem_psap.tx_evt_notify_cnt,
                                 ecall_modem_psap.tx_process_cnt );
      __aprv2_cmd_free(ecall_modem_apr_handle, packet);
      return;
    }
    ecall_tx_pcm(packet);
  }
  else
  {
    ECALL_MSG_1( ECALL_ERROR, "process_evt_notify: unsupported tap point %d",
                               notify_evt->tap_point );
  }

} /* ECALL_PROCESS_IVPCM_EVT_NOTIFY */

#elif ( defined FEATURE_ECALL_HAS_VOICE_B )
/*===========================================================================

                     B - F a m i l y   F u n c t i o n

===========================================================================*/
/*==========================================================================
  FUNCTION ECALL_TX_PCM

  DESCRIPTION
    Pulls a PCM buffer off of the queue and copies it into the Vocoder's
    buffer for encoding and transmission.

  DEPENDENCIES
    FEATUR_ECALL_HAS_VOICE_B

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_tx_pcm
(
  aprv2_packet_t *packet
)
{
  int32 ret;
  vss_ivpcm_evt_notify_v2_t *notify_evt_ptr;
  vss_ivpcm_evt_push_buffer_v2_t push_buff;
  ecall_pcm_buf_type *buf_ptr;
  int16 pcm_temp[ECALL_PCM_LENGTH];
  int16 pcm_buffer[ECALL_PCM_LENGTH];
  uint32 i = 0;
  uint8 *tx_out_buf_ptr = NULL;
  uint8 *tx_in_buf_ptr = NULL;
  
  if(ecall_modem_psap.ready_to_deregister == TRUE )
  {
    if(ecall_modem_psap.pcm_tx_buf_cnt == 0)
    {
      ecall_modem_psap.ready_to_deregister = FALSE;
      ecall_psap_session_stop(TRUE);
    }
    ecall_modem_psap.pcm_tx_buf_cnt--;
  }

  ECALL_MSG_1( ECALL_MED, "ecall_tx_pcm count %d",
                           q_cnt(&ecall_pcm_q) );

  /* Get the payload */
  if (packet == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "ecall_tx_pcm: packet is NULL" );
    return;
  }
  notify_evt_ptr = APRV2_PKT_GET_PAYLOAD( vss_ivpcm_evt_notify_v2_t, packet );
  if (notify_evt_ptr == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "tx_pcm(): Cannot get notify_evt payload out" );
    return;
  }

  if (notify_evt_ptr->request_buf_size != ECALL_PCM_LENGTH_BYTES)
  {
    ECALL_MSG_2( ECALL_ERROR, "tx_pcm: request_buf_size %d != ECALL_PCM_LENGTH_BYTES %d",
                               notify_evt_ptr->request_buf_size,
                               ECALL_PCM_LENGTH_BYTES );
    return;
  }
  push_buff.out_buf_mem_size = notify_evt_ptr->request_buf_size;
  push_buff.in_buf_mem_size  = notify_evt_ptr->request_buf_size;

  push_buff.tap_point       = VSS_IVPCM_TAP_POINT_TX_DEFAULT;  /* Tap Point */
  push_buff.sampling_rate   = notify_evt_ptr->sampling_rate;   /* Sampling Rate, VSS_IVPCM_SAMPLING_RATE_8K */
  push_buff.push_buf_mask   = 2;                               /* Push_buf_mask */
  push_buff.num_in_channels = 1;                               /* Num_in_channels */
  
  push_buff.out_buf_mem_address = ecall_modem_psap.ecall_tx_out_buf_phy_addr;
  push_buff.in_buf_mem_address = ecall_modem_psap.ecall_tx_in_buf_phy_addr;
  tx_out_buf_ptr = (uint8*)ecall_modem_psap.ecall_tx_out_buf_addr;
  tx_in_buf_ptr = (uint8*)ecall_modem_psap.ecall_tx_in_buf_addr;

  if (tx_in_buf_ptr == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "tx_pcm: tx_in_buff_ptr = NULL" );
    return;
  }

  if ((buf_ptr = (ecall_pcm_buf_type *) q_get(&ecall_pcm_q)) != NULL)
  {
    for (i = 0; i < ECALL_PCM_LENGTH; i++)
    {
      pcm_buffer[i] = (buf_ptr->pcm[i]>>2);
    }
    ecall_memscpy ( tx_in_buf_ptr, notify_evt_ptr->request_buf_size,
                    pcm_buffer, sizeof(pcm_buffer) );
    q_put(&ecall_pcm_free_q, &buf_ptr->link);
  }
  else if(psap.rx.state != PsapIdle)
  {
    ECALL_MSG_0( ECALL_HIGH, "ecall_tx_pcm no data, force to get data" );
   
    ecall_psap_rx_downlink();
    
    ecall_psap_tx_process(pcm_temp, sizeof(pcm_temp));

    for (i=0;i < ECALL_PCM_LENGTH;i++) 
    {
      pcm_buffer[i] = (pcm_temp[i]>>2);
    }
    ecall_memscpy ( tx_in_buf_ptr, notify_evt_ptr->request_buf_size,
                    pcm_buffer, sizeof(pcm_buffer) );
  }
  else
  {
    ECALL_MSG_0( ECALL_MED, "sending silence" );
    memset(tx_in_buf_ptr, 0, ECALL_PCM_LENGTH_BYTES);
  }

  ret = __aprv2_cmd_alloc_send(
            ecall_modem_apr_handle,              /* hanlde       */
            APRV2_PKT_MSGTYPE_EVENT_V,           /* msg_type     */
            ecall_modem_apr_src_addr,            /* src_addr     */
            ecall_modem_apr_src_port,            /* src_port     */
            ecall_modem_apr_dest_addr,           /* dst_addr     */
            ecall_modem_apr_dest_port,           /* dst_port     */
            ECALL_APR_PUSH_BUFFER,               /* token        */
            VSS_IVPCM_EVT_PUSH_BUFFER_V2,           /* opcode       */
            &push_buff,                          /* payload      */
            sizeof(push_buff));                  /* payload_size */
  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "__aprv2_cmd_alloc_send failed, ret %d",
                               ret );
  }

  __aprv2_cmd_free(ecall_modem_apr_handle, packet);

} /* ECALL_TX_PCM */

/*==========================================================================
  FUNCTION ECALL_RX_PCM

  DESCRIPTION
    This function will copy the input data from packet, and post a command to
    process this PCM frame

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_rx_pcm
(
  aprv2_packet_t *packet
)
{
  uint8 i;
  int32 ret;
  int16 pcm_buffer[ECALL_PCM_LENGTH];
  uint8 *rx_in_buf_ptr = NULL;
  uint8 *rx_out_buf_ptr = NULL;
  ecall_cmd_type *cmd_ptr = NULL;
  vss_ivpcm_evt_notify_v2_t *notify_evt_ptr;
  vss_ivpcm_evt_push_buffer_v2_t push_buff;

  /* Get the payload */
  if (packet == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "ecall_rx_pcm: packet is NULL" );
    return;
  }
  notify_evt_ptr = APRV2_PKT_GET_PAYLOAD( vss_ivpcm_evt_notify_v2_t, packet );
  if (notify_evt_ptr == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "rx_pcm(): Cannot get notify_evt payload out" );
    return;
  }

  if (notify_evt_ptr->request_buf_size != ECALL_PCM_LENGTH_BYTES)
  {
    ECALL_MSG_2( ECALL_ERROR, "request_buf_size %d is not equal to ECALL_PCM_LENGTH_BYTES %d",
                               notify_evt_ptr->request_buf_size,
                               ECALL_PCM_LENGTH_BYTES );
    return;
  }

  push_buff.out_buf_mem_size = ECALL_PCM_LENGTH_BYTES;
  push_buff.in_buf_mem_size  = ECALL_PCM_LENGTH_BYTES;

  push_buff.tap_point       = VSS_IVPCM_TAP_POINT_RX_DEFAULT;  /* Tap Point */
  push_buff.sampling_rate   = notify_evt_ptr->sampling_rate;   /* Sampling Rate, VSS_IVPCM_SAMPLING_RATE_8K */
  push_buff.push_buf_mask   = 1;                               /* Push_buf_mask */
  push_buff.num_in_channels = 1;                               /* Num_in_channels */

  push_buff.out_buf_mem_address = ecall_modem_psap.ecall_rx_out_buf_phy_addr;
  push_buff.in_buf_mem_address = ecall_modem_psap.ecall_rx_in_buf_phy_addr;
  rx_out_buf_ptr = (uint8*)ecall_modem_psap.ecall_rx_out_buf_addr;
  rx_in_buf_ptr = (uint8*)ecall_modem_psap.ecall_rx_in_buf_addr; 

  cmd_ptr = ecall_get_cmd_buf();
  if (cmd_ptr != NULL)
  {
    /* Post a command to the eCall task to process this PCM frame */
    cmd_ptr->cmd_id = ECALL_CMD_PROCESS_RX_PCM;

    if ( ((notify_evt_ptr->notify_mask & 0x01) == 0) && ((notify_evt_ptr->notify_mask & 0x02) == 0) )
    {
      ECALL_MSG_0( ECALL_MED, "rx_pcm: notify_mask is 0, feed dummy pcm to process" );
      for (i = 0; i < ECALL_PCM_LENGTH; i++)
      {
        cmd_ptr->data.pcm[i] = 0;
      }
    }
    else
    {
      ecall_memscpy ( pcm_buffer, sizeof(pcm_buffer),
                      rx_out_buf_ptr, notify_evt_ptr->request_buf_size );
      for (i = 0; i < ECALL_PCM_LENGTH; i++)
      { 
        /* The vocoder output gives 14 bit linear pcm samples and the 15th,16th bits are the 
          sign extension bits. we need to up-scale two bits to get the actual data*/
        cmd_ptr->data.pcm[i] = (pcm_buffer[i] << 2);
      }
    }

    ecall_put_cmd(cmd_ptr);
  }
  else
  {  
    ECALL_MSG_0( ECALL_ERROR, "ecall_get_cmd_buf() ran out of emodem command buffers at ecall_rx_pcm" );
    return;
  }

  ret = __aprv2_cmd_alloc_send(
            ecall_modem_apr_handle,              /* handle       */
            APRV2_PKT_MSGTYPE_EVENT_V,           /* msg_type     */
            ecall_modem_apr_src_addr,            /* src_addr     */
            ecall_modem_apr_src_port,            /* src_port     */
            ecall_modem_apr_dest_addr,           /* dst_addr     */
            ecall_modem_apr_dest_port,           /* dst_port     */
            ECALL_APR_PUSH_BUFFER,               /* token        */
            VSS_IVPCM_EVT_PUSH_BUFFER_V2,           /* opcode       */
            &push_buff,                          /* payload      */
            sizeof(push_buff));                  /* payload_size */
  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "__aprv2_cmd_alloc_send failed, ret %d",
                               ret );
  }
  __aprv2_cmd_free(ecall_modem_apr_handle, packet);

} /* ECALL_RX_PCM */

/*==========================================================================
  FUNCTION ECALL_VOC_REGISTER

  DESCRIPTION
    This function will send a start PCM session command thru APR interface
    to start a vocoder PCM session with RX/TX mode.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_voc_register
(
  void
)
{
  int32 ret;
  vss_ivpcm_tap_point_t vss_tap_point_rx;
  vss_ivpcm_tap_point_t vss_tap_point_tx;
  uint32 target_payload_len;
  uint32 remaining_target_payload_len;
  uint32 num_tap_points;
  aprv2_packet_t *dst_packet = NULL;
  uint8_t *dst_payload;

  int16 pcm_temp[ECALL_PCM_LENGTH];
  ecall_pcm_buf_type *buf_ptr;

  if (ecall_modem_apr_dest_port == 0)
  {
    /* Initialize CVS stream to get the dest port
       will call ecall_modem_map_shared_memory after successfully created cvs session */
    ECALL_MSG_1( ECALL_MED, "voc_reg: dest_port is 0(%d). Will create passive control session to get port first",
                             ecall_modem_apr_dest_port );

    ecall_modem_apr_init();
    ecall_modem_cvs_init();
    return;
  }

  if (ecall_modem_psap.smem_mapped == FALSE)
  {
    ECALL_MSG_0( ECALL_MED, "voc_reg: memory not mapped. Will register after successfully mapped" );
    ecall_modem_map_shared_memory();
    return;
  }

  if (ecall_psap_voc_registered  == TRUE )
  {
    ECALL_MSG_0( ECALL_HIGH, "vocoder already registered !!" );
    return;
  }

  num_tap_points = 2; /* 2 tap point for TX and RX */
  target_payload_len = sizeof(ecall_modem_psap.ecall_memory_handle) +
                       sizeof(num_tap_points) + num_tap_points*sizeof(vss_ivpcm_tap_point_t);
  remaining_target_payload_len = target_payload_len;

  /* Allocate the dest_packet */
  ret = __aprv2_cmd_alloc_ext(
            ecall_modem_apr_handle,         /* handle       */
            APRV2_PKT_MSGTYPE_SEQCMD_V,     /* msg_type     */
            ecall_modem_apr_src_addr,       /* src_addr     */
            ecall_modem_apr_src_port,       /* src_port     */
            ecall_modem_apr_dest_addr,      /* dst_addr     */
            ecall_modem_apr_dest_port,      /* dst_port     */
            ECALL_APR_IVPCM_START,          /* token        */
            VSS_IVPCM_CMD_START_V2,         /* opcode       */
            target_payload_len,             /* payload_size */
            &dst_packet );                  /* ret_packet   */
  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "__aprv2_cmd_alloc_send failed, ret %d",
                               ret );
  }

  if (dst_packet == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "ecall_voc_register: dst_packet is NULL" );
    return;
  }
  dst_payload = APRV2_PKT_GET_PAYLOAD(uint8_t, dst_packet);

  /* Copy memory_handle to dst_payload */
  ecall_memscpy ( dst_payload, remaining_target_payload_len, 
                  &ecall_modem_psap.ecall_memory_handle, sizeof(ecall_modem_psap.ecall_memory_handle) );
  dst_payload += sizeof(ecall_modem_psap.ecall_memory_handle);
  remaining_target_payload_len -= sizeof(ecall_modem_psap.ecall_memory_handle);

  /* copy num_tap_points to dst_payload */
  ecall_memscpy ( dst_payload, remaining_target_payload_len,
                  &num_tap_points, sizeof(num_tap_points) );
  dst_payload += sizeof(num_tap_points);
  remaining_target_payload_len -= sizeof(num_tap_points);

  /* Configure TX tap point */
  vss_tap_point_tx.tap_point = VSS_IVPCM_TAP_POINT_TX_DEFAULT;
  vss_tap_point_tx.direction = 1; /* input-output tap point, VSS_IVPCM_TAP_POINT_DIR_OUT_IN */
  vss_tap_point_tx.sampling_rate = VSS_IVPCM_SAMPLING_RATE_8K; 
  vss_tap_point_tx.duration = 0;

  /* copy TX tap point structure to destination payload */
  ecall_memscpy ( dst_payload, remaining_target_payload_len,
                  &vss_tap_point_tx, sizeof(vss_tap_point_tx) );
  dst_payload += sizeof( vss_tap_point_tx );
  remaining_target_payload_len -= sizeof(vss_tap_point_tx);

  /* Configure RX tap point */
  vss_tap_point_rx.tap_point = VSS_IVPCM_TAP_POINT_RX_DEFAULT;
  vss_tap_point_rx.direction = 0; /* output tap point, VSS_IVPCM_TAP_POINT_DIR_OUT */
  vss_tap_point_rx.sampling_rate = VSS_IVPCM_SAMPLING_RATE_8K; 
  vss_tap_point_rx.duration = 0;

  /* copy RX tap point structure to destination payload */
  ecall_memscpy ( dst_payload, remaining_target_payload_len,
                  &vss_tap_point_rx, sizeof(vss_tap_point_rx) );
  dst_payload += sizeof( vss_tap_point_rx );

  /* Send the IVPCM_CMD_START command */
  ret = __aprv2_cmd_forward( 
            ecall_modem_apr_handle,  /* handle */
            dst_packet);             /* packet */

  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "__aprv2_cmd_alloc_send failed, ret %d",
                               ret );
  }


  ECALL_MSG_1( ECALL_HIGH, "ecall_pcm_q buffers  %d",
                            q_cnt(&ecall_pcm_q) );

  ECALL_MSG_0( ECALL_HIGH, "PREFILL a buffer so that we dont have to force to get data" );
  memset(pcm_temp, 0, sizeof(int16)*ECALL_PCM_LENGTH);

  if ((buf_ptr = (ecall_pcm_buf_type *)q_get(&ecall_pcm_free_q)) != NULL)
  {
    ecall_memscpy ( buf_ptr->pcm, sizeof(buf_ptr->pcm),
                    pcm_temp,  sizeof(pcm_temp) );
  }

  q_put(&ecall_pcm_q, &buf_ptr->link);

} /* ECALL_VOC_REGISTER */

/*===========================================================================

  FUNCTION ECALL_MODEM_APR_ISR_CB

  DESCRIPTION
    This function will called whenever an APR ISR happens. Will copy everything
    and put into ecall_modem queue to process.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static int32 ecall_modem_apr_isr_cb 
(
  aprv2_packet_t* packet,
  void* dispatch_data
)
{
  ecall_cmd_type *cmd_ptr = NULL;
  int32 ret;
  vss_ivpcm_evt_notify_v2_t* notify_evt;

  if (packet == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "ecall_modem_apr_isr_cb: packet is NULL" );
    return APR_EFAILED;
  }
  if(packet->opcode == VSS_IVPCM_EVT_NOTIFY_V2)
  {
    notify_evt = APRV2_PKT_GET_PAYLOAD( vss_ivpcm_evt_notify_v2_t, packet );
    if (notify_evt == NULL)
    {
      ECALL_MSG_0( ECALL_ERROR, "apr_isr_cb: Cannot get notify_evt payload out" );
      return APR_EFAILED;
    }

    if (notify_evt->tap_point == VSS_IVPCM_TAP_POINT_RX_DEFAULT)
    {
      ecall_modem_psap.rx_evt_notify_cnt++;
    }
    else if (notify_evt->tap_point == VSS_IVPCM_TAP_POINT_TX_DEFAULT)
    {
      ecall_modem_psap.tx_evt_notify_cnt++;
    }
    else
    {
      ECALL_MSG_0( ECALL_ERROR, "apr_isr_cb: unsupported tap point" );
    }
  }

  cmd_ptr = ecall_get_cmd_buf();
  if (cmd_ptr == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "ecall_get_cmd_buf() ran out of cmd buffer at ecall_modem_apr_isr_cb" );
    return APR_EFAILED;
  }
  else
  {
    /* Post a command to the eCall task to process this APR packet */
    cmd_ptr->cmd_id = ECALL_CMD_APR_PACKET;

    /* Copy all APR packet info and process at ecall_ivs_task */
    cmd_ptr->data.apr_packet = packet;
    
    ecall_put_cmd(cmd_ptr);
  }

  ret = __aprv2_cmd_accept_command(ecall_modem_apr_handle,packet);
  if(ret != APR_EOK)
  {
    ECALL_MSG_0( ECALL_ERROR, "__aprv2_cmd_accept_command failed at ecall_modem_apr_isr_cb" );
    return APR_EFAILED;
  }

  return APR_EOK;

} /* ECALL_MODEM_APR_ISR_CB */

/*==========================================================================
  FUNCTION ECALL_MODEM_MAP_SHARED_MEMORY

  DESCRIPTION
    This function will allocate a memory pool for APR interface to process
    the vocoder input/output PCM buffers.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_modem_map_shared_memory
(
  void
)
{
  int32 ret;
  void *ecall_smem_origin_ptr;
  void *ecall_smem_get_ptr;
  uint32 ecall_smem_size = 0;
  uint32  memory_table_size;
  uint32  remaining_memory_table_size;
  uint8_t *memory_table_ptr;
  vss_imemory_block_t            memory_block;
  vss_imemory_table_descriptor_t table_descriptor;
  vss_imemory_cmd_map_physical_t map_physical_cmd;

  if (ecall_modem_psap.smem_mapped == TRUE)
  {
    ECALL_MSG_0( ECALL_ERROR, "map_shared_memory() called but was already mapped!" );
    return;
  }

  if (ecall_modem_psap.smem_allocated == FALSE)
  {
    /* Allocate SMEM shared memory  */
    /* eCall should allocate 80KB (0x14000) of shared memory in B-family. This is to
     * aligned with the shared memory usage of Voicemem. The first 8KB is reserved
     * for eCall usage, while the rest 72KB is for Voicemem. */
    ecall_smem_origin_ptr = smem_alloc(SMEM_VOICE, 0x14000);
    if (ecall_smem_origin_ptr == NULL)
    {
      ECALL_MSG_0( ECALL_ERROR, "smem_alloc failed!" );
      return;
    }

    ecall_smem_get_ptr = smem_get_addr(SMEM_VOICE, &ecall_smem_size);
    ECALL_MSG_2( ECALL_MED, "SMEM:          address: 0x%08X,      size: %d",
                             ecall_smem_get_ptr,
                             ecall_smem_size );

    ecall_memory_table_virtual_addr_orig = (uint32)ecall_smem_origin_ptr;
    ecall_memory_table_physical_addr_orig = qurt_lookup_physaddr(ecall_memory_table_virtual_addr_orig);

    ECALL_MSG_2( ECALL_MED, "memtable_orig: virt_addr_orig: 0x%08X, phys_addr_orig: 0x%08X",
                             ecall_memory_table_virtual_addr_orig,
                             ecall_memory_table_physical_addr_orig );

    ecall_smem_virtual_addr_orig = (uint32)ecall_smem_origin_ptr + ECALL_SMEM_SIZE;
    ecall_smem_physical_addr_orig = qurt_lookup_physaddr(ecall_smem_virtual_addr_orig);

    ECALL_MSG_2( ECALL_MED, "VPCM_orig:     virt_addr_orig: 0x%08X, phys_addr_orig: 0x%08X", 
                             ecall_smem_virtual_addr_orig,
                             ecall_smem_physical_addr_orig );
        
    ecall_modem_psap.smem_allocated = TRUE;
  }

  /* Make Memroy Table address to page size */
  ecall_memory_table_virtual_addr  = ecall_memory_table_virtual_addr_orig + (ECALL_SMEM_SIZE - (ecall_memory_table_virtual_addr_orig % ECALL_SMEM_SIZE) );
  ecall_memory_table_physical_addr = ecall_memory_table_physical_addr_orig + (ECALL_SMEM_SIZE - (ecall_memory_table_physical_addr_orig % ECALL_SMEM_SIZE) );

  memset( (uint8*)ecall_memory_table_virtual_addr, 0, 4096 );

  ECALL_MSG_2( ECALL_MED, "memtable_addr: virt_addr: 0x%08X, phys_addr: 0x%08X",
                           ecall_memory_table_virtual_addr, ecall_memory_table_physical_addr );

  /* Make VPCM address aligned to page size */
  ecall_smem_virtual_addr = ecall_smem_virtual_addr_orig + (ECALL_SMEM_SIZE - (ecall_smem_virtual_addr_orig % ECALL_SMEM_SIZE) );
  ecall_smem_physical_addr = ecall_smem_physical_addr_orig + (ECALL_SMEM_SIZE - (ecall_smem_physical_addr_orig % ECALL_SMEM_SIZE) );

  memset( (uint8*)ecall_smem_virtual_addr, 0, 4096 );

  ECALL_MSG_2( ECALL_MED, "VPCM:          virt_addr: 0x%08X, phys_addr: 0x%08X",
                           ecall_smem_virtual_addr,
                           ecall_smem_physical_addr);

  ecall_modem_psap.ecall_tx_out_buf_addr = ecall_smem_virtual_addr;
  ecall_modem_psap.ecall_tx_in_buf_addr = ecall_smem_virtual_addr + 1024; 
  ecall_modem_psap.ecall_rx_out_buf_addr = ecall_smem_virtual_addr + 2048; 
  ecall_modem_psap.ecall_rx_in_buf_addr = ecall_smem_virtual_addr + 3072; 

  ECALL_MSG_2( ECALL_MED, "ecall_tx_out_buf_addr: 0x%08X, tx_in_buf_addr: 0x%08X",
                           ecall_modem_psap.ecall_tx_out_buf_addr,
                           ecall_modem_psap.ecall_tx_in_buf_addr );
  ECALL_MSG_2( ECALL_MED, "ecall_rx_out_buf_addr: 0x%08X, rx_in_buf_addr: 0x%08X",
                           ecall_modem_psap.ecall_rx_out_buf_addr,
                           ecall_modem_psap.ecall_rx_in_buf_addr );

  ecall_modem_psap.ecall_tx_out_buf_phy_addr = ecall_smem_physical_addr;
  ecall_modem_psap.ecall_tx_in_buf_phy_addr = ecall_smem_physical_addr + 1024; 
  ecall_modem_psap.ecall_rx_out_buf_phy_addr = ecall_smem_physical_addr + 2048; 
  ecall_modem_psap.ecall_rx_in_buf_phy_addr = ecall_smem_physical_addr + 3072; 

  ECALL_MSG_2( ECALL_MED, "ecall_tx_out_buf_phy_addr: 0x%08X, tx_in_buf_phy_addr: 0x%08X",
                           ecall_modem_psap.ecall_tx_out_buf_phy_addr,
                           ecall_modem_psap.ecall_tx_in_buf_phy_addr );
  ECALL_MSG_2( ECALL_MED, "ecall_rx_out_buf_phy_addr: 0x%08X, rx_in_buf_phy_addr: 0x%08X",
                           ecall_modem_psap.ecall_rx_out_buf_phy_addr,
                           ecall_modem_psap.ecall_rx_in_buf_phy_addr );

  memory_table_size = sizeof(table_descriptor) + sizeof(memory_block);
  memory_table_ptr = (uint8*)ecall_memory_table_virtual_addr;
  remaining_memory_table_size = memory_table_size;

  /* Initialize the table_descriptor object */
  table_descriptor.mem_address = 0;
  table_descriptor.mem_size    = 0;

  /* Copy table_descriptor to shared memory */
  ecall_memscpy ( memory_table_ptr, remaining_memory_table_size, 
                  &table_descriptor, sizeof(table_descriptor) );
  memory_table_ptr += sizeof(table_descriptor);
  remaining_memory_table_size -= sizeof(table_descriptor);

  /* Initialize the memory_block object */
  memory_block.mem_address = ecall_smem_physical_addr;
  memory_block.mem_size    = ECALL_SMEM_SIZE;

  /* Copy memory_block to shared memory */
  ecall_memscpy ( memory_table_ptr, remaining_memory_table_size,
                  &memory_block, sizeof(memory_block) );
  memory_table_ptr += sizeof(memory_block);

  /* Initialize the map_physical_cmd object */
  map_physical_cmd.table_descriptor.mem_address = ecall_memory_table_physical_addr;
  map_physical_cmd.table_descriptor.mem_size = memory_table_size;
  map_physical_cmd.is_cached        = TRUE;
  map_physical_cmd.cache_line_size  = 128;
  map_physical_cmd.access_mask      = (uint32_t)3;
  map_physical_cmd.page_align       = 4096;
  map_physical_cmd.min_data_width   = 8;
  map_physical_cmd.max_data_width   = 64;

  ret = __aprv2_cmd_alloc_send(
            ecall_modem_apr_handle,                /* handle       */
            APRV2_PKT_MSGTYPE_SEQCMD_V,            /* msg_type     */
            ecall_modem_apr_src_addr,              /* src_addr     */
            ecall_modem_apr_src_port,              /* src_port     */
            ecall_modem_apr_mvm_addr,              /* dst_addr     */
            APR_NULL_V,                            /* dst_port     */
            ECALL_APR_MAP_MEMORY,                  /* token        */
            VSS_IMEMORY_CMD_MAP_PHYSICAL,          /* opcode       */
            &map_physical_cmd,                     /* payload      */
            sizeof(map_physical_cmd) );            /* payload_size */

  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "__aprv2_cmd_alloc_send failed, ret %d",
                               ret );
  }

} /* ECALL_MODEM_MAP_SHARED_MEMORY */

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
)
{
  vss_imemory_cmd_unmap_t mem_unmap_cmd;
  int32 ret;
 
  if (ecall_modem_psap.smem_mapped == FALSE)
  {
    ECALL_MSG_0( ECALL_ERROR, "unmap_apr_memory() called but was already unmapped" );
    return;
  }
  else if (ecall_psap_voc_registered == TRUE)
  {
    ECALL_MSG_0( ECALL_ERROR, "unmap_apr_memory() called but still registered for vocoder" );
    return;
  }
   
  mem_unmap_cmd.mem_handle = ecall_modem_psap.ecall_memory_handle;
  
  ret = __aprv2_cmd_alloc_send(
            ecall_modem_apr_handle,                /* handle       */
            APRV2_PKT_MSGTYPE_SEQCMD_V,            /* msg_type     */
            ecall_modem_apr_src_addr,              /* src_addr     */
            ecall_modem_apr_src_port,              /* src_port     */
            ecall_modem_apr_mvm_addr,              /* dst_addr     */
            APR_NULL_V,                            /* dst_port     */
            ECALL_APR_UNMAP_MEMORY,                /* token        */
            VSS_IMEMORY_CMD_UNMAP,                 /* opcode       */
            &mem_unmap_cmd,                        /* payload      */
            sizeof(mem_unmap_cmd) );               /* payload_size */

  /* Reset the ecall rx and tx buf addr to NULL */
   ecall_modem_psap.ecall_rx_in_buf_addr      = 0;
   ecall_modem_psap.ecall_rx_out_buf_addr     = 0;
   ecall_modem_psap.ecall_tx_in_buf_addr      = 0;
   ecall_modem_psap.ecall_tx_out_buf_addr     = 0;
   ecall_modem_psap.ecall_tx_in_buf_phy_addr  = 0;
   ecall_modem_psap.ecall_tx_out_buf_phy_addr = 0;
   ecall_modem_psap.ecall_rx_in_buf_phy_addr  = 0;
   ecall_modem_psap.ecall_rx_out_buf_phy_addr = 0;

   /* Reset smem_mapped flag */
   ecall_modem_psap.smem_mapped = FALSE;

  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "__aprv2_cmd_alloc_send failed, ret %d",
                               ret );
  }

} /* ECALL_MODEM_UNMAP_SHARED_MEMORY */

/*===========================================================================

  FUNCTION ECALL_MODEM_APR_INIT

  DESCRIPTION
    This function will initialize the APR interface for eCall

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_modem_apr_init 
( 
  void
)
{
  int32 ret;

  /* Register with APR to receive messages. */
  ret = __aprv2_cmd_register2(
            &ecall_modem_apr_handle,           /* <- Return eCall handle */
            ecall_modem_psap_ecall_dns,         /* -> Name string of the eCall service */
            sizeof(ecall_modem_psap_ecall_dns), /* -> Size of the name string, including NULL */
            0,                                 /* -> Service address to register */
            ecall_modem_apr_isr_cb,            /* -> ISR dispatch callback to register */
            NULL,                              /* -> Static data to pass to the ISR callback */
            &ecall_modem_apr_src_addr);        /* <- Return pointer to the registered service address */
  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "ecall_modem_apr_init: __aprv2_cmd_register2 failed, ret %d",
                               ret );
  }

  ret = __aprv2_cmd_local_dns_lookup(
            ecall_modem_psap_voc_dns, 
            sizeof(ecall_modem_psap_voc_dns), 
            &ecall_modem_apr_dest_addr);
  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "ecall_modem_apr_init: __aprv2_cmd_local_dns_lookup CVS failed, ret %d",
                               ret );
  }

  ret = __aprv2_cmd_local_dns_lookup(
            ecall_modem_psap_mvm_dns,
            sizeof(ecall_modem_psap_mvm_dns),
            &ecall_modem_apr_mvm_addr);
  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "ecall_modem_apr_init: __aprv2_cmd_local_dns_lookup MVM failed, ret %d",
                               ret );
  }

} /* ECALL_MODEM_APR_INIT */

/*==========================================================================
  FUNCTION ECALL_PROCESS_APR_PACKET

  DESCRIPTION
    This function will process the incoming APR packet.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_process_apr_packet
(
  aprv2_packet_t *packet
)
{
  
  switch (packet->opcode)
  {
    case APRV2_IBASIC_EVT_ACCEPTED:
      __aprv2_cmd_free(ecall_modem_apr_handle, packet );
      break;

    case APRV2_IBASIC_RSP_RESULT:
      /* Will free the cmd in the function */
      ecall_process_ibasic_rsp_result(packet);
      break;

    case VSS_IVPCM_EVT_NOTIFY_V2:
      ecall_process_ivpcm_evt_notify(packet);
      break;

    default:
      ECALL_MSG_1( ECALL_ERROR, "Received an unsupported command %d",
                                 packet->opcode );
      __aprv2_cmd_end_command(ecall_modem_apr_handle, packet, APR_EUNSUPPORTED );
      break;
  }

} /* ECALL_PROCESS_APR_PACKET */

/*==========================================================================
  FUNCTION ECALL_PROCESS_IVPCM_EVT_NOTIFY

  DESCRIPTION
    This function will process the incoming IVPCM_EVT_NOTIFY every 20ms and
    start the RX/TX PCM process.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_process_ivpcm_evt_notify
(
  aprv2_packet_t* packet
)
{
  vss_ivpcm_evt_notify_v2_t* notify_evt;

  if (packet == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "process_evt_notify: packet is NULL" );
    return;
  }
  notify_evt = APRV2_PKT_GET_PAYLOAD( vss_ivpcm_evt_notify_v2_t, packet );
  if (notify_evt == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "process_evt_notify: Cannot get notify_evt payload out" );
    return;
  }

  if (notify_evt->tap_point == VSS_IVPCM_TAP_POINT_RX_DEFAULT)
  {
    ecall_modem_psap.rx_process_cnt++;
    if (ecall_modem_psap.rx_evt_notify_cnt != ecall_modem_psap.rx_process_cnt)
    {
      ECALL_MSG_2( ECALL_ERROR, "process_evt_notify: rx_evt_notify_cnt %d != rx_process_cnt %d, drop this frame",
                                 ecall_modem_psap.rx_evt_notify_cnt,
                                 ecall_modem_psap.rx_process_cnt );
      __aprv2_cmd_free(ecall_modem_apr_handle, packet);
      return;
    }
    ecall_rx_pcm(packet);
  }
  else if (notify_evt->tap_point == VSS_IVPCM_TAP_POINT_TX_DEFAULT)
  {
    ecall_modem_psap.tx_process_cnt++;
    if (ecall_modem_psap.tx_evt_notify_cnt != ecall_modem_psap.tx_process_cnt)
    {
      ECALL_MSG_2( ECALL_ERROR, "process_evt_notify: tx_evt_notify_cnt %d != tx_process_cnt %d, drop this frame",
                                 ecall_modem_psap.tx_evt_notify_cnt,
                                 ecall_modem_psap.tx_process_cnt );
      __aprv2_cmd_free(ecall_modem_apr_handle, packet);
      return;
    }
    ecall_tx_pcm(packet);
  }
  else
  {
    ECALL_MSG_1( ECALL_ERROR, "process_evt_notify: unsupported tap point %d",
                               notify_evt->tap_point );
  }

} /* ECALL_PROCESS_IVPCM_EVT_NOTIFY */

#else /* Legacy Vocoder */
/*===========================================================================

                L e g a c y   V o c o d e r   F u n c t i o n

===========================================================================*/

/*==========================================================================
  FUNCTION ECALL_TX_PCM

  DESCRIPTION
    Pulls a PCM buffer off of the queue and copies it into the Vocoder's
    buffer for encoding and transmission.  Called in interrupt context.

  DEPENDENCIES
    Legacy Vocoder

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
voc_pcm_data_status_type ecall_tx_pcm
(
  uint16  *pcm
)
{
  ecall_pcm_buf_type *buf_ptr;
  uint32 i;
  int16 pcm_temp[ECALL_PCM_LENGTH];
  
  if(ecall_modem_psap.ready_to_deregister == TRUE )
  {
    if(ecall_modem_psap.pcm_tx_buf_cnt == 0)
    {
      ecall_modem_psap.ready_to_deregister = FALSE;
      ecall_psap_session_stop(TRUE);
    }
    ecall_modem_psap.pcm_tx_buf_cnt--;
  }

  ECALL_MSG_1( ECALL_MED, "ecall_tx_pcm count %d",
                           q_cnt(&ecall_pcm_q) );

  if ((buf_ptr = (ecall_pcm_buf_type *) q_get(&ecall_pcm_q)) != NULL)
  {
    for (i = 0; i < ECALL_PCM_LENGTH; i++)
    {
      pcm[i] = buf_ptr->pcm[i]>>2;    /*lint !e702*/
    }
    q_put(&ecall_pcm_free_q, &buf_ptr->link);
    return VOC_PCM_DATA_STATUS_AVAILABLE;
  }
 
  if(psap.rx.state != PsapIdle)
  {
    ECALL_MSG_0( ECALL_HIGH, "ecall_tx_pcm no data, force to get data" );
   
    ecall_psap_rx_downlink();
    
    ecall_psap_tx_process(pcm_temp, sizeof(pcm_temp));

    for (i=0;i < ECALL_PCM_LENGTH;i++) 
    {
      pcm[i] = pcm_temp[i]>>2;       /*lint !e702*/
    }
  }
  else
  {
    ECALL_MSG_0( ECALL_MED, "sending silence" );
    memset(pcm,0, ECALL_PCM_LENGTH);
  }

  return VOC_PCM_DATA_STATUS_AVAILABLE;

} /* ecall_tx_pcm() */


/*==========================================================================
  FUNCTION ECALL_RX_PCM

  DESCRIPTION
    Called by Vocoder task in interrupt context to inform us of a new
    incoming PCM frame.  Called every 20ms with 160 PCM samples 
    (16 bits @ 8 kHz, but only 14 bits used in samples).

  DEPENDENCIES
    Legacy Vocoder

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void ecall_rx_pcm
(
  const uint16  *pcm
)
{
  ecall_cmd_type *cmd_ptr;
  uint16 i;

  cmd_ptr = ecall_get_cmd_buf();
  if (cmd_ptr != NULL)
  {
    ECALL_MSG_0( ECALL_MED, "ecall_rx_pcm put cmd RX_PCM" );
    /* Post a command to the eCall task to process this PCM frame */
    cmd_ptr->cmd_id = ECALL_CMD_PROCESS_RX_PCM;

    for (i = 0; i < ECALL_PCM_LENGTH; i++)
    {
      cmd_ptr->data.pcm[i] = (int16) (pcm[i]  << 2);
    }
    ecall_put_cmd(cmd_ptr);
  }
  else
  {
    ECALL_MSG_0( ECALL_ERROR, "Dropped PCM sample; ran out of command buffers" );
  }
} /* ecall_tx_pcm() */

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
)
{
  voc_status_type ret;

  int16 pcm_temp[ECALL_PCM_LENGTH];
  ecall_pcm_buf_type *buf_ptr;

  if (ecall_psap_voc_registered  == TRUE )
  {
    ECALL_MSG_0( ECALL_HIGH, "vocoder already registered !!" );
    return TRUE;
  }
  
  ECALL_MSG_1( ECALL_HIGH, "ecall_pcm_q buffers  %d",
                            q_cnt(&ecall_pcm_q) );

  ecall_modem_psap_voc_configure();
  
  ret = voc_register_pcm_input_client(VOC_PCM_INTERFACE_TX_INPUT_TO_ENC,
                                      ecall_tx_pcm);
  if (VOC_DONE_S != ret)
  {
    ECALL_MSG_1( ECALL_ERROR, "Failed to register eCall TX input client with vocoder! err = %d", 
                               ret );
  }
  else
  {
    ret = voc_register_pcm_output_client(VOC_PCM_INTERFACE_RX_OUTPUT_FROM_DEC,
                                         ecall_rx_pcm);
    if (VOC_DONE_S != ret)
    {
      ECALL_MSG_1( ECALL_ERROR, "Failed to register eCall RX output client with vocoder! err = %d",
                                 ret );
      /* Unregister input client */
      (void)voc_register_pcm_input_client(VOC_PCM_INTERFACE_TX_INPUT_TO_ENC, NULL);
    }
    else
    {
      ECALL_MSG_0( ECALL_HIGH, "PREFILL a buffer so that we dont have to force to get data" );
      memset(pcm_temp, 0, sizeof(int16)*ECALL_PCM_LENGTH);

      if ((buf_ptr = (ecall_pcm_buf_type *)q_get(&ecall_pcm_free_q)) != NULL)
      {
        ecall_memscpy ( buf_ptr->pcm, sizeof(buf_ptr->pcm),
                        pcm_temp,  sizeof(pcm_temp) );
      }

      q_put(&ecall_pcm_q, &buf_ptr->link);

      ECALL_MSG_0( ECALL_HIGH, "Successfully registered eCall TX in/RX out callbacks with vocoder" );
      ecall_psap_voc_registered = TRUE;
      return TRUE;
    }
  }
  return FALSE;

} /* ecall_modem_voc_register() */


/*==========================================================================
  FUNCTION ECALL_VOC_UNREGISTER

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
void ecall_voc_unregister
(
  void
)
{
  voc_status_type ret;
  ecall_pcm_buf_type *buf_ptr;

  ECALL_MSG_0( ECALL_HIGH, "de-register with vocoder RX and TX" );

  ecall_modem_psap_voc_restore();

  ret = voc_register_pcm_input_client(VOC_PCM_INTERFACE_TX_INPUT_TO_ENC, NULL);
  if (VOC_DONE_S != ret)
  {
    ECALL_MSG_1( ECALL_ERROR, "Couldn't unregister eCall TX input client with vocoder! err = %d", 
                               ret );
  }

  ret = voc_register_pcm_output_client(VOC_PCM_INTERFACE_RX_OUTPUT_FROM_DEC, NULL);
  if (VOC_DONE_S != ret)
  {
    ECALL_MSG_1( ECALL_ERROR, "Couldn't unregister eCall RX output client with vocoder! err = %d",
                               ret );
  }

  ECALL_MSG_1( ECALL_MED, "ecall_pcm_q buffers  %d",
                           q_cnt(&ecall_pcm_q) );

  ecall_psap_voc_registered = FALSE;

  /* Move everything back to their free queues */
  while ((buf_ptr = (ecall_pcm_buf_type *) q_get(&ecall_pcm_q)) != NULL)
  {
    q_put(&ecall_pcm_free_q, &buf_ptr->link);
  }

} /* ECALL_VOC_UNREGISTER() */

#endif /* FEATURE_ECALL_HAS_VOICE_A */


#else

  void ecall_modem_psap_dummy_func(void);

#endif /* End FEATURE_ECALL_PSAP */



