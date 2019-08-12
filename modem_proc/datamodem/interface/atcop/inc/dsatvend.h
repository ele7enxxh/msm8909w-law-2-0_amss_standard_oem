#ifndef DSATVEND_H
#define DSATVEND_H
/*===========================================================================

                D A T A   S E R V I C E S

                A T  C O M M A N D   
                
                ( V E N D O R  S P E C I F I C )
                
                P R O C E S S I N G

                E X T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  data services vendor specific AT command processor.

Copyright (c) 2001 - 2015 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatvend.h_v   1.0   08 Aug 2002 11:19:44   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/inc/dsatvend.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/29/15   tk      Feature wrapped $QCVOIPM command with FEATURE_IMS.
01/23/14   sc      Added support for $QCPDPCFGEXT command.
01/07/14   sc      Converted high usage ATCoP macros to functions.
10/23/13   sc      Adding support for $QCACQDBC command to clear acq_DB
10/11/13   sc      Added support for $QCNSP, $QCSIMT, $QCRCIND commands and
                   REMOTE CALL END, REMOTE RING, REMOTE ANSWER
                   unsolicited result codes.
06/05/13   tk      ATCoP changes for SGLTE support on Dime Plus.
05/28/13   tk      Fixed $QCBANDPREF issue with WLAN France 5000 band.
09/24/12   sk      Added $QCRSRP and $QCRSRQ command Support.
09/24/12   tk      Migrated to CM APIs for MRU table access.
08/06/12   tk      Added support for $QCDRX command.
06/21/12   sk      Fixed $QCPDPCFGE apn bearer issue.
06/12/12   tk      Extended $QCSYSMODE command for 3GPP2 modes.
05/18/12   sk      Added APN bearer field to $QCPDPCFGE.
02/17/11   ua      Added support for $QCRMCALL. 
02/17/12   sk      Migrating Data related AT commands interface to unified MH.
01/19/12   sk      Feature cleanup.
10/17/11   ad      Added +CECALL support.
09/27/11   mk      Added support for $QCCLAC command.
07/05/11   nc      Added support for ^MODE,^CARDMODE,^SYSCONFIG,^SYSINFO,^DSCI.
06/07/11   nc      Added support for $QCAPNE command.
05/13/11   ad      Added $QCPDPIMSCFGE support.
03/14/11   ad      Added APN class support in $QCPDPCFGE command.
01/11/11   ad      Removed pending variable .
07/08/10   ad      Added support for $ECALL command.
05/17/10   ad      Added Support for $QCPDPCFGE.
05/10/10   kk      Mainlining pending GOBI changes.
01/18/10   bs      Added support for +PACSP.
06/10/09   ua      Added support for ^PREFMODE command. 
07/16/09   ua      Added support for $QCBANDPREF command.
12/15/09   nc      Featurisation changes for LTE.
08/04/09   nc      Added support for *CNTI.
07/15/09   sa      Added support for $CSQ command.
04/29/09   ua      Fixed compiler warnings. 
03/04/09   sa      AU level CMI modifications.
10/18/08   pp      Added support for SLIP interface [AT$QCSLIP].
02/20/08   sa      Added support for $QCSQ command.
01/11/08   sa      Added support for $QCANTE and $QCRPW commands.
03/07/07   sa      Fixed feature wrap issues
02/06/07   ar      Migrate from SIO to SMD interface.
11/03/06   snb     Dual processor changes.
09/26/06   sa      Inter RAT handover support for $QCDGEN.
05/22/06   rsl     Add support for at$qcsysmode to return current sysmode, 
				   possible values returned: HSDPA/HSUPA/HSDPA+HSUPA/WCDMA/GSM.
01/26/06   snb     Correct size of datagen_info array.
09/08/05   ar      Add support for $QCSDFTEST vendor command.
07/25/05   snb     Add constants,type,array and external functions supporting 
                   WCDMA-mode data generation command $QCDGEN.
02/24/05   snb     Added $QCDGEN command.
10/21/03   ar      Added $QCPWRDN command support.
10/08/03   ar      Add wrapper FEATURE_DSAT_BREW_SUPPORT to BREW commands.
04/17/02   rc      Removed FEATURE wrap FEATURE_DS_SOCKETS around dns 
                   functions.
08/27/01   sb      ATCOP code is now partitioned into multiple VUs. As a 
                   result of the partitioning, some of the files have to be
                   renamed. So making the file/interface name changes.
08/10/01   sjd     Initial release to MSM5200 archives.
06/01/01   sb      created file

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "sys.h"
#ifdef FEATURE_DSAT_LTE
#include "cmapi.h"
#if defined(FEATURE_VOIP) && defined(FEATURE_IMS)
#include "ims_settings_atcop.h"
#endif /* defined(FEATURE_VOIP) && defined(FEATURE_IMS) */
#endif /* FEATURE_DSAT_LTE */
#include "dsat_v.h"
#include "dsati.h"
#include "dsatctab.h"
#include "dsatcmif.h"
#ifdef FEATURE_ECALL_APP 
#include "ecall_app.h"
#endif /* FEATURE_ECALL_APP */
#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsipkt.h"
#endif /* FEATURE_DSAT_ETSI_MODE */
#include "ds_rmnet_meta_sm.h"

/*===========================================================================

                        PUBLIC DATA DECLARATIONS

===========================================================================*/
typedef struct sysconfig_s
{
  dsat_num_item_type mode_val;
  dsat_num_item_type order_val;
  dsat_num_item_type roam_val;
  dsat_num_item_type domain_val;
} dsat_sysconfig_type;

typedef struct
{
  uint32  mask;
  char   *str;
} dsat_mru_band_pref_map_gw_s_type;

typedef struct
{
  uint64  mask;
  char   *str;
} dsat_mru_band_pref_map_lte_s_type;

