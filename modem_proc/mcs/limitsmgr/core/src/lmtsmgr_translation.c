
/*!
  @file
  lmtsmgr_translation.c

  @brief
  This file implements the Translation functions of LIMITS MGR module.

  @ingroup per_implementation
*/

/*=============================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/core/src/lmtsmgr_translation.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/06/15   rj      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "mcs_variation.h"
#include "comdef.h"
#include "stringl.h"
#include "cxm.h"
#include "trm.h"
#include "subs_prio.h"

#include "lmtsmgr_i.h"
#include "lmtsmgr_task.h"
#include "rfm_gsm.h"
#include "sar_i.h"
#include "ddr_coex_i.h"
#include "lmtsmgr_translation.h"

/*=============================================================================

                            TYPEDEFS

=============================================================================*/


/*=============================================================================

                                FUNCTIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  lmtsmgr_trans_rf_bands_to_sys_bands

=============================================================================*/
#if defined(FEATURE_MCS_STX_TO_CXM_CONVERT) || defined(FEATURE_LMTSMGR_SIMULATOR_SUPPORT)
uint32 lmtsmgr_trans_rf_bands_to_sys_bands 
(
  cxm_tech_type tech_id,
  uint32 tech_band
)
{
  uint32 in_sys_band;

  switch (tech_id)
  {
    case CXM_TECH_LTE:
      in_sys_band = tech_band + SYS_BAND_LTE_EUTRAN_BAND1;
      if ((in_sys_band >= SYS_BAND_LTE_EUTRAN_BAND1) &&
            (in_sys_band <= SYS_BAND_LTE_EUTRAN_BAND127))
      {
        return in_sys_band;
      }
      else
      {
        LMTSMGR_MSG_1(ERROR, "Lmtsmgr: LTE band not present in RF band %d", tech_band);
        return (uint32)SYS_BAND_CLASS_MAX;
      }
      break;

    case CXM_TECH_TDSCDMA:
      switch ((rfcom_tdscdma_band_type)tech_band)
      {
        case RFCOM_BAND_TDSCDMA_B34:
          return ((uint32)SYS_BAND_TDS_BANDA);
        case RFCOM_BAND_TDSCDMA_B39:
          return ((uint32)SYS_BAND_TDS_BANDF);
        case RFCOM_BAND_TDSCDMA_B40:
          return ((uint32)SYS_BAND_TDS_BANDE);
                  
        default:
          LMTSMGR_MSG_1(ERROR, "Lmtsmgr: TDSCDMA band not present in RF band %d", tech_band);
          return (uint32)SYS_BAND_CLASS_MAX;
      }
    break;

    case CXM_TECH_GSM1:
    case CXM_TECH_GSM2:
    case CXM_TECH_GSM3:
      switch ((rfcom_gsm_band_type)tech_band)
      {
        case RFCOM_BAND_GSM850:
          return ((uint32)SYS_BAND_GSM_850);
        case RFCOM_BAND_GSM900:
          return ((uint32)SYS_BAND_GSM_EGSM_900);
        case RFCOM_BAND_GSM1800:
          return ((uint32)SYS_BAND_GSM_DCS_1800);
        case RFCOM_BAND_GSM1900:
          return ((uint32)SYS_BAND_GSM_PCS_1900);
                  
        default:
          LMTSMGR_MSG_1(ERROR, "Lmtsmgr: GSM band not present in RF band %d", tech_band);
          return (uint32)SYS_BAND_CLASS_MAX;
      }
    break;

    case CXM_TECH_ONEX:
    case CXM_TECH_HDR:
      in_sys_band = tech_band + SYS_BAND_BC0;
      if ((in_sys_band >= ((SYS_BAND_BC0))) &&
           (in_sys_band <= SYS_BAND_BC19))
      {
        return (in_sys_band);
      }
      else
      {
        LMTSMGR_MSG_2(ERROR, "Lmtsmgr: tech: %d band not present in RF band %d", tech_id, tech_band);
        return (uint32)SYS_BAND_CLASS_MAX;
      }
      break;

    case CXM_TECH_WCDMA:
      if (tech_band >= (uint32)SYS_BAND_WCDMA_I_IMT_2000)
      {
        return tech_band;
      }
      switch ((rfcom_wcdma_band_type)tech_band)
      {
          case RFCOM_BAND_IMT:
              return (uint32)SYS_BAND_WCDMA_I_IMT_2000;
      
          case RFCOM_BAND_1900:
              return (uint32)SYS_BAND_WCDMA_II_PCS_1900;
      
          case RFCOM_BAND_BC3:
              return (uint32)SYS_BAND_WCDMA_III_1700;
      
          case RFCOM_BAND_BC4:
              return (uint32)SYS_BAND_WCDMA_IV_1700;
      
          case RFCOM_BAND_800:
              return (uint32)SYS_BAND_WCDMA_V_850;
      
          case RFCOM_BAND_BC8:
              return (uint32)SYS_BAND_WCDMA_VIII_900;
      
          case RFCOM_BAND_BC9:
              return (uint32)SYS_BAND_WCDMA_IX_1700;
      
          case RFCOM_BAND_BC11:
              return (uint32)SYS_BAND_WCDMA_XI_1500;
      
          case RFCOM_BAND_BC19:
              return (uint32)SYS_BAND_WCDMA_XIX_850;
      
        default:
              LMTSMGR_MSG_2(ERROR, "Lmtsmgr: tech: %d band not present in RF band %d", tech_id, tech_band);
              return (uint32)SYS_BAND_CLASS_MAX;
      }
      break;

    default:
      LMTSMGR_MSG_1(ERROR, "Lmtsmgr: Tech Invalid %d", tech_id);
      return (uint32)SYS_BAND_CLASS_MAX;
    break;
  }
}
#endif

