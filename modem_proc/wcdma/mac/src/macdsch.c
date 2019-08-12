/*==============================================================================================

              D S C H    H A N D L E R


GENERAL DESCRIPTION

  This module contains functions required to
  - process DSCH configuration data
  - process header information of DSCH Transport Block Set

Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2002, 2005, 2008-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

==============================================================================================*/

/*==============================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macdsch.c_v   1.3   07 Aug 2001 18:28:20   psuvarna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/macdsch.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------------------------
10/17/14   ts      Fix to discard DL PDUs received while in the Reset Pending State, to avoid potential ciphering mismatch right after the recovery
07/26/14   rkmk    Reduce image size by using right macro depending upon number of parameters
01/02/12   grk     Feature cleanup
06/02/11   kc      Added support for logical channel to transport channel mapping info
06/14/10   grk     Fixed compilation errors/warnings.
05/07/09   ssg     Updated Copyright Information
04/29/09   av      Fixed compiler warnings in L2 
10/17/08   grk     Merged taxis changes to mainline.
05/19/08   kvk     Now in DSCH hdr read event check the status of rlc_dl_decode_pdu_hdr() 
                   and mark the valid flag  in TB hdr ptr to TRUE/FALSE accordingly
07/21/05   gsc     Fixed some more lint errors.
03/09/05   sk      Fixed all lint errors.
09/26/02   sk      Added rlc_cipher_info under FEATURE_UMTS_CIPHERING_ENABLE
08/07/01   ps      Fixed the leading zero message format problem
06/18/01   ps      Adjusted MSG levels
04/04/01   ps      Initial release after merging "macdschcfg.c" and "macdschhdr.c"

==============================================================================================*/

/*==============================================================================================
                     INCLUDE FILES
===============================================================================================*/
#include  "customer.h"
#include  "rex.h"
#include  "msg.h"
#include  "macrrcif_v.h"
#include  "l2dltask.h"
#include  "macinternal.h"
#include  "macdl.h"
#include  "macdsch.h"

/*==============================================================================================
                         DATA DECLARATIONS
===============================================================================================*/
/* Number of active DSCHs */
uint8 num_active_dschs;



/*==================================================================================================
FUNCTION NAME   process_dcch_dtch_dsch_config

DESCRIPTION   This function processes config info of DCCH/DTCH maaped to a DSCH
==================================================================================================*/
void  process_dcch_dtch_dsch_config
  (
/* Ordered DSCH config data pointer */
  mac_dl_dsch_trch_config_type  *oc_dsch_ptr,

/* Internal DSCH Config data pointer */
  dsch_config_info_type     *int_dsch_ptr
  )
{
  boolean valid = FALSE;
  uint8 i;
  uint8 tbl_index = 0;

  /* Pointer to ordered DCCH/DTCH config data */
  mac_dl_ded_logch_config_type    *oc_dlc_ptr;

  /* Number of DCCH/DTCHs */
  int_dsch_ptr->num_dlc = oc_dsch_ptr->ndlchan;

  /* Set Dedicated Logical Channel Enable flag to FALSE and mark all dedicated logical
     channels as disabled */
  for (i=0; i < MAX_DED_LOGCHAN_PER_TRCH; i++)
    int_dsch_ptr->dlcinfo[i].enable = FALSE;

  /* Process each DCCH/DTCH config information */
  for (i=0 ; i < oc_dsch_ptr->ndlchan; i++)
  {

    valid = FALSE;

    /* Pointer to the ordered DCCH/DTCH config data */
    oc_dlc_ptr = &oc_dsch_ptr->dlchan_info[i];

    /* Invalid RLC ID */
    if (DL_RLC_ID_IS_INVALID(oc_dlc_ptr->rlc_id))
    {
      MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCCH/DTCH%d with invalid RLC ID %d",
                                                  i, oc_dlc_ptr->rlc_id);
    }
    /* RLC Mode is invalid */
    else if (DL_RLC_MODE_IS_INVALID(oc_dlc_ptr->rlc_mode))
    {
      MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCCH/DTCH%d with invalid RLC Mode %d",
                                                  i, oc_dlc_ptr->rlc_mode);
      valid = FALSE;
    }
    /* RB ID is invalid */
    else if (DL_RB_ID_IS_INVALID(oc_dlc_ptr->rb_id))
    {
      MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCCH/DTCH%d with invalid RB ID",
                                                  i, oc_dlc_ptr->rb_id);
      valid = FALSE;
    }
    /* Only one DCCH/DTCH is mapped to DSCH */
    else if (int_dsch_ptr->num_dlc == 1)
    {
      tbl_index = 0;
      valid = TRUE;
    }
    /* Multiple DCCH/DTCHs are mapped to DSCH */
    else
    {
      /* MAC ID is invalid */
      if (DL_MAC_ID_IS_INVALID(oc_dlc_ptr->mac_id))
      {
        MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DCCH/DTCH%d with invalid MAC ID %d",
                                                    i, oc_dlc_ptr->mac_id);
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
      /* Save DCCH/DTCH info in DSCH CONFIG table */
      int_dsch_ptr->dlcinfo[tbl_index].enable = TRUE;
      int_dsch_ptr->dlcinfo[tbl_index].rlc_id = oc_dlc_ptr->rlc_id;
      int_dsch_ptr->dlcinfo[tbl_index].rb_id  = oc_dlc_ptr->rb_id;
      int_dsch_ptr->dlcinfo[tbl_index].rlc_mode = oc_dlc_ptr->rlc_mode;

      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "DL DCH/DCCH or DTCH ID = %d",
                                                 oc_dlc_ptr->rlc_id);
    }
  }
}

