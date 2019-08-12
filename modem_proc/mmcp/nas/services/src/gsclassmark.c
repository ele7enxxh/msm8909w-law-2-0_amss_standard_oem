/*===========================================================================
      COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/gsclassmark.c_v   1.3   17 Jul 2002 11:52:04   kishores  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/src/gsclassmark.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/28/12    am     Fixing compilation warning 
27/10/11    rajesh Encoding classmark2 IE correctly when active rat is LTE.
02/15/08    ab     A5_1 bit in Classmark 1 is set to FALSE when MS does not support A/Gb mode
08/01/07    kgj    Modify to support FEATURE_CGPS_UMTS_CP_GSM
12/14/05    npr    FEATURE_GSM_DTM: DTM Classmark now hooked into NV items
12/07/05    npr    FEATURE_GSM_DTM: Classmark changes for DTM
11/15/05    hj     Reverting changes made in previous version.
11/14/05    hj     Setting Modulation based multislot support to 0 in Radio
                   Capabilities IE.
09/30/05    up     Updated for Release 5 CRs.
Apr/12/05   sv     Additions for EDGE support.
Apr/06/05   sv     Corrections made to EDGE support.
Mar/16/05   sv     EGPRS changes for GEA support.
Feb/17/05   sv     Added support for Rel-4 changes.
Nov/04/04   sv     Classmark 3 changes for EDGE support.
Mar/11/04   VT     In gs_read_coded_classmark_2(), now setting freq_capability
                     if current_band being used is BAND_PGSM or BAND_EGSM.
Mar/10/04   VT     In gs_read_coded_classmark_3(), if PGSM_supported and 
                     EGSM_supported are both enabled: printing a MSG_ERROR 
                     instead of ASSERT and since they're mutually exclusive,  
                     setting PGSM_supported to FALSE.
10/07/02   he      Turned on GEA_1_supported & GEA_2_supported for GPRS.
09/18/01   sbk     New function nas_read_gprs_classmark implemented
06/17/02   rmc     Updated classmark functions to include correct multiband/dual mode information
07/18/02   rmc     Renamed rr_bit to gs_bit for move of bitstream to NAS VU
08/26/02   JC      Renamed FEATURE_GSM_MULTIBAND to FEATURE_GSM_MULTIBAND.
08/30/02   rmc     Fix bugs in classmark encoding for WCDMA
10/28/02   kwa     Fix bugs in classmark 3 encoding for radio access class.
12/09/02   rmc     Indicate UTRAN FDD not supported in classmark 3 if GSM is active RAT
02/13/03   ks      Setting the A5 alogirith supported fields properly in the
                   gs_read_coded_classmark_3() function.
04/15/03   sbs     Enabled LCSVA bit in classmark 2 and MS Position Method
                   capability ( GPS MS Assisted / MS Based ) for FEATURE_MMGPS
                   builds.
10/09/03   cd      Merged in GSM band preference support
11/03/03   guru    Fix for CR36616: Wrong Rev Level Indicator in Classmark Change Msg.
11/21/03   ks      Now using the mm_nv_force_mscr_r99 NV item that was read in
                   MM at powerup instead of reading the NV in clasmark function.
12/08/03   sbs     Location services related fields are now set according to 
                   parameters returned from lsm_ue_pos_capability_get().
08/11/04   ks      Now includng the gmm_int.h file
09/16/04   ab      CR49017, reset CM3 when user set UE perfer mode to 
                   GSM only mode.
11/16/2004 rg      Part of fix for CR51387: Changed lsm_ue_pos_capability_get() to 
                   lsm_gsm_ue_pos_capability_get()
05/17/05   HS      Added support for FEATURE_CCBS (CMSP bit in classmark 2)
10/11/05   hj      Encode RF power Capability as 111 in WCDMA only mode.
02/24/06   abe     Adding support for FEATURE_REL_6_SUPPORT
===========================================================================*/

/* Include files */

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "environ.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "aconfig.h"
#include "ghdi_exp.h"
#include "ghdi_exp_int.h"
#include "msg.h"
#include "err.h"
#include "amssassert.h"
#include "gs_v.h"
#include "gsbitstream.h"
#include "aconfig.h"
#include "mm_v.h"
#include "mm_as_types.h"
#include "gmm_int_v.h"
#include "nv.h"
#include "ghdi_nvmem.h"
#include "ghdi_nvmem_v.h"
#include "libprot.h"
#include "gsclassmark.h"
#include "gmmllcif.h"

#ifdef FEATURE_GSM_MULTIBAND
#include "rr_multiband.h"
#endif

#if defined(FEATURE_MMGPS)
#include "lsm_ue_capability.h"

/*
 *  Create some aliases so we can choose better names on CGPS
 *  without a bunch of #ifdef statements later
 */
#define GSCLASSMARK_POS_NETWORK_BASED   NETWORK_BASED
#define GSCLASSMARK_POS_UE_BASED        UE_BASED
#define GSCLASSMARK_POS_BOTH            BOTH
#define GSCLASSMARK_POS_NONE            NONE

#elif defined(FEATURE_CGPS_UMTS_CP_GSM)
cgps_classmark_cb_type    cgps_classmark_cb_fp;
#endif

#ifdef FEATURE_LTE
#include "lte_rrc_ext_api.h"
#endif

#ifdef FEATURE_GSM_EGPRS
#include "gllclient.h"
#endif

#ifdef FEATURE_DUAL_SIM
extern boolean is_gsm_mode_per_nas_stacks(sys_modem_as_id_e_type as_id);
  boolean gea_classmark_enquiry_per_subs(sys_modem_as_id_e_type as_id,gmmllc_gea_e_type gea_type); 
#else
  boolean gea_classmark_enquiry(gmmllc_gea_e_type gea_type);
#endif

#ifdef FEATURE_SGLTE
extern mm_as_id_e_type mm_cs_stack_id ;
extern mm_as_id_e_type mm_ps_stack_id ;
#endif

boolean gs_read_coded_classmark_3_per_nas_stacks(
  classmark_3_ie_T     *classmark_3_ptr,
  classmark_params_T   *params_ptr,
  sys_modem_as_id_e_type stack_id_nas
);



/**
 * Encodes the classmark1 information in classmark1 IE form
 * 
 * @param gsm_supported TRUE if GSM RAT is supported, FALSE otherwise
 * 
 * @param wcdma_in_use  TRUE if WCDMA is the active RAT, FALSE if GSM is the active RAT
 * 
 * @param MSCR99        TRUE if the network is a R99 or later, FALSE if the network is R98 or earlier
 *                      based on MSCR (MSC revision) value broadcast by network. Should always be set
 *                      to TRUE if called from WCDMA (RRC)
 * 
 * @return byte containing encoded classmark1 information
 */
byte gs_get_classmark_1(boolean gsm_supported, boolean wcdma_in_use, boolean MSCR99)
{
#ifdef FEATURE_DUAL_SIM

  MSG_FATAL_DS( 0, "DUAL SIM: Wrong API called gs_get_classmark_1()", 0,0,0 );
  return 0;

#else

  ghdi_status_T     status;
  hw_classmark_T    hw_classmark = {0};
  byte              return_value = 0;
  byte              rf_power_capability = 0;
  byte              classmark_revision_level;

  byte              es_ind_value = 1;
  byte              A5_1_supported_value = 0;

#ifdef FEATURE_RAT_PRIORITY_LIST
  boolean           mm_checkif_rat_pri_list_has_rat_local;
#endif
  ue_force_rel_mode_type           rr_nv_force_mscr_r99 = mm_nv_force_mscr_r99;
  /**********************************************************************************/
  /* Classmark1 IE is defined in 24.008 10.5.1.5. It contains the following fields: */
  /* RevisionLevel  - Type of MS (Phase1, Phase 2, R99)                             */
  /* ES_IND         - Indicates if early classmark sending is supported by the MS   */
  /* A5_1 supported - Indicates if the MS supports the A5/1 algorithm               */
  /* RF_power_capability - The current power class of the mobile (use depends on    */
  /*                       multiband capability and whether WCDMA is in use)        */
  /**********************************************************************************/
   
#ifdef FEATURE_GSM_MULTIBAND
  mb_initialise();
#endif
  status = ghdi_read_hw_classmark(&hw_classmark);
  
  if (status != GHDI_SUCCESS)
  {
    ERR("Unable to read HW classmark: status=%d",status,0,0); 
  }

  if(rr_nv_force_mscr_r99 == NV_FORCE_UE_DYNAMIC)
  {
    /* Either NV item NV_FORCE_UE_MSCR_R99_I is not set,    */
    /* or it is set to 0  ie., follow the network Rev level */
    /* set the revision level value based on the MSCR99 parameter (always true if wcdma in use) */
    if (MSCR99 || wcdma_in_use)
    {
      /* it is a R99 Network */
      classmark_revision_level = WCDMA_GSM_24008;
    }
    else /* network MSC revision level is R98 or earlier */
    {
      /* must be set to PHASE_2 for R98 or earlier networks */
      classmark_revision_level = PHASE_2;
    }
  }
  else if ((rr_nv_force_mscr_r99 == NV_FORCE_UE_REL99) || (wcdma_in_use))
  {
    /*  NV Item is set to 1, advertise as a R99 UE */
    classmark_revision_level = WCDMA_GSM_24008;
  }
  else if ( rr_nv_force_mscr_r99 == NV_FORCE_UE_REL97 )
  {
    /* It is a PHASE 2 mobile */
    classmark_revision_level = PHASE_2;
  }
  else
  {
    ERR("Invalid NV param value for FORCE_REVISION",0,0,0);
    classmark_revision_level = WCDMA_GSM_24008;
  }

  /* ES_IND should be 1 bit value */
  ASSERT((ES_IND | 0x1) == 0x1);

  if (gsm_supported)
  {
    /* if GSM is supported, use the value in ES_IND config variable */
    es_ind_value = ES_IND;

    /* NOTE that A5_1 bit is coded as 0 if algorithm is supported, and 1 if not supported */
    A5_1_supported_value = (hw_classmark.A5_1_supported ? 0x0 : 0x08);
  }
  else /* WCDMA in use and GSM is not supported */
  {
    /* 24.008 (MS not supporting GSM shall set ES_IND to 0) */
    es_ind_value = 0;

    /* 24.008 (MS not supporting GSM shall set A5_1 bit to 1) */
    A5_1_supported_value = 0x08;
  }

  /* revision level should only be 2-bit value */
  ASSERT((classmark_revision_level | 0x3) == 0x3);

  /* code the part of the classmark that is always coded the same */
  return_value = (byte) (
    (A5_1_supported_value)            | 
    (classmark_revision_level << 5)   |
    (es_ind_value << 4));

#ifdef FEATURE_GSM_MULTIBAND
  /*****************************************************************************/
  /* the encoding of classmark1's RF_power_capability depends on the supported */
  /* bands and whether WCDMA is being used or not                              */
  /*****************************************************************************/
  if (gsm_supported)
  {
    boolean rf_power_capability_encoded = FALSE;
    /*******************************************************************************/
    /* Since at least one GSM band is supported, the coding will depend on whether */
    /* WCDMA is currently being used or not                                        */
    /*******************************************************************************/
    if (wcdma_in_use)
    {
      byte band_count = rr_get_supported_band_count();
      /****************************************************************************/
      /* WCDMA is in use. For a multiband MS or an MS that doesn't support any    */
      /* GSM bands, encode the field as 0x7 (24.008 10.5.1.5)                     */
      /****************************************************************************/
#ifdef FEATURE_RAT_PRIORITY_LIST
      NAS_ENTER_CRIT_SECT(mm_rat_prioritylist_crit_sec);
      mm_checkif_rat_pri_list_has_rat_local = mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_GSM_RADIO_ACCESS);
      NAS_EXIT_CRIT_SECT(mm_rat_prioritylist_crit_sec);
#endif
      if ((band_count > 1)   ||
          (band_count == 0)  ||
#ifndef FEATURE_RAT_PRIORITY_LIST
          (is_umts_mode() && !mm_dual_mode)
#else
          (!mm_checkif_rat_pri_list_has_rat_local)
#endif /* FEATURE_RAT_PRIORITY_LIST */
         )
      {
        rf_power_capability = 0x7;
        rf_power_capability_encoded = TRUE;
      }
      /* otherwise, a single band is supported, encode the RF power capability */
      /* classmark 3 indicates which single band is supported */
    }
    
    /* if rf_power_capability not already encoded */
    if (rf_power_capability_encoded == FALSE)
    {
      /************************************************************************/
      /* To enter this condition, either WCDMA is not in use (GSM is in use), */
      /* or WCDMA is in use and a single GSM band is supported.               */
      /* In either case, code the rf_power_capability of the current GSM band */
      /************************************************************************/
      byte power_class;
      power_class = rr_get_current_rf_power_class();

      switch (power_class)
      {
        case CLASS_1:
          rf_power_capability = 0;
          break;

        case CLASS_2:
          rf_power_capability = 1;
          break;

        case CLASS_3:
          rf_power_capability = 2;
          break;

        case CLASS_4:
          rf_power_capability = 3;
          break;

        case CLASS_5:
          rf_power_capability = 4;
          break;

        default:
          ERR("Invalid power class=%d",(int)power_class,0,0);
          break;
      } /* switch */
    } /* rf_power_capability_encoded == FALSE */
  }
  else /* GSM not supported */
  {
    /* Assuming that wcdma is in use here */
    if (!wcdma_in_use) 
    {
      ERR("Error: GSM not supported and wcdma not in use?",0,0,0);
    }
    /***********************************************************************/
    /* An MS not supporting any GSM bands encodes as 0x7 (24.008 10.5.1.5) */
    /***********************************************************************/
    rf_power_capability = 0x7;
  }
#else /* multiband not supported */
  #error code not present
#endif /* multiband not supported*/

  /* add the RF_power_capability coding (should be 3 bits only) */
  ASSERT((rf_power_capability | 0x7) == 0x7);
  return_value |= rf_power_capability;

  return return_value;

#endif

} /* gs_get_classmark_1 */

/**
 * Encodes the classmark1 information in classmark1 IE form
 *
 * @return byte containing encoded classmark1 information
 */
