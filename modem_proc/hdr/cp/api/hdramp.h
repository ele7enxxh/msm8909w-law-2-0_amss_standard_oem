#ifndef HDRAMP_H
#define HDRAMP_H

/*===========================================================================

            A D D R E S S   M A N A G E M E N T   P R O T O C O L
                           D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the Address Management Protocol (AMP)

Copyright (c) 2000 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdramp.h_v   1.9   18 Apr 2002 15:13:58   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdramp.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/31/13   sat     Mainlining Modem Statistics Feature
03/19/13   cnx     Send SessionClose when both NAI and eHRPD capability change
02/04/13   cnx     Replace PriorSession with UpdateUATI in some network.
11/08/11   cnx     Support Optimized Handoff phase 3.
10/03/11   cnx     Added hdramp_set_hdr_to_lte_resel() and 
                   hdramp_set_entering_tunnel()
06/01/10   pba     Added support for FTD stats.
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/24/09   wsh     CMI: updated included headers due to header split
11/21/08   pba     Added events for SLPQH feature.
10/01/08   wsh     Fixed critical/high lint warnings
05/22/08   wsh     Report shell ESN/MEID to AN. Close session if RUIM changed
09/24/07   wsh     Fixed AMP OVHD_INFO_UPDATED_IND handling
05/29/07   mpa     Added update NAI command
03/27/07   wsh     Added support for new prior session handling
02/12/07   mpa     Fixed RVCT warning.
10/04/06   pba     Moved location information storage from NV to EFS.
06/05/06   pba     Added support to store session data in EFS.
02/07/06   dna     Tell if hdramp_previous_session_opened closed the session.
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
03/27/02   mpa     Removed hdramp_match_receive_ati_address
                   Added #include to hdrind.h
01/21/02   sh      Added hdramp_generate_session_seed
01/17/02   dna     Perform dual address timer init on first acquisition.
05/08/01   nmn     Beautification
05/01/01   dna     For consistency, match_receive_ati_address takes a pointer
12/11/00   nmn     Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#include "hdrhai.h"
#include "dsm.h"
#include "comdef.h"
#include "hdrerrno.h"
#include "hdraddr.h"
#include "hdrind.h"
#include "qw.h"

/* <EJECT> */ 
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/


/* <EJECT> */ 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
typedef enum
{
  HDRAMP_CLEAR_SLP_HOLDOFF_BACK_IN_ORIG_SUBNET = 0,
  HDRAMP_CLEAR_SLP_HOLDOFF_UATIREQ_TX_FAILED,
  HDRAMP_CLEAR_SLP_HOLDOFF_UATIREQ_TIMER_EXPIRED,
  HDRAMP_CLEAR_SLP_HOLDOFF_RCVD_UNICAST_MSG,
  HDRAMP_CLEAR_SLP_HOLDOFF_SESSION_CLOSED,
  HDRAMP_CLEAR_SLP_HOLDOFF_RCVD_UATI_ASSIGNMENT

} hdramp_clear_slp_holdoff_reason_type;

/* Hardware ID Type Field Values*/
typedef enum
{
  HDRAMP_HARDWARE_ID_TYPE_MEID = 0x0000FFFF, /* Mobile Equipment IDentifier */
  HDRAMP_HARDWARE_ID_TYPE_ESN  = 0x00010000, /* ESN */
  HDRAMP_HARDWARE_ID_TYPE_NULL = (int)0xFFFFFFFFuL  /* HWID not defined */
} hdramp_hwid_enum_type;

typedef struct
{
  /* This data type needs to be byte exact with nv_hdramp_hwid_type */
  hdramp_hwid_enum_type type;

  /* At most one value is valid at any given time (ESN or MEID).
   * Technically the ESN and MEID should be a union, but that 
   * would complicate the structure's use and only save 4 bytes. */
  uint32 esn;    /* ESN  */
  qword meid;    /* MEID (56 bit Mobile Equipment IDentifier) */
} hdramp_hwid_type;

