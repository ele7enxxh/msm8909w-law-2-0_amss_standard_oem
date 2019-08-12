/*!
  @file
  wwan_coex_list.c

  @brief
  This file implements the desense functions of LIMTSMGR COEX module.

  @ingroup per_implementation
*/

/*=============================================================================

  Copyright (c) 2013-2016 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/wwan_coex/src/wwan_coex_list.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/25/16   jm      F3 Reduction
11/04/14   ag      Added TTL markers for profiling
10/09/14   ag      Use backoff for L/T aggr and blank for G aggr in DR-DSDS mode
10/08/14   jm      Utilize new trm api to retrieve chain info
10/06/14   jm      Proper blacklist generation for TDS
09/10/14   jm      Band avoidance support for 1X/HDR/WCDMA/TDSCDMA
09/09/14   rj      Action as Backoff for L/T+G scenarios in addition to SGLTE/TDS
07/30/14   sg      Enabled filter path and backoff for DR-DSDS mode
07/22/14   rj      Cleanup FilterPath related BOLT feature in Lmtsmgr
06/23/14   sg      desense indication mechanism
05/06/14   jm      KW Warning Fixes
05/05/14   rj      Filter Path support to send GSM Tx specific flag info to RF
04/28/14   jm      L+G Band Avoidance support
04/19/14   rj      1X-SLTE support added 
04/07/14   ag      Simultaneous capacity in WWAN coexistence is set to FALSE when 
                   not in SGLTE mode or when roaming mode is not set
03/25/14   ag      Handling GPS coexistence in SVLTE mode
03/20/14   jm      Include filter path for additional LTE/TDS bands
03/19/14   jm      L+G Blanking Support
03/17/14   jm      WWAN coex based off of RF device assignment
03/15/14   jm      Add band guard to all C+G combos
03/10/14   ag      Set action for T+G based on CM capability
02/10/14   ag      Mitigation action for SGLTE+G DSDA
02/05/14   jm      Slot level backoff support for SGLTE/SGTDS
11/17/13   ag      Fix for HDR-GSM IM check
11/07/13   ag      Add support for guard band for IM and harmonics.
10/11/13   ag      Binary search for lookup and caching the last check
09/26/13   ag      Added GSM aggressor blocking cases
08/22/13   ag      Added new IM combinations for SVLTE L B3 and 1X BC0
07/19/13   ag      Adding channel condition for L/T B39 and G B1800
06/13/13   rj      SVLTE/SVDO Coex for Triton/DI2.0
05/05/13   ag      Moved RF band enums to SYS band enums
04/06/13   ag      Support for DSDA WWAN Coexistence
03/07/13   rj      Handling new MSGR for LTE BW, Rx freq and sys band to RF band mapping
02/20/13   rj      Remove CW issues 
02/04/13   rj      Migrate SVDO/SVLTE SAR/IM3 support from RF SW to MCS
01/10/12   rj      Fixed Compilation error
06/30/12   ag      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "mcs_variation.h"
#include "comdef.h"
#include "cxm.h"
#include "lmtsmgr_task.h"
#include "lmtsmgr_i.h"
#include "lmtsmgr_msgr.h"
#include "sys.h"              /* for Sys band and channel enums */
#include "stringl.h"
#ifdef FEATURE_LTE
#include "rflte_core_txpl.h"  /* for retrieving LTE NRB */
#include "rflte_ext_mc.h"     /* for rflte_core functions */
#endif /* LTE */
#ifdef FEATURE_CDMA
#include "rfm_cdma.h"         /* for retrieving 1X/HDR Tx frequencies  */
#endif
#include "wwan_coex.h"


/*=============================================================================

                            TYPEDEFS

=============================================================================*/
#define MAX_IM_COMBOS_PER_TYPE 4
#define WWCOEX_GUARD_BAND 0
#define IM_DESENSE_MASK ((1<<IM_TYPE1_DESENSE)|(1<<IM_TYPE2_DESENSE))
/* Max Techs Guard band should be */
#define MAX_NUM_TECHS_GUARD_BAND 2

/* List size of supported LTE/TDS bands for coex */
#define WWCOEX_LTE_BAND_LIST_SIZE 7
#define WWCOEX_TDS_BAND_LIST_SIZE 3

/* Maximum tech/band desense combinations currently supported */

#ifdef FEATURE_LTE
#define WWAN_COEX_MAX_COMBS 70
#else
#define  WWAN_COEX_MAX_COMBS 36
#endif

#define WWCOEX_TECH_BAND_MASK_WO_VBAND 0xFF00FF
#define WWCOEX_TECH_BAND_MASK_WO_ABAND 0xFFFF00

/* Used in determining frequency range */
#define WWCOEX_MAX_FREQ 0xFFFFFFFF

#define LMTSMGR_IS_LTE_FILTER_PATH_BAND(band) ( ((band) == (uint32)SYS_BAND_LTE_EUTRAN_BAND41) ||\
                                                ((band) == (uint32)SYS_BAND_LTE_EUTRAN_BAND40) ||\
                                                ((band) == (uint32)SYS_BAND_LTE_EUTRAN_BAND39) ||\
                                                ((band) == (uint32)SYS_BAND_LTE_EUTRAN_BAND38) ||\
                                                ((band) == (uint32)SYS_BAND_LTE_EUTRAN_BAND34))

#define LMTSMGR_IS_TDS_FILTER_PATH_BAND(band) ( ((band) == (uint32)SYS_BAND_TDS_BANDE) ||\
                                                ((band) == (uint32)SYS_BAND_TDS_BANDF) ||\
                                                ((band) == (uint32)SYS_BAND_TDS_BANDA))


/* Function Type that checks channel combinations for a particular desense
   tech/band combination */
typedef wwan_coex_desense_type (*wwcoex_list_check_channels)
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);

#ifndef MCS_LIMITS_LOW_MEMORY

/* Function declaration for the different desense channel compares */

static wwan_coex_desense_type wwcoex_list_tds_b39_gsm_b1800
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);

static wwan_coex_desense_type wwcoex_list_gsm_victim_tds_aggr
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);


static wwan_coex_desense_type wwcoex_list_hdr_b0_1x_b0
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);

static wwan_coex_desense_type wwcoex_list_hdr_b1_1x_b1
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);

static wwan_coex_desense_type wwcoex_list_hdr_b0_1x_b10
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);

static wwan_coex_desense_type wwcoex_list_1x_b0_hdr_b10
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);

static wwan_coex_desense_type wwcoex_list_hdr_b10_1x_b10
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);

#ifdef FEATURE_LTE
static wwan_coex_desense_type wwcoex_list_gsm_victim_lte_aggr
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);

static wwan_coex_desense_type wwcoex_list_lte_b39_gsm_b1800
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);

static wwan_coex_desense_type wwcoex_list_gsm_b900_lte_b7
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);

static wwan_coex_desense_type wwcoex_list_lte_b25_1x_b1
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);

static wwan_coex_desense_type wwcoex_list_lte_b13_1x_b0
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);

static wwan_coex_desense_type wwcoex_list_lte_b2_1x_b1
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);

static wwan_coex_desense_type wwcoex_list_1x_b15_lte_b2
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);

static wwan_coex_desense_type wwcoex_list_lte_b4_1x_b0
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);

static wwan_coex_desense_type wwcoex_list_lte_b25_1x_b15
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);

static wwan_coex_desense_type wwcoex_list_1x_b15_lte_b25
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);
static wwan_coex_desense_type wwcoex_list_lte_b4_1x_b15
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);
static wwan_coex_desense_type wwcoex_list_1x_b15_lte_b4
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);
static wwan_coex_desense_type wwcoex_list_lte_b5_1x_b0
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);
static wwan_coex_desense_type wwcoex_list_1x_b0_lte_b5
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);
static wwan_coex_desense_type wwcoex_list_lte_b5_1x_b10
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);
static wwan_coex_desense_type wwcoex_list_1x_b15_lte_b5
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);
static wwan_coex_desense_type wwcoex_list_lte_b12_1x_b15
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);
static wwan_coex_desense_type wwcoex_list_lte_b17_1x_b15
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);

static wwan_coex_desense_type wwcoex_list_lte_1x_aggr_gps_victim
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);

static wwan_coex_desense_type wwcoex_list_lte_b3_1x_b0
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
);

#endif /* LTE */

#endif /* MCS_LIMITS_LOW_MEMORY */


typedef struct
{
  /* Aggressor harmonic index */
  uint8   aggr_hmonic_index;
  /* Victim harmonic index */
  uint8   victim_hmonic_index;
}wwcoex_desense_index_comb_type;

typedef struct
{
  wwcoex_desense_index_comb_type combs[MAX_IM_COMBOS_PER_TYPE];
  uint8 num_im_combos;
}wwcoex_im_comb_list_type;

typedef struct
{
  uint16 guard_band_tech_mask;
  uint32 guard_band;
}wwcoex_guard_band_info_type;

/* Structure used in the static desense table */
typedef struct
{
  /* Mask of aggr and victim bands:
     1st lower order 8 bits indicate aggr band,
     2nd lower order 8 bits indicate victim band,
     3rd lower order 4 bits indicate aggr tech,
     4th lower order 4 bits indicate victim tech */
  uint32          tech_band_mask;

  /* Aggressor Tech and Band */
  cxm_tech_type   atech;
  uint32          aband;

  /* Victim Tech and Band */
  cxm_tech_type   vtech;
  uint32          vband;

  /* Function that compares channels of aggressor and victim */
  wwcoex_list_check_channels comp_channel_func;


  /* Mask of all types of desense expected for this tech-band combination */
  uint32         desense_mask;

  /* m and n combinations for IM desense */
  wwcoex_im_comb_list_type   imtype1_list;
  wwcoex_im_comb_list_type   imtype2_list;

  /* For harmonic desense, what max 'n' should we use for this band combination */
  uint8          max_n_for_hmonics;

  /* Mask of all techs which of desense expected for this tech-band combination */
  wwcoex_guard_band_info_type         guard[MAX_NUM_TECHS_GUARD_BAND];

  /* Mask of all types of enhanced actions expected for this tech-band combo */
  uint32         enhanced_action_mask;
} wwcoex_list_comb_rec_type;

/* Cache structure for desense check */
typedef struct
{
   /* possible desense types for tech pair */
   uint32      desense_type;
   /* cache for thee last match */
   uint32      cached_index;

   uint32      aband;
   uint32      vband;
}wwcoex_tech_pair_desense_type;

uint32 wwcoex_search_list
(
  cxm_tech_type vtech,
  cxm_tech_type atech,
  uint32  vband,
  uint32 aband
);
boolean wwcoex_check_im
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info,
  const wwcoex_im_comb_list_type* im_list,
  uint32 guard_band
);

#ifndef MCS_LIMITS_LOW_MEMORY

static wwcoex_tech_pair_desense_type wwan_tech_pair_desense_data[CXM_TECH_MAX][CXM_TECH_MAX];

#endif /* MCS_LIMITS_LOW_MEMORY */

/*=============================================================================

                         INTERNAL VARIABLES

=============================================================================*/
/* Currently the coexistence channel combinations that warrant a
     desense are the following:
     
SGLTE/SGTDS Modes:
-------------------------------------------------------------------------------
| Sr |  Type  |           Aggressor            |            Victim            |
-------------------------------------------------------------------------------
| *  | Noise  | GSM2 Band 1800 Ch 512-736      | TDSCDMA Band 34/39 Ch Any    |
| *  | Noise  | TDSCDMA Band 34/39 Ch Any      | GSM2 Band 1800 Ch 512-736    |
| *  | Noise  | GSM2 Band 1800 Ch 512-736      | LTE Band 34/39 Ch Any        |
| *  | Noise  | LTE Band 34/39 Ch Any          | GSM2 Band 1800 Ch 512-736    |
| 1. | N+Block| TDSCDMA B39 Ch Any             | GSM2 Band 1800 Ch 737-885    |
| 5. | N+Block| LTE B39 Ch Any                 | GSM2 Band 1800 Ch 737-885    |
| 6. | Noise  | GSM2 Band 900 Ch Ngsm in eq    | LTE Band 7 Ch Nlte in eq     |
------------------------------------------------------------------------------- 

SHDR/SVLTE Modes:
-------------------------------------------------------------------------------
| Sr |  Type  |           Aggressor            |            Victim            |
-------------------------------------------------------------------------------
| 2. | IM3    | HDR Band 0 Ch Nhdr in eq       | 1X Band 0 Ch N1x in eq       | 
| 3. | IM3    | HDR Band 1 Ch Nhdr in eq       | 1X Band 1 Ch N1x in eq       | 
| 4. | IM3    | HDR Band 0 Ch Nhdr in eq       | 1X Band 10 Ch N1x in eq      |
| 7. | IM3    | LTE Band 25 Ch Nlte in eq      | 1X Band 1 Ch N1x in eq       |
| 8. | IM3    | LTE Band 13 Ch Nlte in eq      | 1X Band 0 Ch N1x in eq       | 
| 9. | IM3    | LTE Band 2 Ch Nlte in eq       | 1X Band 1 Ch N1x in eq       | 
| 10 | IM3    | LTE Band 2 Ch Nlte in eq       | 1X Band 15 Ch N1x in eq      | 
| 11 | IM2    | LTE Band 4 Ch Nlte in eq       | 1X Band 0 Ch N1x in eq       | 
| 12 | IM3    | LTE Band 25 Ch Nlte in eq      | 1X Band 15 Ch N1x in eq      | 
| 13 | IM2    | 1X Band 15                     | LTE Band 25 Ch Any           |
| 14 | IM     | LTE Band 4 Ch Any              | 1X Band 15                   |
| 15 | IM2    | 1X Band 15                     | LTE Band 4 Ch Any            |
| 16 | IM     | LTE Band 5 Ch Any              | 1X Band 0                    |
| 17 | IM2    | 1X Band 0                      | LTE Band 5 Ch Any            |
| 18 | IM     | LTE Band 5 Ch Any              | 1X Band 10                   |
| 19 | IM2    | 1X Band 15                     | LTE Band 5 Ch Any            |
| 20 | Harm   | LTE Band 12                    | 1X Band 15                   |
| 21 | Harm   | LTE Band 17                    | 1X Band 15                   |
| 46 | IM     | 1X BC0                         | HDR BC10                     |
| 48 | IM     | HDR BC10                       | 1X BC0                       |
| 49 | IM2    | 1X BC0                         | LTE Band 3                   |
| 50 | IM1    | LTE Band 3                     | 1X BC 0                      |
| 60 | GPS IM | LTE B4, 1X BC1                 | GPS                          |
| 61 | GPS IM | LTE B3, 1X BC1                 | GPS                          |
| 62 | GPS IM | LTE B3, 1X BC14                | GPS                          |
| 63 | GPS IM | LTE B9, 1X BC1                 | GPS                          |
| 64 | GPS IM | LTE B9, 1X BC14                | GPS                          |
| 65 | GPS IM | LTE B10, 1X BC1                | GPS                          |
| 66 | GPS IM | LTE B10, 1X BC14               | GPS                          |
| 67 | GPS IM | LTE B2, 1X BC8                 | GPS                          |
| 68 | GPS IM | LTE B25, 1X BC8                | GPS                          |
| 69 | GPS IM | LTE B4, 1X BC14                | GPS                          |
| 70 | GPS IM | LTE B1, 1X BC15                | GPS                          |
------------------------------------------------------------------------------- 

DSDA Modes:
-------------------------------------------------------------------------------
| Sr |  Type  |           Aggressor            |            Victim            |
-------------------------------------------------------------------------------
| *  | Noise  | GSM any band                   | Any tech                     |
| 1. | N+Block| TDSCDMA B39 Ch Any             | GSM2 Band 1800 Ch Any        |
| 5. | N+Block| LTE B39 Ch Any                 | GSM2 Band 1800 Ch Any        |
| 22 | N+IM   | GSM Band 850                   | CDMA BC0                     |
| 23 | N+IM   | GSM Band 850                   | WCDMA Band 5                 |
| 24 | N+IM   | GSM Band 850                   | WCDMA Band 8                 |
| 25 | N+IM   | GSM Band 900                   | Wcdma Band 1                 |
| 26 | N+IM   | GSM Band 900                   | Wcdma Band 2                 | 
| 27 | N+IM   | GSM Band 900                   | CDMA  BC1                    |
| 28 | N+IM   | GSM Band 900                   | Wcdma Band 8                 |
| 29 | N+IM   | GSM Band 1800                  | Wcdma Band 5                 |
| 30 | N+IM   | GSM Band 1800                  | CDMA BC0                     |
| 31 | N+IM   | GSM Band 1800                  | CDMA BC1                     |
| 32 | N+IM   | GSM Band 1800                  | Wcdma Band 1                 |
| 33 | N+IM   | GSM Band 1800                  | Wcdma Band 2                 |
| 34 | N+IM   | GSM Band 1900                  | Wcdma Band 8                 |
| 35 | N+IM   | GSM Band 1900                  | Wcdma Band 1                 |
| 36 | N+IM   | GSM Band 1900                  | Wcdma Band 2                 |      
| 37 | N+IM   | GSM Band 1900                  | CDMA BC1                     |
| 38 | Block  | Wcdma Band 8                   | GSM Band 850                 |
| 39 | Block  | Wcdma Band 2                   | GSM Band 1800                |
| 40 | Block  | Wcdma Band 1                   | GSM Band 1900                |
| 41 | Block  | CDMA BC1                       | GSM Band 1800                |
| 42 | N+Block| GSM Band 900                   | GSM Band 850                 |
| 43 | N+Block| GSM Band 1900                  | GSM Band 1800                |
| 44 | N+Harm | GSM Band 900                   | GSM Band 1800                |
| 45 | Harm   | WCDMA Band 8                   | GSM Band 1800                |
| 48 | Noise  | TDSCDMA Band 34                | GSM Band 1800                |
| 51 | Block  | GSM Band 900                   | CDMA BC0                     |
| 52 | Block  | GSM Band 900                   | WCDMA Band 5                 |
| 53 | N+IM   | GSM Band 900                   | LTE Band 1                   |
| 54 | N+Harm | GSM Band 900                   | LTE Band 3                   |
| 55 | N+Harm | GSM Band 900                   | LTE Band 41                  |
| 56 | N+IM   | GSM Band 1800                  | LTE Band 1                   |
| 57 | N+IM   | GSM Band 1800                  | LTE Band 3                   |
| 58 | Noise  | LTE Band 1                     | GSM Band 1800                |
| 59 | Noise  | LTE Band 34                    | GSM Band 1800                |
------------------------------------------------------------------------------- 

N = Noise / RxBN
IM = Inter Modulation
Block = Blocking
Harm = Harmonic
*/

