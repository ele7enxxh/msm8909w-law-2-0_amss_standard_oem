/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               D S 7 0 7 _ S C R M   E X T E R N A L   F I L E 

GENERAL DESCRIPTION
  This file contains functions that are used to implement the SCRM processing
  available in IS2000.
  
  SCRM's can happen during SO33 data calls.  SCRM's usually happen while a
  call is active, but can also be used to come out of control/hold state.
  
  This module assumes that RLP 3 calls ds_scrm_sch_chng_cback() every 20
  msec.
  
EXTERNALIZED FUNCTIONS
  DS707_SCRM_REG_FUNCS
    Registers various callbacks with MC.
    
  DS707_SCRM_CALL_INIT
    Called when an SO 33 call connects.  Resets various SCRM state
    variables.
  
  DS707_SCRM_RLP_CBACK
    Called by RLP every 20-msec.  Tells SCRM if RLP has data to send.  If
    the conditions are correct, SCRM module will ask MC to send a request
    to base station for R-SCH (i.e., ask MC to send SCRM).
    
  DS707_SCRM_ESCAM_RXED
    Called by MC when there are x frames or less before the current R-SCH
    burst ends.  Indicates to SCRM module that it may want to SCRM in order
    to "extend" the current R-SCH burst.
  
  DS707_SCRM_MC_CBACK
    Called by MC whenever the F-SCH or R-SCH config changes.  This allows
    mobile to figure out if it has enough CPU with current config, and then
    to SCRM accordingly.
  
  DS707_SCRM_MAX_ALLOWABLE_SCH_RATE
    Called by MUX when the F-SCH and R-SCH config changes.  Tells MUX how
    many PDU's it can build without phone CPU running out.
  
  DS707_SCRM_BUILD_BLOB
    Called by ds707_rrm.c.  This function builds the SCRM BLOB, which tells
    the BS what the requested R-SCH will be.
  
  DS707_SCRM_RETRY_PROCESSING
    Called when the SCRM retry delay expires.  Resets SCRM retry delay params
  
  DS707_SCRM_RETRY_DELAY
    Called when MS receives a retry delay (for SCRM) from BS.  Sets the
    SCRM retry delay timer to the value assigned by BS.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  At startup, must call ds707_scrm_reg_funcs().
  
  Before every call, must call ds707_scrm_call_init().

Copyright (c) 2000-2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_scrm.c_v   1.7   27 Jan 2003 17:32:02   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_scrm.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
06/26/13   sc         Replace all the memcpy and memmove with memscpy and 
                      memsmove as its safer.
06/03/13   sd         ATCoP decoupling changes
01/02/12   msh        Coan: Feature cleanup
11/22/10   sn         Replaced REX premption macros with critical section/ISR 
                      lock for mutual exclusion.
10/13/10   op         Migrated to MSG 2.0 macros
12/18/09   ls         Use the scrm_sch_frame_matrix with the max rates
11/10/09   as         Cleared Windows Compiler warnings for MOB builds
05/19/09   ss         Fixed off target lint medium errors.
07/01/08   psng       KlocWork Fixes
05/10/07   ac         Lint fix
04/10/06   TMR        Fixed problem of never setting queue_status in
                      ds707_scrm_rlp_cback()
01/06/06   TMR        Changed parameters to ds707_scrm_max_allowable_sch_rate
                      and ds_scrm_max_allowable_sch_rate
12/07/05   as/kvd     Using mc_queue_cmd to send commands to mc instead of 
                      ds707_mc_queue_cmd and mc_cmd
05/18/03   ak         When a burst ends, set ds_scrm_now = FALSE, indicating
                      next SCRM is for CPU-RF reasons.
04/17/03   sy         Cancelled generic timer for non-zero reverse assignments.
04/10/03   sy         Added support for 32x SCRM and code cleanup. Removed 
                      tables for 5000 series chipset as they belong to old 
                      architecture
01/27/03   ak         Another fix for 32x.
12/12/02   sn/ak      Added SCRM_ON_RF functionality.
12/08/02   ak         For Mux_PDU 5, calculate max_r_sch off of field in 
                      config ptr.
12/03/02   ak         For mux pdu 5, made correction on max_r_sch.
11/18/02   ak         Added case statements for Rel-A, mux PDU 5 (mux option
                      0xf20 (CAI_MUX_VARIABLE)).
11/18/02   ak         Updated file header.
09/10/02   ak         Updated for MSM6100 tables.
02/12/02   snn        Made ds_scrm_now to FALSE in beginning of every call.
                      Added byte parameter to ds_scrm_escam_rxed so that MS
                      knows when its current burst is ended.
12/10/01   ak         on a retry clear, zero out timers, instead of resetting
                      to 1-second.
11/26/01   ak         Changed MIN_BYTES level to 300.
10/05/01   ak         Changes to support new burst-extension SCRM.
09/20/01   ak         For non-IRAM MSM5100, allow 16x-2x.
09/20/01   ak         Fix for infinite retry delay.
09/17/01   sy         Added FEATURE_DS_IS2000_RS2 for RS2 mux options.
09/14/01   rama       SR_ID fix. Use actual SR_ID in the SCRM blob.
09/14/01   sy         Added support for RS2.
09/11/01   ak         Added non-IRAM table for MSM5100.
06/19/01   ak         Fixed bad #ifdef (could not compile).
06/18/01   ak         Re-ordered SCRM table #ifdefs.
06/15/01   snn        Removed two unused variable in ds_scrm_build_blob.
06/13/01   ak         Fixed #define for MSM5105
06/01/01   snn        Removed some #ifdef after codereview.
06/01/01   ak         Look at RF power when figuring out what to SCRM for.
                      SCRM now at min(1-second,end-of-burst).
05/27/01   snn        Added additional parameter for rxc_get_sch_info.
05/21/01   ak         Added SCRM table for MSM5105
05/11/01   ak         Changed to be more compatible with dsrrm.c and
                      dsis2k.c.  Now always call scrm and rrm funcs at end of
                      tx_build_frames.
05/02/01   fc         Changes to conform to the re-modularization of all MC 
                      support functions for DS requests.
04/10/01   lcc        Changed calls to snm_get_current_config to conform to new
                      prototype.
04/02/01   ak         SCRM now only to lower or raise r-SCH. Do not SCRM to
                      cancel when zero left to send.
03/08/01   na         Added SCRM table for MSM5100 - 16XRSCH always allowed.
03/07/01   igt        Fixed compiler warnings
02/28/01   ak         Fix for asking MC too quickly for another SCRM.
02/28/01   ak         When getting max allowable pdu's to build, return value
                      that doesn't require txc to check for single/double-
                      sized.
02/23/01   igt        Added FEATURE_IS2000_R_SCH for the whole file instead of 
                      just parts of it, so the feature can be turned off.
02/16/01   rmd        Removed ds_scrm_need_27MHZ function definition and 
                      scrm_sch_27MHZ_matrix structure definition. This 
                      functionally is now supported in mclk_XXXX.c
02/14/01   igt        New BLOB processing. Will not Cancel R-SCH to go to
                      allowable rates. Now will just SCRM for the proper rate.
02/09/01   igt        Change the SCRM implementation to be based on number of
                      frames sent on the forward and reverse supplementals as
                      opposed to rates. This makes the requested rates more
                      accurate. 
01/11/01   snn        Included err.h outside FEATURE_IS2000_R_SCH.
01/10/01   ak         Changed init() to support being called when service
                      negotiation done in a call.  Also removed TDSO logic,
                      as no longer SCRM when in a TDSO call.
01/10/01   ak         Changes to support MC sending SCH config info in 
                      callback.
01/08/01   ak         Check to see if R-SCH is enabled when getting params.
12/15/00   ak         Fixed bug when didn't SCRM to cancel when R-SCH ==
                      max mux option.  Also put in code to allow R-SCH
                      throttling to be turned off.
