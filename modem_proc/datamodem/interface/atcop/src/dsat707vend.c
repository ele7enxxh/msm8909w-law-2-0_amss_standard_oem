/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

DESCRIPTION
  This module executes AT commands. It executes IS-707 vendor specific
  AT commands.

EXTERNALIZED FUNCTIONS
dsat707_set_mdr_val
  This function writes the MDR value to NV and does any required sync
  in the phone (such as enabling/disabling various service options)

dsat707_exec_qcotc_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCOTC command.

dsat707_exec_qcprev_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCPREV command.
  Returns protocol revision in use.

dsat707_exec_qcmdr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMDR command.
  Set Medium Data Rate.

dsat707_exec_qcscrm_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCSCRM command.
  Enable/Disable mobile from SCRMing.

dsat707_exec_qctrtl_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCTRTL command.
  Enable/Dsiable R_SCH throttling.

dsat707_exec_qcchs_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCCHS command.
  Enable/Dsiable mobile originated control/hold.
  
dsat707_exec_qcqnc_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCQNC command.
  Enable/Dsiable Quick Net Connect.

dsat707_exec_qcso_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCSO command.
  Set Data Service Option number.

dsat707_exec_qcvad_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCVAD command.
  Prearrangement setting. Respond to page message that has a voice service
  option with a page response that has a data service option.

dsat707_exec_qccav_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCCAV command.
  Answer incoming voice call.

dsat707_exec_qcqospri_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCQOSPRI command.
  Set desired value of QOS non-assured priority adjustment.

dsat707_exec_cave_cmd
  This function takes the result from the command line parser and
  executes it. It executes AT^CAVE command. Client sends CAVE authentication
  related parameter (RANDU) to datacard, datacard returns the response. 

dsat707_exec_ssdupdcfm_cmd
  This function takes the result from the command line parser 
  and executes it. It executes AT^SSDUPDCFM command. Client sends 
  updated SSD confirmation (AUTHBS) to datacard.

dsat707_exec_vpm_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT^VPM command. Client asks datacard 
  to generate key.

dsat707_exec_uimauth_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT^UIMAUTH command. Client queries 
  UIM to check the authentication algorithm it supports.

dsat707_exec_gsn_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT^GSN command. Client checks
  datacard/module’s ESN and UIMID. If terminal uses MEID, 
  then it returns MEID, and if terminal uses ESN, then it
  returns ESN.

dsat707_exec_ssdupd_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT^SSDUPD command. Client sends RANDSSD
  to datacard, datacard calculates and reports result.

dsat707_exec_md5_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT^MD5 command. 
  It Generates the CHAP Response for the received CHAP Challenge in 
  HRDP authentication. 


dsat707_exec_hdrcsq_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT^HDRCSQ command. It provides
  the intensity level of current hdr_rssi.
  
dsat707_exec_reset_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT^RESET command. This command
  reboots the device.

  Copyright (c) 2002 - 2014 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MM_DATA/vcs/dsat707vend.c_v   1.14   23 Jan 2003 16:40:12   randrew  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707vend.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   sc      Added support for Dynamic ATCoP.
05/30/14   sc      Fixed ATCoP KW errors.
05/23/14   pg      Fixed LLVM warnings.
04/23/14   sc      Fixed +CFUN issue.
04/17/14   sc      Fixed ATCoP KW errors.
03/11/14   tk/sc   Optimized debug macros usage in ATCoP.
08/30/13   pg      Fixed predictable random number generation issue.
01/04/13   tk      ATCoP changes for DSDX C/G+G with True World Mode Support.
11/30/12   tk      ATCoP changes for Triton DSDA.
08/22/12   sk      Mainlining Nikel Changes.
05/18/12   tk      Migrated to MSG 2.0 macros
09/27/11   ht      Included dsut header to support offtarget stubs.
07/05/11   nc      Added support for ^MODE,^CARDMODE,^SYSCONFIG,^SYSINFO,^DSCI.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/06/10   ad      Added Error msg before Assert(0).
06/04/10   bs      MMGSDI SESSION Migration.
05/10/10   kk      Added support for $$GPS* and $SP* commands.
05/10/10   kk      Added support for ^RESET.
05/10/10   kk      Added support for $QCGSN and $SPMSID.
05/10/10   kk      Added ^SYSINFO, ^HDRCSQ commands.
11/16/09   ca      Added support for the MMGSDI authentication
                   interface commands.
12/12/08   ua      Fixed Off target lint errors.
05/05/03   jd      Removed dependencies on dsiface.h, AT$QCMDR and 
                   AT$QCQOSPRI are set directly
03/05/03   rsl     Fixed at$qcvad.
01/20/03   ar      Added dialstr_modifiers to ds707_async_dial_str_cb 
                   interface.
11/21/02   rsl     Fixed merge issues with at$qcchv.
11/14/02   jd      featurized dsat707mip.h inclusion, cleanup
10/15/02   ak      Added exec_qcchs(), which is for control/hold.
09/30/02   ar      Accomodate changed interface to proc_dial_string
09/24/02   atp     Added support for QOS. (1x Release-A feature).
09/10/02   ak      Fixed bugs in exec_qcscrm() where at$qcscrm? was failing.
09/03/02   ak      In set_mdr_val, no longer write to NV.
07/12/02   ak      Function dsat707_set_mdr_val made available.  Called by
                   dsiface_set_opt.
07/03/02   ak      For QCMDR, QCQNC, QCSCRM, QCTRTL, call dsatpar_exec_param_
                   _cmd, so that input is filtered by common function.
04/03/02   rsl     Initial version of file.

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>

#ifdef FEATURE_DATA_IS707
#include "dsati.h"
#include "dsatctab.h"
#include "dsat707vend.h"
#include "dsat707vendctab.h"
#include "dsat707util.h"
#include "dsat707mipctab.h"
#include "ds_1x_profile.h"
#include "dstaski.h"
#include "ds707.h"
#include "ds707_so_pkt.h"
#include "ds707_so_async.h"
#include "ds707_so_g3fax.h"
#include "dsatparm.h"
#include "mc.h"
#include "msg.h"
#include "dsatact.h"
#include "ds707_async_mgr.h"
#include "dsatvoice.h"
#include "dsrlp_v.h"
#include "db.h"
#include "cm.h"
#include "dsatcmif.h"
#include "dsat_v.h"
#include "dsatme.h"
#include "dstask.h"
#include "dsat707ext.h"
#include "ds707_pkt_mgr.h"
#include "dsatetsipkt.h"
#include "dsatetsictab.h"

#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

#ifdef FEATURE_DSAT_MMGSDI_AUTH
#include "ps_utils.h"
#include "ds3gmmgsdiif.h"
#endif /* FEATURE_DSAT_MMGSDI_AUTH */

#ifdef FEATURE_DS_MOBILE_IP
#include "dsat707mip.h"
#endif /* FEATURE_DS_MOBILE_IP */

#if defined(FEATURE_DSAT_SP_CMDS)
#include "hw.h"
#include "prl_v.h"
#endif /* defined(FEATURE_DSAT_SP_CMDS) */

#if defined(FEATURE_DSAT_GPS_CMDS)
#include "pdapi.h"
#endif /* defined(FEATURE_DSAT_GPS_CMDS) */

#include "ds_profile.h"

/*===========================================================================

                           GLOBAL DATA

===========================================================================*/
#ifdef FEATURE_DSAT_MMGSDI_AUTH
dsat_mmgsdi_auth_e_type dsat_mmgsdi_auth_cmd_pending;
dword randbs_val;
qword dsat_randssd;    /* 56 bits from BS for SSD Generation */
dsat707_md5_s_type dsat707_md5_val = {0};
#endif   /*FEATURE_DSAT_MMGSDI_AUTH*/
#if defined(FEATURE_DSAT_SP_CMDS)
/* For SPSERVICE command*/
dsat_spservice_e_type dsat_spservice;
#endif /* defined(FEATURE_DSAT_SP_CMDS) */

/*===========================================================================
                                 LOCAL DATA
===========================================================================*/
#ifdef FEATURE_CDMA
#define DSAT707_NV_IMSI_S_LENGTH 11
#endif /* FEATURE_CDMA */

#define UNUSED_ARG(_x)        (void)(_x)



#if defined(FEATURE_DSAT_GPS_CMDS)
/* GPS related definitions */
/* All the possible events we might be interested in */
#define DSAT_PD_EVENTS (            \
  PDSM_PD_EVENT_BEGIN             | \
  PDSM_PD_EVENT_POSITION          | \
  PDSM_PD_EVENT_UPDATE_FAILURE    | \
  PDSM_PD_EVENT_END               | \
  PDSM_PD_EVENT_DONE              | \
  PDSM_PD_EVENT_DLOAD_BEGIN       | \
  PDSM_PD_EVENT_DLOAD             | \
  PDSM_PD_EVENT_DLOAD_DONE        | \
  PDSM_PD_DLOAD_EVENT_END )

/* List of all delete param values */
#define DSAT_PDSM_PA_DELETE_ALL (                    \
                            PDSM_PA_DELETE_EPH     | \
                            PDSM_PA_DELETE_ALM     | \
                            PDSM_PA_DELETE_POS     | \
                            PDSM_PA_DELETE_TIME    | \
                            PDSM_PA_DELETE_IONO    | \
                            PDSM_PA_DELETE_UTC     | \
                            PDSM_PA_DELETE_HEALTH  | \
                            PDSM_PA_DELETE_SVDIR   | \
                            PDSM_PA_DELETE_SVSTEER | \
                            PDSM_PA_DELETE_SADATA  | \
                            PDSM_PA_DELETE_RTI     | \
                            PDSM_PA_DELETE_CELLDB_INFO )
                            
LOCAL pdsm_pd_option_s_type   dsat_pd_option;
LOCAL pdsm_pd_info_s_type     dsat_pd_info;
LOCAL pdsm_pd_event_type      dsat_pd_event;
LOCAL pdsm_pd_qos_type        dsat_pd_qos;
LOCAL pdsm_client_id_type     dsat_pd_client_id = -1;
#endif /* defined(FEATURE_DSAT_GPS_CMDS) */

/*===========================================================================
                  INTERNAL FUNCTION FORWARD DECLARATIONS
===========================================================================*/
#if defined(FEATURE_DSAT_GPS_CMDS)
LOCAL void dsat_pd_event_cb ( void*, pdsm_pd_event_type, 
  const pdsm_pd_info_s_type* );
LOCAL dsat_result_enum_type dsat_pd_event_handler(ds_cmd_type         * cmd_ptr   ); /* DS Command pointer         */
LOCAL void dsat_pd_cmd_cb ( void *, pdsm_pd_cmd_e_type,
  pdsm_pd_cmd_err_e_type );
LOCAL void dsat_pa_cmd_cb ( void *, pdsm_pa_cmd_e_type,
  pdsm_pa_cmd_err_e_type );
LOCAL dsat_result_enum_type dsat_gps_init( void );
#endif /* defined(FEATURE_DSAT_GPS_CMDS) */

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

#ifdef FEATURE_DATA_IS707_ASYNC  
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCOTC_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCOTC command.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcotc_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,        /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *rb_ptr                   /*  Place to put response       */
)
{
  /*lint -esym(715,rb_ptr)*/
  /*lint -esym(715,tab_entry)*/
  /*lint -esym(715,tok_ptr)*/
  dsat_result_enum_type result = DSAT_OK;
  boolean digit_mode;
  dsat_dial_val_e_type dialstr_res;
  dsat_dial_modifier_info_type  dialstr_modifiers;
  /*-----------------------------------------------------------------*/
  if (mode == DSAT_CMD)
  {
    dialstr_res = dsat_proc_dial_str ((const byte *)dsatutil_get_val(
                               DSAT707_VEND_ACT_QCOTC_IDX,0,0,STR_TYPE),
                                       ( byte *)dsatutil_get_val(
                              DSAT707_VEND_ACT_QCOTC_IDX,0,0,STR_TYPE));
    memset( (void*)&dialstr_modifiers, 0, sizeof(dialstr_modifiers) );
    
    switch (dialstr_res)
    {
      case DSAT_DIAL_DIGIT:
      case DSAT_DIAL_EMPTY:         /*  OK to originate without address (?) */
        digit_mode = 0;
        /*lint -save -e534*/
        ds707_async_dial_str_cb ( (const byte *)dsatutil_get_val(
                              DSAT707_VEND_ACT_QCOTC_IDX,0,0,STR_TYPE),
                                  digit_mode,
                                  &dialstr_modifiers,
                                  dsat_get_current_1x_subs_id());
        /*lint -restore */
        result = DSAT_NO_RSP;
        break;

      case DSAT_DIAL_ASCII:
        digit_mode = 1;
        /*lint -save -e534*/
        ds707_async_dial_str_cb ( (const byte *)dsatutil_get_val(
                              DSAT707_VEND_ACT_QCOTC_IDX,0,0,STR_TYPE),
                              digit_mode,
                              &dialstr_modifiers,
                              dsat_get_current_1x_subs_id());
        /*lint -restore */
        result = DSAT_NO_RSP;
        break;

      case DSAT_DIAL_ERROR:
      default:
        result = DSAT_ERROR;
        break;
    }
  }
  return result;
} /* dsat707_exec_qcotc_cmd */
#endif /* FEATURE_DATA_IS707_ASYNC */


/*===========================================================================
  FUNCTION DSAT707_EXEC_QCPREV_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCPREV command.
  Returns protocol revision in use.
  1 - JSTD008
  3 - IS_95A
  4 - IS_95B
  6 - IS2000

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcprev_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,         /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{
  /*lint -esym(715,rb_ptr)*/ 
  /*lint -esym(715,tab_entry)*/ 
  /*lint -esym(715,tok_ptr)*/ 
  /*lint -esym(715,mode)*/ 
  byte *rb_ptr = res_buff_ptr->data_ptr;
/*-------------------------------------------------------------------------*/
  rb_ptr = dsatutil_itoa( mc_get_p_rev_in_use(), rb_ptr, 10);
  res_buff_ptr->used = (word)strlen((char *)res_buff_ptr->data_ptr);
  return(DSAT_OK);
} /* dsat707_exec_qcprev_cmd */


#ifdef FEATURE_DS_AT_TEST_ONLY
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMDR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMDR command.
  Set Medium Data Rate.
  0: MDR service only
  1: MDR service if available
  2: LSPD only
  3: SO 33, if available

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmdr_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,         /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *rb_ptr                    /*  Place to put response       */
)
{
  dsat_num_item_type    temp_val = DS_MDR_MODE_DEFAULT;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (tok_ptr->op == (NA|EQ|AR) || (tok_ptr->op == NA)) 
  {
    if(FALSE == DSAT_VALIDATE_SET_707_PKTMGR(DS707_MDR_VAL,DS_MDR_MODE_MAX - 1 ) ||
       FALSE == ds707_pkt_mgr_set_mdr_val_and_so_pkt_recal((dsat_num_item_type)temp_val))
    {
      DS_AT_MSG1_ERROR("$QCMDR: Invalid Number of arguements  %d", tok_ptr->args_found);
      return DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                           rb_ptr->size,
                                          "$QCMDR: (0 - %d)\n",DS_MDR_MODE_MAX - 1);
  }
  else  if(tok_ptr->op == (NA|QU))
  {
    ds707_pkt_mgr_get_1x_profile_val(DS707_MDR_VAL,(void *)&temp_val,0);
    rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                           rb_ptr->size,
                                           "$QCMDR: %d\n",
                                           temp_val);
  }
  else
  {
    /* Not Supported */
    return DSAT_ERROR;
  }
  return DSAT_OK;
} /* dsat707_exec_qcmdr_cmd */


#ifdef FEATURE_IS2000_R_SCH
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCSCRM_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCSCRM command.
  Enable/Disable mobile from SCRMing.
  0 - Mobile never SCRMs
  1 - Mobile can SCRM as needed.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcscrm_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_num_item_type    temp_val = DS_SCRM_DEFAULT;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (tok_ptr->op == (NA|EQ|AR) || (tok_ptr->op == NA)) 
  {
    if(FALSE == DSAT_VALIDATE_SET_707_PKTMGR(DS707_QCSCRM_VAL,1 ) )
    {
      DS_AT_MSG1_ERROR("$QCSCRM: Invalid Number of arguements  %d", tok_ptr->args_found);
      return DSAT_ERROR;
    }
    /*-----------------------------------------------------------------------
      Store the new SCRM Mode setting in NV
    -----------------------------------------------------------------------*/
    /*lint -save -e734 -e534*/
    ds_nv_item.data_scrm_enabled = temp_val;
    dsatutil_put_nv_item(NV_DATA_SCRM_ENABLED_I, &ds_nv_item);
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                           rb_ptr->size,
                                          "$QCSCRM: (0 - 1)\n");
  }
  else  if(tok_ptr->op == (NA|QU))
  {
     ds707_pkt_mgr_get_1x_profile_val(DS707_QCSCRM_VAL,(void *)&temp_val,0);
     rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                            rb_ptr->size,
                                            "$QCSCRM: %d\n",
                                            temp_val);
  }
  else
  {
    /* Not Supported */
    return DSAT_ERROR;
  }
  return DSAT_OK;
} /* dsat707_exec_qcscrm_cmd */


