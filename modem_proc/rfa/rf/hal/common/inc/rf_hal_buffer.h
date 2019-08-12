#ifndef RF_HAL_BUFFER_H
#define RF_HAL_BUFFER_H
/*!
  @file
  rf_hal_buffer.h
 
  @brief
  Contains RF Script buffer implementation
  
  @addtogroup RF_HAL_BUFFER
  @{
*/
/*==============================================================================

  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/hal/common/inc/rf_hal_buffer.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------- 
09/05/14   sb      Fix timing of IP2 cal setup writes
02/03/14   hm      Added get_rffe_info() to retrieve RFFE transactions
12/26/13   hm      Added shim layer function to take in tech scaling as additional argument,
                   for script buffer creation
12/13/13   rp      Enhancement in RFFE transaction time calculation to predict more accurate
                   script execution time.
11/11/13   rp      Enhancement in get transaction/execution time API.
11/01/13   rp      Changed the script type in get execution time API.
10/31/13   rp      Modified the get execution type API to return proper value when new ref
                   time is set at the begining of the script. Also, added an extended command 
                   parameter in get transaction API.
09/28/13   rp      Added an API to return transaction time for given rffe script 
                   entry.
09/26/13   rp      Added an API to return the total execution time of given script.
09/11/13   rp      Added an overloaded function for multibyte rffe append.
01/22/12   sb      Added helper functions for obtaining RFFE/SSBI/GRFC space available in buffer
12/26/12   sb      Added helper functions for Therm read support
11/27/12   aki     Added rf_buffer_delete
10/30/12   sr      changed the default parameter value of new_time_ref to TRUE for
                   append_grfc() as it is the common usage for GRFCs.
07/24/12   shb     Maintain ssbi/rffe execution times and update start_delta in
                   scripts taking execution delays in account
07/20/12   shb     Added arg in clear() to select script to be cleared
07/13/12   sr      Updated APIs to take optional new time reference flag. This will
                   enable clients to set the new start_time instead of delay from
                   the previous transaction.
07/11/12   shb     Updated C shim layer to declare a struct named rf_buffer_intf
                   This enables C code to have type checking instead of void ptr
07/11/12   shb     Deleted reset_buffer_ptr() as clear() is sufficient
07/03/12   ac      added reset_buffer_ptr() to reset the buffer pointer before every statc event
06/25/12   shb     Renamed buffer_intf to rf_buffer_intf. Added clear()
06/19/12   shb     Initial version

============================================================================*/

#include "rf_hal_bus_types.h"
#include "rf_buffer_intf.h"

#define SCLK_FREQUENCY_IN_MHZ_X10  192

#define NUM_CYCLE_RFFE_COMMAND_FRAME 13
#define NUM_CYCLE_RFFE_DATA_FRAME 9
#define NUM_CYCLE_RFFE_ADDRESS_FRAME 9
#define NUM_CYCLE_RFFE_BUS_PARK_CYCLE 1
#define NUM_CYCLE_RFFE_SEQ_START_CONDITION 2
#define NUM_ADDITIONAL_CYCLE_FOR_RFFE_READ NUM_CYCLE_RFFE_BUS_PARK_CYCLE

/* variable to take care of additonal execution delay required by 
   CCS to process given RFFE transaction. It is 0.6 microseconds*/
#define CCS_RFFE_EXECUTION_DELAY_X100 50 

/* variable to take care of CCS Task tear down delay required by 
   CCS at the end of each task. It is 3.2 microseconds*/
#define CCS_TASK_TEAR_DOWN_DELAY_X100 320

/* default value of time required per SSBI transaction. X100 microseconds*/
#define DEFAULT_TIME_PER_SSBI_X100 220

/* default value of time required per RFFE transaction. X100 microseconds*/
#define DEFAULT_TIME_PER_RFFE_X100 130



#ifdef __cplusplus
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Contains RF script buffer with SSBI, RFFE and GRFC scripts that can be
  processed by RF HAL

  @details
  Contains SSBI, RFFE, GRFC scripts of rf_hal_bus_resource_script_type.
  Implements constructor that manages dynamic memory allocation of these
  scripts. Implements base class APIs to support appending of these scripts.
  Implements get_immutable_script_ptr to return a script for processing by HAL
*/
class rf_script_buffer : public rf_buffer_intf
{
public:
  /*! Constructor */
  rf_script_buffer( uint16 ssbi_script_size,
                    uint16 rffe_script_size,
                    uint16 grfc_script_size,
                    /* multiplication factor to convert microseconds to 
                       desired tech units*/
                    uint32 scaling_factor_x100 = 100);

  /*! Destructor */
  ~rf_script_buffer( void );

  /*! Empty contents of all scripts in the buffer */
  boolean clear( rf_buffer_script_type script = RF_BUFFER_ALL );

  /*! Add SSBI to script in buffer */
  boolean append_ssbi
  (
    uint8 bus,
    uint8 address,
    uint8 data,
    rf_buffer_transaction_type rd_wr,
    int16 delay = 0,
    boolean new_time_ref = FALSE
  );

  /*! Add RFFE to script in buffer */
  boolean append_rffe
  ( 
    uint8 slave_id,
    uint8 channel,
    uint16 address,
    uint8 data,
    rf_buffer_transaction_type rd_wr,
    int16 delay = 0,
    boolean extended_cmd = FALSE,
    uint8 half_rate = 0,
    uint8 rd_delay = 0,
    boolean new_time_ref = FALSE
  );

