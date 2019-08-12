#ifndef RFM_1X_SV_H
#define RFM_1X_SV_H

/*!
  @file
  SVDO 1x specific public interface.

  @details
  This file exports the definitions and declartions required for external
  clients to access the 1x portion of the SVDO RF driver.
*/

/*==============================================================================

  Copyright (c) 2010 - 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/cdma/rfm_1x_sv.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
03/14/11   sar     Contents of this file were moved to rfm_1x.h. The file will be
                   removed after the 1x updates their files.
01/20/11   aro     Added new Tx CL Mode and documentation update
01/20/11   aro     Updated TX Closed Loop Power mode Enum
01/19/11   aro     Updated Set Tx Closed Loop Power Interface
01/17/11   aro     Removed unnecessary Interface
01/15/11   aro     Changed the Tune function name
01/13/11   aro     Renamed function names
01/12/11   bmg     Updated the 1x API with link manaager parameters
01/06/11   aro     Added RxAGC Query function
12/16/10   aro     Renamed Get RxAGC function for consistency
12/10/10   aro     Added new interfaces for Tx Power Control
12/06/10   aro     [1] Interface Update
                   [2] Doxygen Update
10/16/09   bmg     Changed rfm_1x_get_cdma_tx_agc() interface to singed
                   1/10th dBm
09/10/09   sr      Defined RF_CFS_TUNE_WAIT
06/02/09   aro     Renamed Device State Enum
06/02/09   Saul    Removed rfm_trm.h.
05/20/09   sar     Updated rfm_1x_get_state() with new return type.
03/24/09   sl      Filled in functionality by calling legacy functions.
02/20/09   sar     Initial Release.

==============================================================================*/
#include "rfm_1x.h"
#endif  /* RFM_1X_SV_H */

