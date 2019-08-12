/*==================================================================================================

                                          F A C H   H A N D L E R


GENERAL DESCRIPTION

  This module contains functions to process FACH configuration data

Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2002-2005, 2008-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

==================================================================================================*/

/*=================================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macfach.c_v   1.8   12 Mar 2002 15:42:00   psuvarna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/macfach.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when      who   what, where, why
--------  ---   ---------------------------------------------------------------------------------
10/17/14  ts    Fix to discard DL PDUs received while in the Reset Pending State, to avoid potential ciphering mismatch right after the recovery
07/26/14  rkmk  Reduce image size by using right macro depending upon number of parameters
01/17/13  as    Removed unwanted F3.
07/13/12  grk   MBMS feature cleanup at L2(MBMS_DEBUG_1, FEATURE_MODEM_MBMS)
01/02/12  grk   Feature cleanup
06/10/11  kc    Fixed returning incorrect transport channel type for certain logical channels 
06/02/11  kc    Added support for logical channel to transport channel mapping info
06/14/10  grk   Fixed compilation errors/warnings.
10/28/09  kvk   Removed cipher_algo under PP feature 
10/05/09  prk   Fixed Klocwork warnings
05/29/09  ssg   Added support for SNOW_3G algo.
05/07/09  ssg   Updated Copyright Information
04/29/09  av    Fixed compiler warnings in L2 
12/10/08  ksr   Merge error for ciphering algo
10/17/08  grk   Merged taxis changes to mainline.
05/19/08  kvk   Now in FACH hdr read event check the status of rlc_dl_decode_pdu_hdr() 
                and mark the valid flag  in TB hdr ptr to TRUE/FALSE accordingly
01/29/08  sk    Support for MBMS under FEATURE_MODEM_MBMS
07/21/05  gsc   Fixed some more lint errors.
03/09/05  sk    Fixed all lint errors.
12/30/04  sk    Fixed lint errors
07/27/03  sk    Set the tb_in_tm_mode flga to FALSE. This flag will indicate to layer1 that the
                transport block is in UM/AM mode.
07/09/03  sk    Initialized ciphering_control flag to false by default.
11/22/02  sk    Populating rlc_hdr_info under FEATURE_UMTS_CIPHERING_ENABLE
11/22/02  sk    Performed ciphering for AM and UM mode rb's in downlink FACH.
03/12/02  ps    Fixed C/T field offset in PROCESS_DCCH_DTCH_TB_WITH_CRNTI_ON_FACH
02/12/02  ps    Fixed compiler warning
11/26/01  ps    Commented the call to "rlc_dl_decode_pdu_hdr()" function
08/07/01  ps    Fixed the leading zero message format problem
06/18/01  ps    Adjusted the MSG levels
04/04/01  ps    Initial release after merging "macfachcfg.c" and "macfachhdr.c"

==================================================================================================*/

/*==================================================================================================
                     INCLUDE FILES
==================================================================================================*/
#include "wcdma_variation.h"
#include  "customer.h"
#include  "rex.h"
#include  "msg.h"
#include  "macrrcif_v.h"
#include  "macrlcif.h"
#include  "l2dltask.h"
#include  "macinternal.h"
#include  "macdl.h"
#include  "macfach.h"

/*==================================================================================================
                            DATA DECLARATIONS
==================================================================================================*/

/* Number of active FACHs */
uint8 num_active_fachs;

/*==================================================================================================
MACRO NAME    PROCESS_BCCH_FACH_CONFIG

DESCRIPTION   This macro processes configuration info of BCCH that is maaped to FACH
==================================================================================================*/
#define   PROCESS_BCCH_FACH_CONFIG\
\
  /* Initially mark BCCH as disabled */ \
  int_fach_ptr->bcch_enable = FALSE;  \
\
    /* BCCH is enabled */ \
    if (oc_fach_ptr->bcch_enable){  \
\
        /* Invalid BCCH ID */ \
        if (DL_RLC_ID_IS_INVALID(oc_fach_ptr->bcch_id)){  \
      MSG_HIGH ("invalid BCCH ID %d",oc_fach_ptr->bcch_id, 0, 0);  \
    } \
    /* Process BCCH/FACH Config Data */ \
    else {  \
      /* Set BCCH Enable flag to TRUE */  \
        int_fach_ptr->bcch_enable = TRUE; \
\
      /* Save BCCH ID */  \
        int_fach_ptr->bcch_id = oc_fach_ptr->bcch_id; \
\
        } \
    }

/*==================================================================================================
MACRO NAME    PROCESS_CCCH_FACH_CONFIG

DESCRIPTION   This macro processes configuration info of CCCH that is maaped to FACH
==================================================================================================*/
#define   PROCESS_CCCH_FACH_CONFIG\
\
  /* Initially mark the CCCH as disabled */ \
  int_fach_ptr->ccch_enable = FALSE;  \
