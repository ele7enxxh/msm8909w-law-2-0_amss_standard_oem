#ifndef WL1_X2W_SRHC_LOG_H
#define WL1_X2W_SRHC_LOG_H

/*===========================================================================
   FILE: wl1x2wsrchlog.h

GENERAL DESCRIPTION
   This file contains all of the definitions necessary for G2W srch log 
   packets.
  
EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

  Copyright (c) 2002 - 2013 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/24/14   cc     Moving G2W driver to WL1 folder
03/15/12   mm     CR 344531 G2W drivers new design
26/03/08   ws     Added PACKED_POST for QDSP6 targets
03/04/07   ws     Corrected Featurisation for GSM only builds
09/16/05   am     Created file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "log.h"
#include "log_codes.h"
#include "wl1x2wsrchmdspsync.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#if ( defined(FEATURE_INTERRAT_HANDOVER_GTOW) || defined(FEATURE_INTERRAT_CELL_RESELECTION_GTOW))
#ifdef FEATURE_G2W_SRCH_LOGGING_ENABLED
/*--------------------------------------------------------------------------
                 LOG PACKET: G2W_SRCH_STEP1_LOG_PACKET
                     
This structure is used to log search step 1 information. These packets occur
upon every id search in idle mode.          
--------------------------------------------------------------------------*/

#ifndef WL1_X2W_SRCH_STEP1_LOG_PACKET
#define WL1_X2W_SRCH_STEP1_LOG_PACKET (LOG_GSM_BASE_C  + 0x25D)
#else
#error redefinition of LOG command code: WL1_X2W_SRCH_STEP1_LOG_PACKET
#endif

/* information for each peak returned */
typedef PACKED struct PACKED_POST {
    /* Primary Scrambling code PSC position in chipx8 resolution. 
       Range :  0....(8x2560 - 1)          */
    uint16 pos ;

    /* energy level for the peak */
    uint16 eng ;

} wl1_x2w_srch_log_psc_peak_struct_type;
 
  
LOG_RECORD_DEFINE(WL1_X2W_SRCH_STEP1_LOG_PACKET)

  /* Non-coherent integration length*/
  uint8 nn;
 /*RSSI value in dBm units*/
  int16 rssi;
  /* Num of peaks */
  uint8 num_peaks;
  
  /* the actual peak log records */
  wl1_x2w_srch_log_psc_peak_struct_type  wl1_x2w_srch_log_psc_peak_info[WL1_X2W_SRCH_MAX_PSC_PEAKS];

LOG_RECORD_END


/*--------------------------------------------------------------------------
                 LOG PACKET: G2W_SRCH_STEP2_LOG_PACKET
                     
This structure is used to log search step 2 information. These packets occur
upon every id search in idle mode.         
--------------------------------------------------------------------------*/

/* the log code for the seach step 2 packet */
#ifndef WL1_X2W_SRCH_STEP2_LOG_PACKET
#define WL1_X2W_SRCH_STEP2_LOG_PACKET (LOG_GSM_BASE_C  + 0x25E)
#else
#error redefinition of LOG command code: WL1_X2W_SRCH_STEP2_LOG_PACKET
#endif

/* information for each code group tested  */
typedef PACKED struct PACKED_POST {
    /* code group id. range: 0...63 */
    uint8 code_grp ;

    /* pn position  0.....(38400x8 -1) */
    uint32 pos ;
    
    /* srch energy */
    uint16 eng ;

} wl1_x2w_srch_log_ssc_peak_struct_type ;


LOG_RECORD_DEFINE(WL1_X2W_SRCH_STEP2_LOG_PACKET)
   
  /* Non-coherent integration length*/
  uint8 nn;

  /*RSSI value in dBm units*/
  int16 rssi;
  
  /* number of PSC positions to be tested */
  uint8 num_psc_pos;

  /* position in chipx2 */
  uint16 psc_pos[WL1_X2W_SRCH_MAX_NASTT_PEAK_COUNT] ;

  /* number of peaks reported */
  uint8 num_peaks;
 
  wl1_x2w_srch_log_ssc_peak_struct_type  wl1_x2w_srch_log_ssc_peak_info[WL1_X2W_SRCH_MAX_NASTT_PEAK_COUNT] ;