/*===========================================================================
  FUNCTION DSAT707_EXEC_QCTRTL_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCTRTL command.
  Enable/Dsiable R_SCH throttling.
  0 - Mobile never throttles R-SCH
  1 - Mobile can throttle R-SCH as needed.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qctrtl_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_num_item_type    temp_val = DS_TRTL_DEFAULT;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (tok_ptr->op == (NA|EQ|AR) || (tok_ptr->op == NA)) 
  {
    if(FALSE == DSAT_VALIDATE_SET_707_PKTMGR(DS707_QCTRTL_VAL,1 ) )
  {
      DS_AT_MSG1_ERROR("$QCTRTL: Invalid Number of arguements  %d", tok_ptr->args_found);
      return DSAT_ERROR;
    }
    /*-----------------------------------------------------------------------
      Store the new TRTL Mode setting in NV
    -----------------------------------------------------------------------*/
    /*lint -save -e734 -e534*/
    ds_nv_item.data_trtl_enabled = (dsat_num_item_type)temp_val;
    dsatutil_put_nv_item(NV_DATA_TRTL_ENABLED_I, &ds_nv_item);
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                           rb_ptr->size,
                                          "$QCTRTL: (0 - 1)\n");
  }
  else  if(tok_ptr->op == (NA|QU))
  {
     ds707_pkt_mgr_get_1x_profile_val(DS707_QCTRTL_VAL,(void *)&temp_val,0);
     rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                            rb_ptr->size,
                                            "$QCTRTL: %d\n",
                                            temp_val);
  }
  else
  {
    /* Not Supported */
    return DSAT_ERROR;
  }
  return DSAT_OK;
} /* dsat707_exec_qctrtl_cmd */
#endif /* FEATURE_IS2000_R_SCH */
#endif /* FEATURE_DS_AT_TEST_ONLY */


#ifdef FEATURE_IS2000_CHS
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCQNC_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCCHS command.
  Enable/Dsiable mobile originated control/hold.
  0 - Disable mobile-originated control/hold.
  1-255 - Enable mobile-originated control-hold.  Mobile waits for this many
          idle (both tx and rx) 20-msec frames before asking BS for
          control/hold.  

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcchs_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_num_item_type    temp_val   = DS_CHS_DEFAULT;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (tok_ptr->op == (NA|EQ|AR) || (tok_ptr->op == NA)) 
  {
    if(FALSE == DSAT_VALIDATE_SET_707_PKTMGR(DS707_QCCHS_VAL,255 ) )
    {
      DS_AT_MSG1_ERROR("$QCCHS: Invalid Number of arguements  %d", tok_ptr->args_found);
      return DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                           rb_ptr->size,
                                          "$QCCHS: (0 - 255)\n");
  }
  else  if(tok_ptr->op == (NA|QU))
  {
     ds707_pkt_mgr_get_1x_profile_val(DS707_QCCHS_VAL,(void *)&temp_val,0);
     rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                            rb_ptr->size,
                                            "$QCCHS: %d\n",
                                            temp_val);
  }
  else
  {
    /* Not Supported */
    return DSAT_ERROR;
  }
  return DSAT_OK;
} /* dsat707_exec_qcchs_cmd */
#endif /* FEATURE_IS2000_CHS */

#ifdef FEATURE_DS_QNC
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCQNC_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCQNC command.
  Enable/Dsiable Quick Net Connect.
  0 - Disable QNC capability
  1 - Enable QNC capability

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcqnc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_num_item_type    temp_val = DS_QNC_DEFAULT;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (tok_ptr->op == (NA|EQ|AR) || (tok_ptr->op == NA)) 
  {
    if(FALSE == DSAT_VALIDATE_SET_707_PKTMGR(DS707_QCQNC_VAL,1))
    {
      DS_AT_MSG1_ERROR("$QCQNC: Invalid Number of arguements  %d", tok_ptr->args_found);
      return DSAT_ERROR;
    }
    ds_nv_item.data_qnc_enabled = (dsat_num_item_type)temp_val;
    dsatutil_put_nv_item( NV_DATA_QNC_ENABLED_I, &ds_nv_item );
    /*lint -restore */
    /*-----------------------------------------------------------------------
     Set service options according to values read from NV
    -----------------------------------------------------------------------*/
     ds707_so_pkt_recal();
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                           rb_ptr->size,
                                          "$QCQNC: (0 - 1)\n");
  }
  else  if(tok_ptr->op == (NA|QU))
  {
     ds707_pkt_mgr_get_1x_profile_val(DS707_QCQNC_VAL,(void *)&temp_val,0);
     rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                            rb_ptr->size,
                                            "$QCQNC: %d\n",
                                            temp_val);
  }
  else
  {
    /* Not Supported */
    return DSAT_ERROR;
  }
  return DSAT_OK;
} /* dsat707_exec_qcqnc_cmd */
#endif /* FEATURE_DS_QNC */
#ifdef FEATURE_DS_CHOOSE_SO
#ifdef FEATURE_DS_AT_TEST_ONLY
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCSO_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCSO command.
  Set Data Service Option number.
  0 - pre-707 numbers
  1 - proprietary 707 numbers
  2 - IS-707 SO numbers

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcso_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:             */
  const dsati_cmd_type *tab_entry,        /*  Ptr to cmd in parse table    */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser   */
  dsm_item_type *rb_ptr                   /*  Place to put response        */
)
{
  dsat_num_item_type    temp_val = DS_SO_SET_DEFAULT;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (tok_ptr->op == (NA|EQ|AR) || (tok_ptr->op == NA)) 
  {
    if(FALSE == DSAT_VALIDATE_SET_707_PKTMGR(DS707_QCSO_VAL,DS_SO_SET_COUNT-1 ))
  {
      DS_AT_MSG1_ERROR("$QCSO: Invalid Number of arguements  %d", tok_ptr->args_found);
      return DSAT_ERROR;
    }
    /*-----------------------------------------------------------------------
      Store the new SO Mode setting in NV
    -----------------------------------------------------------------------*/
    /*lint -save -e734 -e534*/
    ds_nv_item.data_so_set =(dsat_num_item_type)temp_val;
    dsatutil_put_nv_item( NV_DATA_SO_SET_I, &ds_nv_item );
    /*lint -restore */
    /*-----------------------------------------------------------------------
      Enable/Disable the appropriate data service options in SNM based
      on the service option set now in use
    -----------------------------------------------------------------------*/
    ds707_so_pkt_recal();

#ifdef FEATURE_DATA_IS707_ASYNC    
    ds707_so_async_recal();
    ds707_so_g3fax_recal();
#endif /* FEATURE_DATA_IS707_ASYNC */
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                           rb_ptr->size,
                                          "$QCSO: (0 - %d)\n",DS_SO_SET_COUNT-1);
  }
  else  if(tok_ptr->op == (NA|QU))
  {
     ds707_pkt_mgr_get_1x_profile_val(DS707_QCSO_VAL,(void *)&temp_val,0);
     rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                            rb_ptr->size,
                                            "$QCSO: %d\n",
                                            temp_val);
  }
  else
  {
    /* Not Supported */
    return DSAT_ERROR;
  }
  return DSAT_OK;
} /* dsat707_exec_qcso_cmd */
#endif /* FEATURE_DS_AT_TEST_ONLY */
#endif /* FEATURE_DS_CHOOSE_SO */



/*===========================================================================
  FUNCTION DSAT707_EXEC_QCVAD_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCVAD command.
  Prearrangement setting. Respond to page message that has a voice service
  option with a page response that has a data service option.
  0 - Off
  1 - Fax for next call
  2 - Fax for all calls
  3 - Async. for next call
  4 - Async. for all calls

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcvad_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
  db_items_value_type   dbi;
  dsat_num_item_type    temp_val; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (tok_ptr->op == (NA | QU)) 
  {
    db_get (DB_VOICE_AS_DATA, &dbi);
    DSATUTIL_SET_VAL(DSAT707_VEND_PARA_QCVAD_IDX,0,0,0,dbi.voice_as_data,NUM_TYPE)
  }

  result = dsatparm_exec_param_cmd(mode,
                                   tab_entry,
                                   tok_ptr,
                                   rb_ptr);

  if ((result == DSAT_OK) &&
     ((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
  {
    /*-----------------------------------------------------------------------
     The call manager does the work of changing the VAD setting.
     Note, the cast of the qcvad_val setting is OK because the
     CM type gets its values from the DB type, qcvad_val gets
     its value from the DB.
    -----------------------------------------------------------------------*/
    temp_val = (dsat_num_item_type) dsatutil_get_val(DSAT707_VEND_PARA_QCVAD_IDX,0,0,NUM_TYPE);
    /*lint -save -e534*/
    cm_ph_cmd_answer_voice(NULL,
                           NULL,
                           dsatcm_client_id,
                           (cm_answer_voice_e_type)temp_val,
                           600
                           );
    /*lint -restore */
  }
  return result;
} /* dsat707_exec_qcvad_cmd */



/*===========================================================================
  FUNCTION DSAT707_EXEC_QCCAV_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCCAV command.
  Answer incoming voice call.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qccav_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type  result       = DSAT_OK;
  sys_modem_as_id_e_type subs_id      = dsat_get_current_1x_subs_id();
  dsati_mode_e_type      current_mode = dsatcmdp_get_current_mode();

  /*-----------------------------------------------------------------*/
  if (IS_CDMA_MODE(current_mode) && dsatvoice_is_voice(subs_id))
  {
    result = dsatvoice_answer_cmd(subs_id);
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsat707_exec_qccav_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCCHV_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCCHV command.
  Hangs up incoming voice call.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcchv_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type  result       = DSAT_OK;
  sys_modem_as_id_e_type subs_id      = dsat_get_current_1x_subs_id();
  dsati_mode_e_type      current_mode = dsatcmdp_get_current_mode();

  /*-----------------------------------------------------------------*/
  if(IS_CDMA_MODE(current_mode) && dsatvoice_is_voice(subs_id))
  {
    result = dsatvoice_end_voice_call(subs_id);
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsat707_exec_qcchv_cmd */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCQOSPRI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCQOSPRI command.
  Sets desired value of QOS non-assured priority adjustment.
  (Can range from 0 to DS707_QOS_DESIRED_PRI_MAX (0xF).)

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcqospri_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,         /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *rb_ptr                    /*  Place to put response       */
)
{
  dsat_num_item_type    temp_val   = DS707_QOS_DESIRED_PRI_DEFAULT;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (tok_ptr->op == (NA|EQ|AR) || (tok_ptr->op == NA)) 
  {
    if(FALSE == DSAT_VALIDATE_SET_707_PKTMGR(DS707_QCQOSPRI_VAL,DS707_QOS_DESIRED_PRI_MAX ) )
  {
      DS_AT_MSG1_ERROR("$QCQOSPRI: Invalid Number of arguements  %d", tok_ptr->args_found);
      return DSAT_ERROR;
    }
    /*-----------------------------------------------------------------------
      assume range checking is done beforehand
      ds707_qcqospri_val must be between
      DS707_QOS_DESIRED_PRI_MIN .. DS707_QOS_DESIRED_PRI_MAX
    -----------------------------------------------------------------------*/
    dsrlp_set_desired_qos_non_assur_pri( (byte) temp_val );
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                           rb_ptr->size,
                                          "$QCQOSPRI: (0 - %d)\n",DS707_QOS_DESIRED_PRI_MAX);
  }
  else  if(tok_ptr->op == (NA|QU))
  {
     ds707_pkt_mgr_get_1x_profile_val(DS707_QCQOSPRI_VAL,(void *)&temp_val,0);
     rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                            rb_ptr->size,
                                            "$QCQOSPRI: %d\n",
                                            temp_val);
  }
  else
  {
    /* Not Supported */
    return DSAT_ERROR;
  }
  return DSAT_OK;
} /* dsat707_exec_qcqospri_cmd */
#endif /* FEATURE_IS2000_REL_A    */

#ifdef  FEATURE_DSAT_MMGSDI_AUTH
/*===========================================================================
  FUNCTION dsatme_mmgsdi_auth_event_handler

  DESCRIPTION
  This is the event handler function provided while calling MMGSDI 
  authentication interface commands.
   
  DEPENDENCIES
  None.
  
  RETURN VALUE
  Boolean indicating success/fail.
  
  SIDE EFFECTS
  None.
===========================================================================*/

