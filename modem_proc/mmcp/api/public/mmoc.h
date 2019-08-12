#ifndef MMOC_H
#define MMOC_H
/**===========================================================================
@FILE_DOC

                    M U L T I M O D E   C O N T R O L L E R

                             H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary for Call Manager
  (CM) and the protocols ( MC, HDR and REG) to interface with the
  Multi-MOde Controller (MMoC) task.

Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/* <EJECT> */
/**===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM_MM/APPS/vcs/mmoc.h_v   1.3   10 Jun 2002 11:35:48   sjaikuma  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/mmoc.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/10   rk      Adding support for dormant GWL protocol handling in MMOC
06/18/10   xs      Add SENTINEL metacomments for maximum value sentinel in the enum 
01/28/10   mp      EF_EPRL Implementation
11/21/09   rn      LTE-eHRPD Redirection support
09/10/09   rm      Removing inclusion of customer.h from /api files
07/06/09   rm      Moving mmoc_task from mmoc_v.h to mmoc.h
02/23/09   rm      CMI Phase-2 : Removing inclusion of private header files
                   from public
07/30/08   rm      CMI defeaturisation
09/04/08   sv      Added WLAN_WPS API
04/28/08   fj      Lint cleanup, CR 144389
04/17/07   rp      Added RPC metacomments for WLAN.   
04/16/07   ic      Moved inclusion of AEE* headers to mmoci.h
03/19/07   ka      Replacing banned apis
01/31/07   ka      Adding new state in MMOC to wait for TMC to ack operating
                   mode change.
11/15/06   sk      Added hybrid GW support.
08/29/06   sk      Added WLAN Adhoc support.
04/03/06   ka      Changing copyright.
03/02/05   ka      Added prot_subsc_chag param to mmoc_cmd_subscription_chgd
08/20/04   sj      Added support for prl_pref.
02/27/03   SJ      Added support sanity_timeout err_fatal in respective
                   protocols.
02/13/03   SJ      Added mmoc_cmd_wakeup_from_pwr_save API.
01/16/03   SJ      Added define to indicate if MMOC uses DB.
09/03/02   SJ      Unfeaturized GSM support in MMoC.
03/28/02   SJ      Initial release.

===========================================================================*/
#include "comdef.h"    /* Definition for basic types and macros */
#include "prot.h"      /* Common types associated with MMoC <-> protocol
                       ** communciation.
                       */

/** Enumeration of sanity error type.
*/
typedef enum {

  MMOC_SANITY_ERR_DEACT_CMD_CNF,       /**< MMoC's sanity error was because the
                                       ** protocol did not respond to
                                       ** deactivate request send by MMOC.
                                       */

  MMOC_SANITY_ERR_PH_STAT_CMD_CNF,     /**< MMoC's sanity error was because the
                                       ** protocol did not respond to phone
                                       ** status changed command sent by MMOC.
                                       */

  MMOC_SANITY_ERR_GENERIC_CMD_CNF,     /**< MMoC's sanity error was because the
                                       ** protocol did not respond to the
                                       ** generic cmd sent by MMOC.
                                       */

  MMOC_SANITY_ERR_MMGSDI_READ_CNF,     /**< MMoC's sanity error was because the
                                       ** MMGSDI did not respond to the
                                       ** read request sent by MMOC.
                                       */
  MMOC_SANITY_ERR_MAX

} mmoc_sanity_err_e_type;
/*~ SENTINEL mmoc_sanity_err_e_type.MMOC_SANITY_ERR_MAX */

/** Enumeration of PH STAT error type.
*/
typedef enum {

  MMOC_PH_STAT_NO_ERROR,     /**<No error happened while processing ph_stat
                             */

  MMOC_PH_STAT_ERR_SIM_BUSY, /**<SIM Busy error happened while processing ph_stat
                             */

  MMOC_PH_STAT_ERR_NV_ERR,   /**<NV read error happened while processing ph_stat. 
                             ** This error code can be sent to CM by HDR or 1x
                             ** during subscription change.
                             */

  MMOC_PH_STAT_MAX

} mmoc_ph_stat_e_type;