LOG_RECORD_END



/*--------------------------------------------------------------------------
                 LOG PACKET: G2W_SRCH_STEP3_LOG_PACKET
                     
This structure is used to log search step 3 information. These packets occur
upon every id search in idle mode.          
--------------------------------------------------------------------------*/

/* the log code for the seach step 3 packet */
#ifndef WL1_X2W_SRCH_STEP3_LOG_PACKET
#define WL1_X2W_SRCH_STEP3_LOG_PACKET (LOG_GSM_BASE_C  + 0x25F)
#else
#error redefinition of LOG command code: WL1_X2W_SRCH_STEP3_LOG_PACKET
#endif


typedef PACKED struct PACKED_POST {
 /*scrambling code index  0...8191*/
    uint16 scr_code[WL1_X2W_SRCH_STEP3_PEAKS_PER_TASK];

    uint32  pos[WL1_X2W_SRCH_STEP3_PEAKS_PER_TASK];

    uint32  eng[WL1_X2W_SRCH_STEP3_PEAKS_PER_TASK];

} wl1_x2w_srch_log_step3_pilot_struct_type ;

/* information for each task specified */
typedef PACKED struct PACKED_POST {
    /* alignment mode. 000 - no alignment.  
                       001 - align coherent integration time (CIT) to the
                             time when pn count is a multiple of 256 in 
                             chipx1 resolution 
                       010 - align CIT to the time when pn count is a 
                             multiple of 512 in chipx1 resolution
                       011 - align CIT to the time when pn count is a 
                             multiple of 1024 in chipx1 resolution
                       100 - align CIT to the time when pn count is a 
                             multiple of 2560 in chipx1 resolution
                       111 - reserved */
    uint8 align_mode ;

    /* window start pn position  specified in offsets from reference
       counter in chipx8 resolution */
    uint32 win_pn_pos ;
    
    /* code group */
    uint8 code_grp ;

} wl1_x2w_srch_log_step3_input_task_type ;



LOG_RECORD_DEFINE(WL1_X2W_SRCH_STEP3_LOG_PACKET)
   
  /* window size */
  uint16 win_siz ;

  /* coherent integration duration */
  uint16 nc ;

  /* non-coherent integration */
  uint8 nn ;
  
 /*RSSI value in dBm units*/
  int16 rssi;
  
  /* number of tasks specified; max 41 for lpq  */
  uint8 num_tasks_spec ;

  wl1_x2w_srch_log_step3_input_task_type wl1_x2w_srch_log_input_task_info[WL1_X2W_MDSP_LIST_SRCH_MAX_TASKS];

  /* num of tasks finished */
  uint8  num_tasks_finished;
 
  wl1_x2w_srch_log_step3_pilot_struct_type wl1_x2w_srch_log_task_info[WL1_X2W_MDSP_LIST_SRCH_MAX_TASKS] ;

LOG_RECORD_END


/*--------------------------------------------------------------------------
                 LOG PACKET: G2W_SRCH_LIST_LOG_PACKET
                     
This structure is used to log list search information.
--------------------------------------------------------------------------*/

/* the log code for the list seach packet */
#ifndef WL1_X2W_SRCH_LIST_LOG_PACKET
#define WL1_X2W_SRCH_LIST_LOG_PACKET (LOG_GSM_BASE_C  + 0x260)
#else
#error redefinition of LOG command code: WL1_X2W_SRCH_LIST_LOG_PACKET
#endif

 
/* information for each task specified */
typedef PACKED struct PACKED_POST {
    /* coherent integration duration */
    uint16 nc ;
    /* non-coherent integration */
    uint8 nn ;
    /* window start pn position  specified in offsets from reference
       counter in chipx8 resolution */
    uint32 pn_pos ;
    
    /* scrambling code index. 13 bits for scr_code and 3 bits for set */
    uint16 scr_code;

    /* alignment mode. 000 - no alignment.  
                       001 - align coherent integration time (CIT) to the
                             time when pn count is a multiple of 256 in 
                             chipx1 resolution 
                       010 - align CIT to the time when pn count is a 
                             multiple of 512 in chipx1 resolution
                       011 - align CIT to the time when pn count is a 
                             multiple of 1024 in chipx1 resolution
                       100 - align CIT to the time when pn count is a 
                             multiple of 2560 in chipx1 resolution
                       111 - reserved */
    uint8 align_mode ;

    /* peak detector  mode 
         0x00 - Normal mode - the peak detection is enabled and only the
              energy larger than the threshold and also greater than 
              energies half-chip away from it in either direction will
              be considered a peak
    
         0x01 - bypass mode -the peak detection is disabled and all energies
              above the threshold will be reported as peaks
        
         0x02 - Shoulder mode- used for searching associated with PICH in
              sleep mode. The searcher h/w returns 5 sets of peaks per 
              search window(each peak consists of 3 adjacent chipx2 reso-
              lution peaks) DSP interpolates these chipx2 resolution peaks
              to chipx8 resolution and reports the 4 best peaks to the 
              host processor
                       */
    uint8 pd_mode;
    
    /* window size 1...38400 in unit of chips*/
    uint16 win_siz ;

} wl1_x2w_srch_log_list_input_task_type ;


