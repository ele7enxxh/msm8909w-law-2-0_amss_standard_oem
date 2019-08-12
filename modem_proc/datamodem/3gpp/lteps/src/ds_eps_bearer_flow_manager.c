/*!
  @file
  ds_eps_bearer_flow_manager.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2009-2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/lteps/src/ds_eps_bearer_flow_manager.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/11   ttv     Added changes needed for SESSION_PARAMS IOCTL and NSAPI api.
04/03/10   vk      Added support for MSG2.0 based logging
02/10/10   vd      Replaced lte_nas_esm_message_description.h with cm_gw.h 
02/04/10   vd      Changed function naming format.
01/23/10   vk      Added helper macros for virtual function dispatch
11/07/09   vd      Initial version.
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include <stringl/stringl.h>

#ifdef FEATURE_DATA_LTE
#include "dstask.h"
#include "ds3gmgr.h"
#include "ds_eps_bearer_flow_manager.h"
#include "ds_3gpp_bearer_flow_manager.h"
#include "ds_eps_bearer_context.h"
#include "ds3gmshif.h"
#include "dsumtspdpreg.h"
#include "ds_flow_control.h"
#include "ran.h"
#include "cm.h"
#include "cm_gw.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/*---------------------------------------------------------------------------
  EPS kbps to bps multiplier, we need this to be uint64 as the product of this
  and some MACRO values needs to be uint64 type
---------------------------------------------------------------------------*/
uint32 ds_eps_kbps_to_bps_multiplier_32bit = 1000;
uint64 ds_eps_kbps_to_bps_multiplier_64bit = 1000;

/*---------------------------------------------------------------------------
  EPS bearer Flow manager table exposed to other files through header
---------------------------------------------------------------------------*/
ds_eps_bearer_flow_manager_s ds_eps_bflow_manager_tbl[DS_3GPP_MAX_BEARER_CONTEXT] = {{NULL}};


/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_MANAGER_INIT

DESCRIPTION
  Register EPS specific functions

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_bearer_flow_manager_init( void )
{
  ds_3gpp_bearer_flow_manager_ftbl_s  *bearer_flow_manager_ftbl_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    Initialize EPS bearer flow manager Function table
  -------------------------------------------------------------------------*/
  bearer_flow_manager_ftbl_p = 
                    &(ds_bearer_flow_manager_ftbl[DS_3GPPI_SYS_MODE_LTE]);
  memset(
         (void*)bearer_flow_manager_ftbl_p,
         0x0,
         (sizeof(ds_3gpp_bearer_flow_manager_ftbl_s))
        );

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_LTE, nwDataRateToAppDataRate,
                ds_eps_bearer_flow_manager_convert_nw_data_rate_to_app);

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_LTE, appDataRateToNwDataRate,
                    ds_eps_bearer_flow_manager_convert_app_data_rate_to_nw);

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_LTE, flowValidation,
                                 ds_eps_bearer_flow_manager_flow_validation);

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_LTE, allocate_eps_bearer_flow_manager,
            ds_eps_bearer_flow_manager_alloc_eps_bearer_flow_manager);

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_LTE, free_eps_bearer_flow_manager,
            ds_eps_bearer_flow_manager_free_eps_bearer_flow_manager);
                           
} /*ds_eps_bearer_flow_manager_init */

/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_MANAGER_CONVERT_NW_BIT_RATE

DESCRIPTION
  Converts the NW passed bit rate to uint64.

PARAMETERS
  input         -  network octet to be converted

DEPENDENCIES
  None.

RETURN VALUE
uint64          - return converted bit rate
  
SIDE EFFECTS
  None.

===========================================================================*/
uint64 ds_eps_bearer_flow_manager_convert_nw_bit_rate
(
  byte input
)
{
  uint64 converted_rate = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(input == 0)
  {
    return converted_rate;
  }
  if(input <= DS_EPS_1KB_BITRATE_END_RANGE)
  {
    converted_rate = DS_EPS_1KB_BITRATE_MINIMUM_VALUE+
		       (input - DS_EPS_1KB_BITRATE_START_RANGE);   
  }
  else if(input <= DS_EPS_8KB_BITRATE_END_RANGE)
  {
    converted_rate = DS_EPS_8KB_BITRATE_MINIMUM_VALUE+
		(input - DS_EPS_8KB_BITRATE_START_RANGE)* DS_EPS_8KB_BITRATE_MULTIPLE;   
  }
  else if(input <= DS_EPS_64KB_BITRATE_END_RANGE)
  {
    converted_rate = DS_EPS_64KB_BITRATE_MINIMUM_VALUE+
		(input - DS_EPS_64KB_BITRATE_START_RANGE)* DS_EPS_64KB_BITRATE_MULTIPLE;   
  }
  else
  {
    converted_rate = 0;
  }
  // Convert NW rate (kbps) to App rate (bps)
  converted_rate *= ds_eps_kbps_to_bps_multiplier_64bit;
  DS_LTE_MSG1_HIGH("converted rate returned is 0x%x",converted_rate);
  return converted_rate;
} /* ds_eps_bearer_flow_manager_convert_nw_bit_rate */

/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_MANAGER_CONVERT_EXT_NW_BIT_RATE

DESCRIPTION
  Converts the NW passed extended bit rate to uint32.

PARAMETERS
  input         -  network octet to be converted

DEPENDENCIES
  None.

RETURN VALUE
uint32          - return converted bit rate
  
SIDE EFFECTS
  None.

===========================================================================*/
uint64 ds_eps_bearer_flow_manager_convert_ext_nw_bit_rate
(
  byte input
)
{
  uint64 converted_rate = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(input == 0)
  {
    DS_LTE_MSG0_HIGH("Zero passed in extended bit rate");
	return converted_rate;
  }
  if(input <= DS_EPS_100KB_BITRATE_END_RANGE)
  {
    converted_rate = DS_EPS_100KB_BITRATE_MINIMUM_VALUE+
		(input - DS_EPS_100KB_BITRATE_START_RANGE)*DS_EPS_100KB_BITRATE_MULTIPLE;   
  }
  else if(input <= DS_EPS_1MB_BITRATE_END_RANGE)
  {
    converted_rate = DS_EPS_1MB_BITRATE_MINIMUM_VALUE+
		(input - DS_EPS_1MB_BITRATE_START_RANGE)*DS_EPS_1MB_BITRATE_MULTIPLE;   
  }
  else if(input <= DS_EPS_2MB_BITRATE_END_RANGE)
  {
    converted_rate = DS_EPS_2MB_BITRATE_MINIMUM_VALUE+
		(input - DS_EPS_2MB_BITRATE_START_RANGE)* DS_EPS_2MB_BITRATE_MULTIPLE;   
  }
  else
  {
    DS_LTE_MSG0_LOW("Invalid input passed to the extended bit rate:setting to "
                      "Max bit rate");
   	 converted_rate = DS_EPS_EXT_BITRATE_MAXIMUM_VALUE;
  }

  // Convert NW rate (kbps) to App rate (bps)
  converted_rate *= ds_eps_kbps_to_bps_multiplier_64bit;
  DS_LTE_MSG1_HIGH("converted extended rate returned is 0x%x",converted_rate);
  return converted_rate;
} /* ds_eps_bearer_flow_manager_convert_ext_nw_bit_rate */

/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_MANAGER_CONVERT_EXT2_NW_BIT_RATE

DESCRIPTION
  Converts the NW passed extended bit rate to uint32.

PARAMETERS
  input         -  network octet to be converted

DEPENDENCIES
  None.

RETURN VALUE
uint32          - return converted bit rate
  
SIDE EFFECTS
  None.

