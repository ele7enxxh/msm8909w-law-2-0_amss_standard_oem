#ifndef ECALL_MODEM_APIS_H
#define ECALL_MODEM_APIS_H

/**
  @file ecall_modem_apis.h
  @brief This file contains the internal APIs between the eCall application and the modem.
*/

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         E C A L L   M O D E M   A P I S
                            H E A D E R   F I L E

GENERAL DESCRIPTION 
This file contains the internal APIs between the eCall application and the modem.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  
Copyright (c) 2009, 2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodem/inc/ecall_modem_apis.h#1 $ 
  $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/29/11   CM      (Tech Pubs) Edited/added Doxygen comments and markup.
09/20/09   PA      Initial version.

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "stdio.h"
#include "sys.h"

/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/
/** @addtogroup ecall_modem_apis_constants 
@{ */

/** Length of the MSD message in bytes.   */
#define ECALL_MSD_MAX_LENGTH    (140)    
/** @} */ /* end_addtogroup ecall_modem_apis_constants */


/** @addtogroup ecall_modem_apis_data_types  
@{ */
/** The application can expect these callbacks and take the appropriate action,       
        once it registers the callback in start_transmit_data. The eCall services
		notify the client when events associated with this transaction occur by 
		calling the function to which client_func is pointing. See ecall_tx_status
        for the status descriptions.  */
typedef enum ecall_tx_status  {

  ECALL_IVS_SF_RECEIVED,                 
    /**< IVS has received the first synchronization frame (SF) sequence. */

  ECALL_IVS_START_RECEIVED_NO_MSD,       
    /**< Third consecutive Start message is received by the 
		 IVS and there is no MSD at the modem task.*/

  ECALL_IVS_START_RECEIVED_MSD,          
  /**< Start message is received by the IVS and there is an MSD at the modem task. */

  ECALL_IVS_MSD_SENT,                    
  /**<  IVS finished sending the MSD frame sequence of one redundancy version (RV). */

  ECALL_SESSION_COMPLETED,               
  /**< IVS successfully completed the MSD transmission and received two AL-ACKs (positive). 
       Can now switch to voice path. */

  ECALL_SESSION_RESET,                   
  /**< IVS lost synchronization and cannot decode the SYNC frame as usual; 
       it starts over and waits for a start message after sending a RESET. */

  ECALL_IVS_MSD_FAST_MODE,               
  /**< Fast modulation selected for the MSD transmission. */

  ECALL_IVS_MSD_ROBUST_MODE,             
  /**< Robust modulation selected for the MSD transmission. */

  ECALL_IVS_ACK_RECEIVED,                
  /**< ACK message is received. */

  ECALL_IVS_NACK_RECEIVED,               
  /**< NACK message is received. */

  ECALL_IVS_ERR,                         
  /**< eCall modem error occurs at the modem task. */

  ECALL_IVS_TX_COMPLETED,                
  /**< MSD transmission is complete. */

  ECALL_IVS_HLACK_RECEIVED,              
  /**< HLACK message is received. */

  ECALL_IVS_CLEARDOWN_RECEIVED,           
  /**< IVS received two AL_ACKs with a cleardown indication. */

  ECALL_IVS_LOSE_SYNC
  /**< IVS received three bad syncs and lose sync. */
} ecall_tx_status_cb_type;

/** Transmission mode of operation.  */
typedef enum {
  PULL_MODE = 0,    /**< Pull mode waits for START from the PSAP. */
  PUSH_MODE         /**< Push mode IVS sends an Initiation message. */
} ecall_tx_mode_type;   /**< Transmission mode. */

/** Operating mode of the modem. See ecall_operating_mode for details. */
typedef enum ecall_operating_mode
{
  ECALL_MODE_UNKNOWN = -1,  /**< Unknown mode is reported. 
                                 Default is normal mode. */
  ECALL_MODE_NORMAL = 0,    /**< eCall enabled normal mode. */
  ECALL_MODE_ECALL_ONLY,    /**< eCall-only mode.           */
  ECALL_MODE_MAX                  
} ecall_operating_mode_type;    


/**
@brief 
Contains the IVS status and the HLACK data when HLACK notification 
is received. This function is used by the ecall_cb_func_type() function.
*/
typedef struct
{
  ecall_tx_status_cb_type  tx_status;   /**< Transmission status. */
  int16 hlack_data;                     /**< Received hlack data from the PSAP. */
}ecall_cb_data_type;

/** @} */ /* end_addtogroup ecall_modem_apis_data_types  */


/*--------------------------------------------------------------------------*/
/*                       Callback function type                             */
/*--------------------------------------------------------------------------*/
/**@ingroup func_cb_func_type  
  eCall modem notifies all the registered clients about the MSD
  transmission status.

  @param[in] user_data    Optional data field inputted during
                          client registration for notifications.
  @param[in] ivs_status   Modem transmission status.
  
  @return 
  None.

  @dependencies
  None.

  @structure
  #ecall_cb_data_type

*/

typedef void (*ecall_cb_func_type) (
  void               *user_data,
  ecall_cb_data_type ivs_status
);


