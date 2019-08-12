/*===========================================================================
                        mcfg_validate_i.h

DESCRIPTION

   Internal header file for modem configuration validation routines 
 
Copyright (c) 2012-2013 by QUALCOMM Technologies Incorporated.
 
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_utils/src/mcfg_validate_i.h#1 $ $DateTime: 2016/12/13 08:00:27 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
1/18/12      sbt   Create

===========================================================================*/

#ifndef MCFG_VALIDATE_I_H
#define MCFG_VALIDATE_I_H

#include "mcfg_nv.h"

/*==============================================================================
  Typedefs
==============================================================================*/
typedef struct 
{
  int32   data_len;    /**< Length of the data. */
  uint8  *data_ptr;    /**< Pointer to the data. */
} mcfg_data_len_ptr_type;

typedef struct 
{
  uint8                  *name;
  uint8                   attribute;
  uint8                   variant;
  mcfg_data_len_ptr_type *config_data;
  mcfg_data_len_ptr_type *efs_data;
} mcfg_validate_delta_entry;

/*==============================================================================
  Variables
==============================================================================*/


/*============================================================================== 
 
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
 
==============================================================================*/
boolean mcfg_validate_xml_start
(
  mcfg_config_type_e_type  type,
  uint32                   mcfg_version,
  uint32                   base_version
);

boolean mcfg_validate_xml_update
(
  mcfg_validate_delta_entry *entry
);

boolean mcfg_validate_xml_complete
(
  void
);

void mcfg_validate_xml_ut
(
  void
);

boolean mcfg_validate_adjust_xml_data_buf_size
(
  mcfg_data_len_ptr_type *data,
  uint8 attrib
);

boolean mcfg_validate_nvefs_data_match
(
  mcfg_data_len_ptr_type *config_data,
  mcfg_data_len_ptr_type *efs_data
);

boolean mcfg_cfg_diff_nvefs
(
  uint8      cfg_type, 
  uint32     num_mcfg_items, 
  uint8*     ram_cust_data_ptr, 
  uint8      subs_id, 
  boolean    validate
);
#endif /* MCFG_VALIDATE_I_H */
