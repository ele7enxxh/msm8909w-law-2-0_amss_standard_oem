
/*!
  @file
  rflm_wcdma_log.h

  @brief
  FED WCDMA TXAGC API for Binary Logging
  
*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies, Inc. (QTI). All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/rftech/wcdma/inc/rflm_wcdma_log.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/25/14   bn      V1 log packet update (therm bin/timeline emergency info)
05/24/14   bn      Initial Version - Support for WCDMA TxAGC binary logging


==============================================================================*/

#ifndef RFLM_WCDMA_LOG_H
#define RFLM_WCDMA_LOG_H


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "rflm.h"
#include "rflm_time.h"
#include "rflm_wcdma_ccs.h"
#include "rflm_wcdma_sw_api.h"
#include "log.h"

/*==============================================================================

                EXTERNAL DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/************************************************************/
/*                LOG PACKET IDENTIFICATION                 */
/************************************************************/

/*----------------------------------------------------------------------------*/
/*! @brief RFLM WCDMA TxAGC log packet ID */
#define RFLM_LOG_PACKET_WCDMA_TXAGC_ID                  0x1842 

/*----------------------------------------------------------------------------*/
/*! @brief RFLM WCDMA TxAGC user ID */
#define RFLM_LOG_PACKET_RAT_WCDMA_TXAGC                 0

/*----------------------------------------------------------------------------*/
/*! @brief RFLM WCDMA TxAGC tag size */
#define RFLM_LOG_PACKET_WCDMA_TXAGC_TAG_SIZE_WORDS      0


#define RFLM_TIME_PROFILE_US10(x,y) ((rflm_time_ustmr_count_diff(x,y))*100/192)

/*----------------------------------------------------------------------------*/

/************************************************************/
/*                   LOG PACKET VERSIONS                    */
/************************************************************/

/*----------------------------------------------------------------------------*/
/*!@brief Enumeration indicating the WCDMA TXAGC Log Packet version */
typedef enum
{
  RFLM_WCDMA_TXAGC_LOG_VER_1 = 1, /*!< WCDMA TXAGC Log Packet Version 1 */
  //RFLM_WCDMA_TXAGC_LOG_VER_2 = 2, /*!< WCDMA TXAGC Log Packet Version 2 */
} rflm_wcdma_txagc_log_version_type;

/*----------------------------------------------------------------------------*/

/*==============================================================================

              EXTERNAL DEFINITIONS AND TYPES : ENUMS

==============================================================================*/


/*==============================================================================

            EXTERNAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/************************************************************/
/*              LOG PACKET PAYLOAD DEFINITION               */
/************************************************************/

/*----------------------------------------------------------------------------*/
/*! @brief  WCDMA TxAGC Log packet structure Version 1 */
/*! prefix t_ denotes variables in ustrm time          */
/*! prefix p_ denotes variables in timeprofile in us10 */

typedef struct
{
  int16  txagc;  ///< TxAGC Pwr

  uint16 err        :4; ///< RFLM TxAGC update API err status
  uint16 waveform   :4; ///< Waveform type
  uint16 xpt        :4; ///< XPT mode
  uint16 pa         :4; ///< PA state

  uint32 paaction   :4; ///< PA action
  uint32 iqgain     :8; ///< IQ gain
  uint32 envscale   :8; ///< Envelope Scale
  uint32 mpr        :4; ///< MPR bin
  uint32 offset     :8; ///< txagc offset
  uint32 pampr      :8; ///< pa mpr shift factor
  uint32 rgi        :8; ///< RGI Row Index
  uint32 smps       :8; ///< SMPS Row Index
  uint32 dpd        :8; ///< DPD table index

  int8   pacomp;        ///< PA compensate value
  int8   pin;           ///< Pin value 
  int8   pout;          ///< Pout value
  uint8  hyscnt;        ///< PA hystersis count

  uint32 bin        :3; ///< Temp bin
  uint32 rem        :6; ///< Temp rem
  uint32 runfbrx    :1; ///< Run FBRx
  
  uint32 p_calc    :10; ///< Time profile in us10 of FED Calc API
  uint32 p_update  :10; ///< Time profile in us10 of FED Update API

  uint32 p_etdac    :8; ///< Time profile in us10 of FED calc - ET DAC programming
  uint32 p_fbrx     :8; ///< Time profile in us10 of FED calc - FBRx LDO event enqueue

  uint32 p_pastm    :8; ///< Time profile in us10 of FED update - PA state machines
  uint32 p_lookup   :8; ///< Time profile in us10 of FED update - TxAGC Lookup
  uint32 p_fixup    :8; ///< Time profile in us10 of FED update - CMN Fixup
  uint32 p_mdmcfg   :8; ///< Time profile in us10 of FED update - Modem config programming
  uint32 p_iref     :8; ///< Time profile in us10 of FED update - DAC IREF programming
  
  uint32 t_calc    :22; ///< FED TXAGC Calc end time in USTRM units
  uint32 t_fixup   :22; ///< FED TXAGC Fixup end time in USTRM units
  uint32 t_action  :22; ///< FED TXAGC CCS trigger time in USTRM units
  uint32 t_update  :22; ///< FED TXAGC Update end time in USTRM units
  uint32 t_effect  :22; ///< FED TXAGC CCS effect time in USTRM units
  uint32 t_callback:22; ///< FED TXAGC Update callback time in USTRM units

  uint32 t_deficit  :9; ///< TxAGC deficit value in USTRM units
  uint32 sos        :1; ///< FED TxAGC Timeline emergency - SOS
  
  uint32 skip;          ///< FED TxAGC update skip count
  uint32 reserved;
}rflm_wdma_txagc_log_packet_v1;

/*----------------------------------------------------------------------------*/
/*!  @brief WCDMA TXAGC log packet structure */

/// @log_id 0x184B
typedef struct
{
  uint8  log_version;              /*!< Log Packet Version */
  /// @descriminator log_version
  union  rflm_wcdma_txagc_log_packet_versions
  {
    rflm_wdma_txagc_log_packet_v1 v1;   /*!< Log Packet Version 1 */
    //rflm_wdma_txagc_log_packet_v2 v2;   /*!< Log Packet Version 2 */
  } packet;

}rflm_wcdma_txagc_log_packet_t;


/*----------------------------------------------------------------------------*/

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
            API input/output function definitions -- WCDMA TXAGC 
==============================================================================*/

/************************************************************/
/*           DIAG LOG SUBPACKET API DEFINITIONS             */
/************************************************************/

/*----------------------------------------------------------------------------*/

void
rflm_wcdma_txagc_diag_log_subpacket
(
  rflm_dm_handle_id_t handle,
  rflm_ustmr_t t_callback
);

/*----------------------------------------------------------------------------*/

void
rflm_wcdma_txagc_diag_log_packet_allocate
(
  rflm_wcdma_tx_dm_buf_type *w_tx_dm
);

/*----------------------------------------------------------------------------*/

void
rflm_wcdma_txagc_diag_log_packet_deallocate
(
  rflm_wcdma_tx_dm_buf_type *w_tx_dm
);

/*----------------------------------------------------------------------------*/


#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RFLM_WCDMA_TXAGC_LOG_H */
