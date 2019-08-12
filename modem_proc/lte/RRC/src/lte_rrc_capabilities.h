/*!
  @file lte_rrc_capabilities.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_capabilities.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/08/10   mm      Added macro definitions for each FGI bit
08/05/10   mm     Added an API that returns if inter-freq operations are supported
12/15/09   dd      Initial Version
===========================================================================*/

#ifndef LTE_RRC_CAPABILITIES_H
#define LTE_RRC_CAPABILITIES_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <__lte_rrc_capabilities.h>
#include "lte_rrc_int_msg.h"
#include "lte_as.h"
#include <lte_rrc_osys_asn1.h>
#include <lte_rrc_osys_asn1util.h>
#include <lte_pdcp_msg.h>




/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define LTE_RRC_CAP_FGI_EFS_FILENAME "/nv/item_files/modem/lte/rrc/cap/fgi"
#define LTE_RRC_CAP_FGI_REL9_EFS_FILENAME "/nv/item_files/modem/lte/rrc/cap/fgi_rel9"
#define LTE_RRC_CAP_FGI_TDD_EFS_FILENAME "/nv/item_files/modem/lte/rrc/cap/fgi_tdd"
#define LTE_RRC_CAP_FGI_TDD_REL9_EFS_FILENAME "/nv/item_files/modem/lte/rrc/cap/fgi_tdd_rel9"
#define LTE_RRC_CAP_R10_FGI_EFS_FILENAME "/nv/item_files/modem/lte/rrc/cap/fgi_r10"
#define LTE_RRC_CAP_R10_FGI_TDD_EFS_FILENAME "/nv/item_files/modem/lte/rrc/cap/fgi_tdd_rel10"

#define LTE_RRC_CAP_FGI_BIT_1    1  /*! FGI bit position for the following
                                      - Intra-subframe frequency hopping 
                                        for PUSCH scheduled by UL grant
                                      - DCI format 3a (TPC commands for PUCCH 
                                        and PUSCH with single bit power adjustments)
                                      - Multi-user MIMO for PDSCH
                                      - Aperiodic CQI/PMI/RI reporting on PUSCH: 
                                        Mode 2-0 - UE selected subband CQI without PMI
                                      - Aperiodic CQI/PMI/RI reporting on PUSCH: 
                                        Mode 2-2 - UE selected subband CQI with multiple PMI
                                    */
#define LTE_RRC_CAP_FGI_BIT_2    2  /*! FGI bit position for the following 
                                      - Simultaneous CQI and ACK/NACK on PUCCH, 
                                        i.e. PUCCH format 2a and 2b
                                      - Absolute TPC command for PUSCH
                                      - Resource allocation type 1 for PDSCH
                                      - Periodic CQI/PMI/RI reporting on PUCCH: 
                                        Mode 2-0 - UE selected subband CQI without PMI
                                      - Periodic CQI/PMI/RI reporting on PUCCH: 
                                        Mode 2-1 - UE selected subband CQI with single PMI
                                    */
#define LTE_RRC_CAP_FGI_BIT_3    3  /*! FGI bit position for SPS, TTI bundling, 
                                        5-bit RLC SN and 7-bit PDCP SN */ 
#define LTE_RRC_CAP_FGI_BIT_4    4  /*! FGI bit position for shortDRX cycle */
#define LTE_RRC_CAP_FGI_BIT_5    5  /*! FGI bit position for LongDRX cycle and 
                                        DRX command MAC control element */
#define LTE_RRC_CAP_FGI_BIT_6    6  /*! FGI bit position for Prioritized bit rate */
#define LTE_RRC_CAP_FGI_BIT_7    7  /*! FGI bit position for RLC UM */
#define LTE_RRC_CAP_FGI_BIT_8    8  /*! FGI bit position for PS HO to UTRA CELL_DCH */
#define LTE_RRC_CAP_FGI_BIT_9    9  /*! FGI bit position for dedicated HO to GERAN */
#define LTE_RRC_CAP_FGI_BIT_10   10  /*! FGI bit position for CCO/NACC to GERAN idle */
#define LTE_RRC_CAP_FGI_BIT_11   11  /*! FGI bit position for active HO to 1X */
#define LTE_RRC_CAP_FGI_BIT_12   12  /*! FGI bit position for active HO to DO */
#define LTE_RRC_CAP_FGI_BIT_13   13  /*! FGI bit position for inter-freq HO */
#define LTE_RRC_CAP_FGI_BIT_14   14  /*! FGI bit position for reporting event A4/A5 */
#define LTE_RRC_CAP_FGI_BIT_15   15  /*! FGI bit position for reporting event B1 */
#define LTE_RRC_CAP_FGI_BIT_16   16  
                                     /*! FGI bit position for periodic meas 
                                         reporting for non-ANR */
#define LTE_RRC_CAP_FGI_BIT_17   17  /*! FGI bit position for intra-freq SON/ANR */
#define LTE_RRC_CAP_FGI_BIT_18   18  /*! FGI bit position for inter-freq SON/ANR */
#define LTE_RRC_CAP_FGI_BIT_19   19  /*! FGI bit position for inter-RAT SON/ANR */
#define LTE_RRC_CAP_FGI_BIT_20   20  /*! FGI bit position for supporting the following
                                         If bit number 7 is set to '0':
                                         - SRB1 and SRB2 for DCCH + 8x AM DRB

                                         If bit number 7 is set to '1':
                                         - SRB1 and SRB2 for DCCH + 8x AM DRB
                                         - SRB1 and SRB2 for DCCH + 5x AM DRB + 3x UM DRB
                                         */
#define LTE_RRC_CAP_FGI_BIT_21   21  /*! FGI bit position for the following
                                         - Predefined intra- and inter-subframe 
                                           frequency hopping for PUSCH with N_sb > 1
                                         - Predefined inter-subframe frequency 
                                           hopping for PUSCH with N_sb > 1
 */
#define LTE_RRC_CAP_FGI_BIT_22   22  /*! FGI bit position for reporting event B2 for UTRA */
#define LTE_RRC_CAP_FGI_BIT_23   23  /*! FGI bit position for reporting event B2 for GERAN */
#define LTE_RRC_CAP_FGI_BIT_24   24  /*! FGI bit position for reporting event B2 for 1X */
#define LTE_RRC_CAP_FGI_BIT_25   25
                                     /*! FGI bit position for inter-freq 
                                         measurements in EUTRA connected mode */
#define LTE_RRC_CAP_FGI_BIT_26   26 /*! FGI bit position for reporting event B2 for DO */
#define LTE_RRC_CAP_FGI_BIT_27   27 /*! FGI bit position for CS HO to UTRA CELL_DCH */
#define LTE_RRC_CAP_FGI_BIT_28   28 /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_29   29 /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_30   30 /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_31   31 /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_32   32 /*! FGI bit position undefined */

#define LTE_RRC_CAP_FGI_BIT_33   33  /*! ANR for UTRAN */
#define LTE_RRC_CAP_FGI_BIT_34   34  /*! ANR for GERAN */
#define LTE_RRC_CAP_FGI_BIT_35   35  /*! ANR for 1xRTT */ 
#define LTE_RRC_CAP_FGI_BIT_36   36  /*! ANR for HRPD */
#define LTE_RRC_CAP_FGI_BIT_37   37  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_38   38  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_39   39  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_40   40  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_41   41  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_42   42  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_43   43  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_44   44  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_45   45  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_46   46  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_47   47  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_48   48  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_49   49  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_50   50  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_51   51  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_52   52  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_53   53  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_54   54  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_55   55  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_56   56  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_57   57  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_58   58  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_59   59  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_60   60  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_61   61  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_62   62  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_63   63  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_64   64  /*! FGI bit position undefined */


/*Rel-10 FGI's*/
#define LTE_RRC_CAP_FGI_BIT_101    101      /*! bit 101 , - DMRS with OCC (orthogonal cover code) 
                                                                            and SGH (sequence group hopping) disabling*/
#define LTE_RRC_CAP_FGI_BIT_102    102  /*! bit 102, Trigger type 1 SRS (aperiodic SRS) transmission 
                                                                          (Up to X ports)  NOTE: X = number of supported layers on given band */
#define LTE_RRC_CAP_FGI_BIT_103    103  /*! bit 103,PDSCH transmission mode 9 when up to 4 CSI reference signal ports are configured */ 
#define LTE_RRC_CAP_FGI_BIT_104    104  /*! bit 104, PDSCH transmission mode 9 for TDD when 8 CSI reference signal ports are configured */
#define LTE_RRC_CAP_FGI_BIT_105    105  /*! bit 105,  Periodic CQI/PMI/RI reporting on PUCCH: Mode 2-0 - UE selected subband CQI without PMI, 
                                                                  when PDSCH transmission mode 9 is configured
                                                            -  - Periodic CQI/PMI/RI reporting on PUCCH: Mode 2-1 - UE selected subband CQI with single PMI, when 
                                                                 PDSCH transmission mode 9 and up to 4 CSI reference signal ports are configured
                                                              */
#define LTE_RRC_CAP_FGI_BIT_106    106  /*! bit 106  Periodic CQI/PMI/RI/PTI reporting on PUCCH: Mode 2-1 - UE selected subband CQI with
                                                                   single PMI, when PDSCH transmission mode 9 and 8 CSI reference signal ports are configured*/
#define LTE_RRC_CAP_FGI_BIT_107    107  /*! bit 107   Aperiodic CQI/PMI/RI reporting on PUSCH: Mode 2-0 - UE selected subband CQI without PMI, 
                                                      when PDSCH transmission mode 9 is configured- Aperiodic CQI/PMI/RI reporting on PUSCH: Mode 2-2 - 
                                                      UE selected subband CQI with multiple  PMI, when PDSCH transmission mode 9 and up to 4 CSI reference signal ports are configured*/
#define LTE_RRC_CAP_FGI_BIT_108    108  /*! bit 108 , - Aperiodic CQI/PMI/RI reporting on PUSCH: Mode 2-2 - UE selected subband CQI 
                                                                   with multiple PMI, when PDSCH transmission mode 9 and 8 CSI reference signal ports are configured*/
#define LTE_RRC_CAP_FGI_BIT_109    109  /*! bit 109  Periodic CQI/PMI/RI reporting on PUCCH Mode 1-1, submode 1*/
#define LTE_RRC_CAP_FGI_BIT_110    110  /*! bit 110  Periodic CQI/PMI/RI reporting on PUCCH Mode 1-1, submode 2*/
#define LTE_RRC_CAP_FGI_BIT_111    111  /*! bit 111 Measurement reporting trigger Event A6*/
#define LTE_RRC_CAP_FGI_BIT_112    112  /*! bit 112 SCell addition within the Handover to EUTRA procedure*/
#define LTE_RRC_CAP_FGI_BIT_113    113  /*! bit 113  Trigger type 0 SRS (periodic SRS) transmission on X Serving Cells
                                                                      NOTE: X = number of supported component carriers in a given band combination*/
#define LTE_RRC_CAP_FGI_BIT_114    114  /*! bit 114  Reporting of both UTRA CPICH RSCP and Ec/N0 in a Measurement Report*/
#define LTE_RRC_CAP_FGI_BIT_115    115  /*! bit 115  time domain ICIC RLM/RRM measurement subframe restriction for the serving cell
                                                                         time domain ICIC RRM measurement subframe restriction for neighbour cells
                                                                        time domain ICIC CSI measurement subframe restriction*/
#define LTE_RRC_CAP_FGI_BIT_116    116  /* bit 116  Relative transmit phase continuity for spatial multiplexing in UL*/
#define LTE_RRC_CAP_FGI_BIT_117    117  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_118    118  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_119    119  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_120    120  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_121    121  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_122    122  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_123    123  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_124    124  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_125    125  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_126    126  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_127    127  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_128    128  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_129    129  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_130    130  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_131    131  /*! FGI bit position undefined */
#define LTE_RRC_CAP_FGI_BIT_132    132  /*! FGI bit position undefined */

/* CA bandwidth*/
#define LTE_RRC_CAP_CA_BANDWIDTH_MASK_1_4 0x01
#define LTE_RRC_CAP_CA_BANDWIDTH_MASK_3 0x02
#define LTE_RRC_CAP_CA_BANDWIDTH_MASK_5 0x04
#define LTE_RRC_CAP_CA_BANDWIDTH_MASK_10 0x08
#define LTE_RRC_CAP_CA_BANDWIDTH_MASK_15 0x10
#define LTE_RRC_CAP_CA_BANDWIDTH_MASK_20 0x20

/* EFS bitmask*/
#define LTE_RRC_CAP_EFS_REREAD_LTE_CAT          0x01
#define LTE_RRC_CAP_EFS_REREAD_CA_DISABLE       0x02
#define LTE_RRC_CAP_EFS_REREAD_R10_FGI_EFS      0x40
#define LTE_RRC_CAP_EFS_REREAD_R10_FGI_TDD_EFS  0x80
/*<! Length of BCS
     As of today UE supports SET 0 to SET4 and hence 5 bits 
       is enough to encode BCS*/
#define LTE_RRC_ASN_BCS_LENGTH         5  

/*CA band combination set*/
#define LTE_RRC_CAP_BAND_COMBO_SET_0    (1 << (LTE_RRC_ASN_BCS_LENGTH - 1))
#define LTE_RRC_CAP_BAND_COMBO_SET_1    (1 << (LTE_RRC_ASN_BCS_LENGTH - 2))
#define LTE_RRC_CAP_BAND_COMBO_SET_2    (1 << (LTE_RRC_ASN_BCS_LENGTH - 3))
#define LTE_RRC_CAP_BAND_COMBO_SET_3    (1 << (LTE_RRC_ASN_BCS_LENGTH - 4))
#define LTE_RRC_CAP_BAND_COMBO_SET_4    (1 << (LTE_RRC_ASN_BCS_LENGTH - 5))

/*CA bandwidth class*/
#define LTE_RRC_CAP_CA_BAND_CLASS_MASK 1
#define LTE_RRC_CAP_CA_BAND_CLASS_A_MASK (1<<lte_rrc_osys_CA_BandwidthClass_r10_a)
#define LTE_RRC_CAP_CA_BAND_CLASS_B_MASK (1<<lte_rrc_osys_CA_BandwidthClass_r10_b)
#define LTE_RRC_CAP_CA_BAND_CLASS_C_MASK (1<<lte_rrc_osys_CA_BandwidthClass_r10_c)
#define LTE_RRC_CAP_CA_BAND_CLASS_D_MASK (1<<lte_rrc_osys_CA_BandwidthClass_r10_d)
#define LTE_RRC_CAP_CA_BAND_CLASS_E_MASK (1<<lte_rrc_osys_CA_BandwidthClass_r10_e)
#define LTE_RRC_CAP_CA_BAND_CLASS_F_MASK (1<<lte_rrc_osys_CA_BandwidthClass_r10_f)
#define LTE_RRC_CAP_CA_MIN_GUARD_BANDWIDTH 14.4

/* Rel 11 Capabilities */

#define LTE_RRC_CAP_R11_BIT_1  1 /*! pdcp-SN-Extension-r11*/
#define LTE_RRC_CAP_R11_BIT_2  2 /*! supportRohcContextContinue-r11*/
#define LTE_RRC_CAP_R11_BIT_3  3 /*! crs-InterfHandl-r11*/
#define LTE_RRC_CAP_R11_BIT_4  4 /*! dl-CoMP-1CSIproc-r11*/
#define LTE_RRC_CAP_R11_BIT_5  5 /*! dl-CoMP-multiCSIproc-r11*/
#define LTE_RRC_CAP_R11_BIT_6  6 /*! ePDCCH-r11*/
#define LTE_RRC_CAP_R11_BIT_7  7 /*! multiACK-CSIreporting-r11*/
#define LTE_RRC_CAP_R11_BIT_8  8 /*! ss-CCh-InterfHandl-r11*/
#define LTE_RRC_CAP_R11_BIT_9  9 /*! tdd-SpecialSubframe-r11 */
#define LTE_RRC_CAP_R11_BIT_10  10 /*! txDiv-PUCCH1b-ChSelect-r11*/
#define LTE_RRC_CAP_R11_BIT_11  11 /*! ul-CoMP-r11*/
#define LTE_RRC_CAP_R11_BIT_12  12 /*! multipleTimingAdvance-r11*/
#define LTE_RRC_CAP_R11_BIT_13  13 /*! simultaneousRx-Tx-r11*/
#define LTE_RRC_CAP_R11_BIT_14  14 /*! supportedCSI-Proc-r11*/
#define LTE_RRC_CAP_R11_BIT_15  15 /*! rsrqMeasWideband-r11*/
#define LTE_RRC_CAP_R11_BIT_16  16 /*! ue-Rx-TxTimeDiffMeasurementsForMDT-r11*/
#define LTE_RRC_CAP_R11_BIT_17  17 /*! cdma2000-NW-Sharing-r11*/
#define LTE_RRC_CAP_R11_BIT_18  18 /*! inDeviceCoexInd-r11*/
#define LTE_RRC_CAP_R11_BIT_19  19 /*! powerPrefInd-r11*/
#define LTE_RRC_CAP_R11_BIT_20  20 /*! standaloneGNSS-Location-r10*/
#define LTE_RRC_CAP_R11_BIT_21  21 /*! simultaneousAckNackAndCQI-Format3-r11*/
#define LTE_RRC_CAP_R11_BIT_22  22 /*! idc-Config-r11 */
#define LTE_RRC_CAP_R11_BIT_23  23 /*! DRX-Config-v1130 */
#define LTE_RRC_CAP_R11_BIT_24  24 /*! obtainLocationConfig-r11 */
#define LTE_RRC_CAP_R11_BIT_25  25 /*! LocationInfo-r10 */
#define LTE_RRC_CAP_R11_BIT_26  26 /*! CAP R11 bit position undefined */
#define LTE_RRC_CAP_R11_BIT_27  27 /*! CAP R11 bit position undefined */
#define LTE_RRC_CAP_R11_BIT_28  28 /*! CAP R11 bit position undefined */
#define LTE_RRC_CAP_R11_BIT_29  29 /*! CAP R11 bit position undefined */
#define LTE_RRC_CAP_R11_BIT_30  30 /*! CAP R11 bit position undefined */
#define LTE_RRC_CAP_R11_BIT_31  31 /*! CAP R11 bit position undefined */
#define LTE_RRC_CAP_R11_BIT_32  32 /*! CAP R11 bit position undefined */

#ifdef FEATURE_LTE_ELS_ENABLED
/* ELS Capabilities */
#define LTE_RRC_CAP_ELS_BIT_1    0x1 /*! PDCPAggregationLWLAN*/
#define LTE_RRC_CAP_ELS_BIT_2    0x2 /*! UDCCapabilityInformation*/
#define LTE_RRC_CAP_ELS_BIT_3    0x4 /*! UDCCapabilityInformation*/
#define LTE_RRC_CAP_ELS_BIT_4    0x8 /*! TBD*/
#define LTE_RRC_CAP_ELS_BIT_5    0x10 /*! TBD*/
#define LTE_RRC_CAP_ELS_BIT_6    0x20 /*! TBD*/
#define LTE_RRC_CAP_ELS_BIT_7    0x40 /*! TBD*/
#define LTE_RRC_CAP_ELS_BIT_8    0x80 /*! TBD*/
#define LTE_RRC_CAP_ELS_BIT_9    0x100 /*! TBD*/
#define LTE_RRC_CAP_ELS_BIT_10   0x200 /*! TBD*/
#define LTE_RRC_CAP_ELS_BIT_11   0x400 /*! TBD*/
#define LTE_RRC_CAP_ELS_BIT_12   0x800 /*! TBD*/
#define LTE_RRC_CAP_ELS_BIT_13   0x1000 /*! TBD*/
#define LTE_RRC_CAP_ELS_BIT_14   0x2000 /*! TBD*/
#define LTE_RRC_CAP_ELS_BIT_15   0x4000 /*! TBD*/
#define LTE_RRC_CAP_ELS_BIT_16   0x8000 /*! TBD*/

/* Send ELS Capabilities ULM*/
#define LTE_RRC_CAP_SEND_ELS_CAP_ULM_BIT_SPEC    1 /*! If Legacy CAP was requested in ELS ULM*/
#define LTE_RRC_CAP_SEND_ELS_CAP_ULM_BIT_ELS     2 /*! If ELS CAP ULM was requested*/
#endif
/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*Returns the HRPD rx config*/
extern uint8 lte_rrc_cap_hrpd_rx_config
(
 void
);

/*Returns the 1x rx config*/
extern uint8 lte_rrc_cap_1x_rx_config
(
 void
);

/* Returns TRUE if inter-freq operations are supported, FALSE otherwise */
extern boolean lte_rrc_cap_inter_freq_is_supported
(
  void
);

/*===========================================================================

  FUNCTION: lte_rrc_cap_init 

===========================================================================*/
/*!
    @brief
    Allocates and initializes dynamic memory
    @return
    void

*/
/*=========================================================================*/

extern void lte_rrc_cap_init(void);

/*===========================================================================

  FUNCTION: lte_rrc_cap_deinit 

===========================================================================*/
/*!
    @brief
    Deallocates  dynamic memory
    @return
    void

*/
/*=========================================================================*/

extern void lte_rrc_cap_deinit(void);

/*===========================================================================

  FUNCTION:  lte_rrc_cap_get_fgi_bit

===========================================================================*/
/*!
    @brief
    Get FGI bit position

    @return
    void

*/
/*=========================================================================*/


extern uint8 lte_rrc_cap_get_fgi_bit
(
  uint8 indicator_bit_number
);

/*===========================================================================

  FUNCTION:  lte_rrc_cap_get_fgi_bit_by_band_freq

===========================================================================*/
/*!
    @brief
    Get FGI bit position

    @return
    uint8

*/
/*=========================================================================*/
extern uint8 lte_rrc_cap_get_fgi_bit_by_band_freq
(
  uint8 indicator_bit,
  sys_lte_band_mask_e_type lte_band_pref
);

/*===========================================================================

  FUNCTION:  lte_rrc_cap_get_r11_cap_bit

===========================================================================*/
/*!
    @brief
    Get Rel 11 Cap bit supported info

    @return
    boolean

*/
/*=========================================================================*/


extern boolean lte_rrc_cap_get_r11_cap_bit
(
  uint8 r11_bit
);

/*===========================================================================

  FUNCTION:  lte_rrc_cap_get_irat_mask

===========================================================================*/
/*!
    @brief
    Gives the 32 bit mask for IRAT 
    
    @return
    uint32

*/

/*=========================================================================*/
extern uint32 lte_rrc_cap_get_irat_mask
(
  lte_irat_capabilities_s *rat_capabilities_ptr
);
/*===========================================================================

  FUNCTION:  lte_rrc_cap_get_tdscdma_mask

===========================================================================*/
/*!
    @brief
    Gives the 32 bit mask for TDSCDMA 
    
    @return
    uint32

*/

/*=========================================================================*/
extern uint32 lte_rrc_cap_get_tdscdma_mask
(
  lte_irat_capabilities_s *rat_capabilities_ptr
);

/*===========================================================================

  FUNCTION:  lte_rrc_cap_get_fdd_tdd_support_mask

===========================================================================*/
/*!
    @brief
    Gives the 32 bit mask for FDD and TDD bands support

    @return
    uint32

*/
/*=========================================================================*/
uint32 lte_rrc_cap_get_fdd_tdd_support_mask
(
  lte_irat_capabilities_s *rat_capabilities_ptr
);

/*===========================================================================

  FUNCTION:  lte_rrc_cap_get_tdd_mask

===========================================================================*/
/*!
    @brief
    Gives the 32 bit mask for TDD

    @return
    uint32
    
*/
/*=========================================================================*/
extern uint32 lte_rrc_cap_get_tdd_mask
(
  boolean is_tdd_preferred_over_fdd
);

/*===========================================================================

  FUNCTION:  lte_rrc_cap_get_feature_group_indicators_word

===========================================================================*/
/*!
    @brief
    Constructs a 32 bit word with feature group indicator bits
    
    @return
    uint32

*/
/*=========================================================================*/
extern uint32 lte_rrc_cap_get_feature_group_indicators_word(void);

/*===========================================================================

  FUNCTION:  lte_rrc_cap_get_irat_mask_rel9

===========================================================================*/
/*!
    @brief
    Gives the 32 bit mask for IRAT 
    
    @return
    uint32

*/

/*=========================================================================*/
extern uint32 lte_rrc_cap_get_irat_mask_rel9
(
  lte_irat_capabilities_s *rat_capabilities_ptr
);

/*===========================================================================

  FUNCTION:  lte_rrc_cap_get_tdd_mask_rel9

===========================================================================*/
/*!
    @brief
    Gives the 32 bit mask for TDD for rel9 FGI

    @return
    uint32
    
*/
/*=========================================================================*/
extern uint32 lte_rrc_cap_get_tdd_mask_rel9
(
  boolean is_tdd_preferred_over_fdd
);

/*===========================================================================

  FUNCTION:  lte_rrc_cap_get_feature_group_indicators_word_rel9

===========================================================================*/
/*!
    @brief
    Constructs a 32 bit word with rel9 feature group indicator bits
    
    @return
    uint32

*/
/*=========================================================================*/
extern uint32 lte_rrc_cap_get_feature_group_indicators_word_rel9(void);

/*===========================================================================

  FUNCTION:  lte_rrc_cap_diff_fdd_tdd_fgi_is_valid

===========================================================================*/
/*!
    @brief
    Returns TRUE if FDD, TDD FGI are to be set separately
    
    @return
    boolean

*/
/*=========================================================================*/
extern boolean lte_rrc_cap_diff_fdd_tdd_fgi_is_valid
(
  sys_lte_band_mask_e_type lte_band_preference
);

/*===========================================================================

  TRANSITION FUNCTION:  lte_rrc_cap_is_detach_required

===========================================================================*/
/*!
    @brief
    To verify if detach is needed or not on mode change

    @detail
    Called upon sending nas_service_ind to set the detach_reqd field

    @return
    boolean; TRUE is FDD and TDD FGI are to be set separately or
    if cap enquiry is already done on camped mode

*/
/*=========================================================================*/
extern boolean lte_rrc_cap_is_detach_required
(
  sys_lte_band_mask_e_type lte_band_preference
);

/*===========================================================================

  TRANSITION FUNCTION:  lte_rrc_cap_get_rel9_anr_fgi_bit

===========================================================================*/
/*!
    @brief
    To get the correct FGI to be used to verify the ANR support of a RAT

    @return
    uint8; The FGI bit that is applicable for ANR support based on the EFS setting

*/
/*=========================================================================*/
extern uint8 lte_rrc_cap_get_rel9_anr_fgi_bit
(
  uint8 rel9_anr_fgi_bit
);

/*===========================================================================

  FUNCTION:  lte_rrc_cap_set_change_fgi_on_the_fly

===========================================================================*/
/*!
    @brief
     this function will set can_fgi_change_onthe_fly which will control CMAPI FGI change window
    @detail
    Called upon 
    1. mode LMP
    2. SIM invalid.
    3. Cap enq

    @return
    void
*/
/*=========================================================================*/
void lte_rrc_cap_set_change_fgi_on_the_fly
(
  boolean status
);

extern boolean lte_rrc_cap_is_band_combo_valid
(
  sys_sband_lte_e_type camp_band,
  uint8 num_cc, /* NUmber of Configured cells */
  const sys_sband_lte_e_type *cell_band, /* Configured cell bands */
  const lte_bandwidth_e *cell_bw, /* Configured cell bandwith */
  const uint8 *cell_bw_class, /* Configured cell bandwith class*/
  boolean disable_bw_validation /* Disable all BW related validations */
);

extern uint8 lte_rrc_cap_intra_band_ca_validations_disabled
(
  void
);

/*===========================================================================

  FUNCTION:  lte_rrc_cap_inter_freq_gap_less_meas_enabled

===========================================================================*/
/*!
    @brief
    Is inter freq gap less meas enabled via efs or not

    @return
    Is inter freq gap less meas enabled via efs or not

*/
/*=========================================================================*/
extern uint8 lte_rrc_cap_inter_freq_gap_less_meas_enabled
(
  sys_sband_lte_e_type pcell_band, 
  sys_sband_lte_e_type scell_band
);

boolean lte_rrc_is_rel_10_ca_enabled
(
  void
);

uint8 lte_rrc_max_cc_supported
(
  void
);

boolean lte_rrc_cap_ecsfb_is_supported
(
  void
);

boolean lte_rrc_cap_is_mdt_supported
(
  void
);
boolean lte_rrc_cap_utra_proximity_ind_supported
(
  void
);

boolean lte_rrc_cap_eutra_proximity_ind_supported
(
  void
);

boolean lte_rrc_cap_is_feature_supported
(
  lte_rrc_cap_ies_e cap_ie
);
boolean lte_rrc_cap_get_si_req_ho_eutra
(
  lte_rrc_div_duplex_e serv_cell_mode
);
#ifdef FEATURE_LTE_ELS_ENABLED
uint8 lte_rrc_cap_get_els_cap_tag
(
);
uint8 lte_rrc_cap_get_std_cap_tag
(
);
boolean lte_rrc_cap_get_els_cap
(
  uint8 els_bit,
  void* els_cap
);
#endif
boolean lte_rrc_cap_get_si_req_ho_utra
(
  lte_rrc_div_duplex_e serv_cell_mode
);

boolean lte_rrc_cap_transmit_antenna_selection_is_supported
(
  boolean tdd_cell
);

extern boolean lte_rrc_cap_mfbi_is_supported
(
  boolean is_tdd
);
boolean lte_rrc_cap_rach_report_is_supported
(
  void
);
void lte_rrc_cap_get_ue_cap_info
(
  lte_ue_cap_info_s *cap_info_ptr,
  sys_lte_band_mask_e_type lte_band_pref
);

extern boolean lte_rrc_cap_ssf9_is_supported
(
  void
);

#endif /* LTE_RRC_CAPABILITIES_H */
