/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
               M U L T I M O D E   C O N T R O L L E R ( MMoC )

                              MMGSDI MODULE

GENERAL DESCRIPTION
  This module is the main implementation for the session based PRL read operation
  using MMGSDI APIs.

  The MMoC task has 2 seperate message queues, command queue for messages
  from CM and report queue for messages from the various protocols and MMGSDI.

EXTERNALIZED FUNCTIONS

  Command Interface:


  Report Interface:


  Functional Interface:

    mmocmmgsdi_init
    mmocmmgsdi_open_session
    mmocmmgsdi_session_client_id_reg_cb
    mmocmmgsdi_card_status_cb
    mmocmmgsdi_session_open_cb
    mmocmmgsdi_session_read_prl_cb
    mmocmmgsdi_process_subsc_chgd_mmgsdi_read_enter
    mmocmmgsdi_process_subsc_chgd_mmgsdi_read_cnf


  Others:


REGIONAL FUNCTIONS

  mmocmmgsdi_store_client_id
  mmocmmgsdi_error_e_type mmocmmgsdi_get_session_id_ptr
  mmocmmgsdi_error_e_type mmocmmgsdi_session_get_app_info_ptr


INITIALIZATION AND SEQUENCING REQUIREMENTS
  The MMoC task should be started before MMoC APIs are called.


Copyright (c) 2002 - 2011 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/mmoc/src/mmocmmgsdi.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/30/13   dk      propagate fix for CR 470877, to read PRL only iff subscription is available
04/20/12   sk      Fixing compiler warnings
03/21/12   ab      Added support for handling Auto deact ind
                   whem MMOC is waiting for open session confirm
08/19/11   fc      Depreciate test stubs for QTF.
07/20/11   gm      Support MMSS 3.1 with 1x PRL in NV & 3gpp subscr in RUIM
07/13/11   nk      Updated stubs in UT environment to be in sync with QTF.
06/15/11   rk      Adding featurization to read PRL
03/10/11   aj      Read other MMSS files only if PRL can be read.
01/11/11   am      MUTE migration to qtf changes
12/02/10   gm      Fix to compiler warning
11/11/10   am      Removing the compilation error in MOB
11/10/10   am      Reverting MUTE to MOB migration changes
11/10/10   am      Changes for MUTE to MOB migration
11/05/10   sv/gm   Changes to wait for session open confirmation
10/05/10   pm      Remove clk.h
10/01/10   ak      Merged the Fusion code from Dev branch.
09/16/10   gm      Fix compiler warning for unused variables.
07/20/10   mp      mmgsdi_len_type parameter in mmgsdi_session_read_transparent
                   changed to '0' as requested by MMGSDI team
08/05/10   rk      Added support to update SD with GW session info
07/20/10   mp      mmgsdi_len_type parameter in mmgsdi_session_read_transparent
                   changed to '0' as requested by MMGSDI team
05/24/10   mp      Added support for session close and
                   MMOC_TRANS_MMGSDI_INFO_IND transaction to avoid nested callbacks
05/18/10   mp      Fixed ARM compilation issue
03/21/10   mp      MLPL/MSPL/PLMN Interface for MMSS 3.1 support
01/28/10   mp      Initial creation for EF_EPRL Implementation.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h" /* Customer configuration file */
#ifndef FEATURE_CUSTMMODE_H
#error Need to pull in custmmode.h!!! /* custmmode.h has to be pulled in   */
#endif

#include "comdef.h"   /* Definition for basic types and macros             */
#include "msg.h"      /* Message output services.                          */
#include "err.h"      /* Error and error fatal services.                   */
#include "cmd.h"      /* Command header for inter-task communication.      */

#include "sd.h"       /* Interface for System Determination services.      */
#include "sd_v.h"     /* Internal interface definitions for SD.            */
#include "sd_i.h"     /* Internal interface definitions for SD.            */
#include "prl.h"      /* Interface for PRL services.                       */
#include "prot.h"     /* Common header for mmoc <-> protocol communications*/
#include "mmoc.h"     /* External interface definitions for MMoC.          */
#include "mmoci.h"    /* Internal interface definitions for MMoC.          */
#include "mmocdbg.h"  /* For mmocdbg_print_message support                 */
#include "nv.h"       /* Interface to NV Service.                          */

#ifdef FEATURE_MMGSDI_SESSION_LIB

#include "mmgsdilib.h"          /* Interface to MMGSDI sesion based APIs   */
#include "mmgsdilib_common.h"   /* Interface to MMGSDI sesion based APIs   */
#include "mmgsdisessionlib.h"   /* Interface to MMGSDI sesion based APIs   */

#ifdef FEATURE_UIM
#include "uim.h"
#endif

#ifdef FEATURE_NV_RUIM
#include "nvruimi.h"
#endif

#ifndef FEATURE_MMOC_UNIT_TEST
#include "DDISoftwareSwitches.h" /* Interface for GPIO &other bit i/o services */
#include "DALSys.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"
#endif

#include "task.h"     /* Interface for global task resources.              */
#include "dog.h"      /* Interface to watchdog procedures.                 */
#ifdef MMOC_USES_DB_SERVICES
#include "db.h"       /* Interface to database services.                   */
#endif /* ifdef(MMOC_USES_DB_SERVICES) */

#include "event.h"    /* Interface for system event report service.        */

#ifdef FEATURE_MMOC_UNIT_TEST
#error code not present
#endif
#include  "cmll.h"
//#include "cm_v.h"
//#include "cm_i.h"
//#include "mmoci.h"


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== DEPENDENT FEATURES ===========================
=============================================================================
=============================================================================
===========================================================================*/

/* This set of features must be defined for Multi-mode operation.
*/
#if !defined(FEATURE_SD20)
#error FEATURE_SD20 must be defined.
#endif

#ifndef FEATURE_MMODE_QTF
#if(defined(FEATURE_MMOC_UNIT_TEST) && !defined(FEATURE_UIM_RUN_TIME_ENABLE))
#error code not present
#endif
#endif /* !FEATURE_MMODE_QTF */

/*===========================================================================
=============================================================================
=============================================================================
==================== MMoC Forward declarations ==============================
=============================================================================
=============================================================================
===========================================================================*/

static void mmocmmgsdi_session_closed
(
        mmgsdi_session_id_type           mmocmmgsdi_session_id
);

/*===========================================================================
FUNCTION mmocmmgsdi_get_ss_per_session_type

DESCRIPTION
  Function to get the ss  based on session type

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static sd_ss_e_type mmocmmgsdi_get_ss_per_session_type(mmgsdi_session_type_enum_type session_type)
{
  sd_ss_e_type sd_ss = SD_SS_MAX;

  switch(session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
    case MMGSDI_1X_PROV_PRI_SESSION:
    case MMGSDI_CARD_SESSION_SLOT_1:
         sd_ss = SD_SS_MAIN;
         break;

    case MMGSDI_GW_PROV_SEC_SESSION:
    case MMGSDI_CARD_SESSION_SLOT_2:
         if(mmoc_is_sxlte())
         {
           sd_ss = SD_SS_HYBR_3;
         }
         else
         {
           sd_ss = SD_SS_HYBR_2;
         }
         break;

    case MMGSDI_GW_PROV_TER_SESSION:
         sd_ss = SD_SS_HYBR_3;
         break;

    default:
         break;

  }

  MMOC_MSG_HIGH_2("Sess type=%d, ss=%d",session_type,sd_ss);
  return sd_ss;

}


/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== MMoC IMPLEMENATATION RELATED HELPER FUNCTIONS ==========
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================
FUNCTION MMOCMMGSDI_INIT

DESCRIPTION
  Initilize MMOC MGSDI interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void mmocmmgsdi_init
(
      mmoc_state_info_s_type   *mmoc_state_info_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Register the MMGSDI event callback function */

  mmgsdi_status = mmgsdi_client_id_and_evt_reg(
      mmocmmgsdi_card_status_cb,
      mmocmmgsdi_session_client_id_reg_cb,
      (mmgsdi_client_data_type)mmoc_state_info_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
     MMOC_ERR_1 ("MMGSDI MMOC reg cmd failed : STATUS %d",
                     mmgsdi_status);
  }

} /* mmocmmgsdi_init() */


/*===========================================================================
FUNCTION MMOCMMGSDI_STORE_CLIENT_ID

DESCRIPTION
  Internal helper function that stores the client ID

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  Updates stored client ID

===========================================================================*/
static void mmocmmgsdi_store_client_id
(           const mmgsdi_cnf_type      *cnf_ptr
)
{
  mmoc_mmgsdi_info_s_type *mmgsdi_info_ptr = mmoc_get_mmgsdi_info_ptr ();

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  *(&(mmgsdi_info_ptr->mmgsdi_client_id)) = *(&(cnf_ptr->client_id_and_evt_reg_cnf.response_header.client_id));

  MMOC_MSG_HIGH_1("MMGSDI reg. MMOC with Client id : mmgsdi_client_id %d",
                 mmgsdi_info_ptr->mmgsdi_client_id);

} /* mmocmmgsdi_store_client_id() */

#ifdef FEATURE_MMODE_DUAL_SIM
boolean mmoc_is_dsdx(void)
{
  if( cm_get_device_mode()== SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY || \
              cm_get_device_mode() == SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE)
  {
    return TRUE;
  }
  
  return FALSE;
}
#endif

/*====================================================================

FUNCTION mmocmmgsdi_get_session_index

DESCRIPTION

  This function returns index to the session ID for the specified
  session type.  At most one session ID is associated with each session
  type.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  MMOC_MMGSDI_SUCCESS if successful
  MMOC_MMGSDI_NOTFOUND: Session ID for requested session type was not found

SIDE EFFECTS
  none

=====================================================================*/
static mmocmmgsdi_error_e_type mmocmmgsdi_get_session_index
(
      mmgsdi_session_type_enum_type mmocmmgsdi_session_type,
      unsigned int                  *mmocmmgsdi_session_index
)
{
   mmoc_mmgsdi_info_s_type *mmgsdi_info_ptr = mmoc_get_mmgsdi_info_ptr ();
   unsigned int index;

   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   for(index = 0; index < MMOC_MAX_SESSION_IDS; index++)
   {
      /* Look for a session which matches with the type. No more than one
      ** session for any given session type is supported
      */
      if (mmocmmgsdi_session_type == mmgsdi_info_ptr->
            mmgsdi_session_id_table[index].session_type)
      {
         *mmocmmgsdi_session_index = index;
         return MMOC_MMGSDI_SUCCESS;
      }
   }

   return MMOC_MMGSDI_NOTFOUND;

} /* mmocmmgsdi_get_session_index */

/*====================================================================

FUNCTION mmocmmgsdi_is_session_opened

DESCRIPTION

  This function returns if an session is opened with MMGSDI for specified
  session type.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  TRUE : A valid session is opened for that session type
  FALSE: No valid session type is found

SIDE EFFECTS
  none

=====================================================================*/
static boolean mmocmmgsdi_is_session_opened
(
      mmgsdi_session_type_enum_type mmocmmgsdi_session_type
)
{
   mmoc_mmgsdi_info_s_type *mmgsdi_info_ptr = mmoc_get_mmgsdi_info_ptr ();
   unsigned int index;

   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   for(index = 0; index < MMOC_MAX_SESSION_IDS; index++)
   {
      /* Look for a session which matches with the type. No more than one
      ** session for any given session type is supported
      */
      if (mmocmmgsdi_session_type == mmgsdi_info_ptr->
            mmgsdi_session_id_table[index].session_type)
      {
        //if (mmgsdi_info_ptr->mmgsdi_session_id_table[index].app_info != MMGSDI_APP_NONE)
        if (mmgsdi_info_ptr->mmgsdi_session_id_table[index].session_opened)
        {
          return TRUE;
        }
      }
   }

   return FALSE;

} /* mmocmmgsdi_is_session_opened */

/*====================================================================

FUNCTION mmocmmgsdi_session_closed

DESCRIPTION

  This function sets the session_opened  variable to FALSE to indicate
  that session has been closed.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  None

SIDE EFFECTS
  none

=====================================================================*/
static void mmocmmgsdi_session_closed
(
      mmgsdi_session_id_type        mmocmmgsdi_session_id
)
{
   mmoc_mmgsdi_info_s_type *mmgsdi_info_ptr = mmoc_get_mmgsdi_info_ptr ();
   unsigned int index;

   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   for(index = 0; index < MMOC_MAX_SESSION_IDS; index++)
   {
      /* Look for a session which matches with the session ID.
      **
      */
      if (mmocmmgsdi_session_id == mmgsdi_info_ptr->
            mmgsdi_session_id_table[index].session_id)
      {
        mmgsdi_info_ptr->mmgsdi_session_id_table[index].session_opened = FALSE;
        break;
      }
   }

   return;

} /* mmocmmgsdi_session_closed */

