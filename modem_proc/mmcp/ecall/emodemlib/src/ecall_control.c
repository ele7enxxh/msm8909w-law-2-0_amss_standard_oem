/*===========================================================================

                           E C A L L _ C O N T R O L . C

DESCRIPTION
  control message functions                                    

REFERENCES
  C Reference code 3GPP TS  26.268

EXTERNALIZED FUNCTIONS
 
INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright(c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodemlib/src/ecall_control.c#1 $ 
  $DateTime: 2016/12/13 08:00:05 $ 
  $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/17/12    DL       Featurize adding DC offset with FEATURE_ECALL_DCOFFSET
02/01/10    sar      Initial version.

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "msg.h"
#include "err.h"
#include <stringl/stringl.h>

#include "ecall_control.h"
#include "ecall_defines.h"
#include "ecall_sync.h"
#include "ecall_rom.h"
#include "ecall_efs.h"

#ifdef FEATURE_ECALL_IVS
#include "ecall_modem_ivs.h"
#endif

#if (defined FEATURE_ECALL_IVS) || (defined FEATURE_ECALL_PSAP)

/*============================================================================*/
/* UTILITY FUNCTION: CtrlPortName                                             */
/*----------------------------------------------------------------------------*/
/* Description: converter for log output                                      */
/*                                                                            */
/* In:     PortOwner   owner  -> modem identification                         */
/* Return: const char*        <- port name as character string                */
/*----------------------------------------------------------------------------*/

const char* ecall_ctrl_portname(ecall_port_owner owner)
{
  static const char* strIvs   = "IVS";
  static const char* strPsap  = "PSAP";
  static const char* strUndef = "VOID";

  switch (owner) {
    case PortIvs:
      return strIvs;
    case PortPsap:
      return strPsap;
    default:
      return strUndef;
  }
}

