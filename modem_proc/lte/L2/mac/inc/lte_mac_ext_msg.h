/*!
  @file
  lte_mac_ext_msg.h

  @brief
  This file describes the interfaces to LTE Medium Access Control (MAC)

   <!-- Doxygen chokes on the @mainpage vs. @page inside a conditional, but the
       following seems to work.  Basically, make this the mainpage if it
       is the only component being built, otherwise, just make it a page and
       let the upper level component/s @ref it. -->
  @if mac
  @mainpage LTE Medium Access Control (MAC)
  @endif
  @ifnot mac
  @page LTE Medium Access Control  (MAC)
  @endif

  MAC is part of L2. Its core functionality includes Random Access Procedure, <br>
  Prioritization, Multiplexing and Data Transmission on UL and Data reception on the DL.<br><br>

  MAC interacts with RRC for the configuration, RLC for buffer status query, PDU building, <br>
  L1 for interaction regarding the RACH procedure, for grants, A2 for writing to and reading data from.<br><br>

  For the time critical UL transmission there is is functional call API between MAC and RLC. <br>
  This is the only exception to usual message passing interface.

  <h2>High level MAC Task architecture</h2>
  <img src='../pics/TaskArch.JPG'>

  <h2>CS architecture</h2>
  <img src='../pics/CS_Arch.JPG'>

   <h2>Module Decomposition</h2>
  <img src='../pics/ModuleDecomp.JPG'>
*/


/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who       what, where, why
--------   ---       ----------------------------------------------------------
09/15/08   sk        Initial version
===========================================================================*/

#ifndef LTE_MAC_EXT_MSG_H
#define LTE_MAC_EXT_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


#endif /* LTE_MAC_EXT_MSG_H */

