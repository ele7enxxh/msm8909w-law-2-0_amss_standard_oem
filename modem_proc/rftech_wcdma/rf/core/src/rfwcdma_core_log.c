/*!
  @file
  rfwcdma_core_log.c

  @brief
  This file contains all declarations and definitions pertaining to WCDMA
  RF logging
*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/core/src/rfwcdma_core_log.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/08/15   sd      Replace deprecated API rfwcdma_mdsp_get_lna_range() usage
11/26/14   zhh    add max carrier num check before call rfwcdma_core_util_get_rxagc()
11/19/14   vs      Featurize the AGC logging as per the modem support
03/19/14   kr      ASDiv: Logging issue with Ant Pos
01/28/14   vs      Fix incorrect LNA state query for path1
06/30/14   kai     Fix immediate AGC logging
06/18/14   kai     Add AGC Subpacket Version 2
05/01/14   rmb     Add support for new version of AGC log packet 0x4176 v2.
04/11/14   dw      Add Tx ON logging from RFLM
02/04/14   kai     Add xpt logging support
12/17/13   kr      added car_id support to rfwcdma_mdsp_is_tx_accum_frozen()
12/05/13   bn      Add IQ gain logging support
12/04/13   dw      Consolidate all get RxAGC core functions. Move all RxAGC logging through single API
12/04/13   kr      set txagc_frozen_ch0ch1 to 1 
10/28/13   dw      Add LNA state logging support
09/10/13   dw      Remove DBDC featurization and code clean up.
08/23/13   vs      Added support for 3 carrier
08/13/13   aro     Extended log packet definition to support subpacket
07/31/13   aro     Fixed antenna switch position query function call
07/30/13   aro     Doxygen grouping
07/30/13   aro     Added debug mode to print F3 during logging
07/30/13   aro     Fixed the invalid NULL pointer check
07/30/13   aro     Added function to log AGC once
07/29/13   aro     Implementation to populate AGC param
07/29/13   aro     Converted AGC chunk to bitwise field in the structure
07/29/13   aro     Updated log packet payload
07/29/13   aro     Documentation of log packet fields
07/24/13   aro     Consolidated all data structure to one
07/24/13   aro     Added initial framework to support AGC logging for WCDMA
07/24/13   aro     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfwcdma_core_log.h"
#include "rfwcdma_core_txplim.h"
#include "rfwcdma_core_util.h"
#include "rfwcdma_mdsp.h"
#include "rfcommon_msg.h"
#include "rfcommon_log.h"
#include "rfcommon_asdiv_manager.h"
#include "log.h"
#include "seq.h"
#include "log_codes_wcdma_int.h"

/*----------------------------------------------------------------------------*/
/*! Enumeration indicating the maximum number of AGC samples that can be 
part of each log packet */
#define RFWCDMA_LOG_AGC_SAMPLES 50

/*----------------------------------------------------------------------------*/
/*! Enumeration indicating the number of AGC samples in immediate AGC 
log packet */
#define RFWCDMA_LOG_AGC_ONE_INSTANCE_SAMPLES 1

/*----------------------------------------------------------------------------*/
/*! Enumeration indicating the AGC version */
typedef enum
{
  RFWCDMA_LOG_AGC_VER_1 = 1, /*!< AGC Version 1 */
  RFWCDMA_LOG_AGC_VER_2 = 2, /*!< AGC Version 2 */
  RFWCDMA_LOG_AGC_VER_3 = 3  /*!< AGC Version 3 */
} rfwcdma_log_agc_version_type;

/*----------------------------------------------------------------------------*/
/*! Enumeration indicating the AGC version */
typedef enum
{
  RFWCDMA_LOG_AGC_SUBPACKET_VER_1 = 1, /*!< AGC Subpacket Version 1 */
  RFWCDMA_LOG_AGC_SUBPACKET_VER_2 = 2, /*!< AGC Subpacket Version 2 */
} rfwcdma_log_agc_subpacket_version_type;

/*----------------------------------------------------------------------------*/
/*! Enumeration indicating the AGC version */
typedef enum
{
  RFWCDMA_LOG_AGC_CUMULATIVE = 1, /*!< Subpacket holding all AGC data */
} rfwcdma_log_agc_subpacket_id_type;

