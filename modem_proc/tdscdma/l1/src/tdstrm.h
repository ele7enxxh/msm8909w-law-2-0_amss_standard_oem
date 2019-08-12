#ifndef TDSTRM_H
#define TDSTRM_H
/*===========================================================================

                    L 1   T R M   A N D   T C X O   M A N A G E R

DESCRIPTION
  This file contains global declarations and external references
  for the L1 TRM and TCXO Manager.

EXTERNALIZED FUNCTIONS

  tdstrm_pri_request
    This function requests TRM for the primary antenna before WCDMA can 
    attempt to acquire.

  tdstrm_stop_cmd
    This function is called to send WL1 into a TDSL1M_WAIT_FOR_TRM_STOP state.

  tdstrm_debug_delay
    This function generates a delay in msec for debugging purposes

  tdstrm_tcxo_set_restriction
    Invoked due to either a restriction change from TCXO Mgr, or when error becomes
    large and UMTS is breaking the TCXO Mgr restriction or we are freezing the trk_lo
    for debugging purposes. This simply posts a local command which does all the work.
    Necessary because we cant call tcxomgr_report() from interrupt context.

  tdstrm_tcxo_set_restriction_cmd
    Generally invoked by the local command TDS_TCXO_SET_RESTRICTION_CMD to actually 
    perform the freezing or unfreezing of the TCXO. This function needs send 
    acks (when TCXO Mgr gives us a new restriction) and reports (when breaking 
    a restriction) to TCXO Mgr when necessary. Can also be invoked directly,
    if we cannot post a local command and wait for it at the moment.

  tdstrm_tcxo_resume_restriction
    Resumes current restriction after WL1 diverges from the restriction due to error
    in the VCO.

  tdstrm_tcxo_release
    Releases control of the TCXO.

  tdstrm_tcxo_get_vco
    This function returns the vco value from the mdsp.

  tdstrm_tcxo_frozen
    Returns if the tcxo is currently frozen or not.

  tdstrm_tcxo_rpush
    Performs the actual rotator push when using TCXO Mgr 3.0

  tdstrm_tcxo_request
    This function requests TCXO Mgr for control of the VCTCXO.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000-2006 by Qualcomm Technologies, Incorporated. All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdstrm.h#1 $
$DateTime: 2016/12/13 07:58:19 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/30/10   scm     Reserve TCXO control after WFW download.
05/13/09   scm     Implement tdstrm_trm_set_starting_trk_lo().
10/03/08   hk      Bringing in 7k Mailine fixes
04/17/07   kps     Add tdstrm_tcxo_resume_restriction()
04/10/07   kps     Fully featurize file under the TRM/TCXOMGR30 features
03/09/07   kps     change the return type for tdstrm_tcxo_rpush()
01/19/06   kps     Added support for Rx Diversity
12/15/06   kps     Delete wl1_tcxo_freeze_request() and wl1_controls_tcxo().
11/21/06   kps     Add wl1_tcxo_freeze_request() and wl1_controls_tcxo()
11/13/06   kps     TCXO Rotator push support
                   Add tdstrm_tcxo_rpush()
11/06/06   kps     Add tdstrm_tcxo_release().
10/27/06   kps     Added support for setting restrictions in TCXOMGR 3.0
10/13/06   kps     Added basic support for TCXOMGR3.0, under FEATURE_WCDMA_TCXOMGR30
10/02/06   kps     Added declaration for tdstrm_stop_cmd.
09/11/06   kps     Created file. Declaration of tdstrm_request_primary_antenna.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "tdscdma_variation.h"
#include "comdef.h"

#include "tdsl1def.h"
#include "rex.h"
#include "tcxomgr.h"
#include "trm.h"
#include "tdsl1mtask.h"

/* TDSRM call back function type */
typedef void TDSRM_REQUEST_DONE_CB_FUNC_TYPE( tdsl1_ext_cmd_type* cmd_ptr );

#define TDSTRM_BAND_CHANGE_TIMEOUT_MSEC 1000

typedef enum
{
  TDSRM_NO_LOCK,             /* non RXTX chain locked for TDS */
  TDSRM_PRI_LOCK,            /* only primary  RXTX chain locked */
  TDSRM_SEC_LOCK,            /* only secondary  RXTX chain locked */
  TDSRM_PRI_SEC_LOCK         /* both primary and secondary RXTX chain locked */
} tdstrm_lock_state_type;

#ifndef FEATURE_XO
typedef enum
{
  TDSL1TRM_TCXOMGR_NONE,       /* Dont ack or report restriction change to TCXO Mgr */
  TDSL1TRM_TCXOMGR_ACK,        /* Ack restriction change to TCXO Mgr */
  TDSL1TRM_TCXOMGR_REPORT      /* Report restriction change to TCXO Mgr */
} tdsl1trm_tcxomgr_ack_type;

