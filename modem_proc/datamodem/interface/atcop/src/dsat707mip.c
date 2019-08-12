/*===========================================================================

                   D A T A   S E R V I C E S  M I P
                A T   C O M M A N D   P R O C E S S O R

DESCRIPTION
  This module executes AT commands. It executes IS-707 specific Mobile IP
  related AT commands.

EXTERNALIZED FUNCTIONS
dsat707_exec_qcmip_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIP command.
  Enable or disable Mobile IP.

dsat707_exec_qcmipp_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPP command.
  Selects MIP user profile to be active. Takes a profile number
  between 0 and 5.

dsat707_exec_qcmipt_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPT command.
  Enable/disable the use of RFC 2002bis authentication.

dsat707_exec_qcmipep_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPEP command.
  Enable/disable currently active profile.

dsat707_exec_qcmipmass_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMASS command.

dsat707_exec_qcmipmhss_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMHSS command.
  Set MN-HA shared secrets for the currently active profile.

dsat707_exec_qcmipmaspi_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMASPI command.
  Set MN-AAA SPIs for the currently active profile.

dsat707_exec_qcmipmhspi_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMHSPI command.
  Set MN-HA SPIs for the currently active profile.

dsat707_exec_qcmiprt_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPRT command.
  Set the reverse tunneling for currently active profile.

dsat707_exec_qcmipnai_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPNAI command.
  Set the NAI for the currently active profile.

dsat707_exec_qcmipha_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPHA command.

dsat707_exec_qcmippha_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPPHA command.

dsat707_exec_qcmipsha_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPSHA command.

dsat707_exec_qcmipgetp_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPGETP command.
  Return all information corresponding to the specified profile
  number.

  Copyright (c) 1995 - 2014 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MM_DATA/vcs/dsat707mip.c_v   1.10   07 Feb 2003 10:22:40   sramacha  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707mip.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/11/14   tk      Optimized debug macros usage in ATCoP.
06/10/13   pg      Fixed NV read error in MIP commands
05/18/12   tk      Migrated to MSG 2.0 macros
07/22/11   dvk     Global Variable Cleanup
02/24/11   ash     Cleaning up global variables for Q6 free floating effort.
02/11/11   ack     Global Variable Cleanup
01/11/11   ad      Fixed security vulnerabilities in 1X.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
05/10/10   kk      Mainlining pending GOBI changes.
01/11/10   ms      Replaced UIM_3GPD Constants with NVRUIM_3GPD.
12/10/09   ps      Mainlining OMH feature based on new profile management
                   design.
10/15/09   vs      SU Level Api Defeaturization
09/04/09   ss      CMI SU level modifications.
12/12/08   ua      Fixed Off target lint errors.
08/06/08   bs      Merging $QCMIPP fix from branch.
08/11/08   bs      Fixed Klocwork criticals.
07/21/08   bs      Added support for CDMA NAARefresh.
04/09/08   bs      Fixed Klocwork errors.
01/17/08   mga     Merged a minor comment change
06/27/07   sa      Fixed Banned API error in dsat707_qcmipgetp()
05/10/07   ac      Lint fix
03/20/07   as      Fixed Lint error and banned apis
08/30/06   jd      Don't call MIP gen key data from AT$QCMIPP unless sec has
                   indicated random data source has been filled.
10/28/05   as/vrk  added include for dsmip_metasm.h
11/01/04   jd/kvd  Fixed bug in computing len in hex_str_to_bin
                   corrected a mistake in previous checkin
10/26/04   jd/kvd  Corrected NV writing, when MIP cmds are issued through AT.
05/18/04   jk      Added support for using the R-UIM card for 3G data
                   authentication.
05/16/04   vr      Moved uint32_to_str and str_to_uint32 to dsat707util.c
04/28/04   jd      AT$QCMIPM[HA]SSX can set > 4 byte secret, bug fixes.
01/19/04   jd      Send command to reencrypt DMU keys when QCMIPP changes.
                   Code cleanup (removed tabs and fixed some formatting)
05/05/03   jd      Removed dependency on dsiface_def.h
02/07/03   rsl     Don't call exec_param in mipha, mipsha, mippha, mipmass, 
                   mipmhss, mipnai.
12/06/02   ak      Added some char * typecasts so that strcpy wouldn't
                   bomb.
12/02/02   ak      Made set_mdr_val and set_crm_val external functions.
11/14/02   jd      Removed prototypes for dsat707_erase_shadow_nv and 
                   dsat707_fill_mip_vals, reference to qcmip_mode
                   (no longer used)
09/17/02   jay     Added break statements in QCMIPRT processing.
07/12/02   ak      Function to check MDR now a global function.  In exec_
                   _qcmip, call ds707_pkt_so_recal(), after changing qcmdr.
04/03/02   rsl     Initial version of file.

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "msg.h"
#include "dsmip_api.h"
#include "dsatapi.h"
#include "dsat_v.h"
#include "dsat707mip.h"

#ifdef FEATURE_DS_MOBILE_IP

#include "dstaski.h"
#include "ds707.h"
#include "ds707_so_pkt.h"
#include "ps_mip_task.h"
#include "ps_svc.h"
#include "ps_ppp_fsm.h"
#include "ps_utils.h"
#include "dsmip_v.h"
#include "dsmip_dmu.h"

#include "dsati.h"
#include "dsat707mipctab.h"
#include "dsat707vendctab.h"
#include "dsat707extctab.h"
#include "dsat707mip.h"
#include "dsatparm.h"
#include "dsatctab.h"
#include "dsat707util.h"
#include "ds_1x_profile.h"
#include "ds707_pkt_mgr.h"

#include <stringl/stringl.h>

#ifdef FEATURE_DS_MOBILE_IP_DMU
#include "dsmip_metasm.h"
#endif /* FEATURE_DS_MOBILE_IP_DMU */

#include "nvruimi.h"
#include "uim_v.h"
#endif /* FEATURE_DS_MOBILE_IP */
#ifdef FEATURE_DS_MOBILE_IP
/*===========================================================================
                                 LOCAL DATA
===========================================================================*/
/*---------------------------------------------------------------------------
  Global nv_status for the many nv accesses herein
---------------------------------------------------------------------------*/
LOCAL nv_stat_enum_type  atmip_nv_status = (nv_stat_enum_type)0;
/*---------------------------------------------------------------------------
  Range query strings for Mobile IP specific AT commands
---------------------------------------------------------------------------*/
LOCAL const dsat_string_item_type mip_string_range [] =
              "(20,21,23-7E),(0-1)";

LOCAL const dsat_string_item_type mip_hex_string_range [] =
              "[0x00-0xFF] (max 16 bytes), 0-1";

LOCAL const dsat_string_item_type mip_ip_string_range [] =
              "((0-255).(0-255).(0-255).(0-255)),(0-1)";
/*Macro defination for MIP commands*/

#define DSAT_VALIDATE_SET_MIP(mip_val_type,dflm) \
    ( ((tok_ptr->op == NA)|| ((tok_ptr->op == (NA|EQ|AR)) && \
      (tok_ptr->args_found == 1 && (VALID_TOKEN(0)) && \
       ATOI_OK == dsatutil_atoi(&temp_val,tok_ptr->arg[0],10) && (temp_val <= dflm))))&& \
      (TRUE == dsmip_set_1x_profile_val_num(mip_val_type,&temp_val,0))) \

/*===========================================================================
                           FORWARD DECLARATIONS
===========================================================================*/

LOCAL byte* dsat707_qcmipgetp
(
  byte *rb_ptr,
  uint16   size,
  byte profile
);

LOCAL dsat_result_enum_type ds_at707_mip_ip
(
  const tokens_struct_type  *tok_ptr,
  byte                      *rb_ptr,
  const dsati_cmd_type      *table_entry,
  dsat_string_item_type     *ds_atcop_qcmip_ip_val,
  uint32                    *ds_atcop_ip_uint32val,
  uint32                    *shadow_nv_ip,
  boolean                   *shadow_nv_ip_set
);

LOCAL dsat_result_enum_type ds_at707_mip_ss
(
  const tokens_struct_type  *tok_ptr,
  const dsati_cmd_type      *table_entry,
  dsat_string_item_type     *ds_atcop_qcmip_ss_val,
  byte                      *shadow_nv_ss,
  byte                      *shadow_nv_ss_len,
  dsat_num_item_type         shadow_nv_ss_max_len
);

LOCAL dsat_result_enum_type ds_at707_mip_spi
(
  const dsati_cmd_type  *table_entry,
  uint32                *shadow_spi,
  byte                  *shadow_spi_set_flag
);

LOCAL dsat_result_enum_type ds_at707_mip_ssx
(
  const tokens_struct_type  *tok_ptr,
  const dsati_cmd_type      *table_entry,
  dsat_string_item_type     *ds_atcop_qcmip_ssx_val,
  byte                      *shadow_nv_ssx,
  byte                      *shadow_nv_ssx_len,
  dsat_num_item_type         shadow_nv_ssx_max_len
);
/*===========================================================================
                          INTERNAL FUNCTIONS
===========================================================================*/

#ifdef FEATURE_DS_AT_TEST_ONLY
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIP_CMD

  DESCRIPTION
  Exec functions take the result from the command line parser
  and execute the command. 

  This is the exec function for AT$QCMIP command.

  AT$QCMIP enables or disables Mobile IP.
  0 - Mobile IP disabled, Simple IP only.
  1 - Mobile IP preferred.
  2 - Mobile IP only.

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
dsat_result_enum_type dsat707_exec_qcmip_cmd
(
  dsat_mode_enum_type      mode,           /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,     /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr         /*  Place to put response       */
)
{
  dsat_num_item_type    temp_val = 0; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (tok_ptr->op == (NA|EQ|AR) || (tok_ptr->op == NA)) 
  {
    if(FALSE == DSAT_VALIDATE_SET_MIP(DS_QCMIP_VAL,2))
    {
      DS_AT_MSG1_ERROR("$QCMIP: Invalid Number of arguements  %d", tok_ptr->args_found);
      return DSAT_ERROR;
    }
    ds_nv_item.ds_qcmip = (byte) temp_val;
    (void) dsatutil_put_nv_item( NV_DS_QCMIP_I, &ds_nv_item );
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                           rb_ptr->size,
                                          "$QCMIP: (0 - 2)\n");
  }
  else  if(tok_ptr->op == (NA|QU))
  {
     dsmip_get_1x_profile_val_num(DS_QCMIP_VAL,(void *)&temp_val,0);
     rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                            rb_ptr->size,
                                            "$QCMIP: %d\n",
                                            temp_val);
  }
  else
  {
    /* Not Supported */
    return DSAT_ERROR;
  }
  return DSAT_OK;
} /* dsat707_exec_qcmip_cmd */
#endif /* FEATURE_DS_AT_TEST_ONLY */
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPP command.
  Selects MIP user profile to be active. Takes a profile number
  between 0 and 5.

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
dsat_result_enum_type dsat707_exec_qcmipp_cmd
(
  dsat_mode_enum_type      mode,           /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,     /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr         /*  Place to put response       */
)
{
  dsat_num_item_type    temp_val = 0; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (tok_ptr->op == (NA|EQ|AR) || (tok_ptr->op == NA)) 
  {
    if(FALSE == DSAT_VALIDATE_SET_MIP(DS_QCMIPP_VAL,5))
    {
      DS_AT_MSG1_ERROR("$QCMIPP: Invalid Number of arguements  %d", tok_ptr->args_found);
      return DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                           rb_ptr->size,
                                           "$QCMIPP: (0 - 5)\n");
  }
  else  if(tok_ptr->op == (NA|QU))
  {
     dsmip_get_1x_profile_val_num(DS_QCMIPP_VAL,(void *)&temp_val,0);
     rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                            rb_ptr->size,
                                            "$QCMIPP: %d\n",
                                            temp_val);
  }
  else
  {
    /* Not Supported */
    return DSAT_ERROR;
  }
  return DSAT_OK;
} /* dsat707_exec_qcmipp_cmd */

