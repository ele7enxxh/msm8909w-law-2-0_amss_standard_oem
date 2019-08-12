#ifndef MCCAKA_H
#define MCCAKA_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    C D M A   M A I N   C O N T R O L

   3 G   A U T H E N T I C A T I O N    A N D   K E Y   A G R E E M E N T

GENERAL DESCRIPTION

  This is the module for 3G Authentication and Key Agreement, as well as
  the modified 2G Authentication.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2005 - 2013 Qualcomm Technologies, Inc. 
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccbcsms.h_v   1.7   27 Jun 2002 19:49:36   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mccaka.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/04/12   ppr     Feature Cleanup
07/19/10   ag      Replaced "PACKED <name>" with "PACK (<name>)".
08/09/06   fh      Added a new function mccaka_smrm_needed to check wether or
                   not an SMRM needs to be sent in AKA/AES perspective.
                   Renamed mccaka_ac_get_smrm_field to mccaka_ac_set_smrm_field.
                   Miscellaneous code changes after the code review.
05/24/06   fh      AKA/AES: misclaneous fixes within AKA/AES features
05/17/06   fh      Merged in new development of AKA/AES features
04/28/06   fc      Renamed aka and mi files.
02/10/02   yll     Create the module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"

#if defined (FEATURE_IS2000_REL_A_AES)
#include "mccdma.h"
#include "mccsec.h"
#endif /* FEATURE_IS2000_REL_A_AES */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#if defined (FEATURE_IS2000_REL_A_AES)

#if defined (FEATURE_IS2000_REL_A_AES)
#define L_KEY    16    /*128      bits 3G TS 33.105.v.3.0(2000-03)*/
#define L_CK     16    /*128      bits 3G TS 33.105.v.3.0(2000-03)*/
#define L_UAK    16
#endif /* FEATURE_IS2000_REL_A_AES */

#define MCCAKA_IS_VALID_ENC_KEY_SIZE(size)  \
        ((MCCAKA_ENC_KEY_SIZE_64 <= (size)) && ((size) <= MCCAKA_ENC_KEY_SIZE_128))

#define MCCAKA_HAS_MI_FIELDS (P_REV_IS_9_OR_GREATER)

/* Enc_key_size, defined in Table 3.7.4.5-2 of IS 2000 Rel D */
#define MCCAKA_ENC_KEY_SIZE_RESV 0
#define MCCAKA_ENC_KEY_SIZE_64   1
#define MCCAKA_ENC_KEY_SIZE_128  2

typedef uint8                 mccaka_key_type[L_KEY];

/* Command processing of Message Integrity status type. */
typedef enum
{
  MCCAKA_SEC_CMD_NO_ACTION          =   0,
  MCCAKA_SEC_CMD_COMPLETION,
  MCCAKA_SEC_CMD_UAK_NOT_SUPPORTED,
  MCCAKA_SEC_CMD_INVALID_FIELDS

} mccaka_sec_cmd_status_type;

typedef struct
{

  /* CK entropy size. If < 128 bits, needs to perform key strength reduction */
  uint8                             ck_entropy_size;

  /* MSB 24 bits of the crypto-sync. */
  uint32                            pending_sseq_h;

  /* Flag indicating a Security mode request message is sent
  ** and MS waits for the Security mode command message, due
  ** the SSEQ out of sync for MI.
  */
  boolean                           smrm_sent;

} mccaka_params_type;

#endif /* FEATURE_IS2000_REL_A_AES */

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


#if defined (FEATURE_IS2000_REL_A_AES)

