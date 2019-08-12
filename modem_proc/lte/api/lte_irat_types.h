/*!
  @file
  lte_irat_types.h

  @brief
  This file describes the interfaces to LTE Radio resource Controller (RRC_IRAT)

   <!-- Doxygen chokes on the @mainpage vs. @page inside a conditional, but the
       following seems to work.  Basically, make this the mainpage if it
       is the only component being built, otherwise, just make it a page and
       let the upper level component/s @ref it. -->
  @if rrc 
  @mainpage LTE Radio Resource Controller (RRC_IRAT)
  @endif
  @ifnot rrc
  @page LTE Radio Resource Controller (RRC_IRAT)
  @endif

  RRC_IRAT is a sub-module of RRC. This file contains data strucutres and UMIDs
  for messages that RRC will use to communicate with other RATs over 
  message router.

  <h2>High level architecture</h2>
  <img src='../pics/rrc_arch.jpg'>
  
  <h2>API</h2>
 <h3><i>Message Interfaces</i></h3> 
   -- External API to RRC_IRAT is defined in @ref lte_irat_types.h
  
 <h3><i>Function Call Interfaces</i></h3> 
  This module does not have any function call API.

  <b><i> QXDM Log Parser </i></b>
  - @ref QXDM_Parser_DLL

  @ref umids
  @ref all_umids

*/


/*===========================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_irat_types.h#1 $

when       who       what, where, why
--------   ---       ----------------------------------------------------------
03/07/13   mc        Added common structure for LTE --> X RF interface
04/06/11   tmai      Update target cell rscp for making pathloss estimation
12/08/11   tmai      W2L time transfer interface change for HO
09/23/11   np        SRVCC Keys
02/08/11   np        New causes for e1xCSFB failures
11/08/10   np        Define struct for UMTS key for L2W PSHO
10/11/10   np        56bit definition of MEID
09/29/10   np        Define lte_irat_1xcsfb_ho_failure_cause_e causes
09/10/10   np        e1xCSFB Interfaces for HO to 1xCP
06/23/10   np        Consolidate IRAT Tunnel into unified message
06/21/10   np        Add lte_irat_ul_msg_u & lte_irat_ul_ho_prep_tx_req_s
06/17/10   np        IRAT Tunnel Support for 1xCSFB Pre-Reg
03/03/10   np        Removed temp. dedicated_priority_info_type
03/03/10   sk        Added support for rem_validity_timer_is_present field
                     in lte_irat_dedicated_priority_info_s
02/20/10   sk        Added band for geran dedicated priorities
02/10/10   mm        Added a new structure with priority list and validity time
02/08/10   np        Include sys.h & add validity timer to dedicated prio.
02/04/10   mm        Added the common dedicated priority types used by all RATs
02/02/10   vatsac    Initial version

===========================================================================*/

#ifndef LTE_IRAT_TYPES_H
#define LTE_IRAT_TYPES_H
#include <msgr.h>
#include <msgr_types.h>
#include <lte_l1_types.h>
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <sys.h>
#include <sys_v.h>

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief External defines
*/
#define MAX_PRIORITY_LEVELS_PER_RAT 32

#define LTE_IRAT_MAX_EUTRAN_FREQ 64
#define LTE_IRAT_MAX_UTRAN_FREQ 64
#define LTE_IRAT_MAX_GERAN_FREQ 128
#define LTE_IRAT_MAX_CDMA_BAND_CLASS 32

/*! Max size for PLMN Search Detected Frequencies */  
#define LTE_IRAT_MAX_DETECTED_FREQ 64

/*! Max size for Detected CSG Cells */  
#define LTE_IRAT_MAX_CSG_CELLS 64

/*! Max size for Detected CSG Cells */  
#define LTE_IRAT_MAX_MACRO_CELLS 10

/*! Maximum size of IRAT message container TBD */
#define LTE_IRAT_TUNNEL_MAX_MSG_CONTAINER_SIZE (2048)
                            
