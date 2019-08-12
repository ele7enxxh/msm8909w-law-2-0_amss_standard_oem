#ifndef WL1XLM_H
#define WL1XLM_H
/*===========================================================================

                    L 1   L I N K   M A N A G E R

DESCRIPTION
  This file contains global declarations and external references
  for WCDMA L1 Link Manager.


EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2014 by Qualcomm Technologies, Inc. All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wl1xlm.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/17/14   mit     Skip enter mode calls from wl1 in FTM mode
07/15/14   bj      WCDMA Narrow Band Rejection for System Search.
07/07/14   sm      Request and release diversity rxlm resources in ds mode 
                   along with primary 
06/19/14   cc      IRAT DR-DSDS check in
06/13/14   cc      Increasing the MAX num of RxLM buffer index
05/29/14   mm      Update IRAT/CM driver initialization and RxLM buffer allocation call flow
07/24/13   jd      Changes for Dual band Dual carrier
04/23/13   mm      Additional RxLM buffer for back to back CM gaps.
01/30/12   vs      Feature cleanup.
04/13/11   yh      Changed LM interface filename
01/17/11   yh      Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#include "lm_types.h"
#include "rxlm_intf.h"
#include "txlm_intf.h"

/*-------------------------------------------------------------------------*/
/*                        MACROS                                           */
/*-------------------------------------------------------------------------*/

#define WL1_XLM_RXLM_INVALID_IDX  RXLM_MAX_BUFFERS

#define WL1_XLM_TXLM_INVALID_IDX TXLM_MAX_BUFFERS

/*-------------------------------------------------------------------------*/
/*                        TYPE DEFINITIONS                                 */
/*-------------------------------------------------------------------------*/

/* WL1 RxLM enums for each buffer */
typedef enum {
  WL1_XLM_RX_CHAIN0_HOME,
  WL1_XLM_RX_CHAIN1_DIVERSITY,
  WL1_XLM_RX_CHAIN2_HOME,
  WL1_XLM_RX_CHAIN3_DIVERSITY,
  WL1_XLM_RX_CHAIN0_INTERF_NBR0,
  WL1_XLM_RX_CHAIN0_INTERF_NBR1,
  WL1_XLM_RX_CHAIN0_IRAT_GSM_NBR0,
  WL1_XLM_RX_CHAIN0_IRAT_GSM_NBR1,
  WL1_XLM_RX_CHAIN0_IRAT_LTE_NBR,
  WL1_XLM_RX_CHAIN1_IRAT_LTE_NBR,
  WL1_XLM_RX_CHAIN1_NB_REJ,
  WL1_XLM_RX_CHAIN_INVALID,
  WL1_XLM_RX_CHAIN_MAX = WL1_XLM_RX_CHAIN_INVALID,
} wl1_xlm_rx_client_enum_type;

/* WL1 RxLM buffer information */
typedef struct {
  /* Buffer in use or free */
  boolean in_use;
  /* RxLM buffer index */
  uint32 rxlm_buf_idx;
} wl1_xlm_rx_buf_type;


/* WL1 TxLM enums for each buffer */
typedef enum {
  WL1_XLM_TXLM_CHAIN_0,
  WL1_XLM_TXLM_CHAIN_1,
  WL1_XLM_TX_CHAIN_INVALID,
  WL1_XLM_TX_CHAIN_MAX = WL1_XLM_TX_CHAIN_INVALID,
} wl1_xlm_tx_client_enum_type;

/* WL1 TxLM buffer information */
typedef struct {
  /* Buffer in use or free */
  boolean in_use;
  /* RxLM buffer index */
  uint32  txlm_buf_idx;
} wl1_xlm_tx_buf_type;

/* ------------------------------------------------------------------------*/
/*                     GLOBAL DATA DECLARATIONS                            */
/* ----------------------------------------------------------------------- */
/* WL1 maintenance of RxLM buffer. One entry per client */
extern wl1_xlm_rx_buf_type wl1_rxlm_buf[WL1_XLM_RX_CHAIN_MAX];

/* WL1 maintenance of TxLM buffer. One entry per client */
extern wl1_xlm_tx_buf_type wl1_txlm_buf[WL1_XLM_TX_CHAIN_MAX];

/*-------------------------------------------------------------------------*/
/*                         FUNCTIONS                                       */
/*-------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION wl1_xlm_init_rx_buffers

DESCRIPTION
  Function initializes the WL1 RxLM buffer array.
  
DEPENDENCIES
  None

RETURN VALUE
  None 

SIDE EFFECTS
  None
  
===========================================================================*/
void wl1_xlm_init_rx_buffers(void);

