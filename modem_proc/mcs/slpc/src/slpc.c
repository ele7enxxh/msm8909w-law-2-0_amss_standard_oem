/*=========================================================================
  FILE: slpc.c
 
  OVERVIEW:
 

  DEPENDENCIES:
 
 
  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.
=========================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/slpc/src/slpc.c#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who   what, where, why
----------   ---   ----------------------------------------------------------- 
01/28/2015   btl   Added dynamic task priority to prevent FW preemption
12/29/2014   btl   Replace msg_queue with qurt signals for OLS, wakeup
12/17/2014   rj    Compute GSTMR Slam value in RTC resolution 
12/15/2014   tak   Split up vstmr_tstmr_slam function to reduce its size 
07/08/2014   btl   Remove VSTMR slam delay 
05/16/2014   btl   Make VSTMR/HW STMR runtime choosable by techs
05/31/2013   cab   Added support for TSTS
04/24/2013   cab   Added support for N-way technology slam 
04/04/2013   cab   Bumped up threshold for ustmr/qtimer sync due to SW exec 
03/29/2013   cab   Fix issue with ustmr rollover at start tick
03/21/2013   cab   Add case for second GSM client to tstmr write routine 
03/12/2013   cab   Revert start tick changes
03/12/2013   cab   Increased robustness when capturing start tick
01/30/2013   cab   Increased wakeup timer priority
12/11/2012   cab   Add notify callback deregister capability 
12/04/2012   cab   Add alternate tstmr to slpc_start for W/G operation
09/12/2012   cab   Fixed get_sysclk_count to correctly handle negative err 
08/29/2012   cab   Revert to timer_set_64 
08/17/2012   cab   Fix bug with dalsys sync object
08/10/2012   cab   Move to use absolute timer set
08/10/2012   cab   Update min warmup comparison to use signed ints
08/10/2012   cab   Update print statements to unsigned format where needed
07/16/2012   cab   Added alternate id for G/W simultaneous operation 
07/16/2012   cab   Added hardware macros, added slpc_sufficient_warmup_time
04/13/2012   cab   Removed USTMR init, moving to MCPM 
04/11/2012   cab   Update to new timer set interface
09/14/2011   cab   Initial version 

============================================================================*/



/*============================================================================

                           INCLUDE FILES

============================================================================*/

#include <mqueue.h>
#include <fcntl.h>
#include <qurt_timer.h>

#include "slpc_hwio.h"
#include <rcinit_rex.h>
#include "rcinit.h"
#include "qurt_mutex.h"
#include "qurt_anysignal.h"
#include "amssassert.h"
#include "DALSys.h"
#include "DALStdErr.h"
#include "DALDeviceId.h"
#include "DalDevice.h"
#include "DDIInterruptController.h"
#include "DDIChipInfo.h"
#include "err.h"
#include "msg.h"
#include "timer.h"
#include "tcxomgr.h"
#include "slpc.h"
#include "slpci.h"

#ifdef SLPC_VSTMR
#include "vstmr.h"
#include "vstmr_1x.h"
#include "vstmr_hdr.h"
#include "vstmr_geran.h"
#include "vstmr_wcdma.h"
#include "vstmr_lte.h"
#include "vstmr_tds.h"
#endif


/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typedefs,
and other items needed by this module.

============================================================================*/

/*----------------------------------------------------------------------------
  Global constants
----------------------------------------------------------------------------*/

#define ISR_LATENCY_THRESH          57600  /* 3ms */

#define SLPC_GSM_SYS_FREQ           ( 1625000uLL * 8uLL )
#define SLPC_1X_SYS_FREQ            ( 1228800uLL * 8uLL )
#define SLPC_HDR_SYS_FREQ           ( 1228800uLL * 8uLL )
#define SLPC_WCDMA_SYS_FREQ         ( 3840000uLL * 8uLL )
#define SLPC_LTE_SYS_FREQ           ( 3840000uLL * 8uLL )
#define SLPC_TDSCDMA_SYS_FREQ       ( 1280000uLL * 8uLL )
#define SLPC_GSM_ALT_SYS_FREQ       ( 1625000uLL * 8uLL )

#define SLPC_GSM_K                  ( 23 )
#define SLPC_1X_K                   ( 21 )
#define SLPC_HDR_K                  ( 21 )
#define SLPC_WCDMA_K                ( 25 )
#define SLPC_LTE_K                  ( 28 )
#define SLPC_TDSCDMA_K              ( 23 )
#define SLPC_GSM_ALT_K              ( 23 )

/* maximum allowed error to accumulate for tracking (3ppm)*/
#define SLPC_MAX_ACCUM_ERR          ( 3 * 1024 )

/* Number of ustmr timer hardware resources */
#define SLPC_MAX_RESOURCES                  3

/* Loop count timeout for read sync */
#define SLPC_READ_SYNC_LOOP_TIMEOUT         20

/* if qtimer and ustmr out of sync more than this many ticks, declare fatal */
#define SLPC_USTMR_SYNC_THRESH              5000

/* value to represent an invalid resource, or resource not held */
#define SLPC_NO_RESOURCE                   -1

/* signal that can be sent to slpc task */
#define SLPC_WAKEUP_SIG                     0x01
#define SLPC_OLS_SIG                        0x02
#define SLPC_ALL_SIGS                      (SLPC_WAKEUP_SIG | SLPC_OLS_SIG)

/* size of messages to task */
#define SLPC_MSG_SIZE                       2

/* upper part of timetick not captured in ustmr count */
#define SLPC_TIMETICK_UPPER_MASK            0xFFFFFFFFF8000000uLL

#ifdef SLPC_MDM9x35
/* lower part of timetick captured in ustmr count */
#define SLPC_TIMETICK_LOWER_MASK            0x0000000000FFFFFFuLL
#else
/* lower part of timetick captured in ustmr count */
#define SLPC_TIMETICK_LOWER_MASK            0x0000000007FFFFFFuLL
#endif
/* max number of clients that can register for wakeup changes */
#define SLPC_MAX_WAKEUP_CALLBACKS  10

/*
  Blast OS can return timer callbacks up to 100 usec early, so that error
  must be accounted for when sanity checking the wakeup timer.
  Double the buffer for safety.
*/
/* 200 usec = 3840 xo ticks */
#define SLPC_WAKEUP_TIMER_ERR      3840

#define INIT_TECH_CRIT_SECTION(id) \
  if (DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, \
                     &slpc[id].sync_ptr, \
                     &slpc[id].sync_obj)) \
  { \
    ERR_FATAL("sleepctl_init: DALSYS_SyncCreate failed!", 0, 0, 0); \
  } 
#define ENTER_TECH_CRIT_SECTION(id) DALSYS_SyncEnter(slpc[id].sync_ptr)
#define LEAVE_TECH_CRIT_SECTION(id) DALSYS_SyncLeave(slpc[id].sync_ptr)

#define INIT_GLOBAL_CRIT_SECTION()  qurt_mutex_init(&slpc_data.slpc_lock)
#define ENTER_GLOBAL_CRIT_SECTION() qurt_mutex_lock(&slpc_data.slpc_lock)
#define LEAVE_GLOBAL_CRIT_SECTION() qurt_mutex_unlock(&slpc_data.slpc_lock)

/* absolute value macro */
#define  ABS( x ) ( ((x) < 0) ? -(x) : (x) )

/* USTMR theshold for considering a wakeup potentially rude (30ms) */
#define SLPC_RUDE_WAKEUP_THRESH 576000

/* Elevated priority for scheduled wakeups, value of SLPC_HIGH_PRI */
#define SLPC_HIGH_PRI_VALUE 0x25

/*----------------------------------------------------------------------------
  Interrupt data
----------------------------------------------------------------------------*/
#define SLPC_RSRC0_INT_NUM     96
#define SLPC_RSRC1_INT_NUM     97
#define SLPC_RSRC2_INT_NUM     22
#define SLPC_RSRC0_INT_TRIGGER DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER
#define SLPC_RSRC1_INT_TRIGGER DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER
#define SLPC_RSRC2_INT_TRIGGER DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER

enum
{
  SLPC_WAKEUP_EVENT,
  SLPC_OLS_EVENT
};

typedef enum
{
  /* Default SLPC task priority from RCINIT */
  SLPC_LOW_PRI,

  /* Elevated priority for processing wakeups without FW preemption */
  SLPC_HIGH_PRI
}
slpc_pri_enum;

DalChipInfoFamilyType slpc_c_fam;
DalChipInfoVersionType slpc_c_ver;

typedef union 
{
  uint64 ud;
  uint32 uw[2];
}
slpc_dword_type;

/*----------------------------------------------------------------------------
  Sleep controller state
----------------------------------------------------------------------------*/

typedef enum
{
  /* Sleep controller in not active */
  SLPC_INACTIVE_STATE,

  /* Sleep controller has captured start point */
  SLPC_START_STATE,

  /* Sleep controller is counting slow clocks until the "wakeup" point */
  SLPC_SLEEP_STATE,

  /* Wakeup point has been reached.  Next: Warmup, or back to sleep? */
  SLPC_WAKEUP_STATE,

  /* Sleep controller is counting slow clocks until the end of sleep */
  SLPC_WARMUP_STATE
}
slpc_state_enum;


/*----------------------------------------------------------------------------
  Sleep start data
----------------------------------------------------------------------------*/

typedef struct
{
  /* USTMR value at read sync */
  ustmr_type                      ustmr_ts;

  /* TSTMR value at read sync */
  slpc_tstmr_type                 tstmr;

  /* timetick value at read sync */
  uint64                          tick;
}
slpc_start_type;


/*----------------------------------------------------------------------------
  Warmup data
----------------------------------------------------------------------------*/

typedef struct
{
  /* Warmup duration, in microseconds (Max = 65.535ms) */
  uint16                          dur_usec;

  /* Warmup duration, in timeticks */
  uint64                          dur_tick;

  /* Timestamp when we transition to warmup period */
  uint64                          tick;
}
slpc_warmup_type;


/*----------------------------------------------------------------------------
  Wakeup data
----------------------------------------------------------------------------*/

typedef struct
{
  /* Wakeup event notification callback */
  slpc_event_callback_type        cb;

  /* Wakeup event start timestamp, in XO units */
  uint64                          cb_start_tick;

  /* Wakeup event end timestamp, in XO units */
  uint64                          cb_end_tick;

  /* Timer group to be defined to non-deferrable */
  timer_group_type                slpc_timer_group;

  /* Timer used to generate wakeup event */
  timer_type                      timer;

  /* Number of times we recorded invalid wakeup event */
  uint16                          invalid_cnt;

  /* timetick value at wakeup */
  uint64                          tick;

  /* timetick value of last wakeup */
  uint64                          last;

  /* Indicates if wakeup point is an update of an earlier wakeup */
  boolean                         update;

  /* Indicates if wakeup point is an extension of an earlier wakeup */
  boolean                         extension;

  /* Indicates wakeup is a possible rude wakeup */
  boolean                         rude;
}
slpc_wakeup_type;


/*----------------------------------------------------------------------------
  Sleep ending data
----------------------------------------------------------------------------*/

typedef struct
{
  /* Sleep ending event notification callback */
  slpc_event_callback_type        cb;

  /* ustmr value at ending point */
  ustmr_type                      ustmr_ts;

  /* OLS event start timestamp, in XO units */
  uint64                          isr_start_tick;

  /* OLS event end timestamp, in XO units */
  uint64                          isr_end_tick;

  /* Number of times we recorded invalid OLS interrupt */
  uint16                          invalid_cnt;

  /* timetick value at ending point */
  uint64                          tick;
}
slpc_ending_type;



/*----------------------------------------------------------------------------
  Last sleep data - used for slpc_error_feedback
----------------------------------------------------------------------------*/

typedef struct
{
  /* Last sleep duration, in universal STMR ticks */
  tstmr_type                      duration_tstmr;

  /* Last sleep duration, in technology STMR ticks */
  ustmr_type                      duration_ustmr;
}
slpc_last_sleep_type;

typedef struct
{
  /* flag to enable alternate stmr activation on online pulse */
  boolean                              valid;

  /* flag to enable alternate stmr activation on online pulse */
  boolean                              load_tstmr;

  /* the time value of this tstmr at start time transfer point (in tech stmr 
     resolution) */
  uint64                               start_time;

  /* the time value of this tstmr at start time transfer point (in tech rtc
     resolution) */
  uint64                               start_time_in_rtc;

  /* the phase value of this tstmr at start time transfer point */
  uint64                               start_phase;

  /* value to load in alternate tech stmr at online pulse */
  uint64                               tstmr;
}
slpc_alt_id_type;

/*----------------------------------------------------------------------------
  tstmr slam function
----------------------------------------------------------------------------*/
typedef void (*slpc_vstmr_tstmr_slam_type)( slpc_id_type pri_id, uint64 dur );

/*----------------------------------------------------------------------------
  Forward declarations
----------------------------------------------------------------------------*/
static void slpc_ols_event( slpc_id_type id );
static void slpc_ustmr0_isr( void );
static void slpc_ustmr1_isr( void );
static void slpc_ustmr2_isr( void );
static void slpc_gsm1_vstmr_tstmr_slam(slpc_id_type pri_id, uint64 dur);
static void slpc_gsm2_vstmr_tstmr_slam(slpc_id_type pri_id, uint64 dur);
static void slpc_gsm3_vstmr_tstmr_slam(slpc_id_type pri_id, uint64 dur);
static void slpc_1x_vstmr_tstmr_slam(slpc_id_type pri_id, uint64 dur);
static void slpc_hdr_vstmr_tstmr_slam(slpc_id_type pri_id, uint64 dur);
static void slpc_wcdma_vstmr_tstmr_slam(slpc_id_type pri_id, uint64 dur);
static void slpc_wcdma2_vstmr_tstmr_slam(slpc_id_type pri_id, uint64 dur);
static void slpc_lte_vstmr_tstmr_slam(slpc_id_type pri_id, uint64 dur);
static void slpc_tdscdma_vstmr_tstmr_slam(slpc_id_type pri_id, uint64 dur);