/*====================================================================

FUNCTION mmocmmgsdi_get_session_id_ptr

DESCRIPTION

  This function returns a pointer to the session ID for the specified
  session type.  At most one session ID is associated with each session
  type.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  MMOC_MMGSDI_SUCCESS if successful
  MMOC_MMGSDI_UNSUPPORTED: Function is unsupported on this target
  MMOC_MMGSDI_NOTFOUND: Session ID for requested session type was not found

SIDE EFFECTS
  none

=====================================================================*/
static mmocmmgsdi_error_e_type mmocmmgsdi_get_session_id_ptr
(
      mmgsdi_session_type_enum_type mmocmmgsdi_session_type,
      mmgsdi_session_id_type        ** mmocmmgsdi_session_id_ptr
)
{
  mmoc_mmgsdi_info_s_type *mmgsdi_info_ptr = mmoc_get_mmgsdi_info_ptr ();
  unsigned int index;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (index = 0; index < MMOC_MAX_SESSION_IDS; index++)
  {
    /* Look for a session which matches with the type. No more than one
    ** session for any given session type is supported
    */
    if (mmocmmgsdi_session_type == mmgsdi_info_ptr->
         mmgsdi_session_id_table[index].session_type)
    {
      *mmocmmgsdi_session_id_ptr = &(mmgsdi_info_ptr->
           mmgsdi_session_id_table[index].session_id);
      return MMOC_MMGSDI_SUCCESS;
    }
  }

  return MMOC_MMGSDI_NOTFOUND;

} /* mmocmmgsdi_get_session_id_ptr */

/*====================================================================

FUNCTION mmocmmgsdi_get_session_type

DESCRIPTION

  This function returns a pointer to the session ID for the specified
  session type.  At most one session ID is associated with each session
  type.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  MMOC_MMGSDI_SUCCESS if successful
  MMOC_MMGSDI_UNSUPPORTED: Function is unsupported on this target
  MMOC_MMGSDI_NOTFOUND: Session ID for requested session type was not found

SIDE EFFECTS
  none

=====================================================================*/
static mmocmmgsdi_error_e_type mmocmmgsdi_get_session_type
(
      mmgsdi_session_id_type            mmocmmgsdi_session_id,
      mmgsdi_session_type_enum_type    *mmocmmgsdi_session_type
)
{
  mmoc_mmgsdi_info_s_type *mmgsdi_info_ptr = mmoc_get_mmgsdi_info_ptr ();
  unsigned int index;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for(index = 0; index < MMOC_MAX_SESSION_IDS; index++)
  {
    /* Look for a session which matches with session id.
    */
    if (mmocmmgsdi_session_id == mmgsdi_info_ptr->
         mmgsdi_session_id_table[index].session_id)
    {
      *mmocmmgsdi_session_type = mmgsdi_info_ptr->
          mmgsdi_session_id_table[index].session_type;
      return MMOC_MMGSDI_SUCCESS;
    }
  }

  return MMOC_MMGSDI_NOTFOUND;

} /* mmocmmgsdi_get_session_id_ptr */


/*====================================================================

FUNCTION mmocmmgsdi_get_session_app_info_ptr

DESCRIPTION

  This function returns a pointer to the app info for the specified
  session type.  At most one app_info structure is associated with each
  session type.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.
  app_info is valid only after MMGSDI_SUBSCRIPTION_READY evt is received.

RETURN VALUE
  MMOC_MMGSDI_SUCCESS if successful
  MMOC_MMGSDI_UNSUPPORTED: Function is unsupported on this target
  MMOC_MMGSDI_NOTFOUND: app_info for requested session type was not found

SIDE EFFECTS
  none

=====================================================================*/
static mmocmmgsdi_error_e_type mmocmmgsdi_session_get_app_info_ptr
(
    mmgsdi_session_id_type    mmocmmgsdi_session_id,
    mmgsdi_app_enum_type      ** mmocmmgsdi_app_info_ptr
)
{
  mmoc_mmgsdi_info_s_type *mmgsdi_info_ptr = mmoc_get_mmgsdi_info_ptr ();
  unsigned int index;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for(index = 0; index < MMOC_MAX_SESSION_IDS; index++)
  {
    /* Look for a session which matches with session id.
    */
    if (mmocmmgsdi_session_id == mmgsdi_info_ptr->
         mmgsdi_session_id_table[index].session_id)
    {
      *mmocmmgsdi_app_info_ptr = &(mmgsdi_info_ptr->
          mmgsdi_session_id_table[index].app_info);
      return MMOC_MMGSDI_SUCCESS;
    }
  }

  return MMOC_MMGSDI_NOTFOUND;

} /*mmocmmgsdi_session_get_app_info_ptr */

/*====================================================================

FUNCTION mmocmmgsdi_session_get_session_type_ptr

DESCRIPTION

  This function returns a pointer to the session type which 
  matches passed in session ID

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  MMOC_MMGSDI_SUCCESS if successful
  MMOC_MMGSDI_UNSUPPORTED: Function is unsupported on this target
  MMOC_MMGSDI_NOTFOUND: app_info for requested session type was not found

SIDE EFFECTS
  none

=====================================================================*/
static mmocmmgsdi_error_e_type mmocmmgsdi_session_get_session_type
(
    mmgsdi_session_id_type             mmocmmgsdi_session_id,
    mmgsdi_session_type_enum_type      **mmocmmgsdi_session_type_ptr
)
{
  mmoc_mmgsdi_info_s_type *mmgsdi_info_ptr = mmoc_get_mmgsdi_info_ptr ();
  unsigned int index;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for(index = 0; index < MMOC_MAX_SESSION_IDS; index++)
  {
    /* Look for a session which matches with session id.
    */
    if (mmocmmgsdi_session_id == mmgsdi_info_ptr->
         mmgsdi_session_id_table[index].session_id)
    {
      *mmocmmgsdi_session_type_ptr = &(mmgsdi_info_ptr->mmgsdi_session_id_table[index].session_type);
      return MMOC_MMGSDI_SUCCESS;
    }
  }

  return MMOC_MMGSDI_NOTFOUND;

} /*mmocmmgsdi_session_get_app_info_ptr */
/*===========================================================================
FUNCTION mmocmmgsdi_get_bitmask_to_read_items

DESCRIPTION
  This is a utility function called by the SUBSCRIPTION CHANGE transaction
  handler to figure out the items needed to be read from the MMGSDI card for
  this subscription change.

DEPENDENCIES
  None.

RETURN VALUE
  bit mask                  - Indicating the items to be read from card.
  MMOC_MMGSDI_MASK_NONE     - Nothing to be read from card.

SIDE EFFECTS
  None.
===========================================================================*/
mmocmmgsdi_read_mask_e_type            mmocmmgsdi_get_bitmask_to_read_items
(
       mmoc_state_info_s_type          *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{
  mmoc_mmgsdi_info_s_type      *mmgsdi_info_ptr = mmoc_get_mmgsdi_info_ptr ();

  #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
  mmocmmgsdi_error_e_type      mmocmmgsdi_status = MMOC_MMGSDI_READ_FAILED;
 #endif

  #ifdef FEATURE_MMSS_3_1
  mmgsdi_app_enum_type         *mmocmmgsdi_app_info_ptr = NULL;
  mmgsdi_session_id_type       *mmocmmgsdi_session_id_ptr = NULL;
  #endif /* FEATURE_MMSS_3_1 */
#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
  mmgsdi_session_id_type       *mmocmmgsdi_1x_session_id_ptr = NULL;

  mmgsdi_app_enum_type         *mmocmmgsdi_1x_app_info_ptr = NULL;
#endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MMOC_ASSERT( mmoc_info_ptr != NULL );
  MMOC_ASSERT( mmgsdi_info_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MMOC_MSG_HIGH_3("subsc_chg = 0x%x hybr_gw_subs_chg = 0x%x hybr_gw3_subs_chg = 0x%x",
                  mmoc_info_ptr->prot_subsc_chg,
                  mmoc_info_ptr->hybr_gw_subs_chg,
                  mmoc_info_ptr->hybr_3_gw_subs_chg);
  MMOC_MSG_HIGH_5("cdma sub = %d, gwl sub = %d, hybr_gwl sub = %d gw3_sub= %d nv_rtre_control() = %d",
                  mmoc_info_ptr->is_cdma_subs_avail,
                  mmoc_info_ptr->is_gwl_subs_avail,
                  mmoc_info_ptr->is_gw_hybr_subs_avail,
                  mmoc_info_ptr->is_gw_hybr_3_subs_avail, 
                  nv_rtre_control());
  

  if (( ( CHECK_PROTO(mmoc_info_ptr->prot_subsc_chg ,PROT_SUBSC_CHG_GW)   ||
          CHECK_PROTO(mmoc_info_ptr->prot_subsc_chg ,PROT_SUBSC_CHG_CDMA)
        ) &&
        ( mmoc_info_ptr->is_cdma_subs_avail ||
          mmoc_info_ptr->is_gwl_subs_avail
        )
      )
     )
  {
    /* Set the Bit mask to read PRL, if
    **    i.   Subs changed is CDMA or G/W and
    **    ii.  Subsc is available for CDMA or G/W and
    **    iii. MMGSDI session is opened to Read PRL
    **    iv.  RTRE configuration is set to RUIM
    */
    if((mmocmmgsdi_is_session_opened (MMGSDI_1X_PROV_PRI_SESSION)) &&
       (nv_rtre_control() == NV_RTRE_CONTROL_USE_RUIM) &&
       mmoc_info_ptr->is_cdma_subs_avail)
    {
    mmgsdi_info_ptr->read_items |= MMOC_MMGSDI_MASK_PRL;
    }
    else
    {
      MMOC_MSG_HIGH_1("MMSS:No PRL in RUIM, read_items=0x%x",
                   mmgsdi_info_ptr->read_items);
    }
  }
  else
  {
    mmgsdi_info_ptr->read_items = MMOC_MMGSDI_MASK_NONE;
    mmgsdi_info_ptr->mmgsdi_items = MMOC_MMGSDI_MASK_NONE;
    return (mmgsdi_info_ptr->read_items);
  }

  #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
  if(((sd_operator_e_type)sd_ss_get_operator_name(SYS_MODEM_AS_ID_1)) == SD_OPERATOR_CT)
  {
  mmocmmgsdi_status = mmocmmgsdi_get_session_id_ptr(
      (mmgsdi_session_type_enum_type) MMGSDI_1X_PROV_PRI_SESSION,
      &mmocmmgsdi_1x_session_id_ptr);

  if( (MMOC_MMGSDI_SUCCESS != mmocmmgsdi_status) || (mmocmmgsdi_1x_session_id_ptr == NULL))
  {
    MMOC_ERR_0("No 1x session type");
    return (mmgsdi_info_ptr->read_items);
  }

  mmocmmgsdi_status = mmocmmgsdi_session_get_app_info_ptr(
                       *mmocmmgsdi_1x_session_id_ptr,
                       &mmocmmgsdi_1x_app_info_ptr);

  if( (MMOC_MMGSDI_SUCCESS != mmocmmgsdi_status) || (mmocmmgsdi_1x_app_info_ptr == NULL))
  {
    MMOC_ERR_1("Failed to get app info: read_items = %x",
                        mmgsdi_info_ptr->read_items);
    return (mmgsdi_info_ptr->read_items);
  }

   if(mmocmmgsdi_1x_app_info_ptr == NULL)
   {
    MMOC_ERR_0("Failed to get app info: mmocmmgsdi_1x_app_info_ptr is null");
    return (mmgsdi_info_ptr->read_items);
  }
  if(*mmocmmgsdi_1x_app_info_ptr == MMGSDI_APP_CSIM)
   {
	 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	 /* Check which record PLMN record to read from MMGSDI card for USIM
	 **/
	 mmgsdi_info_ptr->read_items =
					  mmgsdi_info_ptr->read_items			   |
					  MMOC_MMGSDI_MASK_CSIM_IMSI;

   }
   else if(*mmocmmgsdi_1x_app_info_ptr == MMGSDI_APP_RUIM)
   {
	 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	 /* Check which record PLMN record to read from MMGSDI card for USIM
	 **/
	 mmgsdi_info_ptr->read_items =
					  mmgsdi_info_ptr->read_items			   |
					  MMOC_MMGSDI_MASK_RUIM_IMSI;

   }
   else
   {
	  MMOC_ERR_0("CDMA IMSI: Failed to update read items: ");
   }
   }
  #endif
  #ifdef FEATURE_MMSS_3_1

  /* Get GW session and application information
  */

  mmocmmgsdi_status = mmocmmgsdi_get_session_id_ptr(
      (mmgsdi_session_type_enum_type) MMGSDI_GW_PROV_PRI_SESSION,
      &mmocmmgsdi_session_id_ptr);

  if( MMOC_MMGSDI_SUCCESS != mmocmmgsdi_status)
  {
    MMOC_ERR_0("No gw session type");
    return (mmgsdi_info_ptr->read_items);
  }

  mmocmmgsdi_status = mmocmmgsdi_session_get_app_info_ptr(
                       *mmocmmgsdi_session_id_ptr,
                       &mmocmmgsdi_app_info_ptr);

  if( MMOC_MMGSDI_SUCCESS != mmocmmgsdi_status)
  {
    MMOC_ERR_1("Failed to get app info: read_items = %x",
                        mmgsdi_info_ptr->read_items);
    return (mmgsdi_info_ptr->read_items);
  }

    /* Set the Bit mask to read MLPL/MSPL, if
    **    i.  Subsc is available for CDMA and G/W (even if no Subsc changed) &
    **    ii. MMGSDI session is opened to Read MLPL/MSPL
    **    iii. MMGSDI session is opened to Read plmn db
    **/
  if( !( (mmocmmgsdi_is_session_opened(MMGSDI_CARD_SESSION_SLOT_1) || 
           mmocmmgsdi_is_session_opened(MMGSDI_CARD_SESSION_SLOT_2)) &&
         mmocmmgsdi_is_session_opened(MMGSDI_GW_PROV_PRI_SESSION) &&
         mmoc_info_ptr->is_gwl_subs_avail &&
         mmoc_info_ptr->is_cdma_subs_avail)
    )
  {
    MMOC_MSG_HIGH_1("EqPRl: legacy behavior, read_items = %x",
                 mmgsdi_info_ptr->read_items);
    mmgsdi_info_ptr->mmgsdi_items = mmgsdi_info_ptr->read_items;
    return (mmgsdi_info_ptr->read_items);
  }
  mmgsdi_info_ptr->read_items =
                       mmgsdi_info_ptr->read_items  |
                       MMOC_MMGSDI_MASK_MLPL        |
                       MMOC_MMGSDI_MASK_MSPL;

  if (*mmocmmgsdi_app_info_ptr == MMGSDI_APP_SIM)
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Check which record PLMN record to read from MMGSDI card for SIM
    **/
    mmgsdi_info_ptr->read_items =
                     mmgsdi_info_ptr->read_items              |
                     MMOC_MMGSDI_MASK_GSM_IMSI                |
                     MMOC_MMGSDI_MASK_GSM_AD                  |
                     MMOC_MMGSDI_MASK_GSM_PLMNWACT            |
                     MMOC_MMGSDI_MASK_GSM_OPLMNWACT           |
                     MMOC_MMGSDI_MASK_GSM_PLMN;
  }
  else if (*mmocmmgsdi_app_info_ptr == MMGSDI_APP_USIM)
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Check which record PLMN record to read from MMGSDI card for USIM
    **/
    mmgsdi_info_ptr->read_items =
                     mmgsdi_info_ptr->read_items              |
                     MMOC_MMGSDI_MASK_USIM_EHPLMN             |
                     MMOC_MMGSDI_MASK_USIM_PLMNWACT           |
                     MMOC_MMGSDI_MASK_USIM_OPLMNWACT          |
                     MMOC_MMGSDI_MASK_USIM_IMSI               |
                     MMOC_MMGSDI_MASK_USIM_AD;
  }

  else
  {
	  MMOC_ERR_0("MMSS:Failed to update read items: ");
  }
  #endif /* #ifdef FEATURE_MMSS_3_1 */

  mmgsdi_info_ptr->mmgsdi_items = mmgsdi_info_ptr->read_items;

  MMOC_MSG_HIGH_2("read_items = %x, mmgsdi_items = %x",
                 mmgsdi_info_ptr->read_items,
                 mmgsdi_info_ptr->mmgsdi_items);

  return (mmgsdi_info_ptr->read_items);

} /* mmocmmgsdi_get_bitmask_to_read_items() */