dsat_result_enum_type dsat707_mmgsdi_auth_event_handler
(
  ds_cmd_type         *cmd_ptr              /* DS Command pointer         */
)
{
#define DSAT_MMGSDI_SW_CHECK()\
{\
  if ( (mmgsdi_auth_cnf->mmgsdi_status== MMGSDI_SUCCESS) &&\
       (mmgsdi_auth_cnf->status_word.sw1 == 0x0090) && \
       (mmgsdi_auth_cnf->status_word.sw2 == 0x0000) )\
  {\
    dsat_check_result = TRUE;\
  }\
  else\
  {\
    dsat_check_result = FALSE;\
  }\
}
  dsm_item_type                * rb_ptr             = NULL;
  dsat_result_enum_type         result              = DSAT_ASYNC_EVENT;
  cb_cmd_e_type                 client_ref          = CB_NONE;
  boolean                       dsat_check_result   = TRUE;
  dsat707_mmgsdi_auth_cmd_type* mmgsdi_auth_cnf     = dsat_get_cmd_payload_ptr(cmd_ptr);

  /* Ignore processing when the command entered is not a CDMA one */
  if(!dsatcmdp_is_cdma_cmd())
  {
    return DSAT_ASYNC_EVENT;
  }

  DS_AT_MSG5_HIGH("In DSAT707 MMGSDI AUTH EVENT HANDLER for event= %d status %d ",
                  "Command Pending %d SW1 = %d SW2 = %d",
                  mmgsdi_auth_cnf->client_ref,
                  mmgsdi_auth_cnf->mmgsdi_status,
                  dsat_mmgsdi_auth_cmd_pending,
                  mmgsdi_auth_cnf->status_word.sw1,
                  mmgsdi_auth_cnf->status_word.sw2);

  client_ref = (cb_cmd_e_type)mmgsdi_auth_cnf->client_ref;

  /* Verify SW and status  */
  DSAT_MMGSDI_SW_CHECK();

  if ( (dsat_check_result) || 
       ( DSAT_MMGSDI_AUTH_UIMAUTH == dsat_mmgsdi_auth_cmd_pending ) )
  {
    switch (client_ref)
    {
      case SSDUDPCFM_MMGSDI_CFM_SSD:
      {
        if ( TRUE == mmgsdi_auth_cnf->auth_cmd.ssd_update_ok )
        {
          result = DSAT_OK;
        }
        else
        {
          DS_AT_MSG0_ERROR("SSD UPD CFM failed");
          result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
        }
      }
        break;/*SSDUDPCFM_MMGSDI_CFM_SSD*/
  
      case SSD_MMGSDI_BSCHAL_SSD:
      {
        nv_stat_enum_type nv_status;
        dword esn;
        
        dsat_mmgsdi_auth_cmd_pending = DSAT_MMGSDI_AUTH_SSD_UPDATE;
        randbs_val = mmgsdi_auth_cnf->auth_cmd.randbs;

        /* ESN Reporting */
        nv_status = dsatutil_get_nv_item (NV_ESN_I, &ds_nv_item);
        if( nv_status == NV_DONE_S )
        {
          esn = ds_nv_item.esn.esn;
          if ( MMGSDI_SUCCESS == 
                         mmgsdi_session_ssd_update ( 
                                     mmgsdi_auth_cnf->session_id,
                                     dsat_randssd,
                                     DSAT_PROCESS_CONTROL_SSD,
                                     esn,
                                     dsat707_mmgsdi_auth_cb,
            (mmgsdi_client_data_type)SSDUDP_MMGSDI_RAND_SSD ))
          {
            DS_AT_MSG0_MED("SSD update request in progress.");
            result = DSAT_ASYNC_CMD;
          }
          else
          {
            DS_AT_MSG0_ERROR("SSD update unsuccessful");
            result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
          }
        }
        else
        {
          result = DSAT_ERROR;
        }
      }
        break;/*SSD_MMGSDI_BSCHAL_SSD*/
  
      case SSDUDP_MMGSDI_RAND_SSD:
      {
        /* BS CHAL response     randbs         4 bytes  */
        rb_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
        /* Format the response */
        rb_ptr->used = (word)snprintf((char*)rb_ptr->data_ptr,
                                            rb_ptr->size,
                                            "%s: %02X%02X%02X%02X",
                                            "^SSDUPD",
                                           (uint8)(randbs_val>>24 & 0xFF),
                                           (uint8)(randbs_val>>16 & 0xFF), 
                                           (uint8)(randbs_val>>8  & 0xFF), 
                                           (uint8)(randbs_val     & 0xFF));
        dsatcmdp_handle_async_cmd_rsp (rb_ptr, DSAT_COMPLETE_RSP);
        result = DSAT_OK;
      }
        break;/*SSDUDP_MMGSDI_RAND_SSD*/

      case CAVE_MMGSDI_AUTH_RANDU:
      {
        dword  authu_val;
        uint16  authu_len;
        authu_len = 
               strlen((char *)&mmgsdi_auth_cnf->auth_cmd.authu);
        if ( authu_len == DSAT_MAX_AUTHU_LEN) 
        {
          rb_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
          authu_val = mmgsdi_auth_cnf->auth_cmd.run_cave_response_data;
          /* Format the response */
          rb_ptr->used = (word)snprintf((char*)rb_ptr->data_ptr,
                                              rb_ptr->size,
                                              "%s: %02X%02X%02X",
                                              "^CAVE",
                                             (uint8)(authu_val>>16 & 0xFF),
                                             (uint8)(authu_val>>8  & 0xFF),
                                             (uint8)(authu_val     & 0xFF));
          dsatcmdp_handle_async_cmd_rsp (rb_ptr, DSAT_COMPLETE_RSP);
          result = DSAT_OK;
        }
        else
        {
          DS_AT_MSG1_ERROR("Invalid length of AUTHU %d is received",authu_len);
          result = dsat_send_cme_error(DSAT_CME_AUTH_PARAM_LENGTH_ERROR);
        }
      }
        break;/*CAVE_MMGSDI_AUTH_RANDU*/

      case VPM_MMGSDI_KEY_GEN:
      {
        int32 vpm_len =  mmgsdi_auth_cnf->auth_cmd.vpm_cnf.data_len;
        if ( vpm_len != DSAT_MAX_VPM_LEN)
        {
          DS_AT_MSG1_ERROR("Invalid length of VPM KEY %d is received",vpm_len);
          result = dsat_send_cme_error(DSAT_CME_AUTH_PARAM_LENGTH_ERROR);
        }
        else
        {
          uint8 i=0;
          rb_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
          /*---------------------------------------------------------------
                      Format the response :
                      1. SMEKEY
                      2. VPM
          -----------------------------------------------------------------*/
          rb_ptr->used = (word)snprintf((char*)rb_ptr->data_ptr,
                                              rb_ptr->size,
                                              "^VPM: ");
          for(i=0;i<DSAT_MAX_SMEKEY_LEN;++i)
          {
            rb_ptr->used += (word)
                   snprintf((char*)&rb_ptr->data_ptr[rb_ptr->used],
                                  rb_ptr->size - rb_ptr->used,
                                  "%02X",
              (uint8)mmgsdi_auth_cnf->auth_cmd.vpm_cnf.key_data[i]);
          }
          rb_ptr->data_ptr[rb_ptr->used++] = ',';

          for(i=0;i<DSAT_MAX_VPM_LEN;++i)
          {
            rb_ptr->used += (word)
                   snprintf((char*)&rb_ptr->data_ptr[rb_ptr->used],
                                  rb_ptr->size - rb_ptr->used,
                                  "%02X",
               (uint8)(mmgsdi_auth_cnf->auth_cmd.vpm_cnf.data[i]));
          }
          dsatcmdp_handle_async_cmd_rsp (rb_ptr, DSAT_COMPLETE_RSP);
          result = DSAT_OK;
        }
      }
        break;/*VPM_MMGSDI_KEY_GEN*/
  
      case MD5_MMGSDI_IP_AUTH:
      {
        uint8 i;
        dsat_mmgsdi_md5_cnf_s_type *response = 
                   &(mmgsdi_auth_cnf->auth_cmd.md5_cnf);
        if ( response->data_len != 0 )
        {
          rb_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
          rb_ptr->used = (word)snprintf((char*)rb_ptr->data_ptr,
                                                rb_ptr->size,
                                                "^MD5: " );
          DS_AT_MSG0_HIGH("MD5 Response");
          for ( i=0; i< response->data_len;++i)
          {
            rb_ptr->used += (word)snprintf(
                               (char*)&rb_ptr->data_ptr[rb_ptr->used],
                                       rb_ptr->size - rb_ptr->used, 
                                       "%02X",
                                       response->data[i]);
            DS_AT_MSG2_HIGH(" Byte %d -- %d ", i,response->data[i]);
          }
          dsatcmdp_handle_async_cmd_rsp (rb_ptr, DSAT_COMPLETE_RSP);
          result = DSAT_OK;
        }
      }
        break;/*MD5_MMGSDI_IP_AUTH*/
  
      case UIMAUTH_MMGSDI_CAVE_MD5:
      {
        rb_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
        rb_ptr->used = (word)snprintf((char*)rb_ptr->data_ptr,
                                      rb_ptr->size,
                                      "^UIMAUTH: (\"^CAVE\"");

        if ( TRUE == mmgsdi_auth_cnf->auth_cmd.srv_available )
        {
          rb_ptr->used += (word)snprintf(
                                (char*)&rb_ptr->data_ptr[rb_ptr->used],
                                rb_ptr->size - rb_ptr->used,
                                ", \"^MD5\"");
        }
        rb_ptr->data_ptr[rb_ptr->used++] = ')';
        dsatcmdp_handle_async_cmd_rsp (rb_ptr, DSAT_COMPLETE_RSP);
        result = DSAT_OK;
      }
        break;/*UIMAUTH_MMGSDI_CAVE_MD5*/
  
      default:
        result = DSAT_ERROR;
        break;/*DEFAULT*/
    }
  }
  else
  {
    DS_AT_MSG0_ERROR("MMGSDI error or undesired SW1 and SW2 values");
    result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
  }
  if( result != DSAT_ASYNC_CMD )
  {
    dsat_mmgsdi_auth_cmd_pending = DSAT_MMGSDI_AUTH_NONE;
  }
  return result;
} /* dsatme_mmgsdi_auth_event_handler */

/*===========================================================================
  FUNCTION DSAT707_MMGSDI_AUTH_CB

  DESCRIPTION
  This is the callback function provided while calling MMGSDI authentication 
  interface commands.
   
  DEPENDENCIES
  None.
  
  RETURN VALUE
  None.
  
  SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void dsat707_mmgsdi_auth_cb 
(
  mmgsdi_return_enum_type status, 
  mmgsdi_cnf_enum_type    cnf, 
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  ds_cmd_type            * cmd_buf  = NULL;
  dsat707_mmgsdi_auth_cmd_type *mmgsdi_auth_cmd = NULL;

  ASSERT( cnf_ptr != NULL );

  DS_AT_MSG2_HIGH("dsat707_mmgsdi_auth_cb() with client ref %d status %d",
                                                           cnf, status);
  cmd_buf  = dsat_get_cmd_buf(sizeof(dsat707_mmgsdi_auth_cmd_type), FALSE);
  mmgsdi_auth_cmd_ptr = cmd_ptr->cmd_payload_ptr;
  /*---------------------------------------------------------------
     Copy general details
     1. MMGSDI Status
     2. Client ref
     3. Header to post it to DS Task
     4. If SW is valid, copy SW1 and SW2
     5. Session ID
  -----------------------------------------------------------------*/
  cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_MMGSDI_AUTH_CMD;
  mmgsdi_auth_cmd->mmgsdi_status = status;
  mmgsdi_auth_cmd->client_ref    = cnf_ptr->response_header.client_data;
  /* Always copy SW1 and SW2 */
  mmgsdi_auth_cmd->status_word.present = TRUE;
  mmgsdi_auth_cmd->status_word.sw1     = 
               cnf_ptr->response_header.status_word.sw1;
  mmgsdi_auth_cmd->status_word.sw2     = 
               cnf_ptr->response_header.status_word.sw2;
  mmgsdi_auth_cmd->session_id          = 
                       cnf_ptr->response_header.session_id;
  mmgsdi_auth_cmd->mmgsdi_status = status;

  if (  MMGSDI_SUCCESS == status  )
  {
    switch ((cb_cmd_e_type)cnf_ptr->response_header.client_data)
    {
      case SSDUDPCFM_MMGSDI_CFM_SSD:
        if ( ( MMGSDI_SESSION_SSD_CONFIRM_CNF == cnf ) && 
             ( dsat_mmgsdi_auth_cmd_pending == DSAT_MMGSDI_AUTH_SSD_CNF ) )
        {
          /*  Session response */
          mmgsdi_auth_cmd->auth_cmd.ssd_update_ok = 
                       cnf_ptr->session_ssd_confirm_cnf.ssd_update_ok;
        }
        else
        {
          DS_AT_MSG1_ERROR("dsat707_mmgsdi_auth_cb() with status error %d",
                      status);
          ASSERT(0);
        }
        break;

      case SSDUDP_MMGSDI_RAND_SSD:
        if ( (MMGSDI_SESSION_SSD_UPDATE_CNF == cnf) && 
             (dsat_mmgsdi_auth_cmd_pending == DSAT_MMGSDI_AUTH_SSD_UPDATE)) 
        {
          DS_AT_MSG0_HIGH("RAND SSD Update complete BS Chal in progress.");
        }
        else
        {
          DS_AT_MSG1_ERROR("dsat707_mmgsdi_auth_cb() with status error %d",
                      status);
          ASSERT(0);
        }
        break;

      case SSD_MMGSDI_BSCHAL_SSD:
       if (( MMGSDI_SESSION_BS_CHAL_CNF == cnf ) && 
           ( dsat_mmgsdi_auth_cmd_pending == DSAT_MMGSDI_AUTH_SSD_BS_CHAL))
        {
          /* Copy randbs */
          mmgsdi_auth_cmd->auth_cmd.randbs = 
                   cnf_ptr->session_bs_chal_cnf.randbs;
        }
        else
        {
          DS_AT_MSG1_ERROR("dsat707_mmgsdi_auth_cb() with status error %d",
                      status);
          ASSERT(0);
        }
        break;
      case CAVE_MMGSDI_AUTH_RANDU:
        if ( ( MMGSDI_RUN_CAVE_CNF == cnf ) && 
             ( dsat_mmgsdi_auth_cmd_pending == DSAT_MMGSDI_AUTH_CAVE ) )
        {
          /*Copy cave response data*/
          mmgsdi_auth_cmd->auth_cmd.run_cave_response_data = 
                               cnf_ptr->run_cave_cnf.run_cave_response_data;
        }
        else
        {
          DS_AT_MSG1_ERROR("dsat707_mmgsdi_auth_cb() with status error %d",
                      status);
          ASSERT(0);
        }
        break;

      case VPM_MMGSDI_KEY_GEN:
        if ( ( MMGSDI_GENERATE_KEY_VPM_CNF == cnf ) && 
             ( dsat_mmgsdi_auth_cmd_pending == DSAT_MMGSDI_AUTH_KEYGEN_VPM ) )
        {
          const mmgsdi_generate_key_vpm_cnf_type *resp_ptr = 
                              &cnf_ptr->generate_key_vpm_cnf;

          /* key data (smekey) and octet_data*/
         (void) dsatutil_memscpy((void*)mmgsdi_auth_cmd->auth_cmd.vpm_cnf.key_data,
                     MMGSDI_KEY_SIZE,
                    (void*)resp_ptr->key_data,
              sizeof(mmgsdi_auth_cmd->auth_cmd.vpm_cnf.key_data));

          mmgsdi_auth_cmd->auth_cmd.vpm_cnf.data_len = 
            MIN(DSAT_MAX_VPM_DATA, resp_ptr->octet_data.data_len );
          if ( (resp_ptr->octet_data.data_len != 0 ) &&
                (resp_ptr->octet_data.data_ptr != NULL ))
          {
            (void) dsatutil_memscpy((void*)mmgsdi_auth_cmd->auth_cmd.vpm_cnf.data,
                      DSAT_MAX_VPM_DATA,
                     (void*)resp_ptr->octet_data.data_ptr,
                 mmgsdi_auth_cmd->auth_cmd.vpm_cnf.data_len);
          }

        }
        else
        {
          DS_AT_MSG1_ERROR("dsat707_mmgsdi_auth_cb() with status error %d",
                      status);
          ASSERT(0);
        }
        break;

      case MD5_MMGSDI_IP_AUTH:
        if ( ( MMGSDI_COMPUTE_IP_AUTH_CNF == cnf ) &&
             ( DSAT_MMGSDI_AUTH_MD5 == dsat_mmgsdi_auth_cmd_pending ))
        {
         const  mmgsdi_cmpt_ip_response_data_type *resp_ptr = 
             &cnf_ptr->compute_ip_cnf.cmpt_ip_response_data;

          mmgsdi_auth_cmd->auth_cmd.md5_cnf.data_len= 
                              MIN(DSAT_MAX_MD5_RSP,resp_ptr->data_len);
          if ( (resp_ptr->data_len !=0)&&
               (resp_ptr->data_ptr != NULL))
          {
            (void) dsatutil_memscpy((void*)mmgsdi_auth_cmd->auth_cmd.md5_cnf.data,
                    DSAT_MAX_MD5_RSP,
                   (void*)resp_ptr->data_ptr,
                   mmgsdi_auth_cmd->auth_cmd.md5_cnf.data_len);
          }
          DS_AT_MSG1_HIGH("MD5 Data of %d bytes copied", 
                 mmgsdi_auth_cmd->auth_cmd.md5_cnf.data_len);
        }
        else
        {
          DS_AT_MSG1_ERROR("dsat707_mmgsdi_auth_cb() case MD5_MMGSDI_IP_AUTH with conformation %d",
                      cnf);
          ASSERT(0);
        }
        break;

      case UIMAUTH_MMGSDI_CAVE_MD5:
      {
        if ( ( MMGSDI_SRV_AVAILABLE_CNF == cnf ) &&
             ( DSAT_MMGSDI_AUTH_UIMAUTH == dsat_mmgsdi_auth_cmd_pending ) )
        {
          mmgsdi_auth_cmd->auth_cmd.srv_available = 
                             cnf_ptr->srv_available_cnf.srv_available;
        }
      }
        break;

      default:
        DS_AT_MSG1_ERROR("dsat707_mmgsdi_auth_cb() with invalid CNF %d",
                    cnf);
        ds_release_cmd_buf(&cmd_buf); /*release the memory*/
        return;
    }/* Switch */

  }/* MMGSDI SUCCESS */

  ds_put_cmd(cmd_buf);
}/*dsat707_mmgsdi_auth_cb*/

