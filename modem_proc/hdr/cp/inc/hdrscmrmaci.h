#ifndef HDRSCMRMACI_H
#define HDRSCMRMACI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

           R E V E R S E  C H A N N E L  M A C  P R O T O C O L

                    I N T E R N A L   M O D U L E

       
GENERAL DESCRIPTION
  This module provides supporting API for SCM module to process
  ConfigurationRequest messages and AttributeUpdateRequest messages directed
  to Reverse Traffic channel MAC protocol. 
    
EXTERNALIZED FUNCTIONS
   
INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2007 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/inc/hdrscmrmaci.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/23/15   wsh     Fixed warning if FEATURE_HDR_REVB_SUPPORTED is undefined
04/29/13   smd     Disable RevB code if FEATURE_HDR_REVB_SUPPORTED is not defined.
04/16/07   cc      Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/

#include "hdrscmrmac4.h"
#include "hdrscmrmac3.h"
#include "hdrscmrmac0.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/


/*===========================================================================
 
                  DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
typedef union
{

#ifdef FEATURE_HDR_REVB_SUPPORTED
  hdrscmrmac4_complex_config_struct_type  sub4;
#endif /* FEATURE_HDR_REVB_SUPPORTED */

} hdrscmrmaci_complex_config_union_type;


 /* EJECT */
/*============================================================================

                        EXTERNAL FUNCTION DEFINITIONS

=============================================================================*/
#ifdef FEATURE_HDR_REVB_SUPPORTED
extern hdrscmrmaci_complex_config_union_type hdrscmrmaci;
#endif /* FEATURE_HDR_REVB_SUPPORTED */
 /* EJECT */
#endif /* HDRSCMRMACI_H */
