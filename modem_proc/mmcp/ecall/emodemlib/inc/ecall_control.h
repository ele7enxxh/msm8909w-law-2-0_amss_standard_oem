#ifndef ECALL_CONTROL_H_
#define ECALL_CONTROL_H_
/*==========================================================================

                           E C A L L   C O N T R O L. H

GENERAL DESCRIPTION
  control message functions 

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/
/*==========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodemlib/inc/ecall_control.h#1 $ 
$DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $


when        who     what, where, why
--------    ---     ---------------------------------------------------------
02/01/10    sar     Initial version  8.2.0

==========================================================================*/

#include "ecall_defines.h"
#include "ecall_sync.h"
#include "ecall_modem.h"

#if (defined(FEATURE_ECALL_IVS) || defined(FEATURE_ECALL_PSAP))
typedef enum {
  DlMsgNoop = -2,
  DlMsgReset,
  DlMsgStart,
  DlMsgNack,
  DlMsgAck,  
  DlMsgSend,
  DlMsgHlack = ECALL_SET_LLMSG + ECALL_SET_HLMSG
} ecall_dl_data;

typedef enum {
  CtrlRxIdle,
  CtrlRxSync,
  CtrlRxLock,
  CtrlTxIdle,
  CtrlTxSend
} ecall_port_state;

typedef enum {
  PortUndef,
  PortIvs,
  PortPsap
} ecall_port_owner;


/*============================================================================*/
/* Control structures                                                         */
/*----------------------------------------------------------------------------*/

typedef struct {
  int16 dlData;           /* message symbol */
  int16 dlIndex;          /* message frame counter */
} ecall_ctrl_tx_port;

typedef struct {
  int16 dlData;           /* detected message symbol */
  int16 dlMetric;         /* receiver metric */
} ecall_ctrl_rx_port;

typedef struct {
  union {
    ecall_ctrl_tx_port tx;        /* port control transmitter */
    ecall_ctrl_rx_port rx;        /* port control receiver */
  } u;
  ecall_port_state state;         /* port state */
  boolean invert;                 /* port inversion flag */
  ecall_port_owner owner;         /* port owner identification */
} ecall_ctrl_port;

typedef struct {
  ecall_ctrl_port port;          /* port struct */
} ecall_ctrl_tx_data;

typedef struct {
  ecall_ctrl_port port;          /* port struct */

  ecall_sync_state *sync;        /* pointer to sync struct */
  int16 *buffer;                 /* pointer to control receiver buffer */

  uint8  dlHlackData;            /* higher-layer message (4 bits) */
  Tern  dlRead;                  /* sync indication (ternary variable) */
  int16 dlIndex;                 /* internal frame counter */
  int16 dlSyncLock;              /* number of sync events required */
} ecall_ctrl_rx_data;


/*============================================================================*/
/* Control functions                                                          */
/*----------------------------------------------------------------------------*/
/*===========================================================================
FUNCTION      ecall_ctrl_portname

DESCRIPTION 
     IVS/PSAP would own the port

DEPENDENCIES
  None

RETURN VALUE
       

SIDE EFFECTS
  None

===========================================================================*/
const char* ecall_ctrl_portname
(
  ecall_port_owner
);

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
  ecall_ctrl_rx_data*,
  const int16*
);

/*===========================================================================
FUNCTION      ECALL_CTRL_TX_RESET

DESCRIPTION 
      reset function control transmitter

DEPENDENCIES
  None

RETURN VALUE
      InOut:  ecall_ctrl_tx_data* control  <-> control struct  
      In:     ecall_port_owner     -> modem identification        

SIDE EFFECTS
  None

===========================================================================*/
void ecall_ctrl_tx_reset
(
  ecall_ctrl_tx_data*,
  ecall_port_owner
);


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
  ecall_ctrl_tx_data*,
  int16*,
  uint32
);

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
  int16*,
  uint32,
  int16,
  int16
);

/*===========================================================================
FUNCTION      ECALL_CTRL_RX_RESET

DESCRIPTION 
      reset function control receiver

DEPENDENCIES
  None

RETURN VALUE
  InOut:  ecall_ctrl_rx_data* control   <-> control struct             
   In:     ecall_port_owner       -> modem identification               
           ecall_sync_state*  sync       -> pointer to sync struct      
           int16*      buffer     -> pointer to control receiver buffer 
           int16       syncLock   -> number of sync events required     

SIDE EFFECTS
  None

===========================================================================*/
void ecall_ctrl_rx_reset
(
  ecall_ctrl_rx_data*,
  ecall_port_owner,
  ecall_sync_state*,
  int16*,
  int16
);

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
  ecall_ctrl_rx_data*,
  const int16*
);

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
  const int16*,
  int16*
);

#endif /*end FEATURISATION*/

#endif

