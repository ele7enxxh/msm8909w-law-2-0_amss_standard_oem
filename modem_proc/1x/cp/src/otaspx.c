/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            O T A S P   T R A N S L A T I O N    S E R V I C E S

GENERAL DESCRIPTION
  This module contains the services which translate a OTASP message between
  the internal format used by the Main Control task and
  the external format used by the Transmit and Receive tasks.

EXTERNALIZED FUNCTIONS
  otaspx_int_to_ext
    Translates an OTASP message from internal to external format.

  otaspx_ext_to_int
    Translates an OTASP message from external to internal format.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 1996 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/otaspx.c_v   1.5   02 Oct 2002 22:52:36   louiel  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/otaspx.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/30/12   ssh     Removed the buggy check on n_sid_nid that was rejecting
                   NAM download request.
10/19/12   ppr     Added defensive checks
02/21/12   ppr     Feature Cleanup
02/04/12   ppr     Feature Cleanup
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
01/13/12   srk     Feature Cleanup: Mainlined Always ON features
07/12/11   ab      KW Fixes
02/09/11   ag      Mainlined FEATURE_EHRPD.
12/15/10   ag      SU level CMI clean up.
11/15/10   jtm     Initial SU API split for CP.
09/15/10   azh     Fixed KlocWork warnings.
11/02/09   jtm     Fixed compiler warnings.
10/21/09   ag      Added support for eHRPD IMSI OTASP operation.
05/14/09   ag      CMI phase 2 changes
05/11/09   jj      Fixed the issue of handling NAM blk download only for PCS
                   bands.
06/30/08   pk      Mainlined feature FEATURE_EXTENDED_PRL
03/19/07   pg      Lint Cleanup.
06/20/06   jqi     Removed the error fatal and responsed with the max allowed
                   features upon the capability request message.
06/14/06   an      Fixed RUIM Build Lint Errors
05/01/06   an      Fixed Lint Errors
07/04/05   ht      Added pre-Rev D MEID interim support.
07/04/05   ht      Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated
                   FEATURE_CDMA_800 and FEATURE_CDMA_1900
03/23/05   fc      Merged the following from MSMSHARED:
03/22/05   an       Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated
                    FEATURE_CDMA_800 and FEATURE_CDMA_1900
03/09/05   jqi      Discard the blocks over OTASPI_PARAM_BLOCK_MAX upon
                    receiving config_req_message.
11/30/04   jqi     Added FEATURE_IS683C_OTASP to complete OTASP support for
                   IS683C PRL.
07/17/03   sj      Added change to accept block ID 0 as well 1 in dload rsp msg.
10/02/02   yll     Linted the code.
08/30/02   ts      Added support for Run Time R-UIM Enable.
05/25/01   ts      Changed the RUIM feature switch to the new UIM (server)
                   feature switch.
03/13/01   fas     Removed dummy function. Removed call to MSG_LOW.
                   FEATURE_RUIM selects inclusion of err.h.
03/07/01   fas     Added call to MSG_LOW to inhibit compiler warnings.
11/28/00   ck      Added support for IS-683A with the R-UIM.
                   Some of the data burst messages are not parsed into the
                   corresponding fields if FEATURE_RUIM is defined since the
                   data is sent to the card for processing.
05/30/00   ck      Added a null function to avoid compiler warning.
04/23/99   ck      Fixed the problem where no data was sent in the config rsp
                   for Block 0 in Std OTASP for PCS targets.
04/16/99   ck      Added capability to translate multiple sid-nid pairs for
                   upload.
02/08/98   ck      Included the check to see if the pointer to roaming list is
                   NULL while uploading the roaming list.
01/21/99   ck      Added support to translate key exchange and re-authentica-
                   tion messages and IS-683A blocks.
04/18/97   dna     Translation for Cellular Indicator Data for TGP Dual Band
08/08/96   dna     Translation for Sprint subsidy lock and pr list dload
06/25/96   dna     Checked in initial version.
04/15/95   day     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"

#include <memory.h>
#include "bit.h"
#include "nv.h"
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  #include "err.h"
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "otaspi_i.h"
#include "otaspe.h"
#include "otaspx.h"
#include "prl.h"
#include "msg.h"
#include "m1x_diag.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*===========================================================================

FUNCTION XLATE_EXT_CONFIG_REQ

DESCRIPTION
  This function translates the received OTASP Configuration Request
  Message external to internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

/*lint -e818*/
word xlate_ext_config_req
(
  word otasp_len,
    /* Length of OTASP message in bytes */
  byte *otasp_ptr,
    /* Pointer to received OTASP message to be translated */
  otaspi_ftc_msg_type *int_ptr
    /* Pointer to place where translated message should be placed */
)
{
  word status;
    /* Status returned to calling procedure */
  word msg_pos;
    /* Bit position in OTASP message relative to start of otasp_ptr */
  byte i;
    /* Loop index */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize status */
  status = OTASPX_DONE_S;  /*lint !e641 */

  /* Advance buffer pointer past msg_type field */
  msg_pos = FSIZ( otaspe_config_req_fix_type, msg_type );

  /* Unpack number of parameter blocks requested */
  int_ptr->config_req.num_blocks =
    b_unpackb( otasp_ptr, msg_pos,
               FSIZ( otaspe_config_req_fix_type, num_blocks ));
  msg_pos += FSIZ( otaspe_config_req_fix_type, num_blocks );

  int_ptr->config_req.num_blocks =
    MIN( int_ptr->config_req.num_blocks, OTASPI_PARAM_BLOCK_MAX );

  /* Unpack variable number of parameter block identifiers */
  for (i=0; i < int_ptr->config_req.num_blocks; i++)
  {
    int_ptr->config_req.block_id[i] =
      b_unpackb( otasp_ptr, msg_pos,
                 FSIZ( otaspe_config_req_var_type, block_id ));
    msg_pos += FSIZ( otaspe_config_req_var_type, block_id );
  }

  /* ------------------------------------------
  ** Sanity check for OTASP Data Message length
  ** ------------------------------------------ */
  if (msg_pos > (otasp_len * 8))
  {
    status = OTASPX_INV_LEN_S; /*lint !e641 */
  }

  /* Return translation status */
  return (status);

} /* xlate_ext_config_req */

