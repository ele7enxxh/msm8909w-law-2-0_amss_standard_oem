/*!
  @file
  cfcm_qsh.h

  @brief
  QSH client for CFCM.
*/

/*==============================================================================

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cfcm/inc/cfcm_qsh.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
07/08/14   rj      Initial check-in
==============================================================================*/

#ifndef CFCM_QSH_H
#define CFCM_QSH_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "cfcm.h"
#include "cfcm_monitor.h"
#include "cfcm_client.h"
#include "qsh.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! Mapping CFM cliend ID to CFCM till CFCM is updated in QSH */
#define CFCM_CLT_QSH QSH_CLT_CFCM

/*! client major version */
#define CFCM_QSH_VER_MAJOR 1

/*! client minor version */
#define CFCM_QSH_VER_MINOR 0

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  cfcm_qsh_analysis_cb

==============================================================================*/
/*!
  @brief
  Callback registered with QSH.
  
  @note
  If async_allowed = TRUE and the analysis is done in a separate thread, the 
  qsh_cb_params_s data should be copied out.
  
  @return
  For now, just return TRUE.
*/
/*============================================================================*/
boolean cfcm_qsh_analysis_cb
(
  qsh_cb_params_s *   qsh_params
);

#endif /* CFM_QSH_H */
