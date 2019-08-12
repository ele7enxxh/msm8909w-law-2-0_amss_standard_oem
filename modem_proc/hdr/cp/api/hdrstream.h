#ifndef HDRSTREAM_H
#define HDRSTREAM_H

/*===========================================================================

                S T R E A M   C O N F I G U R AT I O N 
                           D E F I N I T I O N S

DESCRIPTION
  This contains the declarations related to stream configuration

Copyright (c) 2000-2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrstream.h#1 $ 
$DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/17/07   pba     RelB MMPA support.
03/21/07   pba     Removed hdrstream_hardlink_is_allowed().
03/05/07   sju     Added hdrstream_get_stream_for_protocol() used by slp to get 
                   stream_id.
01/24/07   pba     Updated Restrict Stream Configuration functionality to allow
                   hardlinking of MPA/EMPA.
05/24/06   pba     Re-designed and fixed issues with preferred stream 
                   configuration proposed by AT in AT_INIT phase.
04/24/06   mpa     Removed featurization of hdrstream_stream_is_negotiated().
04/17/06   pba     Added support to accept/propose only supported stream 
                   configuration.
12/13/05   pba     Changes to support multiple personality negotiation
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
07/07/05   dna     Added support for Enhanced Idle State protocol
09/09/04   mpa     Added accessor functions for QOS.
06/09/02   vas     Added hdrstream_get_apptype_for_neg_stream()
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
08/06/01   vas     Renamed function data_has_stream() to stream_is_assigned()
08/02/01   nmn     Moved Application Subtype enum to HAI
04/12/01   nmn     Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdrhai.h"
#include "dsm.h"
#include "comdef.h"
#include "hdrerrno.h"
#include "hdrind.h"

/* <EJECT> */ 
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Number of bytes of the STREAM Attribute ID */
#define HDRSTREAM_ATTRIBUTE_ID_LENGTH 1

#define HDRSTREAM_NUM_STREAMS 4

/* Structure for stream configuration attribute */
typedef struct
{
  uint16 stream_application[HDRSTREAM_NUM_STREAMS];
} hdrstream_stream_config_attrib_type;

/* Structure to hold all configurable parameters for stream protocol */
typedef struct
{
  hdrstream_stream_config_attrib_type stream_config;
} hdrstream_config_params_type;

/* <EJECT> */ 
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRSTREAM_CONFIG_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRSTREAM in-config instance.

DEPENDENCIES
  None
  
PARAMETERS
  msg - Pointer to the DSM item chain holding the message

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