/* OVHD Location data structure */
typedef struct
{
  int32 latitude; /* Latitude of the previously received SP message */
  int32 longitude; /* Longitude of the previously received SP message */
  boolean initialized; /* Set if the location values are valid */

} hdramp_location_type;

/* NAI info is tied to the session */
#define HDRAMP_MAX_USER_ID_LEN 127   /* NV_MAX_PPP_USER_ID_LENGTH  */

typedef struct
{
  uint8 user_id_info[HDRAMP_MAX_USER_ID_LEN];
  uint8 user_id_len;
} hdramp_nai_type;

/* Length ICC_ID file on RUIM card */
#if defined(FEATURE_UIM_RUIM) && defined(FEATURE_MMGSDI)
#define HDRAMP_ICC_ID_LEN 10
#endif /* FEATURE_MMGSDI && FEATURE_UIM_RUIM */

/* This structure contains all data related to the addresses and hardware ID, 
   except for the receive ati structure info which was moved to hdrampati. The
   uati_address is the full 128-bit globally unique identifier assigned to this
   AT. The concept is for this to be based on IPv6 addressing, although the 
   actual implementation is left up to the designers of the network. The 
   transmit_ati is the 32-bit address used by the MAC layers when data is 
   transmitted, to identify from which AT the data originated. The receive ati
   (moved to hdrampati) is a list due to the multiple address matching concept.
   We can potentially have multiple UATIs at the same time, and a received 
   message is considered to be addressed to us if it matches any of our valid
   addresses. The uati_cc and uati_subnet_mask are used to identify the subnet,
   and especially when the subnet has changed. The session_seed is generated 
   upon entering INACTIVE state.  It is used as our RATI, and as a seed to 
   the hash algorithm.
   */
typedef struct       
{
  /*--------------------------------------------------------------------------
  The following two fields are restored while restoring prior session.
  --------------------------------------------------------------------------*/
    /* Static storage for msg seq validation of UATI asignment */
  uint8                  uati_assign_validation_seq_num;
    /* Session Seed for this session */
  uint32                 session_seed;

  /*--------------------------------------------------------------------------
  Prior session HWID and current HWID should be same for prior session restore
  to succeed.
  --------------------------------------------------------------------------*/
  hdramp_hwid_type       session_hwid;

  /*--------------------------------------------------------------------------
  Prior session NAI and current NAI should be same for prior session restore
  to succeed.
  --------------------------------------------------------------------------*/
  hdramp_nai_type        session_nai;

  /*--------------------------------------------------------------------------
  The previous UATI is used to restore prior session however UATI itself is not
  restored.
  --------------------------------------------------------------------------*/
    /* current full UATI */
  hdraddr_type           full_uati;

  /*--------------------------------------------------------------------------
  Used only with current session. These fields are not saved in prior session
  information.
  --------------------------------------------------------------------------*/
    /* Color Code for the UATI */
  uint8                  uati_cc;
    /* Subnet Mask for the UATI */
  uint8                  uati_subnet_mask;
    /* 32-bit address used to transmit messages */
  hdrhai_ati_record_type transmit_ati;
    /* 32-bit address used to validate received msgs */
  uint32                 receive_current_uati;

    /* MaxNoMonitorDistance feature requires remembering the previously
       received latitude/longitude */
  hdramp_location_type location;

  uint16 max_no_monitor_distance;
  uint8 support_secondary_colorcodes;
  uint8 reduced_subnet_mask_offset;
  qword expiration_time;
#if defined(FEATURE_UIM_RUIM) && defined(FEATURE_MMGSDI)
  uint8 icc_id[HDRAMP_ICC_ID_LEN];
#endif /* FEATURE_MMGSDI && FEATURE_UIM_RUIM */
} hdramp_session_info_type;

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_GET_PREVIOUS_UATI

DESCRIPTION
  This function returns the previous session's UATI

DEPENDENCIES
  None

PARAMETERS
  previous_uati_ptr - pointer to the previous UATI

