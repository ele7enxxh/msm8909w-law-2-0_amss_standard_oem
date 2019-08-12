/*!
  @file DsmPullupTest2.cpp

  @brief Testing dsm_pullup() emptying packet

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

$Header: //components/rel/core.mpss/3.5.c12.3/memory/dsm/test/DsmPullupTest2.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/30/09   ag      Initial Revision.

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

TF_DEFINE_BASIC_TEST_CASE(COMPONENT, DsmPullupTest2);

#define TEST_SIZE (DSM_DS_SMALL_ITEM_SIZ - 1)


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           FUNCTIONS DEFINITIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/



void DsmPullupTest2::Test()
{
  dsm_item_type * tmp1 = NULL;
  uint8 buf[200];
  uint16 len;
  uint16 ret;

  TF_MSG("dsm_init");
  dsm_init();

  memset(buf,0xBB,200);

  len = dsm_pushdown(&tmp1, buf, TEST_SIZE, DSM_DS_SMALL_ITEM_POOL);
  TF_MSG("dsm_pushdown returned:len %d:tmp1 %x",(int)len,(int)tmp1);

  tf_dsm_print_packet(tmp1);

  memset(buf,0xAA,200);

  ret = dsm_pullup(&tmp1, buf, TEST_SIZE);
  TF_MSG("dsm_pullup returned:tmp1 %x",(int)tmp1);

  TF_ASSERT(tmp1 == NULL);

  TF_ASSERT(ret == TEST_SIZE);

} /* TF_dsm_test_script() */