/*! Maximum size of 1xCSFB Mobility Parameters TBD */
#define LTE_IRAT_TUNNEL_MAX_C2K_MOBILITY_PARAMS_SIZE (2048)

/*! Maximum UMTS key sizes */
#define LTE_IRAT_MAX_CK_LENGTH 16
#define LTE_IRAT_MAX_IK_LENGTH 16
#define LTE_IRAT_MAX_KC_LENGTH 8
/* Key length used for Kc 128 bit */
#define LTE_IRAT_MAX_KC_128_LENGTH 16

#define LTE_IRAT_MEID_MAX_SIZE  7

#define LTE_RRC_CDMA_TUNNEL_MAX_DSM_ARRAY_SIZE 2

/*! Macro for Maximum number of 1x/DO avoidance channels */
#define LTE_RRC_MAX_1X_DO_AVOID_CHANNELS 40 

/*! Maximum number of LTE cells for which timing can be sent for x2L HO */
#define LTE_IRAT_MAX_CELLS_TIMING_TX_DATA 64

/*! @brief Macros for minimum TDD DL EARFN as per 36.101
*/
#define LTE_IRAT_MIN_TDD_DL_EARFN 36000
/*! @brief Macros for maximum TDD DL EARFN as per 36.101
*/
#define LTE_IRAT_MAX_TDD_DL_EARFN 45589

/*Macro which returns if tdd cell else false*/
#define LTE_CELL_IS_TDD( dl_earfcn ) \
    (((dl_earfcn >= LTE_IRAT_MIN_TDD_DL_EARFN) && (dl_earfcn <= LTE_IRAT_MAX_TDD_DL_EARFN))? TRUE : FALSE )\
   
/*! @brief Enumeration for IRAT RAT types
*/
typedef enum
{
  LTE_IRAT_RAT_TYPE_1XRTT, /*!< 0 */
  LTE_IRAT_RAT_TYPE_HRPD, /*!< 1 */
  LTE_IRAT_RAT_TYPE_GERAN, /*!< 2 */
  LTE_IRAT_RAT_TYPE_UTRAN, /*!< 3 */
  LTE_IRAT_RAT_TYPE_MAX
} lte_irat_rat_type_e;

/*! @brief RAT Capabilities
*/

typedef struct
{

  boolean                        wcdma_is_supported;
  sys_band_mask_type             wcdma_band_preference;
  boolean                        tdscdma_is_supported;
  sys_band_mask_type             tdscdma_band_preference;
  boolean                        lte_is_supported;
  sys_lte_band_mask_e_type       lte_band_preference;
  boolean                        geran_is_supported;
  sys_band_mask_type             geran_band_preference;
  boolean                        cdma_1x_is_supported;
  sys_band_mask_type             cdma_1x_band_preference;
  boolean                        cdma_do_is_supported;
  sys_band_mask_type             cdma_do_band_preference;

}lte_irat_capabilities_s ;

/*! @brief EUTRA cell reselection priority info
*/
typedef struct
{
  sys_sband_e_type band;      /*!< Band - used for GERAN */
  uint32 arfcn;    /*!< EARFCN for E-UTRA, UARFCN for UTRA and ARFCN for GSM */
  uint8 priority; /*!< Cell reselection priority */
} lte_irat_reselection_priority_s;

/*! @brief cdma cell reselection priority info
*/
typedef struct
{
  sys_band_class_e_type band_class; /*! CDMA band class */
  uint8 cell_resel_priority; /*! Cell reselection priority */
} lte_irat_cdma_reselection_priority_s;

