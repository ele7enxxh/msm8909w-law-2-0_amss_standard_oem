/*!
  @file
  ds_umts_bearer_flow_manager.c

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/umtsps/src/ds_umts_bearer_flow_manager.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/19/11   nd      Bearer rate querry uses NW granted bit rates.
02/02/11   ttv     Added support for get session params.
05/05/10   vrk     Initial version.
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include <stringl/stringl.h>

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#include "dstask.h"
#include "ds3gmgr.h"
#include "ds_umts_bearer_flow_manager.h"
#include "ds_3gpp_bearer_flow_manager.h"
#include "ds_umts_bearer_context.h"
#include "ds3gmshif.h"
#include "dsumtspdpreg.h"
#include "dsumtspsqos.h"
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
  UMTS bearer Flow manager table exposed to other files through header
---------------------------------------------------------------------------*/
ds_umts_bearer_flow_manager_s ds_umts_bflow_manager_tbl[DS_3GPP_MAX_BEARER_CONTEXT] = {{NULL}};


/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION DS_UMTS_BEARER_FLOW_MANAGER_INIT

DESCRIPTION
  Register UMTS specific functions

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_umts_bearer_flow_manager_init( void )
{
  ds_3gpp_bearer_flow_manager_ftbl_s  *bearer_flow_manager_ftbl_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    Initialize WCDMA and GSM bearer flow manager Function table
  -------------------------------------------------------------------------*/
#ifdef FEATURE_WCDMA

  bearer_flow_manager_ftbl_p = 
                    &(ds_bearer_flow_manager_ftbl[DS_3GPPI_SYS_MODE_WCDMA]);
  memset(
         (void*)bearer_flow_manager_ftbl_p,
         0x0,
         (sizeof(ds_3gpp_bearer_flow_manager_ftbl_s))
        );

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA, nwDataRateToAppDataRate,
                     ds_umts_bearer_flow_manager_convert_nw_data_rate_to_app);

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA, appDataRateToNwDataRate,
                     ds_umts_bearer_flow_manager_convert_app_data_rate_to_nw);

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA, flowValidation,
                                 ds_umts_bearer_flow_manager_flow_validation);

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA, getBearerRate,
                                 ds_3gpp_bearer_flow_manager_get_app_data_rate);

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA, free_umts_bearer_flow_manager, \
              ds_umts_bearer_flow_manager_free_umts_bearer_flow_manager);

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA, allocate_umts_bearer_flow_manager, \
              ds_umts_bearer_flow_manager_allocate_umts_bearer_flow_manager);

#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_GPRS
  bearer_flow_manager_ftbl_p = 
                    &(ds_bearer_flow_manager_ftbl[DS_3GPPI_SYS_MODE_GSM]);
  memset(
         (void*)bearer_flow_manager_ftbl_p,
         0x0,
         (sizeof(ds_3gpp_bearer_flow_manager_ftbl_s))
        );

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM, nwDataRateToAppDataRate,
                     ds_umts_bearer_flow_manager_convert_nw_data_rate_to_app);

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM, appDataRateToNwDataRate,
                     ds_umts_bearer_flow_manager_convert_app_data_rate_to_nw);

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM, flowValidation,
                                 ds_umts_bearer_flow_manager_flow_validation);

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM, getBearerRate,
                                 ds_3gpp_bearer_flow_manager_get_app_data_rate);

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM, free_umts_bearer_flow_manager, \
              ds_umts_bearer_flow_manager_free_umts_bearer_flow_manager);

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM, allocate_umts_bearer_flow_manager, \
              ds_umts_bearer_flow_manager_allocate_umts_bearer_flow_manager);

#endif /* FEATURE_GSM_GPRS */

#ifdef FEATURE_TDSCDMA
  bearer_flow_manager_ftbl_p = 
                    &(ds_bearer_flow_manager_ftbl[DS_3GPPI_SYS_MODE_TDS]);
  memset(
         (void*)bearer_flow_manager_ftbl_p,
         0x0,
         (sizeof(ds_3gpp_bearer_flow_manager_ftbl_s))
        );

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS, nwDataRateToAppDataRate,
                     ds_umts_bearer_flow_manager_convert_nw_data_rate_to_app);

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS, appDataRateToNwDataRate,
                     ds_umts_bearer_flow_manager_convert_app_data_rate_to_nw);

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS, flowValidation,
                                 ds_umts_bearer_flow_manager_flow_validation);

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS, getBearerRate,
                                 ds_3gpp_bearer_flow_manager_get_app_data_rate);

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS, free_umts_bearer_flow_manager, \
              ds_umts_bearer_flow_manager_free_umts_bearer_flow_manager);

  DS_BEARER_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS, allocate_umts_bearer_flow_manager, \
              ds_umts_bearer_flow_manager_allocate_umts_bearer_flow_manager);

#endif /* FEATURE_TDSCDMA */


} /*ds_umts_bearer_flow_manager_init */



/*===========================================================================
FUNCTION ds_umts_bearer_flow_manager_validate

DESCRIPTION
  This function checks if the pointer to the umts bearer flow manager is valid

PARAMETERS
  umts_bflow_manager_p  -umts_bflow_manager_p to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/


inline boolean ds_umts_bearer_flow_manager_validate
(
  ds_umts_bearer_flow_manager_s *umts_bflow_manager_p
)
{
  boolean ret_val = FALSE;


  if(umts_bflow_manager_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input Bearer Flow Manager parameter passed is INVALID");
    return ret_val;
  }

  if ((umts_bflow_manager_p->ds_umts_bearer_flow_manager_dynamic_p != NULL) && 
       umts_bflow_manager_p->ds_umts_bearer_flow_manager_dynamic_p->cookie == 
       UMTS_BEARER_FLOW_MANAGER_VALID_COOKIE)

  {
    DS_3GPP_MSG0_MED("Umts Bearer Flow Manager has dyn memory allocated");
    ret_val = TRUE;
  }

  return ret_val;
}

/*===========================================================================
FUNCTION ds_umts_bearer_flow_manager_allocate_umts_bearer_flow_manager

DESCRIPTION
  This function allocates dynamic memory for the index in the umts bearer flow manager table

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


boolean ds_umts_bearer_flow_manager_allocate_umts_bearer_flow_manager(int loop_index)
{
     boolean ret_val=FALSE;

   if ((loop_index < 0) || (loop_index >= DS_3GPP_MAX_BEARER_CONTEXT))
   {
      DS_3GPP_MSG0_ERROR("Index invalid, return FALSE");
      return ret_val;
   }

   if (ds_umts_bflow_manager_tbl[loop_index].ds_umts_bearer_flow_manager_dynamic_p == NULL)
   {
       ds_umts_bflow_manager_tbl[loop_index].ds_umts_bearer_flow_manager_dynamic_p=(ds_umts_bearer_flow_manager_dynamic_s *)
       modem_mem_alloc(sizeof(ds_umts_bearer_flow_manager_dynamic_s),MODEM_MEM_CLIENT_DATA);


       if(ds_umts_bflow_manager_tbl[loop_index].ds_umts_bearer_flow_manager_dynamic_p== NULL)
       {
         DS_3GPP_MSG3_ERROR("Heap Memory Allocation failed for client: %d size: %d", 
         MODEM_MEM_CLIENT_DATA,(sizeof(ds_umts_bearer_context_dyn_s)),0);
         return ret_val;
       }

       memset((void *)ds_umts_bflow_manager_tbl[loop_index].ds_umts_bearer_flow_manager_dynamic_p,
              0x0,sizeof(ds_umts_bearer_flow_manager_dynamic_s));
       ds_umts_bflow_manager_tbl[loop_index].ds_umts_bearer_flow_manager_dynamic_p->cookie =   
            UMTS_BEARER_FLOW_MANAGER_VALID_COOKIE;
       ret_val= TRUE;
       DS_3GPP_MSG1_LOW("Dynamic Memory is allocated at index as  part of allocation",loop_index);
      
  }
  else
  {
        DS_3GPP_MSG1_ERROR("Dynamic Memory is already allocated at index ",loop_index);
  }

  return ret_val;
}


/*===========================================================================
FUNCTION ds_umts_bearer_flow_manager_free_umts_bearer_flow_manager

DESCRIPTION
  This function free's the umts bearer flow manager

PARAMETERS
ds_umts_bearer_flow_manager_s *umts_bearer_flow_manager_p 
    
DEPENDENCIES
  

RETURN VALUE
  TRUE 
  FALSE

SIDE EFFECTS
  None

===========================================================================*/


boolean ds_umts_bearer_flow_manager_free_umts_bearer_flow_manager(int loop_index)
{
    
  boolean ret_val=FALSE;
  ds_umts_bearer_flow_manager_s *umts_bearer_flow_manager_p = NULL;

  if ((loop_index < 0) || (loop_index >= DS_3GPP_MAX_BEARER_CONTEXT))
  {
   DS_3GPP_MSG0_ERROR("Index invalid, return FALSE");
   return ret_val;
  }

  umts_bearer_flow_manager_p = &ds_umts_bflow_manager_tbl[loop_index];
      
  if((umts_bearer_flow_manager_p != NULL) && 
     (umts_bearer_flow_manager_p->ds_umts_bearer_flow_manager_dynamic_p != NULL))
  {
    modem_mem_free(umts_bearer_flow_manager_p->ds_umts_bearer_flow_manager_dynamic_p, MODEM_MEM_CLIENT_DATA);
    umts_bearer_flow_manager_p->ds_umts_bearer_flow_manager_dynamic_p = NULL;
    ret_val=TRUE;
  }
  else
  {
    DS_3GPP_MSG0_HIGH(" UMTS Bearer Flow Manager is invalid");
  }

  return ret_val;
}


/*===========================================================================
FUNCTION DS_UMTS_BEARER_FLOW_MANAGER_UPDATE_FILTER_ID

DESCRIPTION
  Assign unique filter id to filters if none are filled
  in by app.

PARAMETERS
 nw_tft - tft spec

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_umts_bearer_flow_manager_update_filter_id
( 
    tft_type_T          *nw_tft
)
{
  packet_filter_type_T *nw_filter = NULL;
  int index = 0;
  byte filter_id =0;
  byte direction =0;      
  uint32              temp_fi_id = 0;     
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  if(nw_tft ==NULL)
  {
    DS_3GPP_MSG0_ERROR("NULL parameters passed");    
    return DS3G_FAILURE;
  }     

  for(index=0; index < nw_tft->num_filters; index++)
  {
    int result = 0;
    /*----------------------------------------------------------------------
     Process Each packet filter
    -----------------------------------------------------------------------*/    
    nw_filter = &(nw_tft->filter_list[index]);    
    filter_id = nw_filter->identifier & 0xF;  
    direction = ((nw_filter->identifier & 0x30)>>4);  
    DS_3GPP_MSG2_HIGH("processing filter with ID:%d and direction 0x%x",
                                                     filter_id,direction);
        
    if((result = ds_bearer_flow_manager_find_and_set_filter_id(
        &temp_fi_id, DS_MIN_FI_PRECED, DS_MAX_FI_ID)) != -1)
    {
      filter_id = (byte) result ;
      nw_filter->identifier &= 0xF0;//overwrite current filter_id   
      nw_filter->identifier |= filter_id;
      DS_3GPP_MSG1_HIGH("Updated filter with ID:%d ",filter_id);
    }
    else
    {
      DS_3GPP_MSG1_ERROR("Failed to update filter id for:%d ",index);
      //return FALSE;//VRK:TODO
    }
  } /* for filters */
  
  
  return TRUE;
}/* ds_umts_bearer_flow_manager_update_filter_id */