/*=============================================================================

  FUNCTION:  lmtsmgr_trans_lmts_tech_to_xo_tech

=============================================================================*/
/*!
    @brief
    Get the XO tech ID corresponding to the limitsmgr tech ID
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_trans_lmts_tech_to_xo_tech
(
  cxm_tech_type   tech,
  tcxomgr_client_info_struct_type *xo_rgs_client
)
{
  tcxomgr_client_id_type  client;
  tcxomgr_as_id_enum_type as_id = 0;

  switch (tech)
  {
    case CXM_TECH_LTE:
      client = TCXOMGR_CLIENT_LTE;
      break;
    case CXM_TECH_TDSCDMA:
      client = TCXOMGR_CLIENT_TDSCDMA;
      break;
    case CXM_TECH_GSM1:
      client = TCXOMGR_CLIENT_GSM;
      break;
    case CXM_TECH_ONEX:
      client = TCXOMGR_CLIENT_CDMA_1X;
      break;
    case CXM_TECH_HDR:
      client = TCXOMGR_CLIENT_CDMA_HDR;
      break;
    case CXM_TECH_WCDMA:
      client = TCXOMGR_CLIENT_WCDMA;
      break;
    case CXM_TECH_GSM2:
      client = TCXOMGR_CLIENT_GSM;
      as_id = 2;
      break;
    case CXM_TECH_GSM3:
      client = TCXOMGR_CLIENT_GSM;
      as_id = 3;
      break;
    default:
       client = TCXOMGR_CLIENT_NONE;
       break;
  }

  xo_rgs_client->id = client;
  xo_rgs_client->as_id = as_id;
  
  return;
}

#ifdef FEATURE_LMTSMGR_SIMULATOR_SUPPORT
#error code not present
#endif /* FEATURE_LMTSMGR_SIMULATOR_SUPPORT */

/*=============================================================================

  FUNCTION:  lmtsmgr_trans_cxm_to_sys_id

=============================================================================*/
/*!
    @brief
    Converts CXM tech IDs to MMCP tech IDs
 
    @return
    MMCP SYS tech ID
*/
/*=============================================================================*/
sys_sys_mode_e_type lmtsmgr_trans_cxm_to_sys_id
(
  cxm_tech_type tech
)
{

  switch (tech)
  {
    case CXM_TECH_ONEX:
        return SYS_SYS_MODE_CDMA;

    case CXM_TECH_GSM1:
    case CXM_TECH_GSM2:
    case CXM_TECH_GSM3:
        return SYS_SYS_MODE_GSM;

    case CXM_TECH_LTE:
        return SYS_SYS_MODE_LTE;

    case CXM_TECH_TDSCDMA:
        return SYS_SYS_MODE_TDS;

    case CXM_TECH_HDR:
        return SYS_SYS_MODE_HDR;
    
    case CXM_TECH_WCDMA:
        return SYS_SYS_MODE_WCDMA;

    default:
        LMTSMGR_MSG_1(ERROR, "Tech id %d not valid", tech);
        return SYS_SYS_MODE_NONE;
  }

}

