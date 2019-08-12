/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         TM LPP Encode/decode function

GENERAL DESCRIPTION
  This file contains TM's LPP protocol utilities, which encode/decode LPP messages, 
  The functions in this file are common LPP functions and can be used for UP/CP.

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2013 - 2016 Qualcomm Technologies, Inc. 
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.
 
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/lpp_common/src/tm_lpp.c#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  06/30/16   skm     set correct EarlyFixReport flags for greater than 24AD cases
  12/23/15   rk      Fixed the OSYS crash caused by LPP Enc ReqAD function by using global heap memory space
                     for OSYS encoding instead of OSYS memory allocation.
  10/01/15   rk      Use explicit LPP AGNSS position method bits to advertise the capability.
  08/16/15   rk      Added common function to send a request to TMCore to stop the OTDOA engine.
  08/04/15   gk      periodic reports shouldnt request for acks
  06/15/15   rk      Added functional support for LTE 256 bands to advertised in LPP session.
  04/20/15   rk      Added interface support for LTE 256 bands a.k.a LTE U.
  01/18/15   gk      Velocity is incorrectly propagated
  01/16/15   rk      Remove LPP InterFrequency feature flag.
  10/27/14   rk      Added LPP RSTD >24 measurement processing support
  09/24/14   rk      Added missing steering source type while injecting AcqAssist. 
  08/25/14   jv      16-bit SVID Support
  07/18/14   rk      Advertise LTE multi-band in LPPProvCap.
  06/04/14   rk      Added LPP R10 inter-freq, extended earfcn, confidence & dopplerUncExt support.
  03/05/14   sj      Fixed LPP CP Fuzz crash  
  07/12/13   rk      Return GNSS 'thereWereNotEnoughSatellitesReceived' error when GPS + GLO measurements aren't valid.
                     Also tweak the 'LocationError' condition not to set this location error if any of the measurements
                     are available in ProvLocationInfo msg.
  05/07/13   rk      In Glonass only position estimate session, LPP gnss-TOD-msec set to actual TOD mod 1 hour.
  04/05/13   rk      Request assistance data if not provided by LS in the out-of-order corner case.
  03/14/13   rk      Decoupled GPS & Glonass AD request processing to allow Glo only request to continue.
  03/05/13   rk      Resolved klocwork critical severity issues.
  02/25/13   rk      Fixed the crash due to null pointer reference in LPP Glonass req. AD function.
  02/07/13   rk      Handle GPS Clock Info when Glonass only measurement is available for processing.
  01/8/13    gk      ECID version
  12/11/12   rk      Added LPP UP A-Glonass MSB support & fixed compiler warnings.
  09/17/12   mj      Modified gps_meas_status_bits_check call
  06/11/12   rk      Added LPP Glonass support in two encode tm_lpp_encode_provide_cap &
                     tm_lpp_encode_request_assistance functions.
  12/14/11   gk      Lint and other fixes
  04/3/11    gk      Initial version

===========================================================================*/
#include "gps_variation.h"
#include "customer.h"   /* Customer configuration file */
#include "sm_api.h"
#include "tm_osysinit.h"
#include "tm_asn1util.h"
#include "tm_lpp.h"
#include "tm_lpp_asn1util.h"
#include "sm_api.h" /* included only for the data structures defined there */
#include "tm_umts_common_utils.h"
#include "msg.h"
#include "tm_lpp_tx_mgr.h"
#include "math.h"
#include "gnss_common.h"
#include "tm_lpp_cp.h"
#include "tm_lpp_common_utils.h"

#include <rfm.h>
#ifdef FEATURE_CGPS_LTE_CELLDB
  #include <lte_rrc_ext_api.h>
#endif

/****************************************************************************************************
                                            MACROS
*****************************************************************************************************/
/* To maintain backward compatibility on older PLs which doesn't have LTE_BAND_NUM definition.
*  The actual definition should come from sys.h file from newer PLs.
*/
#ifndef LTE_BAND_NUM
  typedef uint64 sys_lte_band_mask_e_type;
#endif

/****************************************************************************************************
                                            CONSTANTS
*****************************************************************************************************/
static boolean     b_force_lte_hybrid     = FALSE;

/*
* This table is generated based on the new GAD shape specification
*    for 3GPP release 8
*
*    r = C * ( ((1 + x) ^ k) - 1)
*
*    where C = 0.5, x = 0.14 and 'k' ranges from 0 to 127.
*/
static const float f_GadKTimeUncMapNewLpp[128] =
{
  0.000000F,         /*k=0*/
  0.070000F,         /*k=1*/
  0.149800F,         /*k=2*/
  0.240772F,         /*k=3*/
  0.344480F,         /*k=4*/
  0.462707F,         /*k=5*/
  0.597486F,         /*k=6*/
  0.751134F,         /*k=7*/
  0.926293F,         /*k=8*/
  1.125974F,         /*k=9*/
  1.353611F,         /*k=10*/
  1.613116F,         /*k=11*/
  1.908952F,         /*k=12*/
  2.246206F,         /*k=13*/
  2.630675F,         /*k=14*/
  3.068969F,         /*k=15*/
  3.568625F,         /*k=16*/
  4.138232F,         /*k=17*/
  4.787585F,         /*k=18*/
  5.527846F,         /*k=19*/
  6.371745F,         /*k=20*/
  7.333789F,         /*k=21*/
  8.430520F,         /*k=22*/
  9.680792F,         /*k=23*/
  11.106103F,        /*k=24*/
  12.730958F,        /*k=25*/
  14.583292F,        /*k=26*/
  16.694953F,        /*k=27*/
  19.102246F,        /*k=28*/
  21.846561F,        /*k=29*/
  24.975079F,        /*k=30*/
  28.541590F,        /*k=31*/
  32.607413F,        /*k=32*/
  37.242451F,        /*k=33*/
  42.526394F,        /*k=34*/
  48.550089F,        /*k=35*/
  55.417102F,        /*k=36*/
  63.245496F,        /*k=37*/
  72.169865F,        /*k=38*/
  82.343646F,        /*k=39*/
  93.941757F,        /*k=40*/
  107.163603F,       /*k=41*/
  122.236507F,       /*k=42*/
  139.419618F,       /*k=43*/
  159.008365F,       /*k=44*/
  181.339536F,       /*k=45*/
  206.797071F,       /*k=46*/
  235.818661F,       /*k=47*/
  268.903273F,       /*k=48*/
  306.619732F,       /*k=49*/
  349.616494F,       /*k=50*/
  398.632803F,       /*k=51*/
  454.511396F,       /*k=52*/
  518.212991F,       /*k=53*/
  590.832810F,       /*k=54*/
  673.619404F,       /*k=55*/
  767.996120F,       /*k=56*/
  875.585577F,       /*k=57*/
  998.237558F,       /*k=58*/
  1138.060816F,      /*k=59*/
  1297.459330F,      /*k=60*/
  1479.173636F,      /*k=61*/
  1686.327945F,      /*k=62*/
  1922.483857F,      /*k=63*/
  2191.701597F,      /*k=64*/
  2498.609821F,      /*k=65*/
  2848.485196F,      /*k=66*/
  3247.343123F,      /*k=67*/
  3702.041161F,      /*k=68*/
  4220.396923F,      /*k=69*/
  4811.322492F,      /*k=70*/
  5484.977641F,      /*k=71*/
  6252.944511F,      /*k=72*/
  7128.426743F,      /*k=73*/
  8126.476487F,      /*k=74*/
  9264.253195F,      /*k=75*/
  10561.318642F,     /*k=76*/
  12039.973252F,     /*k=77*/
  13725.639507F,     /*k=78*/
  15647.299038F,     /*k=79*/
  17837.990904F,     /*k=80*/
  20335.379630F,     /*k=81*/
  23182.402778F,     /*k=82*/
  26428.009167F,     /*k=83*/
  30128.000451F,     /*k=84*/
  34345.990514F,     /*k=85*/
  39154.499186F,     /*k=86*/
  44636.199072F,     /*k=87*/
  50885.336942F,     /*k=88*/
  58009.354114F,     /*k=89*/
  66130.733690F,     /*k=90*/
  75389.106406F,     /*k=91*/
  85943.651303F,     /*k=92*/
  97975.832486F,     /*k=93*/
  111692.519033F,    /*k=94*/
  127329.541698F,    /*k=95*/
  145155.747536F,    /*k=96*/
  165477.622191F,    /*k=97*/
  188644.559298F,    /*k=98*/
  215054.867599F,    /*k=99*/
  245162.619063F,    /*k=100*/
  279485.455732F,    /*k=101*/
  318613.489535F,    /*k=102*/
  363219.448069F,    /*k=103*/
  414070.240799F,    /*k=104*/
  472040.144511F,    /*k=105*/
  538125.834743F,    /*k=106*/
  613463.521607F,    /*k=107*/
  699348.484631F,    /*k=108*/
  797257.342480F,    /*k=109*/
  908873.440427F,    /*k=110*/
  1036115.792087F,   /*k=111*/
  1181172.072979F,   /*k=112*/
  1346536.233196F,   /*k=113*/
  1535051.375843F,   /*k=114*/
  1749958.638462F,   /*k=115*/
  1994952.917846F,   /*k=116*/
  2274246.396345F,   /*k=117*/
  2592640.961833F,   /*k=118*/
  2955610.766489F,   /*k=119*/
  3369396.343798F,   /*k=120*/
  3841111.901930F,   /*k=121*/
  4378867.638200F,   /*k=122*/
  4991909.177548F,   /*k=123*/
  5690776.532405F,   /*k=124*/
  6487485.316941F,   /*k=125*/
  7395733.331313F,   /*k=126*/
  8431136.067697F,   /*k=127*/
};

/****************************************************************************************************
                                            FUNCTION DECLARATIONS
*****************************************************************************************************/
/* Local function protocols */
uint8 tm_lpp_utils_microsec_to_GADK( float f_time_unc_microseconds );


/****************************************************************************************************
                                            FUNCTION DEFS
*****************************************************************************************************/
#ifdef FEATURE_CGPS_LTE_CELLDB

/*===========================================================================
  tm_lpp_fill_support_extended_bands_impl

  Description:
    Function to populate the supported extended LTE bands in the optional
    LPP bandEUTRA_v9a0 IE & fill the corresponding bandEUTRA IE to 64.
 
  Parameters:
    p_otdoa_prov_cap - Pointer to OTDOA provide capabilities struct to fill the
                       appropiate IEs.
    lte_band - The starting extended LTE band value.
 
  Return:
    None

===========================================================================*/

void GNSS_INLINE tm_lpp_fill_support_extended_bands_impl( OTDOA_ProvideCapabilities *const p_otdoa_prov_cap, uint8 lte_band )
{
  if ( p_otdoa_prov_cap != NULL )
  {
    p_otdoa_prov_cap->m.supportedBandListEUTRA_v9a0Present = 1; 
    /* bandEUTRA_v9a0 (65..256) */
    p_otdoa_prov_cap->supportedBandListEUTRA_v9a0.elem[p_otdoa_prov_cap->supportedBandListEUTRA_v9a0.n].m.bandEUTRA_v9a0Present = 1;
    p_otdoa_prov_cap->supportedBandListEUTRA_v9a0.elem[p_otdoa_prov_cap->supportedBandListEUTRA_v9a0.n].bandEUTRA_v9a0 = lte_band;
    p_otdoa_prov_cap->supportedBandListEUTRA_v9a0.n++;
    /* To indicate there are some extended LTE bands (>64) add 64 as last entry in the normal ‘bandEUTRA' IE */
    p_otdoa_prov_cap->supportedBandListEUTRA.elem[p_otdoa_prov_cap->supportedBandListEUTRA.n].bandEUTRA = ASN1V_maxBands;
    p_otdoa_prov_cap->supportedBandListEUTRA.n++;
  }
}

/*===========================================================================
  tm_lpp_fill_support_extended_bands

  Description:
    Helper function to fill the supported extended LTE bands in the optional
    LPP bandEUTRA_v9a0 IE & fill the corresponding bandEUTRA IE to 64.
 
  Parameters:
    u_ue_supp_extn_bands - bit map of supported extended LTE bands (65..256)
    camped_lte_cell_band - camped cell extended band number. (0..255)
    lte_band - The starting extended LTE band value.
    p_otdoa_prov_cap - Pointer to OTDOA provide capabilities struct to fill the
                       appropiate IEs.
  Return:
    None

===========================================================================*/
void tm_lpp_fill_support_extended_bands( uint64 u_ue_supp_extn_bands, sys_sband_lte_e_type const camped_lte_cell_band,
                                         uint8 lte_band, OTDOA_ProvideCapabilities *const p_otdoa_prov_cap, 
                                         uint16 const u_total_supp_bands )
{
  if ( (NULL == p_otdoa_prov_cap) || (p_otdoa_prov_cap->supportedBandListEUTRA_v9a0.n >= u_total_supp_bands) )
  {
    MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "OTDOA ptr null (%p) or bandEUTRA_v9a0 size reached", p_otdoa_prov_cap );
    return;
  }

  while ( u_ue_supp_extn_bands != 0 )
  {
    /* Exclude the camped LTE band, which is already populated as first entry.
       Check if the band at bit position - lte_band - is set. If set lte_band has the corresponding band value. */
    if ( (u_ue_supp_extn_bands & 0x1) && (lte_band != camped_lte_cell_band) &&
         (p_otdoa_prov_cap->supportedBandListEUTRA_v9a0.n < u_total_supp_bands) )
    {
      tm_lpp_fill_support_extended_bands_impl( p_otdoa_prov_cap, lte_band + 1 );
    }

    /* Shift to next supported LTE band */
    u_ue_supp_extn_bands = (u_ue_supp_extn_bands >> 1);
    lte_band++;
  }
}

/*===========================================================================
  tm_lpp_supported_lte_bands

  Description:
    Fill the supported LTE bands in the appropiate optionsl LPP parameters - bandEUTRA
    and/or bandEUTRA_v9a0 - if provided correctly. 

  Return:
    FALSE if LPP PDU should be de-allocated, else TRUE to continue encoding.
    The return value doesn't indicate whether LPP band parameters are populated or not,
    rather LPP PDU should be released or continue to use for encoding.

===========================================================================*/
boolean tm_lpp_supported_lte_bands( LPP_Message *p_lpp_pdu, OTDOA_ProvideCapabilities *const p_otdoa_prov_cap,
                                    boolean const camped_lte_cell_band_valid, sys_sband_lte_e_type const camped_lte_cell_band,
                                    sys_lte_band_mask_e_type const ue_supported_lte_bands )
{
  uint8   u_total_supp_normal_bands = 0,
          u_total_supp_ext_bands    = 0,
         lte_band                           = 0;
  uint16 u_total_supp_bands        = 0;
  boolean b_ue_has_normal_bands     = FALSE,
          b_ue_has_extn_bands       = FALSE;
  uint64  u_ue_supp_normal_bands    = 0;

  /* Sanity */
  if ( (NULL == p_lpp_pdu) || (NULL == p_otdoa_prov_cap) || (FALSE == camped_lte_cell_band_valid) )
  {
    return ( TRUE );
  }

  /* Get the normal bands into a variable */
#if (LTE_BAND_NUM == 256)
  u_ue_supp_normal_bands = ue_supported_lte_bands.bits_1_64;
#else
  u_ue_supp_normal_bands = ue_supported_lte_bands;
#endif

  /** 
  *  Check if there are normal LTE bands (1..63) & count total bands ( camped & UE supported LTE bands) to allocate space
  *  for normal band IEs. As per spec 64 isn't a valid band, so ignore when camped cell band is erroneous set to
  *  LTE band 64 (i.e. camped_lte_cell_band == 63).
  *  The camped band is subset of UE configured supported bands.
  **/
  if ( ( camped_lte_cell_band < (ASN1V_maxBands - 1)) || (u_ue_supp_normal_bands != 0) )
  {
    /* Set the MSB to 0, since as per spec LTE band 64 isn't valid & the value 64 is used as marker in bandEUTRA IE */
    u_ue_supp_normal_bands &= 0x7FFFFFFFFFFFFFFF;
    /* Now count total normal bands */
    u_total_supp_normal_bands += tm_util_num_bits_set_in_long64( u_ue_supp_normal_bands );

    if ( 0 == u_total_supp_normal_bands  )
    {
      /* Extreme case:
      *  For weird reason if camped cell band isn't configured as a supported band on UE, but camped cell band is valid
      *  then manually count for that.
      */
      ++u_total_supp_normal_bands;
    }

    b_ue_has_normal_bands = TRUE;
  }

#if (LTE_BAND_NUM == 256)
  /*
  * 3GPP doesn’t believe UE can support more than total 64 band classes. So check if maximum has already reached.
  */
  if ( u_total_supp_normal_bands < ASN1V_maxBands )
  {
    /** 
    *  Check if there are extended LTE bands (65..256) & count total bands ( camped & UE supported LTE bands) greater than
    *  64 to allocate space for extended band IEs.
    *  camped_lte_cell_band will be (0..63) or (0..254) when LTE U feature enabled.
    **/
    if ( (camped_lte_cell_band > (ASN1V_maxBands - 1)) || (ue_supported_lte_bands.bits_65_128 != 0) ||
         (ue_supported_lte_bands.bits_129_192 != 0) || (ue_supported_lte_bands.bits_193_256 != 0)
        )
    {
      /* First, count total supported extended bands by UE for allocating space. Remember camped band is subset of supported bands. */
      u_total_supp_ext_bands += tm_util_num_bits_set_in_long64( ue_supported_lte_bands.bits_65_128 );
      u_total_supp_ext_bands += tm_util_num_bits_set_in_long64( ue_supported_lte_bands.bits_129_192 );
      u_total_supp_ext_bands += tm_util_num_bits_set_in_long64( ue_supported_lte_bands.bits_193_256 );

      b_ue_has_extn_bands = TRUE;
    }
  }
#endif

  /* UE has atleast one LTE band supported */
  if ( (u_total_supp_normal_bands + u_total_supp_ext_bands) < 1 )
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Total LTE bands supported is zero (unusual!)" );
    return ( TRUE );
  }

  /* Account for empty space in extended band IE & 64 marker in normal band IE. The size of normal & extended band IE should be the same. */
  u_total_supp_bands = u_total_supp_normal_bands + u_total_supp_ext_bands;
  /* Trim excessive bands. LPP band IEs (normal + extended) can advertise maximum 64 bands */
  if ( u_total_supp_bands > ASN1V_maxBands )
  {
    MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Total bands exceeds limit (64) %u", u_total_supp_bands );
    u_total_supp_bands = ASN1V_maxBands;
  }

  MSG_7( MSG_SSID_GPSSM, MSG_LEGACY_MED, "Camp cell band %d (valid %d), total normal %u extended %u final %u,"\
            " normal %d extended %d band present, ", camped_lte_cell_band, camped_lte_cell_band_valid,\
            u_total_supp_normal_bands, u_total_supp_ext_bands, u_total_supp_bands, b_ue_has_normal_bands, b_ue_has_extn_bands );

  /* At this point there are 3 possible band combinations:-
  *  1. Only normal bands 2. Only extended bands 3. Both normal & extended bands available.
  *  In any case normal band IE should be used, so alloc the memory.
  */
  if ( (p_otdoa_prov_cap->supportedBandListEUTRA.elem = (SupportedBandEUTRA *)tm_lpp_asn1_osys_IE_malloc(sizeof(SupportedBandEUTRA) * (u_total_supp_bands))) == NULL )
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "LPP malloc failed!");
    return ( FALSE );
  }

  /* UE has extended bands, then allow space & populate if camped cell band is extended */
  if ( TRUE == b_ue_has_extn_bands )
  {
    /* Alloc space for extended band IE */
    if ( (p_otdoa_prov_cap->supportedBandListEUTRA_v9a0.elem = (SupportedBandEUTRA_v9a0 *)tm_lpp_asn1_osys_IE_malloc(sizeof(SupportedBandEUTRA_v9a0) * (u_total_supp_bands))) == NULL )
    {
      return ( FALSE );
    }

    /*** Populate 1: Populate camped cell LTE band of type extended ***/
    /* Check the camped cell LTE band is in extended range (64..255) & populate the IE accordingly. */
    if ( camped_lte_cell_band > (ASN1V_maxBands - 1) )
    {
      /* Extended bands :
      *  Fill camped extended LTE cell band IE.  Spec : bandEUTRA0v9a0 (65..256). MPSS : camped_lte_cell_band (64..255)
      */
      tm_lpp_fill_support_extended_bands_impl( p_otdoa_prov_cap, (uint8)camped_lte_cell_band );
    }
  }

  /*** Populate 2 : Populate camped cell LTE band of type normal ***/
  if ( camped_lte_cell_band < (ASN1V_maxBands - 1 ) )
  {
    /* Normal bands:
    *  Fill camped normal LTE cell band IE.  Spec : bandEUTRA (1..64). MPSS : camped_lte_cell_band (0..63)
    */
    p_otdoa_prov_cap->supportedBandListEUTRA.elem[p_otdoa_prov_cap->supportedBandListEUTRA.n].bandEUTRA = (uint8)camped_lte_cell_band + 1;
    p_otdoa_prov_cap->supportedBandListEUTRA.n++;
  }
  p_otdoa_prov_cap->m.supportedBandListEUTRAPresent = 1;

  /*** Populate 3 : Populate list of supported normal bands into normal band IE. ***/
  if ( TRUE == b_ue_has_normal_bands )
  {
    lte_band = 0; /* Start from LTE band 0..63 */

    while ( u_ue_supp_normal_bands != 0 )
  {
      /* Exclude the camped LTE band, which is already populated above.
         Check if the band at bit position - lte_band - is set. If set lte_band has the corresponding band value. */
      if ( (u_ue_supp_normal_bands & 0x1) && (lte_band != camped_lte_cell_band) &&
           (p_otdoa_prov_cap->supportedBandListEUTRA.n < u_total_supp_bands) )
    {
      /* bandEUTRA (1..64) */
      p_otdoa_prov_cap->supportedBandListEUTRA.elem[p_otdoa_prov_cap->supportedBandListEUTRA.n].bandEUTRA = lte_band + 1;
      p_otdoa_prov_cap->supportedBandListEUTRA.n++;
    }

    /* Shift to next supported LTE band */
      u_ue_supp_normal_bands = (u_ue_supp_normal_bands >> 1);
    lte_band++;
    }
  }

#if (LTE_BAND_NUM == 256)
  /*** Finally, populate list of supported extended bands into extended band IE. ***/
  if ( TRUE == b_ue_has_extn_bands )
  {
    /* Spec : Empty entries are used for those corresponding to SupportedBandEUTRA of non-64 value.
     *  So increase the supportedBandListEUTRA_v9a0 index to same as supportedBandListEUTRA.
     *  Note: No need to worry of out-of-bound array for the below assignment, since n is
     *        increment only after validation above.
     */
    if ( p_otdoa_prov_cap->supportedBandListEUTRA.n <= u_total_supp_bands )
    {
      p_otdoa_prov_cap->supportedBandListEUTRA_v9a0.n = p_otdoa_prov_cap->supportedBandListEUTRA.n;
    }

    /* Extended LTE band starting from 65..128. The camped cell band (2nd param) & lte_band (3rd param) start from zero. */
    tm_lpp_fill_support_extended_bands( ue_supported_lte_bands.bits_65_128, camped_lte_cell_band, 64, p_otdoa_prov_cap, u_total_supp_bands );
    /* Extended LTE band starting from 129..192 */
    tm_lpp_fill_support_extended_bands( ue_supported_lte_bands.bits_129_192, camped_lte_cell_band, 128, p_otdoa_prov_cap, u_total_supp_bands );
    /* Extended LTE band starting from 193..256 */
    tm_lpp_fill_support_extended_bands( ue_supported_lte_bands.bits_193_256, camped_lte_cell_band, 192, p_otdoa_prov_cap, u_total_supp_bands );
  }
#endif

  return ( TRUE );
}
#endif /*  FEATURE_CGPS_LTE_CELLDB */

#ifdef FEATURE_GNSS_LPP_DOPPLER_CONF_EARFCN
/*===========================================================================
  tm_lpp_doppuncext_to_ms

  Description:
    Convert extended doppler uncertainity enum to numeric. 
 
  Parameters:
    e_dopp_unc_ext [IN]: doppler uncertainity enum
 
  Return:
    Return the dopper uncertainity value.

===========================================================================*/
float tm_lpp_doppuncext_to_ms( GNSS_AcquisitionAssistElement_dopplerUncertaintyExt_r10 const e_dopp_unc_ext )
{
  switch ( e_dopp_unc_ext )
  {
     case GNSS_AcquisitionAssistElement_dopplerUncertaintyExt_r10_d60:
       return ( 60.0f );
       break;

     case GNSS_AcquisitionAssistElement_dopplerUncertaintyExt_r10_d80:
       return ( 80.0f );
       break;

     case GNSS_AcquisitionAssistElement_dopplerUncertaintyExt_r10_d100:
       return ( 100.0f );
       break;

     case GNSS_AcquisitionAssistElement_dopplerUncertaintyExt_r10_d120:
       return ( 120.0f );
       break;

       /* If LS choose not to provide doppler unc by setting ‘dopplerUncertaintyExt’ to "No Information",
        * target device would consider the Doppler uncertainty  to max possible value
        */
     default:
       return ( TM_MGP_MAX_DOPPLER_UNC_M_SEC );
  }
}
#endif /*FEATURE_GNSS_LPP_DOPPLER_CONF_EARFCN */

/*=========================================================================== 
  tm_lpp_encode_provide_cap_gnss_support_elem
 
  Description: 
    LPP helper function to populate ProvideCapabilities GNSS Support Elem IE
    with GPS & GLONASS parameters.
 
  Return: 
    True if successful, else False.

===========================================================================*/
boolean tm_lpp_encode_provide_cap_gnss_support_elem( const uint8 pos_mode_supported_flag,
                                                     GNSS_SupportElement *p_lpp_gnss_support_element,
                                                     GNSS_ID_gnss_id u_gnss_id )
{
  boolean b_ret = FALSE;

  /* Sanity check */
  if ( NULL == p_lpp_gnss_support_element )
  {
    return ( FALSE );
  }

  p_lpp_gnss_support_element->gnss_ID.gnss_id = u_gnss_id;  /* GPS or Glonass */

  p_lpp_gnss_support_element->agnss_Modes.posModes.numbits = 3;

  // A-GPS or A-GNSS Standalone
  if ( pos_mode_supported_flag & LPP_POS_MODE_STANDALONE )
  {
    // Set Bit 0 (MSB) - PositioningModes_posModes_standalone;
    OSSETBIT( p_lpp_gnss_support_element->agnss_Modes.posModes, PositioningModes_posModes_standalone );
  }

  /// Gnss-Signals
  p_lpp_gnss_support_element->gnss_Signals.gnss_SignalIDs.numbits = 8;
  if ( LPP_GNSS_ID_GPS == u_gnss_id )
  {
    // A-GPS
    if ( pos_mode_supported_flag & LPP_POS_MODE_UE_BASED )
    {
      // Set Bit 1 (MSB) - PositioningModes_posModes_ue_based;
      OSSETBIT( p_lpp_gnss_support_element->agnss_Modes.posModes, PositioningModes_posModes_ue_based );
    }

    if ( pos_mode_supported_flag & LPP_POS_MODE_UE_ASSISTED )
    {
      // Set Bit 2 (MSB) - PositioningModes_posModes_ue_assisted;
      OSSETBIT( p_lpp_gnss_support_element->agnss_Modes.posModes, PositioningModes_posModes_ue_assisted );
    }

    b_ret = tm_lpp_osys_asn1_int_to_bitstring( LPP_GNSS_SIGNAL_ID_GPS_L1CA_BMASK, &p_lpp_gnss_support_element->gnss_Signals.gnss_SignalIDs );
  }
  else if ( LPP_GNSS_ID_GLONASS == u_gnss_id )
  {
    // A-GNSS modes
    if ( pos_mode_supported_flag & LPP_AGNSS_POS_MODE_UE_BASED )
    {
      // Set Bit 1 (MSB) - PositioningModes_posModes_ue_based;
      OSSETBIT( p_lpp_gnss_support_element->agnss_Modes.posModes, PositioningModes_posModes_ue_based );
    }

    if ( pos_mode_supported_flag & LPP_AGNSS_POS_MODE_UE_ASSISTED )
    {
      // Set Bit 2 (MSB) - PositioningModes_posModes_ue_assisted;
      OSSETBIT( p_lpp_gnss_support_element->agnss_Modes.posModes, PositioningModes_posModes_ue_assisted );
    }

    b_ret = tm_lpp_osys_asn1_int_to_bitstring( LPP_GNSS_SIGNAL_ID_GLONASS_G1_BMASK, &p_lpp_gnss_support_element->gnss_Signals.gnss_SignalIDs );
  }

  // no fta_MeasSupport
  p_lpp_gnss_support_element->adr_Support = FALSE;                /* per RG */
  p_lpp_gnss_support_element->velocityMeasurementSupport = TRUE;  /* per RG */

  return ( b_ret );
}

/*=========================================================================== 
  tm_lpp_encode_provide_cap_generic_ad_support
 
  Description: 
    LPP helper function to populate ProvideCapabilities Generic AD Support IE
    with GPS & GLONASS parameters.
 
  Return: 
    True if successful, else False.

===========================================================================*/
boolean tm_lpp_encode_provide_cap_generic_ad_support( GNSS_GenericAssistDataSupportElement *p_lpp_generic_ad_element,
                                                      GNSS_ID_gnss_id u_gnss_id )
{
  if ( p_lpp_generic_ad_element == NULL )
  {
    return ( FALSE );
  }

  p_lpp_generic_ad_element->gnss_ID.gnss_id = u_gnss_id;

  /* nav model, rti, acq ass, alamananc, utc supported per RG */
  p_lpp_generic_ad_element->m.gnss_NavigationModelSupportPresent = 1;
  p_lpp_generic_ad_element->m.gnss_RealTimeIntegritySupportPresent = 1;
  p_lpp_generic_ad_element->m.gnss_AcquisitionAssistanceSupportPresent = 1;
#ifdef FEATURE_GNSS_LPP_DOPPLER_CONF_EARFCN
  p_lpp_generic_ad_element->gnss_AcquisitionAssistanceSupport.m.confidenceSupport_r10Present = 1;
  p_lpp_generic_ad_element->gnss_AcquisitionAssistanceSupport.m.dopplerUncertaintyExtSupport_r10Present = 1;
  p_lpp_generic_ad_element->gnss_AcquisitionAssistanceSupport.confidenceSupport_r10 = GNSS_AcquisitionAssistanceSupport_confidenceSupport_r10_true;
  p_lpp_generic_ad_element->gnss_AcquisitionAssistanceSupport.dopplerUncertaintyExtSupport_r10 = GNSS_AcquisitionAssistanceSupport_dopplerUncertaintyExtSupport_r10_true;
#endif /*FEATURE_GNSS_LPP_DOPPLER_CONF_EARFCN */
  p_lpp_generic_ad_element->m.gnss_AlmanacSupportPresent = 1;
  p_lpp_generic_ad_element->m.gnss_UTC_ModelSupportPresent = 1;
  p_lpp_generic_ad_element->m.gnss_TimeModelsSupportPresent = 1;
  p_lpp_generic_ad_element->m.gnss_AuxiliaryInformationSupportPresent = 1;

  return ( TRUE );
}

