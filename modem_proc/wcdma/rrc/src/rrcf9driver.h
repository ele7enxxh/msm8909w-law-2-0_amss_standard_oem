#ifndef RRCF9DRIVER_H
#define RRCF9DRIVER_H
/*===========================================================================

            M S M 5 2 0 0  F 9  ( I N T E G R I T Y   E N G I N E ) 
                  E X T E R N A L   H E A D E R   F I L E

DESCRIPTION

  This header file contains all the external definitions for 
  The F9 (Integrity Protection Engine) driver.
  
  Copyright (c) 2002-2003, 2007-2008 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcf9driver.h_v   1.4   18 Jun 2002 18:14:26   kiranc  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcf9driver.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who   what, where, why
--------   ---   ----------------------------------------------------------
01/06/14   sr      Made changes to extend Security API to Support Kasumi algorithm
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
09/05/12   md      Mainlined feature flags FEATURE_RRC_INTEGRITY_SW_ENGINE,
                   FEATURE_WCDMA_A2_POWER_COLLAPSE and FEATURE_WCDMA_NIKEL_SW
12/05/11   rl      Removed the A2 API calls
11/15/11   sks     Made the changes for A2 power optimization.
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
05/07/10   ss      Fixed compiler warnings.
05/29/09   kp      Added support for new SNOW3g security Algo & single cipher algo usage across 
                   access stratum layers.
05/08/09   ss      Updated Copyright Information
06/17/02   kc      Added interfaces to handle Integrity Modify in SMC cmd.
05/15/05   rj      Modified coding style and naming conventions for SW
                   integrity engine
05/07/02   rj      Added function prototypes to implement Integrity
                   Protection in Software.
04/22/02   kc      Changed interfaces and fixed bugs discovered during
                   Integration testing. 
04/10/02   kc      Created Module from RAM's initial file.Base build for 
                   Integrity Support. 

===========================================================================*/

#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "rrcmmif.h"
#include "uecomdef.h"
/*===========================================================================

                DEFINITIONS AND DECLARATIONS

===========================================================================*/

typedef enum
{
  RRC_INTEGRITY_DIR_UPLINK,  /* Uplink message        */
  RRC_INTEGRITY_DIR_DOWNLINK /* Downlink message      */
}rrc_integrity_direction_type;

                  

/*===========================================================================

  FUNCTION rrcf9_init_key_values

  DESCRIPTION

    This function performs initialization for the F9 device
    for each RRC connection. Each time an RRC connection
    is eastablished, once the input parameters are availabe
    this function should be called.

    The following are initialized by this function -
  
    Integrity Key - 128 bit key used by the F9 engine. The
    key is different for CS connections and PS connections.
  
   
  DEPENDENCIES

    None.

  RETURN VALUE

    None

===========================================================================*/
void rrcf9_init_key_values
( 
  byte *cs_key_value,     /* Integrity Key value for PS domain */
  byte *ps_key_value      /* Integrity Key value for CS domain */
);
/*===========================================================================

  FUNCTION rrcf9_init_fresh_per_connection

  DESCRIPTION

    This function performs initialization for the F9 device
    for each RRC connection. Each time an RRC connection
    is eastablished, once the input parameters are availabe
    this function should be called.

    The following are initialized by this function -
  
    FRESH - FRESH is a 32 bit value unique to a UE. It's given
    by the UTRAN in the security mode command.
   
   
  DEPENDENCIES

    None.

  RETURN VALUE

    None

===========================================================================*/
void rrcf9_init_fresh_per_connection
( 
  uint32 fresh_value      /* Value of Fresh */
);
/*===========================================================================

  FUNCTION RRCF9_GENERATE_MAC_FOR_MESSAGE

  DESCRIPTION

    This function passes the encoded RRC message and the
  additional parameters that are unique to the message 
  to the integrity engine. It returns the generated
  MAC-I value.
  
  This function is to be used for both downlink and 
  uplink messages.
  

  DEPENDENCIES

    None.

  RETURN VALUE

    A uint32 value that represents the MAC-I (or XMAC-I) for
  this message.

===========================================================================*/
uint32 rrcf9_generate_mac_for_message
( 
  uint32 count_i,                           /* COUNT-I value for the RB */
  rrc_integrity_direction_type dir,         /* Direction - uplink or downlink message */
  uint8 *msg,                               /* Pointer to the encoded message.    */
  uint32 msg_length,                        /* Length of the message in bits */
  rrc_cn_domain_identity_e_type  cn_domain,  /* Key for which domain_id to be used */
  uecomdef_wcdma_integrity_protection_algorithm_e_type integrity_algo /*integrity protection algorithm*/
);
#endif /* RRCF9DRIVER_H */
