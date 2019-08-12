/*===========================================================================
   FILE:         HALmpmintPlatform.c

   Reference chip release: MDM9x07 (Lykan) lykan_interrupts_unlinked_v3.4
 
   DESCRIPTION:   This is the platform hardware abstraction layer implementation for the
                  MPM interrupt controller block.
                  This platform is for the RPM on 8916.

                 Copyright © 2012-2015 QUALCOMM Technologies Incorporated.
                 All Rights Reserved.
                 QUALCOMM Confidential and Proprietary.

  =============================================================================
  =============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/config/asic/9607/MPMIntDevCfgData.c#1 $
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
  { MPM_TRIGGER_RISING_EDGE,  16              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  5               , MPM_GPIO },
  /*5*/                                                  
  { MPM_TRIGGER_RISING_EDGE,  11              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  12              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  3               , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  17              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  9               , MPM_GPIO },
  /*10*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  13              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  1               , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  20              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  21              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  22              , MPM_GPIO },
  /*15*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  75              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  74              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  28              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  44              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  26              , MPM_GPIO },
/*20*/                                                   
  { MPM_TRIGGER_RISING_EDGE,  43              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  42              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  29              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  69              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  30              , MPM_GPIO },
  /*25*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  37              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  25              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  71              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  34              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  55              , MPM_GPIO },
  /*30*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  8               , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  40              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  48              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  52              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  57              , MPM_GPIO },
  /*35*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  62              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  66              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  59              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  79              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  38              , MPM_GPIO },
  /*40*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  63              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  76              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  /*45*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  /*50*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  /*55*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  /*60*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  89              , MPM_IRQ  }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN  }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },  
  /*64 - End of the table entry */                       
  { MPM_TRIGGER_LEVEL_HIGH, MPM_UNMAPPED_INT  , MPM_EOT  },
};