/*=========================================================================== 
  tm_lpp_encode_provide_cap
 
  Description: 
    LPP function used to encode positioning capabilities of target device (UE).
 
  Parameter:
   p_lpp_tx_type : Pointer to the right LPP transaction element from the transaction table.
 
  Return: 
    True if the encoding of UE capabilities is successful, else return False.

===========================================================================*/
boolean tm_lpp_encode_provide_cap(
                                   tm_lpp_transaction_info_s_type *p_lpp_tx_type
                                  )
{
  LPP_Message                          *p_lpp_pdu;
  ProvideCapabilities                  *p_lpp_prov_cap;
  ProvideCapabilities_r9_IEs           *p_lpp_prov_cap_r9;
  GNSS_SupportElement                  *p_lpp_gnss_support_element;
  GNSS_GenericAssistDataSupportElement *p_lpp_generic_ad_element;
  int                                  encode_status;
  uint8                                num_gnss_system             = 0; // Number of GNSS (GPS, GLONASS) support
  boolean                              b_ret                       = TRUE;
#ifdef FEATURE_CGPS_LTE_CELLDB
  sys_sband_lte_e_type                 camped_cell_band            = SYS_SBAND_LTE_EUTRAN_BAND_MAX;
#endif

  p_lpp_pdu = tm_lpp_asn1_osys_IE_malloc(sizeof(LPP_Message));
  p_lpp_tx_type->z_lpp_data_type.p_pdu_ptr = p_lpp_pdu;

  MSG_MED("TM_LPP: Encode Provide Cap", 0, 0, 0);
  if ( p_lpp_pdu == NULL )
  {
    MSG_ERROR("TM_LPP: Couldnt allocate memory for LPP Prov Cap Msg", 0, 0, 0);
    return ( FALSE );
  }
  /* Initialize OSS rrlp structure */
  p_lpp_pdu->m.transactionIDPresent = 0;
  if ( p_lpp_tx_type->z_current_trans_id.m.u_trans_id_valid )
  {
    p_lpp_pdu->m.transactionIDPresent = 1;
    p_lpp_pdu->transactionID.initiator = (uint8)p_lpp_tx_type->z_current_trans_id.u_initiator;
    p_lpp_pdu->transactionID.transactionNumber = p_lpp_tx_type->z_current_trans_id.u_transaction_id;
  }
  p_lpp_pdu->endTransaction = TRUE;

  /* LPP CP Specific Changes : BEGIN */
  if ( p_lpp_tx_type->u_lpp_session_type == TM_LPP_SESSION_TYPE_CP )
  {
    p_lpp_pdu->m.sequenceNumberPresent = 1;
    tm_lpp_cp_get_nxt_out_seq_no(p_lpp_tx_type);
    p_lpp_pdu->sequenceNumber = p_lpp_tx_type->u_out_sequence_num;

    p_lpp_pdu->m.acknowledgementPresent = 1;
    p_lpp_pdu->acknowledgement.ackRequested = 1;           /* Request Server ACK to LPP Prov Cap           */
    p_lpp_pdu->acknowledgement.m.ackIndicatorPresent = 0;  /* Make sure clear unused field                 */
  }
  /* LPP CP Specific Changes : END */

  p_lpp_pdu->m.lpp_MessageBodyPresent = 1;

  p_lpp_pdu->lpp_MessageBody.t = LPP_MESSAGEBODY_C1;
  if ( (p_lpp_pdu->lpp_MessageBody.u.c1 = (LPP_MessageBody_c1 *)tm_lpp_asn1_osys_IE_malloc(sizeof(LPP_MessageBody_c1))) == NULL )
  {
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_CAP);
    return ( FALSE );
  }
  p_lpp_pdu->lpp_MessageBody.u.c1->t = LPP_MESSAGE_PROV_CAP;
  if ( (p_lpp_pdu->lpp_MessageBody.u.c1->u.provideCapabilities =  (ProvideCapabilities *)
                                                                 tm_lpp_asn1_osys_IE_malloc(sizeof(ProvideCapabilities))) == NULL )
  {
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_CAP);
    return ( FALSE );
  }
  p_lpp_prov_cap = p_lpp_pdu->lpp_MessageBody.u.c1->u.provideCapabilities;
  p_lpp_prov_cap->criticalExtensions.t = LPP_PROV_CAP_C1;
  if ( (p_lpp_prov_cap->criticalExtensions.u.c1 = (ProvideCapabilities_criticalExtensions_c1 *)
                                                  tm_lpp_asn1_osys_IE_malloc(sizeof(ProvideCapabilities_criticalExtensions_c1))) == NULL )
  {
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_CAP);
    return ( FALSE );
  }
  p_lpp_prov_cap->criticalExtensions.u.c1->t = LPP_PROV_CAP_C1_PROV_CAP_R9;

  if ( (p_lpp_prov_cap->criticalExtensions.u.c1->u.provideCapabilities_r9 =
                                                                            (ProvideCapabilities_r9_IEs *)tm_lpp_asn1_osys_IE_malloc(sizeof(ProvideCapabilities_r9_IEs))) == NULL )
  {
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_CAP);
    return ( FALSE );
  }
  p_lpp_prov_cap_r9 = p_lpp_prov_cap->criticalExtensions.u.c1->u.provideCapabilities_r9;
  if ( p_lpp_tx_type->z_lpp_data_type.p_lpp_prov_cap_type->q_flags & LPP_REQ_CAP_AGNSS_REQ )
  {
    if ( (p_lpp_tx_type->z_lpp_data_type.p_lpp_prov_cap_type->z_agnss_capabilities.u_pos_mode_supported) != 0 )
    {
      /* a-gnss-ProvideCapabilities */
      p_lpp_prov_cap_r9->m.a_gnss_ProvideCapabilitiesPresent = 1;
      p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.m.gnss_SupportListPresent = 1;

      num_gnss_system = (p_lpp_tx_type->z_lpp_data_type.p_lpp_prov_cap_type->u_glonass_supported == TRUE) ? 2 : 1; // num of elem to allocate
      p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.gnss_SupportList.n = num_gnss_system;

      p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.gnss_SupportList.elem  =
                                                                             tm_lpp_asn1_osys_IE_malloc(p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.gnss_SupportList.n *
                                                                                                        sizeof(GNSS_SupportElement));
      if ( p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.gnss_SupportList.elem == NULL )
      {
        tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_CAP);
        return ( FALSE );
      }

      // GPS - populate gnss_SupportList
      p_lpp_gnss_support_element = &p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.gnss_SupportList.elem[0];
      if ( !tm_lpp_encode_provide_cap_gnss_support_elem(p_lpp_tx_type->z_lpp_data_type.p_lpp_prov_cap_type->z_agnss_capabilities.u_pos_mode_supported,
                                                        p_lpp_gnss_support_element, LPP_GNSS_ID_GPS) )
      {
        tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_CAP);
        return ( FALSE );
      }

      p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.m.assistanceDataSupportListPresent = 1;

      /* Assistance data support list */
      /* Ref time support - gnss-SystemTime (GPS) */
      p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.assistanceDataSupportList.gnss_CommonAssistanceDataSupport.m.gnss_ReferenceTimeSupportPresent = 1;
      p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.assistanceDataSupportList.gnss_CommonAssistanceDataSupport.gnss_ReferenceTimeSupport.gnss_SystemTime.gnss_ids.numbits = 16;
      OSSETBIT(p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.assistanceDataSupportList.gnss_CommonAssistanceDataSupport.gnss_ReferenceTimeSupport.gnss_SystemTime.gnss_ids,
               LPP_GNSS_ID_BITMAP_GPS);

      /* FTA not supported */
      p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.assistanceDataSupportList.gnss_CommonAssistanceDataSupport.gnss_ReferenceTimeSupport.m.fta_SupportPresent = 0;

      /* refLocationSupport */
      p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.assistanceDataSupportList.gnss_CommonAssistanceDataSupport.m.gnss_ReferenceLocationSupportPresent = 1;

      /* iono*/
      p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.assistanceDataSupportList.gnss_CommonAssistanceDataSupport.m.gnss_IonosphericModelSupportPresent = 1;
      p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.assistanceDataSupportList.gnss_CommonAssistanceDataSupport.gnss_IonosphericModelSupport.ionoModel.numbits = 8;
      OSSETBIT(p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.assistanceDataSupportList.gnss_CommonAssistanceDataSupport.gnss_IonosphericModelSupport.ionoModel,
               LPP_IONO_MODEL);

      /* EarthOrientationParameters not supported by UE */
      p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.assistanceDataSupportList.gnss_CommonAssistanceDataSupport.m.gnss_EarthOrientationParametersSupportPresent = 0;

      /* GPS - gnss GenericAssistanceDataSupport */
      p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.assistanceDataSupportList.gnss_GenericAssistanceDataSupport.n = num_gnss_system;
      if ( (p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.assistanceDataSupportList.gnss_GenericAssistanceDataSupport.elem =
                                                                                                                             (GNSS_GenericAssistDataSupportElement *)
                                                                                                                             tm_lpp_asn1_osys_IE_malloc(p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.assistanceDataSupportList.gnss_GenericAssistanceDataSupport.n * sizeof(GNSS_GenericAssistDataSupportElement))) == NULL )
      {
        tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_CAP);
        return ( FALSE );
      }

      p_lpp_generic_ad_element = &p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.assistanceDataSupportList.gnss_GenericAssistanceDataSupport.elem[0];

      // Populate GenericAssistDataSupportElement for GPS
      if ( !tm_lpp_encode_provide_cap_generic_ad_support(p_lpp_generic_ad_element, LPP_GNSS_ID_GPS) )
      {
        tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_CAP);
        return ( FALSE );
      }


      /* Glonass */
      if ( p_lpp_tx_type->z_lpp_data_type.p_lpp_prov_cap_type->u_glonass_supported == TRUE )
      {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "TM_LPP: Encode Provide Cap - Glonass enabled");

        /* gnss_SupportList */
        p_lpp_gnss_support_element = &p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.gnss_SupportList.elem[1];

        if ( !tm_lpp_encode_provide_cap_gnss_support_elem(p_lpp_tx_type->z_lpp_data_type.p_lpp_prov_cap_type->z_agnss_capabilities.u_pos_mode_supported,
                                                          p_lpp_gnss_support_element,  LPP_GNSS_ID_GLONASS) )
        {
          tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_CAP);
          return ( FALSE );
        }

        /* Assistance data support list */
        /* Ref time support - gnss-SystemTime (Glonass) */
        p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.assistanceDataSupportList.gnss_CommonAssistanceDataSupport.m.gnss_ReferenceTimeSupportPresent = 1;
        p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.assistanceDataSupportList.gnss_CommonAssistanceDataSupport.gnss_ReferenceTimeSupport.gnss_SystemTime.gnss_ids.numbits = 16;
        OSSETBIT(p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.assistanceDataSupportList.gnss_CommonAssistanceDataSupport.gnss_ReferenceTimeSupport.gnss_SystemTime.gnss_ids,
                 LPP_GNSS_ID_BITMAP_GLONASS);

        /* FTA not supported (set to zero in the above GPS section) */

        /* refLocationSupport (set once in above GPS section) */

        /* iono (set once in above GPS section) */

        /* EarthOrientationParameters (set to zero in the above GPS section) */

        /* Glonass - gnss GenericAssistanceDataSupport */
        p_lpp_generic_ad_element = &p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.assistanceDataSupportList.gnss_GenericAssistanceDataSupport.elem[1];

        // Populate GenericAssistDataSupportElement for Glonass
        if ( !tm_lpp_encode_provide_cap_generic_ad_support(p_lpp_generic_ad_element, LPP_GNSS_ID_GLONASS) )
        {
          tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_CAP);
          return ( FALSE );
        }
      }

      /* locationCoordinateTypes */
      p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.m.locationCoordinateTypesPresent = 1;
      p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.locationCoordinateTypes.ellipsoidPointWithAltitudeAndUncertaintyEllipsoid = TRUE;

      /* velocityTypes */
      p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.m.velocityTypesPresent = 1;
      p_lpp_prov_cap_r9->a_gnss_ProvideCapabilities.velocityTypes.horizontalWithVerticalVelocityAndUncertainty = TRUE;
    }
  }
  if ( p_lpp_tx_type->z_lpp_data_type.p_lpp_prov_cap_type->q_flags & LPP_REQ_CAP_OTDOA_REQ )
  {
    if ( p_lpp_tx_type->z_lpp_data_type.p_lpp_prov_cap_type->u_otdoa_supported )
    {
      p_lpp_prov_cap_r9->m.otdoa_ProvideCapabilitiesPresent = 1;
      p_lpp_prov_cap_r9->otdoa_ProvideCapabilities.otdoa_Mode.numbits = 1;
      OSSETBIT(p_lpp_prov_cap_r9->otdoa_ProvideCapabilities.otdoa_Mode, LPP_PROV_OTDOA_CAP);

      if ( TRUE == tm_core_lpp_grt_24_admeas_enabled() )
      {
        /* Advertise >24 AD/Measurement */
        p_lpp_prov_cap_r9->otdoa_ProvideCapabilities.m.additionalNeighbourCellInfoList_r10Present = TRUE;
        p_lpp_prov_cap_r9->otdoa_ProvideCapabilities.additionalNeighbourCellInfoList_r10 = OTDOA_ProvideCapabilities_additionalNeighbourCellInfoList_r10_supported;
      }

#ifdef FEATURE_CGPS_LTE_CELLDB
      if ( p_lpp_tx_type->z_lpp_data_type.q_flags & LPP_CELL_INFO_VALID )
      {
        sys_lte_band_mask_e_type ue_support_lte_bands;
        boolean camped_cell_band_valid = FALSE;

        /* First get the band of camped cell */
        camped_cell_band = lte_rrc_get_band_from_dl_earfcn(p_lpp_tx_type->z_lpp_data_type.z_cached_cell_info.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.freq, &camped_cell_band_valid);

        /* Secondly get list of other UE supported bands. Atpresent UE doesn't support band more than 63.*/
        ue_support_lte_bands = rfm_get_lte_band_mask();

        /* Populate the LPP param with supported bands */
        if ( FALSE == tm_lpp_supported_lte_bands(p_lpp_pdu, &p_lpp_prov_cap_r9->otdoa_ProvideCapabilities, camped_cell_band_valid, camped_cell_band, ue_support_lte_bands) )
        {
          tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_CAP);
          return ( FALSE );
        }
      }
#endif

      /* Check if LTE OTDOA inter-frequency is supported or not in RRC layer */
      if ( TRUE == lte_rrc_cap_inter_freq_rstd_meas_supported() )
      {
        /* Indicate the device supports inter-freq RSTD meas */
        p_lpp_prov_cap_r9->otdoa_ProvideCapabilities.m.interFreqRSTDmeasurement_r10Present = 1;
        p_lpp_prov_cap_r9->otdoa_ProvideCapabilities.interFreqRSTDmeasurement_r10 = OTDOA_ProvideCapabilities_interFreqRSTDmeasurement_r10_supported;
      }
      else
      {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Inter-freq not supported by RRC");
      }
    }
  }

  if ( p_lpp_tx_type->z_lpp_data_type.p_lpp_prov_cap_type->q_flags & LPP_REQ_CAP_ECID_REQ )
  {

    if ( p_lpp_tx_type->z_lpp_data_type.p_lpp_prov_cap_type->u_ecid_supported )
    {
      p_lpp_prov_cap_r9->m.ecid_ProvideCapabilitiesPresent = 1;
      p_lpp_prov_cap_r9->ecid_ProvideCapabilities.ecid_MeasSupported.numbits = 3;
      OSSETBIT(p_lpp_prov_cap_r9->ecid_ProvideCapabilities.ecid_MeasSupported, LPP_PROV_ECID_RSRP);
      OSSETBIT(p_lpp_prov_cap_r9->ecid_ProvideCapabilities.ecid_MeasSupported, LPP_PROV_ECID_RSRQ);
      OSSETBIT(p_lpp_prov_cap_r9->ecid_ProvideCapabilities.ecid_MeasSupported, LPP_PROV_ECID_RXTX);
    }
  }
  /* Do the ASN.1 encoding for LPP */
  memset((void *)&p_lpp_tx_type->z_lpp_data_type.z_pdu_buf, 0, sizeof(p_lpp_tx_type->z_lpp_data_type.z_pdu_buf));  /* crash 3 */
  encode_status = tm_lpp_osys_asn1_encode_pdu((void *)p_lpp_pdu, &p_lpp_tx_type->z_lpp_data_type.z_pdu_buf);

  if ( (encode_status != 0) || (p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value == NULL) || (p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.length == 0) )
  {
    /* ASN.1 encoding failed */
    MSG_ERROR("TM_LPP: Error encoding LPP Provide Cap", 0, 0, 0);

    if ( p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value != NULL )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "LPP ASN1 enc. err. Freeing: %d,%lx,%u",
            encode_status,
            p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value,
            p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.length);

      tm_lpp_asn1_free_osys_buf(p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value);
    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "LPP ASN1 enc. err. No alloc.: %d,%lx,%u",
            encode_status,
            p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value,
            p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.length);
    }
    b_ret = FALSE;
  }
  else
  {
    b_ret = TRUE;
  }

  /* free all the allocated buffers */
  tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_CAP);

  return ( b_ret );
}


/*=========================================================================
  tm_lpp_encode_request_assistance
  
  Description:
    LPP function to encode Request Assistance message
    

  Returns:
    TRUE if the encoding passed, FALSE otherwise    
==========================================================================*/
boolean tm_lpp_encode_request_assistance( tm_lpp_transaction_info_s_type *p_lpp_tx_type )
{
  LPP_Message                                 *p_lpp_pdu;
  tm_lpp_transaction_id_s_type                z_trans_id;
  RequestAssistanceData_criticalExtensions_c1 *p_req_ad_c1                = NULL;
  RequestAssistanceData_r9_IEs                *p_req_ad_r9                = NULL;
  GNSS_GenericAssistDataReqElement            *p_generic_ad_req           = NULL;
  int                                         encode_status;
  boolean                                     b_ret                       = TRUE;
  uint8                                       u_i                         = 0;
  uint8                                       u_GnssGenericADReqElemIndex = 0,
                                              u_GnssTimeRPLElemIndex      = 0;
  boolean                                     b_GpsReqGenericAd           = FALSE; /* Request GPS generic AD or not */

  sm_InternalAssistDataStatusStructType       *p_GnssAssistData           = NULL;   /**< GNSS assistance data */
  sm_GnssAssistDataStruct const               *p_GloAssistData            = NULL;
  GNSS_ReferenceTimeReq                       *pGnss_RefTimeReq           = NULL;

  MSG_MED("TM_LPP: Encode Request AD", 0, 0, 0);

  if ( p_lpp_tx_type == NULL )
  {
    MSG_ERROR("TM_LPP: Null transcation pointer", 0, 0, 0);
    return ( FALSE );
  }

  p_lpp_pdu = tm_lpp_asn1_osys_IE_malloc(sizeof(LPP_Message));

  if ( p_lpp_pdu == NULL )
  {
    MSG_ERROR("TM_LPP: Buffer allocation failed", 0, 0, 0);
    return ( FALSE );
  }
  p_lpp_tx_type->z_lpp_data_type.p_pdu_ptr = p_lpp_pdu;
  p_lpp_pdu->m.transactionIDPresent = 1;
  z_trans_id = p_lpp_tx_type->z_current_trans_id;
  p_lpp_pdu->transactionID.initiator = (uint8)z_trans_id.u_initiator;
  p_lpp_pdu->transactionID.transactionNumber = z_trans_id.u_transaction_id;
  p_lpp_pdu->endTransaction = FALSE;

  /* LPP CP Specific Changes : BEGIN */
  if ( p_lpp_tx_type->u_lpp_session_type == TM_LPP_SESSION_TYPE_CP )
  {
    p_lpp_pdu->m.sequenceNumberPresent = 1;
    tm_lpp_cp_get_nxt_out_seq_no(p_lpp_tx_type);
    p_lpp_pdu->sequenceNumber = p_lpp_tx_type->u_out_sequence_num;

    p_lpp_pdu->m.acknowledgementPresent = 1;
    p_lpp_pdu->acknowledgement.ackRequested = 1;           /* Request Server ACK to LPP Prov Cap           */
    p_lpp_pdu->acknowledgement.m.ackIndicatorPresent = 0;  /* Make sure clear unused field                 */
  }
  /* LPP CP Specific Changes : END */

  p_lpp_pdu->m.lpp_MessageBodyPresent = 1;

  p_lpp_pdu->lpp_MessageBody.t = LPP_MESSAGEBODY_C1;
  if ( (p_lpp_pdu->lpp_MessageBody.u.c1 = tm_lpp_asn1_osys_IE_malloc(sizeof(LPP_MessageBody_c1))) == NULL )
  {
    MSG_ERROR("TM_LPP: Buffer allocation failed", 0, 0, 0);
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_REQ_AD);
    return ( FALSE );
  }

  p_lpp_pdu->lpp_MessageBody.u.c1->t = LPP_MESSAGE_REQ_AD;
  p_lpp_pdu->lpp_MessageBody.u.c1->u.requestAssistanceData = tm_lpp_asn1_osys_IE_malloc(sizeof(RequestAssistanceData));

  if ( p_lpp_pdu->lpp_MessageBody.u.c1->u.requestAssistanceData == NULL )
  {
    MSG_ERROR("TM_LPP: Buffer allocation failed", 0, 0, 0);
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_REQ_AD);
    return ( FALSE );
  }
  p_lpp_pdu->lpp_MessageBody.u.c1->u.requestAssistanceData->criticalExtensions.t = LPP_REQ_AD_CRITICAL_EXT;

  p_lpp_pdu->lpp_MessageBody.u.c1->u.requestAssistanceData->criticalExtensions.u.c1 = tm_lpp_asn1_osys_IE_malloc(sizeof(RequestAssistanceData_criticalExtensions_c1));
  p_req_ad_c1 = p_lpp_pdu->lpp_MessageBody.u.c1->u.requestAssistanceData->criticalExtensions.u.c1;
  if ( p_req_ad_c1 == NULL )
  {
    MSG_ERROR("TM_LPP: Buffer allocation failed", 0, 0, 0);
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_REQ_AD);
    return ( FALSE );
  }
  p_req_ad_c1->t = LPP_REQ_AD_R9;
  p_req_ad_c1->u.requestAssistanceData_r9 = tm_lpp_asn1_osys_IE_malloc(sizeof(RequestAssistanceData_r9_IEs));

  p_req_ad_r9 =   p_req_ad_c1->u.requestAssistanceData_r9;

  if ( p_req_ad_r9 == NULL )
  {
    MSG_ERROR("TM_LPP: Buffer allocation failed", 0, 0, 0);
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_REQ_AD);
    return ( FALSE );
  }
  p_req_ad_r9->m.a_gnss_RequestAssistanceDataPresent = 1;

  // to do encode the LTEcellid
  if ( p_lpp_tx_type->z_lpp_data_type.z_cached_cell_info.u_CachedCellIDValid == TRUE )
  {
    p_req_ad_r9->m.commonIEsRequestAssistanceDataPresent = 1;

    memset(&p_req_ad_r9->commonIEsRequestAssistanceData, 0,
           sizeof(CommonIEsRequestAssistanceData));
    p_req_ad_r9->commonIEsRequestAssistanceData.m.primaryCellIDPresent = 1;
#ifdef FEATURE_CGPS_LTE_CELLDB
    // MNC
    p_req_ad_r9->commonIEsRequestAssistanceData.primaryCellID.mnc.n =
      p_lpp_tx_type->z_lpp_data_type.z_cached_cell_info.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.num_mnc_digits;
    p_req_ad_r9->commonIEsRequestAssistanceData.primaryCellID.mnc.elem = 
      tm_lpp_asn1_osys_IE_malloc( sizeof(uint8) * p_req_ad_r9->commonIEsRequestAssistanceData.primaryCellID.mnc.n );

    if ( NULL == p_req_ad_r9->commonIEsRequestAssistanceData.primaryCellID.mnc.elem )
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "TM_LPP: MNC buffer allocation failed" );
      tm_asn1_free_lpp_message_IEs( p_lpp_pdu, LPP_MESSAGE_REQ_AD );
      return ( FALSE );
    }

    for ( u_i = 0; 
          u_i < p_req_ad_r9->commonIEsRequestAssistanceData.primaryCellID.mnc.n;
          u_i++ )
    {
      p_req_ad_r9->commonIEsRequestAssistanceData.primaryCellID.mnc.elem[u_i] =
        (uint8)p_lpp_tx_type->z_lpp_data_type.z_cached_cell_info.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.mnc[u_i];
    }

    // MCC
    p_req_ad_r9->commonIEsRequestAssistanceData.primaryCellID.mcc.n = 3;
    p_req_ad_r9->commonIEsRequestAssistanceData.primaryCellID.mcc.elem = 
      tm_lpp_asn1_osys_IE_malloc( sizeof(uint8) * p_req_ad_r9->commonIEsRequestAssistanceData.primaryCellID.mcc.n );

    if ( NULL == p_req_ad_r9->commonIEsRequestAssistanceData.primaryCellID.mcc.elem )
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "TM_LPP: MCC buffer allocation failed" );
      tm_asn1_free_lpp_message_IEs( p_lpp_pdu, LPP_MESSAGE_REQ_AD );
      return ( FALSE );
    }

    for ( u_i = 0; 
          u_i < p_req_ad_r9->commonIEsRequestAssistanceData.primaryCellID.mcc.n;
          u_i++ )
    {
      p_req_ad_r9->commonIEsRequestAssistanceData.primaryCellID.mcc.elem[u_i] =
        (uint8)p_lpp_tx_type->z_lpp_data_type.z_cached_cell_info.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.mcc[u_i];
    }
    p_req_ad_r9->commonIEsRequestAssistanceData.primaryCellID.cellidentity.numbits = 28;
    b_ret = tm_lpp_osys_asn1_int_to_bitstring(
      p_lpp_tx_type->z_lpp_data_type.z_cached_cell_info.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.cell_identity,
      &p_req_ad_r9->commonIEsRequestAssistanceData.primaryCellID.cellidentity);

    if ( b_ret == FALSE )
    {
      MSG_ERROR("TM_LPP: failed to convered cellId", 0, 0, 0);
    }
#else

    p_req_ad_r9->commonIEsRequestAssistanceData.primaryCellID.cellidentity.numbits = 0;
#endif
    p_req_ad_r9->commonIEsRequestAssistanceData.extElem1.count = 0;

  }
  p_GnssAssistData = p_lpp_tx_type->z_lpp_data_type.p_AssistData;
  p_GloAssistData = &p_GnssAssistData->z_GloAssistData; // handle to glonass part

  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "TM_LPP: Enc ReqAD as per wishlist 0x%lX (GPS), 0x%lX (GLONASS)",
        p_GnssAssistData->q_WishListMask, p_GloAssistData->q_WishListMask);

  /** Allocate storage for GNSS (GPS + GLO) ref. time & generic AD params */
  p_req_ad_r9->a_gnss_RequestAssistanceData.gnss_GenericAssistDataReq.n = 0;
  pGnss_RefTimeReq = &p_req_ad_r9->a_gnss_RequestAssistanceData.gnss_CommonAssistDataReq.gnss_ReferenceTimeReq;
  pGnss_RefTimeReq->gnss_TimeReqPrefList.n = 0;

  if ( p_GnssAssistData->q_WishListMask ) // GPS
  {
    /* Check any generic AD is requested */
    if ( p_GnssAssistData->q_WishListMask & C_GPS_ASSIS_NAVMODL ||
         p_GnssAssistData->q_WishListMask & C_GPS_ASSIS_ALM ||
         p_GnssAssistData->q_WishListMask & C_GPS_ASSIS_RTI ||
         p_GnssAssistData->q_WishListMask & C_GPS_ASSIS_ACQ ||
         p_GnssAssistData->q_WishListMask & C_GPS_ASSIS_UTC )
    {
      p_req_ad_r9->a_gnss_RequestAssistanceData.gnss_GenericAssistDataReq.n++;
      b_GpsReqGenericAd = TRUE;
    }

    if ( p_GnssAssistData->q_WishListMask & C_GPS_ASSIS_REFTIME )
    {
      pGnss_RefTimeReq->gnss_TimeReqPrefList.n++;
    }
  }

  if ( (p_lpp_tx_type->z_lpp_data_type.p_lpp_prov_cap_type->u_glonass_supported == TRUE) &&
       (p_GloAssistData->q_WishListMask) ) // Glonass
  {
    /* Check any generic AD is requested */
    if ( p_GloAssistData->q_WishListMask & C_GNSS_ASSIST_NAVMODL ||
         p_GloAssistData->q_WishListMask & C_GNSS_ASSIST_ALM ||
         p_GloAssistData->q_WishListMask & C_GNSS_ASSIST_RTI ||
         p_GloAssistData->q_WishListMask & C_GNSS_ASSIST_ACQ ||
         p_GloAssistData->q_WishListMask & C_GNSS_ASSIST_UTC ||
         p_GloAssistData->q_WishListMask & C_GNSS_ASSIST_AUXI_INFO )
    {
      p_req_ad_r9->a_gnss_RequestAssistanceData.gnss_GenericAssistDataReq.n++;
    }
    else
    {
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Req GPS only AD from LS. Glonass not enabled (%d).",
            p_lpp_tx_type->z_lpp_data_type.p_lpp_prov_cap_type->u_glonass_supported);
    }

    if ( p_GloAssistData->q_WishListMask & C_GNSS_ASSIST_REFTIME )
    {
      pGnss_RefTimeReq->gnss_TimeReqPrefList.n++;
    }
  }

  /* Alloc for gnss generic param */
  p_req_ad_r9->a_gnss_RequestAssistanceData.gnss_GenericAssistDataReq.elem =
                                                                             tm_lpp_asn1_osys_IE_malloc(sizeof(GNSS_GenericAssistDataReqElement) *
                                                                                                        p_req_ad_r9->a_gnss_RequestAssistanceData.gnss_GenericAssistDataReq.n);

  /* Alloc for gnss reference time param */
  pGnss_RefTimeReq->gnss_TimeReqPrefList.elem = tm_lpp_asn1_osys_IE_malloc(sizeof(GNSS_ID) *
                                                                           (pGnss_RefTimeReq->gnss_TimeReqPrefList.n));

  if ( ((p_req_ad_r9->a_gnss_RequestAssistanceData.gnss_GenericAssistDataReq.n > 0) &&
        (p_req_ad_r9->a_gnss_RequestAssistanceData.gnss_GenericAssistDataReq.elem == NULL)) ||
       ((pGnss_RefTimeReq->gnss_TimeReqPrefList.n > 0) &&
        (pGnss_RefTimeReq->gnss_TimeReqPrefList.elem  == NULL)) )
  {

    MSG_ERROR("TM_LPP: Buffer allocation failed", 0, 0, 0);
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_REQ_AD);
    return ( FALSE );
  }


  /*** GPS Wishlist processing ***/
  if ( p_GnssAssistData->q_WishListMask )
  {
    /// GNSS_CommonAssistDataReq
    p_req_ad_r9->a_gnss_RequestAssistanceData.m.gnss_CommonAssistDataReqPresent = 1;

    // 1. GPS ReferenceTime
    if ( p_GnssAssistData->q_WishListMask & C_GPS_ASSIS_REFTIME )
    {
      p_req_ad_r9->a_gnss_RequestAssistanceData.gnss_CommonAssistDataReq.m.gnss_ReferenceTimeReqPresent =  1;
      p_req_ad_r9->a_gnss_RequestAssistanceData.gnss_CommonAssistDataReq.extElem1.count = 0;

      pGnss_RefTimeReq->gnss_TimeReqPrefList.elem[u_GnssTimeRPLElemIndex].gnss_id = (GNSS_ID_gnss_id)LPP_GNSS_ID_GPS;
      pGnss_RefTimeReq->m.gps_TOW_assistReqPresent = 1;
      pGnss_RefTimeReq->gps_TOW_assistReq = TRUE;
      pGnss_RefTimeReq->extElem1.count = 0;
      u_GnssTimeRPLElemIndex++;
    }

    // 2. gnss-ReferenceLocationReq
    if ( p_GnssAssistData->q_WishListMask & C_GPS_ASSIS_REFLOC )
    {
      p_req_ad_r9->a_gnss_RequestAssistanceData.gnss_CommonAssistDataReq.m.gnss_ReferenceLocationReqPresent =  1;
    }

    // 3. gnss-IonosphericModelReq
    if ( p_GnssAssistData->q_WishListMask & C_GPS_ASSIS_IONO )
    {
      p_req_ad_r9->a_gnss_RequestAssistanceData.gnss_CommonAssistDataReq.m.gnss_IonosphericModelReqPresent = 1;
      p_req_ad_r9->a_gnss_RequestAssistanceData.gnss_CommonAssistDataReq.gnss_IonosphericModelReq.m.klobucharModelReqPresent = 1;
      p_req_ad_r9->a_gnss_RequestAssistanceData.gnss_CommonAssistDataReq.gnss_IonosphericModelReq.extElem1.count =  0;
      p_req_ad_r9->a_gnss_RequestAssistanceData.gnss_CommonAssistDataReq.gnss_IonosphericModelReq.klobucharModelReq.numbits =  2;

      b_ret = tm_lpp_osys_asn1_int_to_bitstring(
                                                0,
                                                &p_req_ad_r9->a_gnss_RequestAssistanceData.gnss_CommonAssistDataReq.gnss_IonosphericModelReq.klobucharModelReq);
    }

    /// gnss_GenericAssistDataReq

    // Generic AD Req (GPS only here)
    if ( (TRUE == b_GpsReqGenericAd) &&
         (u_GnssGenericADReqElemIndex < p_req_ad_r9->a_gnss_RequestAssistanceData.gnss_GenericAssistDataReq.n) )
    {
      p_req_ad_r9->a_gnss_RequestAssistanceData.m.gnss_GenericAssistDataReqPresent = 1;

      p_generic_ad_req = &p_req_ad_r9->a_gnss_RequestAssistanceData.gnss_GenericAssistDataReq.elem[u_GnssGenericADReqElemIndex];
      u_GnssGenericADReqElemIndex++; // inc so that next system (Glonass) can use remaining elem space
                                     // GPS
      p_generic_ad_req->gnss_ID.gnss_id = (uint8)LPP_GNSS_ID_GPS;

      // 5. GPS NavigationModel (Ephermesis)
      if ( p_GnssAssistData->q_WishListMask & C_GPS_ASSIS_NAVMODL )
      {
        p_generic_ad_req->m.gnss_NavigationModelReqPresent = 1;

        p_generic_ad_req->gnss_NavigationModelReq.t =  LPP_REQ_AD_NAVMODEL_STORED_NAV_LIST;
        p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList = tm_lpp_asn1_osys_IE_malloc(sizeof(StoredNavListInfo));
        if ( p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList == NULL )
        {
          MSG_ERROR("TM_LPP: Buffer allocation failed", 0, 0, 0);
          tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_REQ_AD);
          return ( FALSE );
        }
        p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->m.satListRelatedDataListPresent = 0;
        // Use available eph. data (from PE(CD)) to send to server ?
        if ( (p_GnssAssistData->u_EphInfoIncl == TRUE) && (p_GnssAssistData->w_GpsWeek != C_GPS_WEEK_UNKNOWN) )
        {
          p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->gnss_WeekOrDay = p_GnssAssistData->w_GpsWeek;
          p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->gnss_Toe = (uint8)p_GnssAssistData->w_Toe;
          p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->t_toeLimit = (uint8)p_GnssAssistData->w_ToeLimit;

          // Available GPS SvId (1..32)
          p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->satListRelatedDataList.n =
                                                                                                (p_GnssAssistData->u_NumSvs <= N_GPS_SV) ? p_GnssAssistData->u_NumSvs : N_GPS_SV;

          if ( p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->satListRelatedDataList.n > 0 )
          {
            p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->m.satListRelatedDataListPresent = 1;
            p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->satListRelatedDataList.elem =
                                                                                                     tm_lpp_asn1_osys_IE_malloc(sizeof(SatListRelatedDataElement) *
                                                                                                                                p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->satListRelatedDataList.n);

            // Sat clock & orbit model details (optional IE)
            if ( p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->satListRelatedDataList.elem != NULL )
            {
              for ( u_i = 0; u_i < p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->satListRelatedDataList.n; ++u_i )
              {
                if ( p_GnssAssistData->z_SvEphIode[u_i].w_Sv > 0 )
                {
                  // satellite_id (0..62). z_SvEphIode[u_i].u_Sv (1..32)
                  p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->satListRelatedDataList.elem[u_i].svID.satellite_id =
                                                                                                                                  p_GnssAssistData->z_SvEphIode[u_i].w_Sv - 1;

                  p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->satListRelatedDataList.elem[u_i].iod.numbits = 11;
                  b_ret = tm_lpp_osys_asn1_int_to_bitstring((uint32)p_GnssAssistData->z_SvEphIode[u_i].w_Iode,
                                                            &p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->satListRelatedDataList.elem[u_i].iod);
                  if ( b_ret == FALSE )
                  {
                    MSG_ERROR("TM_LPP: could not convert the int to bitstring", 0, 0, 0);
                  }
                }
              }
            }
            else
            {
              MSG_ERROR("TM_LPP: LPP ReqAD buffer allocation failed", 0, 0, 0);
              tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_REQ_AD);
              return ( FALSE );
            }
          }
          else
          {
            p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->m.satListRelatedDataListPresent = 0;
          }
        }
      }

      // 5. GPS RealTimeIntegrity
      if ( p_GnssAssistData->q_WishListMask & C_GPS_ASSIS_RTI )
      {
        p_generic_ad_req->m.gnss_RealTimeIntegrityReqPresent = 1;
      }

      // 6. GPS AcquisitionAssistance
      if ( p_GnssAssistData->q_WishListMask & C_GPS_ASSIS_ACQ )
      {
        p_generic_ad_req->m.gnss_AcquisitionAssistanceReqPresent = 1;
        p_generic_ad_req->gnss_AcquisitionAssistanceReq.gnss_SignalID_Req.gnss_SignalID = LPP_GNSS_SIGNAL_ID_GPS_L1CA;
        p_generic_ad_req->gnss_AcquisitionAssistanceReq.gnss_SignalID_Req.extElem1.count = 0;
      }

      // 7. GPS Almanac
      if ( p_GnssAssistData->q_WishListMask & C_GPS_ASSIS_ALM )
      {
        p_generic_ad_req->m.gnss_AlmanacReqPresent = 1;
      }

      // 8. GPS UTCModel
      if ( (p_GnssAssistData->q_WishListMask & C_GPS_ASSIS_UTC) )
      {
        p_generic_ad_req->m.gnss_UTCModelReqPresent = 1;
      }
    }
  }


  /*** GLONASS wishlist processing ***/
  if ( p_GloAssistData->q_WishListMask )
  {
    /// GNSS_CommonAssistDataReq
    // gnss_RefTimeReq (Glonass)
    if ( (u_GnssTimeRPLElemIndex < pGnss_RefTimeReq->gnss_TimeReqPrefList.n) &&
         (p_GloAssistData->q_WishListMask & C_GNSS_ASSIST_REFTIME) )
    {
      p_req_ad_r9->a_gnss_RequestAssistanceData.m.gnss_CommonAssistDataReqPresent = 1;

      pGnss_RefTimeReq->gnss_TimeReqPrefList.elem[u_GnssTimeRPLElemIndex].gnss_id = (GNSS_ID_gnss_id)LPP_GNSS_ID_GLONASS;
      pGnss_RefTimeReq->gnss_TimeReqPrefList.elem[u_GnssTimeRPLElemIndex].extElem1.count = 0;
      pGnss_RefTimeReq->m.notOfLeapSecReqPresent = 1;
      pGnss_RefTimeReq->notOfLeapSecReq = 1;
      u_GnssTimeRPLElemIndex++;
    }

    // PE doesn't require GLONASS Iono

    /// gnss_GenericAssistDataReq (Glonass)
    if ( u_GnssGenericADReqElemIndex < p_req_ad_r9->a_gnss_RequestAssistanceData.gnss_GenericAssistDataReq.n )
    {
      p_req_ad_r9->a_gnss_RequestAssistanceData.m.gnss_GenericAssistDataReqPresent = 1;
      p_generic_ad_req = &p_req_ad_r9->a_gnss_RequestAssistanceData.gnss_GenericAssistDataReq.elem[u_GnssGenericADReqElemIndex];
      u_GnssGenericADReqElemIndex++; // inc so that next system can use remaining elem space
      p_generic_ad_req->gnss_ID.gnss_id = (uint8)LPP_GNSS_ID_GLONASS;

      // gnss_TimeModelsReq (Glonass)
      if ( p_GloAssistData->q_WishListMask & C_GNSS_ASSIST_TIME_MODELS )
      {
        p_generic_ad_req->m.gnss_TimeModelsReqPresent = 1;
        p_generic_ad_req->gnss_TimeModelsReq.n = 1;

        p_generic_ad_req->gnss_TimeModelsReq.elem = tm_lpp_asn1_osys_IE_malloc(sizeof(GNSS_TimeModelElementReq));
        if ( p_generic_ad_req->gnss_TimeModelsReq.elem == NULL )
        {
          MSG_ERROR("TM_LPP: Glonass time models buffer allocation failed", 0, 0, 0);
          tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_REQ_AD);
          return ( FALSE );
        }

        p_generic_ad_req->gnss_TimeModelsReq.elem->gnss_TO_IDsReq = LPP_GNSS_TO_ID_GLONASS;
        p_generic_ad_req->gnss_TimeModelsReq.elem->deltaTreq = 1;
      }

      // gnss_NavigationModelReq (Glonass)
      if ( p_GloAssistData->q_WishListMask & C_GNSS_ASSIST_NAVMODL )
      {
        p_generic_ad_req->m.gnss_NavigationModelReqPresent = 1;
        p_generic_ad_req->gnss_NavigationModelReq.t =  LPP_REQ_AD_NAVMODEL_STORED_NAV_LIST;
        p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList = tm_lpp_asn1_osys_IE_malloc(sizeof(StoredNavListInfo));
        if ( p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList == NULL )
        {
          MSG_ERROR("TM_LPP: Glonass nav buffer allocation failed", 0, 0, 0);
          tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_REQ_AD);
          return ( FALSE );
        }
        p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->m.satListRelatedDataListPresent = 0;
        // Use available glonass eph. data (from PE(CD)) to send to server ?
        if ( p_GnssAssistData->z_GloAssistData.u_EphInfoIncl == TRUE )
        {
          MSG_LOW("TM_LPP: Load the available Glonass eph. Num SVs %d", p_GnssAssistData->z_GloAssistData.u_NumSvs, 0, 0);

          p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->gnss_WeekOrDay = p_GnssAssistData->z_GloAssistData.z_AssistDataTime.z_GloEphTime.w_Na;
          p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->gnss_Toe = p_GnssAssistData->z_GloAssistData.z_AssistDataTime.z_GloEphTime.w_Tb;
          p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->t_toeLimit = p_GnssAssistData->z_GloAssistData.z_AssistDataTime.z_GloEphTime.w_ToeLimit;

          // Available Glonass SvId (1..24)
          p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->satListRelatedDataList.n = (p_GnssAssistData->z_GloAssistData.u_NumSvs <= N_GLO_SV) ? p_GnssAssistData->z_GloAssistData.u_NumSvs : N_GLO_SV;

          if ( p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->satListRelatedDataList.n > 0 )
          {
            p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->m.satListRelatedDataListPresent = 1;
            p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->satListRelatedDataList.elem =
                                                                                                     tm_lpp_asn1_osys_IE_malloc(sizeof(SatListRelatedDataElement) *
                                                                                                                                p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->satListRelatedDataList.n);

            // Glonass cat clock & orbit model details (optional IE)
            if ( p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->satListRelatedDataList.elem != NULL )
            {
              for ( u_i = 0; u_i < p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->satListRelatedDataList.n; u_i++ )
              {
                if ( p_GnssAssistData->z_GloAssistData.z_SvEphIode[u_i].w_Sv > GLO_SLOTID_OFFSET )
                {
                  // satellite_id (0..23 & 24..63 are reserved). z_SvEphIode[u_i].w_Sv (65..88)
                  p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->satListRelatedDataList.elem[u_i].svID.satellite_id =
                                                                                                                                  p_GnssAssistData->z_GloAssistData.z_SvEphIode[u_i].w_Sv - (GLO_SLOTID_OFFSET + 1);

                  p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->satListRelatedDataList.elem[u_i].iod.numbits = 11;
                  b_ret = tm_lpp_osys_asn1_int_to_bitstring((uint32)p_GnssAssistData->z_GloAssistData.z_SvEphIode[u_i].w_Iode,
                                                            &p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->satListRelatedDataList.elem[u_i].iod);
                  if ( b_ret == FALSE )
                  {
                    MSG_ERROR("TM_LPP: Glonass : could not convert the int to bitstring", 0, 0, 0);
                  }
                }
              }
            }
            else
            {
              MSG_ERROR("TM_LPP: LPP ReqAD buffer allocation failed", 0, 0, 0);
              tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_REQ_AD);
              return ( FALSE );
            }
          }
          else
          {
            p_generic_ad_req->gnss_NavigationModelReq.u.storedNavList->m.satListRelatedDataListPresent = 0; // no AD held by UE (PE (CD))
          }
        }
      }

      // gnss_RealTimeIntegrityReq (Glonass)
      if ( p_GloAssistData->q_WishListMask & C_GNSS_ASSIST_RTI )
      {
        p_generic_ad_req->m.gnss_RealTimeIntegrityReqPresent = 1;
        p_generic_ad_req->gnss_RealTimeIntegrityReq.extElem1.count = 0;
      }

      // gnss_AcquisitionAssistanceReq (Glonass)
      if ( p_GloAssistData->q_WishListMask & C_GNSS_ASSIST_ACQ )
      {
        p_generic_ad_req->m.gnss_AcquisitionAssistanceReqPresent = 1;
        p_generic_ad_req->gnss_AcquisitionAssistanceReq.gnss_SignalID_Req.gnss_SignalID = LPP_GNSS_SIGNAL_ID_GLONASS_G1;
        p_generic_ad_req->gnss_AcquisitionAssistanceReq.gnss_SignalID_Req.extElem1.count = 0;
      }

      // gnss_AlmanacReq (Glonass)
      if ( p_GloAssistData->q_WishListMask & C_GNSS_ASSIST_ALM )
      {
        p_generic_ad_req->m.gnss_AlmanacReqPresent = 1;
        p_generic_ad_req->gnss_AlmanacReq.extElem1.count = 0;
        p_generic_ad_req->gnss_AlmanacReq.m.modelIDPresent = 0; // modelID not supported by target
      }

      // gnss_UTCModelReq (Glonass)
      if ( p_GloAssistData->q_WishListMask & C_GNSS_ASSIST_UTC )
      {
        p_generic_ad_req->m.gnss_UTCModelReqPresent = 1;
        p_generic_ad_req->gnss_UTCModelReq.m.modelIDPresent = 0; // modelID not supported by target
        p_generic_ad_req->gnss_UTCModelReq.extElem1.count = 0;
      }

      // gnss_AuxiliaryInformationReq (Glonass)
      if ( p_GloAssistData->q_WishListMask & C_GNSS_ASSIST_AUXI_INFO )
      {
        p_generic_ad_req->m.gnss_AuxiliaryInformationReqPresent = 1;
        p_generic_ad_req->gnss_AuxiliaryInformationReq.extElem1.count = 0;
      }

      // gnss_TimeModel - TimeModel is used for AA (MSA). ME will come-up with usecase for MSB also.
      if ( p_GloAssistData->q_WishListMask & C_GNSS_ASSIST_TIME_MODELS )
      {
        p_generic_ad_req->m.gnss_TimeModelsReqPresent = 1;
        p_generic_ad_req->gnss_TimeModelsReq.n = 1;
        p_generic_ad_req->gnss_TimeModelsReq.elem = tm_lpp_asn1_osys_IE_malloc(sizeof(GNSS_TimeModelElementReq));
        if ( p_generic_ad_req->gnss_TimeModelsReq.elem != NULL )
        {
          // Request GPS time offset
          p_generic_ad_req->gnss_TimeModelsReq.elem->gnss_TO_IDsReq = LPP_GNSS_TO_ID_GPS;
          p_generic_ad_req->gnss_TimeModelsReq.elem->deltaTreq = TRUE; // Request deltaT to be mandatory in gnss-TimeModel Provide AD
        }
        else
        {
          MSG_ERROR("TM_LPP: LPP ReqAD buffer allocation failed", 0, 0, 0);
          tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_REQ_AD);
          return ( FALSE );
        }
      }
    }
    else
    {
      MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "LPP can't req. generic glo AD(%lu). Ndx %d",
            p_req_ad_r9->a_gnss_RequestAssistanceData.gnss_GenericAssistDataReq.n, u_GnssGenericADReqElemIndex);
    }
  }

  /*** OTDOA processing ***/
  if ( p_lpp_tx_type->z_lpp_data_type.b_otdoa_needed == TRUE )
  {

#ifdef FEATURE_CGPS_LTE_CELLDB
    p_req_ad_r9->m.otdoa_RequestAssistanceDataPresent = 1;

    p_req_ad_r9->otdoa_RequestAssistanceData.physCellId = p_lpp_tx_type->z_lpp_data_type.z_cached_cell_info.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.cell_id;
#endif
  }
  /* Do the ASN.1 encoding for LPP */
  memset(&p_lpp_tx_type->z_lpp_data_type.z_pdu_buf, 0, sizeof(p_lpp_tx_type->z_lpp_data_type.z_pdu_buf)); /* crash 3 */
  encode_status = tm_lpp_osys_asn1_encode_pdu((void *)p_lpp_pdu, &p_lpp_tx_type->z_lpp_data_type.z_pdu_buf);

  if ( (encode_status != 0) || (p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value == NULL) || (p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.length == 0) )
  {
    /* ASN.1 encoding failed */
    MSG_ERROR("TM_LPP: Error encoding LPP Request AD", 0, 0, 0);

    if ( p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value != NULL )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "LPP ASN1 enc. err. Freeing: %d,%lx,%u",
            encode_status,
            p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value,
            p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.length);

      tm_lpp_asn1_free_osys_buf(p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value);
    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "LPP ASN1 enc. err. No alloc.: %d,%lx,%u",
            encode_status,
            p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value,
            p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.length);
    }
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_REQ_AD);
    return ( FALSE );
  }

  tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_REQ_AD);
  return ( TRUE );
}