===========================================================================*/
uint64 ds_eps_bearer_flow_manager_convert_ext2_nw_bit_rate
(
  byte input
)
{
  uint64 converted_rate = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(input == 0)
  {
    DS_LTE_MSG0_HIGH("Zero passed in extended-2 bit rate");
	return converted_rate;
  }
  if(input <= DS_EPS_4MB_BITRATE_END_RANGE)
  {
    converted_rate = DS_EPS_4MB_BITRATE_MINIMUM_VALUE+
		(input - DS_EPS_4MB_BITRATE_START_RANGE)*DS_EPS_4MB_BITRATE_MULTIPLE;   
  }
  else if(input <= DS_EPS_10MB_BITRATE_END_RANGE)
  {
    converted_rate = DS_EPS_10MB_BITRATE_MINIMUM_VALUE+
		(input - DS_EPS_10MB_BITRATE_START_RANGE)*DS_EPS_10MB_BITRATE_MULTIPLE;   
  }
  else if(input <= DS_EPS_100MB_BITRATE_END_RANGE)
  {
    converted_rate = DS_EPS_100MB_BITRATE_MINIMUM_VALUE+
		(input - DS_EPS_100MB_BITRATE_START_RANGE)* DS_EPS_100MB_BITRATE_MULTIPLE;   
  }
  else
  {
    DS_LTE_MSG0_LOW("Invalid input passed to the extended-2 bit rate:setting to "
                      "Max bit rate");
   	 converted_rate = DS_EPS_APP_BITRATE_MAXIMUM_VALUE;
  }

  // Convert NW rate (kbps) to App rate (bps)
  converted_rate *= ds_eps_kbps_to_bps_multiplier_64bit;
  DS_LTE_MSG1_HIGH("converted extended-2 rate returned is 0x%x",converted_rate);
  return converted_rate;
} /* ds_eps_bearer_flow_manager_convert_ext2_nw_bit_rate */


/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_MANAGER_CONVERT_NW_FLOW_TO_APP_FLOW

DESCRIPTION
  Converts the EPS passed Flow spec to App flow spec.

PARAMETERS
  nw_flow         -  EPS flow to be converted
  app_flow        -  App flow to store the converted info.

DEPENDENCIES
  None.

RETURN VALUE
boolean          - returns if conversion is successfull
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_bearer_flow_manager_convert_nw_flow_to_app_flow
(
  sdf_qos_T              *nw_flow,
  qos_spec_type           *app_flow,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
)
{
  boolean result = TRUE;
  /*Nothing was filled in; we will still return success*/
  ip_flow_type *app_rx_flow = NULL;
  ip_flow_type *app_tx_flow = NULL;
  uint64 rx_mbr = 0;
  uint64 rx_gbr = 0;
  uint64 tx_mbr = 0;
  uint64 tx_gbr = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DS_LTE_MSG0_LOW("EPS:ConvertNwFlowToAppFlow");

  
  if(cause_code_p == NULL)
  {
    return FALSE;
  }
  else
  {
    *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
  }

  if(nw_flow == NULL || app_flow == NULL)
  {
    DS_LTE_MSG0_ERROR("Input params are NULL");
    *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    return FALSE;
  } 
  
  app_rx_flow = &(app_flow->rx.flow_template.req_flow);
  app_tx_flow = &(app_flow->tx.flow_template.req_flow);

  if((nw_flow->length == DS_EPS_QOS_QCI_LEN_FOR_DATA)
    || (nw_flow->length == DS_EPS_QOS_QCI_LEN_FOR_EXT_DATA)
    || (nw_flow->length == DS_EPS_QOS_QCI_LEN_FOR_EXT2_DATA))
  {
    DS_LTE_MSG0_LOW("Converting data rates");

    app_rx_flow->data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_TYPE;
    rx_mbr = ds_eps_bearer_flow_manager_convert_nw_bit_rate(nw_flow->max_bit_rate_dl);
    rx_gbr = ds_eps_bearer_flow_manager_convert_nw_bit_rate(nw_flow->gbr_dl);
    app_rx_flow->lte_params.lte_qci =(lte_qci_enum_type)nw_flow->qci;
    app_rx_flow->field_mask |=(int)(IPFLOW_MASK_DATA_RATE);
    app_rx_flow->field_mask |=(int)(IPFLOW_MASK_LTE_QCI); 


    app_tx_flow->data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_TYPE;
    tx_mbr = ds_eps_bearer_flow_manager_convert_nw_bit_rate(nw_flow->max_bit_rate_ul);
    tx_gbr = ds_eps_bearer_flow_manager_convert_nw_bit_rate(nw_flow->gbr_ul);
    
    app_tx_flow->lte_params.lte_qci = (lte_qci_enum_type)nw_flow->qci;
    app_tx_flow->field_mask |=(int)(IPFLOW_MASK_DATA_RATE);
    app_tx_flow->field_mask |=(int)(IPFLOW_MASK_LTE_QCI);
    
  } /*nw_flow->length == 11||7 */

  if((nw_flow->length == DS_EPS_QOS_QCI_LEN_FOR_EXT_DATA)
     || (nw_flow->length == DS_EPS_QOS_QCI_LEN_FOR_EXT2_DATA))
  {
    DS_LTE_MSG0_LOW("Converting data rates and ext data rates");    
    if((nw_flow->max_bit_rate_dl_ex != 0) && 
       (rx_mbr == DS_EPS_MAX_1_OCTET_BITRATE*ds_eps_kbps_to_bps_multiplier_64bit))
    {
      rx_mbr = ds_eps_bearer_flow_manager_convert_ext_nw_bit_rate(
                                       nw_flow->max_bit_rate_dl_ex);
    }

    if((nw_flow->gbr_dl_ex != 0) &&
       (rx_gbr == DS_EPS_MAX_1_OCTET_BITRATE*ds_eps_kbps_to_bps_multiplier_64bit))
      {
        rx_gbr = ds_eps_bearer_flow_manager_convert_ext_nw_bit_rate(
                                              nw_flow->gbr_dl_ex);
    }

    if((nw_flow->max_bit_rate_ul_ex != 0) &&
       (tx_mbr == DS_EPS_MAX_1_OCTET_BITRATE*ds_eps_kbps_to_bps_multiplier_64bit))
      {
        tx_mbr = ds_eps_bearer_flow_manager_convert_ext_nw_bit_rate(
                                         nw_flow->max_bit_rate_ul_ex);
    }

    if((nw_flow->gbr_ul_ex != 0) &&
       (tx_gbr == DS_EPS_MAX_1_OCTET_BITRATE*ds_eps_kbps_to_bps_multiplier_64bit))
    {
      tx_gbr = ds_eps_bearer_flow_manager_convert_ext_nw_bit_rate(
                                              nw_flow->gbr_ul_ex);
    }
  }/* nw_flow->length == 13|9 */

  if(nw_flow->length == DS_EPS_QOS_QCI_LEN_FOR_EXT2_DATA)
  {
    DS_LTE_MSG0_LOW("Converting data rates and ext2 data rates");    
    if((nw_flow->max_bit_rate_dl_ex2 != 0) && 
       (rx_mbr == DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*ds_eps_kbps_to_bps_multiplier_64bit))
    {
      rx_mbr = ds_eps_bearer_flow_manager_convert_ext2_nw_bit_rate(
                                       nw_flow->max_bit_rate_dl_ex2);
    }

    if((nw_flow->gbr_dl_ex2 != 0) &&
       (rx_gbr == DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*ds_eps_kbps_to_bps_multiplier_64bit))
    {
        rx_gbr = ds_eps_bearer_flow_manager_convert_ext2_nw_bit_rate(
                                              nw_flow->gbr_dl_ex2);
    }

    if((nw_flow->max_bit_rate_ul_ex2 != 0) &&
       (tx_mbr == DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*ds_eps_kbps_to_bps_multiplier_64bit))
    {
        tx_mbr = ds_eps_bearer_flow_manager_convert_ext2_nw_bit_rate(
                                         nw_flow->max_bit_rate_ul_ex2);
    }

    if((nw_flow->gbr_ul_ex2 != 0) &&
       (tx_gbr == DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*ds_eps_kbps_to_bps_multiplier_64bit))
    {
      tx_gbr = ds_eps_bearer_flow_manager_convert_ext2_nw_bit_rate(
                                              nw_flow->gbr_ul_ex2);
    }
    result = TRUE;
  }/* nw_flow->length == 13 */

  /* min and max dl rates are zero */  
  if((tx_mbr == 0) && 
     (rx_mbr == 0))
  {
    DS_LTE_MSG0_ERROR("MAX UL and MAX DL bit rates are zero");
    *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    return FALSE;
  }
  /* Return failure if GBR is great than MBR*/
  if(((lte_qci_enum_type)nw_flow->qci >=LTE_QCI_1) && 
     ((lte_qci_enum_type)nw_flow->qci <=LTE_QCI_4)&&
     ((tx_gbr > tx_mbr)||
      (rx_gbr > rx_mbr)))
  {
    DS_LTE_MSG1_ERROR("GBR is higher than MBR for QCI:%d bearer", nw_flow->qci );
    *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    return FALSE;
  }
  
  result = TRUE;

  /*
  If 3gpp release 11 is supported on the UE send 64bit values to PS. PS will send both 32bit and 64bit 
  format to app. If release 11 is not supported then if the value is more than 256mbps (pre relase 11
  maximum). 3gpp will only send 256mbps to PS. Otherwise we would send whatever we get from network. 
  Either way, it would be in 32bit variables.
  */
  if(ds_3gppi_is_rel_11_supported()) 
  {
     app_rx_flow->data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_EX_TYPE;
     app_rx_flow->data_rate.format.min_max_ex.max_rate = rx_mbr;
     app_rx_flow->data_rate.format.min_max_ex.guaranteed_rate = rx_gbr;

     app_tx_flow->data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_EX_TYPE;
     app_tx_flow->data_rate.format.min_max_ex.max_rate = tx_mbr;
     app_tx_flow->data_rate.format.min_max_ex.guaranteed_rate = tx_gbr;
  }
  else
  {
     app_rx_flow->data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_TYPE;
     app_tx_flow->data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_TYPE;

     if (rx_mbr > DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*ds_eps_kbps_to_bps_multiplier_64bit) 
     {
        app_rx_flow->data_rate.format.min_max.max_rate = DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*
                   ds_eps_kbps_to_bps_multiplier_32bit;
     }
     else
     {
        app_rx_flow->data_rate.format.min_max.max_rate = (uint32)rx_mbr;
     }

     if (rx_gbr > DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*ds_eps_kbps_to_bps_multiplier_64bit) 
     {
        app_rx_flow->data_rate.format.min_max.guaranteed_rate = DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*
                    ds_eps_kbps_to_bps_multiplier_32bit;
     }
     else
     {
        app_rx_flow->data_rate.format.min_max.guaranteed_rate = (uint32)rx_gbr;
     }

     
     if (tx_mbr > DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*ds_eps_kbps_to_bps_multiplier_64bit) 
     {
        app_tx_flow->data_rate.format.min_max.max_rate = DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*
                    ds_eps_kbps_to_bps_multiplier_32bit;
     }
     else
     {
        app_tx_flow->data_rate.format.min_max.max_rate = (uint32)tx_mbr;
     }

     if (tx_gbr > DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*ds_eps_kbps_to_bps_multiplier_64bit) 
     {
        app_tx_flow->data_rate.format.min_max.guaranteed_rate = DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*
                     ds_eps_kbps_to_bps_multiplier_32bit;
     }
     else
     {
        app_tx_flow->data_rate.format.min_max.guaranteed_rate = (uint32)tx_gbr;
     }

  }

  return result; 
} /* ds_eps_bearer_flow_manager_convert_nw_flow_to_app_flow */