#ifdef FEATURE_DS_AT_TEST_ONLY
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPT_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPT command.
  Enable/disable the use of RFC 2002bis authentication.
  0 - Use of RFC 2002bis authentication is disabled. RFC 2002
  style authentication is used instead.
  1 - Use of RFC 2002bis authentication is enabled.

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
dsat_result_enum_type dsat707_exec_qcmipt_cmd
(
  dsat_mode_enum_type      mode,           /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,     /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr         /*  Place to put response       */
)
{
  dsat_num_item_type    temp_val = 0; 
  
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /*-----------------------------------------------------------------------
    If Using R-UIM, 2002bis can be read from the card.   
    -----------------------------------------------------------------------*/
    if((NVRUIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
        NVRUIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()) &&
       ((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
    {
      /* Do not Allow the 2002bis variable to be written */
      return DSAT_ERROR;
    }
  if (tok_ptr->op == (NA|EQ|AR) || (tok_ptr->op == NA)) 
  {
    if(FALSE == DSAT_VALIDATE_SET_MIP(DS_QCMIPT_VAL,1))
    {
      DS_AT_MSG1_ERROR("$QCMIPT: Invalid Number of arguements  %d", tok_ptr->args_found);
      return DSAT_ERROR;
    }
    ds_nv_item.ds_mip_2002bis_mn_ha_auth = (byte) temp_val;
    (void) dsatutil_put_nv_item( NV_DS_MIP_2002BIS_MN_HA_AUTH_I, &ds_nv_item );
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                           rb_ptr->size,
                                           "$QCMIPT: (0 - 1)\n");
  }
  else  if(tok_ptr->op == (NA|QU))
  {
     dsmip_get_1x_profile_val_num(DS_QCMIPT_VAL,(void *)&temp_val,0);
     rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                            rb_ptr->size,
                                            "$QCMIPT: %d\n",
                                            temp_val);
  }
  else
  {
    /* Not Supported */
    return DSAT_ERROR;
  }
  return DSAT_OK;
} /* dsat707_exec_qcmipt_cmd */
#endif /* FEATURE_DS_AT_TEST_ONLY */
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPEP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPEP command.
  Enable/disable currently active profile.
  0 - Disable the currently active profile.
  1 - Enable the currently active profile.

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
dsat_result_enum_type dsat707_exec_qcmipep_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
)
{
  dsat_num_item_type temp_val = 0; 
  uint8              temp_index =0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /*-----------------------------------------------------------------------
    If Using R-UIM, all profiles are always enabled.   
    -----------------------------------------------------------------------*/
    if((NVRUIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
        NVRUIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()) &&
       ((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
    {
      /* Do not Allow the Enable Profile variable to be written */
      return DSAT_ERROR;
    }
  if (tok_ptr->op == (NA|EQ|AR) || (tok_ptr->op == NA)) 
  {
    if(FALSE == DSAT_VALIDATE_SET_MIP(DS_QCMIPEP_VAL,1))
    {
      DS_AT_MSG1_ERROR("$QCMIPT: Invalid Number of arguements  %d", tok_ptr->args_found);
      return  DSAT_ERROR;
    }
    (void) dsatutil_get_nv_item(NV_DS_MIP_ENABLE_PROF_I,&ds_nv_item);
    dsmip_get_1x_profile_val_num(DS_QCMIPP_VAL,(void *)&temp_index,0);
    ds_nv_item.ds_mip_enable_prof[temp_index ] = (byte) temp_val;
    (void) dsatutil_put_nv_item(NV_DS_MIP_ENABLE_PROF_I,&ds_nv_item);
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                           rb_ptr->size,
                                           "$QCMIPEP: (0 - 1)\n");
  }
  else  if(tok_ptr->op == (NA|QU))
  {
     dsmip_get_1x_profile_val_num(DS_QCMIPEP_VAL,(void *)&temp_val,0);
     rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                            rb_ptr->size,
                                            "$QCMIPEP: %d\n",
                                            temp_val);
  }
  else
  {
    /* Not Supported */
    return DSAT_ERROR;
  }
  return DSAT_OK;

 } /* dsat707_exec_qcmipep_cmd */
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMASS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMASS command.

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
dsat_result_enum_type dsat707_exec_qcmipmass_cmd
(
  dsat_mode_enum_type      mode,         /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,   /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,     /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr       /*  Place to put response       */
)
{
  /*lint -esym(715,mode)*/
  byte                   temp_ss[MIP_MN_AAA_SS_MAX_LEN+1];
  dsat_result_enum_type  result = DSAT_OK;
  boolean                set;
  dsat_num_item_type     temp_val; 
  dsmip_shadow_nv_type  *temp_shadow_nv = NULL;
 /* -- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  switch (tok_ptr->op)
  {
    case (NA|EQ|QU):  /* ---------------     query range    ------------- */
      dsatutil_fmt_string_range(tab_entry,
                                rb_ptr->data_ptr,
                                mip_string_range);
      break;

    case (NA|EQ|AR):  /* --------------      set value     -------------- */
      /*------------------------------------------------------------
      If Using R-UIM, Cannot set SS   
      ------------------------------------------------------------*/
      if((NVRUIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
          NVRUIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()))
      {
        return DSAT_ERROR;
      }
      temp_shadow_nv = (dsmip_shadow_nv_type *)dsmip_get_shadow_nv_ptr();
      if(NULL == temp_shadow_nv)
      {
        return DSAT_ERROR;
      }
      result = ds_at707_mip_ss( tok_ptr,
                                tab_entry,
                                temp_ss,
                                temp_shadow_nv->mn_aaa_ss.value,
                                &temp_shadow_nv->mn_aaa_ss.len,MIP_MN_AAA_SS_MAX_LEN );
        /*------------------------------------------------------------------
          Copy the value back to the  associated at cmd value
        ------------------------------------------------------------------*/

      if( result == DSAT_OK && 0 != dsmip_set_1x_profile_val_str(DS_QCMIPMASS_VAL, (char*)temp_ss, MIP_MN_AAA_SS_MAX_LEN))
      {
        result = DSAT_OK;
      }else
      {
        result = DSAT_ERROR;
      }
      break;

    case (NA|QU):     /* --------------     query value    -------------- */
      set = FALSE;
      temp_shadow_nv = (dsmip_shadow_nv_type *)dsmip_get_shadow_nv_ptr();
      if(NULL == temp_shadow_nv)
      {
        return DSAT_ERROR;
      }
      /*--------------------------------------------------------------------
        If shadow password length is non-zero, secret is set
      --------------------------------------------------------------------*/
      if (temp_shadow_nv->mn_aaa_ss.len)
      {
        set = TRUE; 
      }
      else
      {
        if(NVRUIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
           NVRUIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support())
        {
          /*------------------------------------------------------------------
            The MN-AAA SS is set on the card, just set it to a non-zero value
          ------------------------------------------------------------------*/
          set = TRUE;
        }
        else
        {
          /*----------------------------------------------------------------
            Check if password is set in real NV - later check return value
          ----------------------------------------------------------------*/
          dsmip_get_1x_profile_val_num(DS_QCMIPP_VAL,(void *)&temp_val,0);
          ds_nv_item.ds_mip_ss_user_prof.index  =  (byte) temp_val;
          (void) dsatutil_get_nv_item(NV_DS_MIP_SS_USER_PROF_I,&ds_nv_item);
        } 
        if( ds_nv_item.ds_mip_ss_user_prof.mn_aaa_shared_secret_length != 0)
        {
          set = TRUE; 
        }
      }
      if (set)
      {
        (void) strlcpy( 
            (char *) (rb_ptr->data_ptr), 
            "$QCMIPMASS: Set", 
            rb_ptr->size);
      }
      else
      {
        (void) strlcpy( 
            (char *) (rb_ptr->data_ptr), 
            "$QCMIPMASS: Unset", 
            rb_ptr->size);
      }
      break;

    case (NA):        /* ---------------  set default value -------------- */
    default:
      result = DSAT_ERROR;
      break;
  }

  rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_qcmipmass_cmd */
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMHSS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMHSS command.
  Set MN-HA shared secrets for the currently active profile.

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
dsat_result_enum_type dsat707_exec_qcmipmhss_cmd
(
  dsat_mode_enum_type      mode,          /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr        /*  Place to put response       */
)
{
  /*lint -esym(715,mode)*/
  byte                   temp_ss[MIP_MN_HA_SS_MAX_LEN+1];
  dsat_result_enum_type  result = DSAT_OK;
  boolean                set;
  dsat_num_item_type     temp_val; 
  dsmip_shadow_nv_type  *temp_shadow_nv = NULL;
 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - - - */
  switch (tok_ptr->op)
  {
    case (NA|EQ|QU):  /* ---------------     query range    -------------- */
      dsatutil_fmt_string_range(tab_entry,
                                rb_ptr->data_ptr,
                                mip_string_range);
      break;

    case (NA|EQ|AR):  /* ---------------      set value     -------------- */
      /*------------------------------------------------------------
      If Using R-UIM, Cannot set SS   
      ------------------------------------------------------------*/
      if((NVRUIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
          NVRUIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()))
      {
        return DSAT_ERROR;
      }
      temp_shadow_nv = (dsmip_shadow_nv_type *)dsmip_get_shadow_nv_ptr();
      if(NULL == temp_shadow_nv)
      {
        return DSAT_ERROR;
      }
      result = ds_at707_mip_ss( tok_ptr,
                                tab_entry,
                                temp_ss,
                                temp_shadow_nv->mn_ha_ss.value,
                                &temp_shadow_nv->mn_ha_ss.len,MIP_MN_HA_SS_MAX_LEN );
     
      if( result ==  DSAT_OK && 0 != dsmip_set_1x_profile_val_str(DS_QCMIPMHSS_VAL,(char*)temp_ss,MIP_MN_HA_SS_MAX_LEN))
      {
        result = DSAT_OK;
      }
      else
      {
        result = DSAT_ERROR;
      }
      break;
    case (NA|QU):     /* ---------------     query value    -------------- */
      set = FALSE;
      temp_shadow_nv = (dsmip_shadow_nv_type *)dsmip_get_shadow_nv_ptr();
      if(NULL == temp_shadow_nv)
      {
        return DSAT_ERROR;
      }
      /*---------------------------------------------------------------------
        If shadow password length is non-zero, secret is set
      ---------------------------------------------------------------------*/
      if (temp_shadow_nv->mn_ha_ss.len)
      {
        set = TRUE; 
      }
      else
      {
        if(NVRUIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
           NVRUIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support())
        {
          /*-----------------------------------------------------------------
            The MN-HA SS is set on the card, just set it to a non-zero value
          -----------------------------------------------------------------*/
          set = TRUE;
        }
        else
        {
          /*-----------------------------------------------------------------
            Check if password is set in real NV - later, check return value
          -----------------------------------------------------------------*/
          dsmip_get_1x_profile_val_num(DS_QCMIPP_VAL,(void *)&temp_val,0);
          ds_nv_item.ds_mip_ss_user_prof.index = (byte) temp_val;
          (void) dsatutil_get_nv_item(NV_DS_MIP_SS_USER_PROF_I,&ds_nv_item);
        }
        if( ds_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret_length != 0)
        {
          set = TRUE; 
        }
      }
      if (set)
      {
        (void) strlcpy( 
            (char *) (rb_ptr->data_ptr), 
            "$QCMIPMHSS: Set", 
            rb_ptr->size);
      }
      else
      {
        (void) strlcpy( 
            (char *) (rb_ptr->data_ptr), 
            "$QCMIPMHSS: Unset", 
            rb_ptr->size);
      }
      break;

    case (NA):        /* ---------------  set default value -------------- */
    default:
      result = DSAT_ERROR;
      break;
  }
  rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_qcmipmhss_cmd */
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMASPI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMASPI command.
  Set MN-AAA SPIs for the currently active profile.

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
dsat_result_enum_type dsat707_exec_qcmipmaspi_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsmip_shadow_nv_type  *temp_shadow_nv = NULL;
  dsat_num_item_type     temp_val[2] = {2,0};
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /*------------------------------------------------------------
    If Using R-UIM, Cannot set SPI  
    ------------------------------------------------------------*/
    if((NVRUIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
        NVRUIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()) &&
       ((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
    {
      return DSAT_ERROR;
    }
  if (tok_ptr->op == (NA|EQ|AR) || (tok_ptr->op == NA)) 
  {
    if(tok_ptr->op == (NA|EQ|AR))
    {
      if(tok_ptr->args_found == 0 || tok_ptr->args_found > 2)
      {
        result = DSAT_ERROR;
      }
      else
      {
        if((VALID_TOKEN(0)) && 
            (ATOI_OK != dsatutil_atoi(&temp_val[0],tok_ptr->arg[0],10)))
        {
          result = DSAT_ERROR;
        }
        if( (VALID_TOKEN(1)) && 
            (ATOI_OK != dsatutil_atoi(&temp_val[1],tok_ptr->arg[1],10)||
             temp_val[1] > 1))
        {
          result = DSAT_ERROR;
        }
      }
    }
  if( (result == DSAT_OK) &&
        (TRUE == dsmip_set_1x_profile_val_num(DS_QCMIPMASPI_VAL,(void *)temp_val,2)) )
    {
      temp_shadow_nv = (dsmip_shadow_nv_type *)dsmip_get_shadow_nv_ptr();
      if(NULL == temp_shadow_nv)
  {
        return DSAT_ERROR;
      }
    result = ds_at707_mip_spi( tab_entry,
                                 &temp_shadow_nv->mn_aaa_spi.value,
                                 &temp_shadow_nv->mn_aaa_spi.set );
    }else
    {
      DS_AT_MSG1_ERROR("$QCMIPMASPI: Failed to update profile,arg_found %d", tok_ptr->args_found);
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                           rb_ptr->size,
                                           "$QCMIPMASPI: (0 - 4294967295),(0 - 1)\n");
  }
  else  if(tok_ptr->op == (NA|QU))
  {
     dsmip_get_1x_profile_val_num(DS_QCMIPMASPI_VAL,(void *)temp_val,2);
     rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                            rb_ptr->size,
                                            "$QCMIPMASPI: %d,%d\n",
                                            temp_val[0],
                                            temp_val[1]);
  }
  else
  {
    /* Not Supported */
    return DSAT_ERROR;
  }
  return result;
} /* dsat707_exec_qcmipmaspi_cmd */
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMHSPI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMHSPI command.
  Set MN-HA SPIs for the currently active profile.

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
dsat_result_enum_type dsat707_exec_qcmipmhspi_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsmip_shadow_nv_type  *temp_shadow_nv = NULL;
  dsat_num_item_type     temp_val[2] = {3,0};
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /*------------------------------------------------------------
    If Using R-UIM, Cannot set SPI  
    ------------------------------------------------------------*/
    if((NVRUIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
        NVRUIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()) &&
       ((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
    {
      return DSAT_ERROR;
    }
  if (tok_ptr->op == (NA|EQ|AR) || (tok_ptr->op == NA)) 
  {
    if(tok_ptr->op == (NA|EQ|AR))
    {
      if(tok_ptr->args_found == 0 || tok_ptr->args_found > 2)
      {
        result = DSAT_ERROR;
      }
      else
      {
        if((VALID_TOKEN(0)) && 
            (ATOI_OK != dsatutil_atoi(&temp_val[0],tok_ptr->arg[0],10)))
        {
          result = DSAT_ERROR;
        }
        if( (VALID_TOKEN(1)) && 
          ( ATOI_OK != dsatutil_atoi(&temp_val[1],tok_ptr->arg[1],10) ||
            temp_val[1] > 1))
        {
          result = DSAT_ERROR;
        }
      }
    }
  if ( (result == DSAT_OK) && 
       (TRUE == dsmip_set_1x_profile_val_num(DS_QCMIPMHSPI_VAL,(void 
*)temp_val,2)) )
  {
      temp_shadow_nv = (dsmip_shadow_nv_type *)dsmip_get_shadow_nv_ptr();
      if(NULL == temp_shadow_nv)
  {
        return DSAT_ERROR;
      }
    result = ds_at707_mip_spi(tab_entry,
                                &temp_shadow_nv->mn_ha_spi.value,
                                &temp_shadow_nv->mn_ha_spi.set );
    }else
    {
      DS_AT_MSG1_ERROR("$QCMIPMHSPI: Failed to update profile,arg_found %d", tok_ptr->args_found);
      result = DSAT_ERROR;
    }
    
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                           rb_ptr->size,
                                           "$QCMIPMHSPI: (0 - 4294967295),(0 - 1)\n");
  }
  else  if(tok_ptr->op == (NA|QU))
  {
     dsmip_get_1x_profile_val_num(DS_QCMIPMHSPI_VAL,(void *)&temp_val,2);
     rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                            rb_ptr->size,
                                            "$QCMIPMHSPI: %d,%d\n",
                                            temp_val[0],
                                            temp_val[1]);
  }
  else
  {
    /* Not Supported */
    return DSAT_ERROR;
  }
  return result;
} /* dsat707_exec_qcmipmhspi_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMASSX_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMASSX command.

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
dsat_result_enum_type dsat707_exec_qcmipmassx_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
)
{
  /*lint -esym(715,mode)*/
  byte                   temp_ss[MIP_MN_AAA_SS_MAX_LEN*2+1];
  dsat_result_enum_type  result = DSAT_OK;
  byte *                 sresult = rb_ptr->data_ptr;
  boolean                set;
  dsat_num_item_type     temp_val; 
  dsmip_shadow_nv_type  *temp_shadow_nv = NULL;
 /* - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */
  switch (tok_ptr->op)
  {
    case (NA|EQ|QU):  /* ---------------     query range    -------------- */
      dsatutil_fmt_string_range(tab_entry,
                                rb_ptr->data_ptr,
                                mip_hex_string_range);
      break;

    case (NA|EQ|AR):  /* ---------------      set value     -------------- */
      /*------------------------------------------------------------
      If Using R-UIM, Cannot set SS   
      ------------------------------------------------------------*/
      if((NVRUIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
          NVRUIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()))
      {
        return DSAT_ERROR;
      }
      temp_shadow_nv = (dsmip_shadow_nv_type *)dsmip_get_shadow_nv_ptr();
      if(NULL == temp_shadow_nv)
      {
        return DSAT_ERROR;
      }
      result = ds_at707_mip_ssx( tok_ptr,
                                 tab_entry,
                                 temp_ss,
                                 temp_shadow_nv->mn_aaa_ss.value,
                                 &temp_shadow_nv->mn_aaa_ss.len,
                                 MIP_MN_AAA_SS_MAX_LEN
                                 );
      if( result ==  DSAT_OK && 0 != dsmip_set_1x_profile_val_str(DS_QCMIPMASSX_VAL,(char*)temp_ss,MIP_MN_AAA_SS_MAX_LEN))
      {
        result = DSAT_OK;
      }
      else
      {
        result = DSAT_ERROR;
      }
      break;

    case (NA|QU):     /* ---------------     query value    -------------- */
      set = FALSE;
      temp_shadow_nv = (dsmip_shadow_nv_type *)dsmip_get_shadow_nv_ptr();
      if(NULL == temp_shadow_nv)
      {
        return DSAT_ERROR;
      }
      if (temp_shadow_nv->mn_aaa_ss.len)
      {
        set = TRUE; 
      }
      else
      {
        if(NVRUIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
           NVRUIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support())
        {
          /*-----------------------------------------------------------------
            The MN-AAA SS is set on the card, just set it to a non-zero value
          -----------------------------------------------------------------*/
          set = TRUE;
        }
        else
        {
          dsmip_get_1x_profile_val_num(DS_QCMIPP_VAL,(void *)&temp_val,0);
          ds_nv_item.ds_mip_ss_user_prof.index =(byte) temp_val;
          /* later - check return value */
          atmip_nv_status = dsatutil_get_nv_item( NV_DS_MIP_SS_USER_PROF_I,
                                                  &ds_nv_item );
          if((atmip_nv_status != NV_DONE_S) && (atmip_nv_status != NV_NOTACTIVE_S))
          {
            return DSAT_ERROR;
          }
        }
        if( ds_nv_item.ds_mip_ss_user_prof.mn_aaa_shared_secret_length != 0)
        {
          set = TRUE; 
        }
      }
      if (set)
      {
        (void) strlcpy(
            (char *)sresult,
            "$QCMIPMASSX: Set",
            rb_ptr->size);
      }
      else
      {
        (void) strlcpy(
            (char *)sresult,
            "$QCMIPMASSX: Unset", 
            rb_ptr->size);
      }
      break;

    case (NA):        /* ---------------  set default value -------------- */
    default:
      result = DSAT_ERROR;
      break;
  }

  rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_qcmipmassx_cmd */
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMHSSX_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMHSSX command.

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
dsat_result_enum_type dsat707_exec_qcmipmhssx_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
)
{
  /*lint -esym(715,mode)*/
  byte                   temp_ss[MIP_MN_HA_SS_MAX_LEN*2+1];
  dsat_result_enum_type  result= DSAT_OK;
  byte*                  sresult = rb_ptr->data_ptr;
  boolean                set;
  dsat_num_item_type     temp_val; 
  dsmip_shadow_nv_type  *temp_shadow_nv = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  switch (tok_ptr->op)
  {
    case (NA|EQ|QU):  /* ---------------     query range    -------------- */
      dsatutil_fmt_string_range(tab_entry,
                                rb_ptr->data_ptr,
                                mip_hex_string_range);
      break;

    case (NA|EQ|AR):  /* ---------------      set value     -------------- */
      /*------------------------------------------------------------
      If Using R-UIM, Cannot set SS   
      ------------------------------------------------------------*/
      if((NVRUIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
          NVRUIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()))
      {
        return DSAT_ERROR;
      }
      temp_shadow_nv = (dsmip_shadow_nv_type *)dsmip_get_shadow_nv_ptr();
      if(NULL == temp_shadow_nv)
      {
        return DSAT_ERROR;
      }
      result = ds_at707_mip_ssx( tok_ptr,
                                 tab_entry,
                                 temp_ss,
                                 temp_shadow_nv->mn_ha_ss.value,
                                 &temp_shadow_nv->mn_ha_ss.len,
                                 MIP_MN_HA_SS_MAX_LEN);
      if( result ==  DSAT_OK && 0 != dsmip_set_1x_profile_val_str(DS_QCMIPMHSSX_VAL,(char*)temp_ss,MIP_MN_HA_SS_MAX_LEN))
      {
        result = DSAT_OK;
      }
      else
      {
        result = DSAT_ERROR;
      }
      break;

    case (NA|QU):     /* ---------------     query value    -------------- */
      set = FALSE;
      temp_shadow_nv = (dsmip_shadow_nv_type *)dsmip_get_shadow_nv_ptr();
      if(NULL == temp_shadow_nv)
      {
        return DSAT_ERROR;
      }
      if (temp_shadow_nv->mn_ha_ss.len)
      {
        set = TRUE; 
      }
      else
      {
        if(NVRUIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
           NVRUIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support())
        {
          /*-----------------------------------------------------------------
            The MN-HA SS is set on the card, just set it to a non-zero value
          -----------------------------------------------------------------*/
          set = TRUE;
        }
        else
        {
          dsmip_get_1x_profile_val_num(DS_QCMIPP_VAL,(void *)&temp_val,0);
          ds_nv_item.ds_mip_ss_user_prof.index  = (byte) temp_val;
          /* later - check return value */
          atmip_nv_status = dsatutil_get_nv_item( NV_DS_MIP_SS_USER_PROF_I,
                                                  &ds_nv_item );
          if((atmip_nv_status != NV_DONE_S) && (atmip_nv_status != NV_NOTACTIVE_S))
          {
            return DSAT_ERROR;
          }
        }
        if( ds_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret_length != 0)
        {
          set = TRUE; 
        }
      }
      if (set)
      {
        (void) strlcpy(
            (char *)sresult,
            "$QCMIPMHSSX: Set", 
            rb_ptr->size);
      }
      else
      {
        (void) strlcpy(
            (char *)sresult,
            "$QCMIPMHSSX: Unset", 
            rb_ptr->size);
      }
      break;

    case (NA):        /* ---------------  set default value -------------- */
    default:
      result = DSAT_ERROR;
      break;
  }

  rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_qcmipmhspi_cmd */
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPRT_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPRT command.
  Set the reverse tunneling for currently active profile.

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
dsat_result_enum_type dsat707_exec_qcmiprt_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
)
{
 dsat_result_enum_type result = DSAT_OK;
 dsmip_shadow_nv_type  *temp_shadow_nv = NULL;
 dsat_num_item_type     temp_val[2] = {0,0};

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /*-----------------------------------------------------------------------
    If Using R-UIM, RT is not writeable.   
    -----------------------------------------------------------------------*/
    if((NVRUIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
        NVRUIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()) &&
       ((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
    {
      /* Do not Allow the RT variable to be written */
      return DSAT_ERROR;
    }
  if (tok_ptr->op == (NA|EQ|AR) || (tok_ptr->op == NA)) 
  {
    if(tok_ptr->op == (NA|EQ|AR))
    {
      if(tok_ptr->args_found == 0 || tok_ptr->args_found > 2)
      {
        result = DSAT_ERROR;
      }
      else
      {
        if((VALID_TOKEN(0)) && 
            (ATOI_OK != dsatutil_atoi(&temp_val[0],tok_ptr->arg[0],10)||
            temp_val[0] > 1))
        {
          result = DSAT_ERROR;
        }
        if( (VALID_TOKEN(1)) && 
            ( ATOI_OK != dsatutil_atoi(&temp_val[1],tok_ptr->arg[1],10)||
            temp_val[1] > 1))
        {
          result = DSAT_ERROR;
        }
      }
    }
  if ( (result == DSAT_OK) && 
         (TRUE == dsmip_set_1x_profile_val_num(DS_QCMIPRT_VAL,(void *)temp_val,2)) )
  {
      temp_shadow_nv = (dsmip_shadow_nv_type *)dsmip_get_shadow_nv_ptr();
      if(NULL == temp_shadow_nv)
      {
        return DSAT_ERROR;
      }
      switch(temp_val[0])
    {
      /*---------------------------------------------------------------------
        Dont Commit to NV
      ---------------------------------------------------------------------*/
      case 0:
          temp_shadow_nv->rev_tun.set = TRUE;
          temp_shadow_nv->rev_tun.value = (byte) temp_val[0];
        break;
      /*---------------------------------------------------------------------
        Commit to NV: later - check return value
      ---------------------------------------------------------------------*/
      case 1:
          temp_shadow_nv->rev_tun.set = FALSE;
          dsmip_get_1x_profile_val_num(DS_QCMIPP_VAL,
          	                            (void *)&(ds_nv_item.ds_mip_gen_user_prof.index),0);
        (void) dsatutil_get_nv_item(NV_DS_MIP_GEN_USER_PROF_I,&ds_nv_item);

          ds_nv_item.ds_mip_gen_user_prof.rev_tun_pref =((dsat_num_item_type)temp_val[0] ? TRUE : FALSE);
        atmip_nv_status = dsatutil_put_nv_item( NV_DS_MIP_GEN_USER_PROF_I, 
                                                &ds_nv_item );
        if( atmip_nv_status != NV_DONE_S )
        {
          result = DSAT_ERROR;
        }
        break;
      /*---------------------------------------------------------------------
        Error
      ---------------------------------------------------------------------*/
      default:
        result = DSAT_ERROR;
        break;
    }/* switch ()*/
    }else
    {
      DS_AT_MSG1_ERROR("$QCMIPRT: Failed to update profile,arg_found %d", tok_ptr->args_found);
      result = DSAT_ERROR;
    }  
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                           rb_ptr->size,
                                           "$QCMIPRT: (0 - 1),(0 - 1)\n");
  }
  else  if(tok_ptr->op == (NA|QU))
  {
     dsmip_get_1x_profile_val_num(DS_QCMIPRT_VAL,(void *)temp_val,2);
     rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                            rb_ptr->size,
                                            "$QCMIPRT: %d,%d\n",
                                            temp_val[0],
                                            temp_val[1]);
  }
  else
  {
    /* Not Supported */
    return DSAT_ERROR;
  }
 return result;
} /* dsat707_exec_qcmiprt_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPGETP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPGETP command.
  Return all information corresponding to the specified profile
  number.

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
dsat_result_enum_type dsat707_exec_qcmipgetp_cmd
(
  dsat_mode_enum_type      mode,           /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,     /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr         /*  Place to put response       */
)
{
  dsat_num_item_type  profile;                           /* which profile to query       */
  dsat_result_enum_type result   = DSAT_OK;
  dsat_num_item_type    temp_val = 0; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (tok_ptr->op == (NA|EQ|AR) || (tok_ptr->op == NA)) 
  {
    if(FALSE == DSAT_VALIDATE_SET_MIP(DS_QCMIPGETP_VAL,5))
  {
      DS_AT_MSG1_ERROR("$QCMIPGETP: Invalid Number of arguements  %d", tok_ptr->args_found);
    return DSAT_ERROR;
  }
    if(tok_ptr->args_found == 0)
    {
      dsmip_get_1x_profile_val_num(DS_QCMIPP_VAL,(void *)&profile,0);
    }
    else
    {
      profile = temp_val;
    }
 
    /*-----------------------------------------------------------------------
      Do a "get" on the NV for user profile to check the return value
      If the NV status is not "done_s" then the profile does not exist.
      So return error.
    -----------------------------------------------------------------------*/
    ds_nv_item.ds_mip_gen_user_prof.index = (byte) profile;
    atmip_nv_status = dsatutil_get_nv_item(NV_DS_MIP_GEN_USER_PROF_I, &ds_nv_item);
    if(atmip_nv_status != NV_DONE_S)
    {
      return DSAT_ERROR;
    }
    (void) dsat707_qcmipgetp( rb_ptr->data_ptr,rb_ptr->size ,(byte) profile );

  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                           rb_ptr->size,
                                          "$QCMIPGETP: (0 - 5)\n");
  }
  else  if(tok_ptr->op == (NA|QU))
  {
     dsmip_get_1x_profile_val_num(DS_QCMIPGETP_VAL,(void *)&temp_val,0);
     rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                            rb_ptr->size,
                                            "$QCMIPGETP: %d\n",
                                            temp_val);
  }
  else
  {
    /* Not Supported */
    return DSAT_ERROR;
  }
  rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_qcmipgetp_cmd */
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPHA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPHA command.

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
dsat_result_enum_type dsat707_exec_qcmipha_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
)
{
  /*lint -esym(715,mode)*/
  dsat_result_enum_type result = DSAT_OK;
  dsmip_shadow_nv_type          *temp_shadow_nv    = NULL;
  ds707_mip_profile_type     *ds_static_profile = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_static_profile =  ds707_pkt_mgr_get_1x_data_profile_ptr();
  temp_shadow_nv    =  (dsmip_shadow_nv_type *)dsmip_get_shadow_nv_ptr();

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(ds_static_profile == NULL || temp_shadow_nv == NULL)
 {
   return DSAT_ERROR;
 }
 
  result = ds_at707_mip_ip(tok_ptr,
                           rb_ptr->data_ptr,
                           tab_entry,
                           (dsat_string_item_type *)ds_static_profile->mip.ds_qcmipha_val,
                           &ds_static_profile->mip.ds_qcmip_ha_num_val,
                           &(temp_shadow_nv->home_addr.value),
                           (boolean *) &temp_shadow_nv->home_addr.set);

  rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_qcmipha_cmd */
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPNAI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPNAI command.
  Set the NAI for the currently active profile.

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
dsat_result_enum_type dsat707_exec_qcmipnai_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
)
{
  /*lint -esym(715,mode)*/
  dsat_result_enum_type     result = DSAT_OK;
  byte  *                   res_buff_ptr = rb_ptr->data_ptr;
  byte                      nv_pref;
  dsat_string_item_type     temp_qcmipnai_val[MAX_NAI_LEN+1];
  dsmip_shadow_nv_type     *temp_shadow_nv = NULL;
  uint8                     temp_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(temp_qcmipnai_val,0x0,sizeof(temp_qcmipnai_val));
  temp_shadow_nv = (dsmip_shadow_nv_type *)dsmip_get_shadow_nv_ptr();
  if(NULL == temp_shadow_nv)
  {
    return DSAT_ERROR;
  }
  /*-----------------------------------------------------------------------
   Is it AT$QCMIPNAI?
  -----------------------------------------------------------------------*/
  if(tok_ptr->op == (NA|QU))
  {
    /*---------------------------------------------------------------------
      If NAI length is 0 return "unset" else display the nai.
    ---------------------------------------------------------------------*/
     dsmip_get_1x_profile_val_str(DS_QCMIPNAI_VAL,(char *)temp_qcmipnai_val, sizeof(temp_qcmipnai_val));
    
    if(strlen((char *)temp_qcmipnai_val) != 0 )
    {
      (void) strlcpy((char *)res_buff_ptr,
                    (char *)temp_qcmipnai_val,
                    rb_ptr->size);

      /*-------------------------------------------------------------------
        If the shadow_nv is not null nai is not empty.
      -------------------------------------------------------------------*/
      if (temp_shadow_nv->nai.len != 0)
      {
        (void) strlcat(
            (char *)res_buff_ptr, 
            ",0", 
            rb_ptr->size - strlen((char *)res_buff_ptr));
      }
      else
      {
        (void) strlcat(
            (char *)res_buff_ptr, 
            ",1",
            rb_ptr->size - strlen((char *)res_buff_ptr));
      }
    }
    else
    {
      (void) strlcpy(
          (char *)res_buff_ptr,
          "$QCMIPNAI: Unset", 
          rb_ptr->size);
    }
  }/* if AT$QCMIPNAI? */

  /*-----------------------------------------------------------------------
    Is it AT$QCMIPNAI=?
  -----------------------------------------------------------------------*/
  else if(tok_ptr->op == (NA|EQ|QU))
  {
    dsatutil_fmt_string_range(tab_entry,
                              res_buff_ptr,
                              mip_string_range);
  }
  /*-----------------------------------------------------------------------
    Enter a new NAI value
  -----------------------------------------------------------------------*/
  else
  {
    /*---------------------------------------------------------------------
    If Using R-UIM, NAI is not writeable.   
    ---------------------------------------------------------------------*/
    if((NVRUIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
        NVRUIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()) &&
       ((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
    {

      /* Do not Allow the Enable Profile variable to be written */
      return DSAT_ERROR;
    }
    /*---------------------------------------------------------------------
      Check the number of arguments of the AT command and set default to 0 
      if only one argument
    ---------------------------------------------------------------------*/
    switch(tok_ptr->args_found)
    {
      case 1:
        nv_pref = '0';
        break;

      case 2:
        nv_pref = *(tok_ptr->arg[1]);
        break;

      default:
        DS_AT_MSG0_ERROR("Incorrect number of args found!");
        result = DSAT_ERROR;
        return result;
    }

    /*---------------------------------------------------------------------
      If the second argument is not 1 or 0 then return an error before
      processing further
    ---------------------------------------------------------------------*/
    if( (nv_pref != '0') && (nv_pref != '1') )
    {
      DS_AT_MSG0_ERROR("Second argument must be 0 or 1");
      result = DSAT_ERROR;
      return result;
    }

    if( dsat707_strip_quotes( (char *) tok_ptr->arg[0],
                    (char *) temp_qcmipnai_val,
                    tab_entry->compound ) == FALSE )
    {
      result = DSAT_ERROR;
    }
    else
    {
       dsmip_set_1x_profile_val_str(DS_QCMIPNAI_VAL,(char *)temp_qcmipnai_val, strlen((const char *)temp_qcmipnai_val));
      /*-------------------------------------------------------------------
        Depending on the NV commit preference, carry out actions 
        accordingly.  later: check return value
      -------------------------------------------------------------------*/
      switch(nv_pref)
      {
        case '0':

          (void) dsatutil_memscpy((void*)&temp_shadow_nv->nai.value[0],
                                         MAX_NAI_LEN,
                                  (void*)&temp_qcmipnai_val[0], 
                  MAX_NAI_LEN);
          temp_shadow_nv->nai.value[MAX_NAI_LEN - 1] = 0;
          temp_shadow_nv->nai.len = 
            (byte) strlen ((char *) &temp_shadow_nv->nai.value[0]);
          break;

        case '1':
          temp_shadow_nv->nai.len = 0;
          dsmip_get_1x_profile_val_num(DS_QCMIPP_VAL,(void *)&temp_val,0);
          ds_nv_item.ds_mip_gen_user_prof.index = temp_val;
          (void) dsatutil_get_nv_item( NV_DS_MIP_GEN_USER_PROF_I,
                                       &ds_nv_item );
          ds_nv_item.ds_mip_gen_user_prof.nai_length = (byte) 
          strlen((char *)temp_qcmipnai_val);
          (void) strlcpy(
                     (char *)ds_nv_item.ds_mip_gen_user_prof.nai,
                     (char *)temp_qcmipnai_val,
                     MAX_NAI_LEN );
          /*---------------------------------------------------------------
            Write the profile with the new value to NV.
          ---------------------------------------------------------------*/
          atmip_nv_status = dsatutil_put_nv_item(NV_DS_MIP_GEN_USER_PROF_I,
                                      &ds_nv_item);
          if( atmip_nv_status != NV_DONE_S )
          {
            result = DSAT_ERROR;
          }
          break;

        default:
          DS_AT_MSG0_ERROR("Invalid NV preference");
          result = DSAT_ERROR;
          break;
      }/* switch NV preference */
    }/* else parse success */
  }/* else regular processing */

  rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_qcmipnai_cmd() */
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPPHA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPPHA command.

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
dsat_result_enum_type dsat707_exec_qcmippha_cmd
(
  dsat_mode_enum_type      mode,         /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,   /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,     /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr       /*  Place to put response       */
)
{
  /*lint -esym(715,mode)*/
  dsat_result_enum_type result = DSAT_OK;
  ds707_mip_profile_type     *ds_static_profile = NULL;
  dsmip_shadow_nv_type          *temp_shadow_nv    = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_static_profile =  ds707_pkt_mgr_get_1x_data_profile_ptr();
  temp_shadow_nv    =  (dsmip_shadow_nv_type *)dsmip_get_shadow_nv_ptr();

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(ds_static_profile == NULL || temp_shadow_nv == NULL)
 {
   return DSAT_ERROR;
 }
  result = ds_at707_mip_ip(tok_ptr,
                           rb_ptr->data_ptr,
                           tab_entry,
                           (dsat_string_item_type *)ds_static_profile->mip.ds_qcmippha_val,
                           &ds_static_profile->mip.ds_qcmip_pri_ha_num_val,
                           &(temp_shadow_nv->pri_ha_addr.value),
                           (boolean *) &temp_shadow_nv->pri_ha_addr.set);
  rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_qcmippha_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPSHA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPSHA command.

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
dsat_result_enum_type dsat707_exec_qcmipsha_cmd
(
  dsat_mode_enum_type      mode,          /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr        /*  Place to put response       */
)
{
  /*lint -esym(715,mode)*/
  dsat_result_enum_type  result = DSAT_OK;
  ds707_mip_profile_type     *ds_static_profile = NULL;
  dsmip_shadow_nv_type          *temp_shadow_nv    = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    ds_static_profile =  ds707_pkt_mgr_get_1x_data_profile_ptr();
    temp_shadow_nv    =  (dsmip_shadow_nv_type *)dsmip_get_shadow_nv_ptr();

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    if(ds_static_profile == NULL || temp_shadow_nv == NULL)
   {
     return DSAT_ERROR;
   }
  result = ds_at707_mip_ip(tok_ptr,
                           rb_ptr->data_ptr,
                           tab_entry,
                           (dsat_string_item_type *)ds_static_profile->mip.ds_qcmipsha_val,
                           &ds_static_profile->mip.ds_qcmip_sec_ha_num_val,
                           &(temp_shadow_nv->sec_ha_addr.value),
                           (boolean *) &temp_shadow_nv->sec_ha_addr.set);
  rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_qcmipsha_cmd */
/*===========================================================================

FUNCTION DSAT707_QCMIPGETP

DESCRIPTION
  Displays the MIP user profile settings

  FUTURE - add indication for shadow values in use if no profile spec'd

DEPENDENCIES
  rb_ptr must point to enough space to hold all the values.

RETURN VALUE
  rb_ptr incremented by the size of the ASCII representation of
  the profile, so that it points to the final NULL character of the
  string.

SIDE EFFECTS
  None.

=====================================================================*/
LOCAL byte * dsat707_qcmipgetp
(
  byte *rb_ptr,
  uint16   size,
  byte profile
)
{
#define BUF_SIZE() (size < (rb_ptr - base_rb_ptr) ? 0 : (size - (rb_ptr - base_rb_ptr)))
  
  static const byte  prof_num_lbl[] = "Profile:";
  static const byte  nai_lbl[]      = "NAI:";
  static const byte  ha_lbl[]       = "Home Addr:";
  static const byte  pri_ha_lbl[]   = "Primary HA:";
  static const byte  sec_ha_lbl[]   = "Secondary HA:";
  static const byte  aaa_ss_lbl[]   = "MN-AAA SS:";
  static const byte  aaa_spi_lbl[]  = "MN-AAA SPI:";
  static const byte  ha_ss_lbl[]    = "MN-HA SS:";
  static const byte  ha_spi_lbl[]   = "MN-HA SPI:";
  static const byte  rev_tun_lbl[]  = "Rev Tun:";
  static const byte  enable_lbl[]   = "Enabled";
  static const byte  disable_lbl[]  = "Disabled";
  static const byte  set_lbl[]      = "Set";
  static const byte  unset_lbl[]    = "Unset";
  boolean            set;
  dsat_num_item_type temp_s3_val;
  dsat_num_item_type temp_s4_val;
  byte    *base_rb_ptr;
  dsmip_shadow_nv_type   *temp_shadow_nv = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Copy the profile label and profile number
  -------------------------------------------------------------------------*/
   temp_shadow_nv = (dsmip_shadow_nv_type *)dsmip_get_shadow_nv_ptr();
   if(NULL == temp_shadow_nv)
   {
     *rb_ptr++ = '\0';
      return rb_ptr;
   }
   base_rb_ptr = rb_ptr;
  /*Source length differs between small and large DSM items.*/
  (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),(void*)prof_num_lbl,sizeof(prof_num_lbl));
  
  rb_ptr += sizeof(prof_num_lbl) - 1;
  rb_ptr = dsatutil_itoa((uint16) profile, rb_ptr, 10);
  *rb_ptr++ = ' ';

  /*-------------------------------------------------------------------------
    Check if the profile is enabled - later: check return value
  -------------------------------------------------------------------------*/
  (void) dsatutil_get_nv_item(NV_DS_MIP_ENABLE_PROF_I,&ds_nv_item);
  if(ds_nv_item.ds_mip_enable_prof[profile])
  {
    (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),(void*)enable_lbl,sizeof(enable_lbl));
    rb_ptr += sizeof(enable_lbl) - 1;
  }
  else
  {
    (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),(void*)disable_lbl,sizeof(disable_lbl));
    rb_ptr += sizeof(disable_lbl) - 1 ;
  }
  /*-------------------------------------------------------------------------
    Copy contents of S3 and S4 registers (typically CR and LF) to the
    response buffer.
  -------------------------------------------------------------------------*/
  temp_s3_val  = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
  temp_s4_val  = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
  *rb_ptr++ = (byte) temp_s3_val;
  *rb_ptr++ = (byte) temp_s4_val;

  /*-------------------------------------------------------------------------
    Get the user profile - later: check return value
  -------------------------------------------------------------------------*/
  ds_nv_item.ds_mip_gen_user_prof.index = profile;
  (void) dsatutil_get_nv_item(NV_DS_MIP_GEN_USER_PROF_I, &ds_nv_item);

  /*-------------------------------------------------------------------------
    Copy the NAI label and NAI
  -------------------------------------------------------------------------*/
  (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),(void*)nai_lbl,sizeof(nai_lbl));
  rb_ptr += sizeof(nai_lbl) - 1;

  if(ds_nv_item.ds_mip_gen_user_prof.nai_length != 0)
  {
    (void) strlcpy( 
        (char *)rb_ptr, 
        (char *)ds_nv_item.ds_mip_gen_user_prof.nai,
        ds_nv_item.ds_mip_gen_user_prof.nai_length + 1);
    rb_ptr += ds_nv_item.ds_mip_gen_user_prof.nai_length;
  }
  else
  {
   (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),(void*)unset_lbl,sizeof(unset_lbl));
    rb_ptr += sizeof(unset_lbl) - 1 ;
  }
  /*-------------------------------------------------------------------------
    Copy contents of S3 and S4 registers (typically CR and LF) to the
    response buffer.
  -------------------------------------------------------------------------*/
  temp_s3_val  = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
  temp_s4_val  = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
  *rb_ptr++ = (byte) temp_s3_val;
  *rb_ptr++ = (byte) temp_s4_val;
  /*-------------------------------------------------------------------------
    Copy the Home Addr label and Home Addr
  -------------------------------------------------------------------------*/
  (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),(void*)ha_lbl,sizeof(ha_lbl));
  rb_ptr += sizeof(ha_lbl) - 1;
  rb_ptr = dsat707_ip_uint32_to_str(
                           ds_nv_item.ds_mip_gen_user_prof.home_addr,rb_ptr);
  /*-------------------------------------------------------------------------
    Copy contents of S3 and S4 registers (typically CR and LF) to the
    response buffer.
  -------------------------------------------------------------------------*/
  temp_s3_val  = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
  temp_s4_val  = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
  *rb_ptr++ = (byte) temp_s3_val;
  *rb_ptr++ = (byte) temp_s4_val;


  /*-------------------------------------------------------------------------
    Copy the Primary HA label and Primary HA
  --------------------------------------------------------------------------*/
  (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),(void*)pri_ha_lbl,sizeof(pri_ha_lbl));
  rb_ptr += sizeof(pri_ha_lbl) - 1;
  rb_ptr = dsat707_ip_uint32_to_str(
    ds_nv_item.ds_mip_gen_user_prof.primary_ha_addr,rb_ptr);
  
  /*-------------------------------------------------------------------------
    Copy contents of S3 and S4 registers (typically CR and LF) to the
    response buffer.
  -------------------------------------------------------------------------*/
  temp_s3_val  = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
  temp_s4_val  = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
  *rb_ptr++ = (byte) temp_s3_val;
  *rb_ptr++ = (byte) temp_s4_val;


  /*-------------------------------------------------------------------------
    Copy the Secondary HA label and Secondary HA
  -------------------------------------------------------------------------*/
  (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),(void*)sec_ha_lbl,sizeof(sec_ha_lbl));
  rb_ptr += sizeof(sec_ha_lbl) - 1;
  rb_ptr = dsat707_ip_uint32_to_str(
    ds_nv_item.ds_mip_gen_user_prof.secondary_ha_addr,rb_ptr);

  /*-------------------------------------------------------------------------
    Copy contents of S3 and S4 registers (typically CR and LF) to the
    response buffer.
  -------------------------------------------------------------------------*/
  temp_s3_val  = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
  temp_s4_val  = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
  *rb_ptr++ = (byte) temp_s3_val;
  *rb_ptr++ = (byte) temp_s4_val;


  /*-------------------------------------------------------------------------
    Copy the MN AAA SPI label and MN AAA SPI
  -------------------------------------------------------------------------*/
  (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),(void*)aaa_spi_lbl,sizeof(aaa_spi_lbl));
  rb_ptr += sizeof(aaa_spi_lbl) - 1;

  rb_ptr = dsatutil_itoa(ds_nv_item.ds_mip_gen_user_prof.mn_aaa_spi,
                         rb_ptr,
                         10);
  
  /*-------------------------------------------------------------------------
    Copy contents of S3 and S4 registers (typically CR and LF) to the
    response buffer.
  -------------------------------------------------------------------------*/
  temp_s3_val  = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
  temp_s4_val  = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
  *rb_ptr++ = (byte) temp_s3_val;
  *rb_ptr++ = (byte) temp_s4_val;


  /*-------------------------------------------------------------------------
    Copy the MN HA SPI label and MN HA SPI
  -------------------------------------------------------------------------*/
  (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),(void*)ha_spi_lbl,sizeof(ha_spi_lbl));
  rb_ptr += sizeof(ha_spi_lbl) - 1;

  rb_ptr = dsatutil_itoa(ds_nv_item.ds_mip_gen_user_prof.mn_ha_spi,
                         rb_ptr,
                         10);
  
  /*-------------------------------------------------------------------------
    Copy contents of S3 and S4 registers (typically CR and LF) to the
    response buffer.
  -------------------------------------------------------------------------*/
  temp_s3_val  = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
  temp_s4_val  = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
  *rb_ptr++ = (byte) temp_s3_val;
  *rb_ptr++ = (byte) temp_s4_val;


  /*-------------------------------------------------------------------------
    Copy the Reverse Tunelling label and its value
  -------------------------------------------------------------------------*/
  (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),(void*)rev_tun_lbl,sizeof(rev_tun_lbl));
  rb_ptr += sizeof(rev_tun_lbl) - 1;

  rb_ptr = dsatutil_itoa(ds_nv_item.ds_mip_gen_user_prof.rev_tun_pref,
                         rb_ptr,
                         10);

  /*-------------------------------------------------------------------------
    Copy contents of S3 and S4 registers (typically CR and LF) to the
    response buffer.
  --------------------------------------------------------------------------*/
  temp_s3_val  = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
  temp_s4_val  = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
  *rb_ptr++ = (byte) temp_s3_val;
  *rb_ptr++ = (byte) temp_s4_val;

  /*-------------------------------------------------------------------------
    Get the shared secret profile
    future - check return value
  -------------------------------------------------------------------------*/
  if(NVRUIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
     NVRUIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support())
  {
    /*-----------------------------------------------------------------------
      The MN-AAA SS is set on the card, just set it to a non-zero value
    -----------------------------------------------------------------------*/
    ds_nv_item.ds_mip_ss_user_prof.mn_aaa_shared_secret_length = 1;
    ds_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret_length = 1;
  }
  else
  {
    ds_nv_item.ds_mip_ss_user_prof.index = profile;
    (void) dsatutil_get_nv_item(NV_DS_MIP_SS_USER_PROF_I,&ds_nv_item);
  }

  /*-------------------------------------------------------------------------
    Check if the AAA SS length is 0
  -------------------------------------------------------------------------*/
  (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),(void*)aaa_ss_lbl,sizeof(aaa_ss_lbl));
  rb_ptr += sizeof(aaa_ss_lbl) - 1;

  set = FALSE;
  if (temp_shadow_nv->mn_aaa_ss.len)
  {
    set = TRUE;
  }
  else
  {
    if(ds_nv_item.ds_mip_ss_user_prof.mn_aaa_shared_secret_length != 0)
    {
      set = TRUE;
    }
  }

  if (set)
  {
    (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),(void*)set_lbl,sizeof(set_lbl));
    rb_ptr += sizeof(set_lbl) - 1;
  }
  else
  {
    (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),(void*)unset_lbl,sizeof(unset_lbl));
    rb_ptr += sizeof(unset_lbl) - 1;
  }

  /*-------------------------------------------------------------------------
    Copy contents of S3 and S4 registers (typically CR and LF) to the
    response buffer.
  -------------------------------------------------------------------------*/
  temp_s3_val  = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
  temp_s4_val  = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
  *rb_ptr++ = (byte) temp_s3_val;
  *rb_ptr++ = (byte) temp_s4_val; 

  /*-------------------------------------------------------------------------
    Check if the HA SS length is 0
  -------------------------------------------------------------------------*/
  (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),(void*)ha_ss_lbl,sizeof(ha_ss_lbl));
  rb_ptr += sizeof(ha_ss_lbl) - 1;

  set = FALSE;
  if (temp_shadow_nv->mn_ha_ss.len)
  {
    set = TRUE;
  }
  else
  {
    if(ds_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret_length != 0)
    {
      set = TRUE;
    }
  }

  if (set)
  {
    (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),(void*)set_lbl,sizeof(set_lbl));
    rb_ptr += sizeof(set_lbl) - 1;
  }
  else
  {
    (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),(void*)unset_lbl,sizeof(unset_lbl));
    rb_ptr += sizeof(unset_lbl) - 1;
  }

  *rb_ptr++ = '\0';
  return rb_ptr;
}/* dsat707_qcmipgetp() */



