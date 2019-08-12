 /******************************************************************************
 @file    ds_3gpp_profile_validator.c
 @brief   

  DESCRIPTION
  Tech specific implementation of 3GPP Profile Management  

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2009 - 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
****************************************************************************/
/*===========================================================================
4
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/27/13   vb      Validate 3gpp profiel parameters
=============================================================================*/

#include "ds_profile_db.h"
#include "ds_3gpp_profile_validator.h"
#include "ds_3gppi_utils.h"
#include <string.h>
#include "dsumtspdpreg.h"
#include "dsumtspsqos.h"
#include "ds_profile_3gpp.h"
#include "ds_3gpp_qos.h"

/* 3GPP QoS Parameters */
#define DS_PROFILE_3GPP_QOS_PARAM_MAX_UL_BITRATE   QOS_MAX_UL_KBIT_RATE
#define DS_PROFILE_3GPP_QOS_PARAM_MAX_DL_BITRATE   QOS_MAX_DL_KBIT_RATE

#define DS_PROFILE_3GPP_QOS_PARAM_MAX_GTD_UL_BITRATE  QOS_MAX_UL_KBIT_RATE
#define DS_PROFILE_3GPP_QOS_PARAM_MAX_GTD_DL_BITRATE  QOS_MAX_DL_KBIT_RATE

#define DS_PROFILE_3GPP_QOS_PARAM_MAX_SDU_SIZE     QOS_PARAM_MAX_SDU_SIZE

#define DS_PROFILE_3GPP_QOS_PARAM_MAX_TRANS_DELAY  QOS_PARAM_MAX_TRANS_DELAY

#define DS_PROFILE_3GPP_QOS_PARAM_MAX_THANDLE_PRIORITY  \
                                               QOS_PARAM_MAX_THANDLE_PRIORITY

#define DS_PROFILE_3GPP_GPRS_QOS_PARAM_MAX_RELIABILITY_CLASS  \
                                               QOS_PARAM_MAX_RELIABILITY_CLASS

#define DS_PROFILE_3GPP_GPRS_QOS_PARAM_MAX_DELAY_CLASS        \
                                               QOS_PARAM_MAX_DELAY_CLASS

#define DS_PROFILE_3GPP_GPRS_QOS_PARAM_MAX_PRECEDENCE_CLASS   \
                                               QOS_PARAM_MAX_PRECEDENCE_CLASS

#define DS_PROFILE_3GPP_GPRS_QOS_PARAM_MAX_PEAK_THROUGHPUT_CLASS   \
                                            QOS_PARAM_MAX_PEAK_THROUGHPUT_CLASS

#define DS_PROFILE_3GPP_GPRS_QOS_PARAM_MAX_MEAN_THROUGHPUT_CLASS   \
                                            QOS_PARAM_MAX_MEAN_THROUGHPUT_CLASS
											
#define DS_PROFILE_MAX_PDN_CONN_PER_BLOCK  1023
#define DS_PROFILE_MAX_PDN_CONN_REQ_WAIT_TIME 1023
#define DS_PROFILE_MAX_PDN_CONN_TIME 3600

#ifdef FEATURE_DATA_LTE
/* LTE QoS parameters */
#define DS_PROFILE_3GPP_LTE_QOS_PARAM_MAX_UL_BITRATE   QOS_LTE_MAX_UL_KBIT_RATE
#define DS_PROFILE_3GPP_LTE_QOS_PARAM_MAX_DL_BITRATE   QOS_LTE_MAX_DL_KBIT_RATE

#define DS_PROFILE_3GPP_LTE_QOS_PARAM_MAX_GTD_UL_BITRATE  QOS_LTE_MAX_UL_KBIT_RATE
#define DS_PROFILE_3GPP_LTE_QOS_PARAM_MAX_GTD_DL_BITRATE  QOS_LTE_MAX_DL_KBIT_RATE
#endif /* FEATURE_DATA_LTE */


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_INACTIVITY_TIMER_VAL

DESCRIPTION
  The function validates 3gpp inactivity timer value.
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the inactivity timer value.
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/

