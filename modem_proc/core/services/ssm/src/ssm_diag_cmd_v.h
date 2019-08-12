#ifndef SSM_DIAG_CMD_V_H
#define SSM_DIAG_CMD_V_H
/*==========================================================================

         Diagnostic Packet Definitions for DIAG SUBSYS SSM 

  Description: Packet definitions between the diagnostic subsystem
  and the external device.

  !!! NOTE: All member structures of diag packets must be PACKED.

  !!! WARNING: Each command code number is part of the externalized
  diagnostic command interface.  This number *MUST* be assigned
  by a member of QCT's tools development team.

Copyright (c) 2012 - 2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            Edit History

  $Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_diag_cmd_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/15/14   din     Added diag test command to test secure channel.
04/03/14   din     Added new apis to get mode and to write to permission file. 
                   Also added diag command to get ssm mode.
04/29/13   din     Changes pertaining to ADSP were done.
11/05/12   din     File created.

===========================================================================*/


#include "diagcmd.h"
#include "diagpkt.h"
#include "diagi.h"
#include "log_codes.h"
#include "feature.h"
#include "ssm_v.h"


/* -------------------------------------------------------------------------
** Packet Definitions
** ------------------------------------------------------------------------- */

#ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
#define SSM_COMMAND_CODE_OFFSET                       0
#elif  FEATURE_SSM_ADSP_PROC
#define SSM_COMMAND_CODE_OFFSET                       50
#elif  FEATURE_SSM_RIVA_PROC
#define SSM_COMMAND_CODE_OFFSET                       100
#endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */


#define SSM_DIAG_F                                   (0+SSM_COMMAND_CODE_OFFSET)
#define SSM_DIAG_OPEN_PERM_FILE                      (1+SSM_COMMAND_CODE_OFFSET)
#define SSM_DIAG_CLOSE_PERM_FILE                     (2+SSM_COMMAND_CODE_OFFSET)
#define SSM_DIAG_READ_PERM_FILE                      (3+SSM_COMMAND_CODE_OFFSET)
#define SSM_DIAG_WRITE_PERM_FILE                     (4+SSM_COMMAND_CODE_OFFSET)
#define SSM_DIAG_ACH_PROVISION_AUTHENTICATION_COOKIE (5+SSM_COMMAND_CODE_OFFSET)
#define SSM_DIAG_KEY_HANDLER_PROVISION_KEY           (6+SSM_COMMAND_CODE_OFFSET)
#define SSM_DIAG_KEY_HANDLER_READ_KEY                (7+SSM_COMMAND_CODE_OFFSET)
#define SSM_DIAG_OCH_REQUEST_RANDOM_NONCE            (8+SSM_COMMAND_CODE_OFFSET)
#define SSM_DIAG_OCH_REQUEST_TEMPORARY_OVERRIDE      (9+SSM_COMMAND_CODE_OFFSET)
#define SSM_DIAG_OCH_REQUEST_PERMANENT_OVERRIDE      (10+SSM_COMMAND_CODE_OFFSET)
#define SSM_DIAG_OCH_CANCEL_OVERRIDE                 (11+SSM_COMMAND_CODE_OFFSET)

/* ACH removal functionality is only intended for testing and 
 * will be featured out in CRM builds */
#define SSM_DIAG_ACH_REMOVE_AUTHENTICATION_COOKIE    (12+SSM_COMMAND_CODE_OFFSET)
/* Key removal functionality is only intended for testing and 
 * will be featured out in CRM builds */
#define SSM_DIAG_KEY_HANDLER_REMOVE_KEY              (13+SSM_COMMAND_CODE_OFFSET)

/* OCH Reset Random functionality is only intended for testing and 
 * will be featured out in CRM builds */
#define SSM_DIAG_OCH_RESET_RANDOM_NONCE              (14+SSM_COMMAND_CODE_OFFSET)

/* OCH Reset Random functionality is only intended for testing and 
 * will be featured out in CRM builds */
#define SSM_DIAG_OCH_SIGN_RANDOM_NONCE               (15+SSM_COMMAND_CODE_OFFSET)

/* SSM get mode Command */
#define SSM_DIAG_GET_MODE                            (16+SSM_COMMAND_CODE_OFFSET)

/* SSM test secure channel Command */
#define SSM_DIAG_TEST_SECURE_CHANNEL                 (17+SSM_COMMAND_CODE_OFFSET)

#endif /* SSM_DIAG_CMD_V_H */
