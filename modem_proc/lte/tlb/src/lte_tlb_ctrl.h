/*!
  @file 
  lte_tlb_ctrl.h

  @brief
  This file contains all the common define structures/function prototypes 
  for LTE TLB Control Task.


*/

/*===========================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/tlb/src/lte_tlb_ctrl.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/23/14   el      CR 698155: Add sub id support in TLB
04/08/13   md      Reduced stack size
09/13/12   st      CR 364517: Added TLB Mode C enhancements
08/26/09   ax      size the mq correctly.
05/06/09   bn      Initial version
===========================================================================*/

#ifndef LTE_TLB_CTRL_H
#define LTE_TLB_CTRL_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <amssassert.h>
#include <IxErrno.h>
#include <lte.h>
#include <lte_as.h>
#include <lte_assert.h>
#include "lte_tlb.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*! @brief Define Invalid MSG ID
*/
#define LTE_TLB_CTRL_INVALID_MSG_ID                   0x00

/*! @brief Define CLOSE UE TEST LOOP MSG ID
*/
#define LTE_TLB_CTRL_CLOSE_TL_MSG_ID                  0x80

/*! @brief Define CLOSE UE TEST LOOP COMPLETE MSG ID
*/
#define LTE_TLB_CTRL_CLOSE_TL_COMPLETE_MSG_ID         0x81

/*! @brief Define OPEN UE TEST LOOP MSG ID
*/
#define LTE_TLB_CTRL_OPEN_TL_MSG_ID                   0x82

/*! @brief Define OPEN UE TEST LOOP COMPLETE MSG ID
*/
#define LTE_TLB_CTRL_OPEN_TL_COMPLETE_MSG_ID          0x83

/*! @brief Define ACTIVATE RB TEST MODE MSG ID
*/
#define LTE_TLB_CTRL_ACT_RB_TM_MSG_ID                 0x84

/*! @brief Define ACTIVATE RB TEST MODE COMPLETE MSG ID
*/
#define LTE_TLB_CTRL_ACT_RB_TM_COMPLETE_MSG_ID        0x85

/*! @brief Define DEACTIVATE RB TEST MODE MSG ID
*/
#define LTE_TLB_CTRL_DEACT_RB_TM_MSG_ID               0x86

/*! @brief Define DEACTIVATE RB TEST MODE COMPLETE MSG ID
*/
#define LTE_TLB_CTRL_DEACT_RB_TM_COMPLETE_MSG_ID      0x87

/*! @brief Define UPDATE LOCATION INFO MSG ID
*/
#define LTE_TLB_CTRL_RESET_LOCATION_INFO_MSG_ID     0x88

/*! @brief Define MBMS PACKET COUNTER REQUEST MSG ID
*/
#define LTE_TLB_CTRL_MBMS_PKT_CNT_REQ_MSG_ID          0x89

/*! @brief Define MBMS PACKET COUNTER RESPONSE MSG ID
*/
#define LTE_TLB_CTRL_MBMS_PKT_CNT_RESPONSE_MSG_ID     0x8A

/*! @brief Define UPDATE LOCATION INFO MSG ID
*/
#define LTE_TLB_CTRL_UPDATE_LOCATION_INFO_MSG_ID     0x8B

/*! @brief Define the scaling record size
*/
#define LTE_TLB_CTRL_SCALE_RECORD_SIZE                0x3

/*! @brief Define a byte mask */
#define LTE_TLB_CTRL_BYTE_MASK                        0xFF

/*! @brief Define a invalid inst id */
#define LTE_TLB_CTRL_INVALID_ID                       0xFF
/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Union of all TLB Control external and internal messages */
typedef struct
{
  union
  {
    /*! ************* system general external messages *************/
    /*! Message header */
    msgr_hdr_struct_type                hdr;            

    /*! ********* external Messages ************/
    /* TLB message from NAS */
    lte_tlb_dl_info_ind_s               dl_info;
    /*! ************ internal Messages *****************/

  } msg;
} lte_tlb_ctrl_msg_u;

