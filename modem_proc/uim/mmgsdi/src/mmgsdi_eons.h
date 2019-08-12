#ifndef MMGSDI_EONS_H
#define MMGSDI_EONS_H
/*===========================================================================


                    M M G S D I   E O N S   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2011 - 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_eons.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
09/30/15   ar      Fake the fourth byte of EF AD for buggy card
12/03/14   bcho    featurize support to use stored rplmn name across operators
08/27/14   tl      Introduce network related SIM Lock slot policies
06/02/14   bcho    Get PLMN from mobility mgnt info if MOBILITY_MGMT_MASK is set
03/24/14   bcho    Run EONS algorithm when SYS MODE MASK is set
02/06/14   hh      For API calls, populate ONS when EF-SPN is absent or SPN is disabled
12/24/13   bcho    Handle CM SS event for limited regional service also
12/19/13   yt      Support for new CM SS event interface
12/18/13   hh      For API calls, populate SPN and ONS only when condition allows
10/22/13   bcho    Use roaming status received in CM SS event in EONS algorithm
10/17/13   hh      Added MMGSDI_EONS_3GPP_SS_GW_INFO_CHANGED for hybr_gw stacks
09/27/13   am      Prevent potential overflow of var_len
09/20/13   av      Fix buffer overflows
08/30/13   av      Support for Chinese language PLMN names
08/22/13   vdc     Added support for TSTS
07/15/13   vs      EONS updates to support single baseband SGLTE
07/03/13   bcho    PLMN compare fn modified according to Annex A 3GPP TS 23122
07/01/13   at      Support for additional info in EONS APIs
06/28/13   bcho    APIs added to return plmn info from SE13 table.
05/08/13   bcho    Added support to provide rplmn-spn display bit and network
                   info in get operator name API cnf and in EONS event
05/01/13   at      Memory optimization on the static SE13 table entry struct
04/04/13   bcho    Update SE13 table and modify its lookup algorithm
03/21/13   bcho    Send EONS event to session who registered late
01/20/13   vv      Added EONS support for the CSG
09/20/12   bcho    Added support for mmgsdi_get_SE13_plmn_names API
09/11/12   bcho    Ignore display condition param added in mmgsdi_eons_run_algo
09/03/12   bcho    Function prototypes added to support raw caching and to send
                   eons cache ready event
04/12/12   bcho    EONS Event(triggered by SS Evt) supported on test framework
01/30/12   bcho    Implementation of get operator name API, EONS event modified
12/21/11   bcho    Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "comdef.h"
#include "cm.h"
#include "mmgsdi.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#define MMGSDI_EONS_FILE_HEADER_LEN             24
#define MMGSDI_EONS_FILE_HEADER_INFO_LEN         8
#define MMGSDI_EONS_FILE_HEADER_SEC_INFO_OFFSET  8
#define MMGSDI_EONS_FILE_HEADER_TER_INFO_OFFSET 16
#define MMGSDI_EONS_FILE_HEADER_DATA_LEN_OFFSET  4
#define MMGSDI_EONS_FILE_HEADER_PLMN_OFFSET      1
#define MMGSDI_EONS_FILE_HEADER_FLAG_OFFSET      0
#define MMGSDI_EONS_FILE_DATA_NAME_FLAG_OFFSET   0
#define MMGSDI_EONS_FILE_DATA_NAME_CI_OFFSET     1
#define MMGSDI_EONS_FILE_DATA_SPARE_BIT_OFFSET   2
#define MMGSDI_EONS_FILE_DATA_NAME_ENC_OFFSET    3
#define MMGSDI_EONS_FILE_DATA_NAME_LEN_OFFSET    7
#define MMGSDI_EONS_FILE_DATA_NAME_POS_OFFSET    8

/* Min valid SPDI length: 2 Tag bytes + 2 len bytes +
   3 bytes of mandatory PLMN id*/
#define MMGSDI_EONS_SPDI_MIN_VALID_LEN          7
#define MMGSDI_EONS_SPDI_PLMN_LIST_TAG       0x80
#define MMGSDI_EONS_SPDI_DISP_INFO_TAG       0xA3

#define MMGSDI_EONS_SPN_DATA_LEN               16
#define MMGSDI_EONS_SPN_DISP_COND_POS           0
#define MMGSDI_EONS_SPN_DATA_BUF_POS            1