/*===========================================================================
  FUNCTION DS_AT707_MIP_IP

  DESCRIPTION
   This function handles the Mobile IP AT commands involving IP addresses.
   Specifically the function handles the following AT commands:
   AT$QCMIPHA, AT$QCMIPPHA, AT$QCMIPSHA

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
   None
===========================================================================*/
LOCAL dsat_result_enum_type ds_at707_mip_ip
(
  const tokens_struct_type  *tok_ptr,
  byte                      *rb_ptr,
  const dsati_cmd_type      *table_entry,
  dsat_string_item_type     *ds_atcop_qcmip_ip_val,
  uint32                    *ds_atcop_ip_uint32val,
  uint32                    *shadow_nv_ip,
  boolean                   *shadow_nv_ip_set
)
{
  dsat_result_enum_type     result = DSAT_OK;
  byte                      nv_pref;
  dsat_num_item_type        temp_val; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /*-------------------------------------------------------------------------
    Is it AT$QCMIP_XX_IP?
  -------------------------------------------------------------------------*/
  if(tok_ptr->op == (NA|QU))
  {
    /*-----------------------------------------------------------------------
      rb_ptr contains the IP address in the string format, to be displayed
      on the screen.
    -----------------------------------------------------------------------*/
    rb_ptr = dsat707_ip_uint32_to_str(*ds_atcop_ip_uint32val, rb_ptr);

    *rb_ptr++ = ',';

    if(shadow_nv_ip != NULL)
    {
      *rb_ptr++ = '0';
    }
    else
    {
      *rb_ptr++ = '1';
    }
    *rb_ptr = '\0';
  }/* if question mark */

  /*-------------------------------------------------------------------------
    Is it AT$QCMIP_XX_IP=?
  -------------------------------------------------------------------------*/
  else if(tok_ptr->op == (NA|EQ|QU))
  {
    dsatutil_fmt_string_range(table_entry, rb_ptr, mip_ip_string_range);
  }

  /*-------------------------------------------------------------------------
    Enter the IP Address Value
  -------------------------------------------------------------------------*/
  else
  {
    /*-----------------------------------------------------------------------
    If Using R-UIM, Cannot change IP info.   
    -----------------------------------------------------------------------*/
    if((NVRUIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
        NVRUIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()))
    {
      return DSAT_ERROR;
    }
    /*-----------------------------------------------------------------------
      Check the number of arguments of the AT command and set default to 0
      if only one argument
    -----------------------------------------------------------------------*/
    switch(tok_ptr->args_found)
    {
      case 1:
        nv_pref = '0';
        break;

      case 2:
        nv_pref = *(tok_ptr->arg[1]);
        break;

      default:
        DS_AT_MSG0_ERROR("Incorrect number of args found!");
        result = DSAT_ERROR;
        return result;
    }

    /*-----------------------------------------------------------------------
      If the second argument is not 1 or 0 then return an error before
      processing further
    -----------------------------------------------------------------------*/
    if( (nv_pref != '0') && (nv_pref != '1') )
    {
      DS_AT_MSG0_ERROR("Second argument must be 0 or 1");
      result = DSAT_ERROR;
      return result;
    }

    if( dsat707_strip_quotes( (char *) tok_ptr->arg[0],
                              (char *) ds_atcop_qcmip_ip_val,
                              table_entry->compound ) == FALSE )
    {
      result = DSAT_ERROR;
      return result;
    }
    else
    {
      /*---------------------------------------------------------------------
        Convert the string to uint32
      ---------------------------------------------------------------------*/
      if( dsat707_ip_str_to_uint32( (char *)ds_atcop_qcmip_ip_val,
                                    ds_atcop_ip_uint32val) == FALSE )
      {
        return DSAT_ERROR;
      }

      /*---------------------------------------------------------------------
        Enter a new HA value
      ---------------------------------------------------------------------*/
      else
      {
        /*-------------------------------------------------------------------
          Depending on the NV commit preference, carry out actions
          accordingly.
        -------------------------------------------------------------------*/
        switch(nv_pref)
        {
          case '0':
            *shadow_nv_ip = *ds_atcop_ip_uint32val;
            *shadow_nv_ip_set = TRUE;
            break;

          case '1':
            /*---------------------------------------------------------------
              Retrieve current active profile from NV.
              future: check return value
            ---------------------------------------------------------------*/
            *shadow_nv_ip = 0;
            *shadow_nv_ip_set = FALSE;
            dsmip_get_1x_profile_val_num(DS_QCMIPP_VAL,(void *)&temp_val,0);
            ds_nv_item.ds_mip_gen_user_prof.index = (byte) temp_val;
            (void) dsatutil_get_nv_item( NV_DS_MIP_GEN_USER_PROF_I,
                                         &ds_nv_item );

            switch(table_entry->special)
            {
              case SPECIAL_QCMIPHA:
                ds_nv_item.ds_mip_gen_user_prof.home_addr
                  = *ds_atcop_ip_uint32val;
                break;

              case SPECIAL_QCMIPPHA:
                ds_nv_item.ds_mip_gen_user_prof.primary_ha_addr
                  = *ds_atcop_ip_uint32val;
                break;

              case SPECIAL_QCMIPSHA:
                ds_nv_item.ds_mip_gen_user_prof.secondary_ha_addr
                  = *ds_atcop_ip_uint32val;
                break;

              default:
                DS_AT_MSG0_ERROR("Unknown AT command");
                result = DSAT_ERROR;
                break;
            }
            
            /*---------------------------------------------------------------
              Write the profile with the new value to NV.
            ---------------------------------------------------------------*/
            atmip_nv_status = dsatutil_put_nv_item(NV_DS_MIP_GEN_USER_PROF_I,
                                        &ds_nv_item);
            if(atmip_nv_status != NV_DONE_S)
            {
              result = DSAT_ERROR;
            }
            break;

          default:
            DS_AT_MSG0_ERROR("Invalid NV preference");
            result = DSAT_ERROR;
            break;
        }/* switch NV prefernce*/
      }/* else st_to_uint32 success */
    }/* else parse success*/
  }/* else regular processing */

  return result;

} /* ds_atcop_mip_ip */



