/*!
  @file DsmHeaderSizeTest.cpp

  @brief Checking the header size of a DSM item.

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

$Header: //components/rel/core.mpss/3.5.c12.3/memory/dsm/test/DsmHeaderSizeTest.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/09   ag     Initial Revision. Ported from linux tests.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <TestFramework.h>

extern "C" {
  #include "dsm.h"
}

/*===========================================================================

                      DEFINITIONS AND DECLARATIONS 

===========================================================================*/

TF_DEFINE_BASIC_TEST_CASE(COMPONENT, DsmHeaderSizeTest);


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           FUNCTIONS DEFINITIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

void DsmHeaderSizeTest::Test()
{

  int bytes = 0;

  TF_MSG("POOL MGMT TABLE SIZE %d", sizeof(dsm_pool_mgmt_table_type));

  TF_MSG("TAIL COOKIE SIZE %d", sizeof(void *));
  bytes += sizeof(void *);

  TF_MSG("LINK SIZE %d", sizeof(q_link_type));
  bytes += sizeof(q_link_type);

  TF_MSG("PKT_PTR SIZE %d", sizeof(struct dsm_item_s *));
  bytes += sizeof(struct dsm_item_s *);

  TF_MSG("DUP_PTR SIZE %d", sizeof(struct dsm_item_s *));
  bytes += sizeof(struct dsm_item_s *);

  TF_MSG("AP_FIELD %d", sizeof(uint32));
  bytes += sizeof(uint32);

  TF_MSG("APP_PTR %d", sizeof(void*));
  bytes += sizeof(void*);

  TF_MSG("POOL_ID %d", sizeof(dsm_mempool_id_type));
  bytes += sizeof(dsm_mempool_id_type);
  
  TF_MSG("SIZE %d", sizeof(uint16));
  bytes += sizeof(uint16);
  
  TF_MSG("USED %d", sizeof(uint16));
  bytes += sizeof(uint16);

  TF_MSG("REFERENCES %d", sizeof(uint8));
  bytes += sizeof(uint8);
 
  TF_MSG("KIND %d", sizeof(uint8));
  bytes += sizeof(uint8);
 
#ifdef DSM_PRIORITY
  TF_MSG("PRIORITY %d",sizeof(uint8));
  bytes += sizeof(uint8);
#else
  TF_MSG("DSM_PRIORITY is NOT DEFINED");
#endif


#ifdef DSM_TRACER
  TF_MSG("TRACER %d",sizeof(uint8));
  bytes += sizeof(uint8);
#else
  TF_MSG("DSM_TRACER is NOT DEFINED");
#endif

  TF_MSG("DATA_PTR SIZE %d", sizeof(uint8 *));
  bytes += sizeof(uint8 *);

  
  TF_MSG("TRACE SIZE %d:DSM_TRACE_LEN %d", 
            sizeof(dsm_item_trace) * DSM_TRACE_LEN , DSM_TRACE_LEN);
  bytes += sizeof(dsm_item_trace) * DSM_TRACE_LEN;
  
  TF_MSG("HEAD COOKIE SIZE %d", sizeof(void *));
  bytes += sizeof(void *);

  TF_MSG("Total overhead %d",bytes);

} /* Test */
