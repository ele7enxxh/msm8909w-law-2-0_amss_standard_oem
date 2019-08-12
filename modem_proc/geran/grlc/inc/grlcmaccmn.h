/*****************************************************************************
***
*** TITLE
***
***  GPRS LAYER 2 DEFINITIONS COMMON TO RLC AND MAC
***
***
*** DESCRIPTION
***
***  Definitions common to both RLC and MAC operations
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/inc/grlcmaccmn.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 03/15/01    hv     Common RLC & MAC definitions
*** 04/21/04    hv     Added EGPRS MCS definitions and acknack description
*****************************************************************************/

#ifndef INC_GRLCMACCMN_H
#define INC_GRLCMACCMN_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif


#include "gprsdef.h"
#include "gprsdef_g.h"
#include "queue.h"
#include "grlctimer.h"


/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/

/*-----------------------------------------------------------------------------
** Sequence Number Space (SNS) or the transmit workspace is 128 for GPRS (9.0)
**
**-----------------------------------------------------------------------------
*/
#define SNS                       128

/*---------------------------------------------------------------------------
** Window Space (WS) or the RLC transmit/receive window is 64 for GPRS (9.0)
** WS replaces the RLC Window Size (K).
**---------------------------------------------------------------------------
*/
#define WS                        (SNS/2)

/*-----------------------------------------------
** RLC Window Size (9.0).
** The acknack bitmap shall be 1/8 of this value
**-----------------------------------------------
*/
#define RM_RLC_WINDOW_SIZE        WS
#define RM_RBB_ARRAY_LEN          (RM_RLC_WINDOW_SIZE/8)

/*-------------------------------------
** EGPRS Sequence Number Space (E_SNS)
** TS 04.60 9.0
**-------------------------------------
*/
#define E_SNS                     2048

/* Minimum window size for EGPRS Class 10 is 64
*/
#define E_WS_MIN                  64

/* compressed bitmap length maximum of 128 bits given the value of Lc.
** This is stored as 16 octets (max)
*/
#define RM_CRBB_ARRAY_LEN         16

/* uncompressed bitmap length. From TS 04.60 12.3.1, Lu = L-Lc-23
** LuMAX = 255-23-Lc = 232-0 = 232 = 29 octets.
** This is stored as 30 octets (max)
*/
#define RM_URBB_ARRAY_LEN         30

/* Maximum transmit work space for 2 uplink slots (class 10) is 256
** (as per TS 04.60 Annex I) plus a margin.
**
** For Multislot class 12 where the maximum window size is 512 the transmit
** workspace needs to be doubled
*/
#define E_TX_WORKSPACE            (1024)

/* Maximum resceive work space for 4 downlink slots (class 10) is 512
** (as per TS 04.60 Annxe I). No margin required for DL
**
** ! to move this to downlink .. header file
*/
#ifdef  FEATURE_GSM_GPRS_MSC33
  /* MSC34 has max window size of 640. The workspace needs to be greater than
  ** the window size as well as an intergral multiple of ESNS (2048)
  */
  #define E_RX_WORKSPACE           1024
#else
  #define E_RX_WORKSPACE            512
#endif /* FEATURE_GSM_GPRS_MSC33 */
/*-----------------------------------------------------------------------
** Max number of elements in the Acknowledge State Array (9.1.3)
** This array size applies to both the uplink transmit acknowledge state
** array and the downlink receive state array.
**-----------------------------------------------------------------------
*/
#define RM_RLC_TX_WORKSPACE       SNS
#define RM_RLC_RX_WORKSPACE       SNS

/*---------------------------------------------------------------
** BS_CV_MAX is a parameter broadcast in the system information.
** Temporarily defined here as a fixed value.
**---------------------------------------------------------------
*/
#define BS_CV_MAX(gas_id)         (sysinfo[gas_id].bs_cv_max)
#define BS_CV_CONST               15

/*---------------------------
** TLLI field size in octets
**---------------------------
*/
#define RM_TLLI_LEN               (sizeof(uint32)/sizeof(uint8))


/*-------------------------------
** PFI and E field size in octets
**-------------------------------
*/
#define RM_PFI_E_LEN               (sizeof(uint8)/sizeof(uint8))

/*-------------------------------
** PFI valid values 0:127 (7bits)
**-------------------------------
*/
#define RM_PFI_INVALID             0xff


/*===========================================================================
*                 RLC/MAC Block Header Fields Definitions
**===========================================================================
*/

