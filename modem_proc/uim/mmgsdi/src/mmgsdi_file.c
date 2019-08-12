/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            M M G S D I   F I L E   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the processing of all MMGSDI file related
  functions

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_file.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/20/16   ar      Don't RESET card if EF-DIR req is not processed by UIMDRV
05/22/16   vdc     Remove F3 messages for memory allocation failure
05/12/16   tkl     F3 log prints cleanup
05/02/16   ar      Check service availability before allowing EF GSM MBI read
08/10/15   vdc     Retry read request for non spec compliant cards
07/14/15   ar      Correct the caching logic of EF-PRL and EF-EPRL
07/14/15   ar      Cache complete data for req's with partial read/write data
04/30/15   lxu     Support refresh on EF GBABP
04/17/15   nr      Phonebook record read optimization
09/09/14   kk      Enhancements in NV handling
08/27/14   yt      Support for restricted SIM access via MMGSDI session APIs
08/12/14   hh      Skip reading EF ARR only if permitted
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/15/14   yt      Support for NV refresh
06/10/14   tl      Introduce EF 3GPDUPPExt
06/03/14   tl      Remove additional parameter variables
05/28/14   am      Correct file id changes introduced by SFI
04/29/14   nmb     Re-enable AT+CRSM after directory maintenance changes
04/17/14   av      Enhance MMGSDI attributes cache
03/13/14   av      Fix incorrect EF category check in is_access_outside_adf
03/06/14   av      Add missing files in the MMGSDI enum tables
02/22/14   tkl     Initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "comdef.h"
#include "intconv.h"
#include "msg.h"
#include "uim_msg.h"
#include "mmgsdi_file.h"
#include "uim_v.h"
#include "mmgsdiutil.h"
#include "mmgsdicache.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_uim_icc.h"
#include "mmgsdi_nv.h"


/*===========================================================================

                        DEFINITION DEPENDANCIES

===========================================================================*/
#define MMGSDI_READ_ALLOWED_MASK           0x01
#define MMGSDI_WRITE_ALLOWED_MASK          0x02
#define MMGSDI_INCREASE_ALLOWED_MASK       0x04
#define MMGSDI_ACTIVATE_ALLOWED_MASK       0x08
#define MMGSDI_DEACTIVATE_ALLOWED_MASK     0x10

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/



/* ===========================================================================
   STRUCTURE:      mmgsdi_file_ENUM_TABLE_TYPE

===========================================================================*/
typedef PACKED struct PACKED_POST
{
  mmgsdi_file_enum_type                     mmgsdi_file;
  uint16                                    file_id;
  mmgsdi_service_enum_type                  service_type;
} mmgsdi_file_enum_table_type;


/* ===========================================================================
   STRUCTURE:      MMGSDI_DEFAULT_FILE_SEC_ATTR_TABLE_TYPE

   DESCRIPTION:
     This structure contains the mmgsdi_file_enum_type (only for the files in
     MMGSDI cache list) and a bitmask for the corresponding default security
     attributes derived from what is mentioned in the spec. Starting with
     LSBit, the bits in sec_attr_mask correspond to READ, WRITE, INCREASE,
     ACTIVATE, and DEACTIVATE. If an operation is allowed, the corresponding
     bit is set in the mask.
===========================================================================*/
typedef PACKED struct PACKED_POST
{
  mmgsdi_file_enum_type          file;
  uint8                          sec_attr_mask;
} mmgsdi_file_default_file_sec_attr_table_type;


/* ===========================================================================
   STRUCTURE:      mmgsdi_file_MAPPING_TABLE_TYPE

===========================================================================*/
typedef PACKED struct PACKED_POST
{
  uint8                                       mmgsdi_file_index;
  uim_path_type                               path_to_df;
  uint16                                      mmgsdi_file_enum_table_count;
  const mmgsdi_file_enum_table_type          *mmgsdi_file_enum_table_ptr;
  uint16                                      mmgsdi_file_sfi_table_count;
  const mmgsdi_file_sfi_table_type           *mmgsdi_file_sfi_table_ptr;
} mmgsdi_file_index_table_type;


/*===========================================================================

                        TABLES DECLARATIONS

===========================================================================*/
static const mmgsdi_ef_srv_from_app_type mmgsdi_file_ef_srv_from_app_type_table[] =
{
  {MMGSDI_IMAGE,        MMGSDI_GSM_SRV_IMAGE,  MMGSDI_NONE,
                        MMGSDI_USIM_SRV_IMAGE, MMGSDI_NONE},

  {MMGSDI_TELECOM_ADN,  MMGSDI_GSM_SRV_ADN,    MMGSDI_CDMA_SRV_LOCAL_PHONEBOOK,
                        MMGSDI_NONE,           MMGSDI_NONE},

  {MMGSDI_TELECOM_FDN,  MMGSDI_GSM_SRV_FDN,    MMGSDI_CDMA_SRV_FDN,
                        MMGSDI_USIM_SRV_FDN,   MMGSDI_CSIM_SRV_FDN},

  {MMGSDI_TELECOM_LND,  MMGSDI_GSM_SRV_LND,    MMGSDI_CDMA_SRV_LND,
                        MMGSDI_NONE,           MMGSDI_NONE},

  {MMGSDI_TELECOM_SDN,  MMGSDI_GSM_SRV_SDN,    MMGSDI_CDMA_SRV_SDN,
                        MMGSDI_USIM_SRV_SDN,   MMGSDI_CSIM_SRV_SDN},

  {MMGSDI_TELECOM_EXT1, MMGSDI_GSM_SRV_EXT1,   MMGSDI_CDMA_SRV_EXT1,
                        MMGSDI_NONE,           MMGSDI_NONE},

  {MMGSDI_TELECOM_EXT2, MMGSDI_GSM_SRV_EXT2,   MMGSDI_CDMA_SRV_EXT2,
                        MMGSDI_USIM_SRV_EXT2,  MMGSDI_CSIM_SRV_EXT2},

  {MMGSDI_TELECOM_EXT3, MMGSDI_GSM_SRV_EXT3,   MMGSDI_CDMA_SRV_EXT3,
                        MMGSDI_USIM_SRV_EXT3,  MMGSDI_CSIM_SRV_EXT3},

  {MMGSDI_IMAGE_FILE,   MMGSDI_GSM_SRV_IMAGE,  MMGSDI_NONE,
                        MMGSDI_USIM_SRV_IMAGE, MMGSDI_CSIM_SRV_IMAGE},

  {MMGSDI_TELECOM_MML,  MMGSDI_GSM_SRV_MMS,    MMGSDI_CDMA_SRV_MMS,
                        MMGSDI_USIM_SRV_MMS,   MMGSDI_CSIM_SRV_MMS},

  {MMGSDI_TELECOM_MMDF, MMGSDI_NONE,           MMGSDI_NONE,
                        MMGSDI_USIM_SRV_MM_STORAGE, MMGSDI_CSIM_SRV_MM_STORAGE},
};

const mmgsdi_file_enum_table_type mmgsdi_file_ef_under_mf_enum_table[] =
{
  {MMGSDI_ICCID,                            0x2FE2, MMGSDI_NONE},
  {MMGSDI_ELP,                              0x2F05, MMGSDI_NONE},
  {MMGSDI_DIR,                              0x2F00, MMGSDI_NONE},
  {MMGSDI_ARR,                              0x2F06, MMGSDI_NONE}
}; /* mmgsdi_uim_ef_under_mf_enum_table */


/* EFs in DF CDMA */
const mmgsdi_file_enum_table_type mmgsdi_file_cdma_enum_table[] =
{
  {MMGSDI_CDMA_CC,                          0x6F21, MMGSDI_NONE},
  {MMGSDI_CDMA_IMSI_M,                      0x6F22, MMGSDI_NONE},
  {MMGSDI_CDMA_IMSI_T,                      0x6F23, MMGSDI_NONE},
  {MMGSDI_CDMA_TMSI,                        0x6F24, MMGSDI_NONE},
  {MMGSDI_CDMA_ANALOG_HOME_SID,             0x6F25, MMGSDI_NONE},
  {MMGSDI_CDMA_ANALOG_OP_PARAMS,            0x6F26, MMGSDI_NONE},
  {MMGSDI_CDMA_ANALOG_LOCN_AND_REGN_IND,    0x6F27, MMGSDI_NONE},
  {MMGSDI_CDMA_HOME_SID_NID,                0x6F28, MMGSDI_NONE},
  {MMGSDI_CDMA_ZONE_BASED_REGN_IND,         0x6F29, MMGSDI_NONE},
  {MMGSDI_CDMA_SYS_REGN_IND,                0x6F2A, MMGSDI_NONE},

  /* 10 */
  {MMGSDI_CDMA_DIST_BASED_REGN_IND,         0x6F2B, MMGSDI_NONE},
  {MMGSDI_CDMA_ACCOLC,                      0x6F2C, MMGSDI_NONE},
  {MMGSDI_CDMA_CALL_TERM_MODE_PREF,         0x6F2D, MMGSDI_NONE},
  {MMGSDI_CDMA_SCI,                         0x6F2E, MMGSDI_NONE},
  {MMGSDI_CDMA_ANALOG_CHAN_PREF,            0x6F2F, MMGSDI_NONE},
  {MMGSDI_CDMA_PRL,                         0x6F30, MMGSDI_NONE},
  {MMGSDI_CDMA_RUIM_ID,                     0x6F31, MMGSDI_NONE},
  {MMGSDI_CDMA_SVC_TABLE,                   0x6F32, MMGSDI_NONE},
  {MMGSDI_CDMA_SPC,                         0x6F33, MMGSDI_NONE},
  {MMGSDI_CDMA_OTAPA_SPC_ENABLE,            0x6F34, MMGSDI_NONE},

  /* 20 */
  {MMGSDI_CDMA_NAM_LOCK,                    0x6F35, MMGSDI_NONE},
  {MMGSDI_CDMA_OTASP_OTAPA_FEATURES,        0x6F36, MMGSDI_NONE},
  {MMGSDI_CDMA_SERVICE_PREF,                0x6F37, MMGSDI_NONE},
  {MMGSDI_CDMA_ESN_ME,                      0x6F38, MMGSDI_NONE},
  {MMGSDI_CDMA_RUIM_PHASE,                  0x6F39, MMGSDI_NONE},
  {MMGSDI_CDMA_PREF_LANG,                   0x6F3A, MMGSDI_NONE},
  {MMGSDI_CDMA_UNASSIGNED_1,                0x6F3B, MMGSDI_NONE},
  {MMGSDI_CDMA_SMS,                         0x6F3C, MMGSDI_CDMA_SRV_SMS},
  {MMGSDI_CDMA_SMS_PARAMS,                  0x6F3D, MMGSDI_CDMA_SRV_SMSP},
  {MMGSDI_CDMA_SMS_STATUS,                  0x6F3E, MMGSDI_CDMA_SRV_SMS},

  /* 30 */
  {MMGSDI_CDMA_SUP_SVCS_FEATURE_CODE_TABLE, 0x6F3F, MMGSDI_NONE},
  {MMGSDI_CDMA_UNASSIGNED_2,                0x6F40, MMGSDI_NONE},
  {MMGSDI_CDMA_HOME_SVC_PVDR_NAME,          0x6F41, MMGSDI_CDMA_SRV_SPN},
  {MMGSDI_CDMA_UIM_ID_USAGE_IND,            0x6F42, MMGSDI_NONE},
  {MMGSDI_CDMA_ADM_DATA,                    0x6F43, MMGSDI_NONE},
  {MMGSDI_CDMA_MSISDN,                      0x6F44, MMGSDI_NONE},
  {MMGSDI_CDMA_MAXIMUM_PRL,                 0x6F45, MMGSDI_NONE},
  {MMGSDI_CDMA_SPC_STATUS,                  0x6F46, MMGSDI_NONE},
  {MMGSDI_CDMA_ECC,                         0x6F47, MMGSDI_NONE},
  {MMGSDI_CDMA_3GPD_ME3GPDOPC,              0x6F48, MMGSDI_NONE},

  /* 40 */
  {MMGSDI_CDMA_3GPD_3GPDOPM,                0x6F49, MMGSDI_NONE},
  {MMGSDI_CDMA_3GPD_SIPCAP,                 0x6F4A, MMGSDI_NONE},
  {MMGSDI_CDMA_3GPD_MIPCAP,                 0x6F4B, MMGSDI_NONE},
  {MMGSDI_CDMA_3GPD_SIPUPP,                 0x6F4C, MMGSDI_NONE},
  {MMGSDI_CDMA_3GPD_MIPUPP,                 0x6F4D, MMGSDI_NONE},
  {MMGSDI_CDMA_3GPD_SIPSP,                  0x6F4E, MMGSDI_NONE},
  {MMGSDI_CDMA_3GPD_MIPSP,                  0x6F4F, MMGSDI_NONE},
  {MMGSDI_CDMA_3GPD_SIPPAPSS,               0x6F50, MMGSDI_NONE},
  {MMGSDI_CDMA_UNASSIGNED_3,                0x6F51, MMGSDI_NONE},
  {MMGSDI_CDMA_UNASSIGNED_4,                0x6F52, MMGSDI_NONE},

  /* 50 */
  {MMGSDI_CDMA_PUZL,                        0x6F53, MMGSDI_NONE},
  {MMGSDI_CDMA_MAXPUZL,                     0x6F54, MMGSDI_NONE},
  {MMGSDI_CDMA_MECRP,                       0x6F55, MMGSDI_NONE},
  {MMGSDI_CDMA_HRPDCAP,                     0x6F56, MMGSDI_NONE},
  {MMGSDI_CDMA_HRPDUPP,                     0x6F57, MMGSDI_NONE},
  {MMGSDI_CDMA_CSSPR,                       0x6F58, MMGSDI_NONE},
  {MMGSDI_CDMA_ATC,                         0x6F59, MMGSDI_NONE},
  {MMGSDI_CDMA_EPRL,                        0x6F5A, MMGSDI_NONE},
  {MMGSDI_CDMA_BCSMS_CONFIG,                0x6F5B, MMGSDI_NONE},
  {MMGSDI_CDMA_BCSMS_PREF,                  0x6F5C, MMGSDI_NONE},

  /* 60 */
  {MMGSDI_CDMA_BCSMS_TABLE,                 0x6F5D, MMGSDI_NONE},
  {MMGSDI_CDMA_BCSMS_PARAMS,                0x6F5E, MMGSDI_NONE},
  {MMGSDI_CDMA_MMS_NOTIF,                   0x6F65, MMGSDI_NONE},
  {MMGSDI_CDMA_MMS_EXT8,                    0x6F66, MMGSDI_NONE},
  {MMGSDI_CDMA_MMS_ICP,                     0x6F67, MMGSDI_NONE},
  {MMGSDI_CDMA_MMS_UP,                      0x6F68, MMGSDI_NONE},
  {MMGSDI_CDMA_SMS_CAP,                     0x6F76, MMGSDI_NONE},
  {MMGSDI_CDMA_3GPD_IPV6CAP,                0x6F77, MMGSDI_NONE},
  {MMGSDI_CDMA_3GPD_MIPFLAGS,               0x6F78, MMGSDI_NONE},
  {MMGSDI_CDMA_3GPD_TCPCONFIG,              0x6F79, MMGSDI_NONE},

  /* 70 */
  {MMGSDI_CDMA_3GPD_DGC,                    0x6F7A, MMGSDI_NONE},
  {MMGSDI_CDMA_BROWSER_CP,                  0x6F7B, MMGSDI_NONE},
  {MMGSDI_CDMA_BROWSER_BM,                  0x6F7C, MMGSDI_NONE},
  {MMGSDI_CDMA_3GPD_SIPUPPEXT,              0x6F7D, MMGSDI_NONE},
  {MMGSDI_CDMA_MMS_CONFIG,                  0x6F7E, MMGSDI_NONE},
  {MMGSDI_CDMA_JAVA_DURL,                   0x6F7F, MMGSDI_NONE},
  {MMGSDI_CDMA_3GPD_MIPUPPEXT,              0x6F80, MMGSDI_NONE},
  {MMGSDI_CDMA_BREW_DLOAD,                  0x6F81, MMGSDI_CDMA_SRV_BREW},
  {MMGSDI_CDMA_BREW_TSID,                   0x6F82, MMGSDI_CDMA_SRV_BREW},
  {MMGSDI_CDMA_BREW_SID,                    0x6F83, MMGSDI_CDMA_SRV_BREW},

  /* 80 */
  {MMGSDI_CDMA_LBS_XCONFIG,                 0x6F84, MMGSDI_CDMA_SRV_LBS},
  {MMGSDI_CDMA_LBS_XSURL,                   0x6F85, MMGSDI_CDMA_SRV_LBS},
  {MMGSDI_CDMA_LBS_V2CONFIG,                0x6F86, MMGSDI_CDMA_SRV_LBS},
  {MMGSDI_CDMA_LBS_V2PDEADDR,               0x6F87, MMGSDI_CDMA_SRV_LBS},
  {MMGSDI_CDMA_LBS_V2MPCADDR,               0x6F88, MMGSDI_CDMA_SRV_LBS},
  {MMGSDI_CDMA_BREW_AEP,                    0x6F89, MMGSDI_CDMA_SRV_BREW},
  {MMGSDI_CDMA_MODEL,                       0x6F90, MMGSDI_NONE},
  {MMGSDI_CDMA_RC,                          0x6F91, MMGSDI_NONE},
  {MMGSDI_CDMA_APP_LABELS,                  0x6F92, MMGSDI_NONE},
  {MMGSDI_CDMA_USER_AGENT_STRING,           0x6FE3, MMGSDI_NONE},

  /* 90 */
  {MMGSDI_CDMA_GID_REFERENCES,              0x6FE6, MMGSDI_NONE},
  {MMGSDI_CDMA_ME_DOWNLOADABLE_DATA,        0x6FE8, MMGSDI_NONE},
  {MMGSDI_CDMA_ME_SETTING_DATA,             0x6FE9, MMGSDI_NONE},
  {MMGSDI_CDMA_ME_USER_DATA,                0x6FEA, MMGSDI_NONE},
  {MMGSDI_CDMA_RESERVED2,                   0x6FEB, MMGSDI_NONE},
  {MMGSDI_CDMA_RESERVED1,                   0x6FEC, MMGSDI_NONE},
  {MMGSDI_CDMA_UIM_SVC_TABLE,               0x6FED, MMGSDI_NONE},
  {MMGSDI_CDMA_UIM_FEATURES,                0x6FEE, MMGSDI_NONE},
  {MMGSDI_CDMA_UIM_VERSION,                 0x6FEF, MMGSDI_NONE},
  {MMGSDI_CDMA_HRPD_HRPDUPP,                0x6F57, MMGSDI_NONE},
  {MMGSDI_CDMA_SF_EUIM_ID,                  0x6F74, MMGSDI_NONE},

  /* 100 */
  {MMGSDI_CDMA_IMSI_STATUS,                 0x6FE7, MMGSDI_NONE},
  {MMGSDI_CDMA_GID1,                        0x6FE4, MMGSDI_NONE},
  {MMGSDI_CDMA_GID2,                        0x6FE5, MMGSDI_NONE},
  {MMGSDI_CDMA_3GPD_OP_CAP,                 0x6F48, MMGSDI_NONE},
  {MMGSDI_CDMA_3GPD_UPPEXT,                 0x6F7D, MMGSDI_CDMA_SRV_3GPD_EXT}
  /* Need to additionally check (MMGSDI_CDMA_SRV_3GPD_SIP || MMGSDI_CDMA_SRV_3GPD_MIP) in mmgsdi_util_is_file_ok_in_svc_table */
}; /* mmgsdi_file_cdma_enum_table */

