#ifndef SRCH_MDSP_H
#define SRCH_MDSP_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        S E A R C H   T A S K  --  M D S P   S U B S Y S T E M

GENERAL DESCRIPTION
  This module contains functions pertaining to the MDSP used by the
  demodulator and searcher.

EXTERNALIZED FUNCTIONS/MACROS
  srch_mdsp_demod_running

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2002 - 2013 Qualcomm Technologies, Inc. 
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

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/srch_mdsp.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/25/11   jtm     CMI Phase 4 SU API changes.
12/13/10   vks     No need to include mdsp.h and sample_server.h.
11/15/10   jtm     Initial SU API split for CP.
11/05/10   vks     Add support for FTM mdsp enable/disable (decouple from
                   srch_mdsp_enable/disable that FTM is using till now).
11/09/10   jtm     Initial SU API split.
08/09/10   cjb     Added MDSP API to printout PC in case of a timeout.
07/14/10   sst     Remove srch_mdsp_[sleep|wakeup] API's
04/19/10   adw     Added interface to enable / disable mDSP control of the MND.
03/24/10   vks     Add api to export mdsp shared memory address.
03/05/10   vks     Remove the support for CDMA App config message handler (we
                   are using CDMA state config message now).
01/09/10   vks     Add support for 1X Demod State config message.
12/22/09   mca     Added modem_1x_defs.h
12/14/09   vks     Add support for FWS app config message.
12/08/09   vks     Export srch_mdsp_download for Q6/MSGR interface (for FTM).
11/11/09   vks     Clean up on the previous changes
11/09/09   vks     Added Message Router and Q6 FW support under feature
                   FEATURE_1X_SUPPORTS_MSGR.
04/01/09   adw     Categorized included header files and standardized format.
03/31/09   adw     Eliminated implicit includes.
03/26/09   adw     Remove modem_1x_defs.h include.
03/25/09   adw     Include modem_1x_defs.h for 1x specific modem defines.
03/16/09   jtm     Lint fixes.
03/05/07   aps     Added srch_mdsp_add_isr_cb().
03/05/07   nnv     Added function srch_mdsp_mnd_ctrl
01/23/07   rkc     Added srch_mdsp_enable().
10/31/06   tjc     Changed srch_mdsp_sleep/wakeup macros to functions and
                   added srch_mdsp_awake() check function.
07/13/05   grl     Added flushing the sample ram of the requested chain.
06/15/05   bt      Removed MDSP_CMD_FLUSH_SR.
04/20/05   sst     Removed srch_mdsp_isr
03/10/05   ddh     Added Featurized support for executing on 7500 RUMI station
02/07/05   sst     Created srch_mdsp_disable() to disable app and update vars
                   Extern'ed srch_mdsp_pre1x()
02/04/05   bt      Add scheduled sample ram flush.
08/09/04   sst     Extern'ed new generic mdsp isr func srch_mdsp_isr()
06/21/04   sst     Corrected naming of srch_mdsp_check_cmd_ functions
                   Updated srch_mdsp_check_cmd_ functions and their macros
05/04/04   sst     Created functions out of MDSP_CHECK_CMD_E/NE
02/19/04   sst     Lint corrections in MDSP_CHECK_CMD_E/NE
01/28/04   ejv     Mainline T_MSM6500.
12/15/03   sst     Added 2 macro's MDSP_CHECK_CMD_*() to check the MDSP cmd
                   semaphore, in which a timeout and checking of MDSP_dspHalt
                   reg is used to to report MDSP (fatal) errors
09/18/03   bt      Wait for the command semaphore after waking up the dsp srch
                   part. Also delete unused macros.
06/25/03   ejv     Update srch_mdsp_sleep() and srch_mdsp_wakeup().
06/05/03   ejv     Replace old function srch_mdsp_current_image() with
                   new function srch_mdsp_demod_running().
01/23/03   bt      Added srch_mdsp_sleep/wakeup macros.Cleaned up func names.
10/29/02   bt      add srch_mdsp_flush_samp_ram command
09/11/02   va      Added function to get the current mdsp image.
07/23/02   bt      implementation , first cut
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "comdef.h"


/*===========================================================================

                    FUNCTION DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH_MDSP_DEMOD_RUNNING

DESCRIPTION    Checks if the demod is running on the DSP.

DEPENDENCIES   None.

DEPENDENCIES   None.

RETURN VALUE   TRUE if demod is running; FALSE otherwise.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_mdsp_demod_running(void);

#endif /* SRCH_MDSP_H */