byte gs_get_classmark_1_per_nas_stacks
(
  classmark_params_T    *params_ptrs,
  sys_modem_as_id_e_type stack_id_nas
)
{
#if defined FEATURE_DUAL_SIM ||defined FEATURE_SGLTE
  ghdi_status_T     status;
  hw_classmark_T    hw_classmark = {0};
  byte              return_value = 0;
  byte              rf_power_capability = 0;
  byte              classmark_revision_level;

  byte              es_ind_value = 1;
  byte              A5_1_supported_value = 0;

  boolean           rr_nv_force_mscr_r99;

#ifdef FEATURE_RAT_PRIORITY_LIST
  boolean           mm_checkif_rat_pri_list_has_rat_local = FALSE;
#endif
  /**********************************************************************************/
  /* Classmark1 IE is defined in 24.008 10.5.1.5. It contains the following fields: */
  /* RevisionLevel  - Type of MS (Phase1, Phase 2, R99)                             */
  /* ES_IND         - Indicates if early classmark sending is supported by the MS   */
  /* A5_1 supported - Indicates if the MS supports the A5/1 algorithm               */
  /* RF_power_capability - The current power class of the mobile (use depends on    */
  /*                       multiband capability and whether WCDMA is in use)        */
  /**********************************************************************************/
  if(IS_NOT_VALID_STACK_ID(stack_id_nas) ||
     IS_NOT_VALID_SUB_ID(params_ptrs->as_id))
  {
    ERR("Invalid STACK ID:%d or SUB ID:%d", stack_id_nas+1,params_ptrs->as_id+1,0);
    return gs_get_classmark_1
    (
      params_ptrs->gsm_supported,
      params_ptrs->wcdma_in_use,
      params_ptrs->MSCR99
    );
  }

  rr_nv_force_mscr_r99 = mm_nv_force_mscr_r99_sim[params_ptrs->as_id];
#ifdef FEATURE_GSM_MULTIBAND
  mb_initialise();
#endif
  status = ghdi_read_hw_classmark(&hw_classmark);
  
  if (status != GHDI_SUCCESS)
  {
    ERR("Unable to read HW classmark: status=%d",status,0,0); 
  }

  if(rr_nv_force_mscr_r99 == NV_FORCE_UE_DYNAMIC)
  {
    /* Either NV item NV_FORCE_UE_MSCR_R99_I is not set,    */
    /* or it is set to 0  ie., follow the network Rev level */
    /* set the revision level value based on the MSCR99 parameter (always true if wcdma in use) */
    if (params_ptrs->MSCR99 || params_ptrs->wcdma_in_use)
    {
      /* it is a R99 Network */
      classmark_revision_level = WCDMA_GSM_24008;
    }
    else /* network MSC revision level is R98 or earlier */
    {
      /* must be set to PHASE_2 for R98 or earlier networks */
      classmark_revision_level = PHASE_2;
    }
  }
  else if ((rr_nv_force_mscr_r99 == NV_FORCE_UE_REL99) || (params_ptrs->wcdma_in_use))
  {
    /*  NV Item is set to 1, advertise as a R99 UE */
    classmark_revision_level = WCDMA_GSM_24008;
  }
  else if (rr_nv_force_mscr_r99 == NV_FORCE_UE_REL97)
  {
    /* It is a PHASE 2 mobile */
    classmark_revision_level = PHASE_2;
  }
  else
  {
    ERR("Invalid NV param value for FORCE_REVISION",0,0,0);
    classmark_revision_level = WCDMA_GSM_24008;
  }

  /* ES_IND should be 1 bit value */
  ASSERT((ES_IND | 0x1) == 0x1);

  if (params_ptrs->gsm_supported)
  {
    /* if GSM is supported, use the value in ES_IND config variable */
    es_ind_value = ES_IND;

    /* NOTE that A5_1 bit is coded as 0 if algorithm is supported, and 1 if not supported */
    A5_1_supported_value = (hw_classmark.A5_1_supported ? 0x0 : 0x08);
  }
  else /* WCDMA in use and GSM is not supported */
  {
    /* 24.008 (MS not supporting GSM shall set ES_IND to 0) */
    es_ind_value = 0;

    /* 24.008 (MS not supporting GSM shall set A5_1 bit to 1) */
    A5_1_supported_value = 0x08;
  }

  /* revision level should only be 2-bit value */
  ASSERT((classmark_revision_level | 0x3) == 0x3);

  /* code the part of the classmark that is always coded the same */
  return_value = 
    (A5_1_supported_value)            | 
    (classmark_revision_level << 5)   |
    (es_ind_value << 4);

#ifdef FEATURE_GSM_MULTIBAND
  /*****************************************************************************/
  /* the encoding of classmark1's RF_power_capability depends on the supported */
  /* bands and whether WCDMA is being used or not                              */
  /*****************************************************************************/
  if (params_ptrs->gsm_supported)
  {
    boolean rf_power_capability_encoded = FALSE;
    /*******************************************************************************/
    /* Since at least one GSM band is supported, the coding will depend on whether */
    /* WCDMA is currently being used or not                                        */
    /*******************************************************************************/
    if (params_ptrs->wcdma_in_use)
    {
#ifdef FEATURE_DUAL_SIM
      byte band_count = rr_ds_get_supported_band_count(params_ptrs->as_id);
#else
      byte band_count = rr_get_supported_band_count();
#endif
      /****************************************************************************/
      /* WCDMA is in use. For a multiband MS or an MS that doesn't support any    */
      /* GSM bands, encode the field as 0x7 (24.008 10.5.1.5)                     */
      /****************************************************************************/
#ifdef FEATURE_RAT_PRIORITY_LIST
      NAS_ENTER_CRIT_SECT(mm_rat_prioritylist_crit_sec);
      mm_checkif_rat_pri_list_has_rat_local = mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info_sim[stack_id_nas],SYS_RAT_GSM_RADIO_ACCESS);
      NAS_EXIT_CRIT_SECT(mm_rat_prioritylist_crit_sec);
#endif
      if ((band_count > 1)   ||
          (band_count == 0)  ||
#ifndef FEATURE_RAT_PRIORITY_LIST
          (is_umts_mode_per_nas_stacks(stack_id_nas) && !mm_dual_mode_sim[stack_id_nas])
#else
          (!mm_checkif_rat_pri_list_has_rat_local)
#endif /* FEATURE_RAT_PRIORITY_LIST */
         )
      {
        rf_power_capability = 0x7;
        rf_power_capability_encoded = TRUE;
      }
      /* otherwise, a single band is supported, encode the RF power capability */
      /* classmark 3 indicates which single band is supported */
    }
    
    /* if rf_power_capability not already encoded */
    if (rf_power_capability_encoded == FALSE)
    {
      /************************************************************************/
      /* To enter this condition, either WCDMA is not in use (GSM is in use), */
      /* or WCDMA is in use and a single GSM band is supported.               */
      /* In either case, code the rf_power_capability of the current GSM band */
      /************************************************************************/
      byte power_class;
#ifdef FEATURE_DUAL_SIM
      power_class = rr_ds_get_current_rf_power_class( params_ptrs->as_id );
#else
      power_class = rr_get_current_rf_power_class();
#endif

      switch (power_class)
      {
        case CLASS_1:
          rf_power_capability = 0;
          break;

        case CLASS_2:
          rf_power_capability = 1;
          break;

        case CLASS_3:
          rf_power_capability = 2;
          break;

        case CLASS_4:
          rf_power_capability = 3;
          break;

        case CLASS_5:
          rf_power_capability = 4;
          break;

        default:
          ERR("Invalid power class=%d",(int)power_class,0,0);
          break;
      } /* switch */
    } /* rf_power_capability_encoded == FALSE */
  }
  else /* GSM not supported */
  {
    /* Assuming that wcdma is in use here */
    if (!params_ptrs->wcdma_in_use) 
    {
      ERR("Error: GSM not supported and wcdma not in use?",0,0,0);
    }
    /***********************************************************************/
    /* An MS not supporting any GSM bands encodes as 0x7 (24.008 10.5.1.5) */
    /***********************************************************************/
    rf_power_capability = 0x7;
  }
#else /* multiband not supported */
  #error code not present
#endif /* multiband not supported*/

  /* add the RF_power_capability coding (should be 3 bits only) */
  ASSERT((rf_power_capability | 0x7) == 0x7);
  return_value |= rf_power_capability;

  return return_value;

#else

  return gs_get_classmark_1
  (
    params_ptrs->gsm_supported,
    params_ptrs->wcdma_in_use,
    params_ptrs->MSCR99
  );

#endif
}
/**
 * encodes the classmark2 information in classmark2 IE format (24.008 10.5.1.6)
 * 
 * @param classmark_2_ptr pointer to classmark2 IE where encoded classmark2 information will be stored
 * 
 * @param gsm_supported   TRUE if GSM RAT is supported, FALSE otherwise
 * 
 * @param wcdma_supported TRUE if WCDMA RAT is supported, FALSE otherwise
 * 
 * @param wcdma_or_lte_in_use    TRUE if WCDMA or LTE is the active RAT, FALSE if GSM is the active RAT
 * 
 * @param MSCR99          TRUE if the network is a R99 or later, FALSE if the network is R98 or earlier
 *                        based on MSCR (MSC revision) value broadcast by network. Should always be set
 *                        to TRUE if called from WCDMA (RRC)
 */
void gs_read_coded_classmark_2(
  classmark_2_ie_T  *classmark_2_ptr,
  boolean           gsm_supported,
  boolean           wcdma_supported,
  boolean           wcdma_or_lte_in_use,
  boolean           MSCR99
)
{
#ifdef FEATURE_DUAL_SIM

  MSG_FATAL_DS( 0,"DUAL SIM: Wrong API called gs_read_coded_classmark_2()", 0,0,0 );

#else

  byte index = 0;
  hw_classmark_T hw_classmark;
  byte data[3];
  boolean A5_2_supported;
  boolean A5_3_supported;
  byte freq_capability;
  boolean classmark_3_available=FALSE;
  ghdi_status_T status;
#ifdef FEATURE_CGPS_UMTS_CP_GSM
  cgps_classmark_cb_type    cgps_classmark_local_cb_fp;
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  boolean                   mm_checkif_rat_pri_list_has_rat_local;
#endif
  byte                      ss_screening_indicator_local;
#if defined(FEATURE_MMGPS)
  lsm_ue_pos_capability_struct_type lsm_pos_capability;
#elif defined(FEATURE_CGPS_UMTS_CP_GSM)
  gsclassmark_cgps_pos_capability_struct_type   sm_pos_capability;
#endif
  classmark_3_ie_T classmark3;
#ifdef FEATURE_GSM_MULTIBAND
  byte no_of_bands;

  mb_initialise();   /* make sure multiband_info is correctly set up */
#endif

#ifdef FEATURE_SGLTE
  if(MM_IS_IN_SGLTE_STATE_4)
  {
    wcdma_supported = FALSE;
  }
#endif
  ASSERT(classmark_2_ptr != NULL);

  status = ghdi_read_hw_classmark(&hw_classmark);

  if (status != GHDI_SUCCESS)
  {
    ERR("Unable to read HW classmark: status=%d",status,0,0); 
  }

  classmark_2_ptr->length = 0x03;   /* Length of the classmark 2 IE */

  /* initialize all bits to 0 */
  for (index = 0; index < 3; index++)
  {
    data[index] = 0;
  }

  /*************************************************************************************/
  /* PGSM and EGSM bands are mutually exclusive (cannot allow both at the same time    */
  /* since they overlap). Frequency capability indicates support for EGSM band if the  */
  /* GSM900 band is currently being used. If some other band is being used or WCDMA is */
  /* being used, the frequency capability value should be set to 0.                    */
  /*************************************************************************************/
  if (wcdma_or_lte_in_use)
  {
    freq_capability = 0;
  }
  /* WCDMA not in use, if gsm is supported*/
  else if (gsm_supported)
#ifdef FEATURE_GSM_MULTIBAND
  {
    byte current_band;
    current_band = (byte) rr_get_current_band();
    if ((current_band == (byte) BAND_EGSM) || 
        (current_band == (byte) BAND_PGSM) )
    {
      if (rr_is_supported_band(BAND_EGSM))
      {
         freq_capability = 1; /* MS supports E-GSM or R-GSM band */
      }
      else
      {
         freq_capability = 0; /* MS doesn't support E-GSM or R-GSM band */
      }
    }
    else
    {
      freq_capability = 0; /* Bit conveys no info about support/non-support of E-GSM or R-GSM bands */
    }
  }
#else
  #error code not present
#endif
  else /* gsm not supported */
  {
    ERR("GSM not supported/wcdma not in use?",0,0,0);
    freq_capability = 0;
  }

  /***********************************************/
  /* HW classmark indicates A5 algorithm support */
  /***********************************************/
  A5_2_supported  =        hw_classmark.A5_2_supported;
  A5_3_supported  =        hw_classmark.A5_3_supported;

  /*************************************************************/
  /* If A5 algorithms other than 1 through 3 are supported     */
  /* classmark 3 information needs to be generated to indicate */
  /* which ones are supported                                  */
  /*************************************************************/
  classmark_3_available = gs_read_coded_classmark_3
     (
       (classmark_3_ie_T*)(&classmark3),
       gsm_supported,
       wcdma_supported,
       wcdma_or_lte_in_use,
       MSCR99
     );
  if ((hw_classmark.A5_4_supported) ||
      (hw_classmark.A5_5_supported) ||
      (hw_classmark.A5_6_supported) ||
      (hw_classmark.A5_7_supported))
  {
    classmark_3_available = TRUE;
  }
#ifdef FEATURE_GSM_MULTIBAND
  else /* A5 algorithms 4-7 not supported */
  {
    /***************************************************************/
    /* If more than one band is supported, classmark 3 information */
    /* needs to be generated to indicate which bands are supported */
    /***************************************************************/
    no_of_bands = rr_get_supported_band_count();

    if (no_of_bands > 1)
    {
      classmark_3_available = TRUE;
    }
  }
#endif /* multiband not supported */
  /* GSM only not supporting multiband does not require classmark 3 */

  /*******************************************************************/
  /* If WCDMA is supported, always indicate that classmark 3 is used */
  /*******************************************************************/
  if (wcdma_supported)
  {
    /*******************************************************************/
    /* User Prefer/Forced to GSM only mode                             */
    /*******************************************************************/
#ifdef FEATURE_RAT_PRIORITY_LIST
      NAS_ENTER_CRIT_SECT(mm_rat_prioritylist_crit_sec);
      mm_checkif_rat_pri_list_has_rat_local = mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_UMTS_RADIO_ACCESS);
      mm_checkif_rat_pri_list_has_rat_local |= mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_TDS_RADIO_ACCESS);
#ifdef FEATURE_SGLTE
      if(MM_IS_IN_SGLTE_STATE_4)
      {
        mm_checkif_rat_pri_list_has_rat_local = FALSE;
      }
#endif
      NAS_EXIT_CRIT_SECT(mm_rat_prioritylist_crit_sec);
#endif    
#ifndef FEATURE_RAT_PRIORITY_LIST
    if(!(is_gsm_mode() && (!mm_dual_mode)))    /* not in GSM ONLY mode */
#else
    if(mm_checkif_rat_pri_list_has_rat_local)
#endif /* FEATURE_RAT_PRIORITY_LIST */
    {
      classmark_3_available = TRUE;
    }
  }
  
  /**********************************************************/
  /* Code the fist byte of the IE using the common function */
  /**********************************************************/
  data[0] = gs_get_classmark_1(gsm_supported,wcdma_or_lte_in_use,MSCR99);

  if (gsm_supported)
  {
    /**********************************/
    /* Code the second byte of the IE */
    /**********************************/
    if (ps_capability == PS_CAPABILITY_PRESENT)
    {
      data[1] |= 0x40;
    }
  }
  else /* GSM not supported, WCDMA must be in use */
  {
    /* 24.008: An MS not supporting GSM shall set this bit to 0 */
    /* already set to 0, nothing to do */
  }
  ss_screening_indicator_local = ss_screening_indicator;
  data[1] |= (byte) (ss_screening_indicator_local << 4);

  if (sm_capability == 1)
  {
    data[1] |= 0x08;
  }

  /* VBS and VGCS support are both coded as 0 (not shown) */

  if (freq_capability == 1)
  {
    data[1] |= 0x01;
  }

  /**********************************/
  /* Code the third byte of the IE  */
  /**********************************/
  if (classmark_3_available)
  {
    data[2] |= 0x80;
  }

  /* LCSVA Capability coded as 0 (not shown) */

#if defined(FEATURE_MMGPS)
  /* Retrieve Capability information from LSM task */
  lsm_gsm_ue_pos_capability_get( &lsm_pos_capability );

  if ( lsm_pos_capability.lcs_value_added_lcs_supported )
  {
    /* Set LCSVA bit */
    data[2] |= 0x20;
  }
#elif defined(FEATURE_CGPS_UMTS_CP_GSM)
  /* Retrieve Capability information from SM task */
  cgps_classmark_local_cb_fp = cgps_classmark_cb_fp;
  if( cgps_classmark_local_cb_fp )
  {
    cgps_classmark_local_cb_fp( &sm_pos_capability );

    if ( sm_pos_capability.lcs_value_added_lcs_supported )
    {
      /* Set LCSVA bit */
      data[2] |= 0x20;
    }
  }
#endif

  /* USC2 bit coded as 0 (not shown) */

  /* SoLSA bit coded as 0 (not shown) */

#ifdef FEATURE_CCBS
  data[2] |= 0x04;  // CMSP bit is set
#else
  /* CMSP bit coded as 0 (not shown) */
#endif /* FEATURE_CCBS */

  if (A5_2_supported)
  {
    data[2] |= 0x01;
  }

  if (A5_3_supported)
  {
    data[2] |= 0x02;
  }


  for (index = 0; index < 3; index++)
  {
    classmark_2_ptr->data[index] = data[index];
  }

#endif
}

/**
 * gs_read_coded_classmark_2_per_nas_stacks
 */
