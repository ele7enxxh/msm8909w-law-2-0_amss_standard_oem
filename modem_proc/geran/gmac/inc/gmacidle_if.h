/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC IDLE INTERFACE MODE MODULE
***
***
*** DESCRIPTION
***
***	 This is the public interface to all MAC entities of the IDLE mode functions.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/inc/gmacidle_if.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 18/09/07   rmn	   Created
***
*****************************************************************************/

#ifndef INC_GMACIDLE_IF_H
#define INC_GMACIDLE_IF_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */


/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

typedef enum
   {
    PASSIVE,
    UL_PACKET_ACCESS,
    UL_ACCESS_QUEUED,
    UL_ACCESS_REJECT,
    UL_1P_ASS,
    UL_2P_ASS,
    UL_1P_CONTENTION_RES,
    DL_ASS
   }gprs_mac_idle_substates_t;

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION    gmac_idle_get_idle_state() 
===
===  DESCRIPTION
===  This is an access function for external entities to get the mac_idle_substate.
===
===  PARAMS
===   
===
===  RETURN VALUE
===  gprs_mac_idle_substates_t
===    
===  SIDE EFFECTS
=== 
=== 
===========================================================================*/
gprs_mac_idle_substates_t gmac_idle_get_idle_state(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_idle_check_pcco_addressing() 
===
===  DESCRIPTION
===
===  The function check the Packet Cell Change Order is address the mobile correctly.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
=== 
===========================================================================*/
boolean gmac_idle_check_pcco_addressing(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_init_idle_flags() 
===
===  DESCRIPTION
===  Initialises the idle flags, should only be called once at power on.
===
===  PARAMS
===   
===
===  RETURN VALUE
===
===    
===  SIDE EFFECTS
=== 
=== 
===========================================================================*/
void gmac_init_idle_flags( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION    gmac_init_idle_data() 
===
===  DESCRIPTION
===  Initialises the idle data, used for lower layer resets.
===
===  PARAMS
===   
===
===  RETURN VALUE
===
===    
===  SIDE EFFECTS
=== 
=== 
===========================================================================*/
void gmac_init_idle_data( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION    gmac_init_idle_util_data() 
===
===  DESCRIPTION
===  Initialises the idle util data, used for lower layer resets.
===
===  PARAMS
===   
===
===  RETURN VALUE
===
===    
===  SIDE EFFECTS
=== 
=== 
===========================================================================*/
void gmac_init_idle_util_data( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      idle_mode_handler() 
===
===  DESCRIPTION
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void idle_mode_handler(uint8 task_id, uint8 signal_id, gas_id_t gas_id );
/*===========================================================================
===
===  FUNCTION    gmac_idle_tqi_address_match() 
===
===  DESCRIPTION
===  Performs a TQI address check.  Note that TQI is only
===  valid in UL_ACCESS_QUEUED state.
===
===  PARAMS
===    msg_tqi - tqi to be checked
===
===
===  RETURN VALUE
===    boolean - TRUE - TQI passes address check, FALSE TQI fails address check
===    
===  SIDE EFFECTS
===    Reads:    tqi
===              valid_tqi
===              mac_mode
===              mac_idle_substates
===               
=== 
===========================================================================*/
boolean gmac_idle_tqi_address_match(uint16 msg_tqi, gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    idle_ul_realloc_in_progress() 
===
===  DESCRIPTION
===
===  Check whether UL resource reallocation has been initiated in Idle mode
===  while in contention resolution state 
===
===  PARAMS
===
===  RETURN VALUE
===    boolean - TRUE  - Reallocation is ongoing             
===    
===  SIDE EFFECTS               
=== 
===========================================================================*/
boolean idle_ul_realloc_in_progress(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_init_idle_data_non_zero()
===
===  DESCRIPTION
===  Initialises non zero (including all enums) idle data..
===
===  PARAMS
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_idle_data_non_zero( gas_id_t gas_id );

#endif /* INC_GMACIDLE_IF_H */
