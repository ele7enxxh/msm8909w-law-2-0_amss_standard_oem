#ifndef UIMCDMA_H
#define UIMCDMA_H
/*===========================================================================

        E X T E R N A L   U I M  C D M A   S T A T E   M A C H I N E
                            D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the UIM cdma state machine.

Copyright (c) 2001-2007,2009-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uimcdma.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/26/13   ak      Remove unnecessary feature flags
08/13/31   js      UIMDRV Memory De-Couple changes
07/12/13   nmb     Global Reorganization
12/05/12   js      UIM Parallel Processing changes
02/06/12   av      Removed Always ON 1x feature flags
01/16/12   nmb     Removal of ALWAYS ON FEATURE_AUTH
12/23/11   shr     Removed featurization for ALWAYS ON features
02/18/10   vs      Merged CSIM functionality
07/10/07   nk      Added EUIMID support
04/03/06   pv      Lint Fixes
08/27/04   jk      Added support for HRPD AN Authentication services in R-UIM
12/19/03   jk      Added processing for R-UIM 3GPD Services.
02/20/03   ck      Removed the UIM_INSTRUCTION_CLASS defintion as it was already
                   defined in uimi.h
12/19/01   ck      Deleted the commented out definitions.
03/19/01   ts      Created module.  This module contains the cdma state
                   machine definitions.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "target.h"
#include "comdef.h"
#include "uimi.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define UIM_MAX_NUMBER_CDMA_STATES 2

#define UIM_RUIM_UIMID_SIZE        4
#define UIM_RUIM_UIMID_OFFSET      4

/* AKA Authentication related defines */
#define UIM_AKA_AUTH_LEN_OF_AUTN_LEN   0x01     /* Size in bytes */
#define UIM_AKA_AUTH_LEN_OF_RES_LEN    0x01     /* Size in bytes */
#define UIM_AKA_AUTH_VALID_SEQ_NUM     0x00
#define UIM_AKA_AUTH_INVALID_SEQ_NUM   0x01

typedef enum {
  UIM_CDMA_STORE_ESN_ME_ST,                 /* Store ESN_ME state */
  UIM_CDMA_UPDATE_SSD_ST,                   /* Update SSD State */
  UIM_CDMA_CONFIRM_SSD_ST,                  /* Confirm SSD State */
  UIM_CDMA_GENERATE_KEY_ST,                 /* Generate KEY State */
  UIM_CDMA_RUN_CAVE_ST,                     /* Run CAVE State */
  UIM_CDMA_AKA_AUTH_ST,                     /* AKA Auth State */
  UIM_CDMA_BS_CHALLENGE_ST,                 /* BS Challenge State */
  UIM_CDMA_MS_KEY_REQ_ST,                   /* MS Key Request State */
  UIM_CDMA_KEY_GEN_REQ_ST,                  /* Key Generation Request State */
  UIM_CDMA_SERV_KEY_GEN_REQ_ST,             /* Service Key Gen Request State */
  UIM_CDMA_CFG_REQ_ST,                      /* Configuration Request State */
  UIM_CDMA_DLOAD_REQ_ST,                    /* Download Request State */
  UIM_CDMA_SSPR_CFG_REQ_ST,                 /* SSPR Config Request State */
  UIM_CDMA_SSPR_DLOAD_REQ_ST,               /* SSPR Download Request State */
  UIM_CDMA_OTAPA_REQ_ST,                    /* OTAPA Request State */
  UIM_CDMA_COMMIT_ST,                       /* Commit State */
  UIM_CDMA_VALIDATE_ST,                     /* Validation State */
  UIM_CDMA_COMPUTE_IP_AUTH_ST,              /* Compute IP Auth State */
  UIM_CDMA_STORE_ESN_MEID_ME_ST,            /* Store ESN_MEID_ME state */
  UIM_CDMA_DONE_ST                          /* Done State */
} uim_cdma_state_type;


/*===========================================================================

FUNCTION UIM_SEND_CDMA_ERR_RPT

DESCRIPTION
  This function sends an error report for a CDMA command that is being
  requested. We pass cmd_ptr explicitly here since we may be sending err
  report for am unprocessed command that was present in command queue while
  processing hotswap card removal or card error.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
void uim_send_cdma_err_rpt
(
  uim_cmd_type             *cmd_ptr,
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_PROCESS_CDMA_COMMAND

DESCRIPTION
  This procedure processes a CDMA command sent to the UIM task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  It sets the UIM CDMA state variable amongst others.

===========================================================================*/
boolean uim_process_cdma_command
(
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_CDMA_COMMAND_RESPONSE

DESCRIPTION
  This procedure processes the response to a CDMA comamnd that has been
  received from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  Status which is of apdu_status_type

SIDE EFFECTS
  Changes the UIM state variable uim_cdma_state.

===========================================================================*/
uim_cmd_status_type uim_cdma_command_response
(
  uim_rsp_buf_type const   *rsp_ptr,
  uim_instance_global_type *uim_ptr
);
#endif /* UIMCDMA_H */