/*===========================================================================
  FUNCTION DS_AT707_MIP_SS

  DESCRIPTION
   This function handles the Shared Secrets related AT commands of MIP.
   Specifically the AT command handles:
   AT$QCMIPMASS, AT$QCMIPMHSS

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
   None
===========================================================================*/
LOCAL dsat_result_enum_type ds_at707_mip_ss
(
  const tokens_struct_type  *tok_ptr,
  const dsati_cmd_type      *table_entry,
  dsat_string_item_type     *at_ss_val,
  byte                      *shadow_nv_ss,
  byte                      *shadow_nv_ss_len,
  dsat_num_item_type         shadow_nv_ss_max_len
)
{
  dsat_result_enum_type     result = DSAT_OK;
  byte                      ss_len;            /* temp - to calc ss length */
  byte                      nv_pref;
  dsat_num_item_type        temp_val; 

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /*-------------------------------------------------------------------------
    Processing AT$QCMIP_XX_SS[=xxxx][yyy]
      xxxx = password to be set
      y    = 0 - store in RAM
             1 - store in NV
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    Check the number of arguments of the AT command and set default to 0
    if only one argument
  -------------------------------------------------------------------------*/
  switch(tok_ptr->args_found)
  {
    case 1:
      nv_pref = '0';
      break;

    case 2:
      nv_pref = *(tok_ptr->arg[1]);  // arg is byte * - take first byte
      break;

    default:
      DS_AT_MSG0_ERROR("Incorrect number of args found!");
      return DSAT_ERROR;
  }

  /*-------------------------------------------------------------------------
    Strip "" quotes from commandline
  -------------------------------------------------------------------------*/
  if( dsat707_strip_quotes( (char *) tok_ptr->arg[0],
                            (char *) at_ss_val,
                            table_entry->compound ) == FALSE )
  {
    DS_AT_MSG0_ERROR("Bad quotes (or input too long)!");
    return DSAT_ERROR;
  }

  /*-------------------------------------------------------------------------
    Determine the length of the user-entered shared secret
  -------------------------------------------------------------------------*/
  ss_len = (byte) strlen((char *)at_ss_val);
  if( ss_len > NV_MAX_MN_AAA_SHARED_SECRET_LEN )
  {
    DS_AT_MSG0_ERROR("Invalid length");
    return DSAT_ERROR;
  }

  /*-------------------------------------------------------------------------
    Act based on the NV commit preference:
      0 -> commit to shadow NV
      1 -> commit to real NV
  -------------------------------------------------------------------------*/
  switch(nv_pref)
  {
    case '0':
      /*---------------------------------------------------------------------
        Copy the value to shadow NV
      ---------------------------------------------------------------------*/
      *shadow_nv_ss_len = ss_len;
      (void) dsatutil_memscpy((void*)shadow_nv_ss,shadow_nv_ss_max_len,(void*)at_ss_val,*shadow_nv_ss_len);
      break;

    case '1':
      /*---------------------------------------------------------------------
        Determine the active MIP profile based on AT$QCMIPP setting.
      ---------------------------------------------------------------------*/
      dsmip_get_1x_profile_val_num(DS_QCMIPP_VAL,(void *)&temp_val,0);
      ds_nv_item.ds_mip_ss_user_prof.index =(byte)temp_val ;

      /*---------------------------------------------------------------------
        Retrieve the associated shared secret profile (read first since
        this is a compound NV item and we are setting one field only)
        later - check return value
      ---------------------------------------------------------------------*/
      atmip_nv_status = dsatutil_get_nv_item( NV_DS_MIP_SS_USER_PROF_I,
                                              &ds_nv_item );
      if((atmip_nv_status != NV_DONE_S) && (atmip_nv_status != NV_NOTACTIVE_S))
      {
        return DSAT_ERROR;
      }

      /*---------------------------------------------------------------------
        Clear shadow NV setting - do we really want to do this?
      ---------------------------------------------------------------------*/
      *shadow_nv_ss_len = 0;

      /*---------------------------------------------------------------------
        Copy entered value into NV item which will be committed to NV.
      ---------------------------------------------------------------------*/
      switch(table_entry->special)
      {
        case SPECIAL_QCMIPMASS:
          ds_nv_item.ds_mip_ss_user_prof.mn_aaa_shared_secret_length
            = ss_len;
          (void) dsatutil_memscpy((void*)ds_nv_item.ds_mip_ss_user_prof.mn_aaa_shared_secret,
                    NV_MAX_MN_AAA_SHARED_SECRET_LEN,(void*)at_ss_val,ss_len);
          break;

        case SPECIAL_QCMIPMHSS:
          ds_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret_length
            = ss_len;
          (void) dsatutil_memscpy((void*)ds_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret,
                    NV_MAX_MN_HA_SHARED_SECRET_LEN,(void*)at_ss_val,ss_len);
          break;

        default:
          DS_AT_MSG0_ERROR("Unknown AT command");
          return DSAT_ERROR;
      }
      /*---------------------------------------------------------------------
        Write the profile with the new value to NV.
      ---------------------------------------------------------------------*/
      atmip_nv_status = dsatutil_put_nv_item( NV_DS_MIP_SS_USER_PROF_I, 
                                              &ds_nv_item);
     
      if(atmip_nv_status != NV_DONE_S)
      {
        return DSAT_ERROR;
      }
      break;

    default:
      DS_AT_MSG0_ERROR("Second argument must be 0 or 1");
      result = DSAT_ERROR;
      break;
  }/* switch NV preference */

  return result;

} /* ds_at707_mip_ss() */



