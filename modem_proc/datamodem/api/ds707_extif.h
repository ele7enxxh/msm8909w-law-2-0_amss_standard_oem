#ifndef DS707_EXTIF_H
#define DS707_EXTIF_H
/*===========================================================================
                         D S 7 0 7 _ E X T I F
GENERAL DESCRIPTION
  This file is the external interface exported by ds707 to the other layers
  in the DMSS code than need to query ds707 for its state or other 
  information. This file is intended to provide an interface to other layers 
  in the DMSS code and is not intended to be used as an interface by
  entities outside of DMSS.
  
  This is specific to packet data calls.

EXTERNALIZED FUNCTIONS
 DS707_EXTIF_PKT_IS_DORMANT
   Checks if there is a dormant packet data session on CDMA or HDR system.

 DS707_EXTIF_IS_PKT_DATA_SESS_ON_HDR
   Checks if there is a packet data session that is active or dormant on an 
   HDR system.
 
 DS707_EXTIF_IS_PKT_DATA_SESS_ON_CDMA
   Checks if there is a packet data session that is active or dormant on a 
   CDMA system.
 
 INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

 Copyright (c) 2003-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/ds707_extif.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who        what, where, why
--------   ---        -------------------------------------------------------
01/31/11   ttv        Removed feature wrapping in api files.
01/25/11   ss         SU API defeaturization and removal of customer.h.
12/15/09   ls         Added Rev-B Rate support
10/15/09   vs         SU Level Api Defeaturization
09/04/09   ss         CMI SU level modifications.
12/05/05   as/kvd     Moved all external macros to ds707_defs.h
09/10/05   gr         Merged changes to lock out GPS in the middle of a PPP 
                      resync during a handup from 1x to DO
04/12/05   vrk        Moved MC specific definitions from ds707.h to ds707_extif.h
04/12/05   vrk        Added a wrapper function ds707_extif_verify_so
01/27/04   vr         Added function prototypes for
                      ds707_extif_was_last_data_network_cdma and
                      ds707_extif_was_last_data_network_hdr.
10/02/03   vr         Changed function names.
09/16/03   vr         First version of file.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "ds707_defs.h"
#include "dec5000.h"

/*===========================================================================

                        D S 7 0 7 _ E X T I F

===========================================================================*/

/*===========================================================================
                          PUBLIC CONSTANTS
===========================================================================*/

#define DS707_MAX_PKT_CALLS          1

/*===========================================================================
                        PUBLIC DATA STRUCTURES
===========================================================================*/

/*-------------------------------------------------------------------------
  Different Bearer Tech types for which external modules can query the 
  max data rates. 
-------------------------------------------------------------------------*/
typedef enum
{
  DS707_EXTIF_INVALID_TECH,
  DS707_EXTIF_1X,
  DS707_EXTIF_HDR_REV0,
  DS707_EXTIF_HDR_REVA,
  DS707_EXTIF_HDR_REVB,
  DS707_EXTIF_MAX_BEARER_TECH
} ds707_extif_bearer_tech_type;

/*-------------------------------------------------------------------------
  System information - hrpd or ehrpd
-------------------------------------------------------------------------*/
typedef enum
{
  DS707_EXTIF_INVALID_SYSTEM,
  DS707_EXTIF_SYSTEM_NON_EPC,   /* HRPD system  */
  DS707_EXTIF_SYSTEM_EPC        /* eHRPD system */
} ds707_extif_system_info;

/*-------------------------------------------------------------------------
  Structure that will be used by the module querying for max bearer 
  tech rates.
-------------------------------------------------------------------------*/
typedef struct 
{
  ds707_extif_bearer_tech_type bearer_tech;
  uint32 max_tx_bearer_tech_rate;
  uint32 max_rx_bearer_tech_rate;
} ds707_extif_bearer_tech_rate_type;


/*===========================================================================
                        PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_EXTIF_PKT_IS_DORMANT

DESCRIPTION   Checks if there is a dormant packet data session on a CDMA 
              or HDR system.

DEPENDENCIES  None.

RETURN VALUE  TRUE if there is a dormant packet data session, 
              FALSE otherwise.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_extif_pkt_is_dormant( void );

/*===========================================================================
FUNCTION      DS707_EXTIF_INFORM_HDR_SESS_STATUS

DESCRIPTION   Informs the HDR protocols module whether the current is on
              HDR or not. This function is called everytime the session
              status changes.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_extif_inform_hdr_sess_status( void );

/*===========================================================================
FUNCTION      DS707_EXTIF_IS_PKT_DATA_SESS_ON_HDR

DESCRIPTION   Checks if there is a packet data session that is active or 
              dormant on an HDR system.

DEPENDENCIES  None.

RETURN VALUE  TRUE if a packet data session is active or dormant on HDR, 
              FALSE otherwise.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_extif_is_pkt_data_sess_on_hdr( void );

/*===========================================================================
FUNCTION      DS707_EXTIF_IS_PKT_DATA_SESS_ON_CDMA

DESCRIPTION   Checks if there is a packet data session that is active or 
              dormant on an CDMA system.

DEPENDENCIES  None.

RETURN VALUE  TRUE if a packet data session is active or dormant on CDMA, 
              FALSE otherwise.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_extif_is_pkt_data_sess_on_cdma( void );

/*===========================================================================
FUNCTION      DS707_EXTIF_WAS_LAST_DATA_NETWORK_CDMA

DESCRIPTION   Checks if there is a dormant or active packet data session that
              was previously on an CDMA system before the current system it
              is on.

DEPENDENCIES  None.

RETURN VALUE  TRUE if a packet data session is active or dormant and was on 
                   CDMA prior to the current system it is on,
              FALSE otherwise.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_extif_was_last_data_network_cdma( void );

/*===========================================================================
FUNCTION      DS707_EXTIF_IS_PPP_UP

DESCRIPTION   

DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_extif_is_ppp_up( void );

/*===========================================================================
FUNCTION      DS707_EXTIF_WAS_LAST_DATA_NETWORK_HDR

DESCRIPTION   Checks if there is a dormant or active packet data session that
              was previously on an HDR system before the current system it
              is on.

DEPENDENCIES  None.

RETURN VALUE  TRUE if a packet data session is active or dormant and was on 
                   HDR prior to the current system it is on,
              FALSE otherwise.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_extif_was_last_data_network_hdr ( void );

/*===========================================================================
FUNCTION      DS_EXTIF_VERIFY_SO

DESCRIPTION   Wrapper for DS_VERIFY_SO. Called by MC.  
	      Asks DS if the originating SO is correct.

DEPENDENCIES  None

RETURN VALUE  service option to orig with

SIDE EFFECTS  None.
===========================================================================*/
word ds707_extif_verify_so
(
  word so_to_verify           /* SO to verify                              */
);

