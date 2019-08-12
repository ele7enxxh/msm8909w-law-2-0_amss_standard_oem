#ifndef RFC_CARD_MEAS_H
#define RFC_CARD_MEAS_H
/*!
  @file
  rfc_card_meas.h

  @brief
  This file contains functions to support the RF Card (RFC) module which
  manages the RF HW configuration.

*/

/*===========================================================================

Copyright (c) 2010 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/meas/rf/rfc/inc/rfc_card_meas.h#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
10/02/14   kr      Update GRFC Tx scripts for W2G tune back scripts in tx mode.
03/21/14   ra      Support FW cleanup of deprecated files  
07/19/11   whc     Remove old/unused IRAT APIs
07/15/11   whc     IRAT GRFC script support for autogen RFC  
07/11/11   whc     Add API stub for rfc_meas_gen_grfc_script 
06/10/11   tnt     Add support for new GRFC info passing for IRAT
08/18/10   pl      Update GRFC build script to support timing information
07/22/10   pl      Initial release

============================================================================*/
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "rfcom.h"
#include "rfc_common.h"

/*============================================================================== 
 
                               MACROS AND TYPEDEFS 
 
==============================================================================*/
/*Deprecated use of _MEAS_ version*/
#define RFC_MEAS_MAX_NUM_OF_GRFC  RFC_MAX_NUM_OF_GRFC

/*----------------------------------------------------------------------------*/

/*Deprecated these structs, use non-meas copy in rfc_common.h*/
typedef rfc_grfc_script_entry_type rfc_meas_grfc_script_entry_type;
typedef rfc_grfc_script_type rfc_meas_grfc_script_type;


/*============================================================================== 
 
                             FUNCTION DECLARATIONS
 
==============================================================================*/


/*----------------------------------------------------------------------------*/
void rfc_meas_generate_grfc_script
( 
  rfm_mode_enum_type mode,
  uint32             band,
  rfc_grfc_device_type  *device_info,
  rf_buffer_intf *grfc_buf_ptr,
  boolean status
);

#endif /* RFC_CARD_MEAS_H */

