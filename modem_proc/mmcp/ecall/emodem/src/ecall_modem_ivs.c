/*===========================================================================

                        E C A L L _ M O D E M _ I V S . C

DESCRIPTION
    This file would get commands from the App to START or Abort tranmission.
    This is responsible for registering the vocoder and deregistering it. It 
    also registers with the vocoder callbacks                                 

REFERENCES
  C Reference code 3GPP TS  26.268

EXTERNALIZED FUNCTIONS
    - ecall_modem_ivs_init()
    - ecall_ivs_getData()
    - ecall_ivs_process()
    - ecall_ivs_process_start()
    - ecall_ivs_process_abort_tx()
    - ecall_ivs_process_abort_all()

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodem/src/ecall_modem_ivs.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $ 
===========================================================================*/

/*===========================================================================

                         I N C L U D E   F I L E S

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_ECALL_IVS

#include "ecall_modem_apis.h"
#include "ecall_modem_ivs.h"
#include "ecall_ivs_task.h"
#include "ecall_defines.h"
#include "ecall_modem.h"
#include "ecall_sync.h"
#include "ecall_fec.h"
#include "ecall_rom.h"
#include "msg.h"
#include "err.h"
#include <stringl/stringl.h>
#include "queue.h"
#include "event.h"
#include "ecall_efs.h"
#include "ecall_control.h"
#include "ecall_modem_clients.h"

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
#include "qurt.h"
#include "qurt_memory.h"
#include "vss_imemory_public_if.h"
#endif /* FEATURE_ECALL_HAS_VOICE_A */

#ifdef FEATURE_MVS
#include "mvs.h"
#endif

#define ECALL_PCM_BUF_COUNT     8

static int16   pcm_buffer[ECALL_PCM_LENGTH];
LOCAL const uint32 ecall_voc_timer_duration[3] = 
{
  /* This table specified the time duration to detect missed ecall_rx_pcm()  */
  30,   /* 1st timer: 30ms */ 
  10,   /* 2nd timer: 10ms */ 
  20    /* 3rd and so on: 20ms */
};

/* Outgoing PCM sample queue */
static ecall_pcm_buf_type   ecall_pcm_buf[ECALL_PCM_BUF_COUNT];
static q_type               ecall_pcm_free_q;
static q_type               ecall_pcm_q;

#if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
#define ECALL_SMEM_SIZE      4096
/* Vocoder PCM parameters */
static uint16 ecall_modem_apr_src_addr;
static uint16 ecall_modem_apr_dest_addr;
static uint16 ecall_modem_apr_src_port = 0xDDDD;
static uint16 ecall_modem_apr_dest_port = 0;
static char_t ecall_modem_stream_name[] = "default modem voice";
static uint32 ecall_modem_apr_handle;
static char ecall_modem_ivs_ecall_dns[] = "qcom.modem.ecall";
static char ecall_modem_ivs_voc_dns[] = "qcom.audio.cvs";
uint32 ecall_smem_virtual_addr_orig;
uint32 ecall_smem_physical_addr_orig;
uint32 ecall_smem_virtual_addr;
uint32 ecall_smem_physical_addr;
#endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */

#ifdef FEATURE_ECALL_HAS_VOICE_B
static uint16 ecall_modem_apr_mvm_addr;
static char ecall_modem_ivs_mvm_dns[] = "qcom.audio.mvm";
static uint32 ecall_memory_table_virtual_addr_orig;
static uint32 ecall_memory_table_physical_addr_orig;
static uint32 ecall_memory_table_virtual_addr;
static uint32 ecall_memory_table_physical_addr;
#endif /* FEATURE_ECALL_HAS_VOICE_B */

/* Tokens for APR commands */
typedef enum
{
  ECALL_APR_IVPCM_STOP = 1,                 /* VSS_IVPCM_CMD_STOP */
  ECALL_APR_IVPCM_START_RX_ONLY,            /* VSS_IVPCM_CMD_START with RX-only session */
  ECALL_APR_IVPCM_START_RX_TX,              /* VSS_IVPCM_CMD_START with RX/TX session */
  ECALL_APR_MAP_MEMORY,                     /* VSS_ICOMMON_CMD_MAP_MEMORY */
  ECALL_APR_UNMAP_MEMORY,                   /* VSS_ICOMMON_CMD_UNMAP_MEMORY */
  ECALL_APR_PUSH_BUFFER,                    /* VSS_IVPCM_EVT_PUSH_BUFFER */
  ECALL_APR_CREATE_PASSIVE_CONTROL_SESSION  /* VSS_ISTREAM_CMD_CREATE_PASSIVE_CONTROL_SESSION */
}ecall_apr_token_type;

/* Stores the IVS Modem UL an DL states*/
static ecall_ivs_data ivs;

typedef enum {
  ECALL_SYNC_UNLOCK = 0,
  ECALL_SYNC_LOCKED
} ecall_sync_lock_e_type;


ecall_cb_data_type ecall_ivs_status;

/* Local structure for the modem to know about the Vocoder and MSD states*/
LOCAL struct
{   
  ecall_tx_mode_type     tx_mode;         /* PUSH or PULL */
  boolean voc_config_ecall;
  uint8 drop_tx_pcm_count;
  uint8 missed_rx_pcm_count;
  boolean voc_rx_registered;
  boolean voc_tx_registered;
  boolean is_msd_received_from_app;
  #if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
  boolean voc_config_mode;                /* 0: RX only mode 
                                             1: RX/TX   mode */
  boolean voc_pending_register;
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
  boolean voc_re_register;
  ecall_voc_mode_type voc_re_register_mode;
  #endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */

  #ifdef FEATURE_ECALL_HAS_VOICE_B
  uint32_t ecall_memory_handle;
  #endif /* FEATURE_ECALL_HAS_VOICE_B */

  #ifdef FEATURE_MVS 
  mvs_dtx_mode_type mvs_dtx_mode;
  mvs_scr_mode_type mvs_scr_mode;
  #endif  

} ecall_modem_ivs;
  

/*==========================================================================
              I V S   C o m m o n   F u n c t i o n s
===========================================================================*/



/*==========================================================================

  FUNCTION ecall_get_pcm_buf

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
  ecall_pcm_buf_type *buf_ptr = NULL;
  if ((buf_ptr = q_get(&ecall_pcm_free_q)) == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "No items on eCall task outgoing PCM buffer q" );
  }
  return buf_ptr;
} /* ecall_get_pcm_buf() */


/*==========================================================================

  FUNCTION ecall_put_pcm_buf

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


#ifdef FEATURE_ADIE_CODEC_OFF

/*==========================================================================
  FUNCTION      ECALL_IVS_SND_VOL_CB_FUNC

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
void ecall_ivs_snd_vol_cb_func
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
    ECALL_MSG_1( ECALL_HIGH, "set volume non zero %d",
                              volume );
      /* AUX_HANDSET and AUX_NOPP doesn't seem to share the same volume setting */
    snd_set_volume( SND_DEVICE_AUXPCM_HANDSET, SND_METHOD_VOICE, 
                              (uint8)volume , NULL, NULL);
  }
}    

#endif /* FEATURE_ADIE_CODEC_OFF */

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
void ecall_voc_config_restore
(
  void
)
{
  if(ecall_modem_ivs.voc_config_ecall == FALSE)
  {
    ECALL_MSG_0( ECALL_HIGH, "voc config has already been restored from eCall" );
    return;
  }
  /*-----------------------------------------------------------------------*/
  /*         restore previous vocoder configuration before ecall           */
  /*-----------------------------------------------------------------------*/

  #ifdef FEATURE_MVS
  #if defined(MSMAUD_VOC_AMR) || defined(MSMAUD_VOC_HR) \
    || defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR) \
    || defined(MSMAUD_VOC_AMR_WB)

  ECALL_MSG_0( ECALL_HIGH, "restore AMR DTX " );
  #if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
  mvssup_amr_set_scr_mode(MVS_SCR_ENABLE);
  #else /* Legacy Vocoder */
  mvssup_amr_set_scr_mode(ecall_modem_ivs.mvs_scr_mode);
  #endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */

  ECALL_MSG_0( ECALL_HIGH, "restore GSM DTX " );
  #if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
  mvssup_gsm_set_dtx_mode(MVS_DTX_ON);
  #else /* Legacy Vocoder */
  mvssup_gsm_set_dtx_mode(ecall_modem_ivs.mvs_dtx_mode);
  #endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */

  #endif
  #endif /*end FEATURE_MVS*/

  ecall_modem_ivs.voc_config_ecall =  FALSE;

} /* ECALL_VOC_CONFIG_RESTORE */

/*==========================================================================
  FUNCTION ECALL_VOC_CONFIGURE

  DESCRIPTION
    Registers eCall functions with Vocoder so that eCall feeds PCM
    frames for output and receives decoded PCM input.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    Disables EC, NS, AGC and AVC
    EC,NS,AGC effects the TX
    AGC, AVC effects the RX
===========================================================================*/
void ecall_voc_configure
(
  void
)
{

  if(ecall_modem_ivs.voc_config_ecall == TRUE)
  {
    ECALL_MSG_0( ECALL_HIGH, "vocoder has already been configured for eCall" );
    return;
  }

  #ifdef FEATURE_MVS
  #if defined(MSMAUD_VOC_AMR) || defined(MSMAUD_VOC_HR) \
    || defined(MSMAUD_VOC_EFR) || defined(MSMAUD_VOC_FR) \
    || defined(MSMAUD_VOC_AMR_WB)

  #if ( !(defined FEATURE_ECALL_HAS_VOICE_A) && !(defined FEATURE_ECALL_HAS_VOICE_B) ) /* Legacy Vocoder */
  ecall_modem_ivs.mvs_dtx_mode  = mvssup_gsm_get_dtx_mode();
  ECALL_MSG_1( ECALL_HIGH, "get gsm DTX state %d",
                            ecall_modem_ivs.mvs_dtx_mode );

  ecall_modem_ivs.mvs_scr_mode  = mvssup_amr_get_scr_mode();
  ECALL_MSG_1( ECALL_HIGH, "get amr DTX state  %d",
                            ecall_modem_ivs.mvs_scr_mode );
  #endif /* Legacy Vocoder */

  /*-----------------------------------------------------------------------*/
  /*                  Set vocoder configuration                            */
  /*-----------------------------------------------------------------------*/
  ECALL_MSG_0( ECALL_HIGH, "AMR DTX Disable" );
  mvssup_amr_set_scr_mode(MVS_SCR_DISABLE);

  ECALL_MSG_0( ECALL_HIGH, "GSM DTX OFF" );
  mvssup_gsm_set_dtx_mode(MVS_DTX_OFF);
  #endif
  #endif  /* end FEATURE_MVS*/

  /*FEATURE_ADIE_CODEC_OFF is defined if Internal codec is disabled*/
  #ifdef FEATURE_ADIE_CODEC_OFF
    ECALL_MSG_0( ECALL_HIGH, "get Handset volume" );
    snd_get_volume(SND_DEVICE_AUXPCM_HANDSET, SND_METHOD_VOICE,  ecall_ivs_snd_vol_cb_func, NULL);

    ECALL_MSG_0( ECALL_HIGH, "selecting device SND_DEVICE_AUXPCM_HANDSET!" );
    snd_set_device( SND_DEVICE_AUXPCM_HANDSET, SND_MUTE_UNMUTED, SND_MUTE_UNMUTED, NULL, NULL);
  #endif

  ecall_modem_ivs.voc_config_ecall =  TRUE;
} /* ECALL_VOC_CONFIGURE */

/*==========================================================================

  FUNCTION      ECALL_POST_CLIENT_NOTIFY

  DESCRIPTION
      It would post a notification to IVS task for passing the IVS status
      to all the registered clients

  DEPENDENCIES
    None.

  RETURN VALUE
    None.
  
  SIDE EFFECTS
    None.

===========================================================================*/
void ecall_post_client_notify
(
  void
)
{
  ecall_cmd_type *cmd_ptr = NULL;

  cmd_ptr = ecall_get_cmd_buf();

  if(cmd_ptr != NULL)
  {
    /* Post a command to the eCall task to register the client */
    cmd_ptr->cmd_id          =  ECALL_CMD_CLIENT_NOTIFY;
    cmd_ptr->data.ivs_status = ecall_ivs_status;

    ecall_put_cmd(cmd_ptr);
  }
  else
  {
    ECALL_MSG_0( ECALL_ERROR, "No empty cmd buffer to notify clients" );
  }
}

/*==========================================================================

  FUNCTION    ECALL_MODEM_IVS_EVENT_REPORT

  DESCRIPTION
    Prints the event to the qxdm

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_modem_ivs_event_report
(
  event_id_enum_type event_id
)
{
  byte event_payload[3];

  switch (event_id)
  {
    case EVENT_ECALL_SESSION_COMPLETE:
          event_payload[0] = (byte)ivs.tx.rv;                  /* Number of Redundancy Versions  */
      	  event_payload[1] = (byte)ivs.tx.stateCnt[DlMsgAck];  /* Number of ACKs  */
          if(ivs.tx.mod.type == Mod3bit4smp)
          {
            event_payload[2] = 0;        /* FAST Modulation */
          }
          else if(ivs.tx.mod.type == Mod3bit8smp)
          {
            event_payload[2] = 1;        /* SLOW Modulation */
          }
          else
          {
            event_payload[2] = 2;        /* undefined modulation */
          }

          event_report_payload(EVENT_ECALL_SESSION_COMPLETE, 3, (void *)event_payload); 
          break;

    case EVENT_ECALL_SESSION_RESET:
          event_payload[0] = 0;
          event_report_payload(EVENT_ECALL_SESSION_RESET, 1, (void *)event_payload);
          break;

    default:
          ECALL_MSG_0( ECALL_ERROR, "Unknow eCall event " );
          break;
  }
}

/*==========================================================================

  FUNCTION    ECALL_IVS_RX_RESET

  DESCRIPTION
    reset the ecall modem receiver state

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/

void ecall_ivs_rx_reset
(
  void
)
{
  ecall_ctrl_rx_data *ctrl = &ivs.rx.ctrl;
  ecall_sync_state  *sync = &ivs.rx.sync;

  /* reset state */
  ivs.rx.state = IvsIdle;

  /* reset control receiver */
  ecall_ctrl_rx_reset(ctrl, PortIvs, sync, ivs.rx.memCtrl, ECALL_LOCK_START_DL);

  /* reset counters for SEND uplink control */
  ivs.rx.dlData   = DlMsgNoop;
  ivs.rx.dlMsgCnt = -1;
  ivs.rx.dlIndex  = -1;

  /* reset sync */
  ecall_sync_reset(sync, ivs.rx.memSync, NULL);
  ECALL_MSG_0( ECALL_HIGH, "[IVS ] Rx reset receiver" );
}

/*==========================================================================

  FUNCTION    ECALL_IVS_TX_RESET

  DESCRIPTION
    main synchronization function

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_ivs_tx_reset
(
  void
)
{
  int16 k =0;
   /* reset state */
  ivs.tx.state = IvsIdle;

  /* set control link parameters (SEND) */
  ecall_ctrl_tx_reset(&ivs.tx.ctrl, PortIvs);

  /* reset state counters */
  for (k = 0; k <= ECALL_SET_LLMSG+ ECALL_SET_HLMSG; k++) {
    ivs.tx.stateCnt[k] = 0;
    ivs.tx.stateIgn[k] = 0;
  }
  ivs.tx.stateCnt[DlMsgSend] = -1;
  ivs.tx.pendingStart = FALSE;

  /* reset uplink counters */
  ivs.tx.rv      = 0;
  ivs.tx.ulN     = 0;
  ivs.tx.ulIndex = 0;
  ivs.tx.ulDelay = ECALL_IVS_TXDELAY;
  ivs.tx.dlMsgOld = DlMsgNoop;
 
  /* reset modulator */
  ivs.tx.mod_type = IvsModNone;
 
  ECALL_MSG_0( ECALL_HIGH, "[IVS ] Tx reset receiver" );
}

/*==========================================================================

  FUNCTION    ECALL_IVS_RESET

  DESCRIPTION
    Resets the counters and state of the modem

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_ivs_reset
(
  void
) 
{
  ecall_ivs_rx_reset();
  ecall_ivs_tx_reset();
}
 

/*==========================================================================

  FUNCTION    ECALL_MODEM_ENCODE_MSD

  DESCRIPTION  converts the MSD from byte to bit and does the encoding and saves 
               in a bit buffer

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_modem_encode_msd
(
  const uint8 *msd,  
  int16 length
)
{
  if (msd != NULL) 
  {
    /* initialize modulator to be undefined */
    ecall_set_mod_state(&ivs.tx.mod, ModUndef);

    /* clear global NACK count for new message */
    ivs.tx.overallNack = 0;

    /* convert MSD and encode (all redundancy versions) */
    ecall_byte2bit(msd, ivs.tx.memCode, length);
    ecall_fec_encode(ivs.tx.memCode);
  }
}