/*
 ******************************************************************************
 * Function description:
 *  Helper function to check GPS measurement has atleast one valid satellite.
 *
 * Parameters:
 *    pGnssMeasBlk : GNSS measurement block containing satellite information.
 *    
 * Return value:
 *    Return TRUE if atleast one valid satellite found, else false.
 *
 ******************************************************************************
*/
boolean tm_lpp_gps_sv_least_available( const gnss_MeasBlkStructType *const pGnssMeasBlk )
{
  uint8 uI = 0;

  if ( pGnssMeasBlk->u_NumSvs > 0 )
  {
    if ( GNSS_MEAS_BLK_SRC_GPS == pGnssMeasBlk->e_MeasBlkSrc )
    {
      /* Check GPS Meas clock is valid */
      for ( uI = 0; uI < pGnssMeasBlk->u_NumSvs; ++uI )
      {
        if ( pGnssMeasBlk->z_Gnss[uI].w_Cno > 0 )
        {
          return ( TRUE );
        }
      }
    }
  }

  return ( FALSE );
}

/*
 ******************************************************************************
 * Function description:
 *  Helper function to check GLONASS measurement has atleast one valid satellite.
 *
 * Parameters:
 *    pGnssMeasBlk : GNSS measurement block containing satellite information.
 *    
 * Return value:
 *    Return TRUE if atleast one valid satellite found, else false.
 *
 ******************************************************************************
*/
boolean tm_lpp_glo_sv_least_available( const gnss_MeasBlkStructType *const pGnssMeasBlk )
{
  uint8 uI = 0;

  if ( pGnssMeasBlk->u_NumSvs > 0 )
  {
    if ( GNSS_MEAS_BLK_SRC_GLO == pGnssMeasBlk->e_MeasBlkSrc )
    {
      /* Check GLO Meas clock is valid */
      for ( uI = 0; uI < pGnssMeasBlk->u_NumSvs; ++uI )
      {
        if ( pGnssMeasBlk->z_Gnss[uI].w_Cno > 0 )
        {
          return ( TRUE );
        }
      }
    }
  }

  return ( FALSE );
}

/*
 ******************************************************************************
 * Function description:
 *  Builds LPP GNSS measurement report. This function would build LPP ProvideLocationInfo  
 *  measurement report even if only one measurement report system (GPS or GLONASS) is available.
 *
 * Parameters:
 *    p_meas_reports : Source of meas_done GNSS measurement report from ME
 *    p_prov_loc_info_ie : Destination to store the translated measurement report in LPP ProvideLocationInfo message
 *    b_status_check_needed : Stringent measurement evaluation is required or not
 *    b_ls_gnss_method_bmap : LS(LocationServer) interest in GNSS measurement system (GPS and/or GLONASS)
 *
 * Return value:
 *    Return TRUE if LPP ProvideLocationInfo message is filled. FALSE to allow the calling function to
 *    fill the appropiate error code in the LPP ProvideLocationInfo message.
 *
 ******************************************************************************
*/

boolean tm_lpp_prov_loc_gnss_meas_build( const SmGnssMeasReportStructType *p_meas_reports,
                                         A_GNSS_ProvideLocationInformation *p_prov_loc_info_ie,
                                         boolean b_status_check_needed,
                                         uint8 b_ls_gnss_method_bmap )
{
  uint8                        uTotalGoodGpsSVs     = 0,
                               uTotalGoodGloSVs     = 0;
  float                        f_Num;
  double                       d_AdjSubMsec,
                               d_Num;
  int16                        x_Word;
  uint8                        uTotalGpsSvs         = 0,
                               uTotalGloSvs         = 0;
  uint8                        u_i                  = 0;
  gnss_MeasStructType          *p_gps_meas_per_sv   = NULL;
  gnss_MeasStructType          *p_glo_meas_per_sv   = NULL;
  const gnss_MeasBlkStructType *pGpsMeasRpt         = NULL;
  const gnss_MeasBlkStructType *pGloMeasRpt         = NULL;
  const gnss_ClockStructType   *pGnssMeasClock      = NULL;
  GNSS_MeasurementForOneGNSS   *p_element           = NULL;
  GNSS_SgnMeasElement          *p_gnss_elem         = NULL;
  GNSS_SatMeasElement          *p_sat_elem          = NULL;
  float                        f_GnssTodFracPart;
  uint16                       w_GnssTodFrac250ns;
  uint32                       q_GnssTodMsecModHour;
  boolean                      b_fill_gnss_error    = FALSE;

  if ( (NULL == p_meas_reports) || (NULL == p_prov_loc_info_ie) )
  {
    return ( FALSE ); // failure
  }

  MSG_MED("TM_LPP: Build LS msg from %u GNSS system meas report. LS choice 0x%02X.", p_meas_reports->u_meas_reports_bmap,
          b_ls_gnss_method_bmap, 0);

  /* Count how many different measurment system reports (GPS/GLONASS) are provided that are in LS interest */
  if ( (p_meas_reports->u_meas_reports_bmap & SM_GNSS_MEAS_REPORTS_BMAP_GPS) &&
       (GNSS_MEAS_BLK_SRC_GPS == p_meas_reports->z_meas_reports[SM_GNSS_MEAS_SYSTEM_GPS].z_MeasBlk.e_MeasBlkSrc) )
  {
    /* Use GPS measurement clock if valid */
    if ( (b_ls_gnss_method_bmap & C_POS_SOURCE_GPS) &&
         (tm_lpp_gps_sv_least_available(&p_meas_reports->z_meas_reports[SM_GNSS_MEAS_SYSTEM_GPS].z_MeasBlk)) )
    {
      pGnssMeasClock = &p_meas_reports->z_meas_reports[SM_GNSS_MEAS_SYSTEM_GPS].z_MeasBlk.z_MeasClock;
      pGpsMeasRpt = &p_meas_reports->z_meas_reports[SM_GNSS_MEAS_SYSTEM_GPS].z_MeasBlk;
      p_prov_loc_info_ie->gnss_SignalMeasurementInformation.gnss_MeasurementList.n++;
    }
    else if ( b_ls_gnss_method_bmap & C_POS_SOURCE_GPS )
    {
      /* Indicate to fill target device error cause, only if LS asked for GPS Meas */
      b_fill_gnss_error = TRUE;
    }
  }

  if ( b_ls_gnss_method_bmap & C_POS_SOURCE_GLO )
  {
    if ( (p_meas_reports->u_meas_reports_bmap & SM_GNSS_MEAS_REPORTS_BMAP_GLONASS) &&
         (GNSS_MEAS_BLK_SRC_GLO == p_meas_reports->z_meas_reports[SM_GNSS_MEAS_SYSTEM_GLONASS].z_MeasBlk.e_MeasBlkSrc) )
    {
      pGloMeasRpt = &p_meas_reports->z_meas_reports[SM_GNSS_MEAS_SYSTEM_GLONASS].z_MeasBlk;
      p_prov_loc_info_ie->gnss_SignalMeasurementInformation.gnss_MeasurementList.n++;

      /* When ME send the measurement block the GNSS clock are already converted to be consistency between GPS and GLO.
         So check validity of GPS Clock in Glonass measurement only when GPS measurement is invalid. */
      if ( (pGnssMeasClock == NULL) &&
           (tm_lpp_glo_sv_least_available(&p_meas_reports->z_meas_reports[SM_GNSS_MEAS_SYSTEM_GLONASS].z_MeasBlk)) )
      {
        pGnssMeasClock = &p_meas_reports->z_meas_reports[SM_GNSS_MEAS_SYSTEM_GLONASS].z_MeasBlk.z_MeasClock;
      }
    }
    else
    {
      /* Indicate to fill target device error cause */
      b_fill_gnss_error = TRUE;
    }
  }

  /* Return if both measurement data are empty */
  if ( (NULL == pGpsMeasRpt) && (NULL == pGloMeasRpt) )
  {
    MSG_HIGH("TM_LPP: No qualified measurement to build.", 0, 0, 0);
    return ( FALSE ); // failure, fill error code in the calling func
  }


  /* GPS */
  if ( pGpsMeasRpt != NULL )
  {
    /* if this number is ever greater than 16, limit to the first 16 */
    if ( pGpsMeasRpt->u_NumSvs < N_ACTIVE_GPS_CHAN )
    {
      uTotalGpsSvs = pGpsMeasRpt->u_NumSvs;
    }
    else
    {
      uTotalGpsSvs = N_ACTIVE_GPS_CHAN;
    }

    /* count to see if there is any non-zero CNo in the list */
    uTotalGoodGpsSVs = 0;
    p_gps_meas_per_sv = (gnss_MeasStructType *)pGpsMeasRpt->z_Gnss;
    for ( u_i = 0; u_i < uTotalGpsSvs; u_i++ )
    {
      if ( p_gps_meas_per_sv->w_Cno > 0   &&
           (gps_meas_status_bits_check((uint16)p_gps_meas_per_sv->q_MeasStatus, b_status_check_needed)) )
      {
        uTotalGoodGpsSVs++;
      }
      p_gps_meas_per_sv++;
    }

    if ( uTotalGoodGpsSVs == 0 )
    {
      /* Indicate error in GPS measurement report if there are no good GPS SVs to report. 
      *  However proceed to process with GLO Svs.
      */
      b_fill_gnss_error = TRUE; /* Indicate to fill target device error cause */
      p_prov_loc_info_ie->gnss_SignalMeasurementInformation.gnss_MeasurementList.n--; // Exclude GPS from mem allocation
    }
  }

  /* Glonass */
  if ( pGloMeasRpt != NULL )
  {
    /* if this number is ever greater than 14, limit to the first 14 */
    if ( pGloMeasRpt->u_NumSvs < N_ACTIVE_GLO_CHAN )
    {
      uTotalGloSvs = pGloMeasRpt->u_NumSvs;
    }
    else
    {
      uTotalGloSvs = N_ACTIVE_GLO_CHAN;
    }

    /* count to see if there is any non-zero CNo in the list */
    uTotalGoodGloSVs = 0;
    p_glo_meas_per_sv = (gnss_MeasStructType *)pGloMeasRpt->z_Gnss;
    for ( u_i = 0; (u_i < uTotalGloSvs); ++u_i, ++p_glo_meas_per_sv )
    {
      if ( (p_glo_meas_per_sv->w_Cno > 0) &&
           (gnss_meas_status_bits_check(p_glo_meas_per_sv->q_MeasStatus, b_status_check_needed)) )
      {
        uTotalGoodGloSVs++;
      }
    }

    /* if all Glonass SVs having CN0 of 0, proceed only with GPS measurements */
    if ( uTotalGoodGloSVs == 0 )
    {
      b_fill_gnss_error = TRUE; /* Indicate to fill target device error cause */
      p_prov_loc_info_ie->gnss_SignalMeasurementInformation.gnss_MeasurementList.n--; // Exclude Glonass from mem allocation
    }

  } /* Glonass Meas Report end */

  MSG_4(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Total good meas SVs: %u (GPS), %u (GLO). Total SVs  %u (GPS), %u (GLO).",
        uTotalGoodGpsSVs, uTotalGoodGloSVs, uTotalGpsSvs, uTotalGloSvs);

  /* Fill target device error cause & continue to populate for any valid measurement system */
  if ( TRUE == b_fill_gnss_error )
  {
    MSG_HIGH("TM_LPP: Can't provide all LS requested measurements. Fill GNSS-Error.", 0, 0, 0);

    p_prov_loc_info_ie->gnss_Error.u.targetDeviceErrorCauses = tm_lpp_asn1_osys_IE_malloc(sizeof(GNSS_TargetDeviceErrorCauses));
    if ( p_prov_loc_info_ie->gnss_Error.u.targetDeviceErrorCauses == NULL )
    {
      MSG_ERROR("TM_LPP: ProvLocInfo GNSS-Error build buffer alloc failure", 0, 0, 0);
      return ( FALSE ); /* failure, fill error code in the calling func */
    }

    p_prov_loc_info_ie->m.gnss_ErrorPresent = 1;
    p_prov_loc_info_ie->gnss_Error.t = T_A_GNSS_Error_targetDeviceErrorCauses;
    p_prov_loc_info_ie->gnss_Error.u.targetDeviceErrorCauses->cause = GNSS_TargetDeviceErrorCauses_cause_notAllRequestedMeasurementsPossible;

    /* Return if GPS + GLO SVs aren't good */
    if ( (0 == uTotalGoodGpsSVs) && (0 == uTotalGoodGloSVs) )
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "No good measurements to report");
      p_prov_loc_info_ie->gnss_Error.u.targetDeviceErrorCauses->cause = GNSS_TargetDeviceErrorCauses_cause_thereWereNotEnoughSatellitesReceived;
      return ( TRUE );
    }
  }

  /* Populate Measurement reference time from ME measurement GPS clock info */
  if ( pGnssMeasClock != NULL )
  {
    p_prov_loc_info_ie->m.gnss_SignalMeasurementInformationPresent = 1;
    p_prov_loc_info_ie->gnss_SignalMeasurementInformation.measurementReferenceTime.gnss_TimeID.gnss_id = (uint8)LPP_GNSS_ID_GPS;

    /* convert to the right format for LPP */

    f_GnssTodFracPart = -(pGnssMeasClock->z_Time.z_GpsTime.f_ClkTimeBias);
    q_GnssTodMsecModHour = pGnssMeasClock->z_Time.z_GpsTime.q_GpsMsec;
    if ( f_GnssTodFracPart < 0.0f )
    {
      w_GnssTodFrac250ns = (uint16)((1.0f + f_GnssTodFracPart) * 4000);
      q_GnssTodMsecModHour -= 1;
      tm_umts_common_utils_GpsMsecWeekLimit((int32 *)&q_GnssTodMsecModHour, NULL);  /* Handles roll overs */
    }
    else
    {
      w_GnssTodFrac250ns = (uint16)(f_GnssTodFracPart * 4000);
    }
    if ( w_GnssTodFrac250ns > 3999 )
    {
      w_GnssTodFrac250ns = 3999;

    }
    q_GnssTodMsecModHour = q_GnssTodMsecModHour % HOUR_MSECS;
    if ( q_GnssTodMsecModHour > 3599999 )
    {
      q_GnssTodMsecModHour = 3599999;
    }

    p_prov_loc_info_ie->gnss_SignalMeasurementInformation.measurementReferenceTime.gnss_TOD_msec = q_GnssTodMsecModHour;

    p_prov_loc_info_ie->gnss_SignalMeasurementInformation.measurementReferenceTime.m.gnss_TOD_fracPresent = 1;
    p_prov_loc_info_ie->gnss_SignalMeasurementInformation.measurementReferenceTime.gnss_TOD_frac = w_GnssTodFrac250ns;
    p_prov_loc_info_ie->gnss_SignalMeasurementInformation.measurementReferenceTime.m.gnss_TOD_uncPresent = 1;
    p_prov_loc_info_ie->gnss_SignalMeasurementInformation.measurementReferenceTime.gnss_TOD_unc =
                                                                                                  tm_lpp_utils_microsec_to_GADK(pGnssMeasClock->z_Time.z_GpsTime.f_ClkTimeUncMs * 1000);
    p_prov_loc_info_ie->gnss_SignalMeasurementInformation.measurementReferenceTime.gnss_TimeID.extElem1.count = 0;


    /*** GNSS MeasurementList (GPS and/or Glonass) ***/
    p_element = p_prov_loc_info_ie->gnss_SignalMeasurementInformation.gnss_MeasurementList.elem =
                                                                                                  tm_lpp_asn1_osys_IE_malloc(sizeof(GNSS_MeasurementForOneGNSS) * p_prov_loc_info_ie->gnss_SignalMeasurementInformation.gnss_MeasurementList.n);

    if ( p_element == NULL )
    {
      MSG_ERROR("TM_LPP: ProvLocInfo GNSS Meas build buffer alloc failure", 0, 0, 0);
      return ( FALSE ); // failure, fill error code in the calling func
    }
    /* Initialize */
    memset(p_element, 0, sizeof(GNSS_MeasurementForOneGNSS) * p_prov_loc_info_ie->gnss_SignalMeasurementInformation.gnss_MeasurementList.n);
  }

  /*** GNSS MeasurementList - GPS only ***/
  if ( (pGpsMeasRpt != NULL) && (p_element != NULL) )
  {
    p_gps_meas_per_sv = (gnss_MeasStructType *)&pGpsMeasRpt->z_Gnss[0];

    p_element->gnss_ID.gnss_id = GNSS_ID_gnss_id_gps;

    /* this is a good SV to report */
    p_element->gnss_SgnMeasList.n = 1;
    p_gnss_elem = p_element->gnss_SgnMeasList.elem = tm_lpp_asn1_osys_IE_malloc(sizeof(GNSS_SgnMeasElement));
    if ( p_gnss_elem == NULL )
    {
      MSG_ERROR("TM_LPP: ProvLocInfo GPS Meas build buffer alloc failure", 0, 0, 0);
      tm_lpp_asn1_osys_IE_free((void **)&p_prov_loc_info_ie->gnss_SignalMeasurementInformation.gnss_MeasurementList.elem);
      return ( FALSE ); // failure, fill error code in the calling func
    }

    p_gnss_elem->gnss_SignalID.gnss_SignalID = LPP_GNSS_SIGNAL_ID_GPS_L1CA;
    p_gnss_elem->gnss_SatMeasList.n = uTotalGoodGpsSVs;
    p_sat_elem = p_gnss_elem->gnss_SatMeasList.elem = tm_lpp_asn1_osys_IE_malloc(sizeof(GNSS_SatMeasElement) * uTotalGoodGpsSVs);
    if ( NULL ==  p_sat_elem )
    {
      MSG_ERROR("TM_LPP: Prov Loc GPS Meas build buffer alloc failure", 0, 0, 0);
      tm_lpp_asn1_osys_IE_free((void **)&p_element->gnss_SgnMeasList.elem);
      tm_lpp_asn1_osys_IE_free((void **)&p_prov_loc_info_ie->gnss_SignalMeasurementInformation.gnss_MeasurementList.elem);
      return ( FALSE ); // failure, fill error code in the calling func
    }

    for ( u_i = 0; u_i < uTotalGpsSvs; ++u_i, ++p_gps_meas_per_sv )
    {
      if ( (p_gps_meas_per_sv->w_Cno > 0) &&
           (gps_meas_status_bits_check((uint16)p_gps_meas_per_sv->q_MeasStatus, b_status_check_needed)) )
      {
        p_sat_elem->svID.satellite_id = p_gps_meas_per_sv->w_Sv - 1;
        p_sat_elem->cNo = (uint8)((float)(p_gps_meas_per_sv->w_Cno + pGpsMeasRpt->z_Gnss[u_i].w_RfLoss) * (FLT)0.1);

        /* doppler: provided in m/s
           f_Num = f_SvSpeed *(FLT)(-1.0); 
            */
        f_Num = (p_gps_meas_per_sv->z_FiltMeas.f_SvSpeed  - pGpsMeasRpt->z_MeasClock.z_Freq.f_ClkFreqBias);
        x_Word = (int16)(f_Num * 25.0f); /* resolution .04 m/s */
        p_sat_elem->m.dopplerPresent = 1;
        p_sat_elem->doppler = x_Word * (-1); /* need to change polarity since protocol definition of doppler is reverse of what ME is using */

        /*** We tell location server that the reference point of the measurement is
           at TOW, which is now contained in p_msrset->gpsTOW,
           but the actually GPS moment of the measurement is instead at
           the FCOUNT of the measurement, whose GPS msec is
           p_zMeasGpsTime->q_Msec - p_zMeasGpsTime->f_ClkBias.
           We can adjust each SV's submsec value by adding
           p_zMeasGpsTime->f_ClkBias to it. */

        d_AdjSubMsec = (double)p_gps_meas_per_sv->z_FiltMeas.z_SvTime.f_SubMs
                       + (double)pGpsMeasRpt->z_MeasClock.z_Time.z_GpsTime.f_ClkTimeBias;

        if ( d_AdjSubMsec > 1.0 )
        {
          /* if the adjustment results in the submsec value above one, at the
             reference moment (TOW) SV time's msec should be one msec more,
             thus we need to subtract 1 ms from the submsec variable */

          d_AdjSubMsec -= 1.0;
        }
        else if  ( d_AdjSubMsec < 0 )
        {
          /** or if the adjustment results in submsec value negative, at the
            reference moment (TOW) SV time's msec should be one msec less,
            thus we need to use one minus the resuting "submsec" value for
            the submsec value */

          d_AdjSubMsec = 1.0 + d_AdjSubMsec;
        }

        /* both whole chip and fraction of a chip are based on "code phase", so
           convert submsec to code phase */
        d_AdjSubMsec = 1.0 - d_AdjSubMsec;
        /* whole chips: whole chip representation of the code phase */
        d_Num = d_AdjSubMsec * 2097152.0;
        /* whole chips in msec, floating */
        p_sat_elem->codePhase = (uint32)d_Num; /* whole chips in msec, U10 */
        p_sat_elem->m.integerCodePhasePresent = 0;
        p_gnss_elem->m.gnss_CodePhaseAmbiguityPresent = 0;
        /* multipath indicator */
        p_sat_elem->mpathDet = (uint8)LPP_SatMeasElement_mpathDet_notMeasured; /* not used currently */

        /* PseudoRange RMS error: converted from SV time uncertainty */
        f_Num = (float)(p_gps_meas_per_sv->z_FiltMeas.f_SvTimeUncMs * (FLT)LIGHT_MSEC);
        p_sat_elem->codePhaseRMSError = tm_umts_common_utils_psudoRMS_formater(f_Num);

        if ( p_sat_elem->codePhaseRMSError > 62 )
        {
          p_sat_elem->codePhaseRMSError = 62; /* to avoid being considered as xcorelation by some pdm */
        }
        if( msg_status( MSG_SSID_GPSSM, MSG_LEGACY_MED ) )
        {
          (void)snprintf(tm_f3_msg_buffer, C_TM_F3_MSG_BUF_SIZE, "%2.2d %3.3X %8.6f %2.2d %2.2d",
                         (int)p_gps_meas_per_sv->w_Sv,
                         (int)p_gps_meas_per_sv->q_MeasStatus,
                         p_gps_meas_per_sv->z_FiltMeas.f_SvTimeUncMs,
                         p_sat_elem->codePhaseRMSError, u_i);
          MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", tm_f3_msg_buffer);
        }
        ++p_sat_elem;
      }
    }

    MSG_MED("GPS Measurements built for %d SVs", uTotalGoodGpsSVs, 0, 0);

    /* Prepare p_element array (by incrementing) to load the next Glonass measurement. If for some reason GPS measurement
       aren't available continue to load with Glonass measurement. */
    ++p_element;
  } /* GPS Measurement Report */


  /*** GNSS MeasurementList - GLONASS only ***/
  if ( (pGloMeasRpt != NULL) && (uTotalGoodGloSVs > 0) && (p_element != NULL) )
  {
    p_glo_meas_per_sv = (gnss_MeasStructType *)&pGloMeasRpt->z_Gnss[0];
    p_element->gnss_ID.gnss_id = GNSS_ID_gnss_id_glonass;

    /* this is a good SV to report */
    p_element->gnss_SgnMeasList.n = 1;
    p_gnss_elem = p_element->gnss_SgnMeasList.elem = tm_lpp_asn1_osys_IE_malloc(sizeof(GNSS_SgnMeasElement));
    if ( p_gnss_elem == NULL )
    {
      MSG_ERROR("TM_LPP: ProvLocInfo GPS Meas build buffer alloc failure", 0, 0, 0);
      tm_lpp_asn1_osys_IE_free((void **)&p_prov_loc_info_ie->gnss_SignalMeasurementInformation.gnss_MeasurementList.elem);
      return ( FALSE ); // failure, fill error code in the calling func
    }


    p_gnss_elem->gnss_SignalID.gnss_SignalID = LPP_GNSS_SIGNAL_ID_GLONASS_G1;
    p_gnss_elem->m.gnss_CodePhaseAmbiguityPresent = 0;
    // p_gnss_elem->gnss_CodePhaseAmbiguity = 20;  // CPA 0..127. Set 20ms as per RG
    p_gnss_elem->gnss_SatMeasList.n = uTotalGoodGloSVs;
    p_sat_elem = p_gnss_elem->gnss_SatMeasList.elem = tm_lpp_asn1_osys_IE_malloc(sizeof(GNSS_SatMeasElement) * uTotalGoodGloSVs);
    if ( NULL ==  p_sat_elem )
    {
      MSG_ERROR("TM_LPP: Prov Loc GPS Meas build buffer alloc failure", 0, 0, 0);
      tm_lpp_asn1_osys_IE_free((void **)&p_element->gnss_SgnMeasList.elem);
      tm_lpp_asn1_osys_IE_free((void **)&p_prov_loc_info_ie->gnss_SignalMeasurementInformation.gnss_MeasurementList.elem);
      return ( FALSE ); // failure, fill error code in the calling func
    }

    for ( u_i = 0; u_i < uTotalGloSvs; ++u_i, ++p_glo_meas_per_sv )
    {
      if ( (p_glo_meas_per_sv->w_Cno > 0) &&
           (gnss_meas_status_bits_check(p_glo_meas_per_sv->q_MeasStatus, b_status_check_needed)) )
      {
        // svID.satellite_id (0..63). interpretation depends on Gnss-ID, For Glonass 0..23 are valid
        p_sat_elem->svID.satellite_id = p_glo_meas_per_sv->w_Sv - (GLO_SLOTID_OFFSET + 1);
        // cNo 0..63 unit 1 dB-Hz
        p_sat_elem->cNo = (uint8)((float)(p_glo_meas_per_sv->w_Cno + pGloMeasRpt->z_Gnss[u_i].w_RfLoss) * (FLT)0.1);

        // carrierQualityInd isn't supported by UE
        p_sat_elem->m.carrierQualityIndPresent = 0;

        // adr isn't supported by UE
        p_sat_elem->m.adrPresent = 0;

        /* doppler: provided in m/s f_Num = f_SvSpeed *(FLT)(-1.0); */
        f_Num = (p_glo_meas_per_sv->z_FiltMeas.f_SvSpeed  - pGloMeasRpt->z_MeasClock.z_Freq.f_ClkFreqBias);
        x_Word = (int16)(f_Num * 25.0f); /* resolution .04 m/s */
        p_sat_elem->m.dopplerPresent = 1;
        p_sat_elem->doppler = x_Word * (-1); /* need to change polarity since protocol definition of doppler is reverse of what ME is using */

        /*** We tell location server that the reference point of the measurement is
           at TOW, which is now contained in p_msrset->gpsTOW,
           but the actually GPS moment of the measurement is instead at
           the FCOUNT of the measurement, whose GPS msec is
           p_zMeasGpsTime->q_Msec - p_zMeasGpsTime->f_ClkBias.
           We can adjust each SV's submsec value by adding
           p_zMeasGpsTime->f_ClkBias to it. */

        d_AdjSubMsec = (double)p_glo_meas_per_sv->z_FiltMeas.z_SvTime.f_SubMs
                       + (double)pGloMeasRpt->z_MeasClock.z_Time.z_GpsTime.f_ClkTimeBias;

        if ( d_AdjSubMsec > 1.0 )
        {
          /* if the adjustment results in the submsec value above one, at the
             reference moment (TOW) SV time's msec should be one msec more,
             thus we need to subtract 1 ms from the submsec variable */

          d_AdjSubMsec -= 1.0;
        }
        else if ( d_AdjSubMsec < 0 )
        {
          /** or if the adjustment results in submsec value negative, at the
            reference moment (TOW) SV time's msec should be one msec less,
            thus we need to use one minus the resuting "submsec" value for
            the submsec value */

          d_AdjSubMsec = 1.0 + d_AdjSubMsec;
        }

        /* both whole chip and fraction of a chip are based on "code phase", so
           convert submsec to code phase */
        d_AdjSubMsec = 1.0 - d_AdjSubMsec;
        /* whole chips: whole chip representation of the code phase */
        d_Num = d_AdjSubMsec * 2097152.0;
        /* whole chips in msec, floating */
        p_sat_elem->codePhase = (uint32)d_Num; /* whole chips in msec, U10 */
        p_sat_elem->m.integerCodePhasePresent = 0;
        p_gnss_elem->m.gnss_CodePhaseAmbiguityPresent = 0;

        // Multipath indicator
        if ( p_glo_meas_per_sv->u_MultipathEstValid )
        {
          /* mpathDet - multipath indication 0..3 0(notMeasured), 1(low), 2(medium) & 3(high)
          f_MultipathEst in meters */
          if ( p_glo_meas_per_sv->f_MultipathEst < LPP_MEAS_MULTI_PATH_LOW_MTRs )
          {
            p_sat_elem->mpathDet = LPP_SatMeasElement_mpathDet_low;
          }
          else if ( (p_glo_meas_per_sv->f_MultipathEst >= LPP_MEAS_MULTI_PATH_LOW_MTRs) &&
                    (p_glo_meas_per_sv->f_MultipathEst < LPP_MEAS_MULTI_PATH_MED_MTRs) )
          {
            p_sat_elem->mpathDet = LPP_SatMeasElement_mpathDet_medium;
          }
          else if ( p_glo_meas_per_sv->f_MultipathEst >= LPP_MEAS_MULTI_PATH_MED_MTRs )
          {
            p_sat_elem->mpathDet = LPP_SatMeasElement_mpathDet_high;
          }
        }
        else
        {
          /* mpathDet 0(notMeasured) */
          p_sat_elem->mpathDet = LPP_SatMeasElement_mpathDet_notMeasured;
        }
        /* PseudoRange RMS error: converted from SV time uncertainty */
        f_Num = (float)(p_glo_meas_per_sv->z_FiltMeas.f_SvTimeUncMs * (FLT)LIGHT_MSEC);
        p_sat_elem->codePhaseRMSError = tm_umts_common_utils_psudoRMS_formater(f_Num);

        if ( p_sat_elem->codePhaseRMSError > 62 )
        {
          p_sat_elem->codePhaseRMSError = 62; /* to avoid being considered as xcorelation by some pdm */
        }
        if( msg_status( MSG_SSID_GPSSM, MSG_LEGACY_MED ) )
        {
          (void)snprintf(tm_f3_msg_buffer, C_TM_F3_MSG_BUF_SIZE, "Glo meas: %u %lu %8.6f %2.2d",
                         p_glo_meas_per_sv->w_Sv,
                         p_glo_meas_per_sv->q_MeasStatus,
                         p_glo_meas_per_sv->z_FiltMeas.f_SvTimeUncMs,
                         p_sat_elem->codePhaseRMSError);
          MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", tm_f3_msg_buffer);
        }
        ++p_sat_elem;
      }
    }
    MSG_MED("Glonass measurements built for %d SVs", uTotalGoodGloSVs, 0, 0);
  } /* Glonass Measurement Report */

  return ( TRUE );
}

/*
 ******************************************************************************
 * Function description:
 *  Builds LPP measurePosResponse's gps_measurement IE.
 *
 * Parameters:
 *
 *
 * Return value:
 *
 *    u_sv_cnt:  number of reportable SVs.
 *    
 *
 ******************************************************************************
*/