/*===========================================================================

FUNCTION XLATE_EXT_DOWNLOAD_REQ

DESCRIPTION
  This function translates the received OTASP Download Request
  Message external to internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_ext_download_req
(
  word otasp_len,
    /* Length of OTASP message in bytes */
  byte *otasp_ptr,
    /* Pointer to received OTASP message to be translated */
  otaspi_ftc_msg_type *int_ptr
    /* Pointer to place where translated message should be placed */
)
{
  word status;
    /* Status returned to calling procedure */
  word msg_pos;
    /* Bit position in OTASP message relative to start of otasp_ptr */
  word block_pos;
    /* Starting bit position of a parameter block */
  byte i;
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  byte j;
    /* Loop index */
#endif /* FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize status */
  status = OTASPX_DONE_S; /*lint !e641 */

  /* Advance buffer pointer past msg_type field */
  msg_pos = FSIZ( otaspe_download_req_fix_type, msg_type );

  /* Unpack number of parameter blocks */
  int_ptr->dload_req.num_blocks =
    b_unpackb( otasp_ptr, msg_pos,
               FSIZ( otaspe_download_req_fix_type, num_blocks ));
  msg_pos += FSIZ( otaspe_download_req_fix_type, num_blocks );

  int_ptr->dload_req.num_blocks =
    MIN( int_ptr->dload_req.num_blocks, OTASPI_PARAM_BLOCK_MAX );

  /* ----------------------------------
  ** Unpack parameter block information
  ** ---------------------------------- */

  /* Loop through each parameter block set to unpack data */
  for (i=0; i < int_ptr->dload_req.num_blocks; i++)
  {
    /* Unpack parameter block identifier */
    int_ptr->dload_req.block[i].block_id =
      b_unpackb( otasp_ptr, msg_pos,
                 FSIZ( otaspe_download_req_var_type, block_id ));
    msg_pos += FSIZ( otaspe_download_req_var_type, block_id );

    /* Unpack parameter block length */
    int_ptr->dload_req.block[i].block_len =
      b_unpackb( otasp_ptr, msg_pos,
                 FSIZ( otaspe_download_req_var_type, block_len ));
    msg_pos += FSIZ( otaspe_download_req_var_type, block_len );

    /* Save starting bit position of current parameter block */
    block_pos = msg_pos;

    /* Calculate the beginning of next parameter block */
    msg_pos += (int_ptr->dload_req.block[i].block_len * 8);

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    /* Determine if the R-UIM is available for this NAM */
    if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    {
      /* We do not want to parse the data and so copy the bits corresponding to
         each block in the ruim data buffer */

      b_copy(otasp_ptr, block_pos, int_ptr->dload_req.block[i].blk.ruim_data, 0,
             (int_ptr->dload_req.block[i].block_len * 8));

      block_pos += (int_ptr->dload_req.block[i].block_len * 8);
    }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
    {
      /* -----------------------------
      ** Decode parameter block data
      ** --------------------------- */
      switch (int_ptr->dload_req.block[i].block_id)
      {
        /*********************************************/
        /*  Cellular Indicator Data Parameter Block  */
        /*********************************************/
        case OTASP_CDMA_ANALOG_NAM_BLK:
          /* Unpack AMPS first paging channel */
          int_ptr->dload_req.block[i].blk.ftc_cell.firstchp =
            b_unpackw( otasp_ptr, block_pos,
                       FSIZ( otaspe_ftc_cell_data_fix_type, firstchp ));
          block_pos += FSIZ( otaspe_ftc_cell_data_fix_type, firstchp );

          /* Unpack AMPS home SID */
          int_ptr->dload_req.block[i].blk.ftc_cell.home_sid =
            b_unpackw( otasp_ptr, block_pos,
                       FSIZ( otaspe_ftc_cell_data_fix_type, home_sid ));
          block_pos += FSIZ( otaspe_ftc_cell_data_fix_type, home_sid );

          /* Unpack AMPS EXtended Address flag */
          int_ptr->dload_req.block[i].blk.ftc_cell.ex =
            b_unpackb( otasp_ptr, block_pos,
                       FSIZ( otaspe_ftc_cell_data_fix_type, ex ));
          block_pos += FSIZ( otaspe_ftc_cell_data_fix_type, ex );

          /* Unpack IMSI Class assignment of the mobile station */
          int_ptr->dload_req.block[i].blk.ftc_cell.imsi_class =
            b_unpackb( otasp_ptr, block_pos,
                      FSIZ( otaspe_ftc_cell_data_fix_type, imsi_class ));
          block_pos += FSIZ( otaspe_ftc_cell_data_fix_type, imsi_class );

          /* Unpack number of IMSI address digits */
          int_ptr->dload_req.block[i].blk.ftc_cell.imsi_addr_num =
            b_unpackb( otasp_ptr, block_pos,
                       FSIZ( otaspe_ftc_cell_data_fix_type, imsi_addr_num ));
          block_pos += FSIZ( otaspe_ftc_cell_data_fix_type, imsi_addr_num );

          /* Unpack mobile country code */
          int_ptr->dload_req.block[i].blk.ftc_cell.mcc =
            b_unpackw( otasp_ptr, block_pos,
                       FSIZ( otaspe_ftc_cell_data_fix_type, mcc ));
          block_pos += FSIZ( otaspe_ftc_cell_data_fix_type, mcc );

          /* Unpack 11th and 12th digits of the IMSI */
          int_ptr->dload_req.block[i].blk.ftc_cell.imsi_11_12 =
            b_unpackb( otasp_ptr, block_pos,
                       FSIZ( otaspe_ftc_cell_data_fix_type, imsi_11_12 ));
          block_pos += FSIZ( otaspe_ftc_cell_data_fix_type, imsi_11_12 );

          /* Unpack the least significant 10 digits of the IMSI */
          b_unpackq( otasp_ptr, block_pos,
                     FSIZ( otaspe_ftc_cell_data_fix_type, imsi_s ),
                     int_ptr->dload_req.block[i].blk.ftc_cell.imsi_s );
          block_pos += FSIZ( otaspe_ftc_cell_data_fix_type, imsi_s );

          /* Unpack access overload class */
          int_ptr->dload_req.block[i].blk.ftc_cell.accolc =
            b_unpackb( otasp_ptr, block_pos,
                       FSIZ( otaspe_ftc_cell_data_fix_type, accolc ));
          block_pos += FSIZ( otaspe_ftc_cell_data_fix_type, accolc );

          /* Unpack local control status */
          int_ptr->dload_req.block[i].blk.ftc_cell.local_control =
            b_unpackb( otasp_ptr, block_pos,
                       FSIZ( otaspe_ftc_cell_data_fix_type, local_control ));
          block_pos += FSIZ( otaspe_ftc_cell_data_fix_type, local_control );

          /* Unpack termination indicator for the home system */
          int_ptr->dload_req.block[i].blk.ftc_cell.mob_term_home =
            b_unpackb( otasp_ptr, block_pos,
                       FSIZ( otaspe_ftc_cell_data_fix_type, mob_term_home ));
          block_pos += FSIZ( otaspe_ftc_cell_data_fix_type, mob_term_home );

          /* Unpack termination indicator for SID roaming */
          int_ptr->dload_req.block[i].blk.ftc_cell.mob_term_for_sid =
            b_unpackb( otasp_ptr, block_pos,
              FSIZ( otaspe_ftc_cell_data_fix_type, mob_term_for_sid ));
          block_pos += FSIZ( otaspe_ftc_cell_data_fix_type, mob_term_for_sid );

          /* Unpack termination indicator for NID roaming */
          int_ptr->dload_req.block[i].blk.ftc_cell.mob_term_for_nid =
            b_unpackb( otasp_ptr, block_pos,
                       FSIZ( otaspe_ftc_cell_data_fix_type, mob_term_for_nid ));
          block_pos += FSIZ( otaspe_ftc_cell_data_fix_type, mob_term_for_nid );

          /* Unpack number of SID/NID pairs */
          int_ptr->dload_req.block[i].blk.ftc_cell.n_sid_nid =
            b_unpackb( otasp_ptr, block_pos,
                       FSIZ( otaspe_ftc_cell_data_fix_type, n_sid_nid ));
          block_pos += FSIZ( otaspe_ftc_cell_data_fix_type, n_sid_nid );

          for (j = 0; (j < int_ptr->dload_req.block[i].blk.ftc_cell.n_sid_nid) && (j < OTASPI_SID_NID_MAX) ;
               j++)
          {
            /* Unpack system identification */
            int_ptr->dload_req.block[i].blk.ftc_cell.var[j].sid =
              b_unpackw( otasp_ptr, block_pos,
                      FSIZ( otaspe_sid_nid_data_var_type, sid ));
            block_pos += FSIZ( otaspe_sid_nid_data_var_type, sid );

            /* Unpack network identification */
            int_ptr->dload_req.block[i].blk.ftc_cell.var[j].nid =
              b_unpackw( otasp_ptr, block_pos,
                        FSIZ( otaspe_sid_nid_data_var_type, nid ));
            block_pos += FSIZ( otaspe_sid_nid_data_var_type, nid );
          }

          break;

        /*****************************/
        /*  Mobile Directory Number  */
        /*****************************/
        case OTASP_MOB_DIR_NUM_BLK:
          /* Unpack number of digits */
          int_ptr->dload_req.block[i].blk.mob_dir.n_digits =
            b_unpackb( otasp_ptr, block_pos,
                       FSIZ( otaspe_mob_dir_data_fix_type, n_digits ));
          block_pos += FSIZ( otaspe_mob_dir_data_fix_type, n_digits );

          if( int_ptr->dload_req.block[i].blk.mob_dir.n_digits <= OTASPI_N_DIGITS_MAX )
          {
            /* Unpack mobile directory digits */
            for (j=0; j < int_ptr->dload_req.block[i].blk.mob_dir.n_digits; j++)
            {
              int_ptr->dload_req.block[i].blk.mob_dir.digitn[j] =
                b_unpackb( otasp_ptr, block_pos,
                           FSIZ( otaspe_mob_dir_data_var_type, digitn ));
              block_pos += FSIZ( otaspe_mob_dir_data_var_type, digitn );
            }
          }
          else
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Unexpected number of digits in dload req:%d > OTASPI_N_DIGITS_MAX",
              int_ptr->dload_req.block[i].blk.mob_dir.n_digits);
            return OTASPX_INV_LEN_S;
          }
          break;

        /****************************************/
        /*  PCS Indicator Data Parameter Block  */
        /****************************************/
        case OTASP_CDMA_NAM_BLK:
          /* Unpack IMSI Class assignment of the mobile station */
          int_ptr->dload_req.block[i].blk.ftc_pcs.imsi_class =
            b_unpackb( otasp_ptr, block_pos,
                      FSIZ( otaspe_ftc_pcs_data_fix_type, imsi_class ));
          block_pos += FSIZ( otaspe_ftc_pcs_data_fix_type, imsi_class );

          /* Unpack number of IMSI address digits */
          int_ptr->dload_req.block[i].blk.ftc_pcs.imsi_addr_num =
            b_unpackb( otasp_ptr, block_pos,
                       FSIZ( otaspe_ftc_pcs_data_fix_type, imsi_addr_num ));
          block_pos += FSIZ( otaspe_ftc_pcs_data_fix_type, imsi_addr_num );

          /* Unpack mobile country code */
          int_ptr->dload_req.block[i].blk.ftc_pcs.mcc =
            b_unpackw( otasp_ptr, block_pos,
                       FSIZ( otaspe_ftc_pcs_data_fix_type, mcc ));
          block_pos += FSIZ( otaspe_ftc_pcs_data_fix_type, mcc );

          /* Unpack 11th and 12th digits of the IMSI */
          int_ptr->dload_req.block[i].blk.ftc_pcs.imsi_11_12 =
            b_unpackb( otasp_ptr, block_pos,
                       FSIZ( otaspe_ftc_pcs_data_fix_type, imsi_11_12 ));
          block_pos += FSIZ( otaspe_ftc_pcs_data_fix_type, imsi_11_12 );

          /* Unpack the least significant 10 digits of the IMSI */
          b_unpackq( otasp_ptr, block_pos,
                     FSIZ( otaspe_ftc_pcs_data_fix_type, imsi_s ),
                     int_ptr->dload_req.block[i].blk.ftc_pcs.imsi_s );
          block_pos += FSIZ( otaspe_ftc_pcs_data_fix_type, imsi_s );

          /* Unpack access overload class */
          int_ptr->dload_req.block[i].blk.ftc_pcs.accolc =
            b_unpackb( otasp_ptr, block_pos,
                       FSIZ( otaspe_ftc_pcs_data_fix_type, accolc ));
          block_pos += FSIZ( otaspe_ftc_pcs_data_fix_type, accolc );

          /* Unpack local control status */
          int_ptr->dload_req.block[i].blk.ftc_pcs.local_control =
            b_unpackb( otasp_ptr, block_pos,
                       FSIZ( otaspe_ftc_pcs_data_fix_type, local_control ));
          block_pos += FSIZ( otaspe_ftc_pcs_data_fix_type, local_control );

          /* Unpack termination indicator for the home system */
          int_ptr->dload_req.block[i].blk.ftc_pcs.mob_term_home =
            b_unpackb( otasp_ptr, block_pos,
                       FSIZ( otaspe_ftc_pcs_data_fix_type, mob_term_home ));
          block_pos += FSIZ( otaspe_ftc_pcs_data_fix_type, mob_term_home );

          /* Unpack termination indicator for SID roaming */
          int_ptr->dload_req.block[i].blk.ftc_pcs.mob_term_for_sid =
            b_unpackb( otasp_ptr, block_pos,
              FSIZ( otaspe_ftc_pcs_data_fix_type, mob_term_for_sid ));
          block_pos += FSIZ( otaspe_ftc_pcs_data_fix_type, mob_term_for_sid );

          /* Unpack termination indicator for NID roaming */
          int_ptr->dload_req.block[i].blk.ftc_pcs.mob_term_for_nid =
            b_unpackb( otasp_ptr, block_pos,
                       FSIZ( otaspe_ftc_pcs_data_fix_type, mob_term_for_nid ));
          block_pos += FSIZ( otaspe_ftc_pcs_data_fix_type, mob_term_for_nid );

          /* Unpack number of SID/NID pairs */
          int_ptr->dload_req.block[i].blk.ftc_pcs.n_sid_nid =
            b_unpackb( otasp_ptr, block_pos,
                       FSIZ( otaspe_ftc_pcs_data_fix_type, n_sid_nid ));
          block_pos += FSIZ( otaspe_ftc_pcs_data_fix_type, n_sid_nid );

          for (j = 0; j < int_ptr->dload_req.block[i].blk.ftc_pcs.n_sid_nid && j < OTASPI_SID_NID_MAX;
                 j++)
          {
            /* Unpack system identification */
            int_ptr->dload_req.block[i].blk.ftc_pcs.var[j].sid =
              b_unpackw( otasp_ptr, block_pos,
                       FSIZ( otaspe_sid_nid_data_var_type, sid ));
            block_pos += FSIZ( otaspe_sid_nid_data_var_type, sid );

            /* Unpack network identification */
            int_ptr->dload_req.block[i].blk.ftc_pcs.var[j].nid =
              b_unpackw( otasp_ptr, block_pos,
                       FSIZ( otaspe_sid_nid_data_var_type, nid ));
            block_pos += FSIZ( otaspe_sid_nid_data_var_type, nid );
          }
          break;

        /******************/
        /*  Roaming List  */
        /******************/
        case OTASP_PREF_ROAM_LIST_BLK:
          /* Skip past reserved bits */
          block_pos += FSIZ( otaspe_ftc_roam_lst_fix_type, reserved);

          /* Unpack Sequence Number */
          int_ptr->dload_req.block[i].blk.ftc_roam.seq_num =
            b_unpackb( otasp_ptr, block_pos,
                       FSIZ( otaspe_ftc_roam_lst_fix_type, seq_num ));
          block_pos += FSIZ( otaspe_ftc_roam_lst_fix_type, seq_num);

          /* Unpack number of Roaming List bits in this block */
          int_ptr->dload_req.block[i].blk.ftc_roam.num_bits =
            b_unpackw( otasp_ptr, block_pos,
                       FSIZ( otaspe_ftc_roam_lst_fix_type, num_bits ));
          block_pos += FSIZ( otaspe_ftc_roam_lst_fix_type, num_bits );

          /* -------------------------------------------------------------
          ** Set pointer to first byte of Roaming List data in this block.
          ** We depend on this data always being byte alligned.
          ** ------------------------------------------------------------- */
          int_ptr->dload_req.block[i].blk.ftc_roam.bits_ptr =
            otasp_ptr + (block_pos / 8);
          if ((block_pos % 8) != 0)
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Unexpected offset while unpacking Roaming List");
          }
          block_pos += int_ptr->dload_req.block[i].blk.ftc_roam.num_bits;
          break;

        /**********************************/
        /*  Programming Lock Code Verify  */
        /**********************************/
        case OTASP_VERIFY_SPC_BLK:
          /* Unpack SPC digits - copy 4 bit BCD digits into 8 bit fields */
          for (j = 0; j < int_ptr->dload_req.block[i].block_len * 2; j++)
          {
            /* Make sure we don't overflow if they send too many digits */
            if (j < NV_SEC_CODE_SIZE)
            {
              int_ptr->dload_req.block[i].blk.ftc_spc[j] =
                b_unpackb( otasp_ptr, block_pos,
                           FSIZ( otaspe_ftc_spc_type, bcd_digit ));
            }
            block_pos += FSIZ( otaspe_ftc_spc_type, bcd_digit);
          }
          break;

        /**********************************/
        /*  True IMSI Block               */
        /**********************************/
        case OTASP_IMSI_T_BLK:
           /* Unpack IMSI Class assignment of the mobile station */
          int_ptr->dload_req.block[i].blk.imsi_t.imsi_t_class =
            b_unpackb( otasp_ptr, block_pos,
                      FSIZ( otaspe_imsi_t_fix_type, imsi_t_class ));
          block_pos += FSIZ( otaspe_imsi_t_fix_type, imsi_t_class );

          /* Unpack number of IMSI address digits */
          int_ptr->dload_req.block[i].blk.imsi_t.imsi_t_addr_num =
            b_unpackb( otasp_ptr, block_pos,
                       FSIZ( otaspe_imsi_t_fix_type, imsi_t_addr_num ));
          block_pos += FSIZ( otaspe_imsi_t_fix_type, imsi_t_addr_num );

          /* Unpack mobile country code */
          int_ptr->dload_req.block[i].blk.imsi_t.imsi_t_mcc =
            b_unpackw( otasp_ptr, block_pos,
                       FSIZ( otaspe_imsi_t_fix_type, mcc_t ));
          block_pos += FSIZ( otaspe_imsi_t_fix_type, mcc_t );

          /* Unpack 11th and 12th digits of the IMSI_T */
          int_ptr->dload_req.block[i].blk.imsi_t.imsi_t_11_12 =
            b_unpackb( otasp_ptr, block_pos,
                       FSIZ( otaspe_imsi_t_fix_type, imsi_t_11_12 ));
          block_pos += FSIZ( otaspe_imsi_t_fix_type, imsi_t_11_12 );

          /* Unpack the least significant 10 digits of the IMSI_T */
          b_unpackq( otasp_ptr, block_pos,
                     FSIZ( otaspe_imsi_t_fix_type, imsi_t_s ),
                     int_ptr->dload_req.block[i].blk.imsi_t.imsi_t_s );
          block_pos += FSIZ( otaspe_imsi_t_fix_type, imsi_t_s );

          /* Move past the reserved bit */
          block_pos += FSIZ( otaspe_imsi_t_fix_type, reserved );

          break;

        /**********************************/
        /*  eHRPD IMSI Block               */
        /**********************************/
        case OTASP_EHRPD_IMSI_BLK:
          /* Unpack number of mnc digits */
          int_ptr->dload_req.block[i].blk.ehrpd_imsi.ehrpd_mnc_num_digits =
            b_unpackb( otasp_ptr, block_pos,
                      FSIZ( otaspe_ehrpd_imsi_fix_type, ehrpd_mnc_num_digits ));
          block_pos += FSIZ( otaspe_ehrpd_imsi_fix_type, ehrpd_mnc_num_digits );

          /* Unpack number of IMSI digits */
          int_ptr->dload_req.block[i].blk.ehrpd_imsi.ehrpd_imsi_num_digits =
            b_unpackb( otasp_ptr, block_pos,
                      FSIZ( otaspe_ehrpd_imsi_fix_type, ehrpd_imsi_num_digits ));
          block_pos += FSIZ( otaspe_ehrpd_imsi_fix_type, ehrpd_imsi_num_digits );

          /* Move past the reserved bits */
          block_pos += FSIZ( otaspe_ehrpd_imsi_fix_type, reserved );

          if( int_ptr->dload_req.block[i].blk.ehrpd_imsi.ehrpd_imsi_num_digits <= OTASPI_EHRPD_IMSI_DIGITS_MAX )
          {
            /* Unpack the IMSI digits */
            for (j = 0; j < int_ptr->dload_req.block[i].blk.ehrpd_imsi.ehrpd_imsi_num_digits; j++)
            {
              int_ptr->dload_req.block[i].blk.ehrpd_imsi.ehrpd_imsi_digits[j] =
                b_unpackb( otasp_ptr, block_pos,
                          FSIZ( otaspe_ehrpd_imsi_digits_var_type, ehrpd_imsi_digits ));
              block_pos += FSIZ( otaspe_ehrpd_imsi_digits_var_type, ehrpd_imsi_digits );
            }
          }
          else
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Unexpected number of IMSI digits in dload req:%d > OTASPI_EHRPD_IMSI_DIGITS_MAX",
              int_ptr->dload_req.block[i].blk.ehrpd_imsi.ehrpd_imsi_num_digits);
            return OTASPX_INV_LEN_S;
          }

          break;

        default:
          break;
      } /* end of switch */
    }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

    /* ----------------------------
    ** Sanity check for parameter block length
    ** ---------------------------- */
    if (block_pos > msg_pos)
    {
      /* Flag block as unusable by setting length to 0 */
      int_ptr->dload_req.block[i].block_len = 0;
    }
  } /* end of for loop */

  /* ------------------------------------------
  ** Sanity check for OTASP Data Message length
  ** ------------------------------------------ */
  if (msg_pos > (otasp_len * 8))
  {
    status = OTASPX_INV_LEN_S; /*lint !e641 */
  }

  return (status);

} /* xlate_ext_download_req */

/*===========================================================================

FUNCTION XLATE_EXT_MS_KEY_REQ

DESCRIPTION
  This function translates the received OTASP MS Key Request
  Message external to internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_ext_ms_key_req
(
  word otasp_len,
    /* Length of OTASP message in bytes */
  byte *otasp_ptr,
    /* Pointer to received OTASP message to be translated */
  otaspi_ftc_msg_type *int_ptr
    /* Pointer to place where translated message should be placed */
)
{
  word status;
    /* Status returned to calling procedure */
  word msg_pos;
    /* Bit position in OTASP message relative to start of otasp_ptr */
  byte i;
    /* Loop index */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize status */
  status = OTASPX_DONE_S; /*lint !e641 */

  /* Advance buffer pointer past msg_type field */
  msg_pos = FSIZ( otaspe_ms_key_req_fix1_type, msg_type );

  /* Unpack Key exchange protocol version */
  int_ptr->ms_key_req.a_key_p_rev =
    b_unpackb( otasp_ptr, msg_pos,
               FSIZ( otaspe_ms_key_req_fix1_type, a_key_p_rev ));
  msg_pos += FSIZ( otaspe_ms_key_req_fix1_type, a_key_p_rev );

  /* Unpack length of PARAM_P field */
  int_ptr->ms_key_req.param_p_len =
    b_unpackb( otasp_ptr, msg_pos,
               FSIZ( otaspe_ms_key_req_fix1_type, param_p_len ));
  msg_pos += FSIZ( otaspe_ms_key_req_fix1_type, param_p_len );

  /* Unpack the parameter PARAM_P as dwords*/
  for (i = 0 ; ( i < (int_ptr->ms_key_req.param_p_len/sizeof(dword)))
       && (i < OTASPI_PARAM_P/(sizeof(dword))); i++)
  {
    int_ptr->ms_key_req.param_p[i] =
      b_unpackd( otasp_ptr, msg_pos, 8* sizeof(dword));
    msg_pos += (8 * sizeof(dword)) ;
  }

  /* Unpack length of PARAM_G field   */
  int_ptr->ms_key_req.param_g_len =
    b_unpackb( otasp_ptr, msg_pos,
               FSIZ( otaspe_ms_key_req_fix2_type, param_g_len ));
  msg_pos += FSIZ( otaspe_ms_key_req_fix2_type, param_g_len );

  /* Unpack the parameter PARAM_G as dwords*/
  for (i = 0 ;( i < (int_ptr->ms_key_req.param_g_len / sizeof(dword)))
       && (i< OTASPI_PARAM_G_MAX/(sizeof(dword))) ; i++)
  {
    int_ptr->ms_key_req.param_g[i] =
      b_unpackd( otasp_ptr, msg_pos, 8 * sizeof(dword));
    msg_pos += (8 * sizeof(dword)) ;
  }

  /* ------------------------------------------
  ** Sanity check for OTASP Data Message length
  ** ------------------------------------------ */
  if (msg_pos > (otasp_len * 8))
  {
    status = OTASPX_INV_LEN_S; /*lint !e641 */
  }

  /* Return translation status */
  return (status);

} /* xlate_ext_ms_key_req */

