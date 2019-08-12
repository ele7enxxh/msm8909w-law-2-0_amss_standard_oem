#ifndef PS_IPDC_QSH_DBG_H
#define PS_IPDC_QSH_DBG_H

#ifdef __cplusplus
extern "C" {
#endif

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           P S _ I P D C _ Q S H _ D B G . H

GENERAL DESCRIPTION
  Header file for the IP Data Compression protocol utility functions
  
 
  Copyright (c) 2015 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/06/15    cx     Created module.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef TEST_FRAMEWORK
#include <qsh.h>
#endif //TEST_FRAMEWORK
#include "ps_ipdc_ex.h"

/*===========================================================================

                     DEFINITIONS AND CONSTANTS

===========================================================================*/

#ifndef TEST_FRAMEWORK
/*===========================================================================
FUNCTION PS_IPDC_QSH_RESET()

DESCRIPTION
  Reset the cumulative statistics in UDC

DEPENDENCIES
  None
  
ARGUMENTS
  IPDC handle

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_ipdc_qsh_reset(ipdc_handle_type handle);

/*===========================================================================
FUNCTION PS_IPDC_QSH_ANALYSIS()

DESCRIPTION
  Analysis current UDC state

DEPENDENCIES
  None
  
ARGUMENTS
  qsh_params: qsh parameters

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_ipdc_qsh_analysis(qsh_cb_params_s *qsh_params);
#endif //TEST_FRAMEWORK

#endif  // #ifndef PS_IPDC_QSH_DBG_H