/*===========================================================================
FUNCTION DS_UMTS_BEARER_FLOW_MANAGER_CONVERT_NW_BIT_RATE

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
uint32 ds_umts_bearer_flow_manager_convert_nw_bit_rate
(
  byte input
)
{
  uint32 converted_rate = 0;
#if 0
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
  converted_rate *= 1000;
  DS_3GPP_MSG1_HIGH("converted rate returned is 0x%x",converted_rate);
#endif
  return converted_rate;
} /* ds_umts_bearer_flow_manager_convert_nw_bit_rate */

/*===========================================================================
FUNCTION DS_UMTS_BEARER_FLOW_MANAGER_CONVERT_EXT_NW_BIT_RATE

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
uint32 ds_umts_bearer_flow_manager_convert_ext_nw_bit_rate
(
  byte input
)
{
  uint32 converted_rate = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#if 0
  if(input == 0)
  {
    DS_3GPP_MSG0_HIGH("Zero passed in extended bit rate");
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
    DS_3GPP_MSG0_ERROR("Invalid input passed to the extended bit rate");
	converted_rate = DS_EPS_INVALID_EXT_BITRATE;
  }

  // Convert NW rate (kbps) to App rate (bps)
  converted_rate *= 1000;
  DS_3GPP_MSG1_HIGH("converted extended rate returned is 0x%x",converted_rate);
#endif
  return converted_rate;
} /* ds_umts_bearer_flow_manager_convert_ext_nw_bit_rate */


/*===========================================================================
FUNCTION DS_UMTS_BEARER_FLOW_MANAGER_CONVERT_NW_FLOW_TO_APP_FLOW

DESCRIPTION
  Converts the UMTS passed Flow spec to App flow spec.

PARAMETERS
  nw_flow         -  UMTS flow to be converted
  app_flow        -  App flow to store the converted info.

DEPENDENCIES
  None.

RETURN VALUE
boolean          - returns if conversion is successfull
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_umts_bearer_flow_manager_convert_nw_flow_to_app_flow
(
  sdf_qos_T              *nw_flow,
  qos_spec_type           *app_flow
)
{
  boolean result = FALSE;
#if 0
  ip_flow_type *app_rx_flow = NULL;
  ip_flow_type *app_tx_flow = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_HIGH("UMTS:ConvertNwFlowToAppFlow");
  if(nw_flow == NULL || app_flow == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input params are NULL");
    return result;
  } 
  if((nw_flow->length == DS_EPS_QOS_QCI_LEN_FOR_DATA)
    || (nw_flow->length == DS_EPS_QOS_QCI_LEN_FOR_EXT_DATA))
  {
    DS_3GPP_MSG0_HIGH("Converting data rates");
    app_rx_flow = &(app_flow->rx.flow_template.req_flow);
    app_rx_flow->data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_TYPE;
    app_rx_flow->data_rate.format.min_max.max_rate =
         ds_umts_bearer_flow_manager_convert_nw_bit_rate(nw_flow->max_bit_rate_dl);
    app_rx_flow->data_rate.format.min_max.guaranteed_rate =
                 ds_umts_bearer_flow_manager_convert_nw_bit_rate(nw_flow->gbr_dl);
    app_rx_flow->lte_params.lte_qci =(lte_qci_enum_type)nw_flow->qci;
    app_rx_flow->field_mask |=(int)(IPFLOW_MASK_DATA_RATE);
    app_rx_flow->field_mask |=(int)(IPFLOW_MASK_LTE_QCI); 

    app_tx_flow = &(app_flow->tx.flow_template.req_flow);
    app_tx_flow->data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_TYPE;
    app_tx_flow->data_rate.format.min_max.max_rate = 
        ds_umts_bearer_flow_manager_convert_nw_bit_rate(nw_flow->max_bit_rate_ul);
    app_tx_flow->data_rate.format.min_max.guaranteed_rate = 
                ds_umts_bearer_flow_manager_convert_nw_bit_rate(nw_flow->gbr_ul);
    app_tx_flow->lte_params.lte_qci = (lte_qci_enum_type)nw_flow->qci;
    app_tx_flow->field_mask |=(int)(IPFLOW_MASK_DATA_RATE);
    app_tx_flow->field_mask |=(int)(IPFLOW_MASK_LTE_QCI);
    
    if((app_rx_flow->data_rate.format.min_max.max_rate == 0) 
		 && (app_tx_flow->data_rate.format.min_max.max_rate== 0))
    {
      DS_3GPP_MSG0_ERROR("MAX UL and MAX DL bit rates are zero");
      return result;
    }
    if(nw_flow->length == DS_EPS_QOS_QCI_LEN_FOR_DATA)
    {
      result = TRUE;
      return result;
    } 
  } /*nw_flow->length == 11||7 */

  if(nw_flow->length == DS_EPS_QOS_QCI_LEN_FOR_EXT_DATA)
  {
    DS_3GPP_MSG0_HIGH("Converting data rates and ext data rates");    
    if((nw_flow->max_bit_rate_dl_ex != 0) && 
       (app_rx_flow->data_rate.format.min_max.max_rate == 
                                             DS_EPS_MAX_1_OCTET_BITRATE*1000))
    {
      app_rx_flow->data_rate.format.min_max.max_rate =
           ds_umts_bearer_flow_manager_convert_ext_nw_bit_rate(
                                       nw_flow->max_bit_rate_dl_ex);
    }

    if((nw_flow->gbr_dl_ex != 0) &&
       (app_rx_flow->data_rate.format.min_max.guaranteed_rate == 
                                             DS_EPS_MAX_1_OCTET_BITRATE*1000))
      {
        app_rx_flow->data_rate.format.min_max.guaranteed_rate = 
            ds_umts_bearer_flow_manager_convert_ext_nw_bit_rate(
                                              nw_flow->gbr_dl_ex);
    }

    if((nw_flow->max_bit_rate_ul_ex != 0) &&
       (app_tx_flow->data_rate.format.min_max.max_rate == 
                                             DS_EPS_MAX_1_OCTET_BITRATE*1000))
      {
        app_tx_flow->data_rate.format.min_max.max_rate = 
         ds_umts_bearer_flow_manager_convert_ext_nw_bit_rate(
                                         nw_flow->max_bit_rate_ul_ex);
    }

    if((nw_flow->gbr_ul_ex != 0) &&
       (app_tx_flow->data_rate.format.min_max.guaranteed_rate ==
                                       DS_EPS_MAX_1_OCTET_BITRATE*1000))
    {
      app_tx_flow->data_rate.format.min_max.guaranteed_rate =
          ds_umts_bearer_flow_manager_convert_ext_nw_bit_rate(
                                              nw_flow->gbr_ul_ex);
    }

    result = TRUE;
  }/* nw_flow->length == 11 */
#endif
  return result; 
} /* ds_umts_bearer_flow_manager_convert_nw_flow_to_app_flow */


/*===========================================================================
FUNCTION  DS_UMTS_BEARER_FLOW_MANAGER_CONVERT_NW_DATA_RATE_TO_APP
DESCRIPTION
  Converts the UMTS NW passed data rate to App data rate.

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
boolean ds_umts_bearer_flow_manager_convert_nw_data_rate_to_app
(
  ds_bearer_flow_manager_s *bflow_manager_p,
  void           *nw_qos,
  qos_spec_type  *app_qos,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
)
{
  qos_T *nw_alloc_qos;

  *cause_code_p = DS_3GPP_INTERNAL_QOS_NOT_ACCEPTED;

  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    return FALSE;
  }

  nw_alloc_qos = &(((cm_call_mode_info_s_type *) nw_qos)->info.gw_ps_call.qos);

  if((nw_alloc_qos->max_bit_rare_downlink == QOS_BIT_RATE_SUBSCRIBED)&&
     (nw_alloc_qos->max_bit_rare_uplink == QOS_BIT_RATE_SUBSCRIBED)&&
     (((cm_call_mode_info_s_type *)nw_qos)->info.gw_ps_call.tft.valid))
  {
    *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
     DS_3GPP_MSG0_ERROR("TFT requested with R97/98 QoS, rejecting!!!");
     return FALSE;
  }
  if (ds_3gpp_bearer_flow_manager_convert_nw_qos_to_app(bflow_manager_p,
          nw_alloc_qos,app_qos,cause_code_p) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Cannot convert nw qos to app");
    return FALSE;
  }
  memscpy(&bflow_manager_p->ds_bearer_flow_manager_dyn_p->neg_qos,
 	   sizeof(qos_T),
           nw_alloc_qos, sizeof(qos_T));

  return TRUE;
}/* ds_umts_bearer_flow_manager_convert_nw_data_rate_to_app*/

/*===========================================================================
FUNCTION DS_UMTS_BEARER_FLOW_MANAGER_CONVERT_APP_BIT_RATE_TO_NW

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
void ds_umts_bearer_flow_manager_convert_app_bit_rate_to_nw
(
  uint32 app_rate,
  byte   *nw_rate,
  byte   *nw_rate_ex
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#if 0
  // initialize the extend byte to zero
  *nw_rate_ex = 0;
  // convert app rate to kbps
  app_rate /= 1000;

  if(app_rate == 0)
  {
    DS_3GPP_MSG0_ERROR("zero rate passed");
	  *nw_rate = DS_EPS_0KB_BITRATE_VALUE;
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
  else if (app_rate <= DS_EPS_2MB_BITRATE_MAXIMUM_VALUE)
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
  DS_3GPP_MSG2_HIGH("Rate returned to NW is 0x%x, ext_rate:0x%x",*nw_rate,*nw_rate_ex);
#endif
} /* ds_umts_bearer_flow_manager_convert_app_bit_rate_to_nw */