/*! @brief Data structure for dedicated priority list 
*/
typedef struct
{
  uint32 num_eutra_freq;            /*!< Number of EUTRA frequencies */
  lte_irat_reselection_priority_s eutra_freq_list[LTE_IRAT_MAX_EUTRAN_FREQ]; 

  uint32 num_utra_fdd_freq;         /*!< Number of UTRA-FDD frequencies */
  lte_irat_reselection_priority_s utra_fdd_freq_list[LTE_IRAT_MAX_UTRAN_FREQ];

  uint32 num_utra_tdd_freq;         /*!< Number of UTRA-TDD frequencies */
  lte_irat_reselection_priority_s utra_tdd_freq_list[LTE_IRAT_MAX_UTRAN_FREQ];

  uint32 num_geran_freq;            /*!< Number of GERAN frequencies */
  lte_irat_reselection_priority_s geran_freq_list[LTE_IRAT_MAX_GERAN_FREQ];

  uint32 num_hrdp_band_class;       /*!< Number of HRPD band classes */
  lte_irat_cdma_reselection_priority_s cdma_hrpd_list[LTE_IRAT_MAX_CDMA_BAND_CLASS];

  uint32 num_1x_band_class;         /*!< Number of 1XRTT band classes */
  lte_irat_cdma_reselection_priority_s cdma_1x_list[LTE_IRAT_MAX_CDMA_BAND_CLASS];

} lte_irat_dedicated_priority_list_s;

/*! @brief Data structure for dedicated priority info for all RATs 
    NOTE: 'rem_validity_timer' field has to be a value greater than zero if
    'rem_validity_timer_is_present' has been set to TRUE.
    If 'rem_validity_timer_is_present' has been set to FALSE and the dedicated priorities
    are present, then the target RAT should use the priorities as though they never expire.
    The priorities should be discarded when any conditions mentioned in the target RAT spec
    are met. For example: UE going into connected mode on the target RAT.
*/
typedef struct
{
  lte_irat_dedicated_priority_list_s  priority_list; /*!< Dedicated priority list 
                                                                for all RATs */
  boolean rem_validity_timer_is_present;             /*!< TRUE if the rem_validity_timer 
                                                          field is present, FALSE otherwise */
  
  uint64 rem_validity_timer;      /* !< Remaining Validity Timer value in ms */

} lte_irat_dedicated_priority_info_s;


/*! @brief PLMN Search Detected Frequency
*/
typedef struct
{
  uint16                       frequency; /*!< Frequency in 100KHZ*/
  sys_radio_access_tech_e_type rat;       /*!< RAT type*/
  uint16                       bandwidth; /*!< bandwidth allotted on the
                                            frequency in 100KHz*/
} detected_frequency_type;


/*! @brief PLMN Search Detected Frequency List
*/
typedef struct
{
  uint16                  num_freqs;                        /*!< Num Frequencies present */
  detected_frequency_type freq[LTE_IRAT_MAX_DETECTED_FREQ]; /*!< IRAT Frequencies */
} detected_frequency_list_type;


/*! @brief WCDMA CSG Search Detected Cell
*/
typedef struct
{
  uint16  psc;         /*!< Scrambling code of the resel candidate cell */
  uint16  uarfcn;      /*!< Frequency */
  boolean timing_present; /*!< Whether timing and measurements are present.
                            if FALSE, only the PSC is valid */
  uint32  pn_pos;      /*!< PN position In units of chipx8 Range 0 to 307199 */
  int16   ecno_2x;     /*!< 2x the ecno for the cell */
  int16   rscp;        /*!< The RSCP for the cell */
  int8    q_rx_lev_min_actual; /*!< Actual value = IE value * 2+1. Specifies the minimum required Rx level
                                 in the cell expressedin dBm. Minimum required RX level in the cell.
                                 Defined in 25.304  Values in the range -60..-13*/
  int8    p_max;      /*!< The maximum allowed transmission power on the (uplink) carrier
                        frequency in dBm. Defined in 25.304. Values in the range -50..33 */
  int8    q_qual_min; /*!< Minimum required quality level in the cell (dB). Defined in 25.304
                        Values in the range -24..0*/
} detected_wcdma_csg_cell_type;


