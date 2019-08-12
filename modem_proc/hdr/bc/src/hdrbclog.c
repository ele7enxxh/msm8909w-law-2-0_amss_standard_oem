/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               H D R   B R O A D C A S T   P R O T O C O L 
                     L O G    F U N C T I O N S
                     
GENERAL DESCRIPTION

  This module provides logging and diag-related routines used 
  by the HDR broadcast protocol.

EXTERNALIZED FUNCTIONS (Global)


EXTERNALIZED FUNCTIONS (Regional)


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2004 - 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/src/hdrbclog.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
11/15/09   kss     Use runtime init of timer.
10/19/09   wsh     Fixed lint warnings
05/15/09   wsh     Include needed headers to support diag split
01/30/09   etv     Merged the following changes to main branch
                   Fixed Klocwork errors.
02/23/07   rmg     Fixed lint errors.
02/10/05   etv     Fixed lint errors.
02/02/05   etv     Fixed coding style to match convention.
11/04/04   kss     Support for enable/disable periodic metric logging.
08/16/04   kss     Support for moving flow-related log functions to bcmac.
07/08/04   kss     Added include of hdrbcmaci.h
06/26/04   kss     Created module

==========================================================================*/

/* <EJECT> */
/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "hdr_variation.h"
#include "comdef.h"
#include "customer.h"
#include "log_dmss.h"
#include "log.h"
#include "hdrlogi.h"
#include "timer.h"
#include "hdrdebug.h"

#ifdef FEATURE_HDR_BCMCS

#include "hdrbcmac.h"
#include "hdrbcmaci.h"
#include "hdrbcslot.h"
#include "hdrbcecb.h"
#include "hdrbcsec.h"
#include "hdrbcframe.h"


/*===========================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS

This section contains global defidleions for constants, macros, types,
variables

===========================================================================*/


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Timer for periodic logging of metrics */
timer_type hdrbclog_timer;

/* Init state of above timer */
boolean hdrbclog_timer_initialized = FALSE;

/*===========================================================================

FUNCTION HDRBCLOG_DISABLE_SOFT_COMBINING

DESCRIPTION
  Disables soft combining in broadcast.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbclog_disable_soft_combining( void )
{

  /* Tell the Broadcast slot scheduler to stop soft combining */
  hdrbcslot_disable_soft_combining();

} /* hdrbclog_disable_soft_combining */


 /* EJECT */
/*===========================================================================

FUNCTION HDRBCLOG_ENABLE_SOFT_COMBINING

DESCRIPTION
  Re-enables soft combining in broadcast (previous disabled by 
  hdrbclog_disable_soft_combining).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbclog_enable_soft_combining( void )
{

  /* Tell the Broadcast MAC's slot scheduler to resume soft combining */
  hdrbcslot_enable_soft_combining();

} /* hdrbclog_enable_soft_combining */


 /* EJECT */
/*===========================================================================

FUNCTION HDRBCLOG_SET_ECB_DROP_MASK

DESCRIPTION
  Sets drop mask in the Broadcask MAC's Error Control Black handling.
  The bitmask indicates which packets in each row of the ECB to drop 
  (intended for testing and analysis of Reed-Solomon decoding).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbclog_set_ecb_drop_mask(uint32 mask)
{
  /* Give drop mask to Broadcast MAC's ECB module */
  hdrbcecb_set_ecb_drop_mask(mask);

} /* hdrbclog_set_ecb_drop_mask */


 /* EJECT */
/*===========================================================================

FUNCTION HDRBCLOG_CLEAR_LOGICAL_CHANNEL_METRICS

DESCRIPTION
  The function clears all logical channel metrics.
  
DEPENDENCIES
  None.

PARAMETERS
  None.  
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbclog_clear_mcs_chan_metrics(void )
{

  uint8 i;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Log each active logical channel */
  for( i=0; i<HDRBCMAC_MAX_LOGICAL_CHANNELS; i++ )
  {
    hdrbcframe_clear_channel_metrics( i );
    hdrbcsec_clear_channel_metrics( i );
    hdrbcmac_clear_channel_metrics( i );
  }

} /* hdrbclog_clear_mcs_chan_metrics */


 /* EJECT */
