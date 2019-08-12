#ifndef TARGET_H
#define TARGET_H
/**
  @file target.h
  @brief All the declarations and definitions necessary for general 
  configuration of the AMSS software for a given target environment.

*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Utility_Services_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Utility_Services_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "utils_services" 
      group description in the Utility_Services_API_mainpage.dox file. 
===========================================================================*/
/*===========================================================================

      T A R G E T   C O N F I G U R A T I O N   H E A D E R   F I L E

DESCRIPTION
  All the declarations and definitions necessary for general configuration
  of the AMSS software for a given target environment.

Copyright (c) 1998-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/services/target.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/21/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/10/10   EBR     DOXYGENATED FILE
07/12/02   jct     The appropriate target file is now included via customer.h
                   in cust*.h.  TG is now fixed to be T_G and all other T_xxx
                   vars that could be assigned to T_G are deleted.  The use of
                   TG is deprecated. 
10/06/00   dsb     The appropriate target file is now included automatically
                   a make macro. Removed check on T_ACP, T_SMS, and T_SMS_UI.
                   Cannot remove check on T_AUTH as many files still use this 
                   instead of FEATURE_AUTH as they should.
                   Eventually, we want to eliminate T==T_xxx codes, but many
                   upper layer files still use them, so I have left them in.
                   When the rest of the DMSS stops using them, they should be
                   removed here.
08/13/99   jkl     Included T_Q2
01/15/99   hrk     Integrating from COMMON_ARM.03.01.00 VU from MSM3000 subsystem.
                   Re-introduced #defines for all targets. Removal of support for
                   these targets will be done in the next phase.
12/16/98   jct     Created for MSM3000+ support

===========================================================================*/
/** @addtogroup utils_services
@{ */
/** @name Target Macros
@{ */

/** PC test software; Intel 80386/80486 processors, running MS-DOS.
*/
#define T_PC    1
/** Portable phone hardware.
*/     
#define T_P     2
/** Mobile phone hardware.
*/
#define T_M     3
/** REX test software; Intel 80386/80486 processors, running REX and MS-DOS.
*/
#define T_REX   4
/** Stretch-portable hardware.
*/
#define T_S     5
/** Beta II mobile hardware.
*/
#define T_B2    6
/** Individual Subscriber Station 1 (ISS1) Wireless Local Loop (WLL) hardware.
*/
#define T_I1    7
/** Diagnostic monitor.
*/
#define T_DM    8
/** gpsOne<sup>&reg;</sup> Gen 6w Engine-portable hardware.
*/
#define T_G     9
/** ISS2 WLL hardware.
*/
#define T_I2    10
/** Cellular Subscriber System 1 (CSS1) WLL hardware.
*/
#define T_C1    11
/** Service programming tool.
*/
#define T_SP    12
/** Third Generation Portable (TGP).
*/
#define T_T     13
/** 1900 MHz module.
*/
#define T_MD    14
/** Q phones (Q-1900, Q-800).
*/
#define T_Q     15
/** 5GP phones (QCP-1960).
*/
#define T_O     16
/** Q phones (Q-1900, Q-800).
*/
#define T_Q2    17             

/** @cond
*/
/** This is deprecated - force to always be T_G
*/
#define TG T_G
/** @endcond */

/* Backward compatible migration, needs to be eliminated completely
*/
#undef T_AUTH
#include "coreapi_variation.h"
#ifdef FEATURE_AUTH
   #define T_AUTH
#endif



/** @} */ /* eng_name_group Target Macros */
/** @} */ /* end_addtogroup utils_services */
#endif /* TARGET_H */
