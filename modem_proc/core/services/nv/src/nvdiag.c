/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Nonvolatile Storage Diagnostics Packet Processing

General Description
  Diagnostic packet processing routines for NV memory operations.

Copyright (c) 1990-2016 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/services/nv/src/nvdiag.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/14   sri     ESN/MEID over write support via SSM
02/28/14   sri     Max trials of SPC unlock over diag handling...
09/02/13   rks     Replaced memcpy with memscpy
03/04/13   sri     Removed handling for NV_PEEK_F and NV_POKE_F cmds
03/18/13   rks     CR525590:removed modem API depedency and feature FEATURE_NV_MEID_ENABLE
03/04/13   sri     Removed handling for NV_PEEK_F and NV_POKE_F cmds
09/01/09   pc      Made changes for Dual SIM / Dual Standby
08/19/09   pc      Replaced DIAG_MEID_ENABLE with FEATURE_NV_MEID_ENABLE
07/28/09   pc      Blocked write access to NV_SECTIME_TIME_OFFSETS_I.
05/28/09   pc      Exported unreadable_nv_item() and unwritable_nv_item()
10/03/08   vg      Updated code to use PACK() vs. PACKED
12/14/06   as      Fixed critical lint errors.
11/21/06   ck      Added a couple of GPS items to unreadable NV item list
07/12/06   as      Fixed compiler warning.
10/31/05   as      Fixed lint errors.
07/25/05   sl      Made NV_DEVICE_SERIAL_NO_I SPC protected.
07/25/05   as      Removed FEATURE_SIRIUS and mainlined multiple nv write.
07/05/05   sl      Added support for SPC_TIMEOUT to double the timeout value
                   on consecutive incorrect SPCs.
06/30/05   as      Added multiple items NV Write packet to dispatch table.
05/04/05   as      Featurized nv_stat_enum_type to ensure uint16 for
                   external API.
04/26/05   as      Removed unnecessary call to mcc_meid_supported().
04/11/05   as      Moved function that calculates PESN from Diag to MC module.
04/04/05   as      Modified MEID code to use DIAG_MEID_ENABLE
03/30/05   sl      Added support for SPC_UNLOCK_TTL to unlock/lock the sp_state
03/30/05   as      Replaced FEATURE_NV_MEID_ENABLE with
                   FEATURE_DIAG_MEID_ENABLE.
03/14/05   as      Added MEID support
02/18/04   as      Fixed critical lint errors.
12/23/04   ec      Added code for multiple items NV Write with stacking items
                   in a single request packet.
11/02/04   as      Featurized the NV items in unreadable_nv_item() that are 
                   key related using a debug feature FEATURE_FFA_DEBUG_KEYS. 
10/06/04   as      Added code to prevent write access from diag, for a subset
                   of nv items
10/04/04   as      Made unreadable nv items protected by SPC for sp_nv_write 
09/14/04   as      Made passwords unreadable, to prevent cloning phones
05/18/04   as      Added code to prevent overwriting a nonzero ESN, in
                   nvdiag_write 
02/23/04   as      Updated SP_NV_WRITE_ITEM when the feature 
                   FEATURE_SSPR_ENHANCEMENTS is defined.
10/06/03   as      Added read access to mobile IP provisioning items.
05/07/03   jct     Added featurization to dissallow certain operations that
                   can access memory (FEATURE_DIAG_DISALLOW_MEM_OPS):
                      DIAG_NV_PEEK_F
                      DIAG_NV_POKE_F
05/05/03   as      Added NV_RTRE_CONFIG_I,NV_UIM_PREF_PROTOCOL_I to 
                   sp_nv_write_item()
01/08/03   ma      featurization for GSM PLT
08/20/02   lad     FEATURE_DIAG_COUPLED_ARCHITECTURE/1X support.
01/21/02   as      Moved diag_nv_read(), diag_nv_write to diagnv.c
09/18/01   jal     Fixed allocation for NV_READ response.  Fixed handling
                   of SPC unlocking.
08/20/01   jal     Updated for core VU
06/27/01   jal     Created file.
===========================================================================*/
#include "core_variation.h"
#include <stringl/stringl.h>