void gs_read_coded_classmark_2_per_nas_stacks(
  classmark_2_ie_T  *classmark_2_ptr,
  classmark_params_T   *params_ptr,
  sys_modem_as_id_e_type stack_id_nas
)
{
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  byte index = 0;
  hw_classmark_T hw_classmark;
  byte data[3];
  boolean A5_2_supported;
  boolean A5_3_supported;
  byte freq_capability;
  boolean classmark_3_available=FALSE;
  ghdi_status_T status;
#ifdef FEATURE_CGPS_UMTS_CP_GSM
  cgps_classmark_cb_type    cgps_classmark_local_cb_fp;
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  boolean                   mm_checkif_rat_pri_list_has_rat_local;
#endif
  byte                      ss_screening_indicator_local;
#if defined(FEATURE_MMGPS)
  lsm_ue_pos_capability_struct_type lsm_pos_capability;
#elif defined(FEATURE_CGPS_UMTS_CP_GSM)
  gsclassmark_cgps_pos_capability_struct_type   sm_pos_capability;
#endif
  classmark_3_ie_T classmark3;
#ifdef FEATURE_GSM_MULTIBAND
  byte no_of_bands;
#endif
  
#ifdef FEATURE_SGLTE
  if(MM_IS_IN_SGLTE_STATE_4)
  {
    params_ptr->wcdma_supported = FALSE;
  }
#endif

  if(IS_NOT_VALID_STACK_ID(stack_id_nas) ||
     IS_NOT_VALID_SUB_ID(params_ptr->as_id))
  {
    ERR("Invalid STACK ID:%d or SUB ID:%d", stack_id_nas+1,params_ptr->as_id+1,0);
    gs_read_coded_classmark_2
    (
      classmark_2_ptr,
      params_ptr->gsm_supported,
      params_ptr->wcdma_supported,
      params_ptr->wcdma_in_use,
      params_ptr->MSCR99
    );
  }

#ifdef FEATURE_GSM_MULTIBAND
  mb_initialise();   /* make sure multiband_info is correctly set up */
#endif

  ASSERT(classmark_2_ptr != NULL);

  status = ghdi_read_hw_classmark(&hw_classmark);

  if (status != GHDI_SUCCESS)
  {
    ERR("Unable to read HW classmark: status=%d",status,0,0); 
  }

  classmark_2_ptr->length = 0x03;   /* Length of the classmark 2 IE */

  /* initialize all bits to 0 */
  for (index = 0; index < 3; index++)
  {
    data[index] = 0;
  }

  /*************************************************************************************/
  /* PGSM and EGSM bands are mutually exclusive (cannot allow both at the same time    */
  /* since they overlap). Frequency capability indicates support for EGSM band if the  */
  /* GSM900 band is currently being used. If some other band is being used or WCDMA is */
  /* being used, the frequency capability value should be set to 0.                    */
  /*************************************************************************************/
  if (params_ptr->wcdma_in_use)
  {
    freq_capability = 0;
  }
  /* WCDMA not in use, if gsm is supported*/
  else if (params_ptr->gsm_supported)
#ifdef FEATURE_GSM_MULTIBAND
  {
    byte current_band;
#ifdef FEATURE_DUAL_SIM
    current_band = rr_ds_get_current_band(params_ptr->as_id);
#else
    current_band = (byte) rr_get_current_band();
#endif
    if ((current_band == BAND_EGSM) || 
        (current_band == BAND_PGSM) )
    {
#ifdef FEATURE_DUAL_SIM
      if (rr_ds_is_supported_band(BAND_EGSM,params_ptr->as_id))
#else
      if (rr_is_supported_band(BAND_EGSM))
#endif
      {
         freq_capability = 1; /* MS supports E-GSM or R-GSM band */
      }
      else
      {
         freq_capability = 0; /* MS doesn't support E-GSM or R-GSM band */
      }
    }
    else
    {
      freq_capability = 0; /* Bit conveys no info about support/non-support of E-GSM or R-GSM bands */
    }
  }
#else
  #error code not present
#endif
  else /* gsm not supported */
  {
    ERR("GSM not supported/wcdma not in use?",0,0,0);
    freq_capability = 0;
  }

  /***********************************************/
  /* HW classmark indicates A5 algorithm support */
  /***********************************************/
  A5_2_supported  =        hw_classmark.A5_2_supported;
  A5_3_supported  =        hw_classmark.A5_3_supported;

  /*************************************************************/
  /* If A5 algorithms other than 1 through 3 are supported     */
  /* classmark 3 information needs to be generated to indicate */
  /* which ones are supported                                  */
  /*************************************************************/
  classmark_3_available = gs_read_coded_classmark_3_per_nas_stacks
     (
       (classmark_3_ie_T*)(&classmark3),
       params_ptr,
       stack_id_nas
     );
  if ((hw_classmark.A5_4_supported) ||
      (hw_classmark.A5_5_supported) ||
      (hw_classmark.A5_6_supported) ||
      (hw_classmark.A5_7_supported))
  {
    classmark_3_available = TRUE;
  }
#ifdef FEATURE_GSM_MULTIBAND
  else /* A5 algorithms 4-7 not supported */
  {
    /***************************************************************/
    /* If more than one band is supported, classmark 3 information */
    /* needs to be generated to indicate which bands are supported */
    /***************************************************************/
#ifdef FEATURE_DUAL_SIM
    no_of_bands = rr_ds_get_supported_band_count(params_ptr->as_id);
#else
    no_of_bands = rr_get_supported_band_count();
#endif
    if (no_of_bands > 1)
    {
      classmark_3_available = TRUE;
    }
  }
#endif /* multiband not supported */
  /* GSM only not supporting multiband does not require classmark 3 */

  /*******************************************************************/
  /* If WCDMA is supported, always indicate that classmark 3 is used */
  /*******************************************************************/
  if (params_ptr->wcdma_supported)
  {
    /*******************************************************************/
    /* User Prefer/Forced to GSM only mode                             */
    /*******************************************************************/
#ifdef FEATURE_RAT_PRIORITY_LIST
      NAS_ENTER_CRIT_SECT(mm_rat_prioritylist_crit_sec);
      mm_checkif_rat_pri_list_has_rat_local = mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info_sim[stack_id_nas],SYS_RAT_UMTS_RADIO_ACCESS);
      mm_checkif_rat_pri_list_has_rat_local |= mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info_sim[stack_id_nas],SYS_RAT_TDS_RADIO_ACCESS);      
      NAS_EXIT_CRIT_SECT(mm_rat_prioritylist_crit_sec);
#ifdef FEATURE_SGLTE
      if(MM_IS_IN_SGLTE_STATE_4)
      {
        mm_checkif_rat_pri_list_has_rat_local = FALSE;
      }
#endif

#endif    
#ifndef FEATURE_RAT_PRIORITY_LIST
    if(!(is_gsm_mode_per_nas_stacks(stack_id_nas) && (!mm_dual_mode_sim[stack_id_nas])))    /* not in GSM ONLY mode */
#else
    if(mm_checkif_rat_pri_list_has_rat_local)
#endif /* FEATURE_RAT_PRIORITY_LIST */
    {
      classmark_3_available = TRUE;
    }
  }
  
  /**********************************************************/
  /* Code the fist byte of the IE using the common function */
  /**********************************************************/
  data[0] = gs_get_classmark_1_per_nas_stacks(params_ptr,stack_id_nas);

  if (params_ptr->gsm_supported)
  {
    /**********************************/
    /* Code the second byte of the IE */
    /**********************************/
    if (ps_capability == PS_CAPABILITY_PRESENT)
    {
      data[1] |= 0x40;
    }
  }
  else /* GSM not supported, WCDMA must be in use */
  {
    /* 24.008: An MS not supporting GSM shall set this bit to 0 */
    /* already set to 0, nothing to do */
  }
  ss_screening_indicator_local = ss_screening_indicator;
  data[1] |= ((byte) ss_screening_indicator_local << 4);

  if (sm_capability == 1)
  {
    data[1] |= 0x08;
  }

  /* VBS and VGCS support are both coded as 0 (not shown) */

  if (freq_capability == 1)
  {
    data[1] |= 0x01;
  }

  /**********************************/
  /* Code the third byte of the IE  */
  /**********************************/
  if (classmark_3_available)
  {
    data[2] |= 0x80;
  }

  /* LCSVA Capability coded as 0 (not shown) */

#if defined(FEATURE_MMGPS)
  /* Retrieve Capability information from LSM task */
  lsm_gsm_ue_pos_capability_get( &lsm_pos_capability );

  if ( lsm_pos_capability.lcs_value_added_lcs_supported )
  {
    /* Set LCSVA bit */
    data[2] |= 0x20;
  }
#elif defined(FEATURE_CGPS_UMTS_CP_GSM)
  /* Retrieve Capability information from SM task */
  cgps_classmark_local_cb_fp = cgps_classmark_cb_fp;
  if( cgps_classmark_local_cb_fp )
  {
    cgps_classmark_local_cb_fp( &sm_pos_capability );

    if ( sm_pos_capability.lcs_value_added_lcs_supported )
    {
      /* Set LCSVA bit */
      data[2] |= 0x20;
    }
  }
#endif

  /* USC2 bit coded as 0 (not shown) */

  /* SoLSA bit coded as 0 (not shown) */

#ifdef FEATURE_CCBS
  data[2] |= 0x04;  // CMSP bit is set
#else
  /* CMSP bit coded as 0 (not shown) */
#endif /* FEATURE_CCBS */

  if (A5_2_supported)
  {
    data[2] |= 0x01;
  }

  if (A5_3_supported)
  {
    data[2] |= 0x02;
  }


  for (index = 0; index < 3; index++)
  {
    classmark_2_ptr->data[index] = data[index];
  }

#else

  gs_read_coded_classmark_2
  (
    classmark_2_ptr,
    params_ptr->gsm_supported,
    params_ptr->wcdma_supported,
    params_ptr->wcdma_in_use,
    params_ptr->MSCR99
  );

#endif

} /* gs_read_coded_classmark_2 */


/**
 * gs_read_coded_classmark_2_per_subs
 * 
 * This function is called by WRRC.  If the sub configuration says it is 
 *  an SGLTE device then use ps_stack_id
 */
void gs_read_coded_classmark_2_per_subs(
  classmark_2_ie_T  *classmark_2_ptr,
  classmark_params_T   *params_ptr
)
{
  mm_as_id_e_type stack_id_nas = (mm_as_id_e_type)params_ptr->as_id;
#if defined(FEATURE_SGLTE)
  if(!IS_NOT_VALID_SUB_ID(params_ptr->as_id))
  {
    if(MM_SUB_IS_SGLTE_SUB((mm_as_id_e_type)params_ptr->as_id))  
    {
      stack_id_nas = mm_ps_stack_id;
    }
  }
#endif
  gs_read_coded_classmark_2_per_nas_stacks(classmark_2_ptr,params_ptr,
                                           (sys_modem_as_id_e_type)stack_id_nas);
}


/**
 * encodes the given power class as an associated radio capability value (defined
 * as the binary encoding of the power class)
 * 
 * @param power_class power class enumeration
 * 
 * @return encoded associated radio capability value
 */
static int gs_encode_associated_radio_capability(byte power_class)
{
  int capability = 0;
  switch (power_class)
  {
    case CLASS_1:
      capability = 1;
      break;

    case CLASS_2:
      capability = 2;
      break;

    case CLASS_3:
      capability = 3;
      break;

    case CLASS_4:
      capability = 4;
      break;

    case CLASS_5:
      capability = 5;
      break;

    default:
      ERR("Unknown power class=%d",(int) power_class,0,0);
      break;
  } /* switch */

  return capability;
} /* gs_encode_associated_radio_capability */

/**
 * encodes the classmark3 information into the provided classmark3 IE
 * 
 * @param classmark_3_ptr pointer to classmark3 IE where encoded data should be stored
 * 
 * @param gsm_supported   TRUE if GSM RAT is supported, FALSE otherwise
 * 
 * @param wcdma_supported TRUE if WCDMA RAT is supported, FALSE otherwise
 * 
 * @param wcdma_in_use    TRUE if WCDMA is the active RAT, FALSE if GSM is the active RAT
 * 
 * @param MSCR99          TRUE if the network is a R99 or later, FALSE if the network is R98 or earlier
 *                        based on MSCR (MSC revision) value broadcast by network. Should always be set
 *                        to TRUE if called from WCDMA (RRC)
 */
boolean gs_read_coded_classmark_3(
  classmark_3_ie_T *classmark_3_ptr,
  boolean           gsm_supported,
  boolean           wcdma_supported,
  boolean           wcdma_in_use,
  boolean           MSCR99
)
{
#ifdef FEATURE_DUAL_SIM

  MSG_FATAL_DS( 0,"DUAL SIM: Wrong API called gs_read_coded_classmark_3()", 0,0,0 );
  return FALSE;

#else

  byte index = 0;
  hw_classmark_T hw_classmark = {0};
  /*
  ** This field to indicate if some of the capability of classmark - 3 supported by UE.
  */
  boolean CM3_supported = FALSE;

  byte data[MAX_CLASSMARK3_DATA_SIZE]= {0};

#ifndef FEATURE_REL_6_SUPPORT  
  boolean include_rel6_IE = FALSE;
#endif
  boolean include_rel7_IE = FALSE;
  boolean include_rel8_IE = FALSE;

  boolean A5_4_supported;
  boolean A5_5_supported;
  boolean A5_6_supported;
  boolean A5_7_supported;
  ghdi_status_T status;
  gs_bit_stream_T bit_stream;
#if defined(FEATURE_MMGPS)
  lsm_ue_pos_capability_struct_type lsm_pos_capability;
#elif defined(FEATURE_CGPS_UMTS_CP_GSM)
  gsclassmark_cgps_pos_capability_struct_type   sm_pos_capability;
#endif

  boolean PCS1900_supported; 
  boolean DCS1800_supported;
  boolean PGSM_supported;
  boolean EGSM_supported;

  byte DCS1800_rf_power_class = 0;
  byte GSM900_rf_power_class  = 0;

#ifdef FEATURE_GSM_DTM
  byte DTM_GPRS_multislot_subclass  = gmm_nv_dtm_multislot_subclass;
  byte DTM_EGPRS_multislot_subclass = gmm_nv_dtm_multislot_subclass;
#endif
#if defined(FEATURE_CGPS_UMTS_CP_GSM)
  cgps_classmark_cb_type    cgps_classmark_local_cb_fp;
#endif
#if (defined(FEATURE_LTE) && defined(FEATURE_RAT_PRIORITY_LIST))
  boolean                   mm_checkif_rat_pri_list_has_rat_local;
#endif

#if defined(FEATURE_GSM_R_SACCH) || defined(FEATURE_GSM_R_FACCH) 
#ifndef FEATURE_REL_6_SUPPORT
 include_rel6_IE = TRUE;
#endif
#endif

#if defined(FEATURE_LTE)
  boolean eutra_FDD_TDD_support = FALSE;
  include_rel7_IE = TRUE;
#else
  if(mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL7)
  {
    include_rel7_IE = TRUE;
  }
#endif
#if defined(FEATURE_LTE)
  include_rel8_IE = TRUE;
#endif
#ifdef FEATURE_VAMOS
  include_rel7_IE = TRUE;
  include_rel8_IE = TRUE;
#endif
#ifdef FEATURE_GSM_MULTIBAND
  mb_initialise();
#endif
  /* initialize the bit stream */
  gs_bit_stream_initialize(&bit_stream,data,sizeof(data));

#ifdef FEATURE_SGLTE
  if(MM_IS_IN_SGLTE_STATE_4)
  {
    wcdma_supported = FALSE;
  }
#endif

  /*******************************************************************************/
  /* HW classmark contains information related to A5 support and band support in */
  /* single band MSs                                                             */
  /*******************************************************************************/
  status = ghdi_read_hw_classmark(&hw_classmark);

  if (status != GHDI_SUCCESS)
  {
    ERR("ERROR: Unable to read classmark: status=%d",status,0,0); 
  }


  gs_bit_stream_fill(&bit_stream,MAX_CLASSMARK3_DATA_SIZE,0);

  /* start coding the classmark 3 value part using the bit stream */

  /* spare bit */
  gs_bit_stream_insert_spare(&bit_stream,1);

#ifdef FEATURE_GSM_MULTIBAND

  PCS1900_supported = rr_is_supported_band(BAND_PCS);
  DCS1800_supported = rr_is_supported_band(BAND_DCS);

  /* determine whether DCS or PCS are in use by the current NW */
  /* these bands are mutually exclusive, and cannot BOTH be listed in the classmark */
  /* If WCDMA is active (RRC calling this function) just use explicit band support  */
  if (wcdma_in_use == FALSE)
  {
    if (PCS1900_supported && DCS1800_supported)
    {
      if (rr_get_band_indicator_for_nas(TRUE) == SYS_BAND_GSM_PCS_1900)
      {
        DCS1800_supported = FALSE;
      }
      else
      {
        PCS1900_supported = FALSE;
      }
    }
  }
  
  MSG_HIGH_2("DCS support %1d, PCS support %1d", (int)DCS1800_supported,(int)PCS1900_supported);
#else
  #error code not present
#endif /* FEATURE_GSM_MULTIBAND */

  /* Encode the MultibandSupport Classmark3IE Field (not same as FEATURE_GSM_MULTIBAND ifdef) */
  {
    /* figure out which bands are supported */
#ifdef FEATURE_GSM_MULTIBAND
    if (gsm_supported)
    {
      PGSM_supported    = rr_is_supported_band(BAND_PGSM);
      EGSM_supported    = rr_is_supported_band(BAND_EGSM);
  
      /* get the power class values */
      if (DCS1800_supported)
      {
        DCS1800_rf_power_class = rr_get_band_rf_power_class(BAND_DCS);
      }

      if (PGSM_supported)
      {
        GSM900_rf_power_class = rr_get_band_rf_power_class(BAND_PGSM);
      }

      if (EGSM_supported)
      {
        GSM900_rf_power_class = rr_get_band_rf_power_class(BAND_EGSM);
      }

      /************************************************************/
      /* Semantic rule indicates that a single band MS shall not  */
      /* indicate the band it supports in the multiband supported */
      /* field                                                    */
      /************************************************************/
      if (rr_get_supported_band_count() <= 1)
      {
        /* all band supported set to false */
        DCS1800_supported = FALSE;
        PGSM_supported    = FALSE;
        EGSM_supported    = FALSE;
      }
    }
    else /* GSM is not supported */
    {
      /* code all as false */
      PCS1900_supported = FALSE;
      DCS1800_supported = FALSE;
      PGSM_supported    = FALSE;
      EGSM_supported    = FALSE;
    }
#else /* multiband not supported */
    #error code not present
#endif /* FEATURE_GSM_MULTIBAND */

    /* PGSM & EGSM are mutually exclusive */
    if (PGSM_supported && EGSM_supported)
    {
       /* As per 3GPP TS 24.008 Sec: 10.5.1.7,                           *
        *   "indication of support of P-GSM band or, E-GSM or R-GSM band *
        *   is mutually exclusive" and both should not be set in         *
        *   MS Classmark3, at the same time.  Also since E-GSM is a      *
        *   super-set of P-GSM as E-GSM includes all ARFCNs in P-GSM and *
        *   has additional ARFCNs, we'll set PGSM_supported to FALSE.    */
       MSG_MED_0 ("Mutually exclusive PGSM_supported and EGSM_supported are both enabled!!");
       PGSM_supported    = FALSE;
    }

    /* DCS 1800 support */
    gs_bit_stream_insert_field(&bit_stream,1,DCS1800_supported);

    /* EGSM or RGSM support */
    gs_bit_stream_insert_field(&bit_stream,1,EGSM_supported);

    /* PGSM support */
    gs_bit_stream_insert_field(&bit_stream,1,PGSM_supported);

    if (DCS1800_supported || EGSM_supported || PGSM_supported)
    {
      CM3_supported = TRUE;
    }
    
  } /* Encode MultibandSupport Classmark3IE Field */

  /* next depends on multibands that are supported */
  A5_4_supported  =        hw_classmark.A5_4_supported;
  A5_5_supported  =        hw_classmark.A5_5_supported;
  A5_6_supported  =        hw_classmark.A5_6_supported;
  A5_7_supported  =        hw_classmark.A5_7_supported;
  /* A5 bits are the same irrespective of multiband supported flavor */
  gs_bit_stream_insert_field(&bit_stream,1,A5_7_supported);
  gs_bit_stream_insert_field(&bit_stream,1,A5_6_supported);
  gs_bit_stream_insert_field(&bit_stream,1,A5_5_supported);
  gs_bit_stream_insert_field(&bit_stream,1,A5_4_supported);

  if (A5_4_supported || A5_5_supported || A5_6_supported || A5_7_supported)
  {
    CM3_supported = TRUE;
  }
  
  /* MultibandSupport Field = 000 */
  if (DCS1800_supported == FALSE && EGSM_supported == FALSE && PGSM_supported == FALSE)
  {
    /* nothing is inserted */
  }
  /* MultibandSupport Field = 101 or 110 */
  else if ((DCS1800_supported && (EGSM_supported == FALSE) && PGSM_supported) ||
           (DCS1800_supported && EGSM_supported && (PGSM_supported == FALSE))
          )
  {
    /* Associated Radio Capability 2 (capability of DCS 1800) */
    gs_bit_stream_insert_field(
      &bit_stream,
      4,
      (uint32) gs_encode_associated_radio_capability(DCS1800_rf_power_class)
    );

    /* Associated Radio Capability 1 (capability of GSM_900 band (either PGSM or EGSM)) */
    gs_bit_stream_insert_field(
      &bit_stream,
      4,
      (uint32) gs_encode_associated_radio_capability(GSM900_rf_power_class)
    );
  }
  /* MultibandSupport Field = 100 */
  else if (DCS1800_supported && (EGSM_supported == FALSE) && (PGSM_supported == FALSE))
  {
    /* spare bit(4) */
    gs_bit_stream_insert_spare(&bit_stream,4);
    
    /* Associated Radio Capability 1 (capability of DCS 1800) */
    gs_bit_stream_insert_field(
      &bit_stream,
      4,
      (uint32) gs_encode_associated_radio_capability(DCS1800_rf_power_class)
    );
  }
  /* MultibandSupport Field = 010 or 001 */
  else if ((DCS1800_supported == FALSE) && (EGSM_supported || PGSM_supported))
  {
    /* spare bit(4) */
    gs_bit_stream_insert_spare(&bit_stream,4);
    
    /* Associated Radio Capability 1 (capability of GSM 900) */
    gs_bit_stream_insert_field(
      &bit_stream,
      4,
      (uint32) gs_encode_associated_radio_capability(GSM900_rf_power_class)
    );
  }
  else /* MultibandSupport field something else */
  {
    ERR("Invalid combination of supported bands:%d,%d,%d",(int) DCS1800_supported,(int) EGSM_supported, (int) PGSM_supported);
  }

  /* RGSM not currently supported (0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);

  /* HSCSD MultiSlot Capability for CS Domain is currently not supported (0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);

  /* UCS2 treatment (coded as 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);

  /* extended measurement capability (not supported 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);

  /* MS measurement capability (not included 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);

#if defined(FEATURE_MMGPS) || defined(FEATURE_CGPS_UMTS_CP_GSM)
  {
    #if defined(FEATURE_MMGPS)
    network_assisted_gps_support_enum_type    pos_support;
    #else
    gsclassmark_cgps_pos_support_enum_type    pos_support;
    #endif
    boolean   standalone_supported;

    /* MS positioning method capability supported */
    gs_bit_stream_insert_field(&bit_stream,1,1);

    /* MS Assisted EOTD not supported */
    gs_bit_stream_insert_field(&bit_stream,1,0);

    /* MS Based EOTD not supported */
    gs_bit_stream_insert_field(&bit_stream,1,0);

    #if defined(FEATURE_MMGPS)
      /* Retrieve Capability information from LSM task */
      lsm_gsm_ue_pos_capability_get( &lsm_pos_capability );
      pos_support = lsm_pos_capability.network_assisted_gps_support;
      standalone_supported = lsm_pos_capability.standalone_location_method_supported;
    #else
      cgps_classmark_local_cb_fp = cgps_classmark_cb_fp;
      if( cgps_classmark_local_cb_fp )
      {
        cgps_classmark_local_cb_fp( &sm_pos_capability );
        pos_support = sm_pos_capability.cgps_pos_support;
        standalone_supported = sm_pos_capability.standalone_location_method_supported;
      }
      else
      {
        pos_support = GSCLASSMARK_POS_NONE;
        standalone_supported = FALSE;
      }
    #endif

    switch( pos_support )
    {
      case GSCLASSMARK_POS_NETWORK_BASED:
        /* MS Assisted GPS supported */
        gs_bit_stream_insert_field(&bit_stream,1,1);

        /* MS Based GPS NOT supported */
        gs_bit_stream_insert_field(&bit_stream,1,0);

        CM3_supported = TRUE;
        break;

      case GSCLASSMARK_POS_UE_BASED:
        /* MS Assisted GPS NOT supported */
        gs_bit_stream_insert_field(&bit_stream,1,0);

        /* MS Based GPS supported */
        gs_bit_stream_insert_field(&bit_stream,1,1);

        CM3_supported = TRUE;
        break;

      case GSCLASSMARK_POS_BOTH:
        /* MS Assisted GPS supported */
        gs_bit_stream_insert_field(&bit_stream,1,1);

        /* MS Based GPS supported */
        gs_bit_stream_insert_field(&bit_stream,1,1);

        CM3_supported = TRUE;
        break;

      case GSCLASSMARK_POS_NONE:
      default:
        /* MS Assisted GPS NOT supported */
        gs_bit_stream_insert_field(&bit_stream,1,0);

        /* MS Based GPS NOT supported */
        gs_bit_stream_insert_field(&bit_stream,1,0);
        break;
    }

    if ( standalone_supported )
    {
      /* MS Conventional GPS supported */
      gs_bit_stream_insert_field(&bit_stream,1,1);
      CM3_supported = TRUE;
    }
    else
    {
      /* MS Conventional GPS NOT supported */
      gs_bit_stream_insert_field(&bit_stream,1,0);
    }
  }