/*! @brief LTE CSG Search Detected Cell
*/
typedef struct
{
  uint16  pci;         /*!< Cell Id resel candidate cell */
  lte_earfcn_t  earfcn;      /*!< Frequency */
  int16   rsrp;        /*!< The RSRP for the cell */
  int16   rsrq;        /*!< The RSRQ for the cell */
  int16   rssi;        /*!< The RSSI for the cell */
  int32   q_rxlevmin;  /*!< Actual value = IE value * 2. Specifies the minimum required Rx level
                         in the cell expressedin dBm. Minimum required RX level in the cell.
                         Defined in 36.331  Values in the range -70..-22*/
  int8    p_max;      /*!< The maximum allowed transmission power on the (uplink) carrier
                        frequency in dBm. Defined in 36.331. Values in the range -30..33 */
  int8    q_qualmin;  /*!< Minimum required quality level in the cell (dB). Defined in 36.331
                        Values in the range -34..-3 */
} detected_lte_csg_cell_type;


/*! @brief LTE CSG Search Detected Cell
*/
typedef struct
{
  sys_radio_access_tech_e_type rat;       /*!< RAT type*/
  union {
    detected_wcdma_csg_cell_type wcell;
    detected_lte_csg_cell_type   lcell;
  } rat_u;
} detected_csg_cell_type;


/*! @brief CSG Search Detected Cell List
*/
typedef struct
{
  uint16                  num_cells;                    /*!< Num cells present */
  detected_csg_cell_type  cell[LTE_IRAT_MAX_CSG_CELLS]; /*!< IRAT CSG cells */
} detected_csg_cell_list_type;


/*! @brief Serving Cell Info
*/
typedef struct
{
  sys_radio_access_tech_e_type rat; /*!< RAT type*/
  uint32             frequency;     /*!< (E)ARFCN */
  uint16             phy_cell_id;   /*!< Physical Cell Id */
  uint32             cell_identity; /*!< Global cell identity*/
  sys_plmn_id_s_type plmn;          /*!< PLMN */
  sys_csg_id_type    csg_id;        /*!< CSG ID */
} source_cell_info_type;

typedef source_cell_info_type macro_cell_info_type;

typedef struct
{
  /* Cell id for the W/T cell */
  uint32              cell_id;

  /* PLMN id for the W/T cell */
  sys_plmn_id_s_type  plmn_id;
} serving_cell_global_id_s;

/*! @brief UTRA Cell Info
*/
typedef struct
{
  sys_radio_access_tech_e_type rat;           /*!< RAT type*/
  boolean                      is_rat_active; /*!< If UE is camped on this RAT*/
                                              /*!< boolean will be set to TRUE*/  
  uint16                       frequency;     /*!< Frequency */
  uint16                       phy_cell_id;   /*!< Physical Cell Id (PSC)*/    
  boolean                      is_cell_global_id_present;
  serving_cell_global_id_s     serving_cell_global_id;
} utra_serving_cell_info_type;

/*! @brief Detected Macro Cell List
*/
typedef struct
{
  uint16                  num_cells;                      /*!< Num cells present */
  source_cell_info_type   cell[LTE_IRAT_MAX_MACRO_CELLS]; /*!< Macro cells */
} detected_macro_cell_list_type;


/*! @brief Enum of various failures from 1xCP.
 *         These values allow CM to decide on next steps for the 1xCSFB call.
*/

typedef enum
{
  LTE_IRAT_1XCSFB_HO_FAILURE_MSG_INVAILD,       /*!< Malformed UHDM/ECAM  */
  LTE_IRAT_1XCSFB_HO_FAILURE_MSG_IGNORE,        /*!< Unexpected UHDM/ECAM */ 
  LTE_IRAT_1XCSFB_HO_FAILURE_ACQ_FAIL,          /*!< Acquisition on native 1X failed */ 
  LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_REL_ORDER,/*!< Network initiated call end */
  LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_REORDER,  /*!< Reorder */
  LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_INTERCEPT_ORDER, /*!< Intercept Order */
  LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_NORMAL,       /*!< Network initiated call Release and no reason is given */
  LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_SO_REJ,       /*!< Network initiated call Release because SO is not supported */
  LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_OTASP_SPC_ERR,/*!< Network initiated call because of SPC error in OTASP */
 
  LTE_IRAT_1XCSFB_HO_FAILURE_CAUSE_MAX
} lte_irat_1xcsfb_ho_failure_cause_e;