/*===========================================================================
  FUNCTION DSAT707_EXEC_CAVE_CMD

  DESCRIPTION
  This function takes the result from the command line parser and
  executes it. It executes AT^CAVE command. Client sends CAVE authentication
  related parameter <RANDU> to datacard, datacard returns <AUTHU> as 
  the response. 
  
  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  DSAT_ASYNC_CMD : if the command processing is in progress.
    
  SIDE EFFECTS
  None.
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsat707_exec_cave_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr                 /*  Place to put response       */
) 
{
  dsat_result_enum_type result = DSAT_OK;
  mmgsdi_session_id_type session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
  mmgsdi_cave_rand_chal_type randu = 0;
  qword esn ={0x00,0x00};
  mmgsdi_cave_digits_type  digits ={0};
  mmgsdi_cave_rand_type_type rand_type = 
             (mmgsdi_cave_rand_type_type )DSAT_MMGSDI_RANDU_TYPE;
  nv_stat_enum_type nv_status;

  dsat_mmgsdi_auth_cmd_pending = DSAT_MMGSDI_AUTH_NONE;

  /* Process Execute command */
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    /* Check the number of arguments. */
    if (tok_ptr->args_found > 1)
    {
      DS_AT_MSG0_ERROR("Too many arguments");
      return DSAT_ERROR;
    }

    /*RANDU should be 3 bytes of Hex data, ATCoP receives it in 
      ascii so check with 6 bytes AB is 1 byte of hex data but it 
      is received as 2 characters */
    if ( ( VALID_TOKEN(0)) && 
         ( strlen((const char *)tok_ptr->arg[0]) == 
                                    ( DSAT_MAX_RANDU_LEN *2 )))
    {
      /* Get and check session id*/
      if ((ds3g_mmgsdi_get_session_id( MMGSDI_1X_PROV_PRI_SESSION, 
                                                   &session_id ))
                     && (session_id != DS3G_MMGSDI_INVALID_SESSION_ID))
      {
        /*----------------------------------------------------------
           After conversion, data is in bytes but randu is a dword
           so, reverse the byte order in order to send the data 
           as received. 
           Eg: RANDU received: 112233 (3 bytes of data actually ASCII)
           after conversion, it would be 11 22 33 00 in byte format
           but in dword format, it is 33 22 11 00  so, to send it in 
           correct format as received, reverse 11 22 33 00 to be
           33 22 11 00 so this in dword it would be 11 22 33 00  
        ------------------------------------------------------------*/
        if ( (NULL == dsatutil_convert_chset( (const char *)tok_ptr->arg[0], 
                                              ALPHA_HEX,
                                              (const char *)&randu,
                                              ALPHA_8BIT,
                                              sizeof(randu),
                                              FALSE ) ) ||
              ( FALSE == dsatutil_reversen_byte((byte *)&randu,
                                                 strlen((char*)&randu))))
        {
          /* parameter error */
          DS_AT_MSG0_ERROR("Invalid Hex data");
          result = DSAT_ERROR;
        }
        else
        {
                  /* ESN Reporting */
          nv_status = dsatutil_get_nv_item (NV_ESN_I, &ds_nv_item);
          if( nv_status == NV_DONE_S )
          {
            esn[0] = ds_nv_item.esn.esn;
          }
          else
          {
            return DSAT_ERROR;
          }
          dsat_mmgsdi_auth_cmd_pending = DSAT_MMGSDI_AUTH_CAVE;
          if( MMGSDI_SUCCESS == 
                mmgsdi_session_run_cave_algo(  session_id, 
                                               rand_type,
                                               randu, 
                                               DSAT_CAVE_DIG_LEN, 
                                               digits, 
                                               DSAT_PROCESS_CONTROL_CAVE, 
                                               esn, 
                                               dsat707_mmgsdi_auth_cb, 
                      (mmgsdi_client_data_type)CAVE_MMGSDI_AUTH_RANDU ) )
          {
            DS_AT_MSG0_HIGH("CAVE authentication in progress");
            result = DSAT_ASYNC_CMD;
          }
          else
          {
            DS_AT_MSG0_ERROR("CAVE authentication unsuccessful");
            dsat_mmgsdi_auth_cmd_pending = DSAT_MMGSDI_AUTH_NONE;
            result = DSAT_ERROR;
          }
        }
      }
      else
      {
        DS_AT_MSG1_ERROR("DS3G MMGSDI Invalid CDMA Session ID %d", 
               session_id);
        result = DSAT_ERROR;
      }
    }
    else
    {
      result = dsat_send_cme_error(DSAT_CME_AUTH_PARAM_LENGTH_ERROR );
    }
  }
  /*---------------------------------------------------------------------
    Test commands should return OK
  ---------------------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
    result = DSAT_OK;
  }
  else
  {
    DS_AT_MSG0_ERROR("Syntax error");
    result = DSAT_ERROR;
  }
  return result;
}	/* dsat707_exec_cave_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_GSN_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT^GSN command. Client checks
  datacard/module’s ESN/MEID and UIMID.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  DSAT_ASYNC_CMD: if the command is in progress.

  SIDE EFFECTS
  None.
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsat707_exec_gsn_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr                 /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  mmgsdi_file_enum_type file_name =  MMGSDI_NO_FILE_ENUM;

  dsat_mmgsdi_auth_cmd_pending = DSAT_MMGSDI_AUTH_NONE;
  if( tok_ptr->op == NA )
  {
    if(dsat_get_current_1x_app_type() == MMGSDI_APP_RUIM)
    {
      file_name = MMGSDI_CDMA_RUIM_ID;
      DS_AT_MSG0_HIGH("MMGSDI_CDMA_RUIM_ID is used");
    }
    else if(dsat_get_current_1x_app_type() == MMGSDI_APP_CSIM)
    {
      file_name = MMGSDI_CSIM_RUIM_ID;
      DS_AT_MSG0_HIGH("MMGSDI_CSIM_RUIM_ID is used");
    }
    else
    {
      DS_AT_MSG0_ERROR("Invalid SIM or NV mode");
      return DSAT_ERROR;
    }
  
    dsat_mmgsdi_auth_cmd_pending = DSAT_MMGSDI_AUTH_GSN;
    if( MMGSDI_SUCCESS == mmgsdi_session_read_transparent(
                         (dsat_get_current_1x_session_id(), \
                         file_name, 
                         0,
                         0, 
                         dsatme_mmgsdi_read_transparent_cb,
                         (mmgsdi_client_data_type)GSN_MMGSDI_MEID_ESN))
    {
      DS_AT_MSG0_HIGH("Succesfully posted the UIMID read to MMGSDI");
      result = DSAT_ASYNC_CMD;
    }
    else
    {
      dsat_mmgsdi_auth_cmd_pending = DSAT_MMGSDI_AUTH_NONE;
      DS_AT_MSG0_ERROR("MMGSDI rejected the UIMID read request");
      result = DSAT_ERROR;
    }
  }
  /*-------------------------------------------------------
        Test Command
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, and return OK */
    result = DSAT_OK;
  }
  else
  {
    DS_AT_MSG0_ERROR("Invalid syntax");
    result = DSAT_ERROR;
  }
  return result;
}/* dsat707_exec_gsn_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_UIMAUTH_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT^UIMAUTH command. Client queries 
  UIM to check the authentication algorithm it supports.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  DSAT_ASYNC_CMD: if the command is in progress.

  SIDE EFFECTS
  None.
===========================================================================*/
  /*ARGSUSED*/
dsat_result_enum_type dsat707_exec_uimauth_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr                 /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  mmgsdi_session_id_type session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
  mmgsdi_service_enum_type file_name = MMGSDI_NONE;

  if (tok_ptr->op == (NA|EQ|QU))
  {
    if ((ds3g_mmgsdi_get_session_id( MMGSDI_1X_PROV_PRI_SESSION, &session_id ))
                     && (session_id != DS3G_MMGSDI_INVALID_SESSION_ID))
    {
      if(dsat_get_current_1x_app_type() == MMGSDI_APP_RUIM)
      {
        file_name = MMGSDI_CDMA_SRV_HRPD;
        DS_AT_MSG0_HIGH("MMGSDI_CDMA_SRV_HRPD available");
      }
      else if(dsat_get_current_1x_app_type() == MMGSDI_APP_CSIM)
      {
        file_name = MMGSDI_CSIM_SRV_HRPD;
        DS_AT_MSG0_HIGH("MMGSDI_CSIM_SRV_HRPD available");
      }
      else
      {
        DS_AT_MSG0_ERROR("Invalid SIM or NV mode");
        return DSAT_ERROR;
      }
      dsat_mmgsdi_auth_cmd_pending = DSAT_MMGSDI_AUTH_UIMAUTH;
      if ( MMGSDI_SUCCESS == mmgsdi_session_is_service_available ( session_id,
                                                    file_name,
                                                    dsat707_mmgsdi_auth_cb,
                           (mmgsdi_client_data_type)UIMAUTH_MMGSDI_CAVE_MD5 ))
      {
        DS_AT_MSG0_HIGH("Get Service status of HRPD in progress");
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        dsat_mmgsdi_auth_cmd_pending = DSAT_MMGSDI_AUTH_NONE;
        result = DSAT_ERROR;
      }
    }
    else
    {
      DS_AT_MSG0_ERROR("Invalid session ID");
      result = DSAT_ERROR;
    }

  }
  else
  {
    DS_AT_MSG0_ERROR("Invalid operation");
    result = DSAT_ERROR;
  }
  return result;
}/* dsat707_exec_uimauth_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_VPM_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT^VPM command. 
  This generates the VPM Key along with SME KEY

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  DSAT_ASYNC_CMD: if the command is in progress

  SIDE EFFECTS
  None.
===========================================================================*/
  /*ARGSUSED*/
dsat_result_enum_type dsat707_exec_vpm_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr                 /*  Place to put response       */
) 
{
  dsat_result_enum_type result = DSAT_ERROR;
  mmgsdi_session_id_type session_id = DS3G_MMGSDI_INVALID_SESSION_ID;

  dsat_mmgsdi_auth_cmd_pending = DSAT_MMGSDI_AUTH_NONE;

  if( tok_ptr->op == NA )
  {
    /*check session id*/
    if ((ds3g_mmgsdi_get_session_id( MMGSDI_1X_PROV_PRI_SESSION, &session_id ))
                            && (session_id != DS3G_MMGSDI_INVALID_SESSION_ID))
    {
      dsat_mmgsdi_auth_cmd_pending = DSAT_MMGSDI_AUTH_KEYGEN_VPM;
      if(MMGSDI_SUCCESS == 
               mmgsdi_session_generate_key_vpm( session_id,
                                               DSAT_VPM_FIRST_OCTET,
                                               DSAT_VPM_LAST_OCTET,
                                               dsat707_mmgsdi_auth_cb,
                      (mmgsdi_client_data_type)VPM_MMGSDI_KEY_GEN))
      {
        DS_AT_MSG0_HIGH("VPM key generation is in progress");
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        DS_AT_MSG0_ERROR("VPM key generation unsuccessful");
        dsat_mmgsdi_auth_cmd_pending = DSAT_MMGSDI_AUTH_NONE;
        result = DSAT_ERROR;
      }
    }
    else
    {
      DS_AT_MSG1_ERROR("DS3G MMGSDI Invalid CDMA Session ID %d",session_id);
      result = DSAT_ERROR;
    }
  }
  /*-------------------------------------------------------
        Test Command
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, and return OK */
    result = DSAT_OK;
  }
  else
  {
    DS_AT_MSG0_ERROR("Syntax error");
    result = DSAT_ERROR;
  }
  return result;
}/* dsat707_exec_vpm_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_SSDUPD_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT^SSDUPD command. 

  First, BS Chalenge is calculated as <RANDBS> and then SSD Update 
  is sent to CARD. 
  
  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  DSAT_ASYNC_CMD: if the command is in progress.

  SIDE EFFECTS
  None.
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsat707_exec_ssdupd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr                 /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_ERROR;
  mmgsdi_session_id_type session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
  dword rand_seed ;

  dsat_mmgsdi_auth_cmd_pending = DSAT_MMGSDI_AUTH_NONE;

  /* Process Execute command */
  if ( tok_ptr->op == (NA|EQ|AR))
  {
    /* Check the number of arguments. */
    if (tok_ptr->args_found > 1)
    {
      DS_AT_MSG0_ERROR("Too many arguments");
      return DSAT_ERROR;
    }
    /*RANDSSD should be 7 bytes of Hex data, ATCoP receives it in 
      ascii so check with 14 bytes. AB is 1 byte of hex data but it 
      is received as 2 characters */
    if (( VALID_TOKEN(0)) && 
        (strlen((const char *)tok_ptr->arg[0]) == (DSAT_MAX_RANDSSD_LEN*2)))
    {
      /*check session id*/
      if ((ds3g_mmgsdi_get_session_id( 
                          MMGSDI_1X_PROV_PRI_SESSION, &session_id ))
                  && (session_id != DS3G_MMGSDI_INVALID_SESSION_ID))
      {
        /*----------------------------------------------------------
           After conversion, data is in bytes but randssd is a qword
           so, reverse the byte order in order to send the data 
           as received. 
           Eg: RANDSSD received: 112233 (for eg: 3 bytes of data actually ASCII)
           after conversion, it would be 11 22 33 00 in byte format
           but in qword format, it is 33 22 11 00  so, to send it in 
           correct format as received, reverse 11 22 33 00 to be
           33 22 11 00 so this in qword it would be 11 22 33 00  
        ------------------------------------------------------------*/

        if ( (NULL == dsatutil_convert_chset( (const char *)tok_ptr->arg[0], 
                                           ALPHA_HEX,
                                           (const char *)dsat_randssd,
                                           ALPHA_8BIT,
                                           sizeof(dsat_randssd),
                                           FALSE ))||
           ( FALSE == dsatutil_reversen_byte((byte*)dsat_randssd,
                                           strlen((char*)dsat_randssd))))
        {
          /* parameter error */
          DS_AT_MSG0_ERROR("Invalid Hex data");
          result = DSAT_ERROR;
        }
        else
        {
          dsat_mmgsdi_auth_cmd_pending = DSAT_MMGSDI_AUTH_SSD_BS_CHAL;
          ps_utils_generate_rand_num(&rand_seed, sizeof(dword));
          /* get the BS challenge response */
          if ( MMGSDI_SUCCESS == mmgsdi_session_bs_chal(
                                      session_id, 
                                      rand_seed, 
                                      dsat707_mmgsdi_auth_cb, 
             (mmgsdi_client_data_type)SSD_MMGSDI_BSCHAL_SSD))
          {
            DS_AT_MSG1_MED(" BS Challenge invoked successfully with rand %d ",
                                                            rand_seed);
            result = DSAT_ASYNC_CMD;
          }
          else
          {
            DS_AT_MSG0_ERROR("Reporting error for BS challenge");
            dsat_mmgsdi_auth_cmd_pending = DSAT_MMGSDI_AUTH_NONE;
            result = DSAT_ERROR;
          }
        }
      }
      else
      {
        /* Invalid session */
        DS_AT_MSG1_ERROR("DS3G MMGSDI Invalid CDMA Session ID %d",session_id);
        result = DSAT_ERROR;
      }
    }
    else
    {
      /* parameter length error */
      DS_AT_MSG0_ERROR("Invalid length of data received");
      result = dsat_send_cme_error(DSAT_CME_AUTH_PARAM_LENGTH_ERROR );
    }
  }
  /*-------------------------------------------------------
    Test Command
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
    result = DSAT_OK;
  }
  else
  {
   DS_AT_MSG0_ERROR("Syntax error");
   result = DSAT_ERROR;
  }
  return result;
}/* dsat707_exec_ssdupd_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_SSDUPDCFM_CMD

  DESCRIPTION
  This function takes the result from the command line parser and
  executes it. It executes AT^SSDUPDCFM command. Client sends 
  updated SSD confirmation <AUTHBS> to datacard for authentication of 
  already peformed SSD Update. 


  DEPENDENCIES
  None

  RETURN VALUE

  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_CME_UNKNOWN : if there was any problem in executing the command
    DSAT_OK : if it is a success.
    DSAT_ASYNC_CMD: if the command is in progress.

  SIDE EFFECTS
  None.
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsat707_exec_ssdupdcfm_cmd
(
  dsat_mode_enum_type mode,           /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,  /*  Command tokens from parser  */
  dsm_item_type *rb_ptr               /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_ERROR;
  dword authbs = 0 ;
  mmgsdi_session_id_type session_id = DS3G_MMGSDI_INVALID_SESSION_ID;

   dsat_mmgsdi_auth_cmd_pending = DSAT_MMGSDI_AUTH_NONE;

  /* Process Set or Execute command */
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    /* Check the number of arguments. */
    if (tok_ptr->args_found > 1)
    {
      DS_AT_MSG0_ERROR("Too many arguments");
      return DSAT_ERROR;
    }
    /* AUTHU should be 3 bytes of Hex data, ATCoP receives it in 
       ascii so check with 6 bytes. AB is 1 byte of hex data but it 
       is received as 2 characters */
    if (( VALID_TOKEN(0)) && 
        (strlen((const char *)tok_ptr->arg[0]) == (DSAT_MAX_AUTHU_LEN*2)))
    {
      /* check session id */
      if ((ds3g_mmgsdi_get_session_id( MMGSDI_1X_PROV_PRI_SESSION, &session_id ))
                       && (session_id != DS3G_MMGSDI_INVALID_SESSION_ID))
      {
        /*----------------------------------------------------------
           After conversion, data is in bytes but authbs is a dword
           so, reverse the byte order in order to send the data 
           as received. 
           Eg: AUTHBS received: 0317B4 (3 bytes of data actually ASCII)
           after conversion, it would be 03 17 B4 00 in byte format
           but in dword format, it is B4 17 03 00  so, to send it in 
           correct format as received, reverse 03 17 B4 00 to be
           B4 17 03 00 so this in dword it would be  03 17 B4 00 
        ------------------------------------------------------------*/
          /* convert from HEX char pair string to octets*/
        if ( ( NULL == dsatutil_convert_chset( (const char *)tok_ptr->arg[0], 
                                              ALPHA_HEX,
                                              (const char *)&authbs,
                                              ALPHA_8BIT,
                                              sizeof(authbs),
                                              FALSE ))||
              ( FALSE == dsatutil_reversen_byte((byte*)&authbs,
                                              strlen((char*)&authbs))))
        {
          /* parameter error */
          DS_AT_MSG0_ERROR("Invalid Hex data");
          return DSAT_ERROR;
        }
        else
        {
          /* Validate <authbs> to have the MSB 6-bits to be 0 */
          if ( authbs & 0xFC0000 )/* As per requirement */ 
          {
            DS_AT_MSG0_ERROR("MSB 6 bits of 3 bytes data should be 0");
            result = dsat_send_cme_error(DSAT_CME_AUTH_PARAM_LENGTH_ERROR );
            return result;
          }

          dsat_mmgsdi_auth_cmd_pending = DSAT_MMGSDI_AUTH_SSD_CNF;
          if ( MMGSDI_SUCCESS == mmgsdi_session_confirm_ssd( 
                                                 session_id, 
                                                 authbs, 
                                                 dsat707_mmgsdi_auth_cb, 
                        (mmgsdi_client_data_type)SSDUDPCFM_MMGSDI_CFM_SSD ))
          {
            DS_AT_MSG0_HIGH("SSD update confirm request in progress.");
            result = DSAT_ASYNC_CMD;
          }
          else
          {
            DS_AT_MSG0_ERROR("mmgsdi_session_confirm_ssd Failed");
            result = DSAT_ERROR;
            dsat_mmgsdi_auth_cmd_pending = DSAT_MMGSDI_AUTH_NONE;
          }
        }
      }
      else
      {
        DS_AT_MSG1_ERROR("DS3G MMGSDI Invalid CDMA Session ID %d",session_id);
        result = DSAT_ERROR;
      }
    }
    else
    {
      DS_AT_MSG0_ERROR("Invalid length of data received");
      result = dsat_send_cme_error(DSAT_CME_AUTH_PARAM_LENGTH_ERROR );
    }
  }
  /*-------------------------------------------------------
    Test Command
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
    result = DSAT_OK;
  }
  else
  {
    DS_AT_MSG0_ERROR("Syntax error");
    result = DSAT_ERROR;
  }
  return result;
}/* dsat707_exec_ssdupdcfm_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_MD5_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT^MD5 command. 

  It Generates the CHAP Response for the received CHAP Challenge in 
  HRDP authentication. 

  Note: UIMAUTH=? should return ("CAVE,"MD5")

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  DSAT_ASYNC_CMD: if the command is in progress.

  SIDE EFFECTS
  None.
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsat707_exec_md5_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type  result = DSAT_ASYNC_CMD;
  ds3g_siolib_port_e_type port = ds3g_siolib_get_active_port();
  mmgsdi_session_id_type  session_id;
  dsat_error_info_s_type    err_info;

  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  /* Process Execute command */
  if ( tok_ptr->op == (NA|EQ|AR))
  {
    if ( !(tok_ptr->args_found == 2) )
    {
      err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
      goto send_error;
    }
    if ( ( VALID_TOKEN(0)) && ( VALID_TOKEN(1)))
    {
      memset((void *)&dsat707_md5_val, 0x0, sizeof(dsat707_md5_val));
      if ((ds3g_mmgsdi_get_session_id( MMGSDI_1X_PROV_PRI_SESSION, 
                                       &session_id ))
              && (session_id != DS3G_MMGSDI_INVALID_SESSION_ID))
      {
        /*-----------------------------------------------------
            1. Save CHAP ID, CHAP length, session ID
            2. Set the pre processor mode to MD5 ENTRY MODE. 
        -------------------------------------------------------*/
        if ( ATOI_OK != dsatutil_atoi( 
                     (dsat_num_item_type *)&dsat707_md5_val.chap_id, 
                     tok_ptr->arg[0], 10) )
        {
          err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
          err_info.arg_num = 0;
          goto send_error;
        }
        if ( ATOI_OK != dsatutil_atoi( 
                       (dsat_num_item_type *)&dsat707_md5_val.chap_length, 
                       tok_ptr->arg[1], 10))
        {
          err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
          err_info.arg_num = 1;
          goto send_error;
        }
        dsat707_md5_val.session_id  = session_id;
      }
      else
      {
          err_info.errval = DSAT_ERR_INVALID_TOKENS;
          goto send_error;
      }
    }
    else
    {
       err_info.errval = DSAT_ERR_INVALID_TOKENS;
       goto send_error;
    }
    dsatprep_set_preprocessor_mode ( DSATI_MD5_MSG_ENTRY_MODE, port );
    result = DSAT_ASYNC_CMD;
  }
  /*-------------------------------------------------------
    Test Command
  ---------------------------------------------------------*/
  else if ( tok_ptr->op == (NA|EQ|QU))
  {
    result = DSAT_OK;
  }
  else 
  {
     err_info.errval = DSAT_ERR_INVALID_SYNTAX;
     goto send_error;
  }

  return result;

