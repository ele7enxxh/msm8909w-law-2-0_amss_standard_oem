/*===========================================================================
                        mcfg_feature_config.h

DESCRIPTION

   Feature config supprot for the MCFG module
 
Copyright (c) 2012-2013 by QUALCOMM Technologies Incorporated.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/api/mcfg_feature_config.h#1 $ $DateTime: 2016/12/13 08:00:27 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
1/18/12      sbt   Create

===========================================================================*/

#ifndef MODEM_CFG_FEATURE_CONFIG_H
#define MODEM_CFG_FEATURE_CONFIG_H

#include "comdef.h"

/* Mux support */
#define FEATURE_MODEM_CFG_MUX_SUPPORT

/* Debugging support */
//#define MCFG_DEBUG_EFS_LOG_SUPPORT
//#define FEATURE_MCFG_ACTIVATE_W_NO_RESET
/* For testing auth support */
// #define FEATURE_QDST_SECBOOT 

/* Disable default config when NV is not set */
// #define FEATURE_MCFG_DISABLE_DEFAULT_CONFIG_W_NV_INACTIVE

/* For Auto IMSI Selection */
#define FEATURE_MCFG_SELECTION_BY_IMSI 

/* For Multi-SIM support */
#define FEATURE_MCFG_MULTISIM_SUPPORT 

/* For Modem inititated graceful rest */
#define FEATURE_MCFG_SSREQ

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  #define FEATURE_MCFG_REFRESH
#endif

/* For MCFG FS API Support */
#define FEATURE_MCFG_FS_API_SUPPORT

#endif /* MODEM_CFG_FEATURE_CONFIG_H */