\
    /* Ordered to enable CCCH */  \
    if (oc_fach_ptr->ccch_enable){  \
\
        /* Invalid CCCH ID */ \
        if (DL_RLC_ID_IS_INVALID(oc_fach_ptr->ccch_id)){  \
            MSG_HIGH ("invalid CCCH ID %d",oc_fach_ptr->ccch_id, 0, 0);  \
    } \
    /* Save CCCH info in internal config data table */  \
    else {  \
      /* Set CCCH Enable flag */  \
      int_fach_ptr->ccch_enable = TRUE; \
\
      /* Save CTCH ID */  \
      int_fach_ptr->ccch_id = oc_fach_ptr->ccch_id; \
        } \
    }

/*==================================================================================================
MACRO NAME    PROCESS_CTCH_FACH_CONFIG

DESCRIPTION   This macro processes configuration info of CTCH that is maaped to FACH
==================================================================================================*/
#define   PROCESS_CTCH_FACH_CONFIG\
\
  /* Initially mark the CTCH as disabled */ \
  int_fach_ptr->ctch_enable = FALSE;  \
\
    /* CTCH is enabled */ \
    if (oc_fach_ptr->ctch_enable){  \
\
    /* CTCH ID is valid */  \
    if (DL_RLC_ID_IS_INVALID(oc_fach_ptr->ctch_id)){  \
      MSG_HIGH ("invalid CTCH ID %d", oc_fach_ptr->ctch_id, 0, 0); \
    } \
    else {  \
      /* Set CTCH as enabled */ \
        int_fach_ptr->ctch_enable = TRUE; \
\
      /* Save CTCH ID */  \
      int_fach_ptr->ctch_id = oc_fach_ptr->ctch_id; \
\
    } \
  }

/*==================================================================================================
FUNCTION NAME   process_dcch_dtch_fach_config

DESCRIPTION   This function processes config info of DCCH/DTCH that is maaped to FACH
==================================================================================================*/
void  process_dcch_dtch_fach_config
  (
/* Ordered FACH config data pointer */
  mac_dl_fach_trch_config_type      *oc_fach_ptr,

/* Internal FACH Config data pointer */
  fach_config_info_type             *int_fach_ptr
  )
{
  boolean valid = FALSE;
  uint8 i;
  uint8 tbl_index = 0;

  /* Pointer to ordered DCCH/DTCH config data */
  mac_dl_ded_logch_config_type    *oc_dlc_ptr;

  /* Number of DCCH/DTCHs */
  int_fach_ptr->num_dlc = oc_fach_ptr->ndlchan;

  /* Set Dedicated Logical Channel Enable flag to FALSE and mark all dedicated logical
     channels as disabled */
  int_fach_ptr->dlc_enable = FALSE;
  for (i=0; i < MAX_DED_LOGCHAN_PER_TRCH; i++)

    int_fach_ptr->dlcinfo[i].enable = FALSE;

  /* Process each DCCH/DTCH config information */
  for (i=0 ; i < oc_fach_ptr->ndlchan; i++)
  {

    valid = FALSE;

    /* Pointer to the ordered DCCH/DTCH config data */
    oc_dlc_ptr = &oc_fach_ptr->dlchan_info[i];

    /* Invalid RLC ID */
    if (DL_RLC_ID_IS_INVALID(oc_dlc_ptr->rlc_id))
    {
      MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "invalid DCCH/DTCH%d ID %d",
                                                  i, oc_dlc_ptr->rlc_id);
    }
    /* Only one DCCH/DTCH is mapped to FACH */
    else if (int_fach_ptr->num_dlc == 1)
    {
      tbl_index = 0;
      valid = TRUE;
    }
    /* Multiple DCCH/DTCHs are mapped to FACH */
    else
    {
      /* MAC ID is invalid */
      if (DL_MAC_ID_IS_INVALID(oc_dlc_ptr->mac_id))
      {
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "invalid MAC ID %d",
                                                    oc_dlc_ptr->mac_id);
      }
      else
      {
        tbl_index = (uint8)(oc_dlc_ptr->mac_id - MAC_MIN_MAC_ID);
        valid = TRUE;
      }
    }

    /* Config data is valid */
    if (valid == TRUE)
    {
      /* Save DCCH/DTCH info in FACH CONFIG table */
      int_fach_ptr->dlc_enable = TRUE;
      int_fach_ptr->dlcinfo[tbl_index].enable = TRUE;
      int_fach_ptr->dlcinfo[tbl_index].rlc_id = oc_dlc_ptr->rlc_id;
      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "FACH/DCCH or DTCH ID = %d",
                                                 oc_dlc_ptr->rlc_id);
    }
  }
}


