/*==============================================================================

      M O D E M   C O N F I G   T R A I L E R   R E C O R D   U T I L S

GENERAL DESCRIPTION
  Provides an interface to help decode the TLV format of the trailer record.

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_sel/src/mcfg_trl.c#1 $
$DateTime: 2016/12/13 08:00:27 $
$Author: mplcsds1 $
$Change: 11985219 $

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
09/07/12 bd   Created module

==============================================================================*/

#include "comdef.h"

#include "mcfg_common.h"
#include "mcfg_int.h"
#include "mcfg_trl.h"
#include "mcfg_osal.h"

#include <stdio.h>
#include <string.h>

/*==============================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Constants and Macros
==============================================================================*/

/* Magic string appearing at the beginning of the trailer record to indicate
   TLV format */
#define MCFG_TRL_MAGIC     "MCFG_TRL"
#define MCFG_TRL_MAGIC_LEN (8)

/* The maximum format major version supported by the decoding functions in this
   file */
#define MCFG_TRL_MAX_SUPPORTED_FORMAT_VERSION (0x01)

/*------------------------------------------------------------------------------
  TLV info flags
------------------------------------------------------------------------------*/

#define MCFG_TRL_TLV_INFO_FLAG_SIZE_BIT (1 << 0)

/* TLV size is fixed; size field in TLV info struct is the expected size */
#define MCFG_TRL_TLV_INFO_FLAG_SIZE_FIXED (0)

/* TLV size is variable; size field in TLV info struct is minimum data size
   (0 is allowed for no minimum) */
#define MCFG_TRL_TLV_INFO_FLAG_SIZE_VARIABLE (1 << 0)

/*------------------------------------------------------------------------------
  TLV sizes
------------------------------------------------------------------------------*/

#define MCFG_TRL_TLV_SIZE_FORMAT_VERSION  (2)
#define MCFG_TRL_TLV_SIZE_CONFIG_VERSION  (4)
#define MCFG_TRL_TLV_SIZE_CARRIER_MCC_MNC (4)
#define MCFG_TRL_TLV_SIZE_CARRIER_NAME    (0)
#define MCFG_TRL_TLV_SIZE_IIN_LIST        (2)
#define MCFG_TRL_TLV_SIZE_BASE_VERSION    (4)
#define MCFG_TRL_TLV_SIZE_PLMN_LIST       (2)
#define MCFG_TRL_TLV_SIZE_CATEGORY        (4)
#define MCFG_TRL_TLV_SIZE_IIN_STRING      (0)

/*==============================================================================
  Typedefs
==============================================================================*/

typedef PACK(struct) {
  uint8  type;
  uint16 length;
} mcfg_trl_tlv_header_s_type;

typedef boolean (mcfg_trl_tlv_decode_fn_type)
(
  const mcfg_config_info_s_type *config_info,
  uint32                         offset,
  void                          *decode_buffer
);

typedef struct {
  /* Flags describing the TLV */
  uint32 flags;

  /* Expected size of the TLV data (bytes) */
  uint32 size;

  /* Size of the data structure used for the decoded TLV data (bytes, always
     fixed length) */
  uint32 decoded_size;

  /* Function to decode TLV data from the packed format into its associated
     C structure defined in mcfg_trl.h. Set to NULL if no decode function
     implemented. */
  mcfg_trl_tlv_decode_fn_type *decode_fn;
} mcfg_trl_tlv_info_s_type;

/*------------------------------------------------------------------------------
  Packed TLV structures
------------------------------------------------------------------------------*/

typedef PACK(struct) {
  uint8  iin_flag;
  uint8  iin_list_count;
  uint32 iin_list[MCFG_TRL_IIN_LIST_MAX_COUNT];
} mcfg_trl_iin_list_packed_s_type;

typedef PACK(struct) {
  uint8  plmn_flag;
  uint8  plmn_list_count;
  uint32 plmn_list[MCFG_TRL_PLMN_LIST_MAX_COUNT];
} mcfg_trl_plmn_list_packed_s_type;

/*==============================================================================
  Function prototypes
==============================================================================*/

static boolean mcfg_trl_decode_carrier_mcc_mnc
(
  const mcfg_config_info_s_type *config_info,
  uint32                         offset,
  void                          *decode_buffer
);

static boolean mcfg_trl_decode_carrier_name
(
  const mcfg_config_info_s_type *config_info,
  uint32                         offset,
  void                          *decode_buffer
);

