/*===========================================================================
      COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/gsclassmark.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/07   kgj     Created
===========================================================================*/

#ifndef GSCLASSMARK_H
#define GSCLASSMARK_H

#include "comdef.h"



typedef enum
{
  GSCLASSMARK_POS_NETWORK_BASED = 0,
  GSCLASSMARK_POS_UE_BASED,
  GSCLASSMARK_POS_BOTH,
  GSCLASSMARK_POS_NONE
} gsclassmark_cgps_pos_support_enum_type;

typedef struct
{
  boolean   standalone_location_method_supported;
  boolean   ue_based_otdoa_supported;
  boolean   gps_timing_of_cell_frames_supported;
  boolean   ipdl_supported;
  boolean   rx_tx_type2_supported;
  boolean   up_meas_validity_cell_pch_and_ura_pch;
  boolean   lcs_value_added_lcs_supported;

  gsclassmark_cgps_pos_support_enum_type    cgps_pos_support;
} gsclassmark_cgps_pos_capability_struct_type;

typedef void (*cgps_classmark_cb_type) (gsclassmark_cgps_pos_capability_struct_type  *p_pos_capability );

/*
 *  This function is used by CGPS to register a callback to be used by
 *  GSCLASSMARK to obtain the UE Positioning Capability Information.
 */
void gsclassmark_register_cgps_cb( cgps_classmark_cb_type   cb_fp );



#endif  /* GSCLASSMARK_H */
