/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC ETSI TEST MODE MODULE
***
***
*** DESCRIPTION
***
***  Definitions of ETSI Test Mode Functions
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlctestmode.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 03/29/01    hv     Created
*** 09/19/12    hv     Added dual data space functionality
***
*****************************************************************************/

#ifndef INC_GRLCTESTMODE_H
#define INC_GRLCTESTMODE_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#include "grlcdl.h"
#include "geran_multi_sim.h"

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      DS_RX_ACK_TEST_MODE_B_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in GPRS Test Mode state in ACK mode.
===
===  DEPENDENCIES
===    
===   None
===  
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
void ds_rx_ack_test_mode_b_state_pl1_ph_data_ind_event_handler( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      E_DS_RX_ACK_TEST_MODE_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in DS_RX_ACK state in EGPRS TEST Mode B and SRB modes.
===
===  DEPENDENCIES
===    
===   None
===  
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
void e_ds_rx_ack_test_mode_state_pl1_ph_data_ind_event_handler( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      DS_RX_UNACK_TEST_MODE_B_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in TEST_MODE_B state in GPRS mode.
===
===  DEPENDENCIES
===    
===   None
===  
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
void ds_rx_unack_test_mode_b_state_pl1_ph_data_ind_event_handler( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      E_DS_RX_UNACK_TEST_MODE_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in TEST_MODE_B and SRB state in EGPRS mode.
===
===  DEPENDENCIES
===    
===   None
===  
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
void e_ds_rx_unack_test_mode_state_pl1_ph_data_ind_event_handler( gas_id_t gas_id );

#endif /* INC_GRLCTESTMODE_H */

/*** EOF: don't remove! ***/
