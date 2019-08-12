#ifndef ECALL_MSD_H_
#define ECALL_MSD_H_
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               E C A L L _ M S D 
                            H E A D E R   F I L E

GENERAL DESCRIPTION 
This file contains the APIs to construct and print the Ecall MSD structure

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2008 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/hlap/src/ecall_msd.h#1 $ 
  $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/20/09   PA      Initial version.

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_ECALL_APP

#include "ecall_msd_defines.h"
#include "ecall_defines.h"
#include "ecall_modem_apis.h"

/*===========================================================================/
FUNCTION ECALL_CONSTRUCT_MSD_STRUCT

DESCRIPTION
  This function add the GPS fix position values, if any
  and hard codes the rest of the Ecall MSD Structure

DEPENDENCIES
   None

 PARAMETERS

RETURN VALUE
  None

SIDE EFFECTS

==============================================================================*/

void ecall_construct_msd_struct
(
  ecall_msd_structure_s_type *ecall_msd
);

/*===========================================================================/
FUNCTION ECALL_SERIALISE_MSD

DESCRIPTION
  This function serializes the MSD structure into a byte array

DEPENDENCIES
   None

 PARAMETERS

RETURN VALUE
  None

SIDE EFFECTS

==============================================================================*/
void ecall_serialise_msd
(
  const ecall_msd_structure_s_type *msd_struct,
  ecall_msd_array_type msd_byte_array
);

#else

void ecall_msd_dummy_func(void);

#endif //End Featurisation

#endif /* ECALL_MSD_H_ */
