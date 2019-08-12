#ifndef _POLICYMAN_RF_H_
#define _POLICYMAN_RF_H_

/**
  @file policyman_rf.h

  @brief  RAT and RF band information
*/

/*
    Copyright (c) 2013-2016 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_rf.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "sd_v.h"
#include "mre_namedobj.h"
#include "policyman_i.h"
#include "policyman_diag.h"
#include "policyman_set.h"



/*-------- rf_band_set_t --------*/
typedef struct
{
  sys_band_mask_type        gwBands;
  sys_lte_band_mask_e_type  lteBands;
  sys_band_mask_type        tdsBands;
} rf_band_set_t;

/*-------- rf_efs_bands_t --------*/
typedef struct
{
  rf_band_set_t bands;
  rf_band_set_t excludeForVolte;
} rf_efs_bands_t;

/*-------- policyman_rf_device_info_t --------*/
typedef struct
{
  uint32                    ratMask;
  sys_band_mask_type        gwBands;
  sys_lte_band_mask_e_type  lteBands;
  sys_band_mask_type        tdsBands;
} policyman_rf_device_info_t;


typedef struct policyman_rf_info_t  policyman_rf_info_t;


typedef struct
{
  POLICYMAN_NAMED_OBJECT_BASE;

  rf_band_set_t bands;
  rf_band_set_t exclude;
  rf_band_set_t excludeForVolte;
} rf_bands_t;



/*-------- policyman_rf_init --------*/
/**
@brief

@param

@return

*/
void policyman_rf_init(
  void
  );


/*-------- policyman_rf_deinit --------*/
void policyman_rf_deinit(
void
  );

/*-------- policyman_get_hardware_bands_all_techs --------*/
void
policyman_get_hardware_bands_all_techs(
  sys_band_mask_type       *pGwBands,
  sys_lte_band_mask_e_type *pLteBands,
  sys_band_mask_type       *pTdsBands
  );


/*-------- policyman_rf_bands_item_new --------*/
/**
@brief

@param

@return

*/
policyman_item_t * policyman_rf_bands_item_new(
  rf_efs_bands_t  *pBands
);


/*-------- policyman_rf_band_list_action_new --------*/
/**
@brief

@param

@return

*/
mre_status_t
policyman_rf_band_list_action_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
  );



/*-------- policyman_rf_bands_action_new --------*/
/**
@brief

@param

@return

*/
mre_status_t
policyman_rf_bands_action_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
  );

/*-------- policyman_rf_bands_if_action_new --------*/
/**
@brief

@param

@return

*/
mre_status_t policyman_rf_bands_if_action_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
  );


/*=============================================================================
  Methods for the RF band configuration item
=============================================================================*/

/*-------- policyman_rf_bands_get_default_from_efs --------*/
/**
@brief

@param

@return

*/
policyman_status_t policyman_rf_bands_get_default_from_efs(
  cfgitem_execute_ctx_t *pCfgCtx
);


/*-------- policyman_rf_bands_get_default --------*/
/**
@brief

@param

@return

*/
policyman_item_t * policyman_rf_bands_get_default(
  policy_execute_ctx_t *pCtx
  );


/*-------- policyman_rf_bands_compare --------*/
/**
@brief

@param

@return

*/
boolean
policyman_rf_bands_compare(
  policyman_item_t  *pi1,
  policyman_item_t  *pi2
  );


/*-------- policyman_rf_print_bands --------*/
/**
@brief

@param

@return

*/
void policyman_rf_print_bands(
  sys_modem_as_id_e_type    subsId,
  sys_band_mask_type        gwBands,
  sys_lte_band_mask_e_type  lteBands,
  sys_band_mask_type        tdsBands
);

/*-------- policyman_rf_print_lte_bands --------*/
/**
@brief

@param

@return

*/
void policyman_rf_print_lte_bands(
  sys_modem_as_id_e_type    subsId,
  sys_lte_band_mask_e_type  lteBands
);

