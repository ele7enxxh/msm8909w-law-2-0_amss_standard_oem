#ifndef WLE_COMMON_H
#define WLE_COMMON_H
/*==============================================================================

                  GTP-WIFI-LP Location Engine Common

GENERAL DESCRIPTION
   This file contains constants, structures and common utility functions prototypes
   necessary for WLM and NF-LE.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  
  Copyright (c) 2015 QUALCOMM Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

==============================================================================*/


/*==============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
Version Control


when          who     what, where, why
--------      ---     ----------------------------------------------------------
05/01/15       dc     Initial Checkin
==============================================================================*/

/*==============================================================================

                          INCLUDE FILES FOR MODULE

==============================================================================*/

#include "wle_api.h"

/*==============================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================

                                  CONSTANTS

==============================================================================*/

/*==============================================================================

                                    MACROS

==============================================================================*/

// Do not change this number since it affects logs
#define WLE_MAC_ADDR_LENGTH  6
// Do not change this number since it affects RPC
#define WLE_MAX_REPORTED_APS_PER_LOG_MSG 50
#define WLE_SSID_STR_LENGTH 32


/*==============================================================================

                          STRUCTURES AND ENUMERATIONS

==============================================================================*/

typedef enum
{
  C_WLE_SCAN_TYPE_E_NONE  = 0,
  C_WLE_SCAN_TYPE_E_DISC,       /* disc scan info */
  C_WLE_SCAN_TYPE_E_ASYNC_DISC, /* async disc scan info */
  C_WLE_SCAN_TYPE_MAX,
  C_WLE_SCAN_TYPE_SIZE = 0xFF  /* to fix enum size as int8 */
} wle_scan_e_type;

/* WLE (LOWI) AP information */
typedef struct {  
    uint64             t_mac_addr;                // Associated MAC Address
    int32              l_meas_age_mSec;            // LOWI measurement age in milliseconds
    int32              l_rssi0pt5dBm;
    uint16             w_channel;                 // Channel on which beacon was received.    
    char               ssid[WLE_SSID_STR_LENGTH];
} wle_ap_info_struct_type;

/* AP scan list information */
typedef struct {
  wle_scan_e_type  e_ap_info_type;             // Type of AP scan info.  
  uint8   u_num_of_aps;
  wle_ap_info_struct_type z_ap_info[WLE_MAX_REPORTED_APS_PER_LOG_MSG];
} wle_ap_set_struct_type;

/*==============================================================================

                      FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

/*===========================================================================
FUNCTION wle_GetEndianArchitecture

DESCRIPTION
  This function returns the endianness of the modem processor architecture

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  C_GDT_BIG_ENDIAN   : If MP has a big endian architecture
  C_GDT_LITTLE_ENDIAN: If MP has a little endian architecture

SIDE EFFECTS
  None.

===========================================================================*/
uint8 wle_GetEndianArchitecture(void);

#endif /* WLE_COMMON_H */
