#ifndef CMINBAND_H
#define CMINBAND_H
/*===========================================================================

         C A L L   M A N A G E R   I N B A N D   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMINBAND.C

Copyright (c) 1991 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cminband.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
11/08/05   sk      Separated CM reports and commands.
02/06/04   ws      Initial jaguar/mainline merge.
01/21/03   ws      Updated copyright information for 2003
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
10/23/01   VT/IC   Baseline for CCS
8/31/99    SH      Corrected some comments
5/25/99    SH      Initial release.


===========================================================================*/


#include "mmcp_variation.h"
#include "comdef.h"    /* Definition for basic types and macros */
#include "cmi.h"       /* Internal interface to cm.c */

/*===========================================================================

FUNCTION cminband_client_cmd_proc

DESCRIPTION
  Process clients' inband commands.

DEPENDENCIES
  none
RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cminband_client_cmd_proc(

    cm_inband_cmd_s_type  *inband_cmd_ptr
        /* pointer to a CM command */
);


#ifdef CM_GW_SUPPORTED
/*===========================================================================

FUNCTION cminband_rpt_proc

DESCRIPTION
  Process Lower Layer inband related reports

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cminband_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to LL report */
);
#endif /* (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */


#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
/*===========================================================================

FUNCTION cminband_mc_rpt_proc

DESCRIPTION
  Process MC inband related reports

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cminband_mc_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
);
#endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

#ifdef FEATURE_IP_CALL
/*===========================================================================

FUNCTION cminband_ip_rpt_proc

DESCRIPTION
  Send DTMF confirm to client

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cminband_ip_rpt_proc(cm_name_type rpt_type, boolean status);

/*===========================================================================

FUNCTION cminband_ip_ind_proc

DESCRIPTION
  Send DTMF Indication for START/STOP when MT DTMF happens

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cminband_ip_ind_proc(cm_name_type rpt_type, cmipapp_ind_dtmf_ind_s_type dtmf_ind);
#endif

#endif /* CMINBAND_H */