/*----------------------------------------------------
** Medium access mode
** This is granted by the network and not controlled
** or requested by MS
**----------------------------------------------------
*/
#define RM_DYNAMIC_ALLOC                  0
#define RM_EXT_DYNAMIC_ALLOC              1
#define RM_FIXED_ALLOC_NOT_HALF_DUPLEX    2
#define RM_FIXED_ALLOC_HALF_DUPLEX        3

/*-----------------------------
** Contention resolution type
**-----------------------------
*/
#define RM_CONTENTION_UNRESOLVED    0
#define RM_CONTENTION_RESOLVED      1

/*-----------------------------
** cell resel status type
**-----------------------------
*/
typedef enum
{
  RESEL_STARTED,
  RESEL_SUCCESSFUL,
  RESEL_FAILED

} rm_cell_resel_status_t;

/*----------------------
** tbf type definition
**----------------------
*/
#define RM_OPEN_ENDED_TBF           0   /* open-ended tbf */
#define RM_CLOSE_ENDED_TBF          1   /* close-ended tbf */

/*----------------------------
** Channel Coding Scheme Type
**----------------------------
*/
#define RM_CS_1   0
#define RM_CS_2   1
#define RM_CS_3   2
#define RM_CS_4   3

/*------------------------------------------------
** EGPRS Modulation and Coding Scheme Description
** TS 04.60 12.10d
**------------------------------------------------
*/
#define RM_MCS_1      0
#define RM_MCS_2      1
#define RM_MCS_3      2
#define RM_MCS_4      3
#define RM_MCS_5      4
#define RM_MCS_6      5
#define RM_MCS_7      6
#define RM_MCS_8      7
#define RM_MCS_9      8
#define RM_MCS_5_7    9
#define RM_MCS_6_9   10
#define RM_MCS_UNDEF 11

/* Header type literal values
*/
#define HDR_TYPE_1  0
#define HDR_TYPE_2  1
#define HDR_TYPE_3  2

/*--------------------------------------------------------------
** Final Allocation Indicator from the network. When the value
** is RM_ALLOC_FINAL, MS must request a new tbf to send the
** outstanding blocks
**--------------------------------------------------------------
*/
#define RM_ALLOC_NOT_FINAL        0
#define RM_ALLOC_FINAL            1


/*------------------------------------------------------------------
** TLLI Indicator bit.
** Indicates the presence of an optional TLLI field within the RLC
** uplink data block
**------------------------------------------------------------------
*/
#define RM_TLLI_NOT_PRESENT         0
#define RM_TLLI_PRESENT             1


/*------------------------------------------------------------------
** PFI Indicator bit.
** Indicates the presence of an optional PFI field within the RLC
** uplink data block
**------------------------------------------------------------------
*/
#define RM_PFI_NOT_PRESENT         0
#define RM_PFI_PRESENT             1

/*--------------------------------------------------------------
** Stall Indicator field.
** Indicates whether or not the transmit window can advance or
** stalled.
**--------------------------------------------------------------
*/
#define RM_TX_NOT_STALLED           0
#define RM_TX_STALLED               1


/*---------------------------------------
** Supplementary/Polling bit definition
**---------------------------------------
*/
#define RM_SP_RRBP_NOT_VALID      0
#define RM_SP_RRBP_VALID          1


/*-------------------------------------------------------------
** USF granularity field in Packet Channel Assignment 11.2.29
**-------------------------------------------------------------
*/
#define RM_USF_GRANULARITY_1_BLOCK      0
#define RM_USF_GRANULARITY_4_BLOCKS     1


/*-------------------------------------------------------------------
** tlli block channel coding command specified by the network, such
** that RLC should use the coding scheme 1 to encode data blocks
** with TLLI field or the normal commanded coding scheme.
**-------------------------------------------------------------------
*/
#define RM_TLLI_USE_CS_1            0
#define RM_TLLI_USE_CS_COMMANDED    1

/* May use these for EGPRS or RM_TLLI_USE_CS_1/MCS_COMMANDED defined above for GPRS
*/
#define RM_TLLI_USE_MCS_1           0
#define RM_TLLI_USE_MCS_COMMANDED   1

/*--------------------------------------------------------------
** Resent Block Bit field.
** Indicates whether or not all the data blocks within the EGPRS
** radio block are bring transmitted for the first time.
**--------------------------------------------------------------
*/
#define RM_RSB_1ST_TX               0
#define RM_RSB_RE_TX                1