send_error:
  if( err_info.errval == DSAT_ERR_INVALID_SYNTAX ||
      err_info.errval == DSAT_ERR_INVALID_NUM_OF_ARGS ||
      err_info.errval == DSAT_ERR_INVALID_TOKENS )
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  else
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }
  return DSAT_ERROR;
  
}/* dsat707_exec_md5_cmd */
#endif /* FEATURE_DSAT_MMGSDI_AUTH */

/*===========================================================================
  FUNCTION DSAT707_EXEC_HDRCSQ_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT^HDRCSQ command.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.

===========================================================================*/
dsat_result_enum_type dsat707_exec_hdrcsq_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  /* Read Command */
  if ( tok_ptr->op == (NA) )
  {
    uint16 hdr_rssi_intensity = 0;

    hdr_rssi_intensity = dsatcmif_cm_get_hdr_rssi_intensity_level(); 
    
    res_buff_ptr->used = (word)snprintf(
                              (char*)res_buff_ptr->data_ptr,
                               res_buff_ptr->size,
                               "^HDRCSQ: %d",
                               hdr_rssi_intensity);
  }
  /* Test Command */
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    res_buff_ptr->used = 
      (word) snprintf ((char*)res_buff_ptr->data_ptr, 
                              res_buff_ptr->size,
                              "^HDRCSQ: (0, 20, 40, 60, 80, 99)" );
  }
  else
  {
    /* other commands are not supported */
    return DSAT_ERROR;
  }
  return DSAT_OK;
} /* dsat707_exec_hdrcsq_cmd */

#if defined(FEATURE_DSAT_SP_CMDS)
/*===========================================================================
  FUNCTION DSAT707_EXEC_SPMSID_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMSID command.
  returns the device msid.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_spmsid_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
 )
{
  byte                   imsi_s[DSAT707_NV_IMSI_S_LENGTH];
  byte                  *txt_ptr = &imsi_s[0];
  dsat_result_enum_type  result = DSAT_OK;


  if (tok_ptr->op == (NA|QU))
  {
    if (ds707_pkt_mgr_read_min2(txt_ptr) == FALSE)
    {
      DS_AT_MSG0_ERROR("Error reading MIN2 from NV");
      res_buff_ptr->used = DSAT707_NV_IMSI_S_LENGTH + 1;
      memset( res_buff_ptr->data_ptr, 0, DSAT707_NV_IMSI_S_LENGTH + 1 );
    }
    else
    {
      txt_ptr = &imsi_s[3];
      if (ds707_pkt_mgr_read_min1(txt_ptr) == FALSE)
      {
        DS_AT_MSG0_ERROR("Error reading MIN1 from NV");
        res_buff_ptr->used = DSAT707_NV_IMSI_S_LENGTH + 1;
        memset( res_buff_ptr->data_ptr, 0, DSAT707_NV_IMSI_S_LENGTH + 1 );
      }
      else
      {
        res_buff_ptr->used = DSAT707_NV_IMSI_S_LENGTH + 1;

        (void) dsatutil_memscpy((void*)res_buff_ptr->data_ptr,
                                       DSAT707_NV_IMSI_S_LENGTH,
                                (void*)imsi_s,
                DSAT707_NV_IMSI_S_LENGTH );
        res_buff_ptr->data_ptr[res_buff_ptr->used-1] = '\0';
      }
    }
  }/* (tok_ptr->op == NA) */
  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|AR))
  {
    /* this command is valid, but do nothing */
    result = DSAT_ERROR;
  }

  return result;
} /* dsat707_exec_spmsid_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_SPC_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $SPSPC command. It is used to set/get 
  Secure Program Code.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :      if there was any problem in executing the command
    DSAT_OK    :         if it is a success.

  SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_sp_spc_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  char *c_ptr;
  nv_stat_enum_type    nv_stat;

  /* process command */
  if(tok_ptr->op == (NA|EQ|AR))
  {
    /* Verify first string provided & not empty */
    if (!VALID_TOKEN(0) ||
        ('"' == tok_ptr->arg[0][0] && '"' == tok_ptr->arg[0][1]))
    {
      DS_AT_MSG0_HIGH("Null SPC: Secure Program Code string invalid");
      return DSAT_ERROR;
    }

    /*---------------------------------------------------------------
    Processing for a command with an argument (WRITE command)
    ---------------------------------------------------------------*/
    result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );

    if (DSAT_OK == result)
    {
      c_ptr = (char *)(char *)dsatutil_get_val(
                          DSAT707_VEND_ACT_SPSPC_IDX,0,0,MIX_STR_TYPE);

      /* Validate SPC contents */
      while ( '\0' != *c_ptr )
      {
        /* Report error if char is not numeric */
        if (!(('0' <= *c_ptr) && ('9' >= *c_ptr)))
        { 
          DS_AT_MSG0_HIGH("SPC char outside range [0-9]");
          return DSAT_ERROR;
        }
        c_ptr++;
      }

      c_ptr = NULL;

      /* store the SPC lock code into NV.*/
      (void) dsatutil_memscpy((void*)ds_nv_item.sec_code.digits,
                                     NV_SEC_CODE_SIZE,
                              (void*)dsatutil_get_val(DSAT707_VEND_ACT_SPSPC_IDX,0,0,MIX_STR_TYPE),
             NV_SEC_CODE_SIZE);

      nv_stat = dsatutil_put_nv_item(NV_SEC_CODE_I, &ds_nv_item);
      if (nv_stat != NV_DONE_S)
      {
        return DSAT_ERROR;
      }
    }
  }/* (tok_ptr->op == (NA|EQ|AR)) */
  /* process the READ command */
  else if (tok_ptr->op == (NA|QU))
  {
    char sec_code[NV_SEC_CODE_SIZE + 1] = {0};

    nv_stat = dsatutil_get_nv_item(NV_SEC_CODE_I, &ds_nv_item);
    if (nv_stat != NV_DONE_S)
    {
      return DSAT_ERROR;
    }

    (void) dsatutil_memscpy((void*)sec_code,
                                   NV_SEC_CODE_SIZE,
           (void *)ds_nv_item.sec_code.digits,
           NV_SEC_CODE_SIZE);

    res_buff_ptr->used = (word) snprintf ((char*)res_buff_ptr->data_ptr,
                                                 res_buff_ptr->size,
                                                 "%s",
                                                 sec_code);
  }/* (tok_ptr->op == (NA|QU)) */
  /* process the TEST command */
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* Do nothing; return success */
  }

  /* command does not make sense */
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsat707_exec_sp_spc_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_RESET_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$SPRESET command.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_sp_reset_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  /* Reset the phone */
  hw_reset();
  return DSAT_OK;
}/* dsat707_exec_sp_reset_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_FIRMWARE_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$SPFWREV? command.
  returns the device firmware version.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_sp_firmware_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  nv_stat_enum_type    nv_stat;
  nv_stat = dsatutil_get_nv_item(NV_MOB_FIRM_REV_I, &ds_nv_item);
  if (nv_stat != NV_DONE_S)
  {
    return DSAT_ERROR;
  }
  else
  {
    res_buff_ptr->used = snprintf((char*)res_buff_ptr->data_ptr,
                                     res_buff_ptr->size,
                                     "%hu",
                                     ds_nv_item.mob_firm_rev);
  }
  return DSAT_OK;
}  /* dsat707_exec_sp_firmware_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_PRL_ID_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$SPPRL command.
  returns the device prl number.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_sp_prl_id_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  const prl_hdr_s_type  *prl_hdr_ptr = prl_hdr_nv_get_ptr();

  res_buff_ptr->used = snprintf((char*)res_buff_ptr->data_ptr,
                                      res_buff_ptr->size,
                                      "%hu",prl_hdr_ptr->pr_list_id);

  return DSAT_OK;

}  /* dsat707_exec_sp_prl_id_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_ROAM_PREF_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$SPROAM? command.
  returns/set the current roam setting:
    0 - Sprint only
    1 - Automatic
    2 - Roam Only

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_sp_roam_pref_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  nv_stat_enum_type     nv_stat = NV_DONE_S;
  int                   roaming_pref = 0;
  dsat_result_enum_type result = DSAT_OK;

  ds_nv_item.roam_pref.nam = 0;
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, res_buff_ptr);
  if (result != DSAT_OK) 
  {
    return DSAT_ERROR;
  }

  if (tok_ptr->op == (NA|EQ|AR))
  {
    switch ((dsat_num_item_type)dsatutil_get_val(
                             DSAT707_VEND_ACT_SPROAM_IDX,0,0,NUM_TYPE))
    {
      case 0:
        ds_nv_item.roam_pref.roam = NV_ROAM_PREF_HOME;
        break;
      case 1:
        ds_nv_item.roam_pref.roam = NV_ROAM_PREF_ANY;
        break;
      case 2:
        ds_nv_item.roam_pref.roam = NV_ROAM_PREF_AFFIL;
        break;
      default:
        DS_AT_MSG1_ERROR("Unrecognized roaming preference %d",
                   (dsat_num_item_type)dsatutil_get_val(
                     DSAT707_VEND_ACT_SPROAM_IDX,0,0,NUM_TYPE));
        result = DSAT_ERROR;
    }

    if (result != DSAT_ERROR)
    {
      nv_stat = dsatutil_put_nv_item(NV_ROAM_PREF_I, &ds_nv_item);
      if (nv_stat != NV_DONE_S)
      {
        DS_AT_MSG0_ERROR("Error writing NV_ROAM_PREF_I");
        result = DSAT_ERROR;
      }
    }
  }/* (tok_ptr->op == (NA|EQ|AR)) */
  else if (tok_ptr->op == (NA|QU)) /* query */
  {
    nv_stat = dsatutil_get_nv_item(NV_ROAM_PREF_I, &ds_nv_item);

    if (nv_stat != NV_DONE_S)
    {
      result = DSAT_ERROR;
    }
    else
    {
      switch (ds_nv_item.roam_pref.roam)
      {
        case NV_ROAM_PREF_HOME:
          roaming_pref = 0;
          break;
        case NV_ROAM_PREF_ANY:
          roaming_pref = 1;
          break;
        case NV_ROAM_PREF_AFFIL:
          roaming_pref = 2;
          break;
        default:
          result = DSAT_ERROR;
          DS_AT_MSG1_ERROR("Unrecognized roaming preference %d",ds_nv_item.roam_pref.
roam);
          break;
      }
      if (result != DSAT_ERROR)
      {
        res_buff_ptr->used = snprintf((char*)res_buff_ptr->data_ptr,
                                            res_buff_ptr->size,
                                            "%d",roaming_pref);
      }
    }
  }/* (tok_ptr->op == (NA|QU)) */
  /* process the TEST command */
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    res_buff_ptr->used = snprintf((char*)res_buff_ptr->data_ptr,
                                        res_buff_ptr->size,
                                        "0-2");
  }
  /* command does not make sense */
  else
  {
    result = DSAT_ERROR;
  }
  return result;
}  /* dsat707_exec_sp_roam_pref_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_ROAMING_IND_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$SPERI command.
  It returns the device's extended roaming indicator.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_sp_roaming_ind_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  char roaming_ind_strings[][40] = {"SYS_ROAM_STATUS_OFF",
                                    "SYS_ROAM_STATUS_ON",
                                    "SYS_ROAM_STATUS_BLINK",
                                    "SYS_ROAM_STATUS_OUT_OF_NEIGHBORHOOD",
                                    "SYS_ROAM_STATUS_OUT_OF_BLDG",
                                    "SYS_ROAM_STATUS_PREF_SYS",
                                    "SYS_ROAM_STATUS_AVAIL_SYS",
                                    "SYS_ROAM_STATUS_ALLIANCE_PARTNER",
                                    "SYS_ROAM_STATUS_PREMIUM_PARTNER",
                                    "SYS_ROAM_STATUS_FULL_SVC",
                                    "SYS_ROAM_STATUS_PARTIAL_SVC",
                                    "SYS_ROAM_STATUS_BANNER_ON",
                                    "SYS_ROAM_STATUS_BANNER_OFF"};

  const prl_hdr_s_type  *prl_hdr_ptr = prl_hdr_nv_get_ptr();

  if (tok_ptr->op == (NA|QU))
  {
    if ( (prl_hdr_ptr->def_roam_ind > SYS_ROAM_STATUS_NONE) &&
         (prl_hdr_ptr->def_roam_ind < SYS_ROAM_STATUS_MAX) )
    {
      res_buff_ptr->used = snprintf(
                             (char*)res_buff_ptr->data_ptr,
                             res_buff_ptr->size, "%s",
                             roaming_ind_strings[prl_hdr_ptr->def_roam_ind] );
    }
    else
    {
      DS_AT_MSG0_ERROR("Unrecognized roaming indicator");
      result = DSAT_ERROR;
    }
  }
  else /* all other command options are invalid */
  {
    result = DSAT_ERROR;
  }
  return result;
}  /* dsat707_exec_sp_roaming_ind_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_SIG_DBM_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$SPSIGDBM command.
  It returns the current Receive Signal Strength Indicator.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_sp_sig_dbm_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  /* Query */
  if (tok_ptr->op == (NA|QU))
  {
    db_items_value_type dbi;

    db_get (DB_NO_SVC, &dbi);
    if(dbi.no_svc)
    {
      DS_AT_MSG0_ERROR("No cellular service");
    }
    else
    {
      db_get (DB_DIGITAL, &dbi);
      if (dbi.digital)
      {
        db_get (DB_CDMA_RSSI, &dbi);
        res_buff_ptr->used = snprintf((char*)res_buff_ptr->data_ptr,
                                        res_buff_ptr->size,
                                        "%d",dbi.cdma_rssi);
      }
      else
      {
        db_get (DB_RSSI, &dbi);
        res_buff_ptr->used = snprintf((char*)res_buff_ptr->data_ptr,
                                       res_buff_ptr->size,
                                       "%d",dbi.rssi);
      }
    }
  }
  else /* all other command options are invalid */
  {
    return DSAT_ERROR;
  }
  return DSAT_OK;
}   /* dsat707_exec_spr_sig_dbm_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_LOCKED_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$SPLOCKED command.
  It returns phone lock state [Locked / Unlocked].

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_sp_locked_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  nv_stat_enum_type nv_stat = NV_DONE_S;

  /* Query */
  if (tok_ptr->op == (NA|QU))
  {
    nv_stat = dsatutil_get_nv_item(NV_LOCK_I, &ds_nv_item);

    if (nv_stat != NV_DONE_S)
    {
      return DSAT_ERROR;
    }

    res_buff_ptr->used = snprintf((char*)res_buff_ptr->data_ptr,
                                        res_buff_ptr->size,
                                        "%d",ds_nv_item.lock);
  }
  else /* all other command options are invalid */
  {
    return DSAT_ERROR;
  }
  return DSAT_OK;
}/* dsat707_exec_sp_locked_cmd */

