#ifndef M1X_TIME_H
#define M1X_TIME_H

/*=============================================================================

              1 X  T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements 1x modem timekeeping API


EXTERNALIZED FUNCTIONS
  m1x_time_get
    Return the system timestamp value.

Copyright (c) 2011 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/drivers/inc/m1x_time.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/04/12   trc     Initial version, ported from time_cdma.h

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "time_types.h"

/** @addtogroup m1x_time
@{ */
/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/
/**
Time source status code returned by m1x_time_get()
*/
typedef enum
{
  M1X_TIME_INVALID = 0,                 /*!< 1x modem time is invalid/unset */
  M1X_TIME_CDMA_SOURCE,                 /*!< 1x modem time is network based */
  M1X_TIME_DEAD_RECKONING_SOURCE        /*!< 1x modem time is last-known good
                                             network time + timetick-based
                                             dead-reckoning */
} m1x_time_source_type;


/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/**
Returns the 64-bit CDMA system timestamp.

@param[out] time_val Address of the qword in which to return the 64-bit system
                     timestamp.

@return
@see m1x_time_source_type
time_val is set to the current system time in timestamp format.

*/
m1x_time_source_type m1x_time_get
(
  time_type                       time_val
);

/** @} */ /* end_addtogroup m1x_time */

#endif /* M1X_TIME_H */
