#ifndef DSUCSDTIMER_H
#define DSUCSDTIMER_H
/*===========================================================================

              U M T S   C S   D A T A   T I M E R S   D E C L A R A T I O N      
                             H E A D E R   F I L E

DESCRIPTION
  This header file contains declarations for timer functionality used
  by UMTS CS data.
  
Copyright (c) 2003 - 2011 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/inc/dsucsdtimer.h#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/11   SS      Q6 free floating support.
08/06/07   DA      Added TIMERID_ATCOP_SREG to enum
10/21/04   AR      Add timer TIMERID_ADM_IDLE. Make timer counter uint32.
07/15/03   TMR     Created

===========================================================================*/

/* Macro's and enum for supporting timers */

#define INIT_TIMERS()              ds_ucsd_timer_init()
#define START_TIMER(timer_id,msec) ds_ucsd_timer_start(timer_id,msec)
#define STOP_TIMER(timer_id)       ds_ucsd_timer_cancel(timer_id)
#define TIMER_EXPIRED(timer_id)    ds_ucsd_is_timer_expired((uint32)timer_id)
#define TIMER_RUNNING(timer_id)    ds_ucsd_is_timer_running((uint32)timer_id)

/* Counter-timer IDs */

enum
{
   TIMERID_T1,         /* Timer for Unnumbered and ABM retransmission */
   TIMERID_TXID,       /* Timer for XID frame retransmission */
   TIMERID_TTEST,      /* Timer for TEST frame retransmission */
   TIMERID_TRCVR,      /* Timer for REJ/SREJ condition */
   TIMERID_REMAP,      /* Timer for REMAP exchange */
   TIMERID_FIT,        /* Timer for Fax Inactivity Timeout */
   TIMERID_MO_FLUSH,   /* Timer for Uplink flush on MO call terminate */
   TIMERID_MT_FLUSH,   /* Timer for Downlink flush on MT call terminate*/
   TIMERID_ATCOP_SREG, /* Timer for UCSD data transfer inactivity control */
   DS_UCSD_MAX_TIMERS  /* Maximum number of timers */
};

/*
 * Some timers have dual uses, this is intentional and intended to keep
 * the number of counter-timers to a minimum. This is possible because
 * some of the timers are used only in a Transparent service, others only
 * in a non-transparent service. 
 *
 * NOTE: Timer ids TIMERID_MO_FLUSH and TIMERID_MT_FLUSH are used in 
 * TRANSPARENT and NON_TRANSPARENT, so cannot
 * be reused in the way that timers TIMERID_T1 to TIMERID_REMAP are above
 */

#define TIMERID_SYNC           TIMERID_T1    /* Timer for Transparent Sync Time */
#define TIMERID_CTS_INTEG_ON   TIMERID_TXID  /* CTS on integ. timer 07.01 8.2.2 */
#define TIMERID_DCD_INTEG_ON   TIMERID_TTEST  /* DCD on integ. timer 07.01 8.2.2 */
#define TIMERID_CTS_INTEG_OFF  TIMERID_TRCVR  /* CTS off integ. timer 07.01 8.2.2 */
#define TIMERID_DCD_INTEG_OFF  TIMERID_REMAP  /* DCD off integ. timer 07.01 8.2.2 */
#define TIMERID_NT_INIT_DELAY  TIMERID_FIT
#define TIMERID_ADM_IDLE       TIMERID_FIT

 

/* Values of timers ...  */
#define TIMER_COUNT_1_SEC        1000
#define TIMER_COUNT_5_SEC        5000
#define TIMER_COUNT_200_MSEC     200


#define DS_GCSD_TIMER_MSEC_PER_TICK     20


typedef struct
{
   int32    counter;
   boolean  in_use;
   boolean  expired;
   uint8    next;
}  ds_ucsd_timer_T;

/**************************************************************
*  Function prototypes
***************************************************************/
/*===========================================================================

FUNCTION  ds_ucsd_init_timers

DESCRIPTION
   Initializes all of the timer array elements.
   
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
extern void
ds_ucsd_timer_init(void);





/*===========================================================================

FUNCTION ds_ucsd_timer_cancel

DESCRIPTION
   Cancels a timer.  Note that timer can be actively timing, expired, or
   not set and this function will work correctly.
   
         
DEPENDENCIES
   ds_ucsd_timer_init() must be called first
  
RETURN VALUE
   None
    
SIDE EFFECTS
   Potentially removes a timer from the active timer list

===========================================================================*/
extern void 
ds_ucsd_timer_cancel
(
   uint8 id
);


/*===========================================================================

FUNCTION  ds_ucsd_timer_start()

DESCRIPTION
   Starts a timer with the specified ID for a the specified number of 
   milliseconds.  If the timer is already active, it will be reset
   to the new msec value
   
         
DEPENDENCIES
   ds_ucsd_timer_init() must be called first
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
extern void
ds_ucsd_timer_start
(
   uint8 id,
   int32 msec
);

     
/*===========================================================================

FUNCTION   ds_ucsd_timer_tick()

DESCRIPTION
    Function that will be called to advance the timers by a tick.  Note
    that this function needs to be called once every DS_GCSD_TIMER_MSEC_PER_TICK
    milliseconds for the timers to be accurate.
         
DEPENDENCIES
   None
  
RETURN VALUE
   ds_ucsd_timer_init() must be called first
    
SIDE EFFECTS
   None

===========================================================================*/         
extern void
ds_ucsd_timer_tick(void);

/*===========================================================================

FUNCTION   ds_ucsd_is_timer_expired()

DESCRIPTION
    This function returns whether or not the timer specified by the timer_id 
    has expired or not.

DEPENDENCIES
   None
  
RETURN VALUE
   boolean

SIDE EFFECTS
   None

===========================================================================*/
boolean ds_ucsd_is_timer_expired
(
  uint32 timer_id
);

/*===========================================================================

FUNCTION   ds_ucsd_is_timer_running()

DESCRIPTION
    This function returns whether or not the timer specified by the timer_id 
    is running.

DEPENDENCIES
   None
  
RETURN VALUE
   boolean

SIDE EFFECTS
   None

===========================================================================*/
boolean ds_ucsd_is_timer_running
(
  uint32 timer_id
);

#endif /* DSUCSDTIMER_H */
