/*!
  @file 
  rfd_grfc.h

  @brief
  header file for all grfc typedef and functions that are made available to
  external mododule

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfd/nikel/inc/rfd_grfc_modem.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/10/11   tnt     Initial check-in.
===========================================================================*/
#ifndef RFD_GRFC_MODEM_H
#define RFD_GRFC_MODEM_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*----------------------------------------------------------------------------*/
/*!
  @brief 
  this is used to signal the end of rfc table

  @details 
  
*/
#define RESERVED_ENGINE_MAX     GRFC_ENGINE_MAX
#define RESERVED_RF_CTL_SIG_MAX RF_CTL_SIG_MAX

/*----------------------------------------------------------------------------*/
/*!
   @brief 
   All 24 GRFC engines input to RFD Routing Table

   @details 
   These input will drive GRFCs
*/
typedef enum
{
    GRFC_ENGINE_00 = 0,
    GRFC_ENGINE_01 = 1,
    GRFC_ENGINE_02 = 2,
    GRFC_ENGINE_03 = 3,
    GRFC_ENGINE_04 = 4,
    GRFC_ENGINE_05 = 5,
    GRFC_ENGINE_06 = 6,
    GRFC_ENGINE_07 = 7,
    GRFC_ENGINE_08 = 8,
    GRFC_ENGINE_09 = 9,
    GRFC_ENGINE_10 = 10,
    GRFC_ENGINE_11 = 11,
    GRFC_ENGINE_12 = 12,
    GRFC_ENGINE_13 = 13,
    GRFC_ENGINE_14 = 14,
    GRFC_ENGINE_15 = 15,
    GRFC_ENGINE_16 = 16,
    GRFC_ENGINE_17 = 17,
    GRFC_ENGINE_18 = 18,
    GRFC_ENGINE_19 = 19,
    GRFC_ENGINE_20 = 20,
    GRFC_ENGINE_21 = 21,
    GRFC_ENGINE_22 = 22,
    GRFC_ENGINE_23 = 23,
    GRFC_ENGINE_MAX = 24
}rfd_rt_input_grfc_engine_type;

/*----------------------------------------------------------------------------*/
/*!
   @brief 
   All 22 internal GRFC ENGINES RF 

   @details 
   These engines will drive RF control signals
*/
typedef enum
{
    GRFC_ENGINE_RF_00 = 0,
    GRFC_ENGINE_RF_01 = 1,
    GRFC_ENGINE_RF_02 = 2,
    GRFC_ENGINE_RF_03 = 3,
    GRFC_ENGINE_RF_04 = 4,
    GRFC_ENGINE_RF_05 = 5,
    GRFC_ENGINE_RF_06 = 6,
    GRFC_ENGINE_RF_07 = 7,
    GRFC_ENGINE_RF_08 = 8,
    GRFC_ENGINE_RF_09 = 9,
    GRFC_ENGINE_RF_10 = 10,
    GRFC_ENGINE_RF_11 = 11,
    GRFC_ENGINE_RF_12 = 12,
    GRFC_ENGINE_RF_13 = 13,
    GRFC_ENGINE_RF_14 = 14,
    GRFC_ENGINE_RF_15 = 15,
    GRFC_ENGINE_RF_16 = 16,
    GRFC_ENGINE_RF_17 = 17,
    GRFC_ENGINE_RF_18 = 18,
    GRFC_ENGINE_RF_19 = 19,
    GRFC_ENGINE_RF_20 = 20,
    GRFC_ENGINE_RF_21 = 21,
    GRFC_ENGINE_RF_MAX = 22
}rfd_internal_grfc_engine_rf_enum_type;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* RFD_GRFC_MODEM_H */