/*===========================================================================
FUNCTION DS_UMTS_BEARER_FLOW_MANAGER_FILL_MIN_QOS_R9798_PARAMS_FROM_R99_PARAMS

DESCRIPTION
  This function fills up the Minumum Release 97/98 specific QOS parameters in
  the origination or modification parameters based on the R99 Qos parameters.

PARAMETERS
  *app_min_qos_parms      - Ptr to minimum QOS parameters for the current call
  *ota_encoded_min_qos_params - Ptr to QOS parameters to be filled for the current 
                                    call

DEPENDENCIES
  For origination, assumes that the R99 parameters have been filled in already 
  in the orig_params_ptr

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void ds_umts_bearer_flow_manager_fill_min_qos_r9798_params_from_r99_params
(
  const ds_umts_umts_qos_params_type *app_min_qos_params,
  qos_T                              *ota_encoded_min_qos_params
)
{
  uint32  max_bit_rate;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    R97/R98 parameters
    Deduce R97/98 params from R99 parameters based on mapping rules defined
    in TS23.107 Section 9.1.2.3and fill in

    Reliability class:
    2: sdu err <= 10e-5
    3: sdu err 10e-5 < x <= 5*10e-4
    4: sdu err > 5*10e-4  and res ber <= 2*10e-4
    5: sdu err > 5*10e-4  and res ber > 2*10e-4

   Note: Subscribed ( sdu err & res BER ) is the value 0x0. account for it
   in the comparisions
  -------------------------------------------------------------------------*/
  if( app_min_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_SUBSCRIBE )
  {
    ota_encoded_min_qos_params->reliability_class=
                                            (uint8)QOS_REL_CLASS_SUBSCRIBED;
  }
  else if( (app_min_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_1E5 ) ||
           (app_min_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_1E6 ) )
  {
    ota_encoded_min_qos_params->reliability_class  = QOS_REL_CLASS_2;
  }
  else if (app_min_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_1E4 )
  {
    ota_encoded_min_qos_params->reliability_class  = QOS_REL_CLASS_3;
  }
  else if ( ( (app_min_qos_params->sdu_err < DS_UMTS_QOS_SDU_ERR_1E4 ) ||
              (app_min_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_1E1) ) &&
            ( app_min_qos_params->res_biterr >= DS_UMTS_QOS_RES_BER_1E4 ) )
  {
    ota_encoded_min_qos_params->reliability_class  = QOS_REL_CLASS_4;

  }
  else if (((app_min_qos_params->sdu_err < DS_UMTS_QOS_SDU_ERR_1E4 )||
            (app_min_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_1E1) ) &&
           ((app_min_qos_params->res_biterr < DS_UMTS_QOS_RES_BER_1E4 ) &&
            (app_min_qos_params->res_biterr != DS_UMTS_QOS_RES_BER_SUBSCRIBE)))
  {
    ota_encoded_min_qos_params->reliability_class  = QOS_REL_CLASS_5;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Set to subscribed
    -----------------------------------------------------------------------*/
    ota_encoded_min_qos_params->reliability_class= QOS_REL_CLASS_SUBSCRIBED;
  }

  /*-------------------------------------------------------------------------
    Delay Class :
    1: Traffic class = conversational or streaming
    1: Traffic class = interactive and Traffic handling pri =1
    2: Traffic class = interactive and Traffic handling pri =2
    3: Traffic class = interactive and Traffic handling pri =3
    4: Traffic class = background
  -------------------------------------------------------------------------*/
  if( app_min_qos_params->traffic_class == DS_UMTS_QOS_TCLASS_SUBSCRIBE )
  {
    ota_encoded_min_qos_params->delay_class = QOS_DELAY_CLASS_SUBSCRIBED;
  }
  else if( app_min_qos_params->traffic_class <= DS_UMTS_QOS_TCLASS_STRM )
  {
    ota_encoded_min_qos_params->delay_class = QOS_DELAY_CLASS_1;
  }
  else if ( (app_min_qos_params->traffic_class == DS_UMTS_QOS_TCLASS_INTR ) &&
            (app_min_qos_params->thandle_prio == 0x1) )
  {
    ota_encoded_min_qos_params->delay_class = QOS_DELAY_CLASS_1;
  }
  else if ( (app_min_qos_params->traffic_class == DS_UMTS_QOS_TCLASS_INTR ) &&
            (app_min_qos_params->thandle_prio == 0x2) )
  {
    ota_encoded_min_qos_params->delay_class = QOS_DELAY_CLASS_2;
  }
  else if ( (app_min_qos_params->traffic_class == DS_UMTS_QOS_TCLASS_INTR ) &&
            (app_min_qos_params->thandle_prio == 0x3) )
  {
    ota_encoded_min_qos_params->delay_class = QOS_DELAY_CLASS_3;
  }
  else if( app_min_qos_params->traffic_class == DS_UMTS_QOS_TCLASS_BACK )
  {
    ota_encoded_min_qos_params->delay_class = QOS_DELAY_CLASS_4;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Set to subscribed
    -----------------------------------------------------------------------*/
    ota_encoded_min_qos_params->delay_class  = QOS_DELAY_CLASS_SUBSCRIBED;
  }

  /*-------------------------------------------------------------------------
    precedence Class :  always set to subscribed (0x0)
  -------------------------------------------------------------------------*/
  ota_encoded_min_qos_params->precedence_class= QOS_PRECED_CLASS_SUBSCRIBED;

  /*-------------------------------------------------------------------------
    Peak Throughput : In case of asymetric bearers the higher of dl/ul
    is selected and used as the max bit rate
    1Kbytes  : max bit rate < 16            (kbits/sec)
    2Kbytes  : max bit rate 16 <= x < 32    (kbits/sec)
    4Kbytes  : max bit rate 32 <= x < 64    (kbits/sec)
    8Kbytes  : max bit rate 64 <= x < 128   (kbits/sec)
    16Kbytes : max bit rate 128 <= x < 256  (kbits/sec)
    32Kbytes : max bit rate 256 <= x < 512  (kbits/sec)
    64Kbytes : max bit rate 512 <= x < 1024 (kbits/sec)
    128Kbytes: max bit rate 1024 <= x <2048 (kbits/sec)
    256Kbytes: max bit rate >= 2048         (kbits/sec)
  Note : max bit rate (ul/dl) specified as 0xff indicates '0' kbits/sec
  -------------------------------------------------------------------------*/
  if ( (ota_encoded_min_qos_params->max_bit_rare_uplink >
        ota_encoded_min_qos_params->max_bit_rare_downlink )  &&
       (ota_encoded_min_qos_params->max_bit_rare_uplink !=
          QOS_BIT_RATE_0KBPS )
     )
  {
    max_bit_rate = ota_encoded_min_qos_params->max_bit_rare_uplink;
  }
  else
  {
    max_bit_rate = ota_encoded_min_qos_params->max_bit_rare_downlink;
  }

  /*-------------------------------------------------------------------------
    Start encoding Peak throughput
  -------------------------------------------------------------------------*/
  if ( max_bit_rate ==  QOS_BIT_RATE_0KBPS ||
       max_bit_rate ==  QOS_BIT_RATE_SUBSCRIBED )
  {
   ota_encoded_min_qos_params->peak_throughput = QOS_PEAK_TPUT_SUBSCRIBED;
  }
  else if( max_bit_rate < QOS_BIT_RATE_16KBPS )
  {
   ota_encoded_min_qos_params->peak_throughput = QOS_PEAK_TPUT_1KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_32KBPS )
  {
    ota_encoded_min_qos_params->peak_throughput = QOS_PEAK_TPUT_2KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_64KBPS )
  {
    ota_encoded_min_qos_params->peak_throughput = QOS_PEAK_TPUT_4KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_128KBPS )
  {
    ota_encoded_min_qos_params->peak_throughput = QOS_PEAK_TPUT_8KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_256KBPS )
  {
    ota_encoded_min_qos_params->peak_throughput = QOS_PEAK_TPUT_16KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_512KBPS )
  {
    ota_encoded_min_qos_params->peak_throughput = QOS_PEAK_TPUT_32BYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_1024KBPS )
  {
    ota_encoded_min_qos_params->peak_throughput = QOS_PEAK_TPUT_64KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_2048KBPS )
  {
    ota_encoded_min_qos_params->peak_throughput = QOS_PEAK_TPUT_128KBYTES;
  }
  else  
  {
    ota_encoded_min_qos_params->peak_throughput = QOS_PEAK_TPUT_256KBYTES;
  }

  /*-------------------------------------------------------------------------
    Mean throughput : always set to best effort (0x1f)
  -------------------------------------------------------------------------*/
  ota_encoded_min_qos_params->mean_throughput   = QOS_MEAN_TPUT_BEST_EFFORT;
  return;

} /* dsumtsps_fill_min_qos_r9798_params_from_r99_params */

