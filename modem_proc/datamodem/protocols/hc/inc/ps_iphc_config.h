#ifndef IPHC_CONFIG_H
#define IPHC_CONFIG_H
/*===========================================================================
                        P S _ I P H C _ C O N F I G . H

DESCRIPTION
  This file contains the compile time configuration parameters divided into
  configurable and non-configurable compile time values.

Copyright (c) 2006-2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/hc/inc/ps_iphc_config.h#1 $
  $Author: mplcsds1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/01/09    pp     ZI optimizations: Removed IPHC heap config param.
01/31/07    scb    Created a new configuration parameter
                   IPHC_TWICE_ALG_MAX_TRIES to indicate
                   the maximum number of times the twice algorithm can
                   be performed. 
11/02/06    ifk    Created module.
===========================================================================*/

/*===========================================================================

                             INCLUDE FILES 

===========================================================================*/
#include "comdef.h"
#include "customer.h"


/*===========================================================================

                             CONFIGURABLE PARAMETERS

===========================================================================*/
/*---------------------------------------------------------------------------
  Size of the memory heap used by IPHC
---------------------------------------------------------------------------*/
#define IPHC_MEMHEAP_SIZE  65535    /* Size of the memheap used by IPHC    */

/*---------------------------------------------------------------------------
  Total number of retries allowed for the twice algorithm needed to 
  repair the IPHC context.
---------------------------------------------------------------------------*/
#define IPHC_TWICE_ALG_MAX_TRIES  2          /* Number of times twice
                                             algorithm can be performed
                                             to repair TCP context*/
/*---------------------------------------------------------------------------
  Enumeration of configurable parameters
---------------------------------------------------------------------------*/
typedef enum iphci_dynamic_config_e_type
{
  IPHCI_DYNAMIC_CONFIG_MEMHEAP_SIZE         = 0,
  IPHCI_DYNAMIC_CONFIG_TWICE_ALG_MAX_TRIES  = 1,
  IPHCI_DYNAMIC_CONFIG_MAX
} iphci_dynamic_config_e_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION IPHCI_GET_CONFIG

DESCRIPTION
  Function called by IPHC library to acquire a configurable compile time
  parameter.

DEPENDENCIES
  None

RETURN VALUE
   0  Success
  -1 Failure

SIDE EFFECTS
  None
===========================================================================*/
int iphci_get_config
(
  iphci_dynamic_config_e_type  config_param,
  void                        *config_val,
  uint32                       config_val_size 
);
#endif /* ROHC_CONFIG_H */
