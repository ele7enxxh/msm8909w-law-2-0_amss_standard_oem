#ifndef QDSS_CTI_CONFIG_H
#define QDSS_CTI_CONFIG_H

/*=============================================================================

FILE:         qdss_cti_config.h

DESCRIPTION:  

================================================================================
              Copyright (c) 2012 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/rpm.bf/2.1.1/core/debugtrace/cti/config/8936/qdss_cti_config.h#1 $
==============================================================================*/


#define QDSS_CTI0_BASE_PHYS      (0x60810000) 

//CPU/DSP CTIs
#define QDSS_A53SS0_CTI_BASE_PHYS      (0x60858000) 
#define QDSS_A53SS1_CTI_BASE_PHYS      (0x60859000) 
#define QDSS_A53SS2_CTI_BASE_PHYS      (0x6085A000) 
#define QDSS_A53SS3_CTI_BASE_PHYS      (0x6085B000) 
#define QDSS_VENUS_CTI_BASE_PHYS       (0x60830000) 
#define QDSS_PRONTO_CTI_BASE_PHYS      (0x60835000) 
#define QDSS_MPSS_CTI_BASE_PHYS        (0x60838000)  
#define QDSS_RPM_CTI_BASE_PHYS         (0x6083C000) 
//#define QDSS_MODEM_CM3_CTI_BASE_PHYS   (0xFC35C000) 


#define CPU_DSP_CTIS {                          \
      QDSS_A53SS0_CTI_BASE_PHYS,                \
         QDSS_A53SS1_CTI_BASE_PHYS,             \
         QDSS_A53SS2_CTI_BASE_PHYS,             \
         QDSS_A53SS3_CTI_BASE_PHYS,             \
         QDSS_VENUS_CTI_BASE_PHYS,              \
         QDSS_PRONTO_CTI_BASE_PHYS,             \
         QDSS_MPSS_CTI_BASE_PHYS,               \
         QDSS_RPM_CTI_BASE_PHYS,                \
         }
		 
		 /* QDSS_MODEM_CM3_CTI_BASE_PHYS           */

#endif //QDSS_CTI_CONFIG_H