/*============================================================================*/
/*!
  @addtogroup RFWCDMA_LOG_AGC_PACKET_DEFN
 
  @brief
  This module contains the definition of AGC log packet. This will be the
  interface between embedded side and tools to transfer AGC log parameter
  @{
*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Header structure for RF log sub-packets.

  @details
  This structure is the header for any RF sub-packet, contains information about 
  the payload that follows
*/
typedef PACK(struct)
{
  uint8 pkt_id;
  /*!< Sub-packet ID, a unique sub-packet identifier*/ 

  uint8 pkt_ver;
  /*!< Sub-packet version number, if the version is unknown to the parsing tool 
 the sub-packet will be skipped */ 

  uint16 pkt_size;
  /*!< Sub-packet size, size in bytes of the sub-packet, including the header. 
  If the version of the sub-packet is unknown this size can be used to skip the 
  subpacket and decode the next one */ 

} rfwcdma_log_sub_pkt_header_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold the AGC parameter */
typedef PACK(struct)
{
  PACK(struct)
  {
    uint16 agc_status_ch0cr0:1; 
    /*!< Chain 0 Carrier 0 AGC status */

    uint16 agc_status_ch0cr1:1;
    /*!< Chain 0 Carrier 1 AGC status */

    uint16 agc_status_ch0cr2:1;
    /*!< Chain 0 Carrier 2 AGC status */

    uint16 agc_status_ch0cr3:1;
    /*!< Chain 0 Carrier 3 AGC status */

    uint16 agc_status_ch1cr0:1;
    /*!< Chain 1 Carrier 0 AGC status */

    uint16 agc_status_ch1cr1:1;
    /*!< Chain 1 Carrier 1 AGC status */

    uint16 agc_status_ch1cr2:1;
    /*!< Chain 1 Carrier 2 AGC status */

    uint16 agc_status_ch1cr3:1;
    /*!< Chain 1 Carrier 3 AGC status */

    uint16 :8;
    /*!< Reserved */

  }agc_info_chunk_0;
  /*!< 16-bit field to store AGC information */

  PACK(struct)
  {
    uint16 lna_state_ch0lna1:3;
    /*!< LNA state for a given LNA#1 on a Chain 0. During  four carrier 
    dual band (DB-4C) with diversity operation, upto 4 LNA can be 
    used simultaneously. Allocation of LNA for each carrier may 
    differ based on the mode of operation. */ 

    uint16 lna_state_ch0lna2:3;
    /*!< LNA state for a given LNA#2 on a Chain 0  */

    uint16 lna_state_ch1lna1:3;
    /*!< LNA state for a given LNA#1 on a Chain 1  */

    uint16 lna_state_ch1lna2:3;
    /*!< LNA state for a given LNA#2 on a Chain 1 */

    uint16 :4;
    /*!< Reserved */

  }agc_info_chunk_1;
  /*!< 16-bit field to store AGC information */

  PACK(struct)
  {
    uint16 pa_on:2;
    /*!< Indicates if PA is in ON state or not */

    uint16 pa_range:4;
    /*!< Current PA Range */

    uint16 tx_on:1;
    /*!< Indicates if the Tx is ON */

    uint16 txagc_frozen_ch0cr0:1;
    /*!< Flag indicating if the TxAGC is in frozen state for 
    Chain 0 and carrier 0 */

    uint16 txagc_frozen_ch0cr1:1;
    /*!< Flag indicating if the TxAGC is in frozen state for 
    Chain 0 and carrier 1 */

    uint16 :7;
    /*!< Reserved */

  } agc_info_chunk_2;
  /*!< 16-bit field to store AGC information */

  int16 rx_agc_chain0_carr0;
  /*!< Most recent RxAGC value for the given Chain 0  and Carrier 1. 
  This field will be valid only if its corresponding AGC status field is 
  enabled. 
   
  For conversion to dBm unit, following formula should be used 
   
             RxAGC_dBm = -106 + (RxAGC + 512)/10,
   
  where RxAGC is the output from the target. */

  int16 rx_agc_chain0_carr1;
  /*!< Most recent RxAGC value for the given Chain 0  and Carrier 1. 
  This field will be valid only if its corresponding AGC status field is 
  enabled. */

  int16 rx_agc_chain0_carr2;
  /*!< Most recent RxAGC value for the given Chain 0  and Carrier 2. 
  This field will be valid only if its corresponding AGC status field is 
  enabled. */

  int16 rx_agc_chain0_carr3;
  /*!< Most recent RxAGC value for the given Chain 0  and Carrier 3. 
  This field will be valid only if its corresponding AGC status field is 
  enabled. */

  int16 rx_agc_chain1_carr0;
  /*!< Most recent RxAGC value for the given Chain 1  and Carrier 0. 
  This field will be valid only if its corresponding AGC status field is 
  enabled. */

  int16 rx_agc_chain1_carr1;
  /*!< Most recent RxAGC value for the given Chain 1  and Carrier 1. 
  This field will be valid only if its corresponding AGC status field is 
  enabled. */

  int16 rx_agc_chain1_carr2;
  /*!< Most recent RxAGC value for the given Chain 1  and Carrier 2. 
  This field will be valid only if its corresponding AGC status field is 
  enabled. */

  int16 rx_agc_chain1_carr3;
  /*!< Most recent RxAGC value for the given Chain 1  and Carrier 3. 
  This field will be valid only if its corresponding AGC status field is 
  enabled. */

  int16 tx_agc_chain0_carr0;
  /*!< Tx output power for chain 0 carrier 0. This field can be converted to dBm 
  using
   
             Tx_dBm = -70 + (TX_AGC + 512)/10,
   
  where TX_AGC is the output from the target. Vailidy of this
  field will be based on PA ON */

  int16 tx_agc_chain0_carr1;
  /*!< Tx output power for chain 0 carrier 1. Vailidy of this
  field will be based on PA ON */

  int16 tx_agc_total;
  /*!< Total Tx output power */

  int16 tx_agc_lut_idx;
  /*!< This is the PDM value to adjust TX AGC gain. Will be valid only  if 
  “TX_ON” is active */

  int16 tx_pwr_lim;
  /*!< Maximum Tx Power limit (-512 to 511) in 1/12 dBm steps 
   
             Tx Power = -70 + (MAX_TX + 512)/10 in dBm
   
  Will be valid only  if “TX_ON” is 1 */ 

  int16 hdet;
  /*!< This is the raw HDET conversion obtained from the power detector device.
  Will be active only  if “TX_ON” is 1 */

  int16 iq_gain;
  /*!< Digital IQ Gain value applied in Tx signal path */

  uint16 dpd_index;
  /*!< DPD Index applied in Tx */

  int16 env_scale;
  /*!< Envelope Scale value applied in Tx signal path */

  uint16 pa_current;
  /*!< Quiescent current applied to power amplifier */

  uint32 pa_bias;
  /*!< Bias applied to power amplifier */

  uint8 antenna_position;
  /*!< Current Antenna switch position */

  uint8 xpt_mode;
  /*!< Current xPT mode: APT, EPT or ET */

  int16 pin_comp;
  /*!< Pin Compensation value */

  int16 pout_comp;
  /*!< Pout Compensation value */

  uint8 pin_rgi;
  /*!< Pin RGI value */

  int16 delay;
  /*!< Delay */

} rfwcdma_log_agc_param_type;