/*===========================================================================
FUNCTION      ECALL_CTRL_SYNC

DESCRIPTION 
      control message sync function

DEPENDENCIES
  None

RETURN VALUE
  InOut:  ecall_ctrl_rx_data*  control  <-> control struct                   
  In:     const int16* pcm     -> input frame of 16bit PCM samples        

SIDE EFFECTS
  None

===========================================================================*/
void ecall_ctrl_sync
(
  ecall_ctrl_rx_data *control,
  const int16 *pcm
)
{
  ecall_ctrl_port *port = &control->port;
  ecall_sync_state *sync = control->sync;

  /* initialize port data */
  port->u.rx.dlData   = DlMsgNoop;
  port->u.rx.dlMetric = 0;

  if (sync->events < control->dlSyncLock) {
    sync->delay  = 0;

    /* keep control receiver inactive while sync is running */
    if (sync->resync == FALSE) {
      port->state = CtrlRxIdle;
    }

    /* run synchronization */
    ecall_sync(sync, pcm, ecall_ctrl_portname(port->owner), port->invert);

    if (sync->flag == TRUE && sync->delay <= 0)
    {
      ECALL_MSG_2( ECALL_MED, "debug index %d delayMem %d",
                               sync->index,
                               sync->delayMem );

      if (sync->index >= ECALL_NRF_DLCHUNK  ||
        sync->amplitude[2] < sync->amplitude[0])
      {
        /* change port state to sync received */
        port->state = CtrlRxSync;
        
 /*       switch (port->owner) {
          case PortIvs:
            IvsCatchEvent(IVSEVENT_CONTROLSYNC);
            break;
          case PortPsap:
            PsapCatchEvent(PSAPEVENT_CONTROLSYNC);
            break;
          default:
            LogInfo("[%-4s] control callback: port owner undefined.",
              CtrlPortName(PortUndef));
            break;
        } */
        /* count occurrences of the same sync position */
        if (sync->events > 0 &&
          sync->delayMem == sync->delay &&
          sync->index%(ECALL_NRF_DLCHUNK) == 0)
        {
          sync->events++;
          sync->index = 0;
        } 
        else {
          sync->events   = 1;
          sync->index    = 0;
          sync->delayMem = sync->delay;
        }
        if (sync->events >= control->dlSyncLock) {
         /* check for muting gaps passed already (avoids unnecessary delays) */
          while (sync->delay <= -ECALL_PCM_LENGTH) {
            sync->delay += ECALL_PCM_LENGTH;
            sync->index++;
          }
          sync->delayMem   = sync->delay;
          control->dlIndex = sync->index;

          if (sync->resync == FALSE) {
            control->dlRead = Plus;
            control->dlSyncLock = ECALL_LOCK_RESYNC;

            /* lock sync for control message detection */
            port->state = CtrlRxLock;

 /*           switch (port->owner) {
              case PortIvs:
                IvsCatchEvent(IVSEVENT_CONTROLLOCK);
                break;
              case PortPsap:
                PsapCatchEvent(PSAPEVENT_CONTROLLOCK);
                break;
              default:
                LogInfo("[%-4s] control callback: port owner undefined.",
                  CtrlPortName(PortUndef));
                break;
            } */
            ECALL_MSG_1( ECALL_HIGH, "[%-4s] sync locked, starting control message detection", 
                                      ecall_ctrl_portname(port->owner) );

            /* apply sync inversion flag */
            if ((port->invert = sync->invert) == TRUE) {
              ECALL_MSG_1( ECALL_HIGH, "[%-4s] codec inversion detected!",
                                        ecall_ctrl_portname(port->owner) );
            }
          }
          else if (sync->invert == port->invert) {
            /* resynchronization event (assume inversion has not changed) */
            control->dlRead = Plus;
          }
          else {
            control->dlRead = Minus;
          }
        } else {
          sync->amplitude[2] = sync->amplitude[0];
          sync->amplitude[0] = 0;
          sync->npeaks = 0;
        }
      } else {
        sync->amplitude[0] = 0;
        sync->npeaks = 0;
      }
    } else if (sync->resync == TRUE && sync->index > ECALL_NRF_RESYNC) {
      port->u.rx.dlData   = DlMsgReset;
      port->u.rx.dlMetric = 0;
    }
  }
  else {
    /* sync is locked, check whether locked sync is still valid */
    port->state = CtrlRxSync;
    ecall_sync_check(sync, pcm, ecall_ctrl_portname(port->owner));

    if (sync->flag == TRUE) {
      control->dlIndex = sync->index = (sync->index)%ECALL_NRF_DLCHUNK;

      while (sync->delayMem <= -ECALL_PCM_LENGTH) {
        sync->delayMem += ECALL_PCM_LENGTH;
        sync->index++;
        control->dlIndex++;
      }
      while (sync->delayMem > 0) {
        sync->delayMem -= ECALL_PCM_LENGTH;
        sync->index--;
        control->dlIndex--;
      }

      if (sync->check == Plus) {
        /* lower-layer control message */
        control->dlRead = Plus;
        sync->checkCnt = 0;
      }
      else if (sync->check == Minus) {
        /* higher-layer control message */
        control->dlRead = Minus;
        sync->checkCnt = 0;
      }
      else {
        /* following message is ignored */
        control->dlRead = Zero;
        sync->checkCnt++;
        ECALL_MSG_1( ECALL_HIGH, "[%c] message ignored (bad sync check)",
                                  ecall_ctrl_portname(port->owner) );
      }
    }
    /* sync completely lost, reset synchronization */
    if (sync->checkCnt >= ECALL_SYNC_BADCHECK) {
      ecall_sync_reset(sync, NULL, NULL);
      sync->resync = TRUE;
      #ifdef FEATURE_ECALL_IVS
      ecall_modem_ivs_report_lose_sync();
      #endif
    }
  }
}


/*===========================================================================
FUNCTION      ECALL_CTRL_TX_RESET

DESCRIPTION 
      reset function control transmitter

DEPENDENCIES
  None

RETURN VALUE
      InOut:  ecall_ctrl_tx_data* control  <-> control struct  
      In:     const char* owner     -> modem identification        

SIDE EFFECTS
  None

===========================================================================*/
void ecall_ctrl_tx_reset
(
  ecall_ctrl_tx_data *control,
  ecall_port_owner owner
)
{
  control->port.state = CtrlTxIdle;
  control->port.invert = FALSE;
  control->port.owner = owner;

  /* initial port data */
  control->port.u.tx.dlData = DlMsgNoop;
  control->port.u.tx.dlIndex = -1;
}