typedef PACKED struct PACKED_POST {
    /* pn position */
    uint32 pos[WL1_X2W_SRCH_LIST_PEAKS_PER_TASK];

    /* searcher energy */
    uint32 eng[WL1_X2W_SRCH_LIST_PEAKS_PER_TASK];
    
} wl1_x2w_srch_log_list_search_task_type ;

LOG_RECORD_DEFINE(WL1_X2W_SRCH_LIST_LOG_PACKET)
   
  /* Rssi at this point in dbm units */
  int16 rssi;

 /*(4) idle reconfirmation/
    (5) stage1/
    (6) stage2/
    (7) dedicated reconfirmation */
 uint8 srch_type;
 
  /* number of tasks specified; max 41 for lpq */
  uint8 num_task_spec ;
 
  wl1_x2w_srch_log_list_input_task_type wl1_x2w_srch_log_input_task_info[WL1_X2W_MDSP_LIST_SRCH_MAX_TASKS] ;

 /*  Output info   */  
  wl1_x2w_srch_log_list_search_task_type  wl1_x2w_srch_log_task_info[WL1_X2W_MDSP_LIST_SRCH_MAX_TASKS];

LOG_RECORD_END

#define WL1_X2W_SRCH_STEP1_PACKET_LEN(step1_peaks)  ( \
    FPOS(WL1_X2W_SRCH_STEP1_LOG_PACKET_type, wl1_x2w_srch_log_psc_peak_info)+  \
	 ( step1_peaks * sizeof(wl1_x2w_srch_log_psc_peak_struct_type))  )
     
/* step2 */
#define WL1_X2W_SRCH_STEP2_PACKET_LEN(num_psc, out_peaks) ( \
	FPOS(WL1_X2W_SRCH_STEP2_LOG_PACKET_type, psc_pos) + \
	( num_psc * sizeof(uint16) ) + sizeof(uint8) + \
	( out_peaks * sizeof(wl1_x2w_srch_log_ssc_peak_struct_type)) )

/* step3 */
#define WL1_X2W_SRCH_STEP3_PACKET_LEN(input, output)   ( \
	FPOS(WL1_X2W_SRCH_STEP3_LOG_PACKET_type, wl1_x2w_srch_log_input_task_info ) + \
	(input*sizeof(wl1_x2w_srch_log_step3_input_task_type)) + \
	sizeof(uint8) +  \
	(output*sizeof(wl1_x2w_srch_log_step3_pilot_struct_type)) )
    
/* List search */
#define WL1_X2W_SRCH_LIST_SRCH_PACKET_LEN(input_tasks)  ( \
	FPOS(WL1_X2W_SRCH_LIST_LOG_PACKET_type, wl1_x2w_srch_log_input_task_info ) + \
	(input_tasks*sizeof(wl1_x2w_srch_log_list_input_task_type)) + \
	(input_tasks*sizeof(wl1_x2w_srch_log_list_search_task_type))  )

#endif   /* end ifdef FEATURE_G2W_SRCH_LOGGING_ENABLED */
#endif /* FEATURE_INTERRAT_HANDOVER_GTOW || FEATURE_INTERRAT_CELL_RESELECTION_GTOW */

#endif /* WL1_X2W_SRHC_LOG_H */
