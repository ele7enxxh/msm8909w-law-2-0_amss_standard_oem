
#ifndef TDSL1EXTIF_H
#define TDSL1EXTIF_H

/*===========================================================================
                              L1EXTIF.H

GENERAL DESCRIPTION

  This file contains all the routines for CFN callback mechanism. The CFN 
  callback mechansim allows any module (RRC etc) can register for a 
  callback function that will be called every CFN in all the states.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright(c)2005-2006 by Qualcomm Technologies Incorporated.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:$
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/inc/tdsl1extif.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

 when        who     what, where, why
 04/29/09    mg      Add extern tdsl1m_print_debug_msgs
 01/13/09    vsr     Added a structure variable to change the way MAC callback
                     is avoided during HHO
 07/15/08    rgn     Added backward compatible cfn based notify function
 02/22/07    mg      Add tdsl1extif_check_state_for_delayed_gstmr_int
 07/06/06    rmak    Changed tdsl1m_get_state to return uint8 to avoid
                     exposing l1m_state_enum_struct to non-L1 code
 07/13/05    vp      Code changes to support the new MAC-CFN update mechanism.
 --------   ---     ---------------------------------------------------------

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tdscdma_variation.h"
#include "comdef.h"
#ifdef FEATURE_TDSCDMA_CFCM
#include "tdsrlcfc.h"
#endif

/* Call back function type to get l1 sequencer cfn for every cfn update event*/
typedef void (*TDSL1EXTIF_SEQ_GET_CFN_CB_FUNC_TYPE) (uint8);

#define TDSL1EXTIF_INFINITE_CFN_UPDATE 0xFFFF

typedef struct
{
  /** cfn notify callback function */
  TDSL1EXTIF_SEQ_GET_CFN_CB_FUNC_TYPE cfn_notify_cb_func;
  /** cfn for which the callback needs to be called, set it to
   *  continous update  */
  uint16 notify_cfn;
  /** Flag to avoid calls when HHO is ongoing*/
  boolean avoid_during_hho;
}tdsl1extif_cfn_update_notify_struct_type;

/*===========================================================================
FUNCTION     tdsl1extif_seq_register_get_cfn_cb_func

DESCRIPTION  This function registers the call back function which is used to notify the cfn

DEPENDENCIES
  The callback function will be called in the interrupt context. Hence the callback 
  function has to be crisp. Also, L1 may have to do some modifications if any new
  modules deicdes to use this module.

RETURN VALUE
  Boolean TRUE if registration succeed else FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsl1extif_seq_register_get_cfn_cb_func(TDSL1EXTIF_SEQ_GET_CFN_CB_FUNC_TYPE cb_func);

/*===========================================================================
FUNCTION     L1_SEQ_DEREGISTER_GET_CFN_CB_FUNC

DESCRIPTION  This function deregisters the call back function which is used to notify the cfn., 
                    last entry of the call back func table is copied to the deleted entry and Number of 

DEPENDENCIES
  None

RETURN VALUE
  Boolean TRUE if deregistration succeed else FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsseq_l1_deregister_get_cfn_cb_func(TDSL1EXTIF_SEQ_GET_CFN_CB_FUNC_TYPE cb_func);
/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Functions                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION      tdsl1m_get_state

DESCRIPTION
  This function returns the value of tdsl1m_global.l1_state as uint8

DEPENDENCIES
  None

RETURN VALUE
   (uint8) tdsl1m_global.l1_state

SIDE EFFECTS

===========================================================================*/
extern uint8 tdsl1m_get_state(void);

/*===========================================================================

FUNCTION      tdsl1extif_check_state_for_delayed_gstmr_int

DESCRIPTION
  This function controls weather the gstmr delayed error message can be printed 
  or not. If returns TURE error message can be printed else skipped.
  
DEPENDENCIES
  None

RETURN VALUE
   TRUE or FALSE

SIDE EFFECTS

===========================================================================*/
extern boolean tdsl1extif_check_state_for_delayed_gstmr_int(void); 

/*===========================================================================

FUNCTION      tdsl1m_print_debug_msgs

DESCRIPTION
  Print debug information.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdsl1m_print_debug_msgs( void );

#ifdef FEATURE_TDSCDMA_CFCM
/*===========================================================================

FUNCTION      tdsl1_fc_send_fw_cmd

DESCRIPTION
  Send FW CPU flow control command.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdsl1_fc_send_fw_cmd(tdsrlc_fc_fw_cmd_e_type fw_cmd,  uint8 *tds_fc_rtbs_value, uint16 tds_fc_rtbs_index );
#endif

#endif /* TDSL1I_H */

