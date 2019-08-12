#ifndef PS_PPPAUTH_H
#define PS_PPPAUTH_H
/*===========================================================================

                          P S _ P P P _ A U T H . H

DESCRIPTION
  Header file for PPP authentication

Copyright (c) 1999-2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/MM_DATA/ps_ppp_auth.h_v 1.3 21 Feb 2003 20:42:56   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/api/ps_ppp_auth.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/23/11    op     Data SU API cleanup
03/30/10    ssh    FEATURE_DATA_PS_PAP_INTERNAL_AUTH: Gobi mainlining
08/31/09    mga    Merged from eHRPD branch
05/07/09    pp     CMI Phase-4: SU Level API Effort.
09/02/08    rnp    Added STA related changes.
01/22/08    scb    Mainlined features under FEATURE_JCDMA_1X
08/18/07    scb    Featurized the MMGSDI interaction of RUIM code using the
                   FEATURE_RUIM_FUNC_INTERFACE feature.
05/20/07    scb    Added RUIM support for CHAP authentication in 7500
04/19/07    scb    Added a KDDI PPP compliance requirement to configure
                   Application specific AUTH constants
10/24/03    ifk    Added prototype for ppp_auth_set_device_constants()
09/12/03    mvl    Added some JCDMA timing changes.
07/02/03    mvl    Moved some field length defines from .c to .h
04/08/03    mvl    Moved PPP_CHAP_MD5 to ps_ppp_defs.h
04/06/03    mvl    Addition of capabilities to support server style
                   authentication for both PAP and CHAP. 
04/02/03    mvl    Complete modularization of the authentication code.
02/21/03    jd     moved & renamed ppp_auth_get_from_nv() from this file to
                   to is707_get_ppp_auth_info_from_nv() in ps707_dssnet.h
02/06/03    jd     Renamed ppp_auth_get_info() to ppp_auth_get_from_nv()
01/31/03    jd     Added ppp_auth_get_info() prototype
08/02/02    mvl    moved authentication declarations here.
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ps_ppp_fsm.h"
#include "ps_ppp.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Message Id's for PAP and CHAP packets
---------------------------------------------------------------------------*/
#define PPP_PAP_REQ         0x01      /* Authenticate Request              */
#define PPP_PAP_ACK         0x02      /* Authenticate Acknowledge          */
#define PPP_PAP_NAK         0x03      /* Authenticate Nak                  */
#define PPP_CHAP_CHALLENGE  0x01      /* Code for CHAP Challenge           */
#define PPP_CHAP_RESPONSE   0x02      /* Code for CHAP Response            */
#define PPP_CHAP_SUCCESS    0x03      /* Code for CHAP Success             */
#define PPP_CHAP_FAILURE    0x04      /* Code for CHAP Failure             */

/*---------------------------------------------------------------------------
  Length of the various fields in Auth packets
---------------------------------------------------------------------------*/
#define PPP_AUTH_CODE_SIZE     1
#define PPP_AUTH_ID_SIZE       1
#define PPP_AUTH_LENGTH_SIZE   2
#define PPP_CHAP_VALUE_SIZE    1
#define PPP_PAP_UID_SIZE       1
#define PPP_PAP_PWD_SIZE       1

/*---------------------------------------------------------------------------
  Size of the MD5 output. This is 16 bytes.
---------------------------------------------------------------------------*/
#define PPP_CHAP_MD5_SIZE           16

/*===========================================================================
FUNCTION CHAP_PROC()

DESCRIPTION
  This function does the protocol processing for CHAP.

PARAMETERS
  device:       the PPP interface the packet arrived on
  item_ref_ptr: reference to pointer to dsm chain containing the CHAP pkt

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void chap_proc
(
  ppp_dev_enum_type   device,
  dsm_item_type     **item_ref_ptr
);

/*===========================================================================
FUNCTION PAP_PROC()

DESCRIPTION
  This function does the protocol processing for PAP.

PARAMETERS
  device:       the PPP interface the packet arrived on
  item_ref_ptr: reference to pointer to dsm chain containing the PAP pkt.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pap_proc
(
  ppp_dev_enum_type   device,
  dsm_item_type     **item_ref_ptr
);

/*===========================================================================
FUNCTION PPP_EAP_PROC

DESCRIPTION
  This will process EAP packets

PARAMETERS
  ppp_dev_enum_type device
  dsm_item_type **item_ref_ptr

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS 
  None  
===========================================================================*/

void ppp_eap_proc
(
  ppp_dev_enum_type device,
  dsm_item_type **item_ref_ptr
);

void pap_internal_proc
(
  ppp_dev_enum_type   device,
  dsm_item_type     **item_ref_ptr
);

#endif /* PS_PPPAUTH_H */
