#ifndef MMGSDI_EONS_NITZ_H
#define MMGSDI_EONS_NITZ_H
/*===========================================================================


                  M M G S D I   E O N S   S S   E V E N T   A N D   N I T Z
                                      H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2013 - 2014, 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_eons_nitz.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/24/16   bcho    EONS F3 reduction
12/03/14   bcho    featurize support to use stored rplmn name across operators
08/27/14   tl      Introduce network related SIM Lock slot policies
12/19/13   yt      Support for new CM SS event interface
12/05/13   bcho    Re-run the EONS algorithm when PLMNID/LAC/RAT is changed
10/21/13   vdc     Create RPLMN info text file when EONS algo enters NITZ step
10/03/13   vdc     Improvements in NITZ storage in EFS
07/15/13   vs      EONS updates to support single baseband SGLTE

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "comdef.h"
#include "cm.h"
#include "mmgsdi.h"
#include "mmgsdi_eons.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

/* Number of entries in PLMN info array
   The storage of number of PLMN entries is ME specific and it is assumed to
   have 3 PLMN entries per subscription */
#define MMGSDI_MAX_PLMN_NAMES 0x0C

/* Maximum number of subscriptions possible */
#define MMGSDI_EONS_MAX_SUBS 0x04

/* Long & short name lengths - derived based on CM constants */
#define MMGSDI_EONS_NITZ_PLMN_FULL_NAME_LENGTH SYS_MAX_FULL_NAME_CHARS
#define MMGSDI_EONS_NITZ_PLMN_SHORT_NAME_LENGTH SYS_MAX_SHORT_NAME_CHARS

/* Session bit masks
   These bit mask are used for the following purpose:
   1) To determine which entry has the latest PLMN info for a subscription
   2) If we receive same PLMN info on different subs then we can OR the mask*/
#define MMGSDI_EONS_NITZ_GW_NO_MASK               0x00
#define MMGSDI_EONS_NITZ_GW_PRI_MAIN_MASK         0x01
#define MMGSDI_EONS_NITZ_GW_PRI_SEC_MASK          0x02
#define MMGSDI_EONS_NITZ_GW_SEC_MAIN_MASK         0x04
#define MMGSDI_EONS_NITZ_GW_TER_MAIN_MASK         0x08

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_NITZ_RPLMN_PARAMS_TYPE

   DESCRIPTION:
     The structure contains the RPLMN name parameters
-------------------------------------------------------------------------------*/

typedef struct
{
  boolean                        plmn_name_available;
  boolean                        plmn_name_ci;
  uint8                          plmn_name_spare_bits;
  mmgsdi_eons_encoding_enum_type eons_encoding;
}mmgsdi_nitz_rplmn_name_params_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_NITZ_RPLMN_FULL_NAME_TYPE

   DESCRIPTION:
     The structure contains the RPLMN full name, its parameters and its length
-------------------------------------------------------------------------------*/

typedef struct
{
  mmgsdi_nitz_rplmn_name_params_type full_name_params;
  uint8                              full_name_chars[MMGSDI_EONS_NITZ_PLMN_FULL_NAME_LENGTH];
  uint8                              full_name_length;
}mmgsdi_nitz_rplmn_full_name_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_NITZ_RPLMN_SHORT_NAME_TYPE

   DESCRIPTION:
     The structure contains the RPLMN short name, its parameters and its length
-------------------------------------------------------------------------------*/


typedef struct
{
  mmgsdi_nitz_rplmn_name_params_type short_name_params;
  uint8                              short_name_chars[MMGSDI_EONS_NITZ_PLMN_SHORT_NAME_LENGTH];
  uint8                              short_name_length;
}mmgsdi_nitz_rplmn_short_name_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_NITZ_RPLMN_ENTRY_TYPE

   DESCRIPTION:
     The structure represents an entry in the RPLMN info array. Each entry
     contains the PLMN ID, full name, short name and session bit mask.
     session_bit_mask determines which subscription this entry belongs to.
-------------------------------------------------------------------------------*/

typedef struct
{
  mmgsdi_plmn_id_type               plmn_id;
  uint16                            lac;
  mmgsdi_nitz_rplmn_full_name_type  full_name;
  mmgsdi_nitz_rplmn_short_name_type short_name;
  uint8                             session_bit_mask;
  mmgsdi_plmn_id_type               hplmn_id;
}mmgsdi_nitz_rplmn_entry_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_NITZ_RPLMN_INFO_TYPE

   DESCRIPTION:
     The structure contains the RPLMN info with multiple entries of PLMN info,
     previous PLMN index which stores the index of the entry that has non-
     zero bitmask for a subscription
     start_index determines the starting position of the array for replacement
     or writing PLMN info into an entry of RPLMN info array