/* EFs in DF GSM */
const mmgsdi_file_enum_table_type mmgsdi_file_gsm_enum_table[] =
{
  {MMGSDI_GSM_LP,                           0x6F05, MMGSDI_NONE},
  {MMGSDI_GSM_IMSI,                         0x6F07, MMGSDI_NONE},
  {MMGSDI_GSM_KC,                           0x6F20, MMGSDI_NONE},
  {MMGSDI_GSM_PLMN,                         0x6F30, MMGSDI_GSM_SRV_PLMN},
  {MMGSDI_GSM_HPLMN,                        0x6F31, MMGSDI_NONE},
  {MMGSDI_GSM_ACM_MAX,                      0x6F37, MMGSDI_GSM_SRV_AOC},
  {MMGSDI_GSM_SST,                          0x6F38, MMGSDI_NONE},
  {MMGSDI_GSM_ACM,                          0x6F39, MMGSDI_GSM_SRV_AOC},
  {MMGSDI_GSM_GID1,                         0x6F3E, MMGSDI_GSM_SRV_GID1},
  {MMGSDI_GSM_GID2,                         0x6F3F, MMGSDI_GSM_SRV_GID2},

  /* 10 */
  {MMGSDI_GSM_SPN,                          0x6F46, MMGSDI_GSM_SRV_SPN},
  {MMGSDI_GSM_PUCT,                         0x6F41, MMGSDI_GSM_SRV_AOC},
  {MMGSDI_GSM_CBMI,                         0x6F45, MMGSDI_GSM_SRV_CBMI},
  {MMGSDI_GSM_BCCH,                         0x6F74, MMGSDI_NONE},
  {MMGSDI_GSM_ACC,                          0x6F78, MMGSDI_NONE},
  {MMGSDI_GSM_FPLMN,                        0x6F7B, MMGSDI_NONE},
  {MMGSDI_GSM_LOCI,                         0x6F7E, MMGSDI_NONE},
  {MMGSDI_GSM_AD,                           0x6FAD, MMGSDI_NONE},
  {MMGSDI_GSM_PHASE,                        0x6FAE, MMGSDI_NONE},
  {MMGSDI_GSM_VGCS,                         0x6FB1, MMGSDI_GSM_SRV_VGCS},

  /* 20 */
  {MMGSDI_GSM_VGCSS,                        0x6FB2, MMGSDI_GSM_SRV_VGCS},
  {MMGSDI_GSM_VBS,                          0x6FB3, MMGSDI_GSM_SRV_VBS},
  {MMGSDI_GSM_VBSS,                         0x6FB4, MMGSDI_GSM_SRV_VBS},
  {MMGSDI_GSM_EMLPP,                        0x6FB5, MMGSDI_GSM_SRV_ENH_ML_SVC},
  {MMGSDI_GSM_AAEM,                         0x6FB6, MMGSDI_GSM_SRV_AA_EMLPP},
  {MMGSDI_GSM_CBMID,                        0x6F48, MMGSDI_GSM_SRV_DATA_DL_SMSCB},
  {MMGSDI_GSM_ECC,                          0x6FB7, MMGSDI_NONE},
  {MMGSDI_GSM_CBMIR,                        0x6F50, MMGSDI_GSM_SRV_CBMIR},
  {MMGSDI_GSM_DCK,                          0x6F2C, MMGSDI_GSM_SRV_DCK},
  {MMGSDI_GSM_CNL,                          0x6F32, MMGSDI_GSM_SRV_CNL},

  /* 30 */
  {MMGSDI_GSM_NIA,                          0x6F51, MMGSDI_GSM_SRV_NIA},
  {MMGSDI_GSM_KCGPRS,                       0x6F52, MMGSDI_GSM_SRV_GPRS},
  {MMGSDI_GSM_LOCIGPRS,                     0x6F53, MMGSDI_GSM_SRV_GPRS},
  {MMGSDI_GSM_SUME,                         0x6F54, MMGSDI_NONE},
  {MMGSDI_GSM_PLMNWACT,                     0x6F60, MMGSDI_GSM_SRV_UPLMN_SEL_WACT},
  {MMGSDI_GSM_OPLMNWACT,                    0x6F61, MMGSDI_GSM_SRV_OPLMN_SEL_WACT},
  {MMGSDI_GSM_HPLMNACT,                     0x6F62, MMGSDI_GSM_SRV_HPLMN_WACT},
  {MMGSDI_GSM_CPBCCH,                       0x6F63, MMGSDI_GSM_SRV_CPBCCH},
  {MMGSDI_GSM_INVSCAN,                      0x6F64, MMGSDI_GSM_SRV_INV_SCAN},
  {MMGSDI_GSM_RPLMNAT,                      0x6F65, MMGSDI_GSM_SRV_RPLMN_LACT},

  /* 40 */
  {MMGSDI_GSM_PNN,                          0x6FC5, MMGSDI_GSM_SRV_PLMN_NTWRK_NAME},
  {MMGSDI_GSM_OPL,                          0x6FC6, MMGSDI_GSM_SRV_OPLMN_LIST},
  {MMGSDI_GSM_MBDN,                         0x6FC7, MMGSDI_GSM_SRV_MDN},
  {MMGSDI_GSM_EXT6,                         0x6FC8, MMGSDI_GSM_SRV_MDN}, /* This EF is an extension of MBDN.
                                                                            So, assume same service for this
                                                                            EF as for MBDN */
  {MMGSDI_GSM_MBI,                          0x6FC9, MMGSDI_GSM_SRV_MDN},
  {MMGSDI_GSM_MWIS,                         0x6FCA, MMGSDI_GSM_SRV_MWI},
  {MMGSDI_GSM_EXT1,                         0x6F4A, MMGSDI_GSM_SRV_EXT1},
  {MMGSDI_GSM_SPDI,                         0x6FCD, MMGSDI_GSM_SRV_SPDI},
  {MMGSDI_GSM_CFIS,                         0x6FCB, MMGSDI_GSM_SRV_CFI},
  /* CPHS */
  {MMGSDI_GSM_VMWI,                         0x6F11, MMGSDI_NONE},

  /* 50 */
  {MMGSDI_GSM_SVC_STR_TBL,                  0x6F12, MMGSDI_NONE},
  {MMGSDI_GSM_CFF,                          0x6F13, MMGSDI_NONE},
  {MMGSDI_GSM_ONS,                          0x6F14, MMGSDI_NONE},
  {MMGSDI_GSM_ONS_SHORT,                    0x6F18, MMGSDI_NONE},
  {MMGSDI_GSM_CSP,                          0x6F15, MMGSDI_NONE},
  {MMGSDI_GSM_CPHSI,                        0x6F16, MMGSDI_NONE},
  {MMGSDI_GSM_MN,                           0x6F17, MMGSDI_NONE}
}; /* mmgsdi_file_gsm_enum_table */

/* EFs at SoLSA */
const mmgsdi_file_enum_table_type mmgsdi_file_gsm_solsa_enum_table[] =
{
  {MMGSDI_GSM_SAI,                          0x4F30, MMGSDI_GSM_SRV_SOLSA},
  {MMGSDI_GSM_SLL,                          0x4F31, MMGSDI_GSM_SRV_SOLSA}
}; /* mmgsdi_file_gsm_solsa_enum_table */

/* EFs at MExE Level */
const mmgsdi_file_enum_table_type mmgsdi_file_gsm_mexe_enum_table[] =
{
  {MMGSDI_GSM_MEXE_ST,                      0x4F40, MMGSDI_GSM_SRV_MEXE},
  {MMGSDI_GSM_ORPK,                         0x4F41, MMGSDI_GSM_SRV_MEXE},
  {MMGSDI_GSM_ARPK,                         0x4F42, MMGSDI_GSM_SRV_MEXE},
  {MMGSDI_GSM_TPRPK,                        0x4F43, MMGSDI_GSM_SRV_MEXE}
}; /* mmgsdi_file_gsm_mexe_enum_table */


/* EFs at the telecom DF in GSM and CDMA */
const mmgsdi_file_enum_table_type mmgsdi_file_telecom_enum_table[] =
{
  /* 0 */
  {MMGSDI_TELECOM_ADN,                      0x6F3A, MMGSDI_GET_SRV_FROM_APP_TYPE},
  {MMGSDI_TELECOM_FDN,                      0x6F3B, MMGSDI_GET_SRV_FROM_APP_TYPE},
  {MMGSDI_TELECOM_SMS,                      0x6F3C, MMGSDI_GSM_SRV_SMS},
  {MMGSDI_TELECOM_CCP,                      0x6F3D, MMGSDI_GSM_SRV_CCP},
  {MMGSDI_TELECOM_ECCP,                     0x6F4F, MMGSDI_GSM_SRV_ECCP},
  {MMGSDI_TELECOM_MSISDN,                   0x6F40, MMGSDI_GSM_SRV_MSISDN},
  {MMGSDI_TELECOM_SMSP,                     0x6F42, MMGSDI_GSM_SRV_SMSP},
  {MMGSDI_TELECOM_SMSS,                     0x6F43, MMGSDI_GSM_SRV_SMS},
  {MMGSDI_TELECOM_LND,                      0x6F44, MMGSDI_GET_SRV_FROM_APP_TYPE},
  {MMGSDI_TELECOM_SDN,                      0x6F49, MMGSDI_GET_SRV_FROM_APP_TYPE},

  /* 10 */
  {MMGSDI_TELECOM_EXT1,                     0x6F4A, MMGSDI_GET_SRV_FROM_APP_TYPE},
  {MMGSDI_TELECOM_EXT2,                     0x6F4B, MMGSDI_GET_SRV_FROM_APP_TYPE},
  {MMGSDI_TELECOM_EXT3,                     0x6F4C, MMGSDI_GET_SRV_FROM_APP_TYPE},
  {MMGSDI_TELECOM_BDN,                      0x6F4D, MMGSDI_GSM_SRV_BDN},
  {MMGSDI_TELECOM_EXT4,                     0x6F4E, MMGSDI_GSM_SRV_EXT4},
  {MMGSDI_TELECOM_SMSR,                     0x6F47, MMGSDI_GSM_SRV_SMSR},
  {MMGSDI_TELECOM_CMI,                      0x6F58, MMGSDI_GSM_SRV_BDN},
  {MMGSDI_TELECOM_SUME,                     0x6F54, MMGSDI_NONE},
  {MMGSDI_TELECOM_ARR,                      0x6F06, MMGSDI_NONE},
  {MMGSDI_TELECOM_ICE_DN,                   0x6FE0, MMGSDI_NONE},

  /* 20 */
  {MMGSDI_TELECOM_ICE_FF,                   0x6FE1, MMGSDI_NONE},
  {MMGSDI_TELECOM_PSISMSC,                  0x6FE5, MMGSDI_USIM_SRV_SMSP},
}; /* mmgsdi_file_telecom_enum_table */

/* EFs at DF GRAPHICS under DF Telecom */
const mmgsdi_file_enum_table_type mmgsdi_file_telecom_graphics_enum_table[] =
{
  {MMGSDI_IMAGE,                            0x4F20, MMGSDI_GET_SRV_FROM_APP_TYPE},
  {MMGSDI_IMAGE_FILE,                       0x4F20, MMGSDI_GET_SRV_FROM_APP_TYPE},
  {MMGSDI_TELECOM_ICE_GRAPHICS,             0x4F21, MMGSDI_NONE}
}; /* mmgsdi_file_telecom_graphics_enum_table */

/* EFs at the DF PHONEBOOK under  DF Telecom */
const mmgsdi_file_enum_table_type mmgsdi_file_telecom_phonebook_enum_table[] =
{
  {MMGSDI_TELECOM_PBR,                      0x4F30, MMGSDI_NONE},
  {MMGSDI_TELECOM_PSC,                      0x4F22, MMGSDI_NONE},
  {MMGSDI_TELECOM_CC,                       0x4F23, MMGSDI_NONE},
  {MMGSDI_TELECOM_PUID,                     0x4F24, MMGSDI_NONE}
}; /* mmgsdi_file_telecom_phonebook_enum_table */

/* EFs at the DF Multimedia under  DF Telecom */
const mmgsdi_file_enum_table_type mmgsdi_file_telecom_mm_enum_table[] =
{
  {MMGSDI_TELECOM_MML,                      0x4F47, MMGSDI_GET_SRV_FROM_APP_TYPE},
  {MMGSDI_TELECOM_MMDF,                     0x4F48, MMGSDI_GET_SRV_FROM_APP_TYPE}
}; /* mmgsdi_file_telecom_mm_enum_table */

/* EFs at the DF Multimode System Selection under DF Telecom */
const mmgsdi_file_enum_table_type mmgsdi_file_telecom_mmss_enum_table[] =
{
  {MMGSDI_TELECOM_MLPL,                     0x4F20, MMGSDI_NONE},
  {MMGSDI_TELECOM_MSPL,                     0x4F21, MMGSDI_NONE},
  {MMGSDI_TELECOM_MMSSMODE,                 0x4F22, MMGSDI_NONE}
}; /* mmgsdi_file_telecom_mmss_enum_table */

/* EFs of USIM ADF */
const mmgsdi_file_enum_table_type mmgsdi_file_usim_enum_table[] =
{
  /* 0 */
  {MMGSDI_USIM_LI,                          0x6F05, MMGSDI_NONE},
  {MMGSDI_USIM_IMSI,                        0x6F07, MMGSDI_NONE},
  {MMGSDI_USIM_KEYS,                        0x6F08, MMGSDI_NONE},
  {MMGSDI_USIM_KEYSPS,                      0x6F09, MMGSDI_NONE},
  {MMGSDI_USIM_PLMNWACT,                    0x6F60, MMGSDI_USIM_SRV_UPLMN_SEL_WACT},
  {MMGSDI_USIM_UPLMNSEL,                    0x6F30, MMGSDI_NONE},
  {MMGSDI_USIM_HPLMN,                       0x6F31, MMGSDI_NONE},
  {MMGSDI_USIM_ACM_MAX,                     0x6F37, MMGSDI_USIM_SRV_AOC},
  {MMGSDI_USIM_UST,                         0x6F38, MMGSDI_NONE},
  {MMGSDI_USIM_ACM,                         0x6F39, MMGSDI_USIM_SRV_AOC},

  /* 10 */
  {MMGSDI_USIM_GID1,                        0x6F3E, MMGSDI_USIM_SRV_GID1},
  {MMGSDI_USIM_GID2,                        0x6F3F, MMGSDI_USIM_SRV_GID2},
  {MMGSDI_USIM_SPN,                         0x6F46, MMGSDI_USIM_SRV_SPN},
  {MMGSDI_USIM_PUCT,                        0x6F41, MMGSDI_USIM_SRV_AOC},
  {MMGSDI_USIM_CBMI,                        0x6F45, MMGSDI_USIM_SRV_CBMI},
  {MMGSDI_USIM_ACC,                         0x6F78, MMGSDI_NONE},
  {MMGSDI_USIM_FPLMN,                       0x6F7B, MMGSDI_NONE},
  {MMGSDI_USIM_LOCI,                        0x6F7E, MMGSDI_NONE},
  {MMGSDI_USIM_AD,                          0x6FAD, MMGSDI_NONE},
  {MMGSDI_USIM_CBMID,                       0x6F48, MMGSDI_USIM_SRV_DATA_DL_SMSCB},

  /* 20 */
  {MMGSDI_USIM_ECC,                         0x6FB7, MMGSDI_NONE},
  {MMGSDI_USIM_CBMIR,                       0x6F50, MMGSDI_USIM_SRV_CBMIR},
  {MMGSDI_USIM_PSLOCI,                      0x6F73, MMGSDI_NONE},
  {MMGSDI_USIM_FDN,                         0x6F3B, MMGSDI_USIM_SRV_FDN},
  {MMGSDI_USIM_SMS,                         0x6F3C, MMGSDI_USIM_SRV_SMS},
  {MMGSDI_USIM_MSISDN,                      0x6F40, MMGSDI_USIM_SRV_MSISDN},
  {MMGSDI_USIM_SMSP,                        0x6F42, MMGSDI_USIM_SRV_SMSP},
  {MMGSDI_USIM_SMSS,                        0x6F43, MMGSDI_USIM_SRV_SMS},
  {MMGSDI_USIM_SDN,                         0x6F49, MMGSDI_USIM_SRV_SDN},
  {MMGSDI_USIM_EXT2,                        0x6F4B, MMGSDI_USIM_SRV_EXT2},

  /* 30 */
  {MMGSDI_USIM_EXT3,                        0x6F4C, MMGSDI_USIM_SRV_EXT3},
  {MMGSDI_USIM_SMSR,                        0x6F47, MMGSDI_USIM_SRV_SMSR},
  {MMGSDI_USIM_ICI,                         0x6F80, MMGSDI_USIM_SRV_ICI_ICT},
  {MMGSDI_USIM_OCI,                         0x6F81, MMGSDI_USIM_SRV_OCI_OCT},
  {MMGSDI_USIM_ICT,                         0x6F82, MMGSDI_USIM_SRV_ICI_ICT},
  {MMGSDI_USIM_OCT,                         0x6F83, MMGSDI_USIM_SRV_OCI_OCT},
  {MMGSDI_USIM_EXT5,                        0x6F4E, MMGSDI_USIM_SRV_EXT5},
  {MMGSDI_USIM_CCP2,                        0x6F4F, MMGSDI_USIM_SRV_CCP},
  {MMGSDI_USIM_EMLPP,                       0x6FB5, MMGSDI_USIM_SRV_ENH_ML_SVC},
  {MMGSDI_USIM_AAEM,                        0x6FB6, MMGSDI_USIM_SRV_AA_EMLPP},

  /* 40 */
  {MMGSDI_USIM_GMSI,                        0x6FC2, MMGSDI_NONE},
  {MMGSDI_USIM_HIDDENKEY,                   0x6FC3, MMGSDI_NONE},
  {MMGSDI_USIM_BDN,                         0x6F4D, MMGSDI_USIM_SRV_BDN},
  {MMGSDI_USIM_EXT4,                        0x6F55, MMGSDI_USIM_SRV_EXT4},
  {MMGSDI_USIM_CMI,                         0x6F58, MMGSDI_USIM_SRV_BDN},
  {MMGSDI_USIM_EST,                         0x6F56, MMGSDI_NONE},
  {MMGSDI_USIM_ACL,                         0x6F57, MMGSDI_USIM_SRV_ACL},
  {MMGSDI_USIM_DCK,                         0x6F2C, MMGSDI_USIM_SRV_DCK},
  {MMGSDI_USIM_CNL,                         0x6F32, MMGSDI_USIM_SRV_CNL},
  {MMGSDI_USIM_START_HFN,                   0x6F5B, MMGSDI_NONE},

  /* 50 */
  {MMGSDI_USIM_THRESHOLD,                   0x6F5C, MMGSDI_NONE},
  {MMGSDI_USIM_OPLMNWACT,                   0x6F61, MMGSDI_USIM_SRV_OPLMN_SEL_WACT},
  {MMGSDI_USIM_OPLMNSEL,                    0x6F5D, MMGSDI_NONE},
  {MMGSDI_USIM_HPLMNWACT,                   0x6F62, MMGSDI_USIM_SRV_HPLMN_WACT},
  {MMGSDI_USIM_ARR,                         0x6F06, MMGSDI_NONE},
  {MMGSDI_USIM_RPLMNACT,                    0x6F65, MMGSDI_NONE},
  {MMGSDI_USIM_NETPAR,                      0x6FC4, MMGSDI_NONE},
  {MMGSDI_USIM_PNN,                         0x6FC5, MMGSDI_USIM_SRV_PLMN_NTWRK_NAME},
  {MMGSDI_USIM_OPL,                         0x6FC6, MMGSDI_USIM_SRV_OPLMN_LIST},
  {MMGSDI_USIM_MBDN,                        0x6FC7, MMGSDI_USIM_SRV_MDN},

  /* 60 */
  {MMGSDI_USIM_EXT6,                        0x6FC8, MMGSDI_USIM_SRV_MDN}, /* This EF is an extension of MBDN.
                                                                             So, assume same service for this
                                                                             EF as for MBDN */
  {MMGSDI_USIM_MBI,                         0x6FC9, MMGSDI_USIM_SRV_MDN},
  {MMGSDI_USIM_MWIS,                        0x6FCA, MMGSDI_USIM_SRV_MWI},
  {MMGSDI_USIM_SPDI,                        0x6FCD, MMGSDI_USIM_SRV_SPDI},
  {MMGSDI_USIM_EHPLMN,                      0x6FD9, MMGSDI_USIM_SRV_EHPLMN},
  {MMGSDI_USIM_CFIS,                        0x6FCB, MMGSDI_USIM_SRV_CFI},
  /* MBMS */
  {MMGSDI_USIM_VGCSCA,                      0x6FD4, MMGSDI_USIM_SRV_VGCS_SECURITY},
  {MMGSDI_USIM_VBSCA,                       0x6FD5, MMGSDI_USIM_SRV_VBS_SECURITY},
  {MMGSDI_USIM_GBABP,                       0x6FD6, MMGSDI_USIM_SRV_GBA},
  {MMGSDI_USIM_GBANL,                       0x6FDA, MMGSDI_USIM_SRV_GBA},

  /* 70 */
  {MMGSDI_USIM_MSK,                         0x6FD7, MMGSDI_USIM_SRV_MBMS_SECURITY},
  {MMGSDI_USIM_MUK,                         0x6FD8, MMGSDI_USIM_SRV_MBMS_SECURITY},
  /* CPHS */
  {MMGSDI_USIM_VMWI,                        0x6F11, MMGSDI_NONE},
  {MMGSDI_USIM_SVC_STR_TBL,                 0x6F12, MMGSDI_NONE},
  {MMGSDI_USIM_CFF,                         0x6F13, MMGSDI_NONE},
  {MMGSDI_USIM_ONS,                         0x6F14, MMGSDI_NONE},
  {MMGSDI_USIM_ONS_SHORT,                   0x6F18, MMGSDI_NONE},
  {MMGSDI_USIM_CSP,                         0x6F15, MMGSDI_NONE},
  {MMGSDI_USIM_CPHSI,                       0x6F16, MMGSDI_NONE},
  {MMGSDI_USIM_MN,                          0x6F17, MMGSDI_NONE},

  /* 80 */
  /* EFs for MMS Notification */
  {MMGSDI_USIM_MMSN,                        0x6FCE, MMGSDI_USIM_SRV_MMS},
  {MMGSDI_USIM_MMSICP,                      0x6FD0, MMGSDI_USIM_SRV_MMS},
  {MMGSDI_USIM_MMSUP,                       0x6FD1, MMGSDI_USIM_SRV_MMS},
  {MMGSDI_USIM_MMSUCP,                      0x6FD2, MMGSDI_SRV_MAX}, /* MMGSDI_USIM_SRV_MMS && MMGSDI_USIM_SRV_MMS_USR_P */
  {MMGSDI_USIM_EXT8,                        0x6FCF, MMGSDI_USIM_SRV_EXT8},
  {MMGSDI_USIM_EHPLMNPI,                    0x6FDB, MMGSDI_USIM_SRV_EHPLMNI},
  {MMGSDI_USIM_LRPLMNSI,                    0x6FDC, MMGSDI_USIM_LAST_RPMN_SEL_IND},
  {MMGSDI_USIM_EPSLOCI,                     0x6FE3, MMGSDI_USIM_SRV_EPS_MMI},
  {MMGSDI_USIM_EPSNSC,                      0x6FE4, MMGSDI_USIM_SRV_EPS_MMI},
  {MMGSDI_USIM_SPNI,                        0x6FDE, MMGSDI_USIM_SRV_SPNI},

  /* 90 */
  {MMGSDI_USIM_PNNI,                        0x6FDF, MMGSDI_USIM_SRV_PNNI},
  {MMGSDI_USIM_NCP_IP,                      0x6FE2, MMGSDI_USIM_SRV_NCP_IP},
  {MMGSDI_USIM_NASCONFIG,                   0x6FE8, MMGSDI_USIM_SRV_NAS_CONFIG},
  {MMGSDI_USIM_PWS,                         0x6FEC, MMGSDI_USIM_SRV_PWS},
    /* Efs for PKCS #15 support */
  {MMGSDI_PKCS15_ODF,                       0x5031, MMGSDI_NONE},
  {MMGSDI_PKCS15_TI,                        0x5032, MMGSDI_NONE},
  {MMGSDI_PKCS15_US,                        0x5033, MMGSDI_NONE}
}; /* mmgsdi_file_usim_enum_table*/