/*===========================================================================
  FUNCTION DS_AT707_MIP_SPI

  DESCRIPTION
   This function handles the SPI related AT commands of MIP.
   Specifically the AT command handles:
   AT$QCMIPMASPI, AT$QCMIPMHSPI

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
   None
===========================================================================*/
LOCAL dsat_result_enum_type ds_at707_mip_spi
(
  const dsati_cmd_type  *table_entry,
  uint32                *shadow_spi,
  byte                  *shadow_spi_set_flag
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_num_item_type    temp_val; 
  dsmip_val_enum_type   mip_spi;
  uint32                temp_spi_val[2];

  mip_spi = ((table_entry->special == SPECIAL_QCMIPMASPI)? DS_QCMIPMASPI_VAL : DS_QCMIPMHSPI_VAL);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  dsmip_get_1x_profile_val_num(mip_spi,(void *)temp_spi_val,2);
  switch(temp_spi_val[1])
  {
    /*-----------------------------------------------------------------------
      Dont Commit
    -----------------------------------------------------------------------*/
    case 0:
      *shadow_spi          = temp_spi_val[0];
      *shadow_spi_set_flag = TRUE;
      break;

    /*-----------------------------------------------------------------------
      Commit - later check return value on get
    -----------------------------------------------------------------------*/
    case 1:
      *shadow_spi = 0;
      *shadow_spi_set_flag = FALSE;
      dsmip_get_1x_profile_val_num(DS_QCMIPP_VAL,(void*)&temp_val,0);
      ds_nv_item.ds_mip_gen_user_prof.index = (byte) temp_val;
      (void ) dsatutil_get_nv_item(NV_DS_MIP_GEN_USER_PROF_I,&ds_nv_item);

      switch(table_entry->special)
      {
        case SPECIAL_QCMIPMASPI:
          ds_nv_item.ds_mip_gen_user_prof.mn_aaa_spi_set = TRUE;
          ds_nv_item .ds_mip_gen_user_prof.mn_aaa_spi    = temp_spi_val[0];
          break;

        case SPECIAL_QCMIPMHSPI:
          ds_nv_item.ds_mip_gen_user_prof.mn_ha_spi_set = TRUE;
          ds_nv_item.ds_mip_gen_user_prof.mn_ha_spi     = temp_spi_val[0];
          break;

        default:
          DS_AT_MSG0_ERROR("Unknown AT command");
          result = DSAT_ERROR;
          return result;
      }
      atmip_nv_status = dsatutil_put_nv_item(NV_DS_MIP_GEN_USER_PROF_I, 
                                             &ds_nv_item);
      if( atmip_nv_status != NV_DONE_S )
      {
        result = DSAT_ERROR;
        return result;
      }
      break;

    /*-----------------------------------------------------------------------
      Error
    -----------------------------------------------------------------------*/
    default:
      DS_AT_MSG0_ERROR("Second argument must be 0 or 1");
      result = DSAT_ERROR;
      return result;
  }/* switch ()*/

  return result;

} /* ds_atcop_mip_spi() */