typedef enum
{
  DSAT_BAND_PREF_NONE = 0,
  DSAT_BAND_PREF_BC0_A,
  DSAT_BAND_PREF_BC0_B,
  DSAT_BAND_PREF_BC0,
  DSAT_BAND_PREF_BC1,
  DSAT_BAND_PREF_BC3,
  DSAT_BAND_PREF_BC4,
  DSAT_BAND_PREF_BC5,
  DSAT_BAND_PREF_GSM_DCS_1800,
  DSAT_BAND_PREF_GSM_EGSM_900,
  DSAT_BAND_PREF_GSM_PGSM_900,
  DSAT_BAND_PREF_BC6,
  DSAT_BAND_PREF_BC7,
  DSAT_BAND_PREF_BC8,
  DSAT_BAND_PREF_BC9,
  DSAT_BAND_PREF_BC10,
  DSAT_BAND_PREF_BC11,
  DSAT_BAND_PREF_BC12,
  DSAT_BAND_PREF_BC14,
  DSAT_BAND_PREF_BC15,
  DSAT_BAND_PREF_BC16,
  DSAT_BAND_PREF_GSM_450,
  DSAT_BAND_PREF_GSM_480,
  DSAT_BAND_PREF_GSM_750,
  DSAT_BAND_PREF_GSM_850,
  DSAT_BAND_PREF_GSM_RGSM_900,
  DSAT_BAND_PREF_GSM_PCS_1900,
  DSAT_BAND_PREF_WCDMA_I_IMT_2000,
  DSAT_BAND_PREF_WCDMA_II_PCS_1900,
  DSAT_BAND_PREF_WCDMA_III_1700,
  DSAT_BAND_PREF_WCDMA_IV_1700,
  DSAT_BAND_PREF_WCDMA_V_850,
  DSAT_BAND_PREF_WCDMA_VI_800,
  DSAT_BAND_PREF_WCDMA_VII_2600,
  DSAT_BAND_PREF_WCDMA_VIII_900,
  DSAT_BAND_PREF_WCDMA_IX_1700,
  DSAT_BAND_PREF_WLAN_2400_US,
  DSAT_BAND_PREF_WLAN_2400_JP,
  DSAT_BAND_PREF_WLAN_2400_ETSI,
  DSAT_BAND_PREF_WLAN_2400_SP,
  DSAT_BAND_PREF_WLAN_2400_FR,
  DSAT_BAND_PREF_WLAN_5000_US,
  DSAT_BAND_PREF_WLAN_5000_JP,
  DSAT_BAND_PREF_WLAN_5000_ETSI,
  DSAT_BAND_PREF_WLAN_5000_SP,
  DSAT_BAND_PREF_WLAN_5000_FR,
  DSAT_BAND_PREF_ANY,
  DSAT_BAND_PREF_MAX
}dsat_bandpref_e_type;


typedef enum
{
  DSAT_MRU_BAND_INPUT_GSM_450 = 0,
  DSAT_MRU_BAND_INPUT_GSM_480,
  DSAT_MRU_BAND_INPUT_GSM_750,
  DSAT_MRU_BAND_INPUT_GSM_850,
  DSAT_MRU_BAND_INPUT_GSM_EGSM_900,
  DSAT_MRU_BAND_INPUT_GSM_PGSM_900,
  DSAT_MRU_BAND_INPUT_GSM_RGSM_900,
  DSAT_MRU_BAND_INPUT_GSM_DCS_1800,
  DSAT_MRU_BAND_INPUT_GSM_PCS_1900,
  DSAT_MRU_BAND_INPUT_GSM_MAX
}dsat_mru_band_input_gsm_e_type;

typedef enum
{
  DSAT_MRU_BAND_INPUT_WCDMA_I_IMT_2000 = 0,
  DSAT_MRU_BAND_INPUT_WCDMA_II_PCS_1900,
  DSAT_MRU_BAND_INPUT_WCDMA_III_1700,
  DSAT_MRU_BAND_INPUT_WCDMA_IV_1700,
  DSAT_MRU_BAND_INPUT_WCDMA_V_850,
  DSAT_MRU_BAND_INPUT_WCDMA_VI_800,
  DSAT_MRU_BAND_INPUT_WCDMA_VII_2600,
  DSAT_MRU_BAND_INPUT_WCDMA_VIII_900,
  DSAT_MRU_BAND_INPUT_WCDMA_IX_1700,
  DSAT_MRU_BAND_INPUT_WCDMA_XI_1500,
  DSAT_MRU_BAND_INPUT_WCDMA_MAX
}dsat_mru_band_input_wcdma_e_type;

typedef enum
{
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND1 = 0,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND2,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND3,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND4,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND5,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND6,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND7,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND8,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND9,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND10,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND11,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND12,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND13,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND14,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND17,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND18,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND19,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND20,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND21,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND33,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND34,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND35,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND36,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND37,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND38,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND39,
  DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND40,
  DSAT_MRU_BAND_INPUT_LTE_MAX
}dsat_mru_band_input_lte_e_type;

typedef enum
{
  DSAT_QCSYSMODE_ID_NO_SRV = 0,
  DSAT_QCSYSMODE_ID_CDMA,
  DSAT_QCSYSMODE_ID_GSM,
  DSAT_QCSYSMODE_ID_HDR,
  DSAT_QCSYSMODE_ID_WCDMA,
  DSAT_QCSYSMODE_ID_LTE,
  DSAT_QCSYSMODE_ID_TDS,
  DSAT_QCSYSMODE_ID_HDR_REV0,
  DSAT_QCSYSMODE_ID_HDR_REVA,
  DSAT_QCSYSMODE_ID_HDR_REVB,
  DSAT_QCSYSMODE_ID_HDR_EMPA_EHRPD,
  DSAT_QCSYSMODE_ID_HDR_MMPA_EHRPD,
  DSAT_QCSYSMODE_ID_CDMA_HDR,
  DSAT_QCSYSMODE_ID_CDMA_LTE,
  DSAT_QCSYSMODE_ID_MAX
}dsat_qcsysmode_id_e_type;

