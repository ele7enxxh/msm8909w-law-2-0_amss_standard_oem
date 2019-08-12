/*! 
  @file
  rf_cmd_dispatcher.c
 
  @brief
  RF Task dispatcher interface file.

  @details
  This file defines the Common CDMA Main Control interfaces.
 
  @addtogroup RF_COMMON_TASK_RFA_DISPATCHER
  @{
*/

/*==============================================================================

  Copyright (c) 2009 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/common/src/rf_cmd_dispatcher.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/20/15   vr      Autopin check-in
11/16/15   ag      Added Autopin cmd handle for WCDMA for msg support
11/05/15   whz     Added cmd handle for RFLM_CMN_AUTOPIN_RSP
06/05/15   whz     Add AutoPin support for TDS
03/17/15   whz     Added cmd handle for TDSCDMA Therm trigger
03/12/15   gh      Added cmd handle for RFLM_WCDMA_DO_THERM_READ_CMD
10/14/14   kb      Fix compiler warning
08/24/14   kb      Fix compiler warning
07/16/14   kb      Add support to handle TDSCDMA UMID using RF_DISPATCHER
05/15/14   aro     Added workaround to handle WCDMA UMID using RF_DISPATCHER
04/05/13   bmg     Error in dispatch cleanup when last de-registering tech had
                   sub-modules, didn't clean up task data.
                   Reduced unnecessary re-alloc overhead.
11/30/12   aki	   Added NULL check to rf_cmd_dispatch_deregister_tech,
                   cleaning dispatch registrations as part of rf_cmd_dispatch_deinit
11/28/12   bmg     Fixed missed tech registration and refactored tech bitmask
                   code to extract common methods.
11/08/12   jfc     Set pointer to freed memory to NULL 
10/23/12   av      Bug fixes in the new change
10/05/12   av      Expanded rf task architecture to include more tasks. 
09/28/12   aro     F3 Update
03/22/12   aro     Mainlined new Dispatch for RF_TASK
03/20/12   aro     Re-organized Task module and added FWRSP task
03/15/12   aro     Added illustration for new dispatcher design
03/15/12   aro     Added F3 message to assist debugging of tech-table 
                   dynamic growth
03/14/12   aro     Memset the dynamically allocated memory
03/14/12   aro     Fixed the first_registration issue
03/13/12   aro     Added voting mechanism to ensure that all modules and
                   techs are de-registered before de-allocating the dynamically
                   allocated memeory for Tech and module tables
03/13/12   aro     Add feature to dynamically grow the tech table based on
                   maximum number of Tech IDs
03/13/12   aro     Removed un-necessary F3 message
03/13/12   aro     Added support to enable new RF_TASK dispatching. Code
                   is protected with debug flag
03/12/12   aro     Tech Id Update during module registration
03/12/12   aro     Added commented F3 message for on-target registration
                   testing
03/12/12   aro     Removed field to indicate, if a tech needs to be registrated
                   for module-based dispatch. This will now acheived by
                   using invalid module number.
03/12/12   aro     Removed registration_done flag from tech and module
                   table to use NULL pointer check to find if the registration
                   is done or not.
03/12/12   aro     Added tech_id and module_id in the new dispatch tabale to
                   assist debugging
03/09/12   aro     Removed white spaces
03/09/12   aro     Added tech-based and module-based dispatching
                   mechanism
03/09/12   aro     Interface to perform Tech based deregistration
03/09/12   aro     Interface to perform Tech based registration
03/09/12   aro     Added doxygen block to partition functions
03/09/12   aro     Code beautification 
03/09/12   aro     Added doxygen documentation 
06/14/11   sty     Changed message level
05/23/11   bmg     Mainlined FEATURE_QTF_OFFTARGET
08/19/10   mkv     Fixing Klockwork - potential NULL pointer deference. 
13/01/09   pl      update due to rf_cmd_type structure change
10/26/09   gy      Generalized module id to non-zero case and fixed a
                   statement of module id check.
09/11/09   tws     Add support for sending a signal back to calling task when
                   execution complete.
08/26/09   mkv     Integrating REX MSGR Interworking into RFA2. 
                   Passing the entire cmd to the cmd dispatcher instead of just 
                   the payload. MSGR Dispatchers need the entire cmd. 
07/21/09   mkv     Deregister RF Module (free rf_cmd_dispatch_tbl). 
07/10/09   ans     Initial Release.

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"

#ifdef FEATURE_LTE
#include "rf_cap.h" 
#endif /*FEATURE_LTE*/

/* Feature RF Task should be used to encompass complete RF task related file.
This is not meant to be used for other featurization. This is meant just to 
provide compatibility with top leavel feature used in TMC and Task modules. */
#ifdef FEATURE_RF_TASK
#include "msg.h"
#include "err.h"
#include "rf_cmd_dispatcher.h"
#include "rfcom.h"
#include "rfcommon_msg.h"
#include "rfa_msgr.h"
#include "modem_mem.h"
#include "rfm_internal.h"
#include "rf_dispatch.h"
#include "rf_task.h"

#ifdef FEATURE_WCDMA
#include "rflm_wcdma_msg.h"
#endif

#ifdef FEATURE_TDSCDMA
#include "rflm_tds_msg.h"
#endif

#ifdef FEATURE_CDMA
#include "rflm_c2k_msg.h"
#endif

#include "rflm_cmn_msg.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure for command dispatcher table entry.

  @details
  Structure for command dispatcher table entry.
  Stores the command ID, function pointer to handler and 
  payload size.
*/
typedef struct
{
  rf_cmd_id_type cmd_id;
  /*!< Command ID for the command. This is same as index of the table. 
  Stored for validation or hash table if needed. */

  rf_cmd_handler_type cmd_handler;
  /*!< function pointer to the handler function */

  rf_cmd_size_type payload_size;
  /*!< Command palyoad size - stored for validation */

} rf_cmd_dispatch_tbl_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure for module dispatch table entry.

  @details
  Structure for module dispatch table entry.
  Stores the associated task, module ID, total number of commands for that
  module, minimum and maximum command ID and table pointer to command
  dispatcher.