/*EFs at DF PHONEBOOK under USIM ADF */
const mmgsdi_file_enum_table_type mmgsdi_file_usim_phonebook_enum_table[] =
{
  {MMGSDI_USIM_PBR,                         0x4F30, MMGSDI_NONE},
  {MMGSDI_USIM_PSC,                         0x4F22, MMGSDI_NONE},
  {MMGSDI_USIM_CC,                          0x4F23, MMGSDI_NONE},
  {MMGSDI_USIM_PUID,                        0x4F24, MMGSDI_NONE}
}; /* mmgsdi_file_usim_phonebook_enum_table*/

/*EFs at DF GSM under USIM ADF */
const mmgsdi_file_enum_table_type mmgsdi_file_usim_gsm_access_enum_table[] =
{
  {MMGSDI_USIM_KC,                          0x4F20, MMGSDI_USIM_SRV_GSM_ACCESS},
  {MMGSDI_USIM_KCGPRS,                      0x4F52, MMGSDI_USIM_SRV_GSM_ACCESS},
  {MMGSDI_USIM_CPBCCH,                      0x4F63, MMGSDI_USIM_SRV_CPBCCH},
  {MMGSDI_USIM_INVSCAN,                     0x4F64, MMGSDI_USIM_SRV_INV_SCAN}
}; /* mmgsdi_file_usim_gsm_access_enum_table*/

/*EFs at DF MEXe under USIM ADF */
const mmgsdi_file_enum_table_type mmgsdi_file_usim_mexe_enum_table[] =
{
  {MMGSDI_USIM_MEXE_ST,                     0x4F40, MMGSDI_USIM_SRV_MEXE},
  {MMGSDI_USIM_ORPK,                        0x4F41, MMGSDI_USIM_SRV_MEXE},
  {MMGSDI_USIM_ARPK,                        0x4F42, MMGSDI_USIM_SRV_MEXE},
  {MMGSDI_USIM_TPRPK,                       0x4F43, MMGSDI_USIM_SRV_MEXE}
}; /* mmgsdi_file_usim_mexe_enum_table*/

/*EFs at DF WLAN under USIM ADF */
const mmgsdi_file_enum_table_type mmgsdi_file_usim_wlan_enum_table[] =
{
  {MMGSDI_USIM_PSEUDO,                      0x4F41, MMGSDI_USIM_SRV_PSEUDONYM},
  {MMGSDI_USIM_UPLMNWLAN,                   0x4F42, MMGSDI_USIM_SRV_UPLMN_WLAN_ACC},
  {MMGSDI_USIM_0PLMNWLAN,                   0x4F43, MMGSDI_USIM_SRV_OPLMN_WLAN_ACC},
  {MMGSDI_USIM_UWSIDL,                      0x4F44, MMGSDI_USIM_SRV_USER_CTRL_WSID},
  {MMGSDI_USIM_OWSIDL,                      0x4F45, MMGSDI_USIM_SRV_OPER_CTRL_WSID},
  {MMGSDI_USIM_WRI,                         0x4F46, MMGSDI_USIM_SRV_WLAN_REAUTH_ID},
  {MMGSDI_USIM_HWSIDL,                      0x4F47, MMGSDI_USIM_SRV_HWSIDL},
  {MMGSDI_USIM_WEHPLMNPI,                   0x4F48, MMGSDI_USIM_SRV_WEHPLMNPI},
  {MMGSDI_USIM_WHPI,                        0x4F49, MMGSDI_USIM_SRV_WHPI},
  {MMGSDI_USIM_WLRPLMN,                     0x4F4A, MMGSDI_USIM_SRV_WLRPLMN},
  {MMGSDI_USIM_HPLMNDAI,                    0x4F4B, MMGSDI_USIM_SRV_HPLMN_DA}
}; /* mmgsdi_file_usim_wlan_enum_table*/

/*EFs at DF HNB under USIM ADF */
const mmgsdi_file_enum_table_type mmgsdi_file_usim_hnb_enum_table[] =
{
  {MMGSDI_USIM_ACSGL,                       0x4F81, MMGSDI_USIM_SRV_ACSGL},
  {MMGSDI_USIM_CSGT,                        0x4F82, MMGSDI_USIM_SRV_ACSGL},
  {MMGSDI_USIM_HNBN,                        0x4F83, MMGSDI_USIM_SRV_ACSGL},
  {MMGSDI_USIM_OCSGL,                       0x4F84, MMGSDI_USIM_SRV_OCSGL},
  {MMGSDI_USIM_OCSGT,                       0x4F85, MMGSDI_USIM_SRV_OCSGL},
  {MMGSDI_USIM_OHNBN,                       0x4F86, MMGSDI_USIM_SRV_OCSGL},
}; /* mmgsdi_file_usim_hnb_enum_table*/

/* EFs of DCS1800 DF */
const mmgsdi_file_enum_table_type mmgsdi_file_dcs1800_enum_table[] =
{
  {MMGSDI_DCS1800_IMSI,                     0x6F07, MMGSDI_NONE},
  {MMGSDI_DCS1800_KC,                       0x6F20, MMGSDI_NONE},
  {MMGSDI_DCS1800_PLMN,                     0x6F30, MMGSDI_NONE},
  {MMGSDI_DCS1800_SST,                      0x6F38, MMGSDI_NONE},
  {MMGSDI_DCS1800_BCCH,                     0x6F74, MMGSDI_NONE},
  {MMGSDI_DCS1800_ACC,                      0x6F78, MMGSDI_NONE},
  {MMGSDI_DCS1800_FPLMN,                    0x6F7B, MMGSDI_NONE},
  {MMGSDI_DCS1800_LOCI,                     0x6F7E, MMGSDI_NONE},
  {MMGSDI_DCS1800_AD,                       0x6FAD, MMGSDI_NONE}
}; /* mmgsdi_file_dcs1800_enum_table */

/*EFs under ISIM ADF */
const mmgsdi_file_enum_table_type mmgsdi_file_isim_enum_table[] =
{
  {MMGSDI_ISIM_IMPI,                        0x6F02, MMGSDI_NONE},
  {MMGSDI_ISIM_DOMAIN,                      0x6F03, MMGSDI_NONE},
  {MMGSDI_ISIM_IMPU,                        0x6F04, MMGSDI_NONE},
  {MMGSDI_ISIM_ARR,                         0x6F06, MMGSDI_NONE},
  {MMGSDI_ISIM_KEYS,                        0x6F08, MMGSDI_NONE},
  {MMGSDI_ISIM_AD,                          0x6FAD, MMGSDI_NONE},
  {MMGSDI_ISIM_GBABP,                       0x6FD5, MMGSDI_NONE}
}; /* mmgsdi_file_isim_enum_table */

/*EFs for Orange */
const mmgsdi_file_enum_table_type mmgsdi_file_orange_sim_enum_table[] =
{
  {MMGSDI_SIM_7F40_PROP1_DFS,               0x6F9F, MMGSDI_NONE},
  {MMGSDI_SIM_7F40_PROP1_D2FS,              0x6F92, MMGSDI_NONE},
  {MMGSDI_SIM_7F40_PROP1_CSP2,              0x6F98, MMGSDI_NONE},
  {MMGSDI_SIM_7F40_PROP1_PARAMS,            0x6F9B, MMGSDI_NONE}
}; /* mmgsdi_file_orange_sim_enum_table */

const mmgsdi_file_enum_table_type mmgsdi_file_orange_usim_enum_table[] =
{
  {MMGSDI_USIM_7F40_PROP1_DFS,              0x6F9F, MMGSDI_NONE},
  {MMGSDI_USIM_7F40_PROP1_D2FS,             0x6F92, MMGSDI_NONE},
  {MMGSDI_USIM_7F40_PROP1_CSP2,             0x6F98, MMGSDI_NONE},
  {MMGSDI_USIM_7F40_PROP1_PARAMS,           0x6F9B, MMGSDI_NONE}
}; /* mmgsdi_file_orange_usim_enum_table */

/*EFs for Cingular */
const mmgsdi_file_enum_table_type mmgsdi_file_cingular_sim_enum_table[] =
{
  {MMGSDI_SIM_7F66_PROP1_SPT_TABLE,         0x6FD2, MMGSDI_NONE}
}; /* mmgsdi_file_cingular_sim_ef_enum_table */

const mmgsdi_file_enum_table_type mmgsdi_file_cingular_sim_network_enum_table[] =
{
  {MMGSDI_SIM_7F66_PROP1_ACT_HPLMN,         0x4F34, MMGSDI_NONE}
}; /* mmgsdi_file_cingular_sim_network_enum_table */

const mmgsdi_file_enum_table_type mmgsdi_file_cingular_usim_network_enum_table[] =
{
  {MMGSDI_USIM_7F66_PROP1_ACT_HPLMN,        0x4F34, MMGSDI_NONE},
  {MMGSDI_USIM_7F66_PROP1_RAT,              0x4F36, MMGSDI_NONE}
}; /* mmgsdi_file_cingular_usim_network_enum_table */

const mmgsdi_file_enum_table_type mmgsdi_file_cingular_sim_rpm_enum_table[] =
{
  {MMGSDI_SIM_7F66_PROP1_RPM_EF,            0x4F40, MMGSDI_NONE},
  {MMGSDI_SIM_7F66_PROP1_RPM_PARAM,         0x4F41, MMGSDI_NONE},
  {MMGSDI_SIM_7F66_PROP1_RPM_CNTR_LR,       0x4F42, MMGSDI_NONE},
  {MMGSDI_SIM_7F66_PROP1_RPM_CNTRS,         0x4F43, MMGSDI_NONE},
  {MMGSDI_SIM_7F66_PROP1_RPM_VI,            0x4F44, MMGSDI_NONE}
}; /* mmgsdi_file_cingular_sim_rpm_enum_table */

const mmgsdi_file_enum_table_type mmgsdi_file_cingular_usim_rpm_enum_table[] =
{
  {MMGSDI_USIM_7F66_PROP1_RPM_EF,           0x4F40, MMGSDI_NONE},
  {MMGSDI_USIM_7F66_PROP1_RPM_PARAM,        0x4F41, MMGSDI_NONE},
  {MMGSDI_USIM_7F66_PROP1_RPM_CNTR_LR,      0x4F42, MMGSDI_NONE},
  {MMGSDI_USIM_7F66_PROP1_RPM_CNTRS,        0x4F43, MMGSDI_NONE},
  {MMGSDI_USIM_7F66_PROP1_RPM_VI,           0x4F44, MMGSDI_NONE}
}; /* mmgsdi_file_cingular_usim_rpm_enum_table */

const mmgsdi_file_enum_table_type mmgsdi_file_cingular_usim_enum_table[] =
{
  {MMGSDI_USIM_7F66_PROP1_SPT_TABLE,        0x6FD2, MMGSDI_NONE}
}; /* mmgsdi_file_cingular_usim_ef_enum_table */

/*EFs for Home Zone */
const mmgsdi_file_enum_table_type mmgsdi_file_hzi_enum_table[] =
{
  {MMGSDI_SIM_7F43_PROP1_HZ,                 0x6F60, MMGSDI_NONE},
  {MMGSDI_SIM_7F43_PROP1_CACHE1,             0x6F61, MMGSDI_NONE},
  {MMGSDI_SIM_7F43_PROP1_CACHE2,             0x6F62, MMGSDI_NONE},
  {MMGSDI_SIM_7F43_PROP1_CACHE3,             0x6F63, MMGSDI_NONE},
  {MMGSDI_SIM_7F43_PROP1_CACHE4,             0x6F64, MMGSDI_NONE},
  {MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_1,0x6F81, MMGSDI_NONE},
  {MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_2,0x6F82, MMGSDI_NONE},
  {MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_3,0x6F83, MMGSDI_NONE},
  {MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_4,0x6F84, MMGSDI_NONE},
  {MMGSDI_SIM_7F43_PROP1_TAGS,               0x6F80, MMGSDI_NONE},
  {MMGSDI_SIM_7F43_PROP1_SETTINGS,           0x6F87, MMGSDI_NONE},
}; /* mmgsdi_file_hzi_enum_table */

