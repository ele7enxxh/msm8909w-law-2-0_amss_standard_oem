/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    C D M A   S T U B   A P I S

GENERAL DESCRIPTION
  This file contains stubs for APIs exposed to other modules. These
  stubs will only be compiled and available in non-CDMA builds.
  This file should be used to provide stubs for modules not part of 
  Modem such as Audio. Modules within modem such as Multimode already
  have a mechanism to featurize the API's exposed by 1X and there is 
  no need to provide stubs for them.

  Copyright (c) 1990 - 2015 Qualcomm Technologies, Inc.
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

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/onex_stubs.c   1.8   10 Mar 2015 13:34:04   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/stub/src/onex_stubs.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
09/09/15   agh     Initial revision

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "sys.h"
#include "onex_voice_if.h"

/*===========================================================================
                             STUB APIs
===========================================================================*/

/*===========================================================================

STUB_FUNCTION ONEX_VOICE_CALL

DESCRIPTION
  STUB API for onex_voice_call function which is used by CDMA Voice Adapter 
  to communicate with 1X. The stub API will always return ONEX_EUNSUPPORTED.
  
DEPENDENCIES
  None.

RETURN VALUE
  ONEX_EUNSUPPORTED

SIDE EFFECTS
  None.

  
===========================================================================*/
uint32 onex_voice_call (uint32 cmd_id, void* params, uint32 size)
{
  return ONEX_EUNSUPPORTED;
}