static boolean mcfg_trl_decode_config_version
(
  const mcfg_config_info_s_type *config_info,
  uint32                         offset,
  void                          *decode_buffer
);

static boolean mcfg_trl_decode_format_version
(
  const mcfg_config_info_s_type *config_info,
  uint32                         offset,
  void                          *decode_buffer
);

static boolean mcfg_trl_decode_iin_list
(
  const mcfg_config_info_s_type *config_info,
  uint32                         offset,
  void                          *decode_buffer
);

static boolean mcfg_trl_decode_plmn_list
(
  const mcfg_config_info_s_type *config_info,
  uint32                         offset,
  void                          *decode_buffer
);

static boolean mcfg_trl_decode_category
(
  const mcfg_config_info_s_type *config_info,
  uint32                         offset,
  void                          *decode_buffer
);

static boolean mcfg_trl_decode_iin_string
(
  const mcfg_config_info_s_type *config_info,
  uint32                         offset,
  void                          *decode_buffer
);

static uint32 mcfg_trl_find_tlv
(
  const mcfg_config_info_s_type *config_info,
  uint8                          tlv_id
);

static boolean mcfg_trl_format_version_supported
(
  const mcfg_config_info_s_type *config_info
);

/*==============================================================================
  Private Variables
==============================================================================*/

/* Table of TLV information, including decode handler. Indexed by TLV ID. */
static const mcfg_trl_tlv_info_s_type mcfg_trl_tlv_info[] =
{
  /* MCFG_TRL_TLV_ID_FORMAT_VERSION */
  {
    MCFG_TRL_TLV_INFO_FLAG_SIZE_FIXED,
    MCFG_TRL_TLV_SIZE_FORMAT_VERSION,
    sizeof(mcfg_trl_format_version_s_type),
    mcfg_trl_decode_format_version
  },

  /* MCFG_TRL_TLV_ID_CONFIG_VERSION */
  {
    MCFG_TRL_TLV_INFO_FLAG_SIZE_FIXED,
    MCFG_TRL_TLV_SIZE_CONFIG_VERSION,
    sizeof(mcfg_trl_config_version_s_type),
    mcfg_trl_decode_config_version
  },

  /* MCFG_TRL_TLV_ID_CARRIER_MCC_MNC */
  {
    MCFG_TRL_TLV_INFO_FLAG_SIZE_FIXED,
    MCFG_TRL_TLV_SIZE_CARRIER_MCC_MNC,
    sizeof(mcfg_trl_carrier_mcc_mnc_s_type),
    mcfg_trl_decode_carrier_mcc_mnc
  },

  /* MCFG_TRL_TLV_ID_CARRIER_NAME */
  {
    MCFG_TRL_TLV_INFO_FLAG_SIZE_VARIABLE,
    MCFG_TRL_TLV_SIZE_CARRIER_NAME,
    sizeof(mcfg_trl_carrier_name_s_type),
    mcfg_trl_decode_carrier_name
  },

  /* MCFG_TRL_TLV_ID_IIN_LIST */
  {
    MCFG_TRL_TLV_INFO_FLAG_SIZE_VARIABLE,
    MCFG_TRL_TLV_SIZE_IIN_LIST,
    sizeof(mcfg_trl_iin_list_s_type),
    mcfg_trl_decode_iin_list
  },

  /* MCFG_TRL_TLV_ID_BASE_VERSION */
  {
    MCFG_TRL_TLV_INFO_FLAG_SIZE_FIXED,
    MCFG_TRL_TLV_SIZE_BASE_VERSION,
    sizeof(mcfg_trl_config_version_s_type),
    mcfg_trl_decode_config_version
  },

  /* MCFG_TRL_TLV_ID_PLMN_LIST */
  {
    MCFG_TRL_TLV_INFO_FLAG_SIZE_VARIABLE,
    MCFG_TRL_TLV_SIZE_PLMN_LIST,
    sizeof(mcfg_trl_plmn_list_s_type),
    mcfg_trl_decode_plmn_list
  },

  /* MCFG_TRL_TLV_ID_CATEGORY */
  {
    MCFG_TRL_TLV_INFO_FLAG_SIZE_FIXED,
    MCFG_TRL_TLV_SIZE_CATEGORY,
    sizeof(mcfg_trl_category_s_type),
    mcfg_trl_decode_category
  },

  /* MCFG_TRL_TLV_ID_RESERVED_8 */
  {
    MCFG_TRL_TLV_INFO_FLAG_SIZE_VARIABLE,
    0,
    0,
    NULL
  },

  /* MCFG_TRL_TLV_ID_RESERVED_9 */
  {
    MCFG_TRL_TLV_INFO_FLAG_SIZE_VARIABLE,
    0,
    0,
    NULL
  },

  /* MCFG_TRL_TLV_ID_IIN_STRING */
  {
    MCFG_TRL_TLV_INFO_FLAG_SIZE_VARIABLE,
    MCFG_TRL_TLV_SIZE_IIN_STRING,
    sizeof(mcfg_trl_iin_string_s_type),
    mcfg_trl_decode_iin_string
  },

};