12/01/00   ak         Changed callbacks from MC when SCH's change, to acount
                      for re-calculating R-SCH throttle params, when only
                      F-SCH changes.
11/15/00   ak         Removed compiler warning (unused local var).
11/07/00   ttl/ak     Added scrmi_get_f_sch_rate() and 
                      ds_scrm_max_allowable_sch_rate() to limit the max
                      allowable number of MuxPDUs according to table
                      scrm_sch_rate_matrix[].
11/06/00   ks         Added SCRM Support for Full TDSO service option. 
10/18/00   ak         Do not register SCRM initialization as a callback. Now
                      initialized when pkt_so_initializer called from MC or
                      when TDSO starts up.
08/06/00   ak         Now check to see if R-SCH's are enabled when trying
                      to SCRM.
07/31/00   ak         Added table to determine if the 27MHz clock is needed
                      with IRAM.  Also now pay attention to the negotiated
                      max mux option when SCRM'ing.
07/24/00   ak         SCRM is now data-responsive and checks for R-SCH
                      after a SCRM is sent.  Also added code for 27MHz
                      clock up/down support.
07/11/00   ak         Fixes for when to ask for a SCRM and cancel a SCRM.
07/09/00   ak         Created file.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_IS2000_R_SCH
#include "amssassert.h"
#include "cai.h"
#include "caii.h"
#include "dec5000.h"
#include "dsrlp_v.h"
#include "err.h"
#include "data_msg.h"
#include "mc.h"
#include "mccdma.h"
#include "mccdma.h"
#include "mccds.h"
#include "mccsch.h"
#include "mccsch.h"
#include "msg.h"
#include "rxc.h"
#include "snm.h"
#include "txc.h"
#include "ds707_rrm.h"
#include "ds707_scrm.h"
#include "ds707_mc.h"
#include "dsat707vendctab.h"
#include "ds707_pkt_mgr.h"
#include "dsutil.h"
#include "rlgc.h"

/*===========================================================================
                        TYPEDEFS AND VARIABLES
===========================================================================*/
#define SCRM_MIN_BYTES 300

typedef enum
{
    FWD_SCH = 0,
    REV_SCH = 1
} scrm_sch_type;

/*---------------------------------------------------------------------------
  Enum for SCH rates.  If changing these, please see if code below is
  affected.
  The rates supported for 6000 series chipset onwards.
---------------------------------------------------------------------------*/
typedef enum
{
  SCRM_SCH_RATE_NONE   = 0,              /* No SCH                         */
  SCRM_SCH_RATE_1x     = 1,              /* 1x                             */
  SCRM_SCH_RATE_2x     = 2,              /* 2x                             */
  SCRM_SCH_RATE_4x     = 3,              /* 4x                             */
  SCRM_SCH_RATE_8x     = 4,              /* 8x                             */ 
  SCRM_SCH_RATE_16x    = 5,              /* 16x                            */
  SCRM_SCH_RATE_32x    = 6,              /* 32x                            */
  SCRM_SCH_RATE_MAX    = 7               /* used for bounds checking       */
} scrm_sch_rate_type;

/*---------------------------------------------------------------------------
  Enum for SCH frames.  If changing these, please see if code below is
  affected.
---------------------------------------------------------------------------*/
typedef enum
{
  SCRM_SCH_FRAME_NONE= 0,              /* No SCH                           */
  SCRM_SCH_FRAME_1   = 1,              /* 1x-1s or 2x-1d frames            */
  SCRM_SCH_FRAME_2   = 2,              /* 2x-2s or 4x-2d frames            */
  SCRM_SCH_FRAME_4   = 3,              /* 4x-4s or 8x-4d frames            */
  SCRM_SCH_FRAME_8   = 4,              /* 8x-8s or 16x-8d frames           */ 
  SCRM_SCH_FRAME_VARIABLE =5,          /*          32x-8d frames           */
  SCRM_SCH_FRAME_MAX = 6               /* used for bounds checking         */
} scrm_sch_frame_type;

typedef struct
{
  scrm_sch_frame_type f_sch;              /* current f-sch rate in frames   */
  scrm_sch_frame_type r_sch;              /* current r-sch rate in frames   */
  scrm_sch_frame_type max_r_sch;          /* max r-sch allowable (frames)   */
} scrm_sch_struct;

/*---------------------------------------------------------------------------
  These are the SCH config values, as told to us by MC.  They are updated
  whenever the SCH is changed.
---------------------------------------------------------------------------*/
typedef struct
{
  boolean             r_sch_enabled;    /* R-SCH enabled?                  */
  boolean             r_sch_double ;    /* are reverse sch double sized?   */
  boolean             f_sch_double ;    /* are forward sch double sized?   */
  scrm_sch_frame_type max_r_sch;        /* max mux option on R-SCH         */
  scrm_sch_frame_type current_f_sch;    /* current F-SCH rate              */
  scrm_sch_frame_type current_r_sch;    /* current R-SCH rate              */
} scrmi_sch_frame_info_type;

/*---------------------------------------------------------------------------
  This stores the settings for the SCH.
---------------------------------------------------------------------------*/
LOCAL scrmi_sch_frame_info_type sch0_frame_info;

/*---------------------------------------------------------------------------
   #defines indicating number of frames in a second, number of milliseconds
   in a frame, and also a max time for the timer.  The max time, is spec'd
   in 20 msec-frames
---------------------------------------------------------------------------*/
#define SCRM_MAX_TIMER_TIME         0xFFFFFFFF    /* in frame-times        */
#define SCRM_FRAMES_PER_SEC         50            /* 50 frames per second  */
#define SCRM_MSEC_PER_FRAME         20            /* 20 msec per frame     */



/*---------------------------------------------------------------------------
  Timer variables.
---------------------------------------------------------------------------*/
LOCAL uint32   ds_scrm_timer;            /* general SCRM timer             */
LOCAL boolean  ds_scrm_timer_from_bs;    /* SCRM timer assigned by BS?     */
LOCAL uint32   ds_scrm_cpu_rf_timer;     /* CPU/RF Event timer             */

LOCAL boolean  ds_scrm_now;                  /* SCRM asap to extend burst? */
LOCAL boolean  ds_scrm_awaiting_burst_start; /* ESCAM Rx'ed, waiting for   */
                                             /*  burst                     */
/*---------------------------------------------------------------------------
  Array of R-SCH frames  based on F-SCH  frames.  The index into the array is
  the scrm_sch_frames_type.  For example, scrm_sch_frame_matrix[0] gives the
  R-SCH at xyz frames.  The zero'th index implies an F-SCH of NONE, which
  is the first enum in the scrm_sch_rate_type.
  
  Matrices are defined for various MSMs.
  
  It does not matter whether the frames are single or double sided as it   
  takes the same amount of time to prepare and process either. So the     
  array is in terms of frames (single or double sized)                    
---------------------------------------------------------------------------*/
LOCAL const scrm_sch_frame_type  scrm_sch_frame_matrix[SCRM_SCH_FRAME_MAX] = 
                        { SCRM_SCH_FRAME_VARIABLE,   /*F-SCH = NONE, R-SCH = 8   */
                          SCRM_SCH_FRAME_VARIABLE,   /*F-SCH = 1,    R-SCH = 8   */
                          SCRM_SCH_FRAME_VARIABLE,   /*F-SCH = 2,    R-SCH = 8   */
                          SCRM_SCH_FRAME_VARIABLE,   /*F-SCH = 4,    R-SCH = 8   */
                          SCRM_SCH_FRAME_VARIABLE,   /*F-SCH = 8,    R-SCH = 8   */
                          SCRM_SCH_FRAME_VARIABLE    /*F-SCH =32x,   R-SCH = 8   */ 
                        };