#ifndef MCS_LIMITS_LOW_MEMORY

static wwcoex_list_comb_rec_type wwcoex_list_comb_table[WWAN_COEX_MAX_COMBS]= 
{ 
  /* Sr 1: TDSCDMA B39 & GSM 1800 */
  {
    0,
    CXM_TECH_TDSCDMA,
    (uint32)SYS_BAND_TDS_BANDF,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_DCS_1800,
    wwcoex_list_tds_b39_gsm_b1800,
    (1<<NOISE_DESENSE)|(1<<BLOCKING_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (1<<ENH_CXM_SLOT_LEVEL)           /* Enhanced Action Mask */
  },

  /* Sr 2 HDR B0 & 1X B0 */
  {
    0,
    CXM_TECH_HDR,
    (uint32)SYS_BAND_BC0,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC0,
    wwcoex_list_hdr_b0_1x_b0,
    (1<<IM_TYPE1_DESENSE),
    {
      { {2,1}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      1                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 3: HDR B1 & 1X B1 */
  {
    0,
    CXM_TECH_HDR,
    (uint32)SYS_BAND_BC1,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC1,
    wwcoex_list_hdr_b1_1x_b1,
    (1<<IM_TYPE1_DESENSE),
    {
      { {2,1}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      1                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 4: HDR BC0 & 1X B10 */
  {
    0,
    CXM_TECH_HDR,
    (uint32)SYS_BAND_BC0,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC10,
    wwcoex_list_hdr_b0_1x_b10,
    (1<<IM_TYPE1_DESENSE),
    {
      { {2,1}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      1                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

#ifdef FEATURE_LTE

  /* Sr 5: LTE B39 & GSM 1800 */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND39,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_DCS_1800,
    wwcoex_list_lte_b39_gsm_b1800,
    (1<<NOISE_DESENSE)|(1<<BLOCKING_DESENSE), /* Noise for SGLTE, Blocking for L+G */
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (1<<ENH_CXM_SLOT_LEVEL)           /* Enhanced Action Mask */
  },

  /* Sr 6 GSM 900 & LTE B7 */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_EGSM_900,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND7,
    wwcoex_list_gsm_b900_lte_b7,
    (1<<NOISE_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 7: LTE B25 & 1X B1 */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND25,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC1,
    wwcoex_list_lte_b25_1x_b1,
    (1<<IM_TYPE1_DESENSE),
    {
      { {2,1}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      1                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 8: LTE B13 & 1x B0 */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND13,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC0,
    wwcoex_list_lte_b13_1x_b0,
    (1<<IM_TYPE1_DESENSE),
    {
      { {2,1}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      1                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 9: LTE B2 & 1x B1 */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND2,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC1,
    wwcoex_list_lte_b2_1x_b1,
    ((1<<IM_TYPE1_DESENSE)|(1<<NOISE_DESENSE)),
    {
      { {2,1}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      1                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },
  
  /* Sr 10: 1X B15 & LTE B2 */
  {
    0,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC15,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND2,
    wwcoex_list_1x_b15_lte_b2,
    (1<<IM_TYPE2_DESENSE),
    {
      { {2,1}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      1                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },
  
  /* Sr 11 LTE B4 & 1x B0 */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND4,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC0,
    wwcoex_list_lte_b4_1x_b0,
    ((1<<IM_TYPE1_DESENSE)|(1<<NOISE_DESENSE)),
    {
      { {2,1}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      1                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 12 LTE B25 & 1C B15 */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND25,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC15,
    wwcoex_list_lte_b25_1x_b15,
    (1<<IM_TYPE1_DESENSE),
    {
      { {2,1}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      1                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 13: 1X B15 & LTE B25 */
  {
    0,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC15,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND25,
    wwcoex_list_1x_b15_lte_b25,
    (1<<IM_TYPE2_DESENSE),
    {
      { {2,1}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      1                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },
  
  /* Sr 14: LTE B4 & 1X B15 */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND4,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC15,
    wwcoex_list_lte_b4_1x_b15,
    (1<<IM_TYPE1_DESENSE),
    {
      { {2,1}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      1                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },
  
  /* Sr 15: 1x B15 & LTE B4 */
  {
    0,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC15,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND4,
    wwcoex_list_1x_b15_lte_b4,
    (1<<IM_TYPE2_DESENSE),
    {
      { {2,1}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      1                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },
  
  /* Sr 16 LTE B5 & 1x B0 */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND5,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC0,
    wwcoex_list_lte_b5_1x_b0,
    (1<<IM_TYPE1_DESENSE),
    {
      { {2,1}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      1                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },
  
  /* Sr 17: 1X BC0 & LTE B5 */
  {
    0,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND5,
    wwcoex_list_1x_b0_lte_b5,
    (1<<IM_TYPE2_DESENSE),
    {
      { {2,1}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      1                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },
  
  /* Sr 18: LTE B5 & 1x B10 */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND5,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC10,
    wwcoex_list_lte_b5_1x_b10,
    (1<<IM_TYPE1_DESENSE),
    {
      { {2,1}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      1                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },
  
  /* Sr 19: 1x B15 & LTE B5 */
  {
    0,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC15,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND5,
    wwcoex_list_1x_b15_lte_b5,
    (1<<IM_TYPE2_DESENSE),
    {
      { {2,1}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      1                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },
  
  /* Sr 20: LTE B12 & 1X B15 */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND12,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC15,
    wwcoex_list_lte_b12_1x_b15,
    (1<<HARMONIC_DESENSE),
    {
      { {2,1}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      1                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },
  
  /* Sr 21: LTE B17 & 1X B15 */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND17,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC15,
    wwcoex_list_lte_b17_1x_b15,
    (1<<HARMONIC_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    3,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },
#endif /* LTE */ 

  /* Sr 22: GSM850 with C2K BC0 */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_850,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC0,
    NULL,
    (1<<NOISE_DESENSE)|(1<<IM_TYPE1_DESENSE)|(1<<IM_TYPE2_DESENSE),
    {
      { {2,1}, {3,2}, {4,3}, {0,0} },  /* IM type 1 desense harmonic list */
      3                                /* Num valid entries */ 
    },
    { 
      { {2,3}, {3,4}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      2                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {(1<<CXM_TECH_ONEX), 4200000}, {0,0} /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 23: GSM850 with WCDMA B5 */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_850,
    CXM_TECH_WCDMA,
    (uint32)SYS_BAND_WCDMA_V_850,
    NULL,
    (1<<NOISE_DESENSE)|(1<<IM_TYPE1_DESENSE)|(1<<IM_TYPE2_DESENSE),
    {
      { {2,1}, {3,2}, {4,3}, {0,0} },  /* IM type 1 desense harmonic list */
      3                                /* Num valid entries */ 
    },
    { 
      { {2,3}, {3,4}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      2                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 24: GSM850 with Wcdma B8 */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_850,
    CXM_TECH_WCDMA,
    (uint32)SYS_BAND_WCDMA_VIII_900,
    NULL,
    (1<<NOISE_DESENSE)|(1<<IM_TYPE2_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {1,2}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      1                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 25: GSM900 with Wcdma B1 */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_EGSM_900,
    CXM_TECH_WCDMA,
    (uint32)SYS_BAND_WCDMA_I_IMT_2000,
    NULL,
    (1<<NOISE_DESENSE)|(1<<IM_TYPE2_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {4,3}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      1                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 26: GSM900 with Wcdma B2 */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_EGSM_900,
    CXM_TECH_WCDMA,
    (uint32)SYS_BAND_WCDMA_II_PCS_1900,
    NULL,
    (1<<NOISE_DESENSE)|(1<<IM_TYPE2_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {4,3}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      1                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 27: GSM900 with C2K B1 */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_EGSM_900,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC1,
    NULL,
    (1<<NOISE_DESENSE)|(1<<IM_TYPE2_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {4,3}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      1                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {(1<<CXM_TECH_ONEX), 4200000}, {0,0} /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 28: GSM900 with Wcdma B8 */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_EGSM_900,
    CXM_TECH_WCDMA,
    (uint32)SYS_BAND_WCDMA_VIII_900,
    NULL,
    (1<<NOISE_DESENSE)|(1<<IM_TYPE1_DESENSE)|(1<<IM_TYPE2_DESENSE),
    {
      { {2,1}, {3,2}, {4,3}, {0,0} },  /* IM type 1 desense harmonic list */
      3                                /* Num valid entries */ 
    },
    { 
      { {2,3}, {3,4}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      2                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 29: GSM1800 with Wcdma B5 */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_DCS_1800,
    CXM_TECH_WCDMA,
    (uint32)SYS_BAND_WCDMA_V_850,
    NULL,
    (1<<NOISE_DESENSE)|(1<<IM_TYPE1_DESENSE),
    {
      { {1,1}, {2,3}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      2                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 30: GSM1800 with C2K B0 */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_DCS_1800,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC0,
    NULL,
    (1<<NOISE_DESENSE)|(1<<IM_TYPE1_DESENSE),
    {
      { {1,1}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      1                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {(1<<CXM_TECH_ONEX), 4200000 }, {0,0} /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 31: GSM1800 with C2K B1  */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_DCS_1800,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC1,
    NULL,
    (1<<NOISE_DESENSE)|(1<<IM_TYPE2_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {1,2}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      1                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {(1<<CXM_TECH_ONEX), 4200000}, {0,0} /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 32: GSM1800 with Wcdma B1 */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_DCS_1800,
    CXM_TECH_WCDMA,
    (uint32)SYS_BAND_WCDMA_I_IMT_2000,
    NULL,
    (1<<NOISE_DESENSE)|(1<<IM_TYPE2_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {1,2}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      1                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 33: GSM1800 with Wcdma B2 */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_DCS_1800,
    CXM_TECH_WCDMA,
    (uint32)SYS_BAND_WCDMA_II_PCS_1900,
    NULL,
    (1<<NOISE_DESENSE)|(1<<IM_TYPE2_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {1,2}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      1                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 34: GSM1900 with Wcdma B8 */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_PCS_1900,
    CXM_TECH_WCDMA,
    (uint32)SYS_BAND_WCDMA_VIII_900,
    NULL,
    (1<<NOISE_DESENSE)|(1<<IM_TYPE1_DESENSE),
    {
      { {1,1}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      1                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 35: GSM1900 with Wcdma B1 */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_PCS_1900,
    CXM_TECH_WCDMA,
    (uint32)SYS_BAND_WCDMA_I_IMT_2000,
    NULL,
    (1<<NOISE_DESENSE)|(1<<IM_TYPE2_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {2,3}, {3,4}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      2                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 36: GSM1900 with Wcdma B2  */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_PCS_1900,
    CXM_TECH_WCDMA,
    (uint32)SYS_BAND_WCDMA_II_PCS_1900,
    NULL,
    (1<<NOISE_DESENSE)|(1<<IM_TYPE1_DESENSE)|(1<<IM_TYPE2_DESENSE),
    {
      { {2,1}, {3,2}, {4,3}, {0,0} },  /* IM type 1 desense harmonic list */
      3                                /* Num valid entries */ 
    },
    { 
      { {2,3}, {3,4}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      2                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 37: GSM1900 with C2K B1  */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_PCS_1900,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC1,
    NULL,
    (1<<NOISE_DESENSE)|(1<<IM_TYPE1_DESENSE)|(1<<IM_TYPE2_DESENSE),
    {
      { {2,1}, {3,2}, {4,3}, {0,0} },  /* IM type 1 desense harmonic list */
      3                                /* Num valid entries */ 
    },
    { 
      { {2,3}, {3,4}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      2                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {(1<<CXM_TECH_ONEX), 4200000}, {0,0} /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 38: Wcdma B8 with GSM 850  */
  {
    0,
    CXM_TECH_WCDMA,
    (uint32)SYS_BAND_WCDMA_VIII_900,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_850,
    NULL,
    (1<<BLOCKING_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 39: Wcdma B2 with GSM 1800  */
  {
    0,
    CXM_TECH_WCDMA,
    (uint32)SYS_BAND_WCDMA_II_PCS_1900,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_DCS_1800,
    NULL,
    (1<<BLOCKING_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 40: Wcdma B1 with GSM 1900 */
  {
    0,
    CXM_TECH_WCDMA,
    (uint32)SYS_BAND_WCDMA_I_IMT_2000,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_PCS_1900,
    NULL,
    (1<<BLOCKING_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 41: C2K B1 with GSM 1800 */
  {
    0,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC1,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_DCS_1800,
    NULL,
    (1<<BLOCKING_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 42: GSM B900 with GSM 850 */
  {
    0,
    CXM_TECH_GSM1,
    (uint32)SYS_BAND_GSM_EGSM_900,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_850,
    NULL,
    (1<<NOISE_DESENSE)|(1<<BLOCKING_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 43: GSM B1900 with GSM1800 */
  {
    0,
    CXM_TECH_GSM1,
    (uint32)SYS_BAND_GSM_PCS_1900,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_DCS_1800,
    NULL,
    (1<<NOISE_DESENSE)|(1<<BLOCKING_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 44: GSM B900 with GSM1800 */
  {
    0,
    CXM_TECH_GSM1,
    (uint32)SYS_BAND_GSM_EGSM_900,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_DCS_1800,
    NULL,
    (1<<NOISE_DESENSE)|(1<<HARMONIC_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    2,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 45: WCDMA B8 with GSM1800 */
  {
    0,
    CXM_TECH_WCDMA,
    (uint32)SYS_BAND_WCDMA_VIII_900,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_DCS_1800,
    NULL,
    (1<<HARMONIC_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    2,                                 /* # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 46: 1X B0 & HDR B0 */
  {
    0,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC0,
    CXM_TECH_HDR,
    (uint32)SYS_BAND_BC10,
    wwcoex_list_1x_b0_hdr_b10,
    (1<<IM_TYPE2_DESENSE),
    {
      { {2,1}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      1                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },
  
  /* Sr 47: HDR BC10 & 1X BC0 */
  {
    0,
    CXM_TECH_HDR,
    (uint32)SYS_BAND_BC10,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC10,
    wwcoex_list_hdr_b10_1x_b10,
    (1<<IM_TYPE1_DESENSE)|(1<<IM_TYPE2_DESENSE),
    {
      { {2,1}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      1                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 48: TDS B34 and GSM B1800 */
  {
    0,
    CXM_TECH_TDSCDMA,
    (uint32)SYS_BAND_TDS_BANDA,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_DCS_1800,
    wwcoex_list_gsm_victim_tds_aggr,
    (1<<NOISE_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

#ifdef FEATURE_LTE
  /* Sr 49: 1X BC0 and LTE B3 */
  {
    0,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND3,
    NULL,
    (1<<IM_TYPE2_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {2,2}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      1                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },
  
  /* Sr 50: LTE B3 and 1X BC0 */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND3,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC0,
    wwcoex_list_lte_b3_1x_b0,
    ((1<<IM_TYPE1_DESENSE)|(1<<NOISE_DESENSE)),
    {
      { {1,1}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      1                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

#endif
  /* Sr 51: GSM B900 and C2K BC0 */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_EGSM_900,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC0,
    NULL,
    (1<<BLOCKING_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 52: GSM B900 and W B5 */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_EGSM_900,
    CXM_TECH_WCDMA,
    (uint32)SYS_BAND_WCDMA_V_850,
    NULL,
    (1<<BLOCKING_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
	(ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

#ifdef FEATURE_LTE
  /* Sr 53: GSM B900 and LTE B1 */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_EGSM_900,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND1,
    NULL,
    (1<<NOISE_DESENSE)|(1<<IM_TYPE2_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {4,3}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      1                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
	(ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 54: GSM B900 and LTE B3 */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_EGSM_900,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND3,
    NULL,
    (1<<NOISE_DESENSE)|(1<<HARMONIC_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    2,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
	(ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 55: GSM B900 and LTE B41 */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_EGSM_900,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND41,
    NULL,
    (1<<NOISE_DESENSE)|(1<<HARMONIC_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    3,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
	(ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 56: GSM B1800 and LTE B1 */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_DCS_1800,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND1,
    NULL,
    (1<<NOISE_DESENSE)|(1<<IM_TYPE2_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {1,2}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      1                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
	(ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 57: GSM B1800 and LTE B3 */
  {
    0,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_DCS_1800,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND3,
    NULL,
    (1<<NOISE_DESENSE)|(1<<IM_TYPE1_DESENSE)|(1<<IM_TYPE2_DESENSE),
    {
      { {2,1}, {3,2}, {4,3}, {0,0} },  /* IM type 1 desense harmonic list */
      3                                /* Num valid entries */ 
    },
    { 
      { {2,3}, {3,4}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      2                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
	(ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 58: LTE B1 & GSM B1800 */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND1,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_DCS_1800,
    wwcoex_list_gsm_victim_lte_aggr,
    (1<<NOISE_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
	(ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },
  
  /* Sr 59: LTE B34 & GSM B1800 */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND34,
    CXM_TECH_GSM2,
    (uint32)SYS_BAND_GSM_DCS_1800,
    wwcoex_list_gsm_victim_lte_aggr,
    (1<<NOISE_DESENSE),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
	(ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },
  /* Sr 60: LTE B4, 1X BC1 and GPS  */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND4,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC1,
    wwcoex_list_lte_1x_aggr_gps_victim,
    (1<<IM_DESENSE_GPS),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */ 
  },

  /* Sr 61: LTE B3, 1X BC1 and GPS */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND3,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC1,
    wwcoex_list_lte_1x_aggr_gps_victim,
    (1<<IM_DESENSE_GPS),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */ 
  },

  /* Sr 62: LTE B3, 1X BC14 and GPS */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND3,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC14,
    wwcoex_list_lte_1x_aggr_gps_victim,
    (1<<IM_DESENSE_GPS),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */ 
  },

  /* Sr 63: LTE B9, 1X BC1 and GPS */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND9,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC1,
    wwcoex_list_lte_1x_aggr_gps_victim,
    (1<<IM_DESENSE_GPS),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 64: LTE B9, 1X BC14 and GPS */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND9,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC14,
    wwcoex_list_lte_1x_aggr_gps_victim,
    (1<<IM_DESENSE_GPS),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 65: LTE B10, 1X BC1 and GPS */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND10,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC1,
    wwcoex_list_lte_1x_aggr_gps_victim,
    (1<<IM_DESENSE_GPS),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 66: LTE B10, 1X BC14 and GPS */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND10,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC14,
    wwcoex_list_lte_1x_aggr_gps_victim,
    (1<<IM_DESENSE_GPS),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 67: LTE B2, 1X BC8 and GPS */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND2,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC8,
    wwcoex_list_lte_1x_aggr_gps_victim,
    (1<<IM_DESENSE_GPS),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 68: LTE B25, 1X BC8 and GPS */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND25,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC8,
    wwcoex_list_lte_1x_aggr_gps_victim,
    (1<<IM_DESENSE_GPS),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 69: LTE B4, 1X BC14 and GPS */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND4,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC14,
    wwcoex_list_lte_1x_aggr_gps_victim,
    (1<<IM_DESENSE_GPS),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  },

  /* Sr 70: LTE B1, 1X BC15 and GPS */
  {
    0,
    CXM_TECH_LTE,
    (uint32)SYS_BAND_LTE_EUTRAN_BAND1,
    CXM_TECH_ONEX,
    (uint32)SYS_BAND_BC15,
    wwcoex_list_lte_1x_aggr_gps_victim,
    (1<<IM_DESENSE_GPS),
    {
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM type 1 desense harmonic list */
      0                                /* Num valid entries */ 
    },
    { 
      { {0,0}, {0,0}, {0,0}, {0,0} },  /* IM Type 2 desense harmonic list */
      0                                /* Num valid entries */   
    },
    0,                                 /* Max # harmonic desense... */
    {
      {0,0}, {0,0}                     /* tech1, tech2 mask and Guard band value */
    },
    (ENH_CXM_NONE)                     /* Enhanced Action Mask */
  }
#endif /* FEATURE_LTE */
};

#endif /* MCS_LIMITS_LOW_MEMORY */


/*=============================================================================

                                FUNCTIONS

=============================================================================*/

#ifndef MCS_LIMITS_LOW_MEMORY

#ifdef FEATURE_LTE
/*===========================================================================
FUNCTION wwcoex_check_lte_1x_im3_in_lte_rx_band                              

DESCRIPTION
        This API will calculate the IM3 Frequency Range  and check if it falls on 
the LTE Rx band.

DEPENDENCIES 
  None

RETURN VALUE  
  TRUE or FALSE

SIDE EFFECTS
  None
  
===========================================================================*/

static boolean wwcoex_check_lte_1x_im3_in_lte_rx_band
(
  uint32 fc_1xtx,
  uint32 fc_ltetx,
  uint32 fc_lterx,
  uint32 bw_1x,
  uint32 bw_lte
)
{
  uint32 fc_im3;
  uint32 fl_im3;
  uint32 fh_im3;
  uint32 bw_im3;
  uint32 fl_lterx;
  uint32 fh_lterx;
  boolean return_status = FALSE;

//Calcualte IM3 Frequency and BW 

  fc_im3 = 2*fc_ltetx - fc_1xtx;  
  bw_im3 = 2*bw_lte + bw_1x; 
 
// Calculate IM3 Frequency range

  fl_im3 = fc_im3 - bw_im3/2;
  fh_im3 = fc_im3 + bw_im3/2;

// Calculate LTE DL Frequency range

  fl_lterx = fc_lterx - bw_lte/2;
  fh_lterx = fc_lterx + bw_lte/2;

  LMTSMGR_MSG_4( LOW, "wwcoex LTE_rx_desense: fc_ltetx %d,fc_cdmatx %d,f1_im3 %d,f2_im3 %d ",
                       fc_ltetx,fc_1xtx,
                       fl_im3,fh_im3);


  if (fl_im3 <= fh_lterx && fh_im3 >= fl_lterx)
  {
    return_status = TRUE;
  }
  return return_status;
}

/*===========================================================================

FUNCTION wwcoex_check_lte_1x_im2_in_1x_rx_band                              

DESCRIPTION
        This API will calculate the IM2 Frequency Range  and check if it falls on 
the 1x RX channel

DEPENDENCIES 
  None

RETURN VALUE
  TRUE or FALSE 

SIDE EFFECTS
  None

===========================================================================*/

static boolean wwcoex_check_lte_1x_im2_in_1x_rx_band
(
  uint32 fc_ltetx, 
  uint32 lte_bw, 
  uint32 fc_cdmatx, 
  uint32 fc_cdmarx
)
{
  uint32 fc_im2;
  uint32 f1_im2;
  uint32 f2_im2;
  uint32 bw_im2;
  boolean return_status = FALSE;
  uint32 fc_cdmarx_lo = fc_cdmarx - 750;
  uint32 fc_cdmarx_ho = fc_cdmarx + 750;


//Calcualte IM2 Frequency and BW

  fc_im2 = fc_ltetx - fc_cdmatx;
  bw_im2 = lte_bw + 1250;


  f1_im2 = fc_im2 - bw_im2 / 2;
  f2_im2 = fc_im2 + bw_im2 / 2;

  LMTSMGR_MSG_4(LOW, "WWCOEX Im2 backoff:fc_ltetx %d,fc_cdmatx %d,f1_im3 %d,f2_im3 %d ", 
                     fc_ltetx, fc_cdmatx, f1_im2, f2_im2);


  if (fc_cdmarx_lo <= f2_im2 && f1_im2 <= fc_cdmarx_ho)
  {
    return_status = TRUE;
  }

  return return_status;
}


/*===========================================================================

FUNCTION wwcoex_check_lte_1x_im3_in_1x_rx_band                              

DESCRIPTION
        This API will calculate the IM3 Frequency Range  and check if it falls on 
the 1x Rx band.

DEPENDENCIES 
  None

RETURN VALUE 
  TRUE or FALSE 

SIDE EFFECTS
  None
  
===========================================================================*/

static boolean wwcoex_check_lte_1x_im3_in_1x_rx_band
(
  uint32 fc_1xtx,
  uint32 fc_1xrx,
  uint32 fc_ltetx,
  uint32 bw_1x,
  uint32 bw_lte
)
{
  uint32 fc_im3;
  uint32 fl_im3;
  uint32 fh_im3;
  uint32 bw_im3;
  uint32 fl_1xrx;
  uint32 fh_1xrx;
  boolean return_status = FALSE;

//Calcualte IM3 Frequency and BW 

  fc_im3 = 2*fc_ltetx - fc_1xtx;  
  bw_im3 = 2*bw_lte + bw_1x; 
 
// Calculate IM3 Frequency range

  fl_im3 = fc_im3 - bw_im3/2;
  fh_im3 = fc_im3 + bw_im3/2;

// Calculate 1x DL Frequency range

  fl_1xrx = fc_1xrx - bw_1x/2;
  fh_1xrx = fc_1xrx + bw_1x/2;

  LMTSMGR_MSG_4( LOW, "WWCOEX CDMA_1x_rx_desense: fc_ltetx %d,fc_cdmatx %d,f1_im3 %d,f2_im3 %d ",
                         fc_ltetx,fc_1xtx,fl_im3,fh_im3);


  if (fl_im3 <= fh_1xrx && fh_im3 >= fl_1xrx)
  {
    return_status = TRUE;
  }
  return return_status;
}
/*===========================================================================

FUNCTION wwcoex_check_lte_1x_im2_in_lte_rx_band                              

DESCRIPTION
        This API will calculate the IM2 Frequency Range  and check if it falls on 
the LTE RX channel

DEPENDENCIES 
  None

RETURN VALUE TRUE or FALSE 
  None

SIDE EFFECTS
  None

===========================================================================*/

static boolean wwcoex_check_lte_1x_im2_in_lte_rx_band ( uint32 fc_ltetx, uint32 fc_lterx, uint32 lte_bw, uint32 fc_cdmatx )
{
  uint32 fc_im2;
  uint32 fl_im2;
  uint32 fh_im2;
  uint32 bw_im2;
  boolean return_status = FALSE;
  uint32 fc_lterx_l = fc_lterx - lte_bw/2;
  uint32 fc_lterx_h = fc_lterx + lte_bw/2;
  

  //Calcualte IM2 Frequency and BW 

  fc_im2 = fc_cdmatx - fc_ltetx;  
  bw_im2 = lte_bw + 1250; 
 

  fl_im2 = fc_im2-bw_im2/2;
  fh_im2 = fc_im2+bw_im2/2;

  LMTSMGR_MSG_4( LOW, "WWCOEX CDMA_1x desense due to 2nd harmonic:fc_ltetx %d,fc_cdmatx %d,fl_im2 %d,fh_im2 %d ",
                     fc_ltetx,fc_cdmatx,fl_im2,fh_im2);


  if (fc_lterx_l <= fh_im2 && fl_im2 <= fc_lterx_h)
  {
    return_status = TRUE;
  }

  return return_status;
}

/*===========================================================================

FUNCTION wwcoex_check_lte_1x_3rd_harmonic_in_1x_rx_band                              

DESCRIPTION
        This API will calculate the 3RD HARMONIC Frequency Range and check if it falls on 
the 1x Rx band.

DEPENDENCIES 
  None

RETURN VALUE TRUE or FALSE 
  None

SIDE EFFECTS
  None
  
===========================================================================*/

static boolean wwcoex_check_lte_1x_3rd_harmonic_in_1x_rx_band( uint32 fc_1xrx,uint32 fc_ltetx,uint32 bw_1x,uint32 bw_lte)
{
  uint32 fc_3rd;
  uint32 fl_3rd;
  uint32 fh_3rd;
  uint32 bw_3rd;
  uint32 fl_1xrx;
  uint32 fh_1xrx;
  boolean return_status = FALSE;

//Calcualte 3rd Harmonic Frequency and BW

  fc_3rd = 3*fc_ltetx;  
  bw_3rd = 3*bw_lte; 
 
// Calculate 3rd Harmonic Frequency range

  fl_3rd = fc_3rd - bw_3rd/2;
  fh_3rd = fc_3rd + bw_3rd/2;

// Calculate 1x DL Frequency range

  fl_1xrx = fc_1xrx - bw_1x/2;
  fh_1xrx = fc_1xrx + bw_1x/2;

  LMTSMGR_MSG_4( LOW, "WWCOEX CDMA_1x_rx_desense due to 3rd harmonic: fc_ltetx %d,fc_cdmarx %d,fl_3rd %d,fh_3rd %d ",
                           fc_ltetx, fc_1xrx, fl_3rd, fh_3rd);


  if (fl_3rd <= fh_1xrx && fh_3rd >= fl_1xrx)
  {
    return_status = TRUE;
  }
  return return_status;
}

/*===========================================================================

FUNCTION wwcoex_check_lte_1x_im3_in_gps_band                              

DESCRIPTION
   This API will check if GPS is ON and then calculate the IM3 Frequency 
   and check if it falls in the GPS frequency range [1569 - 1611]

DEPENDENCIES 
  None

RETURN VALUE 
  TRUE if the freq condition is satisfied
  FALSE otherwise
  
SIDE EFFECTS
  None

===========================================================================*/
static boolean wwcoex_check_lte_1x_im3_in_gps_band
(
  uint32 fc_ltetx,
  uint32 fc_cdmatx, 
  uint32 lte_bw
)
{
  uint32 fc_im3;
  uint32 fl_im3;
  uint32 fh_im3;
  uint32 bw_im3;
  boolean return_status = FALSE;

  /* GPS frequency range */
  uint32 gps_lo = 1569000;
  uint32 gps_hi = 1611000;
  

  /* Calcualte IM3 Frequency and BW  */
  if ( fc_cdmatx < fc_ltetx)
  {
    fc_im3 = 2 * fc_cdmatx - fc_ltetx;  
    bw_im3 = (2 * 1250) + lte_bw; 
  }
  else
  {
    fc_im3 = 2 * fc_ltetx - fc_cdmatx;  
    bw_im3 = (2 * lte_bw) + 1250; 
  }

  fl_im3 = fc_im3 - bw_im3/2;
  fh_im3 = fc_im3 + bw_im3/2;

  LMTSMGR_MSG_4(MED, "check GPS desense: l_ftx %u, c_ftx %u, f_im %u, im_bw %u",
                     fc_ltetx, fc_cdmatx, fc_im3, bw_im3);

  if (gps_lo <= fh_im3 && fl_im3 <= gps_hi)
  {
    return_status = TRUE;
  }

  return return_status;
}

#endif

/*=============================================================================

  FUNCTION:  wwcoex_list_hdr_b0_1x_b0

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for 1X BC0 and HDR BC0
 
    @return
    TRUE - Victim can be desensed
    FALSE - Victim is not desensed
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_hdr_b0_1x_b0
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;
  uint32 cdma1x_tx_freq, hdr_tx_freq;
  uint32 part1a, part1b, part1, part2a, part2b, part2;
  uint32 cdma1x_rf_chan, hdr_rf_chan;

  /* If there are no victim or aggressor channels, return FALSE i.e
     no desense required. */
  if ( aul_info == NULL || vdl_info == NULL ||
       vul_info == NULL )
  {
    return desense;
  }

  /* 
  | 1X Band 0 Ch N1x in eq     | HDR Band 0 Ch Nhdr in eq       |
  | If HDR Tx freq > 1x Tx freq, 
  |   for N1x [1-799], Nhdr [1-799], (Nhdr+41)>=(N1x+750)>=(Nhdr-41) |
  |   for N1x [991-1023], Nhdr [1-799], (Nhdr+41)>=(N1x-1023+750)>=(Nhdr-41) |      
  | Else
  |   for Nhdr [1-799], N1x [1-799], (N1x+41)>=(Nhdr+750)>=(N1x-41) |
  |   for Nhdr [991-1023], N1x [1-799], (N1x+41)>=(Nhdr-1023+750)>=(N1x-41) | 
  */

      /* Get the 1X/DO Tx frequency from RF */
  cdma1x_tx_freq = vul_info->frequency;
  hdr_tx_freq = aul_info->frequency;
  cdma1x_rf_chan = vul_info->channel;
  hdr_rf_chan = aul_info->channel;

  /* 1x Tx channel frequency is below DO Tx channel */
  part1a = ( ((cdma1x_rf_chan >= 1) && (cdma1x_rf_chan <= 799)) &&
                     ((hdr_rf_chan >= 1)    && (hdr_rf_chan <= 799)   ) &&
                     ((hdr_rf_chan + 41)    >= (cdma1x_rf_chan + 750) ) && 
                     ((hdr_rf_chan - 41)    <= (cdma1x_rf_chan + 750) )  );

  part1b = ( ((cdma1x_rf_chan >= 991) && (cdma1x_rf_chan <= 1023) ) &&
                     ((hdr_rf_chan >= 1)   && (hdr_rf_chan <= 799)      ) &&
                     ((hdr_rf_chan + 41)   >= (cdma1x_rf_chan + 750 - 1023)) &&
                     ((hdr_rf_chan - 41)   <= (cdma1x_rf_chan + 750 - 1023)) );

  /* Combine parts 1a and 1b */
  part1 = ( (cdma1x_tx_freq < hdr_tx_freq) && (part1a || part1b));

  /* 1x Tx channel frequency is above DO Tx channel */
  part2a = ( ((hdr_rf_chan >= 1)     && (hdr_rf_chan <= 799)    ) &&
                     ((cdma1x_rf_chan >= 1)  && (cdma1x_rf_chan <= 799) ) &&
                     ((cdma1x_rf_chan + 41)  >= (hdr_rf_chan + 750)     ) && 
                     ((cdma1x_rf_chan - 41)  <= (hdr_rf_chan + 750)     )  );

  part2b = ( ((hdr_rf_chan >= 991)  && (hdr_rf_chan <= 1023)      ) &&
                     ((cdma1x_rf_chan >= 1) && (cdma1x_rf_chan <= 799)   ) &&
                     ((cdma1x_rf_chan + 41) >= (hdr_rf_chan + 750 - 1023)) &&
                     ((cdma1x_rf_chan - 41) <= (hdr_rf_chan + 750 - 1023)) );

  /* Combine parts 2a and 2b */
  part2 = ( (cdma1x_tx_freq > hdr_tx_freq) && (part2a || part2b));
 
      /* Decide if half duplex backoff should be applied. */
  if( part1 )
  {
    desense = IM_TYPE1_DESENSE;
  }
  else if ( part2 )
  {
    desense = IM_TYPE2_DESENSE;
  }

  LMTSMGR_MSG_1(LOW, "Entered check combo hdr_b0_1x_b0 desense %d ", desense);
  return desense;
}

/*=============================================================================

  FUNCTION:  wwcoex_list_hdr_b1_1x_b1

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for 1X BC1 and HDR BC1
 
    @return
    TRUE - Victim can be desensed
    FALSE - Victim is not desensed
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_hdr_b1_1x_b1
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;
  boolean part1 = FALSE, part2 = FALSE;
  uint32 cdma1x_rf_chan, hdr_rf_chan;

  /* If there are no victim or aggressor channels, return FALSE i.e
     no desense required. */
  if ( aul_info == NULL || vdl_info == NULL ||
       vul_info == NULL )
  {
    return desense;
  }

    /* 
  | 1X Band 1 Ch N1x in eq     | HDR Band 1 Ch Nhdr in eq   |
  | If HDR Ch > 1x Ch, 
  |  (Nhdr + 24.6) >= 800+N1x >= (Nhdr-24.6)  |
  | Else
  |  (N1x + 24.6) >= 800+Nhdr >= (N1x-24.6)  |
  */
  cdma1x_rf_chan = aul_info->channel;
  hdr_rf_chan = vul_info->channel;

  /* 1x chan below HDR chan: */
  part1 = (cdma1x_rf_chan < hdr_rf_chan) &&
                   ((10*hdr_rf_chan + 246) >= (8000 + 10*cdma1x_rf_chan)) &&
                   ((10*hdr_rf_chan - 246) <= (8000 + 10*cdma1x_rf_chan));

  /* 1x chan above HDR chan: */
  part2 = (cdma1x_rf_chan > hdr_rf_chan) &&
                   ((10*cdma1x_rf_chan + 246) >= (8000 + 10*hdr_rf_chan)) &&
                   ((10*cdma1x_rf_chan - 246) <= (8000 + 10*hdr_rf_chan));

  /* Determine if half-duplex backoff is to be applied */
  if( part1 )
  {
    desense = IM_TYPE1_DESENSE;
  }
  else if ( part2 )
  {
    desense = IM_TYPE2_DESENSE;
  }
  LMTSMGR_MSG_1(LOW, "Entered check combo hdr_b1_1x_b1 desense %d ", desense);

  return desense;
}

/*=============================================================================

  FUNCTION:  wwcoex_list_hdr_b0_1x_b10

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for 1X BC10 and HDR BC0
 
    @return
    TRUE - Victim can be desensed
    FALSE - Victim is not desensed
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_hdr_b0_1x_b10
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;

  /* If there are no victim or aggressor channels, return FALSE i.e
     no desense required. */
  if ( aul_info == NULL || vdl_info == NULL ||
       vul_info == NULL )
  {
    return desense;
  }


  /* 
  | 1X Band 10 Ch N1x in eq     | HDR Band 0 Ch Nhdr in eq       |
  |  (0.03*Nhdr + 20.23) >= 0.025*N1x+22.5 >= (0.03*Nhdr+17.77)  |
  */
  if ( ((30*aul_info->channel + 20230) >= (25*vul_info->channel + 22500)) &&
       ((30*aul_info->channel + 17770) <= (25*vul_info->channel + 22500)) )
  {
    desense = IM_TYPE1_DESENSE;
  }

  LMTSMGR_MSG_1(HIGH, "Entered check combo hdr_b0_1x_b10 desense %d ", desense);
  return desense;
}


/*=============================================================================

  FUNCTION:  wwcoex_list_hdr_b10_1x_b0

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for 1X BC0 and HDR BC10
 
    @return
    desense type 
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_1x_b0_hdr_b10
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;

  /* If there are no victim or aggressor channels, return FALSE i.e
     no desense required. */
  if ( aul_info == NULL || vdl_info == NULL ||
       vul_info == NULL )
  {
    return desense;
  }


  /* 
  | 1X Band 0 Ch N1x in eq     | HDR Band 10 Ch Nhdr in eq       |
  |  (0.03*N1x + 20.23) >= 0.025*Nhdr+22.5 >= (0.03*N1x + 17.77)  |
  */
  if ( ((30*aul_info->channel + 20230) >= (25*vul_info->channel + 22500)) &&
       ((30*aul_info->channel + 17770) <= (25*vul_info->channel + 22500)) )
  {
    desense = IM_TYPE2_DESENSE;
  }


  return desense;
}

/*=============================================================================

  FUNCTION:  wwcoex_list_hdr_b10_1x_b10

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for 1X BC10 and HDR BC10
 
    @return
    desense type 
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_hdr_b10_1x_b10
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;
  uint32 cdma1x_tx_freq, a_channels, v_channels;
  boolean part1, part2;

  /* If there are no victim or aggressor channels, return FALSE i.e
     no desense required. */
  if ( aul_info == NULL || vdl_info == NULL ||
       vul_info == NULL )
  {
    return desense;
  }


  /* 
  | 1X Band 10 Ch N1x in eq     | HDR Band 10 Ch Nhdr in eq       |
  */
  cdma1x_tx_freq = vul_info->frequency;
  v_channels = vul_info->channel;
  a_channels = aul_info->channel;
        
  /* 1x chan below HDR chan: */
  part1 = (v_channels < a_channels) &&
                  ((10*a_channels + 492) >= (9000 + 10*v_channels)) &&
                  ((10*a_channels - 492) <= (9000 + 10*v_channels));

  /* 1x chan above HDR chan: */
  part2 = (v_channels > a_channels) &&
                  ((10*v_channels + 492) >= (9000 + 10*a_channels)) &&
                  ((10*v_channels - 492) <= (9000 + 10*a_channels));

  /* Determine if IM3 backoff is to be applied based on IM3 frequency range and cdma1x rx frequency range */
  if (part1 && cdma1x_tx_freq <= 824000)
  {
    /* If atleast one channel combination is found to cause desense then 
       end the loop */
    desense = IM_TYPE1_DESENSE;
  }

  /* Determine if IM3 backoff is to be applied based on IM3 frequency range and DO rx frequency range */
  if (part2 && cdma1x_tx_freq <= 824000)
  {
    /* If atleast one channel combination is found to cause desense then 
               end the loop */
    desense = IM_TYPE2_DESENSE;
  }

  return desense;
}

#ifdef FEATURE_LTE

/*=============================================================================

  FUNCTION:  wwcoex_list_lte_b39_gsm_b1800

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for LTE B39 and GSM B1800
 
    @return
    TRUE - Victim can be desensed
    FALSE - Victim is not desensed
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_lte_b39_gsm_b1800
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;

  /* If there are no victim or aggressor channels, return FALSE i.e
     no desense required. */
  if ( aul_info == NULL || vdl_info == NULL )
  {
    return desense;
  }

  /* Check for channel range only if in SVLTE mode 
     | LTE B39 Ch Any    | GSM2 Band 1800 Ch 737 - 885   | */
  if (LMTSMGR_IS_SGLTE_FEATURE_MODE() ||
      LMTSMGR_IS_SINGLE_TX_DSDA_ENABLED()) //Rohit - Add this check since for L+G case also check the filter path range in BO.2.1.1
  {
     if ( wwcoex_is_gsm_channel_victim(vdl_info->band, vdl_info->channel) )
     {
        LMTSMGR_MSG_0(MED, "Satisfied LTE39 GSM1800 channel combo (SGLTE)");
        desense = NOISE_DESENSE;
     }
  }
  else
  {
    desense = NOISE_DESENSE;
  }

  return desense;
}

/*=============================================================================

  FUNCTION:  wwcoex_list_gsm_b900_lte_b7

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for GSM B900 and LTE B7
 
    @return
    TRUE - Victim can be desensed
    FALSE - Victim is not desensed
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_gsm_b900_lte_b7
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;
  uint32 upper_range, lower_range;
  boolean is_range_update = FALSE;
  uint32   aul_channel;

  LMTSMGR_MSG_0(LOW, "Entered check combo G900 LTE7");

  /* If there are no victim or aggressor channels, return FALSE i.e
     no desense required. */
  if ( aul_info == NULL || vdl_info == NULL ||
       vul_info == NULL )
  {
    return desense;
  }
  
  /* If device is not SGLTE capable */
  if (!(LMTSMGR_IS_SGLTE_FEATURE_MODE()))
  {
    /* For DSDA,DR-DSDS, there is always RxBN interference... */
    desense = NOISE_DESENSE;
  }
  else
  {

    /* | GSM2 Band 900 Ch Ngsm in eq     | LTE Band 7 Ch Nlte in eq       |
       |(6*Ngsm+3320.5) >= Nlte >= (6* Ngsm+3179.5) when 0<= Ngsm <=124   |
       |(6*Ngsm-2823.5) >= Nlte >= (6*Ngsm-2964.5) when 975<= Ngsm <=1023 |
    */
    aul_channel = aul_info->channel;
    if ( aul_channel <= 124 )
    {
      upper_range = 6 * aul_channel * 10 + 33205;
      lower_range = 6 * aul_channel * 10 + 31795;
      is_range_update = TRUE;
    }
    else if ( (aul_channel >= 975) && (aul_channel <= 1023) )
    {
      upper_range = 6 * aul_channel * 10 - 28235;
      lower_range = 6 * aul_channel * 10 - 29645;
      is_range_update = TRUE;
    }

    if( (is_range_update == TRUE) && ( (vul_info->channel*10) >= lower_range) &&
            ( (vul_info->channel*10) <= upper_range) )
    {
      LMTSMGR_MSG_2(LOW, "Found a desense combo with channels: %d %d", 
                      aul_channel, vul_info->channel);
      desense = NOISE_DESENSE;
    }
  }

  return desense;
}

/*=============================================================================

  FUNCTION:  wwcoex_list_lte_b25_1x_b1

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for 1X B1 and LTE B25
 
    @return
    TRUE - Victim can be desensed
    FALSE - Victim is not desensed
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_lte_b25_1x_b1
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;
  uint32 nrb = 50; 
  uint32 lower_range = 0, upper_range = 0;
  uint32 lte_ul_freq, onex_tx_freq;
  uint32 cur_lte_tx_chan;
  uint32 cur_1x_tx_chan;


  /* If there are no victim or aggressor channels, return FALSE i.e
     no desense required. */
  if ( aul_info == NULL || vdl_info == NULL ||
       vul_info == NULL )
  {
    return desense;
  }

  /* Get the current NRB */
  nrb = (uint32)rflte_core_txpl_get_lim_vs_freq_rb_block();

    /* 
  | 1X Band 1 Ch N1x in eq     | LTE Band 25 Ch Nlte in eq       |
  | If LTE Tx freq > 1x Tx freq, 
  |  (2*(Nlte-26040)+(1.8*NRB+12.3)-800) >= N1x >= (2*(Nlte-26040)-(1.8*NRB+12.3)-800) |
  | Else
  |  (0.5*N1x+5*(1.23+0.18*NRB)-400) >= Nlte-26040 >= (0.5*N1x-5*(1.23+0.18*NRB)-400) |
  */

  /* Get the LTE Tx frequency from RF */
  lte_ul_freq = aul_info->frequency;
  cur_lte_tx_chan = aul_info->channel;

  /* Get the 1X Tx frequency from RF */
  onex_tx_freq = vul_info->frequency;
  cur_1x_tx_chan = vul_info->channel;

  /* Compare the freq */
  if (lte_ul_freq > onex_tx_freq)
  {
    /* LTE UL Freq is higher than 1x Tx freq */

    upper_range = 20*(cur_lte_tx_chan - 26040) + (18 * nrb + 123) - 8000;
    lower_range = 20*(cur_lte_tx_chan - 26040) - (18 * nrb + 123) - 8000;

    if( ( (cur_1x_tx_chan*10) >= lower_range) &&
          ( (cur_1x_tx_chan*10) <= upper_range) )
    {
      desense = IM_TYPE1_DESENSE;
    }
  }
  else
  {
    /* LTE UL Freq is lower than 1x Tx freq */

    upper_range = 5*10*cur_1x_tx_chan +  5*(123 + 18*nrb) - 40000 ;
    lower_range = 5*10*cur_1x_tx_chan -  5*(123 + 18*nrb) - 40000 ;

    if( ( (cur_lte_tx_chan*100 - 2604000) >= lower_range) &&
            ( (cur_lte_tx_chan*100 - 2604000) <= upper_range) )
    {
      desense = IM_TYPE2_DESENSE;
    }
  }
  LMTSMGR_MSG_1(HIGH, "Found a desense combo lte_b25_1x_b1 desense %d", desense );

  return desense;
}
/*=============================================================================

  FUNCTION:  wwcoex_list_lte_b13_1x_b0

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for 1X B0 and LTE B13
 
    @return
    TRUE - Victim can be desensed
    FALSE - Victim is not desensed
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_lte_b13_1x_b0
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;
  uint32 cur_1x_tx_chan;

  /* If there are no victim or aggressor channels, return FALSE i.e
     no desense required. */
  if ( vul_info == NULL )
  {
    return desense;
  }

  /* | 1X Band 0 Ch 1-279 & 991-1023  | LTE Band 13 Ch Any    | */
  cur_1x_tx_chan = vul_info->channel;

  if ( (( cur_1x_tx_chan >= 1)   && ( cur_1x_tx_chan <= 279))||
       (( cur_1x_tx_chan >= 991) &&( cur_1x_tx_chan <= 1023)) )
  {
    /* If atleast one channel combination is found to cause desense then 
       end the loop */
    desense = IM_TYPE1_DESENSE;
  }
  LMTSMGR_MSG_1( LOW, "wwcoex lte_b13_1x_b0 desense: Found desense combo ! %d", desense);
  return desense;
}

/*=============================================================================

  FUNCTION:  wwcoex_list_lte_b2_1x_b1

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for 1X B1 and LTE B2
 
    @return
    TRUE - Victim can be desensed
    FALSE - Victim is not desensed
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_lte_b2_1x_b1
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;
  boolean part1a, part1b, part1, part2a, part2b, part2;
  uint32 cur_lte_tx_freq;
  uint32 cdma1x_tx_freq = 0;
  uint32 cur_lte_tx_num_rb;
  uint32 cur_lte_tx_chan;
  uint32 cur_1x_tx_chan;

  /* If there are no victim or aggressor channels, return 
     no desense required. */
  if ( aul_info == NULL || vdl_info == NULL )
  {
    return desense;
  }

  if ( vul_info == NULL )
  {
    desense = NOISE_DESENSE;
  }
  else
  {
    /* | 1X Band 1   | LTE Band 2 Ch Any    | */
  
    /* convert to uint32 to prevent overflow and simplify comparisons */
    cur_lte_tx_num_rb = (18 * (uint32)rflte_core_txpl_get_lim_vs_freq_rb_block()) + 123;
    
    cur_lte_tx_freq = aul_info->frequency;
    cdma1x_tx_freq = vul_info->frequency;
    cur_lte_tx_chan = 10 * (aul_info->channel - 18600);
    cur_1x_tx_chan = vul_info->channel;
  
        
        /* LTE uplink frequency is above 1x Tx frequency */
    part1a = ((int32)(2 * cur_lte_tx_chan + cur_lte_tx_num_rb - 8000) >= (int32)(10 * cur_1x_tx_chan));
    part1b = ((int32)(2 * cur_lte_tx_chan - cur_lte_tx_num_rb - 8000) <= (int32)(10 * cur_1x_tx_chan));
        part1 = ((cur_lte_tx_freq > cdma1x_tx_freq) && part1a && part1b);
    
        /* LTE uplink frequency is below 1x Tx frequency */
    part2a = ((int32)(5 * 10 * cur_1x_tx_chan + 5 * cur_lte_tx_num_rb - 40000) >= (int32)(10 * cur_lte_tx_chan));
    part2b = ((int32)(5 * 10 * cur_1x_tx_chan - 5 * cur_lte_tx_num_rb - 40000) <= (int32)(10 * cur_lte_tx_chan));
        part2 = ((cur_lte_tx_freq < cdma1x_tx_freq) && part2a && part2b);
        
        /* Determine if IM3 backoff is to be applied */
    if (part1)
    {
      desense = IM_TYPE1_DESENSE; //IM_DESENSE_HIGH_PRIO
    }
    else if ( part2 )
    {
      desense = IM_TYPE2_DESENSE;  // IM_DESENSE_LOW_PRIO
    }
  }
  LMTSMGR_MSG_1( LOW, "wwcoex lte_b2_1x_b1 desense: Found desense combo ! %d", desense);
  return desense;
}

/*=============================================================================

  FUNCTION:  wwcoex_list_1x_b15_lte_b2

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for 1X B15 and LTE B2
 
    @return
    TRUE - Victim can be desensed
    FALSE - Victim is not desensed
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_1x_b15_lte_b2
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;
  uint32 cur_lte_tx_freq;
  uint32 cdma1x_tx_freq = 0;
  uint32 cur_lte_rx_freq;
  uint16 cur_lte_bw;

  /* If there are no victim or aggressor channels, return FALSE i.e
     no desense required. */
  if ( aul_info == NULL || vdl_info == NULL ||
       vul_info == NULL )
  {
    return desense;
  }

  /* | 1X Band 15   | LTE Band 2 Ch Any    | */

  /* Bandwidth in KHz */
  cur_lte_bw = vul_info->bandwidth/1000;
  cdma1x_tx_freq = aul_info->frequency;
  cur_lte_tx_freq = vul_info->frequency;
  cur_lte_rx_freq = vdl_info->frequency;
  
  /* Decide the back off has to be applied based on LTE bandwidth, IM3 frequency range and LTE rx frequency range*/
  if ((cur_lte_bw >= 15000) && wwcoex_check_lte_1x_im3_in_lte_rx_band(cdma1x_tx_freq, cur_lte_tx_freq, cur_lte_rx_freq, 1250, cur_lte_bw))
  {
    desense = IM_TYPE2_DESENSE;
  }
  LMTSMGR_MSG_1( LOW, "wwcoex 1x_b15_lte_b2 desense: Found desense combo ! %d", desense);
  return desense;
}



/*=============================================================================

  FUNCTION:  wwcoex_list_lte_b4_1x_b0

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for 1X B0 and LTE B4
 
    @return
    TRUE - Victim can be desensed
    FALSE - Victim is not desensed
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_lte_b4_1x_b0
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;
  uint32 cur_lte_tx_freq;
  uint32 cdma1x_tx_freq = 0;
  uint16 cur_lte_bw;
  uint32 cdma1x_rx_freq = 0;


  /* If there are no victim or aggressor channels, return 
     no desense required. */
  if ( aul_info == NULL || vdl_info == NULL )
  {
    return desense;
  }

  if ( vul_info == NULL )
  {
    if ((vdl_info->frequency >= 860000) && (vdl_info->frequency <= 874000))
    {
      desense = NOISE_DESENSE;
    }
  }
  else
  {
    /* | 1X Band 0   | LTE Band 4 Ch Any    | */
  
    /* Bandwidth in KHz */
    cur_lte_bw = aul_info->bandwidth/1000;
    cur_lte_tx_freq = aul_info->frequency;
    cdma1x_tx_freq = vul_info->frequency;
    cdma1x_rx_freq = vdl_info->frequency;
  
    if (wwcoex_check_lte_1x_im2_in_1x_rx_band(cur_lte_tx_freq, cur_lte_bw, cdma1x_tx_freq, cdma1x_rx_freq))
    {
      desense = IM_TYPE1_DESENSE;
    }
  }
  return desense;
}
  
/*=============================================================================

  FUNCTION:  wwcoex_list_lte_b25_1x_b15

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for 1X B15 and LTE B25
  
    @return
    TRUE - Victim can be desensed
    FALSE - Victim is not desensed
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_lte_b25_1x_b15
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
  {
  wwan_coex_desense_type desense = NO_DESENSE;
  uint32 cur_lte_tx_freq;
  uint32 cdma1x_tx_freq = 0;
  uint32 cdma1x_rx_freq = 0;
  uint16 cur_lte_bw;


  /* If there are no victim or aggressor channels, return FALSE i.e
     no desense required. */
  if ( aul_info == NULL || vdl_info == NULL ||
       vul_info == NULL )
    {
    return desense;
  }

  cur_lte_bw = aul_info->bandwidth/1000;
  cur_lte_tx_freq = aul_info->frequency;
  cdma1x_tx_freq = vul_info->frequency;
  cdma1x_rx_freq = vdl_info->frequency;


  /* | 1X Band 15   | LTE Band 25 Ch Any    | */
  /* Decide the back off has to be applied based on LTE bandwidth, IM3 frequency range and CDMA 1x rx frequency range*/
  if ((cur_lte_bw >= 15000) && wwcoex_check_lte_1x_im3_in_1x_rx_band(cdma1x_tx_freq, cdma1x_rx_freq, cur_lte_tx_freq, 1250, cur_lte_bw))
  {
    desense = IM_TYPE1_DESENSE;
    LMTSMGR_MSG_0(LOW, "Found a desense band combo !");
  }
  return desense;
}

/*=============================================================================

  FUNCTION:  wwcoex_list_1x_b15_lte_b25

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for 1X B15 and LTE B25
    1X aggressor 
 
    @return
    TRUE - Victim can be desensed
    FALSE - Victim is not desensed
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_1x_b15_lte_b25
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;
  uint32 cur_lte_tx_freq;
  uint32 cdma1x_tx_freq = 0;
  uint32 cur_lte_rx_freq;
  uint16 cur_lte_bw;

  /* If there are no victim or aggressor channels, return FALSE i.e
     no desense required. */
  if ( aul_info == NULL || vdl_info == NULL ||
       vul_info == NULL )
  {
    return desense;
  }

  cur_lte_bw = vul_info->bandwidth/1000;
  cdma1x_tx_freq = aul_info->frequency;
  cur_lte_tx_freq = vul_info->frequency;
  cur_lte_rx_freq = vdl_info->frequency;

  /* | 1X Band 15   | LTE Band 25 Ch Any    | */

  /* Decide the back off has to be applied based on LTE bandwidth, IM3 frequency range and LTE rx frequency range*/
  if ((cur_lte_bw >= 15000) && wwcoex_check_lte_1x_im3_in_lte_rx_band(cdma1x_tx_freq, cur_lte_tx_freq, cur_lte_rx_freq, 1250, cur_lte_bw))
  {
    desense = IM_TYPE2_DESENSE;
    LMTSMGR_MSG_0(LOW, "Found a desense band combo !");
  }

  return desense;
}
  

/*=============================================================================

  FUNCTION:  wwcoex_list_lte_b4_1x_b15

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for 1X B15 and LTE B4
 
    @return
     desense type
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_lte_b4_1x_b15
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;
  uint32 cur_lte_tx_freq;
  uint32 cdma1x_tx_freq = 0;
  uint32 cdma1x_rx_freq = 0;
  uint16 cur_lte_bw;

  /* If there are no victim or aggressor channels, return FALSE i.e
     no desense required. */
  if ( aul_info == NULL || vdl_info == NULL ||
       vul_info == NULL )
  {
    return desense;
  }
  
  /* | 1X Band 15   | LTE Band 4 Ch Any    | */

  /* Bandwidth in KHz */
  cur_lte_bw = aul_info->bandwidth/1000;
  cur_lte_tx_freq = aul_info->frequency;
  cdma1x_tx_freq = vul_info->frequency;
  cdma1x_rx_freq = vdl_info->frequency;
  
  /* Decide the back off has to be applied based on LTE bandwidth, IM3 frequency range and CDMA 1x rx frequency range*/
  if (wwcoex_check_lte_1x_im3_in_1x_rx_band(cdma1x_tx_freq, cdma1x_rx_freq, cur_lte_tx_freq, 1250, cur_lte_bw))
  {
    desense = IM_TYPE1_DESENSE;
    LMTSMGR_MSG_0(LOW, "Found a desense band combo wwcoex_list_lte_b4_1x_b15!");
  }
  return desense;
}
      

/*=============================================================================

  FUNCTION:  wwcoex_list_1x_b15_lte_b4

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for 1X B15 and LTE B4
 
    @return
     desense type
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_1x_b15_lte_b4
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
    {
  wwan_coex_desense_type desense = NO_DESENSE;
  uint32 cur_lte_tx_freq;
  uint32 cdma1x_tx_freq = 0;
  uint32 cur_lte_rx_freq = 0;
  uint16 cur_lte_bw;

  /* If there are no victim or aggressor channels, return FALSE i.e
     no desense required. */
  if ( aul_info == NULL || vdl_info == NULL ||
       vul_info == NULL )
      {
    return desense;
  }
    
  /* | 1X Band 15   | LTE Band 4 Ch Any    | */

  /* Bandwidth in KHz */
  cur_lte_bw = vul_info->bandwidth/1000;
  cdma1x_tx_freq = aul_info->frequency;
  cur_lte_tx_freq = vul_info->frequency;
  cur_lte_rx_freq = vdl_info->frequency;

    
  /* Decide the back off has to be applied based on LTE bandwidth, IM3 frequency range and CDMA 1x rx frequency range*/
  if (wwcoex_check_lte_1x_im3_in_lte_rx_band(cdma1x_tx_freq, cur_lte_tx_freq, cur_lte_rx_freq, 1250, cur_lte_bw))
  {
    desense = IM_TYPE2_DESENSE;
    LMTSMGR_MSG_0(LOW, "Found a desense band combo wwcoex_list_1x_b15_lte_b4!");
  }
  return desense;
}

/*=============================================================================

  FUNCTION:  wwcoex_list_lte_b5_1x_b0

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for 1X B0 and LTE B5
 
    @return
     desense type
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_lte_b5_1x_b0
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;
  uint32 cur_lte_tx_freq;
  uint32 cdma1x_tx_freq = 0;
  uint32 cdma1x_rx_freq = 0;
  uint16 cur_lte_bw;

  /* If there are no victim or aggressor channels, return FALSE i.e
     no desense required. */
  if ( aul_info == NULL || vdl_info == NULL ||
       vul_info == NULL )
  {
    return desense;
  }
    

  /* | 1X Band 15   | LTE Band 4 Ch Any    | */

  /* Bandwidth in KHz */
  cur_lte_bw = aul_info->bandwidth/1000;
  cur_lte_tx_freq = aul_info->frequency;
  cdma1x_tx_freq = vul_info->frequency;
  cdma1x_rx_freq = vdl_info->frequency;

    
          /* Decide the back off has to be applied based on LTE bandwidth, IM3 frequency range and CDMA 1x rx frequency range*/
  if (wwcoex_check_lte_1x_im3_in_1x_rx_band(cdma1x_tx_freq, cdma1x_rx_freq, cur_lte_tx_freq, 1250, cur_lte_bw))
  {
    desense = IM_TYPE1_DESENSE;
    LMTSMGR_MSG_0(LOW, "Found a desense band combo wwcoex_list_lte_b5_1x_b0!");
  }
  return desense;
}

/*=============================================================================

  FUNCTION:  wwcoex_list_1x_b0_lte_b5

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for 1X B0 and LTE B5
 
    @return
     desense type
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_1x_b0_lte_b5
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;
  uint32 cur_lte_tx_freq;
  uint32 cdma1x_tx_freq = 0;
  uint32 cur_lte_rx_freq = 0;
  uint16 cur_lte_bw;

  /* If there are no victim or aggressor channels, return FALSE i.e
     no desense required. */
  if ( aul_info == NULL || vdl_info == NULL ||
       vul_info == NULL )
  {
    return desense;
  }

  /* | 1X Band 15   | LTE Band 4 Ch Any    | */

  /* Bandwidth in KHz */
  cur_lte_bw = vul_info->bandwidth/1000;
  cdma1x_tx_freq = aul_info->frequency;
  cur_lte_tx_freq = vul_info->frequency;
  cur_lte_rx_freq = vdl_info->frequency;


  /* Decide the back off has to be applied based on LTE bandwidth, IM3 frequency range and CDMA 1x rx frequency range*/
  if (wwcoex_check_lte_1x_im3_in_lte_rx_band(cdma1x_tx_freq, cur_lte_tx_freq, cur_lte_rx_freq, 1250, cur_lte_bw))
  {
    /* If atleast one channel combination is found to cause desense then 
        end the loop */
    desense = IM_TYPE2_DESENSE;
    LMTSMGR_MSG_0(LOW, "Found a desense band combo wwcoex_list_1x_b0_lte_b5!");
  }
  return desense;
}

/*=============================================================================

  FUNCTION:  wwcoex_list_lte_b5_1x_b10

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for 1X B10 and LTE B5
 
    @return
     desense type
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_lte_b5_1x_b10
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;
  uint32 cur_lte_tx_freq;
  uint32 cdma1x_tx_freq = 0;
  uint16 cur_lte_bw;
  uint32 cdma1x_rx_freq = 0;


  /* If there are no victim or aggressor channels, return FALSE i.e
     no desense required. */
  if ( aul_info == NULL || vdl_info == NULL ||
       vul_info == NULL )
  {
    return desense;
    }

  /* | 1X Band 10   | LTE Band 5 Ch Any    | */

  /* Bandwidth in KHz */

  cur_lte_bw = aul_info->bandwidth/1000;
  cur_lte_tx_freq = aul_info->frequency;
  cdma1x_tx_freq = vul_info->frequency;
  cdma1x_rx_freq = vdl_info->frequency;

  if ((cdma1x_tx_freq <= 824000) && wwcoex_check_lte_1x_im3_in_1x_rx_band(cdma1x_tx_freq, 
                                            cdma1x_rx_freq, cur_lte_tx_freq, 1250, cur_lte_bw))
  {
    desense = IM_TYPE1_DESENSE;
    LMTSMGR_MSG_0(LOW, "Found a desense band combo wwcoex_list_lte_b5_1x_b10!");
  }

  return desense;

}

/*=============================================================================

  FUNCTION:  wwcoex_list_1x_b15_lte_b5

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for 1X B15 and LTE B5
 
    @return
      desense type
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_1x_b15_lte_b5
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;
  uint32 cur_lte_tx_freq;
  uint32 cdma1x_tx_freq = 0;
  uint32 cur_lte_rx_freq;
  uint16 cur_lte_bw;

  /* If there are no victim or aggressor channels, return FALSE i.e
     no desense required. */
  if ( aul_info == NULL || vdl_info == NULL ||
       vul_info == NULL )
  {
    return desense;
  }
      
  /* | 1X Band 15   | LTE Band 2 Ch Any    | */

  /* Bandwidth in KHz */
  cur_lte_bw = vul_info->bandwidth/1000;
  cdma1x_tx_freq = aul_info->frequency;
  cur_lte_tx_freq = vul_info->frequency;
  cur_lte_rx_freq = vdl_info->frequency;

  if ((cur_lte_bw >= 15000) && wwcoex_check_lte_1x_im2_in_lte_rx_band(cur_lte_tx_freq, 
                                            cur_lte_rx_freq, cur_lte_bw, cdma1x_tx_freq))
  {
    /* If atleast one channel combination is found to cause desense then 
         end the loop */
    desense = IM_TYPE2_DESENSE;
    LMTSMGR_MSG_0(LOW, "Found a desense band combo wwcoex_list_1x_b15_lte_b5!");
  }

  return desense;

}

/*=============================================================================

  FUNCTION:  wwcoex_list_lte_b12_1x_b15

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for 1X B15 and LTE B12

    @return
      desense type
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_lte_b12_1x_b15
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;
  uint32 cur_lte_tx_freq;
  uint32 cdma1x_rx_freq = 0;
  uint16 cur_lte_bw;

  /* If there are no victim or aggressor channels, return FALSE i.e
     no desense required. */
  if ( aul_info == NULL || vdl_info == NULL )
  {
    return desense;
  }

  /* | 1X Band 15   | LTE Band 12 | */

  /* Bandwidth in KHz */
  cur_lte_bw = aul_info->bandwidth/1000;
  cur_lte_tx_freq = aul_info->frequency;
  cdma1x_rx_freq = vdl_info->frequency;
  
  /* Decide the back off has to be applied based on LTE bandwidth, IM3 frequency range and CDMA 1x rx frequency range*/
  if (wwcoex_check_lte_1x_3rd_harmonic_in_1x_rx_band(cdma1x_rx_freq, cur_lte_tx_freq, 1250, cur_lte_bw))
  {
    /* If atleast one channel combination is found to cause desense then 
         end the loop */
    desense = HARMONIC_DESENSE;
    LMTSMGR_MSG_0(LOW, "Found a desense band combo wwcoex_list_lte_b12_1x_b15!");
  }

  return desense;
}


/*=============================================================================

  FUNCTION:  wwcoex_list_lte_b17_1x_b15

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for 1X B15 and LTE B17
 
    @return
      desense type
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_lte_b17_1x_b15
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = FALSE;
  uint32 cur_lte_tx_freq;
  uint32 cdma1x_rx_freq = 0;
  uint16 cur_lte_bw;

  /* If there are no victim or aggressor channels, return FALSE i.e
     no desense required. */
  if ( aul_info == NULL || vdl_info == NULL )
  {
    return desense;
  }

  /* | 1X Band 15   | LTE Band 17 | */

  /* Bandwidth in KHz */
  cur_lte_bw = aul_info->bandwidth/1000;
  cur_lte_tx_freq = aul_info->frequency;
  cdma1x_rx_freq = vdl_info->frequency;
    
  /* Decide the back off has to be applied based on LTE bandwidth, IM3 frequency range and CDMA 1x rx frequency range*/
  if (wwcoex_check_lte_1x_3rd_harmonic_in_1x_rx_band(cdma1x_rx_freq, cur_lte_tx_freq, 1250, cur_lte_bw))
  {
    /* If atleast one channel combination is found to cause desense then 
         end the loop */
    desense = HARMONIC_DESENSE;
    LMTSMGR_MSG_0(LOW, "Found a desense band combo wwcoex_list_lte_b17_1x_b15!");
  }  

  return desense;

}

/*=============================================================================

  FUNCTION:  wwcoex_list_gsm_victim_lte_aggr

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for GSM and LTE
 
    @return
      desense type
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_gsm_victim_lte_aggr
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;

  /* If device is not SGLTE capable 
     or DR-DSDS capable */
  if (!(LMTSMGR_IS_SGLTE_FEATURE_MODE()))
  {
    /* For DSDA, there is always RxBN interference... */
    desense = NOISE_DESENSE;
  }

  return desense;
}

/*=============================================================================

  FUNCTION:  wwcoex_list_lte_1x_aggr_gps_victim

=============================================================================*/
/*!
    @brief
    Checks desense channel combination where GPS is the victim of LTE, 1X IM
 
    @return
      desense type
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_lte_1x_aggr_gps_victim
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;
  uint32 cur_lte_tx_freq;
  uint32 cdma1x_tx_freq = 0;
  uint16 cur_lte_bw;
  trm_get_info_input_type input;
  trm_get_info_return_type output;

  /* If there are no victim or aggressor channels, return FALSE i.e
     no desense required. */
  if ( (aul_info == NULL) || (vul_info == NULL) || (vdl_info == NULL) )
  {
    return desense;
  }

  input.client_id = TRM_GPS;
  input.info_type = TRM_CLIENT_INFO;
  input.input_data.client_info.requested_client_id = TRM_GPS;
  trm_get_info(&input, &output);

  /* if GPS is not active, then there is no desense */
  if ((output.info_type != TRM_CLIENT_INFO) || 
      (output.info.client_info.dev != RFM_DEVICE_GPS))
  {
    return desense;
  }
  
  /* Bandwidth in KHz */
  cur_lte_bw = aul_info->bandwidth/1000;
  
  cur_lte_tx_freq = aul_info->frequency;
  cdma1x_tx_freq = vul_info->frequency;

  if (wwcoex_check_lte_1x_im3_in_gps_band(cur_lte_tx_freq, cdma1x_tx_freq, cur_lte_bw))
  {
    /* If atleast one channel combination is found to cause desense then 
       end the loop */
    desense = IM_DESENSE_GPS;
  
    LMTSMGR_MSG_3(HIGH, "SVLTE-GPS IM condition pass for bands L %d, 1x %d, bw L %d",
                            aul_info->band, vul_info->band, cur_lte_bw); 
    
  }

  return desense;

}

/*=============================================================================

  FUNCTION:  wwcoex_list_lte_b3_1x_b0

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for 1X B0 and LTE B3
 
    @return
    TRUE - Victim can be desensed
    FALSE - Victim is not desensed
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_lte_b3_1x_b0
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;
  uint32 cur_lte_tx_freq;
  uint32 cdma1x_tx_freq = 0;
  uint16 cur_lte_bw;
  uint32 cdma1x_rx_freq = 0;
  

  /* If there are no victim or aggressor channels, return 
     no desense required. */
  if ( aul_info == NULL || vdl_info == NULL )
  {
    return desense;
  }

  if ( vul_info == NULL )
  {
    if ((vdl_info->frequency >= 860000) && (vdl_info->frequency <= 874000))
    {
      desense = NOISE_DESENSE;
    }
  }
  else
  {
    /* | 1X Band 1   | LTE Band 2 Ch Any    | */
  
    /* Bandwidth in KHz */
    cur_lte_bw = aul_info->bandwidth/1000;
    cur_lte_tx_freq = aul_info->frequency;
    cdma1x_tx_freq = vul_info->frequency;
    cdma1x_rx_freq = vdl_info->frequency;

    if (wwcoex_check_lte_1x_im2_in_1x_rx_band(cur_lte_tx_freq, cur_lte_bw, cdma1x_tx_freq, cdma1x_rx_freq))
      {
        desense = IM_TYPE1_DESENSE;
      }
    }
  
  return desense;
}
#endif /* LTE */

/*=============================================================================

  FUNCTION:  wwcoex_list_tds_b39_gsm_b1800

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for TDSCDMA B39 and GSM B1800
 
    @return
    TRUE - Victim can be desensed
    FALSE - Victim is not desensed
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_tds_b39_gsm_b1800
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;

  /* If there are no victim or aggressor channels, return FALSE i.e
     no desense required. */
  if ( aul_info == NULL || vdl_info == NULL )
  {
    return desense;
  }

  if (LMTSMGR_IS_DRDSDS_ENABLED())
  {
    /* For DR-DSDS, there is always Noise interference... */
    desense = NOISE_DESENSE;
  }
  /* If device is not SGLTE (implied SGTDS) capable nor DR-DSDS capable...
     TODO: Check for SGTDS as well when SGLTE capable != SGTDS capable */
  else if (!(LMTSMGR_IS_SGLTE_FEATURE_MODE() ||
        LMTSMGR_IS_SINGLE_TX_DSDA_ENABLED())) //Rohit- remove it since we need to replace it with Dynamic filter path and for L+G check fileter path range in else 
  {
    /* For DSDA, there is always Blocking interference... */
    desense = BLOCKING_DESENSE;
  }
  else /* Device is SGLTE capable */
  {
    /* | TDS B39 Ch Any    | GSM2 Band 1800 Ch 737 - 885   | */
    if ( wwcoex_is_gsm_channel_victim(vdl_info->band, vdl_info->channel) )
    {
      LMTSMGR_MSG_0(LOW, "Satisfied TDS39 GSM1800 channel combo");
      desense = NOISE_DESENSE;
    }
  }

  return desense;
}

/*=============================================================================

  FUNCTION:  wwcoex_list_gsm_victim_tds_aggr

=============================================================================*/
/*!
    @brief
    Checks desense channel combination for GSM and TDS
 
    @return
      desense type
*/
/*===========================================================================*/
static wwan_coex_desense_type wwcoex_list_gsm_victim_tds_aggr
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info
)
{
  wwan_coex_desense_type desense = NO_DESENSE;

  /* If device is not SGLTE (implied SGTDS) capable or DR-DSDS capable 
     TODO: Check for SGTDS as well when SGLTE capable != SGTDS capable */
  if (!(LMTSMGR_IS_SGLTE_FEATURE_MODE()))
  {
    /* For DSDA, there is always RxBN interference... */
    desense = NOISE_DESENSE;
  }

  return desense;
}

/*=============================================================================

  FUNCTION:  wwcoex_gen_tech_band_mask

=============================================================================*/
/*!
    @brief
    Generate a unique tech band mask for a given band combination
 
    @details
      Mask of aggr and victim bands:
        1st lower order 8 bits indicate aggr band,
        2nd lower order 8 bits indicate victim band,
        3rd lower order 4 bits indicate aggr tech,
        4th lower order 4 bits indicate victim tech
    @return
     0 if invalid mask; otherwise, mask based on details above

*/
/*===========================================================================*/
static uint32 wwcoex_gen_tech_band_mask
(
   cxm_tech_type atech,
   cxm_tech_type vtech,
   uint32 aband,
   uint32 vband
)
{
  uint32 mask = 0;

  if ( !(LMTSMGR_IS_TECH_VALID(atech) && LMTSMGR_IS_TECH_VALID(vtech)) )
  {
    return mask;
  }

  mask = aband + (vband << 8);
 
  if (IS_C2K_TECH(atech) && IS_C2K_TECH(vtech) &&
      (vband <= SYS_BAND_BC19) && (aband <= SYS_BAND_BC19))
  {
    /* For C2K use tech also in the mask */
    mask += ((uint32)atech << 16) + ((uint32)vtech << 20);
  }

  return mask;
}

/*=============================================================================

  FUNCTION:  wwcoex_get_guard_band

=============================================================================*/
/*!
    @brief
      Finds the guard band value
 
    @return
      Guard Band (in Hz)
*/
/*===========================================================================*/
static uint32 wwcoex_get_guard_band
(
   uint32 index,          /* Index in combo table */
   cxm_tech_type v_tech   /* Victim Tech */
)
{
  if (index >= WWAN_COEX_MAX_COMBS)
   {
      return 0;
   }

  if ((wwcoex_list_comb_table[index].guard[0].guard_band_tech_mask | 
        wwcoex_list_comb_table[index].guard[1].guard_band_tech_mask) & (1<<v_tech))
  {
    if (wwcoex_list_comb_table[index].guard[0].guard_band_tech_mask & (1<<v_tech))
    {
      LMTSMGR_MSG_3(LOW, "Found guard band %d Hz for index %d, tech %d",
                    wwcoex_list_comb_table[index].guard[0].guard_band, index, v_tech );
      return(wwcoex_list_comb_table[index].guard[0].guard_band);
    }
    else      
    {
      LMTSMGR_MSG_3(LOW, "Found guard band %d Hz for index %d, tech %d",
                    wwcoex_list_comb_table[index].guard[1].guard_band, index, v_tech );
                    
      return(wwcoex_list_comb_table[index].guard[1].guard_band);
    }
  }

  LMTSMGR_MSG_3(LOW, "No guard band masks %d, %d for this tech %d",
                  wwcoex_list_comb_table[index].guard[0].guard_band_tech_mask,
                  wwcoex_list_comb_table[index].guard[1].guard_band_tech_mask,
                  v_tech);
  return 0;
}

/*=============================================================================

  FUNCTION:  wwcoex_gen_freq_range_harmonics

=============================================================================*/
/*!
    @brief
    Generates the frequency range that has harmonic desense issue
    based on frequency information
 
    @return
    None
*/
/*===========================================================================*/
static void wwcoex_gen_freq_range_harmonics
(
  uint32 a_ulfreq,             /* Agressor Tx Freq (kHz) */
  uint32 a_ulbw,               /* Agressor Tx BW (Hz) */
  uint32 v_dlfreq,             /* Victim Rx Freq (kHz) */
  uint32 v_dlbw,               /* Victim Rx BW (Hz) */
  uint8  hmonic_index,         /* Harmonic Index */
  uint32 guard_band,           /* Guard Band (in Hz) */
  wwan_coex_desense_mode mode, /* Calculate for Agressor (Tx) or Victim (Rx) */
  uint32* freq_lower,          /* Frequency Range kHz */
  uint32* freq_upper
)
{
   *freq_lower = 0;
   *freq_upper = WWCOEX_MAX_FREQ;

   if (mode == DESENSE_VICTIM)
   {
      *freq_lower = ((hmonic_index * a_ulfreq * 1000) - (hmonic_index * a_ulbw / 2) - (v_dlbw / 2) - guard_band) / 1000;
      *freq_upper = ((hmonic_index * a_ulfreq * 1000) + (hmonic_index * a_ulbw / 2) + (v_dlbw / 2) + guard_band) / 1000;
   }
   else // mode == DESENSE_AGGR
   {
      *freq_lower = ((v_dlfreq * 1000 / hmonic_index) - (a_ulbw / 2) - (guard_band / hmonic_index)) / 1000;
      *freq_upper = ((v_dlfreq * 1000 / hmonic_index) + (a_ulbw / 2) + (guard_band / hmonic_index)) / 1000;
   }

}

/*=============================================================================

  FUNCTION:  wwcoex_gen_freq_range_im_aggr

=============================================================================*/
/*!
    @brief
    Generates the aggressor's frequency range that has IM desense issue
    based on frequency information
 
    @return
    None
*/
/*===========================================================================*/
static void wwcoex_gen_freq_range_im_aggr
(
   lmtsmgr_ba_serv_info_type *aul_freqInfo, /* Aggressor Tx Info */
   lmtsmgr_ba_serv_info_type *vul_freqInfo, /* Victim Tx Info */
   lmtsmgr_ba_serv_info_type *vdl_freqInfo, /* Victim Rx Info */
   uint32 guard_band,                       /* Guard Band (in Hz) */
   wwcoex_im_comb_list_type *im_list,       /* (m,n) IM desense combinations */
   uint32 *freq_lower,                      /* Frequency range (in kHz) */
   uint32 *freq_upper
)
{
  int64 im_bw, lhs, rhs;
  uint8 i;

  if ((aul_freqInfo == NULL) || (vul_freqInfo == NULL) || 
      (vdl_freqInfo == NULL) || (im_list == NULL) ||
      (freq_lower == NULL) || (freq_upper == NULL))
  {
    return;
  }

  /* 
    Solve for Aul:
     (Vdl - IMBW - GB - Vdlbw / 2) < Vhi*Vul - Ahi*Aul < (Vdl + IMBW + GB + Vdlbw / 2)
  */

  for (i=0; i < im_list->num_im_combos; i++)
  {
    /* IM bandwidth in Hz */
    im_bw = ((im_list->combs[i].victim_hmonic_index * vul_freqInfo->bandwidth) + 
                    (im_list->combs[i].aggr_hmonic_index * aul_freqInfo->bandwidth))/2;

    /* Determine the range for this IM combo */
    lhs = (vdl_freqInfo->frequency*1000) + im_bw + guard_band + (vdl_freqInfo->bandwidth/2);
    lhs -= ((int64)im_list->combs[i].victim_hmonic_index * (vul_freqInfo->frequency*1000));
    lhs /= -1 * im_list->combs[i].aggr_hmonic_index;
    lhs /= 1000; // Convert to kHz

    rhs = (vdl_freqInfo->frequency*1000) - im_bw - guard_band - (vdl_freqInfo->bandwidth/2);
    rhs -= ((int64)im_list->combs[i].victim_hmonic_index * (vul_freqInfo->frequency*1000));
    rhs /= -1 * im_list->combs[i].aggr_hmonic_index;
    rhs /= 1000; // Convert to kHz

    if (i == 0)
    {
      *freq_lower = lhs;
      *freq_upper = rhs;
    }
    else
    {
      /* Select the worse case range among all the IM combos */
      *freq_lower = MIN(*freq_lower, lhs);
      *freq_upper = MAX(*freq_upper, rhs);
    }
  }
}

/*=============================================================================

  FUNCTION:  wwcoex_gen_freq_range_im_vict

=============================================================================*/
/*!
    @brief
    Generates the victim's frequency range that has IM desense issue
    based on frequency information
 
    @return
    None
*/
/*===========================================================================*/
static void wwcoex_gen_freq_range_im_vict
(
   lmtsmgr_ba_serv_info_type *vdl_freqInfo,
   uint32 aul_bw,
   uint32 vul_bw,
   wwcoex_im_comb_list_type *im_list,
   uint32 *freq_lower,
   uint32 *freq_upper
)
{
  uint32 i, im_bw, max_im_bw = 0;

  if ((vdl_freqInfo == NULL) || (im_list == NULL) ||
      (freq_lower == NULL) || (freq_upper == NULL))
  {
    return;
  }

  /* Calculate the range for a given Victim DL */
  for (i=0; i < im_list->num_im_combos; i++)
  {
    /* IM bandwidth in Hz */
    im_bw = ((im_list->combs[i].victim_hmonic_index * vul_bw) + 
                    (im_list->combs[i].aggr_hmonic_index * aul_bw))/2;

    max_im_bw = MAX(max_im_bw, im_bw);
  }

  *freq_lower = vdl_freqInfo->frequency*1000 - ((max_im_bw / 2) + (vdl_freqInfo->bandwidth / 2));
  *freq_lower /= 1000; // kHz
  *freq_upper = vdl_freqInfo->frequency*1000 + ((max_im_bw / 2) + (vdl_freqInfo->bandwidth / 2));
  *freq_upper /= 1000; // kHz
   
}


/*=============================================================================

  FUNCTION:  wwcoex_get_band_from_tech_band_mask

=============================================================================*/
/*!
    @brief
    Extracts the band from the tech_band_mask
 
    @param
      mask - tech_band_mask
      mode - find aggressor or victim band
 
    @details
      Mask of aggr and victim bands:
        1st lower order 8 bits indicate aggr band,
        2nd lower order 8 bits indicate victim band,
        3rd lower order 4 bits indicate aggr tech,
        4th lower order 4 bits indicate victim tech
 
    @return
     NON-ZERO - associated band
     0 if band could not be found

*/
/*===========================================================================*/
static uint32 wwcoex_get_band_from_tech_band_mask
(
  uint32 mask,
  wwan_coex_desense_mode mode
)
{
  uint32 band = 0;

  switch (mode)
  {

    case DESENSE_VICTIM:
      /* 2nd lower order 8 bits indicate victim band */
      band = (mask >> 8 ) & 0xFF;
      break;

    case DESENSE_AGGR:
      /* 1st lower order 8 bits indicate aggr band */
      band = mask & 0xFF;
      break;

    default:
      break;
  }

  return band;

}


#endif /* MCS_LIMITS_LOW_MEMORY */

/*=============================================================================

  FUNCTION:  wwcoex_is_gsm_channel_aggr

=============================================================================*/
/*!
    @brief
    Checks the GSM ARFCN if it is in the aggr channel range. This is primarily
    for SGLTE Coexistence in LTE/TDS B39/B34 and GSM 1800
 
    @return
    TRUE - Victim can be desensed
    FALSE - Victim is not desensed
*/
/*===========================================================================*/
boolean wwcoex_is_gsm_channel_aggr
(
   uint32 band,
   uint32 channel
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  boolean is_aggr = FALSE;

  if (band == (uint32)SYS_BAND_GSM_DCS_1800)
  {
    if ( (channel >= 512) && (channel <= 736) )
    {
      is_aggr = TRUE;
    }
  }

  return is_aggr;

#else /* MCS_LIMITS_LOW_MEMORY */

  WWCOEX_DUMMY_CHECK(band);
  WWCOEX_DUMMY_CHECK(channel);

  return FALSE;

#endif /* MCS_LIMITS_LOW_MEMORY */
}

/*=============================================================================

  FUNCTION:  wwcoex_is_gsm_channel_victim

=============================================================================*/
/*!
    @brief
    Checks the GSM ARFCN if it is in the victim channel range. This is primarily
    for SGLTE Coexistence in LTE/TDS B39 and GSM 1800
 
    @return
    TRUE - Victim can be desensed
    FALSE - Victim is not desensed
*/
/*===========================================================================*/
boolean wwcoex_is_gsm_channel_victim
(
   uint32 band,
   uint32 channel
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  boolean is_victim = FALSE;

  if (band == (uint32)SYS_BAND_GSM_DCS_1800)
  {
    if ( (channel >= 737) && (channel <= 885) )
    {
      is_victim = TRUE;
    }
  }

  return is_victim;
  
#else /* MCS_LIMITS_LOW_MEMORY */
  
    WWCOEX_DUMMY_CHECK(band);
    WWCOEX_DUMMY_CHECK(channel);
    return FALSE;
  
#endif /* MCS_LIMITS_LOW_MEMORY */

}

/*=============================================================================

  FUNCTION:  wwcoex_check_harmonic

=============================================================================*/
/*!
    @brief
    Checks for harmonic desense in victim Rx bw
 
    @return
    TRUE
 
*/
/*===========================================================================*/
boolean wwcoex_check_harmonic 
(
  uint32 a_ulfreq,
  uint32 a_ulbw,
  uint32 v_dlfreq,
  uint32 v_dlbw,
  uint8  hmonic_index,
  uint32 guard_band
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  uint32 lhs, rhs;
  
  /* (v_dlfreq - n*a_ulbw/2 - v_dlbw/2 - GB) < (n * a_ulfreq) <  (v_dlfreq + n*a_ulbw/2 + v_dlbw/2 + GB)*/ 

  lhs = (v_dlfreq*1000 - hmonic_index *a_ulbw/2 - v_dlbw/2 - guard_band);
  rhs = (v_dlfreq*1000 + hmonic_index *a_ulbw/2 + v_dlbw/2 + guard_band);

  if ( (lhs < (hmonic_index * a_ulfreq*1000)) && 
                ((hmonic_index * a_ulfreq *1000) < rhs) )
  {
    LMTSMGR_MSG_3(HIGH, "Found (%d)nd Harmonic combo for freqs %d %d", 
                        hmonic_index,
                        a_ulfreq,
                        v_dlfreq );
    return TRUE;
  }

  return FALSE;

#else /* MCS_LIMITS_LOW_MEMORY */
  
  WWCOEX_DUMMY_CHECK(a_ulfreq);
  WWCOEX_DUMMY_CHECK(a_ulbw);
  WWCOEX_DUMMY_CHECK(v_dlfreq);
  WWCOEX_DUMMY_CHECK(v_dlbw);
  WWCOEX_DUMMY_CHECK(hmonic_index);
  WWCOEX_DUMMY_CHECK(guard_band);

  return FALSE;
  
#endif /* MCS_LIMITS_LOW_MEMORY */

}

/*=============================================================================

  FUNCTION:  wwcoex_check_im

=============================================================================*/
/*!
    @brief
    Checks for IM desense in victim Rx bw
 
    @return
    TRUE
 
*/
/*===========================================================================*/
boolean wwcoex_check_im
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vul_info,
  cxm_tech_link_info_s* vdl_info,
  const wwcoex_im_comb_list_type* im_list,
  uint32 guard_band
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  uint32 i, im_freq, im_bw, lhs, rhs;
  boolean imdesense = FALSE;

  /* scan through all im combos to see if any condition meets */
  for (i=0; i < im_list->num_im_combos; i++)
  {
    if (aul_info->frequency < vul_info->frequency )
    {
      /* IM frequency in Hz */
      im_freq = ((im_list->combs[i].victim_hmonic_index * vul_info->frequency) - 
                      (im_list->combs[i].aggr_hmonic_index * aul_info->frequency)) * 1000;    
    }
    else
    {
      /* IM frequency in Hz */
      im_freq = ((im_list->combs[i].aggr_hmonic_index * aul_info->frequency) - 
                 (im_list->combs[i].victim_hmonic_index * vul_info->frequency)) * 1000;
    }

    /* IM bandwidth in Hz */
    im_bw = ((im_list->combs[i].victim_hmonic_index * vul_info->bandwidth) + 
                    (im_list->combs[i].aggr_hmonic_index * aul_info->bandwidth))/2;

    /* LHS and RHS of comparisons */
    lhs = (vdl_info->frequency*1000) - im_bw - guard_band - vdl_info->bandwidth/2;
    rhs = (vdl_info->frequency*1000) + im_bw + guard_band + vdl_info->bandwidth/2;

    if ( (lhs < im_freq) && (im_freq < rhs) )
    {
      LMTSMGR_MSG_4(HIGH, "Found IM(%d,%d) combo for freqs %d %d", 
                        im_list->combs[i].aggr_hmonic_index,
                        im_list->combs[i].victim_hmonic_index,
                        aul_info->frequency,
                        vdl_info->frequency );
      imdesense = TRUE;
      break;
    }
    else
    {
      LMTSMGR_MSG_5(MED, "No IM detected: IM freq %d, lhs %d, rhs %d, vul_freq %d, victim_channel %d",
                       im_freq, lhs, rhs, vul_info->frequency*1000, vdl_info->channel );
    }
  }

  return imdesense;
  
#else /* MCS_LIMITS_LOW_MEMORY */

  WWCOEX_DUMMY_CHECK(aul_info);
  WWCOEX_DUMMY_CHECK(vul_info);
  WWCOEX_DUMMY_CHECK(vdl_info);
  WWCOEX_DUMMY_CHECK(im_list);
  WWCOEX_DUMMY_CHECK(guard_band);

  return FALSE;

#endif /* MCS_LIMITS_LOW_MEMORY */

} 

/*=============================================================================

  FUNCTION:  wwcoex_search_list

=============================================================================*/
/*!
    @brief
    Searches the list for the tech band combination
 
    @return
    Index to the entry if the band combination is found otherwise
    MAX_COMBS is returned
*/
/*===========================================================================*/
uint32 wwcoex_search_list
(
  cxm_tech_type vtech,
  cxm_tech_type atech,
  uint32  vband,
  uint32 aband
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  uint32 srch_val = 0, arr_entry_val, start_idx, end_idx, srch_idx;
  boolean found = FALSE;
  wwcoex_tech_pair_desense_type *tech_pair_node;

  TTLMARKER(LMTSMGR_COEX_SRCHLIST_IN);
  
  tech_pair_node = &wwan_tech_pair_desense_data[atech][vtech];

  if( (aband == tech_pair_node->aband) && (vband == tech_pair_node->vband) )
  {
     LMTSMGR_DBG_MSG_4(LOW, "Debug_skc: Cache hit for atech %d, aband %d  & vtech %d vband %d", 
                            atech, aband, vtech, vband );
     TTLMARKER(LMTSMGR_COEX_SRCHLIST_CACHE);
     return tech_pair_node->cached_index;
  } 

  /* Create the tech_band mask */
  srch_val = wwcoex_gen_tech_band_mask(atech, vtech, aband, vband);

  /* Binary search into the list */

  start_idx = 0;
  end_idx = WWAN_COEX_MAX_COMBS - 1;
  while (1)
  {
    if (end_idx < start_idx)
    {
      /* could not find the val in the list */
      break;
    }

    srch_idx = start_idx + ((end_idx - start_idx) >> 1);
    
    arr_entry_val = wwcoex_list_comb_table[srch_idx].tech_band_mask;
    if (srch_val == arr_entry_val)
    {
      /* Found it */
      found = TRUE;
      break;
    }
    else if (srch_val > arr_entry_val)
    {
      start_idx = srch_idx + 1;
    }
    else if (srch_val < arr_entry_val)
    {
      end_idx = srch_idx - 1;
    }
  } 

  if (found == FALSE)
  {
    srch_idx = WWAN_COEX_MAX_COMBS;
  }

  tech_pair_node->aband = aband;
  tech_pair_node->vband = vband;
  tech_pair_node->cached_index = srch_idx;

  TTLMARKER(LMTSMGR_COEX_SRCHLIST_OUT);
  return srch_idx;

#else /* MCS_LIMITS_LOW_MEMORY */

  WWCOEX_DUMMY_CHECK(vtech);
  WWCOEX_DUMMY_CHECK(atech);
  WWCOEX_DUMMY_CHECK(vband);
  WWCOEX_DUMMY_CHECK(aband);

  return WWAN_COEX_MAX_COMBS;

#endif /* MCS_LIMITS_LOW_MEMORY */

}



/*=============================================================================

  FUNCTION:  wwcoex_is_filtered_path_combo

=============================================================================*/
/*!
    @brief
    Checks if the current frequency reuiqres a filter path.
 
    @return
    TRUE
 
*/
/*===========================================================================*/
uint32 wwcoex_is_filtered_path_combo
(
  cxm_tech_link_info_s* aul_info,
  cxm_tech_link_info_s* vdl_info
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  uint32 filter_combo = 0;

  /* If device is SGLTE capable (implied SGTDS capable also)
    TODO: Check for SGTDS as well when SGLTE capable != SGTDS capable  */
  if (LMTSMGR_IS_SGLTE_FEATURE_MODE() ||
      LMTSMGR_IS_SINGLE_TX_DSDA_ENABLED()) //Rohit- Added it here since with L+G not only for SGLTE (basically Single Tx DSDA) filter path is present so check it in BO.2.1.1
  {
    /* If aggr is G1800 and victim is L B41/B40/B39/B38/B34 or T B40/B39/B34 */
    if ( (aul_info->band == (uint32)SYS_BAND_GSM_DCS_1800) &&
         ( LMTSMGR_IS_LTE_FILTER_PATH_BAND(vdl_info->band) || 
           LMTSMGR_IS_TDS_FILTER_PATH_BAND(vdl_info->band))
       )
    {
      if (wwcoex_is_gsm_channel_aggr(aul_info->band, aul_info->channel))// 512-736
      {
        filter_combo = ((1<<FILTERED_PATH) | (1<<FILTERED_PATH_GSM_TX_ONLY));
      }
      else if (wwcoex_is_gsm_channel_victim(aul_info->band, aul_info->channel))// 737-850
      {
        filter_combo = (1<<FILTERED_PATH_GSM_TX_ONLY);
      }
    }
    /* If victim is G1800 and aggr is L B41/B40/B39/B38/B34 or T B40/B39/B34 */
    else if ( (vdl_info->band == (uint32)SYS_BAND_GSM_DCS_1800) &&
              ( LMTSMGR_IS_LTE_FILTER_PATH_BAND(aul_info->band) || 
                LMTSMGR_IS_TDS_FILTER_PATH_BAND(aul_info->band))
           )
    {
      if (wwcoex_is_gsm_channel_aggr(vdl_info->band, vdl_info->channel))// 512-736
      {
        filter_combo = ((1<<FILTERED_PATH) | (1<<FILTERED_PATH_GSM_TX_ONLY));
      }
      else if (wwcoex_is_gsm_channel_victim(vdl_info->band, vdl_info->channel))// 737-850
      {
        filter_combo = (1<<FILTERED_PATH_GSM_TX_ONLY);
      }
    }
  }

  return filter_combo;
  
#else /* MCS_LIMITS_LOW_MEMORY */
  
    WWCOEX_DUMMY_CHECK(aul_info);
    WWCOEX_DUMMY_CHECK(vdl_info);
  
    return 0;
  
#endif /* MCS_LIMITS_LOW_MEMORY */

}

/*=============================================================================

  FUNCTION:  wwcoex_get_mitigation_action

=============================================================================*/
/*!
    @brief
    Find Mitigation action based on the mode of operation of device DSDA/SGLTE+G/SGTDS+G
 
    @return
      desense type
*/
/*===========================================================================*/
cxm_action_type wwcoex_get_mitigation_action
( 
  cxm_tech_type a_tech,
  cxm_tech_type v_tech
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  cxm_action_type retVal = ACTION_NONE;
    
 /* If either victim or aggr is LTE/TDS or 
    if aggr is LTE, victim is 1X and device is 1X-SLTE 
    then apply backoff */
  if ((a_tech == CXM_TECH_LTE) || (a_tech == CXM_TECH_TDSCDMA) || 
      ((v_tech == CXM_TECH_LTE) && (a_tech == CXM_TECH_ONEX)))
  {
    retVal = ACTION_BACKOFF;
  }
  else /* For anything else, action is BLANK */
  {
    retVal = ACTION_BLANK;
  }
  
  return retVal;

#else /* MCS_LIMITS_LOW_MEMORY */
  
    WWCOEX_DUMMY_CHECK(a_tech);
    WWCOEX_DUMMY_CHECK(v_tech);
  
    return ACTION_NONE;
  
#endif /* MCS_LIMITS_LOW_MEMORY */

}

/*=============================================================================

  FUNCTION:  wwcoex_nonim_compare_freqs

=============================================================================*/
/*!
    @brief
    Compare frequencies for any non-IM issue like Blocking, RxBN, Harmonics
 
    @return
    TRUE
*/
/*===========================================================================*/
boolean wwcoex_nonim_compare_freqs
(
  wwcoex_nonim_freqcompare_in*   compare_in,
  wwcoex_nonim_freqcompare_out*  compare_out
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  cxm_tech_link_info_s *a_info, *v_info;
  uint32 i;
  boolean hmonic_result;
  cxm_tech_type a_tech, v_tech;
  wwan_coex_desense_type desense = NO_DESENSE;
  boolean found = FALSE;
  uint32 guard_band = 0;

  TTLMARKER(LMTSMGR_NONIM_COMP_IN);
  
  if (compare_in == NULL || compare_out == NULL)
  {
    TTLMARKER(LMTSMGR_NONIM_COMP_ERR1);
    return FALSE;
  }

  /* Setup default actions */
  compare_out->desense = NO_DESENSE;
  compare_out->desense_mask = NO_DESENSE;
  compare_out->action = ACTION_NONE;
  compare_out->special_ind_mask = 0;

  /* For GSM as aggressor, there is always noise desense when:
       if victim is TDS/LTE and device is SGLTE/SGTDS[+G] capable or
       if you are NOT TDS/LTE in any mode configuration */
  if (((compare_in->atech == CXM_TECH_GSM1) || 
       (compare_in->atech == CXM_TECH_GSM2) ||
       (compare_in->atech == CXM_TECH_GSM3)) && 
       ((!(LMTSMGR_IS_SGLTE_FEATURE_MODE()) && 
        (compare_in->vtech == CXM_TECH_LTE || compare_in->vtech == CXM_TECH_TDSCDMA)) ||
        (compare_in->vtech != CXM_TECH_LTE && compare_in->vtech != CXM_TECH_TDSCDMA))
     )
  {
    compare_out->desense = NOISE_DESENSE;
    compare_out->desense_mask |= (1<<NOISE_DESENSE);
    compare_out->action = wwcoex_get_mitigation_action(compare_in->atech, compare_in->vtech);
    found = TRUE;
  }

  a_tech = compare_in->atech;
  v_tech = compare_in->vtech;
  a_info = &compare_in->atulfreqinfo.link_info;
  v_info = &compare_in->vtdlfreqinfo.link_info;

  /* Check if this combo needs filtered path... */
  compare_out->special_ind_mask = wwcoex_is_filtered_path_combo(a_info, v_info);
  if (compare_out->special_ind_mask & (1<<FILTERED_PATH))
  {
    compare_out->desense = NOISE_DESENSE;
    compare_out->desense_mask |= (1<<NOISE_DESENSE);
    compare_out->action = ACTION_NONE;

    /* we are done here... */
    TTLMARKER(LMTSMGR_NONIM_COMP_FILTPATH);
    return TRUE;
  }
  
  i = wwcoex_search_list(v_tech, a_tech, v_info->band, a_info->band);
  LMTSMGR_DBG_MSG_5(LOW, "v_tech %d, a_tech %d, v_info->band %d, a_info->band %d, found: %d",
                    v_tech, a_tech, v_info->band, a_info->band, i);
  if (i < WWAN_COEX_MAX_COMBS)
  {
    TTLMARKER(LMTSMGR_NONIM_COMP_FOUND_IN);
    found = TRUE;

    /* check if there is a function pointer for channel comparison... */
    if (wwcoex_list_comb_table[i].comp_channel_func != NULL) 
    {
      desense = wwcoex_list_comb_table[i].comp_channel_func(a_info,
                                                            NULL,//vul_info
                                                            v_info);

      compare_out->desense = desense;
      compare_out->desense_mask |= (1<<desense);
      compare_out->action = wwcoex_get_mitigation_action(compare_in->atech, compare_in->vtech);
      LMTSMGR_DBG_MSG_4(MED, "Found nonIM comp func %d: tech_band_mask 0x%x desense 0x%x action 0x%x", 
                         i, wwcoex_list_comb_table[i].tech_band_mask,
                         desense, compare_out->action );
    }
    else
    {
      LMTSMGR_DBG_MSG_3(MED, "Found nonIM combo index %d: tech_band 0x%x, desense_mask 0x%x",
                       i, wwcoex_list_comb_table[i].tech_band_mask, 
                       wwcoex_list_comb_table[i].desense_mask );

      /* Does it have a blocking issue ? */
      if (wwcoex_list_comb_table[i].desense_mask & (1<<BLOCKING_DESENSE))
      {
        compare_out->desense = BLOCKING_DESENSE;
        compare_out->desense_mask |= (1<<BLOCKING_DESENSE);
        compare_out->action = ACTION_BLANK;
      }
      /* Is there a harmonic issue ? */
      else if (wwcoex_list_comb_table[i].desense_mask & (1<<HARMONIC_DESENSE))
      {
        guard_band = wwcoex_get_guard_band(i, v_tech);

        /* Check if harmonic of aggr Tx freq lies in the victim Rx bw */
        hmonic_result = wwcoex_check_harmonic(a_info->frequency,
                                              a_info->bandwidth,
                                              v_info->frequency,
                                              v_info->bandwidth,
                                  wwcoex_list_comb_table[i].max_n_for_hmonics,
                                  guard_band);

        if (hmonic_result)
        {
          compare_out->desense = HARMONIC_DESENSE;
          compare_out->desense_mask |= (1<<HARMONIC_DESENSE);
          compare_out->action = ACTION_BLANK;
        }
      }
    }
    compare_out->enh_action_mask = wwcoex_list_comb_table[i].enhanced_action_mask;

    TTLMARKER(LMTSMGR_NONIM_COMP_FOUND_OUT);
  }

  TTLMARKER(LMTSMGR_NONIM_COMP_OUT);
  return found;

#else /* MCS_LIMITS_LOW_MEMORY */
  
  WWCOEX_DUMMY_CHECK(compare_in);
  WWCOEX_DUMMY_CHECK(compare_out);
  
  TTLMARKER(LMTSMGR_NONIM_COMP_OUT);

  return FALSE;
  
#endif /* MCS_LIMITS_LOW_MEMORY */

}

/*=============================================================================

  FUNCTION:  wwcoex_im_compare_freqs

=============================================================================*/
/*!
    @brief
    Compare frequencies for checking any IM issue
 
    @return
    TRUE
*/
/*===========================================================================*/
boolean wwcoex_im_compare_freqs
(
  wwcoex_im_freqcompare_in*   compare_in,
  wwcoex_im_freqcompare_out*  compare_out
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  cxm_tech_link_info_s *aul_info, *vul_info, *vdl_info;
  boolean imtype1_result, imtype2_result;
  uint32 i;
  cxm_tech_type a_tech, v_tech;
  wwan_coex_desense_type desense = NO_DESENSE;
  boolean found = FALSE;
  uint32 guard_band = 0;

  TTLMARKER(LMTSMGR_IM_COMP_IN);
  
  if (compare_in == NULL || compare_out == NULL)
  {
    TTLMARKER(LMTSMGR_IM_COMP_ERR1);
    return FALSE;
  }

  /* Setup default actions */
  compare_out->desense = NO_DESENSE;
  compare_out->action = ACTION_NONE;

  a_tech = compare_in->atech;
  v_tech = compare_in->vtech;
  aul_info = &compare_in->atulfreqinfo.link_info;
  vdl_info = &compare_in->vtdlfreqinfo.link_info;
  vul_info = &compare_in->vtulfreqinfo.link_info;

  i = wwcoex_search_list(v_tech, a_tech, vul_info->band, aul_info->band);
  if (i < WWAN_COEX_MAX_COMBS)
  {
    /* See if we find a matching IM band combination */
    if ( (wwcoex_list_comb_table[i].desense_mask & (1<<IM_TYPE1_DESENSE)) ||
         (wwcoex_list_comb_table[i].desense_mask & (1<<IM_TYPE2_DESENSE)) ||
         (wwcoex_list_comb_table[i].desense_mask & (1<<IM_DESENSE_GPS)) )
    {
      found = TRUE;

      if (wwcoex_list_comb_table[i].comp_channel_func != NULL)
      {         
        desense = wwcoex_list_comb_table[i].comp_channel_func(aul_info,
                                                              vul_info,
                                                              vdl_info);

        LMTSMGR_MSG_3(MED, "Found IM comp func %d: tech_band 0x%x, desense %d",
                              i, wwcoex_list_comb_table[i].tech_band_mask, desense);

        compare_out->desense = desense;
        compare_out->action = wwcoex_get_mitigation_action(compare_in->atech, compare_in->vtech);
      }
      else
      {
        LMTSMGR_MSG_3(MED, "Found IM band combo %d: tech_band 0x%x desense 0x%x",
                      i, wwcoex_list_comb_table[i].tech_band_mask, 
                      wwcoex_list_comb_table[i].desense_mask );

        guard_band = wwcoex_get_guard_band(i, v_tech);

        /* Is there a IM Type 1 desense ? */
        if ( (vul_info->frequency < aul_info->frequency) &&
             (wwcoex_list_comb_table[i].desense_mask & (1<<IM_TYPE1_DESENSE)))
        {
          imtype1_result = wwcoex_check_im(aul_info,
                                         vul_info,
                                         vdl_info,
                                         &wwcoex_list_comb_table[i].imtype1_list,
                                         guard_band);

          if (imtype1_result == TRUE)
          {
            compare_out->desense = IM_TYPE1_DESENSE;
            compare_out->desense_mask |= (1<<IM_TYPE1_DESENSE);
            compare_out->action = wwcoex_get_mitigation_action(compare_in->atech, compare_in->vtech);
          }
        }
        /* Is there a IM Type 2 desense ?  */
        else if ((vul_info->frequency > aul_info->frequency) &&
               (wwcoex_list_comb_table[i].desense_mask & (1<<IM_TYPE2_DESENSE)))
        {
          imtype2_result = wwcoex_check_im(aul_info,
                                         vul_info,
                                         vdl_info,
                                         &wwcoex_list_comb_table[i].imtype2_list,
                                         guard_band);

          if (imtype2_result == TRUE)
          {
            compare_out->desense = IM_TYPE2_DESENSE;
            compare_out->desense_mask |= (1<<IM_TYPE2_DESENSE);
            compare_out->action = wwcoex_get_mitigation_action(compare_in->atech, compare_in->vtech);            
          }
        }
        else
        {
          LMTSMGR_MSG_2(ERROR, "Could not find a IM condition for freqs %d, %d",
                        vul_info->frequency, aul_info->frequency);
        }
      }
    }
  }
  
  TTLMARKER(LMTSMGR_IM_COMP_OUT);
  return found;
  
#else /* MCS_LIMITS_LOW_MEMORY */
  
  WWCOEX_DUMMY_CHECK(compare_in);
  WWCOEX_DUMMY_CHECK(compare_out);

  return FALSE;
  
#endif /* MCS_LIMITS_LOW_MEMORY */
}

/*=============================================================================

  FUNCTION:  wwcoex_gen_freq_range

=============================================================================*/
/*!
    @brief
    Generates the victim's frequency range that has any desense issue
    based on frequency information
 
    @details
      - Desense types Blocking & PA Noise will provide full frequency range
      - Other desense types will be calculated based on respective equations
 
    @return
    None
*/
/*===========================================================================*/
void wwcoex_gen_freq_range
(
   cxm_tech_type a_tech,                    /* Agressor Tech */
   cxm_tech_type v_tech,                    /* Victim Tech */
   lmtsmgr_ba_serv_info_type *aul_freqInfo, /* Agresssor Tx Freq Info */
   lmtsmgr_ba_serv_info_type *vul_freqInfo, /* Victim Tx Freq Info */
   lmtsmgr_ba_serv_info_type *vdl_freqInfo, /* Victim Rx Freq Info */
   wwan_coex_desense_type type,             /* Desense Type */
   wwan_coex_desense_mode mode,             /* Calculate for Aggr (Tx) or Victim (Rx)*/
   uint32 *freq_lower,                      /* Frequency Range (kHz) */
   uint32 *freq_upper
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

   uint32 index, guard_band;
   uint8 harm_index;

   if (freq_lower == NULL || freq_upper == NULL)
     return;

   *freq_lower = 0;
   *freq_upper = WWCOEX_MAX_FREQ;

   if (aul_freqInfo == NULL || vdl_freqInfo == NULL)
     return;

   index = wwcoex_search_list(v_tech, a_tech, vdl_freqInfo->band, aul_freqInfo->band);
   if (index >= WWAN_COEX_MAX_COMBS)
   {
     return;
   }

   guard_band = wwcoex_get_guard_band(index, v_tech);

   switch(type)
   {
     case BLOCKING_DESENSE:
     case NOISE_DESENSE:
       /* Utilize entire frequency range */
       break;

     case HARMONIC_DESENSE:
     {
       harm_index = wwcoex_list_comb_table[index].max_n_for_hmonics;
       wwcoex_gen_freq_range_harmonics(aul_freqInfo->frequency, aul_freqInfo->bandwidth, 
                                       vdl_freqInfo->frequency, vdl_freqInfo->bandwidth,
                                       harm_index, guard_band, mode,
                                       freq_lower, freq_upper);
       break;
     }

     case IM_TYPE1_DESENSE:
     case IM_TYPE2_DESENSE:
     {
       if(vul_freqInfo == NULL)
         break;

       switch (mode)
       {
         case DESENSE_AGGR:
         {
           wwcoex_gen_freq_range_im_aggr(aul_freqInfo, vul_freqInfo, vdl_freqInfo, guard_band,
                                         (type == IM_TYPE1_DESENSE)? &wwcoex_list_comb_table[index].imtype1_list :
                                         &wwcoex_list_comb_table[index].imtype2_list, freq_lower, freq_upper);
           break;
         }

         case DESENSE_VICTIM:
         {
           wwcoex_gen_freq_range_im_vict(vdl_freqInfo, aul_freqInfo->bandwidth, vul_freqInfo->bandwidth,
                                         (type == IM_TYPE1_DESENSE) ? 
                                         &wwcoex_list_comb_table[index].imtype1_list :
                                         &wwcoex_list_comb_table[index].imtype2_list, 
                                         freq_lower, freq_upper);
           break;
         }

         default:
           //mode == NONE or VICT_AGGR.. should never reach here.
           break;
       }

       break;
     }

     default:
      LMTSMGR_MSG_1(ERROR, "Frequency Range not supported for desense type %d", type);
      break;
   }

#else /* MCS_LIMITS_LOW_MEMORY */

  WWCOEX_DUMMY_CHECK(a_tech);
  WWCOEX_DUMMY_CHECK(v_tech);
  WWCOEX_DUMMY_CHECK(aul_freqInfo);
  WWCOEX_DUMMY_CHECK(vul_freqInfo);
  WWCOEX_DUMMY_CHECK(vdl_freqInfo);
  WWCOEX_DUMMY_CHECK(type);
  WWCOEX_DUMMY_CHECK(mode);
  WWCOEX_DUMMY_CHECK(freq_lower);
  WWCOEX_DUMMY_CHECK(freq_upper);

#endif /* MCS_LIMITS_LOW_MEMORY */


}

/*=============================================================================

  FUNCTION:  wwcoex_get_all_desense_combs

=============================================================================*/
/*!
    @brief
    Generates a list of bands (and their associated desense issues)
    which have desense issues with the non-blacklisted tech
 
    @param
      bl_tech - Blacklisted Tech
      nb_tech - Non-Blacklisted Tech
      nb_band - Non-Blacklisted Tech's Band
      output  - Pointer to blacklisted band list
    @details

    @return
     output will be filled with the list of blacklisted bands
     and their potential desense issues

*/
/*===========================================================================*/
void wwcoex_get_all_desense_combs
(
  cxm_tech_type bl_tech,
  cxm_tech_type nb_tech,
  uint32 nb_band,
  wwcoex_comb_lookup_out *output
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

  uint32 index, a_mask, v_mask;
  wwcoex_list_comb_rec_type *entry;
  const uint32 supp_lte_band_list[WWCOEX_LTE_BAND_LIST_SIZE] = {SYS_BAND_LTE_EUTRAN_BAND1, 
                                                                SYS_BAND_LTE_EUTRAN_BAND3,
                                                                SYS_BAND_LTE_EUTRAN_BAND4,
                                                                SYS_BAND_LTE_EUTRAN_BAND34,
                                                                SYS_BAND_LTE_EUTRAN_BAND38,
                                                                SYS_BAND_LTE_EUTRAN_BAND39,
                                                                SYS_BAND_LTE_EUTRAN_BAND41};
  const uint32 supp_tds_band_list[WWCOEX_TDS_BAND_LIST_SIZE] = {SYS_BAND_TDS_BANDA,
                                                                SYS_BAND_TDS_BANDF,
                                                                SYS_BAND_TDS_BANDE};

  if (output == NULL)
  {
    return;
  }

  output->num_entries = 0;

  /* Generate tech band mask without victim band information */
  v_mask = wwcoex_gen_tech_band_mask(nb_tech, bl_tech, nb_band, 0);

  /* Generate tech band mask without agressor band information */
  a_mask = wwcoex_gen_tech_band_mask(bl_tech, nb_tech, 0, nb_band);

  /* Check every entry in the combination table */
  for (index = 0; index<WWAN_COEX_MAX_COMBS; index++)
  {
    entry = &wwcoex_list_comb_table[index];

    if (output->num_entries >= WWCOEX_MAX_BL_BANDS)
    {
      LMTSMGR_MSG_0(ERROR, "BL Band list is full!");
      break;
    }

    /* Check if tech band mask in combination table matches blacklisted tech type, 
       non-blacklisted type & non-blacklisted band info */
    if ((uint32)((WWCOEX_TECH_BAND_MASK_WO_VBAND & entry->tech_band_mask) == v_mask) &&
        (entry->atech == nb_tech) && (entry->vtech == bl_tech))
    {
      /* This blacklisted band is a VICTIM */
      output->band_list[output->num_entries].mode = DESENSE_VICTIM;
      output->band_list[output->num_entries].desense_mask = entry->desense_mask;
      output->band_list[output->num_entries].bl_band = 
        wwcoex_get_band_from_tech_band_mask(entry->tech_band_mask, DESENSE_VICTIM);
      output->num_entries++;
    }
    else if ((uint32)((WWCOEX_TECH_BAND_MASK_WO_ABAND & entry->tech_band_mask) == a_mask) &&
             (entry->vtech == nb_tech) && (entry->atech == bl_tech))
    {
      /* This blacklisted band is an AGGRESSOR */
      output->band_list[output->num_entries].mode = DESENSE_AGGR;
      output->band_list[output->num_entries].desense_mask = entry->desense_mask;
      output->band_list[output->num_entries].bl_band = 
        wwcoex_get_band_from_tech_band_mask(entry->tech_band_mask, DESENSE_AGGR);
      output->num_entries++;
    }
  }

  /* Special case - when GSM is aggressor in DSDA, TDS/LTE is the victim,
     and the desense type is only RxBN, this interference combo is not captured
     in the table. These need to be added. */
  if (IS_GSM_TECH(nb_tech) && ((bl_tech == CXM_TECH_LTE) || (bl_tech == CXM_TECH_TDSCDMA)))
  {
    for (index=0; index<((bl_tech == CXM_TECH_LTE) ?
         WWCOEX_LTE_BAND_LIST_SIZE : WWCOEX_TDS_BAND_LIST_SIZE); index++)
    {
      if (output->num_entries >= WWCOEX_MAX_BL_BANDS)
      { 
        LMTSMGR_MSG_0(ERROR, "BL Band list is full!");
        break;
      }

      output->band_list[output->num_entries].mode = DESENSE_VICTIM;
      output->band_list[output->num_entries].desense_mask = (1<<NOISE_DESENSE);
      output->band_list[output->num_entries].bl_band = ((bl_tech == CXM_TECH_LTE) ? 
                                                       supp_lte_band_list[index] :
                                                       supp_tds_band_list[index]);
      output->num_entries++;
    }

  }

#else /* MCS_LIMITS_LOW_MEMORY */

  WWCOEX_DUMMY_CHECK(bl_tech);
  WWCOEX_DUMMY_CHECK(nb_tech);
  WWCOEX_DUMMY_CHECK(nb_band);
  WWCOEX_DUMMY_CHECK(output);

#endif /* MCS_LIMITS_LOW_MEMORY */

}

/*=============================================================================

  FUNCTION:  wwan_coex_list_init

=============================================================================*/
/*!
    @brief
    Initialize the coex freq check list
 
    @return
    TRUE
*/
/*===========================================================================*/
void wwan_coex_list_init()
{

#ifndef MCS_LIMITS_LOW_MEMORY

  uint32 i,j, mask, vband, aband;
  cxm_tech_type vtech, atech;
  wwcoex_list_comb_rec_type temp_rec;

  /* initialize the cache entry */
  for (i = 0; i<CXM_TECH_MAX; i++)
  {
    for (j=0; j<CXM_TECH_MAX; j++) 
    {
      wwan_tech_pair_desense_data[i][j].desense_type = 0;
      wwan_tech_pair_desense_data[i][j].cached_index = WWAN_COEX_MAX_COMBS;
      wwan_tech_pair_desense_data[i][j].vband = SYS_BAND_CLASS_MAX;
      wwan_tech_pair_desense_data[i][j].aband = SYS_BAND_CLASS_MAX;
    }
  }

  /* Create the band mask */
  for (i=0;i<WWAN_COEX_MAX_COMBS; i++)
  {
    atech = wwcoex_list_comb_table[i].atech;
    vtech = wwcoex_list_comb_table[i].vtech;
    aband = wwcoex_list_comb_table[i].aband;
    vband = wwcoex_list_comb_table[i].vband;

    mask = wwcoex_gen_tech_band_mask(atech, vtech, aband, vband);

    if (mask == 0)
    {
       continue;
    }

    /* Store the mask */
    wwcoex_list_comb_table[i].tech_band_mask = mask;

    /* Set the desense mask */
    wwan_tech_pair_desense_data[atech][vtech].desense_type |= ((uint32)wwcoex_list_comb_table[i].desense_mask);

    /* Check if aggr or victim is C2K.... If both are C2K or none of them are C2K, then we dont need
       to do anything... */
    if( IS_C2K_TECH(atech) && !IS_C2K_TECH(vtech) )
    {
      /* Aggressor is C2K but victim is not */
      atech = GET_OTHER_C2K_TECH(atech);

      /* Set the desense mask */
      wwan_tech_pair_desense_data[atech][vtech].desense_type|= ((uint32)wwcoex_list_comb_table[i].desense_mask);
    }
    else if (!IS_C2K_TECH(atech) && IS_C2K_TECH(vtech))
    {
      /* Aggressor is C2K but victim is not */
      vtech = GET_OTHER_C2K_TECH(vtech);

      /* Set the desense mask */
      wwan_tech_pair_desense_data[atech][vtech].desense_type |= ((uint32)wwcoex_list_comb_table[i].desense_mask);
    }
  }

  /* Sort the list in the order of tech_band_mask */
  for (i=0;i<WWAN_COEX_MAX_COMBS-1; i++)
  {
    for (j=i+1; j<WWAN_COEX_MAX_COMBS; j++)
    {
      if (wwcoex_list_comb_table[i].tech_band_mask > 
                wwcoex_list_comb_table[j].tech_band_mask)
      {
        temp_rec = wwcoex_list_comb_table[i];
        wwcoex_list_comb_table[i] = wwcoex_list_comb_table[j];
        wwcoex_list_comb_table[j] = temp_rec;
      }
    }
  }
  
#endif /* MCS_LIMITS_LOW_MEMORY */

}
/*=============================================================================

  FUNCTION:  wwcoex_check_tech_pair_im

=============================================================================*/
/*!
    @brief
    Checks if IM desense exists for tech pair
 
    @return
    TRUE
 
*/
/*===========================================================================*/
boolean wwcoex_check_tech_pair_im
(
  cxm_tech_type tech1,
  cxm_tech_type tech2
)
{

#ifndef MCS_LIMITS_LOW_MEMORY

   boolean im_req = FALSE;

   if ( ( wwan_tech_pair_desense_data[tech1][tech2].desense_type & IM_DESENSE_MASK) ||
        ( wwan_tech_pair_desense_data[tech2][tech1].desense_type & IM_DESENSE_MASK) )
   {
      im_req = TRUE;
   }

   LMTSMGR_DBG_MSG_3(MED, "Debug_skc: IM Desense check for atech %d, vtech %d, is_req: %d", 
                            tech1, tech2, im_req );
   return im_req;

#else /* MCS_LIMITS_LOW_MEMORY */

  WWCOEX_DUMMY_CHECK(tech1);
  WWCOEX_DUMMY_CHECK(tech2);

  return FALSE;

#endif /* MCS_LIMITS_LOW_MEMORY */

}

#ifndef MCS_LIMITS_LOW_MEMORY

#ifdef FEATURE_DUAL_DATA

/*=============================================================================

  FUNCTION:  wwcoex_reset_desense_stat

=============================================================================*/
/*!
    @brief
    Reset the desense stats
 
    @return
    TRUE
*/
/*===========================================================================*/
static void wwcoex_reset_desense_stat(void)
{
  lmtsmgr.desense_stats.cxm_desense_count      = 0;
  lmtsmgr.desense_stats.cxm_entries_count      = 0;
  lmtsmgr.desense_stats.cxm_report_count       = 0;
}

/*=============================================================================

  FUNCTION:  wwcoex_send_desense_ind

=============================================================================*/
/*!
    @brief
    Proces the desense timer expiry
 
    @return
    TRUE
*/
/*===========================================================================*/
static void wwcoex_send_desense_ind(void)
{

  boolean is_desense;
  uint32 i;
  cxm_coex_desense_ind_s    client_ind;
  lmtsmgr_desense_ind_type *ind;
  
  ind = &(lmtsmgr.desense_ind);
  is_desense = (lmtsmgr.desense_stats.status == SEVERE_DESENSE) ? TRUE:FALSE; 

  LMTSMGR_MSG_1(MED, "Desense indication: number of clients registered: %d",
                      ind->count);

  for(i=0;i<ind->count;i++)
  {
    LMTSMGR_MSG_2(MED, "Desense indication: result: %d, umid: %d",
                      is_desense, ind->client[i].umid);
    
    /* Populate the response */
    client_ind.desense_ind.is_desense = is_desense;
     
    /* Do a message router send on the clients UMID */
    lmtsmgr_msgr_send_msg(&client_ind.msg_hdr, 
                           ind->client[i].umid,
                           sizeof(cxm_coex_desense_ind_s));
     
  }

}

#endif /* FEATURE_DUAL_DATA */

#endif /* MCS_LIMITS_LOW_MEMORY */


/*=============================================================================

  FUNCTION:  wwcoex_init_desense_stat

=============================================================================*/
/*!
    @brief
    Initialize the desense statistics
 
    @return
    TRUE
*/
/*===========================================================================*/
void wwcoex_init_desense_stat(void)
{

#ifndef MCS_LIMITS_LOW_MEMORY

#ifdef FEATURE_DUAL_DATA

  /* initialize the counts to zero and initialize timer values 
     The timer values will be moved to EFS in the future */
     
  lmtsmgr.desense_stats.status                 = MILD_DESENSE;
  
  /* Read the thresholds from EFS */
  wwan_coex_read_desense_ind_thresh_efs();

  /* Reset the counts */
  wwcoex_reset_desense_stat();

  /* Define a timer that will be used to indicate expiry of desense period */
  timer_def(&(lmtsmgr.desense_stats.desense_timer), 
            NULL,
            rex_self(),
            LMTSMGR_DESENSE_CALC_SIG,
            NULL, 
            0);

  /* Start the desense timer */
  timer_set(&(lmtsmgr.desense_stats.desense_timer),
              lmtsmgr.desense_stats.mild_coex_time, 0, T_SEC);

#endif /* FEATURE_DUAL_DATA */

#endif /* MCS_LIMITS_LOW_MEMORY */

}


/*=============================================================================

  FUNCTION:  wwcoex_process_desense_timeout

=============================================================================*/
/*!
    @brief
    Proces the desense timer expiry
 
    @return
    TRUE
*/
/*===========================================================================*/
void wwcoex_process_desense_timeout(void)
{

#ifndef MCS_LIMITS_LOW_MEMORY

#ifdef FEATURE_DUAL_DATA

  uint32 timer;
  lmtsmgr_desense_status_e past_status;
  lmgtsmgr_desense_stats *stats;
  float ratio, factor;
  uint32 threshold = 0;

  stats = &(lmtsmgr.desense_stats);
  past_status = stats->status;

  if(past_status == MILD_DESENSE)
  {
    timer = stats->mild_coex_time;
  }
  else
  {
    timer = stats->severe_coex_time;
  }

  /* Check for division by 0 */
  if(stats->cxm_entries_count > 0)
  {
    factor = ((float)(stats->cxm_report_count*stats->delta_t_msec)/(float)(timer*1000));
    ratio  = ((float)(stats->cxm_desense_count*100)/(float)(stats->cxm_entries_count));
    threshold = (int)(factor*ratio);
  }

  LMTSMGR_MSG_5(MED, "Desense timeout: status: %d, report: %d,"
                     " entries: %d, desense: %d, threshold: %d",
                     stats->status, stats->cxm_report_count, 
                     stats->cxm_entries_count, stats->cxm_desense_count, threshold);

  if((past_status == MILD_DESENSE) &&
     (threshold > stats->severe_coex_thresh))
  {
    /* change in status. Send indication */
    stats->status = SEVERE_DESENSE;

    /* Send indication to clients */
    wwcoex_send_desense_ind();

    /* Set timer to severe desense value */
    timer = lmtsmgr.desense_stats.severe_coex_time;
  }
  else if ((past_status == SEVERE_DESENSE) &&
           (threshold < stats->mild_coex_thresh))
  {
    /* change in status. Send indication */
    stats->status = MILD_DESENSE;

    /* Send indication to clients */
    wwcoex_send_desense_ind();

    /* Set timer to severe desense value */
    timer = lmtsmgr.desense_stats.mild_coex_time;
  }

  /* Reset all counts and start again */
  wwcoex_reset_desense_stat();

  /* Start the desense timer for next set of calculations */
  timer_set(&(lmtsmgr.desense_stats.desense_timer),
              timer, 0, T_SEC);

#endif /* FEATURE_DUAL_DATA */

#endif /* MCS_LIMITS_LOW_MEMORY */

}