/*! @brief Enum of various failures from 1xCP.
 *         These values allow CM to decide on next steps for the 1xSRVCC call.
*/

typedef enum
{
  LTE_IRAT_1XSRVCC_HO_FAILURE_MSG_INVAILD,       /*!< Malformed UHDM/ECAM  */
  LTE_IRAT_1XSRVCC_HO_FAILURE_MSG_IGNORE,        /*!< Unexpected UHDM/ECAM */ 
  LTE_IRAT_1XSRVCC_HO_FAILURE_ACQ_FAIL,          /*!< Acquisition on native 1X failed */ 
  LTE_IRAT_1XSRVCC_HO_FAILURE_CALL_REL_REL_ORDER,/*!< Network initiated call end */
  LTE_IRAT_1XSRVCC_HO_FAILURE_CALL_REL_REORDER,  /*!< Reorder */
  LTE_IRAT_1XSRVCC_HO_FAILURE_CALL_REL_INTERCEPT_ORDER, /*!< Intercept Order */
  LTE_IRAT_1XSRVCC_HO_FAILURE_CALL_REL_NORMAL,       /*!< Network initiated call Release and no reason is given */
  LTE_IRAT_1XSRVCC_HO_FAILURE_CALL_REL_SO_REJ,       /*!< Network initiated call Release because SO is not supported */
  LTE_IRAT_1XSRVCC_HO_FAILURE_CALL_REL_OTASP_SPC_ERR,/*!< Network initiated call because of SPC error in OTASP */
 
  LTE_IRAT_1XSRVCC_HO_FAILURE_CAUSE_MAX
} lte_irat_1xsrvcc_ho_failure_cause_e;

/*! @brief Data structure for IRAT e.g. (1x/HDR) message container
*/
typedef struct
{
  uint16 msg_size; /*!< Size of message in bytes */
  uint8  msg_payload[LTE_IRAT_TUNNEL_MAX_MSG_CONTAINER_SIZE]; /*!< Message data */
} lte_irat_tunnel_msg_container_s;

/*! @brief Various IRAT Tunnel UL Messages Requests
*/
typedef enum
{
  /*! ULInformationTransfer RRC Message e.g HDR Pre-Reg etc */
  LTE_IRAT_TUNNEL_UL_MSG_INFO_TRANSFER_REQ,

  /*! CSFBParametersRequestCDMA2000 RRC Message */
  LTE_IRAT_TUNNEL_UL_MSG_CSFB_PARAMS_REQ,
  
  /* ULHandoverPreparationTransfer RRC Message */
  LTE_IRAT_TUNNEL_UL_MSG_UL_HO_PREP_TX_REQ,
  
  LTE_IRAT_TUNNEL_UL_MSG_MAX,

} lte_irat_tunnel_ul_msg_type_e;

/*! @brief Various IRAT Tunnel DL Responses Messages
*/
typedef enum
{
  /*! DLInformationTransfer RRC Message */
  LTE_IRAT_TUNNEL_DL_MSG_DL_INFO_TRANSFER,

  /*! CSFBParametersResponseCDMA2000 RRC Message */
  LTE_IRAT_TUNNEL_DL_MSG_CSFB_PARAMS_RSP,
 
  /*! HandoverFromEUTRAPreparationRequest RRC Message */
  LTE_IRAT_TUNNEL_DL_MSG_HO_FROM_EUTRA_PREP_REQ,

  LTE_IRAT_TUNNEL_DL_MSG_MAX,

} lte_irat_tunnel_dl_msg_type_e;

/*! @brief 1x CSFB Parameters Response
*/
typedef struct
{
  uint32                rand;
  uint32                mobility_params_length;
  uint8                 mobility_params[LTE_IRAT_TUNNEL_MAX_C2K_MOBILITY_PARAMS_SIZE];
} lte_irat_tunnel_dl_msg_csfb_params_rsp_s;