/*==============================================================================

                     INTERNAL FUNCTION DECLARATIONS

==============================================================================*/

/*===========================================================================

  FUNCTION mcfg_trl_decode_carrier_mcc_mnc

  DESCRIPTION
    Decodes the carrier MCC+MNC TLV data.

  DEPENDENCIES
    Assumes the trailer record, TLV length, and decode buffer length have
    already been validated. Also assumes decode_buffer has been zero-init.

  PARAMETERS
    config_info    [in]
    offset         [in] Byte index of the start of the format version TLV
    decode_buffer [out] Output buffer for decoded data

  RETURN VALUE
    TRUE on successful decode, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_trl_decode_carrier_mcc_mnc
(
  const mcfg_config_info_s_type *config_info,
  uint32                         offset,
  void                          *decode_buffer
)
{
  const uint8 *packed_data;
  mcfg_trl_carrier_mcc_mnc_s_type *carrier_mcc_mnc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(config_info);
  MCFG_CHECK_NULL_PTR_RET_FALSE(decode_buffer);

  packed_data = (const uint8 *)
    (&config_info->description[offset] + sizeof(mcfg_trl_tlv_header_s_type));
  carrier_mcc_mnc = (mcfg_trl_carrier_mcc_mnc_s_type *) decode_buffer;

  carrier_mcc_mnc->mcc = packed_data[0] | (packed_data[1] << 8);
  carrier_mcc_mnc->mnc = packed_data[2] | (packed_data[3] << 8);

  return TRUE;
} /* mcfg_trl_decode_carrier_mcc_mnc() */

/*===========================================================================

  FUNCTION mcfg_trl_decode_carrier_name

  DESCRIPTION
    Decodes the carrier MCC+MNC TLV data.

  DEPENDENCIES
    Assumes the trailer record, TLV length, and decode buffer length have
    already been validated. Also assumes decode_buffer has been zero-init.

  PARAMETERS
    config_info    [in]
    offset         [in] Byte index of the start of the format version TLV
    decode_buffer [out] Output buffer for decoded data

  RETURN VALUE
    TRUE on successful decode, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_trl_decode_carrier_name
(
  const mcfg_config_info_s_type *config_info,
  uint32                         offset,
  void                          *decode_buffer
)
{
  const mcfg_trl_tlv_header_s_type *tlv_header;
  const char *packed_data;
  mcfg_trl_carrier_name_s_type *carrier_name;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(config_info);
  MCFG_CHECK_NULL_PTR_RET_FALSE(decode_buffer);

  tlv_header = (const mcfg_trl_tlv_header_s_type *)
    &config_info->description[offset];
  packed_data = (const char *)
    (&config_info->description[offset] + sizeof(mcfg_trl_tlv_header_s_type));
  carrier_name = (mcfg_trl_carrier_name_s_type *) decode_buffer;

  if (tlv_header->length > MCFG_TRL_CARRIER_NAME_MAX_LEN)
  {
    MCFG_MSG_ERROR_2("Carrier name length %hu past maximum %d",
                     tlv_header->length, MCFG_TRL_CARRIER_NAME_MAX_LEN);
  }
  else
  {
    /* Null termination ensured by earlier zero-init of decode buffer and
       extra byte of space available in the decoded data structure */
    memscpy(carrier_name->carrier_name, MCFG_TRL_CARRIER_NAME_MAX_LEN+1, 
            packed_data, tlv_header->length);
  }

  return TRUE;
} /* mcfg_trl_decode_carrier_name() */

