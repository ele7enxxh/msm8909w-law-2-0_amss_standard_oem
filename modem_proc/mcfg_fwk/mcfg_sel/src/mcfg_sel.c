/*==============================================================================

                M O D E M   C O N F I G   S E L E C T I O N

GENERAL DESCRIPTION
  Contains the logic to automatically select a configuration file based on
  an outside identifying parameter, such as the ICCID read from the UIM.

Copyright (c) 2012-2014 by QUALCOMM Technologies Incorporated.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_sel/src/mcfg_sel.c#1 $
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
#include "mcfg_nv.h"
#include "mcfg_osal.h"
#include "mcfg_sel.h"
#include "mcfg_trl.h"
#include "mcfg_uim.h"
#include "mcfg_utils.h"

#include "persistent_device_configuration_v01.h"
#include "qmi_pdc_svc.h"

#ifdef FEATURE_MCFG_REFRESH
#include "mcfg_refresh_i.h"
#include "mcfg_proc.h"
#endif
/*==============================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/

/*==============================================================================

                 PRIVATE FUNCTION FORWARD DECLARATIONS

==============================================================================*/

static boolean mcfg_sel_config_ids_match
(
  const mcfg_config_id_s_type *config_id1,
  const mcfg_config_id_s_type *config_id2
);

static boolean mcfg_sel_config_is_better_match_by_iccid
(
  const char                    *iccid_ascii,
  const mcfg_config_info_s_type *candidate_config,
  const mcfg_config_info_s_type *selected_config,
  boolean                       *selected_config_is_wildcard
);

static boolean mcfg_sel_config_is_direct_iccid_match_with_iin_string
(
  const char *uim_iccid,
  const char *iin_string
);

static boolean mcfg_sel_config_is_better_match_by_version
(
  const mcfg_config_info_s_type *candidate_config,
  const mcfg_config_info_s_type *selected_config
);

static boolean mcfg_sel_config_is_direct_iccid_match
(
  const char                     *uim_iccid,
  const mcfg_trl_iin_list_s_type *config_iin_list
);

static boolean mcfg_sel_config_is_direct_plmn_match
(
  const mcfg_trl_carrier_mcc_mnc_s_type *uim_plmn,
  const mcfg_trl_plmn_list_s_type       *config_plmn_list
);

static uint32 mcfg_sel_find_best_match_by_iccid
(
  const char            *iccid_ascii,
  mcfg_config_id_s_type *config_list,
  uint32                 num_configs
);

static boolean mcfg_sel_iccid_matches_last
(
  mcfg_sub_id_type_e_type sub_id,
  const char             *iccid_ascii
);

static void mcfg_sel_save_iccid
(
  mcfg_sub_id_type_e_type sub_id,
  const char             *iccid_ascii
);

static void mcfg_sel_select_sw_config
(
  mcfg_sub_id_type_e_type sub_id,
  mcfg_config_id_s_type  *config_id,
  mcfg_activation_source_e_type source
);

#ifdef FEATURE_MCFG_SELECTION_BY_IMSI
static boolean mcfg_sel_config_is_better_match_by_plmn
(
  mcfg_trl_carrier_mcc_mnc_s_type *plmn,
  const mcfg_config_info_s_type   *candidate_config,
  const mcfg_config_info_s_type   *selected_config,
  boolean                         *selected_config_is_wildcard
);

static boolean mcfg_sel_plmn_matches_last
(
  mcfg_sub_id_type_e_type          sub_id,
  mcfg_trl_carrier_mcc_mnc_s_type *plmn
);

static void mcfg_sel_save_plmn
(
  mcfg_sub_id_type_e_type          sub_id,
  mcfg_trl_carrier_mcc_mnc_s_type *plmn
);
#endif

/*==============================================================================

                     INTERNAL FUNCTION DECLARATIONS

==============================================================================*/

/*===========================================================================

  FUNCTION mcfg_sel_config_ids_match

  DESCRIPTION
    Checks whether two config IDs are the same.

  DEPENDENCIES
    None

  PARAMETERS
    config_id1 [in]
    config_id2 [in]

  RETURN VALUE
    TRUE if the config IDs match, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_sel_config_ids_match
(
  const mcfg_config_id_s_type *config_id1,
  const mcfg_config_id_s_type *config_id2
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(config_id1);
  MCFG_CHECK_NULL_PTR_RET_FALSE(config_id2);

  return (config_id1->id_len == config_id2->id_len &&
          config_id1->id_len <= MCFG_CONFIG_ID_SIZE_MAX &&
          memcmp(config_id1->id, config_id2->id, config_id1->id_len) == 0);
} /* mcfg_sel_config_ids_match() */

