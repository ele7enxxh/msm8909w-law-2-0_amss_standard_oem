#ifndef QDSS_CSR_CONFIG_H
#define QDSS_CSR_CONFIG_H

/*=============================================================================

FILE:         qdss_csr_config.h

DESCRIPTION:  

================================================================================
              Copyright (c) 2012 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/rpm.bf/2.1.1/core/debugtrace/csr/config/9x07/qdss_csr_config.h#1 $
==============================================================================*/


#define QDSS_QDSSCSR_REG_BASE_PHYS      (0x66001000) 


//Values from hector for bus hang and wdog bite
#define HWEVENT_MUX_CTRL_0 0x00030000 
#define HWEVENT_MUX_CTRL_1 0x0FFF07F0  
#define HWEVENT_MUX_CTRL_2 0x10000006
#define HWEVENT_MUX_CTRL_3 0x7D000000 

// STMAWID_DEFAULT   0
// STMAWID_MODEM     1
// STMAWID_BAMDMA    2
// STMAWID_RESERVED  3
//

#define QDSS_CSR_STMAWID_ASSIGNMENTS       0x03020100  
#define QDSS_CSR_STM_BURST_TRANSALATORS    0x0  //0110, Enable Modem and BAMDMA


#endif //QDSS_CSR_CONFIG_H