/*---------------------------------------------------------------------------
  This array helps convert from frames to rates given that we have single 
  size sch PDUs. This is addressed with scrm_sch_frame_type
---------------------------------------------------------------------------*/
LOCAL const scrm_sch_rate_type  scrm_single_sch_rate_matrix[SCRM_SCH_RATE_MAX] = 
                        {
                           SCRM_SCH_RATE_NONE, /* No SCH, 0 single size PDU*/
                           SCRM_SCH_RATE_1x ,  /* 1x, 1 single size PDU    */
                           SCRM_SCH_RATE_2x,   /* 2x, 2 single size PDUs   */
                           SCRM_SCH_RATE_4x,   /* 4x, 4 single size PDUs   */
                           SCRM_SCH_RATE_8x,   /* 8x, 8 single size PDUs   */ 
                           SCRM_SCH_RATE_16x,  /* 16x, 0 single size PDUs  */
                           SCRM_SCH_RATE_32x   /* 32x, 0 single size PDUs  */
                        };

/*---------------------------------------------------------------------------
  This array helps convert from frames to rates given that we have double 
  size sch PDUs. This is addressed with scrm_sch_frame_type
---------------------------------------------------------------------------*/
/*lint -save -e641*/
LOCAL const scrm_sch_rate_type  scrm_double_sch_rate_matrix[SCRM_SCH_RATE_MAX-1] = 
                        {
                           SCRM_SCH_RATE_NONE, /* No SCH, 0 double size    */
                           SCRM_SCH_RATE_2x,   /* 2x, 1 double size PDU    */
                           SCRM_SCH_RATE_4x,   /* 4x, 2 double size PDU    */
                           SCRM_SCH_RATE_8x,   /* 8x, 4 double size PDU    */ 
                           SCRM_SCH_RATE_16x,  /*16x, 8 double size PDU    */
                           SCRM_SCH_RATE_32x   /*32x, 8 double size PDU    */
                        };

/*---------------------------------------------------------------------------
  These are the control bytes put into the SCRM BLOB.
---------------------------------------------------------------------------*/
#define SCRM_CTL_BYTE      0xE4

LOCAL byte scrm_pref_rate[SCRM_SCH_RATE_MAX-1] =
                              {
                               0x00,          /* 1x                       */
                               0x01,          /* 2x                       */
                               0x02,          /* 4x                       */
                               0x03,          /* 8x                       */
                               0x04,          /* 16x                      */
                               0x05           /* 32x                      */
                              };
/*lint -restore*/

/*===========================================================================
                   INTERNAL FUNCTION DEFINITIONS
===========================================================================*/


/*===========================================================================
FUNCTION      SCRMI_CONVERT_SCRM_TO_ENC_RATE

DESCRIPTION   Converts the SCRM rate to Encoder Rate because they are both 
              different values.

DEPENDENCIES  None

RETURN VALUE  if return value is   ENC_NUM_SCH_RATES then its error 
  
SIDE EFFECTS  None
===========================================================================*/
enc_sch_rate_type scrmi_convert_scrm_to_enc_rate(scrm_sch_rate_type scrm_curr_rate)
{
    enc_sch_rate_type ret_val;
    switch (scrm_curr_rate) 
    {
        case SCRM_SCH_RATE_NONE: 
            ret_val = ENC_NUM_SCH_RATES;       /* This is not elegant */
            break;
        case SCRM_SCH_RATE_1x:
            ret_val = ENC_SCH_FULL_RATE;
            break;
        case SCRM_SCH_RATE_2x:
            ret_val = ENC_SCH_2X_RATE;
            break;
        case SCRM_SCH_RATE_4x:
            ret_val = ENC_SCH_4X_RATE;
            break;
        case SCRM_SCH_RATE_8x:
            ret_val = ENC_SCH_8X_RATE;
            break;
        case SCRM_SCH_RATE_16x:
            ret_val = ENC_SCH_16X_RATE;
            break;
#ifdef FEATURE_32X_SCH_DATA_RATE                                       
        case SCRM_SCH_RATE_32x:
            ret_val = ENC_SCH_32X_RATE;
            break;
#endif
      case SCRM_SCH_RATE_MAX:
        default:
            ret_val = ENC_NUM_SCH_RATES;
            break;
    }
    return (ret_val);
}

/*===========================================================================
FUNCTION      SCRMI_CONVERT_ENC_TO_SCRM_RATE

DESCRIPTION   Converts the Encoder Rate to SCRM rate because they are 
              different values.

DEPENDENCIES  None

RETURN VALUE  if return value is or 
  
SIDE EFFECTS  None
===========================================================================*/
scrm_sch_rate_type scrmi_convert_enc_to_scrm_rate(enc_sch_rate_type enc_rate)
{
  scrm_sch_rate_type ret_val;
  switch(enc_rate)
  {
    case ENC_SCH_8TH_RATE:
    case ENC_SCH_QTR_RATE:
    case ENC_SCH_HALF_RATE:
    case ENC_NUM_SCH_RATES:
      ret_val = SCRM_SCH_RATE_NONE;
      break;
    case ENC_SCH_FULL_RATE:
      ret_val = SCRM_SCH_RATE_1x;
      break;
    case ENC_SCH_2X_RATE:
      ret_val = SCRM_SCH_RATE_2x;
      break;
    case ENC_SCH_4X_RATE:
      ret_val = SCRM_SCH_RATE_4x;
      break;
    case ENC_SCH_8X_RATE:
      ret_val = SCRM_SCH_RATE_8x;
      break;
    case ENC_SCH_16X_RATE:
      ret_val = SCRM_SCH_RATE_16x;
      break;
#ifdef FEATURE_32X_SCH_DATA_RATE                                       
    case ENC_SCH_32X_RATE:
       ret_val = SCRM_SCH_RATE_32x;
       break;
#endif
    default:
      ret_val = SCRM_SCH_RATE_NONE;
      break;
  }
  return (ret_val);
}

/*===========================================================================
FUNCTION      SCRMI_GET_RPC_RATING

DESCRIPTION   Returns the rate which RPC desires in terms of SCRM rates.

DEPENDENCIES  None

RETURN VALUE  if return value is or 
  
SIDE EFFECTS  None
===========================================================================*/
scrm_sch_rate_type scrmi_get_rpc_rating(scrm_sch_rate_type scrm_curr_rate)
{
     enc_sch_rate_type enc_rate;
     scrm_sch_rate_type scrm_rate;
/*------------------------------------------------------------------------*/

     /*----------------------------------------------------------------------
       Convert the SCRM rate to the Encoder rate so that RPC files
       can access the correct values.
     ----------------------------------------------------------------------*/
     enc_rate = scrmi_convert_scrm_to_enc_rate(scrm_curr_rate);
     
     /*----------------------------------------------------------------------
       Confirm that the output values are in the range. If they are not in
       desired range, then return NONE. So, we will not SCRM.
     ----------------------------------------------------------------------*/
     if(ENC_NUM_SCH_RATES == enc_rate)
     {
        return (SCRM_SCH_RATE_NONE);
     }

     /*----------------------------------------------------------------------
        If we came this far, all the inputs are correct. Request RPC to 
        return the value for SCH rate it can support.
     ----------------------------------------------------------------------*/
     enc_rate = rpc_get_sch_scrm_rate(enc_rate);

     /*----------------------------------------------------------------------
       Confirm that the output values are in the range. If they are not in
       desired range, then return NONE. So, we will not SCRM.
     ----------------------------------------------------------------------*/
     if(ENC_NUM_SCH_RATES == enc_rate)
     {
        return (SCRM_SCH_RATE_NONE);
     }

     /*----------------------------------------------------------------------
       Convert the Enc rate to the SCRM rate 
     ----------------------------------------------------------------------*/
     scrm_rate = scrmi_convert_enc_to_scrm_rate(enc_rate);

     /*----------------------------------------------------------------------
       Confirm that the output values are in the range. If they are not in
       desired range, then return NONE. So, we will not SCRM. Return the
       good values
     ----------------------------------------------------------------------*/           
     if(scrm_rate >= SCRM_SCH_RATE_MAX) 
     {
       return (SCRM_SCH_RATE_NONE);
     }
     else
     {
       return scrm_rate;
     } 
}

