#ifndef SRCH_FS_H
#define SRCH_FS_H
/*===========================================================================
                S E A R C H      H E A D E R      F I L E

GENERAL DESCRIPTION:
  This file contains extern declaration for the freq scanning functions used in other states.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

Copyright(c) 2000 to 2011 by Qualcomm Technologies, Inc.
All Rights Reserved.
      
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/srchfs.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/12/13   bj     Removed srchfs_pcs_fine_scan and srch_fs_800_fine_scan function declaration
01/08/13   jd     Added wcdma extended 800 band support for trm.
02/03/11   nd     Added support for Band-BC11 
12/08/10   vsr    Added support for Rx chain swap
05/21/10   nd     Added support for Band-BC11 
05/03/10   sv     Fixed compiler warnings.
02/02/09   nd     extend the LFS feature for conventional OOS under FEATURE_LFS_COOS
09/26/07   nd     Added a new database to hold the freq, rssi and if the chnl 
                  is a valid wcdma channel
28/06/07   rc     Defined WCDMA band boundaries used to validate fine scan
                  frequency given by RRC.
03/05/07   sk     Added code for flexible deep sleep and 
                  FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2 features.
09/27/06   nd     updated the prototype declaration for srchfs_fine_scan()
11/08/05   nd/gv  Checkin the code onto mainline.
11/07/05   nd     Created file and added freq scan related prototypes.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* ------------------------------------------------------------------- */
/* These are the channel numbers used by each band class. Note that    */
/* they do NOT match those specified in 3GPP 25.101 v7.2.0.    */
/* ------------------------------------------------------------------- */

/* --------------------------------- */
/*  BC-1 IMT                         */                  
/* --------------------------------- */
/* Downlink channel numbers. 
   End boundary is extended to accomodate 2MHz step size for Raw Scan */
#define SRCH_FS_RX_CHAN_WCDMA_MIN                                  10562   
#define SRCH_FS_RX_CHAN_WCDMA_MAX                                  10842   


/* --------------------------------- */
/*  BC-2 PCS                         */                  
/* --------------------------------- */
/* Downlink channel numbers 
   End boundary is extended to accomodate 2MHz step size for Raw Scan */
#define SRCH_FS_RX_CHAN_WCDMA_PCS_MIN                              9662     
#define SRCH_FS_RX_CHAN_WCDMA_PCS_MAX                              9942


/* ADDITIONAL CHANNELS downlink channel numbers */
#define SRCH_FS_RX_CHAN_WCDMA_PCS_ADDITIONAL_MIN                   412     
#define SRCH_FS_RX_CHAN_WCDMA_PCS_ADDITIONAL_MAX                   687                     

/* --------------------------------- */
/*  BC-3 1800                        */                  
/* --------------------------------- */
 /* Downlink channel numbers */
#define SRCH_FS_RX_CHAN_WCDMA_1800_MIN                             1162     
#define SRCH_FS_RX_CHAN_WCDMA_1800_MAX                             1513


/* --------------------------------- */
/*  BC-4 1700(uplink)/2100(downlink) */                  
/* --------------------------------- */
 /* Downlink channel numbers */
#define SRCH_FS_RX_CHAN_WCDMA_1700_2100_MIN                        1537   
#define SRCH_FS_RX_CHAN_WCDMA_1700_2100_MAX                        1738
 
/* ADDITIONAL CHANNELS downlink channel numbers */                    

#define SRCH_FS_RX_CHAN_WCDMA_1700_2100_ADDITIONAL_MIN             1887    
#define SRCH_FS_RX_CHAN_WCDMA_1700_2100_ADDITIONAL_MAX             2087

/* --------------------------------- */
/*  BC-5 800                         */                  
/* --------------------------------- */
/* Downlink channel numbers 
   End boundary is extended to accomodate 3MHz step size for Raw Scan */
#define SRCH_FS_RX_CHAN_WCDMA_850_MIN                              4357    
#define SRCH_FS_RX_CHAN_WCDMA_850_MAX                              4462