#define MMGSDI_EONS_CDMA_SPN_DATA_LEN          32
#define MMGSDI_EONS_CDMA_SPN_ENC_POS            1
#define MMGSDI_EONS_CDMA_SPN_LANG_IND_POS       2
#define MMGSDI_EONS_CDMA_SPN_DATA_BUF_POS       3
#define MMGSDI_EONS_CDMA_SPN_ENC_VALID       0x1F
#define MMGSDI_EONS_CDMA_SPN_DISP_COND       0x01

#define MMGSDI_EONS_MCC_MNC_STR_LEN             8
#define MMGSDI_EONS_PLMN_WILD_CARD_VAL       0x0D

/*Display of rplmn is not required when it is in home N/W and bit1 is 0*/
#define MMGSDI_EONS_DISP_COND_RPLMN_BIT_POS  0x01

/* Display of SPN is not required when it is not in home N/W and bit2 is 1*/
#define MMGSDI_EONS_DISP_COND_SPN_BIT_POS    0x02

/* Max length of PLMN long/short name. */
#define MMGSDI_EONS_MAX_PLMN_NAME_LEN        0xFF

/* Max length of PLMN long/short name from network. Per TS44.006:
  The number of octets in a L3 message passed to layer 2 for transmission in
  acknowledged mode shall not exceed 251, due to corresponding limits in 3GPP
  TS 48.056.. so, we can safely mark 255 as max length for PLMN name.
  So, adding 255 each of short and long name, and 22 bytes for the rest of
  the data such as encoding scheme, lets mark 550 as the max length for
  PLMN data */
#define MMGSDI_EONS_NITZ_PLMN_DATA_MAX_LEN    550

#define MMGSDI_EONS_MAX_UCS2_CODES           14

/* Populate PLMN name when:
   NOT RPLMN
   or
   RPLMN && (ignore_cond == MMGSDI_EONS_IGNORE_XXX_RPLMN_DISPLAY_COND)
   or
   (!RPLMN_HPLMN_SPDI) && (ignore_cond != MMGSDI_EONS_IGNORE_XXX_RPLMN_DISPLAY_COND)
   or
   RPLMN_HPLMN_SPDI && (rplmn_display_bit == MMGSDI_EONS_RPLMN_DISPLAY_BIT_NOT_AVAILABLE)
   or
   RPLMN_HPLMN_SPDI && (rplmn_display_bit == MMGSDI_EONS_RPLMN_DISPLAY_BIT_VAL_1)
   */
/* Spec: TS 131.102 EFSPN bit 1: applies to RPLMN that is either HPLMN or SPDI PLMN
   For conditions the spec does not apply, always populate
   To determine whether to populate SPN, we check:
   1. IF the PLMN is NOT RPLMN, populate
   2. otherwise, if ignore_cond is to ignore RPLMN condition, populate
   3. otherwise, if RPLMN is neither HPLMN nor SPDI PLMN, populate
   4. otherwise, if spn_display_bit is MMGSDI_EONS_RPLMN_DISPLAY_BIT_NOT_AVAILABLE, populate
   5. otherwise, if spn_display_bit is MMGSDI_EONS_RPLMN_DISPLAY_BIT_VAL_1, populate
   */
#define MMGSDI_EONS_POPULATE_PLMN_NAME(is_rplmn, is_hplmn_spdi, ignore_cond, rplmn_display_bit) \
          (!(is_rplmn)                                                          ||              \
           (MMGSDI_EONS_IGNORE_SPN_RPLMN_DISPLAY_COND   == (ignore_cond))       ||              \
           (MMGSDI_EONS_IGNORE_RPLMN_DISPLAY_COND       == (ignore_cond))       ||              \
           !(is_hplmn_spdi)                                                     ||              \
           (MMGSDI_EONS_RPLMN_DISPLAY_BIT_NOT_AVAILABLE == (rplmn_display_bit)) ||              \
           (MMGSDI_EONS_RPLMN_DISPLAY_BIT_VAL_1         == (rplmn_display_bit)))
/*
   or
   RPLMN && (ignore_cond == MMGSDI_EONS_IGNORE_SPN_XXX_DISPLAY_COND)
   or
   (!RPLMN_HPLMN_SPDI) && (ignore_cond != MMGSDI_EONS_IGNORE_XXX_RPLMN_DISPLAY_COND)

*/
/* Populate SPN name when:
   NOT RPLMN
   or
   RPLMN && (ignore_cond == MMGSDI_EONS_IGNORE_SPN_XXX_DISPLAY_COND)
   or
   RPLMN_HPLMN_SPDI && (ignore_cond != MMGSDI_EONS_IGNORE_SPN_XXX_DISPLAY_COND)
   or
   (!RPLMN_HPLMN_SPDI) && (spn_display_bit == MMGSDI_EONS_SPN_DISPLAY_BIT_VAL_0)
   */