#else
  /* MS positioning method capability (not included 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);
#endif

#ifdef FEATURE_ECSD_NOT_SUPPORTED
  /* <ECSD Multi Slot Capability> 
  ** EDGE is replaced by ECSD (EDGE for CS Domain) in Rel5 Specs.
  ** For the EDGE specific parameters defined in the MS CM3 IE that are 
  ** applicable only to CS connections, the term ECSD is used instead of EDGE.
  ** Currently our UE does not support ECSD.
  */
  gs_bit_stream_insert_field(&bit_stream,1,0);
#endif

#ifdef FEATURE_GSM_EGPRS
   /* 8-PSK Struct. The MS shall include the 8-PSK struct 
   ** if it supports ECSD or DTM EGPRS or both.
   */
  if (gmm_edge_feature_support)
  {
      CM3_supported = TRUE;

#ifndef FEATURE_ECSD_NOT_SUPPORTED
      /* EDGE Multislot Class included (1) */
      gs_bit_stream_insert_field(&bit_stream,1,1);
      
      /* EDGE MS shall support EDGE Multislot Class '1' for now */
      gs_bit_stream_insert_field(&bit_stream,5,gmm_edge_multislot_class);
#endif

      if(gmm_nv_dtm_feature_support == TRUE)
      {
        /* 8-PSK Struct included (1) */
        gs_bit_stream_insert_field(&bit_stream,1,1);
        
        if (gmm_edge_8psk_power_class !=0)
        {
            /* 8PSK modulation in uplink also supported, so set
            ** the "Modulation Capability" bit to value '1'
            */
            gs_bit_stream_insert_field(&bit_stream,1,1);
  
            /* 8-PSK RF Power Capability 1 & 8-PSK RF Power Capability 2 need
            ** to be encoded accordingly
            ** If GSM400 (or) GSM850 (or) GSM900 are supported indicate EDGE
            ** RF Power Capability 1 - this shall be based on power class - 
            ** as of now the power class is '2'. Similarly 8-PSK RF Power
            ** Capability 2 shall be set to '2' if GSM 1800 (or) GSM 1900 is
            ** supported by the phone.
            */
            if ( (rr_is_supported_band(BAND_GSM_850) == TRUE) ||
                 (rr_is_supported_band(BAND_PGSM) == TRUE) )
            {
                /* 8-PSK RF Power Capability 1 included (1) */
                gs_bit_stream_insert_field(&bit_stream,1,1);
        
                /* GSM 850 or GSM 900 supported, set the power class */
                gs_bit_stream_insert_field(&bit_stream,2,gmm_edge_8psk_power_capability);
            }
            else
            {
                /* 8-PSK RF Power Capability 1 NOT included (0) */
                gs_bit_stream_insert_field(&bit_stream,1,0);
            }
  
            if ( (rr_is_supported_band(BAND_DCS) == TRUE) ||
                 (rr_is_supported_band(BAND_PCS) == TRUE) )
  
            {
                /* 8-PSK RF Power Capability 2 included (1) */
                gs_bit_stream_insert_field(&bit_stream,1,1);
        
                /* GSM 1800 (or) GSM 1900 - one of them supported */
                gs_bit_stream_insert_field(&bit_stream,2,gmm_edge_8psk_power_capability);
            }
            else
            {
                /* 8-PSK RF Power Capability 2 NOT included (0) */
                gs_bit_stream_insert_field(&bit_stream,1,0);
            }
        }
        else
        {
            /* 8PSK modulation in uplink NOT supported */
            gs_bit_stream_insert_field(&bit_stream,1,0);
  
            /* 8-PSK RF Power Capability 1 not included */
            gs_bit_stream_insert_field(&bit_stream,1,0);
  
            /* 8-PSK RF Power Capability 2 not included */
            gs_bit_stream_insert_field(&bit_stream,1,0);
        }
      }
      else
      {
        /* 8-PSK struct (not included 0) */
        gs_bit_stream_insert_field(&bit_stream,1,0);
      }
  }
  else
  {
#ifndef FEATURE_ECSD_NOT_SUPPORTED
    /* EDGE multislot capability (not included 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);
#endif
    /* 8-PSK struct (not included 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);    
  }
#else
#ifndef FEATURE_ECSD_NOT_SUPPORTED
  /* EDGE multislot capability (not included 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);
#endif
  /* 8-PSK struct (not included 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);
#endif /* FEATURE_GSM_EGPRS */

  /* GSM400 bands supported (not included 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);

  /* 
  ** GSM 850 associated radio capability
  ** Should not be included for SINGLE band MS.
  */
#ifdef FEATURE_GSM_MULTIBAND
  if ((gsm_supported == FALSE) || (rr_get_supported_band_count() <= 1))
  {
    /* only a single (or no gsm bands) are supported */
    gs_bit_stream_insert_field(&bit_stream,1,0);
  }
  else if (rr_is_supported_band(BAND_GSM_850))
  {
    CM3_supported = TRUE;

    /* GSM 850 associated radio capability  */
    gs_bit_stream_insert_field(&bit_stream,1,1);
    
    gs_bit_stream_insert_field(
      &bit_stream,
      4,
      (uint32) gs_encode_associated_radio_capability(rr_get_band_rf_power_class(BAND_GSM_850)));
  }
  else
  {   
    gs_bit_stream_insert_field(&bit_stream,1,0);
  }
#else
  #error code not present
#endif

  /* PCS 1900 associated radio capability field encoding */
  {
    /***********************************************************/
    /* Semantic rule indicates that a single band MS shall not */
    /* indicate the band it supports in the PCS1900 associated */
    /* radio capability field                                  */
    /***********************************************************/
#ifdef FEATURE_GSM_MULTIBAND
    if ((gsm_supported == FALSE) || (rr_get_supported_band_count() <= 1))
    {
      /* only a single (or no gsm bands) are supported */
      gs_bit_stream_insert_field(&bit_stream,1,0);
    }
    /* otherwise, if PCS 1900 is a supported band */
    else if (PCS1900_supported)
    {
      CM3_supported = TRUE;
      /* include the PCS1900 associated radio capability field */
      gs_bit_stream_insert_field(&bit_stream,1,1);
      gs_bit_stream_insert_field(
        &bit_stream,
        4,
        (uint32) gs_encode_associated_radio_capability(rr_get_band_rf_power_class(BAND_PCS))
      );
    }
    else /* PCS1900 not supported */
    {
      /* don't include the field */
      gs_bit_stream_insert_field(&bit_stream,1,0);
    }
#else
    #error code not present
#endif /* FEATURE_GSM_MULTIBAND */
  } /* PCS 1900 associated radio capability */ 

#ifdef FEATURE_INTERRAT_HANDOVER_GTOW
  /* UMTS FDD Radio Access Technology Capability (1 if supported, 0 otherwise) */
  /* if GSM to WCMDA Handover is supported, indicate support only if WCDMA is supported */
  gs_bit_stream_insert_field(&bit_stream,1,(wcdma_supported) ? 1 : 0);

  if (wcdma_supported)
  {
    CM3_supported = TRUE;
  }
#else
  /* UMTS FDD Radio Access Technology Capability (1 if supported, 0 otherwise) */
  /* if GSM to WCDMA handover is not supported, code as 0 if GSM is currently the active RAT */
  gs_bit_stream_insert_field(&bit_stream,1,(wcdma_supported && wcdma_in_use) ? 1 : 0);
  if (wcdma_supported && wcdma_in_use)
  {
    CM3_supported = TRUE;
  }
#endif

  /* UMTS TDD Radio Access Technology Capability 3.84 Mpcs (not supported 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);

  /* CDMA2000 Radio Access Technology Capability (not supported 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);

#ifndef FEATURE_GSM_DTM
  /* DTM GPRS MultiSlot sub-class (not included 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);
#else /* DTM is supported - include DTM multislot capability */
  if (gmm_nv_dtm_feature_support)
  {
    CM3_supported = TRUE;
    /* DTM GPRS MultiSlot sub-class (included 1) */
    gs_bit_stream_insert_field(&bit_stream,1,1);
    /* DTM GPRS MultiSlot capability */
    gs_bit_stream_insert_field(&bit_stream,2,DTM_GPRS_multislot_subclass);
    /* Single slot DTM  (not supported 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);
#ifdef FEATURE_GSM_EGPRS
    if (gmm_edge_feature_support)
    {
      /* DTM EGPRS MultiSlot sub-class (included 1) */
      gs_bit_stream_insert_field(&bit_stream,1,1);
      /* DTM EGPRS MultiSlot capability  */
      gs_bit_stream_insert_field(&bit_stream,2,DTM_EGPRS_multislot_subclass);
    }
    else
#endif /*FEATURE_GSM_EGPRS*/
    {
      /* DTM EGPRS MultiSlot sub-class (not supported 0) */
      gs_bit_stream_insert_field(&bit_stream,1,0);
    }
  }
  else
  {
    /* DTM GPRS MultiSlot sub-class (not included 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);
  }
#endif

  /* single band support field encoding */
  {
    /*******************************************************************************/
    /* This field shall be sent if the mobile station supports UMTS(WCDMA) and one */
    /* and only one GSM band with the exception of R-GSM; the field shall not be   */
    /* sent otherwise                                                              */
    /*******************************************************************************/
    if (wcdma_supported)
    {
#ifdef FEATURE_GSM_MULTIBAND
      /* only send if a single band is supported */
      if (gsm_supported && (rr_get_supported_band_count() == 1))
      {
        CM3_supported = TRUE;
        
        /* include the field */
        gs_bit_stream_insert_field(&bit_stream,1,1);

        /* encode the supported band value appropriately */
        if (rr_is_supported_band(BAND_EGSM))
        {
          /* encoded as 0000 */
          gs_bit_stream_insert_field(&bit_stream,4,0x0);
        }
        else if (rr_is_supported_band(BAND_PGSM))
        {
          /* encoded as 0001 */
          gs_bit_stream_insert_field(&bit_stream,4,0x1);
        }
        else if (rr_is_supported_band(BAND_DCS))
        {
          /* encoded as 0010 */
          gs_bit_stream_insert_field(&bit_stream,4,0x2);
        }
        else if (rr_is_supported_band(BAND_GSM_850))
        {
          /* encoded as 0101 */
          gs_bit_stream_insert_field(&bit_stream,4,0x5);
        }
        else if (rr_is_supported_band(BAND_PCS))
        {
          /* encoded as 0110 */
          gs_bit_stream_insert_field(&bit_stream,4,0x6);
        }
        else
        {
          ERR("Invalid supported band value",0,0,0);
        }
      }
      else /* other than 1 GSM band supported (or GSM not supported) */
      {
        /* don't include the field */
        gs_bit_stream_insert_field(&bit_stream,1,0);
      }
#else /* multiband not supported */
      #error code not present
#endif /* FEATURE_GSM_MULTIBAND */
    }
    else /* WCDMA not supported */
    {
      /* don't include the field */
      gs_bit_stream_insert_field(&bit_stream,1,0);
    }
  } /* single band support field */

  /* Release 4 features start here */

#ifdef FEATURE_REL_4_SUPPORT

  /* GSM 700 associated radio capability (not included 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);

  /* UMTS 1.28Mcps TDD Radio Access Technology Capability */
  
NAS_ENTER_CRIT_SECT(mm_rat_prioritylist_crit_sec);
    if (((mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_TDS_RADIO_ACCESS)) && 
          !(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_UMTS_RADIO_ACCESS))) 
