/*!
  @file
  lte_l2_common.h

  @brief
  This file contains common defines  and functions interfaces for LTE L2 layer.


*/

/*===========================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/common/inc/lte_l2_common.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/12/15   el      CR 884865: FR 29101 Fast RLF Declaration for fast resync with eNB
04/07/15   su      CR808724: Limit the number of built mac SDU under 200
02/11/15   sb      CR761765: Double RLC memory for header building 
                   for each HARQ
01/22/15   el      CR785571: Add a common API for calculating 
                   sfn timing difference
01/14/15   ru      CR775616: Fix for PDCPDL Ciph Buf Corruption
11/23/14   sb      CR745901: Turn AM to UM mode and do best effort deliver
                   when new Flow contrl level is triggered.
12/23/14   sb      CR772190: Extend A2 diag framework for L2
08/22/12   ax      CR383618: L2 modem heap use reduction phase I  
12/16/11   ax      feature clean up LTE_L2_SIGNAL
05/23/11   ar      Increased LTE_RLCUL_TX_HDR_BUFF_SIZE to 4096 bytes
05/20/11   bn      Added in change for Nikel reduction HARQ
05/18/10   ax      Added api for dynamic memory allocation from modem mem
02/07/10   sm      Added data indicaiton optimization in L2 path
11/06/09   bn      Added in utility to manage A2 static allocation
10/12/09   ax      added LTE_L2_NTOHL and LTE_L2_NTOHS macros
05/15/08   bn,yg   Initial version
===========================================================================*/

#ifndef LTE_L2_COMMON_H
#define LTE_L2_COMMON_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "lte_variation.h"
#include <comdef.h>
#include <msgr.h>
#include <lte_l2_comdef.h>
#include <lte_assert.h>
#include <modem_mem.h>
#include <lte_l1_types.h>
/*===========================================================================

                               MACROS

===========================================================================*/

/*! @brief convert network byte order (big endian) byte array of size 4 
     into host uint32
*/
#define LTE_L2_BIG_ENDIAN_BYTE_ARRAY_2_UINT32(array)                        \
(                                                                           \
  (uint32)array[3] | (array[2] << 8) |(array[1] << 16) | (array[0] << 24)   \
)

/*! @brief convert network byte order (big endian) byte array of size 2 
     into host uint16
*/
#define LTE_L2_BIG_ENDIAN_BYTE_ARRAY_2_UINT16(array)                        \
(                                                                           \
  (uint16)array[1] | (array[0] << 8)                                        \
)

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*! @brief Size (in bytes) of the memory assigned for RLC header building.
 * 1024 (rlc hdr mem for 1 harq) * 8 (number of harqs) = 8192 bytes
*/
#define LTE_RLCUL_TX_HDR_BUFF_SIZE                   8192

/*! @brief defined maximum length of MAC PDU headers for RACH: 32 bytes*/
#define LTE_MAC_RACH_PDU_HEADER_MAX_LENGTH           32

/*! @brief defined maximum lenght of RACH MSG3: 512 bytes */
#define LTE_MAC_COMMON_RACH_MSG3_MAX_LENGTH          512

/*! @brief defined maximum length of MAC PDU headers and MCE in bytes*/
#define LTE_MAC_UL_DATA_PDU_HEADER_MCE_MAX_LENGTH    512

/*! @brief defined number of SW HARQ. HW reduces the HARQ buffer,
SW needs to backup for that. Total of 8 HARQs*/
#define LTE_MAC_UL_DATA_NUM_SW_HARQ   8

/*! @brief defined A2 memory size for safe guard. The size is words or 32 bytes */
#define LTE_L2_COMMON_A2_STATIC_MEM_SAFE_GUARD_SIZE  (32)

/*! @brief defined A2  cache alignment 32 bytes */
#define LTE_L2_COMMON_A2_CACHE_ALIGNMENT             (32)

/*! @brief defined maximum static memory size in 32-bit word for A2.
Note there are 4 chums of safe guard and 1 overhead safe guard */
#define LTE_L2_COMMON_A2_STATIC_MEM_SIZE  (LTE_RLCUL_TX_HDR_BUFF_SIZE + \
  LTE_MAC_COMMON_RACH_MSG3_MAX_LENGTH + LTE_MAC_RACH_PDU_HEADER_MAX_LENGTH \
  + (LTE_MAC_UL_DATA_NUM_SW_HARQ * LTE_MAC_UL_DATA_PDU_HEADER_MCE_MAX_LENGTH) \
  + (5 * LTE_L2_COMMON_A2_STATIC_MEM_SAFE_GUARD_SIZE)) / 4

/*! @brief defined cookie for safe guard */
#define LTE_L2_COMMON_A2_STATIC_MEM_COOKIE   0xBA0BA0BA

/*! @brief defined MAX number of system frame number multiply subframe number */
#define LTE_L2_SFN_SUBFN_MODULUS             10240

/*! @brief L2 common A2 static memory allocation management structure
*/
typedef struct
{
  /*!index to the next avaible memory location*/
  uint32  index;
  /*! static buffer */
  uint32  buff[LTE_L2_COMMON_A2_STATIC_MEM_SIZE];
} lte_l2_common_a2_static_mem_alloc_s;

/*! @brief L2 common signal structure
*/
typedef struct
{
  //ISignalQ*    signal_q;             /*!< CS signal queue of signal */
  //ISignal*     signal;               /*!< CS signal  */
  //ISignalCtl*  signal_ctrl;          /*!< CS signal control of signal */
  msgr_client_t   msgr_client;       /*!< MSGR client used when registering with
                                          message router*/
} lte_l2_signal_s;