typedef struct
{
  tdsl1def_l1_local_cmd_hdr_type        hdr;         /* generic command header */

  tcxomgr_restriction_type     restriction;       /* Indicates the restriction type */
  
  /* Indicates whether the restriction needs to be acknowledged or 
  ** reported to tcxomgr*/
  tdsl1trm_tcxomgr_ack_type             ack_tcxomgr; 

} tdsl1trm_tcxo_restriction_local_cmd_type;
#endif

typedef struct
{
  int32  vco;            /*get from(or set to) XO mgr, in unit of 2^-10 PPM */
  int32  freq_error_ppm; /*get from(or set to) XO mgr, in unit of 2^-10 PPM */
  tcxomgr_uncertainty_type    freq_uncertainty;
}tdsl1trm_xo_param_type;

extern tdsl1trm_xo_param_type tdsl1trm_xo_param;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* Used when waiting for confirmations from mDSP and TRM */
extern rex_timer_type  tdsl1m_timeout_timer;

/* TDSRM module to store the band info before band change */
extern trm_frequency_type_t tdsrm_cur_band;
extern trm_frequency_type_t tdsrm_stored_band;


/*===========================================================================

                     MACROS

===========================================================================*/

/*===========================================================================

                     FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================
FUNCTION     TDSTRM_STOP_CMD

DESCRIPTION
  This function is called to send WL1 into a TDSL1M_WAIT_FOR_TRM_STOP state.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdstrm_stop_cmd(void);

/*===========================================================================
FUNCTION     tdstrm_pri_request_failed

DESCRIPTION
  Error handling in case of the primary antenna request failed.

DEPENDENCIES
  None.

PARAMETERS
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdstrm_pri_request_failed(void);

/*===========================================================================
FUNCTION     TDSTRM_ENABLE_RXD_CMD

DESCRIPTION
  This function is called to process TDS_TRM_ENABLE_RXD_CMD and enable WL1 RxD.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdstrm_enable_rxd_cmd(void);

/*===========================================================================
FUNCTION     TDSTRM_SET_FOFF_DONE_CB

DESCRIPTION
  Another task is requesting control of the secondary antenna. TDS L1 uses
  Forced OFF to actually disable WL1 RxD. Once Forced OFF is set, and
  RxD is disabled, this function will be called.

  This function then releases the secondary antenna, and re-requests it so 
  that WL1 will get the antenna back once the other task releases it.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdstrm_set_foff_done_cb(void);
/*===========================================================================
FUNCTION     TDSTRM_SECONDARY_TRM_RELEASE

DESCRIPTION
  This function is called once FOFF is set to TRUE while dropping a call to
  release the secondary antenna.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tdstrm_secondary_trm_release(void);

/*===========================================================================
FUNCTION     TDSRM_PRI_REQUEST

DESCRIPTION
  This function is called to require RF resource for both the primary RF chain
  and the secondary RF chain for TDS.

DEPENDENCIES
  None.

PARAMETERS
  band information.

RETURN VALUE
  ENUM.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean tdstrm_pri_request(trm_frequency_type_t  tds_band);

/*===========================================================================
FUNCTION     TDSRM_SEC_REQUEST

DESCRIPTION
  This function is called to require RF resource for both the primary RF chain
  and the secondary RF chain for TDS.

DEPENDENCIES
  None.

PARAMETERS
  band information.

RETURN VALUE
  ENUM.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean tdstrm_sec_request(void);

/*===========================================================================
FUNCTION     TDSRM_RELEASE

DESCRIPTION
  This function is called to release RF resource for both the primary RF chain
  and the secondary RF chain for TDS.

DEPENDENCIES
  None.

PARAMETERS
  NONE.

RETURN VALUE
  NONE.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdstrm_release(void);

/*===========================================================================
FUNCTION     TDSRM_BAND_CHANGE_REQUEST

DESCRIPTION
  This function is called to request band change.

DEPENDENCIES
  None.

PARAMETERS
  BAND information.

RETURN VALUE
  NONE.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean tdstrm_band_change_request(trm_frequency_type_t  tds_new_band);

/*===========================================================================
FUNCTION     TDSRM_IS_LOCKED

DESCRIPTION
  This function is called to query whether TDSRM is in lock state or not.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean: locked or not.

SIDE EFFECTS
  None.
===========================================================================*/

extern boolean tdsrm_is_locked(void);

/*===========================================================================
FUNCTION     TDSTRM_GET_BAND

DESCRIPTION
  This function is to get TDS band for given frequency.
  The frequency value is specified as the UTRA Absolute Radio Frequency Channel Number (UARFCN) 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.
===========================================================================*/

extern sys_band_class_e_type tdstrm_get_band(uint16  freq);