*/
typedef struct 
{
  rf_task_num_type task_num;
  /*!< RF Task Number */

  rf_module_id_type module_id;
  /*!< Module ID*/

  rf_cmd_id_type rf_num_of_cmd;
  /*!< Total number of commands for this module */

  rf_cmd_id_type min_cmd_id;
  /*!< Min cmd ID */

  rf_cmd_id_type max_cmd_id; 
  /*!< max cmd ID */

  rf_cmd_dispatch_tbl_type *rf_cmd_dispatch_tbl;
  /*!< Table pointer for command disptcher. Index zero equals 
  min_cmd_id */

} rf_module_dispatch_tbl_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Dispatch table for Modules.

  @details
  This table stores the dispatch information related to the module.
  This is a local array that is dynamically allocated. The index for 
  module number starts with zero and goes on to maximum number of modules 
  supported. It is initialized only once.
*/
static rf_module_dispatch_tbl_type *rf_module_dispatch_tbl[RF_MAX_TASK] = {NULL};

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stores the number of modules that can be registerd with command
  dispatcher.

  @details
  Stores the number of modules that can be registerd with command
  dispatcher. This variable is set at the time of initialization
  of command dispatcher. 
*/
static rf_module_id_type max_num_of_modules[RF_MAX_TASK] = {0};

/*----------------------------------------------------------------------------*/
/*! Maximum Number of techs supported by UMID. Tech is defined by Bit [31:24] 
in a given UMID. Thus the Maximum number of techs would be 2^8 = 256 */
#define RF_NUM_OF_UMID_TECHS 256

/*----------------------------------------------------------------------------*/
/*! Table to store all the callback handler functions for each Tech from 
the UMID */
static rf_cmd_dispatch_table_type rf_tech_dispatch_table[RF_MAX_TASK];

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set a specific tech bit in the command dispatcher tech mask.
 
  @param task_num
  The task for which the tech bit should be set
 
  @param umid_tech
  The tech to add to the bitmask.
*/
static void
rf_cmd_dispatch_set_tech_mask
(
  rf_task_num_type task_num,
  uint8 umid_tech
)
{
  if ( umid_tech <= 63 )
  {
    rf_tech_dispatch_table[task_num].tech_063_000_reg_mask |= (1ULL << (umid_tech-0));
  }
  else if ( umid_tech >= 64 && umid_tech <= 127 )
  {
    rf_tech_dispatch_table[task_num].tech_127_064_reg_mask |= (1ULL << (umid_tech-64));
  }
  else if ( umid_tech >= 128 && umid_tech <= 191 )
  {
    rf_tech_dispatch_table[task_num].tech_191_128_reg_mask |= (1ULL << (umid_tech-128));
  }
  else
  {
    rf_tech_dispatch_table[task_num].tech_255_192_reg_mask |= (1ULL << (umid_tech-192));
  }
} /* rf_cmd_dispatch_set_tech_mask */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Clear a specific tech bit in the command dispatcher tech mask.
 
  @param task_num
  The task for which the tech bit should be cleared
 
  @param umid_tech
  The tech to remove from the bitmask.
*/
static void
rf_cmd_dispatch_clear_tech_mask
(
  rf_task_num_type task_num,
  uint8 umid_tech
)
{
  uint64 new_tech_063_000_reg_mask = 0;
  uint64 new_tech_127_064_reg_mask = 0;
  uint64 new_tech_191_128_reg_mask = 0;
  uint64 new_tech_255_192_reg_mask = 0;

  if ( umid_tech <= 63 )
  {
    new_tech_063_000_reg_mask |= (1ULL << (umid_tech-0) );
    new_tech_063_000_reg_mask = ~new_tech_063_000_reg_mask;
    rf_tech_dispatch_table[task_num].tech_063_000_reg_mask &= 
                                            new_tech_063_000_reg_mask;
  }
  else if ( umid_tech >= 64 && umid_tech <= 127 )
  {
    new_tech_127_064_reg_mask |= (1ULL << (umid_tech-64) );
    new_tech_127_064_reg_mask = ~new_tech_127_064_reg_mask;
    rf_tech_dispatch_table[task_num].tech_127_064_reg_mask &= 
                                            new_tech_127_064_reg_mask;
  }
  else if ( umid_tech >= 128 && umid_tech <= 191 )
  {
    new_tech_191_128_reg_mask |= (1ULL << (umid_tech-128) );
    new_tech_191_128_reg_mask = ~new_tech_191_128_reg_mask;
    rf_tech_dispatch_table[task_num].tech_191_128_reg_mask &= 
                                            new_tech_191_128_reg_mask;
  }
  else
  {
    new_tech_255_192_reg_mask |= (1ULL << (umid_tech-192) );
    new_tech_255_192_reg_mask = ~new_tech_255_192_reg_mask;
    rf_tech_dispatch_table[task_num].tech_255_192_reg_mask &= 
                                            new_tech_255_192_reg_mask;
  }
} /* rf_cmd_dispatch_clear_tech_mask */


/*============================================================================*/
/*!
  @name Dispatch Initialization

  @brief
  This section includes interfaces to initialize or de-initialize the
  dispatcher.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize the RF CMD dispatch module.

  @details
  Initialize the RF CMD dispatch module. Should be done for each task in the
  system.

  @param task_num
  The RF task with which the command is registered.

  @param num_of_module
  The number of modules to register with particular task.  
*/
void
rf_cmd_dispatch_init 
(
  rf_task_num_type task_num,
  rf_module_id_type num_of_module 
)
{
  /* This function should be called only once. 
  Memory should not be allocated multiple times. */
  static boolean called_once[RF_MAX_TASK]= {FALSE};
  int module_id_counter;
  
  if ( called_once[task_num] == TRUE )
  {
    return;
  }
  
  /* Parameter validation */
  if ( task_num >= RF_MAX_TASK )
  {
    ERR_FATAL( "task_num not supported", 0, 0, 0 );
    return;
  }
  
  if ( num_of_module == 0 )
  {
    ERR_FATAL( "Should register at least one module for command "
               "dispatcher", 0, 0, 0 );
    return;
  }
  
  /* Allocate memory based on number of modules */
  rf_module_dispatch_tbl[task_num] = 
            modem_mem_alloc( num_of_module * sizeof(rf_module_dispatch_tbl_type),
                         MODEM_MEM_CLIENT_RFA );

  if ( rf_module_dispatch_tbl[task_num] == NULL )
  {
    ERR_FATAL("Unable to allocate memory for RF command "
              "dispatcher.", 0, 0, 0);
    return;
  }

  memset(rf_module_dispatch_tbl[task_num], 0,
         num_of_module * sizeof(rf_module_dispatch_tbl_type));

  /* Initial value is set to maximum module ID. */
  for ( module_id_counter = 0; module_id_counter < num_of_module;
        module_id_counter++ )
  {
    rf_module_dispatch_tbl[task_num][module_id_counter].module_id = RFM_NUM_MODES;
  }

  max_num_of_modules[task_num] = num_of_module;

  rf_tech_dispatch_table[task_num].first_registration = TRUE;

  called_once[task_num] = TRUE;
  return;
} /* rf_cmd_dispatch_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  De-initialize the RF CMD dispatch module.

  @details
  De-initialize the RF CMD dispatch module. Should be done for each task in
  the system.

  @param task_num
  RF Task number
*/
void
rf_cmd_dispatch_deinit
(
  rf_task_num_type task_num
)
{

  /* Parameter validation */
  if ( task_num >= RF_MAX_TASK )
  {
   ERR_FATAL( "task_num not supported", 0, 0, 0 );

   RF_MSG( RF_HIGH, "task_num not supported" );

   return;
  }

  {
    int module_id_counter ;

    /* Initial value is set to maximum module ID. */
    for( module_id_counter = 0; module_id_counter < max_num_of_modules[task_num];
         module_id_counter++ )
    {
     rf_cmd_dispatch_deregister_rf_module( task_num, module_id_counter);
    }
  }
  rf_tech_dispatch_table[task_num].first_registration = FALSE;

  /* Free the rf_module dispatch tbl memory. */
  modem_mem_free(rf_module_dispatch_tbl[task_num], MODEM_MEM_CLIENT_RFA);
  rf_module_dispatch_tbl[task_num] = NULL;
  
  return;
} /* rf_cmd_dispatch_deinit */