/*----------------------------------------------------------------------------*/
/*! Definition for AGC log sub packet */
typedef PACK(struct)
{
  rfwcdma_log_sub_pkt_header_type header;
  /*!< Sub-packet header*/ 

  uint8 num_samples;
  /*!< Number of samples of AGC data in the log packet*/

  uint8 start_cfn; 
  /*!< Frame number from which the logging was started for this log */

  rfwcdma_log_agc_param_type agc_params[RFWCDMA_LOG_AGC_SAMPLES];
  /*!< AGC parameter */

} rfwcdma_log_agc_sub_packet_type;

/*----------------------------------------------------------------------------*/
/*! Structure defining the log structure for AGC log */
typedef PACK(struct)
{
  rfcommon_log_rf_header_type rf_header;
  /*!< RF log packet header, this contains information about the version of RF 
  log packet and the number of sub packets */ 

  rfwcdma_log_agc_sub_packet_type agc_sub_pkt;
  /*!< Structure to hold AGC sub packet */

} rfwcdma_log_agc_packet_type;

/*! @} */

/*----------------------------------------------------------------------------*/
/*! Count the Log Packet length */
#define RFWCDMA_LOG_AGC_V3_LENGTH( num_sample )  \
(\
  FPOS( rfwcdma_log_agc_sub_packet_type, agc_params ) + \
  ( num_sample * sizeof( rfwcdma_log_agc_param_type ) ) \
)

/*----------------------------------------------------------------------------*/
/*! Data structure to hopld housekeeping data for WCDMA AGC log */
typedef struct
{
  boolean debug_mode;
  /*!< Flag indicating debug mode */

  word agc_log_code;
  /*!< Variable to store the log code for AGC logging */
  
  uint16 counter;
  /*!< Variable to store the counter of AGC param entery */

  uint16 num_of_sub_packet;
  /*!< Variable to keep track of number of subpackets */
  
  rfwcdma_log_agc_packet_type log_packet;
  /*!< Data struture to hold the log packet data */

} rfwcdma_log_agc_type;