/*===========================================================================
FUNCTION  DS_EPS_BEARER_FLOW_MANAGER_CONVERT_NW_DATA_RATE_TO_APP
DESCRIPTION
  Converts the EPS NW passed data rate to App data rate.

PARAMETERS
nw_qos          -  EPS network qos to be converted
app_qos         -  Application qos structure
DEPENDENCIES
  None.

RETURN VALUE
boolean         - return DS3G_SUCCESS or DS3G_FAILURE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_bearer_flow_manager_convert_nw_data_rate_to_app
(
  ds_bearer_flow_manager_s *bflow_manager_p,
  void           *nw_qos,
  qos_spec_type  *app_qos,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
)
{
  cm_call_mode_info_s_type  *input = NULL;
  cm_lte_call_info_s_type   *lte_info_ptr = NULL;
  sdf_qos_T                 *lte_qos_ptr = NULL;
  ds_eps_bearer_flow_manager_s *eps_bflow_manager_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    get the LTE specific info.  
  -------------------------------------------------------------------------*/
  input = (cm_call_mode_info_s_type *)(nw_qos); 
  lte_info_ptr = (cm_lte_call_info_s_type *)&(input->info.lte_call);
  lte_qos_ptr  = &(lte_info_ptr->sdf_qos);

  DS_LTE_MSG0_LOW("in ds_eps_bearer_flow_manager_convert_nw_data_rate_to_app");
  if(cause_code_p == NULL)
  {
    return DS3G_FAILURE;
  }
  else
  {
    *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
  }

  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    return FALSE;
  }

  if (ds_3gpp_bearer_flow_manager_convert_nw_sdf_qos_to_app(bflow_manager_p,
                                                        (void*)lte_qos_ptr,
                                                        app_qos,
                                                        cause_code_p)== FALSE)
  {
    DS_LTE_MSG0_ERROR("Data Rate conversion Failure");
    return FALSE;
  }
  /*----------------------------------------------------------------------
     Store the Data rate in Mode specific context
  -----------------------------------------------------------------------*/
  eps_bflow_manager_p = (ds_eps_bearer_flow_manager_s *)
    DS_BEARER_FLOW_MGR_VOBJ(bflow_manager_p
                            ,SYS_SYS_MODE_LTE);

  if(!ds_eps_bearer_flow_manager_validate_eps_bearer_flow_manager
     (eps_bflow_manager_p))
  {
    DS_LTE_MSG0_ERROR("eps_bearer_context_p is NULL");
    return DS3G_FAILURE;
  }
  /* Mode specific */
  memscpy((void *)&(eps_bflow_manager_p->eps_bearer_flow_manager_dyn_p->sdf_qos),
           sizeof(sdf_qos_T),
          (void *)&(lte_info_ptr->sdf_qos), sizeof(sdf_qos_T));

  //If the QCI was converted internally; store the new QCI value.
  if (app_qos->rx.flow_template.req_flow.field_mask & IPFLOW_MASK_LTE_QCI)      
  {
    eps_bflow_manager_p->eps_bearer_flow_manager_dyn_p->sdf_qos.qci = 
	app_qos->rx.flow_template.req_flow.lte_params.lte_qci;
  }
  else if(app_qos->tx.flow_template.req_flow.field_mask & IPFLOW_MASK_LTE_QCI)      
  {
    eps_bflow_manager_p->eps_bearer_flow_manager_dyn_p->sdf_qos.qci = 
	app_qos->tx.flow_template.req_flow.lte_params.lte_qci;
  } 
  
  /* Mode independent*/
  memscpy((void *)&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->rx_flow),
           sizeof(ip_flow_type),
           (void *)&(app_qos->rx.flow_template.req_flow),sizeof(ip_flow_type));         

  memscpy((void *)&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->tx_flow),
  	   sizeof(ip_flow_type),
           (void *)&(app_qos->tx.flow_template.req_flow),sizeof(ip_flow_type)); 

  
  /* store umts qos set */
  if(lte_info_ptr->neg_qos.valid)
  {
    memscpy(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->neg_qos), 
            sizeof(qos_T),
             &(lte_info_ptr->neg_qos), sizeof(qos_T));
  }

  return DS3G_SUCCESS;
}/* ds_eps_bearer_flow_manager_convert_nw_data_rate_to_app*/

/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_MANAGER_CONVERT_APP_BIT_RATE_TO_NW_EXT

DESCRIPTION
  Converts the NW passed bit rate to uint32.

PARAMETERS
  input         -  network octet to be converted