/*! @brief Define maximum mail pool size for TLB CTRL Task
*/
#define LTE_TLB_CTRL_MAILPOOL_SIZE 10

/*! @brief Define Mailbox priority for TLB CTRL Task
*/
#define LTE_TLB_CTRL_MB_PRIO 1

/*! @brief Define stack size for TLB CTRL Task
*/
#define LTE_TLB_CTRL_TASK_STACK_SIZE 8192

/*! @brief Define invalid LB ID
*/
#define LTE_TLB_CTRL_INVALID_LB_ID         0xFF

/*! @brief Global structure for TLB CTRL task
*/
typedef struct
{
  /*!< Whether the "main" function of the thread is running */
  boolean task_is_active; 
  msgr_client_t  msgr_client;  /*!< MSGR client */
  msgr_id_t      mb_id;        /*!< Mailbox Id */
} lte_tlb_ctrl_thread_s;

/*! @brief structure for close test loop configuration
*/
typedef struct
{
  uint16   ul_pdcppkt_size;
  uint8    lb_id;
} lte_tlb_ctrl_close_tl_cfg_s;

/*! @brief structure for close test loop mode C configuration
*/
typedef struct
{
  lte_mbsfn_area_id_t area_id; /*!<message container */
  lte_pmch_id_t       pmch_id;
  uint8               lcid;
  uint32              mbms_pkt_cnt;
} lte_tlb_ctrl_close_tl_mode_c_cfg_s;


/*! @brief Global Data structure for TLB CTRL task
*/
typedef struct
{
  lte_tlb_mode_e  mode; /*!< test loopback mode */
  /*! RB/LC config for test loopback mode A*/
  lte_tlb_ctrl_close_tl_cfg_s  rb_cfg[LTE_MAX_RB];
  /*! IP PDU delay in second config for test loopback mode B */
  uint8  ip_ipu_delay;
  /*! Mode C related config/pkt count data */
  lte_tlb_ctrl_close_tl_mode_c_cfg_s mode_c_data;
  boolean  close_tl_active;  /*!< Close test loop is active */
  uint8 msg_inst_id; /*!< message instance id from NAS */
  lte_tlb_ctrl_update_loc_info_s loc_info; /*!< Location Info Update from SS */
} lte_tlb_ctrl_data_s;

/*! @brief Global structure for TLB CTRL task
*/
typedef struct
{
  lte_tlb_ctrl_thread_s     thread;/*!< all the thread control related data */
  lte_tlb_ctrl_data_s       data;  /*!< all the local data structure */
} lte_tlb_ctrl_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_tlb_ctrl_task_is_active

===========================================================================*/
/*!
  @brief
  Can be used to query if the tlb_ctrl task has been initialized

  @return
  TRUE if the task has been initialized.
*/
/*=========================================================================*/
EXTERN boolean lte_tlb_ctrl_task_is_active
(
  void
);

/*===========================================================================

  FUNCTION:  lte_tlb_mode_a_data_init

===========================================================================*/
/*!
    @brief
    This function is initialized all the TLB mode A data structures.

    @detail

    @return
    None.

    @see related_function()

*/
/*=========================================================================*/
EXTERN void lte_tlb_mode_a_data_init
(
  void
);

/*===========================================================================

  FUNCTION:  lte_tlb_mode_c_data_init

===========================================================================*/
/*!
    @brief
    This function initializes all the TLB mode C data structures.

    @detail

    @return
    None.

    @see related_function()

*/
/*=========================================================================*/
EXTERN void lte_tlb_mode_c_data_init
(
  void
);

/*===========================================================================

  FUNCTION:  lte_tlb_ctrl_get_tlb_data_ptr

===========================================================================*/
/*!
    @brief
    Function call to get the pointer to the data structure of the TLB control

    @detail

    @return
    void

    @note

    @see related_function()
*/
/*=========================================================================*/
EXTERN lte_tlb_ctrl_data_s *lte_tlb_ctrl_get_tlb_data_ptr
( 
  void
);

#endif /* LTE_TLB_CTRL_H */
