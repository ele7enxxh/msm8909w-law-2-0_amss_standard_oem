#ifndef WWCOEX_BA_H
#define WWCOEX_BA_H
/*!
  @file wwcoex_ba.h

  @brief
   Provides band avoidance specific functionality to the Limits Manager Layer

*/

/*=============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/wwan_coex/inc/wwcoex_ba.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/28/14   jm      Initial Revision

=============================================================================*/

#include "wwan_coex_mgr.h"
#include "wwan_coex.h"

/*=============================================================================

  FUNCTION:  wwcoex_ba_gen_blacklist

=============================================================================*/
/*!
    @brief
    Generate blacklist based off of two techs
 
    @details
    This should only be called when serving frequency, serving power,
    or a blacklist response has been sent from Tech L1
 
    @return
    newBlacklist will be modified with the new blacklist data
*/
/*===========================================================================*/
void wwcoex_ba_gen_blacklist
(
  cxm_tech_type bl_tech,              /* Tech that will be sent blacklist */
  lmtsmgr_tech_ba_type* bl_tech_info, /* Freq/Power info of blacklisted tech */
  cxm_tech_type st_tech,              /* Tech that will remain static */
  lmtsmgr_tech_ba_type* st_tech_info, /* Freq/Power info of static tech */
  coex_ba_blist_data* newBlacklist    /* Blacklist to be updated */
);

/*=============================================================================

  FUNCTION:  wwcoex_ba_comp_blacklists

=============================================================================*/
/*!
    @brief
    Compares the contents of the blacklists and determines if the data is
    the same.
 
    @details
    All fields are compared except for the blacklist id
    Assumption is that blacklists have already been sorted
 
    @return
    TRUE  - both blacklists have the same data
    FALSE - blacklists have different data
*/
/*===========================================================================*/
boolean wwcoex_ba_comp_blacklists
(
  coex_ba_blist_data *blist1,
  coex_ba_blist_data *blist2
);

#endif /* WWCOEX_BA_H */