/*-------------------------------------------------
** RLC/MAC uplink and downlink release conditions
**-------------------------------------------------
*/
typedef enum
{
  /* Release of uplink tbf 8.1.1.4
  ** 8.7.1
  */
  REL_UL_ABNORMAL_WITHOUT_RETRY,

  /* 8.7.2 */
  REL_UL_ABNORMAL_WITH_ACCESS_RETRY,

  /* 8.7.3 */
  REL_UL_ABNORMAL_WITH_SYS_INFO,

  /* RLC to indicate to MAC of abnormal release with cell reselection
  ** when N3102 expires
  */
  REL_UL_ABNORMAL_CELL_RESEL,

  /* 9.3.2.3, 9.3.3.3 */
  REL_UL_NORMAL,

  /* normal dl tbf release (to idle) */
  REL_DL_NORMAL,

  REL_DL_ABNORMAL_WITHOUT_RETRY,

  /* After MAC sends a PARej msg to RLC with Wait Indication on, RLC waits
  ** for this signal to know when MAC is in idle and ready to accept TBF
  ** establishment etc.. again from RLC.
  */
  REL_UL_MAC_IDLE

} rm_tbf_release_t;


/*----------------------------------------------
** Final Block Indicator (FBI) field definition
**----------------------------------------------
*/
#define RM_FBI_NOT_FINAL            0  /* block is not the last in current TBF */
#define RM_FBI_FINAL                1  /* block is the last in current TBF */

/*--------------------------
** More (M) bit definition
**--------------------------
*/
#define RM_MORE_NO_PDU_FOLLOWS      0
#define RM_MORE_PDU_FOLLOWS         1

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/*=============================================================================
**               Internal and External MAC Header Definitions
**                        for UL/DL Data/Control Block
**
** -------------------------------------------------
** |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |
** -------------------------------------------------
** | Payload   |   RRBP    | S/P |       USF       |  DL RLC data block
** -------------------------------------------------
** | Payload   |    Countdown Value    | SI  |  R  |  UL RLC data block
** -------------------------------------------------
** | Payload   |   RRBP    | S/P |       USF       |  DL RLC/MAC control block
** -------------------------------------------------
** | Payload   |               spare         |  R  |  UL RLC/MAC control block
** -------------------------------------------------
**
**=============================================================================
*/

#ifndef PERLUTF

/* PERLUTF does not support bit structuring.
*/

/* MAC header for downlink RLC data block without the Payload Type
*/
typedef struct
{
  uint8 rrbp:2;
  #define mac_hdr_dl_data_t_rrbp_len    2  /* 2 bits from bit 6 */

  uint8 sp:1;
  #define mac_hdr_dl_data_t_sp_len      1  /* 1 bit from bit 4 */

  uint8 usf:3;
  #define mac_hdr_dl_data_t_usf_len     3  /* 3 bits from bit 3 */

} mac_hdr_dl_data_t;

/* MAC header for uplink RLC data block without the Payload Type
*/
typedef struct
{
  uint8 cv:4;
  #define mac_hdr_ul_data_t_cv_len      4  /* 4 bits from bit 6 */

  uint8 si:1;
  #define mac_hdr_ul_data_t_si_len      1  /* 1 bit from bit 2 */

  uint8 retry:1;
  #define mac_hdr_ul_data_t_retry_len   1  /* 1 bit from bit 1 */

} mac_hdr_ul_data_t;

/* MAC header for downlink RLC control block without the Payload Type
**
** (same as uplink RLC data block)
*/
typedef struct
{
  uint8 rrbp:2;
  #define mac_hdr_dl_ctrl_t_rrbp_len    2  /* 2 bits from bit 6 */

  uint8 sp:1;
  #define mac_hdr_dl_ctrl_t_sp_len      1  /* 1 bit from bit 4 */

  uint8 usf:3;
  #define mac_hdr_dl_ctrl_t_usf_len     3  /* 3 bits from bit 3 */

} mac_hdr_dl_ctrl_t;

/* MAC header for uplink RLC control block without the Payload Type
*/
typedef struct
{
  uint8 spare:5;
  #define mac_hdr_ul_ctrl_t_spare_len   5  /* 5 bits from bit 6 */

  uint8 retry:1;
  #define mac_hdr_ul_ctrl_t_retry_len   1  /* 1 bit from bit 1 */

} mac_hdr_ul_ctrl_t;