#ifdef FEATURE_SGLTE
           && !MM_IS_IN_SGLTE_STATE_4
#endif
          )
    {
        gs_bit_stream_insert_field(&bit_stream,1,1);

    }
    else
    {
        gs_bit_stream_insert_field(&bit_stream,1,0);
    }
NAS_EXIT_CRIT_SECT(mm_rat_prioritylist_crit_sec);

  /* GERAN Feature Package 1 */
#ifdef FEATURE_GSM_GERAN_FEATURE_PACK_1
  gs_bit_stream_insert_field(&bit_stream,1,gmm_edge_nw_ass_cell_change); 
  if (gmm_edge_nw_ass_cell_change)
  {
    CM3_supported = TRUE;
  }  
#else
  gs_bit_stream_insert_field(&bit_stream,1,0); /* (not supported 0) */
#endif

  /* Extended DTM GPRS Multi Slot Class (not included 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);

#endif /* FEATURE_REL_4_SUPPORT */

#if defined(FEATURE_REL5) || \
    (!defined(FEATURE_WCDMA) && defined(FEATURE_REL_6_SUPPORT))
   #ifndef FEATURE_REL_4_SUPPORT
     #error FEATURE_REL5 requires FEATURE_REL_4_SUPPORT
   #endif

    /* High Multislot Capability */
    gs_bit_stream_insert_field(&bit_stream,1,0);

    /* GERAN Iu Mode Capabilities */
    gs_bit_stream_insert_field(&bit_stream,1,0);

    /* GERAN Feture Package 2 */
#ifdef FEATURE_GSM_GERAN_FEATURE_PACK_2
    CM3_supported = TRUE;
    gs_bit_stream_insert_field(&bit_stream,1,1); /* (supported 1) */
#else
    gs_bit_stream_insert_field(&bit_stream,1,0); /* (not supported 0) */
#endif

    /* GMSK Multislot Power Profile */
    gs_bit_stream_insert_field(&bit_stream,2,gmm_nv_multislot_gmsk_power_profile);

    /* 8-PSK Multislot Power Profile */
    gs_bit_stream_insert_field(&bit_stream,2,gmm_nv_multislot_8psk_power_profile);
#endif /* FEATURE_REL5 */

#ifdef FEATURE_REL_6_SUPPORT  
    #if !defined (FEATURE_REL_4_SUPPORT) || \
        (defined(FEATURE_WCDMA) && !defined(FEATURE_REL5))
      #error FEATURE_REL_6_SUPPORT requires FEATURE_REL_4_SUPPORT and FEATURE_REL5
    #endif

    /* T-GSM 400 Bands Supported (not included 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);

    /* T-GSM 900 Associated Radio Capability (not included 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);

    /* SAIC/DARP Support */
#if (defined(FEATURE_GSM_EGPRS_SAIC) && defined(FEATURE_GSM_MDSP_AEQ))
    gs_bit_stream_insert_field(&bit_stream,2,gmm_nv_saic_feature_support);
    if (gmm_nv_saic_feature_support)
    {
      CM3_supported = TRUE;
    }
#else
    gs_bit_stream_insert_field(&bit_stream,2,0); /* (not supported 0) */
#endif /* FEATURE_GSM_EGPRS_SAIC */

#ifdef FEATURE_GSM_EDTM
    /* DTM Enhancements Capability (controlled by NV item 4209) */
    gs_bit_stream_insert_field(&bit_stream,1,gmm_nv_edtm_feature_support);
    if (gmm_nv_edtm_feature_support)
    {
      CM3_supported = TRUE;
    }
#else
    /* DTM Enhancements Capability (not supported 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);
#endif /* FEATURE_GSM_EDTM */

    /* DTM GPRS High Multi Slot Class (not included 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);

#if defined(FEATURE_GSM_R_SACCH) || defined(FEATURE_GSM_R_FACCH)
    if ((gmm_nv_r_acch_feature_support != 0) && (gmm_nv_r_acch_feature_support != 2))
    {
      /* R_SACCH  and R_FACCH are supported */
      gs_bit_stream_insert_field(&bit_stream,1,1);
      CM3_supported = TRUE;
    }
    else
#endif /* FEATURE_GSM_R_SACCH */
    /* R_SACCH not supported and R_FACCH supported */
    gs_bit_stream_insert_field(&bit_stream,1,0);
#else

    if( include_rel6_IE = TRUE)
    {
      /* T-GSM 400 Bands Supported (not included 0) */
      gs_bit_stream_insert_field(&bit_stream,1,0);

      /* T-GSM 900 Associated Radio Capability (not included 0) */
      gs_bit_stream_insert_field(&bit_stream,1,0);

      /* SAIC/DARP Support */
      gs_bit_stream_insert_field(&bit_stream,2,0); /* (not supported 0) */

      /* DTM Enhancements Capability (not supported 0) */
      gs_bit_stream_insert_field(&bit_stream,1,0);

      /* DTM GPRS High Multi Slot Class (not included 0) */
      gs_bit_stream_insert_field(&bit_stream,1,0);

#if defined(FEATURE_GSM_R_SACCH) || defined(FEATURE_GSM_R_FACCH)
      if ((gmm_nv_r_acch_feature_support != 0) && (gmm_nv_r_acch_feature_support != 2))
      {
        /* R_SACCH  and R_FACCH are supported */
        gs_bit_stream_insert_field(&bit_stream,1,1);
        CM3_supported = TRUE;
      }
      else
#endif /* FEATURE_GSM_R_SACCH */
      /* R_SACCH not supported and R_FACCH supported */
      gs_bit_stream_insert_field(&bit_stream,1,0);
    }
#endif /* FEATURE_REL_6_SUPPORT */

  if(include_rel7_IE == TRUE)
  {
    /* GSM 710 Associated Radio Capability (not included 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);

    /* T-GSM 810 Associated Radio Capability (not included 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);

#ifdef FEATURE_GSM_DTM
    if (gmm_nv_dtm_feature_support)
    {
      /* Ciphering Mode Setting Capability (supported 1) */
      gs_bit_stream_insert_field(&bit_stream,1,1);
    }
    else
    {
      /* Ciphering Mode Setting Capability (supported 0) */
      gs_bit_stream_insert_field(&bit_stream,1,0);
    }
#else
    /* Ciphering Mode Setting Capability (supported 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);
#endif
    CM3_supported = TRUE;

    /* Additional Positioning Capabilities (not supported 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);
  }
     
  if(include_rel8_IE == TRUE)
  {
#if defined(FEATURE_LTE)
    NAS_ENTER_CRIT_SECT(mm_rat_prioritylist_crit_sec);
    
    mm_checkif_rat_pri_list_has_rat_local = mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_LTE_RADIO_ACCESS);
#ifdef FEATURE_SGLTE
   if(MM_IS_IN_SGLTE_STATE_4)
   {
     mm_checkif_rat_pri_list_has_rat_local = FALSE;
   }
#endif

    /*E-UTRA FDD support is true only if Rat priority list contains the lte rat */
    if ( mm_checkif_rat_pri_list_has_rat_local && 
         lte_rrc_lte_fdd_is_supported(mm_get_lte_band_capacity(&mm_rat_pri_list_info)))
    {
      /* E-UTRA FDD support - Supported*/
      gs_bit_stream_insert_field(&bit_stream,1,1);
      CM3_supported = TRUE;
      eutra_FDD_TDD_support = TRUE;
    }
    else
    {
      /* E-UTRA FDD support - Not Supported*/
      gs_bit_stream_insert_field(&bit_stream,1,0);
    }
    /*E-UTRA TDD support is true only if Rat priority list contains the lte rat */
    if (mm_checkif_rat_pri_list_has_rat_local  &&
        lte_rrc_lte_tdd_is_supported(mm_get_lte_band_capacity(&mm_rat_pri_list_info)))
    {
      /* E-UTRA TDD support - Supported*/
      gs_bit_stream_insert_field(&bit_stream,1,1);
      eutra_FDD_TDD_support = TRUE;
    }
    else
    {
      /* E-UTRA TDD support - Not Supported*/
      gs_bit_stream_insert_field(&bit_stream,1,0);
    }    

    NAS_EXIT_CRIT_SECT(mm_rat_prioritylist_crit_sec);

    if(eutra_FDD_TDD_support == TRUE)
    {
      /* E-UTRA Measurement and Reporting support - Not supported */
      gs_bit_stream_insert_field(&bit_stream,1,gmm_nv_eutra_measurement_reporting_support);
    }
    else
    {
      /*If both "E-UTRA FDD support" and "E-UTRA TDD support" bits are set to '0', 
            this field shall be set to '0'*/
      gs_bit_stream_insert_field(&bit_stream,1,0);
    }


    if((mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_LTE_RADIO_ACCESS) ||
        mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_UMTS_RADIO_ACCESS) || 
        mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_TDS_RADIO_ACCESS)) &&
        (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_GSM_RADIO_ACCESS)))
     {
       /* Priority-based reselection - Supported*/
       gs_bit_stream_insert_field(&bit_stream,1,1);
     }
    else
    {
       /* Priority-based reselection - NOT Supported*/
       gs_bit_stream_insert_field(&bit_stream,1,0);
    }
#else

    /* E-UTRA FDD support - Not Supported*/
    gs_bit_stream_insert_field(&bit_stream,1,0);

    /* E-UTRA TDD support - Not Supported*/
    gs_bit_stream_insert_field(&bit_stream,1,0);

    /* E-UTRA Measurement and Reporting support - Not supported */
    gs_bit_stream_insert_field(&bit_stream,1,0);
  
    /* Priority-based reselection - Not Supported*/
    gs_bit_stream_insert_field(&bit_stream,1,0);
#endif
}
  /* if feature Vamos is on then start coding rel 9 ie*/
#ifdef FEATURE_VAMOS
  /*Reporting of UTRAN CSG cells not supported*/
  gs_bit_stream_insert_field(&bit_stream,1,0);
  
  if(mm_nv_vamos_feature_support == VAMOS_OFF)
  {
    gs_bit_stream_insert_field(&bit_stream,2,0);
  }
  else if(mm_nv_vamos_feature_support == VAMOS_1)
  {
    gs_bit_stream_insert_field(&bit_stream,2,1);
  }
  else
  {
    gs_bit_stream_insert_field(&bit_stream,2,2);
  }
#else
  /*Reporting of UTRAN CSG cells not supported*/
  gs_bit_stream_insert_field(&bit_stream,1,0);

  gs_bit_stream_insert_field(&bit_stream,2,0);
#endif

  /* Start coding rel 10 changes tighter capability*/
  gs_bit_stream_insert_field(&bit_stream,2,mm_nv_tighter_capability);

  /* spare bit should be at the end of classmark3*/
  gs_bit_stream_insert_spare(&bit_stream,1);

  /* set the length of the classmark 3 IE */
  classmark_3_ptr->length = (byte) gs_bit_stream_byte_count(&bit_stream);

  /* copy the data into the IE padded with 0s at end */
  for (index = 0; index < MIN(classmark_3_ptr->length,MAX_CLASSMARK3_DATA_SIZE); index++)
  {
    classmark_3_ptr->data[index] = data[index];
  }

  return CM3_supported;

#endif

} /* gs_read_coded_classmark_3 */


/**
 * gs_read_coded_classmark_3_per_nas_stacks
 */