/*==========================================================================

  FUNCTION    ECALL_MODEM_IVS_RESUME_MODULATOR

  DESCRIPTION             
      Resume the IVS modulator settings.     

  DEPENDENCIES
    None.

  RETURN VALUE
    None.            
 
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_modem_ivs_resume_modulator
(
  void
)
{
  /* resume the modulator state */
  if (ivs.tx.mod_type == IvsModFast)
  {
    ecall_set_mod_state(&ivs.tx.mod, Mod3bit4smp);
    ECALL_MSG_1( ECALL_HIGH, "IVS: fast modulator resumed (NACK count: %d)",
                              ivs.tx.overallNack );
  }
  else if (ivs.tx.mod_type == IvsModRobust)
  {
    ecall_set_mod_state(&ivs.tx.mod, Mod3bit8smp);
    ECALL_MSG_1( ECALL_HIGH, "IVS: robust modulator resumed (NACK count: %d)",
                              ivs.tx.overallNack );
  }
  else
  {
    ECALL_MSG_0( ECALL_HIGH, "IVS: No need to resume modulator");
    return;
  }
  
  /* define uplink parameters */
  ivs.tx.ulDelay = (ivs.tx.ulDelay + ECALL_IVS_TXINC)%ECALL_PCM_LENGTH;
  ivs.tx.ulN     = ECALL_NRF_SYNC + ivs.tx.mod.nfmuteall + ivs.tx.mod.nfdata;
  ivs.tx.ulIndex = 0;
  ivs.tx.rv      = 0;

  ivs.tx.state = IvsSendMsd;

  /* entering new state -> reset other counters */
  ivs.tx.stateIgn[DlMsgStart] = 0;
  ivs.tx.stateCnt[DlMsgNack]	= 0;
  ivs.tx.stateCnt[DlMsgSend]	= 0;
}

/*==========================================================================

  FUNCTION    ECALL_MODEM_IVS_START_MSG

  DESCRIPTION             
      Int16 metric    -> receiver metric (-1: ignore symbol)      

  DEPENDENCIES
    None.

  RETURN VALUE
    None.            
 
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_modem_ivs_start_msg
(
  int32 metric
)
{
  int16 k=0;

  if (metric < 0 && ivs.tx.stateIgn[DlMsgStart] < ECALL_IVS_GOSTART)
  {
    /* ignore, message reliability too low */
    ivs.tx.stateIgn[DlMsgStart]++;
    ECALL_MSG_0( ECALL_HIGH, "IVS: received START !> ignoring message (reliability too low)" );
    return;
  }

  if (ivs.tx.pendingStart == TRUE)
  {
    ivs.tx.pendingStart = FALSE;
    ivs.tx.stateCnt[DlMsgStart] = ECALL_LOCK_START_DL;
  } 
  else 
  {
    /* IVS goes into ECALL_MODEM_RCVD_MSD_DATA state when we get MSD from APP on additional data request
       we can ignore the start frames so that we don't exceed the count ECALL_LOCK_START, 
       since IVS already received 3 START messages */
    ivs.tx.stateCnt[DlMsgStart]++;
  }

  /* check start... */
  if(ivs.tx.stateCnt[DlMsgStart] > ECALL_LOCK_START_DL + ECALL_FAIL_RESTART &&
    (ivs.tx.stateCnt[DlMsgStart]-1)%ECALL_FAIL_RESTART == 0)
  {
    ECALL_MSG_1( ECALL_HIGH, "IVS: received START (metric %d) too often starting over",
                              metric );

    /* assume PSAP sync failure, starting over... */
    ivs.tx.ulDelay   = (ivs.tx.ulDelay + ECALL_IVS_TXINC)%ECALL_PCM_LENGTH;
    ivs.tx.ulN     = ECALL_NRF_SYNC + ivs.tx.mod.nfmuteall + ivs.tx.mod.nfdata;
    ivs.tx.ulIndex = 0;
    ivs.tx.rv      = 0;

    /* clear counter for unreliable START messages */
    ivs.tx.stateIgn[DlMsgStart] = 0;
    
  }
  else if (ivs.tx.stateCnt[DlMsgStart] == ECALL_LOCK_START_DL ) 
  { 
      ECALL_MSG_1( ECALL_HIGH, "IVS: received START (metric %d), starting...",
                                metric );

    if (ecall_modem_ivs.is_msd_received_from_app == TRUE)
      {
      ecall_ivs_status.tx_status = ECALL_IVS_START_RECEIVED_MSD;
    }
    else
    {
      ecall_ivs_status.tx_status = ECALL_IVS_START_RECEIVED_NO_MSD;
    }
    ecall_post_client_notify();

      /* For mulitple MSD transmission if the modem already has the MSD,
         it has to register TX and transmit MSD */
      #if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
      if (ecall_modem_ivs.voc_pending_register == TRUE)
      {
        ECALL_MSG_2( ECALL_HIGH, "Pending voc_register exists, not register rx/tx voc (rx:%d,tx:%d)",
                                  ecall_modem_ivs.voc_rx_registered,
                                  ecall_modem_ivs.voc_tx_registered );
      }
      else
      {
        ecall_voc_register(ECALL_VOC_MODE_RX_TX);
      }
      #else /* Legacy Vocoder */
      if(ecall_voc_tx_register() == FALSE)
      {
        ecall_ivs_status.tx_status = ECALL_IVS_ERR;
        ecall_post_client_notify();               
        return;
      }
      #endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */
     
    /* downlink sync locked, start sending */
    ECALL_MSG_1( ECALL_HIGH, "IVS: Starting RV %d",
                              ivs.tx.rv );

    /* choose type of uplink modulator */
    if (ivs.tx.overallNack < ECALL_IVS_TXFAST) 
    {
      ivs.tx.mod_type = IvsModFast;
      ecall_set_mod_state(&ivs.tx.mod, Mod3bit4smp);
      ECALL_MSG_1( ECALL_HIGH, "IVS: fast modulator chosen (NACK count: %d)",
                                ivs.tx.overallNack );

      ecall_ivs_status.tx_status = ECALL_IVS_MSD_FAST_MODE;
      ecall_post_client_notify(); 
    }
    else
    {
      ivs.tx.mod_type = IvsModRobust;
      ecall_set_mod_state(&ivs.tx.mod, Mod3bit8smp);
      ECALL_MSG_1( ECALL_HIGH, "IVS: robust modulator chosen (NACK count: %d)",
                                ivs.tx.overallNack );

      ecall_ivs_status.tx_status = ECALL_IVS_MSD_ROBUST_MODE;
      ecall_post_client_notify(); 
    }
    /* define uplink parameters */
    ivs.tx.ulDelay   = (ivs.tx.ulDelay + ECALL_IVS_TXINC)%ECALL_PCM_LENGTH;
    ivs.tx.ulN     = ECALL_NRF_SYNC + ivs.tx.mod.nfmuteall + ivs.tx.mod.nfdata;
    ivs.tx.ulIndex = 0;
    ivs.tx.rv      = 0;

    ivs.tx.state = IvsSendMsd;      /*lint !e641*/

    /* entering new state -> reset other counters */
    ivs.tx.stateIgn[DlMsgStart] = 0;
    ivs.tx.stateCnt[DlMsgNack]  = 0;
    ivs.tx.stateCnt[DlMsgSend]  = 0;       
  } 
  else
  {
    /* preserve state */
    ECALL_MSG_1( ECALL_HIGH, "IVS: received START (metric %d)\n",
                              metric );
  }

  if(ivs.tx.stateCnt[DlMsgAck] == 1)
  {
    /* START after single ACK -> assume ACK was incorrect */
    ivs.tx.stateCnt[DlMsgAck]   = 0;
    ECALL_MSG_0( ECALL_HIGH, "[IVS ] ... !> assuming previous ACK was wrong" );
  }
  if ((ivs.tx.stateCnt[DlMsgHlack] + ivs.tx.stateIgn[DlMsgHlack]) >= 1
          && ivs.tx.dlMsgOld < ECALL_SET_LLMSG)
  {
    /* START after single HLACK -> assume HLACK was incorrect */
    for (k = ECALL_SET_LLMSG; k <= ECALL_SET_LLMSG + ECALL_SET_HLMSG; k++)
    {
      ivs.tx.stateCnt[k] = 0;
      ivs.tx.stateIgn[k] = 0;
    }
    ECALL_MSG_0( ECALL_HIGH, "[IVS ] ... !> assuming previous HLACK was wrong" );
  }
}

/*==========================================================================

  FUNCTION    ECALL_MODEM_IVS_NACK_MSG

  DESCRIPTION             
      Int16 metric    -> receiver metric (-1: ignore symbol)      

  DEPENDENCIES
    None.

  RETURN VALUE
    None.            
 
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_modem_ivs_nack_msg
(
  int32 metric
)
{
  int16 k=0;

  if (ivs.tx.pendingStart == TRUE ||
      ivs.tx.state == IvsAck || ivs.tx.state == IvsHlack)
  {
    /* downlink sync locked, still waiting to start */
    ECALL_MSG_0( ECALL_HIGH, "IVS: received NACK  !> ignoring message (expecting START)" );
    return;
  }
  else if (metric < 0) 
  {
    /* ignore, message reliability too low */
    ECALL_MSG_0( ECALL_HIGH, "IVS: received NACK  !> ignoring message (reliability too low)" );
    return;
  }

  /* message seems reliable, increase counters */
  ivs.tx.stateCnt[DlMsgNack]++;
  ivs.tx.overallNack++;

  if (ivs.tx.stateCnt[DlMsgNack] == 1 &&
      ivs.tx.stateCnt[DlMsgSend] < 0)
  {
    /* ignore, NACK should only follow START or NACK */
    ivs.tx.stateCnt[DlMsgNack] = 0;
    ECALL_MSG_0( ECALL_HIGH, "IVS: received NACK  !> ignoring message (assume IDLE)" );
  }
  else if (ivs.tx.stateCnt[DlMsgNack] == 1) 
  {
    ECALL_MSG_1( ECALL_HIGH, "IVS: received NACK  (metric %d), 1st message",
                              metric );
    /* entering new state -> reset other counters */
    ivs.tx.stateIgn[DlMsgStart] = 0;
    ivs.tx.stateCnt[DlMsgStart] = 0;
    ivs.tx.stateCnt[DlMsgSend]  = 0;        

    ecall_ivs_status.tx_status = ECALL_IVS_NACK_RECEIVED;
    ecall_post_client_notify(); 
  } 
  else
  {
    /* preserve state */
    ECALL_MSG_1( ECALL_HIGH, "IVS: received NACK  (metric %d)",
                              metric );
    ecall_ivs_status.tx_status = ECALL_IVS_NACK_RECEIVED;
    ecall_post_client_notify(); 
  }
  if (ivs.tx.stateCnt[DlMsgAck] == 1)
  {
    /* NACK after single ACK -> assume ACK was incorrect */
    ivs.tx.stateCnt[DlMsgAck]   = 0;
    ECALL_MSG_0( ECALL_HIGH, "[IVS ] ... !> assuming previous ACK was wrong" );
  }
  if ((ivs.tx.stateCnt[DlMsgHlack] + ivs.tx.stateIgn[DlMsgHlack]) >= 1
       && ivs.tx.dlMsgOld < ECALL_SET_LLMSG)
  {
    /* NACK after single HLACK -> assume HLACK was incorrect */
    for (k = ECALL_SET_LLMSG; k <= ECALL_SET_LLMSG + ECALL_SET_HLMSG; k++)
    {
      ivs.tx.stateCnt[k] = 0;
      ivs.tx.stateIgn[k] = 0;
    }
    ECALL_MSG_0( ECALL_HIGH, "[IVS ] ... !> assuming previous HLACK was wrong" );
  }
}


/*==========================================================================

  FUNCTION    ECALL_MODEM_IVS_ACK_MSG

  DESCRIPTION             
      Int16 metric    -> receiver metric (-1: ignore symbol)      

  DEPENDENCIES
    None.

  RETURN VALUE
    None.            
 
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_modem_ivs_ack_msg
(
  int32 metric
)
{
  int16 k=0;

  if (ivs.tx.pendingStart == TRUE || ivs.tx.state == IvsHlack) 
  {
    /* downlink sync locked, still waiting to start */
    ECALL_MSG_0( ECALL_HIGH, "[IVS]: received ACK   !> ignoring message (expecting START)" );
    return;
  }
  else if (ivs.tx.state == IvsAck)
  {
    /* ACK already recorded */
    ECALL_MSG_0( ECALL_HIGH, "[IVS] received ACK   !> ignoring message already recorded" );
    return;      
  }
  else if (metric < 0)
  {
    /* ignore, message reliability too low */
    ECALL_MSG_0( ECALL_HIGH, "[IVS]: received ACK   !> ignoring message (reliability too low)" );
    return;
  }

  /* message seems reliable, two ACK messages required */
  ivs.tx.stateCnt[DlMsgAck]++;

  if (ivs.tx.stateCnt[DlMsgAck] == 1 && 
      ivs.tx.stateCnt[DlMsgSend] < 0)
  {
    /* ignore, ACK should only follow ACK or NACK */
    ivs.tx.stateCnt[DlMsgAck] = 0;
    ECALL_MSG_0( ECALL_HIGH, "IVS: received ACK   !> ignoring message (assume IDLE)" );

  }
  else if (ivs.tx.stateCnt[DlMsgAck] == ECALL_NUMACK_RECVD)
  {
    ECALL_MSG_1( ECALL_HIGH, "IVS: received  2 ACKs   (metric %d),wait for HLACK.",
                              metric );
    
    /* enter ACK state, wait for HLACK */
    ecall_ivs_status.tx_status = ECALL_IVS_TX_COMPLETED;
    ecall_post_client_notify(); 

    ivs.tx.state = IvsAck; 
    /* invoke callback functions and wait for HLACK */
    //IvsReceiveAck();

    /* entering new state -> reset all counters */
    for (k = 0; k < ECALL_SET_LLMSG; k++) 
    {
      ivs.tx.stateCnt[k] = 0;
      ivs.tx.stateIgn[k] = 0;
    }
    ivs.tx.stateCnt[DlMsgSend] = -1;
    ivs.tx.overallNack = 0;
  }
  else
  {
    /* waiting for second ACK... */
    ECALL_MSG_2( ECALL_HIGH, "IVS: received ACK   (metric %d), DlMsgAck = %d waiting...",
                              metric,
                              ivs.tx.stateCnt[DlMsgAck] );
    ecall_ivs_status.tx_status = ECALL_IVS_ACK_RECEIVED;
    ecall_post_client_notify();     
  }

  if ((ivs.tx.stateCnt[DlMsgHlack] + ivs.tx.stateIgn[DlMsgHlack]) >= 1
          && ivs.tx.dlMsgOld < ECALL_SET_LLMSG)
  {
    /* ACK after single HLACK -> assume HLACK was incorrect */
    for (k = ECALL_SET_LLMSG; k <= ECALL_SET_LLMSG + ECALL_SET_HLMSG; k++)
    {
      ivs.tx.stateCnt[k] = 0;
      ivs.tx.stateIgn[k] = 0;
    }
    ECALL_MSG_0( ECALL_HIGH, "[IVS ] ... !> assuming previous HLACK was wrong");
  }
}
/*==========================================================================

  FUNCTION    ECALL_MODEM_IVS_HLACK_MSG

  DESCRIPTION
      In:  Int16 msg  -> new control message symbol                         
      Int16 metric    -> receiver metric (-1: ignore symbol)      

  DEPENDENCIES
    None.

  RETURN VALUE
    None.            
 
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_modem_ivs_hlack_msg
(
  int16 msg,
  int32 metric
)
{
  int16 k=0;
  
  /* higher-layer message symbol */
  uint8 dlHlackData = (uint8)(msg - ECALL_SET_LLMSG);

  ecall_ivs_status.hlack_data = dlHlackData;

  /* added addtional check for msg to fix klocwork errors */
  if (metric < 0 && msg <= (ECALL_SET_LLMSG + ECALL_SET_HLMSG))
  {
    ivs.tx.stateIgn[msg]++;
    ivs.tx.stateIgn[DlMsgHlack]++;
  }
  else if(msg <= (ECALL_SET_LLMSG + ECALL_SET_HLMSG))
  {
    ivs.tx.stateCnt[msg]++;
    ivs.tx.stateCnt[DlMsgHlack]++;
  }
  else
  {
    ECALL_MSG_1( ECALL_ERROR, "incorrect msg symbol %d ",
                               msg );
  }

  if((dlHlackData & 0x01) != 0)
  {
    ECALL_MSG_0( ECALL_ERROR, "HLACK data format version bit not supported. version 0" );
  }

  /* check conditions for successful HLACK reception */
  if (ivs.tx.state == IvsHlack)
  {
    ECALL_MSG_2( ECALL_HIGH, "[IVS ] ignoring additional HLACK (data: 0x0%X, metric: %3d)",
                              dlHlackData,
                              metric );
  }
  else if((msg <= ECALL_SET_LLMSG + ECALL_SET_HLMSG) &&
       ivs.tx.stateCnt[msg] >= 1 &&
       ivs.tx.stateCnt[msg]*2 + ivs.tx.stateIgn[msg] >= 4)
  {
    /* Verify if the second bit is 0 -Positive ACK or 1 - Cleardown*/
    /* Invoke callback function and stop */
    ECALL_MSG_1( ECALL_MED, "dlHlackData = %d",
                             dlHlackData );
    if((dlHlackData & 0x02) == 0)
    {
      ECALL_MSG_1( ECALL_HIGH, "HLACK ECALL_IVS_SESSION_COMPLETED  %d",
                                dlHlackData );
      ecall_ivs_status.tx_status  = ECALL_SESSION_COMPLETED;
    }
    else
    {
      ECALL_MSG_1( ECALL_HIGH, "HLACK ECALL_IVS_CLEARDOWN_RECEIVED  %d",
                                dlHlackData );
      ecall_ivs_status.tx_status  = ECALL_IVS_CLEARDOWN_RECEIVED;
    }
    ecall_modem_ivs_event_report(EVENT_ECALL_SESSION_COMPLETE);
    ecall_post_client_notify();  

    ECALL_MSG_2( ECALL_HIGH, "[IVS ] received HLACK (data: 0x0%X, metric: %3d), finished.",
                              dlHlackData,
                              metric );
    
    ivs.tx.state = IvsHlack;
    
    /* entering IDLE state -> reset all counters */
    for (k = 0; k <= ECALL_SET_LLMSG + ECALL_SET_HLMSG; k++)
    {
      ivs.tx.stateCnt[k] = 0;
      ivs.tx.stateIgn[k] = 0;
    } 
    ivs.tx.stateCnt[DlMsgSend] = -1;
    ivs.tx.overallNack = 0;
  }
  else
  {
    ecall_ivs_status.tx_status  = ECALL_IVS_HLACK_RECEIVED;
    ecall_post_client_notify(); 

    ECALL_MSG_2( ECALL_HIGH, "[IVS ] received HLACK (data: 0x0%X, metric: %3d), waiting...",
                              dlHlackData,
                              metric );
  }
   
}