/** @ingroup func_operating_mode_cb_func_type
  Callback function type. This function defines the eCall operating mode.

  @datatypes
  #ecall_operating_mode_type

  @param[in] *user_data                    Optional data field.
  @param[in] ecall_operating_mode_status   Operating mode of the modem.

  @return
  None.

  @dependencies
  None.
*/
typedef void (*ecall_operating_mode_cb_func_type) (
  void             *user_data,
  ecall_operating_mode_type  ecall_operating_mode_status
);

/** @addtogroup ecall_modem_apis_data_types 
@{ */
/**  MSD type expected by the modem.   */

typedef uint8 ecall_msd_array_type[ECALL_MSD_MAX_LENGTH];

/** @} */ /* end_addtogroup ecall_modem_apis_data_types */

/*===========================================================================
                          EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*==========================================================================
 FUNCTION ECALL_START_TRANSMIT_DATA
===========================================================================*/
/** @ingroup func_start_transmit_data
  Called from the eCall IVS application, this function triggers the modem to send 
  the MSD and register with vocoder to listen for DL data frames from PSAP. 
  It registers with the Tx vocoder after receiving the START triggers.

  @datatypes
  #ecall_msd_array_type \n
  #ecall_tx_mode_type

  @param[in] ecall_msd   MSD sent to the Public Safety Answering Point (PSAP).         
  @param[in] tx_mode     Indicates whether the modem will function in Pull or 
                         Push mode (Push mode should send the initiation signal 
                         message to the PSAP): 
						 - 1 -- Signifies Push mode; send initiation signal 
                         messages to the PSAP
						 - 0 -- Signifies Pull mode

  @return 
  None.

  @dependencies
  None.

*/
void ecall_start_transmit_data
(
  const ecall_msd_array_type ecall_msd, 
  ecall_tx_mode_type  tx_mode
);

/*===========================================================================
FUNCTION ECALL_UPDATE_CM_SUBS_INFO
===========================================================================*/
void ecall_update_cm_subs_info
(
  void
);

/*===========================================================================
FUNCTION ECALL_OPERATING_MODE_REG
===========================================================================*/
/** @ingroup func_operating_mode_reg
   Using this function, the application can register to get the status of the eCall 
   modem configuration (ECALL ONLY or ECALL NORMAL) through the callback passed 
   in this function.

  @param[in] operating_mode_cb_func   Register for function callback to receive modem
                                      operation mode.
  @param[in] operating_mode_cb_data   Optional data.

  @return 
  None.

  @dependencies
  None.

*/
void ecall_operating_mode_reg
(
  ecall_operating_mode_cb_func_type  operating_mode_cb_func,
  void *operating_mode_cb_data
);


/*==========================================================================
  FUNCTION ECALL_ABORT_TRANSMIT_DATA
===========================================================================*/
/** @ingroup func_abort_transmit_data
    This function is called from the eCall IVS application. This aborts the 
    current transmission of the MSD and gives the vocoder to the network for 
    voice connection. Its application responsibility is to indicate modem, 
    either discarding the current MSD or saving it for later transmission.

    @param[in] discard_msd   If TRUE, the eCall modem discards the MSD and requests
                             new GPS data in case of an additional data request                                                                  
                             from the PSAP. If FALSE, the eCall modem does not 
                             discard the data and saves it to send during the next
                             transmission.

    @return
    None.

	@dependencies
    None.

    @sideeffects
    The function still monitors for the incoming DL frames.
*/

void ecall_abort_transmit_data
(
  boolean discard_msd
);


/*==========================================================================
  FUNCTION ECALL_ABORT_ALL
===========================================================================*/
/**
    Called from the eCall IVS application, this function triggers the modem to stop
	receiving data and stop the MSD transmission. It relieves the PCM interface, 
	unmutes the voice call, and deregisters vocoder input and output. No further
	status is sent to the application. 
  
   @return
    None.

   @dependencies
    None.

   @sideeffects
    This stops monitoring for the incoming DL frames and discards the MSD.
*/

void ecall_abort_all
(
  void
);

/*==========================================================================
  FUNCTION ECALL_GET_ECALL_OPERATING_MODE
===========================================================================*/
/** @ingroup func_get_ecall_operating_mode
    This function reads the service bit 89 from the SIM to find out if the 
    eCall modem is ECALL ONLY or ECALL NORMAL. This information is then sent
    to the network access server for the EIP procedure, and sent to the eCall application in the 
    form of a callback, so the application is aware of the eCall modem configuration 
    state.

   @return
    None.

   @dependencies
    None.
*/

ecall_operating_mode_type ecall_get_ecall_operating_mode
( 
  void
);

/*==========================================================================
  FUNCTION ECALL_GET_ECALL_OPERATING_MODE_PER_SUBS
===========================================================================*/
/** @ingroup func_get_ecall_operating_mode_per_subs
    This function reads the service bit 89 from the SIM to find out if the 
    eCall modem is ECALL ONLY or ECALL NORMAL with the Modem active SUBS ID (as_id).
    This information will then sent to the network access server for the EIP procedure,
    and sent to the eCall application in the form of a callback, so the application is
    aware of the eCall modem configuration state.

   @param[in] as_id   Modem Active Subscription IDs

   @return
    None.

   @dependencies
    None.
*/

ecall_operating_mode_type ecall_get_ecall_operating_mode_per_subs
( 
  sys_modem_as_id_e_type as_id
);

#endif /* ECALL_MODEM_APIS_H */

