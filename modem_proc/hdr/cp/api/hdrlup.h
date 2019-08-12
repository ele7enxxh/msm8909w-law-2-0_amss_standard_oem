#ifndef HDRLUP_H
#define HDRLUP_H
/*===========================================================================

              L O C A T I O N   U P D A T E   P R O T O C O L
                           D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the Location Update Protocol.

Copyright (c) 2001,2002,2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2004 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrlup.h_v   1.7   19 Nov 2001 18:42:42   shuang  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrlup.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/26/11   smd     SU API cleanup 
06/05/06   pba     Added support to store session data in EFS.
09/09/04   mpa     Preliminary QOS support.
                   Moved event dispatching to hdrpac (centralized).
10/08/03   mpa     Added hdrlup_get_location().
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
08/17/01   vas     Moved msg id defines here so they can be accessed by PAC
07/10/01   dna     Change LUP to not do AT initiated negotiation
03/25/01   dna     Created module

===========================================================================*/

/*===========================================================================

                    INCLUDE  FILES  FOR  MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "dsm.h"
#include "hdrhai.h"
#include "hdrind.h"

/* <EJECT> */
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

#define HDRLUP_ATTRIBUTE_ID_LENGTH 8

/* <EJECT> */
/*===========================================================================

                 DEFINITIONS  FOR MESSAGE PARSING

===========================================================================*/
#define HDRLUP_LOC_RESP_MSG   0x04
#define HDRLUP_LOC_COMP_MSG   0x06

#ifdef FEATURE_HDR_QOS
#define HDRLUP_SNID_NOTIF_MSG  0x11
#define HDRLUP_SNID_COMP_MSG   0x13
#endif /* FEATURE_HDR_QOS */

#define HDRLUP_CONFIG_REQ_MSG 0x50
#define HDRLUP_CONFIG_RESP_MSG 0x51


/* <EJECT> */
/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

#define HDRLUP_LOC_TYPE_IS2000_LEN 0x05

typedef struct
{
  boolean is_valid;
  uint8 type;
  uint8 length;
  uint8 value[HDRLUP_LOC_TYPE_IS2000_LEN];
} hdrlup_location_type;

#ifdef FEATURE_HDR_QOS
/* SNID is now StorageBLOB, where StorageBLOBLength is an 8 bit field
 * that gives StorageBlob length in octets */

#define HDRLUP_SNID_LEN 255

typedef struct
{
  boolean is_valid;
  uint16 type;
  uint8 length;
  uint8 value[HDRLUP_SNID_LEN];
} hdrlup_snid_type;
#endif /* FEATURE_HDR_QOS */

typedef struct
{
  hdrlup_location_type location; /* The AT's current location */
#ifdef FEATURE_HDR_QOS
  hdrlup_snid_type     snid; /* The AT's current Service Network ID */
#endif /* FEATURE_HDR_QOS */

}hdrlup_session_info_type;

/* <EJECT> */
/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLUP_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to LUP.

DEPENDENCIES
  None

PARAMETERS
  msg - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hdrlup_msg_cb (dsm_item_type * msg_ptr);

/*===========================================================================

FUNCTION HDRLUP_POWERUP_INIT

DESCRIPTION
  This function performs the initialization of the Location Update Protocol.

DEPENDENCIES
  None 

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void hdrlup_powerup_init ( void );

/*===========================================================================

FUNCTION HDRLUP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to LUP.

DEPENDENCIES
  None

PARAMETERS
  ind_name - Indication to process, is a global enum for all
                    possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hdrlup_ind_cb (hdrind_ind_name_enum_type ind_name,
             void *ind_data);

/*===========================================================================

FUNCTION HDRLUP_SEND_LOC

DESCRIPTION
  This function commands LUP to send an unsolicited LocationNotification
  message, if allowed by the current configuration.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrlup_send_loc
(
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRLUP_UPDATE_LOC

DESCRIPTION
  This function commands LUP to update its location to the current
  SID/NID/PZID in use by the dormant 1x data session.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrlup_update_loc
(
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRLUP_GET_LOCATION

DESCRIPTION
  This function is used to query the current SID/NID/PZID location if valid.
  It is needed by 1X to update its location when moving a dormant data session
  from EVDO to 1X.

DEPENDENCIES
  Should not be queried directly outside of the EVDO protocols.

PARAMETERS
  sid - current Sector ID
  nid - current Network ID
  pzid - current Packet Zone ID

RETURN VALUE
  TRUE - the current location value is valid and compatible with IS2000
  FALSE - the current location is either unavailable or not compatible with
          the IS2000 format (SID/NID/PZID).

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrlup_get_location
(
  uint16 *sid,
  uint16 *nid,
  uint8 *pzid
);

/*===========================================================================

FUNCTION HDRLUP_SET_SESSION_INFO_TO_DEFAULT

DESCRIPTION
  This function sets the Location Update Protocol's session parameters
  to default values. 

DEPENDENCIES
  None

PARAMETERS
  curr_config - Pointer to Location Update Protocol's session parameter
                structure in SCMDB.
                   
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrlup_set_session_info_to_default
(
  hdrlup_session_info_type *curr_cfg
);

#endif /* HDRLUP_H */