/*===========================================================================

FUNCTION XLATE_EXT_KEY_GEN_REQ

DESCRIPTION
  This function translates the received OTASP Key Generation Request
  Message external to internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_ext_key_gen_req
(
  word otasp_len,
    /* Length of OTASP message in bytes */
  byte *otasp_ptr,
    /* Pointer to received OTASP message to be translated */
  otaspi_ftc_msg_type *int_ptr
    /* Pointer to place where translated message should be placed */
)
{
  word status;
    /* Status returned to calling procedure */
  word msg_pos;
    /* Bit position in OTASP message relative to start of otasp_ptr */
  byte i;
    /* Loop index */
  byte j,k;
    /* Offset into the array */
  unsigned char temp[4];
   /* Temporary array */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize status */
  status = OTASPX_DONE_S; /*lint !e641 */

  /* Advance buffer pointer past msg_type field */
  msg_pos = FSIZ( otaspe_key_gen_req_fix_type, msg_type );

  /* Unpack length of BS_RESULT field */
  int_ptr->key_gen_req.bs_result_len =
    b_unpackb( otasp_ptr, msg_pos,
               FSIZ( otaspe_key_gen_req_fix_type, bs_result_len ));
  msg_pos += FSIZ( otaspe_key_gen_req_fix_type, bs_result_len );

  /* initialize the temp array */
  for (i = 0; i < 4; i++)
      temp[i] = 0 ;

  /* Initialize the array of 64 dwords to zeroes */
  memset(int_ptr->key_gen_req.bs_result, 0, OTASPI_PARAM_P);

  j = int_ptr->key_gen_req.bs_result_len / sizeof(dword) ;

  k = (int_ptr->key_gen_req.bs_result_len) - (byte)(j * 4);

  j = (byte) ((OTASPI_PARAM_P - int_ptr->key_gen_req.bs_result_len) /
              sizeof(dword)); /*lint !e573 */

  if ((k != 0) && (k < 5)) {
     for (i = k; i > 0; i--) {
         temp[4-i] = b_unpackb(otasp_ptr, msg_pos, 8);
         msg_pos+=8;
     }
     int_ptr->key_gen_req.bs_result[j++] = b_unpackd(temp, 0, 32);
  }

  /* Unpack the parameter BS_RESULT as dwords*/
  for (i= j; i < (OTASPI_PARAM_P / sizeof(dword)) ; i++)
  {
    int_ptr->key_gen_req.bs_result[i] =
      b_unpackd( otasp_ptr, msg_pos, 8 * sizeof(dword));
    msg_pos += (8 * sizeof(dword)) ;
  }

  /* ------------------------------------------
  ** Sanity check for OTASP Data Message length
  ** ------------------------------------------ */
  if (msg_pos > (otasp_len * 8))
  {
    status = OTASPX_INV_LEN_S; /*lint !e641 */
  }

  /* Return translation status */
  return (status);

} /* xlate_ext_key_gen_req */

/*===========================================================================

FUNCTION XLATE_EXT_REAUTH_REQ

DESCRIPTION
  This function translates the received OTASP Re-Authentication Request
  Message external to internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_ext_reauth_req
(
  word otasp_len,
    /* Length of OTASP message in bytes */
  byte *otasp_ptr,
    /* Pointer to received OTASP message to be translated */
  otaspi_ftc_msg_type *int_ptr
    /* Pointer to place where translated message should be placed */
)
{
  word status;
    /* Status returned to calling procedure */
  word msg_pos;
    /* Bit position in OTASP message relative to start of otasp_ptr */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize status */
  status = OTASPX_DONE_S; /*lint !e641 */

  /* Advance buffer pointer past msg_type field */
  msg_pos = FSIZ( otaspe_reauth_req_type, msg_type );

  /* Unpack the Random Challenge Value */
  int_ptr->reauth_req.rand =
    b_unpackd( otasp_ptr, msg_pos,
               FSIZ( otaspe_reauth_req_type, rand ));
  msg_pos += FSIZ( otaspe_reauth_req_type, rand );

    /* ------------------------------------------
  ** Sanity check for OTASP Data Message length
  ** ------------------------------------------ */
  if (msg_pos > (otasp_len * 8))
  {
    status = OTASPX_INV_LEN_S; /*lint !e641 */
  }

  /* Return translation status */
  return (status);

} /* xlate_ext_reauth_req */


/*===========================================================================

FUNCTION XLATE_PROT_CAP_REQ

DESCRIPTION
  This function translates the received OTASP Protocol Capability Request
  Message external to internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_ext_prot_cap_req
(
  word otasp_len,
    /* Length of OTASP message in bytes */
  byte *otasp_ptr,
    /* Pointer to received OTASP message to be translated */
  otaspi_ftc_msg_type *int_ptr
    /* Pointer to place where translated message should be placed */
)
{
  word status;
    /* Status returned to calling procedure */
  word msg_pos;
    /* Bit position in OTASP message relative to start of otasp_ptr */
  uint8 i;
    /* local variable */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize status */
  status = OTASPX_DONE_S; /*lint !e641 */

  if (otasp_len == 1)
  {
      /* there is no more fields in the message other than the msg_type */
      int_ptr->prot_cap_req.otasp_ext_cap_rsp_required = FALSE;
      return(status);
  }

  /* Advance buffer pointer past msg_type field */
  msg_pos = FSIZ( otaspe_prot_cap_req_type, msg_type );

  /* Unpack the otasp_p_rev */
  int_ptr->prot_cap_req.otasp_p_rev =
     b_unpackb( otasp_ptr, msg_pos,
           FSIZ( otaspe_prot_cap_req_type, otasp_p_rev ));
  msg_pos += FSIZ( otaspe_prot_cap_req_type, otasp_p_rev );

  /* This a new format message that supports otasp_p_rev indicator
   * and requires the ext_cap_rsp_msg to be sent
   */
  int_ptr->prot_cap_req.otasp_ext_cap_rsp_required = TRUE;

  /* Unpack the num_cap_records */
  int_ptr->prot_cap_req.num_cap_records =
     b_unpackb( otasp_ptr, msg_pos,
           FSIZ( otaspe_prot_cap_req_type, num_cap_records ));
  msg_pos += FSIZ( otaspe_prot_cap_req_type, num_cap_records );

  for (i = 0; (i < int_ptr->prot_cap_req.num_cap_records)
       && (i < OTASPI_CAP_INFO_REC_TYPE_MAX) ; i++)
  {
      /* Unpack capability information record types */
      int_ptr->prot_cap_req.cap_record_type[i] =
        b_unpackb( otasp_ptr, msg_pos,
                   FSIZ( otaspe_prot_cap_req_type, cap_record_type ));
      msg_pos += FSIZ( otaspe_prot_cap_req_type, cap_record_type );
  }

  /* Return translation status */
  return (status);

} /*xlate_ext_prot_cap_req */


/*===========================================================================

FUNCTION XLATE_SSPR_CONFIG_REQ

DESCRIPTION
  This function translates the received OTASP SSPR Configuration Request
  Message external to internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_ext_sspr_config_req
(
  word otasp_len,
    /* Length of OTASP message in bytes */
  byte *otasp_ptr,
    /* Pointer to received OTASP message to be translated */
  otaspi_ftc_msg_type *int_ptr
    /* Pointer to place where translated message should be placed */
)
{
  word status;
    /* Status returned to calling procedure */
  word msg_pos;
    /* Bit position in OTASP message relative to start of otasp_ptr */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize status */
  status = OTASPX_DONE_S; /*lint !e641 */

  /* Advance buffer pointer past msg_type field */
  msg_pos = FSIZ( otaspe_sspr_cfg_req_fix_type, msg_type );

  /* Unpack the parameter block id */
  int_ptr->sspr_cfg_req.block_id =
     b_unpackb( otasp_ptr, msg_pos,
           FSIZ( otaspe_sspr_cfg_req_fix_type, block_id ));
  msg_pos += FSIZ( otaspe_sspr_cfg_req_fix_type, block_id );

  if (int_ptr->sspr_cfg_req.block_id == OTASP_CFG_PREF_ROAM_LIST_BLK) {
     /* Unpack the Segment offset */
     int_ptr->sspr_cfg_req.req_offset =
        b_unpackw( otasp_ptr, msg_pos,
           FSIZ( otaspe_sspr_cfg_req_var_type, req_offset ));
     msg_pos += FSIZ( otaspe_sspr_cfg_req_var_type, req_offset );

     /* Unpack the maximum segment size  */
     int_ptr->sspr_cfg_req.req_max_size =
        (byte)b_unpackw( otasp_ptr, msg_pos,
                FSIZ( otaspe_sspr_cfg_req_var_type, req_max_size ));
     msg_pos += FSIZ( otaspe_sspr_cfg_req_var_type, req_max_size );
  }

  /* ------------------------------------------
  ** Sanity check for OTASP Data Message length
  ** ------------------------------------------ */
  if (msg_pos > (otasp_len * 8))
  {
    status = OTASPX_INV_LEN_S; /*lint !e641 */
  }

  /* Return translation status */
  return (status);

} /*xlate_ext_sspr_config_req */

/*===========================================================================

FUNCTION XLATE_SSPR_DOWNLOAD_REQ

DESCRIPTION
  This function translates the received OTASP SSPR Download Request
  Message external to internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_ext_sspr_download_req
(
  word otasp_len,
    /* Length of OTASP message in bytes */
  byte *otasp_ptr,
    /* Pointer to received OTASP message to be translated */
  otaspi_ftc_msg_type *int_ptr
    /* Pointer to place where translated message should be placed */
)
{
  word status;
    /* Status returned to calling procedure */
  word msg_pos;
    /* Bit position in OTASP message relative to start of otasp_ptr */
  word block_pos;
    /* Starting bit position of a parameter block */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize status */
  status = OTASPX_DONE_S; /*lint !e641 */

  /* Advance buffer pointer past msg_type field */
  msg_pos = FSIZ( otaspe_sspr_dload_req_type, msg_type );

  /* Unpack the parameter block id */
  int_ptr->sspr_dload_req.block_id =
     b_unpackb( otasp_ptr, msg_pos,
           FSIZ( otaspe_sspr_dload_req_type, block_id ));
  msg_pos += FSIZ( otaspe_sspr_dload_req_type, block_id );

  /* Unpack the block length */
  int_ptr->sspr_dload_req.block_len =
     b_unpackb( otasp_ptr, msg_pos,
           FSIZ( otaspe_sspr_dload_req_type, block_len ));
  msg_pos += FSIZ( otaspe_sspr_dload_req_type, block_len );

  /* Save starting bit position of current parameter block */
  block_pos = msg_pos;

  /* Calculate the beginning of next parameter block
  ** Though there is no other block in this message, this
  ** is used to check if the block is valid */

  msg_pos += (int_ptr->sspr_dload_req.block_len * 8);

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    /* We do not want to parse the data and so copy the bits corresponding to
       PRL in the PRL Data buffer for R-UIM */

    b_copy(otasp_ptr, block_pos, int_ptr->sspr_dload_req.block.blk.prl_data, 0,
           (int_ptr->sspr_dload_req.block_len * 8));

    block_pos += (int_ptr->sspr_dload_req.block_len * 8);
  }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  {
    switch( int_ptr->sspr_dload_req.block_id)
    {
      case OTASP_DLOAD_PREF_ROAM_LIST:
      case OTASP_DLOAD_EXT_PREF_ROAM_LIST:

        /* move past the reserved bits */
        block_pos += FSIZ(otaspe_sspr_prl_fix_type, reserved);

        /* unpack the last segment indicator */
        int_ptr->sspr_dload_req.block.blk.sspr_prl.last_segment =
           b_unpackb(otasp_ptr, block_pos,
                 FSIZ(otaspe_sspr_prl_fix_type, last_segment));
        block_pos += FSIZ(otaspe_sspr_prl_fix_type, last_segment);

        /* unpack the segment offset   */
        int_ptr->sspr_dload_req.block.blk.sspr_prl.segment_offset =
           b_unpackw(otasp_ptr, block_pos,
                 FSIZ(otaspe_sspr_prl_fix_type, segment_offset));
        block_pos += FSIZ(otaspe_sspr_prl_fix_type, segment_offset);

        /* unpack the segment size     */
        int_ptr->sspr_dload_req.block.blk.sspr_prl.segment_size =
           b_unpackb(otasp_ptr, block_pos,
                 FSIZ(otaspe_sspr_prl_fix_type, segment_size));
        block_pos += FSIZ(otaspe_sspr_prl_fix_type, segment_size);

        /* -------------------------------------------------------------
        ** Set pointer to first byte of Roaming List data in this block.
        ** We depend on this data always being byte alligned.
        ** ------------------------------------------------------------- */
        int_ptr->sspr_dload_req.block.blk.sspr_prl.bits_ptr =
            otasp_ptr + (block_pos/ 8);
        if ((block_pos % 8) != 0)  /*lint !e774 */
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Unexpected offset while unpacking Roaming List");
        }
        block_pos += (int_ptr->sspr_dload_req.block.blk.sspr_prl.segment_size * 8);

        break;
      default:
        break;

     } /* end of switch */
  }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
  /* ----------------------------
  ** Sanity check for parameter block length
  ** ---------------------------- */
  if (block_pos > msg_pos)
  {
      /* Flag block as unusable by setting length to 0 */
      int_ptr->sspr_dload_req.block_len = 0;
  }

  /* ------------------------------------------
  ** Sanity check for OTASP Data Message length
  ** ------------------------------------------ */
  if (msg_pos > (otasp_len * 8))
  {
    status = OTASPX_INV_LEN_S; /*lint !e641 */
  }

  return (status);

} /* xlate_ext_sspr_download_req   */

