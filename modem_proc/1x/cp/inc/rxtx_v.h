#ifndef RXTX_V_H
#define RXTX_V_H
/*===========================================================================

                   R X T X   H E A D E R    F I L E

DESCRIPTION
  This file contains all of the definitions necessary to interface with
  the rxtx (layer 2) task.

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
/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/rxtx.h_v   1.2.2.0   30 Nov 2001 17:48:54   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/rxtx_v.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/09/16   ppr     1x2GTA Feature changes
03/05/13   srk     Replace existing watchdog implementation with the new
                   Dog Heartbeat Mechanism.
11/07/12   cjb     Mainlining FEATURE_MODEM_1X_SUPPORTS_RCINIT feature.
09/15/12   ppr     FR#2370 - 1x memory optimization changes
02/15/12   srk     Feature Cleanup: Removed FEATURE_GPS_MODE
02/04/12   ppr     Feature Cleanup
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
11/15/10   jtm     Initial SU API split for CP.
08/02/09   jtm     Lint fixes.
03/09/06   fc      Mainlined FEATURE_IS2000.
01/20/06   fc      Merged the following :
           fh      Added support for AKA and MI.
10/29/04   pg      Added support for bypass 2 good frames at TCI state.
03/02/01   va      Merged the following from the common archive:
  02/26/01 mai     Changed FEATURE_GPSONE to FEATURE_GPS_MODE to move code needed
                   only for GPS part of position location to a new #define.
02/28/01   lh      Changed length field from word to dword.
10/23/00   yll     Added RXTX_REJECT_S to reject the MC's START_GPS_F request
06/19/00   yll     Added support for GPS: RXTX_START_GPS_F
03/06/00   ry      Added IS2000 featurization
11/15/99   va      Added new interface for HO RXTX_HO_F with reset_l2 and
                   encrypt mode as payload.
10/02/98   pms     Backed out nearing of variables
10/02/98   pms     Neared variables to save ROM space
08/03/95   dna     Support for message encryption
12/22/93   jca     Modified for IS-95 Phase 1 upgrade.
07/23/92   jai     Modified file for LINT and linking.
07/09/92   jai     First DMSS release
09/11/91   jai     Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"
#include "modem_1x_defs.h"
#include "rex.h"
#include "queue.h"
#include "caii.h"
#include "caii_i.h"
#include "cmd.h"
#include "rxtx.h"
#include "caix.h"
#include "caix_i.h"
#include "mc_v.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* TCB for RXTX task */
extern rex_tcb_type* rxtx_tcb_ptr;

/* RCInit version */
#define RXTX_TCB_PTR ( rxtx_tcb_ptr )

/* Signal definitions */
#define  RXTX_DOG_RPT_SIG  0x0001
/* This signal is for DOG Heartbeat Mechanism for RXTX task */

#define  RXTX_RX_Q_SIG  0x0002
  /* This signal is set by the Receive task when an item is place on
     the rxtx_rx_q */

#define  RXTX_MC_Q_SIG  0x0004
  /* This signal is set when an item is placed on the rxtx_mc_q */

#define  RXTX_FRAME_SIG 0x0008
  /* This signal is set by RX every frame during Paging and Traffic
     Channel processing */

#define RXTX_TX_CMD_SIG 0x0010
  /* This signal is set by the TX task after processing a commmand from
     the RXTX task. */

/* Queue definitions */
extern q_type          rxtx_rx_q;
extern q_type          rxtx_mc_q;

#ifdef FEATURE_1X_CP_MEM_OPT
/* Enumeration of RXTX queue type */
typedef enum
{
  RXTX_NO_Q,             /* represents buffer meant not for any queue pool. Keep it 0 */
  RXTX_BUF_Q,            /* represents rxtx_buf_q */
  RXTX_MC_Q,             /* represents rxtx_mc_q */
  RXTX_RX_Q,             /* represents rxtx_rx_q */
  RXTX_MAX_Q,            /* Max number of queues for RXTX*/
  RXTX_UNKNOWN_Q         /* represents no queue */
} rxtx_queue_type;

/* Struct for holding the statistics of the RXTX queues */
typedef struct
{
  word allocated_buf_cnt;  /* Count of the allocated bufs so far */
} rxtx_queue_info_type;

/* Array holding RXTX queues statistics */
extern rxtx_queue_info_type rxtx_queue_info[RXTX_MAX_Q];

#define RXTX_FREE_QUEUE_BUF(ptr, queue_type)          \
         do                                           \
         {                                            \
           rxtx_free_queue_buf(ptr, queue_type);      \
           ptr = NULL;                                \
         } while (0);

#else /* !FEATURE_1X_CP_MEM_OPT */
extern q_type          rxtx_rx_free_q;
extern q_type          rxtx_mc_free_q;
#endif /* FEATURE_1X_CP_MEM_OPT */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Definitions for communication between MC and RXTX                       */

/* Commands to be sent from MC to RXTX */
typedef enum
{
  RXTX_START_F,      /* Return to Startup state processing */
  RXTX_TC_F,         /* Process Traffic channel messages */
  RXTX_TC_MSG_F,     /* Send a Traffic channel message */
  RXTX_STOP_TX_F,    /* Perform actions necessary with transmitter off */
  RXTX_START_TX_F,   /* Perform actions necessary with transmitter on */
  RXTX_ENCRYPT_MODE_F, /* Set the encrypt mode */
  RXTX_HO_F,         /* Perform acknowledgement procedures reset */
  RXTX_MAX_F         /* Number of rxtx cmd name types. must be last entry */
} rxtx_cmd_name_type;