uint8 tm_lpp_prov_loc_otdoa_meas_build( const sm_OTDOA_ProvideLocationInformation *p_lte_loc_otdoa_data,
                                        OTDOA_ProvideLocationInformation *p_otdoa_ie)
{
  uint8                                 u_cell_cnt       = 0;
  uint8                                 u_i              = 0;
  uint8                                 u_index          = 0;
  sm_OTDOA_SignalMeasurementInformation *p_sm_otdoa_data = NULL;

  if ( (NULL == p_otdoa_ie) || (NULL == p_lte_loc_otdoa_data) || (0 == p_lte_loc_otdoa_data->m.otdoaSignalMeasurementInformationPresent) )
  {
    return ( 0 );
  }

  memset(p_otdoa_ie, 0, sizeof(OTDOA_ProvideLocationInformation));

  p_sm_otdoa_data = (sm_OTDOA_SignalMeasurementInformation *)&p_lte_loc_otdoa_data->otdoaSignalMeasurementInformation;

  MSG_MED("TM_LPP: Build Prov Loc OTDOA Meas. GlobalRefPresent %d ",
          p_sm_otdoa_data->m.cellGlobalIdRefPresent, 0, 0);

  p_otdoa_ie->otdoaSignalMeasurementInformation.systemFrameNumber.numbits = 10;
  if ( (tm_lpp_osys_asn1_int_to_bitstring(p_sm_otdoa_data->systemFrameNumber,
                                          &p_otdoa_ie->otdoaSignalMeasurementInformation.systemFrameNumber)) == FALSE )
  {
    MSG_ERROR("TM_LPP: Could not convert int to bitstring", 0, 0, 0);
  }

  p_otdoa_ie->otdoaSignalMeasurementInformation.physCellIdRef = p_sm_otdoa_data->physCellIdRef;
  if ( p_sm_otdoa_data->m.cellGlobalIdRefPresent == 1 )
  {
    p_otdoa_ie->otdoaSignalMeasurementInformation.m.cellGlobalIdRefPresent = 1;
    p_otdoa_ie->otdoaSignalMeasurementInformation.cellGlobalIdRef.cellidentity.numbits =
      p_sm_otdoa_data->cellGlobalIdRef.cell_id_numbits;
    if ( (tm_lpp_osys_asn1_int_to_bitstring(p_sm_otdoa_data->cellGlobalIdRef.cellidentity,
                                            &p_otdoa_ie->otdoaSignalMeasurementInformation.cellGlobalIdRef.cellidentity)) == FALSE )
    {
      MSG_ERROR("TM_LPP: Could not convert int to bitstring", 0, 0, 0);
    }


    p_otdoa_ie->otdoaSignalMeasurementInformation.cellGlobalIdRef.mnc.n =
      p_sm_otdoa_data->cellGlobalIdRef.num_mnc_digits;
    p_otdoa_ie->otdoaSignalMeasurementInformation.cellGlobalIdRef.mnc.elem = 
      tm_lpp_asn1_osys_IE_malloc( sizeof(uint8) * p_otdoa_ie->otdoaSignalMeasurementInformation.cellGlobalIdRef.mnc.n );
    if ( NULL == p_otdoa_ie->otdoaSignalMeasurementInformation.cellGlobalIdRef.mnc.elem )
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "TM_LPP: MNC buffer allocation failed for OTDOA" );
      return ( 0 );
    }

    for ( u_i = 0; 
          u_i < p_otdoa_ie->otdoaSignalMeasurementInformation.cellGlobalIdRef.mnc.n;
          u_i++ )
    {
      p_otdoa_ie->otdoaSignalMeasurementInformation.cellGlobalIdRef.mnc.elem[u_i] = p_sm_otdoa_data->cellGlobalIdRef.mnc[u_i];
    }
    p_otdoa_ie->otdoaSignalMeasurementInformation.cellGlobalIdRef.mcc.n = 3;
    p_otdoa_ie->otdoaSignalMeasurementInformation.cellGlobalIdRef.mcc.elem =
      tm_lpp_asn1_osys_IE_malloc( sizeof(uint8) * p_otdoa_ie->otdoaSignalMeasurementInformation.cellGlobalIdRef.mcc.n );
    if ( NULL == p_otdoa_ie->otdoaSignalMeasurementInformation.cellGlobalIdRef.mcc.elem )
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "TM_LPP: MCC buffer allocation failed for OTDOA" );
      tm_lpp_asn1_osys_IE_free((void **)&p_otdoa_ie->otdoaSignalMeasurementInformation.cellGlobalIdRef.mnc.elem);
      return ( 0 );
    }
    for ( u_i = 0; 
          u_i < p_otdoa_ie->otdoaSignalMeasurementInformation.cellGlobalIdRef.mcc.n;
          u_i++ )
    {
      p_otdoa_ie->otdoaSignalMeasurementInformation.cellGlobalIdRef.mcc.elem[u_i] = p_sm_otdoa_data->cellGlobalIdRef.mcc[u_i];
    }
  }

  /* earfcnRef & earfcnRef-v9a0 are mutually exclusive. */
  if ( p_sm_otdoa_data->m.earfcnRefPresent == 1 )
  {
    if ( p_sm_otdoa_data->earfcnRef <= ASN1V_maxEARFCN )
    {
      /* Spec (0..65535) */
      p_otdoa_ie->otdoaSignalMeasurementInformation.m.earfcnRefPresent = 1;
      p_otdoa_ie->otdoaSignalMeasurementInformation.earfcnRef = p_sm_otdoa_data->earfcnRef;
    }
#ifdef FEATURE_GNSS_LPP_DOPPLER_CONF_EARFCN
    else if ( p_sm_otdoa_data->earfcnRef <= ASN1V_maxEARFCN2 )
    {
      /* Spec (65536..262143) */
      p_otdoa_ie->otdoaSignalMeasurementInformation.m.earfcnRef_v9a0Present = 1;
      p_otdoa_ie->otdoaSignalMeasurementInformation.earfcnRef_v9a0 = p_sm_otdoa_data->earfcnRef;
    }
#endif /* FEATURE_GNSS_LPP_DOPPLER_CONF_EARFCN */
  }


  p_otdoa_ie->otdoaSignalMeasurementInformation.m.referenceQualityPresent = p_sm_otdoa_data->m.referenceQualityPresent;
  if ( p_otdoa_ie->otdoaSignalMeasurementInformation.m.referenceQualityPresent == 1 )
  {
    p_otdoa_ie->otdoaSignalMeasurementInformation.referenceQuality.m.error_NumSamplesPresent = p_sm_otdoa_data->referenceQuality.m.error_NumSamplesPresent;
    p_otdoa_ie->otdoaSignalMeasurementInformation.referenceQuality.error_NumSamples.numbits = 3;
    if ( (tm_lpp_osys_asn1_int_to_bitstring(p_sm_otdoa_data->referenceQuality.error_NumSamples,
                                            &p_otdoa_ie->otdoaSignalMeasurementInformation.referenceQuality.error_NumSamples)) == FALSE )
    {
      MSG_ERROR("TM_LPP: Could not convert int to bitstring", 0, 0, 0);
    }

    p_otdoa_ie->otdoaSignalMeasurementInformation.referenceQuality.error_Resolution.numbits = 2;
    if ( (tm_lpp_osys_asn1_int_to_bitstring(p_sm_otdoa_data->referenceQuality.error_Resolution,
                                            &p_otdoa_ie->otdoaSignalMeasurementInformation.referenceQuality.error_Resolution)) == FALSE )
    {
      MSG_ERROR("TM_LPP: Could not convert int to bitstring", 0, 0, 0);
    }

    p_otdoa_ie->otdoaSignalMeasurementInformation.referenceQuality.error_Value.numbits = 5;
    if ( (tm_lpp_osys_asn1_int_to_bitstring(p_sm_otdoa_data->referenceQuality.error_Value,
                                            &p_otdoa_ie->otdoaSignalMeasurementInformation.referenceQuality.error_Value)) == FALSE )
    {
      MSG_ERROR("TM_LPP: Could not convert int to bitstring", 0, 0, 0);
    }
  }

  p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.n = p_sm_otdoa_data->neighbourMeasurementList.n;

  u_cell_cnt = (uint8)p_sm_otdoa_data->neighbourMeasurementList.n;

  /* UE currently can only include up to 24 RSTD measurements in one Provide Location Information */
  if ( p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.n > 0 )
  {
    /* Set OTDOA signal measurement present when the mandatory fields are present irrespective of 'otdoaSignalMeasurementInformationPresent' set by OTDOA engine */
    p_otdoa_ie->m.otdoaSignalMeasurementInformationPresent = 1;
    if ( (p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem =
            tm_lpp_asn1_osys_IE_malloc(sizeof(NeighbourMeasurementElement) *
                                       p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.n)) == NULL )
    {
      MSG_ERROR("TM_LPP: Prov Loc OTDOA Meas build buffer alloc failure", 0, 0, 0);
      tm_lpp_asn1_osys_IE_free((void **)&p_otdoa_ie->otdoaSignalMeasurementInformation.cellGlobalIdRef.mnc.elem);
      tm_lpp_asn1_osys_IE_free((void **)&p_otdoa_ie->otdoaSignalMeasurementInformation.cellGlobalIdRef.mcc.elem);
      return ( u_cell_cnt );
    }
    for ( u_i = 0; u_i < u_cell_cnt; u_i++ )
    {
      p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].m.cellGlobalIdNeighbourPresent =
        p_sm_otdoa_data->neighbourMeasurementList.elem[u_i].m.cellGlobalIdNeighbourPresent;
      if ( p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].m.cellGlobalIdNeighbourPresent == 1 )
      {
        p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].cellGlobalIdNeighbour.cellidentity.numbits = 28;
        if ( (tm_lpp_osys_asn1_int_to_bitstring(
                p_sm_otdoa_data->neighbourMeasurementList.elem[u_i].cellGlobalIdNeighbour.cellidentity,
                &p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].cellGlobalIdNeighbour.cellidentity)) == FALSE )
        {
          MSG_ERROR("TM_LPP: Could not convert int to bitstring", 0, 0, 0);
        }
        p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].cellGlobalIdNeighbour.mnc.n =
            p_sm_otdoa_data->neighbourMeasurementList.elem[u_i].cellGlobalIdNeighbour.num_mnc_digits;
        p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].cellGlobalIdNeighbour.mnc.elem = 
          tm_lpp_asn1_osys_IE_malloc( sizeof(uint8) * p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].cellGlobalIdNeighbour.mnc.n );
        if ( NULL == p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].cellGlobalIdNeighbour.mnc.elem )
        {
          MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "TM_LPP: MNC buffer allocation failed for OTDOA neighbours" );
          tm_lpp_asn1_osys_IE_free((void **)&p_otdoa_ie->otdoaSignalMeasurementInformation.cellGlobalIdRef.mnc.elem);
          tm_lpp_asn1_osys_IE_free((void **)&p_otdoa_ie->otdoaSignalMeasurementInformation.cellGlobalIdRef.mcc.elem);
          tm_lpp_asn1_osys_IE_free((void **)&p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem);
          return ( 0 );
       }

        for ( u_index = 0; 
              u_index < p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].cellGlobalIdNeighbour.mnc.n;
              u_index++ )
        {
          p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].cellGlobalIdNeighbour.mnc.elem[u_index] =
            p_sm_otdoa_data->neighbourMeasurementList.elem[u_i].cellGlobalIdNeighbour.mnc[u_index];
        }
        p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].cellGlobalIdNeighbour.mcc.n = 3;
        p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].cellGlobalIdNeighbour.mcc.elem = 
          tm_lpp_asn1_osys_IE_malloc( sizeof(uint8) * p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].cellGlobalIdNeighbour.mcc.n );
        if ( NULL == p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].cellGlobalIdNeighbour.mcc.elem )
        {
          MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "TM_LPP: MCC buffer allocation failed for OTDOA neighbours" );
          tm_lpp_asn1_osys_IE_free((void **)&p_otdoa_ie->otdoaSignalMeasurementInformation.cellGlobalIdRef.mnc.elem);
          tm_lpp_asn1_osys_IE_free((void **)&p_otdoa_ie->otdoaSignalMeasurementInformation.cellGlobalIdRef.mcc.elem);
          tm_lpp_asn1_osys_IE_free((void **)&p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem);
          tm_lpp_asn1_osys_IE_free((void **)&p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].cellGlobalIdNeighbour.mnc.elem);
          return ( 0 );
        }

        for ( u_index = 0; 
              u_index < p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].cellGlobalIdNeighbour.mcc.n;
              u_index++ )
        {
          p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].cellGlobalIdNeighbour.mcc.elem[u_index] =
            p_sm_otdoa_data->neighbourMeasurementList.elem[u_i].cellGlobalIdNeighbour.mcc[u_index];
        }
      }


      /* earfcnNeighbour & earfcnNeighbour -v9a0 are mutually exclusive */
      if ( p_sm_otdoa_data->neighbourMeasurementList.elem[u_i].m.earfcnNeighbourPresent == 1 )
      {
        if ( p_sm_otdoa_data->neighbourMeasurementList.elem[u_i].earfcnNeighbour <= ASN1V_maxEARFCN)
        {
          /* Spec : (0..65535) */
          p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].m.earfcnNeighbourPresent = 1;
          p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].earfcnNeighbour =
            p_sm_otdoa_data->neighbourMeasurementList.elem[u_i].earfcnNeighbour;
        }
#ifdef FEATURE_GNSS_LPP_DOPPLER_CONF_EARFCN
        else if ( p_sm_otdoa_data->neighbourMeasurementList.elem[u_i].earfcnNeighbour <= ASN1V_maxEARFCN2 )
        {
          /* Spec : (65536..262143) */
          p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].m.earfcnNeighbour_v9a0Present = 1;
          p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].earfcnNeighbour_v9a0 =
            p_sm_otdoa_data->neighbourMeasurementList.elem[u_i].earfcnNeighbour;
        }
#endif /* FEATURE_GNSS_LPP_DOPPLER_CONF_EARFCN */
      }


      p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].rstd_Quality.m.error_NumSamplesPresent = p_sm_otdoa_data->neighbourMeasurementList.elem[u_i].rstd_Quality.m.error_NumSamplesPresent;
      p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].rstd_Quality.error_NumSamples.numbits = 3;

      if ( (tm_lpp_osys_asn1_int_to_bitstring(
              p_sm_otdoa_data->neighbourMeasurementList.elem[u_i].rstd_Quality.error_NumSamples,
              &p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].rstd_Quality.error_NumSamples)) == FALSE )
      {
        MSG_ERROR("TM_LPP: Could not convert int to bitstring", 0, 0, 0);
      }
      p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].rstd_Quality.error_Resolution.numbits = 2;
      if ( (tm_lpp_osys_asn1_int_to_bitstring(
              p_sm_otdoa_data->neighbourMeasurementList.elem[u_i].rstd_Quality.error_Resolution,
              &p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].rstd_Quality.error_Resolution)) == FALSE )
      {
        MSG_ERROR("TM_LPP: Could not convert int to bitstring", 0, 0, 0);
      }
      p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].rstd_Quality.error_Value.numbits = 5;
      if ( (tm_lpp_osys_asn1_int_to_bitstring(
              p_sm_otdoa_data->neighbourMeasurementList.elem[u_i].rstd_Quality.error_Value,
              &p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].rstd_Quality.error_Value)) == FALSE )
      {
        MSG_ERROR("TM_LPP: Could not convert int to bitstring", 0, 0, 0);
      }
      p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].rstd =
        p_sm_otdoa_data->neighbourMeasurementList.elem[u_i].rstd;
      p_otdoa_ie->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[u_i].physCellIdNeighbour =
        p_sm_otdoa_data->neighbourMeasurementList.elem[u_i].physCellIdNeighbor;

    }
  }
  return ( u_cell_cnt );

}

/*
 ******************************************************************************
 * Function description:
 *  Builds LPP measurePosResponse's ECID IE.
 *
 * Parameters:
 *
 *
 * Return value:
 *
 *    TRUE - if success. FALSE otherwise
 *    
 *
 ******************************************************************************
*/
boolean tm_lpp_ecid_meas_element_build( MeasuredResultsElement *p_meas_element, gnss_lpp_ECID_MeasuredResultsElement *p_sm_data )
{
  uint8 u_index;

  if ( p_meas_element == NULL || p_sm_data == NULL )
  {
    return ( FALSE );
  }
  p_meas_element->m.cellGlobalIdPresent = p_sm_data->m.cellGlobalIdPresent;
  p_meas_element->m.rsrp_ResultPresent = p_sm_data->m.rsrp_ResultPresent;
  p_meas_element->m.rsrq_ResultPresent = p_sm_data->m.rsrq_ResultPresent;
  p_meas_element->m.systemFrameNumberPresent = p_sm_data->m.systemFrameNumberPresent;
  p_meas_element->m.ue_RxTxTimeDiffPresent = p_sm_data->m.ue_RxTxTimeDiffPresent;
  p_meas_element->physCellId = p_sm_data->w_physCellId;
  p_meas_element->arfcnEUTRA = p_sm_data->arfcnEUTRA;

  if ( p_meas_element->m.cellGlobalIdPresent == 1 )
  {
    p_meas_element->cellGlobalId.cellIdentity.t = p_sm_data->cellGlobalId.cellIdentity.t;
    if ( p_meas_element->cellGlobalId.cellIdentity.t == 1 )
    {
      if ( (p_meas_element->cellGlobalId.cellIdentity.u.eutra =
                                                                tm_lpp_asn1_osys_IE_malloc(sizeof(ASN1BitStr32))) == NULL )
      {
        MSG_ERROR("TM_LPP: Prov Loc OTDOA Meas build buffer alloc failure", 0, 0, 0);
        return ( FALSE );
      }
      p_meas_element->cellGlobalId.cellIdentity.u.eutra->numbits = p_sm_data->cellGlobalId.cellIdentity.cell_id_num_bits;
      if ( (tm_lpp_osys_asn1_int_to_bitstring(p_sm_data->cellGlobalId.cellIdentity.u.cellIdEutra, p_meas_element->cellGlobalId.cellIdentity.u.eutra)) == FALSE )
      {
        MSG_ERROR("TM_LPP: Could not convert int to bitstring", 0, 0, 0);
      }
    }
    p_meas_element->cellGlobalId.plmn_Identity.mnc.n = p_sm_data->cellGlobalId.plmn_Identity.num_mnc_digits;
    p_meas_element->cellGlobalId.plmn_Identity.mnc.elem = 
      tm_lpp_asn1_osys_IE_malloc( sizeof(uint8) * p_meas_element->cellGlobalId.plmn_Identity.mnc.n );
    if ( NULL == p_meas_element->cellGlobalId.plmn_Identity.mnc.elem )
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "TM_LPP: MNC buffer allocation failed for ECID");
      tm_lpp_asn1_osys_IE_free((void **)&p_meas_element->cellGlobalId.cellIdentity.u.eutra);
      return ( FALSE );
    }

    for ( u_index = 0;
          u_index < p_meas_element->cellGlobalId.plmn_Identity.mnc.n;
          u_index++ )
    {
      p_meas_element->cellGlobalId.plmn_Identity.mnc.elem[u_index] = p_sm_data->cellGlobalId.plmn_Identity.mnc[u_index];
    }

    p_meas_element->cellGlobalId.plmn_Identity.mcc.n = 3;
    p_meas_element->cellGlobalId.plmn_Identity.mcc.elem = 
      tm_lpp_asn1_osys_IE_malloc( sizeof(uint8) * p_meas_element->cellGlobalId.plmn_Identity.mcc.n );
    if ( NULL == p_meas_element->cellGlobalId.plmn_Identity.mcc.elem )
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "TM_LPP: MCC buffer allocation failed for ECID");
      tm_lpp_asn1_osys_IE_free((void **)&p_meas_element->cellGlobalId.cellIdentity.u.eutra);
      tm_lpp_asn1_osys_IE_free((void **)&p_meas_element->cellGlobalId.plmn_Identity.mnc.elem);
      return ( FALSE );
    }

    for ( u_index = 0;
          u_index < p_meas_element->cellGlobalId.plmn_Identity.mcc.n;
          u_index++ )
    {
      p_meas_element->cellGlobalId.plmn_Identity.mcc.elem[u_index] = p_sm_data->cellGlobalId.plmn_Identity.mcc[u_index];
    }
  }
  if ( p_meas_element->m.systemFrameNumberPresent == 1 )
  {
    p_meas_element->systemFrameNumber.numbits = 10;
    if ( (tm_lpp_osys_asn1_int_to_bitstring(p_sm_data->systemFrameNumber,
                                            &p_meas_element->systemFrameNumber)) == FALSE )
    {
      MSG_ERROR("TM_LPP: Could not convert int to bitstring", 0, 0, 0);
    }
  }
  if ( p_meas_element->m.rsrp_ResultPresent == 1 )
  {
    p_meas_element->rsrp_Result = p_sm_data->rsrp_Result;
  }
  if ( p_meas_element->m.rsrq_ResultPresent == 1 )
  {
    p_meas_element->rsrq_Result = p_sm_data->rsrq_Result;
  }
  if ( p_meas_element->m.ue_RxTxTimeDiffPresent == 1 )
  {
    p_meas_element->ue_RxTxTimeDiff = p_sm_data->ue_RxTxTimeDiff;
  }
  return ( TRUE );
}

/*
 ******************************************************************************
 * Function description:
 *  Builds LPP measurePosResponse's ECID IE.
 *
 * Parameters:
 *
 *
 * Return value:
 *
 *    TRUE - if success. FALSE otherwise
 *    
 *
 ******************************************************************************
*/
boolean tm_lpp_prov_loc_ecid_meas_build( const gnss_lpp_ECID_ProvideLocationInformation *p_ecid_data,
                                         ECID_ProvideLocationInformation *p_ecid_ie )
{
  uint8                                      u_index         = 0;
  boolean                                    b_ret           = TRUE;

  gnss_lpp_ECID_SignalMeasurementInformation *p_sm_ecid_data;

  memset((void *)p_ecid_ie, 0, sizeof(ECID_ProvideLocationInformation));

  MSG_MED("TM_LPP: Prov Loc ECID Meas build", 0, 0, 0);
  p_sm_ecid_data = (gnss_lpp_ECID_SignalMeasurementInformation *)&p_ecid_data->ecid_SignalMeasurementInformation;


  if ( p_sm_ecid_data->m.servingCellMeasuredResultsPresent ==  1 )
  {
    b_ret = tm_lpp_ecid_meas_element_build(&p_ecid_ie->ecid_SignalMeasurementInformation.primaryCellMeasuredResults,
                                           &p_sm_ecid_data->servingCellMeasuredResults);
    if ( b_ret == FALSE )
    {
      return ( b_ret );
    }
    p_ecid_ie->ecid_SignalMeasurementInformation.m.primaryCellMeasuredResultsPresent = p_sm_ecid_data->m.servingCellMeasuredResultsPresent;
  }

  if ( p_sm_ecid_data->measuredResultsList.n > 0 )
  {

    if ( (p_ecid_ie->ecid_SignalMeasurementInformation.measuredResultsList.elem  =
                                                                                   tm_lpp_asn1_osys_IE_malloc(sizeof(MeasuredResultsElement) *
                                                                                                              p_sm_ecid_data->measuredResultsList.n)) == NULL )
    {
      return ( b_ret );
    }
    for ( u_index = 0; u_index < p_sm_ecid_data->measuredResultsList.n; u_index++ )
    {
      b_ret = tm_lpp_ecid_meas_element_build(&p_ecid_ie->ecid_SignalMeasurementInformation.measuredResultsList.elem[u_index],
                                             &p_sm_ecid_data->measuredResultsList.elem[u_index]);
      if ( b_ret == FALSE )
      {
        tm_lpp_asn1_osys_IE_free((void **)&p_ecid_ie->ecid_SignalMeasurementInformation.measuredResultsList.elem);
        return ( b_ret );
      }

    }
    p_ecid_ie->ecid_SignalMeasurementInformation.measuredResultsList.n = p_sm_ecid_data->measuredResultsList.n;
  }
  if ( b_ret == TRUE )
  {
    p_ecid_ie->m.ecid_SignalMeasurementInformationPresent = 1;
  }
  return ( b_ret );
}

/*=========================================================================
  tm_lpp_convert veloity
  
  Description:
    LPP function to encode Velocity in 
     Provide Location Information message
    

  Returns:
    Void   
==========================================================================*/

static void tm_lpp_convert_velocity
( const float f_vel_enu[],
  float f_velunc,
  float f_velvertunc,
  HorizontalWithVerticalVelocityAndUncertainty *p_velocity
 )

{
  float  f_heading_ang;
  uint16 w_hor_velocity;
  int16  s_bearing;
  int8   b_vertical_velocity;
  uint8  u_ver_direct;
  uint8  u_ver_velocity_bit_string;
  float  f_vel_east;
  float  f_vel_north;
  float  f_vel_vertical;
  boolean b_ret = FALSE;
  float  f_temp                    = 0;

  if ( p_velocity == NULL )
  {
    MSG_ERROR("TM_LPP: Mem not allocated for vel.", 0, 0, 0);
    return;
  }
  f_vel_east = f_vel_enu[0];
  f_vel_north = f_vel_enu[1];
  f_vel_vertical = f_vel_enu[2];

  tm_umts_common_convert_velocity(&f_heading_ang,
                                  &w_hor_velocity,
                                  &s_bearing,
                                  &b_vertical_velocity,
                                  &u_ver_direct,
                                  &u_ver_velocity_bit_string,
                                  &f_vel_east,
                                  &f_vel_north,
                                  &f_vel_vertical);
  /* vertical velocity direction */
  if ( f_vel_vertical < 0.0f )
  {
    u_ver_direct = HorizontalWithVerticalVelocityAndUncertainty_verticalDirection_downward; 
  }
  else
  {
    u_ver_direct = HorizontalWithVerticalVelocityAndUncertainty_verticalDirection_upward; 
  }

  p_velocity->bearing = (uint16)(fabs(s_bearing));
  p_velocity->horizontalSpeed = w_hor_velocity;
  p_velocity->verticalSpeed = u_ver_velocity_bit_string;
  p_velocity->verticalDirection = u_ver_direct;
  p_velocity->horizontalUncertaintySpeed = (uint8)f_velunc;
  p_velocity->verticalUncertaintySpeed = (uint8)f_velvertunc;
}


/*===========================================================================

FUNCTION tm_lpp_set_end_trans_flag

DESCRIPTION
  This function decides whether the LPP end transaction should be set or not,
  based on the transaction element flags.

PARAMETERS
  p_lpp_tx_type : Transaction element
  p_lpp_pdu : LPP ASN message structure
 
RETURN VALUE
  None 

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_lpp_set_end_trans_flag( tm_lpp_transaction_info_s_type *p_lpp_tx_type, LPP_Message *p_lpp_pdu )
{
  /* for multi-periodic report except the last report, do not set End Transaction flag */
  if ( (p_lpp_tx_type->z_lpp_data_type.q_flags & LPP_ENC_PROV_LOC_PERIODIC_RPT) == LPP_ENC_PROV_LOC_PERIODIC_RPT )
  {
    if ( (p_lpp_tx_type->z_lpp_data_type.q_flags & LPP_ENC_PROV_LOC_PERIODIC_RPT_FINAL) == LPP_ENC_PROV_LOC_PERIODIC_RPT_FINAL )
    {
      MSG_MED("FINAL multi-rpt", 0, 0, 0);
      p_lpp_tx_type->b_end_sent = TRUE;
      p_lpp_pdu->endTransaction = TRUE;
    }
    else
    {
      MSG_MED("NOT FINAL multi-rpt", 0, 0, 0);
      p_lpp_tx_type->b_end_sent = FALSE;
      p_lpp_pdu->endTransaction = FALSE;
    }
  }
  else
  {

    p_lpp_tx_type->b_end_sent = FALSE;
    p_lpp_pdu->endTransaction = FALSE;

    /* Check multiple RSTD measurements are expected */
    if ( p_lpp_tx_type->z_lpp_data_type.p_otoda_data != NULL )
    {
      sm_loc_lte_otdoa_meas_info_type *const pOtodaData = p_lpp_tx_type->z_lpp_data_type.p_otoda_data;

      if ( pOtodaData->u_SeqNum != pOtodaData->u_MaxMessageNum )
      {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Not final multiple RSTD measurements");
        p_lpp_tx_type->b_end_sent = FALSE;
        p_lpp_pdu->endTransaction = FALSE;
        return;
      }
    }
    if (p_lpp_tx_type->z_lpp_data_type.b_earlyResponse_needed == FALSE) 
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Final multiple RSTD measurements");
      p_lpp_tx_type->b_end_sent = TRUE;
      p_lpp_pdu->endTransaction = TRUE;
    }
  }
}
/*=========================================================================
  tm_lpp_encode_provide_location
  
  Description:
    LPP function to encode Provide Location Information message
    

  Returns:
    TRUE if the encoding passed, FALSE otherwise    
==========================================================================*/