/* Spec: TS 131.102 EFSPN bit 2: applies to RPLMN that is neither HPLMN nor SPDI PLMN
   For conditions the spec does not apply, always populate
   To determine whether to populate SPN, we check:
   1. IF the PLMN is NOT RPLMN, populate
   2. otherwise, if ignore_cond is to ignore SPN conditions, populate
   3. otherwise, if RPLMN is HPLMN or SPDI PLMN, populate
   4. otherwise, if spn_display_bit is MMGSDI_EONS_SPN_DISPLAY_BIT_VAL_0, populate
   */
#define MMGSDI_EONS_POPULATE_SPN(is_rplmn, is_hplmn_spdi, ignore_cond, spn_display_bit) \
          (!(is_rplmn)                                                        ||        \
           (MMGSDI_EONS_IGNORE_SPN_RPLMN_DISPLAY_COND == (ignore_cond))       ||        \
           (MMGSDI_EONS_IGNORE_SPN_DISPLAY_COND       == (ignore_cond))       ||        \
           (is_hplmn_spdi)                                                    ||        \
           (MMGSDI_EONS_SPN_DISPLAY_BIT_VAL_0         == (spn_display_bit)))

/*Check validity of PLMN ID*/
#define MMGSDI_EONS_PLMN_ID_VALIDITY_CHECK(plmn_val)  \
          (((plmn_val[0] & 0x0F) != 0x0F) &&          \
           ((plmn_val[0] & 0xF0) != 0xF0) &&          \
           ((plmn_val[1] & 0x0F) != 0x0F) &&          \
           ((plmn_val[2] & 0x0F) != 0x0F) &&          \
           ((plmn_val[2] & 0xF0) != 0xF0))

/* ----------------------------------------------------------------------------

   ENUM:      MMGSDI_EONS_TRIGGER_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the EONS method
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_EONS_TRIGGER_API_CALL,
  MMGSDI_EONS_TRIGGER_SS_EVENT,
  MMGSDI_EONS_TRIGGER_REFRESH_FCN,
  MMGSDI_EONS_TRIGGER_OPL_PNN_CACHE,
  MMGSDI_EONS_TRIGGER_REFRESH_MAX
} mmgsdi_eons_trigger_enum_type;

/* ----------------------------------------------------------------------------

   ENUM:      MMGSDI_EONS_CALL_STACK_ENUM_TYPE

   DESCRIPTION:
     This enum specifies stack for which an EONS operation is being performed
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_EONS_FIRST_CALL_STACK,
  MMGSDI_EONS_SEC_CALL_STACK,
  MMGSDI_EONS_MAX_CALL_STACK
} mmgsdi_eons_call_stack_enum_type;

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

/* ------------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_EONS_NETWORK_INFO_DATA_TYPE

   DESCRIPTION:
     Define a type that contains the networks, country code, network code and name.
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint16   mcc; /* Mobile Network Code */
  uint16   mnc; /* Mobile Country Code */
  uint8    network_type_mask;
  char   * short_name_ptr;
   /* Pointer to a null terminated string containing the
         network's short name.*/
  char   * full_name_ptr;
  /* Pointer to a null terminated string containing the
        network's full name. */
  boolean  gsma_entry; /* Entry is available in GSMA SE13 table */
} mmgsdi_eons_network_info_data_type;

/* ------------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_EONS_NETWORK_INFO_UCS16_DATA_TYPE

   DESCRIPTION:
     Define a type that contains the networks, country code, network code and
     name in 16 bit unicode format.
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  char*   lang_id; /* Languages ID string */
  uint16  mcc; /* Mobile Network Code */
  uint16  mnc; /* Mobile Country Code */
  uint8   network_type_mask; /* Type of the network */
  uint8   short_name_ucs2_len;
  uint8   short_name_ucs2[MMGSDI_EONS_MAX_UCS2_CODES];
  /* network's short name in 16-bit unicode format */
  uint8   full_name_ucs2_len;
  uint8   full_name_ucs2[MMGSDI_EONS_MAX_UCS2_CODES];
  /* network's full name in 16-bit unicode format */
} mmgsdi_eons_network_info_addl_lang_data_type;

