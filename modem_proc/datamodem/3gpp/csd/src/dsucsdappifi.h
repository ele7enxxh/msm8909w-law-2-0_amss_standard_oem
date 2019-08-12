#ifndef DSUCSDAPPIFI_H
#define DSUCSDAPPIFI_H
/*===========================================================================

             U M T S   C I R C U I T - S W I T C H E D   D A T A

                  A P P L I C A T I O N   I N T E R F A C E

                   I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This header file contains interfaces that are needed by UMTS Circuit- 
  Switched Data API implementation. 
  
Copyright (c) 2007 - 2008 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsucsdappifi.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/10/08   bs      Fixed compilation issues.
09/01/08   sa      Added modifications for CMI defeaturization.
06/28/07   ar      Initial version.

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"


#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )

#include "dsucsdappif.h"


/*===========================================================================

                      LOCAL DATA DECLARATIONS

===========================================================================*/

//#ifdef FEATURE_DATA_UCSD_UNIFIED_API

#ifdef FEATURE_MULTIMEDIA_FALLBACK
  #error code not present
#else
  #define DSUCSD_SUPPORTED_CALL_MODE_MASK \
               (DSUCSD_CALL_MODE_DATA)
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

/* Macro to validate the call mode (TRUE is error) */
#define VALIDATE_CALL_MODE( call_mode ) \
    ((call_mode & ~DSUCSD_SUPPORTED_CALL_MODE_MASK) != 0)
    
typedef struct ds_ucsd_client_info
{
  uint8                       call_types;
  ds_ucsd_call_event_cb_type  call_event_cb;
  void                       *user_info_ptr;
} ds_ucsd_client_info_type;


typedef struct ds_ucsd_client
{
  boolean                     in_use;
  ds_ucsd_client_id_type      client_id;
  ds_ucsd_client_info_type    info;
} ds_ucsd_client_type;

//#endif /* FEATURE_DATA_UCSD_UNIFIED_API */

/*===========================================================================

                          FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION DS_UCSD_APP_INIT

DESCRIPTION
  This function initializes the data structures for the client API.
  It should be called once by DS task during powerup.

DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
extern void ds_ucsd_app_init( void );


/*===========================================================================

FUNCTION DS_UCSD_APP_GET_DPATH_INFO

DESCRIPTION
  This function returns the client data path inforation.
        
DEPENDENCIES
   None
  
RETURN VALUE
   Pointer to data path structure.
    
SIDE EFFECTS
   None   
===========================================================================*/
extern ds_ucsd_dpath_info_type*  ds_ucsd_app_get_dpath_info( void );


//#ifdef FEATURE_DATA_UCSD_UNIFIED_API

/*===========================================================================

FUNCTION DS_UCSD_APP_FIND_CLIENT

DESCRIPTION
  This function attempts to find the specified client identifier in
  the client table.  If found, the record pointer is returned to the
  caller if a pointer was passed (NULL permitted).

DEPENDENCIES
  ds_ucsd_app_init() must have been previously invoked.
  
RETURN VALUE
  TRUE if client record was found, FALSE otherwise.
    
SIDE EFFECTS
  None

===========================================================================*/
extern boolean ds_ucsd_app_find_client
(
  const ds_ucsd_client_id_type client_id,
  const ds_ucsd_client_type  **client_pptr
);


/*===========================================================================

FUNCTION DS_UCSD_APP_NOTIFY_CLIENTS_BY_TYPE

DESCRIPTION
  This function traverses the client table to find those clients
  registered for a given call type.  For each record match, the
  registered callback function is invoked to post the event to the
  client.

DEPENDENCIES
  ds_ucsd_app_init() must have been previously invoked.
  
RETURN VALUE
  TRUE if one or more clients were notified, FALSE otherwise.
    
SIDE EFFECTS
  None

===========================================================================*/
boolean ds_ucsd_app_notify_clients_by_type
(
  const uint8                     call_type,
  const ds_ucsd_call_event_type  *event_ptr
);


/*===========================================================================

FUNCTION DS_UCSD_APP_NOTIFY_CLIENT

DESCRIPTION
  This function find a specified client in client table and on match,
  the registered callback function is invoked to post the event to the
  client.

DEPENDENCIES
  ds_ucsd_app_init() must have been previously invoked.
  
RETURN VALUE
  TRUE if one or more clients were notified, FALSE otherwise.
    
SIDE EFFECTS
  None

===========================================================================*/
boolean ds_ucsd_app_notify_client
(
  const ds_ucsd_client_id_type    client_id,
  const ds_ucsd_call_event_type  *event_ptr
);


/*===========================================================================

FUNCTION DS_UCSD_APP_INIT_DATAPATH

DESCRIPTION
  This function initializes a passed datapath structure based on the
  specified call instance.  Datapath is either via SIO/SMD or
  watermark queues.  It is an error condition to use watermarks with
  the remoted interface.

DEPENDENCIES
  None

RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void ds_ucsd_app_init_datapath
(
  ds_ucsd_call_tbl_entry_type  *call_info_ptr,
  ds_ucsd_datapath_type        *datapath_ptr
);

/*===========================================================================

FUNCTION DS_UCSD_LOOKUP_CM_CALLID

DESCRIPTION
   This function performs lookup in the call table, ds_ucsd_call_tbl[],
   to find the Call Manager call ID which correspons to the passed CS
   Data internal call instance ID. 
        
DEPENDENCIES
   None
  
RETURN VALUE
   CM call ID or NO_CALL_ID.
    
SIDE EFFECTS
   None   
===========================================================================*/
extern uint8 ds_ucsd_lookup_cm_callid
(
  const uint8 inst_id                              /* CSData call instance */
);

//#endif /* FEATURE_DATA_UCSD_UNIFIED_API */

#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */


#endif /* DSUCSDAPPIFI_H */
