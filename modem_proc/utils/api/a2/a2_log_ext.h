#ifndef A2_LOG_EXT_H
#define A2_LOG_EXT_H
/*!
  @file a2_log_ext.h

  @brief
  External interface to enable DPL logging on DL.

  @ingroup per_implementation
  @ingroup a2_ext_interface
*/

/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/a2/a2_log_ext.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
09/05/12   yuw     Initial Checkin fro CR399592 DPL Logging Support

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <dsm.h>
#include <sio.h>
#include <a2_common.h>
#include <a2_ext.h>

/*===========================================================================

                               MACROS

===========================================================================*/

/*  Macro of the option that full packet will be logged */
#define A2_LOG_DPL_FULL_PKT  0xFFFFFFFF

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  a2_log_dpl_register_wm

===========================================================================*/
/*!
    @brief
    External API to register DPL logging WM in A2
    There is only one WM for DPL logging between Data and A2

    @arguments:
    dpl_wm_ptr:  The watermark being registered to pass 
                              DPL log packets from A2 to DS
    @note:
    expected to be called one time during power up
    
  @return:
    None
*/
/*=========================================================================*/
void a2_log_dpl_register_wm
(
  /*! The watermark being registered to pass DPL log packets from A2 to DS */
  dsm_watermark_type* dpl_wm_ptr 
);


/*===========================================================================

  FUNCTION:  a2_log_dpl_enable

===========================================================================*/
/*!
    @brief
    External API to register DPL logging in A2

  @arguments:
    epsid     : The eps bear id on which logging will be actived
    ip_type  : The ip type for which logging will be actived
    log_pkt_len  :  length of packet that needs to be logged
                    A2_LOG_DPL_FULL_PKT-> full packet
                    length zero when DPL is enabled is an invalid case
    cb_data  :  The pointer of call back data that caller needs

  @return:
    A2_SUCCESS
    A2_ERROR_INVALID_FIRST_ARGUMENT
    A2_ERROR_INVALID_SECOND_ARGUMENT
    
*/
/*=========================================================================*/
a2_error_e a2_log_dpl_enable
(
  /*! The epsid on which logging will be actived */
  uint8 epsid,
  /*! The ip type for which logging will be actived */
  a2_ip_type_e ip_type,
  /*! length of packet that needs to be logged */
  uint32 log_pkt_len,
  /*! call back data that caller needs to feedback as a part of CB function*/
  uint32 cb_data
);

/*===========================================================================

  FUNCTION:  a2_log_dpl_disable

===========================================================================*/
/*!
    @brief
    External API to deregister DPL logging in A2

  @arguments:
    epsid     : The eps bear id on which logging will be deactived
    ip_type  : The ip type for which logging will be deactived

  @return:
    A2_SUCCESS
    A2_ERROR_INVALID_FIRST_ARGUMENT
*/
/*=========================================================================*/
a2_error_e a2_log_dpl_disable
(
  /*! The epsid on which logging will be deactived */
  uint8 epsid,
  /*! The ip type for which logging will be deactived */
  a2_ip_type_e ip_type
);

/*===========================================================================

  FUNCTION:  a2_log_get_dpl_pkt_info

===========================================================================*/
/*!
    @brief
    External API to get user data from log packet

  @arguments:
    pkt_ptr:  DSM dequeued from the WM that data registered for DPL 
              with a2_log_dpl_register_wm() during power up

  @return:
    user data provided by data when DPL is enabled with a2_log_dpl_enable()
*/
/*=========================================================================*/
uint32 a2_log_get_dpl_pkt_info
(
  /*! DSM of log packet */
  dsm_item_type*  pkt_ptr
);

#endif /* A2_LOG_EXT_H */