/*===========================================================================
FUNCTION wl1_xlm_rx_get_chain_and_tech

DESCRIPTION
  Function translates the Rx client into RxLM chain and tech type and returns 
  status.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE if translation went through, FALSE otherwise

SIDE EFFECTS
  Input chain and tech will be written with the correct value for the client
  
===========================================================================*/
boolean wl1_xlm_rx_get_chain_and_tech(
  /* Client to be translated */
  wl1_xlm_rx_client_enum_type client, 
  /* Chain for the client */
  rxlm_chain_type *chain, 
  /* Tech for the client */
  lm_tech_type  *tech);

/*===========================================================================
FUNCTION wl1_xlm_allocate_rxlm_buffer

DESCRIPTION
  Function allocates a buffer from RxLM for client specified.
  
DEPENDENCIES
  None

RETURN VALUE
  RxLM buffer allocated

SIDE EFFECTS
  None
  
===========================================================================*/
uint32 wl1_xlm_allocate_rxlm_buffer(
  /* Client to have RxLM buffer allocated for */
  wl1_xlm_rx_client_enum_type client);

/*===========================================================================
FUNCTION wl1_xlm_deallocate_rxlm_buffer

DESCRIPTION
  Function deallocates a buffer from RxLM for client specified.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wl1_xlm_deallocate_rxlm_buffer(
  /* Client to have RxLM buffer deallocated for */
  wl1_xlm_rx_client_enum_type client);

/*===========================================================================
FUNCTION wl1_xlm_get_rxlm_buf

DESCRIPTION
  Function gets the RxLM chain for the client specified. If not in use, it 
  ERR_FATALs.
  
DEPENDENCIES
  None

RETURN VALUE
  RxLM buf allocated.

SIDE EFFECTS
  None
  
===========================================================================*/
uint32 wl1_xlm_get_rxlm_buf(
  /* Client to be translated */
  wl1_xlm_rx_client_enum_type client);

/*===========================================================================
FUNCTION wl1_xlm_is_client_in_use

DESCRIPTION
  This function returns whether the inputted rxlm client has been previously
  allocated
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE if the passed in client is in use
  FALSE if the passed in client is not in use

SIDE EFFECTS
  None
  
===========================================================================*/
boolean wl1_xlm_is_client_in_use(
/* the client to determine if in use or not */
wl1_xlm_rx_client_enum_type client);

/*===========================================================================
FUNCTION wl1_xlm_init_tx_buffers

DESCRIPTION
  Function initializes the WL1 TxLM buffer array.
  
DEPENDENCIES
  None

RETURN VALUE
  None 

SIDE EFFECTS
  None
  
===========================================================================*/
void wl1_xlm_init_tx_buffers(void);

/*===========================================================================
FUNCTION wl1_xlm_tx_get_chain_and_tech

DESCRIPTION
  Function translates the Tx client into TxLM chain and tech type and returns 
  status.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE if translation went through, FALSE otherwise

SIDE EFFECTS
  Input chain and tech will be written with the correct value for the client
  
===========================================================================*/
boolean wl1_xlm_tx_get_chain_and_tech(
  /* Client to be translated */
  wl1_xlm_tx_client_enum_type client, 
  /* Chain for the client */
  txlm_chain_type *chain, 
  /* Tech for the client */
  lm_tech_type  *tech);

/*===========================================================================
FUNCTION wl1_xlm_allocate_txlm_buffer

DESCRIPTION
  Function allocates a buffer from TxLM for client specified.
  
DEPENDENCIES
  None

RETURN VALUE
  RxLM buffer allocated

SIDE EFFECTS
  None
  
===========================================================================*/
uint32 wl1_xlm_allocate_txlm_buffer(
  /* Client to have RxLM buffer allocated for */
  wl1_xlm_tx_client_enum_type client);

/*===========================================================================
FUNCTION wl1_xlm_deallocate_txlm_buffer

DESCRIPTION
  Function deallocates a buffer from TxLM for client specified.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wl1_xlm_deallocate_txlm_buffer(
  /* Client to have RxLM buffer deallocated for */
  wl1_xlm_tx_client_enum_type client);

/*===========================================================================
FUNCTION wl1_xlm_get_txlm_buf

DESCRIPTION
  Function gets the RxLM chain for the client specified. If not in use, it 
  ERR_FATALs.
  
DEPENDENCIES
  None

RETURN VALUE
  RxLM buf allocated.

SIDE EFFECTS
  None
  
===========================================================================*/
uint32 wl1_xlm_get_txlm_buf(
  /* Client to be translated */
  wl1_xlm_tx_client_enum_type client);

/*===========================================================================
FUNCTION wl1_xlm_deallocate_all_active_rxlm_buffers

DESCRIPTION
  Function deallocates all active buffers from RxLM for clients in use. This is
  done when L1 state is going suspended.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void wl1_xlm_deallocate_all_active_rxlm_buffers(void);

#endif /* WL1XLM_H */
