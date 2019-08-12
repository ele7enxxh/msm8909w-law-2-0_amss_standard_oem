/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          M A I N    D I F F I E - H E L M A N (D H)   T A S K

GENERAL DESCRIPTION
  This module handles the exponentiation function required for Diffie-Helman
  key exchange specified in IS-683A. It also has a random number generator
  that generates a 'true' random number. MD5 algorithm is used for hashing
  and the seed for MD5 is obtained from the searcher task that fills a buffer
  with random bits which is the random noise from the radio receiver. The
  time taken to fill the buffer while in the traffic channel is in the order
  of 50ms and the time to generate the random number is in the order of 50ms.

LIMITATIONS
  No other task can use the exponentiation function provided by this task
  when the key exchange is in progress

  Copyright (c) 1998 - 2014 Qualcomm Technologies, Inc. 
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

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/dh.c_v   1.1   01 Aug 2002 11:15:14   sjaikuma  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/dh.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/06/14   gga     1X_CP F3 redistribution
12/17/13   gga     Added fix to remove waiting on SRCH signals.
12/05/13   gga     Using security specficy API instead of MD5 hash algorithm 
                   to generate the random number.
07/23/13   srk     Use TCB_PTR MACRO instead of rex_self() or tcb_ptr to make
                   the code compatible with RCINIT.
03/07/13   abd     Fix MSVC compilor errors
03/05/13   srk     Replace existing watchdog implementation with the new
                   Dog Heartbeat Mechanism.
01/03/13   nee     Replacement of memcpy() to memscpy()
12/28/12   gga     Klocwork Reviews
11/07/12   cjb     Mainlining FEATURE_MODEM_1X_SUPPORTS_RCINIT feature.
10/04/12   srk     Fix compiler warnings.
09/15/12   ppr     FR#2370 - 1x memory optimization changes
03/07/12   srk     Replaced MODEM_LEGACY_STARTED with RCINIT_RCEVT_INITGROUPS.
02/15/12   srk     Fixed compiler warnings.
02/04/12   ppr     Feature Cleanup
01/06/12   ppr     1X CP - Feature clean up: Mainline Always On features,
                   Remove Always Off Features
12/20/11   srk     Added RCInit Framework.
12/13/11   srk     Feature clean up: Mainlined FEATURE_1X_WDOG_DYNAMIC
04/13/11   ag      Renamed type WORD to APWRX_WORD.
01/13/10   ssh     Included the modem_1x_defs.h file explicitly for dynamic
                   watchdog support.
12/17/10   ssh     Changed the watchdog timout value for DH task to
                   DOG_DEFAULT_TIMEOUT.
11/02/10   ag      Added support for dynamic watchdog.
03/04/10   jj      Replaced deprecated task_xxx APIs with tmc_xxx APIs.
05/05/09   jj      CMI P-II changes
03/19/09   jtm     Lint fixes.
03/04/09   adw     Added dh_v.h include.
12/19/08   jtm     commended out ifdef for include to apwrx86.h so this file
                   compiles for the off target build. (Orphaned File)
05/11/07   pg      More Lint Cleanup.
12/10/04   sb      Fixed lint issues.
10/20/04   an      Replaced inclusion of srch.h with srchmc.h
09/23/04   fc      Fixed lint errors.
11/20/02   va      Fixed a compiler warning.
07/25/02   jqi     Updated with task operation interface.
01/20/99   ck      Ifdefed the include file and the call to function apwrx
                   based on the compiler
11/19/98   ck      Changed the dh_wait function's wait mask to include
                   OFFLINE_SIG and STOP_SIG.
10/09/98   ck      Added OFFLINE_SIG and STOP_SIG to the wait mask while
                   waiting for signal from Search to enter the main routine.
                   Changed the MD5 hash to hash on equal sizes of the buffer.
08/25/98   ck      Featurised the Exponentiation module and the interface to
                   this task under FEATURE_DH_EXP
06/25/98   ck      Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "1x_variation.h"
#include "target.h"
#include "modem_1x_defs.h"
#include "dh_v.h"

