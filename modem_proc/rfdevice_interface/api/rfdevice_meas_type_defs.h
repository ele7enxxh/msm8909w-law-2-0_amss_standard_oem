/*!
  @file rfdevice_meas_type_defs.h

  @brief
  Provides type definitions needed by the rfdevice_interface.
 
  @details
  This file contains the type definitions used by the rfdevice interface
  functions.

*/

/*============================================================================== 
   
  Copyright (c) 2009 - 2010 Qualcomm Technologies Incorporated. All Rights Reserved 
   
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
 
$DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_meas_type_defs.h#1 $ 
   
when       who     what, where, why 
--------   ---     ------------------------------------------------------------- 
03/07/14   hm      FW Decoupling
09/22/11  tnt      Fix KW
08/18/10   pl      Move script generation macro to internal file
08/07/10   pl      fix macro bug
07/28/10   pl      Added rtr8600 default image support
07/22/10   pl      Initial version.
==============================================================================*/ 

#ifndef RFDEVICE_MEAS_TYPE_DEFS_H
#define RFDEVICE_MEAS_TYPE_DEFS_H

/*============================================================================== 
 
                                 INCLUDE FILES
 
==============================================================================*/
#include "comdef.h"

/*============================================================================== 
 
                               MACROS AND TYPEDEFS 
 
==============================================================================*/
#define RFDEVICE_MEAS_MAX_NUM_OF_RTR_SBI 300

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This structure defines the structure of a RTR Script entry
*/
typedef struct
{
   int16     start_delta; /*!< defines the units of time when the scrip entry should be executed */
   uint8     data;        /*!< the value to be written to the register */
   uint8     addr;        /*!< the address of the register to be written */
   uint8     iq_mode;     /*!< SBI mode control */
   uint8     sbi_bus;     /*!< The SSBI bus to be used */
} rfdevice_meas_script_entry_type;

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This structure defines the structure of a RTR Script entry

   @param rtr_sbi_counter 
   @param rtr_buffer      
*/
typedef struct
{
  uint16                           rtr_sbi_counter;  /*!< The number of entries in the script */

  rfdevice_meas_script_entry_type  rtr_buffer[ RFDEVICE_MEAS_MAX_NUM_OF_RTR_SBI ];
  /*!< The buffer for the RTR script */
} rfdevice_meas_script_type;

#endif /* RFDEVICE_MEAS_TYPE_DEFS_H */