/* General MAC header definition
*/
typedef struct
{
  /* Payload Type field coding, 2-bit field.
  ** This field, combines with the direction (UL/DL), determines the
  ** data type of an RLC/MAC block
  */

  /* ul/dl rlc data block
  */
  #define  MAC_PAYLOAD_DATA                     0x00

  /* dl ctrl block without optional octets
  */
  #define  MAC_PAYLOAD_CTRL_NO_OPT_OCTETS       0x01

  /* ul ctrl block has no optional octets by default
  */
  #define  MAC_PAYLOAD_CTRL_1_OPT_OCTET         0x02

  /* ul: reserved */
  #define MAC_PAYLOAD_RESERVED                  0x03

  /* 2 bits from bit 8 */
  uint8   payload:2;
  #define  mac_hdr_t_payload_len    2

  union
  {
    mac_hdr_ul_data_t  ul_data;
    mac_hdr_ul_ctrl_t  ul_ctrl;
    mac_hdr_dl_data_t  dl_data;
    mac_hdr_dl_ctrl_t  dl_ctrl;
  } type;

  /* 6 bits from bit 6 */
  #define  mac_hdr_t_type_len       6

} mac_hdr_t;

#else  /* PERLUTF */
#error code not present
#endif /* PERLUTF */
/*===========================================================================
*                         RLC/MAC Block Definition
*
*
*  MAC header | RLC header | RLC data unit        | spare
*             <------------------------------------------>
*                        RLC data block
*
* <----------------------RLC/MAC Block------------------->
*
*  MAC header | Ctrl hdr |             RLC/MAC signalling
*             <------------------------------------------>
*                        RLC/MAC control block
*
*===========================================================================*/


/*-------------------------------------------------------
** RLC data block size in octets without the spare bits
** and MAC header according to coding scheme.
**-------------------------------------------------------
*/
extern const uint8 GRLC_DATA_BLOCK_LEN[];

/*-------------------------------------------------------
** EGPRS RLC data block size in octets without the header.
**-------------------------------------------------------
*/
extern const uint8 E_GRLC_DATA_BLOCK_LEN[];

/*-----------------------------------------------------------
** Enumeration of Final Acknowledgement Status in uplink and
** downlink operations
**-----------------------------------------------------------
*/
#define RM_ACKNACK_FAI_TBF_NOT_COMPLETE         0
#define RM_ACKNACK_FAI_TBF_COMPLETE             1


/*==========================================================================
**                         Ack/Nack Description
**
** This information element is part of the Packet ul & dl ack/nack messages
** GSM 04.60 v 6.9.0 Rel 97 12.3
**==========================================================================
*/
typedef struct
{
  /* Final ack indication (FAI).
  *  Indicates whether the entire TBF is being acknowledged
  *  1 bit field
  */
  uint8 fai;
  #define  rm_acknack_desc_t_fai_len      1

  /* Starting sequence number (SSN), 7 bit field
  *  Carries the current value of V(R)
  */
  uint8 ssn;
  #define  rm_acknack_desc_t_ssn_len      7

  /* Receive block bitmap (RBB). Thebitmap is indexed relative to SSN
  *  BSN = (SSN - bit_number) mod SNS, for bit_number = 1 to WS
  *  The bitmap is indexed relative to SSN
  */
  #define RM_ACKNACK_RBB_POS_ACK          1   /* positive ack */
  uint8 rbb[RM_RBB_ARRAY_LEN];                 /* received block bitmap, WS bit field */
  #define  rm_acknack_desc_t_rbb_len      WS

} rm_acknack_desc_t;

/*===========================================================================
*                      Channel Request Description
*
*  4.60 v6.9.0 Rel 97 12.7
*============================================================================*/

/*------------------------------------------
** RLC mode, 1 bit field.
** Indicates the mode of the requested TBF.
**------------------------------------------
*/
#define GRLC_MODE_ACK        0
#define GRLC_MODE_UNACK      1


/*------------------------------------------------------------------
** LLC PDU type, 1 bit field.
** Indicates the type of the first LLC PDU to be transmitted
** over the requested uplink TBF
**
** Ref: llc_pdu_type member variable from channel_req_desc_t struct
**------------------------------------------------------------------
*/
#define RM_LLC_PDU_TYPE_ACK_SACK        0
#define RM_LLC_PDU_TYPE_NON_ACK_SACK    1


/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/


#endif /* INC_GRLCMACCMN_H */

/*** EOF: don't remove! ***/