/* ADDITIONAL CHANNELS downlink channel numbers */                    
#define SRCH_FS_RX_CHAN_WCDMA_850_ADDITIONAL_MIN                   1007    
#define SRCH_FS_RX_CHAN_WCDMA_850_ADDITIONAL_MAX                   1087

/* --------------------------------- */
/*  BC-6 800                         */                  
/* --------------------------------- */
/* Downlink channel numbers 
   End boundary is extended to accomodate 2MHz and 3MHz step size for Raw Scan */
#define SRCH_FS_RX_CHAN_WCDMA_800_MIN                              4387    
#define SRCH_FS_RX_CHAN_WCDMA_800_MAX                              4417

/* ADDITIONAL CHANNELS channel numbers */                    
#define SRCH_FS_RX_CHAN_WCDMA_800_ADDITIONAL_MIN                   1037    
#define SRCH_FS_RX_CHAN_WCDMA_800_ADDITIONAL_MAX                   1062

              
/* --------------------------------- */
/*  BC-8 900                         */                  
/* --------------------------------- */
/* Downlink channel numbers */          
#define SRCH_FS_RX_CHAN_WCDMA_900_MIN                              2937    
#define SRCH_FS_RX_CHAN_WCDMA_900_MAX                              3088

/* --------------------------------- */
/*  BC-9 1700                         */                  
/* --------------------------------- */  
/* Downlink channel numbers */
#define SRCH_FS_RX_CHAN_WCDMA_1700_MIN                             9237    
#define SRCH_FS_RX_CHAN_WCDMA_1700_MAX                             9387

/* --------------------------------- */
/*  BC-11 1500                         */                  
/* --------------------------------- */
/* Downlink channel numbers */          
#define SRCH_FS_RX_CHAN_WCDMA_1500_MIN                              3712     
#define SRCH_FS_RX_CHAN_WCDMA_1500_MAX                              3787

/* --------------------------------- */
/*  BC-19  extended 800                         */
/* --------------------------------- */
/* Downlink channel numbers */          
#define SRCH_FS_RX_CHAN_WCDMA_EXTENDED_800_MIN                              712
#define SRCH_FS_RX_CHAN_WCDMA_EXTENDED_800_MAX                              763

/* Additional channel numbers*/
#define SRCH_FS_RX_CHAN_WCDMA_EXTENDED_800_ADDITIONAL_MIN           787
#define SRCH_FS_RX_CHAN_WCDMA_EXTENDED_800_ADDITIONAL_MAX           837

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* freq scan parameters like raw scan threshold, fine scan threshold etc */
extern srch_fs_parameters_type  srch_fs_parameters;

#include "wcdma_variation.h"
#ifdef FEATURE_WCDMA_FS_FALSE_PEAK_ELIM_OPT

typedef struct 
{
  uint16  ctr_freq;
  int16  rssi;
  boolean valid_channel;
} srch_fs_fine_scan_result_struct_type;

typedef struct
{
  srch_fs_fine_scan_result_struct_type scan_result[L1_MAX_SCAN_FREQ];
  uint8 num_freq;
  uint8 curr_idx;
  /* it will be either fine scan or additional channel scan */
  l1_freq_scan_step_enum_type scan_type;
  /*keeps a record of the timestamp when the FREQ_SCAN cmd sent to mdsp.*/
  uint16 start_ts;
}srch_fs_fine_scan_db_struct_type;

extern srchfs_scan_substate_enum_type srchfs_scan_substate;
extern srch_fs_fine_scan_db_struct_type srch_fs_fine_scan_db;
#endif
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION        SRCHFS_RAW_SCAN

DESCRIPTION

  This function performs RAW scan on frequency band
  [start_ctr_freq, end_ctr_freq], and returns scan result to RRC via
  l1_freq_scan_cnf;

  NOTE: If the start_ctr_freq or the end_ctr_freq is invalid this 
  function will ERR_FATAL.

  The Frequency RAW procedure is as follows:

  AGC RSSI of all WCDMA carrier frequencies, starting from the
  (start_ctr_freq + offset) with 2 MHz increment, are read. All above-threshold
  carrier requencies are sorted in decending order and reported to RRC.

  RRC will subsequently request a FINE scan (with 200 KHz imcrement).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  srchfs_first_ctr_freq and srchfs_last_ctr_freq will be updated.

