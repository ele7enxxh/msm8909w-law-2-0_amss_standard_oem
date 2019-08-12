
#ifndef LSMP_API_H
#define LSMP_API_H

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

    Location Session Manager and Protocols Header File

DESCRIPTION
  This header file contains definitions necessary to interface with LSMP module

  Copyright (c) 2007 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/inc/lsmp_api.h#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/28/14   ah      Added handling of Set Premium Services indication status
12/24/13   mj      Add new lsmp param type for device mode (SS/DSDS/DSDA/TSTS)
07/30/12   mj      Initial file creation and define types for reading premium service NV

===========================================================================*/


#include "comdef.h"    /* Definition for basic types and macros */
#include "sys.h"
#include "gps_common.h"

/* ========================================================================
** ================ LSMP API TYPEDEFS ===========================
** ========================================================================
*/

/* Parameter value types used for repsonses.
   A value type need to be added for every new parameter */
typedef uint32 lsmp_premium_services_param_type;
typedef enum{  
  LSMP_DEVICE_MODE_NA   			      = 0, /** Invalid. Could not retrieve device info from Policy manager */
  LSMP_CORE_DEVICE_MODE_SS   		      = 1, /** Single SIM mode */  
  LSMP_CORE_DEVICE_MODE_DSDS 			  = 2, /** Dual SIM, Dual Standby. Only one SUB supports VOICE and DATA */  
  LSMP_CORE_DEVICE_MODE_DSDA_VOICE        = 3, /** Dual SIM, Dual Active. Two SUBs can support VOICE simultaneously */  
  LSMP_CORE_DEVICE_MODE_DSDA_VOICE_DATA   = 4, /** Dual SIM, Dual Active. Two SUBs can support VOICE and DATA simultaneously */  
  LSMP_CORE_DEVICE_MODE_TSTS 			  = 5, /** Triple SIM, Triple Standby. Only one SUB support VOICE and DATA */  
  LSMP_CORE_DEVICE_MODE_NUM_MODES  		       /** Must always be after last valid mode */
}lsmp_device_mode_e_type;


typedef struct
{
  uint8 					 lsmp_no_of_sims;
  uint8						 lsmp_no_of_active_subs;
  lsmp_device_mode_e_type    lsmp_device_mode;
  sys_overall_feature_t      lsmp_overall_device_feature;
  sys_subs_feature_t		 lsmp_subs_feature[TM_CM_MAX_NO_SUBS];
  sys_ue_mode_e_type		 lsmp_subs_ue_mode[TM_CM_MAX_NO_SUBS];
} lsmp_device_config_param_s_type;


/* New param types need to go above this line */

#define LSMP_PREMIUM_SERVICES_TYPE_DISABLE        0 /* Disable sensors */
#define LSMP_PREMIUM_SERVICES_TYPE_ENABLE_BASIC   1 /* Enable sensors with basic functionality */
#define LSMP_PREMIUM_SERVICES_TYPE_ENABLE_PREMIUM 2 /* Enable sensors with premium functionality */

/* Parameter types for request. The response will contain the param
   value of above types
*/
typedef enum
{
  LSMP_PREMIUM_SERVICES_TYPE,	
  LSMP_DEVICE_CONFIG_TYPE,	
  LSMP_PARAM_TYPE_MAX=0x10000000		
} lsmp_get_param_request_type;

/* Request message type */
typedef struct lsmp_get_msg
{
  lsmp_get_param_request_type	param;			/* Which parameter */
  os_ThreadIdEnumType   	    thread_id;		/* thread id to which response is sent */
  uint32					    msg_id;			/* msg id the response ipc msg should contain */
  uint32						param_value_size; /* size of parameter value */
} lsmp_get_msg_type;
  

/*===========================================================================

FUNCTION lsmp_get_param

DESCRIPTION
    Request to read param value from tm_core.
 
DEPENDENCIES 
   
 
PARAMETERS
   parm         	: Specifies the type of parameter in request
   thread_id    	: Specifies the thread id to which response needs to be sent via
				  	  an ipc message with value
   msg_id       	: Specifies the message id that's associated with this ipc message
   param_value_size : Size of the parameter value   
               
 
RETURN VALUE
   TRUE        		: If the request to read param value is successful
   FALSE        	: If the request to read param value is not successful

SIDE EFFECTS 
  
===========================================================================*/

boolean lsmp_get_param(lsmp_get_msg_type msg);

/*~ FUNCTION lsmp_get_param */

/*===========================================================================

FUNCTION lsmp_send_premium_service_ind_status

DESCRIPTION
    Request to send QMI indication status to tm_core.

DEPENDENCIES

PARAMETERS
   status         	: Specifies success (TRUE) or failure (FALSE)

RETURN VALUE
   TRUE        		: If the request to send ind status is successful
   FALSE        	: If the request to send ind status is not successful

SIDE EFFECTS

===========================================================================*/

boolean lsmp_send_premium_service_ind_status(boolean status);

#ifdef __cplusplus
}
#endif

#endif /* PDAPI_H */
