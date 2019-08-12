/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC NULL MODE MODULE
***
***
*** DESCRIPTION
***
***  This module administrates the signal reception and the associated actions
***  for null mode states.
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/inc/gmacnull.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 20/11/01   sb      Created
***
*****************************************************************************/

#ifndef INC_GMACNULL_H
#define INC_GMACNULL_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif


/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/

/********* NULL mode sub states ************/
typedef enum
{
  NULL_INACTIVE,
  NULL_GPRS_51_ACTIVE,
  NULL_SUSPENDED,
  NULL_SUSPEND_RESEL,
  NULL_UPLINK_TBF_ESTAB,
  NULL_UPLINK_SB_ESTAB,
  NULL_DOWNLINK_SB_ESTAB,
  NULL_UPLINK_TBF_ACCESS,
  NULL_DL_TBF_EST

/*tld dtm*/
#ifdef  FEATURE_GSM_DTM
 ,NULL_DTM_CONNECT_PENDING
 ,NULL_DTM_GTTP_UL_CON
#endif /* FEATURE_GSM_DTM */

 ,NULL_RESET
}gprs_mac_null_substates_t;

typedef struct
{
  /***** gmac_pcco_T3174_exp_flag ****************************************************************
   *
   ************************************************************************************************/
  boolean gmac_pcco_T3174_exp_flag;

  /***** gmac_null_ul_sb_to_l1 ****************************************************************
   *
   ************************************************************************************************/
  boolean gmac_null_ul_sb_to_l1;

  /***** ps_page_response ****************************************************************
   *
   * indication that establishment is for a page response
   *
   ************************************************************************************************/
  boolean ps_page_response;

  /***** rlc_ul_reset_cnf_pending ****************************************************************
   *
   * Indicates that MAC is waiting for a reset confirm from RLC UL task.
   *
   ************************************************************************************************/
  boolean rlc_ul_reset_cnf_pending;

  /***** rlc_dl_reset_cnf_pending ****************************************************************
   *
   * Indicates that MAC is waiting for a reset confirm from RLC DL task.
   *
   ************************************************************************************************/
  boolean rlc_dl_reset_cnf_pending;

}null_flags_t;

typedef enum
{
  GMAC_NULL_NO_RELEASE_IN_PROGRESS,
  GMAC_NULL_TBF_REL_NORMAL,
  GMAC_NULL_GRR_INITIATED_NO_PS_ACCESS,
  GMAC_NULL_TBF_REL_UL_EST_INT_BY_DL_EST,
  GMAC_NULL_TBF_REL_UL_EST_INT_BY_DL_SB_EST
} gmac_null_tbf_rel_t;



/*****************************************************************************
***
***     Public Data
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Function Prototypes
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      null_mode_handler
===
===  DESCRIPTION
===  Handles the signal reception and the associated actions
===  for null mode states.
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void null_mode_handler(uint8 task_id, uint8 signal_id, gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_init_null_flags()
===
===  DESCRIPTION
===  Initialises Null flags, should be called once at start up.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_null_flags( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION    gmac_init_null_data()
===
===  DESCRIPTION
===  Initialises Null data.  Used for Lower layer resets.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_null_data( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION    gmac_set_mac_state_for_reset()
===
===  DESCRIPTION
===  When the lower layers panic reset then this function sets the MAC state.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_set_mac_state_for_reset( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION    gmac_null_get_null_state()
===
===  DESCRIPTION
===  This is an access function for external entities to get the mac_null_substate.
===
===  PARAMS
===
===
===  RETURN VALUE
===  gprs_mac_null_substates_t
===
===  SIDE EFFECTS
===
===
===========================================================================*/
gprs_mac_null_substates_t gmac_null_get_null_state(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_init_null_data_non_zero()
===
===  DESCRIPTION
===  Initialises non zero (including all enums) null data.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_null_data_non_zero( gas_id_t gas_id );

#ifdef PERLUTF
#error code not present
#endif

#endif /* INC_GMACNULL_H */
/*** EOF: don't remove! ***/