/*===========================================================================
  FUNCTION HEXCHAR_TO_NIBBLE

  DESCRIPTION
    Convert input character which is a hex digit ('0' to '9', 'A' to 'F' case
    insensitive) into its hex value (0x00 - 0x0F).

    e.g. input '1' returns 0x01
         input 'a' returns 0x0A

  DEPENDENCIES
    None

  RETURN VALUE
    The hex value of the input hex character.
    0xFF if the input character is not a valid hex digit.

  SIDE EFFECTS
    Overwrites the input string
===========================================================================*/

#define NIBBLE_ERROR  (0xFF)

LOCAL byte hexchar_to_nibble
(
  byte  hexchar
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  hexchar -= '0';
  if (hexchar < 10)
    return hexchar;
  hexchar -= 17;  // 'A' - '0'
  if (hexchar < 6)
    return hexchar + 10;
  hexchar -= 32;  // 'a' - 'A'
  if (hexchar < 6)
    return hexchar + 10;
  return NIBBLE_ERROR;
}



/*===========================================================================
  FUNCTION HEXSTR_TO_BIN

  DESCRIPTION
    Convert a string containing a hex value into the actual binary hex value
    in big-endian form (no limit on length of output binary value).  

    The input string MUST be null terminated.  The output string is placed 
    in the same buffer as the input string, and will always be smaller than 
    the input string.  Note that the input string is corrupted so if the 
    caller needs to preserve the input buffer, they should do a copy before
    calling this function.

    Note that the output is an array of binary bytes, and is not 'null 
    terminated' as a character string would be - so don't try to printf
    the output! 

  DEPENDENCIES
    None

  RETURN VALUE
    Length of output binary hex value in bytes
    0 if an error occurs (invalid input character)

  SIDE EFFECTS
    Overwrites the input string
===========================================================================*/

