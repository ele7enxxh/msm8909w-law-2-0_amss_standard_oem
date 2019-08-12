#ifndef CMBCMCSI_H
#define CMBCMCSI_H
/*===========================================================================

         C A L L   M A N A G E R   B D   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions internal to BCMCS object

Copyright (c) 2003 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmbcmcsi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/08   rm      Mainlining of MBMS changes
05/17/06   pk      Added support for DDTM during BCMCS flows
03/02/03   ic      Initial revision
===========================================================================*/


/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "mmcp_variation.h"
#include "comdef.h"    /* Definition for basic types and macros */
#include "sys.h"       /* System wide common types */
#include "sys_v.h"
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"

#if ((defined FEATURE_BCMCS) || (defined FEATURE_MBMS))

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Enums
** --------------------------------------------------------------------------
*/

/* Enumeration of BCMCS object states
*/
typedef enum cm_bcmcs_state_e {

  CM_BCMCS_STATE_NONE = -1,
    /* For internal use of CM only
    */

  CM_BCMCS_STATE_IDLE,
    /* BCMCS object is idle
    */

  CM_BCMCS_STATE_REQUESTING,
    /* BCMCS object is ready to send flow request
    ** command to LL
    */

  CM_BCMCS_STATE_WAITING_FOR_ACK,
    /* BCMCS object sent flow request
    ** to LL, waiting for ACK
    */

  CM_BCMCS_STATE_MAX
    /* For bounds checking
    */

  #ifdef FEATURE_RPC
  , CM_BCMCS_STATE_BIG=0x10000000
    /* To force enum to 32 bit for windows NT
    */
  #endif // FEATURE_RPC

} cm_bcmcs_state_e_type;


/* Enumeration of BCMCS object states
*/
typedef enum cm_bcmcs_flow_monitoring_state_e {

  CM_BCMCS_FLOW_MONITORING_STATE_NONE = -1,
    /* For internal use of CM only
    */

  CM_BCMCS_FLOW_MONITORING_STATE_ACTIVE,
    /* Actively monitoring the flow
    */

  CM_BCMCS_FLOW_MONITORING_STATE_MAX
    /* For internal use of CM only
    */

  #ifdef FEATURE_RPC
  , CM_BCMCS_FLOW_MONITORING_STATE_BIG=0x10000000
    /* To force enum to 32 bit for windows NT
    */
  #endif // FEATURE_RPC

} cm_bcmcs_flow_monitoring_state_e_type;


/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/
typedef struct {

  int                           num_flows;

  sys_bcmcs_flow_addr_s_type    *flow_addr;

} cm_mbms_flow_status_s_type;

/**--------------------------------------------------------------------------
** Macros
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Functions - internal
** --------------------------------------------------------------------------
*/


#endif /* FEATURE_BCMCS || MBMS */
#endif /* CMBCMCSI_H */

