#ifndef ECALL_MODEM_CLIENTS_H_
#define ECALL_MODEM_CLIENTS_H_

/**
  @file ecall_modem_clients.h
  @brief This header file supports multiple clients getting notification for IVS status
    through registered callbacks.
*/
/*===========================================================================

                  E C A L L   E M O D E M   C L I E N T S. H

DESCRIPTION
    This supports the multiple clients to get notification for ivs status
    through registered callback

REFERENCES
    none

EXTERNALIZED FUNCTIONS
   -ecall_emodem_client_init() 
   -ecall_emodem_client_register()
   -ecall_emodem_client_deregister()

INITIALIZATION AND SEQUENCING REQUIREMENTS
  
Copyright (c) 2010, 2011 Qualcomm Technologies Incorporated.


All rights reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodem/inc/ecall_modem_clients.h#1 $ 
  $DateTime: 2016/12/13 08:00:05 $ 
  $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/29/11    CM      (Tech Pubs) Edited/added Doxygen comments and markup.
06/25/10    sar     Supports multiple clients for IVS

===========================================================================*/

#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_ECALL_IVS

#include "ecall_modem_apis.h"
#include "queue.h"

/** @addtogroup ecall_modem_clients_data_types  
@{ */

/** Client API return status. */
typedef enum ecall_emodem_client_status_e {

  ECALL_EMODEM_CLIENT_ERR = -1,
  /**< Error occurred.  */

  ECALL_EMODEM_CLIENT_OK,
  /**< Operation completed successfully. */

} ecall_emodem_client_status_type;

/*--------------------------------------------------------------------------*/
/*          modem client structure to register for notification             */
/*--------------------------------------------------------------------------*/
/** @brief Defines the eCall modem client. This function is used by the 
ecall_emodem_client_register() function.*/
typedef struct {
  q_link_type  link;        
  /**< Each registered client is added to the queue. */

  int16        clientid;   
  /**< ID is allocated upon calling ecall_emodem_client_init(). */

  ecall_cb_func_type    cb_func; 
  /**< Client callback function to be registered for receiving modem notifications. */

  void                 *user_data; /**< Optional data field. */
} ecall_client_type;

/** @} */ /* end_addtogroup ecall_modem_clients_data_types */
/*===========================================================================
                     INCLUDE FILES FOR EXTERNAL MODULE
===========================================================================*/


/*===========================================================================
FUNCTION      ECALL_EMODEM_CLIENT_INIT
===========================================================================*/
/** @ingroup func_emodem_client_init
  The client gets the client ID on initialization, which is used during registration.
  If free slots are available, ECALL_EMODEM_CLIENT_OK is returned. If there is a 
  failure with the client ID -1, ECALL_EMODEM_CLIENT_ERR is returned.

  @param[in] client_id_ptr    Clients registering for notifications
  need a client ID, which is allocated by the eCall modem in this
  function call.

  @return 
  #ecall_emodem_client_status_e

  @dependencies
  None.

  */

ecall_emodem_client_status_type ecall_emodem_client_init
(
  int16   *client_id_ptr
);


/*===========================================================================
FUNCTION     ECALL_EMODEM_CLIENT_REGISTER
===========================================================================*/
/** @ingroup func_emodem_client_register
   Registers the client for IVS notifications.

  @param[in] client_struct  Structure must contain the client ID
                            and the callback function to be called that needs 
							to be called for notification of eCall modem                            
                            status.
 
  @return 
  #ecall_emodem_client_status_e

  @dependencies
  ecall_emodem_client_init() must be called before calling the register.

  @structure
  #ecall_client_type

*/
ecall_emodem_client_status_type ecall_emodem_client_register
(
  ecall_client_type    *client_struct
);


/*===========================================================================
FUNCTION     ECALL_EMODEM_CLIENT_DEREGISTER
===========================================================================*/
/** @ingroup func_emodem_client_deregister
   Deregisters the client. It cannot get further notification from the IVS.
  
  @param[in] client_id   Client ID that needs to be deregistered.
 
  @return
  ECALL_EMODEM_CLIENT_OK -- Operation was successful.\n
  ECALL_EMODEM_CLIENT_ERR -- Failure occurred.

  @dependencies
  None.

*/

ecall_emodem_client_status_type ecall_emodem_client_deregister
(
  int16 client_id
);

#endif /* FEATURE_ECALL_IVS */

#endif