/*===========================================================================

FUNCTION XLATE_VALIDATION_REQ

DESCRIPTION
  This function translates the received OTASP Validation Request
  Message external to internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_ext_validation_req
(
  word otasp_len,
    /* Length of OTASP message in bytes */
  byte *otasp_ptr,
    /* Pointer to received OTASP message to be translated */
  otaspi_ftc_msg_type *int_ptr
    /* Pointer to place where translated message should be placed */
)
{
  word status;
    /* Status returned to calling procedure */
  word msg_pos;
    /* Bit position in OTASP message relative to start of otasp_ptr */
  word block_pos;
    /* Starting bit position of a parameter block */
  byte i;
    /* Loop index */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  byte j;
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize status */
  status = OTASPX_DONE_S; /*lint !e641 */

  /* Advance buffer pointer past msg_type field */
  msg_pos = FSIZ( otaspe_validation_req_fix_type, msg_type );

  /* Unpack the Number of parameter blocks */
  int_ptr->validn_req.num_blocks =
     b_unpackb( otasp_ptr, msg_pos,
           FSIZ( otaspe_validation_req_fix_type, num_blocks ));
  msg_pos += FSIZ( otaspe_validation_req_fix_type, num_blocks );

  int_ptr->validn_req.num_blocks =
    MIN( int_ptr->validn_req.num_blocks, OTASPI_VALIDN_BLOCK_MAX );

  /* ----------------------------------
  ** Unpack parameter block information
  ** ---------------------------------- */

  /* Loop through each parameter block set to unpack data */
  for (i=0; i < int_ptr->validn_req.num_blocks; i++)
  {
    /* Unpack parameter block identifier */
    int_ptr->validn_req.block[i].block_id =
      b_unpackb( otasp_ptr, msg_pos,
                 FSIZ( otaspe_validation_req_var_type, block_id ));
    msg_pos += FSIZ( otaspe_validation_req_var_type, block_id );

    /* Unpack parameter block length */
    int_ptr->validn_req.block[i].block_len =
      b_unpackb( otasp_ptr, msg_pos,
                 FSIZ( otaspe_validation_req_var_type, block_len ));
    msg_pos += FSIZ( otaspe_validation_req_var_type, block_len );

    /* Save starting bit position of current parameter block */
    block_pos = msg_pos;

    /* Calculate the beginning of next parameter block */
    msg_pos += (int_ptr->validn_req.block[i].block_len * 8);

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    /* Determine if the R-UIM is available for this NAM */
    if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    {
      /* We do not want to parse the data if the data is going to be sent to
         the R-UIM for OTASP */

      b_copy(otasp_ptr, block_pos, int_ptr->validn_req.block[i].blk.validn_data,
             0, int_ptr->validn_req.block[i].block_len * 8 );

      block_pos += int_ptr->validn_req.block[i].block_len * 8;
    }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
    {
      /* -----------------------------
      ** Decode parameter block data
      ** --------------------------- */
      switch (int_ptr->validn_req.block[i].block_id)
      {
        /*********************************************/
        /*          Verify SPC Block                 */
        /*********************************************/
        case OTASP_VERIFY_SPC_BLOCK:

          /* Unpack SPC digits - copy 4 bit BCD digits into 8 bit fields */
          for (j = 0; j < int_ptr->validn_req.block[i].block_len * 2; j++)
          {
            /* Make sure we don't overflow if they send too many digits */
            if (j < NV_SEC_CODE_SIZE)
            {
              int_ptr->validn_req.block[i].blk.verify_spc.spc[j] =
                b_unpackb( otasp_ptr, block_pos,
                           FSIZ( otaspe_ftc_spc_type, bcd_digit ));
            }
            block_pos += FSIZ( otaspe_ftc_spc_type, bcd_digit);
          }

          break;

        /*********************************************/
        /*          Change SPC Block                 */
        /*********************************************/

        case OTASP_CHANGE_SPC_BLOCK:

          /* Unpack SPC digits - copy 4 bit BCD digits into 8 bit fields */
          for (j = 0; j < int_ptr->validn_req.block[i].block_len * 2; j++)
          {
            /* Make sure we don't overflow if they send too many digits */
            if (j < NV_SEC_CODE_SIZE)
            {
              int_ptr->validn_req.block[i].blk.change_spc.spc[j] =
                b_unpackb( otasp_ptr, block_pos,
                           FSIZ( otaspe_ftc_spc_type, bcd_digit ));
            }
            block_pos += FSIZ( otaspe_ftc_spc_type, bcd_digit);
          }

          break;

        /*********************************************/
        /*            Validate SPASM                 */
        /*********************************************/

        case VALIDATE_SPASM:

          /* Unpack the OTAPA SPASM Signature included indicator  */

          int_ptr->validn_req.block[i].blk.validate_spasm.auth_otapa_incl =
               b_unpackb( otasp_ptr, block_pos,
                    FSIZ( otaspe_ftc_valid_spasm_fix_type, auth_otapa_incl));
          block_pos += FSIZ( otaspe_ftc_valid_spasm_fix_type, auth_otapa_incl);

          /* Unpack validation signature if the indicator above is set */
          if (int_ptr->validn_req.block[i].blk.validate_spasm.auth_otapa_incl)
          {
              int_ptr->validn_req.block[i].blk.validate_spasm.auth_otapa =
                  b_unpackd( otasp_ptr, block_pos,
                      FSIZ( otaspe_ftc_valid_spasm_var_type, auth_otapa));
              block_pos += FSIZ( otaspe_ftc_valid_spasm_var_type, auth_otapa);
          }

          int_ptr->validn_req.block[i].blk.validate_spasm.nam_lock =
               b_unpackb( otasp_ptr, block_pos,
                    FSIZ( otaspe_ftc_valid_spasm_fix2_typ, nam_lock));
          block_pos += FSIZ( otaspe_ftc_valid_spasm_fix2_typ, nam_lock);

          /* Move past the reserved bits according to the indicator */
          if (int_ptr->validn_req.block[i].blk.validate_spasm.auth_otapa_incl)
              block_pos += 4;
          else
              block_pos += 6;

          break;
        default:
          break;

      } /* end of switch */
    }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

    /* ----------------------------------------
    ** Sanity check for parameter block length
    ** ------------------------------------- */
    if (block_pos > msg_pos)
    {
      /* Flag block as unusable by setting length to 0 */
      int_ptr->dload_req.block[i].block_len = 0;
    }
  } /* end of for loop */

  /* ------------------------------------------
  ** Sanity check for OTASP Data Message length
  ** ------------------------------------------ */
  if (msg_pos > (otasp_len * 8))
  {
    status = OTASPX_INV_LEN_S; /*lint !e641 */
  }

  return (status);

} /* xlate_ext_validation_req */

/*===========================================================================

FUNCTION XLATE_OTAPA_REQ

DESCRIPTION
  This function translates the received OTASP OTAPA Request
  Message external to internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_ext_otapa_req
(
  word otasp_len,
    /* Length of OTASP message in bytes */
  byte *otasp_ptr,
    /* Pointer to received OTASP message to be translated */
  otaspi_ftc_msg_type *int_ptr
    /* Pointer to place where translated message should be placed */
)
{
  word status;
    /* Status returned to calling procedure */
  word msg_pos;
    /* Bit position in OTASP message relative to start of otasp_ptr */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize status */
  status = OTASPX_DONE_S; /*lint !e641 */

  /* Advance buffer pointer past msg_type field */
  msg_pos = FSIZ( otaspe_otapa_req_type, msg_type );

  /* Unpack the Start_stop indicator */
  int_ptr->otapa_req.start_stop =
     b_unpackb( otasp_ptr, msg_pos,
           FSIZ( otaspe_otapa_req_type, start_stop ));
  msg_pos += FSIZ( otaspe_otapa_req_type, start_stop );

  /* Move past the reserved field  */
  msg_pos += FSIZ( otaspe_otapa_req_type, reserved );

  /* ------------------------------------------
 ** Sanity check for OTASP Data Message length
 ** ------------------------------------------ */
 if (msg_pos > (otasp_len * 8))
 {
   status = OTASPX_INV_LEN_S; /*lint !e641 */
 }

 return (status);

} /*xlate_ext_otapa_req */

/*===========================================================================

FUNCTION OTASPX_EXT_TO_INT

DESCRIPTION
  This function translates a message from the external format used
  by the Receive task to the internal format used by the Main Control
  task.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word otaspx_ext_to_int
(
  word otasp_len,
    /* Length of OTASP message in bytes */
  byte *rx_otasp_ptr,
    /* Pointer to received OTASP message to be translated */
  otaspi_ftc_msg_type *int_otasp_ptr
    /* Pointer to place where translated message should be placed */
)
{
  word status;
    /* Status to be returned to calling procedure */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize status */
  status = OTASPX_DONE_S; /*lint !e641 */

  /* -------------------
  ** Decode message type
  ** ------------------- */
  switch (int_otasp_ptr->gen.msg_type =
             b_unpackb( rx_otasp_ptr,
                        (word)FPOS( otaspe_gen_type, msg_type ),
                        FSIZ( otaspe_gen_type, msg_type )))
  {
    case OTASP_CONFIG_REQ_MSG:
      status = xlate_ext_config_req( otasp_len, rx_otasp_ptr, int_otasp_ptr );
      break;

    case OTASP_DOWNLOAD_REQ_MSG:
      status =
        xlate_ext_download_req( otasp_len, rx_otasp_ptr, int_otasp_ptr );
      break;

    case OTASP_MS_KEY_REQ_MSG:
      status =
        xlate_ext_ms_key_req( otasp_len, rx_otasp_ptr, int_otasp_ptr );
      break;

    case OTASP_KEY_GEN_REQ_MSG:
      status =
        xlate_ext_key_gen_req( otasp_len, rx_otasp_ptr, int_otasp_ptr );
      break;

    case OTASP_REAUTH_REQ_MSG:
      status =
        xlate_ext_reauth_req( otasp_len, rx_otasp_ptr, int_otasp_ptr );
      break;

    case OTASP_COMMIT_REQ_MSG:
      /* No additional OTASP Data Message fields to unpack */
      break;

  case OTASP_PROT_CAP_REQ_MSG:

      status =
         xlate_ext_prot_cap_req( otasp_len, rx_otasp_ptr, int_otasp_ptr );

      break;

    case OTASP_SSPR_CONFIG_REQ_MSG:
      status =
         xlate_ext_sspr_config_req( otasp_len, rx_otasp_ptr, int_otasp_ptr );
      break;

    case OTASP_SSPR_DOWNLOAD_REQ_MSG:
      status =
         xlate_ext_sspr_download_req( otasp_len, rx_otasp_ptr, int_otasp_ptr );
      break;

    case OTASP_VALIDATION_REQ_MSG:
      status =
         xlate_ext_validation_req( otasp_len, rx_otasp_ptr, int_otasp_ptr );
      break;

    case OTASP_OTAPA_REQ_MSG:
      status =
         xlate_ext_otapa_req( otasp_len, rx_otasp_ptr, int_otasp_ptr );
      break;

    default:
      status = OTASPX_INV_MSG_S; /*lint !e641 */
      break;
  } /* end of switch */

  return (status);

} /* otaspx_ext_to_int */