#include "rcinit.h"

#include <string.h>
#include "rex.h"
#include "task.h"
#include "dog.h"
#include "dog_hb_rex.h"
#include "msg.h"
#include "m1x_diag.h"
#include "err.h"
#include "mc_v.h"
#include "cai_v.h"
#include "dh_v.h"
#include "srchmc_i.h"
#include "srchmc_v.h"
#include "secapi.h"
#include "stringl.h"


#include "queue.h"
//joshuam #ifdef T_ARM
#include "apwrx.h"
//joshuam #else
//joshuam #include "apwrx86.h"
//joshuam #endif /* T_ARM */


/*===========================================================================
             DEFINITIONS AND DATA DECLARATIONS FOR MODULE

      This section contains local definitions for constants, macros, types,
      variables and other items needed by this module.
===========================================================================*/

/* DOG report ID for DH task */
dog_report_type dh_dog_rpt_id = 0;

#ifndef FEATURE_1X_CP_MEM_OPT
/* Buffers to be placed on the auth_free_q */
#define                    DH_NUM_CMD_BUFS 3

dh_cmd_type dh_cmd_pool[ DH_NUM_CMD_BUFS ];

/* Queue to hold free buffers to be placed on the dh_cmd_q */
q_type          dh_cmd_free_q;
#endif /* !FEATURE_1X_CP_MEM_OPT */

/* Queue to hold commands for dh */
q_type          dh_cmd_q;

/* Buffer to hold report to be returned to task issuing command */
dh_rpt_type dh_rpt_buf;

/* Buffer for command to Search task */
srch_cmd_type   dh_srch_buf;

/* stores the 160 byte true random number generated */
byte dh_rand_number[DH_RAND];

/* Create TCB pointer */
rex_tcb_type* dh_tcb_ptr = NULL;


/*===========================================================================

FUNCTION DH_PROCESS_COMMAND

DESCRIPTION
  This procedure processes a command sent to the DH task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the command is DH_EXP_F, around 12 seconds of CPU time will be taken
  to do this big calculation.

===========================================================================*/

LOCAL void dh_process_command
(
  dh_cmd_type *cmd_ptr
    /* Pointer to received command */
)
{
   int i;
   dword apwrx[24]; /* considering a maximum of 768 bit prime */
   memset(apwrx, 0, sizeof(apwrx));

   switch(cmd_ptr->hdr.command)
   {
      case DH_EXP_F:

           M1X_MSG( DCP, LEGACY_MED,
             "Starting the exponentiation" );

#ifdef T_ARM
           (void) apwrxN(cmd_ptr->exp.prime_bits,
                  (unsigned APWRX_WORD *)cmd_ptr->exp.dh_base,
                  (unsigned APWRX_WORD *)cmd_ptr->exp.dh_exponent,
                  (unsigned APWRX_WORD *)apwrx,
                  24*sizeof(dword),
                  (unsigned APWRX_WORD *)cmd_ptr->exp.dh_prime);
#else
           (void) apwrxN(cmd_ptr->exp.prime_bits,
                  (unsigned APWRX_WORD *)cmd_ptr->exp.dh_base,
                  (unsigned APWRX_WORD *)cmd_ptr->exp.dh_exponent,
                  (unsigned APWRX_WORD *)apwrx,
                  24*sizeof(dword),
                  (unsigned APWRX_WORD *)cmd_ptr->exp.dh_prime);
#endif

           M1X_MSG( DCP, LEGACY_MED,
             "Finished the exponentiation" );

           /* --------------------------------------------
           ** Report back the result of the exponentiation
           ** ----------------------------------------- */
           if (cmd_ptr->exp.rpt_function != NULL)
           {
               dh_rpt_buf.rpt_type = DH_EXP_R;

               /* Fill in necessary fields of command block */
               dh_rpt_buf.rpt_hdr.task_ptr = NULL;

               /* copy the result into an array of 96 dwords */

               memset(dh_rpt_buf.rpt.exp_result, 0, DH_RESULT);
               for (i = 0 ; i < (cmd_ptr->exp.prime_bits/(int)(sizeof(dword) * 8)) ; i++) /*lint !e573 !e574 !e737*/
                  dh_rpt_buf.rpt.exp_result[i] = apwrx[i];

               /* Queue report for requesting task */
               (*(cmd_ptr->exp.rpt_function))( &dh_rpt_buf );
               M1X_MSG( DCP, LEGACY_MED,
                 "Reporting result of exponentiation" );
           }

      break;

      default:
         M1X_MSG( DCP, LEGACY_ERROR,
           "Invalid command received by DH");
      break;
   }

}  /* dh_process_command */


