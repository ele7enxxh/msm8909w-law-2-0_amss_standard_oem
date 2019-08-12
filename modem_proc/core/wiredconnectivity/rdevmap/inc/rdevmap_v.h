#ifndef RDEVMAP_V_H
#define RDEVMAP_V_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      R U N T I M E   D E V I C E   M A P P E R   S E R V I C E S 

                       H E A D E R    F I L E
                            

GENERAL DESCRIPTION
  This file contains types and declarations associated with the DMSS
  Runtime Device Mapper (RDM).  
  
EXTERNALIZED FUNCTIONS

  rdm_init()
    Initializes the Runtime Device Mapper Services. This function must
    be called before other functions of this module.

  rdm_get_prev_ports()
    Read in the port mappings last know by RDM from EFS.

  rdm_issue_opens()
    RDM issues opens to SRVCs based on last mapping before power down. If there
    is no EFS, the default mapping of DS->U1 and DIAG->U2 (if U2 defined) is
    performed.

  rdm_assign_port()
    Request RDM assign a specified application to the specified device.

  rdm_assign_port_tmp()
    Similar to rdm_assign_port(), except the change is not recorded in NV.

  rdm_close_device()
    This function will close a device used by a service.

  rdm_notify()
    Notify RDM of request completion/status.

  rdm_get_device()
    Query RDM for the device that is currently assigned to the specified
    service (if any).

  rdm_get_multi_dev()
    Query the Runtime Device Mapper for the ports/devices currently being 
    used by the specified application/service.

  rdm_get_service()
    Query the Runtime Device Mapper for the application/service that is 
    currently assigned to the specified port/device (if any).

  rdm_get_valid_ports()
    Query the RDM for the valid devices the specified application can map to.

  rdm_register_open_func()
    Function for Service Tasks to register their open function. This open 
    function is what RDM calls to instruct a Service to open a specified port.

  rdm_register_close_func()
    Function for Service Tasks to register their close function. This close 
    function is what RDM calls to instruct a Service to close it's open port.

  rdm_register_func_multi_dev()
    Function for Service Tasks to register their open/close functions.  The 
    functions are what RDM calls to instruct a Service to open/close device.

  rdm_get_device_str()
    Query the RDM for the ASCII string that corresponds to the 
    rdm_device_enum_type specified.

  rdm_is_srv_holding_dev()
    Test if a service is using a device.

  rdm_set_srv_compat_mask()
    Set device compatible mask for the service from 
    (RDM_LAST_STATIC_SRVC + 1) to (RDM_SRVC_MAX - 1). It tells RDM which 
    device this service can use.

  rdm_is_srv_dev_compat()
    Test if a service and a device is compatible. That is whether the service
    can use the device.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  rdm_init() must be called prior to any of the other functions of
  this module.
                 
Copyright (c) 2000-2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/COMMON/vcs/rdevmap.h_v   1.11.1.1   07 May 2003 18:43:06   wxie  $
  $Header: //components/rel/core.mpss/3.5.c12.3/wiredconnectivity/rdevmap/inc/rdevmap_v.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/09   amv     Moved public reference to sio.h (CMI #2)
11/20/08   amv     split private content from rdevmap.h for CMI implemenation VU.
10/08/07   ar      Added RDM_SMD_DATA11_DEV.
10/11/06   rh      Adding support for FEATURE_HS_USB
08/23/06   sg      Added rdm service for brew extended device.
06/26/06   gd      Cleaned up code - removed usbdc.h inclusion which is not required.
06/19/06   bt      Added FEATURE_USB_BREW_EXTENDED_DEVICE support. 
06/19/06   sb      Added the function to control clk and modem ISR for USB
06/12/06   dsn     Added new BT SoC service type, exported apis for MSM7500
05/01/06   vp      Changes for enableing multiple ports for data.
01/04/06   ptm     Restore SMEM code, removed RPCMP and DIAG_MP services and
                   devices.
08/19/05   cp      Support for APS virtual port
06/05/05   TMR     Merged in MSM7500 code
03/08/05   cc      Merged in OSIFDev_Dec04 branck. 
01/11/05   cc      Duplicate USB  port entries with old names in 
                   rdm_device_enum_type for better backwards compatibility.
12/23/04   cc      Rename USB_CDC, USB_DIAG, USB_NMEA device to be USB_MDM, USB_SER1
                   and USB_SER2 respectively. 
11/24/04   cc      Added support for RDM_USB_NET_WWAN_DEV and RDM_DATA_WWAN_SRVC.
10/20/04   ptm     added RPC MP service.
10/20/04   adm     added DIAG MP service and SMEM DIAG and RPC devices.
09/22/04   ar      Merge serializer support under FEATURE_DATA_SERIALIZER.
04/20/04   pn      Added support for BT_SPP port. 
04/20/04   pn      Added support for BT_SPP port.
07/01/03   cc      added RDM_BREW_SRVC into serice enum type as an OEM service. 
04/08/02   wxie    Protect RDM state in critical section, add capability 
                   to open multiple ports for a service, add new service 
                   easily, and other internal RDM changes.
02/21/03   cc      Added FEATURE_USB_DIAG_NMEA support. 
07/15/02   ry      Added support for the LSM_CSD port
06/17/02   cc      Removes feature wrapping around RDM_RPC_SRVC in 
                   rdm_service_enum_type. This is a part of CR23030 fix.
02/06/02   cc      Add third UART supporting. 
01/09/02   cc      Add ONCRPC service supporting.
12/04/01   cc      Change the function name rdm_data_got_at$qcdmg() 
                   to rdm_data_got_atqcdmg() to avoid future headach.
08/06/01   cc      Merge gpsOne rdevmap.h -r 1.2 to add NMEA supporting. 
03/26/01   dl      Add support for SMEM_DS port.
12/21/00   dwp     Add support for EFS.
11/27/00   dwp     Comment updates.
11/16/00   dwp     Added UI interfaces and updated comments.
11/14/00   dwp     Created module.

===========================================================================*/


/*===========================================================================

                            INCLUDE FILES

===========================================================================*/

#include "core_variation.h"
#include "sio.h"                   /* Serial I/O Services.                 */
#ifdef FEATURE_RUNTIME_DEVMAP /* { */

#include "rdevmap.h"

/**---------------------------------------------------------------------------
  Command type directives
---------------------------------------------------------------------------*/
typedef enum
{
  RDM_CLOSE_PORT,                 /**< Close a serial port */
  RDM_OPEN_PORT                   /**< Open a serial port  */
} rdm_command_enum_type;

#ifdef FEATURE_USB_ON_APPS_PROC
/*===========================================================================

FUNCTION usbotg_turn_on_modem_isr_control               EXTERNALIZED FUNCTION

@brief
  This function sets modem usb ISR and calls USB clock disable.
  
@dependencies
  None

@return

@sideeffects

===========================================================================*/
void usbotg_turn_on_modem_isr_control(void);
#endif /*FEATURE_USB_ON_APPS_PROC*/

#endif /* } FEATURE_RUNTIME_DEVMAP */
#endif /*RDEVMAP_V_H*/