/*=============================================================================

                                   MMGSDI MACROS

=============================================================================*/

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/
/*==========================================================================
FUNCTION: MMGSDI_EONS_RUN_ALGO

DESCRIPTION:
  Function to trigger EONS algorithmn

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_run_algo(
  mmgsdi_session_id_type                         session_id,
  mmgsdi_eons_trigger_enum_type                  eons_trigger,
  mmgsdi_plmn_id_list_type                       plmn_id_list,
  mmgsdi_eons_ignore_disp_cond_enum_type         ignore_display_condition,
  mmgsdi_eons_name_type                        * spn_ptr,
  uint8                                        * lang_ind_ptr,
  mmgsdi_list_plmn_name_type                   * plmn_name_list_ptr,
  mmgsdi_eons_display_info_type                * display_info_ptr
);

/*===========================================================================
FUNCTION MMGSDI_EONS_SEND_LAST_EVENT

DESCRIPTION
  Function to send EONS event to provisioning session which registered late
  i.e. after session registration check whether EONS event is sent to this session type.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_eons_send_last_event(
  mmgsdi_session_id_type        session_id
);

/*===========================================================================
FUNCTION MMGSDI_EONS_SEND_EVENT

DESCRIPTION
  Function to send EONS event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_eons_send_event(
  mmgsdi_session_id_type        session_id,
  mmgsdi_eons_trigger_enum_type eons_trigger,
  boolean                       csg_ef_refreshed
);

/*===========================================================================
FUNCTION MMGSDI_EONS_SEND_CACHE_READY_EVENT

DESCRIPTION
  Function to send EONS cache ready event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_eons_send_cache_ready_event(
  mmgsdi_session_id_type        session_id
);

/*===========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_UPDATE_RECORD

DESCRIPTION
  Function to update record data in OPL-PNN cache

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_opl_pnn_update_record(
  mmgsdi_session_id_type      session_id,
  mmgsdi_file_enum_type       file_enum,
  mmgsdi_data_type            data,
  mmgsdi_rec_num_type         rec_num,
  mmgsdi_cache_init_enum_type cache_state
);

/*===========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_DELETE_CACHE_ALL

DESCRIPTION
  Function to delete OPL-PNN cache associated with GW PRI or GW SEC session

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_opl_pnn_delete_cache_all(
  mmgsdi_session_id_type  session_id
);

/*===========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_READ_RECORD

DESCRIPTION
  Function to read record from OPL-PNN cache

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_opl_pnn_read_record(
  mmgsdi_session_id_type       session_id,
  mmgsdi_file_enum_type        file_enum,
  mmgsdi_len_type              data_len,
  mmgsdi_rec_num_type          rec_num,
  mmgsdi_data_type            *record_data_ptr,
  mmgsdi_cache_init_enum_type *cache_state_ptr
);

/*===========================================================================
FUNCTION MMGSDI_EONS_PROCESS_GET_SE13_PLMN_NAMES

DESCRIPTION
  This function queues a response for handling get se13 plmn names in the cnf

PARAMETERS:
  mmgsdi_get_se13_plmn_names_req_type req_ptr: request pointer

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_process_get_se13_plmn_names(
  mmgsdi_get_se13_plmn_names_req_type * req_ptr
);

/* ============================================================================
   FUNCTION       MMGSDI_EONS_GET_SE13_PLMN_NAMES

   DESCRIPTION
     This function returns plmn names from SE13 table for provided plmn id list.

   PARAMETERS:
     mmgsdi_plmn_id_list_type plmn_id_list: list of PLMN ids
     mmgsdi_list_plmn_name_type plmn_name_list_ptr: list of plmn long/short name

   DEPENDENCIES:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS: if plmn names are retrieved successfully.

     MMGSDI_ERROR: any error

   SIDE EFFECTS:
     None
===============================================================================*/
mmgsdi_return_enum_type  mmgsdi_eons_get_se13_plmn_names (
  mmgsdi_plmn_id_list_type      plmn_id_list,
  mmgsdi_list_plmn_name_type  * plmn_name_list_ptr
);

