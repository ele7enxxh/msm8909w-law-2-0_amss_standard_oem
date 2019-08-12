#ifndef GL1_MSG_WCDMA_MEAS_H
#define GL1_MSG_WCDMA_MEAS_H
/*========================================================================
       G L 1  M E S S A G E  L A Y E R -  W C D M A  M E A S U R M E N T
                            HEADER FILE
DESCRIPTION
   This file defines the interfaces and data structures that are common to
   the WCDMA interRAT measurement feature of GSM operation.

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
========================================================================== */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdrivers/inc/gl1_msg_wcdma_meas.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ---------------------------------------------------------- 
30/01/15   dg       CR786825 On entering dedicated mode, configure G2W dedicated mode meas. only if no ded srch is in progress
13/08/14   xz        CR708896 Remove unuseful aborting treatment for TDS
03/09/14   cja      CR717826 Prevent G2X and WTR hop at the same time
02/07/14   sp       CR688572: Memory opts - reduce global data.
08/05/13   dg       CR479252: Call wcdma_ded_srch_abort_cb in function l1_sc_wcdma_abort_ded_search 
18/07/13   gk       CR513938:Mon counter for WCDMA reconf should not be incremented, if a WCDMA reconf is already in progress
21/08/13   cja      CR531211 Prevent mutex deadlock between GL1 ISR Lock and g2w srch drv lock
18/06/13   cja      CR498800 Use TRM to get IRAT RF ID  
06/05/13   dv       CR482086 Ensure when leaving Idle that wcdma_id_handler runs to completion
03/05/13   sjw      CR380127 Dedicated abort should return indication of failure
29/11/12   jj       CR425555 GL1 is stuck in Idle mode due to incomplete 
                    abort of WCDMA reconfirmation.
22/11/12   ws       CR420178 - use RRC diverstiy type to avoid type redeclaration
02/08/12   pa       CR384676: Consider only JDS cells in G2T measurement. 
14/06/12   cja      CR359280 Turn off W/L/T clocks after WFW/LFW/TFW disable
24/02/12   og       CR339002 Handle failure to abort W ID search
10/02/12   ws       CR 336194 Enable TD-SCDMA IRAT for GERAN
08/10/10   og       Issue WCDMA reconfirm and dedicated search startup and cleanup
                    commands from the ISR instead of TASK. CR259226.
24/02/10   ws       Integrate changes from Mustang mdm8200 G2W driver
01/09/08   cs       Add access function to allow immediate setting of gsm_only
25/10/07   cs       Remove dependency on l1_sc.h for gsm_only indication for CR117884
25/09/06   nt       CR101231: - Added access function gl1_msg_get_id_srch_in_progress()
                    so PL1 knows if it needs to abort an active search before changing 
                    its state.
09/23/04   bm       Added funtion prototype wcdma_ded_srch_timout()
04/08/04   bm       Support for Packet Measurement Report in IDLE mode
02/11/04   gfr      Support for reconfirm in GPRS mode.
01/20/04   jc       Updated for new gtow ho measurment algorithm.
11/03/02   tb       Initial version.
===========================================================================*/


/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "geran_variation.h"
#ifdef CUST_H
#include "customer.h"
#endif


#include "gl1_defs.h"
#include "gl1_defs_g.h"

#ifdef FEATURE_GSM_TDS
#include "tdsiratif.h"
#endif

#include "rrc_rr_types.h"

#define SCR_CODE_ALL	0xffff
#define PN_POSITION_UNKNOWN	        ((uint32)0xffffffff)

#define MAX_WCDMA_CELLS_PER_UARFCN  32

#define MAX_WCDMA_CELLS_PER_DED_SRCH    8
#define MAX_WCDMA_CELLS_PER_DED_RECONF    8

typedef struct	
{	
  uint16 scr_code;	/* Scrambling code (0..511, or all codes */
  diversity_T diversity;	/* cell diversity */
  uint32 PN_pos;	/* PN Position (19 bit value, or PN_POSITION_UNKNOWN) */
} gl1_wcdma_cell_params_type;	

typedef struct	
{	
  gl1_wcdma_cell_params_type cell;	/* cell specific parameters */
  int16 RSCP;	/* Received signal strength (in dBm) */
  int16 EcNo;	/* Energy value returned from firmware */
  int16 RSSI;   /* Needed for packet measurement report */
} gl1_wcdma_cell_meas_type;	

 
typedef struct	
{	
  uint16 UARFCN;	        /* WCDMA Frequency (14 bits) */
  uint16 num_cell_measurements;	/* Number of cells with measurement data */
#ifdef FEATURE_GSM_TDS
  uint32 measuredFreq;
  int32  rssi;
  uint16 ts0_position; 
  /* position of TS0 : valid till ACQ hasn't been complete
   after that cell_position will be valid */
  uint8  last_lna_state;  
  uint8  last_acq_state;
  uint8  num_of_jds_cells;
#endif /* FEATURE_GSM_TDS*/
  gl1_wcdma_cell_meas_type meas[MAX_WCDMA_CELLS_PER_UARFCN]; /* Array of cells */
} gl1_wcdma_cell_meas_rpt;	


/* used for dedicated mode stage1/stage2 results */
typedef struct	
{	
  uint16 UARFCN;    /* WCDMA Frequency (14 bits) */
  uint16 scr_code;  /* Scrambling code (0..511, or all codes */
  uint32 PN_pos;    /* PN Position (19 bit value, or PN_POSITION_UNKNOWN) */
  int16 RSCP;	    /* Received signal strength (in dBm) */
  int16 EcNo;	    /* Energy value returned from firmware */
  int16 rssi;
  uint32 tot_eng;
} gl1_wcdma_cell_ded_rpt;	
 