/*! @brief 1x Handover From EUTRA Preparation Request payload.
 *         This message is used to trigger a tunneled preparation 
 *         procedure with 1xRTT to obtain traffic channel resources for e1xCSFB
*/
typedef struct
{
  lte_irat_rat_type_e   type; /*!< IRAT RAT Type */

  boolean               is_rand_present;
  uint32                rand;

  uint32                mobility_params_length;
  uint8                 mobility_params[LTE_IRAT_TUNNEL_MAX_C2K_MOBILITY_PARAMS_SIZE];
} lte_irat_tunnel_dl_msg_ho_from_eutra_prep_req_s;

/*! @brief ULInformationTransfer Request
*/
typedef struct
{
  lte_irat_rat_type_e             type; /*!< IRAT RAT Type */
  lte_irat_tunnel_msg_container_s msg_container; /*!< IRAT message container */
} lte_irat_tunnel_ul_msg_ul_info_tx_req_s;

/*! @brief DLInformationTransfer
*/
typedef struct
{
  lte_irat_rat_type_e             type; /*!< IRAT RAT Type */
  lte_irat_tunnel_msg_container_s msg_container; /*!< IRAT message container */
} lte_irat_tunnel_dl_msg_dl_info_tx_s;

/*! @brief DLInformationTransfer for CDMA 1XRTT and HRPD 
*/
typedef struct
{
  msgr_hdr_s        msg_hdr;                 /*!< Message router header */
  msgr_attach_s     dsm_attach;              /*!< CDMA message container */
} lte_rrc_irat_cdma_tunnel_dl_msg_data_ind_s;

/*! @brief 56bit MEID
*/

typedef struct 
{
   uint32 numbits;
   uint8  data[LTE_IRAT_MEID_MAX_SIZE];
} lte_irat_meid_s;

/*! @brief ULHandoverPreparationTransfer Request
*/
typedef struct
{
  boolean                         is_meid_present;
  lte_irat_meid_s                 meid; /*!< mobile identification number */
 
  lte_irat_rat_type_e             type; /*!< IRAT RAT Type */
  lte_irat_tunnel_msg_container_s msg_container; /*!< IRAT message container */
} lte_irat_tunnel_ul_msg_ul_ho_prep_tx_req_s;

/*! @brief IRAT UL Message Union
*/
typedef struct
{
  /*! This will indicate the UL Message type */
  lte_irat_tunnel_ul_msg_type_e               ul_msg_type;

  union 
  {
    /*! ULInformationTransfer RRC Message */
    lte_irat_tunnel_ul_msg_ul_info_tx_req_s     ul_info_tx;

    /*! CSFBParametersRequestCDMA2000 Does not have a payload */

    /*! ULHandoverPreparationTransfer */
    lte_irat_tunnel_ul_msg_ul_ho_prep_tx_req_s  ul_ho_prep_tx_req;
  } ul_msg_u;
} lte_irat_tunnel_ul_msg_req_s;

/*! @brief IRAT DL Message Union
*/
typedef struct
{
  /*! This will indicate the DL Message type */
  lte_irat_tunnel_dl_msg_type_e           dl_msg_type;

  union
  {
    /*! DLInformationTransfer RRC Message */
    lte_irat_tunnel_dl_msg_dl_info_tx_s      dl_info_tx;

    /*! CSFBParametersResponseCDMA2000 RRC Message */
    lte_irat_tunnel_dl_msg_csfb_params_rsp_s csfb_params_rsp;

    /*! HandoverFromEUTRAPreparationRequest RRC Message */
    lte_irat_tunnel_dl_msg_ho_from_eutra_prep_req_s ho_from_eutra_prep_req;
  } dl_msg_u;
} lte_irat_tunnel_dl_msg_ind_s;