/*=========================================================================

FUNCTION HDRBCLOG_LOG_MCS_CHAN_METRICS

DESCRIPTION
  Logs HDR BCMCS logical channel metrics (the HDR_MCS_LOG_CHAN_METRICS
  packet).

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrbclog_log_mcs_chan_metrics( void )
{

  LOG_HDR_MCS_LOG_CHAN_METRICS_C_type* log_ptr = NULL;
    /* Points to allocated log packet  */

  hdrlogbc_log_chan_metrics_type *chan_metrics;
    /* Points to fields within the log packet */

  int j = 0;
    /* Loop index */

  uint8 i = 0;
    /* Loop index */

  uint8 chan_count;
    /* Number of logical channels to log */

  uint32 flow_id;
    /* Temp holder for flow_id */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check for log_mask */
  if ( log_status( LOG_HDR_MCS_LOG_CHAN_METRICS_C ) )
  {
    /* Get number of active logical channels */
    chan_count = hdrbcmac_get_channel_count();
    if (chan_count)
    {
      
      log_ptr = (LOG_HDR_MCS_LOG_CHAN_METRICS_C_type *)
          log_alloc( (log_code_type) LOG_HDR_MCS_LOG_CHAN_METRICS_C,
                     sizeof(LOG_HDR_MCS_LOG_CHAN_METRICS_C_type) + 
                       ( sizeof(hdrlogbc_log_chan_metrics_type) * 
                         (chan_count - 1) ) );

      if (log_ptr)
      {
        log_ptr->logical_chan_count = chan_count;

        chan_metrics = log_ptr->metrics;

        /* Log each active logical channel */
        for( i=0; i<HDRBCMAC_MAX_LOGICAL_CHANNELS; i++ )
        {
          /* Check if chan is active and get flow id */
          if (hdrbcmac_get_flow_id(i, &flow_id))
          {
            chan_metrics[j].chan_id = i;
            chan_metrics[j].flow_id = flow_id;

            /* Fill in info from each layer */
            hdrbcframe_get_channel_metrics( i, &chan_metrics[j].framing);
            hdrbcsec_get_channel_metrics( i, &chan_metrics[j].sec);
            hdrbcmac_get_channel_metrics( i, &chan_metrics[j].mac);
            j++;
          }

          /* Don't exceed allocated log space! */
          if (j >= chan_count)
          {
            break; 
          }
        }
        
        log_commit((log_type*) log_ptr);
      }
    }
  }

} /* hdrbclog_log_mcs_chan_metrics */


 /* EJECT */
