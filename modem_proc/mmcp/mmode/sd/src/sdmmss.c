/*===========================================================================

                   Multi-Mode System Selection  


DESCRIPTION
  This file defines the functionality for MMSS.


  EXTERNALIZED FUNCTIONS



INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2010-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/sdmmss.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/10/13   vs      Adding LTE band 27
09/30/13   sm      Adding LTE band 31
07/24/13   vs      Adding LTE band 30.
03/12/13   sm      Added fix to check zero num_mspl_id   
02/11/13   vs      Adding LTE band 28 and 29.
02/06/12   cl      Fixing eqPRL generation logic to detect new GEO
10/16/12   aj      eliminate duplicates in PLMN selector list
09/18/12   qf      FR2502 - 3GPP scan in MMSS operation
08/24/12   fj      Adding LTE band 26.
08/10/12   fj      Adding LTE band 23.
07/19/12   fj      Added TDS support in sdmmss_encode_tot_tbl().
09/21/11   fj      Fix klocwork errors.
01/02/12   vk      Fixed memory leak issue.
11/02/11   fj      Add support for validate and write BST/TOT into EFS.
10/03/11   aj      delete eqprl in EFS if generation fails
07/24/11   aj      400 acq rec support merge to mainline
05/17/11   xs      Adding LTE band 24-25
03/28/11   xs      Added LTE band 41-43
02/10/11   am      Adding QTF support.
02/03/11   am      Added FEATURE_WCDMA_BC19
11/02/10   gm      Annotation to KW error for "Not a Problem"
09/02/10   aj      add MLPL MCC entries into eq PRL
08/30/10   aj      update max supported size of equivalent PRL/MLPL/PLMNDB
07/30/10   aj      LTE Rel 9 Band class support
07/14/10   mh      Fixed compile warnings
07/02/10   aj      Update the default BST to support LTE BC 11
06/28/10   aj      Update the MLPL decoding as per change in standard
06/14/10   gm      Fix to KW and Lint errors.
06/10/10   ak      Mainlining memory optimization changes.
06/08/10   gm      Fix for Klocwork erros.
05/21/10   aj      update sdmmss_intersect_mlpl_plmn_selector()
05/05/10   aj      change MCC format in BST from binary to BCD
04/20/10   aj      updates to fix MCC MNC handling in PRL and MLPL
04/13/10   aj      change in standard for reserved bits in MLPL record 
04/13/10   aj      memory optimization for equivalent PRL sys rec table cache 
03/28/10   aj      Add support for wildcard MNC
03/24/10   aj      Update to read BST from EFS
03/20/10   aj      Initial version.


===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -esym(766,comdef.h) */
#include "mmcp_variation.h"
#include "comdef.h"    /* Definition for basic types and macros */

#include "sdmmss.h"     /* Definition for MMSS data structures */
#include "sdprl.h"
#include "bit.h"
#include "sdi.h"
#include "sdss.h"
#include "modem_mem.h"
#include "sddbg.h"    /* SD debug macros */
#include "sdefs.h"

#ifdef FEATURE_MMODE_QTF
#error code not present
#endif /* FEATURE_MMODE_QTF */

/* For memscpy */
#include <stringl/stringl.h>