/*===========================================================================

FUNCTION DH_INIT

DESCRIPTION
  This procedure initializes the queues and timers for DH Task.
  It should be called only once, at powerup time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dh_init( void )
{
#ifndef FEATURE_1X_CP_MEM_OPT
  byte i; /* Index into free buffer pool */
#endif /* !FEATURE_1X_CP_MEM_OPT */

/*- - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - -*/

  /* -----------------
  ** Initialize queues
  ** ----------------- */
  (void) q_init( &dh_cmd_q );
#ifndef FEATURE_1X_CP_MEM_OPT
  (void) q_init( &dh_cmd_free_q );

  /* Fill dh_free_q */
  for (i = 0; i < DH_NUM_CMD_BUFS; i++)
  {
    dh_cmd_pool[i].hdr.cmd_hdr.done_q_ptr = &dh_cmd_free_q;
    q_put( &dh_cmd_free_q, q_link( &dh_cmd_pool[i],
           &dh_cmd_pool[i].hdr.cmd_hdr.link ) );
  }
#endif /* !FEATURE_1X_CP_MEM_OPT */
} /* dh_init() */


/*===========================================================================

FUNCTION DH_TASK_INIT

DESCRIPTION
  This function initializes the tcb pointer and other pre initializations that
  needs to be done only after TCB_PTR is updated.

DEPENDENCIES
  Must be called from "dh" task context.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dh_task_init(void)
{
  /* Update the DH TCB pointer */
  DH_TCB_PTR = rex_self();

  /* Initialize DOG report ID */
  dh_dog_rpt_id = dog_hb_register_rex( (rex_sigs_type) DH_DOG_RPT_SIG );

  /* ------------------------------------------------------------------
  ** Initially kick watchdog
  ** ------------------------------------------------------------------ */

  dh_dog_report();

} /* dh_task_init */


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
void dh_cmd (
  dh_cmd_type *cmd_ptr                    /* the command to queue up */
)
{
#ifndef FEATURE_1X_CP_MEM_OPT
  /* -------------------------------------------------------
  ** All DH commands should return to the free queue.  Set
  ** this field again here to ensure no buffers get lost.
  ** ------------------------------------------------------- */
  cmd_ptr->hdr.cmd_hdr.done_q_ptr = &dh_cmd_free_q;
#endif /* !FEATURE_1X_CP_MEM_OPT */
    /* init link */
  ( void )q_link( cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link );

  q_put( &dh_cmd_q, &cmd_ptr->hdr.cmd_hdr.link ); /* and queue it */

  (void)rex_set_sigs( DH_TCB_PTR, DH_CMD_Q_SIG );  /* signal the Dh task */

} /*dh_cmd */


/* <EJECT> */
/*===========================================================================

FUNCTION DH_WAIT

DESCRIPTION
      This function just performs a REX wait as specified by the
      caller, except that it automatically kicks the watchdog timer every
      so often while waiting. It also acknowledges the TASK_STOP_SIG and
      TASK_OFFLINE_SIG when they are set.

DEPENDENCIES
      Relies on DH_DOG_RPT_SIG being raised often enough to
      avoid watchdog alarms, so should not be called until dh_init has
      run.

RETURN VALUE
      Returns the value returned by rex_wait.

SIDE EFFECTS
      The watchdog gets kicked periodically.

===========================================================================*/

