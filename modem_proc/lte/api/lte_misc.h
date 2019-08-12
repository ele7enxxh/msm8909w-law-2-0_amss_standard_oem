/*!
  @file lte_misc.c
  

  @brief
  REQUIRED This file has all Miscellaneous routine used by lte modules 

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*=============================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_misc.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/31/11   kp      Added code to get UE category from API lte\common
10/31/11   kp      Initial Version.
=============================================================================*/

#ifndef LTE_MISC_H
#define LTE_MISC_H

/*=============================================================================

                   INCLUDE FILES

=============================================================================*/

#include <comdef.h>
#include <IxErrno.h>
#include <sys.h>
#include <lte_cphy_msg.h>
#include <msgr.h>

/*=============================================================================

                   EXTERNAL CONSTANT/MACRO DEFINITIONS

=============================================================================*/


  /*8x30 JTAG_ID_1 0x0728 :- MM LTE Cat2, HSPA+, DOrB, UMTS*/
  #define LTE_RRC_CAP_8x30_CAT2_JTAG_ID_1 0x0728
  /*8x30 JTAG_ID_2 0x0729 :- MM LTE Cat2, HSPA+, DOrB, UMTS, HDCP*/
  #define LTE_RRC_CAP_8x30_CAT2_JTAG_ID_2 0x0729
  /*8x30 JTAG_ID_3 0x072A :- 3GPP LTE Cat2, HSPA+, TD-SCDMA, UMTS*/
  #define LTE_RRC_CAP_8x30_CAT2_JTAG_ID_3 0x072A
  /*8x30 JTAG_ID_4 0x072B :- 3GPP LTE Cat2, HSPA+, TD-SCDMA, UMTS, HDCP*/
  #define LTE_RRC_CAP_8x30_CAT2_JTAG_ID_4 0x072B
  /*! @brief UE Category -2
  */
  #define LTE_UE_CATEGORY2   2


/*=============================================================================

                   EXTERNAL ENUMERATION TYPES

=============================================================================*/

/*=============================================================================

                   EXTERNAL STRUCTURE/UNION TYPES

=============================================================================*/



/*=============================================================================

                   DATA TYPES

=============================================================================*/

/*! @brief Enumeration of Source Layer
*/
typedef enum
{
  LTE_CLIENT_NONE,          /*!< 0 FOR INTERNAL USE ONLY */
  LTE_ML1_CLIENT,           /*!< 1 */
  LTE_L2_CLIENT,            /*!< 2 */
  LTE_RRC_CLIENT            /*!< 3 */
} lte_layer_client_id_e;

/*! @brief REQUIRED brief description of this structure typedef
  This type is used for returning ue_category
*/
typedef struct
{
  /*Jtag I.D used for debugging only*/
  uint32 debug_pnId;
  
  boolean use_cat_2;   /*!< UE category */
} lte_ue_category_info_s;

#define LTE_COMMON_EFS_REFRESH_LTE_CAT 0x01
#define LTE_COMMON_EFS_REFRESH_CA_DISABLE 0x02
#define LTE_COMMON_EFS_REFRESH_EMBMS_DPD 0x04


typedef struct
{
   /*! Tune-Aways are enabled for DSDS/SRLTE/SGLTE+G/SVLTE+G */
   boolean tuneaway_enabled;
   /*! Conflict checks needed for either TX_BACKOFF or BLANKING*/
   lte_ml1_coex_dsda_conflict_type_e conflict_type;
   /*! Single Tx is enabled for DSDA device mode*/
   boolean single_tx_enabled;
}lte_common_msim_crat_features_s;

typedef struct 
{
  msgr_hdr_s                          msg_hdr; /*!< Message router header */
  /*! Enabled Common LTE Features*/
  lte_common_msim_crat_features_s     common_features;
  /*! Number of Active Sims*/
  sys_modem_dual_standby_pref_e_type  standby_mode;
  /*! SS/DSDS/DSDA/TSTS */
  sys_modem_device_mode_e_type        device_mode;
  /*! SRLTE/SVLTE/SGLTE/Normal */
  lte_cphy_ue_mode_type_e             cphy_ue_mode;     
}lte_common_msim_crat_feature_req_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

  FUNCTION:  lte_get_ue_category

===========================================================================*/
/*!
  @brief
   this function will return ue_category
  
  @return
  ue_category - 2 or 3

*/
/*=========================================================================*/
uint8 lte_get_ue_category
(
  void
);
/*===========================================================================

  FUNCTION:  lte_get_current_time

===========================================================================*/
/*!
    @brief
    Get current time in ms since power up

    @details
    Get current time in ms since power up
    This API will be removed once all SW is migrated to use one of the two
    below APIs

    @return
    None
*/
/*=========================================================================*/
uint64 lte_get_current_time(void);

/*===========================================================================

  FUNCTION:  lte_get_current_time_since_power_up_ms

===========================================================================*/
/*!
    @brief
    Get current time in ms since power up

    @details
    Get current time in ms since power up

    @return
    None
*/
/*=========================================================================*/
/* Current time since power up in ms */
uint64 
lte_get_current_time_since_power_up_ms(void);

/*===========================================================================

  FUNCTION:  lte_get_current_time_since_power_up_secs

===========================================================================*/
/*!
    @brief
    Get current time in secs since power up

    @details
    Get current time in secs since power up

    @return
    None
*/
/*=========================================================================*/
/* Current time since power up in secs */
uint32 
lte_get_current_time_since_power_up_secs(void);

/*===========================================================================

  FUNCTION:  lte_set_current_time

===========================================================================*/
/*!
    @brief
    Set current time in ms since power up

    @details
    Set current time in ms since power up

    @return
    None
*/
/*=========================================================================*/
void lte_set_current_time(uint64 cur_time);

/*===========================================================================

  FUNCTION:  lte_is_ca_feature_enabled

===========================================================================*/
/*!
    @brief  
    if DALCHIPINFO_ID_MDM9225_1
      then CA is disabled
    else
      CA is Enabled  

    @return
      boolean  
*/
/*=========================================================================*/
extern boolean lte_is_ca_feature_enabled 
(
  void
);

/*===========================================================================

  FUNCTION:  lte_allow_efs_refresh

===========================================================================*/
/*!
    @brief  
    Allow UE Cat and CAP feature EFS files to be refreshed

    @return
    None
*/
/*=========================================================================*/
extern void lte_allow_efs_refresh(uint8 efs_refresh_bitmask);


/*===========================================================================

  FUNCTION:  lte_common_msim_crat_feature_enabled

===========================================================================*/
/*!
    @brief
    Returns struct containing enabled features for multisim/concurrent rat
    configured device

    @details
    Returns struct containing enabled features for multisim/concurrent rat
    configured device

    @return
    None
*/
/*=========================================================================*/
extern lte_common_msim_crat_features_s lte_common_msim_crat_feature_enabled(
      lte_cphy_ue_mode_type_e ue_mode, 
      sys_modem_device_mode_e_type device_mode, 
      sys_modem_dual_standby_pref_e_type standby_mode);

/*===========================================================================

  FUNCTION:  lte_is_dpd_feature_enabled

===========================================================================*/
/*!
    @brief
    this function will tell if DPD feature is enabled for eMBMS
    
    @return
    boolean stating if DPD is enabled or not   
*/
/*=========================================================================*/
extern boolean lte_is_dpd_feature_enabled
(
  void
);

#endif /* LTE_MISC_H */
