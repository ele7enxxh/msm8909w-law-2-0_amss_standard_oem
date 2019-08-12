#ifndef DS3G_STATS_H
#define DS3G_STATS_H
/*===========================================================================

                3 G   D A T A   S E R V I C E S   M A N A G E R

                           H E A D E R   F I L E

DESCRIPTION
  This file contains functions and definitions exported by 3G Dsmgr to other
  Data Services software units, namely: the top-level task dispatcher, ATCoP
  and Mode-Specific Handlers.

  Copyright (c) 2001-2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/ds3gstats.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/02/15   ms      Introduced this file for modem power statistics calculation

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ds3gmgr.h"
#include "rex.h"
#include "cm.h"
#include "cm_dualsim.h"
#include "sys.h"
#include "dsm.h"
#include "dstask_v.h"
#include "dsat_v.h"
#include "dsmgr_api.h"
#include "ds3gsubsmgr.h"
#include "ds_sys.h"
#include "ds3g_ext.h"
#include "ps_sys_event.h"
#include "sleep_stats.h"
#include "mcpm_stats.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

extern ds3g_hdlr_func_tbl_type  *ds3gi_hdlr_tbl[SYS_SYS_MODE_MAX]
                                                 [DS_NUM_CALL_TYPES];

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================
FUNCTION      DS3G_STATS_INIT_COMPLETE_CMD

DESCRIPTION   Called once at mobile power-up.  Initializes all statsistic 
              variables and call back functions 
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds3g_stats_init_complete_cmd();

/*===========================================================================
FUNCTION      DS3GI_CONFIG_MODEM_POWER_STATS

DESCRIPTION   This function is called from PS framework when client wants to 
              configure modem power statistics 
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_config_modem_power_stats
(
   ps_sys_tech_enum_type                          tech_type,
   ps_sys_event_enum_type                         event_name,
   ps_sys_subscription_enum_type                  ps_subs_id,
   void                                          *event_info_ptr,
   void                                          *user_data_ptr
);

/*===========================================================================
FUNCTION      DS3GI_QUERY_MODEM_POWER_STATS

DESCRIPTION   This function is called from PS framework when client asks for 
              power statistics information 
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_query_modem_power_stats
(
   ps_sys_tech_enum_type                          tech_type,
   ps_sys_event_enum_type                         event_name,
   ps_sys_subscription_enum_type                  ps_subs_id,
   void                                          *event_info_ptr,
   void                                          *user_data_ptr
);

/*===========================================================================
FUNCTION      DS3G_CONFIG_MODEM_POWER_STATS

DESCRIPTION   This function invokes the mode handlers to refresh/update 
              the actual throughput rate for all active calls for the
              current sampling interval

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_config_modem_power_stats
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id,
  boolean                     enable_modem_power_stats
);

/*===========================================================================
FUNCTION      DS3G_QUERY_MODEM_POWER_STATS

DESCRIPTION   This function check if LTE is registered as modem power stats 
              provider, if LTE is available request LTE to send async report
              otherwise start producing report right away.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_query_modem_power_stats
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id
);

/*===========================================================================
FUNCTION      DS3G_REPORT_MODEM_POWER_STATS

DESCRIPTION   This function gets power statistics for all lower layer, caches 
              last POWER_STATS_CACHE_SIZE values and process the information
              and send to ps framework

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_report_modem_power_stats
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id
);


#endif /* DS3G_STATS_H */