/*----------------------------------------------------------------------------
  Per technology data
----------------------------------------------------------------------------*/

typedef struct
{
  /* Sleep controller identifier */
  const char *                         id;

  slpc_stmr_mode_enum                  stmr_mode;

  /* Software state */
  slpc_state_enum                      state;

  /* Tech system frequency */
  uint64                               system_freq;

  /* Scaling needed in dyn gain err feedback */
  uint8                                k;

  /* TCXO Manager id's required for retrieving tech specific RGS data */
  tcxomgr_client_info_struct_type      tm_client;

  /* Slam function for this client */
  slpc_vstmr_tstmr_slam_type           vstmr_tstmr_slam_func;

  /* Bit value used to set sys online ctrl register to this tech */
  uint32                               route_mask;

  /* ustmr resource used by this client */
  int8                                 slpc_ustmr_rsrc;

  /* Accumulated slew error */
  int64                                slew_err_accum;

  /* Start of sleep data */
  slpc_start_type                      start;

  /* RF warmup time constants */
  slpc_warmup_type                     warmup;

  /* Wakeup event data */
  slpc_wakeup_type                     wakeup;

  /* End of sleep data */
  slpc_ending_type                     ending;

  /* Last sleep duration, for error_feedback */
  slpc_last_sleep_type                 last_sleep;

  /* Total sleep duration, in USTMR ticks */
  uint64                               ustmr_dur;

  /* Total sleep duration, in TSTMR ticks */
  uint64                               tstmr_dur;

  /* Total sleep duration, in TSTMR ticks, of last period */
  uint64                               last_tstmr_dur;

  /* value to load in tech stmr at online pulse */
  uint64                               tstmr_slam;

  /* List of other techs where timing must be maintained */
  slpc_alt_id_type                     alt_techs[SLPC_NUM_CLIENTS];

  /* message queue for communicating with task */
  qurt_signal_t                        sig;

  /* per-tech lock object to allow maximum wakeup parallelism */
  DALSYSSyncObj                        sync_obj;

  /* per-tech lock handle to allow maximum wakeup parallelism */
  DALSYSSyncHandle                     sync_ptr;

  /* slpc thread id for dynamic priority changing */
  qurt_thread_t                        thread_id;

  /* current task priority value */
  uint16                               thread_cur_pri;

  /* default task priority value */
  uint16                               thread_def_pri;
}
slpc_client_type;

#ifdef SLPC_VSTMR
static slpc_client_type slpc[SLPC_NUM_CLIENTS] =
{
  { "GSM", SLPC_STMR_VSTMR, SLPC_INACTIVE_STATE, SLPC_GSM_SYS_FREQ, SLPC_GSM_K, 
    { TCXOMGR_CLIENT_GSM, TCXOMGR_AS_ID_1 }, slpc_gsm1_vstmr_tstmr_slam },

  { "1X", SLPC_STMR_VSTMR, SLPC_INACTIVE_STATE, SLPC_1X_SYS_FREQ, SLPC_1X_K, 
    { TCXOMGR_CLIENT_CDMA_1X, TCXOMGR_AS_ID_1 }, slpc_1x_vstmr_tstmr_slam }, 

  { "HDR", SLPC_STMR_VSTMR, SLPC_INACTIVE_STATE, SLPC_HDR_SYS_FREQ, SLPC_HDR_K, 
    { TCXOMGR_CLIENT_CDMA_HDR, TCXOMGR_AS_ID_1 }, slpc_hdr_vstmr_tstmr_slam },

  { "WCDMA", SLPC_STMR_VSTMR, SLPC_INACTIVE_STATE, SLPC_WCDMA_SYS_FREQ, SLPC_WCDMA_K, 
    { TCXOMGR_CLIENT_WCDMA, TCXOMGR_AS_ID_1 }, slpc_wcdma_vstmr_tstmr_slam },

  { "WCDMA2", SLPC_STMR_VSTMR, SLPC_INACTIVE_STATE, SLPC_WCDMA_SYS_FREQ, SLPC_WCDMA_K, 
    { TCXOMGR_CLIENT_WCDMA, TCXOMGR_AS_ID_2 }, slpc_wcdma2_vstmr_tstmr_slam },

  { "LTE", SLPC_STMR_VSTMR, SLPC_INACTIVE_STATE, SLPC_LTE_SYS_FREQ, SLPC_LTE_K, 
    { TCXOMGR_CLIENT_LTE, TCXOMGR_AS_ID_1 }, slpc_lte_vstmr_tstmr_slam },

  { "TDSCDMA", SLPC_STMR_VSTMR, SLPC_INACTIVE_STATE, SLPC_TDSCDMA_SYS_FREQ, SLPC_TDSCDMA_K, 
    { TCXOMGR_CLIENT_TDSCDMA, TCXOMGR_AS_ID_1 }, slpc_tdscdma_vstmr_tstmr_slam },

  { "GSM2", SLPC_STMR_VSTMR, SLPC_INACTIVE_STATE, SLPC_GSM_ALT_SYS_FREQ, SLPC_GSM_ALT_K, 
    { TCXOMGR_CLIENT_GSM, TCXOMGR_AS_ID_2 }, slpc_gsm2_vstmr_tstmr_slam },

  { "GSM3", SLPC_STMR_VSTMR, SLPC_INACTIVE_STATE, SLPC_GSM_ALT_SYS_FREQ, SLPC_GSM_ALT_K, 
    { TCXOMGR_CLIENT_GSM, TCXOMGR_AS_ID_3 }, slpc_gsm3_vstmr_tstmr_slam }
};
#else
static slpc_client_type slpc[SLPC_NUM_CLIENTS] =
{
  { "GSM", SLPC_STMR_HW, SLPC_INACTIVE_STATE, SLPC_GSM_SYS_FREQ, SLPC_GSM_K, 
    { TCXOMGR_CLIENT_GSM, TCXOMGR_AS_ID_1 } },

  { "1X", SLPC_STMR_HW, SLPC_INACTIVE_STATE, SLPC_1X_SYS_FREQ, SLPC_1X_K, 
    { TCXOMGR_CLIENT_CDMA_1X, TCXOMGR_AS_ID_1 } }, 

  { "HDR", SLPC_STMR_HW, SLPC_INACTIVE_STATE, SLPC_HDR_SYS_FREQ, SLPC_HDR_K, 
    { TCXOMGR_CLIENT_CDMA_HDR, TCXOMGR_AS_ID_1 } },

  { "WCDMA", SLPC_STMR_HW, SLPC_INACTIVE_STATE, SLPC_WCDMA_SYS_FREQ, SLPC_WCDMA_K, 
    { TCXOMGR_CLIENT_WCDMA, TCXOMGR_AS_ID_1 } },

  { "WCDMA2", SLPC_STMR_HW, SLPC_INACTIVE_STATE, SLPC_WCDMA_SYS_FREQ, SLPC_WCDMA_K, 
    { TCXOMGR_CLIENT_WCDMA, TCXOMGR_AS_ID_2 } },

  { "LTE", SLPC_STMR_HW, SLPC_INACTIVE_STATE, SLPC_LTE_SYS_FREQ, SLPC_LTE_K, 
    { TCXOMGR_CLIENT_LTE, TCXOMGR_AS_ID_1 } },

  { "TDSCDMA", SLPC_STMR_HW, SLPC_INACTIVE_STATE, SLPC_TDSCDMA_SYS_FREQ, SLPC_TDSCDMA_K, 
    { TCXOMGR_CLIENT_TDSCDMA, TCXOMGR_AS_ID_1 } },

  { "GSM2", SLPC_STMR_HW, SLPC_INACTIVE_STATE, SLPC_GSM_ALT_SYS_FREQ, SLPC_GSM_ALT_K, 
    { TCXOMGR_CLIENT_GSM, TCXOMGR_AS_ID_2 } },

  { "GSM3", SLPC_STMR_HW, SLPC_INACTIVE_STATE, SLPC_GSM_ALT_SYS_FREQ, SLPC_GSM_ALT_K, 
    { TCXOMGR_CLIENT_GSM, TCXOMGR_AS_ID_3 } }
};
#endif

/*----------------------------------------------------------------------------
  USTMR resource data
----------------------------------------------------------------------------*/

typedef struct
{
  /* indicates this resource is in use by a client */
  boolean in_use;

  /* equivalent of "tramp irq"...the specific id of the interrupt */
  DALInterruptID int_number; 

  /* edge/level and high/low configuration information */
  uint32 int_trigger;

  /* pointer to the actual isr routine */
  DALISR isr_function;

  /* id of client using this resource */
  slpc_id_type id;
}
slpc_ustmr_rsrc_type;

#ifdef SLPC_VSTMR

typedef struct
{
  /* gsm1 technology view */
  vstmr_geran_view_s *gsm_view1;

  /* gsm2 technology view */
  vstmr_geran_view_s *gsm_view2;

  /* gsm3 technology view */
  vstmr_geran_view_s *gsm_view3;

  /* 1x technology view */
  vstmr_1x_view_s *onex_view;

  /* wcdma technology view */
  vstmr_wcdma_view_s *wcdma_view;

  /* wcdma2 technology view */
  vstmr_wcdma_view_s *wcdma_view2;

  /* hdr technology view */
  vstmr_hdr_view_s *hdr_view;

  /* lte technology view */
  vstmr_lte_view_s *lte_view;

  /* tdscdma technology view */
  vstmr_tds_view_s *tdscdma_view;
}
slpc_vstmr_data_type;

#endif

/*----------------------------------------------------------------------------
  Overall data
----------------------------------------------------------------------------*/

typedef struct
{
  /* flags for whether a resource is being used currently */
  slpc_ustmr_rsrc_type slpc_ustmr_rsrc[SLPC_MAX_RESOURCES];

  /* for atomic sections within slpc module */
  qurt_mutex_t slpc_lock;

  /* callbacks for anyone wanting to track wakeup point changes */
  slpc_notify_cb_type notify_cb[SLPC_MAX_WAKEUP_CALLBACKS];

  /* callbacks for anyone wanting wakeup event notifications */
  slpc_notify_wakeup_cb_type notify_wakeup_cb[SLPC_MAX_WAKEUP_CALLBACKS];

  /* handle used for all interrupt servicing */
  DalDeviceHandle *int_handle;

  /* handle used for timetick services */
  DalDeviceHandle *tt_handle;

  /* signal registered with OS */
  qurt_anysignal_t slpc_q6_signal;

#ifdef SLPC_VSTMR
  /* handles and other data required in vstmr interface */
  slpc_vstmr_data_type vstmr_data;
#endif
}
slpc_type;

static slpc_type slpc_data =
{
  { 
    {FALSE, SLPC_RSRC0_INT_NUM, SLPC_RSRC0_INT_TRIGGER, (DALISR) slpc_ustmr0_isr},
    {FALSE, SLPC_RSRC1_INT_NUM, SLPC_RSRC1_INT_TRIGGER, (DALISR) slpc_ustmr1_isr},
    {FALSE, SLPC_RSRC2_INT_NUM, SLPC_RSRC2_INT_TRIGGER, (DALISR) slpc_ustmr2_isr}
  }
};

static uint8 slpc_num_rsrc;

/*----------------------------------------------------------------------------
  Function definitions
----------------------------------------------------------------------------*/

/******************************************************************************
  @brief SLPC_SET_STMR_MODE

  Set whether SLPC client uses HW TSTMR or virtual STMR (VSTMR).
  If not set, will default to VSTMR.

  @param id: identifies the calling client
  @param mode: is client using VSTMR or HW STMR?
  @see 
  @return none. 
******************************************************************************/
void slpc_set_stmr_mode( slpc_id_type id, slpc_stmr_mode_enum mode )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

/*--------------------------------------------------------------------------*/

  ASSERT( id < SLPC_NUM_CLIENTS );
  ASSERT( mode == SLPC_STMR_VSTMR || mode == SLPC_STMR_HW );

  sc->stmr_mode = mode;

  if( mode == SLPC_STMR_HW && slpc_num_rsrc == 0 )
  {
    ERR_FATAL("HW STMR not supported, tech %d", id, 0, 0); 
  }

#ifndef SLPC_VSTMR
  if( mode == SLPC_STMR_VSTMR )
  {
    ERR_FATAL("VSTMR not supported, tech %d", id, 0, 0); 
  }
#endif

  return;
}

/******************************************************************************
  @brief SLPC_GET_STMR_MODE

  Return whether SLPC client uses HW TSTMR or virtual STMR (VSTMR).

  @param id: identifies the calling client
  @param mode: is client using VSTMR or HW STMR?
  @return slpc_stmr_mode_enum
******************************************************************************/
slpc_stmr_mode_enum slpc_get_stmr_mode( slpc_id_type id )
{
  return slpc[id].stmr_mode;
}

/******************************************************************************
  @brief SLPC_IS_ACTIVE

  Is sleep controller active for client (started/sleep/wakeup/warmup)?

  @param id: identifies the calling client
  @return TRUE if slpc active, else FALSE
******************************************************************************/
boolean slpc_is_active( slpc_id_type id )
{
  return ( slpc[id].state != SLPC_INACTIVE_STATE ? TRUE : FALSE );
}

/******************************************************************************
  @brief SLPC_SET_TASK_PRI

  Elevate SLPC priority to higher than SW and usual FW priorities to prevent
  preemption during normal wakeups. Rude wakeups should not be elevated

  @param id: identifies the calling client
  @param pri: SLPC_HIGH_PRI or SLPC_LOW_PRI to default priority
  @return none. 
******************************************************************************/
static void slpc_set_task_pri( slpc_id_type id, slpc_pri_enum pri )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* Tech's SLPC task priority will be set to this value */
  uint16 new_pri;

  /* sanity check of new priority */
  int32 check_pri;