/*===========================================================================

  FUNCTION mcfg_sel_config_is_better_match_by_iccid

  DESCRIPTION
    Determines whether the candidate config is a better match than the
    selected config based on the current ICCID, using version as a
    tiebreaker.

  DEPENDENCIES
    None

  PARAMETERS
    iccid_ascii      [in] ICCID of the current UIM; ASCII encoding
    candidate_config [in] trailer record of the config to consider against
      the current selection
    selected_config  [in] trailer record of the config currently selected
      as the best match; if no config is selected, this must be all zeros
    selected_config_is_wildcard [in/out] set to TRUE if the selected config
      is a wildcard (matches any ICCID); will be updated with the wildcard
      state of the candidate config if this function returns TRUE. Must be
      set to FALSE if no config is selected yet.

  RETURN VALUE
    TRUE if the candidate config is a better match than the selected config,
    FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_sel_config_is_better_match_by_iccid
(
  const char                    *iccid_ascii,
  const mcfg_config_info_s_type *candidate_config,
  const mcfg_config_info_s_type *selected_config,
  boolean                       *selected_config_is_wildcard
)
{
  boolean candidate_is_better_match = FALSE;
  mcfg_trl_iin_list_s_type candidate_iin_list;
  char name[MCFG_TRL_CARRIER_NAME_MAX_LEN+1];
  char candidate_iin_string[MCFG_TRL_IIN_STRING_MAX_LEN+1];

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(iccid_ascii);
  MCFG_CHECK_NULL_PTR_RET_FALSE(candidate_config);
  MCFG_CHECK_NULL_PTR_RET_FALSE(selected_config);
  MCFG_CHECK_NULL_PTR_RET_FALSE(selected_config_is_wildcard);

  MCFG_MSG_MED_1("Evaluating candidate config with len %d", candidate_config->config_len);

  if (mcfg_trl_decode_tlv(
        candidate_config, MCFG_TRL_TLV_ID_CARRIER_NAME, 
        name, MCFG_TRL_CARRIER_NAME_MAX_LEN+1))
  {
    MCFG_MSG_SPRINTF_1("Config: %s", name);
  }

  if (candidate_config->description_len == 0)
  {
    MCFG_MSG_ERROR("Candidate config trailer record is empty!");
  }
  else if (!mcfg_trl_decode_tlv(
             candidate_config, MCFG_TRL_TLV_ID_IIN_LIST, &candidate_iin_list,
             sizeof(candidate_iin_list)))
  {
    MCFG_MSG_ERROR("Couldn't decode IIN list");
  }
  else if (candidate_iin_list.iin_flag & MCFG_TRL_IIN_FLAG_WILDCARD)
  {
    /* If the candidate is flagged as a wildcard (generic config that works with
       any UIM), it is a better match than the selected config if (a) we don't
       have a selected config yet, or (b) the selected config is also a wildcard
       but is an older version. Otherwise, the selected config is a direct ICCID
       match, so it is better than the candidate. */
    MCFG_MSG_MED_1("Candidate is wildcard (flag 0x%x)",
                   candidate_iin_list.iin_flag);
    if (selected_config->description_len == 0 ||
        (*selected_config_is_wildcard &&
         mcfg_sel_config_is_better_match_by_version(
           candidate_config, selected_config)))
    {
      MCFG_MSG_MED("Candidate is the current best match");
      candidate_is_better_match = TRUE;
      *selected_config_is_wildcard = TRUE;
    }
  }
  else if (mcfg_sel_config_is_direct_iccid_match(iccid_ascii, &candidate_iin_list) || 
           ( mcfg_trl_decode_tlv( candidate_config, MCFG_TRL_TLV_ID_IIN_STRING, candidate_iin_string,
                                  MCFG_TRL_IIN_STRING_MAX_LEN+1) && 
             mcfg_sel_config_is_direct_iccid_match_with_iin_string(iccid_ascii, candidate_iin_string)))
  {
    /* The candidate is a direct ICCID match, so it is a better match than the
       selected config if (a) the selected config is a wildcard, (b) we don't
       have a selected config yet, or (c) the selected config is an older
       version. Otherwise, the selected config is a direct ICCID match with a
       newer version, so it is better than the candidate. */
    MCFG_MSG_MED("Candidate is direct ICCID match");
    if (*selected_config_is_wildcard ||
        mcfg_sel_config_is_better_match_by_version(
          candidate_config, selected_config))
    {
      MCFG_MSG_MED("Candidate is the current best match");
      candidate_is_better_match = TRUE;
      *selected_config_is_wildcard = FALSE;
    }
  }
  else
  {
    MCFG_MSG_LOW_1("mcfg_sel_config_is_better_match_by_iccid not found %d", candidate_is_better_match);
  }

  return candidate_is_better_match;
} /* mcfg_sel_config_is_better_match_by_iccid() */

#ifdef FEATURE_MCFG_SELECTION_BY_IMSI
/*===========================================================================

  FUNCTION mcfg_sel_config_is_better_match_by_plmn

  DESCRIPTION
    Determines whether the candidate config is a better match than the
    selected config based on the current PLMN, using version as a
    tiebreaker.

  DEPENDENCIES
    None

  PARAMETERS
    plmn      [in] PLMN of the current UIM, including decoded mcc & mnc
    candidate_config [in] trailer record of the config to consider against
      the current selection
    selected_config  [in] trailer record of the config currently selected
      as the best match; if no config is selected, this must be all zeros
    selected_config_is_wildcard [in/out] set to TRUE if the selected config
      is a wildcard (matches any ICCID); will be updated with the wildcard
      state of the candidate config if this function returns TRUE. Must be
      set to FALSE if no config is selected yet.

  RETURN VALUE
    TRUE if the candidate config is a better match than the selected config,
    FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_sel_config_is_better_match_by_plmn
(
  mcfg_trl_carrier_mcc_mnc_s_type *plmn,
  const mcfg_config_info_s_type   *candidate_config,
  const mcfg_config_info_s_type   *selected_config,
  boolean                         *selected_config_is_wildcard
)
{
  boolean candidate_is_better_match = FALSE;
  mcfg_trl_plmn_list_s_type candidate_plmn_list;
  char name[MCFG_TRL_CARRIER_NAME_MAX_LEN+1];

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(plmn);
  MCFG_CHECK_NULL_PTR_RET_FALSE(candidate_config);
  MCFG_CHECK_NULL_PTR_RET_FALSE(selected_config);
  MCFG_CHECK_NULL_PTR_RET_FALSE(selected_config_is_wildcard);

  MCFG_MSG_MED_1("Evaluating candidate config with len %d", candidate_config->config_len);
  if (mcfg_trl_decode_tlv(
        candidate_config, MCFG_TRL_TLV_ID_CARRIER_NAME, 
        name, MCFG_TRL_CARRIER_NAME_MAX_LEN+1))
  {
    MCFG_MSG_SPRINTF_1("Config: %s", name);
  }

  if (candidate_config->description_len == 0)
  {
    MCFG_MSG_ERROR("Candidate config trailer record is empty!");
  }
  else if (!mcfg_trl_decode_tlv(
             candidate_config, MCFG_TRL_TLV_ID_CARRIER_PLMN_LIST, &candidate_plmn_list,
             sizeof(candidate_plmn_list)))
  {
    MCFG_MSG_ERROR("Couldn't decode MCC-MNC list");
  }
  else if (candidate_plmn_list.plmn_flag & MCFG_TRL_PLMN_FLAG_WILDCARD)
  {
    /* If the candidate is flagged as a wildcard (generic config that works with
       any UIM), it is a better match than the selected config if (a) we don't
       have a selected config yet, or (b) the selected config is also a wildcard
       but is an older version. Otherwise, the selected config is a direct ICCID
       match, so it is better than the candidate. */
    MCFG_MSG_MED_1("Candidate is wildcard (flag 0x%x)",
                   candidate_plmn_list.plmn_flag);
    if (selected_config->description_len == 0 ||
        (*selected_config_is_wildcard &&
         mcfg_sel_config_is_better_match_by_version(
           candidate_config, selected_config)))
    {
      MCFG_MSG_MED("Candidate is the current best match");
      candidate_is_better_match = TRUE;
      *selected_config_is_wildcard = TRUE;
    }
  }
  else if (mcfg_sel_config_is_direct_plmn_match(plmn, &candidate_plmn_list))
  {
    /* The candidate is a direct ICCID match, so it is a better match than the
       selected config if (a) the selected config is a wildcard, (b) we don't
       have a selected config yet, or (c) the selected config is an older
       version. Otherwise, the selected config is a direct ICCID match with a
       newer version, so it is better than the candidate. */
    MCFG_MSG_MED("Candidate is direct MCC-MNC match");
    if (*selected_config_is_wildcard ||
        mcfg_sel_config_is_better_match_by_version(
          candidate_config, selected_config))
    {
      MCFG_MSG_MED("Candidate is the current best match");
      candidate_is_better_match = TRUE;
      *selected_config_is_wildcard = FALSE;
    }
  }

  return candidate_is_better_match;
} /* mcfg_sel_config_is_better_match_by_plmn() */
#endif /* FEATURE_MCFG_SELECTION_BY_IMSI */

