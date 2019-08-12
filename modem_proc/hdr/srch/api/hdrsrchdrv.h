#ifndef HDRSRCHDRV_H
#define HDRSRCHDRV_H

/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       H D R    S R C H    D R I V E R S

                          Search Drivers Header File

GENERAL DESCRIPTION

   This file contains the databases, typedefs, enums and defines used in
   the HDR Pilot Search operations.

EXTERNALIZED FUNCTIONS

REGIONAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2000 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/srch/api/hdrsrchdrv.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ------------------------------------------------------- 
10/22/2014   ljl     Fixed compiler warning.
09/03/2013   wsh     Replaced FEATURE_DIME/TRITON_BOLT_MODEM with HDR version
04/22/2013   arm     Added changes for DSDA coexistence manager.
11/13/2012   vlc     Added srch_params to hdrsrchdrv_struct_type and a prototype
03/19/2012   ljl     Fixed Dime compiler errors.
02/01/2012   mbs     Modified burst length for connected mode measurement
01/11/2012   ljl     Added the parameter for search priority to hdrsrchdrv_enable_srch4().
11/17/2011   ljl     Removed 9k specific code.
07/07/2011   kss     Removed shutdown message for NikeL as it is not used.
04/27/2011   smd     IRAT L2DO updates for NikeL.
04/12/2011   smd     Used srch4 search structure. 
11/18/2010   smd     Added support for offline search.
06/25/2010   smd     Supported hstr slam in slam cfg message.
06/21/2010   cnx     Fixed compiler warnings.
04/21/2010   smd     Increased HDR_MAX_SRCH_CNT to 32.
04/15/2010   smd     Added HSTR and MSTR in MSTR_SLAM message.
12/07/2009   ljl     Fixed the hyperspace mode.
10/01/2009   ljl     Processed srch app cfg rsp msg.
09/17/2009   ljl     Included srchfw_msg.h.
09/14/2009   ljl     Supported srch MSGR interface.
07/29/2009   etv     Included hdrfw_msg.h instead of hdrmsg.h
07/15/2009   etv     Moved sw module definitions to hdrmsg.h
05/18/2009   ljl     Updated for firmware MSGR interface.
10/14/2008   lyl     Added macro HDR_SRCH_MAX_PILOT_ENG
09/28/2009   ljl     Saved the demod index before starting search.
09/23/2008   rkc     Added comments for resbuf to use srch_list's res_ptr.
08/29/2008   lyl     Added srch_task_type in hdrsrchdrv_struct_type
08/08/2008   rkc     Added carrier info array to srchdrv data structure.
06/06/2008   rkc     Added storage to save time to search for logging.
10/17/2007   ljl     Merged Rev B changes from branch.
06/18/2007   rkc     Added freq_offset input to hdrsrchdrv_send_srch_list()
04/10/2007   mt      Changes for 1x pilot search support.
03/09/2007   grl     Added search4 registration APIs for firmware downloading.
11/27/2006   jyw     Added hdrsrchdrv_get_mstr_x8.
09/27/2006   jyw     Added interface for interpolated srch results.
06/30/2006   etv     Supported DSP Handoff algorithm changes.
05/31/2005   grl     Removed the channel info stored in hdrsrchrf now.
01/24/2005   ds      Changed minimum energy threshold back to 37 from 34
11/03/2004   mpa     Removed dependency on std.h (merged with sys.h)
06/01/2004   sq      Added APIs to enable/disable SRCH4
11/18/2003   aaj     Cleaned up unused drivers code
10/14/2003   aaj     Added support for search result validation
10/06/2003   aaj     Mainlined T_MSM6500 and HDRSRCH_MULTIPATH_WIN_CENTER
07/11/2003   sq      Added definitions for T_MSM6500
03/11/2003   ajn     Updated Copyright notice
11/06/2002   aaj     Removed unncessary function prototypes
09/18/2002   aaj     Support for multipath window centering for hdr
08/08/2002   aaj     Moved LABS macro to this file for global access
04/29/2002   aaj     Made hdrsrchdrv_set_srch_dump_isr global
02/21/2002   ajn     Added "dump partial results" flag to abort search
02/08/2002   sst     Added prototype for hdrsrchdrv_abort_search
09/05/2001   aaj     Antenna Diversity integrated in the idle state
08/02/2001   aaj     Handle MDSP Halts, Diversity Changes
07/17/2001   aaj     Changed non-coherent integration length to 2 from 1
06/27/2001   aaj     Added prototype for a debug function to check stray fing
06/19/2001   aaj     HDR Shutdown/Startup/Resume/Suspend support added
04/23/2001   aaj     Major changes to drivers to support TAZ3
03/08/2001   aaj     Added dump_cnt to hdrsrchdrv struct
02/28/2001   aaj     Added RX diversity control function
02/07/2001   aaj     Added rx_agc_db function header
12/18/2000   aaj     Changed HDR_SRCH_SELECT to normal search single antenna
11/22/2000   ajn     Code review changes
10/10/2000   aaj     Changed search result's pos variable to int16 from uint16
07/27/2000   ajn     Merged into HDR SEARCH Task codebase
06/27/2000   aaj     Compiled the code successfully
06/21/2000   aaj     Re-organized some of the functions
05/24/2000   aaj     Created this file