/*! @} */

/*============================================================================*/
/*!
  @name Legacy Module Registration

  @brief
  This section includes dispatcher interface to perform registration using
  legacy way. In this method, each tech will perform registration per
  technology and per tech.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Register the module with RF task.

  @details
  Register the module with RF task. Each module needs to be registered before
  any commands can be dispatched to that module.

  rf_cmd_dispatch_init() should have been called before.

  @param task_num
  The RF task with which the module is registered.

  @param module_id
  The module for which dispatcher is called.  

  @param rf_num_of_cmd
  The number of command associated with the module.  

  @param min_cmd_id
  The minimum number of command id associated with the module.

  @param max_cmd_id
  The minimum number of command id associated with the module.

  @return rf_cmd_dispatch_status_type
  The status of registration.

  @retval RF_CMD_REGISTRATION_SUCCESS
  The registration of the module was successful.

  @retval RF_CMD_REGISTRATION_FAILURE
  The registration of the module was unsuccessful. 
*/
rf_cmd_dispatch_status_type
rf_cmd_dispatch_register_rf_module 
(
  rf_task_num_type task_num,
  rf_module_id_type module_id,
  rf_cmd_id_type rf_num_of_cmd,
  rf_cmd_id_type min_cmd_id,
  rf_cmd_id_type max_cmd_id 
)
{
  /* Parameter validation */
  if ( task_num >= RF_MAX_TASK )
  {
    ERR_FATAL( "task_num not supported", 0, 0, 0 );
    return RF_CMD_REGISTRATION_FAILURE;
  }

  if ( module_id >= max_num_of_modules[task_num] )
  {
    ERR_FATAL("Module with module ID %d not supported ", module_id, 0, 0 );
    return RF_CMD_REGISTRATION_FAILURE;
  }

  if ( rf_module_dispatch_tbl[task_num] == NULL )
  {
    ERR_FATAL("Registration called before dispatch init", 0, 0, 0 );
    return RF_CMD_REGISTRATION_FAILURE;
  }

  if ( max_cmd_id < min_cmd_id )
  {
    ERR_FATAL("Invalid command ID range", 0, 0, 0 );
    return RF_CMD_REGISTRATION_FAILURE;
  }


  /* module_id = RF_NUM_MODES represent no module registered. */
  if ( rf_module_dispatch_tbl[task_num][ module_id ].module_id != RFM_NUM_MODES )
  {
    ERR_FATAL( "Module %d already registerd. Cannot register "
               "module more than once.", module_id, 0, 0 );
    return RF_CMD_REGISTRATION_FAILURE;
  }

  /* For now indexing is done based on command ID. In future one can construct 
  a hash function if command ID is sparse */
  rf_module_dispatch_tbl[task_num][ module_id ].rf_num_of_cmd = 
                             MAX(rf_num_of_cmd, max_cmd_id - min_cmd_id + 1);

  /* Allocate memory based on number of modules */
  rf_module_dispatch_tbl[task_num][ module_id ].rf_cmd_dispatch_tbl = 
     (rf_cmd_dispatch_tbl_type*) modem_mem_alloc( rf_module_dispatch_tbl[task_num][ module_id ].rf_num_of_cmd * sizeof(rf_cmd_dispatch_tbl_type), 
                                                  MODEM_MEM_CLIENT_RFA );

  if ( rf_module_dispatch_tbl[task_num][ module_id ].rf_cmd_dispatch_tbl == NULL )
  {
    ERR_FATAL( "Unable to allocate memory for Module %d registration", 
               module_id, 0, 0 );
    return RF_CMD_REGISTRATION_FAILURE;
  }

  memset( rf_module_dispatch_tbl[task_num][ module_id ].rf_cmd_dispatch_tbl , 0, 
          rf_module_dispatch_tbl[task_num][ module_id ].rf_num_of_cmd * sizeof(rf_cmd_dispatch_tbl_type) );

  rf_module_dispatch_tbl[task_num][ module_id ].task_num = task_num;
  rf_module_dispatch_tbl[task_num][ module_id ].module_id = module_id;
  rf_module_dispatch_tbl[task_num][ module_id ].min_cmd_id = min_cmd_id;
  rf_module_dispatch_tbl[task_num][ module_id ].max_cmd_id = max_cmd_id;

  return RF_CMD_REGISTRATION_SUCCESS;

} /* rf_cmd_dispatch_register_rf_module */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Register the command with RF task.

  @details
  Register the command with RF task. The registration includes handler which
  will be called when command is received.

  This should be called after module is registered using
  rf_cmd_dispatch_register_rf_module().

  @param task_num
  The RF task with which the command is registered.

  @param module_id
  The module for which dispatcher is called.  

  @param cmd_id
  The command for which dispatcher is called.  

  @param cmd_handler
  The function pointer to call when command is received.

  @param payload_size
  The associated payload size for the command.

  @return rf_cmd_dispatch_status_type
  Status of the registration of command.

  @retval RF_CMD_REGISTRATION_SUCCESS
  The registratin of the command was successful.

  @retval RF_CMD_REGISTRATION_FAILURE
  The registratin of the command was unsuccessful. Command will not be
  dispatched if called.
