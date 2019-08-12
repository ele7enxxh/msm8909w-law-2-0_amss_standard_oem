#ifndef HDRLUPC_H
#define HDRLUPC_H
/*===========================================================================

  L O C A T I O N   U P D A T E   P R O T O C O L   C O N F I G U R A T I O N
                   I N T E R N A L   D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the LUP Protocol.

Copyright (c) 2001 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrlupc.h_v   1.3   19 Nov 2001 18:42:58   shuang  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/inc/hdrlupc.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/11   cnx     Added hdrlupc_get_config_result_handle();
                   Moved hdrlupc_config_result_type to hdrlupc.h.
07/06/06   yll     Changed the attrib_len from uint8 to uint16.
12/13/05   pba     Changes to support multiple personality negotiation
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
09/09/04   mpa     Preliminary QOS support.  Moved neg. handling to hdrpac.
08/17/01   vas     Changed hdrlupc_process_msg prototype to accespt stream_id
03/25/01   dna     Created module

===========================================================================*/

/*===========================================================================

                    INCLUDE  FILES  FOR  MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "dsm.h"
#include "comdef.h"
#include "hdrind.h"

/* <EJECT> */
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
#define HDRLUPC_RAN_HANDOFF_DEFAULT_ATTRIB_ID 0xFF
#define HDRLUPC_DEFAULT_ATTRIB_ID_SIZE        8  /* Default Packet App.   */

#define HDRLUPC_DEFAULT_RAN_HO_ATTRIB_SIZE HDRLUPC_ATTRIB_LEN_SIZE + \
                                           HDRLUPC_DEFAULT_ATTRIB_ID_SIZE + \
                                           HDRLUPC_ATTRIB_VAL_SIZE
#define HDRLUPC_ATTRIB_LEN_SIZE   8
#define HDRLUPC_ATTRIB_VAL_SIZE   8

#ifdef FEATURE_HDR_QOS
#define HDRLUPC_RAN_HANDOFF_MULTIFLOW_ATTRIB_ID 0xFFFF
#define HDRLUPC_MULTIFLOW_ATTRIB_ID_SIZE     16  /* Multiflow Packet App. */

#define HDRLUPC_MULTIFLOW_RAN_HO_ATTRIB_SIZE HDRLUPC_ATTRIB_LEN_SIZE + \
                                           HDRLUPC_MULTIFLOW_ATTRIB_ID_SIZE + \
                                           HDRLUPC_ATTRIB_VAL_SIZE
#endif /* FEATURE_HDR_QOS */

typedef struct
{
  union 
  {
    boolean is_accepted;   /* For AN_INIT stage */
    boolean is_requested;  /* For AT_INIT stage */
  } att; 

  uint8 value; /* Value   for simple attributes,
                  ValueID for complex attributes */
} hdrlupc_config_attrib_result_type;

typedef struct
{
  hdrlupc_config_attrib_result_type ran_handoff;
} hdrlupc_config_req_result_type;

typedef struct
{
  boolean is_accepted;
  uint8   value;
} hdrlupc_inconfig_type;

typedef struct
{
  hdrlupc_inconfig_type ran_ho; /* RAN HO attribute */
} hdrlupc_config_result_type;

typedef struct
{
  /* Whether or not we are allowed to send an unsolicited location update */
  boolean unsolicited_is_allowed;
} hdrlupc_config_params_type;

/* <EJECT> */
/*===========================================================================

                 DEFINITIONS  FOR MESSAGE PARSING
             
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRLUPC_POWERUP_INIT

DESCRIPTION
  This function performs the initialization of the Location Update 
  configuration negotiation module.

DEPENDENCIES
  This function must called only once after powerup

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrlupc_powerup_init ( void );

/*===========================================================================

FUNCTION HDRLUPC_CONFIG_INIT_DEFAULT

DESCRIPTION
  Initializes the in-config values to "negotiated"
                          
DEPENDENCIES
  None.

PARAMETERS
  None.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrlupc_config_init(void);

/*===========================================================================

FUNCTION HDRLUPC_PROCESS_IND

DESCRIPTION
  This function processes indications given to LUPC.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for LUP to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrlupc_process_ind 
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
);

/*===========================================================================

FUNCTION HDRLUPC_UNPACK_DEFAULT_CONFIG_REQ_ATTRIB

DESCRIPTION
  This function unpacks default Config Request attributes.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message

RETURN VALUE
  TRUE  - Message unpacked successfully
  FALSE - otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrlupc_unpack_default_config_req_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset
);

/*===========================================================================

FUNCTION HDRLUPPC_PACK_DEFAULT_CONFIG_RSP_ATTRIBS

DESCRIPTION
  This function packs the Multiflow RAN Handoff attributes in a Configuration 
  Response Message (AN INIT)
  
DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  attrib_offset - Bit offset of the attribute from the beginning of the message

RETURN VALUE
  (attrib_offset is updated)
  
SIDE EFFECTS
  None

===========================================================================*/
extern void hdrlupc_pack_default_config_rsp_attribs
(
  dsm_item_type *item_ptr,
  uint16 *attrib_offset
);

#ifdef FEATURE_HDR_QOS
/*===========================================================================

FUNCTION HDRLUPC_UNPACK_MULTIFLOW_CONFIG_REQ_ATTRIB

DESCRIPTION
  This function unpacks Config Request Attributes for the Multiflow Packet App.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message

RETURN VALUE
  TRUE  - Message unpacked successfully
  FALSE - otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrlupc_unpack_multiflow_config_req_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset
);

/*===========================================================================

FUNCTION HDRLUPPC_PACK_MF_CONFIG_RSP_ATTRIBS

DESCRIPTION
  This function packs the Multiflow RAN Handoff attributes in a Configuration 
  Response Message (AN INIT)
  
DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  attrib_offset - Bit offset of the attribute from the beginning of the message

RETURN VALUE
  (attrib_offset is updated)
  
SIDE EFFECTS
  None

===========================================================================*/
extern void hdrlupc_pack_mf_config_rsp_attribs
(
  dsm_item_type *item_ptr,
  uint16 *attrib_offset
);
#endif /* FEATURE_HDR_QOS */
/*===========================================================================

FUNCTION HDRLUPC_UNSOLICITED_IS_ALLOWED

DESCRIPTION
  This function tells whether unsolicited locaiton notification is allowed
  by the current configuration.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  TRUE if the current configuration allows the AT to send an unsolicited
  LocationNotification message, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrlupc_unsolicited_is_allowed ( void );

/*===========================================================================

FUNCTION HDRLUPC_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  This function sets Location Update Protocol's configuration parameters
  to default. 

DEPENDENCIES
  None

PARAMETERS
  current_config - Pointer to Location Update Protocol's current configuration
                   structure in SCMDB.
                   
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrlupc_set_config_to_default
( 
  hdrlupc_config_params_type *current_config 
);
/*===========================================================================

FUNCTION HDRLUPC_GET_INCONFIG_HANDLE

DESCRIPTION
  This function returns a pointer to inconfig instance of LUP

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Pointer to inconfig instance of LUP

SIDE EFFECTS
  None

===========================================================================*/
extern void* hdrlupc_get_inconfig_handle(void);

/*===========================================================================
FUNCTION HDRLUPC_GET_CONFIG_RESULT_HANDLE

DESCRIPTION
  This function returns the pointer to the config result of LUP. 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Pointer to the config result.

SIDE EFFECTS
  None.
===========================================================================*/
extern void* hdrlupc_get_config_result_handle( void );

#endif /* HDRLUPC_H */