#ifdef __GNUC__
/*============================================================================================
MACRO NAME    PROCESS_BCCH_TB_ON_FACH

DESCRIPTION   This macro processes header info of FACH TB that is mapped to BCCH
============================================================================================*/
#define   PROCESS_BCCH_TB_ON_FACH(tb_num)\
\
  /* If BCCH is enabled, set L1 parameters */ \
  if (int_fach_ptr->bcch_enable == TRUE) {  \
\
    /* RLC PDU Offset */  \
    rlc_pdu_offset = 2; \
\
    /* RLC ID */  \
    rlc_id = int_fach_ptr->bcch_id; \
\
    valid = TRUE; \
  } \
    /* BCCH is not enabled */ \
    else {  \
    MSG_HIGH ("TB%d BCCH not enabled",tb_num, 0, 0); \
    }

/*============================================================================================
MACRO NAME    PROCESS_CCCH_TB_ON_FACH

DESCRIPTION   This macro processes header info of FACH TB that is mapped to CCCH
============================================================================================*/
#define   PROCESS_CCCH_TB_ON_FACH(tb_num)\
\
  /* If CCCH is anabled, set L1 parameters */ \
    if (int_fach_ptr->ccch_enable == TRUE) {  \
\
    /* RLC PDU Offset */  \
    rlc_pdu_offset = 8; \
\
    /* RLC ID */  \
    rlc_id = int_fach_ptr->ccch_id; \
\
    valid = TRUE; \
  } \
    /* CCCH not enabled */  \
    else {  \
    MSG_HIGH ("TB%d CCCH not enabled",tb_num, 0, 0); \
  }

/*============================================================================================
MACRO NAME    PROCESS_CTCH_TB_ON_FACH

DESCRIPTION   This macro processes header info of FACH TB that is mapped to CTCH
============================================================================================*/
#define   PROCESS_CTCH_TB_ON_FACH(tb_num)\
\
  /* If CTCH is anabled, set L1 parameters */ \
    if (int_fach_ptr->ctch_enable == TRUE) {  \
\
    /* RLC PDU Offset */  \
    rlc_pdu_offset = 8; \
\
    /* RLC ID */  \
    rlc_id = int_fach_ptr->ctch_id; \
\
    valid = TRUE; \
  } \
  /* CTCH not enabled */  \
    else {  \
    MSG_HIGH ("TB%d CTCH not enabled", tb_num, 0, 0);  \
    }

/*============================================================================================
MACRO NAME    PROCESS_DCCH_DTCH_TB_WITH_URNTI_ON_FACH

DESCRIPTION   This macro processes header info of FACH TB that is mapped to DCCH/DTCH with
        URNTI
============================================================================================*/
#define   PROCESS_DCCH_DTCH_TB_WITH_URNTI_ON_FACH(tb_num)\
\
  /* URNTI field of TB */ \
  urnti = (tb_hdr_ptr->tb_header[0] << 28) + (tb_hdr_ptr->tb_header[1] << 20) + \
      (tb_hdr_ptr->tb_header[2] << 12) + (tb_hdr_ptr->tb_header[3] << 4) +  \
      (tb_hdr_ptr->tb_header[4] >> 4);  \
\
  /* DL TB URNTI is invalid */  \
    if (DL_TB_URNTI_IS_INVALID(urnti)) {  \
    MSG_HIGH ("TB%d with invalid URNTI %x",tb_num, urnti, 0);  \
  } \
\
  /* FACH has one DCCH/DTCH */  \
  else if (int_fach_ptr->num_dlc == 1){ \
\
    /* Pointer to internal DCCH/DTCH config info */ \
    dlcptr = &int_fach_ptr->dlcinfo[0]; \
\
    /* DCCH/DTCH is enabled */  \
    if (dlcptr->enable == TRUE) { \
\
      /* RLC PDU Offset */  \
      rlc_pdu_offset = 36;  \
\
      /* RLC ID */  \
      rlc_id = dlcptr->rlc_id;  \
\
      valid = TRUE; \
    } \
    /* DCCH/DTCH not enabled */ \
    else {  \
      MSG_HIGH ("TB%d with disabled DCCH/DTCH", tb_num, 0, 0); \
    } \
  } \
  /* FACH has multiple DCCH/DTCHs */  \
  else if (int_fach_ptr->num_dlc > 1) { \
\
    /* C/T field of TB */ \
    ct_field = (uint8)(tb_hdr_ptr->tb_header[4] & 0x0f);  \
\
    if (DL_CT_FLD_IS_INVALID(int_fach_ptr->num_dlc,ct_field)) { \
      MSG_HIGH ("TB%d with invalid C/T %x", tb_num, ct_field, 0); \
    } \
    else {  \
      /* Pointer to internal DCCH/DTCH config info */ \
      dlcptr = &int_fach_ptr->dlcinfo[ct_field];  \
\
      /* DCCH/DTCH is enabled */  \
      if (dlcptr->enable == TRUE) { \
\
        /* RLC PDU Offset */  \
        rlc_pdu_offset = 40;  \
\
        /* RLC ID */  \
        rlc_id = dlcptr->rlc_id;  \
\
        valid = TRUE; \
      } \
      /* DCCH/DTCH not enabled */ \
      else {  \
        MSG_HIGH ("TB%d with disabled DCCH/DTCH",tb_num, ct_field, 0); \
      } \
    } \
  }

