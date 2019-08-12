/*!
  @file rf_hal_sbi.h

  @brief
  This file contains a HAL for SBI driver differences. 

  @details
  This file is used to distinguish modem dependencies due to SBI driver in commonly 
  used functions accross multiple targets. Different modems might 
  have different architectures to send SBIs out, however, in the common device driver,
  only one function should be employed. This file defines that common function.
  This file will have a version for each modem.
 
*/

/*============================================================================== 
   
  Copyright (c) 2009, 2011 Qualcomm Technologies Incorporated. All Rights Reserved
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
   
                        EDIT HISTORY FOR MODULE 
   
This section contains comments describing changes made to the module. 
Notice that changes are listed in reverse chronological order. 
 
$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/ccs/rf_hal_sbi.h#1 $ 
   
when       who     what, where, why 
--------   ---     ------------------------------------------------------------- 
10/16/13   ra      Support CCS portability
11/14/12   jfc     Properly inline rfhal_sbi_write_byte() to avoid warning 
10/19/12   adk     Fixed dependencies to port antenna tuner code.
09/07/12   jfc     Declare rfhal_sbi_write_byte as inline 
08/28/12   jfc     Support masked ssbi writes thru access task 
06/25/12   jfc     Get rid of rf_hal_ssbi_write_verify_byte()
05/23/12   jfc     Use rfhal_ naming convention for all functions
05/21/12   jfc     Use rfhal_ naming convention to be backwards compatible
05/15/12   jfc     Add rfhal_sbi_write_script_generic()
10/20/11   shb     Added macro to send SSBI script to RFD from device
08/27/09   can     RFD Macro change and compilation fixes.
08/14/09   can     Initial version.
==============================================================================*/ 

#ifndef RF_HAL_SBI_H
#define RF_HAL_SBI_H

#include "rf_hal_bus_types.h"
#include "rfdevice_cmn_intf.h"
#include "rf_hal_ccs.h"


/*============================================================================== 
 
                               MACROS AND TYPEDEFS 
 
==============================================================================*/


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This macro configures the RTR register page for a given SSBI bus. The use of
  sbi_write_byte should be replaced with this macro everywhere in the RF code.

*/
#define RTR_OUT_SINGLE( bus, reg, value ) \
			rfhal_sbi_write_byte(bus, reg, value)

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This macro outputs a list of values corresponding to the list of specified 
  RTR registers. It overwrites the list of registers. The use of
  rfd_sbi_write_bytes should be replaced with this macro everywhere in the RF code.

*/
#define RTR_OUT_MULTIPLE(bus, addr_ptr, data_ptr, length  ) \
			rfhal_sbi_write_bytes(bus, addr_ptr, data_ptr, length)

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Macro to send a SSBI script to hardware. Passes the script pointer and size
  to RFD function that executes scripts. Callback args are NULL
*/
#define RTR_OUT_SCRIPT(script_ptr, size, ... ) \
			rfhal_sbi_write_script(script_ptr, size)


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This macro reads and returns the value from a RTR register. The use of
  sbi_read_byte should be replaced with this macro everywhere in the RF code.

*/
#define RTR_IN_SINGLE( bus, reg, var)  \
			rfhal_sbi_read_byte(bus, reg, (var))

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This macro reads and returns the value from a SSBI register. The use of
  sbi_read_byte should be replaced with this macro everywhere in the RF code.

*/
#define FTM_SBI_IN( bus, reg, var,cb_ptr,cb_data)  \
			RTR_IN_SINGLE(bus, reg, (var))

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This macro write to a SSBI register. The use of
  sbi_write_byte should be replaced with this macro everywhere in the RF code.

*/
#define FTM_SBI_OUT( bus, reg, var,cb_ptr,cb_data)  \
			RTR_OUT_SINGLE(bus, reg, (var))


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*----------------------------------------------------------------------------*/
/*!
  @brief 
  return type of sbi APIs

*/
typedef rf_hal_bus_result_type rfhal_sbi_result_type;


typedef enum
{
  RFHAL_SBI_PENDING_RESULT_SUCCESS = 0,  
  RFHAL_SBI_PENDING_RESULT_FAILURE = 1     
} rfhal_sbi_pending_result_type;


// For legacy

//typedef rfhal_sbi_result_type rfd_sbi_result_type;
typedef rfhal_sbi_pending_result_type rfd_sbi_pending_result_type;
typedef enum
{
    RFD_SBI_SUCCESS = 0,
    RFD_SBI_FAILURE = 1
} rfd_sbi_result_type;

/*===========================================================================
 
                      PUBLIC FUNCTION DECLARATIONS
 
===========================================================================*/

void rfhal_sbi_init(void);

rfhal_sbi_result_type rfhal_sbi_slave_clock_off
(
  uint8 slave_id,                    //!< physical slave_id
  uint8 reg,                         //!< register to write to disable clock
  uint8 value                        //!< value to write to disable clock

);

rfhal_sbi_result_type rfhal_sbi_write_bytes
(
  uint8 slave_id,                    //!< physical slave_id
  uint8 *reg,                        //!< pointer to array of register to write to
  uint8 *value,                      //!< pointer to array of value to write to
  uint32 count                       //!< how many times to write for
);

/*! @brief Write a SSBI single byte
*/
static inline rfhal_sbi_result_type rfhal_sbi_write_byte
(
  uint8 slave_id,                    //!< physical slave_id
  uint8 reg,                         //!< register to write to
  uint8 value                        //!< value to write to
)
{
  return rfhal_sbi_write_bytes(slave_id, &reg, &value, 1);
};

rfhal_sbi_result_type rfhal_sbi_write_byte_masked
(
  uint8 slave_id,                    //!< physical slave_id
  uint8 reg,                         //!< register to write to
  uint8 value,                       //!< value to write to
  uint8 mask                         //!< mask for write value
);

rfhal_sbi_result_type rfhal_sbi_write_script
(
  rfdevice_ssbi_write_type* sbi_script, //!< Pointer to SSBI script from rf device
  uint16 script_size                    //!< Size of the script to be written to hw
);

rf_hal_bus_result_type rfhal_issue_script_generic
(
  rf_hal_bus_resource_script_type* sbi_script, //!< Pointer to generic script
  rf_hal_bus_resource_script_settings_type* settings
);

rfhal_sbi_result_type rfhal_sbi_read_byte
(
  uint8 slave_id,                    //!< physical slave_id
  uint8 reg,                         //!< register to read from
  uint8 *value                       //!< pointer to store read result to
);

rfhal_sbi_result_type rfhal_sbi_read_bytes
(
  uint8 slave_id,                    //!< physical slave_id
  uint8 *reg,                        //!< pointer to array of register to read from
  uint8 *value,                      //!< pointer to array of value to store read result to
  uint32 count                       //!< how many times to read for
);


#endif /*RF_HAL_SBI_H*/

