#ifndef PS_LL_ADDR_MGR_H
#define PS_LL_ADDR_MGR_H
/*===========================================================================

                  P S _ L L _ A D D R _ M G R . H

===========================================================================*/

/**
   @file ps_ll_addr_mgr.h
   @brief This is the header file for the PS Link-Local Address manager.

   This entity is responsible for the dynamic configuration of IPv4 Link-Local
   addresses per RFC 3927.

   This is particularly useful on a link where connectivity is restricted to
   `nodes' on the same link, e.g., in Ad Hoc networks.

   @note1hang The 169.254/16 addresses registered with IANA for this purpose 
   are not routable addresses.

   @par Caution 
   Use of this module for IP address configuration may cause some
   applications to fail.
 */

/*
EXTERNALIZED FUNCTIONS

  ll_addr_mgr_start()
      Called to start the link-local address manager

  ll_addr_mgr_stop()
      Called to stop the link-local address manager and thereby release the
    address acquired

  ll_addr_mgr_claim()
      Called to acquire a link-local address
*/

/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the linkLayer group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*=============================================================================
Copyright (c) 2006-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_ll_addr_mgr.h#1 $
  $Author: mplcsds1 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
05/07/11    SCD    (Tech Pubs) Continued edit.
12/01/10    SCD    (Tech Pubs) Edited Doxygen markup and comments.
12/01/10    sa     Modified the comments according to doxygen specific 
                   semantics.
12/17/08    pp     CMI: Public/Private API split.
06/23/06    lyr    Created module
===========================================================================*/
/* group: linkLayer */
/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ps_iface.h"

/*===========================================================================

                          FORWARD DECLARATIONS

===========================================================================*/


/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/** @addtogroup datatypes 
  @{
*/

/*---------------------------------------------------------------------------
TYPEDEF LL_ADDR_MGR_IND_ENUM_TYPE
---------------------------------------------------------------------------*/
/**
  Status indications clients can receive from the LL address manager.
*/
typedef enum
{
  /** @cond
  */
  LL_ADDR_MGR_MIN_IND               = -1, /**< Internal use only. */
  /** @endcond */

  LL_ADDR_MGR_IP_CLAIMED_IND        =  0, 
    /**< IP is established/re-established. */
  LL_ADDR_MGR_IP_UNAVAIL_IND        =  1, 
    /**< IP address is unavailable. */
  LL_ADDR_MGR_IP_CHANGED_IND        =  2, 
    /**< IP address is changed because of conflict (defending) or starting 
         over. */

  LL_ADDR_MGR_MAX_IND                     
    /**< Used only for a boundary check. */

} ll_addr_mgr_ind_enum_type;


/** 
  Registers a callback with the LL address manager for indications.
*/
typedef void (* ll_addr_mgr_ind_cback_type)
(
  ll_addr_mgr_ind_enum_type     ind,          
    /**< Indication being posted. */
  void                         *user_data_ptr 
    /**< Pointer to the user data given at registration. */  
);


/*---------------------------------------------------------------------------
TYPEDEF LL_ADDR_MGR_PARAMS_TYPE
---------------------------------------------------------------------------*/
/**
  @brief Starts the LL address manager.
 */
typedef struct
{
  uint32   acquisition_retries;
    /**< Number of attempts to acquire an IP address before giving up and 
         reporting to the client. This is not specified in the RFC; however it
         prevents long waits to get an IP address.\n
         The RFC requires a 99.96% chance of succeeding within two retries.\n
         Each retry takes approximately 7 seconds:\n
@code
PROBE_WAIT[1sec] + ( (PROBE_NUM[3] - 1) * PROBE_MAX[2sec] ) + ANNOUNCE_WAIT[2sec]
@endcode
         Suggest two attempts.
           */
  uint64   mac_addr; /**< Seed for generating the random number. */
} ll_addr_mgr_params_type;

/**
  @brief Passes start data to the PS context.
*/
typedef struct
{
  ps_iface_type                 *iface_ptr;     
    /**< iface pointer of the client. */
  ll_addr_mgr_params_type        params;        
    /**< Related parameters. */
  ll_addr_mgr_ind_cback_type     ind_cback;     
    /**< Callback for notification. */
  void                          *user_data_ptr; 
    /**< User data for the caller. */
  uint32                         ll_addr_mgr_sm_cb_instance;
    /**< Pointer to the LL address manager callback. */
}ll_addr_mgr_start_param_type;
/** @} */ /* end_addtogroup datatypes */

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION LL_ADDR_MGR_START()
===========================================================================*/
/** @ingroup ll_addr_mgr_start
  Starts the LL address manager. This function can only 
  be followed by a ll_addr_mgr_stop() function or one or more calls to the 
  ll_addr_mgr_claim() function.

  @datatypes
  #ps_iface_type\n
  #ll_addr_mgr_params_type\n
  #ll_addr_mgr_ind_cback_type

  @vertspace
  @param[in] iface_ptr     Pointer to the iface value of the client.
  @param[in] params_ptr    Pointer to the related parameters.
  @param[in] ind_cback     Callback for notification.
  @param[in] user_data_ptr Pointer to the user data for the caller.

  @return
  The handle for the link-local address manager to be used upon subsequent
  calls into the LL address manager.

  @dependencies
  This function must be called in mode controller context.
  @newpage
*/
extern void * ll_addr_mgr_start
(
  ps_iface_type                 *iface_ptr,     
  ll_addr_mgr_params_type       *params_ptr,    
  ll_addr_mgr_ind_cback_type     ind_cback,     
  void                          *user_data_ptr  
);


/*===========================================================================
FUNCTION LL_ADDR_MGR_STOP()
===========================================================================*/
/** @ingroup ll_addr_mgr_stop
  Stops the link-local address manager. This function is serialized through 
  the PS task.

  @param[in] ll_addr_mgr_handle  Handle for the link-local address manager.

  @return
  0 -- Stop was taken into account.\n
  -1 -- Serious error occurred; unable to take stop into account.

  @dependencies
  None.
  @newpage
*/
extern int ll_addr_mgr_stop
(
  void *ll_addr_mgr_handle
);


/*===========================================================================
FUNCTION LL_ADDR_MGR_CLAIM()
===========================================================================*/
/** @ingroup ll_addr_mgr_claim
  Claims an IP address on the network. This function triggers the behavior for
  detecting conflicts and obtaining a unique LL address per 
  RFC 3927 @xhyperref{S6,[S6]}.

  @param[in] ll_addr_mgr_handle  Handle for the link-local address manager.

  @return
  0 -- Claim was taken into account.\n
  -1 -- Serious error occurred; unable to take claim into account.

  @dependencies
  This function must be called in mode controller context.
  @newpage
*/
extern int ll_addr_mgr_claim
(
  void *ll_addr_mgr_handle
);


#endif /* PS_LL_ADDR_MGR_H */

