/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  D H C P _ C L I E N T _ T I M E R . C

GENERAL DESCRIPTION
  DMSS Dynamic Host Configuration Protocol client timer code.  This
  file provides the shim layer between the core and the timer services
  routines in both the DHCPv4 and DHCPv6 clients.

  See RFC 3315, Section 14 for DHCPv6 timer retransmission algorithms.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2004-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/dhcp_client_timer.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/09/14    ash    Added Prefix Delegation support.
01/10/14    vl     Optimization of F3 messages
09/12/13    sd     Cryptographic vulnerability - replacing ran_next APIs.
12/12/12    ds     Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR.
01/11/10    ss     Lint warning fixes.
09/26/08    am     Changed malloc/free names for off target support.
06/15/06    es     Added DHCPv6 functionality.
11/05/05    rt     Fixed coverity errors.
08/18/05    clp    Call callback, not set timer if delta is zero.
03/21/05    ssh    Changes pursuant to the new typedef ps_timer_handle_type
10/27/04    clp    Initial development work done.
12/30/04    clp    Code review: static functions now have dhcpi_ prefix
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "dhcp_client_timer.h"
#include "pstimer.h"
#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "ran.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_utils.h"

#define DHCP_CLIENT_MS_PER_S (1000)

/* DHCP6_CLIENT_RAND_GRANULARITY should be less than 2^15
 * because it is passed into rand_dist as a 16 bit parameter */
#define DHCP6_CLIENT_RAND_GRANULARITY (1000)

#define DHCP6_CLIENT_ZERO_POINT_ONE_DIVISOR (10)

typedef struct dhcp_client_timer_info {
  /* cookies: The cookie_ptr takes a value of a pointer to the
   * beginning of this structure. This happens when the shim is
   * started.
   */
  struct dhcp_client_timer_info * cookie_ptr;

  /* The cookie takes the value below when allocated in _new */
#define DHCP_CLIENT_TIMER_COOKIE ((uint32)0x713EC03E)
  uint32 cookie;

  /* module init info */
  dhcp_client_timer_init init;

  /* module config info */
  dhcp_client_timer_config config;

  /* The timer handle used by pstimer */
  ps_timer_handle_type timer_handle;

  /* Last time set, used for DHCPv6 backoff algorithm, in msec */
  uint32 last_time;

} dhcp_client_timer_info;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           PRIVATE FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_TIMER_CB

Description:
  This function is called when a timer expires.

Arguements:
  dhcp_client_timer_init * - the information needed to get initialized.

Return value:
  Handle to timer shim layer object

Dependencies:
  The init must be valid.
  The dhcp_clienti_timer_start() function must be called before the
  timer layer is used.
===========================================================================*/
static void
dhcpi_client_timer_cb
(
  void * handle
)
{
  dhcp_client_timer_info * info = handle;

  /* Sanity check parameters */
  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return;
  }
  if (info->cookie_ptr != info)
  {
    LOG_MSG_ERROR_2("cookie pointer mismatch %x %x", info->cookie_ptr, info);
    ASSERT(0);
    return;
  }
  if (DHCP_CLIENT_TIMER_COOKIE != info->cookie)
  {
    LOG_MSG_ERROR_2("cookie mismatch %x %x",
                    info->cookie, DHCP_CLIENT_TIMER_COOKIE);
    ASSERT(0);
    return;
  }

  if ( NULL != info->config.timer_expire )
  {
    info->config.timer_expire( info->config.timer_expire_handle );
  }
} /* dhcpi_client_timer_cb */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION DHCP_CLIENTI_TIMER_NEW

Description:
  This function initalizes a new timer shim layer object.  A handle to
  the object will be returned.

Arguements:
  dhcp_client_timer_init * - the information needed to get initialized.

Return value:
  Handle to timer shim layer object

Dependencies:
  The init must be valid.
  The dhcp_clienti_timer_start() function must be called before the
  timer layer is used.
===========================================================================*/
void *
dhcp_clienti_timer_new
(
  dhcp_client_timer_init * init
)
{
  dhcp_client_timer_info * handle;
  LOG_MSG_INFO3_0( "INFO: --- DHCP CLIENT TIMER INIT ---" );

  if (NULL == init)
  {
    DATA_ERR_FATAL("init NULL?");
    return NULL;
  }
  if (NULL == init->d_malloc || NULL == init->d_free)
  {
    LOG_MSG_ERROR_2("malloc or free NULL %x %x", init->d_malloc, init->d_free);
    ASSERT(0);
    return NULL;
  }

  handle = init->d_malloc(sizeof(dhcp_client_timer_info));
  if ( NULL == handle )
  {
    DATA_ERR_FATAL("Insufficent memory to init DHCP timer");
    return NULL;
  }
  memset(handle, 0, sizeof(dhcp_client_timer_info));
  handle->init = *init; /* struct copy */
  handle->cookie = DHCP_CLIENT_TIMER_COOKIE;

  return handle;
} /* dhcp_clienti_timer_new */

