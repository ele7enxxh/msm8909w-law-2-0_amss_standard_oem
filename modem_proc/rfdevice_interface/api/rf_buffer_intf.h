#ifndef RF_BUFFER_INTF_H
#define RF_BUFFER_INTF_H
/*!
  @file
  rf_buffer_intf.h

  @brief
  This file contains the interface class definition for all RF script buffers

  @addtogroup RF_BUFFER_INTF
  @{
*/
/*==============================================================================

  Copyright (c) 2012 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rf_buffer_intf.h#1 $

when       who     what, where, why
--------------------------------------------------------------------------------
05/08/14   sn      common timing table for device driver back calculation
02/20/14   cd      Derive from public rfa, so that new operator allocates on
                   MPSS heap
02/13/14   hm      Added shim layer API rf_buffer_append_rffe_multi
02/03/14   hm      Added get_rffe_info() to retrieve RFFE transactions
11/08/13   rp      Added the shim layer get execution time API header.
11/01/13   rp      Added an interface function to get script execution time.
09/11/13   rp      Added overloaded function for append_rffe API
01/22/12   sb      Added helper functions for obtaining RFFE/SSBI/GRFC space available in buffer
12/26/12   sb      Added helper functions for Therm read support
12/20/12   ggs     Added api to append rffe
10/30/12   sr      changed the default parameter value of new_time_ref to TRUE for
                   append_grfc() as it is the common usage for GRFCs.
10/05/12   aa      Added api to append grfc
07/20/12   shb     Added enum to distinguish between SSBI, RFFE, GRFC scripts
07/20/12   shb     Added rf_buffer_clear_script() to clear individual scripts
                   in buffer
07/13/12   sr      Added APIs to take optional new time reference flag. This will
                   enable clients to set the new start_time instead of delay from
                   the previous transaction.
07/11/12   shb     Updated C shim layer to declare a struct named rf_buffer_intf
                   This enables C code to have type checking instead of void ptr
07/11/12   shb     Created virtual destructor in rf_buffer_intf class
07/02/12   ac      added reset_buffer_ptr() to reset the buffer pointer
06/25/12   shb     Added rf_buffer_clear(). Renamed buffer_intf class to
                   rf_buffer_intf
06/20/12   shb     Created rf_buffer_get_ssbi_space_available shim api
06/19/12   shb     Initial version

============================================================================*/

#include "comdef.h"
#include "rfcommon_msg.h"

#ifdef __cplusplus
#include "rfa.h"
#endif

#define RFDEVICE_NUM_TRANS_TIMING_CALC 10
/*----------------------------------------------------------------------------*/
/*!
  @brief Enum to distinguish between read and write transaction to a buffer
*/
typedef enum
{
  /*! Read transaction */
  RF_BUFFER_READ,

  /*! Write transaction */
  RF_BUFFER_WRITE,

  /*! Invalid entry */
  RF_BUFFER_INVALID_TRANSACTION
} rf_buffer_transaction_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief Used to distinguish between SSBI, RFFE, GRFC scripts
*/
typedef enum
{
  /*! SSBI script in RF Buffer */
  RF_BUFFER_SSBI,

  /*! RFFE script in RF Buffer */
  RF_BUFFER_RFFE,

  /*! GRFC script in RF Buffer */
  RF_BUFFER_GRFC,

  /*! Use this enum if referencing all scripts in buffer. Say
  you want to clear all scripts */
  RF_BUFFER_ALL,
  RF_BUFFER_MAX = RF_BUFFER_ALL,

  /*! Invalid enum */
  RF_BUFFER_INVALID_SCRIPT

} rf_buffer_script_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief struct used to retrive SSBI info from script buffer
*/
typedef struct
{
  uint8 bus;
  uint8 address;
  uint8 data;
  rf_buffer_transaction_type rd_wr;
  int16 delay;
} rf_buffer_ssbi_info_type;

/*! @brief The numbef of bytes for extended RFFE write/reads. MIPI standard is minimum 4, maximum 16 */
#define RFDEVICE_RFFE_MAX_BYTES       4	//Should match RF_HAL_BUS_RFFE_MAX_BYTES