/*===========================================================================
FUNCTION     TDSTRM_IS_BAND_CHANGED

DESCRIPTION
  This function is called to unlock TDSRM state.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.
===========================================================================*/

extern boolean tdstrm_is_band_changed(trm_frequency_type_t  tds_new_band);

#ifndef FEATURE_XO
/*===========================================================================
FUNCTION     TDSTRM_TCXO_SET_RESTRICTION

DESCRIPTION
  Invoked due to either a restriction change from TCXO Mgr, or when error becomes
  large and UMTS is breaking the TCXO Mgr restriction or we are freezing the trk_lo
  for debugging purposes. This simply posts a local command which does all the work.
  Necessary because we cant call tcxomgr_report() from interrupt context.

DEPENDENCIES
  None.

PARAMETERS
   tcxomgr_restriction_type    restriction  - (New) restriction type.
   tdsl1trm_tcxomgr_ack_type        ack          - Indicates whether the change needs to be
                                              acked or reported.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdstrm_tcxo_set_restriction(tcxomgr_restriction_type restriction,
                                     tdsl1trm_tcxomgr_ack_type ack);

/*===========================================================================
FUNCTION     TDSTRM_TCXO_SET_RESTRICTION_CMD

DESCRIPTION
  Invoked by the local command TDS_TCXO_SET_RESTRICTION_CMD  to actually perform the
  freezing or unfreezing of the TCXO. This function needs send acks (when TCXO
  Mgr gives us a new restriction) and reports (when breaking a restriction) to
  TCXO Mgr when necessary.

DEPENDENCIES
  None.

PARAMETERS
   tcxomgr_restriction_type    restriction  - (New) restriction type.
   tdsl1trm_tcxomgr_ack_type        ack          - Indicates whether the change needs to be
                                              acked or reported.

RETURN VALUE
  None.

SIDE EFFECTS
    None.
===========================================================================*/
extern void tdstrm_tcxo_set_restriction_cmd(tcxomgr_restriction_type restriction,
                                         tdsl1trm_tcxomgr_ack_type ack);

/*===========================================================================
FUNCTION     TDSTRM_TCXO_RESUME_RESTRICTION

DESCRIPTION
  Resumes current restriction after WL1 diverges from the restriction due to error
  in the VCO.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdstrm_tcxo_resume_restriction(void);

/*===========================================================================
FUNCTION     TDSTRM_TCXO_FROZEN

DESCRIPTION
  Returns if the tcxo is currently frozen or not.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean - TRUE if the TCXO is frozen.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean tdstrm_tcxo_frozen(void);

/*===========================================================================
FUNCTION     TDSTRM_TCXO_DISABLE_PDM_FOR_SLEEP

DESCRIPTION
  Disable TRK_LO PDM via tcxomgr, if there is no restriction on the TCXO.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdstrm_tcxo_disable_pdm_for_sleep(void);

/*===========================================================================
FUNCTION     TDSTRM_TCXO_ENABLE_PDM_FOR_WAKE

DESCRIPTION
  Enable TRK_LO PDM via tcxomgr.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdstrm_tcxo_enable_pdm_for_wake(void);
#endif
/*===========================================================================
FUNCTION     TDSTRM_TCXO_RPUSH

DESCRIPTION
  Performs the actual rotator push when using TCXO Mgr 3.0

DEPENDENCIES
  None.

PARAMETERS
  tcxomgr_client_state_type state    - Indicates whether WL1 is in 
                                       Idle/Traffic/OOS.

RETURN VALUE
  boolean                            - Indicates whether the Rotator push
                                       succeeded. (It can fail if there are no
                                       samples in the accumulator)

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean tdstrm_tcxo_rpush(tcxomgr_client_state_type state);

/*===========================================================================
FUNCTION     TDSTRM_TCXO_RPUSH_INIT

DESCRIPTION
  Initialize RPush while L1 is entering BCH, FACH and DCH states.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void tdstrm_tcxo_rpush_init(void);

/*===========================================================================
FUNCTION     TDSTRM_TCXO_RPUSH_CLEANUP

DESCRIPTION
  Clean up while L1 is leaving BCH, FACH and DCH states.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void tdstrm_tcxo_rpush_cleanup(void);

/*===========================================================================
FUNCTION     TDSTRM_TCXO_REQUEST

DESCRIPTION
  This function requests TCXO Mgr for control of the VCTCXO.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean - FALSE if TCXO request times out, or TCXO Mgr denies control to WCDMA.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean tdstrm_tcxo_request(void);

/*===========================================================================
FUNCTION     TDSTRM_TCXO_RELEASE

DESCRIPTION
  Releases control of the TCXO.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdstrm_tcxo_release(void);

#endif /* TDSTRM_H */
