#ifndef MCFG_TRL_H
#define MCFG_TRL_H
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_sel/inc/mcfg_trl.h#1 $
$DateTime: 2016/12/13 08:00:27 $
$Author: mplcsds1 $
$Change: 11985219 $

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
09/07/12 bd   Created module

==============================================================================*/

#include "comdef.h"

#include "mcfg_common.h"

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Constants and Macros
==============================================================================*/

/*------------------------------------------------------------------------------
  TLV IDs
------------------------------------------------------------------------------*/

#define MCFG_TRL_TLV_ID_FORMAT_VERSION    (0x00)
#define MCFG_TRL_TLV_ID_CONFIG_VERSION    (0x01)
#define MCFG_TRL_TLV_ID_CARRIER_MCC_MNC   (0x02)
#define MCFG_TRL_TLV_ID_CARRIER_NAME      (0x03)
#define MCFG_TRL_TLV_ID_IIN_LIST          (0x04)
#define MCFG_TRL_TLV_ID_BASE_VERSION      (0x05)
#define MCFG_TRL_TLV_ID_CARRIER_PLMN_LIST (0x06)
#define MCFG_TRL_TLV_ID_CATEGORY          (0x07)
#define MCFG_TRL_TLV_ID_IIN_STRING        (0x0A)


/*------------------------------------------------------------------------------
  Constants used in TLV data
------------------------------------------------------------------------------*/

/* Config version: supported system types bitmask values */
#define MCFG_TRL_CONFIG_VERSION_SYS_TYPE_CDMA    (1 << 0)
#define MCFG_TRL_CONFIG_VERSION_SYS_TYPE_GSM     (1 << 1)
#define MCFG_TRL_CONFIG_VERSION_SYS_TYPE_UMTS    (1 << 2)
#define MCFG_TRL_CONFIG_VERSION_SYS_TYPE_LTE     (1 << 3)
#define MCFG_TRL_CONFIG_VERSION_SYS_TYPE_TDSCDMA (1 << 4)

/* Carrier name: maximum number of characters in the carrier name string */
#define MCFG_TRL_CARRIER_NAME_MAX_LEN (63)

/* IIN String: maximum number of characters in the iin list string */
#define MCFG_TRL_IIN_STRING_MAX_LEN (127)

/* IIN list: flags */
/* If this flag is set, the configuration applies to any UIM only if no other
   configuration is found with a matching IIN. Relevant only when iin_list_size
   is 0. */
#define MCFG_TRL_IIN_FLAG_WILDCARD (1 << 0)
#define MCFG_TRL_PLMN_FLAG_WILDCARD (1 << 0)

/* IIN list: maximum number of IINs in the list */
#define MCFG_TRL_IIN_LIST_MAX_COUNT (32)
#define MCFG_TRL_PLMN_LIST_MAX_COUNT (32)


typedef enum
{
  MCFG_CATEGORY_BIT_DUALS      = 0,
  MCFG_CATEGORY_BIT_SUBSI      = 1,
  MCFG_CATEGORY_BIT_VOLTE      = 2,   
  MCFG_CATEGORY_BIT_LAB        = 3,    
} mcfg_category_bit_type;

/* ----------------------------------------------------------------------------
   ENUM:      MCFG_CATEGORY_INDEX_MASK_TYPE
-------------------------------------------------------------------------------*/
typedef enum
{
  MCFG_CATEGORY_BITMASK_DUALS   = 1 << MCFG_CATEGORY_BIT_DUALS,
  MCFG_CATEGORY_BITMASK_SUBSI   = 1 << MCFG_CATEGORY_BIT_SUBSI,
  MCFG_CATEGORY_BITMASK_VOLTE   = 1 << MCFG_CATEGORY_BIT_VOLTE,   
  MCFG_CATEGORY_BITMASK_LAB     = 1 << MCFG_CATEGORY_BIT_LAB,
} mcfg_category_bitmask_type;

/*==============================================================================
  Typedefs
==============================================================================*/

/*------------------------------------------------------------------------------
  TLV data structures
------------------------------------------------------------------------------*/

/* MCFG_TRL_TLV_ID_FORMAT_VERSION */
typedef struct {
  uint8 min_ver;
  uint8 maj_ver;
} mcfg_trl_format_version_s_type;

/* MCFG_TRL_TLV_ID_CONFIG_VERSION */
typedef PACKED struct {
  /* Minor version */
  uint8 min_ver;

  /* Major version */
  uint8 maj_ver;

  /* Supported system types (bitmask) */
  uint8 sys_type;

  /* Compatibility version */
  uint8 compat_version;
} mcfg_trl_config_version_s_type;

/* MCFG_TRL_TLV_ID_CARRIER_MCC_MNC */
typedef struct {
  /* Mobile Country Code */
  uint16 mcc;

  /* Mobile Network Code */
  uint16 mnc;
} mcfg_trl_carrier_mcc_mnc_s_type;

/* MCFG_TRL_TLV_ID_CARRIER_NAME */
typedef struct {
  /* Name of the carrier (network operator) associated with the configuration.
     NULL-terminated ASCII string. */
  char carrier_name[MCFG_TRL_CARRIER_NAME_MAX_LEN + 1];
} mcfg_trl_carrier_name_s_type;

/* MCFG_TRL_TLV_ID_IIN_LIST */
typedef struct {
  uint8  iin_flag;
  uint8  iin_list_count;
  uint32 iin_list[MCFG_TRL_IIN_LIST_MAX_COUNT];
} mcfg_trl_iin_list_s_type;

/* MCFG_TRL_TLV_ID_PLMN_LIST */
typedef struct {
  uint8                           plmn_flag;
  uint8                           plmn_list_count;
  mcfg_trl_carrier_mcc_mnc_s_type plmn_list[MCFG_TRL_PLMN_LIST_MAX_COUNT];
} mcfg_trl_plmn_list_s_type;

/* MCFG_TRL_TLV_ID_CATEGORY */
typedef PACKED struct {
  uint32 category;
} mcfg_trl_category_s_type;


/* MCFG_TRL_TLV_ID_IIN_STRING */
typedef struct {
  /* IIN list, NULL-terminated ASCII string. */
  char iin_string[MCFG_TRL_IIN_STRING_MAX_LEN+1];
} mcfg_trl_iin_string_s_type;


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
);

#endif /* MCFG_TRL_H */