typedef enum
{
  DSAT_QCSYSMODE_HS_IND_NONE,
  DSAT_QCSYSMODE_HS_IND_HSDPA,
  DSAT_QCSYSMODE_HS_IND_HSUPA,
  DSAT_QCSYSMODE_HS_IND_HSDPA_HSUPA,
  DSAT_QCSYSMODE_HS_IND_HSDPA_PLUS,
  DSAT_QCSYSMODE_HS_IND_HSDPA_PLUS_HSUPA,
  DSAT_QCSYSMODE_HS_IND_DC_HSDPA_PLUS,
  DSAT_QCSYSMODE_HS_IND_DC_HSDPA_PLUS_HSUPA,
  DSAT_QCSYSMODE_HS_IND_HSDPA_PLUS_64QAM,
  DSAT_QCSYSMODE_HS_IND_HSDPA_PLUS_HSUPA_64QAM,
  DSAT_QCSYSMODE_HS_IND_DC_HSDPA_PLUS_DC_HSUPA_SUPP_CELL,
  DSAT_QCSYSMODE_HS_IND_MAX
}dsat_qcsysmode_hs_ind_e_type;

#define DSAT_BM_32BIT( val ) ( 1<< ((uint32)(val)) )

#define CHECK_TRUE( val )  ( (val) ? TRUE : FALSE )
  
#define DSAT_BAND_CHECK64( band_pref1, band_pref2 ) \
          ( CHECK_TRUE( (uint64)band_pref1 & (uint64)band_pref2 ) )
  
#define DSAT_BAND_CHECK32( band_pref1, band_pref2 )  \
          ( CHECK_TRUE( (uint32)band_pref1 & (uint32)band_pref2 ) )

#define DSAT_BAND_ADD32( band_pref1, band_pref2 )  \
        ( (uint32)( (uint32)band_pref1 | (uint32)band_pref2 ) )

#define DSAT_MRU_BAND_PREF_NONE         SD_MRU_BAND_PREF_NONE
  
    /**< GSM band pref starts with bit 0 */
  
#define DSAT_MRU_BAND_PREF_GSM_450      SD_MRU_BAND_PREF_GSM_450
    /**< GSM band (450 MHz)                          */
  
#define DSAT_MRU_BAND_PREF_GSM_480      SD_MRU_BAND_PREF_GSM_480
    /**< GSM band (480 MHz)                          */
  
#define DSAT_MRU_BAND_PREF_GSM_750      SD_MRU_BAND_PREF_GSM_750
    /**< GSM band (750 MHz)                          */
  
#define DSAT_MRU_BAND_PREF_GSM_850      SD_MRU_BAND_PREF_GSM_850
    /**< GSM band (850 MHz)                          */
  
#define DSAT_MRU_BAND_PREF_GSM_EGSM_900 SD_MRU_BAND_PREF_GSM_EGSM_900
    /**< GSM band E-GSM (900 MHz)                    */
  
#define DSAT_MRU_BAND_PREF_GSM_PGSM_900 SD_MRU_BAND_PREF_GSM_PGSM_900
    /**< GSM band P-GSM (900 MHz)                    */
  
#define DSAT_MRU_BAND_PREF_GSM_RGSM_900 SD_MRU_BAND_PREF_GSM_RGSM_900
    /**< GSM band R-GSM (900 MHz)                    */
  
#define DSAT_MRU_BAND_PREF_GSM_DCS_1800 SD_MRU_BAND_PREF_GSM_DCS_1800
    /**< GSM band DCS-GSM (1800 MHz)                  */
  
#define DSAT_MRU_BAND_PREF_GSM_PCS_1900 SD_MRU_BAND_PREF_GSM_PCS_1900
    /**< GSM band PCS (1900 MHz)                     */
  
    /**< WCDMA band pref starts with bit 16 */
  
#define DSAT_MRU_BAND_PREF_WCDMA_I_IMT_2000  SD_MRU_BAND_PREF_WCDMA_I_IMT_2000
    /**< WCDMA EUROPE JAPAN & CHINA IMT 2100 band        */
  
#define DSAT_MRU_BAND_PREF_WCDMA_II_PCS_1900 SD_MRU_BAND_PREF_WCDMA_II_PCS_1900
    /**< WCDMA US PCS 1900 band                          */
  
#define DSAT_MRU_BAND_PREF_WCDMA_III_1700    SD_MRU_BAND_PREF_WCDMA_III_1700
    /**< WCDMA EUROPE&CHINA DCS 1800 band                */
  
#define DSAT_MRU_BAND_PREF_WCDMA_IV_1700     SD_MRU_BAND_PREF_WCDMA_IV_1700
    /**< WCDMA US 1700 band                              */
  
#define DSAT_MRU_BAND_PREF_WCDMA_V_850       SD_MRU_BAND_PREF_WCDMA_V_850
    /**< WCDMA US 850 band                               */
  
#define DSAT_MRU_BAND_PREF_WCDMA_VI_800      SD_MRU_BAND_PREF_WCDMA_VI_800
    /**< WCDMA JAPAN 800 band                            */
  
#define DSAT_MRU_BAND_PREF_WCDMA_VII_2600    SD_MRU_BAND_PREF_WCDMA_VII_2600
    /**< WCDMA EUROPE 2600 band                          */
  
#define DSAT_MRU_BAND_PREF_WCDMA_VIII_900    SD_MRU_BAND_PREF_WCDMA_VIII_900
    /**< WCDMA EUROPE & CHINA 900 band.                  */
  
#define DSAT_MRU_BAND_PREF_WCDMA_IX_1700     SD_MRU_BAND_PREF_WCDMA_IX_1700
    /**< WCDMA JAPAN 1700 band                           */
  
