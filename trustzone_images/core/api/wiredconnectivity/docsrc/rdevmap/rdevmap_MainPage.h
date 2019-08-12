#ifndef __RDEVMAP_MAINPAGE_H__
#define __RDEVMAP_MAINPAGE_H__

/*  
  ====================================================================
                             Edit History 
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/docsrc/rdevmap/rdevmap_MainPage.h#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $
  when      who   what, where, why 
  --------  ----  ----------------------------------------------------
  06/10/10  amv    Created
  
  ====================================================================
 
  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary and Confidential
 
  ==================================================================== 
*/ 

/**
@mainpage Rdevmap API Documentation

@section S1 Introduction

    The RDM API provides applications a consistent way of mapping services (or
    applications) to the shared serial devices (or ports) at runtime. Services
    register it's open and close callback function with RDM on powerup. RDM
    makes open callback with SIO port ID of default/requested RDM device ID for 
    service to open port on the device. When some other service makes request to 
    acquire shared device, Rdevmap will close the device, if its in use by other 
    service, before assigning it to requesting service. Rdevmap can also maintain
    persistency for device port mapping by restoring it from EFS file system on
    powerup.

@section S2 Theory of operation

        The RDM centralizes the logic for all applications requiring serial port
    mapping services. Applications wishing to close, open, or reassign a port
    for a given application can use the RDM API.

    Port mappings are sticky if FEATURE_RDM_ALWAYS_NON_PERSISTENT is enabled. If
    a service is successfully mapped to a device, then that service will open
    the same device at the next powerup. This applies to services mapped to the
    NULL device as well. This is only supported with EFS/FLASH.

    RDM stores the most recent service/port mappings in EFS. During
    initialization, RDM first looks for this information in EFS. If it does not
    get any such information, then it uses the compile-time defaults. Using this
    information, it then selectively calls the open routines for services that
    need access to respective ports. If a service was not mapped previously (was
    on the NULL device), then RDM will not call its open routine.

@section S3 Initialization

    To properly use RDM, a software system and applications should follow this
    sequence: 
    <ul>
    <li>1. rdm_init() initializes the RDM internal data structure first.
    <li>2. Services/applications call rdm_register_open_func() and
       rdm_register_close_func() to register their open() and close() routines,
       if they require holding only one port at any given time, or
       services/applications call rdm_register_func_multi_dev() to register
       their open() and close() routines if they wish to hold multiple ports any
       given time.
    <li>3. Applications call rdm_set_srv_compat_mask()/rdm_set_compatibility() to
       specify which ports they can use, unless the application is native to
       RDM. 
    <li>4. rdm_get_prev_ports() will read from EFS for the port mapping saved
       before the last power cycle of the phone, if EFS is used.
    <li>5. rdm_issue_opens() will open the default ports or the previous port
       mapping before the last power cycle for the applications.
    <li>6. From this moment on, an application can call rdm_assign_port() to assign
       a port for any given application.
    </ul>

@section S4 Task context and priority

    RDM does not have its own task; it maintains a state machine that will be
    traversed in the context of multiple tasks. When RDM is not involved in
    assigning a port, it is in the Idle state. When a valid request comes in,
    RDM moves into the Busy state. RDM returns to the Idle state when it has
    completed the current request.

*/


#endif /* #ifndef __RDEVMAP_MAINPAGE_H__ */