/*--------------------------------------------------------------------------*/

  new_pri = (pri == SLPC_LOW_PRI) ? sc->thread_def_pri : SLPC_HIGH_PRI_VALUE;

  /* if current priority is same as new priority we're already done */
  if( new_pri != sc->thread_cur_pri )
  {
    DBG_1( HIGH, "Setting task pri to %d", new_pri );
    TRACE(id, SET_PRI, new_pri, sc->thread_cur_pri, sc->thread_id );
    qurt_thread_set_priority( sc->thread_id, new_pri );

    /* sanity check */
    check_pri = qurt_thread_get_priority( sc->thread_id );
    if ( check_pri < 0 || new_pri != check_pri )
    {
      /* CR997184: Try again, may fail due to thread contention */
      qurt_thread_set_priority( sc->thread_id, new_pri );
      check_pri = qurt_thread_get_priority( sc->thread_id );
      if ( check_pri < 0 || new_pri != check_pri )
      {
        ERR_FATAL("Failed to set priority", new_pri, check_pri, sc->thread_id);
      }
    }
    sc->thread_cur_pri = new_pri;
  }
}

/******************************************************************************
  @brief SLPC_WAKEUP_TIMER_CB
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @return none. 
******************************************************************************/
static void slpc_wakeup_timer_cb( slpc_id_type id )
{
  TRACE(id, WAKEUP_TIMER_CB, 0, 0, 0); 
  qurt_signal_set( &slpc[id].sig, SLPC_WAKEUP_SIG );
}

/******************************************************************************
  @brief SLPC_USTMR0_ISR
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @see 
  @return none. 
******************************************************************************/
static void slpc_ustmr0_isr( void )
{
  int id = slpc_data.slpc_ustmr_rsrc[0].id;
  slpc_client_type *sc = &slpc[id];
  qurt_signal_set( &sc->sig, SLPC_OLS_SIG );
}

/******************************************************************************
  @brief SLPC_USTMR1_ISR
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @see 
  @return none. 
******************************************************************************/
static void slpc_ustmr1_isr( void )
{
  int id = slpc_data.slpc_ustmr_rsrc[1].id;
  slpc_client_type *sc = &slpc[id];
  qurt_signal_set( &sc->sig, SLPC_OLS_SIG );
}

/******************************************************************************
  @brief SLPC_USTMR2_ISR
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @see 
  @return none. 
******************************************************************************/
static void slpc_ustmr2_isr( void )
{
  int id = slpc_data.slpc_ustmr_rsrc[2].id;
  slpc_client_type *sc = &slpc[id];
  qurt_signal_set( &sc->sig, SLPC_OLS_SIG );
}

/******************************************************************************
  @brief SLPC_GET_TICK

  Get 64-bit absolute timetick

  @param none
  @return timetick
******************************************************************************/
uint64 slpc_get_tick ( void )
{
  uint64 tick = 0;
  uint32 offset = 0;

  if (DAL_SUCCESS != DalTimetick_GetTimetick64( 
                     slpc_data.tt_handle, (DalTimetickTime64Type *) &tick ))
  {
    ERR_FATAL("timetick error!", 0, 0, 0); 
  }

  DalTimetick_GetOffset( slpc_data.tt_handle, &offset );

  return (tick - offset);
}

/******************************************************************************
  @brief SLPC_GET_START_TSTMR

  Get start TSTMR value for alt tech

  @param id: identifies the calling client
  @return: alt tstmr
******************************************************************************/
slpc_tstmr_type slpc_get_start_tstmr( slpc_id_type id, slpc_id_type alt_id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* Storage for return of alt_id tech stmr value at time transfer */
  slpc_tstmr_type tstmr;

  /* Initialize tstmr */
  tstmr.alt_time  = 0;
  tstmr.alt_phase = 0;

/*--------------------------------------------------------------------------*/

  ASSERT( id < SLPC_NUM_CLIENTS );
  ASSERT( alt_id < SLPC_NUM_CLIENTS );

  tstmr.time = sc->alt_techs[alt_id].start_time;
  tstmr.phase = sc->alt_techs[alt_id].start_phase;

  return tstmr;
}

#ifdef SLPC_VSTMR
/******************************************************************************
  @brief SLPC_VSTMR_TSTMR_READ_START
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @see 
  @return none. 
******************************************************************************/
static slpc_start_type slpc_vstmr_tstmr_read_start( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* return value */
  slpc_start_type start_ts = {0, {0, 0, 0, 0}, 0};

  /* local copy of tech dependent stmr formatted value */
  vstmr_gstmr_t vstmr_gstmr;
  vstmr_wstmr_t vstmr_wstmr;
  vstmr_1xframe_t vstmr_1xframe;
  vstmr_hstmr_t vstmr_hstmr;
  vstmr_ostmr_t vstmr_ostmr;
  vstmr_tdsstmr_t vstmr_tdsstmr;

/*--------------------------------------------------------------------------*/

  ASSERT( id < SLPC_NUM_CLIENTS );

  /* read the current XO value */
  start_ts.tick = VSTMR_XO_READ_FULL();
  start_ts.ustmr_ts = start_ts.tick & SLPC_TIMETICK_LOWER_MASK;

  /* find corresponding tstmr value for current xo, for all techs */
  /* for GERAN, capture the start time in RTC resolution as well. GERAN has a
     special requirement which can allow it to change its frame size. SLPC is
     not aware those frame adjustments. Hence, SLPC captures the raw RTC for
     GERAN and uses that to compute GERAN's slam point. */

  vstmr_gstmr = vstmr_geran_xo_to_gstmr(slpc_data.vstmr_data.gsm_view1, 
                                        (uint32) start_ts.ustmr_ts);
  sc->alt_techs[SLPC_GSM].start_time = vstmr_gstmr.all;
  sc->alt_techs[SLPC_GSM].start_time_in_rtc = 
     vstmr_xo_to_rtc(slpc_data.vstmr_data.gsm_view1->rtc, (uint32) start_ts.ustmr_ts);

  vstmr_gstmr = vstmr_geran_xo_to_gstmr(slpc_data.vstmr_data.gsm_view2, 
                                        (uint32) start_ts.ustmr_ts);
  sc->alt_techs[SLPC_GSM2].start_time = vstmr_gstmr.all;
  sc->alt_techs[SLPC_GSM2].start_time_in_rtc = 
     vstmr_xo_to_rtc(slpc_data.vstmr_data.gsm_view2->rtc, (uint32) start_ts.ustmr_ts);

  vstmr_gstmr = vstmr_geran_xo_to_gstmr(slpc_data.vstmr_data.gsm_view3, 
                                        (uint32) start_ts.ustmr_ts);
  sc->alt_techs[SLPC_GSM3].start_time = vstmr_gstmr.all;
  sc->alt_techs[SLPC_GSM3].start_time_in_rtc = 
     vstmr_xo_to_rtc(slpc_data.vstmr_data.gsm_view3->rtc, (uint32) start_ts.ustmr_ts);

  vstmr_1xframe = vstmr_1x_xo_to_frame(slpc_data.vstmr_data.onex_view, 
                                       (uint32) start_ts.ustmr_ts);
  sc->alt_techs[SLPC_1X].start_time = vstmr_1xframe.all;
  vstmr_hstmr = vstmr_hdr_xo_to_hstmr(slpc_data.vstmr_data.hdr_view, 
                                       (uint32) start_ts.ustmr_ts);
  sc->alt_techs[SLPC_HDR].start_time = vstmr_hstmr.all;
  vstmr_wstmr = vstmr_wcdma_xo_to_wstmr(slpc_data.vstmr_data.wcdma_view, 
                                       (uint32) start_ts.ustmr_ts);
  sc->alt_techs[SLPC_WCDMA].start_time = vstmr_wstmr.all;
  vstmr_wstmr = vstmr_wcdma_xo_to_wstmr(slpc_data.vstmr_data.wcdma_view2, 
                                       (uint32) start_ts.ustmr_ts);
  sc->alt_techs[SLPC_WCDMA2].start_time = vstmr_wstmr.all;
  vstmr_ostmr = vstmr_lte_xo_to_ostmr(slpc_data.vstmr_data.lte_view, 
                                       (uint32) start_ts.ustmr_ts);
  sc->alt_techs[SLPC_LTE].start_time = vstmr_ostmr.all;
  vstmr_tdsstmr = vstmr_tds_xo_to_stmr(slpc_data.vstmr_data.tdscdma_view, 
                                       (uint32) start_ts.ustmr_ts);
  sc->alt_techs[SLPC_TDSCDMA].start_time = vstmr_tdsstmr.all;

  /* to match legacy operation, return alt tech data to G and W */
  start_ts.tstmr.time = sc->alt_techs[id].start_time;
  if ( (id == SLPC_GSM) || (id == SLPC_GSM2) || (id == SLPC_GSM3) )
  {
    start_ts.tstmr.alt_time = sc->alt_techs[SLPC_WCDMA].start_time;
  }
  if ( id == SLPC_WCDMA )
  {
    start_ts.tstmr.alt_time = sc->alt_techs[SLPC_GSM].start_time;
  }

  return start_ts;
}
#endif

/******************************************************************************
  @brief SLPC_HW_READ_TSTMR
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @see 
  @return none. 
******************************************************************************/
static slpc_tstmr_type slpc_hw_read_tstmr( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* combined tstmr tick value */
  slpc_tstmr_type tstmr;

  /* Initialize tstmr */
  tstmr.alt_time  = 0;
  tstmr.alt_phase = 0;

/*--------------------------------------------------------------------------*/

  ASSERT( id < SLPC_NUM_CLIENTS );

  sc->alt_techs[SLPC_GSM].start_time = SLPC_READ_TSTMR_TIME_GSM();
  sc->alt_techs[SLPC_GSM].start_phase = SLPC_READ_TSTMR_PHASE_GSM();
  sc->alt_techs[SLPC_GSM2].start_time = SLPC_READ_TSTMR_TIME_GSM2();
  sc->alt_techs[SLPC_GSM2].start_phase = SLPC_READ_TSTMR_PHASE_GSM2();
  sc->alt_techs[SLPC_GSM3].start_time = SLPC_READ_TSTMR_TIME_GSM3();
  sc->alt_techs[SLPC_GSM3].start_phase = SLPC_READ_TSTMR_PHASE_GSM3();
  sc->alt_techs[SLPC_1X].start_time = ( SLPC_READ_TSTMR_TIME_1X() >> 2 );
  sc->alt_techs[SLPC_1X].start_phase = SLPC_READ_TSTMR_PHASE_1X();
  sc->alt_techs[SLPC_HDR].start_time = SLPC_READ_TSTMR_TIME_HDR();
  sc->alt_techs[SLPC_HDR].start_phase = SLPC_READ_TSTMR_PHASE_HDR();
  sc->alt_techs[SLPC_WCDMA].start_time = SLPC_READ_TSTMR_TIME_WCDMA();
  sc->alt_techs[SLPC_WCDMA].start_phase = SLPC_READ_TSTMR_PHASE_WCDMA();
  sc->alt_techs[SLPC_WCDMA2].start_time = SLPC_READ_TSTMR_TIME_WCDMA();
  sc->alt_techs[SLPC_WCDMA2].start_phase = SLPC_READ_TSTMR_PHASE_WCDMA();
  sc->alt_techs[SLPC_LTE].start_time = SLPC_READ_TSTMR_TIME_LTE();
  sc->alt_techs[SLPC_LTE].start_phase = SLPC_READ_TSTMR_PHASE_LTE();
  sc->alt_techs[SLPC_TDSCDMA].start_time = SLPC_READ_TSTMR_TIME_TDSCDMA();
  sc->alt_techs[SLPC_TDSCDMA].start_phase = SLPC_READ_TSTMR_PHASE_TDSCDMA();

  tstmr.time = sc->alt_techs[id].start_time;
  tstmr.phase = sc->alt_techs[id].start_phase;
  if ( (id == SLPC_GSM) || (id == SLPC_GSM2) || (id == SLPC_GSM3))
  {
    tstmr.alt_time = sc->alt_techs[SLPC_WCDMA].start_time;
    tstmr.alt_phase = sc->alt_techs[SLPC_WCDMA].start_phase;
  }
  if ( id == SLPC_WCDMA )
  {
    tstmr.alt_time = sc->alt_techs[SLPC_GSM].start_time;
    tstmr.alt_phase = sc->alt_techs[SLPC_GSM].start_phase;
  }

  return tstmr;
}

/******************************************************************************
  @brief SLPC_HW_READ_STMR_SYNC
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @see 
  @return none. 
******************************************************************************/
static slpc_start_type slpc_hw_read_stmr_sync( slpc_id_type id )
{
  /* comparison variable to make sure read was latched */
  ustmr_type ustmr_comp_ts, ustmr_qtimer;

  /* comparison to make sure not on 23bit boundary */
  uint64 tick1, tick2;

  /* loop counter */
  uint8 count = 0;

  /* return value */
  slpc_start_type start_ts = {0, {0, 0, 0, 0}, 0};

/*--------------------------------------------------------------------------*/

  /* repeat the read until ustmr consistently reads the same */
  /* repeat the read until not on timetick upper bits boundary */
  do
  {
    ENTER_GLOBAL_CRIT_SECTION();
    tick1 = slpc_get_tick();
    ustmr_qtimer = tick1 & SLPC_TIMETICK_LOWER_MASK;
    tick1 = tick1 & SLPC_TIMETICK_UPPER_MASK;

    /* trigger the synchronous read of ustmr and tstmr */
    SLPC_HW_READ_SYNC_CMD();

    start_ts.ustmr_ts = SLPC_READ_USTMR_TIME_SYNC();
    start_ts.tstmr = slpc_hw_read_tstmr( id );
    ustmr_comp_ts = SLPC_READ_USTMR_TIME_SYNC();
    LEAVE_GLOBAL_CRIT_SECTION();
    tick2 = slpc_get_tick() & SLPC_TIMETICK_UPPER_MASK;
    if ( ++count >= (SLPC_READ_SYNC_LOOP_TIMEOUT - 2) )
    {
      /* something appears to be wrong, start logging the moving counters */
      TRACE(id, READ_SYNC_ERR, tick1, ustmr_qtimer, ustmr_comp_ts);
    }
    if ( count >= SLPC_READ_SYNC_LOOP_TIMEOUT )
    {
      ERR_FATAL("read sync failure, id: %d, ustmr1: %d, ustmr2: %d", 
                id, start_ts.ustmr_ts, ustmr_comp_ts);
    }
  }
  while ( (start_ts.ustmr_ts != ustmr_comp_ts) || (tick1 != tick2) || 
          (ABS(ustmr_qtimer - ustmr_comp_ts) > (int64) SLPC_USTMR_SYNC_THRESH) );

  /* upper bits from timetick coupled with lower bits from ustmr count */
  start_ts.tick = tick1 + start_ts.ustmr_ts;

  return( start_ts );
}