boolean gs_read_coded_classmark_3_per_nas_stacks(
  classmark_3_ie_T     *classmark_3_ptr,
  classmark_params_T   *params_ptr,
  sys_modem_as_id_e_type stack_id_nas
)
{
#if defined FEATURE_DUAL_SIM ||defined FEATURE_SGLTE
  byte index = 0;
  hw_classmark_T hw_classmark = {0};

  /*
  ** This field to indicate if some of the capability of classmark - 3 supported by UE.
  */
  boolean CM3_supported = FALSE;
#ifndef FEATURE_REL_6_SUPPORT
  boolean include_rel6_IE = FALSE;
#endif
  boolean include_rel7_IE = FALSE;
  boolean include_rel8_IE = FALSE;
  byte data[MAX_CLASSMARK3_DATA_SIZE]={0};

  boolean A5_4_supported;
  boolean A5_5_supported;
  boolean A5_6_supported;
  boolean A5_7_supported;
  ghdi_status_T status;
  gs_bit_stream_T bit_stream;
#if defined(FEATURE_MMGPS)
  lsm_ue_pos_capability_struct_type lsm_pos_capability;
#elif defined(FEATURE_CGPS_UMTS_CP_GSM)
  gsclassmark_cgps_pos_capability_struct_type   sm_pos_capability;
#endif

  boolean PCS1900_supported; 
  boolean DCS1800_supported;
  boolean PGSM_supported;
  boolean EGSM_supported;
#ifdef FEATURE_LTE
  boolean                   mm_checkif_rat_pri_list_has_rat_local;
  boolean eutra_FDD_TDD_support = FALSE;
#endif
  byte DCS1800_rf_power_class = 0;
  byte GSM900_rf_power_class  = 0;

#ifdef FEATURE_GSM_DTM
  byte DTM_GPRS_multislot_subclass;
  byte DTM_EGPRS_multislot_subclass;
#endif
#if defined(FEATURE_CGPS_UMTS_CP_GSM)
  cgps_classmark_cb_type  cgps_classmark_local_cb_fp;
#endif

#if defined(FEATURE_GSM_R_SACCH) || defined(FEATURE_GSM_R_FACCH)
#ifndef FEATURE_REL_6_SUPPORT
 include_rel6_IE = TRUE;
#endif
#endif

#if defined(FEATURE_LTE)
  include_rel7_IE = TRUE;
#else
  if(mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL7)
  {
    include_rel7_IE = TRUE;
  }
#endif
#if defined(FEATURE_LTE)
  include_rel8_IE = TRUE;
#endif
#ifdef FEATURE_VAMOS
  include_rel7_IE = TRUE;
  include_rel8_IE = TRUE;
#endif

#ifdef FEATURE_SGLTE
  if(MM_IS_IN_SGLTE_STATE_4)
  {
    params_ptr->wcdma_supported = FALSE;
  }
#endif
  if(IS_NOT_VALID_STACK_ID(stack_id_nas) ||
     IS_NOT_VALID_SUB_ID(params_ptr->as_id))
  {
    ERR("Invalid STACK ID:%d or SUB ID:%d", stack_id_nas+1,params_ptr->as_id+1,0);
    return gs_read_coded_classmark_3
           (
           classmark_3_ptr,
           params_ptr->gsm_supported,
           params_ptr->wcdma_supported,
           params_ptr->wcdma_in_use,
           params_ptr->MSCR99
           );
  }
  
#ifdef FEATURE_GSM_DTM
  DTM_GPRS_multislot_subclass  = gmm_nv_dtm_multislot_subclass_sim[params_ptr->as_id];
  DTM_EGPRS_multislot_subclass = gmm_nv_dtm_multislot_subclass_sim[params_ptr->as_id];
#endif

#ifdef FEATURE_GSM_MULTIBAND
  mb_initialise();
#endif
  
  /* initialize the bit stream */
  gs_bit_stream_initialize(&bit_stream,data,sizeof(data));

  /*******************************************************************************/
  /* HW classmark contains information related to A5 support and band support in */
  /* single band MSs                                                             */
  /*******************************************************************************/
  status = ghdi_read_hw_classmark(&hw_classmark);

  if (status != GHDI_SUCCESS)
  {
    ERR("ERROR: Unable to read classmark: status=%d",status,0,0); 
  }

  /* fill the bit stream with 0s */
  gs_bit_stream_fill(&bit_stream,MAX_CLASSMARK3_DATA_SIZE,0);

  /* start coding the classmark 3 value part using the bit stream */

  /* spare bit */
  gs_bit_stream_insert_spare(&bit_stream,1);

#ifdef FEATURE_GSM_MULTIBAND
#ifdef FEATURE_DUAL_SIM
  PCS1900_supported = rr_ds_is_supported_band(BAND_PCS,params_ptr->as_id);
  DCS1800_supported = rr_ds_is_supported_band(BAND_DCS,params_ptr->as_id);
#else
  PCS1900_supported = rr_is_supported_band(BAND_PCS);
  DCS1800_supported = rr_is_supported_band(BAND_DCS);
#endif
  /* determine whether DCS or PCS are in use by the current NW */
  /* these bands are mutually exclusive, and cannot BOTH be listed in the classmark */
  /* If WCDMA is active (RRC calling this function) just use explicit band support  */
  if (params_ptr->wcdma_in_use == FALSE)
  {
    if (PCS1900_supported && DCS1800_supported)
    {
#ifdef FEATURE_DUAL_SIM
      if (rr_ds_get_band_indicator_for_nas(TRUE,params_ptr->as_id) == SYS_BAND_GSM_PCS_1900)
#else
      if (rr_get_band_indicator_for_nas(TRUE) == SYS_BAND_GSM_PCS_1900)
#endif
      {
        DCS1800_supported = FALSE;
      }
      else
      {
        PCS1900_supported = FALSE;
      }
    }
  }
  
  MSG_HIGH_DS( MM_SUB, "=MM= DCS support %1d, PCS support %1d", (int)DCS1800_supported,(int)PCS1900_supported, 0);
#else
  #error code not present
#endif /* FEATURE_GSM_MULTIBAND */

  /* Encode the MultibandSupport Classmark3IE Field (not same as FEATURE_GSM_MULTIBAND ifdef) */
  {
    /* figure out which bands are supported */
#ifdef FEATURE_GSM_MULTIBAND
    if (params_ptr->gsm_supported)
    {
#ifdef FEATURE_DUAL_SIM
      PGSM_supported    = rr_ds_is_supported_band(BAND_PGSM,params_ptr->as_id);
      EGSM_supported    = rr_ds_is_supported_band(BAND_EGSM,params_ptr->as_id);
#else
      PGSM_supported    = rr_is_supported_band(BAND_PGSM);
      EGSM_supported    = rr_is_supported_band(BAND_EGSM);
#endif
      /* get the power class values */
      if (DCS1800_supported)
      {
#ifdef FEATURE_DUAL_SIM
        DCS1800_rf_power_class = rr_ds_get_band_rf_power_class(BAND_DCS,params_ptr->as_id);
#else
        DCS1800_rf_power_class = rr_get_band_rf_power_class(BAND_DCS);
#endif
      }

      if (PGSM_supported)
      {
#ifdef FEATURE_DUAL_SIM
        GSM900_rf_power_class = rr_ds_get_band_rf_power_class(BAND_PGSM,params_ptr->as_id);
#else
        GSM900_rf_power_class = rr_get_band_rf_power_class(BAND_PGSM);
#endif
      }

      if (EGSM_supported)
      {
#ifdef FEATURE_DUAL_SIM
        GSM900_rf_power_class = rr_ds_get_band_rf_power_class(BAND_EGSM,params_ptr->as_id);
#else
        GSM900_rf_power_class = rr_get_band_rf_power_class(BAND_EGSM);
#endif
      }

      /************************************************************/
      /* Semantic rule indicates that a single band MS shall not  */
      /* indicate the band it supports in the multiband supported */
      /* field                                                    */
      /************************************************************/
#ifdef FEATURE_DUAL_SIM
      if (rr_ds_get_supported_band_count(params_ptr->as_id) <= 1)
#else
      if (rr_get_supported_band_count() <= 1)
#endif
      {
        /* all band supported set to false */
        DCS1800_supported = FALSE;
        PGSM_supported    = FALSE;
        EGSM_supported    = FALSE;
      }
    }
    else /* GSM is not supported */
    {
      /* code all as false */
      PCS1900_supported = FALSE;
      DCS1800_supported = FALSE;
      PGSM_supported    = FALSE;
      EGSM_supported    = FALSE;
    }
#else /* multiband not supported */
    #error code not present
#endif /* FEATURE_GSM_MULTIBAND */

    /* PGSM & EGSM are mutually exclusive */
    if (PGSM_supported && EGSM_supported)
    {
       /* As per 3GPP TS 24.008 Sec: 10.5.1.7,                           *
        *   "indication of support of P-GSM band or, E-GSM or R-GSM band *
        *   is mutually exclusive" and both should not be set in         *
        *   MS Classmark3, at the same time.  Also since E-GSM is a      *
        *   super-set of P-GSM as E-GSM includes all ARFCNs in P-GSM and *
        *   has additional ARFCNs, we'll set PGSM_supported to FALSE.    */
       MSG_MED_0 ("Mutually exclusive PGSM_supported and EGSM_supported are both enabled!!");
       PGSM_supported    = FALSE;
    }

    /* DCS 1800 support */
    gs_bit_stream_insert_field(&bit_stream,1,DCS1800_supported);

    /* EGSM or RGSM support */
    gs_bit_stream_insert_field(&bit_stream,1,EGSM_supported);

    /* PGSM support */
    gs_bit_stream_insert_field(&bit_stream,1,PGSM_supported);

    if (DCS1800_supported || EGSM_supported || PGSM_supported)
    {
      CM3_supported = TRUE;
    }
    
  } /* Encode MultibandSupport Classmark3IE Field */

  /* next depends on multibands that are supported */
  A5_4_supported  =        hw_classmark.A5_4_supported;
  A5_5_supported  =        hw_classmark.A5_5_supported;
  A5_6_supported  =        hw_classmark.A5_6_supported;
  A5_7_supported  =        hw_classmark.A5_7_supported;
  /* A5 bits are the same irrespective of multiband supported flavor */
  gs_bit_stream_insert_field(&bit_stream,1,A5_7_supported);
  gs_bit_stream_insert_field(&bit_stream,1,A5_6_supported);
  gs_bit_stream_insert_field(&bit_stream,1,A5_5_supported);
  gs_bit_stream_insert_field(&bit_stream,1,A5_4_supported);

  if (A5_4_supported || A5_5_supported || A5_6_supported || A5_7_supported)
  {
    CM3_supported = TRUE;
  }
  
  /* MultibandSupport Field = 000 */
  if (DCS1800_supported == FALSE && EGSM_supported == FALSE && PGSM_supported == FALSE)
  {
    /* nothing is inserted */
  }
  /* MultibandSupport Field = 101 or 110 */
  else if ((DCS1800_supported && (EGSM_supported == FALSE) && PGSM_supported) ||
           (DCS1800_supported && EGSM_supported && (PGSM_supported == FALSE))
          )
  {
    /* Associated Radio Capability 2 (capability of DCS 1800) */
    gs_bit_stream_insert_field(
      &bit_stream,
      4,
      gs_encode_associated_radio_capability(DCS1800_rf_power_class)
    );

    /* Associated Radio Capability 1 (capability of GSM_900 band (either PGSM or EGSM)) */
    gs_bit_stream_insert_field(
      &bit_stream,
      4,
      gs_encode_associated_radio_capability(GSM900_rf_power_class)
    );
  }
  /* MultibandSupport Field = 100 */
  else if (DCS1800_supported && (EGSM_supported == FALSE) && (PGSM_supported == FALSE))
  {
    /* spare bit(4) */
    gs_bit_stream_insert_spare(&bit_stream,4);
    
    /* Associated Radio Capability 1 (capability of DCS 1800) */
    gs_bit_stream_insert_field(
      &bit_stream,
      4,
      gs_encode_associated_radio_capability(DCS1800_rf_power_class)
    );
  }
  /* MultibandSupport Field = 010 or 001 */
  else if ((DCS1800_supported == FALSE) && (EGSM_supported || PGSM_supported))
  {
    /* spare bit(4) */
    gs_bit_stream_insert_spare(&bit_stream,4);
    
    /* Associated Radio Capability 1 (capability of GSM 900) */
    gs_bit_stream_insert_field(
      &bit_stream,
      4,
      gs_encode_associated_radio_capability(GSM900_rf_power_class)
    );
  }
  else /* MultibandSupport field something else */
  {
    ERR("Invalid combination of supported bands:%d,%d,%d",(int) DCS1800_supported,(int) EGSM_supported, (int) PGSM_supported);
  }

  /* RGSM not currently supported (0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);

  /* HSCSD MultiSlot Capability for CS Domain is currently not supported (0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);

  /* UCS2 treatment (coded as 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);

  /* extended measurement capability (not supported 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);

  /* MS measurement capability (not included 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);

#if defined(FEATURE_MMGPS) || defined(FEATURE_CGPS_UMTS_CP_GSM)
  {
    #if defined(FEATURE_MMGPS)
    network_assisted_gps_support_enum_type    pos_support;
    #else
    gsclassmark_cgps_pos_support_enum_type    pos_support;
    #endif
    boolean   standalone_supported;

    /* MS positioning method capability supported */
    gs_bit_stream_insert_field(&bit_stream,1,1);

    /* MS Assisted EOTD not supported */
    gs_bit_stream_insert_field(&bit_stream,1,0);

    /* MS Based EOTD not supported */
    gs_bit_stream_insert_field(&bit_stream,1,0);

    #if defined(FEATURE_MMGPS)
      /* Retrieve Capability information from LSM task */
      lsm_gsm_ue_pos_capability_get( &lsm_pos_capability );
      pos_support = lsm_pos_capability.network_assisted_gps_support;
      standalone_supported = lsm_pos_capability.standalone_location_method_supported;
    #else
      cgps_classmark_local_cb_fp = cgps_classmark_cb_fp;
      if( cgps_classmark_local_cb_fp )
      {
        cgps_classmark_local_cb_fp( &sm_pos_capability );
        pos_support = sm_pos_capability.cgps_pos_support;
        standalone_supported = sm_pos_capability.standalone_location_method_supported;
      }
      else
      {
        pos_support = GSCLASSMARK_POS_NONE;
        standalone_supported = FALSE;
      }
    #endif

    switch( pos_support )
    {
      case GSCLASSMARK_POS_NETWORK_BASED:
        /* MS Assisted GPS supported */
        gs_bit_stream_insert_field(&bit_stream,1,1);

        /* MS Based GPS NOT supported */
        gs_bit_stream_insert_field(&bit_stream,1,0);

        CM3_supported = TRUE;
        break;

      case GSCLASSMARK_POS_UE_BASED:
        /* MS Assisted GPS NOT supported */
        gs_bit_stream_insert_field(&bit_stream,1,0);

        /* MS Based GPS supported */
        gs_bit_stream_insert_field(&bit_stream,1,1);

        CM3_supported = TRUE;
        break;

      case GSCLASSMARK_POS_BOTH:
        /* MS Assisted GPS supported */
        gs_bit_stream_insert_field(&bit_stream,1,1);

        /* MS Based GPS supported */
        gs_bit_stream_insert_field(&bit_stream,1,1);

        CM3_supported = TRUE;
        break;

      case GSCLASSMARK_POS_NONE:
      default:
        /* MS Assisted GPS NOT supported */
        gs_bit_stream_insert_field(&bit_stream,1,0);

        /* MS Based GPS NOT supported */
        gs_bit_stream_insert_field(&bit_stream,1,0);
        break;
    }

    if ( standalone_supported )
    {
      /* MS Conventional GPS supported */
      gs_bit_stream_insert_field(&bit_stream,1,1);
      CM3_supported = TRUE;
    }
    else
    {
      /* MS Conventional GPS NOT supported */
      gs_bit_stream_insert_field(&bit_stream,1,0);
    }
  }
#else
  /* MS positioning method capability (not included 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);
#endif

#ifdef FEATURE_ECSD_NOT_SUPPORTED
  /* <ECSD Multi Slot Capability> 
  ** EDGE is replaced by ECSD (EDGE for CS Domain) in Rel5 Specs.
  ** For the EDGE specific parameters defined in the MS CM3 IE that are 
  ** applicable only to CS connections, the term ECSD is used instead of EDGE.
  ** Currently our UE does not support ECSD.
  */
  gs_bit_stream_insert_field(&bit_stream,1,0);
#endif