/*----------------------------------------------------------------------------*/
/*! Struture to hold housekeeping data for all WCDMA log */
typedef struct
{
  rfwcdma_log_agc_type agc_log;
  /*!< Struture to hold housekeeping data for WCDMA AGC log */

} rfwcdma_log_data_type;

/*----------------------------------------------------------------------------*/
/*! Variable to hold housekeeping data for all WCDMA log */
static rfwcdma_log_data_type rfwcdma_log_data;

/*============================================================================*/
/*!
  @addtogroup RFWCDMA_LOG_AGC_INT_HELPER
 
  @brief
  This module contains the internal helper functions to be used during AGC 
  logging
  @{
*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get pointer to AGC log data
 
  @details
  This function returns the pointer to the AGC log data
 
  @return
  Pointer to AGC log data
*/
static rfwcdma_log_agc_type*
get_agc_log_data
(
  void
)
{
  return &(rfwcdma_log_data.agc_log);
} /* get_agc_log_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize AGC log
 
  @return
  Flag indicating the status of initialization procedure
*/
static boolean
init_agc_log
(
  void
)
{
  rfwcdma_log_agc_type* log_data = NULL; /* Pointer to AGC Log */

  /* Get Pointer to log data */
  log_data = get_agc_log_data();

  log_data->debug_mode = FALSE;

  /* Enable WCDMA AGC log code for logging */

  /* Init Housekeeping Data */
  log_data->agc_log_code = WCDMA_AGC_0x4176_LOG_PACKET;
  log_data->num_of_sub_packet = 1;
  log_data->counter = 0;

  /* Init Log Packet RF HEADER */
  log_data->log_packet.rf_header.pkt_version = RFWCDMA_LOG_AGC_VER_2;
  log_data->log_packet.rf_header.num_sub_pkt = 1;
  log_set_code( &(log_data->log_packet), log_data->agc_log_code );

  return TRUE;

} /* init_agc_log */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Check if AGC log is enabled
 
  @param log_data
  Pointer to data structure containing AGC log data
 
  @return
  Flag indicating if the log is enabled or not
*/
static boolean
is_agc_log_enabled
(
  rfwcdma_log_agc_type* log_data
)
{
  if ( log_data != NULL )
  {
    return log_status( log_data->agc_log_code ) ;
  }
  else
  {
    return FALSE;
  }
} /* is_agc_log_enabled */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Compute the total log size
 
  @param agc_log
  Pointer to data structure containing the log data
 
  @return
  Total size of the log packet
*/
static uint16
compute_log_size
(
  rfwcdma_log_agc_type *agc_log
)
{

  uint16 tot_size = 0;
  uint8 sp_counter = 0;
  uint8 *log_ptr = NULL;
  rfwcdma_log_sub_pkt_header_type *sp_header;

  tot_size += sizeof(rfcommon_log_rf_header_type);

  /* Point to Log Packet */
  log_ptr = (uint8*)(&(agc_log->log_packet));

  /* Pointer to First Subpacket */
  log_ptr += sizeof(rfcommon_log_rf_header_type);

  for ( sp_counter = 0; sp_counter < agc_log->num_of_sub_packet; sp_counter++ )
  {
    sp_header = (rfwcdma_log_sub_pkt_header_type*)log_ptr;
    tot_size += sp_header->pkt_size;
    log_ptr += sp_header->pkt_size;
  } /* for ( sp_counter = 0; sp_counter < agc_log->num_of_sub_packet; 
  sp_counter++ ) */

  RF_MSG_1( RF_HIGH, "Total Log Size %d", tot_size );

  return tot_size;

} /* compute_log_size */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Flush AGC log
 
  @details
  This function is called from AGC log function to flush log to diag.
 
  @param agc_log
  Pointer to data stucture containing AGC log data
 
  @return
  Flag indicating the status of log flush
