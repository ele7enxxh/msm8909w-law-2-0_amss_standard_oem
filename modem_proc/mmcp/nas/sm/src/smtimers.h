#ifndef SM_TIMERS_H
#define SM_TIMERS_H

/*===========================================================================
                        S M  Header File

DESCRIPTION

   Contains the definitions and enumerations used in the SM timer management.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000, 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/
 
/*===========================================================================

            EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/smtimers.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
            
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/28/05   kvk      sm_timer_status type is changed from byte to word.
11/22/03   mks      Created file.
            
===========================================================================*/


/* <EJECT> */
/*==========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

/*=========================================================================
                 DEFINITIONS AND DECLARATIONS FOR THE MODULE
===========================================================================*/


#ifdef FEATURE_DUAL_DATA
extern LOCAL word sm_timer_status_sim[MAX_AS_IDS][MAX_PDP_NUM];    /* RABM Timer Status List */
#define sm_timer_status sm_timer_status_sim[sm_as_id]
#else
extern word sm_timer_status[MAX_PDP_NUM];    /* SM Timer Status List */
#endif

/* ---------------------------------------------------------------------------
* Enumerations for SM timer status
* --------------------------------------------------------------------------*/
typedef enum
{
   SM_TIMER_STOPPED,
   SM_TIMER_PAUSED,
   SM_TIMER_ACTIVE
}sm_timer_status_T;

/* ========================================================================
**                      FUNCTION DECLARATIONS
** ====================================================================== */

/* -----------------------------------------------------------------------
** FUNCTION     : SM_INITIALIZE_TIMERS

** DESCRIPTION  : This function initializes SM timers
** ----------------------------------------------------------------------- */
extern void sm_initialize_timers ( void );

/* -----------------------------------------------------------------------
** FUNCTION     : SM_START_TIMER

** DESCRIPTION  : This function starts an SM timer with given ID
** ----------------------------------------------------------------------- */
extern void sm_start_timer( word timer_id , sm_timer_T type );

/* -----------------------------------------------------------------------
** FUNCTION     : SM_STOP_TIMER

** DESCRIPTION  : This function stops the SM timer specified by timer_id
** ----------------------------------------------------------------------- */
extern void sm_stop_timer( word timer_id ); 

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PROCESS_TIMER_EXPIRY(), defined in smtimer.c

** DESCRIPTION  : Processes the Timer Expiry Command received from REX entity.
** ----------------------------------------------------------------------- */
extern void sm_process_timer_expiry_cmd(sm_cmd_type* cmd_ptr);
/* -----------------------------------------------------------------------
** FUNCTION     : SM_TIMER_EXPIRY

** DESCRIPTION  : This function handles the QoS validation failure and 
**                initiates the PDP deactivation procedures.
** ----------------------------------------------------------------------- */
extern void sm_timer_expiry(unsigned long param);

/* -----------------------------------------------------------------------
** FUNCTION     : sm_proc_timer_expired()

** DESCRIPTION  : This function processes the expiration of a PDP timer.
** ----------------------------------------------------------------------- */
extern void sm_proc_timer_expired(word con_id, sm_timer_T timer_type);

#endif