/*===========================================================================

  FUNCTION mcfg_sel_config_is_better_match_by_version

  DESCRIPTION
    Determines whether the candidate config is a better match than the
    selected config based on their versions. Note that this comparison
    mode only considers the version TLV, so it is best suited as a
    tiebreaker rather than the primary decision logic.

  DEPENDENCIES
    None

  PARAMETERS
    candidate_config [in] trailer record of the config to consider against
      the current selection
    selected_config  [in] trailer record of the config currently selected
      as the best match; if no config is selected, this must be all zeros

  RETURN VALUE
    TRUE if the candidate config is a better match than the selected config,
    FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_sel_config_is_better_match_by_version
(
  const mcfg_config_info_s_type *candidate_config,
  const mcfg_config_info_s_type *selected_config
)
{
  boolean candidate_is_better_match = FALSE;
  mcfg_trl_config_version_s_type candidate_version;
  mcfg_trl_config_version_s_type selected_version;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(selected_config);

  /* If we haven't selected a config yet, the candidate automatically becomes
     the new selection. */
  if (selected_config->description_len == 0)
  {
    MCFG_MSG_MED("Candidate wins version comparison by default");
    candidate_is_better_match = TRUE;
  }
  else if (!mcfg_trl_decode_tlv(candidate_config, MCFG_TRL_TLV_ID_CONFIG_VERSION,
                                &candidate_version, sizeof(candidate_version)) ||
           !mcfg_trl_decode_tlv(selected_config, MCFG_TRL_TLV_ID_CONFIG_VERSION,
                                &selected_version, sizeof(selected_version)))
  {
    MCFG_MSG_ERROR("Couldn't decode config version TLV");
  }
  else
  {
    MCFG_MSG_MED_4("Comparing candidate v%hhu.%hhu vs. selected v%hhu.%hhu",
                   candidate_version.maj_ver, candidate_version.min_ver,
                   selected_version.maj_ver, selected_version.min_ver);
    if (candidate_version.maj_ver > selected_version.maj_ver ||
        (candidate_version.maj_ver == selected_version.maj_ver &&
         candidate_version.min_ver > selected_version.min_ver))
    {
      candidate_is_better_match = TRUE;
    }
  }

  return candidate_is_better_match;
} /* mcfg_sel_config_is_better_match_by_version() */

/*===========================================================================

  FUNCTION mcfg_sel_config_is_better_match_by_carrier

  DESCRIPTION
    Determines whether the candidate config is a better match than the
    selected config based on the current CARRIER, using version as a
    tiebreaker.

  DEPENDENCIES
    None

  PARAMETERS
    carrier      [in] CARRIER of the current UIM, including decoded mcc & mnc
    candidate_config [in] trailer record of the config to consider against
      the current selection
    selected_config  [in] trailer record of the config currently selected
      as the best match; if no config is selected, this must be all zeros
    selected_config_is_wildcard [in/out] set to TRUE if the selected config
      is a wildcard (matches any ICCID); will be updated with the wildcard
      state of the candidate config if this function returns TRUE. Must be
      set to FALSE if no config is selected yet.

  RETURN VALUE
    TRUE if the candidate config is a better match than the selected config,
    FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_sel_config_is_better_match_by_carrier
(
  pdc_selection_carrier_enum_v01   carrier,
  const mcfg_config_info_s_type   *candidate_config,
  const mcfg_config_info_s_type   *selected_config
)
{
  boolean candidate_is_better_match = FALSE;
  mcfg_trl_category_s_type category;
  mcfg_trl_config_version_s_type candidate_version;
  char name[MCFG_TRL_CARRIER_NAME_MAX_LEN+1];

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(candidate_config);
  MCFG_CHECK_NULL_PTR_RET_FALSE(selected_config);

  MCFG_MSG_MED_1("Evaluating candidate config with len %d", candidate_config->config_len);
  if (mcfg_trl_decode_tlv(
        candidate_config, MCFG_TRL_TLV_ID_CARRIER_NAME, 
        name, MCFG_TRL_CARRIER_NAME_MAX_LEN+1))
  {
    MCFG_MSG_SPRINTF_1("Config: %s", name);
  }

  if (candidate_config->description_len == 0)
  {
    MCFG_MSG_ERROR("Candidate config trailer record is empty!");
  }
  else if (!mcfg_trl_decode_tlv(
             candidate_config, MCFG_TRL_TLV_ID_CATEGORY, 
             &category, sizeof(mcfg_trl_category_s_type)))
  {
    MCFG_MSG_ERROR("Couldn't decode category");
  }
  else if (!mcfg_trl_decode_tlv(candidate_config, MCFG_TRL_TLV_ID_CONFIG_VERSION,
           &candidate_version, sizeof(candidate_version)))
  {
    MCFG_MSG_ERROR("Couldn't decode config version");
  }
  else if ((carrier == candidate_version.maj_ver) && 
           ((category.category & MCFG_CATEGORY_BITMASK_SUBSI) > 0))
  {
    MCFG_MSG_MED("Candidate is direct carrier match");
    if (mcfg_sel_config_is_better_match_by_version(
          candidate_config, selected_config))
    {
      MCFG_MSG_MED("Candidate is the current best match");
      candidate_is_better_match = TRUE;
    }
  }

  return candidate_is_better_match;
} /* mcfg_sel_config_is_better_match_by_carrier() */

/*===========================================================================

  FUNCTION mcfg_sel_config_is_direct_iccid_match

  DESCRIPTION
    Compares the ICCID of the current UIM against a list of IINs (essentially
    ICCID prefixes) associated with a config.

  DEPENDENCIES
    None

  PARAMETERS
    uim_iccid       [in]
    config_iin_list [in]

  RETURN VALUE
    TRUE if one or more of the IINs in config_iin_list match uim_iccid,
    FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_sel_config_is_direct_iccid_match
(
  const char                     *uim_iccid,
  const mcfg_trl_iin_list_s_type *config_iin_list
)
{
  uint32 i;
  int ret;
  char iccid_prefix[MCFG_UIM_ICCID_STR_BUF_LEN];
  boolean is_direct_iccid_match = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(uim_iccid);
  MCFG_CHECK_NULL_PTR_RET_FALSE(config_iin_list);

  for (i = 0; i < config_iin_list->iin_list_count; i++)
  {
    ret = snprintf(iccid_prefix, sizeof(iccid_prefix), "%lu",
                   config_iin_list->iin_list[i]);
    MCFG_MSG_SPRINTF_1("Comparing ICCID against prefix '%s'", iccid_prefix);
    if (ret <= 0 || ret > sizeof(iccid_prefix))
    {
      MCFG_MSG_ERROR_1("Error converting IIN to string: %d", ret);
    }
    else if (strncmp(uim_iccid, iccid_prefix, ret) == 0)
    {
      is_direct_iccid_match = TRUE;
      break;
    }
  }

  return is_direct_iccid_match;
} /* mcfg_sel_config_is_direct_iccid_match() */