  /*overloaded function to append multibyte RFFE writes to script buffer.*/
  boolean append_rffe
  ( 
    uint8 slave_id,
    uint8 channel,
    uint16 address,
    uint8 *data,
    rf_buffer_transaction_type rd_wr,
    int16 delay = 0,
    boolean extended_cmd = FALSE,
    uint8 half_rate = 0,
    uint8 rd_delay = 0,
    boolean new_time_ref = FALSE,
    uint8 num_bytes = 1
  );

  /*! Add GRFC */
  boolean append_grfc
  (
    uint16 grfc_id,
    uint8 start_logic,
    int16 delay,
    boolean new_time_ref = TRUE
  );

  /*! Get how many more SSBIs can be added to buffer */
  int16 get_ssbi_space_available(void);

  /*! Get how many more SSBIs/RFFEs/GRFCs can be added to buffer*/
  int16 get_space_available(rf_buffer_script_type script);

  /*! Get a const script ptr from object for execution of script as well as for 
  debugging/oftarget */
  const rf_hal_bus_resource_script_type* get_immutable_script_ptr
  (
    rf_hal_bus_resource_type resource_type
  );

  /*! Get total execution time of a script */
  boolean
  get_script_execution_time
  (
    rf_buffer_script_type resource_type,
    uint32 *execution_time
  );

  /*! Get total transaction time for a give rffe script entry */
  uint32 get_rffe_transaction_time
  (
    uint16 address,
    uint8 num_bytes,
    rf_buffer_transaction_type rd_wr,
    boolean extended_cmd
  );

  /* Get the information of the SSBI by index number*/
  boolean get_ssbi_info(uint16 index, rf_buffer_ssbi_info_type* ssbi_info);

  /* Get the information of the RFFE by index number*/
  boolean get_rffe_info(uint16 index, rf_buffer_rffe_info_type* rffe_info);

  /*Get the number of transactions of a particular communication protocol type*/
  uint16 get_num_trans(rf_buffer_script_type script);

  int16 get_last_transaction_time(rf_hal_bus_resource_type resource_type);  

private:
  /*! SSBI script in RF HAL format */
  rf_hal_bus_resource_script_type ssbi;

  /*! Size of SSBI script */
  uint16 max_ssbi;

  /*! Time per SSBI transaction in x10 units */
  uint32 ssbi_trans_time_x100;

  /*! Variable to keep track of current SSBI execution time - again 
  using x10 units to maintain better precision */
  int32 ssbi_exec_time_x100;

  /*! RFFE scripts in RF HAL format */
  rf_hal_bus_resource_script_type rffe;

  /*! Size of RFFE script */
  uint16 max_rffe;

  /*! Time per RFFE transaction in x10 units */
  uint32 rffe_trans_time_x100;

  /*! Variable to keep track of current RFFE execution time - using 
  x10 units for better precision */
  int32 rffe_exec_time_x100;

  /*! GRFC script in RF HAL format */
  rf_hal_bus_resource_script_type grfc;

  /*! Size of GRFC script */
  uint16 max_grfc;

  /* flag indicating new_time_ref is set to true for given script */
  boolean ssbi_new_time_ref;

  /* flag indicating new_time_ref is set to true for given script */
  boolean rffe_new_time_ref;

  /* delay used to offset the time reference of given script */
  int32 ssbi_new_time_ref_offset_x100;

  /* delay used to offset the time reference of given script */
  int32 rffe_new_time_ref_offset_x100;

  /* Multiplication factor to convert the transaction/execution time 
  from micro seconds to units specified by technology. Value of this
  factor will be provided by each technology while constructing the 
  buffer. For eg. if GSM wants delay to be calculated in QS (Quarter Symbol)
  unit instead of default microseconds unit (where 1 QS = 0.92 microseconds)
  it must provide scaling_factor_x100 = 92 while constructing the buffer*/
  int16 tech_unit_conversion_factor_x100;

}; /* class rf_script_buffer : public buffer_intf */

#endif /* #ifdef __cplusplus */

#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------------------------------------------------------*/
extern rf_buffer_intf* 
rf_buffer_create
(
  uint16 ssbi_script_size,
  uint16 rffe_script_size,
  uint16 grfc_script_size
);

/*----------------------------------------------------------------------------*/
extern rf_buffer_intf* 
rf_buffer_create_tech_scaling
(
  uint16 ssbi_script_size,
  uint16 rffe_script_size,
  uint16 grfc_script_size,
  uint32 scaling_factor_x100
);

/*----------------------------------------------------------------------------*/
void
rf_buffer_delete
(
  rf_buffer_intf* buf
);

/*----------------------------------------------------------------------------*/
extern const rf_hal_bus_resource_script_type* 
rf_buffer_get_script_ptr
(
  rf_buffer_intf* buf_ptr,
  rf_hal_bus_resource_type resource_type
);

extern int16 rf_buffer_get_last_transaction_time
(
   rf_buffer_intf* buf_ptr,
   rf_hal_bus_resource_type resource_type
);

#ifdef __cplusplus
}
#endif

/*! @} */

#endif /* #ifndef RF_HAL_BUFFER_H */