*/
static boolean 
flush_agc_log
(
  rfwcdma_log_agc_type *agc_log
)
{
  boolean ret_val = TRUE; /* Return value */

  /* NULL Pointer check */
  if ( agc_log != NULL )
  {
    agc_log->log_packet.agc_sub_pkt.num_samples = agc_log->counter;
    agc_log->log_packet.agc_sub_pkt.header.pkt_size = 
                      RFWCDMA_LOG_AGC_V3_LENGTH(agc_log->counter);

    log_set_length( &(agc_log->log_packet), compute_log_size(agc_log) ); 

    /* Commit Log */
    if (!log_submit((PACKED void *) &agc_log->log_packet))
    {
      RF_MSG( RF_ERROR, "flush_agc_log: Log Commit Failure");
      ret_val = FALSE;
    } /* if (!log_submit((PACKED void *) &agc_log->log_packet)) */

    /* Reset the counter */
    agc_log->log_packet.agc_sub_pkt.num_samples = 0;
    agc_log->counter = 0;
  } /*  if ( agc_log != NULL )*/
  else
  {
    RF_MSG( RF_ERROR, "flush_agc_log: NULL AGC log data");
    ret_val = FALSE;
  } /* if ! ( agc_log != NULL ) */

  return ret_val;
} /* flush_agc_log */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Fetch AGC parameter
 
  @details
  This function is a helper function for WCDMA AGC logging function to
  propulate the AGC parameter fields with the most recent values.
 
  @param param_ptr
  Pointer to the structure where the agc parameter will be stored
 
  @param debug_mode
  Flag indicating if logging is done in debug mode