===========================================================================*/

extern void srchfs_raw_scan (uint16 start_ctr_freq, uint16 end_ctr_freq);

/*========================================================================
FUNCTION        SRCHFS_SEND_FREQ_SCAN_FAILURE

DESCRIPTION.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

==========================================================================*/
extern void srchfs_send_freq_scan_failure(l1_freq_scan_step_enum_type  step);

/*===========================================================================

FUNCTION        SRCHFS_FINE_SCAN

DESCRIPTION
  This function performs a fine scan of WCDMA carrier frequencies around
  a specified center frequency (ctr_freq), and returns the scan results to 
  RRC via l1_freq_scan_cnf;

  The fine scan procedure is as follows.

  The mobile measures RX_AGC values on all WCDMA carrier frequencies within
  the (2*SRCH_FREQ_SCAN_FINE_SCAN_RANGE/5) MHz frequency band --
  +(SRCH_FREQ_SCAN_FINE_SCAN_RANGE/5) MHz and
  -(SRCH_FREQ_SCAN_FINE_SCAN_RANGE/5) MHz around the specified center frequency.  I.e. from ctr_freq - 1 MHz to ctr_freq + 1 MHz.

  For non-PCS bands this scan is performed in steps of 200 kHz. For the PCS 
  band the steps are in 100 kHz steps.

  Finally, L1 returns all above-threshold carrier frequencies, sorted with the
  largest RX_AGC value first, to RRC .via "RRC_CPHY_FREQ_SCAN_CNF". In general,
  RRC will subsequently request Search acquisition (via "L1_CPHY_ACQ_REQ") on
  the carrier frequency with the largest RX_AGC.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

extern void srchfs_fine_scan (uint16 ctr_freq, l1_freq_scan_band_enum_type band_passed);

/*===========================================================================

FUNCTION        SRCHFS_ADDITIONAL_CHANNEL_SCAN

DESCRIPTION
  This function checks the band on which additional channel scan needs to be done.

DEPENDENCIES
  
  
RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void srchfs_additional_channel_scan (l1_freq_scan_band_enum_type band);

#if defined(FEATURE_FLEXIBLE_DEEP_SLEEP) || defined( FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2) || defined(FEATURE_LFS_COOS)
/*===========================================================================

FUNCTION        SRCHFS_LIST_SCAN

DESCRIPTION
  This function does a scan on a list of frequencies.

DEPENDENCIES
  
  
RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void srchfs_list_scan (l1_freq_scan_cmd_type *cmd);
#endif

#ifdef FEATURE_WCDMA_FS_FALSE_PEAK_ELIM_OPT
/*===========================================================================

FUNCTION        SRCHFS_PROC_FREQ_SCAN_DONE

DESCRIPTION
  This function processes the local cmd SRCH_FS_PROC_DONE

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void srchfs_proc_freq_scan_done(srch_fs_proc_done_cmd_type  *cmd_ptr);
#endif

/*===========================================================================

FUNCTION        SRCHFS_TUNE_AND_CHK_FINE_SCAN

DESCRIPTION
  This function tunes to the specified frequency and checks the RX AGC
  value for a fine scan. If the RX AGC value is above the threshold the 
  value and frequency are stored in the results to be returned to RRC.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern uint16 srchfs_tune_and_chk_fine_scan
(
  uint16                  ctr_freq,
  l1_freq_scan_cnf_type  *freq_scan_cnf,
  uint16                  num_rpt_freq
);

/*===========================================================================
FUNCTION  srchfs_tune_to_chan

DESCRIPTION
  Stop AGC, tune RF to freq passed as parameter, and then restart AGC.
  This method of tuning the RF is faster.
  
DEPENDENCIES
  None
  
PARAMETERS
  uint32 freq_param - frequency to tune RF to.

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void srchfs_tune_to_chan(uint16 freq_param);

#endif