#define DSAT_MRU_BAND_PREF_WCDMA_XI_1500     SD_MRU_BAND_PREF_WCDMA_XI_1500
    /**< WCDMA  1500 band                                */
  
#define DSAT_MRU_BAND_PREF_ANY               SD_MRU_BAND_PREF_ANY
  
#define DSAT_MRU_BAND_PREF_LTE_EMPTY             0
       /**< No LTE MRU_BAND selected */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_1             ((uint64) SYS_BM_64BIT(0))
      /**< Acquire UL:1920-1980; DL:2110-2170 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_2             ((uint64) SYS_BM_64BIT(1))
      /**< Acquire UL:1850-1910; DL:1930-1990 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_3             ((uint64) SYS_BM_64BIT(2))
      /**< Acquire UL:1710-1785; DL:1805-1880 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_4             ((uint64) SYS_BM_64BIT(3))
      /**< Acquire UL:1710-1755; DL:2110-2115 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_5             ((uint64) SYS_BM_64BIT(4))
      /**< Acquire UL: 824- 849; DL: 869- 894 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_6             ((uint64) SYS_BM_64BIT(5))
      /**< Acquire UL: 830- 840; DL: 875- 885 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_7             ((uint64) SYS_BM_64BIT(6))
      /**< Acquire UL:2500-2570; DL:2620-2690 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_8             ((uint64) SYS_BM_64BIT(7))
      /**< Acquire 880- 915; DL: 925- 960 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_9             ((uint64) SYS_BM_64BIT(8))
      /**< Acquire UL:1749.9-1784.9; DL:1844.9-1879.9 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_10             ((uint64) SYS_BM_64BIT(9))
      /**< Acquire UL:1710-1770; DL:2110-2170 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_11             ((uint64) SYS_BM_64BIT(10))
      /**< Acquire 1427.9-1452.9; DL:1475.9-1500.9 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_12             ((uint64) SYS_BM_64BIT(11))
      /**< Acquire UL:698-716; DL:728-746 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_13             ((uint64) SYS_BM_64BIT(12))
      /**< Acquire UL: 777- 787; DL: 746-756 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_14             ((uint64) SYS_BM_64BIT(13))
      /**< Acquire UL: 788- 798; DL: 758-768 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_17             ((uint64) SYS_BM_64BIT(16))
      /**< Acquire UL: 704- 716; DL: 734-746 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_18             ((uint64) SYS_BM_64BIT(17))
      /**< Acquire UL: 815 – 830; DL: 860 – 875 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_19             ((uint64) SYS_BM_64BIT(18))
      /**< Acquire UL: 830 – 845; DL: 875 – 890 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_20             ((uint64) SYS_BM_64BIT(19))
      /**< Acquire UL: 832 – 862; DL: 791 – 821 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_21             ((uint64) SYS_BM_64BIT(20))
      /**< Acquire UL: 1447.9 – 1462.9; DL: 1495.9 – 1510.9 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_33             ((uint64) SYS_BM_64BIT(32))
      /**< Acquire UL: 1900-1920; DL: 1900-1920 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_34             ((uint64) SYS_BM_64BIT(33))
      /**< Acquire UL: 2010-2025; DL: 2010-2025 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_35             ((uint64) SYS_BM_64BIT(34))
      /**< Acquire UL: 1850-1910; DL: 1850-1910 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_36             ((uint64) SYS_BM_64BIT(35))
      /**< Acquire UL: 1930-1990; DL: 1930-1990 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_37             ((uint64) SYS_BM_64BIT(36))
      /**< Acquire UL: 1910-1930; DL: 1910-1930 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_38             ((uint64) SYS_BM_64BIT(37))
      /**< Acquire UL: 2570-2620; DL: 2570-2620 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_39             ((uint64) SYS_BM_64BIT(38))
      /**< Acquire UL: 1880-1920; DL: 1880-1920 */
  
#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_40             ((uint64) SYS_BM_64BIT(39))
      /**< Acquire UL: 2300-2400; DL: 2300-2400 */

#define DSAT_MRU_BAND_PREF_LTE_EUTRAN_MAX            ((uint64) SYS_BM_64BIT(0xFFFFFFFFFFFFFFFF))
      /**< Internal Only */

  /* equivalent to CMSS_ECIO_VALUE_NO_SIGNAL_UMTS */
#define DSAT_ECIO_NO_SIGNAL        63 
#define DSAT_BER_NO_SIGNAL         0xFF
  /* equivalent to CMSS_PATHLOSS_VALUE_NO_SIGNAL_UMTS */
#define DSAT_PATHLOSS_NO_SIGNAL     0xFF
  /* equivalent to CMSS_SIR_VALUE_NO_SIGNAL_UMTS */
#define DSAT_SIR_NO_SIGNAL         0xFF

#ifdef FEATURE_DSAT_ETSI_DATA
#define DSAT_DGEN_MIN_DATA_LEN    21
#define DSAT_DGEN_DONT_EXCEED     13500
#define DSAT_DGEN_MIN_DONT_EXCEED 1
#endif /* FEATURE_DSAT_ETSI_DATA */

/* $QCNSP values */
#define DSAT_QCNSP_RAT_MODE_AUTOMATIC              0
#define DSAT_QCNSP_RAT_MODE_GSM                    1
#define DSAT_QCNSP_RAT_MODE_WCDMA                  2
#define DSAT_QCNSP_RAT_MODE_LTE                    6
#define DSAT_QCNSP_RAT_MODE_MAX                    10

#define DSAT_QCNSP_NET_SEL_MODE_AUTOMATIC          0
#define DSAT_QCNSP_NET_SEL_MODE_MANUAL             1
#define DSAT_QCNSP_NET_SEL_MODE_LIMITED_SRV        2
#define DSAT_QCNSP_NET_SEL_MODE_MAX                3