RETURN VALUE
  E_SUCCESS - Value returned through the pointer is a valid UATI
  E_FAILURE - Value returned through the pointer is not a valid UATI

SIDE EFFECTS
  None

===========================================================================*/

extern hdrerrno_enum_type hdramp_get_previous_uati
(
  hdraddr_type previous_uati_ptr
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_GET_TRANSMIT_ATI

DESCRIPTION
  This function returns the transmit ATI

DEPENDENCIES
  None

PARAMETERS
  transmit_ati_ptr - pointer to the address where the transmit ATI should
                     be placed
  sender - protocol requesting the transmit ATI

RETURN VALUE
  E_SUCCESS - Value returned to the pointer is a valid Transmit address
  E_FAILURE - Value returned to the pointer is not a valid Transmit address

SIDE EFFECTS
  None

===========================================================================*/

extern hdrerrno_enum_type hdramp_get_transmit_ati
(
  hdrhai_ati_record_type * transmit_ati_ptr,
  hdrhai_protocol_name_enum_type sender
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_GET_SESSION_SEED

DESCRIPTION
  This function returns the session seed

DEPENDENCIES
  None

PARAMETERS
  sender - protocol which requested the session seed

RETURN VALUE
  uint32 - the session seed

SIDE EFFECTS
  None

===========================================================================*/

extern uint32 hdramp_get_session_seed 
(
  hdrhai_protocol_name_enum_type sender
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRAMP.

DEPENDENCIES
  None
  
PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

extern void hdramp_msg_cb
( 
  dsm_item_type * msg_ptr 
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRAMP.

DEPENDENCIES
  None
 
PARAMETERS
  ind_name - Indication to process, is a global enum for all
                    possible indications in HDR signaling
  ind_data_ptr - Optional data pointer for indication specific data
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

extern void hdramp_ind_cb
( 
  hdrind_ind_name_enum_type ind_name,
  void *ind_data_ptr
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_ACTIVATE

DESCRIPTION
  This function commands AMP to activate.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdramp_activate
(
  hdrhai_protocol_name_enum_type sender 
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_UPDATE_UATI

DESCRIPTION
  This function commands AMP to update the UATI.  

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdramp_update_uati
(
  hdrhai_protocol_name_enum_type sender 
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_DEACTIVATE

DESCRIPTION
  This function commands AMP to deactivate.  

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdramp_deactivate
(
  hdrhai_protocol_name_enum_type sender 
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_FIRST_ACQ_INIT

DESCRIPTION
  This function commands AMP to initialize upon our first HDR acquisition.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdramp_first_acq_init ( void );

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_POWERUP_INIT

DESCRIPTION
  This function commands AMP to Initialize.  This function is called instead
    of hdramp_init at powerup.

DEPENDENCIES
  None

PARAMETERS
  session_is_open - Determines behavior whether the session is open or closed

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdramp_powerup_init
(
  boolean session_is_open 
);


/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRAMP_GENERATE_SESSION_SEED

DESCRIPTION
  This function generates the session seed for AMP 
  
DEPENDENCIES
  Random number seed should be initialized

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdramp_generate_session_seed(void);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_PREVIOUS_SESSION_OPENED

DESCRIPTION
  This function is called by SCP when previous session is opened.

DEPENDENCIES
  None

PARAMETERS
  Restore_UATI - whether the UATI needs to be restored.
  
RETURN VALUE
  TRUE if the previous session is opened successfully, FALSE if there was
  an error (i.e. wrong HWID) and the session was failed.
  
SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdramp_previous_session_opened( boolean restore_UATI );

/*===========================================================================

FUNCTION HDRAMP_IS_PREVIOUS_UATI_VALID

DESCRIPTION
  This function checks if previous UATI has expired

DEPENDENCIES
  None

PARAMETERS
  valid - indicates if previous session expired
  
RETURN VALUE
  TRUE if Success
  
SIDE EFFECTS
  None

===========================================================================*/
boolean hdramp_is_previous_uati_valid( void );

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_SILENT_RESTORE_PRIOR_SESSION

DESCRIPTION
  This function silently restore prior session

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void hdramp_silent_restore_prior_session( void );

/*===========================================================================

FUNCTION HDRAMP_GET_UATI_COLOR_CODE

DESCRIPTION
  This function returns the UATI color code

DEPENDENCIES
  None

PARAMETERS
  uati_color_code - Pointer to location where color code need to copied 

RETURN VALUE
  E_SUCCESS - Value returned to the pointer is a valid
  E_FAILURE - Value returned to the pointer is not a valid

SIDE EFFECTS
  None

===========================================================================*/

hdrerrno_enum_type hdramp_get_uati_color_code
(
  uint8 *uati_color_code
);

/*===========================================================================

FUNCTION HDRAMP_GET_UATI_SUBNET_MASK

DESCRIPTION
  This function returns the UATI subnet mask

DEPENDENCIES
  None

PARAMETERS
  uati_subnet_mask - Pointer to location where subnet mask need to copied 

RETURN VALUE
  E_SUCCESS - Value returned to the pointer is a valid
  E_FAILURE - Value returned to the pointer is not a valid

SIDE EFFECTS
  None

===========================================================================*/

hdrerrno_enum_type hdramp_get_uati_subnet_mask
(
  uint8 *uati_subnet_mask
);

/*===========================================================================

FUNCTION HDRAMP_IS_SESSION_CLOSE_NAI_PENDNG

DESCRIPTION
  This function checks if sesson close is pending due to NAI update

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  TRUE if session close is pending
  
SIDE EFFECTS
  None

===========================================================================*/
boolean hdramp_is_session_close_nai_pending( void );

/*===========================================================================

FUNCTION HDRAMP_SET_SESSION_CLOSE_NAI_PENDNG

DESCRIPTION
  This function checks sets session_close_nai_pending flag.

DEPENDENCIES
  None

PARAMETERS
  FALSE to set session_close_nai_pending to FALSE.
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void hdramp_set_session_close_nai_pending
( 
  boolean value
);

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================

FUNCTION HDRAMP_SEND_IRMI_MSG

DESCRIPTION
  This function sends the IRMI message

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - If IRMI is allowed
  FALSE - otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdramp_send_irmi_msg
( 
  hdrhai_protocol_name_enum_type caller
);

/*===========================================================================
FUNCTION HDRAMP_SET_HDR_TO_LTE_RESEL

DESCRIPTION
  This function set hdr_to_lte_resel flag 

DEPENDENCIES
  None

PARAMETERS
  resel - Whether we just successful complete hdr to lte reselection 

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdramp_set_hdr_to_lte_resel
(
  boolean resel
);

/*===========================================================================
FUNCTION HDRAMP_SET_FIRST_ENTERING_TUNNEL

DESCRIPTION
  This function set entering_tunnel flag 

DEPENDENCIES
  None

PARAMETERS
  entering_tunnel - Whether this is the first time we enters tunnel mode 

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdramp_set_entering_tunnel
(
  boolean entering_tunnel
);

/*===========================================================================

FUNCTION HDRAMP_GET_UATI_SUBNET

DESCRIPTION
  This function returns the subnet in the UATI assignment.

DEPENDENCIES
  None

PARAMETERS
  uati_subnet - Pointer to location where the subnet will get copied 

RETURN VALUE
  E_SUCCESS - Value returned to the pointer is a valid
  E_FAILURE - Value returned to the pointer is not a valid

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdramp_get_uati_subnet
(
  hdraddr_type uati_subnet
);

/*===========================================================================
FUNCTION HDRAMP_SET_PS_IS_NOT_SUPPORTED

DESCRIPTION
  This function sets the flag ps_is_not_supported, which indicates whether
  we should replace all the restore prior session with update UATI in the
  tunnel mode.
  
DEPENDENCIES
  None.

PARAMETERS
  value - whether restore prior session is supported.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdramp_set_ps_is_not_supported
(
  boolean value
);
#endif /* FEATURE_LTE_TO_HDR_OH */

#endif /* HDRAMP_H */