-------------------------------------------------------------------------------*/

typedef struct
{
  uint8                          start_index;
  uint8                          plmn_info_index[MMGSDI_EONS_MAX_SUBS];
  mmgsdi_nitz_rplmn_entry_type  *nitz_rplmn_entry_ptr[MMGSDI_MAX_PLMN_NAMES];
}mmgsdi_nitz_rplmn_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_NITZ_EFS_RPLMN_INFO_TYPE

   DESCRIPTION:
     The structure contains the RPLMN info with multiple entries of PLMN info,
     previous PLMN index which stores the index of the entry that has non-
     zero bitmask for a subscription
     start_index determines the starting position of the array for replacement
     or writing PLMN info into an entry of RPLMN info array
-------------------------------------------------------------------------------*/

typedef struct
{
  uint8                          start_index;
  uint8                          plmn_info_index[MMGSDI_EONS_MAX_SUBS];
  mmgsdi_nitz_rplmn_entry_type   nitz_rplmn_entry[MMGSDI_MAX_PLMN_NAMES];
}mmgsdi_nitz_efs_rplmn_info_type;


/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/
/*==========================================================================
FUNCTION MMGSDI_EONS_GET_NITZ_ONS

DESCRIPTION
  Function to get operator name string from NITZ data

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
boolean mmgsdi_eons_get_nitz_ons(
  mmgsdi_eons_call_stack_enum_type    stack_type,
  mmgsdi_session_type_enum_type       session_type,
  mmgsdi_eons_info_type             * eons_info_ptr,
  mmgsdi_plmn_info_type             * plmn_info_ptr,
  mmgsdi_plmn_id_type                 hplmn_id
);

/*===========================================================================
FUNCTION MMGSDI_EONS_FREE_NITZ_DATA_PTRS

DESCRIPTION
  Function to free data in RPLMN pointers

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_eons_free_nitz_data_ptrs(
  mmgsdi_eons_nitz_data_type  * nitz_info_ptr
);

/*===========================================================================
  FUNCTION MMGSDI_EONS_MATCH_RPLMN_NAME_WITH_NITZ_NAME

DESCRIPTION
  Function to determine if EONS algo needs to be run and event needs to be sent
  for a call stack

DEPENDENCIES
  None

RETURN VALUE
  Boolean TRUE if RPLMN name matches NITZ name and FALSE if not

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_eons_match_rplmn_name_with_nitz_name(
  const mmgsdi_eons_per_stack_rplmn_info_type  * rplmn_info_ptr,
  const mmgsdi_eons_nitz_data_type             * nitz_info_ptr
);

/*===========================================================================
  FUNCTION MMGSDI_EONS_NITZ_UPDATE_RPLMN_IDENTIFIERS

DESCRIPTION
  Function to copy RPLMN identifiers

DEPENDENCIES
  None

RETURN VALUE
  Boolean indicating whether RPLMN identifiers were updated

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_eons_nitz_update_rplmn_identifiers(
  mmgsdi_eons_info_type   * eons_info_ptr,
  boolean                 * first_stack_rplmn_updated_ptr,
  boolean                 * sec_stack_rplmn_updated_ptr,
  boolean                 * fs_plmn_id_lac_rat_changed_ptr,
  boolean                 * ss_plmn_id_lac_rat_changed_ptr
);

/*===========================================================================
  FUNCTION MMGSDI_EONS_NITZ_CHECK_RUN_ALGO_SEND_EVT_NEEDED

DESCRIPTION
  Function to determine if EONS algo needs to be run and event needs to be sent
  for a call stack

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_eons_nitz_check_run_algo_send_evt_needed(
  const mmgsdi_eons_per_stack_ss_data_type * ss_info_ptr,
  boolean                                    rplmn_updated,
  mmgsdi_eons_per_stack_rplmn_info_type    * rplmn_info_ptr,
  boolean                                  * run_algo_ptr,
  boolean                                  * send_evt_ptr
);

 /*==========================================================================
 FUNCTION MMGSDI_EONS_STORE_NITZ_INFO

 DESCRIPTION
   Function to write rplmn_info.txt with cached data on reception of TASK_OFFLINE/TASK_STOP

 DEPENDENCIES
   None

 RETURN VALUE
   MMGSDI_RETURN_ENUM_TYPE

 SIDE EFFECTS
   None
 ==========================================================================*/
 mmgsdi_return_enum_type mmgsdi_eons_store_nitz_info (void);

#endif /* MMGSDI_EONS_NITZ_H */