/*-------- policyman_rf_bands_display --------*/
/**
@brief

@param

@return

*/
void
policyman_rf_bands_display(
  policyman_item_t  *pItem
  );



/*=============================================================================
  Accessor functions to read from configuration items
=============================================================================*/

/*-------- policyman_rf_get_overall_info --------*/
/**
@brief

@param

@return

*/
policyman_rf_device_info_t *
policyman_rf_get_overall_info(
  void
  );


/*-------- policyman_rf_get_overall_rat_mask --------*/
uint32
policyman_rf_get_overall_rat_mask(
  void
  );


/*-------- policyman_get_bands_for_ueMode --------*/
/**
@brief

@param

@return

*/
rf_bands_item_t *
policyman_get_bands_for_ueMode(
   sys_ue_mode_e_type current_ue_mode,
   policyman_policy_t *pPolicy
   );


/*-------- policyman_get_bands_item_for_named_bands --------*/
rf_bands_item_t *
policyman_get_bands_item_for_named_bands(
  rf_bands_t  *pBands
  );

/*-------- policyman_get_bands_for_named_object --------*/
void
policyman_get_bands_for_named_object(
  rf_bands_t         *pBands,
  rf_bands_t         *pGetBands
);


/*-------- policyman_rf_bands_filter_based_on_rat --------*/
void
policyman_rf_bands_filter_based_on_rat(
  rf_bands_item_t  *prfb,
  uint32           ratMask
);

/*-------- policyman_rf_band_list_read --------*/
boolean
policyman_rf_band_list_read(
  policyman_xml_element_t const *pElem,
  rf_bands_t                    *pBands
  );


/*-------- policyman_rf_bands_fill_subs_diag_log --------*/
void policyman_rf_bands_fill_subs_diag_log(
  policyman_item_t const  *pItem,
  LOG_PM_SUBS_INFO_type   *log_ptr
);

/*-------- policyman_retrieve_rats_bands --------*/
boolean policyman_retrieve_rats_bands(
  sys_modem_as_id_e_type     subsId,
  uint32                    *pRatMask,
  rf_efs_bands_t            *pRetrievedBands
);

/*-----------------------------------------------------------------------------
  Band masks used in policyman_rf.c and also in unit tests.
-----------------------------------------------------------------------------*/


#define ALL_CDMA_BANDS        \
  (                       \
    SD_SS_BAND_PREF_BC0   \
  | SD_SS_BAND_PREF_BC1   \
  | SD_SS_BAND_PREF_BC3   \
  | SD_SS_BAND_PREF_BC4   \
  | SD_SS_BAND_PREF_BC5   \
  | SD_SS_BAND_PREF_BC6   \
  | SD_SS_BAND_PREF_BC7   \
  | SD_SS_BAND_PREF_BC8   \
  | SD_SS_BAND_PREF_BC9   \
  | SD_SS_BAND_PREF_BC10  \
  | SD_SS_BAND_PREF_BC11  \
  | SD_SS_BAND_PREF_BC12  \
  | SD_SS_BAND_PREF_BC14  \
  | SD_SS_BAND_PREF_BC15  \
  | SD_SS_BAND_PREF_BC16  \
  | SD_SS_BAND_PREF_BC17  \
  | SD_SS_BAND_PREF_BC18  \
  | SD_SS_BAND_PREF_BC19  \
  )

#define ALL_HDR_BANDS   ALL_CDMA_BANDS
#define ALL_GSM_BANDS   SD_SS_BAND_PREF_GSM_ANY
#define ALL_WCDMA_BANDS SD_SS_BAND_PREF_WCDMA_ANY
#define ALL_LTE_BANDS   SD_SS_BAND_PREF_LTE_ANY
#define ALL_TDS_BANDS   SD_SS_BAND_PREF_TDS_ANY

#endif /* _POLICYMAN_RF_H_ */