/*===========================================================================
FUNCTION DS_UMTS_BEARER_FLOW_MANAGER_FILL_SEC_CONTEXT_R99_MIN_QOS_PARAMS

DESCRIPTION
  This function fills up Minimum QOS parameters in the
  origination and modification parameters based on the application specified QOS.
  Only Release 99 specific QOS parameters are filled in ( octet 6 to 13)


PARAMETERS
  *app_min_qos_params    - Ptr to min QOS parameters for the current call
  *ota_encoded_min_qos_params  - Ptr to Call origination or modififcation QOS 
                         parameters

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void ds_umts_bearer_flow_manager_fill_sec_context_r99_min_qos_params
(
    const ds_umts_umts_qos_params_type *app_min_qos_params,
    qos_T                              *ota_encoded_min_qos_params
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Traffic class : fill as-is : matches coding rules
  -------------------------------------------------------------------------*/
  ota_encoded_min_qos_params->traffic_class = app_min_qos_params->traffic_class;

  /*-------------------------------------------------------------------------
    Max SDU Size : Set to Subscribed
  -------------------------------------------------------------------------*/
  ota_encoded_min_qos_params->max_sdu_size = QOS_MAX_SDU_SUBSCRIBED;

  /*-------------------------------------------------------------------------
    Downlink Max bit rates
  -------------------------------------------------------------------------*/
  if (app_min_qos_params->max_dl_bitrate > 8640)
  {
    ota_encoded_min_qos_params->max_bit_rare_downlink= QOS_BIT_RATE_8640KBPS;

    ota_encoded_min_qos_params->ext_max_bit_rate_downlink=
#ifdef FEATURE_NAS_REL7
      dsUmtsBearerCntxt_convert_data_rate_to_r7_ext_encode_val(
#else 
      dsUmtsBearerCntxt_convert_data_rate_to_r5_ext_encode_val(      
#endif /* FEATURE_NAS_REL7 */
        app_min_qos_params->max_dl_bitrate);
  }
  else
  {
    ota_encoded_min_qos_params->max_bit_rare_downlink=
      dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(
                                        app_min_qos_params->max_dl_bitrate);

    ota_encoded_min_qos_params->ext_max_bit_rate_downlink= QOS_BIT_RATE_SUBSCRIBED;
  }

  /*-------------------------------------------------------------------------
    Downlink Guaranteed bit rates
  -------------------------------------------------------------------------*/
  if (app_min_qos_params->gtd_dl_bitrate > 8640)
  {
    ota_encoded_min_qos_params->guarantee_ber_dwnlink= QOS_BIT_RATE_8640KBPS;

    ota_encoded_min_qos_params->ext_gtd_bit_rate_downlink=
#ifdef FEATURE_NAS_REL7
      dsUmtsBearerCntxt_convert_data_rate_to_r7_ext_encode_val(
#else 
      dsUmtsBearerCntxt_convert_data_rate_to_r5_ext_encode_val(      
#endif /* FEATURE_NAS_REL7 */                                          
        app_min_qos_params->gtd_dl_bitrate);
  }
  else
  {
    ota_encoded_min_qos_params->guarantee_ber_dwnlink=
      dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(
                                           app_min_qos_params->gtd_dl_bitrate);

    ota_encoded_min_qos_params->ext_gtd_bit_rate_downlink= QOS_BIT_RATE_SUBSCRIBED;
  }

  DATA_MSG0_HIGH("Secondary Activation: Encoding Min QOS: Max and Gtd UL Data Rate");
#ifdef FEATURE_NAS_REL7
  /*-------------------------------------------------------------------------
    Uplink Max bit rates
  -------------------------------------------------------------------------*/
  if (app_min_qos_params->max_ul_bitrate > 8640)
  {
    ota_encoded_min_qos_params->max_bit_rare_uplink= QOS_BIT_RATE_8640KBPS;

    ota_encoded_min_qos_params->ext_max_bit_rate_uplink=
      dsUmtsBearerCntxt_convert_data_rate_to_r7_ext_encode_val(      
                                        app_min_qos_params->max_ul_bitrate);
  }
  else
  {
    ota_encoded_min_qos_params->max_bit_rare_uplink  =
      dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(
        app_min_qos_params->max_ul_bitrate);

    ota_encoded_min_qos_params->ext_max_bit_rate_uplink= QOS_BIT_RATE_SUBSCRIBED;
  }

  /*-------------------------------------------------------------------------
    Uplink Guaranteed bit rates
  -------------------------------------------------------------------------*/
  if (app_min_qos_params->gtd_ul_bitrate > 8640)
  {
    ota_encoded_min_qos_params->guarantee_ber_uplink= QOS_BIT_RATE_8640KBPS;

    ota_encoded_min_qos_params->ext_gtd_bit_rate_downlink=
      dsUmtsBearerCntxt_convert_data_rate_to_r7_ext_encode_val(      
        app_min_qos_params->gtd_ul_bitrate);
  }
  else
  {
    ota_encoded_min_qos_params->guarantee_ber_uplink =
      dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(
        app_min_qos_params->gtd_ul_bitrate);

    ota_encoded_min_qos_params->ext_gtd_bit_rate_uplink= QOS_BIT_RATE_SUBSCRIBED;
  }
#else 
  /*-------------------------------------------------------------------------
    Uplink Max and Guaranteed bit rates
  -------------------------------------------------------------------------*/
  ota_encoded_min_qos_params->max_bit_rare_uplink  =
    dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(
                                             app_min_qos_params->max_ul_bitrate);

  ota_encoded_min_qos_params->guarantee_ber_uplink =
    dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(
                                            app_min_qos_params->gtd_ul_bitrate);
#endif /* FEATURE_NAS_REL7 */

  /*-------------------------------------------------------------------------
    Sdu error ratio : fill in as-is
  -------------------------------------------------------------------------*/
  ota_encoded_min_qos_params->sdu_err_ratio = app_min_qos_params->sdu_err;

  /*-------------------------------------------------------------------------
    Res BER : fill in as-is
  -------------------------------------------------------------------------*/
  ota_encoded_min_qos_params->residual_ber = app_min_qos_params->res_biterr;

  /*-------------------------------------------------------------------------
    Traffic Handling priority : fill in as-is
  -------------------------------------------------------------------------*/
  ota_encoded_min_qos_params->traffic_priority =
                                         (uint8)app_min_qos_params->thandle_prio;

  /*-------------------------------------------------------------------------
    Transfer Delay : Set to Subscribed
  -------------------------------------------------------------------------*/
  ota_encoded_min_qos_params->tranfer_delay = QOS_TXFER_DELAY_SUBSCRIBED;

  /*-------------------------------------------------------------------------
    Signaling Indication : Set to app requested value
  -------------------------------------------------------------------------*/
  ota_encoded_min_qos_params->sig_ind = app_min_qos_params->sig_ind ;

  return;

} /* dsumtsps_fill_sec_context_r99_min_qos_params() */

/*===========================================================================
FUNCTION DS_UMTS_BEARER_FLOW_MANAGER_FILL_QOS_R9798_PARAMS_FROM_R99_PARAMS

DESCRIPTION
  This function fills up the Release 97/98 specific QOS parameters in the
  origination and modification parameters based on the R99 Qos parameters.
  Octet 3 to Octet 5 of the QOS parameters in PDP context req are
  R97/98 qos parameters.

PARAMETERS
  *app_req_qos_params - Ptr to requested QoS parameters for the current call
  *ota_encoded_qos_params - Ptr to QOS parameters to be filled for the 
                                current call

DEPENDENCIES
  For origination, assumes that the R99 parameters have been filled in 
  already in the orig_params_ptr

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void ds_umts_bearer_flow_manager_fill_qos_r9798_params_from_r99_params
(
  const ds_umts_umts_qos_params_type *app_req_qos_params,
  qos_T                              *ota_encoded_qos_params
)
{
  uint32  max_bit_rate;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    R97/R98 parameters (octet 3 to octet 5)
    Deduce R97/98 params from R99 parameters based on mapping rules defined
    in TS23.107 Section 9.1.2.3and fill in

    Reliability class: Octet 3  :
    2: sdu err <= 10e-5
    3: sdu err 10e-5 < x <= 5*10e-4
    4: sdu err > 5*10e-4  and res ber <= 2*10e-4
    5: sdu err > 5*10e-4  and res ber > 2*10e-4

   Note: Subscribed ( sdu err & res BER ) is the value 0x0. account for it
   in the comparisions
  -------------------------------------------------------------------------*/
  if( app_req_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_SUBSCRIBE )
  {
    ota_encoded_qos_params->reliability_class  = QOS_REL_CLASS_SUBSCRIBED;
  }
  else if( (app_req_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_1E5 ) ||
           (app_req_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_1E6 ) )
  {
    ota_encoded_qos_params->reliability_class  = QOS_REL_CLASS_2;
  }
  else if (app_req_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_1E4 )
  {
    ota_encoded_qos_params->reliability_class  = QOS_REL_CLASS_3;
  }
  else if ( ( (app_req_qos_params->sdu_err < DS_UMTS_QOS_SDU_ERR_1E4 ) ||
              (app_req_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_1E1) ) &&
              (app_req_qos_params->res_biterr >= DS_UMTS_QOS_RES_BER_1E4 ) )
  {
    ota_encoded_qos_params->reliability_class  = QOS_REL_CLASS_4;

  }
  else if ( ( (app_req_qos_params->sdu_err < DS_UMTS_QOS_SDU_ERR_1E4 )||
              (app_req_qos_params->sdu_err == DS_UMTS_QOS_SDU_ERR_1E1) ) &&
            ( (app_req_qos_params->res_biterr < DS_UMTS_QOS_RES_BER_1E4 ) &&
              (app_req_qos_params->res_biterr != DS_UMTS_QOS_RES_BER_SUBSCRIBE ) ) )
  {
    ota_encoded_qos_params->reliability_class  = QOS_REL_CLASS_5;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Set to subscribed
    -----------------------------------------------------------------------*/
    ota_encoded_qos_params->reliability_class  = QOS_REL_CLASS_SUBSCRIBED;
  }

  /*-------------------------------------------------------------------------
    Delay Class :
    1: Traffic class = conversational or streaming
    1: Traffic class = interactive and Traffic handling pri =1
    2: Traffic class = interactive and Traffic handling pri =2
    3: Traffic class = interactive and Traffic handling pri =3
    4: Traffic class = background
  -------------------------------------------------------------------------*/
  if( app_req_qos_params->traffic_class == DS_UMTS_QOS_TCLASS_SUBSCRIBE )
  {
    ota_encoded_qos_params->delay_class = QOS_DELAY_CLASS_SUBSCRIBED;
  }
  else if( app_req_qos_params->traffic_class <= DS_UMTS_QOS_TCLASS_STRM )
  {
    ota_encoded_qos_params->delay_class = QOS_DELAY_CLASS_1;
  }
  else if ( (app_req_qos_params->traffic_class == DS_UMTS_QOS_TCLASS_INTR ) &&
            (app_req_qos_params->thandle_prio == 0x1) )
  {
    ota_encoded_qos_params->delay_class = QOS_DELAY_CLASS_1;
  }
  else if ( (app_req_qos_params->traffic_class == DS_UMTS_QOS_TCLASS_INTR ) &&
            (app_req_qos_params->thandle_prio == 0x2) )
  {
    ota_encoded_qos_params->delay_class = QOS_DELAY_CLASS_2;
  }
  else if ( (app_req_qos_params->traffic_class == DS_UMTS_QOS_TCLASS_INTR ) &&
            (app_req_qos_params->thandle_prio == 0x3) )
  {
    ota_encoded_qos_params->delay_class = QOS_DELAY_CLASS_3;
  }
  else if( app_req_qos_params->traffic_class == DS_UMTS_QOS_TCLASS_BACK )
  {
    ota_encoded_qos_params->delay_class = QOS_DELAY_CLASS_4;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Set to subscribed
    -----------------------------------------------------------------------*/
    ota_encoded_qos_params->delay_class  = QOS_DELAY_CLASS_SUBSCRIBED;
  }

  /*-------------------------------------------------------------------------
    precedence Class :  always set to subscribed (0x0)
  -------------------------------------------------------------------------*/
  ota_encoded_qos_params->precedence_class = QOS_PRECED_CLASS_SUBSCRIBED;

  /*-------------------------------------------------------------------------
    Peak Throughput : In case of asymetric bearers the higher of dl/ul
    is selected and used as the max bit rate
    1Kbytes  : max bit rate < 16            (kbits/sec)
    2Kbytes  : max bit rate 16 <= x < 32    (kbits/sec)
    4Kbytes  : max bit rate 32 <= x < 64    (kbits/sec)
    8Kbytes  : max bit rate 64 <= x < 128   (kbits/sec)
    16Kbytes : max bit rate 128 <= x < 256  (kbits/sec)
    32Kbytes : max bit rate 256 <= x < 512  (kbits/sec)
    64Kbytes : max bit rate 512 <= x < 1024 (kbits/sec)
    128Kbytes: max bit rate 1024 <= x <2048 (kbits/sec)
    256Kbytes: max bit rate >= 2048         (kbits/sec)
  Note : max bit rate (ul/dl) specified as 0xff indicates '0' kbits/sec
  -------------------------------------------------------------------------*/
  if ( (ota_encoded_qos_params->max_bit_rare_uplink >
        ota_encoded_qos_params->max_bit_rare_downlink )  &&
       (ota_encoded_qos_params->max_bit_rare_uplink !=
          QOS_BIT_RATE_0KBPS )
     )
  {
    max_bit_rate = ota_encoded_qos_params->max_bit_rare_uplink;
  }
  else
  {
    max_bit_rate = ota_encoded_qos_params->max_bit_rare_downlink;
  }

  /*-------------------------------------------------------------------------
    Start encoding Peak throughput
  -------------------------------------------------------------------------*/
  if ( max_bit_rate ==  QOS_BIT_RATE_0KBPS ||
       max_bit_rate ==  QOS_BIT_RATE_SUBSCRIBED )
  {
   ota_encoded_qos_params->peak_throughput = QOS_PEAK_TPUT_SUBSCRIBED;
  }
  else if( max_bit_rate < QOS_BIT_RATE_16KBPS )
  {
   ota_encoded_qos_params->peak_throughput = QOS_PEAK_TPUT_1KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_32KBPS )
  {
    ota_encoded_qos_params->peak_throughput = QOS_PEAK_TPUT_2KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_64KBPS )
  {
    ota_encoded_qos_params->peak_throughput = QOS_PEAK_TPUT_4KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_128KBPS )
  {
    ota_encoded_qos_params->peak_throughput = QOS_PEAK_TPUT_8KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_256KBPS )
  {
    ota_encoded_qos_params->peak_throughput = QOS_PEAK_TPUT_16KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_512KBPS )
  {
    ota_encoded_qos_params->peak_throughput = QOS_PEAK_TPUT_32BYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_1024KBPS )
  {
    ota_encoded_qos_params->peak_throughput = QOS_PEAK_TPUT_64KBYTES;
  }
  else if( max_bit_rate < QOS_BIT_RATE_2048KBPS )
  {
    ota_encoded_qos_params->peak_throughput = QOS_PEAK_TPUT_128KBYTES;
  }
  else  
  {
    ota_encoded_qos_params->peak_throughput = QOS_PEAK_TPUT_256KBYTES;
  }

  /*-------------------------------------------------------------------------
    Mean throughput : always set to best effort (0x1f)
  -------------------------------------------------------------------------*/
  ota_encoded_qos_params->mean_throughput      = QOS_MEAN_TPUT_BEST_EFFORT;
  return;

} /* dsumtsps_fill_qos_r9798_params_from_r99_params */