rex_sigs_type  dh_wait
(
  /* Mask of signals to wait for */
  word  sigs
)
{
  /* Return value from REX: bitmap of signals found raised */
  rex_sigs_type  rex_signals_mask;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Keep waiting and kicking the watchdog as long as the return
     value has the kick timer signal set and none of the user's
     desired signals are set. Also process offline or stop procedure
     if TASK_OFFLINE_SIG or TASK_STOP_SIG is set*/

  do
  {
    /* Wait for the caller's conditions OR our signals */
    rex_signals_mask = rex_wait( sigs | DH_DOG_RPT_SIG | TASK_STOP_SIG |
                                TASK_OFFLINE_SIG );

    if ((rex_signals_mask & TASK_STOP_SIG) != 0)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "TASK_STOP_SIG received" );
      (void) rex_clr_sigs( DH_TCB_PTR, TASK_STOP_SIG );
    }
    else if ((rex_signals_mask & TASK_OFFLINE_SIG) != 0)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "TASK_OFFLINE_SIG received" );
      (void) rex_clr_sigs( DH_TCB_PTR, TASK_OFFLINE_SIG );
    }
    else if ((rex_signals_mask & DH_DOG_RPT_SIG) != 0)
    {
      /* -----------------------------
      ** Kick watchdog and reset timer
      ** ----------------------------- */
      dh_dog_report();
    }
  } while ( (rex_signals_mask & sigs ) == 0x0000 );
  /* Keep looping as long as user's conditions are un-met */

  /* Return the rex_wait return value with user condition(s) set */
  return ( rex_signals_mask );

} /* dh_wait */


/*===========================================================================

FUNCTION GENERATE_RAND

DESCRIPTION
      This function generates a  random number of length 160 bits using the
      MD5 hash algorithm. MD5 hash outputs a message digest of length 128 bits.
      160 bit random number is obtained by 2 successive iterations of MD5.
      The random bits from the radio noise has the highest entropy and one
      byte is fed as input for every output bit that we need. New seed is
      input each time to MD5 and hence searcher is commanded to collect
      the random bits at the end of each random number generation.

DEPENDENCIES
      None.

RETURN VALUE
      None.

SIDE EFFECTS
      None.

===========================================================================*/

void generate_rand(void)
{
   /* Holds the status of genaration of random number */
   secerrno_enum_type sec_ran_status;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   M1X_MSG( DCP, LEGACY_MED,
     "Starting the random number generation");

   /* Generate the 160 bit random number */

   sec_ran_status = secapi_get_random ( SECAPI_SECURE_RANDOM,
                                       (uint8 *)&dh_rand_number,
                                       sizeof( dh_rand_number ) );

   if( sec_ran_status != E_SUCCESS )
   {
     ERR_FATAL("Secure API failed due to %d",sec_ran_status,0,0);
   }


   M1X_MSG( DCP, LEGACY_MED,
     "Finished the random number generation");

} /* generate_rand */


/*===========================================================================

FUNCTION DH_TASK

DESCRIPTION
  This procedure is the entrance procedure for the DH task.

DEPENDENCIES
  None

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None

===========================================================================*/

