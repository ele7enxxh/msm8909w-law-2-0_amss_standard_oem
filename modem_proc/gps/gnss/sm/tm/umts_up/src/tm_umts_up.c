/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  SUPL sub-module / TM core Interface

GENERAL DESCRIPTION
  This file implements UMTS_UP sub-module / TM core interface: functions owned
  by the UMTS_UP sub-module and called by TM core.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/14/06   LT      Initial version

============================================================================*/

#include "gps_variation.h"
#include "customer.h"   /* Customer configuration file */
#ifdef FEATURE_CGPS_UMTS_UP

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "msg.h"
#include <limits.h>
#include "tm_prtl_iface.h"
#include "tm_umts_up_supl_api.h"
#include "tm_umts_common_utils.h"

/* a structure that contains pointer to API functions UMTS-UP provides to TmCore,
   for handling TmCore messages to UMTS-UP  */
tm_prtl_cb_s_type          tm_umts_up_prtl_iface_table;


static void tm_umts_up_init_prtl_iface_table(void);


/*===========================================================================

FUNCTION tm_umts_up_prtl_iface_init

DESCRIPTION
  This function is used by TmCore which calls it when TM task starts, 
  initiating the UMTS_UP submodule.
  
DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/

void tm_umts_up_prtl_iface_init(void)
{

  MSG_MED("tm_umts_up_init", 0, 0, 0);

  /* init. TmCore's api functions */
  tm_umts_up_init_prtl_iface_table(); 
  
  /* register umts_up with tmCore */
  (void) tm_prtl_reg (TM_PRTL_TYPE_UMTS_UP, &tm_umts_up_prtl_iface_table);  

  /* read in umts common NV items */
  tm_umts_common_utils_read_nv();

  /* also, use this opportunity to init. SUPL submodule */
  tm_umts_up_supl_init();

} 



/*===========================================================================

FUNCTION tm_umts_up_init_prtl_iface_table

DESCRIPTION
  This function initializes the UMTS_UP protocol interface table with pointers
  to the functions TmCore will call. 
  
DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/

static void tm_umts_up_init_prtl_iface_table(void)
{

  /* QQQ: read NV_item here to determine which UMTS_UP protocol is to be used.
          for now, only SUPL is available */

  /********************** if SUPL ***********************************************/
  tm_umts_up_prtl_iface_table.start_sess_req_fp   = tm_umts_up_supl_start_sess_req_handler; 
  tm_umts_up_prtl_iface_table.stop_sess_req_fp    = tm_umts_up_supl_stop_sess_req_handler;
  tm_umts_up_prtl_iface_table.sess_req_data_fp    = tm_umts_up_supl_sess_req_data_handler;
  tm_umts_up_prtl_iface_table.sess_info_fp        = tm_umts_up_supl_sess_info_handler; 
//TDB Aries41 merge  change begin
#ifdef FEATURE_CGPS_PDCOMM
  tm_umts_up_prtl_iface_table.data_pipe_handle_fp = tm_umts_up_supl_data_pipe_handler; 
#endif  
//TBD Aries41 merge change end
  tm_umts_up_prtl_iface_table.timer_cb_fp         = tm_umts_up_supl_timer_cb_handler;
  tm_umts_up_prtl_iface_table.event_cb_fp         = tm_umts_up_supl_event_cb_handler;
  tm_umts_up_prtl_iface_table.general_query_fp    = tm_umts_up_supl_general_query_handler;
  tm_umts_up_prtl_iface_table.gm_event_handle_fp  = tm_umts_up_supl_gm_event_handler; 


  /* else if PreSUPL *************************************************************/

}

#endif /* ifdef FEATURE_CGPS_UMTS_UP */


