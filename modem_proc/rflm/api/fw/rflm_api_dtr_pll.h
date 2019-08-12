/*!
  @file
  rflm_api_dtr_pll.h

  @brief
  RFLM API DTR PLL configurations and voting
  
  @detail
  This file contains RFLM API DTR PLL configurations and voting

*/

/*==============================================================================

  Copyright (c) 2012- 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

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

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/fw/rflm_api_dtr_pll.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
10/03/13   vrb     Added missing #endif
10/02/13   vrb     Initial version.
==============================================================================*/

#ifndef RFLM_API_DTR_PLL_FW_H
#define RFLM_API_DTR_PLL_FW_H


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "rflm_api_cmn.h"



#ifdef __cplusplus
extern "C" {
#endif
/*==============================================================================

FUNCTION:  rflm_dtr_configure_plls

==============================================================================*/
/*!
  @brief
  Configure DTR PLLs to the prescribed frequencies

  @detail
 
  @return
  rflm error code
*/
/*============================================================================*/
extern rflm_err_t rflm_dtr_configure_plls( void );

/*==============================================================================*/
/*!
  @brief
  Vote DTR PLL on for the corresponding Rx Handle

  @detail
  The DTR PLL associated with the specified Rx handle will be voted on
 
  @return
  rflm error code
*/
/*============================================================================*/
extern rflm_err_t rflm_dtr_rx_vote_pll( rflm_handle_rx_t handle,
                                        boolean vote_on_off );

/*==============================================================================*/
/*!
  @brief
  Vote DTR PLL on for the corresponding Tx Handle

  @detail
  The DTR PLL associated with the specified Tx handle will be voted on
 
  @return
  rflm error code
*/
/*============================================================================*/
extern rflm_err_t rflm_dtr_tx_vote_pll( rflm_handle_tx_t handle,
                                        boolean vote_on_off );


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* RFLM_API_DTR_PLL_FW_H */