/******************************************************************************
  @brief SLPC_HW_WRITE_TSTMR
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @see 
  @return none. 
******************************************************************************/
static void slpc_hw_write_tstmr( slpc_id_type id, uint64 tstmr )
{
  switch( id )
  {
  case SLPC_GSM:
    SLPC_WRITE_TSTMR_TIME_GSM( tstmr );
    SLPC_WRITE_TSTMR_CMD_GSM();
    break;
  case SLPC_1X:
    SLPC_WRITE_TSTMR_TIME_1X( tstmr );
    break;
  case SLPC_HDR:
    SLPC_WRITE_TSTMR_TIME_HDR( tstmr );
    break;
  case SLPC_WCDMA:
  case SLPC_WCDMA2:
    SLPC_WRITE_TSTMR_TIME_WCDMA( tstmr );
    SLPC_WRITE_TSTMR_CMD_WCDMA();
    break;
  case SLPC_LTE:
    SLPC_WRITE_TSTMR_TIME_LTE( tstmr );
    SLPC_WRITE_TSTMR_CMD_LTE();
    break;
  case SLPC_TDSCDMA:
    SLPC_WRITE_TSTMR_TIME_TDSCDMA( tstmr );
    SLPC_WRITE_TSTMR_CMD_TDSCDMA();
    break;
  case SLPC_GSM2:
    SLPC_WRITE_TSTMR_TIME_GSM2( tstmr );
    SLPC_WRITE_TSTMR_CMD_GSM2();
    break;
  case SLPC_GSM3:
    SLPC_WRITE_TSTMR_TIME_GSM3( tstmr );
    SLPC_WRITE_TSTMR_CMD_GSM3();
    break;
  default:
    ERR_FATAL("slpc_hw_write_tstmr, invalid id: %d", id, 0, 0); 
  };
}

/******************************************************************************
  @brief SLPC_HW_WRITE_STMR_SYNC
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @see 
  @return none. 
******************************************************************************/
static void slpc_hw_write_stmr_sync( slpc_id_type id, ustmr_type ustmr_ts, 
                                     slpc_tstmr_type *tstmr )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* Mask for which techs are to receive the online pulse event */
  uint32 routing = 0;

  /* loop counter */
  uint8 i;

/*--------------------------------------------------------------------------*/

  /* Set to route to primary, and any alternate techs if needed */
  routing = sc->route_mask;
  for ( i = 0; i < SLPC_NUM_CLIENTS; i++ )
  {
    if ( sc->alt_techs[i].valid )
    {
      TRACE(id, ALT_ID_SET, i, sc->alt_techs[i].valid, sc->alt_techs[i].tstmr);
      routing |= slpc[i].route_mask;
    }
  }

  /* route this ustmr hardware to both primary and alt technology */
  SLPC_USTMR_SET_ROUTING( sc->slpc_ustmr_rsrc, routing );

  /* program the timer value, the ustmr point to trigger the ols event */
  SLPC_USTMR_SET_TIME( sc->slpc_ustmr_rsrc, sc->ending.ustmr_ts );

  /* skip tstmr programming if client is doing it */
  if ( tstmr != NULL )
  {
    /* setup the primary tech stmr slam, arm it */
    slpc_hw_write_tstmr( id, tstmr->time );
  }

  /* setup any alternate tech stmr slam, arm it */
  for ( i = 0; i < SLPC_NUM_CLIENTS; i++ )
  {
    if ( sc->alt_techs[i].valid && sc->alt_techs[i].load_tstmr )
    {
      slpc_hw_write_tstmr( (slpc_id_type) i, sc->alt_techs[i].tstmr );
    }
  }

  /* enable the interrupt to the processor */
  SLPC_USTMR_EN_INT( sc->slpc_ustmr_rsrc );

  /* start the timer */
  SLPC_USTMR_START_TIMER( sc->slpc_ustmr_rsrc );
}

/******************************************************************************
  @brief slpc_enable_alt_id
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @see 
  @return none. 
******************************************************************************/
void slpc_enable_alt_id( slpc_id_type id, slpc_id_type alt_id, 
                         slpc_tstmr_type *tstmr )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

/*--------------------------------------------------------------------------*/

  ASSERT( id < SLPC_NUM_CLIENTS );
  ASSERT( alt_id < SLPC_NUM_CLIENTS );

  sc->alt_techs[alt_id].valid = TRUE;

  if ( tstmr == NULL )
  {
    sc->alt_techs[alt_id].load_tstmr = FALSE;
    sc->alt_techs[alt_id].tstmr = 0;
  }
  else
  {
    sc->alt_techs[alt_id].load_tstmr = TRUE;
    sc->alt_techs[alt_id].tstmr = tstmr->time;
  }
}

/******************************************************************************
  @brief slpc_disable_alt_id
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @see 
  @return none. 
******************************************************************************/
void slpc_disable_alt_id( slpc_id_type id, slpc_id_type alt_id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

/*--------------------------------------------------------------------------*/

  sc->alt_techs[alt_id].valid = FALSE;
  sc->alt_techs[alt_id].load_tstmr = FALSE;
  sc->alt_techs[alt_id].tstmr = 0;
}

/******************************************************************************
  @brief SLPC_ACQUIRE_RESOURCE
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @see 
  @return boolean for success/failure 
******************************************************************************/
boolean slpc_acquire_resource( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* Loop counter */
  uint8 i;

  /* return value */
  boolean ret_val = TRUE;

/*--------------------------------------------------------------------------*/

  ENTER_GLOBAL_CRIT_SECTION();

  /* find an available resource */
  for ( i = 0; i < slpc_num_rsrc; i++ )
  {
    if ( !slpc_data.slpc_ustmr_rsrc[i].in_use )
    {
      slpc_data.slpc_ustmr_rsrc[i].in_use = TRUE;
      slpc_data.slpc_ustmr_rsrc[i].id = id;
      sc->slpc_ustmr_rsrc = i;
      break;
    }
  }

  /* if no resources were available */
  if ( i == slpc_num_rsrc )
  {
    DBG_1(ERROR, "No resources left, id: %d", id);
    ret_val = FALSE;
  }

  LEAVE_GLOBAL_CRIT_SECTION();
  return ret_val;
}

/******************************************************************************
  @brief SLPC_RELEASE_RESOURCE
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @see 
  @return none. 
******************************************************************************/
void slpc_release_resource( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

/*--------------------------------------------------------------------------*/

  ENTER_GLOBAL_CRIT_SECTION();

  if ( sc->slpc_ustmr_rsrc != SLPC_NO_RESOURCE )
  {
    slpc_data.slpc_ustmr_rsrc[sc->slpc_ustmr_rsrc].in_use = FALSE;
    sc->slpc_ustmr_rsrc = SLPC_NO_RESOURCE;
  }

  LEAVE_GLOBAL_CRIT_SECTION();
}

/******************************************************************************
  @brief SLPC_CALL_NOTIFY
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @see 
  @return none. 
******************************************************************************/
static void slpc_call_notify( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* loop counter */
  uint8 i = 0;

/*--------------------------------------------------------------------------*/

  for ( i = 0; i < SLPC_MAX_WAKEUP_CALLBACKS; i++ )
  {
    if ( slpc_data.notify_cb[i] != NULL )
    {
      slpc_data.notify_cb[i]( id, sc->wakeup.tick, sc->wakeup.update, 
                              sc->wakeup.extension );
    }
  }
}

/******************************************************************************
  @brief SLPC_CALL_NOTIFY_WAKEUP
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @see 
  @return none. 
******************************************************************************/
static void slpc_call_notify_wakeup( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* loop counter */
  uint8 i = 0;

/*--------------------------------------------------------------------------*/

  for ( i = 0; i < SLPC_MAX_WAKEUP_CALLBACKS; i++ )
  {
    if ( slpc_data.notify_wakeup_cb[i] != NULL )
    {
      slpc_data.notify_wakeup_cb[i]( id, sc->wakeup.tick );
    }
  }
}

/******************************************************************************
  @brief SLPC_UPDATE_WAKEUP_POINT
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @see 
  @return none. 
******************************************************************************/
static slpc_err_type slpc_update_wakeup_point( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* Local read of current timetick */
  uint64 tick;

  /* return value */
  slpc_err_type err_code = SLPC_ERR_NO_ERR;

/*--------------------------------------------------------------------------*/

  sc->wakeup.last = sc->wakeup.tick;
  sc->wakeup.tick = sc->ending.tick - sc->warmup.dur_tick;

  ENTER_TECH_CRIT_SECTION(id);

  tick = slpc_get_tick();
  TRACE(id, UPDATE_WAKEUP, sc->state, tick, sc->wakeup.tick);

  if ( sc->wakeup.tick < tick )
  {
    DBG_3(ERROR, "Wakeup point is in the past! n=%lu w=%lu s=%lu",
          (uint32) tick, (uint32) sc->wakeup.tick, (uint32) sc->ending.tick);
    err_code = SLPC_ERR_WAKEUP_IN_PAST;
  }
  else
  {
    timer_set_64( &sc->wakeup.timer, (sc->wakeup.tick - tick), 0, T_TICK );
  }

  /* check if this is an update to a previously set wakeup point */
  if ( sc->state == SLPC_SLEEP_STATE ) 
  {
    sc->wakeup.update = TRUE;
  }
  else
  {
    sc->wakeup.update = FALSE;
  }

  /* check if new wakeup is later than old, so this is an extension */
  if ( sc->wakeup.tick > sc->wakeup.last ) 
  {
    sc->wakeup.extension = TRUE;
  }
  else
  {
    sc->wakeup.extension = FALSE;
  }

  /* check if new wakeup is possibly rude; if not, elevate slpc priority */
  if ( sc->wakeup.update && (sc->wakeup.tick - tick) < SLPC_RUDE_WAKEUP_THRESH )
  {
    sc->wakeup.rude = TRUE;
    DBG_0( HIGH, "Rude wakeup" );
    slpc_set_task_pri( id, SLPC_LOW_PRI );
  }
  else
  {
    sc->wakeup.rude = FALSE;
    /* raise priority for scheduled wakeups to prevent pre-emption by FW */
    slpc_set_task_pri( id, SLPC_HIGH_PRI );
  }

  LEAVE_TECH_CRIT_SECTION(id);

  return err_code;
}


/******************************************************************************
  @brief SLPC_TSTMR_TO_USTMR
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return ustmr_type - value of duration in USTMR units 
******************************************************************************/
static int64 slpc_tstmr_to_ustmr( slpc_id_type id, tstmr_type tstmr_dur )
{
  /* Duration in "uncorrected", or raw, XO's */
  int64 dur_xo = 0;

  /* Error storage, in ppm/1024 */
  int64 err = 0;

  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* XO manager xo error data */
  tcxomgr_vco_info_type vco = tcxomgr_get_client_rgs(sc->tm_client);

/*--------------------------------------------------------------------------*/

  /* collect all error in 1/1024 ppm, part from slew, part from RGS */
  err = sc->slew_err_accum + vco.rot_value;

  DBG_3(HIGH, "err: %ld, slew_err_accum: %ld, rot_err: %ld", 
        (int32) err, (int32) sc->slew_err_accum, (int32) vco.rot_value );

  /* convert to intermediate err, accounting for rounding */
  err = ((192 * err) + ((1024 * 10) >> 1)) / (1024 * 10);
  dur_xo = (int64) tstmr_dur * (19200000LL + err);
  dur_xo = (dur_xo + (sc->system_freq >> 1)) / sc->system_freq;

  TRACE(id, TSTMR_TO_USTMR, tstmr_dur, sc->slew_err_accum, dur_xo);

  return dur_xo;
}

/******************************************************************************
  @brief SLPC_GET_WAKEUP_TICK

  Get wakeup interrupt point, in Qtimer ticks

  @param id: identifies the calling client

  @return absolute timetick of the wakeup point
******************************************************************************/
uint64 slpc_get_wakeup_tick( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

/*--------------------------------------------------------------------------*/

  TRACE(id, GET_WAKEUP_TICK, sc->wakeup.tick, 0, 0);
  return sc->wakeup.tick;
}


/******************************************************************************
  @brief SLPC_GET_OLS_TICK

  Get the absolute timetick of the ending interrupt (Qtimer)

  @param id: identifies the calling client

  @return absolute timetick of the online start point
******************************************************************************/
uint64 slpc_get_ols_tick( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

/*--------------------------------------------------------------------------*/

  TRACE(id, GET_OLS_TICK, sc->ending.tick, 0, 0);
  return sc->ending.tick;
}

/******************************************************************************
  @brief SLPC_GET_WAKEUP_AND_OLS_TICK

  Get the absolute timeticks of the wakeup and online start points

  @param id: identifies the calling client

  @return absolute timetick of the wakeup point and OLS point
******************************************************************************/
void slpc_get_wakeup_and_ols_tick( slpc_id_type id, uint64 *wakeup_tick, 
                                   uint64 *ols_tick )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

/*--------------------------------------------------------------------------*/

  ENTER_TECH_CRIT_SECTION(id);

  TRACE(id, GET_WAKEUP_TICK, sc->wakeup.tick, 0, 0);
  TRACE(id, GET_OLS_TICK, sc->ending.tick, 0, 0);

  if ( wakeup_tick != NULL )
  {
    *wakeup_tick = sc->wakeup.tick;
  }
  if ( ols_tick != NULL )
  {
    *ols_tick = sc->ending.tick;
  }

  LEAVE_TECH_CRIT_SECTION(id);
}