#ifdef __cplusplus
  extern "C" {
#endif

#include "comdef.h"
#ifdef CUST_H
#endif
#include "diagcmd.h"
#include "diag.h"
#include "diagnv.h"
#include "diagnv_v.h"
#include "diagdiag.h"
#include "nvdiag.h"
#include "diagtune.h"
#include "timetick.h"
#include "nvim.h"

#ifdef FEATURE_IS2000
  #include "mccreg.h"
#endif


#include "nv.h"

#include "msg.h"

extern boolean diag_set_sp_state( diag_sp_type sp );

typedef struct{
  uint8  item_data[DIAG_NV_ITEM_SIZE];
}nvdiag_large_write1_item;

#define NVDIAG_LARGE_WRITE1_ITEM(X) \
     (((nvdiag_large_write1_item *)X)->item_data)

/** holds function pointer registered by 1X-CP.*/
static nv_1x_cp_cb_type *nv_1xcp_cb = NULL;

/**< Pseudo-ESN prefix. */
static uint32 nv_mcc_pesn_prefix_mask = 0;
static uint32 nv_mcc_pesn_prefix = 0; 

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
/* Tell lint that we are OK with the fact that pkt_len and req are not
** used in some of the packets, as this is dictated by the awesome design */
/*lint -esym(715,pkt_len,req) */
/**
  function to be used to register an API from 1X-CP.

@return
  TRUE  -- if registration is successfull.\n

@dependencies
  None.
*/

boolean nv_mc_init_fp(
   nv_1x_cp_cb_type * fp, 
   uint32 mcc_pesn_prefix_mask,
   uint32 mcc_pesn_prefix
)
{
   ASSERT(fp);
   nv_1xcp_cb = fp;
   nv_mcc_pesn_prefix_mask = mcc_pesn_prefix_mask;
   nv_mcc_pesn_prefix = mcc_pesn_prefix;
   return TRUE;
}


/*===========================================================================

FUNCTION SP_NV_WRITE_ITEM

DESCRIPTION
  This procedure checks the given nv item type to see if it's a Service
  Programming item, which should not be written until the SPC
  has been entered, or security unlocked.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it is a Service Programming item,
  FALSE if not

SIDE EFFECTS
  None.

============================================================================*/
static boolean sp_nv_write_item (
  nv_items_enum_type item
)
{
  switch (item) {
    case NV_SLOT_CYCLE_INDEX_I:
    case NV_CDMA_SID_LOCK_I:
    case NV_CDMA_SID_ACQ_I:
    case NV_ANALOG_SID_LOCK_I:
    case NV_ANALOG_SID_ACQ_I:
    case NV_ANALOG_FIRSTCHP_I:
    case NV_ANALOG_HOME_SID_I:
    case NV_ANALOG_REG_I:
    case NV_PCDMACH_I:
    case NV_SCDMACH_I:
    case NV_NAM_CHKSUM_I:
    case NV_MIN1_I:
    case NV_MIN2_I:
    case NV_MOB_TERM_HOME_I:
    case NV_MOB_TERM_FOR_SID_I:
    case NV_MOB_TERM_FOR_NID_I:
    case NV_ACCOLC_I:
    case NV_HOME_SID_NID_I:
    case NV_SID_NID_I:
    case NV_MIN_CHKSUM_I:
    case NV_SEC_CODE_I:
    case NV_IMSI_MCC_I:
    case NV_IMSI_11_12_I:
    case NV_DIR_NUMBER_I:
    case NV_FSC_I:
    case NV_WDC_I:
    case NV_FSC2_I:
    case NV_FSC2_CHKSUM_I:
    case NV_IMSI_ADDR_NUM_I:
    case NV_DIR_NUMBER_PCS_I:
    case NV_OTKSL_I:
    case NV_RTRE_CONFIG_I:
    case NV_UIM_PREF_PROTOCOL_I:
    case NV_PAP_USER_ID_I:
    case NV_PAP_PASSWORD_I:
    case NV_A_KEY_I:
    case NV_A_KEY_CHKSUM_I:
    case NV_SSD_A_I:
    case NV_SSD_B_I:
    case NV_UP_KEY_I:
    case NV_ROAMING_LIST_I:
    case NV_ROAMING_LIST_683_I:   
    case NV_HDR_AN_AUTH_PASSWORD_I:
    case NV_HDR_AN_AUTH_PASSWD_LONG_I:
    case NV_HDR_AN_PPP_PASSWORD_I:
    case NV_PPP_PASSWORD_I:
    case NV_DS_MIP_SS_USER_PROF_I:
    case NV_DS_MIP_DMU_MN_AUTH_I:
    case NV_DEVICE_SERIAL_NO_I:
    case NV_HDRSCMDB_SECURE_CONFIG_I:
    case NV_GPSONE_PASSWORD_I:
    case NV_GPS1_SEC_UPDATE_RATE_I:

      return (TRUE);

    default:
      return (FALSE);
  }
} /* sp_nv_write_item */



#ifdef FEATURE_NV_WRITE_ONLINE
/*===========================================================================

FUNCTION DRAM_NV_WRITE_ITEM

DESCRIPTION
  This procedure checks the given nv item type to see if it's accessible
  using the "NV WRITE ONLINE" command which modifies both the NV ITEM
  as well as the variable that shadows the NV ITEM and actually gets
  used.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it is allowed to be written to by "NV WRITE ONLINE" command,
  FALSE if not

SIDE EFFECTS
  None.

============================================================================*/
static boolean dram_nv_write_item (
  nv_items_enum_type item
)
{
  switch (item) {
    case NV_AUTO_LOCK_I:
    case NV_AUTO_NAM_I:
    case NV_BEEP_LVL_I:
    case NV_CALL_BEEP_I:
    case NV_CALL_FADE_ALERT_I:
    case NV_CONT_KEY_DTMF_I:
    case NV_CURR_NAM_I:
    case NV_LOCK_CODE_I:
    case NV_SPEAKER_LVL_I:
    case NV_SVC_AREA_ALERT_I:
    case NV_AUTO_ANSWER_I:
    case NV_EAR_LVL_I:
    case NV_LOCK_I:
    case NV_BEEP_SPKR_LVL_I:
    case NV_VOICE_PRIV_I:
    case NV_CDMA_PREF_SERV_I:
    case NV_AIR_TIMER_I:
    case NV_ROAM_TIMER_I:
    case NV_AIR_CNT_I:
    case NV_ROAM_CNT_I:

      return (TRUE);

    default:
      return (FALSE);
  } /* switch (item) */

} /* dram_nv_write_item */
#endif /* #ifdef FEATURE_NV_WRITE_ONLINE */



/*===========================================================================

FUNCTION SP_NV_READ_ITEM

DESCRIPTION
  This procedure checks the given nv item type to see if it's a Service
  Programming item, which should not be read until the SPC has been
  entered, or security unlocked.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it is a Service Programming item,
  FALSE if not

SIDE EFFECTS
  None.

============================================================================*/
static boolean sp_nv_read_item (
  nv_items_enum_type item
)
{
  switch (item) {
    case NV_SEC_CODE_I:
    case NV_FSC_I:
    case NV_FSC2_I:
    case NV_FSC2_CHKSUM_I:
    case NV_OTKSL_I:
    case NV_PAP_USER_ID_I:
    case NV_PPP_USER_ID_I:
    case NV_HDR_AN_AUTH_NAI_I:
    case NV_DEVICE_SERIAL_NO_I:

      return (TRUE);

    default:
      return (FALSE);
  }
} /* sp_nv_read_item */



/*===========================================================================

FUNCTION UNREADABLE_NV_ITEM

DESCRIPTION
  This procedure checks the given nv item type to see if it's one of
  the few items which are never allowed to be read.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it is unreadable.
  FALSE if it's not.

SIDE EFFECTS
  None.

============================================================================*/
boolean unreadable_nv_item (
  nv_items_enum_type item
)
{
  switch (item) {

    /* Keys, secrets, and passwords are unreadable */
    case NV_A_KEY_I:
    case NV_A_KEY_CHKSUM_I:
    case NV_SSD_A_I:
    case NV_SSD_B_I:
    case NV_UP_KEY_I:
#ifndef FEATURE_FFA_DEBUG_KEYS
    case NV_HDR_AN_AUTH_PASSWORD_I:
    case NV_HDR_AN_AUTH_PASSWD_LONG_I:
    case NV_HDR_AN_PPP_PASSWORD_I:
    case NV_PPP_PASSWORD_I:
    case NV_PAP_PASSWORD_I:
    case NV_DS_MIP_SS_USER_PROF_I:
    case NV_DS_MIP_DMU_MN_AUTH_I:
#endif
    case NV_SEC_DEVICE_KEY_I :

    /* We can't read the roaming lists using the usual nv_read command */
    case NV_ROAMING_LIST_I:
    case NV_ROAMING_LIST_683_I:

    /* GPS passwords are unreadable */
    case NV_GPSONE_PASSWORD_I:
    case NV_GPS1_SEC_UPDATE_RATE_I:
      
      return (TRUE);

    default:
      return (FALSE);
  }
} /* unreadable_nv_item */



/*===========================================================================

FUNCTION UNWRITABLE_NV_ITEM

DESCRIPTION
  This procedure checks the given nv item type to see if it's one of
  the few items which are never allowed to be written. 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it is unreadable.
  FALSE if it's not.

SIDE EFFECTS
  None.

============================================================================*/
boolean unwritable_nv_item (
  nv_items_enum_type item
)
{
  switch (item) {

    /* Keys that are not unwritable */
    case NV_SEC_DEVICE_KEY_I :
    case NV_DIAG_SPC_UNLOCK_TTL_I:
    case NV_DIAG_SPC_TIMEOUT_I:
    case NV_SECTIME_TIME_OFFSETS_I:
       return (TRUE);

    default:
      return (FALSE);
  }
} /* unwritable_nv_item */


/*===========================================================================

FUNCTION NVDIAG_READ

DESCRIPTION
  This procedure processes a request to read an NV item.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

============================================================================*/
void * nvdiag_read (
  void * req_pkt,
  uint16 pkt_len     
)
{
  DIAG_NV_READ_F_req_type *req = (DIAG_NV_READ_F_req_type *) req_pkt;
  DIAG_NV_READ_F_rsp_type *rsp;
  const int rsp_len = sizeof( DIAG_NV_READ_F_rsp_type );

  nv_items_enum_type eItem = (nv_items_enum_type)req->item ;
  nv_item_type       *pItemData;


  /*-----------------------------------------------------------------------
    First check to see if this item is not allowed to be read.  If it isn't,
    we will return an error packet.  NOTE:  These items are unreadable,
    even if security is unlocked!
  -----------------------------------------------------------------------*/
  if ( unreadable_nv_item(eItem) ) {
    return( diagpkt_err_rsp( DIAG_BAD_PARM_F, req_pkt, pkt_len ));
  }
   
  /*-----------------------------------------------------------------------
    Next check to see if this item we're requesting is a SP item, and if
    it is, make sure the SPC has been entered properly first, or that
    security is unlocked.  If it hasn't, we will return an error packet.
    Also make sure the security password is not unwritten.  If the security
    password is unwritten, then we rely on the SPC state flag.
  -----------------------------------------------------------------------*/
  else  if (sp_nv_read_item(eItem) &&
       (diag_get_sp_state() == DIAG_SPC_LOCKED) &&
       ((diag_get_security_state() == DIAG_SEC_LOCKED) ||
        diag_check_password (NULL) == TRUE))
  {
    return (diagpkt_err_rsp (DIAG_BAD_SPC_MODE_F, req_pkt, pkt_len));
  }

  /*-----------------------------------------------------------------------
    Anything else is readable at any time.
  -----------------------------------------------------------------------*/
  else {

    if(nvim_op_get_size(eItem) > DIAG_NV_ITEM_SIZE)
    {
      return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
    }

    rsp = (DIAG_NV_READ_F_rsp_type *)diagpkt_alloc( DIAG_NV_READ_F, rsp_len );

    if( rsp == NULL ) {
      /* If we can't allocate, diagpkt_err_rsp() can't either. */
      return( NULL );
    }

    /*-----------------------------------------------------------------------
      Packet command id looks ok, format a command to NV to read the item.
      will wait for the response. The response packet is the same format
      as the request packet, so start by copying the data.
    -----------------------------------------------------------------------*/
    rsp->item = req->item;
    memscpy((void *) ( rsp->item_data ),
            DIAG_NV_ITEM_SIZE,
            (void *) ( req->item_data ),
            DIAG_NV_ITEM_SIZE);

    pItemData = (nv_item_type *) &rsp->item_data[0];

    /* Send the nv read request, and report the status of the
    ** nv operation back to dm. */
    rsp->nv_stat = diag_nv_read(eItem, pItemData);

    return ( rsp );
  }

} /* nvdiag_read */


/*===========================================================================

FUNCTION NVDIAG_READ_EXT

DESCRIPTION
  This procedure processes a request to read an NV item.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

============================================================================*/
void * nvdiag_read_ext (
  void * req_pkt,
  uint16 pkt_len     
)
{
  DIAG_NV_READ_EXT_F_req_type *req = (DIAG_NV_READ_EXT_F_req_type *) req_pkt;
  DIAG_NV_READ_EXT_F_rsp_type *rsp;
  const int rsp_len = sizeof( DIAG_NV_READ_EXT_F_rsp_type );

  nv_items_enum_type eItem = (nv_items_enum_type)req->item ;
  nv_item_type       *pItemData;
  uint16             eContext = (uint16) req->context;


  /*-----------------------------------------------------------------------
    First check to see if this item is not allowed to be read.  If it isn't,
    we will return an error packet.  NOTE:  These items are unreadable,
    even if security is unlocked!
  -----------------------------------------------------------------------*/
  if ( unreadable_nv_item(eItem) ) {
    return( diagpkt_err_rsp( DIAG_BAD_PARM_F, req_pkt, pkt_len ));
  }
   
  /*-----------------------------------------------------------------------
    Next check to see if this item we're requesting is a SP item, and if
    it is, make sure the SPC has been entered properly first, or that
    security is unlocked.  If it hasn't, we will return an error packet.
    Also make sure the security password is not unwritten.  If the security
    password is unwritten, then we rely on the SPC state flag.
  -----------------------------------------------------------------------*/
  else  if (sp_nv_read_item(eItem) &&
       (diag_get_sp_state() == DIAG_SPC_LOCKED) &&
       ((diag_get_security_state() == DIAG_SEC_LOCKED) ||
        diag_check_password (NULL) == TRUE))
  {
    return (diagpkt_err_rsp (DIAG_BAD_SPC_MODE_F, req_pkt, pkt_len));
  }

  /*-----------------------------------------------------------------------
    Anything else is readable at any time.
  -----------------------------------------------------------------------*/
  else {

    if(nvim_op_get_size(eItem) > DIAG_NV_ITEM_SIZE)
    {
      return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
    }

    rsp = (DIAG_NV_READ_EXT_F_rsp_type *)diagpkt_subsys_alloc (DIAG_SUBSYS_NV,
                                                               DIAG_SUBSYS_NV_READ_EXT_F,
                                                               rsp_len);

    if( rsp == NULL ) {
      /* If we can't allocate, diagpkt_err_rsp() can't either. */
      return( NULL );
    }

    /*-----------------------------------------------------------------------
      Packet command id looks ok, format a command to NV to read the item.
      will wait for the response. The response packet is the same format
      as the request packet, so start by copying the data.
    -----------------------------------------------------------------------*/
    rsp->item = req->item;
    memscpy((void *) ( rsp->item_data ),
            DIAG_NV_ITEM_SIZE,
            (void *) ( req->item_data ),
            DIAG_NV_ITEM_SIZE);
    rsp->context = req->context;
    pItemData = (nv_item_type *) &rsp->item_data[0];

    /* Send the nv read request, and report the status of the
    ** nv operation back to dm. */
    rsp->nv_stat = diag_nv_read_ext(eItem, pItemData, eContext);

    return ( rsp );
  }

} /* nvdiag_read_ext */




#ifndef FEATURE_GSM_PLT 
/*===========================================================================

FUNCTION CHK_REGISTRATION_PARAMETER_CHG

DESCRIPTION
  If the specified item to be written is one of the parameter-change
  registration variables: SLOT_CYCLE_INDEX, Station Class Mark, MOB_TERM_HOME,
  MOB_TERM_FOR_SID, or MOB_TERM_FOR_NID, and if the value of that item is
  changing, this routine causes a parameter-change registration event which
  will be noticed at the appropriate time.

  Since this write happens when the phone is offline, and must be reset,
  the phone won't notice that there was a change.  So we'll "notice" the
  change here and take advantage of another cause of parameter-change
  registration listed in 6.6.5.1.6: "Parameter-change registration is
  performed whenever there is no entry in the mobile-station's SID_NID_LIST
  that matches the base-station's SID and NID."

  If the new value is different from the old one we clear out the MC
  registration values (the whole SID_NID_LIST), so that if parameter-change
  registration is the only thing enabled, registration will occur.


DEPENDENCIES
  Assumes the specified item is about to be written to NV.
  This routine should be called only from nvdiag.c's nvdiag_write().

RETURN VALUE
  None.

SIDE EFFECTS
  MC's registration lists may be reinitialized.

============================================================================*/
static void chk_registration_parameter_chg (
  DIAG_NV_WRITE_F_req_type *req
)
{
  word i;       /* Loop index */
  boolean parameter_changed = FALSE;
                /* Flag is set to TRUE if a variable which causes Parameter-
                ** Change Registration is changed.
                */
  nv_items_enum_type eItem = (nv_items_enum_type) req->item;
  nv_item_type *pItemData = (nv_item_type *) &req->item_data[0];
  nv_item_type nvitem; //XXX

  switch (eItem) {
    case NV_SLOT_CYCLE_INDEX_I:
      if ( diag_nv_read(NV_SLOT_CYCLE_INDEX_I, &nvitem) != NV_DONE_S ) {
        NV_MSG_HIGH("Failed to read Slot Cycle Index");
      }
      else if (nvitem.slot_cycle_index != pItemData->slot_cycle_index) {
        parameter_changed = TRUE;
      }
      break;

    case NV_SCM_I:
      if ( diag_nv_read(NV_SCM_I, &nvitem) != NV_DONE_S ) {
        NV_MSG_HIGH("Failed to read Station Class Mark");
      }
      else if (nvitem.scm != pItemData->scm) {
        parameter_changed = TRUE;
      }
      break;

    case NV_MOB_TERM_HOME_I:
      nvitem.mob_term_home.nam = pItemData->mob_term_home.nam;

      if ( diag_nv_read(NV_MOB_TERM_HOME_I, &nvitem) != NV_DONE_S ) {
        NV_MSG_HIGH("Failed to read Mob_Term_Home");
      } else {
        for (i = 0; i < NV_MAX_MINS; i++) {
          if (nvitem.mob_term_home.enabled[i] !=
                pItemData->mob_term_home.enabled[i]) {
            parameter_changed = TRUE;
          }
        }        /* end loop over the MINs' flags in this NAM    */
      }          /* end else the nv_read worked correctly        */
      break;

    case NV_MOB_TERM_FOR_SID_I:
      nvitem.mob_term_for_sid.nam = pItemData->mob_term_for_sid.nam;

      if ( diag_nv_read(NV_MOB_TERM_FOR_SID_I, &nvitem) != NV_DONE_S ) {
        NV_MSG_HIGH("Failed to read Mob_Term_For_SID");
      } else {
        for (i = 0; i < NV_MAX_MINS; i++) {
          if (nvitem.mob_term_for_sid.enabled[i] !=
                pItemData->mob_term_for_sid.enabled[i]) {
            parameter_changed = TRUE;
          }
        }        /* end loop over the MINs' flags in this NAM    */
      }          /* end else the nv_read worked correctly        */
      break;

    case NV_MOB_TERM_FOR_NID_I:
      nvitem.mob_term_for_nid.nam = pItemData->mob_term_for_nid.nam;

      if ( diag_nv_read(NV_MOB_TERM_FOR_NID_I, &nvitem) != NV_DONE_S ) {
        NV_MSG_HIGH("Failed to read Mob_Term_For_NID");
      } else {
        for (i = 0; i < NV_MAX_MINS; i++) {
          if (nvitem.mob_term_for_nid.enabled[i] !=
                pItemData->mob_term_for_nid.enabled[i]) {
            parameter_changed = TRUE;
          }
        }        /* end loop over the MINs' flags in this NAM    */
      }          /* end else the nv_read worked correctly        */
      break;

    default:
      /* nothing to do in default case */
      break;

  } /* end switch on item */

#ifdef FEATURE_IS2000
  if (parameter_changed) {
    mccreg_clear_reg();    /* clear sid_nid_list and registered flag, so   */
                           /* the phone will register if parameter-change  */
                           /* registration is the only type enabled.       */
  }
#else
  (void) parameter_changed;
#endif

} /* chk_registration_parameter_chg */


static void chk_registration_parameter_chg_ext (
  DIAG_NV_WRITE_EXT_F_req_type *req
)
{
  word i;       /* Loop index */
  boolean parameter_changed = FALSE;
                /* Flag is set to TRUE if a variable which causes Parameter-
                ** Change Registration is changed.
                */
  nv_items_enum_type eItem = (nv_items_enum_type) req->item;
  nv_item_type *pItemData = (nv_item_type *) &req->item_data[0];
  nv_item_type nvitem; //XXX
  uint16 eContext = (uint16) req->context;

  switch (eItem) {
    case NV_SLOT_CYCLE_INDEX_I:
      if ( diag_nv_read_ext(NV_SLOT_CYCLE_INDEX_I, &nvitem, eContext) != NV_DONE_S ) {
        NV_MSG_HIGH("Failed to read Slot Cycle Index");
      }
      else if (nvitem.slot_cycle_index != pItemData->slot_cycle_index) {
        parameter_changed = TRUE;
      }
      break;

    case NV_SCM_I:
      if ( diag_nv_read_ext(NV_SCM_I, &nvitem, eContext) != NV_DONE_S ) {
        NV_MSG_HIGH("Failed to read Station Class Mark");
      }
      else if (nvitem.scm != pItemData->scm) {
        parameter_changed = TRUE;
      }
      break;

    case NV_MOB_TERM_HOME_I:
      nvitem.mob_term_home.nam = pItemData->mob_term_home.nam;

      if ( diag_nv_read_ext(NV_MOB_TERM_HOME_I, &nvitem, eContext) != NV_DONE_S ) {
        NV_MSG_HIGH("Failed to read Mob_Term_Home");
      } else {
        for (i = 0; i < NV_MAX_MINS; i++) {
          if (nvitem.mob_term_home.enabled[i] !=
                pItemData->mob_term_home.enabled[i]) {
            parameter_changed = TRUE;
          }
        }        /* end loop over the MINs' flags in this NAM    */
      }          /* end else the nv_read worked correctly        */
      break;

    case NV_MOB_TERM_FOR_SID_I:
      nvitem.mob_term_for_sid.nam = pItemData->mob_term_for_sid.nam;

      if ( diag_nv_read_ext(NV_MOB_TERM_FOR_SID_I, &nvitem, eContext) != NV_DONE_S ) {
        NV_MSG_HIGH("Failed to read Mob_Term_For_SID");
      } else {
        for (i = 0; i < NV_MAX_MINS; i++) {
          if (nvitem.mob_term_for_sid.enabled[i] !=
                pItemData->mob_term_for_sid.enabled[i]) {
            parameter_changed = TRUE;
          }
        }        /* end loop over the MINs' flags in this NAM    */
      }          /* end else the nv_read worked correctly        */
      break;

    case NV_MOB_TERM_FOR_NID_I:
      nvitem.mob_term_for_nid.nam = pItemData->mob_term_for_nid.nam;

      if ( diag_nv_read_ext(NV_MOB_TERM_FOR_NID_I, &nvitem, eContext) != NV_DONE_S ) {
        NV_MSG_HIGH("Failed to read Mob_Term_For_NID");
      } else {
        for (i = 0; i < NV_MAX_MINS; i++) {
          if (nvitem.mob_term_for_nid.enabled[i] !=
                pItemData->mob_term_for_nid.enabled[i]) {
            parameter_changed = TRUE;
          }
        }        /* end loop over the MINs' flags in this NAM    */
      }          /* end else the nv_read worked correctly        */
      break;

    default:
      /* nothing to do in default case */
      break;

  } /* end switch on item */

#ifdef FEATURE_IS2000
  if (parameter_changed) {
    mccreg_clear_reg();    /* clear sid_nid_list and registered flag, so   */
                           /* the phone will register if parameter-change  */
                           /* registration is the only type enabled.       */
  }
#else
  (void) parameter_changed;
#endif

} /* chk_registration_parameter_chg_ext */

#endif // FEATURE_GSM_PLT



/*===========================================================================

FUNCTION NVDIAG_WRITE

DESCRIPTION
  This procedure processes a request to write an NV item.  It is overloaded
  to also do the "NV Write Online" command.  It is important that changes
  in one command should get reflected automatically in the other.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

============================================================================*/
void * nvdiag_write (
  void * req_pkt,
  uint16 pkt_len     
)
{
  DIAG_NV_WRITE_F_req_type *req = (DIAG_NV_WRITE_F_req_type *) req_pkt;
  DIAG_NV_WRITE_F_rsp_type *rsp;
  const int rsp_len = sizeof( DIAG_NV_WRITE_F_rsp_type );

  nv_items_enum_type eItem = (nv_items_enum_type) req->item;
  nv_item_type *pItemData = (nv_item_type *) &req->item_data[0];


  boolean ok_to_write;  /* boolean flag */
  
  /*-----------------------------------------------------------------------
    First check to see if this item is not allowed to be read.  If it isn't,
    we will return an error packet.  NOTE:  These items are unwritable,
    even if security is unlocked!
  -----------------------------------------------------------------------*/
  if ( unwritable_nv_item(eItem) ) {
    return( diagpkt_err_rsp( DIAG_BAD_PARM_F, req_pkt, pkt_len ));
  }

  /*-----------------------------------------------------------------------
    Next check to see if this item we're writing is a SP item, and if
    it is, make sure the SPC has been entered properly first, or that
    security is unlocked.  If it hasn't, we will return an error packet.
    Also make sure the security password is not unwritten.  If the security
    password is unwritten, then we rely on the SPC state flag.
  -----------------------------------------------------------------------*/
  else if ( sp_nv_write_item(eItem) &&
       (diag_get_sp_state() == DIAG_SPC_LOCKED ) &&
       ((diag_get_security_state() == DIAG_SEC_LOCKED) ||
         diag_check_password (NULL) == TRUE))
  {
    return( diagpkt_err_rsp( DIAG_BAD_SPC_MODE_F, req_pkt, pkt_len) );
  }
  /*-----------------------------------------------------------------------
    Next, see if the item being written is the ESN.
    We can't ever write the ESN if security is locked.
    Return the error packet for invalid security mode.
  -----------------------------------------------------------------------*/
  else if ((eItem == NV_ESN_I
            || eItem == NV_MEID_I
           ) && diag_get_security_state() == DIAG_SEC_LOCKED)
  {
    NV_MSG_MED_2( "Bad security mode: %d, %d",
                                  DIAG_BAD_SEC_MODE_F, diag_get_security_state() );
    return( diagpkt_err_rsp( DIAG_BAD_SEC_MODE_F, req_pkt, pkt_len) );
  }

  rsp = (DIAG_NV_WRITE_F_rsp_type *)diagpkt_alloc( DIAG_NV_WRITE_F, rsp_len );

  if( rsp == NULL ) {
    /* If we can't allocate, diagpkt_err_rsp() can't either. */
    return (NULL);
  }

  /*-----------------------------------------------------------------------
    If security is unlocked and ESN is something other than 0, and the
    password is all 0xfffff's (default, unwritten) then we can't overwrite it.
    Return status as if this was reported from NV in the old way - with
    status NV_READONLY.
  -----------------------------------------------------------------------*/
  /* MEID is supported */
  if (( eItem == NV_ESN_I || eItem == NV_MEID_I)
       &&(nv_1xcp_cb != NULL))
  {

    ok_to_write = FALSE;    /* set default value of ok_to_write to FALSE */
    boolean nv_1xcp_status = FALSE;
    qword meid;         /* Mobile Equipment Identifier */
    qword meid_0;       /* MEID with value 0 */
    dword  pesn;        /* Pseudo-ESN */
    nv_items_enum_type  eItemtemp;       /* temp Item ID */
    nv_item_type        ItemDatatemp;    /* temp Item Data */
    nv_stat_enum_type   nv_status;    /* nv status */

    /* check if the ESN is zero */
    if (diagnv_get_esn() == 0L)
    {
      /* ESN is zero */
      /* check the MEID */
      nv_status = diagnv_get_meid (meid);

      qw_set(meid_0, 0L, 0L);   /* initialize meid_0 to a value zero */

      if ( (nv_status == NV_DONE_S && qw_cmp(meid, meid_0) == 0) ||
            nv_status == NV_NOTACTIVE_S )
      {
        /* MEID is zero */
        if (eItem == NV_ESN_I )
        {
          /* We are trying to write an ESN
           * The ESN can be wriiten only when
           *  1. the MEID is zero
           *  2. the ESN is zero
           *  3. the ESN in the request is not a pseudo-ESN
           *  4. The Mobile P_REV is less than 11, 
           *     i.e. mcc_esn_programming_is_allowed_with_meid_support() = TRUE
           *  For P_REV 11 and later, mcc_esn_programming_is_allowed_with_meid_support() = FALSE
           *     the ESN is no longer programmable by itself
           */

          if ( nv_1xcp_cb(NV_MC_ESN_PROG_ALLOWED_WITH_MEID_SUPPORT_F, NULL, NULL) 
             &&((pItemData->esn.esn & nv_mcc_pesn_prefix_mask) != nv_mcc_pesn_prefix ) )
          {
            /* both the MEID and ESN are zero and the
             * new esn is not a pesn
             */
             ok_to_write = TRUE;
          }
        }
        else if (eItem == NV_MEID_I )
        {
          /* check the values we are attempting to write into the MEID */
          /* Writing MEID with a value 0 is not allowed */
          if (!(qw_cmp(pItemData->meid, meid_0) == 0)) 
          {
            /* We are trying to write the MEID
             * We need to write the ESN first and the MEID CD
             * get the MEID from the request packet
             * Get MEID from the request
             */
            qw_equ(meid, pItemData->meid);

            nv_1xcp_status  = nv_1xcp_cb(NV_MC_COMPUTE_PESN_F, &meid, &pesn);

            if(nv_1xcp_status)
           {
               /* populate a ESN write request with the pesn */
               eItemtemp = NV_ESN_I;   /* set Item ID to NV_ESN_I */
               ItemDatatemp.esn.esn = pesn; /* set Item Data to pesn */

               nv_status = diag_nv_write (eItemtemp, &ItemDatatemp);

               if (nv_status == NV_DONE_S) 
               {
                 /* now we can write the MEID */
                 ok_to_write = TRUE;
              }
              else
              {
                /* write failed */
                NV_MSG_MED_1("NVWRITE PESN Failed rc=%d", nv_status);
              }
            }
            else
            {
               NV_MSG_MED("NV_MC_COMPUTE_PESN_F Failed ");
            }
          }
        }
        else
        {
          /* Not ESN or MEID */
          ok_to_write = TRUE;
        }  /* if (eItem == NV_ESN_I )  */
      }  /* if ( (nv_status == NV_DONE_S && qw_cmp(meid, meid_0) == 0) ... */
    }  /* if (diagnv_get_esn() == 0L) */
  }  /* if ( eItem == NV_ESN_I || eItem == NV_MEID_I) */
  else if (eItem == NV_ESN_I && diagnv_get_esn () != 0)
  {
    /* we can not allow writes to a non-zero ESN */
    ok_to_write = FALSE;
  }
  else
  {
    /* The write is ok, let it through */
    ok_to_write = TRUE;
  }

  if (!ok_to_write) 
  {
    /*-----------------------------------------------------------------------
      The response packet is the same format
      as the request packet, so start by copying the data.
    -----------------------------------------------------------------------*/
    rsp->item = req->item;
    memscpy((void *) ( rsp->item_data ),
            DIAG_NV_ITEM_SIZE,
            (void *) ( req->item_data ),
            DIAG_NV_ITEM_SIZE);

    /* Set the status of the operation to READONLY */
    rsp->nv_stat = NV_READONLY_S;
  } else
  {
    /* If neither of those special cases, then we're allowed to write.
       The response packet is the same format as the request packet, so
       start by copying the data. */

    rsp->item = req->item;
    memscpy((void *) ( rsp->item_data ),
            DIAG_NV_ITEM_SIZE,
            (void *) ( req->item_data ),
            DIAG_NV_ITEM_SIZE);

#if !defined(FEATURE_HWTC) && !defined(FEATURE_GSM_PLT)     
    /* Check to see if it's one of the items that would trigger a
       parameter change registration */
    chk_registration_parameter_chg(req);
#endif /* !FEATURE_HWTC */

    /*-----------------------------------------------------------------------
      Packet command id looks ok, format a command to NV to write the item.
      will wait for the response.
    -----------------------------------------------------------------------*/

    /* Send the status of the operation back to dm */
    rsp->nv_stat = diag_nv_write (eItem, pItemData);
  }

  return ( rsp );
} /* nvdiag_write */


/*===========================================================================

FUNCTION NVDIAG_WRITE_EXT

DESCRIPTION
  This procedure processes a request to write an NV item to a particular SIM.  It is overloaded
  to also do the "NV Write Online" command.  It is important that changes
  in one command should get reflected automatically in the other.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

============================================================================*/
void * nvdiag_write_ext (
  void * req_pkt,
  uint16 pkt_len     
)
{
  DIAG_NV_WRITE_EXT_F_req_type *req = (DIAG_NV_WRITE_EXT_F_req_type *) req_pkt;
  DIAG_NV_WRITE_EXT_F_rsp_type *rsp;
  const int rsp_len = sizeof( DIAG_NV_WRITE_EXT_F_rsp_type );

  nv_items_enum_type eItem = (nv_items_enum_type) req->item;
  nv_item_type *pItemData = (nv_item_type *) &req->item_data[0];
  uint16 eContext = (uint16) req->context;


  boolean ok_to_write;  /* boolean flag */
  
  /*-----------------------------------------------------------------------
    First check to see if this item is not allowed to be read.  If it isn't,
    we will return an error packet.  NOTE:  These items are unwritable,
    even if security is unlocked!
  -----------------------------------------------------------------------*/
  if ( unwritable_nv_item(eItem) ) {
    return( diagpkt_err_rsp( DIAG_BAD_PARM_F, req_pkt, pkt_len ));
  }

  /*-----------------------------------------------------------------------
    Next check to see if this item we're writing is a SP item, and if
    it is, make sure the SPC has been entered properly first, or that
    security is unlocked.  If it hasn't, we will return an error packet.
    Also make sure the security password is not unwritten.  If the security
    password is unwritten, then we rely on the SPC state flag.
  -----------------------------------------------------------------------*/
  else if ( sp_nv_write_item(eItem) &&
       (diag_get_sp_state() == DIAG_SPC_LOCKED ) &&
       ((diag_get_security_state() == DIAG_SEC_LOCKED) ||
         diag_check_password (NULL) == TRUE))
  {
    return( diagpkt_err_rsp( DIAG_BAD_SPC_MODE_F, req_pkt, pkt_len) );
  }
  /*-----------------------------------------------------------------------
    Next, see if the item being written is the ESN.
    We can't ever write the ESN if security is locked.
    Return the error packet for invalid security mode.
  -----------------------------------------------------------------------*/
  else if ((eItem == NV_ESN_I
            || eItem == NV_MEID_I
           ) && diag_get_security_state() == DIAG_SEC_LOCKED)
  {
    NV_MSG_MED_2 ( "Bad security mode: %d, %d",
                                  DIAG_BAD_SEC_MODE_F, diag_get_security_state() );
    return( diagpkt_err_rsp( DIAG_BAD_SEC_MODE_F, req_pkt, pkt_len) );
  }

  rsp = (DIAG_NV_WRITE_EXT_F_rsp_type *)diagpkt_subsys_alloc( DIAG_SUBSYS_NV,
                                                              DIAG_SUBSYS_NV_WRITE_EXT_F,
                                                              rsp_len);

  if( rsp == NULL ) {
    /* If we can't allocate, diagpkt_err_rsp() can't either. */
    return (NULL);
  }

  /*-----------------------------------------------------------------------
    If security is unlocked and ESN is something other than 0, and the
    password is all 0xfffff's (default, unwritten) then we can't overwrite it.
    Return status as if this was reported from NV in the old way - with
    status NV_READONLY.
  -----------------------------------------------------------------------*/
  /* MEID is supported */
  if (( eItem == NV_ESN_I || eItem == NV_MEID_I)
      &&(nv_1xcp_cb != NULL))
  {

    ok_to_write = FALSE;    /* set default value of ok_to_write to FALSE */
    qword meid;         /* Mobile Equipment Identifier */
    qword meid_0;       /* MEID with value 0 */
    dword  pesn;        /* Pseudo-ESN */
    nv_items_enum_type  eItemtemp;       /* temp Item ID */
    nv_item_type        ItemDatatemp;    /* temp Item Data */
    nv_stat_enum_type   nv_status;    /* nv status */

    /* check if the ESN is zero */
    if (diagnv_get_esn() == 0L)
    {
      /* ESN is zero */
      /* check the MEID */
      nv_status = diagnv_get_meid_ext (meid, eContext);

      qw_set(meid_0, 0L, 0L);   /* initialize meid_0 to a value zero */

      if ( (nv_status == NV_DONE_S && qw_cmp(meid, meid_0) == 0) ||
            nv_status == NV_NOTACTIVE_S )
      {
        /* MEID is zero */
        if (eItem == NV_ESN_I )
        {
          /* We are trying to write an ESN
           * The ESN can be wriiten only when
           *  1. the MEID is zero
           *  2. the ESN is zero
           *  3. the ESN in the request is not a pseudo-ESN
           *  4. The Mobile P_REV is less than 11, 
           *     i.e. mcc_esn_programming_is_allowed_with_meid_support() = TRUE
           *  For P_REV 11 and later, mcc_esn_programming_is_allowed_with_meid_support() = FALSE
           *     the ESN is no longer programmable by itself
           */
          if ( nv_1xcp_cb(NV_MC_ESN_PROG_ALLOWED_WITH_MEID_SUPPORT_F,NULL,NULL) &&
              ((pItemData->esn.esn & nv_mcc_pesn_prefix_mask) != nv_mcc_pesn_prefix ) )       
          {
            /* both the MEID and ESN are zero and the
             * new esn is not a pesn
             */
             ok_to_write = TRUE;
          }
        }
        else if (eItem == NV_MEID_I )
        {
          /* check the values we are attempting to write into the MEID */
          /* Writing MEID with a value 0 is not allowed */
          if (!(qw_cmp(pItemData->meid, meid_0) == 0)) 
          {
            boolean nv_1xcp_status = FALSE;
            /* We are trying to write the MEID
             * We need to write the ESN first and the MEID CD
             * get the MEID from the request packet
             * Get MEID from the request
             */
            qw_equ(meid, pItemData->meid);

            nv_1xcp_status  = nv_1xcp_cb(NV_MC_COMPUTE_PESN_F, &meid, &pesn);

            if(nv_1xcp_status)
            {
            /* populate a ESN write request with the pesn */
               eItemtemp = NV_ESN_I;   /* set Item ID to NV_ESN_I */
               ItemDatatemp.esn.esn = pesn; /* set Item Data to pesn */

               nv_status = diag_nv_write_ext (eItemtemp, &ItemDatatemp, eContext);

               if (nv_status == NV_DONE_S) 
               {
                  /* now we can write the MEID */
                 ok_to_write = TRUE;
               }
               else
               {
                 /* write failed */
                 NV_MSG_MED_1("NVWRITE PESN Failed rc=%d", nv_status);
               }
            }
            else
            {
               NV_MSG_MED("NV_MC_COMPUTE_PESN_F Failed ");
            }
          }
        }
        else
        {
          /* Not ESN or MEID */
          ok_to_write = TRUE;
        }  /* if (eItem == NV_ESN_I )  */
      }  /* if ( (nv_status == NV_DONE_S && qw_cmp(meid, meid_0) == 0) ... */
    }  /* if (diagnv_get_esn() == 0L) */
  }  /* if ( eItem == NV_ESN_I || eItem == NV_MEID_I) */
  else if ((eItem == NV_ESN_I) && (diagnv_get_esn () != 0))
  {
    /* we can not allow writes to a non-zero ESN */
    ok_to_write = FALSE;
  }
  else
  {
    /* The write is ok, let it through */
    ok_to_write = TRUE;
  }

  if (!ok_to_write) 
  {
    /*-----------------------------------------------------------------------
      The response packet is the same format
      as the request packet, so start by copying the data.
    -----------------------------------------------------------------------*/
    rsp->item = req->item;
    memscpy((void *) ( rsp->item_data ),
            DIAG_NV_ITEM_SIZE,
            (void *) ( req->item_data ),
            DIAG_NV_ITEM_SIZE);
    rsp->context = req->context;
    /* Set the status of the operation to READONLY */
    rsp->nv_stat = NV_READONLY_S;
  } else
  {
    /* If neither of those special cases, then we're allowed to write.
       The response packet is the same format as the request packet, so
       start by copying the data. */

    rsp->item = req->item;
    memscpy((void *) ( rsp->item_data ),
             DIAG_NV_ITEM_SIZE,
            (void *) ( req->item_data ),
            DIAG_NV_ITEM_SIZE);
    rsp->context = req->context;
#if !defined(FEATURE_HWTC) && !defined(FEATURE_GSM_PLT)     
    /* Check to see if it's one of the items that would trigger a
       parameter change registration */
    chk_registration_parameter_chg_ext(req);
#endif /* !FEATURE_HWTC */

    /*-----------------------------------------------------------------------
      Packet command id looks ok, format a command to NV to write the item.
      will wait for the response.
    -----------------------------------------------------------------------*/

    /* Send the status of the operation back to dm */
    rsp->nv_stat = diag_nv_write_ext (eItem, pItemData, eContext);
  }

  return ( rsp );
} /* nvdiag_write_ext */


/*===========================================================================

FUNCTION NVDIAG_WRITE1

DESCRIPTION
  This procedure processes a request to write an NV item. The input argument
  is a short packet which has various length depending on the item. The
  procedure converts the input short packet to a standard packet and call 
  nddiag_write() to write the NV item as regular. The response packet in 
  standard packet returned from calling nvdiag_write() is converted back to
  short packet for sending back to DM later.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

============================================================================*/
void * nvdiag_write1 (
  void * req_pkt,
  uint8 command_code     
)
{
  DIAG_NV_WRITE1_F_req_type *req_short = (DIAG_NV_WRITE1_F_req_type *)req_pkt;
  DIAG_NV_WRITE_F_req_type req_buf;
  DIAG_NV_WRITE_F_req_type *req = &req_buf;
  DIAG_NV_WRITE_F_rsp_type *rsp;

  /* storing the status from response packet to request packet */
  if(req == NULL)
  {
    req_short->nv_stat = DIAG_NV_WRITE_PKT_EXHAUSTED;
    return (NULL);
  }
  else 
  if (req_short->item_len == 0 || req_short->item_len > DIAG_NV_ITEM_SIZE)
  {
    req_short->nv_stat = DIAG_NV_WRITE_ITEM_LEN_BAD;
    return (NULL);
  }

  /* converting from short req packet format to standard req packet format */
  memset((void *)req, 0, sizeof(DIAG_NV_WRITE_F_req_type));
  req->xx_header.opaque_header = (byte)command_code;
  req->item = req_short->item;
  memscpy((void *)req->item_data, DIAG_NV_ITEM_SIZE, 
          (void *)req_short->item_data, req_short->item_len);
  req->nv_stat = req_short->nv_stat;

  /* write item using standard packet and get rsp in standard packet and 
     free the standard req packet after writing is done */
  rsp = (DIAG_NV_WRITE_F_req_type *)nvdiag_write(req, sizeof(DIAG_NV_WRITE_F_req_type));

  /* storing the status from response packet to request packet */
  if (rsp == NULL)
    req_short->nv_stat = DIAG_NV_WRITE_PKT_EXHAUSTED;
  else if (rsp->xx_header.opaque_header == DIAG_BAD_PARM_F)
  {
    req_short->nv_stat = DIAG_NV_WRITE_PARAM_BAD;
    /* free rsp pakcet as it is too small (16 byte of data) */
    diagpkt_free((void *)rsp);
    rsp = NULL;
  }
  else if (rsp->xx_header.opaque_header == DIAG_BAD_SPC_MODE_F)
  {
    req_short->nv_stat = DIAG_NV_WRITE_SPC_MODE_BAD;
    /* free rsp pakcet as it is too small (16 byte of data) */
    diagpkt_free((void *)rsp);
    rsp = NULL;
  }
  else if (rsp->xx_header.opaque_header == DIAG_BAD_SEC_MODE_F)
  {
    req_short->nv_stat = DIAG_NV_WRITE_SEC_MODE_BAD;
    /* free rsp pakcet as it is too small (16 byte of data) */
    diagpkt_free((void *)rsp);
    rsp = NULL;
  }
  else
    req_short->nv_stat = (uint8)rsp->nv_stat;

  return (void *)rsp;
} /* nvdiag_write1 */


/*===========================================================================

FUNCTION NVDIAG_WRITE2

DESCRIPTION
    This procedure processes a request to write multiple NV items. The input
  argument is a packet which has stack of items with various length depending
  on each item. The procedure goes through each item's NV write request in
  short packet and pass the short packet associated with the command code to
  nvdiag_write1() to handle the write. The response packet returned from the
  nvdiag_write1() will be freed except the response packet for first item. The
  first response packet is then used as the response packet of this procedure
  for sending back to DM later.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

============================================================================*/
void * nvdiag_write2 (
  void * req_pkt,
  uint16 pkt_len     
)
{
  DIAG_NV_WRITE2_F_req_type *req_multi;
  DIAG_NV_WRITE1_F_req_type *req_one;
  DIAG_NV_WRITE_F_rsp_type *rsp;
  DIAG_SUBSYS_CMD_F_rsp_type *rsp_pkt;
  uint8 command_code;
  uint16 status_total = 0;
  uint16 item_total;
  uint16 item_total_len;
  uint16 item_len_now = 0;
  uint16 item_len;
  uint16 i;

  /* setting the overall status of NV write to OK or 0 */
  req_multi = (DIAG_NV_WRITE2_F_req_type *)
              (((DIAG_SUBSYS_CMD_F_req_type *)req_pkt)->pkt);
  req_multi->nv_stat_total = 0;

  /* getting params of NV write */
  command_code = DIAG_NV_WRITE_F;
  item_total = req_multi->item_total;
  item_total_len = req_multi->item_total_len;

  /* checking total item & length - if it is wrong, stop processing further */
  if (item_total == 0)
  {
    status_total = DIAG_NV_WRITE_TOTAL_ITEM_NUM_INV;
  }
  else if (pkt_len > DIAG_MAX_RX_PKT_SIZ ||
           item_total_len == 0 ||
           item_total_len > (DIAG_MAX_RX_PKT_SIZ-(4*sizeof(uint8))))
  {
    status_total = DIAG_NV_WRITE_TOTAL_LEN_BAD;
  }
  else
  {
    req_one = (DIAG_NV_WRITE1_F_req_type *)&req_multi->item_data[0];
    for (i = 0; i < item_total; i++)
    {
      /* checking item length - if it is wrong, set item status to error */
      item_len = req_one->item_len ;
      item_len_now += sizeof(DIAG_NV_WRITE1_F_req_type)+item_len-sizeof(uint8);
      if ( (item_len >= DIAG_NV_ITEM_SIZE) && 
           ( (item_len > item_total_len) || (item_len_now > item_total_len) || 
         ( (item_len == 0) || ( (i == item_total-1) && (item_len_now != item_total_len) ) ) || (item_len > DIAG_NV_ITEM_SIZE) ) )
      {
        req_one->nv_stat = DIAG_NV_WRITE_ITEM_LEN_BAD;
        status_total = DIAG_NV_WRITE_ITEM_LEN_MISMATCH;
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len);
      } 
      else
      {
        req_one = (DIAG_NV_WRITE1_F_req_type *)
                   &NVDIAG_LARGE_WRITE1_ITEM(req_one->item_data)[item_len];
      }
    }
  }

  /* handling error situation - error stops entire NV write for all item */
  if (status_total)
  {
    /* set status for the error */ 
    req_multi->nv_stat_total = status_total;

    /* allocate a new rsp pkt with pkt size as input, copy the bad pkt to the
       new rsp pkt and send it back with status set                           */
    rsp_pkt = (DIAG_SUBSYS_CMD_F_rsp_type *)diagpkt_subsys_alloc(
                                                    DIAG_SUBSYS_NV,
                                                    DIAG_SUBSYS_NV_WRITE2_F,
                                                    pkt_len);
    if (rsp_pkt != NULL) 
      memscpy((void *)rsp_pkt,pkt_len, (void *)req_pkt, pkt_len);
        
    return ( (void *)rsp_pkt );
  }

  /* process NV write for each item */
  req_one = (DIAG_NV_WRITE1_F_req_type *)&req_multi->item_data[0];
  for (i = 0; i < item_total; i++)
  {
    /* processing one NV write */
    item_len = req_one->item_len ;
    rsp = (DIAG_NV_WRITE_F_rsp_type *)nvdiag_write1((void *)req_one,
                                                    command_code);
    /* free reqponse buffer */
    if (rsp != NULL)
      diagpkt_free((void *)rsp);

    /* update total status for this item */
    status_total += req_one->nv_stat;

    /* get the next NV write short packet */
    req_one = (DIAG_NV_WRITE1_F_req_type *)&req_one->item_data[item_len];
  }

  /* update total status */
  if (status_total)
    req_multi->nv_stat_total = DIAG_NV_WRITE_ITEM_FAILED;
  else
    req_multi->nv_stat_total = DIAG_NV_WRITE_TOTAL_SUCCESS;

  /* NV write is done, the responses with status from each processed item is
     copied to the saved rsp packet                                          */
  rsp_pkt = (DIAG_SUBSYS_CMD_F_rsp_type *)diagpkt_subsys_alloc(
                                                  DIAG_SUBSYS_NV,
                                                  DIAG_SUBSYS_NV_WRITE2_F,
                                                  pkt_len);
  if (rsp_pkt != NULL)
    memscpy((void *)rsp_pkt,pkt_len, (void *)req_pkt, pkt_len);

  return ( (void *)rsp_pkt );
} /* nvdiag_write2 */


