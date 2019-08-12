#ifndef SIO_WRAPPER_H
#define SIO_WRAPPER_H
/*===========================================================================

                S E R I A L    I / O    S E R V I C E S

                  W R A P P E R    F U N C T I O N S

                       H E A D E R    F I L E

DESCRIPTION
  This file contains types and declarations associated with the DMSS
  Serial I/O Services.

Copyright (c) 2007-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/wiredconnectivity/sio/src/amss/sio_wrapper.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

mm/dd/yy   who     what, where, why
--------   ---     ----------------------------------------------------------
01/05/12   tl      Added USB_SER4 port.
06/23/09   amv     Added sio critical sections to replace INTLOCKs
01/11/07   rh      Created module.

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "sio.h"
#include "sio_priv.h"

#ifdef FEATURE_SMD
#include "smd_sio.h"
#endif

/*===========================================================================

                        FEATURE FLAG MAPPING

===========================================================================*/

#if (defined(FEATURE_HS_USB) && !defined(FEATURE_NO_USB_HW_ON_PROCESSOR))
#define SIO_USES_HS_USB
#endif

#if defined(FEATURE_USB) || defined(SIO_USES_HS_USB)
#define SIO_USES_FSHS_USB
#endif

#if (defined (FEATURE_USB_DIAG_NMEA) || defined(FEATURE_USB_DIAG)) && \
   !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
#ifdef SIO_USES_HS_USB
#define SIO_HAS_USB_SER3
#define SIO_HAS_USB_SER4
#endif
#endif

/* DSM payload type for Unescaped bytes for Async data */
#if defined(FEATURE_DATA_MM) || defined(FEATURE_GSM_PLT) || defined(FEATURE_WCDMA_PLT) || defined (FEATURE_CORE_SERVICE_ONLY) || defined(FEATURE_LTE_PLT)
#define SIO_PAYLOAD       DSM_PS_RAW_APP_PAYLOAD
#else 
#define SIO_PAYLOAD       PS_RAW_APP_PAYLOAD
#endif

#ifdef FEATURE_HTORPC_METACOMMENTS
  /*~ PARTIAL sio_ioctl_param */
  /*~ CASE SIO_IOCTL_CHANGE_BAUD_RATE sio_ioctl_param.rearm_autobaud */
  /*~ CASE SIO_IOCTL_CHANGE_BAUD_NOW sio_ioctl_param.rearm_autobaud */
  /*~ DEFAULT sio_ioctl_param.void */
#endif


/* FUNCTION : smd_bridge_sio_close_ext() a wrapper function for SMD_bridge_close_ext */
void smd_bridge_sio_close_ext(sio_port_id_type port_id, void (*close_func_ptr) (void * cb_data), void *ext_cb_data);

sio_port_id_type sio_smd_bridge_connect(sio_open_type *open_ptr);

sio_port_id_type sio_smd_bridge_control_connect(sio_open_type *open_ptr);

#ifndef FEATURE_SMD
void smd_sio_flush_tx_ext
( 
  sio_port_id_type      port_id,
  sio_vpu_func_ptr_type flush_cb,
  void                  *cb_data
);

void smd_sio_close_ext
( 
  sio_port_id_type      port_id,
  sio_vpu_func_ptr_type close_cb,
  void                  *cb_data
);
#endif
/*===========================================================================

FUNCTION sio_smd_port_close_cb 

@brief
  This function is the close callback function that is registered with SMD
  for bridge ports.  It de-registers with the SMD port manager, and calls
  the client callback if one is present. 
      
@dependencies
  port must be bridge port on dual procssor build
      
@return
  None
    
@sideeffects
  De-registers with SMD port manager
  
===========================================================================*/
#if defined (FEATURE_SMD) && defined (FEATURE_SMD_BRIDGE)
void sio_smd_bridge_port_close_cb
( 
  void  *cb_data
);
#endif

/*===========================================================================

                        INIT FUNCTION DECLARATIONS

===========================================================================*/
#ifndef FEATURE_UART_WRAPPER_FREE
uint32 sio_uart_driver_init(sio_register_device_driver_cb_type register_driver_cb_fn);
#endif
#ifndef FEATURE_FSUSB_WRAPPER_FREE
uint32 sio_fsusb_driver_init(sio_register_device_driver_cb_type register_driver_cb_fn);
#endif
#ifndef FEATURE_HSUSB_WRAPPER_FREE
uint32 sio_hsusb_driver_init(sio_register_device_driver_cb_type register_driver_cb_fn);
#endif
#ifndef FEATURE_SMD_WRAPPER_FREE
uint32 sio_smd_driver_init(sio_register_device_driver_cb_type register_driver_cb_fn);
#endif
#ifndef FEATURE_SMD_BRIDGE_WRAPPER_FREE
uint32 sio_smd_bridge_driver_init(sio_register_device_driver_cb_type register_driver_cb_fn);
#endif
#ifndef FEATURE_SMEM_WRAPPER_FREE
uint32 sio_smem_driver_init(sio_register_device_driver_cb_type register_driver_cb_fn);
#endif
#ifndef FEATURE_MMGPS_WRAPPER_FREE
uint32 sio_mmgps_driver_init(sio_register_device_driver_cb_type register_driver_cb_fn);
#endif
#ifndef FEATURE_APS_WRAPPER_FREE
uint32 sio_aps_driver_init(sio_register_device_driver_cb_type register_driver_cb_fn);
#endif
#ifndef FEATURE_BLUETOOTH_WRAPPER_FREE
uint32 sio_bt_driver_init(sio_register_device_driver_cb_type register_driver_cb_fn);
#endif


/*===========================================================================

FUNCTION sio_enter_crit_section 

@brief
  sio_enter_crit_section
      
@dependencies
  sio_init_crit_section() needs to be called first
      
@return
  None
    
@sideeffects
  None
  
===========================================================================*/
void sio_enter_crit_section(void);

/*===========================================================================

FUNCTION sio_leave_crit_section 

@brief
  sio_leave_crit_section
      
@dependencies
  sio_init_crit_section() needs to be called first
      
@return
  None
    
@sideeffects
  None
  
===========================================================================*/
void sio_leave_crit_section(void);

#endif  /* SIO_WRAPPER_H */
