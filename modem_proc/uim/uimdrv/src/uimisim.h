#ifndef UIMISIM_H
#define UIMISIM_H
/*===========================================================================

           I N T E R N A L   U S I M    S T A T E   M A C H I N E

                      D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the ISIM state machine.

Copyright (c) 2001, 2011-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uimisim.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/26/13   ak      Remove unnecessary feature flags
08/13/31   js      UIMDRV Memory De-Couple changes
07/12/13   nmb     Global Reorganization
12/05/12   js      UIM Parallel Processing changes
08/13/12   js      Do SELECT on ISIM ADF before Auth
05/13/11   nmb     Added support for ISIM GBA Authentication
04/03/06   pv      Lint Fixes
05/16/05   pv      Created module.  This module contains the isim state
                   machine definitions.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "target.h"
#include "comdef.h"

/* Maximum number of states transitioned while processing a ISIM command */
#define UIM_MAX_NUMBER_ISIM_STATES  3

#define UIM_ISIM_AUTN_SPECIFIC_REF_DATA_P2 0x80

#define UIM_ISIM_AUTN_REF_DATA_NUMBER_MASK 0x7

typedef enum {
  UIM_ISIM_AUTHENTICATE_ST,              /* Authenticate state */
  UIM_ISIM_DONE_ST                       /* Done State */
} uim_isim_state_type;


/*===========================================================================

FUNCTION UIM_SEND_ISIM_ERR_RPT

DESCRIPTION
  This function sends an error report for a isim command that is being
  requested. We pass cmd_ptr explicitly here since we may be sending err
  report for am unprocessed command that was present in command queue
  while processing hotswap card removal or card error.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
void uim_send_isim_err_rpt
(
  uim_cmd_type             *cmd_ptr,
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_PROCESS_ISIM_COMMAND

DESCRIPTION
  This procedure processes a command sent to the UIM task. It sets the UIM
  state variable and calls the UIM state machine to process the command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  It sets the UIM state variable amongst others.

===========================================================================*/
boolean uim_process_isim_command
(
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_ISIM_COMMAND_RESPONSE

DESCRIPTION
  This procedure processes the response to a isim comamnd that has been
  recd from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  Status which is of apdu_status_type

SIDE EFFECTS
  Changes the UIM state variable uim_isim_state.

===========================================================================*/

uim_cmd_status_type uim_isim_command_response
(
  uim_rsp_buf_type const   *rsp_ptr,
  uim_instance_global_type *uim_ptr
);
#endif /* UIMISIM_H */
