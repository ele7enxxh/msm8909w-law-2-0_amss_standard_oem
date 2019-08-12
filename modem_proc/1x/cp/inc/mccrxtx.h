#ifndef MCCRXTX_H
#define MCCRXTX_H
/*===========================================================================

                   R X T X   H E A D E R    F I L E

DESCRIPTION
  This file contains all of the definitions necessary to interface between
  the Main Control CDMA subtask and the layer 2 (RXTX) task.

  Copyright (c) 1991 - 2013 Qualcomm Technologies, Inc. 
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

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccrxtx.h_v   1.1.2.0   30 Nov 2001 17:15:50   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mccrxtx.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/15/12   ppr     FR#2370 - 1x memory optimization changes
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
11/18/10   jtm     Removed private file inclusion.
11/15/10   jtm     Initial SU API split for CP.
08/02/09   jtm     Lint fixes.
01/20/06   fc      Merged the following :
           fh      Added support for AKA and MI.
07/29/05   fc      Changed mccrxtx_msg_type to include payload
                   page_match_counter_incremented.
02/26/01   ph      added new member to msg structure for rel A to support
                   the case where parser fails to decrypt (Enh. Encr)
09/04/98   pms     Neared the variables to save ROM space.
07/01/92   jai     Create file.

===========================================================================*/

#include "comdef.h"
#include "rex.h"
#include "qw.h"
#include "queue.h"
#include "caii.h"
#include "caii_v.h"
#include "caii_i.h"
#include "cmd.h"
#include "caix.h"
#include "mc_v.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Commands to be sent from RXTX to MCC */
typedef enum
{
  MCCRXTX_MSG_F,             /* received message */
  MCCRXTX_ACK_TMO_F,         /* Acknowledgement time-out */
  MCCRXTX_MI_AES_FAIL_RPT_F  /* MI failure report */
} mccrxtx_name_type;

/* Return statuses from MCC */
typedef enum
{
  MCCRXTX_DONE_S,     /* Command processed successfully */
  MCCRXTX_BUSY_S,     /* Command is being processed */
  MCCRXTX_BADCMD_S,   /* Invalid command */
  MCCRXTX_BADPARM_S,  /* Bad Parameters */
  MCCRXTX_BADSEQ_S,   /* Command is out of sequence */
  MCCRXTX_MAX_S
}mccrxtx_status_type;

/* Header for all commands */
typedef struct
{
  cmd_hdr_type        cmd_hdr;      /* command header */
  mccrxtx_name_type   command;      /* The command */
  mccrxtx_status_type  status;      /* Command status */
} mccrxtx_hdr_type;

/* Type for MCCRXTX_MSG_F command */
typedef struct
{
  mccrxtx_hdr_type  hdr;     /* header */
  caix_chan_type    chn;     /* Channel message was received on */
  union
  {
    byte            sframe;  /* Subframe message was received in (only
                                applicable for Sync Channel messages) */
    qword           frame;
  } frm;
  boolean           page_match_counter_incremented;
    /* Indicates whether MUX page matched counter has incremented */
  caii_rx_msg_type  msg;    /* translated message */
  boolean           parser_decryption_failure; /* Parser failed to decrypt msg, used by MC */
} mccrxtx_msg_type;

/* Type for MCCRXTX_ACK_TMO_F command */
typedef struct
{
  mccrxtx_hdr_type hdr;      /* command header */
}mccrxtx_ack_tmo_type;

/* Type for MCCRXTX_MI_AES_FAIL_RPT_F command */
typedef struct
{
  mccrxtx_hdr_type hdr;      /* command header */
  word action_type;          /* Action to take upon MI failure */
} mccrxtx_mi_aes_failure_rpt_type;

/* Union of commands */
typedef union
{
  mccrxtx_hdr_type hdr;
  mccrxtx_msg_type msg;
  mccrxtx_ack_tmo_type ack_tmo;
  mccrxtx_mi_aes_failure_rpt_type mi_aes_rpt;
}mccrxtx_cmd_type;

extern q_type mcc_rxtx_q;
extern q_type mcc_rxtx_ack_q;

#ifndef FEATURE_1X_CP_MEM_OPT
extern q_type mcc_rxtx_free_q;
#endif /* !FEATURE_1X_CP_MEM_OPT */


/*===========================================================================

FUNCTION MCCRXTX_CMD

DESCRIPTION
  This function sends a command to the mcc subtask from the rxtx task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  mccrxtx_cmd_type is placed on the mcc_rxtx_q.

===========================================================================*/

extern void mccrxtx_cmd
(
  mccrxtx_cmd_type  *cmd_ptr   /* Pointer to MCCRXTX message buffer */
);


/*===========================================================================

FUNCTION MCCRXTX_INIT

DESCRIPTION
  This function initializes the queues between the Layer 2 task and the CDMA
  Main Control task.  It also places buffers on the free queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mccrxtx_init( void );

#endif /* MCCRXTX_H */