/*! @brief L2 common L2 indication optimization related structure.
    This is to track that L2 layers are not indicating other layer more than 
    once if the signal/indication is not yet processed.
*/
typedef struct
{
  volatile boolean pending; /*!< indication is enqueued but not yet processed*/
  uint32  probable_enqueue_count; /*!< probable number of times indication
                                       command could have been enqueued */
  uint32  enqueue_count; /*<! absolute number of times indication command
                              enqueued for the receiver */
  uint32  dequeue_count; /*!< absolute number of times indication command
                              is received and processed by the receiver task */
} lte_l2_indication_info_s;

/*
* @brief enum for L2_DIAG_CMD_CODE
*/
typedef enum
{
  L2_DIAG_CMD_REQ_ITEM_CODE_MIN = 0xFF00,
  L2_DIAG_CMD_REQ_ITEM_CODE_MAC_CTRL = 0xFF01,
  L2_DIAG_CMD_REQ_ITEM_CODE_MACUL = 0xFF02,
  L2_DIAG_CMD_REQ_ITEM_CODE_MACDL = 0xFF03,
  L2_DIAG_CMD_REQ_ITEM_CODE_PDCPDL = 0xFF04,
  L2_DIAG_CMD_REQ_ITEM_CODE_PDCPUL = 0xFF05,
  L2_DIAG_CMD_REQ_ITEM_CODE_PDCP_OFFLOAD = 0xFF06,
  L2_DIAG_CMD_REQ_ITEM_CODE_RLCDL = 0xFF07,
  L2_DIAG_CMD_REQ_ITEM_CODE_RLCUL = 0xFF08,
  L2_DIAG_CMD_REQ_ITEM_CODE_MAX = 0xFF0F,
}l2_diag_cmd_reqitem_code_e;

#ifndef FEATURE_LTE_L2_DYNAMIC_A2_MEM_ALLOCATION
/*===========================================================================

  FUNCTION:  lte_l2_common_a2_static_mem_allocation

===========================================================================*/
/*!
    @brief
    This function is used by L2 tasks to allocate static memory for A2
    drivers during powerup.

    @detail

    @return
    None

    @note
    This function should be only called by L2 tasks on powerup. It is assumed
    that re-activated LTE stack won't caused L2 task to do A2 static memory
    allocation again.

    @see related_function()
    
*/
/*=========================================================================*/
extern void *lte_l2_common_a2_static_mem_allocation
( 
  uint16 size  /*!< memory size in bytes */
);
#endif 

/*! @brief This macro performs the dynamic/static memory allocation for A2
*/
#ifdef FEATURE_LTE_L2_DYNAMIC_A2_MEM_ALLOCATION
#define LTE_L2_MALLOC_A2_MEMORY(size)           malloc(size)
#else
#define LTE_L2_MALLOC_A2_MEMORY(size)  \
  lte_l2_common_a2_static_mem_allocation(size);
#endif


/*! @brief This macro performs the dynamic memory free for A2
*/
#ifdef FEATURE_LTE_L2_DYNAMIC_A2_MEM_ALLOCATION
#define LTE_L2_FREE_A2_MEMORY(addr)             free(addr)
#else
#define LTE_L2_FREE_A2_MEMORY(addr)
#endif

/*==============================================================================

  FUNCTION:  lte_l2_common_mem_alloc

==============================================================================*/
/*!
    @brief
    This function is called by the L2 modules in order to allocate 
    dynamic memory using modem heap. This function simply calls modem_mem_alloc 
    and returns pointer to the allocated memory. 

    @detail

    @return
    pointer to the memory allocated if successful. NULL if allocation failure
*/
/*============================================================================*/
#define lte_l2_common_mem_alloc(size, client)                           \
        modem_mem_alloc(size, client)


/*==============================================================================

  FUNCTION:  lte_l2_common_mem_free

==============================================================================*/
/*!
    @brief
    This function is called by the L2 modules in order to free 
    dynamic memory from modem heap. This function simply calls modem_mem_free. 

    @return
    None
*/
/*============================================================================*/
#define lte_l2_common_mem_free(ptr, client)                            \
        modem_mem_free(ptr, client)

#define DS_LTE_QCI_INVALID     0xFF

/*===========================================================================

  FUNCTION:  lte_l2_common_next_ceil

===========================================================================*/
/*!
    @brief
    This function returns a multiple of size 2^exp which is big enough to
    hold base. For example, if base=258 and size=2^8(exp=8), then this function
    returns 512.

    @return
    None
*/
/*============================================================================*/
extern uint32 lte_l2_common_next_ceil
(
  uint32 base, 
  uint8 exp
);

/*==============================================================================

  FUNCTION:  lte_l2_common_get_sfn_diff

==============================================================================*/
/*!
    @brief
    This function is called by L2 modules to get the timing difference

    @return
    time difference in ms
    None
*/
/*============================================================================*/
extern uint16 lte_l2_common_get_sfn_diff
(
  lte_l1_sfn_t              start_sys,     /*!< System time of start time */
  lte_l1_subfn_t            start_sub,     /*!< sub time of start time */
  lte_l1_sfn_t              end_sys,       /*!< System time of end time */
  lte_l1_subfn_t            end_sub        /*!< sub time of end time */
);

#endif /* LTE_L2_COMMON_H */