/*==========================================================================

  FUNCTION    ECALL_IVS_TX_STATE

  DESCRIPTION
  

  DEPENDENCIES
    None.

  RETURN VALUE
      In:  Int16 msg  -> new control message symbol                         
      Int16 metric    -> receiver metric (-1: ignore symbol)                
      Bool  syncLock  -> indicates sync lock of control receiver           


  SIDE EFFECTS
    None

===========================================================================*/
void ecall_ivs_tx_state
( 
  int16 msg,
  int32 metric,
  ecall_sync_lock_e_type syncLock
) 
{
  if (syncLock == ECALL_SYNC_LOCKED ) 
  {
    ivs.tx.pendingStart = TRUE;
  }

  if (msg >= (ECALL_SET_LLMSG + ECALL_SET_HLMSG))
  {
    ECALL_MSG_1( ECALL_ERROR, "message symbol %d out of range",
                               msg );
  }
  else if (msg >= ECALL_SET_LLMSG)
  {
    /* Process the HLACK message */
    ecall_modem_ivs_hlack_msg(msg, metric);
  } 
  else
  {
    /* lower-layer message symbol */
    switch (msg)
    {
      case DlMsgStart:
        ecall_modem_ivs_start_msg(metric);   
        break;

      case DlMsgNack:
        ecall_modem_ivs_nack_msg(metric);
        break;

      case DlMsgAck:
        ecall_modem_ivs_ack_msg(metric);
        break;

      default:
        if (ivs.tx.pendingStart == TRUE)
        {
          /* downlink sync locked, still waiting to start */
          ECALL_MSG_0( ECALL_HIGH, "IVS: received IDLE  !> ignoring message (expecting START)" );
        }
        else if ( ivs.tx.stateCnt[DlMsgStart] > 0 ||
                  ivs.tx.stateCnt[DlMsgNack] > 0 ||
                  ivs.tx.stateCnt[DlMsgAck] > 0)
        {
          ECALL_MSG_0( ECALL_HIGH, "IVS: received IDLE  !> ignoring message" );
        }
        else
        {
          ECALL_MSG_0( ECALL_HIGH, "IVS: remaining in IDLE mode" );
        }
        break;
    }
  }
  /* store message if reliable */
  if (metric > 0) {
    ivs.tx.dlMsgOld = msg;
  }
}

/*==========================================================================

  FUNCTION    ECALL_IVS_RX_PROCESS

  DESCRIPTION
      Process the received PCM frame and set the TX IVS state or continue to
      receive downlink data

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_ivs_rx_process
(
  const int16 *pcm,
  uint32 pcm_size
) 
{
  int16 i =0;
  ecall_ctrl_port *port = &ivs.rx.ctrl.port;
  uint32 remaining_memCtrl_size;

  /* IVS receiver, copy PCM data to input buffer */
  remaining_memCtrl_size = sizeof(ivs.rx.memCtrl); 
  for (i = 0; i < ECALL_NRF_MEMCTRL-1; i++)
  {
    ecall_memscpy ( &ivs.rx.memCtrl[i*ECALL_PCM_LENGTH], remaining_memCtrl_size,
                    &ivs.rx.memCtrl[(i+1)*ECALL_PCM_LENGTH], ECALL_PCM_LENGTH_BYTES );
    remaining_memCtrl_size -= ECALL_PCM_LENGTH_BYTES;
  }
  if (remaining_memCtrl_size < pcm_size )
  {
    ECALL_MSG_2( ECALL_ERROR, "remaining_memCtrl_size %d < pcm_size %d",
                               remaining_memCtrl_size,
                               pcm_size );
    return;
  }
  ecall_memscpy ( &ivs.rx.memCtrl[(ECALL_NRF_MEMCTRL-1)*ECALL_PCM_LENGTH], remaining_memCtrl_size, 
                  pcm, pcm_size );

  /* IVS receiver, run control message receiver */
  ecall_ctrl_rx_process(&ivs.rx.ctrl, pcm);

  /* check for uplink SEND trigger */
  switch (ivs.rx.state)
  {
    case IvsTrigger:
      if (port->state == CtrlRxIdle) {
        port->state = CtrlTxIdle;

        /* set control data */
        ivs.rx.state    = IvsStart;
        ivs.rx.dlData   = DlMsgSend;
        ivs.rx.dlMsgCnt = 0;
        ivs.rx.dlIndex  = 0;
        ECALL_MSG_0( ECALL_HIGH, "[IVS ] IvsTrigger" );
      } else {
        /* SEND trigger not allowed during MSD transmission */
        ivs.rx.state = IvsIdle;
        ECALL_MSG_0( ECALL_HIGH, "[IVS ] SendStart during MSD transmission, ignored." );
      }
      break;

    case IvsStart:
      if (port->state == CtrlRxSync || ivs.rx.dlMsgCnt >= ECALL_IVS_NUMSEND)
      {
        /* stop SEND trigger */
        ivs.rx.state    = IvsIdle;
        ivs.rx.dlData   = DlMsgNoop;
        ivs.rx.dlMsgCnt = -1;
        ivs.rx.dlIndex  = -1;
      } 
      else
      {
        /* sending control data */
        port->state        = CtrlTxSend;
        port->u.tx.dlData  = ivs.rx.dlData;
        port->u.tx.dlIndex = ivs.rx.dlIndex;

        if (ivs.rx.dlIndex++ == 0) {
          ECALL_MSG_0( ECALL_HIGH, "[IVS ] sending  SEND" );
        }
        else if (ivs.rx.dlIndex == ECALL_NRF_DLCHUNK) {
          ivs.rx.dlIndex = 0;
          ivs.rx.dlMsgCnt++;
        }
      }
      break;
    default:
      /* do nothing but receiving downlink data */
      break;
  }
}

/*==========================================================================

  FUNCTION    ECALL_IVS_TX_PROCESS

  DESCRIPTION
    Puts the PCM frame to be sent into the pcm_buf queue

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_ivs_tx_process
(
  void
)
{
  
  /*lint --e{641}*/
  uint16 lpcm = sizeof(int16)*ECALL_PCM_LENGTH;
  ecall_ctrl_tx_data *ctrl = &ivs.tx.ctrl;
  ecall_pcm_buf_type  *pcm_buf;
  int16               pcm_buffer[ECALL_PCM_LENGTH];

  /* Initial pcm_buffer to zero */
  memset(pcm_buffer, 0, lpcm);

  if (ctrl->port.state == CtrlTxIdle)
  {
    /* SEND trigger initiated */
  }
  else if (ctrl->port.state == CtrlTxSend)
  {
    /* transmit control data on uplink */
    ecall_ctrl_tx_process(ctrl, pcm_buffer, lpcm);
  }
  else 
  {
    /* sync loss may trigger a reset */
    if (ctrl->port.u.rx.dlData == DlMsgReset)
    {
      ECALL_MSG_1( ECALL_HIGH, "[IVS ] full reset (no sync detected) RV %d",
                                ivs.tx.rv );
      ecall_modem_ivs_event_report(EVENT_ECALL_SESSION_RESET);
        
      ecall_ivs_status.tx_status  = ECALL_SESSION_RESET;
      ecall_post_client_notify();

      ecall_ivs_reset();
    }
    else if (ctrl->port.u.rx.dlData != DlMsgNoop) {
      /* run state machine based on new downlink symbol */
      ecall_ivs_tx_state(ctrl->port.u.rx.dlData, ctrl->port.u.rx.dlMetric,
                        (ctrl->port.state == CtrlRxLock) ? ECALL_SYNC_LOCKED : ECALL_SYNC_UNLOCK);
    }

    /* run transmitter based on IVS state */
    if (ivs.tx.state == IvsIdle ||
        ivs.tx.state == IvsAck  || ivs.tx.state == IvsHlack)
    {
      /* do nothing */
    }
    else if (ivs.tx.state == IvsSendMsd && ivs.tx.mod.type != ModUndef)
    { 
    
      if (ivs.tx.ulIndex < ivs.tx.ulN)
      {
        /* copy frame to PCM vector */
        ecall_ivs_transmitter(&ivs.tx.mod, ivs.tx.memCode, pcm_buffer, lpcm,
        ivs.tx.rv, ivs.tx.ulIndex);
        ivs.tx.ulIndex++;
       }
      else
      {
        /* increase redundancy version counter */
        ivs.tx.rv++;
        ivs.tx.ulIndex = 0;

        ECALL_MSG_1( ECALL_HIGH, "IVS: Starting RV %d",
                                  ivs.tx.rv );

        /* no sync frame is transmitted next */
        ivs.tx.ulN = ivs.tx.mod.nfmuteall + ivs.tx.mod.nfdata;
  
        /* new redundancy version, copy first frame to PCM vector */
        ecall_ivs_transmitter(&ivs.tx.mod, ivs.tx.memCode, pcm_buffer, lpcm,
        ivs.tx.rv, ivs.tx.ulIndex);
        ivs.tx.ulIndex++;
      }
      /* randomize transmit start */
      ecall_memscpy (ivs.tx.memDelay, sizeof(ivs.tx.memDelay),
                     &ivs.tx.memDelay[ECALL_PCM_LENGTH], ECALL_PCM_LENGTH_BYTES);

      ecall_memscpy (&ivs.tx.memDelay[ECALL_PCM_LENGTH], sizeof(ivs.tx.memDelay),
                     pcm_buffer, lpcm);

      ecall_memscpy (pcm_buffer, lpcm, 
                     &ivs.tx.memDelay[ECALL_PCM_LENGTH - ivs.tx.ulDelay], ECALL_PCM_LENGTH_BYTES);
    }
  }

  /* Put pcm into queue only if voc TX is registered */
  if (ecall_modem_ivs.voc_tx_registered == TRUE)
  {
    /* Get a PCM buffer and fill it with TX frame*/
    if((pcm_buf = ecall_get_pcm_buf()) == NULL)
    {
      ECALL_MSG_0( ECALL_ERROR, "No PCM buffers for tx" );
      return;
    }

    ecall_memscpy(pcm_buf->pcm, lpcm, pcm_buffer, lpcm);

    /*Put the PCM buffer into the queue*/
    ecall_put_pcm_buf(pcm_buf); 
  }
  
}

/*==========================================================================

  FUNCTION    ECALL_IVS_PROCESS

  DESCRIPTION
     Process the RX and TX pcm frames to/from the vocoder

  DEPENDENCIES
    None.

  RETURN VALUE
    InOut:  int16* pcm  <-> input and output frame of 16bit PCM samples        

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_ivs_process
(
  int16 *pcm,
  uint32 pcm_size
)
{
  ecall_ctrl_port *prx = &ivs.rx.ctrl.port;
  ecall_ctrl_port *ptx = &ivs.tx.ctrl.port;
  int16 i =0;

  /* invert input signal if required */
  if (prx->invert == TRUE) {
    for (i = 0; i < ECALL_PCM_LENGTH; i++) {
      pcm[i] = (pcm[i] > ECALL_PCM_MIN) ? -pcm[i] : ECALL_PCM_MAX;
    }
  }

  ecall_ivs_rx_process ( pcm, pcm_size );
  if (prx->state == CtrlTxIdle || prx->state == CtrlTxSend) {
     /* control transmit mode */
     ptx->state        = prx->state;
     ptx->u.tx.dlData  = prx->u.tx.dlData;
     ptx->u.tx.dlIndex = prx->u.tx.dlIndex;
  }
  else {
     /* control receive mode */
     ptx->state         = prx->state;
     ptx->u.rx.dlData   = prx->u.rx.dlData;
     ptx->u.rx.dlMetric = prx->u.rx.dlMetric;
  }
  ecall_ivs_tx_process();

  /* invert output signal if required */
  if (ptx->invert == TRUE) {
    for (i = 0; i < ECALL_PCM_LENGTH; i++) {
      pcm[i] = (pcm[i] > ECALL_PCM_MIN) ? -pcm[i] : ECALL_PCM_MAX;
    }
  }
}


/*==========================================================================
                   IVS external reference 
===========================================================================*/

/*==========================================================================

  FUNCTION    IVSGETDATA

  DESCRIPTION
    main synchronization function

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
ecall_ivs_data* ecall_ivs_getData
(
  void
)
{ 
  return &ivs; 
}


/*==========================================================================

  FUNCTION    ECALL_MODEM_IVS_INIT

  DESCRIPTION
     Initialise queues and defualt values for the global data. 
     Gets the eCall operating mode from the sim.

  DEPENDENCIES
    None.

  RETURN VALUE
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_modem_ivs_init
(
  void
)
{
   uint8 i;
  
   /* Initialize outgoing PCM queues */
   (void) q_init(&ecall_pcm_q);
   (void) q_init(&ecall_pcm_free_q);

   for (i = 0; i < ECALL_PCM_BUF_COUNT; i++)
   {
     (void) q_link(&ecall_pcm_buf[i], &ecall_pcm_buf[i].link);
      q_put(&ecall_pcm_free_q, &ecall_pcm_buf[i].link);
   }

   /* Initialize the IVS Modem UL and DL data to all 0 */
   memset(&ivs, 0, sizeof(ivs));

   /* Reset IVS Rx & Tx settings */
   ecall_ivs_reset();
  
   ecall_modem_ivs.voc_rx_registered    = FALSE;
   ecall_modem_ivs.voc_tx_registered    = FALSE;
   ecall_modem_ivs.drop_tx_pcm_count    = 0;
   ecall_modem_ivs.missed_rx_pcm_count  = 0;
   ecall_modem_ivs.voc_config_ecall     = FALSE;
   ecall_modem_ivs.is_msd_received_from_app = FALSE;

   #if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
   ecall_modem_ivs.smem_mapped          = FALSE;
   ecall_modem_ivs.smem_allocated       = FALSE;
   ecall_modem_ivs.voc_re_register      = FALSE;
   ecall_modem_ivs.voc_pending_register = FALSE;
   ecall_modem_ivs.ecall_rx_in_buf_addr      = 0;
   ecall_modem_ivs.ecall_rx_out_buf_addr     = 0;
   ecall_modem_ivs.ecall_tx_in_buf_addr      = 0;
   ecall_modem_ivs.ecall_tx_out_buf_addr     = 0;
   ecall_modem_ivs.ecall_tx_in_buf_phy_addr  = 0;
   ecall_modem_ivs.ecall_tx_out_buf_phy_addr = 0;
   ecall_modem_ivs.ecall_rx_in_buf_phy_addr  = 0;
   ecall_modem_ivs.ecall_rx_out_buf_phy_addr = 0;
   #endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */

} /* ECALL_MODEM_IVS_INIT */