boolean tm_lpp_encode_provide_location( tm_lpp_transaction_info_s_type *p_lpp_tx_type )
{
  LPP_Message                       *p_lpp_pdu;
  ProvideLocationInformation_r9_IEs *p_prov_loc_r9;
  int                               encode_status;
  uint8                             u_cell_cnt           = 0;
  tm_lpp_transaction_id_s_type      z_trans_id;
  boolean                           b_ret                = TRUE;
  uint32                            q_GnssTodMsecModHour;
  float                             f_GnssTodFracPart;
  uint16                            w_GnssTodFrac250ns;

  if ( p_lpp_tx_type == NULL )
  {
    MSG_ERROR("TM_LPP: Null transcation pointer", 0, 0, 0);
    return ( FALSE );
  }

  MSG_MED("TM_LPP:Enc Prov Loc. Flag 0x%lX", p_lpp_tx_type->z_lpp_data_type.q_flags, 0, 0);

  p_lpp_pdu =
              tm_lpp_asn1_osys_IE_malloc(sizeof(LPP_Message));

  if ( p_lpp_pdu == NULL )
  {

    MSG_ERROR("TM_LPP:Enc Prov Loc buffer alloc failure", 0, 0, 0);
    return ( FALSE );
  }
  p_lpp_tx_type->z_lpp_data_type.p_pdu_ptr = p_lpp_pdu;
  z_trans_id = p_lpp_tx_type->z_current_trans_id;
  p_lpp_pdu->m.transactionIDPresent = 0;
  if ( p_lpp_tx_type->z_current_trans_id.m.u_trans_id_valid )
  {
    p_lpp_pdu->m.transactionIDPresent = 1;
    p_lpp_pdu->transactionID.initiator = (uint8)z_trans_id.u_initiator;
    p_lpp_pdu->transactionID.transactionNumber = (uint8)z_trans_id.u_transaction_id;
  }

  tm_lpp_set_end_trans_flag(p_lpp_tx_type, p_lpp_pdu);

  /* LPP CP Specific Changes : BEGIN */
  if ( p_lpp_tx_type->u_lpp_session_type == TM_LPP_SESSION_TYPE_CP )
  {
    p_lpp_pdu->m.sequenceNumberPresent = 1;
    tm_lpp_cp_get_nxt_out_seq_no(p_lpp_tx_type);
    p_lpp_pdu->sequenceNumber = p_lpp_tx_type->u_out_sequence_num;

    p_lpp_pdu->m.acknowledgementPresent = 1;
    p_lpp_pdu->acknowledgement.ackRequested = 1;           /* Request Server ACK to LPP Prov Cap           */
    p_lpp_pdu->acknowledgement.m.ackIndicatorPresent = 0;  /* Make sure clear unused field                 */
  }
  /* LPP CP Specific Changes : END */

  /* for multi-periodic report dont ask for ack */
  if ((p_lpp_tx_type->z_lpp_data_type.q_flags & LPP_ENC_PROV_LOC_PERIODIC_RPT) == LPP_ENC_PROV_LOC_PERIODIC_RPT)
  {
    /* for periodic reports, dont ask for Ack from the server. this will avoid unnecssary race
       conditions where the ack is not received in time */
    p_lpp_pdu->m.acknowledgementPresent = 0;
    p_lpp_pdu->acknowledgement.ackRequested = 0;           /* Request Server ACK to LPP Prov Cap           */
  }

  p_lpp_pdu->m.lpp_MessageBodyPresent = 1;

  p_lpp_pdu->lpp_MessageBody.t = LPP_MESSAGEBODY_C1;
  if ( (p_lpp_pdu->lpp_MessageBody.u.c1 = tm_lpp_asn1_osys_IE_malloc(sizeof(LPP_MessageBody_c1))) == NULL )
  {
    MSG_ERROR("TM_LPP:Enc Prov Loc buffer alloc failure", 0, 0, 0);
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_LOC_INFO);
    return ( FALSE );
  }

  p_lpp_pdu->lpp_MessageBody.u.c1->t = LPP_MESSAGE_PROV_LOC_INFO;


  if ( (p_lpp_pdu->lpp_MessageBody.u.c1->u.provideLocationInformation =
                                                                        tm_lpp_asn1_osys_IE_malloc(sizeof(ProvideLocationInformation))) == NULL )
  {
    MSG_ERROR("TM_LPP:Enc Prov Loc buffer alloc failure", 0, 0, 0);
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_LOC_INFO);
    return ( FALSE );
  }
  p_lpp_pdu->lpp_MessageBody.u.c1->u.provideLocationInformation->criticalExtensions.t = LPP_PROV_LOC_CRITICAL_EXT;
  if ( (p_lpp_pdu->lpp_MessageBody.u.c1->u.provideLocationInformation->criticalExtensions.u.c1 =
                                                                                                 tm_lpp_asn1_osys_IE_malloc(sizeof(ProvideLocationInformation_criticalExtensions_c1))) == NULL )
  {
    MSG_ERROR("TM_LPP:Enc Prov Loc buffer alloc failure", 0, 0, 0);
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_LOC_INFO);
    return ( FALSE );
  }

  p_lpp_pdu->lpp_MessageBody.u.c1->u.provideLocationInformation->criticalExtensions.u.c1->t = LPP_PROV_LOC_CRITICAL_EXT_R9;
  MSG_HIGH("TM_LPP flags %d", p_lpp_tx_type->z_lpp_data_type.q_flags, 0, 0);

  if ( (p_lpp_pdu->lpp_MessageBody.u.c1->u.provideLocationInformation->criticalExtensions.u.c1->u.provideLocationInformation_r9 =
                                                                                                                                  tm_lpp_asn1_osys_IE_malloc(sizeof(ProvideLocationInformation_r9_IEs))) == NULL )
  {
    MSG_ERROR("TM_LPP:Enc Prov Loc buffer alloc failure", 0, 0, 0);
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_LOC_INFO);
    return ( FALSE );
  }
  p_prov_loc_r9 = p_lpp_pdu->lpp_MessageBody.u.c1->u.provideLocationInformation->criticalExtensions.u.c1->u.provideLocationInformation_r9;

  if (p_lpp_tx_type->z_lpp_data_type.b_earlyResponse_needed == TRUE) 
  {
     p_prov_loc_r9->m.commonIEsProvideLocationInformationPresent = 1;
     p_prov_loc_r9->commonIEsProvideLocationInformation.m._v2ExtPresent = 1;
     p_prov_loc_r9->commonIEsProvideLocationInformation.m.earlyFixReport_r12Present = 1;
     p_prov_loc_r9->commonIEsProvideLocationInformation.earlyFixReport_r12 = EarlyFixReport_r12_noMoreMessages;
     if (((p_lpp_tx_type->z_lpp_data_type.q_flags & LPP_ENC_OTDOA_DATA)!=0)&&
         (NULL!=p_lpp_tx_type->z_lpp_data_type.p_otoda_data))
     {
        if (p_lpp_tx_type->z_lpp_data_type.p_otoda_data->u_SeqNum !=
            p_lpp_tx_type->z_lpp_data_type.p_otoda_data->u_MaxMessageNum) 
        {
           p_prov_loc_r9->commonIEsProvideLocationInformation.earlyFixReport_r12 = EarlyFixReport_r12_moreMessagesOnTheWay;
        }
     }
  }

  if ( (p_lpp_tx_type->z_lpp_data_type.q_flags & LPP_ERROR_DATA_VALID) &&
       (p_lpp_tx_type->z_lpp_data_type.lpp_error.u_lpp_error_type & LPP_ERROR_TYPE_PROV_LOC_COMMON_ERROR) )
  {
    // send gnss-error here
    p_prov_loc_r9->m.commonIEsProvideLocationInformationPresent = 1;
    p_prov_loc_r9->commonIEsProvideLocationInformation.m.locationErrorPresent = 1;
    p_prov_loc_r9->commonIEsProvideLocationInformation.locationError.locationfailurecause = p_lpp_tx_type->z_lpp_data_type.lpp_error.u_lpp_error_value;
  }
  else
  {
    if ( p_lpp_tx_type->z_lpp_data_type.q_flags & LPP_ENC_POS_DATA )
    {
      p_prov_loc_r9->m.commonIEsProvideLocationInformationPresent = 1;
      if ( p_lpp_tx_type->z_lpp_data_type.q_flags & LPP_POS_DATA_VALID )
      {
        p_prov_loc_r9->commonIEsProvideLocationInformation.m.locationEstimatePresent = 1;
        p_prov_loc_r9->commonIEsProvideLocationInformation.locationEstimate.t = LPP_PROC_LOC_LOC_EST_SHAPE;
        if ( (p_prov_loc_r9->commonIEsProvideLocationInformation.locationEstimate.u.ellipsoidPointWithAltitudeAndUncertaintyEllipsoid =
                                                                                                                                        tm_lpp_asn1_osys_IE_malloc(sizeof(EllipsoidPointWithAltitudeAndUncertaintyEllipsoid))) == NULL )
        {
          MSG_ERROR("TM_LPP:Enc Prov Loc buffer alloc failure", 0, 0, 0);
          tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_LOC_INFO);
          return ( FALSE );
        }

        if ( (p_lpp_tx_type->z_lpp_data_type.z_pos_info.z_pos.u_Lat0 & 0x80) == 0x80 )
        {
          p_prov_loc_r9->commonIEsProvideLocationInformation.locationEstimate.u.ellipsoidPointWithAltitudeAndUncertaintyEllipsoid->latitudeSign = 1;
        }
        else
        {
          p_prov_loc_r9->commonIEsProvideLocationInformation.locationEstimate.u.ellipsoidPointWithAltitudeAndUncertaintyEllipsoid->latitudeSign = 0;

        }
        p_prov_loc_r9->commonIEsProvideLocationInformation.locationEstimate.u.ellipsoidPointWithAltitudeAndUncertaintyEllipsoid->degreesLatitude = p_lpp_tx_type->z_lpp_data_type.z_pos_info.q_Lat;

        p_prov_loc_r9->commonIEsProvideLocationInformation.locationEstimate.u.ellipsoidPointWithAltitudeAndUncertaintyEllipsoid->degreesLongitude = p_lpp_tx_type->z_lpp_data_type.z_pos_info.l_Long;

        if ( p_lpp_tx_type->z_lpp_data_type.z_pos_info.z_pos.u_Alt0 & 0x80 )
        {
          p_prov_loc_r9->commonIEsProvideLocationInformation.locationEstimate.u.ellipsoidPointWithAltitudeAndUncertaintyEllipsoid->altitudeDirection = 1;
        }
        else
        {
          p_prov_loc_r9->commonIEsProvideLocationInformation.locationEstimate.u.ellipsoidPointWithAltitudeAndUncertaintyEllipsoid->altitudeDirection = 0;
        }
        p_prov_loc_r9->commonIEsProvideLocationInformation.locationEstimate.u.ellipsoidPointWithAltitudeAndUncertaintyEllipsoid->altitude = p_lpp_tx_type->z_lpp_data_type.z_pos_info.w_Alt;
        MSG_MED("ALT %d %d", p_lpp_tx_type->z_lpp_data_type.z_pos_info.w_Alt,
                p_prov_loc_r9->commonIEsProvideLocationInformation.locationEstimate.u.ellipsoidPointWithAltitudeAndUncertaintyEllipsoid->altitudeDirection, 0);
        p_prov_loc_r9->commonIEsProvideLocationInformation.locationEstimate.u.ellipsoidPointWithAltitudeAndUncertaintyEllipsoid->confidence = p_lpp_tx_type->z_lpp_data_type.z_pos_info.z_pos.u_Confidence;
        p_prov_loc_r9->commonIEsProvideLocationInformation.locationEstimate.u.ellipsoidPointWithAltitudeAndUncertaintyEllipsoid->orientationMajorAxis = p_lpp_tx_type->z_lpp_data_type.z_pos_info.z_pos.u_OrientMajor;
        p_prov_loc_r9->commonIEsProvideLocationInformation.locationEstimate.u.ellipsoidPointWithAltitudeAndUncertaintyEllipsoid->uncertaintySemiMinor = p_lpp_tx_type->z_lpp_data_type.z_pos_info.z_pos.u_UncMinor;
        p_prov_loc_r9->commonIEsProvideLocationInformation.locationEstimate.u.ellipsoidPointWithAltitudeAndUncertaintyEllipsoid->uncertaintySemiMajor = p_lpp_tx_type->z_lpp_data_type.z_pos_info.z_pos.u_UncMajor;
        p_prov_loc_r9->commonIEsProvideLocationInformation.locationEstimate.u.ellipsoidPointWithAltitudeAndUncertaintyEllipsoid->uncertaintyAltitude = p_lpp_tx_type->z_lpp_data_type.z_pos_info.z_pos.u_UncAlt;
        if ( p_lpp_tx_type->z_lpp_data_type.z_pos_info.vel_valid == TRUE )
        {
          p_prov_loc_r9->commonIEsProvideLocationInformation.m.velocityEstimatePresent = 1;
          p_prov_loc_r9->commonIEsProvideLocationInformation.velocityEstimate.t = 4;
          if ( (p_prov_loc_r9->commonIEsProvideLocationInformation.velocityEstimate.u.horizontalWithVerticalVelocityAndUncertainty =
                                                                                                                                     tm_lpp_asn1_osys_IE_malloc(sizeof(HorizontalWithVerticalVelocityAndUncertainty))) == NULL )
          {
            MSG_ERROR("TM_LPP:Enc Prov Loc buffer alloc failure", 0, 0, 0);
            tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_LOC_INFO);
            return ( FALSE );
          }
          tm_lpp_convert_velocity(p_lpp_tx_type->z_lpp_data_type.z_pos_info.f_VelEnu,
                                  p_lpp_tx_type->z_lpp_data_type.z_pos_info.f_Vunc3dMps,
                                  p_lpp_tx_type->z_lpp_data_type.z_pos_info.f_velVertUnc,
                                  p_prov_loc_r9->commonIEsProvideLocationInformation.velocityEstimate.u.horizontalWithVerticalVelocityAndUncertainty);
        }
        p_prov_loc_r9->m.a_gnss_ProvideLocationInformationPresent = 1;

        p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_LocationInformation.agnss_List.gnss_ids.numbits = 16;

        if ( p_lpp_tx_type->z_lpp_data_type.z_pos_info.q_pos_sources & C_POS_SOURCE_GLO )
        {
          OSSETBIT(p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_LocationInformation.agnss_List.gnss_ids, LPP_GNSS_ID_BITMAP_GLONASS);
        }
        if ( p_lpp_tx_type->z_lpp_data_type.z_pos_info.q_pos_sources & C_POS_SOURCE_GPS )
        {
          OSSETBIT(p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_LocationInformation.agnss_List.gnss_ids, LPP_GNSS_ID_BITMAP_GPS);
        }

        /* Fix generated using only Glonass measurements ? */
        if ( (p_lpp_tx_type->z_lpp_data_type.z_pos_info.q_pos_sources & C_POS_SOURCE_GLO) &&
             !(p_lpp_tx_type->z_lpp_data_type.z_pos_info.q_pos_sources & ~C_POS_SOURCE_GLO) )  // No other pos sources should be used
        {
          /* Provide the Glonass meas ref time only if server interested */
          if ( p_lpp_tx_type->z_lpp_data_type.req_loc_gnss_method_bmap & C_POS_SOURCE_GLO )
          {
            /* gnss_TOD_msec =  actual glonass-tod-msec mod 1 hours */
            q_GnssTodMsecModHour = p_lpp_tx_type->z_lpp_data_type.z_pos_info.q_GloTodMs;
            q_GnssTodMsecModHour = q_GnssTodMsecModHour % HOUR_MSECS;
            if ( q_GnssTodMsecModHour > 3599999 )
            {
              q_GnssTodMsecModHour = 3599999;
            }

            p_prov_loc_r9->a_gnss_ProvideLocationInformation.m.gnss_LocationInformationPresent = 1;
            p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_LocationInformation.measurementReferenceTime.gnss_TimeID.gnss_id = LPP_GNSS_ID_GLONASS;

            p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_LocationInformation.measurementReferenceTime.m.gnss_TOD_fracPresent = 0; // For Glonass no TOD-frac available from PE

            p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_LocationInformation.measurementReferenceTime.gnss_TOD_msec = q_GnssTodMsecModHour;
            p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_LocationInformation.measurementReferenceTime.m.gnss_TOD_uncPresent = 1;
            p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_LocationInformation.measurementReferenceTime.gnss_TOD_unc =
                                                                                                                              tm_lpp_utils_microsec_to_GADK(p_lpp_tx_type->z_lpp_data_type.z_pos_info.f_ClockBiasUncMs * 1000);
          }
          else
          {
            MSG_MED("Glonass meas ref time (%lu) available but server not interested (0x%lX)",
                    p_lpp_tx_type->z_lpp_data_type.z_pos_info.q_GloTodMs, p_lpp_tx_type->z_lpp_data_type.req_loc_gnss_method_bmap, 0);
          }
        } /* Glonass only */
        else if ( p_lpp_tx_type->z_lpp_data_type.z_pos_info.q_pos_sources & C_POS_SOURCE_GPS )
        {
          /* Provide the GPS meas ref time only if server interested */
          if ( p_lpp_tx_type->z_lpp_data_type.req_loc_gnss_method_bmap & C_POS_SOURCE_GPS )
          {
            p_prov_loc_r9->a_gnss_ProvideLocationInformation.m.gnss_LocationInformationPresent = 1;
            p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_LocationInformation.measurementReferenceTime.gnss_TimeID.gnss_id = LPP_GNSS_ID_GPS;
            OSSETBIT(p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_LocationInformation.agnss_List.gnss_ids, LPP_GNSS_ID_BITMAP_GPS);

            /* convert GPS time to the right format for LPP */
            q_GnssTodMsecModHour = p_lpp_tx_type->z_lpp_data_type.z_pos_info.q_fix_time_gps_msec;
            f_GnssTodFracPart = -(p_lpp_tx_type->z_lpp_data_type.z_pos_info.f_ClockBias);
            if ( f_GnssTodFracPart < 0.0f )
            {
              w_GnssTodFrac250ns = (uint16)(1.0f + f_GnssTodFracPart) * 4000;
              q_GnssTodMsecModHour -= 1;
              tm_umts_common_utils_GpsMsecWeekLimit((int32 *)&q_GnssTodMsecModHour, NULL);  /* Handles roll overs */
            }
            else
            {
              w_GnssTodFrac250ns = (uint16)(f_GnssTodFracPart * 4000);
            }
            if ( w_GnssTodFrac250ns > 3999 )
            {
              w_GnssTodFrac250ns = 3999;
            }

            /* gnss_TOD_msec =  actual gps-tod-msec mod 1 hours */
            q_GnssTodMsecModHour = p_lpp_tx_type->z_lpp_data_type.z_pos_info.q_fix_time_gps_msec;
            q_GnssTodMsecModHour = q_GnssTodMsecModHour % HOUR_MSECS;
            if ( q_GnssTodMsecModHour > 3599999 )
            {
              q_GnssTodMsecModHour = 3599999;
            }

            p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_LocationInformation.measurementReferenceTime.m.gnss_TOD_fracPresent = 1;
            p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_LocationInformation.measurementReferenceTime.gnss_TOD_frac = w_GnssTodFrac250ns;

            p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_LocationInformation.measurementReferenceTime.gnss_TOD_msec = q_GnssTodMsecModHour;
            p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_LocationInformation.measurementReferenceTime.m.gnss_TOD_uncPresent = 1;
            p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_LocationInformation.measurementReferenceTime.gnss_TOD_unc =
                                                                                                                              tm_lpp_utils_microsec_to_GADK(p_lpp_tx_type->z_lpp_data_type.z_pos_info.f_ClockBiasUncMs * 1000);
          }
          else
          {
            MSG_MED("GPS meas ref time (%lu) available but server not interested (0x%lX)",
                    p_lpp_tx_type->z_lpp_data_type.z_pos_info.q_fix_time_gps_msec, p_lpp_tx_type->z_lpp_data_type.req_loc_gnss_method_bmap, 0);
          }
        } /* GPS */
        else
        {
          MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "GPS/Glonass ref time unavailable to report to server!");
        }
        p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_LocationInformation.measurementReferenceTime.m.networkTimePresent = 0;
      }
      else if(p_lpp_tx_type->z_lpp_data_type.b_earlyResponse_needed == TRUE)
      {  /*LPP_POS_DATA_VALID is FALSE, meaning no POS to send to network in earlyResponse*/
         MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "GNSS POS unavailable to report in earlyResponse!");
         return (FALSE);
      }
      else
      {
        p_prov_loc_r9->commonIEsProvideLocationInformation.m.locationErrorPresent = 1;
        p_prov_loc_r9->commonIEsProvideLocationInformation.locationError.locationfailurecause = LPP_FAILURE_POSMETHOD_FAILURE;

        if ( (p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_Error.u.targetDeviceErrorCauses =
                                                                                                      tm_lpp_asn1_osys_IE_malloc(sizeof(GNSS_TargetDeviceErrorCauses))) == NULL )
        {

          MSG_ERROR("TM_LPP:Enc Prov Loc buffer alloc failure for error causes", 0, 0, 0);
          tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_LOC_INFO);
          return ( FALSE );
        }
        // send gnss-error here
        p_prov_loc_r9->m.a_gnss_ProvideLocationInformationPresent = 1;
        p_prov_loc_r9->a_gnss_ProvideLocationInformation.m.gnss_ErrorPresent = 1;
        p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_Error.t = LPP_ERROR_TARGET_DEVICE_CAUSE;
        if ( p_lpp_tx_type->z_lpp_data_type.lpp_error.u_lpp_error_type & LPP_ERROR_TYPE_PROV_LOC_AGNSS_ERROR )
        {
          p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_Error.u.targetDeviceErrorCauses->cause =
                                                                                                         p_lpp_tx_type->z_lpp_data_type.lpp_error.u_lpp_error_value;
        }
        else
        {
          p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_Error.u.targetDeviceErrorCauses->cause = LPP_AGNSS_CAUSE_NOTENOUGHSAT;
        }
      }
    }
    if ( p_lpp_tx_type->z_lpp_data_type.q_flags & LPP_ENC_PRM_DATA )
    {
      if ( (p_lpp_tx_type->z_lpp_data_type.q_flags & LPP_PRM_DATA_VALID) )
      {
        p_prov_loc_r9->m.a_gnss_ProvideLocationInformationPresent = 1;

        /* Check measurement have been populated successfully without setting error code.
           Error set inside tm_lpp_prov_loc_gnss_meas_build() func should not be overwritten, when func return false */
        if ( (FALSE == tm_lpp_prov_loc_gnss_meas_build(p_lpp_tx_type->z_lpp_data_type.p_gnss_meas_report,
                                                       &p_prov_loc_r9->a_gnss_ProvideLocationInformation,
                                                       p_lpp_tx_type->z_lpp_data_type.b_status_check_for_prm_data,
                                                       p_lpp_tx_type->z_lpp_data_type.req_loc_gnss_method_bmap))
             &&
             (p_prov_loc_r9->a_gnss_ProvideLocationInformation.m.gnss_ErrorPresent != TRUE) )
        {
          if ( (p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_Error.u.targetDeviceErrorCauses =
                                                                                                        tm_lpp_asn1_osys_IE_malloc(sizeof(GNSS_TargetDeviceErrorCauses))) == NULL )
          {

            MSG_ERROR("TM_LPP:Enc Prov Loc buffer alloc failure for error causes", 0, 0, 0);
            tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_LOC_INFO);
            return ( FALSE );
          }
          // send gnss-error here
          p_prov_loc_r9->m.a_gnss_ProvideLocationInformationPresent = 1;
          p_prov_loc_r9->a_gnss_ProvideLocationInformation.m.gnss_ErrorPresent = 1;
          p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_Error.t = LPP_ERROR_TARGET_DEVICE_CAUSE;
          p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_Error.u.targetDeviceErrorCauses->cause = LPP_AGNSS_CAUSE_NOTENOUGHSAT;
        }
      }
      else
      {
        if ( (p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_Error.u.targetDeviceErrorCauses =
                                                                                                      tm_lpp_asn1_osys_IE_malloc(sizeof(GNSS_TargetDeviceErrorCauses))) == NULL )
        {

          MSG_ERROR("TM_LPP:Enc Prov Loc buffer alloc failure for error causes", 0, 0, 0);
          tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_LOC_INFO);
          return ( FALSE );
        }
        // send gnss-error here
        p_prov_loc_r9->m.a_gnss_ProvideLocationInformationPresent = 1;
        p_prov_loc_r9->a_gnss_ProvideLocationInformation.m.gnss_ErrorPresent = 1;
        p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_Error.t = LPP_ERROR_TARGET_DEVICE_CAUSE;
        if ( p_lpp_tx_type->z_lpp_data_type.lpp_error.u_lpp_error_type & LPP_ERROR_TYPE_PROV_LOC_AGNSS_ERROR )
        {
          p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_Error.u.targetDeviceErrorCauses->cause =
                                                                                                         p_lpp_tx_type->z_lpp_data_type.lpp_error.u_lpp_error_value;
        }
        else
        {
          p_prov_loc_r9->a_gnss_ProvideLocationInformation.gnss_Error.u.targetDeviceErrorCauses->cause = LPP_AGNSS_CAUSE_NOTENOUGHSAT;
        }
      }
    }

    if ( p_lpp_tx_type->z_lpp_data_type.q_flags & LPP_ENC_OTDOA_DATA )
    {
      boolean                                      b_OtdoaErrDirOrInDir = FALSE; /* TRUE if OTDOA error set by OTDOA engine (direct cause) or due to external reasons (in-direct cause) */
      sm_loc_lte_otdoa_meas_info_type const *const p_otoda_data_tx      = p_lpp_tx_type->z_lpp_data_type.p_otoda_data;

      /* If OTDOA data is available its valid */
      if ( p_otoda_data_tx != NULL )
      {
        /* OTDOA data with RSTD measurements and/or error is present */
        p_prov_loc_r9->m.otdoa_ProvideLocationInformationPresent = 1;

        u_cell_cnt = tm_lpp_prov_loc_otdoa_meas_build(p_otoda_data_tx->p_meas_info,
                                                      &p_prov_loc_r9->otdoa_ProvideLocationInformation);

        MSG_MED("TM_LPP:OTDOA Measurements will be sent u_cell_cnt = %d. LppErrType %u", u_cell_cnt,
                p_lpp_tx_type->z_lpp_data_type.lpp_error.u_lpp_error_type, 0);

        /* force RSTD meas. cnt to 1 or less for a hybrid fix (only for debugging) */
        if ( (u_cell_cnt > 1) && (TRUE == b_force_lte_hybrid) )
        {
          u_cell_cnt = 1;
          MSG_MED("TM_LPP: Force OTDOA meas. to 1", 0, 0, 0);
        }

        /* Send error in these cases:-
        *  1. If RSTD measurment present bit set to 1, but no cell measurements are found
        *  2. OTDOA error due to in-direct cause (Todo : This case wouldn't occur here, clean-up later)
        *  3. OTDOA engine provided with cause for OTDOA error (direct cause)
        */
        if ( (0 == u_cell_cnt) ||
             /* This case would never occur : When OTDOA data is valid, but error filled due to in-direct causes */
             (p_lpp_tx_type->z_lpp_data_type.lpp_error.u_lpp_error_type & LPP_ERROR_TYPE_PROV_LOC_OTDOA_ERROR) ||
             p_otoda_data_tx->p_meas_info->m.otdoa_ErrorPresent )
        {
          b_OtdoaErrDirOrInDir = TRUE;
        }
      }
      else
      {
        /* Send OTDOA error due to in-direct cause (i.e. error not from OTDOA engine) like
        *  1. GNSS measurement not available in combo call flow, OTDOA meas timedout,
        *     OTDOA AD not provided but OTDOA meas requested etc.. So fill appropiate error code
        *     in OTDOA IE section
        */
        b_OtdoaErrDirOrInDir = TRUE;
      }

      if ( TRUE == b_OtdoaErrDirOrInDir )
      {
        if ( (p_prov_loc_r9->otdoa_ProvideLocationInformation.otdoa_Error.u.targetDeviceErrorCauses =
                                                                                                      tm_lpp_asn1_osys_IE_malloc(sizeof(OTDOA_TargetDeviceErrorCauses))) == NULL )
        {
          MSG_ERROR("TM_LPP:Enc Prov Loc buffer alloc failure for error causes", 0, 0, 0);
          tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_LOC_INFO);
          return ( FALSE );
        }

        MSG_MED("TM_LPP:Fill OTDOA Prov loc err", 0, 0, 0);
        p_prov_loc_r9->m.otdoa_ProvideLocationInformationPresent = 1;
        p_prov_loc_r9->otdoa_ProvideLocationInformation.m.otdoa_ErrorPresent = 1;
        p_prov_loc_r9->otdoa_ProvideLocationInformation.otdoa_Error.t = LPP_ERROR_TARGET_DEVICE_CAUSE;

        /* Find if OTDOA error is due to direct (set by OTDOA engine) or in-direct cause.
        *  In case of OTDOA error set by the OTDOA engine use the error IE present in the
        *  OTDOA data struct (i.e. p_meas_info->otdoa_Error), else use the transaction error flag.
        */

        if ( (p_otoda_data_tx != NULL) && (p_otoda_data_tx->p_meas_info->m.otdoa_ErrorPresent) )
        {
          /* Error set by OTDOA engine */
          p_prov_loc_r9->otdoa_ProvideLocationInformation.otdoa_Error.u.targetDeviceErrorCauses->cause = p_otoda_data_tx->p_meas_info->otdoa_Error;

        }
        else
        {
          /* Due to indirect cause */
          if ( p_lpp_tx_type->z_lpp_data_type.lpp_error.u_lpp_error_type & LPP_ERROR_TYPE_PROV_LOC_OTDOA_ERROR )
          {
            p_prov_loc_r9->otdoa_ProvideLocationInformation.otdoa_Error.u.targetDeviceErrorCauses->cause =
                                                                                                           p_lpp_tx_type->z_lpp_data_type.lpp_error.u_lpp_error_value;

          }
          else
          {
            p_prov_loc_r9->otdoa_ProvideLocationInformation.otdoa_Error.u.targetDeviceErrorCauses->cause = LPP_OTDOA_CAUSE_UNDEF;
          }
        }
        MSG_MED("TM_LPP:Sending OTDOA PRov loc err cause %d",
                p_prov_loc_r9->otdoa_ProvideLocationInformation.otdoa_Error.u.targetDeviceErrorCauses->cause, 0, 0);
      } /* OTDOA error */
    }

    if ( p_lpp_tx_type->z_lpp_data_type.q_flags & LPP_ENC_ECID_DATA )
    {
      if ( p_lpp_tx_type->z_lpp_data_type.q_flags & LPP_ECID_DATA_VALID )
      {
        p_prov_loc_r9->m.ecid_ProvideLocationInformationPresent = 1;
        p_prov_loc_r9->ecid_ProvideLocationInformation.m.ecid_SignalMeasurementInformationPresent = 1;

        b_ret = tm_lpp_prov_loc_ecid_meas_build(p_lpp_tx_type->z_lpp_data_type.p_ecid_data,
                                                &p_prov_loc_r9->ecid_ProvideLocationInformation);
        MSG_MED("TM_LPP:ECID MEasurements build ret %d", b_ret, 0, 0);
        if ( b_ret == FALSE ||
             p_lpp_tx_type->z_lpp_data_type.lpp_error.u_lpp_error_type & LPP_ERROR_TYPE_PROV_LOC_ECID_ERROR )
        {
          MSG_MED("TM_LPP:Sending ECID PRov loc err", 0, 0, 0);
          //send otdoa-error here
          if ( (p_prov_loc_r9->ecid_ProvideLocationInformation.ecid_Error.u.targetDeviceErrorCauses =
                                                                                                      tm_lpp_asn1_osys_IE_malloc(sizeof(ECID_TargetDeviceErrorCauses))) == NULL )
          {

            MSG_ERROR("TM_LPP:Enc Prov Loc buffer alloc failure for error causes", 0, 0, 0);
            tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_LOC_INFO);
            return ( FALSE );
          }
          p_prov_loc_r9->m.ecid_ProvideLocationInformationPresent = 1;
          p_prov_loc_r9->ecid_ProvideLocationInformation.m.ecid_ErrorPresent = 1;
          p_prov_loc_r9->ecid_ProvideLocationInformation.ecid_Error.t = LPP_ERROR_TARGET_DEVICE_CAUSE;
          if ( p_lpp_tx_type->z_lpp_data_type.lpp_error.u_lpp_error_type & LPP_ERROR_TYPE_PROV_LOC_ECID_ERROR )
          {
            p_prov_loc_r9->ecid_ProvideLocationInformation.ecid_Error.u.targetDeviceErrorCauses->cause =
                                                                                                         p_lpp_tx_type->z_lpp_data_type.lpp_error.u_lpp_error_value;
            if ( p_lpp_tx_type->z_lpp_data_type.p_ecid_data &&
                 p_lpp_tx_type->z_lpp_data_type.p_ecid_data->m.gnss_lpp_ecid_ErrorPresent )
            {
              p_prov_loc_r9->ecid_ProvideLocationInformation.ecid_Error.u.targetDeviceErrorCauses->m.rsrpMeasurementNotPossiblePresent = p_lpp_tx_type->z_lpp_data_type.p_ecid_data->ecid_Error.targetDeviceErrorCauses.m.rsrpMeasurementNotPossiblePresent;
              p_prov_loc_r9->ecid_ProvideLocationInformation.ecid_Error.u.targetDeviceErrorCauses->m.rsrqMeasurementNotPossiblePresent = p_lpp_tx_type->z_lpp_data_type.p_ecid_data->ecid_Error.targetDeviceErrorCauses.m.rsrqMeasurementNotPossiblePresent;
              p_prov_loc_r9->ecid_ProvideLocationInformation.ecid_Error.u.targetDeviceErrorCauses->m.ueRxTxMeasurementNotPossiblePresent = p_lpp_tx_type->z_lpp_data_type.p_ecid_data->ecid_Error.targetDeviceErrorCauses.m.ueRxTxMeasurementNotPossiblePresent;
            }
          }
          else
          {
            p_prov_loc_r9->ecid_ProvideLocationInformation.ecid_Error.u.targetDeviceErrorCauses->cause = LPP_ECID_CAUSE_UNDEF;
          }
          if ( b_ret == FALSE )
          {
            p_prov_loc_r9->ecid_ProvideLocationInformation.m.ecid_SignalMeasurementInformationPresent = 0;
          }
          MSG_MED("TM_LPP:Sending ECID PRov loc err cause %d",
                  p_prov_loc_r9->ecid_ProvideLocationInformation.ecid_Error.u.targetDeviceErrorCauses->cause, 0, 0);
        }
      }
      else
      {
        //send ecid-error here
        if ( (p_prov_loc_r9->ecid_ProvideLocationInformation.ecid_Error.u.targetDeviceErrorCauses =
                                                                                                    tm_lpp_asn1_osys_IE_malloc(sizeof(ECID_TargetDeviceErrorCauses))) == NULL )
        {

          MSG_ERROR("TM_LPP:Enc Prov Loc buffer alloc failure for error causes", 0, 0, 0);
          tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_LOC_INFO);
          return ( FALSE );
        }
        p_prov_loc_r9->m.ecid_ProvideLocationInformationPresent = 1;
        p_prov_loc_r9->ecid_ProvideLocationInformation.m.ecid_ErrorPresent = 1;
        p_prov_loc_r9->ecid_ProvideLocationInformation.ecid_Error.t = LPP_ERROR_TARGET_DEVICE_CAUSE;
        if ( p_lpp_tx_type->z_lpp_data_type.lpp_error.u_lpp_error_type & LPP_ERROR_TYPE_PROV_LOC_ECID_ERROR )
        {
          p_prov_loc_r9->ecid_ProvideLocationInformation.ecid_Error.u.targetDeviceErrorCauses->cause =
                                                                                                       p_lpp_tx_type->z_lpp_data_type.lpp_error.u_lpp_error_value;
          if ( p_lpp_tx_type->z_lpp_data_type.p_ecid_data &&
               p_lpp_tx_type->z_lpp_data_type.p_ecid_data->m.gnss_lpp_ecid_ErrorPresent )
          {
            p_prov_loc_r9->ecid_ProvideLocationInformation.ecid_Error.u.targetDeviceErrorCauses->m.rsrpMeasurementNotPossiblePresent = p_lpp_tx_type->z_lpp_data_type.p_ecid_data->ecid_Error.targetDeviceErrorCauses.m.rsrpMeasurementNotPossiblePresent;
            p_prov_loc_r9->ecid_ProvideLocationInformation.ecid_Error.u.targetDeviceErrorCauses->m.rsrqMeasurementNotPossiblePresent = p_lpp_tx_type->z_lpp_data_type.p_ecid_data->ecid_Error.targetDeviceErrorCauses.m.rsrqMeasurementNotPossiblePresent;
            p_prov_loc_r9->ecid_ProvideLocationInformation.ecid_Error.u.targetDeviceErrorCauses->m.ueRxTxMeasurementNotPossiblePresent = p_lpp_tx_type->z_lpp_data_type.p_ecid_data->ecid_Error.targetDeviceErrorCauses.m.ueRxTxMeasurementNotPossiblePresent;
          }
        }
        else
        {
          p_prov_loc_r9->ecid_ProvideLocationInformation.ecid_Error.u.targetDeviceErrorCauses->cause =
                                                                                                       LPP_ECID_CAUSE_UNDEF;
        }
        if ( u_cell_cnt == 0 )
        {
          p_prov_loc_r9->ecid_ProvideLocationInformation.m.ecid_SignalMeasurementInformationPresent = 0;
        }
        MSG_MED("TM_LPP:Sending ECID PRov loc err cause %d",
                p_prov_loc_r9->ecid_ProvideLocationInformation.ecid_Error.u.targetDeviceErrorCauses->cause, 0, 0);

      }
    }

    /* If there are no measurements & location estimate send an error. */
    if (
         /* In hybrid call flow when no GNSS + OTDOA measurement */
         ((p_prov_loc_r9->otdoa_ProvideLocationInformation.m.otdoaSignalMeasurementInformationPresent == 0) &&
          (p_prov_loc_r9->otdoa_ProvideLocationInformation.m.otdoa_ErrorPresent == 1) &&
          (p_prov_loc_r9->a_gnss_ProvideLocationInformation.m.gnss_ErrorPresent == 1) &&
          (p_prov_loc_r9->a_gnss_ProvideLocationInformation.m.gnss_SignalMeasurementInformationPresent == 0)) ||

         /* In OTDOA only call flow when OTDOA data is invalid */
         ((p_prov_loc_r9->m.a_gnss_ProvideLocationInformationPresent == 0) &&
          (p_prov_loc_r9->m.otdoa_ProvideLocationInformationPresent == 1) &&
          (p_prov_loc_r9->otdoa_ProvideLocationInformation.m.otdoa_ErrorPresent == 1) &&
          (p_prov_loc_r9->otdoa_ProvideLocationInformation.m.otdoaSignalMeasurementInformationPresent == 0))
        )
    {
      p_prov_loc_r9->m.commonIEsProvideLocationInformationPresent = 1;
      p_prov_loc_r9->commonIEsProvideLocationInformation.m.locationErrorPresent = 1;
      p_prov_loc_r9->commonIEsProvideLocationInformation.locationError.locationfailurecause = LPP_FAILURE_POSMETHOD_FAILURE;
    }
  }
  /* Do the ASN.1 encoding for LPP */
  memset(&p_lpp_tx_type->z_lpp_data_type.z_pdu_buf, 0, sizeof(p_lpp_tx_type->z_lpp_data_type.z_pdu_buf));  /* crash 3 */
  encode_status = tm_lpp_osys_asn1_encode_pdu((void *)p_lpp_pdu, &p_lpp_tx_type->z_lpp_data_type.z_pdu_buf);

  if ( (encode_status != 0) || (p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value == NULL) || (p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.length == 0) )
  {
    /* ASN.1 encoding failed */
    MSG_ERROR("TM_LPP: Error encoding LPP ProvLocInfo", 0, 0, 0);

    if ( p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value != NULL )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "LPP ASN1 enc. err. Freeing: %d,%lx,%u",
            encode_status,
            p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value,
            p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.length);

      tm_lpp_asn1_free_osys_buf(p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value);
    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "LPP ASN1 enc. err. No alloc.: %d,%lx,%u",
            encode_status,
            p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value,
            p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.length);
    }
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_LOC_INFO);
    return ( FALSE );
  }

  tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_PROV_LOC_INFO);
  return ( TRUE );

}



/*=========================================================================
  tm_lpp_encode_error
  
  Description:
    LPP function to encode Provide Location Information message
    

  Returns:
    TRUE if the encoding passed, FALSE otherwise    
==========================================================================*/

boolean tm_lpp_encode_error(
                             tm_lpp_transaction_info_s_type *p_lpp_tx_type
                            )
{
  LPP_Message                  *p_lpp_pdu;
  int                          encode_status;
  tm_lpp_transaction_id_s_type z_trans_id;

  MSG_MED("TM_LPP:Enc ERROR", 0, 0, 0);
  if ( p_lpp_tx_type == NULL )
  {
    MSG_ERROR("TM_LPP: Null transcation pointer", 0, 0, 0);
    return ( FALSE );

  }

  p_lpp_pdu =
              tm_lpp_asn1_osys_IE_malloc(sizeof(LPP_Message));

  if ( p_lpp_pdu == NULL )
  {

    MSG_ERROR("TM_LPP:Enc error buffer alloc failure", 0, 0, 0);
    return ( FALSE );
  }
  p_lpp_tx_type->z_lpp_data_type.p_pdu_ptr = p_lpp_pdu;

  p_lpp_tx_type->b_end_sent = TRUE;
  z_trans_id = p_lpp_tx_type->z_current_trans_id;

  p_lpp_pdu->m.transactionIDPresent = 0;
  if ( p_lpp_tx_type->z_current_trans_id.m.u_trans_id_valid )
  {
    p_lpp_pdu->m.transactionIDPresent = 1;
    p_lpp_pdu->transactionID.initiator = (uint8)z_trans_id.u_initiator;
    p_lpp_pdu->transactionID.transactionNumber = (uint8)z_trans_id.u_transaction_id;
  }
  p_lpp_pdu->endTransaction = TRUE;
  /* LPP CP Specific Changes : BEGIN */
  if ( p_lpp_tx_type->u_lpp_session_type == TM_LPP_SESSION_TYPE_CP )
  {
    p_lpp_pdu->m.sequenceNumberPresent = 1;
    tm_lpp_cp_get_nxt_out_seq_no(p_lpp_tx_type);
    p_lpp_pdu->sequenceNumber = p_lpp_tx_type->u_out_sequence_num;

    p_lpp_pdu->m.acknowledgementPresent = 1;
    p_lpp_pdu->acknowledgement.ackRequested = 1;           /* Request Server ACK to LPP Prov Cap           */
    p_lpp_pdu->acknowledgement.m.ackIndicatorPresent = 0;  /* Make sure clear unused field                 */
  }
  /* LPP CP Specific Changes : END */

  p_lpp_pdu->m.lpp_MessageBodyPresent = 1;

  p_lpp_pdu->lpp_MessageBody.t = LPP_MESSAGEBODY_C1;
  if ( (p_lpp_pdu->lpp_MessageBody.u.c1 = tm_lpp_asn1_osys_IE_malloc(sizeof(LPP_MessageBody_c1))) == NULL )
  {
    MSG_ERROR("TM_LPP:Enc error buffer alloc failure", 0, 0, 0);
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_ERROR);
    return ( FALSE );
  }

  p_lpp_pdu->lpp_MessageBody.u.c1->t = LPP_MESSAGE_ERROR;

  if ( (p_lpp_pdu->lpp_MessageBody.u.c1->u.error = tm_lpp_asn1_osys_IE_malloc(sizeof(Error))) == NULL )
  {
    MSG_ERROR("TM_LPP:Enc error buffer alloc failure", 0, 0, 0);
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_ERROR);
    return ( FALSE );
  }
  p_lpp_pdu->lpp_MessageBody.u.c1->u.error->t = LPP_ERROR_R9;
  if ( (p_lpp_pdu->lpp_MessageBody.u.c1->u.error->u.error_r9 =
                                                               tm_lpp_asn1_osys_IE_malloc(sizeof(Error_r9_IEs))) == NULL )
  {
    MSG_ERROR("TM_LPP:Enc error buffer alloc failure", 0, 0, 0);
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_ERROR);
    return ( FALSE );
  }

  p_lpp_pdu->lpp_MessageBody.u.c1->u.error->u.error_r9->m.commonIEsErrorPresent = 1;

  if ( p_lpp_tx_type->z_lpp_data_type.q_flags & LPP_ERROR_DATA_VALID &&
       p_lpp_tx_type->z_lpp_data_type.lpp_error.u_lpp_error_type & LPP_ERROR_TYPE_LPP_ERROR )
  {
    p_lpp_pdu->lpp_MessageBody.u.c1->u.error->u.error_r9->commonIEsError.errorCause = p_lpp_tx_type->z_lpp_data_type.lpp_error.u_lpp_error_value;
  }
  else
  {
    p_lpp_pdu->lpp_MessageBody.u.c1->u.error->u.error_r9->commonIEsError.errorCause = LPP_ERROR_UNDEF;
  }

  /* Do the ASN.1 encoding for LPP */
  memset(&p_lpp_tx_type->z_lpp_data_type.z_pdu_buf, 0, sizeof(p_lpp_tx_type->z_lpp_data_type.z_pdu_buf));  /* crash 3 */
  encode_status = tm_lpp_osys_asn1_encode_pdu((void *)p_lpp_pdu, &p_lpp_tx_type->z_lpp_data_type.z_pdu_buf);

  if ( (encode_status != 0) || (p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value == NULL) || (p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.length == 0) )
  {
    /* ASN.1 encoding failed */
    MSG_ERROR("TM_LPP: Error encoding LPP Error", 0, 0, 0);

    if ( p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value != NULL )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "LPP ASN1 enc. err. Freeing: %d,%lx,%u",
            encode_status,
            p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value,
            p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.length);

      tm_lpp_asn1_free_osys_buf(p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value);
    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "LPP ASN1 enc. err. No alloc.: %d,%lx,%u",
            encode_status,
            p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value,
            p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.length);
    }
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_ERROR);
    return ( FALSE );
  }

  tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_ERROR);
  return ( TRUE );

}

/*=========================================================================
  tm_lpp_encode_abort_msg
  
  Description:
    LPP function to encode LPP ABORT 
    

  Returns:
    TRUE if the encoding passed, FALSE otherwise    
==========================================================================*/

boolean tm_lpp_encode_abort_msg(
                                 tm_lpp_transaction_info_s_type *p_lpp_tx_type
                                )
{
  LPP_Message                  *p_lpp_pdu;
  int                          encode_status;
  tm_lpp_transaction_id_s_type z_trans_id;

  MSG_MED("TM_LPP:Enc ABORT", 0, 0, 0);
  if ( p_lpp_tx_type == NULL )
  {
    MSG_ERROR("TM_LPP: Null transcation pointer", 0, 0, 0);
    return ( FALSE );

  }

  // tm_lpp_init_encoding_context();
  p_lpp_pdu =
              tm_lpp_asn1_osys_IE_malloc(sizeof(LPP_Message));

  if ( p_lpp_pdu == NULL )
  {

    MSG_ERROR("TM_LPP:Enc Abort buffer alloc failure", 0, 0, 0);
    return ( FALSE );
  }

  p_lpp_tx_type->z_lpp_data_type.p_pdu_ptr = p_lpp_pdu;
  z_trans_id = p_lpp_tx_type->z_current_trans_id;
  p_lpp_tx_type->b_end_sent = TRUE;

  p_lpp_pdu->m.transactionIDPresent = 0;
  if ( p_lpp_tx_type->z_current_trans_id.m.u_trans_id_valid )
  {
    p_lpp_pdu->m.transactionIDPresent = 1;
    p_lpp_pdu->transactionID.initiator = (uint8)z_trans_id.u_initiator;
    p_lpp_pdu->transactionID.transactionNumber = (uint8)z_trans_id.u_transaction_id;
  }
  p_lpp_pdu->endTransaction = TRUE;
  /* LPP CP Specific Changes : BEGIN */
  if ( p_lpp_tx_type->u_lpp_session_type == TM_LPP_SESSION_TYPE_CP )
  {
    p_lpp_pdu->m.sequenceNumberPresent = 1;
    tm_lpp_cp_get_nxt_out_seq_no(p_lpp_tx_type);
    p_lpp_pdu->sequenceNumber = p_lpp_tx_type->u_out_sequence_num;

    p_lpp_pdu->m.acknowledgementPresent = 1;
    p_lpp_pdu->acknowledgement.ackRequested = 1;           /* Request Server ACK to LPP Prov Cap           */
    p_lpp_pdu->acknowledgement.m.ackIndicatorPresent = 0;  /* Make sure clear unused field                 */
  }
  /* LPP CP Specific Changes : END */

  p_lpp_pdu->m.lpp_MessageBodyPresent = 1;

  p_lpp_pdu->lpp_MessageBody.t = LPP_MESSAGEBODY_C1;
  if ( (p_lpp_pdu->lpp_MessageBody.u.c1 = tm_lpp_asn1_osys_IE_malloc(sizeof(LPP_MessageBody_c1))) == NULL )
  {
    MSG_ERROR("TM_LPP:Enc Abort buffer alloc failure", 0, 0, 0);
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_ABORT);
    return ( FALSE );
  }

  p_lpp_pdu->lpp_MessageBody.u.c1->t = LPP_MESSAGE_ABORT;

  if ( (p_lpp_pdu->lpp_MessageBody.u.c1->u.abort_ = tm_lpp_asn1_osys_IE_malloc(sizeof(Abort))) == NULL )
  {
    MSG_ERROR("TM_LPP:Enc Abort alloc failure", 0, 0, 0);
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_ABORT);
    return ( FALSE );
  }
  p_lpp_pdu->lpp_MessageBody.u.c1->u.abort_->criticalExtensions.t = 1;
  if ( (p_lpp_pdu->lpp_MessageBody.u.c1->u.abort_->criticalExtensions.u.c1 =
                                                                             tm_lpp_asn1_osys_IE_malloc(sizeof(Abort_criticalExtensions_c1))) == NULL )
  {
    MSG_ERROR("TM_LPP:Enc Abort buffer alloc failure", 0, 0, 0);
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_ABORT);
    return ( FALSE );
  }

  p_lpp_pdu->lpp_MessageBody.u.c1->u.abort_->criticalExtensions.u.c1->t = 1;

  if ( (p_lpp_pdu->lpp_MessageBody.u.c1->u.abort_->criticalExtensions.u.c1->u.abort_r9 =
                                                                                         tm_lpp_asn1_osys_IE_malloc(sizeof(Abort_r9_IEs))) == NULL )
  {
    MSG_ERROR("TM_LPP:Enc Abort buffer alloc failure", 0, 0, 0);
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_ABORT);
    return ( FALSE );
  }

  p_lpp_pdu->lpp_MessageBody.u.c1->u.abort_->criticalExtensions.u.c1->u.abort_r9->m.commonIEsAbortPresent = 1;

  if ( p_lpp_tx_type->z_lpp_data_type.q_flags & LPP_ERROR_DATA_VALID &&
       p_lpp_tx_type->z_lpp_data_type.lpp_error.u_lpp_error_type & LPP_ERROR_TYPE_LPP_ABORT )
  {

    p_lpp_pdu->lpp_MessageBody.u.c1->u.abort_->criticalExtensions.u.c1->u.abort_r9->commonIEsAbort.abortCause = p_lpp_tx_type->z_lpp_data_type.lpp_error.u_lpp_error_value;
  }
  else
  {
    p_lpp_pdu->lpp_MessageBody.u.c1->u.abort_->criticalExtensions.u.c1->u.abort_r9->commonIEsAbort.abortCause = LPP_ABORT_TARGET;
  }

  /* Do the ASN.1 encoding for LPP */
  memset(&p_lpp_tx_type->z_lpp_data_type.z_pdu_buf, 0, sizeof(p_lpp_tx_type->z_lpp_data_type.z_pdu_buf));  /* crash 3 */
  encode_status = tm_lpp_osys_asn1_encode_pdu((void *)p_lpp_pdu, &p_lpp_tx_type->z_lpp_data_type.z_pdu_buf);
  if ( (encode_status != 0) || (p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value == NULL) || (p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.length == 0) )
  {
    /* ASN.1 encoding failed */
    MSG_ERROR("TM_LPP: Error encoding LPP Abort", 0, 0, 0);

    if ( p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value != NULL )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "LPP ASN1 enc. err. Freeing: %d,%lx,%u",
            encode_status,
            p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value,
            p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.length);

      tm_lpp_asn1_free_osys_buf(p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value);
    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "LPP ASN1 enc. err. No alloc.: %d,%lx,%u",
            encode_status,
            p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.value,
            p_lpp_tx_type->z_lpp_data_type.z_pdu_buf.length);
    }
    tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_ABORT);
    return ( FALSE );
  }

  tm_asn1_free_lpp_message_IEs(p_lpp_pdu, LPP_MESSAGE_ABORT);
  return ( TRUE );

}

/*=========================================================================
  tm_lpp_encode_ack
  
  Description:
    LPP function to encode Explicit LPP ACK message.
 
  Parameters:
    u_seq_num -  sequence number to ACK
    p_pdu_buf - ASN.1 encoded LPP_Message
    end_flag  -  is the transaction being acked ends the transaction ?
    
  Returns:
    TRUE if the encoding passed, FALSE otherwise    
==========================================================================*/
boolean tm_lpp_encode_ack( uint8 u_seq_number, tm_pdu_buf_type  *p_pdu_buf, boolean end_flag )
{
  LPP_Message z_lpp_pdu;
  int         encode_status;

  MSG_MED("<<<< [UE]: UE ACK [SEQ=%d]", u_seq_number, 0, 0);

  memset((void *)&z_lpp_pdu, 0, sizeof(LPP_Message));

  /* For ACK , no need to include a transaction ID */

  z_lpp_pdu.m.acknowledgementPresent               = 1;
  z_lpp_pdu.acknowledgement.m.ackIndicatorPresent  = 1;
  z_lpp_pdu.acknowledgement.ackIndicator           = u_seq_number;

  /* The endTransaction indicator must be included in an Explicit Ack message as follows:
     If the message being ack’ed ends the transaction
     (i.e. carries an endTransaction with value TRUE), then include TRUE in the Ack
     Else include FALSE in the Ack.
  */

  z_lpp_pdu.endTransaction = end_flag;
  /* Do the ASN.1 encoding for LPP ACK Message */
  /* crash 3 BEGIN */
  if ( p_pdu_buf == NULL )
  {
    MSG_ERROR("TM_LPP: null encode buffer found", 0, 0, 0);
    return ( FALSE );
  }
  else
  {
    memset(p_pdu_buf, 0, sizeof(*p_pdu_buf));
  }
  /* crash 3 END */
  encode_status = tm_lpp_osys_asn1_encode_pdu((void *)&z_lpp_pdu, p_pdu_buf);

  if ( (encode_status != 0) || (p_pdu_buf->value == NULL) || (p_pdu_buf->length == 0) )
  {
    /* ASN.1 encoding failed */
    MSG_ERROR("TM_LPP: Error encoding LPP ACK", 0, 0, 0);
    if ( p_pdu_buf->value != NULL )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "LPP ASN1 enc. err. Freeing: %d,%lx,%u",
            encode_status,
            p_pdu_buf->value,
            p_pdu_buf->length);
      tm_lpp_asn1_free_osys_buf(p_pdu_buf->value);
    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "LPP ASN1 enc. err. No alloc.: %d,%lx,%u",
            encode_status,
            p_pdu_buf->value,
            p_pdu_buf->length);
    }
    return ( FALSE );
  }
  return ( TRUE );
} /* tm_lpp_encode_ack */


/*==========================================================================
  tm_lpp_decode_message                                 
                                 
  Description:                     
  LPP function to process incoming LPP message
  
  Returns                               
    TRUE if the decoding passed, FALSE otherwise    
===========================================================================*/
void*  tm_lpp_decode_message( const tm_pdu_buf_type *p_payload,
                              /**< encoded data */
                              int *decode_status_ptr
                             )
{

  LPP_Message *p_lpp_pdu;

  MSG_MED("TM_LPP:Decode Message ", 0, 0, 0);
  p_lpp_pdu =
              tm_lpp_asn1_osys_IE_malloc(sizeof(LPP_Message));

  if ( p_lpp_pdu == NULL )
  {
    MSG_ERROR("TM_LPP: Cannot allocate OSYS memory for the LPP data", 0, 0, 0);
    return ( NULL );

  }
  tm_lpp_osys_asn1_decode_pdu(p_payload->value, (uint16)p_payload->length, (void *)p_lpp_pdu, decode_status_ptr);

  return ( (void *)p_lpp_pdu );
}