*/
static void 
rfwcdma_core_log_get_agc_param
(
  rfwcdma_log_agc_param_type *param_ptr,
  boolean debug_mode
)
{
  int16 rxagc_val = 0;
  rfwcdma_core_util_txagc_logging_type txagc_data;

  /* Parameter validation */
  if ( param_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "rfwcdma_core_log_get_agc_param : NULL pointer" );
    return;
  } /* if ( param_ptr == NULL ) */

  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Populate bit fields from AGC Chunk 1 */
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

  #ifdef FEATURE_WCDMA_DB_DC_HSDPA
  param_ptr->agc_info_chunk_1.lna_state_ch0lna1 = 
               rfwcdma_core_util_get_lna_state( RFM_DEVICE_0 );
  #else
  param_ptr->agc_info_chunk_1.lna_state_ch0lna1 = 
               rfwcdma_core_util_get_lna_state( RFM_DEVICE_0 );
  #endif /*FEATURE_WCDMA_DB_DC_HSDPA */
               
  #ifdef FEATURE_WCDMA_DB_DC_HSDPA               
  param_ptr->agc_info_chunk_1.lna_state_ch0lna2 =  
               rfwcdma_core_util_get_lna_state( RFM_DEVICE_2 );
  #else
  param_ptr->agc_info_chunk_1.lna_state_ch0lna2 = 0;
  #endif
                 
  #ifdef FEATURE_WCDMA_DB_DC_HSDPA
  param_ptr->agc_info_chunk_1.lna_state_ch1lna1 = 
               rfwcdma_core_util_get_lna_state( RFM_DEVICE_1 );
  #else
  param_ptr->agc_info_chunk_1.lna_state_ch1lna1 = 
               rfwcdma_core_util_get_lna_state( RFM_DEVICE_1 );
  #endif /* FEATURE_WCDMA_DB_DC_HSDPA */
                 
  #ifdef FEATURE_WCDMA_DB_DC_HSDPA
  param_ptr->agc_info_chunk_1.lna_state_ch1lna2 = 
               rfwcdma_core_util_get_lna_state( RFM_DEVICE_3 );
  #else
  param_ptr->agc_info_chunk_1.lna_state_ch1lna2 = 0;
  #endif
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Populate bit fields from AGC Chunk 2 */
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

  param_ptr->agc_info_chunk_2.pa_on = rfwcdma_core_util_get_rflm_tx_on();

  param_ptr->agc_info_chunk_2.pa_range = rfwcdma_core_util_get_rflm_pa_range();
  param_ptr->agc_info_chunk_2.tx_on = rfwcdma_core_util_get_rflm_tx_on();
  param_ptr->agc_info_chunk_2.txagc_frozen_ch0cr0 = 
                                 rfwcdma_mdsp_is_tx_accum_frozen(RFCOM_MULTI_CARRIER_ID__0);
  #if  defined(FEATURE_TRITON_MODEM) || defined(FEATURE_JOLOKIA_MODEM)
  param_ptr->agc_info_chunk_2.txagc_frozen_ch0cr1 = 1;
  #else
  param_ptr->agc_info_chunk_2.txagc_frozen_ch0cr1 = rfwcdma_mdsp_is_tx_accum_frozen(RFCOM_MULTI_CARRIER_ID__1);
  #endif



  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Populate bit fields from AGC Chunk 3 */
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

  /* None for now */

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Populate bit fields from AGC Chunk 0 and Populate rest of the payload */
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

  param_ptr->agc_info_chunk_0.agc_status_ch0cr0 = 
               rfwcdma_core_util_get_rxagc( RFAGC_RECEIVE_CHAIN_0, 
                                            RFCOM_SERVINGCELL_RXAGC, 
                                            RFCOM_MULTI_CARRIER_ID__0, 
                                            RFCOM_RXAGC_IN_DB10,
                                            &rxagc_val );
  param_ptr->rx_agc_chain0_carr0 = rxagc_val;

  param_ptr->agc_info_chunk_0.agc_status_ch0cr1 = 
               rfwcdma_core_util_get_rxagc( RFAGC_RECEIVE_CHAIN_0, 
                                            RFCOM_SERVINGCELL_RXAGC, 
                                            RFCOM_MULTI_CARRIER_ID__1, 
                                            RFCOM_RXAGC_IN_DB10,
                                            &rxagc_val );
  param_ptr->rx_agc_chain0_carr1 = rxagc_val;

  if (RF_WCDMA_MAX_NUM_RX_CARRIERS >= RFCOM_MAX_CARRIERS)
  {
    param_ptr->agc_info_chunk_0.agc_status_ch0cr2 = 
                 rfwcdma_core_util_get_rxagc( RFAGC_RECEIVE_CHAIN_0, 
                                              RFCOM_SERVINGCELL_RXAGC, 
                                              RFCOM_MULTI_CARRIER_ID__2, 
                                              RFCOM_RXAGC_IN_DB10,
                                              &rxagc_val );       
    param_ptr->rx_agc_chain0_carr2 = rxagc_val;
  }
  else
  {
    param_ptr->agc_info_chunk_0.agc_status_ch0cr2 = 0;
    param_ptr->rx_agc_chain0_carr2 = 0;
  }

  param_ptr->agc_info_chunk_0.agc_status_ch0cr3 = 0;
  param_ptr->rx_agc_chain0_carr3 = 0;

  param_ptr->agc_info_chunk_0.agc_status_ch1cr0 = 
               rfwcdma_core_util_get_rxagc( RFAGC_RECEIVE_CHAIN_1, 
                                            RFCOM_SERVINGCELL_RXAGC, 
                                            RFCOM_MULTI_CARRIER_ID__0, 
                                            RFCOM_RXAGC_IN_DB10,
                                            &rxagc_val );
  param_ptr->rx_agc_chain1_carr0 = rxagc_val;

  param_ptr->agc_info_chunk_0.agc_status_ch1cr1 = 
               rfwcdma_core_util_get_rxagc( RFAGC_RECEIVE_CHAIN_1, 
                                            RFCOM_SERVINGCELL_RXAGC, 
                                            RFCOM_MULTI_CARRIER_ID__1, 
                                            RFCOM_RXAGC_IN_DB10,
                                            &rxagc_val );
  param_ptr->rx_agc_chain1_carr1 = rxagc_val;

  if (RF_WCDMA_MAX_NUM_RX_CARRIERS >= RFCOM_MAX_CARRIERS)
  {
    param_ptr->agc_info_chunk_0.agc_status_ch1cr2 = 
                 rfwcdma_core_util_get_rxagc( RFAGC_RECEIVE_CHAIN_1, 
                                              RFCOM_SERVINGCELL_RXAGC, 
                                              RFCOM_MULTI_CARRIER_ID__2, 
                                              RFCOM_RXAGC_IN_DB10,
                                              &rxagc_val );       
    param_ptr->rx_agc_chain1_carr2 = rxagc_val;
  }
  else
  {
    param_ptr->agc_info_chunk_0.agc_status_ch1cr2 = 0;
    param_ptr->rx_agc_chain1_carr2 = 0;
  }

  param_ptr->agc_info_chunk_0.agc_status_ch1cr3 = 0;
  param_ptr->rx_agc_chain1_carr3 = 0;
             
    param_ptr->tx_agc_chain0_carr0 = rfwcdma_core_util_get_rflm_txagc_dB10();
  param_ptr->tx_agc_chain0_carr1 = 0;
  param_ptr->tx_agc_total = param_ptr->tx_agc_chain0_carr0;

  rfwcdma_core_util_get_rflm_txagc_logging(&txagc_data);
  param_ptr->tx_agc_lut_idx = txagc_data.rgi;
  param_ptr->tx_pwr_lim = rfwcdma_mdsp_get_tx_power_limit();
  param_ptr->hdet = rfwcdma_core_txplim_get_hdet();
  param_ptr->iq_gain = txagc_data.iq_gain;
  param_ptr->dpd_index = txagc_data.dpd_index;
  param_ptr->env_scale = txagc_data.env_scale;
  param_ptr->pa_current = txagc_data.icq_current;
  param_ptr->pa_bias = txagc_data.smps_bias;
  #ifdef FEATURE_RF_ASDIV
  if (rfcommon_asdiv_is_switch_in_progress() == FALSE)
  {
    param_ptr->antenna_position = 
       rfcommon_asdiv_get_current_position(RFM_DEVICE_0);
  }
  else
  {
    param_ptr->antenna_position = 
       rfcommon_asdiv_get_previous_position(RFM_DEVICE_0);
  }
  #else
  param_ptr->antenna_position = 0;
  #endif /* FEATURE_RF_ASDIV */
  param_ptr->xpt_mode = txagc_data.xpt_mode;
  param_ptr->pin_comp = txagc_data.pin_comp;
  param_ptr->pout_comp = txagc_data.pout_comp;
  param_ptr->pin_rgi = 0xFF;
  param_ptr->delay = txagc_data.xpt_delay;

  /* Logging debug mode */
  if ( debug_mode == TRUE )
  {
    RF_MSG_9( RF_HIGH, "rfwcdma_core_log_get_agc_param: "
              "[ AGC Status %d, LNA State %d, PA On %d ] "
              "[ RxAGC ch0cr0 %d, RxAGC ch0cr1 %d, RxAGC ch1cr0 %d, "
              "RxAGC ch1cr1 %d ] "
              "[ TxAGC ch0cr0 %d, HDET %d ]",
              param_ptr->agc_info_chunk_0.agc_status_ch0cr0,
              param_ptr->agc_info_chunk_1.lna_state_ch0lna1,
              param_ptr->agc_info_chunk_2.pa_on,
              param_ptr->rx_agc_chain0_carr0,
              param_ptr->rx_agc_chain0_carr1,
              param_ptr->rx_agc_chain1_carr0,
              param_ptr->rx_agc_chain1_carr1,
              param_ptr->tx_agc_chain0_carr0,
              param_ptr->hdet );
  } /* if ( debug_mode == TRUE ) */

  return;
} /* rfwcdma_core_log_get_agc_param */