==========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "hdr_variation.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif

#include "hdrfw_msg.h"
#include "srchfw_msg.h"
#include "hdrsrchsect.h"
#include "hdrsrchi.h"
#include "srch4drv_t.h"
#include "srch4trans.h"

/*==========================================================================


                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/
#define HDR_SRCH_COH_ACCUM               96        /* steady state Nc */

#define HDR_SRCH_NONCOH_ACCUM_NON_DIV    1         /* steady state Nn */
#define HDR_SRCH_NONCOH_ACCUM_DIV        1         /* steady state Nn */
#define HDR_SRCH_COH_TRUNC               2         /* Coh Truncation value */

#define HDR_SRCH_1X_COH_ACCUM          512         /* steady state Nc */
#define HDR_SRCH_1X_NONCOH_ACCUM_NON_DIV 1         /* steady state Nn */
#define HDR_SRCH_1X_NONCOH_ACCUM_DIV     1         /* steady state Nn */
#define HDR_SRCH_1X_COH_TRUNC            3         /* Coh Truncation value */
#define HDR_SRCH_1X_BURST_LENGTH_NON_DIV 0
#define HDR_SRCH_1X_BURST_LENGTH_DIV     0

#define HDR_SRCH_1X_SEARCH_WINSIZE     100

/*---------------------------*
 * Search specific constants *
 *---------------------------*/
#define HDR_SRCH_BURST_LENGTH_DIV        1
#define HDR_SRCH_BURST_LENGTH_NON_DIV    2
#define HDR_MAX_SRCH_PEAKS               SRCH4_NUM_PEAKS

#ifdef FEATURE_HDR_REVB
#define HDR_MAX_SRCH_CNT        32   /* Max srches sent to h/w at a time */
#else
#define HDR_MAX_SRCH_CNT        16   /* Max srches sent to h/w at a time */
#endif /* FEATURE_HDR_REVB */

#define HDR_MAX_TA_SRCH_CNT      1   /* Max TA srches sent to h/w at a time */
#define HDR_MAX_TRACE_SIZE     128   /* Max num of hypos in TA trace mode */
#define HDR_MAX_TA_WIN_SIZE     64   /* Max num of hypos in TA trace mode */

#define HDR_MAX_SRCH_ENG    0xffff   /* max value of total srch eng */
#define HDR_SRCH_MAX_PILOT_ENG  512   /* The maximum of pilot energy */

#define HDR_MIN_EARLY_PATH_ENG  46   /* min eng to consider a path early */

#define HDR_SRCH_MIN_PATH_ENG   37   /*-11.4dB:min ecio for a path to be valid*/

/*==========================================================================

                        SEARCH PROGRAMMING MACROS

==========================================================================*/

#define  HDR_SRCH_NC_MASK        0xff00
#define  HDR_SRCH_NN_MASK        0x0030
#define  HDR_SRCH_SPLIT_MASK     0x0004
#define  HDR_SRCH_DUAL_MASK      0x0002
#define  HDR_SRCH_ANT_SEL_MASK   0x0001


/*==========================================================================

                        HYPERSPACE CFG MACROS

==========================================================================*/
#define HDRSRCH_SET_HYPERSPACE_MODE   0x1
#define HDRSRCH_RESET_HYPERSPACE_MODE 0x0


/*==========================================================================

                       MACROS USED IN THE MODULE

This section contains macros for the functions used in this module.

==========================================================================*/

/* Absolute value macro. This exists in srchi.h in DMSS but we can not 
   include it in HDRSRCH files. Don't like redefining it, look at it later */