DEPENDENCIES
  None.

RETURN VALUE
uint32          - return converted bit rate
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_bearer_flow_manager_convert_app_bit_rate_to_nw_ext
(
  uint64 app_rate,
  byte   *nw_rate,
  byte   *nw_rate_ex,
  byte   *nw_rate_ex2
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  // initialize the extend and extended-2 byte to zero
  *nw_rate_ex = 0;
  *nw_rate_ex2 = 0;
  // convert app rate to kbps
  app_rate /= ds_eps_kbps_to_bps_multiplier_64bit;

  if(app_rate == 0)
  {
    DS_LTE_MSG0_ERROR("zero rate passed");
    *nw_rate = DS_EPS_0KB_BITRATE_VALUE;
    return;
  }
  if(app_rate < DS_EPS_8KB_BITRATE_MINIMUM_VALUE)
  {
    *nw_rate = (byte) (app_rate);
  }
  else if(app_rate < DS_EPS_64KB_BITRATE_MINIMUM_VALUE)
  {
    *nw_rate = (byte) ((app_rate - DS_EPS_8KB_BITRATE_MINIMUM_VALUE)/DS_EPS_8KB_BITRATE_MULTIPLE
                        + DS_EPS_8KB_BITRATE_START_RANGE);
  }
  else if(app_rate <= DS_EPS_MAX_1_OCTET_BITRATE)
  {
    *nw_rate = (byte) ((app_rate - DS_EPS_64KB_BITRATE_MINIMUM_VALUE)/DS_EPS_64KB_BITRATE_MULTIPLE
                        + DS_EPS_64KB_BITRATE_START_RANGE);
  }
  else if (app_rate < DS_EPS_1MB_BITRATE_MINIMUM_VALUE)
  {
    *nw_rate = DS_EPS_MAX_1_OCTET_BIT_VALUE;
    *nw_rate_ex = (byte) ((app_rate - DS_EPS_100KB_BITRATE_BASE_RATE)/DS_EPS_100KB_BITRATE_MULTIPLE);
  }
  else if (app_rate < DS_EPS_2MB_BITRATE_MINIMUM_VALUE)
  {
    *nw_rate = DS_EPS_MAX_1_OCTET_BIT_VALUE;
    *nw_rate_ex = (byte) ((app_rate - DS_EPS_1MB_BITRATE_MINIMUM_VALUE)/DS_EPS_1MB_BITRATE_MULTIPLE
                           + DS_EPS_1MB_BITRATE_START_RANGE);
  }
  else if (app_rate <= DS_EPS_EXT_BITRATE_MAXIMUM_VALUE)
  {
    *nw_rate = DS_EPS_MAX_1_OCTET_BIT_VALUE;
    *nw_rate_ex = (byte) ((app_rate - DS_EPS_2MB_BITRATE_MINIMUM_VALUE)/DS_EPS_2MB_BITRATE_MULTIPLE 
                           + DS_EPS_2MB_BITRATE_START_RANGE);
  }
  else if (app_rate < DS_EPS_10MB_BITRATE_MINIMUM_VALUE)
  {
    *nw_rate = DS_EPS_MAX_1_OCTET_BIT_VALUE;
    *nw_rate_ex = DS_EPS_EXT_MAX_BIT_VALUE;
    *nw_rate_ex2 = (byte) ((app_rate - DS_EPS_4MB_BITRATE_MINIMUM_VALUE)/DS_EPS_4MB_BITRATE_MULTIPLE 
                           + DS_EPS_4MB_BITRATE_START_RANGE);
  }
  else if (app_rate < DS_EPS_100MB_BITRATE_MINIMUM_VALUE)
  {
    *nw_rate = DS_EPS_MAX_1_OCTET_BIT_VALUE;
    *nw_rate_ex = DS_EPS_EXT_MAX_BIT_VALUE;
    *nw_rate_ex2 = (byte) ((app_rate - DS_EPS_10MB_BITRATE_MINIMUM_VALUE)/DS_EPS_10MB_BITRATE_MULTIPLE 
                           + DS_EPS_10MB_BITRATE_START_RANGE);
  }
  else if (app_rate <= DS_EPS_EXT2_BITRATE_MAXIMUM_VALUE)
  {
    *nw_rate = DS_EPS_MAX_1_OCTET_BIT_VALUE;
    *nw_rate_ex = DS_EPS_EXT_MAX_BIT_VALUE;
    *nw_rate_ex2 = (byte) ((app_rate - DS_EPS_100MB_BITRATE_MINIMUM_VALUE)/DS_EPS_100MB_BITRATE_MULTIPLE 
                           + DS_EPS_100MB_BITRATE_START_RANGE);
  }
  else
  {
    // treat all higher rates as 10Gbps
    *nw_rate = DS_EPS_MAX_1_OCTET_BIT_VALUE;
    *nw_rate_ex = DS_EPS_EXT_MAX_BIT_VALUE;
    *nw_rate_ex2 = DS_EPS_EXT2_MAX_BIT_VALUE;
  }
  DS_LTE_MSG3_HIGH("Rate returned to NW is 0x%x, ext_rate:0x%x, "
                   "ext2_rate:0x%x",*nw_rate,*nw_rate_ex,*nw_rate_ex2);
} /* ds_eps_bearer_flow_manager_convert_app_bit_rate_to_nw */



/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_MANAGER_CONVERT_APP_BIT_RATE_TO_NW

DESCRIPTION
  Converts the NW passed bit rate to uint32.

PARAMETERS
  input         -  network octet to be converted

DEPENDENCIES
  None.

RETURN VALUE
uint32          - return converted bit rate
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_bearer_flow_manager_convert_app_bit_rate_to_nw
(
  uint32 app_rate,
  byte   *nw_rate,
  byte   *nw_rate_ex
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  // initialize the extend byte to zero
  *nw_rate_ex = 0;
  // convert app rate to kbps
  app_rate /= ds_eps_kbps_to_bps_multiplier_32bit;

  if(app_rate == 0)
  {
    DS_LTE_MSG0_ERROR("zero rate passed");
	  *nw_rate = DS_EPS_0KB_BITRATE_VALUE;
    return;
  }
  if(app_rate < DS_EPS_8KB_BITRATE_MINIMUM_VALUE)
  {
    *nw_rate = (byte) (app_rate);
  }
  else if(app_rate < DS_EPS_64KB_BITRATE_MINIMUM_VALUE)
  {
    *nw_rate = (byte) ((app_rate - DS_EPS_8KB_BITRATE_MINIMUM_VALUE)/DS_EPS_8KB_BITRATE_MULTIPLE
                        + DS_EPS_8KB_BITRATE_START_RANGE);
  }
  else if(app_rate <= DS_EPS_MAX_1_OCTET_BITRATE)
  {
    *nw_rate = (byte) ((app_rate - DS_EPS_64KB_BITRATE_MINIMUM_VALUE)/DS_EPS_64KB_BITRATE_MULTIPLE
                        + DS_EPS_64KB_BITRATE_START_RANGE);
  }
  else if (app_rate < DS_EPS_1MB_BITRATE_MINIMUM_VALUE)
  {
    *nw_rate = DS_EPS_MAX_1_OCTET_BIT_VALUE;
    *nw_rate_ex = (byte) ((app_rate - DS_EPS_100KB_BITRATE_BASE_RATE)/DS_EPS_100KB_BITRATE_MULTIPLE);
  }
  else if (app_rate < DS_EPS_2MB_BITRATE_MINIMUM_VALUE)
  {
    *nw_rate = DS_EPS_MAX_1_OCTET_BIT_VALUE;
    *nw_rate_ex = (byte) ((app_rate - DS_EPS_1MB_BITRATE_MINIMUM_VALUE)/DS_EPS_1MB_BITRATE_MULTIPLE
                           + DS_EPS_1MB_BITRATE_START_RANGE);
  }
  else if (app_rate <= DS_EPS_EXT_BITRATE_MAXIMUM_VALUE)
  {
    *nw_rate = DS_EPS_MAX_1_OCTET_BIT_VALUE;
    *nw_rate_ex = (byte) ((app_rate - DS_EPS_2MB_BITRATE_MINIMUM_VALUE)/DS_EPS_2MB_BITRATE_MULTIPLE 
                           + DS_EPS_2MB_BITRATE_START_RANGE);
  }
  else
  {
    // treat all higher rates as 256mbps
    *nw_rate = DS_EPS_MAX_1_OCTET_BIT_VALUE;
    *nw_rate_ex = DS_EPS_2MB_BITRATE_END_RANGE;
  }
  DS_LTE_MSG2_HIGH("Rate returned to NW is 0x%x, ext_rate:0x%x",*nw_rate,*nw_rate_ex);
} /* ds_eps_bearer_flow_manager_convert_app_bit_rate_to_nw */

