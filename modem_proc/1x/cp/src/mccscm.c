/*******************************************************************
*
*    Supplemental Channel Assignment Message Processing Routine
*
* GENERAL DESCRIPTION
*  This module contains functions to process SCAM messages
* received from a base station.  It also includes functions to
* process the data pattern testing information.  The SCAM processing is
* currently only for version 0.1 of MDR, and the DPT information is
* usable for versions 0.1 or 0.2 of MDR, and possibly also 1.0.
*
*  Copyright (c) 1998 - 2013 Qualcomm Technologies, Inc. 
*  All Rights Reserved Qualcomm Proprietary
*
*  Export of this technology or software is regulated by the U.S. Government.
*  Diversion contrary to U.S. law prohibited.
*
*  All ideas, data and information contained in or disclosed by
*  this document are confidential and proprietary information of
*  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
*  By accepting this material the recipient agrees that this material
*  and the information contained therein are held in confidence and in
*  trust and will not be used, copied, reproduced in whole or in part,
*  nor its contents revealed in any manner to others without the express
*  written permission of Qualcomm Technologies, Inc.
*
*      EDIT HISTORY FOR MODULE
*  This section contains comments describing changes made to the module.
* Notice that changes are listed in reverse chronlogical order.
*
* $PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccscm.c_v   1.5   01 Oct 2002 16:53:58   azafer  $
* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccscm.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $
*
* when       who     what, where, why
* --------   ---     ----------------------------------------------------------
* 04/04/12   trc     Convert to m1x_time module
* 02/04/12   ppr     Feature Cleanup
* 09/02/11   sk      Rebased the code to the NikeL modem package by mainlining
                     FEATURE_MODEM_1X_NIKEL.
* 04/22/11   jtm     CMI Phase 4 - Final SU API changes.
* 12/02/10   jtm     Mainlined non-MDR version of CAI_MAX_USABLE_MUX_RS*
* 11/15/10   jtm     Initial SU API split for CP.
* 10/12/09   ssh     Initialized current time variable to zero as part of KW
                     error fix.
* 08/26/09   adw     Replaced deprecated ts.h api's with new time api's.
* 07/14/09   jj      CMI changes corresponding to making CAI_AC_ALT_SO_MAX
*                    have a single value.
* 05/14/09   jj      CMI phase II update.
* 04/29/09   jj      Replaced assert.h with amssassert.h
* 04/17/09   jtm     Added includes to fix compiler warnings.
* 04/03/09   jj      Removed Target specific feaaturization as part of CMI effort.
* 12/02/08   adw     Commented out _v.h includes that no longer exist inside 1x.
* 04/24/06   an      Fixed lint errors
* 10/10/05   fh      Removed including of dec.h. changed DEC_ constants to be
                     CAI_ constants
* 07/11/05   fh      Cleaned up header file including from MUX:
*                      Changed rxc.h to rxcmc.h
* 08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
* 10/01/02   az      Mainlined FEATURE_IS2000.
* 06/14/01   va      Changed send_rej_order to match the new prototype.
* 05/17/01   lh      Merged the following from common archive:
*   04/10/01   lcc     Changes to conform to new prototype for snm_get_current_config.
*   01/08/01   jq      Made some naming changes to prepare for IS2000_P2.
*   12/19/00   jq      Fixed a problem with sup pilot checking for SCAM.
* 02/27/01   kk      Added new SO group and bitmap initialization.
* 09/20/00   jq      Cleaned all lint errors.
* 07/10/00   cah     Rid msg_file compiler warning for non 95B & Special MDR
*                    builds.
* 06/19/00   yll     Added MSM3300 support.
* 03/28/00   ry      Fixed a compiler error introduced from the previous merge
* 03/28/00   ry      Featurized initialization values of neg_par_so8025 based
                     on the MSM supported
* 03/13/00   ry      Merged the following from PP (MSM3100_CP.03.00.10):
*            mk      Added MSM3100 support.
* 02/16/00   lcc     Minor comment changes.
* 10/20/99   lcc     1. Corrected mux options for DPT service options.
                     2. Added MSM5000/IS2000 MDR support.
* 08/17/99   ram     More code review changes: corrected paranthesis usage,
                     used ASSERT to check for possible corrupt values,
                     Comments to indicate ends of functions.
* 08/02/99   ram     Removed the checking of return values in the
                     mccccl_code_channel_list.update_sups function since
                     the prototype was changed to a void.
                     Made the following changes from code review:
                     1)Changed ERR to MSG_HIGH for base station errors.
                     2)Used ASSERT for situations that should not
                     happen normaly except during testing.
                     3)Cleaned up error handling in scam processing
                     functions so that whenever possible errors are
                     handled within the lower functions and are
                     not cascaded to the calling functions.
                     4)Added some comments as needed.
                     In mccscm_scam_process rejected SCAM if the Pilot
                     PN's are not in the active set and it's not linked
                     to a GHDM. This is specified by IS95B.
* 07/26/99   jq      Fixed a few minor errors due to codereview. Changed
*                    mccscm_scam_timer_chk to mccscm_scam_timer_set
* 07/22/99   jq      Fixed a misplaced #ifdef for SCAM-GHDM linking
* 07/22/99   jq      Added SCAM-GHDM linking feature.
* 05/19/99   doj     Changed references to GHDM structure (gen_ho) to generic
                     handoff structure (ho).
* 04/21/99   ram     Added checking for reverse supplemental parameters
                     in mccscm_mdr_pre_process. This was commented out
                     earlier for internal testing.
                     In SCAM, checking for rev_parms_inc is now done only
                     if rev_included is TRUE.
* 04/06/99   ram     Removed the RXC call to change the RXDSP clock
                     to 2/3 TCXO when an MDR call is up.
                     this is no longer needed because RXDSP clock is always
                     at 2/3 TCXO.
* 03/30/99   ram     Added function to check if a previous start timer is
                     active when a new SCAM is Rx'ed. If so, we ignore the
                     second SCAM. Renamed mccscm_scam_process to mccscm_
                     scam_sup_process since it only processes the sup
                     channel info. Introduced a wrapper function called
                     mccscm_scam_process which does all the processing for
                     SCAM message. Updated based on code review comments.
* 03/15/99   ram     Added function to cancel previous duration timer when
                     in gets superceeded by a new SCAM/GHDM. Code review
                     changes, fix to mdr_pre_process function to check
                     number of supplementals only if code channels are specified
                     in the message. Parameter for mccscm_scam_process is
                     now caii_scam_type. scam_timer_msg is also caii_scam_type.
* 03/09/99   ram     Added function to process IS95B SCAM/GHDM duration timer.
                     Added function to process an request to stop transmitting
                     on supplementals. Included lint fixes given by nmn.
* 02/19/99   ram     Added function to process IS95B scam start timer.
                     Increased error checking for SCAM message. Lint
                     and code-review cleanup.
* 02/11/99   ram     Created mdr_pre_process to pre-process the MDR related
                     fields of SCAM and GHDM. Created scam_process function
                     to process SCAM message. Featurized code under
                     IS95B_MDR and SPECIAL_MDR.
* 01/19/99   lcc     Moved FEATURE_IS95B_MDR below #includes for header files.
                     Now it builds correctly with and without FEATURE_IS95B_MDR.
* 01/19/99   ram     Moved FEATURE_IS95B_MDR defines so that the file
*                    builds correctly when FEATURE_IS95B is turned off.
* 01/05/98   nmn     reincluded entire file when FEATURE_IS95B_MDR is defined.
* 01/05/98   nmn     unincludes entire file with FEATURE_IS95B_MDR undef'd
* 12/23/98   lcc     Added support for RS2 DPT frames.  Also corrected several
*                    DPT related problems.
* 12/22/98   nmn     improved output and fixed interpretation of CCHID
* 12/16/98   nmn     surrounded all DPT code with #defines, fixed & tested
*                    MDR_DPT_PRINT
* 12/11/98   nmn     Linted for 0.2, improved MDR_DPT_PRINT created, not
*                    implemented
* 12/08/98   lcc     Featurized DPT with FEATURE_MDR_DPT_MARKOV.
* 12/02/98   lcc     Added support for official DPT service options (mostly from
*                    Ram).
* 12/01/98   ram     Added code for MDR DPT service options initialization
* 11/23/98   nmn     DPT works sucessfully with two channels
* 11/19/98   nmn     added functionality for DPT using byte-length data
* 10/15/98   nmn     added separate functions to perform different parts
*                    of SCAM processing
* 08/27/98   nmn     finished 0.1 for integration (major SRCH changes, removed
*                    using_channels)
* 07/19/98   nmn     finished UT (including some changes, added using_channels)
* 07/17/98   nmn     began UT
* 07/16/98   nmn     finished turning comments into ERR and MSG output, ready
*                    for UT
* 07/13/98   nmn     linted file
* 07/10/98   nmn     changed global variables into those defined in cai.h
* 07/09/98   nmn     split file into separate .h file, turned magic numbers
*                    into variables, changed spacing, created more comments,
*                    added error and message calls
* 07/06/98   nmn     Initial document
*
*
*
*******************************************************************/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "modem_1x_defs.h"
#include "customer.h"
#include "comdef.h"
#include "mccscm.h"
#include "cai_v.h"
#include "caii_v.h"

#undef MSG_FILE

#include "err.h"
#include "amssassert.h"
#include "mccccl.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "mccrxtx.h"
#include "mccsrch_i.h"
#include "msg.h"
#include "m1x_diag.h"
#include "qw.h"
#include "rex.h"
#include "rxcmc_i.h"
#include "snm_v.h"
#include "snm_i.h"

