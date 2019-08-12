
/*!
  @file
  rflm_tds_log.h

  @brief
  RF LOG API definitions for TDS 
  
  @detail
  This file will contain all definitios and declarations to be
  shared with SW and LM for TDS LM Logging
 
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/rftech/tdscdma/inc/rflm_tds_log.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/28/15   jak     Add declaration for RFLM TDS logging function 
01/25/15   jak     Enable RFLM_LOG_PACKET_TDS_DEBUG_ID logging
03/20/14   jps     Initial Version


==============================================================================*/

#ifndef RFLM_TDS_LOG_H
#define RFLM_TDS_LOG_H


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "rflm.h"
#include "rflm_diag_log.h"
#include "log.h"
#include "rflm_tds_mc.h"

/*==============================================================================

                EXTERNAL DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/************************************************************/
/*                LOG PACKET IDENTIFICATION                 */
/************************************************************/

/*----------------------------------------------------------------------------*/
/*! @brief RFLM TDS log packet ID */
#define RFLM_LOG_PACKET_TDS_UL_REPORT_ID                 0x1868
#define RFLM_LOG_PACKET_TDS_DL_REPORT_ID                 0x1869
#define RFLM_LOG_PACKET_TDS_DEBUG_ID                     0x186A 
/*----------------------------------------------------------------------------*/
/*! @brief RFLM TDS DEBUG user ID */
#define RFLM_LOG_PACKET_RAT_TDS_DEBUG                 0

/*----------------------------------------------------------------------------*/
/*! @brief RFLM TDS DEBUG tag size */
#define RFLM_LOG_PACKET_TDS_DEBUG_TAG_SIZE_WORDS      0
/************************************************************/
/*                   LOG PACKET VERSIONS                    */
/************************************************************/

/*----------------------------------------------------------------------------*/
/*!@brief Enumeration indicating the TDS DebugL Log Packet version */
typedef enum
{
  RFLM_TDS_DEBUG_LOG_VER_1 = 1, /*!< TDS Debug Log Packet Version 1 */

} rflm_tds_debug_log_version_type;

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
/*!  @brief TDS DEBUG log packet structure version 1*/

typedef struct
{
  rflm_tds_log_debug_slot_update_t rflm_tds_log_debug_slot_update;
}rflm_tds_debug_log_packet_v1;

/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*!  @brief TDS DEBUG log packet structure */

/// @log_id 0x186A
typedef struct
{
  rflm_tds_debug_log_version_type  log_version;	/*!< Log Packet Version */
  /// @descriminator log_version
  union  rflm_tds_debug_log_version_type
  {
    rflm_tds_debug_log_packet_v1 v1;   /*!< Log Packet Version 1 */
    //rflm_wdma_txagc_log_packet_v2 v2;   /*!< Log Packet Version 2 */
  } packet;

}rflm_tds_debug_log_packet_t;


/*----------------------------------------------------------------------------*/



/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
              API input/output function definitions -- TDS 
==============================================================================*/

/************************************************************/
/*           DIAG LOG SUBPACKET API DEFINITIONS             */
/************************************************************/

/*----------------------------------------------------------------------------*/
boolean rflm_tds_debug_log_subpacket (void);

int rflm_tds_debug_diag_log_packet_allocate(void);

void rflm_tds_debug_diag_log_packet_deallocate();

void rflm_tds_debug_diag_log_subpacket(  rflm_tds_log_debug_slot_update_t* rflmTdsLogSlotUpdate_ptr);



#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RFLM_TDS_LOG_H */