/*===========================================================================
FUNCTION     SCRMI_CONVERT_MAX_RATE

DESCRIPTION  Convert the max rate given by MC into our frame enum.

             Max rate given by MC is a byte 0 - 4.  The implied rate
             is:
             
                       2^max_rate.
                       
             So 0 = 1x, 1 = 2x, etc...

DEPENDENCIES Assumes that the PDU's are double-sized.

RETURN VALUE New rate type.  If rate type is unknown, then return max
             rate.

SIDE EFFECTS None.
===========================================================================*/
scrm_sch_frame_type scrmi_convert_max_rate
(
  byte    rate_from_mc
)
{
  scrm_sch_frame_type     frame_rate;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  switch(rate_from_mc)
  {
    case 0:
    case 1:
      frame_rate = SCRM_SCH_FRAME_1;       /* 1x or 2x */
      break;
    case 2:
      frame_rate = SCRM_SCH_FRAME_2;       /* 4x       */
      break;
    case 3:
      frame_rate = SCRM_SCH_FRAME_4;       /* 8x       */ 
      break;
    case 4:
      frame_rate = SCRM_SCH_FRAME_8;       /* 16x     */
      break;
    case 5:
      frame_rate = SCRM_SCH_FRAME_VARIABLE;/* 32x     */
      break;
    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Uknown rate %d", rate_from_mc);
      frame_rate = SCRM_SCH_FRAME_NONE;   /* assume worst case.. no SCRM */
  }

  return(frame_rate);
} /* scrmi_convert_max_rate */



/*===========================================================================
FUNCTION     SCRMI_CONVERT_SCH_FRAME

DESCRIPTION  Convert an SCH rate from cai_sch_rate_type to local 
             scrm_sch_frame_type.

DEPENDENCIES None.

RETURN VALUE New frame type.  If frame type is unknown, then return max
             frames.

SIDE EFFECTS None.
===========================================================================*/
scrm_sch_frame_type scrmi_convert_sch_frame
(
  cai_sch_rate_type sch_rate,
  scrm_sch_type     fwd_rev_sch
)
{
  scrm_sch_rate_type  scrm_sch_rate;
  scrm_sch_frame_type scrm_sch_frame;
  boolean             double_size;

  if (sch_rate == CAI_SCH_RATE_NULL)
  {
    return(SCRM_SCH_FRAME_NONE);
  }
  else if (sch_rate <= CAI_SCH_RATE_RS1_16X_RS2_16X)
  {
    
    scrm_sch_rate = (scrm_sch_rate_type)((int)(sch_rate) + 1);

    if ( fwd_rev_sch == FWD_SCH) 
    {
      double_size = sch0_frame_info.f_sch_double;
    }
    else 
    {
      double_size = sch0_frame_info.r_sch_double;
    }
 
    switch (scrm_sch_rate) 
    {
      case SCRM_SCH_RATE_NONE:     /* No SCH                        */
        scrm_sch_frame = SCRM_SCH_FRAME_NONE;
        break;

      case SCRM_SCH_RATE_1x:      /* 1x                             */
        scrm_sch_frame = SCRM_SCH_FRAME_1;
        break;

      case SCRM_SCH_RATE_2x:     /* 2x                             */
        scrm_sch_frame = (scrm_sch_frame_type)
                           ((double_size)?SCRM_SCH_FRAME_1:SCRM_SCH_FRAME_2);
        break;

      case SCRM_SCH_RATE_4x:     /* 4x                             */
        scrm_sch_frame = (scrm_sch_frame_type)
                           ((double_size)?SCRM_SCH_FRAME_2:SCRM_SCH_FRAME_4);
        break;
                                                                     
      case SCRM_SCH_RATE_8x:     /* 8x                             */ 
        scrm_sch_frame = (scrm_sch_frame_type)
                           ((double_size)?SCRM_SCH_FRAME_4:SCRM_SCH_FRAME_8);
        break;

      case SCRM_SCH_RATE_16x:    /* 16x                            */   
        scrm_sch_frame = SCRM_SCH_FRAME_8;
        break;

      default:
        DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Uknown rate %d", scrm_sch_rate);
        return(SCRM_SCH_FRAME_8);

    }/* switch() */                    
    
    return(scrm_sch_frame);
  
  }
  else if(sch_rate == CAI_SCH_RATE_RS1_32X_RS2_18X)         /* 32x */
  {
    scrm_sch_frame = SCRM_SCH_FRAME_VARIABLE;
    return (scrm_sch_frame);   
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Uknown rate %d", sch_rate);
    /*---------------------------------------------------------------
      For forward link the worst case is the max rate supported and
      for the reverse link the worst case is the minimum rate.
    ---------------------------------------------------------------*/
    if(fwd_rev_sch == FWD_SCH)
    {
      return(SCRM_SCH_FRAME_VARIABLE);
    }
    else
    {
      return (SCRM_SCH_FRAME_NONE);
    }
    
  }
} /* scrmi_convert_sch_frame */

/*===========================================================================
FUNCTION      SCRMI_RATE_TO_SCRM_FOR

DESCRIPTION   Finds out if it is necessary to send a SCRM.  Fills out the
              input parameters with the current config (if TRUE is 
              returned).

DEPENDENCIES  None.

RETURN VALUE  Rate to SCRM for.

SIDE EFFECTS  None.
===========================================================================*/
scrm_sch_rate_type scrmi_rate_to_scrm_for
(
  scrm_sch_rate_type     *curr_rate,           /* current r-sch rate       */
  scrm_sch_rate_type     *rate_index           /* wanted r-sch rate        */
)
{
  scrm_sch_frame_type     max_fr_rate;         /* max r-sch fr rate        */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    max_fr_rate is the maximum R-SCH mobile can support right now, given
    the max_mux option negotiated at call startup and from scrm table.
  -------------------------------------------------------------------------*/
  if( sch0_frame_info.current_f_sch >= SCRM_SCH_FRAME_MAX )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "BAD current F-SCH rate");
    return SCRM_SCH_RATE_NONE;
  } 
  max_fr_rate = MIN(sch0_frame_info.max_r_sch,
                       scrm_sch_frame_matrix[sch0_frame_info.current_f_sch]);

  /*lint -save -e641*/
  if( (sch0_frame_info.current_r_sch >= (SCRM_SCH_FRAME_MAX-1)) ||
	  (max_fr_rate >= (SCRM_SCH_FRAME_MAX-1)) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "BAD rate");
    return SCRM_SCH_RATE_NONE;
  }
  /*lint -restore*/

  /*-------------------------------------------------------------------------
    See if there are any power considertions...
    Get the R-SCH power level, and see what R-SCH mobile can support.
  -------------------------------------------------------------------------*/
  if (sch0_frame_info.r_sch_double) 
  {
    *rate_index = scrm_double_sch_rate_matrix[max_fr_rate];
    *curr_rate  = scrm_double_sch_rate_matrix[sch0_frame_info.current_r_sch];
  }
  else
  {
    *rate_index = scrm_single_sch_rate_matrix[max_fr_rate];
    *curr_rate  = scrm_single_sch_rate_matrix[sch0_frame_info.current_r_sch];
  }

  if (sch0_frame_info.r_sch_enabled == FALSE)
  {
    /*---------------------------------------------------------------------
      Reverse SCH not configured.  Cannot SCRM.
    ---------------------------------------------------------------------*/
    *rate_index = SCRM_SCH_RATE_NONE;
    return(SCRM_SCH_RATE_NONE);
  }
  return(*rate_index);
}

