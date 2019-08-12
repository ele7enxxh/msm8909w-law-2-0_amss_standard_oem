#ifndef SVDODRV_H
#define SVDODRV_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        S I M U L T A N E O U S   V O I C E   &   D O   D R I V E R
                    
                           H E A D E R    F I L E

GENERAL DESCRIPTION
  This module contains definitions and declarations of the shared drivers
  between CDMA 1X and DO technologies.  Feature SVDO stands for simultaneous
  1X Voice and DO data.
  
EXTERNALIZED FUNCTIONS
  svdodrv_enable_rtc_to_sram - Calling technology is requesting an RTC source
    for the sample server
  svdodrv_disable_rtc_to_sram - Calling technology is releasing its own RTC
    source for the sample server
  svdodrv_enable_mod - Calling technology is requesting modulator resource
  svdodrv_disable_mod - Calling technology is releasing modulator resource
  svdodrv_is_client_rtc_enabled - Check if client's RTC is enabled or not

NOTE
  To conform to legacy naming convention, the terms DO and HDR are 
  intermittently used.

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/



/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/svdodrv.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/25/09   rkc     Removed check for FEATURE_SVDO and customer.h include.
09/08/09   rkc     Added svdodrv_is_client_rtc_enabled().
07/07/09   vlc     Created file.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "assert.h"

#include "msm.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*=========================================================================
      Typedefs
=========================================================================*/

/* SVDO (simultaneous 1x voice and DO data) driver clients are 1X technology
   and HDR technology.
*/
typedef enum
{
  SVDODRV_CLIENT_1X,
  SVDODRV_CLIENT_HDR,
  SVDODRV_CLIENT_MAX

} svdodrv_client_type;

/* Antenna chains (frequently referred to as receiver chains.)
*/
typedef enum
{
  SVDODRV_RXTX_CHAIN,          /* Transceiver 0 */
  SVDODRV_RX_CHAIN,            /* Receiver 1    */
  SVDODRV_RX_CHAIN_MAX,
  SVDODRV_RX_ANY
  /* Note: SVDODRV_RX_ANY is only allowed with svdodrv_is_client_rtc_enabled() */

} svdodrv_rx_chain_type;

/* Poseidon modem HW provides two RTCs, one for DO and one for 1X.  HW design
   requires that at any given time, only one of these RTCs can be used for all
   SRAMs on a given receive chain.  This necessitates the use of one RTC, either DO or
   1X for both the DO and 1X SRAMS which are on different carriers but on the same
   receiver chain.
*/
typedef enum
{
  SVDODRV_RTC_1X,              /* Use 1X RTC for all SRAMs */
  SVDODRV_RTC_HDR,             /* Use DO RTC for all SRAMs */
  SVDODRV_RTC_NO_CHANGE,       /* There is no change to RTC source */
  SVDODRV_RTC_OFF              /* Turn off RTC for all SRAMs */

} svdodrv_rtc_ret_type;


/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION SVDODRV_INIT 

DESCRIPTION 
  This function initializes variables used by the SVDO driver.  It should be
  called at power up.
  
DEPENDENCIES
  None.
 
PARAMETERS
  None.
 
RETURN VALUE 
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern void svdodrv_init (void);


/*===========================================================================
FUNCTION SVDODRV_ENABLE_RTC_TO_SRAM

DESCRIPTION
  Assigns the calling technology's RTC to the SRAMs.  Should be called
  when the technology wakes up.
 
  Per HW design, the following RTC selection is handled in this function:
  1.  If 1X is running on a carrier of an antenna chain, SRAMs for all DO and
      1x carriers on that antenna chain are run off of the 1x RTC.
  2.  If 1X is not running on an antenna chain, SRAM for all DO carriers on that
      antenna chain are run off of the DO RTC.
  
DEPENDENCIES
  RTC must be active.

PARAMETERS
  Either 1X or DO technology
  Antenna chain 

RETURN VALUE   
  RTC to be used for sample server

SIDE EFFECTS
  None.
===========================================================================*/
extern svdodrv_rtc_ret_type svdodrv_enable_rtc_to_sram
(
  svdodrv_rx_chain_type    ant_index,      
  svdodrv_client_type      svdo_rtc_client
);


/*===========================================================================
FUNCTION SVDODRV_DISABLE_RTC_TO_SRAM

DESCRIPTION
  Deassigns the calling technology's RTC to the SRAMs.  Should be called
  when the technology gets ready to sleep and before the RTC is turned off.
  This may cause a small glitch on the reception of the remaining active
  technology if it is DO.
  
DEPENDENCIES
  RTC must be active.

PARAMETERS
  Either 1X or DO technology
  Antenna chain

RETURN VALUE 
  RTC to be used for sample server 

SIDE EFFECTS
  None.
===========================================================================*/
extern svdodrv_rtc_ret_type svdodrv_disable_rtc_to_sram
(
  svdodrv_rx_chain_type    ant_idx,
  svdodrv_client_type      svdo_client
);



/*===========================================================================
FUNCTION SVDODRV_IS_CLIENT_RTC_ENABLED

DESCRIPTION
  This function returns TRUE if the given SVDO client's RTC is enabled on
  given rx chain, FALSE otherwise.
  
DEPENDENCIES
  None

PARAMETERS
  Rx Chain
  SVDO Client
  
RETURN VALUE 
  TRUE if the given SVDO client's RTC is enabled on rx chain, FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/

extern boolean svdodrv_is_client_rtc_enabled
(
  svdodrv_rx_chain_type    ant_idx,
  svdodrv_client_type      svdo_client
);


/*===========================================================================
FUNCTION SVDODRV_ENABLE_MOD

DESCRIPTION
  Enables (secures) the modulator resource for the calling technology.
  Should be called when the technology needs to transmit on the RL.
    
DEPENDENCIES
  None.

PARAMETERS
  Either 1X or DO technology.

RETURN VALUE
  

SIDE EFFECTS
  None.
===========================================================================*/


/*===========================================================================
FUNCTION SVDODRV_DISABLE_MOD

DESCRIPTION
  Disables (releases) the modulator resource for the calling technology.
  Should be called when the technology no longer needs to transmit on the RL.
  
DEPENDENCIES
  None.

PARAMETERS
  Either 1X or DO technology.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

#endif   /* SVDODRV_H */
