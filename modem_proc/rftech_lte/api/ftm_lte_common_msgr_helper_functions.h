/*!
  @file ftm_lte_common_msgr_helper_functions.h

  @brief
  This files contains the definition of the LTE disaptcher shared 
  for RF CAL and Non-signaling commands, in addition to some common 
  ultilities

*/

/*===========================================================================

  Copyright (c) 2009 - 2011 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/api/ftm_lte_common_msgr_helper_functions.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/20/11   pl      create ftm_lte_init and ftm_lte_deinit
04/20/11   pl      Remove L1 online mode from public interfaces
03/25/10   pl/mkv  Add support for L1 online mode switching
05/12/09   mkv     Initial Check-in
===========================================================================*/

#ifndef FTM_LTE_COMMON_MSGR_HELPER_FUNCTIONS_H
#define FTM_LTE_COMMON_MSGR_HELPER_FUNCTIONS_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ftm.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

  FUNCTION:  ftm_lte_build_msgr_msg  

===========================================================================*/
/*!
  @brief
  This function converts a payload packet from DIAG task into a Message
  router Message

  @param payload_ptr the input payload from Diag
  @param msgr_msg_ptr message router message buffer to store the converted message

  @return
  TRUE if conversion is sucessful
  FALSE if conversion failed
*/
/*=========================================================================*/
boolean ftm_lte_build_msgr_msg(ftm_lte_payload_type *payload_ptr, ftm_msgr_u *msgr_msg_ptr );

/*===========================================================================

  FUNCTION:  ftm_lte_msgr_init  

===========================================================================*/
/*!
  @brief
  FTM LTE initialization for Message router


  @return
  NONE
*/
/*=========================================================================*/
void ftm_lte_init(void);

/*===========================================================================

  FUNCTION:  ftm_lte_msgr_deinit  

===========================================================================*/
/*!
  @brief
  FTM LTE de-initialization for Message router


  @return
  NONE
*/
/*=========================================================================*/
void ftm_lte_deinit(void);
#endif /*  FTM_LTE_COMMON_MSGR_HELPER_FUNCTIONS_H */
