/*===========================================================================

                 H D R  F E T A P  D E F I N I T I O N S

GENERAL DESCRIPTION
  This file contains the defintiions for the HDR forward link
  enhanced test application (FETAP).

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005, 2006, 2008 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/tap/inc/hdrftap.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/16/11   wsh     Reduce heap mem usage by moving memory back to static
08/05/08   rmg     Added support for HDRIND_CCMAC_SCC_PKT_RXED indication.
02/16/06   jyw     Added hdrfetap_disable_tc_ofs.
09/09/05   jyw     Interface clean up to be compatible with TAP0.
09/08/05   jyw     Replaced TAP0 with ETAP.
07/26/05   jyw     Cleaned up the interface.
05/19/05   jyw     create the file

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"

#include "hdr_variation.h"
#if defined ( FEATURE_HDR_IS890 ) || defined ( FEATURE_HDR_IS890A )

#include "rex.h"

#include "hdrhai.h"
#include "hdrind.h"
#include "hdrerrno.h"

/*===========================================================================

              DEFINITIONS AND CONSTANTS FROM HAI Vol. III

===========================================================================*/

#define HDRHAI_IS890A_FETAP HDRHAI_IS890_FTAP

/*===========================================================================

                DEFINITION FOR THE FTAP REDIRECTION 

===========================================================================*/
#define hdrftap_deactivate      hdrfetap_deactivate
#define hdrftap_ind_cb          hdrfetap_ind_cb
#define hdrftap_process_ind     hdrfetap_process_ind
#define hdrftap_msg_cb          hdrfetap_msg_cb
#define hdrftap_process_sig     hdrfetap_process_sig
#define hdrftap_get_pkt         hdrfetap_get_pkt
#define hdrftap_put_pkt         hdrfetap_put_pkt
#define hdrftap_param_assign_cb hdrfetap_param_assign_cb

/*===========================================================================

                    EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRFTAP_ACTIVATE

DESCRIPTION
  This function activates the FETAP once it has been negotiated
  by the stream protocol.

DEPENDENCIES
  None
  
PARAMETERS
  stream_id -  Stream ID negotiated for FETAP.
  app_subtype - Negotiated application subtype for TAP.

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfetap_activate
(
  uint8  fetap_stream_id ,
  uint16 app_subtype
);

/*===========================================================================

FUNCTION HDRFTAP_DEACTIVATE

DESCRIPTION
  This function deactivates the FETAP.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfetap_deactivate( void );

/*===========================================================================

FUNCTION HDRFETAP_IS_IN_TRAFFIC

DESCRIPTION
  This function returns TRUE if the fetap is negotiated on
  any stream and in traffic state.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  TRUE or FALSE
  
SIDE EFFECTS
  None
===========================================================================*/
boolean hdrfetap_is_in_traffic( void );

/*===========================================================================

FUNCTION HDRFETAP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to the fetap.

DEPENDENCIES
  None
 
PARAMETERS
  ind_name      - Indication to process, is a global enum for all
                  possible indications in HDR signaling
  ind_data_ptr  - Optional data pointer for indication specific data
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfetap_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void                      *ind_data_ptr 
);

/*===========================================================================

FUNCTION HDRFETAP_PROCESS_IND

DESCRIPTION
  This function processes indications for the FETAP.

DEPENDENCIES
  None
 
PARAMETERS
  ind_name      - Indication to process, is a global enum for all
                  possible indications in HDR signaling
  ind_data_ptr  - Optional data pointer for indication specific data
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfetap_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type *ind_data_ptr 
);

/*===========================================================================

FUNCTION HDRFETAP_MSG_CB

DESCRIPTION
  This function is registered to be called whenever HMP receives a
  message for the FETAP.

DEPENDENCIES
  None
 
PARAMETERS
  msg_ptr - DSM item chain containing the message
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfetap_msg_cb
(
  dsm_item_type* msg_ptr
);

/*===========================================================================

FUNCTION HDRFETAP_PROCESS_SIG

DESCRIPTION
  This function is the signal handler for the FETAP.

DEPENDENCIES
  None
 
PARAMETERS
  signal - REX signal
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfetap_process_sig
(
  rex_sigs_type signal
);

/*===========================================================================

FUNCTION HDRFETAP_PUT_PKT

DESCRIPTION
  This function is called by hdretas_put_pkt() to deliver
  FETAP receive data.

DEPENDENCIES
  None
 
PARAMETERS
  rx_pkt_ptr    - DSM item chain pointer with received packet
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfetap_put_pkt
(
  dsm_item_type*   rx_pkt_ptr
);

/*===========================================================================

FUNCTION HDRFETAP_DISABLE_TC_OFS

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

void  hdrfetap_disable_tc_ofs
(
  boolean tc_ofs_disable
);
#endif    /* FEATURE_HDR_IS890 || FEATURE_HDR_IS890A */