/*============================================================================================
MACRO NAME    PROCESS_DCCH_DTCH_TB_WITH_CRNTI_ON_FACH

DESCRIPTION   This macro processes header info of FACH TB that is mapped to DCCH/DTCH with
        CRNTI
============================================================================================*/
#define   PROCESS_DCCH_DTCH_TB_WITH_CRNTI_ON_FACH(tb_num)\
\
  /* CRNTI of TB */ \
  crnti = (uint16)((tb_hdr_ptr->tb_header[0] << 12) + (tb_hdr_ptr->tb_header[1] <<  4) +  \
      (tb_hdr_ptr->tb_header[2] >>  4));  \
\
  /* If C-RNTI is valid, check the C/T field */ \
    if (DL_TB_CRNTI_IS_INVALID(crnti)) {  \
    MSG_HIGH ("TB%d with invalid CRNTI %x",tb_num, crnti, 0); \
  } \
  /* FACH has single DCCH/DTCH */ \
  else if (int_fach_ptr->num_dlc == 1){ \
\
    /* Pointer to internal DCCH/DTCH config info */ \
    dlcptr = &int_fach_ptr->dlcinfo[0]; \
\
    /* DCCH/DTCH is enabled */  \
    if (dlcptr->enable == TRUE) { \
\
      /* RLC PDU Offset */  \
      rlc_pdu_offset = 20;  \
\
      /* RLC ID */  \
      rlc_id = dlcptr->rlc_id;  \
\
      valid = TRUE; \
    } \
    /* DCCH/DTCH not enabled */ \
    else {  \
      MSG_HIGH ("TB%d with disabled DCCH/DTCH", tb_num, 0, 0); \
    } \
\
  } \
  /* FACH has multiple DCCH/DTCHs */  \
  else if (int_fach_ptr->num_dlc >1){ \
    /* C/T field of TB */ \
    ct_field = (uint8)(tb_hdr_ptr->tb_header[2] & 0x0f);  \
\
    if (DL_CT_FLD_IS_INVALID(int_fach_ptr->num_dlc,ct_field)) { \
      MSG_HIGH ("TB%d with invalid C/T %x", tb_num, ct_field, 0); \
    } \
    else {  \
      /* Pointer to internal DCCH/DTCH config info */ \
      dlcptr = &int_fach_ptr->dlcinfo[ct_field];  \
\
      /* DCCH/DTCH is enabled */  \
      if (dlcptr->enable == TRUE) { \
\
        /* RLC PDU Offset */  \
        rlc_pdu_offset = 24;  \
\
        /* RLC ID */  \
        rlc_id = dlcptr->rlc_id;  \
\
        valid = TRUE; \
      } \
      /* DCCH/DTCH not enabled */ \
      else {  \
        MSG_HIGH ("TB%d with disabled DCCH/DTCH",tb_num, ct_field, 0); \
      } \
    } \
  }
  
#else /* __GNUC__*/