/*============================================================================================

FUNCTION NAME   process_dsch_config

DESCRIPTION   This function processes DSCH ordered configuration data

============================================================================================*/
void  process_dsch_config
  (
/* Number of DSCHs */
  uint8  num_dschs,

/* Pointer to ordered DSCH configuration data list */
  mac_dl_dsch_trch_config_type  *oc_dsch_ptr_list[])
{
  uint8   i;

  /* ID to Index Table Pointer */
  trch_id_to_idx_map_type       *id_to_idx_ptr;

  /* Pointer to DSCH TrCh config info */
  mac_dl_dsch_trch_config_type    *oc_dsch_ptr;

  /* Pointer to internal DSCH config info */
  dsch_config_info_type       *int_dsch_ptr;

  /* Set number of active DSCHs to zero */
  num_active_dschs = 0;

  /* Initialize DSCH ID to Index Mapping Table */
  for (i=0; i<MAX_TRCH; i++)
  {
    dsch_id_to_idx[i].valid = FALSE;
  }

  /* Invalid number of DSCHs */
  if (num_dschs > UE_MAX_TRCH)
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "invalid number of DSCHs %d",
                                                num_dschs);
  }
  /* Process DSCH config data */
  else
  {
    for (i=0; i < num_dschs; i++)
    {

      /* Pointer to DSCH CCTrCh info */
      oc_dsch_ptr = oc_dsch_ptr_list[i];

      /* DSCH ordered config data pointer is NULL */
      if (oc_dsch_ptr == NULL)
      {
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DSCH%d config pointer is NULL",
                                                    i);
      }
      /* Invalid TrCh ID */
      else if (DL_TRCH_ID_IS_INVALID(oc_dsch_ptr->trch_id))
      {
        MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "DSCH%d with invalid TrCh ID %d",
                                                    i, oc_dsch_ptr->trch_id);
      }
      /* Process DSCH config data */
      else
      {
        /* Pointer to DSCH config table entry */
        int_dsch_ptr = &dsch_cfg_info[num_active_dschs];

        /* Get Transport ID to Index map Table */
        id_to_idx_ptr = DSCH_ID_TO_IDX_PTR(oc_dsch_ptr->trch_id);
        id_to_idx_ptr->valid = TRUE;
        id_to_idx_ptr->idx = num_active_dschs;

        /* Save CCTrCh ID in internal DSCH Config Table */
        int_dsch_ptr->cctrch_id = oc_dsch_ptr->cctrch_id;

        MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "DSCH%d Config Data:TrChID=%d CCTrChID=%d",
                                                   i,
                                                   int_dsch_ptr->trch_id,
                                                   int_dsch_ptr->cctrch_id);

        /* Process DCCH/DTCH data */
        process_dcch_dtch_dsch_config (oc_dsch_ptr, int_dsch_ptr);

        /* Increment the number of active DSCHs */
        num_active_dschs++;
      }
    }
  }
}