/****************************************************************************

                        PLMN specific functions 

****************************************************************************/
/* <EJECT> */
/*===========================================================================
FUNCTION sdmmss_extract_mcc_mnc_from_bcd_values

DESCRIPTION
  get mcc mnc values in decimal from BCD encoded MCC MNC values

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void sdmmss_extract_mcc_mnc_from_bcd_values( 

  sys_mcc_type             input_hex_mcc,
      /* The input BCD MCC value */

  sys_mnc_type             input_hex_mnc,
      /* The input BCD MNC value */

  sys_mcc_type             * mcc_ptr,
      /* The output decimal  MCC value */

  sys_mnc_type             * mnc_ptr,
      /* The output decimal  MNC value */

  boolean                  * is_pcs
      /* The output is_pcs_included (3 dig MNC) value */
)
{
  sys_mcc_type          mcc_digit_1;
  sys_mcc_type          mcc_digit_2;
  sys_mcc_type          mcc_digit_3;

  sys_mnc_type          mnc_digit_1;
  sys_mnc_type          mnc_digit_2;
  sys_mnc_type          mnc_digit_3;

  /* Read MCC values
  */
  mcc_digit_1 = input_hex_mcc/0x100;
  mcc_digit_2 = (input_hex_mcc - (0x100 * mcc_digit_1)) / 0x10;
  mcc_digit_3 = (input_hex_mcc - (0x100 * mcc_digit_1)) - (0x10 * mcc_digit_2); 
  *mcc_ptr    = 100 * mcc_digit_1 + 10 * mcc_digit_2 + mcc_digit_3;

  /* Read MNC values
  */
  mnc_digit_1 = input_hex_mnc/0x100;
  mnc_digit_2 = (input_hex_mnc - (0x100 * mnc_digit_1)) / 0x10;
  mnc_digit_3 = (input_hex_mnc - (0x100 * mnc_digit_1)) - (0x10 * mnc_digit_2); 

  /* If MNC is set to wildcard value
  */
  if(mnc_digit_1 == 0xF && 
     mnc_digit_2 == 0xF && 
     mnc_digit_3 == 0xF )
  {
    *is_pcs  = TRUE;
    *mnc_ptr = SYS_WILDCARD_MNC;
    return;
  }
  /* If 3rd digit in MNC is set to wildcard value
  */
  if(mnc_digit_3 == 0xF)
  {
    *is_pcs  = FALSE;
    *mnc_ptr = 10 * mnc_digit_1 + mnc_digit_2;
  }
  /* If 3rd digit in MNC is included
  */
  else
  {
    *is_pcs  = TRUE;
    *mnc_ptr = 100 * mnc_digit_1 + 10 * mnc_digit_2 + mnc_digit_3;
  }
}//sdmmss_extract_mcc_mnc_from_bcd_values
/*===========================================================================

FUNCTION sdmmss_map_rat_id_to_sys_mode()

DESCRIPTION
  Map the RAT_ID value in BST table into the correct SYS mode in GWL record. 


DEPENDENCIES
  None

RETURN VALUE
  TRUE if the mapping is successful 
  or 
  FALSE if the mapping is failed.

SIDE EFFECTS
  None.

===========================================================================*/
boolean sdmmss_map_rat_id_to_sys_mode( 

  byte                          rat_id,
    /* The input RAT_ID to be mapped */

  sdmmss_sys_type_e_type       *sys_mode
    /* The output SYS mode */
)
{
  boolean                      is_valid = TRUE;

  switch(rat_id)
  {
    case SDMMSS_BST_RAT_ID_GSM:
      *sys_mode = SDMMSS_SYS_TYPE_GSM;
      break;

    case SDMMSS_BST_RAT_ID_WCDMA:
      *sys_mode = SDMMSS_SYS_TYPE_WCDMA;
      break;

    case SDMMSS_BST_RAT_ID_LTE:
      *sys_mode = SDMMSS_SYS_TYPE_LTE;
      break;
	  
    case SDMMSS_BST_RAT_ID_TDS:
      *sys_mode = SDMMSS_SYS_TYPE_TDS;
      break;
    default:
      is_valid = FALSE;
      SD_MSG_HIGH_1( "Unexpected RAT_ID value=%d",rat_id);
      break;
    
  } // end of switch

  return (is_valid);
  
}
/*===========================================================================

FUNCTION sdmmss_default_gsm_bands()

DESCRIPTION
  Fill in the default GSM bands. 


DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void sdmmss_default_gsm_bands( 
  prli_gwl_generic_recs_type * gwl_rec_ptr
)
{
  gwl_rec_ptr->num_bands             = 9;
  gwl_rec_ptr->rec[0].band           = (byte)PRL_GSM_450;
  gwl_rec_ptr->rec[0].uarfcn_incl    = PRL_UARFCN_NOT_INCL;
  gwl_rec_ptr->rec[1].band           = (byte)PRL_GSM_480;
  gwl_rec_ptr->rec[1].uarfcn_incl    = PRL_UARFCN_NOT_INCL;
  gwl_rec_ptr->rec[2].band           = (byte)PRL_GSM_750;
  gwl_rec_ptr->rec[2].uarfcn_incl    = PRL_UARFCN_NOT_INCL;
  gwl_rec_ptr->rec[3].band           = (byte)PRL_GSM_850;
  gwl_rec_ptr->rec[3].uarfcn_incl    = PRL_UARFCN_NOT_INCL;
  gwl_rec_ptr->rec[4].band           = (byte)PRL_GSM_PGSM_900;
  gwl_rec_ptr->rec[4].uarfcn_incl    = PRL_UARFCN_NOT_INCL;
  gwl_rec_ptr->rec[5].band           = (byte)PRL_GSM_EGSM_900;
  gwl_rec_ptr->rec[5].uarfcn_incl    = PRL_UARFCN_NOT_INCL;
  gwl_rec_ptr->rec[6].band           = (byte)PRL_GSM_RGSM_900;
  gwl_rec_ptr->rec[6].uarfcn_incl    = PRL_UARFCN_NOT_INCL;
  gwl_rec_ptr->rec[7].band           = (byte)PRL_GSM_DCS_1800;
  gwl_rec_ptr->rec[7].uarfcn_incl    = PRL_UARFCN_NOT_INCL;
  gwl_rec_ptr->rec[8].band           = (byte)PRL_GSM_PCS_1900;
  gwl_rec_ptr->rec[8].uarfcn_incl    = PRL_UARFCN_NOT_INCL;

}

/*===========================================================================

FUNCTION sdmmss_default_WCDMA_bands()

DESCRIPTION
  Fill in the default WCDMA bands. 


DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void sdmmss_default_wcdma_bands( 
  prli_gwl_generic_recs_type * gwl_rec_ptr
)
{
  gwl_rec_ptr->num_bands             = 10;
  gwl_rec_ptr->rec[0].band           = (byte)PRL_WCDMA_I_IMT_2000;
  gwl_rec_ptr->rec[0].uarfcn_incl    = PRL_UARFCN_NOT_INCL;
  gwl_rec_ptr->rec[1].band           = (byte)PRL_WCDMA_II_PCS_1900;
  gwl_rec_ptr->rec[1].uarfcn_incl    = PRL_UARFCN_NOT_INCL;
  gwl_rec_ptr->rec[2].band           = (byte)PRL_WCDMA_III_1700;
  gwl_rec_ptr->rec[2].uarfcn_incl    = PRL_UARFCN_NOT_INCL;
  gwl_rec_ptr->rec[3].band           = (byte)PRL_WCDMA_IV_1700;
  gwl_rec_ptr->rec[3].uarfcn_incl    = PRL_UARFCN_NOT_INCL;
  gwl_rec_ptr->rec[4].band           = (byte)PRL_WCDMA_V_850;
  gwl_rec_ptr->rec[4].uarfcn_incl    = PRL_UARFCN_NOT_INCL;
  gwl_rec_ptr->rec[5].band           = (byte)PRL_WCDMA_VI_800;
  gwl_rec_ptr->rec[5].uarfcn_incl    = PRL_UARFCN_NOT_INCL;
  gwl_rec_ptr->rec[6].band           = (byte)PRL_WCDMA_VII_2600;
  gwl_rec_ptr->rec[6].uarfcn_incl    = PRL_UARFCN_NOT_INCL;
  gwl_rec_ptr->rec[7].band           = (byte)PRL_WCDMA_VIII_900;
  gwl_rec_ptr->rec[7].uarfcn_incl    = PRL_UARFCN_NOT_INCL;
  gwl_rec_ptr->rec[8].band           = (byte)PRL_WCDMA_IX_1700;
  gwl_rec_ptr->rec[8].uarfcn_incl    = PRL_UARFCN_NOT_INCL;
  gwl_rec_ptr->rec[9].band           = (byte)PRL_WCDMA_XI_1500;
  gwl_rec_ptr->rec[9].uarfcn_incl    = PRL_UARFCN_NOT_INCL;
  gwl_rec_ptr->rec[10].band           = (byte)PRL_WCDMA_XIX_850;
  gwl_rec_ptr->rec[10].uarfcn_incl    = PRL_UARFCN_NOT_INCL;

}



/*===========================================================================

FUNCTION sdmmss_default_LTE_bands()

DESCRIPTION
  Fill in the default LTE bands. 
  LTE bands defined by standard are bands 1-14, 17- 21, 23-25, 33-43 

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void sdmmss_default_lte_bands( 
  prli_gwl_generic_recs_type * gwl_rec_ptr
)
{

  uint8                  idx          = 0;
  uint8                  band         =(uint8) PRL_LTE_EUTRAN_1;

  for(idx = 0 ; 
      band <= (uint8)PRL_LTE_EUTRAN_14 && idx < PRL_MAX_GWL_GENERIC_BAND;
      idx++)
  {
    gwl_rec_ptr->rec[idx].band         = (uint8)band;
    gwl_rec_ptr->rec[idx].uarfcn_incl  = PRL_UARFCN_NOT_INCL;
    band = band + 1;
  }
  
  for(band =(uint8)PRL_LTE_EUTRAN_17 ;
      band <= (uint8)PRL_LTE_EUTRAN_21 && idx < PRL_MAX_GWL_GENERIC_BAND;
      idx++)
  {
    gwl_rec_ptr->rec[idx].band         = (uint8)band;
    gwl_rec_ptr->rec[idx].uarfcn_incl  = PRL_UARFCN_NOT_INCL;
    band = band + 1;
  }

  for(band =(uint8)PRL_LTE_EUTRAN_23;
      band <= (uint8)PRL_LTE_EUTRAN_43 && idx < PRL_MAX_GWL_GENERIC_BAND;
      idx++)
  {
    gwl_rec_ptr->rec[idx].band         = (uint8)band;
    gwl_rec_ptr->rec[idx].uarfcn_incl  = PRL_UARFCN_NOT_INCL;
    band = band + 1;
  }

  
  gwl_rec_ptr->rec[idx].band         = (uint8)PRL_LTE_EUTRAN_66;
  gwl_rec_ptr->rec[idx].uarfcn_incl  = PRL_UARFCN_NOT_INCL;
  gwl_rec_ptr->num_bands             = idx+1;

  
}

/*===========================================================================

FUNCTION sdmmss_decode_3gpp_bst_tbl()

DESCRIPTION
  Decode the 3GPP band support table. 


DEPENDENCIES
  None

RETURN VALUE
  TRUE if the decoding is successful 
  or 
  FALSE if the decoding is failed.

SIDE EFFECTS
  None.

===========================================================================*/
boolean sdmmss_decode_3gpp_bst_tbl( 

  byte                  *buf_ptr, 
  /* Data to be decoded
  */
  sdmmss_bst_tbl_s_type* bst_ptr
)
{
  //sdmmss_bst_tbl_s_type *      bst_ptr        = sdmmss_bst_ptr(FALSE);
  dword                        offset         = 0;
  int                          i              = 0;  
  int                          j              = 0;
  int                          k              = 0;
  int                          l              = 0;
  int                          bst_rec        = 0;   
  int                          num_rat        = 0; 
  int                          num_band_chan  = 0; 
  byte                         bst_rec_type   = 0; 
  byte                         mcc_incl       = 0;
  byte                         rat_id         = 0;
  byte                         band_chan_rec_type = 0;
  byte                         band           = 0;
  byte                         num_chan       = 0;
  sys_mcc_type                 hex_mcc        = SYS_WILDCARD_MCC;
  sys_mcc_type                 mcc            = SYS_WILDCARD_MCC;
  sys_mnc_type                 mnc            = SYS_WILDCARD_MNC;
  uint16                       bst_rec_length = 0;
  uint16                       band_chan_len  = 0;
  boolean                      is_valid       = TRUE;
  boolean                      ret            = TRUE;
  boolean                      is_pcs         = TRUE;
  sdmmss_sys_type_e_type       sys_mode       = SDMMSS_SYS_TYPE_3GPP2;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* verify against null value of bst pointer
  */
  if(buf_ptr == NULL)
  {
    SD_MSG_HIGH_0("invalid bst");
    return FALSE;
  }
  /* Jump over the 3GPP BST Version field
  */
  offset += SDMMSS_DECODE_SIZE_16_BITS;

  /* Extract NUM_BST_RECs 
  */
  bst_ptr->length = SD_B_UNPACKW(buf_ptr,offset,SDMMSS_DECODE_SIZE_16_BITS);
  offset += SDMMSS_DECODE_SIZE_16_BITS;

  SD_MSG_HIGH_1("NUM_BST_REC = %d",bst_ptr->length);
  
  if (bst_ptr->length == 0 || bst_ptr->length > SDMMSS_BST_MAX_LENGTH)
  {
    return FALSE;
  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = 0, bst_rec = 0;  i < bst_ptr->length; i++)
  {

    if (bst_rec >= SDMMSS_BST_MAX_LENGTH)
    {
      SD_MSG_HIGH_1("bst_rec = %d, out of boundary", bst_rec);
      break;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Extract BST_REC_TYPE
    */
    bst_rec_type = SD_B_UNPACKB(buf_ptr,offset,SDMMSS_DECODE_SIZE_8_BITS);
    offset += SDMMSS_DECODE_SIZE_8_BITS;

    SD_MSG_MED_1("BST_REC_TYPE = %d",bst_rec_type);
  
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Extract the BST_REC_LENGTH field
    */
    bst_rec_length = SD_B_UNPACKW(buf_ptr,offset,SDMMSS_DECODE_SIZE_16_BITS);
    offset += SDMMSS_DECODE_SIZE_16_BITS;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    if (bst_rec_type == (byte)SDMMSS_BST_REC_TYPE_1)
    {
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Jump over the Extension_incl field.
      */
      offset += SDMMSS_DECODE_SIZE_8_BITS;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Extract MCC_incl
      */
      mcc_incl = SD_B_UNPACKB(buf_ptr,offset,SDMMSS_DECODE_SIZE_8_BITS);
      offset += SDMMSS_DECODE_SIZE_8_BITS;

      SD_MSG_HIGH_1("mcc_incl = %d",mcc_incl);
    
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      if (mcc_incl == 0)
      {
        sys_plmn_undefine_plmn_id(&(bst_ptr->bst_tbl[bst_rec].plmn));
      }
      else
      {
        /* Extract MCC 
        */
        hex_mcc = (uint32)SD_B_UNPACKW(buf_ptr,
                                       offset,
                                       SDMMSS_DECODE_SIZE_16_BITS);
        offset += SDMMSS_DECODE_SIZE_16_BITS;
		
        SD_MSG_HIGH_1("BST MCC = %x",hex_mcc);

        /* convert hex MCC values to decimal values
        */
        sdmmss_extract_mcc_mnc_from_bcd_values(hex_mcc,
                                               SYS_WILDCARD_MNC,
                                               &mcc,
                                               &mnc,
                                               &is_pcs);
        /* convert to PLMN encoded format
        */  
        ret = sys_plmn_set_mcc_mnc2(is_pcs, 
                                    mcc, 
                                    mnc, 
                                    &(bst_ptr->bst_tbl[bst_rec].plmn));
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Extract Num_RATs
      */
      bst_ptr->bst_tbl[bst_rec].num_mode = SD_B_UNPACKB(buf_ptr,
                                                        offset,
                                                  SDMMSS_DECODE_SIZE_8_BITS);
      offset += SDMMSS_DECODE_SIZE_8_BITS;

      SD_MSG_HIGH_1("Num_RAT = %d",bst_ptr->bst_tbl[bst_rec].num_mode);

   
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      for (j = 0, num_rat = 0; j < bst_ptr->bst_tbl[bst_rec].num_mode; j ++)
      {
        if (num_rat >= SYS_MAX_NUM_3GPP_RATS)
        {
          SD_MSG_HIGH_1("num_rat = %d, out of boundary", num_rat);
          break;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /* Extract RAT_ID
        */
        rat_id = SD_B_UNPACKB(buf_ptr,offset,SDMMSS_DECODE_SIZE_8_BITS);
        offset += SDMMSS_DECODE_SIZE_8_BITS;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /* Extract Band_channel_REC_TYPE
        */
        band_chan_rec_type = \
          SD_B_UNPACKB(buf_ptr,offset,SDMMSS_DECODE_SIZE_8_BITS);
        offset += SDMMSS_DECODE_SIZE_8_BITS;


        SD_MSG_HIGH_1("band_channel_REC_TYPE = %d",band_chan_rec_type);
    
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /* Extract the band_channel_LENGTH field
        */
        band_chan_len = SD_B_UNPACKW(buf_ptr,offset,SDMMSS_DECODE_SIZE_16_BITS);
        offset += SDMMSS_DECODE_SIZE_16_BITS;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        is_valid = sdmmss_map_rat_id_to_sys_mode(rat_id,
                     &sys_mode);

        /* If the RAT_ID is invalid, go to the next RAT
        */
        if(!is_valid)
        {

          /* Move the offset to the next RAT_ID
          */
          offset += (band_chan_len * 8);

          continue;
        }


        /* if the RAT_ID is valid, set the mode in the num_rat
        */
        bst_ptr->bst_tbl[bst_rec].band_tbl[num_rat].mode = sys_mode;
    
        SD_MSG_HIGH_2("RAT_ID=%d,mode=%d",rat_id,
           bst_ptr->bst_tbl[bst_rec].band_tbl[num_rat].mode);
    
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        if (band_chan_rec_type == (byte)SDMMSS_BAND_CHAN_REC_TYPE_1)
        {
          /* Extract num_band_channel_RECs
          */
          bst_ptr->bst_tbl[bst_rec].band_tbl[num_rat].gwl_rec.num_bands = \
            SD_B_UNPACKB(buf_ptr,offset,SDMMSS_DECODE_SIZE_8_BITS);
          offset += SDMMSS_DECODE_SIZE_8_BITS;

          num_band_chan = 0;
      
          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
          /* Extract band and channel, insert into gwl_rec.
          ** if the num_band_chan is not less than PRL_MAX_GWL_GENERIC_BAND, skip adding
          ** band/channel pairs into gwl_rec.
          */
          for (k = 0; 
               (k < bst_ptr->bst_tbl[bst_rec].band_tbl[num_rat].gwl_rec.num_bands)&&
               (num_band_chan < PRL_MAX_GWL_GENERIC_BAND);
               k++)
          {
            /* Extract Band_ID
            */
            band = SD_B_UNPACKB(buf_ptr,offset,SDMMSS_DECODE_SIZE_8_BITS);
            offset += SDMMSS_DECODE_SIZE_8_BITS;

            /* The LTE band definition in SD is 1 less than the in BST table
            */
            if (sys_mode == SDMMSS_SYS_TYPE_LTE)
            {
              band -= 1;
            }

            /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
            /* Extract num_channels
            */
            num_chan = \
              SD_B_UNPACKB(buf_ptr,offset,SDMMSS_DECODE_SIZE_8_BITS);
            offset += SDMMSS_DECODE_SIZE_8_BITS;

            /* if num_chan is 0, UARFCN is not included.
            */
            if (num_chan == 0)
            {
              bst_ptr->bst_tbl[bst_rec].band_tbl[num_rat].gwl_rec.rec[num_band_chan].band \
                = band;
        
              bst_ptr->bst_tbl[bst_rec].band_tbl[num_rat].gwl_rec.rec[num_band_chan].\
                uarfcn_incl = PRL_UARFCN_NOT_INCL;

              SD_MSG_MED_1("band=%d, uarfcn not included",band);
        
              num_band_chan++;

            }
            /* if num_chan is bigger than 0, insert band/channel pairs into gwl_rec.
            ** if the num_band_chan is not less than PRL_MAX_GWL_GENERIC_BAND, skip adding
            ** band/channel pairs into gwl_rec.
            */
            else
            {
              for(l = 0; (l < num_chan)&&(num_band_chan < PRL_MAX_GWL_GENERIC_BAND);
                  l++, num_band_chan++)
              {
                bst_ptr->bst_tbl[bst_rec].band_tbl[num_rat].gwl_rec.rec[num_band_chan].\
                  band = band;
        
                bst_ptr->bst_tbl[bst_rec].band_tbl[num_rat].gwl_rec.rec[num_band_chan].\
                  uarfcn_incl = PRL_UARFCN_INCL;
          
                bst_ptr->bst_tbl[bst_rec].band_tbl[num_rat].gwl_rec.rec[num_band_chan].\
                  uarfcn = \
                  SD_B_UNPACKW(buf_ptr,offset,SDMMSS_DECODE_SIZE_16_BITS);
                offset += SDMMSS_DECODE_SIZE_16_BITS;

                SD_MSG_MED_2("band=%d, uarfcn=%d",band,
                  bst_ptr->bst_tbl[bst_rec].band_tbl[num_rat].gwl_rec.rec[num_band_chan].uarfcn);

              }
            }
      
          } // end of for loop

          /*if the num_band_chan is not less than PRL_MAX_GWL_GENERIC_BAND, 
          ** print out f3 message.
          */
          if (num_band_chan == PRL_MAX_GWL_GENERIC_BAND)
          {
            SD_MSG_HIGH_0("num_band_chan is more than MAX_GWL_GENERIC_BAND");
          }
      
          bst_ptr->bst_tbl[bst_rec].band_tbl[num_rat].gwl_rec.num_bands = num_band_chan;
          
        } // end of if
        else
        {
          /* band_channel_REC_TYPE != 1, fill in default bands based on the RAT
          */
          SD_MSG_MED_1("Fill in default bands for RAT = %d", bst_ptr->bst_tbl[bst_rec].band_tbl[num_rat].mode);
		  
          switch(bst_ptr->bst_tbl[bst_rec].band_tbl[num_rat].mode)
          {
            case SDMMSS_SYS_TYPE_GSM:
              sdmmss_default_gsm_bands(
                    &(bst_ptr->bst_tbl[bst_rec].band_tbl[num_rat].gwl_rec));

        
              break;

            case SDMMSS_SYS_TYPE_WCDMA:
              sdmmss_default_wcdma_bands(
                    &(bst_ptr->bst_tbl[bst_rec].band_tbl[num_rat].gwl_rec));


              break;

            case SDMMSS_SYS_TYPE_LTE:
              sdmmss_default_lte_bands(
                   &(bst_ptr->bst_tbl[bst_rec].band_tbl[num_rat].gwl_rec));


              break;

            case SDMMSS_SYS_TYPE_3GPP2:
            case SDMMSS_SYS_TYPE_3GPP:
            case SDMMSS_SYS_TYPE_WIMAX:
            case SDMMSS_SYS_TYPE_CDMA:
            case SDMMSS_SYS_TYPE_HDR:
            case SDMMSS_SYS_TYPE_MAX:
            default:
              SD_MSG_HIGH_1( "Unexpected RAT value=%d in BST table",
                            bst_ptr->bst_tbl[bst_rec].band_tbl[num_rat].mode);
              break;
          }

          /* Move the offset to the next band_channel_rec
           */
          offset += (band_chan_len * 8);
      
        }

        num_rat++;
    
      } // end of for loop

      /* Adjust the valid Num_RATs
      */
      bst_ptr->bst_tbl[bst_rec].num_mode = (byte)num_rat;
    
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      bst_rec++;
    
    } // if ( bst_rec_type == (byte)SDMMSS_BST_REC_TYPE_1)
    else
    {
      /* Move the offset to the next BST_REC
      */
      offset += (bst_rec_length * 8);

    }  // if ( bst_rec_type == (byte)SDMMSS_BST_REC_TYPE_1)


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  } // end of for loop (i = 0; i < bst_ptr->length; i++)

  SYS_ARG_NOT_USED(ret);
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Adjust the valid BST RECs
  */
  bst_ptr->length = (uint16) bst_rec;

  if (bst_ptr->length > 0)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
}//sdmmss_decode_3gpp_bst_tbl


/*===========================================================================

FUNCTION sdmmss_encode_3gpp_bst_tbl()

DESCRIPTION
  Encode the 3GPP band support table. 


DEPENDENCIES
  None

RETURN VALUE
  TRUE if the decoding is successful 
  or 
  FALSE if the decoding is failed.

SIDE EFFECTS
  None.

===========================================================================*/
boolean sdmmss_encode_3gpp_bst_tbl( 

  const sd_bst_tbl_s_type *          bst_ptr,
  /* BST info to be encoded.
  */
  byte *                             buf_ptr
  /* Pointer to buffer which stores encoded info.
  */
)
{
  dword                        offset         = 0;
  int                          j              = 0;
  int                          k              = 0;
  int                          bst_rec        = 0;   
  int                          num_rat        = 0; 
  int                          num_bands      = 0; 
  byte                         bst_rec_type   = (byte)SDMMSS_BST_REC_TYPE_1; 
  byte                         band_chan_rec_type = \
                               (byte)SDMMSS_BAND_CHAN_REC_TYPE_1;
  byte                         band           = 0;
  byte                         num_chan       = 0;
  dword                        offset_num_chan= 0;
  uint16                       bst_rec_length = 0;
  dword                        offset_bst_rec_length = 0;
  uint16                       band_chan_len  = 0;
  dword                        offset_band_chan_len = 0;
  boolean                      is_new_band    = TRUE;
  const sd_bst_mode_band_s_type  *gwl_rec_ptr   = NULL;
  byte                         extension =0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* verify against null value of bst pointer and buffer pointer.
  */
  if(bst_ptr == NULL)
  {
    SD_MSG_HIGH_0("bst_ptr is NULL");
    return FALSE;
  }

  if(buf_ptr == NULL)
  {
    SD_MSG_HIGH_0("buf_ptr is NULL");
    return FALSE;
  }

  /* Encode "1" for the 3GPP BST Version
  */
  SD_B_PACKW(1,buf_ptr,offset,SDMMSS_DECODE_SIZE_16_BITS);
  offset += SDMMSS_DECODE_SIZE_16_BITS;

  SD_MSG_MED_1("NUM_BST_REC = %d",bst_ptr->length);
  
  if (bst_ptr->length == 0 || bst_ptr->length > SDMMSS_BST_MAX_LENGTH)
  {
    return FALSE;
  }

  /* Encode NUM_BST_RECs 
  */
  SD_B_PACKW(bst_ptr->length,buf_ptr,offset,SDMMSS_DECODE_SIZE_16_BITS);
  offset += SDMMSS_DECODE_SIZE_16_BITS;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (bst_rec = 0;  bst_rec < bst_ptr->length; bst_rec++)
  {

    if (bst_rec >= SDMMSS_BST_MAX_LENGTH)
    {
      SD_MSG_HIGH_1("bst_rec = %d, out of boundary", bst_rec);
      break;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Encode BST_REC_TYPE
    */
    SD_B_PACKB(bst_rec_type,buf_ptr,offset,SDMMSS_DECODE_SIZE_8_BITS);
    offset += SDMMSS_DECODE_SIZE_8_BITS;

    SD_MSG_MED_1("BST_REC_TYPE = %d",bst_rec_type);
  
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    offset_bst_rec_length = offset;
    bst_rec_length = 0;

    /* Jump over the BST_REC_LENGTH field.
    */
    offset += SDMMSS_DECODE_SIZE_16_BITS;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Jump over the Extension_incl field.
    */
    SD_B_PACKB(extension,buf_ptr,offset,SDMMSS_DECODE_SIZE_8_BITS);
	
    offset += SDMMSS_DECODE_SIZE_8_BITS;
    bst_rec_length += SDMMSS_DECODE_SIZE_8_BITS;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Encode MCC_incl
    */
    SD_B_PACKB(bst_ptr->bst_tbl[bst_rec]->is_mcc_included,
               buf_ptr,offset,SDMMSS_DECODE_SIZE_8_BITS);
    offset += SDMMSS_DECODE_SIZE_8_BITS;
    bst_rec_length += SDMMSS_DECODE_SIZE_8_BITS;

    SD_MSG_MED_1("mcc_incl = %d",bst_ptr->bst_tbl[bst_rec]->is_mcc_included);
    
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    if (bst_ptr->bst_tbl[bst_rec]->is_mcc_included != 0)
    {
      /* Encode MCC 
      */
      uint32 _mcc = bst_ptr->bst_tbl[bst_rec]->mcc;
      uint32 hex_mcc = ((_mcc -_mcc %100)/100)*0x100 +
                ((_mcc %100 - _mcc %10)/10)*0x10 +
                _mcc %10;
      SD_B_PACKW((word)hex_mcc,buf_ptr,offset,
                 SDMMSS_DECODE_SIZE_16_BITS);
      offset += SDMMSS_DECODE_SIZE_16_BITS;
      bst_rec_length += SDMMSS_DECODE_SIZE_16_BITS;

      SD_MSG_MED_1("BST MCC = %x",bst_ptr->bst_tbl[bst_rec]->mcc);

    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Encode Num_RATs
    */
    SD_B_PACKB(bst_ptr->bst_tbl[bst_rec]->num_mode,buf_ptr,offset,
               SDMMSS_DECODE_SIZE_8_BITS);
    offset += SDMMSS_DECODE_SIZE_8_BITS;
    bst_rec_length += SDMMSS_DECODE_SIZE_8_BITS;

    SD_MSG_MED_1("Num_RAT = %d",bst_ptr->bst_tbl[bst_rec]->num_mode);

    num_rat = bst_ptr->bst_tbl[bst_rec]->num_mode;

    if (num_rat > SYS_MAX_NUM_3GPP_RATS)
    {
      SD_MSG_HIGH_1("num_rat = %d, out of boundary", num_rat);
      num_rat = SYS_MAX_NUM_3GPP_RATS;
    }
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    for (j = 0; j < num_rat; j++)
    {
      gwl_rec_ptr = &(bst_ptr->bst_tbl[bst_rec]->band_tbl[j]);
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      SD_MSG_MED_1("RAT_ID = %d",gwl_rec_ptr->mode);

      /* If the RAT_ID is invalid, return FALSE.
      */
      if(gwl_rec_ptr->mode >= SDMMSS_BST_RAT_ID_MAX)
      {
        return FALSE;
      }

      /* Encode RAT_ID
      */
      SD_B_PACKB((byte)(gwl_rec_ptr->mode),buf_ptr,offset,SDMMSS_DECODE_SIZE_8_BITS);
      offset += SDMMSS_DECODE_SIZE_8_BITS;
      bst_rec_length += SDMMSS_DECODE_SIZE_8_BITS;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Encode Band_channel_REC_TYPE
      */
      SD_B_PACKB(band_chan_rec_type,buf_ptr,offset,SDMMSS_DECODE_SIZE_8_BITS);
      offset += SDMMSS_DECODE_SIZE_8_BITS;
      bst_rec_length += SDMMSS_DECODE_SIZE_8_BITS;

      SD_MSG_MED_1("band_channel_REC_TYPE = %d",band_chan_rec_type);
    
      band_chan_len = 0;
      offset_band_chan_len = offset;
      offset += SDMMSS_DECODE_SIZE_16_BITS;
      bst_rec_length += SDMMSS_DECODE_SIZE_16_BITS;
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Encode num_band_channel_RECs
      */
      SD_B_PACKB((byte)(gwl_rec_ptr->gwl_rec.num_bands),buf_ptr,offset,
                 SDMMSS_DECODE_SIZE_8_BITS);
      offset += SDMMSS_DECODE_SIZE_8_BITS;
      bst_rec_length += SDMMSS_DECODE_SIZE_8_BITS;
      band_chan_len += SDMMSS_DECODE_SIZE_8_BITS;

      num_bands = gwl_rec_ptr->gwl_rec.num_bands;
      if (num_bands > PRL_MAX_GWL_GENERIC_BAND)
      {
        SD_MSG_HIGH_1("gwl_rec.num_bands=%d out of boundary",
                    gwl_rec_ptr->gwl_rec.num_bands);
        num_bands = PRL_MAX_GWL_GENERIC_BAND;
      }
      
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Encode band and channel.
      */
      band = gwl_rec_ptr->gwl_rec.rec[0].band;

      for (k = 0; k < num_bands; k++)
      {
        /* Compare band_id */
        if (gwl_rec_ptr->gwl_rec.rec[k].band != band)
        {
          is_new_band = TRUE;
        }

        /* If moves to different band_id, encode num_chan */
        if((k != 0) && (is_new_band == TRUE))
        {
          SD_B_PACKB(num_chan,buf_ptr,offset_num_chan,
                     SDMMSS_DECODE_SIZE_8_BITS);
          //offset += SDMMSS_DECODE_SIZE_8_BITS;
          //bst_rec_length += SDMMSS_DECODE_SIZE_8_BITS;
          //band_chan_len += SDMMSS_DECODE_SIZE_8_BITS;
        }

        if (is_new_band)
        {
          band = gwl_rec_ptr->gwl_rec.rec[k].band;

          /* The LTE band definition in SD is 1 less than the in BST table
          */
          if (gwl_rec_ptr->mode == SDMMSS_BST_RAT_ID_LTE)
          {
            band += 1;
          }

          /* Encode Band_ID
          */
          SD_B_PACKB(band,buf_ptr,offset,SDMMSS_DECODE_SIZE_8_BITS);
          offset += SDMMSS_DECODE_SIZE_8_BITS;
          bst_rec_length += SDMMSS_DECODE_SIZE_8_BITS;
          band_chan_len += SDMMSS_DECODE_SIZE_8_BITS;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
          band = gwl_rec_ptr->gwl_rec.rec[k].band;

          /* Calculate num_channels
          */
          num_chan = 0;
          offset_num_chan = offset;
          offset += SDMMSS_DECODE_SIZE_8_BITS;
          bst_rec_length += SDMMSS_DECODE_SIZE_8_BITS;
          band_chan_len += SDMMSS_DECODE_SIZE_8_BITS;

          is_new_band = FALSE;
        }

        if (gwl_rec_ptr->gwl_rec.rec[k].uarfcn_incl == PRL_UARFCN_NOT_INCL)
        {
          is_new_band = TRUE;
        }
        else
        {
          SD_B_PACKW(gwl_rec_ptr->gwl_rec.rec[k].uarfcn,buf_ptr,offset,
                     SDMMSS_DECODE_SIZE_16_BITS);
          offset += SDMMSS_DECODE_SIZE_16_BITS;
          bst_rec_length += SDMMSS_DECODE_SIZE_16_BITS;
          band_chan_len += SDMMSS_DECODE_SIZE_16_BITS;

          num_chan++;
        }
      } // end of for loop

      /* Encode num_chan */
      if(is_new_band == TRUE)
      {
        SD_B_PACKB(num_chan,buf_ptr,offset_num_chan,
                   SDMMSS_DECODE_SIZE_8_BITS);
        //offset += SDMMSS_DECODE_SIZE_8_BITS;
        //bst_rec_length += SDMMSS_DECODE_SIZE_8_BITS;
        //band_chan_len += SDMMSS_DECODE_SIZE_8_BITS;
      }

      /* Encode band_channel_LENGTH */
      SD_B_PACKW(band_chan_len/8,buf_ptr,offset_band_chan_len,
                 SDMMSS_DECODE_SIZE_16_BITS);

    } // end of for loop (j)

    /* Encode BST_REC_LENGTH */
    SD_B_PACKW(bst_rec_length/8,buf_ptr,offset_bst_rec_length,
               SDMMSS_DECODE_SIZE_16_BITS);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  } // end of for loop (bst_rec = 0; bst_rec < bst_ptr->length; bst_rec++)

  return TRUE;

}//sdmmss_encode_3gpp_bst_tbl

static prl_bst_validation_write_enum_type sdmmss_encode_and_write_bst( 
  const sd_bst_tbl_s_type *          bst_ptr,
    /* pointer to BST info to be encoded.
    */
  sdefs_id_e_type efs_id
)
{

  static byte * temp_bst_ptr       = NULL;
  boolean       is_valid           = FALSE;
  prl_bst_validation_write_enum_type is_written = PRL_BST_VALID_WRITE_NONE;

  if(bst_ptr == NULL)
  {
    SD_MSG_HIGH_0("bst_ptr is NULL");
    return PRL_BST_INVALID;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Allocate memory for binary BST if it is not already done
  */
  if(!temp_bst_ptr)
  {
    temp_bst_ptr =(byte*)modem_mem_alloc( SDMMSS_BST_TBL_MAX_SIZE,
                                         MODEM_MEM_CLIENT_MMODE);

    if(temp_bst_ptr == NULL )
    {
      sys_err_fatal_null_ptr_exception();
    }

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Encode BST to temp buffer.
  */
  is_valid = sdmmss_encode_3gpp_bst_tbl(bst_ptr,temp_bst_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (is_valid)
  {
    /* Write the encoded buffer to EFS.
    */
    is_valid = sdefs_write(efs_id,temp_bst_ptr,SDMMSS_BST_TBL_MAX_SIZE);

    SD_MSG_HIGH_1("BST is_written = %d", is_written);

    if (is_valid)
    {
      is_written = PRL_BST_VALID_WRITTEN_SUCCESS;
    }
    else
    {
      is_written = PRL_BST_VALID_WRITTEN_ERR;
    }
  }
  else
  {
    is_written = PRL_BST_INVALID;
  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  /* Deallocate the memory for the binary BST
  */
  if(temp_bst_ptr != NULL)
  {
    modem_mem_free((void *) temp_bst_ptr, MODEM_MEM_CLIENT_MMODE);
    temp_bst_ptr = NULL;
  }  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return (is_written);

} /* sdmmss_validate_and_write_bst() */
/*===========================================================================

FUNCTION sdmmss_validate_and_write_bst()

DESCRIPTION
  Validate and encode the BST. If it's valid, write into EFS.


DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN prl_bst_validation_write_enum_type sdmmss_validate_and_write_bst( 
  const sd_bst_tbl_s_type *          bst_ptr
    /* pointer to BST info to be encoded.
    */
)
{

  return sdmmss_encode_and_write_bst(bst_ptr,SDEFS_BST_TBL);

} /* sdmmss_validate_and_write_bst() */

/*===========================================================================

FUNCTION sdmmss_validate_and_write_manual_srch_bst()

DESCRIPTION
  Validate and encode the BST. If it's valid, write into EFS.


DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN prl_bst_validation_write_enum_type sdmmss_validate_and_write_manual_srch_bst( 
  const sd_bst_tbl_s_type *          bst_ptr
    /* pointer to BST info to be encoded.
    */
)
{

  return sdmmss_encode_and_write_bst(bst_ptr,SDEFS_MANUAL_SRCH_BST_TBL);
  
} /* sdmmss_validate_and_write_bst() */
#ifdef FEATURE_MMSS_3_1

/*===========================================================================

FUNCTION sdmmss_plmn_selector_ptr

DESCRIPTION
  Return the pointer to plmn_selector list. List of all entries from EHPLMN,
  OPLMN and UPLMN( if ignore uplmn is set to FALSE)


DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
sys_plmn_rat_category_list_s_type  *sdmmss_plmn_selector_ptr( 

       boolean  is_reset
        /* Reset the buffer Pointer
        */
)
{

  static sys_plmn_rat_category_list_s_type * plmn_sel_ptr = NULL;

  /* Clear the buffer PTR after clearing the buffer
  */
  if (is_reset)
  {
    if(plmn_sel_ptr)
    {
      modem_mem_free((void *) plmn_sel_ptr, MODEM_MEM_CLIENT_MMODE);
      plmn_sel_ptr = NULL;
    }
    return NULL;
  }

  if(!plmn_sel_ptr)
  {
    plmn_sel_ptr = (sys_plmn_rat_category_list_s_type *)\
                           modem_mem_alloc( sizeof(sys_plmn_rat_category_list_s_type), 
                                                    MODEM_MEM_CLIENT_MMODE);
    if(plmn_sel_ptr == NULL )
    {
      sys_err_fatal_null_ptr_exception();
    } 
   

    /* Initialize the length to 0
    */
    memset(plmn_sel_ptr,0,sizeof(sys_plmn_rat_category_list_s_type));
    plmn_sel_ptr->length = 0;
  }
  return plmn_sel_ptr;
} /* sdmmss_plmn_selector_ptr() */

/****************************************************************************

                        MSPL specific functions 

****************************************************************************/
/*===========================================================================

FUNCTION sdmmss_mspl_ptr

DESCRIPTION
  Fetches pointer to local cache of MSPL table 

DEPENDENCIES
  

RETURN VALUE 


SIDE EFFECTS
  None.

===========================================================================*/
static sdmmss_mspl_s_type  *sdmmss_mspl_ptr( 

       boolean  is_reset
        /* Reset the buffer Pointer
        */
)
{

  static sdmmss_mspl_s_type * mspl_ptr = NULL;

  /* Clear the buffer PTR after clearing the buffer
  */
  if (is_reset)
  {
  
    SD_MSG_HIGH_1("size %d",sizeof(sdmmss_mspl_s_type));
    if(mspl_ptr)
    {
      if (mspl_ptr->mspl_tbl)
      {
        modem_mem_free((void *)mspl_ptr->mspl_tbl,MODEM_MEM_CLIENT_MMODE);
        mspl_ptr->mspl_tbl = NULL;
      }
      modem_mem_free((void *) mspl_ptr, MODEM_MEM_CLIENT_MMODE);
      mspl_ptr = NULL;
    }
    return NULL;
  }

  if(!mspl_ptr)
  {
    mspl_ptr =(sdmmss_mspl_s_type *)modem_mem_alloc( sizeof(sdmmss_mspl_s_type), 
                                                     MODEM_MEM_CLIENT_MMODE);
    if(mspl_ptr == NULL )
    {
      sys_err_fatal_null_ptr_exception();
    }
  
    memset(mspl_ptr,0,sizeof(sdmmss_mspl_s_type));
    mspl_ptr->num_mspl_id = 0;
    mspl_ptr->mspl_tbl = NULL;
  }
  return mspl_ptr;

} /* sdmmss_mspl_ptr() */

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_get_mspl_rec

DESCRIPTION
  Fetches the MSPL record of given id from the MSPL table 

DEPENDENCIES
  mspl_ptr() should have a valid MSPL.

RETURN VALUE 
  ith mspl record 

SIDE EFFECTS
  None.

===========================================================================*/
static  sdmmss_mspl_tbl_s_type *         sdmmss_get_mspl_rec(

  int        mspl_idx
  /* index */

)
{
  sdmmss_mspl_s_type          *mspl_ptr = sdmmss_mspl_ptr(FALSE);
  int                         j         = 0 ;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( mspl_ptr != NULL );

  for(j = 0; j < mspl_ptr->num_mspl_id ; j++)
  {
    if (mspl_ptr->mspl_tbl[j].mspl_id == mspl_idx) 
    {
      return &(mspl_ptr->mspl_tbl[j]);
    }
  }
  SD_MSG_HIGH_1("unknown mspl idx %d ",mspl_idx);
  return NULL;

} /* sdmmss_get_mspl_rec */

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_extract_mspl_rec

DESCRIPTION
  Decodes each mspl record entry from encoded format into cache

DEPENDENCIES


RETURN VALUE 


SIDE EFFECTS
  None.

===========================================================================*/
static dword sdmmss_extract_mspl_rec
(
  byte                            *buf_ptr,
    /* Pointer to buffer from which to extract the MSPL records.
    */
  dword                           offset,
    /* Offset (in bits) to the beginning of MSPL record which to
    ** extract.
    */
  sdmmss_mspl_recs_s_type         *rec_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_ASSERT(buf_ptr);
  SD_ASSERT(rec_ptr);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract MSPL sys_type
  */
  rec_ptr->sys_type = (sdmmss_sys_type_e_type) \
    SD_B_UNPACKB(buf_ptr,
                 offset,
                 FSIZ(sdmmssi_mspl_recs_s_type, sys_type));
  offset += FSIZ(sdmmssi_mspl_recs_s_type, sys_type);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Extract MSPL pri_class
  */
  rec_ptr->pri_class = (sdmmss_pri_class_e_type) \
    SD_B_UNPACKB(buf_ptr,
                 offset,
                 FSIZ(sdmmssi_mspl_recs_s_type, pri_class));
  offset += FSIZ(sdmmssi_mspl_recs_s_type, pri_class);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Extract MSPL sys_pri
  */
  rec_ptr->sys_pri = (sdmmss_sys_pri_e_type) \
    SD_B_UNPACKB(buf_ptr,
                 offset,
                 FSIZ(sdmmssi_mspl_recs_s_type, sys_pri));
  offset += FSIZ(sdmmssi_mspl_recs_s_type, sys_pri);

  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_EQPRL)
  {
    SD_MSG_LOW_3("sys type %d loc %d pri %d",rec_ptr->sys_type,
                                        rec_ptr->pri_class,
                                          rec_ptr->sys_pri);
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Extract MSPL higher_pri_srch_time
  */
  rec_ptr->hp_srch_time = (sdmmss_hp_srch_timer_e_type) \
    SD_B_UNPACKB(buf_ptr,
                 offset,
                 FSIZ(sdmmssi_mspl_recs_s_type, higher_pri_srch_time));
  offset += FSIZ(sdmmssi_mspl_recs_s_type, higher_pri_srch_time);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Extract MSPL network_cap_incl
  */
  rec_ptr->network_cap_ind = (boolean) \
    SD_B_UNPACKB(buf_ptr,
                 offset,
                 FSIZ(sdmmssi_mspl_recs_s_type, network_cap_ind));
  offset += FSIZ(sdmmssi_mspl_recs_s_type, network_cap_ind);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (rec_ptr->network_cap_ind)
  {
    /* Extract MSPL network_cap 
    */
    rec_ptr->network_cap = (sdmmss_network_cap_e_type) \
      SD_B_UNPACKB(buf_ptr,
                   offset,
                   FSIZ(sdmmssi_mspl_recs_s_type, network_cap));
    offset += FSIZ(sdmmssi_mspl_recs_s_type, network_cap);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return (offset);

}/*sdmmss_extract_mspl_rec*/

/*===========================================================================

FUNCTION sdmmss_extract_mspl_table

DESCRIPTION


DEPENDENCIES


RETURN VALUE 


SIDE EFFECTS
  None.

===========================================================================*/

static void sdmmss_extract_mspl_table
(
  byte                      *buf_ptr
    /* Pointer to buffer from which to extract the MSPL records.
    */
)
{
  dword                        offset = 0;
  sdmmss_mspl_s_type           *mspl_ptr = sdmmss_mspl_ptr(FALSE);
  int                          i = 0;
  int                          j = 0;
 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Extract MSPL size 
  */
  mspl_ptr->mspl_size = SD_B_UNPACKW(buf_ptr,
                                     offset,
                                     FSIZ(sdmmssi_mspl_s_type, mspl_size));
  offset += FSIZ(sdmmssi_mspl_s_type, mspl_size);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Extract current MMSS P REV
   */
  mspl_ptr->cur_sdmmss_p_rev = SD_B_UNPACKB(buf_ptr,
                                   offset,
                                   FSIZ(sdmmssi_mspl_s_type, cur_mmss_p_rev));
  offset += FSIZ(sdmmssi_mspl_s_type, cur_mmss_p_rev);
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Extract MSPL VER ID 
  */

  mspl_ptr->mspl_ver_id= SD_B_UNPACKW(buf_ptr,
                                     offset,
                                     FSIZ(sdmmssi_mspl_s_type, mspl_ver_id));
  offset += FSIZ(sdmmssi_mspl_s_type, mspl_ver_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Extract NUM MSPL ID
  */
  mspl_ptr->num_mspl_id = SD_B_UNPACKB(buf_ptr,
                                   offset,
                                   FSIZ(sdmmssi_mspl_s_type, num_mspl_id));
  offset += FSIZ(sdmmssi_mspl_s_type, num_mspl_id);
  SD_MSG_HIGH_1("EqPRL: Extracting MSPL - num_mspl_id %d",mspl_ptr->num_mspl_id);

  
  /* Check for validity of num_mspl_id 
  */
  if(!INRANGE(mspl_ptr->num_mspl_id , 1, SDMMSS_MAX_NUM_MSPL_ID))
  {
    SD_MSG_HIGH_1("EqPRL: num_mspl_id %d not supported",mspl_ptr->num_mspl_id );
    mspl_ptr->num_mspl_id  = 0;
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_ASSERT(mspl_ptr->mspl_tbl == NULL);
  mspl_ptr->mspl_tbl = (sdmmss_mspl_tbl_s_type*)modem_mem_alloc((mspl_ptr->num_mspl_id *sizeof(sdmmss_mspl_tbl_s_type)),MODEM_MEM_CLIENT_MMODE);
  if(mspl_ptr->mspl_tbl == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }
  memset( mspl_ptr->mspl_tbl,0,(mspl_ptr->num_mspl_id *sizeof(sdmmss_mspl_tbl_s_type)));

  for (i = 0; i < mspl_ptr->num_mspl_id; i++)
  {
    /* Extract MSPL ID 
    */
    mspl_ptr->mspl_tbl[i].mspl_id = SD_B_UNPACKB(buf_ptr,offset,
              FSIZ(sdmmssi_mspl_s_type, mspl_id));
    offset += FSIZ(sdmmssi_mspl_s_type, mspl_id);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Extract num_mspl_recs
    */
    mspl_ptr->mspl_tbl[i].num_mspl_recs = SD_B_UNPACKB(buf_ptr,offset,
              FSIZ(sdmmssi_mspl_s_type, num_mspl_recs));
    offset += FSIZ(sdmmssi_mspl_s_type, num_mspl_recs);

    if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_EQPRL)
    {
      SD_MSG_HIGH_2("mspl id %d num mspl recs %d", mspl_ptr->mspl_tbl[i].mspl_id,
               mspl_ptr->mspl_tbl[i].num_mspl_recs );
    }


    if(mspl_ptr->mspl_tbl[i].num_mspl_recs > SDMMSS_MAX_NUM_MSPL_RECS)
    {
      SD_ERR_1("EqPRL: num_mspl_recs %d not supported",
                                  mspl_ptr->mspl_tbl[i].num_mspl_recs);
      mspl_ptr->num_mspl_id  = 0;
      modem_mem_free((void *)mspl_ptr->mspl_tbl,MODEM_MEM_CLIENT_MMODE);
      mspl_ptr->mspl_tbl = NULL;
      return;
    }
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Extract MSPL records 
    */
    for (j = 0; 
         j < mspl_ptr->mspl_tbl[i].num_mspl_recs && 
         j < SDMMSS_MAX_NUM_MSPL_RECS;
         j++)
    {
      offset = sdmmss_extract_mspl_rec(buf_ptr,offset,
               &(mspl_ptr->mspl_tbl[i].mspl_recs[j]));    

    } // for num of mspl records in each mspl id
  }// for num of mspl ids
}/*sdmmss_extract_mspl_table */



/****************************************************************************

                        MLPL specific functions 

****************************************************************************/
/*===========================================================================

FUNCTION sdmmss_mlpl_ptr

DESCRIPTION
  Fetches pointer to local cache of MLPL table 

DEPENDENCIES
  

RETURN VALUE 


SIDE EFFECTS
  None.

===========================================================================*/
static sdmmss_mlpl_s_type  *sdmmss_mlpl_ptr(
         
         boolean  is_reset
        /* Reset the buffer Pointer
        */
)
{

  static sdmmss_mlpl_s_type * mlpl_ptr = NULL;

  /* Clear the buffer PTR after clearing the buffer
  */
  if (is_reset)
  {
  
    SD_MSG_HIGH_1("mlpl size %d ", sizeof(sdmmss_mlpl_s_type));
    if(mlpl_ptr)
    {
      if (mlpl_ptr->mlpl_recs)
      {
        int k=0;
        for (k=0;k<mlpl_ptr->num_mlpl_recs;k++)
        {
          if( mlpl_ptr->mlpl_recs[k].loc_param_val.loc_tag != NULL )
          {
            modem_mem_free((void *)mlpl_ptr->mlpl_recs[k].loc_param_val.loc_tag,MODEM_MEM_CLIENT_MMODE);
            mlpl_ptr->mlpl_recs[k].loc_param_val.loc_tag= NULL;
          }
        }

        modem_mem_free((void *)mlpl_ptr->mlpl_recs,MODEM_MEM_CLIENT_MMODE);
        mlpl_ptr->mlpl_recs = NULL;
      }
      modem_mem_free((void *) mlpl_ptr, MODEM_MEM_CLIENT_MMODE);
      mlpl_ptr = NULL;
    }
    return NULL;
  }

  if(!mlpl_ptr)
  {
    mlpl_ptr =(sdmmss_mlpl_s_type *)modem_mem_alloc( sizeof(sdmmss_mlpl_s_type), 
                                                     MODEM_MEM_CLIENT_MMODE);
    if(mlpl_ptr == NULL )
    {
      sys_err_fatal_null_ptr_exception();
    }
    
    memset(mlpl_ptr,0,sizeof(sdmmss_mlpl_s_type));
    mlpl_ptr->num_mlpl_recs = 0;
    mlpl_ptr->mlpl_recs = NULL;
  }
  return mlpl_ptr;

} /* sdmmss_mlpl_ptr() */

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_extract_mlpl_rec

DESCRIPTION
  This function extracts a single mlpl record 
  from the mlpl table buffer from the offset provided
  into the cache structure.

DEPENDENCIES


RETURN VALUE 


SIDE EFFECTS
  None.

===========================================================================*/
static sdmmss_error_e_type    sdmmss_extract_mlpl_rec
(
  byte                            *buf_ptr,
    /* Pointer to buffer from which to extract the MLPL records.
    */
  dword                           *offset_ptr,
    /* Offset (in bits) to the beginning of MLPL record which to
    ** extract.
    */
  sdmmss_mlpl_recs_s_type         *rec_ptr
)
{

  int                         i      = 0;
  word                        len    = 0;  
  sdmmss_sys_loc_tag_u_type   *loc_tag; 
  dword                       offset = *offset_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_ASSERT(buf_ptr  != NULL);
  SD_ASSERT(rec_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Extract loc_param_type 
  */
  len = FSIZ(sdmmssi_mlpl_recs_s_type, loc_param_type);
  rec_ptr->loc_param_type = SD_B_UNPACKB(buf_ptr,offset,len);
  offset += len;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* Extract MCC 
  */
  if(SYS_IS_BIT_CONTAIN(rec_ptr->loc_param_type,SDMMSS_LOC_PARAM_TYPE_MCC))
  {
    len =  FSIZ(sdmmssi_loc_param_val_s_type, MCC);
    rec_ptr->loc_param_val.MCC = SD_B_UNPACKW(buf_ptr,offset,len );
    offset += len;
  }

  /* Extract MNC 
  */
  if(SYS_IS_BIT_CONTAIN(rec_ptr->loc_param_type,SDMMSS_LOC_PARAM_TYPE_MNC))
  {
    len = FSIZ(sdmmssi_loc_param_val_s_type, MNC);
    rec_ptr->loc_param_val.MNC = SD_B_UNPACKW(buf_ptr,offset,len);
    offset += len;
  }
  
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_EQPRL)
  {
    SD_MSG_HIGH_3("type %d MCC %x MNC %x", rec_ptr->loc_param_type,
             rec_ptr->loc_param_val.MCC,
             rec_ptr->loc_param_val.MNC);
  }


  /* Extract num_sys_loc_tag
  */
  if (SYS_IS_BIT_CONTAIN(rec_ptr->loc_param_type,
                         SDMMSS_LOC_PARAM_TYPE_LOC_TAG))
  {
    len =  FSIZ(sdmmssi_loc_param_val_s_type, num_sys_loc_tag);
    rec_ptr->loc_param_val.num_sys_loc_tag = SD_B_UNPACKB(buf_ptr,
                                                          offset,
                                                          len );
    offset += len;
	
    SD_MSG_HIGH_1("num_sys_Loc_tag %d",rec_ptr->loc_param_val.num_sys_loc_tag );
    /* Do error checking
    */
    if(rec_ptr->loc_param_val.num_sys_loc_tag > SDMMSS_MAX_NUM_SYS_LOC_TAG)
    {
      SD_ERR_1("Eqprl: num_loc_tags %d not supported",
                   rec_ptr->loc_param_val.num_sys_loc_tag);
      return SDMMSS_ERROR_SDMMSS_MAX_NUM_SYS_LOC_TAG_EXCEED;
    }
  }
  /* Extract sys_loc_tags
  */
  SD_ASSERT(rec_ptr->loc_param_val.loc_tag == NULL);
  if (rec_ptr->loc_param_val.num_sys_loc_tag > 0)
  {
    rec_ptr->loc_param_val.loc_tag =
      (sdmmss_sys_loc_tag_s_type*)modem_mem_alloc(
          (rec_ptr->loc_param_val.num_sys_loc_tag*sizeof(sdmmss_sys_loc_tag_s_type))
          ,MODEM_MEM_CLIENT_MMODE);
    
    if( rec_ptr->loc_param_val.loc_tag != NULL )
    {
      memset( rec_ptr->loc_param_val.loc_tag,
              0,
              (rec_ptr->loc_param_val.num_sys_loc_tag*sizeof(sdmmss_sys_loc_tag_s_type))
            );
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
  }

  for (i = 0; i < (rec_ptr->loc_param_val.num_sys_loc_tag); i++)
  {
    loc_tag = &(rec_ptr->loc_param_val.loc_tag[i].sys_loc_tag);

    /* Extract sys_type 
    */
    len = FSIZ(sdmmssi_sys_loc_tag_s_type, sys_type);
    rec_ptr->loc_param_val.loc_tag[i].sys_type = (sdmmss_sys_type_e_type) \
                                                      SD_B_UNPACKB(buf_ptr,
                                                                   offset,
                                                                   len);
    offset += len;

    /* Extract sys_loc_tag_size
    */
    len = FSIZ(sdmmssi_sys_loc_tag_s_type, sys_loc_tag_size);
    rec_ptr->loc_param_val.loc_tag[i].sys_loc_tag_size = SD_B_UNPACKB(buf_ptr,
                                                                      offset,
                                                                      len);
    offset += len;
	
    SD_MSG_LOW_2("tag num %d sys_type %d",i,
                rec_ptr->loc_param_val.loc_tag[i].sys_type );
    /* Extract sys_loc_tag 
    */
    switch (rec_ptr->loc_param_val.loc_tag[i].sys_type)
    {
      case SDMMSS_SYS_TYPE_3GPP2:
      case SDMMSS_SYS_TYPE_CDMA:
        offset += FSIZ(sdmmssi_sys_loc_tag_cdma_s_type, reserved);
        len = FSIZ(sdmmssi_sys_loc_tag_cdma_s_type, SID);
        loc_tag->sys_loc_tag_cdma.SID = SD_B_UNPACKW(buf_ptr,
                                                    offset,
                                                     len);
        offset += len;

        loc_tag->sys_loc_tag_cdma.NID_start = SD_B_UNPACKW(buf_ptr,
                                                          offset,
                        FSIZ(sdmmssi_sys_loc_tag_cdma_s_type, NID_start));
        offset += FSIZ(sdmmssi_sys_loc_tag_cdma_s_type, NID_start);

        loc_tag->sys_loc_tag_cdma.NID_range = SD_B_UNPACKB(buf_ptr,
                                                          offset,
                        FSIZ(sdmmssi_sys_loc_tag_cdma_s_type, NID_range));
        offset += FSIZ(sdmmssi_sys_loc_tag_cdma_s_type, NID_range);
		
        SD_MSG_HIGH_3("sid %d nid %d range %d ",loc_tag->sys_loc_tag_cdma.SID,
                loc_tag->sys_loc_tag_cdma.NID_start ,
                loc_tag->sys_loc_tag_cdma.NID_range);
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case SDMMSS_SYS_TYPE_3GPP:
      case SDMMSS_SYS_TYPE_GSM:
      case SDMMSS_SYS_TYPE_WCDMA:
      case SDMMSS_SYS_TYPE_LTE:
        len = (FSIZ(sdmmssi_sys_loc_tag_gwl_s_type, PLMN_start))/3;
        loc_tag->sys_loc_tag_gwl.PLMN_start.identity[0] = SD_B_UNPACKB(buf_ptr,
                                                           offset,
                                                           len);
        offset += len;
        loc_tag->sys_loc_tag_gwl.PLMN_start.identity[1] = SD_B_UNPACKB(buf_ptr,
                                                           offset,
                                                           len);
        offset += len;
        loc_tag->sys_loc_tag_gwl.PLMN_start.identity[2] = SD_B_UNPACKB(buf_ptr,
                                                           offset,
                                                           len);
        offset += len;

        loc_tag->sys_loc_tag_gwl.PLMN_range = SD_B_UNPACKB(buf_ptr,
                                                           offset,
                        FSIZ(sdmmssi_sys_loc_tag_gwl_s_type, PLMN_range));
        offset += FSIZ(sdmmssi_sys_loc_tag_gwl_s_type, PLMN_range);
		
        SD_MSG_LOW_4("plmn %x %x %x plmn range %d",
                   loc_tag->sys_loc_tag_gwl.PLMN_start.identity[0],
                   loc_tag->sys_loc_tag_gwl.PLMN_start.identity[1],
                   loc_tag->sys_loc_tag_gwl.PLMN_start.identity[2],
                   loc_tag->sys_loc_tag_gwl.PLMN_range);
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case SDMMSS_SYS_TYPE_HDR:
        loc_tag->sys_loc_tag_hrpd.subnet_common_length =SD_B_UNPACKB(buf_ptr,
                                                                     offset,
                  FSIZ(sdmmssi_sys_loc_tag_hrpd_s_type, subnet_common_length));
        offset += FSIZ(sdmmssi_sys_loc_tag_hrpd_s_type, subnet_common_length);
        memscpy( loc_tag->sys_loc_tag_hrpd.subnet_common,
                 sizeof(loc_tag->sys_loc_tag_hrpd.subnet_common),
                 &(buf_ptr[offset/8]),
                 MIN(SYS_REC_MAX_SUBNET_ID_LENGTH,
                     ((loc_tag->sys_loc_tag_hrpd.subnet_common_length+7)/8)));

        offset += ((loc_tag->sys_loc_tag_hrpd.subnet_common_length+7)/8) *8;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case SDMMSS_SYS_TYPE_WIMAX:
      case SDMMSS_SYS_TYPE_MAX:
      default:
        SD_ERR_1("mlpl invalid sys_type %d", 
                     rec_ptr->loc_param_val.loc_tag[i].sys_type);
        break;

    } // end of switch

  
  } // end of for loop

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract mspl_index 
  */
  len =  FSIZ(sdmmssi_mlpl_recs_s_type, mspl_index);
  rec_ptr->mspl_index = SD_B_UNPACKB(buf_ptr,
                                     offset,
                                     len);
  
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_EQPRL)
  {
    SD_MSG_HIGH_1("mspl idx = %d",rec_ptr->mspl_index);
  }
  offset += len;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  *offset_ptr = offset;

  return SDMMSS_ERROR_NO_ERR;  
}

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_extract_mlpl_table

DESCRIPTION
  This function extracts the MLPL records from the table
  and updates the local cache 

DEPENDENCIES


RETURN VALUE 


SIDE EFFECTS
  None.

===========================================================================*/
static void sdmmss_extract_mlpl_table
(
  byte                      *buf_ptr
    /* Pointer to buffer from which to extract the MLPL records.
    */
)
{
  dword                        offset       = 0;
  sdmmss_mlpl_s_type           *mlpl_ptr    = sdmmss_mlpl_ptr(FALSE);
  int                          i            = 0;
  word                         len          = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_ASSERT(buf_ptr != NULL);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Extract MLPL size 
  */
  len =  FSIZ(sdmmssi_mlpl_s_type, mlpl_size);
  mlpl_ptr->mlpl_size = SD_B_UNPACKW(buf_ptr,offset,len);
  offset += len;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Extract CUR_MMSS_P_REV 
  */
  len = FSIZ(sdmmssi_mlpl_s_type, cur_mmss_p_rev);
  mlpl_ptr->mmss_p_rev = SD_B_UNPACKB(buf_ptr,offset,len);
  offset += len;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Extract MLPL VER ID
   */
  len = FSIZ(sdmmssi_mlpl_s_type, mlpl_ver_id);
  mlpl_ptr->mlpl_ver_id = SD_B_UNPACKW(buf_ptr,offset,len );
  offset += len;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Extract MLPL ID
  */
  len = FSIZ(sdmmssi_mlpl_s_type, mlpl_id);
  mlpl_ptr->mlpl_id = SD_B_UNPACKB(buf_ptr,offset,len );
  offset += len;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Extract num_mlpl_recs 
  */
  len = FSIZ(sdmmssi_mlpl_s_type, num_mlpl_recs);
  mlpl_ptr->num_mlpl_recs = SD_B_UNPACKB(buf_ptr,offset,len );
  offset += len;
  SD_MSG_HIGH_3("EqPRL: Extracting MLPL Ver_id: %d, mlpl_id: %d, num_recs: %d",mlpl_ptr->mlpl_ver_id,
             mlpl_ptr->mlpl_id,
             mlpl_ptr->num_mlpl_recs);

  /* No error checking for MLPL max size , since the size allocated 
  ** does not allow for it to exceed SDMMSS_MAX_NUM_MLPL_RECS
  */
  if ( mlpl_ptr->num_mlpl_recs > 0 )
  {
    int k=0;
    SD_ASSERT(mlpl_ptr->mlpl_recs == NULL);
    mlpl_ptr->mlpl_recs = (sdmmss_mlpl_recs_s_type*)modem_mem_alloc((mlpl_ptr->num_mlpl_recs *sizeof(sdmmss_mlpl_recs_s_type)),MODEM_MEM_CLIENT_MMODE);
    if(mlpl_ptr->mlpl_recs == NULL )
    {
      sys_err_fatal_null_ptr_exception();
    }
    memset( mlpl_ptr->mlpl_recs,0,(mlpl_ptr->num_mlpl_recs *sizeof(sdmmss_mlpl_recs_s_type)));
    for (k=0;k<mlpl_ptr->num_mlpl_recs;k++)
    {
      mlpl_ptr->mlpl_recs[k].loc_param_val.loc_tag = NULL;
    }
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Extract MLPL records 
  */
  for (i = 0; i < mlpl_ptr->num_mlpl_recs; i++)
  {
    if(sdmmss_extract_mlpl_rec(buf_ptr,&offset,&(mlpl_ptr->mlpl_recs[i])) !=
                                                        SDMMSS_ERROR_NO_ERR)
    {
      int k=0;
      SD_ERR_0("EqPRL: error extracting MLPL");
      mlpl_ptr->num_mlpl_recs = 0;
      for (k=0;k<i;k++)
      {
        if( mlpl_ptr->mlpl_recs[k].loc_param_val.loc_tag != NULL )
        {
          modem_mem_free((void *)mlpl_ptr->mlpl_recs[k].loc_param_val.loc_tag,MODEM_MEM_CLIENT_MMODE);
          mlpl_ptr->mlpl_recs[k].loc_param_val.loc_tag= NULL;
        }
      }
      modem_mem_free((void *)mlpl_ptr->mlpl_recs,MODEM_MEM_CLIENT_MMODE);
      mlpl_ptr->mlpl_recs = NULL;
      return;
    }
  } 
}

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_get_mlpl_rec

DESCRIPTION
  Fetches the ith MLPL record from the MLPL table 

DEPENDENCIES
  mlpl_ptr() should have a valid MLPL.

RETURN VALUE 
  ith mlpl record 

SIDE EFFECTS
  None.

===========================================================================*/
static  sdmmss_mlpl_recs_s_type *         sdmmss_get_mlpl_rec(
  uint32        i
  /* index */
)
{
  sdmmss_mlpl_s_type           *mlpl_ptr = sdmmss_mlpl_ptr(FALSE);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( mlpl_ptr != NULL );
  if (i >= mlpl_ptr->num_mlpl_recs) 
  {
    SD_MSG_LOW_1("Invalid mlpl index %d",i);
    return NULL;
  }
  return &(mlpl_ptr->mlpl_recs[i]);

} /* sdmmss_get_mlpl_rec */


/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_get_num_sys_loc_tag

DESCRIPTION
  Fetches the number of sys_loc_tags from the mlpl record.  

DEPENDENCIES


RETURN VALUE 
  int 

SIDE EFFECTS
  None.

===========================================================================*/
static byte         sdmmss_get_num_sys_loc_tag(

    const sdmmss_mlpl_recs_s_type * rec_ptr

)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( rec_ptr != NULL );
  SD_ASSERT(SYS_IS_BIT_CONTAIN(rec_ptr->loc_param_type,
                               SDMMSS_LOC_PARAM_TYPE_LOC_TAG));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return (rec_ptr->loc_param_val.num_sys_loc_tag);

} /* sdmmss_get_num_sys_loc_tag */

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_get_mlpl_gwl_loc_tag_info

DESCRIPTION
  Extracts 3gpp tag info for a specified MLPL record and tag number

DEPENDENCIES


RETURN VALUE 
  3GPP tag info - PLMN start, plmn range

SIDE EFFECTS
  None.

===========================================================================*/
static void         sdmmss_get_mlpl_gwl_loc_tag_info(

    const sdmmss_mlpl_recs_s_type   *rec_ptr,
      /* MLPL record in decoded format
      */

    int                             tag_num,
      /* Tag number from which information needs to be extracted
      */

    sys_plmn_id_s_type             *plmn_start,
      /* output the plmn info in the tag specified by tag_num
      */

    byte                           *plmn_range
      /* output the plmn range info in the tag specified by tag_num
      */
)
{
  sdmmss_sys_loc_tag_gwl_s_type    gwl_tag;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( rec_ptr != NULL );
  SD_ASSERT(SYS_IS_BIT_CONTAIN(rec_ptr->loc_param_type,
                               SDMMSS_LOC_PARAM_TYPE_LOC_TAG));
  if(SYS_IS_BIT_CONTAIN( BM(rec_ptr->loc_param_val.loc_tag[tag_num].sys_type),
                         SDMMSS_SYS_TYPE_3GPP2_ALL_MASK))
  {
    SD_ERR_1("not 3gpp loc tag %d",
                 rec_ptr->loc_param_val.loc_tag[tag_num].sys_type);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  gwl_tag = rec_ptr->loc_param_val.loc_tag[tag_num].sys_loc_tag.sys_loc_tag_gwl;

  *plmn_start = gwl_tag.PLMN_start;
  *plmn_range = gwl_tag.PLMN_range;

}/* sdmmss_get_mlpl_gwl_loc_tag_info */


/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_get_mlpl_rec_sys_type

DESCRIPTION
  Fetches the sys_type for the ith location tag of a specified mlpl record.  

DEPENDENCIES


RETURN VALUE 
  int 

SIDE EFFECTS
  None.

===========================================================================*/
static sdmmss_sys_type_e_type     sdmmss_get_mlpl_rec_sys_type(

    const sdmmss_mlpl_recs_s_type    *rec_ptr,
      /* MLPL record in decoded format
      */

    byte                             log_tag_num
      /* Tag number from which information needs to be extracted
      */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( rec_ptr != NULL );
  SD_ASSERT(SYS_IS_BIT_CONTAIN(rec_ptr->loc_param_type,
                               SDMMSS_LOC_PARAM_TYPE_LOC_TAG));
  SD_ASSERT(log_tag_num < rec_ptr->loc_param_val.num_sys_loc_tag);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return rec_ptr->loc_param_val.loc_tag[log_tag_num].sys_type;

} /* sdmmss_get_mlpl_rec_sys_type */

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_get_mlpl_rec_tag_info

DESCRIPTION
  Fetches the tag info for the ith location tag of a specified mlpl record.  

DEPENDENCIES


RETURN VALUE 
  int 

SIDE EFFECTS
  None.

===========================================================================*/
static sdmmss_sys_loc_tag_s_type     sdmmss_get_mlpl_rec_tag_info(

    const sdmmss_mlpl_recs_s_type   *rec_ptr,
      /* MLPL record in decoded format
      */

    byte                            log_tag_num
      /* Tag number from which information needs to be extracted
      */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( rec_ptr != NULL );
  SD_ASSERT(SYS_IS_BIT_CONTAIN(rec_ptr->loc_param_type,
                               SDMMSS_LOC_PARAM_TYPE_LOC_TAG));
  SD_ASSERT(log_tag_num < rec_ptr->loc_param_val.num_sys_loc_tag);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return rec_ptr->loc_param_val.loc_tag[log_tag_num];

} /* sdmmss_get_mlpl_rec_tag_info */

/****************************************************************************

                        3gpp BST specific functions 

****************************************************************************/
/*===========================================================================

FUNCTION sdmmss_bst_ptr

DESCRIPTION
  Return a pointer to local cache of 3GPP band support table


DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN sdmmss_bst_tbl_s_type *sdmmss_bst_ptr(

       boolean  is_reset
        /* Reset the buffer Pointer
        */
)
{

  static sdmmss_bst_tbl_s_type * bst_ptr = NULL;

  /* Clear the buffer PTR after clearing the buffer
  */
  if (is_reset)
  {
    if(bst_ptr)
    {
      modem_mem_free((void *) bst_ptr, MODEM_MEM_CLIENT_MMODE);
      bst_ptr = NULL;
    }
    return NULL;
  }

  /* Allocate memory for BST if it is not already done
  */
  if(!bst_ptr)
  {
    bst_ptr =(sdmmss_bst_tbl_s_type*)\
                              modem_mem_alloc( sizeof(sdmmss_bst_tbl_s_type), 
                                                     MODEM_MEM_CLIENT_MMODE);

    if(bst_ptr == NULL )
    {
      sys_err_fatal_null_ptr_exception();
    }
   

    /* Initialize BST 
    */
    memset(bst_ptr,0,sizeof(sdmmss_bst_tbl_s_type));
    bst_ptr->length = 0;
  }
  return bst_ptr;

} /* sdmmss_bst_ptr() */

/*===========================================================================

FUNCTION sdmmss_default_3gpp_bst_tbl()

DESCRIPTION
  Fill the default values into the 3GPP band support table. 


DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void sdmmss_default_bst_tbl( void )
{
  sdmmss_bst_tbl_s_type *      bst_ptr = sdmmss_bst_ptr(FALSE);
  boolean                      ret = TRUE; 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_MSG_HIGH_0("EqPRL: Load dflt BST");

  /* 1 BST REC */
  bst_ptr->length              = 1;

  /* Default plmn is 0xFFFFFF */
  ret = sys_plmn_set_mcc_mnc2(TRUE, SYS_WILDCARD_MCC, SYS_WILDCARD_MNC, 
                             &(bst_ptr->bst_tbl[0].plmn));

  /* 3 RATs */
  bst_ptr->bst_tbl[0].num_mode = 3;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* GSM bands */
  bst_ptr->bst_tbl[0].band_tbl[0].mode = SDMMSS_SYS_TYPE_GSM;
  sdmmss_default_gsm_bands(&(bst_ptr->bst_tbl[0].band_tbl[0].gwl_rec));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* WCDMA bands */
  bst_ptr->bst_tbl[0].band_tbl[1].mode = SDMMSS_SYS_TYPE_WCDMA;
  sdmmss_default_wcdma_bands( &(bst_ptr->bst_tbl[0].band_tbl[1].gwl_rec));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* LTE bands */
  bst_ptr->bst_tbl[0].band_tbl[2].mode = SDMMSS_SYS_TYPE_LTE;
  sdmmss_default_lte_bands(&(bst_ptr->bst_tbl[0].band_tbl[2].gwl_rec));

  SYS_ARG_NOT_USED(ret);
  
}

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_update_tot_list 

DESCRIPTION
 Populate default bst entry into SDSR_TOT_LST

DEPENDENCIES
 

RETURN VALUE 
  None

SIDE EFFECTS
  None.

===========================================================================*/
void         sdmmss_update_tot_list ( void )
{
  sdmmss_bst_tbl_s_type        *bst_ptr = sdmmss_bst_ptr(FALSE);
  int                           i       = 0;
  int                           j       = 0;
  prli_acq_rec_type             prl_acq_rec;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  /* populate default bst entry into SDSR_TOT_LST
  */
  sdsr_list_clr(SDSR_TOT_LST);
  for(i= 0; i< bst_ptr->length; i++)
  {
    if(!sys_plmn_id_is_undefined(bst_ptr->bst_tbl[i].plmn))
    {
      continue;
    }

    for(j= 0; j < bst_ptr->bst_tbl[i].num_mode; j++)
    {
      prl_acq_rec.acq_type = PRL_ACQ_REC_GWL_GENERIC;
      prl_acq_rec.type.gwl_generic.num_of_rats = 1;
      switch(bst_ptr->bst_tbl[i].band_tbl[j].mode)
      {
        case SDMMSS_SYS_TYPE_GSM:
          prl_acq_rec.type.gwl_generic.gsm_rec = 
                                     bst_ptr->bst_tbl[i].band_tbl[j].gwl_rec;
          prl_acq_rec.type.gwl_generic.pri_lst[0] =  SYS_SYS_MODE_GSM;
          break;
        case SDMMSS_SYS_TYPE_WCDMA:
          prl_acq_rec.type.gwl_generic.wcdma_rec =
                                     bst_ptr->bst_tbl[i].band_tbl[j].gwl_rec;
          prl_acq_rec.type.gwl_generic.pri_lst[0] =  SYS_SYS_MODE_WCDMA;
          break;
        case SDMMSS_SYS_TYPE_LTE:
          prl_acq_rec.type.gwl_generic.lte_rec =
                                     bst_ptr->bst_tbl[i].band_tbl[j].gwl_rec;
          prl_acq_rec.type.gwl_generic.pri_lst[0] =  SYS_SYS_MODE_LTE;
          break;
        default:
        case SDMMSS_SYS_TYPE_3GPP2:
        case SDMMSS_SYS_TYPE_3GPP:
        case SDMMSS_SYS_TYPE_WIMAX:
        case SDMMSS_SYS_TYPE_CDMA:
        case SDMMSS_SYS_TYPE_HDR:
        case SDMMSS_SYS_TYPE_MAX:
          break;
      } // switch(mode) 
      prl_acq_rec.is_valid             = TRUE;
      prl_acq_rec.plmn.net_select_mode = SD_NET_SELECT_MODE_AUTO;
      prl_acq_rec.plmn.num_lac         = 0;
      prl_acq_rec.plmn.lac             = PRL_WILDCARD_LAC;
      sys_plmn_undefine_plmn_id (&prl_acq_rec.plmn.plmn_id);
      sdprl_acq_rec_app( SDSR_TOT_LST, SDSR_MAX, SDSR_MAX,
                               &prl_acq_rec,
                               SD_NO_CHANGE_SYS_IDX,
                               SDPRL_APPEND_ALL, 
                               ACQ_IDX_WILDCARD);
      
    } //for(num_mode)
  }//for (bst_ptr->length)
} /*  sdmmss_update_tot_list */

/*===========================================================================

FUNCTION sdmmss_extract_bst()

DESCRIPTION
  Read and decode the 3GPP band support table from EFS. 


DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void sdmmss_extract_bst( void )
{
  static byte * efs_bst_ptr       = NULL;
  int32        efs_bst_size       = SDMMSS_BST_TBL_MAX_SIZE;
  boolean      is_valid           = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Allocate memory for binary BST if it is not already done
  */
  if(!efs_bst_ptr)
  {
    efs_bst_ptr =(byte*)modem_mem_alloc( SDMMSS_BST_TBL_MAX_SIZE,
                                         MODEM_MEM_CLIENT_MMODE);

    if(efs_bst_ptr == NULL )
    {
      sys_err_fatal_null_ptr_exception();
    }

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Read binary BST from EFS
  */
  is_valid = sdefs_read(SDEFS_BST_TBL, efs_bst_ptr, &efs_bst_size);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (is_valid)
  {
    /* Decode 3GPP BST table
    */
    sdmmss_bst_tbl_s_type* bst_ptr = sdmmss_bst_ptr(FALSE);
    is_valid = sdmmss_decode_3gpp_bst_tbl( efs_bst_ptr,bst_ptr);
  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If reading/decoding failed, use the default 3GPP BST table
  */
  if (!is_valid)
  {
    /* Fill bst_ptr with the default 3GPP BST table
    */
    sdmmss_default_bst_tbl();
  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  /* If no default bst entry, add the entry to bst table
  */
  sdmmss_create_default_bst_entry(); 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Deallocate the memory for the binary BST
  */
  if(efs_bst_ptr != NULL)
  {
    modem_mem_free((void *) efs_bst_ptr, MODEM_MEM_CLIENT_MMODE);
    efs_bst_ptr = NULL;
  }  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
} /* sdmmss_extract_bst() */

/****************************************************************************

                        File DB specific functions 

****************************************************************************/
/*===========================================================================

FUNCTION sdmmss_filedb_ptr

DESCRIPTION
  Fetches pointer to local cache of PLMN db files .
  If is_reset is set to TRUE, deallocate memory for the dynamic cache.
  If set to FALSE return pointer to cache

DEPENDENCIES
  

RETURN VALUE 
   pointer to cache

SIDE EFFECTS
  None.

===========================================================================*/
static sdmmss_file_s_type  *sdmmss_filedb_ptr( 

       boolean  is_reset
        /* Reset the buffer Pointer
        */
)
{

  static sdmmss_file_s_type * filedb_ptr = NULL;

  /* Clear the buffer PTR after clearing the buffer
  */
  if (is_reset)
  {
    if(filedb_ptr == NULL)
    {
      return NULL;
    }
    if(filedb_ptr->mspl)
    {
      modem_mem_free((void *) filedb_ptr->mspl, MODEM_MEM_CLIENT_MMODE);
      SD_MSG_MED_0("EqPRL:Free mem for mspl");
    }

    if(filedb_ptr->mlpl)
    {
      modem_mem_free((void *)filedb_ptr->mlpl, MODEM_MEM_CLIENT_MMODE);
      SD_MSG_MED_0("EqPRL:Free mem for mlpl");
    }

    if(filedb_ptr->plmndb.imsi)
    {
      modem_mem_free((void *)filedb_ptr->plmndb.imsi, MODEM_MEM_CLIENT_MMODE);
      SD_MSG_MED_0("EqPRL:Free mem for imsi");
    }
    if(filedb_ptr->plmndb.ad)
    {
      modem_mem_free((void *)filedb_ptr->plmndb.ad, MODEM_MEM_CLIENT_MMODE);
      SD_MSG_MED_0("EqPRL:Free mem for admin data");
    }
    if(filedb_ptr->plmndb.oplmnwact)
    {
      modem_mem_free((void *)filedb_ptr->plmndb.oplmnwact, MODEM_MEM_CLIENT_MMODE);
      SD_MSG_MED_0("EqPRL:Free mem for oplmnwact");
    }
    if(filedb_ptr->plmndb.plmnwact)
    {
      modem_mem_free((void *)filedb_ptr->plmndb.plmnwact, MODEM_MEM_CLIENT_MMODE);
      SD_MSG_MED_0("EqPRL:Free mem for plmnwact");
    }

    if(filedb_ptr->plmndb.gsm_plmn)
    {
      modem_mem_free((void *)filedb_ptr->plmndb.gsm_plmn, MODEM_MEM_CLIENT_MMODE);
      SD_MSG_MED_0("EqPRL:Free mem for gsm_plmn");
    }

    if(filedb_ptr->plmndb.usim_ehplmn)
    {
      modem_mem_free((void *)filedb_ptr->plmndb.usim_ehplmn, MODEM_MEM_CLIENT_MMODE);
      SD_MSG_MED_0("EqPRL:Free mem for usim_ehplmn");
    }
    modem_mem_free((void *) filedb_ptr, MODEM_MEM_CLIENT_MMODE);
    filedb_ptr = NULL;
    return NULL;
  }  

  if(!filedb_ptr)
  {
    filedb_ptr =(sdmmss_file_s_type *)modem_mem_alloc( sizeof(sdmmss_file_s_type), 
                                                     MODEM_MEM_CLIENT_MMODE);
    if(filedb_ptr == NULL )
    {
      sys_err_fatal_null_ptr_exception();
    }
   
    memset(filedb_ptr,0,sizeof(sdmmss_file_s_type));
  }
  return filedb_ptr;

} /* sdmmss_filedb_ptr() */

/* <EJECT> */

/*===========================================================================

FUNCTION sdmmss_insert_plmn_sel_rec

DESCRIPTION
  Inserts a record into the  plmn selector list 

DEPENDENCIES
 

RETURN VALUE 
  None

SIDE EFFECTS
  None.

===========================================================================*/
static  void         sdmmss_insert_plmn_sel_rec(

  sys_plmn_id_s_type                       plmn,
  /* PLMN to be inserted */

  sys_access_tech_e_type                   rat,
  /* RAT MASK*/

  sys_detailed_plmn_list_category_e_type   category
  /* HPLMN or PPLMN */

)
{
  sys_plmn_rat_category_list_s_type  * plmn_sel_ptr =  sdmmss_plmn_selector_ptr(FALSE);
  uint64                               pos          = plmn_sel_ptr->length;
  uint64                               i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(plmn_sel_ptr->length >= SYS_MMSS_PLMN_LIST_MAX_LENGTH)
  {
    SD_ERR_0("EqPRL: plmn sel list full");
    return;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check if plmn is already present in PLMN selector list 
  */
  for(i=0; i<plmn_sel_ptr->length;i++)
  {
    if( !memcmp(&(plmn_sel_ptr->info[i].plmn),
                &(plmn),
                sizeof(sys_plmn_id_s_type))
      )
    {
      if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_EQPRL)
      {
        SD_MSG_HIGH_4("EqPRL: PLMN already present: %x %x %x at pos %d",plmn.identity[0],
                                            plmn.identity[1],
                                            plmn.identity[2],
                                            i);
      }
      return;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  plmn_sel_ptr->info[pos].plmn                        = plmn;
  plmn_sel_ptr->info[pos].prl_mcc_act_mask            = SYS_ACT_3GPP_ANY_MASK;
  plmn_sel_ptr->info[pos].geo_mcc_wcard_mnc_act_mask  = SYS_ACT_3GPP_ANY_MASK;
  plmn_sel_ptr->info[pos].geo_mcc_mnc_act_mask        = rat;
  plmn_sel_ptr->info[pos].list_category               = category;
  plmn_sel_ptr->info[pos].act                         = rat;
  plmn_sel_ptr->length++;

  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_EQPRL)
  {
    SD_MSG_HIGH_6("EqPRL: PLMN insert: %x %x %x ;rat %d cat %d list_len %d",plmn.identity[0],
                                            plmn.identity[1],
                                            plmn.identity[2],
                                            rat,
                                            category,
                                        plmn_sel_ptr->length);
  }

} /*  sdmmss_insert_plmn_sel_rec */

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_is_ignore_uplmn

DESCRIPTION
  Reads the EFS item - "/nv/item_files/modem/nas/ignore_uplmn" and determines
  whether to ignore UPLMN or not.

  If the EFS read fails, use the default value of TRUE (which means that 
  UPLMN needs to be ignored)

DEPENDENCIES
  

RETURN VALUE 
  boolean . TRUE - UPLMN should not be used in MMSS 
  FALSE - UPLMN should be taken into consideration

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean         sdmmss_is_ignore_uplmn( void )
{
  boolean    is_valid  = FALSE;
  boolean    ign_uplmn = TRUE;
  int32      size      = sizeof(boolean);

  is_valid = sdefs_read(SDEFS_MMSS_IGNORE_UPLMN, &ign_uplmn, &size);
  if(is_valid)
  {
  
    SD_MSG_HIGH_1("EqPRL:ignore uplmn %d", ign_uplmn);
    return ign_uplmn;
  }
  /* default value of ignore_uplmn is TRUE
  */
  return TRUE;

} /* sdmmss_is_ignore_uplmn */

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_extract_from_plmndb

DESCRIPTION
  Extracts records from the given file and inserts it into the plmn
  selector list.

DEPENDENCIES

RETURN VALUE 
  TRUE - if any records are inserted into the plmn selector list
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean        sdmmss_extract_from_plmndb(
     const  byte                       *data_buf,
      /* File from which the data has to be read 
      */

     uint32                             filesize,
      /* File from which the data has to be read 
      */

     const  byte                        *admin_data,
      /* File from which the admin data has to be read 
      */

     sdmmss_plmn_file_type_e_type       file_type,
      /* Type of file (imsi or wact or no act)
      */

     sys_detailed_plmn_list_category_e_type  cat
      /* category - home or preferred
      */

)
{
  uint32                      i              =  0;
  uint32                      pos            =  0;
  boolean                     is_valid       =  FALSE;
  sys_plmn_id_s_type          plmn;
  sys_access_tech_e_type      access_tech;
  sys_plmn_undefine_plmn_id (&plmn);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(data_buf == NULL)
  {
    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(file_type)
  {
    case SDMMSS_PLMN_FILE_TYPE_IMSI:      
      /* Fill MCC Digit 1 and MCC Digit 2
      */
      plmn.identity[0] = (byte)(( ( data_buf[1] & 0xF0 ) / 0x10 ) +
                                ( ( data_buf[2] & 0x0F ) * 0x10 ));

      /* Fill MCC Digit 3 and MNC Digit 3
      */
      plmn.identity[1] = (byte)(( ( data_buf[2] & 0xF0 ) / 0x10 ) +
                                 ( ( data_buf[4] & 0x0F ) * 0x10 ));

      /** If 2 digits MNC is indicated in EF-AD then set MNC digit 3 to 'F'.
      */
      if (admin_data && ((admin_data[3] & 0x0F) == 0x02))
      {
        plmn.identity[1] |= 0xF0;
      }
      /* Fill MNC Digit 1 and MNC Digit 2
      */
      plmn.identity[2] = data_buf[3];
      sdmmss_insert_plmn_sel_rec(plmn,
                                 SYS_ACT_3GPP_ANY_MASK ,
                                 cat);
      is_valid   = TRUE;
      break;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case SDMMSS_PLMN_FILE_TYPE_WACT:

      for(i= 0;i < filesize / SYS_PLMN_W_ACT_ENTRY_SIZE ;i++)
      {
        /* Each entry has 5 bytes . 1st 3 bytes specify plmn id
        ** Next 2 bytes specify the access technologies
        */
        pos = SYS_PLMN_W_ACT_ENTRY_SIZE * i;
        access_tech = SYS_ACT_NONE_MASK;

        /* Get plmn info from 1st 3 bytes
        */
        memscpy( &plmn, sizeof(plmn), data_buf+pos, sizeof(sys_plmn_id_s_type) );
        if(sys_plmn_id_is_undefined(plmn))
        {
          continue;
        }
        /* Get access tech info which is in 4th and 5th byte
        */
        if(data_buf[pos+3] == 0 && data_buf[pos+4] == 0)
        {
          /* If all the ACT bits are set to 0, it implies that 
          ** all ACTs are supported 
          */
          access_tech = SYS_ACT_3GPP_ANY_MASK;
        }
        else
        {
          /* if some of the ACT bits are non zero, selectively determine
          ** which ACTs are supported
          */
          if(data_buf[pos+3] & SDMMSS_LTE_ACT_BIT)
          {
            access_tech = SD_BIT_ADD(access_tech, SYS_ACT_LTE_MASK);
          }
          if(data_buf[pos+3] & SDMMSS_WCDMA_ACT_BIT)
          {
            access_tech = SD_BIT_ADD(access_tech, SYS_ACT_UMTS_MASK);
          }
          if(data_buf[pos+4] & SDMMSS_GSM_ACT_BIT)
          {
            access_tech = SD_BIT_ADD(access_tech, SYS_ACT_GSM_MASK);
          }
        }
        /* insert plmn w act info into the plmn selector list
        */
        sdmmss_insert_plmn_sel_rec(plmn,
                                   access_tech,
                                   cat);
        is_valid = TRUE;
      }
      break;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case SDMMSS_PLMN_FILE_TYPE_NO_ACT:
      for(i= 0;i < filesize / sizeof(sys_plmn_id_s_type);i++)
      {
        pos = sizeof(sys_plmn_id_s_type) * i;
        /* Get plmn info
        */
        memscpy( &plmn, sizeof(plmn), data_buf+pos, sizeof(sys_plmn_id_s_type) );
        if(sys_plmn_id_is_undefined(plmn))
        {
          continue;
        }
        sdmmss_insert_plmn_sel_rec(plmn,
                                   SYS_ACT_3GPP_ANY_MASK,
                                   cat);
        is_valid = TRUE;
      }
      break;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case SDMMSS_PLMN_FILE_TYPE_MAX:
    default:
      break;
  } //switch(file_type)

  return is_valid;

} /*  sdmmss_extract_from_plmndb */

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_create_plmn_selector_list

DESCRIPTION
  Creates the plmn selector list from the plmn db files read from MMGSDI

DEPENDENCIES
 filedb_ptr() should be initialized

RETURN VALUE 
  Boolean

SIDE EFFECTS
  None.

===========================================================================*/
static  void         sdmmss_create_plmn_selector_list( void )

{
  sdmmss_file_s_type * filedb_ptr = sdmmss_filedb_ptr(FALSE);
  boolean              is_hplmn   = FALSE;
  boolean              is_oplmn   = FALSE;
  boolean              is_uplmn   = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(filedb_ptr->app_type)
  {
    case MMGSDI_APP_SIM:
      /*Extract HOME plmn systems
      */
      is_hplmn = sdmmss_extract_from_plmndb(filedb_ptr->plmndb.imsi,
                                 MMGSDI_IMSI_LEN,
                                 filedb_ptr->plmndb.ad,
                                 SDMMSS_PLMN_FILE_TYPE_IMSI,
                                 SYS_DETAILED_PLMN_LIST_CATEGORY_HPLMN);
      /* Extract PREF plmn systems. 
      ** if ignore_uplmn EFS is set to FALSE, read the UPLMN.
      ** Always read the OPLMN
      */
      if (sdmmss_is_ignore_uplmn() == FALSE) 
      {
        is_uplmn = sdmmss_extract_from_plmndb(filedb_ptr->plmndb.plmnwact,
                              filedb_ptr->plmndb.plmnwact_size,
                              NULL,
                              SDMMSS_PLMN_FILE_TYPE_WACT,
                              SYS_DETAILED_PLMN_LIST_CATEGORY_USER_PREFERRED);
      }
      is_oplmn = sdmmss_extract_from_plmndb(filedb_ptr->plmndb.oplmnwact,
                         filedb_ptr->plmndb.oplmnwact_size,
                         NULL,
                         SDMMSS_PLMN_FILE_TYPE_WACT,
                         SYS_DETAILED_PLMN_LIST_CATEGORY_OPERATOR_PREFERRED);

      if(!is_oplmn && !is_uplmn)
      {
        is_oplmn = sdmmss_extract_from_plmndb(filedb_ptr->plmndb.gsm_plmn,
                                   filedb_ptr->plmndb.gsm_plmn_size,
                                   NULL,
                                   SDMMSS_PLMN_FILE_TYPE_NO_ACT,
                                   SYS_DETAILED_PLMN_LIST_CATEGORY_PREFERRED);
      }
      break;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case MMGSDI_APP_USIM:
      #ifdef FEATURE_EQUIVALENT_HPLMN
      is_hplmn = sdmmss_extract_from_plmndb(filedb_ptr->plmndb.usim_ehplmn,
                                      filedb_ptr->plmndb.usim_ehplmn_size,
                                      NULL,
                                      SDMMSS_PLMN_FILE_TYPE_NO_ACT,
                                      SYS_DETAILED_PLMN_LIST_CATEGORY_HPLMN);
      #else
      SD_ERR_0("eqPRL: EHPLMN feature undef");
      is_hplmn = FALSE;
      #endif
      if(!is_hplmn)
      {
        is_hplmn = sdmmss_extract_from_plmndb(filedb_ptr->plmndb.imsi,
                                   MMGSDI_IMSI_LEN,
                                   filedb_ptr->plmndb.ad,
                                   SDMMSS_PLMN_FILE_TYPE_IMSI,
                                   SYS_DETAILED_PLMN_LIST_CATEGORY_HPLMN);
      }     
      /* Extract PREF plmn systems. 
      ** if ignore_uplmn EFS is set to FALSE, read the UPLMN.
      ** Always read the OPLMN
      */
      if (sdmmss_is_ignore_uplmn() == FALSE) 
      {
        is_uplmn = sdmmss_extract_from_plmndb(filedb_ptr->plmndb.plmnwact,
                              filedb_ptr->plmndb.plmnwact_size,
                              NULL,
                              SDMMSS_PLMN_FILE_TYPE_WACT,
                              SYS_DETAILED_PLMN_LIST_CATEGORY_USER_PREFERRED);
      }

      is_oplmn = sdmmss_extract_from_plmndb(filedb_ptr->plmndb.oplmnwact,
                        filedb_ptr->plmndb.oplmnwact_size,
                        NULL,
                        SDMMSS_PLMN_FILE_TYPE_WACT,
                        SYS_DETAILED_PLMN_LIST_CATEGORY_OPERATOR_PREFERRED);
      break;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case MMGSDI_APP_CSIM:
    case MMGSDI_APP_RUIM:
    case MMGSDI_APP_UNKNOWN:
    case MMGSDI_APP_NONE:
    default:
      break;
  }
  
} /*  sdmmss_create_plmn_selector_list */



/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_update_filedb

DESCRIPTION
  MMOC calls this API to update SD module with the files it has 
  read.

DEPENDENCIES


RETURN VALUE 
   None

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void         sdmmss_update_filedb(

       mmgsdi_file_enum_type       file_name,
       /* The name of the file being updated 
       */

       const byte  *               data_buf,
       /* The contents of the file
       */

       uint32                      data_size
       /* The size of the file 
       */

) 
{
  sdmmss_file_s_type * filedb_ptr = sdmmss_filedb_ptr(FALSE);
  byte               * new_buf    = NULL;

  /* Allocate memory and copy it over
  */
  new_buf = (byte *)modem_mem_alloc( data_size, MODEM_MEM_CLIENT_MMODE);
  if(new_buf == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }
  
  memscpy( new_buf, data_size, data_buf, data_size );

  /* Point it to the correct location 
  */
  switch(file_name)
  {

    case MMGSDI_TELECOM_MLPL:
      filedb_ptr->mlpl           = new_buf;
      break;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case MMGSDI_TELECOM_MSPL:
      filedb_ptr->mspl           = new_buf;
      break;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Set the app type depending on the files that have been populated by 
    ** MMOC. 
    */
    case MMGSDI_GSM_IMSI:
      filedb_ptr->plmndb.imsi    = new_buf;
      filedb_ptr->app_type       = MMGSDI_APP_SIM;
      break;

    case MMGSDI_USIM_IMSI:
      filedb_ptr->plmndb.imsi    = new_buf;
      filedb_ptr->app_type       = MMGSDI_APP_USIM;
      break;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case MMGSDI_USIM_EHPLMN:
      filedb_ptr->plmndb.usim_ehplmn      = new_buf;
      filedb_ptr->plmndb.usim_ehplmn_size = data_size;
      filedb_ptr->app_type                = MMGSDI_APP_USIM;
      break;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case MMGSDI_GSM_AD:
    case MMGSDI_USIM_AD:
      filedb_ptr->plmndb.ad               = new_buf;
      break;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case MMGSDI_GSM_PLMNWACT:
    case MMGSDI_USIM_PLMNWACT:
      filedb_ptr->plmndb.plmnwact        = new_buf;
      filedb_ptr->plmndb.plmnwact_size   = data_size;
      break;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case MMGSDI_GSM_OPLMNWACT:
    case MMGSDI_USIM_OPLMNWACT:
      filedb_ptr->plmndb.oplmnwact        = new_buf;
      filedb_ptr->plmndb.oplmnwact_size   = data_size;
      break;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case MMGSDI_GSM_PLMN:
      filedb_ptr->plmndb.gsm_plmn        = new_buf;
      filedb_ptr->plmndb.gsm_plmn_size   = data_size;
      break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    default:
      SD_ERR_1("Unknown file %d ",file_name);
      modem_mem_free((void *) new_buf, MODEM_MEM_CLIENT_MMODE);
      new_buf = NULL;
      break;

  }//switch(filename)

} /*  sdmmss_update_filedb  */

/****************************************************************************

                        Equivalent PRL specific functions 

****************************************************************************/

/*===========================================================================

FUNCTION sdmmss_add_entries_to_enc_sys_tbl

DESCRIPTION
  Adds entries to encoded sys tbl cache

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void  sdmmss_add_entries_to_enc_sys_tbl( void )
{

  prli_sys_rec_tbl_s_type     * sys_tbl_ptr = prl_get_sys_tbl_ptr(FALSE);
  prli_sys_rec_enc_tbl_s_type * enc_sys_tbl = prl_get_enc_sys_tbl_ptr(FALSE);
  uint16                        i           = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_ASSERT( sys_tbl_ptr != NULL );
  SD_ASSERT( enc_sys_tbl != NULL );
  SD_ASSERT( enc_sys_tbl->sys_rec_tbl != NULL );
  if((enc_sys_tbl->offset/8) >= PRL_ENC_SYS_TBL_SIZE )
  {
    SD_ERR_0("EqPRL:ENC sys tbl full");
    return;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for(i = 0; i< sys_tbl_ptr->num_entries ; i++)
  {
    sdprl_encode_sys_record( &enc_sys_tbl->offset,
                             &(sys_tbl_ptr->sys_rec_tbl[i]),  
                             enc_sys_tbl->sys_rec_tbl );
    enc_sys_tbl->num_entries++;
  }
  /* Reset number of entries in sys record table cache
  */
  sys_tbl_ptr->num_entries = 0;
             
}//sdmmss_add_entries_to_enc_sys_tbl

/*===========================================================================

FUNCTION sdmmss_add_entry_to_sys_tbl

DESCRIPTION
  Adds an entry to the sys rec table cache
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void  sdmmss_add_entry_to_sys_tbl( 

  const prli_sys_rec_type      * sys_rec 
    /* PRL sys record in decoded format
    */

)
{
  prli_sys_rec_tbl_s_type     * sys_tbl_ptr = prl_get_sys_tbl_ptr(FALSE);
  prli_sys_rec_enc_tbl_s_type * enc_sys_tbl = prl_get_enc_sys_tbl_ptr(FALSE);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_ASSERT( sys_tbl_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(sys_tbl_ptr->num_entries == PRL_MAX_SYS_REC)
  {
    /* If the sys table cache is full, add all entries into encoded table
    */
    sdmmss_add_entries_to_enc_sys_tbl();
  }
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sys_tbl_ptr->sys_rec_tbl[sys_tbl_ptr->num_entries] = *sys_rec;
  sys_tbl_ptr->num_entries++;

  
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_EQPRL)
  {
    SD_MSG_LOW_6("EqPRL: sys tbl entry %d rec_type %d acq_idx %d, geo %d pref %d pri %d",
             enc_sys_tbl->num_entries,
             sys_rec->rec_type,
             sys_rec->acq_index,
             sys_rec->geo,
             sys_rec->neg_pref,
             sys_rec->pri);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (sys_rec->rec_type)
  {
    case PRL_SYS_REC_IS2000:
      SD_MSG_HIGH_3("EqPRL: sid %d nid _incl %d nid %d",
                 sys_rec->rec.is2000.sid,
                 sys_rec->rec.is2000.nid_incl,
                 sys_rec->rec.is2000.nid);
          break;
    case PRL_SYS_REC_IS856:
      SD_MSG_HIGH_2("EqPRL: subnet len %d subnet %d",
               sys_rec->rec.is856.subnet_length,
               sys_rec->rec.is856.subnet);
        break;
    case PRL_SYS_REC_MCC_MNC:
      SD_MSG_HIGH_3("EqPRL: mcc %x mnc %x subtype %d",
               sys_rec->rec.mcc_mnc.mcc,
               sys_rec->rec.mcc_mnc.mnc,
               sys_rec->rec.mcc_mnc.mcc_mnc_subtype);
      if(sys_rec->rec.mcc_mnc.mcc_mnc_subtype != 
                                     PRL_MCC_MNC_SUBTYPE_MULTIPLE_SUBNET_ID)
      {
        SD_MSG_HIGH_3("EqPRL: sid0 %d nid0 %d num_pairs %d",
               sys_rec->rec.mcc_mnc.info.sid_nid.sid[0],
               sys_rec->rec.mcc_mnc.info.sid_nid.nid[0],
               sys_rec->rec.mcc_mnc.info.sid_nid.sid_nid_pair_len);
      }
      break;
    case PRL_SYS_REC_PLMN:
        SD_MSG_HIGH_3("EqPRL: plmn %x %x %x ",
               sys_rec->rec.plmn.plmn_id.identity[0],
               sys_rec->rec.plmn.plmn_id.identity[1],
               sys_rec->rec.plmn.plmn_id.identity[2]);
        break;
    default:
    case PRL_SYS_REC_MAX:
      break;
    }
  }
  SYS_ARG_NOT_USED(enc_sys_tbl);
}


/*===========================================================================

FUNCTION sdmmss_add_entry_to_acq_tbl

DESCRIPTION
  Adds an entry to the acq rec table cache

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static int  sdmmss_add_entry_to_acq_tbl( 

  const prli_acq_rec_type      * acq_rec 
    /* PRL acq record in decoded format
    */
)
{
  int                       i           = 0;
  prli_acq_rec_tbl_s_type * acq_tbl_ptr = prl_get_acq_tbl_ptr(FALSE);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( acq_tbl_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for(i = 0; i < acq_tbl_ptr->num_entries && i < PRL_MAX_ACQ_REC; i++)
  {
    if( !memcmp(acq_rec,
               &(acq_tbl_ptr->acq_rec_tbl[i]),
               sizeof(prli_acq_rec_type))
      )
    {
      return i;
    }
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(i  == PRL_MAX_ACQ_REC)
  {
    SD_ERR_0("acq_rec tbl full");
    return PRL_MAX_ACQ_REC;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  acq_tbl_ptr->acq_rec_tbl[i] = *acq_rec;
  acq_tbl_ptr->num_entries++;
  SD_MSG_HIGH_6("Acq tbl entry %d , type %d num_rats %d, g_num %d w_num %d l_num %d",
             acq_tbl_ptr->num_entries-1,
             acq_rec->acq_type,
             acq_rec->type.gwl_generic.num_of_rats,
             acq_rec->type.gwl_generic.gsm_rec.num_bands,
             acq_rec->type.gwl_generic.wcdma_rec.num_bands,
             acq_rec->type.gwl_generic.lte_rec.num_bands);
  SD_MSG_HIGH_3("pri list %d %d %d",
             acq_rec->type.gwl_generic.pri_lst[0],
             acq_rec->type.gwl_generic.pri_lst[1],
             acq_rec->type.gwl_generic.pri_lst[2]);

  return i;
}

/*===========================================================================

FUNCTION sdmmss_is_3gpp2_sys_rec_match_type

DESCRIPTION
  Given a prl system record, it checks whether it matches a specified MMSS 
  system type

DEPENDENCIES
  None.

RETURN VALUE
 TRUE , if there is a match
 False , otherwise

SIDE EFFECTS
  None.

===========================================================================*/
static boolean  sdmmss_is_3gpp2_sys_rec_match_type( 

  const prli_sys_rec_type       * sys_rec, 
    /* PRL sys record in decoded format
    */

  sdmmss_sys_type_e_type         sys_type
    /* MMSS sys type to be compared with
    */
)
{
  boolean is_match = FALSE;
  switch(sys_type)
  {
    /* return TRUE if the rec is IS2000, IS856 or mcc mnc based
    */
    case SDMMSS_SYS_TYPE_3GPP2:
      if( sys_rec->rec_type == PRL_SYS_REC_IS2000 ||
          sys_rec->rec_type == PRL_SYS_REC_IS856  ||
         ( sys_rec->rec_type == PRL_SYS_REC_MCC_MNC &&
           INRANGE(sys_rec->rec.mcc_mnc.mcc_mnc_subtype,
                   PRL_MCC_MNC_SUBTYPE_ONLY,
                   PRL_MCC_MNC_SUBTYPE_MULTIPLE_SUBNET_ID)
         )
        )
      {
        is_match = TRUE;
      }
      break;

    /* return TRUE if the rec is IS2000 or MCC MNC SID NID based
    */
    case SDMMSS_SYS_TYPE_CDMA:
      if( sys_rec->rec_type == PRL_SYS_REC_IS2000 ||
         ( sys_rec->rec_type == PRL_SYS_REC_MCC_MNC &&
           INRANGE(sys_rec->rec.mcc_mnc.mcc_mnc_subtype,
                   PRL_MCC_MNC_SUBTYPE_ONLY,
                   PRL_MCC_MNC_SUBTYPE_MULTIPLE_SID_NID)
         )
        )
      {
        is_match = TRUE;
      }
      break;

    /* return TRUE if the rec is IS856 or MCC MNC Subnet ID based
    */
    case SDMMSS_SYS_TYPE_HDR:
      if( sys_rec->rec_type == PRL_SYS_REC_IS856 ||
         ( sys_rec->rec_type == PRL_SYS_REC_MCC_MNC &&
           sys_rec->rec.mcc_mnc.mcc_mnc_subtype == 
             PRL_MCC_MNC_SUBTYPE_MULTIPLE_SUBNET_ID
         )
        )
      {
        is_match = TRUE;
      }
      break;

    case SDMMSS_SYS_TYPE_3GPP:
    case SDMMSS_SYS_TYPE_LTE:
    case SDMMSS_SYS_TYPE_GSM:
    case SDMMSS_SYS_TYPE_WIMAX:
    case SDMMSS_SYS_TYPE_WCDMA:
    case SDMMSS_SYS_TYPE_MAX:
    default:
      break;     
  }

  return is_match;
} /* sdmmss_is_3gpp2_sys_rec_match_type */


/*===========================================================================

FUNCTION sdmmss_dealloc_dyn_memory

DESCRIPTION
  Deallocate dynamic memory used for creating the equivalent PRL


DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void  sdmmss_dealloc_dyn_memory( void )
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(! sdmmss_filedb_ptr(TRUE)  &&   /* reset file db */
     ! sdmmss_plmn_selector_ptr(TRUE) && /* reset PLMN selector list cache */
     ! sdmmss_mspl_ptr(TRUE) &&     /* reset MSPL cache*/
     ! sdmmss_mlpl_ptr(TRUE) &&     /* reset MLPL cache*/
     ! prl_get_sys_tbl_ptr(TRUE) && /* reset sys rec table cache*/
     ! prl_get_acq_tbl_ptr(TRUE) &&  /* reset acq rec table cache*/
     ! prl_get_enc_sys_tbl_ptr(TRUE) /* reset encoded sys tbl cache */
  )
  {
    SD_MSG_MED_0("EqPRL: init/dealloc dyn mem");
  }
  else
  {
    /* should never go here
    */
    SD_ERR_0("EqPRL: error in deallocating dyn mem");
  }

} /* sdmmss_dealloc_dyn_memory() */

/*===========================================================================

FUNCTION sdmmss_init

DESCRIPTION


DEPENDENCIES
  None.

RETURN VALUE
  Initializes the sdmmss component

SIDE EFFECTS
  None.

===========================================================================*/
void                   sdmmss_init( )
{
  sdmmss_dealloc_dyn_memory();
}
/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_extract_home_sys_from_prl

DESCRIPTION


DEPENDENCIES


RETURN VALUE 
  int 

SIDE EFFECTS
  None.

===========================================================================*/
static  sdmmss_error_e_type         sdmmss_extract_home_sys_from_prl(

    sdprl_parser_s_type          *trav_parser,
      /* PRL parser indicating curr pos in provisioned PRL
      */

    sdmmss_sys_type_e_type       sys_type,
      /* MMSS sys type indicating type of home sys - cdma / hdr / 3gpp2_AI
      */

    int                          mspl_rec_cnt
      /* Indicates curr pos in GEO. used to determine if the entry is the 
      ** first in the GEO or not
      */
)
{
  prli_sys_rec_type                 sys_rec;
  sdprl_parser_s_type               prl_parser;
  dword                             num_bits;
  prl_pri_type                      pri;

  /* Initialize the parser to the start of the GEO
  */
  prl_parser            = *trav_parser;
  prl_parser.sys_idx    = trav_parser->geo_idx;
  prl_parser.sys_offset = trav_parser->geo_offset;

  /* Get the first sys rec in the geo
  */
  prl_parser.sys_rec_len = sdprl_parser_get(&prl_parser,&sys_rec);
  num_bits               = prl_parser.sys_rec_len;

  /* continue parsing the geo till we reach the first system record
  ** with matching type
  */
  while(!sdmmss_is_3gpp2_sys_rec_match_type(&sys_rec,sys_type)&&
        num_bits != 0)
  {
    num_bits = sdprl_parser_next(&prl_parser, &sys_rec);
  }

  /* If the first matching sys record entry occurs before the sys record
  ** the traversal parser is pointing to now, or if there is no matching entry
  ** in the current GEO , there is an mspl provisioning
  ** error. So ignore the mspl entry and move to the next.
  */
  if(prl_parser.sys_idx < trav_parser->sys_idx)
  {
    return SDMMSS_ERROR_RECS_ALREADY_INSERTED;
  }
  if( prl_parser.geo_idx != trav_parser->geo_idx ||
      num_bits == 0)
  {
    return SDMMSS_ERROR_NO_RECS_FOUND;
  }

  /* If there is no error, proceed to enter records pointed to by
  ** the traversal parser into the sys rec tbl. continue adding 
  ** records till the first cdma sys record is reached.
  */
  num_bits = sdprl_parser_get(trav_parser,&sys_rec);

  /* If this is not the first entry in the GEO, make sure the GEO field is
  ** corrected. 
  */
  if(mspl_rec_cnt != 0 && sys_rec.geo == PRL_NEW_GEO_REGION)
  {
    sys_rec.geo = PRL_SAME_GEO_REGION;
  }

  do
  {
    sdmmss_add_entry_to_sys_tbl(&sys_rec); 
    trav_parser->prl_insert_idx++;
    num_bits = sdprl_parser_next(trav_parser, &sys_rec);
  }while(trav_parser->sys_idx <= prl_parser.sys_idx && num_bits != 0);

  /* Now proceed to add all cdma entries of the same priority
  ** into the sys record table
  */
  if(prl_parser.sys_rec.pri == PRL_PRI_SAME)
  {
    pri = PRL_PRI_SAME;
    while(sdmmss_is_3gpp2_sys_rec_match_type(&sys_rec,sys_type) && 
          sys_rec.geo  != PRL_NEW_GEO_REGION            &&
          pri          == PRL_PRI_SAME                  &&
          num_bits    != 0 )
    {
      pri = sys_rec.pri;
      sdmmss_add_entry_to_sys_tbl(&sys_rec);
      trav_parser->prl_insert_idx++;
      num_bits = sdprl_parser_next(trav_parser, &sys_rec);
    }
  }
  return SDMMSS_ERROR_NO_ERR;

} /* sdmmss_extract_home_sys_from_prl */

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_extract_pref_sys_from_prl

DESCRIPTION
This function extracts the preferred systems in the GEO which is the second 
priority tier of each sys type in the GEO.
This function is called only if there are at least home systems of the
given sys type in the GEO. IF extract home sys returned error = NO_RECS_FOUND
do not call this API.

DEPENDENCIES
sdmmss_extract_home_sys_from_prl() did not return error NO RECS FOUND.

RETURN VALUE 
  int 

SIDE EFFECTS
  None.

===========================================================================*/
static sdmmss_error_e_type    sdmmss_extract_pref_sys_from_prl(

    sdprl_parser_s_type            *trav_parser,
      /* PRL parser indicating curr pos in provisioned PRL
      */

    sdmmss_sys_type_e_type          sys_type,
      /* MMSS sys type indicating type of pref sys - cdma / hdr / 3gpp2_AI
      */

    int                             mspl_rec_cnt
      /* Indicates curr pos in GEO. used to determine if the entry is the 
      ** first in the GEO or not
      */

)
{
  prli_sys_rec_type                 sys_rec;
  sdprl_parser_s_type               prl_parser;
  dword                             num_bits;
  prl_pri_type                      pri;

  /* Initialize the parser to the start of the GEO
  */
  prl_parser            = *trav_parser;
  prl_parser.sys_idx    = trav_parser->geo_idx;
  prl_parser.sys_offset = trav_parser->geo_offset;

  /* Get the first sys rec in the geo
  */
  prl_parser.sys_rec_len = sdprl_parser_get(&prl_parser,&sys_rec);
  num_bits = prl_parser.sys_rec_len;

  /* continue parsing the geo till we reach the first system record
  ** with matching type
  */
  while(!sdmmss_is_3gpp2_sys_rec_match_type(&sys_rec,sys_type) && 
         num_bits != 0 )
  {
    num_bits = sdprl_parser_next(&prl_parser, &sys_rec);
  }

  /* continue parsing till we pass the first priority set
  */
  do
  {
    pri = sys_rec.pri;
    num_bits = sdprl_parser_next(&prl_parser, &sys_rec);
  }while(pri == PRL_PRI_SAME            &&
        prl_parser.geo_idx == trav_parser->geo_idx     &&
        sdmmss_is_3gpp2_sys_rec_match_type(&sys_rec,sys_type) &&
        num_bits    != 0);

  /* Find the first - second priority level - system . In other words , find
  ** a preferred system , if present
  */
  while(!sdmmss_is_3gpp2_sys_rec_match_type(&sys_rec,sys_type) && 
         num_bits != 0 && 
         prl_parser.geo_idx == trav_parser->geo_idx )
  {
    num_bits = sdprl_parser_next(&prl_parser, &sys_rec);
  }

  /* If the first matching sys record entry occurs before the sys record
  ** the traversal parser is pointing to now, or if there is no matching entry
  ** in the current GEO , there is an mspl provisioning
  ** error. So ignore the mspl entry and move to the next.
  */
  if(prl_parser.sys_idx < trav_parser->prl_insert_idx)
  {
    return SDMMSS_ERROR_RECS_ALREADY_INSERTED;
  }
  if( prl_parser.geo_idx != trav_parser->geo_idx ||
      num_bits == 0)
  {
    return SDMMSS_ERROR_NO_RECS_FOUND;
  }

  /* If there is no error, proceed to enter records pointed to by
  ** the traversal parser into the sys rec tbl. continue adding 
  ** records till the first pref sys record is reached.
  */
  num_bits = sdprl_parser_get(trav_parser,&sys_rec);

  /* If this is not the first entry in the GEO, make sure the GEO field is
  ** corrected. 
  */
  if(mspl_rec_cnt != 0 && sys_rec.geo == PRL_NEW_GEO_REGION)
  {
    sys_rec.geo = PRL_SAME_GEO_REGION;
  }

  do
  {
    sdmmss_add_entry_to_sys_tbl(&sys_rec); 
    trav_parser->prl_insert_idx++;
    num_bits = sdprl_parser_next(trav_parser, &sys_rec);
  }while(trav_parser->sys_idx <= prl_parser.sys_idx && num_bits != 0);

  /* Now proceed to add all cdma entries of the same priority
  ** into the sys record table
  */
  if(prl_parser.sys_rec.pri == PRL_PRI_SAME)
  {
    pri = PRL_PRI_SAME;
    while(sdmmss_is_3gpp2_sys_rec_match_type(&sys_rec, sys_type) && 
          sys_rec.geo   != PRL_NEW_GEO_REGION &&
          pri           == PRL_PRI_SAME &&
          num_bits      != 0 )
    {
      pri = sys_rec.pri;
      sdmmss_add_entry_to_sys_tbl(&sys_rec);
      trav_parser->prl_insert_idx++;
      num_bits = sdprl_parser_next(trav_parser, &sys_rec);
    }
  }
  return SDMMSS_ERROR_NO_ERR;

} /* sdmmss_extract_pref_sys_from_prl */


/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_extract_any_sys_from_prl

DESCRIPTION


DEPENDENCIES


RETURN VALUE 
  int 

SIDE EFFECTS
  None.

===========================================================================*/
static sdmmss_error_e_type    sdmmss_extract_any_sys_from_prl(

    sdprl_parser_s_type            *trav_parser,
      /* PRL parser indicating curr pos in provisioned PRL
      */

    sdmmss_sys_type_e_type         sys_type,
      /* MMSS sys type indicating type of ANY sys- cdma / hdr / 3gpp2_AI
      */

    int                            mspl_rec_cnt
      /* Indicates curr pos in GEO. used to determine if the entry is the 
      ** first in the GEO or not
      */
)
{
  prli_sys_rec_type                 sys_rec;
  sdprl_parser_s_type               prl_parser;
  dword                             num_bits;
  int                               last_match_idx = 0xFFFF ;

  /* Initialize the parser to the start of the GEO
  */
  prl_parser            = *trav_parser;
  prl_parser.sys_idx    = trav_parser->geo_idx;
  prl_parser.sys_offset = trav_parser->geo_offset;

  /* Get the first sys rec in the geo
  */
  prl_parser.sys_rec_len = sdprl_parser_get(&prl_parser,&sys_rec);
  num_bits = prl_parser.sys_rec_len;

  /* Parse the entire geo and note the sys index of the last matching record
  **. Eg. if the geo has 2 HDR entries followed by 2 cdma entries followed 
  ** by another 2 hdr entries. if the mspl entry is HDR ANY MORE - all 
  ** entries till the last HDR entry should be entered into the sys rec tbl
  ** last_match_idx is used to store the index of the last hdr entry 
  */
  while( prl_parser.geo_idx == trav_parser->geo_idx && 
         num_bits           != 0 )
  {
    if(sdmmss_is_3gpp2_sys_rec_match_type(&sys_rec,sys_type))
    {
      last_match_idx = prl_parser.sys_idx;
    }
    num_bits = sdprl_parser_next(&prl_parser, &sys_rec);
  }

  /* If the first matching sys record entry occurs before the sys record
  ** the traversal parser is pointing to now, or if there is no matching entry
  ** in the current GEO , there is an mspl provisioning
  ** error. So ignore the mspl entry and move to the next.
  */
  if(last_match_idx < trav_parser->prl_insert_idx)
  {
    return SDMMSS_ERROR_RECS_ALREADY_INSERTED;
  }
  if( last_match_idx == 0xFFFF)
  {
    return SDMMSS_ERROR_NO_RECS_FOUND;
  }

  /* If there is no error, proceed to enter records pointed to by
  ** the traversal parser into the sys rec tbl. continue adding 
  ** records till the last matching record is reached.
  */
  num_bits = sdprl_parser_get(trav_parser,&sys_rec);

  /* If this is not the first entry in the GEO, make sure the GEO field is
  ** corrected. 
  */
  if(mspl_rec_cnt != 0 && sys_rec.geo == PRL_NEW_GEO_REGION)
  {
    sys_rec.geo = PRL_SAME_GEO_REGION;
  }

  do
  {
    sdmmss_add_entry_to_sys_tbl(&sys_rec); 
    trav_parser->prl_insert_idx++;
    num_bits = sdprl_parser_next(trav_parser, &sys_rec);
  }while(trav_parser->sys_idx <= last_match_idx && num_bits != 0);

  return SDMMSS_ERROR_NO_ERR;

} /* sdmmss_extract_any_sys_from_prl */


/* <EJECT> */
/*===========================================================================


FUNCTION sdmmss_insert_from_plmn_sel_list

DESCRIPTION
   Add 3GPP entries from an MLPL loc tag into a plmn list
   Add plmn_start followed by plmn_range number of entries from the plmn
   selector list

DEPENDENCIES

  
RETURN VALUE 


SIDE EFFECTS
  None.

===========================================================================*/
static void         sdmmss_insert_from_plmn_sel_list(

    sys_plmn_rat_category_list_s_type    * plmn_list,
      /* output PLMN list into which to add entries 
      */

    sys_plmn_id_s_type                    plmn_start,
      /* first entry into the plmn_list
      */

    byte                                  plmn_range
      /* Number of additional consecutive entries to be
      ** inserted into the plmn_list from the plmn_selector lst
      */

)
{
  uint32                             i              = 0;
  uint32                             j              = 0;
  sys_plmn_rat_category_list_s_type  * plmn_sel_ptr = 
                                            sdmmss_plmn_selector_ptr(FALSE);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Add the plmn start entry into the mlpl list regardless of whether it 
  ** is in the plmn selector list 
  */
  sys_add_plmn_to_list(plmn_list,&plmn_start);

  if(plmn_range == 0)
  {
    return;
  }
  /* If plmn range is greater than 0, we need to look for the plmn start
  ** entry in the plmn selector list and add then next plmn_range entries
  ** into the mlpl list
  */
  for(i=0; i<plmn_sel_ptr->length;i++)
  {
    if(sys_plmn_match(plmn_sel_ptr->info[i].plmn,plmn_start))
    {
      for(j=1; (j <= plmn_range) && ((i+j) < plmn_sel_ptr->length ) ;j++)
      {
        sys_add_plmn_to_list(plmn_list,&(plmn_sel_ptr->info[i+j].plmn));
      }
      return;
    }
  }
}/* sdmmss_insert_from_plmn_sel_list */

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_map_3gpp_sys_type_to_act_mask

DESCRIPTION
  Maps the 3gpp sdmmss_sys_type_e_type values to the corresponding 
  sys_access_tech_e_type values

DEPENDENCIES


RETURN VALUE 
  sys_access_tech_e_type

SIDE EFFECTS
  None.

===========================================================================*/
static sys_access_tech_e_type    sdmmss_map_3gpp_sys_type_to_act_mask(

    sdmmss_sys_type_e_type         sys_type

)
{
  sys_access_tech_e_type  act = SYS_ACT_NONE_MASK;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(sys_type)
  {
    case SDMMSS_SYS_TYPE_3GPP:
      act = SYS_ACT_3GPP_ANY_MASK;
      break;

    case SDMMSS_SYS_TYPE_LTE:
      act = SYS_ACT_LTE_MASK;
      break;

    case SDMMSS_SYS_TYPE_GSM:
      act = SYS_ACT_GSM_MASK;
      break;

    case SDMMSS_SYS_TYPE_WCDMA:
      act = SYS_ACT_UMTS_MASK;
      break;

    case SDMMSS_SYS_TYPE_3GPP2:
    case SDMMSS_SYS_TYPE_CDMA:
    case SDMMSS_SYS_TYPE_HDR:
    case SDMMSS_SYS_TYPE_WIMAX:
    case SDMMSS_SYS_TYPE_MAX:
    default:
      act = SYS_ACT_NONE_MASK;
      break;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sdmmss_map_3gpp_sys_type_to_act_mask */


/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_get_mlpl_plmn_list

DESCRIPTION
  Returns the list of plmns associated with an mlpl record

DEPENDENCIES


RETURN VALUE 
  plmn list

SIDE EFFECTS
  None.

===========================================================================*/
static void     sdmmss_get_mlpl_plmn_list(

  const sdmmss_mlpl_recs_s_type          * rec_ptr,
      /* MLPL Record in decoded format
      */

  sys_plmn_rat_category_list_s_type      * plmn_list
      /* insert plmn entries in the mlpl record into this plmn list
      */
)
{

  sys_plmn_id_s_type   plmn;
  sys_mcc_type         mcc          = SYS_WILDCARD_MCC;
  sys_mnc_type         mnc          = SYS_WILDCARD_MNC;
  sys_mnc_type         hex_mnc      = SYS_WILDCARD_MNC;
  boolean              is_pcs       = FALSE;
  byte                 num_loc_tag  = 0;
  int                  comp_mask    = SDMMSS_SYS_TYPE_3GPP2_ALL_MASK;
  byte                 plmn_range   = 0;
  byte                 i            = 0;
  sdmmss_sys_type_e_type  sys_type;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( rec_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  plmn_list->length = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If MCC bit is set, one record has to be inserted into the list
  */
  if(SYS_IS_BIT_CONTAIN(rec_ptr->loc_param_type,SDMMSS_LOC_PARAM_TYPE_MCC))
  {
    /* If MNC bit is set, read the value
    */
    if(SYS_IS_BIT_CONTAIN(rec_ptr->loc_param_type,SDMMSS_LOC_PARAM_TYPE_MNC))
    {
      hex_mnc = rec_ptr->loc_param_val.MNC;
    }
    else
    {
      hex_mnc = SYS_WILDCARD_MNC;
    }
    sdmmss_extract_mcc_mnc_from_bcd_values(rec_ptr->loc_param_val.MCC,
                                           hex_mnc,
                                           &mcc,
                                           &mnc,
                                           &is_pcs);
    if(sys_plmn_set_mcc_mnc2(is_pcs, 
                             mcc,
                             mnc,
                             &plmn))
    {
      sys_add_plmn_to_list(plmn_list,&plmn);
    }
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(SYS_IS_BIT_CONTAIN(rec_ptr->loc_param_type,SDMMSS_LOC_PARAM_TYPE_LOC_TAG))
  {
    num_loc_tag = sdmmss_get_num_sys_loc_tag(rec_ptr);
    /* Check to avoid i getting more than array bound in the following loop
    */
    if(num_loc_tag > SDMMSS_MAX_NUM_SYS_LOC_TAG )
    {
      num_loc_tag = SDMMSS_MAX_NUM_SYS_LOC_TAG;
    }

    for(i = 0; i < num_loc_tag; i++)
    {
      /* Get sys type information from each location tag
      */
      sys_type = sdmmss_get_mlpl_rec_sys_type(rec_ptr,i);

      /* If the location tag has 3GPP2 information , skip to 
      ** next tag
      */
      if(SYS_IS_BIT_CONTAIN(BM(sys_type),comp_mask))
      {
        continue;
      }
      /* If the loc tag has 3gpp info ,it needs to be added into the 
      ** list 
      */
      sdmmss_get_mlpl_gwl_loc_tag_info(rec_ptr,i,&plmn, &plmn_range);
      if(sys_plmn_id_is_undefined(plmn))
      {
        continue;
      }
      /* If the loc tag has valid info extract it into the list 
      */
      sdmmss_insert_from_plmn_sel_list(plmn_list,plmn,plmn_range);
    }
  }
} /* sdmmss_get_mlpl_plmn_list*/

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_get_mlpl_plmn_match

DESCRIPTION
  Determines the match level for a given PLMN with an MLPL record

DEPENDENCIES


RETURN VALUE 
  match level - sdmmss_mlpl_match_lvl_e_type

SIDE EFFECTS
  None.

===========================================================================*/
static  sdmmss_mlpl_match_lvl_e_type     sdmmss_get_mlpl_plmn_match(

  const sys_plmn_id_s_type             * plmn_ptr,
      /* PLMN to be matched
      */
  const sdmmss_mlpl_recs_s_type        * mlpl_rec_ptr,
      /* MLPL Record in decoded format to be matched
      */
  sys_plmn_rat_category_list_s_type    * plmn_list
      /* output the plmns associated with the MLPL rec
      */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( mlpl_rec_ptr != NULL );
  SD_ASSERT( plmn_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  plmn_list->length = 0;

  /* add all mlpl plmn entries into plmn_list
  */
  sdmmss_get_mlpl_plmn_list(mlpl_rec_ptr, plmn_list);

  /* If there is an mcc mnc match or an mcc match alone 
  ** consider it an mlpl match
  */
  if(sys_detailed_plmn_list_find_plmn(plmn_list,*plmn_ptr))
  {
    return SDMMSS_MLPL_MATCH_LVL_MCC_MNC;
  }

  return SDMMSS_MLPL_MATCH_LVL_NONE;

} /* sdmmss_get_mlpl_plmn_match */

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_get_dflt_mspl_idx

DESCRIPTION
  Fetches the default MSPL index

DEPENDENCIES
 sdmmss_set_dflt_mspl_idx() should have been called

RETURN VALUE 
  default mspl index

SIDE EFFECTS
  None.

===========================================================================*/
static  byte *      sdmmss_get_dflt_mspl_idx( void )
{
  static byte dflt_mspl_idx = 0; 
  return &dflt_mspl_idx;

} /* sdmmss_get_dflt_mspl_idx*/

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_get_loc_grp_method

DESCRIPTION
  Fetches the location grouping method

DEPENDENCIES
 sdmmss_set_dflt_mspl_idx() should have been called

RETURN VALUE 
  sdmmss_loc_grp_e_type

SIDE EFFECTS
  None.

===========================================================================*/
static  sdmmss_loc_grp_e_type * sdmmss_get_loc_grp_method( void)
{
  static sdmmss_loc_grp_e_type loc_grp;
  return &loc_grp;

} /* sdmmss_get_loc_grp_method*/

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_set_dflt_mspl_idx

DESCRIPTION
  Updates local data variable dflt_mspl_idx with the correct information
  Default MSPL is identified as follows:  If the bits 0, 1 and 2 of 
  LOC_PARAM_TYPE in a MLPL record are set to 0, the MSPL it points to is
  the default MSPL.
  If there is no default MSPL provisioned, set dflt_mspl_idx to 
  num_mspl_idx +1 (an invalid mspl index)

DEPENDENCIES
  mlpl_ptr() should have a valid MLPL.

RETURN VALUE 
  None

SIDE EFFECTS
  None.

===========================================================================*/
static  void         sdmmss_set_dflt_mspl_idx( void )

{
  sdmmss_mlpl_s_type           *mlpl_ptr = sdmmss_mlpl_ptr(FALSE);
  sdmmss_mlpl_recs_s_type      *rec_ptr  = NULL;
  byte                         *mspl_idx = sdmmss_get_dflt_mspl_idx();
  uint32                        i        = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(i = 0; i< mlpl_ptr->num_mlpl_recs ; i++ )
  {

    if( (rec_ptr = sdmmss_get_mlpl_rec(i)) == NULL )
    {
      continue;
    }

    if(!SYS_IS_BIT_CONTAIN(rec_ptr->loc_param_type,SDMMSS_LOC_PARAM_TYPE_LOC_TAG) &&
       !SYS_IS_BIT_CONTAIN(rec_ptr->loc_param_type,SDMMSS_LOC_PARAM_TYPE_MCC) &&
       !SYS_IS_BIT_CONTAIN(rec_ptr->loc_param_type,SDMMSS_LOC_PARAM_TYPE_MNC))
    {
      SD_MSG_HIGH_1("default mspl_idx = %d",rec_ptr->mspl_index);
      *mspl_idx = rec_ptr->mspl_index;
      return;
    }

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If we got here, we didnt find the default MSPL 
  */
  *mspl_idx = sdmmss_mspl_ptr(FALSE)->num_mspl_id + 1;

} /* sdmmss_set_dflt_mspl_idx */


/* <EJECT> */
/*===========================================================================

FUNCTION  sdmmss_is_mcc_mnc_based_sys_rec

DESCRIPTION
  Decides if system record is mcc mnc based 

DEPENDENCIES

RETURN VALUE 
  TRUE if MCC MNC based 
  FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
static    boolean       sdmmss_is_mcc_mnc_based_sys_rec(
      const prli_sys_rec_type   *sys_rec_ptr,
            /* Pointer to a system record from which to get the MCC.
            */
      sys_plmn_id_s_type        *plmn 
)
{
  sys_mcc_type    mcc;
  sys_mnc_type    mnc;
  boolean         is_pcs;

  if(sys_rec_ptr->rec_type == PRL_SYS_REC_MCC_MNC)
  {
    sdmmss_extract_mcc_mnc_from_bcd_values(sys_rec_ptr->rec.mcc_mnc.mcc,
                                           sys_rec_ptr->rec.mcc_mnc.mnc,
                                           &mcc,
                                           &mnc,
                                           &is_pcs);
    if(sys_plmn_set_mcc_mnc2(is_pcs, 
                             mcc,
                             mnc,
                             plmn))
    {
      return TRUE;
    }
  }
  return FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
} /*sdmmss_is_mcc_mnc_based_sys_rec */

/* <EJECT> */
/*===========================================================================

FUNCTION  sdmmss_match_3gpp2_loc_tag_with_sys_rec

DESCRIPTION
matches an IS2000 or an IS 856 prl sys record with a given MLPL loc tag
Not meant for mcc-mnc or plmn sys records

DEPENDENCIES

RETURN VALUE 

SIDE EFFECTS
  None.

===========================================================================*/
static    boolean      sdmmss_match_3gpp2_loc_tag_with_sys_rec(
    
      const prli_sys_rec_type              *sys_rec_ptr,
       /* Pointer to a system record in the PRL
       */
      const sdmmss_mlpl_recs_s_type        *mlpl_rec_ptr, 
       /* Pointer to an MLPL record
       */
      byte                                  tag_num
       /* Tag number of mlpl sys location tag of interest
       */
)
{
 
  sdmmss_sys_loc_tag_s_type    tag_info;
  word                         nid_start;
  word                         nid_end;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sys_rec_ptr != NULL );
  SD_ASSERT( mlpl_rec_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  tag_info = sdmmss_get_mlpl_rec_tag_info(mlpl_rec_ptr,tag_num);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(tag_info.sys_type)
  {
    case SDMMSS_SYS_TYPE_3GPP2:
    case SDMMSS_SYS_TYPE_CDMA:
      if(sys_rec_ptr->rec.is2000.sid == tag_info.sys_loc_tag.sys_loc_tag_cdma.SID ||
         sys_rec_ptr->rec.is2000.sid == PRL_WILDCARD_SID)
      {
        nid_start = tag_info.sys_loc_tag.sys_loc_tag_cdma.NID_start;
        nid_end = nid_start + tag_info.sys_loc_tag.sys_loc_tag_cdma.NID_range;
        if(INRANGE(sys_rec_ptr->rec.is2000.nid,nid_start,nid_end) ||
           sys_rec_ptr->rec.is2000.nid == PRL_WILDCARD_NID)
        {
          return TRUE;
        }
      }
      break;

    case SDMMSS_SYS_TYPE_HDR:
      SD_ERR_0("need to add HDR info");
      break;

    case SDMMSS_SYS_TYPE_3GPP:
    case SDMMSS_SYS_TYPE_LTE:
    case SDMMSS_SYS_TYPE_GSM:
    case SDMMSS_SYS_TYPE_WIMAX:
    case SDMMSS_SYS_TYPE_WCDMA:
    case SDMMSS_SYS_TYPE_MAX:
    default:
      SD_ERR_0("invalid sys type ");
      break;
  }
  return FALSE;
} /* sdmmss_match_3gpp2_loc_tag_with_sys_rec */

/* <EJECT> */
/*===========================================================================

FUNCTION  sdmmss_determine_mspl_idx

DESCRIPTION
  Finds the MSPL index, given the PLMN as input. 
  It also returns a list of PLMNs associated with the same MSPL index

DEPENDENCIES

RETURN VALUE 
  MSPL index

SIDE EFFECTS
  None.

===========================================================================*/
static    byte       sdmmss_determine_mspl_idx (

      const sys_plmn_id_s_type          *geo_plmn,
            /* Pointer to a PLMN record for which to find the MSPL idx
            */
      sys_plmn_rat_category_list_s_type  *mlpl_lst 
            /* output MLPL plmn list
            */
)
{
  sdmmss_mlpl_s_type           *mlpl_ptr        = sdmmss_mlpl_ptr(FALSE);
  sdmmss_mlpl_recs_s_type      *rec_ptr         = NULL;
  uint32                       i                = 0;
  byte                         *mspl_idx_ptr    = sdmmss_get_dflt_mspl_idx();
  byte                         mspl_idx         = *mspl_idx_ptr;
  sdmmss_mlpl_match_lvl_e_type cur_match_lvl    = SDMMSS_MLPL_MATCH_LVL_NONE;
  sdmmss_mlpl_match_lvl_e_type new_match_lvl    = SDMMSS_MLPL_MATCH_LVL_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( mlpl_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !sys_plmn_id_is_undefined (*geo_plmn))
  {
    /* First find a match for the geo_plmn in the MLPL table
    */
    for(i = 0; i < mlpl_ptr->num_mlpl_recs ; i++)
    {
      rec_ptr = sdmmss_get_mlpl_rec(i);

      if( rec_ptr == NULL )
      {
         continue;
      }  

      /* Find the match level for the given MLPL record
      */
      new_match_lvl = sdmmss_get_mlpl_plmn_match(geo_plmn,rec_ptr,mlpl_lst);

      /* If it is a better match, update the MSPL index and the current 
      ** match level
      */
      if(new_match_lvl > cur_match_lvl)
      {
        cur_match_lvl = new_match_lvl;
        mspl_idx      = rec_ptr->mspl_index;
      }

      /* If there is an MCC MNC Match - no need to search further
      */
      if(cur_match_lvl == SDMMSS_MLPL_MATCH_LVL_MCC_MNC)
      {
        break;
      }

    } // for (num_mlpl_recs)

  } //!sys_plmn_id_is_undefined

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* At this point, MSPL idx = 
  ** a)  MSPL index associated with first PLMN match in MLPL or
  ** b)  default MSPL idx, if no MCC match was found
  */
  /* MLPL list will have the list of associated PLMNs if an MLPL match was found.
  ** if no match was found ,update mlpl_plmn_list to contain the PLMN found
  ** in the PRL.
  */

  if(cur_match_lvl ==  SDMMSS_MLPL_MATCH_LVL_NONE)
  {
    mlpl_lst->length = 0; 
    sys_add_plmn_to_list(mlpl_lst,geo_plmn);
  }
  /* Print mlpl list
  */

  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_EQPRL)
  {
    SD_MSG_HIGH_3("match lvl %d mlpl len %d mspl idx %d ",cur_match_lvl,
                                                     mlpl_lst->length,
                                                     mspl_idx);
    for(i = 0; i<mlpl_lst->length; i++)
    {
  
      SD_MSG_LOW_3("%x %x %x",mlpl_lst->info[i].plmn.identity[0],
                          mlpl_lst->info[i].plmn.identity[1],
                          mlpl_lst->info[i].plmn.identity[2]);
    }
  }
  return mspl_idx;

} /* sdmmss_determine_mspl_idx */


/* <EJECT> */
/*===========================================================================

FUNCTION  sdmmss_find_geo_plmn

DESCRIPTION
  Finds the PLMN associated with the current GEO. If there is an MCC associated, it 
  returns TRUE together with the MCC found.

DEPENDENCIES

RETURN VALUE 
  TRUE if an MCC is found for the current GEO
  FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
static    boolean       sdmmss_find_geo_plmn(

       sdprl_parser_s_type      *parser_ptr,
            /* Pointer to a PRL-parser that is associated with the PRL from
            ** which to get the system table entry.
            */

      sys_plmn_id_s_type        *plmn 
            /* output first plmn found in the GEO
            */

)
{
  prli_sys_rec_type          sys_rec;
  sdmmss_loc_grp_e_type      *loc_grp     = sdmmss_get_loc_grp_method();
  sdmmss_mlpl_recs_s_type    *rec_ptr     = NULL;
  int                        comp_mask    = SDMMSS_SYS_TYPE_3GPP2_ALL_MASK;
  byte                       num_loc_tag  = 0;
  uint32                     i;
  byte                       j;

  sdmmss_sys_type_e_type                 sys_type;
  sys_plmn_rat_category_list_s_type      *plmn_list = NULL;

  /* Get first sys rec in GEO
  */
  if(!sdprl_parser_get(parser_ptr,&sys_rec))
  {
    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(*loc_grp)
  {
    case SDMMSS_LOC_GRP_MCC_BASED:
      do
      {
        if(sdmmss_is_mcc_mnc_based_sys_rec(&sys_rec,plmn))
        {
          return TRUE;
        }
      }while( sdprl_parser_next(parser_ptr,&sys_rec) && 
              sys_rec.geo != PRL_NEW_GEO_REGION);
      break;
      
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case SDMMSS_LOC_GRP_SID_BASED:
      
      /* allocate mem for the plmn_list local variable
      */
      plmn_list = (sys_plmn_rat_category_list_s_type *)\
                modem_mem_alloc( sizeof(sys_plmn_rat_category_list_s_type), 
                                                   MODEM_MEM_CLIENT_MMODE);
      if(plmn_list == NULL )
      {
        sys_err_fatal_null_ptr_exception();
      } 
      /* Initialize the length to 0
      */
      memset(plmn_list,0,sizeof(sys_plmn_rat_category_list_s_type));
      plmn_list->length = 0;

      do
      {
        /* If sys rec is not 3gpp2 , continue to next sys rec in GEO
        */
        if(sys_rec.rec_type == PRL_SYS_REC_PLMN)
        {
        
          SD_MSG_HIGH_0("EqPRL:skip plmn sys rec");
          continue;
        }
        /* If the sys rec is MCC MNC based, get MCC info from the sys rec 
        */
        if(sdmmss_is_mcc_mnc_based_sys_rec(&sys_rec,plmn))
        {
          /* deallocate memory for plmn_list
          */
          if(plmn_list)
          {
            modem_mem_free((void *) plmn_list, MODEM_MEM_CLIENT_MMODE);
            plmn_list = NULL;
          }
          return TRUE;
        }
        /* At this point the sys rec will be either IS2000 or IS856.
        ** Go through the MLPL table to find a match
        */
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        for (i = 0 ; (rec_ptr = sdmmss_get_mlpl_rec(i)) != NULL; i++)
        {
          /* If SYS_LOC_TAG bit is not set, then the MLPL record has only 
          ** MCC/MNC info. Continue to next MLPL record.
          */
          if(!SYS_IS_BIT_CONTAIN(rec_ptr->loc_param_type, 
                                 SDMMSS_LOC_PARAM_TYPE_LOC_TAG))
          {
            continue;
          }
          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
          num_loc_tag = sdmmss_get_num_sys_loc_tag(rec_ptr);

          if (num_loc_tag > SDMMSS_MAX_NUM_SYS_LOC_TAG)
          {
            SD_ERR_1("num_loc_tag = %d, out of boundary",
                         num_loc_tag);

            num_loc_tag = SDMMSS_MAX_NUM_SYS_LOC_TAG;
          }

          for(j = 0; j < num_loc_tag ; j++)
          {
            /* Get sys type information from each location tag
            */
            sys_type = sdmmss_get_mlpl_rec_sys_type(rec_ptr,j);

            /* If any location tag has no 3GPP2 information continue to
            ** the next tag in this MLPL record
            */
            if(!SYS_IS_BIT_CONTAIN(BM(sys_type),comp_mask) ||
               !sdmmss_is_3gpp2_sys_rec_match_type(&sys_rec,sys_type))
            {
              continue;
            }
            /* If a match is found with the PRL sys rec and this location tag
            ** return TRUE, together with the PLMN found
            */
            if(sdmmss_match_3gpp2_loc_tag_with_sys_rec(&sys_rec,rec_ptr,j))
            {
              sdmmss_get_mlpl_plmn_list(rec_ptr,plmn_list);
              if(plmn_list->length > 0)
              {
                *plmn = *(&(plmn_list->info[0].plmn));
                /* deallocate memory for plmn_list
                */
                if(plmn_list)
                {
                  modem_mem_free((void *) plmn_list, MODEM_MEM_CLIENT_MMODE);
                  plmn_list = NULL;
                }
                return TRUE;
              }
            }
          }// for num_loc_tag
        }// for num_mlpl_recs
      }while( sdprl_parser_next(parser_ptr,&sys_rec) && 
              sys_rec.geo != PRL_NEW_GEO_REGION);

      /* deallocate memory for plmn_list
      */
      if(plmn_list)
      {
        modem_mem_free((void *) plmn_list, MODEM_MEM_CLIENT_MMODE);
        plmn_list = NULL;
      }
      break;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case SDMMSS_LOC_GRP_NONE:
    default:
		
      SD_MSG_HIGH_0("No MCC info found");
      break;
  }
  return FALSE;

} /*sdmmss_find_geo_plmn */

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_set_loc_grp_method(

DESCRIPTION
  This function decides which location grouping algorithm needs to be followed.
  It will check whether the MLPL table has any cdma information.
  CASE 1 : only MCC based MLPL records 
  CASE 2 : SID based MLPL records  
  loc grp is set as follows -
  SDMMSS_LOC_GRP_MCC_BASED - case 1
  SDMMSS_LOC_GRP_SID_BASED - case 2

DEPENDENCIES
  mlpl_ptr() should have a valid MLPL.

RETURN VALUE 


SIDE EFFECTS
  None.

===========================================================================*/
static  void                   sdmmss_set_loc_grp_method( void )
{
  sdmmss_mlpl_recs_s_type      *rec_ptr      = NULL;
  uint32                        i            = 0;
  byte                          j            = 0;
  int                           num_loc_tag  = 0;
  sdmmss_sys_type_e_type        sys_type     = SDMMSS_SYS_TYPE_MAX;
  int                           comp_mask    = SDMMSS_SYS_TYPE_3GPP2_ALL_MASK;
  sdmmss_loc_grp_e_type         *loc_grp     = sdmmss_get_loc_grp_method();



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (i = 0 ; (rec_ptr = sdmmss_get_mlpl_rec(i)) != NULL; i++)
  {
    /* If SYS_LOC_TAG bit is not set, then the MLPL record has only MCC/MNC 
    ** info. Continue to next MLPL record.
    */
    if(!SYS_IS_BIT_CONTAIN(rec_ptr->loc_param_type, 
                           SDMMSS_LOC_PARAM_TYPE_LOC_TAG))
    {
      continue;
    }
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    num_loc_tag = sdmmss_get_num_sys_loc_tag(rec_ptr);

    if (num_loc_tag > SDMMSS_MAX_NUM_SYS_LOC_TAG)
    {
      SD_ERR_1("num_loc_tag = %d, out of boundary",
                   num_loc_tag);

      num_loc_tag = SDMMSS_MAX_NUM_SYS_LOC_TAG;
    }

    for(j = 0; j < num_loc_tag ; j++)
    {
      /* Get sys type information from each location tag
      */
      sys_type = sdmmss_get_mlpl_rec_sys_type(rec_ptr,j);

      /* If any location tag has 3GPP2 information , go with CASE 2
      ** grouping (assume that grouping info is in MLPL and not in PRL)
      */
      if(SYS_IS_BIT_CONTAIN(BM(sys_type),comp_mask))
      {
        *loc_grp = SDMMSS_LOC_GRP_SID_BASED;
        SD_MSG_MED_0("EqPRL: sid based grouping");
        return;
      }
    }
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, there is only MCC/MNC info in the MLPL table.
  */
  *loc_grp = SDMMSS_LOC_GRP_MCC_BASED;
  
  SD_MSG_HIGH_0("EqPRL: MCC based grouping");

} /*  sdmmss_set_loc_grp_method( */

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_init_plmn_acq_rec

DESCRIPTION
  initializes a GWL acq record with the bands in the 3GPP band support table
  as per the PLMN specified and the access tech mask

DEPENDENCIES


RETURN VALUE 


SIDE EFFECTS
  None.

===========================================================================*/
static  boolean      sdmmss_init_plmn_acq_rec(
      const sys_plmn_id_s_type    * plmn_ptr,
      sys_access_tech_e_type        act,
      prli_acq_rec_type  *          acq_rec,
      const sys_plmn_rat_category_list_s_type     * mlpl_plmn_list
)
{
  sdmmss_bst_tbl_s_type *     bst_ptr   = sdmmss_bst_ptr(FALSE);
  int                         i         = 0;
  uint8 j = 0;
  int                         idx       = 0;
  sys_access_tech_e_type      rat       = SYS_ACT_NONE_MASK;
  boolean                     is_valid  = FALSE, break_outer_loop = FALSE;
  int                         pri_idx   = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_ASSERT(acq_rec  != NULL);
  SD_ASSERT(plmn_ptr != NULL);
  /* If BST does not have any entries 
  */
  if(bst_ptr->length == 0)
  {
    SD_ERR_0("BST invalid");
    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Get bands  to the PLMN and acc tech. If there are no bands
  ** to be entered , return FALSE. Eg. BST specifies that mcc = US has only 
  ** LTE bands. TheN MCC = US and act = gsm will have no bands will return
  ** FALSE
  */
  memset(acq_rec,0,sizeof(prli_acq_rec_type));
  for(i = 0; i< bst_ptr->length ; i++)
  {
    if(sys_plmn_id_is_undefined (bst_ptr->bst_tbl[i].plmn))
    {
      idx      = i;
      is_valid = TRUE;
      continue;
    }
   /* for finding right BST entry we need to not only match PLMN in search
   ** but all the associated PLMNs via MLPL as well */
    if (sys_mcc_match(*plmn_ptr,bst_ptr->bst_tbl[i].plmn))
    {
      idx      = i;
      is_valid = TRUE;
      break;
    }
   /* for finding right BST entry we need to not only match PLMN in search
   ** but all the associated PLMNs via MLPL as well */
   for(j = 0; j< mlpl_plmn_list->length; j++)
   {
     if (sys_mcc_match(mlpl_plmn_list->info[j].plmn,bst_ptr->bst_tbl[i].plmn))
     {
       idx      = i;
       is_valid = TRUE;
       break_outer_loop = TRUE;
       break;
     }
   }
   if(break_outer_loop == TRUE)
   {
     break;
   }
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!is_valid || idx >= SDMMSS_BST_MAX_LENGTH)
  {
    SD_ERR_0("No BST MCC match");
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  is_valid          = FALSE;
  acq_rec->acq_type = PRL_ACQ_REC_GWL_GENERIC;  
  acq_rec->type.gwl_generic.num_of_rats = 0;

  for( i = 0; i < bst_ptr->bst_tbl[idx].num_mode && \
            i < SYS_MAX_NUM_3GPP_RATS; i++)
  {
    rat = sdmmss_map_3gpp_sys_type_to_act_mask(
                                     bst_ptr->bst_tbl[idx].band_tbl[i].mode);

    if(!SYS_IS_BIT_CONTAIN(rat,act))
    {
      continue;
    }

    switch(rat)
    {
      case SYS_ACT_GSM_MASK:
        *(&(acq_rec->type.gwl_generic.gsm_rec)) = *(&(bst_ptr->bst_tbl[idx].band_tbl[i].gwl_rec));
        if (acq_rec->type.gwl_generic.gsm_rec.num_bands > 0)
        {
          pri_idx = acq_rec->type.gwl_generic.num_of_rats;

          if (pri_idx >= SYS_MAX_NUM_3GPP_RATS)
          {
            is_valid = FALSE;
            SD_ERR_1("pri_idx = %d, out of boundary", pri_idx);
            return is_valid;
          }

          acq_rec->type.gwl_generic.num_of_rats++;
          acq_rec->type.gwl_generic.pri_lst[pri_idx] = SYS_SYS_MODE_GSM;
          is_valid = TRUE;
        }
        break;

      case SYS_ACT_UMTS_MASK:
        *(&(acq_rec->type.gwl_generic.wcdma_rec)) = *(&(bst_ptr->bst_tbl[idx].band_tbl[i].gwl_rec));
        if(acq_rec->type.gwl_generic.wcdma_rec.num_bands > 0)
        {
          pri_idx = acq_rec->type.gwl_generic.num_of_rats;

          if (pri_idx >= SYS_MAX_NUM_3GPP_RATS)
          {
            is_valid = FALSE;
            SD_ERR_1("pri_idx = %d, out of boundary", pri_idx);
            return is_valid;
          }

          acq_rec->type.gwl_generic.num_of_rats++;
          acq_rec->type.gwl_generic.pri_lst[pri_idx] = SYS_SYS_MODE_WCDMA;
          is_valid = TRUE;
        }
        break;

      case SYS_ACT_LTE_MASK:
        *(&(acq_rec->type.gwl_generic.lte_rec)) = *(&(bst_ptr->bst_tbl[idx].band_tbl[i].gwl_rec));
        if (acq_rec->type.gwl_generic.lte_rec.num_bands > 0)
        {
          pri_idx = acq_rec->type.gwl_generic.num_of_rats;

          if (pri_idx >= SYS_MAX_NUM_3GPP_RATS)
          {
            is_valid = FALSE;
            SD_ERR_1("pri_idx = %d, out of boundary", pri_idx);
            return is_valid;
          }

          acq_rec->type.gwl_generic.num_of_rats++;
          acq_rec->type.gwl_generic.pri_lst[pri_idx] = SYS_SYS_MODE_LTE;
          is_valid = TRUE;
        }
        break;

      default:
        break;

    } // switch(rat)
  }// for num of modes

  if(!is_valid)
  {
    SD_MSG_HIGH_0("EqPRL:No BST rat match");
    return FALSE;
  }

  return TRUE;

} /* sdmmss_init_plmn_acq_rec */

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_init_plmn_based_sys_rec

DESCRIPTION
  initializes a PLMN based system record with the given PLMN id


DEPENDENCIES


RETURN VALUE 


SIDE EFFECTS
  None.

===========================================================================*/
static  void      sdmmss_init_plmn_based_sys_rec(
      const sys_plmn_id_s_type     *plmn_ptr,
      prli_sys_rec_type  *          sys_rec
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_ASSERT(sys_rec  != NULL);
  SD_ASSERT(plmn_ptr != NULL);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sys_rec->rec_type                 = PRL_SYS_REC_PLMN;
  sys_rec->acq_index                = 200;
  sys_rec->association_inc          = FALSE;
  sys_rec->data_association         = FALSE;
  sys_rec->pn_association           = FALSE;
  sys_rec->geo                      = PRL_SAME_GEO_REGION;
  sys_rec->neg_pref                 = PRL_PREFERRED;
  sys_rec->roam_ind                 = SYS_ROAM_STATUS_OFF;
  sys_rec->pri                      = PRL_PRI_SAME;
  sys_rec->rec.plmn.net_select_mode = SD_NET_SELECT_MODE_AUTO;
  sys_rec->rec.plmn.plmn_id         = *plmn_ptr;
  sys_rec->rec.plmn.lac             = PRL_WILDCARD_LAC;
  sys_rec->rec.plmn.num_lac = 0;
  sys_rec->association_tag = 0;

  /* this is will be calc at time of writing to prl 
  */
  sys_rec->sys_rec_length = 0x08;

  
} /* sdmmss_init_plmn_based_sys_rec*/

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_intersect_mlpl_plmn_selector

DESCRIPTION
  Determines the intersection of the MLPL list (containing PLMN ids)
  and a RAT list  and a  category and the PLMN selector list (all that have 
  not been entered into the GEO yet)

  For priority class ANY - insert all MCCs in MLPL List with wildcard MNC value
                           and with RATs determined by the sys_type and whether
                           some of the RATs have already been entered into the PRL.

  For priority class HOME/PREF - insert all PLMNs in PLMN DB which are found 
                             in the MLPL List and with RATs determined by 
                             the sys_type and whether some of the RATs have 
                             already been entered into the PRL.



DEPENDENCIES


RETURN VALUE 


SIDE EFFECTS
  None.

===========================================================================*/
static  void      sdmmss_intersect_mlpl_plmn_selector(
      const sys_plmn_rat_category_list_s_type     * mlpl_ptr,
      sdmmss_pri_class_e_type                     pri_class,
      sys_access_tech_e_type                      act_mask,
      sys_plmn_rat_category_list_s_type          *intersect_lst
)
{
  sys_plmn_rat_category_list_s_type  * plmn_sel_ptr =  sdmmss_plmn_selector_ptr(FALSE);
  uint32                                  i         = 0; // plmn selector index
  uint32                                  j         = 0; // mlpl index
  sys_access_tech_e_type                  common_act;
  sys_detailed_plmn_list_category_e_type  category;
  sys_plmn_id_s_type                      plmn;
  sys_mcc_type                            mcc;
  sys_mnc_type                            mnc;
  boolean                                 plmn_undefined;
  boolean                                 includes_pcs = TRUE;
  boolean                                 is_found     = FALSE;
  uint32                                  pos = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  intersect_lst->length = 0;

  if(pri_class == SDMMSS_PRI_CLASS_ANY)
  {
    /* for no. of mlpl plmns
    */
    for( j = 0; j < mlpl_ptr->length &&
                intersect_lst->length < SYS_MMSS_PLMN_LIST_MAX_LENGTH; j++)
    {
      /* Get MCC value 
      */
      sys_plmn_get_mcc_mnc2(mlpl_ptr->info[j].plmn,
                           &plmn_undefined,
                           &includes_pcs,
                           &mcc,
                           &mnc);
      /* set plmn =  mcc + wildcard mnc 
      */
      if(!sys_plmn_set_mcc_mnc2(TRUE,mcc,SYS_WILDCARD_MNC,&plmn))
      {
        return;
      }

      /* find if this MLPL MCC is found in the PLMN selector list   
      */
      is_found = FALSE;
      for (i = 0; i < plmn_sel_ptr->length &&
                  intersect_lst->length < SYS_MMSS_PLMN_LIST_MAX_LENGTH; i++) 
      {
        if(sys_plmn_match(plmn_sel_ptr->info[i].plmn,plmn))
        {
          /* Find what ACT bits need to entered into the EQ prl
          */
          common_act = SD_BIT_COMMON(act_mask,
                                     plmn_sel_ptr->info[i].geo_mcc_wcard_mnc_act_mask);
          if(common_act)
          {
            /* if the MCC has not been inserted into the Intersect list yet
            ** do so. This is to prevent duplicate MCC wildcard MNC entries 
            ** getting inserted into the EQ PRL
            */
            if(!is_found)
            {
              intersect_lst->info[intersect_lst->length].act = common_act;
              intersect_lst->info[intersect_lst->length].plmn = plmn;
              intersect_lst->length++;
            }
            plmn_sel_ptr->info[i].geo_mcc_wcard_mnc_act_mask =
                SD_BIT_DELETE(plmn_sel_ptr->info[i].geo_mcc_wcard_mnc_act_mask,
                              common_act);
            plmn_sel_ptr->info[i].prl_mcc_act_mask  = 
                      SD_BIT_DELETE(plmn_sel_ptr->info[i].prl_mcc_act_mask,
                                    common_act);
          }
          /* set flag to indicate that MLPL MCC is found in PLMN DB
          */
          is_found = TRUE;
        }
      } // for plmn_sel_ptr->length

      /* Add MCC plmn to intersect list if it was not found as part of PLMNDB
      */
      if(is_found == FALSE)
      {
        intersect_lst->info[intersect_lst->length].act = act_mask;
        intersect_lst->info[intersect_lst->length].plmn = plmn;
        intersect_lst->length++;
        /* Add plmn to plmn selector list . This wil ensure that the sys
        ** does not get entered into the PRL multiple times.
        ** Eg. mlpl has plmn 1 which is not listed in plmndb
        ** mspl has lte any and 3gpp any entries. plmn 1 should have 2 sys
        ** records - one for lte only and the other for gw only
        */
        common_act = SD_BIT_DELETE(SYS_ACT_3GPP_ANY_MASK,act_mask);
        sdmmss_insert_plmn_sel_rec(plmn,
                                   common_act,
                                   SYS_DETAILED_PLMN_LIST_CATEGORY_OTHER);
        plmn_sel_ptr->info[plmn_sel_ptr->length-1].geo_mcc_mnc_act_mask = common_act;
        plmn_sel_ptr->info[plmn_sel_ptr->length-1].geo_mcc_wcard_mnc_act_mask 
                                                                = common_act;
        plmn_sel_ptr->info[plmn_sel_ptr->length-1].prl_mcc_act_mask = common_act;
      }
    } // for no. of plmns in each mlpl rec
    return;
  } /* if(pri_class == SDMMSS_PRI_CLASS_ANY) */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For PRI_CLASS = HOME or PREFERRED 
  */

  for(i = 0; i< plmn_sel_ptr->length &&
             intersect_lst->length < SYS_MMSS_PLMN_LIST_MAX_LENGTH; i++)
  {
    category = plmn_sel_ptr->info[i].list_category;
    /* Break out of the loop if the pri is HOME ONLY and the list category is
    ** not. 
    */
    if(pri_class == SDMMSS_PRI_CLASS_HOME_ONLY && 
       category  != SYS_DETAILED_PLMN_LIST_CATEGORY_HPLMN )
    {
      break;
    }
    /* Break out of the loop if the pri is HOME PREF and the list category is
    ** OTHER 
    */
    if(pri_class == SDMMSS_PRI_CLASS_HOME_PREF && 
       category  == SYS_DETAILED_PLMN_LIST_CATEGORY_OTHER)
    {
      break;
    }
    /* Check if there is a PLMN ID match 
    */

    if(sys_detailed_plmn_list_find_plmn(mlpl_ptr,plmn_sel_ptr->info[i].plmn))
    {
      common_act = SD_BIT_COMMON(act_mask,plmn_sel_ptr->info[i].geo_mcc_mnc_act_mask);
      if(common_act)
      {
        pos                                  = intersect_lst->length;
        intersect_lst->info[pos].act         = common_act;
        intersect_lst->info[pos].plmn        = plmn_sel_ptr->info[i].plmn;
        intersect_lst->length++;
        plmn_sel_ptr->info[i].geo_mcc_mnc_act_mask =
                  SD_BIT_DELETE(plmn_sel_ptr->info[i].geo_mcc_mnc_act_mask,common_act);
        plmn_sel_ptr->info[i].prl_mcc_act_mask = 
                  SD_BIT_DELETE(plmn_sel_ptr->info[i].prl_mcc_act_mask,common_act);
      }
    }
  } // for each element in plmn selector 
} /* sdmmss_intersect_mlpl_plmn_selector */

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_insert_plmn_sel_systems_into_prl

DESCRIPTION



DEPENDENCIES


RETURN VALUE 


SIDE EFFECTS
  None.

===========================================================================*/
static  sdmmss_error_e_type       sdmmss_insert_plmn_sel_systems_into_prl(
    sdmmss_sys_type_e_type       sys_type,
       /* MMSS sys type
       */ 

    const sys_plmn_rat_category_list_s_type  *mlpl_lst,
       /* Pointer to the list of candidate PLMNs which will be inserted
       */

    sdmmss_pri_class_e_type      pri_class,
       /* Pri class of the systems to be inserted
       */

    int                          geo_rec_cnt
       /* This is used to decide whether the system inserted is the first
       ** in its GEO
       */
)
{
  sys_access_tech_e_type            rat_list;
     /* This represents the 3GPP rats mentioned in each record of the MSPL
     */

  sys_plmn_rat_category_list_s_type   *intersect_lst;
     /* This represents the intersection of the MLPL list , plmn list and  
     ** Rat (eg lte) and category(eg home) specified in the MSPL
     */

  uint32                            invalid_plmn_cnt = 0;
     /* This represents the number of records in the intersect_lst that did 
     ** not result in any entries being inserted into the sys rec table
     */
  prli_sys_rec_type                 sys_rec;
  prli_acq_rec_type                 acq_rec;
  uint32                            j              = 0;
  byte                              acq_idx        = 0;
     /* This will be populated with a valid acq_idx when GWL acq rec is 
     **  inserted into the acq rec table
     */

  boolean                           is_band       = FALSE;
     /* This represents whether the plmn rat combination has bands in the 
     ** 3gpp band support table. Eg. for mcc US and rat = gsm, is_band = false
     */

  sdmmss_error_e_type               err           = SDMMSS_ERROR_NO_ERR;


  rat_list = sdmmss_map_3gpp_sys_type_to_act_mask(sys_type); 

  if(rat_list == SYS_ACT_NONE_MASK)
  {
    return SDMMSS_ERROR_NO_RECS_FOUND;
  }

  /* allocate memory for the intersect list
  */
  intersect_lst = (sys_plmn_rat_category_list_s_type *)\
                  modem_mem_alloc( sizeof(sys_plmn_rat_category_list_s_type), 
                                                  MODEM_MEM_CLIENT_MMODE);
  if(intersect_lst == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }
 
  /* Initialize the length to 0
  */
  memset(intersect_lst,0,sizeof(sys_plmn_rat_category_list_s_type));
  intersect_lst->length = 0;

  /* now compute the intersection of the MLPL list 
  ** and the plmn selector list and the rat list (computed from sys type)
  */
  sdmmss_intersect_mlpl_plmn_selector(mlpl_lst,
                                    pri_class,
                                    rat_list,        
                                    intersect_lst);

  /* This indicates the number of PLMNs in the intersect_lst that
  ** did not result in an entry being inserted into the sys rec tbl
  */
  invalid_plmn_cnt = 0;

  for( j = 0; j < intersect_lst->length; j++ )
  {
    if ( j >= SYS_MMSS_PLMN_LIST_MAX_LENGTH )
    {
      SD_ERR_0("plmn intersect_list max array buffer reached");
      break;
    }
    /* Initialize an acquisition record entry for this PLMN AcT 
    ** combination 
    */
    is_band = sdmmss_init_plmn_acq_rec(&(intersect_lst->info[j].plmn),
                                      intersect_lst->info[j].act,
                                      &acq_rec, mlpl_lst);
    if(!is_band)
    {
      invalid_plmn_cnt++;
      continue;
    }

    sdmmss_init_plmn_based_sys_rec(&(intersect_lst->info[j].plmn),
                                 &sys_rec);

    /* If this is the first entry in the GEO, set the geo field 
    ** correctly
    */
    if( (j - invalid_plmn_cnt) == 0 &&  geo_rec_cnt == 0)
    {
      sys_rec.geo = PRL_NEW_GEO_REGION;
    }

    acq_idx = (byte)sdmmss_add_entry_to_acq_tbl(&acq_rec);

    /* Update the acq index in the system record
    */
    sys_rec.acq_index = acq_idx + (byte)prl_hdr_get_ptr()->num_acq_recs;

    sdmmss_add_entry_to_sys_tbl(&sys_rec);

  } // For each entry in the intersect list

  if((intersect_lst->length - invalid_plmn_cnt) == 0 )
  {
    err = SDMMSS_ERROR_RECS_ALREADY_INSERTED;
  }
  /* deallocate mem for intersect list
  */
  /*lint -e774*/
  if(intersect_lst != NULL)  /*lint +e744*/
  {
    modem_mem_free((void *) intersect_lst, MODEM_MEM_CLIENT_MMODE);
    intersect_lst = NULL;
  }

  return err;

} /* sdmmss_insert_plmn_sel_systems_into_prl */

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_parse_mspl

DESCRIPTION
  This function parses the MSPL table and inserts records into they
  system table

DEPENDENCIES


RETURN VALUE 
FALSE - if the entire MSPL did not result in adding any entries into the 
        system record table
TRUE - if at least one entry was inserted

SIDE EFFECTS
  None.

===========================================================================*/
static boolean                  sdmmss_parse_mspl(

    byte                                      mspl_idx,
        /* MSPL index */

    const sys_plmn_rat_category_list_s_type   *mlpl_lst,
       /* Pointer to MLPL plmn list */

    sdprl_parser_s_type                 *trav_parser
       /* prl parser pointing to last sys added to the
       ** system record table */
)
{
  byte                          i            = 0;   
  word                          geo_idx      = 0;

  sdmmss_mspl_tbl_s_type        *mspl_tbl    = sdmmss_get_mspl_rec(mspl_idx);

  prli_sys_rec_tbl_s_type       *sys_tbl_ptr = prl_get_sys_tbl_ptr(FALSE);


  int                           invalid_cnt = 0;
     /* This represents the number of MSPL records that did not result in 
     ** any entries being inserted into the sys rec table
     */

  sdmmss_sys_type_e_type        sys_type = SDMMSS_SYS_TYPE_MAX;
     /* This represents the sys type of the MSPL rec being parsed
     */

  sdmmss_error_e_type           err      = SDMMSS_ERROR_NO_ERR;
  sdmmss_error_e_type           err2     = SDMMSS_ERROR_NO_ERR;
  sdmmss_pri_class_e_type       pri_class;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Find the MSPL idx/table to use
  */
  if( mspl_tbl == NULL )
  {
    SD_MSG_HIGH_1("EqPRL:invalid mspl idx %d",mspl_idx);
    /* Try using the default MSPL index instead
    */
    mspl_idx = *sdmmss_get_dflt_mspl_idx();
    mspl_tbl = sdmmss_get_mspl_rec(mspl_idx);
    if(mspl_tbl == NULL)
    {
      SD_MSG_HIGH_1("EqPRL:No default MSPL %d",mspl_idx);
      return FALSE;
    }
    SD_MSG_HIGH_1("EqPRL:Using default mspl %d",mspl_idx);
  }
  /* set the value of the current geo_idx
  */
  if(trav_parser != NULL)
  {
    geo_idx = trav_parser->geo_idx;   
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Parse through the MSPL.
  */
  for(i = 0; i< mspl_tbl->num_mspl_recs ; i++)
  {
    
    sys_type  = mspl_tbl->mspl_recs[i].sys_type;
    pri_class = mspl_tbl->mspl_recs[i].pri_class;

    /* If the sys type is 3gpp2 and
    ** 1. all entries in geo have already been inserted
    ** OR 
    ** 2.if trav_parser == NULL indicating that this is a 3GPP only GEO,
    ** continue to next mspl entry
    */
    if(SYS_IS_BIT_CONTAIN(BM(sys_type),SDMMSS_SYS_TYPE_3GPP2_ALL_MASK) &&
       (  trav_parser == NULL ||
         ( trav_parser->geo_idx != geo_idx ||
           trav_parser->prl_insert_idx >= trav_parser->sys_tbl_num
         )
       )
     )
    {
      invalid_cnt++;
      continue;
    }

    switch(sys_type)
    {
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
      /* For 3gpp2/HDR/CDMA HOME , enter enter all entries in the geo 
      ** from the  current position of the prl parser till the first 
      ** entry in the GEO that matches the mmss sys type and all 
      ** subsequent entries with same priority
      */
      case SDMMSS_SYS_TYPE_3GPP2:
      case SDMMSS_SYS_TYPE_CDMA:
      case SDMMSS_SYS_TYPE_HDR:
        if (trav_parser == NULL)
        {
          SD_ERR_0("trav_parser is NULL");
          break;
        }
        switch(pri_class)
        {
          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
          case SDMMSS_PRI_CLASS_HOME_ONLY:
            /* Extract home systems from the PRL and insert into eqPRL
            */      

            err = sdmmss_extract_home_sys_from_prl(trav_parser,
                                                   sys_type,
                                                   (i-invalid_cnt));
            /* Increment invalid cnt if no HOME systems
            ** got inserted into the EQprl due to this MSPL record
            */
            if(err != SDMMSS_ERROR_NO_ERR)
            {
              
              if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_EQPRL)
              {
                SD_MSG_HIGH_3("Incorrect MSPL idx %d rec %d type %d",mspl_idx,
                                                                         i ,
                                                                  sys_type);
              }
              invalid_cnt++;
              continue;
            }
            break; /* SDMMSS_PRI_CLASS_HOME_ONLY */

          case SDMMSS_PRI_CLASS_HOME_PREF:
          {
            word prev_geo = trav_parser->geo_idx;
            /* Extract home systems from the PRL and insert into eqPRL
            */
            err = sdmmss_extract_home_sys_from_prl(trav_parser,
                                                   sys_type,
                                                   (i-invalid_cnt));
            /* If no home systems were found , increment invalid_cnt 
            ** and return . No point in looking for PREF systems if HOME is
            ** not present.
            */
            if(err == SDMMSS_ERROR_NO_RECS_FOUND)
            {
            
              if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_EQPRL)
              {
                SD_ERR_3("Incorrect MSPL idx %d rec %d  type %d",mspl_idx,
                                                                         i ,
                                            mspl_tbl->mspl_recs[i].sys_type);
              }
              invalid_cnt++;
              continue;
            }

            /* Moving on the next GEO so we skip parsing the pref_system */
            if (prev_geo != trav_parser->geo_idx)
            {
            
              if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_EQPRL)
              {
                SD_MSG_HIGH_0("eqPRL: No 3GPP2 pref systems");
              }

              if (err == SDMMSS_ERROR_NO_ERR)
              {
                /* if there were 3GPP2 home systems added, this is a valid MSPL
                ** and we need to adjust priority of the last system per the MSPL.
                */
                break;
              }
              else
              {
                /* if there were no systems added as part of HOME and the geo ended
                ** this is an incorrect mspl idx
                */
                if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_EQPRL)
                {
                  SD_ERR_3("Incorrect MSPL idx %d rec %d  type %d",mspl_idx,
                            i , mspl_tbl->mspl_recs[i].sys_type);
                }
                invalid_cnt++;
                continue;
              }
            } //prev_geo != trav_parser->geo_idx


            /* Extract PREF systems from the PRL and insert into eqPRL
            */
            err2 = sdmmss_extract_pref_sys_from_prl(trav_parser,
                                                    sys_type,
                                                   (i-invalid_cnt));
            /* Increment invalid cnt if no HOME systems and no PREF systems
            ** got inserted into the EQprl due to this MSPL record
            */
            if( err  != SDMMSS_ERROR_NO_ERR && 
                err2 != SDMMSS_ERROR_NO_ERR                   
              )
            {
              if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_EQPRL)
              {
                SD_MSG_HIGH_3("Incorrect MSPL idx %d rec %d  type %d",mspl_idx,
                            i , mspl_tbl->mspl_recs[i].sys_type);
              }
              invalid_cnt++;
              continue;
            }
            break; /* SDMMSS_PRI_CLASS_HOME_PREF*/
          }

          case SDMMSS_PRI_CLASS_ANY:
            /* Extract ANY system from the PRL and insert into eqPRL
            */
            err = sdmmss_extract_any_sys_from_prl(trav_parser,
                                                 sys_type,
                                                 (i-invalid_cnt));
            /* Increment invalid cnt if no systems
            ** got inserted into the EQprl due to this MSPL record
            */
            if( err != SDMMSS_ERROR_NO_ERR )
            {
              if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_EQPRL)
              {
                SD_MSG_HIGH_3("Incorrect MSPL idx %d rec %d  type %d",mspl_idx,
                                                                         i ,
                                                                  sys_type);
              }
              invalid_cnt++;
              continue;
            }
            break; /* SDMMSS_PRI_CLASS_ANY*/

          case SDMMSS_PRI_CLASS_MAX:
          default:
            break;
        } /* switch (pri_class)*/
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
      /* For 3gpp entries insert entries from plmn selector list
      */
      case SDMMSS_SYS_TYPE_3GPP:
      case SDMMSS_SYS_TYPE_LTE:
      case SDMMSS_SYS_TYPE_GSM:
      case SDMMSS_SYS_TYPE_WCDMA:
        err = sdmmss_insert_plmn_sel_systems_into_prl(sys_type,
                                                      mlpl_lst,
                                                      pri_class,
                                                     (i-invalid_cnt));
        
        if(err != SDMMSS_ERROR_NO_ERR)
        {
          if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_EQPRL)
          {
            SD_MSG_LOW_3("Incorrect MSPL idx %d rec %d  type %d",mspl_idx,
                                                                     i ,
                                                              sys_type);
          }
          invalid_cnt++;
          continue;
        }
        break;

       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
      default:
      case SDMMSS_SYS_TYPE_WIMAX:
      case SDMMSS_SYS_TYPE_MAX:
        SD_ERR_1("invalid sys_type %d",sys_type);
        break;                    
    }

    /* Now set the priority of the last system entered into the sys rec tbl
    ** Eg: if entry in mspl rec was LTE HOME MORE , and 2 LTE PLMNs were
    ** entered into the GEO. The priority of both entries wrt to each other
    ** is SDMMSS_SYS_PRI_SAME. But the second LTE rec should have MORE priority than any
    ** subsequent entry in the GEO
    */

    if(mspl_tbl->mspl_recs[i].sys_pri == SDMMSS_SYS_PRI_MORE)
    {
      sys_tbl_ptr->sys_rec_tbl[sys_tbl_ptr->num_entries-1].pri = PRL_PRI_HIGHER;
    }
    else
    {
      sys_tbl_ptr->sys_rec_tbl[sys_tbl_ptr->num_entries-1].pri = PRL_PRI_SAME;
    }
  } // for each mspl entry

  /* Check whether any record was inserted in the equivalent PRL due to this
  ** MSPL entry
  */
  if(invalid_cnt == mspl_tbl->num_mspl_recs)
  {
    return FALSE;
  }
  return TRUE;
} /*  sdmmss_parse_mspl*/

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_create_new_3gpp_geo

DESCRIPTION
Create new GEOs from
1. All MLPL entries with MCCs that have not been inserted into the PRL yet
2. All PLMN DB entries that have not been entered into the PRL yet.


DEPENDENCIES


RETURN VALUE 


SIDE EFFECTS
  None.

===========================================================================*/
static  void       sdmmss_create_new_3gpp_geo( 

  sys_plmn_rat_category_list_s_type      * plmn_lst_ptr 
  /* dynamically allocated variable to be used for plmn list operations
  */
)
{
  sys_plmn_rat_category_list_s_type  * plmn_sel_ptr =  sdmmss_plmn_selector_ptr(FALSE);
  sdmmss_mlpl_s_type                 *mlpl_ptr      =  sdmmss_mlpl_ptr(FALSE);
  uint32                             i              = 0;
  uint32                             j              = 0;
  sys_plmn_id_s_type                 plmn;
  sdmmss_error_e_type                err            = SDMMSS_ERROR_NO_ERR;
  sys_mcc_type                       mcc_digit_1    = 0;
  sys_mcc_type                       mcc_digit_2    = 0;
  sys_mcc_type                       mcc_digit_3    = 0;
  sys_mcc_type                       mcc            = 0;
  boolean                            is_insert      = FALSE;
  sdmmss_mlpl_recs_s_type           * rec_ptr       = NULL;

  /* -----------------------------------------------------------------------*/
  if (plmn_lst_ptr == NULL)
  {
    SD_ERR_0("plmn_lst_ptr is NULL");
    return;
  }
  /* insert MLPL entries into the PRL 
  */
  memset(plmn_lst_ptr,0,sizeof(sys_plmn_rat_category_list_s_type));
  plmn_lst_ptr->length = 0;

  for(i = 0; i < mlpl_ptr->num_mlpl_recs ; i++)
  {
    rec_ptr = sdmmss_get_mlpl_rec(i);

    if( rec_ptr == NULL )
    {
      continue;
    }  
    /* find list of PLMNs assoiciated with the MLPL
    */
    sdmmss_get_mlpl_plmn_list(rec_ptr, plmn_lst_ptr);

    /* Find if any PLMN in the list is already in the PRL
    */
    is_insert = FALSE;
    for(j = 0; j <  plmn_sel_ptr->length ; j++)
    {
      if(sys_detailed_plmn_list_find_plmn(plmn_lst_ptr,plmn_sel_ptr->info[j].plmn) &&
         plmn_sel_ptr->info[j].prl_mcc_act_mask == 0)
      {
        /* This MLPL has been processed and inserted into the PRL already
        */
        is_insert = TRUE;
        break;
      }      
    }
    if(is_insert)
    {
      continue;
    }

    /* if we got here, it means that the MLPL entries have not been inserted
    ** into the Eq PRL yet. Insert entries into the eqprl as per the MSPL
    */
    is_insert = sdmmss_parse_mspl(rec_ptr->mspl_index,plmn_lst_ptr, NULL);

    /* Append all remaining 3gpp ANY entries in the mlpl list into the
    ** sys record table 
    */
    err = sdmmss_insert_plmn_sel_systems_into_prl(SDMMSS_SYS_TYPE_3GPP,
                                                  plmn_lst_ptr,
                                                  SDMMSS_PRI_CLASS_ANY,
                                                  (is_insert ? 1:0 ) );

  } // for all MLPL records
  
  /* -----------------------------------------------------------------------*/
  /* insert any remaining PLMN db entries into PRL 
  */
 
  for( j = 0; j < plmn_sel_ptr->length ; j++ )
  {
    /* If the plmn is already in the PRL, continue
    */
    if(!plmn_sel_ptr->info[j].prl_mcc_act_mask)
    {
      continue;
    }
    /* Enter this PLMN into a new GE0 
    */
    plmn = plmn_sel_ptr->info[j].plmn;

    mcc_digit_1 = plmn.identity[0] & 0x0F;
    mcc_digit_2 = plmn.identity[0] / 0x10;
    mcc_digit_3 = plmn.identity[1] & 0x0F;

    mcc = 100 * mcc_digit_1 + 10 * mcc_digit_2 + mcc_digit_3;

    if(!sys_plmn_set_mcc_mnc2(TRUE,mcc,SYS_WILDCARD_MNC,&plmn))
    {
      continue;
    }
    plmn_lst_ptr->length = 0;
    sys_add_plmn_to_list(plmn_lst_ptr,&plmn);

    err = sdmmss_insert_plmn_sel_systems_into_prl(SDMMSS_SYS_TYPE_3GPP,
                                                plmn_lst_ptr,
                                                SDMMSS_PRI_CLASS_HOME_PREF,
                                                0);

    /* Enter this PLMN into the same GE0 as wilcard entry. 
    */
   err = sdmmss_insert_plmn_sel_systems_into_prl(SDMMSS_SYS_TYPE_3GPP,
                                          plmn_lst_ptr,
                                          SDMMSS_PRI_CLASS_ANY,
                                         ((err == SDMMSS_ERROR_NO_ERR)? 1:0) );

  } // For each entry in the PLMN db

} /* sdmmss_create_new_3gpp_geo */

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_set_plmn_sel_geo_act_mask

DESCRIPTION
  Sets the geo act mask for all entries in plmn sel list. This is done because
  PLMNs in the selector list may have to be entered into multiple GEOs in
  the PRL and so the mask has to be reset every time a new GEO is being
  parsed

DEPENDENCIES
 

RETURN VALUE 
  None

SIDE EFFECTS
  None.

===========================================================================*/
static  void         sdmmss_set_plmn_sel_geo_act_mask( void )
{
  sys_plmn_rat_category_list_s_type  * plmn_sel_ptr =  sdmmss_plmn_selector_ptr(FALSE);
  uint32  i = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( plmn_sel_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(i = 0; i < plmn_sel_ptr->length ; i++)
  {
    plmn_sel_ptr->info[i].geo_mcc_mnc_act_mask       = plmn_sel_ptr->info[i].act;
    plmn_sel_ptr->info[i].geo_mcc_wcard_mnc_act_mask = SYS_ACT_3GPP_ANY_MASK;
  }

} /*  sdmmss_set_plmn_sel_geo_act_mask*/


/* <EJECT> */
/*===========================================================================

FUNCTION  sdmmss_grouping_algorithm

DESCRIPTION
  This function goes about converting the PRL , MSPL, MLPL, BST , PLMN info
  into 3gpp acq record info in the acq record table cache and system record
  info for the equivalent PRL in the sys rec table cache

DEPENDENCIES


RETURN VALUE 


SIDE EFFECTS
  None.

===========================================================================*/
static  void                    sdmmss_grouping_algorithm( void )
{

  sdprl_parser_s_type         trav_parser;
  sdprl_parser_s_type         mcc_parser;
  prli_sys_rec_type           sys_rec;
  sys_plmn_id_s_type          geo_plmn;
  boolean                     is_geo_mcc_found  = FALSE;
  boolean                     is_valid_mspl             = FALSE;
  byte                        mspl_idx;
  dword                       rec_len           = 0;
  word                        last_geo_idx      = 0;
  sdmmss_error_e_type         err               = SDMMSS_ERROR_NO_ERR;
  sys_plmn_rat_category_list_s_type*        mlpl_plmn_list;
  /* used for local list manipulations */
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Allocate memory for mlpl_plmn_list
  */
  mlpl_plmn_list = (sys_plmn_rat_category_list_s_type *)\
                  modem_mem_alloc( sizeof(sys_plmn_rat_category_list_s_type), 
                                                  MODEM_MEM_CLIENT_MMODE);
  if(mlpl_plmn_list == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  } 

  /* Initialize the length to 0
  */
  mlpl_plmn_list->length = 0;

  /* Initialize PRL parser 
  */
  sdprl_parser_init( &trav_parser,
                      SD_SS_MAIN,
                      prl_buf_get_ptr(),
                      prl_hdr_get_ptr());

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* PSEUDO CODE
  for each geo in the PRL
  { 
  1. find the MCC associated
  2. if there is no MCC associated, go to step 7
  3. If there is an MCC,MNC identify the mspl index (by finding the best 
     match in the MLPL tbl )
  4. Create an MLPL list from the matching MLPL record . If there was no MLPL 
     match, the MLPL list will consist of the MCC found in the PRL and the
     default MSPL will be used to determine priority.
  5. Read the MSPL record for the idx 
  6. For each entry in the MSPL record, decide what systems to add to the 
     sys rec tbl. 
     for 3gpp entries, enter records into the acq tbl as per BST table
  7. enter all entries till the end of the GEO or end of PRL into the sys tbl.
  }
  */


  /* Get the first record from the PRL
  */
  rec_len = sdprl_parser_get(&trav_parser, &sys_rec);

  while(rec_len != 0)
  {

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Initialize the MCC parser to point at the beginning of the curr GEO
    ** It needs to traverse through the geo to find the mcc associated
    */
    mcc_parser             = trav_parser;
    last_geo_idx           = trav_parser.geo_idx;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Initialize the MLPL plmn list( the list of all plmns associated with
    ** each geo) for each geo.
    */
    mlpl_plmn_list->length = 0;

    /* indicate whether the MSPL resulted in any entries being inserted into the 
    ** eqprl
    */
    is_valid_mspl          = FALSE; 
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Find the PLMN associated with the current GEO
    */
    is_geo_mcc_found = sdmmss_find_geo_plmn(&mcc_parser,&geo_plmn);
	
    if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_EQPRL)
    {
      SD_MSG_HIGH_5("geo idx %d mccfound %d  geo plmn %x %x %x", last_geo_idx,is_geo_mcc_found,
		                                                       geo_plmn.identity[0],
		                                                       geo_plmn.identity[1],
		                                                       geo_plmn.identity[2]);
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* If the MCC for the GEO is found , parse the MLPL to find the MSPL idx
    ** and modify the GEO
    */
    if( is_geo_mcc_found )
    {
      /* Clear the PLMN selector list geo act mask
      */
      sdmmss_set_plmn_sel_geo_act_mask();

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
      /* If the MCC for the GEO is found , determine the associated MSPL 
      ** and the MLPL list
      */
      mspl_idx = sdmmss_determine_mspl_idx(&geo_plmn,mlpl_plmn_list);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Parse MSPL and update the sys rec table
      **
      */
      is_valid_mspl = sdmmss_parse_mspl(mspl_idx,mlpl_plmn_list,&trav_parser);

    }
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Append all remaining entries in the GEO into the
    ** sys record table (if we havent reached the end of the PRL yet)
    */
    if (trav_parser.prl_insert_idx < trav_parser.sys_tbl_num) 
    {
      rec_len = sdprl_parser_get(&trav_parser,&sys_rec);
      
      /* If this is not the first entry in the GEO, make sure the GEO field is
      ** corrected. 
      */
       if ( is_valid_mspl &&
            sys_rec.geo == PRL_NEW_GEO_REGION )
       {
         sys_rec.geo = PRL_SAME_GEO_REGION;
       }    
      while(trav_parser.geo_idx == last_geo_idx && rec_len != 0)
      {
        sdmmss_add_entry_to_sys_tbl(&sys_rec);
        trav_parser.prl_insert_idx++;
        rec_len = sdprl_parser_next(&trav_parser,&sys_rec);  
      }
    }
    else
    {
      rec_len = 0;
    }    
    /* Append all remaining 3gpp ANY entries in the GEO into the
    ** sys record table 
    */
    err = sdmmss_insert_plmn_sel_systems_into_prl(SDMMSS_SYS_TYPE_3GPP,
                                                  mlpl_plmn_list,
                                                  SDMMSS_PRI_CLASS_ANY,
                                                  (is_valid_mspl)? 1:0 );
  }// for each geo in the PRL
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Create a GEO for entries in MLPL/PLMN db that have not been entered into 
  ** the PRL yet. mlpl_plmn_list is cleared and used for list manipulations 
  ** within this function.
  */
  sdmmss_create_new_3gpp_geo(mlpl_plmn_list);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Add all remaining entries into encoded table
  */
  sdmmss_add_entries_to_enc_sys_tbl();

  /* dealloc mem for mlpl_plmn_list
  */
  if(mlpl_plmn_list)
   {
     modem_mem_free((void *)mlpl_plmn_list, MODEM_MEM_CLIENT_MMODE);
     mlpl_plmn_list = NULL;
   }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SYS_ARG_NOT_USED(err);

} /* sdmmss_grouping_algorithm */


/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_create_equivalent_prl

DESCRIPTION
  This function creates the equivalent PRL 

DEPENDENCIES


RETURN VALUE 


SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdmmss_create_equivalent_prl()
{
  sdmmss_file_s_type   *filedb_ptr   = NULL;
  sdss_subsc_e_type    subsc         = sdss_get_subsc_type(SD_SS_MAIN);
  uint32               num_plmn_recs = 0;
  uint8                num_mlpl_recs = 0;
  uint8                num_mspl_recs = 0;
  boolean              ret_status    = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If there is both 1x and GW subsc available, proceed 
  */
  SD_MSG_HIGH_1("EqPRL:Subsc type %d",subsc);
  if( subsc != SDSS_SUBSC_1X_GW)
  {
    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If mspl or mlpl is not updated by mmoc , return 
  */
  filedb_ptr = sdmmss_filedb_ptr(FALSE);
  if(filedb_ptr->mspl == NULL || 
     filedb_ptr->mlpl == NULL )
  {
    SD_MSG_HIGH_0("EqPRL: No mspl/mlpl");
    sdmmss_dealloc_dyn_memory();
    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Extract MSPL
  */
  sdmmss_extract_mspl_table(filedb_ptr->mspl);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Extract MLPL
  */
  sdmmss_extract_mlpl_table(filedb_ptr->mlpl);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Extract BST table from EFS
  */
  sdmmss_extract_bst();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Create the plmn selector list from the HOME plmn list
  ** and the PREF plmn list read from PLMN DB
  */
  sdmmss_create_plmn_selector_list();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  num_plmn_recs = sdmmss_plmn_selector_ptr(FALSE)->length;
  num_mlpl_recs = sdmmss_mlpl_ptr(FALSE)->num_mlpl_recs;
  num_mspl_recs = sdmmss_mspl_ptr(FALSE)->num_mspl_id;
  SD_MSG_HIGH_3("EqPRL: length - plmn %d mlpl %d mspl %d", num_plmn_recs,
                                                        num_mlpl_recs,
                                                        num_mspl_recs);
  if(num_plmn_recs == 0 ||
     num_mlpl_recs == 0 ||
     num_mspl_recs == 0)
  {
    sdmmss_dealloc_dyn_memory();
    return FALSE;
  }
  SD_MSG_MED_0("EqPRL:Create eq. PRL");
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Parse through the MLPL table and 
  ** find the default MSPL index
  */
  sdmmss_set_dflt_mspl_idx();  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Decide the algorithm to use for creating the equivalent PRL
  */
  sdmmss_set_loc_grp_method();
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Creates the equivalent PRL in the sys rec table cache and acq rec table
  ** cache
  */
  sdmmss_grouping_algorithm();
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Writes the equivalent PRL from the sys rec table cache and acq rec table
  ** cache into PRL format and into SD's prl buffer. Set return value to FALSE 
  ** if the eq PRL could not be validated successfully 
  */
  ret_status = sdprl_write_to_prl_buffer();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Deallocates the dynamic memory used for creating the equivalent PRL
  */
  sdmmss_dealloc_dyn_memory();
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ret_status;

} /*  sdmmss_create_equivalent_prl*/

/*===========================================================================

FUNCTION sdmmss_update_mmss_gwl_list 

DESCRIPTION
 Populate default bst entry into SDSR_MMSS_GWL_LST.  

DEPENDENCIES
 

RETURN VALUE 
  None

SIDE EFFECTS
  None.

===========================================================================*/
void         sdmmss_update_mmss_gwl_list ( void )
{
  sdmmss_bst_tbl_s_type        *bst_ptr = sdmmss_bst_ptr(FALSE);
  int                           i       = 0;
  int                           j       = 0;
  byte                          num_of_rats;
  prli_acq_rec_type             prl_acq_rec;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  sdsr_list_clr(SDSR_MMSS_GWL_LST);  
  prl_acq_rec.acq_type = PRL_ACQ_REC_GWL_GENERIC;

  /* Loop through the default bst entry to get rats and bands and then
  ** form the MMSS_GWL_LST 
  */
  for(i= 0; i< bst_ptr->length; i++)
  {
    if(!sys_plmn_id_is_undefined(bst_ptr->bst_tbl[i].plmn))
    {
      continue;
    }

    num_of_rats = 0;
    for(j= 0; j < bst_ptr->bst_tbl[i].num_mode; j++)
    {      
      switch(bst_ptr->bst_tbl[i].band_tbl[j].mode)
      {
        case SDMMSS_SYS_TYPE_GSM:
          prl_acq_rec.type.gwl_generic.gsm_rec = 
                                     bst_ptr->bst_tbl[i].band_tbl[j].gwl_rec;
          prl_acq_rec.type.gwl_generic.pri_lst[num_of_rats] =  SYS_SYS_MODE_GSM;
          num_of_rats++;
          break;
        case SDMMSS_SYS_TYPE_WCDMA:
          prl_acq_rec.type.gwl_generic.wcdma_rec =
                                     bst_ptr->bst_tbl[i].band_tbl[j].gwl_rec;
          prl_acq_rec.type.gwl_generic.pri_lst[num_of_rats] =  SYS_SYS_MODE_WCDMA;
          num_of_rats++;
          break;
        case SDMMSS_SYS_TYPE_LTE:
          prl_acq_rec.type.gwl_generic.lte_rec =
                                     bst_ptr->bst_tbl[i].band_tbl[j].gwl_rec;
          prl_acq_rec.type.gwl_generic.pri_lst[num_of_rats] =  SYS_SYS_MODE_LTE;
          num_of_rats++;
          break;
        default:
        case SDMMSS_SYS_TYPE_3GPP2:
        case SDMMSS_SYS_TYPE_3GPP:
        case SDMMSS_SYS_TYPE_WIMAX:
        case SDMMSS_SYS_TYPE_CDMA:
        case SDMMSS_SYS_TYPE_HDR:
        case SDMMSS_SYS_TYPE_MAX:
          break;
      }       
    } 

    if (num_of_rats == 0)
    {
      sys_err_fatal_invalid_value_exception((int)num_of_rats);
    }
    
    prl_acq_rec.type.gwl_generic.num_of_rats = num_of_rats;
    prl_acq_rec.is_valid             = TRUE;
    prl_acq_rec.plmn.net_select_mode = SD_NET_SELECT_MODE_AUTO;
    prl_acq_rec.plmn.num_lac         = 0;
    prl_acq_rec.plmn.lac             = PRL_WILDCARD_LAC;
    sys_plmn_undefine_plmn_id (&prl_acq_rec.plmn.plmn_id);
    sdprl_acq_rec_app(SDSR_MMSS_GWL_LST, 
                      SDSR_MAX, 
                      SDSR_MAX,
                      &prl_acq_rec,
                      SD_NO_CHANGE_SYS_IDX,
                      SDPRL_APPEND_ALL,
                      ACQ_IDX_WILDCARD);
  }
} 


/*===========================================================================

FUNCTION sdmmss_create_default_bst_entry 

DESCRIPTION
 Check if there exists an entry for which mcc is not defined.  If such entry does not exist,
 create a default bst entry. 

DEPENDENCIES
 

RETURN VALUE 
  None

SIDE EFFECTS
  None.

===========================================================================*/
void         sdmmss_create_default_bst_entry ( void )
{
  int                          i = 0;
  sdmmss_bst_tbl_s_type        *bst_ptr = sdmmss_bst_ptr(FALSE);
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return if there exists an entry for which mcc is not defined 
  */
  for(i= 0; i< bst_ptr->length; i++)
  {
    if(sys_plmn_id_is_undefined(bst_ptr->bst_tbl[i].plmn))
    {
      return;
    }
  }
  
  /* create a default entry 
  */
  SD_MSG_HIGH_0("MMSS_GWL: Create default BST entry");
  
  /* BST REC num + 1 */
  bst_ptr->length ++;
  
  /* Default plmn is 0xFFFFFF */
  (void)sys_plmn_set_mcc_mnc2(TRUE, SYS_WILDCARD_MCC, SYS_WILDCARD_MNC, 
                             &(bst_ptr->bst_tbl[bst_ptr->length - 1].plmn));
  
  /* 3 RATs */
  bst_ptr->bst_tbl[bst_ptr->length - 1].num_mode = 3;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* GSM bands */
  bst_ptr->bst_tbl[bst_ptr->length - 1].band_tbl[0].mode = SDMMSS_SYS_TYPE_GSM;
  sdmmss_default_gsm_bands(
    &(bst_ptr->bst_tbl[bst_ptr->length - 1].band_tbl[0].gwl_rec));
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* WCDMA bands */
  bst_ptr->bst_tbl[bst_ptr->length - 1].band_tbl[1].mode = SDMMSS_SYS_TYPE_WCDMA;
  sdmmss_default_wcdma_bands( 
    &(bst_ptr->bst_tbl[bst_ptr->length - 1].band_tbl[1].gwl_rec));
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* LTE bands */
  bst_ptr->bst_tbl[bst_ptr->length - 1].band_tbl[2].mode = SDMMSS_SYS_TYPE_LTE;
  sdmmss_default_lte_bands(
    &(bst_ptr->bst_tbl[bst_ptr->length - 1].band_tbl[2].gwl_rec));


} 

#endif // FEATURE_MMSS_3_1