/*=========================================================================

FUNCTION HDRBCLOG_LOG_PHY_CHAN_METRICS

DESCRIPTION
  Logs HDR BCMCS physical channel metrics (the HDR_MCS_PHY_CHAN_METRICS
  packet).

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrbclog_log_phy_chan_metrics( void )
{

  LOG_HDR_MCS_PHY_CHAN_METRICS_C_type* log_ptr = NULL;
    /* Point to allocated log packet  */

  uint8 i,j;
    /* Loop indexes */

  uint8 mux_count;
    /* Multiplex count (can be 4, 8 or 16) */

  uint64 bit_mask = 1;
    /* Used to construct im_pair_mask */

  uint64 im_pair_mask = 0;
    /* Logged bit mask to indicate active im pairs */

  int mux_index = 0;
    /* Index into the mux_metrics array */

  hdrbcmac_static_metrics* metrics;
    /* Pointer to metrics info */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Don't log without checking for log_mask... */
  if ( log_status( LOG_HDR_MCS_PHY_CHAN_METRICS_C ) )
  {
    /* Get current multiplex count */
    mux_count = (uint8) hdrbcmac_get_multiplex_count();

    /* If the Multiplex count is within bounds */
    if (mux_count <= HDRBCOVHD_MAX_SUPPORTED_MULTIPLEXES)
    {
      log_ptr = (LOG_HDR_MCS_PHY_CHAN_METRICS_C_type *)
            log_alloc( (log_code_type) LOG_HDR_MCS_PHY_CHAN_METRICS_C,
                        sizeof(LOG_HDR_MCS_PHY_CHAN_METRICS_C_type) + 
                        ( sizeof(hdrlogbc_multiplex_metrics_type) * 
                          ( (mux_count* 4) - 1 ) ) );

      if (log_ptr)
      {
        log_ptr->multiplex_count = mux_count;

        /* Get pointer to metrics */
        metrics = hdrbcmac_get_metrics_ptr();


        /* For each interlace... */
        for (j=0; j<HDRBCMAC_INTERLACE_COUNT; j++)
        {
       
          /* Within each interlace, for each multiplex... */
          for (i = 0; i<mux_count; i++) 
          {

            mux_index = (j * mux_count) + i;
          
            /*lint -save -e661 -e662 */
            /* Disable creation and access of out-of-bounds pointer
               lint error. The log packet declares an array of size 
               one and uses it as an array of size 4. The memory
               allocation is done for four though. So it shouldn't 
               be a problem. */
            log_ptr->mux_metrics[ mux_index ].mac_crc_pass = 
              metrics->multiplex[j][i].crc_pass;

            log_ptr->mux_metrics[ mux_index ].mac_crc_fail = 
              metrics->multiplex[j][i].crc_fail;

            /*lint -restore */
  
            /* Set bit if im pair is currently active */
            if(hdrbcmac_is_phys_chan_active(j,i))
            {
              im_pair_mask |= bit_mask;
            }
            bit_mask = bit_mask << 1;
          }
        }

        /* Fill in the monitored im pair bit mask */
        for (j=0; j<8; j++)
        {
          log_ptr->monitored_im_pairs[j] = 
            im_pair_mask & 0x0FF;

          im_pair_mask = im_pair_mask >> 8;
        }

        log_commit((log_type*) log_ptr);

      } /* If log_alloc succeeded */

    } /* If Mux count is within bounds */ 
    else 
    {
      /* This is not good. Mux Count is out-of-bounds.
         Print an error message to detect. */
      HDR_MSG_PROT_2( MSG_LEGACY_ERROR,
                      "MultiplexCount %d is out of bounds. Supported:  %d",
                      mux_count,
                      HDRBCOVHD_MAX_SUPPORTED_MULTIPLEXES );
    }

  } /*  If log is enabled */

} /* hdrbclog_log_phy_chan_metrics */


 /* EJECT */
/*=========================================================================

FUNCTION HDRBCLOG_LOG_PERIODIC_METRICS

DESCRIPTION
  Function for calling all periodic broadcast logging functions.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrbclog_log_periodic_metrics
(
  int32                           time_ms,
    /* Time elapsed from timer setting (not used here) */
    /*lint -esym(715,time_ms)
      Ignore Symbol not used lint error */

  timer_cb_data_type              data
    /*lint -esym(715,data)
      Ignore Symbol not used lint error */
    /* For timer callball type -- not used */
)
{
  /* Call logging functions */
  hdrbclog_log_mcs_chan_metrics();
  hdrbclog_log_phy_chan_metrics();

} /* hdrbclog_log_periodic_metrics */


 /* EJECT */
/*=========================================================================

FUNCTION HDRBCLOG_ENABLE_PERIODIC_LOGGING

DESCRIPTION
  Enables periodic logging of BCMCS stats.

DEPENDENCIES
  None

PARAMETERS
  period - the period for logging the packets (in ms)

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrbclog_enable_periodic_logging
(
  uint32 period
)
{
  /* Define timer on first time through. */
  if (!hdrbclog_timer_initialized)
  {
    timer_def2(&hdrbclog_timer, NULL);
    hdrbclog_timer_initialized = TRUE;
  }

  /* Start timer */
  timer_reg( &hdrbclog_timer, 
             hdrbclog_log_periodic_metrics,
             0,
             period,
             period );

} /* hdrbclog_enable_periodic_logging */


 /* EJECT */
/*=========================================================================

FUNCTION HDRBCLOG_DISABLE_PERIODIC_LOGGING

DESCRIPTION
  Disables periodic logging of BCMCS stats.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrbclog_disable_periodic_logging( void )
{

  /* Stop timer */
  (void) timer_clr( &hdrbclog_timer, T_NONE);

} /* hdrbclog_disable_periodic_logging */

#endif /* FEATURE_HDR_BCMCS */