/*===========================================================================
FUNCTION DS_UMTS_BEARER_FLOW_MANAGER_FILL_SEC_CONTEXT_R99_QOS_PARAMS

DESCRIPTION
  This function fills up QOS parameters in the origination parameters based
  on the converted QoS specified by the application. Only Release
  99 specific QOS parameters are filled in ( octet 6 to 13).

PARAMETERS
  *app_req_qos_params    - pointer to application given (conv to UMTS) requested qos
  *ota_encoded_qos_params  - pointer to the orination parameters

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
LOCAL void ds_umts_bearer_flow_manager_fill_sec_context_r99_qos_params
(
    const ds_umts_umts_qos_params_type *app_req_qos_params,
    qos_T                              *ota_encoded_qos_params
)
{
  uint32 max_bitrate;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Traffic class : fill as-is : matches coding rules
  -------------------------------------------------------------------------*/
  ota_encoded_qos_params->traffic_class = app_req_qos_params->traffic_class;

  /*-------------------------------------------------------------------------
    Set Max SDU Size to subscribed
  -------------------------------------------------------------------------*/
  ota_encoded_qos_params->max_sdu_size = QOS_MAX_SDU_SUBSCRIBED;

  /*-------------------------------------------------------------------------
    Downlink Max bit rates
  -------------------------------------------------------------------------*/
  if (app_req_qos_params->max_dl_bitrate > 8640)
  {
    ota_encoded_qos_params->max_bit_rare_downlink= QOS_BIT_RATE_8640KBPS;

    ota_encoded_qos_params->ext_max_bit_rate_downlink=
#ifdef FEATURE_NAS_REL7
      dsUmtsBearerCntxt_convert_data_rate_to_r7_ext_encode_val(
#else 
      dsUmtsBearerCntxt_convert_data_rate_to_r5_ext_encode_val(
#endif /* FEATURE_NAS_REL7 */
                                        app_req_qos_params->max_dl_bitrate);
  }
  else
  {
#ifdef FEATURE_DATA_RATE_MAPPING
    max_bitrate = dsUmtsBearerCntxt_map_max_bit_rate_to_allowed_rate(
                                        app_req_qos_params->max_dl_bitrate);
#else
    max_bitrate = app_req_qos_params->max_dl_bitrate;
#endif

    ota_encoded_qos_params->max_bit_rare_downlink=
      dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(max_bitrate);

    ota_encoded_qos_params->ext_max_bit_rate_downlink= QOS_BIT_RATE_SUBSCRIBED;
  }

  /*-------------------------------------------------------------------------
    Downlink Guaranteed bit rates
  -------------------------------------------------------------------------*/
  DATA_MSG0_HIGH("Secondary Activation: Encoding Max and Gtd DL Data Rate");

  if (app_req_qos_params->gtd_dl_bitrate > 8640)
  {
    ota_encoded_qos_params->guarantee_ber_dwnlink= QOS_BIT_RATE_8640KBPS;

    ota_encoded_qos_params->ext_gtd_bit_rate_downlink=
#ifdef FEATURE_NAS_REL7
      dsUmtsBearerCntxt_convert_data_rate_to_r7_ext_encode_val(
#else 
      dsUmtsBearerCntxt_convert_data_rate_to_r5_ext_encode_val(
#endif /* FEATURE_NAS_REL7 */
                                            app_req_qos_params->gtd_dl_bitrate);
  }
  else
  {
    ota_encoded_qos_params->guarantee_ber_dwnlink=
      dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(
                                           app_req_qos_params->gtd_dl_bitrate);

    ota_encoded_qos_params->ext_gtd_bit_rate_downlink= QOS_BIT_RATE_SUBSCRIBED;
  }

  DATA_MSG0_HIGH("Secondary Activation: Encoding Max and Gtd UL Data Rate");

#ifdef FEATURE_NAS_REL7
  /*-------------------------------------------------------------------------
    Uplink Max bit rates
  -------------------------------------------------------------------------*/
  if (app_req_qos_params->max_ul_bitrate > 8640)
  {
    ota_encoded_qos_params->max_bit_rare_uplink= QOS_BIT_RATE_8640KBPS;

    ota_encoded_qos_params->ext_max_bit_rate_uplink=
      dsUmtsBearerCntxt_convert_data_rate_to_r7_ext_encode_val(
                                        app_req_qos_params->max_ul_bitrate);
  }
  else
  {
#ifdef FEATURE_DATA_RATE_MAPPING
    max_bitrate = dsUmtsBearerCntxt_map_max_bit_rate_to_allowed_rate(
                                             app_req_qos_params->max_ul_bitrate);
#else
    max_bitrate = app_req_qos_params->max_ul_bitrate;
#endif

    ota_encoded_qos_params->max_bit_rare_uplink  =
      dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(max_bitrate);

    ota_encoded_qos_params->ext_max_bit_rate_uplink= QOS_BIT_RATE_SUBSCRIBED;
  }

  /*-------------------------------------------------------------------------
    Uplink Guaranteed bit rates
  -------------------------------------------------------------------------*/
  if (app_req_qos_params->gtd_ul_bitrate > 8640)
  {
    ota_encoded_qos_params->guarantee_ber_uplink= QOS_BIT_RATE_8640KBPS;

    ota_encoded_qos_params->ext_gtd_bit_rate_uplink=
      dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(
                                  app_req_qos_params->gtd_ul_bitrate);
  }
  else
  {
    ota_encoded_qos_params->guarantee_ber_uplink =
      dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(
        app_req_qos_params->gtd_ul_bitrate);

    ota_encoded_qos_params->ext_gtd_bit_rate_uplink= QOS_BIT_RATE_SUBSCRIBED;
  }

#else
  /*-------------------------------------------------------------------------
    Uplink Max and Guaranteed bit rates
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_RATE_MAPPING
  max_bitrate = dsUmtsBearerCntxt_map_max_bit_rate_to_allowed_rate(
                                             app_req_qos_params->max_ul_bitrate);
#else
  max_bitrate = app_req_qos_params->max_ul_bitrate;
#endif

  ota_encoded_qos_params->max_bit_rare_uplink  =
    dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(max_bitrate);

  ota_encoded_qos_params->guarantee_ber_uplink =
    dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val(
                                            app_req_qos_params->gtd_ul_bitrate);
#endif /* FEATURE_NAS_REL7 */
  /*-------------------------------------------------------------------------
    Sdu error ratio : fill in as-is
  -------------------------------------------------------------------------*/
  ota_encoded_qos_params->sdu_err_ratio = app_req_qos_params->sdu_err;

  /*-------------------------------------------------------------------------
    Res BER : fill in as-is
  -------------------------------------------------------------------------*/
  ota_encoded_qos_params->residual_ber = app_req_qos_params->res_biterr;

  /*-------------------------------------------------------------------------
    Traffic Handling priority : fill in as-is
  -------------------------------------------------------------------------*/
  ota_encoded_qos_params->traffic_priority = (uint8)app_req_qos_params->thandle_prio;

  /*-------------------------------------------------------------------------
    Transfer delay : Set to subscribed
  -------------------------------------------------------------------------*/
  ota_encoded_qos_params->tranfer_delay = QOS_TXFER_DELAY_SUBSCRIBED;

  /*-------------------------------------------------------------------------
    Signaling Indication : Set to app requested value
  -------------------------------------------------------------------------*/
  ota_encoded_qos_params->sig_ind = app_req_qos_params->sig_ind ;

  return;
}



/*===========================================================================
FUNCTION DS_UMTS_BEARER_FLOW_MANAGER_CONVERT_APP_DATA_RATE_TO_NW
DESCRIPTION
  Converts the APP passed data rate to UMTS data rate. 
  orig_call_hdlr converts the UMTS values to NW format.
  In UMTS, except data rates and guaranteed data rates, other parameters
  are common across UL and DL. The table below deascribes how we process
  the parameters given by the application

  Required UL Data Rate            : Taken from TX Required set
  Required UL Guaranteed Data Rate : Taken from TX Required set
  Minimum UL Data Rate             : Taken from TX Minimum set
  Minimum UL Guaranteed Data Rate  : Taken from TX Minimum set

  Required DL Data Rate            : Taken from RX Required set
  Required DL Guaranteed Data Rate : Taken from RX Required set
  Minimum DL Data Rate             : Taken from RX Minimum set
  Minimum DL Guaranteed Data Rate  : Taken from RX Minimum set

  Required Traffic Class (common)             : Taken from RX Required set
  Required Traffic Handling Priority (common) : Taken from RX Required set
  Required RES BER (common)                   : Taken from RX Required set
  Required SDU Err Ratio (common)             : Taken from RX Required set

  Minimum Traffic Class (common)             : Taken from RX Minimum set
  Minimum Traffic Handling Priority (common) : Taken from RX Minimum set
  Minimum RES BER (common)                   : Taken from RX Minimum set
  Minimum SDU Err Ratio (common)             : Taken from RX Minimum set

  In this function, process each QOS field individually. If the field is
  not set, set it to default (subscribed). The out come of the process
  is two sets of QOS.

PARAMETERS
app_qos         -  Application qos to be converted
bflow_manager_p - UMTS bearer flow manager 

DEPENDENCIES
  None.

RETURN VALUE
boolean         - return DS3G_SUCCESS or DS3G_FAILURE
  
SIDE EFFECTS
  None.
//VRK:TODo avoid APP->UMTS->NW format, instead convert APP->NW directly.
===========================================================================*/
boolean ds_umts_bearer_flow_manager_convert_app_data_rate_to_nw
(
   qos_spec_type   *app_qos,
   void            *bflow_manager_p
)
{  
  ds_umts_umts_qos_params_type  *req_qos = NULL;
  ds_umts_umts_qos_params_type  *min_qos = NULL;
  qos_T                         *req_qos_nw = NULL; 
  qos_T                         *min_qos_nw = NULL; 
  ds_umts_bearer_flow_manager_s *umts_bflow_manager_p;
  uint32                       temp_max_bitrate, temp_gtd_bitrate;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(bflow_manager_p == NULL || app_qos == NULL)
  {
    DS_3GPP_MSG0_ERROR("input parameters are NULL");
    return DS3G_FAILURE;
  } 

  umts_bflow_manager_p =(ds_umts_bearer_flow_manager_s*)bflow_manager_p;

  if(!ds_umts_bearer_flow_manager_validate(umts_bflow_manager_p))
  {

     DS_3GPP_MSG0_ERROR("input parameters are Invalid");
     return DS3G_FAILURE;
  }



  min_qos = &(umts_bflow_manager_p->ds_umts_bearer_flow_manager_dynamic_p->app_sps_min_qos);
  req_qos = &(umts_bflow_manager_p->ds_umts_bearer_flow_manager_dynamic_p->app_sps_req_qos);
  
  /*------------------------------------------------------------------------
    VRK:TODO Need subfunction that can be called for req and min qos
  ------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------
    Set the Required and Minimum sets to defaults
    NOTE: All values are set to 0x0 implying subscribed value.
    ref TS24.008 sec 10.5.6.5
  -----------------------------------------------------------------------*/
  memset((char *)min_qos, 0, sizeof(ds_umts_umts_qos_params_type));
  min_qos->traffic_class        = DS_UMTS_QOS_TCLASS_SUBSCRIBE;  
  min_qos->dlvry_order          = DS_UMTS_QOS_DORDER_SUBSCRIBE;
  min_qos->sdu_err              = DS_UMTS_QOS_SDU_ERR_SUBSCRIBE;
  min_qos->res_biterr           = DS_UMTS_QOS_RES_BER_SUBSCRIBE;
  min_qos->dlvr_err_sdu         = DS_UMTS_QOS_SDU_DLVR_SUBSCRIBE;  

  memset((char *)req_qos, 0, sizeof(ds_umts_umts_qos_params_type));
  req_qos->traffic_class        = DS_UMTS_QOS_TCLASS_SUBSCRIBE;  
  req_qos->dlvry_order          = DS_UMTS_QOS_DORDER_SUBSCRIBE;
  req_qos->sdu_err              = DS_UMTS_QOS_SDU_ERR_SUBSCRIBE;
  req_qos->res_biterr           = DS_UMTS_QOS_RES_BER_SUBSCRIBE;
  req_qos->dlvr_err_sdu         = DS_UMTS_QOS_SDU_DLVR_SUBSCRIBE;  

  /*-------------------------------------------------------------------------
    If either the requested Rx or Tx flow has been specified, set the valid 
    flag to TRUE.
  -------------------------------------------------------------------------*/
  if ((app_qos->field_mask & (int)QOS_MASK_RX_FLOW) ||
      (app_qos->field_mask & (int)QOS_MASK_TX_FLOW))
  {
    req_qos->valid_flg = TRUE;
  }
  else
  {
    req_qos->valid_flg = FALSE;
  }

  /*-----------------------------------------------------------------------
    DL Required QOS parameters have been specified by the application
    Convert the DL Data Rate, DL GTD Data Rate, Traffic class, Traffic
    handling priority, Residual Bit Error Rate, SDU Error Ratio
    -----------------------------------------------------------------------*/
  if ( app_qos->field_mask & (int)QOS_MASK_RX_FLOW )
  {
    DS_3GPP_MSG0_HIGH("Secondary activation : Processing RX Req QOS Flow Spec");
    /*-----------------------------------------------------------------------
       Convert the Traffic Class parameter of required set into UMTS specific.
    -----------------------------------------------------------------------*/
    dsumtsps_process_traffic_class (
                                app_qos->rx.flow_template.req_flow.trf_class,
                                app_qos->rx.flow_template.req_flow.field_mask,
                                req_qos );

    /*-----------------------------------------------------------------------
       Convert the Data Rate parameter of required set into UMTS specific:
       DL Max Data Rate, DL Guartanteed Data Rate
    -----------------------------------------------------------------------*/
    dsumtsps_process_data_rate(
                                &app_qos->rx.flow_template.req_flow,
                                app_qos->rx.flow_template.req_flow.latency,
                                app_qos->rx.flow_template.req_flow.field_mask,
                                &temp_max_bitrate,
                                &temp_gtd_bitrate );
    req_qos->max_dl_bitrate =  temp_max_bitrate;
    req_qos->gtd_dl_bitrate =  temp_gtd_bitrate;

    /*-----------------------------------------------------------------------
       Convert the Packet Error Rate parameter of required set into SDU Error.
    -----------------------------------------------------------------------*/
    dsumtsps_process_pkt_err_rate(
                                &app_qos->rx.flow_template.req_flow,
                                app_qos->rx.flow_template.req_flow.field_mask,
                                req_qos);

    /*-----------------------------------------------------------------------
       Convert the Residual Bit Error Rate of required set into UMTS specific.
    -----------------------------------------------------------------------*/
    dsumtsps_process_residual_ber(
                                &app_qos->rx.flow_template.req_flow,
                                app_qos->rx.flow_template.req_flow.field_mask,
                                req_qos);

    /*-----------------------------------------------------------------------
       Convert the Traffic Handling Priority parameter of required set

       From 24.008:
       The Traffic handling priority value is ignored if the Traffic Class is 
       Conversational class, Streaming class or Background class.
    -----------------------------------------------------------------------*/
    dsumtsps_process_trf_hdl_prio(
                                &app_qos->rx.flow_template.req_flow,
                                app_qos->rx.flow_template.req_flow.field_mask,
                                req_qos);

	/*-------------------------------------------------------------------------
	  Fill the signalling Indication bit if the traffic class is interactive.
	  and the Traffic Handling priority is 1.

	  Sig Ind Flag should only be set if the traffic class is interactive
	  and the traffic handling priority is 1 (This is as per spec 
	  23.107 Sec 6.4.3.1).

	  Signalling Indication (Yes/No)
	  Definition: Indicates the signalling nature of the submitted SDUs.
	  This attribute is additional to the other QoS attributes and does
	  not over-ride them. This attribute is only defined for the
	  interactive traffic class. If signalling indication is set to "Yes",
	  the UE should set the traffic handling priority to "1".
	-------------------------------------------------------------------------*/
	dsumtsps_process_sig_ind_prio(&app_qos->rx.flow_template.req_flow,
                              app_qos->rx.flow_template.req_flow.field_mask,
                              req_qos);
  }

  /*-----------------------------------------------------------------------
    UL Required QOS parameters have been specified by the application
    Convert the UL Data Rate, UL GTD Data Rate
  -----------------------------------------------------------------------*/
  if ( app_qos->field_mask & (int)QOS_MASK_TX_FLOW )
  {
    DS_3GPP_MSG0_HIGH("Secondary activation : Processing TX Req QOS Flow Spec");
    /*-----------------------------------------------------------------------
       Convert the Data Rate parameter of required set into UMTS specific:
       DL Max Data Rate, DL Guartanteed Data Rate
    -----------------------------------------------------------------------*/
    dsumtsps_process_data_rate(
                                &app_qos->tx.flow_template.req_flow,
                                app_qos->tx.flow_template.req_flow.latency,
                                app_qos->tx.flow_template.req_flow.field_mask,
                                &temp_max_bitrate,
                                &temp_gtd_bitrate );

    req_qos->max_ul_bitrate =  temp_max_bitrate;
    req_qos->gtd_ul_bitrate =  temp_gtd_bitrate;
  }


  /*-------------------------------------------------------------------------
    If either the minimum Rx or Tx flow has been specified, set the valid 
    flag to TRUE.
  -------------------------------------------------------------------------*/
  if ((app_qos->field_mask & (int)QOS_MASK_RX_MIN_FLOW) ||
      ( app_qos->field_mask & (int)QOS_MASK_TX_MIN_FLOW))
  {
    min_qos->valid_flg = TRUE;
  }
  else
  {
    min_qos->valid_flg = FALSE;    
  }


  /*-----------------------------------------------------------------------
    DL Minimum QOS parameters have been specified by the application
    Convert the DL Data Rate, DL GTD Data Rate, Traffic class, Traffic
    handling priority, Residual Bit Error Rate, SDU Error Ratio
  -----------------------------------------------------------------------*/
  if ( app_qos->field_mask & (int)QOS_MASK_RX_MIN_FLOW )
  {
    DS_3GPP_MSG0_HIGH("Secondary activation : Processing RX Min QOS Flow Spec");
    /*-----------------------------------------------------------------------
       Convert the Traffic Class parameter of minimum set into UMTS specific.
    -----------------------------------------------------------------------*/
    dsumtsps_process_traffic_class (
                            app_qos->rx.flow_template.min_req_flow.trf_class,
                            app_qos->rx.flow_template.min_req_flow.field_mask,
                            min_qos );

    /*-----------------------------------------------------------------------
       Convert the Data Rate parameter of minimum set into UMTS specific:
       DL Max Data Rate, DL Guartanteed Data Rate
    -----------------------------------------------------------------------*/
    dsumtsps_process_data_rate(
                             &app_qos->rx.flow_template.min_req_flow,
                             app_qos->rx.flow_template.min_req_flow.latency,
                             app_qos->rx.flow_template.min_req_flow.field_mask,
                             &temp_max_bitrate,
                             &temp_gtd_bitrate );

    min_qos->max_dl_bitrate =  temp_max_bitrate;
    min_qos->gtd_dl_bitrate =  temp_gtd_bitrate;

    /*-----------------------------------------------------------------------
       Convert the Packet Error Rate parameter of minimum set into SDU Error.
    -----------------------------------------------------------------------*/
    dsumtsps_process_pkt_err_rate(
                            &app_qos->rx.flow_template.min_req_flow,
                            app_qos->rx.flow_template.min_req_flow.field_mask,
                            min_qos);

    /*-----------------------------------------------------------------------
       Convert the Residual Bit Error Rate of minimum set into UMTS specific.
    -----------------------------------------------------------------------*/
    dsumtsps_process_residual_ber(
                            &app_qos->rx.flow_template.min_req_flow,
                            app_qos->rx.flow_template.min_req_flow.field_mask,
                            min_qos);

    /*-----------------------------------------------------------------------
       Convert the Traffic Handling Priority parameter of minimum set
    -----------------------------------------------------------------------*/
    dsumtsps_process_trf_hdl_prio(
                            &app_qos->rx.flow_template.min_req_flow,
                            app_qos->rx.flow_template.min_req_flow.field_mask,
                            min_qos);

    /*-------------------------------------------------------------------------
      Fill the signalling Indication bit if the traffic class is interactive.
      and Traffic Handling priority is 1.

      Sig Ind Flag should only be set if the traffic class is interactive
      and the traffic handling priority is 1 (This is as per spec 
      23.107 Sec 6.4.3.1).

      Signalling Indication (Yes/No)
      Definition: Indicates the signalling nature of the submitted SDUs.
      This attribute is additional to the other QoS attributes and does
      not over-ride them. This attribute is only defined for the
      interactive traffic class. If signalling indication is set to "Yes",
      the UE should set the traffic handling priority to "1".

    -------------------------------------------------------------------------*/
	dsumtsps_process_sig_ind_prio(&app_qos->rx.flow_template.min_req_flow,
                      app_qos->rx.flow_template.min_req_flow.field_mask,
                       min_qos);
  }

  /*-----------------------------------------------------------------------
    UL Required QOS parameters have been specified by the application
    Convert the UL Data Rate, UL GTD Data Rate
  -----------------------------------------------------------------------*/
  if ( app_qos->field_mask & (int)QOS_MASK_TX_FLOW )
  {
    DS_3GPP_MSG0_HIGH("Secondary activation : Processing TX Req QOS Flow Spec");
    /*-----------------------------------------------------------------------
       Convert the Data Rate parameter of required set into UMTS specific:
       DL Max Data Rate, DL Guartanteed Data Rate
    -----------------------------------------------------------------------*/
    dsumtsps_process_data_rate(
                                &app_qos->tx.flow_template.req_flow,
                                app_qos->tx.flow_template.req_flow.latency,
                                app_qos->tx.flow_template.req_flow.field_mask,
                                &temp_max_bitrate,
                                &temp_gtd_bitrate );

    req_qos->max_ul_bitrate =  temp_max_bitrate;
    req_qos->gtd_ul_bitrate =  temp_gtd_bitrate;
  }

  /*-----------------------------------------------------------------------
    UL Required QOS parameters have been specified by the application
    Convert the UL Data Rate, UL GTD Data Rate
  -----------------------------------------------------------------------*/
  if ( app_qos->field_mask & (int)QOS_MASK_TX_MIN_FLOW )
  {
    DS_3GPP_MSG0_HIGH("Secondary activation : Processing TX Min QOS Flow Spec");
    /*-----------------------------------------------------------------------
       Convert the Data Rate parameter of required set into UMTS specific:
       DL Max Data Rate, DL Guartanteed Data Rate
    -----------------------------------------------------------------------*/
    dsumtsps_process_data_rate(
                             &app_qos->tx.flow_template.min_req_flow,
                             app_qos->tx.flow_template.min_req_flow.latency,
                             app_qos->tx.flow_template.min_req_flow.field_mask,
                             &temp_max_bitrate,
                             &temp_gtd_bitrate );

    min_qos->max_ul_bitrate =  temp_max_bitrate;
    min_qos->gtd_ul_bitrate =  temp_gtd_bitrate;
  }


  min_qos_nw = &(umts_bflow_manager_p->ds_umts_bearer_flow_manager_dynamic_p->min_qos);
  req_qos_nw = &(umts_bflow_manager_p->ds_umts_bearer_flow_manager_dynamic_p->req_qos);

  /*------------------------------------------------------------------------
    request Subscribed QOS : NOTE: All values are set to 0x0 implying
    subscribed value : ref TS24.008 sec 10.5.6.5
  -------------------------------------------------------------------------*/
  req_qos_nw->valid                = TRUE;
  req_qos_nw->delay_class          = 0x0;
  req_qos_nw->reliability_class    = 0x0;
  req_qos_nw->peak_throughput      = 0x0;
  req_qos_nw->precedence_class     = 0x0;
  req_qos_nw->mean_throughput      = 0x0;
  req_qos_nw->traffic_class        = 0x0;
  req_qos_nw->deliver_order        = 0x0;
  req_qos_nw->deliver_err_sdu      = 0x0;
  req_qos_nw->max_sdu_size         = 0x0;
  req_qos_nw->max_bit_rare_uplink  = 0x0;
  req_qos_nw->max_bit_rare_downlink= 0x0;
  req_qos_nw->residual_ber         = 0x0;
  req_qos_nw->sdu_err_ratio        = 0x0;
  req_qos_nw->tranfer_delay        = 0x0;
  req_qos_nw->traffic_priority     = 0x0;
  req_qos_nw->guarantee_ber_uplink = 0x0;
  req_qos_nw->guarantee_ber_dwnlink= 0x0;
  req_qos_nw->sig_ind = FALSE;
  req_qos_nw->src_stat_desc = 0;
  req_qos_nw->ext_max_bit_rate_downlink= 0x0;
  req_qos_nw->ext_gtd_bit_rate_downlink= 0x0;

#ifdef FEATURE_REL7
  req_qos_nw->ext_max_bit_rate_uplink= 0x0;
  req_qos_nw->ext_gtd_bit_rate_uplink= 0x0;
#endif

  /*------------------------------------------------------------------------
    Minimum Subscribed QOS : NOTE: All values are set to 0x0 implying
    subscribed value : ref TS24.008 sec 10.5.6.5
  -------------------------------------------------------------------------*/
  min_qos_nw->valid                = TRUE;
  min_qos_nw->delay_class          = 0x0;
  min_qos_nw->reliability_class    = 0x0;
  min_qos_nw->peak_throughput      = 0x0;
  min_qos_nw->precedence_class     = 0x0;
  min_qos_nw->mean_throughput      = 0x0;
  min_qos_nw->traffic_class        = 0x0;
  min_qos_nw->deliver_order        = 0x0;
  min_qos_nw->deliver_err_sdu      = 0x0;
  min_qos_nw->max_sdu_size         = 0x0;
  min_qos_nw->max_bit_rare_uplink  = 0x0;
  min_qos_nw->max_bit_rare_downlink= 0x0;
  min_qos_nw->residual_ber         = 0x0;
  min_qos_nw->sdu_err_ratio        = 0x0;
  min_qos_nw->tranfer_delay        = 0x0;
  min_qos_nw->traffic_priority     = 0x0;
  min_qos_nw->guarantee_ber_uplink = 0x0;
  min_qos_nw->guarantee_ber_dwnlink= 0x0;
  min_qos_nw->sig_ind = FALSE;
  min_qos_nw->src_stat_desc = 0;
  min_qos_nw->ext_max_bit_rate_downlink= 0x0;
  req_qos_nw->ext_gtd_bit_rate_downlink= 0x0;
#ifdef FEATURE_REL7
  min_qos_nw->ext_max_bit_rate_uplink= 0x0;
  req_qos_nw->ext_gtd_bit_rate_uplink= 0x0;
#endif

  /*------------------------------------------------------------------------
    Set the R99 QOS required parameters
  ------------------------------------------------------------------------*/
  DS_3GPP_MSG0_HIGH("Secondary activation: filling Requested qos");
  ds_umts_bearer_flow_manager_fill_sec_context_r99_qos_params(req_qos, req_qos_nw);

  /*------------------------------------------------------------------------
      R97/98 parameters Spec mandates that we translate
      R99 parameters to corresponding R97 values and fill in
  ------------------------------------------------------------------------*/
  ds_umts_bearer_flow_manager_fill_qos_r9798_params_from_r99_params(req_qos, req_qos_nw);

  /*------------------------------------------------------------------------
    Set the R99 QOS minimum parameters
  ------------------------------------------------------------------------*/
  DS_3GPP_MSG0_HIGH("Secondary activation: filling minimum qos");
  ds_umts_bearer_flow_manager_fill_sec_context_r99_min_qos_params(min_qos, min_qos_nw);

  /*------------------------------------------------------------------------
      R97/98 parameters Spec mandates that we translate
      R99 parameters to corresponding R97 values and fill in
  ------------------------------------------------------------------------*/
  DS_3GPP_MSG0_HIGH("Secondary activation: filling R97 qos");
  ds_umts_bearer_flow_manager_fill_min_qos_r9798_params_from_r99_params(min_qos, min_qos_nw);

  /*------------------------------------------------------------------------
    Set the Rel 5 Qos parameters. If the traffic class is interactive and
    traffic priority is 1, set the signalling indication flag.

    Sig Ind Flag should only be set if the traffic class is interactive
    and the traffic handling priority is 1 (This is as per spec 
    23.107 Sec 6.4.3.1).

    Signalling Indication (Yes/No)
    Definition: Indicates the signalling nature of the submitted SDUs.
    This attribute is additional to the other QoS attributes and does
    not over-ride them. This attribute is only defined for the
    interactive traffic class. If signalling indication is set to "Yes",
    the UE should set the traffic handling priority to "1". 
  ------------------------------------------------------------------------*/
  if( (req_qos_nw->traffic_class == DS_UMTS_QOS_TCLASS_INTR) &&
      (req_qos_nw->traffic_priority == 0x1) )
  {
    req_qos_nw->sig_ind = req_qos->sig_ind;
  }

  /*------------------------------------------------------------------------
    Set the Rel 5 min Qos parameters. If the traffic class is interactive and
    traffic priority is 1, set the signalling indication flag.
  ------------------------------------------------------------------------*/
  if( (min_qos_nw->traffic_class == DS_UMTS_QOS_TCLASS_INTR) && 
      (min_qos_nw->traffic_priority == 0x1) )
  {
    min_qos_nw->sig_ind = min_qos->sig_ind;
  }

  return DS3G_SUCCESS;
}/* ds_umts_bearer_flow_manager_convert_app_data_rate_to_nw */

/*===========================================================================
FUNCTION DS_UMTS_BEARER_FLOW_MANAGER_STORE_NW_DATA_RATE
DESCRIPTION
  Converts the APP passed data rate to UMTS data rate and store in bearer
  flow manager
  orig_call_hdlr converts the UMTS values to NW format.
  In UMTS, except data rates and guaranteed data rates, other parameters
  are common across UL and DL. 

PARAMETERS
nw_qos          -  NW qos to be converted
bflow_manager_p -  UMTS bearer flow manager 

DEPENDENCIES
  None.

RETURN VALUE
boolean         - return DS3G_SUCCESS or DS3G_FAILURE
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_umts_bearer_flow_manager_store_nw_data_rate
(
   const cm_gw_ps_call_info_s_type     *gw_ps_call_p,  
   void            *bflow_manager_p
)
{
  ds_umts_umts_qos_params_type  *req_qos = NULL;
  ds_umts_umts_qos_params_type  *min_qos = NULL;
  ds_umts_bearer_flow_manager_s *umts_bflow_manager_p;
  ds_umts_gprs_qos_params_type  *gprs_app_req_qos = NULL;
  ds_umts_gprs_qos_params_type  *gprs_app_min_qos = NULL;
  sys_sys_mode_e_type  umts_network_mode = SYS_SYS_MODE_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
#if defined FEATURE_WCDMA
  umts_network_mode = SYS_SYS_MODE_WCDMA;
#elif defined FEATURE_TDSCDMA
  umts_network_mode =  SYS_SYS_MODE_TDS;
#elif defined FEATURE_GSM_GPRS
  umts_network_mode = SYS_SYS_MODE_GSM;
#endif

  if(bflow_manager_p == NULL || gw_ps_call_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("input parameters are NULL");
    return DS3G_FAILURE;
  } 
  if (!(gw_ps_call_p->qos.valid))
  {
    DS_3GPP_MSG0_ERROR("NW QoS is not valid");
    return DS3G_FAILURE;
  }

  umts_bflow_manager_p = (ds_umts_bearer_flow_manager_s *)
               DS_BEARER_FLOW_MGR_VOBJ((ds_bearer_flow_manager_s *)bflow_manager_p,
                                        umts_network_mode);
  

  if(!ds_umts_bearer_flow_manager_validate(umts_bflow_manager_p))
  {

     DS_3GPP_MSG0_ERROR("input parameters are Invalid");
    return DS3G_FAILURE;
  }

  req_qos = &(umts_bflow_manager_p->ds_umts_bearer_flow_manager_dynamic_p->app_sps_req_qos);

  if (dsumtsps_conv_netw_qos_to_umts_qos(&(gw_ps_call_p->qos), req_qos) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("failed to conver nw req_QOS to umts req qos");
    return DS3G_FAILURE;
  }
  gprs_app_req_qos = &(umts_bflow_manager_p->ds_umts_bearer_flow_manager_dynamic_p->gprs_app_req_qos);
  if(dsumtsps_conv_netw_qos_to_gprs_qos(&(gw_ps_call_p->qos),gprs_app_req_qos )== FALSE)
  {
    DS_3GPP_MSG0_ERROR("failed to conver nw req_QOS to umts req qos");
    return DS3G_FAILURE;
  }
  if(gw_ps_call_p->minimum_qos.valid)
  { 
    min_qos = &(umts_bflow_manager_p->ds_umts_bearer_flow_manager_dynamic_p->app_sps_min_qos);
    if(dsumtsps_conv_netw_qos_to_umts_qos(&(gw_ps_call_p->minimum_qos), min_qos) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("failed to conver nw min_QOS to umts min qos");
      return DS3G_FAILURE;
    }
    gprs_app_min_qos = &(umts_bflow_manager_p->ds_umts_bearer_flow_manager_dynamic_p->gprs_app_req_qos);

    if(dsumtsps_conv_netw_qos_to_gprs_qos(&(gw_ps_call_p->minimum_qos),gprs_app_min_qos )== FALSE)
    {
      DS_3GPP_MSG0_ERROR("failed to conver nw req_QOS to umts min qos");
      return DS3G_FAILURE;
    }
  }
  if( req_qos->traffic_class == DS_UMTS_QOS_TCLASS_CONV ||
      req_qos->traffic_class == DS_UMTS_QOS_TCLASS_STRM )
  {
    umts_bflow_manager_p->ds_umts_bearer_flow_manager_dynamic_p->src_stat_desc_req = 
		                                             (uint32)(gw_ps_call_p->qos.src_stat_desc);
  }

  return DS3G_SUCCESS;
} /* ds_umts_bearer_flow_manager_store_nw_data_rate */

/*===========================================================================
FUNCTION DS_UMTS_BEARER_FLOW_MANAGER_FLOW_VALIDATION

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
boolean ds_umts_bearer_flow_manager_flow_validation
( 
  qos_spec_type      *input_qos
)
{

  ip_flow_type    *rx_flow = NULL;
  ip_flow_type    *tx_flow = NULL; 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(input_qos==NULL)
  {
    DS_3GPP_MSG0_ERROR("Input parameter is NULL");
    return FALSE;
  }
  /*------------------------------------------------------------------------
    Configure the flow pointers to requested flows.
  -------------------------------------------------------------------------*/
  rx_flow = &(input_qos->rx.flow_template.req_flow);
  tx_flow = &(input_qos->tx.flow_template.req_flow);

  
  /*------------------------------------------------------------------------
    Verify REQ TX FLOW Params
  -------------------------------------------------------------------------*/
  if(input_qos->field_mask & (QOS_MASK_TX_FLOW))
  {
    if(dsumtsps_qos_flow_validation(tx_flow,QOS_MASK_TX_FLOW) != DS3G_SUCCESS)
    {
      DS_3GPP_MSG0_ERROR("TX flow validation failed");
      return FALSE;
    }    
  } /* if QOS_MASK_TX_FLOW */

  /*------------------------------------------------------------------------
    Verify REQ RX FLOW Params
  -------------------------------------------------------------------------*/
  if(input_qos->field_mask & (QOS_MASK_RX_FLOW))
  {
    if(dsumtsps_qos_flow_validation(rx_flow,QOS_MASK_RX_FLOW) != DS3G_SUCCESS)
    {
      DS_3GPP_MSG0_ERROR("RX flow validation failed");
      return FALSE;
    }    
  } /* if QOS_MASK_RX_FLOW */

  /*------------------------------------------------------------------------
    Configure the flow pointers to Min flows.
  -------------------------------------------------------------------------*/
  rx_flow = &(input_qos->rx.flow_template.min_req_flow);
  tx_flow = &(input_qos->tx.flow_template.min_req_flow);

  /*------------------------------------------------------------------------
    Verify MIN TX FLOW Params
  -------------------------------------------------------------------------*/
  if(input_qos->field_mask & (QOS_MASK_TX_MIN_FLOW))
  {
    if(dsumtsps_qos_flow_validation(tx_flow,QOS_MASK_TX_MIN_FLOW) 
       != DS3G_SUCCESS)
    {
      DS_3GPP_MSG0_ERROR("TX Min flow validation failed");
      return FALSE;
    }    
  } /* if QOS_MASK_TX_FLOW */

  /*------------------------------------------------------------------------
    Verify MIN RX FLOW Params
  -------------------------------------------------------------------------*/
  if(input_qos->field_mask & (QOS_MASK_TX_MIN_FLOW))
  {
    if(dsumtsps_qos_flow_validation(rx_flow,QOS_MASK_RX_MIN_FLOW) 
       != DS3G_SUCCESS)
    {
      DS_3GPP_MSG0_ERROR("RX Min flow validation failed");
      return FALSE;
    }    
  } /* if QOS_MASK_RX_FLOW */
  
  return TRUE;
} /* ds_umts_bearer_flow_manager_flow_validation */