/******************************************************************************
  @brief SLPC_GET_SYSCLK_COUNT

  Returns the number of TSTMR ticks the controller has been active for.

  @param id: identifies the calling client
  @return TSTMR ticks
******************************************************************************/
uint64 slpc_get_sysclk_count( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* Error calculation in ppm */
  int64 err_ppm = 0;

  /* XO manager xo error data */
  tcxomgr_vco_info_type vco = tcxomgr_get_client_rgs(sc->tm_client);

  /* Local read of current timetick, duration in timeticks */
  uint64 tick, dur_xo, dur_tstmr;

/*--------------------------------------------------------------------------*/

  tick = slpc_get_tick();

  dur_xo = tick - sc->start.tick;

  /* collect all error in 1/1024 ppm, part from slew, part from RGS */
  err_ppm = sc->slew_err_accum + vco.rot_value;
  dur_xo = (int64) dur_xo - (int64) ((err_ppm * (int64) dur_xo) / 1024000000LL);
  dur_tstmr = (dur_xo * sc->system_freq) / 19200000LL;

  TRACE(id, GET_SYSCLK_COUNT, tick, err_ppm, dur_tstmr);
  return dur_tstmr;
}


/******************************************************************************
  @brief SLPC_ERROR_FEEDBACK

  Adjust the slow clock frequency estimate, based on the sleep duration
  and the reacquisition slews.

  @param id: identifies the calling client
  @param slew_err: slew error (TSTMR)
  @return none
******************************************************************************/
void slpc_error_feedback( slpc_id_type id, int64 slew_err )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* Error calculation in ppm */
  int64 err_ppm = 0;

/*--------------------------------------------------------------------------*/

  DBG_2(HIGH, "dur: %lu sysclks, err: %ld sysclks", 
        (uint32) sc->last_tstmr_dur, (int32) slew_err);

  /* conv to 1/1024ppm err:  err = (slew_err / dur_sysclk) * 1024e6 */
  /* apply dyn gain:  err = err * (dur_syslk / 2^k)  */
  /* k is tech specific, (gain is 1 for ~1.25sec dur) */
  /* oombine:  err = (slew_err * 1e6) / (1 << (k-10)) */
  /* limit dyn gain to 4 when duration gets too large */
  if ( sc->last_tstmr_dur <= (1 << (sc->k + 2)))
  {
    err_ppm = (slew_err * 1000000LL) / (int64) (1 << (sc->k - 10));
  }
  else
  {
    err_ppm = (slew_err * 4096000000LL) / (int64) sc->last_tstmr_dur;
  }

  /* apply constant loop gain - 1/8 rounded correctly */
  err_ppm = err_ppm >> 2;
  err_ppm += 1;
  err_ppm = err_ppm >> 1;

  /* Add into accumulator to be used in future duration calcuations */
  sc->slew_err_accum += err_ppm;

  TRACE(id, ERR_FEEDBACK, slew_err, err_ppm, sc->slew_err_accum);

  /* Limit the accum slew error to avoid run away conditions */
  if ( sc->slew_err_accum > SLPC_MAX_ACCUM_ERR )
  {
    sc->slew_err_accum = SLPC_MAX_ACCUM_ERR;
    DBG_1(ERROR, "accum limit:  %ld/1024 ppm", (int32) sc->slew_err_accum);
  }
  else if ( sc->slew_err_accum < -SLPC_MAX_ACCUM_ERR )
  {
    sc->slew_err_accum = -SLPC_MAX_ACCUM_ERR;
    DBG_1(ERROR, "accum limit:  %ld/1024 ppm", (int32) sc->slew_err_accum);
  }

  DBG_2(HIGH, "err: %ld, accum: %ld", (int32) err_ppm, 
        (int32) sc->slew_err_accum);
}

/******************************************************************************
  @brief SLPC_START

  Get snapshot of current timer values for start of sleep cycle

  @param id: identifies the calling client
  @return start tick (TSTMR)
******************************************************************************/
slpc_tstmr_type slpc_start( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

/*--------------------------------------------------------------------------*/

  if ( sc->state == SLPC_INACTIVE_STATE )
  {
    /* Get a synchronous snapshot of current timer values */
#ifdef SLPC_VSTMR
    if( sc->stmr_mode == SLPC_STMR_VSTMR )
    {
      sc->start = slpc_vstmr_tstmr_read_start( id );
    }
    else
#endif
    {
      sc->start = slpc_hw_read_stmr_sync( id );
    }

    sc->state = SLPC_START_STATE;

    TRACE(id, START, 
          sc->start.tstmr.time, sc->start.tick, sc->start.ustmr_ts);
  }
  else
  {
    DBG_2(ERROR, "slpc_start called by %s in state %d", sc->id, sc->state );
  }

  return ( sc->start.tstmr );
}

#ifdef SLPC_VSTMR

/******************************************************************************
  @brief slpc_gsm1_vstmr_tstmr_slam
 
  Performs slam calculations for GSM1 alt ID. 
 
  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none. 
******************************************************************************/
static void slpc_gsm1_vstmr_tstmr_slam( slpc_id_type pri_id, uint64 dur )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[pri_id];

  /* the corrected ustmr value, local container for slam calculations */
  uint64 ustmr_slam, slam;

  /* local container used in slam calculations */
  slpc_dword_type tstmr_slam;

  /* for storing the raw start time */
  uint32 start;

  /* for storing the raw start time in rtc resolution (for geran) */
  uint64 start_time_in_rtc;

  /* local copy of tech dependent stmr formatted value */
  vstmr_gstmr_t vstmr_gstmr, vstmr_gstmr_res;

/*--------------------------------------------------------------------------*/

  ustmr_slam = sc->ending.ustmr_ts;
  
  vstmr_gstmr = (vstmr_gstmr_t) sc->alt_techs[SLPC_GSM].start_time;
  start_time_in_rtc = sc->alt_techs[SLPC_GSM].start_time_in_rtc;
  /* get the RTC integer */
  start = start_time_in_rtc >> 32;
  dur %= VSTMR_GERAN_RTC_MODULO;
  slam = start + dur;
  slam %= VSTMR_GERAN_RTC_MODULO;
  tstmr_slam.uw[0] = (uint32)(start_time_in_rtc & 0xFFFFFFFF);
  tstmr_slam.uw[1] = slam;
  /* GERAN has a special requirement which can allow it to change its frame
     size. Since SLPC is not aware those frame adjustments, GERAN time correction
     is done RTC resolution. */
  vstmr_rtc_sync(slpc_data.vstmr_data.gsm_view1->rtc, ustmr_slam, tstmr_slam.ud);
  vstmr_gstmr_res = vstmr_geran_get_current_gstmr( 
                    slpc_data.vstmr_data.gsm_view1 );
  TRACE(SLPC_GSM, VSMTR_READ, vstmr_gstmr.all, vstmr_gstmr_res.all, ustmr_slam); 

  DBG_4(HIGH, "vstmr slam %d, start=%d, dur=%d, slam=%d", SLPC_GSM, start, dur, slam);
  TRACE(SLPC_GSM, VSTMR_SLAM_CALC, start, dur, slam);

}

/******************************************************************************
  @brief slpc_gsm2_vstmr_tstmr_slam
 
  Performs slam calculations for GSM2 alt ID. 
 
  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none. 
******************************************************************************/
static void slpc_gsm2_vstmr_tstmr_slam( slpc_id_type pri_id, uint64 dur )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[pri_id];

  /* the corrected ustmr value, local container for slam calculations */
  uint64 ustmr_slam, slam;

  /* local container used in slam calculations */
  slpc_dword_type tstmr_slam;

  /* for storing the raw start time */
  uint32 start;

  /* for storing the raw start time in rtc resolution (for geran) */
  uint64 start_time_in_rtc;

  /* local copy of tech dependent stmr formatted value */
  vstmr_gstmr_t vstmr_gstmr, vstmr_gstmr_res;

/*--------------------------------------------------------------------------*/

  ustmr_slam = sc->ending.ustmr_ts;
  
  vstmr_gstmr = (vstmr_gstmr_t) sc->alt_techs[SLPC_GSM2].start_time;
  start_time_in_rtc = sc->alt_techs[SLPC_GSM2].start_time_in_rtc;
  /* get the RTC integer */
  start = start_time_in_rtc >> 32;
  dur %= VSTMR_GERAN_RTC_MODULO;
  slam = start + dur;
  slam %= VSTMR_GERAN_RTC_MODULO;
  tstmr_slam.uw[0] = (uint32)(start_time_in_rtc & 0xFFFFFFFF);
  tstmr_slam.uw[1] = slam;
  /* GERAN has a special requirement which can allow it to change its frame
     size. Since SLPC is not aware those frame adjustments, GERAN time correction
     is done RTC resolution. */
  vstmr_rtc_sync(slpc_data.vstmr_data.gsm_view2->rtc, ustmr_slam, tstmr_slam.ud);
  vstmr_gstmr_res = vstmr_geran_get_current_gstmr( 
                    slpc_data.vstmr_data.gsm_view2 );
  TRACE(SLPC_GSM2, VSMTR_READ, vstmr_gstmr.all, vstmr_gstmr_res.all, ustmr_slam); 

  DBG_4(HIGH, "vstmr slam %d, start=%d, dur=%d, slam=%d", SLPC_GSM2, start, dur, slam);
  TRACE(SLPC_GSM2, VSTMR_SLAM_CALC, start, dur, slam);

}

/******************************************************************************
  @brief slpc_gsm3_vstmr_tstmr_slam
 
  Performs slam calculations for GSM3 alt ID. 
 
  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none. 
******************************************************************************/
static void slpc_gsm3_vstmr_tstmr_slam( slpc_id_type pri_id, uint64 dur )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[pri_id];

  /* the corrected ustmr value, local container for slam calculations */
  uint64 ustmr_slam, slam;

  /* local container used in slam calculations */
  slpc_dword_type tstmr_slam;

  /* for storing the raw start time */
  uint32 start;

  /* for storing the raw start time in rtc resolution (for geran) */
  uint64 start_time_in_rtc;

  /* local copy of tech dependent stmr formatted value */
  vstmr_gstmr_t vstmr_gstmr, vstmr_gstmr_res;

/*--------------------------------------------------------------------------*/

  ustmr_slam = sc->ending.ustmr_ts;
  
  vstmr_gstmr = (vstmr_gstmr_t) sc->alt_techs[SLPC_GSM3].start_time;
  start_time_in_rtc = sc->alt_techs[SLPC_GSM3].start_time_in_rtc;
  /* get the RTC integer */
  start = start_time_in_rtc >> 32;
  dur %= VSTMR_GERAN_RTC_MODULO;
  slam = start + dur;
  slam %= VSTMR_GERAN_RTC_MODULO;
  tstmr_slam.uw[0] = (uint32)(start_time_in_rtc & 0xFFFFFFFF);
  tstmr_slam.uw[1] = slam;
  /* GERAN has a special requirement which can allow it to change its frame
     size. Since SLPC is not aware those frame adjustments, GERAN time correction
     is done RTC resolution. */
  vstmr_rtc_sync(slpc_data.vstmr_data.gsm_view3->rtc, ustmr_slam, tstmr_slam.ud);
  vstmr_gstmr_res = vstmr_geran_get_current_gstmr( 
                    slpc_data.vstmr_data.gsm_view3 );
  TRACE(SLPC_GSM3, VSMTR_READ, vstmr_gstmr.all, vstmr_gstmr_res.all, ustmr_slam); 

  DBG_4(HIGH, "vstmr slam %d, start=%d, dur=%d, slam=%d", SLPC_GSM3, start, dur, slam);
  TRACE(SLPC_GSM3, VSTMR_SLAM_CALC, start, dur, slam);

}

/******************************************************************************
  @brief slpc_1x_vstmr_tstmr_slam
 
  Performs slam calculations for 1X alt ID. 
 
  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none. 
******************************************************************************/
static void slpc_1x_vstmr_tstmr_slam( slpc_id_type pri_id, uint64 dur )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[pri_id];

  /* the corrected ustmr value, local container for slam calculations */
  uint64 ustmr_slam, slam;

  /* local container used in slam calculations */
  slpc_dword_type tstmr_slam;

  /* for storing the raw start time */
  uint32 start;

  /* local copy of tech dependent stmr formatted value */
  vstmr_1xframe_t vstmr_1xframe, vstmr_1xframe_res;

/*--------------------------------------------------------------------------*/

  ustmr_slam = sc->ending.ustmr_ts;
  
  vstmr_1xframe = (vstmr_1xframe_t) sc->alt_techs[SLPC_1X].start_time;
  start = vstmr_1x_frame_to_cx8( vstmr_1xframe );
  dur %= VSTMR_1X_RTC_MODULO;
  slam = start + dur;
  slam %= VSTMR_1X_RTC_MODULO;
  tstmr_slam.uw[0] = vstmr_1xframe.frac;
  tstmr_slam.uw[1] = slam;
  vstmr_1xframe = vstmr_1x_frame_segment_cnt( tstmr_slam.ud );
  vstmr_1x_frame_sync( slpc_data.vstmr_data.onex_view, vstmr_1xframe, 
                       ustmr_slam );
  vstmr_1xframe_res = vstmr_1x_get_current_frame( 
                      slpc_data.vstmr_data.onex_view );
  TRACE(SLPC_1X, VSMTR_READ, vstmr_1xframe.all, vstmr_1xframe_res.all, ustmr_slam);

  DBG_4(HIGH, "vstmr slam %d, start=%d, dur=%d, slam=%d", SLPC_1X, start, dur, slam);
  TRACE(SLPC_1X, VSTMR_SLAM_CALC, start, dur, slam);

}

