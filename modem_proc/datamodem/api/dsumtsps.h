#ifndef DSUMTSPS_H
#define DSUMTSPS_H
/*===========================================================================

  U M T S (W C D M A / G P R S )   P A C K E T - S W I T C H E D   D A T A

                    E X T E R N A L   H E A D E R   F I L E

  DESCRIPTION
  This file contains external data declarations and function prototypes for 
  UMTS (wcdma / gprs) Packet-Switched Data Services.

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2009-2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/dsumtsps.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/11   ttv     Added changes needed for SESSION_PARAMS IOCTL and NSAPI api.
01/25/11   ss      SU API defeaturization and removal of customer.h.
08/31/09   ss      New header file for SU level apis.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "sys.h"

#include "dsumtspdpreg.h"
#include "ps_iface_defs.h"
#include "ps_acl.h"
#include "ps_iface_ioctl.h"

/*---------------------------------------------------------------------------
  Maximum Number of Packet data calls supported.
---------------------------------------------------------------------------*/
#define DSUMTSPS_MAX_PKT_CALLS    SYS_MAX_PDP_CONTEXTS

/*---------------------------------------------------------------------------
  Maximum Number of Secondary PDP contexts per primary supported.
---------------------------------------------------------------------------*/
#define DSUMTSPS_MAX_SEC_PER_PRI  SYS_MAX_SEC_PDP_CONTEXT_PER_PRIMARY

/*---------------------------------------------------------------------------
  Maximum Number of PS interfaces supported. There will be one PS interface
  for each set of primary call + DSUMTSPS_MAX_SEC_PER_PRI secondary calls.
  PS interface is initiated along with a primary context.
---------------------------------------------------------------------------*/
#define DSUMTSPS_MAX_PS_IFACES    SYS_MAX_PRIMARY_PDP_CONTEXTS

/*---------------------------------------------------------------------------
  The total number of phys links per iface. Each PS Iface has one primary
  phys link and DSUMTSPS_MAX_SEC_PER_PRI secondary phys links. 
---------------------------------------------------------------------------*/
#define DSUMTS_MAX_PHYS_LINK_PER_IFACE   \
                SYS_MAX_SEC_PDP_CONTEXT_PER_PRIMARY + 1

/*---------------------------------------------------------------------------
  Default Watermark Levels
---------------------------------------------------------------------------*/

#define DSUMTSPS_UL_WM_DEFAULT_LO   4500
#define DSUMTSPS_UL_WM_DEFAULT_HI   7500
#define DSUMTSPS_UL_WM_DEFAULT_DNE  13500

#define DSUMTSPS_DL_WM_DEFAULT_LO   1600
#define DSUMTSPS_DL_WM_DEFAULT_HI   40000
#define DSUMTSPS_DL_WM_DEFAULT_DNE  49000

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Enum to decide type of call to be originated based on Netpolicy table.
---------------------------------------------------------------------------*/
typedef enum
{
  DS_UMTS_PDP_TYPE_FAIL              = -1,
  DS_UMTS_PDP_TYPE_IPV4              = 0,         /* V4 Call               */
  DS_UMTS_PDP_TYPE_PPP               = 1,         /* PPP Call              */
  DS_UMTS_PDP_TYPE_IPV6              = 2,         /* V6 Call               */
  DS_UMTS_PDP_TYPE_IP_ANY            = 3,         /* V4/V6 Call            */ 
  DS_UMTS_PDP_TYPE_MAX               = 0xFF       /* MAX                   */    
                                           /* enum is defined as a byte    */
} ds_umts_originate_call_e_type;

/*---------------------------------------------------------------------------
 Structure to hold the call related parameters for an ongoing call.
---------------------------------------------------------------------------*/
typedef struct{
  uint32                       profile_number; 
                        /* profile number for the profile used in the call */
  byte                         profile_name[DS_UMTS_MAX_PROFILE_NAME_LEN+1];
                                    /* Profile name in ascii string format */
  ds_umts_pdp_auth_type        auth;                /* Authentication info */
  byte                         apn[DS_UMTS_MAX_APN_STRING_LEN+1]; 
                                                            /* APN string  */
  ds_umts_pdp_type_enum_type   pdp_type;             /* PDP type (IP/PPP)  */

  ds_umts_gprs_qos_params_type gprs_qos;  /* Network alloc GPRS QoS params */
  ds_umts_umts_qos_params_type umts_qos;  /* Network alloc UMTS QoS params */
  boolean                      request_pcscf_address_flag;
} dsumtsps_session_info;

/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
#define dsumtsps_is_ps_data_available ds3gpp_is_ps_data_available

/*===========================================================================
FUNCTION DS3GPP_IS_PS_DATA_AVAILABLE
 
DESCRIPTION
   This function returns TRUE if data is available in any of the uplink
   buffers, otherwise, it returns FALSE.
   The function is accessible at the SU api level.

PARAMETERS  

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If Data is available
  FALSE - If Data is not available
   
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds3gpp_is_ps_data_available(void);

#endif /* DSUMTSPS_H */