/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_MANAGER_CONVERT_APP_FLOW_TO_NW_FLOW

DESCRIPTION
  Converts the EPS passed Flow spec to App flow spec.

PARAMETERS
  nw_flow         -  EPS flow to be converted
  app_flow        -  App flow to store the converted info.

DEPENDENCIES
  None.

RETURN VALUE
boolean          - returns if conversion is successfull
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_bearer_flow_manager_convert_app_flow_to_nw_flow
(
  sdf_qos_T              *nw_flow,
  qos_spec_type           *app_flow
)
{
  ip_flow_type *app_rx_flow = NULL;
  ip_flow_type *app_tx_flow = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_LTE_MSG0_LOW("EPS:ConvertAppFlowToNwFlow");
  if(nw_flow == NULL || app_flow == NULL)
  {
    DS_LTE_MSG0_ERROR("Input params are NULL");
    return FALSE;
  } 

  app_rx_flow = &(app_flow->rx.flow_template.req_flow);
  app_tx_flow = &(app_flow->tx.flow_template.req_flow);

  if(app_flow->field_mask & QOS_MASK_RX_FLOW)
  {
    // converting rx flow
    if (app_rx_flow->data_rate.format_type != DATA_RATE_FORMAT_MIN_MAX_TYPE &&
        app_rx_flow->data_rate.format_type != DATA_RATE_FORMAT_MIN_MAX_EX_TYPE)
    {
      DS_LTE_MSG1_ERROR("RX Flow data rate type is invalid:%d",
                        app_rx_flow->data_rate.format_type);
      return FALSE;
    }
    else if(app_rx_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_TYPE) 
    {
      nw_flow->max_bit_rate_dl_ex2 = 0;
      nw_flow->gbr_dl_ex2 = 0;
      ds_eps_bearer_flow_manager_convert_app_bit_rate_to_nw(
                                app_rx_flow->data_rate.format.min_max.max_rate,
                                             &nw_flow->max_bit_rate_dl,
                                             &nw_flow->max_bit_rate_dl_ex);

      ds_eps_bearer_flow_manager_convert_app_bit_rate_to_nw(
                         app_rx_flow->data_rate.format.min_max.guaranteed_rate,
                                             &nw_flow->gbr_dl,
                                             &nw_flow->gbr_dl_ex);
      }
    else
    {
      ds_eps_bearer_flow_manager_convert_app_bit_rate_to_nw_ext(
                                app_rx_flow->data_rate.format.min_max_ex.max_rate,
                                             &nw_flow->max_bit_rate_dl,
                                             &nw_flow->max_bit_rate_dl_ex,
                                             &nw_flow->max_bit_rate_dl_ex2);

      ds_eps_bearer_flow_manager_convert_app_bit_rate_to_nw_ext(
                         app_rx_flow->data_rate.format.min_max_ex.guaranteed_rate,
                                             &nw_flow->gbr_dl,
                                             &nw_flow->gbr_dl_ex,
                                             &nw_flow->gbr_dl_ex2);
    }
    } /* QOS_MASK_RX_FLOW */

    if(app_flow->field_mask & QOS_MASK_TX_FLOW)
    {
      // converting tx flow
      if (app_tx_flow->data_rate.format_type != DATA_RATE_FORMAT_MIN_MAX_TYPE &&
          app_tx_flow->data_rate.format_type != DATA_RATE_FORMAT_MIN_MAX_EX_TYPE)
      {
        DS_LTE_MSG1_ERROR("RX Flow data rate type is invalid:%d",
                          app_tx_flow->data_rate.format_type);
        return FALSE;
      }
      else if(app_rx_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_TYPE) 
      {
        nw_flow->max_bit_rate_ul_ex2 = 0;
        nw_flow->gbr_ul_ex2 = 0;
        ds_eps_bearer_flow_manager_convert_app_bit_rate_to_nw(
                             app_tx_flow->data_rate.format.min_max.max_rate,
                                           &nw_flow->max_bit_rate_ul,
                                           &nw_flow->max_bit_rate_ul_ex);

        ds_eps_bearer_flow_manager_convert_app_bit_rate_to_nw(
                      app_tx_flow->data_rate.format.min_max.guaranteed_rate,
                                           &nw_flow->gbr_ul,
                                           &nw_flow->gbr_ul_ex);
      }
      else
      {
        ds_eps_bearer_flow_manager_convert_app_bit_rate_to_nw_ext(
                             app_tx_flow->data_rate.format.min_max_ex.max_rate,
                                           &nw_flow->max_bit_rate_ul,
                                           &nw_flow->max_bit_rate_ul_ex,
                                           &nw_flow->max_bit_rate_ul_ex2);

        ds_eps_bearer_flow_manager_convert_app_bit_rate_to_nw_ext(
                      app_tx_flow->data_rate.format.min_max_ex.guaranteed_rate,
                                           &nw_flow->gbr_ul,
                                           &nw_flow->gbr_ul_ex,
                                           &nw_flow->gbr_ul_ex2);
      }
    } /* QOS_MASK_TX_FLOW */

    // if any of the extended-2 bit is none zero, we need to include the extended-2 data
    if (nw_flow->max_bit_rate_dl_ex2 || nw_flow->gbr_dl_ex2 || 
        nw_flow->max_bit_rate_ul_ex2 || nw_flow->gbr_ul_ex2)
    {
      nw_flow->length = DS_EPS_QOS_QCI_LEN_FOR_EXT2_DATA;
    }
    // if any of the extended bit is none zero, we need to include the extended data
    else if (nw_flow->max_bit_rate_dl_ex || nw_flow->gbr_dl_ex || 
        nw_flow->max_bit_rate_ul_ex || nw_flow->gbr_ul_ex)
    {
      nw_flow->length = DS_EPS_QOS_QCI_LEN_FOR_EXT_DATA;
    }
    else
    {
      nw_flow->length = DS_EPS_QOS_QCI_LEN_FOR_DATA;
    }

  /* Configure Data Rate when TX mask is not set*/  
  if (!(app_flow->field_mask & QOS_MASK_TX_FLOW))
  {
    // Configure TX data rate to zero
    nw_flow->max_bit_rate_ul = DS_EPS_0KB_BITRATE_VALUE;
    nw_flow->gbr_ul = DS_EPS_0KB_BITRATE_VALUE;
    if(nw_flow->length == DS_EPS_QOS_QCI_LEN_FOR_EXT_DATA)
    {
      nw_flow->max_bit_rate_ul_ex = DS_EPS_0KB_BITRATE_VALUE; 
      nw_flow->gbr_ul_ex = DS_EPS_0KB_BITRATE_VALUE; 
    }
    else if (nw_flow->length == DS_EPS_QOS_QCI_LEN_FOR_EXT2_DATA) 
    {
      nw_flow->max_bit_rate_ul_ex2 = DS_EPS_EXT2_MIN_BIT_VALUE;
      nw_flow->gbr_ul_ex2 = DS_EPS_EXT2_MIN_BIT_VALUE;
    }
  }

  /* Configure Data Rate when RX mask is not set*/  
  if (!(app_flow->field_mask & QOS_MASK_RX_FLOW))
  {
    // Configure RX data rate to zero
    nw_flow->max_bit_rate_dl = DS_EPS_0KB_BITRATE_VALUE;
    nw_flow->gbr_dl = DS_EPS_0KB_BITRATE_VALUE;
    if(nw_flow->length == DS_EPS_QOS_QCI_LEN_FOR_EXT_DATA)
    {
      nw_flow->max_bit_rate_dl_ex = DS_EPS_0KB_BITRATE_VALUE;
      nw_flow->gbr_dl_ex = DS_EPS_0KB_BITRATE_VALUE;
    }
    else if(nw_flow->length == DS_EPS_QOS_QCI_LEN_FOR_EXT2_DATA)
    {
      nw_flow->max_bit_rate_dl_ex = DS_EPS_EXT2_MIN_BIT_VALUE;
      nw_flow->gbr_dl_ex = DS_EPS_EXT2_MIN_BIT_VALUE;
    }
  }

  return TRUE; 
} /* ds_eps_bearer_flow_manager_convert_app_flow_to_nw_flow */

