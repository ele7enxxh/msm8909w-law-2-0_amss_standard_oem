/*!
  @file
  lte_mac_common.h

  @brief
  This file contains all the common define structures/function prototypes 
  for LTE MAC.


*/

/*===========================================================================

  Copyright (c) 2008 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/mac/src/lte_mac_common.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/13/14   mg      CR738157: QSH - Memory held in RLC UL buffers
08/03/14   mg      CR702710: QSH analysis and reset
10/07/13   ta      CR 529514: DSDS feature. Support for RF Available/Unavailable
                   message
01/12/11   bn      Removed Extraneous comma at the end of enum define
11/06/09   bn      Moved MSG3 PDU memory size to lte_l2_common.h
09/30/09   bn      Modified function return RACH state to return MAC state. 
                   Added abort state in MAC state
09/23/09   bn      Added in function to return MAC RACH status
09/19/09   bn      Added in the option for enable/disable overwrite
07/30/09   bn      Supported store local copy of MSG3 for UL/DL RACH
05/15/08   bn      Initial version
===========================================================================*/

#ifndef LTE_MAC_COMMON_H
#define LTE_MAC_COMMON_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "lte_variation.h"
#ifndef T_WINNT
#include <atomic_ops.h>
#endif
#include <comdef.h>
#include <msgr.h>
#include <msgr_lte.h>
#include "lte_l2_common.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*! @brief defined bitmask for set or clear buffer update indication bit. 
Bit0 maps to buffer1 and bit1 maps to buffer2  */
#define  LTE_MAC_COMMON_SET_BUF1_BM     0x1
#define  LTE_MAC_COMMON_SET_BUF2_BM     0x2
#define  LTE_MAC_COMMON_CLR_BUF1_BM     0x2
#define  LTE_MAC_COMMON_CLR_BUF2_BM     0x1
#define  LTE_MAC_COMMON_CLR_BUF_ALL_BM  0x0
#define  LTE_MAC_COMMON_SET_BUF_ALL_BM  0x3

/*! @brief defined MAC state emumerations */
typedef enum
{
  LTE_MAC_COMMON_IDLE_STATE,
  LTE_MAC_COMMON_ACCESS_STATE,
  LTE_MAC_COMMON_CONNECTED_STATE,
  LTE_MAC_COMMON_HO_STATE,
  LTE_MAC_COMMON_RADIO_FAILURE_STATE,
  LTE_MAC_COMMON_RACH_ABORT_STATE,
  LTE_MAC_COMMON_PENDING_STATE
} lte_mac_common_state_e;

/*! @brief Common Double Buffer Structure to share data between 2 tasks.
One task writes to it and another task reads from it*/
typedef struct
{
  #ifdef T_WINNT
  #error code not present
#else
  atomic_word_t   buf_masks;   /*!< buffer masks */
  #endif
  uint32          read_masks;  /*!< copy of buffer masks for read operation */
  uint32          write_masks; /*!< copy of buffer masks for write operation */
  boolean         overwrite;   /*!< when 2 buffers are full,overwrite is OK or not */ 
  void*           buf[2];      /*!< buffer pointers */
} lte_mac_common_dbuf_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

  FUNCTION:  lte_mac_common_dbuf_init

===========================================================================*/
/*!
    @brief
    This function inits the shared double buffer structure.

    @detail
    - input1 is the double buffering structure.This is global data which will 
    share between the reader and writer
    - input2 is the buffer 1 which is in the creator local space
	- input3 is the buffer 2 which is in the creator local space

    @return
    None

    @note
    This function must be call first before any other utilities functions
    can be used.

    @see related_function()
    lte_mac_common_get_write_ptr, lte_mac_common_get_read_ptr
    lte_mac_common_update_write_status, lte_mac_common_update_read_status
*/
/*=========================================================================*/
extern void lte_mac_common_dbuf_init
(
  lte_mac_common_dbuf_s *dbuf,    /*!< common double buffer pointer */
  void                  *buf1,    /*!< buffer1 pointer */
  void                  *buf2,    /*!< buffer2 pointer */
  boolean               overwrite /*!< when 2 buffers are full,overwrite is OK or not */ 
);

/*===========================================================================

  FUNCTION:  lte_mac_common_get_write_ptr

===========================================================================*/
/*!
    @brief
    This function returns the write pointer of the shared double buffer
    structure.

    @detail
    - input1 is the double buffering structure.This is global data which will 
    share between the reader and writer

    @return
    A void pointer to the buffer which can be written. 

    @note
    lte_mac_common_dbuf_init must be call first.

    @see related_function()
    lte_mac_common_dbuf_init, lte_mac_common_get_read_ptr
    lte_mac_common_update_write_status, lte_mac_common_update_read_status
*/
/*=========================================================================*/
extern void* lte_mac_common_get_write_ptr
(
  lte_mac_common_dbuf_s *dbuf /*!< common double buffer pointer */
);