/*===========================================================================
  tm_lpp_encode_prov_cap                               
                                 
  Description:                               
    LPP function to encode Provide Capability message
                                 
  Parameters:                               
    *p_client - pointer to the client                            
    *p_lpp_tx_type - pointer to the transaction strucutre                             
                                 
  Return Value:                                 
    TRUE if the encoding passed, FALSE otherwise    
=============================================================================*/
boolean tm_lpp_encode_prov_cap( const tm_lpp_tx_mgr_client_type *p_client,
                                tm_lpp_transaction_info_s_type *p_lpp_tx_type
                               )
{
  boolean b_ret = FALSE;

  MSG_MED("TM_LPP:Encode Prov Cap Message ", 0, 0, 0);

  if ( p_client->client_cb_func )
  {
    b_ret = p_client->client_cb_func(TM_LPP_GET_CLIENT_CAP, p_lpp_tx_type);

    if ( b_ret == FALSE )
    {
      return ( b_ret );
    }

    if ( p_lpp_tx_type->z_lpp_data_type.p_lpp_prov_cap_type )
    {
      b_ret = tm_lpp_encode_provide_cap(p_lpp_tx_type);
      if ( b_ret == TRUE )
      {
        if ( p_lpp_tx_type->u_lpp_session_type == TM_LPP_SESSION_TYPE_CP )
        {
          if ( (p_lpp_tx_type->z_lpp_data_type.p_pdu_ptr) &&
               (p_lpp_tx_type->z_lpp_data_type.p_pdu_ptr->m.acknowledgementPresent == 1) &&
               (p_lpp_tx_type->z_lpp_data_type.p_pdu_ptr->acknowledgement.ackRequested == TRUE) )
          {
            p_lpp_tx_type->u_wait_ack = 0x01;  /* bit 0 - Wait server ACK */
            /* Decide whether the call flow is MSA or MSB 
               depending upon what is being received next from the
               EMLC.
            */
          }
          p_lpp_tx_type->b_end_sent = TRUE;
        }
        else
        {
          p_lpp_tx_type->e_next_messages_id = TM_LPP_MESG_REQ_AD;
        }
        p_lpp_tx_type->b_pending_ue_tx = TRUE; /* transaction to be sent out in SUPL POS/SUPLPOSINIT */
        p_lpp_tx_type->b_pending_server_tx = FALSE; /* transaction to be sent out in SUPL POS/SUPLPOSINIT */
        p_lpp_tx_type->b_end_sent = TRUE;
      } /* b_ret == TRUE */
    }
  } /* client_cb_func */
  return ( b_ret );
} /* tm_lpp_encode_prov_cap */



/*===========================================================================
   tm_lpp_encode_req_ad                               
                                 
  Description:                               
    LPP function to encode Request Assistance message
                                 
  Parameters:                               
    *p_client - pointer to the client                            
    *p_lpp_tx_type - pointer to the transaction strucutre                             
                                 
  Return Value:                                 
    TRUE if the encoding passed, FALSE otherwise    
=============================================================================*/
boolean tm_lpp_encode_req_ad( const tm_lpp_tx_mgr_client_type *p_client,
                              tm_lpp_transaction_info_s_type *p_lpp_tx_type )
{
  boolean b_ret = TRUE;

  MSG_MED("TM_LPP:Encode Req AD Message ", 0, 0, 0);
  if ( p_client->client_cb_func )
  {
    b_ret = p_client->client_cb_func(TM_LPP_GET_CLIENT_AD_LIST, p_lpp_tx_type);
  }

  if ( b_ret == TRUE )
  {
    if ( p_lpp_tx_type->z_lpp_data_type.p_AssistData )
    {
      b_ret = tm_lpp_encode_request_assistance(p_lpp_tx_type);

      if ( b_ret == TRUE )
      {
        if ( p_lpp_tx_type->u_lpp_session_type == TM_LPP_SESSION_TYPE_CP )
        {
          if ( (p_lpp_tx_type->z_lpp_data_type.p_pdu_ptr) &&
               (p_lpp_tx_type->z_lpp_data_type.p_pdu_ptr->m.acknowledgementPresent == 1) &&
               (p_lpp_tx_type->z_lpp_data_type.p_pdu_ptr->acknowledgement.ackRequested == TRUE) )
          {
            p_lpp_tx_type->u_wait_ack = 0x01;  /* bit 0 - Wait server ACK */
          }
        }
        p_lpp_tx_type->e_next_messages_id = TM_LPP_MESG_PROV_AD;
        p_lpp_tx_type->b_end_sent = FALSE;
        p_lpp_tx_type->b_pending_ue_tx = TRUE; /* to be sent out in SUPL POS/SUPLPOSINIT */
        p_lpp_tx_type->b_pending_server_tx = TRUE; /* to be sent out in SUPL POS/SUPLPOSINIT */
      }
    }
  }
  return ( b_ret );

}


/*===========================================================================
  tm_lpp_encode_prov_loc_info                               
                                 
  Description:                               
    LPP function to encode Provide Location message
                                 
  Parameters:                               
    *p_client - pointer to the client                            
    *p_lpp_tx_type - pointer to the transaction strucutre                             
                                 
  Return Value:                                 
    TRUE if the encoding passed, FALSE otherwise    
=============================================================================*/
boolean tm_lpp_encode_prov_loc_info( const tm_lpp_tx_mgr_client_type *p_client,
                                     tm_lpp_transaction_info_s_type *p_lpp_tx_type )
{
  boolean b_ret = TRUE;

  MSG_MED("TM_LPP:Encode Prov LocInfo Message ", 0, 0, 0);
  if ( p_lpp_tx_type->b_new_tx == TRUE && (TRUE == p_lpp_tx_type->z_lpp_data_type.b_earlyResponse_needed))
  {
    p_lpp_tx_type->e_messages_id = TM_LPP_MESG_PROV_LOC_INFO;
  }
  if ( p_client->client_cb_func )
  {
    b_ret = p_client->client_cb_func(TM_LPP_PROV_LOC_INFO, p_lpp_tx_type);
  }

  if ( b_ret == TRUE )
  {
    b_ret = tm_lpp_encode_provide_location(p_lpp_tx_type);
    if ( b_ret == TRUE )
    {
      if ( p_lpp_tx_type->u_lpp_session_type == TM_LPP_SESSION_TYPE_CP )
      {
        if ( (p_lpp_tx_type->z_lpp_data_type.p_pdu_ptr) &&
             (p_lpp_tx_type->z_lpp_data_type.p_pdu_ptr->m.acknowledgementPresent == 1) &&
             (p_lpp_tx_type->z_lpp_data_type.p_pdu_ptr->acknowledgement.ackRequested == TRUE) )
        {
          p_lpp_tx_type->u_wait_ack = 0x01;  /* bit 0 - Wait server ACK */
        }
        p_lpp_tx_type->e_next_messages_id =  TM_LPP_MESG_NONE;
      }
      p_lpp_tx_type->b_pending_server_tx = FALSE; /* to be sent out in SUPL POS/SUPLPOSINIT */
      p_lpp_tx_type->b_pending_ue_tx = TRUE; /* to be sent out in SUPL POS/SUPLPOSINIT */
    }
  }

  MSG_MED("<<<< [UE]: Prov Loc [UE SEQ=%d] [WaitAck=%d] [Qflag=0x%x]",
          p_lpp_tx_type->u_out_sequence_num,
          p_lpp_tx_type->u_wait_ack,
          p_lpp_tx_type->z_lpp_data_type.q_flags
         );

  return ( b_ret );

} /* tm_lpp_encode_prov_loc_info */


/*===========================================================================
  tm_lpp_encode_err                              
                                 
  Description:                               
    LPP function to encode ERROR message
                                 
  Parameters:                               
    *p_client - pointer to the client                            
    *p_lpp_tx_type - pointer to the transaction strucutre                             
                                 
  Return Value:                                 
    TRUE if the encoding passed, FALSE otherwise    
=============================================================================*/
boolean tm_lpp_encode_err( const tm_lpp_tx_mgr_client_type *p_client,
                           tm_lpp_transaction_info_s_type *p_lpp_tx_type )
{
  boolean b_ret = TRUE;

  MSG_MED("TM_LPP:Encode LPP ERROR ", 0, 0, 0);
  if ( p_client->client_cb_func )
  {
    b_ret = p_client->client_cb_func(TM_LPP_GET_ERROR_INFO, p_lpp_tx_type);
  }

  if ( b_ret == TRUE )
  {
    {
      b_ret = tm_lpp_encode_error(p_lpp_tx_type);

      if ( b_ret == TRUE )
      {
        if ( p_lpp_tx_type->u_lpp_session_type == TM_LPP_SESSION_TYPE_CP )
        {
          if ( (p_lpp_tx_type->z_lpp_data_type.p_pdu_ptr) &&
               (p_lpp_tx_type->z_lpp_data_type.p_pdu_ptr->m.acknowledgementPresent == 1) &&
               (p_lpp_tx_type->z_lpp_data_type.p_pdu_ptr->acknowledgement.ackRequested == TRUE) )
          {
            p_lpp_tx_type->u_wait_ack = 0x01;  /* bit 0 - Wait server ACK */
          }
        }
        p_lpp_tx_type->e_next_messages_id = TM_LPP_MESG_NONE;
        p_lpp_tx_type->b_end_sent = TRUE;
        p_lpp_tx_type->b_pending_ue_tx = TRUE; /* to be sent out in SUPL POS/SUPLPOSINIT */
        p_lpp_tx_type->b_pending_server_tx = FALSE; /* to be sent out in SUPL POS/SUPLPOSINIT */
      }
    }
  }
  return ( b_ret );

}

/*===========================================================================
  tm_lpp_encode_abort                               
                                 
  Description:                               
    LPP function to encode ABORT message
                                 
  Parameters:                               
    *p_client - pointer to the client                            
    *p_lpp_tx_type - pointer to the transaction strucutre                             
                                 
  Return Value:                                 
    TRUE if the encoding passed, FALSE otherwise    
=============================================================================*/
boolean tm_lpp_encode_abort( const tm_lpp_tx_mgr_client_type *p_client,
                             tm_lpp_transaction_info_s_type *p_lpp_tx_type )
{
  boolean b_ret = TRUE;

  MSG_MED("TM_LPP:Encode LPP ABORT ", 0, 0, 0);
  if ( p_client->client_cb_func )
  {
    b_ret = p_client->client_cb_func(TM_LPP_GET_ABORT_INFO, p_lpp_tx_type);
  }

  if ( b_ret == TRUE )
  {
    {
      b_ret = tm_lpp_encode_abort_msg(p_lpp_tx_type);

      if ( b_ret == TRUE )
      {

        if ( p_lpp_tx_type->u_lpp_session_type == TM_LPP_SESSION_TYPE_CP )
        {

          if ( (p_lpp_tx_type->z_lpp_data_type.p_pdu_ptr) &&
               (p_lpp_tx_type->z_lpp_data_type.p_pdu_ptr->m.acknowledgementPresent == 1) &&
               (p_lpp_tx_type->z_lpp_data_type.p_pdu_ptr->acknowledgement.ackRequested == TRUE) )
          {
            p_lpp_tx_type->u_wait_ack = 0x01;  /* bit 0 - Wait server ACK */
          }
        }
        p_lpp_tx_type->e_next_messages_id = TM_LPP_MESG_NONE;
        p_lpp_tx_type->b_end_sent = TRUE;
        p_lpp_tx_type->b_pending_ue_tx = TRUE; /* to be sent out in SUPL POS/SUPLPOSINIT */
        p_lpp_tx_type->b_pending_server_tx = FALSE; /* to be sent out in SUPL POS/SUPLPOSINIT */
      }
    }
  }
  return ( b_ret );

}


/*===========================================================================
  tm_lpp_handle_provide_ad
 
  Description:
     LPP function to handle decoded Assitance Data message
 
  Parameters:
    *p_client - client type
    *p_lpp_tx_type - pointer to the transaction structure
 
 
  Returns
    TRUE if the decoding passed, FALSE otherwise    
============================================================================*/
boolean tm_lpp_handle_prov_ad( const tm_lpp_tx_mgr_client_type *p_client,
                               tm_lpp_transaction_info_s_type *p_lpp_tx_type )
{
  boolean b_ret = TRUE;

  MSG_MED(">>>> TM_LPP:Handle Prov Ad Message ", 0, 0, 0);
  if ( p_lpp_tx_type->b_new_tx == TRUE )
  {
    p_lpp_tx_type->e_messages_id = TM_LPP_MESG_PROV_AD;
  }
  else if ( p_lpp_tx_type->e_messages_id == TM_LPP_MESG_REQ_AD &&
            p_lpp_tx_type->b_pending_ue_tx == TRUE )
  {
    p_lpp_tx_type->b_pending_ue_tx = FALSE;
  }
  if ( p_client->client_cb_func )
  {
    b_ret = p_client->client_cb_func(TM_LPP_PROV_CLIENT_AD, p_lpp_tx_type);
  }
  if ( p_lpp_tx_type->b_end_sent == TRUE )
  {
    p_lpp_tx_type->b_pending_server_tx = FALSE;
  }
  return ( b_ret );
}


/*===========================================================================
  tm_lpp_handle_req_loc_info
 
  Description:
     LPP function to handle decoded Request Location Information message
 
  Parameters:
    *p_client - client type
    *p_lpp_tx_type - pointer to the transaction structure
 
 
  Returns
    TRUE if the decoding passed, FALSE otherwise    
============================================================================*/
boolean tm_lpp_handle_req_loc_info( const tm_lpp_tx_mgr_client_type *p_client,
                                    tm_lpp_transaction_info_s_type *p_lpp_tx_type )
{


  MSG_MED(">>>> TM_LPP:Handle Req LocInfo Message ", 0, 0, 0);
  if ( p_lpp_tx_type->b_new_tx == TRUE )
  {
    p_lpp_tx_type->e_messages_id = TM_LPP_MESG_REQ_LOC_INFO;
  }

  p_lpp_tx_type->b_pending_ue_tx = TRUE; /* response needs to be sent to the network */
  p_lpp_tx_type->b_pending_server_tx = FALSE; /* response needs to be sent to the network */
  p_lpp_tx_type->e_next_messages_id = TM_LPP_MESG_PROV_LOC_INFO;
  if ( p_client->client_cb_func )
  {
    return ( p_client->client_cb_func(TM_LPP_REQ_CLIENT_LOC_INFO, p_lpp_tx_type) );
  }
  return ( FALSE );

}



/*===========================================================================
  tm_lpp_handle_req_cap
 
  Description:
     LPP function to handle decoded Request Capabilities message
 
  Parameters:
    *p_client - client type
    *p_lpp_tx_type - pointer to the transaction structure
 
 
  Returns
    TRUE if the decoding passed, FALSE otherwise    
============================================================================*/
boolean tm_lpp_handle_req_cap( const tm_lpp_tx_mgr_client_type *p_client,
                               tm_lpp_transaction_info_s_type *p_lpp_tx_type )
{


  MSG_MED(">>>> TM_LPP:Handle Req Cap Message ", 0, 0, 0);
  if ( p_lpp_tx_type->b_new_tx == TRUE )
  {
    p_lpp_tx_type->e_messages_id = TM_LPP_MESG_REQ_CAP;
  }

  p_lpp_tx_type->b_pending_ue_tx = TRUE; /* response needs to be sent to the network */
  p_lpp_tx_type->b_pending_server_tx = FALSE; /* response needs to be sent to the network */

  p_lpp_tx_type->e_next_messages_id = TM_LPP_MESG_PROV_CAP;
  if ( p_client->client_cb_func )
  {
    return ( p_client->client_cb_func(TM_LPP_REQ_CLIENT_CAP, p_lpp_tx_type) );
  }
  return ( FALSE );

}



/*===========================================================================
  tm_lpp_handle_error
 
  Description:
     LPP function to handle decoded ERROR message
 
  Parameters:
    *p_client - client type
    *p_lpp_tx_type - pointer to the transaction structure
 
 
  Returns
    TRUE if the decoding passed, FALSE otherwise    
============================================================================*/
boolean tm_lpp_handle_error( const tm_lpp_tx_mgr_client_type *p_client,
                             tm_lpp_transaction_info_s_type *p_lpp_tx_type )

{


  MSG_MED(">>>> TM_LPP:Handle Error Message ", 0, 0, 0);
  if ( p_lpp_tx_type->b_new_tx == TRUE )
  {
    p_lpp_tx_type->e_messages_id = TM_LPP_MESG_ERROR;
  }

  p_lpp_tx_type->b_pending_ue_tx = FALSE; /* response needs to be sent to the network */
  p_lpp_tx_type->b_pending_server_tx = FALSE; /* response needs to be sent to the network */

  p_lpp_tx_type->e_next_messages_id = TM_LPP_MESG_NONE;
  if ( p_client->client_cb_func )
  {
    return ( p_client->client_cb_func(TM_LPP_HANDLE_ERROR, p_lpp_tx_type) );
  }
  return ( FALSE );

}



/*===========================================================================
  tm_lpp_handle_abort
 
  Description:
     LPP function to handle decoded ABORT message
 
  Parameters:
    *p_client - client type
    *p_lpp_tx_type - pointer to the transaction structure
 
 
  Returns
    TRUE if the decoding passed, FALSE otherwise    
============================================================================*/
boolean tm_lpp_handle_abort( const tm_lpp_tx_mgr_client_type *p_client,
                             tm_lpp_transaction_info_s_type *p_lpp_tx_type )
{
  MSG_MED(">>>> TM_LPP:Handle Abort Message ", 0, 0, 0);
  if ( p_lpp_tx_type->b_new_tx == TRUE )
  {
    p_lpp_tx_type->e_messages_id = TM_LPP_MESG_ABORT;
  }

  p_lpp_tx_type->b_pending_ue_tx = FALSE; /* response needs to be sent to the network */
  p_lpp_tx_type->b_pending_server_tx = FALSE; /* response needs to be sent to the network */

  p_lpp_tx_type->e_next_messages_id = TM_LPP_MESG_NONE;
  if ( p_client->client_cb_func )
  {
    return ( p_client->client_cb_func(TM_LPP_HANDLE_ABORT, p_lpp_tx_type) );
  }
  return ( FALSE );
}



/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded LPP Glonass reference time IE to the format ready
 *  for TM core post data message. Finally return the prepared tmcore post data
 *  for the caller to post to TMCore.
 *
 * Parameters:
 *
 *  p_ref_time: pointer to the structure containing ASN.1 decoded LPP message.
 *
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data 
 *                          structure.
 *
 * Return value:
 *
 *    None.
 *    
 *
 ******************************************************************************
*/

boolean tm_lpp_glo_reftime_to_tmcore_post_data_payload( GNSS_ReferenceTime *p_ref_time,
                                                        tm_post_data_payload_type *p_tm_post_data_payload )
{

  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Prepare Glo RefTime (%u) for TM post", p_ref_time->gnss_SystemTime.gnss_TimeID.gnss_id);

  if ( p_ref_time->gnss_SystemTime.gnss_TimeID.gnss_id == (uint8)LPP_GNSS_ID_GLONASS )
  {
    p_tm_post_data_payload->reftime_glo_data.u_GloFourYear = (p_ref_time->gnss_SystemTime.gnss_DayNumber / FOURYEAR_DAYS + 1);

    p_tm_post_data_payload->reftime_glo_data.w_GloDays = (uint16)(p_ref_time->gnss_SystemTime.gnss_DayNumber - (p_tm_post_data_payload->reftime_glo_data.u_GloFourYear - 1) * FOURYEAR_DAYS + 1);

    p_tm_post_data_payload->reftime_glo_data.q_GloMsec = (p_ref_time->gnss_SystemTime.gnss_TimeOfDay) * 1000 % (HOUR_SECS * 1000);

    /* TOD unc. is of no use to MGP as we don't apply the association of GANSS time to GPS time */
    // p_tm_post_data_payload->reftime_glo_data.f_GloTimeUncMs;

    /* TM doesn't need these gnss_TimeOfDayFrac_msec, notificationOfLeapSecond parameters for now*/
    /* No TOW for Glonass - gps_TOW_Assist */

    if ( (z_umts_nv_item_ram_copy.aagps_default_ref_time_unc_50ms * 50) >= 1000 )
    {
      p_tm_post_data_payload->reftime_glo_data.f_GloTimeUncMs =
                                                                (float)z_umts_nv_item_ram_copy.aagps_default_ref_time_unc_50ms * 50.f;
    }
    else
    {
      p_tm_post_data_payload->reftime_glo_data.f_GloTimeUncMs = 1000.f;
    }

    return ( TRUE );
  }

  return ( FALSE );
}


/*
 ******************************************************************************
 *
 * tm_lpp_glo_eph_to_tmcore_post_data_payload
 *
 * Function description:
 *  Buffers Glonass navigation model in a buffer that associates its SV ID with
 *  the corresponding frequency number                  
 *  
 * Parameters:
 *   p_ganss_sat_element : pointer to the structure holding Glonass nav. model.
 *
 * Return value: 
 *   None.
 *
 ******************************************************************************
*/
void tm_lpp_glo_eph_to_tmcore_post_data_payload( GNSS_NavModelSatelliteElement *p_NavModelEph,
                                                 glo_EphStructType *p_TmPostDataGloEph )
{
  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Prepare Glo Eph (%u) for TM post", p_NavModelEph->svID.satellite_id);

  /* TM API uses 65-88 for Glonass SVs, LPP decoded SV ID ranges from 0-63 though Glonass uses only 24 SVs. */
  if ( p_NavModelEph->svID.satellite_id < N_GLO_SV )
  {
    p_TmPostDataGloEph->z_Mask.z_FilledMask.u_GloMFlag = 0;
    p_TmPostDataGloEph->z_Mask.z_FilledMask.u_LnFlag = 0;
    p_TmPostDataGloEph->z_Mask.z_FilledMask.u_NtFlag = 0;
    p_TmPostDataGloEph->z_Mask.z_FilledMask.u_P4Flag = 0;
    p_TmPostDataGloEph->z_Mask.z_FilledMask.u_TimeSrcPFlag = 0;

    p_TmPostDataGloEph->u_SvId = p_NavModelEph->svID.satellite_id + GLO_SLOTID_OFFSET + 1; /* start at 65 */
    p_TmPostDataGloEph->z_Mask.u_FilledMask = 0;

    /* GLONASS eph reference time tb: 7-bit LSBs*/
    // Issue of Data(iod) Bit String(11) - Bit 5 to Bit 11 (LSB) is used
    p_TmPostDataGloEph->u_Tb = tm_lpp_osys_asn1_bitstring_to_int(sizeof(p_TmPostDataGloEph->u_Tb), &p_NavModelEph->iod);

    /* Only MSB 5 bits are usable; from API: Health flag Bn: 1-bit LSBs */
    if ( p_NavModelEph->svHealth.numbits > 0 )
    {
      /* Bn is Bit 1 (MSB) of the 8-bit OSS_BITSTRING svHealth.*/
      p_TmPostDataGloEph->u_Bn = (p_NavModelEph->svHealth.data[0] >> 7) & 0x1;

      /* 4 bits (Bit 2 (MSB) to 5) in svHealth are Ft;
       API: Ft is a parameter shown the URE at time tb: 4-bit LSBs; */
      p_TmPostDataGloEph->u_Ft = (p_NavModelEph->svHealth.data[0] >> 3) & 0xF;
      p_TmPostDataGloEph->z_Mask.z_FilledMask.u_FtFlag = TRUE;
    }

    // GLONASS-ClockModel
    if ( T_GNSS_ClockModel_glonass_ClockModel == p_NavModelEph->gnss_ClockModel.t )
    {
      /* gloGamma - relative frequency offset from nominal value, INTEGER (-1024..1023).
         LPP ICD def. scale factor 2^-40, LPP decodes into 2's complement,
         MGP API: w_FreqOff using ICD format of sign+magnitude */
      p_TmPostDataGloEph->w_FreqOff = tm_umts_common_utils_2s_comp_to_sign_mag(
                                                                               p_NavModelEph->gnss_ClockModel.u.glonass_ClockModel->gloGamma, 0x400);

      /* gloTau - satellite clock offset (seconds) INTEGER (-2097152..2097151).
         Scale factor 2^-30, LPP decodes into 2's complement, 
         MGP API: q_ClkCorr using ICD format of sign+magnitude */
      p_TmPostDataGloEph->q_ClkCorr = tm_umts_common_utils_2s_comp_to_sign_mag(
                                                                               p_NavModelEph->gnss_ClockModel.u.glonass_ClockModel->gloTau, 0x200000);

      /* gloDeltaTau - Time diff between transmission in G2 & G1 in seconds.
        (IDC defines as 5 bits, scale factor 2^-30); MGP API: u_L1L2; */
      if ( p_NavModelEph->gnss_ClockModel.u.glonass_ClockModel->m.gloDeltaTauPresent )
      {
        p_TmPostDataGloEph->u_L1L2 = tm_umts_common_utils_2s_comp_to_sign_mag(p_NavModelEph->gnss_ClockModel.u.glonass_ClockModel->gloDeltaTau, 0x10);
      }
      else
      {
        /* temp. solution until gnss_common.h is updated with added FilledMask element to support this */
        p_TmPostDataGloEph->u_L1L2 = 0;
      }
    }

    if ( T_GNSS_OrbitModel_glonass_ECEF == p_NavModelEph->gnss_OrbitModel.t )
    {
      /* M: 5 bits. Integer (0...31) */
      p_TmPostDataGloEph->u_En =
                                 p_NavModelEph->gnss_OrbitModel.u.glonass_ECEF->gloEn;

      /* P1: 2 bits. Bit string size 2 */
      if ( p_NavModelEph->gnss_OrbitModel.u.glonass_ECEF->gloP1.numbits > 0 )
      {
        p_TmPostDataGloEph->u_P1 = p_NavModelEph->gnss_OrbitModel.u.glonass_ECEF->gloP1.data[0];
        p_TmPostDataGloEph->u_P1 >>= 6;
        /* P2: boolean */
        p_TmPostDataGloEph->u_P2 = p_NavModelEph->gnss_OrbitModel.u.glonass_ECEF->gloP2;
      }

      /* M: Integer (0..3) */
      p_TmPostDataGloEph->u_GloM = p_NavModelEph->gnss_OrbitModel.u.glonass_ECEF->gloM;
      p_TmPostDataGloEph->z_Mask.z_FilledMask.u_GloMFlag = 1;       // Set u_GloM available

      /* X: Integer (-6710864..67108863);
        API expects ICD format 27 bits: sign magnitude */
      p_TmPostDataGloEph->q_SatPos[0] = tm_umts_common_utils_2s_comp_to_sign_mag(
                                                                                 p_NavModelEph->gnss_OrbitModel.u.glonass_ECEF->gloX,
                                                                                 0x04000000);

      /* X-dot: Integer ( -8388608..6388607)
   API expects ICD format 24 bits: sign-magnitude */
      p_TmPostDataGloEph->q_SatVel[0] =
                                        tm_umts_common_utils_2s_comp_to_sign_mag(
                                                                                 p_NavModelEph->gnss_OrbitModel.u.glonass_ECEF->gloXdot,
                                                                                 0x00800000);

      /* X-dot-dot: Integer ( -16..15 )
         API expects ICD format 5 bits: sign-magnitude */
      p_TmPostDataGloEph->u_Acc[0] =
                                     tm_umts_common_utils_2s_comp_to_sign_mag(
                                                                              p_NavModelEph->gnss_OrbitModel.u.glonass_ECEF->gloXdotdot,
                                                                              0x00000010);

      /* Y: Integer (-6710864..67108863)
         API expects ICD format 24 bits: sign-magnitude */
      p_TmPostDataGloEph->q_SatPos[1] =
                                        tm_umts_common_utils_2s_comp_to_sign_mag(
                                                                                 p_NavModelEph->gnss_OrbitModel.u.glonass_ECEF->gloY,
                                                                                 0x04000000);

      /* Y-dot: Integer ( -8388608..6388607)
         API expects ICD format 24 bits: sign-magnitude */
      p_TmPostDataGloEph->q_SatVel[1] =
                                        tm_umts_common_utils_2s_comp_to_sign_mag(
                                                                                 p_NavModelEph->gnss_OrbitModel.u.glonass_ECEF->gloYdot,
                                                                                 0x00800000);

      /* Y-dot-dot: Integer ( -16..15 )
         API expects ICD format 5 bits: sign-magnitude */
      p_TmPostDataGloEph->u_Acc[1] =
                                     tm_umts_common_utils_2s_comp_to_sign_mag(
                                                                              p_NavModelEph->gnss_OrbitModel.u.glonass_ECEF->gloXdotdot,
                                                                              0x00000010);

      /* Z: Integer (-6710864..67108863)
         API expects ICD format 24 bits: sign-magnitude */
      p_TmPostDataGloEph->q_SatPos[2] =
                                        tm_umts_common_utils_2s_comp_to_sign_mag(
                                                                                 p_NavModelEph->gnss_OrbitModel.u.glonass_ECEF->gloZ,
                                                                                 0x04000000);

      /* Z-dot: Integer ( -8388608..6388607)
         API expects ICD format 24 bits: sign-magnitude */
      p_TmPostDataGloEph->q_SatVel[2] =
                                        tm_umts_common_utils_2s_comp_to_sign_mag(
                                                                                 p_NavModelEph->gnss_OrbitModel.u.glonass_ECEF->gloZdot,
                                                                                 0x00800000);

      /* Z-dot-dot: Integer ( -16..15 )
         API expects ICD format 5 bits: sign-magnitude */
      p_TmPostDataGloEph->u_Acc[2] =
                                     tm_umts_common_utils_2s_comp_to_sign_mag(
                                                                              p_NavModelEph->gnss_OrbitModel.u.glonass_ECEF->gloZdotdot,
                                                                              0x00000010);

    } /* T_GNSS_OrbitModel_glonass_ECEF */

  }

}


/*******************************************************************************
 *  p_Almanac: pointer to the structure containing the per SV based ASN.1 
 *             decoded LPP almanac data.
 *
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data 
 *                          structure.
 *
 * Return value:
 *
 *    None.
 *    
 *
 ******************************************************************************
*/

void tm_lpp_glo_alm_to_tmcore_post_data_payload( const GNSS_AlmanacElement *p_Almanac,
                                                 glo_AlmStructType *p_TmPostDataGloAlmanac
                                                )
{
  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "Prepare Glo Alm (%d) for TM post", p_Almanac->t);

  if ( T_GNSS_AlmanacElement_keplerianGLONASS == p_Almanac->t )
  {
    /* Number of days in a 4-year cycle; (1..1461) as per tech spec*/
    p_TmPostDataGloAlmanac->w_Na = p_Almanac->u.keplerianGLONASS->gloAlm_NA;

    /* n_A is SV_ID : TM API uses 65-88 for Glonass SVs, so decoded SV ID ranges ASN1 INTEGER (1..24) as per tech spec */
    p_TmPostDataGloAlmanac->u_SvId = p_Almanac->u.keplerianGLONASS->gloAlmnA + GLO_SLOTID_OFFSET;

    /* Delat_T_A: (-2097152..2097151); API expects IDC format of 22-bit sign mag.  */
    p_TmPostDataGloAlmanac->q_DracCorrect = tm_umts_common_utils_2s_comp_to_sign_mag(
                                                                                     p_Almanac->u.keplerianGLONASS->gloAlmDeltaTA, 0x00200000);

    /* Delta_i_A: (-131072..131071); API expects IDC format of 18-bit sign mag. */
    p_TmPostDataGloAlmanac->q_InclCorrect = tm_umts_common_utils_2s_comp_to_sign_mag(
                                                                                     p_Almanac->u.keplerianGLONASS->gloAlmDeltaIa, 0x00020000);

    /* Lamda_nA: (-1048576..1048575); API expects IDC format of 21-bit sign mag.  */
    p_TmPostDataGloAlmanac->q_LongAscend = tm_umts_common_utils_2s_comp_to_sign_mag(
                                                                                    p_Almanac->u.keplerianGLONASS->gloAlmLambdaA, 0x00100000);

    /* t_Lamda_n_A (0..2097151) */
    p_TmPostDataGloAlmanac->q_TimeFirstAscend =  p_Almanac->u.keplerianGLONASS->gloAlmtlambdaA;

    /* Delta_TDot_n_A: (-64..63); API expects IDC format of 7-bit sign mag. */
    p_TmPostDataGloAlmanac->u_DractRate = tm_umts_common_utils_2s_comp_to_sign_mag(
                                                                                   p_Almanac->u.keplerianGLONASS->gloAlmDeltaTdotA, 0x00000040);

    /* HnA : Carrier frequency number (0..31) */
    p_TmPostDataGloAlmanac->u_FreqNum = p_Almanac->u.keplerianGLONASS->gloAlmHA;

    /* 1 bit, CnA (0..1)  */
    p_TmPostDataGloAlmanac->u_Health = p_Almanac->u.keplerianGLONASS->gloAlmCA;

    /* Number of 4-year cycle since 1996/1/1, not available from for alm. */
    p_TmPostDataGloAlmanac->z_Mask.z_FilledMask.u_N4Flag = FALSE;

    /* Epsilon_n_A (0..32767) */
    p_TmPostDataGloAlmanac->w_OrbitEcc = p_Almanac->u.keplerianGLONASS->gloAlmEpsilonA;

    /* Omega_n_A: (-32768..32767); API expects IDC format of 16-bit sign mag. */
    p_TmPostDataGloAlmanac->w_Perigee = tm_umts_common_utils_2s_comp_to_sign_mag(
                                                                                 p_Almanac->u.keplerianGLONASS->gloAlmOmegaA, 0x00008000);

    /* TauN: (-512..511); API expects IDC format of 10-bit sign mag.    */
    p_TmPostDataGloAlmanac->w_TauN = tm_umts_common_utils_2s_comp_to_sign_mag(p_Almanac->u.keplerianGLONASS->gloAlmTauA, 0x00000200);

    /* Mn: OSS_BIT string of 2 bits */
    if ( p_Almanac->u.keplerianGLONASS->m.gloAlmMAPresent == 1 )
    {
      p_TmPostDataGloAlmanac->z_Mask.z_FilledMask.u_SvTypeFlag = TRUE;

      p_TmPostDataGloAlmanac->u_SvType = *(p_Almanac->u.keplerianGLONASS->gloAlmMA.data);
      p_TmPostDataGloAlmanac->u_SvType >>= 6;
    }
  }
}



/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded LPP reference position IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_ref_loc: pointer to the structure containing ASN.1 decoded LPP  
 *          reference location data.
 *
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data 
 *                          structure.
 *
 * Return value:
 *
 *    None.
 *    
 *
 ******************************************************************************
*/
boolean tm_lpp_refpos_to_TmCore_post_data_payload( const GNSS_ReferenceLocation *p_ref_loc,
                                                   tm_post_data_payload_type      *p_tm_post_data_payload
                                                  )
{
  gps_RefLocStructType *p_tm_post_data_payload_seed_data = &p_tm_post_data_payload->seed_data;

  uint32               q_Word;
  sint31               l_Word;

  uint8                u_unc_major,
                       u_unc_minor,
                       u_orientation,
                       u_alt_unc,
                       u_confidence;
  float                f_scale_up_to_99pct_conf;

  /** check to make sure that one, and only one source ( GPS vs. GANSS) is true */
  if  ( p_ref_loc == NULL )
  {
    MSG_ERROR("TM_LPP: Err: ref pos. source info. invalid", 0, 0, 0);
    return ( FALSE );
  }

  MSG_MED("TM_LPP:RefPos to tmcore post", 0, 0, 0);
  p_tm_post_data_payload_seed_data->u_AltitudeIsValid = TRUE;
  p_tm_post_data_payload_seed_data->u_HeadingIsValid = FALSE;
  p_tm_post_data_payload_seed_data->u_VelocityIsValid = FALSE;

  /*************** LATITUDE **************************************/
  if ( p_ref_loc->threeDlocation.latitudeSign == (uint8)LPP_LAT_SIGN_NORTH )
  {
    p_tm_post_data_payload_seed_data->d_Latitude = (double)(p_ref_loc->threeDlocation.degreesLatitude)
                                                   * C_GAD_LAT_TO_RAD;
  }
  else /* north */
  {
    p_tm_post_data_payload_seed_data->d_Latitude = -(double)(p_ref_loc->threeDlocation.degreesLatitude)
                                                   * C_GAD_LAT_TO_RAD;
  }

  /*************** LONGITUDE **************************************/

  /*** Already in 2's complement */
  q_Word = (uint32)p_ref_loc->threeDlocation.degreesLongitude;
  l_Word = tm_umts_common_utils_S32Convert(q_Word, 24);

  /* convert longitude from GAD to rad. */
  p_tm_post_data_payload_seed_data->d_Longitude = (double)(l_Word * C_GAD_LON_TO_RAD);


  /******************* ALTIDUDE **********************************************/

  /* NOT 2's compliment, but: 16 bits with 15-bit unsigned Values representing 
         altitude in meters. Bit15: MSB = 0: height; MSB = 1: depth; */

  if ( p_ref_loc->threeDlocation.altitudeDirection == (uint8)LPP_ALT_DEPTH )
  {
    p_tm_post_data_payload_seed_data->f_Altitude = -(float)(p_ref_loc->threeDlocation.altitude);
  }
  else
  {
    p_tm_post_data_payload_seed_data->f_Altitude = (float)(p_ref_loc->threeDlocation.altitude);
  }


  /******************* UNCERTAINTY  **************************************/

  u_unc_major = (uint8)p_ref_loc->threeDlocation.uncertaintySemiMajor;
  p_tm_post_data_payload_seed_data->f_LocUncrtntySemiMajor = tm_umts_common_utils_GADK_to_meter(TRUE, u_unc_major);

  u_unc_minor = (uint8)p_ref_loc->threeDlocation.uncertaintySemiMinor;
  u_unc_minor &= 0x7F;
  p_tm_post_data_payload_seed_data->f_LocUncrtntySemiMinor = tm_umts_common_utils_GADK_to_meter(TRUE, u_unc_minor);

  u_orientation = (uint8)p_ref_loc->threeDlocation.orientationMajorAxis;
  p_tm_post_data_payload_seed_data->f_LocUncrtntyAng = u_orientation;
  u_alt_unc = (uint8)p_ref_loc->threeDlocation.uncertaintyAltitude;
  p_tm_post_data_payload_seed_data->f_AltitudeUnc = tm_umts_common_utils_GADK_to_meter(FALSE, u_alt_unc);

  u_confidence = (uint8)p_ref_loc->threeDlocation.confidence;

  /* Scale up the circle size based on the injected % confidence.
         For example, the PDM default % confidence is 39% (standard ellipse),
         and the AI needs a 99% confidence value, so in that case the following
         logic scales the size up radius by ~3 times. */
  /* "  confidence" is uint32 In percentage (0..100) ref. rrclsmif.h */

  f_scale_up_to_99pct_conf =
                             tm_util_get_cep_scale_ratio_from_pct((float)u_confidence * 0.01f, 0.99f);

  p_tm_post_data_payload_seed_data->f_LocUncrtntySemiMajor *=
                                                              f_scale_up_to_99pct_conf;

  p_tm_post_data_payload_seed_data->f_LocUncrtntySemiMinor *=
                                                              f_scale_up_to_99pct_conf;

  return ( TRUE );

}



/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded LPP IONO IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_iono: pointer to the structure containing ASN.1 decoded LPP  
 *          IONO data.
 *
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data 
 *                          structure.
 *
 * Return value:
 *
 *    None.
 *    
 *
 ******************************************************************************
