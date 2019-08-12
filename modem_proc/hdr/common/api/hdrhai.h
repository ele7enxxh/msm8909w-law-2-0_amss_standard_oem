#ifndef HDRHAI_H
#define HDRHAI_H

/*===========================================================================

          H D R   A I R   I N T E R F A C E   D E F I N I T I O N S

DESCRIPTION
  This contains the common declarations for the HDR air interface (HAI).

Copyright (c) 2000 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrhai.h_v   1.49   14 Mar 2003 11:05:56   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/api/hdrhai.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/24/12   cnx     SU API cleanup.
11/28/11   cnx     Support NV for set preReg status.
11/10/11   cnx     Fixed compiler errors.
11/08/11   cnx     Support Optimized Handoff phase 3.
10/28/11   pba     Optimized handoff phase 2 support
10/19/11   smd     SU API cleanup update.
09/23/11   smd     SU API cleanup.
07/27/11   cnx     Merged Optimized Handoff changes.
06/03/11   cnx     Renamed HDRHAI_CLOSE_EHRPD_CREDENTIALS_CHANGED.
04/27/11   cnx     Added HDRHAI_EXTERNAL_MODULE.
12/15/10   pba     Added ehrpd over Rev B support.
11/09/10   pba     Fixed issue of DO CP not reporting call failure to CM during
                   BSR attempt.
06/25/10   sju     Added support for ConnectionFailureReport based on SAR.
10/30/09   pba     Added ehrpd support.
10/27/09   wsh     Fixed compiler warning for ARM compiler
10/06/09   etv     Force protocol name and instance enum size to 32 bits
04/21/08   etv     Added HDRHAI_DECODER module.
06/08/08   wsh     Merged DCM on mainbranch
08/22/07   etv     Fixed high lint warnings.
08/22/07   pba     Added HDRHAI_MC_TEST_APP_IS890B
07/19/07   sju     Added HDRHAI_ABORT_ERROR for CC reason 0x2
07/13/07   sju     Removed hdrhai_systype_enum. New enum in sys.h will be used.
05/29/07   mpa     Added HDRHAI_CLOSE_NEW_NAI
04/13/07   pba     Added HDRHAI_SCHG_OPENED_RESTORE_PRIOR_AND_PS
03/05/07   sju     Added MMPA to app_subtype_enum_type
01/08/07   yll     Added Support for CSNA
11/15/06   sju     Added HDRHAI_ABORT_MSG.
10/18/06   pba     Defined HDRHAI_RUTC_LIST_SIZE_MAX
10/05/06   wsh     Added macro to test if msg_id is for a config message
10/04/06   pba     Added HDRHAI_LOGGING_MODULE.
09/22/06   pba     Reverted HDRHAI_ABORT_NORMAL_CLOSE_SESS_CFG.
09/08/06   spn     Added a new protocol for reservation module 
09/05/06   mpa     Added HDRHAI_ABORT_NORMAL_CLOSE_SESS_CFG.
08/21/06   mpa     Moved some of the internal protocols to a lower ID range
                   (current limitation on our current QXDM event payload size)
05/24/06   pba     Removed enum hdrhai_snpa_enum_type
05/18/06   pba     Fixed featurization of EMPA.
05/10/06   yll/ksu EMPA: octet stream, octet sequencing
05/09/06   yll     Added support for Minimal UATI Mode.
05/05/06   pba     Added HDRHAI_SESSION_CONFIG_MANAGER_DATABASE
04/21/06   hal     Added SESSION_CHANGED reasons to the HDRIND_SCM_SESSION_CHANGED 
                   indication
04/20/06   pba     Fixed issue with proposing stream configurations.
04/17/06   pba     Added new enum hdrhai_snpa_enum_type
03/22/06   pba     Added two new session close reasons.
01/05/06   mpa     Moved VSP back to 0x19.  Added VSApp prefix 0x1A(vs).
12/06/05   etv     Added Generic Multimode Capab Disc protocol.
09/30/05   mpa     Added HDRHAI_BROADCAST_FRAMING_PROTOCOL.
08/28/05   mpa     (for dna) Added HDRHAI_BROADCAST_MAC_PROTOCOL.
08/22/05   etv     Added SCM to protocol name enum type.
08/01/05   hal     Added support for CloseReason 011 (1x handdown)
                   Added HDRHAI_ABORT_RESELECT to hdrhai_abort_reason_enum_type
07/25/05   etv     Added Max Num of Rel-0/A DRCs in hdrhai_drc_value_enum_type
07/22/05   etv     Added Max DRC values in enum hdrhai_drc_value_enum_type
07/08/05   jyw     Added new Rel. A DRC values and IS890A test app subtype
05/09/05   ar/pba  Integrated Data Over Signaling protocol code
04/21/05   pba     Grouped messages with msg_id >= 0x50 and msg_id <= 0x7F as
                   config and others ( 0x00-0x4F and 0x80-0xFF ) as non-config
03/07/05   pba     Added new enum hdrhai_msg_group_enum_type, to group msgs
                   based msg_id value. Added HDRHAI_PROTOCOL_INSTANCE_ENUM_MAX 
                   to hdrhai_protocol_instance_enum_type.
09/09/04   mpa     Added multiflow streams.
08/26/04   mpa     Added HDRHAI_CLOSE_NEW_ESN to session close reasons.
07/02/04   mpa     Added HDRHAI_BROADCAST_PROTOCOL.  
                   Moved all internal protocols to higher range.
04/13/04   mpa     Added HDRHAI_RX to hdrhai_protocol_name_enum_type
02/25/04   mpa     Added HDRHAI_CLOSE_SILENT_DEACTIVATE to session close reasons
10/05/03   mpa     Added HDRHAI_PHYSICAL_LAYER_PROTOCOL to the name enum type
06/26/03   mpa     Added new HDRHAI_CLOSE_CONN_OPEN_DENY in session close reason
06/19/03   mpa     Separated RLP from STREAM1 in hdrhai_protocol_name_enum_type
03/12/03   mpa     Added session_close_reason_enum_type
03/05/03   dna     Fixed HDRHAI_PROTOCOL_NAME_ENUM_MAX
01/08/03   mpa     Removed unnecessary dependency with hdrsrch.h
07/18/02   dna     Changed advertised HDR protocol revision to 0x01
06/03/02   mpa     Mainlined FEATURE_HDR_7_4_DDR.
04/09/02   sh      Added change HDR and exit HDR for redirect and PRL not
                   preferred abort reason
02/05/01   vas     Added enum for AN authentication status.
01/21/02   sh      Added HDRMC in protocol name and USER RUDE CLOSE in
                   abort reason
12/01/01   vas     Added enum for HDRHAI_AN_AUTHENTICATION_STATE_MODULE
11/20/01   dna     Added enum HDRHAI_SYSTYPE
11/17/01   sh      Added 1xhdr data session handoff support
09/21/01   om      Added IS-890 protocol names
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
08/17/01   vas     Added enum HDRHAI_PACKET_APPLICATION_CONTROLLER
08/13/01   om      Added DRC enum (needed for testapp)
08/06/01   vas     Renamed Application subtype enum for consistency
08/02/01   nmn     Moved Application Subtype enum to HAI from HDRSTREAM
07/10/01   dna     Added new values for hdrhai_open_reason_enum_type
06/06/01   dna     new param for hmp_send_msg()
05/30/01   vas     Added hdrhai_stream_enum_type
05/22/01   dna     Cleanup, and added AC_OR_RTC to the channel_enum_type
03/29/01   dna     Added flow control protocol name
03/10/01   sh      Added HDRHAI_NO_CCC_HASH
01/22/01   nmn     Added configuration response timer length definition
09/27/00   dna     Added 1xEV protocol values and fixed ATI type enum
09/21/00   sh      Added the abort reason enum
09/07/00   dna     Updated enum_name_type per design changes including LMAC
                   and others.
07/27/00   sh      added PLI related indications and removed
                   hai_ind_data_union_type
02/02/00   dna     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "hdrcom_api.h"
/* <EJECT> */ 
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/* Timer for all configuration response messages - 2 seconds (2,000 msec) */
#define HDRHAI_CONFIG_RSP_TIMER_MSEC 0x000007D0