/* Return statuses from RXTX */
typedef enum
{
  RXTX_DONE_S,     /* Command processed successfully */
  RXTX_BUSY_S,     /* Command is being processed */
  RXTX_BADCMD_S,   /* Invalid command */
  RXTX_BADPARM_S,  /* Bad Parameters */
  RXTX_BADSEQ_S,   /* Command is out of sequence */
  RXTX_MAX_S
} rxtx_status_type;

/* Header for all commands */
typedef struct
{
  cmd_hdr_type       cmd_hdr;      /* command header */
#ifdef FEATURE_1X_CP_MEM_OPT
  rxtx_queue_type    queue;        /* queue in which the command will go */
#endif /* FEATURE_1X_CP_MEM_OPT */
  rxtx_cmd_name_type command;      /* The command */
  rxtx_status_type    status;      /* Space for message status */
} rxtx_hdr_type;

/* Type for RXTX_START_F command */
typedef struct
{
  rxtx_hdr_type hdr;
} rxtx_start_type;

/* Type for RXTX_TC_F command */
typedef struct
{
  rxtx_hdr_type hdr;
} rxtx_tc_type;

/* Type for RXTX_TC_MSG_F command */
typedef struct
{
  rxtx_hdr_type hdr;
  caii_tx_msg_type msg;
  boolean ack_req;
    /* Indicator of whether message must be acknowledged */
} rxtx_tc_msg_type;

/* Type for RX_AC_MSG_F command */
typedef struct
{
  rxtx_hdr_type hdr;
  caii_tx_msg_type msg;
} rxtx_ac_msg_type;

/* Type for RX_STOP_TX_F command */
typedef struct
{
  rxtx_hdr_type hdr;
} rxtx_stop_tx_type;

/* Type for RX_START_TX_F command */
typedef struct
{
  rxtx_hdr_type hdr;
} rxtx_start_tx_type;

/* Type for RX_ENCRYPT_MODE_F command */
typedef struct
{
  rxtx_hdr_type hdr;
  byte encrypt_mode;
} rxtx_encrypt_mode_type;

/* Type for sending parameters changed due to HO */
typedef struct
{
  rxtx_hdr_type hdr;
  boolean reset_l2;
  byte encrypt_mode;
} rxtx_ho_type;

/* Union of command types */
typedef union
{
  rxtx_hdr_type hdr;
  rxtx_start_type start;
  rxtx_tc_type tc;
  rxtx_tc_msg_type tc_msg;
  rxtx_ac_msg_type ac_msg;
  rxtx_stop_tx_type stop_tx;
  rxtx_start_tx_type start_tx;
  rxtx_encrypt_mode_type encrypt_mode;
  rxtx_ho_type ho;
} rxtx_cmd_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* Definitions for communication from RX and to RXTX */
typedef struct
{
  q_link_type      link;     /* Queue links */
  q_type  *return_q_ptr;     /* queue to place buffer on when done */
  caix_chan_type    chn;     /* Channel message was received on */
#ifdef FEATURE_IS2000_REL_A
  dword          length;     /* Rel A message can be 32K bytes long */
#else
  word           length;     /* Message size ( in bits ) */
#endif /* FEATURE_IS2000_REL_A */
  cai_rx_msg_type   msg;     /* CDMA message */
} rxtx_rx_msg_type;

/* <EJECT> */
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


/* <EJECT> */
/*===========================================================================

FUNCTION RXTX_CMD

DESCRIPTION
  This function should be called to send a command to the Layer 2 task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Item will be enqueued for Layer 2 task and signal will be set to indicate
  that the item has been enqueued.

===========================================================================*/

extern void rxtx_cmd( rxtx_cmd_type * );

#ifdef FEATURE_1X_CP_MEM_OPT
/*===========================================================================

FUNCTION RXTX_ALLOC_QUEUE_BUF

DESCRIPTION
  This function allocates the memory of size requested and zeroing of the
  allocated memory

DEPENDENCIES
  None

RETURN VALUE
  Returns the pointer to allocated memory on success otherwise returns NULL

SIDE EFFECTS
  Used Exclusively for RXTX related queues

===========================================================================*/
void* rxtx_alloc_queue_buf
(
  rxtx_queue_type queue_type
);

/*===========================================================================

FUNCTION RXTX_FREE_QUEUE_BUF

DESCRIPTION
  This function Frees the memory allocated for RXTX queues.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Used for RXTX related queues

===========================================================================*/
void rxtx_free_queue_buf
(
  void *ptr,
  /* Pointer to be freed */
  rxtx_queue_type queue_type
  /* type of queue */
);
#endif /* FEATURE_1X_CP_MEM_OPT */

/*===========================================================================

FUNCTION RXTX_TA_CHECK_FOR_PERIODIC_MSGS

DESCRIPTION
  This function will go through RxTx queues and deletes
  the periodic messages.
  This API will be called in MC context before doing Tune Away.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rxtx_ta_check_for_periodic_msgs( void );

#endif /*RXTX_V_H*/
