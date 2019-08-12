
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

  dsatsmsi_memory_lookup
    Converts the type of preferred memory storage into a required enum.

  dsatsmsi_send_async_indication_to_te
    If ATCOP get an unsolicited asynchronous event, This function formats 
    the data that needs to be sent to TE. It handles both SMS-DELIVERs and
    SMS-STATUS-REPORTs.

  dsatsmsi_get_mem_list
    Returns the current sms memory storage setting.
    It returns dsat_cpms_list[mem_index]/
               dsat707sms_qcpms_list[mem_index]
               depending on modes.

  dsatsmsi_domain_pref_to_srv
  Maps the wms_gw_domain_pref enum type to +CGSMS <service> number.

INITIALIZATION AND SEQUENCING REQUIREMENTS


   Copyright (c) 2002 - 2016 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatsmsu_ex.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/15/15   sc      Fixed issue in event handling to use subs_id provided in 
                   event info.
06/27/14   tk      Initial revision (created file for Dynamic ATCoP).

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>


#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
#include <stdio.h>
#include <string.h>

#include "dsati.h"
#include "dsatdl.h"
#include "msg.h"
#include "err.h"
#include "wms.h"
#include "dsatsmsi.h"
#include "dsm.h"

#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

#ifdef FEATURE_ETSI_SMS
#include "dsatetsictab.h"
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_CDMA_SMS
#include "dsat707sms.h"
#include "dsat707smsctab.h"
#endif /* FEATURE_CDMA_SMS */

#define VALID_CPMS(x) if((x <= CPMS_NONE)||(x >= CPMS_MAX))\
{\
  DS_AT_MSG0_ERROR("Invalid CPMS memory type");\
  return 0;\
}

/*===========================================================================
            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE
  This section contains reginal definitions for external variables 
  that are declared else where in ATCOP SMS SUB SYSTEM
===========================================================================*/
/*--------------------------------------------------------------------------
  import variable from other files in sms module
---------------------------------------------------------------------------*/

/* WMS client (ATCOP) ID */
extern wms_client_id_type dsatsmsi_client_id;

#ifdef FEATURE_ETSI_SMS_PS
typedef struct {
  wms_gw_domain_pref_e_type     pref;
  dsat_num_item_type            srv;
} domain_pref_tab_type;

const domain_pref_tab_type domain_pref_table[] =
{
  { WMS_GW_DOMAIN_PREF_CS_PREFERRED,    3 },
  { WMS_GW_DOMAIN_PREF_PS_PREFERRED,    2 },
  { WMS_GW_DOMAIN_PREF_CS_ONLY,         1 },
  { WMS_GW_DOMAIN_PREF_PS_ONLY,         0 }
};

const int domain_pref_table_size = ARR_SIZE(domain_pref_table);
#endif /* FEATURE_ETSI_SMS_PS */

#ifdef FEATURE_ETSI_SMS
const smsu_wms_mem_table_type 
    wms_memory_gw_table[] = 
  {
    { WMS_MEMORY_STORE_NV_GW,   "ME" },
    { WMS_MEMORY_STORE_NV_GW,   "MT" },
    { WMS_MEMORY_STORE_SIM,     "SM" },
    { WMS_MEMORY_STORE_SIM,     "SR" },
  };
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_CDMA_SMS
const smsu_wms_mem_table_type 
    wms_memory_is707_table[] = 
  {
    { WMS_MEMORY_STORE_NV_CDMA, "ME" },
    { WMS_MEMORY_STORE_NV_CDMA, "MT" },
    { WMS_MEMORY_STORE_RUIM,    "SM" },
    { WMS_MEMORY_STORE_RUIM,    "SR" },
  };
#endif /* FEATURE_CDMA_SMS */

/*-------------------------------------------------------------------------
            Function Definitions:
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION    DSATSMSI_SEND_ASYNC_INDICATION_TO_TE

DESCRIPTION
  If ATCOP get an unsolicited asynchronous event, This function formats 
  the data that needs to be sent to TE. It handles both SMS-DELIVERs and
  SMS-STATUS-REPORTs.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns number of bytes that are used in formatting the result buffer

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
void dsatsmsi_send_async_indication_to_te
(
  sms_mt_msg_e_type       msg_type,   /* Type of message       */
  wms_message_index_type  msg_index,  /* Index Number          */
  wms_memory_store_e_type mem_store,  /* Type of Memory Store  */
  dsati_operating_cmd_mode_type cmd_mode,
  sys_modem_as_id_e_type        subs_id
)
{
  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatsmsu_send_async_indication_to_te_fp))
  {
    dsatdl_vtable.dsatsmsu_send_async_indication_to_te_fp(msg_type, msg_index, mem_store, cmd_mode, subs_id);
  }

  return;
} /* dsatsmsi_send_async_indication_to_te */

