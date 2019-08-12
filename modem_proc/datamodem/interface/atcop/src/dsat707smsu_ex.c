
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( S M S  C O M M A N D S 
                  U T I L I T Y )
                
                P R O C E S S I N G

GENERAL DESCRIPTION
  This module is used by SMS AT commands. It mainly contains utility functions
  that are used in executing the sms commands. These functions are used 
  intenally in SMS module.

EXTERNALIZED FUNCTIONS INTERNAL TO ATCOP SMS UNIT
  dsat707smsi_route_config
    Sets the SMS message routing configuration for CDMA mode.

   Copyright (c) 2003 - 2014 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707smsu_ex.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   tk      Initial revision (created file for Dynamic ATCoP).

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

/* this file should be included only for SMS for CDMA MODE */
#ifdef FEATURE_CDMA_SMS
#include <stdlib.h>

#include "dsati.h"
#include "dsatsmsi.h"
#include "dsat707smsctab.h"

#include <stringl/stringl.h>

/*===========================================================================
                        DEFINES AND VARIABLES
===========================================================================*/

/* Variable to find out what command is being processed currently */
extern dsat_sms_cmd_s_type   dsatsmsi_pres_cmd;

/* WMS client (ATCOP) ID */
extern wms_client_id_type dsatsmsi_client_id;

/*-------------------------------------------------------------------------
            Function Definitions:
-------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION    DSAT707SMSMI_ROUTE_CONFIG
DESCRIPTION
  Sets the SMS message routing configuration for CDMA mode.
  
DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD: wms accepts the config route command
  DSAT_ERROR:     wms rejects the config route command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsat707smsi_route_config
(
  dsat_num_item_type mt_val, /* what type of deliver routing we are setting */
  dsat_num_item_type ds_val  /* what type of status report routing */
)
{
  dsat_result_enum_type result;
  wms_status_e_type status;
  wms_routes_s_type route;
  int i;

  /* leave the GW message classes untouched */
  for( i = 0 ; i < (int)WMS_MESSAGE_CLASS_CDMA ; i++ )
  {
    route.pp_routes[i].route = WMS_ROUTE_NO_CHANGE;
    route.pp_routes[i].mem_store = WMS_MEMORY_STORE_NONE;
  }
  
  route.pp_routes[WMS_MESSAGE_CLASS_CDMA].route = WMS_ROUTE_STORE_AND_NOTIFY;
  route.pp_routes[WMS_MESSAGE_CLASS_CDMA].mem_store = 
  dsatsmsi_memory_lookup((int)dsatutil_get_val(
            DSAT707_SMS_QCPMS_IDX,0,CPMS_MEM3,NUM_TYPE), CPMS_MEM3,CDMA_CMD_MODE);

/* If mt_val is 2 we Donot Store the message but route it to TE */
  if ( mt_val == 2 )
  {
    route.pp_routes[WMS_MESSAGE_CLASS_CDMA].route = WMS_ROUTE_TRANSFER_AND_ACK;
  }

  /* Routing scheme:
      <ds> == 0 : No routing of STATUS-REPORT.
      <ds> == 1 : Direct routing of STATUS-REPORT.
      <ds> == 2 : Store and route of STATUS-REPORT. STATUS-REPORT is stored
                  in SIM only if there is a mapping MO SMS in SIM. No
                  notification is sent to TE is STATUS-REPORT is not stored.  
  */
  if ( ds_val == 2 )
  {
    /* this boolean maps to direct routing of STATUS-REPORT */
    route.transfer_status_report = FALSE;
  }
  else
  {
    route.transfer_status_report = TRUE;
  }

  /* Now set the routing configuration */
#ifdef FEATURE_DUAL_SIM
  status = wms_cfg_ms_set_routes (
                                dsatsmsi_client_id,
                                dsat_get_current_1x_subs_id(),
                                dsatsmsi_cmd_cb_func,
                               (void *) &dsatsmsi_pres_cmd,
                                &route
                              );
#else
  status = wms_cfg_set_routes (
                               dsatsmsi_client_id,
                               dsatsmsi_cmd_cb_func,
                               (void *) &dsatsmsi_pres_cmd,
                               &route
                               );
#endif /* FEATURE_DUAL_SIM */
  if ( status == WMS_OK_S )
  {
    result = DSAT_ASYNC_CMD;
  }
  else
  {
    result = DSAT_ERROR;
  }

  return result;
}/* dsat707smsi_route_config */

#endif /* FEATURE_CDMA_SMS */