/*===========================================================================

FUNCTION XLATE_INT_CONFIG_RSP

DESCRIPTION
  This function translates the received OTASP Configuration Response
  Message from internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_int_config_rsp
(
  word *otasp_len,
    /* Pointer to OTASP message length in bits */
  byte *otasp_ptr,
    /* Pointer to (bit-packed) OTASP message to be transmitted */
  otaspi_rtc_msg_type *int_ptr
    /* Pointer to internal format OTASP message to be translated */
)
{
  word status;
    /* Status returned to calling procedure */
  word buffer_pos = 0;
    /* Position to pack bits in message */
  word block_len_pos;
    /* Block length bit offset in OTASP message */
  word block_len;
    /* Parameter block data length in bytes */
  word reserve_size;
    /* Number of reserved bits at the end of each parameter block */
  byte i;
    /* Loop counters */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  byte j;
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize status */
  status = OTASPX_DONE_S; /*lint !e641 */

  /* Pack OTASP Data Message type */
  b_packb( int_ptr->config_rsp.msg_type, otasp_ptr, buffer_pos,
           FSIZ( otaspe_config_rsp_fix_type, msg_type ));
  buffer_pos += FSIZ( otaspe_config_rsp_fix_type, msg_type );

  /* Pack number of parameter blocks requested */
  b_packb( int_ptr->config_rsp.num_blocks, otasp_ptr, buffer_pos,
           FSIZ( otaspe_config_rsp_fix_type, num_blocks ));
  buffer_pos += FSIZ( otaspe_config_rsp_fix_type, num_blocks );

  /* -----------------------------------------------
  ** Sanity check internal OTASP Data Message length
  ** ----------------------------------------------- */
  if (int_ptr->config_rsp.num_blocks > OTASPI_PARAM_BLOCK_MAX)
  {
    return (OTASPX_INV_LEN_S); /*lint !e641 */
  }

  /* -----------------------------------------
  ** Pack all parameter block data information
  ** ----------------------------------------- */
  for (i=0; i < int_ptr->config_rsp.num_blocks; i++)
  {
    b_packb( int_ptr->config_rsp.block[i].block_id, otasp_ptr, buffer_pos,
             FSIZ( otaspe_config_rsp_var_type, block_id ));
    buffer_pos += FSIZ( otaspe_config_rsp_var_type, block_id );

    /* ------------------------------------------------------------
    ** Record position of BLOCK_LEN field in buffer so that we can
    ** write value after parameter block length has been calculated
    ** ------------------------------------------------------------ */
    block_len_pos = buffer_pos;
    buffer_pos += FSIZ( otaspe_config_rsp_var_type, block_len );

    /* we do not want to include any blocks if the length is explicitly
    ** set to zero */

    if (int_ptr->config_rsp.block[i].block_len != 0)
    {

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
      /* Determine if the R-UIM is available for this NAM */
     if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
     {
       /* Bit Copy the contents of the response data from the R-UIM */
       b_copy(int_ptr->config_rsp.block[i].blk.ruim_rsp_data, 0, otasp_ptr,
              buffer_pos, int_ptr->config_rsp.block[i].block_len * 8);

       buffer_pos += (int_ptr->config_rsp.block[i].block_len * 8);
     }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
     else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
     {
       /* -------------------------------
       ** Bit-pack parameter block data
       ** ------------------------------- */
       switch (int_ptr->config_rsp.block[i].block_id)
       {
        /* --------------------------------
        **  CDMA/Analog NAM Download Block
        ** --------------------------------*/
        case OTASP_CDMA_ANALOG_NAM_BLK:
          /* Pack AMPS first paging channel */
          b_packw( int_ptr->config_rsp.block[i].blk.rtc_cell.firstchp,
                   otasp_ptr, buffer_pos,
                   FSIZ( otaspe_rtc_cell_data_fix_type, firstchp ));
          buffer_pos += FSIZ( otaspe_rtc_cell_data_fix_type, firstchp );

          /* Pack AMPS home SID */
          b_packw( int_ptr->config_rsp.block[i].blk.rtc_cell.home_sid,
                   otasp_ptr, buffer_pos,
                   FSIZ( otaspe_rtc_cell_data_fix_type, home_sid ));
          buffer_pos += FSIZ( otaspe_rtc_cell_data_fix_type, home_sid );

          /* Pack AMPS EX bit */
          b_packb( int_ptr->config_rsp.block[i].blk.rtc_cell.ex,
                   otasp_ptr, buffer_pos,
                   FSIZ( otaspe_rtc_cell_data_fix_type, ex ));
          buffer_pos += FSIZ( otaspe_rtc_cell_data_fix_type, ex );

          /* Pack SCM */
          b_packb( int_ptr->config_rsp.block[i].blk.rtc_cell.scm,
                   otasp_ptr, buffer_pos,
                   FSIZ( otaspe_rtc_cell_data_fix_type, scm ));
          buffer_pos += FSIZ( otaspe_rtc_cell_data_fix_type, scm );

          /* Pack mobile station protocol revision number */
          b_packb( int_ptr->config_rsp.block[i].blk.rtc_cell.mob_p_rev,
                   otasp_ptr, buffer_pos,
                   FSIZ( otaspe_rtc_cell_data_fix_type, mob_p_rev ));
          buffer_pos += FSIZ( otaspe_rtc_cell_data_fix_type, mob_p_rev );

          /* Pack IMSI Class assignment of the mobile station */
          b_packb( int_ptr->config_rsp.block[i].blk.rtc_cell.imsi_class,
                   otasp_ptr, buffer_pos,
                   FSIZ( otaspe_rtc_cell_data_fix_type, imsi_class ));
          buffer_pos += FSIZ( otaspe_rtc_cell_data_fix_type, imsi_class );

          /* Pack number of IMSI address digits */
          b_packb( int_ptr->config_rsp.block[i].blk.rtc_cell.imsi_addr_num,
                   otasp_ptr, buffer_pos,
                   FSIZ( otaspe_rtc_cell_data_fix_type, imsi_addr_num ));
          buffer_pos += FSIZ( otaspe_rtc_cell_data_fix_type, imsi_addr_num );

          /* Pack mobile country code */
          b_packw( int_ptr->config_rsp.block[i].blk.rtc_cell.mcc,
                   otasp_ptr, buffer_pos,
                   FSIZ( otaspe_rtc_cell_data_fix_type, mcc ));
          buffer_pos += FSIZ( otaspe_rtc_cell_data_fix_type, mcc );

          /* Pack 11th and 12th digits of the IMSI */
          b_packb( int_ptr->config_rsp.block[i].blk.rtc_cell.imsi_11_12,
                   otasp_ptr, buffer_pos,
                   FSIZ( otaspe_rtc_cell_data_fix_type, imsi_11_12 ));
          buffer_pos += FSIZ( otaspe_rtc_cell_data_fix_type, imsi_11_12 );

          /* Pack the least significant 10 digits of the IMSI */
          b_packq( int_ptr->config_rsp.block[i].blk.rtc_cell.imsi_s,
                   otasp_ptr, buffer_pos,
                   FSIZ( otaspe_rtc_cell_data_fix_type, imsi_s ));
          buffer_pos += FSIZ( otaspe_rtc_cell_data_fix_type, imsi_s );

          /* Pack access overload class */
          b_packb( int_ptr->config_rsp.block[i].blk.rtc_cell.accolc,
                   otasp_ptr, buffer_pos,
                   FSIZ( otaspe_rtc_cell_data_fix_type, accolc ));
          buffer_pos += FSIZ( otaspe_rtc_cell_data_fix_type, accolc );

          /* Pack local control status */
          b_packb( int_ptr->config_rsp.block[i].blk.rtc_cell.local_control,
                   otasp_ptr, buffer_pos,
                   FSIZ( otaspe_rtc_cell_data_fix_type, local_control ));
          buffer_pos += FSIZ( otaspe_rtc_cell_data_fix_type, local_control );

          /* Pack termination indicator for the home system */
          b_packb( int_ptr->config_rsp.block[i].blk.rtc_cell.mob_term_home,
                   otasp_ptr, buffer_pos,
                   FSIZ( otaspe_rtc_cell_data_fix_type, mob_term_home ));
          buffer_pos += FSIZ( otaspe_rtc_cell_data_fix_type, mob_term_home );

          /* Pack termination indicator for SID roaming */
          b_packb( int_ptr->config_rsp.block[i].blk.rtc_cell.mob_term_for_sid,
                   otasp_ptr, buffer_pos,
                   FSIZ( otaspe_rtc_cell_data_fix_type, mob_term_for_sid ));
          buffer_pos += FSIZ( otaspe_rtc_cell_data_fix_type, mob_term_for_sid );

          /* Pack termination indicator for NID roaming */
          b_packb( int_ptr->config_rsp.block[i].blk.rtc_cell.mob_term_for_nid,
                   otasp_ptr, buffer_pos,
                   FSIZ( otaspe_rtc_cell_data_fix_type, mob_term_for_nid ));
          buffer_pos += FSIZ( otaspe_rtc_cell_data_fix_type, mob_term_for_nid );

          /* Pack maximum stored SID/NID pairs */
          b_packb( int_ptr->config_rsp.block[i].blk.rtc_cell.max_sid_nid,
                   otasp_ptr, buffer_pos,
                   FSIZ( otaspe_rtc_cell_data_fix_type, max_sid_nid ));
          buffer_pos += FSIZ( otaspe_rtc_cell_data_fix_type, max_sid_nid );

          /* Pack number of stored SID/NID pairs */
          b_packb( int_ptr->config_rsp.block[i].blk.rtc_cell.stored_sid_nid,
                   otasp_ptr, buffer_pos,
                   FSIZ( otaspe_rtc_cell_data_fix_type, stored_sid_nid ));
          buffer_pos += FSIZ( otaspe_rtc_cell_data_fix_type, stored_sid_nid );

          /* -----------------------------------------------
          ** Sanity check internal OTASP Data Message length
          ** ----------------------------------------------- */
          if (int_ptr->config_rsp.block[i].blk.rtc_cell.stored_sid_nid >
                OTASPI_SID_NID_MAX)
          {
            return (OTASPX_INV_LEN_S); /*lint !e641 */
          }

          /* Pack SID/NID pairs */
          for (j=0; j < int_ptr->config_rsp.block[i].blk.rtc_cell.stored_sid_nid;
               j++)
          {
            /* Pack system identification */
            b_packw( int_ptr->config_rsp.block[i].blk.rtc_cell.var[j].sid,
                     otasp_ptr, buffer_pos,
                     FSIZ( otaspe_sid_nid_data_var_type, sid ));
            buffer_pos += FSIZ( otaspe_sid_nid_data_var_type, sid );

            /* Pack network identification */
            b_packw( int_ptr->config_rsp.block[i].blk.rtc_cell.var[j].nid,
                     otasp_ptr, buffer_pos,
                     FSIZ( otaspe_sid_nid_data_var_type, nid ));
            buffer_pos += FSIZ( otaspe_sid_nid_data_var_type, nid );
          }
          break;

        /* -----------------------
        ** Mobile Directory Number
        ** ----------------------- */
        case OTASP_MOB_DIR_NUM_BLK:
          /* Pack number of digits */
          b_packb( int_ptr->config_rsp.block[i].blk.mob_dir.n_digits,
                   otasp_ptr, buffer_pos,
                   FSIZ( otaspe_mob_dir_data_fix_type, n_digits ));
          buffer_pos += FSIZ( otaspe_mob_dir_data_fix_type, n_digits );

          /* Pack mobile directory digits */
          for (j=0; j < int_ptr->config_rsp.block[i].blk.mob_dir.n_digits; j++)
          {
            b_packb( int_ptr->config_rsp.block[i].blk.mob_dir.digitn[j],
                     otasp_ptr, buffer_pos,
                     FSIZ( otaspe_mob_dir_data_var_type, digitn ));
            buffer_pos += FSIZ( otaspe_mob_dir_data_var_type, digitn );
          }
          break;

        /* ---------------------------
        ** CDMA NAM Download Data
        ** --------------------------- */
        case OTASP_CDMA_NAM_BLK:
          {
            /* Pack reserved bits */
            b_packb( 0, otasp_ptr, buffer_pos,
                     FSIZ( otaspe_rtc_pcs_data_fix_type, reserved1 ));
            buffer_pos += FSIZ( otaspe_rtc_pcs_data_fix_type, reserved1 );

            /* Pack slotted mode */
            b_packb( int_ptr->config_rsp.block[i].blk.rtc_pcs.slotted_mode,
                     otasp_ptr, buffer_pos,
                     FSIZ( otaspe_rtc_pcs_data_fix_type, slotted_mode ));
            buffer_pos += FSIZ( otaspe_rtc_pcs_data_fix_type, slotted_mode );

            /* Pack reserved bits */
            b_packb( 0, otasp_ptr, buffer_pos,
                     FSIZ( otaspe_rtc_pcs_data_fix_type, reserved2 ));
            buffer_pos += FSIZ( otaspe_rtc_pcs_data_fix_type, reserved2 );

            /* Pack mobile station protocol revision number */
            b_packb( int_ptr->config_rsp.block[i].blk.rtc_pcs.mob_p_rev,
                     otasp_ptr, buffer_pos,
                     FSIZ( otaspe_rtc_pcs_data_fix_type, mob_p_rev ));
            buffer_pos += FSIZ( otaspe_rtc_pcs_data_fix_type, mob_p_rev );

            /* Pack IMSI Class assignment of the mobile station */
            b_packb( int_ptr->config_rsp.block[i].blk.rtc_pcs.imsi_class,
                     otasp_ptr, buffer_pos,
                     FSIZ( otaspe_rtc_pcs_data_fix_type, imsi_class ));
            buffer_pos += FSIZ( otaspe_rtc_pcs_data_fix_type, imsi_class );

            /* Pack number of IMSI address digits */
            b_packb( int_ptr->config_rsp.block[i].blk.rtc_pcs.imsi_addr_num,
                     otasp_ptr, buffer_pos,
                     FSIZ( otaspe_rtc_pcs_data_fix_type, imsi_addr_num ));
            buffer_pos += FSIZ( otaspe_rtc_pcs_data_fix_type, imsi_addr_num );

            /* Pack mobile country code */
            b_packw( int_ptr->config_rsp.block[i].blk.rtc_pcs.mcc,
                     otasp_ptr, buffer_pos,
                     FSIZ( otaspe_rtc_pcs_data_fix_type, mcc ));
            buffer_pos += FSIZ( otaspe_rtc_pcs_data_fix_type, mcc );

            /* Pack 11th and 12th digits of the IMSI */
            b_packb( int_ptr->config_rsp.block[i].blk.rtc_pcs.imsi_11_12,
                     otasp_ptr, buffer_pos,
                     FSIZ( otaspe_rtc_pcs_data_fix_type, imsi_11_12 ));
            buffer_pos += FSIZ( otaspe_rtc_pcs_data_fix_type, imsi_11_12 );

            /* Pack the least significant 10 digits of the IMSI */
            b_packq( int_ptr->config_rsp.block[i].blk.rtc_pcs.imsi_s,
                     otasp_ptr, buffer_pos,
                     FSIZ( otaspe_rtc_pcs_data_fix_type, imsi_s ));
            buffer_pos += FSIZ( otaspe_rtc_pcs_data_fix_type, imsi_s );

            /* Pack access overload class */
            b_packb( int_ptr->config_rsp.block[i].blk.rtc_pcs.accolc,
                     otasp_ptr, buffer_pos,
                     FSIZ( otaspe_rtc_pcs_data_fix_type, accolc ));
            buffer_pos += FSIZ( otaspe_rtc_pcs_data_fix_type, accolc );

            /* Pack local control status */
            b_packb( int_ptr->config_rsp.block[i].blk.rtc_pcs.local_control,
                     otasp_ptr, buffer_pos,
                     FSIZ( otaspe_rtc_pcs_data_fix_type, local_control ));
            buffer_pos += FSIZ( otaspe_rtc_pcs_data_fix_type, local_control );

            /* Pack termination indicator for the home system */
            b_packb( int_ptr->config_rsp.block[i].blk.rtc_pcs.mob_term_home,
                     otasp_ptr, buffer_pos,
                     FSIZ( otaspe_rtc_pcs_data_fix_type, mob_term_home ));
            buffer_pos += FSIZ( otaspe_rtc_pcs_data_fix_type, mob_term_home );

            /* Pack termination indicator for SID roaming */
            b_packb( int_ptr->config_rsp.block[i].blk.rtc_pcs.mob_term_for_sid,
                     otasp_ptr, buffer_pos,
                     FSIZ( otaspe_rtc_pcs_data_fix_type, mob_term_for_sid ));
            buffer_pos += FSIZ( otaspe_rtc_pcs_data_fix_type, mob_term_for_sid );

            /* Pack termination indicator for NID roaming */
            b_packb( int_ptr->config_rsp.block[i].blk.rtc_pcs.mob_term_for_nid,
                     otasp_ptr, buffer_pos,
                     FSIZ( otaspe_rtc_pcs_data_fix_type, mob_term_for_nid ));
            buffer_pos += FSIZ( otaspe_rtc_pcs_data_fix_type, mob_term_for_nid );

            /* Pack maximum stored SID/NID pairs */
            b_packb( int_ptr->config_rsp.block[i].blk.rtc_pcs.max_sid_nid,
                     otasp_ptr, buffer_pos,
                     FSIZ( otaspe_rtc_pcs_data_fix_type, max_sid_nid ));
            buffer_pos += FSIZ( otaspe_rtc_pcs_data_fix_type, max_sid_nid );

            /* Pack number of stored SID/NID pairs */
            b_packb( int_ptr->config_rsp.block[i].blk.rtc_pcs.stored_sid_nid,
                     otasp_ptr, buffer_pos,
                     FSIZ( otaspe_rtc_pcs_data_fix_type, stored_sid_nid ));
            buffer_pos += FSIZ( otaspe_rtc_pcs_data_fix_type, stored_sid_nid );

            /* -----------------------------------------------
            ** Sanity check internal OTASP Data Message length
            ** ----------------------------------------------- */
            if (int_ptr->config_rsp.block[i].blk.rtc_pcs.stored_sid_nid >
                  OTASPI_SID_NID_MAX)
            {
              return (OTASPX_INV_LEN_S); /*lint !e641 */
            }

            /* Pack SID/NID pairs */
            for (j=0; j < int_ptr->config_rsp.block[i].blk.rtc_pcs.stored_sid_nid;
                 j++)
            {
              /* Pack system identification */
              b_packw( int_ptr->config_rsp.block[i].blk.rtc_pcs.var[j].sid,
                       otasp_ptr, buffer_pos,
                       FSIZ( otaspe_sid_nid_data_var_type, sid ));
              buffer_pos += FSIZ( otaspe_sid_nid_data_var_type, sid );

              /* Pack network identification */
              b_packw( int_ptr->config_rsp.block[i].blk.rtc_pcs.var[j].nid,
                       otasp_ptr, buffer_pos,
                       FSIZ( otaspe_sid_nid_data_var_type, nid ));
              buffer_pos += FSIZ( otaspe_sid_nid_data_var_type, nid );
            }
          }
          break;

        /* ---------------------------------------
        ** Preferred Roaming List Dimensions Block
        ** --------------------------------------- */
        case OTASP_PREF_ROAM_LIST_SIZE_BLK:

          break;

        case OTASP_IMSI_T_BLK:
          /* Pack IMSI_T Class Assignment of mobile station */
          b_packb( int_ptr->config_rsp.block[i].blk.imsi_t.imsi_t_class,
                 otasp_ptr, buffer_pos,
                 FSIZ( otaspe_imsi_t_fix_type, imsi_t_class ));
          buffer_pos += FSIZ( otaspe_imsi_t_fix_type, imsi_t_class );

          /* Pack number of IMSI_T address digits */
          b_packb( int_ptr->config_rsp.block[i].blk.imsi_t.imsi_t_addr_num,
                 otasp_ptr, buffer_pos,
                 FSIZ( otaspe_imsi_t_fix_type, imsi_t_addr_num ));
          buffer_pos += FSIZ( otaspe_imsi_t_fix_type, imsi_t_addr_num );

          /* Pack mobile country code */
          b_packw( int_ptr->config_rsp.block[i].blk.imsi_t.imsi_t_mcc,
                   otasp_ptr, buffer_pos,
                   FSIZ( otaspe_imsi_t_fix_type, mcc_t ));
          buffer_pos += FSIZ( otaspe_imsi_t_fix_type, mcc_t );

          /* Pack 11th and 12th digits of the IMSI_T */
          b_packb( int_ptr->config_rsp.block[i].blk.imsi_t.imsi_t_11_12,
                   otasp_ptr, buffer_pos,
                   FSIZ( otaspe_imsi_t_fix_type, imsi_t_11_12 ));
          buffer_pos += FSIZ( otaspe_imsi_t_fix_type, imsi_t_11_12 );

          /* Pack the least significant 10 digits of the IMSI_T */
          b_packq( int_ptr->config_rsp.block[i].blk.imsi_t.imsi_t_s,
                  otasp_ptr, buffer_pos,
                   FSIZ( otaspe_imsi_t_fix_type, imsi_t_s ));
          buffer_pos += FSIZ( otaspe_imsi_t_fix_type, imsi_t_s );

          /* Pack the reserved bits */
          b_packb( 0, otasp_ptr, buffer_pos,
                   FSIZ( otaspe_imsi_t_fix_type, reserved ));
          buffer_pos += FSIZ( otaspe_imsi_t_fix_type, reserved );
          break;

        /**********************************/
        /*  eHRPD IMSI Block               */
        /**********************************/
        case OTASP_EHRPD_IMSI_BLK:
          /* Pack number of mnc digits */
          b_packb( int_ptr->config_rsp.block[i].blk.ehrpd_imsi.ehrpd_mnc_num_digits,
                 otasp_ptr, buffer_pos,
                 FSIZ( otaspe_ehrpd_imsi_fix_type, ehrpd_mnc_num_digits ));
          buffer_pos += FSIZ( otaspe_ehrpd_imsi_fix_type, ehrpd_mnc_num_digits );

          /* Pack number of IMSI digits */
          b_packb( int_ptr->config_rsp.block[i].blk.ehrpd_imsi.ehrpd_imsi_num_digits,
                 otasp_ptr, buffer_pos,
                 FSIZ( otaspe_ehrpd_imsi_fix_type, ehrpd_imsi_num_digits ));
          buffer_pos += FSIZ( otaspe_ehrpd_imsi_fix_type, ehrpd_imsi_num_digits );

          /* Pack the reserved bits */
          b_packb( 0, otasp_ptr, buffer_pos,
                   FSIZ( otaspe_ehrpd_imsi_fix_type, reserved ));
          buffer_pos += FSIZ( otaspe_ehrpd_imsi_fix_type, reserved );

          /* Pack the IMSI digits */
          for (j = 0; j < int_ptr->config_rsp.block[i].blk.ehrpd_imsi.ehrpd_imsi_num_digits; j++)
          {
            b_packb( int_ptr->config_rsp.block[i].blk.ehrpd_imsi.ehrpd_imsi_digits[j],
                   otasp_ptr, buffer_pos,
                   FSIZ( otaspe_ehrpd_imsi_digits_var_type, ehrpd_imsi_digits ));
            buffer_pos += FSIZ( otaspe_ehrpd_imsi_digits_var_type, ehrpd_imsi_digits );
          }

          /* Pack the reserved bits (0 to 4) as needed */
          for (j = 0;
               j < ( ( (int_ptr->config_rsp.block[i].blk.ehrpd_imsi.ehrpd_imsi_num_digits) * 4) % 8);
               j++)
          {
            b_packb( 0, otasp_ptr, buffer_pos,
                   FSIZ( otaspe_ehrpd_imsi_digits_var_type, reserved ));
            buffer_pos += FSIZ( otaspe_ehrpd_imsi_digits_var_type, reserved );
          }
          break;

        default:
          /* ------------------------------------------------------------
          ** If the mobile station does not support a requested parameter
          ** block, the mobile station shall include the parameter block
          ** with the BLOCK_LEN field set to zero.  Nothing to do here;
          ** block_len will be set to zero in calculation below.
          ** ------------------------------------------------------------ */
          break;
       } /* end of switch */
     }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

    }

    /* Pack reserved bits with zeroes if required for   */
    /* byte alignment at the end of the parameter block */
    if ((buffer_pos % 8) != 0)
    {
      reserve_size = 8 - (buffer_pos % 8);
      b_packb( 0, otasp_ptr, buffer_pos, reserve_size );
      buffer_pos += reserve_size;
    }

    /* -------------------------------------
    ** Calculate parameter block byte length
    ** ------------------------------------- */
    block_len = (buffer_pos - block_len_pos - 8) / 8;  /*lint !e834 */

    /* Pack parameter block byte length */
    b_packb( (byte) block_len, otasp_ptr, block_len_pos,
            FSIZ( otaspe_config_rsp_var_type, block_len ));
  } /* end of loop */

  /* Pack the result code for each block */
  for (i=0; i < int_ptr->config_rsp.num_blocks; i++)
  {
    b_packb( int_ptr->config_rsp.result_code[i], otasp_ptr, buffer_pos,
             FSIZ( otaspe_config_rsp_var2_type, result_code ));
    buffer_pos += FSIZ( otaspe_config_rsp_var2_type, result_code );
  }

  /* Calculate otasp_len in bytes for Data Burst Message NUM_FIELDS field */
  *otasp_len = buffer_pos / 8;

  return (status);

} /* xlate_int_config_rsp */

