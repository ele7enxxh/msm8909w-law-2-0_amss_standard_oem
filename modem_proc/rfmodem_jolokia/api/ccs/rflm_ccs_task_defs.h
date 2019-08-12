#ifndef RFLM_CCS_TASK_DEFS_H
#define RFLM_CCS_TASK_DEFS_H

/*!
  @file 
  rflm_ccs_task_defs.h

  @brief
  RF CCS task definitions
 
*/

/*============================================================================== 
   
  Copyright (c) 2014 -  Qualcomm Technologies Incorporated. All Rights Reserved 
   
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
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/api/ccs/rflm_ccs_task_defs.h#1 $ 
$DateTime: 2016/12/13 07:59:25 $ $Author: mplcsds1 $ 
   
when       who     what, where, why 
--------   ---    -------------------------------------------------------------- 
06/20/14   jc     Increased TQ pair and SW port size 
03/14/14   oei    Remove contents & replace by including FW's "ccs_rf_intf.h"
03/05/14   oei    Initial version rfmodem_dimepm, 16 tasks, use D3925 constants
12/16/13   Saul   CCS. Increase number of tasks per TQ to 32.
11/25/13   Saul   CCS. New CSS Data Buffer Allocation For All Techs.
10/18/13   Saul   CCS. RFFE EXT writes using DMA.
10/08/13   cd     Define task types for D3925
09/03/13   ra     Rebaseline: compatible with older modem and new CCS processor
07/31/13   hdz    Added more backward compatability support
07/23/13   ra     Initial version. Created to support backward compatability
==============================================================================*/ 
#include "comdef.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! 
   @brief In RFMODEM_DIMEPM, FW owns the CCS. As such, we need to rely on
   their definitions and structures for our RF CCS driver
 */
#include "ccs_rf_intf.h"


/*! 
  @brief Number of 32-bit words in the CCS data buffer for any TQ pair
  Each TQ pair is allocated 3 K bytes in the CCS data buffer.
  This define is created for general use whenever it is necessary to get
  the number of words used by any TQ pair.
  Total size in bytes / size of one word (4 bytes or uint32) = num words
*/
// Temporarily apply all increased memory from FW towards TQ pair
// Expecting 11K from FW, which would result in 4K per TQ pair
#if (CCS_RF_TASKQ_DATA_BUF_WORDS>((9*1024)/4))
#define CCS_RF_TASKQ_PAIRS_DATA_BUF_NUM_WORDS     ( (CCS_RF_TASKQ_DATA_BUF_WORDS-(3*1024)/4)/2 )
#else
#define CCS_RF_TASKQ_PAIRS_DATA_BUF_NUM_WORDS     ( (3*1024) / 4 )
#endif

/*! @brief TQ pair 0 address offset in the CCS data buffer
*/
#define CCS_RF_TASKQ_DATA_BUF_PAIR0_ADDR_OFFSET_WORDS     0

/*! @brief TQ pair 0 number of 32-bit words in the CCS data buffer
*/
#define CCS_RF_TASKQ_DATA_BUF_PAIR0_NUM_WORDS     CCS_RF_TASKQ_PAIRS_DATA_BUF_NUM_WORDS

/*! @brief TQ pair 1 address offset in the CCS data buffer
*/
#define CCS_RF_TASKQ_DATA_BUF_PAIR1_ADDR_OFFSET_WORDS    ( CCS_RF_TASKQ_DATA_BUF_PAIR0_ADDR_OFFSET_WORDS + CCS_RF_TASKQ_DATA_BUF_PAIR0_NUM_WORDS )

/*! @brief TQ pair 1 number of 32-bit words in the CCS data buffer
*/
#define CCS_RF_TASKQ_DATA_BUF_PAIR1_NUM_WORDS     CCS_RF_TASKQ_PAIRS_DATA_BUF_NUM_WORDS

/*! @brief TQ IRAT address offset in the CCS data buffer
*/
#define CCS_RF_TASKQ_DATA_BUF_IRAT_ADDR_OFFSET_WORDS    ( CCS_RF_TASKQ_DATA_BUF_PAIR1_ADDR_OFFSET_WORDS + CCS_RF_TASKQ_DATA_BUF_PAIR1_NUM_WORDS )

/*! @brief TQ IRAT number of 32-bit words in the CCS data buffer 
    IRAT is allocated 2 K bytes in the CCS data buffer.
    Total size in bytes / size of one word (4 bytes or uint32) = num words
*/
#define CCS_RF_TASKQ_DATA_BUF_IRAT_NUM_WORDS     ( (2*1024) / 4 )

/*! @brief TQ DEDICATED address offset in the CCS data buffer
*/
#define CCS_RF_TASKQ_DATA_BUF_DEDICATED_ADDR_OFFSET_WORDS    ( CCS_RF_TASKQ_DATA_BUF_IRAT_ADDR_OFFSET_WORDS + CCS_RF_TASKQ_DATA_BUF_IRAT_NUM_WORDS )

/*! @brief TQ DEDICATED number of 32-bit words in the CCS data buffer 
    Dedicated SW port is allocated 1 K bytes in the CCS data buffer.
    Total size in bytes / size of one word (4 bytes or uint32) = num words
*/
#define CCS_RF_TASKQ_DATA_BUF_DEDICATED_NUM_WORDS     ( 1024 / 4 )

/*! @brief Size of taskq data buffer, shared for all TQ.
    + 3k Bytes per pair of TQs
    + 2k for IRAT TQ
    + 1k for RFSW TQ
    = 9k bytes in RFC/CCS for data buffer
*/
#define CCS_RF_TASKQ_TOTAL_DATA_BUF_WORDS   (   CCS_RF_TASKQ_DATA_BUF_PAIR0_NUM_WORDS       \
                                        + CCS_RF_TASKQ_DATA_BUF_PAIR1_NUM_WORDS       \
                                        + CCS_RF_TASKQ_DATA_BUF_IRAT_NUM_WORDS        \
                                        + CCS_RF_TASKQ_DATA_BUF_DEDICATED_NUM_WORDS )


#endif /* RFLM_CCS_TASK_DEFS_H */