/*===========================================================================

FUNCTION ECALL_IVS_PROCESS_START

DESCRIPTION
  This would be called by the ivs task when the app call starts_transmit_data
  a command is queued up in IVS_TASK to process the START_TRANSMIT_DATA

DEPENDENCIES
  None

RETURN VALUE
  On error conditions it calls the APP registered callback indicating
  an error occured in the modem

SIDE EFFECTS
  None

===========================================================================*/
void ecall_ivs_process_start
(
  const uint8 *ecall_msd
)
{
  
  ECALL_MSG_1( ECALL_HIGH, "[IVS] Rx tx_mode = %d in ecall_ivs_process_start()",
                             ecall_modem_ivs.tx_mode );

  ecall_modem_ivs.is_msd_received_from_app = TRUE;

  /*Configure the vocoder for eCall whenever ECALL_START_TRANSMIT_DATA 
    command is posted from APP*/

  ecall_voc_configure();

  if(ecall_modem_ivs.voc_rx_registered == FALSE)
  {
    /* Reset IVS settings */
    ecall_ivs_reset();
  }

  ECALL_MSG_0( ECALL_HIGH, "[IVS ] SendStart received from control----" );

  if (ecall_modem_ivs.tx_mode == PUSH_MODE)
  {
    ECALL_MSG_0( ECALL_HIGH, "[ECALL IVS ] Setting to PUSH_MODE ----" );

    #if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )

    /* Register vocoder with both RX/TX mode */
    ecall_voc_register(ECALL_VOC_MODE_RX_TX);

    #else /* Legacy Vocoder */

    /* Register the RX with the vocoder */
    if(ecall_voc_rx_register() == FALSE)
    { 
      ecall_ivs_status.tx_status  = ECALL_IVS_ERR;
      ecall_post_client_notify();
      return;
    }
    /* Register the TX with the vocoder for PUSH mode*/
    if((ecall_voc_tx_register() == FALSE))
    {
      ecall_ivs_status.tx_status  = ECALL_IVS_ERR;
      ecall_post_client_notify();
      return;
    }
    #endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */

    ivs.rx.state = IvsTrigger;
  }
  else if (ecall_modem_ivs.tx_mode == PULL_MODE)
  {
    ECALL_MSG_0( ECALL_HIGH, "[ECALL IVS ] Setting to PULL_MODE  ---" );

    #if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )

    /* Register vocoder with RX only mode */
    ecall_voc_register(ECALL_VOC_MODE_RX_TX);

    #else /* Legacy Vocoder */

    if(ecall_voc_rx_register() == FALSE)
    { 
      ecall_ivs_status.tx_status  = ECALL_IVS_ERR;
      ecall_post_client_notify();
      return;
    }
    /* Register the TX with the vocoder for PULL mode*/
    if((ecall_voc_tx_register() == FALSE))
    {
      ecall_ivs_status.tx_status  = ECALL_IVS_ERR;
      ecall_post_client_notify();
      return;
    }

    #endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */

    ivs.rx.state = IvsIdle;
  }

  ecall_modem_encode_msd(ecall_msd, ECALL_MSD_MAX_LENGTH); 

  ecall_modem_ivs_resume_modulator();

}

/*===========================================================================

FUNCTION ECALL_IVS_PROCESS_ABORT_TX

DESCRIPTION
   This function would abort the transmission and de-register the TX Input
   for regular voice call. Discarding the current MSD or not is left for the
   application to decide

DEPENDENCIES
  None

RETURN VALUE
    On error conditions it calls the APP registered callback indicating
    an error occured in the modem 

SIDE EFFECTS
  None

===========================================================================*/
void ecall_ivs_process_abort_tx
(
  boolean discard_msd
)
{
  ECALL_MSG_1( ECALL_MED,"process_abort_tx: ivs.tx.state %d",
                          ivs.tx.state );

  if( discard_msd == TRUE )
  {
    ecall_modem_ivs.is_msd_received_from_app = FALSE;
    /* Clear the buffer */
    memset(ivs.tx.memCode,0,ECALL_NRB_CODE_BUFFER);
  }

  if (ivs.tx.state != IvsIdle && ivs.tx.state != IvsHlack)
  {
    ECALL_MSG_1( ECALL_HIGH, "Abort TX with ivs.tx.state(%d), reset IVS",
                              ivs.tx.state );
    ecall_ivs_reset();
  }
  
  #if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
  ecall_voc_register(ECALL_VOC_MODE_RX_ONLY);
  #else /* Legacy Vocoder */
  ecall_voc_tx_unregister();
  if(ecall_modem_ivs.voc_tx_registered == TRUE)
  {
    ecall_ivs_status.tx_status  = ECALL_IVS_ERR;
    ecall_post_client_notify();
  }
  #endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */
  
  ecall_voc_config_restore(); 

  ecall_modem_ivs.drop_tx_pcm_count = 0;

}

/*===========================================================================

FUNCTION ECALL_IVS_PROCESS_ABORT_ALL

DESCRIPTION
   It would abort the transmission and stop monitoring for DL messages.
   De-registers RX and TX with the vocoder.

DEPENDENCIES
  None

RETURN VALUE
    On error conditions it calls the APP registered callback indicating
    an error occured in the modem

SIDE EFFECTS
  None

===========================================================================*/
void ecall_ivs_process_abort_all
(
  void
)
{
   ecall_modem_ivs.drop_tx_pcm_count = 0;
   ecall_modem_ivs.is_msd_received_from_app = FALSE;
  
   /* Clear the buffer */
   memset(ivs.tx.memCode,0,ECALL_NRB_CODE_BUFFER);

   #if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
   ecall_voc_unregister();
   #else /* Legacy Vocoder */
   ecall_voc_tx_unregister();
   ecall_voc_rx_unregister();
   #endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */

   ecall_voc_config_restore();

}


/*===========================================================================

FUNCTION ECALL_START_TRANSMIT_DATA

DESCRIPTION
   It queues up a command to the IVS task to start the eCall MSD TX.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ecall_start_transmit_data
(
  const ecall_msd_array_type ecall_msd, 
  ecall_tx_mode_type  tx_mode
)
{

  ecall_cmd_type *cmd_ptr = NULL;
 

  ecall_modem_ivs.tx_mode = tx_mode;

  cmd_ptr = ecall_get_cmd_buf();

  if (cmd_ptr != NULL)
  {
    /* Post a command to the eCall task to process this PCM frame */
    cmd_ptr->cmd_id         = ECALL_CMD_MODEM_START_TX_MSD;
  
    ECALL_MSG_1( ECALL_HIGH, "ecall_start_transmit_data Rx tx_mode = %d ",
                              tx_mode );

    ecall_memscpy ( cmd_ptr->data.ecall_msd, sizeof(cmd_ptr->data.ecall_msd),
                    ecall_msd,               ECALL_MSD_MAX_LENGTH );

    ecall_put_cmd(cmd_ptr);
  }
  else
  {
    ECALL_MSG_0( ECALL_ERROR, "No empty cmd buffer to queue start_tx_msd" );
  }
}


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
    None 

  SIDE EFFECTS
    None
    
===========================================================================*/

void ecall_abort_transmit_data
(
  boolean discard_msd
)
{
  ecall_cmd_type *cmd_ptr = NULL;

  cmd_ptr = ecall_get_cmd_buf();

  if (cmd_ptr != NULL)
  {
    /* Post a command to the eCall task to process this PCM frame */
    cmd_ptr->cmd_id            = ECALL_CMD_MODEM_ABORT_TX;
    cmd_ptr->data.discard_msd  = discard_msd;
 
    ecall_put_cmd(cmd_ptr);
  }
  else
  {
    ECALL_MSG_0( ECALL_ERROR, "No empty cmd buffer to queue Abort Tx" );
  }

}

/*===========================================================================

  FUNCTION ECALL_ABORT_ALL

  DESCRIPTION
       This would be called by the ivs task when the app call ECALL_ABORT_ALL
       a command is queued up in IVS_TASK to process the ECALL_ABORT_ALL
       This would de-register the tx and rx vocoder. Modem would no longer
       monitor the DL frames. Modem would discard the current MSD.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_abort_all
(                                
  void
)
{

  ecall_cmd_type *cmd_ptr = NULL;

  cmd_ptr = ecall_get_cmd_buf();

  if (cmd_ptr != NULL)
  {
    /* Post a command to the eCall task to process this PCM frame */
    cmd_ptr->cmd_id   =   ECALL_CMD_MODEM_ABORT_ALL;
 
    ecall_put_cmd(cmd_ptr);
  }
  else
  {
    ECALL_MSG_0( ECALL_ERROR, "No empty cmd buffer to queue Abort Tx" );
  }
}


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
)
{
  ecall_ivs_status.tx_status  = ECALL_IVS_ERR;
  ecall_post_client_notify();
}

/*===========================================================================

  FUNCTION ECALL_UPDATE_CM_SUBS_INFO

  DESCRIPTION
      This function will post a command to eCall IVS to inform that CM SUBS
      INFO is available for Dual SIM case.
      
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_update_cm_subs_info
(
  void
)
{
  ecall_cmd_type *cmd_ptr = NULL;

  cmd_ptr = ecall_get_cmd_buf();

  if (cmd_ptr != NULL)
  {
    /* Post a command to the eCall IVS task to inform that CM SUBS info is available */
    cmd_ptr->cmd_id            = ECALL_CMD_UPDATE_CM_SUBS_INFO;
    ecall_put_cmd(cmd_ptr);
  }
  else
  {
    ECALL_MSG_0( ECALL_ERROR, "No empty cmd buffer to queue Abort Tx" );
  }
} /* ECALL_UPDATE_CM_SUBS_INFO */

/*==========================================================================

  FUNCTION ECALL_FEED_RX_PCM

  DESCRIPTION
    This function will feed RX PCM from input to eCall modem whenever the
    voc_timer expires to preserve offset.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

===========================================================================*/
void ecall_feed_rx_pcm
(
  uint16  *pcm,
  uint32  pcm_size
)
{
  ecall_cmd_type *cmd_ptr = NULL;

  if (!ecall_modem_ivs.voc_rx_registered)
  {
    ECALL_MSG_1( ECALL_ERROR, "ecall_feed_rx_pcm() called but not registered with vocoder (registered = %d)!",
                               ecall_modem_ivs.voc_rx_registered );
    return;
  }

  cmd_ptr = ecall_get_cmd_buf();

  if (cmd_ptr != NULL)
  {
    /* Post a command to the eCall task to process this PCM frame */
    cmd_ptr->cmd_id = ECALL_CMD_PROCESS_RX_PCM;

    /* Do not up-scale two bits since we didn't get the pcm from the vocoder */
    ecall_memscpy  ( cmd_ptr->data.pcm, sizeof(cmd_ptr->data.pcm),
                     pcm, pcm_size );

    ECALL_MSG_0( ECALL_MED, "ecall_rx_pcm put_cmd with feeded pcm" );
    ecall_put_cmd(cmd_ptr);
  }
  else
  {
    ECALL_MSG_0( ECALL_ERROR, "Dropped PCM sample; ran out of command buffers" );
  }

} /* ECALL_FEED_RX_PCM */

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
)
{
  uint16 lpcm = sizeof(int16)*ECALL_PCM_LENGTH;

  if (ecall_modem_ivs.voc_rx_registered == FALSE)
  {
    ECALL_MSG_0( ECALL_MED, "voc_timer expires but unregister from vocoder rx" );
    return;
  }

  /* Increment the missed_rx_pcm_count */
  ecall_modem_ivs.missed_rx_pcm_count++;

  ECALL_MSG_1( ECALL_HIGH, "ECALL_IVS_VOC_TIMER_SIG expired, missed rx_pcm %d",
                            ecall_modem_ivs.missed_rx_pcm_count );

  /* Still registered to vocoder rx, reset timer */
  if (ecall_modem_ivs.missed_rx_pcm_count >= 2)
  {
    ecall_ivs_set_voc_timer(ecall_voc_timer_duration[2]);
  }
  else
  {
    ecall_ivs_set_voc_timer(ecall_voc_timer_duration[ecall_modem_ivs.missed_rx_pcm_count]);
  }
    
  /* Initialize dummy PCM data to zero */
  memset(&pcm_buffer, 0, lpcm);

  /* Feed dummy rx_pcm to preserve offset */
  ecall_feed_rx_pcm ( (uint16*)pcm_buffer, sizeof(pcm_buffer) );

} /* ECALL_MODEM_HANDLE_VOC_TIMER_EXPIRE */

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
)
{
    ecall_ivs_status.tx_status = ECALL_IVS_LOSE_SYNC;
    ecall_post_client_notify();
} /* ECALL_MODEM_IVS_REPORT_LOSE_SYNC */

#if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
/*===========================================================================

     A - F a m i l y   &   B - F a m i l y   C o m m o n   F u n c t i o n

===========================================================================*/

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
)
{
  int32 ret;
  ecall_pcm_buf_type *buf_ptr = NULL;

  if(ecall_modem_ivs.voc_rx_registered == FALSE)
  {
    ECALL_MSG_0( ECALL_HIGH, "voc_unregister: Active vocoder session has already been deregistered" );
    return;
  }

  /*-----------------------------------------------------------------------*/
  /*                      Stop the Vocoder PCM session                     */
  /*-----------------------------------------------------------------------*/
  
  ret = __aprv2_cmd_alloc_send(
            ecall_modem_apr_handle,      /* handle       */
            APRV2_PKT_MSGTYPE_SEQCMD_V,  /* msg_type     */
            ecall_modem_apr_src_addr,    /* src_addr     */
            ecall_modem_apr_src_port,    /* src_port     */
            ecall_modem_apr_dest_addr,   /* dst_addr     */
            ecall_modem_apr_dest_port,      /* dst_port     */
            ECALL_APR_IVPCM_STOP,        /* token        */
            VSS_IVPCM_CMD_STOP,          /* opcode       */
            NULL,                        /* payload      */
            0 );                         /* payload_lize */
  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "__aprv2_cmd_alloc_send failed at ecall_voc_session_stop, ret %d",
                               ret );
  }

  /* Move everything back to their free queues */
  while ((buf_ptr = (ecall_pcm_buf_type *) q_get(&ecall_pcm_q)) != NULL)
  {
    q_put(&ecall_pcm_free_q, &buf_ptr->link);
  }

} /* ECALL_VOC_UNREGISTER */
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
    FEATURE_ECALL_HAS_VOICE_A

  RETURN VALUE
    None.

  SIDE EFFECTS
    The vocoder input takes 14 bit linear pcm samples and the 15th,16th bits
    are the sign extension bits. we need to downscale two bits to meet the vocoder format.
 
===========================================================================*/
void ecall_tx_pcm
(
  aprv2_packet_t* packet
)
{
  int32 ret;
  vss_ivpcm_evt_notify_t *notify_evt_ptr;
  vss_ivpcm_evt_push_buffer_t push_buff;
  ecall_pcm_buf_type *buf_ptr = NULL;
  uint32 i = 0;
  uint8 *tx_out_buf_ptr = NULL;
  uint8 *tx_in_buf_ptr = NULL;
  int16 pcm_buffer[ECALL_PCM_LENGTH];

  if(ecall_modem_ivs.voc_tx_registered == FALSE)
  {
    ECALL_MSG_1( ECALL_ERROR, "ecall_tx_pcm() called but not registered with vocoder (registered = %d)!",
                               ecall_modem_ivs.voc_tx_registered );
    return;  
  }

  /* Get the payload */
  if (packet == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "ecall_tx_pcm: packet is NULL" );
    return;
  }
  notify_evt_ptr = APRV2_PKT_GET_PAYLOAD( vss_ivpcm_evt_notify_t, packet );
  if(notify_evt_ptr == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "Failed to get payload at ecall_tx_pcm" );
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
  
  push_buff.out_buf_addr = ecall_modem_ivs.ecall_tx_out_buf_phy_addr;
  push_buff.in_buf_addr = ecall_modem_ivs.ecall_tx_in_buf_phy_addr;
  tx_out_buf_ptr = (uint8*)ecall_modem_ivs.ecall_tx_out_buf_addr;
  tx_in_buf_ptr = (uint8*)ecall_modem_ivs.ecall_tx_in_buf_addr; 

  if (tx_in_buf_ptr == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "tx_pcm: tx_in_buff_ptr = NULL" );
    return;
  }

  if(ecall_modem_ivs.drop_tx_pcm_count > 0)
  {
    if ((buf_ptr = (ecall_pcm_buf_type *) q_get(&ecall_pcm_q)) != NULL)
    {
      /* If IVS sent silence in between the send state drop the next pcm frame to preserve offset*/
      ECALL_MSG_1( ECALL_ERROR, "Dropping PCM frame as silence was sent earlier drop_tx_count%d",
                                 ecall_modem_ivs.drop_tx_pcm_count );

      ecall_modem_ivs.drop_tx_pcm_count--;
      q_put(&ecall_pcm_free_q, &buf_ptr->link);
    }
  }
   
  if ((buf_ptr = (ecall_pcm_buf_type *) q_get(&ecall_pcm_q)) != NULL)
  {
    for (i = 0; i < ECALL_PCM_LENGTH; i++)
    {
      /* The vocoder input takes 14 bit linear pcm samples and the 15th,16th bits are the 
        sign extension bits. we need to downscale two bits to meet the vocoder format*/
      pcm_buffer[i] = (buf_ptr->pcm[i] >> 2);
    }
    ecall_memscpy ( tx_in_buf_ptr, notify_evt_ptr->request_buff_size,
                    pcm_buffer,    sizeof(pcm_buffer) );

    ECALL_MSG_0( ECALL_MED, "ecall_tx_pcm put_cmd" );
    q_put(&ecall_pcm_free_q, &buf_ptr->link);
  }
  else
  {
    /*lint -e641*/
    if(ivs.tx.state == IvsSendMsd)
    {
      /* If the IVS doesn't have a PCM frame to send it will send a silence and 
         drop the next queued up frame to preseve the offset*/
      ecall_modem_ivs.drop_tx_pcm_count++;
    }
    memset(tx_in_buf_ptr, 0, ECALL_PCM_LENGTH_BYTES);
    ECALL_MSG_0( ECALL_MED, "ecall_tx_pcm no data sending silence" );
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

} /* ECALL_TX_PCM */