/*===========================================================================

FUNCTION XLATE_INT_DOWNLOAD_RSP

DESCRIPTION
  This function translates the OTASP Download Response Message from
  internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_int_download_rsp
(
  word *otasp_len,
    /* Pointer to OTASP message length in bytes */
  byte *otasp_ptr,
    /* Pointer to (mobile-originated) OTASP message to be transmitted */
  otaspi_rtc_msg_type *int_ptr
    /* Pointer to internal format OTASP message to be translated */
)
{
  word status;
    /* Status returned to calling procedure */
  word i;
    /* Loop index */
  word buffer_pos = 0;
    /* Bit position in OTASP message relative to start of otasp_ptr */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize status */
  status = OTASPX_DONE_S; /*lint !e641 */

  /* Pack OTASP Data Message type */
  b_packb( int_ptr->dload_rsp.msg_type, otasp_ptr, buffer_pos,
           FSIZ( otaspe_download_rsp_fix_type, msg_type ));
  buffer_pos += FSIZ( otaspe_download_rsp_fix_type, msg_type );

  /* Pack number of parameter blocks */
  b_packb( int_ptr->dload_rsp.num_blocks, otasp_ptr,  buffer_pos,
           FSIZ( otaspe_download_rsp_fix_type, num_blocks ));
  buffer_pos += FSIZ( otaspe_download_rsp_fix_type, num_blocks );

  /* -----------------------------------------------
  ** Sanity check internal OTASP Data Message length
  ** ----------------------------------------------- */
  if (int_ptr->dload_rsp.num_blocks > OTASPI_PARAM_BLOCK_MAX)
  {
    return (OTASPX_INV_LEN_S); /*lint !e641 */
  }

  /* Pack variable number of confirmation blocks */
  for (i=0; i < int_ptr->dload_rsp.num_blocks; i++)
  {
    /* Pack parameter block identifier */
    b_packb( int_ptr->dload_rsp.block[i].block_id, otasp_ptr, buffer_pos,
             FSIZ( otaspe_download_rsp_var_type, block_id ));
    buffer_pos += FSIZ( otaspe_download_rsp_var_type, block_id );

    /* Pack download result code */
    b_packb( int_ptr->dload_rsp.block[i].result_code, otasp_ptr, buffer_pos,
             FSIZ( otaspe_download_rsp_var_type, result_code ));
    buffer_pos += FSIZ( otaspe_download_rsp_var_type, result_code );
  }

  /* Calculate otasp_len in bytes for Data Burst Message NUM_FIELDS field */
  *otasp_len = buffer_pos / 8;

  /* Return translation status */
  return (status);

} /* xlate_int_download_rsp */

/*===========================================================================

FUNCTION XLATE_INT_MS_KEY_RSP

DESCRIPTION
  This function translates the OTASP MS Key Response Message from
  internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_int_ms_key_rsp
(
  word *otasp_len,
    /* Pointer to OTASP message length in bytes */
  byte *otasp_ptr,
    /* Pointer to (mobile-originated) OTASP message to be transmitted */
  otaspi_rtc_msg_type *int_ptr
    /* Pointer to internal format OTASP message to be translated */
)
{
  word status;
    /* Status returned to calling procedure */
  word buffer_pos = 0;
    /* Bit position in OTASP message relative to start of otasp_ptr */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize status */
  status = OTASPX_DONE_S; /*lint !e641 */

  /* Pack OTASP Data Message type */
  b_packb( int_ptr->ms_key_rsp.msg_type, otasp_ptr, buffer_pos,
           FSIZ( otaspe_ms_key_rsp_type, msg_type ));
  buffer_pos += FSIZ( otaspe_ms_key_rsp_type, msg_type );

  /* Pack Data result code */
  b_packb( int_ptr->ms_key_rsp.result_code, otasp_ptr,  buffer_pos,
           FSIZ( otaspe_ms_key_rsp_type, result_code ));
  buffer_pos += FSIZ( otaspe_ms_key_rsp_type, result_code );

  /* Calculate otasp_len in bytes for Data Burst Message NUM_FIELDS field */
  *otasp_len = buffer_pos / 8;

  /* Return translation status */
  return (status);

} /* xlate_int_ms_key_rsp */

/*===========================================================================

FUNCTION XLATE_INT_KEY_GEN_RSP

DESCRIPTION
  This function translates the OTASP Key Generation Response Message from
  internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_int_key_gen_rsp
(
  word *otasp_len,
    /* Pointer to OTASP message length in bytes */
  byte *otasp_ptr,
    /* Pointer to (mobile-originated) OTASP message to be transmitted */
  otaspi_rtc_msg_type *int_ptr
    /* Pointer to internal format OTASP message to be translated */
)
{
  word status;
    /* Status returned to calling procedure */
  word buffer_pos = 0;
    /* Bit position in OTASP message relative to start of otasp_ptr */
  byte i;
    /* Loop index */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize status */
  status = OTASPX_DONE_S; /*lint !e641 */

  /* Pack OTASP Data Message type */
  b_packb( int_ptr->key_gen_rsp.msg_type, otasp_ptr, buffer_pos,
           FSIZ( otaspe_key_gen_rsp_type, msg_type ));
  buffer_pos += FSIZ( otaspe_key_gen_rsp_type, msg_type );

  /* Pack Data RESULT_CODE  */
  b_packb( int_ptr->key_gen_rsp.result_code, otasp_ptr,  buffer_pos,
           FSIZ( otaspe_key_gen_rsp_type, result_code ));
  buffer_pos += FSIZ( otaspe_key_gen_rsp_type, result_code );

  /* Pack the length of MS_RESULT field which is set to 64 bytes   */
  b_packd( int_ptr->key_gen_rsp.ms_result_len, otasp_ptr,  buffer_pos,
           FSIZ( otaspe_key_gen_rsp_type, ms_result_len ));
  buffer_pos += FSIZ( otaspe_key_gen_rsp_type, ms_result_len );

  for ( i = 0; i < (int_ptr->key_gen_rsp.ms_result_len/sizeof(dword)); i++)
  {
      b_packd(int_ptr->key_gen_rsp.ms_result[i], otasp_ptr, buffer_pos, 32);
      buffer_pos += 32;
  }

  /* Calculate otasp_len in bytes for Data Burst Message NUM_FIELDS field */
  *otasp_len = buffer_pos / 8;

  /* Return translation status */
  return (status);

} /* xlate_int_key_gen_rsp */

/*===========================================================================

FUNCTION XLATE_REAUTH_RSP

DESCRIPTION
  This function translates the OTASP Re-Authentication Response Message from
  internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_int_reauth_rsp
(
  word *otasp_len,
    /* Pointer to OTASP message length in bytes */
  byte *otasp_ptr,
    /* Pointer to (mobile-originated) OTASP message to be transmitted */
  otaspi_rtc_msg_type *int_ptr
    /* Pointer to internal format OTASP message to be translated */
)
{
  word status;
    /* Status returned to calling procedure */
  word buffer_pos = 0;
    /* Bit position in OTASP message relative to start of otasp_ptr */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize status */
  status = OTASPX_DONE_S; /*lint !e641 */

  /* Pack OTASP Data Message type */
  b_packb( int_ptr->reauth_rsp.msg_type, otasp_ptr, buffer_pos,
           FSIZ( otaspe_reauth_rsp_type, msg_type ));
  buffer_pos += FSIZ( otaspe_reauth_rsp_type, msg_type );

  /* Pack the Authentication Signature value */
  b_packd( int_ptr->reauth_rsp.authr, otasp_ptr, buffer_pos,
               FSIZ( otaspe_reauth_rsp_type, authr ));
  buffer_pos += FSIZ( otaspe_reauth_rsp_type, authr );

  /* Pack the Random Challenge value */
  b_packb( int_ptr->reauth_rsp.randc, otasp_ptr, buffer_pos,
               FSIZ( otaspe_reauth_rsp_type, randc ));
  buffer_pos += FSIZ( otaspe_reauth_rsp_type, randc );

  /* Pack the Call history parameter */
  b_packb( int_ptr->reauth_rsp.count, otasp_ptr, buffer_pos,
               FSIZ( otaspe_reauth_rsp_type, count ));
  buffer_pos += FSIZ( otaspe_reauth_rsp_type, count );

  /* Pack the Authentication data input parameter */
  b_packd( int_ptr->reauth_rsp.auth_data, otasp_ptr, buffer_pos,
               FSIZ( otaspe_reauth_rsp_type, auth_data ));
  buffer_pos += FSIZ( otaspe_reauth_rsp_type, auth_data );

  /* Calculate otasp_len in bytes for Data Burst Message NUM_FIELDS field */
  *otasp_len = buffer_pos / 8;

  /* Return translation status */
  return (status);

} /* xlate_int_authr_rsp */