/*===========================================================================
FUNCTION DHCP_CLIENTI_TIMER_START

Description:
  This function starts the timer layer for use.

Arguements:
  void * handle - Handle to the timer object.
  dhcp_client_timer_configure * - Configuration infomation to
    interface with other systems.

Return value:
  boolean - TRUE if started ok, otherwise false.

Dependencies:
  The dhcp_clienti_timer_new must have initialized the handle.
===========================================================================*/
boolean
dhcp_clienti_timer_start
(
  void * handle,
  dhcp_client_timer_config * config
)
{
  dhcp_client_timer_info * info = handle;
  ps_timer_handle_type timer;
  boolean ret = FALSE;

  LOG_MSG_INFO3_0( "INFO: --- DHCP CLIENT TIMER START ---" );
  /* Sanity check parameters */
  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return FALSE;
  }
  if ( DHCP_CLIENT_TIMER_COOKIE != info->cookie )
  {
    LOG_MSG_ERROR_2("cookie mismatch %x %x",
                    info->cookie, DHCP_CLIENT_TIMER_COOKIE);
    ASSERT(0);
    return FALSE;
  }
  if ( NULL == config )
  {
    DATA_ERR_FATAL("config NULL?");
    return FALSE;
  }


  /* Initialize "config" info */
  info->config = *config; /* struct copy */
  info->cookie_ptr = info;

  timer = ps_timer_alloc( dhcpi_client_timer_cb, handle );
  if ( PS_TIMER_INVALID_HANDLE == timer )
  {
    DATA_ERR_FATAL("Failed to allocate timer");
  }
  else
  {
    ret = TRUE;
    info->timer_handle = timer;
  }

  return ret;
} /* dhcp_clienti_timer_start */

/*===========================================================================
FUNCTION DHCP6_CLIENTI_TIMER_FREE

Description:
  This function frees the timer layer.
 
Arguements:
  void * handle - Handle to the timer object.

Return value:
  boolean - TRUE if started ok, otherwise false.

Dependencies:
  The dhcp_clienti_timer_new must have initialized the handle.
===========================================================================*/
void
dhcp_clienti_timer_free
(
  void ** handle
)
{
  dhcp_client_timer_info * info;

  /* Sanity check parameters */
  if (NULL == handle)
  {
    DATA_ERR_FATAL("handle pointer NULL?");
    return;
  }
  info = *handle;

  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return;
  }
  if (DHCP_CLIENT_TIMER_COOKIE != info->cookie)
  {
    LOG_MSG_ERROR_2("cookie mismatch %x %x",
                    info->cookie, DHCP_CLIENT_TIMER_COOKIE);
    ASSERT(0);
    return;
  }

  /* If the free fails... we continue */
  (void) ps_timer_free( info->timer_handle );
  info->init.d_free(info);
  LOG_MSG_INFO3_0( "INFO: --- DHCP CLIENT TIMER FREE ---" );
  *handle = NULL;
} /* dhcp_clienti_timer_free */

/*===========================================================================
FUNCTION DHCP_CLIENTI_TIMER_SET

Description:
  This function starts a timer set to expire with the specified time,
  in seconds.  If there is already a timer running associated with
  this instance, it will be cancelled, and the current timer set in
  its place.

Arguements:
  void * handle - Handle to the timer object.
  uint32 delta - The amount of time to set the timer for in seconds.
  dhcp_client_timer_mode mode - indicates the mode of the timer:
     - Norm - no randomization to the number.
     - V4 Backoff - +/- 1 second randomization to the delta value.
     - V4 Init - 1 - delta second randomization.
     - V6 Backoff_Start - initial backoff retransmission time randomization.
                          See RFC 3315, Section 14. Delta should be passed
                          as the IRT (initial retransmission time) value.
     - V6 Backoff_Norm  - initial backoff retransmission time randomization.
                          See RFC 3315, Section 14. Delta should be passed
                          as the MRT (max retransmission time) value.
     - V6 Init - 0 to delta second randomization.

Return value:
  None.

Dependencies:
  The dhcp_clienti_timer_new must have initialized the handle.
  The timer layer must have been started.
===========================================================================*/

