#ifndef SNS_SMGR_CONFIG_H
#define SNS_SMGR_CONFIG_H
/*=============================================================================
  @file sns_smgr_config.h
 
  This header file contains the definitions that are needed to configure
  the DSPS Sensor Manager (SMGR) for a specific environment. Includes memory 
  allocations, etc.
 
******************************************************************************   
*   Copyright (c) 2010-2012, 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
*   Qualcomm Technologies Proprietary and Confidential.
*
******************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_config.h#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-03-26  ps   Updated license header
  2012-10-03  sc   Scale timestamp back down to 32768Hz tick.
  2012-08-23  ag   Fix timestamps; handle 19.2MHz QTimer
  2011-04-24  br   nullified the differentiation SNS_SMGR_USEC_PER_TICK between PCSIM and real target
  2011-01-21  sc   Added support for PCSIM (different usec/tick value used)
  2010-10-05  jb   CC3 initial build
  2010-08-20  JB   Characterize dynamic range 
  2010-08-04  JB   Merge with revised ucos, use PC timer
  2010-06-11  JB   Initial version
  
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

  /* Option to use static memory for SMGR available space. Use malloc/free
     when the option is undef */
     
#undef  SNS_SMGR_STATIC_MEMORY_OPTION 

#ifdef SNS_SMGR_STATIC_MEMORY_OPTION
  /* Number of blocks in standing order list. Blocks are linked in a mesh
     configuration to control each report and to manage sensor sampling and
     filtering.  Each report requires at least 1 + <number of items reported> 
     + <1 for each item in a buffered report>.  Each sensor being sampled 
     requires 2 + <number of software filters for that sensor>. Each block is
     ~52 bytes (estimate as of May 2010).
     Estimate 100 blocks will support about 10 clients with an average of 2 
     reports per client and 2 items per report 
  */
#define SNS_SMGR_NUM_SOL_BLOCKS             50
#endif


  /* Nominal microseconds per tick of the scheduling clock. It is understood
     that the clock can drift because of clock crystal variation. This
     may look like a FLT number, but the compiler uses it in a constant 
     expression which is then cast to an integer. 
  */
 #define SNS_SMGR_USEC_PER_TICK              30.51

  /* Number of report items buffered in a SOL block. This should be the number
     that will fit without enlarging the union size */
#define SNS_SMGR_NUM_BUF_ITEMS_PER_BLOCK    2

  /* Characterize dynamic range control */
  /* Up Threshold, fraction of nominal range limit. If magnitude of any axis
     > UP_THRESH, go to next higher range */
#define SMGR_RANGE_UP_THRESH                0.9
  /* Down Threshold, fraction of nominal range limit. If magnitude of any axis
     < DOWN_THRESH for a time duration, go to lower range */
#define SMGR_RANGE_DOWN_THRESH              0.6
  /* Duration of wait for dropping to lower range */
#define SMGR_DOWN_RANGE_DELAY_MSEC          1000
#define SMGR_DOWN_RANGE_DELAY               (uint32_t)(SMGR_DOWN_RANGE_DELAY_MSEC*1000/SNS_SMGR_USEC_PER_TICK)

  /* Tune the responsiveness of adjusting the heartbeat. Set number of 
     heartbeats from the last schedule until it is time to evaluate whether
     the heartbeat needs another adjustment. Max value <= 255 */
#define SMGR_EVAL_HB_AFTER_SCHED          5
#define SMGR_EVAL_HB_AFTER_ADJUST_WORSE   5
#define SMGR_EVAL_HB_AFTER_ADJUST_RECOVER 20
#define SMGR_EVAL_HB_AFTER_EVAL_NO_ADJUST 100

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/


#endif /* #ifndef SNS_SMGR_CONFIG_H */