/*====================================================================

FUNCTION mmocmmgsdi_open_session

DESCRIPTION
  Internal function which calls mmgsdi_session_open for the specified
  session type, and generates an error if unsuccessful.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.
  Call after client registration with MMGSDI is complete, and MMOC
  has a valid client ID.

RETURN VALUE
  none

SIDE EFFECTS
  Request to open a session enqueued with MMGSDI.
  ERROR if unsuccessful.

=====================================================================*/

static void mmocmmgsdi_open_session
(

         mmgsdi_session_type_enum_type mmocmmgsdi_session_type
            /* Type of session to be opened
            */
)
{
   mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_SUCCESS;
   mmoc_mmgsdi_info_s_type *mmgsdi_info_ptr = mmoc_get_mmgsdi_info_ptr ();

   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   /* Request to open a session for the session type using
   * the assigned client id.
   */
   mmgsdi_status = mmgsdi_session_open(mmgsdi_info_ptr->mmgsdi_client_id,
                                       mmocmmgsdi_session_type,
                                       0,
                                       mmocmmgsdi_card_status_cb,
                                       FALSE,
                                       mmocmmgsdi_session_open_cb,
                                       (mmgsdi_client_data_type) mmocmmgsdi_session_type);

   if(mmgsdi_status != MMGSDI_SUCCESS)
   {
     MMOC_ERR_3( "Session open Failed: Status=%d, Type=%d, Client_id=%d",
       mmgsdi_status, mmocmmgsdi_session_type,mmgsdi_info_ptr->mmgsdi_client_id);
     return;
   }
   MMOC_MSG_LOW_1("Opening session type :%d",mmocmmgsdi_session_type);

   return;

} /* mmocmmgsdi_open_session */


