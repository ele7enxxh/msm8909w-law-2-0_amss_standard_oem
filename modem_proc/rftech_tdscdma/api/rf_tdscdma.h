#ifndef RF_TDSCDMA_H
#define RF_TDSCDMA_H

/*===========================================================================


        R F  D r i v e r  TDSCDMA  E x t e r n a l  I n t e r f a c e 
                          H e a d e r  F i l e

DESCRIPTION
  This file contains all declarations and definitions necessary to use
  the TDSCDMA specific RF drivers.

  
Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/api/rf_tdscdma.h#1 $ $DateTime: 2016/12/13 07:59:50 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
04/24/13   qzh     Porting FTM RF code from NIKEL to DIME
07/16/12   jps     Initial FTM burst TX/RX framework
05/27/11   jhe     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  TDSCDMA specific constant definitions.
---------------------------------------------------------------------------*/
#define RF_TDSCDMA_NUM_RX_GAIN_STATE    4

#define RF_TDSCDMA_NV_TXAGC_MIN               0
#define RF_TDSCDMA_NV_TXAGC_MAX             1023

/* Minimum TDSCDMA RX Power Level based on UE dynamic range in dbm */
#define RF_TDSCDMA_MIN_RX_POWER_DBM      -106

/* Maximum TDSCDMA RX Power Level based on UE dynamic range in dbm  */
#define RF_TDSCDMA_MAX_RX_POWER_DBM      -21

/* Minimum TDSCDMA TX Power Level based on UE dynamic range in dbm */
#define RF_TDSCDMA_MIN_TX_POWER_DBM      -70

/* TxAGC Resolution */
#define RF_TDSCDMA_TXAGC_RESOLUTION      10
 
/* Maximum TX power that UE is capable to transmit based on UE power class in db10 */
/* 23dBm => (23+70)*10 = 930 */
#define RF_TDSCDMA_UE_MAX_TX_POWER_DB10  930

/* Maximum TX power that UE is capable to transmit based on UE power class in dbm */
#define RF_TDSCDMA_UE_MAX_TX_POWER       23

/* Minimum TX power that UE is capable to transmit based on UE power class in dbm */
#define RF_TDSCDMA_UE_MIN_TX_POWER       -55

/* Minimum TX power level above which to trigger hdet conversion in dbm */
#define RF_TDSCDMA_HDET_TRIG_PWR_LVL     15

#define RF_SATURATE( val, min, max )  MAX( MIN( (val), (max) ), (min) )

#if 0
/*******************************************************************************
 * 000 - 035  -- T2G start memory mapping
 * 036 - 074  -- T2G stop memory mapping
 *******************************************************************************/
#define RF_TDSCDMACM_MDSP_BUFFER_CM_START_INDEX          0
#define RF_TDSCDMACM_MDSP_BUFFER_CM_STOP_INDEX           36

#define RF_TDSCDMACM_MDSP_BUFFER_IFREQ_START_INDEX       0
#define RF_TDSCDMACM_MDSP_BUFFER_IFREQ_LAST_INDEX        495
#define RF_TDSCDMACM_MDSP_BUFFER_IFREQ_NUM_SETUP         3

#define RF_TDSCDMACM_MDSP_TTOT_SETUP_SIZE                \
  ((RF_TDSCDMACM_MDSP_BUFFER_IFREQ_LAST_INDEX-RF_TDSCDMACM_MDSP_BUFFER_IFREQ_START_INDEX+1)/ \
  RF_TDSCDMACM_MDSP_BUFFER_IFREQ_NUM_SETUP)

#define RF_TDSCDMACM_MDSP_IFREQ_BUF_SIZE                 (RF_TDSCDMACM_MDSP_TTOT_SETUP_SIZE*2)
#define RF_TDSCDMACM_MDSP_IFREQ_BUF_SIZE_MAX             RF_TDSCDMACM_MDSP_IFREQ_BUF_SIZE

typedef enum
{
  RF_TDSCDMA_RECEIVE_CHAIN_0,
  RF_TDSCDMA_RECEIVE_CHAIN_1
} rf_tdscdma_receive_chain_type;
#endif

#endif  /* RF_TDSCDMA_H */
