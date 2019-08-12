#ifndef PS_EAP_AKA_H
#define PS_EAP_AKA_H

/** 
  @file ps_eap_aka.h 
  @brief 
  This file contains EAP-AKA processing functions.
*/
/*===========================================================================
        E A P  A U T H E N T I C A T I O N   K E Y   A G R E E M E N T
                        F O R  W L A N
                
===========================================================================*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the eap group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*=============================================================================
Copyright (c) 2006-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_eap_aka.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
02/24/11    jee     Added Doxygen comments
12/19/08    pp      Common Modem Interface: Public/Private API split.
05/02/08    ifk     Moved EAP-AKA/SIM code to CDPS.
11/14/06    lti     Code review changes and restructuring
06/08/06    lti     Created module.

===========================================================================*/
/* group: eap */

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "ps_eap.h"

/*===========================================================================

                     EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION EAP_AKA_POWERUP_INIT
===========================================================================*/
/** @ingroup eap_aka_powerup_init
  Initializes the EAP-AKA software entity during power-up.

  @return 
  None.

  @dependencies 
  None.
  
  @sideeffects
  This function registers for EAP-AKA packets over the EAP transport.
  @newpage
*/
void eap_aka_powerup_init
(
  void
);

/*===========================================================================
FUNCTION EAP_AKA_INIT
===========================================================================*/
/** @ingroup eap_aka_init
  Initializes the EAP-AKA software entity.

  @return 
  None.

  @dependencies 
  None.
  
  @sideeffects
  This function registers for EAP-AKA packets over the EAP transport.
  @newpage
*/
void eap_aka_init
(
  void
);


#endif /* PS_EAP_AKA_H */