/*===========================================================================

  FUNCTION mcfg_trl_decode_config_version

  DESCRIPTION
    Decodes the config version TLV data.

  DEPENDENCIES
    Assumes the trailer record, TLV length, and decode buffer length have
    already been validated. Also assumes decode_buffer has been zero-init.

  PARAMETERS
    config_info    [in]
    offset         [in] Byte index of the start of the format version TLV
    decode_buffer [out] Output buffer for decoded data

  RETURN VALUE
    TRUE on successful decode, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_trl_decode_config_version
(
  const mcfg_config_info_s_type *config_info,
  uint32                         offset,
  void                          *decode_buffer
)
{
  const uint8 *packed_data;
  mcfg_trl_config_version_s_type *config_version;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(config_info);
  MCFG_CHECK_NULL_PTR_RET_FALSE(decode_buffer);

  packed_data = (const uint8 *)
    (&config_info->description[offset] + sizeof(mcfg_trl_tlv_header_s_type));
  config_version = (mcfg_trl_config_version_s_type *) decode_buffer;

  config_version->min_ver        = packed_data[0];
  config_version->maj_ver        = packed_data[1];
  config_version->sys_type       = packed_data[2];
  /* Upper 3 bits reserved for future use */
  config_version->compat_version = (packed_data[3] & 0x1F);

  return TRUE;
} /* mcfg_trl_decode_config_version() */

/*===========================================================================

  FUNCTION mcfg_trl_decode_format_version

  DESCRIPTION
    Decodes the format version TLV data.

  DEPENDENCIES
    Assumes the trailer record, TLV length, and decode buffer length have
    already been validated. Also assumes decode_buffer has been zero-init.

  PARAMETERS
    config_info    [in]
    offset         [in] Byte index of the start of the format version TLV
    decode_buffer [out] Output buffer for decoded data

  RETURN VALUE
    TRUE on successful decode, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_trl_decode_format_version
(
  const mcfg_config_info_s_type *config_info,
  uint32                         offset,
  void                          *decode_buffer
)
{
  const uint8 *packed_data;
  mcfg_trl_format_version_s_type *format_version;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(config_info);
  MCFG_CHECK_NULL_PTR_RET_FALSE(decode_buffer);

  packed_data = (const uint8 *)
    (&config_info->description[offset] + sizeof(mcfg_trl_tlv_header_s_type));
  format_version = (mcfg_trl_format_version_s_type *) decode_buffer;

  format_version->min_ver = packed_data[0];
  format_version->maj_ver = packed_data[1];

  return TRUE;
} /* mcfg_trl_decode_format_version() */

/*===========================================================================

  FUNCTION mcfg_trl_decode_iin_list

  DESCRIPTION
    Decodes the IIN list TLV data.

  DEPENDENCIES
    Assumes the trailer record, TLV length, and decode buffer length have
    already been validated. Also assumes decode_buffer has been zero-init.

  PARAMETERS
    config_info    [in]
    offset         [in] Byte index of the start of the format version TLV
    decode_buffer [out] Output buffer for decoded data

  RETURN VALUE
    TRUE on successful decode, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_trl_decode_iin_list
(
  const mcfg_config_info_s_type *config_info,
  uint32                         offset,
  void                          *decode_buffer
)
{
  const mcfg_trl_tlv_header_s_type *tlv_header;
  const mcfg_trl_iin_list_packed_s_type *packed_data;
  mcfg_trl_iin_list_s_type *iin_list;
  boolean success = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(config_info);
  MCFG_CHECK_NULL_PTR_RET_FALSE(decode_buffer);

  tlv_header = (const mcfg_trl_tlv_header_s_type *)
    &config_info->description[offset];
  packed_data = (const mcfg_trl_iin_list_packed_s_type *)
    (&config_info->description[offset] + sizeof(mcfg_trl_tlv_header_s_type));
  iin_list = (mcfg_trl_iin_list_s_type *) decode_buffer;

  if (tlv_header->length !=
        (MCFG_TRL_TLV_SIZE_IIN_LIST +
           (packed_data->iin_list_count * sizeof(uint32))))
  {
    MCFG_MSG_ERROR_2("Bad IIN list length %hu for %hhu entries",
                     tlv_header->length, packed_data->iin_list_count);
  }
  else if (packed_data->iin_list_count > MCFG_TRL_IIN_LIST_MAX_COUNT)
  {
    MCFG_MSG_HIGH_2("IIN list count %hhu is past maximum %d",
                    packed_data->iin_list_count, MCFG_TRL_IIN_LIST_MAX_COUNT);
  }
  else
  {
    iin_list->iin_flag       = packed_data->iin_flag;
    iin_list->iin_list_count = packed_data->iin_list_count;
    memscpy(iin_list->iin_list, MCFG_TRL_IIN_LIST_MAX_COUNT * sizeof(uint32),
            packed_data->iin_list, iin_list->iin_list_count * sizeof(uint32));
    success = TRUE;
  }

  return success;
} /* mcfg_trl_decode_iin_list() */