*/
void tm_lpp_Iono_to_TmCore_post_data_payload( const  GNSS_IonosphericModel *p_Iono,
                                              tm_post_data_payload_type   *p_tm_post_data_payload )
{

  gps_IonoStructType *p_tm_post_data_payload_iono = &p_tm_post_data_payload->iono_data;

  MSG_MED("TM_LPP:Iono to tmcore post", 0, 0, 0);
  if ( p_Iono->m.klobucharModelPresent == 1 )
  {
    if ( p_Iono->klobucharModel.dataID.data[0] == 0 )
    {
      p_tm_post_data_payload_iono->u_Alpha0 = (uint8)p_Iono->klobucharModel.alfa0;
      p_tm_post_data_payload_iono->u_Alpha1 = (uint8)p_Iono->klobucharModel.alfa1;
      p_tm_post_data_payload_iono->u_Alpha2 = (uint8)p_Iono->klobucharModel.alfa2;
      p_tm_post_data_payload_iono->u_Alpha3 = (uint8)p_Iono->klobucharModel.alfa3;
      p_tm_post_data_payload_iono->u_Beta0 = (uint8)p_Iono->klobucharModel.beta0;
      p_tm_post_data_payload_iono->u_Beta1 = (uint8)p_Iono->klobucharModel.beta1;
      p_tm_post_data_payload_iono->u_Beta2 = (uint8)p_Iono->klobucharModel.beta2;
      p_tm_post_data_payload_iono->u_Beta3 = (uint8)p_Iono->klobucharModel.beta3;
    }
  }

}

/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded LPP navigation model IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_Ephem: pointer to the structure containing the per SV based ASN.1 
 *            decoded LPP navigation model data.
 *
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data 
 *                          structure.
 *
 * Return value:
 *
 *    None.
 *    
 *
 ******************************************************************************
*/

void tm_lpp_eph_to_TmCore_post_data_payload( const GNSS_NavModelSatelliteElement *p_Ephem,
                                             tm_post_data_payload_type      *p_tm_post_data_payload,
                                             uint16 w_week
                                            )
{
  gps_EphStructType        *p_tm_post_data_payload_eph_data = &p_tm_post_data_payload->eph_data;
  NavModelNAV_KeplerianSet *p_orbit_model;
  NAV_ClockModel           *p_clock_model;

  MSG_MED("TM_LPP:Eph to tmcore post", 0, 0, 0);
  p_tm_post_data_payload_eph_data->u_Sv = p_Ephem->svID.satellite_id + 1;
  p_tm_post_data_payload_eph_data->w_Iodc = p_Ephem->iod.data[0];
  p_tm_post_data_payload_eph_data->w_Iodc |= p_Ephem->iod.data[1] << 8;
  p_tm_post_data_payload_eph_data->u_Health = p_Ephem->svHealth.data[0];

  p_orbit_model = p_Ephem->gnss_OrbitModel.u.nav_KeplerianSet;
  if ( p_orbit_model == NULL  )
  {
    MSG_ERROR("TM_LPP: No orbit model\n", 0, 0, 0);
  }
  else
  {
    p_tm_post_data_payload_eph_data->u_Accuracy = (uint8)p_orbit_model->navURA;
    p_tm_post_data_payload_eph_data->w_Cic = (uint16)p_orbit_model->navCic;
    p_tm_post_data_payload_eph_data->w_Cis = (uint16)p_orbit_model->navCis;
    p_tm_post_data_payload_eph_data->w_Crc = (uint16)p_orbit_model->navCrc;
    p_tm_post_data_payload_eph_data->w_Crs = (uint16)p_orbit_model->navCrs;
    p_tm_post_data_payload_eph_data->w_Cuc = (uint16)p_orbit_model->navCuc;
    p_tm_post_data_payload_eph_data->w_Cus = (uint16)p_orbit_model->navCus;
    p_tm_post_data_payload_eph_data->w_DeltaN = (uint16)p_orbit_model->navDeltaN;
    p_tm_post_data_payload_eph_data->q_SqrtA = p_orbit_model->navAPowerHalf;
    p_tm_post_data_payload_eph_data->q_E = p_orbit_model->navE;
    p_tm_post_data_payload_eph_data->u_FitIntervalIsValid = TRUE;
    p_tm_post_data_payload_eph_data->u_FitIntervalFlag = p_orbit_model->navFitFlag;
    p_tm_post_data_payload_eph_data->q_I0 = (uint32)p_orbit_model->navI0;
    p_tm_post_data_payload_eph_data->w_Idot = (uint16)p_orbit_model->navIDot;
    p_tm_post_data_payload_eph_data->q_M0 = (uint32)p_orbit_model->navM0;
    p_tm_post_data_payload_eph_data->q_Omega = (uint32)p_orbit_model->navOmega;
    p_tm_post_data_payload_eph_data->q_Omega0 = (uint32)p_orbit_model->navOmegaA0;
    p_tm_post_data_payload_eph_data->q_OmegaDot = (uint32)p_orbit_model->navOmegaADot;
    p_tm_post_data_payload_eph_data->w_Toe = (uint16)p_orbit_model->navToe;
    if ( p_orbit_model->m.addNAVparamPresent == 1 )
    {
      p_tm_post_data_payload_eph_data->u_AodoIsValid = TRUE;
      p_tm_post_data_payload_eph_data->u_Aodo = p_orbit_model->addNAVparam.ephemAODA;
      p_tm_post_data_payload_eph_data->u_CodeL2 = p_orbit_model->addNAVparam.ephemCodeOnL2;
      p_tm_post_data_payload_eph_data->u_PcodeFlagIsValid = TRUE;
      p_tm_post_data_payload_eph_data->u_PcodeFlag = p_orbit_model->addNAVparam.ephemL2Pflag;
      p_tm_post_data_payload_eph_data->u_Sf1BitsAreValid = FALSE; /* QQQ: change to TRUE when resolved the QQQ below */
      p_tm_post_data_payload_eph_data->q_Sf1ResrvBits[0] = p_orbit_model->addNAVparam.ephemSF1Rsvd.reserved1;
      p_tm_post_data_payload_eph_data->q_Sf1ResrvBits[1] = p_orbit_model->addNAVparam.ephemSF1Rsvd.reserved2;
      p_tm_post_data_payload_eph_data->q_Sf1ResrvBits[2] = p_orbit_model->addNAVparam.ephemSF1Rsvd.reserved3;
    }
  }

  p_clock_model = p_Ephem->gnss_ClockModel.u.nav_ClockModel;

  if ( p_clock_model == NULL )
  {
    MSG_ERROR("TM_LPP: No orbit model\n", 0, 0, 0);
  }
  else
  {
    p_tm_post_data_payload_eph_data->u_TgdIsValid = TRUE;
    p_tm_post_data_payload_eph_data->q_Af0 = (uint32)p_clock_model->navaf0;
    p_tm_post_data_payload_eph_data->w_Af1 = (uint16)p_clock_model->navaf1;
    p_tm_post_data_payload_eph_data->u_Af2 = (uint8)p_clock_model->navaf2;
    p_tm_post_data_payload_eph_data->u_Tgd = (uint8)p_clock_model->navTgd;
    p_tm_post_data_payload_eph_data->w_Toc = p_clock_model->navToc;
  }

  p_tm_post_data_payload_eph_data->u_Iode = (uint8)p_tm_post_data_payload_eph_data->w_Iodc;


  /* QQQ: we seem to need the week number for now */
  if ( w_week != 0 )
  {
    p_tm_post_data_payload_eph_data->u_GpsWeekIsValid = TRUE;
    p_tm_post_data_payload_eph_data->w_GpsWeek = w_week;
  }
  else
  {
    p_tm_post_data_payload_eph_data->u_GpsWeekIsValid = FALSE;
  }
}

/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded LPP almanac IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_Almanac: pointer to the structure containing the per SV based ASN.1 
 *             decoded LPP almanac data.
 *
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data 
 *                          structure.
 *
 * Return value:
 *
 *    None.
 *    
 *
 ******************************************************************************
*/

void tm_lpp_alm_to_TmCore_post_data_payload( const GNSS_AlmanacElement         *p_Almanac,
                                             tm_post_data_payload_type   *p_tm_post_data_payload
                                            )
{
  gps_AlmStructType       *p_tm_post_data_payload_alm_data = &p_tm_post_data_payload->alm_data;
  AlmanacNAV_KeplerianSet *p_Alm_Kep;

  MSG_MED("TM_LPP:Alm to tmcore post", 0, 0, 0);
  if ( p_Almanac->t == LPP_ALAMANC_KEPL_NAV )
  {
    p_Alm_Kep =  p_Almanac->u.keplerianNAV_Almanac;
    p_tm_post_data_payload_alm_data->u_Sv = (uint8)p_Alm_Kep->svID.satellite_id + 1; /* 6-bit unsigned */
    p_tm_post_data_payload_alm_data->w_Af0  = (uint16)p_Alm_Kep->navAlmaf0;
    p_tm_post_data_payload_alm_data->w_Af1 = (uint16)p_Alm_Kep->navAlmaf1;
    p_tm_post_data_payload_alm_data->w_DeltaI = (uint16)p_Alm_Kep->navAlmDeltaI;
    p_tm_post_data_payload_alm_data->w_E = (uint16)p_Alm_Kep->navAlmE;
    p_tm_post_data_payload_alm_data->q_M0 = (uint32)p_Alm_Kep->navAlmMo;
    p_tm_post_data_payload_alm_data->q_Omega = (uint32)p_Alm_Kep->navAlmOmega;
    p_tm_post_data_payload_alm_data->w_OmegaDot = (uint16)p_Alm_Kep->navAlmOMEGADOT;
    p_tm_post_data_payload_alm_data->q_Omega0 = (uint32)p_Alm_Kep->navAlmOMEGAo;
    p_tm_post_data_payload_alm_data->q_SqrtA = (uint32)p_Alm_Kep->navAlmSqrtA;
    p_tm_post_data_payload_alm_data->u_Health = (uint8)p_Alm_Kep->navAlmSVHealth;

    p_tm_post_data_payload_alm_data->u_DataIdIsValid = FALSE;
    p_tm_post_data_payload_alm_data->u_SvGhealthIsValid = FALSE;
  }
}


/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded LPP UTC Model IE to the format ready
 *  for TM core post data message.
 *
 * Parameters:
 *
 *  p_assist: pointer to the structure containing ASN.1 decoded LPP UTC model.
 *
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data 
 *                          structure.
 *
 * Return value:
 *
 *    None.
 *    
 *
 ******************************************************************************
*/

void tm_lpp_UtcModel_to_TmCore_post_data_payload( const  GNSS_UTC_Model *p_Utc,
                                                  tm_post_data_payload_type   *p_tm_post_data_payload )
{
  gps_UtcInfoRptStructType *p_tm_post_data_payload_utc_model = &p_tm_post_data_payload->utc_model_data;

  MSG_MED("TM_LPP:UTCModel to tmcore post", 0, 0, 0);
  p_tm_post_data_payload_utc_model->q_A0 = (uint32)p_Utc->u.utcModel1->gnss_Utc_A0;
  p_tm_post_data_payload_utc_model->q_A1 = (uint32)p_Utc->u.utcModel1->gnss_Utc_A1;
  p_tm_post_data_payload_utc_model->u_DeltaTls = (uint8)p_Utc->u.utcModel1->gnss_Utc_DeltaTls;
  p_tm_post_data_payload_utc_model->u_Tot = p_Utc->u.utcModel1->gnss_Utc_Tot;
  p_tm_post_data_payload_utc_model->u_WNt = p_Utc->u.utcModel1->gnss_Utc_WNt;
  p_tm_post_data_payload_utc_model->u_WNlsf = p_Utc->u.utcModel1->gnss_Utc_WNlsf;
  p_tm_post_data_payload_utc_model->u_DN = (uint8)p_Utc->u.utcModel1->gnss_Utc_DN;
  p_tm_post_data_payload_utc_model->u_DeltaTlsf = (uint8)p_Utc->u.utcModel1->gnss_Utc_DeltaTlsf;
}



/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded LPP GPS acquistion assistance's SV direction IE to 
 *  the format ready for TM core post data message.
 *
 * Parameters:
 *
 *  p_gnss_acq_assist: pointer to the structure containing the per SV based ASN.1 
 *            decoded LPP almanac data.
 *
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data 
 *                          structure.
 *
 * Return value:
 *
 *    None.
 *    
 *
 ******************************************************************************
*/

void tm_lpp_AcqAssistSvDir_to_TmCore_post_data_payload( const GNSS_AcquisitionAssistList *p_gnss_acq_assist,
                                                        tm_post_data_payload_type *p_tm_post_data_payload,
                                                        uint32 q_GnssType
                                                       )
{
#define LPP_AZ_RAD (DEG_TO_RADIANS * (0.703125f)) // 0.0122718463030851562500000000000

  gnss_SvDirStructType          *p_tm_post_data_payload_sv_dir    = NULL;
  gnss_AzElInfoStructType       *p_tm_post_data_sv_azim_elev_info = NULL;
  GNSS_AcquisitionAssistElement *p_Acq                            = NULL;
  uint8                         u_i                               = 0;

  if ( (p_gnss_acq_assist == NULL) || (p_gnss_acq_assist->elem == NULL) || (p_tm_post_data_payload == NULL) )
  {
    return;
  }

  p_tm_post_data_payload_sv_dir = &p_tm_post_data_payload->gnss_sv_dir;
  p_tm_post_data_payload_sv_dir->q_GnssType = q_GnssType;
  p_tm_post_data_sv_azim_elev_info = &p_tm_post_data_payload_sv_dir->z_SvAzElInfo[0]; // N_SV array size
  p_Acq = p_gnss_acq_assist->elem;
  p_tm_post_data_payload_sv_dir->u_NumSvs = 0;
  p_tm_post_data_payload_sv_dir->q_GnssType = MGP_GNSS_TYPE_GPS;

  MSG_MED("TM_LPP: GPS AcqAssist (%lu) to tmcore post", p_gnss_acq_assist->n, 0, 0);
  memset(p_tm_post_data_payload_sv_dir, 0, sizeof(*p_tm_post_data_payload_sv_dir));

  for ( u_i = 0; ((u_i < p_gnss_acq_assist->n) && (u_i < N_SV)); ++u_i, ++p_tm_post_data_sv_azim_elev_info )
  {
    p_tm_post_data_sv_azim_elev_info->w_Sv = (uint8)p_Acq->svID.satellite_id + 1; // SvId (0..63) for GPS 
    p_tm_post_data_sv_azim_elev_info->f_Azimuth =  (float)p_Acq->azimuth * (FLT)LPP_AZ_RAD;
    p_tm_post_data_sv_azim_elev_info->f_Elevation = (float)p_Acq->elevation * (FLT)LPP_AZ_RAD;

    p_tm_post_data_payload_sv_dir->u_NumSvs++;

    /* Move to next element in the list */
    if ( u_i < (p_gnss_acq_assist->n - 1) )
    {
      p_Acq++;
    }
  } /* SV for-loop */
}

/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded LPP acquistion assistance IE's steering data to 
 *  the format ready for TM core post data message.
 *
 * Parameters:
 *
 *  p_assist: pointer to the structure containing the per SV based ASN.1 
 *            decoded LPP acq. assistance data.
 *
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data 
 *                          structure.
 *
 * Return value:
 *
 *    None.
 *    
 *
 ******************************************************************************
*/

void tm_lpp_AcqAssistSteering_to_TmCore_post_data_payload( GNSS_AcquisitionAssistance const *const p_gnss_acq_assist,
                                                           tm_post_data_payload_type *p_tm_post_data_payload,
                                                           uint16 const w_gps_week,
                                                           uint32 const q_gps_msec
                                                          )
{
  GNSS_AcquisitionAssistList       *p_acq_assist                   = NULL;
  gnss_SvAcqAssistCommonStructType *p_tm_post_data_payload_aa_data = NULL;
  gnss_SvSteerStructType           *p_SvSteerPerSv                 = NULL;
  GNSS_AcquisitionAssistElement    *p_Acq                          = NULL;
  uint8                            u_i;

  if ( (NULL == p_gnss_acq_assist) || (NULL == p_gnss_acq_assist->gnss_AcquisitionAssistList.elem) || (NULL == p_tm_post_data_payload) )
  {
    MSG_MED("TM_LPP:AcqAssistSteer sanity failed (%p, %p)", p_gnss_acq_assist, p_tm_post_data_payload, 0);
    return;
  }

  p_acq_assist = (GNSS_AcquisitionAssistList *)(&p_gnss_acq_assist->gnss_AcquisitionAssistList);
  p_tm_post_data_payload_aa_data = &p_tm_post_data_payload->gnss_aa_data;
  p_SvSteerPerSv = &p_tm_post_data_payload_aa_data->z_SvSteerInfo[0];  // N_SV array size
  p_Acq = p_acq_assist->elem;

  memset(p_tm_post_data_payload_aa_data, 0, sizeof(*p_tm_post_data_payload_aa_data));

  MSG_MED("TM_LPP:AcqAssistSteer (%lu) to tmcore post", p_acq_assist->n, 0, 0);
  p_tm_post_data_payload_aa_data->u_WeekValid = (w_gps_week != C_GPS_WEEK_UNKNOWN);
  p_tm_post_data_payload_aa_data->w_Week = w_gps_week;
  p_tm_post_data_payload_aa_data->q_Msec = q_gps_msec;

  p_tm_post_data_payload_aa_data->u_NumSvs = 0;
  p_tm_post_data_payload_aa_data->u_TimeRefIsGnss = TRUE; /* steering data is referenced to GPS time */
  p_tm_post_data_payload_aa_data->u_RefPnPhaseValid = FALSE;
  p_tm_post_data_payload_aa_data->q_SvSteerSrc = MGP_GNSS_SV_STEER_SRC_GPS;

  for ( u_i = 0; ((u_i < p_acq_assist->n) && (u_i < N_SV)); u_i++ )
  {
    p_SvSteerPerSv->w_Sv = (uint8)p_Acq->svID.satellite_id + 1;
    // unit is 0.5 m/s
    p_SvSteerPerSv->f_SvSpeed = (float)p_Acq->doppler0  * (FLT)0.5 * (FLT)(-1.0); /* need to flip the poliarity as ME is using a different convention for range-rate */

    /* initialize these for FALSE */
    p_SvSteerPerSv->u_SvAccelValid = TRUE;
    p_SvSteerPerSv->u_SvSpeedUncValid = TRUE;
    /* scale unit is 1/210 m/s2*/
    p_SvSteerPerSv->f_SvAccel = ((float)(p_Acq->doppler1 - 42.0) * (FLT)0.00476190 * (FLT)(-1.0));

    p_SvSteerPerSv->f_SvTimeUnc = (float)LPP_SvTimeUnc[(uint8)p_Acq->codePhaseSearchWindow];

#ifdef FEATURE_GNSS_LPP_DOPPLER_CONF_EARFCN
    /* If ‘dopplerUncertaintyExt’ is provided ignore the dopplerUncertainty field to improve GNSS receiver acquisition sensitivity. */
    if ( p_Acq->m.dopplerUncertaintyExt_r10Present )
    {
      /* Enumerated values define 60 m/s, 80 m/s, 100 m/s, 120 m/s. Any thing other than this value is considered
         as max dopper unc of 900 meters/sec */
      p_SvSteerPerSv->f_SvSpeedUnc = tm_lpp_doppuncext_to_ms(p_Acq->dopplerUncertaintyExt_r10);
    }
    else
#endif /*FEATURE_GNSS_LPP_DOPPLER_CONF_EARFCN */
    {
      p_SvSteerPerSv->f_SvSpeedUnc = (float)LPP_DopplerUncToMPS[(uint8)p_Acq->dopplerUncertainty];
    }

#ifdef FEATURE_GNSS_LPP_DOPPLER_CONF_EARFCN
    /* When confidence level (confidence-r10) is provided upscale the data to 99 percentile CEP. */
    if ( p_gnss_acq_assist->m.confidence_r10Present )
    {
      float f_confidence_cep_scale_99 = tm_util_get_cep_scale_ratio_from_pct(((float)p_gnss_acq_assist->confidence_r10 * 0.01f), 0.99f);

      /* Upscale doppler unc */
      p_SvSteerPerSv->f_SvSpeedUnc *= f_confidence_cep_scale_99;

      /* Code phase search window */
      p_SvSteerPerSv->f_SvTimeUnc *= f_confidence_cep_scale_99;
    }
#endif /*FEATURE_GNSS_LPP_DOPPLER_CONF_EARFCN */

    /* LPP has a bug where codePhase range is 0-1022. FOr network to provide 1023, they need set the codePhase1023 to TRUE
           and send 1022. The subMsec that is being received is different from RRLP GPS, no need to subtract from 1*/
    if ( p_Acq->m.codePhase1023Present == 1 )
    {
      if ( p_Acq->codePhase1023 == TRUE )
      {
        /* no need to check the actual value since this bit should be set only if value is 1022 */
        p_SvSteerPerSv->f_SubMsec = 1023.0f / 1024.0f;
      }
      else
      {
        p_SvSteerPerSv->f_SubMsec = (float)(p_Acq->codePhase) / 1024.0f;
      }
    }
    else
    {
      p_SvSteerPerSv->f_SubMsec = (float)(p_Acq->codePhase) / 1024.0f;
    }
    /* */
    if ( q_gps_msec < p_Acq->intCodePhase )
    {
      p_SvSteerPerSv->q_Ms = (uint32)((q_gps_msec + MS_PER_WEEK - p_Acq->intCodePhase) % 80);
    }
    else
    {
      p_SvSteerPerSv->q_Ms = (uint32)((q_gps_msec  - p_Acq->intCodePhase) % 80);
    }
    p_SvSteerPerSv->u_Modulo80ms = TRUE;

    if ( u_i < (p_acq_assist->n - 1) )
      p_Acq++;


    p_tm_post_data_payload_aa_data->u_NumSvs++;
    p_SvSteerPerSv++;

  } /* SV for-loop */

}

/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded LPP GLONASS acquistion assistance IE's steering data to 
 *  the format ready for TM core post data message.
 *
 * Parameters:
 *
 *  p_gnss_acq_assist [IN]: pointer to the structure containing the per SV based ASN.1 
 *                          decoded LPP GLONASS acq. assistance data.
 *  p_prot_glo_aa_buff[OUT]: pointer to Glonass AA data out structure.
 *  q_glo_rt_msec     [IN]: Glonass millisecond in day reference time.
 *
 * Return value:
 *
 *    None.
 *    
 *
 ******************************************************************************
*/

void tm_lpp_glo_acq_assist_to_tmcore_post_data_payload
(
  GNSS_AcquisitionAssistance const *const p_gnss_acq_assist,
  prot_glo_acqast_buffer_s_type *p_prot_glo_aa_buff,
  const uint32 q_glo_rt_msec
 )
{
#define LPP_AZ_RAD (DEG_TO_RADIANS * (0.703125f))

  uint8                               u_i                           = 0;
  uint32                              u_num_acq_assist              = 0;
  GNSS_AcquisitionAssistList const    *p_gnss_acq_assist_list       = NULL;
  gnss_SvSteerStructType              *p_prot_glo_sv_steering_persv = NULL;
  GNSS_AcquisitionAssistElement const *p_gnss_acq_assist_elem       = NULL;
  gnss_SvAcqAssistCommonStructType    *p_prot_glo_aa_steering       = NULL;
  gnss_SvDirStructType                *p_prot_glo_aa_sv_dir         = NULL;
  gnss_AzElInfoStructType             *p_prot_glo_sv_dir_azim_elev  = NULL;

  if ( (NULL == p_gnss_acq_assist) || (NULL == p_prot_glo_aa_buff) )
  {
    MSG_ERROR("TM_LPP_UP: Invalid glonass steering in/out struct.", 0, 0, 0);
    return;
  }

  p_gnss_acq_assist_list = &(p_gnss_acq_assist->gnss_AcquisitionAssistList);

  // Steering
  memset(&p_prot_glo_aa_buff->z_steering_glo_data, 0, sizeof(p_prot_glo_aa_buff->z_steering_glo_data));
  p_prot_glo_aa_steering = &p_prot_glo_aa_buff->z_steering_glo_data;
  p_prot_glo_sv_steering_persv = &p_prot_glo_aa_buff->z_steering_glo_data.z_SvSteerInfo[0];
  p_prot_glo_aa_steering->u_NumSvs = 0;
  p_prot_glo_aa_buff->z_steering_glo_data.q_SvSteerSrc = (uint32)MGP_GNSS_SV_STEER_SRC_GLO;
  // SV Direction
  memset(&p_prot_glo_aa_buff->gnss_sv_dir, 0, sizeof(p_prot_glo_aa_buff->gnss_sv_dir));
  p_prot_glo_aa_sv_dir = &p_prot_glo_aa_buff->gnss_sv_dir;
  p_prot_glo_sv_dir_azim_elev = &p_prot_glo_aa_sv_dir->z_SvAzElInfo[0];
  p_prot_glo_aa_sv_dir->u_NumSvs = 0;
  p_prot_glo_aa_buff->gnss_sv_dir.q_GnssType = (uint32)MGP_GNSS_TYPE_GLO;
  // Source structure
  p_gnss_acq_assist_elem = p_gnss_acq_assist_list->elem;

  MSG_MED("TM_LPP_UP: Build %lu Glonass AA SVDirection & Steering info", p_gnss_acq_assist_list->n, 0, 0);

  /* Compute Glonass time & FrequencyNumber separately when GPS TimeModel & Glo AuxiInfo AD is received respectively */

  /* ME can't eat more than it can digest */
  if ( p_gnss_acq_assist_list->n > N_GLO_FREQ )
  {
    MSG_ERROR("TM_LPP_UP: Can't handle AA data of %u elements. Max %d can be consumed.", p_gnss_acq_assist_list->n, N_GLO_FREQ, 0);
    u_num_acq_assist = N_GLO_FREQ;
  }
  else
  {
    u_num_acq_assist = p_gnss_acq_assist_list->n;
  }

  /* AA p_gnss_acq_assist_list->n (1..64) */
  for ( u_i = 0; u_i < u_num_acq_assist; ++u_i, ++p_gnss_acq_assist_elem )
  {
    // svID.satellite_id (0..63). 0..23 valid Glonass sv ids. 24..63 reserved.
    if ( p_gnss_acq_assist_elem->svID.satellite_id < N_GLO_SV )
    {
      //
      // AA-Steering
      //
      // local u_Sv (65..88)
      p_prot_glo_sv_steering_persv->w_Sv = (uint8)p_gnss_acq_assist_elem->svID.satellite_id + GLO_SLOTID_OFFSET + 1;
      // unit is 0.5 m/s
      /* need to flip the poliarity as ME is using a different convention for range-rate */
      p_prot_glo_sv_steering_persv->f_SvSpeed = (float)p_gnss_acq_assist_elem->doppler0  * (FLT)0.5 * (FLT)(-1.0);

      p_prot_glo_sv_steering_persv->u_SvAccelValid = TRUE;
      p_prot_glo_sv_steering_persv->u_SvSpeedUncValid = TRUE;

      /* scale unit is 1/210 m/s2*/
      p_prot_glo_sv_steering_persv->f_SvAccel = ((float)(p_gnss_acq_assist_elem->doppler1 - 42.0) * (FLT)0.00476190 * (FLT)(-1.0));

      p_prot_glo_sv_steering_persv->f_SvTimeUnc = (float)LPP_SvTimeUnc[(uint8)p_gnss_acq_assist_elem->codePhaseSearchWindow];

#ifdef FEATURE_GNSS_LPP_DOPPLER_CONF_EARFCN
      /* If ‘dopplerUncertaintyExt’ is provided ignore the dopplerUncertainty field to improve GNSS receiver acquisition sensitivity. */
      if ( p_gnss_acq_assist_elem->m.dopplerUncertaintyExt_r10Present )
      {
        /* Enumerated values define 60 m/s, 80 m/s, 100 m/s, 120 m/s. Any thing other than this value is considered
           as max dopper unc of 900 meters/sec */
        p_prot_glo_sv_steering_persv->f_SvSpeedUnc = tm_lpp_doppuncext_to_ms(p_gnss_acq_assist_elem->dopplerUncertaintyExt_r10);
      }
      else
#endif /*FEATURE_GNSS_LPP_DOPPLER_CONF_EARFCN */
      {
        p_prot_glo_sv_steering_persv->f_SvSpeedUnc = (float)LPP_DopplerUncToMPS[(uint8)p_gnss_acq_assist_elem->dopplerUncertainty];
      }

#ifdef FEATURE_GNSS_LPP_DOPPLER_CONF_EARFCN
      /* And when confidence level (confidence-r10) is provided upscale the confidence to 99 percentile CEP.
      */
      if ( p_gnss_acq_assist->m.confidence_r10Present )
      {
        float f_confidence_cep_scale_99 = tm_util_get_cep_scale_ratio_from_pct(((float)p_gnss_acq_assist->confidence_r10 * 0.01f), 0.99f);

        /* Upscale doppler unc */
        p_prot_glo_sv_steering_persv->f_SvSpeedUnc *= f_confidence_cep_scale_99;

        /* Code phase search window */
        p_prot_glo_sv_steering_persv->f_SvTimeUnc *= f_confidence_cep_scale_99;
      }
#endif /*FEATURE_GNSS_LPP_DOPPLER_CONF_EARFCN */

      /* LPP has a bug where codePhase range is 0-1022. FOr network to provide 1023, they need set the codePhase1023 to TRUE
         and send 1022. The subMsec that is being received is different from RRLP GPS, no need to subtract from 1*/
      if ( p_gnss_acq_assist_elem->m.codePhase1023Present == 1 )
      {
        if ( p_gnss_acq_assist_elem->codePhase1023 == TRUE )
        {
          /* no need to check the actual value since this bit should be set only if value is 1022 */
          p_prot_glo_sv_steering_persv->f_SubMsec = 1023.0f / 1024.0f;
        }
        else
        {
          p_prot_glo_sv_steering_persv->f_SubMsec = (float)(p_gnss_acq_assist_elem->codePhase) / 1024.0f;
        }
      }
      else
      {
        p_prot_glo_sv_steering_persv->f_SubMsec = (float)(p_gnss_acq_assist_elem->codePhase) / 1024.0f;
      }

      if ( q_glo_rt_msec < p_gnss_acq_assist_elem->intCodePhase )
      {
        p_prot_glo_sv_steering_persv->q_Ms = (uint32)((q_glo_rt_msec + DAY_MSECS - p_gnss_acq_assist_elem->intCodePhase) % 80);
      }
      else
      {
        p_prot_glo_sv_steering_persv->q_Ms = (uint32)((q_glo_rt_msec  - p_gnss_acq_assist_elem->intCodePhase) % 80);
      }

      p_prot_glo_sv_steering_persv->u_Modulo80ms = TRUE;
      p_prot_glo_aa_steering->u_NumSvs++;
      ++p_prot_glo_sv_steering_persv; // increment steering dest elem


      //
      // AA-SV Direction
      //
      // local u_Sv (65..88) 
      p_prot_glo_sv_dir_azim_elev->w_Sv = (uint8)p_gnss_acq_assist_elem->svID.satellite_id + GLO_SLOTID_OFFSET + 1; 
      p_prot_glo_sv_dir_azim_elev->f_Azimuth = (float)p_gnss_acq_assist_elem->azimuth * (FLT)LPP_AZ_RAD;
      p_prot_glo_sv_dir_azim_elev->f_Elevation = (float)p_gnss_acq_assist_elem->elevation * (FLT)LPP_AZ_RAD;
      p_prot_glo_sv_dir_azim_elev->u_ElevUncValid = FALSE;

      // Glonass Frequency/Channel number field (b_GloFreqIndex) is filled by LPP AuxiInfo message
      p_prot_glo_aa_sv_dir->u_NumSvs++;
      ++p_prot_glo_sv_dir_azim_elev; // increment svdir dest elem
    }
    else
    {
      MSG_HIGH("TM_LPP: Glonass AA SvId %u invalid", p_gnss_acq_assist_elem->svID.satellite_id, 0, 0);
    }
  } /* SV for-loop */

  /* Ensure the total SV count for SV-Dir & Steering are same */
  SM_ASSERT(p_prot_glo_aa_steering->u_NumSvs == p_prot_glo_aa_sv_dir->u_NumSvs);
}


/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded LPP OTDOA assistance IE's steering data to 
 *  the format ready for TM core post data message.
 *
 * Parameters:
 *
 *  p_assist: pointer to the structure containing the per SV based ASN.1 
 *            decoded LPP OTDOA assistance data.
 *
 *  p_tm_post_data_payload: pointer to the structure of TmCore post-data 
 *                          structure.
 *
 * Return value:
 *
 *    None.
 *    
 *
 ******************************************************************************
*/

void tm_lpp_OTDOAAssist_to_TmCore_post_data_payload( OTDOA_ProvideAssistanceData *p_otdoa_assist,
                                                     tm_post_data_payload_type *p_tm_post_data_payload
                                                    )
{

  sm_OTDOA_AssistanceData *p_tm_post_data_payload_otdoa_data = &p_tm_post_data_payload->otdoa_assist_data.otdoa_assist_data;
  uint8                   i                                  = 0;
  uint8                   j                                  = 0;
  uint8                   k                                  = 0;
  uint32                  prs_value                          = 0;

  MSG_MED("TM_LPP:OTDOA Assist to tmcore post", 0, 0, 0);


  memset(p_tm_post_data_payload_otdoa_data, 0, sizeof(sm_OTDOA_AssistanceData));
  p_tm_post_data_payload_otdoa_data->m.otdoa_ReferenceCellInfoPresent =
                                                                        p_otdoa_assist->m.otdoa_ReferenceCellInfoPresent;

  if ( p_otdoa_assist->m.otdoa_ReferenceCellInfoPresent == 1 )
  {
    /* reference cell info*/
    p_tm_post_data_payload_otdoa_data->m.otdoa_ReferenceCellInfoPresent = 1;
    p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.cpLength =
                                                                          p_otdoa_assist->otdoa_ReferenceCellInfo.cpLength;
    p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.m.antennaPortConfigPresent =
                                                                                            p_otdoa_assist->otdoa_ReferenceCellInfo.m.antennaPortConfigPresent;
    if ( p_otdoa_assist->otdoa_ReferenceCellInfo.m.antennaPortConfigPresent == 1 )
    {
      p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.antennaPortConfig =
                                                                                     p_otdoa_assist->otdoa_ReferenceCellInfo.antennaPortConfig;
    }
    p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.m.cellGlobalIdPresent =
                                                                                       p_otdoa_assist->otdoa_ReferenceCellInfo.m.cellGlobalIdPresent;
    if ( p_otdoa_assist->otdoa_ReferenceCellInfo.m.cellGlobalIdPresent == 1 )
    {
      p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.cellGlobalId.cell_id_numbits =
                                                                                                p_otdoa_assist->otdoa_ReferenceCellInfo.cellGlobalId.cellidentity.numbits;

      p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.cellGlobalId.cellidentity =
                                                                                             tm_lpp_osys_asn1_bitstring_to_int(sizeof(uint32),
                                                                                                                               &p_otdoa_assist->otdoa_ReferenceCellInfo.cellGlobalId.cellidentity);
      if ( p_otdoa_assist->otdoa_ReferenceCellInfo.cellGlobalId.mcc.n > 0 )
      {
        for ( i = 0; i < p_otdoa_assist->otdoa_ReferenceCellInfo.cellGlobalId.mcc.n; i++ )
        {
          p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.cellGlobalId.mcc[i] =
                                                                                           p_otdoa_assist->otdoa_ReferenceCellInfo.cellGlobalId.mcc.elem[i];
        }
      }

      if ( p_otdoa_assist->otdoa_ReferenceCellInfo.cellGlobalId.mnc.n > 0 )
      {
        p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.cellGlobalId.num_mnc_digits =
                                                                                                 p_otdoa_assist->otdoa_ReferenceCellInfo.cellGlobalId.mnc.n;
        for ( i = 0; i < p_otdoa_assist->otdoa_ReferenceCellInfo.cellGlobalId.mnc.n; i++ )
        {
          p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.cellGlobalId.mnc[i] =
                                                                                           p_otdoa_assist->otdoa_ReferenceCellInfo.cellGlobalId.mnc.elem[i];
        }
      }
    }

    /* earfcnRef & earfcnRef-v9a0 are mutually exclusive (optional ie).
       RG: UE uses earfcnRef_v9a0 as the frequency of the cell if both earfcnRef & earfcnRef_v9a0 are present. */
#ifdef FEATURE_GNSS_LPP_DOPPLER_CONF_EARFCN
    if ( 1 == p_otdoa_assist->otdoa_ReferenceCellInfo.m.earfcnRef_v9a0Present )
    {
      /* earfcnRef_v9a0 (65536..262143) */
      p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.earfcnRef = p_otdoa_assist->otdoa_ReferenceCellInfo.earfcnRef_v9a0;
      p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.m.earfcnRefPresent = 1;
    }
    else
#endif /* FEATURE_GNSS_LPP_DOPPLER_CONF_EARFCN */
      if ( 1 == p_otdoa_assist->otdoa_ReferenceCellInfo.m.earfcnRefPresent )
      {
        /* earfcnRef (0..65535) */
        p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.earfcnRef = p_otdoa_assist->otdoa_ReferenceCellInfo.earfcnRef;
        p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.m.earfcnRefPresent = 1;
      }

    p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.physCellId =
                                                                            p_otdoa_assist->otdoa_ReferenceCellInfo.physCellId;
    p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.m.prsInfoPresent =
                                                                                  p_otdoa_assist->otdoa_ReferenceCellInfo.m.prsInfoPresent;
    if ( p_otdoa_assist->otdoa_ReferenceCellInfo.m.prsInfoPresent == 1 )
    {
      p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.prsInfo.numDL_Frames =
                                                                                        p_otdoa_assist->otdoa_ReferenceCellInfo.prsInfo.numDL_Frames;
      p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.prsInfo.prs_Bandwidth =
                                                                                         p_otdoa_assist->otdoa_ReferenceCellInfo.prsInfo.prs_Bandwidth;
      p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.prsInfo.prs_ConfigurationIndex =
                                                                                                  p_otdoa_assist->otdoa_ReferenceCellInfo.prsInfo.prs_ConfigurationIndex;
      p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.prsInfo.m.prs_MutingInfo_r9Present =
                                                                                                      p_otdoa_assist->otdoa_ReferenceCellInfo.prsInfo.m.prs_MutingInfo_r9Present;
      if (  p_otdoa_assist->otdoa_ReferenceCellInfo.prsInfo.m.prs_MutingInfo_r9Present == 1 )
      {
        p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.prsInfo.prs_MutingInfo_r9.t =
                                                                                                 (uint8)p_otdoa_assist->otdoa_ReferenceCellInfo.prsInfo.prs_MutingInfo_r9.t;
        switch ( p_otdoa_assist->otdoa_ReferenceCellInfo.prsInfo.prs_MutingInfo_r9.t  )
        {
           case LPP_PRS_MUTINGINFO_POS2:
             prs_value = tm_lpp_osys_asn1_bitstring_to_int(sizeof(uint32), p_otdoa_assist->otdoa_ReferenceCellInfo.prsInfo.prs_MutingInfo_r9.u.po2_r9);
             p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.prsInfo.prs_MutingInfo_r9.u.po2_r9[0] = (uint8)prs_value;
             MSG_MED("TM_LPP:PRS_VALUE %d, muting pattern %d", prs_value, p_otdoa_assist->otdoa_ReferenceCellInfo.prsInfo.prs_MutingInfo_r9.t, 0);
             break;

           case LPP_PRS_MUTINGINFO_POS4:
             prs_value = tm_lpp_osys_asn1_bitstring_to_int(sizeof(uint32), p_otdoa_assist->otdoa_ReferenceCellInfo.prsInfo.prs_MutingInfo_r9.u.po4_r9);
             p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.prsInfo.prs_MutingInfo_r9.u.po4_r9[0] = (uint8)prs_value;
             MSG_MED("TM_LPP:PRS_VALUE %d, muting pattern %d", prs_value, p_otdoa_assist->otdoa_ReferenceCellInfo.prsInfo.prs_MutingInfo_r9.t, 0);
             break;

           case LPP_PRS_MUTINGINFO_POS8:
             prs_value = tm_lpp_osys_asn1_bitstring_to_int(sizeof(uint32), p_otdoa_assist->otdoa_ReferenceCellInfo.prsInfo.prs_MutingInfo_r9.u.po8_r9);
             p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.prsInfo.prs_MutingInfo_r9.u.po8_r9[0] = (uint8)prs_value;
             MSG_MED("TM_LPP:PRS_VALUE %d, muting pattern %d", prs_value, p_otdoa_assist->otdoa_ReferenceCellInfo.prsInfo.prs_MutingInfo_r9.t, 0);
             break;

           case LPP_PRS_MUTINGINFO_POS16:
             prs_value = tm_lpp_osys_asn1_bitstring_to_int(sizeof(uint32), p_otdoa_assist->otdoa_ReferenceCellInfo.prsInfo.prs_MutingInfo_r9.u.po16_r9);
             p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.prsInfo.prs_MutingInfo_r9.u.po16_r9[0] = (uint8)(prs_value & 0xFF);
             p_tm_post_data_payload_otdoa_data->otdoa_ReferenceCellInfo.prsInfo.prs_MutingInfo_r9.u.po16_r9[1] = (uint8)((prs_value & 0xFF00) >> 8);
             MSG_MED("TM_LPP:PRS_VALUE %d, muting pattern %d", prs_value, p_otdoa_assist->otdoa_ReferenceCellInfo.prsInfo.prs_MutingInfo_r9.t, 0);
             break;

           default:
             break;
        } /* switch prs muting info */
      } /* prs muting info present */
    }

  } /* reference cell info present*/


  p_tm_post_data_payload_otdoa_data->m.otdoa_NeighbourCellInfoPresent =
                                                                        p_otdoa_assist->m.otdoa_NeighbourCellInfoPresent;
  if ( p_otdoa_assist->m.otdoa_NeighbourCellInfoPresent == 1 )
  {
    p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.n =
                                                                   p_otdoa_assist->otdoa_NeighbourCellInfo.n;
    if ( p_otdoa_assist->otdoa_NeighbourCellInfo.n > 0 )
    {
      for ( i = 0; i < p_otdoa_assist->otdoa_NeighbourCellInfo.n; i++ )
      {
        p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].n =
                                                                                             p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].n;
        if ( p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].n > 0 )
        {
          /* NeighbourCellInfo elements 1..24 */
          for ( j = 0; j < p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].n; j++ )
          {
            /* earfcn & earfcn-v9a0 are mutually exclusive (optional ie).
               RG: UE uses earfcn_v9a0 as the frequency of the cell if both earfcn & earfcn_v9a0 are present. */
#ifdef FEATURE_GNSS_LPP_DOPPLER_CONF_EARFCN
            if ( 1 == p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].m.earfcn_v9a0Present )
            {
              /* Spec : (65536..262143) */
              p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].m.earfcnPresent = 1;
              p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].earfcn = p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].earfcn_v9a0;
            }
            else