/*==========================================================================
  FUNCTION ECALL_RX_PCM

  DESCRIPTION
    This function will copy the input data from packet, and post a command to
    process this PCM frame.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A

  RETURN VALUE
    None.

  SIDE EFFECTS
   The vocoder output gives 14 bit linear pcm samples and the 15th,16th bits 
   are the sign extension bits. we need to up-scale two bits to get the actual data.
 
===========================================================================*/
void ecall_rx_pcm
(
  aprv2_packet_t* packet
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

  if(ecall_modem_ivs.voc_rx_registered == FALSE)
  {
    ECALL_MSG_1( ECALL_ERROR, "rx_pcm() called but not registered with vocoder (register = %d)!",
                               ecall_modem_ivs.voc_rx_registered );
    return;
  }

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

  push_buff.out_buf_addr = ecall_modem_ivs.ecall_rx_out_buf_phy_addr;
  push_buff.in_buf_addr = ecall_modem_ivs.ecall_rx_in_buf_phy_addr;
  rx_out_buf_ptr = (uint8*)ecall_modem_ivs.ecall_rx_out_buf_addr;
  rx_in_buf_ptr = (uint8*)ecall_modem_ivs.ecall_rx_in_buf_addr; 

  if(rx_out_buf_ptr == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "rx_pcm: rx_out_buf_ptr is NULL!" );
    return;
  }

  /* Set a 30ms timer to identify a missed rx_pcm() and reset the miss count */
  ecall_ivs_set_voc_timer(ecall_voc_timer_duration[0]);
  ecall_modem_ivs.missed_rx_pcm_count = 0;

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
      ecall_memscpy ( pcm_buffer,     sizeof(pcm_buffer), 
                      rx_out_buf_ptr, notify_evt_ptr->request_buff_size );
      for (i = 0; i < ECALL_PCM_LENGTH; i++)
      {
        
        /* The vocoder output gives 14 bit linear pcm samples and the 15th,16th bits are the 
          sign extension bits. we need to up-scale two bits to get the actual data*/
        cmd_ptr->data.pcm[i] = (pcm_buffer[i] << 2);
      }
    }
    ECALL_MSG_0( ECALL_MED, "ecall_rx_pcm put_cmd" );
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
    Registers eCall functions with Vocoder so that eCall feeds PCM
    frames for output and receives decoded PCM input.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A

  RETURN VALUE
    None

  SIDE EFFECTS
    None
 
===========================================================================*/
void ecall_voc_register
(
  ecall_voc_mode_type voc_mode
)
{
  ECALL_MSG_1( ECALL_MED, "Request to register vocoder for mode: %d (0:RX only, 1:RX-TX)",
                           voc_mode );
  
  if (ecall_modem_apr_dest_port == 0)
  {
    /* Initialize CVS stream to get the dest port
       will call ecall_modem_map_shared_memory after successfully created cvs session */
    ECALL_MSG_1( ECALL_MED, "voc_reg: dest_port is %d. Will init APR then create passive control session to get port",
                             ecall_modem_apr_dest_port );

    /* Initialize APR interface for message routing */
    ecall_modem_apr_init();

    ecall_modem_ivs.voc_re_register_mode = voc_mode;
    ecall_modem_cvs_init();
    return;
  }
  
  if(ecall_modem_ivs.smem_mapped == FALSE)
  {
    /* Map the memory pool for APR interface,
       will call ecall_voc_register again after successfully mapped */
    ECALL_MSG_0( ECALL_MED, "voc_reg: memory not mapped. Will register after successfully mapped");
    ecall_modem_ivs.voc_re_register_mode = voc_mode;
    ecall_modem_map_shared_memory();
    return;
  }

  ecall_modem_ivs.voc_pending_register = TRUE;

  if(voc_mode == ECALL_VOC_MODE_RX_ONLY)
  {
    if(ecall_modem_ivs.voc_rx_registered == TRUE &&
       ecall_modem_ivs.voc_tx_registered == TRUE)
    {
      ECALL_MSG_0( ECALL_HIGH, "Vocoder was configured as RX/TX mode, unregister first" );
      /* Inform ibasic_rsp_result() voc need to re-register
         Will wait for a successful voc unregister, then do the reregister*/
      ecall_modem_ivs.voc_re_register = TRUE;
      ecall_modem_ivs.voc_re_register_mode = ECALL_VOC_MODE_RX_ONLY;
      ecall_voc_unregister();
      return;
    }
    else if(ecall_modem_ivs.voc_rx_registered == FALSE)
    {
      /* Register vocoder for RX only mode */
      ecall_voc_register_rx();
      return;
    } 
  }
  else if (voc_mode == ECALL_VOC_MODE_RX_TX)
  {
    if(ecall_modem_ivs.voc_rx_registered == TRUE &&
       ecall_modem_ivs.voc_tx_registered == FALSE)
    {
      ECALL_MSG_0( ECALL_HIGH, "Vocoder was configured as RX only mode, unregister first" );
      /* Inform ibasic_rsp_result() voc need to re-register
         Will wait for a successful voc unregister, then do the reregister*/
      ecall_modem_ivs.voc_re_register = TRUE;
      ecall_modem_ivs.voc_re_register_mode = ECALL_VOC_MODE_RX_TX;
      ecall_voc_unregister();
      return;
    }
    else if(ecall_modem_ivs.voc_rx_registered == FALSE)
    {
      /* Register vocoder for RX/TX mode */
      ecall_voc_register_rx_tx();
      return;
    }
  }
  else
  {
    ECALL_MSG_1( ECALL_ERROR, "Unsupported vocoder mode %d",
                               voc_mode );
    return;
  }

  ECALL_MSG_1( ECALL_MED, "Vocoder already register for mode %d",
                           ecall_modem_ivs.voc_config_mode );

} /* ECALL_VOC_REGISTER */

/*==========================================================================
  FUNCTION ECALL_VOC_REGISTER_RX_TX

  DESCRIPTION
    Registers eCall functions with Vocoder RX and TX session so that eCall
    feeds PCM frames for output and receives decoded PCM input.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A

  RETURN VALUE
    None

  SIDE EFFECTS
    None
 
===========================================================================*/
void ecall_voc_register_rx_tx
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

  if(ecall_modem_ivs.voc_rx_registered == TRUE)
  {
    ECALL_MSG_1( ECALL_HIGH, "An active vocoder PCM session already exist with mode = %d",
                              ecall_modem_ivs.voc_config_mode );
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
            ECALL_APR_IVPCM_START_RX_TX,    /* token        */
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
    ECALL_MSG_0( ECALL_ERROR, "vocoder_register_rx_tx: dst_packet is NULL" );
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
  remaining_target_payload_len -= sizeof( vss_tap_point_tx );

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

} /* ECALL_VOC_REGISTER_RX_TX */

/*==========================================================================
  FUNCTION ECALL_VOC_REGISTER_RX

  DESCRIPTION
    Registers eCall functions with Vocoder RX so that eCall receives decoded
    PCM input.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A

  RETURN VALUE
    None

  SIDE EFFECTS
    None
 
===========================================================================*/
void ecall_voc_register_rx
(
  void
)
{
  int32 ret;
  vss_ivpcm_tap_point_t vss_tap_point;
  uint32 target_payload_len;
  uint32 remaining_target_payload_len;
  uint32 num_tap_points;
  aprv2_packet_t *dst_packet = NULL;
  uint8_t *dst_payload;

  if(ecall_modem_ivs.voc_rx_registered == TRUE)
  {
    ECALL_MSG_1( ECALL_HIGH, "An active vocoder PCM session already exist with mode = %d",
                              ecall_modem_ivs.voc_config_mode );
    return;
  }
  
  num_tap_points = 1; /* 1 tap point for TX */
  target_payload_len = sizeof(num_tap_points) + num_tap_points*sizeof(vss_ivpcm_tap_point_t);
  remaining_target_payload_len = target_payload_len;

  /* allocate the dest_packet */
  ret = __aprv2_cmd_alloc_ext(
            ecall_modem_apr_handle,         /* handle       */
            APRV2_PKT_MSGTYPE_SEQCMD_V,     /* msg_type     */
            ecall_modem_apr_src_addr,       /* src_addr     */
            ecall_modem_apr_src_port,       /* src_port     */
            ecall_modem_apr_dest_addr,      /* dst_addr     */
            ecall_modem_apr_dest_port,      /* dst_port     */
            ECALL_APR_IVPCM_START_RX_ONLY,  /* token        */
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
    ECALL_MSG_0( ECALL_ERROR, "vocoder_register_rx: dst_packet is NULL" );
    return;
  }
  dst_payload = APRV2_PKT_GET_PAYLOAD(uint8_t, dst_packet);

  /* copy num_tap_points to dst_payload */
  ecall_memscpy ( dst_payload, remaining_target_payload_len,
                  &num_tap_points, sizeof(num_tap_points) );
  dst_payload += sizeof(num_tap_points);
  remaining_target_payload_len -= sizeof(num_tap_points);

  /* Configure RX tap point */
  vss_tap_point.tap_point = VSS_IVPCM_TAP_POINT_RX_DEFAULT;
  vss_tap_point.direction = 0; /* output-only tap point, VSS_IVPCM_TAP_POINT_DIR_OUT */
  vss_tap_point.sampling_rate = VSS_IVPCM_SAMPLING_RATE_8K; 
  vss_tap_point.duration = 0;

  /* copy RX tap point structure to destination payload */
  ecall_memscpy ( dst_payload, remaining_target_payload_len,
                  &vss_tap_point, sizeof(vss_tap_point) );
  
  /* Send the IVPCM_CMD_START command */
  ret = __aprv2_cmd_forward( 
            ecall_modem_apr_handle,  /* handle */
            dst_packet);             /* packet */

  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "__aprv2_cmd_alloc_send failed, ret %d",
                               ret );
  }

} /* ECALL_REGISTER_RX */

/*==========================================================================
  FUNCTION ECALL_MODEM_MAP_SHARED_MEMORY

  DESCRIPTION
    This function will allocate a shared memory region from SMEM pool if not
    allocated yet, and send a map memory command to CVD thru APR.

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

  if (ecall_modem_ivs.smem_mapped == TRUE)
  {
    ECALL_MSG_0( ECALL_ERROR, "map_shared_memory() called but was already mapped!" );
    return;
  }

  if (ecall_modem_ivs.smem_allocated == FALSE)
  {
    /* Allocate SMEM shared memory  */
    ecall_smem_origin_ptr = smem_alloc(SMEM_ECALL, ECALL_SMEM_SIZE);
    if (ecall_smem_origin_ptr == NULL)
    {
      ECALL_MSG_0( ECALL_ERROR, "smem_alloc failed!" );
      return;
    }

    ecall_smem_virtual_addr_orig = (uint32)ecall_smem_origin_ptr;
    ecall_smem_physical_addr_orig = blast_lookup_physaddr(ecall_smem_virtual_addr_orig);
    ecall_smem_get_ptr = smem_get_addr(SMEM_ECALL, &ecall_smem_size);
    ECALL_MSG_2( ECALL_MED, "ecall_smem_get_ptr: 0x%08X, ecall_smem_size: %d", 
                             ecall_smem_get_ptr,
                             ecall_smem_size );
    

    ecall_modem_ivs.smem_allocated = TRUE;
  }

  ecall_smem_virtual_addr = ecall_smem_virtual_addr_orig + (ECALL_SMEM_SIZE - (ecall_smem_virtual_addr_orig % ECALL_SMEM_SIZE) );
  ecall_smem_physical_addr = ecall_smem_physical_addr_orig + (ECALL_SMEM_SIZE - (ecall_smem_physical_addr_orig % ECALL_SMEM_SIZE) );

  /* Initialize the mem_map object */
  mem_map_cmd.phys_addr = ecall_smem_physical_addr;
  mem_map_cmd.mem_size = ECALL_SMEM_SIZE*2;
  mem_map_cmd.mem_pool_id = VSS_ICOMMON_MAP_MEMORY_SHMEM8_4K_POOL;

  memset( (uint8*)ecall_smem_virtual_addr, 0, 4096 );

  ecall_modem_ivs.ecall_tx_out_buf_addr = ecall_smem_virtual_addr;
  ecall_modem_ivs.ecall_tx_in_buf_addr = ecall_smem_virtual_addr + 1000; 
  ecall_modem_ivs.ecall_rx_out_buf_addr = ecall_smem_virtual_addr + 2000; 
  ecall_modem_ivs.ecall_rx_in_buf_addr = ecall_smem_virtual_addr + 3000; 

  ecall_modem_ivs.ecall_tx_out_buf_phy_addr = ecall_smem_physical_addr;
  ecall_modem_ivs.ecall_tx_in_buf_phy_addr = ecall_smem_physical_addr + 1000; 
  ecall_modem_ivs.ecall_rx_out_buf_phy_addr = ecall_smem_physical_addr + 2000; 
  ecall_modem_ivs.ecall_rx_in_buf_phy_addr = ecall_smem_physical_addr + 3000; 
  
  ret = __aprv2_cmd_alloc_send(
            ecall_modem_apr_handle,                /* handle       */
            APRV2_PKT_MSGTYPE_SEQCMD_V,            /* msg_type     */
            ecall_modem_apr_src_addr,              /* src_addr     */
            ecall_modem_apr_src_port,              /* src_port     */
            ecall_modem_apr_dest_addr,             /* dst_addr     */
            ecall_modem_apr_dest_port,             /* dst_port     */
            ECALL_APR_MAP_MEMORY,                  /* token        */
            VSS_ICOMMON_CMD_MAP_MEMORY,            /* opcode       */
            &mem_map_cmd,                          /* payload      */
            sizeof(mem_map_cmd) );                 /* payload_size */

  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "__aprv2_cmd_alloc_send failed, ret %d",
                               ret );
  }

} /* ECALL_MODEM_MAP_SHARED_MEMORY */

/*==========================================================================
  FUNCTION ECALL_MODEM_UNMAP_SHARED_MEMORY

  DESCRIPTION
    This function will send the unmap shared memory command to CVD thru APR.

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
  int32_t ret;
 
  if (ecall_modem_ivs.smem_mapped == FALSE)
  {
    ECALL_MSG_0( ECALL_ERROR, "unmap_shared_memory() called but was already unmapped" );
    return;
  }
  else if (ecall_modem_ivs.voc_rx_registered == TRUE || ecall_modem_ivs.voc_tx_registered == TRUE)
  {
    ECALL_MSG_0( ECALL_ERROR, "unmap_shared_memory() called but still registered for vocoder" );
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
   ecall_modem_ivs.ecall_rx_in_buf_addr      = 0;
   ecall_modem_ivs.ecall_rx_out_buf_addr     = 0;
   ecall_modem_ivs.ecall_tx_in_buf_addr      = 0;
   ecall_modem_ivs.ecall_tx_out_buf_addr     = 0;
   ecall_modem_ivs.ecall_tx_in_buf_phy_addr  = 0;
   ecall_modem_ivs.ecall_tx_out_buf_phy_addr = 0;
   ecall_modem_ivs.ecall_rx_in_buf_phy_addr  = 0;
   ecall_modem_ivs.ecall_rx_out_buf_phy_addr = 0;

   /* Reset smem_mapped flag */
   ecall_modem_ivs.smem_mapped = FALSE;

  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "__aprv2_cmd_alloc_send failed, ret %d",
                               ret );
  }

} /* ECALL_MODEM_UNMAP_SHARED_MEMORY */

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
      /* Will free the cmd in the function */
      ecall_process_ivpcm_evt_notify(packet);
      break;

    default:
      ECALL_MSG_1( ECALL_ERROR, "Received an unsupported command %d",
                                 packet->opcode );
      __aprv2_cmd_end_command(ecall_modem_apr_handle, packet, APR_EUNSUPPORTED );
      break;
  }

} /* ECALL_PROCESS_APR_PACKET */

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

    /* Copy APR packet pointer and process at ecall_ivs_task */
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
            &ecall_modem_apr_handle,           /* <- Return eCall handle */
            ecall_modem_ivs_ecall_dns,         /* -> Name string of the eCall service */
            sizeof(ecall_modem_ivs_ecall_dns), /* -> Size of the name string, including NULL */
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
            ecall_modem_ivs_voc_dns, 
            sizeof(ecall_modem_ivs_voc_dns), 
            &ecall_modem_apr_dest_addr);
  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "ecall_modem_apr_init: __aprv2_cmd_local_dns_lookup failed, ret %d",
                               ret );
  }

} /* ECALL_MODEM_APR_INIT */

