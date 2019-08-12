#ifndef PS_PPPI_AUTH_H
#define PS_PPPI_AUTH_H
/*===========================================================================

                          P S _ P P P I _ A U T H . H

DESCRIPTION
  Header file for PPP authentication (Internal)

Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/MM_DATA/ps_ppp_auth.h_v 1.3 21 Feb 2003 20:42:56   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_pppi_auth.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/01/12    sb     Always On UIM Feature Cleanup
10/27/11    op     Added support for PDN level authentication
05/07/09    pp     Initial release. Created as part of CMI Phase-4: SU Level 
                   API Effort [Split from ps_ppp_auth.h].
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "ps_ppp_fsm.h"
#include "ps_ppp.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#ifdef FEATURE_STA_PPP
/*---------------------------------------------------------------------------
TYPEDEF PPP_AUTH_TYPE_ENUM_TYPE

DESCRIPTION
  Enum defining a subset of PPP constants that can acessed by the sta 
  framework
---------------------------------------------------------------------------*/
typedef enum
{
  PPP_AUTH_PASSWD_ENCRYPT = 0,
  PPP_AUTH_TIMEOUT        = 1,
  PPP_AUTH_RETRY_COUNT    = 2,
  PPP_AUTH_RETRY_LIMIT    = 3,
  PPP_AUTH_MAX_PARAM                       /* max value - array sizing      */
} ppp_auth_type_enum_type;
#endif /* FEATURE_STA_PPP */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                        PUBLIC FUNCTION DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#ifdef FEATURE_RUIM_FUNC_INTERFACE
/*===========================================================================
FUNCTION PPP_AUTH_INIT()

DESCRIPTION
   This function is called at system initialization time.  It initializes the
   the authorization configuration parameters for MD5 or CAVE authorization 
   on the interface specified. As a part of the authentication procedure the 
   MMGDS library is called to register a client id for each interface. 
   The default values for all the other parameters are populated as well 
   on a one time basis


RETURN VALUE
  int: -1 on failure
        0 on success

DEPENDENCIES
  To be used only when the FEATURE_UIM_CAVE_AN_AUTH feature is enabled.

SIDE EFFECTS
  None
===========================================================================*/
int ppp_auth_init
(
  void
);
#endif /* FEATURE_RUIM_FUNC_INTERFACE */

/*===========================================================================
FUNCTION PPP_AUTH_SET_DEVICE_CONSTANTS()

DESCRIPTION
  This function is called to set the configurable protocol constants for
  AUTH on the interface specified.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Set configurable protocol constants for AUTH.
===========================================================================*/
void ppp_auth_set_device_constants
(
  ppp_dev_enum_type           device,       /* the interface to initialize */
  nv_ppp_configure_data_type *config
);

/*===========================================================================
FUNCTION PPP_AUTH_INIT_DEVICE_CONSTANT()

DESCRIPTION
  This function is called to set the configurable protocol constants for
  AUTH on the interface specified.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Set configurable protocol constants for AUTH.
===========================================================================*/
void ppp_auth_init_device_constant
(
  ppp_dev_enum_type           device,       /* the interface to initialize */
  ppp_init_consts_enum_type constant,
  uint32 value
);

/*===========================================================================
FUNCTION PPP_AUTH_START()

DESCRIPTION
  This function checks for authentication and kicks it off if it was
  negotiated, otherwise it starts IPCP.

PARAMETERS
  device: the device on which auth is being started.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_auth_start
(
  ppp_dev_enum_type device
);

/*===========================================================================
FUNCTION PPP_AUTH_COMPLETE()

DESCRIPTION
  This function completes the PPP authentication phase and starts the PPP
  network phase.  This includes cleaning up of any data that was stored.

PARAMETERS
  ppp_cb_ptr: ppp control block pointer for completed authentication
  protocol:   protocol which completed (for protocol specific cleanup).  

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Cleans the buffer for the incoming packet and returns buffer to free q.
  Changes ppp_cb_ptr->flags to show that authentication is complete.
===========================================================================*/
void ppp_auth_complete
(
  ppp_type *ppp_cb_ptr,                     /* ptr to PPP control block    */
  uint16    protocol                        /* the completed auth protocol */
);

/*===========================================================================
FUNCTION PPP_AUTH_TIMEOUT()

DESCRIPTION
  This function will be called by other functions on time out.

PARAMETERS
  ppp_cb_ptr: ppp control block pointer for timeout.

RETURN VALUE
  TRUE:  Informs the calling function to retry
  FALSE: Informs calling function, retries are over and to
         terminate the call.

DEPENDENCIES
  Same as dependencies of the functions ppp_chap_timeout

SIDE EFFECTS
  If number of the retries are non-zero then an authentication packet is
  transmitted.
===========================================================================*/
boolean ppp_auth_timeout
(
  ppp_type *ppp_cb_ptr                         /* ptr to PPP control block */
);

/*===========================================================================
FUNCTION PPP_AUTH_GET_RAND_CHAP_CHAL_VALUE()

DESCRIPTION
  This function generates a random value of size challenge_len and places
  this value in challenge_value

PARAMETERS
  challenge_value
  challenge_len

RETURN VALUE 
  None 

DEPENDENCIES
  None

SIDE EFFECTS 
  None  
===========================================================================*/
void ppp_auth_get_rand_chap_chal_value
(
  unsigned char *challenge_value,
  uint8          challenge_len
);

/*===========================================================================
FUNCTION PPP_AUTH_CALC_MD5_CHAP_RESP_VALUE()

DESCRIPTION
  This function calculates the MD5 hash value of the passed in params

PARAMETERS
  uint8          identifier,
  unsigned char *password,
  uint8          password_len,
  unsigned char *chap_chal_value,
  uint8          chap_chal_value_len,
  unsigned char *chap_resp_value

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS 
  Calculated value stored in chap_resp_value
===========================================================================*/
void ppp_auth_calc_md5_chap_resp_value
(
  uint8          identifier,
  unsigned char *password,
  uint8          password_len,
  unsigned char *chap_chal_value,
  uint8          chap_chal_value_len,
  unsigned char *chap_resp_value
);

#ifdef FEATURE_STA_PPP
/*===========================================================================
FUNCTION AUTH_GETCONST()

DESCRIPTION
 To get auth constants from the phone

PARAMETERS
 ppp_cb_ptr    : pointer to ppp_cb
 ppp_auth_enum : the fsm constant to fetch

RETURN VALUE
 Integer value of the constant, -1 in case of an error.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int auth_getconst
(
  ppp_type              * ppp_cb_ptr,
  ppp_auth_type_enum_type ppp_auth_enum
);

/*===========================================================================
FUNCTION AUTH_SETCONST()

DESCRIPTION
 To set auth constants on the phone

PARAMETERS
 ppp_cb_ptr    : pointer to ppp_cb
 ppp_auth_enum : the auth constant to set
 value         : byte-array of data for new value

RETURN VALUE
 0 for success, -1 in case of an error.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int auth_setconst
(
  ppp_type              * ppp_cb_ptr,
  ppp_auth_type_enum_type ppp_auth_enum,
  uint32 value
);
#endif /* FEATURE_STA_PPP */
#endif /* PS_PPPI_AUTH_H */