*/
rf_cmd_dispatch_status_type
rf_cmd_dispatch_register_handler 
(
  rf_task_num_type task_num,
  rf_module_id_type module_id,
  rf_cmd_id_type cmd_id,
  rf_cmd_handler_type cmd_handler,
  rf_cmd_size_type payload_size
)
{

  rf_cmd_dispatch_tbl_type *cmd_dispatch_ptr = NULL;

  rf_cmd_id_type cmd_index = 0;

  /* Parameter validation */
  if ( task_num >= RF_MAX_TASK)
  {
    ERR_FATAL( "task_num not supported", 0, 0, 0 );
    return RF_CMD_REGISTRATION_FAILURE;
  }

  /*Ensure that the module_id is valid. If the module_id could be invalid if
  the module id is not registered earlier using 
  rf_cmd_dispatch_register_rf_module or if the module was registered with
  an id that is greater than the maximum number of modules.*/
  if ( module_id == RFM_NUM_MODES || module_id >= max_num_of_modules[task_num] )
  {
    ERR_FATAL( "Module with module ID %d not supported ", module_id, 0, 0 );
    return RF_CMD_REGISTRATION_FAILURE;
  }

  if ( rf_module_dispatch_tbl[task_num] == NULL )
  {
    ERR_FATAL("Registration called before dispatch init", 0, 0, 0 );
    return RF_CMD_REGISTRATION_FAILURE;
  }

  if ( rf_module_dispatch_tbl[task_num][ module_id ].rf_cmd_dispatch_tbl == NULL )
  {
    ERR_FATAL( "Command Registration called before module registreation", 
               0, 0, 0 );
    return RF_CMD_REGISTRATION_FAILURE;
  }

  if ( rf_module_dispatch_tbl[task_num][ module_id ].min_cmd_id > cmd_id  || 
       cmd_id > rf_module_dispatch_tbl[task_num][ module_id ].max_cmd_id )
  {
    ERR_FATAL("Invalid Command ID range", 0, 0, 0 );
    return RF_CMD_REGISTRATION_FAILURE;
  }

  if ( cmd_handler == NULL )
  {
    ERR_FATAL("No handler specified for Command", 0, 0, 0 );
    return RF_CMD_REGISTRATION_FAILURE;
  }

  if ( payload_size > RF_MAX_CMD_PAYLOAD_SIZE )
  {
    ERR_FATAL("Payload size more than permitted", 0, 0, 0 );
    return RF_CMD_REGISTRATION_FAILURE;
  }

  cmd_dispatch_ptr = rf_module_dispatch_tbl[task_num][ module_id ].rf_cmd_dispatch_tbl;

  cmd_index = cmd_id - rf_module_dispatch_tbl[task_num][ module_id ].min_cmd_id;

  cmd_dispatch_ptr[ cmd_index ].cmd_id = cmd_id;
  cmd_dispatch_ptr[ cmd_index ].cmd_handler = cmd_handler;
  cmd_dispatch_ptr[ cmd_index ].payload_size = payload_size;

  return RF_CMD_REGISTRATION_SUCCESS;
} /* rf_cmd_dispatch_register_handler */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Register the module with RF task.

  @details
  Register the command with RF task. 

  This passes the function pointer to call when dispatching. Also the size of
  associated data is passed.

  @param task_num
  The RF task with which the command is registered.

  @param module_id
  The module for which dispatcher is called.  

  @param rf_num_of_cmd
  The number of command associated with the module.  

  @param min_cmd_id
  The minimum number of command id associated with the mocule.

  @param max_cmd_id
  The minimum number of command id associated with the mocule.

*/
void 
rf_cmd_dispatch_deregister_rf_module
(
  rf_task_num_type task_num,
  rf_module_id_type module_id
)
{
  /*! @todo Check if the task_num and the module_id are valid or not. Also 
  check to see if rf_cmd_dispatch tbl was created at all. */
  modem_mem_free( rf_module_dispatch_tbl[task_num][ module_id ].rf_cmd_dispatch_tbl, 
                  MODEM_MEM_CLIENT_RFA);
  rf_module_dispatch_tbl[task_num][ module_id ].rf_cmd_dispatch_tbl = NULL;

} /* rf_cmd_dispatch_deregister_rf_module */

/*! @} */