/*EFs under CSIM ADF */
const mmgsdi_file_enum_table_type mmgsdi_file_csim_enum_table[] =
{
  {MMGSDI_CSIM_ARR,                         0x6F06, MMGSDI_NONE},
  {MMGSDI_CSIM_CALL_COUNT,                  0x6F21, MMGSDI_NONE},
  {MMGSDI_CSIM_IMSI_M,                      0x6F22, MMGSDI_NONE},
  {MMGSDI_CSIM_IMSI_T,                      0x6F23, MMGSDI_NONE},
  {MMGSDI_CSIM_TMSI,                        0x6F24, MMGSDI_NONE},
  {MMGSDI_CSIM_ANALOG_HOME_SID,             0x6F25, MMGSDI_NONE},
  {MMGSDI_CSIM_ANALOG_OP_PARAMS,            0x6F26, MMGSDI_NONE},
  {MMGSDI_CSIM_ANALOG_LOCN_AND_REGN_IND,    0x6F27, MMGSDI_NONE},
  {MMGSDI_CSIM_CDMA_HOME_SID_NID,           0x6F28, MMGSDI_NONE},
  {MMGSDI_CSIM_CDMA_ZONE_BASED_REGN_IND,    0x6F29, MMGSDI_NONE},

  /* 10 */
  {MMGSDI_CSIM_CDMA_SYS_REGN_IND,           0x6F2A, MMGSDI_NONE},
  {MMGSDI_CSIM_CDMA_DIST_BASED_REGN_IND,    0x6F2B, MMGSDI_NONE},
  {MMGSDI_CSIM_ACCOLC,                      0x6F2C, MMGSDI_NONE},
  {MMGSDI_CSIM_CALL_TERM_MODE_PREF,         0x6F2D, MMGSDI_NONE},
  {MMGSDI_CSIM_SSCI,                        0x6F2E, MMGSDI_NONE},
  {MMGSDI_CSIM_ANALOG_CHAN_PREF,            0x6F2F, MMGSDI_NONE},
  {MMGSDI_CSIM_PRL,                         0x6F30, MMGSDI_NONE},
  {MMGSDI_CSIM_RUIM_ID,                     0x6F31, MMGSDI_NONE},
  {MMGSDI_CSIM_CST,                         0x6F32, MMGSDI_NONE},
  {MMGSDI_CSIM_SPC,                         0x6F33, MMGSDI_NONE},

  /* 20 */
  {MMGSDI_CSIM_OTAPA_SPC_ENABLE,            0x6F34, MMGSDI_NONE},
  {MMGSDI_CSIM_NAM_LOCK,                    0x6F35, MMGSDI_NONE},
  {MMGSDI_CSIM_OTASP_OTAPA_FEATURES,        0x6F36, MMGSDI_NONE},
  {MMGSDI_CSIM_SERVICE_PREF,                0x6F37, MMGSDI_NONE},
  {MMGSDI_CSIM_ESN_ME,                      0x6F38, MMGSDI_NONE},
  {MMGSDI_CSIM_UNASSIGNED_1,                0x6F39, MMGSDI_NONE},
  {MMGSDI_CSIM_LANG_IND,                    0x6F3A, MMGSDI_NONE},
  {MMGSDI_CSIM_FDN,                         0x6F3B, MMGSDI_NONE},
  {MMGSDI_CSIM_SMS,                         0x6F3C, MMGSDI_CSIM_SRV_SMS},
  {MMGSDI_CSIM_SMS_PARAMS,                  0x6F3D, MMGSDI_CSIM_SRV_SMSP},

  /* 30 */
  {MMGSDI_CSIM_SMS_STATUS,                  0x6F3E, MMGSDI_CSIM_SRV_SMS},
  {MMGSDI_CSIM_SSFC,                        0x6F3F, MMGSDI_NONE},
  {MMGSDI_CSIM_SPN,                         0x6F41, MMGSDI_CSIM_SRV_SPN},
  {MMGSDI_CSIM_USGIND,                      0x6F42, MMGSDI_NONE},
  {MMGSDI_CSIM_AD,                          0x6F43, MMGSDI_NONE},
  {MMGSDI_CSIM_MSISDN,                      0x6F44, MMGSDI_NONE},
  {MMGSDI_CSIM_MAXIMUM_PRL,                 0x6F45, MMGSDI_NONE},
  {MMGSDI_CSIM_SPC_STATUS,                  0x6F46, MMGSDI_NONE},
  {MMGSDI_CSIM_ECC,                         0x6F47, MMGSDI_NONE},
  {MMGSDI_CSIM_3GPD_ME3GPDOPC,              0x6F48, MMGSDI_SRV_MAX}, /* MMGSDI_CSIM_SRV_3GPD_SIP || MMGSDI_CSIM_SRV_3GPD_MIP */

  /* 40 */
  {MMGSDI_CSIM_3GPD_3GPDOPM,                0x6F49, MMGSDI_SRV_MAX}, /* MMGSDI_CSIM_SRV_3GPD_SIP || MMGSDI_CSIM_SRV_3GPD_MIP */
  {MMGSDI_CSIM_3GPD_SIPCAP,                 0x6F4A, MMGSDI_CSIM_SRV_3GPD_SIP},
  {MMGSDI_CSIM_3GPD_MIPCAP,                 0x6F4B, MMGSDI_CSIM_SRV_3GPD_MIP},
  {MMGSDI_CSIM_3GPD_SIPUPP,                 0x6F4C, MMGSDI_CSIM_SRV_3GPD_SIP},
  {MMGSDI_CSIM_3GPD_MIPUPP,                 0x6F4D, MMGSDI_CSIM_SRV_3GPD_MIP},
  {MMGSDI_CSIM_3GPD_SIPSP,                  0x6F4E, MMGSDI_CSIM_SRV_3GPD_SIP},
  {MMGSDI_CSIM_3GPD_MIPSP,                  0x6F4F, MMGSDI_CSIM_SRV_3GPD_MIP},
  {MMGSDI_CSIM_3GPD_SIPPAPSS,               0x6F50, MMGSDI_CSIM_SRV_3GPD_SIP},
  {MMGSDI_CSIM_UNASSIGNED_2,                0x6F51, MMGSDI_NONE},
  {MMGSDI_CSIM_UNASSIGNED_3,                0x6F52, MMGSDI_NONE},

  /* 50 */
  {MMGSDI_CSIM_PUZL,                        0x6F53, MMGSDI_NONE},
  {MMGSDI_CSIM_MAXPUZL,                     0x6F54, MMGSDI_NONE},
  {MMGSDI_CSIM_MECRP,                       0x6F55, MMGSDI_NONE},
  {MMGSDI_CSIM_HRPDCAP,                     0x6F56, MMGSDI_CSIM_SRV_HRPD},
  {MMGSDI_CSIM_HRPDUPP,                     0x6F57, MMGSDI_CSIM_SRV_HRPD},
  {MMGSDI_CSIM_CSSPR,                       0x6F58, MMGSDI_NONE},
  {MMGSDI_CSIM_ATC,                         0x6F59, MMGSDI_CSIM_SRV_HRPD},
  {MMGSDI_CSIM_EPRL,                        0x6F5A, MMGSDI_NONE},
  {MMGSDI_CSIM_BCSMS_CONFIG,                0x6F5B, MMGSDI_CSIM_SRV_BSVR_CAT_PROG_BCSMS},
  {MMGSDI_CSIM_BCSMS_PREF,                  0x6F5C, MMGSDI_CSIM_SRV_BSVR_CAT_PROG_BCSMS},

  /* 60 */
  {MMGSDI_CSIM_BCSMS_TABLE,                 0x6F5D, MMGSDI_CSIM_SRV_BSVR_CAT_PROG_BCSMS},
  {MMGSDI_CSIM_BCSMS_PARAMS,                0x6F5E, MMGSDI_CSIM_SRV_BSVR_CAT_PROG_BCSMS},
  {MMGSDI_CSIM_BAKPARA,                     0x6F63, MMGSDI_CSIM_SRV_BCMCS},
  {MMGSDI_CSIM_UPBAKPARA,                   0x6F64, MMGSDI_CSIM_SRV_BCMCS},
  {MMGSDI_CSIM_MMSN,                        0x6F65, MMGSDI_CSIM_SRV_MMS},
  {MMGSDI_CSIM_MMS_EXT8,                    0x6F66, MMGSDI_CSIM_SRV_EXT8},
  {MMGSDI_CSIM_MMS_ICP,                     0x6F67, MMGSDI_CSIM_SRV_MMS},
  {MMGSDI_CSIM_MMS_UP,                      0x6F68, MMGSDI_CSIM_SRV_MMS},
  {MMGSDI_CSIM_MMS_UCP,                     0x6F69, MMGSDI_SRV_MAX}, /* MMGSDI_CSIM_SRV_MMS && MMGSDI_CSIM_SRV_MMS_USR_P */
  {MMGSDI_CSIM_AUTH_CAP,                    0x6F6A, MMGSDI_CSIM_SRV_APP_AUTH},

  /* 70 */
  {MMGSDI_CSIM_3GCIK,                       0x6F6B, MMGSDI_CSIM_SRV_AKA},
  {MMGSDI_CSIM_DCK,                         0x6F6C, MMGSDI_CSIM_SRV_DCK},
  {MMGSDI_CSIM_GID1,                        0x6F6D, MMGSDI_CSIM_SRV_GID1},
  {MMGSDI_CSIM_GID2,                        0x6F6E, MMGSDI_CSIM_SRV_GID2},
  {MMGSDI_CSIM_CNL,                         0x6F6F, MMGSDI_CSIM_SRV_CNL},
  {MMGSDI_CSIM_HOME_TAG,                    0x6F70, MMGSDI_NONE},
  {MMGSDI_CSIM_GROUP_TAG,                   0x6F71, MMGSDI_NONE},
  {MMGSDI_CSIM_SPECIFIC_TAG,                0x6F72, MMGSDI_NONE},
  {MMGSDI_CSIM_CALL_PROMPT,                 0x6F73, MMGSDI_NONE},
  {MMGSDI_CSIM_SF_EUIM_ID,                  0x6F74, MMGSDI_CSIM_SRV_SF_EUIMID},

  /* 80 */
  {MMGSDI_CSIM_EST,                         0x6F75, MMGSDI_NONE},
  {MMGSDI_CSIM_HIDDEN_KEY,                  0x6F76, MMGSDI_NONE},
  {MMGSDI_CSIM_LCS_VERSION,                 0x6F77, MMGSDI_CSIM_SRV_LCS_IP},
  {MMGSDI_CSIM_LCS_CP,                      0x6F78, MMGSDI_CSIM_SRV_LCS_IP},
  {MMGSDI_CSIM_SDN,                         0x6F79, MMGSDI_CSIM_SRV_SDN},
  {MMGSDI_CSIM_EXT2,                        0x6F7A, MMGSDI_CSIM_SRV_EXT2},
  {MMGSDI_CSIM_EXT3,                        0x6F7B, MMGSDI_CSIM_SRV_EXT3},
  {MMGSDI_CSIM_ICI,                         0x6F7C, MMGSDI_CSIM_SRV_ICI},
  {MMGSDI_CSIM_OCI,                         0x6F7D, MMGSDI_CSIM_SRV_OCI},
  {MMGSDI_CSIM_EXT5,                        0x6F7E, MMGSDI_CSIM_SRV_EXT5},

  /* 90 */
  {MMGSDI_CSIM_CCP2,                        0x6F7F, MMGSDI_CSIM_SRV_CCP},
  {MMGSDI_CSIM_APP_LABELS,                  0x6F80, MMGSDI_NONE},
  {MMGSDI_CSIM_DEV_MODEL,                   0x6F81, MMGSDI_NONE},
  {MMGSDI_CSIM_ROOT_CERT,                   0x6F82, MMGSDI_CSIM_SRV_ROOT_CERT},
  {MMGSDI_CSIM_SMS_CAP,                     0x6F83, MMGSDI_NONE},
  {MMGSDI_CSIM_3GPD_MIPFLAGS,               0x6F84, MMGSDI_NONE},
  {MMGSDI_CSIM_3GPD_SIPUPPEXT,              0x6F85, MMGSDI_NONE},
  {MMGSDI_CSIM_3GPD_MIPUPPEXT,              0x6F86, MMGSDI_NONE},
  {MMGSDI_CSIM_3GPD_IPV6CAP,                0x6F87, MMGSDI_NONE},
  {MMGSDI_CSIM_3GPD_TCPCONFIG,              0x6F88, MMGSDI_CSIM_SRV_3GPD_EXT},
  /* Need to additionally check (MMGSDI_CSIM_SRV_3GPD_SIP || MMGSDI_CSIM_SRV_3GPD_MIP) in mmgsdi_util_is_file_ok_in_svc_table */

  /* 100 */
  {MMGSDI_CSIM_3GPD_DGC,                    0x6F89, MMGSDI_CSIM_SRV_3GPD_EXT},
  /* Need to additionally check (MMGSDI_CSIM_SRV_3GPD_SIP || MMGSDI_CSIM_SRV_3GPD_MIP) in mmgsdi_util_is_file_ok_in_svc_table */
  {MMGSDI_CSIM_BROWSER_CP,                  0x6F8A, MMGSDI_CSIM_SRV_BROWSER},
  {MMGSDI_CSIM_BROWSER_BM,                  0x6F8B, MMGSDI_CSIM_SRV_BROWSER},
  {MMGSDI_CSIM_MMS_CONFIG,                  0x6F8C, MMGSDI_NONE},
  {MMGSDI_CSIM_JDL,                         0x6F8D, MMGSDI_CSIM_SRV_JAVA},
  {MMGSDI_CSIM_LBS_XCONFIG,                 0x6F8E, MMGSDI_CSIM_SRV_LBS},
  {MMGSDI_CSIM_LBS_XSURL,                   0x6F8F, MMGSDI_CSIM_SRV_LBS},
  {MMGSDI_CSIM_LBS_V2CONFIG,                0x6F90, MMGSDI_CSIM_SRV_LBS},
  {MMGSDI_CSIM_LBS_V2PDEADDR,               0x6F91, MMGSDI_CSIM_SRV_LBS},
  {MMGSDI_CSIM_LBS_V2MPCADDR,               0x6F92, MMGSDI_CSIM_SRV_LBS},

  /* 110 */
  {MMGSDI_CSIM_BREW_DLOAD,                  0x6F93, MMGSDI_CSIM_SRV_BREW},
  {MMGSDI_CSIM_BREW_TSID,                   0x6F94, MMGSDI_CSIM_SRV_BREW},
  {MMGSDI_CSIM_BREW_SID,                    0x6F95, MMGSDI_CSIM_SRV_BREW},
  {MMGSDI_CSIM_BREW_AEP,                    0x6F96, MMGSDI_CSIM_SRV_BREW},
  {MMGSDI_CSIM_3GPD_UPPEXT,                 0x6F85, MMGSDI_CSIM_SRV_3GPD_EXT}
  /* Need to additionally check (MMGSDI_CSIM_SRV_3GPD_SIP || MMGSDI_CSIM_SRV_3GPD_MIP) in mmgsdi_util_is_file_ok_in_svc_table */
}; /* mmgsdi_file_csim_enum_table */

/*EFs at DF Phonebook under CSIM ADF */
const mmgsdi_file_enum_table_type mmgsdi_file_csim_phonebook_enum_table[] =
{
  {MMGSDI_CSIM_PBR,                         0x4F30, MMGSDI_NONE},
  {MMGSDI_CSIM_PSC,                         0x4F22, MMGSDI_NONE},
  {MMGSDI_CSIM_CHANGE_COUNTER,              0x4F23, MMGSDI_NONE},
  {MMGSDI_CSIM_PUID,                        0x4F24, MMGSDI_NONE}
}; /* mmgsdi_file_csim_phonebook_enum_table*/

/*EFs at DF JCDMA under CSIM ADF */
const mmgsdi_file_enum_table_type mmgsdi_file_csim_jcdma_enum_table[] =
{
  {MMGSDI_CSIM_EXT_ME_SETTING_DATA,         0x4FE2, MMGSDI_NONE},
  {MMGSDI_CSIM_USER_AGENT_STRING,           0x4FE3, MMGSDI_NONE},
  {MMGSDI_CSIM_IMSI_STATUS,                 0x4FE7, MMGSDI_NONE},
  {MMGSDI_CSIM_ME_DOWNLOADABLE_DATA,        0x4FE8, MMGSDI_NONE},
  {MMGSDI_CSIM_ME_SETTING_DATA,             0x4FE9, MMGSDI_NONE},
  {MMGSDI_CSIM_ME_USER_DATA,                0x4FEA, MMGSDI_NONE},
  {MMGSDI_CSIM_UIM_SVC_TABLE,               0x4FED, MMGSDI_NONE},
  {MMGSDI_CSIM_UIM_FEATURES,                0x4FEE, MMGSDI_NONE},
  {MMGSDI_CSIM_UIM_VERSION,                 0x4FEF, MMGSDI_NONE}
}; /* mmgsdi_file_csim_jcdma_enum_table*/

/* Access conditions are stored in this table for most of the files in spec having
   access conditions different than:
   READ ALLOWED
   WRITE ALLOWED
   INCREASE ALLOWED
   DEACTIVATE NOTALLOWED
   ACTIVATE NOTALLOWED
   Note: NOTALLOWED here only refers to ADM and Never in spec language. ALLOWED
   refers to PIN1/PIN2/ALWAYS in spec language */
static const mmgsdi_file_default_file_sec_attr_table_type mmgsdi_file_default_file_sec_attr_table[] =
{
  /* FILE                            SECURITY ATTRIBUTES MASK  */
  {MMGSDI_ICCID,                     MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_IMSI,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK | MMGSDI_ACTIVATE_ALLOWED_MASK},
  {MMGSDI_GSM_HPLMN,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_SST,                   MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_SPN,                   MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_ACC,                   MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_LOCI,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_WRITE_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK |
                                     MMGSDI_ACTIVATE_ALLOWED_MASK},
  {MMGSDI_GSM_EMLPP,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_PHASE,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_AD,                    MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_OPLMNWACT,             MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_HPLMNACT,              MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_CBMID,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_ECC,                   MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_GID1,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_GID2,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_SAI,                   MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_VGCS,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_VGCSS,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_VBS,                   MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_VBSS,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_EMLPP,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_CNL,                   MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_NIA,                   MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_SUME,                  0x00},
  {MMGSDI_GSM_INVSCAN,               MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_MEXE_ST,               MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_ORPK,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_ARPK,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_GSM_TPRPK,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_TELECOM_ADN,               MMGSDI_READ_ALLOWED_MASK | MMGSDI_WRITE_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK |
                                     MMGSDI_ACTIVATE_ALLOWED_MASK | MMGSDI_DEACTIVATE_ALLOWED_MASK},
  {MMGSDI_TELECOM_LND,               MMGSDI_READ_ALLOWED_MASK | MMGSDI_WRITE_ALLOWED_MASK},
  {MMGSDI_TELECOM_SDN,               MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_TELECOM_EXT3,              MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_TELECOM_BDN,               MMGSDI_READ_ALLOWED_MASK | MMGSDI_WRITE_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK |
                                     MMGSDI_ACTIVATE_ALLOWED_MASK | MMGSDI_DEACTIVATE_ALLOWED_MASK},
  {MMGSDI_TELECOM_CMI,               MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_IMAGE,                     MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_IMAGE_FILE,                MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},

  {MMGSDI_USIM_IMSI,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_HPLMN,                MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_UST,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_SPN,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_ACC,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_EMLPP,                MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_THRESHOLD,            MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_OPLMNWACT,            MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_HPLMNWACT,            MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_AD,                   MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_CBMID,                MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_SPDI,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_ECC,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_GID1,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_GID2,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_WHPI,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_HPLMNDAI,             MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_HPLMN,                MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_SDN,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_EXT3,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_CMI,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_CNL,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_PNN,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_OPL,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_MMSICP,               MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_VGCSCA,               MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_VBSCA,                MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_MSK,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_MUK,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_GBANL,                MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_SPNI,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_PNNI,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_NCP_IP,               MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_NASCONFIG,            MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_PBR,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_INVSCAN,              MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_MEXE_ST,              MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_ORPK,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_ARPK,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_TPRPK,                MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_PSEUDO,               MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_0PLMNWLAN,            MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_OWSIDL,               MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_HWSIDL,               MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_WEHPLMNPI,            MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_WHPI,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_HPLMNDAI,             MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_OCSGL,                MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_OCSGT,                MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_USIM_OHNBN,                MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_TELECOM_ICE_DN,            MMGSDI_READ_ALLOWED_MASK | MMGSDI_WRITE_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK |
                                     MMGSDI_ACTIVATE_ALLOWED_MASK | MMGSDI_DEACTIVATE_ALLOWED_MASK},
  {MMGSDI_TELECOM_ICE_FF,            MMGSDI_READ_ALLOWED_MASK | MMGSDI_WRITE_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK |
                                     MMGSDI_ACTIVATE_ALLOWED_MASK | MMGSDI_DEACTIVATE_ALLOWED_MASK},
  {MMGSDI_TELECOM_ICE_GRAPHICS,      MMGSDI_READ_ALLOWED_MASK | MMGSDI_WRITE_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK |
                                     MMGSDI_ACTIVATE_ALLOWED_MASK | MMGSDI_DEACTIVATE_ALLOWED_MASK},

  {MMGSDI_CSIM_IMSI_M,               MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK | MMGSDI_ACTIVATE_ALLOWED_MASK},
  {MMGSDI_CSIM_IMSI_T,               MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK | MMGSDI_ACTIVATE_ALLOWED_MASK},
  {MMGSDI_CSIM_CST,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_AD,                   MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_RUIM_ID,              MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_PRL,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_TMSI,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_WRITE_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK |
                                     MMGSDI_ACTIVATE_ALLOWED_MASK},
  {MMGSDI_CSIM_ACCOLC,               MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_EPRL,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_MAXIMUM_PRL,          MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_SPN,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_ECC,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_3GPD_SIPUPP,          MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_3GPD_MIPUPP,          MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_HRPDUPP,              MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_SF_EUIM_ID,           MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_OTASP_OTAPA_FEATURES, MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_ESN_ME,               MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_USGIND,               MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_3GPD_SIPCAP,          MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_3GPD_MIPCAP,          MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_PUZL,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_MAXPUZL,              MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_HRPDCAP,              MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_HRPDUPP,              MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_CSSPR,                MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_ATC,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_BCSMS_CONFIG,         MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_BCSMS_TABLE,          MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_BAKPARA,              MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_UPBAKPARA,            MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_MMS_ICP,              MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_AUTH_CAP,             MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_3GCIK,                MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_GID1,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_GID2,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_CNL,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_HOME_TAG,             MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_GROUP_TAG,            MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_SPECIFIC_TAG,         MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_CALL_PROMPT,          MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_LCS_VERSION,          MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_LCS_CP,               MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_SDN,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CSIM_EXT3,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},

  {MMGSDI_CDMA_IMSI_M,               MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK | MMGSDI_ACTIVATE_ALLOWED_MASK},
  {MMGSDI_CDMA_IMSI_T,               MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK | MMGSDI_ACTIVATE_ALLOWED_MASK},
  {MMGSDI_CDMA_TMSI,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_WRITE_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK |
                                     MMGSDI_ACTIVATE_ALLOWED_MASK},
  {MMGSDI_CDMA_SVC_TABLE,            MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_HOME_SVC_PVDR_NAME,   MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_ECC,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_3GPD_SIPUPP,          MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_3GPD_MIPUPP,          MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_HRPDUPP,              MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_SF_EUIM_ID,           MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_ACCOLC,               MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_PRL,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_RUIM_ID,              MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_OTASP_OTAPA_FEATURES, MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_ESN_ME,               MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_RUIM_PHASE,           MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_UIM_ID_USAGE_IND,     MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_ADM_DATA,             MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_MAXIMUM_PRL,          MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_ECC,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_3GPD_SIPCAP,          MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_3GPD_MIPCAP,          MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_PUZL,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_MAXPUZL,              MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_HRPDCAP,              MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_HRPDUPP,              MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_CSSPR,                MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_ATC,                  MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_EPRL,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_BCSMS_CONFIG,         MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_BCSMS_TABLE,          MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_GID1,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_GID2,                 MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK},
  {MMGSDI_CDMA_MMS_ICP,              MMGSDI_READ_ALLOWED_MASK | MMGSDI_INCREASE_ALLOWED_MASK}
};

/* SFI values for files under MF */
static const mmgsdi_file_sfi_table_type mmgsdi_mf_file_sfi_table[] =
{
  { MMGSDI_ICCID,          0x02, 10,                        MMGSDI_TRANSPARENT_FILE},    /* ICC Identification */
  { MMGSDI_ELP,            0x05, MMGSDI_INVALID_FILE_SIZE,  MMGSDI_TRANSPARENT_FILE},    /* Preferred Languages */
  { MMGSDI_ARR,            0x06, MMGSDI_INVALID_REC_LEN,    MMGSDI_LINEAR_FIXED_FILE},    /* Access Rules Reference */
  { MMGSDI_DIR,            0x1E, MMGSDI_INVALID_REC_LEN,    MMGSDI_LINEAR_FIXED_FILE}     /* Application Directory */
};