/*===========================================================================

  FUNCTION mcfg_sel_config_is_direct_iccid_match_with_iin_string

  DESCRIPTION
    Compares the ICCID of the current UIM against a list of IINs (essentially
    ICCID prefixes) associated with a config.

  DEPENDENCIES
    None

  PARAMETERS
    uim_iccid       [in]
    config_iin_list [in]

  RETURN VALUE
    TRUE if one or more of the IINs in config_iin_list match uim_iccid,
    FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_sel_config_is_direct_iccid_match_with_iin_string
(
  const char *uim_iccid,
  const char *iin_string
)
{
  char iin[MCFG_TRL_IIN_STRING_MAX_LEN+1];
  uint32 len, offset = 0;
  boolean is_direct_iccid_match = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(uim_iccid);
  MCFG_CHECK_NULL_PTR_RET_FALSE(iin_string);

  while (mcfg_utils_get_phrase((char*)iin_string, &offset, MCFG_TRL_IIN_STRING_MAX_LEN+1, iin))
  {
    len = strlen(iin);

    MCFG_MSG_SPRINTF_1("comparing ICCID against IIN string '%s'", iin);
    if (strlen(uim_iccid) > len && strncasecmp(uim_iccid, iin, len)==0)
    {
      MCFG_MSG_SPRINTF_1("match found for ICCID '%s'", uim_iccid);
      is_direct_iccid_match = TRUE;
      break;
    }
  }

  return is_direct_iccid_match;
} /* mcfg_sel_config_is_direct_iccid_match_with_iin_string() */

/*===========================================================================

  FUNCTION mcfg_sel_config_is_direct_plmn_match

  DESCRIPTION
    Compares the PLMN of the current UIM against a list of PLMNs (essentially
    PLMN prefixes) associated with a config.

  DEPENDENCIES
    None

  PARAMETERS
    uim_plmn       [in]
    config_plmn_list [in]

  RETURN VALUE
    TRUE if one or more of the PLMNs in config_plmn_list match uim_plmn,
    FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_sel_config_is_direct_plmn_match
(
  const mcfg_trl_carrier_mcc_mnc_s_type *uim_plmn,
  const mcfg_trl_plmn_list_s_type       *config_plmn_list
)
{
  uint32 i;
  boolean is_direct_plmn_match = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(uim_plmn);
  MCFG_CHECK_NULL_PTR_RET_FALSE(config_plmn_list);

  for (i = 0; i < config_plmn_list->plmn_list_count; i++)
  {
    MCFG_MSG_MED_4("Comparing PLMN %d:%d against prefix %d:%d", 
                   uim_plmn->mcc,
                   uim_plmn->mnc,
                   config_plmn_list->plmn_list[i].mcc,
                   config_plmn_list->plmn_list[i].mnc);
    if (config_plmn_list->plmn_list[i].mcc == uim_plmn->mcc &&
        config_plmn_list->plmn_list[i].mnc == uim_plmn->mnc)
    {
      is_direct_plmn_match = TRUE;
      break;
    }
  }

  return is_direct_plmn_match;
} /* mcfg_sel_config_is_direct_plmn_match() */

/*===========================================================================

  FUNCTION mcfg_sel_find_best_match_by_iccid

  DESCRIPTION
    Finds the best available software config for the current UIM based on
    ICCID.

  DEPENDENCIES
    None

  PARAMETERS
    iccid_ascii [in] Valid ICCID of the UIM currently inserted in slot 1;
                     ASCII encoding
    config_list [in] List of available configs
    num_configs [in] Number of available configs

  RETURN VALUE
    Index in config_list array of the best available config, or
    MCFG_CONFIG_LIST_SIZE_MAX if no suitable match found.

  SIDE EFFECTS
    None

===========================================================================*/
static uint32 mcfg_sel_find_best_match_by_iccid
(
  const char            *iccid_ascii,
  mcfg_config_id_s_type *config_list,
  uint32                 num_configs
)
{
  uint32 i;
  uint32 selected_config_index = MCFG_CONFIG_LIST_SIZE_MAX;
  mcfg_config_info_s_type selected_config_info;
  mcfg_config_info_s_type current_config_info;
  boolean selected_config_is_wildcard = FALSE;
  mcfg_trl_category_s_type category;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&selected_config_info, 0, sizeof(selected_config_info));
  for (i = 0; i < num_configs; i++)
  {
    if (!mcfg_utils_get_config_info(
          MCFG_TYPE_SW, &config_list[i], &current_config_info))
    {
      MCFG_MSG_ERROR_1("Couldn't get config info at index %lu", i);
    }
    else 
    {
      if (mcfg_trl_decode_tlv(&current_config_info, MCFG_TRL_TLV_ID_CATEGORY, 
             &category, sizeof(mcfg_trl_category_s_type)))
      {
        if ((category.category & MCFG_CATEGORY_BITMASK_SUBSI) > 0)
        {
          MCFG_MSG_LOW_1("subsidized MBN skip %d", i);
          continue;
        }
      }
      if (mcfg_sel_config_is_better_match_by_iccid(
               iccid_ascii, &current_config_info, &selected_config_info,
               &selected_config_is_wildcard))
      {
      selected_config_index = i;
      memscpy(&selected_config_info, sizeof(mcfg_config_info_s_type),
              &current_config_info, sizeof(mcfg_config_info_s_type));
      }
    }
  }

  if (selected_config_index == MCFG_CONFIG_LIST_SIZE_MAX)
  {
    MCFG_MSG_HIGH("No configs available for current ICCID");
  }

  return selected_config_index;
} /* mcfg_sel_find_best_match_by_iccid() */

