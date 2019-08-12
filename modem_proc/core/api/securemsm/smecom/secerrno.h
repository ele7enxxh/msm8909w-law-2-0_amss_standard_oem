#ifndef SECERRNO_H
#define SECERRNO_H

/**
   @file secerrno.h

   @brief 
   Security services error number definitions.

   This module contains the definition of the Security Services return codes 
   (error numbers). Functions using this definition return an error code
   to indicate the status of execution.

   Externalized Functions:
   None.

   Initialization and sequencing requirements:
   None.
*/

/*===========================================================================
            A S W   S E C U R I T Y   S E R V I C E S
         E R R O R   N U M B E R   D E F I N I T I O N S

Copyright (c) 2000-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/COMMON/vcs/secerrno.h_v   1.9   30 Aug 2003 20:19:56   omichael  $
  $Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/secerrno.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ 
  $Author: mplcsds1 $


when       who     what, where, why
--------   ---     -----------------------------------------------------------------------
01/17/11   leo     (Tech Pubs) Edited Doxygen comments and markup.
12/30/10   nk      Doxygenated.
10/19/10   nk      moved this from crypto to smecom module
09/09/09   bm      removed FEATURE_SEC for CMI Compliance
06/10/09   ejt     removed PLATFORM_LTK around #include "IxErrno.h"
06/01/05   rv      undefined 'errno' & '_errno' defined by VS .NET for sirius LTK builds.
06/10/02   om      Changed secerrno_enum_type to be typedef'd.
05/07/01   yll     Added certificate parsing and verification related errno.
10/23/00   om      Created Module

==========================================================================================*/

/*===========================================================================

                INCLUDE FILES FOR MODULE

===========================================================================*/

#include "IxErrno.h"

/** @addtogroup SMECOMUTIL_COMDEF 
@{ 
*/

/*===========================================================================

          DEFINITIONS AND CONSTANTS FOR ERROR CODES

===========================================================================*/

/**
   Maps the SEC error number type to the common error number.
*/
typedef errno_enum_type secerrno_enum_type;

/**
   Error number callback function pointer type.
*/
typedef void (*secerrno_cb_fptr_type)( void*, secerrno_enum_type );


/** @} */  /* end_addtogroup SMECOMUTIL_COMDEF */

#endif /* SECERRNO_H */
