/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 


This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.
*/


/**@file controllers.h

Holds all controllers supported by the Jungo implementation.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/controllers.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/21/10  sw    (Tech Pubs) - edited/added Doxygen comments and markup.
    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com 
**************************************************************************/ 

#ifndef _CONTROLLERS_H_
#define _CONTROLLERS_H_


/** @ingroup hsusb_jungo_api 
@{ */

/* Host Controllers */
#define controller_type_xxx                     0x0000
#define controller_type_isp176x                 0x0001
#define controller_type_isp1362                 0x0002
#define controller_type_isp116x_local           0x0003
#define controller_type_isp116x_pci             0x0103
#define controller_type_ehci_local              0x0004  /**< EHCI controller. */
#define controller_type_ehci_pci                0x0104
#define controller_type_ohci_local              0x0005
#define controller_type_ohci_pci                0x0105
#define controller_type_uhci_local              0x0006
#define controller_type_uhci_pci                0x0106
#define controller_type_ahci_local              0x0007
#define controller_type_ahci_pci                0x0107
#define controller_type_td243hc                 0x0008  /**< TDI Host controller. */
#define controller_type_td243hc_rev2            0x0009  /**< TDI Host controller rev2. */

/* Device Controllers */
#define controller_type_netchip                 0x0501
#define controller_type_atmel                   0x0502
#define controller_type_tdi_4x                  0x0503  /**< ChipIdea Device controller. */
#define controller_type_isp_1582                0x0504
#define controller_type_omapv1030               0x0505
#define controller_type_td243fc                 0x0506  /**< TDI Function controller. */
#define controller_type_td243fc_rev2            0x0507  /**< TDI Function controller rev2. */
#define controller_type_snps_usb3_dcd           0x0508  /**< SNPS USB3.0 Device controller. */

/** Dummy value for the template DCD */
#define controller_type_dcd_name                0x05FF

/* Transceiver Controllers */
#define controller_type_pmic                    0x0703

/** Dummy value for the template TCD. */
#define controller_type_tcd_name                0x07FF

/* OTG Controllers */
#define controller_type_tdi_4x_otg              0x0903 /**< OTG controller. */

/** @} */ /* end_group hsusb_jungo_api */

#endif