#ifdef FEATURE_MCFG_SELECTION_BY_IMSI
/*===========================================================================

  FUNCTION mcfg_sel_find_best_match_by_plmn

  DESCRIPTION
    Finds the best available software config for the current UIM based on
    plmn.

  DEPENDENCIES
    None

  PARAMETERS
    plmn        [in] Valid plmn of the UIM 
    config_list [in] List of available configs
    num_configs [in] Number of available configs

  RETURN VALUE
    Index in config_list array of the best available config, or
    MCFG_CONFIG_LIST_SIZE_MAX if no suitable match found.

  SIDE EFFECTS
    None

===========================================================================*/
static uint32 mcfg_sel_find_best_match_by_plmn
(
  mcfg_trl_carrier_mcc_mnc_s_type           *plmn,
  mcfg_config_id_s_type                     *config_list,
  uint32                                    num_configs
)
{
  uint32 i;
  uint32 selected_config_index = MCFG_CONFIG_LIST_SIZE_MAX;
  mcfg_config_info_s_type selected_config_info;
  mcfg_config_info_s_type current_config_info;
  boolean selected_config_is_wildcard = FALSE;
  mcfg_trl_category_s_type category;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&selected_config_info, 0, sizeof(selected_config_info));
  for (i = 0; i < num_configs; i++)
  {
    if (!mcfg_utils_get_config_info(
          MCFG_TYPE_SW, &config_list[i], &current_config_info))
    {
      MCFG_MSG_ERROR_1("Couldn't get config info at index %lu", i);
    }
    else
      if (mcfg_trl_decode_tlv(&current_config_info, MCFG_TRL_TLV_ID_CATEGORY, 
             &category, sizeof(mcfg_trl_category_s_type)))
      {
        if ((category.category & MCFG_CATEGORY_BITMASK_SUBSI) > 0)
        {
          MCFG_MSG_LOW_1("subsidized MBN skip %d", i);
          continue;
        }
      }
    {


      if (mcfg_sel_config_is_better_match_by_plmn(
               plmn, &current_config_info, &selected_config_info,
               &selected_config_is_wildcard))
      {
      selected_config_index = i;
      memscpy(&selected_config_info, sizeof(mcfg_config_info_s_type),
              &current_config_info, sizeof(mcfg_config_info_s_type));
      }
    }
  }

  if (selected_config_index == MCFG_CONFIG_LIST_SIZE_MAX)
  {
    MCFG_MSG_HIGH("No configs available for current plmn");
  }

  return selected_config_index;
} /* mcfg_sel_find_best_match_by_plmn() */
#endif /* FEATURE_MCFG_SELECTION_BY_IMSI */

/*===========================================================================

  FUNCTION mcfg_sel_find_best_match_by_carrier

  DESCRIPTION
    Finds the best available software config for the current UIM based on
    carrier.

  DEPENDENCIES
    None

  PARAMETERS
    carrier     [in] Valid carrier of the UIM 
    config_list [in] List of available configs
    num_configs [in] Number of available configs

  RETURN VALUE
    Index in config_list array of the best available config, or
    MCFG_CONFIG_LIST_SIZE_MAX if no suitable match found.

  SIDE EFFECTS
    None

===========================================================================*/
static uint32 mcfg_sel_find_best_match_by_carrier
(
  pdc_selection_carrier_enum_v01             carrier,
  mcfg_config_id_s_type                     *config_list,
  uint32                                    num_configs
)
{
  uint32 i;
  uint32 selected_config_index = MCFG_CONFIG_LIST_SIZE_MAX;
  mcfg_config_info_s_type selected_config_info;
  mcfg_config_info_s_type current_config_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&selected_config_info, 0, sizeof(selected_config_info));
  for (i = 0; i < num_configs; i++)
  {
    if (!mcfg_utils_get_config_info(
          MCFG_TYPE_SW, &config_list[i], &current_config_info))
    {
      MCFG_MSG_ERROR_1("Couldn't get config info at index %lu", i);
    }
    else if (mcfg_sel_config_is_better_match_by_carrier(
               carrier, &current_config_info, &selected_config_info))
    {
      selected_config_index = i;
      memscpy(&selected_config_info, sizeof(mcfg_config_info_s_type),
              &current_config_info, sizeof(mcfg_config_info_s_type));
    }
  }

  if (selected_config_index == MCFG_CONFIG_LIST_SIZE_MAX)
  {
    MCFG_MSG_HIGH("No configs available for current carrier");
  }

  return selected_config_index;
} /* mcfg_sel_find_best_match_by_carrier() */

/*===========================================================================

  FUNCTION mcfg_sel_iccid_matches_last

  DESCRIPTION
    Checks whether the given ICCID is the same ICCID as we last used for
    ICCID-based autoselection.

  DEPENDENCIES
    None

  PARAMETERS
    iccid_ascii [in] Valid ICCID of the UIM currently inserted in slot 1;
                     ASCII encoding

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_sel_iccid_matches_last
(
  mcfg_sub_id_type_e_type sub_id,
  const char             *iccid_ascii
)
{
  char last_iccid[MCFG_UIM_ICCID_STR_BUF_LEN];
  mcfg_nv_status_e_type status;
  boolean matches = FALSE;
  mcfg_slot_index_type_e_type slot_index;
  mcfg_config_id_s_type  config;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  slot_index = mcfg_uim_map_sub_id_to_slot_index(sub_id);
  MCFG_ASSERT_COND_RET_FALSE(slot_index < MCFG_NUM_OF_UIM_SLOT);

  if (!mcfg_utils_get_active_config(MCFG_TYPE_SW, &config, sub_id))
  {
    MCFG_MSG_LOW_1("there's not active config available for sloti %d, iccid reselect", slot_index);
    return FALSE;
  }

  if (mcfg_utils_selected_config_available(MCFG_TYPE_SW, sub_id))
  {
    MCFG_MSG_LOW_1("there's pending config available for sloti %d, iccid reselect", slot_index);
    return FALSE;
  }

  status = mcfg_nv_read(MCFG_NV_ITEM_LAST_AUTOSELECT_ICCID, 0, slot_index, 
                        last_iccid, sizeof(last_iccid));
  if (status != MCFG_NV_STATUS_OK)
  {
    if (status != MCFG_NV_STATUS_NOT_ACTIVE)
    {
      MCFG_MSG_ERROR_1("Couldn't read last autoselect ICCID NV item: status %d",
                       status);
    }
  }
  else if (strncmp(last_iccid, iccid_ascii, sizeof(last_iccid)) == 0)
  {
    matches = TRUE;
  }

  return matches;
} /* mcfg_sel_iccid_matches_last() */