/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_MANAGER_CONVERT_APP_DATA_RATE_TO_NW
DESCRIPTION
  Converts the APP passed data rate to EPS NW data rate.

PARAMETERS
app_qos         -  Application qos to be converted
nw_qos          -  EPS network qos
DEPENDENCIES
  None.

RETURN VALUE
boolean         - return DS3G_SUCCESS or DS3G_FAILURE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_bearer_flow_manager_convert_app_data_rate_to_nw
(
   qos_spec_type   *app_qos,
   void            *nw_qos
)
{
  sdf_qos_T                   *lte_qos_ptr = NULL;
  ip_flow_type                *app_rx_flow = NULL;
  ip_flow_type                *app_tx_flow = NULL;
  boolean                      result = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(nw_qos == NULL || app_qos == NULL)
  {
    DS_LTE_MSG0_ERROR("input parameters are NULL");
    return DS3G_FAILURE;
  }

  lte_qos_ptr  = (sdf_qos_T *)nw_qos;

  /*-------------------------------------------------------------------------
    get the RX/TX app flows  
  -------------------------------------------------------------------------*/
  app_rx_flow = &(app_qos->rx.flow_template.req_flow);
  app_tx_flow = &(app_qos->tx.flow_template.req_flow);

  if( (app_qos->field_mask & QOS_MASK_TX_FLOW) &&
      (app_qos->field_mask & QOS_MASK_RX_FLOW) )
  {
  // make sure that QCI matches in rx/tx flow
    if (app_rx_flow->lte_params.lte_qci != app_tx_flow->lte_params.lte_qci)
    {
      DS_LTE_MSG0_ERROR("RX QCI and TX QCI does not match.");
      return DS3G_FAILURE;
    }
  }

  lte_qos_ptr->valid = TRUE;
  lte_qos_ptr->length = 1;
  if(app_qos->field_mask & QOS_MASK_TX_FLOW)
  {
    lte_qos_ptr->qci = (byte) app_tx_flow->lte_params.lte_qci;
  }
  // if both are set, they will be same.
  if(app_qos->field_mask & QOS_MASK_RX_FLOW)
  {
    lte_qos_ptr->qci = (byte) app_rx_flow->lte_params.lte_qci;
  }


  /*------------------------------------------------------------------------
    Convert App QoS to LTE QoS
  -------------------------------------------------------------------------*/
  if(lte_qos_ptr->qci >=(byte)(LTE_QCI_1)
                && lte_qos_ptr->qci <=(byte)(LTE_QCI_4))
    {
    DS_LTE_MSG1_HIGH("Received GBR qos request from App with QCI value:%d",
		     lte_qos_ptr->qci);
      /*----------------------------------------------------------------------
        RX/TX FLOW SPEC: Convert Max and Guaranteed Data Rates (bps)
      -----------------------------------------------------------------------*/
      result = ds_eps_bearer_flow_manager_convert_app_flow_to_nw_flow(
                                                       lte_qos_ptr,app_qos);
      if(result == FALSE)
      {
        return result;
      }
    }/* GBR QCI */
  else if(lte_qos_ptr->qci >=(byte)(LTE_QCI_5)
          && lte_qos_ptr->qci <=(byte)(LTE_QCI_9))
  {
    DS_LTE_MSG1_HIGH("Received NON-GBR qos request from App with QCI value:%d",
                      lte_qos_ptr->qci);

  } /* N-GBR QCI*/  
  else
  {
    DS_LTE_MSG1_ERROR("Received qos with invalid QCI value:%d",lte_qos_ptr->qci);
    return DS3G_FAILURE;
  }


  return DS3G_SUCCESS;
}/* ds_eps_bearer_flow_manager_convert_app_data_rate_to_nw */

/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_MANAGER_FLOW_VALIDATION

DESCRIPTION
  Verify the app did not pass invalid flow parameters.

PARAMETERS
 input_qos           - Pointer to App passed Qos.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_bearer_flow_manager_flow_validation
( 
  qos_spec_type      *input_qos
)
{
  ip_flow_type    *rx_flow = NULL;
  ip_flow_type    *tx_flow = NULL; 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(input_qos==NULL)
  {
    DS_LTE_MSG0_ERROR("Input parameter is NULL");
    return FALSE;
  }
  /*------------------------------------------------------------------------
    Configure the flow pointers.
  -------------------------------------------------------------------------*/
  rx_flow = &(input_qos->rx.flow_template.req_flow);
  tx_flow = &(input_qos->tx.flow_template.req_flow);

  if( (input_qos->field_mask & QOS_MASK_TX_FLOW) &&
      (input_qos->field_mask & QOS_MASK_RX_FLOW) )
  {
    /*----------------------------------------------------------------------
      Verify if RX and TX QCI Match
    ----------------------------------------------------------------------*/
    if (tx_flow->lte_params.lte_qci != rx_flow->lte_params.lte_qci)
    {
      DS_LTE_MSG0_ERROR("RX QCI and TX QCI do not match in flow!");
      return FALSE;
    }

    /*----------------------------------------------------------------------
      Verify if RX and TX data rate are both zero.
    ----------------------------------------------------------------------*/
    if ((tx_flow->lte_params.lte_qci >= LTE_QCI_1)  &&
       (tx_flow->lte_params.lte_qci <=LTE_QCI_4))
    {
      if (((rx_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_TYPE)  &&
          (rx_flow->data_rate.format.min_max.max_rate == 0) &&
          (tx_flow->data_rate.format.min_max.max_rate == 0)) ||
          ((rx_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_EX_TYPE) &&
          (rx_flow->data_rate.format.min_max_ex.max_rate == 0) &&
          (tx_flow->data_rate.format.min_max_ex.max_rate == 0)))
      {
        DS_LTE_MSG0_ERROR("Data rate zero for GBR");
        return FALSE;
      }
    }
  }

  /*------------------------------------------------------------------------
    Verify TX FLOW Params
  -------------------------------------------------------------------------*/
  if(input_qos->field_mask & (QOS_MASK_TX_FLOW))
  {
    /* make sure the QCI value is valid*/
    if((tx_flow->lte_params.lte_qci < LTE_QCI_1)||
                      (tx_flow->lte_params.lte_qci > LTE_QCI_9))
    {
      DS_LTE_MSG0_ERROR("Invalid QCI value in Tx flow spec!");
      return FALSE;
    }

    /* If the QCI is GBR, make sure the TX MBR is not zero  */      
    if ((tx_flow->lte_params.lte_qci >= LTE_QCI_1)  &&
       (tx_flow->lte_params.lte_qci <=LTE_QCI_4))
    {
      if ((((tx_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_TYPE) &&
          (tx_flow->data_rate.format.min_max.max_rate == 0)) ||  
          ((tx_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_EX_TYPE) &&
          (tx_flow->data_rate.format.min_max_ex.max_rate == 0))) &&
           (~(input_qos->field_mask & QOS_MASK_RX_FLOW)))     
      {
        DS_LTE_MSG0_ERROR("TX Data rate cannot be zero");
        return FALSE;
      }
    }

    if (((tx_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_TYPE) &&
        (tx_flow->data_rate.format.min_max.max_rate >
                        DS_EPS_EXT_BITRATE_MAXIMUM_VALUE * ds_eps_kbps_to_bps_multiplier_32bit)) ||
       ((tx_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_EX_TYPE) &&
        (tx_flow->data_rate.format.min_max_ex.max_rate >
                        DS_EPS_EXT2_BITRATE_MAXIMUM_VALUE * ds_eps_kbps_to_bps_multiplier_64bit)))
    {
      DS_LTE_MSG0_ERROR("TX Data rate exceeded maximum supported");
      return FALSE;
    }
  } /* if QOS_MASK_TX_FLOW */

  /*------------------------------------------------------------------------
    Verify RX FLOW Params
  -------------------------------------------------------------------------*/
  if(input_qos->field_mask & (QOS_MASK_RX_FLOW))
  {
    /* make sure the QCI value is valid*/
    if ((rx_flow->lte_params.lte_qci < LTE_QCI_1)||
        (rx_flow->lte_params.lte_qci > LTE_QCI_9) )
    {
      DS_LTE_MSG0_ERROR("Invalid QCI value in Rx flow spec!");
      return FALSE;
    }

    /* If the QCI is GBR, make sure the RX MBR is not zero */      
    if ((rx_flow->lte_params.lte_qci >= LTE_QCI_1)  &&
       (rx_flow->lte_params.lte_qci <=LTE_QCI_4))
    {
      if ((((rx_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_TYPE) &&
      (rx_flow->data_rate.format.min_max.max_rate == 0)) ||  
      ((rx_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_EX_TYPE) &&
      (rx_flow->data_rate.format.min_max_ex.max_rate == 0))) &&
           (~(input_qos->field_mask & QOS_MASK_TX_FLOW)))        
      {
        DS_LTE_MSG0_ERROR("RX Data rate cannot be zero");
        return FALSE;
      }
    }

    if (((rx_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_TYPE) &&
        (rx_flow->data_rate.format.min_max.max_rate >
                        DS_EPS_EXT_BITRATE_MAXIMUM_VALUE * ds_eps_kbps_to_bps_multiplier_32bit)) ||
       ((rx_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_EX_TYPE) &&
        (rx_flow->data_rate.format.min_max_ex.max_rate >
                        DS_EPS_EXT2_BITRATE_MAXIMUM_VALUE * ds_eps_kbps_to_bps_multiplier_64bit)))
    {
      DS_LTE_MSG0_ERROR("Rx Data rate exceeded maximum supported");
      return FALSE;
    }
  } /* if QOS_MASK_RX_FLOW */

  return TRUE;
} /* ds_eps_bearer_flow_manager_flow_validation */