/*===========================================================================
FUNCTION DS_UMTS_BEARER_FLOW_GET_UMTS_NETW_ALLOC_QOS

DESCRIPTION
  Returns the pointer to umts_netw_alloc_qos from ds_umts_bflow_manager_tbl
  based on the call instance value.

PARAMETERS
 call_instance                - index of ds_umts_bflow_manager_tbl.

DEPENDENCIES
  None.

RETURN VALUE
 ds_umts_umts_qos_params_type - pointer to umts_netw_alloc_qos.
  
SIDE EFFECTS
  None.

===========================================================================*/
ds_umts_umts_qos_params_type*
ds_umts_bearer_flow_manager_get_umts_netw_alloc_qos
(
  uint32 call_instance
)
{
  if(call_instance >= DS_3GPP_MAX_BEARER_CONTEXT)
  {
    DS_3GPP_MSG0_HIGH("call instance excedded DS_3GPP_MAX_BEARER_CONTEXT");
    return NULL;
  }
  
  if(!ds_umts_bearer_flow_manager_validate(&ds_umts_bflow_manager_tbl[call_instance]))
  {
     DS_3GPP_MSG0_ERROR("input call parameters are Invalid");
     return NULL;
  }

  return &(ds_umts_bflow_manager_tbl[call_instance].ds_umts_bearer_flow_manager_dynamic_p->umts_netw_alloc_qos);
} /* ds_umts_bearer_flow_manager_get_umts_netw_alloc_qos */

