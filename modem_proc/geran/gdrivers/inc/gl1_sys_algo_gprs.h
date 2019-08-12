#ifndef GL1_SYS_ALGO_GPRS_H
#define GL1_SYS_ALGO_GPRS_H
/*===========================================================================

     DECLARATION OF GPRS ALGORITHM (AGC...) FUNCTIONS

GENERAL DESCRIPTION
Declares the functions and types associated with the generic gl1 gprs
algorithms.  Currently includes only AGC.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
The following is the desired order in which functions should be called

sys_algo_gprs_agc_cfg
sys_algo_agc_process_pdtch


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/KOVU/drivers/vcs/gl1_sys_algo_gprs.h_v   1.1   15 Oct 2002 13:28:18   grahamr  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdrivers/inc/gl1_sys_algo_gprs.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
04/08/05   gfr      Combined GSM and GPRS channel types
07/22/04   gfr      EGPRS support.
08/13/03   gfr      Pass correct metrics parameter to AGC
10/14/02   gr       Changed process_pdtch interface to be called once per
                    message instead of once per block.
09/06/02   gr       Initial Revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* definitions */

#include "geran_variation.h"
#ifdef CUST_H
#include "customer.h"
#endif


#include "comdef.h"
#include "gl1_hw.h"
#include "gl1_hw_g.h"
#include "gl1_sys_algo.h"
#include "msg.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/



/*===========================================================================

FUNCTION sys_algo_gprs_agc_cfg

DESCRIPTION
   Configure AGC parameters.  Called typically at the start of a TBF when
   downlink resources have been assigned.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void sys_algo_gprs_agc_cfg
(
  dBx16_T Pb,          /* power difference in dBx16 */
  dBx16_T P0,          /* power difference in dBx16 */
  boolean pbcch_exists,
  boolean pdch_dl_power_control,
  gas_id_t gas_id
);



/*=============================================================================
FUNCTION sys_algo_agc_gprs_expected_pwr

DESCRIPTION
  Un-normalize the given RSSI to the given channel power level.
  Internal NPL1 function.

DEPENDENCIES
  The message layer must be initialized.

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================= */
dBx16_T sys_algo_agc_gprs_expected_pwr
(
  dBx16_T pwr_dBm_x16,   /* BCCH normalized power in dBmx16 */
  gl1_hw_channel_type  chan_type,
  gas_id_t gas_id
);




/*=============================================================================
FUNCTION sys_algo_agc_process_pdtch

DESCRIPTION
   Applies the appropriate measured rssis from a previous PDTCH block to the
   AGC algorithm.  Must be called by PL1 for every PDTCH message.
   If downlink power control was off when the PDTCH was received, the
   Pr values must be set to 0.

DEPENDENCIES
   The AGC must be initialized.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void sys_algo_agc_process_pdtch
(
  sys_algo_agc_T *gprs_agc,  /* gprs agc pointer */
  gl1_defs_tn_type tn,       /* list of valid timeslots */
  dBx16_T Pr,                /* Pr values in dBx16 for each timeslot */
  boolean valid,              /* number of timeslots / Pr values */
  gas_id_t gas_id
);




/*=============================================================================
FUNCTION sys_algo_gprs_agc_update

DESCRIPTION
   Update AGC with new metrics info.  Called internally by NPL1 before passing
   metrics up to PL1.

DEPENDENCIES
   The message layer must be initialized.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void sys_algo_gprs_agc_update
(
  sys_algo_agc_T *gprs_agc,
#if defined (FEATURE_GSM_RX_DIVERSITY_PDT) || defined (FEATURE_GSM_RX_DIVERSITY_IDLE)
  sys_algo_agc_T *gprs_agc_divrx, 			
#endif /*FEATURE_GSM_RX_DIVERSITY_PDT || FEATURE_GSM_RX_DIVERSITY_IDLE */
  gl1_defs_rx_pkt_metrics_type **metrics_info,
  uint8 num_metrics,
  gl1_hw_channel_type  chan_type,
  uint32 time_stamp,
  boolean msg_complete,
  gas_id_t gas_id
);



/*=============================================================================
FUNCTION sys_algo_gprs_agc_reset

DESCRIPTION
   Reset the GPRS AGC fifo.  Called when aborting DL PDTCH processing to
   keep everything in sync.

DEPENDENCIES
   The AGC must be initialized.

RETURN VALUE
   None

SIDE EFFECTS
   None
============================================================================= */
void sys_algo_gprs_agc_reset (gas_id_t gas_id);



#endif /* GL1_SYS_ALGO_GPRS_H */


