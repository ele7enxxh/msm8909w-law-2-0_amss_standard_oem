#ifndef CMBCMCS_H
#define CMBCMCS_H
/*===========================================================================

         C A L L   M A N A G E R   B D   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with   CM BCMCS data object.

  The BCMCS data object is responsible for:
  1. Processing clients' BCMCS data commands.
  2. Processing LL replies for clients' BCMCS data commands.
  3. Processing LL BCMCS data related notifications.
  4. Notifying the client list of BCMCS data events.

Copyright (c) 2004 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmbcmcs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/21/10   ak      Mainlining memory optimization changes.
04/23/10   ak      Memory optimization updates.
02/25/09   sv      Correct Lint errors
07/01/08   rm      Mainlining of MBMS changes
05/17/06   pk      Added support for DDTM during BCMCS flows
04/06/06   sk      Added waiting_pwrsave_exit_for_ss to cmbcmcs_s_type.
                   Added cmbcmcs_sd_rpt_proc().
                   Removed cmbcmcs_timer_proc().
11/08/05   sk      Separated CM reports and commands.
05/13/05   ic      Added cmbcmcs_srv_status_is_srv() [ moved from sys.* ] 
03/02/03   ic      Initial revision
===========================================================================*/


/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "mmcp_variation.h"
#include "comdef.h"    /* Definition for basic types and macros */
#include "sys.h"       /* system wide common types */
#include "sys_v.h"
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmtaski.h"
#include "cmbcmcsi.h"  /* Data internal to BCMCS object */

#if ((defined FEATURE_BCMCS) || (defined FEATURE_MBMS))

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

/* Indicates that transaction id field is not used */
#define     CM_BCMCS_TRANSACTION_ID_NONE ((byte)-1)

/**--------------------------------------------------------------------------
** Enums
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/* Broadcast data type - holds all the information that is associated
** with BCMCS data.
*/
typedef struct cmbcmcs_s  {

  cm_init_mask                     init_mask;
    /* Indicate whether this object was properly initialized
    */

  cm_bcmcs_state_e_type            state;
    /* Indicate BCMCS object state
    */

  cm_bcmcs_cmd_s_type                    bcmcs_cmd;
    /* Copy of the BCMCS CM command requested by client
    */
  #ifdef FEATURE_BCMCS
  cm_mc_rpt_type                   bcmcs_rpt;
    /* Copy of the last BCMCS report received by CM from LL
    */
  #endif

  byte                             transaction_id;
    /* Transaction id for the last LL command sent
    */

  cm_ss_e_type                     waiting_pwrsave_exit_for_ss;
    /* The SS which was most recently requested to get out of power save.
    */
  
  cm_bcmcs_flow_monitoring_state_e_type flow_monitoring_state;
    /* The BCMCS flow monitoring state
    */

  cm_mbms_flow_status_s_type        mbms_flow;
    /* Stores MBMS report that was last received
    */

} cmbcmcs_s_type;


/**--------------------------------------------------------------------------
** Macros
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Functions - internal
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Functions - external
** --------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION cmbcmcs_client_cmd_proc

DESCRIPTION
  Process clients' BCMCS data commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cmbcmcs_client_cmd_proc(

  cm_bcmcs_cmd_s_type  *bcmcs_cmd_ptr
    /* pointer to a CM command
    */
);

/*===========================================================================

FUNCTION cmbcmcs_init

DESCRIPTION

  Initializing the BCMCS object.

  This function must be called before the BCMCS object
  is being used, in any way, place, or form.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cmbcmcs_init( void );

/*===========================================================================

FUNCTION cmbcmcs_ptr

DESCRIPTION

  Return a pointer to the one and only BCMCS object.

  The BCMCS object is responsible for:
  1. Processing clients' BCMCS commands.
  2. Processing LL replies for clients' BCMCS commands.
  3. Processing LL BCMCS related notifications.
  4. Notifying the client list of BCMCS events.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern cmbcmcs_s_type  *cmbcmcs_ptr( void );

/*===========================================================================

FUNCTION cmbcmcs_mc_rpt_proc

DESCRIPTION

  Process LL reports related to BCMCS


DEPENDENCIES
  BCMCS object must have already been initialized with
  cmbcmcs_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cmbcmcs_mc_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
);

/*===========================================================================

FUNCTION cmbcmcs_gw_rpt_proc

DESCRIPTION

  Process LL reports related to MBMS


DEPENDENCIES
  BCMCS object must have already been initialized with
  cmbcmcs_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cmbcmcs_gw_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
);


/*===========================================================================

FUNCTION cmbcmcs_sd_rpt_proc

DESCRIPTION
  Process system determination reports

DEPENDENCIES
  Bcmcs object must have already been initialized with
  cmbcmcs_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmbcmcs_sd_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD report */
);

/*==============================================================================
FUNCTION cmbcmcs_srv_status_is_srv

DESCRIPTION

  Function that verifies that the input value indicates BCMCS service.

PARAMETERS

  srv_status_is_srv  value      Value that is to be checked for validity.

RETURN VALUE

  boolean  Flag indicating whether the value indicates service (TRUE)
           or no service (FALSE).

==============================================================================*/
boolean cmbcmcs_srv_status_is_srv
(
  sys_bcmcs_srv_status_e_type  value
);

/*==============================================================================
FUNCTION cmbcmcs_free

DESCRIPTION

  Frees the memory allocated to the BCMCS object.

PARAMETERS

  None.

RETURN VALUE

  None.

==============================================================================*/
void cmbcmcs_free( void );

/*==============================================================================
FUNCTION cmbcmcs_transaction_id_ptr

DESCRIPTION

  Returns the pointer to latest transaction id value.

PARAMETERS

  None.

RETURN VALUE

  Transaction id.

==============================================================================*/
byte* cmbcmcs_transaction_id_ptr( void );

#endif /* FEATURE_BCMCS || MBMS */
#endif /* CMBCMCS_H */

