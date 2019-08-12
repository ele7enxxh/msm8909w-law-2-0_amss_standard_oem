#ifndef GPSRUIM_H
#define GPSRUIM_H

#include "gps_variation.h"
#ifdef FEATURE_GPSONE
#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

    Position Determination RUIM interface Header File

DESCRIPTION
  This header file contains all the definitions necessary for using
  PD module as a client to RUIM interface.

  Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_ruim.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/24/14   rh      Add SIM hot swap support  
12/23/13   mj	      Remove FEATURE_DUAL_SIM
03/03/10   ns      DSDS support. Enable if DSDS enabled.
07/02/08   ank     Added LBS support for CDG OMH R-UIM Spec

===========================================================================*/

#include "mmgsdilib.h"
#include "uim.h"


#ifdef FEATURE_MMGSDI_SESSION_LIB
typedef struct  
{
  mmgsdi_subscription_ready_evt_info_type    subscription_ready;
}tm_ruim_sub_event_info_s_type;

typedef struct  
{
  mmgsdi_session_close_evt_info_type         session_close;
}tm_ruim_close_event_info_s_type;

#endif /* FEATURE_MMGSDI_SESSION_LIB */

typedef struct
{
  uint8 data_len;
  uint8 data[UIM_CDMA_SVC_TABLE_SIZE];
} tm_ruim_cdma_svc_table;

/*===========================================================================

FUNCTION  TM_RUIM_GSDI_INIT

DESCRIPTION
  GSDI (generic SIM driver interface) Initialization function. This function does the following:
  - Registers the client call back function for GSDI events,
  - Register as a client of MMGSDI.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tm_ruim_gsdi_init ( void );

/*===========================================================================

FUNCTION  TM_RUIM_CARRIER_NV_REFRESH_DONE

DESCRIPTION
  Called by TM task to notify TM-RUIM that Carrier-specific NV refresh done
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tm_ruim_carrier_nv_refresh_done ( void );

/*===========================================================================

FUNCTION  TM_RUIM_HANDLE_POST_E911

DESCRIPTION
  TM RUIM handling after E911 session
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tm_ruim_handle_post_e911 ( void );

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================

FUNCTION tm_gsdi_handle_sim_card_inserted_evt
                                        
DESCRIPTION
  This function issues a read to the CDMA service table to see if 3GPD is 
  supported on the R-UIM.  It is called during initialization.
  
PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE

                                                 
SIDE EFFECTS


===========================================================================*/ 
void tm_gsdi_handle_sim_card_inserted_evt(void);

/*===========================================================================

FUNCTION tm_gsdi_sim_handle_subscription_ready_evt
                                        
DESCRIPTION
  
PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE

                                                 
SIDE EFFECTS


===========================================================================*/ 
void tm_gsdi_sim_handle_subscription_ready_evt(void* p_data);


/*===========================================================================

FUNCTION tm_gsdi_handle_session_changed
                                        
DESCRIPTION
  
PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE

                                                 
SIDE EFFECTS


===========================================================================*/ 
void tm_gsdi_handle_session_changed_evt(void);


/*===========================================================================

FUNCTION tm_gsdi_handle_session_closed_evt
                                        
DESCRIPTION
  
PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE

                                                 
SIDE EFFECTS


===========================================================================*/ 
void tm_gsdi_handle_session_closed_evt(void* p_data);

/*===========================================================================

FUNCTION tm_gsdi_handle_client_reg_cb
                                        
DESCRIPTION
  
PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE

                                                 
SIDE EFFECTS


===========================================================================*/ 
void tm_gsdi_handle_client_reg_cb(void);



/*===========================================================================

FUNCTION  tm_gsdi_handle_lbs_svc_available

DESCRIPTION
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tm_gsdi_handle_lbs_svc_available(void);

#else
/*===========================================================================

FUNCTION tm_gsdi_sim_init_handler
                                        
DESCRIPTION
  This function issues a read to the CDMA service table to see if 3GPD is 
  supported on the R-UIM.  It is called during initialization.
  
PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE

                                                 
SIDE EFFECTS


===========================================================================*/ 
void tm_gsdi_sim_init_handler(void);

/*===========================================================================

FUNCTION TM_RUIM_CDMA_SVC_INIT_REPORT

DESCRIPTION
  Accept status reports from R-UIM and prints out message for CDMA SVC table 
  check


DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tm_ruim_cdma_svc_init_report(void *);

/*===========================================================================
FUNCTION tm_ruim_mmgsdi_read_cmda_svc_table

DESCRIPTION
  This function issues a read command to MMGSDI API to read the CDMA Svc tbl

ARGUMENTS
  

DEPENDENCIES
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tm_ruim_mmgsdi_read_cmda_svc_table(void);
#endif /* FEATURE_MMGSDI_SESSION_LIB */
#ifdef __cplusplus
}
#endif

#endif /*FEATURE_GPSONE */

#endif /* GPSRUIM_H */