/*===========================================================================

  FUNCTION ECALL_MODEM_CVS_INIT

  DESCRIPTION
    This function will initialize the CVS session for eCall

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A

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

  ECALL_MSG_0( ECALL_MED, "Request a passive control session to get the dest port" );

  ret = __aprv2_cmd_alloc_send(
            ecall_modem_apr_handle,                                    /* handle       */
            APRV2_PKT_MSGTYPE_SEQCMD_V,                                /* msg_type     */
            ecall_modem_apr_src_addr,                                  /* src_addr     */
            ecall_modem_apr_src_port,                                  /* src_port     */
            ecall_modem_apr_dest_addr,                                 /* dst_addr     */
            APR_NULL_V,                                                /* dst_port     */
            ECALL_APR_CREATE_PASSIVE_CONTROL_SESSION,                  /* token        */ 
            VSS_ISTREAM_CMD_CREATE_PASSIVE_CONTROL_SESSION,            /* opcode       */
            ecall_modem_stream_name,                                   /* payload      */
            sizeof(ecall_modem_stream_name));                          /* payload_size */
  
  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "__aprv2_cmd_alloc_send(CREATE_PASSIVE_CONTROL_SESSION) failed, ret=%d",
                               ret );
  }

} /* ECALL_MODEM_CVS_INIT */

/*==========================================================================
  FUNCTION ECALL_PROCESS_IBASIC_RSP_RESULT

  DESCRIPTION
    This function will process the incoming IBASIC_RSP_RESULT with
    the incoming APR packet and the token code.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A

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
  uint32_t status;
  ecall_pcm_buf_type *pcm_buf;
  uint16 lpcm = sizeof(int16)*ECALL_PCM_LENGTH;

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
        ECALL_MSG_0( ECALL_HIGH, "Map memory process successfully, register for vocoder now" );
        ecall_modem_ivs.smem_mapped = TRUE;
        ecall_voc_register(ecall_modem_ivs.voc_re_register_mode);
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
        ecall_modem_ivs.smem_mapped = FALSE;
      }
      else
      {
        ECALL_MSG_1( ECALL_ERROR, "Unmap memory process failed, error code %d",
                                   status );
      }
      __aprv2_cmd_free(ecall_modem_apr_handle, packet );
      break;

    case ECALL_APR_IVPCM_START_RX_TX:
      if(status == APR_EOK)
      {
        ECALL_MSG_0( ECALL_HIGH, "Successfully register with both RX/TX vocoder session" );
        /* Disable DTX */
        ecall_voc_configure();

        /* Prefill a buffer when register for vocoder TX */
        if (q_cnt(&ecall_pcm_q) == 0)
        {
          if ( (pcm_buf = ecall_get_pcm_buf()) == NULL )
          {
            ECALL_MSG_0( ECALL_ERROR, "No PCM buffers for tx prefill" );
          }
          else
          {
            ECALL_MSG_0( ECALL_HIGH, "Prefill a buffer when reg for voc TX" );
            memset(pcm_buf->pcm, 0, lpcm);
            ecall_put_pcm_buf(pcm_buf);
          }
        }
        else
        {
          ECALL_MSG_1( ECALL_MED, "ecall_pcm_q already have %d item, will not prefill",
                                   q_cnt(&ecall_pcm_q) );
        }
        ecall_modem_ivs.voc_rx_registered = TRUE;
        ecall_modem_ivs.voc_tx_registered = TRUE;
        ecall_modem_ivs.voc_pending_register = FALSE;
      }
      else
      {
        /* Let ecall clients know we failed to start the vocoder PCM session */
        ECALL_MSG_1( ECALL_ERROR, "Failed to register with both RX/TX vocoder session, error code %d",
                                   status );
        ecall_ivs_status.tx_status  = ECALL_IVS_ERR;
        ecall_post_client_notify();
      }
      __aprv2_cmd_free(ecall_modem_apr_handle, packet);
      break;

    case ECALL_APR_IVPCM_START_RX_ONLY:
      if(status == APR_EOK)
      {
        ECALL_MSG_0( ECALL_HIGH, "Successfully register with RX-only vocoder session" );
        ecall_modem_ivs.voc_rx_registered = TRUE;
        ecall_modem_ivs.voc_pending_register = FALSE;
      }
      else
      {
        /* Let ecall clients know we failed to start the vocoder PCM session */
        ECALL_MSG_1( ECALL_ERROR, "Failed to register with RX-only vocoder session, error code %d",
                                   status );
        ecall_ivs_status.tx_status  = ECALL_IVS_ERR;
        ecall_post_client_notify();
      }
      __aprv2_cmd_free(ecall_modem_apr_handle, packet);
      break;

    case ECALL_APR_IVPCM_STOP:
      if(status == APR_EOK)
      {
        ECALL_MSG_0( ECALL_HIGH, "Successfully deregister with both RX/TX with vocoder" );
        ecall_modem_ivs.voc_rx_registered = FALSE;
        ecall_modem_ivs.voc_tx_registered = FALSE;

        /* Reset the missed_rx_pcm_count */
        ecall_modem_ivs.missed_rx_pcm_count = 0;

        if (ecall_modem_ivs.voc_re_register == TRUE)
        {
          ECALL_MSG_1( ECALL_MED, "Re-register vocoder with mode %d",
                                   ecall_modem_ivs.voc_re_register_mode );
          if (ecall_modem_ivs.voc_re_register_mode == ECALL_VOC_MODE_RX_ONLY)
          {
            ecall_voc_register_rx();
          }
          else if (ecall_modem_ivs.voc_re_register_mode == ECALL_VOC_MODE_RX_TX)
          {
            ecall_voc_register_rx_tx();
          }
          else
          {
            ECALL_MSG_1( ECALL_ERROR, "ibasic_rsp_result: Unsupported vocoder mode %d",
                                       ecall_modem_ivs.voc_re_register_mode );
            return;
          }
          ecall_modem_ivs.voc_re_register = FALSE;
        }
        else
        {
          /* Unmap the memory */
          ecall_modem_unmap_shared_memory();
        }
      }
      else
      {
        /* Let ecall clients know we failed to end the vocoder PCM session */
        ECALL_MSG_1( ECALL_ERROR, "Failed to stop IVPCM session, error code %d",
                                   status );
        ecall_ivs_status.tx_status  = ECALL_IVS_ERR;
        ecall_post_client_notify();
      }
      __aprv2_cmd_free(ecall_modem_apr_handle, packet);
      break;

    case ECALL_APR_CREATE_PASSIVE_CONTROL_SESSION:
      if (status == APR_EOK)
      {
        ECALL_MSG_1( ECALL_MED, "Successfully create passive CVS control session, port %x",
                                 packet->src_port );
        ecall_modem_apr_dest_port = packet->src_port;
        ecall_voc_register(ecall_modem_ivs.voc_re_register_mode);
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
    ecall_rx_pcm(packet);
  }
  else if (notify_evt->tap_point == VSS_IVPCM_TAP_POINT_TX_DEFAULT)
  {
    ecall_tx_pcm(packet);
  }
  else
  {
    ECALL_MSG_1( ECALL_ERROR, "process_evt_notify: unsupported tap point %d",
                               notify_evt->tap_point );
  }

} /* ECALL_PROCESS_IVPCM_EVT_NOTIFY */

#elif (defined FEATURE_ECALL_HAS_VOICE_B)
/*===========================================================================

                     B - F a m i l y   F u n c t i o n

===========================================================================*/

/*==========================================================================
  FUNCTION ECALL_TX_PCM

  DESCRIPTION
    Pulls a PCM buffer off of the queue and copies it into the Vocoder's
    buffer for encoding and transmission.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
    The vocoder input takes 14 bit linear pcm samples and the 15th,16th bits
    are the sign extension bits. we need to downscale two bits to meet the vocoder format.
 
===========================================================================*/
void ecall_tx_pcm
(
  aprv2_packet_t* packet
)
{
  int32 ret;
  vss_ivpcm_evt_notify_v2_t *notify_evt_ptr;
  vss_ivpcm_evt_push_buffer_v2_t push_buff;
  ecall_pcm_buf_type *buf_ptr = NULL;
  uint32 i = 0;
  uint8 *tx_out_buf_ptr = NULL;
  uint8 *tx_in_buf_ptr = NULL;
  int16 pcm_buffer[ECALL_PCM_LENGTH];

  if(ecall_modem_ivs.voc_tx_registered == FALSE)
  {
    ECALL_MSG_1( ECALL_ERROR, "ecall_tx_pcm() called but not registered with vocoder (registered = %d)!",
                               ecall_modem_ivs.voc_tx_registered );
    return;  
  }

  /* Get the payload */
  if (packet == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "ecall_tx_pcm: packet is NULL" );
    return;
  }
  notify_evt_ptr = APRV2_PKT_GET_PAYLOAD( vss_ivpcm_evt_notify_v2_t, packet );
  if(notify_evt_ptr == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "Failed to get payload at ecall_tx_pcm" );
    return;
  }

  if (notify_evt_ptr->request_buf_size != ECALL_PCM_LENGTH_BYTES)
  {
    ECALL_MSG_2( ECALL_ERROR, "tx_pcm: request_buf_size %d != ECALL_PCM_LENGTH_BYTES %d",
                               notify_evt_ptr->request_buf_size,
                               ECALL_PCM_LENGTH_BYTES );
    return;
  }

  /* out_buf_mem_size needs to aligned with 128 byte */
  push_buff.out_buf_mem_size = notify_evt_ptr->request_buf_size + ( 128 - (notify_evt_ptr->request_buf_size % 128) );
  push_buff.in_buf_mem_size  = ECALL_PCM_LENGTH_BYTES;
  
  push_buff.tap_point       = VSS_IVPCM_TAP_POINT_TX_DEFAULT;  /* Tap Point */
  push_buff.sampling_rate   = notify_evt_ptr->sampling_rate;   /* Sampling Rate, VSS_IVPCM_SAMPLING_RATE_8K */
  push_buff.push_buf_mask   = 2;                               /* Push_buf_mask */
  push_buff.num_in_channels = 1;                               /* Num_in_channels */
  
  push_buff.out_buf_mem_address = ecall_modem_ivs.ecall_tx_out_buf_phy_addr;
  push_buff.in_buf_mem_address = ecall_modem_ivs.ecall_tx_in_buf_phy_addr;
  tx_out_buf_ptr = (uint8*)ecall_modem_ivs.ecall_tx_out_buf_addr;
  tx_in_buf_ptr = (uint8*)ecall_modem_ivs.ecall_tx_in_buf_addr; 

  if (tx_in_buf_ptr == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "tx_pcm: tx_in_buff_ptr = NULL" );
    return;
  }

  if(ecall_modem_ivs.drop_tx_pcm_count > 0)
  {
    if ((buf_ptr = (ecall_pcm_buf_type *) q_get(&ecall_pcm_q)) != NULL)
    {
      /* If IVS sent silence in between the send state drop the next pcm frame to preserve offset*/
      ECALL_MSG_1( ECALL_ERROR, "Dropping PCM frame as silence was sent earlier drop_tx_count%d",
                                 ecall_modem_ivs.drop_tx_pcm_count );

      ecall_modem_ivs.drop_tx_pcm_count--;
      q_put(&ecall_pcm_free_q, &buf_ptr->link);
    }
  }
   
  if ((buf_ptr = (ecall_pcm_buf_type *) q_get(&ecall_pcm_q)) != NULL)
  {
    for (i = 0; i < ECALL_PCM_LENGTH; i++)
    {
      /* The vocoder input takes 14 bit linear pcm samples and the 15th,16th bits are the 
        sign extension bits. we need to downscale two bits to meet the vocoder format*/
      pcm_buffer[i] = (buf_ptr->pcm[i] >> 2);
    }
    ecall_memscpy  ( tx_in_buf_ptr, notify_evt_ptr->request_buf_size, 
                     pcm_buffer, sizeof(pcm_buffer) );

    ECALL_MSG_0( ECALL_MED, "ecall_tx_pcm put_cmd" );
    q_put(&ecall_pcm_free_q, &buf_ptr->link);
  }
  else
  {
    /*lint -e641*/
    if(ivs.tx.state == IvsSendMsd)
    {
      /* If the IVS doesn't have a PCM frame to send it will send a silence and 
         drop the next queued up frame to preseve the offset*/
      ecall_modem_ivs.drop_tx_pcm_count++;
    }
    memset(tx_in_buf_ptr, 0, ECALL_PCM_LENGTH_BYTES);
    ECALL_MSG_0( ECALL_MED,"ecall_tx_pcm no data sending silence" );
  }

  ret = __aprv2_cmd_alloc_send(
            ecall_modem_apr_handle,              /* handle       */
            APRV2_PKT_MSGTYPE_EVENT_V,           /* msg_type     */
            ecall_modem_apr_src_addr,            /* src_addr     */
            ecall_modem_apr_src_port,            /* src_port     */
            ecall_modem_apr_dest_addr,           /* dst_addr     */
            ecall_modem_apr_dest_port,           /* dst_port     */
            ECALL_APR_PUSH_BUFFER,               /* token        */
            VSS_IVPCM_EVT_PUSH_BUFFER_V2,        /* opcode       */
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
    process this PCM frame.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
   The vocoder output gives 14 bit linear pcm samples and the 15th,16th bits 
   are the sign extension bits. we need to up-scale two bits to get the actual data.
 
===========================================================================*/
void ecall_rx_pcm
(
  aprv2_packet_t* packet
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

  if(ecall_modem_ivs.voc_rx_registered == FALSE)
  {
    ECALL_MSG_1( ECALL_ERROR, "rx_pcm() called but not registered with vocoder (register = %d)!",
                               ecall_modem_ivs.voc_rx_registered );
    return;
  }

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
    ECALL_MSG_2( ECALL_ERROR, "request_buff_size %d is not equal to ECALL_PCM_LENGTH_BYTES %d",
                               notify_evt_ptr->request_buf_size,
                               ECALL_PCM_LENGTH_BYTES );
    return;
  }

  /* out_buf_mem_size needs to aligned with 128 bytes */
  push_buff.out_buf_mem_size = notify_evt_ptr->request_buf_size + ( 128 - (notify_evt_ptr->request_buf_size % 128) );
  push_buff.in_buf_mem_size  = ECALL_PCM_LENGTH_BYTES;

  push_buff.tap_point       = VSS_IVPCM_TAP_POINT_RX_DEFAULT;  /* Tap Point */
  push_buff.sampling_rate   = notify_evt_ptr->sampling_rate;   /* Sampling Rate, VSS_IVPCM_SAMPLING_RATE_8K */
  push_buff.push_buf_mask   = 1;                               /* Push_buf_mask */
  push_buff.num_in_channels = 1;                               /* Num_in_channels */

  push_buff.out_buf_mem_address = ecall_modem_ivs.ecall_rx_out_buf_phy_addr;
  push_buff.in_buf_mem_address = ecall_modem_ivs.ecall_rx_in_buf_phy_addr;
  rx_out_buf_ptr = (uint8*)ecall_modem_ivs.ecall_rx_out_buf_addr;
  rx_in_buf_ptr = (uint8*)ecall_modem_ivs.ecall_rx_in_buf_addr; 

  if(rx_out_buf_ptr == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "rx_pcm: rx_out_buf_ptr is NULL!" );
    return;
  }

  /* Set a 30ms timer to identify a missed rx_pcm() and reset the miss count */
  ecall_ivs_set_voc_timer(ecall_voc_timer_duration[0]);
  ecall_modem_ivs.missed_rx_pcm_count = 0;

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
      ecall_memscpy  ( pcm_buffer, sizeof(pcm_buffer),
                       rx_out_buf_ptr, notify_evt_ptr->request_buf_size );
      for (i = 0; i < ECALL_PCM_LENGTH; i++)
      {
        
        /* The vocoder output gives 14 bit linear pcm samples and the 15th,16th bits are the 
          sign extension bits. we need to up-scale two bits to get the actual data*/
        cmd_ptr->data.pcm[i] = (pcm_buffer[i] << 2);
      }
    }
    ECALL_MSG_0( ECALL_MED, "ecall_rx_pcm put_cmd" );
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
    Registers eCall functions with Vocoder so that eCall feeds PCM
    frames for output and receives decoded PCM input.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None

  SIDE EFFECTS
    None
 
