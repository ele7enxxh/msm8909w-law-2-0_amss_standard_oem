#ifndef HDRIS890_H
#define HDRIS890_H

/*===========================================================================

       H D R  E N H A N C E D  T E S T   A P P L I C A T I O N

                          D E F I N I T I O N S

GENERAL DESCRIPTION
  Top level module of the IS890A.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  hdris890a_init() has to be called before 
===========================================================================*/

/**
  @file
  @brief
  Contains HDR enhanced test application definitions based on the IS-890-A Test
  Application Protocol from 3GPP2 specification C.S0029.
*/

/*===========================================================================
Copyright (c) 2005, 2007-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/tap/api/hdris890.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/14/12   smd     Moved it from modem/hdr/api.
09/22/10   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
10/01/09   wsh     Added redirection defines to API header 
08/27/09   wsh     CMI:Split HDRIS890 due to external reference in OEMCM.c
06/12/08   etv     Added constant for invalid channel frequency.
08/07/07   cws     Added MC connected stats.
08/01/07   grl     Fixed RCVT 2.2 compiler warnings.
10/28/05   jyw     Added function hdris890a_update_num_tx_bytes.
09/27/05   jyw     Fixed bug in the macro definitions.
09/09/05   jyw     General clean up/more comments.
09/08/05   jyw     Replaced TAP0 with ETAP.
08/02/05   jyw     General Clean up.
07/26/05   jyw     Added the revision header.
05/19/05   jyw     Created this file

===========================================================================*/

/*========================================================================== 
                    INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

/** @addtogroup hdr_api High Data Rate API
    @{ */

/*===========================================================================

                    DATA DECLARATIONS FROM IS-890A

===========================================================================*/

/*===========================================================================

                  REDIRECTION FOR THE IS890 

===========================================================================*/
/** @name IS-890 Redirection
    Redirection for legacy IS-890 compliant code to IS-890-A compliant code.
    @{ */

#define hdris890_is_in_traffic     hdris890a_is_in_traffic
#define hdris890_get_num_rx_bytes  hdris890a_get_num_rx_bytes
#define hdris890_get_num_tx_bytes  hdris890a_get_num_tx_bytes

/** @} */

/*===========================================================================

               HDRIS890A EXTERNAL FUNCTION DECLARATIONS 

===========================================================================*/

/** 
  Checks whether is890a (IS-890-A Test Application Protocol, 3GPP2 specification
  C.S0029) is currently negotiated on any stream and in the traffic state. 

  @return
      TRUE -- is890a is currently in the traffic state.\n
      FALSE -- is890a is currently not in the traffic state.

  @dependencies
      None.
*/ 
boolean hdris890a_is_in_traffic( void );

/** 
  Returns the total number of bytes transmitted by Reverse Test Application
  Protocol (RTAP) and Forward Test Application Protocol (FTAP) since the last
  activation.

  @return
      Number of transmitted bytes.

  @dependencies
      None.
*/ 
uint32 hdris890a_get_num_tx_bytes( void );

/** 
  Returns the total number of bytes received
  by RTAP and FTAP since the last activation.

  @return
      Number of received bytes.

  @dependencies
      None.
*/ 
uint32 hdris890a_get_num_rx_bytes( void );

/** @} */ /* end_addtogroup hdr_api */

#endif /* HDRIS890_H */