/* SFI values for files under USIM ADF */
static const mmgsdi_file_sfi_table_type mmgsdi_usim_file_sfi_table[] =
{
  { MMGSDI_USIM_ECC,       0x01, MMGSDI_INVALID_REC_LEN,    MMGSDI_LINEAR_FIXED_FILE},   /* Emergency call codes */
  { MMGSDI_USIM_LI,        0x02, MMGSDI_INVALID_FILE_SIZE,  MMGSDI_TRANSPARENT_FILE},    /* Language indication */
  { MMGSDI_USIM_AD,        0x03, 4,                         MMGSDI_TRANSPARENT_FILE},    /* Administrative data */
  { MMGSDI_USIM_UST,       0x04, MMGSDI_INVALID_FILE_SIZE,  MMGSDI_TRANSPARENT_FILE},    /* USIM service table */
  { MMGSDI_USIM_EST,       0x05, MMGSDI_INVALID_REC_LEN,    MMGSDI_LINEAR_FIXED_FILE},   /* Enabled services table */
  { MMGSDI_USIM_ACC,       0x06, 2,                         MMGSDI_TRANSPARENT_FILE},    /* Access control class */
  { MMGSDI_USIM_IMSI,      0x07, 9,                         MMGSDI_TRANSPARENT_FILE},    /* IMSI */
  { MMGSDI_USIM_KEYS,      0x08, 33,                        MMGSDI_TRANSPARENT_FILE},    /* Ciphering and integrity keys */
  { MMGSDI_USIM_KEYSPS,    0x09, 33,                        MMGSDI_TRANSPARENT_FILE},    /* Ciphering and integrity keys for packet switched domain */
  { MMGSDI_USIM_PLMNWACT,  0x0A, MMGSDI_INVALID_FILE_SIZE,  MMGSDI_TRANSPARENT_FILE},    /* User PLMN selector */
  { MMGSDI_USIM_LOCI,      0x0B, 11,                        MMGSDI_TRANSPARENT_FILE},    /* Location information */
  { MMGSDI_USIM_PSLOCI,    0x0C, 14,                        MMGSDI_TRANSPARENT_FILE},    /* Packet switched location information */
  { MMGSDI_USIM_FPLMN,     0x0D, MMGSDI_INVALID_FILE_SIZE,  MMGSDI_TRANSPARENT_FILE},    /* Forbidden PLMNs */
  { MMGSDI_USIM_CBMID,     0x0E, MMGSDI_INVALID_FILE_SIZE,  MMGSDI_TRANSPARENT_FILE},    /* CBMID */
  { MMGSDI_USIM_START_HFN, 0x0F, 6,                         MMGSDI_TRANSPARENT_FILE},    /* Hyperframe number */
  { MMGSDI_USIM_THRESHOLD, 0x10, 3,                         MMGSDI_TRANSPARENT_FILE},    /* Maximum value of hyperframe number */
  { MMGSDI_USIM_OPLMNWACT, 0x11, MMGSDI_INVALID_FILE_SIZE,  MMGSDI_TRANSPARENT_FILE},    /* Operator PLMN selector */
  { MMGSDI_USIM_HPLMN,     0x12, 1,                         MMGSDI_TRANSPARENT_FILE},    /* Higher Priority PLMN search period */
  { MMGSDI_USIM_HPLMNWACT, 0x13, MMGSDI_INVALID_FILE_SIZE,  MMGSDI_TRANSPARENT_FILE},    /* Preferred HPLMN access technology */
  { MMGSDI_USIM_ICI,       0x14, MMGSDI_INVALID_REC_LEN,    MMGSDI_CYCLIC_FILE},    /* Incoming call information */
  { MMGSDI_USIM_OCI,       0x15, MMGSDI_INVALID_REC_LEN,    MMGSDI_CYCLIC_FILE},    /* Outgoing call information */
  { MMGSDI_USIM_CCP2,      0x16, MMGSDI_INVALID_REC_LEN,    MMGSDI_LINEAR_FIXED_FILE},    /* Capability configuration parameters 2 */
  { MMGSDI_USIM_ARR,       0x17, MMGSDI_INVALID_REC_LEN,    MMGSDI_LINEAR_FIXED_FILE},    /* Access Rule Reference */
  { MMGSDI_USIM_PNN,       0x19, MMGSDI_INVALID_REC_LEN,    MMGSDI_LINEAR_FIXED_FILE},    /* PLMN Network Name */
  { MMGSDI_USIM_OPL,       0x1A, MMGSDI_INVALID_REC_LEN,    MMGSDI_LINEAR_FIXED_FILE},    /* Operator Network List */
  { MMGSDI_USIM_SPDI,      0x1B, MMGSDI_INVALID_FILE_SIZE,  MMGSDI_TRANSPARENT_FILE},    /* Service Provider Display Information */
  { MMGSDI_USIM_ACM,       0x1C, 3,                         MMGSDI_CYCLIC_FILE},    /* Accumulated Call Meter (see note) */
  { MMGSDI_USIM_EHPLMN,    0x1D, MMGSDI_INVALID_FILE_SIZE,  MMGSDI_TRANSPARENT_FILE},    /* Equivalent HPLMN */
  { MMGSDI_USIM_EPSLOCI,   0x1E, 18,                        MMGSDI_TRANSPARENT_FILE},    /* EPS location information */
  { MMGSDI_USIM_EPSNSC,    0x18, MMGSDI_INVALID_REC_LEN,    MMGSDI_LINEAR_FIXED_FILE}    /* EPS NAS Security Context */
};

/* SFI values for files under CSIM ADF */
static const mmgsdi_file_sfi_table_type mmgsdi_csim_file_sfi_table[] =
{
  { MMGSDI_CSIM_AD,        0x01, 3,                         MMGSDI_TRANSPARENT_FILE},    /* Administrative data */
  { MMGSDI_CSIM_CST,       0x02, MMGSDI_INVALID_FILE_SIZE,  MMGSDI_TRANSPARENT_FILE},    /* CSIM Service Table */
  { MMGSDI_CSIM_ACCOLC,    0x03, 1,                         MMGSDI_TRANSPARENT_FILE},    /* Access Overload Class */
  { MMGSDI_CSIM_IMSI_M,    0x04, 10,                        MMGSDI_TRANSPARENT_FILE},    /* IMSI_M */
  { MMGSDI_CSIM_IMSI_T,    0x05, 10,                        MMGSDI_TRANSPARENT_FILE},    /* IMSI_T */
  { MMGSDI_CSIM_TMSI,      0x06, 16,                        MMGSDI_TRANSPARENT_FILE},    /* TMSI */
  { MMGSDI_CSIM_PRL,       0x07, MMGSDI_INVALID_FILE_SIZE,  MMGSDI_TRANSPARENT_FILE},    /* PRL */
  { MMGSDI_CSIM_SPN,       0x08, 35,                        MMGSDI_TRANSPARENT_FILE},    /* Home Service Provider Display Information */
  { MMGSDI_CSIM_ECC,       0x09, MMGSDI_INVALID_FILE_SIZE,  MMGSDI_TRANSPARENT_FILE},    /* Emergency Call Codes */
  { MMGSDI_CSIM_LANG_IND,  0x0A, MMGSDI_INVALID_FILE_SIZE,  MMGSDI_TRANSPARENT_FILE},    /* Language Indication */
  { MMGSDI_CSIM_3GCIK,     0x0B, 32,                        MMGSDI_TRANSPARENT_FILE},    /* 3G Cipher and Integrity Key */
  { MMGSDI_CSIM_CDMA_HOME_SID_NID,
                           0x0C, 5,                         MMGSDI_LINEAR_FIXED_FILE},    /* CDMA Home SID and NID */
  { MMGSDI_CSIM_CDMA_SYS_REGN_IND,
                           0x0D, 7,                         MMGSDI_TRANSPARENT_FILE},    /* CDMA System-Network Registration Indicators */
  { MMGSDI_CSIM_EPRL,      0x0E, MMGSDI_INVALID_FILE_SIZE,  MMGSDI_TRANSPARENT_FILE},    /* Extended PRL */
  { MMGSDI_CSIM_EST,       0x0F, MMGSDI_INVALID_FILE_SIZE,  MMGSDI_TRANSPARENT_FILE},    /* Enabled Services Table */
  { MMGSDI_CSIM_ICI,       0x10, MMGSDI_INVALID_REC_LEN,    MMGSDI_CYCLIC_FILE},    /* Incoming Call Information */
  { MMGSDI_CSIM_OCI,       0x11, MMGSDI_INVALID_REC_LEN,    MMGSDI_CYCLIC_FILE},    /* Outgoing Call Information */
  { MMGSDI_CSIM_CCP2,      0x12, MMGSDI_INVALID_REC_LEN,    MMGSDI_LINEAR_FIXED_FILE}     /* Capability Control Parameters2 */
};

/* SFI values for files under USIM GSM DF */
static const mmgsdi_file_sfi_table_type mmgsdi_usim_gsm_access_file_sfi_table[] =
{
  /* GSM ACCESS */
  { MMGSDI_USIM_KC,        0x01, 9,                         MMGSDI_TRANSPARENT_FILE},    /* GSM Ciphering Key Kc */
  { MMGSDI_USIM_KCGPRS,    0x02, 9,                         MMGSDI_TRANSPARENT_FILE}     /* GPRS Ciphering Key KcGPRS */
};

/* SFI values for files under USIM WLAN DF */
static const mmgsdi_file_sfi_table_type mmgsdi_usim_wlan_file_sfi_table[] =
{
  /* WLAN */
  { MMGSDI_USIM_ORPK,      0x01, MMGSDI_INVALID_REC_LEN,    MMGSDI_LINEAR_FIXED_FILE},    /* Pseudonym */
  { MMGSDI_USIM_ARPK,      0x02, MMGSDI_INVALID_REC_LEN,    MMGSDI_LINEAR_FIXED_FILE},    /* User controlled PLMN for WLAN */
  { MMGSDI_USIM_TPRPK,     0x03, MMGSDI_INVALID_REC_LEN,    MMGSDI_LINEAR_FIXED_FILE},    /* Operator controlled PLMN for WLAN */
  { MMGSDI_USIM_UWSIDL,    0x04, MMGSDI_INVALID_REC_LEN,    MMGSDI_LINEAR_FIXED_FILE},    /* User controlled WSID list */
  { MMGSDI_USIM_OWSIDL,    0x05, MMGSDI_INVALID_REC_LEN,    MMGSDI_LINEAR_FIXED_FILE},    /* Operator controlled WSID list */
  { MMGSDI_USIM_WRI,       0x06, MMGSDI_INVALID_FILE_SIZE,  MMGSDI_TRANSPARENT_FILE},    /* WLAN Reauthentication Identity */
  { MMGSDI_USIM_HWSIDL,    0x07, MMGSDI_INVALID_REC_LEN,    MMGSDI_LINEAR_FIXED_FILE},    /* Home I-WLAN Specific Identifier List */
  { MMGSDI_USIM_WEHPLMNPI, 0x08, 1,                         MMGSDI_TRANSPARENT_FILE},    /* I-WLAN Equivalent HPLMN Presentation Indication */
  { MMGSDI_USIM_WHPI,      0x09, 1,                         MMGSDI_TRANSPARENT_FILE},    /* I-WLAN HPLMN Priority Indication */
  { MMGSDI_USIM_WLRPLMN,   0x0A, 3,                         MMGSDI_TRANSPARENT_FILE},    /* I-WLAN Last Registered PLMN */
  { MMGSDI_USIM_HPLMNDAI,  0x0B, 1,                         MMGSDI_TRANSPARENT_FILE}     /* HPLMN Direct Access Indicator */
}; /* mmgsdi_usim_wlan_file_sfi_table */

/* SFI values for files under USIM HNB DF */
static const mmgsdi_file_sfi_table_type mmgsdi_usim_hnb_file_sfi_table[] =
{
  /* HNB */
  { MMGSDI_USIM_ACSGL,     0x01, MMGSDI_INVALID_REC_LEN,    MMGSDI_LINEAR_FIXED_FILE},    /* Allowed CSG lists */
  { MMGSDI_USIM_CSGT,      0x02, MMGSDI_INVALID_REC_LEN,    MMGSDI_LINEAR_FIXED_FILE},    /* CSG Type */
  { MMGSDI_USIM_HNBN,      0x03, MMGSDI_INVALID_REC_LEN,    MMGSDI_LINEAR_FIXED_FILE},    /* HNB name */
  { MMGSDI_USIM_OCSGL,     0x04, MMGSDI_INVALID_REC_LEN,    MMGSDI_LINEAR_FIXED_FILE},    /* Operator CSG lists */
  { MMGSDI_USIM_OCSGT,     0x05, MMGSDI_INVALID_REC_LEN,    MMGSDI_LINEAR_FIXED_FILE},    /* Operator CSG Type */
  { MMGSDI_USIM_OHNBN,     0x06, MMGSDI_INVALID_REC_LEN,    MMGSDI_LINEAR_FIXED_FILE}     /* Operator HNB name */
}; /* mmgsdi_usim_hnb_file_sfi_table */

/* MMGSDI File index table with mapping to path to DF, size and pointer
   to file enum table, size and pointer to sfi table */