/*===========================================================================

  FUNCTION mcfg_trl_decode_plmn_list

  DESCRIPTION
    Decodes the PLMN list TLV data.

  DEPENDENCIES
    Assumes the trailer record, TLV length, and decode buffer length have
    already been validated. Also assumes decode_buffer has been zero-init.

  PARAMETERS
    config_info    [in]
    offset         [in] Byte index of the start of the format version TLV
    decode_buffer [out] Output buffer for decoded data

  RETURN VALUE
    TRUE on successful decode, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_trl_decode_plmn_list
(
  const mcfg_config_info_s_type *config_info,
  uint32                         offset,
  void                          *decode_buffer
)
{
  const mcfg_trl_tlv_header_s_type *tlv_header;
  const mcfg_trl_plmn_list_packed_s_type *packed_data;
  mcfg_trl_plmn_list_s_type *plmn_list;
  boolean success = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(config_info);
  MCFG_CHECK_NULL_PTR_RET_FALSE(decode_buffer);

  tlv_header = (const mcfg_trl_tlv_header_s_type *)
    &config_info->description[offset];
  packed_data = (const mcfg_trl_plmn_list_packed_s_type *)
    (&config_info->description[offset] + sizeof(mcfg_trl_tlv_header_s_type));
  plmn_list = (mcfg_trl_plmn_list_s_type *) decode_buffer;

  if (tlv_header->length !=
        (MCFG_TRL_TLV_SIZE_PLMN_LIST +
           (packed_data->plmn_list_count * sizeof(mcfg_trl_carrier_mcc_mnc_s_type))))
  {
    MCFG_MSG_ERROR_2("Bad PLMN list length %hu for %hhu entries",
                     tlv_header->length, packed_data->plmn_list_count);
  }
  else if (packed_data->plmn_list_count > MCFG_TRL_PLMN_LIST_MAX_COUNT)
  {
    MCFG_MSG_HIGH_2("PLMN list count %hhu is past maximum %d",
                    packed_data->plmn_list_count, MCFG_TRL_PLMN_LIST_MAX_COUNT);
  }
  else
  {
    plmn_list->plmn_flag       = packed_data->plmn_flag;
    plmn_list->plmn_list_count = packed_data->plmn_list_count;
    memscpy(plmn_list->plmn_list, MCFG_TRL_PLMN_LIST_MAX_COUNT * sizeof(mcfg_trl_carrier_mcc_mnc_s_type),
            packed_data->plmn_list, plmn_list->plmn_list_count * sizeof(mcfg_trl_carrier_mcc_mnc_s_type));
    success = TRUE;
  }

  return success;
} /* mcfg_trl_decode_plmn_list() */

/*===========================================================================

  FUNCTION mcfg_trl_decode_category

  DESCRIPTION
    Decodes the category TLV data.

  DEPENDENCIES
    Assumes the trailer record, TLV length, and decode buffer length have
    already been validated. Also assumes decode_buffer has been zero-init.

  PARAMETERS
    config_info    [in]
    offset         [in] Byte index of the start of the format version TLV
    decode_buffer [out] Output buffer for decoded data

  RETURN VALUE
    TRUE on successful decode, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_trl_decode_category
(
  const mcfg_config_info_s_type *config_info,
  uint32                         offset,
  void                          *decode_buffer
)
{
  const uint8 *packed_data;
  const uint32 *packed_data_32;
  mcfg_trl_category_s_type *category;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(config_info);
  MCFG_CHECK_NULL_PTR_RET_FALSE(decode_buffer);

  memscpy(decode_buffer, sizeof(mcfg_trl_category_s_type),
          (const uint8 *)(&config_info->description[offset] + sizeof(mcfg_trl_tlv_header_s_type)),
          sizeof(mcfg_trl_category_s_type));

  return TRUE;
} /* mcfg_trl_decode_category() */

