#ifndef CXM_ANTENNA_H
#define CXM_ANTENNA_H
/*!
  @file
  cxm_antenna.h

  @brief
  This file contains the message handlers and algorithms for sharing
  the diversity antenna with WLAN using WCI-2 Control Plane msging over QMB

*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/inc/cxm_antenna.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
11/17/13   btl     Initial version

==========================================================================*/

/*=============================================================================

                      INCLUDES

=============================================================================*/
#include <coexistence_service_v02.h>
#include <trm.h>

/*===========================================================================

                       DATA TYPES AND MACROS

===========================================================================*/

/*! @brief Listing of logical states software uses for antenna decisions */
typedef enum
{
  CXM_ANTENNA_UNUSED,
  CXM_ANTENNA_PENDING,
  CXM_ANTENNA_IRAT,
  CXM_ANTENNA_GRANTED
} cxm_antenna_state_e;

/*! @brief Antenna Sharing data storage type */
typedef struct
{
  cxm_antenna_state_e       state;
  coex_antenna_state_v02   *info;
  coex_tech_sync_state_v02 *sync;
  boolean                  *token_wrap;
  trm_duration_t            grant_dur;
  trm_duration_t            irat_dur;
  uint64                    grant_start_ts;
  uint64                    irat_start_ts;
} cxm_antenna_state_s;

/*=============================================================================

                      EXTERNAL FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  cxm_get_antenna_state_ptr

=============================================================================*/
/*!
    @brief
    Returns a pointer to the global cxm_antenna_state struct for diag logging.

    @return
    cxm_antenna_state_s *
*/
/*===========================================================================*/
cxm_antenna_state_s *cxm_get_antenna_state_ptr( void );

/*=============================================================================

  FUNCTION:  cxm_antenna_init

=============================================================================*/
/*!
    @brief
    Initialize resources and start diversity antenna resource request

    @return
    void
*/
/*===========================================================================*/
void cxm_antenna_init
(
  coex_tech_sync_state_v02 *sync_state_p,
  coex_antenna_state_v02   *state_info_p,
  boolean                  *token_wrap_flag_p
);

/*=============================================================================

  FUNCTION:  cxm_antenna_deinit

=============================================================================*/
/*!
    @brief
    Release resources associated with WWAN/WLAN antenna sharing

    @return
    void
*/
/*===========================================================================*/
void cxm_antenna_deinit( void );

/*=============================================================================

  FUNCTION:  cxm_antenna_handle_grant

=============================================================================*/
/*!
    @brief
    We have been granted the WLAN antenna; execute appropriate actions and
    send message to WLAN

    @return
    void
*/
/*===========================================================================*/
void cxm_antenna_handle_grant( void );

/*=============================================================================

  FUNCTION:  cxm_antenna_handle_release

=============================================================================*/
/*!
    @brief
    We have been asked us to release Antenna. Can be caused two ways:
      1. TRM asked us to release through callback
      2. Our grant duration is up and our timer fired

    @detail
    First try to re-acquire antenna. If that fails, submit a new request
    and notify WLAN that we lost it.

    @return
    void
*/
/*===========================================================================*/
void cxm_antenna_handle_release( void );

/*=============================================================================

  FUNCTION:  cxm_antenna_update_duration

=============================================================================*/
/*!
    @brief
    Update the grant duration saved in the QMB antenna state struct, based
    on the original grant duration and how much time has passed since grant

    @return
    void
*/
/*===========================================================================*/
void cxm_antenna_update_duration( void );

#endif /* CXM_ANTENNA_H */