/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_GET_LTE_NETW_ALLOC_QOS

DESCRIPTION
  Convert app QoS and stores the results in lte_netw_alloc_qos.

PARAMETERS
  bearer_context_p   - bearer context pointer.
  lte_netw_alloc_qos - pointer to hold the converted values

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
void
ds_eps_bearer_flow_manager_get_lte_netw_alloc_qos
(
  ds_bearer_context_s          *bearer_context_p,
  ds_3gpp_lte_qos_params_type  *lte_netw_alloc_qos
)
{

  ds_bearer_flow_manager_s    *bflow_manager_p;
  ip_flow_type                *app_rx_flow;
  ip_flow_type                *app_tx_flow;
  
  if(bearer_context_p == NULL)
  {
    DS_LTE_MSG0_HIGH("Null pointer passed.");
    return;
  }

  bflow_manager_p = ds_bearer_flow_manager_find_flow_manager(bearer_context_p);

  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    return;
  }

  app_rx_flow = &(bflow_manager_p->ds_bearer_flow_manager_dyn_p->rx_flow);
  app_tx_flow = &(bflow_manager_p->ds_bearer_flow_manager_dyn_p->tx_flow);

  if(app_rx_flow == NULL || app_tx_flow == NULL)
  {
    DS_LTE_MSG0_HIGH("Null flows in bflow_manager_p.");
    return;
  }

  if( (app_rx_flow->field_mask & (uint32)IPFLOW_MASK_LTE_QCI) || 
      (app_rx_flow->field_mask & (uint32)IPFLOW_MASK_DATA_RATE) ||
      (app_tx_flow->field_mask & (uint32)IPFLOW_MASK_DATA_RATE)
    )
  {
    lte_netw_alloc_qos->valid_flg = TRUE;
  }

  if (app_rx_flow->field_mask & (uint32)IPFLOW_MASK_LTE_QCI)
  {
    lte_netw_alloc_qos->qci = 
             (ds_3gpp_lte_qci_e_type )app_rx_flow->lte_params.lte_qci;
  }
  if(app_rx_flow->field_mask & (uint32)IPFLOW_MASK_DATA_RATE)
  {
    if(app_rx_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_TYPE)
    {
      lte_netw_alloc_qos->max_dl_bit_rate = 
            (app_rx_flow->data_rate.format.min_max.max_rate)/ds_eps_kbps_to_bps_multiplier_32bit;
      lte_netw_alloc_qos->g_dl_bit_rate = 
            (app_rx_flow->data_rate.format.min_max.guaranteed_rate)/ds_eps_kbps_to_bps_multiplier_32bit;
    }
    else if (app_rx_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_EX_TYPE) 
    {
    lte_netw_alloc_qos->max_dl_bit_rate = 
            (uint32)((app_rx_flow->data_rate.format.min_max_ex.max_rate)/ds_eps_kbps_to_bps_multiplier_64bit);
    lte_netw_alloc_qos->g_dl_bit_rate = 
            (uint32)((app_rx_flow->data_rate.format.min_max_ex.guaranteed_rate)/ds_eps_kbps_to_bps_multiplier_64bit);
    }
    
  }
  if(app_tx_flow->field_mask & (uint32)IPFLOW_MASK_DATA_RATE)
  {
    if(app_tx_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_TYPE)
    {
    lte_netw_alloc_qos->max_ul_bit_rate = 
             (app_tx_flow->data_rate.format.min_max.max_rate)/ds_eps_kbps_to_bps_multiplier_32bit;
    lte_netw_alloc_qos->g_ul_bit_rate = 
             (app_tx_flow->data_rate.format.min_max.guaranteed_rate)/ds_eps_kbps_to_bps_multiplier_32bit;
    }
    else if(app_tx_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_EX_TYPE)
    {
      lte_netw_alloc_qos->max_ul_bit_rate = 
             (uint32)((app_tx_flow->data_rate.format.min_max_ex.max_rate)/ds_eps_kbps_to_bps_multiplier_64bit);
      lte_netw_alloc_qos->g_ul_bit_rate = 
             (uint32)((app_tx_flow->data_rate.format.min_max_ex.guaranteed_rate)/ds_eps_kbps_to_bps_multiplier_64bit);
    }
    
  }

} /* ds_eps_bearer_flow_manager_get_lte_netw_alloc_qos */