void dh_task
(
  dword dummy
    /* Parameter required for REX.  Tell lint to ignore it. */
    /*lint -esym(715,dummy) */
)
{

  rex_sigs_type rex_signals_mask;  /* Mask of signals returned by rex */

  dh_cmd_type *cmd_ptr = NULL;            /* Pointer to received command */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------*/
  /* Task initialization */
  /*---------------------*/
  dh_task_init();

  rcinit_handshake_startup();

  /* Generate the random number */

  generate_rand();

  for (;;)
  {
    /* Never exit this loop... */


    /* If there is already a command on our command queue,  don't wait. */
    if (q_cnt( &dh_cmd_q ) != 0)
    {
      rex_signals_mask = DH_CMD_Q_SIG;
    }
    else
    {


      rex_signals_mask =
        rex_wait( DH_DOG_RPT_SIG

                  | DH_CMD_Q_SIG | DH_ABORT_EXP_SIG

                  | TASK_STOP_SIG | DH_RAND_USED_SIG | TASK_OFFLINE_SIG );


    }


    if ((rex_signals_mask & DH_DOG_RPT_SIG) != 0)
    {
      /* -----------------------------
      ** Kick watchdog and reset timer
      ** ----------------------------- */
      dh_dog_report();
    }

    /* -------------------------------------------------------
    ** Check if powerdown command signal was set.  If set then
    ** clear signal, and process task stop procedure.
    ** ------------------------------------------------------- */
    if ((rex_signals_mask & TASK_STOP_SIG) != 0)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "TASK_STOP_SIG received" );
      (void) rex_clr_sigs( DH_TCB_PTR, TASK_STOP_SIG );

      /* we want to abort the exponentiation in progress if any
      ** as we are going to powerdown  */
      (void) rex_set_sigs( DH_TCB_PTR, DH_ABORT_EXP_SIG);


    }

    /* -----------------------------------------------------
    ** Check if offline command signal was set.  If set then
    ** clear signal, and process task offline procedure.
    ** ----------------------------------------------------- */
    if ((rex_signals_mask & TASK_OFFLINE_SIG) != 0)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "TASK_OFFLINE_SIG received" );
      (void) rex_clr_sigs( DH_TCB_PTR, TASK_OFFLINE_SIG );
    }

    if ((rex_signals_mask & DH_RAND_USED_SIG) != 0)
    {
      M1X_MSG( DCP, LEGACY_MED,
        " RAND_USED_SIG received");
      (void) rex_clr_sigs( DH_TCB_PTR, DH_RAND_USED_SIG );

      generate_rand();
    }



    /* we received an abort signal for exponentiation, but we are
    ** not doing any, so clear it */
    /* this signal must be processed before processing the CMD_Q_SIG */
    if ((rex_signals_mask & DH_ABORT_EXP_SIG) != 0)
    {
       (void) rex_clr_sigs( DH_TCB_PTR,  DH_ABORT_EXP_SIG );
    }

    /*-----------------------------
    ** We have a command to process
    ** ---------------------------- */
    if ((rex_signals_mask & DH_CMD_Q_SIG) != 0)
    {
      (void) rex_clr_sigs( DH_TCB_PTR, DH_CMD_Q_SIG );
      if ((cmd_ptr = (dh_cmd_type*) q_get( &dh_cmd_q )) != NULL)
      {
        dh_process_command( cmd_ptr );
#ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( cmd_ptr, MC_DH_CMD_Q );
#else /* !FEATURE_1X_CP_MEM_OPT */
        /* return buffer to free queue */
        if( cmd_ptr->hdr.cmd_hdr.done_q_ptr != NULL )
        {
          /* place command on requested queue */
          q_put( cmd_ptr->hdr.cmd_hdr.done_q_ptr, &cmd_ptr->hdr.cmd_hdr.link );
        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "done_q_ptr was NULL!");
        }
#endif /* FEATURE_1X_CP_MEM_OPT */
      }
    } /* end if ((rex_signals_mask & DH_CMD_Q_SIG) != 0) */

  } /* end for (;;) */

} /* end dh_task */
/*===========================================================================

FUNCTION GET_RAND_FROM_DH

DESCRIPTION
      This function returns a random number that is generated and stored in
      DH. Successive Calls to this function should not be made in less than
      100ms. This is a limitation and can be reduced by reducing the number
      of bytes of input seed to the generator.

DEPENDENCIES
      None.

RETURN VALUE
      A pointer to the random number.

SIDE EFFECTS
      None.

===========================================================================*/

byte* get_rand_from_dh(void)
{
  static byte * random_number = NULL;

   random_number = dh_rand_number;

   /* The RAND_USED_SIG is set to indicate that the random number has been
   ** retrieved and a new number needs to be generated */
   (void) rex_set_sigs(DH_TCB_PTR, DH_RAND_USED_SIG);

   return(random_number);
}

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
void dh_dog_report( void )
{
  dog_hb_report(dh_dog_rpt_id);
  (void) rex_clr_sigs( DH_TCB_PTR, DH_DOG_RPT_SIG );

} /* dh_dog_report */