/*============================================================================================
MACRO NAME    PROCESS_BCCH_TB_ON_FACH

DESCRIPTION   This macro processes header info of FACH TB that is mapped to BCCH
============================================================================================*/
#define   PROCESS_BCCH_TB_ON_FACH(tb_num)\
\
  /* If BCCH is enabled, set L1 parameters */ \
  if (int_fach_ptr->bcch_enable == TRUE) {  \
\
    /* RLC PDU Offset */  \
    rlc_pdu_offset = 2; \
\
    /* RLC ID */  \
    rlc_id = int_fach_ptr->bcch_id; \
\
    valid = TRUE; \
  } \
    /* BCCH is not enabled */ \
    else {  \
    MSG_HIGH ("TB%d BCCH not enabled",##tb_num##, 0, 0); \
    }

/*============================================================================================
MACRO NAME    PROCESS_CCCH_TB_ON_FACH

DESCRIPTION   This macro processes header info of FACH TB that is mapped to CCCH
============================================================================================*/
#define   PROCESS_CCCH_TB_ON_FACH(tb_num)\
\
  /* If CCCH is anabled, set L1 parameters */ \
    if (int_fach_ptr->ccch_enable == TRUE) {  \
\
    /* RLC PDU Offset */  \
    rlc_pdu_offset = 8; \
\
    /* RLC ID */  \
    rlc_id = int_fach_ptr->ccch_id; \
\
    valid = TRUE; \
  } \
    /* CCCH not enabled */  \
    else {  \
    MSG_HIGH ("TB%d CCCH not enabled",##tb_num##, 0, 0); \
  }

/*============================================================================================
MACRO NAME    PROCESS_CTCH_TB_ON_FACH

DESCRIPTION   This macro processes header info of FACH TB that is mapped to CTCH
============================================================================================*/
#define   PROCESS_CTCH_TB_ON_FACH(tb_num)\
\
  /* If CTCH is anabled, set L1 parameters */ \
    if (int_fach_ptr->ctch_enable == TRUE) {  \
\
    /* RLC PDU Offset */  \
    rlc_pdu_offset = 8; \
\
    /* RLC ID */  \
    rlc_id = int_fach_ptr->ctch_id; \
\
    valid = TRUE; \
  } \
  /* CTCH not enabled */  \
    else {  \
    MSG_HIGH ("TB%d CTCH not enabled", ##tb_num##, 0, 0);  \
    }

/*============================================================================================
MACRO NAME    PROCESS_DCCH_DTCH_TB_WITH_URNTI_ON_FACH

DESCRIPTION   This macro processes header info of FACH TB that is mapped to DCCH/DTCH with
        URNTI
============================================================================================*/
#define   PROCESS_DCCH_DTCH_TB_WITH_URNTI_ON_FACH(tb_num)\
\
  /* URNTI field of TB */ \
  urnti = (tb_hdr_ptr->tb_header[0] << 28) + (tb_hdr_ptr->tb_header[1] << 20) + \
      (tb_hdr_ptr->tb_header[2] << 12) + (tb_hdr_ptr->tb_header[3] << 4) +  \
      (tb_hdr_ptr->tb_header[4] >> 4);  \
\
  /* DL TB URNTI is invalid */  \
    if (DL_TB_URNTI_IS_INVALID(urnti)) {  \
    MSG_HIGH ("TB%d with invalid URNTI %x",##tb_num##, urnti, 0);  \
  } \
\
  /* FACH has one DCCH/DTCH */  \
  else if (int_fach_ptr->num_dlc == 1){ \
\
    /* Pointer to internal DCCH/DTCH config info */ \
    dlcptr = &int_fach_ptr->dlcinfo[0]; \
\
    /* DCCH/DTCH is enabled */  \
    if (dlcptr->enable == TRUE) { \
\
      /* RLC PDU Offset */  \
      rlc_pdu_offset = 36;  \
\
      /* RLC ID */  \
      rlc_id = dlcptr->rlc_id;  \
\
      valid = TRUE; \
    } \
    /* DCCH/DTCH not enabled */ \
    else {  \
      MSG_HIGH ("TB%d with disabled DCCH/DTCH", ##tb_num##, 0, 0); \
    } \
  } \
  /* FACH has multiple DCCH/DTCHs */  \
  else if (int_fach_ptr->num_dlc > 1) { \
\
    /* C/T field of TB */ \
    ct_field = (uint8)(tb_hdr_ptr->tb_header[4] & 0x0f);  \
\
    if (DL_CT_FLD_IS_INVALID(int_fach_ptr->num_dlc,ct_field)) { \
      MSG_HIGH ("TB%d with invalid C/T %x", ##tb_num##, ct_field, 0); \
    } \
    else {  \
      /* Pointer to internal DCCH/DTCH config info */ \
      dlcptr = &int_fach_ptr->dlcinfo[ct_field];  \
\
      /* DCCH/DTCH is enabled */  \
      if (dlcptr->enable == TRUE) { \
\
        /* RLC PDU Offset */  \
        rlc_pdu_offset = 40;  \
\
        /* RLC ID */  \
        rlc_id = dlcptr->rlc_id;  \
\
        valid = TRUE; \
      } \
      /* DCCH/DTCH not enabled */ \
      else {  \
        MSG_HIGH ("TB%d with disabled DCCH/DTCH",##tb_num##, ct_field, 0); \
      } \
    } \
  }

/*============================================================================================
MACRO NAME    PROCESS_DCCH_DTCH_TB_WITH_CRNTI_ON_FACH

DESCRIPTION   This macro processes header info of FACH TB that is mapped to DCCH/DTCH with
        CRNTI
============================================================================================*/
#define   PROCESS_DCCH_DTCH_TB_WITH_CRNTI_ON_FACH(tb_num)\
\
  /* CRNTI of TB */ \
  crnti = (uint16)((tb_hdr_ptr->tb_header[0] << 12) + (tb_hdr_ptr->tb_header[1] <<  4) +  \
      (tb_hdr_ptr->tb_header[2] >>  4));  \
\
  /* If C-RNTI is valid, check the C/T field */ \
    if (DL_TB_CRNTI_IS_INVALID(crnti)) {  \
    MSG_HIGH ("TB%d with invalid CRNTI %x",##tb_num##, crnti, 0); \
  } \
  /* FACH has single DCCH/DTCH */ \
  else if (int_fach_ptr->num_dlc == 1){ \
\
    /* Pointer to internal DCCH/DTCH config info */ \
    dlcptr = &int_fach_ptr->dlcinfo[0]; \
\
    /* DCCH/DTCH is enabled */  \
    if (dlcptr->enable == TRUE) { \
\
      /* RLC PDU Offset */  \
      rlc_pdu_offset = 20;  \
\
      /* RLC ID */  \
      rlc_id = dlcptr->rlc_id;  \
\
      valid = TRUE; \
    } \
    /* DCCH/DTCH not enabled */ \
    else {  \
      MSG_HIGH ("TB%d with disabled DCCH/DTCH", ##tb_num##, 0, 0); \
    } \
\
  } \
  /* FACH has multiple DCCH/DTCHs */  \
  else if (int_fach_ptr->num_dlc >1){ \
    /* C/T field of TB */ \
    ct_field = (uint8)(tb_hdr_ptr->tb_header[2] & 0x0f);  \
\
    if (DL_CT_FLD_IS_INVALID(int_fach_ptr->num_dlc,ct_field)) { \
      MSG_HIGH ("TB%d with invalid C/T %x", ##tb_num##, ct_field, 0); \
    } \
    else {  \
      /* Pointer to internal DCCH/DTCH config info */ \
      dlcptr = &int_fach_ptr->dlcinfo[ct_field];  \
\
      /* DCCH/DTCH is enabled */  \
      if (dlcptr->enable == TRUE) { \
\
        /* RLC PDU Offset */  \
        rlc_pdu_offset = 24;  \
\
        /* RLC ID */  \
        rlc_id = dlcptr->rlc_id;  \
\
        valid = TRUE; \
      } \
      /* DCCH/DTCH not enabled */ \
      else {  \
        MSG_HIGH ("TB%d with disabled DCCH/DTCH",##tb_num##, ct_field, 0); \
      } \
    } \
  }

#endif /* __GNUC__ */  

/*======================================================================================================
FUNCTION NAME   process_fach_tbs_hdr

DESCRIPTION   This function processes FACH TBS Header information
======================================================================================================*/
void  process_fach_tbs_hdr
  (
/* First TB ID */
  uint8 tb_id,

/* Pointer to internal FACH config info */
  fach_config_info_type   *int_fach_ptr,

/* Pointer to FACH TBS Header Info */
  l1_dl_tb_set_hdr_struct_type  *tbs_hdr_ptr
  )
{
  uint8       i, num_tb, first_hdr_byte, ct_field, rlc_pdu_offset = 0;
  boolean       valid, status = TRUE;
  uint16        crnti;
  uint32        urnti;
  rlc_lc_id_type    rlc_id = 0;

  /* Pointer to internal DCCH/DTCH config info */
  dlc_fach_config_info_type *dlcptr = NULL;

  /* Pointer to DL TB info */
  dl_tb_info_type       *dl_tb_info_ptr;

  /* RLC Header Info */
  rlc_dl_tb_hdr_info_type   rlc_hdr_info;

  /* RLC Ciphering Info */
  rlc_dl_tb_cipher_info_type  rlc_cipher_info;

  /* Pointer to TB Ciphering Parameters */
  l1_dl_ciphering_param_struct_type *tb_cipher_ptr;

  /* Pointer to Transport block header information */
  l1_dl_tb_hdr_struct_type      *tb_hdr_ptr;

    if (tbs_hdr_ptr==NULL)
    {
        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TBS_HDR_PTR NULL\n");
        return;
    }

  /* Number of Transport Blocks */
  num_tb = tbs_hdr_ptr->num_tb;

  /* Process each transport block */
  for (i=0 ; i <num_tb; i++)
  {

    /* Set tb_in_tm_mode to FALSE */
    tbs_hdr_ptr->tb_in_tm_mode = FALSE;

    /* Get pointer to transport block information */
    tb_hdr_ptr = &tbs_hdr_ptr->tb_hdr_info[i];

    /* TB Header pointer is NULL */
    if (tb_hdr_ptr == NULL) /*lint !e774*/
    {
      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "TB%d header pointer is NULL",
                                                  i);
    }
    else
    {

      /* Initially mark the TB config parameters as invalid */
      valid = FALSE;

      /* Initially mark the ciperhing control flag to FALSE */
      tb_hdr_ptr->ciphering_param.ciphering_control = FALSE;

      tb_hdr_ptr->ciphering_param.ciphering_algo = UE_WCDMA_CIPHER_ALGO_UEA0;

      /* Get first byte of header to decide logical channel type */
      first_hdr_byte = tb_hdr_ptr->tb_header[0];

      /* If two MSBits are 00, then it is BCCH */
      if (int_fach_ptr->bcch_enable && ((first_hdr_byte & 0xc0) == 0))
      {
        PROCESS_BCCH_TB_ON_FACH(i)
      }
      /* If first byte is 0x40, then it is CCCH */
      else if (int_fach_ptr->ccch_enable && (first_hdr_byte == 0x40))
      {
        PROCESS_CCCH_TB_ON_FACH(i)
      }
      /* If first byte is 0x80, then it is CTCH */
      else if (int_fach_ptr->ctch_enable && (first_hdr_byte == 0x80))
      {
        PROCESS_CTCH_TB_ON_FACH(i)
      }
      /* if four MSBits are 1100, then it is DCCH/DTCH with U-RNTI */
      else if (int_fach_ptr->dlc_enable && ((first_hdr_byte & 0xf0) == 0xc0))
      {
        PROCESS_DCCH_DTCH_TB_WITH_URNTI_ON_FACH(i)
      }
      /* if four MSBits are 1101, it is DCCH/DTCH with C-RNTI */
      else if (int_fach_ptr->dlc_enable && ((first_hdr_byte & 0xf0) == 0xd0))
      {
        PROCESS_DCCH_DTCH_TB_WITH_CRNTI_ON_FACH(i)
        MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "DCCH TB valid %d off %d rlc ID %d",
                                                   valid,
                                                   rlc_pdu_offset,
                                                   rlc_id);
      }

      /* TB parameters are valid */
      if (valid == TRUE)
      {
        /* Get RLC Header info */
        rlc_hdr_info.rlc_id =rlc_id;
        rlc_hdr_info.rlc_pdu_start_offset = rlc_pdu_offset;
        rlc_hdr_info.hdr_ptr = (uint8 *)&tb_hdr_ptr->tb_header[0];
        /*Decode the RLC PDU header*/
        if ((status = rlc_dl_decode_pdu_hdr (&rlc_hdr_info, &rlc_cipher_info)) == TRUE)
        {
          /* Set Ciphering parameters of TB */
          tb_cipher_ptr = &tb_hdr_ptr->ciphering_param;
          tb_cipher_ptr->ciphering_control = FALSE;

          if (rlc_cipher_info.cipher_flag)
          {
            tb_cipher_ptr->ciphering_control = TRUE;

             tb_cipher_ptr->ciphering_algo = rlc_cipher_info.ciphering_algo;
            
            tb_cipher_ptr->ciphering_bit_offset = rlc_cipher_info.cipher_offset;
            tb_cipher_ptr->ciphering_key_id = (uint8)rlc_cipher_info.cipher_key_id;
            tb_cipher_ptr->ciphering_count = rlc_cipher_info.count_c;
            tb_cipher_ptr->ciphering_radio_bearerid = (uint8)rlc_cipher_info.rb_id;
          }

          /* Set other header parameters of TB */
          tb_hdr_ptr->tb_id = (uint8)(tb_id + i);
          tb_hdr_ptr->valid = TRUE;
          tb_hdr_ptr->num_bit_offset_trblk = rlc_pdu_offset;
          tb_hdr_ptr->discard_l2_pdu = rlc_hdr_info.discard_l2_pdu;
        }
        else
        {
          /*If the header decoding fails the mark the TB as invalid*/
          tb_hdr_ptr->valid = FALSE;          
        }
  
        /* Save RLC ID in Transport Block info table */
        dl_tb_info_ptr = GET_DL_TB_INFO_PTR(tb_id + i);
        if (status == TRUE)
        {
          dl_tb_info_ptr->valid = TRUE;
          dl_tb_info_ptr->rlc_id = rlc_id;
        }
        else
        {
          dl_tb_info_ptr->valid = FALSE;
        }
      }
    }
  }
}

/*============================================================================================
FUNCTION NAME   process_fach_config

DESCRIPTION   This function processes FACH configuration data

============================================================================================*/
void  process_fach_config
(
  /* Number of FACHs */
  uint8  num_fachs,

  /* Pointer to ordered FACH configuration data list */
  mac_dl_fach_trch_config_type  *oc_fach_ptr_list[]
)
{
  uint8   i;

  /* ID to Index Table Pointer */
  trch_id_to_idx_map_type     *id_to_idx_ptr;

  /* Pointer to FACH TrCh config info */
  mac_dl_fach_trch_config_type  *oc_fach_ptr;

  /* Pointer to config info in FACH CONFIG table */
  fach_config_info_type     *int_fach_ptr;

  /* Set number of active FACHs to zero */
  num_active_fachs = 0;

  for (i=0;i<2*MAX_TRCH;i++)
  {
    fach_id_to_idx[i].valid = FALSE;
  }

  /* Invalid number of FACHs */
  if (num_fachs > UE_MAX_TRCH)
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "invalid number of FACHs %d",
                                                num_fachs);
  }
  /* Process each FACH config data */
  else
  {
    for (i=0; i < num_fachs; i++)
    {
      /* Pointer to the FACH CCTrCh info */
      oc_fach_ptr = oc_fach_ptr_list[i];

      /* FACH ordered config data pointer is NULL */
      if (oc_fach_ptr == NULL)
      {
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "FACH%d config pointer is NULL",
                                                    i);
      }
      /* Invalid TrCh ID */
      else if (DL_TRCH_ID_IS_INVALID(oc_fach_ptr->trch_id))
      {
        MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "FACH%d with invalid TrCh ID %d",
                                                    i, oc_fach_ptr->trch_id);
      }
      /* Invalid CCTrCh ID */
      else if (DL_CCTRCH_ID_IS_INVALID(oc_fach_ptr->cctrch_id))
      {
        MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "FACH%d with invalid CCTrCh ID",
                                                    i, oc_fach_ptr->ctch_id);
      }
      /* Process FACH config data */
      else
      {
        /* Pointer to the FACH config table entry */
        int_fach_ptr = &fach_cfg_info[num_active_fachs];

        /* Get Transport ID to Index map Table */
        id_to_idx_ptr = FACH_ID_TO_IDX_PTR(oc_fach_ptr->trch_id);
        id_to_idx_ptr->valid = TRUE;
        id_to_idx_ptr->idx = num_active_fachs;

        /* Save CCTrCh ID in internal FACH Config Table */
        int_fach_ptr->cctrch_id = oc_fach_ptr->cctrch_id;
        int_fach_ptr->trch_id = oc_fach_ptr->trch_id;

        MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "FACH%d Config Data : TrChID=%d CCTrChID=%d",
                                                   i,
                                                   int_fach_ptr->trch_id,
                                                   int_fach_ptr->cctrch_id);

        /* Process BCCH data */
        PROCESS_BCCH_FACH_CONFIG
          MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "FACH/BCCH id = %d",
                                                     int_fach_ptr->bcch_id);

        /* Process CCCH data if any */
        PROCESS_CCCH_FACH_CONFIG
          MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "FACH/CCCH id = %d",
                                                     int_fach_ptr->ccch_id);

        /* Process CTCH data if any */
        PROCESS_CTCH_FACH_CONFIG
          MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "FACH/CTCH id = %d",
                                                     int_fach_ptr->ctch_id);


        /* Process DCCH/DTCH data */
        process_dcch_dtch_fach_config (oc_fach_ptr, int_fach_ptr);

        /* Increment the number of active FACHs */
        num_active_fachs++;
      }

    }
  }
}