extern void hdrstream_config_msg_cb
(
  dsm_item_type * msg_ptr 
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRSTREAM.

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

extern void hdrstream_ind_cb
( 
  hdrind_ind_name_enum_type ind_name, 
  void * ind_data_ptr
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_STREAM_IS_ASSIGNED

DESCRIPTION
  This boolean accessor function determines whether there has been
    at least one stream allocated for a packet application.

DEPENDENCIES
  None.

PARAMETERS
  app_subtype -  Application sub-type to check streams for

RETURN VALUE
  TRUE  - Stream has been allocation for a Packet Application
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern boolean hdrstream_stream_is_assigned
(
  hdrhai_app_subtype_enum_type   app_subtype
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_STREAM_IS_NEGOTIATED

DESCRIPTION
  This boolean accessor function determines whether a stream was negotiated
  for a application subtype.

DEPENDENCIES
  None.

PARAMETERS
  app_subtype -  Application sub-type to check streams for

RETURN VALUE
  TRUE  - This packet application was bound to a stream during negotiation
          (may not be current yet)
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrstream_stream_is_negotiated
(
  hdrhai_app_subtype_enum_type   app_subtype
);

/*===========================================================================

FUNCTION HDRSTREAM_CONFIGURE

DESCRIPTION
  This function conditionally commands HDRSTREAM to configure.

DEPENDENCIES
  None
 
PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  TRUE - the configure command is accepted
  FALSE - the configure command is rejected
  
SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrstream_configure
(
  hdrhai_protocol_name_enum_type sender
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_POWERUP_INIT

DESCRIPTION
  This function initialzes hdrstream.  This function is only called
    during powerup.

DEPENDENCIES
  None
 
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
extern void hdrstream_powerup_init ( void );

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSTREAM_GET_STREAM_CONFIG

DESCRIPTION
  This accessor function returns the configured applications for each
    stream.

DEPENDENCIES
  None
 
PARAMETERS
  stream_ptr - first element of a four-application array.

RETURN VALUE
  TRUE - streams were returned
  FALSE - streams were not returned
  
SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrstream_get_stream_config 
(
  hdrhai_app_subtype_enum_type * stream_ptr
);

/*===========================================================================

FUNCTION HDRSTREAM_GET_STREAM

DESCRIPTION
  This helper function returns the stream ID of the stream associated with the
  app subtype specified.

DEPENDENCIES

PARAMETERS
  app_subtype - The application subtype who's stream is desired.
  stream_id - Pointer to loaction in which to return the stream_id, if 
              one is found.

RETURN VALUE
  TRUE - There is a stream configured with the Service Network
  FALSE - There is no stream configured with the Service Network

SIDE EFFECTS
  None

===========================================================================*/

extern boolean hdrstream_get_stream  
( 
  hdrhai_app_subtype_enum_type app_subtype,
  hdrhai_stream_enum_type *stream_id
);

/*===========================================================================

FUNCTION HDRSTREAM_GET_NEGOTIATED_STREAM

DESCRIPTION
  This helper function returns the stream ID of the stream associated with the
  app subtype specified.

DEPENDENCIES

PARAMETERS
  app_subtype - The application subtype who's stream is desired.
  stream_id - Pointer to location in which to return the stream_id, if 
              one is found.

RETURN VALUE
  TRUE - There is a stream configured with the Service Network
  FALSE - There is no stream configured with the Service Network

SIDE EFFECTS
  None

===========================================================================*/

extern boolean hdrstream_get_negotiated_stream  
( 
  hdrhai_app_subtype_enum_type app_subtype,
  hdrhai_stream_enum_type *stream_id
);
/*===========================================================================

FUNCTION HDRSTREAM_GET_APPTYPE_FOR_NEG_STREAM

DESCRIPTION
  This function returns the negotiated App Subtype for the particular Stream 
  ID. Note that this value is the negotiated value, and may not have been
  committed.

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrhai_app_subtype_enum_type hdrstream_get_apptype_for_neg_stream
( 
  hdrhai_stream_enum_type stream_id 
);

/*===========================================================================

FUNCTION HDRSTREAM_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  This function initializes STREAM configuration parameters to default. 

DEPENDENCIES
  None

PARAMETERS
  current_cfg - Pointer to STREAM protocol's current configuration 
                structure in SCMDB.
                   
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrstream_set_config_to_default
( 
  hdrstream_config_params_type *current_config 
);

/*===========================================================================

FUNCTION HDRSTREAM_GET_INCONFIG_HANDLE

DESCRIPTION
  This function returns a pointer to inconfig instance of STREAM protocol

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Pointer to inconfig instance of STREAM protocol

SIDE EFFECTS
  None

===========================================================================*/
extern void* hdrstream_get_inconfig_handle(void);

/*===========================================================================

FUNCTION HDRSTREAM_GET_STREAM_FOR_PROTOCOL

DESCRIPTION
  This helper function get stream id from protocol id.

DEPENDENCIES
  None
  
PARAMETERS
  protocol_id   - Protocol ID

RETURN VALUE
  stream_id

SIDE EFFECTS
  None
===========================================================================*/
extern hdrhai_stream_enum_type hdrstream_get_stream_for_protocol
( 
  uint8                    protocol_id
);

/*===========================================================================

FUNCTION HDRSTREAM_GET_NEGOTIATED_SNPKTAPP

DESCRIPTION
  This function returns the negotiated packet application bound to SN if any.
  Otherwise returns HDRHAI_APP_STREAM_NOT_USED.

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Returns negotiated packet application bound to SN or 
  HDRHAI_APP_STREAM_NOT_USED.

SIDE EFFECTS
  None

===========================================================================*/
hdrhai_app_subtype_enum_type hdrstream_get_negotiated_snpktapp(void);

#endif               /* HDRSTREAM_H */