/*============================================================================*/
/*!
  @name UMID based Registration

  @brief
  This section includes dispatcher interfaces to perform registration using
  Technology from UMID. There is no command based registration. Dispatcher
  will maintain an static table to accomodate callback handler funciton
  for each tech, where the number of techs is defined by maximum number of
  techs supported by UMID. Tech is specified by bit[31:24] of an UMID. Thus
  the maximum number of Techs supported would be 2^8 = 256.
 
  There are two kinds of dispatching.
 
  <b><i>Tech Based:</i></b> In this kind of dispatching, the RF_TASK will look 
  at the tech specified in the UMID (bit[31:24]) and dispatch by calling the
  callback handler function pertaining to this tech.
 
  <b><i>Module Based:</i></b> In this kind of dispatching, the RF_TASK will look 
  at the module specified in the UMID (bit[23:16]) and dispatch by calling the
  callback handler function pertaining to this module. Module based callback
  function will always have the precedence eventhough the tech based callback
  handler function is registerd.
 
  @image html rf_task_dispatch_table.jpg
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform UMID tech based registration
 
  @details
  This function should be called to perform registration of the callback
  handler funciton for a technology. The technology enumeration corresponds
  to the tech field (31:24) from 32-bit UMID.
 
  This function should be called only once per tech. If the function is called
  back-to-back for same tech without de-registration in the middle, then the
  scenario will be flagged as a failure and the second registration attempt
  will be ignored.
 
  @param umid_tech
  Tech corresponding bit [31:24] from UMID which is to be registered with the
  RF task
 
  @param umid_module
  Module corresponding bit [23:16] from UMID which is to be registered with the
  RF task. This field will be used to identify, if tech-based or module-based
  registration is requested. If the umid_module is less than
  MSGR_MODULE_RF_MAX, then it is considered module-based; if not, it will be
  tech-based.
 
  This indicates that for this tech, module based registration 
  needs to be done. This will be for the UMIDs which need to dispatched 
  based on module such as MSGR_TECH_RFA. MSGR_TECH_RFA is special because 
  this tech has 1x/HDR/WCDMA/LTE/TDSCDMA as modules; and the UMID has to be 
  dispatched  based on the module. In this case, module dispatching always 
  get precedence over the tech based dispatching. This field will be 
  populated during dispatch init only.
 
  @param tech_callback
  Callback handler function which is to be registered for the given tech.
 
  @return
  Status indicating, whether the tech registration was successfull or not.
*/
rf_cmd_dispatch_status_type
rf_cmd_dispatch_register_tech
(
  rf_task_num_type task_num,
  uint16 umid_tech,
  uint32 umid_module,
  rf_cmd_handler_func_type tech_callback
)
{
  rf_cmd_module_table_type *module_tbl_ptr = NULL;
  rf_cmd_tech_table_type *tech_tbl_ptr = NULL;

  /* Perform Parameter Validation : umid_tech */
  if ( umid_tech >= RF_NUM_OF_UMID_TECHS )
  {
    RF_MSG_1( RF_ERROR, "rf_cmd_dispatch_register_tech: Invalid Tech 0x%x",
              umid_tech );
    return RF_CMD_REGISTRATION_FAILURE;
  } /* if ( umid_tech >= RF_NUM_OF_UMID_TECHS ) */

  /* Perform Parameter Validation : tech_callback */
  if ( tech_callback == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cmd_dispatch_register_tech: Invalid CB for "
                        "Tech 0x%x", umid_tech );
    return RF_CMD_REGISTRATION_FAILURE;
  } /* if ( tech_callback == NULL ) */

  if ( rf_tech_dispatch_table[task_num].first_registration == TRUE )
  {
    rf_tech_dispatch_table[task_num].tech_data = 
      modem_mem_alloc( sizeof(rf_cmd_tech_table_type)*(umid_tech + 1),
                       MODEM_MEM_CLIENT_RFA);
	/* KW fix */
     if ( rf_tech_dispatch_table[task_num].tech_data == NULL )
     {
         RF_MSG_1( RF_ERROR, "modem_mem_alloc failed for MODEM_MEM_CLIENT_RFA",0);
         return RF_CMD_REGISTRATION_FAILURE;
      } 
	
    rf_tech_dispatch_table[task_num].max_tech_id = umid_tech;
    rf_tech_dispatch_table[task_num].first_registration = FALSE;
		
    memset( rf_tech_dispatch_table[task_num].tech_data, 0x0,
              sizeof(rf_cmd_tech_table_type)*(umid_tech + 1) );
    
  } /* if ( first_registration == TRUE ) */
  else
  {
    uint8 *tbl_start_memory_ptr = NULL;
    uint32 old_tech_data_size = 0;
    uint8 *tbl_added_memory_ptr = NULL;
    uint32 new_tech_data_size = 0;
    uint32 memset_size = 0;

    /* Check if rellocation is required or not */
    if ( umid_tech > rf_tech_dispatch_table[task_num].max_tech_id )
    {
      new_tech_data_size = sizeof(rf_cmd_tech_table_type)*(umid_tech + 1);
      rf_tech_dispatch_table[task_num].tech_data =
        modem_mem_realloc( rf_tech_dispatch_table[task_num].tech_data,
                           new_tech_data_size,
                           MODEM_MEM_CLIENT_RFA);

      tbl_start_memory_ptr = (uint8*)rf_tech_dispatch_table[task_num].tech_data;

      if ( rf_tech_dispatch_table[task_num].tech_data != NULL )
      {
        old_tech_data_size = 
         sizeof(rf_cmd_tech_table_type)*(rf_tech_dispatch_table[task_num].max_tech_id+1);
        memset_size = new_tech_data_size - old_tech_data_size;
        tbl_added_memory_ptr = tbl_start_memory_ptr  + old_tech_data_size;
        memset( tbl_added_memory_ptr, 0x0, memset_size );

        RF_MSG_5( RF_HIGH, "rf_cmd_dispatch_register_tech: Tech REALLOC : "
                  "[0x%8x] { 0x%08x + 0x%08x = 0x%08x } [0x%08x]",
                  tbl_start_memory_ptr,
                  old_tech_data_size,
                  memset_size,
                  new_tech_data_size,
                  tbl_added_memory_ptr
                  );
      } /* if ( rf_tech_dispatch_table.tech_data != NULL ) */
      else
      {
        RF_MSG( RF_ERROR, "rf_cmd_dispatch_register_tech: Tech REALLOC failed");
        return RF_CMD_REGISTRATION_FAILURE;
      } /* if ! ( rf_tech_dispatch_table.tech_data != NULL ) */

      rf_tech_dispatch_table[task_num].max_tech_id = umid_tech;

    } /* if ( umid_tech + 1 > rf_tech_dispatch_table.max_tech_id ) */
  } /* if ! ( first_registration == TRUE ) */

  tech_tbl_ptr = rf_tech_dispatch_table[task_num].tech_data;

  /* Check, if the registraion is module-based or tech-based */
  if ( umid_module < MSGR_MODULE_RF_MAX )
  {
    /*------------------------------------------------------------------------*/
    /* Module Based registration */
    /*------------------------------------------------------------------------*/

    /* Check whether the Module ID is valis or not */
    if ( umid_module >= MSGR_MODULE_RF_MAX )
    {
      RF_MSG_2( RF_ERROR, "rf_cmd_dispatch_register_tech: Invalid Module 0x%x "
                          "for Tech 0x%x", umid_module, umid_tech );
      return RF_CMD_REGISTRATION_FAILURE;
    } /* if ( umid_tech >= RF_NUM_OF_UMID_TECHS ) */

    /* Only one registration is allowed for each tech. If there are back to
    back registration attempts, then this will be error condition. */
    if ( tech_tbl_ptr[umid_tech].tech_based_cb != NULL )
    {
      //RF_MSG_1( RF_FATAL, "rf_cmd_dispatch_register_tech: Tech 0x%x already "
      //           "(Tech Based)registered. Aborting MODULE-BASED.",
      //           umid_tech );

      ERR_FATAL( "rf_cmd_dispatch_register_tech: Tech 0x%x already "
                 "(Tech Based)registered. Aborting MODULE-BASED.",
                 umid_tech, 0, 0 );
      return RF_CMD_REGISTRATION_FAILURE;
    } /* if ( rf_tech_table[umid_tech].tech_based_cb != NULL ) */
    else
    {
      if ( tech_tbl_ptr[umid_tech].module_table != NULL )
      {
        /* Get the Module Table pointer */
        module_tbl_ptr = tech_tbl_ptr[umid_tech].module_table;

        if ( module_tbl_ptr[umid_module].module_based_cb != NULL )
        {
          //RF_MSG_2( RF_FATAL, "rf_cmd_dispatch_register_tech: Tech 0x%x Module " 
          //          "0x%x already registered. Aborting MODULE-BASED.",
          //           umid_tech, umid_tech );

          ERR_FATAL( "rf_cmd_dispatch_register_tech: Tech 0x%x Module 0x%x "
                     "already registered. Aborting MODULE-BASED.",
                     umid_tech, umid_module, 0 );
          return RF_CMD_REGISTRATION_FAILURE;
        }
      } /* if ( rf_tech_table[umid_tech].module_table != NULL ) */
    } /* if ! ( rf_tech_table[umid_tech].tech_based_cb != NULL ) */

    /* Check if the memory allocation for the module table is already done
    or not. If the allocation is not done, then  allocate memory */
    if ( tech_tbl_ptr[umid_tech].module_table == NULL )
    {
      tech_tbl_ptr[umid_tech].module_table = 
         modem_mem_alloc( sizeof(rf_cmd_module_table_type)*MSGR_MODULE_RF_MAX,
                          MODEM_MEM_CLIENT_RFA);
  
      /* NULL pointer check for allocated memory */
      if ( tech_tbl_ptr[umid_tech].module_table == NULL )
      {
        RF_MSG_2( RF_ERROR, "rf_cmd_dispatch_register_tech: Malloc failed for "
                            "Tech 0x%x, Module 0x%x", umid_tech, umid_module );
        return RF_CMD_REGISTRATION_FAILURE;
      } /* if ( rf_tech_table[umid_tech].module_table == NULL ) */ 
      else
      {
        memset( tech_tbl_ptr[umid_tech].module_table, 0x0,
                sizeof(rf_cmd_module_table_type)*MSGR_MODULE_RF_MAX );
      }
    } /* if ( rf_tech_table[umid_tech].module_table == NULL ) */

    tech_tbl_ptr[umid_tech].tech_id = umid_tech;

    /* Get the Module Table pointer */
    module_tbl_ptr = tech_tbl_ptr[umid_tech].module_table;

    /* Go ahead withe Module Registration */

    /* Register Tech callback funtion */
    module_tbl_ptr[umid_module].module_based_cb = tech_callback;

    module_tbl_ptr[umid_module].module_id = (uint8)umid_module;

    if ( umid_module <= 63 )
    {
      tech_tbl_ptr[umid_tech].module_063_000_reg_mask |= ( 1ULL << umid_module );
    }

    /* Mark that this tech is now present in the dispatcher */
    rf_cmd_dispatch_set_tech_mask(task_num, umid_tech);

    RF_MSG_3( RF_MED, "rf_cmd_dispatch_register_tech: "
              "MODULE BASED [ 0x%x / 0x%x ] | Module Mask 0x%x", 
              umid_tech, 
              umid_module,
              tech_tbl_ptr[umid_tech].module_063_000_reg_mask );
    
  } /* if ( rf_tech_table[umid_tech].module_based_dispatch == TRUE ) */
  else
  {
    /*------------------------------------------------------------------------*/
    /* Tech Based registration */
    /*------------------------------------------------------------------------*/

    /* Only one registration is allowed for each tech. If there are back to
    back registration attempts, then this will be error condition. */
    if ( tech_tbl_ptr[umid_tech].tech_based_cb != NULL )
    {
      //RF_MSG_1( RF_FATAL, "rf_cmd_dispatch_register_tech: Tech 0x%x already "
      //           "(Tech Based)registered",umid_tech );

      ERR_FATAL( "rf_cmd_dispatch_register_tech: Tech 0x%x already "
                 "(Tech Based)registered",umid_tech, 0, 0 );
      return RF_CMD_REGISTRATION_FAILURE;
    }
    else if ( tech_tbl_ptr[umid_tech].module_table != NULL )
    {
      //RF_MSG_1( RF_FATAL, "rf_cmd_dispatch_register_tech: Tech 0x%x already "
      //           "(Module Based)registered",umid_tech );

      ERR_FATAL( "rf_cmd_dispatch_register_tech: Tech 0x%x already "
                "(Module Based)registered",umid_tech, 0, 0 );
      return RF_CMD_REGISTRATION_FAILURE;
    }

    /* Go ahead with tech Registration */

    /* Register Tech callback funtion */
    tech_tbl_ptr[umid_tech].tech_based_cb = tech_callback;

    tech_tbl_ptr[umid_tech].tech_id = umid_tech;

    rf_cmd_dispatch_set_tech_mask(task_num, umid_tech);

    RF_MSG_5( RF_MED, "rf_cmd_dispatch_register_tech: "
              "TECH BASED [ 0x%x ] | Tech Mask 0x%08x%08x%08x%08x",
              umid_tech,
              rf_tech_dispatch_table[task_num].tech_255_192_reg_mask,
              rf_tech_dispatch_table[task_num].tech_191_128_reg_mask,
              rf_tech_dispatch_table[task_num].tech_127_064_reg_mask,
              rf_tech_dispatch_table[task_num].tech_063_000_reg_mask );
    
  } /* if ! ( rf_tech_table[umid_tech].module_based_dispatch == TRUE ) */

  /* return success */
  return RF_CMD_REGISTRATION_SUCCESS;

} /* rf_cmd_dispatch_register_tech */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform UMID tech based deregistration
 
  @details
  This function should be called to perform deregistration of the callback
  handler funciton for a technology. The technology enumeration corresponds
  to the tech field (31:24) from 32-bit UMID.
 
  @param umid_tech
  Tech corresponding bit [31:24] from UMID which is to be deregistered with the
  RF task