/*===========================================================================

  FUNCTION mcfg_trl_decode_iin_string

  DESCRIPTION
    Decodes the carrier IIN string TLV data.

  DEPENDENCIES
    Assumes the trailer record, TLV length, and decode buffer length have
    already been validated. Also assumes decode_buffer has been zero-init.

  PARAMETERS
    config_info    [in]
    offset         [in] Byte index of the start of the format version TLV
    decode_buffer [out] Output buffer for decoded data

  RETURN VALUE
    TRUE on successful decode, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_trl_decode_iin_string
(
  const mcfg_config_info_s_type *config_info,
  uint32                         offset,
  void                          *decode_buffer
)
{
  const mcfg_trl_tlv_header_s_type *tlv_header;
  const char *packed_data;
  mcfg_trl_iin_string_s_type *iin_string;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(config_info);
  MCFG_CHECK_NULL_PTR_RET_FALSE(decode_buffer);

  tlv_header = (const mcfg_trl_tlv_header_s_type *)
    &config_info->description[offset];
  packed_data = (const char *)
    (&config_info->description[offset] + sizeof(mcfg_trl_tlv_header_s_type));
  iin_string = (mcfg_trl_iin_string_s_type *) decode_buffer;

  if (tlv_header->length > MCFG_TRL_IIN_STRING_MAX_LEN)
  {
    MCFG_MSG_ERROR_2("IIN String length %hu past maximum %d",
                     tlv_header->length, MCFG_TRL_IIN_STRING_MAX_LEN);
  }
  else
  {
    /* Null termination ensured by earlier zero-init of decode buffer and
       extra byte of space available in the decoded data structure */
    memscpy(iin_string->iin_string, MCFG_TRL_IIN_STRING_MAX_LEN+1, 
            packed_data, tlv_header->length);
  }

  return TRUE;
} /* mcfg_trl_decode_iin_string() */

/*===========================================================================

  FUNCTION mcfg_trl_find_tlv

  DESCRIPTION
    Searches for a TLV with matching ID in the given trailer record. Also
    verifies that the length field of the TLV is valid (is correct per the
    TLV info and does not exceed the length of the entire trailer record).

  DEPENDENCIES
    The magic header must have already been checked, and therefore this
    function assumes the length of the trailer record is at least
    MCFG_TRL_MAGIC_LEN. The TLV ID must already be sanity checked to be
    within the supported bounds of mcfg_trl_tlv_info[].

  PARAMETERS
    config_info [in]
    tlv_id      [in]

  RETURN VALUE
    Byte offset to start of TLV structure with matching TLV ID, or zero
    if the TLV was not found

  SIDE EFFECTS
    None

===========================================================================*/
static uint32 mcfg_trl_find_tlv
(
  const mcfg_config_info_s_type *config_info,
  uint8                          tlv_id
)
{
  boolean found = FALSE;
  uint32 offset = MCFG_TRL_MAGIC_LEN;
  const mcfg_trl_tlv_header_s_type *tlv_header;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_ZERO(config_info);

  while (!found && offset <= (config_info->description_len -
                                sizeof(mcfg_trl_tlv_header_s_type)))
  {
    tlv_header = (const mcfg_trl_tlv_header_s_type *)
      &config_info->description[offset];
    if (tlv_header->type == tlv_id)
    {
      found = TRUE;
    }
    else
    {
      offset += sizeof(mcfg_trl_tlv_header_s_type) + tlv_header->length;
    }
  }

  if (!found)
  {
    offset = 0;
    MCFG_MSG_HIGH_1("TLV 0x%02x not found", tlv_id);
  }
  else if ((offset + sizeof(mcfg_trl_tlv_header_s_type) + tlv_header->length) >
             config_info->description_len)
  {
    MCFG_MSG_ERROR_4("Bad size for TLV 0x%02x: length %hu at offset %lu past "
                     "trailer size %d", tlv_id, tlv_header->length, offset,
                     config_info->description_len);
    offset = 0;
  }
  else if (((mcfg_trl_tlv_info[tlv_id].flags & MCFG_TRL_TLV_INFO_FLAG_SIZE_BIT) ==
              MCFG_TRL_TLV_INFO_FLAG_SIZE_VARIABLE &&
            tlv_header->length < mcfg_trl_tlv_info[tlv_id].size) ||
           ((mcfg_trl_tlv_info[tlv_id].flags & MCFG_TRL_TLV_INFO_FLAG_SIZE_BIT) ==
              MCFG_TRL_TLV_INFO_FLAG_SIZE_FIXED &&
            tlv_header->length != mcfg_trl_tlv_info[tlv_id].size))
  {
    offset = 0;
    MCFG_MSG_ERROR_3("Bad length for TLV 0x%02x: got %hu, expected/min %lu",
                     tlv_id, tlv_header->length, mcfg_trl_tlv_info[tlv_id].size);
  }

  return offset;
} /* mcfg_trl_find_tlv() */