/*===========================================================================

FUNCTION XLATE_INT_COMMIT_RSP

DESCRIPTION
  This function translates the OTASP Commit Response Message from
  internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_int_commit_rsp
(
  word *otasp_len,
    /* Pointer to OTASP message length in bytes */
  byte *otasp_ptr,
    /* Pointer to (mobile-originated) OTASP message to be transmitted */
  otaspi_rtc_msg_type *int_ptr
    /* Pointer to internal format OTASP message to be translated */
)
{
  word status;
    /* Status returned to calling procedure */
  word buffer_pos = 0;
    /* Bit position in OTASP message relative to start of otasp_ptr */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize status */
  status = OTASPX_DONE_S; /*lint !e641 */

  /* Pack OTASP Data Message type */
  b_packb( int_ptr->commit_rsp.msg_type, otasp_ptr, buffer_pos,
           FSIZ( otaspe_commit_rsp_type, msg_type ));
  buffer_pos += FSIZ( otaspe_commit_rsp_type, msg_type );

  /* Pack Data commit result code */
  b_packb( int_ptr->commit_rsp.result_code, otasp_ptr,  buffer_pos,
           FSIZ( otaspe_commit_rsp_type, result_code ));
  buffer_pos += FSIZ( otaspe_commit_rsp_type, result_code );

  /* Calculate otasp_len in bytes for Data Burst Message NUM_FIELDS field */
  *otasp_len = buffer_pos / 8;

  /* Return translation status */
  return (status);

} /* xlate_int_commit_rsp */

/*===========================================================================

FUNCTION XLATE_INT_CAP_RSP

DESCRIPTION
  This function translates the received OTASP Protocol Capability Response
  Message internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_int_cap_rsp
(
  word *otasp_len,
    /* Pointer to OTASP message length in bytes */
  byte *otasp_ptr,
    /* Pointer to (mobile-originated) OTASP message to be transmitted */
  otaspi_rtc_msg_type *int_ptr
    /* Pointer to internal format OTASP message to be translated */
)
{
  word i;
    /* Loop index */
  word buffer_pos = 0;
    /* Bit position in OTASP message relative to start of otasp_ptr */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Pack OTASP Data Message type */
  b_packb( int_ptr->cap_rsp.msg_type, otasp_ptr, buffer_pos,
           FSIZ( otaspe_prot_cap_rsp_fix_type, msg_type ));
  buffer_pos += FSIZ( otaspe_prot_cap_rsp_fix_type, msg_type );


  if (int_ptr->cap_rsp.msg_type == OTASP_EXT_PROT_CAP_RSP_MSG)
  {
    /* This is the extended Cap Rsp Msg */
    /* Include the otasp_mob_p_rev field */
    b_packb( int_ptr->cap_rsp.otasp_mob_p_rev, otasp_ptr, buffer_pos,
             FSIZ( otaspe_prot_cap_rsp_fix3_type, otasp_mob_p_rev ));
    buffer_pos += FSIZ( otaspe_prot_cap_rsp_fix3_type, otasp_mob_p_rev );
  }


  /* Pack mobile station firmware revision number */
  b_packw( int_ptr->cap_rsp.mob_firm_rev, otasp_ptr, buffer_pos,
           FSIZ( otaspe_prot_cap_rsp_fix_type, mob_firm_rev ));
  buffer_pos += FSIZ( otaspe_prot_cap_rsp_fix_type, mob_firm_rev );

  /* Pack mobile station manufacturer's model number */
  b_packb( int_ptr->cap_rsp.mob_model, otasp_ptr, buffer_pos,
           FSIZ( otaspe_prot_cap_rsp_fix_type, mob_model ));
  buffer_pos += FSIZ( otaspe_prot_cap_rsp_fix_type, mob_model );

  /* Pack number of features */
  b_packb( int_ptr->cap_rsp.num_features, otasp_ptr, buffer_pos,
           FSIZ( otaspe_prot_cap_rsp_fix_type, num_features ));
  buffer_pos += FSIZ( otaspe_prot_cap_rsp_fix_type, num_features );

  /* Pack mobile station feature list */
  for (i=0; i < int_ptr->cap_rsp.num_features; i++)
  {
    /* Pack feature identifier */
    b_packb( int_ptr->cap_rsp.var[i].feature_id, otasp_ptr, buffer_pos,
             FSIZ( otaspe_prot_cap_rsp_var_type, feature_id ));
    buffer_pos += FSIZ( otaspe_prot_cap_rsp_var_type, feature_id );

    /* Pack feature protocol version */
    b_packb( int_ptr->cap_rsp.var[i].feature_p_rev, otasp_ptr, buffer_pos,
             FSIZ( otaspe_prot_cap_rsp_var_type, feature_p_rev ));
    buffer_pos += FSIZ( otaspe_prot_cap_rsp_var_type, feature_p_rev );
  }


  if (int_ptr->cap_rsp.msg_type == OTASP_EXT_PROT_CAP_RSP_MSG)
  {
    /* This is the extended Cap Rsp Msg */

    /* Pack Num Cap Record */
    b_packb( int_ptr->cap_rsp.num_cap_records, otasp_ptr, buffer_pos,
             FSIZ( otaspe_prot_cap_rsp_var3_type, num_cap_records ));
    buffer_pos += FSIZ( otaspe_prot_cap_rsp_var3_type, num_cap_records );

    /* Include the cap Info Record field */
    for (i=0; i < int_ptr->cap_rsp.num_cap_records; i++)
    {
      /* Pack Cap Record Type */
      b_packb( int_ptr->cap_rsp.cap_rec[i].cap_record_type, otasp_ptr, buffer_pos,
               FSIZ( otaspe_prot_cap_rsp_var3_type, cap_record_type ));
      buffer_pos += FSIZ( otaspe_prot_cap_rsp_var3_type, cap_record_type );

      /* Pack Cap Record Length */
      b_packb( int_ptr->cap_rsp.cap_rec[i].cap_record_length, otasp_ptr, buffer_pos,
               FSIZ( otaspe_prot_cap_rsp_var3_type, cap_record_length ));
      buffer_pos += FSIZ( otaspe_prot_cap_rsp_var3_type, cap_record_length );

      if (int_ptr->cap_rsp.cap_rec[i].cap_record_length == 0)
      {
          /* no record to process */
          break;
      }

      switch (int_ptr->cap_rsp.cap_rec[i].cap_record_type)
      {
      case OTASPI_CAP_INFO_OPER_MODE:
        /* Pack the OPER MODE */
        b_packb( int_ptr->cap_rsp.cap_rec[i].info.op_mode, otasp_ptr, buffer_pos,
                  FSIZ( otaspe_prot_cap_rsp_var3_type, op_mode ) );
        buffer_pos += FSIZ( otaspe_prot_cap_rsp_var3_type, op_mode );
        break;

      case OTASPI_CAP_INFO_CDMA_BAND_CLASS:
        /* Pack the band class info */
        b_packw( int_ptr->cap_rsp.cap_rec[i].info.band_class, otasp_ptr, buffer_pos,
                  FSIZ( otaspe_prot_cap_rsp_var3_type, band_class ) );
        buffer_pos += FSIZ( otaspe_prot_cap_rsp_var3_type, band_class );
        break;

      case OTASPI_CAP_INFO_MEID:
        /* Pack the MEID */
        b_packq( int_ptr->cap_rsp.cap_rec[i].info.meid, otasp_ptr, buffer_pos,
                  FSIZ( otaspe_prot_cap_rsp_var3_type, meid ) );
        buffer_pos += FSIZ( otaspe_prot_cap_rsp_var3_type, meid );
        break;

      default:
        M1X_MSG( DCP, LEGACY_ERROR,
          "Unsupported cap rec type %d",
          int_ptr->cap_rsp.cap_rec[i].cap_record_type);
        break;

      }

    } /* for (i=0; i < int_ptr->cap_rsp.num_cap_records; i++) */
  }
  else

  {
    /* Pack the additional length */
    b_packb( int_ptr->cap_rsp.add_length, otasp_ptr, buffer_pos,
             FSIZ( otaspe_prot_cap_rsp_var2_type, add_length ));
    buffer_pos += FSIZ( otaspe_prot_cap_rsp_var2_type, add_length );

    /* Pack the additional bytes */
    for ( i = 0; i < int_ptr->cap_rsp.add_length; i++ )
    {
       b_packb( int_ptr->cap_rsp.addnl_fields[i], otasp_ptr, buffer_pos,
             FSIZ( otaspe_prot_cap_rsp_fix2_type, addnl_byte ));
       buffer_pos += FSIZ( otaspe_prot_cap_rsp_fix2_type, addnl_byte );
    }
  }

  /* Calculate otasp len in bytes for data burst data length field */
  *otasp_len = buffer_pos / 8;

  return (OTASPX_DONE_S); /*lint !e641 */

} /* xlate_int_cap_rsp */

/*===========================================================================

FUNCTION XLATE_INT_SSPR_CONFIG_RSP

DESCRIPTION
  This function translates the received OTASP Protocol SSPR Configuration
  Response Message external to internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_int_sspr_config_rsp
(
  word *otasp_len,
    /* Pointer to OTASP message length in bytes */
  byte *otasp_ptr,
    /* Pointer to (mobile-originated) OTASP message to be transmitted */
  otaspi_rtc_msg_type *int_ptr
    /* Pointer to internal format OTASP message to be translated */
)
{
  word status;
    /* Status returned to calling procedure */
  word buffer_pos = 0;
    /* Bit position in OTASP message relative to start of otasp_ptr */
  word block_len_pos;
    /* Block length bit offset in OTASP message */
  word block_len;
    /* Parameter block data length in bytes */
  word reserve_size;
    /* Number of reserved bits at the end of each parameter block */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  byte* data_ptr;
  /* Loop index */
  word i;
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize status */
  status = OTASPX_DONE_S; /*lint !e641 */

  /* Pack OTASP Data Message type */
  b_packb( int_ptr->sspr_cfg_rsp.msg_type, otasp_ptr, buffer_pos,
           FSIZ( otaspe_sspr_cfg_rsp_fix_type, msg_type ));
  buffer_pos += FSIZ( otaspe_sspr_cfg_rsp_fix_type, msg_type );

  /* Pack block id  */
  b_packb( int_ptr->sspr_cfg_rsp.block_id, otasp_ptr, buffer_pos,
           FSIZ( otaspe_sspr_cfg_rsp_fix_type, block_id ));
  buffer_pos += FSIZ( otaspe_sspr_cfg_rsp_fix_type, block_id );

  /* Pack result code */
  b_packb( int_ptr->sspr_cfg_rsp.result_code, otasp_ptr, buffer_pos,
           FSIZ( otaspe_sspr_cfg_rsp_fix_type, result_code ));
  buffer_pos += FSIZ( otaspe_sspr_cfg_rsp_fix_type, result_code );

  /* ------------------------------------------------------------
  ** Record position of BLOCK_LEN field in buffer so that we can
  ** write value after parameter block length has been calculated
  ** ------------------------------------------------------------ */
  block_len_pos = buffer_pos;
  buffer_pos += FSIZ( otaspe_sspr_cfg_rsp_fix_type, block_len );

  if (int_ptr->sspr_cfg_rsp.block_len != 0)
  {
#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    /* Determine if the R-UIM is available for this NAM */
    if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    {
      /* Bit Copy the contents of the response data from the R-UIM */
      b_copy(int_ptr->sspr_cfg_rsp.block.blk.ruim_rsp_data, 0, otasp_ptr,
             buffer_pos, int_ptr->sspr_cfg_rsp.block_len * 8);

      buffer_pos += (int_ptr->sspr_cfg_rsp.block_len * 8);
    }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
    {
      /* -------------------------------
      ** Bit-pack parameter block data
      ** ------------------------------- */
      switch (int_ptr->sspr_cfg_rsp.block_id)
      {
        /* ----------------------------------------
        **  Preferred Roaming List Dimensions Block
        ** ------------------------------------- */
        case OTASP_CFG_PRL_DIMENSIONS_BLK:
          /* Pack the maximum preferred roaming list size   */
          b_packw(int_ptr->sspr_cfg_rsp.block.blk.prl_size.max_pr_list_size,
             otasp_ptr, buffer_pos, FSIZ( otaspe_rtc_roam_siz_type, max_pr_list_size));
          buffer_pos += FSIZ( otaspe_rtc_roam_siz_type, max_pr_list_size);

          /* Pack the Current Preferred Roaming List size   */
          b_packw(int_ptr->sspr_cfg_rsp.block.blk.prl_size.cur_pr_list_size,
             otasp_ptr, buffer_pos, FSIZ( otaspe_rtc_roam_siz_type, cur_pr_list_size));
          buffer_pos += FSIZ( otaspe_rtc_roam_siz_type, cur_pr_list_size);

          /* Pack the Preferred Roaming List Identification */
          b_packw(int_ptr->sspr_cfg_rsp.block.blk.prl_size.pr_list_id,
             otasp_ptr, buffer_pos, FSIZ( otaspe_rtc_roam_siz_type, pr_list_id));
          buffer_pos += FSIZ( otaspe_rtc_roam_siz_type, pr_list_id);

            /* Pack the reserved bits  */
          b_packb(0, otasp_ptr, buffer_pos,
                    FSIZ( otaspe_rtc_roam_siz_type, reserved));
          buffer_pos += FSIZ( otaspe_rtc_roam_siz_type, reserved);

          /* Pack the number of Acquisition Records  */
          b_packw(int_ptr->sspr_cfg_rsp.block.blk.prl_size.num_acq_recs,
             otasp_ptr, buffer_pos, FSIZ( otaspe_rtc_roam_siz_type, num_acq_recs));
          buffer_pos += FSIZ( otaspe_rtc_roam_siz_type, num_acq_recs);

          /* Pack the number of System Records  */
          b_packw(int_ptr->sspr_cfg_rsp.block.blk.prl_size.num_sys_recs,
             otasp_ptr, buffer_pos, FSIZ( otaspe_rtc_roam_siz_type, num_sys_recs));
          buffer_pos += FSIZ( otaspe_rtc_roam_siz_type, num_sys_recs);

        break;

        case OTASP_CFG_PREF_ROAM_LIST_BLK:

          /* Pack the seven reserved bits with 0  */
          b_packb( 0, otasp_ptr, buffer_pos,
                   FSIZ( otaspe_sspr_prl_fix_type, reserved));
          buffer_pos += FSIZ( otaspe_sspr_prl_fix_type, reserved);

          /* Pack the last segment indicator bit */
          b_packb(int_ptr->sspr_cfg_rsp.block.blk.sspr_prl.last_segment,
             otasp_ptr, buffer_pos, FSIZ( otaspe_sspr_prl_fix_type, last_segment));
          buffer_pos += FSIZ( otaspe_sspr_prl_fix_type, last_segment);

          /* Pack the segment offset */
          b_packw(int_ptr->sspr_cfg_rsp.block.blk.sspr_prl.segment_offset,
             otasp_ptr, buffer_pos, FSIZ( otaspe_sspr_prl_fix_type, segment_offset));
          buffer_pos += FSIZ( otaspe_sspr_prl_fix_type, segment_offset);

          /* Pack the segment size */
            b_packb(int_ptr->sspr_cfg_rsp.block.blk.sspr_prl.segment_size,
             otasp_ptr, buffer_pos, FSIZ( otaspe_sspr_prl_fix_type, segment_size));
          buffer_pos += FSIZ( otaspe_sspr_prl_fix_type, segment_size);

          data_ptr =  int_ptr->sspr_cfg_rsp.block.blk.sspr_prl.bits_ptr;
          if (data_ptr != NULL)
          {
            /* Pack the segment of the roaming list */
            for (i = 0; i < int_ptr->sspr_cfg_rsp.block.blk.sspr_prl.segment_size ; i++)
            {
                b_packb(*data_ptr, otasp_ptr, buffer_pos, 8);
                ++data_ptr;   /* increment the data pointer  */
                buffer_pos += 8;
            }
          }
        break;
        /* ----------------------------------------
        **  Extended Preferred Roaming List Dimensions Block
        ** ------------------------------------- */

        case OTASP_CFG_EXT_PRL_DIMENSIONS_BLK:
          /* Pack the maximum preferred roaming list size   */
          b_packw(int_ptr->sspr_cfg_rsp.block.blk.prl_size.max_pr_list_size,
             otasp_ptr, buffer_pos, FSIZ( otaspe_rtc_ext_roam_siz_fix_type, max_pr_list_size));
          buffer_pos += FSIZ( otaspe_rtc_ext_roam_siz_fix_type, max_pr_list_size);

          /* Pack the Current Preferred Roaming List size   */
          b_packw(int_ptr->sspr_cfg_rsp.block.blk.prl_size.cur_pr_list_size,
             otasp_ptr, buffer_pos, FSIZ( otaspe_rtc_ext_roam_siz_fix_type, cur_pr_list_size));
          buffer_pos += FSIZ( otaspe_rtc_ext_roam_siz_fix_type, cur_pr_list_size);

          /* Pack the Preferred Roaming List Identification */
          b_packw(int_ptr->sspr_cfg_rsp.block.blk.prl_size.pr_list_id,
             otasp_ptr, buffer_pos, FSIZ( otaspe_rtc_ext_roam_siz_fix_type, pr_list_id));
          buffer_pos += FSIZ( otaspe_rtc_ext_roam_siz_fix_type, pr_list_id);

          /* Pack the Protocal revision information */
          b_packb(int_ptr->sspr_cfg_rsp.block.blk.prl_size.cur_sspr_p_rev,
             otasp_ptr, buffer_pos, FSIZ( otaspe_rtc_ext_roam_siz_fix_type, cur_sspr_p_rev));
          buffer_pos += FSIZ( otaspe_rtc_ext_roam_siz_fix_type, cur_sspr_p_rev);

          /*lint -e641*/
          if( int_ptr->sspr_cfg_rsp.block.blk.prl_size.cur_sspr_p_rev == PRL_SSPR_P_REV_1 )
          /*lint +e641*/
          {
            /* Pack the reserved bits  */
            b_packb(0, otasp_ptr, buffer_pos,
                    FSIZ( otaspe_rtc_ext_roam_siz_var_type, reserved));
            buffer_pos += FSIZ( otaspe_rtc_ext_roam_siz_var_type, reserved);
          }

          /* Pack the number of Acquisition Records  */
          b_packw(int_ptr->sspr_cfg_rsp.block.blk.prl_size.num_acq_recs,
             otasp_ptr, buffer_pos, FSIZ( otaspe_rtc_ext_roam_siz_var_type, num_acq_recs));
          buffer_pos += FSIZ( otaspe_rtc_ext_roam_siz_var_type, num_acq_recs);

          /*lint -e641*/
          if( int_ptr->sspr_cfg_rsp.block.blk.prl_size.cur_sspr_p_rev == PRL_SSPR_P_REV_3 )
          /*lint +e641*/
          {
            /* Pack the reserved bits  */
            /*lint -e734*/
            b_packb((byte) int_ptr->sspr_cfg_rsp.block.blk.prl_size.num_comm_subnet_recs,
             otasp_ptr, buffer_pos, FSIZ( otaspe_rtc_ext_roam_siz_var_type, num_comm_subnet_recs));
            /*lint +e734*/

            buffer_pos += FSIZ( otaspe_rtc_ext_roam_siz_var_type, num_comm_subnet_recs);
          }

          /* Pack the number of System Records  */
          b_packw(int_ptr->sspr_cfg_rsp.block.blk.prl_size.num_sys_recs,
             otasp_ptr, buffer_pos, FSIZ( otaspe_rtc_ext_roam_siz_var_type, num_sys_recs));
          buffer_pos += FSIZ( otaspe_rtc_ext_roam_siz_var_type, num_sys_recs);

        break;

        default:
             /* ------------------------------------------------------------
             ** If the mobile station does not support a requested parameter
             ** block, the mobile station shall include the parameter block
             ** with the BLOCK_LEN field set to zero.  Nothing to do here;
             ** block_len will be set to zero in calculation below.
             ** ------------------------------------------------------------ */
        break;
      } /* end of switch */
    }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
  }

  /* Pack reserved bits with zeroes if required for   */
  /* byte alignment at the end of the parameter block */
  if ((buffer_pos % 8) != 0)
  {
    reserve_size = 8 - (buffer_pos % 8);
    b_packb( 0, otasp_ptr, buffer_pos, reserve_size );
    buffer_pos += reserve_size;
  }

  /* -------------------------------------
  ** Calculate parameter block byte length
  ** ------------------------------------- */
  block_len = (buffer_pos - block_len_pos - 8) / 8; /*lint !e834 */

  /* Pack parameter block byte length */
  b_packb( (byte) block_len, otasp_ptr, block_len_pos,
           FSIZ( otaspe_sspr_cfg_rsp_fix_type, block_len ));

  /* Calculate otasp_len in bytes for Data Burst Message NUM_FIELDS field */
  *otasp_len = buffer_pos / 8;

  return (status);

} /* xlate_int_sspr_cfg_rsp */

