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


/**@file drivers.h

Holds definitions for initalization functions required for the various 
controllers and drivers supported.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/drivers.h#1 $
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

#ifndef _DRIVERS_H_
#define _DRIVERS_H_

#include <jos.h>


/** @ingroup hsusb_jungo_api 
@{ */

/* Host Controller drivers */
jresult_t ehci_pci_init(void);
jresult_t ehci_local_init(void);
jresult_t ohci_pci_init(void);
jresult_t ohci_local_init(void);
jresult_t uhci_pci_init(void);
jresult_t ahci_local_init(void);
jresult_t isp176x_init(void);
jresult_t isp116x_pci_init(void);
jresult_t isp116x_local_init(void);
jresult_t isp1362_init(void);
jresult_t td243hc_init(void);
jresult_t td243hc_rev2_init(void);

/* Device Controller drivers */
jresult_t netchip_init(void);
jresult_t atmel_init(void);
jresult_t tdi_4x_init(void);
jresult_t isp_1582_init(void);
jresult_t omapv1030_init(void);
jresult_t td243fc_init(void);
jresult_t td243fc_rev2_init(void);
jresult_t dcd_name_init(void);
jresult_t snps_usb3_dcd_init(void);

/* OTG Controllers drivers */
jresult_t tdi_4x_otg_init(void);

/* Transciever Controller Drivers */
jresult_t pmic_init(void);

/* Usb drivers */
jresult_t uhub_init(void);
jresult_t ucompdev_init(void);
jresult_t ugen_init(void);
jresult_t umass_init(void);
jresult_t jkbd_init(void);
jresult_t jhid_general_init(void);
jresult_t jcdc_ecm_init(void);
jresult_t jserial_init(void);
jresult_t jloopback_init(void);
jresult_t jprinter_init(void);
jresult_t jiccd_init(void);
jresult_t jcdc_eem_init(void);
jresult_t jaudio_init(void);
/* HSU addition */
jresult_t hsu_ecm_cd_init(void);
/* End of HSU addition */
/* Embedded HS host electrical test driver */
jresult_t jetest_init(void);

jresult_t utest_init(void);

jresult_t iccd_app_init(void);
void iccd_app_uninit(void);
jresult_t cdc_eem_init(void);
void cdc_eem_uninit(void);

jresult_t mass_empty_init(void);
void mass_empty_uninit(void);

int mass_test_init(void);
void mass_test_uninit(void);

jresult_t audio_sample_init(void);
void audio_sample_uninit(void);

jresult_t audio_loopback_init(void);
void audio_loopback_uninit(void);

/** @} */ /* end_group hsusb_jungo_api */
    
#endif
