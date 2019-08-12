#ifndef DH_V_H
#define DH_V_H
/*===========================================================================

               E X T E R N A L   D I F F I E - H E L M A N
                      T A S K   D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the Authentication Task.

  Copyright (c) 1998 - 2013 Qualcomm Technologies, Inc. 
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

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/dh.h_v   1.0.2.0   30 Nov 2001 16:53:36   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/dh_v.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $


when       who    what, where, why
--------   ---    ----------------------------------------------------------
12/05/13   gga     Using security specficy API instead of MD5 hash algorithm 
                   to generate the random number.
03/05/13   srk     Replace existing watchdog implementation with the new
                   Dog Heartbeat Mechanism.
11/07/12   cjb     Mainlining FEATURE_MODEM_1X_SUPPORTS_RCINIT feature.
02/04/12   ppr     Feature Cleanup
01/06/12   ppr     1X CP - Feature clean up: Mainline Always On features,
                   Remove Always Off Features
11/15/10   jtm     Initial SU API split for CP.
11/02/10   ag     Added support for dynamic watchdog.
07/01/09   jj     Moved a symbol from this file to dh.h
05/14/09   jj     CMI phase-2 changes.
12/10/04   sb      Fixed lint issue.
10/20/04   an     Replaced inclusion of srch.h with srchmc.h
09/23/04   fc     Fixed lint errors.
01/20/99   ck     Removed set_a_key_temp as it was not being used and also
                  removed extern from functions generate_rand and dh_wait
08/25/98   ck     Featurised the interface routines for the exponentiation
                  module under FEATURE_DH_EXP
06/25/98   ck     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "1x_variation.h"
#include "target.h"
#include "modem_1x_defs.h"
#include "dh.h"


#include "comdef.h"
#include "dog.h"
#include "queue.h"
#include "qw.h"
#include "cmd.h"
#include "srchmc.h"
#include "srchmc_v.h"
#include "srchmc_i.h"
#include "rex.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* TCB for DH task */
extern rex_tcb_type* dh_tcb_ptr;

/* RCInit version */
#define DH_TCB_PTR ( dh_tcb_ptr )

#define DH_BASE                   64
  /* Number of bytes of Diffie-Helman dh_base  */

#define DH_PRIME                  64
  /* Number of bytes of Diffie-Helman dh_prime  */

#define DH_RESULT                 64
#define DH_EXPONENT               64
  /* Number of bytes of Diffie-Helman dh_exponent  */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*   External declarations for structures declared in mc_cdma.c            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

extern srch_cmd_type  dh_srch_buf;  /* Buffer for command to Search task */

/* DH DOG report ID and DOG report time interval. */
extern dog_report_type dh_dog_rpt_id;

/*--------------------------------------------------------------------------
                             Signal Masks
--------------------------------------------------------------------------*/
#define  DH_DOG_RPT_SIG        0x0001
  /* This signal is for DOG Heartbeat Mechanism for DH task */

#define  DH_CMD_Q_SIG          0x0002
  /* This signal is set when an item is put on the dh_cmd_q. */

#define  DH_RAND_USED_SIG      0x0004
  /* This signal is set when the random number has been retrieved from DH*/

#define  DH_SRCH_RAND_SIG      0x0008
  /* This signal is set when search has filled the a-key buffer with random bits */

#define  DH_ABORT_EXP_SIG      0x0010
  /* This signal is set when exponentiation needs to be aborted  */

/*--------------------------------------------------------------------------
                                  DH REPORT

--------------------------------------------------------------------------*/
/* -------------------------------------------------------------------
** The DH Task optionally sends a report when it completes
** a command which produces a result which would be useful to the
** task which issued the command.
** ------------------------------------------------------------------- */

typedef enum
{
  DH_EXP_R                   /* Diffie Helman exponentiation report*/
} dh_report_code_type;

typedef struct
{
  cmd_hdr_type         rpt_hdr;
    /* Includes the queue links,  pointer to requesting task TCB,
       signal to set on completion,  and done queue. */
  dh_report_code_type rpt_type; /* What type of report this is */
  union
  {
    dword               exp_result[DH_RESULT/sizeof(dword)]; /* Exponentiation result */
  } rpt;
}
dh_rpt_type;

/*--------------------------------------------------------------------------
                                 DH COMMANDS

--------------------------------------------------------------------------*/


/* Command types */
typedef enum {
  DH_EXP_F           /* Begin exponentiating */
} dh_cmd_name_type;

/* The common Header for all the commands */
typedef struct {
  cmd_hdr_type          cmd_hdr;        /* command header */
  dh_cmd_name_type      command;        /* The command */
} dh_hdr_type;

/* Begin the exponentiation */
typedef struct {                  /* DH_EXP_F */
  dh_hdr_type     hdr;
  int             prime_bits;
  dword           dh_base[DH_BASE/sizeof(dword)];
  dword           dh_exponent[DH_EXPONENT/sizeof(dword)];
  dword           dh_prime[DH_PRIME/sizeof(dword)];
  void            (*rpt_function)( dh_rpt_type * );
} dh_exp_type;

typedef union {
  dh_hdr_type                hdr;     /* Generic header */
  dh_exp_type                exp;     /* DH exponentiation */
} dh_cmd_type;


/* Queue for other tasks to get dh command buffers from */
extern q_type dh_cmd_free_q;







/* <EJECT> */
/*===========================================================================

FUNCTION DH_CMD

DESCRIPTION
  The dh_cmd pointed to by the parameter passed in is queued up for DH.

DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
extern void dh_cmd
(
  dh_cmd_type *cmd_ptr                    /* the command to queue up */
);



/*===========================================================================

FUNCTION DH_DOG_REPORT

DESCRIPTION
  This function sends report to Dog heartbeat mechanism and clears the
  dog report signal.

DEPENDENCIES
  Depends on previously initialized DOG report ID.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void dh_dog_report( void );



/*===========================================================================

FUNCTION DH_WAIT

DESCRIPTION    This function just performs a REX wait as specified by the
               caller, except that it automatically kicks the watchdog timer
               every so often while waiting.

DEPENDENCIES   Relies on DH_DOG_RPT_SIG being raised often
               enough to avoid watchdog alarms, so should not be called
               until dh_init has run.

RETURN VALUE   Returns the value returned by rex_wait.

SIDE EFFECTS   The watchdog gets kicked periodically.

===========================================================================*/

rex_sigs_type dh_wait
(
  word  sigs
    /* Mask of signals to wait for */
);

void generate_rand(void);


#endif /*DH_V_H*/
