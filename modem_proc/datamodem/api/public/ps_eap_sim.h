#ifndef PS_EAP_SIM_H
#define PS_EAP_SIM_H

/** 
  @file ps_eap_sim.h 
  @brief 
   This file contains EAP-SIM processing functions.
*/
/*===========================================================================
        E A P  S U B S C R I B E R  I D E N T I T Y  M O D U L E
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


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_eap_sim.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
02/24/11    jee     Added Doxygen comments
12/19/08    pp      Common Modem Interface: Public/Private API split.
05/02/08    ifk     Moved EAP-AKA/SIM code to CDPS.
05/13/07    lti     Added header for sim/aka internals
02/26/09    lti     Moved internal function declarations to source file
11/14/06    lti     Code review changes and restructuring
06/08/06    lti     Created module.

===========================================================================*/
/* group: eap */


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

               EAP SIM DATA DECLARATIONS AND DEFINITIONS

===========================================================================*/

/*===========================================================================

                     EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION EAP_SIM_INIT
===========================================================================*/
/** @ingroup eap_sim_init
  Initializes the EAP-SIM software entity.

  @return 
  None.

  @dependencies 
  None.
  
  @sideeffects
  This function registers for PEAP packets over the EAP transport.
  @newpage
*/
void eap_sim_init
(
  void
);


#endif /* PS_EAP_SIM_H */
