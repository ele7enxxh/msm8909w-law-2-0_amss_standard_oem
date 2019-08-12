/*!
  @file DsmInitTest.cpp

  @brief Testing initialization of pools in dsm_init().

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

$Header: //components/rel/core.mpss/3.5.c12.3/memory/dsm/test/DsmInitTest.cpp#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/30/10   ag      Remove diag pool verification as dsm_init do not init diag 
07/30/09   ag      Initial Revision. Ported from linux tests.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "TestFramework.h"

extern "C" {
  #include "dsm.h"
  #include "tf_dsm_main.h"
}

/*===========================================================================

                      DEFINITIONS AND DECLARATIONS 

===========================================================================*/

TF_DEFINE_BASIC_TEST_CASE(COMPONENT, DsmInitTest);


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           FUNCTIONS DEFINITIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

void DsmInitTest::Test()
{

  TF_MSG("dsm_init");
  dsm_init();

  tf_dsm_print_pool(DSM_DS_SMALL_ITEM_POOL);
#ifdef FEATURE_DSM_LARGE_ITEMS
  tf_dsm_print_pool(DSM_DS_LARGE_ITEM_POOL);
#endif
#ifdef FEATURE_DSM_DUP_ITEMS
  tf_dsm_print_pool(DSM_DUP_ITEM_POOL);
#endif

  return ;

} /* Test */