#ifdef FEATURE_MCFG_SELECTION_BY_IMSI
/*===========================================================================

  FUNCTION mcfg_sel_plmn_matches_last

  DESCRIPTION
    Checks whether the given PLMN is the same PLMN as we last used for
    ICCID-based autoselection.

  DEPENDENCIES
    None

  PARAMETERS
    iccid_ascii [in] Valid ICCID of the UIM currently inserted in slot 1;
                     ASCII encoding

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_sel_plmn_matches_last
(
  mcfg_sub_id_type_e_type          sub_id,
  mcfg_trl_carrier_mcc_mnc_s_type *plmn
)
{
  mcfg_trl_carrier_mcc_mnc_s_type last_plmn;
  mcfg_slot_index_type_e_type sloti;
  mcfg_nv_status_e_type status;
  boolean matches = FALSE;
  mcfg_config_id_s_type  config;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sloti = mcfg_uim_map_sub_id_to_slot_index(sub_id);
  MCFG_ASSERT_COND_RET_FALSE(sloti < MCFG_NUM_OF_UIM_SLOT);

  if (!mcfg_utils_get_active_config(MCFG_TYPE_SW, &config, sub_id))
  {
    MCFG_MSG_LOW_1("there's not active config available for sloti %d, imsi reselect", sloti);
    return FALSE;
  }

  if (mcfg_utils_selected_config_available(MCFG_TYPE_SW, sub_id))
  {
    MCFG_MSG_LOW_1("there's pending config available for sloti %d, imsi reselect", sloti);
    return FALSE;
  }

  status = mcfg_nv_read(MCFG_NV_ITEM_LAST_AUTOSELECT_PLMN, 0, (mcfg_sub_id_type_e_type)sloti, 
                        &last_plmn, sizeof(last_plmn));
  if (status != MCFG_NV_STATUS_OK)
  {
    if (status != MCFG_NV_STATUS_NOT_ACTIVE)
    {
      MCFG_MSG_ERROR_1("Couldn't read last autoselect plmn NV item: status %d",
                       status);
    }
  }
  else if (memcmp(plmn, &last_plmn, sizeof(last_plmn)) == 0)
  {
    matches = TRUE;
  }

  return matches;
} /* mcfg_sel_plmn_matches_last() */
#endif /* FEATURE_MCFG_SELECTION_BY_IMSI */

/*===========================================================================

  FUNCTION mcfg_sel_save_iccid

  DESCRIPTION
    Saves the given ICCID to NV cache as the last ICCID we ran
    auto-selection with.

  DEPENDENCIES
    None

  PARAMETERS
    iccid_ascii [in] Valid ICCID of the UIM currently inserted in slot 1;
                     ASCII encoding. Buffer size must be at least
                     MCFG_UIM_ICCID_STR_BUF_LEN bytes, and string must be
                     NULL-terminated

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void mcfg_sel_save_iccid
(
  mcfg_sub_id_type_e_type sub_id,
  const char             *iccid_ascii
)
{
  mcfg_nv_status_e_type status;
  mcfg_slot_index_type_e_type slot_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET(iccid_ascii);

  slot_index = mcfg_uim_map_sub_id_to_slot_index(sub_id);
  MCFG_ASSERT_COND_RET(slot_index < MCFG_NUM_OF_UIM_SLOT);

  status = mcfg_nv_write(MCFG_NV_ITEM_LAST_AUTOSELECT_ICCID, 0, slot_index, 
                         (void *) iccid_ascii, MCFG_UIM_ICCID_STR_BUF_LEN);
  if (status != MCFG_NV_STATUS_OK)
  {
    MCFG_MSG_ERROR_1("Couldn't write last autoselect ICCID NV item: status %d",
                     status);
  }
} /* mcfg_sel_save_iccid() */

#ifdef FEATURE_MCFG_SELECTION_BY_IMSI
/*===========================================================================

  FUNCTION mcfg_sel_save_plmn

  DESCRIPTION
    Saves the given plmn to NV cache as the last plmn we ran
    auto-selection with.

  DEPENDENCIES
    None

  PARAMETERS
    plmn  [in] valid plmn of the UIM for specified subscription

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void mcfg_sel_save_plmn
(
  mcfg_sub_id_type_e_type          sub_id,
  mcfg_trl_carrier_mcc_mnc_s_type *plmn
)
{
  mcfg_nv_status_e_type status;
  mcfg_slot_index_type_e_type sloti;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET(plmn);

  sloti = mcfg_uim_map_sub_id_to_slot_index(sub_id);
  MCFG_ASSERT_COND_RET(sloti < MCFG_NUM_OF_UIM_SLOT);

  status = mcfg_nv_write(MCFG_NV_ITEM_LAST_AUTOSELECT_PLMN, 0, (mcfg_sub_id_type_e_type)sloti, 
                         (void *) plmn, sizeof(mcfg_trl_carrier_mcc_mnc_s_type));
  if (status != MCFG_NV_STATUS_OK)
  {
    MCFG_MSG_ERROR_1("Couldn't write last autoselect PLMN NV item: status %d",
                     status);
  }
} /* mcfg_sel_save_plmn() */
#endif /* FEATURE_MCFG_SELECTION_BY_IMSI */

/*===========================================================================

  FUNCTION mcfg_sel_select_sw_config

  DESCRIPTION
    If the given software configuration is different from the active config,
    then sets it as the new active config.

  DEPENDENCIES
    None

  PARAMETERS
    config_id [in]

  RETURN VALUE
    None

  SIDE EFFECTS
    May trigger a device reset to active a new config

===========================================================================*/
static void mcfg_sel_select_sw_config
(
  mcfg_sub_id_type_e_type sub_id,
  mcfg_config_id_s_type  *config_id,
  mcfg_activation_source_e_type source
)
{
  mcfg_config_id_s_type active_config_id;
#ifdef FEATURE_MCFG_REFRESH
  mcfg_refresh_state_e_type state;
  uint8 slot;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_MSG_HIGH_3("Autoselect picked config with ID %02X%02X%02X",
                  config_id->id[0], config_id->id[1], config_id->id[2]);
  mcfg_utils_print_config(MCFG_TYPE_SW, config_id);

  if (mcfg_utils_get_active_config(MCFG_TYPE_SW, &active_config_id, sub_id) &&
      mcfg_sel_config_ids_match(config_id, &active_config_id))
  {
    MCFG_MSG_HIGH("Already using correct config");
  }
  else if (!mcfg_utils_set_selected_config(MCFG_TYPE_SW, config_id, sub_id))
  {
    MCFG_MSG_ERROR("Couldn't set selected config!");
  }
  else if (source == MCFG_ACTIVATION_SUBSI_CARRIER && sub_id == MCFG_SUB_ID_FIRST &&
           mcfg_utils_get_active_config(MCFG_TYPE_HW, &active_config_id, MCFG_SUB_ID_FIRST))
  {
    MCFG_MSG_HIGH("Selected config based on carrier on sub0, reload HW config");
    mcfg_utils_deactivate_config(MCFG_TYPE_HW, MCFG_SUB_ID_FIRST, TRUE);
    mcfg_utils_set_selected_config(MCFG_TYPE_HW, &active_config_id, MCFG_SUB_ID_FIRST);
    mcfg_utils_activate_config(MCFG_TYPE_HW, MCFG_ACTIVATION_SUBSI_CARRIER);
  }
  else
  {
    MCFG_MSG_MED("Activating new config");
#ifdef FEATURE_MCFG_REFRESH

    slot = mcfg_refresh_sub_to_slot_index((mcfg_refresh_index_type)sub_id);
    if(mcfg_refresh_autoselect_enabled(slot))
    {
      state = mcfg_refresh_get_state(slot);
      if (state == MCFG_REFRESH_STATE_ACTIVATING_SLOT && source == MCFG_ACTIVATION_ICCID_SELECT)
      {
        MCFG_MSG_MED_1("Refresh clients for slot %d trigged by ICCID selection .....", slot);
        mcfg_utils_deactivate_config(MCFG_TYPE_SW, sub_id, TRUE);
        modem_cfg_process_single(MCFG_TYPE_SW, MCFG_REFRESH_TYPE_SLOT, (mcfg_refresh_index_type)sub_id);
        mcfg_refresh_set_state((mcfg_refresh_index_type)slot, MCFG_REFRESH_STATE_UPDATING_SLOT);
        mcfg_refresh_clients(MCFG_REFRESH_TYPE_SLOT, (mcfg_refresh_index_type)slot);
      }
      else if (source == MCFG_ACTIVATION_IMSI_SELECT)
      {
        MCFG_MSG_MED("Activating new config triggered by IMSI selection .....");
        mcfg_utils_activate_config(MCFG_TYPE_SW, MCFG_ACTIVATION_IMSI_SELECT);
      }
      else if (state == MCFG_REFRESH_STATE_IDLE)
      {
        MCFG_MSG_MED("Activating new config triggered by ICCID selection .....");
        mcfg_utils_activate_config(MCFG_TYPE_SW, MCFG_ACTIVATION_ICCID_SELECT);
      }
      else
      {
        MCFG_MSG_ERROR_2("Invalid state %d for slot %d.....", state, slot);
      }
    }
    else
#endif
    {
#ifndef REMOVE_QMI_PDC_CONFIG_CHANGE_IND_V01
      pdc_generate_config_change_ind(MCFG_TYPE_SW, config_id);
#endif
      if (mcfg_utils_activate_config(MCFG_TYPE_SW, source) != MCFG_ERR_NONE)
      {
        MCFG_MSG_ERROR_1("Couldn't activate config! source %d", source);
      }
    }
  }
} /* mcfg_sel_select_sw_config() */