ds_profile_db_result_type ds_3gpp_profile_validate_inactivity_timer_val
(
  ds_profile_db_ident_type        ident,
  const ds_profile_db_info_type    *info,
  const void *profile
)
{
  ds_profile_db_result_type ret_val = DB_RESULT_SUCCESS;
  uint32 inactivity_timer_val = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(info == NULL)
  {
    DS_3GPP_MSG3_HIGH("Info parameter is NULL",0,0,0);
    return DB_RESULT_FAIL;
  }
  /*----------------------------------------------------------------------- 
    Check if the inactivity timer value is non negative
   -----------------------------------------------------------------------*/
  memscpy((void *)&inactivity_timer_val, info->len, info->buf,info->len);

  return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_MNC

DESCRIPTION
  The function validates 3gpp mnc
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the inactivity timer value.
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_mnc
(
  ds_profile_db_ident_type        ident,
  const ds_profile_db_info_type    *info,
  const void *profile
)
{
  ds_profile_db_result_type ret_val = DB_RESULT_SUCCESS;
  ds_umts_3gpp_mnc_type mnc;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(info == NULL)
  {
    DS_3GPP_MSG3_HIGH("Info parameter is NULL",0,0,0);
    return DB_RESULT_FAIL;
  }
  /*----------------------------------------------------------------------- 
    Check if the mnc_digits are non negative
   -----------------------------------------------------------------------*/
  memset(&mnc,0,sizeof(ds_umts_3gpp_mnc_type));
  memscpy((void *)&mnc, info->len, info->buf,info->len);

  return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_MCC

DESCRIPTION
  The function validates 3gpp mcc
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the inactivity timer value.
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_mcc
(
  ds_profile_db_ident_type        ident,
  const ds_profile_db_info_type    *info,
  const void *profile
)
{
  ds_profile_db_result_type ret_val = DB_RESULT_SUCCESS;
  uint16    mcc = 0;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(info == NULL)
  {
    DS_3GPP_MSG3_HIGH("Info parameter is NULL",0,0,0);
    return DB_RESULT_FAIL;
  }
  /*----------------------------------------------------------------------- 
    Check if mcc is non negative
   -----------------------------------------------------------------------*/
  memscpy((void *)&mcc, info->len, info->buf,info->len);

  return ret_val;
}


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_OPERATOR_PCO

DESCRIPTION
  The function validates 3gpp operator pco
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the inactivity timer value.
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_operator_pco
(
	ds_profile_db_ident_type        ident,
  const ds_profile_db_info_type    *info,
  const void *profile
)
{
  ds_profile_db_result_type ret_val = DB_RESULT_SUCCESS;
  uint16 operator_pco = 0;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	if(info == NULL)
	{
		DS_3GPP_MSG3_HIGH("Info parameter is NULL",0,0,0);
		return DB_RESULT_FAIL;
	}
  /*----------------------------------------------------------------------- 
    Check if operator pco is non negative
   -----------------------------------------------------------------------*/
  memscpy((void *)&operator_pco, info->len, info->buf,info->len);

  return ret_val;
}


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_APN_BEARER

DESCRIPTION
  The function validates 3gpp apn bearer
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the inactivity timer value.
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_apn_bearer
(
  ds_profile_db_ident_type        ident,
  const ds_profile_db_info_type    *info,
  const void *profile
)
{
  ds_profile_db_result_type ret_val = DB_RESULT_SUCCESS;
  uint8 apn_bearer = 0;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(info == NULL)
  {
    DS_3GPP_MSG3_HIGH("Info parameter is NULL",0,0,0);
    return DB_RESULT_FAIL;
  }


  /*----------------------------------------------------------------------- 
    Check if apn bearer is non negative
   -----------------------------------------------------------------------*/
  memscpy((void *)&apn_bearer, info->len, info->buf,info->len);

  if(apn_bearer != DS_PROFILE_3GPP_APN_BEARER_FLAG_ALL && 
     (apn_bearer & ~(DS_PROFILE_3GPP_APN_BEARER_FLAG_G | 
     DS_PROFILE_3GPP_APN_BEARER_FLAG_W | DS_PROFILE_3GPP_APN_BEARER_FLAG_L)) != 0)
  {
    DS_3GPP_MSG3_ERROR("APN bearer is not valid:%d",apn_bearer,0,0);
    ret_val = DB_RESULT_FAIL;
  }

  return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_SRC_STAT_DESC

DESCRIPTION
  The function validates 3gpp src stat descriptor for both req and min
  qos
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the inactivity timer value.
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_src_stat_desc
(
  ds_profile_db_ident_type        ident,
  const ds_profile_db_info_type    *info,
  const void *profile
)
{
  ds_profile_db_result_type ret_val = DB_RESULT_SUCCESS;
  uint32 src_stat_desc = 0;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(info == NULL)
  {
    DS_3GPP_MSG3_HIGH("Info parameter is NULL",0,0,0);
    return DB_RESULT_FAIL;
  }
  /*----------------------------------------------------------------------- 
    Check if src stat descriptor is non negative
   -----------------------------------------------------------------------*/
  memscpy((void *)&src_stat_desc, info->len, info->buf,info->len);

  return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_VERSION

DESCRIPTION
  The function validates 3gpp version
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the inactivity timer value.
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_version
(
	ds_profile_db_ident_type        ident,
  const ds_profile_db_info_type    *info,
  const void *profile
)
{
  ds_profile_db_result_type ret_val = DB_RESULT_SUCCESS;
  byte profile_version = 0;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(info == NULL)
  {
    DS_3GPP_MSG3_HIGH("Info parameter is NULL",0,0,0);
    return DB_RESULT_FAIL;
  }
  /*----------------------------------------------------------------------- 
    Check if PROFILE VERSION is non negative
   -----------------------------------------------------------------------*/
  memscpy((void *)&profile_version, info->len, info->buf,info->len);

  return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_LINGER_PARAMS

DESCRIPTION
  The function validates 3gpp linger params
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the inactivity timer value.
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_linger_params
(
  ds_profile_db_ident_type        ident,
  const ds_profile_db_info_type    *info,
  const void *profile
)
{
  ds_profile_db_result_type ret_val = DB_RESULT_SUCCESS;
  ds_profile_db_linger_params_type linger_params;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(info == NULL)
  {
    DS_3GPP_MSG3_HIGH("Info parameter is NULL",0,0,0);
    return DB_RESULT_FAIL;
  }
  /*----------------------------------------------------------------------- 
    Check if linger value timeout is non negative
   -----------------------------------------------------------------------*/
  memset(&linger_params,0,sizeof(ds_profile_db_linger_params_type));
  memscpy((void *)&linger_params, info->len, info->buf,info->len);

  return ret_val;
}


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_GPRS_QOS_PARAMS

DESCRIPTION
  The function validates gprs qos parameters
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the gprs qos parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_gprs_qos_params
(
  ds_profile_db_ident_type        ident,
  const ds_profile_db_info_type    *info,
  const void *profile
)
{
  ds_profile_db_result_type ret_val = DB_RESULT_SUCCESS;
  ds_profile_db_3gpp_umts_gprs_qos_params_type gprs_qos_params;

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(info == NULL)
  {
    DS_3GPP_MSG3_HIGH("Info parameter is NULL",0,0,0);
    return DB_RESULT_FAIL;
  }
  /*----------------------------------------------------------------------- 
    Check if GPRS QOS params are non negative
   -----------------------------------------------------------------------*/
  memset(&gprs_qos_params,0,sizeof(ds_profile_db_3gpp_umts_gprs_qos_params_type));
  memscpy((void *)&gprs_qos_params, info->len, info->buf,info->len);

  if (gprs_qos_params.precedence > 
      DS_PROFILE_3GPP_GPRS_QOS_PARAM_MAX_PRECEDENCE_CLASS)
  {

    DS_3GPP_MSG3_ERROR("Invalid Precedence class: %d in gprs req qos",
                       gprs_qos_params.precedence,0,0);
    return DB_RESULT_FAIL;
  }

  if (gprs_qos_params.delay > DS_PROFILE_3GPP_GPRS_QOS_PARAM_MAX_DELAY_CLASS)
  {
    DS_3GPP_MSG3_ERROR("Invalid Delay class: %d in gprs req qos",
                       gprs_qos_params.delay,0,0);
    return DB_RESULT_FAIL;
  }

  if (gprs_qos_params.reliability > 
      DS_PROFILE_3GPP_GPRS_QOS_PARAM_MAX_RELIABILITY_CLASS)
 {
    DS_3GPP_MSG3_ERROR("Invalid Reliablilty class: %d in gprs req qos",
                       gprs_qos_params.reliability,0,0);
    return DB_RESULT_FAIL;
 }

 if (gprs_qos_params.peak > 
     DS_PROFILE_3GPP_GPRS_QOS_PARAM_MAX_PEAK_THROUGHPUT_CLASS)
 {
   DS_3GPP_MSG3_ERROR("Invalid Peak throughput class: %d in gprs req qos",
                      gprs_qos_params.peak,0,0);
   return DB_RESULT_FAIL;
 }

 if (gprs_qos_params.mean > 
     DS_PROFILE_3GPP_GPRS_QOS_PARAM_MAX_MEAN_THROUGHPUT_CLASS)
 {
   DS_3GPP_MSG3_ERROR("Invalid Mean throughput class: %d in gprs req qos",
                      gprs_qos_params.mean,0,0);
   return DB_RESULT_FAIL;
 }

 return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_UMTS_QOS_PARAMS

DESCRIPTION
  The function validates umts qos parameters
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the umts qos parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_umts_qos_params
(
  ds_profile_db_ident_type        ident,
  const ds_profile_db_info_type    *info,
  const void *profile
)
{
  ds_profile_db_result_type ret_val = DB_RESULT_SUCCESS;
  ds_profile_db_3gpp_umts_qos_params_type umts_qos_params;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(info == NULL)
  {
    DS_3GPP_MSG3_HIGH("Info parameter is NULL",0,0,0);
    return DB_RESULT_FAIL;
  }
  /*----------------------------------------------------------------------- 
    Check if UMTS QOS params are non negative
   -----------------------------------------------------------------------*/
  memset(&umts_qos_params,0,sizeof(ds_profile_db_3gpp_umts_qos_params_type));
  memscpy((void *)&umts_qos_params, info->len, info->buf,info->len);

  if (umts_qos_params.traffic_class > (ds_profile_db_3gpp_umts_qos_tclass)DS_PROFILE_3GPP_TC_BACKGROUND)
  {
    DS_3GPP_MSG3_ERROR("UMTS QOS parameters are not valid",0,0,0);
    return DB_RESULT_FAIL;
  }

  if ( (umts_qos_params.max_ul_bitrate > DS_PROFILE_3GPP_QOS_PARAM_MAX_UL_BITRATE) ||
       (umts_qos_params.max_dl_bitrate > DS_PROFILE_3GPP_QOS_PARAM_MAX_DL_BITRATE) ||
       (umts_qos_params.gtd_ul_bitrate > DS_PROFILE_3GPP_QOS_PARAM_MAX_GTD_UL_BITRATE) ||
       (umts_qos_params.gtd_dl_bitrate > DS_PROFILE_3GPP_QOS_PARAM_MAX_GTD_DL_BITRATE) )
  {
    DS_3GPP_MSG3_ERROR("UMTS QOS parameters are not valid",0,0,0);
    return DB_RESULT_FAIL;
  }

  if (umts_qos_params.dlvry_order > (ds_profile_db_3gpp_umts_qos_dorder)DS_PROFILE_3GPP_DO_OFF)
  {
    DS_3GPP_MSG3_ERROR("Invalid Delivery ordr: %d in Req QoS",
                     umts_qos_params.dlvry_order,0,0);
    return DB_RESULT_FAIL;
  }

  if (umts_qos_params.max_sdu_size > DS_PROFILE_3GPP_QOS_PARAM_MAX_SDU_SIZE)
  {
    DS_3GPP_MSG3_ERROR("Invalid Max SDU size: %d in Req QoS",
                     umts_qos_params.max_sdu_size,0,0);
    return DB_RESULT_FAIL;
  }

  if (umts_qos_params.sdu_err > (ds_profile_db_3gpp_umts_qos_sdu_error)DS_PROFILE_3GPP_SDU_ERR_RATIO_MAX)
  {
    DS_3GPP_MSG3_ERROR("Invalid SDU err ratio: %d in Req QoS",
                     umts_qos_params.sdu_err,0,0);
    return DB_RESULT_FAIL;
  }

  if (umts_qos_params.res_biterr > (ds_profile_db_3gpp_umts_qos_res_ber)DS_PROFILE_3GPP_RESIDUAL_BER_6ENEG8)
  {
    DS_3GPP_MSG3_ERROR("Invalid Residual BER ratio: %d in Req QoS",
                     umts_qos_params.res_biterr,0,0);
    return DB_RESULT_FAIL;
  }

  if (umts_qos_params.dlvr_err_sdu > (ds_profile_db_3gpp_umts_qos_sdu_dlvr)DS_PROFILE_3GPP_DELIVER_ERR_SDU_NO_DELIVER)
  {
    DS_3GPP_MSG3_ERROR("Invalid Delivery err SDU: %d in Req QoS",
                     umts_qos_params.dlvr_err_sdu,0,0);
    return DB_RESULT_FAIL;
  }

  if(umts_qos_params.trans_delay > DS_PROFILE_3GPP_QOS_PARAM_MAX_TRANS_DELAY)
  {
    DS_3GPP_MSG3_ERROR("Invalid trans delay: %d in Req QoS",
                     umts_qos_params.trans_delay,0,0);
    return DB_RESULT_FAIL;
  }

  if(umts_qos_params.thandle_prio > DS_PROFILE_3GPP_QOS_PARAM_MAX_THANDLE_PRIORITY)
  {
    DS_3GPP_MSG3_ERROR("Invalid traffic handling priority: %d in Req QoS",
                     umts_qos_params.thandle_prio,0,0);
    return DB_RESULT_FAIL;
  }

  return ret_val;
}


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_LTE_QOS_PARAMS

DESCRIPTION
  The function validates lte qos parameters
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the lte qos parameters
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_lte_qos_params
(
  ds_profile_db_ident_type        ident,
  const ds_profile_db_info_type    *info,
  const void *profile
)
{
  ds_profile_db_result_type ret_val = DB_RESULT_SUCCESS;
#ifdef FEATURE_DATA_LTE
  ds_profile_db_3gpp_lte_qos_params_type lte_qos_params;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(info == NULL)
  {
    DS_3GPP_MSG3_HIGH("Info parameter is NULL",0,0,0);
    return DB_RESULT_FAIL;
  }
  /*----------------------------------------------------------------------- 
    Check if LTE QOS params are non negative
   -----------------------------------------------------------------------*/
  memset(&lte_qos_params,0,sizeof(ds_profile_db_3gpp_lte_qos_params_type));
  memscpy((void *)&lte_qos_params,info->len,info->buf,info->len);
 
  if ((lte_qos_params.max_ul_bit_rate > DS_PROFILE_3GPP_LTE_QOS_PARAM_MAX_UL_BITRATE) ||
      (lte_qos_params.max_dl_bit_rate > DS_PROFILE_3GPP_LTE_QOS_PARAM_MAX_DL_BITRATE) ||
      (lte_qos_params.g_ul_bit_rate  > DS_PROFILE_3GPP_LTE_QOS_PARAM_MAX_GTD_UL_BITRATE) ||
      (lte_qos_params.g_dl_bit_rate > DS_PROFILE_3GPP_LTE_QOS_PARAM_MAX_GTD_DL_BITRATE) )
  {
    DS_3GPP_MSG3_ERROR("LTE QOS parameters are not valid",0,0,0);
    ret_val = DB_RESULT_FAIL;
  }
#else
  DS_3GPP_MSG3_HIGH("LTE parameters could not be validated as LTE is not defined",0,0,0);
  ret_val = DB_RESULT_NOT_SUPPORTED;
#endif
  return ret_val;
}


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_H_COMP

DESCRIPTION
  The function validates Header compression 
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the lte qos parameters
 
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_h_comp
(
  ds_profile_db_ident_type          ident,
  const ds_profile_db_info_type    *info,
  const void                       *profile
)
{
  ds_profile_db_result_type        ret_val = DB_RESULT_FAIL;
  ds_umts_pdp_header_comp_e_type   h_comp  = DS_UMTS_PDP_HEADER_COMP_MAX;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - - - */
  if(info == NULL)
  {
    DS_3GPP_MSG3_ERROR("Info parameter is NULL",0,0,0);
    return DB_RESULT_FAIL;
  }

  memscpy((void *)&h_comp,info->len,info->buf,info->len);

  if((h_comp == DS_UMTS_PDP_HEADER_COMP_OFF)     ||
     (h_comp == DS_UMTS_PDP_HEADER_COMP_ON)      ||
     (h_comp == DS_UMTS_PDP_HEADER_COMP_RFC1144) ||
     (h_comp == DS_UMTS_PDP_HEADER_COMP_RFC2507) ||
     (h_comp == DS_UMTS_PDP_HEADER_COMP_RFC3095) )
  {
    DS_3GPP_MSG3_MED("Header compression is valid: %d",h_comp,0,0);
    ret_val = DB_RESULT_SUCCESS; 
  }

  return ret_val;

}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_PDP_TYPE

DESCRIPTION
  The function validates pdp type 
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the lte qos parameters
 
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_pdp_type
(
  ds_profile_db_ident_type        ident,
  const ds_profile_db_info_type    *info,
  const void *profile
)
{
  ds_profile_db_result_type ret_val = DB_RESULT_SUCCESS;
  ds_profile_db_3gpp_pdp_protocol  pdp_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - - - */
  if(info == NULL)
  {
    DS_3GPP_MSG3_HIGH("Info parameter is NULL",0,0,0);
    return DB_RESULT_FAIL;
  }

  memscpy((void *)&pdp_type,info->len,info->buf,info->len);

  if(pdp_type < 0)
  {
    DS_3GPP_MSG3_HIGH("PDP type is invalid:%d",pdp_type,0,0);
    ret_val = DB_RESULT_FAIL; 
  }

  return ret_val;
}
/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_GEN_PARAMS

DESCRIPTION
  The function validates general params
  Validate the internal parameters which currently don't have explicit identifiers
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_gen_params
(
  const ds_profile_db_3gpp_tech_params_type *profile
)
{
  ds_profile_db_result_type ret_val = DB_RESULT_FAIL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(profile == NULL)
  {
    DS_3GPP_MSG3_ERROR("Profile parameter is NULL",0,0,0);
    return ret_val;
  }

  if(profile->pdp_context.pdp_addr.ip_vsn <= 0)
  {
    DS_3GPP_MSG3_ERROR("PDP address IP version is invalid",0,0,0);
    return ret_val;
  }

  if((profile->tech_common_params.dns_addr.primary_ip_version <= 0) || 
     (profile->tech_common_params.dns_addr.primary_ip_version <= 0))
  {
    DS_3GPP_MSG3_ERROR("Tech params primary ip version is %d:"
                       "Tech params secondary ip version is %d",
                       profile->tech_common_params.dns_addr.primary_ip_version,
                       profile->tech_common_params.dns_addr.secondary_ip_version,
                       0);
    return ret_val;
  }

  ret_val = DB_RESULT_SUCCESS;

  return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_MAX_PDN_CONN_PER_BLK

DESCRIPTION
  The function validates maximum pdn connections per block
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the pdn connections per block
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_max_pdn_conn_per_blk
(
  ds_profile_db_ident_type        ident,
  const ds_profile_db_info_type    *info,
  const void *profile
)
{
  ds_profile_db_result_type ret_val = DB_RESULT_SUCCESS;
  uint32 max_pdn_conn_per_block = 0;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(info == NULL)
  {
    DS_3GPP_MSG3_HIGH("Info parameter is NULL",0,0,0);
    return DB_RESULT_FAIL;
  }
  /*----------------------------------------------------------------------- 
    Check if max pdn connections per block is under 1024
   -----------------------------------------------------------------------*/
  memscpy((void *)&max_pdn_conn_per_block,info->len,info->buf,info->len);

  if(max_pdn_conn_per_block > DS_PROFILE_MAX_PDN_CONN_PER_BLOCK)
  {
    DS_3GPP_MSG3_HIGH("MAX PDN CONN PER BLK OUT OF RANGE",
                      max_pdn_conn_per_block,0,0);
    return DB_RESULT_FAIL;
  }

  return ret_val;


}
/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_MAX_PDN_CONN_TIME

DESCRIPTION
  The function validates maximum pdn conn time
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the pdn connections per block
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_max_pdn_conn_time
(
  ds_profile_db_ident_type        ident,
  const ds_profile_db_info_type    *info,
  const void *profile
)
{
  ds_profile_db_result_type ret_val = DB_RESULT_SUCCESS;
  uint32 max_pdn_conn_time = 0;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(info == NULL)
  {
    DS_3GPP_MSG3_HIGH("Info parameter is NULL",0,0,0);
    return DB_RESULT_FAIL;
  }
  /*----------------------------------------------------------------------- 
    Check if max pdn connections per block is under 1024
   -----------------------------------------------------------------------*/
  memscpy((void *)&max_pdn_conn_time,info->len,info->buf,info->len);

  if(max_pdn_conn_time > DS_PROFILE_MAX_PDN_CONN_TIME)
  {
    DS_3GPP_MSG3_HIGH("MAX PDN CONN TIME OUT OF RANGE",max_pdn_conn_time,0,0);
    return DB_RESULT_FAIL;
  }

  return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_MAX_PDN_REQ_WAIT_TIME

DESCRIPTION
  The function validates maximum pdn req wait time
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the pdn connections per block
   
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE = otherwise
  
SIDE EFFECTS 
  None 
  
===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_max_pdn_req_wait_time
(
  ds_profile_db_ident_type        ident,
  const ds_profile_db_info_type    *info,
  const void *profile
)
{
  ds_profile_db_result_type ret_val = DB_RESULT_SUCCESS;
  uint32 max_pdn_conn_req_wait_time = 0;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(info == NULL)
  {
    DS_3GPP_MSG3_HIGH("Info parameter is NULL",0,0,0);
    return DB_RESULT_FAIL;
  }
  /*----------------------------------------------------------------------- 
    Check if max pdn connections per block is under 1024
   -----------------------------------------------------------------------*/
  memscpy((void *)&max_pdn_conn_req_wait_time,info->len,info->buf,info->len);

  if(max_pdn_conn_req_wait_time > DS_PROFILE_MAX_PDN_CONN_REQ_WAIT_TIME)
  {
    DS_3GPP_MSG3_HIGH("MAX PDN REQ WAIT TIME OUT OF RANGE",
                      max_pdn_conn_req_wait_time,0,0);
    return DB_RESULT_FAIL;
  }

  return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_VALIDATE_ROAMING_PDP_TYPE

DESCRIPTION
  The function validates roaming pdp type 
    
PARAMETERS: 
  *profile  --  Profile passed by profile DB.
  *info     --  Contains the roaming PDP parameters


DEPENDENCIES
  None.

RETURN VALUE 
  DB_RESULT_SUCCESS - success
  DB_RESULT_FAILURE - otherwise

SIDE EFFECTS 
  None 

===========================================================================*/
ds_profile_db_result_type ds_3gpp_profile_validate_roaming_pdp_type
(
   ds_profile_db_ident_type        ident,
   const ds_profile_db_info_type  *info,
   const void                     *profile
)
{
  ds_profile_db_result_type        ret_val = DB_RESULT_SUCCESS;
  ds_profile_db_3gpp_pdp_protocol  pdp_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - - - */
  if(info == NULL)
  {
    DS_3GPP_MSG3_HIGH("Info parameter is NULL",0,0,0);
    return DB_RESULT_FAIL;
  }

  memscpy((void *)&pdp_type,info->len,info->buf,info->len);

  switch( pdp_type )
  {
    case DB_3GPP_ROAMING_IPV4:
    case DB_3GPP_ROAMING_IPV6:
    case DB_3GPP_ROAMING_IPV4V6:
    case DB_3GPP_ROAMING_IP_MAX:
      ret_val = DB_RESULT_SUCCESS;
      break;

    default:
      DS_3GPP_MSG3_HIGH("PDP type is invalid:%d",pdp_type,0,0);
      ret_val = DB_RESULT_FAIL;
  }

  return ret_val;
} /* ds_3gpp_profile_validate_roaming_pdp_type */
