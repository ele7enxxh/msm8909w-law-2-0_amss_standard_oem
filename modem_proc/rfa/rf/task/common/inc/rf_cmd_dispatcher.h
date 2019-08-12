#ifndef RF_CMD_DISPATCHER_H
#define RF_CMD_DISPATCHER_H

/*!
  @file
  RF Task command dispatcher interface file.
*/

/*==============================================================================

  Copyright (c) 2009 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/common/inc/rf_cmd_dispatcher.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
08/24/14   kb      Fix compiler warning
10/05/12   av      Expanded rf task architecture to include more tasks. 
03/13/12   aro     Added voting mechanism to ensure that all modules and
                   techs are de-registered before de-allocating the dynamically
                   allocated memeory for Tech and module tables
03/13/12   aro     Add feature to dynamically grow the tech table based on
                   maximum number of Tech IDs
03/13/12   aro     Updated dispatch handler function to take only command
                   pointer as argument.
03/12/12   aro     Removed field to indicate, if a tech needs to be registrated
                   for module-based dispatch. This will now acheived by
                   using invalid module number.
03/12/12   aro     Removed registration_done flag from tech and module
                   table to use NULL pointer check to find if the registration
                   is done or not.
03/12/12   aro     Added tech_id and module_id in the new dispatch tabale to
                   assist debugging
03/09/12   aro     Added tech-based and module-based dispatching
                   mechanism
03/09/12   aro     Interface to perform Tech based deregistration
03/09/12   aro     Interface to perform Tech based registration
03/09/12   aro     Flag to check if the tech registration is done
03/09/12   aro     Added defintion for Tech callback handler function and
                   tech callback functions
03/09/12   aro     Doxygen Documentation Update
05/23/11   bmg     Mainlined FEATURE_QTF_OFFTARGET
09/17/09   mkv     Updating the command handler signature from void* 
                   to rf_cmd_type*
08/26/09   mkv     Integrating REX MSGR Interworking into RFA2. 
                   Passing the entire cmd to the cmd dispatcher instead of just 
                   the payload. MSGR Dispatchers need the entire cmd.  
07/21/09   mkv     Deregister RF Module (free rf_cmd_dispatch_tbl).    
06/21/09   ans     Initial Release.

==============================================================================*/

#include "comdef.h"
#include "rf_cmd_interface.h"
#include "rf_task_common_services.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The command size for the data associated with the command. 

  @details 
  The command size foreach RF command. This command size should be less
  than RF_MAX_CMD_PAYLOAD_SIZE.
*/
typedef uint32 rf_cmd_size_type ;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The command dispatcher status type. 
*/
typedef enum
{
  RF_CMD_REGISTRATION_SUCCESS,
   /*!< Command registered successfully. */

  RF_CMD_REGISTRATION_FAILURE 
  /*!< Command not registered. It will not be dispatched. */
} rf_cmd_dispatch_status_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer for the handler function associated with each command.

  @details
  The function pointer prototype associated with each command.

  @param module_id
  The module for which dispatcher is called.  

  @param cmd
  The command for which dispatcher is called.  

  @param cmd_payload_size
  The size of the message associated with the command.  

  @param payload
  The data associated with the command. The size of data is specified in
  payload_size.  
*/
typedef void (*rf_cmd_handler_type)( rf_module_id_type module_id, 
                                     rf_cmd_id_type cmd, 
                                     rf_cmd_size_type cmd_payload_size, 
                                     rf_cmd_type* cmd_ptr );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer for the handler function associated with each command.

  @details
  The function pointer prototype associated with each command.

  @param module_id
  The module for which dispatcher is called.  

  @param cmd
  The command for which dispatcher is called.  

  @param cmd_payload_size
  The size of the message associated with the command.  

  @param payload
  The data associated with the command. The size of data is specified in
  payload_size.
*/
typedef void (*rf_cmd_handler_func_type)( rf_cmd_type* cmd_ptr );

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure to hold callabck functions for module based command
  dispatching. Techs refers to the 8-bit technology field [Bit 31:24] in UMID 
  message, whereas Module refers to 8-bit module field [Bit 23:16] in UMID 
  message