/* used for dedicated mode reconfirmation reports */
typedef struct	
{	
  uint16 scr_code;  /* Scrambling code (0..511, or all codes */
  uint32 PN_pos;    /* PN Position (19 bit value, or PN_POSITION_UNKNOWN) */
  int16 RSCP;	    /* Received signal strength (in dBm) */
  int16 EcNo;	    /* Energy value returned from firmware */
  int16 rssi;
  uint32 tot_eng;
} gl1_wcdma_cell_ded_reconf_type;


/* used for dedicated mode reconfirmation reports */
typedef struct
{
  uint16 UARFCN;	            /* WCDMA Frequency (14 bits) */
  uint16 num_cell_measurements;	/* Number of cells with measurement data */
  gl1_wcdma_cell_ded_reconf_type   meas[MAX_WCDMA_CELLS_PER_DED_RECONF];
} gl1_wcdma_cell_ded_reconf_rpt;	


 
/*  callback functions */

typedef void (*gl1_msg_wcdma_meas_report_cb_type)(gl1_wcdma_cell_meas_rpt *rpt, gas_id_t gas_id);
typedef void (*gl1_msg_wcdma_ded_report_cb_type)(gl1_wcdma_cell_ded_rpt *rpt, gas_id_t gas_id);
typedef void (*gl1_msg_wcdma_meas_abort_cb_type)(gas_id_t gas_id);
typedef void (*gl1_msg_wcdma_ded_reconf_cb_type)(gl1_wcdma_cell_ded_reconf_rpt *rpt, gas_id_t gas_id);

/* Exported functions */
extern void gl1_msg_wcdma_srchdrv_init( rfm_device_enum_type  source_rf_device, gas_id_t gas_id );

extern boolean gl1_msg_get_recon_srch_progress ( gas_id_t gas_id);

extern boolean gl1_msg_wcdma_srch_active( gas_id_t gas_id );

extern void gl1_msg_cfg_wcdma_meas( 
            gl1_msg_wcdma_meas_report_cb_type id_report_cb,
            gl1_msg_wcdma_meas_abort_cb_type  id_abort_cb,
            gl1_msg_wcdma_meas_report_cb_type recon_report_cb,
            gl1_msg_wcdma_meas_abort_cb_type  recon_abort_cb,
            gas_id_t                          gas_id);
            
extern void gl1_msg_cfg_wcdma_meas_ded( 
            gl1_msg_wcdma_ded_report_cb_type ded_report_cb,
            gl1_msg_wcdma_meas_abort_cb_type ded_abort_cb,
            gl1_msg_wcdma_ded_reconf_cb_type ded_reconf_cb,
            gas_id_t                         gas_id);
            
extern void gl1_msg_set_gsm_only( boolean gl1_gsm_only_mode,gas_id_t gas_id );
            
extern void gl1_msg_cleanup_wcdma_meas( gas_id_t gas_id );
extern void gl1_msg_leave_wcdma_meas_ded(gas_id_t gas_id );
extern void gl1_msg_wcdma_identify (
                             uint16                       UARFCN,
                             uint16                       num_cell_params,
                             gl1_wcdma_cell_params_type   *cells,
                             gl1_wcdma_cell_meas_rpt      *rpt,
                             gas_id_t                     gas_id  );

extern void gl1_msg_wcdma_reconfirm (
                             uint16                       UARFCN,
                             uint16                       num_cell_params,
                             gl1_wcdma_cell_params_type   *cells,
                             gl1_wcdma_cell_meas_rpt      *rpt,
                             gl1_defs_tn_type             tn_offset,
                             gas_id_t                     gas_id  );

extern void gl1_msg_wcdma_ded_srch (
                             uint16                       UARFCN,
                             uint16                       num_cell_params,
                             gl1_wcdma_cell_params_type   *cells,
                             gl1_wcdma_cell_ded_rpt       *rpt,
                             uint16                       num_cell_reconf,
                             gl1_wcdma_cell_params_type   *reconf_cells,
                             gl1_wcdma_cell_ded_reconf_rpt *reconf_rpt,
                             gl1_defs_tn_type             tn_offset,
                             gas_id_t                     gas_id  );

extern boolean gl1_msg_abort_wcdma_identify ( gas_id_t gas_id  );
extern boolean gl1_msg_abort_wcdma_reconfirm ( gas_id_t gas_id  );
extern boolean gl1_msg_abort_wcdma_ded_srch ( gas_id_t gas_id  );
extern void wcdma_ded_srch_timout(gas_id_t gas_id);
extern boolean gl1_msg_get_id_srch_in_progress ( gas_id_t gas_id  );
extern void gl1_msg_reset_rx_flags(gas_id_t gas_id);
extern void gl1_msg_wcdmadrv_abort_search( gas_id_t gas_id );

/* This function is called in GSM L1 task context in
 * idle mode and sets a flag to trigger the start rf command
 * in the ISR.
 */
extern void  wcdma_start_rf_cmd( gas_id_t gas_id  );

extern void  wcdma_reconf_start_rf_cmd( gas_id_t gas_id );

extern boolean g2w_is_wfw_active (gas_id_t gas_id );

extern void wcdma_set_srch_done( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_check_ded_srch_in_progress

DESCRIPTION
Retruns the value of ded_srch_in_progress flag

DEPENDENCIES
 NA

RETURN VALUE
  ded_srch_in_progress flag

SIDE EFFECTS

===========================================================================*/

extern boolean gl1_check_ded_srch_in_progress(gas_id_t gas_id );

#endif /* GL1_MSG_WCDMA_MEAS_H */