/*===========================================================================
  FUNCTION DSATETSIME_EXEC_SP_UNLOCK_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $SPUNLOCK command.
  Syntax format:
    AT$SPUNLOCK=####(##)
    #### is PIN, should also accept SPC (######).
  Response should be:
    ERROR - Unable to unlock phone, still locked.
    OK - Phone already unlocked or was unlocked."

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :      if there was any problem in executing the command
    DSAT_OK    :      if it is a success.

  SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_sp_unlock_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{
  dsat_result_enum_type  result = DSAT_OK;
  char                  *c_ptr = NULL;
  char                   unlock_code[NV_LOCK_CODE_SIZE + 1] = {0};
  char                   sec_code[NV_SEC_CODE_SIZE + 1] = {0};
  uint8                  i = 0;
  nv_stat_enum_type      nv_stat = NV_DONE_S;

  /* process command */
  if(tok_ptr->op == (NA|EQ|AR))
  {
    /* Verify first string provided & not empty */
    if (!VALID_TOKEN(0) ||
        ('"' == tok_ptr->arg[0][0] && '"' == tok_ptr->arg[0][1]))
    {
      DS_AT_MSG0_HIGH("Null PIN string invalid");
      return DSAT_ERROR;
    }

    /*---------------------------------------------------------------
    Processing for a command with an argument (WRITE command)
    ---------------------------------------------------------------*/
    result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );

    if (DSAT_OK == result)
    {
      c_ptr = (char *)dsatutil_get_val(
        DSAT707_VEND_ACT_SPUNLOCK_IDX,0,0,MIX_STR_TYPE);

      /* Read till '(' */
      while (( '\0' != *c_ptr ) && ('(' != *c_ptr))
      {
        /* Report error if char is not numeric */
        if (!(('0' <= *c_ptr) && ('9' >= *c_ptr)))
        {
          DS_AT_MSG0_HIGH("PIN char outside range [0-9]");
          return DSAT_ERROR;
        }
        unlock_code[i++]= *c_ptr;
        if (i > NV_LOCK_CODE_SIZE )
        {
          DS_AT_MSG0_ERROR("LOCK Code too big");
          return DSAT_ERROR;
        }
        c_ptr++;
      }

      /* skip '(' and parse rest of the string till ')' */
      if (*c_ptr == '(')
      {
        c_ptr++;
      }

      i = 0;
      while (( '\0' != *c_ptr ) && (')' != *c_ptr))
      {
        /* Report error if char is not numeric */
        if (!(('0' <= *c_ptr) && ('9' >= *c_ptr)))
        {
          DS_AT_MSG0_HIGH("PIN char outside range [0-9]");
          return DSAT_ERROR;
        }
        sec_code[i++]= *c_ptr;
        if (i > NV_SEC_CODE_SIZE )
        {
          DS_AT_MSG1_ERROR("SEC Code too big %d ", i);
          return DSAT_ERROR;
        }
        c_ptr++;
      }

      /* We should be here with ending paranthesis, otherwise it is a syntax 
error */
      if (( *c_ptr == '\0' ) ||
          ((*c_ptr != ')')  && ((*(c_ptr+1)) != '\0')))
      {
        /* We have a syntax error here! */
        return DSAT_ERROR;
      }

      /* Get SPC code from NV */
      nv_stat = dsatutil_get_nv_item(NV_SEC_CODE_I, &ds_nv_item);
      if (nv_stat != NV_DONE_S)
      {
        return DSAT_ERROR;
      }

      /* Verify the SPC */
      if( memcmp((byte *)ds_nv_item.sec_code.digits,
                  sec_code,
                  NV_SEC_CODE_SIZE ) == 0 )
      {
        /* SPC is verified, now verify LOCK code */
        nv_stat = dsatutil_get_nv_item(NV_LOCK_CODE_I, &ds_nv_item);
        if (nv_stat != NV_DONE_S)
        {
          return DSAT_ERROR;
        }

        if( memcmp((byte *)ds_nv_item.lock_code.digits,
                    unlock_code,
                    NV_LOCK_CODE_SIZE ) == 0 )
        {
          /* Unlock the phone.*/
          ds_nv_item.lock = 0;
          nv_stat = dsatutil_put_nv_item(NV_LOCK_I, &ds_nv_item);

          if (nv_stat != NV_DONE_S)
          {
            return DSAT_ERROR;
          }
        }
        else
        {
          /* Lock code mis-match */
          result = DSAT_ERROR;
        }
      }
      else
      {
        /* SPC code mis-match */
        result = DSAT_ERROR;
      }
    }
  }/* tok_ptr->op == (NA|EQ|AR)) */
  /* all other command options does not make sense */
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsat707_exec_sp_unlock_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_LOCK_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $SPLOCK command.
  Syntax format:
    AT$SPLOCK=####
    #### is PIN which must be correct before phone is locked.
  Response should be:
    ERROR - Unable to lock phone, still unlocked.
    OK - Phone already locked or was locked."

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :      if there was any problem in executing the command
    DSAT_OK    :      if it is a success.

  SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_sp_lock_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{
  dsat_result_enum_type  result = DSAT_OK;
  char                  *c_ptr = NULL;
  nv_stat_enum_type      nv_stat = NV_DONE_S;
  
  /* process command */
  if(tok_ptr->op == (NA|EQ|AR))
  {
    /* Verify first string provided & not empty */
    if (!VALID_TOKEN(0) ||
        ('"' == tok_ptr->arg[0][0] && '"' == tok_ptr->arg[0][1]))
    {
      DS_AT_MSG0_HIGH("Null PIN string invalid");
      return DSAT_ERROR;
    }

    /*---------------------------------------------------------------
    Processing for a command with an argument (WRITE command)
    ---------------------------------------------------------------*/
    result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );

    if (DSAT_OK == result)
    {
      c_ptr = (char *)dsatutil_get_val(
        DSAT707_VEND_ACT_SPLOCK_IDX,0,0,MIX_STR_TYPE);
      while ( '\0' != *c_ptr )
      {
        /* Report error if char is not numeric */
        if (!(('0' <= *c_ptr) && ('9' >= *c_ptr)))
        { 
          DS_AT_MSG0_HIGH("PIN char outside range [0-9]");
          return DSAT_ERROR;
        }
        c_ptr++;
      }
      c_ptr = NULL;

      /* verify the lock code from NV.*/
      nv_stat = dsatutil_get_nv_item(NV_LOCK_CODE_I, &ds_nv_item);
      if (nv_stat != NV_DONE_S)
      {
        return DSAT_ERROR;
      }

      if( memcmp((byte *)ds_nv_item.lock_code.digits,
                  (char *)dsatutil_get_val(
                  DSAT707_VEND_ACT_SPLOCK_IDX,0,0,MIX_STR_TYPE),
                  NV_LOCK_CODE_SIZE ) == 0 )
      {
        /* Lock the phone.*/
        ds_nv_item.lock = 1;
        nv_stat = dsatutil_put_nv_item(NV_LOCK_I, &ds_nv_item);

        if (nv_stat != NV_DONE_S)
        {
          DS_AT_MSG1_ERROR("Error updating NV_LOCK_I %d", nv_stat);
          return DSAT_ERROR;
        }
      }
      else
      {
        /* Lock code mis-match */
        result = DSAT_ERROR;
      }

    }
  }/* (tok_ptr->op == (NA|EQ|AR)) */
  /* all other command options does not make sense */
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsat707_exec_sp_lock_cmd */

/*===========================================================================
  FUNCTION DSATETSIME_EXEC_SP_LOCK_CHG_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $SPLOCKCHG command.
  Syntax format:
  AT$SPLOCKCHG=####(##),####
    First  #### is current PIN, should also accept SPC (######).
    Second #### should be new PIN.

  Response should be:
    ERROR - Unable to change lock PIN.
    OK -Lock PIN changed."

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :      if there was any problem in executing the command
    DSAT_OK    :      if it is a success.

  SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_sp_lock_chg_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{
  dsat_result_enum_type  result = DSAT_OK;
  char                  *c_ptr = NULL;
  char                   old_lock_code[NV_LOCK_CODE_SIZE + 1] = {0};
  char                   sec_code[NV_SEC_CODE_SIZE + 1] = {0};
  uint8                  i = 0;
  nv_stat_enum_type      nv_stat;

  /* process command */
  if(tok_ptr->op == (NA|EQ|AR))
  {
    /* Verify first string provided & not empty */
    if (!VALID_TOKEN(0) ||
        ('"' == tok_ptr->arg[0][0] && '"' == tok_ptr->arg[0][1]))
    {
      DS_AT_MSG0_HIGH("Null PIN string invalid");
      return DSAT_ERROR;
    }

    /*---------------------------------------------------------------
    Processing for a command with an argument (WRITE command)
    ---------------------------------------------------------------*/
    result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );

    if (DSAT_OK == result)
    {
      c_ptr = (char *)(char *)dsatutil_get_val(
        DSAT707_VEND_ACT_SPLOCKCHG_IDX,0,0,MIX_STR_TYPE);
      /* Read till '(' */
      while (( '\0' != *c_ptr ) && ('(' != *c_ptr))
      {
        /* Report error if char is not numeric */
        if (!(('0' <= *c_ptr) && ('9' >= *c_ptr)))
        {
          DS_AT_MSG0_HIGH("PIN char outside range [0-9]");
          return DSAT_ERROR;
        }
        old_lock_code[i++]= *c_ptr;
        if (i > NV_LOCK_CODE_SIZE )
        {
          DS_AT_MSG0_ERROR("LOCK Code too big");
          return DSAT_ERROR;
        }
        c_ptr++;
      }

      /* skip '(' and parse rest of the string till ')' */
      if (*c_ptr == '(')
      {
        c_ptr++;
      }

      i = 0;
      while (( '\0' != *c_ptr ) && (')' != *c_ptr))
      {
        /* Report error if char is not numeric */
        if (!(('0' <= *c_ptr) && ('9' >= *c_ptr)))
        {
          DS_AT_MSG0_HIGH("PIN char outside range [0-9]");
          return DSAT_ERROR;
        }
        sec_code[i++]= *c_ptr;
        if (i > NV_SEC_CODE_SIZE )
        {
          DS_AT_MSG1_ERROR("SEC Code too big %d ", i);
          return DSAT_ERROR;
        }
        c_ptr++;
      }

      /* We should be here with ending paranthesis, otherwise it is a syntax 
error */
      if ((*c_ptr != ')')  && ((*(c_ptr+1)) != '\0'))
      {
        /* We have a syntax error here! */
        return DSAT_ERROR;
      }

      /* Get SPC code from NV */
      nv_stat = dsatutil_get_nv_item(NV_SEC_CODE_I, &ds_nv_item);
      if (nv_stat != NV_DONE_S)
      {
        return DSAT_ERROR;
      }

      /* Verify the SPC */
      if( memcmp((byte *)ds_nv_item.sec_code.digits,
                  sec_code,
                  NV_SEC_CODE_SIZE ) == 0 )
      {
        /* SPC is verified, now verify LOCK code */
        nv_stat = dsatutil_get_nv_item(NV_LOCK_CODE_I, &ds_nv_item);
        if (nv_stat != NV_DONE_S)
        {
          return DSAT_ERROR;
        }

        if( memcmp((byte *)ds_nv_item.lock_code.digits,
                    old_lock_code,
                    NV_LOCK_CODE_SIZE ) == 0 )
        {
          /* Now verify the new lock code */
          c_ptr = (char *)(char *)dsatutil_get_val(
          DSAT707_VEND_ACT_SPLOCKCHG_IDX,0,1,MIX_STR_TYPE);
          while (( '\0' != *c_ptr ) && ('(' != *c_ptr))
          {
            /* Report error if char is not numeric */
            if (!(('0' <= *c_ptr) && ('9' >= *c_ptr)))
            {
              DS_AT_MSG0_HIGH("PIN char outside range [0-9]");
              return DSAT_ERROR;
            }
            c_ptr++;
          }

          /* store the New lock code into NV.*/
          (void) dsatutil_memscpy((void*)ds_nv_item.lock_code.digits,NV_LOCK_CODE_SIZE,
                (void*)dsatutil_get_val(DSAT707_VEND_ACT_SPLOCKCHG_IDX,0,1,MIX_STR_TYPE),
                  NV_LOCK_CODE_SIZE);

          nv_stat = dsatutil_put_nv_item(NV_LOCK_CODE_I, &ds_nv_item);
          if (nv_stat != NV_DONE_S)
          {
            return DSAT_ERROR;
          }
        }
        else /* Lock code mis-match */
        {
          result = DSAT_ERROR;
        }
      }
      else /* SPC code mis-match */
      {
        result = DSAT_ERROR;
      }
    }
  }/* tok_ptr->op == (NA|EQ|AR)) */
  /* all other command options does not make sense */
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsat707_exec_sp_lock_chg_cmd */

/*===========================================================================
  FUNCTION DSATETSIME_EXEC_SP_CURRENT_LOCK_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $SPCURRENTLOCK command.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :      if there was any problem in executing the command
    DSAT_OK    :      if it is a success.

  SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_sp_current_lock_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  /* process command */
  if (tok_ptr->op == (NA|QU))
  {
    char lock_code[NV_LOCK_CODE_SIZE + 1] = {0};
    nv_stat_enum_type    nv_stat;

    nv_stat = dsatutil_get_nv_item(NV_LOCK_CODE_I, &ds_nv_item);
    if (nv_stat != NV_DONE_S)
    {
      return DSAT_ERROR;
    }

    (void) dsatutil_memscpy((void*)lock_code,NV_LOCK_CODE_SIZE,
          (void*)ds_nv_item.lock_code.digits,NV_LOCK_CODE_SIZE);

    res_buff_ptr->used = (word) snprintf ((char*)res_buff_ptr->data_ptr,
                                                    res_buff_ptr->size,
                                                    "%s",
                                                    lock_code);
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsat707_exec_sp_current_lock_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_NAI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$SPNAI command. It prints "0" as we don't
  support more than 1 MIP session.


  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK    : if it is a success.
    DSAT_ERROR : if there was any problem in executing the command

  SIDE EFFECTS
  None.

===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsat707_exec_sp_nai_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  /* Query */
  if (tok_ptr->op == (NA|QU))
  {
    res_buff_ptr->used = snprintf((char*)res_buff_ptr->data_ptr,
                                     res_buff_ptr->size,
                                     "0");
  }
  else
  /* all other command options are invalid */
  {
    return DSAT_ERROR;
  }
  return DSAT_OK;
} /* dsat707_exec_sp_nai_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_SPSERVICE_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$SPSERVICE command.
  This should show the current service capabilites. 

  Response:
    0 -- No Service
    1 -- 1xRTT (CDMA)
    2 -- EVDO
    3 -- EVDO REV A
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
/*ARGSUSED*/
 dsat_result_enum_type dsat707_exec_spservice_cmd
(
  dsat_mode_enum_type      mode,         /* AT command mode:            */
  const dsati_cmd_type     *tab_entry,   /* Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,     /* Command tokens from parser  */
  dsm_item_type            *res_buff_ptr /* Place to put response       */
)
{
  if ( tok_ptr->op == (NA|QU ))
  {
     res_buff_ptr->used = (word)snprintf((char*)res_buff_ptr->data_ptr,
                                               res_buff_ptr->size,
                                               "$SPSERVICE:%d",
                                               dsat_spservice);
  }
  else
  {
    return DSAT_ERROR;
  }
  return DSAT_OK;
}
#endif /* defined(FEATURE_DSAT_SP_CMDS) */