#ifdef __GNUC__
/*==========================================================================================
MACRO NAME    PROCESS_DCCH_DTCH_TB_WITH_URNTI_ON_DSCH

DESCRIPTION   This macro processes header info of DSCH TB that is mapped to DSCH with
        URNTI
==========================================================================================*/
#define   PROCESS_DCCH_DTCH_TB_WITH_URNTI_ON_DSCH(tb_num)\
\
  /* If dedicated logical channels are mapped to this transport channel */  \
  urnti = (tb_hdr_ptr->tb_header[0] << 26) + (tb_hdr_ptr->tb_header[1] << 18) + \
      (tb_hdr_ptr->tb_header[2] << 10) + (tb_hdr_ptr->tb_header[3] << 2) +  \
      (tb_hdr_ptr->tb_header[4] >> 6);  \
\
  /* If U-RNTI is valid, check C/T field */ \
    if (DL_TB_URNTI_IS_INVALID(urnti)) {  \
    MSG_HIGH ("TB%d with invalid URNTI %x",tb_num, urnti, 0); \
  } \
  /* Only one DCCH/DTCH is mapped to DSCH */  \
  else if (int_dsch_ptr->num_dlc == 1) {  \
\
    /* Pointer to internal DCCH/DTCH config info */ \
        dlcptr = &int_dsch_ptr->dlcinfo[0]; \
\
    /* DCCH/DTCH is enabled */  \
        if (dlcptr->enable == TRUE) { \
\
      /* RLC PDU Offset */  \
      rlc_pdu_offset = 34;  \
\
      valid = TRUE; \
    } \
    /* DCCH/DTCH is not enabled */  \
    else {  \
      MSG_HIGH ("TB%d:DCCH/DTCH not enabled",tb_id, 0, 0); \
    } \
  } \
  /* Multiple DCCH/DTCHs are mapped to DSCH */  \
  else if (int_dsch_ptr->num_dlc > 1){  \
\
    /* C/T field value of TB header */  \
    ct_field = (uint8)((tb_hdr_ptr->tb_header[4] >> 2) & 0x0f); \
\
    /* C/T field is invalid */  \
      if (DL_CT_FLD_IS_INVALID(int_dsch_ptr->num_dlc,ct_field)){  \
      MSG_HIGH ("TB%d with invalid C/T %d", tb_num, ct_field, 0); \
    } \
    /* Valid C/T field */ \
    else {  \
      /* Pointer to internal DCCH/DTCH config info */ \
      dlcptr = &int_dsch_ptr->dlcinfo[ct_field];  \
\
          /* DCCH/DTCH is enabled */  \
      if (dlcptr->enable == TRUE) { \
\
        /* RLC PDU offset */  \
        rlc_pdu_offset = 38;  \
\
        valid = TRUE; \
      } \
      /* DCCH/DTCH not enabled */ \
      else {  \
          MSG_HIGH ("TB%d:DCCH/DTCH not enabled",tb_id, 0, 0); \
      } \
    } \
  }

/*==========================================================================================
MACRO NAME    PROCESS_DCCH_DTCH_TB_WITH_CRNTI_ON_DSCH

DESCRIPTION   This macro processes header info of DSCH TB that is mapped to DSCH with
        CRNTI
==========================================================================================*/
#define   PROCESS_DCCH_DTCH_TB_WITH_CRNTI_ON_DSCH(tb_num)\
\
  /* C-RNTI value */  \
  crnti = (uint16)((tb_hdr_ptr->tb_header[0] << 10) + (tb_hdr_ptr->tb_header[1] << 2) + \
           (tb_hdr_ptr->tb_header[2] >> 6));  \