*/
typedef struct
{
  uint8 module_id;
  /*!< Field to store the module_id */

  rf_cmd_handler_func_type module_based_cb;
  /*!< Callback function pertaining to a module based dispatching */

} rf_cmd_module_table_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure to hold Tech specific callabck functions.
*/
typedef struct
{
  uint8 tech_id;
  /*!< Field to store the tech_id */

  rf_cmd_handler_func_type tech_based_cb;
  /*!< Callback function pertaining to a tech based dispatching from UMID. This 
  callback function will be executed, only if there is no module registration 
  for this tech (i.e. there is no module-based dispatching ). Module 
  registration for the tech will be checked by looking at the module_table 
  pointer.*/

  rf_cmd_module_table_type *module_table;
  /*!< Pointer to module table pertaining to the tech. The memory for this 
  field will dynamically allocated when a registration is requested for 
  each Tech and Module together. */ 

  uint64 module_063_000_reg_mask;
  /*!< Mask to keep track of registration of Modules IDs 0 through 63 */

} rf_cmd_tech_table_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure to hold al  Tech specific callabck functions. Techs refers to the 
  8-bit technology field in UMID message.
*/
typedef struct
{
  boolean first_registration;
  /*!< Flag indicating, if first registration is done or not. */

  uint8 max_tech_id;
  /*!< Variable to keep track of maximum tech ID number which has requested 
  registration till now. This will be used to alloc/realloc memory pointed 
  by tech_data */ 

  rf_cmd_tech_table_type *tech_data;
  /*!< 
  @brief
  Pointer to the memeory whoch hold tech data for several tech. 
   
  @details 
  This chunk of memory grows dynamically based on the maximum tech id that 
  requests registration with RF_TASK. For instance, assume that following 
  tech_ids request for registration is particular order. 
  -# Tech ID 5 
  -# Tech ID 3 
  -# Tech ID 8. 
   
  In the above sequence, when Tech ID 5 requests registration, tech_data will 
  be allocated with the size of 5 * sizeof(rf_cmd_tech_data_type). For the 
  second registraiton, since 3 is less than 5, no re-allocation is done. 
  However, when Tech ID 8 requests registration, re-allocation will be 
  done with new size of  8 * sizeof(rf_cmd_tech_data_type).   
  */ 

  uint64 tech_063_000_reg_mask;
  /*!< Mask to keep track of registration of Tech IDs 0 through 63 */

  uint64 tech_127_064_reg_mask;
  /*!< Mask to keep track of registration of Tech IDs 127 through 64 */

  uint64 tech_191_128_reg_mask;
  /*!< Mask to keep track of registration of Tech IDs 191 through 128 */

  uint64 tech_255_192_reg_mask;
  /*!< Mask to keep track of registration of Tech IDs 255 through 192 */

} rf_cmd_dispatch_table_type;

/*----------------------------------------------------------------------------*/
void
rf_cmd_dispatch_init 
(
  rf_task_num_type task_num,
  rf_module_id_type num_of_module 
);

/*----------------------------------------------------------------------------*/
void
rf_cmd_dispatch_deinit
( 
  rf_task_num_type task_num
);

/*----------------------------------------------------------------------------*/
rf_cmd_dispatch_status_type
rf_cmd_dispatch_register_rf_module 
(
  rf_task_num_type task_num,
  rf_module_id_type module_id,
  rf_cmd_id_type rf_num_of_cmd,
  rf_cmd_id_type min_cmd_id,
  rf_cmd_id_type max_cmd_id 
);

/*----------------------------------------------------------------------------*/
void
rf_cmd_dispatch_deregister_rf_module
(
  rf_task_num_type task_num,
  rf_module_id_type module_id
);

/*----------------------------------------------------------------------------*/
rf_cmd_dispatch_status_type
rf_cmd_dispatch_register_handler 
(
  rf_task_num_type task_num,
  rf_module_id_type module_id,
  rf_cmd_id_type cmd_id,
  rf_cmd_handler_type cmd_handler,
  rf_cmd_size_type payload_size
);

/*----------------------------------------------------------------------------*/
rf_cmd_dispatch_status_type
rf_cmd_dispatch_register_tech
(
  rf_task_num_type task_num,
  uint16 umid_tech,
  uint32 umid_module,
  rf_cmd_handler_func_type tech_callback
);

/*----------------------------------------------------------------------------*/
void
rf_cmd_dispatch_deregister_tech
(
  rf_task_num_type task_num,
  uint16 umid_tech,
  uint32 umid_module
);

/*----------------------------------------------------------------------------*/
void
rf_cmd_process
(
  rf_task_num_type task_num
);

#endif /*  RF_CMD_DISPATCHER_H */