/*===========================================================================
FUNCTION      SCRMI_CPU_RF_CHECK

DESCRIPTION   Called to see if we should SCRM due to CPU/RF conditions.

DEPENDENCIES  Called when at least some data to send (either rexmits or
              new data)

RETURN VALUE  None.
  
SIDE EFFECTS  None
===========================================================================*/
boolean scrmi_cpu_rf_check
(
  word                  bytes_to_send,
  scrm_sch_rate_type    curr_rate,
  scrm_sch_rate_type    scrm_rate
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    See if the 1-second timer between SCRM's due to event 1 (CPU/RF) is 
    still active.  If so, return FALSE.
  -------------------------------------------------------------------------*/
  if (ds_scrm_cpu_rf_timer > 0)
  {
    return(FALSE);
  }
  /*-------------------------------------------------------------------------
    It is okay to SCRM.  Do not SCRM if:
     
      - the rate to SCRM for is RATE_NONE.
      - the rate to SCRM for is the current rate
      - not in a burst, but less than trigger amount of bytes to send.
      
      Note that this functin should only be called if there is at least
      one byte of data to send.  Bytes_to_send is only an indication of 
      new data bytes to send, and does not include rexmits.
  -------------------------------------------------------------------------*/
  if ((scrm_rate == SCRM_SCH_RATE_NONE) || (scrm_rate == curr_rate))
  {
    return(FALSE);
  }

  if ((curr_rate == SCRM_SCH_RATE_NONE) && (bytes_to_send < SCRM_MIN_BYTES))
  {
    return(FALSE);
  }

  /*-------------------------------------------------------------------------
    Want to SCRM for a non-zero rate which is different than current rate.
    Also know there is at least one-byte to send (dependency of this 
    function).
  -------------------------------------------------------------------------*/
  return(TRUE);
}/* scrmi_cpu_rf_check() */

/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_SCRM_REG_FUNCS

DESCRIPTION   SCRM Registers various callbacks with MC. Called only at 
              startup.

DEPENDENCIES  None

RETURN VALUE  None.
  
SIDE EFFECTS  None
===========================================================================*/
void ds707_scrm_reg_funcs(void)
{
  mccsch_reg_sch_config_change_cb(ds707_scrm_mc_cback);
  mccsch_reg_rsch_assign_cb(ds707_scrm_escam_rxed);
  mccsch_reg_rsch_extension_cb(
      ds707_scrm_extend_burst,
      DS707_SCRM_EXTENSION_TIME);
} /* ds707_scrm_reg_funcs() */


/*===========================================================================
FUNCTION      DS707_SCRM_CALL_INIT

DESCRIPTION   Called when a new data call is started.

DEPENDENCIES  None

RETURN VALUE  None.
  
SIDE EFFECTS  None
===========================================================================*/
void ds707_scrm_call_init(void)
{
  const caii_srv_cfg_type  *config_ptr = NULL; /* pointer to SNM config    */
  caii_srv_cfg_type  config_local;             /* copy of SNM config       */
  uint8                     qcscrm_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_QCSCRM_VAL,(void *)&qcscrm_val,
                                          DS707_NON_ARRAY);
  /*-------------------------------------------------------------------------
    Lock interrupts/tasks to protect SCRM global data. This function will 
    get called in interrupt context in 1x and so using ISR lock for mutual
    exclusion.
  -------------------------------------------------------------------------*/
  REX_ISR_LOCK(&ds707_scrm_global_crit_sect);
  
  /*-------------------------------------------------------------------------
    Clear the flag for sending a SCRM. This should be cleared at beginning
    of every call.
  -------------------------------------------------------------------------*/
  ds_scrm_now = FALSE;

  /*-------------------------------------------------------------------------
    See if R-SCH is even possible.  Can be prevented either by service
    negotiation or AT command.
  -------------------------------------------------------------------------*/
  config_ptr = snm_get_current_config();
  if (config_ptr == NULL)
    return;
  memscpy(&config_local, sizeof(caii_srv_cfg_type), 
          config_ptr, sizeof(caii_srv_cfg_type));

  if ((config_local.rev_sch_cc_incl == FALSE)         ||
      (config_local.num_rev_sch == 0)                 ||
      (qcscrm_val == FALSE)
     )
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Current config does not allow R-SCH");
    sch0_frame_info.r_sch_enabled = FALSE;
  }
  else
  {
    sch0_frame_info.r_sch_enabled = TRUE;
    
    /*-----------------------------------------------------------------------
      Find out what the max allowable R-SCH is.
    -----------------------------------------------------------------------*/
    switch(config_local.rev_sch[0].rev_sch_mux)
    {
      case CAI_MUX_1X_RS1:
      case CAI_MUX_1X_RS2:
        sch0_frame_info.max_r_sch    = SCRM_SCH_FRAME_1;
        sch0_frame_info.r_sch_double = FALSE;
        break;
      
      case CAI_MUX_2X_RS1_SINGLE:
      case CAI_MUX_2X_RS2_SINGLE:
        sch0_frame_info.max_r_sch    = SCRM_SCH_FRAME_2;
        sch0_frame_info.r_sch_double = FALSE;
        break;
      
      case CAI_MUX_2X_RS1_DOUBLE:
      case CAI_MUX_2X_RS2_DOUBLE:
        sch0_frame_info.max_r_sch    = SCRM_SCH_FRAME_1;
        sch0_frame_info.r_sch_double = TRUE;
        break;
      
      case CAI_MUX_4X_RS1_SINGLE:
      case CAI_MUX_4X_RS2_SINGLE:        
       sch0_frame_info.max_r_sch = SCRM_SCH_FRAME_4;
       sch0_frame_info.r_sch_double = FALSE;
       break;

    
      case CAI_MUX_4X_RS1_DOUBLE:
      case CAI_MUX_4X_RS2_DOUBLE:
        sch0_frame_info.max_r_sch    = SCRM_SCH_FRAME_2;
        sch0_frame_info.r_sch_double = TRUE;
        break;
      
      case CAI_MUX_8X_RS1_SINGLE:
      case CAI_MUX_8X_RS2_SINGLE:
        sch0_frame_info.max_r_sch    = SCRM_SCH_FRAME_8;
        sch0_frame_info.r_sch_double = FALSE;
        break;
      
      case CAI_MUX_8X_RS1_DOUBLE:
      case CAI_MUX_8X_RS2_DOUBLE:
        sch0_frame_info.max_r_sch    = SCRM_SCH_FRAME_4;
        sch0_frame_info.r_sch_double = TRUE;
        break;
      
      case CAI_MUX_16X_RS1_DOUBLE:
      case CAI_MUX_16X_RS2_DOUBLE:
        sch0_frame_info.max_r_sch    = SCRM_SCH_FRAME_8;
        sch0_frame_info.r_sch_double = TRUE;
        break;
      
#ifdef FEATURE_IS2000_REL_A 
      case CAI_MUX_VARIABLE:
        sch0_frame_info.max_r_sch = 
         scrmi_convert_max_rate(config_local.rev_sch[0].sch_chn_cfg.max_rate);
        sch0_frame_info.r_sch_double = TRUE;
        break;
#endif
      default:
        DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Unknown R-SCH max rate %d",
                        config_local.rev_sch[0].rev_sch_mux);
        
        sch0_frame_info.max_r_sch    = SCRM_SCH_FRAME_8;
        sch0_frame_info.r_sch_double = TRUE;
        break;
    }
    
    switch(config_local.for_sch[0].for_sch_mux)
    {
      case CAI_MUX_1X_RS1:
      case CAI_MUX_2X_RS1_SINGLE:
      case CAI_MUX_4X_RS1_SINGLE: 
      case CAI_MUX_8X_RS1_SINGLE: 
      case CAI_MUX_1X_RS2:
      case CAI_MUX_2X_RS2_SINGLE: 
      case CAI_MUX_4X_RS2_SINGLE: 
      case CAI_MUX_8X_RS2_SINGLE: 
        sch0_frame_info.f_sch_double = FALSE;
        break;
            
      case CAI_MUX_2X_RS1_DOUBLE: 
      case CAI_MUX_4X_RS1_DOUBLE: 
      case CAI_MUX_8X_RS1_DOUBLE: 
      case CAI_MUX_16X_RS1_DOUBLE:
      case CAI_MUX_2X_RS2_DOUBLE: 
      case CAI_MUX_4X_RS2_DOUBLE: 
      case CAI_MUX_8X_RS2_DOUBLE: 
#ifdef FEATURE_IS2000_REL_A 
      case CAI_MUX_VARIABLE:
#endif
        sch0_frame_info.f_sch_double = TRUE;
        break;
      default:
        sch0_frame_info.f_sch_double = TRUE;
        break;
    }

    /*-----------------------------------------------------------------------
      Until SCAM shows up, the SCH's are not yet assigned.
    -----------------------------------------------------------------------*/
    sch0_frame_info.current_f_sch = SCRM_SCH_FRAME_NONE;
    sch0_frame_info.current_r_sch = SCRM_SCH_FRAME_NONE;

    /*-----------------------------------------------------------------------
      The timer is set as a default to 1-second at the end of the routine.
    -----------------------------------------------------------------------*/
    ds_scrm_timer            = SCRM_FRAMES_PER_SEC;
    ds_scrm_timer_from_bs    = FALSE;
    ds_scrm_cpu_rf_timer     = 0;
    ds_scrm_awaiting_burst_start = FALSE;

  } /* if R-SCH is enabled */

  /*-------------------------------------------------------------------------
    Unlock interrupts/tasks.
  -------------------------------------------------------------------------*/
  REX_ISR_UNLOCK(&ds707_scrm_global_crit_sect);
} /* ds707_scrm_call_init() */