#define DSAT_QCNSP_ACQ_ORDER_PREF_AUTOMATIC        0
#define DSAT_QCNSP_ACQ_ORDER_PREF_GSM              1
#define DSAT_QCNSP_ACQ_ORDER_PREF_WCDMA            2
#define DSAT_QCNSP_ACQ_ORDER_PREF_LTE              3
#define DSAT_QCNSP_ACQ_ORDER_PREF_MAX              4

/* ====================================================================================
 *                     QCRMCALL Definitions 
 * ==================================================================================*/
typedef enum
{
  DSAT_PENDING_QCRMCALL_NONE        = 0X0, /* None  */
  DSAT_PENDING_QCRMCALL_START_V4    = 0X1,  /* Start V4 action */
  DSAT_PENDING_QCRMCALL_START_V6    = 0X2,  /* Start V6 action */
  DSAT_PENDING_QCRMCALL_STOP_V4     = 0X4,  /* Stop V4 action */
  DSAT_PENDING_QCRMCALL_STOP_V6     = 0X8,  /* Stop V6 action */
  DSAT_PENDING_QCRMCALL_QUERY       = 0X10  /* Query */
}dsat_qcrmcall_state_e_type;

typedef struct
{
  dsat_qcrmcall_state_e_type state;
  uint32                     v4_instance;
  uint32                     v6_instance;
}dsat_qcrmcall_s_type;

typedef struct
{
  uint8                      action;
  rmnet_qcrmcall_result_type result;
} dsat_rmnet_info_s_type;

/* ===================================================================================*/
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCCLR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcclr command.
  at$qcclr command is used for clearing mobile error log.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    OK : if the command has been successfully executed
    ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Sets parameter value for "SET"  command.
  Sets response in supplied buffer for "READ" and "TEST" commands.
  Calls the necessary functions to clear the error log.

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcclr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCDMR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcdmr command.
  at$qcdmr is used to set the DM baud rate.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DS_ATCOP_OK : if the command has been successfully executed
    DS_ATCOP_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcdmr_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);



/*===========================================================================

FUNCTION DSATVEND_EXEC_QCDNSP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcdnsp command.
  at$qcdnsp is used to set primary DNS IP address.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DS_ATCOP_OK : if the command has been successfully executed
    DS_ATCOP_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcdnsp_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCDNSS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcdnss command.
  at$qcdnss is used to set secondary DNS IP address.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DS_ATCOP_OK : if the command has been successfully executed
    DS_ATCOP_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcdnss_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCTER_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcter command.
  at$qcter is used to set current and default TE-DCE baud rate.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcter_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCPWRDN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcpwrdn command.
  at$qcpwrdn is used to power down the terminal.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Phone is powered off.

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcpwrdn_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


#ifdef FEATURE_DSAT_BREW_SUPPORT
/*===========================================================================

FUNCTION DSATVEND_EXEC_BREW_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcdmg command.
  at$qcbrew is used to transition the serial port to diagnostic
  monitor.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Transitions the serial port to Brew.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_brew_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
#endif /* FEATURE_DSAT_BREW_SUPPORT */

#ifdef FEATURE_DSAT_ETSI_DATA
/*===========================================================================

FUNCTION DSATVEND_EXEC_QCDGEN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcdgen command.
  at$qcdgen is used to send a specified quantity of PS data as an IP packet 
  over a PDP-IP context already activated by +CGACT command.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Phone is powered off.

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcdgen_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCPDPCFGE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcpdpcfge command.
  at$qcpdpcfge is used to edit APN values in APN table. 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcpdpcfge_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCPDPCFGEXT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcpdpcfgext command.
  at$qcpdpcfgext is used for getting and setting MBIM context type. 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcpdpcfgext_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCPDPIMSCFGE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcpdpimscfge command.
  at$qcpdpimscfge is used to edit PDP profile registry . 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcpdpimscfge_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCAPNE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcapne command.
  at$qcapne is used to edit APN values in APN table. 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcapne_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

#endif /* FEATURE_DSAT_ETSI_DATA */

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCMRUE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$QCMRUE command.
  AT$QCMRUE is used to edit MRU database. 

  For Manual MRU :
  at$qcmrue=3,3,1,"00101"
  OK
  
  For AUTO MRU (NULL PLMN-ID) : 
  at$qcmrue=3,3,1,""
  OK

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : If the command has been successfully executed
    DSAT_ERROR : If there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcmrue_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCMRUC_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$QCMRUC command.
  AT$QCMRUC is used to clear MRU database. 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : If the command has been successfully executed
    DSAT_ERROR : If there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcmruc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCNSP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCNSP command.
  AT$QCNSP command is used to configure the 
  network selection mode.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR   : if there was any problem in executing the command
    DSAT_OK        :  if it is a success and execution completed.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_qcnsp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSAVEND_PROCESS_QCNSP_CMD

DESCRIPTION
  This function process the response for the $QCNSP command on return from
  asynchronous processing. 
 
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_process_qcnsp_cmd 
(
  dsat_cmd_pending_enum_type cmd_type,
  ds_at_cm_ph_pref_type  *pref_mode    /* Network preference mode */
);
/*===========================================================================

FUNCTION DSATVEND_EXEC_QCSIMT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcsimt command.
  This function returns SIM TYPE
    * RUIM
    * USIM
    * SIM

DEPENDENCIES
  None
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success and execution completed.
  
SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_qcsimt_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATVEND_MRU_ACTION

DESCRIPTION
  This function perfoms the action on the MRU tables

DEPENDENCIES
  Phone should be in Offline.

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_EVENT: If there is a further Async calls made
    DSAT_OK: If Everything is fine.
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_mru_action
(
  dsat_cmd_pending_enum_type cmd_type
);

/*===========================================================================

FUNCTION DSATVEND_PROCESS_MRU_UPDATE_RESULT

DESCRIPTION
  This function processes the MRU update result.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_EVENT: If there is a further Async calls made
    DSAT_OK: If Everything is fine.
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_process_mru_update_result
(
  cm_ph_cmd_err_e_type err
);

/*===========================================================================

FUNCTION DSATVEND_PROCESS_MRU_READ_RESULT

DESCRIPTION
  This function processes the MRU read result.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_EVENT: If there is a further Async calls made
    DSAT_OK: If Everything is fine.
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_process_mru_read_result
(
  cm_ph_cmd_err_e_type             err,
  cm_mmode_mru_table_entry_u_type *mru_entry
);
#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCATMOD_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcatmod command.
  AT$QCATMOD is used indicate the SIOLIB AT Command Processing State.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcatmod_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT*/
