#ifndef MCNVDF_H
#define MCNVDF_H
/*==========================================================================

               M C   N V   D E F A U L T   S E R V I C E

DESCRIPTION
  This header file contains the interface definitions for the MC NV
  default values service.

  Copyright (c) 2000 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mcnvdf.h_v   1.0.2.0   30 Nov 2001 17:32:20   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mcnvdf.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/09/00   ts      Created file.  Needed a service to return default values for
                   NV items that reside on the RUIM.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "nv.h"

/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION MC_SET_ESN_FOR_DEFAULT

DESCRIPTION
  This function sets the static variable - esn - for use in deriving default
  values of some NV items processed by this service.

DEPENDENCIES
  This function must be called with the appropriate esn value before using
  the other functions.

RETURN VALUE
  None

SIDE EFFECTS
  The esn value will be set appropriately after this function is executed.

===========================================================================*/
extern void mc_set_esn_for_default
(
  dword esn_value
);

/*===========================================================================

FUNCTION MC_DEFAULT_VALUES

DESCRIPTION
  This function returns default values for the NV items that reside on
  the RUIM.  This function is used to provide default values in the
  case the RUIM data is not accessible.

DEPENDENCIES
  This function uses nvruim_default_imsi_s1 and nvruim_default_imsi_t_s1
  which use the static - esn variable.  This variable must be set by
  nvruim_set_esn_for_default before this function is used.


RETURN VALUE
  Return default values for the NV access.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mc_default_values
(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to data buffer for item */
);


#endif /* MCNVDF_H */