===========================================================================*/
void ecall_voc_register
(
  ecall_voc_mode_type voc_mode
)
{
  ECALL_MSG_1( ECALL_MED,"Request to register vocoder for mode: %d (0:RX only, 1:RX-TX)",
                          voc_mode );
  
  if (ecall_modem_apr_dest_port == 0)
  {
    /* Initialize CVS stream to get the dest port
       will call ecall_modem_map_shared_memory after successfully created cvs session */
    ECALL_MSG_1( ECALL_MED, "voc_reg: dest_port is 0(%d). Will init APR then create passive control session to get port",
                             ecall_modem_apr_dest_port );

    /* Initialize APR interface for message routing */
    ecall_modem_apr_init();

    ecall_modem_ivs.voc_re_register_mode = voc_mode;
    ecall_modem_cvs_init();
    return;
  }
  
  if(ecall_modem_ivs.smem_mapped == FALSE)
  {
    /* Map the memory pool for APR interface,
       will call ecall_voc_register again after successfully mapped */
    ECALL_MSG_0( ECALL_MED, "voc_reg: memory not mapped. Will register after successfully mapped" );
    ecall_modem_ivs.voc_re_register_mode = voc_mode;
    ecall_modem_map_shared_memory();
    return;
  }

  ecall_modem_ivs.voc_pending_register = TRUE;

  if(voc_mode == ECALL_VOC_MODE_RX_ONLY)
  {
    if(ecall_modem_ivs.voc_rx_registered == TRUE &&
       ecall_modem_ivs.voc_tx_registered == TRUE)
    {
      ECALL_MSG_0( ECALL_HIGH, "Vocoder was configured as RX/TX mode, unregister first" );
      /* Inform ibasic_rsp_result() voc need to re-register
         Will wait for a successful voc unregister, then do the reregister*/
      ecall_modem_ivs.voc_re_register = TRUE;
      ecall_modem_ivs.voc_re_register_mode = ECALL_VOC_MODE_RX_ONLY;
      ecall_voc_unregister();
      return;
    }
    else if(ecall_modem_ivs.voc_rx_registered == FALSE)
    {
      /* Register vocoder for RX only mode */
      ecall_voc_register_rx();
      return;
    } 
  }
  else if (voc_mode == ECALL_VOC_MODE_RX_TX)
  {
    if(ecall_modem_ivs.voc_rx_registered == TRUE &&
       ecall_modem_ivs.voc_tx_registered == FALSE)
    {
      ECALL_MSG_0( ECALL_HIGH, "Vocoder was configured as RX only mode, unregister first" );
      /* Inform ibasic_rsp_result() voc need to re-register
         Will wait for a successful voc unregister, then do the reregister*/
      ecall_modem_ivs.voc_re_register = TRUE;
      ecall_modem_ivs.voc_re_register_mode = ECALL_VOC_MODE_RX_TX;
      ecall_voc_unregister();
      return;
    }
    else if(ecall_modem_ivs.voc_rx_registered == FALSE)
    {
      /* Register vocoder for RX/TX mode */
      ecall_voc_register_rx_tx();
      return;
    }
  }
  else
  {
    ECALL_MSG_1( ECALL_ERROR, "Unsupported vocoder mode %d",
                               voc_mode );
    return;
  }

  ECALL_MSG_1( ECALL_MED, "Vocoder already register for mode %d",
                           ecall_modem_ivs.voc_config_mode );

} /* ECALL_VOC_REGISTER */

/*==========================================================================
  FUNCTION ECALL_VOC_REGISTER_RX_TX

  DESCRIPTION
    Registers eCall functions with Vocoder RX and TX session so that eCall
    feeds PCM frames for output and receives decoded PCM input.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None

  SIDE EFFECTS
    None
 
===========================================================================*/
void ecall_voc_register_rx_tx
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

  if(ecall_modem_ivs.voc_rx_registered == TRUE)
  {
    ECALL_MSG_1( ECALL_HIGH, "An active vocoder PCM session already exist with mode = %d",
                              ecall_modem_ivs.voc_config_mode );
    return;
  }

  num_tap_points = 2; /* 2 tap point for TX and RX */
  target_payload_len = sizeof(ecall_modem_ivs.ecall_memory_handle) +
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
            ECALL_APR_IVPCM_START_RX_TX,    /* token        */
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
    ECALL_MSG_0( ECALL_ERROR, "vocoder_register_rx_tx: dst_packet is NULL" );
    return;
  }
  dst_payload = APRV2_PKT_GET_PAYLOAD(uint8_t, dst_packet);

  /* Copy memory_handle to dst_payload */
  ecall_memscpy ( dst_payload, remaining_target_payload_len, 
                  &ecall_modem_ivs.ecall_memory_handle, sizeof(ecall_modem_ivs.ecall_memory_handle) );
  dst_payload += sizeof(ecall_modem_ivs.ecall_memory_handle);
  remaining_target_payload_len -= sizeof(ecall_modem_ivs.ecall_memory_handle);

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

} /* ECALL_VOC_REGISTER_RX_TX */

/*==========================================================================
  FUNCTION ECALL_VOC_REGISTER_RX

  DESCRIPTION
    Registers eCall functions with Vocoder RX so that eCall receives decoded
    PCM input.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None

  SIDE EFFECTS
    None
 
===========================================================================*/
void ecall_voc_register_rx
(
  void
)
{
  int32 ret;
  vss_ivpcm_tap_point_t vss_tap_point;
  uint32 target_payload_len;
  uint32 num_tap_points;
  uint32 remaining_target_payload_len;
  aprv2_packet_t *dst_packet = NULL;
  uint8_t *dst_payload;

  if(ecall_modem_ivs.voc_rx_registered == TRUE)
  {
    ECALL_MSG_1( ECALL_HIGH, "An active vocoder PCM session already exist with mode = %d",
                              ecall_modem_ivs.voc_config_mode );
    return;
  }
  
  num_tap_points = 1; /* 1 tap point for TX */
  target_payload_len = sizeof(ecall_modem_ivs.ecall_memory_handle) +
                       sizeof(num_tap_points) + num_tap_points*sizeof(vss_ivpcm_tap_point_t);
  remaining_target_payload_len = target_payload_len;

  /* allocate the dest_packet */
  ret = __aprv2_cmd_alloc_ext(
            ecall_modem_apr_handle,         /* handle       */
            APRV2_PKT_MSGTYPE_SEQCMD_V,     /* msg_type     */
            ecall_modem_apr_src_addr,       /* src_addr     */
            ecall_modem_apr_src_port,       /* src_port     */
            ecall_modem_apr_dest_addr,      /* dst_addr     */
            ecall_modem_apr_dest_port,      /* dst_port     */
            ECALL_APR_IVPCM_START_RX_ONLY,  /* token        */
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
    ECALL_MSG_0( ECALL_ERROR, "vocoder_register_rx: dst_packet is NULL" );
    return;
  }
  dst_payload = APRV2_PKT_GET_PAYLOAD(uint8_t, dst_packet);

  /* Copy memory_handle to dst_payload */
  ecall_memscpy ( dst_payload, remaining_target_payload_len, 
                  &ecall_modem_ivs.ecall_memory_handle, sizeof(ecall_modem_ivs.ecall_memory_handle) );
  dst_payload += sizeof(ecall_modem_ivs.ecall_memory_handle);
  remaining_target_payload_len -= sizeof(ecall_modem_ivs.ecall_memory_handle);

  /* copy num_tap_points to dst_payload */
  ecall_memscpy ( dst_payload, remaining_target_payload_len,
                  &num_tap_points, sizeof(num_tap_points) );
  dst_payload += sizeof(num_tap_points);
  remaining_target_payload_len -= sizeof(num_tap_points);

  /* Configure RX tap point */
  vss_tap_point.tap_point = VSS_IVPCM_TAP_POINT_RX_DEFAULT;
  vss_tap_point.direction = 0; /* output-only tap point, VSS_IVPCM_TAP_POINT_DIR_OUT */
  vss_tap_point.sampling_rate = VSS_IVPCM_SAMPLING_RATE_8K; 
  vss_tap_point.duration = 0;

  /* copy RX tap point structure to destination payload */
  ecall_memscpy ( dst_payload, remaining_target_payload_len,
                  &vss_tap_point, sizeof(vss_tap_point) );
  
  /* Send the IVPCM_CMD_START command */
  ret = __aprv2_cmd_forward( 
            ecall_modem_apr_handle,  /* handle */
            dst_packet);             /* packet */

  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "__aprv2_cmd_alloc_send failed, ret %d",
                               ret );
  }

} /* ECALL_REGISTER_RX */

/*==========================================================================
  FUNCTION ECALL_MODEM_MAP_SHARED_MEMORY

  DESCRIPTION
    This function will allocate a shared memory region from SMEM pool if not
    allocated yet, and send a map memory command to CVD thru APR.

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

  if (ecall_modem_ivs.smem_mapped == TRUE)
  {
    ECALL_MSG_0( ECALL_ERROR, "map_shared_memory() called but was already mapped!" );
    return;
  }

  if (ecall_modem_ivs.smem_allocated == FALSE)
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
        
    ecall_modem_ivs.smem_allocated = TRUE;
  }

  /* Make Memroy Table address to page size */
  ecall_memory_table_virtual_addr  = ecall_memory_table_virtual_addr_orig + (ECALL_SMEM_SIZE - (ecall_memory_table_virtual_addr_orig % ECALL_SMEM_SIZE) );
  ecall_memory_table_physical_addr = ecall_memory_table_physical_addr_orig + (ECALL_SMEM_SIZE - (ecall_memory_table_physical_addr_orig % ECALL_SMEM_SIZE) );

  memset( (uint8*)ecall_memory_table_virtual_addr, 0, 4096 );

  ECALL_MSG_2( ECALL_MED, "memtable_addr: virt_addr: 0x%08X, phys_addr: 0x%08X",
                           ecall_memory_table_virtual_addr,
                           ecall_memory_table_physical_addr );

  /* Make VPCM address aligned to page size */
  ecall_smem_virtual_addr = ecall_smem_virtual_addr_orig + (ECALL_SMEM_SIZE - (ecall_smem_virtual_addr_orig % ECALL_SMEM_SIZE) );
  ecall_smem_physical_addr = ecall_smem_physical_addr_orig + (ECALL_SMEM_SIZE - (ecall_smem_physical_addr_orig % ECALL_SMEM_SIZE) );

  memset( (uint8*)ecall_smem_virtual_addr, 0, 4096 );

  ECALL_MSG_2( ECALL_MED, "VPCM: virt_addr: 0x%08X, phys_addr: 0x%08X",
                           ecall_smem_virtual_addr,
                           ecall_smem_physical_addr );

  ecall_modem_ivs.ecall_tx_out_buf_addr = ecall_smem_virtual_addr;
  ecall_modem_ivs.ecall_tx_in_buf_addr = ecall_smem_virtual_addr + 1024; 
  ecall_modem_ivs.ecall_rx_out_buf_addr = ecall_smem_virtual_addr + 2048; 
  ecall_modem_ivs.ecall_rx_in_buf_addr = ecall_smem_virtual_addr + 3072; 

  ECALL_MSG_2( ECALL_MED, "ecall_tx_out_buf_addr: 0x%08X, tx_in_buf_addr: 0x%08X",
                           ecall_modem_ivs.ecall_tx_out_buf_addr,
                           ecall_modem_ivs.ecall_tx_in_buf_addr );
  ECALL_MSG_2( ECALL_MED, "ecall_rx_out_buf_addr: 0x%08X, rx_in_buf_addr: 0x%08X",
                           ecall_modem_ivs.ecall_rx_out_buf_addr,
                           ecall_modem_ivs.ecall_rx_in_buf_addr );

  ecall_modem_ivs.ecall_tx_out_buf_phy_addr = ecall_smem_physical_addr;
  ecall_modem_ivs.ecall_tx_in_buf_phy_addr = ecall_smem_physical_addr + 1024; 
  ecall_modem_ivs.ecall_rx_out_buf_phy_addr = ecall_smem_physical_addr + 2048; 
  ecall_modem_ivs.ecall_rx_in_buf_phy_addr = ecall_smem_physical_addr + 3072; 

  ECALL_MSG_2( ECALL_MED, "ecall_tx_out_buf_phy_addr: 0x%08X, tx_in_buf_phy_addr: 0x%08X",
                           ecall_modem_ivs.ecall_tx_out_buf_phy_addr,
                           ecall_modem_ivs.ecall_tx_in_buf_phy_addr );
  ECALL_MSG_2( ECALL_MED, "ecall_rx_out_buf_phy_addr: 0x%08X, rx_in_buf_phy_addr: 0x%08X",
                           ecall_modem_ivs.ecall_rx_out_buf_phy_addr,
                           ecall_modem_ivs.ecall_rx_in_buf_phy_addr );

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
    This function will send the unmap shared memory command to CVD thru APR.

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
  int32_t ret;
 
  if (ecall_modem_ivs.smem_mapped == FALSE)
  {
    ECALL_MSG_0( ECALL_ERROR, "unmap_shared_memory() called but was already unmapped" );
    return;
  }
  else if (ecall_modem_ivs.voc_rx_registered == TRUE || ecall_modem_ivs.voc_tx_registered == TRUE)
  {
    ECALL_MSG_0( ECALL_ERROR, "unmap_shared_memory() called but still registered for vocoder" );
    return;
  }
   
  mem_unmap_cmd.mem_handle = ecall_modem_ivs.ecall_memory_handle;
  
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
   ecall_modem_ivs.ecall_rx_in_buf_addr      = 0;
   ecall_modem_ivs.ecall_rx_out_buf_addr     = 0;
   ecall_modem_ivs.ecall_tx_in_buf_addr      = 0;
   ecall_modem_ivs.ecall_tx_out_buf_addr     = 0;
   ecall_modem_ivs.ecall_tx_in_buf_phy_addr  = 0;
   ecall_modem_ivs.ecall_tx_out_buf_phy_addr = 0;
   ecall_modem_ivs.ecall_rx_in_buf_phy_addr  = 0;
   ecall_modem_ivs.ecall_rx_out_buf_phy_addr = 0;

   /* Reset smem_mapped flag */
   ecall_modem_ivs.smem_mapped = FALSE;

  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "__aprv2_cmd_alloc_send failed, ret %d",
                               ret );
  }

} /* ECALL_MODEM_UNMAP_SHARED_MEMORY */

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
      /* Will free the cmd in the function */
      ecall_process_ivpcm_evt_notify(packet);
      break;

    case VSS_IMEMORY_RSP_MAP:
      /* Will free the cmd in the function */
      ecall_process_imemory_rsp_map(packet);
      break;

    default:
      ECALL_MSG_1( ECALL_ERROR, "Received an unsupported command %d",
                                 packet->opcode );
      __aprv2_cmd_end_command(ecall_modem_apr_handle, packet, APR_EUNSUPPORTED );
      break;
  }

} /* ECALL_PROCESS_APR_PACKET */

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

    /* Copy APR packet pointer and process at ecall_ivs_task */
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
            ecall_modem_ivs_ecall_dns,         /* -> Name string of the eCall service */
            sizeof(ecall_modem_ivs_ecall_dns), /* -> Size of the name string, including NULL */
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
            ecall_modem_ivs_voc_dns, 
            sizeof(ecall_modem_ivs_voc_dns), 
            &ecall_modem_apr_dest_addr);
  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "ecall_modem_apr_init: __aprv2_cmd_local_dns_lookup CVS failed, ret %d",
                               ret );
  }

  ret = __aprv2_cmd_local_dns_lookup(
            ecall_modem_ivs_mvm_dns,
            sizeof(ecall_modem_ivs_mvm_dns),
            &ecall_modem_apr_mvm_addr);
  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "ecall_modem_apr_init: __aprv2_cmd_local_dns_lookup MVM failed, ret %d",
                               ret );
  }

} /* ECALL_MODEM_APR_INIT */

/*===========================================================================

  FUNCTION ECALL_MODEM_CVS_INIT

  DESCRIPTION
    This function will initialize the CVS session for eCall

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_B

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

  ECALL_MSG_0( ECALL_MED,"Request a passive control session to get the dest port" );

  ret = __aprv2_cmd_alloc_send(
            ecall_modem_apr_handle,                                    /* handle       */
            APRV2_PKT_MSGTYPE_SEQCMD_V,                                /* msg_type     */
            ecall_modem_apr_src_addr,                                  /* src_addr     */
            ecall_modem_apr_src_port,                                  /* src_port     */
            ecall_modem_apr_dest_addr,                                 /* dst_addr     */
            APR_NULL_V,                                                /* dst_port     */
            ECALL_APR_CREATE_PASSIVE_CONTROL_SESSION,                  /* token        */ 
            VSS_ISTREAM_CMD_CREATE_PASSIVE_CONTROL_SESSION,            /* opcode       */
            ecall_modem_stream_name,                                   /* payload      */
            sizeof(ecall_modem_stream_name));                          /* payload_size */
  
  if (ret != APR_EOK)
  {
    ECALL_MSG_1( ECALL_ERROR, "__aprv2_cmd_alloc_send(CREATE_PASSIVE_CONTROL_SESSION) failed, ret=%d",
                               ret );
  }

} /* ECALL_MODEM_CVS_INIT */

