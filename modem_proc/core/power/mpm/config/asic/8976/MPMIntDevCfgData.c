/*===========================================================================
   FILE:         HALmpmintPlatform.c
 
   DESCRIPTION:   This is the platform hardware abstraction layer implementation for the
                  MPM interrupt controller block.
                  This platform is for the RPM on 8916.

                 Copyright © 2012-2013 QUALCOMM Technologies Incorporated.
                 All Rights Reserved.
                 QUALCOMM Confidential and Proprietary.

  =============================================================================
  =============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/config/asic/8976/MPMIntDevCfgData.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $
========================================================================*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */
#include "vmpm_internal.h"

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

/**
 * Table mapping the MPM Pin number to the GPIO and/or IRQ pin 
 * number per the IP Catalog. 
 */
mpm_int_pin_mapping_type devcfg_MpmInterruptPinNum_Mapping[] =
{
  /* Trigger                 Local Pin#         Pin Type   */
  /* -------                 -------------      --------   */
  /*0*/
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  100             , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_GPIO },
  /*5*/                                                  
  { MPM_TRIGGER_RISING_EDGE,  5               , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  9               , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  106             , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  119             , MPM_GPIO },
  /*10*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  133             , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  135             , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  12              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  13              , MPM_GPIO },	
  { MPM_TRIGGER_RISING_EDGE,  138             , MPM_GPIO },
  /*15*/                        
  { MPM_TRIGGER_RISING_EDGE,  139             , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  140             , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  21              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  52              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  25              , MPM_GPIO },
  /*20*/                                                   
  { MPM_TRIGGER_RISING_EDGE,  141             , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  142             , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  28              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  144             , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  17              , MPM_GPIO },
  /*25*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  33              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  56              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  60              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  38              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  107             , MPM_GPIO },
  /*30*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  109             , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  67              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  112             , MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  113             , MPM_GPIO },
  /*35*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  114             , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  115             , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  68              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  118             , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  120             , MPM_GPIO },
  /*40*/                                                   
  { MPM_TRIGGER_RISING_EDGE,  121             , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  102             , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_GPIO },
  /*45*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  /*50*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  { MPM_TRIGGER_RISING_EDGE,  47              , MPM_GPIO }, 
  { MPM_TRIGGER_RISING_EDGE,  48              , MPM_GPIO }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  /*55*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  /*60*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_IRQ  }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_IRQ  }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },  
  /*64 - End of the table entry */                       
  { MPM_TRIGGER_LEVEL_HIGH,   MPM_UNMAPPED_INT, MPM_EOT  },
};