#ifdef FEATURE_DATA_UCSD_SCUDIF_TEST

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCSCFTEST_CMD

DESCRIPTION
  This function sets the SCUIDF test mode.

DEPENDENCIES
  The values exported may or may not be appropriate for the mode-specific
  handler.  Validation in mode-specific handler is assumed. 
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcscftest_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

#endif /* FEATURE_DATA_UCSD_SCUDIF_TEST */
/*===========================================================================

FUNCTION DSATVEND_EXEC_QCSYSMODE_CMD

DESCRIPTION
  This function returns the current system mode.

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcsysmode_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSAVEND_PROCESS_QCSYSMODE_CMD

DESCRIPTION
  This function process the response for the ^QCSYSMODE command
  on return from asynchronous processing.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_process_qcsysmode_cmd
(
  dsat_cm_msg_s_type *msg_ptr
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCANTE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $QCANTE command.
  The result is in res_buff_ptr, reporting number of antenna bars
  in a scale of 0-4.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success and execution completed.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcante_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCRPW_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $QCRPW command.
  The result is in res_buff_ptr, reporting recieved radio signal 
  power in a scale of 0-75.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success and execution completed.


SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcrpw_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCSQ_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcsq command.
  This function returns RSCP, ECIO,SIR,PATHLOSS and RSSI if present
  in the following format $QCSQ: <rscp>,<ecio>,<sir>,<pathloss>,<rssi>.

DEPENDENCIES
  None
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success and execution completed.
  
SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcsq_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


#ifdef FEATURE_DATA_PS_SLIP
/*===========================================================================

FUNCTION DSATVEND_EXEC_QCSLIP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCSLIP command.
  AT$QCSLIP command is used to switch to SLIP mode.
  It brings up the SLIP iface and applications can use the SLIP iface for
  data transfer.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR   : if there was any problem in executing the command
    DSAT_CONNECT : if SLIP is ready

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcslip_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
#endif /* FEATURE_DATA_PS_SLIP */

#ifdef FEATURE_DSAT_DEV_CMDS
#ifdef FEATURE_DSAT_ETSI_MODE
/*===========================================================================

FUNCTION DSATVEND_EXEC_QCCNTI_CMD

DESCRIPTION
  This function takes the result from the command line parser and
  executes it. It executes *CNTI command, which deals as follows:
  *CNTI=<n>
  *CNTI: <n>,<tech>[,<tech>[...]]
    <n>: parameter controls the results displayed by the response
  
    0- Technology currently in use to access the network  
    1- The available technologies on the current network  
    2- All technologies supported by the device
  
  <tech>: alphanumeric string used to identify technology
   GSM
   GPRS
   EDGE
   UMTS
   HSDPA
   HSUPA

DEPENDENCIES
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
                        
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution
SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvend_exec_qccnti_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
#endif /* FEATURE_DSAT_ETSI_MODE */
#endif /* FEATURE_DSAT_DEV_CMDS */
/*===========================================================================

FUNCTION DSATVEND_EXEC_QCBANDPREF_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcbandpref command.
  This command provides the ability to set/get the band preferences

  
<band_pref>: Is a string containing indexes separated by comma. 
for eg: "1,2,3,4,5,6" which means set all the bandprefs which has indexes from
1,2,3,4,5,6 in the test command.

DEPENDENCIES
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if success.
SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcbandpref_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
/*===========================================================================

FUNCTION DSATVEND_EXEC_PREFMODE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT^PREFMODE command.
  This command provides the ability to set/get the network mode preferences. 

  
<pref_mode> network mode, values as follows:
0 Automatic
2 CDMA mode
4 HDR mode
8 CDMA/HDR HYBRID mode
 
The below note is ignored:
For the data card which only support CDMA 1X, the command isn't realized. 


DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if success.
SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvend_exec_prefmode_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
FUNCTION DSATVEND_PROCESS_PREFMODE_CMD

DESCRIPTION
  This function process the response for the ^PREFMODE command on return from
  asynchronous processing. 
 
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_process_prefmode_cmd 
(
  dsat_cmd_pending_enum_type cmd_type,        /* cmd type */
  ds_at_cm_ph_pref_type  *pref_mode    /* Network preference mode */
);

/*===========================================================================

FUNCTION DSATVEND_PROCESS_SYSCONFIG_CMD

DESCRIPTION
  This function process the response for the ^SYSCONFIG command on return from
  asynchronous processing. 
 
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_CMD_ERR_RESP:  If mode value is not valid. 
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_process_sysconfig_cmd 
(
  dsat_cmd_pending_enum_type cmd_type,
  ds_at_cm_ph_pref_type  *pref_mode    /* Network preference mode */
);
/*===========================================================================
  FUNCTION DSATVEND_EXEC_SYSINFO_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT^SYSINFO command.

  The command inquires the current system message. Such as 
  system service status, domain, roam, system mode, UIM card status, etc.

  <srv_status> system service status, values as follows:
  0 no service
  1 limited service
  2 service available
  3 limited area service
  4 power saving and dormancy status.

  <srv_domain> system service, values as follows:
  0 no service
  1 only CS service
  2 only PS service
  3 PS+CS service
  4 CS and PS don't register and are in the status of serching.
  255 CDMA doesn't support.

  <roam_status> roaming status, values as follows:
  0 non-roaming status.
  1 roaming status.

  <sys_mode> system mode, values as follows:
  0 no service
  1 AMPS mode (not use provisionally)
  2 CDMA mode
  3 GSM/GPRS mode
  4 HDR mode
  5 WCDMA mode
  6 GPS mode
  7 GSM/WCDMA
  8 CDMA/HDR HYBRID

  <sim_stat> UIM card status, values as follows:
  1 UIM card status available
  240 ROMSIM version
  255 UIM card doesn't exist

  <lock_state> CDMA production doesn't use the parameter.
  <sys_submode> CDMA production doesn't use the parameter.


  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_ASYNC_CMD : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsatvend_exec_sysinfo_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
); /* dsatvend_exec_sysinfo_cmd */

