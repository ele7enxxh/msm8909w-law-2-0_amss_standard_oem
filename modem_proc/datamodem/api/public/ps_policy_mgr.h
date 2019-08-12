#ifndef PS_POLICY_MGR_H
#define PS_POLICY_MGR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       P S _ P O L I C Y _ M G R . H

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/**
   @file ps_policy_mgr.h
   @brief This file defines the API exported by policy manager module
 */

/*
EXTERNAL FUNCTIONS

  PS_POLICY_MGR_ADD_RECORD()
    Adds a new record in the Policy database

  PS_POLICY_MGR_CLEAN_DB()
    Cleans up the Policy database

  PS_POLICY_MGR_REG_GET_PROFILE_CBACK()
    Registers DS function for retrieving profile based on app_identifier
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the policyMgr group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*===========================================================================
Copyright (c) 2007-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_policy_mgr.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
-------     ---    ------------------------------------------------------------
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
05/06/11    sd     (Tech Pubs) Continued to edit Doxygen markup and comments.
12/01/10    sd     (Tech Pubs) Edited Doxygen markup and comments.
09/27/07    msr    Created the file.

=============================================================================*/
/* group: policyMgr */

/*=============================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

/*===========================================================================

                      EXTERNAL DATA DECLARATIONS

===========================================================================*/

/** @addtogroup constants
  @{
*/

/**
  Indicates the size of the ref_cnt array in the iface_private structure: [n].
*/
#define PS_POLICY_MGR_IFACE_PRIORITY_MAX      32
/**
  Indicates the maximum priority values supported per iface: [n - 1].
*/
#define PS_POLICY_MGR_PRIORITY_MAX            (PS_POLICY_MGR_IFACE_PRIORITY_MAX - 1)

/**
  Legacy priority value used for applications/users when the policy database is
  unintialized. Legacy priority has the highest priority, so it is not affected
  by arbitration.
 */
#define PS_POLICY_MGR_LEGACY_PRIORITY         PS_POLICY_MGR_PRIORITY_MAX

/**
  Used by policy manager users to indicate that the user is not 
  interested in matching the profile field while the priority is retrieved from 
  the policy manager database.
 */
#define PS_POLICY_MGR_PROFILE_DONT_CARE       0x7FFFFFFF

/**
  Used by users of the policy manager to indicate that the user is not 
  interested in matching the app_identifier field while the priority is 
  retrieved from the policy manager database.
 */
#define PS_POLICY_MGR_APP_DONT_CARE           0x7FFFFFFF

/**
  Defines the default entry behavior in the policy manager. The multiple default
  profile types include:\n
  - [ANY_APP, ANY_PROFILE] (Default: Entry matches any sort of inputs)
  - [particular app, ANY_PROFILE] 
  - [ANY_APP, particular profile] 

  Because inputs are not matched to any other entry in the database, default
  entries are accessed in the following priority (provided order is maintained).

   @code
   [app, ANY_PROFILE]      matches inputs  (APP_DONT_CARE, profile)
   [ANY_APP, profile]      matches inputs  (app, PROFILE_DONT_CARE)
   [ANY_APP, ANY_PROFILE]  matches inputs  (app, profile)
   @endcode 

  These ANY_* values cannot be used as input during get_priority and must only
  be used while filling entries in the database.

  The mode handler is responsible for the order in which the default profile
  entries are filled in the policy database. The policy manager by itself does 
  not monitor the order in which the records are filled in the database.
 */
#define PS_POLICY_MGR_ANY_PROFILE             0x7EFEFEFE

/**
  Indicates that the client of the policy manager is interested in matching any
  app_identifier field. 
 */
#define PS_POLICY_MGR_ANY_APP                 0x7FEFEFEF

/**
  Indicates the invaid values for the priority.
 */
#define PS_POLICY_MGR_PRIORITY_INVALID        (-1)
/**
  Indicates the invaid values for the profile.
*/
#define PS_POLICY_MGR_PROFILE_INVALID         (-1)

#define PS_POLICY_MGR_MAX_NUM_PROFILE_SUPPORTED 16
#define PS_POLICY_MGR_MIN_NUM_DEFAULT_ENTRIES    1

#define PS_POLICY_MGR_MAX_POLICY_ENTRIES        \
          (PS_POLICY_MGR_MAX_NUM_PROFILE_SUPPORTED + \
           PS_POLICY_MGR_MIN_NUM_DEFAULT_ENTRIES)
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/**
  Callback type registered by the DS for retrieving the profile based on the 
  application identifier.
*/
typedef int32 (* get_app_profile_cb)(uint32 app_identifier);
/** @} */ /* end_addtogroup datatypes */
/*=============================================================================

                        PUBLIC FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================
FUNCTION PS_POLICY_MGR_ADD_RECORD()
=============================================================================*/
/** @ingroup ps_policy_mgr_add_record
  Adds an entry in the policy database.

  @param[in] app_identifier Application identifier value.
  @param[in] profile_id     Profile ID of the application.
  @param[in] priority       Priority of the application profile.

  @return
  0  -- Success.\n 
  -1 -- Failure.

  @dependencies
  None.
  @newpage
*/
int32 ps_policy_mgr_add_record
(
  int64  app_identifier,
  int32  profile_id,
  int32  priority
);

/*===========================================================================
FUNCTION PS_POLICY_MGR_CLEAN_DB()
===========================================================================*/
/** @ingroup ps_policy_mgr_clean_db
  Cleans up the policy database.

  @return
  None.

  @dependencies
  None.
  @newpage
*/
void ps_policy_mgr_clean_db
(
  void
);

/*===========================================================================
FUNCTION PS_POLICY_MGR_REG_GET_PROFILE_CBACK()
===========================================================================*/
/** @ingroup ps_policy_mgr_reg_get_profile_cback 
  Registers the callback from DS to get the application profile based on
  the application identifier.

  @datatypes
  #get_app_profile_cb

  @vertspace
  @param[in] cback_f_ptr  Pointer to the DS function to be registered.

  @return
  None.

  @dependencies
  None.
  @newpage
*/
void ps_policy_mgr_reg_get_profile_cback
(
  get_app_profile_cb cback_f_ptr
);


#endif /* PS_POLICY_MGR_H */