/*----------------------------------------------------------------------------*/
/*!
  @brief struct used to retrive RFFE info from script buffer
*/
typedef struct
{
  uint8 channel;
  uint8 slave_id;
  uint8 num_bytes;
  uint16 address;
  uint8 data[RFDEVICE_RFFE_MAX_BYTES] ALIGN(4);
  rf_buffer_transaction_type rd_wr;
} rf_buffer_rffe_info_type;

/*----------------------------------------------------------------------------*/
#ifdef __cplusplus

/*!
  @brief
  Interface buffer class that contains all APIs that every RF script buffer
  needs to implement

  @details
  Should not contain any data definitions. Just virtual interface methods that
  each RF buffer type needs to implement. All scripts (data) should be defined
  in derived classes as the type may be different for various script types

  Contains simple virtual function implementations returning failure. These are
  not made pure virtual functions as it is not necessary to force every derived
  script class to implement all the APIs. Thus, a script that contains SSBI
  only need not support append_rffe and append_grfc methods. They inherit the
  base class implementation that will just return failure.
*/
class rf_buffer_intf: public rfa
{
public:

  /*! Empty virtual destructor - base, interface class has no data so nothing
  to delete */
  virtual ~rf_buffer_intf( void )
  {
  }

  /*! Derived class implementation should clear the contents of all buffers.
  Base class implementation just returns FAILURE. */
  virtual boolean clear( rf_buffer_script_type script = RF_BUFFER_ALL )
  {
    RF_MSG(RF_ERROR,"clear() method not implemented");
    return FALSE;
  }

  /*! virtual base class implementation that just returns FALSE. Derived
  class must implement this function if it contains a SSBI buffer */
  virtual boolean append_ssbi(uint8 bus,
                              uint8 address,
                              uint8 data,
                              rf_buffer_transaction_type rd_wr,
                              int16 delay,
                              boolean new_time_ref = FALSE)
  {
    RF_MSG(RF_ERROR,"append_ssbi() method not implemented");
    return FALSE;
  }

  /*! virtual base class implementation that just returns FALSE. Derived
  class must implement this function if it contains a RFFE buffer */
  virtual boolean append_rffe(uint8 slave_id,
                              uint8 channel,
                              uint16 address,
                              uint8 data,
                              rf_buffer_transaction_type rd_wr,
                              int16 delay,
                              boolean extended_cmd,
                              uint8 half_rate,
                              uint8 rd_delay,
                              boolean new_time_ref = FALSE)
  {
    RF_MSG(RF_ERROR,"append_rffe() method not implemented");
    return FALSE;
  }

  /*! Overloaded funciton to virtual base class implementation that just returns
  FALSE. Derived classes must implement this function if it contains a RFFE
  buffer */
  virtual boolean append_rffe(uint8 slave_id,
                              uint8 channel,
                              uint16 address,
                              uint8 *data,
                              rf_buffer_transaction_type rd_wr,
                              int16 delay,
                              boolean extended_cmd,
                              uint8 half_rate,
                              uint8 rd_delay,
                              boolean new_time_ref = FALSE,
                              uint8 num_bytes = 1)
  {
    RF_MSG(RF_ERROR,"append_rffe() method not implemented");
    return FALSE;
  }

  /*! virtual base class implementation that just returns FALSE. Derived
  class must implement this function if it contains a GRFC buffer */
  virtual boolean append_grfc(uint16 grfc_id,
                              uint8 start_logic,
                              int16 delay,
                              boolean new_time_ref = TRUE)
  {
    RF_MSG(RF_ERROR,"append_grfc() method not implemented");
    return FALSE;
  }

  /*! virtual base class implementation that just returns 0.Derived
  class must implement this function if it contains a SSBI buffer */
  virtual int16 get_ssbi_space_available(void)
  {
    RF_MSG(RF_ERROR,"get_ssbi_space_available() method not implemented");
    return 0;
  }

  virtual int16 get_space_available(rf_buffer_script_type script)
  {
    RF_MSG(RF_ERROR,"get_space_available() method not implemented");
    return 0;
  }

  virtual boolean get_ssbi_info(uint16 index, rf_buffer_ssbi_info_type* ssbi_info)
  {
    RF_MSG(RF_ERROR,"get_ssbi_info() method not implemented");
    return FALSE;
  }

  virtual boolean get_rffe_info(uint16 index, rf_buffer_rffe_info_type* rffe_info)
  {
    RF_MSG(RF_ERROR,"get_rffe_info() method not implemented");
    return FALSE;
  }