/*==============================================================================
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================

  FUNCTION mcfg_sel_sw_config_store_updated

  DESCRIPTION
    Notifies the automatic configuration selection module that the stored
    SW configuration files have changed, e.g. new config uploaded, config
    deleted, etc. Ensures that automatic selection logic will run again
    at next boot to verify that we are using the best available config.

  DEPENDENCIES
    None

  PARAMETERS
    slot_index

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_sel_sw_config_store_updated
(
  mcfg_slot_index_type_e_type slot_index
)
{
  mcfg_slot_index_type_e_type start, end, i;
  mcfg_nv_status_e_type status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET(slot_index <= MCFG_NUM_OF_UIM_SLOT);

  if (slot_index < MCFG_NUM_OF_UIM_SLOT)
  {
    start = slot_index;
    end = (slot_index+1);
    MCFG_MSG_MED_1("Last saved ICCID and IMSI NVs are deleted for sloti %d", slot_index);
  }
  else
  {
    start = MCFG_SLOT_INDEX_FIRST;
    end = MCFG_NUM_OF_UIM_SLOT;
    MCFG_MSG_MED("Last saved ICCID and IMSI NVs are deleted for all slots");
  }

  /* Erase the cached ICCID/PLMN so auto selection will run on next reboot */
  for (i = start; i < end; i++)
  {
    status = mcfg_nv_delete(MCFG_NV_ITEM_LAST_AUTOSELECT_ICCID, 0, i);
    if (status != MCFG_NV_STATUS_OK && status != MCFG_NV_STATUS_NOT_ACTIVE)
    {
      MCFG_MSG_ERROR_1("Couldn't cleanup last autoselect ICCID NV item: status %d",
                       status);
    }
#ifdef FEATURE_MCFG_SELECTION_BY_IMSI
    status = mcfg_nv_delete(MCFG_NV_ITEM_LAST_AUTOSELECT_PLMN, 0, i);
    if (status != MCFG_NV_STATUS_OK && status != MCFG_NV_STATUS_NOT_ACTIVE)
    {
      MCFG_MSG_ERROR_1("Couldn't cleanup last autoselect PLMN NV item: status %d",
                       status);
    }
#endif /*FEATURE_MCFG_SELECTION_BY_IMSI*/
  }
} /* mcfg_sel_sw_config_store_updated() */

/*===========================================================================

  FUNCTION mcfg_sel_sw_by_iccid

  DESCRIPTION
    Searches for a MCFG_SW matching the parameters for the given ICCID. If
    a match is found, the MCFG_SW will be selected and activated (if not
    already active).

  DEPENDENCIES
    None

  PARAMETERS
    iccid_ascii [in] Valid ICCID of the UIM currently inserted in slot 1;
                     ASCII encoding. Buffer size must be at least
                     MCFG_UIM_ICCID_STR_BUF_LEN bytes, and string must be
                     NULL-terminated

  RETURN VALUE
    None

  SIDE EFFECTS
    May result in a system reboot if the configuration is to be changed

===========================================================================*/
void mcfg_sel_sw_by_iccid
(
  mcfg_sub_id_type_e_type sub_id,
  const char             *iccid_ascii
)
{
  uint32 num_configs;
  mcfg_config_id_s_type *config_list;
  uint32 selected_config_index;
  mcfg_slot_index_type_e_type sloti;
  mcfg_config_id_s_type active_config_id;
  mcfg_config_id_s_type selected_config_id;
#ifdef FEATURE_MCFG_HW_CONFIG_REQUIRED
  mcfg_config_id_s_type   config_id;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET(iccid_ascii);

  sloti = mcfg_uim_map_sub_id_to_slot_index(sub_id);
  MCFG_ASSERT_COND_RET(sloti < MCFG_NUM_OF_UIM_SLOT);

  if(!mcfg_uim_nv_autoselect_enabled(sloti))
  {
    MCFG_MSG_MED("Selection based on ICCID is disabled");
    return;
  }

  if (mcfg_sel_iccid_matches_last(sub_id, iccid_ascii))
  {
    MCFG_MSG_HIGH("ICCID matches cache from last autoselect");
  }
#ifdef FEATURE_MCFG_HW_CONFIG_REQUIRED
  else if (!mcfg_utils_get_active_config(MCFG_TYPE_HW, &config_id, MCFG_SUB_ID_FIRST))
  {
    MCFG_MSG_ERROR("Platform MBN has to be activated first before auto-selection");
  }
#endif
  else
  {
    MCFG_MSG_HIGH_1("Running autoselect logic for ICCID: %s", iccid_ascii);
    config_list = mcfg_malloc(
      sizeof(mcfg_config_id_s_type) * MCFG_CONFIG_LIST_SIZE_MAX);
    if (config_list == NULL)
    {
      MCFG_MSG_ERROR("Couldn't allocate memory for config list!");
    }
    else
    {
      num_configs = mcfg_utils_list_configs(
        MCFG_TYPE_SW, MCFG_CONFIG_LIST_SIZE_MAX, config_list);
      if (num_configs == 0)
      {
        MCFG_MSG_HIGH("No configs present");
      }
      else
      {
        selected_config_index = mcfg_sel_find_best_match_by_iccid(
          iccid_ascii, config_list, num_configs);
        if (selected_config_index < MCFG_CONFIG_LIST_SIZE_MAX)
        {
          mcfg_sel_select_sw_config(sub_id, 
                                    &config_list[selected_config_index],
                                    MCFG_ACTIVATION_ICCID_SELECT);
        }
      }

      /* Clear the last saved ICCID and IMSI NVs if MBN is getting updated as part of ICCID selection */
      if (mcfg_utils_selected_config_available(MCFG_TYPE_SW, sub_id))
      {
        MCFG_MSG_HIGH_1("MBN being updated as part of ICCID selection. Clean up saved ICCID and IMSI NVs, slot %d", sloti);
        mcfg_sel_sw_config_store_updated(sloti);
      }

      mcfg_sel_save_iccid(sub_id, iccid_ascii);
      mcfg_free(config_list);
    }
  }
} /* mcfg_sel_sw_by_iccid() */

