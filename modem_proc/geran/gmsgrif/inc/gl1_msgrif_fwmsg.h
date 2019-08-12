#ifndef GL1_MSGRIF_FWMSG_H
#define GL1_MSGRIF_FWMSG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  GL1 Msgr Interface
                  FW Messages Header File

GENERAL DESCRIPTION
   This header file contains declarations relevant to L1 task processing.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmsgrif/inc/gl1_msgrif_fwmsg.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   -------- ---------------------------------------------
07/04/14   cja     CR644445 Mainline feature FEATURE_GSM_ASYNC_FW_INTF
14/10/13   pa      CR536820: Added GL1 Msgr If Task. Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Alphabetical Order */
#include "geran_variation.h"
#include "comdef.h"

#include "geran_dual_sim.h"
/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                         FUNCTION DECLARATIONS

===========================================================================*/
void gl1_msgrif_handle_fw_async_msgr_msgs(gas_id_t gas_id);

#endif /* GL1_MSGRIF_FWMSG_H */