*/
void
rf_cmd_dispatch_deregister_tech
(
  rf_task_num_type task_num,
  uint16 umid_tech,
  uint32 umid_module
)
{
  rf_cmd_tech_table_type *tech_tbl_ptr = NULL;
  uint64 new_module_063_000_reg_mask = 0;

  /* Perform Parameter Validation : umid_tech */
  if ( umid_tech >= RF_NUM_OF_UMID_TECHS )
  {
    RF_MSG_1( RF_ERROR, "rf_cmd_dispatch_deregister_tech: Invalid Tech 0x%x",
              umid_tech );
    return;
  } /* if ( umid_tech >= RF_NUM_OF_UMID_TECHS ) */

  tech_tbl_ptr = rf_tech_dispatch_table[task_num].tech_data;

  if ( NULL != tech_tbl_ptr )
  {
    /* Dellocate memory if module based dispatch was done */

    /* Check if the memory allocation for the module table was done
    or not. If the allocation is done, then de-allocate memory */
    if ( tech_tbl_ptr[umid_tech].module_table != NULL )
    {
      /* Perform Parameter Validation : umid_tech */
      if ( umid_module > rf_tech_dispatch_table[task_num].max_tech_id )
      {
        RF_MSG_2(RF_ERROR, "rf_cmd_dispatch_deregister_tech: Invalid Mode 0x%x "
                 "for tech 0x%x", umid_module, umid_tech);
        return;
      } /* if ( umid_tech >= rf_tech_dispatch_table.max_tech_id ) */

      tech_tbl_ptr[umid_tech].module_table[umid_module].module_id = 0;
      tech_tbl_ptr[umid_tech].module_table[umid_module].module_based_cb = NULL;

      if ( umid_module <= 63 )
      {
        new_module_063_000_reg_mask |= (1ULL << umid_module);
        new_module_063_000_reg_mask = ~new_module_063_000_reg_mask;

        /* Update the Vote Mask */
        tech_tbl_ptr[umid_tech].module_063_000_reg_mask &=
           new_module_063_000_reg_mask;
      }

      if ( tech_tbl_ptr[umid_tech].module_063_000_reg_mask == 0x0 )
      {
        modem_mem_free(tech_tbl_ptr[umid_tech].module_table,
                       MODEM_MEM_CLIENT_RFA);
        tech_tbl_ptr[umid_tech].module_table = NULL;
        RF_MSG(RF_MED, "rf_cmd_dispatch_deregister_tech: Module deallocated");
      }

      RF_MSG_3(RF_MED, "rf_cmd_dispatch_deregister_tech: "
               "MODULE BASED [ 0x%x / 0x%x ] | Module Mask 0x%08x",
               umid_tech,
               umid_module,
               tech_tbl_ptr[umid_tech].module_063_000_reg_mask);

    } /* if ( rf_tech_table[umid_tech].module_table != NULL ) */

    /* Now if there is no more module table (or never was), clean up the
       tech data. */
    if ( tech_tbl_ptr[umid_tech].module_table == NULL )
    {
      tech_tbl_ptr[umid_tech].tech_id = 0x0;
      tech_tbl_ptr[umid_tech].tech_based_cb = NULL;

      rf_cmd_dispatch_clear_tech_mask(task_num, umid_tech);

      if ( (rf_tech_dispatch_table[task_num].tech_063_000_reg_mask == 0x0) &&
           (rf_tech_dispatch_table[task_num].tech_127_064_reg_mask == 0x0) &&
           (rf_tech_dispatch_table[task_num].tech_191_128_reg_mask == 0x0) &&
           (rf_tech_dispatch_table[task_num].tech_255_192_reg_mask == 0x0)
           )
      {
        modem_mem_free(rf_tech_dispatch_table[task_num].tech_data,
                       MODEM_MEM_CLIENT_RFA);
        rf_tech_dispatch_table[task_num].tech_data = NULL;

        RF_MSG(RF_MED, "rf_cmd_dispatch_deregister_tech: Tech deallocated");
      }

      RF_MSG_5(RF_MED, "rf_cmd_dispatch_deregister_tech: "
               "TECH BASED [ 0x%x ] | Tech Mask 0x%08x%08x%08x%08x",
               umid_tech,
               rf_tech_dispatch_table[task_num].tech_255_192_reg_mask,
               rf_tech_dispatch_table[task_num].tech_191_128_reg_mask,
               rf_tech_dispatch_table[task_num].tech_127_064_reg_mask,
               rf_tech_dispatch_table[task_num].tech_063_000_reg_mask);
    }
  } 
  else
  {
    RF_MSG(RF_ERROR, "rf_cmd_dispatch_deregister_tech: tech_tbl_ptr is NULL");
  }
  /* return */
  return;

} /* rf_cmd_dispatch_deregister_tech */