/*===========================================================================
FUNCTION DS_EPS_BEARER_FLOW_MANAGER_UPDATE_FLOW_ON_RESUME

DESCRIPTION
  Updates flow from W->L handoff with L params

PARAMETERS
  bflow_manager_p   - bearer flow manager ptr.

DEPENDENCIES
  None.

RETURN VALUE
  boolean - True if success, false otherwise.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_bearer_flow_manager_update_flow_on_resume
(
  ds_bearer_flow_manager_s *bflow_manager_p
)
{
  qos_spec_type       app_qos;
  ds_flow_context_s  *flow_context_p = NULL;
  ds_eps_bearer_flow_manager_s *eps_bflow_manager_p = NULL;
  sdf_qos_T sdf_qos;
  ds_3gpp_internal_cause_code_enum_type icause_code;
  icause_code = DS_3GPP_INTERNAL_CC_INVALID;

  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    return FALSE;
  }

  memset(&app_qos, 0, sizeof(qos_spec_type));
  memset(&sdf_qos, 0, sizeof(sdf_qos_T));

  // get eps sdf qos
  eps_bflow_manager_p = (ds_eps_bearer_flow_manager_s *)
    DS_BEARER_FLOW_MGR_VOBJ(bflow_manager_p,
                            SYS_SYS_MODE_LTE);

  if(!ds_eps_bearer_flow_manager_validate_eps_bearer_flow_manager
     (eps_bflow_manager_p))
  {
    DS_LTE_MSG0_ERROR("eps_bearer_context_p is NULL");
    return FALSE;
  }

  if(bflow_manager_p->ds_bearer_flow_manager_dyn_p->neg_qos.valid)
  {  
    /* convert this to sdf qos */
    ds_3gpp_convert_pre_rel8_to_eps_qos(&bflow_manager_p->ds_bearer_flow_manager_dyn_p
                                        ->neg_qos, &sdf_qos);
  }

  if(sdf_qos.valid)
  {
    memscpy((void *)&(eps_bflow_manager_p->eps_bearer_flow_manager_dyn_p->sdf_qos),
            sizeof(sdf_qos_T),
           (void *)&(sdf_qos), sizeof(sdf_qos_T));
  }


  if(eps_bflow_manager_p->eps_bearer_flow_manager_dyn_p->sdf_qos.valid)
  {
    // convert lte qos to app format
    if (ds_3gpp_bearer_flow_manager_convert_nw_sdf_qos_to_app(bflow_manager_p,
                              &(eps_bflow_manager_p->eps_bearer_flow_manager_dyn_p->sdf_qos),
                                                      &app_qos,
                                                    &icause_code) == FALSE)  
    {
      DS_LTE_MSG0_ERROR("Data Rate not updated in the flow");
      return FALSE;
    }
	
    /* Mode independent*/
    memscpy((void *)&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->rx_flow),
	     sizeof(ip_flow_type),
	     (void *)&(app_qos.rx.flow_template.req_flow),sizeof(ip_flow_type));		  
	
    memscpy((void *)&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->tx_flow),
		sizeof(ip_flow_type),
		(void *)&(app_qos.tx.flow_template.req_flow),sizeof(ip_flow_type)); 

    /*---------------------------------------------------------------------
      Update all the Flows with the updated flow rate
      Need to optimize for the case, there will be some flows in going
      down state and will not be intereseted in flow rate change.
    ---------------------------------------------------------------------*/          
    flow_context_p = list_peek_front(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p
                                       ->flow_context));
    while(ds_flow_context_validate_flow(flow_context_p))
    {
      ds_flow_context_ps_flow_set_granted_flow(flow_context_p);
      flow_context_p = list_peek_next(&(bflow_manager_p->
                                        ds_bearer_flow_manager_dyn_p->flow_context),
                                    &(flow_context_p->link));
    }
  }
  else
  {
    DS_LTE_MSG0_LOW("No sdf qos to update the flow");
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================
FUNCTION ds_eps_bearer_flow_manager_allocate_eps_bearer_flow_manager

DESCRIPTION
  This function allocates dynamic memory for the index in the eps bearer flow manager table

PARAMETERS
int alloc_index
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE 
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_eps_bearer_flow_manager_alloc_eps_bearer_flow_manager
(
   int alloc_index
)
{
  ds_eps_bearer_flow_manager_dyn_s *eps_bearer_flow_manager_dyn_p = NULL;
  boolean ret_val = FALSE;
 /*------------------------------------------------------------------------*/
  if((alloc_index < 0) || (alloc_index >= DS_3GPP_MAX_BEARER_CONTEXT))
  {
    DS_LTE_MSG1_LOW("Alloc index out of range:%d",alloc_index);
    return ret_val;
  }

  eps_bearer_flow_manager_dyn_p = ds_eps_bflow_manager_tbl[alloc_index].
                                                  eps_bearer_flow_manager_dyn_p;
  if(eps_bearer_flow_manager_dyn_p == NULL)
  {
    eps_bearer_flow_manager_dyn_p = (ds_eps_bearer_flow_manager_dyn_s *)
    modem_mem_alloc(sizeof(ds_eps_bearer_flow_manager_dyn_s),
                    MODEM_MEM_CLIENT_DATA);
    if(eps_bearer_flow_manager_dyn_p != NULL)
    {
      memset(eps_bearer_flow_manager_dyn_p,0,
             sizeof(ds_eps_bearer_flow_manager_dyn_s));
      eps_bearer_flow_manager_dyn_p->cookie = EPS_BEARER_FLOW_MGR_VALID_COOKIE;
      eps_bearer_flow_manager_dyn_p->index = alloc_index;
      DS_LTE_MSG1_MED("EPS bearer context allocated at index:%d",
                       eps_bearer_flow_manager_dyn_p->index);
      ds_eps_bflow_manager_tbl[alloc_index].eps_bearer_flow_manager_dyn_p = 
        eps_bearer_flow_manager_dyn_p;
      ret_val = TRUE;
    }
    else
    {

      DS_LTE_MSG1_LOW("Failure to allocate EPS BFLOW MGR at index:%d",
                         alloc_index);

    }

  }
  else
  {
    DS_LTE_MSG1_LOW("EPS BFLOW MGR dyn memory already allocated at index:%d",
                       alloc_index);
  }

  return ret_val;
}

/*===========================================================================
FUNCTION ds_eps_bearer_flow_manager_validate_eps_bearer_flow_manager

DESCRIPTION
  This function validates dynamic memory for the index in the eps bearer
  flow manager table

PARAMETERS
int alloc_index
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE 
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_eps_bearer_flow_manager_validate_eps_bearer_flow_manager
(
   ds_eps_bearer_flow_manager_s *eps_bearer_flow_manager_p
)
{
    boolean ret_val = FALSE;
 /*------------------------------------------------------------------------*/
    if(eps_bearer_flow_manager_p == NULL)
    {
      DS_LTE_MSG0_ERROR("Input EPS BFLOW MGR is NULL");
      return ret_val;
    }

    if((eps_bearer_flow_manager_p->eps_bearer_flow_manager_dyn_p != NULL) &&
       (eps_bearer_flow_manager_p->eps_bearer_flow_manager_dyn_p->cookie 
        == EPS_BEARER_FLOW_MGR_VALID_COOKIE))
    {
      DS_LTE_MSG1_LOW("EPS BFLOW MGR is allocated at index:%d",
                      eps_bearer_flow_manager_p->eps_bearer_flow_manager_dyn_p->index);
      ret_val = TRUE;
    }

    return ret_val;
}

/*===========================================================================
FUNCTION ds_eps_bearer_flow_manager_free_eps_bearer_flow_manager

DESCRIPTION
  This function frees dynamic memory for the index in the
  eps bearer flow manager table

PARAMETERS
int index
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE 
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_eps_bearer_flow_manager_free_eps_bearer_flow_manager
(
   int index
)
{
   boolean ret_val = FALSE;
   ds_eps_bearer_flow_manager_s *eps_bearer_flow_manager_p = NULL;
 /*------------------------------------------------------------------------*/

   if((index < 0) || (index >= DS_3GPP_MAX_BEARER_CONTEXT))
   {
     DS_LTE_MSG1_ERROR("Index:%d is out of bounds",index);
     return ret_val;
   }

   eps_bearer_flow_manager_p = &ds_eps_bflow_manager_tbl[index];

   if(eps_bearer_flow_manager_p->eps_bearer_flow_manager_dyn_p != NULL)
   {
     modem_mem_free(eps_bearer_flow_manager_p->eps_bearer_flow_manager_dyn_p,
                    MODEM_MEM_CLIENT_DATA);
     eps_bearer_flow_manager_p->eps_bearer_flow_manager_dyn_p = NULL;
     ret_val = TRUE;
   }

   return ret_val;

}

#endif /*FEATURE_DATA_LTE */
