/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      PDCOMM NV Items Module

GENERAL DESCRIPTION
  This module contains the PDCOMMS interface routines for the Non-Volatile Memory
  module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/pd_comms/src/pd_comms_nv_efs.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/13/10   ssheshad     Initial Release.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#include "pd_comms_nv_efs.h"
#include "msg.h"
#include "pdapi.h"
#include "stringl.h"


/* ensure the order of the items in this table is same as the item definitions */
cgps_nv_efs_reg_item_struct pdcomm_nv_reg_items[] =
{
  /* data location, item file name, size */

  /*CFPS_NV_EFS_PDCOMM_APN_PROFILES , */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "pdcomms/gnss_pd_comms_lbs_apn_profile_list", 
    sizeof(pdsm_pa_apn_profiles_type) * PDSM_PA_MAX_APN_PROFILES, 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},
    
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "pdcomms/gnss_pd_comms_xtra_apn_profile_list", 
    sizeof(pdsm_pa_apn_profiles_type) * PDSM_PA_MAX_APN_PROFILES, 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},


  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "pdcomms/gnss_pd_comms_emergency_lbs_apn_profile_list", 
    sizeof(pdsm_pa_apn_profiles_type) * PDSM_PA_MAX_APN_PROFILES, 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL},


  /* Default RSA Certificate Key length */
  {CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, "pdcomms/gnss_pd_comms_ssl_cert_key_length", 
    sizeof(uint16) , 0, CGPS_NV_EFS_REG_TYPE_INTEGRAL}
  /* add entries below this line and make sure you update the CGPS_NV_EFS_PDCOMMS_MAX_ITEM*/
};


  /*RAM Copy of the NV's*/

  typedef struct
  {      
    pdsm_pa_apn_profiles_type gnss_pd_comms_lbs_apn_profile_list[PDSM_PA_MAX_APN_PROFILES];
    pdsm_pa_apn_profiles_type gnss_pd_comms_xtra_apn_profile_list[PDSM_PA_MAX_APN_PROFILES];
    pdsm_pa_apn_profiles_type gnss_pd_comms_emergency_lbs_apn_profile_list[PDSM_PA_MAX_APN_PROFILES];
    uint16                    gnss_pd_comms_ssl_certificate_keysize;
  } pd_comms_nv_cached_struct_type;

  
#ifdef FEATURE_GNSS_NO_NV_WRITES
   #error code not present
#endif

  /*===========================================================================
  
  FUNCTION pd_comms_nv_load_default_values_to_cache
  
  DESCRIPTION
    This function is used to load default values to cache
    
  RETURN VALUE
    None
    
  DEPENDENCIES
   
  ===========================================================================*/
void pd_comms_nv_load_default_values_to_cache(void)
{

#ifdef FEATURE_GNSS_NO_NV_WRITES  
   #error code not present
#endif

}


/*===========================================================================

FUNCTION pd_comms_nv_efs_init

DESCRIPTION
  This function is used to initialize internal PDCOMM-NV data efs structure.
  
RETURN VALUE
  None
  
DEPENDENCIES
 
===========================================================================*/
void pd_comms_nv_efs_init(void)
{ 
  boolean ret_val = TRUE;
  ret_val =  cgps_nv_efs_init (CGPS_NV_EFS_PDCOMM, CGPS_NV_EFS_PDCOMMS_MIN_ITEM,
                               CGPS_NV_EFS_PDCOMMS_MAX_ITEM,
                               pdcomm_nv_reg_items);
  if (ret_val)
  {
    MSG_MED("PDCOMMS_NV_EFS: NV EFS init successful",0,0,0);
    pd_comms_nv_load_default_values_to_cache();
  }
  else
  {
    MSG_MED("SM_NV_EFS: NV EFS init failed",0,0,0);
  }
}

/*===========================================================================

FUNCTION pd_comms_nv_efs_reg_item_read

DESCRIPTION
  This function is used to read NV item. If the read fails, it sets it to
  default value.
  
RETURN VALUE
  None
  
DEPENDENCIES
 
===========================================================================*/
boolean pd_comms_nv_efs_reg_item_read(cgps_nv_efs_reg_item_type const item, 
                                      uint32 data_size,
                                      void *data_ptr,
                                      boolean force_read_from_hw
                                      )
{
  cgps_nv_efs_reg_rw_stat_enum_type status = CGPS_NV_EFS_REG_RW_STAT_INVALID;    
  boolean return_value = FALSE;
  boolean nv_read_needed = TRUE;

#ifdef FEATURE_GNSS_NO_NV_WRITES
  #error code not present
#endif  

  if(nv_read_needed)
  {
	  status =  cgps_nv_efs_reg_item_read(item, data_size, data_ptr, 
	                                      CGPS_NV_EFS_PDCOMMS_MIN_ITEM,
	                                      CGPS_NV_EFS_PDCOMMS_MAX_ITEM, pdcomm_nv_reg_items);

#ifdef FEATURE_GNSS_NO_NV_WRITES
    #error code not present
#endif
    MSG_MED("PDCOMM NV read from Hw",0,0,0);
  }
  else
  {
     MSG_MED("PDCOMM NV read from Cache",0,0,0);
  	status = CGPS_NV_EFS_REG_RW_STAT_OK;
  }

  if (status == CGPS_NV_EFS_REG_RW_STAT_OK )
  {
    MSG_MED("PDCOMM_NV_EFS: NV EFS file read succeeded for item = %d", item, 0, 0);
    return_value = TRUE;
  }
  return return_value;

}

/*===========================================================================

FUNCTION pd_comms_nv_efs_reg_item_write

DESCRIPTION
  This function is used to write an NV item. 
 
  
RETURN VALUE
  None
  
DEPENDENCIES
 
===========================================================================*/
boolean pd_comms_nv_efs_reg_item_write(cgps_nv_efs_reg_item_type const item, 
                                       uint32 data_size,
                                       void *data_ptr
                                       )
{
  cgps_nv_efs_reg_rw_stat_enum_type status = CGPS_NV_EFS_REG_RW_STAT_INVALID;    
  boolean return_value = TRUE;
  boolean nv_write_needed = TRUE;

#ifdef FEATURE_GNSS_NO_NV_WRITES
  #error code not present
#endif  

  if(nv_write_needed)  
  {
	  status =  cgps_nv_efs_reg_item_write(item, data_size, data_ptr, 
	                                       CGPS_NV_EFS_PDCOMMS_MIN_ITEM,
	                                       CGPS_NV_EFS_PDCOMMS_MAX_ITEM, pdcomm_nv_reg_items);
  }
  else
  {
  	status = CGPS_NV_EFS_REG_RW_STAT_OK;
  }  

  if (status == CGPS_NV_EFS_REG_RW_STAT_OK )
  {
    MSG_MED("PDCOMM_NV_EFS: NV EFS file write succeeded for item = %d", item, 0, 0);
    return_value = TRUE;
  }
  else
  {
    MSG_MED("PDCOMM_NV_EFS: NV EFS file write failed for item = %d", item, 0, 0);
    return_value = FALSE;
  }
  return return_value;

}

