#ifndef __GPS_COEX_MGR_H__
#define __GPS_COEX_MGR_H__
/*!
  @file gps_coex_mgr.h

  @brief
   APIs exposed by the CXM for WWAN-GPS COEX

*/

/*=============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

=============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/gps_coex_mgr.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/13/14   jm      Adding frequency information to power list
05/20/13   ag      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "cxm.h"

/*=============================================================================

                       COMMON TYPES AND DEFINES

=============================================================================*/

/*! @brief Power Link information */
typedef struct
{
  /* Tx Power in dBm*10 format. 
     Will be accessed only if direction = UL or UL_AND_DL */
  int32   tx_power; 
  uint32  frequency;                  /*!< Tech operating frequency in KHz */
  uint32  bandwidth;                  /*!< Tech operating bandwidth in Hz */
  uint32  band;                       /*!< Tech operating band (sys_band_class_e_type) */
  uint32  channel;                    /*!< Tech operating channel */
}cxm_filter_power_link_s;


/*! @brief  Power List */
typedef struct
{
  uint8                    num_link_info_sets;
  cxm_filter_power_link_s  link_list[CXM_MAX_SUPPORTED_LINK_SETS];
}cxm_filter_power_list_s;

/*=============================================================================

                       MESSAGE DATA TYPES/STRUCTURES

=============================================================================*/

/*! @brief Message for sending the GPS coex power information */
typedef struct
{
  msgr_hdr_struct_type        msg_hdr;
  cxm_tech_type               tech_id;   /*!< WWAN Technology */
  cxm_filter_power_list_s     power_data; /*!< WWAN power info */
}cxm_filtered_power_ind_s;

/*=============================================================================

                        MSGR UMID Definitions

=============================================================================*/

/*! @brief These indications are used for GPS Coexistence mitigation
 */
enum 
{
  /*! Tech Power Indication ( Tech L1 to GPS ) for updating Tx/Rx power */
  MSGR_DEFINE_UMID( MCS, CXM, IND, FILTERED_POWER,
                    FILTERED_POWER_ID, cxm_filtered_power_ind_s ),  
};


/*=============================================================================

                        Function Declarations

=============================================================================*/

#endif /* __GPS_COEX_MGR_H__ */