#define LABS_VAL( x ) ( ((x) < 0) ? -(x) : (x) )

/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/

/* Search capture control is a bit field. The register bit
   patterns are: 00 => no refresh, 01 => before, 10=> after */

typedef enum
{
  NO_REFRESH = 0,
  REFRESH_BEFORE,     /* Refresh prior to starting first search. Bit0 */
  REFRESH_AFTER       /* Refresh after the last search. Bit1 */
}
hdrsrchdrv_cap_ctrl_enum_type;

typedef enum
{
  SRCH_STATUS_INACTIVE = 0,     /* search is not active */
  SRCH_STATUS_ABORT_NO_DUMP,    /* search has been aborted, swallow dump */
  SRCH_STATUS_ABORT_WITH_DUMP,  /* search has been aborted, dump wanted */
  SRCH_STATUS_ACTIVE,           /* normal search returning 4 peaks / pilot */
  SRCH_STATUS_TA_RX0,           /* TA search, results in RX0 */
  SRCH_STATUS_TA_RX1            /* TA search, results in RX1 */
}
hdrsrchdrv_srch_status_enum_type;

typedef enum
{
   HDRSRCH_DIVERSITY_SRCH = 0,   /* Diversity search */
   HDRSRCH_NON_DIV_SRCH,         /* Non-diversity search */
   HDRSRCH_PRI_CHAIN_ASET_SRCH   /* Primary chain ASET search */
}
hdrsrchdrv_srch_task_enum_type;

/* Carrier select */
typedef enum
{
   HDRSRCH_CARRIER_0 = SRCH4_CARRIER_0,
   HDRSRCH_CARRIER_1 = SRCH4_CARRIER_1,
   HDRSRCH_CARRIER_2 = SRCH4_CARRIER_2
} hdrsrchdrv_carrier_type;

/* Burst length for search */
typedef enum
{
  HDR_SRCH_SHORT_BURST_LENGTH = 1,
  HDR_SRCH_LONG_BURST_LENGTH = 2
} hdrsrchdrv_burst_len_type;

/*----------------------------------------------------------------------*
                     hdrsrchdrv_params_struct_type
 *----------------------------------------------------------------------*/

typedef struct
{
  uint16                           coh_int;
    /* coherent integration length */

  int8                             noncoh_int;
    /* non-coherent integration length */

  int8                             burst_len;
    /* Pilot burst length              */

  int8                             split_pilot;
    /* split pilot which allows coherent integration of 48 chips and
       non-coherent int of 2 on the same pilot */

  int8                             default_ant;
    /* default antenna to be used if the diversity is not ON */

}
hdrsrchdrv_params_struct_type;


/*----------------------------------------------------------------------*
                     hdrsrchdrv_srch_res_struct_type
 *----------------------------------------------------------------------*/

typedef srch4_srch_res_struct_type hdrsrchdrv_srch_res_struct_type;

/*----------------------------------------------------------------------*
                     hdrsrchdrv_srch_list_struct_type
 *----------------------------------------------------------------------*/
typedef struct
{
    srch4_srch_struct_type            srch_task;
      /* copy fields from srch4 srch type */

    hdrsrch_sect_struct_type*         sect_ptr;
     /* Sector to which this search belongs to. This will be set to null
        if the searches are not sector specific such as acquisition srches */
} hdrsrchdrv_srch_list_struct_type;

/*--------------------------------------*
 * HDR Search Validation Data Structure *
 *--------------------------------------*/

typedef struct
{
    uint32                        total_eng;
      /* Total energy after searching */

    int                           strong_pilot_count;
      /* See how many pilots cross min path threshold */

    uint8                         carrier_idx;
      /* Carrier Index */
}
hdrsrchdrv_srch_val_struct_type;

/*----------------------------------------------------------------------*
                     hdrsrchdrv_carrier_idx_struct_type
 *----------------------------------------------------------------------*/
typedef struct
{
  /* This structure is particulalry useful in RevB.  After srch list has been
     sorted, this structure is filled in with necessary information.  This
     structure's fields are then used to process the srch dump on a per
     carrier index basis. */
  uint8                             carrier_idx;
    /* Carrier Selector Rotator/Sample RAM index */

  uint8                             demod_idx;
    /* Demod carrier index */

  uint8                             srch_task_cnt;
    /* Number of entries in srch list per carrier index */

  boolean                           srch_valid;
    /* TRUE if srch results are valid on a per carrier basis */

  hdrsrchdrv_srch_val_struct_type   srch_val;
    /* Srch validation data structure containing per-carrier accumulated
       energies. */
}
hdrsrchdrv_carr_idx_struct_type;