#if defined(FEATURE_DSAT_GPS_CMDS)
/*===========================================================================
  FUNCTION DSAT707_EXEC_A_TRACKING_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$A_TRACKING command.
  Initiates NMEA output stream.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_a_tracking_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  UNUSED_ARG(mode);
  UNUSED_ARG(tab_entry);
  UNUSED_ARG(tok_ptr);
  UNUSED_ARG(res_buff_ptr);

  result = dsat_gps_init();

  return result;
} /* dsat707_exec_dsat_a_tracking_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_NMEA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$SPNMEA command.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_sp_nmea_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  pdsm_pa_info_type     dsat_pa_info;

  result = dsat_gps_init();

  if (result == DSAT_OK)
  {
    result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, res_buff_ptr);
  }

  if (result == DSAT_OK)
  {
    if (tok_ptr->op == (NA|EQ|AR))
    {
      if ((dsat_num_item_type)dsatutil_get_val(
                       DSAT707_VEND_ACT_SPNMEA_IDX,0,0,NUM_TYPE) == 1 ) /* Disable */
      {
        /* Diable NMEA stream. Send NMEA port as none.*/
        dsat_pa_info.nmea_config_info.pd_nmea_port_type = 
          PDSM_PA_NMEA_PORT_TYPE_NONE;
        dsat_pa_info.nmea_config_info.pd_nmea_reporting_type = 
          PDSM_PA_NMEA_REPORT_NONE;
      }
      else if ((dsat_num_item_type)dsatutil_get_val(
                        DSAT707_VEND_ACT_SPNMEA_IDX,0,0,NUM_TYPE) == 0) /* Enable */
      {
        /* Enable stream with the NMEA port 
         * 0 - UART1, 1-UART2, 2-USB, 3 - Shared memory. */
        dsat_pa_info.nmea_config_info.pd_nmea_port_type = 
          (pdsm_pa_nmea_port_e_type)dsatutil_get_val(
                             DSAT707_VEND_ACT_SPNMEA_IDX,0,1,NUM_TYPE);
        dsat_pa_info.nmea_config_info.pd_nmea_reporting_type = 
          PDSM_PA_NMEA_REPORT_AT_1HZ_INTERVAL;
      }
      if (!pdsm_set_parameters(dsat_pa_cmd_cb, NULL, PDSM_PA_NMEA_CONFIG_INFO,
                                  &dsat_pa_info, dsat_pd_client_id))
      {
        result = DSAT_ERROR;
      }
    }/* (tok_ptr->op == (NA|EQ|AR)) */
    else if (tok_ptr->op == (NA|QU))
    {
      /* Get the NMEA settings from NV */
      nv_stat_enum_type    nv_stat;
      uint8 nmea_disabled = 0;
      nv_stat = dsatutil_get_nv_item(NV_CGPS_NMEA_CONFIG_INFO_I, &ds_nv_item);
      if (nv_stat != NV_DONE_S)
      {
        return DSAT_ERROR;
      }

      if (ds_nv_item.cgps_nmea_config_info.nmea_port_type == 
            (int32)PDSM_PA_NMEA_PORT_TYPE_NONE)
      {
        nmea_disabled = 1;
      }
      else
      {
        nmea_disabled = 0; /* enabled */
      }

      res_buff_ptr->used = snprintf((char*)res_buff_ptr->data_ptr,
                                     res_buff_ptr->size,
                                     "%d", nmea_disabled);
    }/* (tok_ptr->op == (NA|QU)) */
    /* all other command options does not make sense */
    else
    {
      result = DSAT_ERROR;
    }
  }
  return result;
} /* dsat707_exec_sp_nmea_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_LOCATION_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$SPLOCATION command.
  It returns GPS lock state [ON / OFF].

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_sp_location_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  nv_stat_enum_type     nv_stat = NV_DONE_S;
  uint8                 lcs_on = 0;

  result = dsat_gps_init();
  
  if (result == DSAT_OK)
  {
    result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, res_buff_ptr);
  }

  if (result == DSAT_OK)
  {
    /* Query */
    if (tok_ptr->op == (NA|QU))
    {
      /* Get GPS lock status from NV.*/
      nv_stat = dsatutil_get_nv_item(NV_GPS1_LOCK_I, &ds_nv_item);
      if (nv_stat != NV_DONE_S)
      {
        return DSAT_ERROR;
      }
      if (ds_nv_item.gps1_lock != (int32)PDSM_GPS_LOCK_ALL)
      {
        lcs_on = 1; /* LCS on in some form */
      }

      res_buff_ptr->used = snprintf((char*)res_buff_ptr->data_ptr,
                                          res_buff_ptr->size,
                                          "%d",lcs_on);
    }
    else if (tok_ptr->op == (NA|EQ|AR))
    {
      pdsm_pa_info_type dsat_pa_info;
      /* set GPS lock */
      if ((dsat_num_item_type)dsatutil_get_val(
                    DSAT707_VEND_ACT_SPLOCATION_IDX,0,0,NUM_TYPE) == 0) /* LCS OFF */
      {
        dsat_pa_info.gps_lock = PDSM_GPS_LOCK_ALL;
      }
      else
      {
        dsat_pa_info.gps_lock = PDSM_GPS_LOCK_NONE;
      }

        /* Send it to PDSM to delete the GPS related parameters */
      if (!pdsm_set_parameters(dsat_pa_cmd_cb, 
                                NULL,
                                PDSM_PA_GPS_LOCK,
                                &dsat_pa_info,
                                dsat_pd_client_id))
      {
        result = DSAT_ERROR;
      }
    }
    else /* all other command options are invalid */
    {
      return DSAT_ERROR;
    }
  }
  return DSAT_OK;
}/* dsat707_exec_sp_location_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_GPS_MODE_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$$GPS_MODE command.
    0 - speed
    1 - accuracy
    2 - data
    3 - ms assisted
    4 - ms based
    5 - standalone

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_gps_mode_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  /* Init DSAT GPS, if not done already */
  result = dsat_gps_init();

  if (result == DSAT_OK)
  {
    result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, res_buff_ptr);
  }

  if (result == DSAT_OK && tok_ptr->op == (NA|EQ|AR))
  {
    switch ((dsat_num_item_type)dsatutil_get_val(DSAT707_VEND_ACT_ACT_GPS_MODE_IDX,0,0,NUM_TYPE))
    {
#if defined(FEATURE_CGPS)
      case 0:
        dsat_pd_option.operation_mode = PDSM_SESSION_OPERATION_OPTIMAL_SPEED;
        break;

      case 1:
        dsat_pd_option.operation_mode =
          PDSM_SESSION_OPERATION_OPTIMAL_ACCURACY;
        break;

      case 2:
        dsat_pd_option.operation_mode = PDSM_SESSION_OPERATION_OPTIMAL_DATA;
        break;

      case 3:
        dsat_pd_option.operation_mode = PDSM_SESSION_OPERATION_MSASSISTED;
        break;

      case 4:
        dsat_pd_option.operation_mode = PDSM_SESSION_OPERATION_MSBASED;
        break;

      case 5:
        dsat_pd_option.operation_mode =
          PDSM_SESSION_OPERATION_STANDALONE_ONLY;
        break;
#else /* defined(FEATURE_CGPS) */
      case 0:
        dsat_pd_option.session_info.operation_mode =
          PDSM_SESSION_OPERATION_OPTIMAL_SPEED;
        break;

      case 1:
        dsat_pd_option.session_info.operation_mode =
          PDSM_SESSION_OPERATION_OPTIMAL_ACCURACY;
        break;

      case 2:
        dsat_pd_option.session_info.operation_mode =
          PDSM_SESSION_OPERATION_OPTIMAL_DATA;
        break;

      case 3:
        dsat_pd_option.session_info.operation_mode =
          PDSM_SESSION_OPERATION_STANDALONE_ONLY;
        break;

      case 4:
        dsat_pd_option.session_info.operation_mode =
          PDSM_SESSION_OPERATION_MSBASED;
        break;

      case 5:
        dsat_pd_option.session_info.operation_mode =
          PDSM_SESSION_OPERATION_STANDALONE_ONLY;
        break;
#endif /* !defined(FEATURE_CGPS) */

      default:
        DS_AT_MSG0_ERROR("Illegal value for GPS mode");
        result = DSAT_ERROR;
        break;
    }
  }
  return result;
} /* dsat707_exec_dsat_gps_mode_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_PDE_TRS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$$PDE_TRS command.
  Sets the GPS transport layer.
    0 - TCP
    1 - DBM
    2 - UI (SIO)
    3 - [Reserved for GCS]
    4 - [Reserved for CP]

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_pde_trs_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
#ifndef FEATURE_CGPS
  pdsm_pa_info_type dsat_pa_info;
#endif
  result = dsat_gps_init();

  if (result == DSAT_OK)
  {
    result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, res_buff_ptr);
  }

#ifndef FEATURE_CGPS
  if (result == DSAT_OK && tok_ptr->op == (NA|EQ|AR))
  {
    switch((dsat_num_item_type)dsatutil_get_val(DSAT707_VEND_ACT_ACT_PDE_TRS_IDX,0,0,NUM_TYPE))
    {
      case 0: /* TCP */
        dsat_pa_info.ptlm_mode = PTLM_MODE_TCP;
        break;

      case 1: /* DBM */
        dsat_pa_info.ptlm_mode = PTLM_MODE_DBM;
        break;

#if defined(FEATURE_MMGPS)
      case 2: /* SIO */
        dsat_pa_info.ptlm_mode = PTLM_MODE_SIO;
        break;

      case 3: /* GCS */
        dsat_pa_info.ptlm_mode = PTLM_MODE_GCS;
        break;

      case 4: /* CP */
        dsat_pa_info.ptlm_mode = PTLM_MODE_CP;
        break;
#endif /* defined(FEATURE_MMGPS) */

      default:
        DS_AT_MSG0_ERROR("Illegal GPS transport mode");
        result = DSAT_ERROR;
        break;
    }

    /* Set the parameter in PDSM */
    if (result == DSAT_OK)
    {
        if (!pdsm_set_parameters(dsat_pa_cmd_cb, NULL, PDSM_PA_PTLM_MODE,
                                 &dsat_pa_info, dsat_pd_client_id))
        {
          result = DSAT_ERROR;
        }
    }
  }
#endif
  return result;
} /* dsat707_exec_dsat_pde_trs_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_GPS_PDEADDR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$$GPS_PDEADDR command.
  Sets the IP address and port of the PDE.
  [Data1].[Data2].[Data3].[Data4]:[Data5]

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_gps_pdeaddr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  result = dsat_gps_init();

  if (result == DSAT_OK)
  {
    result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, res_buff_ptr);
  }

  if (result == DSAT_OK)
  {
    if (tok_ptr->op == (NA|EQ|AR))
    {
      /* Process AT$$GPS_ADDR= x,y,z,w,p */
      /* Load up local version of PDE address and port */
      dsat_pd_option.lsinfo.server_option = PDSM_SERVER_OPTION_USE_LOCAL;
#if defined(FEATURE_CGPS)
      dsat_pd_option.lsinfo.server_adrs.server_addr_type = 
        PDSM_SERVER_TYPE_IPV4_ADDRESS;
      dsat_pd_option.lsinfo.server_adrs.server_adrs.server_adrs_v4.\
        server_adrs =
          (((dsat_num_item_type)dsatutil_get_val(
          DSAT707_VEND_ACT_ACT_GPS_PDEADDR_IDX,0,0,NUM_TYPE)) << 24) |
          (((dsat_num_item_type)dsatutil_get_val(
          DSAT707_VEND_ACT_ACT_GPS_PDEADDR_IDX,0,1,NUM_TYPE)) << 16) |
          (((dsat_num_item_type)dsatutil_get_val(
          DSAT707_VEND_ACT_ACT_GPS_PDEADDR_IDX,0,2,NUM_TYPE)) << 8) |
          ((dsat_num_item_type)dsatutil_get_val(
          DSAT707_VEND_ACT_ACT_GPS_PDEADDR_IDX,0,3,NUM_TYPE));
      dsat_pd_option.lsinfo.server_adrs.server_adrs.server_adrs_v4.port_id =
                                      (dsat_num_item_type)dsatutil_get_val(
                                      DSAT707_VEND_ACT_ACT_GPS_PDEADDR_IDX,0,4,NUM_TYPE);
      DS_AT_MSG3_MED("CGPS pdeaddr opt=%d, addr=0x%x, port=%d",
        dsat_pd_option.lsinfo.server_option,
        dsat_pd_option.lsinfo.server_adrs.server_adrs.server_adrs_v4.\
          server_adrs,
        dsat_pd_option.lsinfo.server_adrs.server_adrs.server_adrs_v4.port_id);

#else /* defined(FEATURE_CGPS) */
      dsat_pd_option.lsinfo.server_adrs = (((dsat_num_item_type)dsatutil_get_val(
      DSAT707_VEND_ACT_ACT_GPS_PDEADDR_IDX,0,0,NUM_TYPE)) << 24) |
                                            (((dsat_num_item_type)dsatutil_get_val(
                                            DSAT707_VEND_ACT_ACT_GPS_PDEADDR_IDX,0,1,NUM_TYPE)) << 16) |
                                            (((dsat_num_item_type)dsatutil_get_val(
                                            DSAT707_VEND_ACT_ACT_GPS_PDEADDR_IDX,0,2,NUM_TYPE)) << 8) |
                                            ((dsat_num_item_type)dsatutil_get_val(
                                            DSAT707_VEND_ACT_ACT_GPS_PDEADDR_IDX,0,3,NUM_TYPE));
      dsat_pd_option.lsinfo.server_port = (dsat_num_item_type)dsatutil_get_val(
                                            DSAT707_VEND_ACT_ACT_GPS_PDEADDR_IDX,0,4,NUM_TYPE);

      DS_AT_MSG3_MED("pdeaddr opt=%d, addr=0x%x, port=%d",
                dsat_pd_option.lsinfo.server_option,
                dsat_pd_option.lsinfo.server_adrs,
                dsat_pd_option.lsinfo.server_port);
#endif /* !defined(FEATURE_CGPS) */
    }/* (tok_ptr->op == (NA|EQ|AR)) */
    else if (tok_ptr->op == (NA))
    {
      /* Process AT$$GPS_ADDR   [No arguments] */
      dsat_pd_option.lsinfo.server_option = PDSM_SERVER_OPTION_USE_DEFAULT;
      memset(&dsat_pd_option.lsinfo.server_adrs,0,
              sizeof(dsat_pd_option.lsinfo.server_adrs));
    }
  }/* (result == DSAT_OK) */
  
  return result;
} /* dsat707_exec_dsat_gps_pdeaddr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_GPS_PORT_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$$GPS_PORT command.
  Sets the port of the PDE.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_gps_port_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  result = dsat_gps_init();

  if (result == DSAT_OK)
  {
    result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, res_buff_ptr);
  }

  if (result == DSAT_OK)
  {
    if (tok_ptr->op == (NA|EQ|AR))
    {
       /* Load up local version of PDE port */
      dsat_pd_option.lsinfo.server_option = PDSM_SERVER_OPTION_USE_LOCAL;

#ifdef FEATURE_CGPS
      dsat_pd_option.lsinfo.server_adrs.server_adrs.server_adrs_v4.port_id = 
               (dsat_num_item_type)dsatutil_get_val(
                       DSAT707_VEND_ACT_ACT_GPS_PORT_IDX,0,0,NUM_TYPE);
#else
      dsat_pd_option.lsinfo.server_port = 
      (dsat_num_item_type)dsatutil_get_val(
                        DSAT707_VEND_ACT_ACT_GPS_PORT_IDX,0,0,NUM_TYPE);