#define GOT_NIBBLE (0x10)

LOCAL byte hexstr_to_bin
(
  byte * str
)
{
  int  i;
  int  bin;             /* byte index / binary */
  byte nibble;
  byte newbyte;         /* accumulated byte value */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Translate hex digits into hex values byte by byte.

    Each character will represent a binary nibble, so we convert the input
    string byte-by-byte, creating output string nibble-by-nibble.  

    'newbyte' is used as temporary storage while forming the output byte 
    (from 2 input bytes) - we pre-fill this with a '00' byte if the input
    string length is odd, so the output string will be byte aligned (i.e.
    the output string will have a lead '0' nibble).

    GOT_NIBBLE is bitwise OR'ed into 'newbyte' in the upper nibble when the 
    first nibble is being shifted in to 'newbyte' (from the right).  This
    ensures 'newbyte' is non-zero (this is our state that tells us whether
    we're processing the first or second nibble) after the first nibble is 
    processed, even if it is zero (e.g. '0' input would produce 0x10).
    GOT_NIBBLE is shifted out of the output when the second nibble is 
    processed, so won't impact on the conversion at all.
  -------------------------------------------------------------------------*/
  newbyte = 0;
  bin = 0;
  if (strlen((char*) str) % 2)
  {
    newbyte = GOT_NIBBLE;
  };

  for (i = 0; str[i] != 0; i++)
  {
    if (newbyte)
    {
      newbyte <<= 4;
      if ((nibble = hexchar_to_nibble(str[i])) == NIBBLE_ERROR )
        return FALSE;
      DS_AT_MSG2_LOW("Debug - '%c' -> got nibble (%x)", str[i], nibble);
      newbyte |= nibble;
      DS_AT_MSG1_LOW("Debug - got byte (%x)", newbyte);
      str[bin++] = newbyte;
      newbyte = 0;
    }
    else
    {
      if ((nibble = hexchar_to_nibble(str[i])) == NIBBLE_ERROR )
        return FALSE;
      DS_AT_MSG2_LOW("Debug - '%c' -> got nibble (%x)", str[i], nibble);
      newbyte = nibble | GOT_NIBBLE;
    }
  }

  /* return length of the hex value  ( converted from string)    in bytes */

  return( (byte) (bin) );    /* return length of converted string */

} /* hexstr_to_bin() */



/*===========================================================================
  FUNCTION DS_AT707_MIP_SSX

  DESCRIPTION
   This function handles setting the Mobile IP shared secrets in hex format.

   Called by AT command handlers for: AT$QCMIPMASSX, AT$QCMIPMHSSX

  DEPENDENCIES
    None

  RETURN VALUE
    AT result code to be returned by calling AT command handler

  SIDE EFFECTS
   Adds stuff to result
===========================================================================*/
LOCAL dsat_result_enum_type ds_at707_mip_ssx
(
  const tokens_struct_type  *tok_ptr,
  const dsati_cmd_type      *table_entry,
  dsat_string_item_type     *at_ssx_val,
  byte                      *shadow_nv_ssx,
  byte                      *shadow_nv_ssx_len,
  dsat_num_item_type         shadow_nv_ssx_max_len
)
{
  dsat_result_enum_type     result = DSAT_OK;
  byte                      ss_len;            /* temp - to calc ss length */
  byte                      nv_pref;
  dsat_num_item_type        temp_val; 


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /*-------------------------------------------------------------------------
    ( tok_ptr->op == (NA|EQ|AR) )

    Processing AT$QCMIP_XX_SSX[=xxxx],[yyy]
      xxxx = password to be set
      y    = 0 - store in RAM
             1 - store in NV
  -------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------
    Check the number of arguments of the AT command and set default to 0
    if only one argument
  -----------------------------------------------------------------------*/
  switch(tok_ptr->args_found)
  {
    case 1:
      nv_pref = '0';
      break;

    case 2:
      nv_pref = *(tok_ptr->arg[1]);  // arg is byte * - take first byte
      break;

    default:
      DS_AT_MSG0_ERROR("Incorrect number of args found!");
      result = DSAT_ERROR;
      return result;
  }

  /*-----------------------------------------------------------------------
    Strip "" quotes from commandline and validate output length
  -----------------------------------------------------------------------*/
  if( dsat707_strip_quotes( (char *) tok_ptr->arg[0],
                            (char *) at_ssx_val,
                            table_entry->compound ) == FALSE )
  {
    DS_AT_MSG0_ERROR("Bad quotes (or input too long)!");
    return DSAT_ERROR;
  }

  
  /*------------------------------------------------------------------------
    Hex String to binary conversion here - returns length of binary string
  ------------------------------------------------------------------------*/
 
  ss_len = hexstr_to_bin(at_ssx_val);
  
  if ((ss_len == 0)||(ss_len > NV_MAX_MN_AAA_SHARED_SECRET_LEN))
  {
    DS_AT_MSG0_ERROR("Invalid hex string!");
    return DSAT_ERROR;
  }

  /*---------------------------------------------------------------------
    Act based on the NV commit preference:
      0 -> commit to shadow NV
      1 -> commit to real NV
  ---------------------------------------------------------------------*/
  switch(nv_pref)
  {
    case '0':
      /*-----------------------------------------------------------------
        Copy the value to shadow NV
      -----------------------------------------------------------------*/
      *shadow_nv_ssx_len = ss_len;
      (void) dsatutil_memscpy((void*)shadow_nv_ssx,shadow_nv_ssx_max_len,(void*)at_ssx_val,ss_len);
      break;

    case '1':
      /*-----------------------------------------------------------------
        Determine the active MIP profile based on AT$QCMIPP setting.
      -----------------------------------------------------------------*/
      dsmip_get_1x_profile_val_num(DS_QCMIPP_VAL,(void *)&temp_val,0);
      ds_nv_item.ds_mip_ss_user_prof.index = (byte) temp_val;

      /*-----------------------------------------------------------------
        Retrieve the associated shared secret profile (read first since
        this is a compound NV item and we are setting one field only)
        future: check return value
      -----------------------------------------------------------------*/
      (void) dsatutil_get_nv_item(NV_DS_MIP_SS_USER_PROF_I,&ds_nv_item);

      /*-----------------------------------------------------------------
        Clear shadow NV setting
      -----------------------------------------------------------------*/
      *shadow_nv_ssx_len = 0;

      /*-----------------------------------------------------------------
        Copy entered value into NV item which will be committed to NV.
      -----------------------------------------------------------------*/
      DS_AT_MSG2_MED(" rewriting ASSX or HSSX %d len = %d",table_entry->special,
                                               ss_len);
   
      switch(table_entry->special)
      {
        case SPECIAL_QCMIPMASSX:
          ds_nv_item.ds_mip_ss_user_prof.mn_aaa_shared_secret_length
            = ss_len;
          (void) dsatutil_memscpy((void*)ds_nv_item.ds_mip_ss_user_prof.mn_aaa_shared_secret,
                      NV_MAX_MN_AAA_SHARED_SECRET_LEN,(void*)at_ssx_val,ss_len);
          break;

        case SPECIAL_QCMIPMHSSX:
          ds_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret_length
            = ss_len;
          (void) dsatutil_memscpy((void*)ds_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret,
                      NV_MAX_MN_HA_SHARED_SECRET_LEN,(void*)at_ssx_val,ss_len);
          break;

        default:
          DS_AT_MSG0_ERROR("Unknown AT command");
          result = DSAT_ERROR;
          return result;
      }
      /*-----------------------------------------------------------------
        Write the profile with the new value to NV.
      -----------------------------------------------------------------*/
      atmip_nv_status = dsatutil_put_nv_item( NV_DS_MIP_SS_USER_PROF_I, 
                                              &ds_nv_item );
      if(atmip_nv_status != NV_DONE_S)
      {
        result = DSAT_ERROR;
        return result;
      }
      break;

    default:
      DS_AT_MSG0_ERROR("Second argument must be 0 or 1");
      result = DSAT_ERROR;
      break;
  }/* switch NV preference */

  return result;
} /* ds_atcop_mip_ssx() */
#endif /* FEATURE_DS_MOBILE_IP */