/*----------------------------------------------------------------------*
                     hdrsrchdrv_struct_type
 *----------------------------------------------------------------------*/

typedef struct
{

  hdrfw_mstr_slam_msg_t        mstr_slam_msg;
    /* MSTR slam cfg message */

#if !( defined T_MSM8960 || defined FEATURE_HDR_DIME_MODEM )
  hdrfw_srch_shutdown_msg_t    srch_shutdown_msg;
    /* Shutdown message */
#endif

  hdrfw_hyperspace_cfg_msg_t   hyperspace_cfg_msg;
    /* Hyperspace cfg message */

}
hdrsrchdrv_fw_msg_struct_type;


/*----------------------------------------------------------------------*
                     hdrsrchdrv_struct_type
 *----------------------------------------------------------------------*/

typedef struct
{
  boolean                           mstr_valid;
    /* flag indicates whether or not the MSTR is valid */

  uint32                            mstr_x8;
    /* AT timer reference (MSTR_RTC_OFFSET) in chipx8 units */

  boolean                           mstr_slam_pending;
    /* Whether waiting for mstr slam response */

  int8                              num_carriers;
    /* Number of distinct carrier indexes encountered in srch list */

  hdrsrchdrv_carr_idx_struct_type   carrier_info[HDRSRCH_MAX_DEMOD_CARRIERS_CNT];
    /* Carrier information stored in srch_list. */

  hdrsrchdrv_srch_list_struct_type  srch_list[HDR_MAX_SRCH_CNT];
    /* Stores the searches to be sent to the DSP. */

  uint8                             list_cnt;
    /* Number of searches on the search list */

  hdrsrchdrv_srch_task_enum_type    srch_task_type;
  /* To indicate different type of search tasks */

  uint8                             dump_cnt;
    /* count number of dumps. Note that this is modulo 256 as it is
       an unsigned byte variable */

  hdrsrchdrv_srch_status_enum_type  active_srch;
    /* Enum to indicate whether or not a search is active in the hardware.
       It also denotes the kind of search that is active right now */

  hdrsrchdrv_srch_res_struct_type   resbuf[HDR_MAX_SRCH_CNT];
    /* HDR searcher result buffer.
       Note: After srch dump, this should not be accessed directly due to
             potential sorting of the srch_list.  Instead, use srch_list's
             res_ptr. */

  hdrsrchdrv_params_struct_type     params;
    /* common search parameters */

  hdrsrch_system_enum_type          system;
    /* the search list comprised of DO or 1x pilots */

  timetick_type                     time_to_search_start_sclk;
    /* Start of srch time in sclks */

  timetick_type                     time_to_search_sclk;
    /* Search time in sclks timed from start of submitted srch to dump, if any. */

  hdrsrchdrv_fw_msg_struct_type     fw_msg;
    /* Firmware messages */

  uint16                            last_Captured_Sample_RTCCx2;
    /* last captured sample RTC in cx2 */                                                        

  srch4_hdr_search_type             srch_params;
    /* parameters used to call Search 4 start search function */

} hdrsrchdrv_struct_type;


/*==========================================================================

                    EXTERN VARIABLES and FUNCTIONS

This section contains variables that are accessible outside this module
==========================================================================*/

/*------------------*
 * extern variables *
 *------------------*/

extern hdrsrchdrv_struct_type hdrsrchdrv;
/* HDR search driver variables */

/*------------------*
 * extern functions *
 *------------------*/
/*=========================================================================

FUNCTION     : HDRSRCHDRV_SEND_SRCH_LIST

DESCRIPTION  : This function accepts a list  of search parameters as an
               input and programs the searches to the DSP if search is
               not already pending with the DSP. It also accepts the SRAM
               capture control flag as a parameter which controls whether
               the sample ram will be refreshed before or after the searches
               are executed.

               This function copies the parameters to the DSP RAM and writes
               the search count register to start searching.

DEPENDENCIES : None

INPUTS       : srch_list[] => This list contains the search parameters
               cap_ctrl    => Sample Buffer capture control
               srch_cnt    => Number of searches in the list
               search_system => type of system to be searched, DO or 1x
               freq_offset => Frequency offset ( 4.6875 Hz/LSB )

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchdrv_send_srch_list
(
  hdrsrchdrv_srch_list_struct_type *slist,      /* search list */
  hdrsrchdrv_cap_ctrl_enum_type     cap_ctrl,  /* search capture control*/
  uint8                             srch_cnt,      /* number of searches    */
  hdrsrch_system_enum_type          search_system, /* system type for search */
  int16                             freq_offset, /* freq offset 4.6875 Hz/LSB */
  boolean                           atomic_search /* Perform atomic search 
                                                    or can reschedule? */
);