/*===========================================================================

  FUNCTION:  lte_mac_common_update_write_status

===========================================================================*/
/*!
    @brief
    This function updates the status of the double buffer. It should
    be called after finish writing all the new data into the write buffer.

    @detail
    - input1 is the double buffering structure.This is global data which will 
    share between the reader and writer

    @return
    None.

    @note
    lte_mac_common_dbuf_init must be call first.

    @see related_function()
    lte_mac_common_dbuf_init, lte_mac_common_get_read_ptr
    lte_mac_common_get_write_ptr, lte_mac_common_update_read_status
*/
/*=========================================================================*/
extern void lte_mac_common_update_write_status
(
  lte_mac_common_dbuf_s  *dbuf /*!< common double buffer pointer */
);

/*===========================================================================

  FUNCTION:  lte_mac_common_get_read_ptr

===========================================================================*/
/*!
    @brief
    This function returns the read pointer of the shared double buffer
    structure.

    @detail
    - input1 is the double buffering structure.This is global data which will 
    share between the reader and writer

    @return
    A void pointer to a read buffer. In case all the buffers are empty, it
    will return NULL.

    @note
    lte_mac_common_dbuf_init must be call first.

    @see related_function()
    lte_mac_common_dbuf_init, lte_mac_common_update_write_status
    lte_mac_common_get_write_ptr, lte_mac_common_update_read_status
*/
/*=========================================================================*/
extern void* lte_mac_common_get_read_ptr
(
  lte_mac_common_dbuf_s *dbuf /*!< common double buffer pointer */
);

/*===========================================================================

  FUNCTION:  lte_mac_common_update_read_status

===========================================================================*/
/*!
    @brief
    This function updates the read status of the shared double buffer
    structure. This function should be called after finishing reading.

    @detail
    - input1 is the double buffering structure.This is global data which will 
    share between the reader and writer

    @return
    None.

    @note
    lte_mac_common_dbuf_init must be call first.

    @see related_function()
    lte_mac_common_dbuf_init, lte_mac_common_update_write_status
    lte_mac_common_get_write_ptr, lte_mac_common_get_read_ptr
*/
/*=========================================================================*/
extern void lte_mac_common_update_read_status
(
  lte_mac_common_dbuf_s *dbuf /*!< common double buffer pointer */
);

/*===========================================================================

  FUNCTION:  lte_mac_common_dbuf_reset

===========================================================================*/
/*!
    @brief
    This function resets the shared double buffer structure.

    @detail
    - input1 is the double buffering structure.This is global data which will 
    share between the reader and writer

    @return
    None

    @note
    None

    @see related_function()
    lte_mac_common_get_write_ptr, lte_mac_common_get_read_ptr
    lte_mac_common_update_write_status, lte_mac_common_update_read_status
*/
/*=========================================================================*/
extern void lte_mac_common_dbuf_reset
(
  lte_mac_common_dbuf_s     *dbuf   /*!< common double buffer pointer */
);

/*===========================================================================

  FUNCTION:  lte_mac_ctrl_get_state

===========================================================================*/
/*!
    @brief
    This function returns state of MAC control

    @detail
    
    @return
    None

    @see related_function()

*/
/*=========================================================================*/
extern lte_mac_common_state_e lte_mac_ctrl_get_state
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_ul_calc_ta_time

===========================================================================*/
/*!
    @brief
    Calculate the time between last rf_available_ind and rf_unavailable_ind

    @detail
    - input1: Time of last RF_AVAILABLE_IND
    - input2: Time of last RF_UNAVAILABLE_IND

    @return
    tuneaway time in milliseconds

    @note
    Does not work as expected for tuneaways greater than 10240 milliseconds

    @see related_function()

*/
/*=========================================================================*/
extern uint32 lte_mac_ul_calc_ta_time
( 
  lte_sfn_s last_rfa,         /*!< Time of last RF_AVAILABLE_IND */
  lte_sfn_s last_rfna         /*!< Time of last RF_UNAVAILABLE_IND */
);

/*===========================================================================

  FUNCTION:  lte_mac_common_bytes_per_ms_to_kbps

===========================================================================*/
/*!
    @brief
     Given the amount of data in bytes and the time in ms it returns the data
     rate in kbps

    @detail
    - input1: amount of data in bytes
    - input2: Time in milliseconds

    @return
    data rate in kbps

    @note
    returns 0 if the time is 0
    1 byte per millisecond = 8 kbps
    (data*8*1000)/(time*1000)

    @see related_function()

*/
/*=========================================================================*/
extern uint64 lte_mac_common_bytes_per_ms_to_kbps
( 
  uint64 data,         /*!< Data in bytes */
  uint32 time         /*!< Time in milliseconds */
);

#endif /* LTE_MAC_COMMON_H */
