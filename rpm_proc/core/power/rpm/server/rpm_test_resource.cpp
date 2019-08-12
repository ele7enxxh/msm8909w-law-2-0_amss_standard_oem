/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "rpmserver.h"

static void rpm_test_xlate(rpm_translation_info *info)
{
}

static void rpm_test_apply(rpm_application_info *info)
{
}

void rpm_test_resource_init()
{
    rpm_register_resource(RPM_TEST_REQ, 1, 4, rpm_test_xlate, rpm_test_apply, 0);
}