/*===========================================================================

FUNCTION NVDIAG_SPC
DESCRIPTION
  Processes an entry of the Service Programming Code.  This function reads
  the security code from NV, and compares it to what was sent in the packet.
  If they are the same, the successful SPC entry is noted in this task, and
  a value of TRUE is returned to the external device.
  If they are NOT the same, SP remains locked, a value of false is returned
  to the external device, and a rex_timed_wait is called, to make DIAG pause
  for 10 seconds, to deter hackers who are trying all possible SPC's.

===========================================================================*/
void * nvdiag_spc (
  void * req_pkt,
  uint16 pkt_len
)
{
  DIAG_SPC_F_req_type *req = (DIAG_SPC_F_req_type *)req_pkt;
  DIAG_SPC_F_rsp_type *rsp=NULL;
  const int rsp_len = sizeof(DIAG_SPC_F_rsp_type);

  int i;
  nv_sec_code_type sec_code;
#if defined(FEATURE_DIAG_SPC_TIMEOUT) && defined(FEATURE_DIAG_SPC_TTL)
  static uint8 diag_spc_count = 0;
  uint64 now=0, delta=0;
  static uint64 ts_max_spc_attempt_detect=0;
#endif

  (void) pkt_len; /* suppress compiler warning */
#if defined(FEATURE_DIAG_SPC_TIMEOUT) && defined(FEATURE_DIAG_SPC_TTL)
    /* get the latest timeout count */
	diag_nv_read (NV_DIAG_SPC_UNLOCK_TTL_I, &diag_spc_count);
    if(!diag_spc_count)
    {
       /* first attempt to SPC unlock after bootup */
       now = timetick_get_sclk64();
       /* attempts failed in runtime */
       if(ts_max_spc_attempt_detect <= now) 
	   {
             delta = now - ts_max_spc_attempt_detect;
	   } /* else { // if reached: timetick malfunction or corruption, delta = 0; // avoid SPC unlock } */
       if(NVDIAG_SPC_TIME_OUT <= delta)
       {
           /* 15 or x sec time out passed */
           diag_spc_count = DIAG_SPC_COUNT_MAX;
           ts_max_spc_attempt_detect =0;
		   /* allow retry */
           diag_nv_write (NV_DIAG_SPC_UNLOCK_TTL_I, &diag_spc_count);
       }
       /* no attempts left */
       if(!diag_spc_count) 
       {
	      diag_timeout ();
          return rsp;
       }
    }
#endif /* defined(FEATURE_DIAG_SPC_TIMEOUT) && defined(FEATURE_DIAG_SPC_TTL) */

  rsp = (DIAG_SPC_F_rsp_type *)diagpkt_alloc(DIAG_SPC_F, rsp_len);

  if(!rsp){
    return rsp;
  }
/*------------------------------------------------------------------------*/

#ifndef FEATURE_HWTC
  if ( diag_nv_read (NV_SEC_CODE_I, &sec_code) != NV_DONE_S ) {
    NV_MSG_HIGH("Failed to read Security Code");
    /* Since we couldn't check it, assume it was wrong (but don't make diag
    ** timeout)
    */
    rsp->sec_code_ok = FALSE;
  }
  else
  {
    /* Start out assuming it is ok */
    rsp->sec_code_ok = TRUE;
    (void) diag_set_sp_state (DIAG_SPC_UNLOCKED);

    /* Now check every character in the code, and make sure they match! */
    for (i = 0; i < NV_SEC_CODE_SIZE; i++) {

      /* If a character doesn't match, set the return value to False, and
      ** break out of this loop.
      */
      if (req->sec_code.digits[i] != sec_code.digits[i])
      {
        rsp->sec_code_ok = FALSE;
      }
    }
    if(rsp->sec_code_ok == FALSE)
    {
#if defined(FEATURE_DIAG_SPC_TTL) && defined(FEATURE_DIAG_SPC_TIMEOUT)
	    /* incorrect code provided decrement the count */
        diag_spc_count--;
	if(!diag_spc_count) {
          ts_max_spc_attempt_detect = timetick_get_sclk64();
	}
        diag_nv_write (NV_DIAG_SPC_UNLOCK_TTL_I, &diag_spc_count);
#endif /* defined(FEATURE_DIAG_SPC_TTL) || defined(FEATURE_DIAG_SPC_TIMEOUT) */

        (void) diag_set_sp_state (DIAG_SPC_LOCKED);

        diagpkt_commit (rsp);

#if !defined(FEATURE_DIAG_SPC_TTL) && !defined(FEATURE_DIAG_SPC_TIMEOUT)
        rsp = NULL;
#endif

        diag_timeout ();
    }
	else
	{
#if defined(FEATURE_DIAG_SPC_TIMEOUT) && defined(FEATURE_DIAG_SPC_TTL)
	   /* SPC provided was correct, the count is untouched*/
#endif
	}
  }
#else  /* FEATURE_HWTC */
  #error code not present
#endif /* FEATURE_HWTC */


#if defined(FEATURE_DIAG_SPC_TTL) || defined(FEATURE_DIAG_SPC_TIMEOUT)
   if(rsp->sec_code_ok != TRUE)
      rsp = NULL;
#endif
  return (rsp);

} /* nvdiag_spc */