#endif
      DS_AT_MSG2_MED("pdeaddr opt=%d port=%d",
                dsat_pd_option.lsinfo.server_option, 
                (dsat_num_item_type)dsatutil_get_val(
                DSAT707_VEND_ACT_ACT_GPS_PORT_IDX,0,0,NUM_TYPE));
    }/* (tok_ptr->op == (NA|EQ|AR)) */
    else if (tok_ptr->op == (NA))
    {
      /* Process AT$$GPS_PORT   [No arguments] */
      dsat_pd_option.lsinfo.server_option = PDSM_SERVER_OPTION_USE_DEFAULT;
#ifdef FEATURE_CGPS
      dsat_pd_option.lsinfo.server_adrs.server_adrs.server_adrs_v4.port_id = 0;
#else
      dsat_pd_option.lsinfo.server_port = 0;
#endif
    }
  }/* (result == DSAT_OK) */
  
  return result;
} /* dsat707_exec_dsat_gps_port_cmd */
/*===========================================================================
  FUNCTION DSAT707_EXEC_INIT_MASK_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$$INIT_MASK command.
  Sets the PE init mask to clear data structures.
    0  - Synonymous with 7
    1  - Delete ephemeris data in EFS
    2  - Delete almanac data in EFS
    4  - Delete MS position data in EFS
    7  - Delete all data in EFS
    8  - Set max MS speed to DATA2
    16 - System Init

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_gps_init_mask_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  pdsm_pa_info_type dsat_pa_info;
  
  result = dsat_gps_init();

  if (result == DSAT_OK)
  {
    result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, res_buff_ptr);
  }

  if (result == DSAT_OK && tok_ptr->op == (NA|EQ|AR))
  {
    /* Map the zero case to be (mostly) all inclusive */
    if ((dsat_num_item_type)dsatutil_get_val(
                 DSAT707_VEND_ACT_ACT_INIT_MASK_IDX,0,0,NUM_TYPE) == 0)
    {
      /* All inclusive */
      dsat_pa_info.delete_params.pdsm_delete_parms_flags = 
DSAT_PDSM_PA_DELETE_ALL;
    }
    else if (((dsat_num_item_type)dsatutil_get_val(
        DSAT707_VEND_ACT_ACT_INIT_MASK_IDX,0,0,NUM_TYPE)) & DSAT_PDSM_PA_DELETE_ALL)
    {
      /* valid mask bits combination */
       dsat_pa_info.delete_params.pdsm_delete_parms_flags = 
                           (dsat_num_item_type)dsatutil_get_val(
                           DSAT707_VEND_ACT_ACT_INIT_MASK_IDX,0,0,NUM_TYPE);
    }
    else
    {
      /* Not a valid mask bits combination */
      return DSAT_ERROR;
    }

    /* Send it to PDSM to delete the GPS related parameters */
    if (!pdsm_set_parameters(dsat_pa_cmd_cb,
                                NULL,
                                PDSM_PA_DELETE_PARAMS,
                                &dsat_pa_info,
                                dsat_pd_client_id))
    {
        result = DSAT_ERROR;
    }
  }
  return result;
} /* dsat707_exec_dsat_init_mask_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_GET_POS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$$get_pos command.
  Initiates a GPS position fix.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_get_pos_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  DS_AT_MSG0_MED("In dstat get pos");

  result = dsat_gps_init();

  if (result == DSAT_OK)
  {
    DS_AT_MSG0_MED("sending position request");
    /* Get position from PDSM */
    if (!pdsm_get_position(dsat_pd_cmd_cb, 
                             NULL,
#ifndef FEATURE_CGPS
                             PDSM_PD_SVC_TYPE_MASK,
#endif
                             &dsat_pd_option, 
                             &dsat_pd_qos, 
                             dsat_pd_client_id))
    {
      result = DSAT_ERROR;
    }
  }
  return result;
} /* dsat707_exec_dsat_get_pos_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_GPS_INFO_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$GPS_INFO command.
  Gets results of last position fix requested.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_gps_info_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  int                   lat_deg = 0;
  int                   lon_deg = 0;
  float                 lat_min = 0;
  float                 lon_min = 0;
  char                  lat_ns = 0;
  char                  lon_ew = 0;
  int                   i = 0;
  int                   chk_sum = 0;
  dsm_item_type        *tmp_buffer = NULL;

  UNUSED_ARG(mode);
  UNUSED_ARG(tab_entry);
  UNUSED_ARG(tok_ptr);

  result = dsat_gps_init();

  if (dsat_pd_info.pd_info.pd_ext_data.lat < 0)
  {
    dsat_pd_info.pd_info.pd_ext_data.lat *= -1;
    lat_ns = 'S';
  }
  else
  {
      lat_ns = 'N';
  }
  lat_deg = (int)(dsat_pd_info.pd_info.pd_ext_data.lat/100000000LL);
  lat_min = (float)(((dsat_pd_info.pd_info.pd_ext_data.lat -
              (lat_deg * 100000000LL))/100000000.0) * 60.0);

  if (dsat_pd_info.pd_info.pd_ext_data.lon < 0)
  {
      dsat_pd_info.pd_info.pd_ext_data.lon *= -1;
      lon_ew = 'W';
  }
  else
  {
      lon_ew = 'E';
  }
  lon_deg = (int)(dsat_pd_info.pd_info.pd_ext_data.lon/100000000LL);
  lon_min = (float)(((dsat_pd_info.pd_info.pd_ext_data.lon -
              (lon_deg * 100000000LL))/100000000.0) * 60.0);

  tmp_buffer = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
  (void)snprintf(
    (char *)tmp_buffer->data_ptr,
    tmp_buffer->size,
    "$GPGGA,%02d%02d%05.02f,%02d%09.06f,%c,%03d%09.06f,%c,%d,%02d,%.1f,%.1f,M,,,,",
      dsat_pd_info.pd_info.pd_ext_data.hour,
      dsat_pd_info.pd_info.pd_ext_data.minute,
      (float)dsat_pd_info.pd_info.pd_ext_data.sec/100.0,
      lat_deg, lat_min, lat_ns,
      lon_deg, lon_min, lon_ew,
      dsat_pd_info.pd_info.pd_ext_data.fix_quality,
      dsat_pd_info.pd_info.pd_ext_data.num_sv_in_view,
      (float)dsat_pd_info.pd_info.pd_ext_data.h_dop/10.0,
      (float)dsat_pd_info.pd_info.pd_ext_data.altitude/10.0);

  i = 1;          /* Skip over the $ in the GPGGA message for checksum */
  chk_sum = 0;
  while (tmp_buffer->data_ptr[i])
  {
    chk_sum ^= tmp_buffer->data_ptr[i];
    i++;
  }

  res_buff_ptr->used = snprintf((char *)res_buff_ptr->data_ptr,
                                      res_buff_ptr->size, "%s*%02X",
                                      tmp_buffer->data_ptr,
                                      chk_sum);

  (void) dsm_free_buffer(tmp_buffer);
  return result;
} /* dsat707_exec_dsat_gps_info_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_GPS_STOP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$$GPS_STOP command.
  Stops position fix [seris] in progress.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_gps_stop_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  UNUSED_ARG(mode);
  UNUSED_ARG(tab_entry);
  UNUSED_ARG(tok_ptr);
  UNUSED_ARG(res_buff_ptr);

  result = dsat_gps_init();

  if (result == DSAT_OK)
  {
    if (!pdsm_end_session(dsat_pd_cmd_cb, PDSM_PD_END_SESSION_FIX_REQ,
                          NULL, dsat_pd_client_id))
    {
      DS_AT_MSG0_ERROR("Error stopping outstanding position request");
      result = DSAT_ERROR;
    }
  }
  return result;
} /* dsat707_exec_dsat_gps_stop_cmd */
#endif /* defined(FEATURE_DSAT_GPS_CMDS) */

/*===========================================================================
                       INTERNAL FUNCTION DEFINITIONS
===========================================================================*/
#if defined(FEATURE_DSAT_GPS_CMDS)
/*===========================================================================

  FUNCTION DSAT_PD_EVENT_CB

  DESCRIPTION
  Called from pdsm (position location task) when it has a position event (from 
  _ANY_ client.)

  DEPENDENCIES
  Being in pdsm task space, can only copy position results to local memory.
  No real processing can happen here!

  RETURN VALUE
  void

  SIDE EFFECTS
  Updates local copy of last position report

===========================================================================*/
/*ARGSUSED*/
LOCAL void dsat_pd_event_cb
(
  void                         *user_data,  /* user data pointer */
  pdsm_pd_event_type            pd_event,    /* Indicate PD event */
  const pdsm_pd_info_s_type    *pd_info_ptr
    /* A pointer to a PD state information struct */
)
{
  ds_cmd_type            *cmd_buf = NULL;
  ds_at_pdsm_info        *pdsm_info = NULL;

  if (pd_info_ptr == NULL)
  {
    DS_AT_MSG0_ERROR("NULL position information received!");
    return;
  }

  DS_AT_MSG6_MED("pd_event=0x%x, lat=%d, lon=%d, estatus=0x%x, fixtype=%d,alt=%d",
                 pd_event,
                 pd_info_ptr->pd_info.pd_data.lat,
                 pd_info_ptr->pd_info.pd_data.lon,
                 pd_info_ptr->pd_info.end_status,
                 pd_info_ptr->pd_info.pd_data.fix_type,
                 pd_info_ptr->pd_info.pd_data.altitude);

  cmd_buf = dsat_get_cmd_buf(sizeof(ds_at_pdsm_info), TRUE);
  pdsm_info = cmd_ptr->cmd_payload_ptr;

  if (NULL != cmd_buf)
  {
    /* send the message to ATCOP */
    cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_PD_INFO_CMD;
    pdsm_info->dsat_pd_event = pd_event;
    (void) dsatutil_memscpy((void*)&pdsm_info->dsat_pd_info,
              sizeof(pdsm_pd_info_s_type), (void*)pd_info_ptr,sizeof(pdsm_pd_info_s_type));

    ds_put_cmd(cmd_buf);
  }
} /* dsat_pd_event_cb */

/*===========================================================================
FUNCTION DSAT_PD_EVENT_HANDLER

DESCRIPTION
  This function is handler function for GPS event.

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ERROR : if there was any problem .
  DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
======================================================================*/

dsat_result_enum_type dsat_pd_event_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  ds_at_pdsm_info *pdsm_info = dsat_get_cmd_payload_ptr(cmd_ptr);

  DS_AT_MSG1_HIGH("PB Handler %d",pdsm_info->dsat_pd_event);
  
  dsat_pd_event = pdsm_info->dsat_pd_event;

  (void) dsatutil_memscpy((void*)&dsat_pd_info,sizeof(dsat_pd_info),
        (void*)&pdsm_info->dsat_pd_info,sizeof(dsat_pd_info));
  
  return DSAT_ASYNC_EVENT;
}/*dsat_pd_event_handler*/


/*===========================================================================

  FUNCTION DSAT_PD_CMD_CB

  DESCRIPTION
    Called from pdsm (position location task) after we have called
    pdsm_get_position (or similar). This will indicate if there were any
    problems with the arguments to the call, if pdsm is in a good
    state, etc. This is NOT called with the normal results of a position
    request (see dsat_pd_event_cb for that).

  DEPENDENCIES
    Being in pdsm task space, can only copy position results to local memory.
    No real processing can happen here!

  RETURN VALUE
    void

  SIDE EFFECTS
    Updates local copy of last position report

===========================================================================*/
/*ARGSUSED*/
LOCAL void dsat_pd_cmd_cb
(
  void                         *data_block_ptr,  /* client data pointer */
  pdsm_pd_cmd_e_type           pd_cmd,           /* Which command caused 
error */
  pdsm_pd_cmd_err_e_type       pd_cmd_err        /* Command error code */
)
{
  DS_AT_MSG2_MED("cmd_cb cmd=%d, err=%d", pd_cmd, pd_cmd_err);
} /* dsat_pd_event_cb */

/*===========================================================================

  FUNCTION DSAT_PA_CMD_CB

  DESCRIPTION
    Called from pdsm (position location task) after we have called
    pdsm_set_parameters (or similar). This will indicate if there were any
    problems with the arguments to the call, if pdsm is in a good
    state, etc. This is NOT called with the normal results of a position
    request (see dsat_pd_event_cb for that).

  DEPENDENCIES
    Being in pdsm task space, can only copy position results to local memory.
    No real processing can happen here!

  RETURN VALUE
    void

  SIDE EFFECTS
    Updates local copy of last position report

===========================================================================*/
/*ARGSUSED*/
LOCAL void dsat_pa_cmd_cb
(
  void                         *data_block_ptr,  /* client data pointer */
  pdsm_pa_cmd_e_type           pd_cmd,           /* Which command caused 
error */
  pdsm_pa_cmd_err_e_type       pd_cmd_err        /* Command error code */
)
{
  DS_AT_MSG2_MED("pa_cmd_cb cmd=%d, err=%d", pd_cmd, pd_cmd_err);
} /* dsat_pd_event_cb */

/*===========================================================================
  FUNCTION DSAT_GPS_INIT

  DESCRIPTION
    This function initializes all strctures associated with the PDSM for
    position determination.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    If it hasn't done so already, initalizes the internal data structures for
    using GpsOne.
    Note that once any of the Sprint GPS commands is executed we'll init and
    attach to the PDSM with no way to release.
===========================================================================*/
LOCAL dsat_result_enum_type dsat_gps_init( void )
{
#define DSAT_ACCURACY 64
#define DSAT_PERFORMANCE 30
  pdsm_client_status_e_type pd_status;
  
  /* Check to see if we've set up everything already */
  if (dsat_pd_client_id < 0)
  {
    /* Nope, init our "client"*/
    dsat_pd_client_id = pdsm_client_init(PDSM_CLIENT_TYPE_TEST2);
    if (dsat_pd_client_id < 0)
    {
      DS_AT_MSG2_ERROR("Can't set pdsm client ID, req=%d, got=%d",
                 PDSM_CLIENT_TYPE_TEST2, dsat_pd_client_id);
      return DSAT_ERROR;
    }

    /* Activate the client */
    pd_status = pdsm_client_act(dsat_pd_client_id);
    if (pd_status != PDSM_CLIENT_OK)
    {
      /* Handle failure */
      pd_status = pdsm_client_release(dsat_pd_client_id);
      DS_AT_MSG0_ERROR("Can't activate pdsm client");
      return DSAT_ERROR;
    }

    /* And register our event callback */
    pd_status = pdsm_client_pd_reg(dsat_pd_client_id,
                                   NULL,
                                   dsat_pd_event_cb,
                                   PDSM_CLIENT_EVENT_REG,
                                   (pdsm_pd_event_type)DSAT_PD_EVENTS,
                                   NULL);
    if (pd_status != PDSM_CLIENT_OK)
    {
      /* Handle failure */
      pd_status = pdsm_client_deact(dsat_pd_client_id);
      pd_status = pdsm_client_release(dsat_pd_client_id);
      DS_AT_MSG0_ERROR("Can't register client with pdsm");
      return DSAT_ERROR;
    }

    /* We're registered and set up with the PDSM, so now init our default
     * position request information in the static data structures
     */
#ifdef FEATURE_CGPS
    DS_AT_MSG0_LOW("CGPS feature is on");
    dsat_pd_option.session = PDSM_PD_SESS_TYPE_NEW;
    dsat_pd_option.operation_mode = PDSM_SESSION_OPERATION_OPTIMAL_SPEED;
    dsat_pd_option.fix_rate.num_fixes = 1;
    dsat_pd_option.fix_rate.time_between_fixes = 1;
    dsat_pd_option.lsinfo.server_option = PDSM_SERVER_OPTION_USE_DEFAULT;
    memset(&dsat_pd_option.lsinfo.server_adrs,0,
            sizeof(dsat_pd_option.lsinfo.server_adrs));
    dsat_pd_option.class_id = 0;
#else
    dsat_pd_option.session = PDSM_PD_SESS_TYPE_NEW;
    dsat_pd_option.session_info.operation_mode = 
      PDSM_SESSION_OPERATION_OPTIMAL_SPEED;
    dsat_pd_option.session_info.data_download.dload_option = 
      PDSM_DATA_DLOAD_SINGLE;
    dsat_pd_option.session_info.data_download.duration = 30; // minutes?
    dsat_pd_option.fix_rate.num_fixes = 1;
    dsat_pd_option.fix_rate.time_between_fixes = 1;
    dsat_pd_option.privacy = PDSM_PD_PRIVACY_LEVEL_0;
    /* Use the default PDE unless an override is given later */
    dsat_pd_option.lsinfo.server_option = PDSM_SERVER_OPTION_USE_DEFAULT;
    dsat_pd_option.lsinfo.server_adrs = 0;
    dsat_pd_option.lsinfo.server_port = 0;
    dsat_pd_option.class_id = 0;
    dsat_pd_option.dptr = NULL;

    dsat_pd_qos.accuracy_threshold = DSAT_ACCURACY;
    dsat_pd_qos.performance = DSAT_PERFORMANCE;
#endif
    DS_AT_MSG0_MED("dsat gps init OK");
  }
  return DSAT_OK;
}/* dsat_gps_init */

#endif /* defined(FEATURE_DSAT_GPS_CMDS) */

/*===========================================================================
  FUNCTION DSAT707_EXEC_RESET_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT^RESET command. This causes the
  device to reboot.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.

===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsat707_exec_reset_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_ERROR;
  /* Read Command */
  if ( tok_ptr->op == (NA) )
  {
    sys_oprt_mode_e_type  new_opmode = SYS_OPRT_MODE_PWROFF;

    SET_PENDING(DSAT707_VEND_ACT_RESET_IDX, 0, DSAT_PENDING_RESET_WRITE);

    result = dsatcmif_change_operating_mode(new_opmode);
    if (DSAT_ASYNC_CMD != result)
    {
      SET_PENDING(DSAT707_VEND_ACT_RESET_IDX, 0, DSAT_PENDING_RESET_NONE);
    }
  }
  return result;
} /* dsat707_exec_reset_cmd */

#endif /* FEATURE_DATA_IS707 */