/*===========================================================================

FUNCTION XLATE_INT_SSPR_DOWNLOAD_RSP

DESCRIPTION
  This function translates the received OTASP Protocol SSPR Download
  Response Message external to internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_int_sspr_download_rsp
(
  word *otasp_len,
    /* Pointer to OTASP message length in bytes */
  byte *otasp_ptr,
    /* Pointer to (mobile-originated) OTASP message to be transmitted */
  otaspi_rtc_msg_type *int_ptr
    /* Pointer to internal format OTASP message to be translated */
)
{
  word status;
    /* Status returned to calling procedure */
  word buffer_pos = 0;
    /* Bit position in OTASP message relative to start of otasp_ptr */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize status */
  status = OTASPX_DONE_S; /*lint !e641 */

  /* Pack OTASP Data Message type */
  b_packb( int_ptr->sspr_dload_rsp.msg_type, otasp_ptr, buffer_pos,
           FSIZ( otaspe_sspr_dload_rsp_fix_type, msg_type ));
  buffer_pos += FSIZ( otaspe_sspr_dload_rsp_fix_type, msg_type );

  /* Pack block id  */
  b_packb( int_ptr->sspr_dload_rsp.block_id, otasp_ptr, buffer_pos,
           FSIZ( otaspe_sspr_dload_rsp_fix_type, block_id ));
  buffer_pos += FSIZ( otaspe_sspr_dload_rsp_fix_type, block_id );

  /* Pack result code */
  b_packb( int_ptr->sspr_dload_rsp.result_code, otasp_ptr, buffer_pos,
           FSIZ( otaspe_sspr_dload_rsp_fix_type, result_code ));
  buffer_pos += FSIZ( otaspe_sspr_dload_rsp_fix_type, result_code );

  /* Pack segment offset */
  b_packw( int_ptr->sspr_dload_rsp.segment_offset, otasp_ptr, buffer_pos,
           FSIZ( otaspe_sspr_dload_rsp_fix_type, segment_offset ));
  buffer_pos += FSIZ( otaspe_sspr_dload_rsp_fix_type, segment_offset );

  /* Pack segment size */
  b_packw( int_ptr->sspr_dload_rsp.segment_size, otasp_ptr, buffer_pos,
           FSIZ( otaspe_sspr_dload_rsp_fix_type, segment_size ));
  buffer_pos += FSIZ( otaspe_sspr_dload_rsp_fix_type, segment_size );

  /* Calculate otasp_len in bytes for Data Burst Message NUM_FIELDS field */
  *otasp_len = buffer_pos / 8;

  return (status);

} /* xlate_int_sspr_dload_rsp */

/*===========================================================================

FUNCTION XLATE_INT_VALIDATION_RSP

DESCRIPTION
  This function translates the received OTASP Protocol Validation
  Response Message external to internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_int_validation_rsp
(
  word *otasp_len,
    /* Pointer to OTASP message length in bytes */
  byte *otasp_ptr,
    /* Pointer to (mobile-originated) OTASP message to be transmitted */
  otaspi_rtc_msg_type *int_ptr
    /* Pointer to internal format OTASP message to be translated */
)
{
  word status;
      /* Status returned to calling procedure */
  word i;
    /* Loop index */
  word buffer_pos = 0;
    /* Bit position in OTASP message relative to start of otasp_ptr */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize status */
  status = OTASPX_DONE_S; /*lint !e641 */

  /* Pack OTASP Data Message type */
  b_packb( int_ptr->validn_rsp.msg_type, otasp_ptr, buffer_pos,
           FSIZ( otaspe_validn_rsp_fix_type, msg_type ));
  buffer_pos += FSIZ( otaspe_validn_rsp_fix_type, msg_type );

  /* Pack the number of blocks */
  b_packb( int_ptr->validn_rsp.num_blocks, otasp_ptr, buffer_pos,
           FSIZ( otaspe_validn_rsp_fix_type, num_blocks ));
  buffer_pos += FSIZ( otaspe_validn_rsp_fix_type, num_blocks );

  /* Pack variable number of confirmation blocks */
  for (i=0; i < int_ptr->validn_rsp.num_blocks; i++)
  {
    /* Pack parameter block identifier */
    b_packb( int_ptr->validn_rsp.block[i].block_id, otasp_ptr, buffer_pos,
             FSIZ( otaspe_validn_rsp_var_type, block_id ));
    buffer_pos += FSIZ( otaspe_validn_rsp_var_type, block_id );

    /* Pack download result code */
    b_packb( int_ptr->dload_rsp.block[i].result_code, otasp_ptr, buffer_pos,
             FSIZ( otaspe_validn_rsp_var_type, result_code ));
    buffer_pos += FSIZ( otaspe_validn_rsp_var_type, result_code );
  }

 /* Calculate otasp_len in bytes for Data Burst Message NUM_FIELDS field */
  *otasp_len = buffer_pos / 8;

  return (status);

} /* xlate_int_validation_rsp */

/*===========================================================================

FUNCTION XLATE_INT_OTAPA_RSP

DESCRIPTION
  This function translates the received OTASP Protocol OTAPA Response
  Message external to internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_int_otapa_rsp
(
  word *otasp_len,
    /* Pointer to OTASP message length in bytes */
  byte *otasp_ptr,
    /* Pointer to (mobile-originated) OTASP message to be transmitted */
  otaspi_rtc_msg_type *int_ptr
    /* Pointer to internal format OTASP message to be translated */
)
{
  word status;
     /* Status returned to calling procedure */
  word buffer_pos = 0;
    /* Bit position in OTASP message relative to start of otasp_ptr */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize status */
  status = OTASPX_DONE_S; /*lint !e641 */

  /* Pack OTASP Data Message type */
  b_packb( int_ptr->otapa_rsp.msg_type, otasp_ptr, buffer_pos,
           FSIZ( otaspe_otapa_rsp_fix_type, msg_type ));
  buffer_pos += FSIZ( otaspe_otapa_rsp_fix_type, msg_type );

  /* Pack the result code */
  b_packb( int_ptr->otapa_rsp.result_code, otasp_ptr, buffer_pos,
           FSIZ( otaspe_otapa_rsp_fix_type, result_code ));
  buffer_pos += FSIZ( otaspe_otapa_rsp_fix_type, result_code );

  /* Pack the seven reserved bits with 0  */
  b_packb( 0, otasp_ptr, buffer_pos,
           FSIZ( otaspe_otapa_rsp_fix_type, reserved));
  buffer_pos += FSIZ( otaspe_otapa_rsp_fix_type, reserved);

  /* Pack the NAM LOCK Indicator */
  b_packb( int_ptr->otapa_rsp.nam_lock_ind, otasp_ptr, buffer_pos,
           FSIZ( otaspe_otapa_rsp_fix_type, nam_lock_ind ));
  buffer_pos += FSIZ( otaspe_otapa_rsp_fix_type, nam_lock_ind );

  if ( int_ptr->otapa_rsp.nam_lock_ind )  {
     /* Pack the SPASM Random Challenge */
     b_packd( int_ptr->otapa_rsp.rand_otapa, otasp_ptr, buffer_pos,
           FSIZ( otaspe_otapa_rsp_var_type, rand_otapa ));
     buffer_pos += FSIZ( otaspe_otapa_rsp_var_type, rand_otapa );
  }

  /* Calculate otasp_len in bytes for Data Burst Message NUM_FIELDS field */
  *otasp_len = buffer_pos / 8;

  return (status);

} /* xlate_int_otapa_rsp */

/*===========================================================================

FUNCTION OTASPX_INT_TO_EXT

DESCRIPTION
  This function translates a message from the internal format used by
  the Main Control task to the external format used by the Transmit
  task.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word otaspx_int_to_ext
(
  word *len_ptr,
    /* Pointer to translated OTASP message length in bytes */
  otaspi_rtc_msg_type *int_msg_ptr,
    /* Pointer to internal mesage to be translated */
  byte *tx_msg_ptr
    /* Pointer to where translated message should be placed */
)
{
  word status;
    /* Status returned to calling procedure */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (int_msg_ptr->gen.msg_type)
  {
    case OTASP_CONFIG_RSP_MSG:
      status = xlate_int_config_rsp( len_ptr, tx_msg_ptr, int_msg_ptr );
      break;

    case OTASP_DOWNLOAD_RSP_MSG:
      status = xlate_int_download_rsp( len_ptr, tx_msg_ptr, int_msg_ptr );
      break;

    case OTASP_MS_KEY_RSP_MSG:
      status = xlate_int_ms_key_rsp( len_ptr, tx_msg_ptr, int_msg_ptr );
      break;

    case OTASP_KEY_GEN_RSP_MSG:
      status = xlate_int_key_gen_rsp( len_ptr, tx_msg_ptr, int_msg_ptr );
      break;

    case OTASP_REAUTH_RSP_MSG:
      status = xlate_int_reauth_rsp( len_ptr, tx_msg_ptr, int_msg_ptr );
      break;

    case OTASP_COMMIT_RSP_MSG:
      status = xlate_int_commit_rsp( len_ptr, tx_msg_ptr, int_msg_ptr );
      break;


  case OTASP_EXT_PROT_CAP_RSP_MSG:      /* interim MEID solution */

  case OTASP_PROT_CAP_RSP_MSG:
      status = xlate_int_cap_rsp( len_ptr, tx_msg_ptr, int_msg_ptr );
      break;

    case OTASP_SSPR_CONFIG_RSP_MSG:
      status = xlate_int_sspr_config_rsp( len_ptr, tx_msg_ptr, int_msg_ptr );
      break;

    case OTASP_SSPR_DOWNLOAD_RSP_MSG:
      status = xlate_int_sspr_download_rsp( len_ptr, tx_msg_ptr, int_msg_ptr );
      break;

    case OTASP_VALIDATION_RSP_MSG:
      status = xlate_int_validation_rsp( len_ptr, tx_msg_ptr, int_msg_ptr );
      break;

    case OTASP_OTAPA_RSP_MSG:
      status = xlate_int_otapa_rsp( len_ptr, tx_msg_ptr, int_msg_ptr );
      break;

    default:
      status = OTASPX_INV_MSG_S; /*lint !e641 */
      break;
  } /* end of switch */

  return (status);

} /* otaspx_int_to_ext */
/*lint +e818*/