/*! @brief CDMA IRAT DL Message
*/
typedef struct
{
  /*! This will indicate the DL Message type */
  lte_irat_tunnel_dl_msg_type_e  dl_msg_type;

  boolean                        is_rand_present; /*!< Flag to indicate presence of RAND. 
                                                    Only present if DL Msg type is 
                                                    LTE_IRAT_TUNNEL_DL_MSG_CSFB_PARAMS_RSP
                                                    OR 
                                                    LTE_IRAT_TUNNEL_DL_MSG_HO_FROM_EUTRA_PREP_REQ
                                                   */
  uint32                         rand;

}lte_irat_cdma_tunnel_dl_msg_s;

/*! @brief CDMA IRAT DL Message Union using DSM Items
*/
typedef struct
{

  /*! message header type*/
  msgr_hdr_s msg_hdr;

  msgr_attach_s  dsm_attach;    /*!< IRAT message container for dedicated CDMA 2000 Info.
                                     For CSFB Paramter Response and HO from EUTRA Prep Request populate
                                       1. uint32 mobility_params_length;
                                       2. uint8  mobility_params[LTE_IRAT_TUNNEL_MAX_C2K_MOBILITY_PARAMS_SIZE]
                                     For CDMA DL Message, populate
                                       1. DedicatedInfoCDMA2000 
                                */

  lte_irat_cdma_tunnel_dl_msg_s dl_msg_info; /*! < CDMA tunnel DL Msg */


} lte_irat_cdma_tunnel_dl_msg_ind_s;

/*! @brief CDMA IRAT UL Message
*/
typedef struct
{
  lte_irat_tunnel_ul_msg_type_e  ul_msg_type;      /*!< CDMA UL Msg Type  */
  boolean                        is_meid_present;  /*!< Flag to indicate presence of MEID. 
                                                        Only present if UL Msg type is 
                                                        LTE_IRAT_TUNNEL_UL_MSG_UL_HO_PREP_TX_REQ */     
  lte_irat_meid_s                meid;             /*!< mobile identification number */
} lte_irat_cdma_tunnel_ul_msg_s;

/*! @brief CDMA IRAT UL Message Union using DSM Items
*/
typedef struct
{

  msgr_hdr_s     msg_hdr;          /*!< Message router header */
  msgr_attach_s  dsm_attach[LTE_RRC_CDMA_TUNNEL_MAX_DSM_ARRAY_SIZE];    
                                        /*!< IRAT message container for dedicated CDMA 2000 Info 
                                        For CDMA UL Msg, populate
                                          1. DedicatedInfoCDMA2000
                                        For CSFB Param Req, populate
                                          none
                                        For HO Prep TX Req, populate
                                          1. DedicatedInfoCDMA2000
                                   */
  uint32                         trans_id;         /*!< Transaction ID as populated by HDR*/
  boolean                        cnf_is_reqd;      /*!< Is confirmation required as populated by HDR ? */

  lte_irat_cdma_tunnel_ul_msg_s msg; /*! < CDMA tunnel UL Msg */
}lte_irat_cdma_tunnel_ul_msg_req_s;

/*! @brief IRAT DL Message Union
*/
typedef struct
{
  /* GW Ciphering key */
  uint8   ck[LTE_IRAT_MAX_CK_LENGTH];
  /* GW Integrity Key */
  uint8   ik[LTE_IRAT_MAX_IK_LENGTH];
  /* Ciphering key used in L2GW SRVCC */
  uint8   ck_srvcc[LTE_IRAT_MAX_CK_LENGTH];
  /* Integrity key used in L2GW SRVCC  */
  uint8   ik_srvcc[LTE_IRAT_MAX_IK_LENGTH];
  /* Obsoleted field */
  uint8   kc[LTE_IRAT_MAX_KC_LENGTH];
  /* KC 128 bit used for the GEA4/A5 algo in GSM */
  uint8   kc_128bit[LTE_IRAT_MAX_KC_128_LENGTH];
  uint8   kcsn;
} lte_irat_umts_sec_info_s;