/*===========================================================================

FUNCTION    DSATSMSI_MEMORY_LOOKUP

DESCRIPTION
  Converts the type of preferred memory storage into a required enum.
  
DEPENDENCIES
  None

RETURN VALUE
  returns the memory type enum:
  WMS_MEMORY_STORE_NV_GW        (ETSI only)
  WMS_MEMORY_STORE_SIM          (ETSI only)
  WMS_MEMORY_STORE_RUIM         (CDMA only)
  WMS_MEMORY_STORE_NV_CDMA      (CDMA only)

SIDE EFFECTS
  None

===========================================================================*/

wms_memory_store_e_type dsatsmsi_memory_lookup
( 
 int mem_val,                   /* value of the mem index to be mapped */ 
 cpms_mem_e_type mem_type,       /* which memory (<mem[1-3]>) to mapped */
 dsati_operating_cmd_mode_type op_mode 
)
{
  wms_memory_store_e_type return_val = WMS_MEMORY_STORE_NONE;
  const dsat_string_item_type * mem_valstr;
  int index;

  const smsu_wms_mem_table_type * wms_memory_table;
  const def_list_type           * mem_def;

  switch(op_mode)
  {
#ifdef FEATURE_ETSI_SMS
    case ETSI_CMD_MODE:
      wms_memory_table = wms_memory_gw_table;
      break;
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_CDMA_SMS
    case CDMA_CMD_MODE:
      wms_memory_table = wms_memory_is707_table;
      break;
#endif /* FEATURE_CDMA_SMS */
      
    default:
      /* please compiler */
      wms_memory_table = NULL;
      ERR_FATAL("wrong op_mode %d",(int)op_mode,0,0);
  }
  /* get the right mem_valstr */
  mem_def = dsatsmsi_get_mem_list(mem_type,op_mode);

  /* sanity check */
  if ( mem_def == NULL )
  {
    /* Error fatal already happened, klocwork doesn't know this */
    return return_val;
  }

  mem_valstr = mem_def->list_v[mem_val];

  /* look for the wms_memory by mem_str */
  for (index = 0; index < WMS_MEM_TABLE_SIZE; index++)
  {
    if(strcmp(wms_memory_table[index].mem_str, 
              (char *) mem_valstr) == 0)
    {
      return_val = wms_memory_table[index].wms_memory;
      break;
    }
  }

  return return_val;
} /* dsatsmsi_memory_lookup */


/*===========================================================================

FUNCTION    DSATSMSI_GET_MEM_LIST

DESCRIPTION
  Returns the current sms memory storage setting.
  It returns dsat_cpms_list[mem_index]/
             dsat707sms_qcpms_list[mem_index]
  depending on modes.

DEPENDENCIES
  None

RETURN VALUE
  Returns a def_list_type array.

SIDE EFFECTS
  None

===========================================================================*/
const def_list_type * dsatsmsi_get_mem_list
(
 cpms_mem_e_type mem_index,   /* which memory (<mem[1-3]) setting we are looking for */
  dsati_operating_cmd_mode_type cmd_mode
)
{
  const def_list_type           * mem_list;

  VALID_CPMS(mem_index);

#ifdef FEATURE_ETSI_SMS 
  if(cmd_mode == ETSI_CMD_MODE)
  {
    mem_list = &dsat_cpms_list[mem_index];
  }
  else
#endif /* FEATURE_ETSI_SMS */
#ifdef FEATURE_CDMA_SMS 
  if (cmd_mode == CDMA_CMD_MODE)
  {
    mem_list = &dsat707sms_qcpms_list[mem_index];
  }
  else
#endif /* FEATURE_CDMA_SMS */
  {
    DS_AT_MSG1_ERROR("We are in wrong mode: %d",cmd_mode);
    return 0;
  }

  return mem_list;

} /* dsatsmsi_get_mem_list */