/*===========================================================================
FUNCTION      ECALL_CTRL_TX_PROCESS

DESCRIPTION 
      process function control transmitter

DEPENDENCIES
  None

RETURN VALUE
   InOut:  ecall_ctrl_tx_data* control  <-> control struct                   
           int16* pcm           <-> frame of 16bit PCM samples               

SIDE EFFECTS
  None

===========================================================================*/
void ecall_ctrl_tx_process
(
  ecall_ctrl_tx_data *control,
  int16 *pcm,
  uint32 pcm_size
)
{
  ecall_ctrl_port *port = &control->port;

  /* PSAP transmitter, initialize PCM data to zero */
  memset(pcm, 0, sizeof(int16)*ECALL_PCM_LENGTH);

  /* safety check on index range */
  if (port->u.tx.dlIndex >= 0 && port->u.tx.dlIndex < ECALL_NRF_DLCHUNK) {
    if (port->u.tx.dlData != DlMsgNoop) {
      ecall_ctrl_tx_mod(pcm, pcm_size, (int16)port->u.tx.dlData, port->u.tx.dlIndex);
    }
  } else if (port->u.tx.dlIndex >= ECALL_NRF_DLCHUNK) {
    ECALL_MSG_1( ECALL_ERROR, "message index too large (value: %d). Abort",
                               port->u.tx.dlIndex );
    ecall_modem_abort();
  }
}

/*===========================================================================
FUNCTION      ECALL_CTRL_TX_MOD

DESCRIPTION 
      control message transmitter using prestored sequences

DEPENDENCIES
  None

RETURN VALUE
    In:     int16  symbol  -> lower-layer or higher-layer message symbol      
            int16  index   -> position within message frame 

SIDE EFFECTS
  None

===========================================================================*/
void ecall_ctrl_tx_mod
(
  int16 *pcm,
  uint32 pcm_size,
  int16 symbol,
  int16 index
)
{
  int16 i;
  int16 lpcm = sizeof(int16)*ECALL_PCM_LENGTH;

  if (!(symbol >= 0 && symbol < ECALL_NUM_STORED_DLMSG) &&
    !(symbol >= ECALL_SET_LLMSG && symbol <= (ECALL_SET_LLMSG + ECALL_SET_HLMSG)))
  {
    ECALL_MSG_1( ECALL_ERROR, "message symbol %d out of range, set it to default START",
                               symbol );
    symbol = 0;
  }
  if (index < ECALL_NRF_WAKEUP) {
    /* wakeup tone */
    for (i = 0; i < ECALL_PCM_LENGTH; i++)
      pcm[i] = ecall_wakeupSin500[i%16];
  }
  else if (index < ECALL_NRF_SYNC) {
    /* sync frame */
    ecall_memscpy ( pcm, pcm_size, 
                    &ecall_syncFrame[(index-ECALL_NRF_WAKEUP)*ECALL_PCM_LENGTH], (uint16)lpcm);

    /* invert sync preamble for higher-layer messages */
    if (symbol >= ECALL_SET_LLMSG) {
      for (i = 0; i < ECALL_PCM_LENGTH; i++)
        pcm[i] = -pcm[i];
    }
    #ifdef FEATURE_ECALL_DCOFFSET
    /* add signal offset */
    for (i = 0; i < ECALL_PCM_LENGTH; i++) {
      pcm[i] += (pcm[i] == 0) ? 12000 : 5000;
    }
    #endif

    /*lint -save -e506 -e774*/  
    /* wakeup tone extends into the first frame of the preamble */
    if ((ECALL_NRF_WAKEUP > 0) && (index == ECALL_NRF_WAKEUP)) { 
      for (i = 0; i < 32; i++)
        pcm[i] = ecall_wakeupSin500[i%16];
    } /*lint -restore*/
  }
  else {
    i = (symbol < ECALL_SET_LLMSG)
      ? index - (ECALL_NRF_DLMUTE1LL + ECALL_NRF_SYNC)
      : index - (ECALL_NRF_DLMUTE1HL + ECALL_NRF_SYNC);

    if (symbol >= 0 && symbol < ECALL_NUM_STORED_DLMSG && i >= 0 && i < ECALL_NRF_DLDATA) {
      /* data message lower-layer symbol */
      ecall_memscpy ( pcm, pcm_size, 
                      &ecall_dlPcmData[symbol][i*ECALL_PCM_LENGTH], (uint16)lpcm);
    }
    else if (symbol >= ECALL_SET_LLMSG && i >= 0 && i < ECALL_NRF_DLDATA) {
      /* 1st part of data message higher-layer symbol */
      ecall_memscpy ( pcm, pcm_size,
                      &ecall_dlPcmData[(symbol-ECALL_SET_LLMSG)%4][i*ECALL_PCM_LENGTH],(uint16)lpcm);
    }
    else if (symbol >= ECALL_SET_LLMSG && i >= ECALL_NRF_DLDATA && i < 2*ECALL_NRF_DLDATA) {
      /* 2nd part of data message higher-layer symbol */
      i -= ECALL_NRF_DLDATA;
      ecall_memscpy (pcm, pcm_size,
                     &ecall_dlPcmData[((symbol-ECALL_SET_LLMSG)/4)%4][i*ECALL_PCM_LENGTH], (uint16)lpcm);
    }
    else {
      /* muting frame */
      memset(pcm, 0, (uint16)lpcm);
    }
  }
}