#define HDRHAI_NO_CCC_HASH     HDRSRCH_NO_CCC_HASH

#define HDRHAI_REVISION_NUMBER  1 /* IS-856-1 */

#define HDRHAI_MAX_SP_NEIGHBORS 32 /* NumNeighbors field of SP msg is 5 bits*/

/* A message is a config message if msg_id is between 0x50 and 0x7F */
#define HDRHAI_MSG_IS_CONFIG(msg_id) ( ( (msg_id) >= 0x50 ) && \
                                       ( (msg_id) <= 0x7F ) )

#define HDRHAI_RUTC_LIST_SIZE_MAX 5 /* Max RouteUpdateTriggerCodeListSize */

typedef enum 
{
  HDRHAI_NON_CONFIG,          /* msgs with msg_id < 0x50 and msg_id > 0x7F */
  HDRHAI_CONFIG,              /* msgs with msg_id >= 0x50 and msg_id <= 0x7F */
  HDRHAI_MSG_GROUP_ENUM_MAX
}hdrhai_msg_group_enum_type;

/* Enumeration of Access Terminal Identifier (ATI) types */
typedef enum
{
  HDRHAI_BATI = 0,           /* Broadcast ATI type */
  HDRHAI_MATI = 1,           /* Multicast ATI type */
  HDRHAI_UATI = 2,           /* Unicast ATI Type */
  HDRHAI_RATI = 3,           /* Random ATI Type */
  HDRHAI_ATI_INACTIVE        /* ATI not active */
} hdrhai_ati_enum_type;