#ifdef FEATURE_SAVE_SMS
/*===========================================================================

FUNCTION DIAGP_SMS_READ

DESCRIPTION
  This procedure processes a request to read an SMS message.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

============================================================================*/
/* 2 dwords: (confirmation_code, deletion_list), plus NV_SMS_HEADER_SIZE */
#define DIAG_SMS_HDR_SIZE FPOS(nv_sms_type, data)

/* cmd_code plus nv_status plus sms header (static data) */
#define DIAG_SMS_READ_HDR_SIZE (FPOS(nvdiag_sms_read_rsp_type, sms_message) \
                                + DIAG_SMS_HDR_SIZE)

void * nvdiag_sms_read (
  void * req_pkt,
  word pkt_len
)
{
  nvdiag_sms_read_req_type *req = req_pkt;
  nvdiag_sms_read_rsp_type *rsp;
  word rsp_len = sizeof (nvdiag_sms_read_rsp_type);

  rsp = (nvdiag_sms_read_rsp_type *)diagpkt_alloc (DIAG_SMS_READ_F, rsp_len);

  /*-----------------------------------------------------------------------
  Format a command to NV to read the item.
  Will wait for the response.
  -----------------------------------------------------------------------*/
  rsp->sms_message.address = req->address;

  /* Send the nv read request, and report the status of the
  ** nv operation back to dm. */
  rsp->nv_stat =
    /* Cast the sms data type to the nv_item_type because that's what
       the command interface demands, but the structure (sms_message) is
       actually big enough to hold the larger SMS message. */
#ifdef FEATURE_NV_ENUMS_ARE_WORDS
    #error code not present
#else
    (uint16)diag_nv_read(NV_SMS_I,
                (nv_item_type *) &rsp->sms_message);
#endif

  /* If the NV read failed for any reason, make sure we set the length
  ** of the message to 0 before returning */
  if (rsp->nv_stat != NV_DONE_S) {
    rsp->sms_message.length = 0;
  }

  /* size of header of packet plus actual size of SMS data */
  rsp_len = DIAG_SMS_READ_HDR_SIZE + rsp->sms_message.length;

  diagbuf_shorten ((void*) rsp, rsp_len);

  return (rsp);

} /* nvdiag_sms_read */



