#ifndef _TZAPPS_TEST_CONFIG_H_
#define _TZAPPS_TEST_CONFIG_H_
/*===========================================================================
  Copyright (c) 2013-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/tzdrm/tzcommon/common/shared/inc/tzapps_test_config.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/16/14   dm      Moved common inclusions to the header file
04/03/14   tp      Modified by running "atyle --style=allman" command.
02/14/14   dm      Moved the definition of CPcheck variable to source file
04/08/13   dm      Added new function to get the CP test flag value
04/04/13   dm      Created
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "qsee_log.h"
#include "qsee_core.h"

/**
  Enables or Disables the internal test flag.
  If g_internal_test_flag = 1, internal unit testing enabled in tzapps
  currently, this flag  is used for playready decrypt test cases
  If g_internal_test_flag = 0, internal unit testing disabled in tzapps
  as in customer release builds

  @return
  1 - Success
  0 - Failure

  @param
  None

  @dependencies
  None.

  @sideeffects
  None
*/
boolean get_internal_test_flag(void);

/**
  Enables/Disables the Content Protection Check.

  @return
   0 - Success
  -1 - Failure

  @param
  None

  @dependencies
  This API is supported only on internal builds.

  @sideeffects
  None
*/
long qsapps_cpcheck_toggle
(
    uint8             bContentProtection
);

/**
  Returns the status of content protection flag.
  If get_internal_test_CP_flag = 1, enforce content protection check while decrypting the secure content
  If get_internal_test_CP_flag = 0, bypass content protection check while decrypting the secure content

  @return
  1 - Content Protection check is turned on.
  0 - Content Protection check is turned off (only for internal testing)

  @param
  None

  @dependencies
  None

  @sideeffects
  None
*/
boolean get_internal_test_CP_flag(void);

#endif /* _TZAPPS_TEST_CONFIG_H_*/