/*=============================================================================

  FUNCTION:  lmtsmgr_trans_trm_to_cxm_client

=============================================================================*/
/*!
    @brief
    This is a TRM to CXM client mapping 
 
    @return
    None
*/
/*===========================================================================*/
cxm_tech_type lmtsmgr_trans_trm_to_cxm_client 
(
  trm_client_enum_t client    /* Client's whose state update is sent */
)
{
  switch (client)
  {
    case TRM_LTE:
        return CXM_TECH_LTE;

    case TRM_TDSCDMA:
        return CXM_TECH_TDSCDMA;

    case TRM_1X:
        return CXM_TECH_ONEX;

    case TRM_HDR:
        return CXM_TECH_HDR;

    case TRM_GSM1:
        return CXM_TECH_GSM1;

    case TRM_UMTS:
    case TRM_WCDMA:
        return CXM_TECH_WCDMA;

    case TRM_GSM2:
        return CXM_TECH_GSM2;

    case TRM_GSM3:
        return CXM_TECH_GSM3;

    default:
        return CXM_TECH_DFLT_INVLD;
  }
}

#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
/*=============================================================================

  FUNCTION:  lmtsmgr_trans_cxm_to_trm_client

=============================================================================*/
/*!
    @brief
    Converts CXM tech IDs to TRM client IDs
 
    @return
    TRM client ID
*/
/*=============================================================================*/
trm_client_enum_t lmtsmgr_trans_cxm_to_trm_client
(
  cxm_tech_type tech    
)
{
  switch (tech)
  {
     case CXM_TECH_LTE:
       return TRM_LTE;

     case CXM_TECH_TDSCDMA:
       return TRM_TDSCDMA;

     case CXM_TECH_ONEX:
       return TRM_1X;

     case CXM_TECH_HDR:
       return TRM_HDR;

     case CXM_TECH_GSM1:
       return TRM_GSM1;

     case CXM_TECH_WCDMA:
       return TRM_WCDMA;

     case CXM_TECH_GSM2:
       return TRM_GSM2;

     case CXM_TECH_GSM3:
       return TRM_GSM3;

    default:
        return TRM_LAST_CLIENT;
  }
}
#endif /* Antenna Switch Div feature */

/*=============================================================================

  FUNCTION:  lmtsmgr_trans_sys_bands_to_rf_bands

=============================================================================*/

uint32 lmtsmgr_trans_sys_bands_to_rf_bands 
(
  uint32 tech_band,
  cxm_tech_type tech_id
)
{

  switch (tech_id)
  {
    case CXM_TECH_LTE:
      if ((tech_band >= SYS_BAND_LTE_EUTRAN_BAND1) &&
            (tech_band <= SYS_BAND_LTE_EUTRAN_BAND127))
      {
        return ((uint32)(tech_band-SYS_BAND_LTE_EUTRAN_BAND1));
      }
      else
      {
        LMTSMGR_MSG_1(ERROR, "LTE band %d not present in SYS_BAND", tech_band);
        return (uint32)RFCOM_BAND_LTE_INVALID;
      }
    break;

    case CXM_TECH_TDSCDMA:
      switch ((sys_band_class_e_type)tech_band)
      {
        case SYS_BAND_TDS_BANDA:
          return ((uint32)RFCOM_BAND_TDSCDMA_B34);
        case SYS_BAND_TDS_BANDF:
          return ((uint32)RFCOM_BAND_TDSCDMA_B39);
        case SYS_BAND_TDS_BANDE:
          return ((uint32)RFCOM_BAND_TDSCDMA_B40);
                  
        default:
          LMTSMGR_MSG_1(ERROR, "TDSCDMA band %d not present in SYS_BAND", tech_band);
          return (uint32)RFCOM_BAND_TDSCDMA_INVALID;
      }
    break;

    case CXM_TECH_GSM1:
    case CXM_TECH_GSM2:
    case CXM_TECH_GSM3:
      switch ((sys_band_class_e_type)tech_band)
      {
        case SYS_BAND_GSM_850:
          return ((uint32)RFCOM_BAND_GSM850);
        case SYS_BAND_GSM_EGSM_900:
          return ((uint32)RFCOM_BAND_GSM900);
        case SYS_BAND_GSM_DCS_1800:
          return ((uint32)RFCOM_BAND_GSM1800);
        case SYS_BAND_GSM_PCS_1900:
          return ((uint32)RFCOM_BAND_GSM1900);
                  
        default:
          LMTSMGR_MSG_1(ERROR, "GSM band %d not present in SYS_BAND", tech_band);
          return (uint32)RFCOM_NUM_GSM_BANDS;
      }
    break;

    case CXM_TECH_ONEX:
    case CXM_TECH_HDR:
      if ((tech_band >= ((SYS_BAND_BC0))) &&
           (tech_band <= SYS_BAND_BC19))
      {
        return ((uint32)(tech_band-SYS_BAND_BC0));
      }
      else
      {
        LMTSMGR_MSG_2(ERROR, "Tech %d band %d not present in SYS_BAND", 
                      tech_id, tech_band);

        return (uint32)RFCOM_NUM_1X_BANDS;
      }
    break;

    case CXM_TECH_WCDMA:
      switch ((sys_band_class_e_type)tech_band)
      {
        case SYS_BAND_WCDMA_I_IMT_2000:
          return ((uint32)RFCOM_BAND_IMT);
        case SYS_BAND_WCDMA_II_PCS_1900:
          return ((uint32)RFCOM_BAND_1900);
        case SYS_BAND_WCDMA_III_1700:
          return ((uint32)RFCOM_BAND_BC3);
        case SYS_BAND_WCDMA_IV_1700:
          return ((uint32)RFCOM_BAND_BC4);
        case SYS_BAND_WCDMA_V_850:
          return ((uint32)RFCOM_BAND_800);
        case SYS_BAND_WCDMA_VIII_900:
          return ((uint32)RFCOM_BAND_BC8);
        case SYS_BAND_WCDMA_IX_1700:
          return ((uint32)RFCOM_BAND_BC9);
        case SYS_BAND_WCDMA_XI_1500:
          return ((uint32)RFCOM_BAND_BC11);
        case SYS_BAND_WCDMA_XIX_850:
          return ((uint32)RFCOM_BAND_BC19);
                  
        default:
          LMTSMGR_MSG_2(ERROR, "Tech %d band %d not present in SYS_BAND", 
                        tech_id, tech_band);
          return (uint32)RFCOM_BAND_INVALID;
      }
    break;

    default:
      LMTSMGR_MSG_1(ERROR, "Tech %d Invalid", tech_id);
      return (uint32)RFCOM_BAND_LTE_INVALID;
    break;
  }
}