/*==========================================================================
  FUNCTION ECALL_PROCESS_IBASIC_RSP_RESULT

  DESCRIPTION
    This function will process the incoming IBASIC_RSP_RESULT with
    the incoming APR packet and the token code.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_B

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
  uint32_t status;
  ecall_pcm_buf_type *pcm_buf;
  uint16 lpcm = sizeof(int16)*ECALL_PCM_LENGTH;

  /* Get the payload */
  if (packet == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "ecall_process_ibasic_rsp_result: packet is NULL" );
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
      /* MAP_MEMORY will return VSS_IMEMORY_RSP_MAP for success case */   
      ECALL_MSG_1( ECALL_ERROR, "Map memory process failed, error code %d",
                                 status );
      __aprv2_cmd_free(ecall_modem_apr_handle, packet );
      break;

    case ECALL_APR_UNMAP_MEMORY:
      if(status == APR_EOK)
      {
        ECALL_MSG_0( ECALL_HIGH, "Unmap memory process successfully" );
        ecall_modem_ivs.smem_mapped = FALSE;
      }
      else
      {
        ECALL_MSG_1( ECALL_ERROR, "Unmap memory process failed, error code %d",
                                   status );
      }
      __aprv2_cmd_free(ecall_modem_apr_handle, packet );
      break;

    case ECALL_APR_IVPCM_START_RX_TX:
      if(status == APR_EOK)
      {
        ECALL_MSG_0( ECALL_HIGH, "Successfully register with both RX/TX vocoder session" );
        /* Disable DTX */
        ecall_voc_configure();

        /* Prefill a buffer when register for vocoder TX */
        if (q_cnt(&ecall_pcm_q) == 0)
        {
          if ( (pcm_buf = ecall_get_pcm_buf()) == NULL )
          {
            ECALL_MSG_0( ECALL_ERROR, "No PCM buffers for tx prefill" );
          }
          else
          {
            ECALL_MSG_0( ECALL_HIGH, "Prefill a buffer when reg for voc TX" );
            memset(pcm_buf->pcm, 0, lpcm);
            ecall_put_pcm_buf(pcm_buf);
          }
        }
        else
        {
          ECALL_MSG_1( ECALL_MED, "ecall_pcm_q already have %d item, will not prefill",
                                   q_cnt(&ecall_pcm_q) );
        }
        ecall_modem_ivs.voc_rx_registered = TRUE;
        ecall_modem_ivs.voc_tx_registered = TRUE;
        ecall_modem_ivs.voc_pending_register = FALSE;
      }
      else
      {
        /* Let ecall clients know we failed to start the vocoder PCM session */
        ECALL_MSG_1( ECALL_ERROR, "Failed to register with both RX/TX vocoder session, error code %d",
                                   status );
        ecall_ivs_status.tx_status  = ECALL_IVS_ERR;
        ecall_post_client_notify();
      }
      __aprv2_cmd_free(ecall_modem_apr_handle, packet);
      break;

    case ECALL_APR_IVPCM_START_RX_ONLY:
      if(status == APR_EOK)
      {
        ECALL_MSG_0( ECALL_HIGH, "Successfully register with RX-only vocoder session" );
        ecall_modem_ivs.voc_rx_registered = TRUE;
        ecall_modem_ivs.voc_pending_register = FALSE;
      }
      else
      {
        /* Let ecall clients know we failed to start the vocoder PCM session */
        ECALL_MSG_1( ECALL_ERROR, "Failed to register with RX-only vocoder session, error code %d",
                                   status );
        ecall_ivs_status.tx_status  = ECALL_IVS_ERR;
        ecall_post_client_notify();
      }
      __aprv2_cmd_free(ecall_modem_apr_handle, packet);
      break;

    case ECALL_APR_IVPCM_STOP:
      if(status == APR_EOK)
      {
        ECALL_MSG_0( ECALL_HIGH, "Successfully deregister with both RX/TX with vocoder" );
        ecall_modem_ivs.voc_rx_registered = FALSE;
        ecall_modem_ivs.voc_tx_registered = FALSE;

        /* Reset the missed_rx_pcm_count */
        ecall_modem_ivs.missed_rx_pcm_count = 0;

        if (ecall_modem_ivs.voc_re_register == TRUE)
        {
          ECALL_MSG_1( ECALL_MED, "Re-register vocoder with mode %d",
                                   ecall_modem_ivs.voc_re_register_mode );
          if (ecall_modem_ivs.voc_re_register_mode == ECALL_VOC_MODE_RX_ONLY)
          {
            ecall_voc_register_rx();
          }
          else if (ecall_modem_ivs.voc_re_register_mode == ECALL_VOC_MODE_RX_TX)
          {
            ecall_voc_register_rx_tx();
          }
          else
          {
            ECALL_MSG_1( ECALL_ERROR, "ibasic_rsp_result: Unsupported vocoder mode %d",
                                       ecall_modem_ivs.voc_re_register_mode );
            return;
          }
          ecall_modem_ivs.voc_re_register = FALSE;
        }
        else
        {
          /* Unmap the memory */
          ecall_modem_unmap_shared_memory();
        }
      }
      else
      {
        /* Let ecall clients know we failed to end the vocoder PCM session */
        ECALL_MSG_1( ECALL_ERROR, "Failed to stop IVPCM session, error code %d",
                                   status );
        ecall_ivs_status.tx_status  = ECALL_IVS_ERR;
        ecall_post_client_notify();
      }
      __aprv2_cmd_free(ecall_modem_apr_handle, packet);
      break;

    case ECALL_APR_CREATE_PASSIVE_CONTROL_SESSION:
      if (status == APR_EOK)
      {
        ECALL_MSG_1( ECALL_MED, "Successfully create passive CVS control session, port %x",
                                 packet->src_port );
        ecall_modem_apr_dest_port = packet->src_port;
        ecall_voc_register(ecall_modem_ivs.voc_re_register_mode);
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
    ecall_rx_pcm(packet);
  }
  else if (notify_evt->tap_point == VSS_IVPCM_TAP_POINT_TX_DEFAULT)
  {
    ecall_tx_pcm(packet);
  }
  else
  {
    ECALL_MSG_1( ECALL_ERROR, "process_evt_notify: unsupported tap point %d",
                               notify_evt->tap_point );
  }

} /* ECALL_PROCESS_IVPCM_EVT_NOTIFY */

/*==========================================================================
  FUNCTION ECALL_PROCESS_IMEMORY_RSP_MAP

  DESCRIPTION
    This function will process the respond of VSS_IMEMORY_CMD_MAP_PHYSICAL
    and save the memory handle.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_process_imemory_rsp_map
(
  aprv2_packet_t* packet
)
{
  vss_imemory_rsp_map_t *rsp_map;

  if (packet == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "process_rsp_map: packet is NULL" );
    return;
  }
  rsp_map = APRV2_PKT_GET_PAYLOAD( vss_imemory_rsp_map_t, packet );
  if (rsp_map == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "process_rsp_map: Cannot get rsp_map payload out" );
    return;
  }

  /* Save the eCall memory handle from MVM */
  ECALL_MSG_1( ECALL_HIGH, "Map memory successfully with handle: %d, register for vocoder now",
                            rsp_map->mem_handle );
  ecall_modem_ivs.ecall_memory_handle = rsp_map->mem_handle;
  ecall_modem_ivs.smem_mapped = TRUE;
  ecall_voc_register(ecall_modem_ivs.voc_re_register_mode);

  __aprv2_cmd_free(ecall_modem_apr_handle, packet );

} /* ECALL_PROCESS_IVPCM_EVT_NOTIFY */

#else /* Legacy Vocoder */
/*===========================================================================

                L e g a c y   V o c o d e r   F u n c t i o n

===========================================================================*/

/*==========================================================================

  FUNCTION ecall_tx_pcm

  DESCRIPTION
    Pulls a PCM buffer off of the queue and copies it into the Vocoder's
    buffer for encoding and transmission.  Called in interrupt context.

  DEPENDENCIES
    Legacy Vocoder

  RETURN VALUE
    None

  SIDE EFFECTS
    The vocoder input takes 14 bit linear pcm samples and the 15th,16th bits
    are the sign extension bits. we need to downscale two bits to meet the vocoder format

===========================================================================*/
static voc_pcm_data_status_type ecall_tx_pcm
(
  uint16  *pcm
)
{
  ecall_pcm_buf_type *buf_ptr = NULL;
  uint32 i = 0;
 
  if (!ecall_modem_ivs.voc_tx_registered)
  {
    ECALL_MSG_1( ECALL_ERROR, "ecall_tx_pcm() called but not registered with vocoder (registered = %d)!",
                               ecall_modem_ivs.voc_tx_registered );
    return VOC_PCM_DATA_STATUS_UNAVAILABLE;
  }

  if(ecall_modem_ivs.drop_tx_pcm_count > 0)
  {
    if ((buf_ptr = (ecall_pcm_buf_type *) q_get(&ecall_pcm_q)) != NULL)
    {
      /* If IVS sent silence in between the send state drop the next pcm frame to preserve offset*/
      ECALL_MSG_1( ECALL_ERROR, "Dropping PCM frame as silence was sent earlier drop_tx_count%d",
                                 ecall_modem_ivs.drop_tx_pcm_count );

      ecall_modem_ivs.drop_tx_pcm_count--;
      q_put(&ecall_pcm_free_q, &buf_ptr->link);
    }
  }
   
  if ((buf_ptr = (ecall_pcm_buf_type *) q_get(&ecall_pcm_q)) != NULL)
  {
    for (i = 0; i < ECALL_PCM_LENGTH; i++)
    {
      /* The vocoder input takes 14 bit linear pcm samples and the 15th,16th bits are the 
        sign extension bits. we need to downscale two bits to meet the vocoder format*/
       pcm[i] = buf_ptr->pcm[i]>>2;   /*lint !e702*/
    }

    ECALL_MSG_0( ECALL_MED,"ecall_tx_pcm put_cmd" );
    q_put(&ecall_pcm_free_q, &buf_ptr->link);
    return VOC_PCM_DATA_STATUS_AVAILABLE;
  }
  else
  {
    /*lint -e641*/
    if(ivs.tx.state == IvsSendMsd)
    {
      /* If the IVS doesn't have a PCM frame to send it will send a silence and 
         drop the next queued up frame to preseve the offset*/
      ecall_modem_ivs.drop_tx_pcm_count++;
    }
    /*lint +e641*/
    memset(pcm, 0, ECALL_PCM_LENGTH_BYTES);
  }

  ECALL_MSG_0( ECALL_MED, "ecall_tx_pcm no data sending silence" );
  return VOC_PCM_DATA_STATUS_AVAILABLE;

} /* ecall_tx_pcm() */


/*==========================================================================

  FUNCTION ecall_rx_pcm

  DESCRIPTION
    Called by Vocoder task in interrupt context to inform us of a new
    incoming PCM frame.  Called every 20ms with 160 PCM samples 
    (16 bits @ 8 kHz, but only 14 bits used in samples).

  DEPENDENCIES
    Legacy Vocoder

  RETURN VALUE
    None

  SIDE EFFECTS
   The vocoder output gives 14 bit linear pcm samples and the 15th,16th bits 
   are the sign extension bits. we need to up-scale two bits to get the actual data.

===========================================================================*/
static void ecall_rx_pcm
(
  const uint16  *pcm
)
{
  ecall_cmd_type *cmd_ptr = NULL;
  uint16 i =0;

  if (!ecall_modem_ivs.voc_rx_registered)
  {
    ECALL_MSG_1( ECALL_ERROR, "ecall_rx_pcm() called but not registered with vocoder (registered = %d)!",
                               ecall_modem_ivs.voc_rx_registered );
    return;
  }

  /* Set a 30ms timer to identify a missed rx_pcm() and reset the miss count */
  ecall_ivs_set_voc_timer(ecall_voc_timer_duration[0]);
  ecall_modem_ivs.missed_rx_pcm_count = 0;

  cmd_ptr = ecall_get_cmd_buf();

  if (cmd_ptr != NULL)
  {
    /* Post a command to the eCall task to process this PCM frame */
    cmd_ptr->cmd_id = ECALL_CMD_PROCESS_RX_PCM;

    for (i = 0; i < ECALL_PCM_LENGTH; i++)
    {
      /* The vocoder output gives 14 bit linear pcm samples and the 15th,16th bits are the 
        sign extension bits. we need to up-scale two bits to get the actual data*/
      cmd_ptr->data.pcm[i] = (int16) (pcm[i] << 2);
    }

    ECALL_MSG_0( ECALL_MED, "ecall_rx_pcm put_cmd" );
    ecall_put_cmd(cmd_ptr);
  }
  else
  {
    ECALL_MSG_0( ECALL_ERROR, "Dropped PCM sample; ran out of command buffers" );
  }

} /* ecall_rx_pcm() */

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
)
{
  voc_status_type ret = VOC_CMD_FAILED_S;

  if (ecall_modem_ivs.voc_rx_registered == TRUE)
  {
    ECALL_MSG_0( ECALL_ERROR, "ecall_voc_rx_register already registered!" );
    return TRUE;
  }

  ret = voc_register_pcm_output_client(VOC_PCM_INTERFACE_RX_OUTPUT_FROM_DEC ,
                                         ecall_rx_pcm);
  if (VOC_DONE_S != ret) {
    ECALL_MSG_1( ECALL_ERROR, "Failed to register eCall RX output client with vocoder! err = %d",
                               ret );
    return FALSE;
  }

  ECALL_MSG_0( ECALL_HIGH, "Successfully registered eCall RX out callback with vocoder" );
  ecall_modem_ivs.voc_rx_registered = TRUE;

  return TRUE;
}

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
)
{
  voc_status_type ret = VOC_CMD_FAILED_S;
  ecall_pcm_buf_type  *pcm_buf;
  uint16 lpcm = sizeof(int16)*ECALL_PCM_LENGTH;

  if (ecall_modem_ivs.voc_tx_registered == TRUE)
  {
    ECALL_MSG_0( ECALL_HIGH, "ecall_voc_tx_register already registered!" );
    return TRUE;
  }

  ret = voc_register_pcm_input_client(VOC_PCM_INTERFACE_TX_INPUT_TO_ENC ,
                                      ecall_tx_pcm);

  if (VOC_DONE_S != ret) {
      ECALL_MSG_1( ECALL_ERROR, "Failed to register eCall TX input client with vocoder! err = %d", 
                                 ret );
     return FALSE;
  }

  /* Prefill a buffer when register for vocoder TX */
  if (q_cnt(&ecall_pcm_q) == 0)
  {
    if ( (pcm_buf = ecall_get_pcm_buf()) == NULL )
    {
      ECALL_MSG_0( ECALL_ERROR, "No PCM buffers for tx prefill" );
      return FALSE;
    }
    else
    {
      ECALL_MSG_0( ECALL_HIGH, "Prefill a buffer when reg for voc TX" );
      memset(pcm_buf->pcm, 0, lpcm);
      ecall_put_pcm_buf(pcm_buf);
    }
  }
  else
  {
    ECALL_MSG_1( ECALL_MED, "ecall_pcm_q already have %d item, will not prefill",
                             q_cnt(&ecall_pcm_q) );
  }
  
  ECALL_MSG_0( ECALL_HIGH, "Successfully registered eCall TX in callback with vocoder" );
  ecall_modem_ivs.voc_tx_registered = TRUE;

  return TRUE;
} /* ecall_voc_tx_register() */

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
)
{
  voc_status_type ret;
  ecall_pcm_buf_type *buf_ptr = NULL;
 
  if (!ecall_modem_ivs.voc_tx_registered)
  {
    ECALL_MSG_0( ECALL_HIGH, "ecall_voc_tx_unregister called but not registered!" );
    return;
  }

  ret = voc_register_pcm_input_client(VOC_PCM_INTERFACE_TX_INPUT_TO_ENC, NULL);
  if (VOC_DONE_S != ret) {
    ECALL_MSG_1( ECALL_ERROR, "Couldn't unregister eCall TX input client with vocoder! err = %d", 
                               ret );
    return;
  }

  /* Move everything back to their free queues */
  while ((buf_ptr = (ecall_pcm_buf_type *) q_get(&ecall_pcm_q)) != NULL)
  {
    q_put(&ecall_pcm_free_q, &buf_ptr->link);
  }

  ECALL_MSG_0( ECALL_HIGH, "De registered eCall TX input with vocoder" );

  ecall_modem_ivs.voc_tx_registered = FALSE;
} /* ecall_modem_voc_unregister() */



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
)
{

  voc_status_type ret;

  if (!ecall_modem_ivs.voc_rx_registered)
  {
    ECALL_MSG_0( ECALL_ERROR, "ecall_voc_rx_unregister called but not registered!" );
    return;
  }

  ret = voc_register_pcm_output_client(VOC_PCM_INTERFACE_RX_OUTPUT_FROM_DEC, NULL);

  if (VOC_DONE_S != ret) {
    ECALL_MSG_1( ECALL_ERROR, "Couldn't unregister eCall RX output client with vocoder! err = %d",
                               ret );
    return;
  }

  /* Reset the missed_rx_pcm_count */
  ecall_modem_ivs.missed_rx_pcm_count = 0;

  ECALL_MSG_0( ECALL_HIGH, "De registered eCall RX output with vocoder" );
  ecall_modem_ivs.voc_rx_registered = FALSE;
}

#endif /* FEATURE_ECALL_HAS_VOICE_A */


#else

void ecall_modem_ivs_dummy_func(void);

#endif // End Featurisation