/*! @} */

/*============================================================================*/
/*!
  @name Command Processing

  @brief
  This section includes functions to perform command processing when RF_TASK
  receives the command.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to process a received command.

  @details
  Function to process a received command.
*/
void
rf_cmd_process
(
  rf_task_num_type task_num
)
{
  rf_cmd_type *cmd;
  rf_module_id_type  module_id = 0;
  rf_cmd_id_type cmd_id;
  rf_cmd_dispatch_tbl_type *cmd_dispatch_ptr = NULL;
  rf_cmd_id_type cmd_index = 0;
  uint8 tech_id;
  rf_cmd_module_table_type *module_tbl_ptr = NULL;
  boolean rf_dispatch_done = FALSE;
  boolean new_dispatch_done = FALSE;
  rf_cmd_tech_table_type *tech_tbl_ptr = NULL;
  
  cmd = rf_cmd_get(task_num);

  if ( cmd == NULL )
  {
    return;
  }

  #ifdef FEATURE_WCDMA
  if ( (cmd->msgr_hdr.id == RFLM_WCDMA_DO_MTPL_CMD) || 
       (cmd->msgr_hdr.id == RFLM_WCDMA_DO_THERM_READ_CMD) ||
       (cmd->msgr_hdr.id == RFLM_WCDMA_TRIGGER_AUTO_PIN_READ_CMD) )
  {
    rf_dispatch_command( rf_task_get_dispatch_handle(),cmd );
    rf_dispatch_done = TRUE;
  }
  #endif

  #ifdef FEATURE_TDSCDMA
  if ( cmd->msgr_hdr.id == RFLM_TDSCDMA_TRIGGER_PDET_CMD )
  {
    rf_dispatch_command( rf_task_get_dispatch_handle(),cmd );
    rf_dispatch_done = TRUE;
  }
  else if ( cmd->msgr_hdr.id == RFLM_TDSCDMA_TRIGGER_THERM_CMD)
  {
    rf_dispatch_command( rf_task_get_dispatch_handle(),cmd );
    rf_dispatch_done = TRUE;
  }
  else if ( cmd->msgr_hdr.id == RFLM_TDSCDMA_TRIGGER_AUTOPIN_CMD)
  {
    rf_dispatch_command( rf_task_get_dispatch_handle(),cmd );
    rf_dispatch_done = TRUE;
  }
  #endif

  #ifdef FEATURE_CDMA
  
  if ( cmd->msgr_hdr.id == RFLM_C2K_READ_AUTOPIN_CMD )
  {
    rf_dispatch_command( rf_task_get_dispatch_handle(),cmd );
    rf_dispatch_done = TRUE;
  }
 
  #endif

  if ( cmd->msgr_hdr.id == RFLM_CMN_AUTOPIN_RSP )
  {
    rf_dispatch_command( rf_task_get_dispatch_handle(),cmd );
    rf_dispatch_done = TRUE;
  }

  tech_id = cmd->rex_hdr.cmd_hdr.tech_id;
  module_id = cmd->rex_hdr.cmd_hdr.module_id;

  tech_tbl_ptr = rf_tech_dispatch_table[task_num].tech_data;

  if ( ( tech_id <= rf_tech_dispatch_table[task_num].max_tech_id ) &&
       ( rf_dispatch_done == FALSE ) )
  {
    if ( tech_tbl_ptr[tech_id].tech_based_cb != NULL )
    {
      tech_tbl_ptr[tech_id].tech_based_cb( cmd );

      /* If required, send the acknowledge signal */
      if( cmd->complete_sig && cmd->task_to_sig )
      {
        (void) rex_set_sigs( cmd->task_to_sig, cmd->complete_sig );
      } /* if( cmd->complete_sig && cmd->task_to_sig ) */

      new_dispatch_done = TRUE;
    } /* if ( tech_tbl_ptr[tech_id].tech_based_cb != NULL ) */
    else if ( tech_tbl_ptr[tech_id].module_table != NULL )
    {
      /* Get the Module Table pointer */
      module_tbl_ptr = tech_tbl_ptr[tech_id].module_table;
  
      if ( module_tbl_ptr[module_id].module_based_cb != NULL)
      {
        module_tbl_ptr[module_id].module_based_cb( cmd );

        /* If required, send the acknowledge signal */
        if( cmd->complete_sig && cmd->task_to_sig )
        {
          (void) rex_set_sigs( cmd->task_to_sig, cmd->complete_sig );
        }

        new_dispatch_done = TRUE;
      } /* if ( module_tbl_ptr[module_id].module_based_cb != NULL) */
    } /* if ! ( tech_tbl_ptr[tech_id].tech_based_cb != NULL ) */
  } /* if ( tech_id <= rf_tech_dispatch_table.max_tech_id ) */

  if ( ( new_dispatch_done == FALSE ) &&
       ( rf_dispatch_done == FALSE ) )
  {
    if ( rf_module_dispatch_tbl[task_num] == NULL )
    {
      cmd->status = RF_CMD_NOT_SUPPORTED;
      RF_MSG_2( RF_HIGH, "Module %d Command %d not supported. "
                "Dispatch Table empty.", cmd->rex_hdr.cmd_hdr.module_id, 
                cmd->rex_hdr.cmd_hdr.cmd_id );
  
      if( cmd->complete_sig && cmd->task_to_sig != NULL )
      {
        (void) rex_set_sigs( cmd->task_to_sig, cmd->complete_sig );
      }
  
      rf_cmd_free(task_num, cmd );
      return;
    }
  
    if ( module_id >= max_num_of_modules[task_num] ||
         cmd->rex_hdr.cmd_hdr.cmd_id > rf_module_dispatch_tbl[task_num][ module_id ].max_cmd_id ||
         cmd->rex_hdr.cmd_hdr.cmd_id < rf_module_dispatch_tbl[task_num][ module_id ].min_cmd_id
        )
    {
      cmd->status = RF_CMD_NOT_SUPPORTED;
      RF_MSG_2( RF_HIGH, "Module %d Command %d not supported",
                cmd->rex_hdr.cmd_hdr.module_id, cmd->rex_hdr.cmd_hdr.cmd_id );
  
      if( cmd->complete_sig && cmd->task_to_sig )
      {
        (void) rex_set_sigs( cmd->task_to_sig, cmd->complete_sig );
      }
  
      rf_cmd_free( task_num, cmd );
  
      return;
    }
  
    cmd_id = cmd->rex_hdr.cmd_hdr.cmd_id;
  
    cmd_dispatch_ptr = rf_module_dispatch_tbl[task_num][ module_id ].rf_cmd_dispatch_tbl;
  
    cmd_index = cmd_id - rf_module_dispatch_tbl[task_num][ module_id ].min_cmd_id;
  
  
    if ( cmd_dispatch_ptr==NULL || cmd_dispatch_ptr[ cmd_index ].cmd_id!=cmd_id )
    {
      cmd->status = RF_CMD_NOT_SUPPORTED;
  
      RF_MSG_2( RF_HIGH, "Module %d Command %d not supported. Command not "
                "registered. Invalid command ID.", 
                cmd->rex_hdr.cmd_hdr.module_id, 
                cmd->rex_hdr.cmd_hdr.cmd_id );
  
      if( cmd->complete_sig && cmd->task_to_sig)
      {
        (void) rex_set_sigs( cmd->task_to_sig, cmd->complete_sig );
      }
  
      rf_cmd_free( task_num, cmd );
      return;
  
    }
  
    if ( cmd->status != RF_CMD_NOT_SUPPORTED )
    {
      cmd_dispatch_ptr[cmd_index].cmd_handler( module_id, 
                                        cmd_id, 
                                        cmd_dispatch_ptr[cmd_index].payload_size, 
                                        cmd );
  
      RF_MSG_2( RF_LOW, "Module %d Command %d Success.", 
                cmd->rex_hdr.cmd_hdr.module_id, 
                cmd->rex_hdr.cmd_hdr.cmd_id );
  
      cmd->status = RF_CMD_SUCCESS;
  
      /* If required, send the acknowledge signal */
      if( cmd->complete_sig && cmd->task_to_sig )
      {
        (void) rex_set_sigs( cmd->task_to_sig, cmd->complete_sig );
      }
    }
  } /* if ( new_dispatch_done == FALSE ) */

  /* Processing of command complete. Free Memory */
  rf_cmd_free(task_num, cmd );

  return;
} /* rf_cmd_process */

/*! @} */

#endif /* FEATURE_RF_TASK */

/*! @} */