/*=========================================================================

FUNCTION     : HDRSRCHDRV_INIT

DESCRIPTION  : This function initializes the HDR Search Driver routines

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchdrv_init( void );

/*=========================================================================

FUNCTION     : HDRSRCHDRV_SET_BURST_LEN

DESCRIPTION  : This function initializes the burst length for the
next search operation

DEPENDENCIES : None

INPUT        : Burst length to use next

RETURN VALUE : None

SIDE EFFECTS
  Retains the value till it is set again

=========================================================================*/

void hdrsrchdrv_set_burst_len( hdrsrchdrv_burst_len_type burst_len );

/*===========================================================================

FUNCTION        HDRSRCHDRV_UPDATE_MSTR_X8

DESCRIPTION     This function updates the MSTR variable maintained by the
                searcher driver.

DEPENDENCIES    none

RETURN VALUE    none

SIDE EFFECTS    none

===========================================================================*/

void hdrsrchdrv_update_mstr_x8( void );


/*===========================================================================

FUNCTION        HDRSRCHDRV_GET_MSTR_X8

DESCRIPTION     The function returns the MSTR in the unit of cx8. The MSTR is
                read from the mdsp if reference pilot tracking is still valid.
                otherwise, the function returns the recent good MSTR.

DEPENDENCIES    none

RETURN VALUE    The MSTR in the unit of cx8.

SIDE EFFECTS    none

===========================================================================*/

uint32 hdrsrchdrv_get_mstr_x8( void );

/*===========================================================================

FUNCTION     : HDRSRCHDRV_GET_RX_AGC_DB

DESCRIPTION  : This function returns the RX AGC estimate in dB units

DEPENDENCIES : None

PARAMETERS   : None

RETURN VALUE : None

SIDE EFFECTS : None

===========================================================================*/
int16 hdrsrchdrv_get_rx_agc_db( uint8 antenna );

/*=========================================================================

FUNCTION     : HDRSRCHDRVG_DIVERSITY_INIT

DESCRIPTION  : This function controls the RX diversity

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchdrv_diversity_init ( boolean div_flag );

/*=========================================================================

FUNCTION     : HDRSRCHDRV_STARTUP_PROC

DESCRIPTION  : This function does the startup processing

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS : Changes firmware/hardware settings

=========================================================================*/

void hdrsrchdrv_startup_proc( void );

/*=========================================================================

FUNCTION     : HDRSRCHDRV_SHUTDOWN_PROC

DESCRIPTION  : This function does the HDR shutdown processing

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS : Changes firmware/hardware settings

=========================================================================*/

void hdrsrchdrv_shutdown_proc( void );

/*=========================================================================

FUNCTION     : HDRSRCHDRV_ABORT_SEARCH

DESCRIPTION  : This function aborts the current search

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : Changes firmware/hardware settings
               srchAbort is set
               srchNumTasks will be reset to 0
               Current search will finish, then no more searches
                 will be programmed.

=========================================================================*/

void hdrsrchdrv_abort_search
(
  boolean                         dump_partial_results
    /* Flag to generate DUMP signal for aborted search results */
);

/*=========================================================================

FUNCTION     : HDRSRCHDRV_LOSTDUMP_CB

DESCRIPTION  : This function will be called if there is a lost dump.

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS

=========================================================================*/

void hdrsrchdrv_lostdump_cb(void *);

/*=========================================================================

FUNCTION     : HDRSRCHDRV_DUMP_CB

DESCRIPTION  : This function will be called after a status dump.

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS

=========================================================================*/

void hdrsrchdrv_dump_cb(void *);