/******************************************************************************
  @brief slpc_hdr_vstmr_tstmr_slam
 
  Performs slam calculations for HDR alt ID. 
 
  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none. 
******************************************************************************/
static void slpc_hdr_vstmr_tstmr_slam( slpc_id_type pri_id, uint64 dur )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[pri_id];

  /* the corrected ustmr value, local container for slam calculations */
  uint64 ustmr_slam, slam;

  /* for storing the raw start time */
  uint32 start;

  /* local copy of tech dependent stmr formatted value */
  vstmr_hstmr_t vstmr_hstmr, vstmr_hstmr_res;

/*--------------------------------------------------------------------------*/

  ustmr_slam = sc->ending.ustmr_ts;
  
  vstmr_hstmr = (vstmr_hstmr_t) sc->alt_techs[SLPC_HDR].start_time;
  start = vstmr_hstmr.cx8;
  dur %= VSTMR_HDR_RTC_MODULO;
  slam = start + dur;
  slam %= VSTMR_HDR_RTC_MODULO;
  vstmr_hstmr.cx8 = slam;
  vstmr_hdr_hstmr_sync( slpc_data.vstmr_data.hdr_view, vstmr_hstmr, 
                        ustmr_slam );
  vstmr_hstmr_res = vstmr_hdr_get_current_hstmr( 
                    slpc_data.vstmr_data.hdr_view );
  TRACE(SLPC_HDR, VSMTR_READ, vstmr_hstmr.all, vstmr_hstmr_res.all, ustmr_slam);

  DBG_4(HIGH, "vstmr slam %d, start=%d, dur=%d, slam=%d", SLPC_HDR, start, dur, slam);
  TRACE(SLPC_HDR, VSTMR_SLAM_CALC, start, dur, slam);

}

/******************************************************************************
  @brief slpc_wcdma_vstmr_tstmr_slam
 
  Performs slam calculations for WCDMA alt ID. 
 
  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none. 
******************************************************************************/
static void slpc_wcdma_vstmr_tstmr_slam( slpc_id_type pri_id, uint64 dur )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[pri_id];

  /* the corrected ustmr value, local container for slam calculations */
  uint64 ustmr_slam, slam;

  /* local container used in slam calculations */
  slpc_dword_type tstmr_slam;

  /* for storing the raw start time */
  uint32 start;

  /* local copy of tech dependent stmr formatted value */
  vstmr_wstmr_t vstmr_wstmr, vstmr_wstmr_res;

/*--------------------------------------------------------------------------*/

  ustmr_slam = sc->ending.ustmr_ts;
  
  vstmr_wstmr = (vstmr_wstmr_t) sc->alt_techs[SLPC_WCDMA].start_time;
  start = vstmr_wcdma_seg_to_cx8( vstmr_wstmr );
  dur %= VSTMR_WCDMA_RTC_MODULO;
  slam = start + dur;
  slam %= VSTMR_WCDMA_RTC_MODULO;
  tstmr_slam.uw[0] = vstmr_wstmr.frac;
  tstmr_slam.uw[1] = slam;
  vstmr_wstmr = vstmr_wcdma_segment_cnt( tstmr_slam.ud );
  vstmr_wcdma_wstmr_sync( slpc_data.vstmr_data.wcdma_view, vstmr_wstmr, 
                          ustmr_slam );
  vstmr_wstmr_res = vstmr_wcdma_get_current_wstmr( 
                    slpc_data.vstmr_data.wcdma_view );
  TRACE(SLPC_WCDMA, VSMTR_READ, vstmr_wstmr.all, vstmr_wstmr_res.all, ustmr_slam);

  DBG_4(HIGH, "vstmr slam %d, start=%d, dur=%d, slam=%d", SLPC_WCDMA, start, dur, slam);
  TRACE(SLPC_WCDMA, VSTMR_SLAM_CALC, start, dur, slam);
}

/******************************************************************************
  @brief slpc_wcdma2_vstmr_tstmr_slam
 
  Performs slam calculations for WCDMA2 alt ID. 
 
  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none. 
******************************************************************************/
static void slpc_wcdma2_vstmr_tstmr_slam( slpc_id_type pri_id, uint64 dur )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[pri_id];

  /* the corrected ustmr value, local container for slam calculations */
  uint64 ustmr_slam, slam;

  /* local container used in slam calculations */
  slpc_dword_type tstmr_slam;

  /* for storing the raw start time */
  uint32 start;

  /* local copy of tech dependent stmr formatted value */
  vstmr_wstmr_t vstmr_wstmr, vstmr_wstmr_res;

/*--------------------------------------------------------------------------*/

  ustmr_slam = sc->ending.ustmr_ts;
  
  vstmr_wstmr = (vstmr_wstmr_t) sc->alt_techs[SLPC_WCDMA2].start_time;
  start = vstmr_wcdma_seg_to_cx8( vstmr_wstmr );
  dur %= VSTMR_WCDMA_RTC_MODULO;
  slam = start + dur;
  slam %= VSTMR_WCDMA_RTC_MODULO;
  tstmr_slam.uw[0] = vstmr_wstmr.frac;
  tstmr_slam.uw[1] = slam;
  vstmr_wstmr = vstmr_wcdma_segment_cnt( tstmr_slam.ud );
  vstmr_wcdma_wstmr_sync( slpc_data.vstmr_data.wcdma_view2, vstmr_wstmr, 
                          ustmr_slam );
  vstmr_wstmr_res = vstmr_wcdma_get_current_wstmr( 
                    slpc_data.vstmr_data.wcdma_view2 );
  TRACE(SLPC_WCDMA2, VSMTR_READ, vstmr_wstmr.all, vstmr_wstmr_res.all, ustmr_slam);

  DBG_4(HIGH, "vstmr slam %d, start=%d, dur=%d, slam=%d", SLPC_WCDMA2, start, dur, slam);
  TRACE(SLPC_WCDMA2, VSTMR_SLAM_CALC, start, dur, slam);
}

/******************************************************************************
  @brief slpc_lte_vstmr_tstmr_slam
 
  Performs slam calculations for LTE alt ID. 
 
  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none. 
******************************************************************************/
static void slpc_lte_vstmr_tstmr_slam( slpc_id_type pri_id, uint64 dur )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[pri_id];

  /* the corrected ustmr value, local container for slam calculations */
  uint64 ustmr_slam, slam;

  /* for storing the raw start time */
  uint32 start;

  /* local copy of tech dependent stmr formatted value */
  vstmr_ostmr_t vstmr_ostmr, vstmr_ostmr_res;

/*--------------------------------------------------------------------------*/

  ustmr_slam = sc->ending.ustmr_ts;
  
  vstmr_ostmr = (vstmr_ostmr_t) sc->alt_techs[SLPC_LTE].start_time;
  start = vstmr_ostmr.ostmr_cnt;
  dur %= VSTMR_LTE_RTC_MODULO;
  slam = start + dur;
  slam %= VSTMR_LTE_RTC_MODULO;
  vstmr_ostmr.ostmr_cnt = slam;
  vstmr_lte_ostmr_sync( slpc_data.vstmr_data.lte_view, vstmr_ostmr, 
                        ustmr_slam );
  vstmr_ostmr_res = vstmr_lte_get_current_ostmr( 
                    slpc_data.vstmr_data.lte_view );
  TRACE(SLPC_LTE, VSMTR_READ, vstmr_ostmr.all, vstmr_ostmr_res.all, ustmr_slam);

  DBG_4(HIGH, "vstmr slam %d, start=%d, dur=%d, slam=%d", SLPC_LTE, start, dur, slam);
  TRACE(SLPC_LTE, VSTMR_SLAM_CALC, start, dur, slam);
}

/******************************************************************************
  @brief slpc_tdscdma_vstmr_tstmr_slam
 
  Performs slam calculations for TDSCDMA alt ID. 
 
  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none. 
******************************************************************************/
static void slpc_tdscdma_vstmr_tstmr_slam( slpc_id_type pri_id, uint64 dur )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[pri_id];

  /* the corrected ustmr value, local container for slam calculations */
  uint64 ustmr_slam, slam;

  /* local container used in slam calculations */
  slpc_dword_type tstmr_slam;

  /* for storing the raw start time */
  uint32 start;

  /* local copy of tech dependent stmr formatted value */
  vstmr_tdsstmr_t vstmr_tdsstmr, vstmr_tdsstmr_res;

/*--------------------------------------------------------------------------*/

  ustmr_slam = sc->ending.ustmr_ts;
  
  vstmr_tdsstmr = (vstmr_tdsstmr_t) sc->alt_techs[SLPC_TDSCDMA].start_time;
  start = vstmr_tds_seg_to_cx8( vstmr_tdsstmr );
  dur %= VSTMR_TDS_RTC_MODULO;
  slam = start + dur;
  slam %= VSTMR_TDS_RTC_MODULO;
  tstmr_slam.uw[0] = vstmr_tdsstmr.frac;
  tstmr_slam.uw[1] = slam;
  vstmr_tdsstmr = vstmr_tds_segment_cnt( tstmr_slam.ud );
  vstmr_tds_stmr_sync( slpc_data.vstmr_data.tdscdma_view, vstmr_tdsstmr, 
                       ustmr_slam );
  vstmr_tdsstmr_res = vstmr_tds_get_current_stmr( 
                      slpc_data.vstmr_data.tdscdma_view );
  TRACE(SLPC_TDSCDMA, VSMTR_READ, vstmr_tdsstmr.all, vstmr_tdsstmr_res.all, ustmr_slam);

  DBG_4(HIGH, "vstmr slam %d, start=%d, dur=%d, slam=%d", SLPC_TDSCDMA, start, dur, slam);
  TRACE(SLPC_TDSCDMA, VSTMR_SLAM_CALC, start, dur, slam);
}

/******************************************************************************
  @brief SLPC_VSTMR_SLAM
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none. 
******************************************************************************/
void slpc_vstmr_slam( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* For holding the converted duration for the alternate technologies */
  uint64 alt_dur;

  int i;

/*--------------------------------------------------------------------------*/

  /* slam the primary tech */
  sc->vstmr_tstmr_slam_func( id, sc->tstmr_dur );

  TRACE(id, VSTMR_SLAM, sc->tstmr_dur, 0, 0);

  for ( i = 0; i < SLPC_NUM_CLIENTS; i++ )
  {
    if ( sc->alt_techs[i].valid )
    {
      /* calculate the alternate duration */
      alt_dur = (uint64) (sc->tstmr_dur * slpc[i].system_freq);
      alt_dur = (alt_dur + (sc->system_freq >> 1)) / sc->system_freq;

      TRACE(id, VSTMR_SLAM, i, alt_dur, 0);

      /* slam the alternate tech */
      slpc[i].vstmr_tstmr_slam_func( id, alt_dur );
    }
  }
}

#endif

/******************************************************************************
  @brief SLPC_SET_DURATION_AND_WARMUP
  Sets the sleep duration (in TSTMR ticks) and programs the wakeup timer

  @detail
  IF HW TSTMR: The warmup period is subtracted from the sleep duration 
               (ending/OLS) to determine the wakeup point.
  IF VSTMR: RTC is "slammed" at commit_wakeup

  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in tstmr ticks
  @param dur_usec: warmup length, in usec
  @return slpc_err_type
******************************************************************************/
slpc_err_type slpc_set_duration_and_warmup( slpc_id_type id, uint64 tstmr_dur, 
                                            uint16 dur_usec )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* return value */
  slpc_err_type err_code = SLPC_ERR_NO_ERR;

/*--------------------------------------------------------------------------*/

  /* must first do slpc_start, cannot be past wakeup complete */
  if ( (sc->state == SLPC_INACTIVE_STATE) || (sc->state == SLPC_WARMUP_STATE) )
  {
    /* Get a synchronous snapshot of current timer values */
    DBG_2(ERROR, "slpc_set_duration_and_warmup called by %s in state %d", 
          sc->id, sc->state );
  }
  else
  {
    ENTER_TECH_CRIT_SECTION(id);

    /* Calculate ustmr duration */
    sc->tstmr_dur = tstmr_dur;
    sc->ustmr_dur = (ustmr_type) slpc_tstmr_to_ustmr( id, tstmr_dur );
    sc->ending.tick = sc->start.tick + sc->ustmr_dur;
    sc->ending.ustmr_ts = sc->ending.tick & SLPC_TIMETICK_LOWER_MASK;

    /* Update warmup and wakeup based on new duration */
    if ( sc->warmup.dur_usec != dur_usec )
    {
      sc->warmup.dur_usec = dur_usec;
      sc->warmup.dur_tick = (19200000ULL * sc->warmup.dur_usec) / 1000000ULL;
    }

    TRACE(id, SET_DURATION, sc->ending.tick, sc->tstmr_dur, sc->ustmr_dur);
    TRACE(id, SET_WARMUP, sc->warmup.dur_usec, sc->warmup.dur_tick, 0xFFFF);

    /* Update the wakeup point based on the new sleep duration/warmup */
    err_code = slpc_update_wakeup_point(id);

    LEAVE_TECH_CRIT_SECTION(id);

    /* notify anyone else about this technology dur/wakeup update */
    slpc_call_notify(id);

    DBG_3(MED, "Set duration and warmup: id=%s, wake=%lu, ols=%lu", 
          sc->id, (uint32) sc->wakeup.tick, (uint32) sc->ending.tick);

    /* Enter sleep state */
    sc->state = SLPC_SLEEP_STATE;
  }

  return err_code;
}

/******************************************************************************
  @brief SLPC_SET_DURATION

  Sets the sleep duration (in TSTMR ticks) and programs the wakeup timer

  @detail 
  IF HW TSTMR: The warmup period is subtracted from the sleep duration 
               to determine the wakeup point.
  IF VSTMR: warmup time does not matter, RTC is "slammed" at wakeup

  @param id: identifies the calling client
  @param tstmr_dur: the duration of sleep requested, in tstmr ticks
  @return slpc_err_type
******************************************************************************/
slpc_err_type slpc_set_duration( slpc_id_type id, uint64 tstmr_dur )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* return value */
  slpc_err_type err_code = SLPC_ERR_NO_ERR;