/*===========================================================================
FUNCTION      ECALL_CTRL_RX_RESET

DESCRIPTION 
      reset function control receiver

DEPENDENCIES
  None

RETURN VALUE
  InOut:   ecall_ctrl_rx_data* control   <-> control struct             
   In:     ecall_port_owner owner      -> modem identification               
           ecall_sync_state*  sync       -> pointer to sync struct      
           int16*      buffer     -> pointer to control receiver buffer 
           int16       syncLock   -> number of sync events required     

SIDE EFFECTS
  None

===========================================================================*/
void ecall_ctrl_rx_reset
(
  ecall_ctrl_rx_data *control,
  ecall_port_owner owner,
  ecall_sync_state *sync,
  int16 *buffer,
  int16 syncLock
)
{
  control->port.state = CtrlRxIdle;
  control->port.invert = FALSE;
  control->port.owner = owner;

  /* initial port data */
  control->port.u.rx.dlData = DlMsgNoop;
  control->port.u.rx.dlMetric = 0;

  /* set control pointers */
  control->sync = sync;
  control->buffer = buffer;

  /* set internal variables */
  control->dlRead = Zero;
  control->dlIndex = 0;
  control->dlSyncLock = syncLock;

  /* clear memory */
  memset(control->buffer, 0, sizeof(int16)*(ECALL_NRF_MEMCTRL*ECALL_PCM_LENGTH));
}

