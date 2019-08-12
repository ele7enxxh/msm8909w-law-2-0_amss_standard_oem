#ifndef SRCH_RX_DIV_H
#define SRCH_RX_DIV_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             S R C H    R X   D I V  ---  H E A D E R   F I L E

GENERAL DESCRIPTION
  This module contains external header information for SRCH to use the RX DIV
  state machine.

EXTERNALIZED FUNCTIONS  (all SRCH regional)

  srch_rx_div_is_enabling - returns true if diversity is active or enabling.
  srch_rx_div_is_enabled - returns true if we're currently doing rx diversity.
  srch_rx_div_is_disabled - returns true if diversity is not in progress.
  srch_rx_div_allowed_for_bc - returns true if div is allowed for band class.
  srch_rx_div_set_ctrl  - sets RX diversity nv item
  srch_rx_div_is_capable - returns true if this phone supports rx diversity.
  srch_rx_div_search_comb_is_enabled - returns true if combining is enabled.
  srch_rx_div_get_div_comb_method - returns the rx diversity combining method.
  srch_rx_div_get_algo - returns the diveristy combining algorithm type.
  srch_rx_div_is_auto_enabled - returns true if rx diversity autoswitching is
                                enabled.
  srch_rx_div_is_off_with_no_ffpc - returns where diversity is disabled with
                                    no ffpc.
  srch_rx_div_is_bc_enable_mask_enabled - returns true if band class diversity
                                          switching is enabled.
  srch_rx_div_set_bc_enable_mask - sets the band class enable mask RX diversity
                                   nv item.
  srch_rx_div_is_bc_enable_mask_enabled - returns true if band class diversity
                                          switching is enabled.
  srch_rx_div_bc_enable_mask - returns the band class enable mask.
  srch_rx_div_request - requests that the SRCH_RX_DIV state machine
                        try and do diversity.
  srch_rx_div_stop - tells the SRCH_RX_DIV state machine to stop diversity.
  srch_rx_div_update_state - updates the rx diversity state depending on band
                             class and whether ffpc is enabled.
  srchrdiv_set_ftm_second_chain_test - sets the FTM test mode for using 2nd
                                       Rx only.
  srchrdiv_get_ftm_second_chain_test - returns true if the FTM 2nd chain test
                                       mode is set.

  srch_rx_div_override_chain_selection - Common API to override chain selection

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

  Copyright (c) 2005 - 2013 Qualcomm Technologies, Inc. 
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/srch_rx_div.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/15/13   bb      Changes to abort ongoing searches if Searcher combining
                   algoritm is enabled
05/11/11   cjb     Added common API to override chain selection.
11/15/10   jtm     Initial SU API split for CP.
11/09/10   jtm     Initial SU API split.
10/28/10   mca     Added support for diversity search combining methods.
06/08/10   adw     Updated RxDiv en/dis api comment block.
05/19/10   sst     Added new api command for runtime RxDiv en/dis
10/21/09   adw     Added a func to return true if div is active or enabling.
04/01/09   adw     Categorized included header files and standardized format.
03/31/09   adw     Eliminated implicit includes.
03/25/09   adw     Include modem_1x_defs.h for 1x specific modem defines.
02/10/09   adw     Merge from 2h08 branch, based on VU_MODEM_1X_SRCH.13.00.01
11/19/08   adw     Merge from main/latest, based on VU_MODEM_1X_SRCH.12.00.24
11/13/08   adw     Added external function srch_rx_div_allowed_for_bc().
05/01/07   tjc     Add bit to rx diversity nv item to enable/disable rx
                   diversity when FFPC is not available.
                   Add bit to rx diversity nv item to enabled/disable rx
                   diversity on a BC basis.
04/30/07   sst     Update RX Diversity featurization
02/13/07   tjc     Changed function name to match what it is doing
                   Start/Stop now enable/disable autodiversity
11/10/06   tjc     Add bit to rx diversity nv item to enable/disable
                   autoswitching
08/29/05   ejv     Remove featurization from *ftm_second_chain_test.
05/31/05   rng     Merged from SHDR Sandbox.
01/05/05   gs      Implementation, first cut.
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "comdef.h"


/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------
      Data types
-------------------------------------------------------------------------*/

/* Diversity search combining type */
typedef enum
{
  SRCH_RX_DIV_COMB_AVG,   /* Average the energies */
  SRCH_RX_DIV_COMB_SUM,   /* Sum the energies */
  SRCH_RX_DIV_COMB_MAX    /* Take the maximum value of the two energies */

} srch_rx_div_comb_enum_type;


/*===========================================================================

                    FUNCTION DECLARATIONS FOR MODULE

===========================================================================*/

/* the following functions are defined outside of the RxDiv featurization to
   remove dependency requirements */

/*===========================================================================

FUNCTION        SRCH_RX_DIV_SEARCH_COMB_IS_ENABLED

DESCRIPTION     This function returns if we are using diversity search 
                combining (as opposed to only diversity finger combining) 

DEPENDENCIES    None

RETURN VALUE    TRUE    : Rx Diversity searching combining enabled
                FALSE   : Rx Diversity searching combining disabled

SIDE EFFECTS    None

===========================================================================*/
extern boolean srch_rx_div_search_comb_is_enabled( void );


/*===========================================================================

FUNCTION        SRCH_RX_DIV_SEARCH_COMB_NV_IS_ENABLED

DESCRIPTION     This function returns if we are using diversity search
                combining (as opposed to only diversity finger combining)

DEPENDENCIES    None

RETURN VALUE    TRUE    : Rx Diversity searching combining enabled
                FALSE   : Rx Diversity searching combining disabled

SIDE EFFECTS    None

===========================================================================*/
boolean srch_rx_div_search_comb_nv_is_enabled( void );

/*===========================================================================

FUNCTION        SRCH_RX_DIV_GET_DIV_COMB_METHOD

DESCRIPTION     This function returns the diversity search combining 
                method

DEPENDENCIES    None

RETURN VALUE    Combining method

SIDE EFFECTS    None.

===========================================================================*/
extern srch_rx_div_comb_enum_type srch_rx_div_get_div_comb_method( void );

/*===========================================================================

FUNCTION       SRCHRDIV_SET_FTM_SECOND_CHAIN_TEST

DESCRIPTION    Sets the FTM test mode for using 2nd Rx only.

DEPENDENCIES

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srchrdiv_set_ftm_second_chain_test( boolean test_mode );

/*===========================================================================

FUNCTION       SRCHRDIV_GET_FTM_SECOND_CHAIN_TEST

DESCRIPTION    Retrieves the FTM test mode for using 2nd Rx only.

DEPENDENCIES

RETURN VALUE   true if the FTM second chain test mode is set.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srchrdiv_get_ftm_second_chain_test( void );

/*===========================================================================

FUNCTION       SRCH_RX_DIV_OVERRIDE_CHAIN_SELECTION

DESCRIPTION    This function overrides chain selection.

DEPENDENCIES   None.

RETURN VALUE   TRUE if successful. Else, returns FALSE.

SIDE EFFECTS   Affects chain selection and DIV operation.

===========================================================================*/
extern boolean srch_rx_div_override_chain_selection
(
  uint8 chain_select       /* bit mask */
);

#endif /* SRCH_RX_DIV_H */
