#ifndef _PICTBRIDGE_API_H_
#define _PICTBRIDGE_API_H_

/*==============================================================================

             PictBridge API --- H E A D E R   F I L E

==============================================================================*/

/**
@file pictbridge_api.h

Contains the data structures and API for DPS wrapper functions providing a generic
PictBridge API that does not depend on a particular implementation.
  
Externalized functions:
 - pictbridge_connect_printer()
 - pictbridge_disconnect_printer()
 - pictbridge_register_callback()
 - pictbridge_configure_print_service()
 - pictbridge_get_capability()
 - pictbridge_get_job_status()
 - pictbridge_get_device_status()
 - pictbridge_start_job()
 - pictbridge_abort_job()
 - pictbridge_continue_job()
      
Initialization and sequencing requirements:\n
pictbridge_connect_printer() must be called before using any other functions
exposed by this API.
*/

/*=============================================================================
  Copyright (c) 2006-2010 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

  EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
    $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/pictbridge_api.h#1 $
    
    when      who   what, where, why
    --------  ---   ---------------------------------------------------------
    06/23/10  sw    (Tech Pubs) Edited/added Doxygen comments and markup.
    11/16/06  dhk   First revision of file  
    
=============================================================================*/


/*=============================================================================

  INCLUDE FILES FOR MODULE
  
=============================================================================*/
#include "hsu_dps_defs.h"


/** @ingroup hsusb_qc_api 
@{ */


/*=============================================================================
Typedefs
=============================================================================*/


/*=============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/


/*=============================================================================
FUNCTION    pictbridge_connect_printer
=============================================================================*/
/**
Initiates a spoof-disconnect and re-enumerates the device as a Still Image
Capture Device (SICD).\ The process could take several seconds.

This function sends a Printer Connected notification to the client through a 
registered callback function. 

@dependencies
None.

@sideeffects
None.

@return
None.
*/
uint32 pictbridge_connect_printer(void);

/*=============================================================================
FUNCTION    pictbridge_disconnect_printer
=============================================================================*/
/**
Initiates a spoof-disconnect and restores the original enumeration.\ The
process could take several seconds.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
uint32 pictbridge_disconnect_printer(void);


/*==============================================================================
FUNCTION pictbridge_configure_print_service
===========================================================================*/
/**
Allows OEMs to specify a vendor name, product name, and serial number.

This function can be called before receiving the Printer Connected
notification. Exchanging DPS_ConfigurePrintService with the printer is
performed automatically after connecting to the printer successfully.

In DPS terms, after the DPS discovery is finished, DPS_ConfigurePrintService is 
performed automatically. If pictbridge_connect_printer() is not called until
the DPS discovery is finished, the default vendor name, product name, and
serial number are sent to the printer.

@param *vendor_name   [IN] -- Vendor name.
@param *product_name  [IN] -- Product name.
@param *serial_number [IN] -- Serial number as a string.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
uint32 pictbridge_configure_print_service
(
    const char * vendor_name,
    const char * product_name,
    const char * serial_number
);


/** PictBridge dynamic composition switching operation status.
*/
typedef enum
{
    PB_RDM_DONE,        /**< &nbsp; */
    PB_RDM_NOT_ALLOWED, /**< &nbsp; */  
    PB_RDM_DEVMAP_BUSY, /**< &nbsp; */
    PB_RDM_APP_BUSY     /**< &nbsp; */
} pb_rdm_change_status_t;


/** Pictbridge notifications.
*/
typedef enum
{
    PICTBRIDGE_DPS_DISCOVERY,     /**< &nbsp; */  
    PICTBRIDGE_PRINTER_CFG,       /**< &nbsp; */  
    PICTBRIDGE_JOB_STUTUS,        /**< &nbsp; */  
    PICTBRIDGE_DEVICE_STATUS,     /**< &nbsp; */  
    PICTBRIDGE_GET_CAPABILITY,    /**< &nbsp; */  
    PICTBRIDGE_GET_JOB_STUTUS,    /**< &nbsp; */  
    PICTBRIDGE_GET_DEVICE_STATUS, /**< &nbsp; */  
    PICTBRIDGE_RDM_CHANGE_STATUS, /**< &nbsp; */  
    PICTBRIDGE_PRINTER_CONNECT,   /**< &nbsp; */  
    PICTBRIDGE_PRINTER_DISCONNECT /**< &nbsp; */  
} pictbridge_notifiy_t;

typedef void (*pictbridge_notify_cb_t)(void * pUser, pictbridge_notifiy_t type, void * data);

uint32 pictbridge_register_callback
(
    void * pClientData,
    pictbridge_notify_cb_t pictbridge_cb
);

/* Very thin pass-through wrapper functions  */
int pictbridge_start_job(hsu_dps_job_config_t *config, hsu_dps_job_info_t *info, int count);
int pictbridge_abort_job(uint32 abort_style);
int pictbridge_continue_job (void);

int pictbridge_get_capability(hsu_dps_capability_t cap_type, uint32 paper_size);
int pictbridge_get_job_status(void);
int pictbridge_get_device_status(void);

boolean pictbridge_handle_mode_change(pb_rdm_change_status_t status);

/** @} */ /* endgroup hsusb_qc_api */

#endif /* _PICTBRIDGE_API_H_ */
