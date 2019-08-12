/*=============================================================================================

                                    B C H   H A N D L E R

GENERAL DESCRIPTION

  This module contains functions to :
  - process BCH configuration data
  - process header information of BCH Transport Block Set

Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2005, 2008-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

==============================================================================================*/

/*==============================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macbch.c_v   1.4   07 Aug 2001 18:27:24   psuvarna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/macbch.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when      who     what, where, why
--------  ---     ----------------------------------------------------------------------
07/26/14   rkmk    Reduce image size by using right macro depending upon number of parameters
05/29/09  ssg     Added support for SNOW_3G algo.
05/07/09  ssg     Updated Copyright Information
04/29/09  av      Fixed compiler warnings in L2
09/18/08  kvk     Ciphering Algo needs to set along with Ciphering control for PP
07/21/05  gsc     Lint fix
08/07/01  ps      Fixed the leading zero message format problem
06/18/01  ps      Adjusted MSG levels
05/02/01  ps      Changed few functions to MACROs and cleaned up the debug log code
04/04/01  ps      Initial release after merging "macbchcfg.c" and "macbchhdr.c"

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
#include  "macbch.h"


/*==============================================================================================
FUNCTION NAME   process_bch_config

PURPOSE     This function processes BCH configuration data received from RRC.

==============================================================================================*/
void  process_bch_config
(
   /* Number of BCH type CCTrChs */
   uint8    num_bchs,

   /* Pointer to the first BCH Configuration Data */
   mac_dl_bch_cctrch_config_type *oc_bch_info
)
{
   uint8   i;

   /* Pointer to BCH Config information table */
   bch_config_info_type         *int_bch_ptr;

   /* Initially mark all BCHs as disabled */
   for (i=0; i<UE_MAX_CCTRCH; i++) {
      bch_cfg_info[i].bch_enable = FALSE;
   }

   /* Invalid number of BCHs */
   if (num_bchs > MAC_DL_MAX_CCTRCH) {
      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "invalid number of BCHs %d",
                                                  num_bchs);
   }
   /* Ordered BCH Info pointer is NULL */
   else if (oc_bch_info == NULL) {
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "BCH ordered config info pointer is NULL");
   }
   /* Valid Number of BCHs */
   else {
      /* Process each BCH */
      for (i=0; i<num_bchs; i++) {

         /* CCTrCh ID is invalid */
         if (DL_CCTRCH_ID_IS_INVALID(oc_bch_info[i].cctrch_id)) {
            MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "BCH%d CCTrCh ID is invalid",
                                                        i,
                                                        oc_bch_info[i].cctrch_id);
         }
         /* BCCH ID is invalid */
         else if (DL_RLC_ID_IS_INVALID(oc_bch_info[i].bcch_id)) {
            MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "BCH%d BCCH ID %d is invalid",
                                                        i,
                                                        oc_bch_info[i].bcch_id);
         }
         /* BCH parameters are valid */
         else {
            /* Pointer to BCH config table entry */
            int_bch_ptr = GET_BCH_INFO_PTR(oc_bch_info[i].cctrch_id);

            /* Save BCH info in config info table */
            int_bch_ptr->bch_enable = TRUE;
            int_bch_ptr->bcch_id = oc_bch_info[i].bcch_id;

            MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "BCH%d: CCTrCh ID = %d BCCH ID %d",
                                                       i,
                                                       oc_bch_info[i].cctrch_id,
                                                       oc_bch_info[i].bcch_id);
         }
      }
   }
}

/*==============================================================================================

FUNCTION NAME   process_bch_tbs_hdr

PURPOSE     This function process BCH Transport Block Set Header Information

==============================================================================================*/
void  process_bch_tbs_hdr
(
  /* Transport Block ID */
  uint8 tb_id,

  /* Pointer to internal BCH info */
  bch_config_info_type    *int_bch_ptr,

  /* Pointer to Transport Block Set Header Information */
  l1_dl_tb_set_hdr_struct_type  *tbs_hdr_ptr
)
{
  uint8   i;

  /* Pointer to Transport block header information */
  l1_dl_tb_hdr_struct_type    *tb_hdr_ptr;

  /* Pointer to DL Transport Block Info */
  dl_tb_info_type       *dl_tb_info_ptr;

  /* Process each transport block header information */
  for (i=0 ; i <tbs_hdr_ptr->num_tb; i++) {

    /* Transport block header data pointer */
        tb_hdr_ptr = &tbs_hdr_ptr->tb_hdr_info[i];

    /* TB Header Info pointer is NULL */
    if (tb_hdr_ptr == NULL){ /*lint !e774*/
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "TB%d Header Info pointer is NULL");
    }
    else {
      /* Set L1 parameters */
      tb_hdr_ptr->ciphering_param.ciphering_control = FALSE;
      tb_hdr_ptr->tb_id = (uint8)(tb_id + i);
      tb_hdr_ptr->valid = TRUE;
      tb_hdr_ptr->num_bit_offset_trblk = 0;

      /* Save RLC ID in Transport Block info table */
        dl_tb_info_ptr = GET_DL_TB_INFO_PTR(tb_id + i);
      dl_tb_info_ptr->valid = TRUE;
      dl_tb_info_ptr->rlc_id = int_bch_ptr->bcch_id;

      MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "TB%d:TB ID = %d, RLC ID = %d",
                                                 i,
                                                 tb_hdr_ptr->tb_id,
                                                 int_bch_ptr->bcch_id);
    }
  }
}