/*===========================================================================
FUNCTION      ECALL_CTRL_RX_PROCESS

DESCRIPTION 
       process function control receiver  

DEPENDENCIES
  None

RETURN VALUE
  InOut:  ecall_ctrl_rx_data*  control  <-> control struct                   
  In:     const int16* pcm       -> input frame of 16bit PCM samples           

SIDE EFFECTS
  None

===========================================================================*/
void ecall_ctrl_rx_process
(
  ecall_ctrl_rx_data *control,
  const int16 *pcm
)
{
  int16 i;
  int16 metric;
  boolean syncRun = FALSE;

  ecall_ctrl_port *port = &control->port;
  ecall_sync_state *sync = control->sync;

  /* increase sync index */
  sync->index++;

  if (control->dlRead == Zero) {
    /* run synchronization (sync function to set port data) */
    ecall_ctrl_sync(control, pcm);
    syncRun = TRUE;
  }
 
  if (control->dlRead == Plus &&
    (control->dlIndex)%ECALL_NRF_DLCHUNK == ECALL_NRF_DLMUTE1LL + ECALL_NRF_DLDATA - 1)
  {
    /* enough lower-layer data received, detect data now */
    i = (ECALL_NRF_MEMCTRL - ECALL_NRF_DLDATA)*ECALL_PCM_LENGTH;
    port->u.rx.dlData = 
      ecall_ctrl_rx_demod(&control->buffer[i + sync->delayMem], &metric);
    port->u.rx.dlMetric = metric;

    /* prepare for next sync */
    sync->index      = (sync->index)%ECALL_NRF_DLCHUNK;
    sync->check      = Zero;
    control->dlRead  = Zero;
    control->dlIndex = 0;
  }
  else if (control->dlRead == Minus &&
    control->dlIndex == ECALL_NRF_DLMUTE1HL + 2*ECALL_NRF_DLDATA - 1)
  {
    /* enough higher-layer data received, detect first data portion */
    i = (ECALL_NRF_MEMCTRL - 2*ECALL_NRF_DLDATA)*ECALL_PCM_LENGTH;
    port->u.rx.dlData = 
      ecall_ctrl_rx_demod(&control->buffer[i + sync->delayMem], &metric);
    port->u.rx.dlMetric = metric;

    /* detect second data portion and combine */
    i = (ECALL_NRF_MEMCTRL - ECALL_NRF_DLDATA)*ECALL_PCM_LENGTH;

    /*lint -save -e656 -e641 -e64 -e734*/
    port->u.rx.dlData +=
      4*ecall_ctrl_rx_demod(&control->buffer[i + sync->delayMem], &metric);
    port->u.rx.dlData += ECALL_SET_LLMSG;
    port->u.rx.dlMetric = MIN(port->u.rx.dlMetric, metric);
    /*lint -restore*/

    /* prepare for next sync */
    sync->index      = (sync->index)%ECALL_NRF_DLCHUNK;
    sync->check      = Zero;
    control->dlRead  = Zero;
    control->dlIndex = 0;
  }
  else if (syncRun == FALSE) {
    /* sync detected, wait for enough data to process */
    control->dlIndex++;
    port->u.rx.dlData   = DlMsgNoop;
    port->u.rx.dlMetric = 0;
  }
}

/*===========================================================================
FUNCTION      ECALL_CTRL_RX_DEMOD

DESCRIPTION 
       control message receiver  

DEPENDENCIES
  None

RETURN VALUE
       In:     const int16* pcm     -> input PCM buffer                           
       Out:    int16*       metric  <- reliability factor (-1: skip)              
       Return: int16                <- demodulated message                                

SIDE EFFECTS
  None

===========================================================================*/
int16 ecall_ctrl_rx_demod
(
  const int16 *pcm,
  int16 *metric
)
{
  int16 k, n, msg;
  int32 xCorr[4];
  int32 maxCorr, runnerup;

  /* correlation receiver */
  for (k = 0; k < 4; k++) {
    xCorr[k] = 0;
    for (n = 0; n < ECALL_NRF_DLDATA*ECALL_PCM_LENGTH; n++)
      xCorr[k] += (pcm[n]*ecall_dlPcmDataMatch[k][n]) >> 12; /*lint !e702 */
  }

  /* find largest correlation value */
  maxCorr = xCorr[0];
  msg = 0;
  for (k = 1; k < 4; k++) {
    if (xCorr[k] > maxCorr) {
      maxCorr = xCorr[k];
      msg = k;
    }
  }
  /* find second largest correlation value */
  runnerup = xCorr[(msg+1)%4];
  for (k = 0; k < 4; k++) {
    if (xCorr[k] > runnerup && xCorr[k] < maxCorr)
      runnerup = xCorr[k];
  }
  /* test on reliability by absolute and relative value */
  if (maxCorr < ECALL_IVS_THRESHOLD) {
    *metric = -1;
  }
  else if ((*metric = (int16)((maxCorr-runnerup)/(maxCorr/100))) <= 33) {
    *metric = -1;
  }
  return msg;
}

#endif /* end FEATURE_ECALL_IVS or FEATURE_ECALL_PSAP */

