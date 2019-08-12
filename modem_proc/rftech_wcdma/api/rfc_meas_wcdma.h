#ifndef RFC_MEAS_WCDMA_H
#define RFC_MEAS_WCDMA_H

/*!
  @file
  rfc_meas_wcdma.h

  @brief
  This module contains prototypes for rfc meas functions for WCDMA */

/*==============================================================================
Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/02/14   kr      Update GRFC Tx scripts for W2G tune back scripts in tx mode.
07/19/11   whc     Initial Revision.

==============================================================================*/

#include "rfc_common.h"
#include "rf_buffer_intf.h"

/*This function generates a GRFC script from WCDMA RFC signal lists for IRAT purposes */
void rfc_meas_wcdma_generate_grfc_script(uint32 band, rfc_grfc_device_type  *device_info, rf_buffer_intf *grfc_buf_ptr, boolean status);

#endif /*RFC_MEAS_WCDMA_H*/