#ifndef FEATURE_HWTC
/*===========================================================================

FUNCTION NVDIAG_SMS_WRITE

DESCRIPTION
  This procedure processes a request to write an SMS message.  It is
  a variable length command to save on bandwidth, so the buffer to write
  into should only copy the needed bytes.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

============================================================================*/
void * nvdiag_sms_write (
  void * req_pkt,
  word pkt_len
)
{
#if defined(FEATURE_UI_SMS_NV_RESERVATION)
  nv_reservation_type nv_mem_res;
  word nv_raw_bytes;
#endif /* defined(FEATURE_UI_SMS_NV_RESERVATION) */

  nvdiag_sms_write_req_type *req =
    (nvdiag_sms_write_req_type *) req_pkt;
  nvdiag_sms_write_rsp_type *rsp;
  const int rsp_len = sizeof (nvdiag_sms_write_rsp_type);

  /*-----------------------------------------------------------------------
   Start by error-checking the received length - this is a variable length
   packet.
  -----------------------------------------------------------------------*/
  if (pkt_len != FPOS(nvdiag_sms_write_req_type, sms_message) +
                 DIAG_SMS_HDR_SIZE +
                 req->sms_message.length)
  {
    return ( ERR_PKT( DIAG_BAD_LEN_F ) );
  }

  rsp = (nvdiag_sms_write_rsp_type *)diagpkt_alloc (DIAG_SMS_WRITE_F, rsp_len);

  /*-----------------------------------------------------------------------
    To write an SMS message to NV, must first reserve space in the dynamic
    NV memory pool.
  -----------------------------------------------------------------------*/
#if defined(FEATURE_UI_SMS_NV_RESERVATION)
  nv_raw_bytes = nv_sms_sizeof (req->sms_message.length);

  /*--------------------------------------------------------
   If writing a control list (SMS address 0), assume the
   existing SMS messages are no longer valid and delete all
   SMS messages from NV before writing the control list.
  --------------------------------------------------------*/
  if (req->sms_message.address == 0) {
    /*-----------------------------------------------------------------------
     Specifying true indicates allowing partial memory reservation.
     NV will handle case of running out of NV after processing deletion list.
    -----------------------------------------------------------------------*/

    nv_mem_res = nv_reserve_mem ( nv_raw_bytes, TRUE );

    req->sms_message.deletion_list = 0xFFFFFFFF;
  }
  else
  {
    /*-----------------------------------------------------------------------
     Specifying FALSE indicates not allowing partial memory reservation.
     If we can't get all of the memory we need, we must fail.
    -----------------------------------------------------------------------*/

    nv_mem_res = nv_reserve_mem ( nv_raw_bytes, FALSE );

    req->sms_message.deletion_list = 0;

  }

  /* Check if we were able to reserve the memory we need.  Fail otherwise */
  if (nv_mem_res.amount_reserved == 0) {
    rsp->nv_stat = NV_NOMEM_S;
    return (rsp);
  }
  else {
    req->sms_message.confirmation_code =
      nv_mem_res.confirmation_code;
  }

#else
  /*-----------------------------------------------------------------------
   Confirmation code and deletion lists cannot be specified externally.
  -----------------------------------------------------------------------*/
  req->sms_message.confirmation_code = 0;
  req->sms_message.deletion_list = 0;

#endif /* defined(FEATURE_UI_SMS_NV_RESERVATION) */

#ifdef FEATURE_NV_ENUMS_ARE_WORDS
  #error code not present
#else
  rsp->nv_stat = (uint16)diag_nv_write (NV_SMS_I, &req->sms_message);
#endif

  return (rsp);
} /* nvdiag_sms_write */
#endif /* FEATURE_HWTC */
#endif /* FEATURE_SAVE_SMS */