/*===========================================================================

FUNCTION MCCAKA_POWERUP_LOAD

DESCRIPTION
  This functions loads the AKA related items from NV upon powerup. If some
  items are not active in NV, it assigns them the default values. It also
  initializes variables for AKA and modified 2G Authentication.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If AKA NV items are loaded successfully.
  FALSE - Otherewise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mccaka_powerup_load
(
  uint8     nam
);


/*===========================================================================

FUNCTION MCCAKA_TC_SEND_SMRM_MSG

DESCRIPTION
  This function sends the Security Mode Request Message on TC, in order to
  do the SSEQ resynchronization.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mccaka_tc_send_smrm_msg ( void );


/*===========================================================================

FUNCTION MCCAKA_PROC_FCSCH_RES

DESCRIPTION
  This function process the RES messages from the comman channel.

DEPENDENCIES
  None.

RETURN VALUE
  A RES processing status wil be returned.

SIDE EFFECTS
  None.

===========================================================================*/
extern mccaka_sec_cmd_status_type  mccaka_proc_fcsch_res
(
  const mccdma_matched_msg_type   *msg_ptr
);


/*===========================================================================

FUNCTION MCCAKA_CHECK_FDSCH_MSG_SEC_FIELDS

DESCRIPTION
  This function checks the security related fields of the messages
  received on the dedicated channel.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the security related fields are valid, FALSE if not

SIDE EFFECTS
  None.

===========================================================================*/
boolean  mccaka_check_fdsch_msg_sec_fields
(
  const mccrxtx_msg_type          *msg_ptr
);

/*===========================================================================

FUNCTION MCCAKA_PROC_FDSCH_MSG_SEC_FIELDS

DESCRIPTION
  This function processes the security related fields of the messages
  received on the dedicated channel.

DEPENDENCIES
  None.

RETURN VALUE
  A message processing status wil be returned.

SIDE EFFECTS
  None.

===========================================================================*/
extern mccaka_sec_cmd_status_type  mccaka_proc_fdsch_msg_sec_fields
(
  const mccrxtx_msg_type          *msg_ptr
);


/*===========================================================================

FUNCTION MCCAKA_PROC_HO_MSG_SEC_FIELDS

DESCRIPTION
  This function processes the security related fields of handoff messages
  received on the dedicated channel

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void  mccaka_proc_ho_msg_sec_fields
(
  const caii_ho_msg_type *msg_ptr
);

/*===========================================================================

FUNCTION MCCAKA_PROC_MI_AES_FAILURE_RPT

DESCRIPTION
  This function process the MACI or AES failure report from MI.

DEPENDENCIES
  None.

RETURN VALUE
  The next CDMA state to transit to.

SIDE EFFECTS
  None.

===========================================================================*/
extern word mccaka_proc_mi_aes_failure_rpt
(
  word              fail_status,   /* MI failure status */
  word              curr_state
);




/*===========================================================================

FUNCTION MCCAKA_TC_SEND_SEC_MODE_COMP_ORDER

DESCRIPTION
  This function sends the Security Mode Completion Order on TC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccaka_tc_send_sec_mode_comp_order ( void );


/*===========================================================================

FUNCTION MCCAKA_SMRM_NEEDED

DESCRIPTION
  This function determines if an SMRM is needed in AKA/AES perspective

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if SMRM is needed. FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccaka_smrm_needed(void);

/*===========================================================================

FUNCTION MCCAKA_AC_SET_SMRM_FIELD

DESCRIPTION
  This function sets the fields from MCCAKA for the Security Mode Request
  Message (SMRM) to be sent on the Access Channel / EACH, in order to do
  the SSEQ resynchronization.

DEPENDENCIES
  None.

RETURN VALUE
  The pointer to Security Mode Request Message with the fields filled.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mccaka_ac_set_smrm_field
(
  caii_ac_smrm_type   *smrm
);


/*===========================================================================

FUNCTION MCCAKA_UPDATE_2G_KEY

DESCRIPTION
  This function copies the CEMA keys from CAVE module, to be used as the
  pending keys for modified 2G Authentication.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern void mccaka_update_2g_key
(
  word  curr_state
);


/*===========================================================================

FUNCTION MCCAKA_SET_SMRM_SENT_FLAG

DESCRIPTION
  This function sets the smrm_sent flag of mccaka data structure


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mccaka_set_smrm_sent_flag
(
  boolean flag
);

#endif /* FEATURE_IS2000_REL_A_AES */

#endif /* MCCAKA_H */