/*===========================================================================

  FUNCTION mcfg_trl_format_version_supported

  DESCRIPTION
    Checks whether the trailer record TLV format version is supported
    by the decoding routines in this file.

  DEPENDENCIES
    None

  PARAMETERS
    config_info [in]

  RETURN VALUE
    TRUE if supported, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_trl_format_version_supported
(
  const mcfg_config_info_s_type *config_info
)
{
  uint32 offset;
  mcfg_trl_format_version_s_type format_version;
  boolean supported = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  format_version.maj_ver = 0;

  offset = mcfg_trl_find_tlv(config_info, MCFG_TRL_TLV_ID_FORMAT_VERSION);
  if (offset != 0 && offset < MCFG_CONFIG_DESCRIPTION_SIZE_MAX &&
      mcfg_trl_decode_format_version(config_info, offset, &format_version))
  {
    if (format_version.maj_ver > MCFG_TRL_MAX_SUPPORTED_FORMAT_VERSION)
    {
      MCFG_MSG_ERROR_3("Incompatible trailer record encountered: version %d.%d,"
                       " max supported %d.x",
                       format_version.maj_ver, format_version.min_ver,
                       MCFG_TRL_MAX_SUPPORTED_FORMAT_VERSION);
    }
    else
    {
      supported = TRUE;
    }
  }

  return supported;
} /* mcfg_trl_format_version_supported() */

/*==============================================================================
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================

  FUNCTION mcfg_trl_decode_tlv

  DESCRIPTION
    Finds a trailer record TLV and decodes it into the appropriate data
    structure.

  DEPENDENCIES
    None

  PARAMETERS
    config_info       [in]
    tlv_id            [in]
    decoded_data     [out] Pointer to decoded data structure for this TLV
    decoded_data_len [out] Size of the decoded data structure

  RETURN VALUE
    TRUE if TLV was successfully found and decoded, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_trl_decode_tlv
(
  const mcfg_config_info_s_type *config_info,
  uint8                          tlv_id,
  void                          *decoded_data,
  uint32                         decoded_data_len
)
{
  boolean success = FALSE;
  uint32 offset;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(config_info);
  MCFG_CHECK_NULL_PTR_RET_FALSE(decoded_data);

  if (config_info->description_len < MCFG_TRL_MAGIC_LEN ||
      strncmp((const char *) config_info->description, MCFG_TRL_MAGIC,
              MCFG_TRL_MAGIC_LEN) != 0)
  {
    MCFG_MSG_ERROR("Missing or unexpected trailer magic");
  }
  else if (config_info->description_len > sizeof(config_info->description))
  {
    MCFG_MSG_ERROR_2("Description length (%d) past maximum (%d)",
                     config_info->description_len,
                     sizeof(config_info->description));
  }
  else if (!mcfg_trl_format_version_supported(config_info))
  {
    MCFG_MSG_ERROR("Trailer record format version not supported");
  }
  else if (tlv_id >= ARR_SIZE(mcfg_trl_tlv_info) ||
           mcfg_trl_tlv_info[tlv_id].decode_fn == NULL)
  {
    MCFG_MSG_ERROR_1("Decoding of TLV ID 0x%02x is not supported", tlv_id);
  }
  else if (decoded_data_len != mcfg_trl_tlv_info[tlv_id].decoded_size)
  {
    MCFG_MSG_ERROR_2("Invalid decode buffer size %lu, expected %lu",
                     decoded_data_len, mcfg_trl_tlv_info[tlv_id].decoded_size);
  }
  else
  {
    offset = mcfg_trl_find_tlv(config_info, tlv_id);
    if (offset == 0)
    {
      MCFG_MSG_ERROR_1("TLV ID 0x%02x not found or has invalid length", tlv_id);
    }
    else
    {
      memset(decoded_data, 0, decoded_data_len);
      success = mcfg_trl_tlv_info[tlv_id].decode_fn(
        config_info, offset, decoded_data);
    }
  }

  return success;
} /* mcfg_trl_decode_tlv() */

