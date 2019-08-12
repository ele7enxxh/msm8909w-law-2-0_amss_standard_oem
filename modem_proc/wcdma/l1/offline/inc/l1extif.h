
#ifndef L1EXTIF_H
#define L1EXTIF_H

/*===========================================================================
                              L1EXTIF.H

GENERAL DESCRIPTION

  This file contains all the routines for CFN callback mechanism. The CFN 
  callback mechansim allows any module (RRC etc) can register for a 
  callback function that will be called every CFN in all the states.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright(c)2005-2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:$
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/l1extif.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

 when        who     what, where, why  
 07/01/14    hk      Header file inclusion cleanup
 07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
 04/05/12    gsk     Moving wl1_check_state_for_delayed_gstmr_int to wl1api.h
 04/29/09    mg      Add extern l1m_print_debug_msgs
 01/13/09    vsr     Added a structure variable to change the way MAC callback
                     is avoided during HHO
 07/15/08    rgn     Added backward compatible cfn based notify function
 02/22/07    mg      Add wl1_check_state_for_delayed_gstmr_int
 07/06/06    rmak    Changed l1m_get_state to return uint8 to avoid
                     exposing l1m_state_enum_struct to non-L1 code
 07/13/05    vp      Code changes to support the new MAC-CFN update mechanism.
 --------   ---     ---------------------------------------------------------

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "wfw_sw_intf.h"

typedef enum
{
  START_G2W_MODE = WFW_START_MODE_X2W,
  /** START WCDMA*/
  START_WCDMA_MODE = WFW_START_MODE_WCDMA
}mdsp_sync_start_cfg_enum_type;

/* Call back function type to get l1 sequencer cfn for every cfn update event*/
typedef void (*L1_SEQ_GET_CFN_CB_FUNC_TYPE) (uint8);

#define WL1_INFINITE_CFN_UPDATE 0xFFFF

typedef struct
{
  /** cfn notify callback function */
  L1_SEQ_GET_CFN_CB_FUNC_TYPE cfn_notify_cb_func;
  /** cfn for which the callback needs to be called, set it to
   *  continous update  */
  uint16 notify_cfn;
  /** Flag to avoid calls when HHO is ongoing*/
  boolean avoid_during_hho;
}l1_cfn_update_notify_struct_type;

/*===========================================================================
FUNCTION     L1_SEQ_REGISTER_GET_CFN_CB_FUNC

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
boolean l1_seq_register_get_cfn_cb_func(L1_SEQ_GET_CFN_CB_FUNC_TYPE cb_func);

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
boolean l1_seq_deregister_get_cfn_cb_func(L1_SEQ_GET_CFN_CB_FUNC_TYPE cb_func);
/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Functions                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION      l1m_get_state

DESCRIPTION
  This function returns the value of l1m_state as uint8

DEPENDENCIES
  None

RETURN VALUE
   (uint8) l1m_state

SIDE EFFECTS

===========================================================================*/
extern uint8 l1m_get_state(void);


#ifdef FEATURE_RRC_DELAY_ERR_FATAL
/*===========================================================================

FUNCTION      L1M_PRINT_DEBUG_MSGS

DESCRIPTION
  Print debug information.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void l1m_print_debug_msgs( void );
#endif /* FEATURE_RRC_DELAY_ERR_FATAL */

/* call back function type for failed block passing to higher layer info */
typedef void (*DL_DEC_CTCH_PDU_DECODE_ERROR_CB_FUNC_TYPE)(dword);

#endif /* WL1I_H */