static const diagpkt_user_table_entry_type nvdiag_tbl[] =
{
  {DIAG_NV_READ_F, DIAG_NV_READ_F, nvdiag_read},

  {DIAG_NV_WRITE_F, DIAG_NV_WRITE_F, nvdiag_write},

#ifdef FEATURE_SAVE_SMS

#ifndef FEATURE_HWTC
  {DIAG_SMS_WRITE_F, DIAG_SMS_WRITE_F, nvdiag_sms_write},
#endif

  {DIAG_SMS_READ_F, DIAG_SMS_READ_F, nvdiag_sms_read},
#endif
  {DIAG_SPC_F, DIAG_SPC_F, nvdiag_spc},

};

static const diagpkt_user_table_entry_type nvdiag_subsys_tbl[] =
{
  {DIAG_SUBSYS_NV_WRITE2_F, DIAG_SUBSYS_NV_WRITE2_F, nvdiag_write2},
  {DIAG_SUBSYS_NV_READ_EXT_F, DIAG_SUBSYS_NV_READ_EXT_F, nvdiag_read_ext},
  {DIAG_SUBSYS_NV_WRITE_EXT_F, DIAG_SUBSYS_NV_WRITE_EXT_F, nvdiag_write_ext},  
};

#ifdef __cplusplus
  
  DIAGPKT_DISPATCH_AUTOREGISTER (DIAGPKT_NO_SUBSYS_ID, nvdiag_tbl);

#else

void nvdiag_init (void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAGPKT_NO_SUBSYS_ID, nvdiag_tbl);
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_NV, nvdiag_subsys_tbl);
}

#endif

/* Restore error reporting for these symbols */
/*lint +esym(715,pkt_len,req) */


#ifdef __cplusplus
  }
#endif