/*! @brief Timing Tx info for target HO cell passed by LTE RRC to ML1
*/
typedef struct
{
  /*! Flag to indicate supplemental target cell info is present */
  boolean present;

  /*! The 10ms frame boundary of the cell */
  uint64 cell_timing;

  /*! The CP type of the detected cell. */
  lte_l1_cp_e cp_type;

  /*! RSRP in Q4 or X16 format meaning each unit is 
  1/16th of a dbm. */
  int16 rsrp;

   /*! RSRQ in Q4 or X16 format meaning each unit is 
  1/16th of a dbm. */
  int16 rsrq;

  /*! RSSI in Q4 or X16 format meaning each unit is 
  1/16th of a db. */
  int16 rssi;

  /*! Snapshot of both Rat's timers used for time transfer.
      Note: The source RAT stmr must be enabled until 
      handover confirm is sent */
  lte_l1_lte_stmr_snapshot_s snapshot;
} lte_irat_target_cell_info_s;


/*! @brief Timing Tx info for each cell
*/

typedef struct
{
  /*! The physical cell id of the detected cell */
  uint16 cell_id;

  /*! The earfcn for searching LTE cells on */
  lte_earfcn_t earfcn;

  /*! The 10ms frame boundary of the cell */
  uint64 cell_timing;

  /*! The CP type of the detected cell. */
  lte_l1_cp_e cp_type;

  /*! RSRP in Q4 or X16 format meaning each unit is 
  1/16th of a dbm. */
  int16 rsrp;

   /*! RSRQ in Q4 or X16 format meaning each unit is 
  1/16th of a dbm. */
  int16 rsrq;

  /*! RSSI in Q4 or X16 format meaning each unit is 
  1/16th of a db. */
  int16 rssi;

} lte_irat_cell_timing_info_s;

/*! @brief Timing Tx info for cells passed by src RAT during x2L IRAT HO
*/
typedef struct 
{
  byte number_of_cells;
  lte_irat_cell_timing_info_s cell_timing_info[LTE_IRAT_MAX_CELLS_TIMING_TX_DATA];

  /*! Snapshot of both Rat's timers used for time transfer.
      Note: The source RAT stmr must be enabled until 
      handover confirm is sent */
  lte_l1_lte_stmr_snapshot_s snapshot;
} lte_irat_timing_tx_info_s;

/*! 
  @brief
  Component carrier, CC, specific tune params
*/
typedef struct
{
  /*! Band on which LTE will be tuned */
  uint32 band;

  /*! Channel bandwidth on which LTE will use */
  lte_bandwidth_idx_e  bandwidth;

  /*! Channel on which LTE will be tuned */
  lte_earfcn_t         earfcn;

  /*! Device on which LTE will be tuned */
  uint32               device;

  /*! Diversity device on which LTE will be tuned */
  uint32               div_device;

  /*! LM buffer index for source prx */
  uint32 rxlm_buf_idx_ant0;

  /*! LM buffer index for source drx */
  uint32 rxlm_buf_idx_ant1;

  /*! Chain mask indicating active Rx chains */
  uint32 chain_mask;
} lte_irat_cc_tune_param_s;

/*! 
  @brief
  LTE --> X tuneaway parameters.
 
  This structure defines the interface betweem LTE and all target RATs when
  tuning away from LTE.
*/
typedef struct 
{
  /*Old fields, should be deleted after other parts has been updated*/
  uint32 lte_rxlm_rx_buffer_ant0;
  uint32 lte_rxlm_rx_buffer_ant1;

  /*! LM buffer index for target prx */
  uint32 target_rxlm_buf_idx_ant0;

  /*! LM buffer index for target drx */
  uint32 target_rxlm_buf_idx_ant1;

  /*! Number of CCs in the system */
  uint8 num_carriers;  

  /* Tune params for each CC */
  lte_irat_cc_tune_param_s carrier[LTE_CPHY_MAX_SERVING_CELL];

} lte_irat_l2x_tuneaway_param_s;

#endif /* LTE_IRAT_TYPES_H */