  virtual uint16 get_num_trans(rf_buffer_script_type script)
  {
    RF_MSG(RF_ERROR,"get_num_trans() method not implemented");
    return 0;
  }

  virtual boolean
  get_script_execution_time
  (
    rf_buffer_script_type resource_type,
    uint32 *execution_time
  )
  {
    RF_MSG(RF_ERROR,"get_script_execution_time() method not implemented");
    return FALSE;
  }

};

#else

struct rf_buffer_s;
typedef struct rf_buffer_s rf_buffer_intf;

#endif /* #ifdef __cplusplus */


#ifdef __cplusplus
extern "C"
{
#endif
  extern uint8 rfdevice_timing_back_calc[RFDEVICE_NUM_TRANS_TIMING_CALC];
  /*----------------------------------------------------------------------------*/
  extern boolean
  rf_buffer_clear
  (
    rf_buffer_intf* buf_ptr
  );

  /*----------------------------------------------------------------------------*/
  extern boolean
  rf_buffer_clear_script
  (
    rf_buffer_intf* buf_ptr,
    rf_buffer_script_type script
  );

  /*----------------------------------------------------------------------------*/
  extern boolean
  rf_buffer_append_ssbi
  (
    rf_buffer_intf* buf_ptr,
    uint8 bus,
    uint8 address,
    uint8 data,
    rf_buffer_transaction_type rd_wr,
    int16 delay
  );

  /*----------------------------------------------------------------------------*/
  extern boolean
  rf_buffer_append_ssbi_with_new_time
  (
    rf_buffer_intf* buf_ptr,
    uint8 bus,
    uint8 address,
    uint8 data,
    rf_buffer_transaction_type rd_wr,
    int16 delay,
    boolean new_time_ref
  );
  /*----------------------------------------------------------------------------*/
  extern boolean
  rf_buffer_append_grfc
  (
    rf_buffer_intf* buf_ptr,
    uint16 grfc_id,
    uint8 start_logic,
    int16 delay,
    boolean new_time_ref
  );

  /*----------------------------------------------------------------------------*/
  extern boolean
  rf_buffer_append_rffe
  (
    rf_buffer_intf* buf_ptr,
    uint8 slave_id,
    uint8 channel,
    uint16 address,
    uint8 data,
    rf_buffer_transaction_type rd_wr,
    int16 delay,
    boolean extended_cmd,
    uint8 half_rate,
    uint8 rd_delay,
    boolean new_time_ref
  );

  /*----------------------------------------------------------------------------*/
  extern boolean
  rf_buffer_append_rffe_multi

  (
    rf_buffer_intf* buf_ptr,
    uint8 slave_id,
    uint8 channel,
    uint16 address,
    uint8 *data,
    rf_buffer_transaction_type rd_wr,
    int16 delay,
    boolean extended_cmd,
    uint8 half_rate,
    uint8 rd_delay,
    boolean new_time_ref,
    uint8 num_bytes
  );
  /*----------------------------------------------------------------------------*/
  extern int16
  rf_buffer_get_ssbi_space_available
  (
    rf_buffer_intf* buf_ptr
  );

  /*----------------------------------------------------------------------------*/
  extern int16
  rf_buffer_get_space_available
  (
    rf_buffer_intf* buf_ptr,
    rf_buffer_script_type script
  );
  /*----------------------------------------------------------------------------*/
  extern boolean
  rf_buffer_destroy
  (
    rf_buffer_intf* buf_ptr
  );

  /*----------------------------------------------------------------------------*/
  extern boolean
  rf_buffer_get_ssbi_info
  (
    rf_buffer_intf* buf_ptr,
    uint16 index,
    rf_buffer_ssbi_info_type* ssbi_info
  );
  /*----------------------------------------------------------------------------*/

  extern uint16 rf_buffer_get_num_trans
  (
    rf_buffer_intf* buf_ptr,
    rf_buffer_script_type script
  );

  extern boolean
  rf_buffer_get_script_execution_time
  (
    rf_buffer_intf* buf_ptr,
    rf_buffer_script_type resource_type,
    uint32 *execution_time
  );


#ifdef __cplusplus
}
#endif

/*! @} */

#endif /* #ifndef RF_BUFFER_INTF_H */