/*! @} */

/*============================================================================*/
/*!
  @addtogroup RFWCDMA_LOG_AGC_MC_INTERFACE
 
  @brief
  This module contains the interfaces exported to perform AGC logging
  @{
*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Buffer AGC Log

  @details
  This function will log WCDMA RxAGC and TxAGC data into a local buffer. This
  will be flushed back to diag when the buffer is full. This function
  will be periodically triggered by WL1.
*/
void 
rfwcdma_core_log_agc_data
( 
  void 
)
{
  rfwcdma_log_agc_type* log_data = NULL; /* Pointer to AGC Log */

  /* Get Pointer to log data */
  log_data = get_agc_log_data();

  /* NULL pointer check */
  if ( log_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfwcdma_core_log_agc_data: NULL log Data" );
    return;
  } /* if ( log_data == NULL ) */

  /* Check if the log is enabled */
  if( is_agc_log_enabled(log_data) )
  {
    /* Log until the maximum instance of AGC before flushing it to diag */
    if ( log_data->counter < RFWCDMA_LOG_AGC_SAMPLES )
    {
      /* For the first AGC Log instance, populate the header with log
      timestamp and the frame number */
      if ( log_data->counter == 0 )
      {
        /* Set Time Stamp */
        log_set_timestamp(&log_data->log_packet);

        /* Update Subpacket header */
        log_data->log_packet.agc_sub_pkt.header.pkt_id = 
                                        RFWCDMA_LOG_AGC_CUMULATIVE;
        log_data->log_packet.agc_sub_pkt.header.pkt_ver = 
                                        RFWCDMA_LOG_AGC_SUBPACKET_VER_2;
        log_data->log_packet.agc_sub_pkt.start_cfn = seq_get_cfn();
      } /* if ( rfm_wcdma_agc_log_count == 0 ) */

      /* Fetch the AGC params */
      rfwcdma_core_log_get_agc_param( 
        &(log_data->log_packet.agc_sub_pkt.agc_params[log_data->counter]),
        log_data->debug_mode );
  
      /* Increment AGC log counter */
      log_data->counter++;
    } /* if ( rfm_wcdma_agc_log_count < RFM_WCDMA_AGC_LOG_SAMPLES ) */

    /* For case, when the AGC log instance reaches maximum, flush the log
    to diag */
    if ( log_data->counter == RFWCDMA_LOG_AGC_SAMPLES )
    {
      flush_agc_log( log_data );
    } /* if ( rfm_wcdma_agc_log_count == RFM_WCDMA_AGC_LOG_SAMPLES ) */
  } /* if( is_agc_log_enabled(log_data) ) */

  return;

} /* rfwcdma_core_log_agc_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Immediate AGC Log

  @details
  This function will perform immediate AGC logging to flush it to diag. No 
  buffering will be done in this case. 
*/ 
void 
rfwcdma_core_log_agc_data_once
( 
  void 
)
{
  rfwcdma_log_agc_type *log_data = NULL; /* Pointer to AGC Log */
  rfwcdma_log_agc_packet_type *agc_log = NULL; /* one instance of AGC log */

  /* Get Pointer to log data */
  log_data = get_agc_log_data();

  /* NULL pointer check */
  if ( log_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfwcdma_core_log_agc_data_once: NULL log Data" );
    return;
  } /* if ( log_data == NULL ) */

  /* Check if the log is enabled */
  if( is_agc_log_enabled(log_data) )
  {
    /* Make sure that the the pending log is flush before doing one-time
    logging */
    if ( log_data->counter != 0 )
    {
      flush_agc_log( log_data );
    } /* if ( log_data->counter != 0 ) */

    /* Allocate Log */
    agc_log = 
      (rfwcdma_log_agc_packet_type*)log_alloc( log_data->agc_log_code,
              sizeof(rfcommon_log_rf_header_type) + 
              RFWCDMA_LOG_AGC_V3_LENGTH(RFWCDMA_LOG_AGC_ONE_INSTANCE_SAMPLES));

    /* NULL Pointer check */
    if ( agc_log == NULL )
    {
      RF_MSG( RF_ERROR, "rfwcdma_core_log_agc_data_once: Log alloc failed" );
      return;
    } /* if ( agc_log == NULL ) */

    agc_log->rf_header.pkt_version = RFWCDMA_LOG_AGC_VER_2;
    agc_log->rf_header.num_sub_pkt = 1;
    agc_log->agc_sub_pkt.header.pkt_id = RFWCDMA_LOG_AGC_CUMULATIVE;
    agc_log->agc_sub_pkt.header.pkt_ver = RFWCDMA_LOG_AGC_SUBPACKET_VER_2;
    agc_log->agc_sub_pkt.header.pkt_size = 
               RFWCDMA_LOG_AGC_V3_LENGTH(RFWCDMA_LOG_AGC_ONE_INSTANCE_SAMPLES);

    agc_log->agc_sub_pkt.num_samples = RFWCDMA_LOG_AGC_ONE_INSTANCE_SAMPLES;
    agc_log->agc_sub_pkt.start_cfn = seq_get_cfn();
    rfwcdma_core_log_get_agc_param( agc_log->agc_sub_pkt.agc_params, 
                                    log_data->debug_mode );

    log_commit(agc_log);
  } /* if( is_agc_log_enabled(log_data) ) */

  return;

} /* rfwcdma_core_log_agc_data_once */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Flush AGC Log

  @details
  This function will flush the AGC log data buffered till now
