#ifndef NV_CNV_H
#define NV_CNV_H
/*===========================================================================

                  E X T E R N A L   C N V
              T A S K   D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the Authentication Task.

Copyright (c) 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/nv_cnv.h#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/06/05   ck     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "comdef.h"
#include "queue.h"
#include "cmd.h"

#ifdef FEATURE_NV_CNV

/**************************************************************************/
/* Format of CNV data.                                                    */
/**************************************************************************/

/*
FORMAT of CNV data
---------------------------------
typedef {
   uint8   name[4];   (CNV*)
   uint16  version;
   uint16  pad;
} nv_cnv_header_type

**** multiple instances of NV data
typedef {
  uint16     nv_item_number;     FFFF signifies last item
  uint16     nv_item_size;
} nv_cnv_item_hdr_type

{
  uint8      data[nv_item_size];  when size is non zero
  uint8      pad[0-3 bytes depending on nv_item_size];  FFs
} variable data
end multiple instance *****

FF FF 02 00 CRC(MSB) CRC(LSB)
---------------------------------

CRC used is 16 bit stepwise CRC (CRC CCITT-16)

*/

/* <EJECT> */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*   External declarations for structures declared in mc_cdma.c            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Timer definitions */
extern rex_timer_type cnv_rpt_timer;  /* Watchdog timer */

/*--------------------------------------------------------------------------
                             Signal Masks
--------------------------------------------------------------------------*/
#define  CNV_RPT_TIMER_SIG      0x0001
  /* This signal is set when the cnv_rpt_timer expires.
     This timer is used to enable CNV to kick the watchdog on time */

#define  CNV_CMD_Q_SIG          0x0002
  /* This signal is set when an item is put on the cnv_cmd_q. */

#define  CNV_NV_CMD_SIG         0x0004
  /* This signal is set when command is completed by NV. */

/*--------------------------------------------------------------------------
                                 MACROS
--------------------------------------------------------------------------*/

/* Kick the watchdog */
#define  CNV_DOG_RPT() \
  dog_report(DOG_CNV_RPT); \
  (void) rex_set_timer ( &cnv_rpt_timer, DOG_CNV_RPT_TIME )



/*--------------------------------------------------------------------------
                                  CNV REPORT

--------------------------------------------------------------------------*/
/* -------------------------------------------------------------------
** The CNV Task optionally sends a report when it completes
** a command which produces a result which would be useful to the
** task which issued the command.
** ------------------------------------------------------------------- */
typedef enum {
  CNV_BACKUP_NV_R               /* Backup NV report*/
} cnv_report_code_type;

typedef struct {
  cmd_hdr_type         rpt_hdr;
  /* Includes the queue links,  pointer to requesting task TCB,
       signal to set on completion,  and done queue. */
  cnv_report_code_type rpt_type;   /* What type of report this is */
  void                 *user_data; /* User data */
  union {
    boolean            status;     /*  Backup result */
  } rpt;
}
cnv_rpt_type;

/*--------------------------------------------------------------------------
                                 CNV COMMANDS

--------------------------------------------------------------------------*/

/* Command types */
typedef enum {
  CNV_BACKUP_NV_F           /* Begin backup */
} cnv_cmd_name_type;

/* The common Header for all the commands */
typedef struct {
  cmd_hdr_type          cmd_hdr;        /* command header */
  cnv_cmd_name_type     command;        /* The command */
  void                  *user_data;     /* User data */
  void                  (*rpt_function)( cnv_rpt_type * );
                                        /* Pointer to Function to send report */
} cnv_hdr_type;

/* Begin the back up */
typedef struct {                  /* CNV_BACKUP_F */
  cnv_hdr_type          hdr;
} cnv_backup_type;

typedef union {
  cnv_hdr_type          hdr;            /* Generic header */
  cnv_backup_type       backup;         /* Backup NV */
} cnv_cmd_type;

/* Queue for other tasks to get cnv command buffers from */
extern q_type cnv_cmd_free_q;

/**************************************************************************/
/* Information below is the CNV header.                                   */
/**************************************************************************/

typedef struct {
   byte    name[4];    /* This shall contain "CNV" */
   uint16  version;    /* This shall contain the version number of CNV file */
   uint16  pad;
} cnv_header_type;

typedef struct{
  uint16     nv_item_number;     /* FFFF signifies last item */
  uint16     nv_item_size;
} cnv_item_hdr_type;

/*===========================================================================

FUNCTION CNV_TASK

DESCRIPTION
  This procedure is the entrance procedure for the CNV task.

DEPENDENCIES
  None

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None

===========================================================================*/
void 
cnv_task (
  dword dummy
    /* Parameter required for REX.  Tell lint to ignore it. */
    /*lint -esym(715,dummy) */
);

/*===========================================================================

FUNCTION CNV_CMD

DESCRIPTION
  The cnv_cmd pointed to by the parameter passed in is queued up for CNV.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void 
cnv_cmd (
  cnv_cmd_type *cmd_ptr                    /* the command to queue up */
);
 
/*===========================================================================

FUNCTION CNV_WAIT

DESCRIPTION
  This function just performs a REX wait as specified by the caller, except
  that it automatically kicks the watchdog timer every so often while waiting.

DEPENDENCIES
  Relies on CNV_RPT_TIMER_SIG being raised often enough to avoid watchdog
  alarms, so should not be called until cnv_init has run.

RETURN VALUE
  Returns the value returned by rex_wait.

SIDE EFFECTS
  The watchdog gets kicked periodically.

===========================================================================*/
rex_sigs_type 
cnv_wait (
  word  sigs                               /* Mask of signals to wait for */
);
#endif /* FEATURE_NV_CNV*/
#endif /* NV_CNV_H */