/*===========================================================================
FUNCTION      DS707_SCRM_RLP_CBACK

DESCRIPTION   Called by RLP every 20 msec.  See if vars allow us to send a
              SCRM.

DEPENDENCIES  None

RETURN VALUE  None.
  
SIDE EFFECTS  None
===========================================================================*/
void ds707_scrm_rlp_cback
(
  uint32     bytes_to_send                       /*bytes RLP needs to xmit */
)
{
  scrm_sch_rate_type    curr_rate;               /* current r-sch rate     */ 
  scrm_sch_rate_type    scrm_rate;               /* rate to scrm for       */
  boolean               send_msg = FALSE;        /* send msg to mc?        */
  mc_queue_cmd_status_type queue_status;
  uint8                    qcscrm_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_QCSCRM_VAL,(void *)&qcscrm_val,
                                          DS707_NON_ARRAY);
  /*-------------------------------------------------------------------------
    See if user has turned off SCRM's (for testing purposes).
  -------------------------------------------------------------------------*/
  if (qcscrm_val == FALSE)
  {
    return;
  }

  /*-------------------------------------------------------------------------
    This is the SCRM clock-tick, since it is called every 20 msec by RLP.
    Decrement the SCRM timers if necessary.  Also get the cpu rf info.
    this is done every 20 msec also, as the RF conditions are averaged over
    the last n frames.
  -------------------------------------------------------------------------*/
  if (ds_scrm_timer > 0)
  {
    ds_scrm_timer--;
  }

  if (ds_scrm_cpu_rf_timer > 0)
  {
    ds_scrm_cpu_rf_timer--;
  }

  /*-------------------------------------------------------------------------
    Are there are any bytes to send (i.e., at least one?)
  -------------------------------------------------------------------------*/
  if (dsrlp_has_data_to_send(DSRLP_FIXED_SR_ID_INDEX) == FALSE)
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Waiting for general clock to expire or waiting for burst to start.
  -------------------------------------------------------------------------*/
  if ((ds_scrm_timer > 0)                       ||
      (ds707_rrm_awaiting_mc == TRUE)             ||
      (ds_scrm_awaiting_burst_start == TRUE))
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Only want to SCRM if SCRM for non-zero value.
  -------------------------------------------------------------------------*/
  (void) scrmi_rate_to_scrm_for(&curr_rate, &scrm_rate);

  if (scrm_rate == SCRM_SCH_RATE_NONE)
  {
    return;
  }

  /*-------------------------------------------------------------------------
    At this point, can SCRM.  Two reasons.  First is because we have to
    SCRM right now (i.e., for burst-extension).  Second reason is because
    the CPU/RF requires it.
  -------------------------------------------------------------------------*/
  if (ds_scrm_now == TRUE)
  {
    send_msg = TRUE;
  }
  else if ((ds_scrm_cpu_rf_timer == 0) &&
           (scrmi_cpu_rf_check((word)bytes_to_send, curr_rate, scrm_rate) == TRUE))
  {
    send_msg = TRUE;
  }

  if (send_msg == TRUE)
  {
    ds707_rrm_awaiting_mc  = TRUE;
    
    queue_status = mc_queue_cmd(MC_RES_REQ_F);
    if (queue_status == MC_FREE_QUEUE_EMPTY) 
    {
      ERR_FATAL( "No MC command links available", 0, 0, 0);
    }
    else if (queue_status != MC_CMD_QUEUED) 
    {
      ERR_FATAL( "Unknow status return from mc_queue_cmd()", 0, 0, 0);
    }

    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Ask mc for a blob");
  }
} /* ds707_scrm_rlp_cback() */


/*===========================================================================
FUNCTION      DS707_SCRM_ESCAM_RXED

DESCRIPTION   Called by MC when an ESCAM with an R-SCH assignment is rx'ed.
              This ESCAM indicates our last SCRM was answered, and so the
              appropriate state vars should be updated.

DEPENDENCIES  None

RETURN VALUE  None.
  
SIDE EFFECTS  None
===========================================================================*/
void ds707_scrm_escam_rxed(byte rev_sch_duration)
{
  /*-------------------------------------------------------------------------
    We received an ESCAM, and so a burst will be starting soon.  We cannot
    SCRM while this is occurring.
    
    However, we can cancel the general SCRM timer, as that SCRM has been
    answered.
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    Enter critical section to protect SCRM global data.
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&ds707_scrm_global_crit_sect);

  ds_scrm_now   = FALSE;
  if (rev_sch_duration == 0) 
  {
    ds_scrm_awaiting_burst_start = FALSE;
  }
  else
  {
    ds_scrm_awaiting_burst_start = TRUE;
  
    if (ds_scrm_timer_from_bs == FALSE)
    {
      ds_scrm_timer = 0;
    }
  }

  /*-------------------------------------------------------------------------
    Leave critical section.
  -------------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION(&ds707_scrm_global_crit_sect);

  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "r-sch escam rxed");
} /* ds707_scrm_escam_rxed() */


/*===========================================================================
FUNCTION      DS707_SCRM_EXTEND_BURST

DESCRIPTION   Called by MC when there are x frames or less before end of
              this burst assignment.  Data should SCRM regardless, in order
              to extend this burst.

DEPENDENCIES  None

RETURN VALUE  None.
  
SIDE EFFECTS  None
===========================================================================*/
void ds707_scrm_extend_burst(void)
{
  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "mc sez extend r-sch burst");
  ds_scrm_now = TRUE;
} /* ds707_scrm_extend_burst() */