/*==========================================================================
FUNCTION MMGSDI_EONS_COMPARE_SIM_PLMN_WITH_NW_PLMN

DESCRIPTION
  Function to compare SIM PLMN ID with PLMN ID received from network.
  This follows Annex A 3GPP TS 23122 for PLMN comparison.

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
boolean mmgsdi_eons_compare_sim_plmn_with_nw_plmn(
  mmgsdi_plmn_id_type sim_plmn,
  mmgsdi_plmn_id_type nw_plmn
);

/*==========================================================================
FUNCTION MMGSDI_EONS_PLMN_MATCH

DESCRIPTION
  Function to compare two PLMN IDs

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
boolean mmgsdi_eons_plmn_match(
  mmgsdi_plmn_id_type plmn_id_1,
  mmgsdi_plmn_id_type plmn_id_2
);

/* ============================================================================
FUNCTION       MMGSDI_EONS_GET_SE13_PLMN_INFO_BY_NAME

DESCRIPTION
  This function returns plmn info from SE13 table for given long/short name

PARAMETERS:
  mmgsdi_get_se13_plmn_info_by_name_req_type
    se13_plmn_info_by_name_req_ptr: long name, short name and RAT info
  mmgsdi_get_se13_plmn_info_by_name_cnf_type se13_plmn_info_by_name_cnf_ptr:
    cnf data ptr

DEPENDENCIES:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

  MMGSDI_SUCCESS: if plmn info is retrieved successfully.

  MMGSDI_ERROR: any error

SIDE EFFECTS:
  None
===============================================================================*/
mmgsdi_return_enum_type  mmgsdi_eons_get_se13_plmn_info_by_name(
  const mmgsdi_get_se13_plmn_info_by_name_req_type *se13_plmn_info_by_name_req_ptr,
  mmgsdi_get_se13_plmn_info_by_name_cnf_type       *se13_plmn_info_by_name_cnf_ptr
);

/* ============================================================================
FUNCTION       MMGSDI_EONS_GET_SE13_PLMN_INFO_BY_INDEX

DESCRIPTION
  This function returns plmn info from SE13 table for given index

PARAMETERS:
  uint16                        index                : SE13 table index
  mmgsdi_se13_network_info_type se13_network_info_ptr: network info ptr

DEPENDENCIES:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

  MMGSDI_SUCCESS: if plmn info is retrieved successfully.

  MMGSDI_ERROR: any error

SIDE EFFECTS:
  None
===============================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_get_se13_plmn_info_by_index(
  uint16                          index,
  mmgsdi_se13_network_info_type * se13_network_info_ptr
);

/*===========================================================================
FUNCTION MMGSDI_EONS_FREE_RPLMN_INFO_PTRS

DESCRIPTION
  Function to free pointers in RPLMN info

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_eons_free_rplmn_info_ptrs(
  mmgsdi_eons_per_stack_rplmn_info_type  * rplmn_info_ptr
);

/*===========================================================================
  FUNCTION MMGSDI_EONS_COPY_PLMN_NAME_INFO

DESCRIPTION
  Utility function to copy PLMN name info

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_copy_plmn_name_info(
  const mmgsdi_plmn_name_type  * in_plmn_ptr,
  mmgsdi_plmn_name_type        * out_plmn_ptr,
  boolean                        allocate_mem
);

/*===========================================================================
FUNCTION MMGSDI_EONS_MAP_NETWORK_CODING_SCHEME

DESCRIPTION
  Function to map network coding scheme to mmgsdi operator name coding scheme

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_OPERATOR_NAME_ENCODING_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_eons_encoding_enum_type mmgsdi_eons_map_network_coding_scheme(
  sys_network_name_coding_scheme_type_e_type network_encoding
);

/*===========================================================================
FUNCTION MMGSDI_EONS_MAP_SYS_MODE_TO_RAT_TYPE

DESCRIPTION
  Function to map system mode to radio access technology

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RAT_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_rat_enum_type mmgsdi_eons_map_sys_mode_to_rat_type(
  sys_sys_mode_e_type              sys_mode
);

/*==========================================================================
FUNCTION MMGSDI_EONS_COPY_PLMN_NAME_TO_EFS_DATA

DESCRIPTION
  Function to copy plmn name to efs data buffer

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
void mmgsdi_eons_copy_plmn_name_to_efs_data(
  const mmgsdi_eons_nitz_data_type  * nitz_info_ptr,
  uint8                             * buffer_ptr,
  uint32                              buffer_len
);

/*===========================================================================
FUNCTION MMGSDI_EONS_MAP_RAT_TYPE_TO_SYS_MODE

DESCRIPTION
  Function to map  radio access technology to system mode

DEPENDENCIES
  None

RETURN VALUE
  SYS_SYS_MODE_E_TYPE

SIDE EFFECTS
  None
===========================================================================*/
sys_sys_mode_e_type mmgsdi_eons_map_rat_type_to_sys_mode(
  mmgsdi_rat_enum_type              rat_type
);

/*==========================================================================
FUNCTION MMGSDI_EONS_PLMN_GET_MCC_MNC

DESCRIPTION
  Function to get MCC and MNC values from PLMN ID

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_plmn_get_mcc_mnc(
  mmgsdi_plmn_id_type   plmn,
  uint16              * mcc_ptr,
  uint16              * mnc_ptr
);
#endif /* MMGSDI_EONS_H */