\
  /* C-RNTI is valid */ \
    if (DL_TB_CRNTI_IS_INVALID(crnti)) {  \
    MSG_HIGH ("TB%d with invalid CRNTI %x",tb_num, crnti, 0); \
  } \
  /* Only one DCCH/DTCH is mapped to DSCH */  \
  else if (int_dsch_ptr->num_dlc == 1) {  \
\
    /* DCCH/DTCH config info pointer */ \
    dlcptr = &int_dsch_ptr->dlcinfo[0]; \
\
    /* DCCH/DTCH is enabled */  \
    if (dlcptr->enable == TRUE) { \
\
      /* RLC PDU offset */  \
      rlc_pdu_offset = 18;  \
\
      valid = TRUE; \
    } \
    /* DCCH/DTCH is not enabled */  \
    else {  \
      MSG_HIGH ("DCCH/DTCH of TB%d not enabled",tb_num, 0, 0); \
    } \
    } \
  /* Multiple DCCH/DTCHs are mapped to DSCH */  \
  else if (int_dsch_ptr->num_dlc > 1){  \
\
    /* C/T field value */ \
    ct_field = (uint8)((tb_hdr_ptr->tb_header[2] >> 2) & 0x0f); \
\
    /* Invalid C/T field */ \
    if (DL_CT_FLD_IS_INVALID(int_dsch_ptr->num_dlc,ct_field)){  \
      MSG_HIGH ("TB%d with invalid C/T %x", tb_num, ct_field, 0); \
    } \
    else {  \
      /* Pointer to internal DCCH/DTCH config data */ \
      dlcptr = &int_dsch_ptr->dlcinfo[ct_field];  \
\
          /* DCCH/DTCH is enabled */  \
      if (dlcptr->enable == TRUE) { \
\
        /* RLC PDU offset */  \
        rlc_pdu_offset = 22;  \
\
        valid = TRUE; \
      } \
      /* DCCH/DTCH not enabled */ \
      else {  \
        MSG_HIGH ("DCCH/DTCH of TB%d not enabled",tb_num, 0, 0);  \
      } \
    } \
  }

#else /* __GNUC__*/
/*==========================================================================================
MACRO NAME    PROCESS_DCCH_DTCH_TB_WITH_URNTI_ON_DSCH

DESCRIPTION   This macro processes header info of DSCH TB that is mapped to DSCH with
        URNTI
==========================================================================================*/
#define   PROCESS_DCCH_DTCH_TB_WITH_URNTI_ON_DSCH(tb_num)\
\
  /* If dedicated logical channels are mapped to this transport channel */  \
  urnti = (tb_hdr_ptr->tb_header[0] << 26) + (tb_hdr_ptr->tb_header[1] << 18) + \
      (tb_hdr_ptr->tb_header[2] << 10) + (tb_hdr_ptr->tb_header[3] << 2) +  \
      (tb_hdr_ptr->tb_header[4] >> 6);  \