/*===========================================================================
FUNCTION DS_UMTS_BEARER_FLOW_GET_GPRS_NETW_ALLOC_QOS

DESCRIPTION
  Returns the pointer to gprs_netw_alloc_qos from ds_umts_bflow_manager_tbl
  based on the call instance value.

PARAMETERS
 call_instance                - index of ds_umts_bflow_manager_tbl.

DEPENDENCIES
  None.

RETURN VALUE
 ds_umts_umts_qos_params_type - pointer to gprs_netw_alloc_qos.
  
SIDE EFFECTS
  None.

===========================================================================*/
ds_umts_gprs_qos_params_type*
ds_umts_bearer_flow_manager_get_gprs_netw_alloc_qos
(
  uint32 call_instance
)
{
  if(call_instance >= DS_3GPP_MAX_BEARER_CONTEXT)
  {
    DS_3GPP_MSG0_HIGH("call instance excedded DS_3GPP_MAX_BEARER_CONTEXT");
    return NULL;
  }

  if(!ds_umts_bearer_flow_manager_validate(&ds_umts_bflow_manager_tbl[call_instance]))
  {
     DS_3GPP_MSG0_ERROR("input call parameters are Invalid");
     return NULL;
  }


  return &(ds_umts_bflow_manager_tbl[call_instance].ds_umts_bearer_flow_manager_dynamic_p->gprs_netw_alloc_qos);
} /* ds_umts_bearer_flow_manager_get_gprs_netw_alloc_qos */