/*===========================================================================
  FUNCTION DSATVEND_EXEC_SYSCONFIG_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ^SYSCONFIG command

^ SYSCONFIG =  <mode>,<acqorder>,<roam>,<srvdomain>	<CR><LF>OK<CR><LF>
^ SYSCONFIG?
    ^SYSCONFIG:<mode>,<acqorder>,<roam>,<srvdomain><CR><LF><CR><LF>OK<CR><LF>

  <mode> system mode reference:
  2    Automatically select
  13   GSM ONLY
  14   WCDMA ONLY
  15   TDSCDMA ONLY
  16   no change
  <acqorder>: network accessing order reference:
  0    Automatically
  1    GSM first, UTRAN second
  2    UTRAN first, GSM second
  3    No change
  <roam>: roaming support:
  0	not support
  1	can roam
  2   No change
  <srvdomain>: domain configuration:
  0	CS_ONLY
  1	PS_ONLY
  2   CS_PS
  3   ANY
  4   No change

  DEPENDENCIES
  None

  RETURN VALUE
  
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_ASYNC_CMD : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsatvend_exec_sysconfig_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
); /* dsatvend_exec_sysconfig_cmd */

/*===========================================================================

FUNCTION DSATVEND_EXEC_PACSP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+PACSP command.
  This function returns the current PLMN mode bit setting of EF CSP file.

DEPENDENCIES
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : If the data is read at initalization, result is returned
              immediately. This is another success case. 
SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvend_exec_pacsp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCBOOTVER_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $QCBOOTVER command.
  The result is in res_buff_ptr, reporting boot code build info.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :         if it is a success and execution completed.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcbootver_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCVOLT_CMD

DESCRIPTION
  This function returns the main chip voltage.

DEPENDENCIES
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success and execution completed.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcvolt_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCHWREV_CMD

DESCRIPTION
  This function returns the MSM chip hw revision info

DEPENDENCIES
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success and execution completed.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvend_exec_qchwrev_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

#ifdef FEATURE_DSAT_GOBI_MAINLINE
/*===========================================================================

FUNCTION DSATVEND_EXEC_QCTEMP_CMD

DESCRIPTION
  This function returns the current RF PA temperature in C 

DEPENDENCIES
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success and execution completed.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvend_exec_qctemp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCAGC_CMD

DESCRIPTION
  Get current AGC value from specified band/channel
   
  AT$QCAGC <band>,<rx_chan>[,<path>]
     band
     chan
     path  ["main"] "aux"

   band -
      WCDMA_IMT     
      WCDMA_800    (Cell)
      WCDMA_1900   (PCS)
      CDMA_800    (Cell)
      CDMA_1900   (PCS)
   
  Note: Restricted to FTM
        

DEPENDENCIES
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success and execution completed.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcagc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
  );


#ifdef FEATURE_WCDMA
/*===========================================================================

FUNCTION DSATVEND_EXEC_QCALLUP_CMD

DESCRIPTION
  Turn RF transmitter on / off for specified band tuned to the first channel
   
  AT$QCALLUP <band>,<tx_chan>[,on|off]
  
   band -
      WCDMA_IMT     
      WCDMA_800    (Cell)
      WCDMA_1900   (PCS)
   
  Note: Restricted to FTM

DEPENDENCIES
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success and execution completed.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcallup_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
#endif  /* FEATURE_WCDMA */

/*===========================================================================

FUNCTION DSATVEND_FTM_DIAGPKY_RSP_HANDLER

DESCRIPTION
   Called in DS context for DS_AT_FTM_DIAGPKT_RSP_CMD event.
   This event is used to execute callbacks to process FTM diag packets.

DEPENDENCIES
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success and execution completed.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvend_ftm_diagpkt_rsp_handler
(
  ds_cmd_type         *cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================

FUNCTION dsatvend_exec_qcsku_cmd

DESCRIPTION
  This function returns the SKU info

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcsku_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
#endif /* FEATURE_DSAT_GOBI_MAINLINE */
#ifdef FEATURE_ECALL_APP
/*===========================================================================
FUNCTION DSATVEND_ECALL_AT_CMD_HANDLER

DESCRIPTION
  This function is handler function for reporting ecall session response.

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_NO_CARRIER  : If the Ecall Session status is a failure.
  DSAT_ASYNC_EVENT : Any other case.
 
SIDE EFFECTS
  None
  
======================================================================*/
dsat_result_enum_type dsatvend_ecall_at_cmd_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);
/*===========================================================================
FUNCTION  DSAT_ECALL_CB_FUNC

DESCRIPTION
  ECALL status command callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/


LOCAL void dsat_ecall_cb_func 
( 
  ecall_session_status_type  session_status 
);

/*===========================================================================

FUNCTION DSAVEND_EXEC_ECALL_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$ECALL command.
  This command provides the ability to make a eCall to the network. 

dsat_ecall_val[0]  -> Start/Stop the eCall 0 - Stop ; 1 - Start
dsat_ecall_val[1]  -> Type of the eCall    0 - Test ; 1 - Emergency
dsat_ecall_val[2]  -> Activation type      0 - Manual; 1- Automatic

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_ecall_cmd 
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
#endif /* FEATURE_ECALL_APP */
/*===========================================================================

FUNCTION DSATVEND_EXEC_QCSKU_CMD

DESCRIPTION
  This function returns the SKU info

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcsku_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
/*===========================================================================

FUNCTION DSATVEND_EXEC_QCGSN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +GSN command.
  It reads the required NV items (ESN/MEID) and prints them out.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:        syntax error.
    DSAT_OK          : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcgsn_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCCLAC_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $QCCLAC command.
  It reads all the supported AT commands and prints them.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :         if it is a success and execution completed.
SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcclac_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
);

/*===========================================================================
FUNCTION   DSATVEND_IFACE_EV_HANDLER

DESCRIPTION
 This is a handler function for all the Rmnet releated events. 
 ATCoP is intrested in Rmnet START, STOP and QUERY events. 

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK: Call is connected or teard down succesfully. 
  DSAT_ASYNC_EVENT: Waiting for more events before command processing is complete. 
  DSAT_CMD_ERR_RSP: If connection failed. 

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsatvend_rmnet_ev_handler
(
  ds_cmd_type * cmd_ptr     /* DS Command pointer */
);