#ifdef FEATURE_MCFG_SELECTION_BY_IMSI
/*===========================================================================

  FUNCTION mcfg_sel_sw_by_plmn

  DESCRIPTION
    Searches for a MCFG_SW matching the parameters for the given plmn. If
    a match is found, the MCFG_SW will be selected and activated (if not
    already active).

  DEPENDENCIES
    None

  PARAMETERS
    plmn [in] Valid plmn of the UIM currently inserted for specified
              subscription

  RETURN VALUE
    None

  SIDE EFFECTS
    May result in a system reboot if the configuration is to be changed

===========================================================================*/
void mcfg_sel_sw_by_plmn
(
  mcfg_sub_id_type_e_type          sub_id,
  mcfg_trl_carrier_mcc_mnc_s_type *plmn
)
{
  uint32 num_configs;
  mcfg_config_id_s_type *config_list;
  uint32 selected_config_index;
  mcfg_slot_index_type_e_type sloti;
#ifdef FEATURE_MCFG_HW_CONFIG_REQUIRED
  mcfg_config_id_s_type   config_id;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET(plmn);
  sloti = mcfg_uim_map_sub_id_to_slot_index(sub_id);
  MCFG_ASSERT_COND_RET(sloti < MCFG_NUM_OF_UIM_SLOT);

  if(!mcfg_uim_nv_imsiselect_enabled(sloti))
  {
    MCFG_MSG_MED("Selection based on IMSI is disabled");
    return;
  }

  if (mcfg_sel_plmn_matches_last(sub_id, plmn))
  {
    MCFG_MSG_HIGH("PLMN matches cache from last autoselect");
  }
#ifdef FEATURE_MCFG_HW_CONFIG_REQUIRED
  else if (!mcfg_utils_get_active_config(MCFG_TYPE_HW, &config_id, MCFG_SUB_ID_FIRST))
  {
    MCFG_MSG_ERROR("Platform MBN has to be activated first before auto-selection");
  }
#endif
  else
  {
    MCFG_MSG_HIGH_2("Running autoselect logic for mcc=%d mnc=%d", 
                    plmn->mcc, plmn->mnc);
    config_list = mcfg_malloc(
      sizeof(mcfg_config_id_s_type) * MCFG_CONFIG_LIST_SIZE_MAX);
    if (config_list == NULL)
    {
      MCFG_MSG_ERROR("Couldn't allocate memory for config list!");
    }
    else
    {
      num_configs = mcfg_utils_list_configs(
        MCFG_TYPE_SW, MCFG_CONFIG_LIST_SIZE_MAX, config_list);
      if (num_configs == 0)
      {
        MCFG_MSG_HIGH("No configs present");
      }
      else
      {
        selected_config_index = mcfg_sel_find_best_match_by_plmn(
          plmn, config_list, num_configs);
        if (selected_config_index < MCFG_CONFIG_LIST_SIZE_MAX)
        {
          mcfg_sel_select_sw_config(sub_id, 
                                    &config_list[selected_config_index],
                                    MCFG_ACTIVATION_IMSI_SELECT);
        }
      }
      mcfg_sel_save_plmn(sub_id, plmn);
      mcfg_free(config_list);
    }
  }
} /* mcfg_sel_sw_by_plmn */
#endif /* FEATURE_MCFG_SELECTION_BY_IMSI */

/*===========================================================================

  FUNCTION mcfg_sel_sw_by_carrier

  DESCRIPTION
    Searches for a subsidized Carrier MBN 

  DEPENDENCIES
    None

  PARAMETERS
    plmn [in] Carrier Index

  RETURN VALUE
    None

  SIDE EFFECTS
    May result in a system reboot if the configuration is to be changed

===========================================================================*/
void mcfg_sel_sw_by_carrier
(
  mcfg_sub_id_type_e_type          sub_id,
  pdc_selection_carrier_enum_v01   carrier
)
{
  uint32 num_configs;
  mcfg_config_id_s_type *config_list;
  uint32 selected_config_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET(carrier != PDC_SELECTION_OPEN_MARKET_V01);

  MCFG_MSG_HIGH_2("Running autoselect logic for subi %d carrier %d", 
                  sub_id, carrier);
  config_list = mcfg_malloc(
    sizeof(mcfg_config_id_s_type) * MCFG_CONFIG_LIST_SIZE_MAX);
  if (config_list == NULL)
  {
    MCFG_MSG_ERROR("Couldn't allocate memory for config list!");
  }
  else
  {
    num_configs = mcfg_utils_list_configs(
      MCFG_TYPE_SW, MCFG_CONFIG_LIST_SIZE_MAX, config_list);
    if (num_configs == 0)
    {
      MCFG_MSG_HIGH("No configs present");
    }
    else
    {
      selected_config_index = mcfg_sel_find_best_match_by_carrier(
        carrier, config_list, num_configs);
      if (selected_config_index < MCFG_CONFIG_LIST_SIZE_MAX)
      {
        mcfg_sel_select_sw_config(sub_id, 
                                  &config_list[selected_config_index],
                                  MCFG_ACTIVATION_SUBSI_CARRIER);
      }
    }
    mcfg_free(config_list);
  }
} /* mcfg_sel_sw_by_carrier */

