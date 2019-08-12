/*!
  @file
  rflm_lte_ccs_intf.h

  @brief
  CCS Memory definitions for LTE to interact with SW and FED
  
  @detail
  
  
  @todo
 

*/

/*==============================================================================

  Copyright (c) 2013-14 Qualcomm Technologies, Inc. (QTI). All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/api/lte/rflm_lte_ccs_intf.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/06/15   am       Adjust the tx rf event and autopin event memory as per the requirement
12/31/15   am       Create separate TQ segment to schedule autopin events
06/18/14   php      Remove unnecessary memory assigned for TxOFF  
06/18/14   php      Initial Version
==============================================================================*/

#ifndef RFLM_LTE_CCS_INTF_H
#define RFLM_LTE_CCS_INTF_H


/************************************************************/
/*                CCS MEMORY USAGE DIVISION                 */
/************************************************************/
/* Memory Size to be used by TxAGC per Issue Seq */
#define RFLM_LTE_CCS_TX_MEM_TXAGC_MEM_SIZE           845
/* Worse case number of unexecuted TxAGC Issue Seqs present in the memory */
/* If all slots are issued at once, we need the following number to be 4*/
/* If all slots are issued at once, but if slot1 updates are disabled, it becomes 2*/
/* If all slot0 and slot 1 are issued separately, it becomes 2*/
#define RFLM_LTE_CCS_TX_MEM_TXAGC_NUM                3

#define RFLM_LTE_CCS_TX_MEM_FOR_EACH_XSW             0
#define RFLM_LTE_CCS_TX_MEM_FOR_ALL_XSW              RFLM_LTE_CCS_TX_MEM_FOR_EACH_XSW * 2

/* Worst case Memory size of unexecuted TxAGC Issue Seqs */
#define RFLM_LTE_CCS_TX_MEM_TXAGC_TOTAL_MEM_SIZE     RFLM_LTE_CCS_TX_MEM_TXAGC_MEM_SIZE * \
                                                     RFLM_LTE_CCS_TX_MEM_TXAGC_NUM + \
                                                     RFLM_LTE_CCS_TX_MEM_FOR_ALL_XSW

/* Memory Size for Tx OFF -> PA Off + ASM + Ant SWAP */
#define RFLM_LTE_CCS_TX_MEM_TX_OFF_MEM_SIZE          0

/* Memory Size for TxPLL AFC */
#define RFLM_LTE_CCS_TX_MEM_TXPLL_MEM_SIZE           250

/* Total Memory Size for Tx */
#define RFLM_LTE_CCS_TX_MEM_OFFSET                   0
#define RFLM_LTE_CCS_TX_MEM_SIZE                     RFLM_LTE_CCS_TX_MEM_TXAGC_TOTAL_MEM_SIZE + \
                                                     RFLM_LTE_CCS_TX_MEM_TX_OFF_MEM_SIZE + \
                                                     RFLM_LTE_CCS_TX_MEM_TXPLL_MEM_SIZE

/* Total Memory Size for Rx */													
#define RFLM_LTE_CCS_RX_MEM_OFFSET                   RFLM_LTE_CCS_TX_MEM_OFFSET + \
                                                     RFLM_LTE_CCS_TX_MEM_SIZE

/*Total memory size for Rx. This allows 4 LNA updates (2 PRx, 2 DRx) without overlap*/
/*Each LNA update is 128 bytes. This can change in future*/
#define RFLM_LTE_CCS_RX_MEM_SIZE                     535
#define RFLM_LTE_CCS_RX_ON_PLL_TIME                  150 * 19.2 /*This should come from rf systems; 19.2 ticks per us*/
#define RFLM_LTE_CCS_MAX_NUM_LNA_UPDATES             4

/* Total Memory Size for RX and TX Events */
#define RFLM_LTE_CCS_RF_EVENT_MEM_OFFSET             RFLM_LTE_CCS_RX_MEM_OFFSET + \
                                                     RFLM_LTE_CCS_RX_MEM_SIZE

#define RFLM_LTE_CCS_TX_RF_EVENT_MEM_SIZE            200
#define RFLM_LTE_CCS_RX_RF_EVENT_MEM_SIZE            0


#define RFLM_LTE_CCS_RX_RF_EVENT_MEM_OFFSET          RFLM_LTE_CCS_RF_EVENT_MEM_OFFSET + \
                                                     RFLM_LTE_CCS_TX_RF_EVENT_MEM_SIZE

#define RFLM_LTE_CCS_AUTOPIN_RF_EVENT_MEM_SIZE       250
                                                     

#define RFLM_LTE_CCS_RF_EVENT_MEM_SIZE               RFLM_LTE_CCS_TX_RF_EVENT_MEM_SIZE + \
                                                     RFLM_LTE_CCS_RX_RF_EVENT_MEM_SIZE + \
                                                     RFLM_LTE_CCS_AUTOPIN_RF_EVENT_MEM_SIZE    
/* Total Memory Size for RX and TX Scripts */
#define RFLM_LTE_CCS_RF_SCRIPT_MEM_SIZE              0
#define RFLM_LTE_CCS_RF_SCRIPT_MEM_OFFSET            RFLM_LTE_CCS_RF_EVENT_MEM_OFFSET + \
                                                     RFLM_LTE_CCS_RF_EVENT_MEM_SIZE

/* Total CCS Memory Used by LTE */
#define RFLM_LTE_CSS_RF_TOTAL_MEM_USED               RFLM_LTE_CCS_RF_EVENT_MEM_OFFSET + \
                                                     RFLM_LTE_CCS_RF_EVENT_MEM_SIZE

/* Maximum CCS RF event size of tuner AOL and CL */
#define RFLM_LTE_CCS_AOL_CL_RF_EVENT_SIZE            320

#endif /* RFLM_LTE_CCS_INTF_H */