/*=========================================================================

FUNCTION     : HDRSRCHDRV_REQUEST_SRCH4_MDSP_APP

DESCRIPTION  : This function requests the SRCH4 application be loaded
               for HDR. It will download the firmware srch4 application
               to the MDSP if it isn't already downloaded.

DEPENDENCIES : It needs to be called when HDR gets the RF lock.

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchdrv_request_srch4_mdsp_app( void );

/*=========================================================================

FUNCTION     : HDRSRCHDRV_RELEASE_SRCH4_MDSP_APP

DESCRIPTION  : This function releases the SRCH4 operation for HDR
               allowing the application to be removed from MDSP memory.

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchdrv_release_srch4_mdsp_app( void );

/*=========================================================================

FUNCTION     : HDRSRCHDRV_ENABLE_SRCH4

DESCRIPTION  : This function enables SRCH4 operation for HDR. It calls 
               SRCH4 APIs to enable MDSP clock, request a HPQ for use by
               HDRSRCH and set the HDR search priority.

DEPENDENCIES : It needs to be called when HDR gets the RF lock.
               hdrsrchdrv_request_srch4_mdsp_app must have been called.

INPUT        : search priority

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchdrv_enable_srch4
(
  srch4_priority_type priority
    /* Search priority */
);

/*=========================================================================

FUNCTION     : HDRSRCHDRV_DISABLE_SRCH4

DESCRIPTION  : This function disables SRCH4 operation for HDR and may
               turn off clocks to the MDSP and the srch4 hardware. Note
               this will not remove the srch4 from the MDSP memory.
               hdrsrchdrv_release_srch4_mdsp_app must be called to 
               permit the application's removal.

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchdrv_disable_srch4( void );


/*=========================================================================

FUNCTION     : HDRSRCHDRV_START_SLAM

DESCRIPTION  : This function sends the msg to start slam.

DEPENDENCIES : None

INPUT        : set_timing - Whether software gives firmware timing
               hstr       - hstr number
               mstr_x8    - mstr in chipx8

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchdrv_start_slam
(
  boolean   set_timing,
    /* Whether software gives firmware timing */

  uint16    hstr,
    /* hstr number */

  uint32    mstr_x8,
    /* mstr in chipx8 */

  uint64    cdma_sys_time,
    /* cdms system time */

  uint32    hdr_rtc
    /* hdr rtc  number */

);


/*=========================================================================

FUNCTION     : HDRSRCHDRV_SEND_HYPERSPACE_CFG_MSG

DESCRIPTION  : This function sends the message to config hyperspace flag.

DEPENDENCIES : None

INPUT        : hyperspace - Hyperspace flag

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchdrv_send_hyperspace_cfg_msg
( 
  uint16                     hyperspace
    /* hyper space flag */
);


/*=========================================================================

FUNCTION     : HDRSRCHDRV_SET_DUMP_STATUS

DESCRIPTION  : This function sets the dump status.

DEPENDENCIES : None

INPUT        : TRUE: The dump is ready for process
               FALSE: The dump is not ready for process

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchdrv_set_dump_status
( 
  boolean                    status
    /* Whether the dump is ready */
);


/*=========================================================================

FUNCTION     : HDRSRCHDRV_DUMP_IS_READY

DESCRIPTION  : This function returns the dump status.

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : TRUE : The dump is ready
               FALSE: The dump is not ready

SIDE EFFECTS : None

=========================================================================*/

boolean hdrsrchdrv_dump_is_ready( void );


/*=========================================================================

FUNCTION     : HDRSRCHDRV_IS_SLAM_PENDING

DESCRIPTION  : This function checks whether it is waiting for slam response.

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : TRUE  - The slam response is received.
               FALSE - The slam response is not received.

SIDE EFFECTS : None

=========================================================================*/

boolean hdrsrchdrv_is_slam_pending( void );

/*=========================================================================

FUNCTION     : HDRSRCHDRV_SET_SRCH4_PARAM

DESCRIPTION  : This function sets srch4 params

DEPENDENCIES : None

INPUT        : srch4 parameters

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchdrv_set_srch4_param
(
  srch4_mode_type mode, // srch4 mode
  uint16          rtc_cnt_cx2
  /* RTC position in cx2 when  streaming is stopped for connected mode
     and idle offline mode, to be passed on to SRCH FW for offline
     search  */
);

/*=========================================================================

FUNCTION     : HDRSRCHDRV_IS_SEARCH_ACTIVE

DESCRIPTION  : This function returns the current status of search

DEPENDENCIES : None

INPUT        : none

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

boolean hdrsrchdrv_is_search_active(void);

#endif /* HDRSRCHDRV_H */