*/ 
void 
rfwcdma_core_log_agc_data_flush
( 
  void 
)
{
  rfwcdma_log_agc_type *log_data = NULL; /* Pointer to AGC Log */

  /* Get Pointer to log data */
  log_data = get_agc_log_data();

  /* NULL pointer check */
  if ( log_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfwcdma_core_log_agc_data_flush: NULL log Data" );
    return;
  } /* if ( log_data == NULL ) */

  /* Check if the log is enabled */
  if( is_agc_log_enabled(log_data) )
  {
    /* Make sure that the the pending log is flush before doing one-time
    logging */
    if ( log_data->counter != 0 )
    {
      flush_agc_log( log_data );
    } /* if ( log_data->counter != 0 ) */

  } /* if( is_agc_log_enabled(log_data) ) */

  return;

} /* rfwcdma_core_log_agc_data_flush */

/*! @} */

/*============================================================================*/
/*!
  @addtogroup RFWCDMA_LOG_COMMON
 
  @brief
  This module exports common WCDMA log interface
  @{
*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize WCDMA logging

  @details
  This function will called during WCDMA initialization to perform actions
  necessary to prepare WCDMA for logging.
 
  @return
  Flag indicating the status of log initialization
*/
boolean 
rfwcdma_log_init
(
  void
)
{
  boolean ret_val = TRUE;

  /* Initialize AGC log */
  ret_val &= init_agc_log();

  return ret_val;

} /* rfwcdma_log_init */

/*! @} */