/*===========================================================================
FUNCTION DS707_EXTIF_CHECK_PPP_RESYNC_LOCK()

DESCRIPTION
  This is a wrapper function for the dss707_check_ppp_resync_lock. It checks
  the PPP resync lock status (locked/unlocked) for all the ifaces (V4 and V6 
  returns a boolean (TRUE if locked and FALSE if unlocked). It also 
  registers a callback call back to notify the lock release event, if 
  it is currently locked.
  

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  boolean lock_status = TRUE  if locked 
                        FALSE if unlocked 

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds707_extif_check_ppp_resync_lock
(
  void (* ppp_resync_lock_release_cb)(void)  
);

/*===========================================================================
FUNCTION      DS707_EXTIF_GET_BEARER_TECH_MAX_RATE

DESCRIPTION   Returns the max rate that a bearer technology can carry. 
              This function can be used by other modules to query for a given
              technology.

DEPENDENCIES  None.

PARAMETERS    bearer_tech_query: The memory needs to be allocated by the 
                module querying.  Return values will be populated inside this
                structure. 

RETURN VALUE  TRUE: If the values are filled in.
              FALSE:  If the values are not filled in.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_extif_get_bearer_tech_max_rate
( 
  ds707_extif_bearer_tech_rate_type *bearer_tech_query 
);

/*===========================================================================
FUNCTION      DS707_EXTIF_IS_PKT_DATA_SESS_TRANFERABLE

DESCRIPTION   This function checks if the packet data session is tranferable. 
              On session change, HDR-CP queries this function, with the new
              system on which the session is negotiated. Since DS hasnt yet 
              processed the session change, the system info is what it was 
              before the session change. Depending on the new and old system
              this function returns true or false depending on whether the 
              session can be transfered or not

DEPENDENCIES  None.

RETURN VALUE  TRUE if packet data session is tranferable FALSE otherwise.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_extif_is_pkt_data_sess_transferable( 
  ds707_extif_system_info sys_info
);

/*===========================================================================

                        D S 7 0 7 _ S C R M

===========================================================================*/

/*===========================================================================
                        PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION     DS707_SCRM_MAX_ALLOWABLE_SCH_RATE

DESCRIPTION  Look at the table scrm_sch_rate_matrix[] for the max allowable
             rate the MSM can do in the Rev. SCH. 
             
             Do not use current_f_sch0, etc.  That's because mux updates
             itself asynchronously.  Use the values MUX returns, by
             using the mux information.

DEPENDENCIES None.

RETURN VALUE Max number of MuxPDUs.

SIDE EFFECTS None.
===========================================================================*/
byte ds707_scrm_max_allowable_sch_rate
(
  boolean             rxc_sch_data_is_valid,
  dec_rc_type         sch_rc,
  dec_sch_rate_type   dec_sch_rate,
  boolean             double_size,
  boolean             rxc_sup_dec_int_enable
);

/*===========================================================================

                        D S 7 0 7 _ S O _ P K T

===========================================================================*/

/*===========================================================================
                        PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================/
FUNCTION       DS707_SO_PKT_RECAL

DESCRIPTION    Re-does the service option page responses and the enable/
               disabling of the service options, based on the current 
               settings.

DEPENDENCIES   This function should be called only after ATCOP has been
               initialized, since it invokes functions that use some AT
               command settings.

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_so_pkt_recal(void);

/*===========================================================================

             D S 7 0 7 _ D A T A _ S E S S I O N _P R O F I L E

===========================================================================*/

/*===========================================================================
                        PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_DATA_SESSION_GET_AUTH_INFO_FROM_PROFILE

DESCRIPTION   The caller of this function provides the profile id. This API
              then retrieves the user id and pasword associated with that 
              profile and returns them back to the caller in supplied
              output parameters.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
int8 ds707_data_session_get_auth_info_from_profile
(
 uint8    profile_id,       /* Input Parameter */
 char   * user_id,          /* char array of max size PPP_MAX_USER_ID_LEN. OUTPUT param*/
 uint8  * user_id_len,      /* uint8 length value. OUTPUT param*/
 char   * auth_password,    /* char array of max size PPP_MAX_PASSWD_LEN. OUTPUT param */
 uint8  * auth_password_len /* uint length value. OUTPUT param */
);

/*===========================================================================

                             D S D C T M

===========================================================================*/

/*===========================================================================
                        PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================

FUNCTION DCTM_POST_SIP_FAIL

DESCRIPTION
  This function  posts call fail type to CM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dctm_post_sip_fail(void);

#endif /* DS707_EXTIF_H        */
