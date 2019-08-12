#ifndef MCI_I_H
#define MCI_I_H
/*===========================================================================

          M A I N   C O N T R O L   T A S K    H E A D E R

DESCRIPTION
  This file contains global declarations and external references
  required by internal MC units and subtasks.

  Copyright (c) 1990 - 2013 Qualcomm Technologies, Inc. 
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mci.h_v   1.10   08 Oct 2002 12:21:36   hexinw  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mci_i.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/04/12   ppr     Feature Cleanup
01/06/12   ppr     1X CP - Feature clean up: Mainline Always On features,
                   Remove Always Off Features
12/13/10   ag      Replaced inclusion of prot.h by prot_v.h.
11/17/10   vks     Move dog parameter definitions to mci.h as they are
                   being used by FTM (test_mc.c)
11/15/10   jtm     Initial SU API split for CP.
09/15/10   ag      Added support for dynamic DOG.
01/10/10   jj      CSIM P2 changes, migration to MMGSDI API
01/08/08   ag      Mainlined FEATURE_LPM
01/05/04   pg      Mainlined FEATURE_MULTIMODE_ARCH.
08/20/03   yll     Changed FEATURE_HDR_HYBRID to FEATURE_HDR.
10/08/02   hxw     Changed FEATURE_GPSONE_MSBASED to FEATURE_GPSONE_OOS.
09/30/02   sb      Mainlined FEATURE_SD20.
09/06/02   va      Cleaned up lint errors.
06/07/02   sj      Added support for multimode architecture.
03/08/02   hxw     Added FEATURE_GPSONE_MSBASED to support MSBased gpsOne.
02/14/02   fc      Merged from 1.5 in MSM archive.
 02/06/02  abh     Added code to go into GPS state when Phone is in Acquisition state.
 02/06/02  sh      Added hybird operation support
 05/25/01  ts      Added UIM queue for new UIM server interface.
06/26/01   sb      Merged the following from common archive:
           05/25/01   ts      Added UIM queue for new UIM server interface.
05/01/01   dna     Added support for HDR.
02/16/01   fc      Added support for system determination 2.0. All changes
                   are featurized under FEATURE_SD20.
05/12/99   kmp     Changed T_AUTH to FEATURE_AUTH
09/04/98   pms     Neared the variables to save ROM space.
08/25/98   ck      Externalized mc_dh_q
01/29/98   sk      Added nv_get_item, nv_put_item, mcid_chk as externs.
02/15/96   dna     Included target.h to provide T_AUTH define for extern.
06/07/95   dna     Added External Authentication Task Report Queue
07/09/92   ip      Initial release.

===========================================================================*/

#include "1x_variation.h"
#include "queue.h"
#include "target.h"
#include "rex.h"

#include "prot_v.h"           /* Common type declarations for multimode. */
#include "mcc.h"
#include "mcc_v.h"
#include "mcc_i.h"
#include "mci.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/* The auth report queue is used by MCC and ACPMC.                         */
extern  q_type mc_auth_q;


#ifdef FEATURE_UIM_RUIM
/* The uim report queue is used by MCC and ACPMC.                          */
extern  q_type mc_uim_q;
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_CSIM
/* The MMGSDI report queue */
extern  q_type mc_mmgsdi_q;
#endif /* #ifdef FEATURE_CSIM */


/* The dh report queue is used by MCC.                                     */
extern  q_type            mc_dh_q;


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION mc_map_deact_to_mcc_exit_reason

DESCRIPTION
  This function maps the deact_reason received in MMoC's deactivate request
  to mcc_exit_name_type.

DEPENDENCIES
  None.

RETURN VALUE
  mcc_exit_reason

SIDE EFFECTS
  None.
===========================================================================*/
extern mcc_exit_name_type              mc_map_deact_to_mcc_exit_reason
(
       prot_deact_e_type               deact_reason
          /* MMoC's protocol deactivate reason.
          */

);


/*===========================================================================
FUNCTION mc_map_gen_cmd_to_mcc_exit_reason

DESCRIPTION
  This function maps the MMOC's generic command received to
  mcc_exit_name_type.

DEPENDENCIES
  None.

RETURN VALUE
  mcc_exit_reason

SIDE EFFECTS
  None.
===========================================================================*/
extern mcc_exit_name_type              mc_map_gen_cmd_to_mcc_exit_reason
(
       prot_gen_cmd_s_type             *gen_cmd_ptr
          /* Pointer to the MMoC's generic command.
          */

);

#endif /* MCI_I_H */