/*===========================================================================

FUNCTION mmoc_cmd_deact_from_dormant

DESCRIPTION
  This function will be used to command the MMoC to deactivate  protocols in 
  given stack from its dormant state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_cmd_deact_from_dormant
(

        sd_ss_e_type                   ss,
          /* SS for which protocol has to be deactivated if dormant
          */

        sd_ss_mode_pref_e_type         mode
          /* Modes which have to be deactivated from dormant state
          */

);

/** @}
** 
*/

/**===========================================================================

@FUNCTION mmoc_rpt_prot_deactd_cnf

@DESCRIPTION
  This function will be used to confirm that the MMoC that the protocol has
  been deactivated. This function will queue the protocol deactivated report
  to the MMoC task.

  Note! This function should be called only in response to protocol
  deactivate request from MMoC.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_rpt_prot_deactd_cnf
(

        prot_trans_type                trans_id
           /**< Protocols set this same as the trans_id received in their
           ** Deactivate request.
           */

);
/*~ FUNCTION mmoc_rpt_prot_deactd_cnf */

/**===========================================================================

@FUNCTION mmoc_rpt_prot_auto_deactd_ind

@DESCRIPTION
  This function will be used to inform the MMoC that the protocol has been
  deactivated autonomously. This function will queue the deactivated
  indication report to the MMoC task.

  Note! This function should not be called in response to a protocol
  deactivate request from MMoC.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_rpt_prot_auto_deactd_ind
(

        prot_deact_e_type              deactd_reason,
           /**< Reason that the protocol was deactivated.
           */

        prot_state_e_type              prot_state
           /**< Protocol which sent the autonomous deactivation.
           */


);
/*~ FUNCTION mmoc_rpt_prot_auto_deactd_ind */

/**===========================================================================

@FUNCTION mmoc_rpt_prot_auto_actd_ind

@DESCRIPTION
  This function will be used to inform the MMoC that the protocol has been
  activated autonomously. This function will queue the activated
  indication report to the MMoC task.

  Note! This function should not be called in response to a protocol
  activate request from MMoC.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_rpt_prot_auto_actd_ind
(

        prot_act_e_type              actd_reason,
           /**< Reason that the protocol was activated.
           */

        prot_state_e_type              prot_state
           /**< Protocol which sent the autonomous activation.
           */


);
/*~ FUNCTION mmoc_rpt_prot_auto_actd_ind */

#define MMOC_API_PH_STAT_CNF_WITH_STATUS


/**===========================================================================

FUNCTION mmoc_proc_prot_gen_cmd

@DESCRIPTION
  This function should be used by protocols to process the Generic command
  received from MMoC.  The Generic system could be received because of
  Reselection timeout, Get networks request, Terminate get networks or
  preferred system selection change.


@DEPENDENCIES
  None.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an HDR system.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH.

    SD_SS_ACT_PWR_SAVE  - Enter power save mode.


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type               mmoc_proc_prot_gen_cmd
(

        boolean                        is_resel_allowed,
          /**< Protocol indicated if reselection is allowed at this time or not.
          ** true = reselection allowed. This flag is passed on to SD20 in the
          ** call to sd_ss_ind_misc_timer(). So, this parameter is used only
          ** if the gen_ss_cmd type is Resetecion timer expired, for other
          ** types it is not used.
          */

        boolean                        is_prot_active,
          /**< Flag which specifies if the protocol calling this function is
          ** ACTIVE or not.
          */

        const prot_gen_cmd_s_type            *gen_cmd_ptr
           /**< Pointer to generic system selection command information.
           ** Same as what was received by the protocols.
           */
);
/*~ FUNCTION mmoc_proc_prot_gen_cmd */
/*~ PARAM gen_cmd_ptr POINTER */

/*===========================================================================
=============================================================================
=============================================================================
============================== Protocols -> MMOC functional interface =======
=============================================================================
=============================================================================
===========================================================================*/

/** @defgroup PMFI Protocols->MMOC Functional Interface
*  @{
*/

/* The following section defines the API when the clients ( example
** protocols ) have to communicate to MMoC using the functional interface.
*/





/**===========================================================================

@FUNCTION mmoc_task

@DESCRIPTION
   This function is the entrance for the MMoC task. This is an infinite loop
   outside of the state machine which handles the input signals.

@DEPENDENCIES

@RETURN VALUE
  void.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  void                           mmoc_task(

        dword                          ignored
           /**< Parameter received from Rex - ignored.
           */

);

#endif /* MMOC_H */

/** @}
**
*/
