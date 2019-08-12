/*===========================================================================

                 H D R  R E T A P  D E F I N I T I O N S

GENERAL DESCRIPTION
  This file contains the defintiions for the HDR reverse link
  enhanced test application (RETAP).

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005, 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/tap/inc/hdrrtap.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/21/07   cws     Removed RTCMAC interface functions.
02/16/06   jyw     Added hdrretap_rtcmac3_roll_seqno
09/09/05   jyw     Interface clean up to be compatible with TAP0
09/08/05   jyw     Replaced TAP0 with ETAP.
08/02/05   jyw     Cleaned up the interface inclusion.
07/26/05   jyw     Added the revision header.
05/20/05   jyw     Created the file

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"

#include "hdr_variation.h"
#if defined ( FEATURE_HDR_IS890A) || defined ( FEATURE_HDR_IS890 )

#include "rex.h"

#include "hdrhai.h"
#include "hdrind.h"
#include "hdrerrno.h"

/*===========================================================================

              DEFINITIONS AND CONSTANTS FROM HAI Vol. III

===========================================================================*/

/*===========================================================================

               DEFINITION FOR THE RTAP REDIRECTION 

===========================================================================*/
#define hdrrtap_ind_cb              hdrretap_ind_cb
#define hdrrtap_process_ind         hdrretap_process_ind
#define hdrrtap_msg_cb              hdrretap_msg_cb
#define hdrrtap_process_sig         hdrretap_process_sig
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRRETAP_ACTIVATE

DESCRIPTION
  This function activates the RETAP once it has been negotiated
  by the stream protocol.

DEPENDENCIES
  None
  
PARAMETERS
  retap_stream_id -  Stream ID negotiated for RTAP

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrretap_activate
( 
  uint8 retap_stream_id,
  uint16 app_subtype
);

/*===========================================================================

FUNCTION HDRRETAP_DEACTIVATE

DESCRIPTION
  This function deactivates the RETAP and frees the fill packet.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrretap_deactivate( void );

/*===========================================================================

FUNCTION HDRRETAP_IS_IN_TRAFFIC

DESCRIPTION
  This function returns TRUE if the RETAP is negotiated on
  any stream AND in traffic state.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  TRUE or FALSE
  
SIDE EFFECTS
  None
===========================================================================*/
boolean hdrretap_is_in_traffic( void );

/*===========================================================================

FUNCTION HDRRETAP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to the RETAP.

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
void hdrretap_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void *ind_data_ptr 
);

/*===========================================================================

FUNCTION HDRRETAP_PROCESS_IND

DESCRIPTION
  This function processes the indications for the RETAP.

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
void hdrretap_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  void *ind_data_ptr 
);

/*===========================================================================

FUNCTION HDRRETAP_MSG_CB

DESCRIPTION
  This function is registered to be called whenever HMP receives a
  message for the RETAP.

DEPENDENCIES
  None
 
PARAMETERS
  msg_ptr - DSM item chain containing the message
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrretap_msg_cb
(
  dsm_item_type* msg_ptr
);

/*===========================================================================

FUNCTION HDRRETAP_PROCESS_SIG

DESCRIPTION
  This function is the signal handler for the RETAP;

DEPENDENCIES
  None
 
PARAMETERS
  signal - REX signal
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrretap_process_sig
(
  rex_sigs_type signal
);

/*===========================================================================

FUNCTION HDRRETAP_PUT_PKT

DESCRIPTION
  This function is registered with PCP to handle received packets
  for RETAP. Currently IS-890 does not define any forward link
  packets for RTAP.

DEPENDENCIES
  None
 
PARAMETERS
  rx_pkt_ptr points to the DSM item containing the message
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrretap_put_pkt
(
  dsm_item_type*   rx_pkt_ptr
);

/*===========================================================================

FUNCTION HDRFTAP_DISABLE_TC_OFS

DESCRIPTION
  The function modified the tc_ofs_enable based on the dip switch set. 

DEPENDENCIES
  None
 
PARAMETERS
  None
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrretap_disable_tc_ofs
(
  boolean tc_ofs_disable
);

#endif    /* FEATURE_HDR_IS890A || FEATURE_HDR_IS890 */
