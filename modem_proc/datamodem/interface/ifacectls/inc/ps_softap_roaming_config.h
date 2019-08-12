#ifndef PS_SOFTAP_ROAMING_CONFIG_H
#define PS_SOFTAP_ROAMING_CONFIG_H
/*===========================================================================

                PS_SOFTAP_ROAMING_CONFIG.H

DESCRIPTION
   This is NAT iface autoconnect roaming header file.

Copyright (c) 2010 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/ifacectls/inc/ps_softap_roaming_config.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "cm.h"
#include "sys.h"

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================

                            DATA DECLARATIONS

===========================================================================*/
#define NAT_IFACE_ROAM_IND_FILE_MAX_LEN   256

/*---------------------------------------------------------------------------
  Subset of the CM SS info structure needed by SoftAP
---------------------------------------------------------------------------*/
typedef struct
{
   uint64                       changed_fields;
   sys_srv_status_e_type        srv_status;
   sys_roam_status_e_type       roam_status;
   boolean                      hdr_hybrid;
   sys_srv_status_e_type        hdr_srv_status;
   sys_roam_status_e_type       hdr_roam_status;
}ps_softap_roaming_cm_ss_info_type;

/*---------------------------------------------------------------------------
  CM info
---------------------------------------------------------------------------*/
typedef struct
{
  cm_client_id_type                               cm_client_id;
  ps_softap_roaming_cm_ss_info_type               ss_info;
  boolean                                         inited;
  sys_roam_status_e_type                          ss_roaming_status;
} ps_softap_roaming_cm_info_type;


/*---------------------------------------------------------------------------
  Eri roam ind file info
---------------------------------------------------------------------------*/
typedef struct
{
  uint16      len;
  uint8       data[NAT_IFACE_ROAM_IND_FILE_MAX_LEN];
} ps_softap_roaming_eri_roam_ind_info_type;

/*---------------------------------------------------------------------------
  EFS autoconnect roam value
---------------------------------------------------------------------------*/
extern boolean autoconnect_roam_trigger;

/*---------------------------------------------------------------------------
  Structure that holds the SOFTAP roaming configuration
---------------------------------------------------------------------------*/
extern ps_softap_roaming_cm_info_type   ps_softap_roaming_config;

/*---------------------------------------------------------------------------
  Structure that holds eri info
---------------------------------------------------------------------------*/
extern ps_softap_roaming_eri_roam_ind_info_type  ps_softap_eri_roam_config;

/*===========================================================================

                            FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
  FUNCTION PS_SOFTAP_ROAMING_CONFIG_INIT()

  DESCRIPTION
   Open the EFS autoconnect roam value file and read the value
    
  PARAMETERS
    None

  RETURN VALUE
    None
        
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ps_softap_roaming_config_init
( void );

/*===========================================================================
  FUNCTION PS_SOFTAP_ERI_CONFIG_INIT()

  DESCRIPTION
   Open the EFS ERI values file and read the values
    
  PARAMETERS
    None

  RETURN VALUE
    None
        
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ps_softap_eri_config_init
( void );

/*===========================================================================
FUNCTION PS_SOFTAP_CM_INIT()

DESCRIPTION 
  Register for CM SS events and activate the CM client. 

DEPENDENCIES
  None

PARAMETERS 
  None 

RETURN VALUE
  0 - SUCCESS
 -1 - ERROR
===========================================================================*/
int32 ps_softap_cm_init
( void );

/*===========================================================================
  FUNCTION PS_ROAMING_ENABLE_AUTOCONNECT_POST_CMD()

  DESCRIPTION
  Posts a cmd to DCC to enable autoconnect in roaming
    
  PARAMETERS
    None
 
  RETURN VALUE
    None
        
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ps_roaming_enable_autoconnect_post_cmd
( void );

/*===========================================================================
  FUNCTION PS_ROAMING_DISABLE_AUTOCONNECT_POST_CMD()

  DESCRIPTION
  Posts a cmd to DCC to disable autoconnect in roaming
    
  PARAMETERS
    None
 
  RETURN VALUE
    None
        
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ps_roaming_disable_autoconnect_post_cmd
( void );

#ifdef __cplusplus
}
#endif

#endif /* PS_SOFTAP_ROAMING_CONFIG_H */