/*===========================================================================

FUNCTION DSATVEND_QCRMCALL_ABORT_CMD_HANDLER

DESCRIPTION
  This function is an abort command handler, which stops the current originate 
  request which if it is still in progress. 

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:        syntax error.
    DSAT_ASYNC_CMD          : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatvend_qcrmcall_abort_cmd_handler
(
  const dsati_cmd_type *cmd_table_entry_ptr  /* Ptr to cmd entry in table. */
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCRMCALL_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $QCRMCALL command.
  $QCRMCALL command is used to trigger a RmNet call via AT command. 
  It just passes on the Start Network Interface parameters into RmNet to
  bring up the call. 

  $QCRMCALL =<action>, <instance> [,<IP type> [,<Act> [,<umts_profile> [,<cdma_profile> [,<APN>]]]]]

   Response:
   $QCRMCALL:<Instance>, <V4>
   $QCRMCALL:<Instance>, <V6> 
   OK

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:    syntax error.
    DSAT_ASYNC_CMD : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcrmcall_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

#ifdef FEATURE_DSAT_LTE
/*===========================================================================

FUNCTION DSATVEND_EXEC_QCACQDB_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCACQDBC command.
  at$QCACQDBC is used to clear ACQ database. 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcacqdbc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

#if defined(FEATURE_VOIP) && defined(FEATURE_IMS)
/*===========================================================================
FUNCTION   DSAT_CONVERT_AMRNUM_TO_AMRSTR

DESCRIPTION
  Function will convert AMR number value int AMR string. It will check the SET bit in given number
  and generate corresponding string.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void dsat_convert_amrnum_to_amrstr
(
  char                   *rsp_buf,
  dsat_num_item_type      buf_len,
  dsat_num_item_type      amr_num
);

/*===========================================================================
FUNCTION DSATVEND_VOIP_CONFIG_CB

DESCRIPTION
  This function handles set/get response from IMS.  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void dsatvend_voipm_config_cb
(
  ims_settings_at_cmd_rsp_info at_cmd_rsp_info
);
/*===========================================================================
FUNCTION DSATVEND_VOIPM_CONFIG_HANDLER

DESCRIPTION
  Handler function for IMS  response.

DEPENDENCIES
  None
  
RETURN VALUE
  
DSAT_ERROR : if there was any problem in executing the command.
DSAT_OK : if it is a success and execution completed.

SIDE EFFECTS
  None
  
======================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_voipm_config_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCVOIPM_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCVOIPM command.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command.
    DSAT_ASYNC_CMD : if success.
    DSAT_OK : if it is a success and execution completed.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcvoipm_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
#endif /* defined(FEATURE_VOIP) && defined(FEATURE_IMS) */
/*===========================================================================

FUNCTION DSATVEND_EXEC_QCDRX_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $QCDRX command.
  This command provides the ability to set/get the drx coefficient.

  <drx_coefficient> drx coefficient, values as follows:
    0 Not specified by MS
    6 CN = 6, T = 32
    7 CN = 7, T = 64
    8 CN = 8, T = 128
    9 CN = 9, T = 256

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command.
    DSAT_ASYNC_CMD : if success.
    DSAT_OK : if it is a success and execution completed.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcdrx_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
FUNCTION DSATVEND_PROCESS_QCDRX_CMD

DESCRIPTION
  This function process the response for the $QCDRX command on return from
  asynchronous processing.
 
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command.
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_process_qcdrx_cmd
(
  dsat_cmd_pending_enum_type cmd_type,  /* cmd type */
  ds_at_ph_info_u_type  *ph_info        /* PH info */
);
/*===========================================================================
FUNCTION DSATVEND_PROCESS_QCRSRP_CMD

DESCRIPTION
  This function executes $QCRSRP and $QCRSRQ commands and provides response 
  Calls CM API to receive neighbour cell info like Cell id,EARFCN,RSRP,RSRQ.
 
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command.
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcrsrp_cmd
(
    dsat_mode_enum_type mode,             /*  AT command mode:            */
    const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
    const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
    dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
#endif /* FEATURE_DSAT_LTE */

#ifdef FEATURE_SGLTE
/*===========================================================================

FUNCTION DSATVEND_EXEC_QCHCOPS_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes
  it. It executes $QCHCOPS command, a read only command and a version of
  +COPS read command for hybrid stack.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP: if there was a problem in lower layers
    DSAT_ASYNC_CMD : if aynchronous command continues successfully
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_qchcops_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCHCREG_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes
  it. It executes $QCHCREG command, a read only command and a version of
  +CREG read command for hybrid stack.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_qchcreg_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
#endif /* FEATURE_SGLTE */

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCPRFMOD_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This command is used to configure parameters 
  for 3GPP2/EPC profile.
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcprfmod_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCPRFCRT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This command is used to create or delete 
  any 3GPP2/EPC profile.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcprfcrt_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

#endif /* DSATVEND_H */
