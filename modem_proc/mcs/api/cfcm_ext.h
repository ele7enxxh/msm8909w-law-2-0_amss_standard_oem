
/*!
  @file
  cfcm_ext.h

  @brief
  CFCM(Centralized Flow control Manager) API exposed outside modem.

*/

/*==============================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/cfcm_ext.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/07/14   rj      initial version
==============================================================================*/

#ifndef CFCM_EXT_H
#define CFCM_EXT_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "comdef.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief data rate used when no cap is needed
*/
#define CFCM_MAX_DATA_RATE                          0xffffffff

/*! @brief modem radio access technology enumeration
*/
typedef enum
{
  CFCM_MODEM_RAT_LTE= 0,   /*!< LTE radio access tech */
  CFCM_MODEM_RAT_MAX       /*!< MAX */
} cfcm_modem_rat_e;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  cfcm_cap_ul_data_rate

==============================================================================*/
/*!
    @brief
    Cap the UL data rate for the specified modem radio access technoloy

    @detail
    - this API is intended for external entity to control the UL data rate for
    the specified radio access technology. The external entity is expected to
    assume all responsibility for thermal mitigation on the modem for the RAT. 

    - When this API is called, it is expected that LTE internal thermal 
    mitigation mechanism is not enabled. Otherwise, data_rate will not be 
    applied and the functino will return FALSE. 

    - The data rate convergence will be a best effort appoximation over a
    period of time in hundreds ms. SRB and RLC control PDUs are excluded, 
    in other words, only application level data is accounted for.

    - The data rate is sticky. At power UP, CFCM_MAX_DATA_RATE is used, it is 
    going to be changed with this API call only. 

    - setting data rate to 0 will effectively prevent application data from
    being transmitted. Use with caution.

    @return
    TRUE if data rate is successfully set. FALSE otherwise. 
*/
/*============================================================================*/
boolean cfcm_cap_ul_data_rate
(
  cfcm_modem_rat_e rat, /*!< the radio access technology rate is applied to */
  uint32          data_rate/*!< maximum data rate in number of byte per ms.  
                   Using CFCM_MAX_DATA_RATE  will turn off flow control */
);

#endif /* CFCM_EXT_H */