\
  /* If U-RNTI is valid, check C/T field */ \
    if (DL_TB_URNTI_IS_INVALID(urnti)) {  \
    MSG_HIGH ("TB%d with invalid URNTI %x",##tb_num##, urnti, 0); \
  } \
  /* Only one DCCH/DTCH is mapped to DSCH */  \
  else if (int_dsch_ptr->num_dlc == 1) {  \
\
    /* Pointer to internal DCCH/DTCH config info */ \
        dlcptr = &int_dsch_ptr->dlcinfo[0]; \
\
    /* DCCH/DTCH is enabled */  \
        if (dlcptr->enable == TRUE) { \
\
      /* RLC PDU Offset */  \
      rlc_pdu_offset = 34;  \
\
      valid = TRUE; \
    } \
    /* DCCH/DTCH is not enabled */  \
    else {  \
      MSG_HIGH ("TB%d:DCCH/DTCH not enabled",tb_id, 0, 0); \
    } \
  } \
  /* Multiple DCCH/DTCHs are mapped to DSCH */  \
  else if (int_dsch_ptr->num_dlc > 1){  \
\
    /* C/T field value of TB header */  \
    ct_field = (uint8)((tb_hdr_ptr->tb_header[4] >> 2) & 0x0f); \
\
    /* C/T field is invalid */  \
      if (DL_CT_FLD_IS_INVALID(int_dsch_ptr->num_dlc,ct_field)){  \
      MSG_HIGH ("TB%d with invalid C/T %d", ##tb_num##, ct_field, 0); \
    } \
    /* Valid C/T field */ \
    else {  \
      /* Pointer to internal DCCH/DTCH config info */ \
      dlcptr = &int_dsch_ptr->dlcinfo[ct_field];  \
\
          /* DCCH/DTCH is enabled */  \
      if (dlcptr->enable == TRUE) { \
\
        /* RLC PDU offset */  \
        rlc_pdu_offset = 38;  \
\
        valid = TRUE; \
      } \
      /* DCCH/DTCH not enabled */ \
      else {  \
          MSG_HIGH ("TB%d:DCCH/DTCH not enabled",tb_id, 0, 0); \
      } \
    } \
  }

/*==========================================================================================
MACRO NAME    PROCESS_DCCH_DTCH_TB_WITH_CRNTI_ON_DSCH

DESCRIPTION   This macro processes header info of DSCH TB that is mapped to DSCH with
        CRNTI
==========================================================================================*/
#define   PROCESS_DCCH_DTCH_TB_WITH_CRNTI_ON_DSCH(tb_num)\
\
  /* C-RNTI value */  \
  crnti = (uint16)((tb_hdr_ptr->tb_header[0] << 10) + (tb_hdr_ptr->tb_header[1] << 2) + \
           (tb_hdr_ptr->tb_header[2] >> 6));  \
\
  /* C-RNTI is valid */ \
    if (DL_TB_CRNTI_IS_INVALID(crnti)) {  \
    MSG_HIGH ("TB%d with invalid CRNTI %x",##tb_num##, crnti, 0); \
  } \
  /* Only one DCCH/DTCH is mapped to DSCH */  \
  else if (int_dsch_ptr->num_dlc == 1) {  \
\
    /* DCCH/DTCH config info pointer */ \
    dlcptr = &int_dsch_ptr->dlcinfo[0]; \
\
    /* DCCH/DTCH is enabled */  \
    if (dlcptr->enable == TRUE) { \
\
      /* RLC PDU offset */  \
      rlc_pdu_offset = 18;  \
\
      valid = TRUE; \
    } \
    /* DCCH/DTCH is not enabled */  \
    else {  \
      MSG_HIGH ("DCCH/DTCH of TB%d not enabled",##tb_num##, 0, 0); \
    } \
    } \
  /* Multiple DCCH/DTCHs are mapped to DSCH */  \
  else if (int_dsch_ptr->num_dlc > 1){  \
\
    /* C/T field value */ \
    ct_field = (uint8)((tb_hdr_ptr->tb_header[2] >> 2) & 0x0f); \
\
    /* Invalid C/T field */ \
    if (DL_CT_FLD_IS_INVALID(int_dsch_ptr->num_dlc,ct_field)){  \
      MSG_HIGH ("TB%d with invalid C/T %x", ##tb_num##, ct_field, 0); \
    } \
    else {  \
      /* Pointer to internal DCCH/DTCH config data */ \
      dlcptr = &int_dsch_ptr->dlcinfo[ct_field];  \
\
          /* DCCH/DTCH is enabled */  \
      if (dlcptr->enable == TRUE) { \
\
        /* RLC PDU offset */  \
        rlc_pdu_offset = 22;  \
\
        valid = TRUE; \
      } \
      /* DCCH/DTCH not enabled */ \
      else {  \
        MSG_HIGH ("DCCH/DTCH of TB%d not enabled",##tb_num##, 0, 0);  \
      } \
    } \
  }

#endif /* __GNUC__ */  

/*====================================================================================================
FUNCTION NAME process_dsch_tbs_hdr

DESCRIPTION   This function processes header information of DSCH transport block set
====================================================================================================*/
void  process_dsch_tbs_hdr
  (
/* First DSCH TB ID */
  uint8 tb_id,

/* Pointer to internal DSCH config info table */
  dsch_config_info_type     *int_dsch_ptr,

/* Pointer to DSCH TBS Header Information */
  l1_dl_tb_set_hdr_struct_type  *tbs_hdr_ptr
  )
{
  uint8       i, num_tb, ct_field, rlc_pdu_offset = 0;
  boolean       valid,status = TRUE;
  uint32        urnti;
  uint16        crnti;

  /* Pointer internal TB info */
  dl_tb_info_type       *dl_tb_info_ptr;

  /* RLC Header Info */
  rlc_dl_tb_hdr_info_type   rlc_hdr_info;

  /* RLC Ciphering Info */
  rlc_dl_tb_cipher_info_type  rlc_cipher_info;

  /* Pointer to config info of DCCH/DTCH mapped to DSCH */
  dlc_dsch_config_info_type   *dlcptr = NULL;

  /* Pointer to Transport block header information */
  l1_dl_tb_hdr_struct_type    *tb_hdr_ptr;

    if (tbs_hdr_ptr==NULL)
    {
        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "DSCH_TBS_HDR_PTR is NULL");
        return;
    }
  /* Number of DSCH Transport Blocks */
  num_tb = tbs_hdr_ptr->num_tb;

  /* Process header info of each transport block */
  for (i=0 ; i < num_tb; i++)
  {
    /* Initially mark TB parameters as invalid */
    valid = FALSE;

    /* Get pointer to transport block information */
    tb_hdr_ptr = &tbs_hdr_ptr->tb_hdr_info[i];

    /* TB Header info pointer is NULL */
    if (tb_hdr_ptr == NULL) /*lint !e774*/
    {
      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "TB%d header info pointer is NULL",
                                                  i);
    }
    /* if two MSBits are 00, then it is DCCH/DTCH TB with U-RNTI */
    else if ((tb_hdr_ptr->tb_header[0] & 0xc0) == 0x00)
    {
      PROCESS_DCCH_DTCH_TB_WITH_URNTI_ON_DSCH(i)
    }
    /* if two MSBits are 01, then it is DCCH/DTCH TB with C-RNTI */
    else if ((tb_hdr_ptr->tb_header[0] & 0xc0) == 0x40)
    {
      PROCESS_DCCH_DTCH_TB_WITH_CRNTI_ON_DSCH(i)
    }
    /* TB with invalid RNTI header */
    else
    {
      MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "TB%d with invalid RNTI header %x",
                                                  i, tb_hdr_ptr->tb_header[0]);
    }

    /* TB header parameters are valid */
    if (valid == TRUE)
    {

      /* Get RLC PDU Offset and Ciphering Info */
      rlc_hdr_info.rlc_id = dlcptr->rlc_id; /*lint !e613*/
      rlc_hdr_info.rlc_pdu_start_offset = 0;
      rlc_hdr_info.hdr_ptr = (uint8 *)&tb_hdr_ptr->tb_header[0];

      if ((status = rlc_dl_decode_pdu_hdr (&rlc_hdr_info, &rlc_cipher_info)) == TRUE)
      {
        /* Set L1 parameters */
        tb_hdr_ptr->ciphering_param.ciphering_control = FALSE;/*lint !e613*/
        tb_hdr_ptr->tb_id = (uint8)(tb_id + i);/*lint !e613*/
        tb_hdr_ptr->valid = TRUE;/*lint !e613*/
        tb_hdr_ptr->num_bit_offset_trblk = rlc_pdu_offset;/*lint !e613*/
        tb_hdr_ptr->discard_l2_pdu = rlc_hdr_info.discard_l2_pdu;
      }
      else
      {
        tb_hdr_ptr->valid = FALSE;
      }
      /* Save RLC ID in Transport Block info table */
      dl_tb_info_ptr = GET_DL_TB_INFO_PTR(tb_id+i);
      if (status == TRUE)
      {

        dl_tb_info_ptr->valid = TRUE;
        dl_tb_info_ptr->rlc_id = dlcptr->rlc_id; /*lint !e613*/
      }
      else
      {
        dl_tb_info_ptr->valid = FALSE;
      }
    }
  }
}

/*===============================================================================================
FUNCTION NAME mac_dl_check_dsch_trch

DESCRIPTION   This function checks if given rlc-id and rb-id pair map to a DL-DCH transport channel
===============================================================================================*/
log_wcdma_trch_type mac_dl_check_dsch_trch(rlc_lc_id_type rlc_id, rb_id_type rb_id)
{
  log_wcdma_trch_type return_trch_val = WCDMA_TRCH_INVALID;
  int i, j;

  for(i = 0; i < num_active_dschs; i++)
  {
    for(j = 0; j < dsch_cfg_info[i].num_dlc; j++)
    {
      if((dsch_cfg_info[i].dlcinfo[j].enable == TRUE) && 
         (dsch_cfg_info[i].dlcinfo[j].rlc_id == rlc_id) && 
         (dsch_cfg_info[i].dlcinfo[j].rb_id == rb_id))
      {
        return_trch_val = WCDMA_TRCH_HSDSCH;
        break;
      }
    }
  }

  MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "function mac_dl_check_dsch_trch returning %d",
                                             return_trch_val);
  return return_trch_val;
}