void
dhcp_clienti_timer_set
(
  void * handle,
  uint32 delta,
  dhcp_client_timer_mode mode
)
{
  dhcp_client_timer_info * info = handle;
  ps_timer_error_type err;

  int32 randval;

  /* Sanity check parameters */
  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return;
  }
  if (info->cookie_ptr != info)
  {
    LOG_MSG_ERROR_2("cookie pointer mismatch %x %x", info->cookie_ptr, info);
    ASSERT(0);
    return;
  }
  if (DHCP_CLIENT_TIMER_COOKIE != info->cookie)
  {
    LOG_MSG_ERROR_2("cookie mismatch %x %x",
                    info->cookie, DHCP_CLIENT_TIMER_COOKIE);
    ASSERT(0);
    return;
  }

  dhcp_clienti_timer_clear(handle);

  delta *= DHCP_CLIENT_MS_PER_S; /* timers needs delta in milliseconds */

  switch (mode)
  {
    case DHCP_CLIENT_TIMER_NORM:
      /* Do nothing to process delta */
      break;
    case DHCP_CLIENT_TIMER_RAND_BACKOFF:
      delta += ps_utils_generate_16bit_rand_num_by_range(0, 
                                            DHCP_CLIENT_MS_PER_S * 2) -
                                            DHCP_CLIENT_MS_PER_S;
      break;
    case DHCP_CLIENT_TIMER_RAND_INIT:
      /* This will have problems if delta > 64 seconds, since ran_dist
       * works using 16 bit numbers. If delta is 1000, we are avoiding
       * calling the function to generate random nuber because it will 
       * return 1000 only as random which we are taking care of at the
       * end of switch-case. */
      if(delta > DHCP_CLIENT_MS_PER_S)
      {
        delta = ps_utils_generate_16bit_rand_num_by_range(DHCP_CLIENT_MS_PER_S,
                                                          (word)delta);
      }
      break;
    case DHCP6_CLIENT_TIMER_RAND_BACKOFF_START:
      /* randval is uniformly distributed random no. from -0.1 to 0.1
       * scaled using DHCP6_CLIENT_RAND_GRANULARITY */
      randval = ps_utils_generate_16bit_rand_num_by_range(0, 
                                 2*DHCP6_CLIENT_RAND_GRANULARITY) - 
                                 DHCP6_CLIENT_RAND_GRANULARITY;

      /* RT = IRT + RAND*IRT */
      delta += (  (((int32)delta) * randval) /
                  (DHCP6_CLIENT_ZERO_POINT_ONE_DIVISOR *
                   DHCP6_CLIENT_RAND_GRANULARITY) );
      /* Store previous time internally in milliseconds */
      info->last_time = delta;

      break;
    case DHCP6_CLIENT_TIMER_RAND_BACKOFF_NORM:
      /* randval is uniformly distributed random no. from -0.1 to 0.1
       * scaled using DHCP6_CLIENT_RAND_GRANULARITY */
      randval = ps_utils_generate_16bit_rand_num_by_range(0, 
                                2*DHCP6_CLIENT_RAND_GRANULARITY)- 
                                DHCP6_CLIENT_RAND_GRANULARITY;

      /* RT = 2*RTprev + RAND*RTprev */
      info->last_time = (2 * info->last_time) +
                     ( (((int32)info->last_time) * randval) /
                     (10 * DHCP6_CLIENT_RAND_GRANULARITY) );

      if ( 0 != delta && (info->last_time > delta) )
      {
        /* if (RT > MRT), RT = MRT + RAND*MRT */
        info->last_time = delta +
                     ( ( ((int32)delta) * randval ) /
                     (DHCP6_CLIENT_ZERO_POINT_ONE_DIVISOR *
                      DHCP6_CLIENT_RAND_GRANULARITY) );
      }
      delta = info->last_time;

      break;
    case DHCP6_CLIENT_TIMER_RAND_INIT:
      /* This will have problems if delta > 64 seconds, since ran_dist
       * works using 16 bit numbers */
      if(delta != 0)
      {
        delta = ps_utils_generate_16bit_rand_num_by_range(0, (word)delta);
      }
      break;
    default:
      /* Error */
      LOG_MSG_ERROR_1( "Invalid mode %d", mode );
      ASSERT( 0 );
      break;
  }

  if ( 0 != delta )
  {
    err = ps_timer_start( info->timer_handle, (int64)(uint64)delta );
    if ( PS_TIMER_SUCCESS != err )
    {
      LOG_MSG_ERROR_1( "timer start returned %d", (int32)err );
    }
  }
  else
  {
    dhcpi_client_timer_cb( handle );
  }
  return;
} /* dhcp_clienti_timer_set */


/*===========================================================================
FUNCTION DHCP_CLIENTI_TIMER_CLEAR

Description:
  This function clears the timer associated with this instance, if
  there is one.

Arguements:
  void * handle - Handle to the timer object.

Return value:
  None.

Dependencies:
  The dhcp_clienti_timer_new must have initialized the handle.
  The timer layer must have been started.
===========================================================================*/
void
dhcp_clienti_timer_clear
(
  void * handle
)
{
  dhcp_client_timer_info * info = handle;
  ps_timer_error_type err;

  /* Sanity check parameters */
  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return;
  }
  if (info->cookie_ptr != info)
  {
    LOG_MSG_ERROR_2("cookie pointer mismatch %x %x", info->cookie_ptr, info);
    ASSERT(0);
    return;
  }
  if (DHCP_CLIENT_TIMER_COOKIE != info->cookie)
  {
    LOG_MSG_ERROR_2("cookie mismatch %x %x",
                    info->cookie, DHCP_CLIENT_TIMER_COOKIE);
    ASSERT(0);
    return;
  }

  err = ps_timer_cancel( info->timer_handle );
  if ( PS_TIMER_SUCCESS != err )
  {
    LOG_MSG_ERROR_1( "timer clear returned %d", err );
    ASSERT(0);
  }
  return;
} /* dhcp_clienti_timer_clear */
