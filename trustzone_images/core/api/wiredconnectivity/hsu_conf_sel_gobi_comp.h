#ifndef _HSU_CONF_SEL_GOBI_COMP_H_
#define _HSU_CONF_SEL_GOBI_COMP_H_

/*=============================================================================

     High Speed USB Composition Utilities 
        H E A D E R   F I L E
=============================================================================*/
/**
@file hsu_conf_sel_gobi_comp.h

Includes Product ID (PID) definitions for Gobi&tm;.

PID definitions for compositions that do not use IAD, but enumerate with a 
single interface per function instead

*/
/*=============================================================================    
Copyright (c) 2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: 
  $DateTime: 
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  06/22/10 lo  (Tech Pubs) Edited/added Doxygen comments and markup.
  02/15/10 sm  Initial version
    
=============================================================================*/

/** @ingroup hsusb_qc_api
    @{
*/

/*=============================================================================
  Forward Declarations
=============================================================================*/

/*=============================================================================
  Constants
=============================================================================*/

/*=============================================================================
  HS-USB Product IDs
=============================================================================*/
/*
 * Product IDs for compositions that do not use IAD, but enumerate with a 
 * single interface per function instead. 
 */
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MS_MDM_NET      0x9000
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MDM_NET         0x9209
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MDM             0x9002
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MDM_NET_SER3    0x9003
#define HSU_PROD_ID_SINGLE_IF_DIAG_MDM                  0x9004
#define HSU_PROD_ID_SINGLE_IF_DIAG_MDM_NET              0x920A
#define HSU_PROD_ID_SINGLE_IF_DIAG_MS                   0x9006

#define HSU_PROD_ID_SINGLE_IF_NET_DIAG_MDM              HSU_PROD_ID_SINGLE_IF_NET_DIAG_MDM_NMEA
#define HSU_PROD_ID_SINGLE_IF_NET_DIAG_NMEA             HSU_PROD_ID_SINGLE_IF_NET_DIAG_MDM_NMEA
#define HSU_PROD_ID_SINGLE_IF_NET_DIAG                  HSU_PROD_ID_SINGLE_IF_NET_DIAG_MDM_NMEA

/** HSU_PROD_ID_SINGLE_IF_NET_DIAG_MDM_NMEA and HSU_PRODUCT_ID_NON_AMSS define on custhsusb_6295.h 
*/

/** @} */ /* endgroup hsusb_qc_api */

#endif /* _HSU_CONF_SEL_GOBI_COMP_H_ */