/*--------------------------------------------------------------------------*/

  /* must first do slpc_start, cannot be past wakeup complete */
  if ( (sc->state == SLPC_INACTIVE_STATE) || (sc->state == SLPC_WARMUP_STATE) )
  {
    /* Get a synchronous snapshot of current timer values */
    DBG_2(ERROR, "slpc_set_duration called by %s in state %d", 
          sc->id, sc->state );
  }
  else
  {
    ENTER_TECH_CRIT_SECTION(id);

    /* Calculate ustmr duration */
    sc->tstmr_dur = tstmr_dur;
    sc->ustmr_dur = (ustmr_type) slpc_tstmr_to_ustmr( id, tstmr_dur );
    sc->ending.tick = sc->start.tick + sc->ustmr_dur;
    sc->ending.ustmr_ts = sc->ending.tick & SLPC_TIMETICK_LOWER_MASK;

    TRACE(id, SET_DURATION, sc->ending.tick, sc->tstmr_dur, sc->ustmr_dur);

    /* Update the wakeup point based on the new sleep duration */
    err_code = slpc_update_wakeup_point(id);

    LEAVE_TECH_CRIT_SECTION(id);

    /* notify anyone else about this technology dur/wakeup update */
    slpc_call_notify(id);

    DBG_3(MED, "Set duration: id=%s, wake=%lu, ols=%lu", 
          sc->id, (uint32) sc->wakeup.tick, (uint32) sc->ending.tick);

    /* Enter sleep state */
    sc->state = SLPC_SLEEP_STATE;
  }

  return err_code;
}

/******************************************************************************
  @brief SLPC_SET_WARMUP

  Set the warmup period. This determines amount of time prior to the 
  ending point that the wakeup callback should occur.

  @param id: identifies the calling client
  @param warmup_usec: warmup period in usec
  @return slpc_err_type
******************************************************************************/
slpc_err_type slpc_set_warmup( slpc_id_type id, uint16 dur_usec )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* return value */
  slpc_err_type err_code = SLPC_ERR_NO_ERR;

/*--------------------------------------------------------------------------*/

  /* Convert microseconds to slow clocks only if warmup has changed */
  if ( sc->warmup.dur_usec != dur_usec )
  {
    ENTER_TECH_CRIT_SECTION(id);

    sc->warmup.dur_usec = dur_usec;
    sc->warmup.dur_tick = (19200000ULL * sc->warmup.dur_usec) / 1000000ULL;

    TRACE(id, SET_WARMUP, sc->warmup.dur_usec, sc->warmup.dur_tick, 0);

    /* Only update the wakeup point if we're SLEEP */
    if ( sc->state == SLPC_SLEEP_STATE )
    {
      /* Update the wakeup point based on the new warmup time */
      err_code = slpc_update_wakeup_point(id);

      LEAVE_TECH_CRIT_SECTION(id);

      /* notify anyone else about this technology dur/wakeup update */
      slpc_call_notify(id);
    }
    else
    {
      LEAVE_TECH_CRIT_SECTION(id);
    }
  }

  return err_code;
}

/******************************************************************************
  @brief SLPC_SET_NOTIFY_CALLBACK
  Set a callbacks for anyone wanting to track wakeup point changes, 
  called whenever any tech sets a wakeup point

  @param n_cb: callback
  @return none
******************************************************************************/
void slpc_set_notify_callback( slpc_notify_cb_type n_cb )
{
  uint8 i = 0;

/*--------------------------------------------------------------------------*/

  ENTER_GLOBAL_CRIT_SECTION();
  while ( slpc_data.notify_cb[i++] != NULL );
  i--;

  if ( i < SLPC_MAX_WAKEUP_CALLBACKS )
  {
    /* Save the notify callback */
    slpc_data.notify_cb[i] = n_cb;
  }
  else
  {
    ERR_FATAL("no room for notify callback allocation", 0, 0, 0); 
  }
  LEAVE_GLOBAL_CRIT_SECTION();

}

/******************************************************************************
  @brief SLPC_SET_NOTIFY_WAKEUP_CALLBACK

  This callback will be called at any tech's wakeup event. For anyone
  wanting to track wakeups.

  @param id: n_cb: notify callback

  @return none. 
******************************************************************************/
void slpc_set_notify_wakeup_callback( slpc_notify_wakeup_cb_type n_cb )
{
  uint8 i = 0;

/*--------------------------------------------------------------------------*/

  ENTER_GLOBAL_CRIT_SECTION();
  while ( slpc_data.notify_wakeup_cb[i++] != NULL );
  i--;

  if ( i < SLPC_MAX_WAKEUP_CALLBACKS )
  {
    /* Save the notify callback */
    slpc_data.notify_wakeup_cb[i] = n_cb;
  }
  else
  {
    ERR_FATAL("no room for wakeup notify callback allocation", 0, 0, 0); 
  }
  LEAVE_GLOBAL_CRIT_SECTION();
}

/******************************************************************************
  @brief SLPC_DEREGISTER_NOTIFY_CALLBACK

  Clear a previously set notify callback

  @param n_cb: callback
  @return none
******************************************************************************/
void slpc_deregister_notify_callback( slpc_notify_cb_type n_cb )
{
  uint8 i;

/*--------------------------------------------------------------------------*/

  for ( i = 0; i < SLPC_MAX_WAKEUP_CALLBACKS; i++ )
  {
    if ( slpc_data.notify_cb[i] == n_cb )
    {
      slpc_data.notify_cb[i] = NULL;
    }
  }
}

/******************************************************************************
  @brief SLPC_DEREGISTER_NOTIFY_WAKEUP_CALLBACK

  Deregister the wakeup callback, if previously set

  @param id: n_cb: notify callback

  @return none. 
******************************************************************************/
void slpc_deregister_notify_wakeup_callback( slpc_notify_wakeup_cb_type n_cb )
{
  uint8 i;

/*--------------------------------------------------------------------------*/

  for ( i = 0; i < SLPC_MAX_WAKEUP_CALLBACKS; i++ )
  {
    if ( slpc_data.notify_wakeup_cb[i] == n_cb )
    {
      slpc_data.notify_wakeup_cb[i] = NULL;
    }
  }
}

/******************************************************************************
  @brief SLPC_SET_WAKEUP_CALLBACK

  Sets the function to call at the wakeup event. Wakeup event will happen
  at (ending point - warmup time). In this callback, wakeup can either be
  extended (set_duration), or commited (commit_wakeup, at which point it
  cannot be changed).

  @param id: identifies the calling client
  @param e_cb: event callback
  @return none
******************************************************************************/
void slpc_set_wakeup_callback( slpc_id_type id, slpc_event_callback_type w_cb )
{
  /* Save the wakeup event callback */
  slpc[id].wakeup.cb = w_cb;

}

/******************************************************************************
  @brief SLPC_SET_OLS_CALLBACK

  Set the callback to call when the slpc ending point (online start) is
  reached.

  Note this callback is not used with VSTMR, because there is no hardware
  to "slam", so everything is ready after complete_wakeup.

  @param id: identifies the calling client
  @param e_cb: event callback
  @return none
******************************************************************************/
void slpc_set_ols_callback( slpc_id_type id, slpc_event_callback_type e_cb )
{
  /* Save the ols event callback */
  slpc[id].ending.cb = e_cb;

}

/******************************************************************************
  @brief SLPC_WAKEUP_EVENT

  Wakeup triggered by core timer (ending point - warmup time). Call the
  client wakeup callbacks. Clients can either extend sleep or commit
  to wakeup.

  @param id: identifies the calling client
  @return none
******************************************************************************/
void slpc_wakeup_event( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

/*--------------------------------------------------------------------------*/

  ENTER_TECH_CRIT_SECTION(id);

  if ( sc->state != SLPC_SLEEP_STATE )
  {
    LEAVE_TECH_CRIT_SECTION(id);
    sc->wakeup.invalid_cnt++;
    DBG_2(ERROR, "Wakeup in wrong state %d, cnt=%d", sc->state, 
          sc->wakeup.invalid_cnt);
    return;
  }

  /* Capture the timestamp when the wakeup ISR starts */
  sc->wakeup.cb_start_tick = slpc_get_tick();

  TRACE(id, WAKEUP_START, sc->state, sc->wakeup.tick, sc->wakeup.cb_start_tick);

  /* Reasonable wakeup? */
  if ( (sc->wakeup.cb_start_tick + SLPC_WAKEUP_TIMER_ERR) >= sc->wakeup.tick )
  {
    /* Update the sleep controller's software state to "wakeup" state */
    sc->state = SLPC_WAKEUP_STATE;

    LEAVE_TECH_CRIT_SECTION(id);

    /* Invoke the wakeup callback */
    if (sc->wakeup.cb != NULL )
    {
      sc->wakeup.cb();
    }
  }
  else
  {
    LEAVE_TECH_CRIT_SECTION(id);
    DBG_3(ERROR, "%s Wakeup ignored!  now=%lu, wake=%lu", sc->id, 
          (uint32) sc->wakeup.cb_start_tick, (uint32) sc->wakeup.tick);
  }

  /* Capture the timestamp when the wakeup ISR ends */
  sc->wakeup.cb_end_tick = slpc_get_tick();

  /* notify anyone else about this technology wakeup event */
  slpc_call_notify_wakeup(id);

  DBG_3(MED, "Wakeup event: %s, start=%lu, end=%lu", sc->id, 
        (uint32) sc->wakeup.cb_start_tick, (uint32) sc->wakeup.cb_end_tick);

  TRACE(id, WAKEUP_END, sc->wakeup.tick,
          sc->wakeup.cb_start_tick, sc->wakeup.cb_end_tick); 
}

/******************************************************************************
  @brief SLPC_SUFFICIENT_WARMUP_TIME

  May be called by the wakeup_cb to determine whether or not the wakeup
  event is being processed too late.

  @param id: identifies the calling client
  @return boolean
******************************************************************************/
boolean slpc_sufficient_warmup_time ( slpc_id_type id, uint32 min_warmup )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* Current timetick */
  uint64 tick;

  /* convert from usec to xo ticks */
  int64 min_ticks = (min_warmup * 19200000uLL) / 1000000uLL;

/*--------------------------------------------------------------------------*/

  tick = slpc_get_tick();
  if ( ( (int64) sc->ending.tick - (int64) tick) > min_ticks ) return TRUE;

  return FALSE;
}

/******************************************************************************
  @brief SLPC_COMPLETE_WAKEUP

  May be called by the wakeup_cb to commit to the wakeup if there is
  sufficient warmup time remaining.

  @param id: identifies the calling client
  @param min_warmup: min required warmup time (usec)
  @param tstmr: pointer to TSTMR for SLPC to program, or NULL if client
                will handle this
  @return FALSE if not enough time or no HW resources available
  @return TRUE if succeeded
******************************************************************************/
boolean slpc_complete_wakeup ( slpc_id_type id, uint32 min_warmup, 
                               slpc_tstmr_type *tstmr )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* Current timetick */
  uint64 tick;

  /* Current relative location to ending tick */
  int64 delta;

  /* convert from usec to xo ticks */
  int64 min_ticks = (min_warmup * 19200000uLL) / 1000000uLL;

/*--------------------------------------------------------------------------*/

#ifdef SLPC_VSTMR
  if ( sc->stmr_mode == SLPC_STMR_VSTMR )
  {
    /* virtually slam the tech stmr */
    slpc_vstmr_slam( id );
    sc->state = SLPC_INACTIVE_STATE;
    TRACE(id, COMPLETE_WAKEUP, min_warmup, 0, 0);
    DBG_0(HIGH, "slpc_complete_wakeup called");
    /* Record the sleep duration, for error_feedback */
    sc->last_tstmr_dur = sc->tstmr_dur;
    return TRUE;
  }
  else
#endif
  {
    /* SLPC task priority only needs elevation for wakeup event */
    slpc_set_task_pri( id, SLPC_LOW_PRI );

    if (!slpc_acquire_resource( id ))
    {
      TRACE(id, COMPLETE_WAKEUP, 0xFFFFFFFF, sc->ending.tick, min_ticks);
      return FALSE;
    }

    ENTER_TECH_CRIT_SECTION(id);

    tick = slpc_get_tick();
    delta = (int64) sc->ending.tick - (int64) tick;

    if ( delta > min_ticks )
    {
      if ( sc->state != SLPC_WAKEUP_STATE )
      {
        ERR_FATAL("Complete wakeup, %d in wrong state=%d!", id, sc->state, 0);
      }

      /* Program new sleep ending point, taking into account h/w overhead */
      /* Multiple techs should not be slamming the same tstmrs, so don't
       * need to lock globally */
      slpc_hw_write_stmr_sync( id, sc->ending.ustmr_ts, tstmr );

      /* Capture the timetick when the wakeup is committed to. */
      sc->warmup.tick = tick; 

      /* Update the sleep controller's software state to "warmup" */
      sc->state = SLPC_WARMUP_STATE;

      LEAVE_TECH_CRIT_SECTION(id);
      DBG_3(MED, "Complete wakeup: %s, warmup_ts=%lu, ols_ts=%lu", 
            sc->id, (uint32) sc->warmup.tick, (uint32) sc->ending.tick);
      TRACE(id, COMPLETE_WAKEUP, tick, sc->ending.tick, min_ticks);

      return TRUE;
    }
    else
    {
      LEAVE_TECH_CRIT_SECTION(id);
      slpc_release_resource( id );
      DBG_3(ERROR, "Complete wakeup: not enough time, %s, now=%lu, ols_ts=%lu", 
            sc->id, (uint32) tick, (uint32) sc->ending.tick);
      TRACE(id, COMPLETE_WAKEUP, tick, sc->ending.tick, min_ticks);

      return FALSE;
    }
  }
}

