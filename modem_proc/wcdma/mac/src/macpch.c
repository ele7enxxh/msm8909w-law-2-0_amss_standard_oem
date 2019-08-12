
/*==============================================================================================

                P C H   H A N D L E R

GENERAL DESCRIPTION

  This module contains functions to :
  - process PCH configuration data
  - process PCH Transport Block Set Header information

Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2005-2006, 2008-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

==============================================================================================*/

/*==============================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macpch.c_v   1.4   07 Aug 2001 18:29:14   psuvarna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/macpch.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when      who   what, where, why
--------  ---   ----------------------------------------------------
07/26/14   rkmk    Reduce image size by using right macro depending upon number of parameters
05/29/09  ssg   Added support for SNOW_3G algo.
05/07/09  ssg   Updated Copyright Information
04/29/09  av    Fixed compiler warnings in L2 
09/18/08  kvk   Ciphering Algo needs to set along with Ciphering control for PP
03/12/06  sk    Corrected the indentation
09/21/05  ps    Fixed lint errors.
08/07/01  ps    Fixed the leading zero message format problem
06/18/01  ps    Adjusted MS levels
05/02/01  ps    Changed few functions to MACROs
04/04/01  ps    Initial release after merging "macpchcfg.c" and "macpchhdr.c"

==============================================================================================*/

/*==============================================================================================
                     INCLUDE FILES
==============================================================================================*/
#include  "customer.h"
#include  "rex.h"
#include  "msg.h"
#include  "macrrcif_v.h"
#include  "l2dltask.h"
#include  "macinternal.h"
#include  "macdl.h"
#include  "macpch.h"


/*==============================================================================================

FUNCTION NAME   process_pch_config

DESCRIPTION   This function processes PCH Configuration Data received from RRC

==============================================================================================*/
void  process_pch_config
(
  /* Number of PCHs */
  uint8   num_pchs,

  /* Pointer to ordered PCH config info */
  mac_dl_pch_cctrch_config_type *oc_pch_info
)
{
    uint8   i;

  /* Pointer to internal PCH config info */
  pch_config_info_type  *int_pch_ptr;

  /* Initially mark all PCHs as disabled */
  for (i=0; i<UE_MAX_CCTRCH; i++)
  {
    pch_cfg_info[i].pch_enable = FALSE;
  }

  /* PCH ordered config data pointer is NULL */
  if (oc_pch_info == NULL)
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "PCH%d ordered config pointer is NULL",
                                                i);
  }
  /* Invalid number of CCTrChs */
  else if (num_pchs > MAC_DL_MAX_CCTRCH)
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "invalid number of PCH Type CCTrChs",
                                                num_pchs);
  }
  /* Process each CCTrCh */
  else
  {
    for (i=0; i<num_pchs; i++)
    {
      /* Invalid CCTrCh ID */
      if (DL_CCTRCH_ID_IS_INVALID(oc_pch_info[i].cctrch_id))
      {
        MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "PCH%d with invalid CCTrCh ID",
                                                    i,
                                                    oc_pch_info[i].cctrch_id);
      }
      /* Invalid RLC ID */
      else if (DL_RLC_ID_IS_INVALID(oc_pch_info[i].pcch_id))
      {
        MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "PCH%d with invalid PCCH ID %d",
                                                    i, oc_pch_info[i].pcch_id);
      }
      /* Save PCH info in local table */
      else
      {
        /* Pointer to the PCH config table entry */
        int_pch_ptr = GET_PCH_INFO_PTR(oc_pch_info[i].cctrch_id);

        /* Save PCH info in config info table */
        int_pch_ptr->pch_enable = TRUE;
        int_pch_ptr->pcch_id = oc_pch_info[i].pcch_id;

        MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "PCH%d: CCTrCh ID=%d PCCH ID=%d",
                                                   i,
                                                   oc_pch_info[i].cctrch_id,
                                                   oc_pch_info[i].pcch_id);
      }
    }
  }
}

/*===========================================================================================
FUNCTION    process_pch_tbs_hdr

DESCRIPTION   This function processes PCH TBS Header information
        Called by Layer 1 in interrupt context
============================================================================================*/
void  process_pch_tbs_hdr
(
  /* First PCH TB ID */
  uint8 tb_id,

  /* Pointer to internal PCH info */
  pch_config_info_type      *int_pch_ptr,

  /* Pointer to PCH TBS Header Info */
  l1_dl_tb_set_hdr_struct_type  *tbs_hdr_ptr
)
{
  uint8           i;

  /* Pointer to internal DL TB Info */
  dl_tb_info_type       *dl_tb_info_ptr;

  /* Pointer to Transport block header information */
  l1_dl_tb_hdr_struct_type    *tb_hdr_ptr;

  /* Process each TB */
  for (i=0 ; i <tbs_hdr_ptr->num_tb; i++)
  {
    /* Transport block header data pointer */
    tb_hdr_ptr = &tbs_hdr_ptr->tb_hdr_info[i];

    if (tb_hdr_ptr == NULL) /* lint !e774 */
    {
      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "TB%d Header pointer is NULL",
                                                  i);
    }
    else
    {
      /* Set L1 parameters */
      tb_hdr_ptr->ciphering_param.ciphering_control = FALSE;
      tb_hdr_ptr->tb_id = (uint8)(tb_id + i);
      tb_hdr_ptr->valid = TRUE;
      tb_hdr_ptr->num_bit_offset_trblk = 0;

      /* Save RLC ID of the transport block in Transport Block info table */
      dl_tb_info_ptr = GET_DL_TB_INFO_PTR(tb_id+i);
      dl_tb_info_ptr->valid = TRUE;
      dl_tb_info_ptr->rlc_id = int_pch_ptr->pcch_id;

      MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "TB%d:TB ID=%d, RLC ID=%d",
                                                 i,
                                                 tb_hdr_ptr->tb_id,
               int_pch_ptr->pcch_id);
    }
  }
}