/*=============================================================================

  FUNCTION:  lmtsmgr_trans_cxm_tech_to_rf_mode

=============================================================================*/
/*!
    @brief
    Maps tech type to RF mode
 
    @return
    None
*/
/*=============================================================================*/
rfm_mode_enum_type lmtsmgr_trans_cxm_tech_to_rf_mode(cxm_tech_type tech_id)
{
  switch (tech_id) 
  {
    case CXM_TECH_GSM1:
    case CXM_TECH_GSM2:
    case CXM_TECH_GSM3:
      return RFCOM_GSM_MODE;

    case CXM_TECH_LTE:
      return RFM_LTE_MODE;

    case CXM_TECH_ONEX:
      return RFM_1X_MODE;

    case CXM_TECH_HDR:
      return RFM_1XEVDO_MODE;

    case CXM_TECH_WCDMA:
      return RFM_IMT_MODE;

    case CXM_TECH_TDSCDMA:
      return RFM_TDSCDMA_MODE;

    default:
      LMTSMGR_MSG_1(ERROR, "Unsupported tech %d", tech_id);
      return RFM_INVALID_MODE;

  }
}

/*=============================================================================

  FUNCTION:  lmtsmgr_trans_sys_id_to_cxm_client

=============================================================================*/
/*!
    @brief
    Converts MMCP SYS Tech IDs to CXM client IDs
 
    @return
    CXM client ID
*/
/*=============================================================================*/
cxm_tech_type lmtsmgr_trans_sys_id_to_cxm_client
(
  sys_sys_mode_e_type  mode,
  sys_modem_as_id_e_type asid
)
{
  cxm_tech_type tech = CXM_TECH_MAX;
  
  switch(mode)
  {
    case SYS_SYS_MODE_GSM:
       tech = lmtsmgr.gsm_id_list[asid];
       break;
    
    case SYS_SYS_MODE_WCDMA:
       tech = CXM_TECH_WCDMA;
       break;
    
    case SYS_SYS_MODE_HDR:
      tech= CXM_TECH_HDR;
      break;
    
    case SYS_SYS_MODE_CDMA:
      tech = CXM_TECH_ONEX;
      break;
    
    case SYS_SYS_MODE_TDS:
      tech = CXM_TECH_TDSCDMA;
      break;
    
    case SYS_SYS_MODE_LTE:
      tech = CXM_TECH_LTE;
      break;
      
    default:
      break;  
  }
  
  return tech;
}