/******************************************************************************
  @brief SLPC_ABORT

  Stop the current sleep cycle and reset the sleep controller state

  @param id: identifies the calling client
  @return none. 
******************************************************************************/
void slpc_abort ( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

/*--------------------------------------------------------------------------*/

  TRACE(id, ABORT, sc->state, sc->wakeup.tick, 0);

  ENTER_TECH_CRIT_SECTION(id);

  /* Record the sleep duration, for error_feedback */
  sc->last_tstmr_dur = sc->tstmr_dur;

  if ( (sc->state == SLPC_START_STATE) ||
       (sc->state == SLPC_SLEEP_STATE) )
  {
    timer_clr( &sc->wakeup.timer, T_NONE );
  }
  else if ( sc->state == SLPC_WARMUP_STATE )
  {
    /* Note: state will never be WARMUP unless running in HW-based
     * STMR mode (should never get here with VSTMR) */

    /* Disable the ustmr interrupt */
    SLPC_USTMR_DIS_INT( sc->slpc_ustmr_rsrc );

    /* already setup OLS, need to tear it down */
    slpc_release_resource( id );
  }

  /* Update the sleep controller's software state to "inactive" state */
  sc->state = SLPC_INACTIVE_STATE;

  LEAVE_TECH_CRIT_SECTION(id);
}

/******************************************************************************
  @brief SLPC_OLS_EVENT
 
  Technology online start event - RTC slam. Only applicable if tech is using
  HW-based STMR, not VSTMR
 
  @param id: identifies the calling client
  @return none. 
******************************************************************************/
static void slpc_ols_event( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

/*--------------------------------------------------------------------------*/

  if ( sc->state != SLPC_WARMUP_STATE )
  {
    sc->ending.invalid_cnt++;
    DBG_2(ERROR, "OLS in wrong state %d, cnt=%d", sc->state, 
          sc->ending.invalid_cnt);
    return;
  }

  /* Capture the timestamp when the ols ISR starts */
  sc->ending.isr_start_tick = slpc_get_tick();

  /* If this ISR is excessively latent, print error */
  if ( sc->ending.isr_start_tick - sc->ending.tick >= ISR_LATENCY_THRESH ) 
  {
    DBG_2(ERROR, "%s ols event, latency: %ld ticks", sc->id,
          (int32) sc->ending.isr_start_tick - sc->ending.tick );
  }
  else
  {
    DBG_2(MED, "%s ols event, latency: %ld ticks", sc->id,
          (int32) sc->ending.isr_start_tick - sc->ending.tick );
  }

  /* Disable the ustmr interrupt */
  SLPC_USTMR_DIS_INT( sc->slpc_ustmr_rsrc );

  /* Update the sleep controller's software state to "inactive" state */
  sc->state = SLPC_INACTIVE_STATE;

  /* Record the sleep duration, for error_feedback */
  sc->last_tstmr_dur = sc->tstmr_dur;

  slpc_release_resource( id );

  TRACE(id, OLS_START, sc->state, sc->ending.tick, sc->ending.isr_start_tick);

  /* Invoke the ols callback */
  if (sc->ending.cb != NULL )
  {
    sc->ending.cb();
  }

  /* Record the timestamp of the end of this ISR */
  sc->ending.isr_end_tick = slpc_get_tick();

  TRACE(id, OLS_END, sc->ending.tick, sc->ending.isr_start_tick, 
        sc->ending.isr_end_tick);
}

/******************************************************************************
  @brief SLPC_TASK_COMMON

  SLPC task function -- Initialize and loop forever

  @param tech: tech id
  @return none. 
******************************************************************************/
void slpc_task_common( slpc_id_type id )
{
  /* received signal */
  unsigned int sigs;

  /* qurt_thread_get_priority retval */
  int32 pri;

/*--------------------------------------------------------------------------*/

  rcinit_handshake_startup();

  /* save task info for wakeup dynamic priority change */
  slpc[id].thread_id = qurt_thread_get_id();
  pri = qurt_thread_get_priority( slpc[id].thread_id );
  if( pri < 0 )
  {
    ERR_FATAL("Priority init failed", pri, slpc[id].thread_id, 0);
  }
  slpc[id].thread_cur_pri = (uint16) pri;
  slpc[id].thread_def_pri = (uint16) pri;

  while( TRUE )
  {
    sigs = qurt_signal_wait_any( &slpc[id].sig, SLPC_ALL_SIGS );
    qurt_signal_clear( &slpc[id].sig, sigs );
    if( sigs & SLPC_WAKEUP_SIG )
    {
      slpc_wakeup_event( id );
    }
    if( sigs & SLPC_OLS_SIG )
    {
      slpc_ols_event( id );
    }
  }
}

/******************************************************************************
  @brief SLPC_TASK 1/9 - GSM

  SLPC task function for GSM, loop forever

  @param unused: unused.
  @return none. 
******************************************************************************/
void slpc_gsm_task( dword unused )
{
  slpc_task_common( SLPC_GSM );
}

/******************************************************************************
  @brief SLPC_TASK 2/9 - 1x

  SLPC task function for 1x, loop forever

  @param unused: unused.
  @return none. 
******************************************************************************/
void slpc_1x_task( dword unused )
{
  slpc_task_common( SLPC_1X );
}

/******************************************************************************
  @brief SLPC_TASK 3/9 - HDR

  SLPC task function for HDR, loop forever

  @param unused: unused.
  @return none. 
******************************************************************************/
void slpc_hdr_task( dword unused )
{
  slpc_task_common( SLPC_HDR );
}

/******************************************************************************
  @brief SLPC_TASK 4/9 - WCDMA

  SLPC task function for WCDMA, loop forever

  @param unused: unused.
  @return none. 
******************************************************************************/
void slpc_wcdma_task( dword unused )
{
  slpc_task_common( SLPC_WCDMA );
}

/******************************************************************************
  @brief SLPC_TASK 5/9 - WCDMA2

  SLPC task function for WCDMA, loop forever

  @param unused: unused.
  @return none. 
******************************************************************************/
void slpc_wcdma2_task( dword unused )
{
  slpc_task_common( SLPC_WCDMA2 );
}

/******************************************************************************
  @brief SLPC_TASK 6/9 - LTE

  SLPC task function for LTE, loop forever

  @param unused: unused.
  @return none. 
******************************************************************************/
void slpc_lte_task( dword unused )
{
  slpc_task_common( SLPC_LTE );
}

/******************************************************************************
  @brief SLPC_TASK 7/9 - TDSCDMA

  SLPC task function for TDSCDMA, loop forever

  @param unused: unused.
  @return none. 
******************************************************************************/
void slpc_tds_task( dword unused )
{
  slpc_task_common( SLPC_TDSCDMA );
}

/******************************************************************************
  @brief SLPC_TASK 8/9 - GSM2

  SLPC task function for GSM2, loop forever

  @param unused: unused.
  @return none. 
******************************************************************************/
void slpc_gsm2_task( dword unused )
{
  slpc_task_common( SLPC_GSM2 );
}

/******************************************************************************
  @brief SLPC_TASK 9/9 - GSM3

  SLPC task function for GSM3, loop forever

  @param unused: unused.
  @return none. 
******************************************************************************/
void slpc_gsm3_task( dword unused )
{
  slpc_task_common( SLPC_GSM3 );
}

/******************************************************************************
  @brief SLPC_INIT
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @see 
  @return none. 
******************************************************************************/
void slpc_init( void )
{
  /* loop variable */
  uint8 i, id;

  /* make sure initialization is done only once */
  static boolean slpc_initialized = FALSE;

/*--------------------------------------------------------------------------*/

  if ( slpc_initialized )
  {
    return;
  }

  slpc_initialized = TRUE;

  INIT_GLOBAL_CRIT_SECTION();

  /* create qurt signals for communication with tasks */
  qurt_signal_init( &slpc[SLPC_GSM].sig );
  qurt_signal_init( &slpc[SLPC_1X].sig );
  qurt_signal_init( &slpc[SLPC_HDR].sig );
  qurt_signal_init( &slpc[SLPC_WCDMA].sig );
  qurt_signal_init( &slpc[SLPC_WCDMA2].sig );
  qurt_signal_init( &slpc[SLPC_LTE].sig );
  qurt_signal_init( &slpc[SLPC_TDSCDMA].sig );
  qurt_signal_init( &slpc[SLPC_GSM2].sig );
  qurt_signal_init( &slpc[SLPC_GSM3].sig );

  if( ( DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, 
                        &slpc_data.int_handle) != DAL_SUCCESS ) || 
      ( slpc_data.int_handle == NULL ) )
  {
    ERR_FATAL("Cannot attach to int ctrl driver!", 0, 0, 0); 
  }

  if( ( DalTimetick_Attach("SystemTimer", &slpc_data.tt_handle) 
        != DAL_SUCCESS ) || ( slpc_data.tt_handle == NULL ) )
  {
    ERR_FATAL("Cannot attach to timetick driver!", 0, 0, 0); 
  }

  slpc_trace_init();

  slpc_c_fam = DalChipInfo_ChipFamily();
  slpc_c_ver = DalChipInfo_ChipVersion();

  slpc[SLPC_GSM].route_mask = HWIO_SYS_ONLINE_CTRLn_GSM_EN_BMSK;
  slpc[SLPC_1X].route_mask = HWIO_SYS_ONLINE_CTRLn_SYS_1X_EN_BMSK;
  slpc[SLPC_HDR].route_mask = HWIO_SYS_ONLINE_CTRLn_DO_EN_BMSK;
  slpc[SLPC_WCDMA].route_mask = HWIO_SYS_ONLINE_CTRLn_UMTS_EN_BMSK;
  slpc[SLPC_WCDMA2].route_mask = HWIO_SYS_ONLINE_CTRLn_UMTS_EN_BMSK;
  slpc[SLPC_LTE].route_mask = HWIO_SYS_ONLINE_CTRLn_LTE_EN_BMSK;
  slpc[SLPC_TDSCDMA].route_mask = HWIO_SYS_ONLINE_CTRLn_TDS_EN_BMSK;
  slpc[SLPC_GSM2].route_mask = HWIO_SYS_ONLINE_CTRLn_GSM_EN_G1_BMSK;
  slpc[SLPC_GSM3].route_mask = HWIO_SYS_ONLINE_CTRLn_GSM_EN_G2_BMSK;
  slpc_num_rsrc = HWIO_SYS_ONLINE_CTRLn_MAXn + 1;

  ASSERT( slpc_num_rsrc <= SLPC_MAX_RESOURCES );

  for ( i = 0; i < slpc_num_rsrc; i++ )
  {
    if( DalInterruptController_RegisterISR( slpc_data.int_handle, 
            slpc_data.slpc_ustmr_rsrc[i].int_number, 
            slpc_data.slpc_ustmr_rsrc[i].isr_function,
            NULL, 
            slpc_data.slpc_ustmr_rsrc[i].int_trigger) != DAL_SUCCESS )
    {
      ERR_FATAL("Cannot register isr on rsrc %d", i, 0, 0); 
    }
  }

  for ( id = 0; id < SLPC_NUM_CLIENTS; id++ )
  {
    INIT_TECH_CRIT_SECTION(id);

    timer_group_set_deferrable(&slpc[id].wakeup.slpc_timer_group, FALSE);
    timer_def_priority( &slpc[id].wakeup.timer, &slpc[id].wakeup.slpc_timer_group, 
               NULL, 0, (timer_t1_cb_type) &slpc_wakeup_timer_cb, 
               (timer_cb_data_type) id, TIMER_PRIORITY_0);

    /* Initialize the sleep controller's state */
    slpc[id].state = SLPC_INACTIVE_STATE;
    slpc[id].slew_err_accum = 0;
    slpc[id].wakeup.invalid_cnt = 0;
    slpc[id].ending.invalid_cnt = 0;
    slpc[id].slpc_ustmr_rsrc = SLPC_NO_RESOURCE;
    slpc[id].warmup.dur_tick = 0;
    slpc[id].warmup.dur_usec = 0;
#ifdef SLPC_VSTMR
    slpc[id].stmr_mode = SLPC_STMR_VSTMR;
#else
    slpc[id].stmr_mode = SLPC_STMR_HW;
#endif

    for ( i = 0; i < SLPC_NUM_CLIENTS; i++ )
    {
      slpc[id].alt_techs[i].valid = FALSE;
      slpc[id].alt_techs[i].tstmr = 0;
    }
  }

#ifdef SLPC_VSTMR
  /* always assume VSTMR by default */
  slpc_data.vstmr_data.gsm_view1 = 
    vstmr_geran_get_view_handle(VSTMR_RTC_GERAN_SUB0, VSTMR_GERAN_GSTMR_VIEW);
  slpc_data.vstmr_data.gsm_view2 = 
    vstmr_geran_get_view_handle(VSTMR_RTC_GERAN_SUB1, VSTMR_GERAN_GSTMR_VIEW);
  slpc_data.vstmr_data.gsm_view3 = 
    vstmr_geran_get_view_handle(VSTMR_RTC_GERAN_SUB1, VSTMR_GERAN_GSTMR_VIEW);
  slpc_data.vstmr_data.wcdma_view = 
    vstmr_wcdma_get_view_handle(VSTMR_RTC_WCDMA_SUB0, VSTMR_WCDMA_REF_COUNT_VIEW);
  slpc_data.vstmr_data.wcdma_view2 = 
    vstmr_wcdma_get_view_handle(VSTMR_RTC_WCDMA_SUB1, VSTMR_WCDMA_REF_COUNT_VIEW);
  slpc_data.vstmr_data.onex_view = 
    vstmr_1x_get_view_handle(VSTMR_RTC_1X_SUB0, VSTMR_1X_RTC_VIEW);
  slpc_data.vstmr_data.hdr_view = 
    vstmr_hdr_get_view_handle(VSTMR_RTC_HDR_SUB0, VSTMR_HDR_RTC_VIEW);
  slpc_data.vstmr_data.lte_view = 
    vstmr_lte_get_view_handle(VSTMR_RTC_LTE_SUB0, VSTMR_LTE_OSTMR_VIEW);
  slpc_data.vstmr_data.tdscdma_view = 
    vstmr_tds_get_view_handle(VSTMR_RTC_TDS_SUB0, VSTMR_TDS_WALL_TIME_VIEW);
#endif
}