#ifdef FEATURE_GSM_EGPRS
   /* 8-PSK Struct. The MS shall include the 8-PSK struct 
   ** if it supports ECSD or DTM EGPRS or both.
   */
  if (gmm_edge_feature_support_sim[params_ptr->as_id])
  {
      CM3_supported = TRUE;

#ifndef FEATURE_ECSD_NOT_SUPPORTED
      /* EDGE Multislot Class included (1) */
      gs_bit_stream_insert_field(&bit_stream,1,1);
      
      /* EDGE MS shall support EDGE Multislot Class '1' for now */
      gs_bit_stream_insert_field(&bit_stream,5,gmm_edge_multislot_class_sim[params_ptr->as_id]);
#endif

      if (gmm_nv_dtm_feature_support_sim[params_ptr->as_id] == TRUE)
      {
        /* 8-PSK Struct included (1) */
        gs_bit_stream_insert_field(&bit_stream,1,1);
        
        if (gmm_edge_8psk_power_class_sim[params_ptr->as_id] !=0)
        {
            /* 8PSK modulation in uplink also supported, so set
            ** the "Modulation Capability" bit to value '1'
            */
            gs_bit_stream_insert_field(&bit_stream,1,1);
  
            /* 8-PSK RF Power Capability 1 & 8-PSK RF Power Capability 2 need
            ** to be encoded accordingly
            ** If GSM400 (or) GSM850 (or) GSM900 are supported indicate EDGE
            ** RF Power Capability 1 - this shall be based on power class - 
            ** as of now the power class is '2'. Similarly 8-PSK RF Power
            ** Capability 2 shall be set to '2' if GSM 1800 (or) GSM 1900 is
            ** supported by the phone.
            */
  #ifdef FEATURE_DUAL_SIM
            if ( (rr_ds_is_supported_band(BAND_GSM_850,params_ptr->as_id) == TRUE) ||
                 (rr_ds_is_supported_band(BAND_PGSM,params_ptr->as_id) == TRUE) )
  #else
            if ( (rr_is_supported_band(BAND_GSM_850) == TRUE) ||
                 (rr_is_supported_band(BAND_PGSM) == TRUE) )
  #endif
            {
                /* 8-PSK RF Power Capability 1 included (1) */
                gs_bit_stream_insert_field(&bit_stream,1,1);
        
                /* GSM 850 or GSM 900 supported, set the power class */
                gs_bit_stream_insert_field(&bit_stream,2,gmm_edge_8psk_power_capability_sim[params_ptr->as_id]);
            }
            else
            {
                /* 8-PSK RF Power Capability 1 NOT included (0) */
                gs_bit_stream_insert_field(&bit_stream,1,0);
            }
  #ifdef FEATURE_DUAL_SIM
            if ( (rr_ds_is_supported_band(BAND_DCS,params_ptr->as_id) == TRUE) ||
                 (rr_ds_is_supported_band(BAND_PCS,params_ptr->as_id) == TRUE) )
  #else
            if ( (rr_is_supported_band(BAND_DCS) == TRUE) ||
                 (rr_is_supported_band(BAND_PCS) == TRUE) )
  #endif
            {
                /* 8-PSK RF Power Capability 2 included (1) */
                gs_bit_stream_insert_field(&bit_stream,1,1);
        
                /* GSM 1800 (or) GSM 1900 - one of them supported */
                gs_bit_stream_insert_field(&bit_stream,2,gmm_edge_8psk_power_capability_sim[params_ptr->as_id]);
            }
            else
            {
                /* 8-PSK RF Power Capability 2 NOT included (0) */
                gs_bit_stream_insert_field(&bit_stream,1,0);
            }
        }
        else
        {
            /* 8PSK modulation in uplink NOT supported */
            gs_bit_stream_insert_field(&bit_stream,1,0);
  
            /* 8-PSK RF Power Capability 1 not included */
            gs_bit_stream_insert_field(&bit_stream,1,0);
  
            /* 8-PSK RF Power Capability 2 not included */
            gs_bit_stream_insert_field(&bit_stream,1,0);
        }
      }
      else
      {
         /* 8-PSK struct (not included 0) */
         gs_bit_stream_insert_field(&bit_stream,1,0);	  
      }
  }
  else
  {
#ifndef FEATURE_ECSD_NOT_SUPPORTED
    /* EDGE multislot capability (not included 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);
#endif
    /* 8-PSK struct (not included 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);    
  }
#else
#ifndef FEATURE_ECSD_NOT_SUPPORTED
  /* EDGE multislot capability (not included 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);
#endif
  /* 8-PSK struct (not included 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);
#endif /* FEATURE_GSM_EGPRS */

  /* GSM400 bands supported (not included 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);

  /* 
  ** GSM 850 associated radio capability
  ** Should not be included for SINGLE band MS.
  */
#ifdef FEATURE_GSM_MULTIBAND
  if ((params_ptr->gsm_supported == FALSE) || 
#ifdef FEATURE_DUAL_SIM
      (rr_ds_get_supported_band_count(params_ptr->as_id) <= 1)
#else
      (rr_get_supported_band_count() <= 1)
#endif
     )
  {
    /* only a single (or no gsm bands) are supported */
    gs_bit_stream_insert_field(&bit_stream,1,0);
  }
#ifdef FEATURE_DUAL_SIM
  else if (rr_ds_is_supported_band(BAND_GSM_850,params_ptr->as_id))
#else
  else if (rr_is_supported_band(BAND_GSM_850))
#endif
  {
    CM3_supported = TRUE;

    /* GSM 850 associated radio capability  */
    gs_bit_stream_insert_field(&bit_stream,1,1);
    
    gs_bit_stream_insert_field(
      &bit_stream,
      4,
#ifdef FEATURE_DUAL_SIM
      gs_encode_associated_radio_capability((byte)rr_ds_get_band_rf_power_class(BAND_GSM_850,params_ptr->as_id))
#else
      gs_encode_associated_radio_capability((byte)rr_get_band_rf_power_class(BAND_GSM_850))
#endif
     );
  }
  else
  {   
    gs_bit_stream_insert_field(&bit_stream,1,0);
  }
#else
  #error code not present
#endif

  /* PCS 1900 associated radio capability field encoding */
  {
    /***********************************************************/
    /* Semantic rule indicates that a single band MS shall not */
    /* indicate the band it supports in the PCS1900 associated */
    /* radio capability field                                  */
    /***********************************************************/
#ifdef FEATURE_GSM_MULTIBAND
    if ((params_ptr->gsm_supported == FALSE) || 
#ifdef FEATURE_DUAL_SIM
        (rr_ds_get_supported_band_count(params_ptr->as_id) <= 1)
#else
        (rr_get_supported_band_count() <= 1)
#endif
      )
    {
      /* only a single (or no gsm bands) are supported */
      gs_bit_stream_insert_field(&bit_stream,1,0);
    }
    /* otherwise, if PCS 1900 is a supported band */
    else if (PCS1900_supported)
    {
      CM3_supported = TRUE;
      /* include the PCS1900 associated radio capability field */
      gs_bit_stream_insert_field(&bit_stream,1,1);
      gs_bit_stream_insert_field(
        &bit_stream,
        4,
#ifdef FEATURE_DUAL_SIM
        gs_encode_associated_radio_capability((byte)rr_ds_get_band_rf_power_class(BAND_PCS,params_ptr->as_id))
#else
        gs_encode_associated_radio_capability((byte)rr_get_band_rf_power_class(BAND_PCS))
#endif
      );
    }
    else /* PCS1900 not supported */
    {
      /* don't include the field */
      gs_bit_stream_insert_field(&bit_stream,1,0);
    }
#else
    #error code not present
#endif /* FEATURE_GSM_MULTIBAND */
  } /* PCS 1900 associated radio capability */ 

#ifdef FEATURE_INTERRAT_HANDOVER_GTOW
  /* UMTS FDD Radio Access Technology Capability (1 if supported, 0 otherwise) */
  /* if GSM to WCMDA Handover is supported, indicate support only if WCDMA is supported */
  gs_bit_stream_insert_field(&bit_stream,1,(params_ptr->wcdma_supported) ? 1 : 0);

  if (params_ptr->wcdma_supported)
  {
    CM3_supported = TRUE;
  }
#else
  /* UMTS FDD Radio Access Technology Capability (1 if supported, 0 otherwise) */
  /* if GSM to WCDMA handover is not supported, code as 0 if GSM is currently the active RAT */
  gs_bit_stream_insert_field(&bit_stream,1,(params_ptr->wcdma_supported && params_ptr->wcdma_in_use) ? 1 : 0);
  if (params_ptr->wcdma_supported && params_ptr->wcdma_in_use)
  {
    CM3_supported = TRUE;
  }
#endif

  /* UMTS TDD Radio Access Technology Capability (not supported 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);

  /* CDMA2000 Radio Access Technology Capability (not supported 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);

#ifndef FEATURE_GSM_DTM
  /* DTM GPRS MultiSlot sub-class (not included 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);
#else /* DTM is supported - include DTM multislot capability */
  if (gmm_nv_dtm_feature_support_sim[params_ptr->as_id])
  {
    CM3_supported = TRUE;
    /* DTM GPRS MultiSlot sub-class (included 1) */
    gs_bit_stream_insert_field(&bit_stream,1,1);
    /* DTM GPRS MultiSlot capability */
    gs_bit_stream_insert_field(&bit_stream,2,DTM_GPRS_multislot_subclass);
    /* Single slot DTM  (not supported 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);
#ifdef FEATURE_GSM_EGPRS
    if (gmm_edge_feature_support_sim[params_ptr->as_id])
    {
      /* DTM EGPRS MultiSlot sub-class (included 1) */
      gs_bit_stream_insert_field(&bit_stream,1,1);
      /* DTM EGPRS MultiSlot capability  */
      gs_bit_stream_insert_field(&bit_stream,2,DTM_EGPRS_multislot_subclass);
    }
    else
#endif /*FEATURE_GSM_EGPRS*/
    {
      /* DTM EGPRS MultiSlot sub-class (not supported 0) */
      gs_bit_stream_insert_field(&bit_stream,1,0);
    }
  }
  else
  {
    /* DTM GPRS MultiSlot sub-class (not included 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);
  }
#endif

  /* single band support field encoding */
  {
    /*******************************************************************************/
    /* This field shall be sent if the mobile station supports UMTS(WCDMA) and one */
    /* and only one GSM band with the exception of R-GSM; the field shall not be   */
    /* sent otherwise                                                              */
    /*******************************************************************************/
    if (params_ptr->wcdma_supported)
    {
#ifdef FEATURE_GSM_MULTIBAND
      /* only send if a single band is supported */
#ifdef FEATURE_DUAL_SIM
      if (params_ptr->gsm_supported && (rr_ds_get_supported_band_count(params_ptr->as_id) == 1))
#else
      if (params_ptr->gsm_supported && (rr_get_supported_band_count() == 1))
#endif
      {
        CM3_supported = TRUE;
        
        /* include the field */
        gs_bit_stream_insert_field(&bit_stream,1,1);

        /* encode the supported band value appropriately */
#ifdef FEATURE_DUAL_SIM
        if (rr_ds_is_supported_band(BAND_EGSM,params_ptr->as_id))
#else
        if (rr_is_supported_band(BAND_EGSM))
#endif
        {
          /* encoded as 0000 */
          gs_bit_stream_insert_field(&bit_stream,4,0x0);
        }
#ifdef FEATURE_DUAL_SIM
        else if (rr_ds_is_supported_band(BAND_PGSM,params_ptr->as_id))
#else
        else if (rr_is_supported_band(BAND_PGSM))
#endif
        {
          /* encoded as 0001 */
          gs_bit_stream_insert_field(&bit_stream,4,0x1);
        }
#ifdef FEATURE_DUAL_SIM
        else if (rr_ds_is_supported_band(BAND_DCS,params_ptr->as_id))
#else
        else if (rr_is_supported_band(BAND_DCS))
#endif
        {
          /* encoded as 0010 */
          gs_bit_stream_insert_field(&bit_stream,4,0x2);
        }
#ifdef FEATURE_DUAL_SIM
        else if (rr_ds_is_supported_band(BAND_GSM_850,params_ptr->as_id))
#else
        else if (rr_is_supported_band(BAND_GSM_850))
#endif
        {
          /* encoded as 0101 */
          gs_bit_stream_insert_field(&bit_stream,4,0x5);
        }
#ifdef FEATURE_DUAL_SIM
        else if (rr_ds_is_supported_band(BAND_PCS,params_ptr->as_id))
#else
        else if (rr_is_supported_band(BAND_PCS))
#endif
        {
          /* encoded as 0110 */
          gs_bit_stream_insert_field(&bit_stream,4,0x6);
        }
        else
        {
          ERR("Invalid supported band value",0,0,0);
        }
      }
      else /* other than 1 GSM band supported (or GSM not supported) */
      {
        /* don't include the field */
        gs_bit_stream_insert_field(&bit_stream,1,0);
      }
#else /* multiband not supported */
      #error code not present
#endif /* FEATURE_GSM_MULTIBAND */
    }
    else /* WCDMA not supported */
    {
      /* don't include the field */
      gs_bit_stream_insert_field(&bit_stream,1,0);
    }
  } /* single band support field */

  /* Release 4 features start here */

#ifdef FEATURE_REL_4_SUPPORT

  /* GSM 700 associated radio capability (not included 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);

  /* UMTS 1.28Mcps TDD Radio Access Technology Capability  */
  NAS_ENTER_CRIT_SECT(mm_rat_prioritylist_crit_sec);
      if (((mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info_sim[stack_id_nas],SYS_RAT_TDS_RADIO_ACCESS)) && 
            !(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info_sim[stack_id_nas],SYS_RAT_UMTS_RADIO_ACCESS))) 
#ifdef FEATURE_SGLTE
             && !MM_IS_IN_SGLTE_STATE_4
#endif
         )
      {
          gs_bit_stream_insert_field(&bit_stream,1,1);

      }
      else
      {
          gs_bit_stream_insert_field(&bit_stream,1,0);
      }
  NAS_EXIT_CRIT_SECT(mm_rat_prioritylist_crit_sec);

  /* GERAN Feature Package 1 */
#ifdef FEATURE_GSM_GERAN_FEATURE_PACK_1
  gs_bit_stream_insert_field(&bit_stream,1,gmm_edge_nw_ass_cell_change_sim[params_ptr->as_id]); 
  if (gmm_edge_nw_ass_cell_change_sim[params_ptr->as_id])
  {
    CM3_supported = TRUE;
  }  
#else
  gs_bit_stream_insert_field(&bit_stream,1,0); /* (not supported 0) */
#endif

  /* Extended DTM GPRS Multi Slot Class (not included 0) */
  gs_bit_stream_insert_field(&bit_stream,1,0);

#endif /* FEATURE_REL_4_SUPPORT */

#if defined(FEATURE_REL5) || (!defined(FEATURE_WCDMA) && defined(FEATURE_REL_6_SUPPORT))
   #ifndef FEATURE_REL_4_SUPPORT
     #error FEATURE_REL5 requires FEATURE_REL_4_SUPPORT
   #endif

    /* High Multislot Capability */
    gs_bit_stream_insert_field(&bit_stream,1,0);

    /* GERAN Iu Mode Capabilities */
    gs_bit_stream_insert_field(&bit_stream,1,0);

    /* GERAN Feture Package 2 */
#ifdef FEATURE_GSM_GERAN_FEATURE_PACK_2
    CM3_supported = TRUE;
    gs_bit_stream_insert_field(&bit_stream,1,1); /* (supported 1) */
#else
    gs_bit_stream_insert_field(&bit_stream,1,0); /* (not supported 0) */
#endif
    gs_bit_stream_insert_field(&bit_stream,2,gmm_nv_multislot_gmsk_power_profile_sim[params_ptr->as_id]);

    /* 8-PSK Multislot Power Profile */
    gs_bit_stream_insert_field(&bit_stream,2,gmm_nv_multislot_8psk_power_profile_sim[params_ptr->as_id]);
       
#endif /* FEATURE_REL5 */

#ifdef FEATURE_REL_6_SUPPORT  
    #if !defined (FEATURE_REL_4_SUPPORT) || (defined(FEATURE_WCDMA) && !defined(FEATURE_REL5))
      #error FEATURE_REL_6_SUPPORT requires FEATURE_REL_4_SUPPORT and FEATURE_REL5
    #endif

    /* T-GSM 400 Bands Supported (not included 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);

    /* T-GSM 900 Associated Radio Capability (not included 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);

    /* SAIC/DARP Support */
#ifdef FEATURE_GSM_EGPRS_SAIC
    gs_bit_stream_insert_field(&bit_stream,2,gmm_nv_saic_feature_support);
    if (gmm_nv_saic_feature_support)
    {
      CM3_supported = TRUE;
    }
#else
    gs_bit_stream_insert_field(&bit_stream,2,0); /* (not supported 0) */
#endif /* FEATURE_GSM_EGPRS_SAIC */

#ifdef FEATURE_GSM_EDTM
    /* DTM Enhancements Capability (controlled by NV item 4209) */
    gs_bit_stream_insert_field(&bit_stream,1,gmm_nv_edtm_feature_support_sim[params_ptr->as_id]);
    if (gmm_nv_edtm_feature_support_sim[params_ptr->as_id])
    {
      CM3_supported = TRUE;
    }
#else
    /* DTM Enhancements Capability (not supported 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);
#endif /* FEATURE_GSM_EDTM */

    /* DTM GPRS High Multi Slot Class (not included 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);

#if defined(FEATURE_GSM_R_SACCH) || defined(FEATURE_GSM_R_FACCH)
    if ((gmm_nv_r_acch_feature_support != 0) && (gmm_nv_r_acch_feature_support != 2))
    {
      /* R_SACCH  and R_FACCH are supported */
      gs_bit_stream_insert_field(&bit_stream,1,1);
      CM3_supported = TRUE;
    }
    else
#endif /* FEATURE_GSM_R_SACCH */
      /* R_SACCH not supported and R_FACCH supported */
      gs_bit_stream_insert_field(&bit_stream,1,0);
#else
  if( include_rel6_IE = TRUE)
  {
        /* T-GSM 400 Bands Supported (not included 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);

    /* T-GSM 900 Associated Radio Capability (not included 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);

    /* SAIC/DARP Support */
    gs_bit_stream_insert_field(&bit_stream,2,0); /* (not supported 0) */

    /* DTM Enhancements Capability (not supported 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);

    /* DTM GPRS High Multi Slot Class (not included 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);

#if defined(FEATURE_GSM_R_SACCH) || defined(FEATURE_GSM_R_FACCH)
    if ((gmm_nv_r_acch_feature_support != 0) && (gmm_nv_r_acch_feature_support != 2))
    {
      /* R_SACCH  and R_FACCH are supported */
      gs_bit_stream_insert_field(&bit_stream,1,1);
      CM3_supported = TRUE;
    }
    else
#endif /* FEATURE_GSM_R_SACCH */
    /* R_SACCH not supported and R_FACCH supported */
    gs_bit_stream_insert_field(&bit_stream,1,0);
  }
#endif /* FEATURE_REL_6_SUPPORT */

  if(include_rel7_IE == TRUE)
  {
    /* GSM 710 Associated Radio Capability (not included 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);

    /* T-GSM 810 Associated Radio Capability (not included 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);

#ifdef FEATURE_GSM_DTM
    if (gmm_nv_dtm_feature_support_sim[params_ptr->as_id])
    {
      /* Ciphering Mode Setting Capability (supported 1) */
      gs_bit_stream_insert_field(&bit_stream,1,1);
    }
    else
    {
      /* Ciphering Mode Setting Capability (supported 0) */
      gs_bit_stream_insert_field(&bit_stream,1,0);
    }
#else
    /* Ciphering Mode Setting Capability (supported 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);
#endif
    CM3_supported = TRUE;

    /* Additional Positioning Capabilities (not supported 0) */
    gs_bit_stream_insert_field(&bit_stream,1,0);
  }

  if(include_rel8_IE == TRUE)
  {
#if defined(FEATURE_LTE)

      NAS_ENTER_CRIT_SECT(mm_rat_prioritylist_crit_sec);

      mm_checkif_rat_pri_list_has_rat_local = mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info_sim[stack_id_nas],SYS_RAT_LTE_RADIO_ACCESS);

#ifdef FEATURE_SGLTE
      if(MM_IS_IN_SGLTE_STATE_4)
      {
        mm_checkif_rat_pri_list_has_rat_local = FALSE;
      }
#endif
    /*E-UTRA FDD support is true only if Rat priority list contains the lte rat */
    if (mm_checkif_rat_pri_list_has_rat_local && 
        lte_rrc_lte_fdd_is_supported(mm_get_lte_band_capacity(&mm_rat_pri_list_info_sim[stack_id_nas])))
    {
      /* E-UTRA FDD support - Supported*/
      gs_bit_stream_insert_field(&bit_stream,1,1);
      CM3_supported = TRUE;
      eutra_FDD_TDD_support = TRUE;
    }
    else
    {
      /* E-UTRA FDD support - Not Supported*/
      gs_bit_stream_insert_field(&bit_stream,1,0);
    }

    /*E-UTRA TDD support is true only if Rat priority list contains the lte rat */
    if (mm_checkif_rat_pri_list_has_rat_local  &&
        lte_rrc_lte_tdd_is_supported(mm_get_lte_band_capacity(&mm_rat_pri_list_info_sim[stack_id_nas])))
    {
      /* E-UTRA TDD support - Supported*/
      gs_bit_stream_insert_field(&bit_stream,1,1);
      eutra_FDD_TDD_support = TRUE;
    }
    else
    {
      /* E-UTRA TDD support - Not Supported*/
      gs_bit_stream_insert_field(&bit_stream,1,0);
    }

    NAS_EXIT_CRIT_SECT(mm_rat_prioritylist_crit_sec);

    if(eutra_FDD_TDD_support == TRUE)
    {
      /* E-UTRA Measurement and Reporting support*/
      gs_bit_stream_insert_field(&bit_stream,1,gmm_nv_eutra_measurement_reporting_support_sim[params_ptr->as_id]);
    }
    else
    {
      /*If both "E-UTRA FDD support" and "E-UTRA TDD support" bits are set to '0', 
            this field shall be set to '0'*/
      gs_bit_stream_insert_field(&bit_stream,1,0);
    }

    if (
#ifdef FEATURE_SGLTE
         (!MM_IS_IN_SGLTE_STATE_4) &&
#endif
         (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_LTE_RADIO_ACCESS) ||
          mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_UMTS_RADIO_ACCESS) || 
          mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_TDS_RADIO_ACCESS)) &&
         (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_GSM_RADIO_ACCESS)))
     {
       /* Priority-based reselection - Supported*/
       gs_bit_stream_insert_field(&bit_stream,1,1);
     }
    else
    {
       /* Priority-based reselection - NOT Supported*/
       gs_bit_stream_insert_field(&bit_stream,1,0);
    }
#else

    /* E-UTRA FDD support - Not Supported*/
    gs_bit_stream_insert_field(&bit_stream,1,0);

    /* E-UTRA TDD support - Not Supported*/
    gs_bit_stream_insert_field(&bit_stream,1,0);

    /* E-UTRA Measurement and Reporting support - Not supported */
    gs_bit_stream_insert_field(&bit_stream,1,0);
  
    /* Priority-based reselection - Not Supported*/
    gs_bit_stream_insert_field(&bit_stream,1,0);
#endif
  }
  /* if feature Vamos is on then start coding rel 9 ie*/
#ifdef FEATURE_VAMOS
  /*Reporting of UTRAN CSG cells not supported*/
  gs_bit_stream_insert_field(&bit_stream,1,0);
  
  if(mm_nv_vamos_feature_support == VAMOS_OFF)
  {
    gs_bit_stream_insert_field(&bit_stream,2,0);
  }
  else if(mm_nv_vamos_feature_support == VAMOS_1)
  {
    gs_bit_stream_insert_field(&bit_stream,2,1);
  }
  else
  {
    gs_bit_stream_insert_field(&bit_stream,2,2);
  }