/* Structure for storing ATI record, that is the ATI type & value */
typedef struct
{
  hdrhai_ati_enum_type ati_type;
  uint32 ati_value;
} hdrhai_ati_record_type;

#define HDRHAI_ATI_NO_VALUE 0xffffffffUL /* FFFF is an invalid ATI value */

#define HDRHAI_MAX_PILOTS 16

/* Enumeration of the possible DRC values */
typedef enum
{
  HDRHAI_DRC_NULL_RATE     = 0x00,
  HDRHAI_DRC_38_4          = 0x01,
  HDRHAI_DRC_76_8          = 0x02,
  HDRHAI_DRC_153_6         = 0x03,
  HDRHAI_DRC_307_2_SHORT   = 0x04,
  HDRHAI_DRC_307_2_LONG    = 0x05,
  HDRHAI_DRC_614_4_SHORT   = 0x06,
  HDRHAI_DRC_614_4_LONG    = 0x07,
  HDRHAI_DRC_921_6         = 0x08,
  HDRHAI_DRC_1228_8_SHORT  = 0x09,
  HDRHAI_DRC_1228_8_LONG   = 0x0a,
  HDRHAI_DRC_1843_2        = 0x0b,
  HDRHAI_DRC_2457_6        = 0x0c,
  HDRHAI_MAX_REL_0_DRC = HDRHAI_DRC_2457_6,
  HDRHAI_MAX_NUM_REL_0_DRCS,
  HDRHAI_DRC_1536          = 0x0d,  /* new to Rel. A */
  HDRHAI_DRC_3072          = 0x0e,  /* new to Rel. A */
  HDRHAI_MAX_REL_A_DRC = HDRHAI_DRC_3072,
  HDRHAI_MAX_NUM_REL_A_DRCS
#ifdef FEATURE_HDR_REVB
  ,
  HDRHAI_DRC_REL_B_NA      = 0x0f,  /* N/A to Rel. B */
  HDRHAI_DRC_REL_B_460_8   = 0x10,  /* new to Rel. B */
  HDRHAI_DRC_REL_B_614_4   = 0x11,  /* new to Rel. B */
  HDRHAI_DRC_REL_B_768_0   = 0x12,  /* new to Rel. B */
  HDRHAI_DRC_REL_B_921_6   = 0x13,  /* new to Rel. B */
  HDRHAI_DRC_REL_B_1075_2  = 0x14,  /* new to Rel. B */
  HDRHAI_DRC_REL_B_1228_8  = 0x15,  /* new to Rel. B */
  HDRHAI_DRC_REL_B_1834_2  = 0x16,  /* new to Rel. B */
  HDRHAI_DRC_REL_B_2150_4  = 0x17,  /* new to Rel. B */
  HDRHAI_DRC_REL_B_2457_6  = 0x18,  /* new to Rel. B */
  HDRHAI_DRC_REL_B_3686_4  = 0x19,  /* new to Rel. B */
  HDRHAI_DRC_REL_B_4300_8  = 0x1a,  /* new to Rel. B */
  HDRHAI_DRC_REL_B_4915_2  = 0x1b,  /* new to Rel. B */
  HDRHAI_MAX_REL_B_DRC = HDRHAI_DRC_REL_B_4915_2,
  HDRHAI_MAX_NUM_REL_B_DRCS
#endif /* FEATURE_HDR_REVB */
} hdrhai_drc_value_enum_type;

/* Enumeration for Access Network Authentication Status */
typedef enum
{
  HDRHAI_AN_NOT_AUTHENTICATED = 0x00,     /* AN Stream not authenticated */
  HDRHAI_AN_AUTHENTICATED =0x01,          /* AN Stream is authenticated  */
  HDRHAI_AN_AUTH_DISABLED = 0xFF    /* AN Stream authentication Disabled */
} hdrhai_an_auth_status_enum_type;

/* Enumeration for Connection Failure Reasons */
typedef enum
{
  HDRHAI_GENERAL_FAILURE = 0x0,            /* General connection failure reason */
  HDRHAI_1X_TUNE_AWAY_FAILURE = 0x1,       /* 1x tune-away failure reason */
  HDRHAI_LOCATION_TUNE_AWAY_FAILURE = 0x2  /* Location tune-away failure reason */

} hdrhai_connection_failure_enum_type;

/* <EJECT> */ 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#endif               /* HDRHAI_H */
