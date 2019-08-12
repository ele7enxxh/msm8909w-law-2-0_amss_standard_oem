/*****************************************************************************
***
*** TITLE
***
*** GPRS MAC-MAC ACCESS FUNCTION 
***
***
*** DESCRIPTION
***
***  This module provides the access function implementations to support the
***  MAC to MAC signaling.
***
*** 
*** EXTERNALIZED FUNCTIONS
***
***	 gprs_mac_mac_acc_func()
*** 
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***	
***  none
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/src/gmacmac.c#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 06/25/01   tlx	   Created
***
*****************************************************************************/

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "geran_tasks.h"
#include "string.h"
#include "geran_msgs.h"
#include "gprs_mem.h"
#include "task.h"
#include "grlcutil.h"
#include "gmacdef.h"
#include "gmactransfer_if.h"
#include "gmacmac.h"
#include "gmactask.h"
#include "gmacproc.h"
#include "gmacds.h"

/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      gprs_mac_mac_acc_func
===
===  DESCRIPTION
===  MAC access function. Used by MAC to send messages to itself.
===
===  The function makes a copy of the message structure and places it
===  in its own message queue.
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
void gprs_mac_mac_acc_func(mac_int_sig_t *common_msg_ptr,
                           gas_id_t gas_id)
{
  /* pointer to msg */
  mac_int_sig_t *local_msg_ptr = NULL;
  geran_task_id_t task_id = GERAN_TASK_ID_GMAC;
  gmac_global_data_t *global_data_ptr = NULL;

  task_id = gmac_map_gas_id_to_task_id(gas_id);
  global_data_ptr = gmac_get_global_data_ptr(gas_id);
  common_msg_ptr->gas_id = gas_id;
  
  /* get buffer memory for the command */
  local_msg_ptr = GPRS_MEM_MALLOC(sizeof(mac_int_sig_t) / sizeof(uint8));
  
  if (local_msg_ptr != NULL)
  {
    gmac_store_trace_send_msg_buf(gas_id, MS_MAC_MAC,common_msg_ptr);
    /* copy param to command buffer */
     *local_msg_ptr = *common_msg_ptr;  
       
    /* init q item by pointing to data area */
    (void)q_link(local_msg_ptr, &local_msg_ptr->link);
    
    /* add MAC signal queue */
    q_put(&global_data_ptr->mac_q, &local_msg_ptr->link);

    /* Set the command queue signal for receive task */
    (void)rex_set_sigs(geran_tcb_read(task_id), GPRS_MAC_MASTER_SIG);
  }
  else
  {
    ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
  }
}

/*** EOF: don't remove! ***/