/*===============================================================================================
FUNCTION NAME mac_dl_check_fach_trch

DESCRIPTION   This function checks if the input rlc id and rb id pair map to a FACH transport channel
===============================================================================================*/

log_wcdma_trch_type mac_dl_check_fach_trch(rlc_lc_id_type rlc_id, rb_id_type rb_id)
{
  log_wcdma_trch_type return_trch_val = WCDMA_TRCH_INVALID;
  int i, j;

  for(i = 0; i < num_active_fachs; i++) 
  {

    do 
    {
      /* Case when BCCH is mapped on to FACH */
      if((fach_cfg_info[i].bcch_enable == TRUE) &&
          (fach_cfg_info[i].bcch_id == rlc_id))
      {
          return_trch_val = WCDMA_TRCH_FACH;
          break;        
      }
      /* Case when CCCH is mapped on to FACH */
      if((fach_cfg_info[i].ccch_enable == TRUE) &&
          (fach_cfg_info[i].ccch_id == rlc_id))
      {
          return_trch_val = WCDMA_TRCH_FACH;
          break;        
      }
      /* Case when CTCH is mapped on to FACH */
      if((fach_cfg_info[i].ctch_enable == TRUE) &&
          (fach_cfg_info[i].ctch_id == rlc_id))
      {
          return_trch_val = WCDMA_TRCH_FACH;
          break;        
      }           
    } while (0);

    if(return_trch_val != WCDMA_TRCH_INVALID)
    {
      break;
    }

      /* Case when DCCH/DTCH is mapped on to FACH */
    for(j = 0; j < fach_cfg_info[i].num_dlc; j++)
    {
      if((fach_cfg_info[i].dlcinfo[j].enable == TRUE) && 
         (fach_cfg_info[i].dlcinfo[j].rlc_id == rlc_id))
       {
          return_trch_val = WCDMA_TRCH_FACH;
          break;
       }
    }
  }

  return return_trch_val;
}
