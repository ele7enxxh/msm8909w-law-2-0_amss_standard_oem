#ifndef UIMUSIM_H
#define UIMUSIM_H
/*===========================================================================

           I N T E R N A L   U S I M    S T A T E   M A C H I N E

                      D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the USIM state machine.

Copyright (c) 2001,2006-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uimusim.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/02/14   nmb     Directory Maintenance Changes
09/26/13   ak      Remove unnecessary feature flags
08/13/31   js      UIMDRV Memory De-Couple changes
07/12/13   nmb     Global Reorganization
12/05/12   js      UIM Parallel Processing changes
01/12/09   sun     Added a new state - UIM_SELECT_ADF_ST before ODD INS
                   auth
03/03/08   sun     Added support for OMA BCAST
07/05/07   sp      Add a new state - UIM_STATUS_BEFORE_USIM_AUTH_ST to send
                   out a status command before each usim authenticate command.
                   Increase UIM_MAX_NUMBER_USIM_STATES to accommodate the
                   new UIM_STATUS_BEFORE_USIM_AUTH_ST state.
07/02/07   sun     Added support for ODD INS Class
04/03/06   pv      Lint Fixes
01/29/01   ck      Created module.  This module contains the usim state
                   machine definitions.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "target.h"
#include "comdef.h"
#include "uimi.h"

/* Maximum number of states transitioned while processing a USIM command */
#define UIM_MAX_NUMBER_USIM_STATES  3

#define UIM_AUTN_GLOBAL_REF_DATA_P2   0x00
#define UIM_AUTN_SPECIFIC_REF_DATA_P2 0x80
#define UIM_AUTH_REF_MBMS             0x05
#define UIM_AUTH_FIRST_CMD_BLOCK      0x80
#define UIM_AUTH_FIRST_RSP_BLOCK      0xA0
#define UIM_AUTH_NEXT_RSP_BLOCK       0x20
#define UIM_BCAST_SPE_SIG_MODE        0x02

#define UIM_AUTN_REF_DATA_NUMBER_MASK 0x1F

typedef enum {
  UIM_AUTHENTICATE_ST,              /* Authenticate state */
  UIM_STATUS_BEFORE_USIM_AUTH_ST,   /* Status state */
  UIM_AUTHENTICATE_ODD_INS_ST,      /* Authenticate state for ODD INS */
  UIM_AUTHENTICATE_ODD_INS_RSP_ST,  /* Authentication Response State for ODD Ins*/
  UIM_BCAST_ST,                     /* BCast State*/
  UIM_BCAST_RSP_ST,
  UIM_USIM_DONE_ST                  /* Done State */
} uim_usim_state_type;

/*===========================================================================

FUNCTION UIM_SEND_USIM_ERR_RPT

DESCRIPTION
  This function sends an error report for a usim command that is being
  requested. We pass cmd_ptr explicitly here since we may be sending
  err report for am unprocessed command that was present in command
  queue while processing hotswap card removal or card error.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
void uim_send_usim_err_rpt
(
  uim_cmd_type             *cmd_ptr,
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_PROCESS_USIM_COMMAND

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
boolean uim_process_usim_command
(
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_USIM_COMMAND_RESPONSE

DESCRIPTION
  This procedure processes the response to a usim comamnd that has been
  recd from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  Status which is of apdu_status_type

SIDE EFFECTS
  Changes the UIM state variable uim_usim_state.

===========================================================================*/
uim_cmd_status_type uim_usim_command_response
(
  uim_rsp_buf_type const   *rsp_ptr,
  uim_instance_global_type *uim_ptr
);
#endif /* UIMUSIM_H */