const mmgsdi_file_index_table_type mmgsdi_file_index_table[] =
{
  { MMGSDI_EF_UNDER_MF_START,
    { 1, {0x3F00,0xFFFF,0xFFFF,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_ef_under_mf_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_ef_under_mf_enum_table,
    sizeof(mmgsdi_mf_file_sfi_table)/sizeof(mmgsdi_file_sfi_table_type),
    mmgsdi_mf_file_sfi_table },

  { MMGSDI_RUIM_EF_START,
    { 2, {0x3F00,0X7F25,0xFFFF,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_cdma_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_cdma_enum_table,
    0, NULL },

  { MMGSDI_GSM_EF_START,
    { 2, {0x3F00,0X7F20,0xFFFF,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_gsm_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_gsm_enum_table,
    0, NULL },

  { MMGSDI_GSM_SOLSA_DF_START,
    { 3, {0x3F00,0X7F20,0X5F70,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_gsm_solsa_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_gsm_solsa_enum_table,
    0, NULL },

  { MMGSDI_GSM_MEXE_DF_START,
    { 3, {0x3F00,0X7F20,0X5F3C,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_gsm_mexe_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_gsm_mexe_enum_table,
    0, NULL },

  { MMGSDI_TELECOM_EF_START,
    { 2, {0x3F00,0X7F10,0xFFFF,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_telecom_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_telecom_enum_table,
    0, NULL },

  { MMGSDI_TELECOM_GRAPHICS_DF_START,
    { 3, {0x3F00,0X7F10,0X5F50,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_telecom_graphics_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_telecom_graphics_enum_table,
    0, NULL },

  { MMGSDI_TELECOM_PHONEBOOK_DF_START,
    { 3, {0x3F00,0X7F10,0X5F3A,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_telecom_phonebook_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_telecom_phonebook_enum_table,
    0, NULL },

  { MMGSDI_TELECOM_MM_DF_START,
    { 3, {0x3F00,0X7F10,0X5F3B,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_telecom_mm_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_telecom_mm_enum_table,
    0, NULL },

  { MMGSDI_TELECOM_MMSS_DF_START,
    { 3, {0x3F00,0X7F10,0X5F3C,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_telecom_mmss_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_telecom_mmss_enum_table,
    0, NULL },

  { MMGSDI_USIM_EF_START,
    { 2, {0x3F00,0X7FFF,0xFFFF,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_usim_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_usim_enum_table,
    sizeof(mmgsdi_usim_file_sfi_table)/sizeof(mmgsdi_file_sfi_table_type),
    mmgsdi_usim_file_sfi_table },

  { MMGSDI_USIM_PHONEBOOK_DF_START,
    { 3, {0x3F00,0X7FFF,0X5F3A,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_usim_phonebook_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_usim_phonebook_enum_table,
    0, NULL },

  { MMGSDI_USIM_GSM_ACCESS_DF_START,
    { 3, {0x3F00,0X7FFF,0X5F3B,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_usim_gsm_access_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_usim_gsm_access_enum_table,
    sizeof(mmgsdi_usim_gsm_access_file_sfi_table)/sizeof(mmgsdi_file_sfi_table_type),
    mmgsdi_usim_gsm_access_file_sfi_table },

  { MMGSDI_USIM_MEXE_DF_START,
    { 3, {0x3F00,0X7FFF,0X5F3C,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_usim_mexe_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_usim_mexe_enum_table,
    sizeof(mmgsdi_usim_wlan_file_sfi_table)/sizeof(mmgsdi_file_sfi_table_type),
    mmgsdi_usim_wlan_file_sfi_table },

  { MMGSDI_USIM_WLAN_DF_START,
    { 3, {0x3F00,0X7FFF,0X5F40,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_usim_wlan_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_usim_wlan_enum_table,
    0, NULL },

  { MMGSDI_USIM_HNB_DF_START,
    { 3, {0x3F00,0X7FFF,0X5F50,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_usim_hnb_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_usim_hnb_enum_table,
    sizeof(mmgsdi_usim_hnb_file_sfi_table)/sizeof(mmgsdi_file_sfi_table_type),
    mmgsdi_usim_hnb_file_sfi_table },

  { MMGSDI_DCS1800_EF_START,
    { 2, {0x3F00,0X7F21,0xFFFF,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_dcs1800_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_dcs1800_enum_table,
    0, NULL },

  { MMGSDI_ISIM_EF_START,
    { 2, {0x3F00,0X7FFF,0xFFFF,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_isim_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_isim_enum_table,
    0, NULL },

  { MMGSDI_ORANGE_SIM_EF_START,
    { 2, {0x3F00,0X7F40,0xFFFF,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_orange_sim_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_orange_sim_enum_table,
    0, NULL },

  { MMGSDI_ORANGE_USIM_EF_START,
    { 3, {0x3F00,0X7FFF,0X7F40,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_orange_usim_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_orange_usim_enum_table,
    0, NULL },

  { MMGSDI_CINGULAR_SIM_EF_START,
    { 2, {0x3F00,0x7F66,0xFFFF,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_cingular_sim_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_cingular_sim_enum_table,
    0, NULL },

  { MMGSDI_CINGULAR_SIM_NETWORK_DF_START,
    { 3, {0x3F00,0x7F66,0X5F30,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_cingular_sim_network_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_cingular_sim_network_enum_table,
    0, NULL },

  { MMGSDI_CINGULAR_SIM_RPM_DF_START,
    { 3, {0x3F00,0x7F66,0X5F40,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_cingular_sim_rpm_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_cingular_sim_rpm_enum_table,
    0, NULL },

  { MMGSDI_CINGULAR_USIM_EF_START,
    { 3, {0x3F00,0X7FFF,0X7F66,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_cingular_usim_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_cingular_usim_enum_table,
    0, NULL },

  { MMGSDI_CINGULAR_USIM_NETWORK_DF_START,
    { 4, {0x3F00,0X7FFF,0x7F66,0X5F30,0xFFFF}},
    sizeof(mmgsdi_file_cingular_usim_network_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_cingular_usim_network_enum_table,
    0, NULL },

  { MMGSDI_CINGULAR_USIM_RPM_DF_START,
    { 4, {0x3F00,0X7FFF,0x7F66,0X5F40,0xFFFF}},
    sizeof(mmgsdi_file_cingular_usim_rpm_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_cingular_usim_rpm_enum_table,
    0, NULL },

  { MMGSDI_HZI_EF_START,
    { 2, {0x3F00,0X7F43,0xFFFF,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_hzi_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_hzi_enum_table,
    0, NULL },

  { MMGSDI_CSIM_EF_START,
    { 2, {0x3F00,0X7FFF,0xFFFF,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_csim_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_csim_enum_table,
    sizeof(mmgsdi_csim_file_sfi_table)/sizeof(mmgsdi_file_sfi_table_type),
    mmgsdi_csim_file_sfi_table },

  { MMGSDI_CSIM_PHONEBOOK_DF_START,
    { 3, {0x3F00,0X7FFF,0X5F3A,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_csim_phonebook_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_csim_phonebook_enum_table,
    sizeof(mmgsdi_csim_file_sfi_table)/sizeof(mmgsdi_file_sfi_table_type),
    mmgsdi_csim_file_sfi_table },

  { MMGSDI_CSIM_JCDMA_DF_START,
    { 3, {0x3F00,0X7FFF,0X5FEF,0xFFFF,0xFFFF}},
    sizeof(mmgsdi_file_csim_jcdma_enum_table)/sizeof(mmgsdi_file_enum_table_type),
    mmgsdi_file_csim_jcdma_enum_table,
    sizeof(mmgsdi_csim_file_sfi_table)/sizeof(mmgsdi_file_sfi_table_type),
    mmgsdi_csim_file_sfi_table }
}; /* mmgsdi_file_index_table */


/*===========================================================================
         S T A T I C    F U N C T I O N
============================================================================*/

/*===========================================================================
FUNCTION: MMGSDI_FILE_ENUM_GET_EF_SRV_FROM_APP_TYPE

DESCRIPTION
  Find the Service enum type for EF that has different service type depending
  on the application type

DEPENDENCIES
  none

RETURN VALUE
  mmgsdi_service_enum_type

SIDE EFFECTS
  none
===========================================================================*/
static mmgsdi_service_enum_type mmgsdi_file_get_ef_srv_from_app_type(
  mmgsdi_file_enum_type file,
  mmgsdi_app_enum_type  app_type)
{
  uint32                     i          = 0;
  uint32                     table_size = 0;
  mmgsdi_service_enum_type   service    = MMGSDI_NONE;

  table_size = sizeof(mmgsdi_file_ef_srv_from_app_type_table) /
                                        sizeof(mmgsdi_ef_srv_from_app_type);

  for (i = 0; i < table_size; i++)
  {
    if(mmgsdi_file_ef_srv_from_app_type_table[i].file == file)
    {
      switch(app_type)
      {
        case MMGSDI_APP_SIM:
          service = mmgsdi_file_ef_srv_from_app_type_table[i].sim_srv;
          break;
        case MMGSDI_APP_RUIM:
          service = mmgsdi_file_ef_srv_from_app_type_table[i].ruim_srv;
          break;
        case MMGSDI_APP_USIM:
          service = mmgsdi_file_ef_srv_from_app_type_table[i].usim_srv;
          break;
        case MMGSDI_APP_CSIM:
          service = mmgsdi_file_ef_srv_from_app_type_table[i].csim_srv;
          break;
        default:
          UIM_MSG_ERR_1("Invalid app_type 0x%x", app_type);
          service = MMGSDI_NONE;
          break;
      }
    }
  }

  return service;
}/* mmgsdi_file_get_ef_srv_from_app_type */


/*===========================================================================
FUNCTION MMGSDI_FILE_IS_SFI_SUPPORTED

DESCRIPTION
  Utility functions for finding out whether the SFI support is present in the
  card and whether SFI usage is enabled in NV or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If
        1. Card ATR mentions that SFI is supported AND
        2. SFI NV item is enabled
  FALSE: Otherwise
===========================================================================*/
boolean mmgsdi_file_is_sfi_supported(
  mmgsdi_slot_id_enum_type         slot_id)
{
  if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_USE_SFI, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED)
  {
    mmgsdi_slot_data_type *data_slot_ptr = NULL;

    data_slot_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

    if(data_slot_ptr)
    {
      return data_slot_ptr->sfi_supported;
    }
  }

  return FALSE;
} /* mmgsdi_file_is_sfi_supported */


/*===========================================================================
FUNCTION MMGSDI_FILE_GET_SFI_FROM_FILE

DESCRIPTION
  Utility functions for finding out the sfi for a particular EFs

DEPENDENCIES
  None

RETURN VALUE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_file_get_sfi_from_file(
  mmgsdi_file_enum_type  mmgsdi_file,
  uint8                 *sfi_ptr)
{
  const mmgsdi_file_sfi_table_type     *sfi_table_ptr = NULL;
  uint8                                 index_count    = 0;
  uint32                                sfi_count     = 0;
  uint32                                i              = 0;
  uint32                                j              = 0;

  if (sfi_ptr == NULL)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* Initialize to no file enum */
  *sfi_ptr = UIM_INVALID_SFI;

  index_count = sizeof(mmgsdi_file_index_table) / sizeof(mmgsdi_file_index_table_type);

  for ( i = 0; i < index_count; i++ )
  {
    if (mmgsdi_file_index_table[i].mmgsdi_file_index == MMGSDIFILE_GET_SFI_TABLE_INDEX_FROM_FILE(mmgsdi_file))
    {
      sfi_table_ptr = mmgsdi_file_index_table[i].mmgsdi_file_sfi_table_ptr;
      sfi_count = mmgsdi_file_index_table[i].mmgsdi_file_sfi_table_count;

      if (sfi_table_ptr != NULL)
      {
        for (j = 0; j < sfi_count; j++)
        {
          if (mmgsdi_file == sfi_table_ptr[j].mmgsdi_file)
          {
            *sfi_ptr = sfi_table_ptr[j].sfi;
            return MMGSDI_SUCCESS;
          }
        }
      }
    }
  }
  return MMGSDI_NOT_FOUND;
} /* mmgsdi_file_get_sfi_from_file */


/*===========================================================================
         S T A T I C    F U N C T I O N   P R O T O T Y P E S
============================================================================*/

/*===========================================================================
FUNCTION: MMGSDI_FILE_GET_SRV_FROM_FILE

DESCRIPTION
  Maps MMGSDI File Enum to MMGSDI Service

DEPENDENCIES
  none

RETURN VALUE
  Success/failure

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_file_get_srv_from_file(
  mmgsdi_app_enum_type       app_type,
  mmgsdi_file_enum_type      mmgsdi_file,
  mmgsdi_service_enum_type  *mmgsdi_services_type_ptr)
{
  const mmgsdi_file_enum_table_type    *enum_table_ptr = NULL;
  uint8                                 index_count    = 0;
  uint32                                enum_count     = 0;
  uint32                                i              = 0;
  uint32                                j              = 0;

  if (mmgsdi_services_type_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Initialize the gsdi_services_item_p */
  *mmgsdi_services_type_ptr = MMGSDI_NONE;

  index_count = sizeof(mmgsdi_file_index_table) / sizeof(mmgsdi_file_index_table_type);

  for ( i = 0; i < index_count; i++ )
  {
    if (mmgsdi_file_index_table[i].mmgsdi_file_index == MMGSDIFILE_GET_SFI_TABLE_INDEX_FROM_FILE(mmgsdi_file))
    {
      enum_table_ptr = mmgsdi_file_index_table[i].mmgsdi_file_enum_table_ptr;
      enum_count = mmgsdi_file_index_table[i].mmgsdi_file_enum_table_count;

      for (j = 0; j < enum_count; j++)
      {
        if (mmgsdi_file == enum_table_ptr[j].mmgsdi_file )
        {
          if (enum_table_ptr[j].service_type == MMGSDI_GET_SRV_FROM_APP_TYPE)
          {
            *mmgsdi_services_type_ptr =
              mmgsdi_file_get_ef_srv_from_app_type(mmgsdi_file, app_type);
          }
          else if (enum_table_ptr[j].service_type == MMGSDI_SRV_MAX)
          {
            /* Additional logic is required, e.g., several services && or ||
               together, calling functino will have to handle the logic */
            /* for now keep as MMGSDI_NONE */
          }
          else
          {
            *mmgsdi_services_type_ptr = enum_table_ptr[j].service_type;
          }
          return MMGSDI_SUCCESS;
        }
      }
    }
  }
  UIM_MSG_ERR_1("Cannot find file 0x%x in mmgsdi_file_enum_table",
                mmgsdi_file);
  return MMGSDI_NOT_FOUND;
} /* mmgsdi_file_get_srv_from_file */


/*===========================================================================
FUNCTION MMGSDI_FILE_GET_PATH_FROM_FILE

DESCRIPTION
  Utility functions for finding out the path for a particular EFs

DEPENDENCIES
  None

RETURN VALUE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_file_get_path_from_file(
  mmgsdi_file_enum_type  mmgsdi_file,
  mmgsdi_path_type      *file_path_ptr)
{
  const mmgsdi_file_enum_table_type   *enum_table_ptr = NULL;
  uint8                                 index_count    = 0;
  uint32                                enum_count     = 0;
  uint32                                i              = 0;
  uint32                                j              = 0;

  if (file_path_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Initialize to file path */
  file_path_ptr->path_len = 0;
  memset(file_path_ptr->path_buf, 0xFF, (sizeof(uint16) * MMGSDI_MAX_PATH_LEN));

  index_count = sizeof(mmgsdi_file_index_table) / sizeof(mmgsdi_file_index_table_type);

  for ( i = 0; i < index_count; i++ )
  {
    if (mmgsdi_file_index_table[i].mmgsdi_file_index == MMGSDIFILE_GET_SFI_TABLE_INDEX_FROM_FILE(mmgsdi_file))
    {
      enum_table_ptr = mmgsdi_file_index_table[i].mmgsdi_file_enum_table_ptr;
      enum_count = mmgsdi_file_index_table[i].mmgsdi_file_enum_table_count;

      for (j = 0; j < enum_count; j++)
      {
        if (mmgsdi_file == enum_table_ptr[j].mmgsdi_file)
        {
          if (mmgsdi_file_index_table[i].path_to_df.len > 0 &&
              mmgsdi_file_index_table[i].path_to_df.len < MMGSDI_MAX_PATH_LEN)
          {
            file_path_ptr->path_len = mmgsdi_file_index_table[i].path_to_df.len + 1;
            mmgsdi_memscpy(file_path_ptr->path_buf,
                           sizeof(uint16) * MMGSDI_MAX_PATH_LEN,
                           mmgsdi_file_index_table[i].path_to_df.path,
                           sizeof(uint16) * mmgsdi_file_index_table[i].path_to_df.len);
            file_path_ptr->path_buf[mmgsdi_file_index_table[i].path_to_df.len] =
                enum_table_ptr[j].file_id;

            return MMGSDI_SUCCESS;
          }
          else
          {
            UIM_MSG_ERR_2("invalid path_len 0x%x for file 0x%x",
                          mmgsdi_file_index_table[i].path_to_df.len, mmgsdi_file);
            return MMGSDI_NOT_FOUND;
          }
        }
      }
    }
  }
  UIM_MSG_ERR_1("Cannot find file 0x%x in mmgsdi_file_enum_table",
                mmgsdi_file);
  return MMGSDI_NOT_FOUND;
} /* mmgsdi_file_get_path_from_file */


/*===========================================================================
FUNCTION MMGSDI_FILE_GET_FILE_FROM_PATH

DESCRIPTION
  Utility functions for finding out the mmgsdi enum given a path

DEPENDENCIES
  None

RETURN VALUE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_file_get_file_from_path(
  mmgsdi_app_enum_type   app_type,
  mmgsdi_path_type       path,
  mmgsdi_file_enum_type *mmgsdi_file_ptr)
{
  const mmgsdi_file_enum_table_type    *enum_table_ptr = NULL;
  uint8                                 index_count    = 0;
  uint32                                enum_count     = 0;
  uint32                                i              = 0;
  uint32                                j              = 0;
  uint8                                 app_type_index = 0;

  if (mmgsdi_file_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Initialize to no file enum */
  *mmgsdi_file_ptr = MMGSDI_NO_FILE_ENUM;

  if ((path.path_len <= 1) ||
      (path.path_len > MMGSDI_MAX_PATH_LEN))
  {
    return MMGSDI_ERROR;
  }

  index_count = sizeof(mmgsdi_file_index_table) / sizeof(mmgsdi_file_index_table_type);

  for ( i = 0; i < index_count; i++ )
  {
    if ((mmgsdi_file_index_table[i].path_to_df.len  == (path.path_len -1)) &&
        (memcmp(path.path_buf,
                mmgsdi_file_index_table[i].path_to_df.path,
                (sizeof(uint16) * mmgsdi_file_index_table[i].path_to_df.len)) == 0))
    {
      enum_table_ptr = mmgsdi_file_index_table[i].mmgsdi_file_enum_table_ptr;
      enum_count = mmgsdi_file_index_table[i].mmgsdi_file_enum_table_count;

      for (j = 0;j < enum_count; j++)
      {
        if (path.path_buf[path.path_len-1] == enum_table_ptr[j].file_id )
        {
          if ((mmgsdi_file_index_table[i].mmgsdi_file_index & 0xF0) == MMGSDI_USIM_EF_START ||
              (mmgsdi_file_index_table[i].mmgsdi_file_index & 0xF0) == MMGSDI_CSIM_EF_START ||
              (mmgsdi_file_index_table[i].mmgsdi_file_index & 0xF0) == MMGSDI_ISIM_EF_START)
          {
            switch (app_type)
            {
               case MMGSDI_APP_USIM:
                 app_type_index = MMGSDI_USIM_EF_START;
                 break;
               case MMGSDI_APP_CSIM:
                 app_type_index = MMGSDI_CSIM_EF_START;
                 break;
               case MMGSDI_APP_UNKNOWN:
                 app_type_index = MMGSDI_ISIM_EF_START;
                 break;
               case MMGSDI_APP_RUIM:
               case MMGSDI_APP_SIM:
               case MMGSDI_APP_NONE:
               default:
                 UIM_MSG_ERR_2("invalid app_type 0x%x for mmgsdi_file_index: 0x%x",
                               app_type, path.path_buf);
                 return MMGSDI_NOT_FOUND;
            }
            if (app_type_index ==
                  MMGSDIFILE_GET_TECH_TYPE_FROM_FILE((uint16)enum_table_ptr[j].mmgsdi_file))
            {
              *mmgsdi_file_ptr = enum_table_ptr[j].mmgsdi_file;
              return MMGSDI_SUCCESS;
            }
          }
          else
          {
            *mmgsdi_file_ptr = enum_table_ptr[j].mmgsdi_file;
            return MMGSDI_SUCCESS;
          }
        }
      }
    }
  }
  UIM_MSG_ERR_2("Cannot find file for path len 0x%x, path buf 0x%x ",
                path.path_len, path.path_buf);
  return MMGSDI_NOT_FOUND;
} /* mmgsdi_file_get_file_from_path */


/* ============================================================================
FUNCTION MMGSDI_UTIL_SET_DEFAULT_SEC_ATTR

DESCRIPTION
  This function sets the default security attributes for various file
  access operations

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_file_set_default_sec_attr(
  mmgsdi_file_attributes_type * attr_ptr)
{
  mmgsdi_file_security_access_type * operation_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(attr_ptr);

  /* Update the security attributes based on the file type */
  switch(attr_ptr->file_type)
  {
    case MMGSDI_LINEAR_FIXED_FILE:
      operation_ptr = &attr_ptr->file_info.linear_fixed_file.file_security;
      break;
    case MMGSDI_CYCLIC_FILE:
      attr_ptr->file_info.cyclic_file.increase_allowed = TRUE;
      operation_ptr = &attr_ptr->file_info.cyclic_file.file_security;
      break;
    case MMGSDI_TRANSPARENT_FILE:
      operation_ptr = &attr_ptr->file_info.transparent_file.file_security;
      break;
    default:
      return MMGSDI_ERROR;
  }

  /* Set Default Security attributes for READ operation */
  operation_ptr->read.protection_method = MMGSDI_ALWAYS_ALLOWED;
  operation_ptr->read.num_protection_pin = 0;
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(operation_ptr->read.protection_pin_ptr);

  /* Set Default Security attributes for WRITE operation */
  operation_ptr->write.protection_method = MMGSDI_ALWAYS_ALLOWED;
  operation_ptr->write.num_protection_pin = 0;
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(operation_ptr->write.protection_pin_ptr);

  /* Set Default Security attributes for INCREASE operation */
  operation_ptr->increase.protection_method = MMGSDI_ALWAYS_ALLOWED;
  operation_ptr->increase.num_protection_pin = 0;
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(operation_ptr->increase.protection_pin_ptr);

  /* Set Default Security attributes for INVALIDATE/DEACTIVATE operation */
  operation_ptr->invalidate_deactivate.protection_method = MMGSDI_ALWAYS_ALLOWED;
  operation_ptr->invalidate_deactivate.num_protection_pin = 0;
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(operation_ptr->invalidate_deactivate.protection_pin_ptr);

  /* Set Default Security attributes for REHABILITATE/ACTIVATE operation */
  operation_ptr->rehabilitate_activate.protection_method = MMGSDI_ALWAYS_ALLOWED;
  operation_ptr->rehabilitate_activate.num_protection_pin = 0;
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(operation_ptr->rehabilitate_activate.protection_pin_ptr);

  return MMGSDI_SUCCESS;
}/* mmgsdi_file_set_default_sec_attr */


/* ============================================================================
FUNCTION MMGSDI_FILE_SET_DEFAULT_SEC_ATTR_PER_SPEC

DESCRIPTION
  This function sets the default security attributes for various file
  access operations DERIVED FROM the access conditions mentioned in the spec
  for the file. For the files for which the access condition is ADM or NEVER
  (per the spec), the access condition is defaulted to MMGSDI_NEVER_ALLOWED.
  For all other access conditions mentioned in the spec for a file, the access
  condition is defaulted to MMGSDI_ALWAYS_ALLOWED.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_file_set_default_sec_attr_per_spec(
  mmgsdi_file_attributes_type * attr_ptr,
  mmgsdi_file_enum_type         mmgsdi_file)
{
  mmgsdi_file_security_access_type * operation_ptr = NULL;
  uint32                             i             = 0;
  uint32                             table_size    = 0;

  MMGSDIUTIL_RETURN_IF_NULL(attr_ptr);

  /* Update the security attributes based on the file type */
  switch(attr_ptr->file_type)
  {
    case MMGSDI_LINEAR_FIXED_FILE:
      operation_ptr = &attr_ptr->file_info.linear_fixed_file.file_security;
      break;
     case MMGSDI_CYCLIC_FILE:
      operation_ptr = &attr_ptr->file_info.cyclic_file.file_security;
      break;
    case MMGSDI_TRANSPARENT_FILE:
      operation_ptr = &attr_ptr->file_info.transparent_file.file_security;
      break;
    default:
      return MMGSDI_ERROR;
  }

  /* Default READ,WRITE,INCREASE to ALWAYS_ALLOWED and everything else to
     NEVER_ALLOWED and then proceed with scanning the default security
     attributes table and update the values based on that. If an entry for
     a file is not found in that table, these default values are used.
     Also, Default increase_allowed to FALSE which may later be updated
     after scanning the default security attributes table. */
  attr_ptr->file_info.cyclic_file.increase_allowed       = FALSE;
  operation_ptr->read.protection_method                  = MMGSDI_ALWAYS_ALLOWED;
  operation_ptr->write.protection_method                 = MMGSDI_ALWAYS_ALLOWED;
  operation_ptr->increase.protection_method              = MMGSDI_ALWAYS_ALLOWED;
  operation_ptr->rehabilitate_activate.protection_method = MMGSDI_NEVER_ALLOWED;
  operation_ptr->invalidate_deactivate.protection_method = MMGSDI_NEVER_ALLOWED;

  table_size = sizeof(mmgsdi_file_default_file_sec_attr_table) /
                                        sizeof(mmgsdi_file_default_file_sec_attr_table_type);

  /* If an entry is found in the default security attributes table, update the
     default security attribute for the file */
  for (i = 0; i < table_size; i++)
  {
    if(mmgsdi_file_default_file_sec_attr_table[i].file == mmgsdi_file)
    {
      operation_ptr->read.protection_method =
        (mmgsdi_file_default_file_sec_attr_table[i].sec_attr_mask &  MMGSDI_READ_ALLOWED_MASK) ?
          MMGSDI_ALWAYS_ALLOWED : MMGSDI_NEVER_ALLOWED;
      operation_ptr->write.protection_method =
        (mmgsdi_file_default_file_sec_attr_table[i].sec_attr_mask & MMGSDI_WRITE_ALLOWED_MASK) ?
          MMGSDI_ALWAYS_ALLOWED : MMGSDI_NEVER_ALLOWED;
      operation_ptr->increase.protection_method =
        (mmgsdi_file_default_file_sec_attr_table[i].sec_attr_mask & MMGSDI_INCREASE_ALLOWED_MASK) ?
          MMGSDI_ALWAYS_ALLOWED : MMGSDI_NEVER_ALLOWED;
      operation_ptr->rehabilitate_activate.protection_method =
        (mmgsdi_file_default_file_sec_attr_table[i].sec_attr_mask & MMGSDI_ACTIVATE_ALLOWED_MASK) ?
          MMGSDI_ALWAYS_ALLOWED : MMGSDI_NEVER_ALLOWED;
      operation_ptr->invalidate_deactivate.protection_method =
        (mmgsdi_file_default_file_sec_attr_table[i].sec_attr_mask & MMGSDI_DEACTIVATE_ALLOWED_MASK) ?
          MMGSDI_ALWAYS_ALLOWED : MMGSDI_NEVER_ALLOWED;
      break;
    }
  }

  operation_ptr->read.num_protection_pin = 0;
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(operation_ptr->read.protection_pin_ptr);

  operation_ptr->write.num_protection_pin = 0;
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(operation_ptr->write.protection_pin_ptr);

  if(attr_ptr->file_type == MMGSDI_CYCLIC_FILE &&
     operation_ptr->increase.protection_method == MMGSDI_ALWAYS_ALLOWED)
  {
    attr_ptr->file_info.cyclic_file.increase_allowed = TRUE;
  }
  operation_ptr->increase.num_protection_pin = 0;
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(operation_ptr->increase.protection_pin_ptr);

  operation_ptr->invalidate_deactivate.num_protection_pin = 0;
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(operation_ptr->invalidate_deactivate.protection_pin_ptr);

  operation_ptr->rehabilitate_activate.num_protection_pin = 0;
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(operation_ptr->rehabilitate_activate.protection_pin_ptr);

  return MMGSDI_SUCCESS;
}/* mmgsdi_file_set_default_sec_attr_per_spec */


/* ==========================================================================
FUNCTION MMGSDI_FILE_IS_EF_UNDER_MF

DESCRIPTION
  This function determines if the EF is under MF

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  Boolean: TRUE if EF is under MF else FALSE

SIDE EFFECTS
  NONE
==========================================================================*/
boolean mmgsdi_file_is_ef_under_mf(const mmgsdi_access_type *access_ptr)
{
  uint8                   mmgsdi_file_base = 0;
  boolean                 result        = FALSE;

  if (access_ptr == NULL)
  {
    return result;
  }

  switch (access_ptr->access_method)
  {
    case MMGSDI_EF_ENUM_ACCESS:
      mmgsdi_file_base = (uint8) MMGSDIFILE_GET_TECH_TYPE_FROM_FILE(access_ptr->file.file_enum);
      if (mmgsdi_file_base == MMGSDI_EF_UNDER_MF_START)
      {
        result = TRUE;
      }
      break;

    case MMGSDI_DF_ENUM_ACCESS:
      break;

    case MMGSDI_BY_PATH_ACCESS:
      /* If the path length is 2 and the second file ID in the path does not
         start with '7F' (indicating a DF), interpret as file under MF */
      if ((access_ptr->file.path_type.path_len == 2) &&
          (access_ptr->file.path_type.path_buf[0x00] == 0x3F00) &&
          ((access_ptr->file.path_type.path_buf[0x01] & 0x7F00) != 0x7F00))
      {
        result = TRUE;
      }
      break;

    default:
      break;
  }

  return result;
} /* mmgsdi_file_is_ef_under_mf */


/* ==========================================================================
FUNCTION MMGSDI_FILE_IS_EF_UNDER_DF_IN_MF

DESCRIPTION
  This function determines if the EF is under a directory in MF (other than
  the GSM/RUIM DF and ADFs)

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  Boolean: TRUE if EF is under a directory in MF else FALSE

SIDE EFFECTS
  NONE
==========================================================================*/
boolean mmgsdi_file_is_ef_under_df_in_mf(const mmgsdi_access_type *access_ptr)
{
  uint8                   mmgsdi_file_base = 0;
  boolean                 result           = FALSE;

  if (access_ptr == NULL)
  {
    return result;
  }

  switch (access_ptr->access_method)
  {
    case MMGSDI_EF_ENUM_ACCESS:
      mmgsdi_file_base = (uint8) MMGSDIFILE_GET_TECH_TYPE_FROM_FILE(access_ptr->file.file_enum);
      if (mmgsdi_file_base != MMGSDI_RUIM_EF_START &&
          mmgsdi_file_base != MMGSDI_GSM_EF_START &&
          mmgsdi_file_base != MMGSDI_USIM_EF_START &&
          mmgsdi_file_base != MMGSDI_CSIM_EF_START &&
          mmgsdi_file_base != MMGSDI_ISIM_EF_START)
      {
        result = TRUE;
      }
      break;

    case MMGSDI_BY_PATH_ACCESS:
      if (access_ptr->file.path_type.path_len > 2 &&
          access_ptr->file.path_type.path_buf[0] == 0x3F00 &&
          access_ptr->file.path_type.path_buf[1] != MMGSDI_ADF_LEVEL &&
          access_ptr->file.path_type.path_buf[1] != MMGSDI_RUIM_LEVEL &&
          access_ptr->file.path_type.path_buf[1] != MMGSDI_GSM_LEVEL)
      {
        result = TRUE;
      }
      break;

    case MMGSDI_DF_ENUM_ACCESS:
    default:
      break;
  }

  return result;
} /* mmgsdi_file_is_ef_under_df_in_mf */


/* ==========================================================================
FUNCTION MMGSDI_FILE_IS_DF_UNDER_MF

DESCRIPTION
  This function determines if the DF is under MF

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  Boolean: TRUE if DF is under MF else FALSE

SIDE EFFECTS
  NONE
==========================================================================*/
boolean mmgsdi_file_is_df_under_mf(const mmgsdi_access_type *access_ptr)
{
  boolean result = FALSE;

  if (access_ptr == NULL)
  {
    UIM_MSG_ERR_0("mmgsdi_file_is_df_under_mf: NULL mmgsdi_access_type pointer");
    return result;
  }

  switch (access_ptr->access_method)
  {
    case MMGSDI_EF_ENUM_ACCESS:
      break;

    case MMGSDI_DF_ENUM_ACCESS:
      if (access_ptr->file.df_enum == MMGSDI_MF_DIR ||
          access_ptr->file.df_enum == MMGSDI_GSM_DIR ||
          access_ptr->file.df_enum == MMGSDI_TELECOM_DIR ||
          access_ptr->file.df_enum == MMGSDI_DCS1800_DIR ||
          access_ptr->file.df_enum == MMGSDI_RUIM_DIR)
      {
        result = TRUE;
      }
      break;

    case MMGSDI_BY_PATH_ACCESS:
      /* DF IDs are 7FXX. So check if second file ID is not 7FFF (i.e. ADF) and
         starts with 7F */
      if (access_ptr->file.path_type.path_len == 2 &&
          access_ptr->file.path_type.path_buf[0x00] == 0x3F00 &&
          access_ptr->file.path_type.path_buf[0x01] != 0x7FFF &&
          ((access_ptr->file.path_type.path_buf[0x01] & 0x7F00) == 0x7F00))
      {
        result = TRUE;
      }
      break;

    default:
      break;
  }

  return result;
} /* mmgsdi_file_is_df_under_mf */

/* ==========================================================================
FUNCTION MMGSDI_FILE_IS_ACCESS_OUTSIDE_ADF

DESCRIPTION
  This function determines if the EF/DF is outside of ADFs

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  Boolean: TRUE if EF/DF access is allowed, else FALSE

SIDE EFFECTS
  NONE
==========================================================================*/
boolean mmgsdi_file_is_access_outside_adf(
  const mmgsdi_access_type *access_ptr)
{
  uint8                   mmgsdi_file_base = 0;
  boolean                 allow         = TRUE;

  if (access_ptr == NULL)
  {
    return FALSE;
  }

  switch(access_ptr->access_method)
  {
    case MMGSDI_EF_ENUM_ACCESS:

      mmgsdi_file_base = (uint8) MMGSDIFILE_GET_TECH_TYPE_FROM_FILE(access_ptr->file.file_enum);

      /* Only files outside of ADFs can be accessed by card sessions */
      if (mmgsdi_file_base == MMGSDI_USIM_EF_START ||
          mmgsdi_file_base == MMGSDI_ISIM_EF_START ||
          mmgsdi_file_base == MMGSDI_CSIM_EF_START)
      {
          allow = FALSE;
      }
      break;

    case MMGSDI_DF_ENUM_ACCESS:
      if (access_ptr->file.df_enum == MMGSDI_LAST_SEL_ADF_DIR)
      {
        allow = FALSE;
      }
      break;

    case MMGSDI_BY_PATH_ACCESS:
      /* Allow EF/DF outside ADF only */
      if (access_ptr->file.path_type.path_len >= 2 &&
          access_ptr->file.path_type.path_buf[0x01] == 0x7FFF)
      {
        allow = FALSE;
      }
      break;

    default:
      allow = FALSE;
      break;
  }

  if (allow == FALSE)
  {
    UIM_MSG_ERR_1("access outside ADF is false; access method 0x%x",
                  access_ptr->access_method);
  }
  return allow;
} /* mmgsdi_file_is_access_outside_adf */


/* ============================================================================
FUNCTION MMGSDI_FILE_IS_VALID_ACCESS

DESCRIPTION
  This function takes the session_type and MMGSDI File enum and determines
  if the file access is valid for the session type

DEPENDENCIES
  None

LIMITATIONS
  Supports only Provisioning sessions

RETURN VALUE
  boolean:  TRUE : Valid access
            FALSE: Invalid access

SIDE EFFECTS
  None
============================================================================*/
boolean mmgsdi_file_is_valid_access(
  mmgsdi_session_type_enum_type session_type,
  mmgsdi_file_enum_type         mmgsdi_file
)
{
  mmgsdi_uim_ef_item_category_enum_type EF_category = ROOT;

  EF_category = MMGSDIFILE_GET_CATEGORY_FROM_FILE(mmgsdi_file);

  switch(session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
    case MMGSDI_GW_PROV_SEC_SESSION:
    case MMGSDI_GW_PROV_TER_SESSION:
      if((EF_category == CDMA) || (EF_category == CSIM))
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }
    case MMGSDI_1X_PROV_PRI_SESSION:
    case MMGSDI_1X_PROV_SEC_SESSION:
    case MMGSDI_1X_PROV_TER_SESSION:
      if((EF_category == GSM) || (EF_category == USIM))
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }
    default:
      return TRUE;
  }
} /* mmgsdi_file_is_valid_access */


/*===========================================================================
FUNCTION MMGSDI_FILE_SET_UIM_PATH

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_file_set_uim_path(
  const mmgsdi_access_type                *mmgsdi_access_ptr,
  uim_path_type                           *uim_path_ptr
)
{
  mmgsdi_return_enum_type                  mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_path_type                         mmgsdi_path;


  MMGSDIUTIL_RETURN_IF_NULL_2(mmgsdi_access_ptr, uim_path_ptr);

  /* initialize path array */
  memset(uim_path_ptr->path, 0xFF, sizeof(uim_path_ptr->path));

  switch (mmgsdi_access_ptr->access_method)
  {
     case MMGSDI_EF_ENUM_ACCESS:
       mmgsdi_status = mmgsdi_file_get_path_from_file(mmgsdi_access_ptr->file.file_enum,
                                                      &mmgsdi_path);
       if(mmgsdi_status == MMGSDI_SUCCESS)
       {
         uim_path_ptr->len = (uint8)mmgsdi_path.path_len;
         mmgsdi_memscpy(uim_path_ptr->path,
                        sizeof(word) * uim_path_ptr->len,
                        mmgsdi_path.path_buf,
                        sizeof(uint16) * mmgsdi_path.path_len);
       }
       break;

     case MMGSDI_BY_PATH_ACCESS:
       uim_path_ptr->len = (uint8)mmgsdi_access_ptr->file.path_type.path_len;
       mmgsdi_memscpy(uim_path_ptr->path,
                      sizeof(word) * uim_path_ptr->len,
                      mmgsdi_access_ptr->file.path_type.path_buf,
                      sizeof(uint16) * mmgsdi_access_ptr->file.path_type.path_len);
       mmgsdi_status = MMGSDI_SUCCESS;
       break;

     case MMGSDI_BY_APP_ID_ACCESS:
       uim_path_ptr->len = 2;
       uim_path_ptr->path[0] = MMGSDI_MF_LEVEL;
       uim_path_ptr->path[1] = MMGSDI_ADF_LEVEL;
       break;

     case MMGSDI_DF_ENUM_ACCESS:
       uim_path_ptr->len = 2;
       uim_path_ptr->path[0] = MMGSDI_MF_LEVEL;
       uim_path_ptr->path[1] = mmgsdi_access_ptr->file.df_enum;
       break;

      default:
        UIM_MSG_ERR_1("Unable to convert to UIM path, invalid access_method : 0x%x",
                      mmgsdi_access_ptr->access_method);
        mmgsdi_status = MMGSDI_ERROR;
    }

  return mmgsdi_status;
} /* mmgsdi_file_set_uim_path */


/*===========================================================================
FUNCTION MMGSDI_FILE_GET_SFI_FROM_ATTR_CACHE

DESCRIPTION
  Retrive SFI from the attributes cache for the EF, if present

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_file_get_sfi_from_attr_cache(
  mmgsdi_session_id_type                   session_id,
  mmgsdi_slot_id_enum_type                 slot_id,
  const mmgsdi_access_type                *access_ptr,
  uint8                                   *sfi_ptr
)
{
  mmgsdi_return_enum_type         mmgsdi_status   = MMGSDI_ERROR;
  mmgsdi_attr_cache_element_type *attr_cache_ptr  = NULL;

  MMGSDIUTIL_RETURN_IF_NULL_2(access_ptr, sfi_ptr);

  /* Try retrieving the SFI from file attributes cache */
  mmgsdi_status = mmgsdi_cache_attr_read(session_id,
                                         slot_id,
                                         access_ptr,
                                         &attr_cache_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    *sfi_ptr = attr_cache_ptr->sfi;
    mmgsdi_cache_delete_file_attr_cache_item(&attr_cache_ptr);
  }

  return mmgsdi_status;
} /* mmgsdi_file_get_sfi_from_attr_cache */


/*===========================================================================
  FUNCTION MMGSDI_FILE_IS_SKIP_UICC_ARR

  DESCRIPTION
    Checks if reading EF ARR should be skipped for the file

  PARAMETERS
    access  : Access type
    slot_id : Which slot to be accessed

  RETURN VALUE
    TRUE  : Skip reading EF ARR
    FALSE : read EF ARR

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean mmgsdi_file_is_skip_uicc_arr (
  mmgsdi_access_type        access,
  mmgsdi_slot_id_enum_type  slot_id
)
{
  uint8                  i = 0;
  mmgsdi_file_enum_type  read_uicc_arr_file_list[] =
  {
    MMGSDI_CSIM_3GPD_SIPUPP,
    MMGSDI_CSIM_3GPD_MIPUPP,
    MMGSDI_CSIM_HRPDUPP
  };
  mmgsdi_nv_context_type nv_context = MMGSDI_NV_CONTEXT_INVALID;

  /* Do not skip reading EF ARR if all of the followings apply
     1. File access type is MMGSDI_EF_ENUM_ACCESS
     2. MMGSDI_FEATURE_CSIM_UPP_UPDATE_AFTER_OTASP is enabled
     3. The file is in the read_uicc_arr_file_list[]
   */
  nv_context = mmgsdi_util_get_efs_item_index_for_slot(slot_id);
  if (MMGSDI_EF_ENUM_ACCESS  == access.access_method &&
      MMGSDI_FEATURE_ENABLED ==
        mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_CSIM_UPP_UPDATE_AFTER_OTASP,
                                     nv_context)
      )
  {
    for (i = 0;
         i < sizeof(read_uicc_arr_file_list)/sizeof(read_uicc_arr_file_list[0]);
         i++)
    {
      if (access.file.file_enum == read_uicc_arr_file_list[i])
      {
        return FALSE;
      }
    }
  }

  return TRUE;
} /* mmgsdi_file_is_skip_uicc_arr  */


/*===========================================================================
FUNCTION MMGSDI_FILE_SET_UIM_SFI

DESCRIPTION
  Locates the SFI in the static tables and/or attributes caches and returns the
  SFI

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_file_set_uim_sfi(
  mmgsdi_session_id_type                   session_id,
  mmgsdi_slot_id_enum_type                 slot_id,
  const mmgsdi_access_type                *mmgsdi_access_ptr,
  uint8                                   *sfi_ptr
)
{
  mmgsdi_return_enum_type         mmgsdi_status   = MMGSDI_ERROR;

  MMGSDIUTIL_RETURN_IF_NULL_2(mmgsdi_access_ptr, sfi_ptr);

  /* initialize path array */
  *sfi_ptr = UIM_INVALID_SFI;

  if (mmgsdi_file_is_sfi_supported(slot_id) == FALSE)
  {
    UIM_MSG_MED_0("SFI not supported");
    return MMGSDI_ERROR;
  }

  if (mmgsdi_access_ptr->access_method == MMGSDI_EF_ENUM_ACCESS)
  {
    mmgsdi_status = mmgsdi_file_get_sfi_from_file(mmgsdi_access_ptr->file.file_enum,
                                                  sfi_ptr);
  }

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_status = mmgsdi_file_get_sfi_from_attr_cache(session_id,
                                                        slot_id,
                                                        mmgsdi_access_ptr,
                                                        sfi_ptr);
  }

  return mmgsdi_status;
} /* mmgsdi_file_set_uim_sfi */


/* ==========================================================================
   FUNCTION:      MMGSDI_FILE_DETERMINE_EF_ENUM_DIR_LEVELS

   DESCRIPTION:
     This function determines the directory level for the EF enum type
     Result will be populated into the passed in pointer

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.


   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_file_determine_ef_enum_df_levels(
  mmgsdi_file_enum_type       mmgsdi_file,
  uim_dir_type               *uim_dir_level_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  uint32                  mmgsdi_ef_start_value = 0;

  MMGSDIUTIL_RETURN_IF_NULL(uim_dir_level_ptr);
  MMGSDIUTIL_RETURN_IF_EXCEEDS(mmgsdi_file, MMGSDI_NO_FILE_ENUM);

  mmgsdi_ef_start_value = MMGSDIFILE_GET_TECH_TYPE_FROM_FILE(mmgsdi_file);

  switch(mmgsdi_ef_start_value)
  {
    case MMGSDI_EF_UNDER_MF_START: /* 0x00 */
      *uim_dir_level_ptr = 0x3F00;
      break;

    case MMGSDI_RUIM_EF_START: /* 0x10 */
      *uim_dir_level_ptr = 0x7F25;
      break;

    case MMGSDI_GSM_EF_START: /* 0x20 */
      *uim_dir_level_ptr = 0x7F20;
      break;

    case MMGSDI_TELECOM_EF_START: /* 0x30 */
      *uim_dir_level_ptr = 0x7F10;
      break;

    case MMGSDI_USIM_EF_START: /* 0x40 */
    case MMGSDI_CSIM_EF_START: /* 0xA0 */
    case MMGSDI_ISIM_EF_START: /* 0x60 */
      *uim_dir_level_ptr = 0x7FFF;
      break;

    case MMGSDI_DCS1800_EF_START: /* 0x50 */
      *uim_dir_level_ptr = 0x7F21;
      break;

    case MMGSDI_ORANGE_SIM_EF_START: /* 0x70 */
    case MMGSDI_CINGULAR_SIM_EF_START: /* 0x80 */
      *uim_dir_level_ptr = 0xFFFF;
      break;

    case MMGSDI_EF_PATH_START:
      if ((mmgsdi_file == MMGSDI_PKCS15_PRKDF) ||
          (mmgsdi_file == MMGSDI_PKCS15_PUKDF) ||
          (mmgsdi_file == MMGSDI_PKCS15_SKDF) ||
          (mmgsdi_file == MMGSDI_PKCS15_CDF) ||
          (mmgsdi_file == MMGSDI_PKCS15_DODF) ||
          (mmgsdi_file == MMGSDI_PKCS15_AODF))
      {
        *uim_dir_level_ptr = 0xFFFF;
      }
      else
      {
        UIM_MSG_ERR_1("EF_ENUM access when EF_PATH should be used uim_item:0x%x", mmgsdi_file);
        mmgsdi_status = MMGSDI_NOT_FOUND;
      }
      break;

    default:
      UIM_MSG_ERR_1("Invalid UIM EF level category - 0x%x", mmgsdi_ef_start_value);
      *uim_dir_level_ptr = 0xFFFF;
      break;
  } /* switch */

  return mmgsdi_status;
} /* mmgsdi_file_determine_ef_enum_df_levels */


/* ==========================================================================
   FUNCTION:      MMGSDI_FILE_DETERMINE_UIM_DIR_TYPE

   DESCRIPTION:
     This function determines the uim directory type for MF, DF, ADF and EFs

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.


   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_file_determine_uim_dir_type (
  const mmgsdi_access_type   *mmgsdi_access_ptr,
  uim_dir_type               *uim_dir_ptr
)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL_2(uim_dir_ptr, mmgsdi_access_ptr);

  *uim_dir_ptr = 0;

  switch(mmgsdi_access_ptr->access_method)
  {
    case MMGSDI_EF_ENUM_ACCESS:
      MMGSDIUTIL_RETURN_IF_EXCEEDS(mmgsdi_access_ptr->file.file_enum,
                                  MMGSDI_NO_FILE_ENUM);
      mmgsdi_status = mmgsdi_file_determine_ef_enum_df_levels(
                                   mmgsdi_access_ptr->file.file_enum,
                                   uim_dir_ptr);
      break;
    case MMGSDI_BY_APP_ID_ACCESS:
      *uim_dir_ptr = 0x7FFF;
      break;
    case MMGSDI_DF_ENUM_ACCESS:
      *uim_dir_ptr = (uim_dir_type)mmgsdi_access_ptr->file.df_enum;
      break;
    default:
      break;
      /* Future:
         MF => UIM_MF
         DF => first level DF: DF1
            => subsequent levelts: 0xFFFF
         ADF => UIM_ADF
      */
  }

  return mmgsdi_status;
} /* mmgsdi_file_determine_uim_dir_type */


/* ============================================================================
FUNCTION MMGSDI_FILE_GET_FILE_INFO_FROM_TABLE

DESCRIPTION
  This utility function gets the file info such as rec_len/file_size and the
  file attributes by table lookup via file_enum

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_file_get_file_info_from_table(
  mmgsdi_file_enum_type              mmgsdi_file,
  mmgsdi_file_sfi_table_type        *file_info)
{
  const mmgsdi_file_sfi_table_type     *sfi_table_ptr  = NULL;
  uint8                                 index_count    = 0;
  uint32                                sfi_count      = 0;
  uint32                                i              = 0;
  uint32                                j              = 0;

  if (file_info == NULL)
  {
    UIM_MSG_ERR_0("file info ptr NULL");
    return MMGSDI_ERROR;
  }

  /* Initialize out variables */
  file_info->file_type = MMGSDI_MAX_FILE_STRUCTURE_ENUM;
  file_info->size      = MMGSDI_INVALID_FILE_SIZE;
  file_info->sfi       = UIM_INVALID_SFI;

  index_count = sizeof(mmgsdi_file_index_table) / sizeof(mmgsdi_file_index_table_type);

  for ( i = 0; i < index_count; i++ )
  {
    if (mmgsdi_file_index_table[i].mmgsdi_file_index == MMGSDIFILE_GET_SFI_TABLE_INDEX_FROM_FILE(mmgsdi_file))
    {
      sfi_table_ptr = mmgsdi_file_index_table[i].mmgsdi_file_sfi_table_ptr;
      sfi_count = mmgsdi_file_index_table[i].mmgsdi_file_sfi_table_count;

      if (sfi_table_ptr != NULL)
      {
        for (j = 0; j < sfi_count; j++)
        {
          if (mmgsdi_file == sfi_table_ptr[j].mmgsdi_file)
          {
            file_info->file_type   = sfi_table_ptr[j].file_type;
            file_info->size        = sfi_table_ptr[j].size;
            file_info->mmgsdi_file = sfi_table_ptr[j].mmgsdi_file;
            file_info->sfi         = sfi_table_ptr[j].sfi;
            return MMGSDI_SUCCESS;
          }
        }
      }
      /* File not found in the SFI table corresponding to the file index.
         No need to look further into tables for other file indexes. */
      break;
    }
  }

  return MMGSDI_NOT_FOUND;
} /* mmgsdi_file_get_file_info_from_table */


/* ============================================================================
FUNCTION MMGSDI_FILE_CHECK_FILE_INFO_AND_UPDATE_LEN

DESCRIPTION
  This utility function performs sanity checks on file size / record size of
  file:
  1. For cyclic/linear files:
    a) Get record length of EF
    b) If len passed into the function is greater than record length, truncate
       the len to the record size.
    c) If len passed into the function is less than the record length, return
       ERROR
    d) If len passed into the function is zero, update len to the record length
       that was retrived in step 1a) above.
  2. For transparent files:
    a) Get file size
    b) If len passed into the function is greater than the file size, truncate
       the len.
    c) If len passed into the function is less than the file size, it is a valid
       usecase.
    d) If len passed into the function is zero, update len to the file size
       that was retrived in step 2a) above.

  How is record length for cyclic/linear file or file size for transparent file
  determined:
  1. First a static table look up is performed for the EF.
  2. If record_len/file_size not found there, get the attributes from the
     attributes cache (as part of this step, if the attributes are not found to
     be cached yet, attributes are read from the card and cached.
  3. If the EF attributes are for some reason not cached at step2, we send a SELECT
     to the card without caching the attributes upon getting a response.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_file_check_file_info_and_update_len(
  mmgsdi_session_id_type             session_id,
  mmgsdi_slot_id_enum_type           slot_id,
  mmgsdi_access_type                 access,
  mmgsdi_file_structure_enum_type    file_type,
  mmgsdi_offset_type                 offset,
  mmgsdi_len_type                   *data_len_ptr,
  boolean                            is_select_mandatory)
{
  mmgsdi_file_sfi_table_type          file_info;
  mmgsdi_get_file_attr_cnf_type      *select_cnf_ptr              = NULL;
  mmgsdi_get_file_attr_req_type      *select_ptr                  = NULL;
  mmgsdi_return_enum_type             mmgsdi_status               = MMGSDI_SUCCESS;
  mmgsdi_attr_cache_element_type     *attr_cache_ptr              = NULL;
  mmgsdi_protocol_enum_type           protocol                    = MMGSDI_NO_PROTOCOL;
  mmgsdi_len_type                     cache_data_len              = 0;
  mmgsdi_cache_init_enum_type         cache_state                 = MMGSDI_CACHE_MAX_ENUM;
  boolean                             is_cache_not_init           = FALSE;

  if(data_len_ptr == NULL ||
     *data_len_ptr < 0 ||
     offset < 0)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  memset(&file_info, 0x00, sizeof(mmgsdi_file_sfi_table_type));

  protocol = mmgsdi_util_determine_protocol(slot_id);

  /* Check whether the requested file is part of cache list */
  mmgsdi_status = mmgsdi_cache_read_len(session_id,
                                        &access,
                                        &cache_data_len,
                                        0,
                                        &cache_state);
  if(mmgsdi_status == MMGSDI_NOT_INIT)
  {
    is_cache_not_init = TRUE;
  }

  /* Lets look into our file attr cache and see if we have already cached
     these attributes there... If 'is_select_mandatory' is TRUE, make sure
     we must send an explicit SELECT to the card to fetch attributes
    (thereby not using the attributes stored in static MMGSDI tables) */
  mmgsdi_status = mmgsdi_cache_populate_attr_cache_if_needed(
                    mmgsdi_generic_data_ptr->client_id,
                    session_id,
                    slot_id,
                    mmgsdi_file_is_skip_uicc_arr(access, slot_id),
                    is_select_mandatory,
                    &access,
                    &attr_cache_ptr,
                    NULL);

  if(mmgsdi_status == MMGSDI_SUCCESS &&
     attr_cache_ptr != NULL)
  {
    switch(attr_cache_ptr->file_type)
    {
      case MMGSDI_TRANSPARENT_FILE:
        /* If the file is part of cache list then read the entire file from card.
           We do this only for first time when the file is not present in the cache.
           For subsequent partial/complete read requests, the contents are
           directly returned from cache. In case of PRL and EPRL we have
           different reading logic (to read 2 bytes first) so skip updating
           the length if file is PRL or EPRL */
        if(is_cache_not_init && 
           access.file.file_enum != MMGSDI_CSIM_PRL  &&
           access.file.file_enum != MMGSDI_CDMA_PRL  &&
           access.file.file_enum != MMGSDI_CSIM_EPRL &&
           access.file.file_enum != MMGSDI_CDMA_EPRL)
        {
          *data_len_ptr = attr_cache_ptr->file_size;
        }
        else if(attr_cache_ptr->file_size >= *data_len_ptr + offset)
        {
          if(*data_len_ptr == 0)
          {
            *data_len_ptr = attr_cache_ptr->file_size - offset;
          }
        }
        else if((attr_cache_ptr->file_size < *data_len_ptr + offset) &&
                (attr_cache_ptr->file_size > offset))
        {
          *data_len_ptr = attr_cache_ptr->file_size - offset;
        }
        else
        {
          mmgsdi_status = MMGSDI_ERROR;
        }
        if (file_type != MMGSDI_TRANSPARENT_FILE)
        {
          mmgsdi_status = MMGSDI_EF_INCONSISTENT;
        }
        break;
      case MMGSDI_LINEAR_FIXED_FILE:
      case MMGSDI_CYCLIC_FILE:
        if(*data_len_ptr == 0 ||
           *data_len_ptr > attr_cache_ptr->rec_len)
        {
          *data_len_ptr = attr_cache_ptr->rec_len;
        }
        else if(attr_cache_ptr->rec_len != *data_len_ptr)
        {
          mmgsdi_status = MMGSDI_ERROR;
        }
        if (file_type == MMGSDI_TRANSPARENT_FILE)
        {
          /* Ensure that transparent EFs are not accessed via the APIs for
             record-based EFs. */
          mmgsdi_status = MMGSDI_EF_INCONSISTENT;
        }
        break;
      default:
        mmgsdi_status = MMGSDI_ERROR;
        break;
    }
    mmgsdi_cache_delete_file_attr_cache_item(&attr_cache_ptr);
    if(MMGSDI_SUCCESS != mmgsdi_status)
    {
      UIM_MSG_ERR_3("File info check failed: file_type 0x%x, offset 0x%x, data_len 0x%x",
                    file_type, offset, *data_len_ptr);
    }

    return mmgsdi_status;
  }
  else if (mmgsdi_status == MMGSDI_NOT_FOUND)
  {
    mmgsdi_cache_delete_file_attr_cache_item(&attr_cache_ptr);
    return MMGSDI_NOT_FOUND;
  }
  else if (access.access_method == MMGSDI_EF_ENUM_ACCESS)
  {
    /* Before sending the SELECT request to the
       card, lets once try to look up the SFI static table for the
       file info.. and if we find it there, we can avoid sending
       a SELECT to the card for size, file_type related info.
       NOTE that we access the static SFI table only if the original
       access method is BY ENUM. This is because we want to sent a
       SELECT to the card the first time the SELECT is performed by PATH*/
    mmgsdi_cache_delete_file_attr_cache_item(&attr_cache_ptr);

    mmgsdi_status = mmgsdi_file_get_file_info_from_table(access.file.file_enum, &file_info);

    if(MMGSDI_SUCCESS == mmgsdi_status &&
       file_info.size != MMGSDI_INVALID_FILE_SIZE)
    {
      switch(file_info.file_type)
      {
        case MMGSDI_TRANSPARENT_FILE:
           /* If the file is part of cache list then read the entire file from
             card. We do this only for first time when the file is not present
             in the cache. For subsequent partial/complete read requests, the
             contents are directly returned from cache. In case of PRL and
             EPRL we have different reading logic (to read 2 bytes first)
             so skip updating the length if file is PRL or EPRL */
          if(is_cache_not_init && 
             access.file.file_enum != MMGSDI_CSIM_PRL  &&
             access.file.file_enum != MMGSDI_CDMA_PRL  &&
             access.file.file_enum != MMGSDI_CSIM_EPRL &&
             access.file.file_enum != MMGSDI_CDMA_EPRL)
          {
            *data_len_ptr = file_info.size;
          }
          else if (file_info.size >= *data_len_ptr + offset)
          {
            if (*data_len_ptr == 0)
            {
              *data_len_ptr = file_info.size - offset;
            }
          }
          else if((file_info.size < *data_len_ptr + offset) &&
                  (file_info.size > offset))
          {
            *data_len_ptr = file_info.size - offset;
          }
          else
          {
            mmgsdi_status = MMGSDI_ERROR;
          }
          if (file_type != MMGSDI_TRANSPARENT_FILE)
          {
            mmgsdi_status = MMGSDI_EF_INCONSISTENT;
          }
          break;
        case MMGSDI_LINEAR_FIXED_FILE:
        case MMGSDI_CYCLIC_FILE:
          if(*data_len_ptr == 0 ||
             *data_len_ptr > file_info.size)
          {
            *data_len_ptr = file_info.size;
          }
          else if(file_info.size != *data_len_ptr)
          {
            mmgsdi_status = MMGSDI_ERROR;
          }
          if (file_type == MMGSDI_TRANSPARENT_FILE)
          {
            /* Ensure that transparent EFs are not accessed via the APIs for
               record-based EFs. */
            mmgsdi_status = MMGSDI_EF_INCONSISTENT;
          }
          break;
        default:
          mmgsdi_status = MMGSDI_ERROR;
          break;
      }

      if(MMGSDI_SUCCESS != mmgsdi_status)
      {
        UIM_MSG_ERR_3("File info check failed: file_type 0x%x, offset 0x%x, data_len 0x%x",
                      file_type, offset, *data_len_ptr);
      }

      return mmgsdi_status;
    }
  }

  /* Couldn't get file info from hardcode table or from the attributes cache.
     Hence we must send a SELECT to the card (without attemtping to cache the
     attributes upon getting a response from the card) */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(select_ptr,
                                     sizeof(mmgsdi_get_file_attr_req_type));

  if(select_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  select_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
  select_ptr->request_header.session_id        = session_id;
  select_ptr->request_header.request_type      = MMGSDI_GET_FILE_ATTR_REQ;
  select_ptr->request_header.orig_request_type = MMGSDI_GET_FILE_ATTR_REQ;
  select_ptr->request_header.request_len       =
    uint32toint32(sizeof(mmgsdi_get_file_attr_req_type));
  select_ptr->request_header.payload_len       = 0;
  select_ptr->request_header.slot_id           = slot_id;
  select_ptr->request_header.client_data       = 0;
  select_ptr->request_header.response_cb       = NULL;
  select_ptr->activate_aid                     = FALSE;
  select_ptr->skip_uicc_arr                    = TRUE;

  mmgsdi_memscpy(&select_ptr->access, sizeof(mmgsdi_access_type),
                 &access, sizeof(mmgsdi_access_type));

  switch (protocol)
  {
    case MMGSDI_UICC:
      mmgsdi_status = mmgsdi_uim_uicc_select(select_ptr,
                                             TRUE,
                                             &select_cnf_ptr,
                                             TRUE);
      break;
    case MMGSDI_ICC:
      mmgsdi_status = mmgsdi_uim_icc_select(select_ptr,
                                            TRUE,
                                            &select_cnf_ptr);
      break;
    default:
      mmgsdi_status = MMGSDI_ERROR;
      break;

  }
  MMGSDIUTIL_TMC_MEM_FREE(select_ptr);

  if(select_cnf_ptr == NULL || mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_0("Select of EF failed");
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)select_cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(select_cnf_ptr);
    return MMGSDI_ERROR;
  }

  if(select_cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_status = select_cnf_ptr->response_header.mmgsdi_status;
    UIM_MSG_ERR_1("Invalid select: Unable to proceed with status: 0x%x",
                  select_cnf_ptr->response_header.mmgsdi_status);
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)select_cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(select_cnf_ptr);
    return mmgsdi_status;
  }

  switch(select_cnf_ptr->file_attrib.file_type)
  {
    case MMGSDI_TRANSPARENT_FILE:
      /* If the file is part of cache list then read the entire file from
         card. We do this only for first time when the file is not present
         in the cache. For subsequent partial/complete read requests, the
         contents are directly returned from cache. In case of PRL and
         EPRL we have different reading logic (to read 2 bytes first)
         so skip updating the length if file is PRL or EPRL */
      if(is_cache_not_init && 
         access.file.file_enum != MMGSDI_CSIM_PRL  &&
         access.file.file_enum != MMGSDI_CDMA_PRL  &&
         access.file.file_enum != MMGSDI_CSIM_EPRL &&
         access.file.file_enum != MMGSDI_CDMA_EPRL)
      {
        *data_len_ptr = select_cnf_ptr->file_attrib.file_size;
      }
      else if (select_cnf_ptr->file_attrib.file_size >= *data_len_ptr + offset)
        {
        if (*data_len_ptr == 0)
        {
          *data_len_ptr = select_cnf_ptr->file_attrib.file_size - offset;
        }
      }
      else if ((select_cnf_ptr->file_attrib.file_size < *data_len_ptr + offset) &&
               (select_cnf_ptr->file_attrib.file_size > offset))
      {
        *data_len_ptr = select_cnf_ptr->file_attrib.file_size - offset;
      }
      else
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      if (file_type != MMGSDI_TRANSPARENT_FILE)
      {
        mmgsdi_status = MMGSDI_EF_INCONSISTENT;
      }
      break;
    case MMGSDI_LINEAR_FIXED_FILE:
      if(*data_len_ptr == 0 ||
         *data_len_ptr > select_cnf_ptr->file_attrib.file_info.linear_fixed_file.rec_len)
      {
        *data_len_ptr = select_cnf_ptr->file_attrib.file_info.linear_fixed_file.rec_len;
      }
      else if(select_cnf_ptr->file_attrib.file_info.linear_fixed_file.rec_len != *data_len_ptr)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      if (file_type == MMGSDI_TRANSPARENT_FILE)
      {
        /* Ensure that transparent EFs are not accessed via the APIs for
           record-based EFs. */
        mmgsdi_status = MMGSDI_EF_INCONSISTENT;
      }
      break;
    case MMGSDI_CYCLIC_FILE:
      if(*data_len_ptr == 0 ||
         *data_len_ptr > select_cnf_ptr->file_attrib.file_info.cyclic_file.rec_len)
      {
        *data_len_ptr = select_cnf_ptr->file_attrib.file_info.cyclic_file.rec_len;
      }
      else if(select_cnf_ptr->file_attrib.file_info.cyclic_file.rec_len != *data_len_ptr)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      if (file_type == MMGSDI_TRANSPARENT_FILE)
      {
        /* Ensure that transparent EFs are not accessed via the APIs for
           record-based EFs. */
        mmgsdi_status = MMGSDI_EF_INCONSISTENT;
      }
      break;
    default:
      mmgsdi_status = MMGSDI_ERROR;
      break;
  }

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_2("File info check failed: offset 0x%x, data_len 0x%x",
                  offset, *data_len_ptr);
  }

  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)select_cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(select_cnf_ptr);

  return mmgsdi_status;
} /* mmgsdi_file_check_file_info_and_update_len */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_COMPARE_ACCESS_TYPE

   DESCRIPTION:
     This function compares the two access type parameters and returns true if
     they are the same.

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     boolean

     TRUE:          Access paarmeters are same
     FALSE:         Otherwise

   SIDE EFFECTS:

==========================================================================*/
boolean mmgsdi_file_compare_access_type(
  const mmgsdi_access_type  *access_1_ptr,
  const mmgsdi_access_type  *access_2_ptr
)
{
  boolean are_equal = FALSE;

  if(!access_1_ptr || !access_2_ptr)
  {
    return FALSE;
  }

  if(access_1_ptr->access_method != access_2_ptr->access_method)
  {
    return FALSE;
  }

  switch (access_1_ptr->access_method)
  {
    case MMGSDI_EF_ENUM_ACCESS:
      if(access_1_ptr->file.file_enum == access_2_ptr->file.file_enum)
      {
        are_equal = TRUE;
      }
      break;
    case MMGSDI_DF_ENUM_ACCESS:
      if(access_1_ptr->file.df_enum == access_2_ptr->file.df_enum)
      {
        are_equal = TRUE;
      }
      break;
    case MMGSDI_BY_PATH_ACCESS:
      if (access_1_ptr->file.path_type.path_len > MMGSDI_MAX_PATH_LEN ||
          access_1_ptr->file.path_type.path_len < 0 ||
          access_2_ptr->file.path_type.path_len > MMGSDI_MAX_PATH_LEN ||
          access_2_ptr->file.path_type.path_len < 0)
      {
        break;
      }
      if(access_1_ptr->file.path_type.path_len == access_2_ptr->file.path_type.path_len &&
         memcmp(access_1_ptr->file.path_type.path_buf,
                access_2_ptr->file.path_type.path_buf,
                int32touint32(sizeof(uint16) * access_1_ptr->file.path_type.path_len)) == 0)
      {
        are_equal = TRUE;
      }
      break;
    case MMGSDI_BY_APP_ID_ACCESS:
      if (access_1_ptr->file.app_id.data_len > MMGSDI_MAX_AID_LEN ||
          access_1_ptr->file.app_id.data_len < 0 ||
          access_2_ptr->file.app_id.data_len > MMGSDI_MAX_AID_LEN ||
          access_2_ptr->file.app_id.data_len < 0)
      {
        break;
      }
      if(access_1_ptr->file.app_id.data_len == access_2_ptr->file.app_id.data_len &&
         memcmp(access_1_ptr->file.app_id.data_ptr,
                access_2_ptr->file.app_id.data_ptr,
                int32touint32(access_1_ptr->file.app_id.data_len)) == 0)
      {
        are_equal = TRUE;
      }
      break;
    default:
      break;
  }

  return are_equal;
} /* mmgsdi_file_compare_access_type */


/* ==========================================================================
FUNCTION MMGSDI_FILE_COMPARE_PATH_TYPE

DESCRIPTION
  This function determines if the EF file path is same as one that we already
  cached.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  boolean: TRUE if EF is matched else FALSE

SIDE EFFECTS
  NONE
==========================================================================*/
boolean mmgsdi_file_compare_path_type(
  mmgsdi_path_type   file1,
  mmgsdi_path_type   file2
)
{
  uint8      loop_cnt = 0;

  if(file1.path_len < 1 || file1.path_len > MMGSDI_MAX_PATH_LEN ||
     file1.path_len != file2.path_len)
  {
    return FALSE;
  }
  
  for(loop_cnt = 0; loop_cnt < file1.path_len; loop_cnt++)
  {
    if(file1.path_buf[loop_cnt] != file2.path_buf[loop_cnt])
    {
      /* Mismatch found.*/
      return FALSE;
    }
  }

  /* Both input file paths are same */
  return TRUE;
} /* mmgsdi_file_compare_path_type */