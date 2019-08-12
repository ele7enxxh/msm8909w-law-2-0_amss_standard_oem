#ifndef RFUMTS_H
#define RFUMTS_H

/*===========================================================================


        R F  D r i v e r  U M T S  E x t e r n a l  I n t e r f a c e
                          H e a d e r  F i l e

DESCRIPTION
  This file contains all declarations and definitions necessary to use
  the UMTS specific RF drivers.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  See rfm.h

Copyright (c) 1999 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6200/DRIVERS/vcs/rfumts.h_v   1.0   01 Jul 2002 18:16:50   doerfler  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/wcdma/rfumts.h#1 $ $DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/16/12   jl      Add max UTRAN for FTM RF mode
05/15/12   sa      Added RF_RXAGC_RESOLUTION
12/02/09   ckl     Changed W2W interfrequency related definitions.
07/23/09    bn     Setting max power to 23 dBm
10/17/06   ycl     Modifications for initial MSM7600 build.
08/31/05    ka     Mainline FEATURE_WCDMA_MAX_TX_PWR_NV_LIM_FIX.
03/08/05   jtn     Single RF driver.
08/25/04    xw     Added RFUMTS_DEFAULT_CHAN_NUM for W1800 band.
06/07/04    eh     Added support for 800Mhz.
03/01/04   dhh     Provided interface for WL1 to get tx rotator angle from RFNV.
01/06/04   eh      Added rf_init_status_type.
12/13/02   sd      Sync'ed up default channel number definition with the
                   latest version of rf.h.
11/07/02   sd      Removed GSM to WCDMA intersystem start and stop alpha
                   constants and placed in rfgsm.h.
11/06/02   sd      Exported two functions, one to get maximum Tx power and
                   one to convert Tx dBm to AGC units.
11/04/02   sd      Added support for intersystem measurements.
09/10/02   sd      Added support for multimode API that includes GSM drivers.
07/01/02   sd      Initial revision.

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  UMTS specific constant definitions.
---------------------------------------------------------------------------*/
#define RFAGC_MIN_AGC_VAL            -512
#define RFAGC_MAX_AGC_VAL            511

#define RF_NV_TXAGC_MIN               0
#define RF_NV_TXAGC_MAX             1023

/* Minimum WCDMA RX Power Level based on UE dynamic range in dbm */
#define RF_MIN_RX_POWER_DBM      -106

/* Maximum WCDMA RX Power Level based on UE dynamic range in dbm  */
#define RF_MAX_RX_POWER_DBM      -21

/* Minimum WCDMA TX Power Level based on UE dynamic range in dbm */
#define RF_MIN_TX_POWER_DBM      -70

/* TxAGC Resolution */
#define RF_TXAGC_RESOLUTION      10

/* RxAGC Resolution */
#define RF_RXAGC_RESOLUTION      10

/* Maximum TX power that UE is capable to transmit based on UE power class  in db10 */
/* 23dBm => (23+70)*10 = 930 */
#define RF_UE_MAX_TX_POWER_DB10  930

/* Maximum TX power that UE is capable to transmit based on UE power class  in dbm */
#define RF_UE_MAX_TX_POWER       23

/* Minimum TX power that UE is capable to transmit based on UE power class  in dbm */
#define RF_UE_MIN_TX_POWER       -57

/* Maximum TX power that UE is capable to transmit based on UE power
   class  in dbm */
#define RFUMTS_UE_MAX_TX_POWER       RF_UE_MAX_TX_POWER

/* Minimum TX power -52 dBm UE is capable to transmit in db10 */
/* -52dBm => (-52+70)*10 - 512 = -332 (in MDSP units) */
#define RF_UE_MIN_TX_POWER_DB10  -332

#define RF_SATURATE( val, min, max )  MAX( MIN( (val), (max) ), (min) )

#define RF_UTRAN_MAX_TX_POWER_FTM_RF_MODE 30

/*******************************************************************************
 * 000 - 035  -- W2G start memory mapping
 * 036 - 074  -- W2G stop memory mapping
 *******************************************************************************/
#define RFWCDMACM_MDSP_BUFFER_CM_START_INDEX          0
#define RFWCDMACM_MDSP_BUFFER_CM_STOP_INDEX           36

#define RFWCDMACM_MDSP_BUFFER_IFREQ_START_INDEX       0
#define RFWCDMACM_MDSP_BUFFER_IFREQ_LAST_INDEX        495
#define RFWCDMACM_MDSP_BUFFER_IFREQ_NUM_SETUP         3

#define RFWCDMACM_MDSP_WTOW_SETUP_SIZE                \
  ((RFWCDMACM_MDSP_BUFFER_IFREQ_LAST_INDEX-RFWCDMACM_MDSP_BUFFER_IFREQ_START_INDEX+1)/ \
  RFWCDMACM_MDSP_BUFFER_IFREQ_NUM_SETUP)

#define RFWCDMACM_MDSP_IFREQ_BUF_SIZE                 (RFWCDMACM_MDSP_WTOW_SETUP_SIZE*2)
#define RFWCDMACM_MDSP_IFREQ_BUF_SIZE_MAX             RFWCDMACM_MDSP_IFREQ_BUF_SIZE

typedef enum
{
  RFAGC_RECEIVE_CHAIN_0,
  RFAGC_RECEIVE_CHAIN_1
} rfagc_receive_chain_type;

#endif  /* RFUMTS_H */