#endif /* FEATURE_GNSS_LPP_DOPPLER_CONF_EARFCN */
              if ( 1 == p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].m.earfcnPresent )
              {
                /* Spec : (0..65535) */
                p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].m.earfcnPresent = 1;
                p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].earfcn = p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].earfcn;
              }

            /* phys cellid */
            p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].physCellId = p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].physCellId;
            /* antenna config port */
            p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].m.antennaPortConfigPresent =
                                                                                                                                                p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].m.antennaPortConfigPresent;
            if ( p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].m.antennaPortConfigPresent == 1 )
            {
              p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].antennaPortConfig =
                                                                                                                                         p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].antennaPortConfig;
            }
            /* cell global id */
            p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].m.cellGlobalIdPresent = p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].m.cellGlobalIdPresent;
            if ( p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].m.cellGlobalIdPresent == 1 )
            {
              p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].cellGlobalId.cell_id_numbits = p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].cellGlobalId.cellidentity.numbits;

              p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].cellGlobalId.cellidentity =
                                                                                                                                                 tm_lpp_osys_asn1_bitstring_to_int(sizeof(uint32),
                                                                                                                                                                                   &p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].cellGlobalId.cellidentity);
              if ( p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].cellGlobalId.mcc.n > 0 )
              {
                for ( k = 0; k < p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].cellGlobalId.mcc.n; k++ )
                {
                  p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].cellGlobalId.mcc[k] =
                                                                                                                                               p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].cellGlobalId.mcc.elem[k];
                }
              }

              if ( p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].cellGlobalId.mnc.n > 0 )
              {
                p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].cellGlobalId.num_mnc_digits = p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].cellGlobalId.mnc.n;
                for ( k = 0; k < p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].cellGlobalId.mnc.n; k++ )
                {
                  p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].cellGlobalId.mnc[k] =
                                                                                                                                               p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].cellGlobalId.mnc.elem[k];
                }
              }
            } /* cellid present*/

            /* prs info*/
            p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].m.prsInfoPresent = p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].m.prsInfoPresent;
            if ( p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].m.prsInfoPresent == 1  )
            {
              p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].prsInfo.prs_ConfigurationIndex =
                                                                                                                                                      p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].prsInfo.prs_ConfigurationIndex;
              p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].prsInfo.prs_Bandwidth =
                                                                                                                                             p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].prsInfo.prs_Bandwidth;
              p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].prsInfo.numDL_Frames =
                                                                                                                                            p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].prsInfo.numDL_Frames;

              if ( p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].prsInfo.m.prs_MutingInfo_r9Present == 1 )
              {
                p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].prsInfo.m.prs_MutingInfo_r9Present = 1;
                p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].prsInfo.prs_MutingInfo_r9.t =
                                                                                                                                                     (uint8)p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].prsInfo.prs_MutingInfo_r9.t;

                switch ( p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].prsInfo.prs_MutingInfo_r9.t )
                {
                   case LPP_PRS_MUTINGINFO_POS2:
                     prs_value = tm_lpp_osys_asn1_bitstring_to_int(sizeof(uint32), p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].prsInfo.prs_MutingInfo_r9.u.po2_r9);
                     p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].prsInfo.prs_MutingInfo_r9.u.po2_r9[0] = (uint8)prs_value;
                     MSG_MED("TM_LPP:PRS_VALUE %d, muting pattern %d", prs_value, p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].prsInfo.prs_MutingInfo_r9.t, 0);
                     break;

                   case LPP_PRS_MUTINGINFO_POS4:
                     prs_value = tm_lpp_osys_asn1_bitstring_to_int(sizeof(uint32), p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].prsInfo.prs_MutingInfo_r9.u.po4_r9);
                     p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].prsInfo.prs_MutingInfo_r9.u.po4_r9[0] = (uint8)prs_value;
                     MSG_MED("TM_LPP:PRS_VALUE %d, muting pattern %d", prs_value, p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].prsInfo.prs_MutingInfo_r9.t, 0);
                     break;

                   case LPP_PRS_MUTINGINFO_POS8:
                     prs_value = tm_lpp_osys_asn1_bitstring_to_int(sizeof(uint32), p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].prsInfo.prs_MutingInfo_r9.u.po8_r9);
                     p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].prsInfo.prs_MutingInfo_r9.u.po8_r9[0] = (uint8)prs_value;
                     MSG_MED("TM_LPP:PRS_VALUE %d, muting pattern %d", prs_value, p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].prsInfo.prs_MutingInfo_r9.t, 0);
                     break;

                   case LPP_PRS_MUTINGINFO_POS16:
                     prs_value = tm_lpp_osys_asn1_bitstring_to_int(sizeof(uint32), p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].prsInfo.prs_MutingInfo_r9.u.po16_r9);
                     p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].prsInfo.prs_MutingInfo_r9.u.po16_r9[0] = (uint8)(prs_value & 0xFF);
                     p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].prsInfo.prs_MutingInfo_r9.u.po16_r9[1] = (uint8)((prs_value & 0xFF00) >> 8);
                     MSG_MED("TM_LPP:PRS_VALUE %d, muting pattern %d", prs_value, p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].prsInfo.prs_MutingInfo_r9.t, 0);
                     break;

                   default:
                     break;
                } /* switch prs muting info */
              } /* r9 info present */
            } /* prsinfo present */
            p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].expectedRSTD =
                                                                                                                                  p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].expectedRSTD;
            p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].expectedRSTD_Uncertainty =
                                                                                                                                              p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].expectedRSTD_Uncertainty;
            p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].m.cpLengthPresent =
                                                                                                                                       p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].m.cpLengthPresent;
            p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].cpLength =
                                                                                                                              p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].cpLength;
            p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].m.prs_SubframeOffsetPresent =
                                                                                                                                                 p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].m.prs_SubframeOffsetPresent;
            p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].prs_SubframeOffset =
                                                                                                                                        p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].prs_SubframeOffset;
            p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].m.slotNumberOffsetPresent =
                                                                                                                                               p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].m.slotNumberOffsetPresent;
            p_tm_post_data_payload_otdoa_data->otdoa_NeighbourCellInfo.neighbor_freq_elem[i].neighbor_cell_elem[j].slotNumberOffset =
                                                                                                                                      p_otdoa_assist->otdoa_NeighbourCellInfo.elem[i].elem[j].slotNumberOffset;
          } /* for each neigbor cell info loop j*/
        } /* if neighbor cell info n > 0*/
      } /* for each neighbor freq info loop i*/
    } /* if neighbor freq info n > 0*/
  } /* if otdoa neighbor cell info present*/
}


/*
 ******************************************************************************
 *
 * tm_lpp_glo_nav_model_proc
 *
 * Function description:
 *  This function processes the OSYS ASN.1 decoded LPP Assistance data for glo eph and 
 *  prepares such assistance data for injecting to MGP.
 *
 * Parameters:
 *  p_assist: pointer to the OSYS ASN.1 decoded LPP gpsAsssitance data.
 * 
 * Return value:
 *
 ******************************************************************************
*/
void tm_lpp_glo_nav_model_proc( tm_prtl_type q_protocol,
                                prot_glo_eph_buffer_s_type z_prot_glo_eph_buf[N_GLO_SV],
                                tm_sess_handle_type q_active_sess_handle,
                                const GNSS_GenericAssistDataElement *const p_elem,
                                tm_post_data_payload_type *p_tm_post_data_payload )
{
  uint8                         j                      = 0;
  GNSS_NavModelSatelliteElement *p_NavModelEph         = NULL;
  const uint8                   num_elem               = (uint8)p_elem->gnss_NavigationModel.gnss_SatelliteList.n;
  uint32                        q_eph_svs_injected     = 0,
                                q_eph_svs_not_injected = 0;

  if ( (num_elem > 0) && (num_elem < 65) )
  {
    for ( j = 0; j < num_elem; ++j ) // iterate each nav satellite
    {
      p_NavModelEph = &p_elem->gnss_NavigationModel.gnss_SatelliteList.elem[j];

      if ( p_NavModelEph->svID.satellite_id < N_GLO_SV ) // 0..23
      {
        tm_lpp_glo_eph_to_tmcore_post_data_payload(p_NavModelEph,
                                                   &z_prot_glo_eph_buf[p_NavModelEph->svID.satellite_id].z_eph_glo_data);

        /* if this SV already has freq. number, inject it now; otherwise keep data in the buffer and inject when all 
                              assistance have been delivered */
        if ( z_prot_glo_eph_buf[p_NavModelEph->svID.satellite_id].u_freq_num_valid )
        {
          /*  Copy over to TM post struct */
          memscpy(&p_tm_post_data_payload->eph_glo_data, sizeof(p_tm_post_data_payload->eph_glo_data),
                  &z_prot_glo_eph_buf[p_NavModelEph->svID.satellite_id].z_eph_glo_data,
                  sizeof(z_prot_glo_eph_buf[p_NavModelEph->svID.satellite_id].z_eph_glo_data));
          p_tm_post_data_payload->eph_glo_data.u_SvId = p_NavModelEph->svID.satellite_id;

          /* Post data to TM */
          if ( FALSE == tm_post_data(q_protocol,
                                     q_active_sess_handle,
                                     TM_POST_DATA_TYPE_EPH_GLO,
                                     p_tm_post_data_payload
                                    ) )
          {
            MSG_HIGH("TM_LPP(GLO) to Tmcore data post failed", 0, 0, 0);
          }
          else
          {
            q_eph_svs_injected |= 1L <<  p_tm_post_data_payload->eph_glo_data.u_SvId;
            MSG_MED("TM_LPP(GLO): SV %u eph. posted to TmCore", p_tm_post_data_payload->eph_glo_data.u_SvId, 0, 0);
          }
        } /* if this SV already has freq. number */
        else
        {
          q_eph_svs_not_injected |= 1L << p_NavModelEph->svID.satellite_id;
        }
      }
    } /* Iterate for each svId */

    if ( q_eph_svs_injected != 0 )
    {
      MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED,
            "TM_LPP(GLO): eph posted to TmCore (eph. recv) 0x%lx, total SVs = %u",
            q_eph_svs_injected, num_elem);
    }
    if ( q_eph_svs_not_injected != 0 )
    {
      MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED,
            "TM_LPP(GLO): eph pending posting to TmCore (eph. recv) 0x%lx, total SVs = %u",
            q_eph_svs_not_injected, num_elem);
    }
  } /* If ends */

}

/*
 ******************************************************************************
 *
 * tm_lpp_cp_glo_alm_proc
 *
 * Function description:
 *  This function processes the OSYS ASN.1 decoded LPP Assistance data for glo alm and 
 *  prepares such assistance data for injecting to MGP.
 *
 * Parameters:
 *  p_assist: pointer to the OSYS ASN.1 decoded LPP gpsAsssitance data.
 * 
 * Return value:
 *
 ******************************************************************************
*/
void tm_lpp_glo_alm_proc( tm_prtl_type q_protocol,
                          prot_glo_eph_buffer_s_type z_prot_glo_eph_buf[N_GLO_SV],
                          tm_sess_handle_type q_active_sess_handle,
                          const GNSS_GenericAssistDataElement *const p_elem,
                          tm_post_data_payload_type *p_tm_post_data_payload )
{
  uint8               i                  = 0;
  GNSS_AlmanacElement *p_Almanac         = NULL;
  uint32              q_alm_svs_injected = 0;

  /* z_elem.gnss_Almanac.weekNumber & z_elem.gnss_Almanac.toa available only for non-Glonass GNSS */
  if ( p_elem->gnss_Almanac.gnss_AlmanacList.n > 0 )
  {
    for ( i = 0; i < p_elem->gnss_Almanac.gnss_AlmanacList.n; i++ )
    {
      p_Almanac = &p_elem->gnss_Almanac.gnss_AlmanacList.elem[i];
      if ( T_GNSS_AlmanacElement_keplerianGLONASS == p_Almanac->t )
      {
        tm_lpp_common_glo_eph_svid_freq_num_map_alm(p_Almanac->u.keplerianGLONASS, z_prot_glo_eph_buf);
        tm_lpp_glo_alm_to_tmcore_post_data_payload(p_Almanac, &p_tm_post_data_payload->alm_glo_data);

        if ( FALSE == tm_post_data(q_protocol,
                                   q_active_sess_handle,
                                   TM_POST_DATA_TYPE_ALM_GLO,
                                   p_tm_post_data_payload
                                  ) )
        {
          MSG_HIGH("TM_LPP(GLO) Alm. to Tmcore data post failed", 0, 0, 0);
        }
        else
        {
          q_alm_svs_injected |= 1L << (p_Almanac->u.keplerianGLONASS->gloAlmnA - 1); // gloAlmnA is (1..24) - SV slot id
          MSG_MED("TM_LPP_UP: SUPL(GLO): Slot Id %u Alm. posted to TmCore", p_tm_post_data_payload->alm_glo_data.u_SvId, 0, 0);
        } /* posted almanac element */
      } /* Alm model for Glonass */
    } /* for no of alamanac */

  } /* n > 0*/
  /* Move to next element of the list */

  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "TM_LPP(GLO): Alm. posted to TmCore 0x%lX, total SVs = %u",
        q_alm_svs_injected, p_elem->gnss_Almanac.gnss_AlmanacList.n);
}

/*
 ******************************************************************************
 *
 * tm_lpp_cp_glo_rti_proc
 *
 * Function description:
 *  This function processes the OSYS ASN.1 decoded LPP Assistance data for glo alm and 
 *  prepares such assistance data for injecting to MGP.
 *
 * Parameters:
 *  p_assist: pointer to the OSYS ASN.1 decoded LPP gpsAsssitance data.
 * 
 * Return value:
 *
 ******************************************************************************
*/
void tm_lpp_glo_rti_proc( tm_prtl_type q_protocol,
                          tm_sess_handle_type q_active_sess_handle,
                          const GNSS_GenericAssistDataElement *const p_elem,
                          tm_post_data_payload_type *p_tm_post_data_payload )
{
  uint32           u_i                  = 0;
  uint32           q_RealTimeIntegrity0 = 0;
  uint32           q_RealTimeIntegrity1 = 0;
  BadSignalElement *p_RTI               = NULL;

  if ( p_elem->gnss_RealTimeIntegrity.gnss_BadSignalList.n > 0 )
  {
    for ( u_i = 0; u_i < p_elem->gnss_RealTimeIntegrity.gnss_BadSignalList.n; u_i++ )
    {
      p_RTI = &p_elem->gnss_RealTimeIntegrity.gnss_BadSignalList.elem[u_i];

      /* LPP satellite_id (0..63). For Glonass 0..23 is valid, interpreted as slot number 1 to 24. The SvId 24..63 is reserved */
      if ( p_RTI->badSVID.satellite_id < N_GLO_SV )
      {
        q_RealTimeIntegrity0 |= 1L << (uint32)(p_RTI->badSVID.satellite_id);
      }
      else
      {
        q_RealTimeIntegrity1 |= 1L << (uint32)(p_RTI->badSVID.satellite_id - N_GLO_SV);
      }

    } /* for each sv */

    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "TM_LPP(Glo): SV RTI/Health list 0x%X, 0x%X (SV > 24)",
          q_RealTimeIntegrity0, q_RealTimeIntegrity1);

    /* Determine if we have enough info. about some SVs' health and inform MGP about such info., if available */
    if ( q_RealTimeIntegrity0 )
    {
      p_tm_post_data_payload->gnss_sv_health_info.e_GnssType = MGP_GNSS_TYPE_GLO;
      p_tm_post_data_payload->gnss_sv_health_info.q_BadSvMask = q_RealTimeIntegrity0;

      if ( FALSE == tm_post_data(q_protocol,
                                 q_active_sess_handle,
                                 TM_POST_DATA_TYPE_GNSS_SV_HEALTH_INFO,
                                 p_tm_post_data_payload
                                ) )
      {
        MSG_HIGH("TM_LPP(Glo): RTI to Tmcore data post failed", 0, 0, 0);
      }
      else
      {
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "TM_LPP(Glo): RTI for Sv Ids 0x%lX posted to TmCore",
              q_RealTimeIntegrity0);
      }
    }

  } /* if n > 0 */
}

/*
 ******************************************************************************
 *
 * tm_lpp_cp_glo_utc_proc
 *
 * Function description:
 *  This function processes the OSYS ASN.1 decoded LPP Assistance data for glo utc and 
 *  prepares such assistance data for injecting to MGP.
 *
 * Parameters:
 *  p_assist: pointer to the OSYS ASN.1 decoded LPP gpsAsssitance data.
 * 
 * Return value:
 *
 ******************************************************************************
*/
void tm_lpp_glo_utc_proc( tm_prtl_type q_protocol,
                          tm_sess_handle_type q_active_sess_handle,
                          const GNSS_GenericAssistDataElement *const p_elem,
                          tm_post_data_payload_type *p_tm_post_data_payload )
{
  if ( p_elem->gnss_UTC_Model.t == T_GNSS_UTC_Model_utcModel3 ) /* Glonass UTC model */
  {
    /* Init */
    p_tm_post_data_payload->utc_model_glo_data.z_Mask.z_FilledMask.u_N4Flag = FALSE;
    p_tm_post_data_payload->utc_model_glo_data.z_Mask.z_FilledMask.u_TaoGpsFlag = FALSE;

    p_tm_post_data_payload->utc_model_glo_data.z_Mask.z_FilledMask.u_NtFlag = TRUE;
    p_tm_post_data_payload->utc_model_glo_data.w_Nt = p_elem->gnss_UTC_Model.u.utcModel3->nA; // (1..1461) scale factor 1 day

    /* Notification of expected leap second correction. Present only GLONASS M satellites are present in GLONASS constellation */
    if ( p_elem->gnss_UTC_Model.u.utcModel3->m.kpPresent == 1 )
    {
      p_tm_post_data_payload->utc_model_glo_data.u_Kp = *(p_elem->gnss_UTC_Model.u.utcModel3->kp.data);
      p_tm_post_data_payload->utc_model_glo_data.u_Kp >>= 6;
      p_tm_post_data_payload->utc_model_glo_data.z_Mask.z_FilledMask.u_KpFlag = TRUE;
    }
    else
    {
      p_tm_post_data_payload->utc_model_glo_data.z_Mask.z_FilledMask.u_KpFlag = FALSE;
    }

    /* B1 and B2 are OPTIONAL. TM API & PE has no B1 or B2 */
    /* z_tm_post_data_payload.utc_model_glo_data.q_TaoGps = ? */
    /* tauC is time diff between UTC and GLONASS. Do not need update. Assume UTC to GPS offset is injected to MGP. */

    if ( FALSE == tm_post_data(q_protocol,
                               q_active_sess_handle,
                               TM_POST_DATA_TYPE_UTC_MODEL_GLO,
                               p_tm_post_data_payload
                              ) )
    {
      MSG_HIGH("TM_LPP(Glo) UTC to Tmcore data post failed", 0, 0, 0);
    }
    else
    {
      MSG_MED("TM_LPP(Glo): UTC posted to TmCore", 0, 0, 0);
    }
  } /* if utc model 3*/
  else
  {
    MSG_ERROR("Glo Time Model = %d not utcModel3", p_elem->gnss_UTC_Model.t, 0, 0);
  }
}

/*
 ******************************************************************************
 *
 * tm_lpp_cp_glo_time_model_proc
 *
 * Function description:
 *  This function processes the OSYS ASN.1 decoded LPP Assistance data for glo time model  
 *  and prepares such assistance data for injecting to MGP.
 *
 * Parameters:
 *  p_assist: pointer to the OSYS ASN.1 decoded LPP gpsAsssitance data.
 * 
 * Return value:
 *  TRUE  : Leap second provided
 *  FALSE : Leap second not provided
 *
 ******************************************************************************
*/
boolean tm_lpp_glo_time_model_proc( tm_prtl_type q_protocol,
                                    gnss_time_info *z_pos_time_stamp,
                                    tm_sess_handle_type q_active_sess_handle,
                                    const GNSS_GenericAssistDataElement *const p_elem,
                                    tm_post_data_payload_type *p_tm_post_data_payload )
{
  uint32  u_i     = 0;
  boolean ret_val = FALSE;

  for ( u_i = 0; u_i < p_elem->gnss_TimeModels.n; ++u_i )
  {
    if ( LPP_GNSS_TO_ID_GPS == p_elem->gnss_TimeModels.elem[u_i].gnss_TO_ID )
    {
      /* Init with default values */
      p_tm_post_data_payload->gnss_time_model_data.w_WeekNo = C_GNSS_GANSST_WEEK_UNKNOWN;
      p_tm_post_data_payload->gnss_time_model_data.x_A1 = C_GNSS_GANSST_A1_UNKNOWN;
      p_tm_post_data_payload->gnss_time_model_data.x_A2 = C_GNSS_GANSST_A2_UNKNOWN;
      p_tm_post_data_payload->gnss_time_model_data.w_DeltaT = C_GNSS_GANSST_DELTAT_UNKNOWN;

      /* A0 is a.k.a TauGPS */
      p_tm_post_data_payload->gnss_time_model_data.l_A0 = p_elem->gnss_TimeModels.elem[u_i].tA0;
      p_tm_post_data_payload->gnss_time_model_data.q_GanssTimeModelRefTime = p_elem->gnss_TimeModels.elem[u_i].gnss_TimeModelRefTime;
      p_tm_post_data_payload->gnss_time_model_data.u_GnssToID = C_GNSS_TO_ID_GPS;

      if ( TRUE == p_elem->gnss_TimeModels.elem[u_i].m.deltaTPresent )
      {
        /* deltaT is leap second for time difference between GPS to GNSS. 
        a.k.a GGTO - GPS to GNSS time offset. */
        p_tm_post_data_payload->gnss_time_model_data.w_DeltaT = p_elem->gnss_TimeModels.elem[u_i].deltaT;
        /* Convert to glonass time & set the z_set_assist_pos_time_stamp struct. Later used in injecting AA. */
        tm_util_convert_to_glonass_time(p_elem->gnss_TimeModels.elem[u_i].deltaT,
                                        z_pos_time_stamp);
        ret_val = TRUE;
      }
      if ( TRUE == p_elem->gnss_TimeModels.elem[u_i].m.weekNumberPresent )
      {
        p_tm_post_data_payload->gnss_time_model_data.w_WeekNo = p_elem->gnss_TimeModels.elem[u_i].weekNumber;
      }
      if ( TRUE == p_elem->gnss_TimeModels.elem[u_i].m.tA1Present )
      {
        p_tm_post_data_payload->gnss_time_model_data.x_A1 = p_elem->gnss_TimeModels.elem[u_i].tA1;
      }
      if ( TRUE == p_elem->gnss_TimeModels.elem[u_i].m.tA2Present )
      {
        p_tm_post_data_payload->gnss_time_model_data.x_A2 = p_elem->gnss_TimeModels.elem[u_i].tA2;
      }

      if ( FALSE == tm_post_data(q_protocol,
                                 q_active_sess_handle,
                                 TM_POST_DATA_TYPE_TIME_MODEL_GLO,
                                 p_tm_post_data_payload
                                ) )
      {
        MSG_HIGH("TM_LPP_UP: SUPL(Glo) TimeModel to Tmcore data post failed", 0, 0, 0);
      }
      else
      {
        MSG_MED("TM_LPP_UP: SUPL(Glo): TimeModel posted to TmCore", 0, 0, 0);
      }
    }
  }

  return ( ret_val );
}



/*
 ******************************************************************************
 *
 * tm_lpp_cp_glo_eph_proc
 *
 * Function description:
 *  This function injects any glonass eph that didn't have freq/SV ID association before.
 *
 * Parameters:
 *  p_assist: pointer to the OSYS ASN.1 decoded LPP gpsAsssitance data.
 * 
 * Return value:
 *
 ******************************************************************************
*/
void tm_lpp_glo_inj_eph( tm_prtl_type q_protocol,
                         prot_glo_eph_buffer_s_type z_prot_glo_eph_buf[N_GLO_SV],
                         tm_sess_handle_type q_active_sess_handle,
                         tm_post_data_payload_type *p_tm_post_data_payload )
{
  uint32 q_eph_injected_svs = 0;
  uint32 u_num_eph_injected = 0;
  uint32 i                  = 0;

  /* inject any Glonass SV eph. that didn't have freq.Num / svID association before */
  for ( i = 0; i < N_GLO_SV; ++i )
  {
    if ( (z_prot_glo_eph_buf[i].z_eph_glo_data.u_SvId > GLO_SLOTID_OFFSET) &&
         (z_prot_glo_eph_buf[i].z_eph_glo_data.u_SvId < (N_GLO_SV + GLO_SLOTID_OFFSET + 1)) &&
         (z_prot_glo_eph_buf[i].u_freq_num_valid == TRUE)
        )
    {

      /* inject this SV eph */
      memscpy(&p_tm_post_data_payload->eph_glo_data, sizeof(p_tm_post_data_payload->eph_glo_data),
              &z_prot_glo_eph_buf[i].z_eph_glo_data, sizeof(z_prot_glo_eph_buf[i].z_eph_glo_data));

      if ( FALSE == tm_post_data(q_protocol,
                                 q_active_sess_handle,
                                 TM_POST_DATA_TYPE_EPH_GLO,
                                 p_tm_post_data_payload
                                ) )
      {
        MSG_ERROR("TM_LPP_CP: CP(Glo): Eph. SV %u inj. to TmCore failed",
                  z_prot_glo_eph_buf[i].z_eph_glo_data.u_SvId, 0, 0);
      }
      else
      {
        u_num_eph_injected++;
        q_eph_injected_svs |=
                              1L << (z_prot_glo_eph_buf[i].z_eph_glo_data.u_SvId - GLO_SLOTID_OFFSET - 1);
      }

      /* reset these flags so that they won't get injected again later */
      z_prot_glo_eph_buf[i].z_eph_glo_data.u_SvId = 0;
      z_prot_glo_eph_buf[i].u_freq_num_valid = FALSE;
    }

  } /* For loop */

  if ( u_num_eph_injected != 0 )
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "TM_LPP(Glo): Eph posted to TmCore (when getting aux) 0x%lX, total SVs = %u",
          q_eph_injected_svs, u_num_eph_injected);
  }
}


/*
 ******************************************************************************
 * Function description:
 *  Convert the ASN.1 decoded LPP FTA assistance IE's to 
 *  the format ready for LOC_LTE message.
 *
 * Parameters:
 *
 *  p_ref_time : pointer to the structure containing the ref time ASN.1 
 *            decoded LPP FTA assistance data.
 *
 *
 * Return value:
 *
 *    None.
 *    
 *
 ******************************************************************************
*/
void tm_lpp_fta_post_loc_lte( GNSS_ReferenceTime *p_ref_time,
                              tm_post_data_payload_type *p_tm_post_data_payload )
{

  //FTA present;
  gnss_FTA_Information *p_fta_assist = &p_tm_post_data_payload->fta_assist_data;
  uint8                i,
                       j,
                       ndx           = 0;

  if ( p_ref_time->m.referenceTimeUncPresent == 1 )
  {
    p_fta_assist->m.referenceTimeUncPresent = 1;
    p_fta_assist->referenceTimeUnc = p_ref_time->referenceTimeUnc;
  }
  if ( p_ref_time->m.gnss_ReferenceTimeForCellsPresent == 1 )
  {
    p_fta_assist->m.gnss_ReferenceTimeForCellsPresent = 1;
    p_fta_assist->fta_ReferenceTimeForCells.n = p_ref_time->gnss_ReferenceTimeForCells.n;
    for ( i = 0; i < p_fta_assist->fta_ReferenceTimeForCells.n; i++ )
    {
      p_fta_assist->fta_ReferenceTimeForCells.elem[ndx].m.bsAlignPresent = p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].m.bsAlignPresent;
      p_fta_assist->fta_ReferenceTimeForCells.elem[ndx].bsAlign =  p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].bsAlign;
      p_fta_assist->fta_ReferenceTimeForCells.elem[ndx].referenceTimeUnc = p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].referenceTimeUnc;
      p_fta_assist->fta_ReferenceTimeForCells.elem[ndx].networkTime.m.frameDriftPresent = p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.m.frameDriftPresent;
      p_fta_assist->fta_ReferenceTimeForCells.elem[ndx].networkTime.cellID.t = p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.cellID.t;

      if ( p_fta_assist->fta_ReferenceTimeForCells.elem[ndx].networkTime.cellID.t == GNSS_FTA_NETWORKTIME_CELLID_EUTRA )
      {
        /*
        * RG : earfcn_v9a0 is used when 'earfcn isn't set to 65535 & earfcn_v9a0 is present' (or)              
        *      'earfcn is set to 65535 & earfcn_v9a0 is present'.
        *                                                                                        
        *     earfcn is used only when earfcn isn't 65535 & earfcn_v9a0 isn't present.
        * 
        *     UE does not use the frequency to identify the cell for gnss-ReferenceTimeForCells if
        *     earfcn is set to 65536 but earfcn_v9a0 isn't present.
        */
#ifdef FEATURE_GNSS_LPP_DOPPLER_CONF_EARFCN
        if ( 1 == p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA->m.earfcn_v9a0Present )
        {
          /* Spec : earfcn-v9a0 (65536..262143) */
          p_fta_assist->fta_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA.earfcn = p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA->earfcn_v9a0;
        }
        else
#endif /* FEATURE_GNSS_LPP_DOPPLER_CONF_EARFCN */
          if ( p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA->earfcn != ASN1V_maxEARFCN )
          {
            /* Spec : earfcn (0..65535) */
            p_fta_assist->fta_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA.earfcn = p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA->earfcn;
          }
          else
          {
            MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Invalid earfcn number %u, %d for pyhcellid %u",
                  p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA->earfcn,
                  p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA->m.earfcn_v9a0Present,
                  p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA->physCellId);
            continue; /* Skip this cell due to invalid earfcn number */
          }

        p_fta_assist->fta_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA.m.cellGlobalIdEUTRAPresent = p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA->m.cellGlobalIdEUTRAPresent;
        if ( p_fta_assist->fta_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA.m.cellGlobalIdEUTRAPresent == 1 )
        {
          if ( p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA->cellGlobalIdEUTRA.cellIdentity.t == 1 )
          {
            p_fta_assist->fta_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA.cellGlobalIdEUTRA.cellidentity =
                                                                                                                          tm_lpp_osys_asn1_bitstring_to_int(sizeof(uint32),
                                                                                                                                                            p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA->cellGlobalIdEUTRA.cellIdentity.u.eutra);
            p_fta_assist->fta_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA.cellGlobalIdEUTRA.cell_id_numbits =
                                                                                                                             p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA->cellGlobalIdEUTRA.cellIdentity.u.eutra->numbits;
          }


          if ( p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA->cellGlobalIdEUTRA.plmn_Identity.mnc.n > 0 )
          {
            for ( j = 0; j < p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA->cellGlobalIdEUTRA.plmn_Identity.mnc.n; j++ )
            {
              p_fta_assist->fta_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA.cellGlobalIdEUTRA.mnc[j] =
                                                                                                                      p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA->cellGlobalIdEUTRA.plmn_Identity.mnc.elem[j];
            }
            p_fta_assist->fta_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA.cellGlobalIdEUTRA.num_mnc_digits = p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA->cellGlobalIdEUTRA.plmn_Identity.mnc.n;

          }

          if ( p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA->cellGlobalIdEUTRA.plmn_Identity.mcc.n > 0 )
          {
            for ( j = 0; j < p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA->cellGlobalIdEUTRA.plmn_Identity.mcc.n; j++ )
            {
              p_fta_assist->fta_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA.cellGlobalIdEUTRA.mcc[j] =
                                                                                                                      p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA->cellGlobalIdEUTRA.plmn_Identity.mcc.elem[j];
            }
          }
        }
        p_fta_assist->fta_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA.physCellId = (uint16)
                                                                                                  p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.cellID.u.eUTRA->physCellId;
      } /* networktime cell id is eutra*/

      p_fta_assist->fta_ReferenceTimeForCells.elem[ndx].networkTime.m.frameDriftPresent = p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.m.frameDriftPresent;
      if ( p_fta_assist->fta_ReferenceTimeForCells.elem[ndx].networkTime.m.frameDriftPresent == 1 )
      {
        p_fta_assist->fta_ReferenceTimeForCells.elem[ndx].networkTime.frameDrift = p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.frameDrift;
      }
      p_fta_assist->fta_ReferenceTimeForCells.elem[ndx].networkTime.fractionalSecondsFromFrameStructureStart = p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.fractionalSecondsFromFrameStructureStart;
      p_fta_assist->fta_ReferenceTimeForCells.elem[ndx].networkTime.secondsFromFrameStructureStart = p_ref_time->gnss_ReferenceTimeForCells.elem[ndx].networkTime.secondsFromFrameStructureStart;

      ndx++; /* increment the FTA index */
    } /* end of for */
  }
}

/*
 ******************************************************************************
 * Function description:
 *  Function to process 'Force LTE hybrid fix' command
 *
 * Parameters:
 *
 *  b_sub_cmd : flag to enable/disable feature.
 *
 *
 * Return value:
 *
 *    None.
 *
 *
 ******************************************************************************
*/
void tm_lpp_proc_force_lte_hybrid_fix_cmd
(
  uint8 b_sub_cmd
 )
{
  if ( 0 == b_sub_cmd )
  {
    b_force_lte_hybrid = FALSE;
  }
  else
  {
    b_force_lte_hybrid = TRUE;
  }
}

/*
 ******************************************************************************
 *
 * tm_lpp_utils_microsec_to_GADK
 *
 * Function description:
 * This is obtained from the LPP spec V9.9.0
 * The uncertainty r, expressed in microseconds, is mapped to a number K, with the following formula:
                        r = C*(((1+x)K)-1)
 * with C = 0.5 and x = 0.14. To encode any higher value of uncertainty than that corresponding in the above
 * formula to K=127, the same value, K=127, shall also be used. The uncertainty is then coded on 7 bits, as the
 * binary encoding of K. Examples of gnss-TOD-unc value are as in the table Value of K to Value of uncertainty
 * relation below.
 *
 * Parameters:
 *
 *   q_time_unc_microseconds - Time uncertainity in microseconds 
 *
 * Return value: 
 *
 *   K value, which has a range 0 <= K <= 127;
 *
 ******************************************************************************
*/

uint8 tm_lpp_utils_microsec_to_GADK( float f_time_unc_microseconds )
{
  uint8              u_KValue,
                     u_Offset;
  uint32             i;
  static const float *p_GadKUncMap;

  p_GadKUncMap = f_GadKTimeUncMapNewLpp;

  u_Offset = (128 / 4);
  u_KValue = (128 / 2);

  /* Perform binary search of 128 possible K values. 
   * 6 compares here, followed by one after the for loop.  */
  for ( i = 6; i; i-- )
  {
    if ( u_KValue < 128 )
    {
      if ( f_time_unc_microseconds > p_GadKUncMap[u_KValue] )
        u_KValue += u_Offset;
      else
        u_KValue -= u_Offset;

      u_Offset >>= 1;
    }
  }

  /* Final check; bump up if not equal to cover unc region */
  if ( f_time_unc_microseconds > p_GadKUncMap[u_KValue] )
    u_KValue += 1;


  if ( u_KValue < 1 )
    u_KValue = 1;   /* Min value is 1 */
  else if ( u_KValue > 127 )
    u_KValue = 127; /* Max value is 127 */

  return ( u_KValue );
}

/*
 ******************************************************************************
 *
 * tm_lpp_cp_proc_otdoa_stop_req
 *
 * Function description:
 *  This function would send a request to TMCore to stop the OTDOA engine.
 *
 * Parameters:
 *  sess_handle: LPP UP or CP session type.
 *  reason     : Reason to stop the session.
 *  prtl_type  : Protocol module type invoking this call.
 * 
 * Return value:
 *  None.
 *
 ******************************************************************************
*/
void tm_lpp_proc_otdoa_stop_req( tm_sess_handle_type sess_handle, tm_sess_stop_reason_e_type const reason, 
                                 tm_prtl_type prtl_type )
{
  tm_post_data_payload_type z_tm_post_data_payload;

  /* Send request to stop the OTDOA engine */
  z_tm_post_data_payload.otdoa_stop.e_stop_reason = reason;
  if ( tm_post_data( prtl_type,
                     sess_handle,
                     TM_POST_DATA_TYPE_OTDOA_STOP,
                     &z_tm_post_data_payload
                    ) == FALSE
      )
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_MED, "OTDOA engine stop request failed" );
  }
  else
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_MED, "OTDOA engine stop request sent" );
  }
}