/*===========================================================================
FUNCTION      DS707_SCRM_MC_CBACK

DESCRIPTION   Called by MC when the SCH config changes.

              Does two things: 1) calls txc so that it can do it's R-SCH
              throttling calculations.  2) Calculates R-SCH and F-SCH
              parameters for future calculations.
             
              Do not need to see if a SCRM needs to be sent in this function.
              RLP 3 will call ds_scrm_sch_chng_cback() every 20 msec, and 
              that will trigger a SCRM if necessary.
             
DEPENDENCIES  None.

RETURN VALUE  NONE
             
SIDE EFFECTS  None.
===========================================================================*/
void ds707_scrm_mc_cback
(
  mccsch_sch_info_type* sch_info_ptr
)
{
  /*-------------------------------------------------------------------------
    Enter critical section to protect SCRM global data.
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&ds707_scrm_global_crit_sect);

  txc_inform_sch_chng();

  /*-------------------------------------------------------------------------
    Record the F-SCH and R-SCH values.
  -------------------------------------------------------------------------*/
  if (sch_info_ptr->f_sch_0.active == TRUE)
  {
    sch0_frame_info.current_f_sch =           
            scrmi_convert_sch_frame(sch_info_ptr->f_sch_0.sch_rate, FWD_SCH);
  }
  else
  {
    sch0_frame_info.current_f_sch = SCRM_SCH_FRAME_NONE;
  }

  /*-------------------------------------------------------------------------
    Checking to see if timers need to be cancelled/re-started.
  -------------------------------------------------------------------------*/
  if (sch_info_ptr->r_sch_0.active == TRUE)
  {
    sch0_frame_info.current_r_sch = 
            scrmi_convert_sch_frame(sch_info_ptr->r_sch_0.sch_rate, REV_SCH);

    /*-----------------------------------------------------------------------
      Burst has started.
    -----------------------------------------------------------------------*/
    if (sch_info_ptr->r_sch_0.changed == TRUE)
    {
      ds_scrm_awaiting_burst_start = FALSE;
      ds_scrm_cpu_rf_timer = SCRM_FRAMES_PER_SEC;
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      For safety, always set current_r_sch to FRAME_NONE.  If in fact this
      is the end of some burst, then see if global vars need to be modified.
    -----------------------------------------------------------------------*/
    if (sch_info_ptr->r_sch_0.changed == TRUE)
    {
      if (sch0_frame_info.current_r_sch == SCRM_SCH_FRAME_NONE)
      {
        ds_scrm_awaiting_burst_start  = FALSE;
      }
    }
    ds_scrm_now = FALSE;
    ds_scrm_cpu_rf_timer = 0;
    sch0_frame_info.current_r_sch = SCRM_SCH_FRAME_NONE;
  }

  /*-------------------------------------------------------------------------
    Leave critical section.
  -------------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION(&ds707_scrm_global_crit_sect);
} /* ds707_scrm_mc_cback() */

/*===========================================================================
FUNCTION     DS707_SCRM_MAX_ALLOWABLE_SCH_RATE

DESCRIPTION  Look at the table scrm_sch_rate_matrix[] for the max allowable
             rate the MSM can do in the Rev. SCH. 
             
             Do not use current_f_sch0, etc.  That's because mux updates
             itself asynchronously.  Use the values MUX returns, by
             using the mux information.

DEPENDENCIES None.

RETURN VALUE Max number of MuxPDUs.

SIDE EFFECTS None.
===========================================================================*/
byte ds707_scrm_max_allowable_sch_rate
(
  boolean             rxc_sch_data_is_valid,
  dec_rc_type         sch_rc,
  dec_sch_rate_type   dec_sch_rate,
  boolean             double_size,
  boolean             local_rxc_sup_dec_int_enable
)
{
  scrm_sch_frame_type f_sch_rate, max_sch_rate;
  byte                max_num_pdu = 0;
  uint8               qctrtl_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_QCTRTL_VAL,(void *)&qctrtl_val,
                                          DS707_NON_ARRAY);
  if (qctrtl_val == FALSE)
  {
    /*----------------------------------------------------------------------- 
     User has turned off the R-SCH throttling mechanism for testing purposes.
     Return the maximum possible number of PDU's, under any circumstances.
    -----------------------------------------------------------------------*/
    return(8);
  }

  if ((local_rxc_sup_dec_int_enable == FALSE) ||
      (rxc_sch_data_is_valid == FALSE))
  {
    f_sch_rate = SCRM_SCH_FRAME_NONE;
  }
  else
  {
    switch (dec_sch_rate)
    {
    case DEC_SCH_RATE_1X:
        f_sch_rate = SCRM_SCH_FRAME_1;
        break;
      
      case DEC_SCH_RATE_2X:
        f_sch_rate = (scrm_sch_frame_type)
                       ((double_size)?SCRM_SCH_FRAME_1:SCRM_SCH_FRAME_2);
        break;
      
      case DEC_SCH_RATE_4X:
        f_sch_rate = (scrm_sch_frame_type)
                       ((double_size)?SCRM_SCH_FRAME_2:SCRM_SCH_FRAME_4);
        break;
      
      case DEC_SCH_RATE_8X:
        f_sch_rate = (scrm_sch_frame_type)
                       ((double_size)?SCRM_SCH_FRAME_4:SCRM_SCH_FRAME_8);
        break;
      
      case DEC_SCH_RATE_16X:
        f_sch_rate = SCRM_SCH_FRAME_8;
        break;
    
#ifdef FEATURE_32X_SCH_DATA_RATE
      case DEC_SCH_RATE_32X:
        f_sch_rate = SCRM_SCH_FRAME_VARIABLE;
        break;
#endif      
      default:
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid RXC SCH rate");
        f_sch_rate = SCRM_SCH_FRAME_8;
        break;
    }
  }
  
  max_sch_rate = scrm_sch_frame_matrix[f_sch_rate];

  if (max_sch_rate == SCRM_SCH_FRAME_NONE)
  {
    max_num_pdu = 0;
  }
  else
  {
    switch( max_sch_rate)
    {
      case SCRM_SCH_FRAME_1:
        max_num_pdu =1; 
        break;    
      case SCRM_SCH_FRAME_2: 
        max_num_pdu =2; 
        break;
      case SCRM_SCH_FRAME_4: 
        max_num_pdu =4; 
        break;
      case SCRM_SCH_FRAME_8: 
        max_num_pdu =8; 
        break;
      case SCRM_SCH_FRAME_VARIABLE: 
        max_num_pdu = 8; 
        break;
      default:
        DATA_IS707_MSG1( MSG_LEGACY_MED, 
                         "ds707_scrm_max_allowable_sch_rate: "
                         "ignoring max_sch_rate type %d", 
                          max_sch_rate );
	break;
    }
  }

  return (max_num_pdu);
} /* ds707_scrm_max_allowable_sch_rate() */

/*===========================================================================
FUNCTION      DS707_SCRM_BUILD_BLOB

DESCRIPTION   This builds the SCRM blob to be sent to the Base Station.  This
              is called from the RRM module.

DEPENDENCIES  None

RETURN VALUE  FALSE = MC should send nothing to BS.  TRUE means a BLOB
              should be sent.  In the TRUE case, a blob_size of zero
              means MC should send a SCRM that cancels the assigned R-SCH
              configuration.
  
SIDE EFFECTS  None
===========================================================================*/
boolean ds707_scrm_build_blob
(
  uint8 *blob_size,   /* eventual size of the blob                         */
  uint8 *blob_ptr,    /* points to BLOB that is to be filled out           */
  word   so           /* SCRM is for this active service option            */
)
{
  boolean                ret_val = FALSE;
  scrm_sch_rate_type     curr_rate;
  scrm_sch_rate_type     scrm_rate;
  scrm_sch_rate_type     rpc_rating;
  uint8                  qcscrm_val = 0;
  uint8                  print_rate = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_QCSCRM_VAL,(void *)&qcscrm_val,
                                          DS707_NON_ARRAY);
  /*-------------------------------------------------------------------------
    Verify input parameters.
  -------------------------------------------------------------------------*/
  ASSERT(blob_size != NULL);
  ASSERT(blob_ptr  != NULL);

  if (qcscrm_val == FALSE)
  {
    return(FALSE);
  }


  /*-------------------------------------------------------------------------
    Enter critical section to protect SCRM global data.
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&ds707_scrm_global_crit_sect);

  /*-------------------------------------------------------------------------
    Build the requested BLOB only if timer is not active and there is data
    to send.  A retry delay may have come in between the msg to MC and
    MC calling this function.
  -------------------------------------------------------------------------*/
  (void) scrmi_rate_to_scrm_for(&curr_rate, &scrm_rate);

  rpc_rating = scrmi_get_rpc_rating(scrm_rate);
  DATA_IS707_MSG2(MSG_LEGACY_HIGH, "SCRM %d, RPC_Scrm: %d",scrm_rate,rpc_rating);
  scrm_rate = MIN(rpc_rating,scrm_rate);

  if ((ds_scrm_timer == 0) && (scrm_rate != SCRM_SCH_RATE_NONE))
  {
    /*-----------------------------------------------------------------------
      Want to build a SCRM.  There is data to send, and there is either
      capacity for higher R-SCH, or we have more than we can handle.

      When asking for a SCRM, always ask for infinite duration.  Also always
      ask for the max R-SCH supportable, given the current F-SCH.

      Currently only one sr_id is supported.
    -----------------------------------------------------------------------*/
    blob_ptr[0] = SCRM_CTL_BYTE;

    blob_ptr[1]  = dsrlp_get_sr_id(DSRLP_FIXED_SR_ID_INDEX) << 5;

    /*lint -save -e641*/
    blob_ptr[1] |= scrm_pref_rate[scrm_rate - 1] << 1;
    /*lint -restore*/

    blob_ptr[1] |= 0x01;

    blob_ptr[2]  = 0xFF;

    *blob_size = 3;
     ret_val = TRUE;
  }

  /*-------------------------------------------------------------------------
    Always wait one second, even if no BLOB was built.  This is so that
    data does not pester MC too frequently with SCRM requests.  Do not reset
    timer if it's already active (i.e, do not overwrite a BS ordered retry
    delay).
  -------------------------------------------------------------------------*/
  if (ds_scrm_timer == 0)
  {
    ds_scrm_timer         = SCRM_FRAMES_PER_SEC;
    ds_scrm_timer_from_bs = FALSE;

    if ((ds_scrm_now == FALSE) && (ds_scrm_cpu_rf_timer == 0))
    {
      ds_scrm_cpu_rf_timer = SCRM_FRAMES_PER_SEC;
      /*---------------------------------------------------------------------
        We decided to SCRM; but let's check if its worth it.
        If current rate is same as the rate we are planning to SCRM, its of
        no use sending scrm.
        So if current rate == SCRM Rate,
          make ret_val as FALSE. So that we donot scrm.
      ---------------------------------------------------------------------*/
      if (curr_rate == scrm_rate) 
      {
          ds_scrm_timer = 0;
          ret_val = FALSE;  
      }
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "SCRM due to CPU-RF event");
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "SCRM due to EXTENSION");
    }
  }
  ds_scrm_now = FALSE;

  /*-------------------------------------------------------------------------
    Leave critical section.
  -------------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION(&ds707_scrm_global_crit_sect);

  /*-------------------------------------------------------------------------
    Do the MSG printing outside of the INTLOCK'd code.
  -------------------------------------------------------------------------*/
  if (ret_val == TRUE)
  {
    switch(blob_ptr[1])
    {
     /*-----------------------------------------------------------------
        The first three bits specify the sr_id and next 4bits specify 
        the rate.
        ex: 0x2b: 001    0101  1 
                 sr_id  32x  
      ------------------------------------------------------------------*/
      case 0x2b :
        print_rate = 32; 
        break;
      case 0x29:
        print_rate = 16;
        break;
      case 0x27:
        print_rate = 8;
        break;
      case 0x25:
        print_rate = 4;
        break;
      case 0x23:
        print_rate = 2;
        break;
      case 0x21:
        print_rate = 1;
        break;
    }

    DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Sending SCRM BLOB  %dx", print_rate);
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "No SCRM: R-SCH OK or retry delay");
  }

  return(ret_val);
}/* ds707_scrm_build_blob() */

