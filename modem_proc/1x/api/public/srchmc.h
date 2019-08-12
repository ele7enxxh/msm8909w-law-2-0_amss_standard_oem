#ifndef SRCHMC_H
#define SRCHMC_H
/*===========================================================================

             S E A R C H   M C   A P I   H E A D E R    F I L E

DESCRIPTION
  This file contains external declarations to be used by the MC task.

  Copyright (c) 2004 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/srchmc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/13/09   jtm     Removed FEATURE_1X_DISABLE_CMI.
04/01/09   adw     Added temporary CMI-lite featurization.
03/25/09   adw     Removed unnecessary target and customer includes.
02/24/09   adw     Initial interface cleanup for CMI Phase-II.
03/04/09   adw     Removed the temporary _v.h include.
12/01/08   adw     Temporarily include the internal _v.h file.
11/21/08   adw     Merge from CMI sandbox, based on VU_MODEM_1X_SRCH.12.00.13
10/18/06   bn      Added new structure to support timing transfering from
                   DO to 1x
10/12/06   awj     Added featurized pseudo-idle support for UMTS/1x
06/05/06   rkc     Added srch_agc_to_dbm.
02/02/06   awj     Added SRCH_MAX_SETS
09/21/05   pa      Changed value of NO_SYS_MEAS_RESULTS
09/20/05   pa      Added AGC gated ecio measurement enum.
08/29/05   grl     Added header to SRCH_ACCESS_F message.
08/08/05   kwo     Added pgslot field to idle_cmd_type
08/03/05   ejv     Mainline FEATURE_TRANS_MANAGER.
08/02/05   ejv     Removed unnecessary includes.
07/24/05   fh      Renamed SRCH_FIND_STRONGEST_PILOT_F, SRCH_STRONG_PILOT_F
                   and related report names, data types, and data structures
07/13/05   ejv     Remove srch_access_inform.  Added srch_notify_block_gps.
06/24/05   ejv     Remove srch_tune_rfm.
06/01/05   grl     Temporarily added srch_tune_rfm for calls from mccidl.c
05/31/05   rng     Merged in from SHDR Sandbox.
04/12/05   fc/ejv  Added support for DCA.
03/21/05   ejv     Removed runtime Cset sizes.
02/07/05   ejv     Added srch_get_rx_agc from srch.h.
01/24/05   ejv     Added srch_get_pilot_ecio from srch.h.
12/10/04   ejv     Updates to the RelD interface.
12/06/04   kwo     Added SRCH_ACCESS_F and SRCH_FIND_STRONGEST_PILOT_F
11/12/04   ejv     Replaced magic number.  Remove use_sch flag.
11/10/04   ejv     Added srch_tune_rfm and srch_get_rx_div_cap from srch.h.
11/03/04   kwo     Removed obsolete fade and unslew command support
10/15/04   ejv     Initial version.  Moved MC-specific data from srch.h.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "cai.h"


/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
              Externally Exported CAI and Other Constants
--------------------------------------------------------------------------*/

/* Per IS-95 Appendex 2, Table D-2 */
/* Maximum size of the Active Set */
#define  SRCH_ASET_MAX            CAI_N6M

/* Per IS-95 Appendex 2, Table D-2 */
/* Max number of real Candidates by spec */
#define  SRCH_MAX_CAN_CNT         CAI_N7M

/* Maximum size of the (pC+C) Set */
#define  SRCH_CSET_MAX            (SRCH_MAX_CAN_CNT + 5)

/* Per IS-95 Appendex 2, Table D-2 */
/* Maximum size of the Neighbor Set */
#define  SRCH_NSET_MAX            CAI_N8M

/* Max number of supplemental channels by spec */
#define  SRCH_SUP_CHANNEL_MAX     CAI_MAX_NUM_SUP



/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH_RUDE_WAKEUP

DESCRIPTION    This function is invoked to signal the searcher task to
               wake up.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Search SLEEP signal may be cleared.
               srch_rudely_awakened flag is set.

===========================================================================*/
extern void srch_rude_wakeup
(
  boolean orig
);


/*===========================================================================

FUNCTION       SRCH_GET_RX_AGC

DESCRIPTION    This function returns the current value of the rx agc register.
               For targets using MSM2 or higher, this is simply a passthru for
               RF CAGC function calls.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern byte srch_get_rx_agc ( void );


#endif /* SRCHMC_H */
