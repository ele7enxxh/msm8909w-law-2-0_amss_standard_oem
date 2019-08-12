#ifndef _DS_QMI_VOICE_H
#define _DS_QMI_VOICE_H
/*===========================================================================

                         D S _ Q M I _ V O I C E . H

DESCRIPTION



EXTERNALIZED FUNCTIONS



Copyright (c) 2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_voice.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/25/09    vs     Initial revision.
===========================================================================*/

#include "comdef.h"
#include "customer.h"



/*===========================================================================
  FUNCTION QMI_VOICE_INIT()

  DESCRIPTION
    Register the VOICE service with QMUX for all applicable QMI links

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void  qmi_voice_init
(
  void
);

/*===========================================================================
  FUNCTION QMI_VOICE_HAS_VALID_CLIENT()

  DESCRIPTION
    Check if QMI-Voice has a valid client

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_voice_has_valid_client
(
  void
);


#endif /* _DS_QMI_VOICE_H */