/*===========================================================================
FUNCTION DS_UMTS_BEARER_FLOW_MANAGER_UPDATE_FLOW_ON_RESUME

DESCRIPTION
  Updates flow from L->W handoff with W params

PARAMETERS
  bflow_manager_p   - bearer flow manager ptr.

DEPENDENCIES
  None.

RETURN VALUE
  boolean - True if success, false otherwise.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_umts_bearer_flow_manager_update_flow_on_resume
(
    ds_bearer_context_s* bearer_context_p
)
{
  qos_spec_type       app_qos;
  ds_flow_context_s  *flow_context_p = NULL;
  ds_3gpp_internal_cause_code_enum_type icause_code;
  ds_umts_umts_qos_params_type* umts_qos_p;
  ds_umts_gprs_qos_params_type* gprs_qos_p;
   
  ds_bearer_flow_manager_s *bflow_manager_p;
  /*----------------------------------------------------------------------------------------*/
  icause_code = DS_3GPP_INTERNAL_CC_INVALID;
  DS_3GPP_MSG0_HIGH("in ds_umts_bearer_flow_manager_update_flow_on_resume");

  bflow_manager_p = ds_bearer_flow_manager_find_flow_manager(bearer_context_p);

  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    return FALSE;
  }

  memset(&app_qos, 0, sizeof(qos_spec_type));

  umts_qos_p = 
    &(umts_bearer_context_tbl[(uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr].\
		    ds_umts_bearer_context_dyn_p->umts_qos_val);
  gprs_qos_p = 
    &(umts_bearer_context_tbl[(uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr].\
		    ds_umts_bearer_context_dyn_p->gprs_qos_val);

  if(bflow_manager_p->ds_bearer_flow_manager_dyn_p->neg_qos.valid)
  {
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

    // convert the stored UMTS flow to app flow
    if (ds_3gpp_bearer_flow_manager_convert_nw_qos_to_app(bflow_manager_p,
                      &(bflow_manager_p->ds_bearer_flow_manager_dyn_p->neg_qos),
                                                      &app_qos,
                                                      &icause_code) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Cannot convert nw qos to app");
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return FALSE;
    }
  

   /*------------------------------------------------------------------------- 
     Associate the mode specific blob that is going to be populated to the 
     bearer context pointer.
   -------------------------------------------------------------------------*/
   bearer_context_p->ds_bearer_context_dyn_p->vobj_data_ptr_tbl[DS_3GPP_UMTS_VOBJ] =\
     (void*)&umts_bearer_context_tbl[(uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr];

   /*------------------------------------------------------------------------- 
     Convert the network given QOS params to UMTS parameters and store them in
     the bearer context. Also convert the network QOS parameters to GPRS parms
     and store them in the bearer context. If there is a failure in the conv,
     return FAILURE to the calling function (3gpp_hdlr) so that a call tear
     down is initiated.
   -------------------------------------------------------------------------*/

   if ( !dsUmtsBearerCntxt_conv_qos_netw_to_umts( 
  	       &(bflow_manager_p->ds_bearer_flow_manager_dyn_p->neg_qos),umts_qos_p ) )
   {
     umts_qos_p->valid_flg = FALSE;
     DS_3GPP_MSG1_ERROR("Unable to convert Netw to UMTS QoS, Bearer Inst:%d",
              (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);
   }
   else
   {  
     if( umts_qos_p->traffic_class == DS_UMTS_QOS_TCLASS_CONV ||
         umts_qos_p->traffic_class == DS_UMTS_QOS_TCLASS_STRM )
     {
       /*----------------------------------------------------------------------
         SRC STAT DESC: Convert & Source statiscs descriptor,same for UL and DL 
       ----------------------------------------------------------------------*/
       DS_3GPP_MSG1_HIGH("converting the source statistics parameter: src_stat_desc - %d", 
                bflow_manager_p->ds_bearer_flow_manager_dyn_p->neg_qos.src_stat_desc);
       umts_bearer_context_tbl[(uint32)bearer_context_p->ds_bearer_context_dyn_p->
         client_data_ptr].ds_umts_bearer_context_dyn_p->src_stat_desc = 
           bflow_manager_p->ds_bearer_flow_manager_dyn_p->neg_qos.src_stat_desc;
     }
   }

   if ( !dsUmtsBearerCntxt_conv_qos_netw_to_gprs( 
  	    &(bflow_manager_p->ds_bearer_flow_manager_dyn_p->neg_qos),gprs_qos_p ) )
   {
     gprs_qos_p->valid_flg = FALSE;
     DS_3GPP_MSG1_ERROR("Unable to convert Netw to GPRS QoS, Bearer Inst:%d",
              (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);

   }
   DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    /*---------------------------------------------------------------------
      Update all the Flows with the updated flow rate
      Need to optimize for the case, there will be some flows in going
      down state and will not be intereseted in flow rate change.
    ---------------------------------------------------------------------*/          
    flow_context_p = list_peek_front(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->flow_context));
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
    DS_3GPP_MSG0_ERROR("No UMTS qos to update flow on resume");
    return FALSE;
  }
  return TRUE;
}

#endif /* WCDMA or GPRS */
