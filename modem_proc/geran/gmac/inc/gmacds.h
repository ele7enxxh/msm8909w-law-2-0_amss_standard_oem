/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC Data Space
***
***
*** DESCRIPTION
***
***  Contains utility functions and definitions for data spacing (multi
***  SIM operation)
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/inc/gmacds.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 20/09/12   SB      Created
***
*****************************************************************************/

#ifndef INC_GMACDS_H
#define INC_GMACDS_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#include "geran_multi_sim.h"
#include "geran_tasks.h"
#include "gmacl1sig.h"
#include "gmacl1sig_g.h"
#include "gmacgrrsig.h"
#include "mac_grr.h"
#include "gmacsigl1.h"
#include "gmacsigl1_g.h"
#include "gs.h"
#include "gmacrlc.h"
#include "grlcmac.h"

/*****************************************************************************
***
***     Definitions
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/
/*===========================================================================
===
===  FUNCTION    gmac_get_current_gas_id()
===
===  DESCRIPTION
===  Returns the current active gas_id.
===
===  DEPENDENCIES
===  
===
===  RETURN VALUE
===    gas_id
=== 
===  SIDE EFFECTS
===    none
===
===
===========================================================================*/
gas_id_t gmac_get_current_gas_id(void);

/*===========================================================================
===
===  FUNCTION    gmac_map_task_to_gas_id()
===
===  DESCRIPTION
===  Mapping of TASK_ID to GAS_ID
===
===  DEPENDENCIES
===  
===
===  RETURN VALUE
===    task_id
=== 
===  SIDE EFFECTS
===    none
===
===
===========================================================================*/
gas_id_t gmac_map_task_id_to_gas_id(geran_task_id_t task_id);

/*===========================================================================
===
===  FUNCTION    gmac_map_gas_id_to_task_id()
===
===  DESCRIPTION
===  Mapping of GAS_ID to TASK_ID
===
===  DEPENDENCIES
===  
===
===  RETURN VALUE
===    task_id
=== 
===  SIDE EFFECTS
===    none
===
===
===========================================================================*/
geran_task_id_t gmac_map_gas_id_to_task_id( gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_map_gas_id_to_layer1_queue()
===
===  DESCRIPTION
===  Mapping of GAS_ID to GS L1 Queue
===
===  DEPENDENCIES
===  
===
===  RETURN VALUE
===    gs_queue_id_T
=== 
===  SIDE EFFECTS
===    none
===
===
===========================================================================*/
gs_queue_id_T gmac_map_gas_id_to_layer1_queue( gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_grr_mac_msg_insert_gas_id
===
===  DESCRIPTION
===  Inserts the current active gas_id into a message.
===
===  INPUT
===  Pointer to GRR message structure of the message to be sent.
===  gas_id
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_grr_mac_msg_insert_gas_id(mac_grr_sig_t *grr_msg_ptr,
                                    gas_id_t       gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_get_rr_gas_id
===
===  DESCRIPTION
===  Returns the gas_id from the incoming RR msg supplied.
===
===  INPUT
===  grr_mac_sig_t
===
===  DEPENDENCIES
===  None
===
===  RETURN VALUE
===  gas_id_t
===
===  SIDE EFFECTS
===  None
===
===========================================================================*/
gas_id_t gmac_get_rr_gas_id( grr_mac_sig_t *grr_msg_ptr);

/*===========================================================================
===
===  FUNCTION      gmac_get_l1_gas_id
===
===  DESCRIPTION
===  Returns the gas_id from the incoming L1 msg supplied.
===
===  INPUT
===  l1_mac_sig_t
===
===  DEPENDENCIES
===  None
===
===  RETURN VALUE
===  gas_id_t
===
===  SIDE EFFECTS
===  None
===
===========================================================================*/
gas_id_t gmac_get_l1_gas_id(l1_mac_sig_t *l1_msg_ptr );

/*===========================================================================
===
===  FUNCTION      gmac_l1_mac_insert_gas_id
===
===  DESCRIPTION
===  Insert the active gas id in to a MAC->L1 message.
===
===  INPUT
===  Pointer to L1 message structure of the message to be sent.
===  gas_id
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_l1_mac_insert_gas_id(mac_l1_sig_t *common_msg_ptr,
                                      gas_id_t      gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_get_rr_gas_id
===
===  DESCRIPTION
===  Returns the gas_id from the incoming RLC msg supplied.
===
===  INPUT
===  rlc_mac_msg_t
===
===  DEPENDENCIES
===  None
===
===  RETURN VALUE
===  gas_id_t
===
===  SIDE EFFECTS
===  None
===
===========================================================================*/
gas_id_t gmac_get_rlc_gas_id( rlc_mac_msg_t *rlc_msg_ptr );

/*===========================================================================
===
===  FUNCTION      gmac_rlc_mac_insert_gas_id
===
===  DESCRIPTION
===  Insert the gas id in to a MAC->RLC message.
===
===  INPUT
===  Pointer to RLC message structure of the message to be sent.
===  gas_id
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_rlc_mac_insert_gas_id(mac_rlc_msg_t *rlc_msg_ptr,
                                gas_id_t       gas_id);

#endif /* INC_GMACDS_H */
/*** EOF: don't remove! ***/