#else
  /*Reporting of UTRAN CSG cells not supported*/
  gs_bit_stream_insert_field(&bit_stream,1,0);

  gs_bit_stream_insert_field(&bit_stream,2,0);
#endif

  /* Start coding rel 10 changes tighter capability*/
  gs_bit_stream_insert_field(&bit_stream,2,mm_nv_tighter_capability);



  /* spare bit should be at the end of classmark3*/
  gs_bit_stream_insert_spare(&bit_stream,1);

  /* set the length of the classmark 3 IE */
  classmark_3_ptr->length = (byte) gs_bit_stream_byte_count(&bit_stream);

  /* copy the data into the IE padded with 0s at end */
  for (index = 0; index < MIN(classmark_3_ptr->length,MAX_CLASSMARK3_DATA_SIZE); index++)
  {
    classmark_3_ptr->data[index] = data[index];
  }

  return CM3_supported;

#else

  return gs_read_coded_classmark_3
  (
    classmark_3_ptr,
    params_ptr->gsm_supported,
    params_ptr->wcdma_supported,
    params_ptr->wcdma_in_use,
    params_ptr->MSCR99
  );
#endif

} /* gs_read_coded_classmark_3_per_nas_stacks */

/**
 * gs_read_coded_classmark_3_per_subs
 * 
 * This function is called by WRRC.  If the sub configuration says it is 
 *  an SGLTE device then use ps_stack_id
 */
boolean gs_read_coded_classmark_3_per_subs(
  classmark_3_ie_T     *classmark_3_ptr,
  classmark_params_T   *params_ptr
)
{
  mm_as_id_e_type stack_id_nas = (mm_as_id_e_type)params_ptr->as_id;

#if defined(FEATURE_SGLTE)
  if(!IS_NOT_VALID_SUB_ID(params_ptr->as_id))
  {
    if(MM_SUB_IS_SGLTE_SUB((mm_as_id_e_type)params_ptr->as_id))  
    {
      stack_id_nas = mm_ps_stack_id;
    }
  }
#endif
  return gs_read_coded_classmark_3_per_nas_stacks(classmark_3_ptr,params_ptr,
                                                  (sys_modem_as_id_e_type)stack_id_nas);
}

#ifdef FEATURE_DUAL_SIM

void nas_read_gprs_hw_classmark_per_subs(gprs_hw_classmark_T *classmark
                              ,sys_modem_as_id_e_type as_id
                               )
{
  if(IS_NOT_VALID_SUB_ID(as_id))
  {
    ERR_FATAL("Invalid SUB ID:%d", as_id+1,0,0);
    return;
  }
  MSG_HIGH_DS(MM_LOCAL_SUB, "=MM= Class Mark Read from Nv is  : %d",gmm_gea_support_sim[as_id],0,0);

#ifdef FEATURE_GSM_EGPRS
  classmark->GEA_1_supported = gea_classmark_enquiry_per_subs( as_id, GMMLLC_GEA_1 );
  classmark->GEA_2_supported = gea_classmark_enquiry_per_subs( as_id, GMMLLC_GEA_2 ); 
  classmark->GEA_3_supported = gea_classmark_enquiry_per_subs( as_id, GMMLLC_GEA_3 );
  classmark->GEA_4_supported = gea_classmark_enquiry_per_subs( as_id, GMMLLC_GEA_4 );
  classmark->GEA_5_supported = gea_classmark_enquiry_per_subs( as_id, GMMLLC_GEA_5 );
  classmark->GEA_6_supported = gea_classmark_enquiry_per_subs( as_id, GMMLLC_GEA_6 );
  classmark->GEA_7_supported = gea_classmark_enquiry_per_subs( as_id, GMMLLC_GEA_7 );
#elif defined(FEATURE_GSM_GPRS)
  classmark->GEA_1_supported = 1;
  classmark->GEA_2_supported = 1;
  classmark->GEA_3_supported = 0;
  classmark->GEA_4_supported = 0;
  classmark->GEA_5_supported = 0;
  classmark->GEA_6_supported = 0;
  classmark->GEA_7_supported = 0;
#else
  classmark->GEA_1_supported = 0;
  classmark->GEA_2_supported = 0;
  classmark->GEA_3_supported = 0;
  classmark->GEA_4_supported = 0;
  classmark->GEA_5_supported = 0;
  classmark->GEA_6_supported = 0;
  classmark->GEA_7_supported = 0;
#endif
}

boolean gea_classmark_enquiry_per_subs(sys_modem_as_id_e_type as_id,gmmllc_gea_e_type gea_type) 
{
    boolean gea_supported_bit;

    switch (gea_type)
    {
        case GMMLLC_GEA_NONE:
        case GMMLLC_GEA_UNDEFINED:
             gea_supported_bit = TRUE;
        break;

        case GMMLLC_GEA_1:

             /* GEA1 support is identified by means of bitmask bit0 */
             gea_supported_bit = ( gmm_gea_support_sim[as_id] & 0x01 ) ? TRUE:FALSE;
        break;
           /* --------------------------------------------------------------------- */

        case GMMLLC_GEA_2:

             /* GEA2 support is identified by means of bitmask bit1 */
             gea_supported_bit = ( gmm_gea_support_sim[as_id] & 0x02 ) ? TRUE:FALSE;
        break;
             /* --------------------------------------------------------------------- */
        case GMMLLC_GEA_3:
             /* GEA3 support is identified by means of bitmask bit2 */
             gea_supported_bit = ( gmm_gea_support_sim[as_id] & 0x04 ) ? TRUE:FALSE;
             break;
             /* --------------------------------------------------------------------- */
        case GMMLLC_GEA_4:
             /* GEA4 support is identified by means of bitmask bit3 */
             gea_supported_bit = ( gmm_gea_support_sim[as_id] & 0x08 ) ? TRUE:FALSE;
             break;
             /* --------------------------------------------------------------------- */
        case GMMLLC_GEA_5:
        case GMMLLC_GEA_6:
        case GMMLLC_GEA_7:
        default:
             gea_supported_bit = FALSE;
             break;
    }

    return(gea_supported_bit);
}

#else
void nas_read_gprs_hw_classmark(gprs_hw_classmark_T *classmark)
{
  MSG_HIGH_DS(MM_LOCAL_SUB, "=MM= Class Mark Read from Nv is  : %d",gmm_gea_support,0,0);

#ifdef FEATURE_GSM_EGPRS
  classmark->GEA_1_supported = gea_classmark_enquiry(GMMLLC_GEA_1);
  classmark->GEA_2_supported = gea_classmark_enquiry(GMMLLC_GEA_2 ); 
  classmark->GEA_3_supported = gea_classmark_enquiry(GMMLLC_GEA_3 );
  classmark->GEA_4_supported = gea_classmark_enquiry(GMMLLC_GEA_4 );
  classmark->GEA_5_supported = gea_classmark_enquiry(GMMLLC_GEA_5 );
  classmark->GEA_6_supported = gea_classmark_enquiry(GMMLLC_GEA_6 );
  classmark->GEA_7_supported = gea_classmark_enquiry(GMMLLC_GEA_7 );
#elif defined(FEATURE_GSM_GPRS)
  classmark->GEA_1_supported = 1;
  classmark->GEA_2_supported = 1;
  classmark->GEA_3_supported = 0;
  classmark->GEA_4_supported = 0;
  classmark->GEA_5_supported = 0;
  classmark->GEA_6_supported = 0;
  classmark->GEA_7_supported = 0;
#else
  classmark->GEA_1_supported = 0;
  classmark->GEA_2_supported = 0;
  classmark->GEA_3_supported = 0;
  classmark->GEA_4_supported = 0;
  classmark->GEA_5_supported = 0;
  classmark->GEA_6_supported = 0;
  classmark->GEA_7_supported = 0;
#endif
}

boolean gea_classmark_enquiry(gmmllc_gea_e_type gea_type) 
{
    boolean gea_supported_bit;

    switch (gea_type)
    {
        case GMMLLC_GEA_NONE:
        case GMMLLC_GEA_UNDEFINED:

          gea_supported_bit = TRUE;
         break;

        case GMMLLC_GEA_1:

          /* GEA1 support is identified by means of bitmask bit0 */
          gea_supported_bit = ( gmm_gea_support & 0x01 ) ? TRUE:FALSE;
         break;
        /* --------------------------------------------------------------------- */

        case GMMLLC_GEA_2:

          /* GEA2 support is identified by means of bitmask bit1 */
          gea_supported_bit = ( gmm_gea_support & 0x02 ) ? TRUE:FALSE;
         break;
          /* --------------------------------------------------------------------- */
        case GMMLLC_GEA_3:
          /* GEA3 support is identified by means of bitmask bit2 */
          gea_supported_bit = ( gmm_gea_support & 0x04 ) ? TRUE:FALSE;
         break;
          /* --------------------------------------------------------------------- */
        case GMMLLC_GEA_4:
          /* GEA4 support is identified by means of bitmask bit3. */
          gea_supported_bit = ( gmm_gea_support & 0x08 ) ? TRUE:FALSE;
         break;
          /* --------------------------------------------------------------------- */
        case GMMLLC_GEA_5:
        case GMMLLC_GEA_6:
        case GMMLLC_GEA_7:
        default:
           gea_supported_bit = FALSE;
    }  

    return(gea_supported_bit);
}  

#endif
#ifdef FEATURE_CGPS_UMTS_CP_GSM
void gsclassmark_register_cgps_cb( cgps_classmark_cb_type    cb_fp )
{
  cgps_classmark_cb_fp = cb_fp;
}
#endif  /* FEATURE_CGPS_UMTS_CP_GSM */

/* SGLTE interface changes */
#ifdef FEATURE_SGLTE
void gs_read_coded_classmark_2_sglte(
classmark_2_ie_T *classmark_2_ptr,
  boolean           gsm_supported,
  boolean           wcdma_supported,
  boolean           wcdma_in_use,
  boolean           MSCR99,
  sys_radio_access_tech_e_type current_rat
)
{
  classmark_params_T params_ptr;
  params_ptr.gsm_supported = gsm_supported;
  params_ptr.wcdma_supported = wcdma_supported;
  params_ptr.wcdma_in_use = wcdma_in_use;
  params_ptr.MSCR99 = MSCR99;
#ifndef FEATURE_DUAL_SIM
   params_ptr.as_id = SYS_MODEM_AS_ID_1;
#endif
  if ( SYS_RAT_GSM_RADIO_ACCESS == current_rat )
  {
    gs_read_coded_classmark_2_per_nas_stacks(classmark_2_ptr, &params_ptr, (sys_modem_as_id_e_type)mm_cs_stack_id);
    
  }
  else if((SYS_RAT_UMTS_RADIO_ACCESS == current_rat) || (SYS_RAT_TDS_RADIO_ACCESS == current_rat ) ||
               (SYS_RAT_LTE_RADIO_ACCESS == current_rat))
  {
    gs_read_coded_classmark_2_per_nas_stacks(classmark_2_ptr, &params_ptr, (sys_modem_as_id_e_type)mm_ps_stack_id);
   }
}
boolean gs_read_coded_classmark_3_sglte(
  classmark_3_ie_T     *classmark_3_ptr,
  boolean           gsm_supported,
  boolean           wcdma_supported,
  boolean           wcdma_in_use,
  boolean           MSCR99,
  sys_radio_access_tech_e_type current_rat
)
{
  classmark_params_T   params_ptr;
  params_ptr.gsm_supported = gsm_supported;
  params_ptr.wcdma_supported = wcdma_supported;
  params_ptr.wcdma_in_use = wcdma_in_use;
  params_ptr.MSCR99 = MSCR99;
#ifndef FEATURE_DUAL_SIM
  params_ptr.as_id = SYS_MODEM_AS_ID_1;
#endif
  if ( SYS_RAT_GSM_RADIO_ACCESS == current_rat )
  {
    return ( gs_read_coded_classmark_3_per_nas_stacks(classmark_3_ptr, &params_ptr, (sys_modem_as_id_e_type)mm_cs_stack_id));
  }
  else 
  {
    return (gs_read_coded_classmark_3_per_nas_stacks(classmark_3_ptr, &params_ptr, (sys_modem_as_id_e_type)mm_ps_stack_id));
  }
}

byte gs_get_classmark_1_sglte(
boolean gsm_supported, 
boolean wcdma_in_use, 
boolean MSCR99,
sys_radio_access_tech_e_type current_rat
)
{

  classmark_params_T params_ptrs;
  params_ptrs.gsm_supported = gsm_supported;
  params_ptrs.wcdma_in_use = wcdma_in_use;
  params_ptrs.MSCR99 = MSCR99;
#ifndef FEATURE_DUAL_SIM
  params_ptrs.as_id = SYS_MODEM_AS_ID_1;
#endif
  if ( SYS_RAT_GSM_RADIO_ACCESS == current_rat )
  {
    return (gs_get_classmark_1_per_nas_stacks( &params_ptrs, (sys_modem_as_id_e_type)mm_cs_stack_id));
  }
  else 
  {
    return (gs_get_classmark_1_per_nas_stacks( &params_ptrs, (sys_modem_as_id_e_type)mm_ps_stack_id));
  }
}
#endif

/**
 * Encodes the classmark1 information in classmark1 IE form
 * 
 * 
**/
byte gs_get_classmark_1_with_rat
(
  classmark_params_T    *params_ptr
)
{
  sys_modem_as_id_e_type stack_id_nas;
#ifndef FEATURE_DUAL_SIM
  params_ptr->as_id = SYS_MODEM_AS_ID_1;
#endif
  stack_id_nas = params_ptr->as_id;
#if defined(FEATURE_SGLTE)
  if(!IS_NOT_VALID_SUB_ID(params_ptr->as_id))
  {
    if(MM_SUB_IS_SGLTE_SUB(params_ptr->as_id))
    {
      if(params_ptr->rat == SYS_RAT_GSM_RADIO_ACCESS)
      {
        stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
      }
      else
      {
        stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
      }
    }
  }
#endif 
  return (gs_get_classmark_1_per_nas_stacks(params_ptr,stack_id_nas));
}


/**
 * encodes the classmark2 information in classmark2 IE format (24.008 10.5.1.6)
 * 
 * @param classmark_2_ptr pointer to classmark2 IE where encoded classmark2 information will be stored
 * 
 * @param gsm_supported   TRUE if GSM RAT is supported, FALSE otherwise
 * 
 * @param wcdma_supported TRUE if WCDMA RAT is supported, FALSE otherwise
 * 
 * @param wcdma_or_lte_in_use    TRUE if WCDMA or LTE is the active RAT, FALSE if GSM is the active RAT
 * 
 * @param MSCR99          TRUE if the network is a R99 or later, FALSE if the network is R98 or earlier
 *                        based on MSCR (MSC revision) value broadcast by network. Should always be set
 *                        to TRUE if called from WCDMA (RRC)
 */

void gs_read_coded_classmark_2_with_rat(
  classmark_2_ie_T  *classmark_2_ptr,
  classmark_params_T   *params_ptr
)
{
  sys_modem_as_id_e_type stack_id_nas;
#ifndef FEATURE_DUAL_SIM
  params_ptr->as_id = SYS_MODEM_AS_ID_1;
#endif
  stack_id_nas = params_ptr->as_id;
#if defined(FEATURE_SGLTE)
  if(!IS_NOT_VALID_SUB_ID(params_ptr->as_id))
  {
    if(MM_SUB_IS_SGLTE_SUB(params_ptr->as_id))
    {
      if(params_ptr->rat == SYS_RAT_GSM_RADIO_ACCESS)
      {
        stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
      }
      else
      {
        stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
      }
    }
  }
#endif 
  gs_read_coded_classmark_2_per_nas_stacks(classmark_2_ptr, params_ptr,stack_id_nas);
}


/**
 * encodes the classmark3 information into the provided classmark3 IE
 * 
 * @param classmark_3_ptr pointer to classmark3 IE where encoded data should be stored
 * 
 * @param gsm_supported   TRUE if GSM RAT is supported, FALSE otherwise
 * 
 * @param wcdma_supported TRUE if WCDMA RAT is supported, FALSE otherwise
 * 
 * @param wcdma_in_use    TRUE if WCDMA is the active RAT, FALSE if GSM is the active RAT
 * 
 * @param MSCR99          TRUE if the network is a R99 or later, FALSE if the network is R98 or earlier
 *                        based on MSCR (MSC revision) value broadcast by network. Should always be set
 *                        to TRUE if called from WCDMA (RRC)
 */

/* interface changes end here */
boolean gs_read_coded_classmark_3_with_rat(
  classmark_3_ie_T     *classmark_3_ptr,
  classmark_params_T   *params_ptr
)
{
  sys_modem_as_id_e_type stack_id_nas;
#ifndef FEATURE_DUAL_SIM
  params_ptr->as_id = SYS_MODEM_AS_ID_1;
#endif
  stack_id_nas = params_ptr->as_id;
#if defined(FEATURE_SGLTE)
  if(!IS_NOT_VALID_SUB_ID(params_ptr->as_id))
  {
    if(MM_SUB_IS_SGLTE_SUB(params_ptr->as_id))
    {
      if(params_ptr->rat == SYS_RAT_GSM_RADIO_ACCESS)
      {
        stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
      }
      else
      {
        stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
      }
    }
  }
#endif 
  return (gs_read_coded_classmark_3_per_nas_stacks(classmark_3_ptr, params_ptr,stack_id_nas));
}


/* interface changes end here */