/*===========================================================================

FUNCTION mmocmmgsdi_session_read_prl


DESCRIPTION
  This is a function is used to read PRL through MMGSDI session.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_MMGSDI_SUCCESS         - PRL read request accepted by MMGSDI.
  MMOC_MMGSDI_PRL_READ_FAILED - prl read failed.


SIDE EFFECTS
  None
===========================================================================*/
mmocmmgsdi_error_e_type      mmocmmgsdi_session_read_prl
(
       mmoc_state_info_s_type          *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */
)
{
  mmgsdi_session_id_type      *mmocmmgsdi_session_id_ptr = NULL;
  mmocmmgsdi_error_e_type     mmocmmgsdi_status = MMOC_MMGSDI_PRL_READ_FAILED;
  mmgsdi_return_enum_type     mmgsdi_status = MMGSDI_MAX_RETURN_ENUM;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MMOC_ASSERT(mmoc_info_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the session ID from session ID table
  */
  mmocmmgsdi_status = mmocmmgsdi_get_session_id_ptr(
      (mmgsdi_session_type_enum_type) MMGSDI_1X_PROV_PRI_SESSION,
      &mmocmmgsdi_session_id_ptr);

  if (MMOC_MMGSDI_SUCCESS != mmocmmgsdi_status)
  {
    MMOC_ERR_1("get_session Failed!!! mmocmmgsdi_status=%d",
                    mmocmmgsdi_status);
    return (mmocmmgsdi_status);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Read PRL from the card through MMGSDI session api.
  */
  mmgsdi_status = mmgsdi_session_read_prl (
        *mmocmmgsdi_session_id_ptr,
        mmocmmgsdi_session_read_prl_cb,
      (mmgsdi_client_data_type) mmoc_info_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    mmoc_info_ptr->mmoc_mmgsdi_wait_info.file_enum = MMGSDI_CSIM_PRL;
    return (MMOC_MMGSDI_SUCCESS);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Read PRL using MMGSDI session api failed.
  */
  MMOC_ERR_1("mmgsdi_session_read_prl Failed!!! mmgsdi_status=%d",
                 mmgsdi_status);

  return (MMOC_MMGSDI_PRL_READ_FAILED);

} /* mmocmmgsdi_session_read_prl */

/*===========================================================================

FUNCTION mmocmmgsdi_session_read_transparent


DESCRIPTION
  This is a function is used to read transparent records through MMGSDI session.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_MMGSDI_SUCCESS          - Read request accepted by MMGSDI.
  MMOC_MMGSDI_READ_FAILED      - Transparent read failed.


SIDE EFFECTS
  None
===========================================================================*/
mmocmmgsdi_error_e_type      mmocmmgsdi_session_read_transparent
(
       mmoc_state_info_s_type          *mmoc_info_ptr,
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */
       mmgsdi_session_type_enum_type   session_type,
            /* Type of session used to retrieve data from the card data
            */
       mmgsdi_access_type              file_access_info,
            /* Access information to read data from the card
            */
       mmgsdi_callback_type             session_read_cb
)
{
  mmgsdi_session_id_type          *mmocmmgsdi_session_id_ptr = NULL;
  mmocmmgsdi_error_e_type         mmocmmgsdi_status = MMOC_MMGSDI_READ_FAILED;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_MAX_RETURN_ENUM;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MMOC_ASSERT(mmoc_info_ptr != NULL);
  MMOC_ASSERT(session_read_cb != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the session ID from session ID table
  */
  mmocmmgsdi_status = mmocmmgsdi_get_session_id_ptr(
      (mmgsdi_session_type_enum_type) session_type,
      &mmocmmgsdi_session_id_ptr);

  if (MMOC_MMGSDI_SUCCESS != mmocmmgsdi_status)
  {
    MMOC_ERR_1("get_session Failed!!! mmocmmgsdi_status=%d",
                    mmocmmgsdi_status);
    return (mmocmmgsdi_status);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read data using MMGSDI session api
  */
  mmoc_info_ptr->mmoc_mmgsdi_wait_info.file_enum = file_access_info.file.file_enum;
  mmgsdi_status = mmgsdi_session_read_transparent
                  (
                    *mmocmmgsdi_session_id_ptr,
                    file_access_info,
                    0, /* start from the beginning of file */
                    0, /* retrive the entire file */
                    session_read_cb,
                    (mmgsdi_client_data_type) mmoc_info_ptr
                  );

  if (MMGSDI_SUCCESS == mmgsdi_status)
  {
    return (MMOC_MMGSDI_SUCCESS);
  }
  else
  {
    /* Read data using MMGSDI session api failed.
    */
    MMOC_ERR_2("mmgsdi_session_read_transparent Failed!!! \
                    file_enum = %d, mmgsdi_status=%d",
                    file_access_info.file.file_enum, mmgsdi_status);
    mmoc_info_ptr->mmoc_mmgsdi_wait_info.file_enum = MMGSDI_NO_FILE_ENUM;
    return (MMOC_MMGSDI_READ_FAILED);
  }

} /* mmocmmgsdi_session_read_transparent */

/*===========================================================================

FUNCTION mmocmmgsdi_session_read_mlpl_mspl


DESCRIPTION
  This is a function is used to read MLPL and MSPL records through MMGSDI
  session.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_MMGSDI_SUCCESS          - MLPL/MSPL read request accepted by MMGSDI.
  MMOC_MMGSDI_READ_FAILED      - MLPL/MSPL read failed.


SIDE EFFECTS
  None
===========================================================================*/
mmocmmgsdi_error_e_type      mmocmmgsdi_session_read_mlpl_mspl
(
       mmoc_state_info_s_type          *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */
)
{
  mmoc_mmgsdi_info_s_type *mmgsdi_info_ptr = mmoc_get_mmgsdi_info_ptr ();
  mmocmmgsdi_error_e_type  mmocmmgsdi_status = MMOC_MMGSDI_READ_FAILED;
  mmgsdi_access_type       file_access_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*
  ** Initialize access enum based on card mode.
  */
  file_access_info.access_method = MMGSDI_EF_ENUM_ACCESS;

  #ifdef FEATURE_MMSS_3_1

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Check which record (MLPL/MSPL)record to read from MMGSDI card
  **/

  if ( MMOC_IS_BIT_CONTAIN(mmgsdi_info_ptr->read_items,
                           MMOC_MMGSDI_MASK_MLPL) )
  {
    file_access_info.file.file_enum = MMGSDI_TELECOM_MLPL;
    mmgsdi_info_ptr->read_items = MMOC_BIT_DELETE(mmgsdi_info_ptr->read_items,
                                                  MMOC_MMGSDI_MASK_MLPL);
  }
  else if ( MMOC_IS_BIT_CONTAIN(mmgsdi_info_ptr->read_items,
                                MMOC_MMGSDI_MASK_MSPL)
          )
  {
    file_access_info.file.file_enum = MMGSDI_TELECOM_MSPL;
    mmgsdi_info_ptr->read_items = MMOC_BIT_DELETE(mmgsdi_info_ptr->read_items,
                                                  MMOC_MMGSDI_MASK_MSPL);
  }
  else
  {
    return (MMOC_MMGSDI_READ_NONE);
  }

  #endif /* #ifdef FEATURE_MMSS_3_1 */

  #ifdef FEATURE_MMODE_SC_SVLTE
  MMOC_MSG_HIGH_1("gw pri slot %d", mmoc_info_ptr->pri_slot);
  #endif
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Read the requested record from MMGSDI card
  **/
  mmocmmgsdi_status = mmocmmgsdi_session_read_transparent
                      (
                        mmoc_info_ptr,
                        #ifdef FEATURE_MMODE_SC_SVLTE
                        mmoc_info_ptr->pri_slot,
                        #else
                        MMGSDI_CARD_SESSION_SLOT_1,
                        #endif
                        file_access_info,
                        mmocmmgsdi_session_read_cb
                      );

  if (MMOC_MMGSDI_SUCCESS == mmocmmgsdi_status)
  {
    return (mmocmmgsdi_status);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read records failed. clear the read items bitmask so that no more
  ** items are read.
  */
  mmgsdi_info_ptr->read_items = MMOC_MMGSDI_MASK_NONE;

  MMOC_ERR_1("Read mlpl_mspl Failed, read_items = %x",
                  mmgsdi_info_ptr->read_items);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return (mmocmmgsdi_status);

} /* mmocmmgsdi_session_read_mlpl */

/*===========================================================================

FUNCTION mmocmmgsdi_session_read_card_imsi


DESCRIPTION
  This is a function is used to read card imsi through MMGSDI session.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_MMGSDI_SUCCESS          - card read accepted by MMGSDI.
  MMOC_MMGSDI_READ_FAILED      - card read failed.


SIDE EFFECTS
  None
===========================================================================*/
mmocmmgsdi_error_e_type      mmocmmgsdi_session_read_card_imsi
(
       mmoc_state_info_s_type          *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */
)
{
  mmoc_mmgsdi_info_s_type     *mmgsdi_info_ptr = mmoc_get_mmgsdi_info_ptr ();
  mmocmmgsdi_error_e_type      mmocmmgsdi_status = MMOC_MMGSDI_READ_FAILED;
  mmgsdi_access_type           file_access_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize access enum based on card mode.
  */
  file_access_info.access_method = MMGSDI_EF_ENUM_ACCESS;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Check which IMSI value to read from MMGSDI card for SIM
  **/


 if ( MMOC_IS_BIT_CONTAIN(mmgsdi_info_ptr->read_items,
                                MMOC_MMGSDI_MASK_CSIM_IMSI)
          )
  {
    file_access_info.file.file_enum = MMGSDI_CSIM_IMSI_M;
    mmgsdi_info_ptr->read_items = MMOC_BIT_DELETE(mmgsdi_info_ptr->read_items,
                                                  MMOC_MMGSDI_MASK_CSIM_IMSI);
  }
  else if ( MMOC_IS_BIT_CONTAIN(mmgsdi_info_ptr->read_items,
                                MMOC_MMGSDI_MASK_RUIM_IMSI)
          )
  {
    file_access_info.file.file_enum = MMGSDI_CDMA_IMSI_M;
    mmgsdi_info_ptr->read_items = MMOC_BIT_DELETE(mmgsdi_info_ptr->read_items,
                                                  MMOC_MMGSDI_MASK_RUIM_IMSI);
  }
  else
  {
    return (MMOC_MMGSDI_READ_NONE);
  }
   MMOC_MSG_HIGH_1("mmocmmgsdi_session_read_card_imsi file enum = %d", file_access_info.file.file_enum);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Read the requested record from MMGSDI card
  **/
  mmocmmgsdi_status = mmocmmgsdi_session_read_transparent
                      (
                        mmoc_info_ptr,
                        MMGSDI_1X_PROV_PRI_SESSION,
                        file_access_info,
                        (mmgsdi_callback_type) mmocmmgsdi_session_read_card_imsi_cb
                      );

  if (MMOC_MMGSDI_SUCCESS == mmocmmgsdi_status)
  {
    return (mmocmmgsdi_status);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read card failed.
  */
  mmgsdi_info_ptr->read_items = MMOC_MMGSDI_MASK_NONE;

  MMOC_ERR_1("read_plmn Failed, read_items = %x",
                  mmgsdi_info_ptr->read_items);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return (mmocmmgsdi_status);

} /* mmocmmgsdi_session_read_card_imsi */

/*===========================================================================

FUNCTION mmocmmgsdi_session_read_plmn


DESCRIPTION
  This is a function is used to read PLMN records through MMGSDI session.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_MMGSDI_SUCCESS          - PLMN read request accepted by MMGSDI.
  MMOC_MMGSDI_READ_FAILED      - PLMN read failed.


SIDE EFFECTS
  None
===========================================================================*/
mmocmmgsdi_error_e_type      mmocmmgsdi_session_read_plmn
(
       mmoc_state_info_s_type          *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */
)
{
  mmoc_mmgsdi_info_s_type     *mmgsdi_info_ptr = mmoc_get_mmgsdi_info_ptr ();
  mmocmmgsdi_error_e_type      mmocmmgsdi_status = MMOC_MMGSDI_READ_FAILED;
  mmgsdi_access_type           file_access_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize access enum based on card mode.
  */
  file_access_info.access_method = MMGSDI_EF_ENUM_ACCESS;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Check which record PLMN record to read from MMGSDI card for SIM
  **/

  if ( MMOC_IS_BIT_CONTAIN(mmgsdi_info_ptr->read_items,
                           MMOC_MMGSDI_MASK_GSM_IMSI))
  {
    file_access_info.file.file_enum = MMGSDI_GSM_IMSI;
    mmgsdi_info_ptr->read_items = MMOC_BIT_DELETE(mmgsdi_info_ptr->read_items,
                                                  MMOC_MMGSDI_MASK_GSM_IMSI);
  }
  else if ( MMOC_IS_BIT_CONTAIN(mmgsdi_info_ptr->read_items,
                                MMOC_MMGSDI_MASK_GSM_AD)
          )
  {
    file_access_info.file.file_enum = MMGSDI_GSM_AD;
    mmgsdi_info_ptr->read_items = MMOC_BIT_DELETE(mmgsdi_info_ptr->read_items,
                                                  MMOC_MMGSDI_MASK_GSM_AD);
  }
  else if ( MMOC_IS_BIT_CONTAIN(mmgsdi_info_ptr->read_items,
                                MMOC_MMGSDI_MASK_GSM_PLMNWACT)
          )
  {
    file_access_info.file.file_enum = MMGSDI_GSM_PLMNWACT;
   mmgsdi_info_ptr->read_items = MMOC_BIT_DELETE(mmgsdi_info_ptr->read_items,
                                                 MMOC_MMGSDI_MASK_GSM_PLMNWACT);
  }
  else if ( MMOC_IS_BIT_CONTAIN(mmgsdi_info_ptr->read_items,
                                MMOC_MMGSDI_MASK_GSM_OPLMNWACT)
          )
  {
    file_access_info.file.file_enum = MMGSDI_GSM_OPLMNWACT;
    mmgsdi_info_ptr->read_items = MMOC_BIT_DELETE(mmgsdi_info_ptr->read_items,
                                                  MMOC_MMGSDI_MASK_GSM_OPLMNWACT );
  }
  else if ( MMOC_IS_BIT_CONTAIN(mmgsdi_info_ptr->read_items,
                                MMOC_MMGSDI_MASK_GSM_PLMN)
          )
  {
    file_access_info.file.file_enum = MMGSDI_GSM_PLMN;
    mmgsdi_info_ptr->read_items =MMOC_BIT_DELETE(mmgsdi_info_ptr->read_items,
                                                 MMOC_MMGSDI_MASK_GSM_PLMN);
  }
  else if ( MMOC_IS_BIT_CONTAIN(mmgsdi_info_ptr->read_items,
                                MMOC_MMGSDI_MASK_USIM_EHPLMN)
          )
  {
    file_access_info.file.file_enum = MMGSDI_USIM_EHPLMN;

    mmgsdi_info_ptr->read_items = MMOC_BIT_DELETE(mmgsdi_info_ptr->read_items,
                                                  MMOC_MMGSDI_MASK_USIM_EHPLMN);
  }
  else if ( MMOC_IS_BIT_CONTAIN(mmgsdi_info_ptr->read_items,
                                 MMOC_MMGSDI_MASK_USIM_PLMNWACT)
            )
  {
    file_access_info.file.file_enum = MMGSDI_USIM_PLMNWACT;
    mmgsdi_info_ptr->read_items = MMOC_BIT_DELETE(mmgsdi_info_ptr->read_items,
                                                  MMOC_MMGSDI_MASK_USIM_PLMNWACT);
  }
  else if ( MMOC_IS_BIT_CONTAIN(mmgsdi_info_ptr->read_items,
                                MMOC_MMGSDI_MASK_USIM_OPLMNWACT)
          )
  {
    file_access_info.file.file_enum = MMGSDI_USIM_OPLMNWACT;
    mmgsdi_info_ptr->read_items =MMOC_BIT_DELETE(mmgsdi_info_ptr->read_items,
                                                 MMOC_MMGSDI_MASK_USIM_OPLMNWACT );
  }
  else if (MMOC_IS_BIT_CONTAIN (mmgsdi_info_ptr->read_items,
                                MMOC_MMGSDI_MASK_USIM_IMSI)
          )
  {
    file_access_info.file.file_enum = MMGSDI_USIM_IMSI;
    mmgsdi_info_ptr->read_items =MMOC_BIT_DELETE(mmgsdi_info_ptr->read_items,
                                                 MMOC_MMGSDI_MASK_USIM_IMSI);
  }
  else if ( MMOC_IS_BIT_CONTAIN(mmgsdi_info_ptr->read_items,
                                MMOC_MMGSDI_MASK_USIM_AD)
          )
  {
    file_access_info.file.file_enum = MMGSDI_USIM_AD;
    mmgsdi_info_ptr->read_items = MMOC_BIT_DELETE(mmgsdi_info_ptr->read_items,
                                                  MMOC_MMGSDI_MASK_USIM_AD);
  }
  else
  {
    return (MMOC_MMGSDI_READ_NONE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Read the requested record from MMGSDI card
  **/
  mmocmmgsdi_status = mmocmmgsdi_session_read_transparent
                      (
                        mmoc_info_ptr,
                        MMGSDI_GW_PROV_PRI_SESSION,
                        file_access_info,
                        (mmgsdi_callback_type) mmocmmgsdi_session_read_cb
                      );

  if (MMOC_MMGSDI_SUCCESS == mmocmmgsdi_status)
  {
    return (mmocmmgsdi_status);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read records failed.
  */
  mmgsdi_info_ptr->read_items = MMOC_MMGSDI_MASK_NONE;

  MMOC_ERR_1("read_plmn Failed, read_items = %x",
                  mmgsdi_info_ptr->read_items);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return (mmocmmgsdi_status);

} /* mmocmmgsdi_session_read_plmn */



/*====================================================================

FUNCTION mmocmmgsdi_session_read_card_imsi_cb

DESCRIPTION
  Callback function for session based MMGSDI interface.  Called by MMGSDI
  in response to call to mmgsdi_session_read_card_imsi() function.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  none

SIDE EFFECTS
  none

=====================================================================*/
void   mmocmmgsdi_session_read_card_imsi_cb
(
       mmgsdi_return_enum_type       status,
       mmgsdi_cnf_enum_type          cnf,
       const mmgsdi_cnf_type        *cnf_ptr
)
{

	mmoc_rpt_msg_s_type 		   *msg_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MMOC_ASSERT(cnf_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (MMGSDI_SUCCESS != status || MMGSDI_READ_CNF !=  cnf)
  {
    MMOC_ERR_2("MMGSDI read card imsi failed. Status %d, Confirmation %d",
                   status, cnf);
  }


#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
	else
 	{

	/*call the sd function here to update cdma_imsi value
    	*/
    sd_misc_update_card_imsi( cnf_ptr->read_cnf.read_data.data_ptr,
                            (uint32 ) cnf_ptr->read_cnf.read_data.data_len);
	  } /* else */
#endif


 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 /* Queue a report to clear the transaction when waiting for session open cnf.
 */

 MMOC_MSG_HIGH_0("MMGSDI sending MMOC_RPT_MMGSDI_CNF ");
 /* Get the report buffer.
 **/
 msg_ptr = mmoc_get_rpt_buf_else_err_fatal();

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 /* Fill in all the fields.
 */
 msg_ptr->rpt.name					 = MMOC_RPT_MMGSDI_CNF;
 msg_ptr->param.mmgsdi_cnf.status	 = status;

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 /* Post the report.
 */
 mmoc_send_rpt( msg_ptr );



} /* mmocmmgsdi_session_read_card_imsi_cb */

/*===========================================================================
FUNCTION MMOCMMGSDI_SESSION_CLIENT_REG_CB

DESCRIPTION
  Callback function for session based MMGSDI interface.  Called by MMGSDI
  to acknowledge MMOC client registration and event registration.  Client ID
  provided in this callback.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  None

SIDE EFFECTS
  Error Fatal if registration unsuccessful
  Client ID is stored

===========================================================================*/
void mmocmmgsdi_session_client_id_reg_cb
(
     mmgsdi_return_enum_type      status,
     mmgsdi_cnf_enum_type         cnf,
     const mmgsdi_cnf_type        *cnf_ptr
)
{
  mmoc_mmgsdi_info_s_type    *mmgsdi_info_ptr = mmoc_get_mmgsdi_info_ptr ();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MMOC_ASSERT(cnf_ptr != NULL);
  MMOC_ASSERT((mmoc_state_info_s_type *) (cnf_ptr->client_id_and_evt_reg_cnf.
              response_header.client_data) != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((status == MMGSDI_SUCCESS)                 &&
      (cnf == MMGSDI_CLIENT_ID_AND_EVT_REG_CNF)  &&
      (cnf_ptr != NULL))
  {
    /* Store the client id assigned for mmoc */
    mmocmmgsdi_store_client_id(cnf_ptr);
  }
  else
  {
    MMOC_ERR_3("mmocmmgsdi_session_client_id_reg_cb Failed!!! \
        mmgsdi_info_ptr->mmgsdi_client_id = %d,status = %d,cnf = %d",
        mmgsdi_info_ptr->mmgsdi_client_id, status, cnf);
  }

  return;
} /* mmocmmgsdi_session_client_id_reg_cb */

/*===========================================================================
FUNCTION MMOCMMGSDI_CARD_STATUS_CB

DESCRIPTION
  Callback function for MMGSDI generated event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmocmmgsdi_card_status_cb
(
  const mmgsdi_event_data_type *event
)
{
  mmgsdi_app_enum_type         *mmocmmgsdi_app_info_ptr = NULL;
  mmgsdi_session_type_enum_type  mmocmmgsdi_sess_type = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_session_type_enum_type  *mmocmmgsdi_session_type_ptr = NULL;
  sd_ss_e_type                   sd_ss = SD_SS_MAX;
  mmoc_mmgsdi_info_s_type       *mmgsdi_info_ptr = mmoc_get_mmgsdi_info_ptr ();

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MMOC_ASSERT(event != NULL);

  MMOC_MSG_HIGH_2("mmocmmgsdi_card_status_cb:Received MMGSDI Event: evt=%d,sess_id=%d", event->evt,event->session_id);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(event->evt != MMGSDI_CARD_INSERTED_EVT
    && event->evt != MMGSDI_SIM_BUSY_EVT)
  {
    if(MMOC_MMGSDI_SUCCESS != mmocmmgsdi_get_session_type(
                                    event->session_id,&mmocmmgsdi_sess_type))
    {
      MMOC_MSG_HIGH_2("Invalid session type and id combination:Sess type=%d, Sess Id=%d",\
         mmocmmgsdi_sess_type,event->session_id);
      return;
    }
  
    sd_ss = mmocmmgsdi_get_ss_per_session_type(mmocmmgsdi_sess_type);
  
    if((mmocmmgsdi_sess_type == MMGSDI_MAX_SESSION_TYPE_ENUM) || (sd_ss == SD_SS_MAX))
    {
      MMOC_MSG_HIGH_3("Invalid session, sess_type=%d, ss=%d ,session id=%d",mmocmmgsdi_sess_type,sd_ss,\
                         event->session_id);
      return;
    }
  
  MMOC_MSG_HIGH_3("Found session, sess_type=%d, ss=%d ,session id=%d",mmocmmgsdi_sess_type,sd_ss,\
                       event->session_id);
  }
  
  switch (event->evt)
  {
    case MMGSDI_SUBSCRIPTION_READY_EVT:

      /* Subscription is available.Update the app type for that session
      **/
      MMOC_MSG_HIGH_3("MMGSDI_SUBSCRIPTION_READY_EVT, session_id=%d, \
            slot=%d, app_type=%d", event->session_id,
            event->data.subscription_ready.app_info.slot,
            event->data.subscription_ready.app_info.app_data.app_type);

      if (MMOC_MMGSDI_SUCCESS == mmocmmgsdi_session_get_app_info_ptr(
                                event->session_id,
                                &mmocmmgsdi_app_info_ptr))
      {
        *mmocmmgsdi_app_info_ptr = *(&(event->data.subscription_ready.app_info.app_data.app_type));

        /* Update GW session info to SD so that it is used to
        ** check if LTE needs to be disabled.
        */
        MMOC_MSG_HIGH_1("Updating SD with app_type=%d", *mmocmmgsdi_app_info_ptr);
        if( *mmocmmgsdi_app_info_ptr == MMGSDI_APP_SIM ||
            *mmocmmgsdi_app_info_ptr == MMGSDI_APP_USIM)
        {
          sd_ss_misc_update_session_info (*mmocmmgsdi_app_info_ptr, sd_ss );
          if( (sd_ss == SD_SS_MAIN)&& mmoc_is_1x_sxlte())
          {
            /*In SVLTE, HYBR2 shares the same subscription with MAIN*/
            sd_ss_misc_update_session_info (*mmocmmgsdi_app_info_ptr, SD_SS_HYBR_2 );
          }
        }
      }
      else
      {
        MMOC_ERR_1("Session ID not found: %d", event->session_id);
      }

      #ifdef FEATURE_MMODE_SC_SVLTE
      if( MMOC_MMGSDI_SUCCESS == mmocmmgsdi_session_get_session_type(
                event->session_id,
                &mmocmmgsdi_session_type_ptr))
      {
        MMOC_MSG_HIGH_2("mmocmmgsdi_session_type %d, slot %d",*mmocmmgsdi_session_type_ptr,\
          event->data.subscription_ready.app_info.slot);
        if (*mmocmmgsdi_session_type_ptr == MMGSDI_GW_PROV_PRI_SESSION ||
            *mmocmmgsdi_session_type_ptr == MMGSDI_1X_PROV_PRI_SESSION )
        {
          switch(event->data.subscription_ready.app_info.slot)
          {
            case MMGSDI_SLOT_1:
              mmoc_get_state_info_ptr()->pri_slot = MMGSDI_CARD_SESSION_SLOT_1;
              break;
            case MMGSDI_SLOT_2:
              mmoc_get_state_info_ptr()->pri_slot = MMGSDI_CARD_SESSION_SLOT_2;
              break;
            default:
              break;
          }
        }
        
      }
      else
      {
        MMOC_ERR_1("Session Index not found for session type: %d",
                        mmocmmgsdi_session_type_ptr);
      }
      #endif
      break;

     case MMGSDI_SESSION_CHANGED_EVT:

      /* One of the sessions got changed. Update the app type for that session
      **/
      MMOC_MSG_HIGH_3("MMGSDI_SESSION_CHANGED_EVT, session_id=%d, \
            slot=%d, app_type=%d", event->session_id,
            event->data.session_changed.app_info.slot,
            event->data.session_changed.app_info.app_data.app_type);

      if( MMOC_MMGSDI_SUCCESS == mmocmmgsdi_session_get_app_info_ptr(
                                event->session_id,
                                &mmocmmgsdi_app_info_ptr))
      {
        *mmocmmgsdi_app_info_ptr = *(&(event->data.session_changed.app_info.app_data.app_type));

        /* Update GW session info to SD so that it is used to
        ** check if LTE needs to be disabled.
        */
        
        MMOC_MSG_HIGH_1("Updating SD with app_type=%d", *mmocmmgsdi_app_info_ptr);
        if( *mmocmmgsdi_app_info_ptr == MMGSDI_APP_SIM ||
            *mmocmmgsdi_app_info_ptr == MMGSDI_APP_USIM)
        {
          sd_ss_misc_update_session_info (*mmocmmgsdi_app_info_ptr, sd_ss );
          if( (sd_ss == SD_SS_MAIN)&& mmoc_is_1x_sxlte())
          {
            /*In SVLTE, HYBR2 shares the same subscription with MAIN*/
            sd_ss_misc_update_session_info (*mmocmmgsdi_app_info_ptr, SD_SS_HYBR_2 );
          }
        }
      }
      else
      {
        MMOC_ERR_1("Session ID not found: %d", event->session_id);
      }

      break;

    case MMGSDI_CARD_INSERTED_EVT:

      /* Queue Command to MMOC to open/re-open the session */
      MMOC_MSG_HIGH_3("MMGSDI_CARD_INSERTED_EVT, session_id=%d, \
            client_id=%d, slot=%d", event->session_id,
            event->client_id, event->data.card_inserted.slot );

      mmoc_cmd_mmgsdi_info_ind ();

      break;

    case MMGSDI_SESSION_CLOSE_EVT:

      /* One of the sessions got Closed. Update the app type for that session
      **/
      MMOC_MSG_HIGH_2("MMGSDI_SESSION_CLOSE_EVT, session_id=%d, \
            slot=%d ", event->data.session_close.session_id,
            event->data.session_close.slot);

      if( MMOC_MMGSDI_SUCCESS == mmocmmgsdi_session_get_app_info_ptr(
                                event->data.session_close.session_id,
                                &mmocmmgsdi_app_info_ptr))
      {

        /* Update GW session is closed.Reset SD session GW  info
        ** So that SD can fall back to limited service searching
        ** on LTE rat.
        */
        
        MMOC_MSG_HIGH_1("Updating SD with app_type=%d", *mmocmmgsdi_app_info_ptr);
        if ( *mmocmmgsdi_app_info_ptr == MMGSDI_APP_SIM ||
             *mmocmmgsdi_app_info_ptr == MMGSDI_APP_USIM)
        {
          sd_ss_misc_update_session_info (MMGSDI_APP_NONE, sd_ss );
          if( (sd_ss == SD_SS_MAIN)&& mmoc_is_1x_sxlte())
          {
            /*In SVLTE, HYBR2 shares the same subscription with MAIN*/
            sd_ss_misc_update_session_info (*mmocmmgsdi_app_info_ptr, SD_SS_HYBR_2 );
          }
        }

        *mmocmmgsdi_app_info_ptr = MMGSDI_APP_NONE;
      }
      else
      {
        MMOC_ERR_1("Session ID not found: %d", event->session_id);
      }

      /* Update the session_opened variable to FALSE
      **/
      mmocmmgsdi_session_closed (event->session_id);

      break;

    case MMGSDI_SIM_BUSY_EVT:
      {
        mmgsdi_session_type_enum_type  session_slot = MMGSDI_CARD_SESSION_SLOT_1 ; 

        MMOC_MSG_HIGH_3("SIMBUSY: slot:%d, sim_busy:%d, PRI_SLOT:%d", \
          event->data.sim_busy.slot,event->data.sim_busy.sim_busy,mmoc_get_state_info_ptr()->pri_slot);
        
        switch(event->data.sim_busy.slot)
        {
              case MMGSDI_SLOT_1:
                session_slot = MMGSDI_CARD_SESSION_SLOT_1;
                break;
              case MMGSDI_SLOT_2:
                session_slot = MMGSDI_CARD_SESSION_SLOT_2;
                break;
              default:
                break;
         }
        
        if(session_slot == mmoc_get_state_info_ptr()->pri_slot)
        {
          mmgsdi_info_ptr->sim_busy_status 
                    = event->data.sim_busy.sim_busy;
          
          /* Set the REX signal corresponding to MMoC's report queue and return.
                  */
          (void) rex_set_sigs(get_mmoc_tcb(), MMOC_MMGSDI_ABORT_TIMER_SIG);
        }
      }
      break;
    default:
      break;
  } /* switch */

  return;
}  /* mmocmmgsdi_card_status_cb() */


/*====================================================================

FUNCTION mmocmmgsdi_session_open_cb

DESCRIPTION
  Callback function for session based MMGSDI interface.  Called by MMGSDI
  in response to call to mmgsdi_open_session() function.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  none

SIDE EFFECTS
  none

=====================================================================*/
void   mmocmmgsdi_session_open_cb
(
       mmgsdi_return_enum_type      status,
       mmgsdi_cnf_enum_type         cnf,
       const mmgsdi_cnf_type        *cnf_ptr
)
{
  mmgsdi_session_id_type        *mmocmmgsdi_session_id_ptr = NULL;
  mmgsdi_app_enum_type          *mmocmmgsdi_app_info_ptr = NULL;
  mmocmmgsdi_error_e_type       mmocmmgsdi_status = MMOC_MMGSDI_NOTFOUND;
  mmgsdi_session_type_enum_type mmocmmgsdi_session_type;
  mmoc_mmgsdi_info_s_type       *mmgsdi_info_ptr = mmoc_get_mmgsdi_info_ptr ();
  unsigned int                  index = 0;
  mmoc_rpt_msg_s_type           *msg_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MMOC_ASSERT(cnf_ptr != NULL);
  //MMOC_ASSERT(((void *) cnf_ptr->session_open_cnf.response_header.client_data) != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mmocmmgsdi_session_type = (mmgsdi_session_type_enum_type) (cnf_ptr->
                             session_open_cnf.response_header.client_data);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (MMGSDI_SUCCESS != status || MMGSDI_SESSION_OPEN_CNF !=  cnf)
  {
    MMOC_MSG_MED_3("Session open Failed, Status = %d, Cnf = %d, Type = %d",
                   status, cnf, mmocmmgsdi_session_type);
    return;
  }

  MMOC_MSG_HIGH_3("MMGSDI Session opened, App Type = %d, Session Type = %d \
      Session Id = %d ", cnf_ptr->session_open_cnf.app_info.app_type,
    mmocmmgsdi_session_type, cnf_ptr->session_open_cnf.session_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the session id ptr from the session ID table */
  mmocmmgsdi_status = mmocmmgsdi_get_session_id_ptr(
      (mmgsdi_session_type_enum_type) mmocmmgsdi_session_type,
      &mmocmmgsdi_session_id_ptr);

  if( MMOC_MMGSDI_SUCCESS != mmocmmgsdi_status)
  {
    MMOC_MSG_MED_3("MMGSDI fail session open unknown session type %d, session id %d, app type %d",
           cnf_ptr->session_open_cnf.response_header.client_data,
           cnf_ptr->session_open_cnf.response_header.session_id,
           cnf_ptr->session_open_cnf.app_info.app_type);
    return;
  }

  /* Copy the opened session id to the pointer entry
   *  in the session id table.
   */
  *mmocmmgsdi_session_id_ptr = *(&(cnf_ptr->session_open_cnf.response_header.session_id));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy the app type to the session id table.
   */
  if( MMOC_MMGSDI_SUCCESS == mmocmmgsdi_session_get_app_info_ptr(
                              cnf_ptr->session_open_cnf.response_header.session_id,
                              &mmocmmgsdi_app_info_ptr))
  {
    *mmocmmgsdi_app_info_ptr = *(&(cnf_ptr->session_open_cnf.app_info.app_type));
  }
  else
  {
    MMOC_ERR_1("Session ID not found to save App type: %d",
      cnf_ptr->session_open_cnf.response_header.session_id);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Set the session_opened variable.
   */
  if( MMOC_MMGSDI_SUCCESS == mmocmmgsdi_get_session_index(
            (mmgsdi_session_type_enum_type) mmocmmgsdi_session_type,
            &index))
  {
    mmgsdi_info_ptr->mmgsdi_session_id_table[index].session_opened = TRUE;
  }
  else
  {
    MMOC_ERR_1("Session Index not found for session type: %d",
                    mmocmmgsdi_session_type);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Queue a report to clear the transaction when waiting for session open cnf.
  */

  /* Get the report buffer.
  **/
  msg_ptr = mmoc_get_rpt_buf_else_err_fatal();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in all the fields.
  */
  msg_ptr->rpt.name                   = MMOC_RPT_MMGSDI_CNF;
  msg_ptr->param.mmgsdi_cnf.status    = status;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the report.
  */
  mmoc_send_rpt( msg_ptr );

  return;

} /* mmocmmgsdi_session_open_cb */


/*====================================================================

FUNCTION mmocmmgsdi_session_read_prl_cb

DESCRIPTION
  Callback function for session based MMGSDI interface.  Called by MMGSDI
  in response to call to mmgsdi_session_read_prl() function.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  none

SIDE EFFECTS
  none

=====================================================================*/
void   mmocmmgsdi_session_read_prl_cb
(
       mmgsdi_return_enum_type       status,
       mmgsdi_cnf_enum_type          cnf,
       const mmgsdi_cnf_type        *cnf_ptr
)
{
  mmoc_rpt_msg_s_type           *msg_ptr;
  mmoc_mmgsdi_info_s_type       *mmgsdi_info_ptr = mmoc_get_mmgsdi_info_ptr ();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MMOC_ASSERT(cnf_ptr != NULL);

  mmoc_get_state_info_ptr()->mmoc_mmgsdi_wait_info.file_enum = MMGSDI_NO_FILE_ENUM;

  MMOC_MSG_HIGH_5("PRL Read: Status %d, Confirmation %d, valid %d, size %d, prl_version %d",
        status, cnf, cnf_ptr->session_read_prl_cnf.valid, cnf_ptr->session_read_prl_cnf.size,
        cnf_ptr->session_read_prl_cnf.prl_version);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (MMGSDI_SIM_BUSY == status)
  {
    MMOC_MSG_MED_0("PRL read failed, sim busy");
    mmoc_send_cm_rpt( CM_MMOC_SUBSCRIPTION_FAILURE, NULL);
    mmgsdi_info_ptr->read_items   = MMOC_MMGSDI_MASK_NONE;
    mmgsdi_info_ptr->mmgsdi_items = MMOC_MMGSDI_MASK_NONE;
  }
  else  if (MMGSDI_SUCCESS == status && MMGSDI_SESSION_READ_PRL_CNF ==  cnf)
  {
    if ((cnf_ptr->session_read_prl_cnf.valid) &&
        (cnf_ptr->session_read_prl_cnf.size) &&
        (cnf_ptr->session_read_prl_cnf.roaming_list_ptr != NULL))
    {

      /* Call in to SD lib to update the prl read through MMGSDI to SD buffer
      **/

      prl_update_from_mmgsdi (cnf_ptr->session_read_prl_cnf.size,
                              cnf_ptr->session_read_prl_cnf.prl_version,
                              cnf_ptr->session_read_prl_cnf.roaming_list_ptr
                             );
    }

  } /* else */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the report buffer.
  **/
  msg_ptr = mmoc_get_rpt_buf_else_err_fatal();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->rpt.name                   = MMOC_RPT_MMGSDI_CNF;
  msg_ptr->param.mmgsdi_cnf.status    = status;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the report.
  */
  mmoc_send_rpt( msg_ptr );


} /* mmocmmgsdi_session_read_prl_cb */

/*====================================================================

FUNCTION mmocmmgsdi_session_read_cb

DESCRIPTION
  Callback function for session based MMGSDI interface.  Called by MMGSDI
  in response to call to mmgsdi_session_read_transparent() function.

  Check if MMGSDI read failed due to SIM-BUSY ,
  if yes - notify CM CM_MMOC_SUBSCRIPTION_FAILURE ,
  so that it can recover after SIM is ready.
DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  none

SIDE EFFECTS
  none

=====================================================================*/
void   mmocmmgsdi_session_read_cb
(
       mmgsdi_return_enum_type       status,
       mmgsdi_cnf_enum_type          cnf,
       const mmgsdi_cnf_type*        cnf_ptr
)
{
  mmoc_rpt_msg_s_type            *msg_ptr;
  mmoc_state_info_s_type* mmoc_state = mmoc_get_state_info_ptr();
  mmoc_mmgsdi_info_s_type  *mmgsdi_info_ptr = mmoc_get_mmgsdi_info_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MMOC_ASSERT(cnf_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MMOC_MSG_HIGH_5("Data from MMGSDI. Status=%d, Cnf=%d, data_len=%d, data_ptr=%x, file_enum: %d",
				status, cnf, cnf_ptr->read_cnf.read_data.data_len, 
				cnf_ptr->read_cnf.read_data.data_ptr,
				cnf_ptr->read_cnf.access.file.file_enum);

  if(mmoc_state->mmoc_mmgsdi_wait_info.file_enum !=
                                cnf_ptr->read_cnf.access.file.file_enum)
  {
    MMOC_MSG_MED_1("MMOC waiting for %d",mmoc_state->mmoc_mmgsdi_wait_info.file_enum);
    return;
  }
  mmoc_state->mmoc_mmgsdi_wait_info.file_enum = MMGSDI_NO_FILE_ENUM;

  if ( (MMGSDI_SUCCESS  != status)                    ||
       (MMGSDI_READ_CNF !=  cnf)                      ||
       (cnf_ptr->read_cnf.read_data.data_ptr == NULL) ||
       (cnf_ptr->read_cnf.read_data.data_len == 0))
  {

    if(status == MMGSDI_SIM_BUSY)
    {
      mmoc_send_cm_rpt( CM_MMOC_SUBSCRIPTION_FAILURE, NULL);

      /* Posting subsc failure to CM, clear read_items as sim busy*/
      mmgsdi_info_ptr->read_items   = MMOC_MMGSDI_MASK_NONE;
      mmgsdi_info_ptr->mmgsdi_items = MMOC_MMGSDI_MASK_NONE;
  }
  }
  else
  {

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_MMSS_3_1

    /* Call in to SD lib to update the data read through MMGSDI to SD buffer
    **/
    sd_misc_update_filedb ( cnf_ptr->read_cnf.access.file.file_enum,
                            cnf_ptr->read_cnf.read_data.data_ptr,
                            (uint32 ) cnf_ptr->read_cnf.read_data.data_len );

    #endif /* #ifdef FEATURE_MMSS_3_1 */

  } /* else */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the report buffer.
  **/
  msg_ptr = mmoc_get_rpt_buf_else_err_fatal();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  **/
  msg_ptr->rpt.name                   = MMOC_RPT_MMGSDI_CNF;
  msg_ptr->param.mmgsdi_cnf.status    = status;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the report.
  */
  mmoc_send_rpt( msg_ptr );


} /* mmocmmgsdi_session_read_cb */

/*===========================================================================

FUNCTION mmocmmgsdi_process_subsc_chgd_mmgsdi_read_enter


DESCRIPTION
  This is a function for handling the transaction state MMGSDI_READ_ENTER
  for the subscription changed transaction . If a MMGSDI session is used
  to read items such as PRL,PLMN,MSPL/MLPL, move to MMGSDI_READ_CNF
  else move to PH_STAT_ENTER.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Should wait for response for mmgsdi read.
  MMOC_EVT_STATUS_NOT_CONSUMED - Start ph_stat_enter processing.


SIDE EFFECTS
  None
===========================================================================*/
mmoc_evt_status_e_type      mmocmmgsdi_process_subsc_chgd_mmgsdi_read_enter
(
       const mmoc_rpt_msg_s_type       *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

       mmoc_state_info_s_type          *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */
)
{
  mmoc_mmgsdi_info_s_type  *mmgsdi_info_ptr = mmoc_get_mmgsdi_info_ptr();
  mmocmmgsdi_error_e_type   mmocmmgsdi_status = MMOC_MMGSDI_SUCCESS;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Check and read PRL from MMGSDI card
  **/

  if(mmgsdi_info_ptr->sim_busy_status == TRUE)
  {
    MMOC_MSG_HIGH_0("Marking read_items None as sim_busy");

    /* Posting subsc failure to CM, clear read_items as sim busy*/
    mmoc_send_cm_rpt( CM_MMOC_SUBSCRIPTION_FAILURE, NULL);
    mmgsdi_info_ptr->read_items = MMOC_MMGSDI_MASK_NONE ;
    mmgsdi_info_ptr->mmgsdi_items = MMOC_MMGSDI_MASK_NONE;
   
    mmoc_update_trans_state( MMOC_TRANS_STATE_PROT_PH_STAT_ENTER,
                             mmoc_info_ptr
                           );
    
    return MMOC_EVT_STATUS_NOT_CONSUMED;
  }
  
  if ( MMOC_IS_BIT_CONTAIN (mmgsdi_info_ptr->read_items,
                            MMOC_MMGSDI_MASK_PRL) )
  {
    mmgsdi_info_ptr->read_items = MMOC_BIT_DELETE(mmgsdi_info_ptr->read_items,
                                                  MMOC_MMGSDI_MASK_PRL);

    mmocmmgsdi_status = mmocmmgsdi_session_read_prl (mmoc_info_ptr);

    if (mmocmmgsdi_status == MMOC_MMGSDI_SUCCESS)
    {
        mmoc_update_trans_state( MMOC_TRANS_STATE_MMGSDI_READ_CNF,
                                 mmoc_info_ptr
                               );

        return MMOC_EVT_STATUS_CONSUMED;
    }
  }

  #ifdef FEATURE_MMSS_3_1

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Check and read MLPL/MSPL records from MMGSDI card
  **/

  mmocmmgsdi_status = mmocmmgsdi_session_read_mlpl_mspl (mmoc_info_ptr);

  if (mmocmmgsdi_status == MMOC_MMGSDI_SUCCESS)
  {
    mmoc_update_trans_state( MMOC_TRANS_STATE_MMGSDI_READ_CNF,
                             mmoc_info_ptr
                           );

    return MMOC_EVT_STATUS_CONSUMED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Check and read PLMN records from MMGSDI card
  **/

  mmocmmgsdi_status = mmocmmgsdi_session_read_plmn (mmoc_info_ptr);

  if (mmocmmgsdi_status == MMOC_MMGSDI_SUCCESS)
  {
    mmoc_update_trans_state( MMOC_TRANS_STATE_MMGSDI_READ_CNF,
                             mmoc_info_ptr
                           );

    return MMOC_EVT_STATUS_CONSUMED;
  }


  #endif /* #ifdef FEATURE_MMSS_3_1 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 	/* Check and read imsi from MMGSDI card
	**/

#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
	if(((sd_operator_e_type)sd_ss_get_operator_name(SYS_MODEM_AS_ID_1)) == SD_OPERATOR_CT)
	{


	mmocmmgsdi_status = mmocmmgsdi_session_read_card_imsi (mmoc_info_ptr);

	if (mmocmmgsdi_status == MMOC_MMGSDI_SUCCESS)
	{
	  mmoc_update_trans_state( MMOC_TRANS_STATE_MMGSDI_READ_CNF,
							   mmoc_info_ptr
							 );

	  return MMOC_EVT_STATUS_CONSUMED;
	}
	}
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Either all items are read successfully or tried to read them
  ** through MMGSDI session. Now move to PH_STATE_ENTER and
  ** continue default processing.
  */

  mmgsdi_info_ptr->read_items = MMOC_MMGSDI_MASK_NONE ;

  mmoc_update_trans_state( MMOC_TRANS_STATE_PROT_PH_STAT_ENTER,
                           mmoc_info_ptr
                         );

  return MMOC_EVT_STATUS_NOT_CONSUMED;

} /* mmocmmgsdi_process_subsc_chgd_mmgsdi_read_enter*/

/*===========================================================================

FUNCTION mmocmmgsdi_process_subsc_chgd_mmgsdi_read_cnf


DESCRIPTION
  This is a function for handling the transaction state MMGSDI_READ_CNF for the
  subscription changed transaction . If the report is MMOC_RPT_MMGSDI_CNF,
  move to PH_STAT_ENTER else stay in the same state.


DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Processing finished.
  MMOC_EVT_STATUS_NOT_CONSUMED - Start ph_stat_enter processing.


SIDE EFFECTS
  None
===========================================================================*/
mmoc_evt_status_e_type         mmocmmgsdi_process_subsc_chgd_mmgsdi_read_cnf
(
        const mmoc_rpt_msg_s_type      *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */
)
{

  mmoc_mmgsdi_info_s_type *mmgsdi_info_ptr = mmoc_get_mmgsdi_info_ptr ();

  /* Check if the rpt ptr is NULL
  */
  if ( rpt_ptr == NULL )
  {
    MMOC_ERR_0("rpt NULL");
    return MMOC_EVT_STATUS_CONSUMED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Process the received report.
  */
  switch (rpt_ptr->rpt.name )
  {
   case MMOC_RPT_PROT_AUTO_DEACTD_IND:
      return mmoc_buf_auto_deact_ind_rpt(rpt_ptr,mmoc_info_ptr,rpt_ptr->param.prot_deactd_ind.ss);

    case MMOC_RPT_MMGSDI_CNF:
      if ( mmgsdi_info_ptr->read_items)
      {
        /* More items need to be read from MMGSDI card
        ** Move to MMOC_TRANS_STATE_MMGSDI_READ_ENTER to read those items.
        */
        mmoc_update_trans_state( MMOC_TRANS_STATE_MMGSDI_READ_ENTER,
                                 mmoc_info_ptr
                               );
      }
      else
      {
        /* All items are read from the card.
        ** Move to MMOC_TRANS_STATE_PROT_PH_STAT_ENTER to continue.
        */
        mmoc_update_trans_state( MMOC_TRANS_STATE_PROT_PH_STAT_ENTER,
                                 mmoc_info_ptr
                               );
      }
      return MMOC_EVT_STATUS_NOT_CONSUMED;

    default:
      MMOC_MSG_MED_1( "Unexpected report received %d",
                        rpt_ptr->rpt.name);
      return MMOC_EVT_STATUS_CONSUMED;
  } /* switch ( rpt_ptr->rpt.name ) */

} /* mmocmmgsdi_process_subsc_chgd_mmgsdi_read_cnf */

/* <EJECT> */

/*===========================================================================
=============================================================================
=============================================================================
==================== MMGSDI_INFO_IND TRANSACTION ============================
=============================================================================
=============================================================================
===========================================================================*/
/*===========================================================================

FUNCTION mmocmmgsdi_process_mmgsdi_info_null

DESCRIPTION
  This function handles the MMOC_CMD_MMGSDI_INFO_IND
  command in the NULL transaction state.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmocmmgsdi_process_mmgsdi_info_null
(

        const mmoc_cmd_msg_s_type      *cmd_ptr,
            /* Pointer to the command event that was received.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  boolean         wait_session_open = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MMOC_ASSERT( cmd_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The MMOC_CMD_MMGSDI_INFO_IND should only create this transaction.
  */
  if ( cmd_ptr == NULL ||
       cmd_ptr->cmd.name != MMOC_CMD_MMGSDI_INFO_IND
     )
  {
    MMOC_ERR_0("Autonomous MMGSDI_INFO_IND transaction created incorrectly!");

    return mmoc_clear_transaction( mmoc_info_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get 1X Provisioning session information. If there is no session opened,
  ** open the session now.
  */
  if( !( mmocmmgsdi_is_session_opened(MMGSDI_1X_PROV_PRI_SESSION) ) )
  {
    /* Use the client ID to open a session to read PRL*/
    mmocmmgsdi_open_session(MMGSDI_1X_PROV_PRI_SESSION);
    wait_session_open = TRUE;
  }

  /* Get CARD_SESSION_SLOT_1 session information. If there is no session opened,
  ** open the session now.
  */
  if( !( mmocmmgsdi_is_session_opened(MMGSDI_CARD_SESSION_SLOT_1) ) )
  {
    /* Use the client ID to open a session to read MLPL/MSPL*/
    mmocmmgsdi_open_session(MMGSDI_CARD_SESSION_SLOT_1);
    wait_session_open = TRUE;
  }

  /* Get CARD_SESSION_SLOT_2 session information. If there is no session opened,
  ** open the session now.
  */
  if( !( mmocmmgsdi_is_session_opened(MMGSDI_CARD_SESSION_SLOT_2) ) )
  {
    /* Use the client ID to open a session to read MLPL/MSPL*/
    mmocmmgsdi_open_session(MMGSDI_CARD_SESSION_SLOT_2);
    wait_session_open = TRUE;
  }

  /* Get GW Provisioning session information. If there is no session opened,
  ** open the session now.
  */
  if( !( mmocmmgsdi_is_session_opened(MMGSDI_GW_PROV_PRI_SESSION) ) )
  {
    /* Use the client ID to open a session to read PLMN*/
    mmocmmgsdi_open_session(MMGSDI_GW_PROV_PRI_SESSION);
    wait_session_open = TRUE;
  }

  #if defined(FEATURE_MMODE_DUAL_SIM)
      /* Get hybr GW provisioning session information. If there is no session opened,
         ** open the session now.
           */
    if( !( mmocmmgsdi_is_session_opened(MMGSDI_GW_PROV_SEC_SESSION) ) )
    {
      /*open a sec  session */
      mmocmmgsdi_open_session(MMGSDI_GW_PROV_SEC_SESSION);
      wait_session_open = TRUE;
    }
  
  
  #if defined FEATURE_MMODE_TRIPLE_SIM 
      /* Get hybr GW3 provisioning session information. If there is no session opened,
    ** open the session now.
    */
    if( !( mmocmmgsdi_is_session_opened(MMGSDI_GW_PROV_TER_SESSION) ) )
    {
      /*open a ter session */
      mmocmmgsdi_open_session(MMGSDI_GW_PROV_TER_SESSION);
      wait_session_open = TRUE;
    }
 
  #endif

  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Session open request sent, wait for confirmation.
  */
  if( wait_session_open)
  {
    mmoc_update_trans_state( MMOC_TRANS_STATE_WAIT_SESSION_OPEN_CNF,
                             mmoc_info_ptr
                           );
  }
  else
  {
  /* Transaction cleared.
  **/
  return mmoc_clear_transaction(mmoc_info_ptr);
  }

  return MMOC_EVT_STATUS_CONSUMED;

} /* mmocmmgsdi_process_mmgsdi_info_null() */


/*===========================================================================

FUNCTION mmocmmgsdi_process_session_open_cnf

DESCRIPTION
  This function handles the MMOC_CMD_MMGSDI_INFO_IND
  command in the NULL transaction state.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmocmmgsdi_process_session_open_cnf
(

        const mmoc_rpt_msg_s_type      *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{

  mmoc_mmgsdi_info_s_type       *mmgsdi_info_ptr = mmoc_get_mmgsdi_info_ptr ();
  unsigned int                  index_1x_pri;
  unsigned int                  index_gw_pri;
  #if defined(FEATURE_MMODE_DUAL_SIM)
  unsigned int                  index_gw_sec;
  boolean                       all_gw2_sessions_opened = TRUE;
  #endif
  #if defined(FEATURE_MMODE_TRIPLE_SIM)
  unsigned int                  index_gw_ter;
  boolean                       all_gw3_sessions_opened = TRUE;
  #endif
  unsigned int                  index_card_slot_1;
  unsigned int                  index_card_slot_2;
  boolean                       all_sessions_opened = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MMOC_ASSERT( rpt_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The MMOC_CMD_MMGSDI_INFO_IND should only create this transaction.
  */

  if (rpt_ptr == NULL)
  {
    MMOC_ERR_0("unexpected report ");
    return MMOC_EVT_STATUS_CONSUMED;
  }

  if (rpt_ptr->rpt.name == MMOC_RPT_PROT_AUTO_DEACTD_IND)
  {
    MMOC_MSG_LOW_0("Received autonomous deactivated report");


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Process LTE to DO redirection abort
      */
      if( rpt_ptr->param.prot_deactd_ind.prot_state == PROT_STATE_ONLINE_HDR
                                      &&
          rpt_ptr->param.prot_deactd_ind.reason     == PROT_DEACT_REDIR_ABORT
                                      &&
          mmoc_info_ptr->prot_state[SD_SS_HYBR_HDR] != PROT_STATE_ONLINE_HDR)
      {

         MMOC_MSG_HIGH_0("DO Redir aborted");

         mmoc_info_ptr->is_redir_allowed = FALSE;

         return MMOC_EVT_STATUS_CONSUMED;

      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #ifdef MMOC_HYBR_SUPPORTED
      /* Check if the deactivation was from SS_HDR system selection instance.
      */
      if ( rpt_ptr->param.prot_deactd_ind.prot_state  == PROT_STATE_ONLINE_HDR &&
           mmoc_info_ptr->prot_state[SD_SS_HYBR_HDR] == PROT_STATE_ONLINE_HDR)
      {

        /* If hybrid preference is not ON then hybrid hdr instance should
        ** not be triggered to go ONLINE
        */
        if (sd_misc_is_hybr_opr())
        {

          /* It is for SS_HDR instance, activate SS_HDR and return.
          */
          (void)mmoc_activate_prot_online_ss_hdr( sd_ss_hybr_1_act_get(NULL),
                                                  mmoc_info_ptr
                                                );
        }
        else
        {

          MMOC_ERR_0 ("AUTO_DEACTD_IND from HYBR_HDR");

          mmoc_update_prot_state( PROT_STATE_NULL,
                                  SD_SS_HYBR_HDR,
                                  mmoc_info_ptr
                                );

        }

        return MMOC_EVT_STATUS_CONSUMED;
      }
      #endif /* MMOC_HYBR_SUPPORTED */


      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      /* Check if auto deactivation from HYBR_3 stack */
      if(rpt_ptr->param.prot_deactd_ind.ss == SD_SS_HYBR_3 &&
         mmoc_info_ptr->prot_state[SD_SS_HYBR_3] == PROT_STATE_ONLINE_GWL)
      {
        /* If HYBR_3 stack should be online, then activate SS_HYBR_3 again. */
        if(mmoc_info_ptr->active_ss & ACTIVE_SS_HYBR_3)
        {
          (void)mmoc_activate_prot_online_ss_hybr_3( sd_ss_hybr_3_act_get(NULL),
                                                  mmoc_info_ptr
                                                );
        }
        else
        {
          MMOC_ERR_2 ("AUTO_DEACTD_IND from HYBR_3, DS_Pref = %d, single_stby_ss = %d",
                                                                       mmoc_info_ptr->standby_pref ,
                                                                       mmoc_info_ptr->active_ss);
          mmoc_update_prot_state( PROT_STATE_NULL,
                                  SD_SS_HYBR_3,
                                  mmoc_info_ptr
                                );

        }

        return MMOC_EVT_STATUS_CONSUMED;
      }
      #endif //FEATURE_MMODE_TRIPLE_SIM

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Check if auto deactivation from HYBR_2 stack */
      if(mmoc_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM) &&
         rpt_ptr->param.prot_deactd_ind.ss == SD_SS_HYBR_2 &&
         mmoc_info_ptr->prot_state[SD_SS_HYBR_2] == PROT_STATE_ONLINE_GWL)
      {
        if((mmoc_is_sxlte() &&
		   mmoc_info_ptr->active_ss & ACTIVE_SS_MAIN) ||
		   mmoc_info_ptr->active_ss & ACTIVE_SS_HYBR_2)
        {
          /* check main active for SG/SV  + G */
          (void)mmoc_activate_prot_online_ss_hybr_2( sd_ss_hybr_2_act_get(NULL),
                                                  mmoc_info_ptr
                                                );
        }
        else
        {
          MMOC_ERR_2 ("AUTO_DEACTD_IND from HYBR_2, DS_Pref = %d, active_ss = %d",
                                                  mmoc_info_ptr->standby_pref ,
                                                                       mmoc_info_ptr->active_ss);
          mmoc_update_prot_state( PROT_STATE_NULL,
                                  SD_SS_HYBR_2,
                                  mmoc_info_ptr
                                );

        }

        return MMOC_EVT_STATUS_CONSUMED;
      }
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      if(mmoc_is_featuremode(SYS_OVERALL_FEATURE_MODE_SGLTE)||
         mmoc_is_featuremode(SYS_OVERALL_FEATURE_MODE_SVLTE))
      {
        /* Check if auto deactivation from HYBR_2 stack
         */
        if(rpt_ptr->param.prot_deactd_ind.ss == SD_SS_HYBR_2 &&
           mmoc_info_ptr->prot_state[SD_SS_HYBR_2] == PROT_STATE_ONLINE_GWL)
        {

          (void)mmoc_activate_prot_online_ss_hybr_2( sd_ss_hybr_2_act_get(NULL),
                                                    mmoc_info_ptr
                                                  );
          return MMOC_EVT_STATUS_CONSUMED;
        }
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      if(mmoc_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM))
      {
        /* For dual standby, activate MAIN only if MAIN should be active.
        */
      if(mmoc_info_ptr->active_ss & ACTIVE_SS_MAIN)
        {
          /*  Activate main at the end of transaction.
          */
          return mmoc_activate_prot_online_ss_main( sd_ss_act_get( NULL ),
                                                    mmoc_info_ptr
                                                  );
        }
        return MMOC_EVT_STATUS_CONSUMED;
      }
      else
      {
        /*  Activate main at the end of transaction.
        */
        return mmoc_activate_prot_online_ss_main( sd_ss_act_get( NULL ),
                                                  mmoc_info_ptr);
      }
  }



  if ( rpt_ptr->rpt.name != MMOC_RPT_MMGSDI_CNF )
  {
    MMOC_ERR_1("Invalid rpt %d, while waiting for session open cnf",
                    rpt_ptr->rpt.name);

    return MMOC_EVT_STATUS_CONSUMED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (MMOC_MMGSDI_SUCCESS == mmocmmgsdi_get_session_index(
            (mmgsdi_session_type_enum_type) MMGSDI_1X_PROV_PRI_SESSION,
            &index_1x_pri)) &&
      (MMOC_MMGSDI_SUCCESS == mmocmmgsdi_get_session_index(
            (mmgsdi_session_type_enum_type) MMGSDI_GW_PROV_PRI_SESSION,
            &index_gw_pri)) &&
      (MMOC_MMGSDI_SUCCESS == mmocmmgsdi_get_session_index(
            (mmgsdi_session_type_enum_type) MMGSDI_CARD_SESSION_SLOT_1,
            &index_card_slot_1))   &&
      (MMOC_MMGSDI_SUCCESS == mmocmmgsdi_get_session_index(
            (mmgsdi_session_type_enum_type) MMGSDI_CARD_SESSION_SLOT_2,
            &index_card_slot_2))
    )
  {
    if( mmgsdi_info_ptr->mmgsdi_session_id_table[index_1x_pri].session_opened &&
        mmgsdi_info_ptr->mmgsdi_session_id_table[index_gw_pri].session_opened &&
        mmgsdi_info_ptr->mmgsdi_session_id_table[index_card_slot_1].session_opened &&
        mmgsdi_info_ptr->mmgsdi_session_id_table[index_card_slot_2].session_opened
      )
    {
      all_sessions_opened = TRUE;
    }
  }
  else
  {
    MMOC_MSG_HIGH_0("Pri Session Index not found");
    return MMOC_EVT_STATUS_CONSUMED;
  }
  MMOC_MSG_HIGH_4("sess opn cnf: 1x=%d, gw=%d, card_1=%d card_2 %d",mmgsdi_info_ptr->mmgsdi_session_id_table[index_1x_pri].session_opened,\
  mmgsdi_info_ptr->mmgsdi_session_id_table[index_gw_pri].session_opened,\
  mmgsdi_info_ptr->mmgsdi_session_id_table[index_card_slot_1].session_opened, 
  mmgsdi_info_ptr->mmgsdi_session_id_table[index_card_slot_2].session_opened);

  #ifdef FEATURE_MMODE_DUAL_SIM
    if( MMOC_MMGSDI_SUCCESS == mmocmmgsdi_get_session_index(
              (mmgsdi_session_type_enum_type) MMGSDI_GW_PROV_SEC_SESSION,
              &index_gw_sec)
      )
    {
      if(!mmgsdi_info_ptr->mmgsdi_session_id_table[index_gw_sec].session_opened)
      {
        all_gw2_sessions_opened = FALSE;
      }
    }
    else
    {
      MMOC_MSG_HIGH_0("Sec Session Index not found");
      return MMOC_EVT_STATUS_CONSUMED;
    }
    MMOC_MSG_HIGH_1("sess opn cnf:session_opened, gw2=%d",mmgsdi_info_ptr->mmgsdi_session_id_table[index_gw_sec].session_opened);
  
  
  #if defined FEATURE_MMODE_TRIPLE_SIM
  
    if( MMOC_MMGSDI_SUCCESS == mmocmmgsdi_get_session_index(
              (mmgsdi_session_type_enum_type) MMGSDI_GW_PROV_TER_SESSION,
              &index_gw_ter)
      )
    {
      if(!mmgsdi_info_ptr->mmgsdi_session_id_table[index_gw_ter].session_opened)
      {
        all_gw3_sessions_opened = FALSE;
      }
    }
    else
    {
      MMOC_MSG_HIGH_0("Ter Session Index not found");
      return MMOC_EVT_STATUS_CONSUMED;
    }
    MMOC_MSG_HIGH_1("sess opn cnf:session_opened gw3=%d",mmgsdi_info_ptr->mmgsdi_session_id_table[index_gw_ter].session_opened);
  
  #endif
  
  #endif

  /* Once all the sessions are opened go to the next state */
  MMOC_MSG_HIGH_1("sess: all_sessions_opened=%d",all_sessions_opened);
  #ifdef FEATURE_MMODE_DUAL_SIM
  MMOC_MSG_HIGH_1("sess: all_gw2_sessions_opened=%d",all_gw2_sessions_opened);
  #endif
  #ifdef FEATURE_MMODE_TRIPLE_SIM
    MMOC_MSG_HIGH_1("sess: all_gw3_sessions_opened=%d",all_gw3_sessions_opened);
  #endif
  if(all_sessions_opened
      #ifdef FEATURE_MMODE_DUAL_SIM
      && all_gw2_sessions_opened
      #endif
      #ifdef FEATURE_MMODE_TRIPLE_SIM
      && all_gw3_sessions_opened
      #endif
  )
  {
    /* Transaction cleared.
    **/
    return mmoc_clear_transaction(mmoc_info_ptr);
  }
  else
  {
    return MMOC_EVT_STATUS_CONSUMED;
  }

} /* mmocmmgsdi_process_session_open_cnf() */


/*===========================================================================

FUNCTION mmocmmgsdi_process_mmgsdi_info_ind

DESCRIPTION
  This function is the transaction handler for MMOC_TRANS_MMGSDI_INFO_IND
  transaction.


DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
mmoc_evt_status_e_type         mmocmmgsdi_process_mmgsdi_info_ind
(

        const mmoc_cmd_msg_s_type      *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type      *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  mmoc_evt_status_e_type evt_status = MMOC_EVT_STATUS_CONSUMED;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Invoke the appropriate transaction state handler.
  */
  switch(mmoc_info_ptr->trans_state)
  {
    case MMOC_TRANS_STATE_NULL:
      evt_status = mmocmmgsdi_process_mmgsdi_info_null( cmd_ptr,
                                                        mmoc_info_ptr
                                                      );
      break;

    case MMOC_TRANS_STATE_WAIT_SESSION_OPEN_CNF:
      evt_status = mmocmmgsdi_process_session_open_cnf( rpt_ptr,
                                                        mmoc_info_ptr
                                                        );
    break;

    default:
      MMOC_ERR_1("Received event in unknown state, state =%d",
                     mmoc_info_ptr->trans_state);

      evt_status = mmoc_clear_transaction( mmoc_info_ptr );

      break;
  }

  return evt_status;

} /* mmocmmgsdi_process_mmgsdi_info_ind() */

/* <EJECT> */

#endif /* #ifdef FEATURE_MMGSDI_SESSION_LIB */

