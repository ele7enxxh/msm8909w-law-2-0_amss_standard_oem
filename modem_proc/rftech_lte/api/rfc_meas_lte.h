#ifndef RFC_MEAS_LTE_H
#define RFC_MEAS_LTE_H

/*!
  @file
  rfc_meas_lte.h

  @brief
  This module contains prototypes for rfc meas functions for LTE */

/*==============================================================================
Copyright (c) 2011 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/api/rfc_meas_lte.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
07/14/11   whc     Initial Revision.

==============================================================================*/

#include "rfc_common.h"
/*This function generates a GRFC script from LTE RFC signal lists for IRAT purposes */
void rfc_meas_lte_generate_grfc_script(uint32 band, rfc_grfc_device_type  *device_info, rfc_grfc_script_type *grfc_buf_ptr);




#endif /*RFC_MEAS_LTE_H*/