#ifdef FEATURE_ETSI_SMS
/*===========================================================================

FUNCTION    DSATSMSI_ROUTE_CONFIG
DESCRIPTION
  Sets the SMS message routing configuration.
  
DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD: wms accepts the config route command
  DSAT_ERROR:     wms rejects the config route command

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsatsmsi_route_config
(
 dsat_num_item_type     mt_val, /* what type of deliver routing we are setting */
 dsat_num_item_type     ds_val,  /* what type of status report routing we are setting */
 sys_modem_as_id_e_type subs_id               /* Subscription id */
)
{
  dsat_result_enum_type result;
  wms_status_e_type status;
  wms_routes_s_type route;
  int tid = 100;
  int i;

  /* Set the Default Routing configuration */
  /* Class 0 messages default treatment is don't attempt to store */
  route.pp_routes[0].route = WMS_ROUTE_TRANSFER_AND_ACK;
  route.pp_routes[0].mem_store = 
    dsatsmsi_memory_lookup((dsat_num_item_type)dsatutil_get_val(
                DSATETSI_EXT_ACT_CPMS_ETSI_IDX,subs_id,2,NUM_TYPE), CPMS_MEM3,ETSI_CMD_MODE);

  for( i = 1 ; i < 5 ; i++ )
  {
    route.pp_routes[i].route = WMS_ROUTE_STORE_AND_NOTIFY;
    route.pp_routes[i].mem_store = 
      dsatsmsi_memory_lookup((dsat_num_item_type)dsatutil_get_val(
                 DSATETSI_EXT_ACT_CPMS_ETSI_IDX,subs_id,2,NUM_TYPE), CPMS_MEM3,ETSI_CMD_MODE);
  }

  /* Even for GSM only builds, we need to set the route for 
     WMS_MESSAGE_CLASS_CDMA to WMS_ROUTE_NO_CHANGE.  The reason:
     We will have a problem if we keep WMS_MESSAGE_CLASS_CDMA in 
     the CDMA feature flag, since that will not match the NV item 
     size. */
     
  route.pp_routes[5].route = WMS_ROUTE_NO_CHANGE;
  route.pp_routes[5].mem_store = WMS_MEMORY_STORE_NONE;

  if ( mt_val == 2 )
  {
    if ((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CSMS_IDX,subs_id,0,NUM_TYPE) == 0)
    {
      route.pp_routes[0].route = WMS_ROUTE_TRANSFER_AND_ACK;
      route.pp_routes[1].route = WMS_ROUTE_TRANSFER_AND_ACK;
      route.pp_routes[3].route = WMS_ROUTE_TRANSFER_AND_ACK;
      route.pp_routes[4].route = WMS_ROUTE_TRANSFER_AND_ACK;
    }
    else
    {
      if (TRUE == dsatme_is_thin_ui())
      {
        route.pp_routes[0].route = WMS_ROUTE_TRANSFER_ONLY;
      }
      else
      {
        route.pp_routes[0].route = WMS_ROUTE_TRANSFER_AND_ACK;
      }
      route.pp_routes[1].route = WMS_ROUTE_TRANSFER_ONLY;
      route.pp_routes[3].route = WMS_ROUTE_TRANSFER_ONLY;
      route.pp_routes[4].route = WMS_ROUTE_TRANSFER_ONLY;
    }
  }
  else if ( mt_val == 3 )
  {
    route.pp_routes[3].route = WMS_ROUTE_TRANSFER_ONLY;
  }
  else
  {
    /* please the lint */
  }

  /* Routing scheme:
      <ds> == 0 : No routing of STATUS-REPORT.
      <ds> == 1 : Direct routing of STATUS-REPORT.
      <ds> == 2 : Store and route of STATUS-REPORT. STATUS-REPORT is stored
                  in SIM only if there is a mapping MO SMS in SIM. No
                  notification is sent to TE is STATUS-REPORT is not stored.  
  */
  if ( ds_val == 2 || ds_val == 0 )
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
                                subs_id,
                                dsatsmsi_cmd_cb_func,
                                (void *) &tid,
                                &route
                              );
#else
  status = wms_cfg_set_routes (
                                dsatsmsi_client_id,
                                dsatsmsi_cmd_cb_func,
                                (void *) &tid,
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
}/* dsatsmsi_route_config */
#endif /* FEATURE_ETSI_SMS */


#ifdef FEATURE_ETSI_SMS_PS
/*===========================================================================

FUNCTION    DSATSMSI_DOMAIN_PREF_TO_SRV

DESCRIPTION
  Maps the wms_gw_domain_pref enum type to +CGSMS <service> number.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE : succeeds
  FALSE: fails

SIDE EFFECTS
  None
===========================================================================*/
boolean dsatsmsi_domain_pref_to_srv
( 
 wms_gw_domain_pref_e_type      pref,     /* wms domain pref enum      */
 dsat_num_item_type             * srv_ptr /* pointer to +CGSMS <service>    */
)
{
  int index;

  for(index = 0; index < domain_pref_table_size; index++)
  {
    if(domain_pref_table[index].pref == pref)
    {
      *srv_ptr = domain_pref_table[index].srv;
      return TRUE;
    }
  }

  return FALSE;
} /* dsatsmsi_domain_pref_to_srv */
#endif /* FEATURE_ETSI_SMS_PS */
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */
