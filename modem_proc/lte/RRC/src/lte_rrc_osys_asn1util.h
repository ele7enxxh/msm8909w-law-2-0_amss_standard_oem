/*!
  @file
  lte_rrc_osys_asn1util.h
 
  @file
  Macro definitions of ASN1 utilities for encoding and decoding ASN1 messages
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_osys_asn1util.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/17/10   np       Update logging for Rel 9.3
06/07/10   aramani  Fixed macros, added comments and adhered to coding guide  
06/01/10   aramani  Initial Version
===========================================================================*/

#ifndef LTE_RRC_OSYS_ASN1UTIL_H
#define LTE_RRC_OSYS_ASN1UTIL_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "lte_variation.h"
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <comdef.h>
#include "rtxCommon.h"
#include "rtxPrintStream.h"
#include "lte.h"
#include "asn1per.h"
#include "IxErrno.h"
#include "lte_rrc_diag.h"
#include "lte_rrc_asn1utilcommoni.h"
#include "lte_rrc_osys_asn1.h"
#include "lte_rrc_osys_asn1MemFree.h"
#include "lte_rrc_osys_asn1Dec.h"
#include "lte_rrc_osys_asn1Enc.h"
#ifdef FEATURE_LTE_ELS_ENABLED
#include "lte_rrc_osys_els_asn1.h"
#include "lte_rrc_osys_els_asn1MemFree.h"
#include "lte_rrc_osys_els_asn1Dec.h"
#include "lte_rrc_osys_els_asn1Enc.h"
#endif
#include "lte_rrc_mem_util.h"
#include "fs_lib.h"
#include "fs_sys_types.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*! @brief max string size per line
*/
#define 	LTE_RRC_OSYS_ASN1_MAX_LINE_LENGTH 38

/*! @brief OSYS temp buffer size
*/
#define 	LTE_RRC_OSYS_ASN1_TEMP_BUF_LENGTH 1024

/*! @brief OSYS PDU Types */
#define          lte_rrc_osys_LocationInfo_ellipsoid_Point_r10_PDU 22
#define          lte_rrc_osys_LocationInfo_ellipsoidPointWithUncertaintyCircle_r11_PDU  26
#define          lte_rrc_osys_LocationInfo_ellipsoidPointWithUncertaintyEllipse_r11_PDU  27
#define          lte_rrc_osys_LocationInfo_ellipsoidPointWithAltitude_r10_PDU  23
#define          lte_rrc_osys_LocationInfo_ellipsoidPointWithAltitudeAndUncertaintyEllipsoid_r11_PDU  24

#define          lte_rrc_osys_HorizontalVelocity_PDU  28


#define          lte_rrc_osys_BCCH_BCH_Message_PDU 1
#define          lte_rrc_osys_BCCH_DL_SCH_Message_PDU 2
#define          lte_rrc_osys_MCCH_Message_PDU 3
#define          lte_rrc_osys_PCCH_Message_PDU 4
#define          lte_rrc_osys_DL_CCCH_Message_PDU 5
#define          lte_rrc_osys_DL_DCCH_Message_PDU 6
#define          lte_rrc_osys_UL_CCCH_Message_PDU 7
#define          lte_rrc_osys_UL_DCCH_Message_PDU 8
#define          lte_rrc_osys_VarShortMAC_Input_PDU 34
#define          lte_rrc_osys_SystemInformationBlockType1_PDU 13
#define          lte_rrc_osys_SystemInformationBlockType1_v8h0_IEs_PDU 14

#define          lte_rrc_osys_SystemInformationBlockType2_v8h0_IEs_PDU 18
#define          lte_rrc_osys_SystemInformationBlockType5_v8h0_IEs_PDU 19
#define          lte_rrc_osys_SystemInformationBlockType6_v8h0_IEs_PDU 20
#define          lte_rrc_osys_UE_EUTRA_Capability_PDU 32
#define          lte_rrc_osys_UE_EUTRA_Capability_v9a0_IEs_PDU 33
#define          lte_rrc_osys_LogMeasInfo_r10_PDU  41
#ifdef FEATURE_LTE_ELS_ENABLED
// Ordering needs to be compatible with QXDM/APEX
#define          lte_rrc_osys_ELS_SystemInformationBlockType1_PDU 36
#define          lte_rrc_osys_ELS_DL_DCCH_Message_PDU 39
#define          lte_rrc_osys_ELS_UL_DCCH_Message_PDU 40
#define          ELS_BCCH_DL_SCH_Message   lte_rrc_osys_ELS_SystemInformationBlockType1_PDU
#define          ELS_DL_DCCH_Message       lte_rrc_osys_ELS_DL_DCCH_Message_PDU
#define          ELS_UL_DCCH_Message       lte_rrc_osys_ELS_UL_DCCH_Message_PDU
#endif




#define     BCCH_DL_SCH_Message   lte_rrc_osys_BCCH_DL_SCH_Message_PDU
#define     MCCH_Message          lte_rrc_osys_MCCH_Message_PDU
#define     PCCH_Message          lte_rrc_osys_PCCH_Message_PDU
#define     DL_CCCH_Message       lte_rrc_osys_DL_CCCH_Message_PDU
#define     DL_DCCH_Message       lte_rrc_osys_DL_DCCH_Message_PDU
#define     UL_CCCH_Message       lte_rrc_osys_UL_CCCH_Message_PDU
#define     UL_DCCH_Message       lte_rrc_osys_UL_DCCH_Message_PDU
#define     UE_EUTRA_Capability   lte_rrc_osys_UE_EUTRA_Capability_PDU
#define     VarShortMAC_Input     lte_rrc_osys_VarShortMAC_Input_PDU
#define     LogMeas_Info          lte_rrc_osys_LogMeasInfo_r10_PDU
#define     LocationInfo_ellipsoid_Point_r10_PDU  lte_rrc_osys_LocationInfo_ellipsoid_Point_r10_PDU
#define     LocationInfo_ellipsoidPointWithAltitude_r10_PDU  lte_rrc_osys_LocationInfo_ellipsoidPointWithAltitude_r10_PDU
#define     LocationInfo_ellipsoidPointWithUncertaintyCircle_r11_PDU  lte_rrc_osys_LocationInfo_ellipsoidPointWithUncertaintyCircle_r11_PDU
#define     LocationInfo_ellipsoidPointWithUncertaintyEllipse_r11_PDU  lte_rrc_osys_LocationInfo_ellipsoidPointWithUncertaintyEllipse_r11_PDU
#define     LocationInfo_ellipsoidPointWithAltitudeAndUncertaintyEllipsoid_r11_PDU  lte_rrc_osys_LocationInfo_ellipsoidPointWithAltitudeAndUncertaintyEllipsoid_r11_PDU
#define     HorizontalVelocity_PDU  lte_rrc_osys_HorizontalVelocity_PDU

/*MACRO definitions for RRC-defined MEM_FREE functions*/
#define lte_rrc_asn1_osys_IE_free(ptr) lte_rrc_asn1_osys_IE_free_func(ptr); ptr = NULL;
#define lte_rrc_osys_asn1_free_buf(ptr) lte_rrc_osys_asn1_free_buf_func(ptr); ptr = NULL;
#define lte_rrc_osys_asn1_free_pdu(ptr,number) lte_rrc_osys_asn1_free_pdu_func(ptr,number); ptr = NULL;


/*! @brief Enumeration used to indicate which memory heap to use
*/
typedef enum
{
  LTE_RRC_OSYS_MEM_HEAP_MODEM,  /*!< MODEM HEAP */
  LTE_RRC_OSYS_MEM_HEAP_RRC,    /*!< RRC heap or RRC small heap*/
  LTE_RRC_OSYS_MEM_HEAP_SIB,    /*!< SIB heap */
  LTE_RRC_OSYS_MEM_HEAP_MAX      /*!< Invalid */
} lte_rrc_osys_mem_heap_e;



/*! @brief This structure holds the Static part of private data
*/
typedef struct
{
  uint8 placeholder;

#ifdef T_QDSP6
  /*! ASN1 encode/decode/free profiling information */
  boolean asn1_profiling_enabled;
  unsigned long long cycle_count_start;
  unsigned long long cycle_count_end;
  unsigned long long total_cycles;
  unsigned long long micro_secs;
#endif

} lte_rrc_osys_asn1util_static_data_s;

/*! @brief This structure holds the Dynamic part of private data
*/
typedef struct
{
  /*! Run-time context structure.  This structure is a container structure 
      that holds all working variables involved in encoding or decoding a
      message
  */
  OSCTXT lte_rrc_asn1_ctxt;

  /*! @brief Used to get ASN.1 encoding status in case of encoding failure
  */
  lte_rrc_encode_info_s enc_info;

  /*! @brief Used to get ASN.1 decoding status in case of decoding failure
  */
  lte_rrc_decode_info_s dec_info;

  /*! @brief Tell which heap to use for mallocs
  */
  lte_rrc_osys_mem_heap_e mem_heap_to_use;

} lte_rrc_osys_asn1util_dynamic_data_s;

/*! @brief ASN1Util Private data sturctured definition
*/
typedef struct
{
  /*!< Static part of private data */
  lte_rrc_osys_asn1util_static_data_s *sd_ptr;
  /*!< Dynamic part of private data */
  lte_rrc_osys_asn1util_dynamic_data_s *dd_ptr;
}lte_rrc_osys_asn1util_s;

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

extern lte_rrc_osys_asn1util_s *osys_priv_ptr;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern void lte_rrc_osys_init(void);
extern void lte_rrc_osys_free_context(void);
extern void *lte_rrc_asn1_osys_sib_malloc(size_t size);
extern void *lte_rrc_asn1_osys_realloc(void *ptr, size_t size);
extern void lte_rrc_asn1_osys_free(void *ptr);
extern void lte_rrc_osys_asn1_free_buf_func(void *buf);
extern void *lte_rrc_asn1_osys_malloc(size_t size);
extern uint64 lte_rrc_osys_asn1_bitstring_to_int 
(
  size_t max_bytes,           /*!< Maximum number of bytes that are available 
                                   to hold result - use sizeof(uint32), 
                                   sizeof(uint16), etc */
  ASN1BitStr32 *bitstring_ptr  /*!< Pointer to the Objective Systems bitstring*/
);

uint32 lte_rrc_osys_asn1_meas_subframe_pattern_octet_to_uint32
(
  OSOCTET data[],             /*!< Octet array */
  int start,                  /*!< Start index of array*/
  int end 					  /*!< End index of array*/	
);
extern uint64 lte_rrc_osys_asn1_dynamic_bitstring_to_int
(
  size_t max_bytes,           /*!< Maximum number of bytes that are available 
                                   to hold result - use sizeof(uint32), 
                                   sizeof(uint16), etc */
  ASN1DynBitStr *bitstring_ptr  /*!< Pointer to the Objective Systems bitstring*/
);

extern uint64 lte_rrc_osys_asn1_synchronousSystemTime_to_int 
(
  size_t max_bytes,           /*!< Maximum number of bytes that are available 
                                   to hold result - use sizeof(uint32), 
                                   sizeof(uint16), etc */
  lte_rrc_osys_SystemTimeInfoCDMA2000_cdma_SystemTime_synchronousSystemTime *bitstring_ptr  /*!< Pointer to the Objective Systems bitstring*/
);
extern uint64 lte_rrc_osys_asn1_asynchronousSystemTime_to_int 
(
  size_t max_bytes,           /*!< Maximum number of bytes that are available 
                                   to hold result - use sizeof(uint32), 
                                   sizeof(uint16), etc */
  lte_rrc_osys_SystemTimeInfoCDMA2000_cdma_SystemTime_asynchronousSystemTime *bitstring_ptr  /*!< Pointer to the Objective Systems bitstring*/
);
extern uint64 lte_rrc_osys_asn1_variableBitMapOfARFCNs_to_int
(
  size_t max_bytes,           /*!< Maximum number of bytes that are available 
                                   to hold result - use sizeof(uint32), 
                                   sizeof(uint16), etc */
  lte_rrc_osys_CarrierFreqsGERAN_followingARFCNs_variableBitMapOfARFCNs *bitstring_ptr  /*!< Pointer to the Objective Systems bitstring*/
);
extern lte_errno_e lte_rrc_osys_asn1_int_to_bitstring
(
  uint64 int_value,           /*!< Integer value to be converted into Objective 
                                   Systems bitstring */
  ASN1BitStr32 *bitstring_ptr /*!< Pointer to the Objective Systems bitstring - 
                                  length field should be filled up correctly or 
                                  else API yields a bad result */
);
extern void * lte_rrc_asn1_osys_IE_realloc
(
  void *ptr,    /*!< Pointer to memory location to reallocate */
  size_t size   /*!< Size in bytes to be allocated */
);
extern void lte_rrc_asn1_osys_IE_free_func
(
  void *ptr    /*!< Pointer to memory location to be freed */
);

extern unsigned long long lte_rrc_asn1_profile_read_pcycles(void);

#ifdef T_QDSP6																  

#define LTE_RRC_OSYS_ASN1_PROFILING_INFO_INIT								  \
do {																		  \
  if (osys_priv_ptr->sd_ptr->asn1_profiling_enabled > 0) {					  \
	osys_priv_ptr->sd_ptr->cycle_count_start = 0;							  \
	osys_priv_ptr->sd_ptr->cycle_count_end = 0;								  \
	osys_priv_ptr->sd_ptr->total_cycles = 0;								  \
	osys_priv_ptr->sd_ptr->micro_secs = 0;									  \
  }																			  \
} while(0)		

#define LTE_RRC_OSYS_ASN1_PROFILING_START									  \
do {																		  \
  if (osys_priv_ptr->sd_ptr->asn1_profiling_enabled > 0) {					  \
    osys_priv_ptr->sd_ptr->cycle_count_start = lte_rrc_asn1_profile_read_pcycles();	\
  }																			  \
} while(0)		

#define LTE_RRC_OSYS_ASN1_PROFILING_END										  \
do {																		  \
  if (osys_priv_ptr->sd_ptr->asn1_profiling_enabled > 0) {					  \
    osys_priv_ptr->sd_ptr->cycle_count_end = lte_rrc_asn1_profile_read_pcycles(); \
	osys_priv_ptr->sd_ptr->total_cycles = (osys_priv_ptr->sd_ptr->cycle_count_end - osys_priv_ptr->sd_ptr->cycle_count_start) / 6; \
	osys_priv_ptr->sd_ptr->micro_secs = osys_priv_ptr->sd_ptr->total_cycles / 100; \
  }																			  \
} while(0)		

#define LTE_RRC_OSYS_ASN1_ENCODE_PROFILING_PRINT_STATS						  \
  LTE_RRC_MSG_2_MED("OSYS encode total cycles %d, micro sec %d.",	  \
							 osys_priv_ptr->sd_ptr->total_cycles, osys_priv_ptr->sd_ptr->micro_secs);

#define LTE_RRC_OSYS_ASN1_DECODE_PROFILING_PRINT_STATS						  \
  LTE_RRC_MSG_2_MED("OSYS decode total cycles %d, micro sec %d.",	  \
							 osys_priv_ptr->sd_ptr->total_cycles, osys_priv_ptr->sd_ptr->micro_secs);

#define LTE_RRC_OSYS_ASN1_FREE_PROFILING_PRINT_STATS						  \
  LTE_RRC_MSG_2_HIGH("OSYS free total cycles %d, micro sec %d.",	  \
							 osys_priv_ptr->sd_ptr->total_cycles, osys_priv_ptr->sd_ptr->micro_secs);

#else
#define LTE_RRC_OSYS_ASN1_PROFILING_INFO_INIT				/* nothing */						  
#define LTE_RRC_OSYS_ASN1_PROFILING_START					/* nothing */						  
#define LTE_RRC_OSYS_ASN1_PROFILING_END						/* nothing */						  
#define LTE_RRC_OSYS_ASN1_ENCODE_PROFILING_PRINT_STATS		/* nothing */						  
#define LTE_RRC_OSYS_ASN1_DECODE_PROFILING_PRINT_STATS		/* nothing */						  
#define LTE_RRC_OSYS_ASN1_FREE_PROFILING_PRINT_STATS		/* nothing */						  
#endif /* End of #ifdef T_QDSP6*/

extern void* lte_rrc_osys_asn1_decode_pdu(void *encoded_pdu_ptr,                         
                                          size_t encoded_pdu_size,
                                          int pdu_num,
                                          int *decode_status_ptr);

extern int lte_rrc_osys_asn1_encode_pdu(void *unencoded_pdu_ptr,
                                        lte_rrc_pdu_buf_s *pdu_buf_ptr,
                                        int pdu_num);

extern int lte_rrc_osys_asn1_encode_pdu_with_buf(void *unencoded_pdu_ptr,
                                                 lte_rrc_pdu_buf_s *pdu_buf_ptr,
                                                 int pdu_num);

extern void lte_rrc_osys_asn1_free_pdu_func(void *pdu_buf_ptr, int pdu_num);

extern void* lte_rrc_asn1_osys_IE_malloc(size_t size);

extern int lte_rrc_osys_compare_encoded_pdus
(
  void *encoded_pdu_ptr_1,                         
  uint32 encoded_pdu_size_1,
  void *encoded_pdu_ptr_2,                         
  uint32 encoded_pdu_size_2,
  int pdu_num
);

extern void lte_rrc_osys_asn1_copy_pdu_func
(
  void *pdu_src_buf_ptr, 
  void *pdu_dest_buf_ptr, int pdu_num
);
/* Leaving the macros for now for test code */
#ifdef TEST_FRAMEWORK
          #error code not present
#endif /* TEST_FRAMEWORK */

#ifdef __cplusplus
}
#endif

#endif