/*===========================================================================
FUNCTION     DS707_SCRM_RETRY_PROCESSING

DESCRIPTION  Called to clear SCRM timer.

DEPENDENCIES None.

RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void ds707_scrm_retry_processing(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Enter critical section to protect SCRM global data.
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&ds707_scrm_global_crit_sect);  
  
  ds_scrm_timer         = 0;
  ds_scrm_timer_from_bs = FALSE;

  /*-------------------------------------------------------------------------
    Leave critical section.
  -------------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION(&ds707_scrm_global_crit_sect);
} /* ds707_scrm_retry_processing() */

/*===========================================================================
FUNCTION      DS707_SCRM_RETRY_DELAY

DESCRIPTION   Called when the SCRM retry delay is changed from the BS.

DEPENDENCIES  None

RETURN VALUE  None.
  
SIDE EFFECTS  None
===========================================================================*/
void ds707_scrm_retry_delay
(
  retry_delay_type retry_parm
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Enter critical section to protect SCRM global data.
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&ds707_scrm_global_crit_sect);

  if (retry_parm.infinite_delay == TRUE)
  {
    /*-----------------------------------------------------------------------
       if the delay is infinite, set the timer to the max value (at this
       value, it will take > 900 days for it to expire)
    -----------------------------------------------------------------------*/
    ds_scrm_timer = SCRM_MAX_TIMER_TIME;
  }
  else
  {
    if (retry_parm.retry_delay_duration == 0)
    {
      /*---------------------------------------------------------------------
        Base station wants us to SCRM now, so clear any timers which are 
        holding us back
      ---------------------------------------------------------------------*/
      ds_scrm_timer         = 0;
      ds_scrm_cpu_rf_timer  = 0;
      ds_scrm_timer_from_bs = FALSE;
    }
    else
    {
      /*---------------------------------------------------------------------
        Base station has a specific delay for us.  Clear CPU/RF timer, as
        BS says it's okay to SCRM after the delay ends.
      ---------------------------------------------------------------------*/
      ds_scrm_timer = 
                   (retry_parm.retry_delay_duration/SCRM_MSEC_PER_FRAME) + 1;
      ds_scrm_cpu_rf_timer  = 0;
      ds_scrm_timer_from_bs = TRUE;
    }
  }

  /*-------------------------------------------------------------------------
    Leave critical section.
  -------------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION(&ds707_scrm_global_crit_sect);
} /* ds707_scrm_retry_delay() */

byte ds_scrm_max_allowable_sch_rate
(
  boolean             rxc_sch_data_is_valid,
  dec_rc_type         sch_rc,
  dec_sch_rate_type   dec_sch_rate,
  boolean             double_size,
  boolean             local_rxc_sup_dec_int_enable
)
{
  byte rate;

  rate = ds707_scrm_max_allowable_sch_rate(rxc_sch_data_is_valid,
                                           sch_rc,
                                           dec_sch_rate,
                                           double_size,
                                           local_rxc_sup_dec_int_enable);

  return rate;
}


#endif /* FEATURE_IS2000_R_SCH */
#endif /* FEATURE_DATA_IS707   */

